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
#include <stdio.h>
#include <string.h>
#include "be.h"

/* peephole optimizations
 * well not exactly
 * the compiler actually generated risc-like instruction sequences
 * and some of the things in this module re-cisc it :)
 */
extern COMPILER_PARAMS cparams;
extern int regmap[REG_MAX][2];

extern int prm_assembler;
extern int prm_asmfile;
extern int prm_bepeep;
extern int prm_useesp;
extern int usingEsp;
extern int pushlevel;
extern int funcstackheight;

#define live(mask, reg) (mask & (1 << reg))

OCODE *peep_head = 0, *peep_tail = 0, *peep_insert;
void insert_peep_entry(OCODE *after, enum e_op opcode, int size, AMODE *ap1, AMODE *ap2);

void o_peepini(void)
{
    peep_head = peep_tail = 0;
}
void oa_adjust_codelab(void *select, int offset)
{
    OCODE *peep = (OCODE *)select;
    if (peep->oper1 && peep->oper1->offset && peep->oper1->offset->type == en_labcon)
        if (peep->oper1->offset->v.i < 0)
            peep->oper1->offset->v.i += offset;
    if (peep->oper2 && peep->oper2->offset && peep->oper2->offset->type == en_labcon)
        if (peep->oper2->offset->v.i < 0)
            peep->oper2->offset->v.i += offset;
    if (peep->oper3 && peep->oper3->offset && peep->oper3->offset->type == en_labcon)
        if (peep->oper3->offset->v.i < 0)
            peep->oper3->offset->v.i += offset;
}

AMODE *makedreg(int r)
/*
 *      make an address reference to a data register.
 */
{
    AMODE *ap;
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_dreg;
    ap->preg = r;
    ap->length = ISZ_UINT;
    ap->liveRegs = ~0;
    return ap;
} 

AMODE *makefreg(int r)
/*
 *      make an address reference to a data register.
 */
{
    AMODE *ap;
    ap = beLocalAlloc(sizeof(AMODE));
    ap->mode = am_freg;
    ap->preg = r;
    ap->length = ISZ_LDOUBLE;
    return ap;
}
/*-------------------------------------------------------------------------*/

AMODE *copy_addr(AMODE *ap)
/*
 *      copy an address mode structure (these things dont last).
 */
{
    AMODE *newap;
    if (ap == 0)
        return 0;
    newap = beLocalAlloc(sizeof(AMODE));
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
static void fixlen(AMODE *ap)
{
    if (ap)
    {
        /* the output routines require positive length field */
        if (ap->length < 0)
            ap->length =  - ap->length;
        if (prm_assembler == pa_nasm && ap->mode == am_immed && isintconst(ap->offset)
            )
        {
            /* for the x86, assume initially that integer constants are unsized
             * But the x86 allows signed byte constants on most math ops
             * so, since NASM won't do this automatically we must tell it the
             * sizing and we do this by setting the constant size to 1
             * and the output routines will know what to do with this info
             */
            if ((int)ap->offset->v.i <= CHAR_MAX && (int)ap->offset->v.i >= CHAR_MIN)
                ap->length = ISZ_UCHAR;
            else
                ap->length = ISZ_NONE;
        }
        if (prm_useesp && (ap->mode == am_indisp || ap->mode == am_indispscale) && ap->offset)
        {
            if (usingEsp)
            {
                // this is to adjust the stack level to the current depth
                if (ap->preg == ESP && !ap->keepesp)
                    ap->offset = exprNode(en_add, ap->offset, intNode(en_c_i, pushlevel + funcstackheight));
            }
            else
            {
                // this is to adjust for the frame pointer being smaller
                if (ap->preg == EBP)
                {
                    int resolved = 0;
                    int n = resolveoffset(NULL, ap->offset, &resolved);
                    if (resolved)
                    {
                        diag("fixlen: resolveoffset failed");
                    }
                    else
                    {
                        if (n > 0)
                        {
                            ap->offset = exprNode(en_add, ap->offset, intNode(en_c_i, 4));
                        }
                    }
                }
            }
        }
    }
}

/*-------------------------------------------------------------------------*/

OCODE *gen_code(int op, AMODE *ap1, AMODE *ap2)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE *new;
    new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op;
    new->oper1 = copy_addr(ap1);
    new->oper2 = copy_addr(ap2);
    if (op == op_lea)
        new->oper2->seg = 0;
    fixlen(new->oper1);
    fixlen(new->oper2);
    add_peep(new);
    return new;
}

/*-------------------------------------------------------------------------*/

OCODE *gen_code3(int op, AMODE *ap1, AMODE *ap2, AMODE *ap3)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE *new;
    new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op;
    new->oper1 = copy_addr(ap1);
    new->oper2 = copy_addr(ap2);
    new->oper3 = copy_addr(ap3);
    fixlen(new->oper1);
    fixlen(new->oper2);
    fixlen(new->oper3);
    add_peep(new);
    return new;
}

