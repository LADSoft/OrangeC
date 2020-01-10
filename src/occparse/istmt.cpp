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
 * istmt.c
 *
 * change the statement list to icode
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "compiler.h"
#include "rtti.h"

extern int tempCount;
extern BLOCK* currentBlock;
extern BLOCK** blockArray;
extern int blockMax;
extern int virtualfuncs;
extern TYPE stdpointer;
extern NAMESPACEVALUELIST* globalNameSpace;
extern int calling_inline;
extern QUAD *intermed_head, *intermed_tail;
extern ARCH_ASM* chosenAssembler;
extern COMPILER_PARAMS cparams;
extern IMODE* structret_imode;
extern TYPE stdfunc;
extern int blockCount, exitBlock;
extern TEMP_INFO** tempInfo;
extern bool functionHasAssembly;
extern TYPE stddouble;
extern SimpleExpression* objectArray_exp;
#ifdef DUMP_GCSE_INFO
extern FILE* icdFile;
#endif
extern BLOCK** blockArray;
extern int maxBlocks, maxTemps;
extern int retlab, startlab;
extern std::vector<SimpleSymbol*> temporarySymbols;
extern int inlinesym_count;
extern int tempBottom, nextTemp;
extern TYPE stdint;
extern SimpleSymbol* baseThisPtr;
extern SYMBOL* theCurrentFunc;
extern std::vector<SimpleSymbol*> functionVariables;
extern std::vector<SimpleSymbol*> globalCache;

IMODE* returnImode;
int retcount;
int consIndex;
EXPRESSION* xcexp;
int catchLevel;
int codeLabelOffset;
SimpleSymbol* currentFunction;

static LIST* mpthunklist;

static int breaklab;
static int contlab;
static int tryStart, tryEnd;
static int plabel;

IMODE* genstmt(STATEMENT* stmt, SYMBOL* funcsp);

void genstmtini(void)
{
    mpthunklist = nullptr;
    returnImode = nullptr;
}

/*-------------------------------------------------------------------------*/


IMODE* imake_label(int label)
/*
 *      make a node to reference an immediate value i.
 */

{
    IMODE* ap = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset = (SimpleExpression*)Alloc(sizeof(SimpleExpression));
    ap->offset->type = se_labcon;
    ap->offset->i = label;
    ap->size = ISZ_ADDR;
    return ap;
}
/*-------------------------------------------------------------------------*/

IMODE* make_direct(int i)
/*
 *      make a direct reference to an immediate value.
 */
{
    return make_ioffset(intNode(en_c_i, i));
}

/*-------------------------------------------------------------------------*/

void gen_genword(STATEMENT* stmt, SYMBOL* funcsp)
/*
 * generate data in the code seg
 */
{
    (void)funcsp;
    gen_icode(i_genword, 0, make_immed(ISZ_UINT, (int)stmt->select->v.i), 0);
}

/*-------------------------------------------------------------------------*/
IMODE* set_symbol(const char* name, int isproc)
/*
 *      generate a call to a library routine.
 */
{
    SYMBOL* sym;
    IMODE* result;
    (void)isproc;
    sym = gsearch(name);
    if (sym == 0)
    {
        LIST* l1;
        sym = (SYMBOL*)Alloc(sizeof(SYMBOL));
        sym->storage_class = sc_external;
        sym->name = sym->decoratedName = litlate(name);
        SymbolManager::Get(sym);
        sym->tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
        sym->tp->type = isproc ? bt_func : bt_int;
        sym->safefunc = true;
        insert(sym, globalNameSpace->valueData->syms);
    }
    else
    {
        if (sym->storage_class == sc_overloads)
            sym = (SYMBOL*)(sym->tp->syms->table[0]->p);
    }
    result = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
    result->offset = (SimpleExpression*)Alloc(sizeof(SimpleExpression));
    result->offset->type = se_global;
    result->offset->sp = SymbolManager::Get(sym);
    result->mode = i_direct;
    if (isproc)
    {
        result->offset->type = se_pc;
        if (chosenAssembler->arch->libsasimports)
            result->mode = i_direct;
        else
            result->mode = i_immed;
    }
    return result;
}


/*-------------------------------------------------------------------------*/

