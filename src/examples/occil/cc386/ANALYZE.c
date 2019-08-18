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
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "diag.h"
#ifdef i386
#include "gen386.h"
#endif

extern int setjmp_used;
extern SYM *currentfunc;
extern int prm_xcept;
extern TRYBLOCK *try_block_list;
extern int cf_freeaddress, cf_freedata, cf_freefloat;
extern long framedepth, stackdepth;
extern LIST *varlisthead;
extern long lc_maxauto;
extern int stackadd, stackmod;
extern int anyusedfloat;
extern int stdaddrsize;
extern int prm_stackalign;
extern int prm_dsnotss;

int funcfloat;
int floatregs = 1, dataregs = 1, addrregs = 1;
CSE *olist; /* list of optimizable expressions */
/*
 *      this module will step through the parse tree and find all
 *      optimizable expressions. at present these expressions are
 *      limited to expressions that are valid throughout the scope
 *      of the function. the list of optimizable expressions is:
 *
 *              constants
 *              global and static addresses
 *              auto addresses
 *              contents of auto addresses.
 *
 *      contents of auto addresses are valid only if the address is
 *      never referred to without dereferencing.
 *
 *      scan will build a list of optimizable expressions which
 *      opt1 will replace during the second optimization pass.
 */


int equalnode(ENODE *node1, ENODE *node2)
/*
 *      equalnode will return 1 if the expressions pointed to by
 *      node1 and node2 are equivalent.
 */
{
    if (node1 == 0 || node2 == 0)
        return 0;
    if (node1->nodetype != node2->nodetype)
    {
        if (lvalue(node1) && lvalue(node2))
        {
            return equalnode(node1->v.p[0], node2->v.p[0]);
        }
        return 0;
    }
    if (natural_size(node1) != natural_size(node2))
        return 0;
    switch (node1->nodetype)
    {
        default:
            return (lvalue(node1) && equalnode(node1->v.p[0], node2->v.p[0]));
            return 1;
        case en_autoreg:
        case en_autocon:
        case en_nacon:
        case en_napccon:
            return node1->v.sp->mainsym == node2->v.sp->mainsym;
        case en_labcon:
        case en_nalabcon:
        case en_absacon:
        case en_icon:
        case en_lcon:
        case en_lucon:
        case en_iucon:
        case en_boolcon:
        case en_ccon:
        case en_cucon:
        case en_llcon:
        case en_llucon:
            return node1->v.i == node2->v.i;
        case en_rcon:
        case en_fcon:
        case en_lrcon:
        case en_rimaginarycon:
        case en_fimaginarycon:
        case en_lrimaginarycon:
            return FPFEQ(&node1->v.f, &node2->v.f);
        case en_rcomplexcon:
        case en_fcomplexcon:
        case en_lrcomplexcon:
            return FPFEQ(&node1->v.c.r, &node2->v.c.r) 
			&& FPFEQ(&node1->v.c.i, &node2->v.c.i);
    }
}

//-------------------------------------------------------------------------

CSE *searchnode(ENODE *node)
/*
 *      searchnode will search the common expression table for an entry
 *      that matches the node passed and return a pointer to it.
 */
{
    CSE *csp;
    csp = olist;
    while (csp != 0)
    {
        if (equalnode(node, csp->exp))
            return csp;
        csp = csp->next;
    }
    return 0;
}

//-------------------------------------------------------------------------

CSE *enternode(ENODE *node, int duse, int size)
/*
 *      enternode will enter a reference to an expression node into the
 *      common expression table. duse is a flag indicating whether or not
 *      this reference will be dereferenced.
 */
{
    CSE *csp;
    if (size == 0)
        size = natural_size(node);
    if ((csp = searchnode(node)) == 0)
    {
         /* add to tree */
        csp = xalloc(sizeof(CSE));
        csp->next = olist;
        csp->uses = 1;
        csp->reg =  - 1;
        csp->duses = (duse != 0);
        csp->exp = copynode(node);
        csp->voidf = 0;
        csp->size = size;
        olist = csp;
        return csp;
    }
    else
        if (chksize(csp->size, size))
            csp->size = size;
    ++(csp->uses);
    if (duse)
        ++(csp->duses);
    return csp;
}

//-------------------------------------------------------------------------

