/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
 
/*
 *      this module contains all of the code generation routines
 *      for evaluating expressions and conditions.
 */
extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;
extern int retlab;
extern int nextLabel;
extern SYMBOL *declclass;
extern QUAD *intermed_head, *intermed_tail;
extern int lineno;
extern SYMBOL *inlinesp_list[];
extern int inlinesp_count;
extern BLOCK *currentBlock;
extern BLOCKLIST *blocktail;
extern int catchLevel;
extern EXPRESSION *xcexp;
extern int consIndex;
extern int inlinesym_count;
extern EXPRESSION *inlinesym_thisptr[MAX_INLINE_NESTING];
extern LIST *temporarySymbols;
extern TYPE stdpointer;

int calling_inline;

IMODE *inlinereturnap;
IMODE *structret_imode;
LIST *immed_list[4091];

SYMBOL *inlinesp_list[MAX_INLINE_NESTING];
int inlinesp_count;

static STORETEMPHASH *storeHash[DAGSIZE];
static STORETEMPHASH *loadHash[DAGSIZE];
static STORETEMPHASH *immedHash[DAGSIZE];
static CASTTEMPHASH *castHash[DAGSIZE];
static DAGLIST *name_value_hash[DAGSIZE];
static LIST *incdecList;
static LIST *incdecListLast;
static int push_nesting;
static EXPRESSION *this_bound;
static int inline_level;

IMODE *gen_expr(SYMBOL *funcsp, EXPRESSION *node, int flags, int size);
IMODE *gen_void(EXPRESSION *node, SYMBOL *funcsp);
IMODE *gen_relat(EXPRESSION *node, SYMBOL *funcsp);
void truejp(EXPRESSION *node, SYMBOL *funcsp, int label);
void falsejp(EXPRESSION *node, SYMBOL *funcsp, int label);

