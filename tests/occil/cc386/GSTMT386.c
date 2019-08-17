/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "gen386.h"
#include "diag.h"
#include <stdlib.h>

extern int noStackFrame;
extern int prm_lscrtdll;
extern ENODE *allocaSP;
extern int used_alloca;
extern int virtualfuncs;
extern int prm_asmfile;
extern SYM *templateSP;
extern SNODE *funcstmt,  *funcendstmt;
extern ENODE *thisenode;
extern SYM *thissp;
extern TABLE *gsyms;
extern int stdaddrsize;
extern int global_flag;
extern int fsave_mask, save_mask;
extern TYP stdfunc;
extern struct amode push[], pop[];
extern OCODE *peep_tail,  *peep_head,  *peep_insert;
extern SYM *currentfunc;
extern int regs[3];
extern int prm_cplusplus, prm_profiler, prm_farkeyword;
extern long firstlabel, nextlabel;
extern long lc_maxauto;
extern ENODE *block_rundown;
extern SYM *declclass;
extern int funcfloat;
extern int prm_smartframes;
extern OCODE *frame_ins;
extern int vtabhead;
extern int conscount;
extern int prm_xcept;
extern TRYBLOCK *try_block_list;
extern int xceptoffs;
extern int prm_cplusplus, prm_xcept;
extern int genning_inline;
extern SNODE *funcbrstmt;
extern int prm_debug;

int retlab, startlab;
int stackframeoff;

LIST *mpthunklist;

struct cases {
    LLONG_TYPE bottom;
    LLONG_TYPE top;
    int count;
    int deflab;
    int tablepos;
    int diddef : 1;
    struct caseptrs {
        int label;
        int binlabel;
        LLONG_TYPE id;
    } *ptrs;
} ;
static int returndreg;
static int breaklab;
static int contlab;
static long plabel;
static int retsize;

AMODE *makedreg(int r)
/*
 *      make an address reference to a data register.
 */
{
    AMODE *ap;
    ap = xalloc(sizeof(AMODE));
    ap->mode = am_dreg;
    ap->preg = r;
    ap->length = BESZ_DWORD;
    return ap;
} 

AMODE *makefreg(int r)
/*
 *      make an address reference to a data register.
 */
{
    AMODE *ap;
    ap = xalloc(sizeof(AMODE));
    ap->mode = am_freg;
    ap->preg = r;
    ap->length = 10;
    return ap;
}

//-------------------------------------------------------------------------

AMODE *make_direct(int i)
/*
 *      make a direct reference to an immediate value.
 */
{
    return make_offset(makeintnode(en_icon, i));
}

//-------------------------------------------------------------------------

AMODE *make_strlab(char *s)
/*
 *      generate a direct reference to a string label.
 */
{
    AMODE *ap;
    ap = xalloc(sizeof(AMODE));
    ap->mode = am_direct;
    ap->offset = makenode(en_nacon, s, 0);
    return ap;
}

//-------------------------------------------------------------------------

void genwhile(SNODE *stmt)
/*
 *      generate code to evaluate a while statement.
 */
{
    int lab1, lab2, lab3;
	int areg, sreg;
    InitRegs(); /* initialize temp registers */
    lab1 = contlab; /* save old continue label */
    contlab = nextlabel++; /* new continue label */
    if (stmt->s1 != 0)
     /* has block */
    {
        lab2 = breaklab; /* save old break label */
        breaklab = nextlabel++;
        gen_codes(op_jmp, 0, make_label(contlab), 0);
        lab3 = nextlabel++;
        gen_label(lab3);
        genstmt(stmt->s1);
        gen_label(contlab);
        if (stmt->lst)
            gen_line(stmt->lst);
        InitRegs();
		ChooseRegs(&areg, &sreg);
        truejp(stmt->exp, areg, sreg, lab3);
        gen_label(breaklab);
        breaklab = lab2; /* restore old break label */
    }
    else
     /* no loop code */
    {
        if (stmt->lst)
            gen_line(stmt->lst);
        gen_label(contlab);
        InitRegs();
		ChooseRegs(&areg, &sreg);
        truejp(stmt->exp, areg, sreg, contlab);
    }
    contlab = lab1; /* restore old continue label */
}

//-------------------------------------------------------------------------

void gen_for(SNODE *stmt)
/*
 *      generate code to evaluate a for loop
 */
{
    int old_break, old_cont, exit_label, loop_label, start_label;
	int areg, sreg;
    old_break = breaklab;
    old_cont = contlab;
    loop_label = nextlabel++;
    contlab = nextlabel++;
    start_label = nextlabel++;
    exit_label = nextlabel++;
    InitRegs();
    if (stmt->label != 0)
    {
        gen_void_external(stmt->label);
    }

    gen_codes(op_jmp, 0, make_label(start_label), 0);
    gen_label(loop_label);
    if (stmt->s1 != 0)
    {
        breaklab = exit_label;
        genstmt(stmt->s1);
    }
    if (stmt->lst)
        gen_line(stmt->lst);
    gen_label(contlab);
    InitRegs();
    if (stmt->s2 != 0)
    {
        gen_void_external(stmt->s2);
    }
    gen_label(start_label);
    InitRegs();
	ChooseRegs(&areg, &sreg);
    if (stmt->exp != 0)
        truejp(stmt->exp, areg, sreg, loop_label);
    else
        gen_codes(op_jmp, 0, make_label(loop_label), 0);
    gen_label(exit_label);
    breaklab = old_break;
    contlab = old_cont;
}