IMODE* call_library(const char* lib_name, int size)
/*
 *      generate a call to a library routine.
 */
{
    IMODE* result;
    result = set_symbol(lib_name, 1);
    gen_icode(i_gosub, 0, result, 0);
    gen_icode(i_parmadj, 0, make_parmadj(size), make_parmadj(size));
    result = tempreg(ISZ_UINT, 0);
    result->retval = true;
    return result;
}
static void AddProfilerData(SYMBOL* funcsp)
{
    LCHAR* pname;
    if (cparams.prm_profiler)
    {
        STRING* string;
        int i;
        int l = strlen(funcsp->decoratedName);
        pname = (LCHAR*)Alloc(sizeof(LCHAR) * l + 1);
        for (i = 0; i < l + 1; i++)
            pname[i] = funcsp->decoratedName[i];
        string = (STRING*)Alloc(sizeof(STRING));
        string->strtype = l_astr;
        string->size = 1;
        string->pointers = (SLCHAR**)Alloc(sizeof(SLCHAR*));
        string->pointers[0] = (SLCHAR*)Alloc(sizeof(SLCHAR));
        string->pointers[0]->str = pname;
        string->pointers[0]->count = l;
        string->suffix = nullptr;
        stringlit(string);
        plabel = string->label;
        gen_icode(i_parm, 0, imake_label(plabel), 0);
        call_library("__profile_in", getSize(bt_pointer));
    }
}

//-------------------------------------------------------------------------

void SubProfilerData(void)
{
    if (cparams.prm_profiler)
    {
        gen_icode(i_parm, 0, imake_label(plabel), 0);
        call_library("__profile_out", getSize(bt_pointer));
    }
}

/*-------------------------------------------------------------------------*/
void count_cases(CASEDATA* cd, struct cases* cs)
{
    while (cd)
    {
        cs->count++;
        if (cd->val < cs->bottom)
            cs->bottom = cd->val;
        if (cd->val > cs->top)
            cs->top = cd->val;
        cd = cd->next;
    }
}
/*-------------------------------------------------------------------------*/
void gather_cases(CASEDATA* cd, struct cases* cs)
{
    int pos = 0;
    if (!cs->ptrs)
    {
        cs->ptrs = (struct caseptrs*)Alloc((cs->count) * sizeof(struct caseptrs));
    }
    while (cd)
    {
        cs->ptrs[pos].label = cd->label + codeLabelOffset;
        cs->ptrs[pos++].id = cd->val;
        cd = cd->next;
    }
}

/*-------------------------------------------------------------------------*/

int gcs_compare(void const* left, void const* right)
{
    struct caseptrs const* lleft = (struct caseptrs const*)left;
    struct caseptrs const* lright = (struct caseptrs const*)right;
    if (lleft->id < lright->id)
        return -1;
    return lleft->id > lright->id;
}
/*-------------------------------------------------------------------------*/

void genxswitch(STATEMENT* stmt, SYMBOL* funcsp)
/*
 *      analyze and generate best switch statement.
 */
{
    int oldbreak, i;
    struct cases cs;
    IMODE *ap, *ap3;

#ifdef USE_LONGLONG
    unsigned long long a = 1;
#endif
    oldbreak = breaklab;
    breaklab = stmt->breaklabel + codeLabelOffset;
    memset(&cs, 0, sizeof(cs));
#ifndef USE_LONGLONG
    cs.top = INT_MIN;
    cs.bottom = INT_MAX;
#else
    cs.top = (a << 63);     /* LLONG_MIN*/
    cs.bottom = cs.top - 1; /* LLONG_MAX*/
#endif
    count_cases(stmt->cases, &cs);
    cs.top++;
    ap3 = gen_expr(funcsp, stmt->select, F_VOL | F_SWITCHVALUE, ISZ_UINT);
    ap = LookupLoadTemp(nullptr, ap3);
    if (ap != ap3)
    {
        IMODE* barrier;
        //        if (stmt->select->isatomic)
        //        {
        //            barrier = doatomicFence(funcsp, nullptr, stmt->select, nullptr);
        //        }
        gen_icode(i_assn, ap, ap3, nullptr);
        //        if (stmt->select->isatomic)
        //        {
        //            doatomicFence(funcsp, nullptr, stmt->select, barrier);
        //        }
    }
    if (chosenAssembler->arch->preferopts & OPT_EXPANDSWITCH)
    {
        EXPRESSION* en = anonymousVar(sc_auto, &stdint);
        en->v.sp->anonymous = false;
        cacheTempSymbol(SymbolManager::Get(en->v.sp));
        if (ap->size != -ISZ_UINT)
        {
            ap3 = tempreg(-ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap, nullptr);
            ap = ap3;
        }
        ap3 = (IMODE*)Alloc(sizeof(IMODE));
        ap3->mode = i_direct;
        ap3->offset = SymbolManager::Get(en);
        ap3->size = -ISZ_UINT;
        gen_icode(i_assn, ap3, ap, nullptr);
        ap = ap3;
    }
    gen_icode2(i_coswitch, make_immed(ISZ_UINT, cs.count), ap, make_immed(ISZ_UINT, cs.top - cs.bottom),
               stmt->label + codeLabelOffset);
    gather_cases(stmt->cases, &cs);
    qsort(cs.ptrs, cs.count, sizeof(cs.ptrs[0]), gcs_compare);
    for (i = 0; i < cs.count; i++)
    {
        gen_icode2(i_swbranch, 0, make_immed(ISZ_UINT, cs.ptrs[i].id), 0, cs.ptrs[i].label);
    }
    breaklab = oldbreak;
}