void iexpr_init(void)
{
    calling_inline = 0;
    inlinesp_count = 0;
    push_nesting = 0;
    this_bound = 0;
    memset(&immed_list, 0, sizeof(immed_list));
    inline_level = 0;
}
void iexpr_func_init(void)
{
    memset(name_value_hash, 0, sizeof(DAGLIST *) * DAGSIZE);
    memset(storeHash, 0 , sizeof(storeHash));
    memset(loadHash, 0 , sizeof(loadHash));
    memset(immedHash, 0 , sizeof(immedHash));
    memset(castHash, 0, sizeof(castHash));
    incdecList = NULL;
}
void DumpIncDec(SYMBOL *funcsp)
{
    LIST *l = incdecList;
    incdecList = NULL;
    incdecListLast = incdecList;
    while (l)
    {
        gen_void((EXPRESSION *)l->data, funcsp);
        l = l->next;
    }
}
IMODE *LookupExpression(enum i_ops op, int size, IMODE *left, IMODE *right)
{
    IMODE *ap = NULL;
    QUAD head ;
    memset(&head, 0, sizeof(head));
    head.dc.left = left;
    head.dc.right = right;
    head.dc.opcode = op;
    if (!left->bits && (!right || !right->bits))
        ap = (IMODE *)LookupNVHash((UBYTE *)&head, DAGCOMPARE, name_value_hash);
    if (!ap)
    {	
        BOOLEAN vol = FALSE;
        BOOLEAN rest = FALSE;
        int pragmas = 0;
         ap = tempreg(size, 0);
        gen_icode(op, ap, left, right);
        if (!left->offset->unionoffset && (!right || !right->offset->unionoffset))
            if (!left->bits && (!right || !right->bits))
                ReplaceHash((QUAD *)ap, (UBYTE *)intermed_tail, DAGCOMPARE, name_value_hash);
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
        else
            if (right && right->offset)
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
IMODE *GetStoreTemp(IMODE *dest)
{
    int hash = dhash((UBYTE *)&dest, sizeof(dest));
    STORETEMPHASH *sh = storeHash[hash];
    while (sh)
    {
        if (sh->mem == dest)
        {
            return sh->temp;
        }
        sh = sh->next;
    }
    return NULL;
}
IMODE *GetLoadTemp(IMODE *dest)
{
    int hash = dhash((UBYTE *)&dest, sizeof(dest));
    STORETEMPHASH *sh = loadHash[hash];
    while (sh)
    {
        if (sh->mem == dest)
        {
            return sh->temp;
        }
        sh = sh->next;
    }
    return NULL;
}
IMODE *LookupStoreTemp(IMODE *dest, IMODE *src)
{
/*	if (dest->offset->type != en_tempref || dest->mode == i_ind)
    {
        if (!dest->bits)
        {
            int hash = dhash((UBYTE *)&dest, sizeof(dest));
            STORETEMPHASH *sh = storeHash[hash];
            while (sh)
            {
                if (sh->mem == dest)
                {
                    return sh->temp;
                }
                sh = sh->next;
            }
            sh = Alloc(sizeof(STORETEMPHASH));
            sh->next = storeHash[hash];
            storeHash[hash] = sh;
            sh->mem = dest;
            sh->temp = tempreg(dest->size, FALSE);
            sh->temp->offset->v.sp->storeTemp = TRUE;
            return sh->temp;
        }
    }
    */
    return src;
}
IMODE *LookupLoadTemp(IMODE *dest, IMODE *source)
{
    (void) dest;
    
    if ((source->mode != i_immed)
        && (source->offset->type != en_tempref || source->mode == i_ind))
    {
        if (source->bits || (source->size >= ISZ_FLOAT && !chosenAssembler->arch->hasFloatRegs))
        {
            source = tempreg(source->size , FALSE);
        }
        else
        {
            int hash = dhash((UBYTE *)&source, sizeof(source));
            STORETEMPHASH *sh = loadHash[hash];
            while (sh)
            {
                if (sh->mem == source)
                {
                    return sh->temp;
                }
                sh = sh->next;
            }
            sh = Alloc(sizeof(STORETEMPHASH));
            sh->next = loadHash[hash];
            loadHash[hash] = sh;
            sh->mem = source;
            sh->temp = tempreg(source->size, FALSE);
              sh->temp->offset->v.sp->loadTemp = TRUE;
            sh->temp->vol = source->vol;
            sh->temp->restricted = source->restricted;
            return sh->temp;
        }
    }
    return source;
}
IMODE *LookupImmedTemp(IMODE *dest, IMODE *source)
{
    return source;
}
IMODE *LookupCastTemp(IMODE *im, int size)
{
    if (im->mode != i_immed)
    {
        CASTTEMPHASH ch;
        CASTTEMPHASH *sh;
        int hash;
        memset(&ch, 0, sizeof(ch));
        ch.sf.im = im;
        ch.sf.size = size;
        hash = dhash((UBYTE *)&ch.sf, sizeof(ch.sf));
        sh = castHash[hash];
        while (sh)
        {
            if (!memcmp(&sh->sf, &ch.sf, sizeof(ch.sf)))
            {
                break;
            }
            sh = sh->next;
        }
        if (!sh)
        {
            sh = Alloc(sizeof(ch));
            memcpy(&sh->sf, &ch.sf, sizeof(ch.sf));
            sh->rv = tempreg(size, 0);
            sh->next = castHash[hash];
            castHash[hash] = sh;
        }
        return sh->rv;
    }
    else
    {
        return tempreg(size, 0);
    }		
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
        l =  - l;
    if (r < 0)
        r =  - r;
    return (l > r);
}
/*-------------------------------------------------------------------------*/
SYMBOL *varsp(EXPRESSION *node)
{
    if (!node)
        return 0;
    switch (node->type)
    {
        case en_label:
        case en_auto:
        case en_pc:
        case en_global:
        case en_tempref:
        case en_threadlocal:
            return node->v.sp;
        default:
            return 0;
    }
}

/*-------------------------------------------------------------------------*/

IMODE *make_imaddress(EXPRESSION *node, int size)
{
    EXPRESSION *node1 = node;
    SYMBOL *sp = node->v.sp;
    IMODE *ap2;
    if (sp && sp->imaddress)
        return sp->imaddress;
    if (sp && sp->storage_class != sc_auto && sp->storage_class != sc_register)
    {
        IncGlobalFlag();
        sp->allocate = TRUE;
        node1 = Alloc(sizeof(EXPRESSION));
        *node1 = *node;
    }
    ap2 = (IMODE *)Alloc(sizeof(IMODE));
    ap2->offset = node1;
    ap2->mode = i_immed;
    ap2->size = size;
    if (!sp->imvalue) // the aliasing needs this regardless of whether we really use it
    {
        TYPE *tp = sp->tp;
        sp->imvalue = (IMODE *)Alloc(sizeof(IMODE));
        *(sp->imvalue) = *ap2;
        sp->imvalue->mode = i_direct;
        while (tp->array)
            tp = tp->btp;
        if (isstructured(tp) || basetype(tp)->type == bt_memberptr)
            sp->imvalue->size = ISZ_UINT;
        else
            sp->imvalue->size = sizeFromType(tp);
    }
    if (sp && sp->storage_class != sc_auto && sp->storage_class != sc_register)
        DecGlobalFlag();
    if (sp)
        sp->imaddress = ap2;
    return ap2;
}

/*-------------------------------------------------------------------------*/

IMODE *make_bf(EXPRESSION *node, IMODE *ap, int size)
/*
 *      construct a bit field reference
 */
{
    IMODE *ap1 = Alloc(sizeof(IMODE));
    SYMBOL *sp = varsp(ap->offset);
    if (node->startbit ==  - 1)
        diag("Illegal bit field");
    *ap1 = *ap;
    ap1->startbit = node->startbit;
    ap1->bits = node->bits;
    ap1->size = size;
    ap1->retval = FALSE;
    if (sp)
        sp->usedasbit = TRUE;
    return ap1;
}

/*-------------------------------------------------------------------------*/

IMODE *make_immed(int size, LLONG_TYPE i)
/*
 *      make a node to reference an immediate value i.
 */

{
    int index = ((ULLONG_TYPE)i)%(sizeof(immed_list)/sizeof(immed_list[0]));
    LIST *a = immed_list[index];
    IMODE *ap;
    while (a)
    {
        ap = (IMODE *)a->data;
        if (ap->offset->v.i == i && size == ap->size)
            return ap;
        a = a->next;
    }

    IncGlobalFlag();
    ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset = intNode(en_c_i, 0);
    ap->offset->v.i = i;
    ap->size = size;
    
    a = (LIST *)Alloc(sizeof(LIST));
    a->data = ap;
    a->next = immed_list[index];
    immed_list[index] = a;
    DecGlobalFlag();
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE *make_fimmed(int size,FPF f)
/*
 *      make a node to reference an immediate value i.
 */

{
    IMODE *ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset= exprNode((size == ISZ_FLOAT || size == ISZ_IFLOAT) ? en_c_f : (size == ISZ_DOUBLE || size == ISZ_IDOUBLE) ? en_c_d : en_c_ld, NULL, NULL);
    ap->offset->v.f = f;
    ap->size = size;
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE *make_parmadj(long i)
/*
 *			make a direct immediate, e.g. for parmadj
 */
{
    EXPRESSION *node = intNode(en_c_i, i);
    IMODE *ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset = node;
    return ap;
}

/*-------------------------------------------------------------------------*/


IMODE *make_ioffset(EXPRESSION *node)
/*
 *      make a direct reference to a node.
 */
{
    EXPRESSION *node1 = node->left;
    IMODE *ap;
    SYMBOL *sp = varsp(node->left);
    if (sp && sp->imvalue && sp->imvalue->size == natural_size(node))
        return sp->imvalue;
    if (sp && sp->storage_class != sc_auto && sp->storage_class != sc_register)
    {
        IncGlobalFlag();
        node1 = Alloc(sizeof(EXPRESSION));
        *node1 = *(node->left);
    }
    ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->offset = node1;
    if (sp && sp->storage_class != sc_auto && sp->storage_class != sc_register)
        DecGlobalFlag();
    ap->mode = i_direct;
    ap->size = natural_size(node);
    if (sp && !sp->imvalue)
        sp->imvalue = ap;
    return ap;
}

/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/

IMODE *indnode(IMODE *ap1, int size)
/*
 * copy the address mode and change it to an indirect type
 *
 */
{
    IMODE *ap;
    SYMBOL *sp;
    if (ap1->mode == i_ind)
    {
        ap = LookupLoadTemp(ap1, ap1);
        if (ap != ap1)
        {
            gen_icode(i_assn, ap, ap1, NULL);
        }
        ap1 = ap;
    }
    if (ap1->bits)
    {
        IMODE *ap2 = Alloc(sizeof(IMODE));
        *ap2 = *ap1;
        
        if (ap1->mode == i_immed)
        {
            ap2->mode = i_direct;
        }
        else if (ap1->mode == i_direct)
        {
            ap2->mode = i_ind;
        }
        ap2->ptrsize = ap1->size;
        ap2->size = size;
        return ap2;
    }
    sp = varsp(ap1->offset);
    if (sp && ap1->mode == i_immed && sp->imvalue && sp->imvalue->size == size)
    {
        ap = sp->imvalue;
        ap->offset = ap1->offset;
    }
    else
    {
        IMODELIST *iml = NULL;
        if (sp)
        {
            switch (sp->storage_class)
            {
                case sc_auto:
                case sc_parameter:
                case sc_global:
                case sc_localstatic:
                case sc_static:
                case sc_external:
                    {
                        IMODE *im = LookupLoadTemp(ap1, ap1);
                        if (im != ap1)
                            gen_icode(i_assn, im, ap1, NULL);
                        ap1 = im;
                        sp = im->offset->v.sp;
                    }
                    break;
                default:
                    break;
            }
            iml = sp->imind;
            while (iml)
            {
                if (iml->im->size == size)
                {
                    ap = iml->im;
                    break;
                }
                iml = iml->next;
            }
        }
        if (!iml)
        {
            if (sp)
            {
                EXPRESSION *node1 = ap1->offset;
                if (sp && sp->storage_class != sc_auto && sp->storage_class !=
                    sc_register)
                {
                    IncGlobalFlag();
                    node1 = Alloc(sizeof(EXPRESSION));
                    *node1 = *ap1->offset;
                }
                ap = (IMODE *)Alloc(sizeof(IMODE));
                *ap =  *ap1;
                ap->offset = node1;
                ap->retval = FALSE;
                if (sp && sp->storage_class != sc_auto && sp->storage_class !=
                    sc_register)
                    DecGlobalFlag();
            }
            else
            {
                ap = (IMODE *)Alloc(sizeof(IMODE));
                *ap =  *ap1;
                ap->retval = FALSE;
            }
            ap->ptrsize = ap1->size ;
            ap->size = size ;
            if (ap1->mode == i_immed)
            {
                ap->mode = i_direct;
                if (sp)
                {
                    if (sp->imvalue && sp->imvalue->size == ap->size)
                        ap = sp->imvalue;
                    else if (sp && !isstructured(sp->tp) && sizeFromType(sp->tp) == ap->size)
                        sp->imvalue = ap;
                }
            }
            else
            {
                ap->mode = i_ind;
                if (sp)
                {
                    IMODELIST *iml;
                    if (sp->storage_class != sc_auto && sp->storage_class !=
                        sc_register)
                    {
                        IncGlobalFlag();
                    }
                    iml = Alloc(sizeof(IMODELIST));
                    iml->next = sp->imind;
                    sp->imind = iml;
                    iml->im = ap;
                    if (sp->storage_class != sc_auto && sp->storage_class !=
                        sc_register)
                    {
                        DecGlobalFlag();
                    }
                }
            }
        }
    }
    return ap;
}
/*-------------------------------------------------------------------------*/

IMODE *gen_deref(EXPRESSION *node, SYMBOL *funcsp, int flags)
/*s
 *      return the addressing mode of a dereferenced node.
 */
{
    IMODE *ap1, *ap2, *ap3;
    int siz1;
    SYMBOL *sp;
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
            siz1 = ISZ_ADDR;
            break;
        case en_l_bool:
            siz1 = ISZ_BOOLEAN;
            break;
        case en_l_uc:
            siz1 = ISZ_UCHAR;
            break;
        case en_l_c:
            siz1 =  - ISZ_UCHAR;
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
            siz1 =  - ISZ_USHORT;
            break;
        case en_l_i:
            siz1 =  - ISZ_UINT;
            break;
        case en_l_p:
        case en_l_ref:
            siz1 =  ISZ_ADDR;
            break;
        case en_l_fp:
            siz1 = ISZ_FARPTR;
            break ;
        case en_l_sp:
            siz1 = ISZ_SEG;
            break;
        case en_l_l:
            siz1 =  - ISZ_ULONG;
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
            siz1 =  - ISZ_ULONGLONG;
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
        default:
            siz1 = ISZ_UINT;
    }
    /* deref for add nodes */
    if (node->left->type == en_add || node->left->type == en_arrayadd 
        || node->left->type == en_structadd)
    {
        ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR);
        ap2 = LookupLoadTemp(NULL, ap1);
        if (ap2 != ap1)
        {
            gen_icode(i_assn, ap2, ap1, NULL);
            ap1 = ap2;
        }
        ap1 = indnode(ap1, siz1);
        if (ap1->offset)
        {
            ap1->vol = node->isvolatile;
            ap1->restricted = node->isrestrict;
        }
    }
    else if (node->left->type == en_const)
    {
            ap1 = make_immed(siz1, node->v.sp->value.i);
    }
    /* deref for auto variables */
    else if (node->left->type == en_imode)
    {
        ap1 = (IMODE *)node->left->v.imode;
    }
    else
    {
        /* other deref */
        switch (nt)
        {
            EXPRESSION *node1;
            IMODE *ap2;
            case en_labcon:
                ap1 = (IMODE *)Alloc(sizeof(IMODE));
                ap1->mode = i_direct;
                ap1->offset = node->left;
                ap1->size = siz1;
                //node->left->v.sp->genreffed = TRUE;
                break;
            case en_threadlocal:
                sp = node->left->v.sp;
                sp->genreffed = TRUE;
                ap1 = make_ioffset(node);
                ap2 = LookupLoadTemp(ap1, ap1);
                if (ap1 != ap2)
                    gen_icode(i_assn, ap2, ap1, NULL);
                ap1 = indnode(ap2, siz1);
                break;
            case en_auto:
                sp = node->left->v.sp;
                if (sp->thisPtr)
                {
                    if (inlinesym_count)
                    {
                        EXPRESSION *exp = inlinesym_thisptr[inlinesym_count-1];
                        if (exp)
                            return gen_expr(funcsp, exp , 0, natural_size(exp));
                    }
                }
                if (sp->storage_class == sc_parameter && sp->inlineFunc.stmt)
                {
                    node = (EXPRESSION *)sp->inlineFunc.stmt;
                    sp = node->left->v.sp;
                }
                if (catchLevel)
                    sp->inCatch = TRUE;
                if (!sp->imaddress && catchLevel)
                {
                    ap1 = make_ioffset(node);
                    break;
                }
                // fall through
            case en_label:
            case en_global:
            case en_pc:
            
                sp = node->left->v.sp;
                if (!sp->stackblock)
//    			if (!isstructured(sp->tp))
                {
                    sp->genreffed = TRUE;
                    ap1 = make_ioffset(node);
                    if (!store)
                    {
                        ap2 = LookupLoadTemp(NULL, ap1);
                        if (ap2 != ap1)
                        {
                            gen_icode(i_assn, ap2, ap1, NULL);
                            ap1 = ap2;
                        }
                    }
                    break;
                }
            // fall through
            default:
                ap1 = gen_expr(funcsp, node->left, 0, natural_size(node->left)); /* generate address */
                ap2 = LookupLoadTemp(NULL, ap1);
                if (ap2 != ap1)
                {
                    gen_icode(i_assn, ap2, ap1, NULL);
                    ap1 = ap2;
                }
                ap1 = indnode(ap1, siz1);
                break;
        }
    }
    if (!store)
    {
        ap2 = LookupLoadTemp(NULL, ap1);
        if (ap2 != ap1)
        {
            gen_icode(i_assn, ap2, ap1, NULL);
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

IMODE *gen_unary(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, enum i_ops op)
/*
 *      generate code to evaluate a unary minus or complement.
 */
{
    IMODE *ap, *ap1;
    (void)flags;
    ap1 = gen_expr(funcsp, node->left, F_VOL, size);
    ap = LookupLoadTemp(NULL, ap1);
    if (ap1 != ap)
        gen_icode(i_assn, ap, ap1, NULL);
    ap = LookupExpression(op, size, ap, NULL);
    ap->vol = node->left->isvolatile;
    ap->restricted = node->left->isrestrict;
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE *gen_asrhd(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, enum i_ops op)
{
    IMODE *ap,  *ap1,  *ap2, *ap3;
    int lab = nextLabel++;
    LLONG_TYPE n ;
    (void)flags;
    (void)size;
    n = mod_mask(node->right->v.i);
    ap = tempreg(natural_size(node->left), 0);
    ap->offset->v.sp->pushedtotemp = TRUE;
    ap3 = gen_expr(funcsp, node->left, F_VOL | F_COMPARE, natural_size(node->left));
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);
    gen_icode(i_assn, ap, ap1, NULL);
    ap3 = gen_expr(funcsp, node->right, F_COMPARE, natural_size(node->right));
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    DumpIncDec(funcsp);
    gen_icgoto(i_jge, lab, ap, make_immed(ap->size,0));
    gen_icode(i_add, ap, ap, make_immed(ISZ_UINT, n));
    gen_label(lab);
    gen_icode(op, ap, ap, ap2);
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE *gen_binary(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, enum i_ops op)
/*
 *      generate code to evaluate a binary node and return 
 *      the addressing mode of the result.
 */
{
    IMODE *ap,  *ap1,  *ap2, *ap3;
    int vol,rest, pragmas;
    (void)flags;
    ap3 = gen_expr(funcsp, node->left, 0, natural_size(node->left));
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);

    if (op == i_lsl || op == i_lsr || op == i_asr)
        flush_dag();	
    ap3 = gen_expr(funcsp, node->right, 0, natural_size(node->right));
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    if (ap1->size >= ISZ_CFLOAT || ap2->size >= ISZ_CFLOAT)
        size = imax(ap1->size,ap2->size);
    else
        if (ap1->size >= ISZ_IFLOAT && ap2->size < ISZ_IFLOAT)
        {
            if (ap2->size < ISZ_FLOAT)
                size = ap1->size;
            else
                size = imax(ap1->size - ISZ_IFLOAT, ap2->size - ISZ_FLOAT) + ISZ_CFLOAT;
        }
        else
            if (ap2->size >= ISZ_IFLOAT && ap1->size < ISZ_IFLOAT)
            {
                if (ap1->size < ISZ_FLOAT)
                    size = ap2->size;
                else
                    size = imax(ap2->size - ISZ_IFLOAT, ap1->size - ISZ_FLOAT) + ISZ_CFLOAT;
            }
    ap = LookupExpression(op, size, ap1, ap2);
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE *gen_pdiv(SYMBOL *funcsp, EXPRESSION *node, int flags, int size)
/*
 *			generate code for an array/structure size compensation
 */
{
    IMODE *ap,  *ap1,  *ap2, *ap3;
    (void)flags;
    (void)size;
    ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);
    ap3 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    ap = LookupExpression(i_udiv, ISZ_UINT, ap1, ap2);
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE *gen_pmul(SYMBOL *funcsp, EXPRESSION *node, int flags, int size)
/*
 *			generate code for an array/structure size compensation
 */
{
    IMODE *ap,  *ap1,  *ap2, *ap3;
    (void)flags;
    (void)size;
    ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);
    ap3 = gen_expr(funcsp, node->right, 0, ISZ_UINT);
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    ap = LookupExpression(i_mul, ISZ_UINT, ap1, ap2);
    return ap;
}

/*-------------------------------------------------------------------------*/

IMODE *gen_hook(SYMBOL *funcsp, EXPRESSION *node, int flags, int size)
/*
 *      generate code to evaluate a condition operator node (?:)
 */
{
    IMODE *ap1,  *ap2, *ap3;
    int false_label, end_label;
    LIST *idl;
    false_label = nextLabel++;
    end_label = nextLabel++;
    flags = flags | F_VOL;
    DumpIncDec(funcsp);
    falsejp(node->left, funcsp, false_label);
    node = node->right;
    ap1 = tempreg(natural_size(node), 0);
    ap1->offset->v.sp->pushedtotemp = TRUE; // needed to make the global opts happy
    ap3 = gen_expr(funcsp, node->left, flags, size);
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    gen_icode(i_assn, ap1, ap2, 0);
    ap1->offset->v.sp->iglobal = TRUE;
    DumpIncDec(funcsp);
    gen_igoto(i_goto, end_label);
    gen_label(false_label);
    ap3 = gen_expr(funcsp, node->right, flags, size);
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    gen_icode(i_assn, ap1, ap2, 0);
    ap1->offset->v.sp->iglobal = TRUE;
    DumpIncDec(funcsp);
    gen_label(end_label);
    return ap1;
}


/*-------------------------------------------------------------------------*/

IMODE *gen_moveblock(EXPRESSION *node, SYMBOL *funcsp)
/*
 * Generate code to copy one structure to another
 */
{
    IMODE *ap1,  *ap2, *ap3;
    if (!node->size)
        return (0);
    ap3 = gen_expr(funcsp, node->left, F_VOL, ISZ_UINT);
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);
    ap3 = gen_expr(funcsp, node->right, F_VOL, ISZ_UINT);
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    gen_icode(i_assnblock, make_immed(ISZ_UINT,node->size), ap1, ap2);
    return (ap1);
}
IMODE *gen_clearblock(EXPRESSION *node, SYMBOL *funcsp)
/*
 * Generate code to copy one structure to another
 */
{
    IMODE *ap1, *ap3, *ap4, *ap5;
    if (node->size)
    {
        ap4 = make_immed(ISZ_UINT,node->size);
    }
    else if (node->right)
    {
        ap5 = gen_expr( funcsp, node->right, F_VOL, ISZ_UINT);
        ap4 = LookupLoadTemp(NULL, ap5);
        if (ap5 != ap4)
            gen_icode(i_assn, ap4, ap5, NULL);
    }
    else
    {
        return (0);
    }
    ap3 = gen_expr( funcsp, node->left, F_VOL, ISZ_UINT);
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);
    gen_icode(i_clrblock, 0, ap1, ap4);
    return (ap1);
}

/*-------------------------------------------------------------------------*/

IMODE *gen_assign(SYMBOL *funcsp, EXPRESSION *node, int flags, int size)
/*
 *      generate code for an assignment node. if the size of the
 *      assignment destination is larger than the size passed then
 *      everything below this node will be evaluated with the
 *      assignment size.
 */
{
    IMODE *ap1, *ap2, *ap3, *ap4;
    EXPRESSION *enode , * temp;
    LIST *l2, *lp;
    (void)flags;
    (void)size;
    ap2 = gen_expr(funcsp, node->right, flags & ~F_NOVALUE, natural_size(node->left));
    ap4 = LookupLoadTemp(ap2, ap2);
    if (ap4 != ap2)
        gen_icode(i_assn, ap4, ap2, NULL);
    ap1 = gen_expr(funcsp, node->left, (flags & ~F_NOVALUE) | F_STORE, natural_size(node->left));
    ap1->vol = ap2->vol;
    ap2->restricted = ap4->restricted;
    ap1->offset->pragmas = ap4->offset->pragmas;
    gen_icode(i_assn, ap1, ap4, NULL);
    /*
    if (ap1->mode != i_direct || ap1->offset->type != en_tempref)
    {
        ap3 = tempreg(ap4->size, FALSE);
        gen_icode(i_assn, ap3, ap4, NULL);
        ap4 = ap3;
    }
    ap3 = LookupStoreTemp(ap1, ap1);
    if (ap3 != ap1)
    {
        gen_icode(i_assn, ap3, ap4, NULL);
        gen_icode(i_assn, ap1, ap3, NULL);
    }
    else
        gen_icode(i_assn, ap1, ap4, NULL);
    */
    ap1->vol = node->left->isvolatile;
    ap1->restricted = node->left->isrestrict;
    return ap1;
}

/*-------------------------------------------------------------------------*/

IMODE *gen_aincdec(SYMBOL *funcsp, EXPRESSION *node, int flags, int size, enum i_ops op)
/*
 *      generate an auto increment or decrement node. op should be
 *      either op_add (for increment) or op_sub (for decrement).
 */
{
    IMODE *ap1, *ap2,*ap3, *ap4, *ap5, *ap6;
    int siz1;
    EXPRESSION *ncnode;
    LIST *l;
    (void)size;
    siz1 = natural_size(node->left);
    ap1 = gen_expr( funcsp, RemoveAutoIncDec(node->left), 0, siz1);
    ncnode = node->left;
    while (castvalue(ncnode))
        ncnode = ncnode->left;
    ap6 = gen_expr( funcsp, ncnode, F_STORE, siz1);
    ap5 = LookupLoadTemp(ap1, ap1);
    if (ap5 != ap1)
        gen_icode(i_assn, ap5, ap1, NULL);
    if (flags & (F_NOVALUE | F_COMPARE))
    {
        if (flags & F_COMPARE)
        {
            ap3 = tempreg(siz1, 0);
            gen_icode(i_assn, ap3, ap5, NULL);
            intermed_tail->needsOCP = TRUE;
        }
        else
        {
            ap3 = ap5;
        }
        ap2 = gen_expr(funcsp, node->right, 0, siz1);
        ap2 = LookupExpression(op, siz1, ap5, ap2);
        ap4 = LookupStoreTemp(ap6, ap6);
        if (ap4 != ap6)
        {
            gen_icode(i_assn, ap4, ap2, NULL);
            gen_icode(i_assn, ap6, ap4, NULL);
        }
        else
            gen_icode(i_assn, ap6, ap2, NULL);
        return ap3;
    }
    else
    {
        l = Alloc(sizeof(LIST));
        l->data = (void *)node;
        if (!incdecList)
            incdecList = incdecListLast = l;
        else
            incdecListLast = incdecListLast->next = l;
        return ap5;
    }
    /*
    ap3 = tempreg(siz1,0);
    gen_icode(i_assn, ap3, ap5, 0);
    ap2 = LookupExpression(op, siz1, ap3, ap2);
    ap4 = LookupStoreTemp(ap6, ap6);
    if (ap4 != ap6)
    {
        gen_icode(i_assn, ap4, ap2, NULL);
        gen_icode(i_assn, ap6, ap4, NULL);
    }
    else
        gen_icode(i_assn, ap6, ap2, NULL);
        
    return ap3;
    */
}

/*-------------------------------------------------------------------------*/
static EXPRESSION *getAddress(EXPRESSION *exp)
{
    EXPRESSION *rv = NULL;
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
static EXPRESSION *getFunc(EXPRESSION *exp)
{
    EXPRESSION *rv = NULL;
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
int push_param(EXPRESSION *ep, SYMBOL *funcsp)
/*
 *      push the operand expression onto the stack.
 */
{
    IMODE *ap, *ap3;
    int temp;
    int rv = 0;
    EXPRESSION *exp = getFunc(ep);
    if (exp || ep->type == en_blockassign || ep->type == en_blockclear)
    {
        EXPRESSION *ep1 = exp;
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
            gen_icode(i_parmstack, ap = tempreg(ISZ_ADDR, 0), make_immed(ISZ_UINT, rv), NULL );
            exp->v.sp->imvalue = ap;
            gen_expr(funcsp, ep, 0, ISZ_UINT );
        }
        else
        {
            temp = natural_size(ep);
            ap3 = gen_expr( funcsp, ep, 0, temp);
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, NULL);
            if (ap->size == ISZ_NONE)
                ap->size = temp;
            gen_nodag(i_parm, 0, ap, 0);
            rv = sizeFromISZ(ap->size);
        }
    }
    else
    {
        switch (ep->type)
        {
            case en_void:
                while (ep->type == en_void)
                {
                    gen_expr( funcsp, ep->left, 0, ISZ_UINT);
                    ep = ep->right;
                }
                push_param(ep, funcsp);
                break;
            case en_argnopush:
                gen_expr( funcsp, ep->left, 0, ISZ_UINT);
                break;
            case en_imode:
                ap = (IMODE *)ep->left;
                gen_nodag(i_parm, 0, ap, 0);
                rv = sizeFromISZ(ap->size);
                break;
            default:
                temp = natural_size(ep);
                ap3 = gen_expr( funcsp, ep, 0, temp);
                ap = LookupLoadTemp(NULL, ap3);
                if (ap != ap3)
                    gen_icode(i_assn, ap, ap3, NULL);
                if (ap->size == ISZ_NONE)
                    ap->size = temp;
                gen_nodag(i_parm, 0, ap, 0);
                rv = sizeFromISZ(ap->size);
                break;
        }
        if (rv % chosenAssembler->arch->stackalign)
            rv = rv + chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

static int push_stackblock(EXPRESSION *ep, SYMBOL *funcsp, int sz)
/*
 * Push a structure on the stack
 */
{
    IMODE *ap, *ap3;
    if (!sz)
        return 0;
    switch (ep->type)
    {
        case en_imode:
            ap = ep->v.imode;
            break;
        default:
            ap3 = gen_expr( funcsp, ep, F_ADDR, ISZ_UINT);
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, NULL);
            break;
    }
    gen_nodag(i_parmblock, 0, ap, make_immed(ISZ_UINT,sz));
    if (sz % chosenAssembler->arch->stackalign)
        sz = sz + chosenAssembler->arch->stackalign - sz % chosenAssembler->arch->stackalign;
    return sz;
}

/*-------------------------------------------------------------------------*/

static int gen_parm(INITLIST *a, SYMBOL *funcsp)
/*
 *      push a list of parameters onto the stack and return the
 *      size of parameters pushed.
 */
{
    int rv;
        if (ispointer(a->tp) || isref(a->tp))
        {
            TYPE *btp = basetype(a->tp);
            if (btp->vla || basetype(btp->btp)->vla)
            {
                rv = push_stackblock(a->exp->left, funcsp, a->tp->size);
                DumpIncDec(funcsp);
                push_nesting += rv;
                return rv;
            }
        }
        if (!cparams.prm_cplusplus && isstructured(a->tp))
        {
            rv = push_stackblock(a->exp->left, funcsp, a->exp->size);
        }
        else if (a->exp->type == en_stackblock)
        {
            rv = push_stackblock(a->exp->left, funcsp, a->tp->size);
        }
        else if (isstructured(a->tp) && a->exp->type == en_thisref) // constructor
        {
            EXPRESSION *ths = a->exp->v.t.thisptr;
            if (ths && ths->type == en_auto && ths->v.sp->stackblock)
            {
                IMODE *ap;
                // constructor or other function creating a structure on the stack
                ths = a->exp->left->v.func->thisptr;
                ths->v.sp->stackblock = TRUE;
                
                rv = ths->v.sp->tp->size;
                if (rv % chosenAssembler->arch->stackalign)
                    rv = rv + chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
                gen_icode(i_parmstack, ap = tempreg(ISZ_ADDR, 0), make_immed(ISZ_UINT, rv), NULL );
                ths->v.sp->imvalue = ap;
                gen_expr(funcsp, a->exp, 0, ISZ_UINT );
            }
            else
            {
                IMODE *ap = gen_expr(funcsp, a->exp, 0, ISZ_ADDR);
                gen_nodag(i_parm, 0, ap, 0);
                rv = a->tp->size;
            }
        }
        else
        {
            rv = push_param(a->exp, funcsp);
        }
        DumpIncDec(funcsp);
        push_nesting += rv;
        return rv;
}
static int sizeParam(INITLIST *a, SYMBOL *funcsp)
{
    int rv ;
    if (ispointer(a->tp) || isref(a->tp) || a->tp->type == bt_func || a->tp->type == bt_ifunc || a->byRef)
        rv = sizeFromISZ(ISZ_ADDR);
    else
        rv = a->tp->size;
    if (rv % chosenAssembler->arch->stackalign)
        rv += chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
    return rv;
}
static int genCdeclArgs(INITLIST *args, SYMBOL *funcsp)
{
    int rv = 0;
    if (args)
    {
        rv = genCdeclArgs(args->next, funcsp);
        rv += gen_parm(args, funcsp);
    }
    return rv;
}
static void genCallLab(INITLIST *args, int callLab)
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
static int genPascalArgs(INITLIST *args, SYMBOL *funcsp)
{
    int rv = 0;
    if (args)
    {
        rv = gen_parm(args, funcsp);
        rv += genPascalArgs(args->next, funcsp);
    }
    return rv;
}
static int sizeParams(INITLIST *args, SYMBOL *funcsp)
{
    int rv = 0;
    if (args)
    {
        rv = sizeParam(args, funcsp);
        rv += sizeParams(args->next, funcsp);
    }
    return rv;
}
/*-------------------------------------------------------------------------*/

IMODE *gen_tcall(EXPRESSION *node, SYMBOL *funcsp, int flags)
/*
 *      generate a trap call node and return the address mode
 *      of the result.
 */
{
    IMODE *ap;
    int siz1 = ISZ_UINT;
    (void)funcsp;
    ap = make_ioffset(node->left);
    ap->mode = i_immed;
    gen_igosub(i_int, ap);
    if (!(flags &F_NOVALUE)) {
        ap = tempreg(siz1, 0);
    }
    return ap;
}


IMODE *gen_trapcall(SYMBOL *funcsp, EXPRESSION *node, int flags)
{
    /* trap call */
    (void)funcsp;
    (void)flags;
    gen_igosub(i_trap, make_immed(ISZ_UINT,node->v.i));
    return 0;
}
IMODE *gen_stmt_from_expr(SYMBOL *funcsp, EXPRESSION *node, int flags)
{
    IMODE *rv;
    /*
    STORETEMPHASH *holdst[DAGSIZE];
    STORETEMPHASH *holdld[DAGSIZE];
    STORETEMPHASH *holdim[DAGSIZE];
    CASTTEMPHASH *holdcast[DAGSIZE];
    DAGLIST *holdnv[DAGSIZE];
    */
    (void)flags;
    /*
    memcpy(holdst, storeHash, sizeof(storeHash));
    memcpy(holdld, loadHash, sizeof(loadHash));
    memcpy(holdnv, name_value_hash, sizeof(name_value_hash));
    memcpy(holdim, immedHash, sizeof(immedHash));
    memcpy(holdcast, castHash, sizeof(castHash));
    */
    /* relies on stmt only being used for inlines */
    rv = genstmt(node->v.stmt, funcsp);	
    /*
    memcpy(storeHash, holdst, sizeof(storeHash));
    memcpy(loadHash, holdld, sizeof(loadHash));
    memcpy(name_value_hash, holdnv, sizeof(name_value_hash));
    memcpy(immedHash, holdim, sizeof(immedHash));
    memcpy(castHash, holdcast, sizeof(castHash));
    */
    if (node->left)
        rv = gen_expr( funcsp, node->left, 0, natural_size(node->left));
    return rv;
}
/*-------------------------------------------------------------------------*/

static void gen_arg_destructors(SYMBOL *funcsp, INITLIST *arg)
{
    if (arg)
    {
        gen_arg_destructors(funcsp, arg->next);
        if (arg->dest)
            gen_expr( funcsp, arg->dest, F_NOVALUE, ISZ_UINT);
    }
}
IMODE *gen_funccall(SYMBOL *funcsp, EXPRESSION *node, int flags)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    IMODE *ap3;
    FUNCTIONCALL *f = node->v.func;
    
    IMODE *ap;
    int adjust = 0 ;
    int adjust2 = 0;
    QUAD *gosub = NULL;
    if (!f->ascall)
    {
        return gen_expr(funcsp, f->fcall, 0, ISZ_ADDR);
    }
    if (chosenAssembler->gen->handleIntrins)
    {
        ap = chosenAssembler->gen->handleIntrins(node, flags &F_NOVALUE);
        if (ap)
            return ap;
    }
    if (!f->sp->noinline && f->sp->isInline)
    {
        ap = gen_inline(funcsp, node, flags);
        if (ap)
            return ap;
    }
    {
        int n = sizeParams(f->arguments, funcsp);
        int v = sizeFromISZ(ISZ_ADDR);
        if (v % chosenAssembler->arch->stackalign)
            v = v + chosenAssembler->arch->stackalign - v % chosenAssembler->arch->stackalign;
        if (isstructured(basetype(f->functp)->btp) || basetype(basetype(f->functp)->btp)->type == bt_memberptr)
        {
            if (f->returnEXP)
                n += v;
        }
        if (f->thisptr)
        {
            n += v;
        }
        adjust +=n;
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
            gen_icode(i_substack, NULL, make_immed(ISZ_UINT, n), NULL);
        }
    }
    if(f->sp->linkage == lk_pascal)
    {
        if (isstructured(basetype(f->functp)->btp) || basetype(basetype(f->functp)->btp)->type == bt_memberptr)
        {
            if (f->returnEXP)
                push_param(f->returnEXP, funcsp);
        }
        genPascalArgs(f->arguments, funcsp);
    }
    else
    {
        int n = 0;
        if (f->thisptr && f->thisptr->type == en_auto && f->thisptr->v.sp->stackblock)
        {
            EXPRESSION *exp = f->thisptr;
            // constructor or other function creating a structure on the stack
            int rv = exp->v.sp->tp->size;
            if (rv % chosenAssembler->arch->stackalign)
                rv = rv + chosenAssembler->arch->stackalign - rv % chosenAssembler->arch->stackalign;
            gen_icode(i_parmstack, ap = tempreg(ISZ_ADDR, 0), make_immed(ISZ_UINT, rv), NULL );
            exp->v.sp->imvalue = ap;
            genCdeclArgs(f->arguments, funcsp);
            ap3 = gen_expr(funcsp, exp, 0, ISZ_UINT );
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, NULL);
            if (ap->size == ISZ_NONE)
                ap->size = ISZ_ADDR;
            gen_nodag(i_parm, 0, ap, 0);
        }
        else
        {
            genCdeclArgs(f->arguments, funcsp);
            if (f->thisptr)
            {
                push_param(f->thisptr, funcsp);
            }
        }
        if (f->callLab == -1)
        {
            f->callLab = ++consIndex;
            genCallLab(f->arguments, f->callLab);
        }
        if (isstructured(basetype(f->functp)->btp) || basetype(basetype(f->functp)->btp)->type == bt_memberptr)
        {
            if (f->returnEXP)
                push_param(f->returnEXP, funcsp);
        }
    }
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
        ap = ap3 = gen_expr( funcsp, f->fcall, 0, ISZ_UINT);
//		ap = LookupLoadTemp(NULL, ap3);
//		if (ap != ap3)
//			gen_icode(i_assn, ap, ap3, NULL);
        if (ap->mode == i_immed && ap->offset->type == en_pc) {
            if (ap->offset->v.sp->indirect) {
                IMODE *ap1 = (IMODE *)Alloc(sizeof(IMODE));
                *ap1 = *ap;
                ap1->retval = FALSE;
                ap = ap1;
                ap->mode = i_direct;
            }
/*            if (type == i_gosub)
                if (ap->offset->v.sp->farproc)
                    type = i_fargosub;
*/
        }
        if (f->callLab && xcexp)
        {
            xcexp->right->v.i = f->callLab;
            gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
        }
        gosub = gen_igosub(type, ap);
    }
    if ((flags & F_NOVALUE) && !isstructured(basetype(f->functp)->btp))
    {
        gosub->novalue = sizeFromType(basetype(f->functp)->btp);
    }
    else
    {
        gosub->novalue = -1;
    }
    /* undo pars and make a temp for the result */
    if (f->sp->linkage != lk_stdcall && f->sp->linkage != lk_pascal)
        gen_nodag(i_parmadj, 0, make_parmadj(adjust), make_parmadj(adjust));
    else
        gen_nodag(i_parmadj, 0, make_parmadj(adjust2), make_parmadj(adjust));
    push_nesting -= adjust;
    if (!(flags &F_NOVALUE) && !isvoid(basetype(f->functp)->btp)) {
        /* structures handled by callee... */
        if (!isstructured(basetype(f->functp)->btp) && basetype(f->functp)->btp->type != bt_memberptr)
        {
            IMODE *ap1, *ap2;
             int siz1 = sizeFromType(basetype(f->functp)->btp);
             ap1 = tempreg(siz1, 0);
            ap1->retval = TRUE;
            gen_icode(i_assn, ap = tempreg(siz1, 0), ap1, 0);
        }
        else
        {
             ap = tempreg(ISZ_ADDR, 0);
            ap->retval = TRUE;
        }
            
    }
    else
    {
            ap = tempreg(ISZ_ADDR, 0);
            ap->retval = TRUE;
    }
    gen_arg_destructors(funcsp, f->arguments);
    return ap;
}
IMODE *gen_atomic_barrier(SYMBOL *funcsp, ATOMICDATA *ad, IMODE *addr, IMODE *barrier)
{
    IMODE *left, *right;
    if (needsAtomicLockFromType(ad->tp))
    {
        IMODE *right;
        if (barrier)
        {
            left = make_immed(ISZ_UINT, -ad->memoryOrder1->v.i);
        }
        else
        {
            left = make_immed(ISZ_UINT, ad->memoryOrder1->v.i);
            barrier = tempreg(ISZ_ADDR, 0);
            right = make_immed(ISZ_UINT, ad->tp->size - ATOMIC_FLAG_SPACE);
            gen_icode(i_add, barrier, addr, right);
        }
        gen_icode(i_atomic_flag_fence, NULL, left, barrier);
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
        gen_icode(i_atomic_fence, NULL, left, NULL);
        return (IMODE *) -1;
    }
}
IMODE *gen_atomic(SYMBOL *funcsp, EXPRESSION *node, int flags, int size)
{
    IMODE *rv = NULL;
    switch(node->v.ad->atomicOp)
    {
        IMODE *left, *right;
        IMODE *av;
        IMODE *barrier;
        int sz, op;
        case ao_init:
            sz = sizeFromType(node->v.ad->tp);
            av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            left = indnode(av, sz);
            right = gen_expr(funcsp, node->v.ad->value, 0, sz);
            gen_icode(i_assn, left, right, NULL);
            if (needsAtomicLockFromType(node->v.ad->tp))
            {
                IMODE *temp = tempreg(ISZ_ADDR, 0);
                gen_icode(i_add, temp, av, make_immed(ISZ_UINT, node->v.ad->tp->size - ATOMIC_FLAG_SPACE));
                temp = indnode(temp, ISZ_UINT);
                gen_icode(i_atomic_flag_clear, NULL, make_immed(ISZ_UINT, mo_relaxed), temp);
            }
            rv = right;
            break;
        case ao_flag_set_test:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            right = gen_expr(funcsp, node->v.ad->flg, 0, ISZ_UINT);
            rv = tempreg(ISZ_UINT, 0);
            gen_icode(i_atomic_flag_test_and_set, rv, left, right);
            intermed_tail->alwayslive = TRUE;
            break;
        case ao_flag_clear:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            right = gen_expr(funcsp, node->v.ad->flg, 0, ISZ_UINT);
            gen_icode(i_atomic_flag_clear, NULL, left, right);
            break;
        case ao_fence:
            left = gen_expr(funcsp, node->v.ad->memoryOrder1, 0, ISZ_UINT);
            gen_icode(i_atomic_fence, NULL, left, NULL);
            break;
        case ao_load:
            av = gen_expr(funcsp, node->v.ad->address, 0, sizeFromType(node->v.ad->tp));
            left = indnode(av,sizeFromType(node->v.ad->tp));
            rv = tempreg(sizeFromType(node->v.ad->tp), 0);
            barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
            gen_icode(i_assn, rv, left, NULL);
            intermed_tail->alwayslive = TRUE;
            gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            break;
        case ao_store:
            sz = sizeFromType(node->v.ad->tp);
            right = gen_expr(funcsp, node->v.ad->value, 0, sz);
            av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            left = indnode(av, sz);
            barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
            gen_icode(i_assn, left, right, NULL);
            gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            rv = right;
            break;
        case ao_modify:
            sz = sizeFromType(node->v.ad->tp);
            av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            left = indnode(av, sz);
            right = gen_expr(funcsp, node->v.ad->value, 0, sz);
            switch((int)node->v.ad->third->v.i)
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
            }            
            
            barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
            rv = tempreg(sz, 0);
            gen_icode(i_assn, rv, left, NULL);
            gen_icode(op, left, left, right);
            gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            break;
        case ao_cmpswp:
            sz = sizeFromType(node->v.ad->tp);
            av = gen_expr(funcsp, node->v.ad->address, 0, ISZ_ADDR);
            left = indnode(av, sz);
            right = gen_expr(funcsp, node->v.ad->third, 0, ISZ_ADDR);
            right = indnode(right, sz);
            rv = gen_expr(funcsp, node->v.ad->value, F_VOL, sz);
            barrier = gen_atomic_barrier(funcsp, node->v.ad, av, 0);
            gen_icode(i_cmpswp, left, rv, right);
            rv = tempreg(ISZ_UINT, 0);
            rv->retval = TRUE;
            av = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, av, rv, NULL);
            rv = av;
            gen_atomic_barrier(funcsp, node->v.ad, av, barrier);
            break;
    }
    return rv;
}
IMODE *doatomicFence(SYMBOL * funcsp, EXPRESSION *parent, EXPRESSION *node, IMODE *barrier)
{
    static LIST *lst;
    int start = !barrier;
    if (node && node->isatomic)
    {
        int afImmed = mo_seq_cst;
        if (parent && parent->type == en_assign)
            afImmed |= 128; // store
        if (node->lockOffset)
        {
            LIST *cur;
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
                if (equalnode(cur->data, node))
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
                gen_icode(i_atomic_flag_fence, NULL, make_immed(ISZ_UINT, start ? afImmed : -afImmed), barrier);
            }
            if (start)
            {
                cur = Alloc(sizeof(LIST));
                cur->next = lst;
                lst = cur;
                cur->data = node;
            }
        }
        else
        {
            gen_icode(i_atomic_fence, NULL, make_immed(ISZ_UINT, start ? afImmed : -afImmed), NULL);
            barrier = (IMODE *)-1;
        }
    }
    return barrier;
}
/*-------------------------------------------------------------------------*/