//-------------------------------------------------------------------------

void genif(SNODE *stmt)
/*
 *      generate code to evaluate an if statement.
 */
{
    int lab1, lab2;
	int areg, sreg;
    lab1 = nextlabel++; /* else label */
    lab2 = nextlabel++; /* exit label */
    InitRegs(); /* clear temps */
	ChooseRegs(&areg, &sreg);
    falsejp(stmt->exp, areg, sreg, lab1);
    genstmt(stmt->s1);
    if (stmt->s2 != 0)
     /* else part exists */
    {
        gen_codes(op_jmp, 0, make_label(lab2), 0);
        gen_label(lab1);
        genstmt(stmt->s2);
        gen_label(lab2);
    }
    else
     /* no else code */
        gen_label(lab1);
}

//-------------------------------------------------------------------------

void gendo(SNODE *stmt)
/*
 *      generate code for a do - while loop.
 */
{
    int oldcont, oldbreak, looplab;
	int areg, sreg;
    oldcont = contlab;
    oldbreak = breaklab;
    looplab = nextlabel++;
    contlab = nextlabel++;
    breaklab = nextlabel++;
    gen_label(looplab);
    genstmt(stmt->s1); /* generate body */
    gen_label(contlab);
    InitRegs();
	ChooseRegs(&areg, &sreg);
    truejp(stmt->exp, areg, sreg, looplab);
    gen_label(breaklab);
    breaklab = oldbreak;
    contlab = oldcont;
}

//-------------------------------------------------------------------------

AMODE *set_symbol(char *name, int flag)
{
    SYM *sp;
    AMODE *ap;
    sp = gsearch(name);
    if (sp == 0)
    {
        ++global_flag;
        sp = makesym(flag ? sc_externalfunc : sc_external);
        sp->tp = &stdfunc;
        sp->name = name;
        sp->extflag = 1;
        insert(sp, gsyms);
        --global_flag;
    }
    ap = xalloc(sizeof(AMODE));
    ap->mode = am_immed;
    ap->length = BESZ_DWORD;
    ap->offset = makenode(en_nacon, sp, 0);
    return ap;
}

//-------------------------------------------------------------------------

void call_library(char *lib_name)
/*
 *      generate a call to a library routine.
 */
{
    AMODE *ap;
    ap = set_symbol(lib_name, 1);
	if (prm_lscrtdll)
	{
		ap->mode = am_direct;
	}
    gen_codes(op_call, 0, ap, 0);
}
//-------------------------------------------------------------------------

void count_cases(SNODE *stmt, struct cases *cs)
{
    while (stmt)
    {
        switch(stmt->stype) {        
             case st_tryblock:
                break;
            case st_throw:
                break;
            case st_return:
            case st_expr:
                break;
            case st_while:
            case st_do:
                count_cases(stmt->s1,cs);
                break;
            case st_for:
                count_cases(stmt->s1,cs);
                break;
            case st_if:
                count_cases(stmt->s1,cs);
                count_cases(stmt->s2,cs);
                break;
            case st_switch:
                break;
            case st_block:
                count_cases(stmt->exp,cs);
                break;
            case st_asm:
                break;
            case st_case:
               if (!stmt->s2)
                {
                    cs->count++;
                    if (stmt->switchid < cs->bottom)
                        cs->bottom = stmt->switchid;
                    if (stmt->switchid > cs->top)
                        cs->top = stmt->switchid;
                }
                break;
        }
        stmt = stmt->next;
    }
}
//-------------------------------------------------------------------------
void gather_cases(SNODE *stmt, struct cases *cs, int compact)
{
    if (!cs->ptrs) {
        global_flag++;
        if (compact) {
            unsigned i;
            cs->ptrs = xalloc((cs->top - cs->bottom) * sizeof(struct caseptrs));
            for (i = 0; i < cs->top-cs->bottom; i++)
			{
                cs->ptrs[i].label = cs->deflab;
			}
        } else
            cs->ptrs = xalloc((cs->count) * sizeof(struct caseptrs));
        global_flag--;
    }
    while (stmt)
    {
        switch(stmt->stype) {        
             case st_tryblock:
                break;
            case st_throw:
                break;
            case st_return:
            case st_expr:
                break;
            case st_while:
            case st_do:
                gather_cases(stmt->s1,cs,compact);
                break;
            case st_for:
                gather_cases(stmt->s1,cs,compact);
                break;
            case st_if:
                gather_cases(stmt->s1,cs,compact);
                gather_cases(stmt->s2,cs,compact);
                break;
            case st_switch:
                break;
            case st_block:
                gather_cases(stmt->exp,cs,compact);
                break;
            case st_asm:
                break;
            case st_case:
                if (stmt->s2)
                 /* default case */
                {
                    cs->diddef = TRUE;
                    stmt->label = (SNODE*)cs->deflab;
                }
                else
                {
                    if (compact) {
                        cs->ptrs[stmt->switchid - cs->bottom].label = nextlabel;
                        stmt->label = (SNODE*)nextlabel++;
                    } else {
                        cs->ptrs[cs->tablepos].label = nextlabel;
                        cs->ptrs[cs->tablepos].binlabel =  - 1;
                        cs->ptrs[cs->tablepos++].id = stmt->switchid;
                        stmt->label = (SNODE*)nextlabel++;
                    }
                }
                break;
        }
        stmt = stmt->next;
    }
}
//-------------------------------------------------------------------------


