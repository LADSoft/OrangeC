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
/*
 * Statement parser
 */
#include <stdio.h> 
#include <string.h>
#include <ctype.h>
#include "lists.h"
#include "expr.h" 
#include "c.h" 
#include "ccerr.h"
#include "diag.h"

extern TABLE labelsyms;
extern int prm_oldfor;
extern TYP stdint;
extern int funcnesting;
extern int prm_c99;
extern int defaulttype;
extern int indefaultinit;
extern int stdpragmas;
extern int block_nesting;
extern char *cpp_funcname_tab[];
extern LIST *instantiated_inlines;
extern LIST *local_using_list,  *local_tag_using_list;
extern SYM undef;
extern ENODE *block_rundown;
extern int lastch;
extern enum e_sym lastst, lastlastst;
extern char lastid[];
extern TABLE lsyms, oldlsym, ltags, oldltag;
extern int lineno;
extern SYM *currentfunc;
extern int skm_closepa[];
extern int incldepth;
extern short unalteredline[];
extern int prm_cplusplus, prm_debug;
extern int prm_ansi;
extern int vtabhead;
extern int currentfile;
extern FILELIST *lastinc;
extern int dbgblocknum;
extern short *lptr;
extern int prm_cmangle;
extern int arg_nesting;
extern SNODE *funcendstmt;
extern SYM *typequal;
extern char declid[];
extern SYM *catch_cleanup_func,  *global_vararray_func;
extern TYP **headptr,  *head;
extern int prm_xcept;
extern int skm_declend[];
extern int global_flag;
extern int stdaddrsize, stdintsize;
extern ENODE *expr_runup[];
extern ENODE *expr_rundown[];
extern int expr_updowncount;
extern int infuncargs;
extern int gotoCount;

ENODE *blockVarArraySP; 

int selectscalar(TYP *tp, ENODE *ep)
{
    return tp->type == bt_pointer && (!tp->val_flag || lvalue(ep))
	|| scalarnoncomplex(tp) ;
}
int incatchclause;
int asmline;
int skm_openpa[] = 
{
    openpa, begin, semicolon, 0
};
int skm_semi[] = 
{
    semicolon, end, 0
};
int skm_tocase[] = 
{
    kw_case, kw_default, end, 0
};
long nextlabel, firstlabel;
int goodcode;
int switchbreak;

TRYBLOCK *try_block_list;

#define CASE_MAX 512
struct caselist {
    struct caselist *next;
    int last;
    LLONG_TYPE cases[CASE_MAX];
} *currentCases ;

static int gotellipse;
static char phibuf[4096];
static int lastlineno = 0;
static int hasdefault;
static int inswitch;
static int switchwidth;

SNODE *cbautoinithead,  *cbautoinittail,  *cbautorundownhead, 
    *cbautorundowntail;
LASTDEST *lastDestPointer;


void stmtini(void)
{
    lastlineno = 0;
    nextlabel = firstlabel = 1;
    goodcode = 0;
    asmline = FALSE;
    lastDestPointer = 0;
    inswitch = 0;
}
/* get the size of the expression, and cast up to int as necessary */
static TYP *stmtsize(ENODE **node, TYP *tp)
{
#ifdef XXXXX
    TYP *rv;
    enum e_bt t1 ;
    if (!tp || !*node)
        return tp;
    t1 = tp->type;
    if (t1 <= bt_int)
        t1 = bt_int;
    else if (t1 <= bt_unsigned)
        t1 = bt_unsigned;
    
    rv = inttype(t1);
    if (rv->type != tp->type)
        promote_type(rv,node);
    return rv ;
#else
    return tp;
#endif
}
/* scan an expression and determine if there are any pias */
TYP *doassign(ENODE **exp, int canerror, int *skm)
{
    TYP *tp;
    goodcode |= GF_INIF;
    if ((tp = expression(exp, TRUE)) == 0)
    {
        if (canerror)
            generror(ERR_EXPREXPECT, 0, skm);
        *exp = makenode(en_nacon, &undef, 0);
    }
    goodcode &= ~GF_INIF;
    // cast to higher level
    tp = stmtsize(exp,tp);
    return tp;
}

//-------------------------------------------------------------------------

SNODE *whilestmt(void)
/* 
 *      whilestmt parses the c while statement. 
 */
{
    SNODE *snp;
    int ogc = goodcode;
    TYP *tp ;
    goodcode |= GF_INLOOP;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_while;
    snp->s1 = 0;
    getsym();
    needpunc(openpa, 0);
    tp = doassign(&snp->exp, TRUE, 0);
    if (tp)
    {
        if (tp->type == bt_cond)
            tp = tp->btp ;
        if (!selectscalar(tp, snp->exp)) 
            generror(ERR_SELECTSCALAR, 0, 0);
    }
    needpunc(closepa, skm_closepa);
    goodcode |= GF_CONTINUABLE | GF_BREAKABLE;
    snp->s1 = cppblockedstatement(TRUE);
	if (!switchbreak)
	{
		ENODE *ep = snp->exp;
		while(castvalue(ep))
			ep = ep->v.p[0];
		if (isintconst(ep->nodetype) && ep->v.i)
			ogc |= GF_UNREACH;
	}
    goodcode = ogc;
    return snp;
}

//-------------------------------------------------------------------------

SNODE *dostmt(void)
/* 
 *      dostmt parses the c do-while construct. 
 */
{
    SNODE *snp;
    TYP *tp;
    int ogc = goodcode, oswb = switchbreak;
    goodcode |= GF_INLOOP;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_do;
    getsym();
    goodcode |= GF_CONTINUABLE | GF_BREAKABLE;
    snp->s1 = cppblockedstatement(TRUE);
    goodcode = ogc;
    if (lastst != kw_while)
        gensymerror(ERR_IDENTEXPECT, lastid);
    else
        getsym();
    needpunc(openpa, 0);
    tp = doassign(&snp->exp, TRUE, 0);
    if (tp)
    {
        if (tp->type == bt_cond)
            tp = tp->btp ;
        if (!selectscalar(tp, snp->exp)) 
            generror(ERR_SELECTSCALAR, 0, 0);
    }

    needpunc(closepa, skm_closepa);
    if (lastst != eof)
        needpunc(semicolon, 0);
	if (!switchbreak)
	{
		ENODE *ep = snp->exp;
		while(castvalue(ep))
			ep = ep->v.p[0];
		if (isintconst(ep->nodetype) && ep->v.i)
			goodcode |= GF_UNREACH;
	}
    switchbreak = oswb;
    return snp;
}

//-------------------------------------------------------------------------