void genselect(STATEMENT* stmt, SYMBOL* funcsp, bool jmptrue)
{
    if (stmt->altlabel + codeLabelOffset)
    {
        //		gen_label(stmt->altlabel + codeLabelOffset);
        //		intermed_tail->dc.opcode = i_skipcompare;
    }
    if (jmptrue)
        truejp(stmt->select, funcsp, stmt->label + codeLabelOffset);
    else
        falsejp(stmt->select, funcsp, stmt->label + codeLabelOffset);
    if (stmt->altlabel + codeLabelOffset)
    {
        //		gen_label(stmt->altlabel + codeLabelOffset);
        //		intermed_tail->dc.opcode = i_skipcompare;
    }
}
/*-------------------------------------------------------------------------*/
static void gen_try(SYMBOL* funcsp, STATEMENT* stmt, int startLab, int endLab, int transferLab, STATEMENT* lower)
{
    gen_label(startLab);
    stmt->tryStart = ++consIndex;
    xcexp->right->v.i = consIndex;
    gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
    genstmt(lower, funcsp);
    stmt->tryEnd = ++consIndex;
    xcexp->right->v.i = consIndex;
    gen_expr(funcsp, xcexp, F_NOVALUE, ISZ_ADDR);
    gen_label(endLab);
    /* not using gen_igoto because it will make a new block */
    gen_icode(i_goto, nullptr, nullptr, nullptr);
    intermed_tail->dc.v.label = transferLab;
    tryStart = stmt->tryStart;
    tryEnd = stmt->tryEnd;
}
static void gen_catch(SYMBOL* funcsp, STATEMENT* stmt, int startLab, int transferLab, STATEMENT* lower)
{
    int oldtryStart = tryStart;
    int oldtryEnd = tryEnd;
    gen_label(startLab);
    currentBlock->alwayslive = true;
    intermed_tail->alwayslive = true;
    catchLevel++;
    genstmt(lower, funcsp);
    catchLevel--;
    /* not using gen_igoto because it will make a new block */
    gen_icode(i_goto, nullptr, nullptr, nullptr);
    intermed_tail->dc.v.label = transferLab;
    tryStart = oldtryStart;
    tryEnd = oldtryEnd;
    stmt->tryStart = tryStart;
    stmt->tryEnd = tryEnd;
}
static STATEMENT* gen___try(SYMBOL* funcsp, STATEMENT* stmt)
{
    int label = nextLabel++;
    while (stmt)
    {
        int mode = 0;
        IMODE* left = nullptr;
        switch (stmt->type)
        {
            case st___try:
                mode = 1;
                break;
            case st___catch:
                mode = 2;
                if (stmt->sp)
                {
                    left = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
                    left->mode = i_direct;
                    left->size = ISZ_OBJECT;
                    left->offset = (SimpleExpression*)(SimpleExpression*)Alloc(sizeof(SimpleExpression));
                    left->offset->type = se_auto;
                    left->offset->sp = SymbolManager::Get(stmt->sp);
                }
                break;
            case st___fault:
                mode = 3;
                break;
            case st___finally:
                mode = 4;
                break;
            default:
                gen_label(label);
                return stmt;
        }
        gen_icode(i_seh, nullptr, left, nullptr);
        intermed_tail->alwayslive = true;
        intermed_tail->sehMode = mode | 0x80;
        intermed_tail->dc.v.label = label;
        genstmt(stmt->lower, funcsp);
        gen_icode(i_seh, nullptr, left, nullptr);
        intermed_tail->sehMode = mode;
        intermed_tail->dc.v.label = label;
        stmt = stmt->next;
        if (stmt && stmt->type == st___try)
            break;
    }
    gen_label(label);
    return stmt;
}
/*
 *      generate a return statement.
 */