IMODE *gen_expr(SYMBOL *funcsp, EXPRESSION *node, int flags, int size)
/*
 *      general expression evaluation. returns the addressing mode
 *      of the result.
 *
 *			The first argument of this and other routines is a target for
 *			assignmennt.  If this is 0 and a target is needed a temp will be
 *			used; otherwise the target will be evaluated annd used.
 */
{
    IMODE *ap1,  *ap2,  *ap3;
    IMODE *rv;
    IMODE *lbarrier, *rbarrier;
    int lab0;
    int siz1;
    int ctype;
    int store = flags & F_STORE;
    flags &= ~F_STORE;
    if (node == 0)
    {
/*        diag("null node in gen_expr.");*/
        return 0;
    }
    while(node->type == en_not_lvalue || node->type == en_lvalue)
        node = node->left;
    lbarrier = doatomicFence(funcsp, node, node->left, 0);
    rbarrier = doatomicFence(funcsp, NULL, node->right, 0);
    switch (node->type)
    {
        case en_shiftby:
            ap3 = gen_expr( funcsp, node->left, flags, size);
            ap1 = LookupLoadTemp(NULL, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, NULL);
            rv = ap1;
            break;
        case en_x_wc:
            ctype = en_c_wc;
            siz1 =  ISZ_WCHAR;
            goto castjoin;
        case en_x_c:
            ctype = en_c_c;
            siz1 =  - ISZ_UCHAR;
            goto castjoin;
        case en_x_u16:
            ctype = en_c_u16;
            siz1 =  ISZ_U16;
            goto castjoin;
        case en_x_u32:
            ctype = en_c_u32;
            siz1 =  ISZ_U32;
            goto castjoin;
        case en_x_bool:
            ctype = en_c_bool;
            siz1 = ISZ_BOOLEAN;
            goto castjoin;
        case en_x_bit:
            siz1 = ISZ_BIT;
            goto castjoin;
        case en_x_uc:
            ctype = en_c_uc;
            siz1 = ISZ_UCHAR;
            goto castjoin;
        case en_x_s:
            ctype = en_c_i;
            siz1 =  - ISZ_USHORT;
            goto castjoin;
        case en_x_us:
            ctype = en_c_ui;
            siz1 = ISZ_USHORT;
            goto castjoin;
        case en_x_i:
            ctype = en_c_i;
            siz1 =  - ISZ_UINT;
            goto castjoin;
        case en_x_ui:
            ctype = en_c_ui;
            siz1 = ISZ_UINT;
            goto castjoin;
        case en_x_l:
            ctype = en_c_l;
            siz1 =  - ISZ_ULONG;
            goto castjoin;
        case en_x_ul:
            ctype = en_c_ul;
            siz1 = ISZ_ULONG;
            goto castjoin;
        case en_x_ll:
            ctype = en_c_ll;
            siz1 =  - ISZ_ULONGLONG;
            goto castjoin;
        case en_x_ull:
            ctype = en_c_ull;
            siz1 = ISZ_ULONGLONG;
            goto castjoin;

        case en_x_f:
            ctype = en_c_f;
            siz1 = ISZ_FLOAT;
            goto castjoin;
        case en_x_d:
            ctype = en_c_d;
            siz1 = ISZ_DOUBLE;
            goto castjoin;
        case en_x_ld:
            ctype = en_c_ld;
            siz1 = ISZ_LDOUBLE;
            goto castjoin;
        case en_x_fi:
            ctype = en_c_fi;
            siz1 = ISZ_IFLOAT;
            goto castjoin;
        case en_x_di:
            ctype = en_c_di;
            siz1 = ISZ_IDOUBLE;
            goto castjoin;
        case en_x_ldi:
            ctype = en_c_ldi;
            siz1 = ISZ_ILDOUBLE;
            goto castjoin;
        case en_x_fc:
            ctype = en_c_fc;
            siz1 = ISZ_CFLOAT;
            goto castjoin;
        case en_x_dc:
            ctype = en_c_dc;
            siz1 = ISZ_CDOUBLE;
            goto castjoin;
        case en_x_ldc:
            ctype = en_c_ldc;
            siz1 = ISZ_CLDOUBLE;
            goto castjoin;
        case en_x_fp:
            ctype = en_c_ui;
            siz1 = ISZ_FARPTR;
            goto castjoin;
        case en_x_sp:
            ctype = en_c_ui;
            siz1 = ISZ_SEG;
            goto castjoin;
        case en_x_p:
            siz1 = ISZ_ADDR;
            ctype = en_c_ui;
 castjoin: 
             ap3 = gen_expr(funcsp, node->left, flags, natural_size(node->left));
            ap1 = LookupLoadTemp(NULL, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, NULL);
            if (ap1->mode == i_immed) 
            {
                if (isintconst(ap1->offset))
                {
                    if (siz1 >= ISZ_FLOAT) 
                    {
                        FPF f;
                        IntToFloat(&f, siz1, ap1->offset->v.i);
                        ap1 = make_fimmed(siz1, f);
                    }
                }
                else if (isfloatconst(ap1->offset))
                {
                    if (siz1 < ISZ_FLOAT) 
                    {
                        ap1 = make_immed(siz1, FPFToLongLong(&ap1->offset->v.f));
                    }
                }
            }
            if (ap1->size == siz1)
                ap2 = ap1;
            else
            {
                ap2 = LookupCastTemp(ap1, siz1);
                gen_icode(i_assn, ap2, ap1, NULL);
//                gen_icode(i_assn, ap2 = tempreg(siz1, 0), ap1, 0);
            }
            if (ap2->offset)
            {
                ap2->vol = node->left->isvolatile;
                ap2->restricted = node->left->isrestrict;
            }
            rv = ap2;
            break;
        case en_substack:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT );
            gen_icode(i_substack, ap2 = tempreg(ISZ_ADDR, 0), ap1, NULL );
            rv = ap2;
            break;
        case en_alloca:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_UINT );
            gen_icode(i_substack, ap2 = tempreg(ISZ_ADDR, 0), ap1, NULL );
            rv = ap2;
            break;
        case en_loadstack:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR );
            gen_icode(i_loadstack, 0, ap1,0 );
            rv = NULL;
            break;
        case en_savestack:
            ap1 = gen_expr(funcsp, node->left, 0, ISZ_ADDR );
            gen_icode(i_savestack, 0, ap1,0 );
            rv = NULL;
            break;
        case en_threadlocal:
            ap1 = make_ioffset(node);
            ap1->offset = node;
            ap2 = LookupLoadTemp(ap1, ap1);
            if (ap1 != ap2)
                gen_icode(i_assn, ap2, ap1, NULL);
            rv = ap2;
            break;
        case en_auto:
            if (node->v.sp->stackblock)
            {
                rv = node->v.sp->imvalue;
                break;
            }
            if (node->v.sp->storage_class == sc_parameter && node->v.sp->inlineFunc.stmt)
            {
                return gen_expr (funcsp, ((EXPRESSION *)node->v.sp->inlineFunc.stmt)->left, flags, size);
            }
            node->v.sp->allocate = TRUE;
            // fallthrough
        case en_pc:
        case en_global:
        case en_absolute:
        case en_label:
            node->v.sp->genreffed = TRUE;
            if (node->v.sp->imaddress)
            {
               ap1 = node->v.sp->imaddress;
            }
            else
            {
                ap1 = (IMODE *)Alloc(sizeof(IMODE));
                ap1->offset = node;
                ap1->mode = i_immed;
                ap1->size = size;
            }
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, NULL);
                ap1 = ap2;
            }
            rv = ap1; /* return reg */
            node->v.sp->imaddress = ap1;
            break;
        case en_labcon:
            ap1 = (IMODE *)Alloc(sizeof(IMODE));
            ap1->offset = node;
            ap1->mode = i_immed;
            ap1->size = size;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, NULL);
                ap1 = ap2;
            }
            rv = ap1; /* return reg */
            break;
        case en_imode:
            ap2 = (IMODE *)node->left;
            rv = ap2; /* return reg */
            break;
        case en_c_bool:
        case en_c_c:
        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_i:
        case en_c_l:
        case en_c_ul:
        case en_c_ui:
        case en_c_ll:
        case en_c_ull:
        case en_c_wc:
        case en_nullptr:        
            if (size >= ISZ_FLOAT)
            {
                FPF f;
                LongLongToFPF(&f, node->v.i);
                ap1 = make_fimmed(size, f);
            }
            else
            {
                ap1 = make_immed(size, node->v.i);
            }
            ap1->offset->unionoffset = node->unionoffset;
            ap1->offset->type = node->type;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, NULL);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            if (size >= ISZ_FLOAT)
            {
                ap1 = make_fimmed(size, node->v.f);
            }
            else
            {
                LLONG_TYPE a = FPFToLongLong(&node->v.f);
                ap1 = make_immed(size, a);
            }
            ap1->offset->type = node->type;
            ap2 = LookupImmedTemp(ap1, ap1);
            if (ap1 != ap2)
            {
                gen_icode(i_assn, ap2, ap1, NULL);
                ap1 = ap2;
            }
            rv = ap1;
            break;
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            diag("gen_expr: Complex Constant");
            ap1 = Alloc(sizeof(IMODE));
            ap1->mode = i_immed;
            ap1->offset = node;
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
                gen_icode(i_assn, ap2, ap1, NULL);
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
        case en_l_dc:
        case en_l_ldc:
        case en_l_fp:
        case en_l_sp:
        case en_l_bit:
            ap1 = gen_deref(node, funcsp, flags | store);
            rv = ap1;
            break;
        case en_bits:
            size = natural_size(node->left);
            ap3 = gen_expr( funcsp, node->left, 0, size);
            ap1 = LookupLoadTemp(NULL, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, NULL);
            ap1 = make_bf(node, ap1, size);
            rv = ap1;
            break;
        case en_uminus:
            rv = gen_unary( funcsp, node, flags, size, i_neg);
            break;
        case en_compl:
            rv = gen_unary( funcsp, node, flags, size, i_not);
            break;
        case en_add:
            rv = gen_binary( funcsp, node, flags, size, i_add);
            break;
        case en_sub:
            rv = gen_binary( funcsp, node, flags, size, i_sub);
            break;
        case en_and:
            rv = gen_binary( funcsp, node, flags, size, i_and);
            break;
        case en_or:
            rv = gen_binary( funcsp, node, flags, size, i_or);
            break;
        case en_xor:
            rv = gen_binary( funcsp, node, flags, size, i_eor);
            break;
        case en_arraymul:
            rv = gen_pmul( funcsp, node, flags, size);
            break;
        case en_arraydiv:
            rv = gen_pdiv( funcsp, node, flags, size);
            break;
        case en_mul:
            rv = gen_binary( funcsp, node, flags, size, i_mul);
            break;
        case en_umul:
            rv = gen_binary( funcsp, node, flags, size, i_mul);
            break;
        case en_div:
            rv = gen_binary( funcsp, node, flags, size, i_sdiv);
            break;
        case en_udiv:
            rv = gen_binary( funcsp, node, flags, size, i_udiv);
            break;
        case en_mod:
            rv = gen_binary( funcsp, node, flags, size, i_smod);
            break;
        case en_umod:
            rv = gen_binary( funcsp, node, flags, size, i_umod);
            break;
        case en_lsh:
            rv = gen_binary( funcsp, node, flags, size, i_lsl);
            break;
        case en_arraylsh:
            rv = gen_binary( funcsp, node, flags, size, /*i_arraylsh*/ i_lsl);
            break;
        case en_rshd:
            rv = gen_asrhd( funcsp, node, flags, size, i_asr);
            break;
        case en_rsh:
            rv = gen_binary( funcsp, node, flags, size, i_asr);
            break;
        case en_ursh:
            rv = gen_binary( funcsp, node, flags, size, i_lsr);
            break;
        case en_assign:
            rv = gen_assign( funcsp, node, flags, size);
            break;
        case en_blockassign:
            rv = gen_moveblock(node, funcsp);
            break;
        case en_blockclear:
            rv = gen_clearblock(node, funcsp);
            break;
        case en_autoinc:
            rv = gen_aincdec( funcsp, node, flags, size, i_add);
            break;
        case en_autodec:
            rv = gen_aincdec( funcsp, node, flags, size, i_sub);
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
            rv = ap1 ;
            break;
        case en_atomic:
            ap1 = gen_atomic(funcsp, node, flags, size);
            rv = ap1;
            break;
        case en_cond:
            ap1 = gen_hook(funcsp, node, flags, size);
            rv = ap1 ;
            break;
        case en_void:
            gen_void(node->left, funcsp);
            ap1 = gen_expr( funcsp, node->right, flags, size);
            rv = ap1;
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
            ap1 = gen_expr( funcsp, node->left, flags, size);
            if (!node->dest && xcexp)
            {
                node->v.t.thisptr->xcInit = ++consIndex;
                xcexp->right->v.i = consIndex;
                gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
            }
            if (node->left->type == en_stmt)
            {
                rv = tempreg(ISZ_ADDR, 0);
                rv->retval = TRUE;
            }
            else
            {  
                rv = ap1; 
            }
            break;
        case en_structadd:
            rv = gen_binary( funcsp, node,flags,ISZ_ADDR,/*i_struct*/ i_add);
            break;
        case en_arrayadd:
            rv = gen_binary( funcsp, node,flags,ISZ_ADDR,/*i_arrayindex*/ i_add);
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
            ap1 = LookupLoadTemp(NULL, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, NULL);
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
        case en_const:
            /* should never get here unless the constant optimizer is turned off */
            ap1 = make_immed(natural_size(node), node->v.sp->value.i);
            rv = ap1;
            break;
        default:
             /* explicitly uncoded */
            diag("uncoded node in gen_expr.");
            rv = 0;
    }
    if (flags & F_NOVALUE)
    {
        DumpIncDec(funcsp);
    }
    doatomicFence(funcsp, NULL, node->right, rbarrier);
    doatomicFence(funcsp, NULL, node->left, lbarrier);
    return rv;
}