int analyzeswitch(SNODE *stmt, struct cases *cs)
{
    int size = natural_size(stmt->exp);
    count_cases(stmt->s1,cs) ;
    cs->top++;
    if (cs->count == 0)
        return (0);
    if (cs->count < 5)
        return 3;
    if (cs->count *10 / (cs->top - cs->bottom) >= 8)
        return (1);
    // do a simple switch instead of a binary if it is a long long
    if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
        return 3;
    return (2);
}

//-------------------------------------------------------------------------

void bingen(int lower, int avg, int higher, AMODE *ap1, struct cases *cs, int size)
{
    AMODE *ap2 = make_immed(cs->ptrs[avg].id);
    AMODE *ap3 = make_label(cs->ptrs[avg].label);
    if (cs->ptrs[avg].binlabel !=  - 1)
        gen_label(cs->ptrs[avg].binlabel);
    gen_coden(op_cmp, size, ap1, ap2);
    gen_code(op_je, ap3, 0);
    if (avg == lower)
    {
        if (cs->deflab < 0)
            cs->deflab = nextlabel++;
        ap3 = make_label(cs->deflab);
        gen_code(op_jmp, ap3, 0);
    }
    else
    {
        int avg1 = (lower + avg) / 2;
        int avg2 = (higher + avg + 1) / 2;
        if (avg + 1 < higher)
            ap3 = make_label(cs->ptrs[avg2].binlabel = nextlabel++);
        else
            ap3 = make_label(cs->deflab);
        if (size < 0)
            gen_code(op_jg, ap3, 0);
        else
            gen_code(op_ja, ap3, 0);
        bingen(lower, avg1, avg, ap1, cs, size);
        if (avg + 1 < higher)
            bingen(avg + 1, avg2, higher, ap1, cs, size);
    }
}

//-------------------------------------------------------------------------
int sortcmpsigned(const void *one, const void *two)
{
    if (((struct caseptrs *)one)->id < ((struct caseptrs *)two)->id)
        return -1;
    if (((struct caseptrs *)one)->id > ((struct caseptrs *)two)->id)
        return 1;
    return 0;
}
int sortcmpunsigned(const void *one, const void *two)
{
    if ((ULLONG_TYPE)((struct caseptrs *)one)->id < (ULLONG_TYPE)((struct caseptrs *)two)->id)
        return -1;
    if ((ULLONG_TYPE)((struct caseptrs *)one)->id > (ULLONG_TYPE)((struct caseptrs *)two)->id)
        return 1;
    return 0;
}
void genbinaryswitch(SNODE *stmt, struct cases *cs)
{
    AMODE *ap1;
	int size = natural_size(stmt->exp);
    InitRegs();
    ap1 = gen_expr_external(stmt->exp, FALSE, F_DREG, BESZ_DWORD);
    gather_cases(stmt->s1,cs,FALSE);
    qsort(cs->ptrs,cs->count, sizeof(cs->ptrs[0]), size < 0 ? sortcmpsigned : sortcmpunsigned);
    bingen(0, (cs->count) / 2, cs->count, ap1, cs, size);
}

//-------------------------------------------------------------------------

void gencompactswitch(SNODE *stmt, struct cases *cs)
{
    int tablab, size;
    AMODE *ap,  *ap2;
    LLONG_TYPE i;
    tablab = nextlabel++;
    InitRegs();
    size = natural_size(stmt->exp);
    if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
    {
	    ap = gen_expr_external(stmt->exp, FALSE, F_AXDX, BESZ_QWORD);
        if (cs->bottom)
        {
            gen_codes(op_sub, BESZ_DWORD, makedreg(EAX), make_immed(cs->bottom));
            #if sizeof(LLONG_TYPE) == 4
                gen_codes(op_sbb, BESZ_DWORD, makedreg(EDX), make_immed(cs->bottom < 0
                    ?  - 1: 0));
            #else 
                gen_codes(op_sbb, BESZ_DWORD, makedreg(EDX), make_immed(cs->bottom >>
                    32));
            #endif 
        }
        else
        {
            gen_codes(op_test, BESZ_DWORD, makedreg(EDX), makedreg(EDX));
        }
		ap->mode = am_dreg;
		ap->preg = EAX;
    }
    else
    {
	    ap = gen_expr_external(stmt->exp, FALSE, F_DREG | F_VOL, BESZ_DWORD);
        if (cs->bottom)
        {
            gen_codes(op_sub, BESZ_DWORD, ap, make_immed(cs->bottom));
        }
        else
        {
            gen_codes(op_test, BESZ_DWORD, ap, ap);
        }
    }
    gen_codes(op_jl, 0, make_label(cs->deflab), 0);
    gen_codes(op_cmp, BESZ_DWORD, ap, make_immed(cs->top - cs->bottom));
    gen_codes(op_jge, 0, make_label(cs->deflab), 0);
    ap->mode = am_indispscale;
    ap->sreg = ap->preg;
    ap->preg =  - 1;
    ap->scale = 2;
	if (prm_farkeyword)
		ap->seg = CS;
    ap->offset = makeintnode(en_labcon, tablab);
    gen_codes(op_jmp, BESZ_DWORD, ap, 0);

    InitRegs();
    align(stdaddrsize);
    gen_label(tablab);

    gather_cases(stmt->s1,cs,TRUE);
    for (i = 0; i < cs->top - cs->bottom; i++)
        gen_codes(op_dd, 4, make_label(cs->ptrs[i].label), 0);
}

