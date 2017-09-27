/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
 
extern BLOCK *currentBlock;
extern BLOCK **blockArray;
extern int blockMax;
extern int virtualfuncs;
extern TYPE stdpointer;
extern NAMESPACEVALUES *globalNameSpace;
extern int calling_inline;
extern QUAD *intermed_head, *intermed_tail;
extern ARCH_ASM *chosenAssembler;
extern COMPILER_PARAMS cparams;
extern IMODE *structret_imode ;
extern TYPE stdfunc;
extern int prm_globsub, prm_copyprop;
extern int prm_optlive;
extern int blockCount, exitBlock;
extern int total_errors;
extern TEMP_INFO **tempInfo;
extern BOOLEAN functionHasAssembly;
extern TYPE stddouble;
extern EXPRESSION *objectArray_exp;
#ifdef DUMP_GCSE_INFO
    extern FILE *icdFile;
#endif 
extern BLOCK **blockArray;
extern int maxBlocks, maxTemps;
extern int retlab, startlab;
extern LIST *temporarySymbols;
extern int inlinesym_count;
extern int tempBottom, nextTemp;
extern TYPE stdint;

IMODE *returnImode;
int retcount;
int consIndex;
EXPRESSION *xcexp;
int catchLevel;
int tempCount;
int optflags = ~0;
int codeLabelOffset;

static LIST *mpthunklist;


static int breaklab;
static int contlab;
static int tryStart, tryEnd;
static int plabel;

IMODE *genstmt(STATEMENT *stmt, SYMBOL *funcsp);

void genstmtini(void)
{
    mpthunklist = NULL;
    returnImode = NULL;
}

/*-------------------------------------------------------------------------*/

EXPRESSION *tempenode(void)
{
    SYMBOL *sp;
    char buf[256];
    sp = Alloc(sizeof(SYMBOL));
    sp->storage_class = sc_temp;
    sprintf(buf, "$$t%d", tempCount);
    sp->name = litlate(buf);
    sp->value.i = tempCount++;
    return varNode(en_tempref, sp);
}
IMODE *tempreg(int size, int mode)
/*
 * create a temporary register
 */
{
    IMODE *ap;
    ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->offset = tempenode();
    ap->offset->v.sp->tp = Alloc(sizeof(TYPE));
    ap->offset->v.sp->tp->type = bt_int;
    ap->offset->v.sp->tp->size = sizeFromISZ(size);
    ap->size = size;
    if (mode)
    {
        ap->mode = i_immed;
        ap->offset->v.sp->imaddress = ap;
    }
    else
    {
        ap->mode = i_direct;
        ap->offset->v.sp->imvalue = ap;
    }
    return ap;
}

IMODE *imake_label(int label)
/*
 *      make a node to reference an immediate value i.
 */

{
    IMODE *ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset = exprNode(en_labcon, NULL, NULL);
    ap->offset->v.i = label;
    ap->size = ISZ_ADDR;
        return ap;
}
/*-------------------------------------------------------------------------*/

IMODE *make_direct(int i)
/*
 *      make a direct reference to an immediate value.
 */
{
    return make_ioffset(intNode(en_c_i, i));
}


/*-------------------------------------------------------------------------*/

void gen_genword(STATEMENT *stmt, SYMBOL *funcsp)
/*
 * generate data in the code seg
 */
{
    (void)funcsp;
    gen_icode(i_genword, 0, make_immed(ISZ_UINT,(int)stmt->select), 0);
}

/*-------------------------------------------------------------------------*/

IMODE *set_symbol(char *name, int isproc)
/*
 *      generate a call to a library routine.
 */
{
    SYMBOL *sp;
    IMODE *result;
    (void) isproc;
    sp = gsearch(name);
    if (sp == 0)
    {
        LIST *l1;
        IncGlobalFlag();
        sp = (SYMBOL *)Alloc(sizeof(SYMBOL));
        sp->storage_class = sc_external;
        sp->name = sp->errname = sp->decoratedName = litlate(name);
        GENREF(sp);
        sp->tp = (TYPE *)Alloc(sizeof(TYPE));
        sp->tp->type = isproc ? bt_func : bt_int;
        sp->safefunc = TRUE;
        insert(sp, globalNameSpace->syms);
        InsertExtern(sp);
        DecGlobalFlag();
    }
    else
    {
        if (sp->storage_class == sc_overloads)
            sp = (SYMBOL *)(sp->tp->syms->table[0]->p);
    }
    result = (IMODE *)Alloc(sizeof(IMODE));
    result->offset = varNode(en_global, sp);
    result->mode = i_direct;
    if (isproc)
    {
        result->offset->type = en_pc;
        if (chosenAssembler->arch->libsasimports)
            result->mode = i_direct;
        else
            result->mode = i_immed;
    }
    return result;
}