/*-------------------------------------------------------------------------*/

IMODE *gen_void(EXPRESSION *node, SYMBOL *funcsp)
{
    gen_expr( funcsp, node, F_NOVALUE, natural_size(node));
    /*   gen_code(op_void,0,0);*/
    return 0;
}

/*-------------------------------------------------------------------------*/

int natural_size(EXPRESSION *node)
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
        case en_stmt:
            return natural_size(node->left);
        case en_func:
        case en_intcall:
            if (!node->v.func->functp || !isfunction(node->v.func->functp))
                return 0;
            if (isstructured(basetype(node->v.func->functp)->btp) || basetype(node->v.func->functp)->btp->type == bt_memberptr)
                return ISZ_ADDR;
            else if (node->v.func->ascall)
                return sizeFromType(basetype(node->v.func->functp)->btp);
            else
                return ISZ_ADDR;
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_nullptr:        
            return ISZ_ADDR;
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
            return  - ISZ_USHORT;
        case en_l_wc:
        case en_x_wc:
        case en_c_wc:
        case en_c_us:
        case en_l_us:
        case en_x_us:
            return  ISZ_USHORT;
        case en_c_l:
        case en_l_l:
        case en_x_l:
            return - ISZ_ULONG;
        case en_c_i:
        case en_l_i:
        case en_x_i:
            return  - ISZ_UINT;
        case en_c_ul:
        case en_l_ul:
        case en_x_ul:
            return ISZ_ULONG;
        case en_c_ui:
        case en_l_ui:
        case en_x_ui:
            return ISZ_UINT;
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
            return  - ISZ_ULONGLONG;
        case en_imode:
            return ISZ_ADDR;
        case en_trapcall:
        case en_label:
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
            return - ISZ_UINT;
        case en_void:
            return natural_size(node->right);
        case en_cond:
            return natural_size(node->right);
        case en_atomic:
            return -ISZ_UINT;
        case en_voidnz:
            return natural_size(node->right);
        case en_const:
            return sizeFromType(node->v.sp->tp);
        case en_templateparam: // may get this during the initial parsing of the template
            return -ISZ_UINT;
        default:
            diag("natural size error.");
            break;
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

