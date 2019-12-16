/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

/*
 * iexpr.c
 *
 * routies to take an enode list and generate icode
 */
#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "assert.h"
#include "ppPragma.h"

/*
 *      this module contains all of the code generation routines
 *      for evaluating expressions and conditions.
 */
extern COMPILER_PARAMS cparams;
extern ARCH_ASM* chosenAssembler;
extern int retlab;
extern int nextLabel;
extern SYMBOL* declclass;
extern TYPE stdchar;
extern QUAD *intermed_head, *intermed_tail;
extern int lineno;
extern SYMBOL* inlinesp_list[];
extern int inlinesp_count;
extern BLOCK* current;
extern BLOCKLIST* blocktail;
extern int catchLevel;
extern EXPRESSION* xcexp;
extern int consIndex;
extern int inlinesym_count;
extern EXPRESSION* inlinesym_thisptr[MAX_INLINE_NESTING];
extern TYPE stdpointer;
extern TYPE stdint;
extern int fastcallAlias;
extern bool setjmp_used;
extern bool functionHasAssembly;
extern std::unordered_map<IMODE*, IMODE*> loadHash;
extern CASTTEMPHASH* castHash[DAGSIZE];
extern SimpleExpression* objectArray_exp;
extern std::vector<SimpleSymbol*> typedefs;

SimpleSymbol* baseThisPtr;

int calling_inline;

IMODE* inlinereturnap;
IMODE* structret_imode;

SYMBOL* inlinesp_list[MAX_INLINE_NESTING];
int inlinesp_count;

static DAGLIST* name_value_hash[DAGSIZE];
static LIST* incdecList;
static LIST* incdecListLast;
static int push_nesting;
static EXPRESSION* this_bound;
static int inline_level;
static int stackblockOfs;
IMODE* gen_expr(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_void(EXPRESSION* node, SYMBOL* funcsp);
IMODE* gen_relat(EXPRESSION* node, SYMBOL* funcsp);
void truejp(EXPRESSION* node, SYMBOL* funcsp, int label);
void falsejp(EXPRESSION* node, SYMBOL* funcsp, int label);

void iexpr_init(void)
{
    stackblockOfs = 0;
    calling_inline = 0;
    inlinesp_count = 0;
    push_nesting = 0;
    this_bound = 0;
    inline_level = 0;
}
void iexpr_func_init(void)
{
    memset(name_value_hash, 0, sizeof(DAGLIST*) * DAGSIZE);
    loadHash.clear();
    memset(castHash, 0, sizeof(castHash));
    incdecList = nullptr;
}

void DumpIncDec(SYMBOL* funcsp)
{
    LIST* l = incdecList;
    incdecList = nullptr;
    incdecListLast = incdecList;
    while (l)
    {
        gen_void((EXPRESSION*)l->data, funcsp);
        l = l->next;
    }
}
void DumpLogicalDestructors(EXPRESSION* node, SYMBOL* funcsp)
{
    LIST* listitem = node->destructors;
    while (listitem)
    {
        gen_void((EXPRESSION*)listitem->data, funcsp);
        listitem = listitem->next;
    }
}
IMODE* LookupExpression(enum i_ops op, int size, IMODE* left, IMODE* right)
{
    IMODE* ap = nullptr;
    QUAD head;
    memset(&head, 0, sizeof(head));
    head.dc.left = left;
    head.dc.right = right;
    head.dc.opcode = op;
    if (!left->bits && (!right || !right->bits))
        ap = (IMODE*)LookupNVHash((UBYTE*)&head, DAGCOMPARE, name_value_hash);
    if (!ap)
    {
        bool vol = false;
        bool rest = false;
        int pragmas = 0;
        ap = tempreg(size, 0);
        gen_icode(op, ap, left, right);
        if (!left->offset->unionoffset && (!right || !right->offset->unionoffset))
            if (!left->bits && (!right || !right->bits))
                ReplaceHash((QUAD*)ap, (UBYTE*)intermed_tail, DAGCOMPARE, name_value_hash);
        if (left->offset)
            if (right && right->offset)
            {
                vol = left->vol | right->vol;
                rest = left->restricted & right->restricted;
                pragmas = left->offset->pragmas & right->offset->pragmas;
            }
            else
            {
                vol = left->vol;
                rest = left->restricted;
            }
        else if (right && right->offset)
        {
            vol = right->vol;
            rest = right->restricted;
        }
        else
        {
            vol = rest = 0;
        }
        ap->vol = vol;
        ap->restricted = rest;
        ap->offset->pragmas = pragmas;
        intermed_tail->isvolatile = vol;
        intermed_tail->isrestrict = rest;
        intermed_tail->cxlimited = pragmas & STD_PRAGMA_CXLIMITED;
    }
    else
        gen_icode(op, ap, left, right);
    return ap;
}
int chksize(int lsize, int rsize)
/*
 * compare two sizes, ignoring sign during comparison
 */
{
    int l, r;
    l = lsize;
    r = rsize;
    if (l < 0)
        l = -l;
    if (r < 0)
        r = -r;
    return (l > r);
}

IMODE* make_imaddress(EXPRESSION* node, int size)
{
    SimpleExpression* node1 = SymbolManager::Get(node);
    SimpleSymbol *sym = SymbolManager::Get(node->v.sp);
    IMODE* ap2;
    if (sym && sym->imaddress)
        return sym->imaddress;
    if (sym && sym->storage_class != sc_auto && sym->storage_class != sc_register)
    {
        IncGlobalFlag();
        sym->allocate = true;
    }
    ap2 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
    ap2->offset = node1;
    ap2->mode = i_immed;
    ap2->size = size;
    if (!sym->imvalue)  // the aliasing needs this regardless of whether we really use it
    {
        TYPE* tp = node->v.sp->tp;
        sym->imvalue = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
        *(sym->imvalue) = *ap2;
        sym->imvalue->mode = i_direct;
        while (tp->array)
            tp = tp->btp;
        if (isstructured(tp) || basetype(tp)->type == bt_memberptr)
            sym->imvalue->size = ISZ_UINT;
        else
            sym->imvalue->size = sizeFromType(tp);
    }
    if (sym && sym->storage_class != sc_auto && sym->storage_class != sc_register)
        DecGlobalFlag();
    if (sym)
    {
        sym->addressTaken = true;
        sym->imaddress = ap2;
    }
    return ap2;
}

IMODE* make_ioffset(EXPRESSION* node)
/*
 *      make a direct reference to a node.
 */
{
    SimpleExpression* node1 = SymbolManager::Get(node->left);
    IMODE* ap;
    SimpleSymbol* sym = varsp(node1);
    if (sym && sym->imvalue && sym->imvalue->size == natural_size(node))
        return sym->imvalue;
    if (sym && sym->storage_class != sc_auto && sym->storage_class != sc_register)
    {
        IncGlobalFlag();
    }
    ap = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
    ap->offset = node1;
    if (sym && sym->storage_class != sc_auto && sym->storage_class != sc_register)
        DecGlobalFlag();
    ap->mode = i_direct;
    ap->size = natural_size(node);
    if (sym && !sym->imvalue)
        sym->imvalue = ap;
    return ap;
}


/*-------------------------------------------------------------------------*/

IMODE* make_bf(EXPRESSION* node, IMODE* ap, int size)
/*
 *      construct a bit field reference
 */
{
    IMODE* ap1 = (IMODE*)Alloc(sizeof(IMODE));
    SimpleSymbol* sym = varsp(ap->offset);
    if (node->startbit == -1)
        diag("Illegal bit field");
    *ap1 = *ap;
    ap1->startbit = node->startbit;
    ap1->bits = node->bits;
    ap1->size = size;
    ap1->retval = false;
    if (sym)
        sym->usedasbit = true;
    return ap1;
}

static IMODE* gen_bit_load(IMODE* ap)
{
    IMODE* result1 = tempreg(ap->size, false);
    IMODE* result2 = tempreg(ap->size, false);
    int n = (1 << ap->bits) - 1;
    gen_icode(ap->size < 0 ? i_asr : i_lsr, result1, ap, make_immed(-ISZ_UINT, ap->startbit));
    gen_icode(i_and, result2, result1, make_immed(-ISZ_UINT, n));
    ap->bits = ap->startbit = 0;
    return result2;
}
static IMODE* gen_bit_mask(IMODE* ap)
{
    IMODE* result = tempreg(ap->size, false);
    int n = ~(((1 << ap->bits) - 1) << ap->startbit);
    gen_icode(i_and, result, ap, make_immed(-ISZ_UINT, n));
    ap->bits = ap->startbit = 0;
    return result;
}
/*-------------------------------------------------------------------------*/

IMODE* gen_deref(EXPRESSION* node, SYMBOL* funcsp, int flags)
/*s
 *      return the addressing mode of a dereferenced node.
 */
{
    IMODE *ap1, *ap2, *ap3;
    int siz1;
    SimpleSymbol* sym = nullptr;
    int nt = node->left->type;
    int store = flags & F_STORE;
    flags &= ~F_STORE;
    (void)flags;
    /* get size */
    switch (node->type) /* get load size */
    {
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en__initblk:
        case en__cpblk:
            siz1 = ISZ_ADDR;
            break;
        case en_l_bool:
            siz1 = ISZ_BOOLEAN;
            break;
        case en_l_uc:
            siz1 = ISZ_UCHAR;
            break;
        case en_l_c:
            siz1 = -ISZ_UCHAR;
            break;
        case en_l_u16:
            siz1 = ISZ_U16;
            break;
        case en_l_u32:
            siz1 = ISZ_U32;
            break;
        case en_l_us:
        case en_l_wc:
            siz1 = ISZ_USHORT;
            break;
        case en_l_s:
            siz1 = -ISZ_USHORT;
            break;
        case en_l_i:
            siz1 = -ISZ_UINT;
            break;
        case en_l_inative:
            siz1 = -ISZ_UNATIVE;
            break;
        case en_l_unative:
            siz1 = ISZ_UNATIVE;
            break;
        case en_l_p:
        case en_l_ref:
            siz1 = ISZ_ADDR;
            break;
        case en_l_fp:
            siz1 = ISZ_FARPTR;
            break;
        case en_l_sp:
            siz1 = ISZ_SEG;
            break;
        case en_l_l:
            siz1 = -ISZ_ULONG;
            break;
        case en_add:
        case en_arrayadd:
        case en_structadd:
            siz1 = ISZ_ADDR;
            break;
        case en_l_ui:
            siz1 = ISZ_UINT;
            break;
        case en_l_ul:
            siz1 = ISZ_ULONG;
            break;
        case en_l_ll:
            siz1 = -ISZ_ULONGLONG;
            break;
        case en_l_ull:
            siz1 = ISZ_ULONGLONG;
            break;
        case en_l_f:
            siz1 = ISZ_FLOAT;
            break;
        case en_l_d:
            siz1 = ISZ_DOUBLE;
            break;
        case en_l_ld:
            siz1 = ISZ_LDOUBLE;
            break;
        case en_l_fi:
            siz1 = ISZ_IFLOAT;
            break;
        case en_l_di:
            siz1 = ISZ_IDOUBLE;
            break;
        case en_l_ldi:
            siz1 = ISZ_ILDOUBLE;
            break;
        case en_l_fc:
            siz1 = ISZ_CFLOAT;
            break;
        case en_l_dc:
            siz1 = ISZ_CDOUBLE;
            break;
        case en_l_ldc:
            siz1 = ISZ_CLDOUBLE;
            break;
        case en_l_bit:
            siz1 = ISZ_BIT;
            break;
        case en_l_string:
            siz1 = ISZ_STRING;
            break;
        case en_l_object:
            siz1 = ISZ_OBJECT;
            break;
        default:
            siz1 = ISZ_UINT;
    }
    /* deref for add nodes */
    if ((architecture == ARCHITECTURE_MSIL) &&
        (node->left->type == en_bits || (node->left->type == en_structadd && node->left->right->type == en_structelem)))
    {
        // prepare for the MSIL ldfld instruction
        IMODE *aa1, *aa2;
        EXPRESSION* bitnode = node->left->type == en_bits ? node->left : nullptr;
        if (bitnode)
            node = node->left;
        aa1 = gen_expr(funcsp, node->left->left, 0, ISZ_ADDR);
        aa1->msilObject = true;
        //        if (!aa1->offset || aa1->mode != i_direct || aa1->offset->type != en_tempref)
        {
            aa2 = tempreg(aa1->size, 0);
            gen_icode(i_assn, aa2, aa1, nullptr);
        }
        aa1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
        aa1->offset = SymbolManager::Get(node->left->right);
        aa1->mode = i_direct;
        aa1->size = aa2->size;
        ap1 = LookupExpression(i_add, aa1->size, aa2, aa1);
        ap1 = indnode(ap1, siz1);
        ap1->fieldname = true;
        ap1->vararg = aa1->offset;
        ap1->vol = node->isvolatile;
        ap1->restricted = node->isrestrict;
        if (bitnode)
        {
            ap1->bits = bitnode->bits;
            ap1->startbit = bitnode->startbit;
        }
    }
    else if (node->left->type == en_add || node->left->type == en_arrayadd || node->left->type == en_structadd)
    {
        ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
        ap2 = LookupLoadTemp(nullptr, ap1);
        if (ap2 != ap1)
        {
            gen_icode(i_assn, ap2, ap1, nullptr);
            ap1 = ap2;
        }
        ap1 = indnode(ap1, siz1);
        if (ap1->offset)
        {
            ap1->vol = node->isvolatile;
            ap1->restricted = node->isrestrict;
        }
    }
    else if ((chosenAssembler->arch->denyopts & DO_UNIQUEIND) && (node->left->type == en_global || node->left->type == en_auto) &&
             ((isarray(node->left->v.sp->tp) && !basetype(node->left->v.sp->tp)->msil) || isstructured(node->left->v.sp->tp)))
    {
        ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
        ap2 = LookupLoadTemp(nullptr, ap1);
        if (ap2 != ap1)
        {
            gen_icode(i_assn, ap2, ap1, nullptr);
            ap1 = ap2;
        }
        ap1 = indnode(ap1, siz1);
    }
    else if (node->left->type == en_const)
    {
        ap1 = gen_expr(funcsp, node->left->v.sp->init->exp, 0, 0);
        //            ap1 = make_immed(siz1, node->v.sp->value.i);
    }
    /* deref for auto variables */
    else if (node->left->type == en_imode)
    {
        ap1 = (IMODE*)node->left->v.imode;
    }
    else
    {
        /* other deref */
        switch (nt)
        {
            EXPRESSION* node1;
            IMODE* ap2;
            case en_labcon:
                ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                ap1->mode = i_direct;
                ap1->offset = SymbolManager::Get(node->left);
                ap1->size = siz1;
                // node->left->v.sp->genreffed = true;
                break;
            case en_threadlocal:
                sym = SymbolManager::Get(node->left->v.sp);
                GENREF(sym);
                ap1 = make_ioffset(node);
                ap2 = LookupLoadTemp(ap1, ap1);
                if (ap1 != ap2)
                    gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = indnode(ap2, siz1);
                break;
            case en_auto:
                sym = SymbolManager::Get(node->left->v.sp);
                if (sym->thisPtr)
                {
                    if (inlinesym_count)
                    {
                        EXPRESSION* exp = inlinesym_thisptr[inlinesym_count - 1];
                        if (exp)
                            return gen_expr(funcsp, exp, 0, natural_size(exp));
                    }
                    else if (baseThisPtr)  // for this ptrs inherited in a constructor
                    {
                        if (sym != baseThisPtr)
                        {
                            sym = baseThisPtr;
                        }
                    }
                }
                if (sym->storage_class == scc_parameter && sym->paramSubstitute)
                {
                    node = sym->paramSubstitute;
                    // paramsubstitute is a NORMAL expression which is a derefed auto constant 
                    sym = SymbolManager::Get(sym->paramSubstitute->left->v.sp);
                }
                sym->allocate = true;
                if (catchLevel)
                    sym->inCatch = true;
                if (!sym->imaddress && catchLevel)
                {
                    ap1 = make_ioffset(node);
                    break;
                }
                // fall through
            case en_global:
            case en_pc:
                if (nt == en_global || nt == en_pc)
                    sym = SymbolManager::Get(node->left->v.sp);
                if (!sym->stackblock)
                //    			if (!isstructured(sym->tp))
                {
                    GENREF(sym);
                    ap1 = make_ioffset(node);
                    if (!store)
                    {
                        ap2 = LookupLoadTemp(nullptr, ap1);
                        if (ap2 != ap1)
                        {
                            gen_icode(i_assn, ap2, ap1, nullptr);
                            ap1 = ap2;
                        }
                    }
                    break;
                }
            // fall through
            default:
                ap1 = gen_expr(funcsp, node->left, 0, natural_size(node->left)); /* generate address */
                ap2 = LookupLoadTemp(nullptr, ap1);
                if (ap2 != ap1)
                {
                    gen_icode(i_assn, ap2, ap1, nullptr);
                    ap2 = indnode(ap2, siz1);
                    ap2->bits = ap1->bits;
                    ap2->startbit = ap1->startbit;
                    ap1 = ap2;
                }
                else
                {
                    ap2 = indnode(ap1, siz1);
                    ap2->bits = ap1->bits;
                    ap2->startbit = ap1->startbit;
                    ap1 = ap2;
                }
                break;
        }
    }
    if (!store)
    {
        ap2 = LookupLoadTemp(nullptr, ap1);
        if (ap2 != ap1)
        {
            gen_icode(i_assn, ap2, ap1, nullptr);
            if (ap1->bits > 0 && (chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
            {
                ap2->bits = ap1->bits;
                ap2->startbit = ap1->startbit;
                ap1->bits = ap1->startbit = 0;
                ap2 = gen_bit_load(ap2);
            }
            ap1 = ap2;
        }
    }
    if (ap1->offset)
    {
        ap1->vol = node->isvolatile;
        ap1->restricted = node->isrestrict;
    }
    return ap1;
}
/*-------------------------------------------------------------------------*/

IMODE* gen_unary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op)
/*
 *      generate code to evaluate a unary minus or complement.
 */
{
    IMODE *ap, *ap1;
    (void)flags;
    ap1 = gen_expr(funcsp, node->left, F_VOL, size);
    ap = LookupLoadTemp(nullptr, ap1);
    if (ap1 != ap)
        gen_icode(i_assn, ap, ap1, nullptr);
    ap = LookupExpression(op, size, ap, nullptr);
    ap->vol = node->left->isvolatile;
    ap->restricted = node->left->isrestrict;
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE* gen_asrhd(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op)
{
    IMODE *ap, *ap1, *ap2, *ap3;
    int lab = nextLabel++;
    long long n;
    (void)flags;
    (void)size;
    n = mod_mask(node->right->v.i);
    ap = tempreg(natural_size(node->left), 0);
    ap->offset->sp->pushedtotemp = true;
    ap3 = gen_expr(funcsp, node->left, F_VOL | F_COMPARE, natural_size(node->left));
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, nullptr);
    gen_icode(i_assn, ap, ap1, nullptr);
    ap3 = gen_expr(funcsp, node->right, F_COMPARE, natural_size(node->right));
    ap2 = LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, nullptr);
    DumpIncDec(funcsp);
    DumpLogicalDestructors(node, funcsp);
    gen_icgoto(i_jge, lab, ap, make_immed(ap->size, 0));
    gen_icode(i_add, ap, ap, make_immed(ISZ_UINT, n));
    gen_label(lab);
    gen_icode(op, ap, ap, ap2);
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE* gen_binary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op)
/*
 *      generate code to evaluate a binary node and return
 *      the addressing mode of the result.
 */
{
    IMODE *ap, *ap1, *ap2, *ap3;
    int vol, rest, pragmas;
    (void)flags;
    ap3 = gen_expr(funcsp, node->left, flags, natural_size(node->left));
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, nullptr);

    if (op == i_lsl || op == i_lsr || op == i_asr)
        flush_dag();

    ap3 = gen_expr(funcsp, node->right, flags, natural_size(node->right));
    ap2 = LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, nullptr);
    if (ap1->size >= ISZ_CFLOAT || ap2->size >= ISZ_CFLOAT)
        size = imax(ap1->size, ap2->size);
    else if (ap1->size >= ISZ_IFLOAT && ap2->size < ISZ_IFLOAT)
    {
        if (op == i_add)
            size = ap1->size - ISZ_IFLOAT + ISZ_CFLOAT;
        else
            size = ap1->size;
    }
    else if (ap2->size >= ISZ_IFLOAT && ap1->size < ISZ_IFLOAT)
    {
        if (op == i_add)
            size = ap2->size - ISZ_IFLOAT + ISZ_CFLOAT;
        else
            size = ap2->size;
    }
    else if (ap1->size >= ISZ_IFLOAT && ap2->size >= ISZ_IFLOAT)
    {
        if (op == i_mul || op == i_sdiv)
            size = ap2->size - ISZ_IFLOAT + ISZ_FLOAT;
    }
    ap = LookupExpression(op, size, ap1, ap2);
    return ap;
}
// int size is assumed to be 32 bits, needs fixing...
#define N 32
#define oneshl32 (((unsigned long long)1) << N)
static int ChooseMultiplier(unsigned d, int prec, unsigned long long* m, int* sh, int* l)
{
    if (d != 0)
    {
        unsigned long long ml, mh;
        unsigned n = d;
        int l1 = 0;
        while (n != 1)
            l1++, n >>= 1;
        if ((1 << l1) != d)
            l1++;
        *sh = *l = l1;
        ml = (oneshl32 << l1) / d;
        mh = ((oneshl32 << l1) + (oneshl32 >> (prec - l1))) / d;
        while (*sh && ml / 2 < mh / 2)
            (*sh)--, ml /= 2, mh /= 2;
        *m = mh;
        return 1;
    }
    return 0;
}

