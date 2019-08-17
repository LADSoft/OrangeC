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
 * Register analysis
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "gen386.h"
#include "diag.h"
#include "rtti.h"

/* pc-relative expressions not optimized */
extern long nextlabel;
extern AMODE push[], pop[];
extern OCODE *peep_tail;
extern SYM *currentfunc;
extern int prm_stackcheck, prm_farkeyword;
extern int floatregs, dataregs, addrregs;
extern int cf_maxfloat, cf_maxaddress, cf_maxdata, cf_freeaddress, cf_freedata,
    cf_freefloat;
extern CSE *olist; /* list of optimizable expressions */
extern int funcfloat;
extern SYM *declclass;
extern int conscount;
extern TRYBLOCK *try_block_list;
extern int prm_cplusplus, prm_xcept;
extern int asmMask, asmRMask;
extern int stackframeoff;
extern int prm_debug;
long lc_maxauto;
int fsave_mask, save_mask;
OCODE *frame_ins;
int floatstack_mode;
extern int startlab;
extern int prm_smartframes;
extern int prm_xcept, conscount, xceptoffs;

void reserveregs(int *datareg, int *addreg, int *floatreg)
/*
 * Reserve regs goes through and reserves a register for variables with
 * the REGISTER keyword.  Note that it currently does register allocation
 * backwards...  NOT the standard I suspect.
 */
{
    CSE *csp = olist;

    while (csp)
    {
        switch (csp->exp->nodetype)
        {
            case en_fcomplexref:
            case en_rcomplexref:
            case en_lrcomplexref:
            case en_fimaginaryref:
            case en_rimaginaryref:
            case en_lrimaginaryref:
            case en_floatref:
            case en_doubleref:
            case en_longdoubleref:
            case en_ll_ref:
            case en_ull_ref:
            case en_fp_ref:
                break;
            case en_bool_ref:
            case en_b_ref:
            case en_w_ref:
            case en_ub_ref:
            case en_uw_ref:
            case en_l_ref:
            case en_ul_ref:
			case en_i_ref:
			case en_ui_ref:
			case en_a_ref: case en_ua_ref:
                if (csp->exp->v.p[0]->nodetype != en_autoreg)
                    break;
                if ((csp->duses <= csp->uses / 4) && (*datareg < cf_maxdata) &&
                    dataregs)
                    csp->reg = (*datareg)++;
                else if (!(csp->size ==  - 1 || csp->size == 1 || csp->size ==
                    5) && (*addreg < cf_maxaddress) && addrregs)
                {
                    csp->reg = (*addreg)++;
                }
                else if ((*datareg < cf_maxdata) && dataregs)
                    csp->reg = (*datareg)++;
                if (csp->reg !=  - 1)
                {
                    int size = (csp->size < 0 ?  - csp->size: csp->size);
                    ((SYM*)csp->exp->v.p[0]->v.p[0])->mainsym->inreg = TRUE;
                    ((SYM*)csp->exp->v.p[0]->v.p[0])->mainsym->value.i =  - csp
                        ->reg - size * 256;
                }
                break;
        }
        csp = csp->next;
    }
}

/*  Assums the dseg is the next higher seg value from the cseg	*/
void loadds(void)
{
    gen_code(op_push, makesegreg(DS), 0);
    gen_code(op_push, makesegreg(CS), 0);
    gen_codes(op_add, 2, make_stack(0), make_immed(8));
    gen_code(op_pop, makesegreg(DS), 0);
}

//-------------------------------------------------------------------------