/*-------------------------------------------------------------------------*/

IMODE *call_library(char *lib_name, int size)
/*
 *      generate a call to a library routine.
 */
{
    IMODE *result;
    result = set_symbol(lib_name, 1);
    gen_icode(i_gosub, 0, result, 0);
    gen_icode(i_parmadj, 0, make_parmadj(size), make_parmadj(size));
    result = tempreg(ISZ_UINT, 0);
    result->retval = TRUE;
    return result;
}
static AddProfilerData(SYMBOL *funcsp)
{
    LCHAR *pname;
    if (cparams.prm_profiler)
    {
        STRING *string;
        int i;
        int l = strlen(funcsp->decoratedName);
        pname = Alloc(sizeof(LCHAR)*l + 1);
        for (i=0; i < l+1; i++)
             pname[i] = funcsp->decoratedName[i];
        string = (STRING *)Alloc(sizeof(STRING));
        string->strtype = l_astr;
        string->size = 1;
        string->pointers = Alloc(sizeof(SLCHAR * ));
        string->pointers[0] = Alloc(sizeof(SLCHAR));
        string->pointers[0]->str = pname;
        string->pointers[0]->count = l;
        string->suffix = NULL;
        stringlit (string);
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
void count_cases(CASEDATA *cd, struct cases *cs)
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
void gather_cases(CASEDATA *cd, struct cases *cs)
{
    int pos = 0;
    if (!cs->ptrs) {
        IncGlobalFlag();
        cs->ptrs = (struct caseptrs *)Alloc((cs->count) * sizeof(struct caseptrs));
        DecGlobalFlag();
    }
    while (cd)
    {
        cs->ptrs[pos].label = cd->label + codeLabelOffset;
        cs->ptrs[pos++].id = cd->val;
        cd = cd->next;
    }
}

/*-------------------------------------------------------------------------*/

int gcs_compare(void const *left, void const *right)
{
    struct caseptrs const *lleft = left;
    struct caseptrs const *lright = right;
    if (lleft->id < lright->id)
        return -1 ;
    return lleft->id > lright->id;
}
/*-------------------------------------------------------------------------*/

void genxswitch(STATEMENT *stmt, SYMBOL *funcsp)
/*
 *      analyze and generate best switch statement.
 */
{
    int oldbreak, i;
    struct cases cs;
    IMODE *ap, *ap3;

#ifdef USE_LONGLONG
    ULLONG_TYPE a = 1;
#endif
    oldbreak = breaklab;
    breaklab = stmt->breaklabel + codeLabelOffset;
    memset(&cs,0,sizeof(cs));
#ifndef USE_LONGLONG
    cs.top = INT_MIN;
    cs.bottom = INT_MAX;
#else
    cs.top = (a << 63); /* LLONG_MIN*/
    cs.bottom = cs.top - 1; /* LLONG_MAX*/
#endif
    count_cases(stmt->cases,&cs) ;
    cs.top++;
    ap3 = gen_expr(funcsp, stmt->select, F_VOL | F_SWITCHVALUE, ISZ_UINT);
    ap = LookupLoadTemp(NULL, ap3);
    if (ap != ap3)
    {
        IMODE *barrier;
        if (stmt->select->isatomic)
        {
            barrier = doatomicFence(funcsp, NULL, stmt->select, NULL);
        }
        gen_icode(i_assn, ap, ap3, NULL);
        if (stmt->select->isatomic)
        {
            doatomicFence(funcsp, NULL, stmt->select, barrier);
        }
    }
    if (chosenAssembler->arch->preferopts & OPT_EXPANDSWITCH)
    {
        EXPRESSION *en = anonymousVar(sc_auto, &stdint);
        if (ap->size != -ISZ_UINT)
        {
            ap3 = tempreg(-ISZ_UINT, 0);
            gen_icode(i_assn, ap3, ap, NULL);
            ap = ap3;
        }
        ap3 = Alloc(sizeof(IMODE));
        ap3->mode = i_direct;
        ap3->offset = en;
        ap3->size = - ISZ_UINT;
        gen_icode(i_assn, ap3, ap, NULL);
        ap = ap3;
    }
    gen_icode2(i_coswitch, make_immed(ISZ_UINT,cs.count), ap, make_immed(ISZ_UINT,cs.top - cs.bottom), stmt->label + codeLabelOffset);
    gather_cases(stmt->cases,&cs);
    qsort(cs.ptrs, cs.count, sizeof(cs.ptrs[0]), gcs_compare);
    for (i = 0; i < cs.count; i++)
    {
        gen_icode2(i_swbranch,0,make_immed(ISZ_UINT,cs.ptrs[i].id),0,cs.ptrs[i].label);
    }
    breaklab = oldbreak;
}

void genselect(STATEMENT *stmt, SYMBOL *funcsp, BOOLEAN jmptrue)
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
static void gen_try(SYMBOL *funcsp, STATEMENT *stmt, int startLab, int endLab, int transferLab, STATEMENT *lower)
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
    gen_icode(i_goto, NULL, NULL, NULL);
    intermed_tail->dc.v.label = transferLab;
    tryStart = stmt->tryStart;
    tryEnd = stmt->tryEnd;
}
static void gen_catch(SYMBOL *funcsp, STATEMENT *stmt, int startLab, int transferLab, STATEMENT *lower)
{
    int oldtryStart = tryStart;
    int oldtryEnd = tryEnd;
    gen_label(startLab);
    currentBlock->alwayslive = TRUE;
    intermed_tail->alwayslive = TRUE;
    catchLevel++;
    genstmt(lower, funcsp);
    catchLevel--;
    /* not using gen_igoto because it will make a new block */
    gen_icode(i_goto, NULL, NULL, NULL);
    intermed_tail->dc.v.label = transferLab;
    tryStart = oldtryStart;
    tryEnd = oldtryEnd;
    stmt->tryStart = tryStart;
    stmt->tryEnd = tryEnd;
}
/*
 *      generate a return statement.
 */
void genreturn(STATEMENT *stmt, SYMBOL *funcsp, int flag, int noepilogue, IMODE *allocaAP)
{
    IMODE *ap = NULL,  *ap1, *ap3;
    EXPRESSION ep;
    int size;
    /* returns a value? */
    if (stmt != 0 && stmt->select != 0)
    {
        // the return type should NOT be an array at this point unless it is a managed one...
        if (basetype(funcsp->tp)->btp && (isstructured(basetype(funcsp->tp)->btp) || isarray(basetype(funcsp->tp)->btp) ||
                                          basetype(basetype(funcsp->tp)->btp)->type == bt_memberptr))
        {
            if (chosenAssembler->msil)
            {
                ap = gen_expr(funcsp, stmt->select, F_OBJECT | F_INRETURN, ISZ_ADDR);
                DumpIncDec(funcsp);
            }
            else
            {
                EXPRESSION *en = anonymousVar(sc_parameter, &stdpointer);
                SYMBOL *sp = en->v.sp;
                ap = gen_expr(funcsp, stmt->select, 0, ISZ_ADDR);
                DumpIncDec(funcsp);
                sp->offset = chosenAssembler->arch->retblocksize;
                sp->name = "__retblock";
                sp->allocate = FALSE;
                if ((funcsp->linkage == lk_pascal) &&
                    basetype(funcsp->tp)->syms->table[0] &&
                    ((SYMBOL *)basetype(funcsp->tp)->syms->table[0])->tp->type != bt_void)
                    sp->offset = funcsp->paramsize;
                deref(&stdpointer, &en);
                ap = gen_expr(funcsp, en, 0, ISZ_ADDR);
            }
            size = ISZ_ADDR;
        }
        else
        {
            size = natural_size(stmt->select);
            ap3 = gen_expr(funcsp, stmt->select, 0, size);
            DumpIncDec(funcsp);
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
            {
                IMODE *barrier;
                if (stmt->select->isatomic)
                {
                    barrier = doatomicFence(funcsp, NULL, stmt->select, NULL);
                }
                gen_icode(i_assn, ap, ap3, NULL);
                if (stmt->select->isatomic)
                {
                    doatomicFence(funcsp, NULL, stmt->select, barrier);
                }
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
                ap1->retval = TRUE;
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
        if (funcsp->linkage == lk_pascal || funcsp->linkage == lk_stdcall)
        {
           retsize = funcsp->paramsize ;
        }
        if (1 || !inlinesym_count || (!noepilogue && funcsp->retcount > 1))
        {
            gen_label(retlab);
        }
        else
        {
            QUAD *q = intermed_tail;
            while (q && q->dc.opcode != i_block && q->dc.opcode != i_goto)
            {
                q = q->back;
            }
            if (q && q->dc.opcode == i_goto)
            {
                q->back->fwd = q->fwd;
                if (q->fwd)
                    q->fwd->back = q->back;
                else
                    intermed_tail = q->back;
                addblock(-1);
            }
        }
        if (!noepilogue)
        {
        
            if (!inlinesym_count)
            {
                if (allocaAP && !chosenAssembler->msil)
                {
                    gen_icode(i_loadstack, 0, allocaAP, 0);
                }
    /*			if (funcsp->loadds && funcsp->farproc)
                    gen_icode(i_unloadcontext,0,0,0);
    */
                if (cparams.prm_xcept && funcsp->xc && funcsp->xc->xcRundownFunc)
                    gen_expr(funcsp, funcsp->xc->xcRundownFunc, F_NOVALUE, ISZ_UINT);
                SubProfilerData();
                gen_icode(i_epilogue,0,0,0);
                if (funcsp->linkage == lk_interrupt || funcsp->linkage == lk_fault) {
    /*				if (funcsp->loadds)
                        gen_icode(i_unloadcontext,0,0,0);
    */
                    gen_icode(i_popcontext, 0,0,0);
                    gen_icode(i_rett, 0, make_immed(ISZ_UINT,funcsp->linkage == lk_interrupt), 0);
                } else
                {
                    gen_icode(i_ret, 0, make_immed(ISZ_UINT,retsize), NULL);
                }
            }
        }
    }
    else
    {	
        /* not using gen_igoto because it will make a new block */
        gen_icode(i_goto, NULL, NULL, NULL);
        intermed_tail->dc.v.label = retlab;
    }
}

void gen_varstart(void *exp)
{
    IMODE *ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset = exp;
    ap->size = ISZ_ADDR;
    gen_icode(i_varstart, 0, ap, 0);
}
void gen_func(void *exp, int start)
{
    IMODE *ap = (IMODE *)Alloc(sizeof(IMODE));
    ap->mode = i_immed;
    ap->offset = exp;
    ap->size = ISZ_ADDR;
    gen_icode(i_func, 0, ap, 0)->dc.v.label = start;
}
void gen_dbgblock(int start)
{
    gen_icode(start ? i_dbgblock : i_dbgblockend, 0, 0, 0);
}
/*-------------------------------------------------------------------------*/

IMODE *genstmt(STATEMENT *stmt, SYMBOL *funcsp)
/*
 *      genstmt will generate a statement and follow the next pointer
 *      until the block is generated.
 */
{
    IMODE *rv = NULL;
    while (stmt != 0)
    {
        STATEMENT *last = stmt;
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
                gen_try(funcsp, stmt, stmt->label + codeLabelOffset, stmt->endlabel + codeLabelOffset, stmt->breaklabel + codeLabelOffset, stmt->lower);
                break;
            case st_catch:
            {
                STATEMENT *last;
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
            case st_expr:
            case st_declare:
                if (stmt->select)
                    rv = gen_expr(funcsp, stmt->select, F_NOVALUE, natural_size(stmt->select));
                break;
            case st_return:
                genreturn(stmt, funcsp, 0, 0, NULL);
                break;
            case st_line:
                gen_line(stmt->lineData);
                break;
            case st_select:
                genselect(stmt, funcsp, TRUE);
                break;
            case st_notselect:
                genselect(stmt, funcsp, FALSE);
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
    struct _data *next;
    IMODE *mem;
    IMODE *addr;
} DATA;
static    DATA *buckets[32];
static void StoreInBucket(IMODE *mem, IMODE *addr)
{
    DATA *lst;
    int bucket = (int)mem;
    bucket = (bucket >> 16) ^ (bucket >> 8) ^ (bucket >> 2);
    bucket %= 32;
    lst = buckets[bucket];
    while (lst)
    {
        if (lst->mem == mem)
        {
            if (lst->addr != addr)
                lst->mem->offset->v.sp->noCoalesceImmed = TRUE;
            return;
        }
        lst = lst->next;
    }
    lst = Alloc(sizeof(DATA));
    lst->mem = mem;
    lst->addr = addr;
    lst->next = buckets[bucket];
    buckets[bucket] = lst;
}
static IMODE *GetBucket(IMODE *mem)
{
    DATA *lst;
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
    return NULL;
}
/* coming into this routine we have two major requirements:
 * first, imodes that describe the same thing are the same object
 * second, identical expressions can be identified in that the temps
 * 		involved each time the expression is evaluated are the same
 */

void optimize(SYMBOL *funcsp)
{
            //printf("optimization start\n");
    if (chosenAssembler->gen->pre_gcse)
        chosenAssembler->gen->pre_gcse(intermed_head);
    #ifdef DUMP_GCSE_INFO
        if (icdFile && funcsp)
            fprintf(icdFile, 
                "\n*************************FUNCTION %s********************************\n", funcsp->name);
    #endif 

    /*
     * icode optimizations goes here.  Note that LCSE is done through
     * DAG construction during the actual construction of the blocks
     * so it is already done at this point.
     *
     * Order IS important!!!!!!!!! be careful!!!!!
     *
     * note that some of these optimizations make changes to the code,
     * with the exception of the actual global optimization pass we are
     * never really deleting dead code at the time we make changes
     * becase we aren't 100% certain what will really be dead
     * we do separate dead-code passes occasionally to clean it up
     */
    /* Global opts */

    flows_and_doms();
    gatherLocalInfo(funcsp);
    if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !functionHasAssembly)
    {
        Precolor();
        RearrangePrecolors();
    //printf("ssa\n");
        TranslateToSSA();

    //printf("const\n");
        if (optflags & OPT_CONSTANT)
        {
            ConstantFlow(); /* propagate constants */
            RemoveInfiniteThunks();
//			RemoveCriticalThunks();
            doms_only(FALSE);
        }
//		if (optflags & OPT_RESHAPE)
//			Reshape();		/* loop expression reshaping */
    //printf("stren\n");
        if (! (chosenAssembler->arch->denyopts & DO_NOGLOBAL))
        {
            if ((cparams.prm_optimize_for_speed) && (optflags & OPT_LSTRENGTH))
                ReduceLoopStrength(); /* loop index variable strength reduction */
        //printf("invar\n");
            if ((cparams.prm_optimize_for_speed) && (optflags & OPT_INVARIANT))
                MoveLoopInvariants();	/* move loop invariants out of loops */
        }
        if ((optflags & OPT_GLOBAL) && ! (chosenAssembler->arch->denyopts & DO_NOGLOBAL))
        {
        //printf("alias\n");
            AliasPass1();
        }
    //printf("ssa out\n");
        TranslateFromSSA(FALSE);
        removeDead(blockArray[0]);
//		RemoveCriticalThunks();
        if ((optflags & OPT_GLOBAL) && ! (chosenAssembler->arch->denyopts & DO_NOGLOBAL))
        {
    //printf("alias 2\n");
            SetGlobalTerms();
            AliasPass2();
            //printf("global\n");
            GlobalOptimization(); /* partial redundancy, code motion */
            AliasRundown();
        }
        nextTemp = tempBottom;
                 //printf("end opt\n");
           RemoveCriticalThunks();
        removeDead(blockArray[0]);
        RemoveInfiniteThunks();
    }
    else
    {
        RemoveCriticalThunks();
        RemoveInfiniteThunks();
    }
    /* backend modifies ICODE to improve code generation */
    if (chosenAssembler->gen->post_gcse)
    {
        chosenAssembler->gen->post_gcse(intermed_head);
    }
    /* register allocation - this first where we go into SSA form and backi s because 
     * at this point for global allocation we had to reuse original
     * register names, but the register allocation phase works better
     * when registers are disentangled and have smaller lifetimes
     * 
     * while we are back in SSA form we do some improvements to the code that will
     * help in register allocation and code generation.
     */
    definesInfo();
    liveVariables();
    doms_only(TRUE);
        //printf("to ssa\n");
    TranslateToSSA();
    CalculateInduction();
    /* lower for backend, e.g. do transformations that will improve the eventual
     * code gen, such as picking scaled indexed modes, moving constants, etc...
     */
        //printf("prealloc\n");
    Prealloc(1);
        //printf("from ssa\n");
    TranslateFromSSA(!(chosenAssembler->arch->denyopts & DO_NOREGALLOC));
    //printf("peep\n");
    peep_icode(FALSE);			/* peephole optimizations at the ICODE level */
    RemoveCriticalThunks();
    removeDead(blockArray[0]);			/* remove dead blocks */

            //printf("allocate\n");
    /* now do the actual allocation */
    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
    {
        AllocateRegisters(intermed_head); 
    /* backend peephole optimization can sometimes benefit by knowing what is live */
            //printf("live\n");

        CalculateBackendLives();
    }
    sFree();
    peep_icode(TRUE);	/* we do branche opts last to not interfere with other opts */
            //printf("optimzation done\n");

}
/*-------------------------------------------------------------------------*/
static void InsertParameterThunks(SYMBOL *funcsp, BLOCK *b)
{
    HASHREC *hr = basetype(funcsp->tp)->syms->table[0];
    QUAD *old , *oldit;
    BLOCK *ocb = currentBlock;
    old = b->head->fwd;
    while (old != b->tail && old->dc.opcode != i_label) old = old->fwd;
    old = old->fwd;
    oldit = intermed_tail;
    intermed_tail = old->back;
    intermed_tail->fwd = NULL;
    currentBlock = b;
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->tp->type == bt_void || sp->tp->type == bt_ellipse || isstructured(sp->tp))
        {
            hr = hr->next;
            continue;
        }
        if (!sp->imvalue || sp->imaddress)
        {
            hr = hr->next;
            continue;
        }
        if (funcsp->oldstyle && sp->tp->type == bt_float)
        {
            IMODE *right = (IMODE *)Alloc(sizeof(IMODE));
            *right = *sp->imvalue;
            right->size = ISZ_DOUBLE;
            if (!chosenAssembler->arch->hasFloatRegs)
            {
                IMODE *dp = tempreg(ISZ_DOUBLE, 0);
                IMODE *fp = tempreg(ISZ_FLOAT, 0);
                /* oldstyle float gets promoted from double */
                gen_icode(i_assn, dp, right, 0);
                gen_icode(i_assn, fp, dp, 0);
                gen_icode(i_assn, sp->imvalue, fp, 0);
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
void genfunc(SYMBOL *funcsp, BOOLEAN doOptimize)
/*
 *      generate a function body and dump the icode
 */
{
    IMODE *oldReturnImode = returnImode;
    IMODE *allocaAP = NULL;
    SYMBOL *oldCurrentFunc;
    EXPRESSION *funcexp = varNode(en_global, funcsp);
    SYMBOL *tmpl = funcsp;
    HASHREC *hr;
    if (total_errors)
        return;
    returnImode = NULL;
    while (tmpl)
        if (tmpl->templateLevel)
            break;
        else
            tmpl = tmpl->parentClass;
            
    /* for inlines */
    codeLabelOffset = nextLabel - INT_MIN ;
    nextLabel += funcsp->labelCount;

    temporarySymbols = NULL;
    contlab = breaklab =  - 1;
    structret_imode = 0 ;
    tempCount = 0;
    blockCount = 0;
    blockMax = 0;
    exitBlock = 0;
    consIndex = 0;
    retcount = 0;
    objectArray_exp = NULL;
    oldCurrentFunc = theCurrentFunc;
    theCurrentFunc = funcsp;
    iexpr_func_init();

    if (funcsp->xc && funcsp->xc->xctab)
    {
        EXPRESSION *exp;
        xcexp = varNode(en_auto, funcsp->xc->xctab);
        xcexp = exprNode(en_add, xcexp, intNode(en_c_i, (LLONG_TYPE)&(((struct _xctab *)0)->funcIndex)));
        deref(&stdpointer, &xcexp);
        exp = intNode(en_c_i, 0);
        xcexp = exprNode(en_assign, xcexp, exp);
    }
    else
    {
        xcexp = NULL; 
   }
    cseg();
    gen_line(funcsp->linedata);
    gen_func(funcexp, 1);
    /* in C99 inlines can clash if declared 'extern' in multiple modules */
    /* in C++ we introduce virtual functions that get coalesced at link time */
    if (funcsp->linkage == lk_virtual || tmpl)
    {
        funcsp->linkage = lk_virtual;
        gen_virtual(funcsp, FALSE);
    }
    else
    {
        if (funcsp->storage_class == sc_global || ((funcsp->storage_class == sc_member || funcsp->storage_class == sc_virtual) && funcsp->inlineFunc.stmt))
                globaldef(funcsp);
        else
            localdef(funcsp);
        gen_strlab(funcsp); /* name of function */
    }
    addblock( - 1);
       if (funcsp->linkage == lk_interrupt || funcsp->linkage == lk_fault) {
        gen_icode(i_pushcontext, 0,0,0);
/*		if (funcsp->loadds) */
/*	        gen_icode(i_loadcontext, 0,0,0); */
    }
    gen_icode(i_prologue,0,0,0);
    if (cparams.prm_debug)
    {
        if (basetype(funcsp->tp)->syms->table[0] && ((SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p)->thisPtr)
        {
            EXPRESSION *exp = varNode(en_auto, ((SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p));
            exp->v.sp->tp->used = TRUE;
            gen_varstart(exp);
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
    AllocateLocalContext(NULL, funcsp, nextLabel++);
    if (funcsp->allocaUsed && !chosenAssembler->msil)
    {
            EXPRESSION *allocaExp = anonymousVar(sc_auto, &stdpointer);
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
    if (chosenAssembler->arch->denyopts & DO_NOREGALLOC)
        FreeLocalContext(NULL, funcsp, nextLabel++);
    if (doOptimize)
        optimize(funcsp);
    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
        FreeLocalContext(NULL, funcsp, nextLabel++);
        
    if (!(chosenAssembler->arch->denyopts & DO_NOREGALLOC))
        AllocateStackSpace(funcsp);
    FillInPrologue(intermed_head, funcsp);
    /* Code gen from icode */
    rewrite_icode(); /* Translate to machine code & dump */
    if (chosenAssembler->gen->post_function_gen)
        chosenAssembler->gen->post_function_gen(funcsp, intermed_head);
    if (funcsp->linkage == lk_virtual || tmpl)
        gen_endvirtual(funcsp);
    AllocateLocalContext(NULL, funcsp, nextLabel++);
    XTDumpTab(funcsp);
    FreeLocalContext(NULL, funcsp, nextLabel++);
    intermed_head = NULL;
    dag_rundown();
    oFree();
    theCurrentFunc = oldCurrentFunc;
    returnImode = oldReturnImode;
    if (blockCount > maxBlocks)
        maxBlocks = blockCount;
    if (tempCount > maxTemps)
        maxTemps = tempCount;
        
    // this is explicitly to clean up the this pointer
    hr = basetype(funcsp->tp)->syms->table[0];    
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym->storage_class == sc_parameter)
        {
            sym->imind = NULL;
            sym->imvalue = NULL;
            sym->imaddress = NULL;
        }
        hr = hr->next;
    }
}
void genASM(STATEMENT *st)
{
    cseg();
    contlab = breaklab =  - 1;
    structret_imode = 0 ;
    tempCount = 0;
    blockCount = 0;
    blockMax = 0;
    exitBlock = 0;
    genstmt(st, NULL);
    rewrite_icode(); /* Translate to machine code & dump */
}