IMODE* gen_udivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op, bool mod)
{
    int n = natural_size(node);
    if (!(chosenAssembler->arch->denyopts & DO_NOFASTDIV) &&
        (n == ISZ_UINT || n == -ISZ_UINT || n == ISZ_ULONG || n == -ISZ_ULONG) && isintconst(node->right))
    {
        unsigned long long m;
        int post, l, pre = 0;
        unsigned d = node->right->v.i;
        if (d == 1)
            return gen_expr(funcsp, node->left, flags, size);
        if (ChooseMultiplier(d, N, &m, &post, &l))
        {
            IMODE *num = gen_expr(funcsp, node->left, F_VOL, size), *ap, *ap1, *ap2, *ap3, *ap4, *ap5;
            QUAD* q;
            ap1 = LookupLoadTemp(nullptr, num);
            ap = tempreg(n, 0);
            if (ap1 != num)
            {
                gen_icode(i_assn, ap1, num, nullptr);
                num = ap1;
            }
            if (m >= oneshl32 && !(d & 1))
            {
                int ld;
                unsigned x = d & (oneshl32 - d);
                while (!(x & 1))
                {
                    x >>= 1;
                    pre++;
                }
                ChooseMultiplier(d >> pre, N - pre, &m, &post, &ld);
            }
            if (d == 1)
                return num;
            else if (d == 1 << l)
            {
                if (mod)
                {
                    gen_icode(i_and, ap, num, make_immed(ISZ_UINT, (1 << l) - 1));
                    return ap;
                }
                ap5 = tempreg(n, 0);
                gen_icode(i_lsr, ap5, num, make_immed(ISZ_UINT, l));

                //                return n >> l;
            }
            else if (m >= oneshl32)
            {
                ap3 = tempreg(n, 0);
                gen_icode_with_conflict(i_assn, ap3, num, nullptr, true);
                ap1 = tempreg(n, 0);
                gen_icode(i_muluh, ap1, ap3, make_immed(ISZ_UINT, m - oneshl32));
                ap2 = tempreg(n, 0);
                gen_icode(i_sub, ap2, num, ap1);
                ap3 = tempreg(n, 0);
                gen_icode(i_lsr, ap3, ap2, make_immed(ISZ_UINT, 1));
                ap4 = tempreg(n, 0);
                gen_icode(i_add, ap4, ap1, ap3);
                ap5 = tempreg(n, 0);
                if (post - 1)
                    gen_icode(i_lsr, ap5, ap4, make_immed(ISZ_UINT, post - 1));
                else
                    gen_icode(i_assn, ap5, ap4, nullptr);
                /*
                unsigned t1 = ((m-oneshl32) *n) >> 32;
                unsigned q = (n-t1)>>1;
                return (t1 + q) >> (post-1);
                */
            }
            else
            {
                ap1 = tempreg(n, 0);
                gen_icode_with_conflict(i_assn, ap1, num, nullptr, true);
                ap3 = tempreg(n, 0);
                if (pre)
                {
                    gen_icode(i_lsr, ap3, ap1, make_immed(ISZ_UINT, pre));
                }
                else
                {
                    gen_icode(i_assn, ap3, ap1, nullptr);
                }
                ap2 = tempreg(n, 0);
                gen_icode(i_muluh, ap2, ap3, make_immed(ISZ_UINT, m - oneshl32));
                ap5 = tempreg(n, 0);
                if (post)
                    gen_icode(i_lsr, ap5, ap2, make_immed(ISZ_UINT, post));
                else
                    gen_icode(i_assn, ap5, ap2, nullptr);
                /*
                unsigned t1 = n >> pre;
                t1 = (m * t1)>>32;
                return t1 >> post;
                */
            }
            if (mod)
            {
                ap2 = tempreg(n, 0);
                gen_icode(i_mul, ap2, ap5, make_immed(ISZ_UINT, d));
                gen_icode(i_sub, ap, num, ap2);
            }
            else
            {
                gen_icode(i_assn, ap, ap5, nullptr);
            }
            return ap;
        }
    }
    return gen_binary(funcsp, node, flags, size, op);
}
IMODE* gen_sdivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op, bool mod)
{
    int n = natural_size(node);
    if (!(chosenAssembler->arch->denyopts & DO_NOFASTDIV) && (n == ISZ_UINT || n == -ISZ_UINT) && node->right->type == en_c_i)
    {
        int d = node->right->v.i;
        int ad = d < 0 ? -d : d, q;
        unsigned long long m;
        int post, l, pre = 0;
        if (ChooseMultiplier(ad, 31, &m, &post, &l))
        {
            IMODE *num = gen_expr(funcsp, node->left, F_VOL, size), *ap, *ap1, *ap2, *ap3, *ap4, *ap5 = nullptr;
            QUAD* q;
            ap1 = LookupLoadTemp(nullptr, num);
            ap = tempreg(n, 0);
            if (ap1 != num)
            {
                gen_icode(i_assn, ap1, num, nullptr);
                num = ap1;
            }
            if (ad == 1)
                ap = num;
            //                q = ad;
            else if (ad == 1 << l)
            {
                ap2 = tempreg(n, 0);
                if (l > 1)
                    gen_icode(i_asr, ap2, num, make_immed(ISZ_UINT, l - 1));
                else
                {
                    gen_icode(i_assn, ap2, num, nullptr);
                }
                ap3 = tempreg(n, 0);
                gen_icode(i_lsr, ap3, ap2, make_immed(ISZ_UINT, N - l));
                ap4 = tempreg(n, 0);
                gen_icode(i_add, ap4, ap3, num);
                ap5 = tempreg(n, 0);
                gen_icode(i_asr, ap5, ap4, make_immed(ISZ_UINT, l));
                /*
                q = n >> (l-1);
                q = (unsigned)q >> (32 - l);
                q = (n + q) >> l ;
                */
            }
            else if (m < oneshl32 / 2)
            {
                ap3 = tempreg(n, 0);
                gen_icode_with_conflict(i_assn, ap3, num, nullptr, true);
                ap1 = tempreg(n, 0);
                gen_icode(i_mulsh, ap1, ap3, make_immed(ISZ_UINT, m));
                ap2 = tempreg(n, 0);
                if (post)
                    gen_icode(i_asr, ap2, ap1, make_immed(ISZ_UINT, post));
                else
                    gen_icode(i_assn, ap2, ap1, nullptr);
                ap4 = tempreg(n, 0);
                gen_icode(i_asr, ap4, num, make_immed(ISZ_UINT, N - 1));
                ap5 = tempreg(n, 0);
                gen_icode(i_sub, ap5, ap2, ap4);
                /*
                q = ((LLONG)m) * n >> 32;
                q = q >> post;
                q = q - (n < 0 ? -1 : 0);
                */
            }
            else
            {
                ap3 = tempreg(n, 0);
                gen_icode_with_conflict(i_assn, ap3, num, nullptr, true);
                ap1 = tempreg(n, 0);
                gen_icode(i_mulsh, ap1, ap3, make_immed(ISZ_UINT, m - oneshl32));
                ap2 = tempreg(n, 0);
                gen_icode(i_add, ap2, ap1, num);
                ap4 = tempreg(n, 0);
                if (post)
                    gen_icode(i_asr, ap4, ap2, make_immed(ISZ_UINT, post));
                else
                    gen_icode(i_assn, ap4, ap2, nullptr);
                ap3 = tempreg(n, 0);
                gen_icode(i_asr, ap3, num, make_immed(ISZ_UINT, N - 1));
                ap5 = tempreg(n, 0);
                gen_icode(i_sub, ap5, ap4, ap3);
                /*
                q = ((LLONG)(m-oneshl32))*n>>32;
                q = (n + q) >> post;
                q = q - (n < 0 ? -1 : 0);
                */
            }
            if (d < 0)
            {
                ap3 = tempreg(n, 0);
                gen_icode(i_neg, ap3, ap5, nullptr);
                ap5 = ap3;
            }
            if (mod)
            {
                ap3 = tempreg(n, 0);
                gen_icode(i_mul, ap3, ap5, make_immed(ISZ_UINT, d));
                gen_icode(i_sub, ap, num, ap3);
            }
            else
            {
                gen_icode(i_assn, ap, ap5, nullptr);
            }
            return ap;
        }
    }
    return gen_binary(funcsp, node, flags, size, op);
}
/*-------------------------------------------------------------------------*/

IMODE* gen_pdiv(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *			generate code for an array/structure size compensation
 */
{
    int n = natural_size(node);
    if (!(chosenAssembler->arch->denyopts & DO_NOFASTDIV) && n == ISZ_ADDR && node->right->type == en_c_i)
    {
        int d = node->right->v.i;
        if (d)
        {
            int dinv;
            int e = 0;
            IMODE *num = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT), *ap, *ap1;
            ap1 = LookupLoadTemp(nullptr, num);
            if (ap1 != num)
            {
                gen_icode(i_assn, ap1, num, nullptr);  // DAL fixed
                num = ap1;
            }
            while (!(d & 1))
                e++, d >>= 1;
            dinv = d;
            while (((dinv * d) & 0xffffffff) != 1)
            {
                long long dis = dinv, ds = d;
                dinv = dis * (2 - dis * ds);
            }
            ap = tempreg(n, 0);
            gen_icode(i_mul, ap, num, make_immed(ISZ_UINT, dinv));
            if (e)
                gen_icode(i_asr, ap, ap, make_immed(ISZ_UINT, e));
            return ap;
            //            return (dinv * n) >> e;
        }
    }
    return gen_binary(funcsp, node, flags, size, i_sdiv);
}

/*-------------------------------------------------------------------------*/

