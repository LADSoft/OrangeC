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
#include "gen386.h"

extern SYM *currentfunc;
extern long nextlabel;
extern int prm_peepopt, prm_dsnotss;
extern int prm_nasm;
extern int prm_asmfile;

OCODE *peep_head = 0, *peep_tail = 0, *peep_insert;
int noRelease;

void peepini(void)
{
    peep_head = peep_tail = 0;
}

//-------------------------------------------------------------------------

AMODE *copy_addr(AMODE *ap)
/*
 *      copy an address mode structure (these things dont last).
 */
{
    AMODE *newap;
    if (ap == 0)
        return 0;
    newap = xalloc(sizeof(AMODE));
    newap->mode = ap->mode;
    newap->preg = ap->preg;
    newap->sreg = ap->sreg;
    newap->seg = ap->seg;
    newap->length = ap->length;
    newap->tempflag = ap->tempflag;
    newap->offset = ap->offset;
    newap->scale = ap->scale;
    return newap;
}

/* any code that is being added to the peep list MUST have its operands
 * processed here so that the length fields will be valid
 * ultimately, all code generation calls gen_code() which calls this.
 *
 */
static void fixlen(int op, AMODE *ap)
{
    if (ap)
    {
        /* the output routines require positive length field */
        if (ap->length < 0)
            ap->length =  - ap->length;
        if (prm_nasm && ap->mode == am_immed && isintconst(ap->offset->nodetype)
            )
        {
            /* for the x86, assume initially that integer constants are unsized
             * (I have really messed up the code generator in this regard
             * so we do it here).
             * But the x86 allows signed byte constants on most math ops
             * so, since NASM won't do this automatically we must tell it the
             * sizing and we do this by setting the constant size to 1
             * and the output routines will know what to do with this info
             */
            if ((int)ap->offset->v.i < 0x80 && (int)ap->offset->v.i >=  - 0x80)
                ap->length = 1;
            else
                ap->length = 0;

        }
    }
}

//-------------------------------------------------------------------------

OCODE *gen_code(int op, AMODE *ap1, AMODE *ap2)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE *new;
	if (prm_dsnotss)
	{
		if (op == op_lea)
			switch(ap2->mode)
			{
				case am_indispscale:
					if (ap2->sreg == EBP)
					{
						ap1->seg = SS;
						break;
					}
				case am_direct:
				case am_indisp:
					if (ap2->preg == EBP)
					{
						ap1->seg = SS;
					}
					break;
				default:
					break;
			}
		else
			if (op == op_mov && ap1->mode == am_dreg && ap2->mode == am_dreg)
				ap1->seg = ap2->seg;
	}
    new = xalloc(sizeof(OCODE));
    new->opcode = op;
    new->oper1 = copy_addr(ap1);
    new->oper2 = copy_addr(ap2);
    if (op == op_lea)
        new->oper2->seg = 0;
    fixlen(op,new->oper1);
    fixlen(op,new->oper2);
    add_peep(new);
	if (!noRelease)
		FreeInd(ap1, ap2);
	noRelease = FALSE;
    return new;
}

//-------------------------------------------------------------------------

OCODE *gen_code3(int op, AMODE *ap1, AMODE *ap2, AMODE *ap3)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE *new;
    new = xalloc(sizeof(OCODE));
    new->opcode = op;
    new->oper1 = copy_addr(ap1);
    new->oper2 = copy_addr(ap2);
    new->oper3 = copy_addr(ap3);
    fixlen(op,new->oper1);
    fixlen(op,new->oper2);
    fixlen(op,new->oper3);
    add_peep(new);
    return new;
}

//-------------------------------------------------------------------------

OCODE *gen_codes(int op, int len, AMODE *ap1, AMODE *ap2)
{
    OCODE *new = gen_code(op, ap1, ap2);
    if (len < 0)
        len =  - len;
    if (new->oper1)
        new->oper1->length = len;
    if (new->oper2)
        new->oper2->length = len;
    return new;
}

//-------------------------------------------------------------------------

void gen_coden(int op, int len, AMODE *ap1, AMODE *ap2)
{
    OCODE *new = gen_codes(op, len, ap1, ap2);
    new->noopt = TRUE;
}

//-------------------------------------------------------------------------

void gen_codefs(int op, int len, AMODE *ap1, AMODE *ap2)
{
    if (ap1 && ap2 && ap1->mode == am_freg && ap2->mode == am_freg)
        len = 10;
    gen_codes(op, len, ap1, ap2);
}

//-------------------------------------------------------------------------

void gen_codef(int op, AMODE *ap1, AMODE *ap2)
{
    if (ap1)
        gen_codefs(op, ap1->length, ap1, ap2);
    else
        gen_codefs(op, 0, ap1, ap2);
}

//-------------------------------------------------------------------------

void gen_codes2(int op, int len, AMODE *ap1, AMODE *ap2)
{
    OCODE *new = gen_code(op, ap1, ap2);
    if (len < 0)
        len =  - len;
    if (new->oper2)
        new->oper2->length = len;
}

//-------------------------------------------------------------------------

void gen_code2(int op, int len1, int len2, AMODE *ap1, AMODE *ap2)
{
    OCODE *new = gen_code(op, ap1, ap2);
    if (len1 < 0)
        len1 =  - len1;
    if (len2 < 0)
        len2 =  - len2;
    new->oper1->length = len1;
    if (new->oper2)
        new->oper2->length = len2;
}

//-------------------------------------------------------------------------

void gen_codelab(SYM *lab)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE *new;
    new = xalloc(sizeof(OCODE));
    new->opcode = op_funclabel;
    new->oper1 = lab;
    new->oper2 = 0;
    add_peep(new);
}

//-------------------------------------------------------------------------

void gen_branch(int op, AMODE *ap1)
{
    gen_code(op, ap1, 0);
}

//-------------------------------------------------------------------------