SNODE *forstmt(void)
/*
 * Generate a block for a for statement
 */
{
    SNODE *snp,  *forexp;
    int ogc = goodcode, oswb = switchbreak;
    int plussemi = 0;
    int declared = 0;
    int oldpragma;
    TABLE ools, oolt;
    goodcode |= GF_INLOOP;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    getsym();
    snp->label = snp->exp = snp->s2 = 0;
    snp->stype = st_for;
    if (needpunc(openpa, skm_closepa))
    {
            /* CPLUSPLUS allows you to declare a variable here */
            if (castbegin(lastst) || lastst == kw_auto || lastst == kw_register)
            {
                if (prm_cplusplus || prm_c99)
                {
                    ENODE *exp = 0,  **next = &exp;
                    cbautoinithead = cbautoinittail = 0;
                    ools = oldlsym;
                    oldlsym = lsyms;
                    oolt = oldltag;
                    oldltag = ltags;
                    oldpragma = stdpragmas;
                    declared = 1;
                    block_nesting++;
                    blockdecl();
                    block_nesting--;
                    while (cbautoinithead)
                    {
                        if (*next)
                        {
                            *next = makenode(en_void, (*next), cbautoinithead
                                ->exp);
                            next = &(*next)->v.p[1];
                        }
                        else
                             *next = cbautoinithead->exp;
                        cbautoinithead = cbautoinithead->next;
                    }
                    snp->label = exp;
                    plussemi = 1;
                }
                else
                {
                    generror(ERR_NODECLARE, 0, 0);
                    snp->label = makenode(en_nacon, &undef, 0);
                    while (castbegin(lastst))
                        getsym();
                    goto forjoin;
                }
            }
            else
            forjoin: 
        {
            if (expression(&snp->label, TRUE) == 0)
                snp->label = 0;
            plussemi = needpunc(semicolon, 0);
        }
        if (plussemi && lastst != closepa)
        {
            TYP *tp;
            goodcode |= GF_INIF;
            tp = expression(&snp->exp, TRUE);
            tp = stmtsize(&snp->exp, tp);
            if (tp)
            {
                if (tp->type == bt_cond)
                    tp = tp->btp ;
                if (!selectscalar(tp, snp->exp)) 
                    generror(ERR_SELECTSCALAR, 0, 0);
            }
            forexp = snp->exp;
            goodcode &= ~GF_INIF;
            //                  doassign(&snp->exp,FALSE,0);
            if (lastst == closepa)
            {
                snp->s2 = snp->exp;
                forexp = snp->exp = 0;
            }
            else
            if (needpunc(semicolon, 0))
            {
                if (expression(&snp->s2, TRUE) == 0)
                {
                    snp->s2 = 0;
                }
            }
        }
        needpunc(closepa, skm_closepa);
    }
    goodcode |= GF_CONTINUABLE | GF_BREAKABLE;
    snp->s1 = cppblockedstatement(TRUE);
    if (declared && !prm_oldfor)
    {
        SYM *q = lsyms.head;
        gather_labels(&oldlsym, &lsyms);
        while (q && q->next != oldlsym.head)
            q = q->next;
        if (q)
            q->next = 0;
        addblocklist(lsyms.head, oldlsym.head);
        addrundown(snp);
        check_funcused(&oldlsym, &lsyms);
        if (oldlsym.head != lsyms.head)
            arg_nesting++;
        stdpragmas = oldpragma;
        lsyms = oldlsym;
        oldlsym.head = ools.head;
        ltags = oldltag;
        oldltag.head = oolt.head;
    }
    goodcode = ogc;
    if (!switchbreak)
	{
		if (!forexp)
	        goodcode |= GF_UNREACH;
		else
		{
			ENODE *ep = snp->exp;
			while(castvalue(ep))
				ep = ep->v.p[0];
			if (isintconst(ep->nodetype) && ep->v.i)
				goodcode |= GF_UNREACH;
		}
	}
    switchbreak = oswb;
    return snp;
}

//-------------------------------------------------------------------------

SNODE *ifstmt(void)
/* 
 *      ifstmt parses the c if statement and an else clause if 
 *      one is present. 
 */
{
    SNODE *snp;
    int ogc = goodcode;
    int temp = 0, temp1 = 0;
    TYP *tp;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_if;
    getsym();
    needpunc(openpa, 0);
    tp = doassign(&snp->exp, TRUE, 0);
    if (tp)
    {
        if (tp->type == bt_cond)
            tp = tp->btp ;
        if (!selectscalar(tp, snp->exp)) 
            generror(ERR_SELECTSCALAR, 0, 0);
    }
    
    needpunc(closepa, skm_closepa);
    snp->s1 = cppblockedstatement(FALSE);
    temp1 = goodcode &(GF_RETURN | GF_BREAK | GF_CONTINUE | GF_GOTO | GF_THROW 
					   | GF_UNREACH);
    if (lastst == kw_else)
    {
        goodcode = ogc;
        getsym();
        snp->s2 = cppblockedstatement(FALSE);
        temp = goodcode &(GF_RETURN | GF_BREAK | GF_CONTINUE | GF_GOTO |
            GF_THROW | GF_UNREACH);
    }
    else
        snp->s2 = 0;
    goodcode = ogc | (temp &temp1);
    return snp;
}

//-------------------------------------------------------------------------

SNODE *casestmt(void)
/* 
 *      cases are returned as seperate statements. for normal 
 *      cases label is the case value and s2 is zero. for the 
 *      default case s2 is nonzero. 
 */
{
    SNODE *snp;
    SNODE *head = 0,  *tail;
    LLONG_TYPE ssize;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    if (lastst == kw_case)
    {
        struct caselist *cc = currentCases;
        getsym();
        snp->s2 = 0;
        snp->stype = st_case;
        snp->switchid = (ssize = intexpr(0));
        snp->label = (SNODE *)-1;
        if (switchwidth == 1 && (ssize &0xffffff00) || switchwidth == 2 && (ssize &0xffff0000)
            || switchwidth ==  - 1 && (ssize <  - 0x80 || ssize > 0x7f) || switchwidth ==  -
            2 && (ssize <  - 0x8000 || ssize > 0x7fff))
            generror(ERR_LOSTCONVCASE, 0, 0);
        while (cc) {
            int i;
            for (i=0; i < cc->last; i++)
                if (ssize == cc->cases[i]) {
                    generror(ERR_DUPCASE,ssize,0);
                    cc = 0;
                    break ;
                }
            if (cc && !cc->next) {
                if (cc->last >= CASE_MAX) {
                    cc->next = xalloc(sizeof(struct caselist));
                    cc = cc->next;
                }
                cc->cases[cc->last++] = ssize;
            }
            if (cc)
                cc = cc->next;
        }
    }
    else // default
    {
        goodcode |= GF_DEF;
        getsym();
        snp->stype = st_case;
        snp->s2 = (SNODE*)1;
        if (hasdefault++) {
            generror(ERR_HASDEF,0,0);
        }
    }
    needpunc(colon, 0);
    return snp;
}


//-------------------------------------------------------------------------