//-------------------------------------------------------------------------

void gensimpleswitch(SNODE *stmt, struct cases *cs)
{
    int i = 0, j;
    AMODE *ap1,  *ap2,  *ap3,  *ap4;
    int size = natural_size(stmt->exp);
    InitRegs();
    if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
	    ap1 = gen_expr_external(stmt->exp, FALSE, F_AXDX, BESZ_QWORD);
    else
	    ap1 = gen_expr_external(stmt->exp, FALSE, F_DREG, BESZ_DWORD);
    gather_cases(stmt->s1,cs,FALSE);
    qsort(cs->ptrs,cs->count, sizeof(cs->ptrs[0]), size < 0 ? sortcmpsigned : sortcmpunsigned);

    for (i = 0; i < cs->count; i++)
    {
        if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
        {
            int lab = nextlabel++;
            ap2 = make_immed(cs->ptrs[i].id);
            #if sizeof(LLONG_TYPE) == 4
                ap4 = make_immed(cs->ptrs[i].id < 0 ? 0xffffffff : 0);
            #else 
                ap4 = make_immed(cs->ptrs[i].id >> 32);
            #endif 
            ap3 = make_label(cs->ptrs[i].label);
            gen_coden(op_cmp, BESZ_DWORD, makedreg(EAX), ap2);
            gen_code(op_jne, make_label(lab), 0);
            gen_coden(op_cmp, BESZ_DWORD, makedreg(EDX), ap4);
            gen_code(op_je, ap3, 0);
            gen_label(lab);
        }
        else
        {
            ap2 = make_immed(cs->ptrs[i].id);
            ap3 = make_label(cs->ptrs[i].label);
            gen_coden(op_cmp, ap1->length, ap1, ap2);
            gen_code(op_je, ap3, 0);
        }
    }
    gen_codes(op_jmp, 0, make_label(cs->deflab), 0);
}

//-------------------------------------------------------------------------


//-------------------------------------------------------------------------


void genxswitch(SNODE *stmt)
/*
 *      analyze and generate best switch statement.
 */
{
    int oldbreak;
    struct cases cs;
    int a = 1;
    OCODE *result;
    oldbreak = breaklab;
    breaklab = nextlabel++;
    memset(&cs,0,sizeof(cs));
#if sizeof(LLONG_TYPE) == 4
    cs.top = INT_MIN;
    cs.bottom = INT_MAX;
#else
    cs.top = ((LLONG_TYPE)a << 63); // LLONG_MIN
    cs.bottom = cs.top - 1; // LLONG_MAX
#endif
    cs.deflab = nextlabel++;
    switch (analyzeswitch(stmt,&cs))
    {
        case 3:
            gensimpleswitch(stmt, &cs);
            break;
        case 2:
            genbinaryswitch(stmt, &cs);
            break;
        case 1:
            gencompactswitch(stmt, &cs);
            break;
        case 0:
            break;
    }
    result = gen_codes(op_blockstart, 0, 0, 0);
    result->blocknum = (int)stmt->label;
    genstmt(stmt->s1);
    result = gen_codes(op_blockend, 0, 0, 0);
    result->blocknum = (int)stmt->label;
    if (!cs.diddef)
        gen_label(cs.deflab);
    gen_label(breaklab);
    breaklab = oldbreak;
}

//-------------------------------------------------------------------------

void AddProfilerData(void)
{
    char pname[256];
    if (prm_profiler)
    {
        sprintf(pname, "%s", currentfunc->name);
        plabel = stringlit(pname, FALSE, strlen(pname)+1);
        gen_codes(op_push, BESZ_DWORD , make_label(plabel), 0);
        call_library("__profile_in");
        gen_codes(op_pop, BESZ_DWORD , makedreg(ECX), 0);
    }
}

//-------------------------------------------------------------------------