void gen_line(SNODE *stmt)
{
    OCODE *new = xalloc(sizeof(OCODE));
    new->opcode = op_line;
    new->oper3 = (AMODE*)((int)stmt->s1);
    new->oper2 = (AMODE*)((int)stmt->exp);
    new->oper1 = (AMODE*)stmt->label;
    add_peep(new);
}

//-------------------------------------------------------------------------

void gen_comment(char *txt)
{
    OCODE *new = xalloc(sizeof(OCODE));
    new->opcode = op_comment;
    new->oper2 = 0;
    new->oper1 = (AMODE*)txt;
    add_peep(new);
}

//-------------------------------------------------------------------------


void add_peep(OCODE *new)
/*
 *      add the ocoderuction pointed to by new to the peep list.
 */
{
    if (peep_head == 0)
    {
        peep_head = peep_tail = new;
        new->fwd = 0;
        new->back = 0;
    }
    else
    {
        new->fwd = 0;
        new->back = peep_tail;
        peep_tail->fwd = new;
        peep_tail = new;
    }
}

//-------------------------------------------------------------------------

void gen_label(int labno)
/*
 *      add a compiler generated label to the peep list.
 */
{
    OCODE *new;
    new = xalloc(sizeof(OCODE));
    new->opcode = op_label;
    new->oper1 = (AMODE*)labno;
    add_peep(new);
}

//-------------------------------------------------------------------------

void flush_peep(void)
/*
 *      output all code and labels in the peep list.
 */
{
    opt3(); /* do the peephole optimizations */
    opt3();
    if (peep_head)
        outcode_gen(peep_head);
    if (prm_asmfile)
    {
        while (peep_head != 0)
        {
            switch (peep_head->opcode)
            {
                case op_label:
                    put_label((int)peep_head->oper1);
                    break;
                case op_funclabel:
                    gen_strlab(peep_head->oper1);
                    break;
                default:
                    put_code(peep_head);
                    break;

            }
            peep_head = peep_head->fwd;
        }
    }
    peep_head = 0;
}

//-------------------------------------------------------------------------

void peep_add(OCODE *ip)
/*
 * Turn add,1 into inc
 * turn add,0 into nothing (can be genned by the new structure mechanism)
 */
{
    if (ip->oper2->mode != am_immed || !isintconst(ip->oper2->offset->nodetype))
        return ;

    /* change to inc */
    if (ip->oper2->offset->v.i == 1)
    {
        if (ip->fwd->opcode != op_adc)
        {
            ip->opcode = op_inc;
            ip->oper2 = 0;
		    return ;
        }
    }
    else if (ip->oper2->offset->v.i == 0)
    {
        remove_peep_entry(ip);
	    return ;
    }
	if (ip->oper1->length == 1 || ip->oper1->length == -1)
	{
		ip->oper2->offset->v.i &= 0xff;
	}
	if (ip->oper1->length == 2 || ip->oper1->length == -2)
	{
		if (ip->oper2->length != 1)
			ip->oper2->offset->v.i &= 0xffff;
	}
}

/*
 * Turn sub,1 into dec
 */
void peep_sub(OCODE *ip)
{
    if (ip->oper2->mode != am_immed || !isintconst(ip->oper2->offset->nodetype))
        return ;
    if (ip->oper2->offset->v.i == 1)
    {
        if (ip->fwd->opcode != op_sbb)
        {
            ip->opcode = op_dec;
            ip->oper2 = 0;
		    return ;
        }
    }
	if (ip->oper2->mode == am_immed)
	{
		if (ip->oper1->length == 1 || ip->oper1->length == -1)
		{
			ip->oper2->offset->v.i &= 0xff;
		}
		if (ip->oper1->length == 2 || ip->oper1->length == -2)
		{
			if (ip->oper2->length != 1)
				ip->oper2->offset->v.i &= 0xffff;
		}
		if (ip->oper1->mode == am_dreg && ip->oper1->preg == ESP)
		{
			if ((int)ip->oper2->offset->v.i  < 0)
			{
				ip->oper2->offset->v.i = - ip->oper2->offset->v.i;
				ip->opcode = op_add;
			}
		}
	}
}

/*
 * shifts left a constant node, error if it is not composed of adds
 * and constants
 */
int makeshl(ENODE *node, int scale, int *rv)
{
    int t1, t2, rv1, rv2;
    switch (node->nodetype)
    {
        case en_add:
        case en_addstruc:
            t1 = makeshl(node->v.p[0], scale, &rv1);
            t1 &= makeshl(node->v.p[1], scale, &rv2);
            if (!t1)
                return 0;
            *rv = rv1 + rv2;
            return 1;
        case en_icon:
        case en_iucon:
        case en_lcon:
        case en_lucon:
        case en_ccon:
        case en_cucon:
        case en_boolcon:
            *rv = node->v.i << scale;
            return 1;
    }
    return 0;

}

/*
 * fix lea followed by indirect use of the register
 * may be somewhat iffy, but, I don't think that indirect modes
 * get reused so it should be safe...
 */