void genreturn(STATEMENT* stmt, SYMBOL* funcsp, int flag, int noepilogue, IMODE* allocaAP)
{
    IMODE *ap = nullptr, *ap1, *ap3;
    EXPRESSION ep;
    int size;
    /* returns a value? */
    if (stmt != 0 && stmt->select != 0)
    {
        // the return type should NOT be an array at this point unless it is a managed one...
        if (basetype(funcsp->tp)->btp &&
            (isstructured(basetype(funcsp->tp)->btp) || (isarray(basetype(funcsp->tp)->btp) && (architecture == ARCHITECTURE_MSIL)) ||
             basetype(basetype(funcsp->tp)->btp)->type == bt_memberptr))
        {
            if (architecture == ARCHITECTURE_MSIL)
            {
                EXPRESSION* exp = stmt->select;
                while (castvalue(exp))
                    exp = exp->left;
                if (isconstzero(&stdint, exp))
                    ap = make_immed(ISZ_OBJECT, 0);  // LDNULL
                else
                    ap = gen_expr(funcsp, stmt->select, F_OBJECT | F_INRETURN, ISZ_ADDR);
                DumpIncDec(funcsp);
            }
            else
            {
                EXPRESSION* en = anonymousVar(sc_parameter, &stdpointer);
                SYMBOL* sym = en->v.sp;
                ap = gen_expr(funcsp, stmt->select, 0, ISZ_ADDR);
                DumpIncDec(funcsp);
                sym->offset = chosenAssembler->arch->retblocksize;
                sym->name = "__retblock";
                sym->allocate = false;
                if ((funcsp->attribs.inheritable.linkage == lk_pascal) && basetype(funcsp->tp)->syms->table[0] &&
                    ((SYMBOL*)basetype(funcsp->tp)->syms->table[0])->tp->type != bt_void)
                {
                    sym->offset = funcsp->paramsize;
                }
                SimpleSymbol *ssym = SymbolManager::Get(sym);
                ssym->offset = sym->offset;
                ssym->allocate = false;
                deref(&stdpointer, &en);
                ap = gen_expr(funcsp, en, 0, ISZ_ADDR);
            }
            size = ISZ_ADDR;
        }
        else
        {
            EXPRESSION* exp = stmt->select;
            while (castvalue(exp))
                exp = exp->left;
            size = natural_size(stmt->select);
            if (size == ISZ_OBJECT && isconstzero(&stdint, exp))
                ap3 = make_immed(ISZ_OBJECT, 0);  // LDNULL
            else
                ap3 = gen_expr(funcsp, stmt->select, 0, size);
            DumpIncDec(funcsp);
            ap = LookupLoadTemp(nullptr, ap3);
            if (ap != ap3)
            {
                IMODE* barrier;
                //                if (stmt->select->isatomic)
                //                {
                //                    barrier = doatomicFence(funcsp, nullptr, stmt->select, nullptr);
                //                }
                gen_icode(i_assn, ap, ap3, nullptr);
                //                if (stmt->select->isatomic)
                //                {
                //                    doatomicFence(funcsp, nullptr, stmt->select, barrier);
                //                }
            }
            if (abs(size) < ISZ_UINT)
                size = -ISZ_UINT;
        }
    }
    else
    {
        DumpIncDec(funcsp);
    }
    if (stmt && stmt->destexp)
    {
        gen_expr(funcsp, stmt->destexp, F_NOVALUE, ISZ_ADDR);
    }
    if (ap)
    {
        if (returnImode)
            ap1 = returnImode;
        else
        {
            ap1 = tempreg(ap->size, 0);
            if (!inlinesym_count)
                ap1->retval = true;
            else
                returnImode = ap1;
        }
        gen_icode(i_assn, ap1, ap, 0);
    }
    /* create the return or a branch to the return
     * return is put at end of function...
     */
    if (flag)
    {
        int retsize = 0;
        if (funcsp->attribs.inheritable.linkage == lk_pascal || funcsp->attribs.inheritable.linkage == lk_stdcall)
        {
            retsize = funcsp->paramsize;
        }
        gen_label(retlab);
        intermed_tail->retcount = retcount;
        if (!noepilogue)
        {

            if (!inlinesym_count)
            {
                if (allocaAP && (architecture != ARCHITECTURE_MSIL))
                {
                    gen_icode(i_loadstack, 0, allocaAP, 0);
                }
                /*			if (funcsp->loadds && funcsp->farproc)
                                gen_icode(i_unloadcontext,0,0,0);
                */
                if (cparams.prm_xcept && funcsp->xc && funcsp->xc->xcRundownFunc)
                    gen_expr(funcsp, funcsp->xc->xcRundownFunc, F_NOVALUE, ISZ_UINT);
                SubProfilerData();
                gen_icode(i_epilogue, 0, 0, 0);
                if (funcsp->attribs.inheritable.linkage == lk_interrupt || funcsp->attribs.inheritable.linkage == lk_fault)
                {
                    /*				if (funcsp->loadds)
                                        gen_icode(i_unloadcontext,0,0,0);
                    */
                    gen_icode(i_popcontext, 0, 0, 0);
                    gen_icode(i_rett, 0, make_immed(ISZ_UINT, funcsp->attribs.inheritable.linkage == lk_interrupt), 0);
                }
                else
                {
                    gen_icode(i_ret, 0, make_immed(ISZ_UINT, retsize), nullptr);
                }
            }
        }
    }
    else
    {
        /* not using gen_igoto because it will make a new block */
        gen_icode(i_goto, nullptr, nullptr, nullptr);
        intermed_tail->dc.v.label = retlab;
        retcount++;
    }
}