void allocate(int datareg, int addreg, int floatreg, SNODE *block)
/*
 *      allocate will allocate registers for the expressions that have
 *      a high enough desirability.  It also puts the function
 * header, consisting of saved registers and stack decrments for local
 * variables
 */
{
    CSE *csp;
    ENODE *exptr;
    unsigned mask, rmask, i, fmask, frmask, size;
    AMODE *ap,  *ap2;
    mask = asmMask;
    rmask = asmRMask;
    fmask = frmask = 0;
    for (i = cf_freedata; i < datareg; i++)
    {
        rmask = rmask | (1 << (15-i));
        mask = mask | (1 << i);
    }
    for (i = cf_freeaddress + 16; i < addreg; i++)
    {
        rmask = rmask | (1 << (23-i));
        mask = mask | (1 << (i - 8));
    }
    while (bsort(&olist))
        ;
     /* sort the expression list */
    csp = olist;
    while (csp != 0)
    {
        if (csp->reg ==  - 1 && !(csp->exp->cflags &DF_VOL) && !csp->voidf)
        {
            if (desire(csp) < 3)
                csp->reg =  - 1;
            else
            {
                if (csp->exp->nodetype == en_rcon || csp->exp->nodetype ==
                    en_fcon || csp->exp->nodetype == en_lrcon || csp->exp
                    ->nodetype == en_floatref || csp->exp->nodetype ==
                    en_doubleref || csp->exp->nodetype == en_longdoubleref ||
                    csp->exp->nodetype == en_fimaginarycon || 
                    csp->exp->nodetype == en_rimaginarycon ||
                    csp->exp->nodetype == en_lrimaginarycon ||
                    csp->exp->nodetype == en_fimaginaryref || 
                    csp->exp->nodetype == en_rimaginaryref ||
                    csp->exp->nodetype == en_lrimaginaryref) {}
                else if ((csp->duses <= csp->uses / 4) && (datareg < cf_maxdata)
                    && dataregs)
                    csp->reg = (datareg)++;
                else if (!(csp->size == 1 || csp->size ==  - 1 || csp->size ==
                    5) && (addreg < cf_maxaddress) && addrregs)
                    csp->reg = (addreg)++;
                else if ((datareg < cf_maxdata) && dataregs)
                    csp->reg = (datareg)++;
                if (csp->reg !=  - 1)
                //                        if (lvalue(csp->exp))
                //                           csp->seg = defseg(csp->exp->v.p[0]) ;
                //                        else
				if (!csp->seg)
				{
                    csp->seg = defseg(csp->exp);
					if (csp->seg)
						csp->reg = -1;
				}
            }

        }
        if (csp->reg !=  - 1)
        {
            if (lvalue(csp->exp) && !((SYM*)csp->exp->v.p[0]->v.p[0])->funcparm)
            {
                ((SYM*)csp->exp->v.p[0]->v.p[0])->mainsym->inreg = TRUE;
                ((SYM*)csp->exp->v.p[0]->v.p[0])->mainsym->value.i =  - csp
                    ->reg - (csp->size < 0 ?  - csp->size: csp->size) *256;
            }
            if (csp->reg < 16)
            {
                rmask = rmask | (1 << (15-csp->reg));
                mask = mask | (1 << csp->reg);
            }
            if (csp->reg < 32)
            {
                rmask = rmask | (1 << (23-csp->reg));
                mask = mask | (1 << (csp->reg - 8));
            }
            else
            {
                frmask = frmask | (1 << (39-csp->reg));
                fmask = fmask | (1 << (csp->reg - 32));
            }
        }
        csp = csp->next;
    }
    allocstack(); /* Allocate stack space for the local vars */
    floatstack_mode = 0; /* no space for floating point temps */
    if (currentfunc->intflag || currentfunc->faultflag)
    {
        mask = 0;
        rmask = 0;
        if (currentfunc->loadds)
            loadds();
        if (prm_farkeyword)
        {
            GenPush(ES + 24, am_dreg, 0);
            GenPush(FS + 24, am_dreg, 0);
            GenPush(GS + 24, am_dreg, 0);
        }
        gen_code(op_pushad, 0, 0);
    }
    if ((conscount || try_block_list || currentfunc->value.classdata.throwlist
        && currentfunc->value.classdata.throwlist->data) && prm_xcept)
    {
        xceptoffs = lc_maxauto += sizeof(XCEPTDATA);
    }
	if (prm_debug)
	{
        rmask = rmask | (1 << (15-EBX));
        mask = mask | (1 << EBX);
        rmask = rmask | (1 << (15-ESI-4));
        mask = mask | (1 << (ESI+4));
        rmask = rmask | (1 << (15-EDI-4));
        mask = mask | (1 << (EDI+4));
	}
    if (prm_cplusplus && prm_xcept || (funcfloat || lc_maxauto || currentfunc
        ->tp->lst.head && currentfunc->tp->lst.head != (SYM*) - 1)
         || (currentfunc->value.classdata.cppflags &PF_MEMBER) && !(currentfunc
             ->value.classdata.cppflags &PF_STATIC)
     || !prm_smartframes || !stackframeoff)
    {
        /* enter is *really* inefficient so we will not use it */
        if (!currentfunc->intflag)
            gen_codes(op_push, 4, makedreg(EBP), 0);
        gen_codes(op_mov, 4, makedreg(EBP), makedreg(ESP));
        if (lc_maxauto)
            gen_code(op_sub, makedreg(ESP), make_immed(lc_maxauto));
        // FIXME ... could auto-alloc an FP value when no frame!
        frame_ins = peep_tail;
    }
    else
        frame_ins = 0;
    if (mask != 0)
        PushRegs(rmask);
    save_mask = mask;
    if (fmask != 0)
        fsave_mask = fmask;
    if (currentfunc->loadds && !currentfunc->intflag)
    {
        loadds();

    }
    if (prm_stackcheck && lc_maxauto)
    {
        AMODE *ap1;
        ap = set_symbol("__stackerror", 1);
        ap1 = set_symbol("__stackbottom", 0);
        ap1->mode = am_direct;
        gen_codes(op_cmp, 4, makedreg(ESP), ap1);
        gen_codes(op_jb, 0, ap, 0);
    }
    AddProfilerData();
    if ((conscount || try_block_list || currentfunc->value.classdata.throwlist
        && currentfunc->value.classdata.throwlist->data) && prm_xcept)
    {
        currentfunc->value.classdata.conslabel = nextlabel++;
        currentfunc->value.classdata.destlabel = nextlabel++;
        gen_codes(op_mov, 4, makedreg(EAX), make_label(nextlabel - 2));
        call_library("__InitExceptBlock");
        gen_label(nextlabel - 1);
    }
}