void peep_lea2(OCODE *ip, AMODE *ap)
{
    #ifdef XXXXX
        if (!ap)
            return ;
        if (ap->mode == am_indisp)
        {
            if (ap->preg == ip->oper1->preg)
            {
                ap->mode = ip->oper2->mode;
                ap->preg = ip->oper2->preg;
                ap->sreg = ip->oper2->sreg;
                ap->scale = ip->oper2->scale;
                ap->offset = makenode(en_add, ap->offset, ip->oper2->offset);
            }
            else
                return ;
        }
        else if (ap->mode == am_indispscale && (ip->oper2->mode == am_indisp ||
            ip->oper2->mode == am_direct))
        {
            if (ap->preg == ip->oper1->preg)
            {
                if (ip->oper2->mode == am_indisp)
                    ap->preg = ip->oper2->preg;
                else
                    ap->mode = am_indisp;
                ap->offset = makenode(en_add, ap->offset, ip->oper2->offset);
            }
            else if (ap->sreg == ip->oper1->preg && ap->scale == 1)
            {
                if (ap->scale)
                {
                    long rv;
                    if (!makeshl(ip->oper2->offset, ap->scale, &rv))
                        return ;
                    ap->offset = makenode(en_add, ap->offset, makeintnode
                        (en_icon, rv));
                    if (ip->oper2->mode == am_indisp)
                        ap->sreg = ip->oper2->preg;
                    else
                        ap->mode = am_indisp;
                }
                else if (ip->oper2->mode == am_indisp)
                {
                    ap->sreg = ip->oper2->preg;
                    ap->offset = makenode(en_add, ap->offset, ip->oper2->offset)
                        ;
                }
                else

                {
                    ap->mode = am_indisp;
                    ap->offset = makenode(en_add, ap->offset, ip->oper2->offset)
                        ;
                }
            }
            else
                return ;
        }
        else
            return ;

        remove_peep_entry(ip);
    #endif 
}

/*
 * fix lea followed by indirect use of the register
 *
 */
void peep_lea(OCODE *ip)
{
    OCODE *ip1 = ip->fwd;

    /*  Returns if no optimization is to be done. */
    if (ip1->noopt)
        return ;
    /*  Replaces LEA by MOV if an address is to be taken. */
    if (ip->oper2->mode == am_direct && (ip->oper2->offset->nodetype ==
        en_napccon || ip->oper2->offset->nodetype == en_nacon || ip->oper2
        ->offset->nodetype == en_labcon))
    {
        if (ip1->opcode == op_push && equal_address(ip->oper1, ip1->oper1))
        {
            ip1->oper1 = ip->oper2;
            ip1->oper1->mode = am_immed;
            ip->back->fwd = ip1;
            ip1->back = ip->back;
        }
        else if (ip1->opcode == op_mov && equal_address(ip->oper1, ip1->oper2))
        {
			int domod = TRUE;
			
			OCODE *ipf1 = ip1->fwd;
	        while (ipf1 && ipf1->opcode != op_line && ipf1->opcode != op_label)
			{
				if (ipf1->oper2 && (ipf1->oper2->mode == am_dreg && ipf1->oper2->preg == ip->oper1->preg ||
									(ipf1->oper2->mode == am_indisp && ipf1->oper2->preg == ip->oper1->preg) ||
									(ipf1->oper2->mode == am_indispscale 
									 && (ipf1->oper2->preg == ip->oper1->preg || ipf1->oper2->sreg == ip->oper1->preg))))
				{
					domod = FALSE;
					break;
				}
				if (ipf1->oper1 && (ipf1->oper1->mode == am_dreg && ipf1->oper1->preg == ip->oper1->preg ||
									(ipf1->oper1->mode == am_indisp && ipf1->oper1->preg == ip->oper1->preg) ||
									(ipf1->oper1->mode == am_indispscale 
									 && (ipf1->oper1->preg == ip->oper1->preg || ipf1->oper1->sreg == ip->oper1->preg))))
				{
					if (ipf1->opcode != op_mov)
						domod = FALSE;
					break;
				}
				ipf1 = ipf1->fwd;
			}
			if (domod)
			{
	            ip1->oper2 = ip->oper2;
    	        ip1->oper2->mode = am_immed;
        	    ip->back->fwd = ip1;
            	ip1->back = ip->back;
			}
        }
        else
        {
            ip->opcode = op_mov;
            ip->oper2->mode = am_immed;
        }
        return ;
    }
    #ifdef XXXXX
        if (ip->oper1->preg < EBX)
        {
            ip1 = ip->fwd;
            while (ip1->opcode != op_line && ip1->opcode != op_label && ip1
                ->opcode != op_void && (ip1->oper1 && (ip1->oper1->mode !=
                am_dreg || ip1->oper1->preg != ip->oper1->preg)) || !ip1->oper1)
            {
                peep_lea2(ip, ip1->oper2);
                if (ip1->oper1 && ip1->oper1->mode == am_dreg && (ip1->oper1
                    ->preg == ip->oper2->preg || ip->oper2->sreg == ip1->oper1
                    ->preg))
                    break;
                peep_lea2(ip, ip1->oper1);
                ip1 = ip1->fwd;
            }
        }
    #endif 
    switch (ip->oper2->mode)
    {
        case am_indispscale:
            break;
            if (ip->oper2->sreg >= 0)
            {
                if (ip->oper1->preg != ip->oper2->preg || (ip->oper2->scale !=
                    0))
                    return ;
                if (ip->oper2->sreg == ip->oper1->preg)
                {
                    int temp = ip->oper2->sreg;

                    ip->oper2->sreg = ip->oper2->preg;
                    ip->oper2->preg = temp;
                }
                ip1 = xalloc(sizeof(OCODE));
                ip1->opcode = op_add;
                ip1->oper1 = makedreg(ip->oper1->preg);
                ip1->oper2 = makedreg(ip->oper2->sreg);
                ip1->oper1->length = ip1->oper2->length = ip->oper1->length;
                ip1->fwd = ip->fwd;
                ip1->back = ip;
                ip->fwd->back = ip1;
                ip->fwd = ip1;
            }
        case am_indisp:
            if (ip->oper1->preg != ip->oper2->preg)
            {
                return ;
            }
            ip->opcode = op_add;
            if (ip->oper2->offset)
            {
                AMODE *ap = ip->oper2;
                ip->oper2 = xalloc(sizeof(AMODE));
                ip->oper2->mode = am_immed;
                ip->oper2->offset = ap->offset;
            }
            else
            {
                ip->fwd->back = ip->back;
                ip->back->fwd = ip->fwd;
            }
            break;
    }
}

/*
 * fix lea followed by constant add to the register
 *
 */