void gen_compare(EXPRESSION *node, SYMBOL *funcsp, int btype, int label)
/*
 *      generate code to do a comparison of the two operands of
 *      node.
 */
{
    IMODE *ap1,  *ap2, *ap3;
    IMODE *barrier;
    int siz0,siz1,size;
    siz0 = natural_size(node->left);
    siz1 = natural_size(node->right);
    if (chksize(siz1, siz0))
        size = siz1;
    else
        size = siz0;
    // the ordering here is to accomodate the x86 FP stack
    ap3 = gen_expr( funcsp, node->right, F_COMPARE, size);
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
    {
        if (node->right->isatomic)
        {
            barrier = doatomicFence(funcsp, NULL, node->right, NULL);
        }
        gen_icode(i_assn, ap2, ap3, NULL);
        if (node->right->isatomic)
        {
            doatomicFence(funcsp, NULL, node->right, barrier);
        }
    }
    ap3 = gen_expr( funcsp, node->left, F_COMPARE, size);
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
    {
        if (node->left->isatomic)
        {
            barrier = doatomicFence(funcsp, NULL, node->left, NULL);
        }
        gen_icode(i_assn, ap1, ap3, NULL);
        if (node->left->isatomic)
        {
            doatomicFence(funcsp, NULL, node->left, barrier);
        }
    }
    if (incdecList)
    {
        // this is needed for the optimizer...  the incdec confuses it
        if (ap1->mode != i_immed)
        {
            ap3 = tempreg(ap1->size, 0);
            gen_icode(i_assn, ap3, ap1, NULL);
            ap1 = ap3;
        }
        if (ap2->mode != i_immed)
        {
            ap3 = tempreg(ap2->size, 0);
            gen_icode(i_assn, ap3, ap2, NULL);
            ap2 = ap3;
        }
        DumpIncDec(funcsp);
    }
    gen_icgoto(btype, label, ap1, ap2);
}

