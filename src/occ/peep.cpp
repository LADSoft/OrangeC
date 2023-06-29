/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <cstring>
#include "be.h"
#include "config.h"
#include "gen.h"
#include "occ.h"
#include "OptUtils.h"
#include "gen.h"
#include "peep.h"
#include "outcode.h"
#include "outasm.h"
#include "memory.h"
/* peephole optimizations
 * well not exactly
 * the compiler actually generated risc-like instruction sequences
 * and some of the things in this module re-cisc it :)
 */

#define live(mask, reg) (mask & (1 << reg))

namespace occx86
{
OCODE *peep_head = 0, *peep_tail = 0, *peep_insert = 0;
void insert_peep_entry(OCODE* after, enum e_opcode opcode, int size, AMODE* ap1, AMODE* ap2);

void o_peepini(void) { peep_head = peep_tail = 0; }
void oa_adjust_codelab(void* select, int offset)
{
    OCODE* peep = (OCODE*)select;
    if (peep->oper1 && peep->oper1->offset && peep->oper1->offset->type == Optimizer::se_labcon)
        if (peep->oper1->offset->i < 0)
            peep->oper1->offset->i += offset;
    if (peep->oper2 && peep->oper2->offset && peep->oper2->offset->type == Optimizer::se_labcon)
        if (peep->oper2->offset->i < 0)
            peep->oper2->offset->i += offset;
    if (peep->oper3 && peep->oper3->offset && peep->oper3->offset->type == Optimizer::se_labcon)
        if (peep->oper3->offset->i < 0)
            peep->oper3->offset->i += offset;
}
AMODE* makedregSZ(int r, char size)
{
    AMODE* reg = makedreg(r);
    reg->length = size;
    return reg;
}
AMODE* makedreg(int r)
/*
 *      make an address reference to a data register.
 */
{
    AMODE* ap;
    ap = beLocalAllocate<AMODE>();
    ap->mode = am_dreg;
    ap->preg = r;
    ap->length = ISZ_UINT;
    ap->liveRegs = ~0;
    return ap;
}

AMODE* makefreg(int r)
/*
 *      make an address reference to a data register.
 */
{
    AMODE* ap;
    ap = beLocalAllocate<AMODE>();
    ap->mode = am_freg;
    ap->preg = r;
    ap->length = ISZ_LDOUBLE;
    return ap;
}
/*-------------------------------------------------------------------------*/

AMODE* copy_addr(AMODE* ap)
/*
 *      copy an address mode structure (these things dont last).
 */
{
    AMODE* newap;
    if (ap == 0)
        return 0;
    newap = beLocalAllocate<AMODE>();
    newap->mode = ap->mode;
    newap->preg = ap->preg;
    newap->sreg = ap->sreg;
    newap->seg = ap->seg;
    newap->length = ap->length;
    newap->tempflag = ap->tempflag;
    newap->offset = ap->offset;
    newap->scale = ap->scale;
    newap->liveRegs = ap->liveRegs;
    newap->keepesp = ap->keepesp;
    return newap;
}

/* any code that is being added to the peep list MUST have its operands
 * processed here so that the length fields will be valid
 * ultimately, all code generation calls gen_code() which calls this.
 *
 */
static void fixlen(AMODE* ap)
{
    if (ap)
    {
        /* the output routines require positive length field */
        if (ap->length < 0)
            ap->length = -ap->length;
        if ((Optimizer::cparams.prm_assembler == pa_nasm || Optimizer::cparams.prm_assembler == pa_oasm) && ap->mode == am_immed &&
            isintconst(ap->offset))
        {
            /* for the x86, assume initially that integer constants are unsized
             * But the x86 allows signed byte constants on most math ops
             * so, since NASM won't do this automatically we must tell it the
             * sizing and we do this by setting the constant size to 1
             * and the output routines will know what to do with this info
             */
            if ((int)ap->offset->i <= CHAR_MAX && (int)ap->offset->i >= CHAR_MIN)
                ap->length = ISZ_UCHAR;
            else
                ap->length = ISZ_NONE;
        }
        if (Optimizer::cparams.prm_useesp && (ap->mode == am_indisp || ap->mode == am_indispscale) && ap->offset)
        {
            if (usingEsp)
            {
                // this is to adjust the stack level to the current depth
                if (ap->preg == ESP && !ap->keepesp)
                    ap->offset = simpleExpressionNode(Optimizer::se_add, ap->offset,
                                                      Optimizer::simpleIntNode(Optimizer::se_i, pushlevel + funcstackheight));
            }
        }
    }
}

/*-------------------------------------------------------------------------*/

OCODE* gen_code(int op, AMODE* ap1, AMODE* ap2)
/*
 *      generate a code sequence into the peep list.
 */
{    
    OCODE* newitem;

    newitem = beLocalAllocate<OCODE>();
    newitem->opcode = (e_opcode)op;
    newitem->oper1 = copy_addr(ap1);
    newitem->oper2 = copy_addr(ap2);
    if (op == op_lea)
        newitem->oper2->seg = 0;
    fixlen(newitem->oper1);
    fixlen(newitem->oper2);
    add_peep(newitem);
    return newitem;
}
OCODE* gen_code_sse(int op, AMODE* ap1, AMODE* ap2)
{
    if (ap1->mode != am_dreg)
        ap1->length = 0;
    if (ap2->mode != am_dreg)
        ap2->length = 0;
    return gen_code(op, ap1, ap2);
}
OCODE* gen_code_sse(int ops, int opd, int sz, AMODE* ap1, AMODE* ap2)
{
    int op = opd;
    if (sz == ISZ_FLOAT || sz == ISZ_IFLOAT || sz == ISZ_CFLOAT)
        op = ops;
    if (ap1->mode != am_dreg)
        ap1->length = 0;
    if (ap2->mode != am_dreg)
        ap2->length = 0;
    return gen_code(op, ap1, ap2);
}
OCODE* gen_code_sse_imm(int ops, int opd, int sz, AMODE* ap1, AMODE* ap2, AMODE* ap3)
{
    int op = opd;
    if (sz == ISZ_FLOAT || sz == ISZ_IFLOAT || sz == ISZ_CFLOAT)
        op = ops;
    if (ap1->mode != am_dreg)
        ap1->length = 0;
    if (ap2->mode != am_dreg)
        ap2->length = 0;
    return gen_code3(op, ap1, ap2, ap3);
}

/*-------------------------------------------------------------------------*/

OCODE* gen_code3(int op, AMODE* ap1, AMODE* ap2, AMODE* ap3)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE* newitem;
    newitem = beLocalAllocate<OCODE>();
    newitem->opcode = (e_opcode)op;
    newitem->oper1 = copy_addr(ap1);
    newitem->oper2 = copy_addr(ap2);
    newitem->oper3 = copy_addr(ap3);
    fixlen(newitem->oper1);
    fixlen(newitem->oper2);
    fixlen(newitem->oper3);
    add_peep(newitem);
    return newitem;
}

/*-------------------------------------------------------------------------*/

OCODE* gen_codes(int op, int len, AMODE* ap1, AMODE* ap2)
{

    OCODE* newitem = gen_code(op, ap1, ap2);

    if (len < 0)
        len = -len;
    if (newitem->oper1)
        newitem->oper1->length = len;

    if (newitem->oper2 && ((len != ISZ_UINT && len != ISZ_U32) || !newitem->oper2->offset || newitem->oper2->mode != am_immed))
        newitem->oper2->length = len;
    return newitem;
}

/*-------------------------------------------------------------------------*/