void peep_lea3(OCODE *ip)
{
    OCODE *ip1 = ip->fwd;
    /*  Returns if no optimization is to be done. */
    if (ip->noopt)
        return ;
    if (ip->oper2->mode == am_indisp || ip->oper2->mode == am_indispscale)
    {
        if (ip1->opcode == op_add)
        {
            if (ip1->oper1->mode == am_dreg && equal_address(ip->oper1, ip1
                ->oper1))
            {
                if (ip1->oper2->mode == am_immed)
                {
                    if (ip1->oper2->offset)
                        if (!ip->oper2->offset)
                            ip->oper2->offset = makeintnode(en_icon, ip1->oper2
                                ->offset->v.i);
                        else
                            ip->oper2->offset->v.i += ip1->oper2->offset->v.i;
                    ip1->fwd->back = ip1->back;
                    ip1->back->fwd = ip1->fwd;
                }
            }
        }
    }
}

/*
 * turn a movzx followed by a move into a single movzx if appropriate
 * or a movzx preceded by a move into a single movzx
 */
void peep_signextend(OCODE *ip)
{
    OCODE *ip1 = ip->back;
    if (ip1->noopt)
        return ;
    if (ip1->opcode == op_mov && (/*equal_address(ip->oper2, ip1->oper1) ||*/ ip1
        ->oper1->mode == am_dreg && ip->oper2->mode == am_dreg && ip1->oper1
        ->preg == ip->oper2->preg) && ip1->oper2->mode != am_immed && (ip1
        ->oper2->mode != am_dreg || ip1->oper2->preg < EBX)) // || ip->oper2
//        ->length != 1 && ip->oper2->length !=  - 1))
    {
		if (ip1->oper2->mode == am_seg)
		{
			ip1->oper2->length = BESZ_WORD;
			ip->oper1->length = BESZ_DWORD;
			ip->opcode = op_mov;
		}
		else
	        ip1->oper2->length = ip->oper2->length;
        ip->oper2 = ip1->oper2;
        remove_peep_entry(ip1);
    }
    return ; /* buggy right now, see ar/maketbl.c */
    if (ip1->opcode == op_mov)
    {
        if (equal_address(ip->oper1, ip1->oper2))
        if (ip1->oper1->mode == am_dreg && ip->oper1->preg < EBX && (ip->oper2
            ->mode != am_dreg || ip->oper2->preg <= EBX || ip->oper2->length >
            1))
        {
            ip->oper1 = ip1->oper1;
            remove_peep_entry(ip1);
        }
    }
}

//-------------------------------------------------------------------------

void replacereg(AMODE *ap, int reg, int reg2)
{
    if (!ap)
        return ;
    switch (ap->mode)
    {
        case am_indispscale:
            if (ap->sreg == reg)
                ap->sreg = reg2;
        case am_indisp:
        case am_dreg:
            if (ap->preg == reg)
                ap->preg = reg2;
    }
}

//-------------------------------------------------------------------------

OCODE *peep_test(OCODE *ip)
{
    OCODE *ip2;

    /*  Checks if the current instruction in the peep is something like
     *        TEST reg,reg
     */
    if (ip->oper1->mode == am_dreg && equal_address(ip->oper1, ip->oper2))
    {
         /*  Goes to the previous significant instruction. Any label should
             cause a
         * stop in the search. The Line directives and the void opcodes are ignored.
         */
        for (ip2 = ip->back; ip2 && (ip2->opcode == op_line || ip2->opcode ==
            op_void); ip2 = ip2->back)
            ;
        if (!ip2)
            return ip;
        /*  The following combinations for the previous significant function will
         * cause the removal of the TEST reg,reg instruction:
         *
         *      ADC reg,...
         *      ADD reg,...
         *      AND reg,...
         *      DEC reg
         *      INC reg
         *      OR  reg,...
         *      SBB reg,...
         *      SUB reg,...
         *      XOR reg,...
         */
        if ((ip2->opcode == op_adc || ip2->opcode == op_add || ip2->opcode ==
            op_and || ip2->opcode == op_dec || ip2->opcode == op_inc || ip2
            ->opcode == op_or || ip2->opcode == op_sbb || ip2->opcode == op_sub
            || ip2->opcode == op_xor) && equal_address(ip2->oper1, ip->oper1))
        {
            ip2 = ip->back;
            remove_peep_entry(ip);
            return ip2;
        }
    }
    return ip;
}

/*
 * CMP after a mov to the reg optimized (when not reused)
 * cmp reg,0 -> test reg,reg
 */
void peep_cmp(OCODE *ip)
{
    OCODE *ip1;
    #ifdef XXXXX
        ip1 = ip->back;
        if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed && ip1
            ->opcode == op_mov && equal_address(ip->oper1, ip1->oper1))
        {
            if (!isintconst(ip1->oper2->offset->nodetype))
            {
                ip->oper1 = ip1->oper2;
                remove_peep_entry(ip1);
            }
        }
    #endif 
    ip1 = ip->fwd;
    if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed && ip->oper2
        ->offset->v.i == 0 && ip1->opcode != op_jb && ip1->opcode != op_jnc)
    {
        ip->opcode = op_test;
        ip->oper2 = copy_addr(ip->oper1);
    }
	if (ip->oper2->mode == am_immed)
	{
		if (ip->oper1->length == 1 || ip->oper1->length == -1)
		{
			ip->oper2->offset->v.i &= 0xff;
		}
		if (ip->oper1->length == 2 || ip->oper1->length == -2)
		{
			if (ip->oper2->length != 1)
				ip->oper2->offset->v.i &= 0xffff;
		}
	}
}