CSE *voidauto(ENODE *node)
/*
 *      voidauto will void an auto dereference node which points to
 *      the same auto constant as node.
 */
{
    CSE *csp;
    csp = olist;
    while (csp != 0)
    {
        if (lvalue(csp->exp) && equalnode(node, csp->exp->v.p[0]))
        {
            if (csp->voidf)
                return 0;
            csp->voidf = 1;
            return csp;
        }
        csp = csp->next;
    }
    return 0;
}

//-------------------------------------------------------------------------

CSE *voidlref(ENODE *node)
/*
 *      voidlref will void an lref
 */
{
    CSE *csp;
    csp = olist;
    while (csp != 0)
    {
        if (equalnode(node, csp->exp))
        {
            if (csp->voidf)
                return 0;
            csp->voidf = 1;
            return csp;
        }
        csp = csp->next;
    }
    return 0;
}

//-------------------------------------------------------------------------

void voidall(void)
/*
 * Go through the CSP list voiding any use of a variable that
 * has its address taken or is volatile
 */
{
    CSE *csp;
    csp = olist;
    while (csp)
    {
        if (lvalue(csp->exp) && (csp->exp->v.p[0]->nodetype == en_autocon ||
            csp->exp->v.p[0]->nodetype == en_autoreg))
        {
            int tp;
            if (tp = csp->exp->v.p[0]->v.sp->tp->type == bt_union || tp ==
                bt_struct || tp == bt_class)
                csp->voidf = 1;
        }
        if (csp->exp->nodetype == en_autocon || csp->exp->nodetype ==
            en_autoreg)
        {
            CSE *two = voidauto(csp->exp);
            if (two)
            {
                csp->uses += two->duses;
            }
        }
        csp = csp->next;
    }
}

//-------------------------------------------------------------------------

void scan_for_inline(ENODE *node)
{
    SYM *sp;
    ENODE *node2 = node->v.p[1]->v.p[0];
    if (node2->nodetype == en_nacon || node2->nodetype == en_napccon)
    {
        sp = node2->v.sp;
        if (sp && (sp->value.classdata.cppflags &PF_INLINE))
            scan(sp->value.classdata.inlinefunc->stmt);
    }
}

//-------------------------------------------------------------------------