void SubProfilerData(void)
{
    if (prm_profiler)
    {
        gen_codes(op_push, BESZ_DWORD , make_label(plabel), 0);
        call_library("__profile_out");
        gen_codes(op_pop, BESZ_DWORD , makedreg(ECX), 0);
    }
}

//-------------------------------------------------------------------------

OCODE *ThunkAddPeep(OCODE *after, int op, AMODE *ap1, AMODE *ap2)
{
    OCODE *new;
    new = xalloc(sizeof(OCODE));
    new->opcode = op;
    if (op == op_label)
    {
        new->oper1 = ap1;
        new->oper2 = 0;
    }
    else
    {
        new->oper1 = copy_addr(ap1);
        new->oper2 = copy_addr(ap2);
    }
    //        fixlen(new->oper1);
    //        fixlen(new->oper2);
    new->fwd = after->fwd;
    after->fwd->back = new;
    new->back = after;
    after->fwd = new;
    return new;
}

//-------------------------------------------------------------------------

void ThunkUncommittedStack(void)
{
    if (lc_maxauto >= 4096)
    {
        OCODE *after = frame_ins->back;
        AMODE *ap;
        remove_peep_entry(frame_ins);
        after = ThunkAddPeep(after, op_mov, makedreg(ECX), make_immed
            (lc_maxauto));
        ap = set_symbol("___substack", 1);
        after = ThunkAddPeep(after, op_call, ap, 0);
    }
}

//-------------------------------------------------------------------------

