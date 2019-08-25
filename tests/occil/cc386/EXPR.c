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
 * Evaluate expressions very recursive descent.  MAke sure your STACK
 * is large enough to parse the expressions you want to parse.  a 4K stack
 * will handle all but the very extreme cases
 */
#include <stdio.h>
#include <string.h>
#include <float.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "lists.h"
#include "rtti.h"
#include "diag.h"

extern int setjmp_used;
extern int inprototype;
extern int prm_c99;
extern int cantnewline;
extern int prm_ansi ;
extern int defaulttype;
extern int stdpragmas;
extern int indefaultinit;
extern SNODE *cbautoinithead,  *cbautoinittail;
extern int lineno;
extern int prm_bss;
extern ENODE *block_rundown;
extern int lastch;
extern TYP *head,  **headptr;
extern LIST *instantiated_inlines;
extern int stdretblocksize, stdintsize, stddefalignment;
extern int stdshortsize, stdlongsize, stdlonglongsize, stdfloatsize, stddoublesize, stdldoublesize;

extern int block_nesting;
extern int global_flag;
extern enum e_sym lastst;
extern char lastid[], laststr[];
extern TABLE *gsyms, lsyms;
extern LLONG_TYPE ival;
extern FPF rval;
extern int skm_declcomma[], skm_declclosepa[];
extern TABLE *tagtable;
extern char declid[100];
extern int goodcode;
extern int prm_cplusplus;
extern int regdsize, regasize, regfsize, stackadd, stackmod;
extern int stdaddrsize;
extern long nextlabel;
extern SYM *declclass;
extern int prm_cmangle;
extern SYM *currentfunc;
extern int stdmemberptrsize;
extern int laststrlen;
extern SYM *typequal;
extern char *cpp_funcname_tab[];
extern int nonslookup;
extern int bssbytes, databytes, constbytes;
extern SYM *dynamic_cast_func;
extern int prm_xcept;
extern SYM *typeinfo_class;
extern SYM *get_typeinfo_func;
extern int skm_declend[];
extern SYM *throw_func;
extern SYM *rethrow_func;
extern int conscount;
extern int used_alloca;
SYM *parm_namespaces[20][100];
int parm_ns_counts[20];
int parm_ns_level;
int parsing_params;

ENODE *expr_runup[50];
ENODE *expr_rundown[50];

typedef TYP * (*exprFunc)(ENODE **);

int expr_updowncount = 0;
/* Default types */
extern TYP stdint, stdlongdouble, stduns, stdstring, stdwstring;
extern TYP stdfloat, stdimaginary, stddouble, stdrimaginary, stdlrimaginary;
extern TYP stdlonglong,stdunsignedlonglong, stdunsigned;
extern TYP stdcomplex, stdrcomplex, stdlrcomplex;
TYP stdvoid = 
{
    bt_matchall, 0, 0,  - 1,  - 1, 4
};
TYP stdmatch = 
{
    bt_matchall, 0, UF_DEFINED | UF_USED,  - 1,  - 1, 4, 0, &stdvoid
};
TYP stdchar = 
{
    bt_char, 0, 0,  - 1,  - 1, 1
};
TYP stdbool = 
{
    bt_char, 0, 0,  - 1,  - 1, 1
};
TYP stdfunc = 
{
    bt_func, 1, UF_DEFINED | UF_USED,  - 1,  - 1, 0, 0, &stdint
};
TYP stdvoidfunc = 
{
    bt_func, 1, UF_DEFINED | UF_USED,  - 1,  - 1, 0, 0, &stdvoid, 0, 
    {
         - 1, 0
    }
};

int skm_closepa[] = 
{
    closepa, comma, semicolon, end, 0
};
int skm_closebr[] = 
{
    closebr, comma, openbr, semicolon, end, 0
};
SYM undef;
SYM *thissp;
int nodeclfunc;

static int dummyram;
static int dumpos;
static ENODE *thisx;
static SYM *lastsym;
static char regname[] = "processor reg";
static char *nm = 0;
static TABLE defalt_table;
TYP *asntyp = 0,  *andtyp = 0;
static int global_deref = 0;

ENODE *thisenode;
int intemplateargs;

void exprini(void)
{
    ENODE *newnode;
    SYM *sp;
    intemplateargs = 0;
    andtyp = asntyp = 0;
    dumpos = 0;
    undef.value.i = 0;
    undef.name = "UNDEFINED";
    undef.mainsym = &undef;
    global_deref = 0;
        thissp = sp = makesym(sc_auto);
        sp->tp = maketype(bt_int, stdintsize); // dummy so backend is happy
        sp->name = "**THIS**";
        sp->value.classdata.defalt = 0;
        sp->extflag = FALSE;
        sp->absflag = FALSE;
        sp->intflag = FALSE;
        sp->faultflag = FALSE;
        sp->pascaldefn = FALSE;
        sp->isstdcall = FALSE;
        sp->init = 0;
        sp->indecltable = 0;
        sp->funcparm = 1;
        sp->inreg = 0;
        sp->staticlabel = FALSE;
        sp->value.i = stdretblocksize;
        thisenode = makenode(en_autocon, sp, 0);
        dummyram = 0;
        nodeclfunc = 0;
}

//-------------------------------------------------------------------------

TYP *basictype(TYP *t)
{
    while (t->type == bt_cond)
        t = t->btp;
    return t;
}

//-------------------------------------------------------------------------

int retvalsize(TYP *t)
{
    int rv = 0;
    if (t->type == bt_ref)
    {
        t = t->btp;
        if (!isstructured(t))
            rv = 100000;
        // reference return
    }
    switch (t->type)
    {
        case bt_void:
            return 0;
        case bt_farpointer:
            return rv + BESZ_FARPTR;
        case bt_bool:
			return BESZ_BOOL;
        case bt_char:
        case bt_short:
        case bt_int:
        case bt_long:
		case bt_longlong:
			if (t->size == 8)
				return rv - BESZ_QWORD;
			else
	            return rv +  - (t->size);
        case bt_unsignedchar:
        case bt_unsignedshort:
        case bt_unsigned:
        case bt_unsignedlong:
        case bt_unsignedlonglong:
			if (t->size == 8)
				return rv + BESZ_QWORD;
			else
	            return rv +  (t->size);
        case bt_float:
            return rv + BESZ_FLOAT;
        case bt_double:
            return rv + BESZ_DOUBLE;
        case bt_longdouble:
            return rv + BESZ_LDOUBLE;
        case bt_fimaginary:
            return rv + BESZ_IFLOAT;
        case bt_rimaginary:
            return rv + BESZ_IDOUBLE;
        case bt_lrimaginary:
            return rv + BESZ_ILDOUBLE;
        case bt_fcomplex:
            return rv + BESZ_CFLOAT;
        case bt_rcomplex:
            return rv + BESZ_CDOUBLE;
        case bt_lrcomplex:
            return rv + BESZ_CLDOUBLE;
        default:
            return rv - stdintsize;
    }
}

//-------------------------------------------------------------------------

ENODE *makenode(enum e_node nt, void *v1, void *v2)
/*
 *      build an expression node with a node type of nt and values
 *      v1 and v2.
 */
{
    ENODE *ep;
    ep = xalloc(sizeof(ENODE));
    ep->nodetype = nt;
    ep->cflags = 0;
    ep->v.p[0] = v1;
    ep->v.p[1] = v2;
    return ep;
} 
ENODE *makeintnode(enum e_node nt, LLONG_TYPE val)
{
    ENODE *ep;
    ep = xalloc(sizeof(ENODE));
    ep->nodetype = (char)nt;
    ep->cflags = 0;
    ep->v.i = val;
    return ep;
}
TYP *deref(ENODE **node, TYP *tp)
/*
 *      build the proper dereference operation for a node using the
 *      type pointer tp.
 */
{
    ENODE *onode =  *node;
    if ((*node)->nodetype == en_placeholder)
        return tp;
    switch (tp->type)
    {
        case bt_double:
            *node = makenode(en_doubleref,  *node, 0);
            break;
        case bt_longdouble:
            *node = makenode(en_longdoubleref,  *node, 0);
            break;
        case bt_float:
            *node = makenode(en_floatref,  *node, 0);
            break;
        case bt_fimaginary:
            *node = makenode(en_fimaginaryref,  *node, 0);
            break;
        case bt_rimaginary:
            *node = makenode(en_rimaginaryref,  *node, 0);
            break;
        case bt_lrimaginary:
            *node = makenode(en_lrimaginaryref,  *node, 0);
            break;
        case bt_fcomplex:
            *node = makenode(en_fcomplexref,  *node, 0);
            break;
        case bt_rcomplex:
            *node = makenode(en_rcomplexref,  *node, 0);
            break;
        case bt_lrcomplex:
            *node = makenode(en_lrcomplexref,  *node, 0);
            break;
        case bt_unsignedchar:
            *node = makenode(en_ub_ref,  *node, 0);
            break;
        case bt_unsignedshort:
            *node = makenode(en_uw_ref,  *node, 0);
            break;
        case bt_char:
            *node = makenode(en_b_ref,  *node, 0);
            break;
        case bt_bool:
            *node = makenode(en_bool_ref,  *node, 0);
            break;
        case bt_short:
            *node = makenode(en_w_ref,  *node, 0);
            break;
        case bt_unsigned:
            *node = makenode(en_ui_ref,  *node, 0);
			break ;
        case bt_unsignedlong:
            *node = makenode(en_ul_ref,  *node, 0);
            break;
        case bt_farpointer:
            *node = makenode(en_fp_ref,  *node, 0);
            break;
        case bt_ref:
        case bt_pointer:
            *node = makenode(en_a_ref,  *node, 0);
            break;
        case bt_int:
        case bt_enum:
            *node = makenode(en_i_ref,  *node, 0);
            break;
        case bt_long:
        case bt_matchall:
            *node = makenode(en_l_ref,  *node, 0);
            break;
        case bt_longlong:
        case bt_memberptr:
            *node = makenode(en_ll_ref,  *node, 0);
            break;
        case bt_unsignedlonglong:
            *node = makenode(en_ull_ref,  *node, 0);
            break;
        case bt_struct:
        case bt_union:
        case bt_class:
            break;
        default:
            generror(ERR_DEREF, 0, 0);
            break;
    }
    (*node)->cflags = onode->cflags;
    return tp;
}

//-------------------------------------------------------------------------

int checkstructconst(TYP *tp)
{
    SYM *sp = tp->lst.head;
    while (sp)
    {
        if (isstructured(sp->tp))
        {
            if (checkstructconst(sp->tp))
                return TRUE ;
        }
        if (sp->tp->type != bt_func && sp->tp->type != bt_defunc)
        {
            if (sp->tp->cflags &DF_CONST)
            {
                generror(ERR_MODCONS, 0, 0);
                return TRUE;
            }
        }
        sp = sp->next;
    }
    return FALSE;
}

//-------------------------------------------------------------------------

ENODE *sbll(SYM *sp_in, ENODE *en, ENODE *ep1)
{
    ENODE *epx = 0;
    if (prm_cplusplus)
    {
        SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &sp_in->tp->lst);
        if (sp1)
        {
            SYM *sp;
            #ifdef XXXXX
                SYM *sm = makesym(0);
                TYP tp;
                memset(&tp, 0, sizeof(tp));
                tp.type = bt_func;
                tp.lst.head = tp.lst.tail = sp_in;
                tp.sp = sm;
                sm->tp = &tp;
            #endif 
            sp = funcovermatch(sp1, sp_in->tp, makenode(en_void, en, 0), FALSE,
                FALSE);
            if (sp)
            {
                ENODE *pnode = makenode(en_napccon, sp, 0);
                ENODE *qn,  *rnode,  *tcnode = makenode(en_substack,
                    makeintnode(en_icon, sp_in->tp->size), 0);
                sp->mainsym->extflag = TRUE;
                qn = makenode(en_void, en, 0);
                qn = makenode(en_void, qn, 0);
                rnode = makenode(en_void, pnode, pnode);
                pnode = makenode(en_void, rnode, qn);
                rnode = makeintnode(en_icon, sp->tp->btp->size);
                if (sp->pascaldefn)
                    pnode = makenode(en_pfcall, rnode, pnode);
                else
                    if (sp->isstdcall)
                        pnode = makenode(en_sfcall, rnode, pnode);
                    else
                        pnode = makenode(en_fcall, rnode, pnode);
                pnode = makenode(en_thiscall, tcnode, pnode);
                pnode = doinline(pnode);
                epx = makenode(en_void, pnode, ep1);
            }
        }
    }
    if (!epx)
    {
        //checkstructconst(sp_in->tp);
        epx = makenode(en_stackblock, en, ep1);
        en->size = sp_in->tp->size;
    }
    return epx;
}

//-------------------------------------------------------------------------

ENODE *cbll(ENODE *ep1, ENODE *ep2, TYP *tp)
{
    if (ep2->nodetype == en_pfcallb)
        ep1 = makenode(en_pcallblock, ep1, ep2);
    else
        if (ep2->nodetype == en_sfcallb)
            ep1 = makenode(en_scallblock, ep1, ep2);
        else
            ep1 = makenode(en_callblock, ep1, ep2);

    return ep1;
}

//-------------------------------------------------------------------------

ENODE *make_callblock(ENODE *ep1, ENODE *ep2, TYP *tp, int size)
{
        ENODE *ep3,  *ep4;
        ENODE *rnode;
        int thiscall = 0;
        if (ep2->nodetype == en_thiscall)
        {
            ep3 = ep2->v.p[0];
            ep2 = ep2->v.p[1];
            thiscall = 1;
        }
    if (ep2->nodetype == en_fcallb || ep2->nodetype == en_pfcallb || ep2
        ->nodetype == en_sfcallb)
    {
        if (!ep1)
            ep1 = (ENODE*)dummyvar(size, tp, 0);

        ep4 = ep1 = cbll(ep1, ep2, tp);
        if (thiscall)
            ep1 = makenode(en_thiscall, ep3, ep1);
        if (prm_cplusplus)
        {
            TYP tp3;
            ep1 = makenode(en_conslabel, ep1, (void*)copynode(ep4->v.p[0]));
            tp3.lst.head =  - 1;
            tp3.lst.tail =  - 1;
            tp3.type = bt_func;
            rnode = do_destructor(ep4->v.p[0]->v.sp, &tp3, tp, 0, 1, 0, TRUE);
            if (!rnode)
                rnode = makenode(en_destlabel, makeintnode(en_icon, 0), (void*)
                    copynode(ep4->v.p[0]));
            if (block_rundown)
                block_rundown = makenode(en_void, rnode, block_rundown);
            else
                block_rundown = rnode;
        }
    }
        else
        {
            if (thiscall || !ep1)
                ep1 = ep2;
            ep4 = ep1;
            if (thiscall)
                ep1 = makenode(en_thiscall, ep3, ep1);
            if (prm_cplusplus)
                ep1 = makenode(en_conslabel, ep1, (void*)copynode(ep4->v.p[0]));
            rnode = makenode(en_destlabel, makeintnode(en_icon, 0), (void*)
                copynode(ep4->v.p[0]));
            if (block_rundown)
                block_rundown = makenode(en_void, rnode, block_rundown);
            else
                block_rundown = rnode;
        }
    return ep1;
}

//-------------------------------------------------------------------------

ENODE *immed_callblock(ENODE *ep1, ENODE *ep2, TYP *tp, int size)
{
    ENODE *obd = block_rundown,  *rv;
    block_rundown = 0;
    rv = make_callblock(ep1, ep2, tp, size);
	if (block_rundown)
	    rv = makenode(en_void, rv, block_rundown);
    block_rundown = obd;
    return rv;
}

//-------------------------------------------------------------------------

ENODE *repcallb(ENODE *ep2)
/*
 * repcallb replaces callblocks that haven't been resolved to a return address
 *	by allocating a return struct
 */
{
    if (ep2 == 0)
        return ep2;
    switch (ep2->nodetype)
    {
        case en_structret:
        case en_placeholder:
        case en_regref:
            break;
        case en_conslabel:
        case en_destlabel:
        case en_movebyref:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_fcomplexcon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_fimaginarycon:
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_icon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_llcon:
        case en_llucon:
        case en_boolcon:
        case en_ccon:
        case en_cucon:
        case en_nacon:
        case en_napccon:
        case en_absacon:
        case en_autocon:
        case en_autoreg:
        case en_labcon:
        case en_nalabcon:
            break;
        case en_fp_ref:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_ub_ref:
        case en_uw_ref:
        case en_b_ref:
        case en_bool_ref:
        case en_w_ref:
        case en_l_ref:
        case en_ul_ref:
		case en_i_ref:
		case en_ui_ref:
		case en_a_ref: case en_ua_ref:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_ll_ref:
        case en_ull_ref:
        case en_cl_reg:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_uminus:
        case en_bits:
        case en_asuminus:
        case en_ascompl:
        case en_not:
        case en_compl:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_cfc:
        case en_cfi:
        case en_crc:
        case en_cri:
        case en_clrc:
        case en_clri:
        case en_cb:
        case en_cub:
        case en_cbool:
        case en_cw:
        case en_cuw:
        case en_cl:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cll:
        case en_cull:
        case en_csp:
        case en_cf:
        case en_cd:
        case en_cp:
        case en_cld:
        case en_cfp:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_pfcallb:
        case en_sfcallb:
        case en_fcallb:
            {
                SYM *sp = ep2->v.p[1]->v.p[0]->v.p[1]->v.sp;
                ep2 = make_callblock(0, ep2, sp->tp->btp, sp->tp->btp->size);
                ep2 = repcallb(ep2);
            }
            break;
        case en_pfcall:
        case en_sfcall:
        case en_fcall:
        case en_stackblock:
        case en_assign:
        case en_intcall:
        case en_refassign:
        en_lassign: case en_add:
        case en_sub:
        case en_addstruc:
        en_addcast: case en_umul:
        case en_udiv:
        case en_umod:
        case en_mul:
        case en_div:
        case en_mod:
        case en_lsh:
        case en_asalsh:
        case en_asarsh:
        case en_alsh:
        case en_arsh:
        case en_arshd:
        case en_asarshd:
        case en_rsh:
        case en_and:
        case en_or:
        case en_xor:
        case en_land:
        case en_lor:
        case en_eq:
        case en_ne:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_gt:
        case en_ge:
        case en_cond:
        case en_void:
        case en_voidnz:
        case en_dvoid:
        case en_asadd:
        case en_assub:
        case en_asmul:
        case en_asdiv:
        case en_asor:
        case en_asand:
        case en_asxor:
        case en_asmod:
        case en_aslsh:
        case en_asumod:
        case en_asudiv:
        case en_asumul:
        case en_pmul:
		case en_arrayindex:
        case en_asrsh:
        case en_pdiv:
        case en_moveblock:
        case en_repcons:
        case en_ainc:
        case en_adec:
		case en_array:
            ep2->v.p[1] = repcallb(ep2->v.p[1]);
        case en_trapcall:
        case en_substack:
        case en_clearblock:
		case en_loadstack:
		case en_savestack:
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        case en_thiscall:
            switch (ep2->v.p[1]->nodetype)
            {
            case en_pfcallb:
            case en_sfcallb:
            case en_fcallb:
                {
                    SYM *sp = ep2->v.p[1]->v.p[1]->v.p[0]->v.p[1]->v.sp;
                    ep2 = make_callblock(0, ep2, sp->tp->btp, sp->tp->btp->size)
                        ;
                    ep2 = repcallb(ep2);
                }
                break;
            default:
                ep2->v.p[1] = repcallb(ep2->v.p[1]);
                ep2->v.p[0] = repcallb(ep2->v.p[0]);
                break;
            }
            break;
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
            // skip the struct function placeholder
            ep2->v.p[1]->v.p[1] = repcallb(ep2->v.p[1]->v.p[1]);
            ep2->v.p[1]->v.p[0] = repcallb(ep2->v.p[1]->v.p[0]);
            ep2->v.p[0] = repcallb(ep2->v.p[0]);
            break;
        default:
            DIAG("repcallb: unknown node type");
    }
    return ep2;
}

//-------------------------------------------------------------------------



ENODE *dummyvar(int size, TYP *type, char *name)
{
    char nm[20];
    ENODE *newnode;
    SYM *sp = makesym(sc_auto);
    if (name)
        sp->name = litlate(name);
    else
    {
        sprintf(nm, "**DUMMY%d", dumpos++);
        sp->name = litlate(nm);
    }
        sp->value.classdata.defalt = 0;
    type->uflags |= UF_USED;
    sp->tp = copytype(type, 0);
    sp->tp->cflags = 0;
    sp->extflag = FALSE;
    sp->absflag = FALSE;
    sp->intflag = FALSE;
    sp->faultflag = FALSE;
    sp->pascaldefn = FALSE;
    sp->isstdcall = FALSE;
    sp->init = 0;
    sp->indecltable = 0;
    sp->funcparm = 0;
    sp->inreg = 0;
    sp->mainsym = sp;
    if (currentfunc || indefaultinit)
    {
        sp->staticlabel = FALSE;
        sp->value.i = block_nesting;
        sp->storage_class = sc_auto;
        if (!indefaultinit)
            insert(sp, &lsyms);
        newnode = makenode(en_autocon, sp, 0);
    }
    else
    {
        /* have to do this for C++ constructor matching */
        sp->staticlabel = TRUE;
        sp->value.i = nextlabel++;
        sp->storage_class = sc_static;
        insert_decl_sp(sp);
        insertSpace(sp, prm_bss ? &bssbytes: &databytes);
        newnode = makenode(en_nalabcon, sp, 0);
    }
    return newnode;
}

//-------------------------------------------------------------------------