/*-------------------------------------------------------------------------*/

OCODE *gen_codes(int op, int len, AMODE *ap1, AMODE *ap2)
{
    OCODE *new = gen_code(op, ap1, ap2);
    if (len < 0)
        len =  - len;
    if (new->oper1)
        new->oper1->length = len;
    if (new->oper2 && ((len != ISZ_UINT && len != ISZ_U32) || !new->oper2->offset || new->oper2->mode != am_immed))
        new->oper2->length = len;
    return new;
}

/*-------------------------------------------------------------------------*/

void gen_coden(int op, int len, AMODE *ap1, AMODE *ap2)
{
    OCODE *new = gen_codes(op, len, ap1, ap2);
    new->noopt = TRUE;
}

/*-------------------------------------------------------------------------*/

void gen_codefs(int op, int len, AMODE *ap1, AMODE *ap2)
{
    if (ap1 && ap2 && ap1->mode == am_freg && ap2->mode == am_freg)
        len = ISZ_LDOUBLE;
    gen_codes(op, len, ap1, ap2);
}

/*-------------------------------------------------------------------------*/

void gen_codef(int op, AMODE *ap1, AMODE *ap2)
{
    if (ap1)
        gen_codefs(op, ap1->length, ap1, ap2);
    else
        gen_codefs(op, 0, ap1, ap2);
}

/*-------------------------------------------------------------------------*/

void gen_codes2(int op, int len, AMODE *ap1, AMODE *ap2)
{
    OCODE *new = gen_code(op, ap1, ap2);
    if (len < 0)
        len =  - len;
    if (new->oper2)
        new->oper2->length = len;
}

/*-------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------*/

void gen_codelab(SYMBOL *lab)
/*
 *      generate a code sequence into the peep list.
 */
{
    OCODE *new;
    new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_funclabel;
    new->oper1 = (AMODE *)lab;
    new->oper2 = 0;
    add_peep(new);
}

/*-------------------------------------------------------------------------*/

void gen_branch(int op, int label)
{
    AMODE *ap1 = make_label(label);
    gen_code(op, ap1, 0);
}

/*-------------------------------------------------------------------------*/

void gen_comment(char *txt)
{
    OCODE *new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_comment;
    new->oper2 = 0;
    new->oper1 = (AMODE*)txt;
    add_peep(new);
}

/*-------------------------------------------------------------------------*/


void add_peep(OCODE *new)
/*
 *      add the instruction pointed to by new to the peep list.
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

/*-------------------------------------------------------------------------*/

void oa_gen_label(int labno)
/*
 *      add a compiler generated label to the peep list.
 */
{
    OCODE *new;
    new = beLocalAlloc(sizeof(OCODE));
    new->opcode = op_label;
    new->oper1 = (AMODE*)labno;
    add_peep(new);
}

/*-------------------------------------------------------------------------*/