IMODE* gen_pmul(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *			generate code for an array/structure size compensation
 */
{
    IMODE *ap, *ap1, *ap2, *ap3;
    (void)flags;
    (void)size;
    ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, nullptr);
    ap3 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
    ap2 = LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, nullptr);
    ap = LookupExpression(i_mul, ISZ_UINT, ap1, ap2);
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE* gen_hook(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *      generate code to evaluate a condition operator node (?:)
 */
{
    IMODE *ap1, *ap2, *ap3;
    int false_label, end_label;
    LIST* idl;
    false_label = nextLabel++;
    end_label = nextLabel++;
    flags = flags | F_VOL;
    if ((architecture == ARCHITECTURE_MSIL))
        flags &= ~F_NOVALUE;
    DumpIncDec(funcsp);
    falsejp(node->left, funcsp, false_label);
    node = node->right;
    ap1 = tempreg(natural_size(node), 0);
    ap1->offset->sp->pushedtotemp = true;  // needed to make the global opts happy
    ap3 = gen_expr(funcsp, node->left, flags, size);
    ap2 = LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, nullptr);
    gen_icode(i_assn, ap1, ap2, 0);
    intermed_tail->hook = true;
    DumpIncDec(funcsp);
    gen_igoto(i_goto, end_label);
    gen_label(false_label);
    ap3 = gen_expr(funcsp, node->right, flags, size);
    ap2 = LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, nullptr);
    gen_icode(i_assn, ap1, ap2, 0);
    intermed_tail->hook = true;
    DumpIncDec(funcsp);
    gen_label(end_label);
    return ap1;
}

/*-------------------------------------------------------------------------*/

IMODE* gen_moveblock(EXPRESSION* node, SYMBOL* funcsp)
/*
 * Generate code to copy one structure to another
 */
{
    IMODE *ap1, *ap2, *ap3, *ap6, *ap7, *ap8;
    if (!node->size)
        return (0);
    if ((architecture == ARCHITECTURE_MSIL))
    {
        int mode;
        if (node->left->type == en_msil_array_access)
        {
            gen_expr(funcsp, node->left, F_STORE, ISZ_OBJECT);
            ap2 = gen_expr(funcsp, node->right, F_OBJECT, ISZ_OBJECT);
            gen_icode(i_parm, 0, ap2, 0);
            ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
            ap1->mode = i_immed;
            ap1->offset = SymbolManager::Get(node->left);
            ap1->size = ap2->size;
            gen_icode(i_assn, ap1, ap2, nullptr);
        }
        else
        {
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            mode = ap1->mode;
            ap7 = tempreg(ISZ_UINT, 0);
            *ap7 = *ap1;
            ap1 = ap7;
            ap1->mode = i_ind;
            if (isconstzero(&stdpointer, node->right))
            {
                ap3 = make_immed(ISZ_OBJECT, 0);  // LDNULL
            }
            else
                ap3 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
            ap2 = LookupLoadTemp(nullptr, ap3);
            if (ap2 != ap3)
                gen_icode(i_assn, ap2, ap3, nullptr);
            gen_icode(i_assn, ap1, ap2, nullptr);
            intermed_tail->alttp = SymbolManager::Get((TYPE *)node->altdata);
            intermed_tail->blockassign = true;
            intermed_tail->oldmode = mode;
        }
    }
    else
    {
        ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
        ap1 = LookupLoadTemp(nullptr, ap3);
        if (ap1 != ap3)
            gen_icode(i_assn, ap1, ap3, nullptr);
        ap3 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
        ap2 = LookupLoadTemp(nullptr, ap3);
        if (ap2 != ap3)
            gen_icode(i_assn, ap2, ap3, nullptr);
        ap6 = make_immed(ISZ_UINT, node->size);
        gen_icode(i_assnblock, ap6, ap1, ap2);
    }
    return (ap1);
}
IMODE* gen_clearblock(EXPRESSION* node, SYMBOL* funcsp)
/*
 * Generate code to copy one structure to another
 */
{
    IMODE *ap1, *ap3, *ap4, *ap5, *ap6, *ap7, *ap8;
    if (node->size)
    {
        ap4 = make_immed(ISZ_UINT, node->size);
    }
    else if (node->right)
    {
        ap5 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
        ap4 = LookupLoadTemp(nullptr, ap5);
        if (ap5 != ap4)
            gen_icode(i_assn, ap4, ap5, nullptr);
    }
    else
    {
        return (0);
    }
    ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, nullptr);
    if ((architecture == ARCHITECTURE_MSIL))
    {
        ap6 = make_immed(ISZ_UINT, 0);
        ap7 = LookupLoadTemp(nullptr, ap6);
        if (ap7 != ap6)
            gen_icode(i_assn, ap7, ap6, nullptr);
        ap8 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
        memcpy(ap8, ap7, sizeof(IMODE));
        ap8->mode = i_ind;
        gen_icode(i_clrblock, ap8, ap1, ap4);
    }
    else
    {
        gen_icode(i_clrblock, nullptr, ap1, ap4);
    }
    return (ap1);
}
IMODE* gen_cpinitblock(EXPRESSION* node, SYMBOL* funcsp, bool cp, int flags)
/*
 * Generate code to copy one structure to another
 */
{
    IMODE *ap1, *ap3, *ap4, *ap5, *ap6, *ap7, *ap8;
    ap6 = gen_expr(funcsp, node->left->left, F_VOL, ISZ_UINT);
    ap7 = LookupLoadTemp(nullptr, ap6);
    if (ap7 != ap6)
        gen_icode(i_assn, ap7, ap6, nullptr);
    ap8 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
    memcpy(ap8, ap7, sizeof(IMODE));
    ap8->mode = i_ind;
    ap3 = gen_expr(funcsp, node->left->right, F_VOL, ISZ_UINT);
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, nullptr);
    ap5 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
    ap4 = LookupLoadTemp(nullptr, ap5);
    if (ap5 != ap4)
        gen_icode(i_assn, ap4, ap5, nullptr);
    if (cp)
        gen_icode(i__cpblk, ap8, ap1, ap4);
    else
        gen_icode(i__initblk, ap8, ap1, ap4);
    intermed_tail->alwayslive = true;

    if (!(flags & F_NOVALUE))
    {

        ap6 = gen_expr(funcsp, node->left->left, F_VOL, ISZ_UINT);
        ap7 = LookupLoadTemp(nullptr, ap6);
        if (ap7 != ap6)
            gen_icode(i_assn, ap7, ap6, nullptr);
    }
    return (ap7);
}

/*-------------------------------------------------------------------------*/
static void PushArrayLimits(SYMBOL* funcsp, TYPE* tp)
{
    if (!tp || !isarray(tp))
        return;
    int n1 = basetype(tp)->size;
    int n2 = basetype(basetype(tp)->btp)->size;
    if (n1 && n2)
    {
        IMODE* ap = tempreg(-ISZ_UINT, 0);
        gen_icode(i_assn, ap, make_immed(-ISZ_UINT, n1 / n2), nullptr);
        gen_icode(i_parm, 0, ap, 0);
    }
    else
    {
        IMODE* ap;
        if (basetype(tp)->esize)
            ap = gen_expr(funcsp, basetype(tp)->esize, 0, -ISZ_UINT);
        else
            ap = make_immed(-ISZ_UINT, 1);  // not really creating something, but give it a size so we can proceed.
        gen_icode(i_parm, 0, ap, 0);
    }
    PushArrayLimits(funcsp, basetype(tp)->btp);
}

IMODE* gen_assign(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *      generate code for an assignment node. if the size of the
 *      assignment destination is larger than the size passed then
 *      everything below this node will be evaluated with the
 *      assignment size.
 */
{
    IMODE *ap1, *ap2, *ap3, *ap4;
    EXPRESSION *enode, *temp;
    LIST *l2, *lp;
    (void)flags;
    (void)size;
    if (node->right->type == en_msil_array_init)
    {
        TYPE* base = node->right->v.tp;
        PushArrayLimits(funcsp, node->right->v.tp);
        while (isarray(base))
            base = basetype(base)->btp;
        ap1 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, isstructured(base) ? ISZ_OBJECT : sizeFromType(base));
        ap2 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
        ap2->mode = i_immed;
        ap2->offset = SymbolManager::Get(node->right);
        ap2->size = ap1->size;
        gen_icode(i_assn, ap1, ap2, nullptr);
    }
    else if (node->left->type == en_msil_array_access)
    {
        TYPE* base = node->left->v.msilArray->tp;
        while (isarray(base))
            base = basetype(base)->btp;
        gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, sizeFromType(base));
        ap2 = gen_expr(funcsp, node->right, (flags & ~F_NOVALUE), sizeFromType(base));
        // gen_icode(i_parm, 0, ap2, 0);
        ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
        ap1->mode = i_immed;
        ap1->offset = SymbolManager::Get(node->left);
        ap1->size = ap2->size;
        gen_icode(i_assn, ap1, ap2, nullptr);
    }
    else
    {
        if (chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        {
            EXPRESSION* ex = node->right;
            int n = 0, m;
            ap1 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
            if (ap1->bits > 0 && (chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
            {
                n = ap1->startbit;
                m = ap1->bits;
                ap1->bits = ap1->startbit = 0;
                ap3 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
                ap4 = LookupLoadTemp(ap3, ap3);
                if (ap4 != ap3)
                {
                    ap4->bits = ap3->bits;
                    ap4->startbit = ap3->startbit;
                    ap3->bits = ap3->startbit = 0;
                    gen_icode(i_assn, ap4, ap3, nullptr);
                }
                ap3 = gen_bit_mask(ap4);
            }
            else
            {
                ap3 = nullptr;
            }
            while (castvalue(ex))
                ex = ex->left;
            if ((ap1->size == ISZ_OBJECT || ap1->size == ISZ_STRING) && isconstzero(&stdint, ex))
            {
                ap2 = make_immed(ISZ_OBJECT, 0);  // LDNULL
            }
            else
            {
                switch (node->right->type)
                {
                    case en_auto:
                    case en_global:
                    case en_pc:
                    case en_threadlocal:
                        if (isstructured(node->right->v.sp->tp))
                        {
                            ap2 = gen_expr(funcsp, node->right, (flags & ~F_NOVALUE) | F_OBJECT, natural_size(node->left));
                            break;
                        }
                    default:
                        ap2 = gen_expr(funcsp, node->right, (flags & ~F_NOVALUE), natural_size(node->left));
                        break;
                }
            }
            ap4 = LookupLoadTemp(ap2, ap2);
            if (ap4 != ap2)
                gen_icode(i_assn, ap4, ap2, nullptr);
            if (ap3)
            {
                gen_icode(i_and, ap4, ap4, make_immed(-ISZ_UINT, (1 << m) - 1));
                if (n)
                    gen_icode(i_lsl, ap4, ap4, make_immed(-ISZ_UINT, n));
                gen_icode(i_or, ap4, ap3, ap4);
            }
        }
        else
        {
            ap2 = gen_expr(funcsp, node->right, flags & ~F_NOVALUE, natural_size(node->left));
            ap4 = LookupLoadTemp(ap2, ap2);
            if (ap4 != ap2)
                gen_icode(i_assn, ap4, ap2, nullptr);
            ap1 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
        }
        ap1->vol = ap2->vol;
        ap2->restricted = ap4->restricted;
        ap1->offset->pragmas = ap4->offset->pragmas;
        gen_icode(i_assn, ap1, ap4, nullptr);
        /*
        if (ap1->mode != i_direct || ap1->offset->type != en_tempref)
        {
            ap3 = tempreg(ap4->size, false);
            gen_icode(i_assn, ap3, ap4, nullptr);
            ap4 = ap3;
        }
        ap3 = LookupStoreTemp(ap1, ap1);
        if (ap3 != ap1)
        {
            gen_icode(i_assn, ap3, ap4, nullptr);
            gen_icode(i_assn, ap1, ap3, nullptr);
        }
        else
            gen_icode(i_assn, ap1, ap4, nullptr);
        */
    }
    if (node->left->isatomic)
    {
        gen_icode(i_atomic_fence, nullptr, make_immed(ISZ_UINT, mo_seq_cst | 0x80), nullptr);
    }
    if (!(flags & F_NOVALUE) && (chosenAssembler->arch->preferopts & OPT_REVERSESTORE) && ap1->mode == i_ind)
    {
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), (flags & ~F_NOVALUE), natural_size(node->left));
    }
    ap1->vol = node->left->isvolatile;
    ap1->restricted = node->left->isrestrict;
    return ap1;
}

/*-------------------------------------------------------------------------*/