SNODE *switchstmt(void)
/*
 * Handle the SWITCH statement
 */
{
    SNODE *snp;
    TYP *tp;
    int osw = switchwidth;
    int oswb = switchbreak;
    int ogf = goodcode;
    struct caselist *occ = currentCases ;
    int ohd = hasdefault;
    switchbreak = 0;
    hasdefault = 0;
    currentCases = xalloc(sizeof(struct caselist));
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_switch;
    snp->label = dbgblocknum + 1;
    getsym();
    needpunc(openpa, 0);
    tp = doassign(&snp->exp, TRUE, 0);
    switchwidth = natural_size(snp->exp);
    if (tp)
    {
        if (tp->type == bt_cond)
            tp = tp->btp ;
        if (!scalarnonfloat(tp)) 
                generror(ERR_SWITCHINT, 0, 0);
    }
    needpunc(closepa, skm_closepa);
    inswitch++;
    goodcode |= GF_UNREACH | GF_BREAKABLE;
    snp->s1 = cppblockedstatement(TRUE);
    if ((goodcode & GF_DEF) && (goodcode & GF_RETURN) && !(switchbreak))
        ogf |= GF_UNREACH | GF_RETURN;
    else
        ogf &= ~GF_UNREACH;
    inswitch--;
    hasdefault = ohd;
    currentCases = occ;
    goodcode = ogf;
    switchbreak = oswb;
    switchwidth = osw;
    return snp;
}

//-------------------------------------------------------------------------

ENODE *do_ret_constructor(TYP *tp, TYP *head1, ENODE *node, int size, int
    offset, int implicit)
{
    char buf[256];
    int voidfunc = FALSE;
    SYM *sp;
    ENODE *pnode,  *rnode,  *tcnode = 0;
    if (tp->lst.head == (SYM*) - 1)
        voidfunc = TRUE;
    {
        SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &head1->lst);
        if (!sp1)
        {
            return 0;
        }
        sp = funcovermatch(sp1, tp, node, FALSE, FALSE);
        if (!sp)
        {
            return 0;
        }
        pnode = makenode(en_napccon, sp, 0);
    }
    if (!isaccessible(sp))
        genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(sp->parentclass
            ->parentclass, sp->name, 0));
    if (implicit && (sp->value.classdata.cppflags &PF_EXPLICIT))
        generror(ERR_NOEXPLICITHERE, 0, 0);
    sp->mainsym->extflag = TRUE;
    parmlist(&node, tp, sp->tp);
    rnode = makenode(en_void, pnode, pnode);
    pnode = makenode(en_void, rnode, node);
    rnode = makeintnode(en_icon, sp->tp->btp->size);
    if (sp->pascaldefn)
        pnode = makenode(en_pfcall, rnode, pnode);
    else
        if (sp->isstdcall)
            pnode = makenode(en_sfcall, rnode, pnode);
        else
            pnode = makenode(en_fcall, rnode, pnode);
    if (!(sp->value.classdata.cppflags &PF_STATIC))
        tcnode = makenode(en_structret, 0, 0);
        if (prm_cplusplus && tcnode && (sp->value.classdata.cppflags &PF_MEMBER)
            && !(sp->value.classdata.cppflags &PF_STATIC))
        {
            pnode = makenode(en_thiscall, tcnode, pnode);
        }
        if (size == 1)
            pnode = doinline(pnode);
        else
        {
            ENODE *snode;
            snode = makeintnode(en_icon, head1->size);
            snode = makenode(en_void, makeintnode(en_icon, size), (void*)snode);
            pnode = makenode(en_repcons, (void*)snode, (void*)pnode);
            SetExtFlag(sp, TRUE);
        }
        pnode->cflags = head1->cflags;
    return pnode;

}

//-------------------------------------------------------------------------

SNODE *breakcontrundowns(SNODE *snp)
{
    LASTDEST *p = lastDestPointer;
    SNODE *result = 0,  **v = &result;
    while (p)
    {
        if (p->block && p->chain)
        {
            SNODE *snp2 = xalloc(sizeof(SNODE));
            snp2->next = 0;
            snp2->stype = st_expr;
            snp2->exp = p->block;
            *v = snp2;
            v = &(snp2->next);
        }
        p = p->chain;
    }
    *v = snp;
    return result;
}

//-------------------------------------------------------------------------

ENODE *retrundowns(void)
{
    ENODE *ep = 0;
    if (prm_cplusplus)
    {
        LASTDEST *p = lastDestPointer;
        while (p)
        {
            if (p->block && p->chain)
            {
                if (ep)
                    ep = makenode(en_void, ep, p->block);
                else
                    ep = p->block;
            }
            p = p->chain;
        }
    }
    return ep;
}

//-------------------------------------------------------------------------