//-------------------------------------------------------------------------

void loadregs(void)
/*
 * initialize allocated registers
 */
{
    CSE *csp;
    ENODE *exptr;
    unsigned mask, rmask, i, fmask, frmask, size;
    AMODE *ap,  *ap2;
    csp = olist;
    while (csp != 0)
    {
        int sz;
        if (csp->reg !=  - 1)
        {
             /* see if preload needed */
            exptr = csp->exp;
            if (!lvalue(exptr) || ((SYM*)exptr->v.p[0]->v.p[0])->funcparm)
            {
                exptr = csp->exp;
                InitRegs();
                sz = csp->size;
                ap = gen_expr_external(exptr, FALSE, F_MEM | F_DREG | F_IMMED, sz);
                if (csp->reg < 16)
                {
                    if (ap->mode == am_dreg)
                        peep_tail->oper1->preg = csp->reg;
                    else
                    {
                        if (csp->reg > 3 && chksize(BESZ_WORD, sz))
                            DIAG("byte sized non-register in analyze");
                        ap2 = makedreg(csp->reg);
                        ap2->length = BESZ_DWORD;
                        if (ap->mode == am_immed || ap->length == BESZ_DWORD || ap
                            ->length ==  - BESZ_DWORD)
                            gen_codes(op_mov, BESZ_DWORD, ap2, ap);
                        else
                            if (sz < 0)
                                gen_code(op_movsx, ap2, ap);
                            else
                                gen_code(op_movzx, ap2, ap);
                    }
                }
                else
                if (csp->reg < 32)
                {
                    if (ap->mode == am_dreg)
                        peep_tail->oper1->preg = csp->reg - 12;
                    else
                    {
                        if (csp->reg - 12 > 3 && chksize(BESZ_WORD, sz))
                            DIAG("byte sized non-register in analyze");
                        ap2 = makedreg(csp->reg - 12);
                        ap2->length = BESZ_DWORD;
                        if (ap->mode == am_immed || ap->length == BESZ_DWORD || ap
                            ->length ==  - BESZ_DWORD)
                            gen_codes(op_mov, BESZ_DWORD, ap2, ap);
                        else
                            if (sz < 0)
                                gen_code(op_movsx, ap2, ap);
                            else
                                gen_code(op_movzx, ap2, ap);
                    }
                }
                else
                {
                    /* Should never get here */
                    DIAG("float reg assigned in analyze");
                }
                if (lvalue(exptr) && ((SYM*)exptr->v.p[0]->v.p[0])->funcparm)
                {
                    ((SYM*)exptr->v.p[0]->v.p[0])->mainsym->inreg = TRUE;
                    ((SYM*)exptr->v.p[0]->v.p[0])->mainsym->value.i =  - csp
                        ->reg - (csp->size < 0 ?  - csp->size: csp->size) *256;
                }
            }
        }
        csp = csp->next;
    }
    gen_label(startlab);
}