void gen_coden(int op, int len, AMODE* ap1, AMODE* ap2)
{
    OCODE* newitem = gen_codes(op, len, ap1, ap2);
    newitem->noopt = true;
}

/*-------------------------------------------------------------------------*/

void gen_codefs(int op, int len, AMODE* ap1, AMODE* ap2)
{
    if (ap1 && ap2 && ap1->mode == am_freg && ap2->mode == am_freg)
        len = ISZ_LDOUBLE;
    gen_codes(op, len, ap1, ap2);
}

/*-------------------------------------------------------------------------*/

void gen_codef(int op, AMODE* ap1, AMODE* ap2)
{
    if (ap1)
        gen_codefs(op, ap1->length, ap1, ap2);
    else
        gen_codefs(op, 0, ap1, ap2);
}

/*-------------------------------------------------------------------------*/

void gen_codes2(int op, int len, AMODE* ap1, AMODE* ap2)
{
    OCODE* newitem = gen_code(op, ap1, ap2);
    if (len < 0)
        len = -len;
    if (newitem->oper2)
        newitem->oper2->length = len;
}

/*-------------------------------------------------------------------------*/

void gen_code2(int op, int len1, int len2, AMODE* ap1, AMODE* ap2)
{
    OCODE* newitem = gen_code(op, ap1, ap2);
    if (len1 < 0)
        len1 = -len1;
    if (len2 < 0)
        len2 = -len2;
    newitem->oper1->length = len1;
    if (newitem->oper2)
        newitem->oper2->length = len2;
}

/*-------------------------------------------------------------------------*/

void gen_codelab(Optimizer::SimpleSymbol* lab)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE* newitem;
    newitem = beLocalAllocate<OCODE>();
    newitem->opcode = (e_opcode)op_funclabel;
    newitem->oper1 = (AMODE*)lab;
    newitem->oper2 = 0;
    add_peep(newitem);
}

/*-------------------------------------------------------------------------*/

void gen_branch(int op, int label)
{
    AMODE* ap1 = make_label(label);
    gen_code(op, ap1, 0);
}

/*-------------------------------------------------------------------------*/

void gen_comment(char* txt)
{
    OCODE* newitem = beLocalAllocate<OCODE>();
    newitem->opcode = (e_opcode)op_comment;
    newitem->oper2 = 0;
    newitem->oper1 = (AMODE*)txt;
    add_peep(newitem);
}

/*-------------------------------------------------------------------------*/

void add_peep(OCODE* newitem)
/*
 *      add the instruction pointed to by newitem to the peep list.
 */
{
    if (peep_head == 0)
    {
        peep_head = peep_tail = newitem;
        newitem->fwd = 0;
        newitem->back = 0;
    }
    else
    {
        newitem->fwd = 0;
        newitem->back = peep_tail;
        peep_tail->fwd = newitem;
        peep_tail = newitem;
    }
}

/*-------------------------------------------------------------------------*/

void oa_gen_label(int labno)
/*
 *      add a compiler generated label to the peep list.
 */
{
    OCODE* newitem;
    newitem = beLocalAllocate<OCODE>();
    newitem->opcode = (e_opcode)op_label;
    newitem->oper1 = make_label(labno);
    add_peep(newitem);
}

/*-------------------------------------------------------------------------*/

void flush_peep(Optimizer::SimpleSymbol* funcsp, Optimizer::QUAD* list)
/*
 *      output all code and labels in the peep list.
 */
{
    (void)funcsp;
    (void)list;
    oa_peep(); /* do the peephole optimizations */
    oa_peep();
    if (peep_head)
        outcode_gen(peep_head);
    if (Optimizer::cparams.prm_asmfile)
    {
        while (peep_head != 0)
        {
            switch ((e_op)peep_head->opcode)
            {
                case op_label:
                    oa_put_label(peep_head->oper1->offset->i);
                    break;
                case op_funclabel:
                    oa_gen_strlab((Optimizer::SimpleSymbol*)peep_head->oper1);
                    break;
                default:
                    oa_put_code(peep_head);
                    break;
            }
            peep_head = peep_head->fwd;
        }
    }
    peep_head = 0;
}

/*-------------------------------------------------------------------------*/

void peep_add(OCODE* ip)
/*
 * Turn add,1 into inc
 * turn add,0 into nothing (can be genned by the newitem structure mechanism)
 */
{
    if (ip->oper2->mode != am_immed)
        return;

    if (ip->fwd->opcode != op_adc)
    {
        /* change to inc */
        if (isintconst(ip->oper2->offset))
        {
            if (ip->oper2->offset->i == 1)
            {
                ip->opcode = op_inc;
                ip->oper2 = 0;
                return;
            }
            else if (ip->oper2->offset->i == 0)
            {
                remove_peep_entry(ip);
                return;
            }
        }
        if (ip->oper1->mode == am_dreg)
        {
            OCODE* ip1 = ip->back;
            if (ip1 && (ip1->opcode == op_mov || ip1->opcode == op_add) && ip1->oper1->mode == am_dreg &&
                ip1->oper1->preg == ip->oper1->preg && ip1->oper2->mode == am_immed &&
                (!varsp(ip->oper2->offset) || !varsp(ip1->oper2->offset)))
            {
                if (!isintconst(ip->oper2->offset))
                    ip1->oper2->length = ISZ_UINT;
                ip->oper2->length = ISZ_UINT;
                ip->oper2->offset = simpleExpressionNode(Optimizer::se_add, ip->oper2->offset, ip1->oper2->offset);
                ip->opcode = ip1->opcode;
                remove_peep_entry(ip1);
            }
            else
            {
                ip1 = ip->fwd;
                if ((ip1->opcode == op_add) && ip1->oper1->mode == am_dreg && ip1->oper1->preg == ip->oper1->preg &&
                    ip1->oper2->mode == am_immed && (!varsp(ip->oper2->offset) || !varsp(ip1->oper2->offset)))
                {
                    if (!isintconst(ip->oper2->offset))
                        ip1->oper2->length = ISZ_UINT;
                    ip->oper2->length = ISZ_UINT;
                    ip->oper2->offset = simpleExpressionNode(Optimizer::se_add, ip->oper2->offset, ip1->oper2->offset);
                    ip->opcode = ip1->opcode;
                    remove_peep_entry(ip1);
                }
            }
        }
    }
    return;
}

/*
 * Turn sub,1 into dec
 */
void peep_sub(OCODE* ip)
{
    if (ip->oper2->mode != am_immed || !isintconst(ip->oper2->offset))
        return;
    if (ip->fwd->opcode != op_sbb)
    {
        if (ip->oper2->offset->i == 1)
        {
            ip->opcode = op_dec;
            ip->oper2 = 0;
        }
        else if (ip->oper2->offset->i == 0)
        {
            remove_peep_entry(ip);
        }
        else if (ip->oper1->mode == am_dreg && ip->oper1->preg == ESP)
        {
            if ((int)ip->oper2->offset->i < 0)
            {
                ip->oper2->offset->i = -ip->oper2->offset->i;
                ip->opcode = op_add;
            }
        }
    }
    return;
}

/*-------------------------------------------------------------------------*/