/*-------------------------------------------------------------------------*/

IMODE *gen_set(EXPRESSION *node, SYMBOL *funcsp, int btype)
{
    IMODE *ap1,  *ap2,  *ap3;
    int siz0,siz1,size;
    siz0 = natural_size(node->left);
    siz1 = natural_size(node->right);
    if (chksize(siz1, siz0))
        size = siz1;
    else
        size = siz0;
    ap3 = gen_expr( funcsp, node->left, 0, size);
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);
    ap3 = gen_expr( funcsp, node->right, 0, size);
    ap2 = LookupLoadTemp(NULL, ap3);
    if (ap2 != ap3)
        gen_icode(i_assn, ap2, ap3, NULL);
    ap3 = tempreg(ISZ_UINT, 0);
    gen_icode(btype, ap3, ap1, ap2);

    return ap3;
}

/*-------------------------------------------------------------------------*/

IMODE *defcond(EXPRESSION *node, SYMBOL *funcsp)
{
    IMODE *ap1,  *ap2,  *ap3;
    int size = natural_size(node);
    ap3 = gen_expr( funcsp, node->left, 0, size);
    ap1 = LookupLoadTemp(NULL, ap3);
    if (ap1 != ap3)
        gen_icode(i_assn, ap1, ap3, NULL);
    ap2 = make_immed(ap1->size,0);
    ap3 = tempreg(ISZ_UINT, 0);
    gen_icode(i_sete, ap3, ap2, ap1);
    return ap3;
}