AMODE *genreturn(SNODE *stmt, int flag)
/*
 *      generate a return statement.
 */
{
    AMODE *ap = 0,  *ap1,  *ap2;
    ENODE ep;
    int size, esize, t, q;
    SubProfilerData();
    ThunkUncommittedStack();
    if (stmt != 0 && stmt->exp != 0)
    {
        if (!genning_inline)
            InitRegs();
        if (currentfunc->tp->btp && isstructured(currentfunc->tp->btp))
        {
            size = currentfunc->tp->btp->size;
//            ep.nodetype = en_ul_ref;
//            ep.v.p[0] = stmt->exp;
            ap = gen_expr_external(stmt->exp, FALSE, F_DREG, BESZ_DWORD);
            returndreg = TRUE;
        }
        else if (currentfunc->tp->btp && currentfunc->tp->btp->type ==
            bt_memberptr)
        {
            size = 4;
            ap = gen_expr_external(stmt->exp, FALSE, F_DREG | F_MEM, BESZ_DWORD );
            returndreg = TRUE;

        }
        else
        {
            {
                if (currentfunc->tp->btp->type == bt_bool)
                    size = 5;
                else
                    size = currentfunc->tp->btp->size;
                if (size == 4 && currentfunc->tp->btp->type == bt_float)
                    size = 7;
                if (currentfunc->tp->btp->type == bt_farpointer)
                {
                    ap = gen_expr_external(stmt->exp, FALSE, F_AXDX, BESZ_FARPTR);
                    ap->length = BESZ_QWORD;
                    retsize = 8;
                }
                else if (currentfunc->tp->btp->type == bt_longlong ||
                    currentfunc->tp->btp->type == bt_unsignedlonglong)
                {
                    size = currentfunc->tp->btp->type == bt_longlong ?  - BESZ_QWORD: BESZ_QWORD;
                    ap = gen_expr_external(stmt->exp, FALSE, F_AXDX, size);
                }
                else if (currentfunc->tp->btp->type == bt_float || currentfunc->tp->btp->type == bt_double 
                        || currentfunc->tp->btp->type == bt_longdouble)
                {
                    retsize = 10;
                    ap = gen_expr_external(stmt->exp, FALSE, F_FREG, 10);
                }
                else if (currentfunc->tp->btp->type == bt_fimaginary || currentfunc->tp->btp->type == bt_rimaginary 
                        || currentfunc->tp->btp->type == bt_lrimaginary)
                {
                    retsize = 17;
                    ap = gen_expr_external(stmt->exp, FALSE, F_FREG, 17);
                }
                else if (currentfunc->tp->btp->type == bt_fcomplex || currentfunc->tp->btp->type == bt_rcomplex 
                        || currentfunc->tp->btp->type == bt_lrcomplex)
                {
                    retsize = 22;
                    ap = gen_expr_external(stmt->exp, FALSE, F_FREG, 22);
                }
                else
                {
                    returndreg = TRUE;
                    retsize = 4;
                    switch (currentfunc->tp->btp->type)
                    {
                        case bt_char:
                        case bt_short:
                        case bt_int:
                        case bt_long:
                            size = - BESZ_DWORD;
                            break;
                        default:
                            size = BESZ_DWORD;
                            break;
                    }
                    ap = gen_expr_external(stmt->exp, FALSE, F_DREG, size);
                    if (ap->preg != 0)
                    {
                        gen_codes(op_mov, size, makedreg(0), ap);
                        ap = makedreg(EAX);
                    }
                }
            }
        }
    }
    if (flag &1)
    {
        if (retlab !=  - 1)
            gen_label(retlab);
        if (flag &2)
        {
            return ap;
        }
        if (currentfunc->value.classdata.eofdest)
        {
            if (returndreg)
            {
                if (currentfunc->tp->btp->type == bt_longlong || currentfunc
                    ->tp->btp->type == bt_unsignedlonglong)
                    gen_codes(op_push, BESZ_DWORD , makedreg(2), 0);
                gen_codes(op_push, BESZ_DWORD , makedreg(0), 0);

            }
            gen_expr_external(currentfunc->value.classdata.eofdest, TRUE, F_DREG | F_IMMED | F_MEM | F_AXDX | F_FREG, 0);
            if (returndreg)
            {
                gen_codes(op_pop, BESZ_DWORD , makedreg(0), 0);
                if (currentfunc->tp->btp->type == bt_longlong || currentfunc
                    ->tp->btp->type == bt_unsignedlonglong)
                    gen_codes(op_pop, BESZ_DWORD , makedreg(2), 0);
            }

        }
        if ((conscount || try_block_list || currentfunc
            ->value.classdata.throwlist && currentfunc
            ->value.classdata.throwlist->data) && prm_xcept)
            call_library("__RundownExceptBlock");
		if (used_alloca)
		{
			AMODE *ap = gen_expr_external(allocaSP, FALSE, F_DREG | F_MEM | F_IMMED, 4);
			gen_codes(op_mov, 4, makedreg(ESP), ap);
		}
        if (fsave_mask != 0)
            diag("Float restore in return");
        if (currentfunc->loadds && !currentfunc->intflag)
            gen_code(op_pop, makesegreg(DS), 0);
        if (save_mask != 0)
            PopRegs(save_mask);
        if (prm_cplusplus && prm_xcept || (funcfloat || lc_maxauto ||
            currentfunc->tp->lst.head && currentfunc->tp->lst.head != (SYM*) -
            1)
             || (currentfunc->value.classdata.cppflags &PF_MEMBER) && !
                 (currentfunc->value.classdata.cppflags &PF_STATIC)
         || !prm_smartframes || !stackframeoff)
        {
            /* LEAVE is a little faster in simple tests, but since this version
             * can be paired with other instructions on later processors we
             * do it this way.  Also don't reload ESP if no local vars...
             */
            if (funcfloat || lc_maxauto)
                gen_code(op_mov, makedreg(ESP), makedreg(EBP));
            else
                InitRegs();
            if (!currentfunc->intflag)
                gen_code(op_pop, makedreg(EBP), 0);
        }
        if (currentfunc->intflag || currentfunc->faultflag)
        {
            gen_codes(op_popad, 0, 0, 0);
            if (prm_farkeyword)
            {
                GenPop(GS + 24, am_dreg, 0);
                GenPop(FS + 24, am_dreg, 0);
                GenPop(ES + 24, am_dreg, 0);
            }
        }
        if (currentfunc->loadds && currentfunc->intflag)
            gen_code(op_pop, makesegreg(DS), 0);
        if (currentfunc->faultflag)
        {
            gen_codes(op_add, BESZ_DWORD , makedreg(ESP), make_immed(4));
            gen_codes(op_iretd, 0, 0, 0);
        }
        if (currentfunc->intflag)
        {
            gen_codes(op_iretd, 0, 0, 0);
        }
        else
        {
            int retop = op_ret;
            if (currentfunc->farproc)
                retop = op_retf;
            if (currentfunc->pascaldefn || currentfunc->isstdcall)
            {
                long retsize = 0;
                if (currentfunc->tp->lst.head && currentfunc->tp->lst.head != 
                    (SYM*) - 1)
                {
                    retsize = currentfunc->paramsize - 8;
                }
                if (currentfunc->tp->btp && isstructured(currentfunc->tp->btp))
                    retsize += 4;
                if (currentfunc->farproc)
                    retsize -= 4;
                if (retsize)
                {
                    gen_codes(retop, 0, make_immed(retsize), 0);
                    return 0;
                }
            }
            gen_codes(retop, 0, 0, 0);
        }
    }
    else
    {
        gen_codes(op_jmp, 0, make_label(retlab), 0);
		InitRegs();
    }
    return 0;
}

//-------------------------------------------------------------------------

void gen_tryblock(void *val)
{
    AMODE *ap1 = xalloc(sizeof(AMODE));
    ap1->mode = am_indisp;
    ap1->preg = EBP;
    ap1->offset = makeintnode(en_icon,  - xceptoffs + 12); // ESP
    switch ((int)val)
    {
        case 0:
            // begin try
            gen_codes(op_push, BESZ_DWORD , makedreg(EBX), 0);
            gen_codes(op_push, BESZ_DWORD , makedreg(ESI), 0);
            gen_codes(op_push, BESZ_DWORD , makedreg(EDI), 0);
            gen_codes(op_push, BESZ_DWORD , ap1, 0);
            gen_codes(op_mov, BESZ_DWORD , ap1, makedreg(ESP));
            break;
        case 1:
            // begin catch
            gen_codes(op_mov, BESZ_DWORD , makedreg(ESP), ap1);
            gen_codes(op_pop, BESZ_DWORD , ap1, 0);
            gen_codes(op_pop, BESZ_DWORD , makedreg(EDI), 0);
            gen_codes(op_pop, BESZ_DWORD , makedreg(ESI), 0);
            gen_codes(op_pop, BESZ_DWORD , makedreg(EBX), 0);
            break;
        case 2:
            // end try
            gen_codes(op_mov, BESZ_DWORD , makedreg(ESP), ap1);
            gen_codes(op_pop, BESZ_DWORD , ap1, 0);
            gen_codes(op_add, BESZ_DWORD , makedreg(ESP), make_immed(3 *4));
            break;
    }
}