int isintconst(int type)
{
    switch (type)
    {
        case en_icon:
        case en_lcon:
        case en_lucon:
        case en_iucon:
        case en_boolcon:
        case en_ccon:
        case en_cucon:
        case en_llcon:

        case en_llucon:
            return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------

int isfloatconst(int type)
{
    switch (type)
    {
        case en_rcon:
        case en_fcon:
        case en_lrcon:
            return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------

int isimaginaryconst(int type)
{
    switch (type)
    {
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
            return 1;
    }
    return 0;
}
int iscomplexconst(int type)
{
    switch (type)
    {
        case en_fcomplexcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
            return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------

int matchreflvalue(ENODE **node, TYP *tp1, TYP *tp2)
{
    if (node && ! *node || !exactype(tp1->btp, tp2,FALSE))
    {
        generror(ERR_REFLVALUE, 0, 0);
        return 0;
    }
    if (node)
        *node = (*node)->v.p[0];
    return 1;
}

//-------------------------------------------------------------------------

ENODE *declRef(SYM *sp_in, TYP *tp1, TYP *tp2, ENODE *ep1, ENODE *ep2, int sc)
{
    char buf[256];
    int voidfunc = FALSE;
    SYM *sp = 0,  *spd = 0,  *sm,  *spx;
    TYP tpx, tpy;
    ENODE *pnode,  *rnode,  *tcnode = 0,  *epx;
    ENODE *location;
    ENODE *node = makenode(en_void, ep2, 0);
    {
        SYM *sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &sp_in->tp->btp
            ->lst);
        if (sp1)
        {
            sm = makesym(0);
            tpy.type = bt_func;
            tpy.lst.head = tpy.lst.tail = sm;
            sm->tp = tp2;
            sp = funcovermatch(sp1, &tpy, node, FALSE, FALSE);
        }
        sp1 = search(cpp_funcname_tab[CI_DESTRUCTOR], &sp_in->tp->btp->lst);
        if (sp1)
        {
            tpx.lst.head =  - 1;
            tpx.lst.tail =  - 1;
            tpx.type = bt_func;
            spd = funcovermatch(sp1, &tpx, 0, FALSE, FALSE);
        }
    }
    if (!sp && !spd)
        return 0;
    if (sc == sc_auto)
    {
        epx = dummyvar(tp1->btp->size, tp1->btp, 0);
    }
    else
    {
        spx = sm;
        sprintf(buf, "BDV_%d", dummyram++);
        spx->name = litlate(buf);
        spx->tp = copytype(tp1->btp, 0);
        insert_decl_sp(spx);
        insertSpace(spx, prm_bss ? &bssbytes: &databytes);
        epx = makenode(en_nacon, spx, 0);
    }
    deref(&epx, tp1->btp);
    gensymerror(ERR_TEMPINIT, sp_in->name);
    if (sp)
    {
        if (!isaccessible(sp))
            genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(0, sp->name, 0));
        sp->mainsym->extflag = TRUE;
        pnode = makenode(en_napccon, sp, 0);
        parmlist(&node, &tpy, sp->tp);
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
            tcnode = epx;

        if (prm_cplusplus && tcnode && (sp->value.classdata.cppflags &PF_MEMBER)
            && !(sp->value.classdata.cppflags &PF_STATIC))
        {
            pnode = makenode(en_thiscall, tcnode, pnode);
        }
        pnode = doinline(pnode);
    }
    else
        pnode = makeintnode(en_icon, 0);
    pnode->cflags = tp1->cflags;
    pnode = makenode(en_conslabel, pnode, epx);
    if (sc == sc_auto)
    {
        if (indefaultinit)
        {
            if (expr_runup[expr_updowncount - 1])
                expr_runup[expr_updowncount - 1] = makenode(en_void,
                    expr_runup[expr_updowncount - 1], pnode);
            else
                expr_runup[expr_updowncount - 1] = pnode;
        }
        else
        {
            SNODE *snp = xalloc(sizeof(SNODE));
            snp->next = 0;
            snp->stype = st_expr;
            snp->exp = pnode;
            if (cbautoinithead == 0)
                cbautoinithead = cbautoinittail = snp;
            else
            {
                cbautoinittail->next = snp;
                cbautoinittail = snp;
            }
        }
    }
    else
    {
        cppinitinsert(pnode);
    }
    if (spd)
    {
        if (!isaccessible(spd))
            genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(0, spd->name, 0));
        spd->mainsym->extflag = TRUE;
        if (!(spd->value.classdata.cppflags &PF_STATIC))
            tcnode = epx;
        if (spd->value.classdata.cppflags &PF_VIRTUAL)
        {
            ENODE *inode,  *nnode;
            inode = makeintnode(en_icon, spd->value.classdata.vtabindex);
            if (spd->value.classdata.vtaboffs)
                tcnode = makenode(en_add, tcnode, makeintnode(en_icon, spd
                    ->value.classdata.vtaboffs));
            nnode = makenode(en_a_ref, (void*)copy_enode(tcnode), 0);
            pnode = makenode(en_add, inode, nnode);
            pnode = makenode(en_a_ref, pnode, 0);
        }
        else
            pnode = makenode(en_napccon, spd, 0);
        rnode = makenode(en_void, pnode, makenode(en_napccon, spd, 0));
        pnode = makenode(en_void, rnode, makenode(en_void, 0, 0));
        rnode = makeintnode(en_icon, spd->tp->btp->size);
        if (spd->pascaldefn)
            pnode = makenode(en_pfcall, rnode, pnode);
        else
            if (spd->isstdcall)
                pnode = makenode(en_sfcall, rnode, pnode);
            else
                pnode = makenode(en_fcall, rnode, pnode);
        if (prm_cplusplus && tcnode && (spd->value.classdata.cppflags
            &PF_MEMBER) && !(spd->value.classdata.cppflags &PF_STATIC))
        {
            pnode = makenode(en_thiscall, tcnode, pnode);
        }
        pnode = doinline(pnode);

    }
    else
        pnode = makeintnode(en_icon, 0);

    pnode->cflags = tp1->cflags;
    pnode = makenode(en_destlabel, pnode, epx);
    if (sc == sc_auto)
    {
        if (indefaultinit)
            expr_rundown[expr_updowncount - 1] = makenode(en_void, pnode,
                expr_rundown[expr_updowncount - 1]);
        else if (block_rundown)
            block_rundown = makenode(en_void, pnode, block_rundown);
        else
            block_rundown = pnode;
    }
    else
    {
        cpprundowninsert(pnode);
    }
    return epx;
}

//-------------------------------------------------------------------------

SYM *unnamed_search(char *na, SYM *cosym, int *offset)
{
    SYM *rv;
    cosym = cosym->tp->lst.head;
    while (cosym)
    {
        if (cosym->isunnamed)
        {
            rv = search(na, &cosym->tp->lst);
            if (!rv && cosym->tp->sp->hasunnamed)
                rv = unnamed_search(na, cosym, offset);
            if (rv)
            {
                *offset += cosym->value.i;
                return rv;
            }
        }
        cosym = cosym->next;
    }
    return 0;
}

//-------------------------------------------------------------------------

TYP *nameref(ENODE **node, TYP *tin)
/*
 * get an identifier.  If it is followed by parenthesis gather the
 * function parms.  If it is an undefined function declare it as external
 * for now.
 *
 * For C++ also gather in the class type qualifier
 *
 */
{
    SYM *sp = 0,  *cosym = 0,  *sp1;
    TYP *tp,  *tp1;
    int reclass = FALSE;
    int dclasssel = FALSE;
    ENODE *pnode = 0,  *qnode = 0,  *xnode = 0;
    int offset = 0;
    int override = FALSE, member = FALSE;
	int nosymerr = FALSE;
    char buf[256], cobuf[256];
    int notbase = FALSE;
    cobuf[0] = 0;
    strcpy(buf, lastid);
    getsym();
    if (cantnewline)
    {
        *node = makeintnode(en_icon,0) ;
        return &stdint ;
    }
        if (typequal)
        {
            override = TRUE;
            cosym = typequal;
            if (declclass)
            {
                *node = xnode = makenode(en_a_ref, copynode(thisenode), 0);
            }
            else
                *node = xnode = 0;
            typequal = 0;
        }
        else
        {

            if (lastst == classsel)
            {
                override = TRUE;
                dclasssel = TRUE;
                cosym = typesearch(lastid);
                docls: if (cosym && cosym->storage_class != sc_namespace &&
                    cosym->storage_class != sc_type)
                    gensymerror(ERR_NOTNSORTYPQUAL, cosym->name);
                getsym();
                if (lastst == kw_operator)
                {
                    TYP *tp2;
                    getsym();
                    return dooperator(&node);
                }
                else
                {
                    if (declclass)
                    {
                        *node = xnode = makenode(en_a_ref, copynode(thisenode),
                            0);
                    }
                    else
                    {
                        *node = xnode = makeintnode(en_icon, 0); /* better be a
                            member ptr ref if we get here */
                    }
                    if (!cosym || !isstructured(cosym->tp) && cosym
                        ->storage_class != sc_namespace)
                        generror(ERR_TYPEQUALEXP, 0, 0);

                    if (lastst != id)
                    {
                        *node = makeintnode(en_icon,  - 4);
                        return  &stdint;
                    }
                    strcpy(buf, lastid);
                    getsym();
                }
            }
            else
            {
                ENODE *ynode =  *node;
                if (tin && tin->type == bt_ref)
                {
                    ynode = makenode(en_a_ref, ynode, 0);
                    tin = tin->btp;
                }
                if (tin && isstructured(tin))
                {
                    override = TRUE;
                    cosym = tin->sp;
                    xnode = ynode;
                }
                else if (declclass)
                {
                    cosym = declclass;
                    *node = xnode = makenode(en_a_ref, copynode(thisenode), 0);
                    reclass = TRUE;
                }
            }
        }
        if (cosym)
            strcpy(cobuf, cosym->name);
        join: 
    nm = litlate(buf);
        if (!sp)
        if (cosym)
        {
            if (!override)
                sp = search(buf, &lsyms);
            if (sp)
            {
                cosym = 0;
                xnode = 0;
            }
            else
            {
                if (cosym->storage_class == sc_namespace)
                {
                    if (parsing_params)
                    {
                       
                            parm_namespaces[parm_ns_level][parm_ns_counts[parm_ns_level]++] = cosym;
                    }
                    sp = namespace_search(buf, cosym->value.classdata.parentns,
                        0);
                    if (!sp)
                        sp = namespace_search(buf, cosym
                            ->value.classdata.parentns, 1);
                    override = FALSE;
                }
                else
                {
                    sp = basesearch(buf, &cosym->tp->lst.head, TRUE);
                    if (!sp && cosym->hasunnamed)
                        sp = unnamed_search(buf, cosym, &offset);
                    if (!sp && cosym->parentclass)
                    {
                        SYM *sp2 = cosym->parentclass;
                        while (sp2)
                        {
                            sp = basesearch(buf, &sp2->tp->lst.head, TRUE);
                            if (sp)
                            {
                                if (!(sp->value.classdata.cppflags &PF_STATIC))
                                    gensymerror(ERR_STATICNOACCESS, sp->name);
                                break;
                            }
                            sp2 = sp2->parentclass;
                        }
                    }
                    if (!sp)
                        if (!override)
                            sp = typesearch(buf);
                        else {
							nosymerr = TRUE;
                            generror(ERR_MEMBERNAMEEXP, 0, 0);
						}
                }
            }
        }
        else
        sp = typesearch(buf);

    if (prm_cplusplus && override && cosym && !(cosym->value.classdata.cppflags
        &PF_STATIC))
        member = TRUE;
    if (sp == 0)
    {
        /* No such identifier */
        if (lastst == openpa)
        {
            /* External function, put it in the symbol table */
            ++global_flag;
            sp = makesym(sc_externalfunc);
            sp->name = litlate(buf);
            sp->tp = maketype(bt_func, 0);
            *(sp->tp) = stdfunc;
            sp->tp->sp = sp;
            sp->mainsym->extflag = TRUE;
            setalias(sp);
            *node = makenode(en_nacon, sp, 0);
                //                                    if (cosym)
                //                                       insert(sp,&cosym->tp->lst.head);
                //                                    else
            insert(sp, gsyms);
            --global_flag;
            tp = sp->tp;
            goodcode |= GF_ASSIGN;
        }
        else
        {
            /* External non-function.  These also get put in the symbol table so that
             * we don't keep spitting errors out but also put an error out
             */
            sp = makesym(sc_external);
            sp->name = litlate(buf);
            sp->tp = tp = maketype(bt_matchall, stdintsize);
            *(sp->tp) = stdmatch;
            sp->tp->sp = sp;
                if (!cosym)
                insert(sp, &lsyms);
            *node = makenode(en_nacon, &undef, 0);
			if (!nosymerr)
	            if (prm_cplusplus && override)
    	            genclass2error(ERR_UNDEFINED, cobuf, buf);
        	    else
            	    gensymerror(ERR_UNDEFINED, buf);
            tp = deref(node, tp);
            /* now skip sub-pointers if it was an unknown struct ref */
            if (tin)
            {
                while (lastst == pointsto || lastst == dot)
                {
                    getsym();
                    getsym();
                }
            }
        }
    }
    else
    {
        /* If we get here the symbol was already in the table
         */
        foundsp: 
            if (prm_cplusplus && lastst == lt)
                if (sp->tp->type == bt_struct || sp->tp->type == bt_class)
            if (sp->istemplate)
            {
                sp = lookupTemplateType(sp,0);
                if (lastst == classsel)
                {
                    override = TRUE;
                    dclasssel = TRUE;
                    cosym = sp;
                    sp = 0;
                    goto docls;
                }
            }
            if (prm_cplusplus && andtyp && andtyp->type == bt_memberptr && 
                (andtyp->btp->type == bt_func || andtyp->btp->type == bt_ifunc))
            {
                tp1 = andtyp->btp;
                *node = pnode = makenode(en_placeholder, (void*)sp, 0);
            }
            if (prm_cplusplus && andtyp && andtyp->type == bt_pointer && 
                (andtyp->btp->type == bt_func || andtyp->btp->type == bt_ifunc))
            {
                tp1 = andtyp->btp;
                *node = pnode = makeintnode(en_icon, 0);
            }
            if (prm_cplusplus && andtyp && andtyp->type == bt_farpointer && 
                (andtyp->btp->type == bt_func || andtyp->btp->type == bt_ifunc))
            {
                tp1 = andtyp->btp;
                *node = pnode = makeintnode(en_icon, 0);
            }
        if (sp->storage_class == sc_namespace)
        {
            gensymerror(ERR_ILLEGALNAMESPACEUSE, sp->name);
            *node = makeintnode(en_icon, 0);
            return  &stdint;
        }
        sp->tp->uflags |= UF_USED;
        if ((tp = sp->tp) == 0)
        {
            /* This lack of type info should never happen */
            tp = &stdmatch;
            *node = makenode(en_nacon, &undef, 0);
            gensymerror(ERR_UNDEFINED, buf);
            tp = deref(node, tp);
            return tp; /* guard against untyped entries */
        }
        switch (sp->storage_class)
        {
            case sc_defunc:
                if (!reclass &&  *node || declclass && sp->parentclass &&
                    declclass->mainsym == sp->parentclass->mainsym)
                    *node = makenode(en_thiscall,  *node, 0);
                else
                    *node = makenode(en_placeholder, sp, 0);
                break;
            case sc_static:
            case sc_global:
            case sc_external:
            case sc_externalfunc:
            case sc_abs:
                if ((sp->tp->cflags &DF_CONST) && scalarnonfloat(sp->tp) && sp
                    ->storage_class == sc_static && !(goodcode & GF_AND))
                {
                    if (sp->init)
                        *node = makeintnode(en_icon, sp->init->val.i);
                    else
                        *node = makeintnode(en_icon, 0);
                    return sp->tp;
                }
                else
                {
                    sp->mainsym->extflag = TRUE;
                        if (sp->value.classdata.gdeclare)
                            sp->value.classdata.gdeclare->mainsym->extflag =
                                TRUE;
                    if (sp->absflag)
                        *node = makenode(en_absacon, sp, 0);
                    else
                    if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
                    {
						if (!strcmp(sp->name, "_setjmp"))
							setjmp_used = TRUE;
                        *node = makenode(en_napccon, sp, 0);
                    }
                    else
                        if (sp->staticlabel)
                            *node = makenode(en_nalabcon, sp, 0);
                        else
                            *node = makenode(en_nacon, sp, 0);
                }
                break;
            case sc_const:
                /* constants and enums */
                *node = makeintnode(en_icon, sp->value.i);
                if (prm_cplusplus)
                    return sp->tp;
                else
                    return  &stdint;
                /* structure/class elements */
            case sc_member:
                if (sp->value.classdata.cppflags &PF_STATIC)
                {
                    *node = makenode(en_nacon, sp, 0);
                    sp->mainsym->extflag = TRUE;
                    sp->value.classdata.gdeclare->mainsym->extflag = TRUE;
                }
                else
                {
                    qnode = makeintnode(en_icon, sp->value.i + offset);
                    *node = makenode(en_addstruc, qnode,  *node);
                }
                break;
            default:
                 /* auto and any errors */
                if ((sp->tp->cflags &DF_CONST) && sp->init && scalarnonfloat(sp
                    ->tp) && sp->storage_class == sc_static)
                {
                    *node = makeintnode(en_icon, sp->init->val.i);
                    return sp->tp;
                }
                else
                {
                    SYM *sp1;
                    if (prm_cplusplus && isstructured(sp->tp) && lastst ==
                        openpa)
                    {
                        ENODE *ep2;
                        ENODE *epm = dummyvar(sp->tp->size, sp->tp, 0);
                        ENODE *epq;
                        TYP tp3,  *tp2;
                        getsym();
                        tp3.lst.head =  - 1;
                        tp3.lst.tail =  - 1;
                        tp3.type = bt_func;
                        tp2 = gatherparms(&ep2, FALSE);
                        epq = do_destructor(epm->v.sp, &tp3, sp->tp, 0, 1, 0,
                            TRUE);
                        if (epq)
                            expr_rundown[expr_updowncount - 1] = makenode
                                (en_void, epq, expr_rundown[expr_updowncount -
                                1]);
                        epq = do_constructor(epm->v.sp, tp2, sp->tp, ep2, 1, 0,
                            FALSE, FALSE);
                        if (epq)
                            if (expr_runup[expr_updowncount - 1])
                                expr_runup[expr_updowncount - 1] = makenode
                                    (en_void, expr_runup[expr_updowncount - 1],
                                    epq);
                            else
                                expr_runup[expr_updowncount - 1] = epq;
                        *node = epm;
                        tp = sp->tp;
                        //                                          needpunc(closepa, skm_declclosepa) ;
                        parm_ns_level--;
                    }
                    else if (sp->storage_class != sc_auto && sp->storage_class
                        != sc_autoreg)
                    {
                        gensymerror(ERR_ILLCLASS2, sp->name);
                        tp = &stdint;
                        *node = makenode(en_nacon, &undef, 0);
                    }
                    else
                    {
                        /* auto variables */
                        if (sp->storage_class == sc_auto)
                            *node = makenode(en_autocon, sp, 0);
                        else if (sp->storage_class == sc_autoreg)
                            *node = makenode(en_autoreg, sp, 0);
                    }
                }
                break;
        }
        /* dereference if it isn't an array or structure address */
        if (tp)
        {
            (*node)->cflags = tp->cflags;
            /* deref if not an array or if is function parm */
            if (sp->importable && tp->type != bt_func && tp->type != bt_ifunc)
            {
                *node = makenode(en_add, (void*) * node, makeintnode(en_icon, 2)
                    );
                *node = makenode(en_a_ref, (void*) * node, 0);
                *node = makenode(en_a_ref, (void*) * node, 0);
            }
            if (tp && (tp->val_flag == 0 || (tp->val_flag &VARARRAY) || (sp
                ->funcparm && (tp->type == bt_pointer || tp->type ==
                bt_farpointer))) && (*node)->nodetype != en_icon)
            /* and dereference it if it is an imported variable */
                if (tp->type != bt_defunc)
				{
                    tp = deref(node, tp);
					if (sp->funcparm)
						tp->cflags |= DF_FUNCPARM;
				}
            /* and dereference again if it is a refernece variable */
            if (tp->type == bt_ref)
            {
                //if (tp->btp->type == bt_pointer)
                deref(node, tp->btp);
                tp = tp->btp;
                (*node)->cflags = tp->cflags;
            }
        }
        if (sp->parentclass && !(sp->value.classdata.cppflags &PF_STATIC) && 
            /*member && dclasssel && */((goodcode &GF_AND) || sp->tp->type ==
            bt_defunc && lastst != openpa))
        {
            // if a func, ep1 will get overwritten later
            if (sp->tp->type == bt_defunc)
            {
                TYP *tp1 = maketype(bt_memberptr, 0);
                *node = makenode(en_placeholder, (void*)sp, 0);
                tp1->btp = tp;
                tp1->sp = sp->parentclass;
                tp = tp1;
            }
            else if (asntyp)
            {
                TYP *tp1 = maketype(bt_memberptr, 0);
                tp1->btp = tp;
                tp1->sp = sp->parentclass;
                tp = tp1;
                cseg();
                put_label(nextlabel);
                genlong(sp->value.i + 1);
                genlong(0);
                nl();
                *node = makeintnode(en_labcon, nextlabel++);
            }
        }
    }
    if (dclasssel && (!(goodcode &GF_AND) || tp->type != bt_memberptr) && sp
        ->storage_class != sc_defunc && sp->tp->type != bt_ifunc && sp->tp
        ->type != bt_func && (sp->value.classdata.cppflags &PF_MEMBER) && !(sp
        ->value.classdata.cppflags &PF_STATIC))
        gensymerror(ERR_STATICNOACCESS, sp->name);
    if (override)
        (*node)->cflags |= DF_HASOVERRIDE;

    if (tp->type == bt_fcomplex || tp->type == bt_rcomplex || tp->type == bt_lrcomplex)
    {
        if (*node)
            if (stdpragmas &STD_PRAGMA_CXLIMITED)
                (*node)->cflags |= DF_CXLIMITED;
    }
    lastsym = sp;
    return tp;
}

//-------------------------------------------------------------------------

int cppcast(TYP *newtype, TYP *oldtype, ENODE **node, int alwayserr)
{
    ENODE *pnode =  *node,  *qnode;
    int rv = FALSE;
    SYM *sp;
    SYM *s1;
    CLASSLIST *l;
    if (oldtype->type == bt_void)
    {
        generror(ERR_NOVOID, 0, 0);
    }
    else if (newtype)
    {
        if (newtype->type == bt_pointer && oldtype->type == bt_pointer &&
            isstructured(newtype->btp) && isstructured(oldtype->btp))
        {
            sp = newtype->btp->sp;
            s1 = oldtype->btp->sp;
            l = s1->value.classdata.baseclass;
            while (l)
            {
                if (sp->mainsym == l->data->mainsym)
                {
                    if (l->offset)
                        pnode = makenode(en_addcast, pnode, makeintnode(en_icon,
                            l->offset));
                    break;
                }
                l = l->link;
            }
            if (!l)
            {
                l = sp->value.classdata.baseclass;
                while (l)
                {
                    if (s1->mainsym == l->data->mainsym)
                    {
                        if (l->offset)
                            pnode = makenode(en_addcast, pnode, makeintnode
                                (en_icon,  - l->offset));
                        break;
                    }
                    l = l->link;
                }
            }
            if (l)
                rv = TRUE;
            else if (alwayserr)
                genmismatcherror(oldtype, newtype);

        }
        else
            if (isstructured(oldtype))
        if (isstructured(newtype))
        {
            SYM *sp = newtype->sp;
            SYM *s1 = oldtype->sp;
            CLASSLIST *l = s1->value.classdata.baseclass;
            while (l)
            {
                if (!strcmp(sp->name, l->data->name))
                {
                    pnode = makenode(en_addstruc, pnode, makeintnode(en_icon, l
                        ->offset));
                    break;
                }
                l = l->link;
            }
        }
        else
        {
            SYM *sp = oldtype->sp;
            sp = search("$casts$", &sp->tp->lst);
            if (!sp)
                genmismatcherror(oldtype, newtype);
            else
            {
                sp = castmatch(oldtype->sp, newtype, &sp->tp->lst);
                if (!sp)
                    genmismatcherror(oldtype, newtype);
                else
                {
                    ENODE *xnode;
                    CLASSLIST *l = oldtype->sp->value.classdata.baseclass;
                    if (!isaccessible(sp))
                        genclasserror(ERR_NOTACCESSIBLE, fullcppmangle(0, sp
                            ->name, 0));
                    while (l)
                    {
                        if (!strcmp(sp->parentclass->name, l->data->name))
                            break;
                        l = l->link;
                    }
                    if (l && l->offset)
                        xnode = makenode(en_addstruc, pnode, makeintnode
                            (en_icon, l->offset));
                    else
                        xnode = pnode;
                    sp->mainsym->extflag = TRUE;
                    pnode = makenode(en_void, 0, 0);
                    qnode = makenode(en_napccon, sp, 0);
                    qnode = makenode(en_void, qnode, qnode);
                    pnode = makenode(en_void, qnode, pnode);
                    qnode = makeintnode(en_icon, retvalsize(sp->tp->btp));
                    if (isstructured(sp->tp->btp))
                    {
                        pnode = makenode(en_fcallb, qnode, pnode);
                        (pnode)->size = sp->tp->btp->size;
                    }
                    else
                        pnode = makenode(en_fcall, qnode, pnode);
                    pnode = makenode(en_thiscall, xnode, pnode);
                    pnode = doinline(pnode);
                    rv = TRUE;
                }
            }
        }
        else if (isstructured(newtype) || newtype->type == bt_pointer &&
            isstructured(newtype->btp))
        {
            if (newtype->type == bt_pointer && scalarnonfloat(oldtype))
            {
                ENODE *epx =  *node;
                while (castvalue(epx))
                    epx = epx->v.p[0];
                if (!isintconst(epx->nodetype) || epx->v.i != 0)
                    genmismatcherror(oldtype, newtype);
            }
            else
                genmismatcherror(oldtype, newtype);
        }
        else if (newtype->type != oldtype->type && (newtype->type ==
            bt_memberptr || oldtype->type == bt_memberptr))
        {
            rv = FALSE;
        }
        else if (newtype->type != bt_void && oldtype->type != bt_void)
            rv = TRUE;

    }

    *node = pnode;
    return rv;
}

//-------------------------------------------------------------------------

void promote_type(TYP *typ, ENODE **node)
/*
 * Type promotion for casts and function args 
 */
{
    if (typ->type == bt_ref)
        typ = typ->btp;
    switch (typ->type)
    {
        case bt_bool:
            *node = makenode(en_cbool,  *node, 0);
            break;
        case bt_char:
            *node = makenode(en_cb,  *node, 0);
            break;
        case bt_unsignedchar:
            *node = makenode(en_cub,  *node, 0);
            break;
        case bt_short:
            *node = makenode(en_cw,  *node, 0);
            break;
        case bt_unsignedshort:
            *node = makenode(en_cuw,  *node, 0);
            break;
        case bt_int:
        case bt_enum:
            *node = makenode(en_ci,  *node, 0);
            break;
        case bt_long:
            *node = makenode(en_cl,  *node, 0);
            break;
        case bt_unsigned:
            *node = makenode(en_cui,  *node, 0);
            break;
        case bt_unsignedlong:
            *node = makenode(en_cul,  *node, 0);
            break;
        case bt_float:
            *node = makenode(en_cf,  *node, 0);
            break;
        case bt_fcomplex:
            *node = makenode(en_cfc,  *node, 0);
            break;
        case bt_rcomplex:
            *node = makenode(en_crc,  *node, 0);
            break;
        case bt_lrcomplex:
            *node = makenode(en_clrc,  *node, 0);
            break;
        case bt_fimaginary:
            *node = makenode(en_cfi,  *node, 0);
            break;
        case bt_rimaginary:
            *node = makenode(en_cri,  *node, 0);
            break;
        case bt_lrimaginary:
            *node = makenode(en_clri,  *node, 0);
            break;
        case bt_double:
            *node = makenode(en_cd,  *node, 0);
            break;
        case bt_longdouble:
            *node = makenode(en_cld,  *node, 0);
            break;
        case bt_longlong:
            *node = makenode(en_cll,  *node, 0);
            break;
        case bt_unsignedlonglong:
            *node = makenode(en_cull,  *node, 0);
            break;
        case bt_farpointer:
            *node = makenode(en_cfp,  *node, 0);
            break;
        case bt_segpointer:
            *node = makenode(en_csp,  *node, 0);
            break;
        default:
            *node = makenode(en_cp,  *node, 0);
            break;
    }
    (*node)->cflags = typ->cflags;
}

//-------------------------------------------------------------------------

TYP *gatherparms(ENODE **node, int isvoid)
/*
 * create a type tree and primary parameter list for a function
 *
 * At this point the parameter list is backwards from what codegen
 * needs!
 */
{
    ENODE *ep1 = 0,  *ep2 = 0,  **ep3 = &ep1;
    TABLE tbl;
    int odf = global_deref;
    SYM **t = &tbl.head,  *newt;
    TYP *tp;
    char buf[256];
    int ogc = goodcode;
    parsing_params = TRUE;
    parm_ns_counts[parm_ns_level] = 0;
    tbl.tail = tbl.head = 0;
    goodcode |= DF_FUNCPARMS;
    goodcode &= ~GF_INIF;
    global_deref = 0;
    strcpy(buf, declid);
    if (lastst == closepa || isvoid)
    {
            if (prm_cplusplus)
                tbl.head = tbl.tail = (SYM*) - 1;
            else
            tbl.head = tbl.tail = 0;
    }
    else if (lastst == kw_void)
        tbl.head = tbl.tail = (SYM*) - 1;
    else
    while (lastst != closepa)
    {
        tp = exprnc(&ep2);
        if (!tp)
        {
            generror(ERR_EXPREXPECT, 0, 0);
            tp = &stdint;
            ep2 = makenode(en_icon, 0, 0);
        } else
            opt4(&ep2, FALSE);
        ep2->cflags = tp->cflags;
//        if (tp->bits !=  - 1)
//        {
//            ep2 = makenode(en_bits, ep2, 0);
//            ep2->bits = tp->bits;
//            ep2->startbit = tp->startbit;
//        }
        newt = makesym(sc_auto);
        newt->tp = tp;
        newt->next = 0;
        newt->name = 0;
        *t = newt;
        t = &newt->next;
        tbl.tail = newt;
        *ep3 = makenode(en_void, ep2, 0);
        ep3 = &(*ep3)->v.p[1];
        if (lastst == comma)
            getsym();
        else
            break;
    }
    if (!isvoid)
        needpunc(closepa, skm_closepa);
    tp = maketype(bt_func, 0);
    tp->val_flag = NORMARRAY;
    tp->btp = &stdint;
    tp->lst = tbl;
    tp->bits =  - 1;
    tp->startbit =  - 1;
    tp->uflags = UF_DEFINED | UF_USED;
    goodcode = ogc;
    *node = ep1;
    global_deref = odf;
    strcpy(declid, buf);
    parsing_params = FALSE;
    parm_ns_level++;
    return tp;
}

//-------------------------------------------------------------------------

int checkparmconst(TYP *tp /*right*/, TYP *tpi /*left*/)
/*
 * Check the CONST flags for parameters
 */
{
    if (tpi->type != bt_pointer && tpi->type != bt_farpointer && tpi->type !=
        bt_ref)
        return 0;
    tpi = tpi->btp;

    if (tp->type == bt_pointer || tp->type == bt_farpointer || tp->type ==
        bt_ref)
        tp = tp->btp;
    while ((tp->type == bt_pointer || tp->type == bt_farpointer || tp->type ==
        bt_ref) && !tp->val_flag && (!tpi || tpi->type == bt_pointer || tpi
        ->type == bt_farpointer))
    {
        if ((tp->cflags &DF_CONST) && (!tpi || !(tpi->cflags &DF_CONST)))
        {
            if (prm_cplusplus)
                generror(ERR_MODCONS, 0, 0);
            else
                generror(ERR_SUSPICIOUSCONST, 0, 0);
            return 1;
        }
        tp = tp->btp;
        if (tpi)
            tpi = tpi->btp;
    }
    #ifdef XXXXX	
        if ((tp->cflags &DF_CONST) && (!tpi || !(tpi->cflags &DF_CONST)))
        {
            if (prm_cplusplus)
                generror(ERR_MODCONS, 0, 0);
            else
                generror(ERR_SUSPICIOUSCONST, 0, 0);
            return 1;
        }
    #endif 
    return 0;
}

//-------------------------------------------------------------------------

ENODE *copynode(ENODE *node)
/*
 *      copy the node passed into a new enode so it wont get
 *      corrupted during substitution.
 */
{
    ENODE *temp;
    if (node == 0)
        return 0;
    temp = xalloc(sizeof(ENODE));
    temp->cflags = node->cflags;
    temp->nodetype = node->nodetype;
    temp->v.p[0] = node->v.p[0];
    temp->v.p[1] = node->v.p[1];
    return temp;
}


//-------------------------------------------------------------------------

ENODE *copy_enode(ENODE *node)
{
    /*
     * routine takes an enode tree and replaces it with a copy of itself.
     * Used because we have to munge the block_nesting field (value.i) of each
     * sp in an inline function to force allocation of the variables
     */
    ENODE *temp;
    if (node == 0)
        return 0;
    temp = xalloc(sizeof(ENODE));
    memcpy(temp, node, sizeof(ENODE));
    switch (temp->nodetype)
    {
        case en_structret:
		case en_tempref:
            break;
        case en_autoreg:
        case en_autocon:
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_fcomplexcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_icon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_boolcon:
        case en_ccon:
        case en_cucon:
        case en_nacon:
        case en_absacon:
        case en_napccon:
        case en_llcon:
        case en_llucon:
        case en_labcon:
        case en_nalabcon:
            break;
        case en_fp_ref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_cfi:
        case en_cri:
        case en_clri:
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_bits:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_ll_ref:
        case en_ull_ref:
        case en_b_ref:
        case en_w_ref:
        case en_ul_ref:
        case en_l_ref:
        case en_ub_ref:
        case en_uw_ref:
		case en_i_ref:
		case en_ui_ref:
		case en_a_ref: case en_ua_ref:
        case en_bool_ref:
        case en_uminus:
        case en_asuminus:
        case en_ascompl:
        case en_compl:
        case en_not:
        case en_cf:
        case en_cd:
        case en_cld:
        case en_cb:
        case en_cub:
        case en_cbool:
        case en_cw:
        case en_cuw:
        case en_cll:
        case en_cull:
        case en_cl:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cp:
        case en_cfp:
        case en_csp:
        case en_trapcall:
        case en_cl_reg:
        case en_movebyref:
        case en_substack:
		case en_loadstack:
		case en_savestack:
            temp->v.p[0] = copy_enode(node->v.p[0]);
            break;
        case en_asadd:
        case en_assub:
        case en_add:
        case en_sub:
        case en_addstruc:
        en_addcast: case en_asalsh:
        case en_asarsh:
        case en_alsh:
        case en_arsh:
        case en_arshd:
        case en_asarshd:
        case en_asmul:
        case en_asdiv:
        case en_asmod:
        case en_aslsh:
        case en_asumod:
        case en_asudiv:
        case en_asumul:
        case en_asrsh:
        case en_asand:
        case en_assign:
        case en_refassign:
        case en_lassign:
        case en_asor:
        case en_asxor:
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
        case en_void:
        case en_dvoid:
        case en_pmul:
		case en_arrayindex:
        case en_pdiv:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_lsh:
        case en_rsh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_moveblock:
        case en_stackblock:
        case en_fcall:
        case en_fcallb:
        case en_pfcall:
        case en_pfcallb:
        case en_sfcall:
        case en_sfcallb:
        case en_thiscall:
        case en_repcons:
        case en_ainc:
        case en_adec:
		case en_array:
            temp->v.p[1] = copy_enode(node->v.p[1]);
        case en_clearblock:
            temp->v.p[0] = copy_enode(node->v.p[0]);
            break;
        case en_conslabel:
        case en_destlabel:
            temp->v.p[0] = copy_enode(node->v.p[0]);
            temp->v.p[1] = copy_enode(node->v.p[1]);
            break;
        default:
            DIAG("copy_enode: Unknown nodetype");
            break;
    }
    return temp;
}

//-------------------------------------------------------------------------

ENODE *copy_default(ENODE *node)
{
    /*
     * routine takes an enode tree and replaces it with a copy of itself.
     * Used because we have to munge the block_nesting field (value.i) of each
     * sp in an inline function to force allocation of the variables
     */
    ENODE *temp;
    SYM *sp;
    if (node == 0)
        return 0;
    temp = xalloc(sizeof(ENODE));
    memcpy(temp, node, sizeof(ENODE));
    switch (temp->nodetype)
    {
        case en_structret:
            break;
        case en_autoreg:
        case en_autocon:
            if (!strncmp(temp->v.sp->name, "**DUMMY", 7))
            {
                SYM *sp = search(temp->v.sp->name, &defalt_table);
                if (!sp)
                {
                    temp->v.sp->defalt = dummyvar(temp->v.sp->tp->size, temp
                        ->v.sp->tp, 0);
                    insert(temp->v.sp, &defalt_table);
                    sp = temp->v.sp->defalt->v.sp;
                }
                else
                    sp = sp->defalt->v.sp;
                temp->v.sp = sp;
            }
            break;
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_fcomplexcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_icon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_boolcon:
        case en_ccon:
        case en_cucon:
        case en_nacon:
        case en_absacon:
        case en_napccon:
        case en_llcon:
        case en_llucon:
            break;
        case en_fp_ref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_cfi:
        case en_cri:
        case en_clri:
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_bits:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_ll_ref:
        case en_ull_ref:
        case en_b_ref:
        case en_w_ref:
        case en_ul_ref:
        case en_l_ref:
        case en_ub_ref:
        case en_uw_ref:
		case en_i_ref:
		case en_ui_ref:
		case en_a_ref: case en_ua_ref:
        case en_bool_ref:
        case en_uminus:
        case en_asuminus:
        case en_ascompl:
        case en_compl:
        case en_not:
        case en_cf:
        case en_cd:
        case en_cld:
        case en_cb:
        case en_cub:
        case en_cbool:
        case en_cw:
        case en_cuw:
        case en_cll:
        case en_cull:
        case en_cl:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cp:
        case en_cfp:
        case en_csp:
        case en_trapcall:
        case en_cl_reg:
        case en_movebyref:
        case en_substack:
		case en_loadstack:
		case en_savestack:
            temp->v.p[0] = copy_default(node->v.p[0]);
            break;
        case en_asadd:
        case en_assub:
        case en_ainc:
        case en_adec:
        case en_add:
        case en_sub:
        case en_addstruc:
        en_addcast: case en_asalsh:
        case en_asarsh:
        case en_alsh:
        case en_arsh:
        case en_arshd:
        case en_asarshd:
        case en_asmul:
        case en_asdiv:
        case en_asmod:
        case en_aslsh:
        case en_asumod:
        case en_asudiv:
        case en_asumul:
        case en_asrsh:
        case en_asand:
        case en_assign:
        case en_refassign:
        case en_lassign:
        case en_asor:
        case en_asxor:
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
        case en_void:
        case en_dvoid:
        case en_pmul:
		case en_arrayindex:
        case en_pdiv:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_lsh:
        case en_rsh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_moveblock:
        case en_stackblock:
        case en_fcall:
        case en_fcallb:
        case en_pfcall:
        case en_pfcallb:
        case en_sfcall:
        case en_sfcallb:
        case en_thiscall:
        case en_repcons:
		case en_array:
            temp->v.p[1] = copy_default(node->v.p[1]);
        case en_clearblock:
            temp->v.p[0] = copy_default(node->v.p[0]);
            break;
        case en_conslabel:
        case en_destlabel:
            temp->v.p[0] = copy_default(node->v.p[0]);
            temp->v.p[1] = copy_default(node->v.p[1]);
            break;
    }
    return temp;
}

//-------------------------------------------------------------------------

TYP *FindMemberPointer(TYP *tp1, TYP *tp2, ENODE **ep2)
{
    SYM *sp;
    SYM *s1;
    CLASSLIST *l;
    if (tp1->type != bt_memberptr)
        return tp1;
    if (tp2->type != bt_memberptr)
    {
        cseg();
        put_label(nextlabel);
        *ep2 = makenode(en_labcon, (void*)nextlabel++, 0);
        genlong(0);
        genlong(0);
        nl();
        return tp1;
    }
    sp = tp2->sp;
    s1 = tp1->sp;
    if (sp->mainsym != s1->mainsym)
    {
        l = sp->value.classdata.baseclass;
        while (l)
        {
            if (s1->mainsym == l->data->mainsym)
            {
                break;
            }
            l = l->link;
        }
        if (!l)
        {
            sp = tp1->sp;
            s1 = tp2->sp;
            l = sp->value.classdata.baseclass;
            while (l)
            {
                if (s1->mainsym == l->data->mainsym && l->offset == 0)
                {
                    break;
                }
                l = l->link;
            }
            if (!l)
            {
                genmismatcherror(tp1, tp2);
            }
        }
    }
    if ((*ep2)->nodetype != en_placeholder)
        return tp2;
    sp = funcovermatch((*ep2)->v.sp, tp1->btp, 0, FALSE, FALSE);
    if (sp && !(sp->value.classdata.cppflags &PF_STATIC))
    {
        int val = 0;
        SYM *sp1;
        cseg();
        put_label(nextlabel);
        *ep2 = makenode(en_labcon, (void*)nextlabel++, 0);
        SetExtFlag(sp, TRUE);
        if (sp->value.classdata.cppflags &PF_VIRTUAL)
        {
            sp1 = gen_mp_virtual_thunk(sp);
        }
        else
            sp1 = sp;

        if (tp1->btp->type == bt_func)
        {
            s1 = sp->parentclass;
            l = tp2->sp->value.classdata.baseclass;
            while (l)
            {
                if (s1->mainsym == l->data->mainsym)
                {
                    val = l->offset;
                    break;
                }
                l = l->link;
            }
        }
        genref(sp1, 0);
        genlong(val);
        nl();
        return tp1;
    }
    return tp2;
}

//-------------------------------------------------------------------------

void parameter_rundown(TABLE *tbl)
{
    if (prm_cplusplus)
    {
        SYM *sp = tbl->head;
        while (sp)
        {
            if (sp->funcparm && isstructured(sp->tp))
            {
                SYM *sp1 = search(cpp_funcname_tab[CI_DESTRUCTOR], &sp->tp->lst)
                    ;
                SYM *spd = 0;
                if (sp1)
                {
                    TYP tpx;
                    tpx.lst.head =  - 1;
                    tpx.lst.tail =  - 1;
                    tpx.type = bt_func;
                    spd = funcovermatch(sp1, &tpx, 0, FALSE, FALSE);
                    if (spd)
                    {
                        ENODE *pnode,  *rnode,  *tcnode = 0,  *epx;
                        if (sp->storage_class == sc_auto)
                            epx = makenode(en_autocon, sp->mainsym, 0);
                        else if (sp->storage_class == sc_autoreg)
                            epx = makenode(en_autoreg, sp->mainsym, 0);
                        spd->mainsym->extflag = TRUE;
                        if (!(spd->value.classdata.cppflags &PF_STATIC))
                            tcnode = epx;
                        if (spd->value.classdata.cppflags &PF_VIRTUAL)
                        {
                            ENODE *inode,  *nnode;
                            inode = makeintnode(en_icon, spd
                                ->value.classdata.vtabindex);
                            if (spd->value.classdata.vtaboffs)
                                tcnode = makenode(en_add, tcnode, makeintnode
                                    (en_icon, spd->value.classdata.vtaboffs));
                            nnode = makenode(en_a_ref, (void*)copy_enode(tcnode)
                                , 0);
                            pnode = makenode(en_add, inode, nnode);
                            pnode = makenode(en_a_ref, pnode, 0);
                        }
                        else
                            pnode = makenode(en_napccon, spd, 0);
                        rnode = makenode(en_void, pnode, makenode(en_napccon,
                            spd, 0));
                        pnode = makenode(en_void, rnode, makenode(en_void, 0, 0)
                            );
                        rnode = makeintnode(en_icon, spd->tp->btp->size);
                        if (spd->pascaldefn)
                            pnode = makenode(en_pfcall, rnode, pnode);
                        else
                            if (spd->isstdcall)
                                pnode = makenode(en_sfcall, rnode, pnode);
                            else
                                pnode = makenode(en_fcall, rnode, pnode);
                        if (prm_cplusplus && tcnode && (spd
                            ->value.classdata.cppflags &PF_MEMBER) && !(spd
                            ->value.classdata.cppflags &PF_STATIC))
                        {
                            pnode = makenode(en_thiscall, tcnode, pnode);
                        }
                        pnode = doinline(pnode);
                        if (pnode)
                        {
                            sp->mainsym->value.classdata.conslabel =  - 1;
                            pnode = makenode(en_destlabel, pnode, epx);
                            if (block_rundown)
                                block_rundown = makenode(en_void, pnode,
                                    block_rundown);
                            else
                                block_rundown = pnode;
                        }
                    }
                }
            }
            sp = sp->next;
        }
    }
}

int checkexactpointer(TYP *tp1, TYP *tp2)
{
	tp1 = tp1->btp;
	tp2 = tp2->btp;
	if (tp1->type == bt_void || tp2->type == bt_void)
		return 1;
	while (tp1 && tp2)
	{
		if (tp1->type != tp2->type || tp1->size != tp2->size)
			return 0;
		if (isstructured(tp1) || tp1->type == bt_enum)
			if (tp1->sp->mainsym != tp2->sp->mainsym)
				return 0;
		tp1 = tp1->btp;
		tp2 = tp2->btp;
	}
	return (!tp1 && !tp2);
}
//-------------------------------------------------------------------------

void parmlist(ENODE **node, TYP *tpi, TYP *tp)
/*
 * take the primary type and node trees, the function argument expectations,
 * and check for type mismatch errors
 *
 * also reverse the primary node tree so the parms will be ready for
 * code generation
 */
{
    ENODE *ep1 = 0,  *ep2 = 0,  *ep3 =  *node;
    SYM *spi = tpi->lst.head,  *sp = 0;
    TYP *tp2;
    int memptr;
    int matching = FALSE;
    if (tp->type == bt_pointer || tp->type == bt_farpointer || tp->type ==
        bt_memberptr)
        tp = tp->btp;
    if (tp)
        sp = tp->lst.head;
    if (tp && !sp)
        if (!tp->sp->hasproto)
            gensymerror(ERR_NOPROTO, tp->sp->name);

    if (sp &&!tp->sp->oldstyle && (sp == (SYM*) - 1 || sp->tp->type !=
        bt_ellipse))
        matching = TRUE;
    while (TRUE)
    {
        memptr = 0;
        if (!spi || spi == (SYM*) - 1)
        {
            if (sp == (SYM*) - 1)
                break;
            if (sp && sp->tp->type != bt_ellipse)
                    if (!sp->value.classdata.defalt)
                    genfuncerror(ERR_CALLLENSHORT, tp->sp->name, 0);
                else
                while (sp && sp != (SYM*) - 1)
                {
                    ENODE *en;
                    memset(&defalt_table, 0, sizeof(defalt_table));
                    if (sp->tp->val_flag && isstructured(sp->tp))
                    {
                        en = copy_default(sp->value.classdata.defalt->ep);
                        ep1 = sbll(sp, en, ep1);
                    }
                    else
                        ep1 = makenode(en_void, copy_default(sp
                            ->value.classdata.defalt->ep), ep1);
                    if (sp->value.classdata.defalt->cons)
                    {
                        conscount++;
                        if (expr_runup[expr_updowncount - 1])
                            expr_runup[expr_updowncount - 1] = makenode(en_void,
                                expr_runup[expr_updowncount - 1], copy_default
                                (sp->value.classdata.defalt->cons));
                        else
                            expr_runup[expr_updowncount - 1] = copy_default(sp
                                ->value.classdata.defalt->cons);
                    }
                    if (sp->value.classdata.defalt->dest)
                    {
                        conscount++;
                        expr_rundown[expr_updowncount - 1] = makenode(en_void,
                            copy_default(sp->value.classdata.defalt->dest),
                            expr_rundown[expr_updowncount - 1]);
                    }
                    sp = sp->next;
                }
            break;
        }
        else
        {
            if (spi->tp->type == bt_cond)
                spi->tp = spi->tp->btp;
            ep2 = ep3->v.p[0];
            ep3 = ep3->v.p[1];
            if (matching)
            {
                if (!sp || sp == (SYM*) - 1)
                {
                    if (!tp->sp->oldstyle)
                        genfuncerror(ERR_CALLLENLONG, tp->sp->name, 0);
                    break;
                }
                else
                {
                    int flag;
                    checkparmconst(spi->tp, sp->tp);
					if ((sp->tp->type == bt_pointer ||sp->tp->type == bt_farpointer)
						&& (spi->tp->type == bt_pointer || spi->tp->type == bt_farpointer))
					{
//						if (!checkexactpointer(sp->tp, spi->tp))
//							generror(ERR_SUSPICIOUS,0,0);
					}
                    if (sp->tp->type == bt_memberptr)
                    {
                        //                              if (spi->tp->type != bt_memberptr) 
                        //                                generror(ERR_MEMBERNAMEEXP,0,0);
                        //                              else 
                        {
                            TYP *tp1 = FindMemberPointer(sp->tp, spi->tp, &ep2);
                            if (!exactype(sp->tp, tp1, FALSE))
                                generror(ERR_MEMBERNAMEEXP, 0, 0);
                            memptr = stdmemberptrsize;
                            while (castvalue(ep2))
                                ep2 = ep2->v.p[0];
                            if (lvalue(ep2))
                                ep2 = ep2->v.p[0];
                        }
                    }
                    else if (sp->tp->type == bt_pointer && (sp->tp->btp->type 
                        == bt_func))
                    {
						if (spi->tp->type == bt_defunc)
						{
	                        SYM *sp1 = funcovermatch(spi, sp->tp->btp, 0, FALSE,
    	                        FALSE);
        		            if (sp1)
                	        {
                    	        ep2 = makenode(en_napccon, (void*)sp1, 0);
                        	}
						}
						else
						{
							if (spi->tp->type == bt_func || spi->tp->type == bt_ifunc)
							{
								if (!exactype(sp->tp->btp, spi->tp, 2))
									generror(ERR_MISMATCH,0,0);
							}
							else if (spi->tp->type == bt_pointer &&
									 (spi->tp->btp->type == bt_func ||
									  spi->tp->btp->type == bt_ifunc))
							{
								if (!exactype(sp->tp->btp, spi->tp->btp, 2))
									generror(ERR_MISMATCH,0,0);
							}
							else
							{
								ENODE *ep3 = ep2;
	                            while (castvalue(ep3))
    	                            ep3 = ep3->v.p[0];
								if (!isintconst(ep3->nodetype) || 
									ep3->v.i != 0)
									generror(ERR_MISMATCH, 0, 0);
							}
						}
                    }
                    else if (sp->tp->type == bt_pointer && (sp->tp->btp->type 
                        == bt_void) && spi->tp->type == bt_defunc)
                    {
                        if (spi->tp->lst.head->next)
                            gensymerror(ERR_CANTRESOLVE, spi->name);
                        ep2 = makenode(en_napccon, (void*)spi->tp->lst.head, 0);
                    }
                    else if (!(flag = checktype(spi->tp, sp->tp, FALSE)) ||
                        isstructured(sp->tp))
                    {
                        if (!(isscalar(sp->tp) && isscalar(spi->tp)))
                        if (prm_cplusplus)
                        {
                            TYP *tp1 = sp->tp,  *tp1i = spi->tp;
                            if (tp1->type == bt_ref)
                                tp1 = tp1->btp;
                            if (tp1i->type == bt_ref)
                                tp1i = tp1i->btp;
                            if (isstructured(tp1) && (!isstructured(tp1i) ||
                                strcmp(sp->name, tp1i->sp->name)))
                            {
                                {
                                    ENODE *dv;
                                    ENODE *qnode,  *rnode,  *xnode;
                                    SYM fsym,  *xs; /* fsym and ftype only used
                                        for error messages */
                                    TYP ftype;
                                    int hasdest = FALSE;
                                    dv = dummyvar(tp1->size, tp1, 0);
                                    xnode = makenode(en_void, ep2, 0);
                                    xs = sp; /* fsym and ftype only used for
                                        error messages */
                                    fsym.tp = sp->tp;
                                    fsym.next = 0;
                                    ftype.type = bt_func;
                                    ftype.lst.head = ftype.lst.tail = &fsym;
                                    qnode = do_constructor(dv->v.sp, &ftype,
                                        tp1, xnode, 1, 0, TRUE, TRUE);
                                    ftype.lst.head = ftype.lst.tail =  - 1;
                                    if (sp->tp->type == bt_ref)
                                    {
                                        rnode = do_destructor(dv->v.sp, &ftype,
                                            tp1, 0, 1, 0, FALSE);
                                    }
                                    else
                                    {
                                        if (qnode || search
                                            (cpp_funcname_tab[CI_DESTRUCTOR],
                                            &tp1->lst))
                                            rnode = makenode(en_destlabel,
                                                makeintnode(en_icon, 0),
                                                makenode(en_autocon, (void*)dv
                                                ->v.sp, 0));
                                        else
                                            rnode = 0;
                                    }
                                    if (rnode)
                                        expr_rundown[expr_updowncount - 1] =
                                            makenode(en_void, rnode,
                                            expr_rundown[expr_updowncount - 1]);
                                    if (qnode)
                                    {
                                        qnode->cflags |= DF_CALLRUNHEDGE;
                                        if (expr_runup[expr_updowncount - 1])
                                            expr_runup[expr_updowncount - 1] =
                                                makenode(en_void,
                                                expr_runup[expr_updowncount -
                                                1], qnode);
                                        else
                                            expr_runup[expr_updowncount - 1] =
                                                qnode;
                                    }
                                    if (qnode || rnode)
                                    {
                                        ep2 = dv;
                                        spi = copysym(dv->v.sp);
                                        spi->tp = xs->tp;
                                        spi->next = xs->next;
                                        genfuncerror(ERR_TEMPUSED, tp->sp->name,
                                            sp->name);
                                    }
                                    else
                                        dv->v.sp->nullsym = TRUE;
                                }
                            }
                            else if (tp1->type == bt_pointer && isstructured
                                (tp1->btp) && tp1i->type == bt_pointer &&
                                isstructured(tp1i->btp))
                            {
                                CLASSLIST *l = tp1i->btp->sp
                                    ->value.classdata.baseclass->link;
                                while (l)
                                {
                                    if (l->data->mainsym == tp1->btp->sp
                                        ->mainsym)
                                        break;
                                    l = l->link;
                                }
                                if (!l)
                                    genfuncerror(ERR_CALLMISMATCH, tp1->btp->sp
                                        ->name, tp1i->btp->sp->name);
                                cppcast(tp1, tp1i, &ep2, FALSE);
                            }
                        }
                        else if (!flag)
                        {
                            if (sp->tp->type != bt_pointer && sp->tp->type !=
                                bt_farpointer || !isintconst(ep2->nodetype))
                                if (spi->tp->type == bt_void)
                                    generror(ERR_NOVOID, 0, 0);
                                else
                                    genfuncerror(ERR_CALLMISMATCH, tp->sp->name,
                                        sp->name);
                                else if (ep2->v.i != 0)
                                    generror(ERR_MISMATCH, 0, 0);
                        }
                    }
                }
            }
            else if (spi->tp->type == bt_pointer && spi->tp->btp->type ==
                bt_func && ep2->nodetype == en_placeholder)
            {
                SYM *sp1 = funcovermatch(ep2->v.sp, spi->tp->btp, 0, FALSE,
                    FALSE);
                if (sp1)
                {
                    SetExtFlag(sp1, TRUE);
                    ep2 = makenode(en_napccon, (void*)sp1, 0);
                }
            }
            else if (spi->tp->type == bt_void)
                generror(ERR_NOVOID, 0, 0);
            else if (spi->tp->type == bt_float)
                promote_type(&stddouble, &ep2);
            else if (spi->tp->type == bt_fimaginary)
                promote_type(&stdrimaginary, &ep2);
            else if (spi->tp->type == bt_defunc)
            {
                SYM *sp1 = spi->tp->lst.head,  *sp2;
                while (sp1 && sp1->istemplate)
                    sp1 = sp1->next;
                if (sp1)
                {
                    sp2 = sp1->next;
                    while (sp2 && sp2->istemplate)
                        sp2 = sp2->next;
                    if (sp2)
                        genfunc2error(ERR_AMBIGFUNC, funcwithns(sp1),
                            funcwithns(sp2));
                    ep2 = makenode(en_napccon, (void*)sp1, 0);
                }
            }
        }
        if (sp && sp->tp->type == bt_ref)
        {
            if (!isintconst(ep2->nodetype) && !isfloatconst(ep2->nodetype))
            {
                if (prm_cplusplus)
                {
                    while (castvalue(ep2))
                        ep2 = ep2->v.p[0];
                    if (lvalue(ep2))
                        ep2 = ep2->v.p[0];
                }
                else
                if (isscalar(sp->tp->btp))
                {
                    while (castvalue(ep2))
                        ep2 = ep2->v.p[0];
                    if (spi && !(spi->tp->type == bt_ref || spi->tp->type ==
                        bt_pointer && !spi->tp->val_flag))
                        if (lvalue(ep2))
                            ep2 = ep2->v.p[0];
                }
            }
            else
            {
                ENODE *x;
                tp2 = sp->tp->btp;
                x = dummyvar(tp2->size, tp2, 0);
                deref(&x, tp2);
                ep2 = makenode(en_refassign, x, ep2);
                genfuncerror(ERR_TEMPUSED, tp->sp->name, sp->name);
            }
            ep1 = makenode(en_void, ep2, ep1);
        }
        else if (spi && spi != (SYM*) - 1 && spi->tp->val_flag && isstructured
            (spi->tp))
        {
            ep1 = sbll(spi, ep2, ep1);
        }
        else
        {
            if (sp && sp != (SYM*) - 1 && sp->tp->type != bt_ellipse)
            {
                TYP *tp = sp->tp;
                if (tp->type == bt_pointer && (tp->btp->val_flag &VARARRAY))
                    tp = tp->btp;
                if (tp->type == bt_pointer && (tp->val_flag &VARARRAY))
                {
                    memptr = tp->size;
                    if (lvalue(ep2))
                        ep2 = ep2->v.p[0];
                }
                else
                    promote_type(sp->tp, &ep2);
            }
            else if (ep2->nodetype == en_llcon || ep2->nodetype == en_llucon)
                promote_type(&stdlonglong, &ep2);
            else if (isintconst(ep2->nodetype))
                promote_type(&stdint, &ep2);
            else if (ep2->nodetype == en_fcon)
                promote_type(&stdfloat, &ep2);
            if (memptr)
            {
                ep1 = makenode(en_stackblock, ep2, ep1);
                ep2->size = memptr;
            }
            else
                ep1 = makenode(en_void, ep2, ep1);
        }

        spi = spi->next;
        if (sp && sp != (SYM*) - 1)
        {
            sp = sp->next;
            if (sp && sp->tp->type == bt_ellipse)
                matching = FALSE;
        }
    }
    if (tp)
    {
        if (!tp->btp)
            tp->btp = &stdint;
        promote_type(tp->btp, &ep1);
    }
    else
    {
        promote_type(tpi->btp, &ep1);
    }
    *node = ep1;
}

//-------------------------------------------------------------------------

int floatrecurse(ENODE *node)
/*
 * Go through a node and see if it will be promoted to type FLOAT
 */
{
    if (!node)
        return 0;
    switch (node->nodetype)
    {
        case en_structret:
            return 0;
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_cfi:
        case en_cri:
        case en_clri:
        case en_fcomplexcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_doubleref:
        case en_longdoubleref:
        case en_floatref:
        case en_cld:
        case en_cd:
        case en_cf:
            return 1;
        case en_substack:
		case en_loadstack:
		case en_savestack:
        case en_labcon:
        case en_trapcall:
        case en_nacon:
        case en_autocon:
        case en_autoreg:
        case en_nalabcon:
        case en_l_ref:
		case en_i_ref:
		case en_ui_ref:
		case en_a_ref: case en_ua_ref:
        case en_tempref:
        case en_napccon:
        case en_absacon:
        case en_cl:
        case en_regref:
        case en_ul_ref:
        case en_ll_ref:
        case en_ull_ref:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cll:
        case en_cull:
        case en_cp:
        case en_cfp:
        case en_csp:
        case en_icon:
        case en_boolcon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_ccon:
        case en_cucon:
        case en_llcon:
        case en_llucon:
        case en_bits:
        case en_fp_ref:
        case en_ub_ref:
        case en_cub:
        case en_b_ref:
        case en_bool_ref:
        case en_cb:
        case en_uw_ref:
        case en_cuw:
        case en_cw:
        case en_w_ref:
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
            return 0;
        case en_fcall:
        case en_pfcall:
        case en_sfcall:
        case en_sfcallb:
        case en_fcallb:
        case en_pfcallb:
        case en_callblock:
            return (floatrecurse(node->v.p[1]));
        case en_not:
        case en_compl:
        case en_uminus:
        case en_asuminus:
        case en_ascompl:
        case en_cl_reg:
        case en_moveblock:
        case en_stackblock:
        case en_movebyref:
        case en_clearblock:
            return floatrecurse(node->v.p[0]);
        case en_refassign:
        case en_assign:
        case en_lassign:
        case en_ainc:
        case en_adec:
        case en_add:
        case en_sub:
        case en_addstruc:
        en_addcast: case en_umul:
        case en_udiv:
        case en_umod:
        case en_pmul:
		case en_arrayindex:
        case en_mul:
        case en_div:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_asalsh:
        case en_asarsh:
        case en_alsh:
        case en_arsh:
        case en_arshd:
        case en_asarshd:
        case en_lsh:
        case en_rsh:
        case en_land:
        case en_lor:
        case en_asadd:
        case en_assub:
        case en_asmul:
        case en_asdiv:
        case en_asmod:
        case en_asand:
        case en_asumod:
        case en_asudiv:
        case en_asumul:
        case en_asor:
        case en_aslsh:
        case en_asxor:
        case en_asrsh:
        case en_repcons:
        case en_void:
        case en_dvoid:
		case en_array:
            return (floatrecurse(node->v.p[0]) || floatrecurse(node->v.p[1]));
        case en_cond:
            return floatrecurse(node->v.p[1]);
        case en_conslabel:
        case en_destlabel:
            return floatrecurse(node->v.p[0]);
    }
    return (0);
}

//-------------------------------------------------------------------------

void floatcheck(ENODE *node)
/*
 * Error if node will be promoted to type float
 */
{
    if (floatrecurse(node))
        generror(ERR_INVFLOAT, 0, 0);
}

//-------------------------------------------------------------------------

int castbegin(int st)
/*
 *      return 1 if st in set of [ kw_char, kw_short, kw_long, kw_int, kw_bool
 *      kw_float, kw_double, kw_struct, kw_union, kw_float, or is typedef ]
 */
{
    SYM *sp;
    switch (st)
    {
        case kw_typename:
        case kw_typeof:
        case kw_void:
        case kw_char:
        case kw_short:
        case kw_int:
        case kw_long:
        case kw_float:
        case kw_double:
        case kw_bool:
        case kw_struct:
        case kw_union:
        case kw_signed:
        case kw_unsigned:
        case kw_volatile:
        case kw_const:
        case kw_wchar_t:
        case kw_class:
        case kw___int64:
		case kw_enum:
            return 1;
        default:
            if (st != id)
                return 0;
    }
    if (prm_cplusplus)
    {
        if (lastst == id)
        {
            SYM *typesp;
            if (typesp = typesearch(lastid))
            {
                char buf[512];
                if (typesp && typesp->istemplate)
                {
                    int v = lastst;
                    if (v == id)
                        strcpy(buf, lastid);
                    getsym();
                    if (lastst == lt)
                    {
                        typesp = lookupTemplateType(typesp,0);
                        typequal = typesp;
                        v = gt;
                    }
                    else
                        if (v == classsel)
                            gensymerror(ERR_NEEDSPECIAL, typesp->name);
                    backup(v);
                    if (v == id)
                        strcpy(lastid, buf);
                    if (!typesp)
                        return FALSE;
                }
                if (typesp->storage_class == sc_type || typesp->storage_class 
                    == sc_namespace)
                {
                    if (typesp->istemplate || typesp->istemplatevar)
                        return TRUE;
                    strcpy(buf, lastid);
                    getsym();
                    if (lastst == classsel)
                    {
                        typesp = parsetype(typesp, FALSE);
                        if (typesp && typesp->storage_class == sc_type)
                            return TRUE;
                    }
                    else if (lastst == openpa)
                    {
                        backup(id);
                        strcpy(lastid, buf);
                        return FALSE;
                    }
                    else
                    {
                        backup(id);
                        strcpy(lastid, buf);
                        return TRUE;
                    }
                }
            }
            return FALSE;
        }
    }
    nm = lastid;
    sp = typesearch(lastid);

    return sp && sp->storage_class == sc_type;
}

//-------------------------------------------------------------------------

int tostring(int *elems)
{
    char string[MAX_STRLEN];
    int st = lastst;
    int size = 0;
    string[0] = 0;
    while (lastst == sconst || lastst == lsconst)
    {
        if (st == lsconst)
        {
            if (laststrlen + size + 2 > MAX_STRLEN/2)
            {
                generror(ERR_STRINGTOOBIG, 0, 0);
                break;
            }
            if (lastst == lsconst)
                {
                    memcpy(string + size * 2, laststr, 2 *laststrlen);
                    ((short*)string)[size + laststrlen] = 0;
                }
            else {
                char *p = laststr;
                int i;
                for (i=0; i < laststrlen+1; i++)
                    ((short *)string)[size+i] = *p++;
            }
        }
        else
        {
            if (laststrlen + size + 1> MAX_STRLEN)
            {
                generror(ERR_STRINGTOOBIG, 0, 0);
                break;
            }
            if (lastst == sconst)
            {
                memcpy(((char*)string) + size, laststr, laststrlen);
                ((char*)string)[size + laststrlen] = 0;
            } else {
                short *p = laststr;
                int i;
                for (i=0; i < laststrlen+1; i++)
                    string[size+i] = *p++;
            }
        }
        size += laststrlen;
        getsym();
    }
    *elems = size + 1;
    return (stringlit(string, st == lsconst, size + 1));
}

//-------------------------------------------------------------------------

ENODE *onefunc(ENODE *pnode, TYP **tptr, ENODE *qnode, TYP *tp1)
{
    int continuetc = FALSE, hasover = FALSE, structured = FALSE;
    SYM *sp;
    TYP *tp;
    ENODE *tcnode = 0,  *rnode,  *tcnode2;
    /* Now a check for overloaded () */
        if (pnode->nodetype == en_thiscall)
        {
            if (pnode->cflags &DF_HASOVERRIDE)
                hasover = TRUE;
            tcnode = pnode->v.p[0];
            pnode = pnode->v.p[1];
            //      continuetc = TRUE ;
        }
    tp =  *tptr;
    if (tp->type != bt_defunc && tp->type != bt_func && tp->type != bt_ifunc &&
        (tp->type != bt_memberptr && tp->type != bt_pointer && tp->type !=
        bt_farpointer || tp->btp->type != bt_func))
    {
        if (tp->type != bt_matchall)
            gensymerrorexp(ERR_NOFUNC, nm);
        pnode = makenode(en_nacon, &undef, 0);
    }
    else
    {

        int notbase = 0;
            if (prm_cplusplus && (*tptr)->sp)
            {
                char *p = (*tptr)->sp->name;
                if (prm_cmangle && (*p == '_'))
                    p++;
                if (!strcmp(p, "main"))
                    generror(ERR_NOMAIN, 0, 0);
            }
            if ((*tptr)->type == bt_defunc)
            {
                nm = fullcppmangle((*tptr)->sp->parentclass, (*tptr)->sp->name,
                    tp1);
                sp = funcovermatch((*tptr)->sp, tp1, qnode, FALSE, FALSE);
                if (!sp || sp->istemplate)
                {
                    sp = lookupTemplateFunc((*tptr)->sp, tp1, nm);
                    if (!sp)
                    {
                        //    	           gensymerror(ERR_NOFUNCMATCH,nm);
                        pnode = makenode(en_nacon, &undef, 0);
                        goto exit;
                    }
                }
                checkaccess: if (!isaccessible(sp) && (!(*tptr)->sp
                    ->parentclass || !isfriend(sp, &(*tptr)->sp->parentclass
                    ->tp->lst)))
                    genclasserror(ERR_NOTACCESSIBLE, sp->name);
                sp->mainsym->extflag = TRUE;
                if (!tcnode && sp->parentclass && declclass)
                {
                    CLASSLIST *l = declclass->value.classdata.baseclass;
                    while (l)
                    {
                        if (!strcmp(l->data->name, sp->parentclass->name))
                        {
                            tcnode = makenode(en_a_ref, copynode(thisenode), 0);
                            break;
                        }
                        l = l->link;
                    }
                }
                if (tcnode)
                {
                    if (sp->value.classdata.vtaboffs)
                        tcnode = makenode(en_addstruc, tcnode, makeintnode
                            (en_icon, sp->value.classdata.vtaboffs));
                }
                tcnode2 = tcnode;
                if (tcnode2 && tcnode2->nodetype == en_a_ref)
                {
                    tcnode2 = tcnode2->v.p[0];
                    if (tcnode2->v.sp != en_autocon)
                        tcnode2 = 0;
                }
                else
                    tcnode2 = 0;
                if ((sp->value.classdata.cppflags &PF_VIRTUAL) && !hasover &&
                    !notbase && (tcnode->nodetype == en_a_ref || tcnode
                    ->nodetype == en_ua_ref))
                {
                    ENODE *inode,  *nnode;
                    inode = makeintnode(en_icon, sp->value.classdata.vtabindex);
                    nnode = makenode(en_ua_ref, copy_enode(tcnode), 0); 
                        // it is a ulref to prevent the inlining from optimizing it away
                    pnode = makenode(en_add, inode, nnode);
                    pnode = makenode(en_a_ref, pnode, 0);
                    //            if (tcnode && icon)
                    //               tcnode = makenode(en_addstruc,tcnode,makeintnode(en_icon,icon)) ;
                }
                else
                if (sp->tp->type == bt_memberptr || sp->tp->type == bt_pointer 
                    || sp->tp->type == bt_farpointer)
                {
                    pnode = makenode(en_nacon, sp, 0);
                    deref(&pnode, sp->tp);
                }
                else
                {
                    pnode = makenode(en_napccon, sp, 0);
                }
                (*tptr) = sp->tp;
            }
            else
        {
            if ((*tptr)->type == bt_memberptr)
                sp = (*tptr)->btp->sp;
            else
            if (prm_cplusplus)
            {
                sp = funcovermatch((*tptr)->sp, tp1, qnode, FALSE, FALSE);
                if (sp)
                {
                    if (sp->istemplate)
                        sp = lookupTemplateFunc((*tptr)->sp, tp1, nm);
                    if (sp->tp->type == bt_memberptr || sp->tp->type ==
                        bt_pointer || sp->tp->type == bt_farpointer)
                    {
                        pnode = makenode(en_nacon, sp, 0);
                        deref(&pnode, sp->tp);
                    }
                    else
                        pnode = makenode(en_napccon, sp, 0);
                    (*tptr) = sp->tp;
                }
                else
                    sp = (*tptr)->sp;
            }
            else if ((*tptr)->type == bt_pointer)
                sp = (*tptr)->btp->sp;
            else
                sp = (*tptr)->sp;
        }
            foundfunc: 
            /* Check if (*tptr) is a base class of the current func */
            if (prm_cplusplus && sp->parentclass)
            {
                SYM *sp1 = declclass;
                if (sp1)
                {
                    CLASSLIST *l;
                    sp1 = sp1->mainsym;
                    l = sp1->value.classdata.baseclass;
                    notbase = TRUE;
                    while (sp1)
                    {
                        if (sp1 == sp->parentclass)
                        {
                            notbase = FALSE;
                            goto finish;
                        }
                        if (!l)
                            sp1 = 0;
                        else
                        {
                            sp1 = l->data;
                            l = l->link;
                        }
                    }
                }
                if (sp && (sp->tp->type != bt_pointer) && (*tptr)->type !=
                    bt_memberptr && ((sp->value.classdata.cppflags &PF_MEMBER)
                    && !(sp->value.classdata.cppflags &PF_STATIC) && (!tcnode 
                    || tcnode->nodetype == en_icon) && ((!declclass || notbase)
                    || (currentfunc && (currentfunc->value.classdata.cppflags
                    &PF_MEMBER) && (currentfunc->value.classdata.cppflags
                    &PF_STATIC)))))
                    if (!strstr(sp->name, "$bcall"))
                        gensymerror(ERR_NONSTATICMUSTCLASS, sp->name);
            }
        finish: sp->mainsym->extflag = TRUE;
        /* make a function node */
        parmlist(&qnode, tp1, (*tptr));
        rnode = makenode(en_void, pnode, makenode(en_napccon, sp, 0)); 
            // might not be a napccon but we don't care
        pnode = makenode(en_void, rnode, qnode);
        tp = *tptr;
        while (tp->type != bt_func && tp->type != bt_ifunc)
            tp = tp->btp;
        rnode = makeintnode(en_icon, retvalsize(tp->btp));
        if ((*tptr)->type != bt_memberptr)
        {
            if ((*tptr)->type == bt_pointer || (*tptr)->type == bt_farpointer)
            {
                (*tptr) = (*tptr)->btp;
                continuetc = TRUE;
            }
            (*tptr) = (*tptr)->btp;
        }

        if (sp->intflag || sp->faultflag)
            pnode = makenode(en_intcall, qnode, pnode);
        else
        if (isstructured((*tptr)))
        {
            if (sp->pascaldefn)
                pnode = makenode(en_pfcallb, rnode, pnode);
            else
                if (sp->isstdcall)
                    pnode = makenode(en_sfcallb, rnode, pnode);
                else
                    pnode = makenode(en_fcallb, rnode, pnode);
            pnode->size = (*tptr)->size;
            structured = TRUE;
        }
        else
            if (sp->pascaldefn)
                pnode = makenode(en_pfcall, rnode, pnode);
            else
                if (sp->isstdcall)
                    pnode = makenode(en_sfcall, rnode, pnode);
                else
                    pnode = makenode(en_fcall, rnode, pnode);
            if (prm_cplusplus && tcnode && (continuetc || (sp
                ->value.classdata.cppflags &PF_MEMBER) && !(sp
                ->value.classdata.cppflags &PF_STATIC)))
                pnode = makenode(en_thiscall, tcnode, pnode);
            if (structured && (lastst == dot || lastst == pointsto))
                pnode = immed_callblock(0, pnode,  *tptr, (*tptr)->size);
        pnode = doinline(pnode);
        pnode->cflags = (*tptr)->cflags;
    }
    exit: return pnode;
}

//-------------------------------------------------------------------------

ENODE *complex_functions(ENODE *pnode, TYP **tptr, ENODE *qnode, TYP *tp1)
{
    TYP **tp3,  **tp4;
    ENODE *ep3,  *ep4;
    if ((*tptr)->type != bt_cond)
        return onefunc(pnode, tptr, qnode, tp1);

    ep3 = pnode->v.p[1]->v.p[0];
    ep4 = pnode->v.p[1]->v.p[1];
    tp3 = &(*tptr)->lst.head;
    tp4 = &(*tptr)->lst.tail;
    ep3 = complex_functions(ep3, tp3, qnode, tp1);
    ep4 = complex_functions(ep4, tp4, qnode, tp1);
    pnode->v.p[1]->v.p[0] = ep3;
    pnode->v.p[1]->v.p[1] = ep4;
    return pnode;

}

//-------------------------------------------------------------------------

TYP *doalloca(ENODE **node)
{
    TYP *tp = 0, *tp1 ;
    ENODE *ep, *ep1, *epd ;
	if (!currentfunc)
		generror(ERR_ALLOCA,0,0);
    if (needpunc(openpa, 0))
    {
        tp1 = exprnc(&ep);
        if (tp1)
        {
            if (!scalarnonfloat(tp1)) 
            {
                ep = makenode(en_icon,0,0);
                generror(ERR_SELECTSCALAR,0,0);
            }
            ep = makenode(en_substack, ep, 0);
                            
            *node = ep;
			
			used_alloca = TRUE;
            needpunc(closepa, 0);
        } else
            *node = makenode(en_icon,0, 0) ;
        tp1 = maketype(en_void, 0);
        tp = maketype(bt_pointer, stdaddrsize);
        tp->btp = tp1 ;
    } else
        *node = makenode(en_icon,0, 0) ;
    return tp ;
}

//-------------------------------------------------------------------------

static void newoverload(enum overloadop op, TYP *tp, TYP *decl, ENODE **ep)
{
    SYM *sp1,  *sp = 0;
    ENODE *pnode,  *rnode;
    if (isstructured(decl))
    {
        sp1 = search(cpp_funcname_tab[op], &decl->lst);
        if (sp1)
            sp = funcovermatch(sp1, tp, 0, FALSE, FALSE);
    } if (!sp)
    {
        sp1 = search(cpp_funcname_tab[op], gsyms);
        if (!sp1)
        {
            gensymerrorexp(ERR_NOFUNCMATCH, fullcppmangle(0,
                cpp_funcname_tab[op], tp));
            *ep = makenode(en_icon, 0, 0);
            return ;
        }
        sp = funcovermatch(sp1, tp, 0, FALSE, FALSE);
        if (!sp)
        {
            gensymerrorexp(ERR_NOFUNCMATCH, fullcppmangle(0,
                cpp_funcname_tab[op], tp));
            *ep = makenode(en_icon, 0, 0);
            return ;
        }
    }
    sp->mainsym->extflag = TRUE;
    pnode = makenode(en_napccon, sp, 0);
    parmlist(ep, tp, sp->tp);
    rnode = makenode(en_void, pnode, pnode);
    pnode = makenode(en_void, rnode,  *ep);
    rnode = makeintnode(en_icon, sp->tp->btp->size);
    if (sp->pascaldefn)
        pnode = makenode(en_pfcall, rnode, pnode);
    else
        if (sp->isstdcall)
            pnode = makenode(en_sfcall, rnode, pnode);
        else
            pnode = makenode(en_fcall, rnode, pnode);
    pnode = doinline(pnode);
    *ep = pnode;
}

//-------------------------------------------------------------------------

static void newcons(ENODE **node, TYP *tp, int array, ENODE *size, TYP *initype,
    ENODE *initnode)
{
    ENODE *dummy;
    ENODE *pnode,  *rnode,  *xnode,  *tcnode = 0;
    if (isstructured(tp->btp))
    {
        TYP *typ = tp->btp;
        SYM *sp1,  *sp;
        sp1 = search(cpp_funcname_tab[CI_CONSTRUCTOR], &typ->lst);
        dummy = dummyvar(stdaddrsize, &stdint, 0); // fixme 
        dummy = makenode(en_a_ref, (void*)dummy, 0);
        xnode = makenode(en_assign, (void*)dummy,  *node);
        if (!sp1)
        {
            if (size)
            {
                ENODE *snode = makenode(en_assign, makenode(en_a_ref, dummy, 0),
                    size);
                snode = makenode(en_void, snode, makenode(en_asadd, dummy,
                    makeintnode(en_icon, stdintsize)));
                xnode = makenode(en_void, xnode, dummy);
                *node = makenode(en_voidnz, xnode, snode);
            }
            return ;
        }
        sp = funcovermatch(sp1, initype, 0, FALSE, FALSE);
        if (!sp)
        {
            gensymerrorexp(ERR_NOFUNCMATCH, fullcppmangle(typ->sp,
                cpp_funcname_tab[CI_CONSTRUCTOR], typ));
            *node = makenode(en_icon, 0, 0);
            return ;
        }
        pnode = makenode(en_napccon, sp, 0);
        sp->mainsym->extflag = TRUE;
        parmlist(&initnode, initype, sp->tp);
        rnode = makenode(en_void, pnode, pnode);
        pnode = makenode(en_void, rnode, initnode);
        rnode = makeintnode(en_icon, sp->tp->btp->size);
        if (sp->pascaldefn)
            pnode = makenode(en_pfcall, rnode, pnode);
        else
            if (sp->isstdcall)
                pnode = makenode(en_sfcall, rnode, pnode);
            else
                pnode = makenode(en_fcall, rnode, pnode);
        if (!(sp->value.classdata.cppflags &PF_STATIC))
        {
            tcnode = copy_enode(dummy);
            pnode = makenode(en_thiscall, tcnode, pnode);
        }
        if (!array)
            pnode = doinline(pnode);
        else
        {
            ENODE *snode = makeintnode(en_icon, typ->size);
            snode = makenode(en_void, size, snode);
            pnode = makenode(en_repcons, (void*)snode, (void*)pnode);
            SetExtFlag(sp, TRUE);
            snode = makenode(en_asadd, dummy, makeintnode(en_icon, stdintsize));
            pnode = makenode(en_void, snode, pnode);
            rnode = makenode(en_a_ref, dummy, 0);
            rnode = makenode(en_assign, rnode, copy_enode(size));
            pnode = makenode(en_void, rnode, pnode);
        }
        xnode = makenode(en_void, xnode, dummy);
        *node = makenode(en_voidnz, xnode, pnode);

    }
    else
    {
        ENODE *snode;
        dummy = dummyvar(stdaddrsize, &stdint, 0); // fixme 
        dummy = makenode(en_a_ref, (void*)dummy, 0);
        xnode = makenode(en_assign, (void*)dummy,  *node);
        snode = makenode(en_assign, makenode(en_a_ref, dummy, 0), size);
        snode = makenode(en_void, snode, makenode(en_asadd, dummy, makeintnode
            (en_icon, stdintsize)));
        xnode = makenode(en_void, xnode, dummy);
        *node = makenode(en_voidnz, xnode, snode);
    }
}

//-------------------------------------------------------------------------

static void newdest(ENODE **node, TYP *tp, int array, ENODE *deadptr)
{
    ENODE *xnode;
    if (tp->btp && isstructured(tp->btp))
    {
        TYP *typ = tp->btp, nulltype;
        SYM *sp1 = search(cpp_funcname_tab[CI_DESTRUCTOR], &typ->lst),  *sp;
        ENODE *pnode,  *rnode,  *xnode,  *tcnode = 0;
        if (!sp1)
        {
            goto join;
        }
        memset(&nulltype, 0, sizeof(nulltype));
        nulltype.lst.head = nulltype.lst.tail =  - 1;
        nulltype.type = bt_func;
        sp = funcovermatch(sp1, &nulltype, 0, FALSE, FALSE);
        if (!sp)
        {
            goto join;
        }
        pnode = makenode(en_napccon, sp, 0);
        sp->mainsym->extflag = TRUE;
        xnode = makenode(en_void, 0, 0);
        parmlist(&xnode, &nulltype, sp->tp);
        rnode = makenode(en_void, pnode, pnode);
        pnode = makenode(en_void, rnode, xnode);
        rnode = makeintnode(en_icon, sp->tp->btp->size);
        if (sp->pascaldefn)
            pnode = makenode(en_pfcall, rnode, pnode);
        else
            if (sp->isstdcall)
                pnode = makenode(en_sfcall, rnode, pnode);
            else
                pnode = makenode(en_fcall, rnode, pnode);
        if (!(sp->value.classdata.cppflags &PF_STATIC))
        {
            tcnode = copy_enode(deadptr);
            pnode = makenode(en_thiscall, tcnode, pnode);
        }
        if (!array)
            pnode = doinline(pnode);
        else
        {
            ENODE *snode = makeintnode(en_icon, typ->size);
            ENODE *size = makenode(en_addstruc, deadptr, makeintnode(en_icon,  
                - stdintsize));
            size = makenode(en_a_ref, size, 0);
            snode = makenode(en_void, size, snode);
            pnode = makenode(en_repcons, (void*)snode, (void*)pnode);
            SetExtFlag(sp, TRUE);
        }
        *node = makenode(en_void, pnode,  *node);
        if (tcnode)
        {
            xnode = makenode(en_void, tcnode, makeintnode(en_icon, 0));
            *node = makenode(en_voidnz, xnode,  *node);
        }
    }
    else
    {
        join: xnode = makenode(en_void, deadptr, makenode(en_icon, 0, 0));
        *node = makenode(en_voidnz, xnode,  *node);
    }
}

//-------------------------------------------------------------------------

TYP *donew(ENODE **node)
{
    SYM *newt;
    int doarray = FALSE;
    TYP *tptr,  *tp;
    TYP *prmtyp = 0;
    TYP *sizetype = 0;
    TYP *inittyp = 0;
    ENODE *prms = 0;
    ENODE *size = 0;
    ENODE *init;
    ENODE *fullsize = 0;
    if (lastst == openpa)
    {
        goodcode |= GF_ASSIGN;
        getsym();
        prmtyp = gatherparms(&prms, FALSE);
        parm_ns_level--;
    }
    nodeclfunc = TRUE;
    declid[0] = 0;
    typequal = 0;
    decl(0, 0); /* do cast declaration */
    headptr = &head;
    decl1(sc_type, 0);
    nodeclfunc = FALSE;
    if (declid[0])
        generror(ERR_NODECLARE, 0, 0);
    if (!head || defaulttype && head->type == bt_int)
    // || !isstructured(tp))
        generror(ERR_TYPENAMEEXP, 0, 0);
    if (head->type == bt_consplaceholder)
        tptr = declclass->tp;
    else
        tptr = head;
    if (lastst == openbr)
    {
        int total = 1;
        doarray = TRUE;
        getsym();
        sizetype = exprnc(&size);
        needpunc(closebr, 0);
        while (lastst == openbr)
        {
            ENODE *xsize = 0;
            getsym();
            exprnc(&xsize);
            if (!xsize)
            {
                generror(ERR_NEEDCONST, 0, 0);
                break;
            }
            else
            {
                ENODE *ysize = xsize;
                while (castvalue(ysize))
                    ysize = ysize->v.p[0];
                if (isintconst(ysize->nodetype))
                    size = makenode(en_umul, (void*)size, (void*)xsize);
                else
                    generror(ERR_NEEDCONST, 0, 0);
            }
            needpunc(closebr, 0);
        }
        asnfit(&size, sizetype, &size, &stdunsigned, FALSE);
    }
    if (lastst == openpa)
    {
        getsym();
        inittyp = gatherparms(&init, FALSE);
        parm_ns_level--;
        if (doarray)
            generror(ERR_NEWARRAYINIT, 0, 0);
    }
    else
    {
        inittyp = maketype(bt_func, 0);
        inittyp->lst.head = inittyp->lst.tail =  - 1;
        init = makenode(en_void, 0, 0);
    }
    if (!prmtyp)
    {
        prmtyp = maketype(bt_func, 0);
        prmtyp->btp = maketype(bt_pointer, stdaddrsize);
        prmtyp->btp->btp = maketype(bt_void, 0);

    }
    newt = makesym(0);
    newt->tp = maketype(bt_unsigned, stdaddrsize);
    newt->next = 0;
    newt->name = 0;
    fullsize = makeintnode(en_icon, tptr->size);
    if (size)
        fullsize = makenode(en_mul, (void*)fullsize, (void*)size);
    if (doarray)
        fullsize = makenode(en_add, (void*)fullsize, makeintnode(en_icon,
            stdaddrsize));
    if (!prmtyp->lst.head || prmtyp->lst.head ==  - 1)
    {
        prmtyp->lst.head = prmtyp->lst.tail = newt;
        prms = makenode(en_void, (void*)fullsize, 0);
    }
    else
    {
        newt->next = prmtyp->lst.head;
        prmtyp->lst.head = newt;
        prms = makenode(en_void, (void*)fullsize, (void*)prms);
    }
    *node = prms;
    newoverload(doarray ? ov_newa : ov_new, prmtyp, tptr, node);
    tp = maketype(bt_pointer, stdaddrsize);
    tp->btp = tptr;
    newcons(node, tp, doarray, copy_enode(size), inittyp, init);
    return tp;
}

//-------------------------------------------------------------------------

TYP *dodelete(ENODE **node)
{
    SYM *newt;
    int opened = FALSE, doarray = FALSE;
    TYP *decl = 0;
    TYP *func;
    ENODE *decle = 0;
    goodcode |= GF_ASSIGN;
    if (lastst == openbr)
    {
        doarray = TRUE;
        getsym();
        needpunc(closebr, 0);
    }
    decl = exprnc(&decle);
    if (decl->type != bt_pointer && decl->type != bt_farpointer && decl->type 
        != bt_segpointer)
        generror(ERR_NOPOINTER, 0, 0);
    func = maketype(bt_func, 0);
    newt = makesym(0);
    newt->tp = maketype(bt_pointer, stdaddrsize);
    newt->tp->btp = maketype(bt_void, 0);
    newt->next = 0;
    newt->name = 0;
    func->lst.head = func->lst.tail = newt;

    *node = copy_enode(decle);
    if (doarray)
        *node = makenode(en_add,  *node, makeintnode(en_icon,  - stdintsize));
    *node = makenode(en_void,  *node, 0);
    newoverload(doarray ? ov_deletea : ov_delete, func, decl->type ==
        bt_pointer ? decl->btp: decl, node);
    newdest(node, decl, doarray, decle);
    return newt->tp->btp;
}

//-------------------------------------------------------------------------

TYP *docppcasts(enum e_st st, ENODE **node)
{
    TYP *newtype = 0,  *oldtype;
    getsym();
    if (needpunc(lt, 0))
    {
        declid[0] = 0;
        typequal = 0;
        decl(0, 0); /* do cast declaration */
        headptr = &head;
        decl1(sc_type, 0);
        newtype = head;
        if (needpunc(gt, 0) && needpunc(openpa, 0))
        {
            oldtype = exprnc(node);
            if (needpunc(closepa, 0))
            {
                if (newtype->type == bt_pointer && oldtype->type == bt_pointer
                    && isstructured(newtype->btp) && isstructured(oldtype->btp))
                {
                    if (st == kw_static_cast)
                    {
                        if (!cppcast(newtype, oldtype, node, TRUE))
                            return  &stdmatch;
                    } 
                    else
                    {
                        SYM *sp = newtype->btp->sp;
                        SYM *s1 = oldtype->btp->sp;
                        CLASSLIST *l = s1->value.classdata.baseclass;
                        while (l)
                        {
                            if (sp->mainsym == l->data->mainsym)
                            {
                                if (l->offset)
                                    *node = makenode(en_addcast,  *node,
                                        makeintnode(en_icon, l->offset));
                                break;
                            }
                            l = l->link;
                        }
                        if (l)
                            cppcast(newtype, oldtype, node, TRUE);
                        else
                        {
                            if (!prm_xcept)
                                generror(ERR_NOXCEPT, 0, 0);
                            else if (oldtype->btp->sp
                                ->value.classdata.baseclass->flags &CL_VTAB)
                            {
                                ENODE *pnode = makenode(en_napccon, (void*)
                                    dynamic_cast_func, 0);
                                ENODE *rnode =  *node;
                                ENODE *snode;
                                dynamic_cast_func->mainsym->extflag = TRUE;
                                snode = makenode(en_void, rnode, 0);
                                rnode = makenode(en_napccon, (void*)getxtsym
                                    (oldtype->btp), 0);
                                snode = makenode(en_void, rnode, snode);
                                rnode = makenode(en_napccon, (void*)getxtsym
                                    (newtype->btp), 0);
                                snode = makenode(en_void, rnode, snode);
                                snode = makenode(en_void, snode, 0);
                                pnode = makenode(en_void, pnode, pnode);
                                snode = makenode(en_void, pnode, snode);
                                snode = makenode(en_fcall, makeintnode(en_icon,
                                    stdaddrsize), snode);
                                *node = snode;
                            }
                            else
                            {
                                char buf[256];
                                typenum2(buf, oldtype->btp);
                                gensymerror(ERR_TYPENOVIRTFUNC, buf);
                            }
                        }
                    }
                }
                else
                {
                    genmismatcherror(oldtype, newtype);
                }
            }
        }
    }
    return newtype;
}

//-------------------------------------------------------------------------

TYP *dotypeid(ENODE **node)
{
    SYM *sp;
    TYP *tp;
    if (!prm_xcept)
        generror(ERR_NOXCEPT, 0, 0);
    getsym();
    if (needpunc(openpa, skm_closepa))
    {
        ENODE *temp;
        if (castbegin(lastst)) {
            declid[0] = 0;
            typequal = 0;
            decl(0, 0); /* do cast declaration */
            headptr = &head;
            decl1(sc_type, 0);
            tp = head;
        } else
            tp = exprnc(&temp);
        if (tp)
        {
            if (tp->type == bt_pointer && isstructured(tp->btp) && (tp->btp->sp
                ->value.classdata.baseclass->flags &CL_VTAB))
            {
                TYP *tp1 = copytype(&stdlonglong, 0);
                ENODE *arg1,  *arg2,  *pnode;
                tp1->size = 2 * stdaddrsize + sizeof(CLASSXCEPT);
                arg1 = dummyvar(tp1->size, tp1, 0); // fixme
                arg2 = temp;
                pnode = makenode(en_void, arg2, 0);
                pnode = makenode(en_void, arg1, pnode);
                pnode = makenode(en_void, pnode, 0);
                temp = makenode(en_napccon, (void*)get_typeinfo_func, 0);
                get_typeinfo_func->mainsym->extflag = TRUE;
                temp = makenode(en_void, temp, temp);
                pnode = makenode(en_void, temp, pnode);
                pnode = makenode(en_fcall, makeintnode(en_icon, stdaddrsize), pnode);
                *node = pnode;
            }
            else
            {
                global_flag++;
                sp = makesym(sc_label);
                sp->tp = &stdint;
                global_flag--;
                sp->value.i = nextlabel++;
                *node = makenode(en_labcon, 0, 0);
                (*node)->v.i = nextlabel - 1;
                insertRTTI(sp, tp);
            }
        }
        else
            *node = makenode(en_icon, 0, 0);
        needpunc(closepa, skm_closepa);
    }
    if (!typeinfo_class)
    {
        SYM *nstd = search2("_std", gsyms, TRUE, FALSE);
        if (nstd && nstd->storage_class == sc_namespace)
            typeinfo_class = namespace_search("_type_info", nstd
                ->value.classdata.parentns, TRUE);
        if (!typeinfo_class)
        {
            //         generror(ERR_TYPEINFOUNDEF,0,0) ;
            tp = &stdint;
        }
    }
    if (typeinfo_class)
        tp = copytype(typeinfo_class->tp, DF_CONST);
    return tp;
}

//-------------------------------------------------------------------------

TYP *dooperator(ENODE **node)
{
    char *q = lastid;
    int aftertype = 0;
    TYP *rtype = 0;
    if (castbegin(lastst))
    {
        aftertype = 1;
        decl(0, 0); /* do cast declaration */
        headptr = &head;
        decl1(sc_type, 0);
        if (head->type == bt_func || head->type == bt_ifunc)
        {
            backup(openpa);
        }
        else
            goto error;
    }
    if (lastst >= id)
    {
        error: generror(ERR_OPERATOREXPECTED, 0, skm_declend);
        *node = makenode(en_icon, 0, 0);
        return  &stdmatch;
    }
    else
    {
        SYM *sp;
        int type = lastst + IT_OV_THRESHOLD;
        getsym(); /* past the operator */
        /* special case operator () and [] since they are two symbols */
        switch (type)
        {
            case ov_ind:
                if (lastst != closebr)
                    goto error;
                if (aftertype)
                    goto error;
                getsym();
                break;
            case ov_new:
                if (aftertype)
                    goto error;
                if (lastst == openbr)
                {
                    sp->value.i = ov_newa;
                    getsym();
                    needpunc(closebr, 0);
                }
                break;
            case ov_arg:
                break;
            case ov_delete:
                if (aftertype)
                    goto error;
                if (lastst == openbr)
                {
                    sp->value.i = ov_deletea;
                    getsym();
                    needpunc(closebr, 0);
                }
                break;
            default:
                if (aftertype)
                    goto error;
                break;
        }
        /* special case name for overloaded cast operators */

        if (type == ov_arg)
        {
            int temp1 = head->type;
            int temp2 = head->btp->cflags;
            head->btp->cflags = 0;
            q[0] = '$';
            q[1] = 'o';
            cpponearg(q + 2, head->btp);
            head->btp->cflags = temp2;
            rtype = head;
        }
        else
            strcpy(q, cpp_funcname_tab[type]);
        sp = search(q, gsyms);
        if (sp)
        {
            if (lastst != openpa && lastst != lt)
                generror(ERR_PUNCT, openpa, skm_closepa);
            else
            {
                *node = makenode(en_placeholder, (void*)sp, 0);
                return sp->tp;
            }
        }

    }
    *node = makenode(en_icon, 0, 0);
    generror(ERR_OPERATORNOTDEFINED, 0, 0);
    return  &stdmatch;
}

//-------------------------------------------------------------------------

void dothrow(ENODE **node)
{
    ENODE *args = 0,  *rnode;
    SYM *func;
    goodcode |= GF_THROW | GF_ASSIGN;
    getsym();
    if (!prm_xcept)
        generror(ERR_NOXCEPT, 0, 0);
    if (lastst != semicolon)
    {
        ENODE *ep1,  *ep2;
        TYP *tp = exprnc(&ep1);
        func = throw_func;
        if (lvalue(ep1) || isstructured(tp))
        {
            if (!tp->val_flag)
            {
                while (castvalue(ep1))
                    ep1 = ep1->v.p[0];
                ep1 = ep1->v.p[0];
            }
        }
        else
        {
            ENODE *x = dummyvar(tp->size, tp, 0);
            deref(&x, tp);
            ep1 = makenode(en_assign, x, ep1);
            ep1 = makenode(en_void, ep1, x->v.p[0]);
        }
        args = makenode(en_napccon, (void*)getxtsym(tp), 0);
        args = makenode(en_void, args, 0);
        args = makenode(en_void, ep1, args);
        args = makenode(en_void, makeintnode(en_icon, 0), args);
    }
    else
        func = rethrow_func;
    func->mainsym->extflag = TRUE;
    args = makenode(en_void, args, 0);
    rnode = makenode(en_napccon, (void*)func, 0);
    rnode = makenode(en_void, rnode, rnode);
    args = makenode(en_void, rnode, args);
    args = makenode(en_fcall, makeintnode(en_icon, 0), args);
    *node = args;
    return ;
}

//-------------------------------------------------------------------------

TYP *primary(ENODE **node)
/*
 *      primary will parse a primary expression and set the node pointer
 *      returning the type of the expression parsed. primary expressions
 *      are any of:
 *                      id
 *                      constant
 *											true or false
 *                      string
 *											this
 *											::primary
 *                      ( expression )
 *                      primary++
 *                      primary--
 *                      primary[ expression ]
 *                      primary.id
 *                      primary->id
 *                      primary( parameter list )
 *                      primary.*id
 *                      primary->*id
 *											(* expression)( parameter list )
 *                      (typecast)primary
 *                      (typecast)(unary)
 *                      new ...
 *                      delete ...
 *                      static_cast<type>(expr)
 *                      dynamic_cast<type>(expr)
 *                      typeid(expr) 
 *                      operator ... ( parameter list )
 */
{
    ENODE *pnode = 0,  *qnode,  *rnode;
    SYM *sp = 0;
    TYP *tptr,  *tp1,  *tp2;
    int flag = 0;
    int gcode, ia;
    int isstring = FALSE, gdf;
    SYM *lastdeclclass = declclass;
    char *p;
    switch (lastst)
    {
        case kw__trap:
            getsym();
            if (needpunc(openpa, 0))
            {
                long num = intexpr(0);
                if (num > 255 || num < 0)
                    generror(ERR_INVTRAP, 0, 0);
                qnode = makeintnode(en_icon, num);
                pnode = makenode(en_trapcall, qnode, 0);
                needpunc(closepa, 0);
            }
            goodcode |= GF_ASSIGN;
            *node = pnode;
            return  &stdint;
        case kw_D0:
        case kw_D1:
        case kw_D2:
        case kw_D3:
        case kw_D4:
        case kw_D5:
        case kw_D6:
        case kw_D7:
        case kw_D8:
        case kw_D9:
        case kw_DA:
        case kw_DB:
        case kw_DC:
        case kw_DD:
        case kw_DE:
        case kw_DF:
            tptr = xalloc(sizeof(TYP));
            *tptr = stduns;
            tptr->sp = 0;
            pnode = makenode(en_regref, (char*)(regdsize *65536+lastst - kw_D0),
                0);
            pnode = makenode(en_ul_ref, pnode, 0);
            *node = pnode;
            getsym();
            return tptr;
        case kw_A0:
        case kw_A1:
        case kw_A2:
        case kw_A3:
        case kw_A4:
        case kw_A5:
        case kw_A6:
        case kw_A7:
        case kw_A8:
        case kw_A9:
        case kw_AA:
        case kw_AB:
        case kw_AC:
        case kw_AD:
        case kw_AE:
        case kw_AF:
            tptr = xalloc(sizeof(TYP));
            *tptr = stduns;
            tptr->sp = 0;
            pnode = makenode(en_regref, (char*)(regasize *65536+lastst - kw_D0),
                0);
            pnode = makenode(en_ul_ref, pnode, 0);
            *node = pnode;
            getsym();
            return tptr;
        case kw_F0:
        case kw_F1:
        case kw_F2:
        case kw_F3:
        case kw_F4:
        case kw_F5:
        case kw_F6:
        case kw_F7:
        case kw_F8:
        case kw_F9:
        case kw_FA:
        case kw_FB:
        case kw_FC:
        case kw_FD:
        case kw_FE:
        case kw_FF:
            tptr = xalloc(sizeof(TYP));
            *tptr = stdlongdouble;
            tptr->sp = 0;
            pnode = makenode(en_regref, (char*)(regfsize *65536+lastst - kw_D0),
                0);
            pnode = makenode(en_longdoubleref, pnode, 0);
            *node = pnode;
            getsym();
            return tptr;
        case kw_cr0:
        case kw_cr1:
        case kw_cr2:
        case kw_cr3:
        case kw_cr4:
        case kw_cr5:
        case kw_cr6:
        case kw_cr7:
        case kw_dr0:
        case kw_dr1:
        case kw_dr2:
        case kw_dr3:
        case kw_dr4:
        case kw_dr5:
        case kw_dr6:
        case kw_dr7:
        case kw_tr0:
        case kw_tr1:
        case kw_tr2:
        case kw_tr3:
        case kw_tr4:
        case kw_tr5:
        case kw_tr6:
        case kw_tr7:
            tptr = xalloc(sizeof(TYP));
            *tptr = stduns;
            tptr->sp = 0;
            pnode = makenode(en_regref, (char*)(regdsize *65536+lastst - kw_cr0
                + 48), 0);
            pnode = makenode(en_ul_ref, pnode, 0);
            *node = pnode;
            getsym();
            return tptr;
        case kw_asm:
        case kw__export:
        case kw__import:
        case kw__pascal:
        case kw__stdcall:
        case kw__indirect:
            goto dodefault;
        case kw_alloca:
            getsym();
            return doalloca(node);
        case kw___typeid:
                getsym();
                if (needpunc(openpa,skm_closepa)) {
                    int size;
                    gdf = global_deref;
                    global_deref = 0;
                    tptr = expression(&pnode, FALSE);
                    global_deref = gdf;
                    needpunc(closepa, skm_closepa);
                    if (!tptr)
                        return 0;
                    size = retvalsize(tptr);
                    if (size >= 100000)
                        size -= 100000;
                    *node = makeintnode(en_icon,size);
                    return tptr;
                }
                return 0;
        case classsel:
            getsym();
            if (lastst == kw_operator)
            {
                getsym();
                tptr = dooperator(&pnode);
            }
            else
            {
                declclass = 0;
                nonslookup = TRUE;
                tptr = nameref(&pnode, 0);
                declclass = lastdeclclass;
            }
            break;
        case kw_operator:
            getsym();
            tptr = dooperator(&pnode);
            break;
        case kw_static_cast:
        case kw_dynamic_cast:
            tptr = docppcasts(lastst, &pnode);
            break;
        case kw_typeid:
            tptr = dotypeid(&pnode);
            break;
        case kw_throw:
            tptr = &stdvoid;
            dothrow(&pnode);
            break;
        case id:
            tptr = nameref(&pnode, 0);
            if (!tptr)
                tptr = maketype(bt_int, stdintsize);
            if (tptr->sp && (tptr->sp->storage_class == sc_member) && !(tptr
                ->sp->value.classdata.cppflags &PF_STATIC) && currentfunc && 
                (currentfunc->value.classdata.cppflags &PF_MEMBER) && 
                (currentfunc->value.classdata.cppflags &PF_STATIC))
                gensymerror(ERR_STATICNOACCESS, tptr->sp->name);
            break;
        case iconst:
            tptr = &stdint;
            pnode = makeintnode(en_icon, ival);
            getsym();
            break;
        case iuconst:
            tptr = &stduns;
            pnode = makeintnode(en_iucon, ival);
            getsym();
            break;
        case lconst:
            tptr = &stdint;
            pnode = makeintnode(en_lcon, ival);
            getsym();
            break;
        case luconst:
            tptr = &stdunsigned;
            pnode = makeintnode(en_lucon, ival);
            getsym();
            break;
        case llconst:
            tptr = &stdlonglong;
            pnode = makeintnode(en_llcon, ival);
            getsym();
            break;
        case lluconst:
            tptr = &stdunsignedlonglong;
            pnode = makeintnode(en_llucon, ival);
            getsym();
            break;
        case cconst:
            tptr = &stdchar;
            pnode = makeintnode(en_ccon, ival);
            getsym();
            break;
        case kw___I:
            tptr = &stdimaginary;
            pnode = xalloc(sizeof(ENODE));
            pnode->nodetype = en_fimaginarycon;
            pnode->cflags = (stdpragmas &STD_PRAGMA_FENV) ?
                DF_FENV_ACCESS : 0;
            pnode->cflags |= (stdpragmas &STD_PRAGMA_CXLIMITED) ?
                    DF_CXLIMITED : 0;
            LongLongToFPF(&pnode->v.f, 1);
            getsym();
            break;
		case kw__NAN:
            tptr = &stdfloat;
            pnode = xalloc(sizeof(ENODE));
            pnode->nodetype = en_fcon;
            pnode->cflags = (stdpragmas &STD_PRAGMA_FENV) ?
                DF_FENV_ACCESS : 0;
            pnode->cflags |= (stdpragmas &STD_PRAGMA_CXLIMITED) ?
                    DF_CXLIMITED : 0;
            ;
            LongLongToFPF(&pnode->v.f, 0);
			pnode->v.f.type = IFPF_IS_NAN;
            getsym();
            break;
        case rconst:
            tptr = &stddouble;
            pnode = xalloc(sizeof(ENODE));
            pnode->nodetype = en_rcon;
            pnode->cflags = (stdpragmas &STD_PRAGMA_FENV) ?
                DF_FENV_ACCESS : 0;
            pnode->cflags |= (stdpragmas &STD_PRAGMA_CXLIMITED) ?
                    DF_CXLIMITED : 0;
            ;
            pnode->v.f = rval;
            getsym();
            break;
        case lrconst:
            tptr = &stdlongdouble;
            pnode = xalloc(sizeof(ENODE));
            pnode->nodetype = en_lrcon;
            pnode->cflags = (stdpragmas &STD_PRAGMA_FENV) ?
                DF_FENV_ACCESS : 0;
            pnode->cflags |= (stdpragmas &STD_PRAGMA_CXLIMITED) ?
                    DF_CXLIMITED : 0;
            ;
            pnode->v.f = rval;
            getsym();
            break;
        case fconst:
            tptr = &stdfloat;
            pnode = xalloc(sizeof(ENODE));
            pnode->nodetype = en_fcon;
            pnode->cflags = (stdpragmas &STD_PRAGMA_FENV) ?
                DF_FENV_ACCESS : 0;
            pnode->cflags |= (stdpragmas &STD_PRAGMA_CXLIMITED) ?
                    DF_CXLIMITED : 0;
            ;
            pnode->v.f = rval;
            getsym();
            break;
        case sconst:
            isstring = TRUE;
            tptr = copytype(&stdstring, 0);
            pnode = makeintnode(en_labcon, tostring(&tptr->size));
            *node = pnode;
            break;
        case kw___func__:
			lastst = sconst;
            isstring = TRUE;
            unmangle(laststr, currentfunc->name);
            tptr = copytype(&stdstring, DF_CONST);
            pnode = makeintnode(en_labcon, tostring(&tptr->size));
            *node = pnode;
            break;
        case lsconst:
            isstring = TRUE;
            tptr = copytype(&stdwstring, 0);
            pnode = makeintnode(en_labcon, tostring(&tptr->size));
            tptr->size *= stdwstring.btp->size;
            *node = pnode;
            break;
            case kw_true:
                tptr = &stdbool;
                pnode = makeintnode(en_boolcon, 1);
                getsym();
                *node = pnode;
                return tptr;
            case kw_false:
                tptr = &stdbool;
                pnode = makeintnode(en_boolcon, 0);
                getsym();
                *node = pnode;
                return tptr;
            case kw_this:
                getsym();
                if (declclass && currentfunc)
                {
                    if (currentfunc->value.classdata.cppflags &PF_STATIC)
                        generror(ERR_STATICNOTHIS, 0, 0);
                    pnode = makenode(en_a_ref, copynode(thisenode), 0);
                    tptr = maketype(bt_pointer, stdaddrsize);
                    tptr->btp = declclass->tp;
                }
                else
                {
                    generror(ERR_NOCLASSNOTHIS, 0, 0);
                    pnode = makenode(en_nacon, &undef, 0);
                    tptr = &stdint;
                }
                break;
            case kw_new:
                getsym();
                tptr = donew(&pnode);
                break;
            case kw_delete:
                getsym();
                tptr = dodelete(&pnode);
                //                                    if (!pnode)
                //                                       return 0 ;
                break;

        case openpa:
            getsym();
            if (lastst == star)
            {
                /* function pointers */
                gcode = goodcode;
                goodcode &= ~(GF_AND | GF_SUPERAND | GF_INIF);
                getsym();
                gdf = global_deref;
                global_deref = 1;
                tptr = expression(&pnode, FALSE);
                global_deref = gdf;
                if (!tptr)
                    return 0;
                needpunc(closepa, skm_closepa);
                break;
            }
            else
            castcont: if (!castbegin(lastst) || cantnewline && prm_ansi)
            {
                /* an expression in parenthesis */
                int oitplarg;
                gcode = goodcode;
                goodcode &= ~(GF_AND | GF_SUPERAND | GF_INIF);
                gdf = global_deref;
                global_deref = 0;
                oitplarg = intemplateargs;
                intemplateargs = 0;
                tptr = expression(&pnode, FALSE);
                intemplateargs = oitplarg;
                global_deref = gdf;
                if (!tptr)
                    return 0;
                goodcode = gcode | (goodcode &GF_ASSIGN);
                needpuncexp(closepa, skm_closepa);
                goto contfor;
            }
            else
            {
                 /* cast operator */
                /* a cast */
                int oip = inprototype;
                inprototype=TRUE;
                declid[0] = 0;
                typequal = 0;
                decl(0, 0); /* do cast declaration */
                headptr = &head;
                decl1(sc_type, 0);
                tptr = head;
                inprototype = oip;
                //								if (tptr->type == bt_void)
                //									generror(ERR_NOVOID,0,0) ;
                if (needpunc(closepa, 0))
                {
                    if (lastst == begin) 
                    {
                        ENODE *ep1 = dummyvar(tptr->size,tptr,0);
                        ENODE *ep = doeinit(ep1,tptr);
						if (!prm_c99)
							generror(ERR_C99_STYLE_INITIALIZATION_USED, 0, 0);
                        if (expr_runup[expr_updowncount - 1])
                            expr_runup[expr_updowncount - 1] = makenode(en_void,
                                expr_runup[expr_updowncount - 1], ep);
                        else
                            expr_runup[expr_updowncount - 1] = ep;
                        *node = ep1;
                        return tptr;
                    } 
                    else
                    { 
                        gcode = goodcode;
                        goodcode &= ~(GF_AND | GF_SUPERAND);
                        gdf = global_deref;
                        global_deref = 0;
                        if ((tp1 = unary(&pnode)) == 0)
                        {
                            generror(ERR_IDEXPECT, 0, 0);
                            *node = pnode = makenode(en_icon, 0, 0);
                            return  &stdint;
                        }
//                        tptr->sp = tp1->sp;
                        global_deref = gdf;
                        goodcode = gcode | (goodcode &GF_ASSIGN);
                        pnode->cflags = tptr->cflags;
                        if (tptr->type == bt_memberptr)
                        {
                            ENODE *epx = pnode;
                            if (!isintconst(epx->nodetype) || epx->v.i != 0)
                            {
                                tp1 = FindMemberPointer(tptr, tp1, &pnode);
                                if (!exactype(tptr, tp1, FALSE ) && (!isintconst(epx
                                    ->nodetype) || epx->v.i != 0))
                                    generror(ERR_MEMBERPTRMISMATCH, 0, 0);
                                //                         else
                                //                            tp2= deref(&ep2,tp1) ;
                            }
                        }
                        else
                        if (pnode->nodetype == en_placeholder)
                        {
                            SYM *spx = pnode->v.sp;
                            spx = spx->tp->lst.head;
                            if (spx)
                            {
                                SetExtFlag(spx, TRUE);
                                if (spx->next)
                                    genfunc2error(ERR_AMBIGFUNC, funcwithns(spx),
                                        funcwithns(spx->next));
                                pnode = makenode(en_napccon, spx, 0);
                            }
                        }
                            //                                                   if (prm_cplusplus) {
                            //                                                      cppcast(tptr,tp1,&pnode,FALSE) ;
                            //                                                   } else 
                        if (isstructured(tp1) || isstructured(tptr))
                        {
                            if (tptr->type != bt_void)
                                if (!prm_c99 && !prm_cplusplus && !cppcast(tptr, tp1, &pnode, FALSE))
                                    genmismatcherror(tp1, tptr);
                        }
                        else
                            promote_type(tptr, &pnode);
                    }
                }
                else
                {
                    *node = makeintnode(en_icon, 0);
                    return (&stdint);
                }
            }
            *node = pnode;
            return tptr;
        default:
            dodefault: *node = makeintnode(en_icon, 0); return 0;
    }
    contfor: 
    /* modifiers that can appear after an expression */
    for (;;)
    {
        int i;
        switch (lastst)
        {
            case autoinc:
            case autodec:
                    if (prm_cplusplus)
                    {
                        TYP *tp;
                        // patch in an integer value for the last param
                        ENODE *qnode = makeintnode(en_icon, 0);
                        tp = mathoverload(lastst, tptr, maketype(bt_int, stdintsize),
                            &pnode, &qnode);
                        if (tp)
                        {
                            tptr = tp;
                            getsym();
                            break;
                        }
                    }
                if (isstring)
                    generror(ERR_INVALIDSTRING, 0, 0);
                if (tptr->type == bt_pointer || tptr->type == bt_farpointer ||
                    tptr->type == bt_segpointer)
                if (tptr->btp->val_flag &VARARRAY)
                {
                    rnode = makenode(tptr->sp->storage_class == sc_auto ?
                        en_autocon : en_nacon, (void*)tptr->sp, 0);
                    rnode = makenode(en_add, rnode, makeintnode(en_icon,
                        stdaddrsize + stdintsize));
                    rnode = makenode(en_a_ref, rnode, 0);
                }
                else
                {
                    rnode = makeintnode(en_icon, tptr->btp->size);
                    if (tptr->btp->size == 0)
                        generror(ERR_ZEROPTR, 0, 0);
                }
                else
                    rnode = makeintnode(en_icon, 1);
                if (!lvalue(pnode) && tptr->type != bt_ref)
                {
                    if (tptr->cflags &DF_CONST)
                        generror(ERR_MODCONS, 0, 0);
                    else
                        generror(ERR_LVALUE, 0, 0);
                if (pnode->cflags &DF_CONST)
                    generror(ERR_MODCONS, 0, 0);
                }

                    checkConstClass(pnode);
                pnode = makenode(lastst == autoinc ? en_ainc : en_adec, pnode,
                    rnode);
                goodcode |= GF_ASSIGN;
                tptr->uflags |= UF_ALTERED;
                getsym();
                break;
            case openbr:
                 /* build a subscript reference */
                if (tptr->type == bt_cond)
                    tptr = tptr->btp;
                if (tptr->val_flag && (tptr->cflags &DF_AUTOREG))
                {
                    gensymerror(ERR_NOANDREG, tptr->sp->name);
                }
                flag = 1;
                getsym();
                gcode = goodcode;
                goodcode &= ~(GF_AND | GF_SUPERAND);
                gdf = global_deref;
                global_deref = 0;
                tp2 = expression(&rnode, FALSE);
				promote_type(&stdint, &rnode);
				tp2 = &stdint;
                global_deref = gdf;
                goodcode = gcode &~GF_ASSIGN;
                needpuncexp(closebr, skm_closebr);
                    if (prm_cplusplus)
                    {
                        TYP *tp = mathoverload(openbr, tptr, tp2, &pnode,
                            &rnode);
                        if (tp)
                        {
                            tptr = tp;
                            break;
                        }
                    }
                if (tptr->type != bt_pointer && tptr->type != bt_farpointer &&
                    tptr->type != bt_segpointer)
                {
                    TYP *tp3 = tptr;
                    if (tp3->type == bt_cond)
                        tp3 = tp3->btp;
                    if (!isscalar(tp3))
                        generrorexp(ERR_NONPORT, 0, skm_closebr);
                    if (tp2->type == bt_pointer || tp2->type == bt_farpointer 
                        || tp2->type == bt_segpointer)
                    {
                        tp2 = tp2->btp;
                        qnode = makeintnode(en_icon, tp2->size);
//                        qnode = makenode(en_pmul, qnode, pnode);
//                        pnode = makenode(en_add, rnode, qnode);
                        qnode = makenode(en_umul, pnode, qnode);
                        pnode = makenode(en_add, qnode, rnode);
                    }
                    else
                    {
                        generror(ERR_NOPOINTER, 0, 0);
                        pnode = makenode(en_add, pnode, rnode);
                        //                                          pnode = makenode(en_cl,pnode,0) ;
                    }
                    tptr = tp2;

                }
                else
                {
                    if (tptr->type == bt_pointer && (tptr->val_flag &VARARRAY))
                        tptr = tptr->btp;
                    tptr = tptr->btp;
                    if (tp2)
                    {
                        TYP *tp3 = tp2;
                        if (tp3->type == bt_cond)
                            tp3 = tp3->btp;
                        if (!isscalar(tp3))
                            generror(ERR_NONPORT, 0, 0);
                        //                                                   rnode = makenode(en_cl,rnode,0) ;
                        if (tptr->esize)
                        {
                            // vararray handling
                            qnode = makenode(tptr->sp->storage_class == sc_auto
                                ? en_autocon : en_nacon, (void*)tptr->sp, 0);
                            qnode = makenode(en_add, qnode, makeintnode(en_icon,
                                stdaddrsize + ((int)tptr->esizeindex - 1)
                                *stdintsize));
                            qnode = makenode(en_a_ref, qnode, 0);
                        }
                        else
                            qnode = makeintnode(en_icon, tptr->size);
//                        qnode = makenode(en_pmul, qnode, rnode);
//                        pnode = makenode(en_add, pnode, qnode);
                        qnode = makenode(en_umul, qnode, rnode);
                        pnode = makenode(en_add, pnode, qnode);
                        //                                                   pnode = makenode(en_cl,pnode,0) ;
                    }
                    else
                        pnode = makeintnode(en_icon, 0);
                }
                pnode->cflags = tptr->cflags;
                if (tptr->val_flag == 0)
                    tptr = deref(&pnode, tptr);
                break;
            case pointsto:
                 /* pointer reference */
                if (tptr->type == bt_cond)
                    tptr = tptr->btp;
                    if (prm_cplusplus)
                    {
                        TYP *tp = mathoverload(pointsto, tptr, 0, &pnode, 0);
                        if (tp)
                        {
                            tptr = tp;
                        }
                    }
                if (tptr->type != bt_pointer && tptr->type != bt_farpointer &&
                    tptr->type != bt_segpointer || !isstructured(tptr->btp))
                {
                    generror(ERR_POINTTOSTRUCT, 0, 0);
                    while (lastst == pointsto || lastst == dot)
                    {
                        getsym();
                        getsym();
                    }
					pnode = makenode(en_nacon, (void *)&undef, 0);
					deref(&pnode, &stdint);
					tptr = &stdmatch;
                    break;
                }
                else
                    tptr = tptr->btp;
                pnode->cflags = tptr->cflags;
                if (tptr->val_flag == 0)
                {
                    pnode = makenode(en_ua_ref, pnode, 0);
                    pnode->cflags = tptr->cflags;
                }
                goto arounddot;
                /*
                 *      fall through to dot operation
                 */
            case dot:
                if (tptr->type == bt_cond)
                    tptr = tptr->btp;
                if (!isstructured(tptr) && (tptr->type != bt_ref ||
                    !isstructured(tptr->btp)))
                {
                    generror(ERR_ADDROFSTRUCT, 0, 0);
                    while (lastst == pointsto || lastst == dot)
                    {
                        getsym();
                        getsym();
                    }
					pnode = makenode(en_nacon, (void *)&undef, 0);
					deref(&pnode, &stdint);
					tptr = &stdmatch;
                    break;
                }
                arounddot: if (isstring)
                    generror(ERR_INVALIDSTRING, 0, 0);
				tptr->uflags &= ~UF_ASSIGNED;
                getsym(); /* past -> or . */
                if (lastst != id)
                if (lastst == compl)
                {
                    TYP *tp;
                    getsym();
                    if (lastst != id)
                    {
                        generror(ERR_IDEXPECT, 0, 0);
                        pnode = makenode(en_icon, 0, 0);
                    }
                    else
                    {
                        SYM *sp;
                        if (tptr && isstructured(tptr) /*&& strchr(tptr->sp
                            ->name,'#')*/)
                        {
                            sp = typesearch(lastid);
                            if (sp && sp->istemplate)
                            {
                                getsym();
                                if (lastst != lt)
                                {
                                    gensymerror(ERR_TEMPLATEFEWARG, lastid);
                                    backup(id);
                                    pnode = makenode(en_icon, 0, 0);
                                    goto compljn2;
                                }
                                else
                                    sp = lookupTemplateType(sp,0);
                                if (sp != tptr->sp)
                                {
                                    gensymerror(ERR_BADESTRUCT, tptr->sp->name);
                                    pnode = makenode(en_icon, 0, 0);
                                }
                                else
                                {
                                    tp = gatherparms(&qnode, TRUE);
                                    pnode = do_destructor(tptr->sp, tp, tptr,
                                        pnode, 1, 0, FALSE);
                                    parm_ns_level--;
                                }
                            }
                            else if (strcmp(lastid, tptr->sp->name))
                            {
                                gensymerror(ERR_BADESTRUCT, tptr->sp->name);
                                pnode = makenode(en_icon, 0, 0);
                                getsym();
                            }
                            else
                            {
                                tp = gatherparms(&qnode, TRUE);
                                getsym();
                                pnode = do_destructor(tptr->sp, tp, tptr, pnode,
                                    1, 0, FALSE);
                                parm_ns_level--;
                            }
                        }
                        else
                            getsym();
                        compljn2: goodcode |= GF_ASSIGN;
                        needpunc(openpa, skm_closepa);
                        needpunc(closepa, 0);
                    }
                }
                else
                {
                    generror(ERR_IDEXPECT, 0, 0);
                    pnode = makenode(en_icon, 0, 0);
                }
                else
                {

                    tp2 = nameref(&pnode, tptr);
                    if (tp2->bits !=  - 1)
                    {
                        pnode = makenode(en_bits, pnode, 0);
                        pnode->bits = tp2->bits;
                        pnode->startbit = tp2->startbit;
                        pnode->cflags = tp2->cflags | pnode->v.p[0]->cflags;
                    }
                    if (pnode->nodetype != en_placeholder)
                        pnode->cflags = tptr->cflags | pnode->v.p[0]->cflags;
                    tp2->uflags |= tptr->uflags;
                    tptr = tp2;
                    break;
                    /* don't get here */
                    sp = search(nm = litlate(lastid), &tptr->lst);
                    if (sp == 0)
                    {
                        tptr = &stdmatch;
                        pnode = makenode(en_nacon, &undef, 0);
                        gensymerror(ERR_UNDEFINED, nm);

                        getsym();
                        while (lastst == pointsto || lastst == dot)
                        {
                            getsym();
                            getsym();
                        }
                        break;
                    }
                    else
                    {
                        tp2 = sp->tp;
                            if (prm_cplusplus)
                            {
                                if (tp2->type == bt_class)
                                {
                                    declclass = tp2->sp;
                                }
                            }
                        pnode = make_callblock(0, pnode, tp2, pnode->size);
                        qnode = makeintnode(en_icon, sp->value.i);
                        pnode = makenode(en_addstruc, pnode, qnode);
                        pnode->cflags = tptr->cflags | pnode->v.p[0]->cflags;
                        tp2->uflags |= tptr->uflags;
                        tptr = tp2;
                        if (tptr->val_flag == 0)
                            tptr = deref(&pnode, tptr);
                        if (tp2->bits !=  - 1)
                        {
                            qnode = pnode;
                            pnode = makenode(en_bits, qnode, 0);
                            pnode->bits = tp2->bits;
                            pnode->startbit = tp2->startbit;
                            pnode->cflags = tptr->cflags | pnode->v.p[0]
                                ->cflags;
                        }
                    }
                    getsym(); /* past id */
                }
                break;
                case pointstar:
                     /* pointer reference to member ptr */
                    if (prm_cplusplus)
                    {
                        TYP *tp = mathoverload(pointsto, tptr, 0, &pnode, 0);
                        if (tp)
                        {
                            tptr = tp;
                        }
                    }
                    if (tptr->type != bt_pointer && tptr->type != bt_farpointer
                        && tptr->type != bt_segpointer)
                    {
                        generror(ERR_NOPOINTER, 0, 0);
                        break;
                    }
                    else
                        tptr = tptr->btp;
                    pnode->cflags = tptr->cflags;
                    if (tptr->val_flag == 0)
                    {
                        pnode = makenode(en_ua_ref, pnode, 0);
                        pnode->cflags = tptr->cflags;
                    }

                    /*
                     *      fall through to dot operation
                     */
                case dotstar:
                    if (isstring)
                        generror(ERR_INVALIDSTRING, 0, 0);
                    getsym(); /* past ->* or .* */
                    if (lastst != id)
                        generror(ERR_IDEXPECT, 0, 0);
                    if (tptr->type != bt_class && tptr->type != bt_struct)
                        generror(ERR_CLASSINSTDOTSTAR, 0, 0);
                    tp2 = unary(&qnode);
                    if (!tp2 || tp2->type != bt_memberptr)
                    {
                        generror(ERR_MEMBERNAMEEXP, 0, 0);
                        pnode = makenode(en_icon, 0, 0);
                    }
                    else
                    {
                        SYM *sp,  *s1;
                        CLASSLIST *l;
                        sp = tptr->sp;
                        s1 = tp2->sp;
                        l = sp->value.classdata.baseclass;
                        while (l)
                        {
                            if (s1->mainsym == l->data->mainsym)
                            {
                                if (l->mode != BM_PUBLIC)
                                {
                                    l = 0;
                                    break;
                                }
                                if (l->offset)
                                    pnode = makenode(en_addstruc, pnode,
                                        makeintnode(en_icon, l->offset));
                                break;
                            }
                            l = l->link;
                        }
                        if (!l)
                        {
                            doubleerr(ERR_NOTPUBLICBASECLASS, tp2->sp->name,
                                tptr->sp->name);
                        }
                        tp1 = tp2->btp;
                        if (tp1->type == bt_func || tp1->type == bt_ifunc)
                        {
                            if (tp2->sp->value.classdata.baseclass->link)
                            {
                                ENODE *ep1 = copy_enode(qnode);
                                ep1->nodetype = en_a_ref;
                                ep1->v.p[0] = makenode(en_add, ep1->v.p[0],
                                    makeintnode(en_icon, stdaddrsize));
                                pnode = makenode(en_add, pnode, ep1);
                            }
                            pnode = makenode(en_thiscall, pnode, qnode);
                            tptr = tp2;
                        }
                        else
                        {
                            pnode = makenode(en_add, pnode, makeintnode(en_icon,
                                - 1));
                            pnode = makenode(en_add, pnode, qnode);
                            tptr = deref(&pnode, tp2->btp);
                        }
                    }
                    break;
            case lt:
                // template-qualified function name 
                if (!prm_cplusplus || tptr->type != bt_defunc && tptr->type !=
                    bt_func && tptr->type != bt_ifunc)
                {
                    goto fini;
                }
                tptr = InstantiateTemplateFuncDirect(&pnode, tptr);
                break;
            case openpa:
                 /* function reference */
                if (tptr->type == bt_cond)
                    tptr = tptr->btp;
                getsym();
                flag = 1;
                if (isstring)
                    generror(ERR_INVALIDSTRING, 0, 0);
                /* Get function args */
                tp1 = gatherparms(&qnode, FALSE);
                if (tptr->type == bt_memberptr)
                {
                    if (pnode->nodetype == en_thiscall)
                    {
                        ENODE *thx = pnode;
                        pnode = pnode->v.p[1];
                        pnode = complex_functions(pnode, &tptr, qnode, tp1);
                        thx->v.p[1] = pnode;
                        pnode = thx;
                        tptr = tptr->btp->btp;
                    }
                    else
                    {
                        pnode = makenode(en_icon, 0, 0);
                        gensymerror(ERR_NOFUNC, tptr->btp->sp->name);
                    }
                }
                else
                    pnode = complex_functions(pnode, &tptr, qnode, tp1);
                goodcode |= GF_ASSIGN;
                parm_ns_level--;
                break;
            default:
                goto fini;
        }
    }
    fini: *node = pnode;
    /* symbol level error checking */
    if (tptr && !flag && !isstring && !(goodcode &GF_AND) && lastsym && tptr
        ->type != bt_func && tptr->type != bt_ifunc && !tptr->val_flag &&
        lastsym->storage_class != sc_type && lastsym->storage_class !=
        sc_static && lastsym->storage_class != sc_global && lastsym
        ->storage_class != sc_external && lastsym->storage_class !=
        sc_externalfunc)
    {
        if (!(lastsym->tp->uflags &UF_DEFINED) && lastst != assign && lastst !=
            asplus && lastst != asminus && lastst != astimes && lastst !=
            asdivide && lastst != asmodop && lastst != asrshift && lastst !=
            aslshift && lastst != asor && lastst != asxor && lastst != asand)
        {
            gensymerror(ERR_SYMUNDEF, lastsym->name);
            lastsym->tp->uflags |= UF_DEFINED;
        }
    }
	if (lastsym)
        lastsym->tp->uflags &= ~UF_ASSIGNED;
    declclass = lastdeclclass;
    return tptr;
}

//-------------------------------------------------------------------------

int isstructured(TYP *tp)
{
    switch (tp->type)
    {
        case bt_struct:
        case bt_class:
        case bt_union:
            return TRUE;
    }
    return FALSE;
}

//-------------------------------------------------------------------------

int castvalue(ENODE *node)
/*
 * See if this is a cast operator */
{
    switch (node->nodetype)
    {
        case en_cb:
        case en_cub:
        case en_cbool:
        case en_cw:
        case en_cuw:
        case en_cl:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cf:
        case en_cd:
        case en_cp:
        case en_cld:
        case en_cfp:
        case en_csp:
        case en_cll:
        case en_cull:
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_cfi:
        case en_cri:
        case en_clri:
            return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------

int lvalue(ENODE *node)
/* See if this is an lvalue; that is has it been dereferenced?
 */
{
    if (!prm_cplusplus)
    {
        while (castvalue(node))
            node = node->v.p[0];
    }
    switch (node->nodetype)
    {
        case en_fp_ref:
        case en_bool_ref:
        case en_b_ref:
        case en_w_ref:
        case en_l_ref:
		case en_i_ref:
		case en_ui_ref:
		case en_a_ref: case en_ua_ref:
        case en_ub_ref:
        case en_uw_ref:
        case en_ul_ref:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_ull_ref:
        case en_ll_ref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
            return 1;
        case en_bits:
        case en_cl_reg:
            return lvalue(node->v.p[0]);
    }
    return 0;
}

//-------------------------------------------------------------------------
int pointermode(TYP *tp1, TYP *tp2)
{
    if (tp1->type == bt_ref || tp1->type == bt_cond)
        tp1 = tp1->btp;
    if (tp2->type == bt_ref || tp2->type == bt_cond)
        tp2 = tp2->btp;
    return ((tp1->type == bt_pointer || tp1->type == bt_farpointer) * 2 + 
            (tp2->type == bt_pointer || tp2->type == bt_farpointer));
}
//-------------------------------------------------------------------------

TYP *unary(ENODE **node)
/*
 *      unary evaluates unary expressions and returns the type of the
 *      expression evaluated. unary expressions are any of:
 *
 *                      primary
 *                      ++unary
 *                      --unary
 *                      !unary
 *                      ~unary
 *                      &unary
 *                      -unary
 *                      *unary
 *                      sizeof(typecast)
 *
 */
{
    TYP *tp,  *tp1,  *oldandtyp = andtyp;
    ENODE *ep1,  *ep2;
    int i, gdf, needclose, st;

    thisx = 0;
    switch (lastst)
    {
        case autodec:
            /* fall through to common increment */
        case autoinc:
            st = lastst;
            getsym();
            gdf = global_deref;
            global_deref = 0;
            tp = unary(&ep1);
            global_deref = gdf;
            if (tp == 0)
            {
                generror(ERR_IDEXPECT, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
                return 0;
            }
            goodcode |= GF_ASSIGN;
                if (prm_cplusplus)
                {
                    TYP *tptr = mathoverload(st, tp, 0, &ep1, 0);
                    if (tptr)
                    {
                        tp = tptr;
                        break;
                    }
                }
            if (lvalue(ep1) || tp->type == bt_ref)
            {
                    checkConstClass(ep1);
                if (tp->type == bt_pointer || tp->type == bt_farpointer || tp
                    ->type == bt_segpointer)
                {
                    if (tp->btp->size == 0)
                        generror(ERR_ZEROPTR, 0, 0);
                    if (tp->btp->val_flag &VARARRAY)
                    {
                        ep1 = makenode(tp->sp->storage_class == sc_auto ?
                            en_autocon : en_nacon, (void*)tp->sp, 0);
                        ep1 = makenode(en_add, ep1, makeintnode(en_icon,
                            stdaddrsize + stdintsize));
                        ep1 = makenode(en_a_ref, ep1, 0);
                    }
                    else
                        ep2 = makeintnode(en_icon, tp->btp->size);
                }
                else
                    ep2 = makeintnode(en_icon, 1);
                if (ep1->cflags &DF_CONST)
                    generror(ERR_MODCONS, 0, 0);
                ep1 = makenode(st == autodec ? en_assub : en_asadd, ep1, ep2);
            }
            else
            {
                if (tp->cflags &DF_CONST)
                    generror(ERR_MODCONS, 0, 0);
                else
                    generror(ERR_LVALUE, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
                return (0);
            }
            tp->uflags |= UF_ALTERED;
            break;
        case minus:
        case plus:
            {
                int stt = lastst;
                getsym();
                gdf = global_deref;
                global_deref = 0;
                tp = unary(&ep1);
#ifdef ICODE
                if (tp->type < bt_int) {
                    tp = maketype(bt_int, stdintsize);
                    promote_type(tp,&ep1);
                }
#endif
                global_deref = gdf;
                goodcode &= ~GF_ASSIGN;
                if (tp == 0)
                {
                    generror(ERR_IDEXPECT, 0, 0);
                    *node = makenode(en_nacon, &undef, 0);
                    return 0;
                }
                    if (prm_cplusplus)
                    {
                        TYP *tptr = mathoverload(stt, tp, 0, &ep1, 0);
                        if (tptr)
                        {
                            tp = tptr;
                            break;
                        }
                    }
                if (stt == minus)
                    ep1 = makenode(en_uminus, ep1, 0);
            }
            break;
        case not:
            getsym();
            gdf = global_deref;
            global_deref = 0;
            tp = unary(&ep1);
            global_deref = gdf;
            goodcode &= ~GF_ASSIGN;
            if (tp == 0)
            {
                generror(ERR_IDEXPECT, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
                return 0;
            }
                if (prm_cplusplus)
                {
                    TYP *tptr = mathoverload(not, tp, 0, &ep1, 0);
                    if (tptr)
                    {
                        *node = ep1;
                        tp = tptr;
                        break;
                    }
                }
            tp = &stdint;
            ep1 = makenode(en_not, ep1, 0);
            break;
        case compl:
            getsym();
            if (lastst == id)
            {
                if (declclass)
                {
                    SYM *typesp = typesearch(lastid);
                    if (typesp && typesp->istemplate)
                    {
                        getsym();
                        if (lastst != lt)
                        {
                            gensymerror(ERR_TEMPLATEFEWARG, lastid);
                            backup(id);
                            goto compljn1;
                        }
                        else
                            typesp = lookupTemplateType(typesp,0);
                    }
                    else if (!strcmp(declclass->name, lastid))
                        typesp = declclass;
                    if (typesp == declclass)
                    {
                        generror(ERR_ILLSTRUCT, 0, 0);
                        tp = gatherparms(&ep1, TRUE);
                        ep2 = copynode(thisenode);
                        deref(&ep2, &stdint); /* fixme */
                        ep1 = do_destructor(declclass, tp, declclass->tp, ep2,
                            1, 0, FALSE);
                        goodcode |= GF_ASSIGN;
                        getsym();
                        needpunc(openpa, skm_closepa);
                        needpunc(closepa, 0);
                        parm_ns_level--;
                        break;
                    }
                }
            }
            compljn1: gdf = global_deref;
            global_deref = 0;
            tp = unary(&ep1);
#ifdef ICODE
                if (tp->type < bt_int) {
                    tp = maketype(bt_int, stdintsize);
                    promote_type(tp,&ep1);
                }
#endif
            global_deref = gdf;
            goodcode &= ~GF_ASSIGN;
            if (tp == 0)
            {
                generror(ERR_IDEXPECT, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
                return 0;
            }
            if (pointermode(tp,tp) == 3)
                generror(ERR_ILLUSEPTR,0,0);
            floatcheck(ep1);
                if (prm_cplusplus)
                {
                    TYP *tptr = mathoverload(compl, tp, 0, &ep1, 0);
                    if (tptr)
                    {
                        *node = ep1;
                        tp = tptr;
                        break;
                    }
                    else if (tp->type == bt_struct || tp->type == bt_class)
                    {
                        if (lastst == openpa)
                        {
                            getsym();
                            if (lastst != closepa)
                                generror(ERR_NODESTRUCTARG, 0, skm_declclosepa);
                            else
                                getsym();

                        }
                    }
                }
            ep1 = makenode(en_compl, ep1, 0);
            break;
        case star:
            getsym();
            gdf = global_deref;
            global_deref = 0;
            tp = unary(&ep1);
            global_deref = gdf;
            goodcode &= ~GF_ASSIGN;
            if (tp == 0)
            {
                generror(ERR_IDEXPECT, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
                return 0;
            }
            if (tp->val_flag && (tp->cflags &DF_AUTOREG))
            {
                gensymerror(ERR_NOANDREG, tp->sp->name);
            }
                if (prm_cplusplus)
                {
                    TYP *tptr = mathoverload(star, tp, 0, &ep1, 0);
                    if (tptr)
                    {
                        tp = tptr;
                        break;
                    }
                }
            if (tp->btp == 0)
            {
                generror(ERR_DEREF, 0, 0);
            }
            else
                if (tp->btp->type != bt_void)
                    tp = tp->btp;
            ep1->cflags = tp->cflags;
            if (tp->val_flag == 0)
                tp = deref(&ep1, tp);
            break;
        case and:
            getsym();
            if (!(goodcode &GF_INFUNCPARMS))
                goodcode |= GF_AND;
            gdf = global_deref;
            global_deref = 0;
            andtyp = asntyp;
            tp = unary(&ep1);
            andtyp = oldandtyp;
            global_deref = gdf;
            goodcode &= ~GF_AND;
            goodcode &= ~GF_ASSIGN;
            if (tp == 0)
            {
                generror(ERR_IDEXPECT, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
                return 0;
            }
                if (prm_cplusplus)
                {
                    TYP *tptr = mathoverload(and, tp, 0, &ep1, 0);
                    if (tptr)
                    {
                        tp = tptr;
                        break;
                    }
                }
                if (tp->btp == bt_memberptr)
                {
                    generror(ERR_NOADDRMEMBERPTR, 0, 0);
                    *node = makenode(en_nacon, &undef, 0);
                    return 0;
                }

            else if (tp->startbit !=  - 1)
                generror(ERR_BFADDR, 0, 0);
            else if (tp->val_flag && (tp->cflags &DF_AUTOREG))
                gensymerror(ERR_NOANDREG, tp->sp->name);
            else if (((tp->type != bt_segpointer && tp->type != bt_pointer &&
                tp->type != bt_farpointer || !tp->val_flag) && !isstructured(tp)
                && tp->type != bt_func && tp->type != bt_ifunc && tp->type !=
                bt_defunc && tp->type != bt_memberptr) && (!tp->btp || tp->btp
                ->type != bt_defunc))
            {
                if (!lvalue(ep1) && !castvalue(ep1))
                    generror(ERR_ADDRMEMORYLOC, 0, 0);
            }
            else if ((tp->type == bt_pointer || tp->type == bt_farpointer || tp
                ->type == bt_segpointer) && tp->val_flag && !(goodcode
                &GF_SUPERAND))
                generror(ERR_SUPERAND, 0, 0);
            tp->uflags |= UF_ALTERED | UF_DEFINED;
                if (prm_cplusplus && tp->sp && !tp->sp->parentclass)
                {
                    char *p = tp->sp->name;
                    if (prm_cmangle)
                        p++;
                    if (!strcmp(p, "main"))
                        generror(ERR_MAINNOPTR, 0, 0);
                }
                if ((tp->type == bt_defunc || tp->type == bt_func) && tp->sp
                    ->parentclass)
                    generror(ERR_MUSTCALLMEMBER, 0, 0);
                if (tp->type != bt_memberptr)
            {
                if (lvalue(ep1) && (!tp->val_flag || (tp->cflags & DF_FUNCPARM)))
                {
                    while (castvalue(ep1))
                        ep1 = ep1->v.p[0];
                    ep1 = ep1->v.p[0];
                    if (ep1->nodetype == en_regref)
                        gensymerror(ERR_NOANDREG, tp->sp->name);
                }
                tp1 = maketype(tp->type == bt_farpointer ? bt_farpointer : (tp
                    ->type == bt_segpointer ? bt_segpointer : bt_pointer), tp
                    ->type == bt_farpointer ? (stdaddrsize + stdshortsize) : stdaddrsize);
                tp1->btp = tp;
                tp = tp1;
            }
            break;
        case kw_sizeof:
            getsym();
            needclose = FALSE;
            if (lastst == openpa)
            {
                needclose = TRUE;
                getsym();
            }

            if (castbegin(lastst))
            {
                sizeof_cast: typequal = 0;
                decl(0, 0);
                headptr = &head;
                decl1(sc_type, 0);
                decl2(sc_type, 0);
            }
            else if (lastst == id)
            {
                SYM *sp = typesearch(lastid);
                if (sp)
                {
                    goto sizeof_primary;
                }
                if (cantnewline) 
                {
					// in a #if statement, sizeof an undefined identifier is 0
                    getsym();
                    head = &stdvoid;
                }
                else
                {
                    generror(ERR_SIZE, 0, skm_declclosepa);
                    head = 0;
                }
            }
            else if (lastst == kw_enum)
            {
                getsym();
                if (lastst == id)
                {
                    SYM *sp;
                    if (prm_cplusplus)
                    {
                        if ((sp = search(nm, gsyms)) != 0)
                        {
                            head = sp->tp;
                            getsym();
                        }
                        else
                            goto sizeof_primary;
                    }
                    else if ((sp = search(nm, tagtable)) != 0)
                    {
                        head = sp->tp;
                        getsym();
                    }
                    else
                        goto sizeof_primary;
                }
            }
            else
            {
                ENODE *node = 0;
                sizeof_primary: gdf = global_deref;
                global_deref = 0;
				if (needclose)
	                head = expression(&node, FALSE);
				else
	                head = unary(&node);
                global_deref = gdf;
            }
            if (head != 0)
            {
                if (head->type == bt_pointer && (head->btp->val_flag &VARARRAY))
                    head = head->btp;
                if (head->val_flag &VARARRAY)
                {
                    if (head->btp->sp)
                    {
                        ep1 = makenode(head->btp->sp->storage_class == sc_auto ?
                            en_autocon : en_nacon, (void*)head->btp->sp, 0);
                        ep1 = makenode(en_add, ep1, makeintnode(en_icon,
                            stdaddrsize + stdintsize));
                        ep1 = makenode(en_a_ref, ep1, 0);
                    }
                    else
                    {
                        head = head->btp;
                        ep1 = head->esize;
                        head = head->btp;
                        while (head)
                        {
                            ep1 = makenode(en_mul, ep1, (head->esize) ? head->esize : makeintnode(en_icon,head->size));
                            head = head->btp;
                        }   
                    }
                }
                else
                {
                    if (head->type == bt_func || head->type == bt_ifunc)
                        head->size = stdaddrsize;
                    if (head->size == 0 || head->bits !=  - 1)
                        generror(ERR_SIZE, 0, 0);
                    ep1 = makeintnode(en_icon, head->size);
                }
            }
            else
            {
                generror(ERR_IDEXPECT, 0, 0);
                ep1 = makeintnode(en_icon, 0);
            }
            goodcode &= ~GF_ASSIGN;
            tp = &stdint;
            if (needclose)
                needpunc(closepa, 0);
            break;
        default:
            tp = primary(&ep1);
            if (!tp)
                return 0;
            break;
    }
    /* Dereference if necessary */
    if (global_deref)
    {
        global_deref = 0;
        goodcode &= ~GF_ASSIGN;
        if (tp == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = makenode(en_nacon, &undef, 0);
            return 0;
        }
        if (tp->btp == 0)
        {
            generror(ERR_DEREF, 0, 0);
        }
        else
            if (tp->btp->type != bt_void)
                tp = tp->btp;
        ep1->cflags = tp->cflags;
        if (tp->val_flag == 0)
            tp = deref(&ep1, tp);
    }
    *node = ep1;
    return tp;
}

//-------------------------------------------------------------------------

    /*
     * This handles calling an overloaded math (unary or binary) operator
     * in C++.
     */
    ENODE *makeparam(ENODE *in, ENODE *link, TYP *tp)
    {
        ENODE *rv;
        if (tp->val_flag && isstructured(tp))
        {
            rv = sbll(tp->sp, in, link);
        }
        else
            rv = makenode(en_void, in, link);
        return rv;
    }
    TYP *mathoverload(enum e_bt lst, TYP *tp1, TYP *tp2, ENODE **ep1, ENODE
        **ep2)
    {
        int structured = FALSE;
        ENODE *rte = 0,  *rnode;
        TYP *rtp = 0;
        TYP tpfunc, tpvoid;
        ENODE v1, v2;
        SYM s1, s2,  *spx = 0,  *spy;
        char *buf;
        int memfunc = FALSE;
        if (!prm_cplusplus)
            return 0;
        memset(&v1, 0, sizeof(v1));
        memset(&v2, 0, sizeof(v2));
        memset(&s1, 0, sizeof(s1));
        memset(&s2, 0, sizeof(s2));
        memset(&tpfunc, 0, sizeof(tpfunc));
        memset(&tpvoid, 0, sizeof(tpvoid));
        tpvoid.type = bt_void;
        tpfunc.type = bt_func;
        s1.tp = tp1;
        if (tp2)
            s2.tp = tp2;
        else
            s2.tp = &tpvoid;
        v1.nodetype = en_void;
        v1.v.p[1] = &v2;
        v1.v.p[0] = ep1;
        v2.v.p[0] = ep2;
        if (isstructured(tp1))
        {
            tpfunc.lst.head = &s2;
            spy = spx = search(cpp_funcname_tab[lst + IT_THRESHOLD], &tp1->lst);
            if (spx)
                spx = funcovermatch(spx, &tpfunc, &v1, FALSE, FALSE);
            if (!spx && spy)
                spx = lookupTemplateFunc(spy, &tpfunc, spy->name);
            if (spx)
                memfunc = TRUE;
        } if (!spx && (isstructured(tp1) || tp2 && isstructured(tp2)))
        {
            tpfunc.lst.head = &s1;
            if (tp2)
                s1.next = &s2;
            else
                s1.next = 0;
            spy = spx = gsearch(cpp_funcname_tab[lst + IT_THRESHOLD]);
            if (spx)
                spx = funcovermatch(spx, &tpfunc, &v1, FALSE, FALSE);
            if (!spx && spy)
                spx = lookupTemplateFunc(spy, &tpfunc, spy->name);
        }
        if (spx)
        {
            ENODE *qnode;
            spx->mainsym->extflag = TRUE;
            rtp = spx->tp->btp;
            if (memfunc)
            {
                if (tp2)
                    rte = makeparam(*ep2, rte, spx->tp->lst.head->tp);
            }
            else
            {
                rte = makeparam(*ep1, 0, spx->tp->lst.head->tp);
                if (tp2)
                    rte = makeparam(*ep2, rte, spx->tp->lst.head->next->tp);
            }
            if (rte)
                rte = makenode(en_cp, rte, 0);
            else
                rte = makenode(en_void, 0, 0);
            rnode = makenode(en_napccon, spx, 0);
            rnode = makenode(en_void, rnode, rnode);
            rte = makenode(en_void, rnode, rte);
            qnode = makeintnode(en_icon, retvalsize(spx->tp->btp));
            if (isstructured(rtp))
            {
                rte = makenode(en_fcallb, qnode, rte);
                (rte)->size = rtp->size;
                structured = TRUE;
            }
            else
                rte = makenode(en_fcall, qnode, rte);
            if (memfunc && !(spx->value.classdata.cppflags &PF_STATIC))
            {
                rte = makenode(en_thiscall,  *ep1, rte);
            }
            if (structured && (lastst == dot || lastst == pointsto))
                rte = immed_callblock(0, rte, rtp, rtp->size);
            rte = doinline(rte);
            *ep1 = rte;
            goodcode |= GF_ASSIGN;
        }
        return rtp;
    }
int absrank(TYP *tp)
{
    switch(tp->type) {
        case bt_bool:
            return 0;
        case bt_char:
        case bt_unsignedchar:
            return 1;
        case bt_short:
        case bt_unsignedshort:
            return 2;
        case bt_enum:
        case bt_int:
        case bt_unsigned:
            return 3;
        case bt_long:
        case bt_unsignedlong:
            return 4;
        case bt_longlong:
        case bt_unsignedlonglong:
            return 5;
        case bt_float:
        case bt_fimaginary:
        case bt_fcomplex:
            return 6;
        case bt_double:
        case bt_rimaginary:
        case bt_rcomplex:
            return 7;
        case bt_longdouble:
        case bt_lrimaginary:
        case bt_lrcomplex:
            return 8;        
    }
    return -1;
}
//-------------------------------------------------------------------------

    TYP *ambigcontext(TYP *tp, int *rv)
    {
        SYM *sp;
        if (tp->type != bt_defunc)
            return tp;
        if (tp->lst.head != tp->lst.tail)
        {
            gensymerror(ERR_AMBIGCONTEXT, tp->sp->name);
            *rv = 1;
        }
        sp = tp->lst.head;
        return sp->tp;
    }
TYP *inttype(enum e_bt t1)
{
       switch(t1) {
        default:
        case bt_int:
        case bt_unsigned:
            return maketype(t1,stdintsize);
            break ;
        case bt_long:
        case bt_unsignedlong:
            return maketype(t1,stdlongsize);
            break ;
        case bt_longlong:
        case bt_unsignedlonglong:
            return maketype(t1,stdlonglongsize);
            break ;
       }
}
TYP *forcefit(ENODE **node1, TYP *tp1, ENODE **node2, TYP *tp2, int allowpointers, int multop)
/*
 * compare two types and determine if they are compatible for purposes
 * of the current operation.  Return an appropriate type.  Also checks for
 * dangerous pointer conversions...
 */
{
    TYP *tp3;
    int isctp1, isctp2, ismtp1;
    if (tp1->type == bt_matchall)
        return tp1;
    if (tp2->type == bt_matchall)
        return tp2;
//	if ((tp1->type == bt_pointer ||tp1->type == bt_farpointer)
//		&& (tp2->type == bt_pointer || tp2->type == bt_farpointer))
//	{
//		if (!checkexactpointer(tp1, tp2))
//			generror(ERR_SUSPICIOUS,0,0);
//	}
    if (tp2->type == bt_defunc)
    {
        SYM *sp2 = funcovermatch(tp2->sp, tp1->type == bt_pointer ? tp1->btp:
            tp1, 0, TRUE, FALSE);
        if (sp2)
        {
            *node2 = makenode(en_napccon, (void*)sp2, 0);
            tp2 = sp2->tp;
        } 
        else
            gensymerror(ERR_CANTRESOLVE, tp2->sp->name);
    }
    if (tp1->type == bt_defunc)
    {
        SYM *sp2 = funcovermatch(tp1->sp, tp2->type == bt_pointer ? tp2->btp:
            tp2, 0, TRUE, FALSE);
        if (sp2)
        {
            *node1 = makenode(en_napccon, (void*)sp2, 0);
            tp1 = sp2->tp;
        }
        else
            gensymerror(ERR_CANTRESOLVE, tp1->sp->name);
    }
    if (tp1->type == bt_ref)
        tp1 = tp1->btp;
    if (tp2->type == bt_ref)
        tp2 = tp2->btp;
    tp1 = basictype(tp1);
    tp2 = basictype(tp2);
    isctp1 = isscalar(tp1);
    isctp2 = isscalar(tp2);
   
//    if (isctp1 && isctp2 && tp1->type == tp2->type)
//        return tp1 ;
    if (tp1->type >= bt_float  && tp1->type <= bt_lrcomplex|| tp2->type >= bt_float && tp2->type <= bt_lrcomplex) {
        int isim1;
        int isim2;
		int iscplx1;
		int iscplx2;
		int isflt1;
		int isflt2;
		int rank;
		// now convert any non-float to a float...
		if (tp1->type < bt_float)
		{
			promote_type(tp1 = tp2, node1);
		}
		if (tp2->type < bt_float)
		{
			promote_type(tp2 = tp1, node2);
		}
    
        isim1 = tp1->type >= bt_fimaginary && tp1->type <= bt_lrimaginary;
        isim2 = tp2->type >= bt_fimaginary && tp2->type <= bt_lrimaginary;
		iscplx1 = tp1->type >= bt_fcomplex && tp1->type <= bt_lrcomplex;
		iscplx2 = tp2->type >= bt_fcomplex && tp2->type <= bt_lrcomplex;
		isflt1 = tp1->type >= bt_float && tp1->type <= bt_longdouble;
		isflt2 = tp2->type >= bt_float && tp2->type <= bt_longdouble;

		if (iscplx1 && iscplx2 || isim1 && isim2 || isflt1 && isflt2)
		{
			TYP *rv;
			if (tp1->type > tp2->type)
			{
				promote_type(tp1, node2);
				rv = tp2;
			}
			else if (tp1->type < tp2->type)
			{
				promote_type(tp2, node1);
				rv = tp2;
			}
			else
				rv = tp1;
			if (multop)
			{
				switch (rv->type)
				{
					case bt_fimaginary:
						rv = &stdfloat;
						break;
					case bt_rimaginary:
						rv = &stddouble;
						break;
					case bt_lrimaginary:
						rv = &stdlongdouble;
						break;
				}
			}
			return rv;
		}
		// here on out one of the types should be imaginary or complex, the other
		// is imaginary complex or float
		// we don't do type promotions on the arguments here but instead defer that
		// to the backend for the sake of optimizations...
		
		if (tp1->type == bt_lrcomplex || tp1->type == bt_lrimaginary || tp1->type == bt_longdouble)
			rank = 2;
		else if (tp2->type == bt_lrcomplex || tp2->type == bt_lrimaginary || tp2->type == bt_longdouble)
			rank = 2;
		else if (tp1->type == bt_rcomplex || tp1->type == bt_rimaginary || tp1->type == bt_double)
			rank = 1;
		else if (tp2->type == bt_rcomplex || tp2->type == bt_rimaginary || tp2->type == bt_double)
			rank = 1;
		else
			rank = 0;
		if (iscplx1 || iscplx2)
		{
			switch (rank)
			{
				case 2:
					return &stdlrcomplex;
				default:
				case 1:
					return &stdrcomplex;
				case 0:
					return &stdcomplex;
			}
		}
		if (isim1 || isim2)
		{
			if (multop)
			{
				switch (rank)
				{
					case 2:
						return &stdlrimaginary;
					default:
					case 1:
						return &stdrimaginary;
					case 0:
						return &stdimaginary;
				}
			}
			else
			{
				switch (rank)
				{
					case 2:
						return &stdlrcomplex;
					default:
					case 1:
						return &stdrcomplex;
					case 0:
						return &stdcomplex;
				}
			}
		}
		DIAG("forcefit: unknown floating type");
		return &stddouble;
    }
    if (isctp1 && isctp2) {
        TYP *rv ;
       enum e_bt t1, t2;
       t1 = tp1->type;
       t2 = tp2->type;
       if (t1 == bt_unsignedchar || t1 == bt_unsignedshort)
         t1 = bt_unsigned;
       if (t1 < bt_int)
         t1= bt_int;
       if (t2 == bt_unsignedchar || t2 == bt_unsignedshort)
         t2 = bt_unsigned;
       if (t2 < bt_int)
         t2= bt_int;
       if (t1 != t2) {
          if (t2 > t1)
            t1 = t2;                
       }
       rv = inttype(t1);
//#ifdef ICODE
       if (rv->type != tp1->type || rv->type != tp2->type)
	   {
         promote_type(rv, node1);
         promote_type(rv,node2);
	   }
//#endif
       return rv;
    } else { // have a pointer or other exceptional case
        if (tp1->type == bt_defunc)
            if (exactype(tp1, tp2, FALSE))
                return tp1;
        if (tp1->type == bt_void && tp2->type == bt_void)
            return tp1;
        if (tp1->type <= bt_unsignedlonglong && (tp2->type == bt_pointer || 
                tp2->type == bt_farpointer ||
                tp2->type == bt_segpointer ||
                tp2->type == bt_func || tp2->type == bt_ifunc)) {
            if (node1 && isintconst((*node1)->nodetype))
            {
                    if ((*node1)->v.i == 0)
                        return tp2;
            }
            if (tp1->type <= bt_unsignedshort && allowpointers >= 0)
                generror(ERR_SHORTPOINTER,0,0);
            if (!prm_cplusplus || allowpointers < 0)
                if (allowpointers)
                    return  &stdint;
                else
                    return tp2;
        }
        if (tp2->type <= bt_unsignedlonglong && (tp1->type == bt_pointer || 
                tp1->type == bt_farpointer ||
                tp1->type == bt_segpointer ||
                tp1->type == bt_func || tp1->type == bt_ifunc)) {
            if (node2 && isintconst((*node2)->nodetype))
            {
                    if ((*node2)->v.i == 0)
                        return tp1;
            }
            if (tp1->type <= bt_unsignedshort && allowpointers >= 0)
                generror(ERR_SHORTPOINTER,0,0);
            if (!prm_cplusplus || allowpointers < 0)
                if (allowpointers)
                    return  &stdint;
                else
                    return tp1;
        }
        if (isstructured(tp1)) {
            if (exactype(tp1, tp2, FALSE))
                return tp1;
			if (prm_cplusplus)
            	cppcast(tp2, tp1, node1, FALSE);
			else
				generror(ERR_ILLSTRUCT, 0, 0);
            return tp2;
        }
        if (isstructured(tp2)) {
            if (exactype(tp1, tp2, FALSE))
                return tp2;
			if (prm_cplusplus)
            	cppcast(tp1, tp2, node1, FALSE);
			else
				generror(ERR_ILLSTRUCT, 0, 0);
            return tp1;
        }
        if (tp1->type == bt_func || tp1->type == bt_ifunc)
            if (tp2->type == bt_func || tp2->type == bt_ifunc || tp2->type ==
                    bt_pointer || tp2->type == bt_farpointer || tp2->type ==
                    bt_segpointer)
                return tp1;
        if (tp2->type == bt_func || tp2->type == bt_ifunc)
            if (tp1->type == bt_func || tp1->type == bt_ifunc || tp1->type ==
                    bt_pointer || tp1->type == bt_farpointer || tp1->type ==
                    bt_segpointer)
                return tp2;
        if (tp1->type == bt_pointer || tp1->type == bt_farpointer || tp1->type == bt_segpointer)
            if (tp2->type == bt_pointer || tp2->type == bt_farpointer || tp2->type == bt_segpointer)
                if (allowpointers)
                    return  &stdint;
                else
				{
                    return tp1;
				}
    }
        if (prm_cplusplus)
        {
            int toerr = 0;
            TYP *tp3 = ambigcontext(tp1, &toerr);
            TYP *tp4 = ambigcontext(tp2, &toerr);
            if (!toerr)
                genmismatcherror(tp4, tp3);
        }
        else
            generror(ERR_MISMATCH, 0, 0);
    return tp1;
}
TYP *asnfit(ENODE **node1, TYP *tp1, ENODE **node2, TYP *tp2, int matchref)
/*
 * compare two types and determine if they are compatible for purposes
 * of the current operation.  Return an appropriate type.  Also checks for
 * dangerous pointer conversions...
 */
{
    TYP *tp3;
    int isctp1, isctp2;
    if (tp1->type == bt_matchall)
        return tp2;
    if (tp2->type == bt_matchall)
        return tp1;
	if (tp1->type == bt_farpointer && tp2->type == bt_pointer)
		return tp1;
//	if ((tp1->type == bt_pointer ||tp1->type == bt_farpointer)
//		&& (tp2->type == bt_pointer || tp2->type == bt_farpointer))
//	{
//		if (!checkexactpointer(tp1, tp2))
//			generror(ERR_SUSPICIOUS,0,0);
//	}
    if (tp2->type == bt_defunc)
    {
        SYM *sp2 = funcovermatch(tp2->sp, tp1->type == bt_pointer ? tp1->btp:
            tp1, 0, TRUE, FALSE);
        if (sp2)
        {
            *node2 = makenode(en_napccon, (void*)sp2, 0);
            tp2 = sp2->tp;
        } 
        else
            gensymerror(ERR_CANTRESOLVE, tp2->sp->name);
    }
    if (tp1->type == bt_defunc)
    {
        SYM *sp2 = funcovermatch(tp1->sp, tp2->type == bt_pointer ? tp2->btp:
            tp2, 0, TRUE, FALSE);
        if (sp2)
        {
            *node1 = makenode(en_napccon, (void*)sp2, 0);
            tp1 = sp2->tp;
        }
        else
            gensymerror(ERR_CANTRESOLVE, tp1->sp->name);
    }
        if (matchref && tp1->type == bt_memberptr && tp2->type == bt_memberptr)
        {
            if (exactype(tp1, tp2, FALSE))
                return tp1;
        }
    if (tp1->type == bt_ref)
        tp1 = tp1->btp;
    if (tp2->type == bt_ref)
        tp2 = tp2->btp;
    tp1 = basictype(tp1);
    tp2 = basictype(tp2);
    isctp1 = isscalar(tp1);
    isctp2 = isscalar(tp2);
    if (isctp1 && isctp2) {
        if (tp1->type > tp2->type)
        {
            promote_type(tp1, node2);
        }
#ifdef XXXXX
        if (absrank(tp1) < absrank(tp2))
            generror(ERR_LOSTCONV,0,0);
#endif
        return tp1;
    }
    else if (isstructured(tp1)) {
            if (exactype(tp1, tp2, FALSE))
                return tp1;
            if (cppcast(tp2, tp1, node1, FALSE))
                return tp2;
    } else if (tp1->type == bt_pointer || tp1->type == bt_farpointer || 
                tp1->type == bt_segpointer) 
    {
            if (scalarnonfloat(tp2) && node2&& isintconst((*node2)->nodetype))
            {
                    if ((*node2)->v.i == 0)
                        return tp1;
            }
            if (prm_cplusplus)
              if (!exactype(tp1, tp2, FALSE))
              {
                if (tp2->type == bt_memberptr)
                    goto error;
                if (cppcast(tp1, tp2, node2, FALSE))
                    return tp1;
                goto error;
              }
              else
                return tp1;

			if (tp2->type == tp1->type)
				return tp1; 
            if (tp2->type == bt_func || tp2->type == bt_ifunc)
                return tp1;
            if (scalarnonfloat(tp2)) {
            if (tp1->type <= bt_unsignedshort)
                    generror(ERR_SHORTPOINTER,0,0);
                return tp1;
            }
    } else if (scalarnonfloat(tp1) && tp2->type == bt_pointer || tp2->type == bt_farpointer ||
                tp2->type == bt_segpointer) 
    {
            if (tp1->type <= bt_unsignedshort)
                generror(ERR_SHORTPOINTER,0,0);
            return tp1;
    }
error:
        if (prm_cplusplus)
        {
            int toerr = 0;
            TYP *tp3 = ambigcontext(tp1, &toerr);
            TYP *tp4 = ambigcontext(tp2, &toerr);
            if (!toerr)
                genmismatcherror(tp4, tp3);
        }
        else
            generror(ERR_MISMATCH, 0, 0);
    return tp1;
}

//-------------------------------------------------------------------------

int isscalar(TYP *tp)
/*
 * this is misnamed... it checks for ANY basic numeric type
 */
{
    if (tp->type == bt_cond)
        tp = tp->btp;
    switch (tp->type)
    {
        case bt_float:
        case bt_double:
        case bt_longdouble:
        case bt_bool:
        case bt_char:
        case bt_unsignedchar:
        case bt_short:
        case bt_unsignedshort:
        case bt_long:
        case bt_unsigned:
        case bt_enum:
        case bt_int:
        case bt_unsignedlong:
        case bt_unsignedlonglong:
        case bt_longlong:
        case bt_rcomplex:
        case bt_fcomplex:
        case bt_lrcomplex:
        case bt_rimaginary:
        case bt_fimaginary:
        case bt_lrimaginary:
            return TRUE;
        default:
            return FALSE;
    }
}
int scalarnoncomplex(TYP *tp)
/*
 * this is misnamed... it checks for ANY basic numeric type
 */
{
    if (tp->type == bt_cond)
        tp = tp->btp;
    switch (tp->type)
    {
        case bt_float:
        case bt_double:
        case bt_longdouble:
        case bt_bool:
        case bt_char:
        case bt_unsignedchar:
        case bt_short:
        case bt_unsignedshort:
        case bt_long:
        case bt_unsigned:
        case bt_enum:
        case bt_int:
        case bt_unsignedlong:
        case bt_unsignedlonglong:
        case bt_longlong:
            return TRUE;
        default:
            return FALSE;
    }
}

//-------------------------------------------------------------------------

int scalarnonfloat(TYP *tp)
{
    if (tp->type == bt_cond)
        tp = tp->btp;
    switch (tp->type)
    {
        case bt_bool:
        case bt_char:
        case bt_unsignedchar:
        case bt_short:
        case bt_unsignedshort:
        case bt_long:
        case bt_unsigned:
        case bt_enum:
        case bt_int:
        case bt_unsignedlong:
        case bt_unsignedlonglong:
        case bt_longlong:
            return TRUE;
    }
    return FALSE;
}

//-------------------------------------------------------------------------

void checknp(TYP *tp1, TYP *tp2, ENODE *ep1, ENODE *ep2)
/*
 * look for non-portable pointer conversions
 */
{

        if (prm_cplusplus)
            return ;
    if (tp1->type == bt_pointer || tp2->type == bt_pointer || tp1->type ==
        bt_farpointer || tp2->type == bt_farpointer || tp2->type ==
        bt_segpointer)
        if (tp1->type != tp2->type)
            if ((ep1 && isintconst(ep1->nodetype) && ep1->v.i != 0) || (ep2 && isintconst(ep2
                ->nodetype) && ep2->v.i != 0))
                generror(ERR_NONPORT, 0, 0);
}

//-------------------------------------------------------------------------

int isunsignedresult(TYP *tp1, TYP *tp2, ENODE *ep1, ENODE *ep2)
{
    int t1 = tp1->type;
    int t2 = tp2->type;
    if (t1 == bt_unsigned || t1 == bt_unsignedchar || t1 == bt_unsignedshort ||
        t1 == bt_pointer || t1 == bt_unsignedlong || t1 == bt_unsignedlonglong 
        || t2 == bt_unsigned || t2 == bt_unsignedchar || t2 == bt_unsignedshort
        || t2 == bt_pointer || t2 == bt_unsignedlong || t2 ==
        bt_unsignedlonglong)
        return TRUE;
    t1 = ep1->nodetype;
    t2 = ep2->nodetype;
    if (t1 == en_lucon || t1 == en_iucon || t1 == en_cub || t1 == en_cuw || t1 
        == en_cul || t1 == en_cui || t1 == en_llucon || t1 == en_cull || t2 == en_lucon || t2 
        == en_iucon || t2 == en_cub || t2 == en_cuw || t2 == en_cul || t2 == en_cui || t2 ==
        en_llucon || t2 == en_cull)
        return TRUE;
    return FALSE;

}

//-------------------------------------------------------------------------

TYP *multops(ENODE **node)
/*
 *      multops parses the multiply priority operators. the syntax of
 *      this group is:
 *
 *              unary
 *              multop * unary
 *              multop / unary
 *              multop % unary
 */
{
    ENODE *ep1,  *ep2;
    TYP *tp1,  *tp2;
    int oper;
    tp1 = unary(&ep1);
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    if (tp1->val_flag && (tp1->cflags &DF_AUTOREG))
    {
        gensymerror(ERR_NOANDREG, tp1->sp->name);
    }
    while (lastst == star || lastst == divide || lastst == modop)
    {
        oper = lastst;
        getsym(); /* move on to next unary op */
        tp2 = unary(&ep2);
        if (tp2 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = ep1;
            return tp1;
        }
        if (tp2->val_flag && (tp2->cflags &DF_AUTOREG))
        {
            gensymerror(ERR_NOANDREG, tp2->sp->name);
        }
        goodcode &= ~GF_ASSIGN;
        if (tp2 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = ep1;
            return tp1;
        }
            if (prm_cplusplus)
            {
                TYP *tptr = mathoverload(oper, tp1, tp2, &ep1, &ep2);
                if (tptr)
                {
                    tp1 = tptr;
                    continue;
                }
            }
        tp1 = forcefit(&ep1, tp1, &ep2, tp2, FALSE, TRUE);
        switch (oper)
        {
            case star:
                if (isunsignedresult(tp1, tp2, ep1, ep2))
                    ep1 = makenode(en_umul, ep1, ep2);
                else
                    ep1 = makenode(en_mul, ep1, ep2);
                break;
            case divide:
                if (isunsignedresult(tp1, tp2, ep1, ep2))
                    ep1 = makenode(en_udiv, ep1, ep2);
                else
                    ep1 = makenode(en_div, ep1, ep2);
                break;
            case modop:
                if (isunsignedresult(tp1, tp2, ep1, ep2))
                    ep1 = makenode(en_umod, ep1, ep2);
                else
                    ep1 = makenode(en_mod, ep1, ep2);
                floatcheck(ep1);
                break;
        }
        if (pointermode(tp1,tp2) == 3)
            generror(ERR_ILLUSEPTR,0,0);
    }
    *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *addops(ENODE **node)
/*
 *      addops handles the addition and subtraction operators.
 */
{
    ENODE *ep1,  *ep2,  *ep3;
    TYP *tp1,  *tp2;
    int oper;
    tp1 = multops(&ep1);
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    while (lastst == plus || lastst == minus)
    {
        oper = lastst;
        getsym();
        tp2 = multops(&ep2);
        goodcode &= ~GF_ASSIGN;
        if (tp2 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = ep1;
            return tp1;
        }
            if (oper == plus && pointermode(tp1,tp2) == 3)
                generror(ERR_ILLPTRADD,0,0);
            if (prm_cplusplus)
            {
                TYP *tptr = mathoverload(oper, tp1, tp2, &ep1, &ep2);
                if (tptr)
                {
                    tp1 = tptr;
                    continue;
                }
            }
        if (tp1->type != bt_segpointer && tp2->type != bt_segpointer)
        {
            if (tp1->type == bt_pointer || tp1->type == bt_farpointer)
            {
                if (tp2->type == tp1->type)
                {
                    forcefit(&ep1, tp1, &ep2, tp2, FALSE, FALSE);
                    ep1 = makenode(oper == plus ? en_add : en_sub, ep1, ep2);
                    ep1->cflags = ep1->v.p[0]->cflags | ep2->cflags;
                    if (tp1->btp->size == 0)
                        generror(ERR_ZEROPTR, 0, 0);
                    else
                    if (tp1->btp->val_flag &VARARRAY)
                    {
                        ep3 = makenode(tp1->sp->storage_class == sc_auto ?
                            en_autocon : en_nacon, (void*)tp1->sp, 0);
                        ep3 = makenode(en_add, ep3, makeintnode(en_icon,
                            stdaddrsize + stdintsize));
                        ep3 = makenode(en_a_ref, ep3, 0);
                        ep1 = makenode(en_pdiv, ep1, ep3);
                    }
                    else
                    if (tp1->btp->size > 1)
                    {
                        ep3 = makeintnode(en_icon, tp1->btp->size);

                        ep1 = makenode(en_pdiv, ep1, ep3);
                    }
                    ep1->cflags = ep1->v.p[0]->cflags;
                    tp1 = &stdint;
                    continue;
                }
                else
                {
                    if (!isscalar(tp2))
                        tp2 = forcefit(0, tp1, &ep2, tp2, FALSE, FALSE);
					else
						promote_type(tp2 = &stdint, &ep2);
                    if (tp1->btp->size == 0)
                        generror(ERR_ZEROPTR, 0, 0);
                    if (tp1->btp->val_flag &VARARRAY)
                    {
                        ep3 = makenode(tp1->sp->storage_class == sc_auto ?
                            en_autocon : en_nacon, (void*)tp1->sp, 0);
                        ep3 = makenode(en_add, ep3, makeintnode(en_icon,
                            stdaddrsize + stdintsize));
                        ep3 = makenode(en_a_ref, ep3, 0);
                    }
                    else
					{
						if (tp1->val_flag)
						{
							tp2 = maketype(tp1->type, stdaddrsize);
							tp2->btp = tp1->btp;
							tp1 = tp2;
						}
                        ep3 = makeintnode(en_icon, tp1->btp->size);
					}
                    ep2 = makenode(en_pmul, ep3, ep2);
                    ep2->cflags = ep2->v.p[1]->cflags;
                }
            }
            else if (tp2->type == bt_pointer || tp2->type == bt_farpointer)
            {
                if (!isscalar(tp1))
                    tp1 = forcefit(0, tp2, &ep1, tp1, FALSE, FALSE);
				else
					promote_type(tp1 = &stdint, &ep1);
                if (tp2->btp->size == 0)
                    generror(ERR_ZEROPTR, 0, 0);
                if (tp2->btp->val_flag &VARARRAY)
                {
                    ep3 = makenode(tp2->sp->storage_class == sc_auto ?
                        en_autocon : en_nacon, (void*)tp2->sp, 0);
                    ep3 = makenode(en_add, ep3, makeintnode(en_icon,
                        stdaddrsize + stdintsize));
                    ep3 = makenode(en_a_ref, ep3, 0);
                }
                else
				{
					if (tp2->val_flag)
					{
						tp1 = maketype(tp2->type, stdaddrsize);
						tp1->btp = tp2->btp;
						tp2 = tp1;
					}
                    ep3 = makeintnode(en_icon, tp2->btp->size);
				}
                ep1 = makenode(en_pmul, ep3, ep1);
                ep1->cflags = ep1->v.p[1]->cflags;
                tp1 = tp2;
            }
            else
                tp1 = forcefit(&ep1, tp1, &ep2, tp2, FALSE, FALSE);
        }
        else
            tp1 = forcefit(&ep1, tp1, &ep2, tp2, FALSE, FALSE);
        ep1 = makenode(oper == plus ? en_add : en_sub, ep1, ep2);
        ep1->cflags = ep1->v.p[1]->cflags | ep2->cflags;
    }
    exit: *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *shiftop(ENODE **node)
/*
 *      shiftop handles the shift operators << and >>.
 */
{
    ENODE *ep1,  *ep2;
    TYP *tp1,  *tp2;
    int oper;
    tp1 = addops(&ep1);
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    while (lastst == lshift || lastst == rshift)
    {
        oper = (lastst == lshift);
        getsym();
        tp2 = addops(&ep2);
        goodcode &= ~GF_ASSIGN;
        if (tp2 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = ep1;
            return tp1;
        }
        else
        {
            //                        tp1 = forcefit(&ep1,tp1,&ep2,tp2,TRUE,FALSE,FALSE);
            if (pointermode(tp1,tp2))
                generror(ERR_ILLUSEPTR,0,0);
            if (tp1->size > -stdintsize && tp1->size < stdintsize) {
                tp1 = &stdint;
                promote_type(tp1, &ep1);
            }  
			if (!isintconst(ep2->nodetype))
	            ep2 = makenode(en_cl_reg, ep2, 0);
            if (tp1->type == bt_unsigned || tp1->type == bt_unsignedchar || tp1
                ->type == bt_unsignedshort || tp1->type == bt_unsignedlong ||
                tp1->type == bt_unsignedlonglong)
                ep1 = makenode(oper ? en_lsh : en_rsh, ep1, ep2);
            else
                ep1 = makenode(oper ? en_alsh : en_arsh, ep1, ep2);
        }
        floatcheck(ep1);
    }
    *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *relation(ENODE **node)
/*
 *      relation handles the relational operators < <= > and >=.
 */
{
    ENODE *ep1,  *ep2;
    TYP *tp1,  *tp2;
    int nt;
    tp1 = shiftop(&ep1);
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    for (;;)
    {
        int st = lastst;
        if (lastst != lt && (lastst != gt || intemplateargs) && lastst != leq
            && lastst != geq)
            goto fini;
        getsym();
        tp2 = shiftop(&ep2);
        goodcode &= ~GF_ASSIGN;
        if (tp2 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = ep1;
            return tp1;
        }
        else
        {
                if (prm_cplusplus)
                {
                    TYP *tptr = mathoverload(st, tp1, tp2, &ep1, &ep2);
                    if (tptr)
                    {
                        tp1 = tptr;
                        continue;
                    }
                }
            switch (st)
            {

                case lt:
                    if (isunsignedresult(tp1, tp2, ep1, ep2))
                        nt = en_ult;
                    else
                        nt = en_lt;
                    break;
                case gt:
                    if (isunsignedresult(tp1, tp2, ep1, ep2))
                        nt = en_ugt;
                    else
                        nt = en_gt;
                    break;
                case leq:
                    if (isunsignedresult(tp1, tp2, ep1, ep2))
                        nt = en_ule;
                    else
                        nt = en_le;
                    break;
                case geq:
                    if (isunsignedresult(tp1, tp2, ep1, ep2))
                        nt = en_uge;
                    else
                        nt = en_ge;
                    break;
            }
            checknp(tp1, tp2, ep1, ep2);
            tp1 = forcefit(&ep1, tp1, &ep2, tp2, TRUE, FALSE);
            tp1 = &stdint;
            ep1 = makenode(nt, ep1, ep2);
        }
    }
    fini:  *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *equalops(ENODE **node)
/*
 *      equalops handles the equality and inequality operators.
 */
{
    ENODE *ep1,  *ep2;
    TYP *tp1,  *tp2;
    int oper;
    tp1 = relation(&ep1);
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    while (lastst == eq || lastst == neq)
    {
        oper = lastst;
        getsym();
        tp2 = relation(&ep2);
        goodcode &= ~GF_ASSIGN;
        checknp(tp1, tp2, ep1, ep2);
        if (tp2 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = ep1;
            return tp1;
        }
        else
        {
                if (prm_cplusplus)
                {
                    TYP *tptr = mathoverload(oper, tp1, tp2, &ep1, &ep2);
                    if (tptr)
                    {
                        tp1 = tptr;
                        continue;
                    }
                }
            if (tp1->type == bt_memberptr)
            {
                ENODE *epx = ep2;
                if (!isintconst(epx->nodetype) || epx->v.i != 0)
                {
                    tp2 = FindMemberPointer(tp1, tp2, &ep2);
                    if (!exactype(tp1, tp2, FALSE) && (!isintconst(epx->nodetype) ||
                        epx->v.i != 0))
                        generror(ERR_MEMBERPTRMISMATCH, 0, 0);
                    else
                        tp2 = deref(&ep2, tp2);
                }
            }
            else if (tp2->type == bt_memberptr)
            {
                ENODE *epx = ep1;
                if (!isintconst(epx->nodetype) || epx->v.i != 0)
                {
                    tp1 = FindMemberPointer(tp2, tp1, &ep1);
                    if (!exactype(tp1, tp2, FALSE) && (!isintconst(epx->nodetype) ||
                        epx->v.i != 0))
                        generror(ERR_MEMBERPTRMISMATCH, 0, 0);
                    else
                        tp1 = deref(&ep1, tp1);
                }
            }
            else
                tp1 = forcefit(&ep1, tp1, &ep2, tp2, TRUE, FALSE);
            tp1 = &stdint;
            ep1 = makenode(oper == eq ? en_eq : en_ne, ep1, ep2);
        }
    }
    *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *binop(ENODE **node, exprFunc xfunc, int nt, int sy)
/*
 *      binop is a common routine to handle all of the legwork and
 *      error checking for bitand, bitor, bitxor, andop, and orop.
 */
{
    ENODE *ep1,  *ep2;
    TYP *tp1,  *tp2;
    tp1 = (*xfunc)(&ep1);
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    while (lastst == sy)
    {
        getsym();
        tp2 = (*xfunc)(&ep2);
        goodcode &= ~GF_ASSIGN;
        if (tp2 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            *node = ep1;
            return tp1;
        }
        else {
            if (sy != lor && sy != land && pointermode(tp1,tp2))
                generror(ERR_ILLUSEPTR,0,0);
            if (prm_cplusplus)
            {
                TYP *tptr = mathoverload(sy, tp1, tp2, &ep1, &ep2);
                if (tptr)
                {
                    tp1 = tptr;
                    continue;
                }
            }
        }
        {
			if (sy != lor && sy != land)
	            tp1 = forcefit(&ep1, tp1, &ep2, tp2, sy == lor || sy == land 
    	            ?  - 1: 0, FALSE);
            ep1 = makenode(nt, ep1, ep2);
        }
        if (sy != lor && sy != land)
            floatcheck(ep1);
        else
            tp1 = &stdint;
    }
    *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *bitand(ENODE **node)
/*
 *      the bitwise and operator...
 */
{
    return binop(node, equalops, en_and, and);
}

//-------------------------------------------------------------------------

TYP *bitxor(ENODE **node)
{
    return binop(node, bitand, en_xor, uparrow);
}

//-------------------------------------------------------------------------

TYP *bitor(ENODE **node)
{
    return binop(node, bitxor, en_or, or);
}

//-------------------------------------------------------------------------

TYP *andop(ENODE **node)
{
    return binop(node, bitor, en_land, land);
}

//-------------------------------------------------------------------------

TYP *orop(ENODE **node)
{
    return binop(node, andop, en_lor, lor);
}

//-------------------------------------------------------------------------

TYP *conditional(ENODE **node)
/*
 *      this routine processes the hook operator.
 */
{
    TYP *tp1,  *tp2,  *tp3,  *tpx;
    ENODE *ep1,  *ep2,  *ep3;
    tp1 = orop(&ep1); /* get condition */

    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    if (lastst == hook)
    {
        int gcode1, gcode2;
        getsym();
        if ((tp2 = expression(&ep2, 0)) == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            goodcode &= ~GF_ASSIGN;
            goto cexit;
        }
        needpunc(colon, 0);
        gcode2 = goodcode;
        goodcode &= ~GF_ASSIGN;
        if ((tp3 = conditional(&ep3)) == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            goodcode &= ~GF_ASSIGN;
            goto cexit;
        }
        if (ep3)
            ep3 = repcallb(ep3);
        gcode1 = gcode2 &goodcode;
        goodcode = (goodcode &~GF_ASSIGN) | gcode1;
        tpx = maketype(bt_cond, 0);
        if (!(tp2->type == tp3->type && tp2->type == bt_defunc))
            tp1 = forcefit(&ep2, tp2, &ep3, tp3, FALSE, FALSE);
        tpx->btp = tp1;
        tpx->lst.head = (SYM*)tp2;
        tpx->lst.tail = (SYM*)tp3;
        ep2 = makenode(en_void, ep2, ep3);
        ep1 = makenode(en_cond, ep1, ep2);
        opt4(&ep1, FALSE);
        tp1 = tpx;
        if ((tp3->type == bt_defunc || tp3->type == bt_func) && tp3->sp
            ->parentclass)
            generror(ERR_MUSTCALLMEMBER, 0, 0);
        if ((tp2->type == bt_defunc || tp2->type == bt_func) && tp2->sp
            ->parentclass)
            generror(ERR_MUSTCALLMEMBER, 0, 0);

/*
        if (tp2->type == bt_void && tp2->type == bt_void)
            goodcode |= GF_ASSIGN;
        else
            goodcode &= ~GF_ASSIGN;
*/
        tp1 = copytype(tp1, (tp2->cflags | tp3->cflags) &(DF_CONST | DF_VOL));
    }
    cexit: *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *autoasnop(ENODE **node, SYM *sp, int defalt)
/*
 * 			Handle assignment operators during auto init of local vars
 */
{
    ENODE *ep1,  *ep2;
    TYP *tp1,  *tp2;
    int needclosebr = FALSE;
    if (!defalt)
    {
        expr_runup[expr_updowncount] = 0;
        expr_rundown[expr_updowncount++] = 0;
    }
    getsym();
    if ((tp1 = sp->tp) == 0)
    {
        tp1 = &stdmatch;
        *node = makenode(en_nacon, &undef, 0);
        gensymerror(ERR_UNDEFINED, nm);
        tp1 = deref(&ep1, tp1);
        expr_updowncount--;
        return tp1; /* guard against untyped entries */
    }
    if (sp->storage_class != sc_auto && sp->storage_class != sc_autoreg)
        gensymerror(ERR_ILLCLASS2, sp->name);
    if (sp->storage_class == sc_auto)
        ep1 = makenode(en_autocon, sp, 0);
    else if (sp->storage_class == sc_autoreg)
        ep1 = makenode(en_autoreg, sp, 0);
    if (tp1)
        tp1->uflags |= UF_DEFINED | UF_USED | UF_ALTERED;
    if ((tp1->uflags &UF_CANASSIGN) && !(goodcode &GF_INLOOP))
        tp1->uflags |= UF_ASSIGNED;
    if (tp1->val_flag == 0)
    {
        tp1 = deref(&ep1, tp1);
    }
    else
    {
        /* handle static-like assignment of structures and arrays */
        /* dynamic assignments of one struc to another are handled
         * elsewhere */
        if (lastst == begin || lastst == sconst || lastst == lsconst)
        {
            if (prm_c99) 
            {
                *node = doeinit(ep1,sp->tp);
            }
            else
            {
                SYM *sp1;
                global_flag++;
                sp1 = makesym(sc_static);
                sp1->name = xalloc(strlen(sp->name) + 7);
                strcpy(sp1->name, "$$INIT");
                strcat(sp1->name, sp->name);
                sp1->tp = cponetype(sp->tp);
                sp1->staticlabel = TRUE;
                sp1->value.i = nextlabel++;
                ep2 = makeintnode(en_labcon, sp1->value.i);
                backup(assign);
                doinit(sp1);
                ep1 = makenode(en_moveblock, ep1, ep2);
                ep1->size = sp1->tp->size;
                sp->tp = sp1->tp;
                *node = ep1;
                global_flag--;
                expr_updowncount--;
            }
            return sp->tp;
        }
    }
    if (tp1 == 0)
    {
        expr_updowncount--;
        return 0;
    }
    if (lastst == begin)
    {
        needclosebr = TRUE;
        getsym();
    }
    tp2 = asnop(&ep2, tp1);
    if (tp2 == 0)
    {
        generror(ERR_LVALUE, 0, 0);
        *node = makenode(en_nacon, &undef, 0);
    }
    else
    {
            if (prm_cplusplus)
            {
                TYP *tp = mathoverload(pointsto, tp1, tp2, &ep1, &ep2);
                if (tp)
                {
                    tp1 = tp;
                    goto finit;
                }
            }
        if (isstructured(tp1))
        {
			tp2 = basictype(tp2);
            if (!checktypeassign(tp1, tp2))
            {
                generror(ERR_ILLSTRUCT, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
            }
            else if (!exactype(tp1, tp2, FALSE))
            {
                generror(ERR_ILLSTRUCT, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
            }
            else
            {
                int thiscall = 0;
                ENODE *epn = ep2;
                checknp(tp1, tp2, ep1, ep2);
                    if (ep2->nodetype == en_thiscall)
                    {
                        ep2 = ep2->v.p[1];
                        thiscall = 1;
                    }
                if (ep2->nodetype == en_fcallb || ep2->nodetype == en_pfcallb 
                    || ep2->nodetype == en_sfcallb)
				{
                    ep1 = cbll(ep1, ep2, tp2);
				}
                else
                {
                    checkstructconst(tp1);
                    ep1 = makenode(en_moveblock, ep1, ep2);
                    ep1->size = tp1->size;
                }
                    if (thiscall)
                    {
                        epn->v.p[1] = ep1;
                        ep1 = epn;
                    }
                *node = ep1;
            }
        }
        else
        {
            if (tp1->type == bt_ref)
            {
                ENODE *ep3;
                if (checktypeassign(tp1, tp2) || !(ep3 = declRef(sp, tp1, tp2, ep1,
                    ep2, sc_auto)))
                {
                    if (isintconst(ep2->nodetype) || isfloatconst(ep2->nodetype)
                        )
                    {
                        if (!isscalar(tp1->btp))
                            matchreflvalue(0, tp1, tp2);
                    }
                    else
                        matchreflvalue(0, tp1, tp2);
                    if (!isintconst(ep2->nodetype) && !isfloatconst(ep2
                        ->nodetype))
                    {
                        if (isscalar(tp2))
                        {
                            while (castvalue(ep2))
                                ep2 = ep2->v.p[0];
                            if (lvalue(ep2))
                                ep2 = ep2->v.p[0];
                        }
                    }
                    else
                    {
                        ENODE *node = dummyvar(tp1->btp->size, tp1->btp, 0);
                        deref(&node, tp1->btp);
                        ep2 = makenode(en_refassign, node, ep2);
                        gensymerror(ERR_TEMPINIT, sp->name);
                    }
                }
                else
                {
                    ep2 = ep3;
                }
            }
            else if (tp1->val_flag)
            {
                generror(ERR_LVALUE, 0, 0);
                *node = makenode(en_nacon, &undef, 0);
                expr_updowncount--;
                return tp1;
            }
            if (scalarnonfloat(tp1) && (sp->tp->cflags &DF_CONST) && sp
                ->storage_class == sc_static)
            {
                ENODE *ep4;
                opt4(&ep2, FALSE);
                ep4 = ep2;
                while (castvalue(ep4))
                    ep4 = ep4->v.p[0];
                if (isintconst(ep4->nodetype))
                {
                    sp->init = xalloc(sizeof(struct decldata));
                    sp->init->val.i = ep4->v.i;
                    sp->init->mode = dd_long;
                }

            }
            checkparmconst(tp2, tp1);

            checknp(tp1, tp2, ep1, ep2);
            if (tp1->type == bt_memberptr)
            {
                ENODE *epx = ep2;
                tp2 = FindMemberPointer(tp1, tp2, &ep2);
                if (!exactype(tp1, tp2, FALSE) && (!isintconst(epx->nodetype) || epx
                    ->v.i != 0))
                    generror(ERR_MEMBERPTRMISMATCH, 0, 0);
            } 
            else
            {
                if (tp1->type == bt_ref)
                    tp1 = tp1->btp;
                else
				{
                    tp1 = asnfit(&ep1, tp1, &ep2, tp2, TRUE);
				}
            }
                checkConstClass(ep1);
            if (tp1->type == bt_memberptr)
            {
                while (castvalue(ep1))
                    ep1 = ep1->v.p[0];
                if (lvalue(ep1))
                    ep1 = ep1->v.p[0];
                while (castvalue(ep2))
                    ep2 = ep2->v.p[0];
                if (lvalue(ep2))
                    ep2 = ep2->v.p[0];
                *node = makenode(en_moveblock, ep1, ep2);
                (*node)->size = stdmemberptrsize;
            }
            else
            {
                if (tp1->type == bt_bool)
                {
                    ep2 = makenode(en_cbool, ep2, 0);
                    typedconsts(ep2);
                }
            }
            *node = makenode(en_assign, ep1, ep2);

        }
    }
    finit: if (needclosebr)
        needpunc(end, 0);
    if (!defalt)
    {
        expr_updowncount--;
        if (expr_runup[expr_updowncount] || expr_rundown[expr_updowncount])
        {
            *node = makenode(en_dvoid,  *node, 0);
            (*node)->cflags |= DF_ACTUALEXPR;
        }
        if (expr_rundown[expr_updowncount])
        {
            ENODE *epq = expr_rundown[expr_updowncount];
            ENODE *epb = 0,  *eps = 0,  *epbt = 0,  *epst = 0;
            while (epq)
            {
                ENODE *epr = epq->v.p[1];
                epq->v.p[1] = 0;
                if (epq->cflags &DF_CALLRUNHEDGE)
                {
                    if (epb)
                        epbt = epbt->v.p[1] = epq;
                    else
                        epb = epbt = epq;
                }
                else
                {
                    if (eps)
                        epst = epst->v.p[1] = epq;
                    else
                        eps = epst = epq;
                }
                epq = epr;
            }
            if (eps)
                (*node)->v.p[1] = eps;
            if (epb)
                if (block_rundown)
                    block_rundown = makenode(en_void, epb, block_rundown);
                else
                    block_rundown = epb;
        }
        if (expr_runup[expr_updowncount])
            *node = makenode(en_void, expr_runup[expr_updowncount],  *node);
    }
    return (tp1);
}

//-------------------------------------------------------------------------

    void asncombine(ENODE **node)
    /*
     * a simple optimization which turns an equate into a functional equate
     * if possible.  Code gen proceeds a little more cleanly if this info
     * is known.
     */
    {
        ENODE *var = (*node)->v.p[0];
        ENODE *exp = (*node)->v.p[1];
        ENODE *var2;
        int op = 0;
        if (!exp || (*node)->nodetype != en_assign)
            return ;
        var2 = exp->v.p[0];
        switch (exp->nodetype)
        {
            case en_add:
            case en_addstruc:
                en_addcast: if (equalnode(var, var2))
                {
                    op = en_asadd;
                    break;
                }
                else
                    return ;
            case en_sub:
                if (equalnode(var, var2))
                {
                    op = en_assub;
                    break;
                }
                else
                    return ;
            case en_mul:
                if (equalnode(var, var2))
                {
                    op = en_asmul;
                    break;
                }
                else
                    return ;
            case en_umul:
                if (equalnode(var, var2))
                {
                    op = en_asumul;
                    break;
                }
                else
                    return ;
            case en_div:
                if (equalnode(var, var2))
                {
                    op = en_asdiv;
                    break;
                }
                else
                    return ;
            case en_udiv:
                if (equalnode(var, var2))
                {
                    op = en_asudiv;
                    break;
                }
                else
                    return ;
            case en_lsh:
                if (equalnode(var, var2))
                {
                    op = en_aslsh;
                    break;
                }
                else
                    return ;
            case en_alsh:
                if (equalnode(var, var2))
                {
                    op = en_asalsh;
                    break;
                }
                else
                    return ;
            case en_rsh:
                if (equalnode(var, var2))
                {
                    op = en_asrsh;
                    break;
                }
                else
                    return ;
            case en_arsh:
                if (equalnode(var, var2))
                {
                    op = en_asarsh;
                    break;
                }
                else
                    return ;
            case en_and:
                if (equalnode(var, var2))
                {
                    op = en_asand;
                    break;
                }
                else
                    return ;
            case en_or:
                if (equalnode(var, var2))
                {
                    op = en_asor;
                    break;
                }
                else
                    return ;
            case en_xor:
                if (equalnode(var, var2))
                {
                    op = en_asxor;
                    break;
                }
                else
                    return ;
            default:
                return ;
        }
        exp->nodetype = op;
        (*node) = exp;
    }

/* This checks an assignment ENODE to see if we are trying to modify
 * a class member from within a const member function
 */
void checkConstClass(ENODE *node)
{
    if (declclass && currentfunc)
    if (currentfunc->tp->cflags &DF_CONST)
    {
        while (castvalue(node))
            node = node->v.p[0];
        if (lvalue(node))
        {
            node = node->v.p[0];
            if (!lvalue(node))
            if (node->nodetype == en_addstruc)
            {
                while (node && node->nodetype == en_addstruc)
                    node = node->v.p[1];
                if (lvalue(node))
                {
                    node = node->v.p[0];
                    if (node->nodetype == en_autocon && node->v.sp == thissp)
                        generror(ERR_MODCONS, 0, 0);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------

TYP *asnop(ENODE **node, TYP *tptr)
/*
 *      asnop handles the assignment operators.
 */
{
    ENODE *ep1,  *ep2,  *ep3;
    TYP *tp1,  *tp2,  *oldastyp;
    int op;
    SYM *sp1;
    oldastyp = asntyp;
    if (tptr)
        asntyp = tptr;
    else
        asntyp = 0;
    tp1 = conditional(&ep1);
    lastsym = 0;
    if (!tptr)
        asntyp = tp1;
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    for (;;)
    {
        enum e_sym st = lastst;
        switch (lastst)
        {
            case lassign:
                op = en_lassign;
                if (goodcode &GF_INIF)
                    generror(ERR_BADEQUATE, 0, 0);
                goto ascomm;
            case assign:
                op = en_assign;
                if (goodcode &GF_INIF)
                    generror(ERR_BADEQUATE, 0, 0);

                ascomm: getsym();
                tp2 = asnop(&ep2, asntyp);
                ascomm2: ep3 = ep1;
                if (tp1)
                {
                    tp1->uflags |= UF_DEFINED | UF_ALTERED;
                }
                goodcode |= GF_ASSIGN;
                if ((tp1->uflags &UF_CANASSIGN) && !(goodcode &GF_INLOOP))
                    tp1->uflags |= UF_ASSIGNED;
                if (!lvalue(ep3) && tp1->type != bt_ref)
                if (ep3->cflags &DF_CONST)
                    generror(ERR_MODCONS, 0, 0);
                checkparmconst(tp2, tp1);
                    if (prm_cplusplus)
                    {
                        TYP *tp = mathoverload(st, tp1, tp2, &ep1, &ep2);
                        if (tp)
                        {
                            tp1 = tp;
                            break;
                        }
                    }
                if (tp2 == 0 || tp1 == 0)
                    generror(ERR_LVALUE, 0, 0);
                else
                {
                    int ptrmode = pointermode(tp1,tp2);
                     switch(st) {
                        case assign:
                        case lassign:
                             if (ptrmode == 2) {
                                if (tp2->type == bt_cond)
                                    tp2 = tp2->btp;
                                if (tp2->type != bt_func && tp2->type != bt_ifunc)
                                    if (!isintconst(ep2->nodetype) || ep2->v.i != 0)
                                      generror(ERR_NONPORT,0,0);
                             } else if (ptrmode == 1)
                                generror(ERR_NONPORT,0,0);
                             break;
                        case asplus:
                        case asminus:
                            break;
                        default:
                            if (ptrmode)
                                generror(ERR_ILLUSEPTR,0,0);
                            break;
                     } 
					tp2->uflags |= UF_USED;
                        if (tp1->type == bt_memberptr)
                        {
                            ENODE *epx = ep2;
                            tp2 = FindMemberPointer(tp1, tp2, &ep2);
                            if (!exactype(tp1, tp2, FALSE) && (!isintconst(epx
                                ->nodetype) || epx->v.i != 0))
                                generror(ERR_MEMBERPTRMISMATCH, 0, 0);
                            if (op != en_assign)
                                generror(ERR_NOASSIGNCOMPLEXMEMBERPTR, 0, 0);
                            while (castvalue(ep1))
                                ep1 = ep1->v.p[0];
                            if (lvalue(ep1))
                                ep1 = ep1->v.p[0];
                            while (castvalue(ep2))
                                ep2 = ep2->v.p[0];
                            if (lvalue(ep2))
                                ep2 = ep2->v.p[0];
                            ep1 = makenode(en_moveblock, ep1, ep2);
                            ep1->size = stdmemberptrsize;
                            break;
                        }
                        checkConstClass(ep1);
                    if (isstructured(tp1))
                    {
						tp2 = basictype(tp2);
                        if (st == assign && prm_cplusplus && isstructured(tp2)
                            && tp2->sp->mainsym != tp1->sp->mainsym)
                        {
                            SYM *sp = tp1->sp;
                            SYM *s1 = tp2->sp;
                            CLASSLIST *l = s1->value.classdata.baseclass;
                            while (l)
                            {
                                if (!strcmp(sp->name, l->data->name))
                                {
                                    break;
                                }
                                l = l->link;
                            }
                            if (!l)
                            {
                                generror(ERR_ILLSTRUCT, 0, 0);
                                *node = makenode(en_nacon, &undef, 0);
                            }
                            else
                            {
                                ENODE *epn,  *ept = 0;
                                cppcast(tp1, tp2, &ep2, FALSE);
                                epn = ep2;
                                if (epn->nodetype == en_thiscall)
                                {
                                    epn = epn->v.p[1];
                                    ept = ep2;
                                }
                                if (epn->nodetype == en_fcallb || epn->nodetype
                                    == en_pfcallb || epn->nodetype ==
                                    en_sfcallb)
                                    ep1 = cbll(ep1, epn, tp2);
                                if (ept)
                                {
                                    ept->v.p[1] = ep1;
                                    ep1 = ept;
                                }
                                else
                                {
                                    ep1 = makenode(en_moveblock, ep1, ep2);
                                    ep1->size = tp1->size;
                                }
                            }
                        }
                        else
                        {
                            if (!checktypeassign(tp1, tp2))
                            {
                                if (prm_cplusplus && st == assign && (sp1 =
                                    search(cpp_funcname_tab[CI_CONSTRUCTOR],
                                    &tp1->lst)))
                                {
                                    ENODE *epm = dummyvar(tp1->size, tp1, 0);
                                    ENODE *epq;
                                    TYP tp3;
                                    SYM *sm;
                                    tp3.lst.head =  - 1;
                                    tp3.lst.tail =  - 1;
                                    tp3.type = bt_func;
                                    epq = do_destructor(epm->v.sp, &tp3, tp1, 0,
                                        1, 0, TRUE);
                                    if (epq)
                                        expr_rundown[expr_updowncount - 1] =
                                            makenode(en_void, epq,
                                            expr_rundown[expr_updowncount - 1]);
                                    sm = makesym(sc_auto);
                                    sm->tp = tp2;
                                    tp3.lst.head = tp3.lst.tail = sm;
                                    ep2 = makenode(en_void, ep2, 0);
                                    epq = do_constructor(epm->v.sp, &tp3, tp1,
                                        ep2, 1, 0, FALSE, TRUE);
                                    if (epq)
                                        if (expr_runup[expr_updowncount - 1])
                                            expr_runup[expr_updowncount - 1] =
                                                makenode(en_void,
                                                expr_runup[expr_updowncount -
                                                1], epq);
                                        else
                                            expr_runup[expr_updowncount - 1] =
                                                epq;
                                    ep1 = makenode(en_moveblock, ep1, epm);
                                    ep1->size = tp1->size;
                                }
                                else
                                    generror(ERR_ILLSTRUCT, 0, 0);
                            }
                            else if (!exactype(tp1, tp2, FALSE))
                            {
                                generror(ERR_ILLSTRUCT, 0, 0);
                                *node = makenode(en_nacon, &undef, 0);
                            }
                            else
                            {
                                ENODE *epn,  *ept = 0;

//                                if (op != en_asadd && op != en_assub)
//                                    checknp(tp1, tp2, ep1, ep2);
                                epn = ep2;
                                if (epn->nodetype == en_thiscall)
                                {
                                    epn = epn->v.p[1];
                                    ept = ep2;
                                }
                                if (epn->nodetype == en_fcallb || epn->nodetype
                                    == en_pfcallb || epn->nodetype ==
                                    en_sfcallb)
                                {
                                    ep1 = cbll(ep1, epn, tp2);
                                    if (ept)
                                    {
                                        ept->v.p[1] = ep1;
                                        ep1 = ept;
                                    }
                                }
                                else
                                {
                                    checkstructconst(tp1);
                                    ep1 = makenode(en_moveblock, ep1, ep2);
                                    ep1->size = tp1->size;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (!lvalue(ep1) && tp1->type != bt_ref)
                            if (tp1->cflags &DF_CONST)
                                generror(ERR_MODCONS, 0, 0);
                            else
                                generror(ERR_LVALUE, 0, 0);
//                        if (op != en_asadd && op != en_assub)
//                            checknp(tp1, tp2, ep1, ep2);
                        if (prm_cplusplus && st == assign)
                        {
                            if (tp1->type == bt_pointer && tp2->type ==
                                bt_pointer)
                                if (isstructured(tp1->btp) && isstructured(tp2
                                    ->btp) && !exactype(tp1, tp2, FALSE))
                                    cppcast(tp1, tp2, &ep2, FALSE);
                                else if (tp2->btp->type == bt_void && tp1->btp
                                    ->type != bt_void)
                                    genmismatcherror(tp2, tp1);
                                else
                                    tp1 = asnfit(&ep1, tp1, &ep2, tp2, TRUE);
                                else
                                    tp1 = asnfit(&ep1, tp1, &ep2, tp2, TRUE);
                        }
                        else
						{
		                    tp1 = asnfit(&ep1, tp1, &ep2, tp2, TRUE);
						}
                        if (tp2->type == bt_defunc && tp1->type == bt_pointer
                            && tp1->btp->type == bt_func)
                        {
                            SYM *sp2 = funcovermatch(tp2->sp, tp1->btp, 0, TRUE,
                                FALSE);
                            if (sp2)
                                ep2 = makenode(en_napccon, (void*)sp2, 0);
                        }
                        if (tp1->type == bt_bool)
                        {
                            ep2 = makenode(en_cbool, ep2, 0);
                            typedconsts(ep2);
                        }
                        ep1 = makenode(op, ep1, ep2);
                    }
                }
                  															if (op == en_assign) {
                	  															asncombine(&ep1);
                																	op = ep1->nodetype;
                																}
                if (op == en_asmod || op == en_asumod || op == en_aslsh || op 
                    == en_asrsh || op == en_asalsh || op == en_asarsh || op ==
                    en_asand || op == en_asor || op == en_asxor)
                    floatcheck(ep1);
                break;
            case asplus:
                op = en_asadd;

                ascomm3: getsym();
                tp2 = asnop(&ep2, asntyp);
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                if (tp1->type == bt_pointer || tp1->type == bt_farpointer ||
                    tp1->type == bt_segpointer)
                {
                    int ptrmode = pointermode(tp1,tp2);
                    if (ptrmode == 3)
                        if (st == asplus)
                            generror(ERR_ILLPTRADD,0,0);
                        else
                            generror(ERR_INVPTRSUB,0,0);

                    if (tp1->btp->size == 0)
                        generror(ERR_ZEROPTR, 0, 0);
                    if (tp1->btp->val_flag &VARARRAY)
                    {
                        ep3 = makenode(tp1->sp->storage_class == sc_auto ?
                            en_autocon : en_nacon, (void*)tp1->sp, 0);
                        ep3 = makenode(en_add, ep3, makeintnode(en_icon,
                            stdaddrsize + stdintsize));
                        ep3 = makenode(en_a_ref, ep3, 0);
                    }
                    else
                        ep3 = makeintnode(en_icon, tp1->btp->size);
                    ep2 = makenode(en_pmul, ep2, ep3);
                    tp2 = tp1;
                }
                goto ascomm2;
            case asminus:
                op = en_assub;
                goto ascomm3;
            case astimes:
                if (tp1->type == bt_unsigned || tp1->type == bt_unsignedchar ||
                    tp1->type == bt_unsignedshort || tp1->type ==
                    bt_unsignedlong || tp1->type == bt_unsignedlonglong)
                    op = en_asumul;
                else
                    op = en_asmul;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            case asdivide:
                if (tp1->type == bt_unsigned || tp1->type == bt_unsignedchar ||
                    tp1->type == bt_unsignedshort || tp1->type ==
                    bt_unsignedlong || tp1->type == bt_unsignedlonglong)
                    op = en_asudiv;
                else
                    op = en_asdiv;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            case asmodop:
                if (tp1->type == bt_unsigned || tp1->type == bt_unsignedchar ||
                    tp1->type == bt_unsignedshort || tp1->type ==
                    bt_unsignedlong || tp1->type == bt_unsignedlonglong)
                    op = en_asumod;
                else
                    op = en_asmod;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            case aslshift:
                if (tp1->type == bt_unsigned || tp1->type == bt_unsignedchar ||
                    tp1->type == bt_unsignedshort || tp1->type ==
                    bt_unsignedlonglong || tp1->type == bt_unsignedlong)
                    op = en_aslsh;
                else
                    op = en_asalsh;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            case asrshift:
                if (tp1->type == bt_unsigned || tp1->type == bt_unsignedchar ||
                    tp1->type == bt_unsignedshort || tp1->type ==
                    bt_unsignedlonglong || tp1->type == bt_unsignedlong)
                    op = en_asrsh;
                else
                    op = en_asarsh;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            case asand:
                op = en_asand;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            case asor:
                op = en_asor;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            case asxor:
                op = en_asxor;
				if (!exactype(tp1, tp2, TRUE))
					promote_type(tp1, & ep2);
                goto ascomm;
            default:
                goto asexit;
        }
    }
    asexit:  *node = ep1;
    asntyp = oldastyp;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *exprnc(ENODE **node)
/*
 *      evaluate an expression where the comma operator is not legal.
 * e.g. a function argument
 */
{
    TYP *tp;
    int notinternal = !expr_updowncount;
    if (notinternal)
    {
        expr_runup[expr_updowncount] = 0;
        expr_rundown[expr_updowncount++] = 0;
    }
    tp = asnop(node, 0);
    if (tp == 0)
        *node = 0;
    if (tp && tp->type == bt_void)
        goodcode |= GF_ASSIGN;
    if (notinternal)
    {
        expr_updowncount--;
        if (expr_runup[expr_updowncount] || expr_rundown[expr_updowncount])
        {
            *node = makenode(en_dvoid,  *node, expr_rundown[expr_updowncount]);
            (*node)->cflags |= DF_ACTUALEXPR;
        }
        if (expr_runup[expr_updowncount])
            *node = makenode(en_void, expr_runup[expr_updowncount],  *node);
    }
    if (*node)
        *node = repcallb(*node);
    return tp;
}

//-------------------------------------------------------------------------

TYP *commaop(ENODE **node)
/*
 *      evaluate the comma operator. comma operators are kept as
 *      void nodes.
 */
{
    TYP *tp1;
    ENODE *ep1,  *ep2;
    int ocode;
    tp1 = asnop(&ep1, 0);
    if (tp1 == 0)
    {
        *node = ep1;
        return 0;
    }
    if (lastst == comma)
    {
        ocode = goodcode | ~GF_ASSIGN;
        goodcode &= ~GF_ASSIGN;
        getsym();
        tp1 = commaop(&ep2);
        goodcode &= ocode;
        if (tp1 == 0)
        {
            generror(ERR_IDEXPECT, 0, 0);
            goto coexit;
        }
        ep1 = makenode(en_void, ep1, ep2);
    }
    coexit:  *node = ep1;
    return tp1;
}

//-------------------------------------------------------------------------

TYP *expression(ENODE **node, int tocheck)
/*
 *      evaluate an expression where all operators are legal.
 */
{
    TYP *tp;
    expr_runup[expr_updowncount] = 0;
    expr_rundown[expr_updowncount++] = 0;
    tp = commaop(node);
    if (tp == 0)
        *node = 0;
    if (tp && tp->type == bt_void)
        goodcode |= GF_ASSIGN;
    expr_updowncount--;
    if (expr_runup[expr_updowncount] || expr_rundown[expr_updowncount])
    {
        *node = makenode(en_dvoid,  *node, expr_rundown[expr_updowncount]);
        (*node)->cflags |= DF_ACTUALEXPR;
    }
    if (expr_runup[expr_updowncount])
        *node = makenode(en_void, expr_runup[expr_updowncount],  *node);
    if (*node)
        *node = repcallb(*node);
    return tp;
}