void scanexpr(ENODE *node, int duse, int size)
/*
 *      scanexpr will scan the expression pointed to by node for optimizable
 *      subexpressions. when an optimizable expression is found it is entered
 *      into the tree. if a reference to an autocon node is scanned the
 *      corresponding auto dereferenced node will be voided. duse should be
 *      set if the expression will be dereferenced.
 *
 */
{
    CSE *csp,  *csp1;
    int sz;
    if (node == 0)
        return ;
    switch (node->nodetype)
    {
        case en_tempref:
        case en_labcon:
        case en_nalabcon:
        case en_regref:
            break;
        case en_structret:
            break;
        case en_conslabel:
        case en_destlabel:
        case en_movebyref:
            scanexpr(node->v.p[0], duse, size);
            break;
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_fcomplexcon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_fimaginarycon:
            funcfloat++;
            anyusedfloat++;
        case en_icon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_boolcon:
        case en_ccon:
        case en_cucon:
        case en_llcon:
        case en_llucon:
            /* not putting constants in registers */
            break;
        case en_autocon:
            if (node->v.sp->iscatchvar)
                break;
            csp = enternode(node, duse, 4);
#ifdef i386
			csp->seg = prm_dsnotss ? SS : 0;
#endif
			break;
        case en_autoreg:
        case en_nacon:
        case en_absacon:
            enternode(node, duse, 4);
        case en_napccon:
            break;
        case en_bits:
            scanexpr(node->v.p[0], duse, natural_size(node));
            break;
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
            funcfloat++;
            scanexpr(node->v.p[0], 1, natural_size(node->v.p[0]));
            break;
        case en_fp_ref:
            scanexpr(node->v.p[0], 1, natural_size(node->v.p[0]));
            break;
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
            funcfloat++;
        case en_b_ref:
        case en_bool_ref:
        case en_w_ref:
        case en_ub_ref:
        case en_uw_ref:
        case en_ul_ref:
        case en_l_ref:
			case en_i_ref:
			case en_ui_ref:
			case en_a_ref: case en_ua_ref:
            if (node->v.p[0]->nodetype == en_autocon && !node->v.p[0]->v.sp
                ->iscatchvar || node->v.p[0]->nodetype == en_autoreg)
                enternode(node, duse, natural_size(node));
            else
                scanexpr(node->v.p[0], 1, natural_size(node->v.p[0]));
            break;
        case en_ll_ref:
        case en_ull_ref:
        case en_cl_reg:
            scanexpr(node->v.p[0], 1, natural_size(node->v.p[0]));
            break;
        case en_uminus:
        case en_asuminus:
        case en_ascompl:
        case en_compl:
        case en_not:
            scanexpr(node->v.p[0], duse, natural_size(node->v.p[0]));
            break;
        case en_cf:
        case en_cd:
        case en_cld:
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_cfi:
        case en_cri:
        case en_clri:
            funcfloat++;
        case en_cb:
        case en_cub:
        case en_cbool:
        case en_cw:
        case en_cuw:
        case en_cfp:
        case en_csp:
        case en_cl:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cp:
        case en_cll:
        case en_cull:
            scanexpr(node->v.p[0], duse, natural_size(node));
            break;
        case en_asadd:
        case en_assub:
            size = natural_size(node->v.p[0]);
            scanexpr(node->v.p[0], duse, 0);
            scanexpr(node->v.p[1], duse, size);
            break;
        case en_ainc:
        case en_adec:
        case en_add:
        case en_sub:
        case en_addstruc:
        case en_addcast: 
			scanexpr(node->v.p[0], duse, natural_size(node->v.p[0]))
                ;
            scanexpr(node->v.p[1], duse, natural_size(node->v.p[1]));
            break;
        case en_asalsh:
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
        en_lassign: case en_asor:
        case en_asxor:
            size = natural_size(node->v.p[0]);
            scanexpr(node->v.p[0], 0, 0);
            scanexpr(node->v.p[1], 0, size);
            break;
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
            scan_for_inline(node);
        case en_void:
        case en_voidnz:
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
		case en_array:
            scanexpr(node->v.p[0], 0, natural_size(node->v.p[0]));
            scanexpr(node->v.p[1], 0, natural_size(node->v.p[1]));
            break;
        case en_trapcall:
        case en_substack:
        case en_clearblock:
		case en_loadstack:
		case en_savestack:
            scanexpr(node->v.p[0], 0, natural_size(node->v.p[0]));
            break;
        case en_fcall:
        case en_fcallb:
        case en_pfcall:
        case en_pfcallb:
        case en_sfcall:
        case en_sfcallb:
            /* used to put funcs in regs, but not any more */
            scan_for_inline(node);
        case en_thiscall:
            scanexpr(node->v.p[0], 0, 0);
            scanexpr(node->v.p[1], 0, 0);
            break;
        default:
            DIAG("scan_expression: invalid node type");
            break;
    }
}

//-------------------------------------------------------------------------

void scan(SNODE *block)
/*
 *      scan will gather all optimizable expressions into the expression
 *      list for a block of statements.
 */
{
    while (block != 0)
    {
        switch (block->stype)
        {
            case st_tryblock:
                break;
            case st_throw:
                if (block->exp)
                {
                    opt4(&block->exp, FALSE);
                    scanexpr(block->exp, 0, 0);
                }
                break;
            case st_return:
            case st_expr:
                opt4(&block->exp, FALSE);
                scanexpr(block->exp, 0, 0);
                break;
            case st_while:
            case st_do:
                opt4(&block->exp, FALSE);
                scanexpr(block->exp, 0, 0);
                scan(block->s1);
                break;
            case st_for:
                opt4(&block->label, FALSE);
                scanexpr(block->label, 0, 0);
                opt4(&block->exp, FALSE);
                scanexpr(block->exp, 0, 0);
                scan(block->s1);
                opt4(&block->s2, FALSE);
                scanexpr(block->s2, 0, 0);
                break;
            case st_if:
                opt4(&block->exp, FALSE);
                scanexpr(block->exp, 0, 0);
                scan(block->s1);
                scan(block->s2);
                break;
            case st_switch:
                opt4(&block->exp, FALSE);
                scanexpr(block->exp, 0, 0);
                scan(block->s1);
                break;
            case st_case:
                scan(block->s1);
                break;
            case st_block:
                scan(block->exp);
                break;
            case st_asm:
                if (block->exp)
                    asm_scan(block->exp);
                break;
        }
        block = block->next;
    }
}