OCODE* peep_test(OCODE* ip)
{
    OCODE* ip2;

    if (ip->oper1->mode == am_dreg && (ip->oper1->length == ISZ_UINT || ip->oper1->length == ISZ_U32) &&
        ip->oper2->mode == am_immed && isintconst(ip->oper2->offset) && (ip->oper2->offset->i & 0xffffffffU) == 0xffffffffU)

    {
        ip->oper2 = ip->oper1;
    }
    /*  Checks if the current instruction in the peep is something like
     *        TEST reg,reg
     */
    if (ip->oper1->mode == am_dreg && equal_address(ip->oper1, ip->oper2))
    {
        /*  Goes to the previous significant instruction. Any label should
            cause a
        * stop in the search. The Line directives and the void opcodes are ignored.
        */
        for (ip2 = ip->back;
             ip2 && ((e_op)ip2->opcode == op_line || (e_op)ip2->opcode == op_void || (e_op)ip2->opcode == op_blockstart ||
                     (e_op)ip2->opcode == op_blockend || (e_op)ip2->opcode == op_varstart);
             ip2 = ip2->back)
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
        if ((ip2->opcode == op_adc || ip2->opcode == op_add || ip2->opcode == op_and || ip2->opcode == op_dec ||
             ip2->opcode == op_inc || ip2->opcode == op_or || ip2->opcode == op_sbb || ip2->opcode == op_sub ||
             ip2->opcode == op_xor) &&
            equal_address(ip2->oper1, ip->oper1))
        {
            ip2 = ip->back;
            remove_peep_entry(ip);
            return ip2;
        }
        /* just for fun change it to an AND instruction... */
        ip->opcode = op_and;
    }
    return ip;
}
OCODE* peep_neg(OCODE* ip)
{
    if (ip->oper1->mode == am_dreg)
    {
        OCODE* ipf = ip->fwd;
        while ((ipf->opcode >= op_aaa && ipf->opcode < op_jecxz) || ipf->opcode >= op_lahf)
        {
            if (ipf->opcode == op_add)
            {
                if (ipf->oper2->mode == am_dreg && ipf->oper2->preg == ip->oper1->preg)
                {
                    if (!live(ipf->oper2->liveRegs, ip->oper1->preg))
                    {
                        remove_peep_entry(ip);
                        ipf->opcode = op_sub;
                    }
                }
                break;
            }
            if (ipf->oper1)
            {
                if (ipf->oper1->mode == am_dreg || ipf->oper1->mode == am_indisp || ipf->oper1->mode == am_indispscale)
                    if (ipf->oper1->preg == ip->oper1->preg)
                        break;
                if (ipf->oper1->mode == am_indispscale)
                    if (ipf->oper1->sreg == ip->oper1->preg)
                        break;
            }
            if (ipf->oper2)
            {
                if (ipf->oper2->mode == am_dreg || ipf->oper2->mode == am_indisp || ipf->oper2->mode == am_indispscale)
                    if (ipf->oper2->preg == ip->oper1->preg)
                        break;
                if (ipf->oper2->mode == am_indispscale)
                    if (ipf->oper2->sreg == ip->oper1->preg)
                        break;
            }
            if (ipf->oper3)
            {
                if (ipf->oper3->mode == am_dreg || ipf->oper3->mode == am_indisp || ipf->oper3->mode == am_indispscale)
                    if (ipf->oper3->preg == ip->oper1->preg)
                        break;
                if (ipf->oper1->mode == am_indispscale)
                    if (ipf->oper1->sreg == ip->oper1->preg)
                        break;
            }
            ipf = ipf->fwd;
        }
    }

    return ip->fwd;
}
void peep_cmpzx(OCODE* ip)
{
    (void)ip;
    return;
#ifdef XXXXX
    if (ip->back->opcode == op_movzx && ip->back->back->opcode == op_movzx

        || ip->back->opcode == op_movsx && ip->back->back->opcode == op_movsx)
    {
        if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_dreg)
        {
            if (ip->oper1->preg == ip->back->back->oper1->preg && ip->oper2->preg == ip->back->oper1->preg)
            {
                if (ip->oper1->length == ip->back->back->oper1->length && ip->oper1->length == ip->back->oper1->length)
                {
                    if (ip->back->oper2->length == ip->back->back->oper2->length)
                    {
                        if (ip->back->oper2->mode == am_dreg || ip->back->back->oper2->mode == am_dreg)
                        {
                            int live = ip->oper1->liveRegs;
                            if (ip->back->opcode == op_movzx)
                            {
                                OCODE* ip2;
                                ip2 = ip->fwd;
                                while (ip2 && ip2->opcode < op_aaa && ip2->opcode != op_label)
                                    ip2 = ip2->fwd;
                                if (ip2->opcode == op_mov && ip2->oper1->mode == am_dreg && isintconst(ip2->oper2->offset) &&
                                    ip2->oper2->offset->i == 0)
                                {
                                    if (ip2->fwd->oper1 && ip2->fwd->oper1->mode == am_dreg &&
                                        ip2->fwd->oper1->preg == ip2->oper1->preg)
                                        switch (ip2->fwd->opcode)
                                        {
                                            case op_setl:
                                                ip2->fwd->opcode = op_setb;
                                                break;
                                            case op_setle:
                                                ip2->fwd->opcode = op_setbe;
                                                break;
                                            case op_setg:
                                                ip2->fwd->opcode = op_seta;
                                                break;
                                            case op_setge:
                                                ip2->fwd->opcode = op_setae;
                                                break;
                                        }
                                }
                                else
                                    switch (ip2->opcode)
                                    {
                                        case op_jl:
                                            ip2->opcode = op_jb;
                                            break;
                                        case op_jle:
                                            ip2->opcode = op_jbe;
                                            break;
                                        case op_jg:
                                            ip2->opcode = op_ja;
                                            break;
                                        case op_jge:
                                            ip2->opcode = op_jae;
                                            break;
                                    }
                            }
                            ip->oper1 = ip->back->back->oper2;
                            ip->oper2 = ip->back->oper2;
                            if (!live(live, ip->back->back->oper1->preg))
                                remove_peep_entry(ip->back->back);
                            if (!live(live, ip->back->oper1->preg))
                                remove_peep_entry(ip->back);
                        }
                        else
                        {
                            ip->back->back->oper1->length = ip->back->back->oper2->length;
                            ip->back->back->opcode = op_mov;
                            ip->oper2 = ip->back->oper2;
                            ip->oper1->length = ip->oper2->length;
                            if (!live(ip->oper1->liveRegs, ip->back->oper1->preg))
                                remove_peep_entry(ip->back);
                        }
                    }
                }
            }
        }
    }
    else if (ip->back->opcode == op_movzx || ip->back->opcode == op_movsx)
    {
        if (ip->oper2->mode == am_immed)
        {
            int bContinue;
            if (ip->back->oper2->length == ISZ_UCHAR)
            {
                int n = ip->oper2->offset->i & 0xffffff80U;
                int n1 = n & 0xffffff00U;
                bContinue =
                    (ip->back->opcode == op_movsx && (n == 0 || n == 0xffffff80U) || ip->back->opcode == op_movzx && n1 == 0);
            }
            else
            {
                int n = ip->oper2->offset->i & 0xffff8000U;
                int n1 = n & 0xffff0000U;
                bContinue =
                    (ip->back->opcode == op_movsx && (n == 0 || n == 0xffff8000U) || ip->back->opcode == op_movzx && n1 == 0);
            }
            if (bContinue)
            {
                if (ip->oper1->mode == am_dreg && ip->back->oper1->mode == am_dreg && ip->oper1->preg == ip->back->oper1->preg)
                {
                    if (!live(ip->oper1->liveRegs, ip->oper1->preg))
                    {
                        ip->oper1 = ip->back->oper2;
                        ip->oper2->length = ip->oper1->length;
                        if (ip->back->opcode == op_movzx)
                        {
                            OCODE* ip2;
                            ip2 = ip->fwd;
                            while (ip2 && ip2->opcode < op_aaa && ip2->opcode != op_label)
                                ip2 = ip2->fwd;
                            if (ip2->opcode == op_mov && ip2->oper1->mode == am_dreg && isintconst(ip2->oper2->offset) &&
                                ip2->oper2->offset->i == 0)
                            {
                                if (ip2->fwd->oper1 && ip2->fwd->oper1->mode == am_dreg &&
                                    ip2->fwd->oper1->preg == ip2->oper1->preg)
                                    switch (ip2->fwd->opcode)
                                    {
                                        case op_setl:
                                            ip2->fwd->opcode = op_setb;
                                            break;
                                        case op_setle:
                                            ip2->fwd->opcode = op_setbe;
                                            break;
                                        case op_setg:
                                            ip2->fwd->opcode = op_seta;
                                            break;
                                        case op_setge:
                                            ip2->fwd->opcode = op_setae;
                                            break;
                                    }
                            }
                            else
                                switch (ip2->opcode)
                                {
                                    case op_jl:
                                        ip2->opcode = op_jb;
                                        break;
                                    case op_jle:
                                        ip2->opcode = op_jbe;
                                        break;
                                    case op_jg:
                                        ip2->opcode = op_ja;
                                        break;
                                    case op_jge:
                                        ip2->opcode = op_jae;
                                        break;
                                }
                        }
                        if (!live(ip->oper1->liveRegs, ip->back->oper1->preg))
                            remove_peep_entry(ip->back);
                    }
                }
            }
        }
    }