SNODE *retstmt(void)
/*
 * Handle return 
 */
{
    SNODE *snp;
    TYP *tp;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_return;
    snp->exp = 0;
    getsym();
    if (lastst == end || lastst == semicolon)
    {
        if (currentfunc->tp->btp->type != bt_void)
            generror(ERR_FUNCRETVAL2, 0, 0);
        needpunc(semicolon, 0);
    }
    else
    {
        ENODE *epn;
        int ogc = goodcode;
        goodcode |= GF_SUPERAND;
        tp = expression(&(snp->exp), TRUE);
        tp = stmtsize(&snp->exp, tp);
        if (currentfunc->tp->btp->type == bt_bool)
        {
            snp->exp = makenode(en_cbool, snp->exp, 0);
            typedconsts(snp->exp);
        }
        else if (isstructured(currentfunc->tp->btp))
        {
            ENODE *ep1,  *ep2,  *qn,  **epq = 0,  **ep;
            TYP *tp1;
            if (snp->exp && (snp->exp->nodetype == en_void || snp->exp
                ->nodetype == en_dvoid))
            {
                ep = actualexpr(&snp->exp);
                if (!ep)
                    ep = &snp->exp;
//                epq = &(*ep)->v.p[1];
//                ep = &(*ep)->v.p[0];
            }
            else
                ep = &snp->exp;
            ep1 = makenode(en_structret, 0, 0);
            if (prm_cplusplus)
            {
                SYM s;
                qn = makenode(en_void, (*ep), 0);
                tp1 = maketype(bt_func, 0);
                memset(&s, 0, sizeof(s));
                s.tp = tp;
                tp1->lst.head = tp1->lst.tail = &s;
                ep2 = do_ret_constructor(tp1, currentfunc->tp->btp, qn, 1, 0,
                    !exactype(tp, currentfunc->tp->btp, FALSE));
                if (ep2)
                    tp = currentfunc->tp->btp;
                else if (isstructured(tp))
                {
                    tp1->lst.head = tp1->lst.tail = tp->sp;
                    ep2 = do_ret_constructor(tp1, tp, qn, 1, 0, TRUE);
                }
            }
            else
                ep2 = 0;
            if (!ep2)
            {
                ep2 = makenode(en_moveblock, ep1,  *ep);
                ep2->size = tp->size;
            }
            ep2 = makenode(en_void, ep2, ep1);
            *ep = ep2;
            epn = retrundowns();
            if (epn)
                if (epq)
                    *epq = makenode(en_void,  *epq, epn);
                else
                    *ep = makenode(en_dvoid,  *ep, epn);
        }
        else
        {
            if (currentfunc->tp->btp->type == bt_ref && !isstructured
                (currentfunc->tp->btp->btp))
            {
                while (castvalue(snp->exp))
                    snp->exp = snp->exp->v.p[0];
                if (lvalue(snp->exp))
                    snp->exp = snp->exp->v.p[0];
            }
            if (currentfunc->tp->btp->type == bt_memberptr)
            {
                tp = FindMemberPointer(currentfunc->tp->btp, tp, &snp->exp);
                while (castvalue(snp->exp))
                    snp->exp = snp->exp->v.p[0];
                if (lvalue(snp->exp))
                    snp->exp = snp->exp->v.p[0];
                //                  } else if (currentfunc->tp->btp->type == bt_ref) {
                //                    while (castvalue(snp->exp))
                //                        snp->exp = snp->exp->v.p[0] ;
                //                    if (lvalue(snp->exp))
                //                        snp->exp = snp->exp->v.p[0] ;
            }
            epn = retrundowns();
            if (epn)
                snp->exp = makenode(en_dvoid, snp->exp, epn);
        }
        goodcode = ogc;
        if (lastst != eof)
            needpunc(semicolon, 0);
        if (tp->type == bt_void)
        {
            generror(ERR_NOVOIDRET, 0, 0);
        }
        else
        {
            TYP *tp1 = tp;
            if (tp1->type == bt_ref)
                tp1 = tp1->btp;
            if (currentfunc->tp->btp->type == bt_void)
                generror(ERR_VOIDFUNCNOVALUE, 0, 0);
            else {
                if (!checktype(currentfunc->tp->btp, tp1, TRUE))
                    if (!(isscalar(tp1) && isscalar(currentfunc->tp->btp)))
                        if (currentfunc->tp->btp->type != bt_pointer &&
                            currentfunc->tp->btp->type != bt_farpointer ||
                            floatrecurse(snp->exp))
                            generror(ERR_RETMISMATCH, 0, 0);
				if ((isstructured(currentfunc->tp->btp) || isstructured(tp))
					&& !exactype(currentfunc->tp->btp, tp, 0))
					generror(ERR_ILLSTRUCT, 0, 0);
				if (tp && (tp->type == bt_pointer ||tp->type == bt_farpointer)
					&& (currentfunc->tp->btp->type == bt_pointer || currentfunc->tp->btp->type == bt_farpointer))
				{
					if (!checkexactpointer(currentfunc->tp->btp, tp))
						generror(ERR_SUSPICIOUS,0,0);
				}
            }
        }
    }
    return snp;
}

//-------------------------------------------------------------------------

SNODE *breakstmt(void)
/*
 * handle break
 */
{
    SNODE *snp;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_break;
    getsym();
    if (!(goodcode &GF_BREAKABLE))
        generror(ERR_NOCONTINUE, 0, 0);
    if (lastst != eof)
        needpunc(semicolon, 0);
    return breakcontrundowns(snp);
}

//-------------------------------------------------------------------------

SNODE *contstmt(void)
/*
 * handle continue
 */
{
    SNODE *snp;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_continue;
    if (!(goodcode &GF_CONTINUABLE))
        generror(ERR_NOCONTINUE, 0, 0);
    getsym();
    if (lastst != eof)
        needpunc(semicolon, 0);
    return breakcontrundowns(snp);
}

//-------------------------------------------------------------------------

SNODE *_genwordstmt(void)
/*
 * Insert data in the code stream
 */
{
    SNODE *snp;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st__genword;
    snp->exp = 0;
    getsym();
    if (lastst != openpa)
    {
        generror(ERR_PUNCT, openpa, skm_semi);
        getsym();
        snp = 0;
    }
    else
    {
        getsym();
        snp->exp = (ENODE*)intexpr(0);
        if (lastst != closepa)
        {
            generror(ERR_PUNCT, closepa, skm_semi);
            snp = 0;
        }
        getsym();
    }
    if (lastst != eof)
        needpunc(semicolon, 0);
    return (snp);
}

//-------------------------------------------------------------------------

SNODE *exprstmt(void)
/* 
 *      exprstmt is called whenever a statement does not begin 
 *      with a keyword. the statement should be an expression. 
 */
{
    SNODE *snp;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_expr;
    goodcode &= ~(GF_ASSIGN);
    if (expression(&snp->exp, TRUE) == 0)
    {
        generror(ERR_EXPREXPECT, 0, skm_semi);
        snp->exp = 0;
    }
    if (!(goodcode &GF_ASSIGN))
        generror(ERR_CODENONE, 0, 0);
    if (lastst != eof)
        needpunc(semicolon, 0);
    return snp;
}

//-------------------------------------------------------------------------

SNODE *snp_line(int tocode)
/*
 * construct a statement for the beginning of a new line
 */
{
    SNODE *snp3 = 0;
    if (lineno != lastlineno && lastst != semicolon && lastst != begin)
    {
        int i = 0, j, l = pstrlen(unalteredline);
        snp3 = xalloc(sizeof(SNODE));
        snp3->next = 0;
        snp3->stype = st_line;
        snp3->exp = (ENODE*)lineno;
        snp3->s1 = (SNODE*)currentfile;
        snp3->next = 0;
        for (j = 0; j < l && isspace(unalteredline[j]); j++)
            ;
        for (; j < l; j++)
            phibuf[i++] = unalteredline[j];
        if (phibuf[i - 1] == '\n')
            i--;
        phibuf[i] = 0;
        snp3->label = (SNODE*)xalloc(i + 1);
        strcpy(snp3->label, phibuf);
        lastlineno = lineno;
        if (tocode && currentfile)
            lastinc->hascode = TRUE;
    }
    return snp3;
}

//-------------------------------------------------------------------------

    void dodefaultinit(SYM *sp)
    /*
     * Evalueate a C++ default clause
     */
    {
        TYP *tp;
        ENODE *ep;
        char nm[256];
        if (lastst == assign)
        {
            indefaultinit = TRUE;
            strcpy(nm, declid);
            expr_runup[expr_updowncount] = 0;
            expr_rundown[expr_updowncount++] = 0;
            if ((tp = autoasnop(&ep, sp, TRUE)) == 0)
            {
                generror(ERR_EXPREXPECT, 0, 0);
                getsym();
                expr_updowncount--;
            }
            else
            {
                ep = ep->v.p[1];
                if (sp->tp->type == bt_ref)
                    if (ep->nodetype == en_refassign && (isintconst(ep->v.p[1]
                        ->nodetype) && ep->v.p[1]->v.i != 0 || isfloatconst(ep
                        ->v.p[1]->nodetype)))
                        generror(ERR_REFLVALUE, 0, 0);
                sp->value.classdata.defalt = xalloc(sizeof(DEFVALUE));
                sp->value.classdata.defalt->ep = ep;
                sp->value.classdata.defalt->cons = expr_runup[
                    --expr_updowncount];
                sp->value.classdata.defalt->dest =
                    expr_rundown[expr_updowncount];
            }
            strcpy(declid, nm);
            indefaultinit = FALSE;
        }
    }
