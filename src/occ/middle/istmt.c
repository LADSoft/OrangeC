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
 * istmt.c
 *
 * change the statement list to icode
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "compiler.h"

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
extern int nextLabel;
extern int total_errors;
extern int errorline;
extern char *errorfile;
extern TEMP_INFO **tempInfo;
extern BOOL functionHasAssembly;
extern TYPE stddouble;
#ifdef DUMP_GCSE_INFO
    extern FILE *icdFile;
#endif 
extern BLOCK **blockArray;
extern int maxBlocks, maxTemps;
extern int retlab, startlab;

int tempCount;
int optflags = ~0;
static LIST *mpthunklist;

static int breaklab;
static int contlab;

void genstmt(STATEMENT *stmt, SYMBOL *funcsp);

void genstmtini(void)
{
    mpthunklist = NULL;
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
    gen_icode(i_genword, 0, make_immed(ISZ_NONE,(int)stmt->select), 0);
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
        sp->genreffed = TRUE;
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
        cs->ptrs[pos].label = cd->label;
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
    breaklab = stmt->breaklabel;
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
    ap3 = gen_expr(funcsp, stmt->select, F_VOL | F_NOVALUE, ISZ_UINT);
    ap = LookupLoadTemp(NULL, ap3);
    if (ap != ap3)
    {
        IMODE *barrier;
        if (stmt->select->isatomic)
        {
            barrier = doatomicFence(funcsp, stmt->select, NULL);
        }
        gen_icode(i_assn, ap, ap3, NULL);
        if (stmt->select->isatomic)
        {
            doatomicFence(funcsp, stmt->select, barrier);
        }
    }
    gen_icode2(i_coswitch, make_immed(ISZ_NONE,cs.count), ap, make_immed(ISZ_NONE,cs.top - cs.bottom), stmt->label);
    gather_cases(stmt->cases,&cs);
    qsort(cs.ptrs, cs.count, sizeof(cs.ptrs[0]), gcs_compare);
    for (i = 0; i < cs.count; i++)
    {
        gen_icode2(i_swbranch,0,make_immed(ISZ_NONE,cs.ptrs[i].id),0,cs.ptrs[i].label);
    }
    breaklab = oldbreak;
}

void genselect(STATEMENT *stmt, SYMBOL *funcsp, BOOL jmptrue)
{
    if (stmt->altlabel)
    {
//		gen_label(stmt->altlabel);
//		intermed_tail->dc.opcode = i_skipcompare;
    }
    if (jmptrue)
        truejp(stmt->select, funcsp, stmt->label);
    else
        falsejp(stmt->select, funcsp, stmt->label);
    if (stmt->altlabel)
    {
//		gen_label(stmt->altlabel);
//		intermed_tail->dc.opcode = i_skipcompare;
    }
}
/*-------------------------------------------------------------------------*/