IMODE* gen_aincdec(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op)
/*
 *      generate an auto increment or decrement node. op should be
 *      either op_add (for increment) or op_sub (for decrement).
 */
{
    IMODE *ap1 = nullptr, *ap2, *ap3 = nullptr, *ap4, *ap5, *ap6, *ap7 = nullptr;
    int siz1;
    EXPRESSION* ncnode;
    LIST* l;
    int n, m;
    (void)size;
    siz1 = natural_size(node->left);
    if (flags & F_NOVALUE)
    {
        ncnode = node->left;
        while (castvalue(ncnode))
            ncnode = ncnode->left;
        ap6 = gen_expr(funcsp, ncnode, F_STORE, siz1);
        if (ap6->bits > 0 && (chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
        {
            n = ap6->startbit;
            m = ap6->bits;
            ap6->bits = ap6->startbit = 0;
            ap7 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
            ap4 = LookupLoadTemp(ap7, ap7);
            if (ap4 != ap7)
            {
                ap4->bits = ap7->bits;
                ap4->startbit = ap7->startbit;
                ap7->bits = ap7->startbit = 0;
                gen_icode(i_assn, ap4, ap7, nullptr);
            }
            ap7 = gen_bit_mask(ap4);
        }
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = LookupLoadTemp(ap1, ap1);
        if (ap5 != ap1)
            gen_icode(i_assn, ap5, ap1, nullptr);
        ap2 = gen_expr(funcsp, node->right, 0, siz1);
        ap2 = LookupExpression(op, siz1, ap5, ap2);
        if (ap7)
        {
            gen_icode(i_and, ap2, ap2, make_immed(-ISZ_UINT, (1 << m) - 1));
            if (n)
                gen_icode(i_lsl, ap2, ap2, make_immed(-ISZ_UINT, n));
            gen_icode(i_or, ap2, ap7, ap2);
        }
        ap4 = LookupStoreTemp(ap6, ap6);
        if (ap4 != ap6)
        {
            gen_icode(i_assn, ap4, ap2, nullptr);
            gen_icode(i_assn, ap6, ap4, nullptr);
        }
        else
            gen_icode(i_assn, ap6, ap2, nullptr);
        return nullptr;
    }
    else if (flags & F_COMPARE)
    {
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = LookupLoadTemp(ap1, ap1);
        if (ap1 != ap5)
            gen_icode(i_assn, ap5, ap1, nullptr);
        if ((architecture == ARCHITECTURE_MSIL))
        {
            ap3 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
            ap5 = LookupLoadTemp(ap3, ap3);
            if (ap5 != ap3)
                gen_icode(i_assn, ap5, ap3, nullptr);
            ap3 = ap1;
        }
        ncnode = node->left;
        while (castvalue(ncnode))
            ncnode = ncnode->left;
        ap6 = gen_expr(funcsp, ncnode, F_STORE, siz1);
        if (ap6->bits > 0 && (chosenAssembler->arch->denyopts & DO_MIDDLEBITS))
        {
            n = ap6->startbit;
            m = ap6->bits;
            ap6->bits = ap6->startbit = 0;
            ap7 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
            ap4 = LookupLoadTemp(ap7, ap7);
            if (ap4 != ap7)
            {
                ap4->bits = ap7->bits;
                ap4->startbit = ap7->startbit;
                ap7->bits = ap7->startbit = 0;
                gen_icode(i_assn, ap4, ap7, nullptr);
            }
            ap7 = gen_bit_mask(ap4);
        }
        if (!((architecture == ARCHITECTURE_MSIL)))
        {
            ap3 = tempreg(siz1, 0);
            gen_icode(i_assn, ap3, ap5, nullptr);
            intermed_tail->needsOCP = true;
        }
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = LookupLoadTemp(ap1, ap1);
        if (ap5 != ap1)
            gen_icode(i_assn, ap5, ap1, nullptr);
        ap2 = gen_expr(funcsp, node->right, 0, siz1);
        ap2 = LookupExpression(op, siz1, ap5, ap2);
        if (ap7)
        {
            gen_icode(i_and, ap2, ap2, make_immed(-ISZ_UINT, (1 << m) - 1));
            if (n)
                gen_icode(i_lsl, ap2, ap2, make_immed(-ISZ_UINT, n));
            gen_icode(i_or, ap2, ap7, ap2);
        }
        ap4 = LookupStoreTemp(ap6, ap6);
        if (ap4 != ap6)
        {
            gen_icode(i_assn, ap4, ap2, nullptr);
            gen_icode(i_assn, ap6, ap4, nullptr);
        }
        else
            gen_icode(i_assn, ap6, ap2, nullptr);
        return ap3;
    }
    else
    {
        ap1 = gen_expr(funcsp, RemoveAutoIncDec(node->left), 0, siz1);
        ap5 = LookupLoadTemp(ap1, ap1);
        if (ap5 != ap1)
            gen_icode(i_assn, ap5, ap1, nullptr);
        l = (LIST*)Alloc(sizeof(LIST));
        l->data = (void*)node;
        if (!incdecList)
            incdecList = incdecListLast = l;
        else
            incdecListLast = incdecListLast->next = l;
        return ap5;
    }
}
/*-------------------------------------------------------------------------*/
static EXPRESSION* getAddress(EXPRESSION* exp)
{
    EXPRESSION* rv = nullptr;
    if (exp->type == en_add)
    {
        rv = getAddress(exp->left);
        if (!rv)
            rv = getAddress(exp->right);
    }
    else if (exp->type == en_auto)
    {
        return exp;
    }
    return rv;
}
/*-------------------------------------------------------------------------*/
static EXPRESSION* getFunc(EXPRESSION* exp)
{
    EXPRESSION* rv = nullptr;
    if (exp->type == en_thisref)
        exp = exp->left;
    if (exp->type == en_add)
    {
        rv = getFunc(exp->left);
        if (!rv)
            rv = getFunc(exp->right);
    }
    else if (exp->type == en_func)
    {
        return exp;
    }
    return rv;
}
int push_param(EXPRESSION* ep, SYMBOL* funcsp, bool vararg, EXPRESSION* valist, int flags)
/*
 *      push the operand expression onto the stack.
 */
{
    IMODE *ap, *ap3;
    int temp;
    int rv = 0;
    EXPRESSION* exp = getFunc(ep);
    if (exp || ep->type == en_blockassign || ep->type == en_blockclear)
    {
        EXPRESSION* ep1 = exp;
        if (exp)
        {
            exp = ep1->v.func->returnEXP;
            if (!exp)
                exp = ep1->v.func->thisptr;
            if (exp)
                exp = getAddress(exp);
        }
        else if (ep->type == en_auto)
        {
            exp = ep;
        }
        else
        {
            exp = ep->left;
        }
        if (exp && exp->type == en_auto && exp->v.sp->stackblock)
        {
            // constructor or other function creating a structure on the stack
            rv = exp->v.sp->tp->size;
            if (rv % chosenAssembler->arch->stackalign)
                rv = rv + chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
            gen_icode(i_parmstack, ap = tempreg(ISZ_ADDR, 0), make_immed(ISZ_UINT, rv), nullptr);
            intermed_tail->vararg = vararg;
            SymbolManager::Get(exp->v.sp)->imvalue = ap;
            gen_expr(funcsp, ep, 0, ISZ_UINT);
        }
        else
        {
            temp = natural_size(ep);
            ap3 = gen_expr(funcsp, ep, 0, temp);
            ap = LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, nullptr);
            if (ap->size == ISZ_NONE)
                ap->size = temp;
            gen_nodag(i_parm, 0, ap, 0);
            intermed_tail->vararg = vararg;
            intermed_tail->valist = valist && valist->type == en_l_p;
            rv = sizeFromISZ(ap->size);
        }
    }
    else
    {
        EXPRESSION* exp1 = ep;
        switch (ep->type)
        {
            case en_void:
                while (ep->type == en_void)
                {
                    gen_expr(funcsp, ep->left, 0, ISZ_UINT);
                    ep = ep->right;
                }
                push_param(ep, funcsp, vararg, valist, flags);
                break;
            case en_argnopush:
                gen_expr(funcsp, ep->left, 0, ISZ_UINT);
                break;
            case en_imode:
                ap = (IMODE*)ep->left;
                gen_nodag(i_parm, 0, ap, 0);
                intermed_tail->vararg = vararg;
                intermed_tail->valist = valist && valist->type == en_l_p;
                rv = sizeFromISZ(ap->size);
                break;
            default:
                while (castvalue(exp1))
                    exp1 = exp1->left;
                temp = natural_size(ep);
                if (temp == ISZ_OBJECT && isconstzero(&stdint, exp1))
                    ap3 = make_immed(ISZ_OBJECT, 0);  // LDNULL
                else
                    ap3 = gen_expr(funcsp, ep, flags, temp);
                if (ap3->bits > 0)
                    ap3 = gen_bit_load(ap3);
                ap = LookupLoadTemp(nullptr, ap3);
                if (ap != ap3)
                    gen_icode(i_assn, ap, ap3, nullptr);
                if (ap->size == ISZ_NONE)
                    ap->size = temp;
                gen_nodag(i_parm, 0, ap, 0);
                intermed_tail->vararg = vararg;
                intermed_tail->valist = valist && valist->type == en_l_p;
                rv = sizeFromISZ(ap->size);
                break;
        }
        if (rv % chosenAssembler->arch->stackalign)
            rv = rv + chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

static int push_stackblock(TYPE* tp, EXPRESSION* ep, SYMBOL* funcsp, int sz, bool vararg, EXPRESSION* valist)
/*
 * Push a structure on the stack
 */
{
    IMODE *ap, *ap1, *ap2, *ap3;
    if (!sz)
        return 0;
    switch (ep->type)
    {
        case en_imode:
            ap = ep->v.imode;
            break;
        default:
            ap3 = gen_expr(funcsp, ep, F_ADDR, ISZ_UINT);
            if (ap3->mode != i_direct && (chosenAssembler->arch->preferopts & OPT_ARGSTRUCTREF))
            {
                ap = (IMODE*)oAlloc(sizeof(IMODE));
                *ap = *ap3;
                ap3 = ap;
                ap3->mode = i_direct;
            }
            ap = LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, nullptr);
            break;
    }
    gen_nodag(i_parmblock, 0, ap, make_immed(ISZ_UINT, sz));
    intermed_tail->alttp = SymbolManager::Get(tp);
    intermed_tail->vararg = vararg;
    intermed_tail->valist = valist && valist->type == en_l_p;
    if (sz % chosenAssembler->arch->stackalign)
        sz = sz + chosenAssembler->arch->stackalign - sz % chosenAssembler->arch->stackalign;
    return sz;
}

/*-------------------------------------------------------------------------*/

static int gen_parm(INITLIST* a, SYMBOL* funcsp)
/*
 *      push a list of parameters onto the stack and return the
 *      size of parameters pushed.
 */
{
    int rv;
    if (a->vararg && ((architecture == ARCHITECTURE_MSIL)))
    {
        if (!objectArray_exp)
        {
            TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
            tp->type = bt_pointer;
            tp->size = 0;
            tp->array = tp->msil = true;
            tp->rootType = tp;
            tp->btp = (TYPE*)Alloc(sizeof(TYPE));
            tp->btp->type = bt___object;
            tp->btp->size = getSize(bt_pointer);
            tp->btp->rootType = tp->btp;
            objectArray_exp = SymbolManager::Get(anonymousVar(sc_auto, tp));
            cacheTempSymbol(objectArray_exp->sp);
            objectArray_exp->sp->anonymous = false;
        }
        intermed_tail->varargPrev = true;
    }
    if (ispointer(a->tp) || isref(a->tp))
    {
        TYPE* btp = basetype(a->tp);
        if (btp->vla || basetype(btp->btp)->vla)
        {
            rv = push_stackblock(a->tp, a->exp->left, funcsp, a->tp->size, a->vararg, a->valist ? a->exp : nullptr);
            DumpIncDec(funcsp);
            push_nesting += rv;
            return rv;
        }
    }
    if ((!cparams.prm_cplusplus && isstructured(a->tp)) || ((architecture == ARCHITECTURE_MSIL) && isarray(a->tp) && basetype(a->tp)->msil))
    {
        if (a->exp->type != en_stackblock && (architecture == ARCHITECTURE_MSIL))
        {
            EXPRESSION* exp1 = a->exp;
            while (castvalue(exp1))
                exp1 = exp1->left;

            if (isconstzero(&stdint, exp1))
            {
                IMODE *ap4 = tempreg(-ISZ_UINT, 0), *ap3 = gen_expr(funcsp, exp1, 0, -ISZ_UINT);
                ap3->size = ISZ_OBJECT;  // only time we will set the OBJECT size for an int constant is if it is to be an LDNULL
                gen_icode(i_assn, ap4, ap3, nullptr);
                gen_nodag(i_parm, 0, ap3, 0);
                rv = 1;
            }
            else
            {
                rv = push_param(a->exp, funcsp, a->vararg, a->valist ? a->exp : nullptr, F_OBJECT);
            }
        }
        else
        {
            rv = push_stackblock(a->tp, a->exp->left, funcsp, a->exp->size, a->vararg, a->valist ? a->exp : nullptr);
        }
    }
    else if (a->exp->type == en_stackblock)
    {
        rv = push_stackblock(a->tp, a->exp->left, funcsp, a->tp->size, a->vararg, a->valist ? a->exp : nullptr);
    }
    else if (isstructured(a->tp) && a->exp->type == en_thisref)  // constructor
    {
        EXPRESSION* ths = a->exp->v.t.thisptr;
        if (ths && ths->type == en_auto && ths->v.sp->stackblock)
        {
            IMODE* ap;
            // constructor or other function creating a structure on the stack
            ths = a->exp->left->v.func->thisptr;
            ths->v.sp->stackblock = true;

            rv = ths->v.sp->tp->size;
            if (rv % chosenAssembler->arch->stackalign)
                rv = rv + chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
            gen_icode(i_parmstack, ap = tempreg(ISZ_ADDR, 0), make_immed(ISZ_UINT, rv), nullptr);
            intermed_tail->vararg = a->vararg;
            SymbolManager::Get(ths->v.sp)->imvalue = ap;
            gen_expr(funcsp, a->exp, 0, ISZ_UINT);
        }
        else
        {
            IMODE* ap = gen_expr(funcsp, a->exp, 0, ISZ_ADDR);
            gen_nodag(i_parm, 0, ap, 0);
            intermed_tail->vararg = a->vararg;
            intermed_tail->valist = a->valist ? (a->exp && a->exp->type == en_l_p) : 0;
            rv = a->tp->size;
        }
    }
    else
    {
        rv = push_param(a->exp, funcsp, a->vararg, a->valist ? a->exp : nullptr, 0);
    }
    DumpIncDec(funcsp);
    push_nesting += rv;
    return rv;
}
static int sizeParam(INITLIST* a, SYMBOL* funcsp)
{
    int rv;
    if (ispointer(a->tp) || isref(a->tp) || a->tp->type == bt_func || a->tp->type == bt_ifunc || a->byRef)
        rv = sizeFromISZ(ISZ_ADDR);
    else
        rv = basetype(a->tp)->size;
    if (rv % chosenAssembler->arch->stackalign)
        rv += chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
    return rv;
}
static int genCdeclArgs(INITLIST* args, SYMBOL* funcsp)
{
    int rv = 0;
    if (args)
    {
        rv = genCdeclArgs(args->next, funcsp);
        int n = gen_parm(args, funcsp);
        rv += n;
        stackblockOfs -= n;
        if (args->exp->type == en_auto)
            if (args->exp->v.sp->stackblock)
                args->exp->v.sp->offset = stackblockOfs;
    }
    return rv;
}
static void genCallLab(INITLIST* args, int callLab)
{
    if (args)
    {
        genCallLab(args->next, callLab);
        if (args->exp->type == en_thisref)
        {
            args->exp->v.t.thisptr->xcDest = callLab;
        }
    }
}
static int genPascalArgs(INITLIST* args, SYMBOL* funcsp)
{
    int rv = 0;
    if (args)
    {
        rv = gen_parm(args, funcsp);
        rv += genPascalArgs(args->next, funcsp);
    }
    return rv;
}
static int sizeParams(INITLIST* args, SYMBOL* funcsp)
{
    int rv = 0;
    if (args)
    {
        rv = sizeParam(args, funcsp);
        rv += sizeParams(args->next, funcsp);
    }
    return rv;
}

IMODE* gen_trapcall(SYMBOL* funcsp, EXPRESSION* node, int flags)
{
    /* trap call */
    (void)funcsp;
    (void)flags;
    gen_igosub(i_trap, make_immed(ISZ_UINT, node->v.i));
    return 0;
}
IMODE* gen_stmt_from_expr(SYMBOL* funcsp, EXPRESSION* node, int flags)
{
    IMODE* rv;
    (void)flags;
    /* relies on stmt only being used for inlines */
    rv = genstmt(node->v.stmt, funcsp);
    if (node->left)
        rv = gen_expr(funcsp, node->left, 0, natural_size(node->left));
    return rv;
}
/*-------------------------------------------------------------------------*/

static bool has_arg_destructors(INITLIST* arg)
{
    if (arg)
        return !!arg->dest || has_arg_destructors(arg->next);
    return false;
}
static void gen_arg_destructors(SYMBOL* funcsp, INITLIST* arg)
{
    if (arg)
    {
        gen_arg_destructors(funcsp, arg->next);
        if (arg->dest)
            gen_expr(funcsp, arg->dest, F_NOVALUE, ISZ_UINT);
    }
}
static int MarkFastcall(SYMBOL* sym, TYPE* functp, bool thisptr)
{
#ifndef CPPTHISCALL
    if (sym->attribs.inheritable.linkage != lk_fastcall)
        return 0;
#endif

    if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !functionHasAssembly)
    {
        /* if there is a setjmp in the function, no variable gets moved into a reg */
        if (!(setjmp_used))
        {

            QUAD* tail = intermed_tail;
            int i = 0;
            SYMLIST* hr;
            if (isfunction(functp))
            {
                if (basetype(functp)->sp)
                    sym = basetype(functp)->sp;
                hr = basetype(functp)->syms->table[0];
                if (!ismember(sym) && sym->attribs.inheritable.linkage != lk_fastcall && basetype(sym->tp)->type != bt_memberptr)
                    return 0;
            }
            else
            {
                return 0;
            }

            int structret = !!isstructured(basetype(sym->tp)->btp);

            while (hr && tail && tail->dc.opcode != i_block)
            {

                if (tail->dc.opcode == i_parm)
                {
                    // change it to a move
                    SYMBOL* sp = hr->p;
                    TYPE* tp = basetype(sp->tp);
                    if (thisptr ||
                        ((tp->type < bt_float || (tp->type == bt_pointer && basetype(basetype(tp)->btp)->type != bt_func) ||
                          isref(tp)) &&
                         sp->offset - (chosenAssembler->arch->fastcallRegCount + structret) * chosenAssembler->arch->parmwidth <
                             chosenAssembler->arch->retblocksize))
                    {
                        IMODE* temp = tempreg(tail->dc.left->size, 0);
                        QUAD* q = (QUAD*)(QUAD*)Alloc(sizeof(QUAD));
                        *q = *tail;
                        q->dc.opcode = i_assn;
                        q->ans = temp;
                        tail->dc.left = temp;
                        tail->fastcall = ++i;
                        if (tail->dc.left->size == ISZ_ULONGLONG || tail->dc.left->size == -ISZ_ULONGLONG)
                            ++i;  // long long is presently meant to be the one and only arg...
                        q->back = tail->back;
                        q->fwd = tail;
                        q->back->fwd = q;
                        q->fwd->back = q;
                        q->genConflict = true;
                    }
                    else
                    {
                        break;
                    }
                    if (sym->attribs.inheritable.linkage != lk_fastcall)
                        break;
                    if (thisptr)
                    {
                        if (hr->p->thisPtr)
                            hr = hr->next;
                        thisptr = false;
                    }
                    else
                    {
                        hr = hr->next;
                    }
                }
                else if (tail->dc.opcode == i_gosub)
                {
                    break;
                }
                tail = tail->back;
            }
            return i * chosenAssembler->arch->parmwidth;
        }
    }
    return 0;
}
IMODE* gen_funccall(SYMBOL* funcsp, EXPRESSION* node, int flags)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    int fastcallSize = 0;
    IMODE* ap3;
    FUNCTIONCALL* f = node->v.func;
    bool managed = false;
    IMODE* stobj = nullptr;
    IMODE* ap;
    int adjust = 0;
    int adjust2 = 0;
    QUAD* gosub = nullptr;
    if (!f->ascall)
    {
        return gen_expr(funcsp, f->fcall, 0, ISZ_ADDR);
    }

    if (f->sp->isInline)
    {
        if (f->sp->noinline)
        {
            f->sp->dumpInlineToFile = true;
        }
        else
        {
            ap = gen_inline(funcsp, node, flags);
            if (ap)
                return ap;
        }
    }
    if ((architecture == ARCHITECTURE_MSIL) && (f->sp->attribs.inheritable.linkage2 != lk_unmanaged && msilManaged(f->sp)))
        managed = true;
    if (f->returnEXP && managed && isstructured(basetype(f->functp)->btp))
    {
        switch (f->returnEXP->type)
        {
            case en_auto:
            case en_pc:
            case en_global:
            case en_tempref:
            case en_threadlocal:
                break;
            default:
                stobj = gen_expr(funcsp, f->returnEXP, 0, ISZ_UINT);
                break;
        }
    }
    {
        int n = sizeParams(f->arguments, funcsp);
        int v = sizeFromISZ(ISZ_ADDR);
        if (v % chosenAssembler->arch->stackalign)
            v = v + chosenAssembler->arch->stackalign - v % chosenAssembler->arch->stackalign;
        if (isfunction(f->functp) &&
            (isstructured(basetype(f->functp)->btp) || basetype(basetype(f->functp)->btp)->type == bt_memberptr))
        {
            if (f->returnEXP)
                n += v;
        }
        if (f->thisptr)
        {
            n += v;
        }
        adjust += n;
    }
    if (cparams.prm_stackalign)
    {
        int k = push_nesting + adjust;
        if (k % cparams.prm_stackalign)
        {
            int n = cparams.prm_stackalign - k % cparams.prm_stackalign;
            adjust2 = n;
            adjust += n;
            push_nesting += n;
            // make an instruction for adjustment.
            gen_icode(i_substack, nullptr, make_immed(ISZ_UINT, n), nullptr);
        }
    }
    int cdeclare = stackblockOfs;
    if (f->sp->attribs.inheritable.linkage == lk_pascal)
    {
        if (isstructured(basetype(f->functp)->btp) || basetype(basetype(f->functp)->btp)->type == bt_memberptr)
        {
            if (f->returnEXP)
                push_param(f->returnEXP, funcsp, false, nullptr, F_OBJECT);
        }
        genPascalArgs(f->arguments, funcsp);
    }
    else
    {
        int n = 0;
        if (f->thisptr && f->thisptr->type == en_auto && f->thisptr->v.sp->stackblock)
        {
            EXPRESSION* exp = f->thisptr;
            // constructor or other function creating a structure on the stack
            int rv = exp->v.sp->tp->size;
            if (rv % chosenAssembler->arch->stackalign)
                rv = rv + chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
            gen_icode(i_parmstack, ap = tempreg(ISZ_ADDR, 0), make_immed(ISZ_UINT, rv), nullptr);
            SimpleSymbol* sym = SymbolManager::Get(exp->v.sp);
            sym->imvalue = ap;
            sym->offset = -rv + stackblockOfs;
            cacheTempSymbol(SymbolManager::Get(exp->v.sp));
            genCdeclArgs(f->arguments, funcsp);
            ap3 = gen_expr(funcsp, exp, 0, ISZ_UINT);
            ap = LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, nullptr);
            if (ap->size == ISZ_NONE)
                ap->size = ISZ_ADDR;
            gen_nodag(i_parm, 0, ap, 0);
        }
        else if (chosenAssembler->arch->preferopts & OPT_REVERSEPARAM)
        {
            if (isstructured(basetype(f->functp)->btp) || basetype(basetype(f->functp)->btp)->type == bt_memberptr)
            {
                if (f->returnEXP && !managed)
                    push_param(f->returnEXP, funcsp, false, nullptr, 0);
            }
            if (f->thisptr)
            {
                push_param(f->thisptr, funcsp, false, nullptr, F_OBJECT);
            }
            genPascalArgs(f->arguments, funcsp);
        }
        else
        {
            genCdeclArgs(f->arguments, funcsp);
            if (f->thisptr)
            {
                push_param(f->thisptr, funcsp, false, nullptr, F_OBJECT);
            }
        }
        if (f->callLab == -1)
        {
            f->callLab = ++consIndex;
            genCallLab(f->arguments, f->callLab);
        }
        fastcallSize = MarkFastcall(f->sp, f->functp, !!f->thisptr);
        if (!(chosenAssembler->arch->preferopts & OPT_REVERSEPARAM))
        {
            if (isfunction(f->functp) &&
                (isstructured(basetype(f->functp)->btp) || basetype(basetype(f->functp)->btp)->type == bt_memberptr))
            {
                if (f->returnEXP && !managed)
                    push_param(f->returnEXP, funcsp, false, nullptr, 0);
            }
        }
    }
    gen_icode(i_tag, nullptr, nullptr, nullptr);
    intermed_tail->beforeGosub = true;
    intermed_tail->ignoreMe = true;
    /* named function */
    if (f->fcall->type == en_imode)
    {
        ap = f->fcall->v.imode;
        if (f->callLab && xcexp)
        {
            xcexp->right->v.i = f->callLab;
            gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
        }
        gosub = gen_igosub(node->type == en_intcall ? i_int : i_gosub, ap);
    }
    else
    {
        enum i_ops type = i_gosub;
        if (node->type == en_intcall)
            type = i_int;
        ap = ap3 = gen_expr(funcsp, f->fcall, 0, ISZ_UINT);
        if (ap->mode == i_immed && ap->offset->type == se_pc)
        {
            if (f->sp && f->sp->attribs.inheritable.linkage2 == lk_import && ((architecture != ARCHITECTURE_MSIL)))
            {
                IMODE* ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                *ap1 = *ap;
                ap1->retval = false;
                ap = ap1;
                ap->mode = i_direct;
            }
        }
        else if (f->sp && f->sp->attribs.inheritable.linkage2 == lk_import && f->sp->storage_class != sc_virtual)
        {
            IMODE* ap1 = ap;
            gen_icode(i_assn, ap = tempreg(ISZ_ADDR, 0), ap1, 0);
            ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
            *ap1 = *ap;
            ap1->retval = false;
            ap = ap1;
            //            ap->mode = i_ind;
            ap->mode = i_direct;
        }
        if (f->callLab && xcexp)
        {
            xcexp->right->v.i = f->callLab;
            gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
        }
        gosub = gen_igosub(type, ap);
    }
    if ((architecture == ARCHITECTURE_MSIL))
    {
        bool vaarg = false;
        if (f->sp->name[0] == '_' && !strcmp(f->sp->name, "__va_arg__"))
            vaarg = true;
        gosub->altvararg = f->vararg;
        ArgList **p = &gosub->altargs;
        INITLIST *il = f->arguments;
        while (il)
        {
            *p = (ArgList*)Alloc(sizeof(ArgList));
            (*p)->tp = SymbolManager::Get(il->tp);
            if (vaarg && !lvalue(il->exp) && !castvalue(il->exp))
                (*p)->exp = SymbolManager::Get(il->exp);
            il = il->next;
            p = &(*p)->next;
        }
    }
    gosub->altsp = SymbolManager::Get(f->sp);
    if ((f->sp->storage_class == sc_typedef || f->sp->storage_class == sc_cast) && isfuncptr(f->sp->tp))
    {
        typedefs.push_back(gosub->altsp);
    }
    gosub->fastcall = !!fastcallSize;

    if ((flags & F_NOVALUE) && !isstructured(basetype(f->functp)->btp) && basetype(f->functp)->btp->type != bt_memberptr)
    {
        if (basetype(f->functp)->btp->type == bt_void)
            gosub->novalue = 0;
        else
            gosub->novalue = sizeFromType(basetype(f->functp)->btp);
    }
    else if (isfunction(f->functp) && isstructured(basetype(f->functp)->btp))
    {
        if ((flags & F_NOVALUE) && !managed)
            gosub->novalue = -2;
        else
            gosub->novalue = 0;
    }
    else
    {
        gosub->novalue = -1;
    }
    stackblockOfs = cdeclare;
    /* undo pars and make a temp for the result */
    if (chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
    {
        int n = f->thisptr ? 1 : 0;
        INITLIST* args = f->arguments;
        while (args)
        {
            n++;
            args = args->next;
        }
        if (f->returnEXP && !managed)
            n++;
        gen_nodag(i_parmadj, 0, make_parmadj(n), make_parmadj(!isvoid(basetype(f->functp)->btp)));
    }
    else
    {
        adjust -= fastcallSize;
        if (adjust < 0)
            adjust = 0;
        adjust2 -= fastcallSize;
        if (adjust2 < 0)
            adjust2 = 0;
        if (f->sp->attribs.inheritable.linkage != lk_stdcall && f->sp->attribs.inheritable.linkage != lk_pascal)
            gen_nodag(i_parmadj, 0, make_parmadj(adjust), make_parmadj(adjust));
        else
            gen_nodag(i_parmadj, 0, make_parmadj(adjust2), make_parmadj(adjust));
    }

    push_nesting -= adjust;
    if (f->returnEXP && managed && isfunction(f->functp) && isstructured(basetype(f->functp)->btp))
    {
        if (!(flags & F_INRETURN))
        {
            IMODE* ap1;
            int siz1;
            if (stobj)
            {
                ap1 = (IMODE*)Alloc(sizeof(IMODE));
                *ap1 = *stobj;
                ap1->mode = i_ind;
                ap1->size = ISZ_OBJECT;
                ap1->offset->sp->tp = SymbolManager::Get(basetype(f->returnSP->tp));
            }
            else
            {
                ap1 = gen_expr(funcsp, f->returnEXP, F_OBJECT, ISZ_OBJECT);
            }
            ap = tempreg(ISZ_OBJECT, 0);
            ap->retval = true;
            gen_icode(i_assn, ap1, ap, 0);
            ap = ap1;
        }
        else
        {
            gosub->novalue = -3;
            ap = nullptr;
        }
    }
    else if (!(flags & F_NOVALUE) && isfunction(f->functp) && isarray(basetype(f->functp)->btp))
    {
        IMODE* ap1;
        int siz1;
        if (stobj)
        {
            ap1 = (IMODE*)Alloc(sizeof(IMODE));
            *ap1 = *stobj;
            ap1->mode = i_ind;
            ap1->size = ISZ_OBJECT;
        }
        else
        {
            ap1 = tempreg(ISZ_OBJECT, 0);
        }
        ap1->offset->sp->tp = SymbolManager::Get(basetype(basetype(f->functp)->btp));
        ap = tempreg(ISZ_OBJECT, 0);
        ap->retval = true;
        gen_icode(i_assn, ap1, ap, 0);
        ap = ap1;
    }
    else if (!(flags & F_NOVALUE) && isfunction(f->functp) && !isvoid(basetype(f->functp)->btp))
    {
        /* structures handled by callee... */
        if (!isstructured(basetype(f->functp)->btp) && basetype(f->functp)->btp->type != bt_memberptr)
        {
            IMODE *ap1, *ap2;
            int siz1 = sizeFromType(basetype(f->functp)->btp);
            ap1 = tempreg(siz1, 0);
            ap1->retval = true;
            gen_icode(i_assn, ap = tempreg(siz1, 0), ap1, 0);
        }
        else
        {
            IMODE* ap1;
            ap1 = tempreg(ISZ_ADDR, 0);
            ap1->retval = true;
            gen_icode(i_assn, ap = tempreg(ISZ_ADDR, 0), ap1, 0);
        }
    }
    else
    {
        ap = tempreg(ISZ_ADDR, 0);
        ap->retval = true;
    }
    if (has_arg_destructors(f->arguments))
    {
        IMODE* ap1 = tempreg(ap->size, 0);
        gen_icode(i_assn, ap1, ap, nullptr);
        ap = ap1;
    }
    gen_arg_destructors(funcsp, f->arguments);
    return ap;
}
IMODE* gen_atomic_barrier(SYMBOL* funcsp, ATOMICDATA* ad, IMODE* addr, IMODE* barrier)
{
    IMODE *left, *right;
    if (needsAtomicLockFromType(ad->tp))
    {
        IMODE* right;
        if (barrier)
        {
            left = make_immed(ISZ_UINT, -ad->memoryOrder1->v.i);
        }
        else
        {
            left = make_immed(ISZ_ADDR, ad->memoryOrder1->v.i);
            barrier = tempreg(ISZ_ADDR, 0);
            right = make_immed(ISZ_ADDR, ad->tp->size - ATOMIC_FLAG_SPACE);
            gen_icode(i_add, barrier, addr, right);
        }
        gen_icode(i_atomic_flag_fence, nullptr, left, barrier);
        return barrier;
    }
    else
    {
        if (barrier)
        {
            left = make_immed(ISZ_UINT, -ad->memoryOrder1->v.i);
        }
        else
        {
            left = make_immed(ISZ_UINT, ad->memoryOrder1->v.i);
        }
        gen_icode(i_atomic_fence, nullptr, left, nullptr);
        return (IMODE*)-1;
    }
}
IMODE* gen_atomic(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
{
    IMODE* rv = nullptr;
    switch (node->v.ad->atomicOp)
    {
        IMODE *left, *right;
        IMODE* av;
        IMODE* barrier;
        int sz;
        i_ops op;
        case ao_init:
            sz = sizeFromType(node->v.ad->tp);
            av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            left = indnode(av, sz);
            right = gen_expr(funcsp, node->v.ad->value, 0, sz);
            gen_icode(i_assn, left, right, nullptr);
            if (needsAtomicLockFromType(node->v.ad->tp))
            {
                IMODE* temp = tempreg(ISZ_ADDR, 0);
                gen_icode(i_add, temp, av, make_immed(ISZ_UINT, node->v.ad->tp->size - ATOMIC_FLAG_SPACE));
                temp = indnode(temp, ISZ_UINT);
                gen_icode(i_atomic_flag_clear, nullptr, make_immed(ISZ_UINT, mo_relaxed), temp);
            }
            rv = right;
            break;
        case ao_flag_set_test:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            right = gen_expr(funcsp, node->v.ad->flg, F_STORE, ISZ_UINT);
            rv = tempreg(ISZ_UINT, 0);
            gen_icode(i_atomic_flag_test_and_set, rv, left, right);
            intermed_tail->alwayslive = true;
            break;
        case ao_flag_clear:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            right = gen_expr(funcsp, node->v.ad->flg, F_STORE, ISZ_UINT);
            gen_icode(i_atomic_flag_clear, nullptr, left, right);
            break;
        case ao_fence:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            gen_icode(i_atomic_fence, nullptr, left, nullptr);
            break;
        case ao_load:
            av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            if (isstructured(node->v.ad->tp))
            {
                left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                gen_icode(i_atomic_fence, nullptr, left, nullptr);
                EXPRESSION* exp = anonymousVar(sc_auto, node->v.ad->tp);
                rv = (IMODE*)Alloc(sizeof(IMODE));
                rv->mode = i_immed;
                rv->size = ISZ_ADDR;
                rv->offset = SymbolManager::Get(exp);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                gen_icode(i_assnblock, make_immed(ISZ_UINT, node->v.ad->tp->btp->size), rv, av);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            else
            {
                left = indnode(av, sizeFromType(node->v.ad->tp));
                rv = tempreg(sizeFromType(node->v.ad->tp), 0);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                gen_icode(i_assn, rv, left, nullptr);
                intermed_tail->atomic = true;
                intermed_tail->alwayslive = true;
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            break;
        case ao_store:
            if (isstructured(node->v.ad->tp))
            {
                left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                gen_icode(i_atomic_fence, nullptr, left, nullptr);
                right = gen_expr(funcsp, node->v.ad->value, F_STORE, ISZ_ADDR);
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                gen_icode(i_assnblock, make_immed(ISZ_UINT, node->v.ad->tp->btp->size), av, right);
                rv = right;
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            else
            {
                sz = sizeFromType(node->v.ad->tp);
                right = gen_expr(funcsp, node->v.ad->value, 0, sz);
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                left = indnode(av, sz);
                gen_icode(i_assn, left, right, nullptr);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
                rv = right;
            }
            break;
        case ao_modify:
            if (isstructured(node->v.ad->tp))
            {
                left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                gen_icode(i_atomic_fence, nullptr, left, nullptr);
                // presumed xchg
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                right = gen_expr(funcsp, node->v.ad->value, F_STORE, ISZ_ADDR);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                EXPRESSION* exp = anonymousVar(sc_auto, node->v.ad->tp);
                rv = (IMODE*)Alloc(sizeof(IMODE));
                rv->mode = i_immed;
                rv->size = ISZ_ADDR;
                rv->offset = SymbolManager::Get(exp);
                gen_icode(i_assnblock, make_immed(ISZ_UINT, node->v.ad->tp->btp->size), rv, av);
                gen_icode(i_assnblock, make_immed(ISZ_UINT, node->v.ad->tp->btp->size), av, right);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            else
            {
                switch ((int)node->v.ad->third->v.i)
                {
                    default:
                    case asplus:
                        op = i_add;
                        break;
                    case asminus:
                        op = i_sub;
                        break;
                    case asor:
                        op = i_or;
                        break;
                    case asand:
                        op = i_and;
                        break;
                    case asxor:
                        op = i_eor;
                        break;
                    case assign:
                        op = i_xchg;
                        break;
                }
                sz = sizeFromType(node->v.ad->tp);
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                left = indnode(av, sz);
                right = gen_expr(funcsp, node->v.ad->value, F_STORE, sz);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                rv = tempreg(sz, 0);
                if (needsAtomicLockFromType(node->v.ad->tp))
                {
                    IMODE* tv = tempreg(sz, 0);
                    gen_icode(i_assn, rv, left, nullptr);
                    if (op == i_xchg)
                    {
                        gen_icode(i_assn, tv, right, nullptr);
                    }
                    else
                    {
                        gen_icode(i_assn, tv, rv, nullptr);
                        gen_icode(op, tv, tv, right);
                    }
                    gen_icode(i_assn, left, tv, nullptr);
                }
                else
                {
                    gen_icode(op, rv, left, right);
                    intermed_tail->atomic = true;
                    intermed_tail->alwayslive = true;
                }
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            break;
        case ao_cmpswp:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            gen_icode(i_atomic_fence, nullptr, left, nullptr);
            if (isstructured(node->v.ad->tp))
            {

                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                right = gen_expr(funcsp, node->v.ad->third, 0, ISZ_ADDR);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                rv = tempreg(ISZ_UINT, 0);
                gen_icode(i_assn, rv, make_immed(ISZ_UINT, 0), nullptr);
                int labno = nextLabel++, labno2 = nextLabel++;
                gen_icgoto(i_cmpblock, labno, right, av);
                QUAD* q = intermed_tail;
                while (q->dc.opcode != i_cmpblock)
                    q = q->back;
                q->ans = make_immed(ISZ_UINT, node->v.ad->tp->btp->size);
                left = gen_expr(funcsp, node->v.ad->value, F_VOL, ISZ_ADDR);
                gen_icode(i_assnblock, make_immed(ISZ_UINT, node->v.ad->tp->btp->size), av, left);
                gen_icode(i_assn, rv, make_immed(ISZ_UINT, 1), nullptr);
                gen_igoto(i_goto, labno2);
                gen_label(labno);
                gen_icode(i_assnblock, make_immed(ISZ_UINT, node->v.ad->tp->btp->size), right, av);
                gen_icode(i_assn, rv, make_immed(ISZ_UINT, 0), nullptr);
                gen_label(labno2);
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            else
            {
                left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
                gen_icode(i_atomic_fence, nullptr, left, nullptr);
                sz = sizeFromType(node->v.ad->tp);
                av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
                left = indnode(av, sz);
                right = gen_expr(funcsp, node->v.ad->third, 0, ISZ_ADDR);
                right = indnode(right, sz);
                rv = gen_expr(funcsp, node->v.ad->value, F_VOL, sz);
                barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
                if (needsAtomicLockFromType(node->v.ad->tp))
                {
                    IMODE* temp = tempreg(sz, 0);
                    IMODE* asnfrom = rv;
                    rv = tempreg(ISZ_UINT, 0);
                    gen_icode(i_assn, temp, left, nullptr);
                    int lbno = nextLabel++, lbno2 = nextLabel++;
                    ;
                    gen_icode(i_sete, rv, temp, right);
                    gen_icgoto(i_je, lbno, rv, make_immed(ISZ_UINT, 0));
                    gen_icode(i_assn, temp, asnfrom, nullptr);
                    gen_icode(i_assn, left, temp, nullptr);
                    gen_igoto(i_goto, lbno2);
                    gen_label(lbno);
                    gen_icode(i_assn, right, temp, nullptr);
                    gen_label(lbno2);
                }
                else
                {
                    gen_icode(i_cmpswp, left, rv, right);
                    rv = tempreg(ISZ_UINT, 0);
                    rv->retval = true;
                    av = tempreg(ISZ_UINT, 0);
                    gen_icode(i_assn, av, rv, nullptr);
                    rv = av;
                }
                gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            }
            left = gen_expr(funcsp, node->v.ad->memoryOrder2, 0, ISZ_UINT);
            gen_icode(i_atomic_fence, nullptr, left, nullptr);
            break;
    }
    return rv;
}
/*
IMODE* doatomicFence(SYMBOL* funcsp, EXPRESSION* parent, EXPRESSION* node, IMODE* barrier)
{
    static LIST* lst;
    int start = !barrier;
    if (node && node->isatomic)
    {
        int afImmed = mo_seq_cst;
        if (parent && parent->type == en_assign)
            afImmed |= 128;  // store
        if (node->lockOffset)
        {
            LIST* cur;
            IMODE *t, *q;
            int n = node->lockOffset;
            while (castvalue(node))
                node = node->left;
            if (lvalue(node))
            {
                node = node->left;
            }
            if (!start)
            {
                lst = lst->next;
            }
            cur = lst;
            while (cur)
            {
                if (equalnode((EXPRESSION *)cur->data, node))
                    break;
                cur = cur->next;
            }
            if (!barrier)
            {
                q = gen_expr(funcsp, node, 0, ISZ_ADDR);
                barrier = tempreg(ISZ_ADDR, 0);
                gen_icode(i_add, barrier, q, make_immed(ISZ_UINT, n));
            }
            if (!cur)
            {
                gen_icode(i_atomic_flag_fence, nullptr, make_immed(ISZ_UINT, start ? afImmed : -afImmed), barrier);
            }
            if (start)
            {
                cur = (LIST *)Alloc(sizeof(LIST));
                cur->next = lst;
                lst = cur;
                cur->data = node;
            }
        }
        else
        {
            gen_icode(i_atomic_fence, nullptr, make_immed(ISZ_UINT, start ? afImmed : -afImmed), nullptr);
            barrier = (IMODE*)-1;
        }
    }
    return barrier;
}
*/
/*-------------------------------------------------------------------------*/

IMODE* gen_expr(SYMBOL* funcsp, EXPRESSION* node, int flags, int size)
/*
 *      general expression evaluation. returns the addressing mode
 *      of the result.
 *
 *			The first argument of this and other routines is a target for
 *			assignmennt.  If this is 0 and a target is needed a temp will be
 *			used; otherwise the target will be evaluated annd used.
 */
{
    IMODE *ap1, *ap2, *ap3;
    IMODE* rv;
    IMODE *lbarrier, *rbarrier;
    int lab0;
    int siz1;
    int store = flags & F_STORE;
    SimpleSymbol* sym;

    flags &= ~F_STORE;
    if (node == 0)
    {
        /*        diag("null node in gen_expr.");*/
        return 0;
    }
    while (node->type == en_not_lvalue || node->type == en_lvalue)
        node = node->left;
    //    lbarrier = doatomicFence(funcsp, node, node->left, 0);
    //    rbarrier = doatomicFence(funcsp, nullptr, node->right, 0);
    if (flags & F_NOVALUE)
    {
        if ((architecture == ARCHITECTURE_MSIL))
        {
            switch (node->type)
            {
                case en_autoinc:
                case en_autodec:
                case en_assign:
                case en_func:
                case en_thisref:
                case en_intcall:
                case en_blockassign:
                case en_blockclear:
                case en_void:
                case en__cpblk:
                case en__initblk:
                    break;
                default:
                    gen_nodag(i_expressiontag, 0, 0, 0);
                    intermed_tail->dc.v.label = 1;
                    intermed_tail->ignoreMe = true;
                    break;
            }
        }
    }
    switch (node->type)
    {
        case en_shiftby:
            ap3 = gen_expr(funcsp, node->left, flags, size);
            ap1 = LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, nullptr);
            rv = ap1;
            break;
        case en_x_wc:
            siz1 = ISZ_WCHAR;
            goto castjoin;
        case en_x_c:
            siz1 = -ISZ_UCHAR;
            goto castjoin;
        case en_x_u16:
            siz1 = ISZ_U16;
            goto castjoin;
        case en_x_u32:
            siz1 = ISZ_U32;
            goto castjoin;
        case en_x_bool:
            siz1 = ISZ_BOOLEAN;
            goto castjoin;
        case en_x_bit:
            siz1 = ISZ_BIT;
            goto castjoin;
        case en_x_uc:
            siz1 = ISZ_UCHAR;
            goto castjoin;
        case en_x_s:
            siz1 = -ISZ_USHORT;
            goto castjoin;
        case en_x_us:
            siz1 = ISZ_USHORT;
            goto castjoin;
        case en_x_i:
            siz1 = -ISZ_UINT;
            goto castjoin;
        case en_x_ui:
            siz1 = ISZ_UINT;
            goto castjoin;
        case en_x_inative:
            siz1 = -ISZ_UNATIVE;
            goto castjoin;
        case en_x_unative:
            siz1 = ISZ_UNATIVE;
            goto castjoin;
        case en_x_l:
            siz1 = -ISZ_ULONG;
            goto castjoin;
        case en_x_ul:
            siz1 = ISZ_ULONG;
            goto castjoin;
        case en_x_ll:
            siz1 = -ISZ_ULONGLONG;
            goto castjoin;
        case en_x_ull:
            siz1 = ISZ_ULONGLONG;
            goto castjoin;

        case en_x_string:
            siz1 = ISZ_STRING;
            goto castjoin;
        case en_x_object:
            siz1 = ISZ_OBJECT;
            goto castjoin;

        case en_x_f:
            siz1 = ISZ_FLOAT;
            goto castjoin;
        case en_x_d:
            siz1 = ISZ_DOUBLE;
            goto castjoin;
        case en_x_ld:
            siz1 = ISZ_LDOUBLE;
            goto castjoin;
        case en_x_fi:
            siz1 = ISZ_IFLOAT;
            goto castjoin;
        case en_x_di:
            siz1 = ISZ_IDOUBLE;
            goto castjoin;
        case en_x_ldi:
            siz1 = ISZ_ILDOUBLE;
            goto castjoin;
        case en_x_fc:
            siz1 = ISZ_CFLOAT;
            goto castjoin;
        case en_x_dc:
            siz1 = ISZ_CDOUBLE;
            goto castjoin;
        case en_x_ldc:
            siz1 = ISZ_CLDOUBLE;
            goto castjoin;
        case en_x_fp:
            siz1 = ISZ_FARPTR;
            goto castjoin;
        case en_x_sp:
            siz1 = ISZ_SEG;
            goto castjoin;
        case en_x_p:
            siz1 = ISZ_ADDR;
        castjoin:
            ap3 = gen_expr(funcsp, node->left, flags & ~F_NOVALUE, natural_size(node->left));
            ap1 = LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, nullptr);
            if (ap1->mode == i_immed)
            {
                if (isintconst(ap1->offset))
                {
                    if (siz1 >= ISZ_FLOAT)
                    {
                        FPF f;
                        IntToFloat(&f, natural_size(node->left), ap1->offset->i);
                        ap1 = make_fimmed(siz1, f);
                    }
                }
                else if (isfloatconst(ap1->offset))
                {
                    if (siz1 < ISZ_FLOAT)
                    {
                        ap1 = make_immed(siz1, (long long)(ap1->offset->f));
                    }
                }
            }
            if (ap1->size == siz1)
                ap2 = ap1;
            else
            {
                ap2 = LookupCastTemp(ap1, siz1);
                gen_icode(i_assn, ap2, ap1, nullptr);
                //                gen_icode(i_assn, ap2 = tempreg(siz1, 0), ap1, 0);
            }
            if (ap2->offset)
            {
                ap2->vol = node->left->isvolatile;
                ap2->restricted = node->left->isrestrict;
            }
            rv = ap2;
            break;
        case en_msil_array_access:
        {
            int i;
            ap1 = gen_expr(funcsp, node->v.msilArray->base, 0, ISZ_ADDR);
            ap1->msilObject = true;
            gen_icode(i_parm, 0, ap1, 0);
            for (i = 0; i < node->v.msilArray->count; i++)
            {
                ap1 = gen_expr(funcsp, node->v.msilArray->indices[i], 0, ISZ_ADDR);
                gen_icode(i_parm, 0, ap1, 0);
            }
            if (!(flags & F_STORE))
            {
                TYPE* base = node->v.msilArray->tp;
                while (isarray(base))
                    base = basetype(base)->btp;
                rv = tempreg(isstructured(base) ? ISZ_OBJECT : sizeFromType(base), 0);
                ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                ap1->size = rv->size;
                ap1->mode = i_immed;
                ap1->offset = SymbolManager::Get(node);
                gen_icode(i_assn, rv, ap1, nullptr);
            }
            else
            {
                rv = nullptr;
            }
            break;
        }
        case en_substack:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            gen_icode(i_substack, ap2 = tempreg(ISZ_ADDR, 0), ap1, nullptr);
            rv = ap2;
            break;
        case en_alloca:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            gen_icode(i_substack, ap2 = tempreg(ISZ_ADDR, 0), ap1, nullptr);
            rv = ap2;
            break;
        case en__initblk:
            rv = gen_cpinitblock(node, funcsp, false, flags);
            break;
        case en__cpblk:
            rv = gen_cpinitblock(node, funcsp, true, flags);
            break;
        case en_loadstack:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
            gen_icode(i_loadstack, 0, ap1, 0);
            rv = nullptr;
            break;
        case en_savestack:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
            gen_icode(i_savestack, 0, ap1, 0);
            rv = nullptr;
            break;
        case en_threadlocal:
            ap1 = make_ioffset(node);
            ap1->offset = SymbolManager::Get(node);
            ap2 = LookupLoadTemp(ap1, ap1);
            if (ap1 != ap2)
                gen_icode(i_assn, ap2, ap1, nullptr);
            rv = ap2;
            break;
        case en_auto:
            if (node->v.sp->stackblock)
            {
                rv = SymbolManager::Get(node->v.sp)->imvalue;
                break;
            }
            sym = SymbolManager::Get(node->v.sp);
            if (sym->storage_class == scc_parameter && sym->paramSubstitute)
            {
                return gen_expr(funcsp, sym->paramSubstitute->left, flags, size);
            }
            sym->allocate = true;
            // fallthrough
        case en_pc:
        case en_global:
        case en_absolute:
            sym = SymbolManager::Get(node->v.sp);
            GENREF(sym);
            if (sym->imaddress && (architecture != ARCHITECTURE_MSIL))
            {
                ap1 = sym->imaddress;
            }
            else
            {
                ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                ap1->offset = SymbolManager::Get(node);
                ap1->mode = i_immed;
                if (flags & F_OBJECT)
                    ap1->msilObject = true;
                ap1->size = size;
            }
            if (isarray(node->v.sp->tp) && basetype(node->v.sp->tp)->msil && !store)
                ap1->msilObject = true;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1; /* return reg */
            sym->imaddress = ap1;
            sym->addressTaken = true;
            break;
        case en_labcon:
            ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
            ap1->offset = SymbolManager::Get(node);
            ap1->mode = i_immed;
            ap1->size = size;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1; /* return reg */
            break;
        case en_imode:
            ap2 = (IMODE*)node->left;
            rv = ap2; /* return reg */
            break;
        case en_c_bool:
        case en_c_c:
        case en_c_i:
        case en_c_l:
        case en_c_ll:
        case en_nullptr:
            if (size >= ISZ_FLOAT)
            {
                FPF f;
                f = (long long)node->v.i;
                ap1 = make_fimmed(size, f);
            }
            else
            {
                ap1 = make_immed(size, node->v.i);
                ap1->offset->type = se_i;
            }
            ap1->offset->unionoffset = node->unionoffset;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;

        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_ul:
        case en_c_ui:

        case en_c_ull:
        case en_c_wc:
            if (size >= ISZ_FLOAT)
            {
                FPF f;
                f = (long long)node->v.i;
                ap1 = make_fimmed(size, f);
            }
            else
            {
                ap1 = make_immed(size, node->v.i);
                ap1->offset->type = se_ui;
            }
            ap1->offset->unionoffset = node->unionoffset;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case en_c_string:
            ap1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
            ap1->mode = i_immed;
            ap1->size = ISZ_STRING;
            ap1->offset = SymbolManager::Get(node);
            rv = ap1;
            break;
        case en_c_f:
        case en_c_d:
        case en_c_ld:
            if (size >= ISZ_FLOAT)
            {
                ap1 = make_fimmed(size, node->v.f);
            }
            else
            {
                long long a = (long long)node->v.f;
                ap1 = make_immed(size, a);
            }
            ap1->offset->type = se_f;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            if (size >= ISZ_FLOAT)
            {
                ap1 = make_fimmed(size, node->v.f);
            }
            else
            {
                long long a = (long long)node->v.f;
                ap1 = make_immed(size, a);
            }
            ap1->offset->type = se_fi;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            ap1 = (IMODE*)Alloc(sizeof(IMODE));
            ap1->mode = i_immed;
            ap1->offset = SymbolManager::Get(node);
            switch (node->type)
            {
                case en_c_fc:
                    ap1->size = ISZ_CFLOAT;
                    break;
                case en_c_dc:
                    ap1->size = ISZ_CDOUBLE;
                    break;
                case en_c_ldc:
                    ap1->size = ISZ_CLDOUBLE;
                    break;
                default:
                    break;
            }
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, nullptr);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case en_l_bool:
        case en_l_wc:
        case en_l_c:
        case en_l_s:
        case en_l_uc:
        case en_l_us:
        case en_l_u16:
        case en_l_u32:
        case en_l_l:
        case en_l_i:
        case en_l_ui:
        case en_l_inative:
        case en_l_unative:
        case en_l_p:
        case en_l_ref:
        case en_l_ul:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_ll:
        case en_l_ull:
        case en_l_fi:
        case en_l_fc:
        case en_l_di:
        case en_l_dc:
        case en_l_ldi:
        case en_l_ldc:
        case en_l_fp:
        case en_l_sp:
        case en_l_bit:
        case en_l_string:
            ap1 = gen_deref(node, funcsp, flags | store);
            rv = ap1;
            break;
        case en_l_object:
            ap1 = gen_deref(node, funcsp, flags | store);
            ap1->offset->sp->tp = SymbolManager::Get(node->v.tp);
            rv = ap1;
            break;
        case en_bits:
            size = natural_size(node->left);
            ap3 = gen_expr(funcsp, node->left, 0, size);
            ap1 = LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, nullptr);
            ap1 = make_bf(node, ap1, size);
            rv = ap1;
            break;
        case en_uminus:
            rv = gen_unary(funcsp, node, flags, size, i_neg);
            break;
        case en_compl:
            rv = gen_unary(funcsp, node, flags, size, i_not);
            break;
        case en_add:
            rv = gen_binary(funcsp, node, flags, size, i_add);
            break;
        case en_sub:
            rv = gen_binary(funcsp, node, flags, size, i_sub);
            break;
        case en_and:
            rv = gen_binary(funcsp, node, flags, size, i_and);
            break;
        case en_or:
            rv = gen_binary(funcsp, node, flags, size, i_or);
            break;
        case en_xor:
            rv = gen_binary(funcsp, node, flags, size, i_eor);
            break;
        case en_arraymul:
            rv = gen_pmul(funcsp, node, flags, size);
            break;
        case en_arraydiv:
            rv = gen_pdiv(funcsp, node, flags, size);
            break;
        case en_mul:
            rv = gen_binary(funcsp, node, flags, size, i_mul);
            break;
        case en_umul:
            rv = gen_binary(funcsp, node, flags, size, i_mul);
            break;
        case en_div:
            rv = gen_sdivide(funcsp, node, flags, size, i_sdiv, false);
            break;
        case en_udiv:
            rv = gen_udivide(funcsp, node, flags, size, i_udiv, false);
            break;
        case en_mod:
            rv = gen_sdivide(funcsp, node, flags, size, i_smod, true);
            break;
        case en_umod:
            rv = gen_udivide(funcsp, node, flags, size, i_umod, true);
            break;
        case en_lsh:
            rv = gen_binary(funcsp, node, flags, size, i_lsl);
            break;
        case en_arraylsh:
            rv = gen_binary(funcsp, node, flags, size, /*i_arraylsh*/ i_lsl);
            break;
        case en_rshd:
            rv = gen_asrhd(funcsp, node, flags, size, i_asr);
            break;
        case en_rsh:
            rv = gen_binary(funcsp, node, flags, size, i_asr);
            break;
        case en_ursh:
            rv = gen_binary(funcsp, node, flags, size, i_lsr);
            break;
        case en_assign:
            rv = gen_assign(funcsp, node, flags, size);
            break;
        case en_blockassign:
            rv = gen_moveblock(node, funcsp);
            break;
        case en_blockclear:
            rv = gen_clearblock(node, funcsp);
            break;
        case en_autoinc:
            rv = gen_aincdec(funcsp, node, flags, size, i_add);
            break;
        case en_autodec:
            rv = gen_aincdec(funcsp, node, flags, size, i_sub);
            break;
        case en_land:
        case en_lor:
        case en_eq:
        case en_ne:
        case en_lt:
        case en_le:
        case en_gt:
        case en_ge:
        case en_ult:
        case en_ule:
        case en_ugt:
        case en_uge:
        case en_not:
        case en_mp_compare:
        case en_mp_as_bool:
            ap1 = gen_relat(node, funcsp);
            rv = ap1;
            break;
        case en_atomic:
            ap1 = gen_atomic(funcsp, node, flags, size);
            rv = ap1;
            break;
        case en_cond:
            ap1 = gen_hook(funcsp, node, flags, size);
            rv = ap1;
            break;
        case en_void:
        {
            EXPRESSION* search = node;
            while (search && search->type == en_void)
            {
                gen_void(search->left, funcsp);
                search = search->right;
            }
            ap1 = gen_expr(funcsp, search, flags, size);
            rv = ap1;
        }
        break;
        case en_literalclass:
            gen_void(node->left, funcsp);
            ap1 = make_immed(size, 0);
            rv = ap1;
            break;
        case en_thisref:
            if (node->dest && xcexp)
            {
                node->v.t.thisptr->xcDest = ++consIndex;
                xcexp->right->v.i = consIndex;
                gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
            }
            ap1 = gen_expr(funcsp, node->left, flags, size);
            if (!node->dest && xcexp)
            {
                node->v.t.thisptr->xcInit = ++consIndex;
                xcexp->right->v.i = consIndex;
                gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
            }
            if (node->left->type == en_stmt)
            {
                rv = tempreg(ISZ_ADDR, 0);
                rv->retval = true;
            }
            else
            {
                rv = ap1;
            }
            break;
        case en_structadd:
            if (node->right->type == en_structelem)
            {
                // prepare for the MSIL ldflda instruction
                IMODE *aa1, *aa2;
                aa1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
                aa1->msilObject = true;
                //                if (!aa1->offset || aa1->mode != i_direct || aa1->offset->type != en_tempref)
                {
                    aa2 = tempreg(aa1->size, 0);
                    gen_icode(i_assn, aa2, aa1, nullptr);
                }
                aa1 = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                aa1->offset = SymbolManager::Get(node->right);
                aa1->mode = i_immed;
                aa1->size = aa2->size;
                ap1 = LookupExpression(i_add, aa2->size, aa2, aa1);
                rv = ap1;
                break;
            }
            else
                rv = gen_binary(funcsp, node, flags, ISZ_ADDR, /*i_struct*/ i_add);
            break;
        case en_arrayadd:
            rv = gen_binary(funcsp, node, flags, ISZ_ADDR, /*i_arrayindex*/ i_add);
            break;
            /*		case en_array:
                        rv = gen_binary( funcsp, node,flags,ISZ_ADDR,i_array);
            */
        case en_voidnz:
            lab0 = nextLabel++;
            falsejp(node->left->left, funcsp, lab0);
            gen_void(node->left->right, funcsp);
            gen_label(lab0);
            ap3 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
            ap1 = LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, nullptr);
            rv = ap1;
            break;
        case en_trapcall:
            rv = gen_trapcall(funcsp, node, flags);
            break;
        case en_func:
        case en_intcall:
            rv = gen_funccall(funcsp, node, flags);
            break;
        case en_stmt:
            rv = gen_stmt_from_expr(funcsp, node, flags);
            break;
        case en_templateparam:
            rv = gen_expr(funcsp, node->v.sp->tp->templateParam->p->byNonType.val, 0, ISZ_UINT);
            break;
        case en_const:
            /* should never get here unless the constant optimizer is turned off */
            ap1 = gen_expr(funcsp, node->v.sp->init->exp, 0, 0);
            //            ap1 = make_immed(natural_size(node), node->v.sp->value.i);
            rv = ap1;
            break;
        default:
            /* explicitly uncoded */
            diag("uncoded node in gen_expr.");
            rv = 0;
    }
    if (flags & F_SWITCHVALUE)
    {
        DumpIncDec(funcsp);
    }
    if (flags & F_NOVALUE)
    {
        if ((architecture == ARCHITECTURE_MSIL))
        {
            switch (node->type)
            {
                case en_autoinc:
                case en_autodec:
                case en_assign:
                case en_func:
                case en_thisref:
                case en_intcall:
                case en_blockassign:
                case en_blockclear:
                case en_void:
                case en__cpblk:
                case en__initblk:
                    break;
                default:
                    gen_nodag(i_expressiontag, 0, 0, 0);
                    intermed_tail->dc.v.label = 0;
                    intermed_tail->ignoreMe = true;
                    break;
            }
        }
        DumpIncDec(funcsp);
    }
    //    doatomicFence(funcsp, nullptr, node->right, rbarrier);
    //    doatomicFence(funcsp, nullptr, node->left, lbarrier);
    return rv;
}

/*-------------------------------------------------------------------------*/

IMODE* gen_void(EXPRESSION* node, SYMBOL* funcsp)
{
    if (node->type != en_auto)
        gen_expr(funcsp, node, F_NOVALUE, natural_size(node));
    return 0;
}

/*-------------------------------------------------------------------------*/

int natural_size(EXPRESSION* node)
/*
 *      return the natural evaluation size of a node.
 */
{
    int siz0, siz1;
    if (node == 0)
    {
        return 0;
    }
    switch (node->type)
    {
        case en_thisshim:
            return ISZ_ADDR;
        case en_msil_array_access:
            return sizeFromType(node->v.msilArray->tp);
        case en_stmt:
            return natural_size(node->left);
        case en_funcret:
            while (node->type == en_funcret)
                node = node->left;
        case en_func:
        case en_intcall:
            if (!node->v.func->functp || !isfunction(node->v.func->functp))
                return 0;
            if (isstructured(basetype(node->v.func->functp)->btp) || basetype(node->v.func->functp)->btp->type == bt_memberptr)
                return ISZ_ADDR;
            else if (node->v.func->ascall)
            {
                if (isarray(basetype(node->v.func->sp->tp)->btp))
                    return ISZ_OBJECT;
                return sizeFromType(basetype(node->v.func->functp)->btp);
            }
            else
                return ISZ_ADDR;
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_nullptr:
            return ISZ_ADDR;
        case en__initblk:
        case en__cpblk:
            return ISZ_NONE;
        case en_bits:
        case en_shiftby:
            return natural_size(node->left);
            ;
        case en_c_c:
        case en_l_c:
        case en_x_c:
            return -ISZ_UCHAR;
        case en_c_uc:
        case en_l_uc:
        case en_x_uc:
            return ISZ_UCHAR;
        case en_c_u16:
        case en_l_u16:
        case en_x_u16:
            return ISZ_U16;
        case en_c_u32:
        case en_l_u32:
        case en_x_u32:
            return ISZ_U32;
        case en_c_s:
        case en_l_s:
        case en_x_s:
            return -ISZ_USHORT;
        case en_l_wc:
        case en_x_wc:
        case en_c_wc:
        case en_c_us:
        case en_l_us:
        case en_x_us:
            return ISZ_USHORT;
        case en_c_l:
        case en_l_l:
        case en_x_l:
            return -ISZ_ULONG;
        case en_c_i:
        case en_l_i:
        case en_x_i:
        case en_structelem:
            return -ISZ_UINT;
        case en_c_ul:
        case en_l_ul:
        case en_x_ul:
            return ISZ_ULONG;
        case en_c_ui:
        case en_l_ui:
        case en_x_ui:
            return ISZ_UINT;
        case en_x_inative:
        case en_l_inative:
            return -ISZ_UNATIVE;
        case en_x_unative:
        case en_l_unative:
            return ISZ_UNATIVE;
        case en_c_d:
        case en_l_d:
        case en_x_d:
            return ISZ_DOUBLE;
        case en_c_ld:
        case en_l_ld:
        case en_x_ld:
            return ISZ_LDOUBLE;
        case en_l_f:
        case en_c_f:
        case en_x_f:
            return ISZ_FLOAT;
        case en_l_fi:
        case en_c_fi:
        case en_x_fi:
            return ISZ_IFLOAT;
        case en_l_di:
        case en_c_di:
        case en_x_di:
            return ISZ_IDOUBLE;
        case en_l_ldi:
        case en_c_ldi:
        case en_x_ldi:
            return ISZ_ILDOUBLE;
        case en_l_fc:
        case en_c_fc:
        case en_x_fc:
            return ISZ_CFLOAT;
        case en_l_dc:
        case en_c_dc:
        case en_x_dc:
            return ISZ_CDOUBLE;
        case en_l_ldc:
        case en_c_ldc:
        case en_x_ldc:
            return ISZ_CLDOUBLE;
        case en_c_bool:
            return ISZ_BOOLEAN;
        case en_c_ull:
        case en_l_ull:
        case en_x_ull:
            return ISZ_ULONGLONG;
        case en_c_ll:
        case en_l_ll:
        case en_x_ll:
            return -ISZ_ULONGLONG;
        case en_imode:
            return ISZ_ADDR;
        case en_c_string:
        case en_l_string:
        case en_x_string:
            return ISZ_STRING;
        case en_l_object:
        case en_x_object:
            return ISZ_OBJECT;
        case en_trapcall:
        case en_auto:
        case en_pc:
        case en_threadlocal:
        case en_global:
        case en_absolute:
        case en_labcon:
            return ISZ_ADDR;
        case en_tempref:
            return ISZ_UINT;
        case en_l_p:
        case en_x_p:
        case en_l_ref:
            return ISZ_ADDR;
        case en_x_fp:
        case en_l_fp:
            return ISZ_FARPTR;
        case en_x_sp:
        case en_l_sp:
            return ISZ_SEG;
        case en_l_bool:
        case en_x_bool:
            return ISZ_BOOLEAN;
        case en_l_bit:
        case en_x_bit:
            return ISZ_BIT;
        case en_compl:
        case en_uminus:
        case en_assign:
        case en_autoinc:
        case en_autodec:
        case en_blockassign:
        case en_stackblock:
        case en_blockclear:
        case en_not_lvalue:
        case en_lvalue:
            return natural_size(node->left);
        case en_arrayadd:
        case en_structadd:
        case en_add:
        case en_sub:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_arraymul:
        case en_mul:
        case en_div:
        case en_arraydiv:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
            siz0 = natural_size(node->left);
            siz1 = natural_size(node->right);
            if (chksize(siz1, siz0))
                return siz1;
            else
                return siz0;
        case en_rshd:
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_ursh:
        case en_argnopush:
        case en_thisref:
            return natural_size(node->left);
            /*		case en_array:
                        return ISZ_ADDR ;
            */
        case en_eq:
        case en_ne:
        case en_lt:
        case en_le:
        case en_gt:
        case en_ge:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_land:
        case en_lor:
        case en_not:
        case en_mp_compare:
        case en_mp_as_bool:
            return -ISZ_UINT;
        case en_literalclass:
            return -ISZ_UINT;
        case en_void:
            while (node->type == en_void && node->right)
                node = node->right;
            if (node->type == en_void)
                return 0;
            else
                return natural_size(node);
        case en_cond:
            return natural_size(node->right);
        case en_atomic:
            return -ISZ_UINT;
        case en_voidnz:
            return natural_size(node->right);
        case en_const:
            return sizeFromType(node->v.sp->tp);
        case en_templateparam:  // may get this during the initial parsing of the template
        case en_templateselector:
            return -ISZ_UINT;
        default:
            diag("natural size error.");
            break;
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

void gen_compare(EXPRESSION* node, SYMBOL* funcsp, i_ops btype, int label)
/*
 *      generate code to do a comparison of the two operands of
 *      node.
 */
{
    IMODE *ap1, *ap2, *ap3;
    IMODE* barrier;
    int siz0, siz1, size;
    siz0 = natural_size(node->left);
    siz1 = natural_size(node->right);
    if (chksize(siz1, siz0))
        size = siz1;
    else
        size = siz0;
    // the ordering here is to accomodate the x86 FP stack
    if (chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        ap3 = gen_expr(funcsp, node->left, F_COMPARE | F_OBJECT, size);
    else
        ap3 = gen_expr(funcsp, node->right, F_COMPARE | F_OBJECT, size);
    ap2 = LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
    {
        //        if (node->right->isatomic)
        //        {
        //            barrier = doatomicFence(funcsp, nullptr, node->right, nullptr);
        //        }
        gen_icode(i_assn, ap2, ap3, nullptr);
        //        if (node->right->isatomic)
        //        {
        //            doatomicFence(funcsp, nullptr, node->right, barrier);
        //        }
    }
    if (chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        ap3 = gen_expr(funcsp, node->right, F_COMPARE, size);
    else
        ap3 = gen_expr(funcsp, node->left, F_COMPARE, size);
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
    {
        //        if (node->left->isatomic)
        //        {
        //            barrier = doatomicFence(funcsp, nullptr, node->left, nullptr);
        //        }
        gen_icode(i_assn, ap1, ap3, nullptr);
        //        if (node->left->isatomic)
        //        {
        //            doatomicFence(funcsp, nullptr, node->left, barrier);
        //        }
    }
    if (incdecList)
    {
        // this is needed for the optimizer...  the incdec confuses it
        if (ap1->mode != i_immed)
        {
            ap3 = tempreg(ap1->size, 0);
            gen_icode(i_assn, ap3, ap1, nullptr);
            ap1 = ap3;
        }
        if (ap2->mode != i_immed)
        {
            ap3 = tempreg(ap2->size, 0);
            gen_icode(i_assn, ap3, ap2, nullptr);
            ap2 = ap3;
        }
        DumpIncDec(funcsp);
        DumpLogicalDestructors(node, funcsp);
    }
    if (chosenAssembler->arch->preferopts & OPT_REVERSESTORE)
        gen_icgoto(btype, label, ap2, ap1);
    else
        gen_icgoto(btype, label, ap1, ap2);
}

/*-------------------------------------------------------------------------*/

IMODE* gen_set(EXPRESSION* node, SYMBOL* funcsp, i_ops btype)
{
    IMODE *ap1, *ap2, *ap3;
    int siz0, siz1, size;
    siz0 = natural_size(node->left);
    siz1 = natural_size(node->right);
    if (chksize(siz1, siz0))
        size = siz1;
    else
        size = siz0;
    ap3 = gen_expr(funcsp, node->left, 0, size);
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, nullptr);
    ap3 = gen_expr(funcsp, node->right, 0, size);
    ap2 = LookupLoadTemp(nullptr, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, nullptr);
    ap3 = tempreg(ISZ_UINT, 0);
    gen_icode(btype, ap3, ap1, ap2);

    return ap3;
}

/*-------------------------------------------------------------------------*/

IMODE* defcond(EXPRESSION* node, SYMBOL* funcsp)
{
    IMODE *ap1, *ap2, *ap3;
    int size = natural_size(node);
    ap3 = gen_expr(funcsp, node->left, 0, size);
    ap1 = LookupLoadTemp(nullptr, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, nullptr);
    ap2 = make_immed(ap1->size, 0);
    ap3 = tempreg(ISZ_UINT, 0);
    gen_icode(i_sete, ap3, ap2, ap1);
    return ap3;
}

/*-------------------------------------------------------------------------*/

static IMODE* truerelat(EXPRESSION* node, SYMBOL* funcsp)
{
    IMODE *ap1 = nullptr, *ap3;
    int lab0, lab1;
    int siz1, siz2;
    if (node == 0)
        return 0;
    switch (node->type)
    {
        case en_eq:
            ap1 = gen_set(node, funcsp, i_sete);
            break;
        case en_ne:
            ap1 = gen_set(node, funcsp, i_setne);
            break;
        case en_lt:
            ap1 = gen_set(node, funcsp, i_setl);
            break;
        case en_le:
            ap1 = gen_set(node, funcsp, i_setle);
            break;
        case en_gt:
            ap1 = gen_set(node, funcsp, i_setg);
            break;
        case en_ge:
            ap1 = gen_set(node, funcsp, i_setge);
            break;
        case en_ult:
            ap1 = gen_set(node, funcsp, i_setc);
            break;
        case en_ule:
            ap1 = gen_set(node, funcsp, i_setbe);
            break;
        case en_ugt:
            ap1 = gen_set(node, funcsp, i_seta);
            break;
        case en_uge:
            ap1 = gen_set(node, funcsp, i_setnc);
            break;
        case en_not:
            ap1 = defcond(node, funcsp);
            break;
        case en_mp_compare:
        case en_mp_as_bool:
            ap1 = tempreg(ISZ_UINT, 0);
            lab0 = nextLabel++;
            lab1 = nextLabel++;
            truejp(node, funcsp, lab0);
            ap3 = make_immed(ISZ_UINT, 0);
            gen_icode(i_assn, ap1, ap3, nullptr);
            intermed_tail->hook = true;
            gen_igoto(i_goto, lab1);
            gen_label(lab0);
            ap3 = make_immed(ISZ_UINT, 1);
            gen_icode(i_assn, ap1, ap3, nullptr);
            intermed_tail->hook = true;
            gen_label(lab1);
            break;
        default:
            diag("True-relat error");
            break;
    }
    //    ap1->size = ISZ_UCHAR;
    return ap1;
}

/*-------------------------------------------------------------------------*/

IMODE* gen_relat(EXPRESSION* node, SYMBOL* funcsp)
{
    long lab1, lab2;
    IMODE* ap1;
    if (node->type != en_land && node->type != en_lor)
    {
        ap1 = truerelat(node, funcsp);
    }
    else
    {
        ap1 = tempreg(ISZ_UINT, 0);
        ap1->offset->sp->pushedtotemp = true;  // don't lazily optimize this temp...
        lab1 = nextLabel++, lab2 = nextLabel++;
        truejp(node, funcsp, lab1);
        gen_iiconst(ap1, 0);
        intermed_tail->hook = true;
        gen_igoto(i_goto, lab2);
        gen_label(lab1);
        gen_iiconst(ap1, 1);
        intermed_tail->hook = true;
        gen_label(lab2);
    }
    return ap1;
}

/*-------------------------------------------------------------------------*/

void truejp(EXPRESSION* node, SYMBOL* funcsp, int label)
/*
 *      generate a jump to label if the node passed evaluates to
 *      a true condition.
 */
{
    IMODE *ap1, *ap2, *ap3, *ap4;
    int siz1, siz2;
    int lab0;
    int i;
    if (node == 0)
        return;
    switch (node->type)
    {
        case en_eq:
            gen_compare(node, funcsp, i_je, label);
            break;
        case en_ne:
            gen_compare(node, funcsp, i_jne, label);
            break;
        case en_lt:
            gen_compare(node, funcsp, i_jl, label);
            break;
        case en_le:
            gen_compare(node, funcsp, i_jle, label);
            break;
        case en_gt:
            gen_compare(node, funcsp, i_jg, label);
            break;
        case en_ge:
            gen_compare(node, funcsp, i_jge, label);
            break;
        case en_ult:
            gen_compare(node, funcsp, i_jc, label);
            break;
        case en_ule:
            gen_compare(node, funcsp, i_jbe, label);
            break;
        case en_ugt:
            gen_compare(node, funcsp, i_ja, label);
            break;
        case en_uge:
            gen_compare(node, funcsp, i_jnc, label);
            break;
        case en_land:
            lab0 = nextLabel++;
            falsejp(node->left, funcsp, lab0);
            truejp(node->right, funcsp, label);
            gen_label(lab0);
            break;
        case en_lor:
            truejp(node->left, funcsp, label);
            truejp(node->right, funcsp, label);
            break;
        case en_not:
            falsejp(node->left, funcsp, label);
            break;
        case en_mp_compare:
            lab0 = nextLabel++;
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            ap2 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
            ap1 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap1, ap2, 0);
            DumpIncDec(funcsp);
            DumpLogicalDestructors(node, funcsp);
            for (i = 0; i < siz1 / siz2 - 1; i++)
            {
                ap4 = indnode(ap3, ap3->size);
                ap2 = indnode(ap1, ap1->size);
                gen_icgoto(i_jne, lab0, ap4, ap2);
                gen_icode(i_add, ap3, ap3, make_immed(ap3->size, siz2));
                gen_icode(i_add, ap1, ap1, make_immed(ap3->size, siz2));
            }
            ap4 = indnode(ap3, ap3->size);
            ap2 = indnode(ap1, ap1->size);
            gen_icgoto(i_je, label, ap4, ap2);
            gen_label(lab0);
            break;
        case en_mp_as_bool:
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr(funcsp, node->left, F_ADDR, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            DumpIncDec(funcsp);
            DumpLogicalDestructors(node, funcsp);
            for (i = 0; i < siz1 / siz2; i++)
            {
                ap2 = indnode(ap3, ap3->size);
                gen_icgoto(i_jne, label, ap2, make_immed(ap3->size, 0));
                if (i < siz1 / siz2 - 1)
                    gen_icode(i_add, ap3, ap3, make_immed(ap3->size, siz2));
            }
            break;
        default:
            siz1 = natural_size(node);
            ap3 = gen_expr(funcsp, node, F_COMPARE, siz1);
            ap1 = LookupLoadTemp(nullptr, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, nullptr);
            DumpIncDec(funcsp);
            DumpLogicalDestructors(node, funcsp);
            gen_icgoto(i_jne, label, ap1, make_immed(ap1->size, 0));
            break;
    }
}

/*-------------------------------------------------------------------------*/

void falsejp(EXPRESSION* node, SYMBOL* funcsp, int label)
/*
 *      generate code to execute a jump to label if the expression
 *      passed is false.
 */
{
    IMODE *ap, *ap1, *ap2, *ap3, *ap4;
    int siz1, siz2;
    int lab0;
    int i;
    if (node == 0)
        return;
    switch (node->type)
    {
        case en_eq:
            gen_compare(node, funcsp, i_jne, label);
            break;
        case en_ne:
            gen_compare(node, funcsp, i_je, label);
            break;
        case en_lt:
            gen_compare(node, funcsp, i_jge, label);
            break;
        case en_le:
            gen_compare(node, funcsp, i_jg, label);
            break;
        case en_gt:
            gen_compare(node, funcsp, i_jle, label);
            break;
        case en_ge:
            gen_compare(node, funcsp, i_jl, label);
            break;
        case en_ult:
            gen_compare(node, funcsp, i_jnc, label);
            break;
        case en_ule:
            gen_compare(node, funcsp, i_ja, label);
            break;
        case en_ugt:
            gen_compare(node, funcsp, i_jbe, label);
            break;
        case en_uge:
            gen_compare(node, funcsp, i_jc, label);
            break;
        case en_land:
            falsejp(node->left, funcsp, label);
            falsejp(node->right, funcsp, label);
            break;
        case en_lor:
            lab0 = nextLabel++;
            truejp(node->left, funcsp, lab0);
            falsejp(node->right, funcsp, label);
            gen_label(lab0);
            break;
        case en_not:
            truejp(node->left, funcsp, label);
            break;
        case en_mp_compare:
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr(funcsp, node->left, 0, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            ap2 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
            ap1 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap1, ap2, 0);
            DumpIncDec(funcsp);
            DumpLogicalDestructors(node, funcsp);
            for (i = 0; i < siz1 / siz2; i++)
            {
                ap4 = indnode(ap3, ap3->size);
                ap2 = indnode(ap1, ap1->size);
                gen_icgoto(i_jne, label, ap4, ap2);
                if (i < siz1 / siz2 - 1)
                {
                    gen_icode(i_add, ap3, ap3, make_immed(ap3->size, siz2));
                    gen_icode(i_add, ap1, ap1, make_immed(ap3->size, siz2));
                }
            }
            break;
        case en_mp_as_bool:
            lab0 = nextLabel++;
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr(funcsp, node->left, F_ADDR, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            DumpIncDec(funcsp);
            DumpLogicalDestructors(node, funcsp);
            for (i = 0; i < siz1 / siz2 - 1; i++)
            {
                ap2 = indnode(ap3, ap3->size);
                gen_icgoto(i_jne, lab0, ap2, make_immed(ap3->size, 0));
                gen_icode(i_add, ap3, ap3, make_immed(ap3->size, siz2));
            }
            gen_icgoto(i_je, label, ap2, make_immed(ap3->size, 0));
            gen_label(lab0);
            break;
        default:
            siz1 = natural_size(node);
            ap3 = gen_expr(funcsp, node, F_COMPARE, siz1);
            ap = LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, nullptr);
            DumpIncDec(funcsp);
            DumpLogicalDestructors(node, funcsp);
            gen_icgoto(i_je, label, ap, make_immed(ap->size, 0));
            break;
    }
}