void peep_push(OCODE *ip)
{
	if (ip->oper1->mode == am_dreg && ip->oper1->preg == EDX)
	{
		OCODE *fwd = ip->fwd;
		if (fwd->opcode == op_push && fwd->oper1->mode == am_dreg && fwd->oper1->preg == EAX)
		{
			if (ip->back->opcode == op_mov && ip->back->back->opcode == op_mov)
			{
				if (ip->back->oper1->mode == am_dreg && ip->back->oper1->preg == EAX);
				{
					if (ip->back->back->oper1->mode == am_dreg && ip->back->back->oper1->preg == EDX)
					{
						ip->oper1 = ip->back->back->oper2;
						fwd->oper1 = ip->back->oper2;
						remove_peep_entry(ip->back->back);
						remove_peep_entry(ip->back);
					}
				}
			}
		}
	}
}
/*
 * turn qualified move + add into LEA
 * turn move followed by movxx into a single movxx if possible
 * Turn sequences with a mov followed by several adds and a move to
 * another register into one less instruction
 * turn a mov mem,reg followed by a use of the mem into a use of the reg
 * turn moves to self into nothingness
 * turn moves of seg to a (short) variable into one line of code
 */
void peep_mov(OCODE *ip)
{
    OCODE *ip1 = ip->fwd;
    OCODE *ipb1,  *ipb2 = 0, *ipf1;
    OCODE *ip2;
    int bad = 0;

    if (ip1 && ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed)
    {
        /*
         *  Treats the MOV REG,const/ADD REG,const instruction sequences.
         */
        if (ip1->opcode == op_add && equal_address(ip->oper1, ip1->oper1) &&
            ip1->oper2->mode == am_immed)
        {
            ip->oper2->offset = makenode(en_add, ip->oper2->offset, ip1->oper2
                ->offset);
            remove_peep_entry(ip1);
            ip1 = ip->fwd;
        }
    }
    if (ip1 && ip->oper1->mode == am_dreg && ip->oper2->mode == am_seg && ip1
        ->opcode == op_mov && ip1->oper2->mode == am_dreg && ip1->oper2->length
        == 2)
    {

        ip->oper1 = ip1->oper1;
        remove_peep_entry(ip1);
        return ;
    }
    /*
     *  Treats the MOV REG,const/MOV [mem],reg instruction sequences.
     */
    ip1 = ip->back;
    if (ip1 && ip1->opcode == op_mov && ip1->oper2->mode == am_immed &&
        equal_address(ip1->oper1, ip->oper2) && (ip->oper1->mode ==
        am_indispscale || ip->oper1->mode == am_indisp || ip->oper1->mode ==
        am_direct))
    {
		int domod = TRUE;
		
		ipf1 = ip->fwd;
        while (ipf1 && ipf1->opcode != op_line && ipf1->opcode != op_label)
		{
			if (ipf1->oper2 && (ipf1->oper2->mode == am_dreg && ipf1->oper2->preg == ip1->oper1->preg ||
								(ipf1->oper2->mode == am_indisp && ipf1->oper2->preg == ip1->oper1->preg) ||
								(ipf1->oper2->mode == am_indispscale 
								 && (ipf1->oper2->preg == ip1->oper1->preg || ipf1->oper2->sreg == ip1->oper1->preg))))
			{
				domod = FALSE;
				break;
			}
			if (ipf1->oper1 && (ipf1->oper1->mode == am_dreg && ipf1->oper1->preg == ip1->oper1->preg ||
								(ipf1->oper1->mode == am_indisp && ipf1->oper1->preg == ip1->oper1->preg) ||
								(ipf1->oper1->mode == am_indispscale 
								 && (ipf1->oper1->preg == ip1->oper1->preg || ipf1->oper1->sreg == ip1->oper1->preg))))
			{
				if (ipf1->opcode != op_mov)
					domod = FALSE;
				break;
			}
			ipf1 = ipf1->fwd;
		}
		if (domod)
		{
	        ip->oper2 = ip1->oper2;
    	    remove_peep_entry(ip1);
		}
    }
    ip1 = ip->fwd;
    /* move followed by lea indisp of reg */
    if (ip->oper1->mode == am_dreg && ip->oper1->preg < EBX && ip1->opcode ==
        op_lea && ip1->oper2->mode == am_indisp && ip1->oper2->preg == ip
        ->oper1->preg)
    if (ip1->oper2->offset == 0 || ip1->oper2->offset->v.i == 0)
    {
        ip->oper1->preg = ip1->oper1->preg;
        remove_peep_entry(ip1);
    }

    ipb1 = ip->back;
    if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_dreg && novalue(ip)
        && ip->oper2->tempflag && ipb1)
    {
        while (ipb1 && ipb1->opcode != op_line && ipb1->opcode != op_label &&
            ipb1->opcode != op_movzx && ipb1->opcode != op_movsx && ipb1->oper1
            && equal_address(ipb1->oper1, ip->oper2) && (!ipb1->oper2 ||
            !equal_address(ip->oper1, ipb1->oper2) && (ipb1->oper2->mode !=
            am_indisp || ipb1->oper2->preg != ip->oper1->preg) && (ipb1->oper2
            ->mode != am_indispscale || ip->oper1->preg != ipb1->oper2->preg &&
            ip->oper1->preg != ipb1->oper2->sreg)))
        {
            if (ipb1->opcode == op_mov || ipb1->opcode == op_lea)
            {
                ipb2 = ipb1;
                break;
            }
            else if (ipb1->oper2 && equal_address(ip->oper1, ipb1->oper2))
                break;
            ipb1 = ipb1->back;
        }
        if (ipb2)
        {
            while (ipb2 && ipb2 != ip)
            {
                replacereg(ipb2->oper1, ip->oper2->preg, ip->oper1->preg);
                if (ipb2 != ipb1)
                    replacereg(ipb2->oper2, ip->oper2->preg, ip->oper1->preg);
                ipb2 = ipb2->fwd;
            }
            remove_peep_entry(ip);
        }
    }

    ip2 = ip1->fwd;

    if (ip->opcode == op_mov && equal_address(ip->oper1, ip->oper2))
    {
        remove_peep_entry(ip);
    }
}