//-------------------------------------------------------------------------

void asm_scannode(ENODE *node)
{
    CSE *csp;
    if (node->nodetype == en_add || node->nodetype == en_addstruc)
    {
        asm_scannode(node->v.p[0]);
        asm_scannode(node->v.p[1]);
    }
    else
    {
        switch (node->nodetype)
        {
            case en_icon:
            case en_lcon:
            case en_iucon:
            case en_lucon:
            case en_ccon:
            case en_cucon:
            case en_boolcon:
                break;
            case en_napccon:
            case en_nacon:
            case en_absacon:
            case en_autocon:
            case en_autoreg:
                csp = enternode(node, 0, 0);
                csp->voidf = TRUE;
                break;
            case en_labcon:
            case en_nalabcon:
                break;
            default:
                DIAG("Invalid node in assembler line");
                break;
        }
    }
}

//-------------------------------------------------------------------------

void asm_scan1(AMODE *ap)
{
    if (!ap || !ap->offset)
        return ;
    asm_scannode(ap->offset);
}

//-------------------------------------------------------------------------

void asm_scan(OCODE *cd)
{
    asm_scan1(cd->oper1);
    asm_scan1(cd->oper2);
    asm_scan1(cd->oper3);
}

//-------------------------------------------------------------------------

void asm_repnode(ENODE **node)
{
    if ((*node)->nodetype == en_add || (*node)->nodetype == en_addstruc)
    {
        asm_repnode(&(*node)->v.p[0]);
        asm_repnode(&(*node)->v.p[1]);
    }
    else
    if ((*node)->nodetype == en_autocon || (*node)->nodetype == en_autoreg)
    {
        *node = makeintnode(en_icon, (*node)->v.sp->value.i);
    }
    else if ((*node)->nodetype == en_nacon || (*node)->nodetype == en_napccon)
    {
        *node = makenode((*node)->nodetype, (*node)->v.sp, 0);
    }
    else if ((*node)->nodetype == en_nalabcon)
    {
        *node = makenode((*node)->nodetype, (*node)->v.sp, 0);
    }
    else if ((*node)->nodetype == en_labcon)
    {
        *node = makeintnode((*node)->nodetype, (*node)->v.i);
    }
    else if ((*node)->nodetype == en_absacon)
    {
        *node = makeintnode(en_absacon, ((SYM*)(*node)->v.p[0])->value.i);

    }
}

//-------------------------------------------------------------------------