void evaluateVLAtypedef(SYM *sp)
{
    TYP *tp = sp->tp;
    if (tp->type == bt_pointer && !(tp->val_flag & VARARRAY))
        tp = tp->btp;
    if (tp->val_flag & VARARRAY) 
    {
        TYP *tp1 = tp->btp;
        while (tp1->esize)
        {
            if (!funcnesting) {
                // DIAG("evaluateVLAtypedef: global typedef to VLA") ;
            } else 
            {
                ENODE *ep1 = dummyvar(stdintsize, &stdint, 0);
                ENODE *ep2;
                SNODE *snp = xalloc(sizeof(SNODE));
                ep1->v.sp->value.i = 1;
                ep1 = makenode(en_a_ref, ep1, 0);
                ep2 = makenode(en_assign, ep1, tp1->esize);
                tp1->esize = ep1;
                snp->next = 0;
                snp->stype = st_expr;
                snp->exp = ep2;
                snp->flags |= ST_VARARRAY ;
                if (cbautoinithead == 0)
                    cbautoinithead = cbautoinittail = snp;
                else
                {
                    cbautoinittail->next = snp;
                    cbautoinittail = snp;
                }
            }
            tp1 = tp1->btp;
        }
    }
}
ENODE *createVarArray(SYM *sp, TYP *tp, int global, int alloc)
{
    TYP *list[100];
    int i;
    ENODE *ep,  *ep1,  *result = 0;
    int count = 0;
    if (tp->type == bt_pointer && (tp->val_flag &VARARRAY))
    {
        TYP *tp1 = tp->btp;
        enum e_node sv = global ? en_nacon : en_autocon;
        while (tp1->esize)
        {
            if (tp1->esize == (ENODE*) - 1)
            {
                tp1->esize = list[count++] = makeintnode(en_icon, 1);
                generror(ERR_VLAMUSTSIZE, 0, 0);
            } 
            else
                list[count++] = tp1->esize;
            tp1->esizeindex = (count + 1); // replace with index in table
            tp1->sp = sp;
            tp1 = tp1->btp;
        }
        list[count++] = makeintnode(en_icon, tp1->size);

        ep = makeintnode(en_icon, count - 1);
        ep1 = makenode(sv, (void*)sp, 0);
        ep1 = makenode(en_add, makeintnode(en_icon, stdaddrsize), ep1);
        ep1 = makenode(en_a_ref, ep1, 0);
        result = makenode(en_assign, ep1, ep);

        ep = list[count - 1];
        ep1 = makenode(sv, (void*)sp, 0);
        ep1 = makenode(en_add, makeintnode(en_icon, stdaddrsize + count *
            stdintsize), ep1);
        ep1 = makenode(en_a_ref, ep1, 0);
        result = makenode(en_void, result, makenode(en_assign, ep1, ep));
        for (i = count - 2; i >= 0; i--)
        {
            ENODE *ep2 = makenode(sv, (void*)sp, 0);
            ep2 = makenode(en_add, makeintnode(en_icon, stdaddrsize + (i + 2)
                *stdintsize), ep2);
            ep2 = makenode(en_a_ref, ep2, 0);
            ep = list[i];
            ep = makenode(en_mul, ep, ep2);
            ep1 = makenode(sv, (void*)sp, 0);
            ep1 = makenode(en_add, makeintnode(en_icon, stdaddrsize + (i + 1)
                *stdintsize), ep1);
            ep1 = makenode(en_a_ref, ep1, 0);
            result = makenode(en_void, result, makenode(en_assign, ep1, ep));
        }
        if (alloc)
        {
            ep = makenode(sv, (void*)sp, 0);
            ep = makenode(en_add, ep, makeintnode(en_icon, stdaddrsize +
                stdintsize));
            ep = makenode(en_a_ref, ep, 0);
            if (global)
            {
                ENODE *ep2 = makenode(en_napccon, (void*)global_vararray_func,
                    0);
                global_vararray_func->mainsym->extflag = TRUE;
                ep1 = makenode(en_void, ep, 0);
                ep1 = makenode(en_void, ep1, 0);
                ep2 = makenode(en_void, ep2, ep2);
                ep1 = makenode(en_void, ep2, ep1);
                ep1 = makenode(en_fcall, makeintnode(en_icon, stdaddrsize), ep1);

            }
            else
            {
                ep1 = makenode(en_substack, ep, 0);
            }
            ep = makenode(global ? en_nacon : en_autocon, (void*)sp, 0);
            ep = makenode(en_a_ref, ep, 0);
            ep1 = makenode(en_assign, ep, ep1);
            result = makenode(en_void, result, ep1);
        }
        return result;
    }
    return 0;
}

//-------------------------------------------------------------------------