/*
 * change some ands to registers to movzx
 * delete ands of full size
 * delete itself if followed by AND or OR with same args...
 */
void peep_and(OCODE *ip)
{
    OCODE *ip2 = ip->fwd;
    OCODE *ip3 = ip->back;
    int i, j;
    if (ip2->opcode == op_or || ip2->opcode == op_and)
    {
        if (equal_address(ip2->oper1, ip->oper1) && (equal_address(ip2->oper2,
            ip->oper2) || ip->oper2->mode == ip2->oper2->mode && ip->oper2
            ->mode == am_immed && ip->oper2->offset->v.i == ~ip2->oper2->offset
            ->v.i))
        {
            remove_peep_entry(ip);
            return ;
        }
    }
    if (ip->oper1->mode == am_dreg)
    {
        if (ip->oper2->mode == am_immed)
        {
            unsigned t = ip->oper2->offset->v.i;
            if (ip->oper1->length == 2)
            {
                t &= 0xffff;
                if (t == 0xff && ip->oper1->preg <= EBX)
                {
                    ip->oper2->length = 1;
                    ip->oper2->mode = am_dreg;
                    ip->oper2->preg = ip->oper1->preg;
                    ip->opcode = op_movzx;
                }
                else if (t == 0xffff)
                {
                    ip->back->fwd = ip->fwd;
                    ip->fwd->back = ip->back;
					return;
                }
            }
            else if (ip->oper1->length == 4)
            {
                if (t == 0xff && ip->oper1->preg <= EBX)
                {
                    ip->oper2->length = 1;
                    ip->oper2->mode = am_dreg;
                    ip->oper2->preg = ip->oper1->preg;
                    ip->opcode = op_movzx;
                }
                else if (t == 0xffff)
                {
                    ip->oper2->length = 2;
                    ip->oper2->mode = am_dreg;
                    ip->oper2->preg = ip->oper1->preg;
                    ip->opcode = op_movzx;
                }
                else if (t == 0xffffffff)
                {
                    ip->back->fwd = ip->fwd;
                    ip->fwd->back = ip->back;
					return;
                }
            }
        }
    }
	if (ip->oper2->mode == am_immed)
	{
		if (ip->oper1->length == 1 || ip->oper1->length == -1)
		{
			ip->oper2->offset->v.i &= 0xff;
		}
		if (ip->oper1->length == 2 || ip->oper1->length == -2)
		{
			if (ip->oper2->length != 1)
				ip->oper2->offset->v.i &= 0xffff;
		}
	}
}
void peep_or(OCODE *ip)
{
	if (ip->oper2->mode == am_immed)
	{
		if (ip->oper1->length == 1 || ip->oper1->length == -1)
		{
			ip->oper2->offset->v.i &= 0xff;
		}
		if (ip->oper1->length == 2 || ip->oper1->length == -2)
		{
			if (ip->oper2->length != 1)
				ip->oper2->offset->v.i &= 0xffff;
		}
	}
}
/*
 * take care of the special case a = ~ a or a = - a
 */
void peep_neg(OCODE *ip)
{
    if (!novalue(ip->fwd))
        return ;
    if (ip->fwd->opcode != op_mov || ip->back->opcode != op_mov)
        return ;
    if (!equal_address(ip->oper1, ip->fwd->oper2) || !equal_address(ip->oper1,
        ip->back->oper1))
        return ;
    if (!equal_address(ip->back->oper2, ip->fwd->oper1))
        return ;
    ip->oper1 = ip->fwd->oper1;
    remove_peep_entry(ip->back);
    remove_peep_entry(ip->fwd);
}

/*
 * get rid of a BTR immediately preceding a BTS if args match
 */
void peep_btr(OCODE *ip)
{
    if (ip->fwd->opcode == op_bts && equal_address(ip->oper1, ip->fwd->oper1)
        && equal_address(ip->oper2, ip->fwd->oper2))
    {
        remove_peep_entry(ip);
    }
}

/*
 * relbr->jmp->label  replaced with a relbr of opposite nature
 * not implemented yet
 */