void gen_varstart(void* exp)
{
    if (cparams.prm_debug)
    {
        IMODE* ap = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
        ap->mode = i_immed;
        ap->offset = SymbolManager::Get((expr*)exp);
        ap->size = ISZ_ADDR;
        gen_icode(i_varstart, 0, ap, 0);
    }
}
void gen_func(void* exp, int start)
{
    IMODE* ap = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset = SymbolManager::Get((expr*)exp);
    ap->size = ISZ_ADDR;
    gen_icode(i_func, 0, ap, 0)->dc.v.label = start;
}
void gen_dbgblock(int start) { gen_icode(start ? i_dbgblock : i_dbgblockend, 0, 0, 0); }

void gen_asm(STATEMENT* stmt)
/*
 * generate an ASM statement
 */
{
    QUAD* newQuad;
    newQuad = (QUAD*)(QUAD*)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_passthrough;
    newQuad->dc.left = (IMODE*)stmt->select; /* actually is defined by the INASM module*/
    //if (chosenAssembler->gen->adjust_codelab)
    //    chosenAssembler->gen->adjust_codelab(newQuad->dc.left, codeLabelOffset);
    adjust_codelab(newQuad->dc.left, codeLabelOffset);
    flush_dag();
    add_intermed(newQuad);
}
void gen_asmdata(STATEMENT* stmt)
{
    QUAD* newQuad;
    newQuad = (QUAD*)(QUAD*)Alloc(sizeof(QUAD));
    newQuad->dc.opcode = i_datapassthrough;
    newQuad->dc.left = (IMODE*)stmt->select; /* actually is defined by the INASM module*/
    flush_dag();
    add_intermed(newQuad);
}

/*-------------------------------------------------------------------------*/