//-------------------------------------------------------------------------

void genstmt(SNODE *stmt)
/*
 *      genstmt will generate a statement and follow the next pointer
 *      until the block is generated.
 */
{
    while (stmt != 0)
    {
        OCODE *result;
        switch (stmt->stype)
        {
            case st_case:
                if ((int)stmt->label >= 0)
                    gen_label((int)stmt->label);
                break;
            case st_block:
                result = gen_codes(op_blockstart, 0, 0, 0);
                result->blocknum = (int)stmt->label;
                genstmt(stmt->exp);
                result = gen_codes(op_blockend, 0, 0, 0);
                result->blocknum = (int)stmt->label;
                break;
            case st_label:
                gen_label((int)stmt->label);
                break;
            case st_goto:
                gen_codes(op_jmp, 0, make_label((int)stmt->label), 0);
                break;
            case st_tryblock:
                InitRegs(); // important!!!
                gen_tryblock(stmt->label);
                break;
            case st_expr:
                gen_void_external(stmt->exp);
                InitRegs();
                break;
            case st_return:
                genreturn(stmt, 0);
                break;
            case st_if:
                genif(stmt);
                break;
            case st_while:
                genwhile(stmt);
                break;
            case st_do:
                gendo(stmt);
                break;
            case st_for:
                gen_for(stmt);
                break;
            case st_continue:
                gen_codes(op_jmp, 0, make_label(contlab), 0);
                break;
            case st_break:
                gen_codes(op_jmp, 0, make_label(breaklab), 0);
                break;
            case st_switch:
                genxswitch(stmt);
                break;
            case st_line:
                gen_line(stmt);
                break;
            case st_asm:
                if (stmt->exp)
                    add_peep(stmt->exp);
                break;
            case st__genword:
                gen_codes(op_genword, 0, make_immed((int)stmt->exp), 0);
                break;
            default:

                DIAG("unknown statement.");
                break;
        }
        stmt = stmt->next;
    }
}

//-------------------------------------------------------------------------

    void scppinit(void)
    /*
     * code in MAIN to execute C++ startup routines
     * it is safe to do it in MAIN since you cannot call main or take its address
     *
     */
    {
        #ifdef OLD
            if (!strcmp(currentfunc->name, "_main"))
            {
                AMODE *ap1,  *ap2,  *ap3,  *ap4;
                int lbl = nextlabel++;
                InitRegs();
                ap1 = makedreg(ESI);
                ap4 = makedreg(EAX);
                ap2 = set_symbol("cppistart", 0);
                ap3 = set_symbol("cppiend", 0);
                gen_code(op_push, ap1, 0);
                gen_code(op_mov, ap1, ap2);
                gen_label(lbl);
                gen_code(op_lodsd, 0, 0);
                gen_code(op_call, ap4, 0);
                gen_codes(op_cmp, BESZ_DWORD , ap1, ap3);
                gen_code(op_jb, make_label(lbl), 0);
                gen_code(op_pop, ap1, 0);
            }
        #endif 
    }
    SYM *gen_mp_virtual_thunk(SYM *vsp)
    {
        LIST *v = mpthunklist;
        SYM *sp;
        AMODE *ap1,  *ap2;
        char buf[256];
        while (v)
        {
            sp = (SYM*)v->data;
            if (sp->value.i == vsp->value.classdata.vtabindex)
                if (isstructured(vsp->tp->btp) == isstructured(sp->tp->btp))
                    return sp;
            v = v->link;
        }
        global_flag++;
        sp = makesym(sc_static);
        sp->tp = vsp->tp;
        sp->value.i = vsp->value.classdata.vtabindex;
        sprintf(buf, "@$mpt$%d$%d", sp->value.i, isstructured(sp->tp->btp));
        sp->name = litlate(buf);
        sp->staticlabel = FALSE;
        v = xalloc(sizeof(LIST));
        v->data = sp;
        v->link = mpthunklist;
        mpthunklist = v;
        global_flag--;
        //   gen_codelab(sp) ;
        gen_virtual(sp, FALSE);
        ap1 = makedreg(ESP);
        ap1->mode = am_indisp;
        ap1->offset = makeintnode(en_icon, isstructured(sp->tp->btp) ? 8 : 4);
        gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), ap1);
        ap1->preg = 0;
        if (sp->value.classdata.vtaboffs)
            ap1->offset = makeintnode(en_icon, sp->value.classdata.vtaboffs);
        else
            ap1->offset = makeintnode(en_icon, 0);
        gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), ap1);
        ap1->offset = makeintnode(en_icon, sp->value.i);
        gen_codes(op_jmp, BESZ_DWORD , ap1, 0);
        flush_peep();
        gen_endvirtual(sp);
        return sp;
    }
    SYM *gen_vsn_virtual_thunk(SYM *func, int ofs)
    {
        SYM *sp;
        AMODE *ap1,  *ap2;
        char buf[256];
        global_flag++;
        sp = makesym(sc_static);
        sp->value.i = ofs;
        sprintf(buf, "@$vsn%s$%d", func->name, sp->value.i);
        sp->name = litlate(buf);
        sp->staticlabel = FALSE;
        sp->tp = func->mainsym; // strange use of the TP field here 
        global_flag--;
        gen_virtual(sp, FALSE);
        ap1 = makedreg(ESP);
        ap1->mode = am_indisp;
        ap1->offset = makeintnode(en_icon, BESZ_DWORD );
        gen_codes(op_add, BESZ_DWORD , ap1, make_immed( - ofs));
        ap2 = xalloc(sizeof(AMODE));
        ap2->offset = makenode(en_napccon, (void*)func, 0);
        ap2->mode = am_immed;
        gen_codes(op_jmp, BESZ_DWORD , ap2, 0);
        flush_peep();
        gen_endvirtual(sp);
        return sp;

    }