void doautoinit(SYM *sym)
/*
 * This is here rather than in init because autoinit is a type of 
 * statement
 */
{
    TYP *tp;
        #ifdef XXXXX
            if (prm_cplusplus && sym->storage_class != sc_static && sym
                ->storage_class != sc_global && (sym->tp->type == bt_class ||
                sym->tp->type == bt_struct))
            {
                SNODE *snp = 0,  *snp1;
                ENODE *exp;
                SYM *typesp = sym->tp->sp;

                //      if (lastst == openpa || lastst == assign) {
                snp = snp_line(TRUE);
                if (snp)
                    if (cbautoinithead == 0)
                        cbautoinithead = cbautoinittail = snp;
                    else
                {
                    cbautoinittail->next = snp;
                    cbautoinittail = snp;
                }
                basedecl(&exp, sym);
                if (exp)
                {
                    snp = xalloc(sizeof(SNODE));
                    snp->next = 0;
                    snp->stype = st_expr;
                    snp->exp = exp;
                    if (snp)
                        if (cbautoinithead == 0)
                            cbautoinithead = cbautoinittail = snp;
                        else
                    {
                        cbautoinittail->next = snp;
                        cbautoinittail = snp;
                    }
                }
                //      }
                if (!(typesp->value.classdata.cppflags &PF_HASCONS) && typesp
                    ->value.classdata.baseclass->vtabsp)
                {
                    ENODE *exp = makenode(en_nacon, typesp
                        ->value.classdata.baseclass->vtabsp, 0);
                    ENODE *exp1,  *exp2,  *exp3;
                    CLASSLIST *l = typesp->value.classdata.baseclass;
                    if (sym->absflag)
                        exp1 = makenode(en_absacon, sym, 0);
                    else if (sym->storage_class == sc_auto)
                        exp1 = makenode(en_autocon, sym, 0);
                    else
                        exp1 = makenode(en_nacon, sym, 0);
                    while (l)
                    {
                        if (l->flags &CL_VTAB)
                        {
                            exp3 = makenode(en_addstruc, exp1, makeintnode
                                (en_icon, l->offset));
                            exp3 = makenode(en_a_ref, exp3, 0);
                            exp2 = makenode(en_addstruc, exp, makeintnode
                                (en_icon, l->vtaboffs));
                            exp2 = makenode(en_assign, exp3, exp2);
                            if (snp)
                                snp->exp = makenode(en_void, exp2, snp->exp);
                            else
                            {
                                snp = xalloc(sizeof(SNODE));
                                snp->next = 0;
                                snp->stype = st_expr;
                                snp->exp = exp2;
                                if (cbautoinithead == 0)
                                    cbautoinithead = cbautoinittail = snp;
                                else
                                {
                                    cbautoinittail->next = snp;
                                    cbautoinittail = snp;
                                }
                            }
                        }
                        l = l->link;
                    }
                }
            }
            else
        #endif 
        tp = sym->tp;
    while (tp->type == bt_pointer && !(sym->tp->val_flag &VARARRAY))
        tp = tp->btp;
    if (tp->type == bt_pointer && (tp->val_flag &VARARRAY) && !infuncargs)
    {
        SNODE *snp = snp_line(TRUE);
        ENODE *ep;
        if (snp)
            if (cbautoinithead == 0)
                cbautoinithead = cbautoinittail = snp;
            else
        {
            cbautoinittail->next = snp;
            cbautoinittail = snp;
        }
        snp = xalloc(sizeof(SNODE));
        snp->next = 0;
        snp->stype = st_expr;
        snp->exp = createVarArray(sym, tp, FALSE, tp == sym->tp);
        snp->flags |= ST_VARARRAY ;
        if (cbautoinithead == 0)
            cbautoinithead = cbautoinittail = snp;
        else
        {
            cbautoinittail->next = snp;
            cbautoinittail = snp;
        }
        if (tp == sym->tp)
        {		
			if (!blockVarArraySP)
			{
				TYP *tp = maketype(bt_pointer, stdaddrsize);
				blockVarArraySP = dummyvar(stdaddrsize, &stdint, 0);
				deref(&blockVarArraySP, tp);
			}
        }

        if (tp == sym->tp && lastst == assign || prm_cplusplus && lastst ==
            openpa)
            gensymerror(ERR_NOINIT, sym->name);
    }
    if (lastst == assign || prm_cplusplus && lastst == openpa)
    {
        SNODE *snp = snp_line(TRUE);
        int s = lastst;
        if (goodcode & GF_UNREACH) {
            generror(ERR_UNREACHABLE,0,0);
            goodcode &= ~(GF_RETURN | GF_BREAK | GF_CONTINUE | GF_GOTO | GF_THROW | GF_UNREACH);
        }
        if (snp)
            if (cbautoinithead == 0)
                cbautoinithead = cbautoinittail = snp;
            else
        {
            cbautoinittail->next = snp;
            cbautoinittail = snp;
        }
        snp = xalloc(sizeof(SNODE));
        snp->next = 0;
        snp->stype = st_expr;

        if (autoasnop(&(snp->exp), sym, FALSE) == 0)
        {
            generror(ERR_EXPREXPECT, 0, 0);
        }
        else
        {
            if (cbautoinithead == 0)
                cbautoinithead = cbautoinittail = snp;
            else
            {
                cbautoinittail->next = snp;
                cbautoinittail = snp;
            }
        }
        if (s == openpa)
            needpunc(closepa, 0);
    }

}

//-------------------------------------------------------------------------

SNODE *compound(void)
/* 
 * Process the body of a compound block.  Declarations are already
 * handled by now.
 * 
 */
{
    SNODE *head,  *tail,  *temp;
    head = cbautoinithead;
    tail = cbautoinittail;
    cbautoinithead = 0;
    goodcode &= ~(GF_RETURN | GF_BREAK | GF_CONTINUE | GF_GOTO | GF_THROW);
    while (lastst != end && lastst != eof)
    {
        if ((goodcode &(GF_RETURN | GF_BREAK | GF_CONTINUE | GF_GOTO | GF_THROW | GF_UNREACH)) && !castbegin(lastst))
        if (lastst == id)
        {
            while (isspace(lastch))
                getch();
            if (lastch != ':')
                generror(ERR_UNREACHABLE, 0, 0);

        }
        else
            if (lastst != semicolon && lastst != kw_case && lastst != kw_default && lastst != kw_break)
                generror(ERR_UNREACHABLE, 0, 0);
        if (lastst != semicolon)
            if (lastst != kw_break && !castbegin(lastst))
                goodcode &= ~(GF_RETURN | GF_BREAK | GF_CONTINUE | GF_GOTO | GF_THROW | GF_UNREACH);
        temp = statement();
        if (head == 0)
            head = tail = temp;
        else
        {
            tail->next = temp;
        }
        if (head)
            while (tail->next != 0)
                tail = tail->next;

    }
    if (prm_ansi && lastlastst != semicolon && lastlastst != end)
        generror(ERR_PUNCT, semicolon, 0);
    funcendstmt = snp_line(TRUE);
    if (lastst == eof)
        generror(ERR_PUNCT, end, 0);
    else
        getsym();

    return head;
}

//-------------------------------------------------------------------------

SNODE *labelstmt(int fetchnext)
/* 
 *      labelstmt processes a label that appears before a 
 *      statement as a seperate statement. 
 */
{
    SNODE *snp;
    SYM *sp;
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    snp->stype = st_label;
    goodcode &= ~GF_UNREACH;
    if ((sp = search(lastid, &labelsyms)) == 0)
    {
        sp = makesym(sc_label);
        sp->name = litlate(lastid);
        sp->tp = xalloc(sizeof(TYP));
        sp->tp->type = bt_unsigned;
        sp->tp->uflags = 0;
        sp->tp->bits = sp->tp->startbit =  - 1;
        sp->value.i = nextlabel++;
        insert(sp, &labelsyms);
    }
    else
    {
        if (sp->storage_class != sc_ulabel)
            gensymerror(ERR_DUPLABEL, sp->name);
        else
            sp->storage_class = sc_label;
    }
    getsym(); /* get past id */
    needpunc(colon, 0);
    if (sp->storage_class == sc_label)
    {
        snp->label = (SNODE*)sp->value.i;
        snp->next = 0;
        if (lastst != end && fetchnext && lastst != kw_case && lastst !=
            kw_default)
            snp->next = statement();
        return snp;
    }
    return 0;
}

//-------------------------------------------------------------------------