IMODE* genstmt(STATEMENT* stmt, SYMBOL* funcsp)
/*
 *      genstmt will generate a statement and follow the next pointer
 *      until the block is generated.
 */
{
    IMODE* rv = nullptr;
    while (stmt != 0)
    {
        STATEMENT* last = stmt;
        switch (stmt->type)
        {
            case st_nop:
                break;
            case st_varstart:
                gen_varstart(stmt->select);
                break;
            case st_dbgblock:
                gen_dbgblock(stmt->label);
                break;
                break;
            case st_block:
                rv = genstmt(stmt->lower, funcsp);
                genstmt(stmt->blockTail, funcsp);
                break;
            case st_label:
                gen_label((int)stmt->label + codeLabelOffset);
                break;
            case st_goto:
                if (stmt->destexp)
                {
                    gen_expr(funcsp, last->destexp, F_NOVALUE, ISZ_ADDR);
                }
                gen_igoto(i_goto, (int)stmt->label + codeLabelOffset);
                break;
            case st_asmgoto:
                gen_igoto(i_asmgoto, (int)stmt->label + codeLabelOffset);
                break;
            case st_asmcond:
                gen_igoto(i_asmcond, (int)stmt->label + codeLabelOffset);
                break;
            case st_try:
                gen_try(funcsp, stmt, stmt->label + codeLabelOffset, stmt->endlabel + codeLabelOffset,
                        stmt->breaklabel + codeLabelOffset, stmt->lower);
                break;
            case st_catch:
            {
                STATEMENT* last = nullptr;
                while (stmt && stmt->type == st_catch)
                {
                    // the following adjustment to altlabel is required to get the XT info proper
                    gen_catch(funcsp, stmt, stmt->altlabel += codeLabelOffset, stmt->breaklabel + codeLabelOffset, stmt->lower);
                    last = stmt;
                    stmt = stmt->next;
                }
                stmt = last;
                gen_label(stmt->breaklabel + codeLabelOffset);
            }
            break;
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                stmt = gen___try(funcsp, stmt);
                if (last->type != st_return && last->type != st_goto && last->destexp)
                {
                    gen_expr(funcsp, last->destexp, F_NOVALUE, ISZ_ADDR);
                }
                continue;
            case st_expr:
            case st_declare:
                if (stmt->select)
                    rv = gen_expr(funcsp, stmt->select, F_NOVALUE, natural_size(stmt->select));
                break;
            case st_return:
                genreturn(stmt, funcsp, 0, 0, nullptr);
                break;
            case st_line:
                gen_line(stmt->lineData);
                break;
            case st_select:
                genselect(stmt, funcsp, true);
                break;
            case st_notselect:
                genselect(stmt, funcsp, false);
                break;
            case st_switch:
                genxswitch(stmt, funcsp);
                break;
            case st__genword:
                gen_genword(stmt, funcsp);
                break;
            case st_passthrough:
                gen_asm(stmt);
                break;
            case st_datapassthrough:
                gen_asmdata(stmt);
                break;
            default:
                diag("unknown statement.");
                break;
        }
        if (last->type != st_return && last->type != st_goto && last->destexp)
        {
            gen_expr(funcsp, last->destexp, F_NOVALUE, ISZ_ADDR);
        }
        stmt = stmt->next;
    }
    return rv;
}
// simplify addresses to make the aliasing work faster...
// this is necessary because the inlining replaces what were
// references to 'this' with addresses
//
// it also improves teh code generation when inlining functions
// with parameters passed by reference...
typedef struct _data
{
    struct _data* next;
    IMODE* mem;
    IMODE* addr;
} DATA;
static DATA* buckets[32];
static void StoreInBucket(IMODE* mem, IMODE* addr)
{
    DATA* lst;
    int bucket = (int)mem;
    bucket = (bucket >> 16) ^ (bucket >> 8) ^ (bucket >> 2);
    bucket %= 32;
    lst = buckets[bucket];
    while (lst)
    {
        if (lst->mem == mem)
        {
            if (lst->addr != addr)
                lst->mem->offset->sp->noCoalesceImmed = true;
            return;
        }
        lst = lst->next;
    }
    lst = (DATA*)Alloc(sizeof(DATA));
    lst->mem = mem;
    lst->addr = addr;
    lst->next = buckets[bucket];
    buckets[bucket] = lst;
}
static IMODE* GetBucket(IMODE* mem)
{
    DATA* lst;
    int bucket = (int)mem;
    bucket = (bucket >> 16) ^ (bucket >> 8) ^ (bucket >> 2);
    bucket %= 32;
    lst = buckets[bucket];
    while (lst)
    {
        if (lst->mem == mem)
            return lst->addr;
        lst = lst->next;
    }
    return nullptr;
}
/*-------------------------------------------------------------------------*/
static void InsertParameterThunks(SYMBOL* funcsp, BLOCK* b)
{
    SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
    QUAD *old, *oldit;
    BLOCK* ocb = currentBlock;
    old = b->head->fwd;
    while (old != b->tail && old->dc.opcode != i_label)
        old = old->fwd;
    old = old->fwd;
    oldit = intermed_tail;
    intermed_tail = old->back;
    intermed_tail->fwd = nullptr;
    currentBlock = b;
    while (hr)
    {
        SYMBOL* sym = hr->p;
        SimpleSymbol* simpleSym = SymbolManager::Get(sym);
        if (sym->tp->type == bt_void || sym->tp->type == bt_ellipse || isstructured(sym->tp))
        {
            hr = hr->next;
            continue;
        }
        if (!simpleSym->imvalue || simpleSym->imaddress)
        {
            hr = hr->next;
            continue;
        }
        if (funcsp->oldstyle && sym->tp->type == bt_float)
        {
            IMODE* right = (IMODE*)(IMODE*)Alloc(sizeof(IMODE));
            *right = *simpleSym->imvalue;
            right->size = ISZ_DOUBLE;
            if (!chosenAssembler->arch->hasFloatRegs)
            {
                IMODE* dp = tempreg(ISZ_DOUBLE, 0);
                IMODE* fp = tempreg(ISZ_FLOAT, 0);
                /* oldstyle float gets promoted from double */
                gen_icode(i_assn, dp, right, 0);
                gen_icode(i_assn, fp, dp, 0);
                gen_icode(i_assn, simpleSym->imvalue, fp, 0);
            }
        }
        hr = hr->next;
    }
    currentBlock = ocb;
    if (old->back == b->tail)
    {
        b->tail = intermed_tail;
    }
    old->back = intermed_tail;
    intermed_tail->fwd = old;
    intermed_tail = oldit;
}
void CopyVariables(SYMBOL *funcsp)
{
    functionVariables.clear();
    for (HASHTABLE* syms = funcsp->inlineFunc.syms; syms; syms = syms->next)
    {
        for (SYMLIST* hr = syms->table[0]; hr; hr = hr->next)
        {
            if (hr->p->storage_class == sc_auto || hr->p->storage_class == sc_parameter)
            {
                SimpleSymbol *sym = SymbolManager::Get(hr->p);
                sym->i = syms->blockLevel;
                functionVariables.push_back(sym);
            }
        }
    }
}
void genfunc(SYMBOL* funcsp, bool doOptimize)
/*
 *      generate a function body and dump the icode
 */
{
    IMODE* oldReturnImode = returnImode;
    IMODE* allocaAP = nullptr;
    SYMBOL* oldCurrentFunc;
    SimpleSymbol *oldCurrentFunction;
    EXPRESSION* funcexp = varNode(en_global, funcsp);
    SYMBOL* tmpl = funcsp;
    SYMLIST* hr;
    if (TotalErrors())
        return;
    oldCurrentFunction = currentFunction;
    currentFunction = SymbolManager::Get(funcsp);
    currentFunction->initialized = true;
    CopyVariables(funcsp);
    returnImode = nullptr;
    while (tmpl)
        if (tmpl->templateLevel)
            break;
        else
            tmpl = tmpl->parentClass;

    /* for inlines */
    codeLabelOffset = nextLabel - INT_MIN;
    nextLabel += funcsp->labelCount;

    temporarySymbols.clear();
    contlab = breaklab = -1;
    structret_imode = 0;
    tempCount = 0;
    blockCount = 0;
    blockMax = 0;
    exitBlock = 0;
    consIndex = 0;
    retcount = 0;
    objectArray_exp = nullptr;
    oldCurrentFunc = theCurrentFunc;
    theCurrentFunc = funcsp;
    iexpr_func_init();

    if (funcsp->xc && funcsp->xc->xctab)
    {
        EXPRESSION* exp;
        temporarySymbols.push_back(SymbolManager::Get(funcsp->xc->xctab));
        xcexp = varNode(en_auto, funcsp->xc->xctab);
        xcexp = exprNode(en_add, xcexp, intNode(en_c_i, XCTAB_INDEX_OFS));
        deref(&stdpointer, &xcexp);
        exp = intNode(en_c_i, 0);
        xcexp = exprNode(en_assign, xcexp, exp);
    }
    else
    {
        xcexp = nullptr;
    }
    cseg();
    gen_line(funcsp->linedata);
    gen_func(funcexp, 1);
    /* in C99 inlines can clash if declared 'extern' in multiple modules */
    /* in C++ we introduce virtual functions that get coalesced at link time */
    if (funcsp->attribs.inheritable.linkage == lk_virtual || tmpl)
    {
        funcsp->attribs.inheritable.linkage = lk_virtual;
        gen_virtual(SymbolManager::Get(funcsp), false);
    }
    else
    {
        gen_funcref(SymbolManager::Get(funcsp));
        gen_strlab(SymbolManager::Get(funcsp)); /* name of function */
    }
    addblock(-1);
    if (funcsp->attribs.inheritable.linkage == lk_interrupt || funcsp->attribs.inheritable.linkage == lk_fault)
    {
        gen_icode(i_pushcontext, 0, 0, 0);
        /*		if (funcsp->loadds) */
        /*	        gen_icode(i_loadcontext, 0,0,0); */
    }
    gen_icode(i_prologue, 0, 0, 0);
    if (cparams.prm_debug)
    {
        if (basetype(funcsp->tp)->syms->table[0] && ((SYMBOL*)basetype(funcsp->tp)->syms->table[0]->p)->thisPtr)
        {
            EXPRESSION* exp = varNode(en_auto, ((SYMBOL*)basetype(funcsp->tp)->syms->table[0]->p));
            exp->v.sp->tp->used = true;
            gen_varstart(exp);
        }
    }
    else
    {
        if (basetype(funcsp->tp)->syms->table[0] && ((SYMBOL*)basetype(funcsp->tp)->syms->table[0]->p)->thisPtr)
        {
            baseThisPtr = SymbolManager::Get((SYMBOL*)basetype(funcsp->tp)->syms->table[0]->p);
        }
    }
    gen_label(startlab);
    AddProfilerData(funcsp);
    if (cparams.prm_xcept && funcsp->xc && funcsp->xc->xcInitializeFunc)
    {
        gen_expr(funcsp, funcsp->xc->xcInitializeFunc, F_NOVALUE, ISZ_UINT);
        gen_label(funcsp->xc->xcInitLab + codeLabelOffset);
    }
    /*    if (funcsp->loadds && funcsp->farproc) */
    /*	        gen_icode(i_loadcontext, 0,0,0); */
    AllocateLocalContext(nullptr, funcsp, nextLabel++);
    if (funcsp->allocaUsed && (architecture != ARCHITECTURE_MSIL))
    {
        EXPRESSION* allocaExp = anonymousVar(sc_auto, &stdpointer);
        allocaAP = gen_expr(funcsp, allocaExp, 0, ISZ_ADDR);
        gen_icode(i_savestack, 0, allocaAP, 0);
    }
    /* Generate the icode */
    /* LCSE is done while code is generated */
    genstmt(funcsp->inlineFunc.stmt->lower, funcsp);
    if (funcsp->inlineFunc.stmt->blockTail)
    {
        gen_icode(i_functailstart, 0, 0, 0);
        genstmt(funcsp->inlineFunc.stmt->blockTail, funcsp);
        gen_icode(i_functailend, 0, 0, 0);
    }
    genreturn(0, funcsp, 1, 0, allocaAP);
    gen_func(funcexp, 0);
    tFree();
    InsertParameterThunks(funcsp, blockArray[1]);
    FreeLocalContext(nullptr, funcsp, nextLabel++);

//    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
//        AllocateStackSpace(funcsp);
//    FillInPrologue(intermed_head, funcsp);
    /* Code gen from icode */
//    rewrite_icode(); /* Translate to machine code & dump */
//    if (chosenAssembler->gen->post_function_gen)
//        chosenAssembler->gen->post_function_gen(SymbolManager::Get(funcsp), intermed_head);
//    post_function_gen(currentFunction, intermed_head);
    AddFunction();
    if (funcsp->attribs.inheritable.linkage == lk_virtual || tmpl)
        gen_endvirtual(SymbolManager::Get(funcsp));
    AllocateLocalContext(nullptr, funcsp, nextLabel);
    funcsp->retblockparamadjust = chosenAssembler->arch->retblockparamadjust;
    XTDumpTab(funcsp);
    FreeLocalContext(nullptr, funcsp, nextLabel);
    intermed_head = nullptr;
//    dag_rundown();
//    oFree();
    theCurrentFunc = oldCurrentFunc;
    currentFunction = oldCurrentFunction;
    returnImode = oldReturnImode;
    if (blockCount > maxBlocks)
        maxBlocks = blockCount;
    if (tempCount > maxTemps)
        maxTemps = tempCount;

    // this is explicitly to clean up the this pointer
    hr = basetype(funcsp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sym = hr->p;
        if (sym->storage_class == sc_parameter)
        {
            SimpleSymbol* simpleSym = SymbolManager::Get(sym);
            simpleSym->imind = nullptr;
            simpleSym->imvalue = nullptr;
            simpleSym->imaddress = nullptr;
        }
        hr = hr->next;
    }
    baseThisPtr = nullptr;
    nextLabel+=2; // temporary
}
void genASM(STATEMENT* st)
{
    cseg();
    contlab = breaklab = -1;
    structret_imode = 0;
    tempCount = 0;
    blockCount = 0;
    blockMax = 0;
    exitBlock = 0;
    genstmt(st, nullptr);
}