OCODE *peep_jmp(OCODE *ip)
{
    static struct
    {
        enum e_op TrueJmp;
        enum e_op FalseJmp;
    }
    TableJmp[] = 
    {
        {
            op_ja, op_jna
        } , 
        {
            op_jae, op_jnae
        }
        , 
        {
            op_jb, op_jnb
        }
        , 
        {
            op_jbe, op_jnbe
        }
        , 
        {
            op_jc, op_jnc
        }
        , 
        {
            op_je, op_jne
        }
        , 
        {
            op_jg, op_jng
        }
        , 
        {
            op_jge, op_jnge
        }
        , 
        {
            op_jl, op_jnl
        }
        , 
        {
            op_jle, op_jnle
        }
        , 
        {
            op_jo, op_jno
        }
        , 
        {
            op_jp, op_jpo
        }
        , 
        {
            op_jpe, op_jpo
        }
        , 
        {
            op_js, op_jns
        }
        , 
        {
            op_jz, op_jnz
        }
        , 
        {
            op_void, op_void
        }
    };
    int i;
    OCODE *ip1,  *ip2;

    /*  Checks if the current instruction is a conditional jump. */
    for (i = 0; TableJmp[i].TrueJmp != op_void && TableJmp[i].TrueJmp != ip
        ->opcode && TableJmp[i].FalseJmp != ip->opcode; i++)
        ;
    /*  Treats the conditionnal jumps with immediate labels. */
    if (TableJmp[i].TrueJmp != op_void && ip->oper1->mode == am_immed && ip
        ->oper1->offset->nodetype == en_labcon)
    {
         /*  Looks for the next significant instruction. */
        for (ip1 = ip->fwd; ip1 && ip1->opcode == op_line; ip1 = ip1->fwd)
            ;
        if (ip1 && ip1->opcode == op_jmp && ip1->oper1->offset->nodetype ==
            en_labcon)
        {
             /*  Looks for the first significat line after the JMP. */
            for (ip2 = ip1->fwd; ip2 && ip2->opcode == op_line; ip2 = ip2->fwd)
                ;
            /*  Optimizes the

            Jcc  LAB1
            JMP  LAB2
            LAB1:

            sequence to

            JNcc LAB2
            LAB1:
             */
            if (ip2 && ip2->opcode == op_label && (int)(ip2->oper1) == ip
                ->oper1->offset->v.i)
            {
                ip->oper1 = ip1->oper1;
                if (ip->opcode == TableJmp[i].TrueJmp)
                    ip->opcode = TableJmp[i].FalseJmp;
                else
                    ip->opcode = TableJmp[i].TrueJmp;
                remove_peep_entry(ip1);
            }
        }
    }

    /*  Does the jump size optimization. As a 80486 instruction may have up to 15
     * bytes, if the target label is only up to 8 instructions away, the jumps can
     * be safely changed to SHORT ones.
     */
    #ifdef XXXXX
        if (ip->oper1->mode == am_immed && ip->oper1->offset->nodetype ==
            en_labcon)
        {
            for (i = 8, ip1 = ip->back; i && ip1 && (ip1->opcode != op_label ||
                (int)(ip1->oper1) != ip->oper1->offset->v.i); ip1 = ip1->back)
                if (ip1->opcode != op_label && ip1->opcode != op_line && ip1
                    ->opcode != op_void)
                    i--;
            if (!i || !ip1)
                for (i = 9, ip1 = ip->fwd; i && ip1 && (ip1->opcode != op_label
                    || (int)(ip1->oper1) != ip->oper1->offset->v.i); ip1 = ip1
                    ->fwd)
                    if (ip1->opcode != op_label && ip1->opcode != op_line &&
                        ip1->opcode != op_void)
                        i--;
            /* using the negative sign to tell it not to put in the OFFSET keyword */
            if (i && ip1)
                ip->oper1->length =  - 1;
        }
    #endif 
    return ip;
}

//-------------------------------------------------------------------------

void peep_uctran(OCODE *ip)
/*
 *      peephole optimization for unconditional transfers.
 *      deletes instructions which have no path.
 *      applies to bra, jmp, and rts instructions.
 */
{
    while (ip->fwd != 0 && ip->fwd->opcode != op_label)
    {
        if (ip->fwd->opcode == op_blockstart || ip->fwd->opcode == op_blockend)
            ip = ip->fwd;
        else
        {
            ip->fwd = ip->fwd->fwd;
            if (ip->fwd != 0)
                ip->fwd->back = ip;
        }
    }
}

//-------------------------------------------------------------------------

void peep_label(OCODE *ip)
/*
 *              peephole optimization for labels
 *              deletes relbranches that jump to the next instruction
 */
{
    OCODE *curpos,  *index;
    curpos = ip;
    if (!curpos->back)
        return ;
    do
    {
        curpos = curpos->back;
        if (!curpos)
            return ;
    }
    while (curpos->opcode == op_label || curpos->opcode == op_line);
    while ((curpos->opcode == op_jmp) || curpos->opcode == op_cmp || curpos
        ->opcode == op_test || (curpos->opcode == op_jne) || (curpos->opcode ==
        op_je) || (curpos->opcode == op_jge) || (curpos->opcode == op_jle) || 
        (curpos->opcode == op_jg) || (curpos->opcode == op_jl) || (curpos
        ->opcode == op_jnc) || (curpos->opcode == op_jbe) || (curpos->opcode ==
        op_ja) || (curpos->opcode == op_jb) || (curpos->opcode == op_js) || 
        (curpos->opcode == op_jns))
    {
        index = curpos->fwd;
        if (curpos->noopt)
            return ;
        if (curpos->opcode == op_cmp || curpos->opcode == op_test)
        {
            remove_peep_entry(curpos);
            curpos = curpos->back;
        }
        else
        {
            do
            {
                if ((index->opcode == op_label) && (curpos->oper1->mode ==
                    am_immed) && ((int)index->oper1 == curpos->oper1->offset
                    ->v.i))
                {
                    remove_peep_entry(curpos);
                    curpos = curpos->back;
                    break;
                }
                index = index->fwd;
            }
            while (index != ip->fwd);
            if (index == ip->fwd)
                break;
        }
        while (curpos->opcode == op_label || curpos->opcode == op_line)
            curpos = curpos->back;
    }
}

//-------------------------------------------------------------------------

int novalue(OCODE *ip)
{
    return ip->fwd->opcode == op_void;
}

//-------------------------------------------------------------------------

int equal_address(AMODE *ap1, AMODE *ap2)
{
    if (ap1->mode != ap2->mode)
        return (FALSE);
    switch (ap1->mode)
    {
        case am_axdx:
            return TRUE;
        case am_immed:
            return equalnode(ap1->offset, ap2->offset);
        case am_indispscale:
            if (ap1->scale != ap2->scale || ap1->sreg != ap2->sreg)
                return (FALSE);
            if (ap1->sreg != ap2->sreg)
                return FALSE;
        case am_indisp:
            if (ap1->offset)
            if (ap2->offset)
            {
                if (!equalnode(ap1->offset, ap2->offset))
                    return FALSE;
            }
            else
                return (FALSE);
            else
                if (ap2->offset)
                    return (FALSE);
        case am_dreg:
        case am_freg:
            if (ap1->preg != ap2->preg)
                return (FALSE);
            if (ap1->length != ap2->length)
                return FALSE;
            return TRUE;

        case am_direct:
            return equalnode(ap1->offset, ap2->offset);
    }
    return (FALSE);
}

/*
 * this returns the address of a single bit, or -1
 */