/*-------------------------------------------------------------------------*/


static IMODE *truerelat(EXPRESSION *node, SYMBOL *funcsp)
{
    IMODE *ap1, *ap3;
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
            lab0 = nextLabel++;
            lab1 = nextLabel++;
            truejp(node, funcsp, lab0);
            ap1 = make_immed(ISZ_UINT, 0);
            gen_igoto(i_goto, lab1);
            gen_label(lab0);
            ap1 = make_immed(ISZ_UINT, 1);
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

IMODE *gen_relat(EXPRESSION *node, SYMBOL *funcsp)
{
    long lab1, lab2;
    IMODE *ap1;
    if (node->type != en_land && node->type != en_lor)
    {
        ap1 = truerelat(node, funcsp);
    }
    else
    {
        ap1 = tempreg(ISZ_UINT, 0);
        ap1->offset->v.sp->pushedtotemp = TRUE; // don't lazily optimize this temp...
        lab1 = nextLabel++, lab2 = nextLabel++;
        truejp(node, funcsp, lab1);
        gen_iiconst(ap1,0);
        gen_igoto(i_goto, lab2);
        gen_label(lab1);
        gen_iiconst(ap1,1);
        gen_label(lab2);
    }
    return ap1;
}

/*-------------------------------------------------------------------------*/

void truejp(EXPRESSION *node, SYMBOL *funcsp, int label)
/*
 *      generate a jump to label if the node passed evaluates to
 *      a true condition.
 */
{
    EXPRESSION nnode;
    IMODE *ap1, *ap2, *ap3, *ap4;
    int siz1, siz2;
    int lab0;
    int i;
    if (node == 0)
        return ;
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
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr( funcsp, node->left, 0, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            ap2 = gen_expr( funcsp, node->right, 0, ISZ_UINT);
            ap1 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap1, ap2, 0);
            DumpIncDec(funcsp);
            for (i=0; i < siz1/siz2; i++)
            {
                ap4 = indnode(ap3, ap3->size);
                ap2 = indnode(ap1, ap1->size);
                gen_icgoto(i_jne, label, ap4, ap2);
                gen_icode(i_add, ap3, ap3, make_immed(ap3->size, siz2));
                gen_icode(i_add, ap1, ap1, make_immed(ap3->size, siz2));
            }
            break;
        case en_mp_as_bool:
            nnode = *node;
            nnode.type = en_l_i;
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr( funcsp, &nnode, F_ADDR, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            DumpIncDec(funcsp);
            for (i=0; i < siz1/siz2; i++)
            {
                ap2 = indnode(ap3, ap3->size);
                gen_icgoto(i_jne, label, ap2, make_immed(ap3->size,0));
                gen_icode(i_add, ap3, ap3, make_immed(ap3->size, siz2));
                
            }
            break;
        default:
            siz1 = natural_size(node);
            ap3 = gen_expr( funcsp, node, F_COMPARE, siz1);
            ap1 = LookupLoadTemp(NULL, ap3);
            if (ap1 != ap3)
                gen_icode(i_assn, ap1, ap3, NULL);
            DumpIncDec(funcsp);
            gen_icgoto(i_jne, label, ap1, make_immed(ap1->size,0));
            break;
    }
}

/*-------------------------------------------------------------------------*/

void falsejp(EXPRESSION *node, SYMBOL *funcsp, int label)
/*
 *      generate code to execute a jump to label if the expression
 *      passed is false.
 */
{
    IMODE *ap, *ap1, *ap2, *ap3, *ap4;
    EXPRESSION nnode;
    int siz1, siz2;
    int lab0;
    int i;
    if (node == 0)
        return ;
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
            lab0 = nextLabel++;
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr( funcsp, node->left, 0, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            ap2 = gen_expr( funcsp, node->right, 0, ISZ_UINT);
            ap1 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap1, ap2, 0);
            DumpIncDec(funcsp);
            for (i=0; i < siz1/siz2-1; i++)
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
            nnode = *node;
            nnode.type = en_l_i;
            lab0 = nextLabel++;
            siz1 = node->size;
            siz2 = getSize(bt_int);
            ap2 = gen_expr( funcsp, &nnode, F_ADDR, ISZ_UINT);
            ap3 = tempreg(ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap2, 0);
            DumpIncDec(funcsp);
            for (i=0; i < siz1/siz2-1; i++)
            {
                ap2 = indnode(ap3, ap3->size);
                gen_icgoto(i_jne, lab0, ap2, make_immed(ap3->size,0));
                gen_icode(i_add, ap3, ap3, make_immed(ap3->size, siz2));
            }
            gen_icgoto(i_je, label, ap2, make_immed(ap3->size, 0));
            gen_label(lab0);
            break;
        default:
            siz1 = natural_size(node);
            ap3 = gen_expr( funcsp, node, F_COMPARE, siz1);
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
                gen_icode(i_assn, ap, ap3, NULL);
            DumpIncDec(funcsp);
            gen_icgoto(i_je, label, ap, make_immed(ap->size,0));
            break;
    }
}