void genfunc(SNODE *stmt)
/*
 *      generate a function body.
 */
{
    //int isconst = FALSE;
    cseg();
    currentfunc->gennedvirtfunc = (virtualfuncs || (currentfunc
        ->value.classdata.cppflags &(PF_HEADERFUNC | PF_INSTANTIATED)));
    if (currentfunc->storage_class == sc_global && !(currentfunc
        ->value.classdata.cppflags &PF_INLINE))
        if (!currentfunc->gennedvirtfunc)
            globaldef(currentfunc);
    returndreg = FALSE;
    retsize = 0;
    contlab = breaklab =  - 1;
    retlab = nextlabel++;
    startlab = nextlabel++;
    funcfloat = 0;
        if (currentfunc->gennedvirtfunc)
            gen_virtual(templateSP ? templateSP : currentfunc, FALSE);
        else
        gen_codelab(currentfunc);
     /* name of function */

    if (funcstmt)
        gen_line(funcstmt);
    funcstmt = 0;
    opt1(stmt); /* push args & also subtracts SP */
	if (funcbrstmt)
        gen_line(funcbrstmt);
	funcbrstmt = 0;
	if (prm_debug)
		gen_code(op_nop, 0, 0); // padding to make breakpoint on open bracket happy
        #ifdef XXXXX
            if (isconst)
            {
                SYM *psp = currentfunc->parentclass;
                if (currentfunc->value.classdata.basecons)
                    gen_void_external(currentfunc->value.classdata.basecons);
                if (psp->value.classdata.baseclass->vtabsp)
                {
                    CLASSLIST *l = psp->value.classdata.baseclass;
                    ENODE *thisn = copynode(thisenode);
                    ENODE *ts = makenode(en_nacon, psp
                        ->value.classdata.baseclass->vtabsp, 0);
                    ENODE *exp1,  *exp2;
                    thisn = makenode(en_a_ref, thisn, 0);
                    while (l)
                    {
                        if (l->flags &CL_VTAB)
                        {
                            exp1 = makenode(en_addstruc, ts, makeintnode
                                (en_icon, l->vtaboffs));
                            exp2 = makenode(en_addstruc, thisn, makeintnode
                                (en_icon, l->offset));
                            exp2 = makenode(en_a_ref, exp2, 0);
                            exp2 = makenode(en_assign, exp2, exp1);
                            InitRegs();
                            gen_expr_external(exp2, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
                        }
                        l = l->link;
                    }
                }
            }
        #endif 
        if (prm_cplusplus)
            scppinit();
    InitRegs();
	if (used_alloca)
		{
			AMODE *ap = gen_expr_external(allocaSP, FALSE, F_DREG | F_MEM, 4);
			gen_codes(op_mov, 4, ap, makedreg(ESP));
		}
    genstmt(stmt);
        if (block_rundown)
            if (retsize)
        if (retsize > 4)
        {
            gen_code(op_fstp, floatconvpos(), 0);
            gen_void_external(block_rundown);
            gen_code(op_fld, floatconvpos(), 0);
        }
        else
        {
            gen_code(op_push, makedreg(EAX), 0);
            gen_void_external(block_rundown);
            gen_code(op_pop, makedreg(EAX), 0);
        }
        else
            gen_void_external(block_rundown);
    if (funcendstmt)
        gen_line(funcendstmt);
    funcendstmt = 0;
    genreturn(0, 1);
        thissp->inreg = FALSE;
        thissp->value.i = 8;
    flush_peep();
        if (currentfunc->gennedvirtfunc)
            gen_endvirtual(templateSP ? templateSP : currentfunc);
        if ((conscount || try_block_list || currentfunc
            ->value.classdata.throwlist && currentfunc
            ->value.classdata.throwlist->data) && prm_xcept)
            dumpXceptBlock(currentfunc);
}