#endif
}
/*
 * cmp reg,0 -> and reg,reg
 */
void peep_cmp(OCODE* ip)
{
    OCODE* ip1;
    if (!equal_address(ip->oper1, ip->oper2))
    {
        ip1 = ip->back;
        if (ip1->opcode == op_mov)
        {
            if (ip1->oper1->mode == am_dreg)
            {
                if (ip->oper1->mode == am_dreg && ip->oper1->preg == ip1->oper1->preg)
                {
                    if (!live(ip->oper1->liveRegs, ip->oper1->preg))
                    {
                        if (ip1->oper2->mode == am_dreg ||
                            (ip1->oper2->mode != am_immed && (ip->oper2->mode == am_dreg || ip->oper2->mode == am_immed)))
                        {
                            if (ip->oper1->length == ip1->oper2->length)
                            {

                                ip1->oper2->liveRegs = ip->oper1->liveRegs;
                                ip->oper1 = ip1->oper2;
                                remove_peep_entry(ip1);
                                return;
                            }
                        }
                    }
                }
            }
        }
        ip1 = ip->fwd;
        if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed && ip->oper2->offset->type != Optimizer::se_add &&
            ip->oper2->offset->i == 0 && ip1->opcode != op_jc && ip1->opcode != op_jnc && ip1->opcode != op_jae &&
            ip1->opcode != op_jbe && ip1->opcode != op_ja && ip1->opcode != op_jb)
        {
            OCODE* ip2 = ip->back;
            ip->opcode = op_and;
            ip->oper2 = copy_addr(ip->oper1);
            if (ip2->opcode == op_mov && equal_address(ip->oper1, ip2->oper1) && ip2->oper2->mode != am_dreg &&
                ip2->oper2->mode != am_immed && (ip1->opcode == op_je || ip1->opcode == op_jne))
            {
                if (!live(ip->oper1->liveRegs, ip->oper1->preg))
                {
                    int m;
                    switch (ip->oper1->length)
                    {
                        case ISZ_UCHAR:
                        case -ISZ_UCHAR:
                        case ISZ_BOOLEAN:
                            m = 255;
                            break;
                        case ISZ_USHORT:
                        case -ISZ_USHORT:
                        case ISZ_U16:
                        case -ISZ_U16:
                        case ISZ_WCHAR:
                            m = 0xffff;
                            break;
                        default:
                            m = -1;
                            break;
                    }
                    ip2->opcode = op_test;
                    ip2->oper1 = ip2->oper2;
                    ip2->oper2 = aimmed(m);
                    remove_peep_entry(ip);
                    return;
                }
            }
        }
        if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed && ip->oper2->offset->i != 0 &&
            (ip1->opcode == op_je || ip1->opcode == op_jne))
        {
            OCODE* ip2 = ip->back;
            if (ip2->opcode == op_mov && equal_address(ip->oper1, ip2->oper1) && ip2->oper2->mode != am_immed)
            {
                if (!live(ip->oper1->liveRegs, ip->oper1->preg))
                {
                    ip->oper1 = ip2->oper2;
                    remove_peep_entry(ip2);
                    return;
                }
            }
        }
    }
}

/*
 * turn moves to self into nothingness
 * turn moves of seg to a (short) variable into one line of code
 * clean up move reg,const followed by move [mem],reg
 */