void flush_peep(SYMBOL *funcsp, QUAD *list)
/*
 *      output all code and labels in the peep list.
 */
{
    (void)funcsp;
    (void) list;
    oa_peep(); /* do the peephole optimizations */
    oa_peep();
    if (peep_head)
        outcode_gen(peep_head);
    if (cparams.prm_asmfile)
    {
        while (peep_head != 0)
        {
            switch (peep_head->opcode)
            {
                case op_label:
                    oa_put_label((int)peep_head->oper1);
                    break;
                case op_funclabel:
                    oa_gen_strlab((SYMBOL *)peep_head->oper1);
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

void peep_add(OCODE *ip)
/*
 * Turn add,1 into inc
 * turn add,0 into nothing (can be genned by the new structure mechanism)
 */
{
    if (ip->oper2->mode != am_immed)
        return ;

    if (ip->fwd->opcode != op_adc)
    {
    /* change to inc */
        if (isintconst(ip->oper2->offset))
        {
            if (ip->oper2->offset->v.i == 1)
            {
                ip->opcode = op_inc;
                ip->oper2 = 0;
                return;
            } else if (ip->oper2->offset->v.i == 0)
            {
                remove_peep_entry(ip);
                return;
            }
        }
        if (ip->oper1->mode == am_dreg)
        {
            OCODE *ip1 = ip->back;
            if (ip1 && (ip1->opcode == op_mov || ip1->opcode == op_add)
                && ip1->oper1->mode == am_dreg && ip1->oper1->preg == ip->oper1->preg
                && ip1->oper2->mode == am_immed && (!varsp(ip->oper2->offset) ||
                                                    !varsp(ip1->oper2->offset)))
            {
                if (!isintconst(ip->oper2->offset))
                    ip1->oper2->length = ISZ_UINT;
                if (!isintconst(ip1->oper2->offset))
                    ip->oper2->length = ISZ_UINT;
                ip->oper2->offset = exprNode(en_add, ip->oper2->offset, ip1->oper2->offset);
                ip->opcode = ip1->opcode;
                remove_peep_entry(ip1);
            }
            else
            {
                ip1 =  ip->fwd;
                if ((ip1->opcode == op_add)
                    && ip1->oper1->mode == am_dreg && ip1->oper1->preg == ip->oper1->preg
                    && ip1->oper2->mode == am_immed && (!varsp(ip->oper2->offset) ||
                                                        !varsp(ip1->oper2->offset)))
                {
                    if (!isintconst(ip->oper2->offset))
                        ip1->oper2->length = ISZ_UINT;
                    if (!isintconst(ip1->oper2->offset))
                        ip->oper2->length = ISZ_UINT;
                    ip->oper2->offset = exprNode(en_add, ip->oper2->offset, ip1->oper2->offset);
                    ip->opcode = ip1->opcode;
                    remove_peep_entry(ip1);
                }
            }
        }
    }
    return ;
}

/*
 * Turn sub,1 into dec
 */
void peep_sub(OCODE *ip)
{
    if (ip->oper2->mode != am_immed || !isintconst(ip->oper2->offset))
        return ;
    if (ip->fwd->opcode != op_sbb)
    {
        if (ip->oper2->offset->v.i == 1)
        {
            ip->opcode = op_dec;
            ip->oper2 = 0;
        }
        else if (ip->oper2->offset->v.i == 0)
        {
            remove_peep_entry(ip);
        }
        else if (ip->oper1->mode == am_dreg && ip->oper1->preg == ESP)
        {
            if ((int)ip->oper2->offset->v.i < 0)
            {
                ip->oper2->offset->v.i = - ip->oper2->offset->v.i;
                ip->opcode = op_add;
            }
        }
    }
    return ;
}

/*-------------------------------------------------------------------------*/

OCODE *peep_test(OCODE *ip)
{
    OCODE *ip2 ;

    if (ip->oper1->mode == am_dreg && (ip->oper1->length == ISZ_UINT || ip->oper1->length == ISZ_U32)
        && ip->oper2->mode == am_immed && isintconst(ip->oper2->offset) 
        && (ip->oper2->offset->v.i & 0xffffffffU) == 0xffffffffU)
        
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
        for (ip2 = ip->back; ip2 && (ip2->opcode == op_line ||
            ip2->opcode == op_void || ip2->opcode == op_blockstart || 
            ip2->opcode == op_blockend || ip2->opcode == op_varstart);
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
        if ((ip2->opcode == op_adc || ip2->opcode == op_add || ip2->opcode ==
            op_and || ip2->opcode == op_dec || ip2->opcode == op_inc || ip2
            ->opcode == op_or || ip2->opcode == op_sbb || ip2->opcode == op_sub
            || ip2->opcode == op_xor) && equal_address(ip2->oper1, ip->oper1))
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
OCODE *peep_neg(OCODE *ip)
{
    if (ip->oper1->mode == am_dreg)
    {
        OCODE *ipf = ip->fwd;
        while ((ipf->opcode >= op_aaa && ipf->opcode < op_jecxz) || ipf->opcode >= op_lahf)
        {
            if (ipf->opcode == op_add)
            {
                if (ipf->oper2->mode == am_dreg && ipf->oper2->preg == ip->oper1->preg)
                {
                    remove_peep_entry(ip);
                    ipf->opcode = op_sub;
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
void peep_cmpzx(OCODE *ip)
{
    (void)ip;
    return;
#ifdef XXXXX
    if (ip->back->opcode == op_movzx && ip->back->back->opcode == op_movzx

        || ip->back->opcode == op_movsx && ip->back->back->opcode == op_movsx)
    {
        if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_dreg)
        {
            if (ip->oper1->preg == ip->back->back->oper1->preg &&
                ip->oper2->preg == ip->back->oper1->preg)
            {
                if (ip->oper1->length == ip->back->back->oper1->length &&
                    ip->oper1->length == ip->back->oper1->length)
                {
                    if (ip->back->oper2->length == ip->back->back->oper2->length)
                    {
                        if (ip->back->oper2->mode == am_dreg || ip->back->back->oper2->mode == am_dreg)
                        {
                            int live = ip->oper1->liveRegs;
                            if (ip->back->opcode == op_movzx)
                            {
                                OCODE *ip2;
                                ip2 = ip->fwd;
                                while (ip2 && ip2->opcode < op_aaa && ip2->opcode != op_label)
                                    ip2 = ip2->fwd;
                                if (ip2->opcode == op_mov && ip2->oper1->mode == am_dreg &&
                                    isintconst(ip2->oper2->offset) && ip2->oper2->offset->v.i ==0)
                                {
                                    if (ip2->fwd->oper1 && ip2->fwd->oper1->mode == am_dreg &&
                                        ip2->fwd->oper1->preg == ip2->oper1->preg)
                                        switch(ip2->fwd->opcode)
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
                                else switch(ip2->opcode)
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
                int n = ip->oper2->offset->v.i & 0xffffff80U;
                int n1 = n & 0xffffff00U;
                bContinue = (ip->back->opcode == op_movsx && (n == 0 || n == 0xffffff80U)
                    || ip->back->opcode == op_movzx && n1 == 0);
            }
            else
            {
                int n = ip->oper2->offset->v.i & 0xffff8000U;
                int n1 = n & 0xffff0000U;
                bContinue = (ip->back->opcode == op_movsx && (n == 0 || n == 0xffff8000U)
                    || ip->back->opcode == op_movzx && n1 == 0);
            }
            if (bContinue)
            {
                if (ip->oper1->mode == am_dreg && ip->back->oper1->mode == am_dreg
                    && ip->oper1->preg == ip->back->oper1->preg)
                {
                    if (!live(ip->oper1->liveRegs, ip->oper1->preg))
                    {
                        ip->oper1 = ip->back->oper2;
                        ip->oper2->length = ip->oper1->length;
                        if (ip->back->opcode == op_movzx)
                        {
                            OCODE *ip2;
                            ip2 = ip->fwd;
                            while (ip2 && ip2->opcode < op_aaa && ip2->opcode != op_label)
                                ip2 = ip2->fwd;
                            if (ip2->opcode == op_mov && ip2->oper1->mode == am_dreg &&
                                isintconst(ip2->oper2->offset) && ip2->oper2->offset->v.i ==0)
                            {
                                if (ip2->fwd->oper1 && ip2->fwd->oper1->mode == am_dreg &&
                                    ip2->fwd->oper1->preg == ip2->oper1->preg)
                                    switch(ip2->fwd->opcode)
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
                            else switch(ip2->opcode)
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
void peep_cmp(OCODE *ip)
{
    OCODE *ip1;
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
                        if (ip1->oper2->mode == am_dreg || (ip1->oper2->mode != am_immed && (ip->oper2->mode == am_dreg || ip->oper2->mode == am_immed)))
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
        if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed && ip->oper2
            ->offset->v.i == 0 && ip1->opcode != op_jc && ip1->opcode != op_jnc 
            && ip1->opcode != op_jae && ip1->opcode != op_jbe 
            && ip1->opcode != op_ja && ip1->opcode != op_jb)
        {
            OCODE *ip2 = ip->back;
            ip->opcode = op_and;
            ip->oper2 = copy_addr(ip->oper1);
            if (ip2->opcode == op_mov && equal_address(ip->oper1, ip2->oper1) && ip2->oper2->mode != am_dreg
                && ip2->oper2->mode != am_immed
                && (ip1->opcode == op_je || ip1->opcode == op_jne) )
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
        if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_immed && ip->oper2
            ->offset->v.i != 0 && (ip1->opcode == op_je || ip1->opcode == op_jne)) 
        {
            OCODE *ip2 = ip->back;
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
void peep_mov(OCODE *ip)
{
    OCODE *ip1 = ip->fwd;
    if (ip->back && ip->back->opcode == op_mov)
    {
        if (equal_address(ip->oper1, ip->back->oper2) &&
            equal_address(ip->oper2, ip->back->oper1))
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
        if (isintconst(ip->oper2->offset) && (int)ip->oper2->offset->v.i == -1)
        {
            if (ip->oper2->length == ISZ_ULONG || ip->oper2->length == ISZ_UINT || ip->oper2->length == ISZ_U32)
            {
                ip->oper2->length = ISZ_UCHAR;
                ip->opcode = op_or; /* shorter form of move reg, -1 */
                return;
            }
        }

        if (isintconst(ip->oper2->offset) && ip->oper2->offset->v.i == 0)
        {
            switch(ip1->opcode)
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
                    if (ip1->opcode == op_push && equal_address(ip1->oper1, ip->oper1)
                        && !live(ip1->oper1->liveRegs, ip1->oper1->preg))
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
    if (ip1 && ip->oper1->mode == am_dreg && ip->oper2->mode == am_seg && ip1
        ->opcode == op_mov && ip1->oper2->mode == am_dreg && ip1->oper2->length
        == 2)
    {

        ip->oper1 = ip1->oper1;
        remove_peep_entry(ip1);
        return ;
    }
    ip1 = ip->back;
    if (!ip1)
        return;
    if (ip1->opcode == op_mov && ip->oper1->mode != am_dreg
        && ip1->oper2->mode == am_immed && ip->oper2->mode == am_dreg
        && ip1->oper1->mode == am_dreg && ip1->oper1->preg == ip->oper2->preg)
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

        if (ip1->back->opcode == op_mov && equal_address(ip1->back->oper1, ip-> oper2)
            && equal_address(ip1->back->oper2, ip->oper1))
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
                        ip1->oper1 = ip->oper1;
                        ip1->oper1->liveRegs = -1;
                        if (!live(ip->oper1->liveRegs, ip->oper2->preg))
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
    while (ip1 && ip1->opcode == op_line)
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
void peep_movzx(OCODE *ip)
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
                                            ip->oper2->offset->v.i = (int)(char)ip->oper2->offset->v.i;
                                        }
                                        else /* two */
                                        {
                                            ip->oper2->offset->v.i = (int)(short)ip->oper2->offset->v.i;
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
void peep_movzx2(OCODE *ip)
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
                    OCODE *c1 = beLocalAlloc(sizeof(OCODE));
                    c1->opcode = op_xor;
                    c1->oper1 = makedreg(ip->oper1->preg);
                    c1->oper2 = makedreg(ip->oper1->preg);
                    c1->back = ip->back;
                    c1->fwd = ip;
                    ip->back->fwd = c1;
                    ip->back = c1;
                    ip->opcode = op_mov;
                    ip->oper1->length = ip->oper2->length;
                    ip->noopt = TRUE;
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
                    OCODE *c1 = beLocalAlloc(sizeof(OCODE));
                    c1->opcode = op_xor;
                    c1->oper1 = makedreg(ip->oper1->preg);
                    c1->oper2 = makedreg(ip->oper1->preg);
                    c1->back = ip->back;
                    c1->fwd = ip;
                    ip->back->fwd = c1;
                    ip->back = c1;
                    ip->opcode = op_mov;
                    ip->oper1->length = ip->oper2->length;
                    ip->noopt = TRUE;
                }
            }
        }
        else
        {
            if (ip->oper1->preg <= EBX)
            {
                if (((ip->oper2->mode != am_indisp && ip->oper2->mode != am_indispscale)
                     || ip->oper2->preg != ip->oper1->preg) && 
                     (ip->oper2->mode != am_indispscale || ip->oper2->sreg != ip->oper1->preg))
                {
                    OCODE *c1 = beLocalAlloc(sizeof(OCODE));
                    c1->opcode = op_xor;
                    c1->oper1 = makedreg(ip->oper1->preg);
                    c1->oper2 = makedreg(ip->oper1->preg);
                    c1->back = ip->back;
                    c1->fwd = ip;
                    ip->back->fwd = c1;
                    ip->back = c1;
                    ip->opcode = op_mov;
                    ip->oper1->length = ip->oper2->length;
                    ip->noopt = TRUE;
                }
                else
                {
                    OCODE *c1 = beLocalAlloc(sizeof(OCODE));
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
                    ip->noopt = TRUE;
                }
            }
        }
    }
}
void peep_tworeg(OCODE *ip)
{
    if (ip->oper1 && ip->oper2 && ip->oper1->mode == am_dreg && ip->oper2->mode == am_dreg 
        && ip->oper1->preg != ip->oper2->preg)
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
void peep_mathdirect(OCODE *ip)
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
        if (ip->oper1->length == ISZ_USHORT || ip->oper1->length == -ISZ_USHORT || ip->oper1->length == ISZ_U16)
        {
            ip->oper2->offset->v.i &= 0xffff;
        }
        if (ip->oper1->length == ISZ_UCHAR || ip->oper1->length == -ISZ_UCHAR)
        {
            ip->oper2->offset->v.i &= 0xff;
        }
    }
}
void peep_xor(OCODE *ip)
{
    if (ip->oper1->mode == am_dreg && ip->oper2->mode == am_dreg)
        if (ip->oper1->length == ISZ_USHORT || ip->oper1->length == ISZ_U16)
            if (ip->oper1->preg == ip->oper2->preg)
                ip->oper1->length = ip->oper2->length = ISZ_UINT;
}
void peep_or(OCODE *ip)
{
    if (ip->oper2->mode == am_immed)
        if (ip->oper2->offset->v.i == 0)
            remove_peep_entry(ip);
}
/*
 * change some ands to registers to movzx
 * delete ands of full size
 * delete itself if followed by AND or OR with same args...
 */
void peep_and(OCODE *ip)
{
    OCODE *ip2 = ip->fwd;
    if (ip2->opcode == op_or || ip2->opcode == op_and)
    {
        if (equal_address(ip2->oper1, ip->oper1))
        {
            if (equal_address(ip2->oper2,ip->oper2))
            {
                remove_peep_entry(ip);
                return ;
            }
            else if (equal_address(ip2->oper1, ip2->oper2))
            {
                remove_peep_entry(ip2);
                return ;
            }
            else if (ip2->opcode == op_and
                     && ip->oper2->mode == am_immed && ip2->oper2->mode == am_immed 
                     && isintconst(ip->oper2->offset)
                     && isintconst(ip2->oper2->offset))
            {
                ip2->oper2->offset->v.i &= ip->oper2->offset->v.i;
                remove_peep_entry(ip);
                return;
            }
        }
    }
    if (ip->opcode == op_and && ip->oper1->mode == am_dreg)
    {
        if (ip->oper2->mode == am_immed)
        {
            unsigned t = ip->oper2->offset->v.i;
            if (ip->oper1->length == ISZ_USHORT || ip->oper1->length == ISZ_U16)
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
                        int t1 = ip->back->oper2->offset->v.i;
                        if ((t == 0xff && t1 == 24) || (t == 0xffff && t1 == 16) ||
                            (t == 0xffffff && t1 == 8))
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
                        int t1 = ip->back->oper2->offset->v.i;
                        if ((t == 0xff000000U && t1 == 24) || (t == 0xffff0000U && t1 == 16) ||
                            (t == 0xffffff00U && t1 == 8))
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
         unsigned t = ip->oper2->offset->v.i;
        if (t == 0xffffffffU)
        {
            remove_peep_entry(ip);
        }
        
    }
}
/* take lea instructions followed by an add to the register and optimize
 * can be generated by structure references to stacked variables, for example
 */
void peep_lea(OCODE *ip)
{
    OCODE *ip1 = ip->fwd;
    if (ip1->opcode == op_add && ip1->oper1->mode == am_dreg)
    {
        if (ip1->oper1->preg == ip->oper1->preg)
        {
            if (ip1->oper2->mode == am_immed)
            {
                ip->oper2->offset = exprNode(en_add, ip->oper2->offset, 
                                             ip1->oper2->offset);
                remove_peep_entry(ip1);
            }
        }
    }
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
void peep_sar(OCODE *ip)
{
    if (ip->oper1->mode == am_dreg && ip->oper1->preg == EDX && (ip->oper1->length == ISZ_UINT || ip->oper1->length == ISZ_U32))
        if (ip->oper2->mode == am_immed && isintconst(ip->oper2->offset) && ip->oper2->offset->v.i == 0x1f)
        {
            OCODE *ip1 = ip->back;
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
                                ip1->oper1 = ip1->oper2 = NULL;
                            }
                        }
                    }
        }
}
void peep_mul(OCODE *ip)
{
    if (ip->oper1->mode == am_dreg)
        if (ip->oper2 && ip->oper2->mode == am_immed && isintconst(ip->oper2->offset))
        {
            if (ip->oper2->offset->v.i == 0)
            {
                ip->opcode = op_xor;
                ip->oper2 = ip->oper1;
            }
            else if (ip->oper2->offset->v.i == 1)
            {
                remove_peep_entry(ip);
            }
        }
}
void peep_fmath(OCODE *ip)
{
    if (ip->back->opcode == op_fld)
    {
        if (ip->back->oper1->length != ISZ_LDOUBLE)
        {
            if (ip->opcode == op_faddp)
                ip->opcode = op_fadd;
            else
                ip->opcode = op_fmul;
            ip->oper1 = ip->back->oper1;
            remove_peep_entry(ip->back);
        }
    }
    else if (ip->back->opcode == op_fild)
    {
        if (ip->opcode == op_faddp)
            ip->opcode = op_fiadd;
        else
            ip->opcode = op_fimul;
        ip->oper1 = ip->back->oper1;
        remove_peep_entry(ip->back);
    }
}
void peep_fstp(OCODE *ip)
{
    // prefer integer registers for what are essentially copies
    if (ip->back->opcode == op_fld)
    {
        if (ip->back->oper1->length == ip->oper1->length)
        {
            int sz = ip->oper1->length;
            AMODE *apl = ip->back->oper1;
            AMODE *apal = ip->oper1;
            AMODE *ax;
            ULLONG_TYPE live = ip->oper1->liveRegs;
            int ofsl = 0, ofsa = 0, reg = -1, push = FALSE, i;
            if (sz >= ISZ_IFLOAT)
                sz = sz - ISZ_IFLOAT + ISZ_FLOAT;
            sz = sizeFromISZ(sz);
            ip = ip->fwd;
            remove_peep_entry(ip->back->back);
            remove_peep_entry(ip->back);
            for (i=0; i < 4; i++)
            {
                if (regmap[i][0] < 3 && !(live & ((ULLONG_TYPE)1 << i)))
                {
                    if ((apl->mode != am_indisp || apl->preg != regmap[i][0])
                         && (apl->mode != am_indispscale  || ( apl->preg != regmap[i][0] && apl->sreg != regmap[i][0]))
                        && (apal->mode != am_indisp || apal->preg != regmap[i][0])
                         && (apal->mode != am_indispscale  || ( apal->preg != regmap[i][0] && apal->sreg != regmap[i][0])))
                    {
                        reg = regmap[i][0];
                        break;
                    }
                }		
            }
            if (reg == -1)
            {
                for (i=0; i < 6; i++)
                {
                    if ((apl->mode != am_indisp || apl->preg != regmap[i][0])
                         && (apl->mode != am_indispscale  || ( apl->preg != regmap[i][0] && apl->sreg != regmap[i][0]))
                        && (apal->mode != am_indisp || apal->preg != regmap[i][0])
                         && (apal->mode != am_indispscale  || ( apal->preg != regmap[i][0] && apal->sreg != regmap[i][0])))
                    {
                        reg = regmap[i][0];
                        push = TRUE;
                        break ;
                    }
                }
            }
            ax = makedreg(reg);
            ax->liveRegs = live;
            if (push)
            {
                if ((apl->mode == am_indisp || apl->mode == am_indispscale) && apl->preg == ESP)
                    ofsl = 4;
                if ((apal->mode == am_indisp || apal->mode == am_indispscale) && apal->preg == ESP)
                    ofsa = 4;
                insert_peep_entry(ip, op_push, ISZ_UINT, ax, 0);
            }
            switch(sz)
            {
                AMODE *apl1, *apa1;
                case 10:
                    apl1 = copy_addr(apl);
                    apl1->length = ISZ_UINT;
                    apl1->offset = exprNode(en_add, apl1->offset, intNode(en_c_i, 8+ofsl));
                    apa1 = copy_addr(apal);
                    apa1->length = ISZ_UINT;
                    apa1->offset = exprNode(en_add, apa1->offset, intNode(en_c_i, 8+ofsa));
                    insert_peep_entry(ip, op_mov, ISZ_UINT, ax, apl1);
                    insert_peep_entry(ip, op_mov, ISZ_UINT, apa1, ax);
                    // fallthrough
                case 8:
                    apl1 = copy_addr(apl);
                    apl1->length = ISZ_UINT;
                    apl1->offset = exprNode(en_add, apl1->offset, intNode(en_c_i, 4+ofsl));
                    apa1 = copy_addr(apal);
                    apa1->length = ISZ_UINT;
                    apa1->offset = exprNode(en_add, apa1->offset, intNode(en_c_i, 4+ofsa));
                    insert_peep_entry(ip, op_mov, ISZ_UINT, ax, apl1);
                    insert_peep_entry(ip, op_mov, ISZ_UINT, apa1, ax);
                    // fallthrough
                case 4:
                    apl1 = copy_addr(apl);
                    apl1->length = ISZ_UINT;
                    apl1->offset = exprNode(en_add, apl1->offset, intNode(en_c_i, ofsl));
                    apa1 = copy_addr(apal);
                    apa1->length = ISZ_UINT;
                    apa1->offset = exprNode(en_add, apa1->offset, intNode(en_c_i, ofsa));
                    insert_peep_entry(ip, op_mov, ISZ_UINT, ax, apl1);
                    insert_peep_entry(ip, op_mov, ISZ_UINT, apa1, ax);
                    
                    break;
            }
            if (push)
            {
                insert_peep_entry(ip, op_pop, ISZ_UINT, ax, 0);
            }
        }
    }
    else if (ip->oper1->mode == am_indisp && ip->oper1->preg == ESP
        && ip->back->back->opcode == op_fld
        && ip->back->opcode == op_sub && ip->back->oper1->mode == am_dreg && ip->back->oper1->preg == ESP)
    {
        if (ip->back->back->oper1->length == ip->oper1->length)
        {
            int sz = ip->oper1->length;
            AMODE *apl = ip->back->back->oper1;
            int ofs = 0;
            if (sz >= ISZ_IFLOAT)
                sz = sz - ISZ_IFLOAT + ISZ_FLOAT;
            sz = sizeFromISZ(sz);
            ip = ip->fwd;
            remove_peep_entry(ip->back->back->back);
            remove_peep_entry(ip->back->back);
            remove_peep_entry(ip->back);
            switch(sz)
            {
                AMODE *apl1;
                case 10:
                    apl1 = copy_addr(apl);
                    apl1->length = ISZ_UINT;
                    apl1->offset = exprNode(en_add, apl1->offset, intNode(en_c_i, 8+ofs));
                    insert_peep_entry(ip, op_push, ISZ_UINT, apl1, 0);
                    if ((apl->mode == am_indisp || apl->mode == am_indispscale) && apl->preg == ESP)
                        ofs += 4;
                    // fallthrough
                case 8:
                    apl1 = copy_addr(apl);
                    apl1->length = ISZ_UINT;
                    apl1->offset = exprNode(en_add, apl1->offset, intNode(en_c_i, 4 + ofs));
                    insert_peep_entry(ip, op_push, ISZ_UINT, apl1, 0);
                    if ((apl->mode == am_indisp || apl->mode == am_indispscale) && apl->preg == ESP)
                        ofs += 4;
                    // fallthrough
                case 4:
                    apl1 = copy_addr(apl);
                    apl1->length = ISZ_UINT;
                    apl1->offset = exprNode(en_add, apl1->offset, intNode(en_c_i, ofs));
                    insert_peep_entry(ip, op_push, ISZ_UINT, apl1, 0);
                    
                    break;
            }
        }
    }
    else if (ip->fwd->opcode == op_fld)
    {
        // fstp followed by fld == fst followed by nothing
        if (equal_address(ip->fwd->oper1, ip->oper1))
        {
            if (ip->oper1->length != ISZ_LDOUBLE)
            {
                ip->opcode = op_fst;
                remove_peep_entry(ip->fwd);
            }
        }
    }    
}
/*-------------------------------------------------------------------------*/

int novalue(OCODE *ip)
{
    return ip->fwd->opcode == op_void;
}

/*-------------------------------------------------------------------------*/

int equal_address(AMODE *ap1, AMODE *ap2)
{
    if (ap1->mode != ap2->mode)
        return (FALSE);
    if (ap1->length != ap2->length && ap1->length != -ap2->length)
    {
        int n1 = ap1->length < 0 ? - ap1->length : ap1->length;
        int n2 = ap2->length < 0 ? - ap2->length : ap2->length;
        if (n1 == ISZ_ULONG || n1 == ISZ_ADDR || n1 == ISZ_UINT || n1 == ISZ_U32)
            n1 = ISZ_UINT;
        if (n2 == ISZ_ULONG || n2 == ISZ_ADDR || n2 == ISZ_UINT || n2 == ISZ_U32)
            n2 = ISZ_UINT;
        if (n1 != n2)
            return FALSE;
    }
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
            if (ap1->preg != ap2->preg)
                return (FALSE);
            return TRUE;
        case am_freg:
            if (ap1->preg != ap2->preg)
                return (FALSE);
            return TRUE;

        case am_direct:
            return equalnode(ap1->offset, ap2->offset);
        default:
            break;
    }
    return (FALSE);
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
        // special case and reg1,reg1 because it is used for testing for nonzero
        if (ip->opcode == op_and && ip->oper2)
            if (ip->oper1->preg == ip->oper2->preg)
                return;
        if (ip->oper1 && (ip->oper1->length == ISZ_USHORT || ip->oper1->length == ISZ_U16))
            ip->oper1->length = ISZ_UINT;
        if (ip->oper2 && (ip->oper2->length == ISZ_USHORT || ip->oper1->length == ISZ_U16))
            ip->oper2->length = ISZ_UINT;
    }
}
OCODE * peep_div(OCODE *ip)
{
    if (ip->oper1->mode == am_dreg)
    {
        OCODE *ip2 = ip->back;
        while (ip2 && ip2->opcode != op_label && ip2->opcode < op_ja && ip2->opcode > op_jmp)
        {
            if (ip2->opcode == op_mov && ip2->oper1->mode == am_dreg && ip2->oper1->preg == ip->oper1->preg)
                break;
            ip2 = ip2->back;
        }
        if (ip2 && ip2->opcode != op_label && ip2->opcode < op_ja && ip2->opcode > op_jmp)
        {
            AMODE *oper = ip2->oper2;
            if (oper->mode != am_indisp || oper->preg != EAX && oper->preg != EDX && oper->preg != ESP)
                if (oper->mode != am_indispscale || oper->preg != EAX && oper->preg != EDX && oper->preg != ESP && oper->sreg != EAX && oper->sreg != EDX)
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
    return ip;
}
/*
 * treat move reg, value followed by push reg when value is an int or constant
 */
void peep_push(OCODE *ip)
{
    if (ip->oper1->mode == am_dreg)
    {
        OCODE *ip1 = ip->back;
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
            if (ip1->opcode == op_xor && equal_address(ip1->oper1, ip1->oper2) &&
                equal_address(ip1->oper1, ip->oper1))
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
void peep_call(OCODE *ip)
{
    if (ip->oper1->mode == am_dreg)
    {
        OCODE *ip1 = ip->back;
        if (ip1->opcode == op_mov && equal_address(ip->oper1, ip1->oper1))
        {
            ip->oper1 = ip1->oper2;
			remove_peep_entry(ip1);
        }
    }
}
void insert_peep_entry(OCODE *after, enum e_op opcode, int size, AMODE *ap1, AMODE *ap2)
{
    OCODE *a = beLocalAlloc(sizeof(OCODE));
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
 * peep list. It insures ther is no NULL pointer to cause exceptions.
 */
void remove_peep_entry(OCODE *ip)
{
    if (ip == peep_head)
    {
        peep_head = ip->fwd;
        peep_head->back = NULL;
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
    OCODE *ip;
    if (!prm_bepeep)
        return;
    for (ip = peep_head; ip; ip = ip->fwd)		
    {
        if (!ip->noopt)
        {
            switch (ip->opcode)
            {
                case op_fstp:
                    peep_fstp(ip);
                    break;
                case op_faddp:
                case op_fmulp:
                    peep_fmath(ip);
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
                        while (ip->fwd && ip->fwd->opcode == op_mov && 
                               ip->oper1->mode == am_indisp && ip->oper1->preg == EBP &&
                               ip->oper2->mode == am_dreg &&
                               equal_address(ip->oper1, ip->fwd->oper1) &&
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