SNODE *gotostmt(void)
/* 
 *      gotostmt processes the goto statement and puts undefined 
 *      labels into the symbol table. 
 */
{
    SNODE *snp;
    SYM *sp;
    getsym();
    if (lastst != id)
    {
        generror(ERR_IDEXPECT, 0, 0);
        return 0;
    }
    snp = xalloc(sizeof(SNODE));
    snp->next = 0;
    if ((sp = search(lastid, &labelsyms)) == 0)
    {
        sp = makesym(sc_ulabel);
        sp->name = litlate(lastid);
        sp->value.i = nextlabel++;
        sp->tp = xalloc(sizeof(TYP));
        sp->tp->bits = sp->tp->startbit =  - 1;
        sp->tp->type = bt_unsigned;
        insert(sp, &labelsyms);
    }
    sp->tp->uflags = UF_USED;
    getsym(); /* get past label name */
    gotoCount++ ;
    if (lastst != eof)
        needpunc(semicolon, 0);
    if (sp->storage_class != sc_label && sp->storage_class != sc_ulabel)
        gensymerror(ERR_LABEL, sp->name);
    else
    {
        snp->stype = st_goto;
        snp->label = (SNODE*)sp->value.i;
        snp->next = 0;
        return snp;
    }
    return 0;
}

//-------------------------------------------------------------------------

SNODE *catchstmt(TRYBLOCK *a)
{
    int pushed = FALSE;
    SNODE *sn = snp_line(TRUE),  *sn1,  *sn2;
    CATCHBLOCK *c,  **s,  *p;
    global_flag++;
    c = xalloc(sizeof(CATCHBLOCK));
    global_flag--;
    c->label = nextlabel++;
    getsym();
    if (needpunc(openpa, 0))
    {
        if (lastst != ellipse)
        {
            TYP *tp;
            if (gotellipse)
            {
                generror(ERR_ELLIPSEHANDLERLAST, 0, 0);
                gotellipse = FALSE;
            }
            incatchclause = TRUE;
            declid[0] = 0;
            typequal = 0;
            decl(0, 0); /* do cast declaration */
            headptr = &head;
            decl1(sc_auto, 0);
            incatchclause = FALSE;
            tp = head;
            if (!tp || defaulttype && tp->type == bt_int)
            // || !isstructured(tp))
                generror(ERR_TYPENAMEEXP, 0, 0);
            else
            {
                global_flag++;
                c->xtcatch = copytype(tp, 0);
                global_flag--;
            }
            global_flag++;
            c->dummy = dummyvar(tp->size, tp, declid[0] ? declid : 0);
            global_flag--;
            c->dummy->v.sp->iscatchvar = TRUE;
            pushed = TRUE;
            p = a->catchit;
            while (p)
            {
                char buf[256];
                if (p->xtcatch == tp)
                {
                    typenum2(buf, tp);
                    gensymerror(ERR_DUPHANDLER, buf);
                    break;
                }
                p = p->next;
            }


        }
        else
        {
            getsym();
            gotellipse = TRUE;
        }
        if (needpunc(closepa, 0))
        {
            if (lastst != begin)
                needpunc(begin, 0);
            else
            {
                ENODE *ep1,  *ep2 = makenode(en_napccon, (void*)
                    catch_cleanup_func, 0);
                int blocknum = dbgblocknum;
                catch_cleanup_func->mainsym->extflag = TRUE;
                sn1 = xalloc(sizeof(SNODE));
                sn1->stype = st_label;
                sn1->label = (SNODE*)c->label;
                if (sn)
                    sn->next = sn1;
                else
                    sn = sn1;
                sn1->next = xalloc(sizeof(SNODE));
                sn1->next->stype = st_tryblock;
                sn1->next->label = (void*)1; // beginning of catch block
                sn1 = sn1->next;
                getsym();
                sn2 = xalloc(sizeof(SNODE));
                sn2->exp = compoundblock();
                sn2->stype = st_block;
                sn2->label = blocknum + 1;
                sn1 = sn1->next = sn2;
                ep1 = makenode(en_void, 0, 0);
                ep2 = makenode(en_void, ep2, ep2);
                ep1 = makenode(en_void, ep2, ep1);
                ep1 = makenode(en_fcall, makeintnode(en_icon, 0), ep1);
                sn1->next = xalloc(sizeof(SNODE));
                sn1->next->stype = st_expr;
                sn1->next->exp = ep1;
                sn1 = sn1->next;
                sn1->next = xalloc(sizeof(SNODE));
                sn1->next->stype = st_goto;
                sn1->next->label = a->catchendlab;
                s = &a->catchit;
                while (*s)
                    s = &(*s)->next;
                *s = c;
            }
        }
    }
    if (pushed)
        lsyms.head->name = "$$$dummy";
    return sn;
}

//-------------------------------------------------------------------------

SNODE *trystmt(void)
{
    SNODE *sn,  *sn1;
    int oldgotellipse = gotellipse;
    gotellipse = FALSE;
    getsym();
    global_flag++;
    sn = xalloc(sizeof(SNODE));
    sn->stype = st_label;
    sn->label = (SNODE*)nextlabel++;
    if (!prm_xcept)
        generror(ERR_NOXCEPT, 0, 0);
    if (lastst != begin)
        needpunc(begin, 0);
    else
    {
        TRYBLOCK *a = xalloc(sizeof(TRYBLOCK));
        int blocknum = dbgblocknum;
        a->startlab = nextlabel++;
        a->endlab = nextlabel++;
        a->catchendlab = nextlabel++;
        a->next = try_block_list;
        try_block_list = a;
        sn->label = (SNODE*)a->startlab;
        sn1 = sn;
        sn1->next = xalloc(sizeof(SNODE));
        sn1->next->stype = st_tryblock;
        sn1->next->label = (void*)0; // start of try block
        sn1 = sn1->next;
        getsym();
        sn1->next = xalloc(sizeof(SNODE));
        sn1->next->exp = compoundblock();
        sn1->next->stype = st_block;
        sn1->next->label = blocknum + 1;
        sn1 = sn1->next;
        sn1->next = xalloc(sizeof(SNODE));
        sn1->next->stype = st_tryblock;
        sn1->next->label = (void*)2; // end of try block
        sn1 = sn1->next;
        sn1->next = xalloc(sizeof(SNODE));
        sn1->next->stype = st_goto;
        sn1->next->label = (SNODE*)a->catchendlab;
        sn1 = sn1->next;
        sn1->next = xalloc(sizeof(SNODE));
        sn1->next->stype = st_label;
        sn1->next->label = (SNODE*)a->endlab;
        sn1 = sn1->next;
        if (lastst != kw_catch)
        {
            generror(ERR_CATCHEXP, 0, 0);
        }
        else
        {
            global_flag--;
            while (lastst == kw_catch)
            {
                SYM *sn2 = catchstmt(a);
                sn1->next = sn2;
                while (sn1->next)
                    sn1 = sn1->next;
            }
            global_flag++;
        }
        sn1->next = xalloc(sizeof(SNODE));
        sn1->next->stype = st_label;
        sn1->next->label = (SNODE*)a->catchendlab;
        sn1 = sn1->next;

    }
    gotellipse = oldgotellipse;
    global_flag--;
    return sn;
}