int single_bit(int val)
{
    unsigned long v = 1, t = val;
    int i;
    for (i = 0; i < 32; i++, v <<= 1)
    {
        if (v > t)
            return  - 1;
        if (v == t)
            return i;
    }
    return  - 1;

}

/*
 * this one extends two-byte ops to four bytes in order to avoid
 * OPSIZ prefixes
 */
void peep_prefixes(OCODE *ip)
{
    if (!ip->oper1)
        return ;
    if (ip->opcode == op_mov || ip->opcode == op_and || ip->opcode == op_sub ||
        ip->opcode == op_mul || ip->opcode == op_neg || ip->opcode == op_not)
    if (ip->oper1->mode == am_dreg && (!ip->oper2 || ip->oper2->mode == am_dreg)
        )
    {
        if (ip->oper1 && ip->oper1->length == 2)
            ip->oper1->length = 4;
        if (ip->oper2 && ip->oper2->length == 2)
            ip->oper2->length = 4;
    }
}

/*
 * This one optimizes the add,call sequence used by the C++ compiler in
 * generating calls therough the vtab
 */
void peep_call(OCODE *ip)
{
    OCODE *ip1 = ip->back;
    if (ip1->opcode == op_add && ip1->oper2->mode == am_immed)
    {
        if (ip->oper1->mode == am_indisp)
        {
            if (ip->oper1->preg == ip1->oper1->preg)
            {
                ip->oper1->offset = makenode(en_add, ip->oper1->offset, ip1
                    ->oper2->offset);
                ip1->back->fwd = ip;
                ip->back = ip1->back;
            }
        }
        else if (ip->oper1->mode == am_indispscale)
        {
            if (ip->oper1->preg == ip1->oper1->preg)
            {
                ip->oper1->offset = makenode(en_add, ip->oper1->offset, ip1
                    ->oper2->offset);
                ip1->back->fwd = ip;
                ip->back = ip1->back;
            }
            else if (ip->oper1->sreg == ip1->oper1->preg && ip->oper1->scale ==
                0)
            {
                ip->oper1->offset = makenode(en_add, ip->oper1->offset, ip1
                    ->oper2->offset);
                ip1->back->fwd = ip;
                ip->back = ip1->back;
            }
        }
    }
}

/*
 * Sometimes the following gets emitted:
 *  POP AX POP DX PUSH DX PUSH AX
 */
void peep_pop(OCODE *ip)
{
    OCODE *ip1 = ip;
    if (ip1->oper1->mode == am_dreg && ip1->oper1->preg == EAX)
    {
        ip1 = ip1->fwd;
        if (ip1->opcode == op_pop && ip1->oper1->mode == am_dreg && ip1->oper1
            ->preg == EDX)
        {
            ip1 = ip1->fwd;
            if (ip1->opcode == op_push && ip1->oper1->mode == am_dreg && ip1
                ->oper1->preg == EDX)
            {
                ip1 = ip1->fwd;
                if (ip1->opcode == op_push && ip1->oper1->mode == am_dreg &&
                    ip1->oper1->preg == EAX)
                {
                    ip1->fwd->back = ip->back;
                    ip->back->fwd = ip1->fwd;
                }
            }
        }
    }
    ip1 = ip->fwd;
    if (ip1->opcode == op_push && equal_address(ip->oper1, ip1->oper1))
    {
        ip->back->fwd = ip1->fwd;
        ip1->fwd->back = ip->back;
    }
}

/*
 *  This function removes the specified peep list entry from the
 * peep list. It insures ther is no NULL pointer to cause exceptions.
 */
void remove_peep_entry(OCODE *ip)
{
    if (ip->back)
        ip->back->fwd = ip->fwd;
    if (ip->fwd)
        ip->fwd->back = ip->back;
}

//-------------------------------------------------------------------------

void opt3(void)
/*
 *      peephole optimizer. This routine calls the instruction
 *      specific optimization routines above for each instruction
 *      in the peep list.
 */
{
    OCODE *ip;

    if (!prm_peepopt)
        return ;
    for (ip = peep_head; ip; ip = ip->fwd)
    if (!ip->noopt)
    {
        peep_prefixes(ip);
        switch (ip->opcode)
        {
            case op_pop:
                peep_pop(ip);
                break;
            case op_add:
                peep_add(ip);
                break;
			case op_or:
			case op_xor:
				peep_or(ip);
				break;
            case op_and:
                peep_and(ip);
                break;
            case op_btr:
                peep_btr(ip);
                break;
            case op_cmp:
                peep_cmp(ip);
                break;
            case op_lea:
                peep_lea(ip);
                peep_lea3(ip);
                break;
            case op_mov:
                peep_mov(ip);
                break;
            case op_movsx:
            case op_movzx:
                peep_signextend(ip);
                break;
            case op_neg:
            case op_not:
                peep_neg(ip);
                break;
            case op_sub:
                peep_sub(ip);
                break;
            case op_test:
                ip = peep_test(ip);
                break;
			case op_push:
				peep_push(ip);
				break;
            case op_jmp:
            case op_ja:
            case op_jna:
            case op_jae:
            case op_jnae:
            case op_jb:
            case op_jnb:
            case op_jbe:
            case op_jnbe:
            case op_jc:
            case op_jnc:
            case op_je:
            case op_jne:
            case op_jg:
            case op_jng:
            case op_jge:
            case op_jnge:
            case op_jl:
            case op_jnl:
            case op_jle:
            case op_jnle:
            case op_jo:
            case op_jno:
            case op_jp:
            case op_jpe:
            case op_jpo:
            case op_js:
            case op_jns:
            case op_jz:
            case op_jnz:
                ip = peep_jmp(ip);
                /* fall through */
                if (ip->opcode == op_jmp)
                    peep_uctran(ip);
                break;
            case op_ret:
                peep_uctran(ip);
                break;
            case op_label:
                peep_label(ip);
                break;
            case op_call:
                peep_call(ip);
                break;
        }
    }
}