int voidexpr(ENODE *node)
{
    CSE *csp;
    if (node == 0)
        return 0;
    switch (node->nodetype)
    {
        case en_conslabel:
        case en_destlabel:
        case en_movebyref:
            return voidexpr(node->v.p[0]);
        case en_structret:
            return 0;
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_fcomplexcon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_fimaginarycon:
            return 1;
        case en_boolcon:
        case en_icon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_ccon:
        case en_cucon:
        case en_llcon:
        case en_llucon:
        case en_nacon:
        case en_napccon:
        case en_absacon:
        case en_autocon:
        case en_autoreg:
            return 0;
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
            return 1;
        case en_bool_ref:
        case en_ub_ref:
        case en_uw_ref:
        case en_b_ref:
        case en_w_ref:
        case en_l_ref:
        case en_ul_ref:
        case en_ll_ref:
        case en_ull_ref:
        case en_fp_ref:
			case en_i_ref:
			case en_ui_ref:
			case en_a_ref: case en_ua_ref:
            return 0;
        case en_uminus:
        case en_bits:
        case en_asuminus:
        case en_ascompl:
        case en_not:
        case en_compl:
        case en_cl_reg:
            return voidexpr(node->v.p[0]);
        case en_fcall:
        case en_pfcall:
        case en_pfcallb:
        case en_sfcall:
        case en_sfcallb:
        case en_fcallb:
            {
                SYM *sp;
                ENODE *node2 = node->v.p[1]->v.p[0]->v.p[0];
                if (node2->nodetype == en_nacon || node2->nodetype ==
                    en_napccon)
                {
                    sp = node2->v.sp;
                    if (sp && (sp->value.classdata.cppflags &PF_INLINE))
                        voidfloat(sp->value.classdata.inlinefunc->stmt);
                }
            }
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_cfi:
        case en_cri:
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
        case en_cf:
        case en_cd:
        case en_cp:
        case en_cld:
        case en_cfp:
        case en_csp:
        case en_thiscall:
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
        case en_pmul:
		case en_arrayindex:
        case en_moveblock:
        case en_stackblock:
        case en_intcall:
        case en_pdiv:
        case en_repcons:
        case en_scallblock:
        case en_callblock:
        case en_pcallblock:
		case en_array:
            return voidexpr(node->v.p[0]) || voidexpr(node->v.p[1]);
        case en_trapcall:
        case en_substack:
        case en_clearblock:
		case en_loadstack:
		case en_savestack:
            return voidexpr(node->v.p[0]);
            //												return voidexpr(node->v.p[1]);
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
        case en_lassign:
            if (voidexpr(node->v.p[1]))
            {
                csp = searchnode(node->v.p[0]);
                if (csp)
                    csp->voidf = 1;
            }
            return voidexpr(node->v.p[0]);
        default:
            return 0;
    }

}

//-------------------------------------------------------------------------

void voidfloat(SNODE *block)
/*
 * Scan through a block and void all CSEs which do asadd, asmul, asmodiv
 * of float to int
 */
{
    while (block != 0)
    {
        switch (block->stype)
        {
            case st_return:
            case st_expr:
                voidexpr(block->exp);
                break;
            case st_while:
            case st_do:
                voidexpr(block->exp);
                voidfloat(block->s1);
                break;
            case st_for:
                voidexpr(block->label);
                voidexpr(block->exp);
                voidfloat(block->s1);
                voidexpr(block->s2);
                break;
            case st_if:
                voidexpr(block->exp);
                voidfloat(block->s1);
                voidfloat(block->s2);
                break;
            case st_switch:
                voidexpr(block->exp);
                voidfloat(block->s1);
                break;
            case st_case:
                voidfloat(block->s1);
                break;
            case st_block:
                voidfloat(block->exp);
                break;
        }
        block = block->next;
    }
}

//-------------------------------------------------------------------------

void asm_repcse1(AMODE *ap)
{
    if (!ap || !ap->offset)
        return ;
    asm_repnode(&ap->offset);
}

//-------------------------------------------------------------------------

void asm_repcse(OCODE *cd)
{
    asm_repcse1(cd->oper1);
    asm_repcse1(cd->oper2);
    asm_repcse1(cd->oper3);
}