//-------------------------------------------------------------------------

SNODE *asm_statement(int shortfin);
SNODE *statement(void)
/* 
 *      statement figures out which of the statement processors 
 *      should be called and transfers control to the proper 
 *      routine. 
 */
{
    SNODE *snp = 0,  *snp2,  **psnp;
    SNODE *snp3 = snp_line(TRUE);
    int done = FALSE;
    //        while (!done) {
    //            done = TRUE ;
    switch (lastst)
    {
        case kw_using:
            using_keyword();
            break;
            #ifndef ICODE
            case kw_asm:
                asmline = TRUE;
                getsym();
                if (lastst == begin)
                {
                    getsym();
                    snp = snp3;
                    if (snp)
					{
                        psnp = &snp->next;
					}
                    else
                        psnp = &snp;
                    *psnp = 0;
                    while (lastst != end && lastst != eof)
                    {
                        *psnp = asm_statement(TRUE);
                        while (*psnp)
                            psnp = &(*psnp)->next;
                        *psnp = snp_line(TRUE);
                        while (*psnp)
                            psnp = &(*psnp)->next;
                    }
                    asmline = FALSE;
                    if (lastst == end)
                        getsym();
                    return snp;
                }
                snp = asm_statement(FALSE);
	            asmline = FALSE;
                break;
            #endif 
        case semicolon:
            getsym();
            snp = 0;
            break;
        case begin:
            {
                int oldpragma = stdpragmas;
                int blocknum = dbgblocknum;
                getsym();
                if (((goodcode & GF_UNREACH) && lastst != semicolon && lastst != kw_case && lastst != kw_default) && !castbegin(lastst))
                    generror(ERR_UNREACHABLE, 0, 0);
                snp2 = compoundblock();
                snp = xalloc(sizeof(SNODE));
                snp->next = 0;
                snp->exp = snp2;
                snp->stype = st_block;
                snp->label = blocknum + 1;
                stdpragmas = oldpragma;
                break;
            }
        case kw_if:
            snp = ifstmt();
            break;
        case kw_while:
            snp = whilestmt();
            snp->lst = snp3;
            snp3 = 0;
            break;
        case kw_for:
            snp = forstmt();
            snp->lst = snp3;
            snp3 = 0;
            break;
        case kw_return:
            snp = retstmt();
            goodcode |= GF_RETURN;
            break;
        case kw_break:
            goodcode |= GF_BREAK;
            if (!(goodcode & GF_RETURN))
                switchbreak = 1;
            snp = breakstmt();
            break;
        case kw_goto:
            goodcode |= GF_GOTO;
            snp = gotostmt();
            break;
        case kw_continue:
            goodcode |= GF_CONTINUE;
            snp = contstmt();
            break;
        case kw_do:
            snp = dostmt();
            break;
        case kw_switch:
            snp = switchstmt();
            break;
        case kw_try:
            snp = trystmt();
            break;
        case kw_else:
            generror(ERR_ELSE, 0, 0);
            getsym();
            break;
        case kw_case:
        case kw_default:
            if (!inswitch) {
                generror(ERR_CASENOSWITCH,0,0);
            }
            snp = casestmt();
            break;
        case kw__genword:
            snp = _genwordstmt();
            break;
        doid: case id:
        default:
                if (castbegin(lastst) || lastst == kw_typedef)
                {
                    if (prm_cplusplus || prm_c99)
                    {
                        cbautoinithead = cbautoinittail = 0;
                        if (blockdecl())
                        {
                            snp = cbautoinithead;
                            goto doexpr;
                        }
                        snp = cbautoinithead;
                    }
                    else
                    {
                        generror(ERR_NODECLARE, 0, skm_semi);
                        snp = 0;
                    }
                }
                else
                {
                doexpr: if (lastst == id)
                {
                    while (isspace(lastch))
                        getch();
                    if (lastch == ':' &&  *lptr != ':')
                        snp = labelstmt(TRUE);
                    else
                        snp = exprstmt();
                }
                else
                    snp = exprstmt();
            }
            break;
    }
    //        }
    if (snp != 0)
    {
        if (snp3)
        {
            snp3->next = snp;
            snp = snp3;
        }
    }
    return snp;
}

/* Handling for special C++ situations */
SNODE *cppblockedstatement(int breakdest)
{
    SNODE *snp;
	ENODE *oldblockva;
    TABLE oldoldlsym, oldoldltag;
    LIST *nsusing,  *nstusing;
    int oldpragma = stdpragmas;
    long oldlcauto;
    ENODE *obrd = block_rundown;
    LASTDEST rundown;
    int nested = FALSE;
    rundown.last = lastDestPointer;
    rundown.block = block_rundown;
	oldblockva = blockVarArraySP;
	blockVarArraySP = 0;
    if (breakdest)
        rundown.chain = 0;
    else
        rundown.chain = lastDestPointer;
    lastDestPointer = &rundown;
    block_rundown = 0;
        if (prm_cplusplus || prm_c99)
        {
            oldoldlsym = oldlsym;
            oldlsym = lsyms;
            oldoldltag = oldltag;
            oldltag = ltags;
            nsusing = local_using_list;
            nstusing = local_tag_using_list;
            if (lastst != begin)
            {
                block_nesting++;
                nested = TRUE;
            }
        }
    snp = statement();
        if (prm_cplusplus || prm_c99)
        {
            if (nested)
                block_nesting--;
            addblocklist(lsyms.head, oldlsym.head);
            addrundown(snp);
            check_funcused(&oldlsym, &lsyms);
            gather_labels(&oldlsym, &lsyms);
            cseg();
            lsyms = oldlsym;
            oldlsym.head = oldoldlsym.head;
            ltags = oldltag;
            oldltag.head = oldoldltag.head;
            local_using_list = nsusing;
            local_tag_using_list = nstusing;
        }
    block_rundown = rundown.block;
    lastDestPointer = rundown.last;
	if (blockVarArraySP)
	{
		SNODE *snp2 = xalloc(sizeof(SNODE)), *snp3;
        snp2->stype = st_expr;
        snp2->exp = makenode(en_savestack, blockVarArraySP, 0);
		snp2->next = snp;
		snp3 = snp = snp2;
		while (snp3->next)
			snp3 = snp3->next;
		snp2 = xalloc(sizeof(SNODE)), *snp3;
        snp2->stype = st_expr;
        snp2->exp = makenode(en_loadstack, blockVarArraySP, 0);
		snp3->next = snp2 ;
	}
	blockVarArraySP = oldblockva;
    return (snp);
}