void genreturn(STATEMENT *stmt, SYMBOL *funcsp, int flag, int noepilogue, IMODE *allocaAP)
/*
 *      generate a return statement.
 */
{
    IMODE *ap,  *ap1, *ap3;
    EXPRESSION ep;
    /* returns a value? */
    if (stmt != 0 && stmt->select != 0)
    {
        if (basetype(funcsp->tp)->btp && (isstructured(basetype(funcsp->tp)->btp) ||
                                          basetype(basetype(funcsp->tp)->btp)->type == bt_memberptr))
        {
            SYMBOL *sp = anonymousVar(sc_parameter, &stdpointer);
            EXPRESSION *en = varNode(en_auto, sp);
            IMODE *ap3 = gen_expr(funcsp, stmt->select, 0, ISZ_ADDR), *ap2, *ap1;
            DumpIncDec(funcsp);
            if (!ap3->retval)
            {
                ap1 = LookupLoadTemp(NULL, ap3);
                if (ap1 != ap3)
                {
                    IMODE *barrier;
                    if (stmt->select->isatomic)
                    {
                        barrier = doatomicFence(funcsp, stmt->select, NULL);
                    }
                    gen_icode(i_assn, ap1, ap3, NULL);
                    if (stmt->select->isatomic)
                    {
                        doatomicFence(funcsp, stmt->select, barrier);
                    }
                }
            }
            else
            {
                ap1 = ap3;
            }
            if ((funcsp->linkage == lk_pascal) &&
                basetype(funcsp->tp)->syms->table[0] && 
                ((SYMBOL *)basetype(funcsp->tp)->syms->table[0])->tp->type != bt_void)
            {
                sp->offset = funcsp->paramsize;
            }
            else
            {
                sp->offset = chosenAssembler->arch->retblocksize+(funcsp->farproc
                    *getSize(bt_pointer));
                if (funcsp->storage_class == sc_member || funcsp->storage_class == sc_virtual)
                    sp->offset += getSize(bt_pointer);
            }
                
            en = exprNode(en_l_p, en, NULL);
            ap3 = gen_expr(funcsp, en, 0, ISZ_ADDR);
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
            {
                IMODE *barrier;
                if (en->isatomic)
                {
                    barrier = doatomicFence(funcsp, en, NULL);
                }
                gen_icode(i_assn, ap, ap3, NULL);
                if (en->isatomic)
                {
                    doatomicFence(funcsp, en, barrier);
                }
            }
            gen_icode(i_assnblock, make_immed(ISZ_NONE, basetype(funcsp->tp)->btp->size),
                      ap, ap1);
            ap1 = tempreg(ISZ_ADDR, 0);
            ap1->retval = TRUE;
            gen_icode(i_assn, ap1, ap, NULL);
        }
        else if (basetype(funcsp->tp)->btp && basetype(funcsp->tp)->btp->type ==
            bt_memberptr)
        {
            ap3 = gen_expr(funcsp, stmt->select, F_VOL, ISZ_ADDR);
            DumpIncDec(funcsp);
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
            {
                IMODE *barrier;
                if (stmt->select->isatomic)
                {
                    barrier = doatomicFence(funcsp, stmt->select, NULL);
                }
                gen_icode(i_assn, ap, ap3, NULL);
                if (stmt->select->isatomic)
                {
                    doatomicFence(funcsp, stmt->select, barrier);
                }
            }
            ap1 = tempreg(ISZ_ADDR, 0);
            ap1->retval = TRUE;
            gen_icode(i_assn, ap1, ap, 0);
        }
        else
        {
            int size = natural_size(stmt->select);
            ap3 = gen_expr(funcsp, stmt->select, 0, size);
            DumpIncDec(funcsp);
            ap = LookupLoadTemp(NULL, ap3);
            if (ap != ap3)
            {
                IMODE *barrier;
                if (stmt->select->isatomic)
                {
                    barrier = doatomicFence(funcsp, stmt->select, NULL);
                }
                gen_icode(i_assn, ap, ap3, NULL);
                if (stmt->select->isatomic)
                {
                    doatomicFence(funcsp, stmt->select, barrier);
                }
            }
            if (abs(size) < ISZ_UINT)
                size = -ISZ_UINT;
            ap1 = tempreg(size, 0);
            ap1->retval = TRUE;
            gen_icode(i_assn, ap1, ap, 0);
        }
    }
    else
    {
        DumpIncDec(funcsp);
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
        gen_label(retlab);
        if (!noepilogue)
        {
            if (allocaAP)
            {
                gen_icode(i_loadstack, 0, allocaAP, 0);
            }
/*			if (funcsp->loadds && funcsp->farproc)
                gen_icode(i_unloadcontext,0,0,0);
*/
        
            gen_icode(i_epilogue,0,0,0);
            if (funcsp->linkage == lk_interrupt || funcsp->linkage == lk_fault) {
/*				if (funcsp->loadds)
                    gen_icode(i_unloadcontext,0,0,0);
*/
                gen_icode(i_popcontext, 0,0,0);
                gen_icode(i_rett, 0, make_immed(ISZ_NONE,funcsp->linkage == lk_interrupt), 0);
            } else
            {
                gen_icode(i_ret, 0, make_immed(ISZ_NONE,retsize), 0);
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

/*-------------------------------------------------------------------------*/
void gen_tryblock(void *val)
{
    /* xceptoffs will be put in the label field later */
    gen_icode(i_tryblock, 0, make_immed(ISZ_NONE, (int)val), 0);
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

void genstmt(STATEMENT *stmt, SYMBOL *funcsp)
/*
 *      genstmt will generate a statement and follow the next pointer
 *      until the block is generated.
 */
{
    while (stmt != 0)
    {
        switch (stmt->type)
        {
            case st_varstart:
                gen_varstart(stmt->select);
                break;
            case st_dbgblock:
                gen_dbgblock(stmt->label);
                break;
            case st_tryblock:
/*				gen_tryblock(stmt->label); */
                break;
            case st_block:
                genstmt(stmt->lower, funcsp);
                genstmt(stmt->blockTail, funcsp);
                break;
            case st_label:
                gen_label((int)stmt->label);
                break;
            case st_goto:
                gen_igoto(i_goto, (int)stmt->label);
                break;
            case st_asmgoto:
                gen_igoto(i_asmgoto, (int)stmt->label);
                break;
            case st_asmcond:
                gen_igoto(i_asmcond, (int)stmt->label);
                break;
            case st_throw:
/*                gen_throw((TYPE *)stmt->lst, stmt->select);*/
                break;
/*			case st_functailexpr: 
                gen_icode(i_functailstart, 0, 0, 0); 
                gen_expr(funcsp, stmt->select, F_NOVALUE, natural_size(stmt->select)); *
                gen_icode(i_functailend, 0, 0, 0);
               break;
*/
            case st_expr:
            case st_declare:
                gen_expr(funcsp, stmt->select, F_NOVALUE, natural_size(stmt->select));
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
        stmt = stmt->next;
    }
}

/*-------------------------------------------------------------------------*/
#ifdef XXXXX
SYMBOL *gen_mp_virtual_thunk(SYMBOL *vsp)
{

        QUAD *oi = intermed_head, *ot = intermed_tail;
        LIST *v = mpthunklist;
        SYMBOL *sp;
        IMODE *ap1,  *ap2;
        char buf[256];
        while (v)
        {
            sp = (SYMBOL*)v->data;
            if (sp->value.i == vsp->value.classdata.vtabindex)
                if (isstructured(vsp->tp->btp) == isstructured(sp->tp->btp))
                    return sp;
            v = v->link;
        }
        intermed_head = intermed_tail = NULL;
        blockMax = 0;
        blockCount = tempCount = 0;
        exitBlock = 0;
        IncGlobalFlag();
        sp = Alloc(sizeof(SYMBOL));
        sp->storage_class = sc_static;
        sp->tp = vsp->tp;
        sp->value.i = vsp->value.classdata.vtabindex;
        //sprintf(buf, "@$mpt$%d$%d", sp->value.i, isstructured(sp->tp->btp));
        sp->name = sp->decoratedName = litlate(buf);
        sp->errname = sp->decoratedName;
        sp->staticlabel = FALSE;
        sp->gennedvirtfunc = TRUE;
        v = (LIST *)Alloc(sizeof(LIST));
        v->data = sp;
        v->link = mpthunklist;
        mpthunklist = v;
        DecGlobalFlag();

        gen_virtual(sp, FALSE);
        addblock(i_goto);
        gen_icode(i_substack, NULL, ap2 = tempreg(ISZ_ADDR, 0), NULL);
        gen_icode(i_add, ap2, ap2 , make_immed(ISZ_NONE, isstructured(sp->tp->btp) ? ISZ_ADDR * 2 : ISZ_ADDR));
        ap1 = indnode(ap2, ISZ_ADDR);
        gen_icode(i_assn, ap2 = tempreg(ISZ_ADDR, 0), ap1, 0);
        if (sp->value.classdata.baseofs)
            gen_icode(i_add, ap2, ap2, make_immed(ISZ_NONE, sp->value.classdata.baseofs));
        ap1 = indnode(ap2, ISZ_ADDR);
        gen_icode(i_assn, ap2 = tempreg(ISZ_ADDR, 0), ap1, 0);
        gen_icode(i_add, ap2, ap2, make_immed(ISZ_NONE, sp->value.i));
        ap1 = indnode(ap2, ISZ_ADDR);
        gen_icode(i_directbranch, 0, ap1, 0);
        addblock(i_ret);
        optimize();
        rewrite_icode(); /* Translate to machine code & dump */
        if (chosenAssembler->gen->post_function_gen)
            chosenAssembler->gen->post_function_gen(intermed_head);
        gen_endvirtual(sp);
        intermed_head = oi;
        intermed_tail = ot;
        return sp;
}

/*-------------------------------------------------------------------------*/

SYMBOL *gen_vsn_virtual_thunk(SYMBOL *func, int ofs)
{
        QUAD *oi = intermed_head, *ot = intermed_tail;		
        SYMBOL *sp;
        IMODE *ap1,  *ap2;
        char buf[256];
        //sprintf(buf, "@$vsn%s$%d", func->decoratedName, ofs);
        sp = search(buf, gsyms);
        if (sp)
            return sp;
        intermed_head = intermed_tail = NULL;
        blockMax = 0;
        blockCount = tempCount = 0;
        exitBlock = 0;
        IncGlobalFlag();
        sp = Alloc(sizeof(SYMBOL));
        sp->storage_class = sc_static;
        sp->value.i = ofs;
        sp->name = sp->decoratedName = litlate(buf);
        sp->errname = sp->decoratedName;
        sp->staticlabel = FALSE;
        sp->tp = func->tp;
        sp->gennedvirtfunc = TRUE;
        insert(sp, gsyms);
        DecGlobalFlag();
        gen_virtual(sp, FALSE);
        addblock(i_goto);
        gen_icode(i_substack, NULL, ap2 = tempreg(ISZ_ADDR, 0), NULL );
        gen_icode(i_add, ap1 = tempreg(ISZ_ADDR, 0), ap2, make_immed(ISZ_NONE, getSize(bt_pointer)));
        ap1 = indnode(ap1, ISZ_ADDR);
        gen_icode(i_add, ap1, ap1 , make_immed(ISZ_NONE, - ofs));
        ap1 = make_imaddress(varNode(en_napccon, func), ISZ_ADDR);
        gen_icode(i_directbranch, 0, ap1, 0);
        addblock(i_ret);
        optimize();
        rewrite_icode(); /* Translate to machine code & dump */
        if (chosenAssembler->gen->post_function_gen)
            chosenAssembler->gen->post_function_gen(intermed_head);
        gen_endvirtual(sp);
        intermed_head = oi;
        intermed_tail = ot;
        return sp;
}
#endif
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
    if (cparams.prm_optimize && !functionHasAssembly)
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
        if (optflags & OPT_LSTRENGTH)
            ReduceLoopStrength(); /* loop index variable strength reduction */
    //printf("invar\n");
        if (optflags & OPT_INVARIANT)
            MoveLoopInvariants();	/* move loop invariants out of loops */

        if (optflags & OPT_GLOBAL)
        {
        //printf("alias\n");
            AliasPass1();
        }
    //printf("ssa out\n");
        TranslateFromSSA(FALSE);
        removeDead(blockArray[0]);
//		RemoveCriticalThunks();
        if (optflags & OPT_GLOBAL)
        {
    //printf("alias 2\n");
            AliasPass2();
            //printf("global\n");
            GlobalOptimization(); /* partial redundancy, code motion */
            AliasRundown();
        }
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
    TranslateFromSSA(TRUE);
    //printf("peep\n");
      peep_icode(FALSE);			/* peephole optimizations at the ICODE level */
    RemoveCriticalThunks();
    removeDead(blockArray[0]);			/* remove dead blocks */

            //printf("allocate\n");
    /* now do the actual allocation */
    AllocateRegisters(intermed_head); 
    /* backend peephole optimization can sometimes benefit by knowing what is live */
            //printf("live\n");

    CalculateBackendLives();
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
void genfunc(SYMBOL *funcsp)
/*
 *      generate a function body and dump the icode
 */
{
    IMODE *allocaAP = NULL;
    SYMBOL *oldCurrentFunc;
    EXPRESSION *funcexp = varNode(en_global, funcsp);
    if (total_errors)
        return;
//	//printf("%s\n", funcsp->name);
    contlab = breaklab =  - 1;
    structret_imode = 0 ;
    tempCount = 0;
    blockCount = 0;
    blockMax = 0;
    exitBlock = 0;
    oldCurrentFunc = theCurrentFunc;
    theCurrentFunc = funcsp;
    iexpr_func_init();
    /*      firstlabel = nextLabel;*/
    cseg();
    gen_line(funcsp->linedata);
    gen_func(funcexp, 1);
    /* in C99 inlines can clash if declared 'extern' in multiple modules */
    /* in C++ we introduce virtual functions that get coalesced at link time */
    if (cparams.prm_cplusplus && funcsp->linkage == lk_inline)
        gen_virtual(funcsp, FALSE);
    else
    {
        if (funcsp->storage_class == sc_global)
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
    gen_label(startlab);
/*    if (funcsp->loadds && funcsp->farproc) */
/*	        gen_icode(i_loadcontext, 0,0,0); */
    AllocateLocalContext(NULL, funcsp);
    if (funcsp->allocaUsed)
    {
            EXPRESSION *allocaExp = varNode(en_auto, anonymousVar(sc_auto, &stdpointer));
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
    optimize(funcsp);
    FreeLocalContext(NULL, funcsp);
        
    AllocateStackSpace(funcsp);
    FillInPrologue(intermed_head, funcsp);
    /* Code gen from icode */
    rewrite_icode(); /* Translate to machine code & dump */
    if (chosenAssembler->gen->post_function_gen)
        chosenAssembler->gen->post_function_gen(funcsp, intermed_head);
    if (cparams.prm_cplusplus && funcsp->linkage == lk_inline)
        gen_endvirtual(funcsp);
    intermed_head = NULL;
    dag_rundown();
    oFree();
    theCurrentFunc = oldCurrentFunc;
    if (blockCount > maxBlocks)
        maxBlocks = blockCount;
    if (tempCount > maxTemps)
        maxTemps = tempCount;
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