//-------------------------------------------------------------------------

void exchange(CSE **c1)
/*
 *      exchange will exchange the order of two expression entries
 *      following c1 in the linked list.
 */
{
    CSE *csp1,  *csp2;
    csp1 =  *c1;
    csp2 = csp1->next;
    csp1->next = csp2->next;
    csp2->next = csp1;
    *c1 = csp2;
}

//-------------------------------------------------------------------------

int desire(CSE *csp)
/*
 *      returns the desirability of optimization for a subexpression.
 */
{
    if (csp->voidf || (isintconst(csp->exp->nodetype) && csp->exp->v.i < 16 &&
        csp->exp->v.i >= 0))
        return 0;
    if (lvalue(csp->exp))
        return 2 *csp->uses;
    return csp->uses;
}

//-------------------------------------------------------------------------

int bsort(CSE **list)
/*
 *      bsort implements a bubble sort on the expression list.
 */
{
    CSE *csp1,  *csp2;
    csp1 =  *list;
    if (csp1 == 0 || csp1->next == 0)
        return 0;
    bsort(&(csp1->next));
    csp2 = csp1->next;
    if (desire(csp1) < desire(csp2))
    {
        exchange(list);
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------

void repexpr(ENODE *node, int size)
/*
 *      repexpr will replace all allocated references within an expression
 *      with tempref nodes.
 */
{
    CSE *csp;
    if (node == 0)
        return ;
    if (size == 0)
        size = natural_size(node);
    switch (node->nodetype)
    {
        case en_tempref:
        case en_labcon:
        case en_nalabcon:
        case en_regref:
            break;
        case en_structret:
            break;
        case en_conslabel:
        case en_destlabel:
        case en_movebyref:
            repexpr(node->v.p[0], size);
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
            if ((csp = searchnode(node)) != 0)
            if (csp->reg > 0)
            {
                node->nodetype = en_tempref;
                #ifdef i386
                    node->v.i = csp->reg | (size << 16) | (csp->seg << 8);
                #else 
                    node->v.i = csp->reg | (size << 16);
                #endif 
            }
            break;
        case en_fp_ref:
            repexpr(node->v.p[0], 0);
            break;
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
            repexpr(node->v.p[0], 0);
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
            if ((csp = searchnode(node)) != 0)
            {
                if (csp->reg > 0)
                {
                    node->nodetype = en_tempref;
                    #ifdef i386
	                    node->v.i = csp->reg | (size << 16) ;
                    #else 
                        node->v.i = csp->reg | (size << 16);
                    #endif 
                }
                else
                    repexpr(node->v.p[0], 0);
            }
            else
                repexpr(node->v.p[0], 0);
            break;
        case en_ll_ref:
        case en_ull_ref:
        case en_cl_reg:
            repexpr(node->v.p[0], 0);
            break;
        case en_uminus:
        case en_bits:
        case en_asuminus:
        case en_ascompl:
        case en_not:
        case en_compl:
            repexpr(node->v.p[0], 0);
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
            repexpr(node->v.p[0], natural_size(node->v.p[0]));
            break;
        case en_pfcall:
        case en_pfcallb:
        case en_sfcall:
        case en_sfcallb:
        case en_fcallb:
        case en_fcall:
            {
                SYM *sp;
                ENODE *node2 = node->v.p[1]->v.p[0]->v.p[0];
                if (node2->nodetype == en_nacon || node2->nodetype ==
                    en_napccon)
                {
                    sp = node2->v.sp;
                    if (sp && (sp->value.classdata.cppflags &PF_INLINE))
                        repcse(sp->value.classdata.inlinefunc->stmt);
                }
            }
        case en_stackblock:
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
        case en_assign:
        case en_intcall:
        case en_refassign:
        case en_lassign: 
		case en_ainc:
        case en_adec:
        case en_add:
        case en_sub:
        case en_addstruc:
        case en_addcast: 
		case en_umul:
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
        case en_thiscall:
        case en_repcons:
		case en_array:
            repexpr(node->v.p[1], 0);
        case en_trapcall:
        case en_substack:
        case en_clearblock:
		case en_loadstack:
		case en_savestack:
            repexpr(node->v.p[0], 0);
            break;
        default:
            DIAG("repexpr: invalid node type");
            break;
    }
}

//-------------------------------------------------------------------------

void repcse(SNODE *block)
/*
 *      repcse will scan through a block of statements replacing the
 *      optimized expressions with their temporary references.
 */
{
    while (block != 0)
    {
        switch (block->stype)
        {
            case st_tryblock:
                break;
            case st_throw:
                if (block->exp)
                    repexpr(block->exp, 0);
                break;
            case st_return:
            case st_expr:
            case st_while:
            case st_do:
                repexpr(block->exp, 0);
                repcse(block->s1);
                break;
            case st_for:
                repexpr(block->label, 0);
                repexpr(block->exp, 0);
                repcse(block->s1);
                repexpr(block->s2, 0);
                break;
            case st_if:
                repexpr(block->exp, 0);
                repcse(block->s1);
                repcse(block->s2);
                break;
            case st_switch:
                repexpr(block->exp, 0);
                repcse(block->s1);
                break;
            case st_case:
                repcse(block->s1);
                break;
            case st_block:
                repcse(block->exp);
                break;
            case st_asm:
                if (block->exp)
                    asm_repcse(block->exp);
                break;
        }
        block = block->next;
    }
}

//-------------------------------------------------------------------------

void repvoidassign(ENODE *node, int size)
/*
 *      repvoidassign will replace all allocated references within an expression
 *      with tempref nodes.
 */
{
    CSE *csp;
    if (node == 0)
        return ;
    if (size == 0)
        size = natural_size(node);
    switch (node->nodetype)
    {
        case en_structret:
            break;
        case en_conslabel:
        case en_destlabel:
        case en_movebyref:
            repvoidassign(node->v.p[0], size);
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
            break;
        case en_fp_ref:
            repvoidassign(node->v.p[0], 0);
            break;
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
            repvoidassign(node->v.p[0], 0);
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
            repvoidassign(node->v.p[0], 0);
            break;
        case en_ll_ref:
        case en_ull_ref:
        case en_cl_reg:
            repvoidassign(node->v.p[0], 0);
            break;
        case en_uminus:
        case en_bits:
        case en_asuminus:
        case en_ascompl:
        case en_not:
        case en_compl:
            repvoidassign(node->v.p[0], 0);
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
            repvoidassign(node->v.p[0], natural_size(node->v.p[0]));
            break;
        case en_pfcall:
        case en_pfcallb:
        case en_sfcall:
        case en_sfcallb:
        case en_fcallb:
        case en_fcall:
            {
                SYM *sp;
                ENODE *node2 = node->v.p[1]->v.p[0]->v.p[0];
                if (node2->nodetype == en_nacon || node2->nodetype ==
                    en_napccon)
                {
                    sp = node2->v.sp;
                    if (sp && (sp->value.classdata.cppflags &PF_INLINE))
                        voidAssign(sp->value.classdata.inlinefunc->stmt, 0);
                }
            }
        case en_stackblock:
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
        case en_intcall:
        case en_ainc:
        case en_adec:
        case en_add:
        case en_sub:
        case en_addstruc:
        case en_addcast: 
		case en_umul:
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
        case en_pdiv:
        case en_pmul:
		case en_arrayindex:
        case en_moveblock:
        case en_thiscall:
        case en_repcons:
		case en_array:
            repvoidassign(node->v.p[1], 0);
        case en_trapcall:
        case en_substack:
        case en_clearblock:
		case en_loadstack:
		case en_savestack:
            repvoidassign(node->v.p[0], 0);
            break;
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
        case en_asrsh:
        case en_assign:
        case en_refassign:
            en_lassign: voidlref(node->v.p[0]);
            repvoidassign(node->v.p[1], 0);
            repvoidassign(node->v.p[0], 0);
            break;
    }
}

//-------------------------------------------------------------------------

void voidAssign(SNODE *block, int state)
/*
 *      voidAssign will scan through a block of statements replacing the
 *      optimized expressions with their temporary references.
 */
{
    int istate = state;
    while (block != 0)
    {
        switch (block->stype)
        {
            case st_tryblock:
                if (!state)
                {
                    if ((int)block->label == 0)
                    // begin try
                        istate++;
                    else if ((int)block->label == 2)
                    // end try
                        istate--;
                }
                break;
            case st_throw:
                if (istate && block->exp)
                    repvoidassign(block->exp, 0);
                break;
            case st_return:
            case st_expr:
            case st_while:
            case st_do:
                if (istate)
                    repvoidassign(block->exp, 0);
                voidAssign(block->s1, istate);
                break;
            case st_for:
                if (istate)
                {
                    repvoidassign(block->label, 0);
                    repvoidassign(block->exp, 0);
                    repvoidassign(block->s2, 0);
                }
                voidAssign(block->s1, istate);
                break;
            case st_if:
                if (istate)
                    repvoidassign(block->exp, 0);
                voidAssign(block->s1, istate);
                voidAssign(block->s2, istate);
                break;
            case st_switch:
                if (istate)
                    repvoidassign(block->exp, 0);
                voidAssign(block->s1, istate);
                break;
            case st_case:
                voidAssign(block->s1, istate);
                break;
            case st_block:
                voidAssign(block->exp, istate);
                break;
            case st_asm:
                //																if (block->exp)
                //																	asm_voidAssign(block->exp);
                break;
        }
        block = block->next;
    }
}

//-------------------------------------------------------------------------

void allocstack(void)
/*
 * This allocates space for local variables
 * we do this AFTER the register optimization so that we can
 * avoid allocating space on the stack twice
 */
{
    int lvl =  - 1; 
        // base assignments are at level 0, function body starts at level 1
    int again = TRUE;
    int oldauto, max;
    lc_maxauto = max = 0;
    while (again)
    {
        LIST *t = varlisthead;
        int align;
        lvl++;
        oldauto = max;
        again = FALSE;
        while (t)
        {
            SYM *sp = t->data;
            while (sp && (sp->inreg || sp->funcparm || sp->storage_class ==
                sc_static || sp->storage_class == sc_label || sp->storage_class
                == sc_ulabel || sp->storage_class == sc_const))
			{
                sp = sp->next;
			}
            if (!sp)
            {
                t = t->link;
                continue;
            }
            if (sp->value.i >= lvl)
                again = TRUE;
            if (sp->value.i == lvl)
            {
                lc_maxauto = oldauto;
                while (sp)
                {
                    if (!sp->inreg && !sp->funcparm && sp->storage_class !=
                        sc_static)
                    {
                        int val;
						TYP *tp = sp->tp;
						if (tp->type == bt_pointer && (tp->btp->val_flag & VARARRAY))
							tp = tp->btp;
                        if (!prm_stackalign && tp->size >= stdaddrsize)
                            align = stdaddrsize;
                        else
                            align = getalign(sc_auto, tp);
                        lc_maxauto += tp->size;
                        val = lc_maxauto % align;
                        if (val != 0)
                            lc_maxauto += align - val;
                        sp->value.i =  - lc_maxauto;
                    }
                    sp = sp->next;
                }
            }
            if (lc_maxauto > max)
                max = lc_maxauto;
            t = t->link;
        }
    }
    lc_maxauto = max;
    lc_maxauto += stackadd;
    lc_maxauto &= stackmod;
}

//-------------------------------------------------------------------------

void opt1(SNODE *block)
/*
 *      opt1 is the externally callable optimization routine. it will
 *      collect and allocate common subexpressions and substitute the
 *      tempref for all occurrances of the expression within the block.
 *
 */
{
    int datareg = cf_freedata;
    int addreg = 16+cf_freeaddress;
    int floatreg = 32+cf_freefloat;
    olist = 0;
	if (!setjmp_used)
	{
		scan(block);
    	voidall(); /* Disallow regs whose address is taken  */
    	voidfloat(block); /* disallow optimizing things which are assigned values
        derived from floats */
	}
    if ((try_block_list || currentfunc->value.classdata.throwlist &&
        currentfunc->value.classdata.throwlist->data) && prm_xcept)
        voidAssign(block, FALSE);
     /* void having assignment targets in registers for exception handling */

    reserveregs(&datareg, &addreg, &floatreg); /* Allocate register vars */
    allocate(datareg, addreg, floatreg, block); /* allocate registers  for opt*/
    repcse(block); /* replace allocated expressions */
    loadregs(); /* Initialize allocated regs */
}