void peep_mov(OCODE* ip)
{
    OCODE* ip1 = ip->fwd;
    if (ip->back && ip->back->opcode == op_mov)
    {
        if (equal_address(ip->oper1, ip->back->oper2) && equal_address(ip->oper2, ip->back->oper1))
        {
            if (ip->oper2->mode == am_dreg && !live(ip->oper2->liveRegs, ip->oper2->preg))
                remove_peep_entry(ip->back);
            remove_peep_entry(ip);
            return;
        }
    }
    /*    OCODE *ip2;*/
    if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed)
    {
        if (isintconst(ip->oper2->offset) && (int)ip->oper2->offset->i == -1)
        {
            if (ip->oper2->length == ISZ_ULONG || ip->oper2->length == ISZ_UINT || ip->oper2->length == ISZ_U32)
            {
                ip->oper2->length = ISZ_UCHAR;
                ip->opcode = op_or; /* shorter form of move reg, -1 */
                return;
            }
        }

        if (isintconst(ip->oper2->offset) && ip->oper2->offset->i == 0)
        {
            switch (ip1->opcode)
            {
                case op_seta:
                case op_setnbe:
                case op_setae:
                case op_setnb:
                case op_setnc:
                case op_setb:
                case op_setc:
                case op_setnae:
                case op_setbe:
                case op_setna:
                case op_sete:
                case op_setz:
                case op_setg:
                case op_setnle:
                case op_setl:
                case op_setnge:
                case op_setge:
                case op_setnl:
                case op_setle:
                case op_setng:
                case op_setne:
                case op_setnz:
                case op_seto:
                case op_setno:
                case op_setp:
                case op_setnp:
                case op_setpe:
                case op_setpo:
                case op_sets:
                case op_setns:
                    break;
                default:
                    if (ip1->opcode == op_push && equal_address(ip1->oper1, ip->oper1) &&
                        !live(ip1->oper1->liveRegs, ip1->oper1->preg))
                    {
                        ip1->oper1 = ip->oper2;
                        ip1->oper1->length = ISZ_UCHAR;
                        remove_peep_entry(ip);
                    }
                    else
                    {
                        ip->oper2 = ip->oper1;
                        ip->opcode = op_xor;
                    }
                    break;
            }
        }
        return;
    }
    if (ip1 && ip->oper1->mode == am_dreg && ip->oper2->mode == am_seg && ip1->opcode == op_mov && ip1->oper2->mode == am_dreg &&
        ip1->oper2->length == 2)
    {

        ip->oper1 = ip1->oper1;
        remove_peep_entry(ip1);
        return;
    }
    if (ip1 && ip1->opcode == op_and && ip1->oper1->mode == am_dreg && equal_address(ip->oper1, ip1->oper1))
    {
        if (ip->oper1->length == ip1->oper1->length && ip1->oper2->mode == am_immed)
        {
            OCODE* ip2 = ip1->fwd;
            if (ip2->opcode == op_cmp && equal_address(ip2->oper1, ip1->oper1) && ip2->oper2->mode == am_immed)
            {
                if (ip2->oper2->offset->type == Optimizer::se_i && ip2->oper2->offset->i == 0)
                {
                    if (!live(ip2->oper1->liveRegs, ip2->oper1->preg))
                    {
                        ip->opcode = op_test;
                        ip->oper1 = ip->oper2;
                        ip->oper2 = ip1->oper2;
                        remove_peep_entry(ip1);
                        remove_peep_entry(ip2);
                        return;
                    }
                }
            }
        }
    }
    ip1 = ip->back;
    if (!ip1)
        return;
    if (ip1->opcode == op_mov && ip->oper1->mode != am_dreg && ip1->oper2->mode == am_immed && ip->oper2->mode == am_dreg &&
        ip1->oper1->mode == am_dreg && ip1->oper1->preg == ip->oper2->preg)
    {
        if (!live(ip->oper2->liveRegs, ip1->oper1->preg))
        {
            ip->oper2 = ip1->oper2;
            remove_peep_entry(ip1);
            return;
        }
    }
    if (ip->oper2->mode == am_dreg && ip1->back)
    {

        if (ip1->back->opcode == op_mov && equal_address(ip1->back->oper1, ip->oper2) && equal_address(ip1->back->oper2, ip->oper1))
        {
            switch (ip1->opcode)
            {
                case op_add:
                case op_sub:
                case op_adc:
                case op_sbb:
                case op_xor:
                case op_or:
                case op_and:
                    if (ip1->oper2->mode != am_dreg && ip1->oper2->mode != am_immed)
                    {
                        if (!live(ip->oper1->liveRegs, ip->oper2->preg))
                        {
                            ip->opcode = ip1->opcode;
                            ip1->opcode = op_mov;
                            remove_peep_entry(ip1->back);
                        }
                        break;
                    }
                case op_shl:
                case op_sal:
                case op_shr:
                case op_sar:
                case op_neg:
                case op_not:
                case op_inc:
                case op_dec:
                    if (equal_address(ip1->oper1, ip->oper2))
                    {
                        int l = ip->oper1->liveRegs;
                        ip1->oper1 = ip->oper1;
                        ip1->oper1->liveRegs = -1;
                        if (!live(l, ip->oper2->preg))
                            remove_peep_entry(ip);
                        else
                        {
                            ip->oper1 = ip1->back->oper1;
                            ip->oper2 = ip1->back->oper2;
                        }
                        remove_peep_entry(ip1->back);
                        return;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    while (ip1 && (e_op)ip1->opcode == op_line)
        ip1 = ip1->back;

    if (!ip1)
        return;
    if (ip1->opcode == op_mov)
    {
        if (ip->oper1->mode == am_dreg && ip1->oper2->mode == am_dreg)
        {
            if (equal_address(ip1->oper1, ip->oper2))
            {
                if (ip->oper1->preg == ip1->oper2->preg)
                {
                    remove_peep_entry(ip);
                }
                else
                {
                    *ip->oper2 = *ip1->oper2;
                }
            }
        }
    }
}
void peep_movzx(OCODE* ip)
{
    if (ip->back->opcode == op_mov)
        if (ip->oper2->mode == am_dreg)
        {
            if (ip->oper2->preg == ip->oper1->preg)
            {
                if (ip->back->oper1->mode == am_dreg)
                {
                    if (ip->back->oper1->preg == ip->oper2->preg)
                    {
                        if (ip->back->oper1->length == ip->oper2->length)
                        {
                            if (ip->back->oper2->mode == am_immed)
                            {
                                if (isintconst(ip->back->oper2->offset))
                                {
                                    ip->oper2 = ip->back->oper2;
                                    remove_peep_entry(ip->back);
                                    if (ip->opcode == op_movsx) /* sign extend this constant */
                                    {
                                        if (ip->oper2->length == 1)
                                        {
                                            ip->oper2->offset->i = (int)(char)ip->oper2->offset->i;
                                        }
                                        else /* two */
                                        {
                                            ip->oper2->offset->i = (int)(short)ip->oper2->offset->i;
                                        }
                                        ip->oper2->length = ISZ_UINT;
                                    }
                                    ip->opcode = op_mov;
                                }
                            }
                            else
                            {
                                ip->oper2 = ip->back->oper2;
                                remove_peep_entry(ip->back);
                            }
                        }
                    }
                }
            }
        }
}
void peep_movzx2(OCODE* ip)
{
    return;
    if (ip->opcode == op_movzx)
    {

        if (ip->oper2->mode == am_dreg)
        {
            if (ip->oper2->length == ISZ_UCHAR)
            {
                if (ip->oper2->preg == ip->oper1->preg && ip->oper1->preg <= EBX)
                {
                    if (ip->oper2->length == ISZ_UCHAR)
                        ip->oper2 = aimmed(0xff);
                    else
                        ip->oper2 = aimmed(0xffff);
                    ip->opcode = op_and;
                }
                else if (ip->oper1->preg <= EBX && (ip->oper2->preg & 3) != ip->oper1->preg)
                {
                    OCODE* c1 = beLocalAllocate<OCODE>();
                    c1->opcode = op_xor;
                    c1->oper1 = makedreg(ip->oper1->preg);
                    c1->oper2 = makedreg(ip->oper1->preg);
                    c1->back = ip->back;
                    c1->fwd = ip;
                    ip->back->fwd = c1;
                    ip->back = c1;
                    ip->opcode = op_mov;
                    ip->oper1->length = ip->oper2->length;
                    ip->noopt = true;
                }
            }
            else
            {
                if (ip->oper2->preg == ip->oper1->preg)
                {
                    if (ip->oper2->length == ISZ_UCHAR)
                        ip->oper2 = aimmed(0xff);
                    else
                        ip->oper2 = aimmed(0xffff);
                    ip->opcode = op_and;
                }
                else
                {
                    OCODE* c1 = beLocalAllocate<OCODE>();
                    c1->opcode = op_xor;
                    c1->oper1 = makedreg(ip->oper1->preg);
                    c1->oper2 = makedreg(ip->oper1->preg);
                    c1->back = ip->back;
                    c1->fwd = ip;
                    ip->back->fwd = c1;
                    ip->back = c1;
                    ip->opcode = op_mov;
                    ip->oper1->length = ip->oper2->length;
                    ip->noopt = true;
                }
            }
        }
        else
        {
            if (ip->oper1->preg <= EBX)
            {
                if (((ip->oper2->mode != am_indisp && ip->oper2->mode != am_indispscale) || ip->oper2->preg != ip->oper1->preg) &&
                    (ip->oper2->mode != am_indispscale || ip->oper2->sreg != ip->oper1->preg))
                {
                    OCODE* c1 = beLocalAllocate<OCODE>();
                    c1->opcode = op_xor;
                    c1->oper1 = makedreg(ip->oper1->preg);
                    c1->oper2 = makedreg(ip->oper1->preg);
                    c1->back = ip->back;
                    c1->fwd = ip;
                    ip->back->fwd = c1;
                    ip->back = c1;
                    ip->opcode = op_mov;
                    ip->oper1->length = ip->oper2->length;
                    ip->noopt = true;
                }
                else
                {
                    OCODE* c1 = beLocalAllocate<OCODE>();
                    c1->opcode = op_and;
                    c1->oper1 = makedreg(ip->oper1->preg);
                    if (ip->oper2->length == ISZ_UCHAR)
                        c1->oper2 = aimmed(0xff);
                    else
                        c1->oper2 = aimmed(0xffff);
                    c1->fwd = ip->fwd;
                    c1->back = ip;
                    ip->fwd->back = c1;
                    ip->fwd = c1;
                    ip->opcode = op_mov;
                    ip->oper1->length = ip->oper2->length;
                    ip->noopt = true;
                }
            }
        }
    }
}
void peep_tworeg(OCODE* ip)
{
    if (ip->oper1 && ip->oper2 && ip->oper1->mode == am_dreg && ip->oper2->mode == am_dreg && ip->oper1->preg != ip->oper2->preg)
    {
        if (ip->back && ip->back->opcode == op_mov)
        {
            if (equal_address(ip->back->oper1, ip->oper2))
            {
                if (!live(ip->oper1->liveRegs, ip->oper2->preg))
                {
                    if (!equal_address(ip->oper1, ip->back->oper2))
                        if (ip->back->oper2->mode == am_dreg)
                            ip->oper2->preg = ip->back->oper2->preg;
                        else
                            ip->oper2 = ip->back->oper2;
                    else
                    {
                        if (ip->opcode == op_mov)
                        {
                            remove_peep_entry(ip);
                        }
                        else
                            ip->oper2 = ip->back->oper2;
                    }
                    remove_peep_entry(ip->back);
                }
            }
        }
    }
}
void peep_mathdirect(OCODE* ip)
{
    if (ip->oper1 && ip->oper2 && ip->oper1->mode == am_dreg && ip->fwd->opcode == op_mov)
    {
        if (!live(ip->fwd->oper1->liveRegs, ip->oper1->preg))
        {
            if (equal_address(ip->fwd->oper1, ip->oper2))
            {
                if (equal_address(ip->oper1, ip->fwd->oper2))
                {
                    ip->fwd->opcode = ip->opcode;
                    remove_peep_entry(ip);
                }
            }
        }
    }
    if (ip->oper2 && ip->oper2->mode == am_immed)
    {
        if (ip->oper1->length == ISZ_USHORT || ip->oper1->length == -ISZ_USHORT || ip->oper1->length == ISZ_U16 ||
            ip->oper1->length == ISZ_WCHAR)
        {
            ip->oper2->offset->i &= 0xffff;
        }
        if (ip->oper1->length == ISZ_UCHAR || ip->oper1->length == -ISZ_UCHAR)
        {
            ip->oper2->offset->i &= 0xff;
        }
    }
}
void peep_xor(OCODE* ip)
{
    if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_dreg)
        if (ip->oper1->length == ISZ_USHORT || ip->oper1->length == ISZ_U16 || ip->oper1->length == ISZ_WCHAR)
            if (ip->oper1->preg == ip->oper2->preg)
                ip->oper1->length = ip->oper2->length = ISZ_UINT;
}
void peep_or(OCODE* ip)
{
    if (ip->oper2->mode == am_immed)
        if (ip->oper2->offset->i == 0)
            remove_peep_entry(ip);
}
/*
 * change some ands to registers to movzx
 * delete ands of full size
 * delete itself if followed by AND or OR with same args...
 */
void peep_and(OCODE* ip)
{
    OCODE* ip2 = ip->fwd;
    if (ip2->opcode == op_or || ip2->opcode == op_and)
    {
        if (equal_address(ip2->oper1, ip->oper1))
        {
            if (equal_address(ip2->oper2, ip->oper2))
            {
                remove_peep_entry(ip);
                return;
            }
            else if (equal_address(ip2->oper1, ip2->oper2))
            {
                remove_peep_entry(ip2);
                return;
            }
            else if (ip2->opcode == op_and && ip->oper2->mode == am_immed && ip2->oper2->mode == am_immed &&
                     isintconst(ip->oper2->offset) && isintconst(ip2->oper2->offset))
            {
                ip2->oper2 = aimmed(ip2->oper2->offset->i & ip->oper2->offset->i);
                remove_peep_entry(ip);
                return;
            }
        }
    }
    if (ip->opcode == op_and && ip->oper1->mode == am_dreg)
    {
        if (ip->oper2->mode == am_immed)
        {
            unsigned t = ip->oper2->offset->i;
            if (ip->oper1->length == ISZ_USHORT || ip->oper1->length == ISZ_U16 || ip->oper1->length == ISZ_WCHAR)
            {
                t &= 0xffff;
                if (t == 0xffff)
                {
                    remove_peep_entry(ip);
                    return;
                }
            }
            else if (ip->oper1->length == ISZ_UINT || ip->oper1->length == ISZ_ADDR || ip->oper1->length == ISZ_U32)
            {
                if (t == 0xffffffffU)
                {
                    remove_peep_entry(ip);
                    return;
                }
            }
            if (ip->back->opcode == op_shr)
            {
                if (ip->back->oper1->mode == am_dreg && ip->back->oper1->preg == ip->oper1->preg)
                {
                    if (ip->back->oper2->mode == am_immed)
                    {
                        int t1 = ip->back->oper2->offset->i;
                        if ((t == 0xff && t1 == 24) || (t == 0xffff && t1 == 16) || (t == 0xffffff && t1 == 8))
                        {
                            remove_peep_entry(ip);
                            return;
                        }
                    }
                }
            }
            if (ip->back->opcode == op_shl || ip->back->opcode == op_sal)
            {
                if (ip->back->oper1->mode == am_dreg && ip->back->oper1->preg == ip->oper1->preg)
                {
                    if (ip->back->oper2->mode == am_immed)
                    {
                        int t1 = ip->back->oper2->offset->i;
                        if ((t == 0xff000000U && t1 == 24) || (t == 0xffff0000U && t1 == 16) || (t == 0xffffff00U && t1 == 8))
                        {
                            remove_peep_entry(ip);
                            return;
                        }
                    }
                }
            }
        }
    }
    else if (ip->oper2->mode == am_immed)
    {
        unsigned t = ip->oper2->offset->i;
        if (t == 0xffffffffU)
        {
            remove_peep_entry(ip);
        }
    }
}
/* take lea instructions followed by an add to the register and optimize
 * can be generated by structure references to stacked variables, for example
 */
void peep_lea(OCODE* ip)
{
    OCODE* ip1 = ip->fwd;
    if (ip1->opcode == op_add && ip1->oper1->mode == am_dreg)
    {
        if (ip1->oper1->preg == ip->oper1->preg)
        {
            if (ip1->oper2->mode == am_immed)
            {
                ip->oper2->offset = simpleExpressionNode(Optimizer::se_add, ip->oper2->offset, ip1->oper2->offset);
                remove_peep_entry(ip1);
            }
        }
    }
}
/*
 * get rid of a BTR immediately preceding a BTS if args match
 */
void peep_btr(OCODE* ip)
{
    if (ip->fwd->opcode == op_bts && equal_address(ip->oper1, ip->fwd->oper1) && equal_address(ip->oper2, ip->fwd->oper2))
    {
        remove_peep_entry(ip);
    }
}
void peep_sar(OCODE* ip)
{
    if (ip->oper1->mode == am_dreg && ip->oper1->preg == EDX && (ip->oper1->length == ISZ_UINT || ip->oper1->length == ISZ_U32))
        if (ip->oper2->mode == am_immed && isintconst(ip->oper2->offset) && ip->oper2->offset->i == 0x1f)
        {
            OCODE* ip1 = ip->back;
            if (ip1->opcode == op_mov)
                if (ip1->oper1->mode == am_dreg && ip1->oper2->mode == am_dreg)
                    if (ip1->oper1->preg == EDX && ip1->oper2->preg == EAX)
                    {
                        if ((ip1->oper1->length == ISZ_UINT || ip1->oper1->length == ISZ_U32) &&
                            (ip1->oper2->length == ISZ_UINT || ip1->oper2->length == ISZ_U32))
                        {
                            if (ip->fwd->opcode == op_div || ip->fwd->opcode == op_idiv)
                            {
                                remove_peep_entry(ip);
                                ip1->opcode = op_cdq;
                                ip1->oper1 = ip1->oper2 = nullptr;
                            }
                        }
                    }
        }
}
void peep_mul(OCODE* ip)
{
    if (ip->oper1->mode == am_dreg)
        if (ip->oper2 && ip->oper2->mode == am_immed && isintconst(ip->oper2->offset))
        {
            if (ip->oper2->offset->i == 0)
            {
                ip->opcode = op_xor;
                ip->oper2 = ip->oper1;
            }
            else if (ip->oper2->offset->i == 1)
            {
                remove_peep_entry(ip);
            }
        }
}
/*-------------------------------------------------------------------------*/

int novalue(OCODE* ip) { return (e_op)ip->fwd->opcode == op_void; }

/*-------------------------------------------------------------------------*/

int equal_address(AMODE* ap1, AMODE* ap2)
{
    if (ap1->mode != ap2->mode)
        return (false);
    if (ap1->length != ap2->length && ap1->length != -ap2->length)
    {
        int n1 = ap1->length < 0 ? -ap1->length : ap1->length;
        int n2 = ap2->length < 0 ? -ap2->length : ap2->length;
        if (n1 == ISZ_ULONG || n1 == ISZ_ADDR || n1 == ISZ_UINT || n1 == ISZ_U32)
            n1 = ISZ_UINT;
        if (n2 == ISZ_ULONG || n2 == ISZ_ADDR || n2 == ISZ_UINT || n2 == ISZ_U32)
            n2 = ISZ_UINT;
        if (n1 != n2)
            return false;
    }
    switch (ap1->mode)
    {
        case am_axdx:
            return true;
        case am_immed:
            return equalnode(ap1->offset, ap2->offset);
        case am_indispscale:
            if (ap1->scale != ap2->scale || ap1->sreg != ap2->sreg)
                return (false);
            if (ap1->sreg != ap2->sreg)
                return false;
        case am_indisp:
            if (ap1->offset)
                if (ap2->offset)
                {
                    if (!equalnode(ap1->offset, ap2->offset))
                        return false;
                }
                else
                    return (false);
            else if (ap2->offset)
                return (false);
        case am_dreg:
            if (ap1->preg != ap2->preg)
                return (false);
            return true;
        case am_freg:
            if (ap1->preg != ap2->preg)
                return (false);
            return true;
        case am_xmmreg:
            if (ap1->preg != ap2->preg)
                return (false);
            return true;
        case am_mmreg:
            if (ap1->preg != ap2->preg)
                return (false);
            return true;

        case am_direct:
            return equalnode(ap1->offset, ap2->offset);
        default:
            break;
    }
    return (false);
}

/*
 * this one extends two-byte ops to four bytes in order to avoid
 * OPSIZ prefixes
 */
void peep_prefixes(OCODE* ip)
{
    if (!ip->oper1)
        return;
    if (ip->opcode == op_mov || ip->opcode == op_and || ip->opcode == op_sub || ip->opcode == op_mul || ip->opcode == op_neg ||
        ip->opcode == op_not)
        if (ip->oper1->mode == am_dreg && (!ip->oper2 || ip->oper2->mode == am_dreg))
        {
            // special case and reg1,reg1 because it is used for testing for nonzero
            if (ip->opcode == op_and && ip->oper2)
                if (ip->oper1->preg == ip->oper2->preg)
                    return;
            if (ip->oper1 && (ip->oper1->length == ISZ_USHORT || ip->oper1->length == ISZ_U16 || ip->oper1->length == ISZ_WCHAR))
                ip->oper1->length = ISZ_UINT;
            if (ip->oper2 && (ip->oper2->length == ISZ_USHORT || ip->oper2->length == ISZ_U16 || ip->oper2->length == ISZ_WCHAR))
                ip->oper2->length = ISZ_UINT;
        }
}
OCODE* peep_div(OCODE* ip)
{
#if 0  // see comments below
        if (ip->oper1->mode == am_dreg)
        {
            OCODE* ip2 = ip->back;
            while (ip2 && ip2->opcode != op_label && ip2->opcode < op_ja &&
                   ip2->opcode > op_jmp)  // It appears that this loop is NEVER run op_ja is less than op_jmp
            {
                if (ip2->opcode == op_mov && ip2->oper1->mode == am_dreg && ip2->oper1->preg == ip->oper1->preg)
                    break;
                ip2 = ip2->back;
            }
            if (ip2 && ip2->opcode != op_label && ip2->opcode < op_ja &&
                ip2->opcode > op_jmp)  // this if statement is never travelled, same reason above
            {
                AMODE* oper = ip2->oper2;
                if (oper->mode != am_indisp || oper->preg != EAX && oper->preg != EDX && oper->preg != ESP)
                    if (oper->mode != am_indispscale ||
                        oper->preg != EAX && oper->preg != EDX && oper->preg != ESP && oper->sreg != EAX && oper->sreg != EDX)
                        if (oper->mode != am_dreg || oper->preg != EAX && oper->preg != EDX && oper->preg != ESP)
                        {
                            if (oper->mode == am_immed)
                            {
                                oper = make_muldivval(oper);
                            }
                            if (!live(ip->oper1->liveRegs, ip2->oper1->preg))
                                remove_peep_entry(ip2);
                            oper->liveRegs = ip->oper1->liveRegs;
                            ip->oper1 = oper;
                        }
            }
        }
#endif
    return ip;
}
/*
 * treat move reg, value followed by push reg when value is an int or constant
 */
void peep_push(OCODE* ip)
{
    if (ip->oper1->mode == am_dreg)
    {
        OCODE* ip1 = ip->back;
        if (ip1)
        {
            if (ip1->opcode == op_mov && equal_address(ip1->oper1, ip->oper1))
            {
                if (!live(ip1->oper1->liveRegs, ip1->oper1->preg))
                {
                    if (ip->oper1->length == ip1->oper2->length)
                    {
                        ip->oper1 = ip1->oper2;
                        remove_peep_entry(ip1);
                        return;
                    }
                    else if (ip1->oper2->mode == am_immed)
                    {
                        ip->oper1 = ip1->oper2;
                        ip->oper1->length = ISZ_UINT;
                        remove_peep_entry(ip1);
                        return;
                    }
                }
            }
            if (ip1->opcode == op_xor && equal_address(ip1->oper1, ip1->oper2) && equal_address(ip1->oper1, ip->oper1))
            {
                if (!live(ip1->oper1->liveRegs, ip1->oper1->preg))
                {
                    ip->oper1 = aimmed(0);
                    ip->oper1->length = ISZ_UCHAR;
                    remove_peep_entry(ip1);
                    return;
                }
            }
        }
    }
}
void peep_call(OCODE* ip)
{
    if (ip->oper1->mode == am_dreg)
    {
        OCODE* ip1 = ip->back;
        if (ip1->opcode == op_mov && equal_address(ip->oper1, ip1->oper1))
        {
            if (!live(ip->oper1->liveRegs, ip->oper1->preg))
            {
                ip->oper1 = ip1->oper2;
                remove_peep_entry(ip1);
            }
        }
    }
}
void peep_fmovs(OCODE* ip)
{
    if (ip->oper1 && ip->oper2)  // might be plain strin movsd...
        if (ip->oper1->mode == am_xmmreg && ip->oper2->mode == am_xmmreg)
            if (ip->oper1->preg == ip->oper2->preg)
            {
                remove_peep_entry(ip);
                return;
            }
}
void peep_fld(OCODE* ip)
{
    // don't try to optimize complex returns
    if (ip->fwd->opcode == op_fld || ip->back->opcode == op_fld)
        return;
    int n = ip->oper1->length;
    ip->oper1->length = 0;
    if (ip->back->opcode == op_movsd || ip->back->opcode == op_movss)
    {
        if (equal_address(ip->back->oper1, ip->oper1))
        {
            if (ip->back->oper2->mode == am_xmmreg)
            {
                if (ip->back->back->opcode == ip->back->opcode)
                {
                    if (equal_address(ip->back->back->oper1, ip->back->oper2))
                    {
                        if (ip->back->back->oper2->mode != am_xmmreg)
                        {
                            ip->oper1 = ip->back->back->oper2;
                            remove_peep_entry(ip->back->back);
                            remove_peep_entry(ip->back);
                        }
                    }
                }
            }
        }
    }
    ip->oper1->length = n;
    if (ip->back->opcode == op_fstp)
    {
        if (equal_address(ip->back->oper1, ip->oper1))
        {
            remove_peep_entry(ip->back);
            remove_peep_entry(ip);
        }
    }
}
static int matches_set(OCODE* ip)
{
    switch (ip->opcode)
    {
                case op_seta:
                case op_setnbe:
                case op_setae:
                case op_setnb:
                case op_setnc:
                case op_setb:
                case op_setc:
                case op_setnae:
                case op_setbe:
                case op_setna:
                case op_sete:
                case op_setz:
                case op_setg:
                case op_setnle:
                case op_setl:
                case op_setnge:
                case op_setge:
                case op_setnl:
                case op_setle:
                case op_setng:
                case op_setne:
                case op_setnz:
                case op_seto:
                case op_setno:
                case op_setp:
                case op_setnp:
                case op_setpe:
                case op_setpo:
                case op_sets:
                case op_setns:
                    break;
                default: 
                    return -1;
    }
    if (ip->fwd->opcode == op_and && ip->fwd->oper1->mode == am_dreg && ip->fwd-> oper2->mode == am_immed && ip->oper1->mode == am_dreg)
    {
         if (ip->oper1->preg == ip->fwd->oper1->preg && ip->fwd->oper2->offset->type == Optimizer::se_i && ip->fwd->oper2->offset->i == 1)
         {
             return ip->oper1->preg;
         }
    }
    return -1;
}
// we can elide a setne reg followed by an and reg,1 if there was a previous set & and
//
void peep_setne(OCODE *ip)
{
    int final = matches_set(ip);
    if (final >= 0)
    {
        OCODE *ip1 = ip->back;
        while (ip1->opcode <= (e_opcode)op_blockend) ip1 = ip1->back;
        ip1 = ip1->back;
        int initial = matches_set(ip1);
        if (initial >= 0)
        {
            remove_peep_entry(ip->fwd);
            remove_peep_entry(ip);
            ip1->oper1->preg = final;
            ip1->fwd->oper1->preg = final;
        }
    }
}
void insert_peep_entry(OCODE* after, enum e_opcode opcode, int size, AMODE* ap1, AMODE* ap2)
{
    OCODE* a = beLocalAllocate<OCODE>();
    a->opcode = opcode;
    a->oper1 = ap1;
    a->oper2 = ap2;
    a->size = size;
    a->back = after->back->back;
    a->fwd = after;
    after->back->fwd = a;
    after->back = a;
}
/*
 *  This function removes the specified peep list entry from the
 * peep list. It insures ther is no nullptr pointer to cause exceptions.
 */
void remove_peep_entry(OCODE* ip)
{
    if (ip == peep_head)
    {
        peep_head = ip->fwd;
        peep_head->back = nullptr;
    }
    else
    {
        if (ip->back)
            ip->back->fwd = ip->fwd;
        if (ip->fwd)
            ip->fwd->back = ip->back;
    }
}

/*-------------------------------------------------------------------------*/

void oa_peep(void)
/*
 *      peephole optimizer. This routine calls the instruction
 *      specific optimization routines above for each instruction
 *      in the peep list.
 */
{
    OCODE* ip;
    if (!Optimizer::cparams.prm_bepeep)
        return;
    for (ip = peep_head; ip; ip = ip->fwd)
    {
        if (!ip->noopt)
        {
            switch (ip->opcode)
            {
                case op_lock:
                    ip = ip->fwd;  // skip next instruction
                    break;
                case op_mul:
                case op_imul:
                    peep_mul(ip);
                    break;
                case op_sar:
                    peep_sar(ip);
                    break;
                case op_lea:
                    peep_lea(ip);
                    break;
                case op_call:
                    peep_call(ip);
                    break;
                case op_push:
                    peep_push(ip);
                    break;
                case op_add:
                    peep_tworeg(ip);
                    peep_mathdirect(ip);
                    peep_add(ip);
                    break;
                case op_sub:
                    peep_tworeg(ip);
                    peep_sub(ip);
                    break;
                case op_and:
                    peep_tworeg(ip);
                    peep_mathdirect(ip);
                    peep_and(ip);
                    break;
                case op_xor:
                    peep_xor(ip);
                case op_or:
                    peep_tworeg(ip);
                    peep_mathdirect(ip);
                    peep_or(ip);
                    break;
                case op_btr:
                    peep_btr(ip);
                    break;
                case op_cmp:
                    peep_tworeg(ip);
                    peep_mathdirect(ip);
                    peep_cmpzx(ip);
                    peep_cmp(ip);
                    break;
                case op_movsx:
                    peep_movzx(ip);
                    break;
                case op_movzx:
                    peep_movzx(ip);
                    peep_movzx2(ip);
                    break;
                case op_mov:
                    if (equal_address(ip->oper1, ip->oper2))
                    {
                        remove_peep_entry(ip);
                        break;
                    }
                    else
                    {
                        while (ip->fwd && ip->fwd->opcode == op_mov && ip->oper1->mode == am_indisp && ip->oper1->preg == EBP &&
                               ip->oper2->mode == am_dreg && equal_address(ip->oper1, ip->fwd->oper1) &&
                               equal_address(ip->oper2, ip->fwd->oper2))
                            remove_peep_entry(ip->fwd);
                        peep_tworeg(ip);
                        peep_mov(ip);
                    }
                    break;
                case op_test:
                    peep_tworeg(ip);
                    ip = peep_test(ip);
                    break;
                case op_neg:
                    ip = peep_neg(ip);
                    break;
                case op_cdq:
                    if (ip->fwd->opcode == op_cdq)
                        remove_peep_entry(ip->fwd);
                    break;
                case op_div:
                case op_idiv:
                    ip = peep_div(ip);
                    break;
                case op_movss:
                case op_movsd:
                    peep_fmovs(ip);
                    break;
                case op_fld:
                    peep_fld(ip);
                    break;
                case op_setne:
                    peep_setne(ip);
                    break;
                default:
                    break;
            }
        }
    }
    for (ip = peep_head; ip; ip = ip->fwd)
    {
        if (!ip->noopt)
        {
            peep_prefixes(ip);
        }
    }
}
}  // namespace occx86