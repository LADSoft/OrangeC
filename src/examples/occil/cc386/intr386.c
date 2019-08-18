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
 * handle intrinsics
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "lists.h"
#include "gen386.h"
#include "stdlib.h"

#define KWINTRINSHASH 33

extern int prm_cmangle, prm_intrinsic;
extern int regs[3];
extern long nextlabel;

static AMODE* temp_data() { return NULL; }
static HASHREC **hashtable = 0;
static AMODE edxreg[] = 
{
    {
        am_dreg, EDX, 0, 0, 0, 4
    }
};
static AMODE alreg[] = 
{
    {
        am_dreg, EAX, 0, 0, 0, 1
    }
};
static AMODE clreg[] = 
{
    {
        am_dreg, ECX, 0, 0, 0, 1
    }
};
static AMODE axreg[] = 
{
    {
        am_dreg, EAX, 0, 0, 0, 2
    }
};
static AMODE dxreg[] = 
{
    {
        am_dreg, EDX, 0, 0, 0, 2
    }
};
static int pushedax, pushedcx;

static INTRINSICS intrinsic_table[] = 
{
    {
        0, "abs", 1
    }
    , 
    {
        0, "div", 2
    }
    , 
    {
        0, "ldiv", 3
    }
    , 
    {
        0, "_rotl", 4
    }
    , 
    {
        0, "_rotr", 5
    }
    , 
    {
        0, "memcmp", 6
    }
    , 
    {
        0, "memcpy", 7
    }
    , 
    {
        0, "memmove", 8
    }
    , 
    {
        0, "memset", 9
    }
    , 
    {
        0, "strcat", 10
    }
    , 
    {
        0, "strcmp", 11
    }
    , 
    {
        0, "strcpy", 12
    }
    , 
    {
        0, "strlen", 13
    }
    , 
    {
        0, "strncat", 14
    }
    , 
    {
        0, "strncmp", 15
    }
    , 
    {
        0, "strncpy", 16
    }
    , 
    {
        0, "memchr", 17
    }
    , 
    {
        0, "strchr", 18
    }
    , 
    {
        0, "strrchr", 19
    }
    , 
    {
        0, "inportb", 20
    }
    , 
    {
        0, "inport", 21
    }
    , 
    {
        0, "inp", 22
    }
    , 
    {
        0, "inpw", 23
    }
    , 
    {
        0, "outportb", 24
    }
    , 
    {
        0, "outport", 25
    }
    , 
    {
        0, "outp", 26
    }
    , 
    {
        0, "outpw", 27
    }
    , 
    {
        0, "_crotl", 28
    }
    , 
    {
        0, "_crotr", 29
    }
    , 
    {
        0, "__disable", 30
    }
    , 
    {
        0, "__enable", 31
    }
    , 
    {
        0, "inportd", 32
    }
    , 
    {
        0, "outportd", 33
    }
    , 
    {
        0, "labs", 1
    }
    ,  // same as abs()
    {
        0, 0, 0
    }
    , 
};
void IntrinsIni(void)
{
    INTRINSICS *q = intrinsic_table;
    if (!hashtable)
    {
        hashtable = (INTRINSICS **)malloc(KWINTRINSHASH *sizeof(INTRINSICS*));
        memset(hashtable, 0, KWINTRINSHASH *sizeof(INTRINSICS*));
        if (!hashtable)
            fatal("Out of memory");
        while (q->word)
        {
            AddHash(q, hashtable, KWINTRINSHASH);
            q++;
        }
    }
}

//-------------------------------------------------------------------------

void SearchIntrins(SYM *sp)
{
    HASHREC **q;
    char *p = sp->name;
    if (!prm_intrinsic)
    {
        sp->tp->cflags &= ~DF_INTRINS;
        return ;
    }
    if (prm_cmangle)
        p++;
    q = LookupHash(p, hashtable, KWINTRINSHASH);
    if (!q)
    {
        gensymerror(ERR_NOINTRINSFOUND, sp->name);
        sp->tp->cflags &= ~DF_INTRINS;
        return ;
    }
    sp->value.i = ((INTRINSICS*)(*q))->val;
}

//-------------------------------------------------------------------------

static int oneparm(ENODE *parms)
{
    if (!parms)
        return 0;
    return 1;
}

//-------------------------------------------------------------------------

static int twoparms(ENODE *parms)
{
    if (!parms || !parms->v.p[1])
        return 0;
    return 1;
}

//-------------------------------------------------------------------------

static int threeparms(ENODE *parms)
{
    if (!parms || !parms->v.p[1] || !parms->v.p[1]->v.p[1])
        return 0;
    return 1;
}

//-------------------------------------------------------------------------

static AMODE *roll(ENODE *parms, int size, int op)
{
    AMODE *ap1,  *ap2,  *ap3;
	int areg, sreg;
    int pushed = FALSE;
    if (!twoparms(parms))
        return 0;
    ap1 = gen_expr_external(parms->v.p[1]->v.p[0], 0, 0, 4);
	ChooseRegs(&areg, &sreg);
    ap1 = do_extend(ap1, areg, sreg, size, F_DREG | F_VOL);
    ap2 = gen_expr_external(parms->v.p[0], 0, 0, 0);
    if (ap2->mode != am_immed)
    {
		ChooseRegs(&areg, &sreg);
        ap2 = do_extend(ap2, areg, sreg, 4, 0);
        if (ap1->mode == am_dreg && ap1->preg == ECX)
        {
            ap3 = temp_data();
            gen_code(op_mov, ap3, ap1);
            //freeop(ap1);
            ap1 = ap3;
        }
        if (ap2->mode != am_dreg || ap2->preg != ECX)
        {
            if (regs[2])
            {
                GenPush(ECX, am_dreg, 0);
                pushed = TRUE;
            }
            gen_code(op_mov, makedreg(ECX), ap2);
            ap2 = clreg;
        }
    }
    ap1->length = size;
    gen_code(op, ap1, ap2);
    if (pushed)
        GenPop(ECX, am_dreg, 0);
    //freeop(ap2);
    //freeop(ap1);
    return ap1;
}

//-------------------------------------------------------------------------

static void LoadCX(ENODE *parms)
{
    AMODE *ap1;
	int areg, sreg;
    regs[0]++;
    ap1 = gen_expr_external(parms, FALSE, FALSE, 4);
	ChooseRegs(&areg, &sreg);
    ap1 = do_extend(ap1, areg, sreg, 4, F_DREG | F_VOL);
    regs[0]--;
    if (ap1->preg != ECX)
        gen_code(op_mov, makedreg(ECX), ap1);
}

//-------------------------------------------------------------------------

static void LoadSIDI(ENODE *parms, int backwards, int savecx)
{
    AMODE *ap1,  *ap2,  *ap3;
	int areg, sreg;
    pushedax = pushedcx = FALSE;
    ap2 = gen_expr_external(parms->v.p[0], FALSE, FALSE, 0);
    ap1 = gen_expr_external(parms->v.p[1]->v.p[0], FALSE, FALSE, 0);
	/*
    if (ap1->mode == am_indisp && ap1->preg == ESI || (ap1->mode ==
        am_indispscale && (ap1->preg == ESI || ap1->sreg == ESI)) || (ap1->mode
        == am_dreg && ap1->preg == ESI))
    {
        ap3 = temp_data();
        gen_code(op_mov, ap3, ap1);
        //freeop(ap1);
        ap1 = ap3;
    }
*/
    gen_codes(op_mov, 4, makedreg(ESI), ap2);
    gen_codes(op_mov, 4, makedreg(EDI), ap1);
    if (backwards)
        gen_code(op_xchg, makedreg(ESI), makedreg(EDI));
    gen_code(op_cld, 0, 0);
}

//-------------------------------------------------------------------------

static void LoadDI(ENODE *parms, int savecx)
{
    AMODE *ap1;
    pushedax = pushedcx = FALSE;
	MarkRegs();
    ap1 = gen_expr_external(parms->v.p[0], FALSE, FALSE, 4);
    if (regs[1] && savecx)
    {
        GenPush(ECX, am_dreg, 0);
        pushedcx = TRUE;
    }
    if (regs[0])
    {
        GenPush(EAX, am_dreg, 0);
        pushedax = TRUE;
    }
    gen_codes(op_mov, 4, makedreg(EDI), ap1);
    gen_code(op_cld, 0, 0);
	ReleaseRegs();
}

//-------------------------------------------------------------------------

static AMODE *popDI(AMODE *ap, int tocx)
{
    AMODE *ap1 = ap;
    if (pushedax)
    {
        GenPop(EAX, am_dreg, 0);
    }
    if (pushedcx)
        GenPop(ECX, am_dreg, 0);
    GenPop(EDI, am_dreg, 0);
    if (ap1 == 0)
        ap1 = clreg;
    return ap1;
}

//-------------------------------------------------------------------------

static AMODE *popSIDI(AMODE *ap, int tocx)
{
    AMODE *ap1 = popDI(ap, tocx);
    GenPop(ESI, am_dreg, 0);
    return ap1;
}

//-------------------------------------------------------------------------

static void skipdi(void)
{
    gen_code(op_sub, makedreg(EAX), makedreg(EAX));
    gen_code(op_mov, makedreg(ECX), make_immed( - 1));
    gen_code(op_repnz, 0, 0);
    gen_code(op_scasb, 0, 0);
    gen_code(op_dec, makedreg(EDI), 0);
}

//-------------------------------------------------------------------------

static AMODE *xstrlen(int todec)
{
    AMODE *ap1;
    gen_code(op_sub, makedreg(EAX), makedreg(EAX));
    gen_code(op_mov, makedreg(ECX), make_immed( - 1));
    gen_code(op_repnz, 0, 0);
    gen_code(op_scasb, 0, 0);
    gen_code(op_not, makedreg(ECX), 0);
    if (todec)
        gen_code(op_dec, makedreg(ECX), 0);
    ap1 = temp_data();
    if (ap1->preg != ECX)
        gen_code(op_mov, ap1, makedreg(ECX));
    return ap1;
}

//-------------------------------------------------------------------------

static void xmove(void)
{
    gen_code3(op_shrd, makedreg(EAX), makedreg(ECX), make_immed(2));
    gen_code(op_shr, makedreg(ECX), make_immed(2));
    gen_code(op_repnz, 0, 0);
    gen_code(op_movsd, 0, 0);
    gen_code3(op_shld, makedreg(ECX), makedreg(EAX), make_immed(2));
    gen_code(op_repnz, 0, 0);
    gen_code(op_movsb, 0, 0);
}

//-------------------------------------------------------------------------

static void xset(void)
{
    gen_codes(op_mov, 1, makedreg(4), makedreg(0));
    gen_code2(op_movzx, 4, 2, makedreg(EDX), makedreg(EAX));
    gen_code(op_shl, makedreg(EAX), make_immed(16));
    gen_code(op_or, makedreg(EAX), makedreg(EDX));
    gen_code3(op_shrd, makedreg(EDX), makedreg(ECX), make_immed(2));
    gen_code(op_shr, makedreg(ECX), make_immed(2));
    gen_code(op_repnz, 0, 0);
    gen_code(op_stosd, 0, 0);
    gen_code3(op_shld, makedreg(ECX), makedreg(EDX), make_immed(2));
    gen_code(op_repnz, 0, 0);
    gen_code(op_stosb, 0, 0);
}

//-------------------------------------------------------------------------

static AMODE *xstrcmp(void)
{
    int neg = nextlabel++, exit = nextlabel++;
    AMODE *ap1 = temp_data();
    gen_code(op_repz, 0, 0);
    gen_code(op_cmpsb, 0, 0);
    gen_code(op_mov, ap1, make_immed(0));
    gen_code(op_je, make_label(exit), 0);
    gen_code(op_js, make_label(neg), 0);
    gen_code(op_inc, ap1, 0);
    gen_comment("; TEST EAX,IMMED (branches around next four bytes of code)\n");
    gen_codes(op_genword, 0, make_immed(0xa9), 0);
    gen_codes(op_nop, 0, 0, 0);
    gen_codes(op_nop, 0, 0, 0);
    gen_codes(op_nop, 0, 0, 0);
    gen_label(neg);
    gen_code(op_dec, ap1, 0);
    gen_label(exit);
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *inport(ENODE *parms, int size)
{
    AMODE *ap1;
    int pushed = FALSE, pushedax = FALSE;
    if (!oneparm(parms))
        return 0;
    ap1 = gen_expr_external(parms->v.p[0], 0, 0, 2);
    if (ap1->mode != am_dreg || ap1->preg != EDX)
    {
        //freeop(ap1);
        if (regs[2])
        {
            pushed = TRUE;
            GenPush(EDX, am_dreg, 0);
        }
        gen_codes(op_mov, 2, makedreg(EDX), ap1);
    }
    if (regs[0])
    {
        pushedax = TRUE;
        GenPush(EAX, am_dreg, 0);
    }
    switch (size)
    {
        case 1:
            gen_code(op_in, alreg, dxreg);
            gen_code(op_movzx, makedreg(EAX), alreg);
            break;
        case 2:
            gen_code(op_in, axreg, dxreg);
            gen_code(op_movzx, makedreg(EAX), axreg);
            break;
    }
    if (pushedax)
    {
        ap1 = temp_data();
        gen_code(op_mov, ap1, makedreg(EAX));
        GenPop(EAX, am_dreg, 0);
    }
    else
    {
        regs[0]++;
        ap1 = makedreg(EAX);
    }
    if (pushed)
        GenPop(EDX, am_dreg, 0);
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *outport(ENODE *parms, int size)
{
    AMODE *ap1,  *ap2;
    int pushed = FALSE, pushedax = FALSE;
    if (!twoparms(parms))
        return 0;
    ap2 = gen_expr_external(parms->v.p[0], FALSE, FALSE, size);
    if (regs[0] && (ap2->mode != am_dreg || ap2->preg != EAX))
    {
        pushedax = TRUE;
        GenPush(EAX, am_dreg, 0);
        regs[0]--;
    }
    else
	{
		int areg, sreg;
		ChooseRegs(&areg, &sreg);
        ap2 = do_extend(ap2, areg, sreg, size, F_DREG | F_VOL);
	}
    ap1 = gen_expr_external(parms->v.p[1]->v.p[0], FALSE, FALSE, 2);
    if (ap1->mode != am_dreg || ap1->preg != EDX)
    {
        //freeop(ap1);
        if (regs[2])
        {
            pushed = TRUE;
            GenPush(EDX, am_dreg, 0);
        }
        gen_codes(op_mov, 2, makedreg(EDX), ap1);
    }
    switch (size)
    {
        case 1:
            gen_code(op_out, dxreg, alreg);
            break;
        case 2:
            gen_code(op_out, dxreg, axreg);
            break;
    }
    if (pushed)
        GenPop(EDX, am_dreg, 0);
    if (pushedax)
    {
        GenPop(EAX, am_dreg, 0);
        regs[0]++;
    }
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *HandleIntrins(ENODE *node, int novalue)
{
    ENODE *parms = node->v.p[1]->v.p[1]->v.p[0];
    AMODE *ap1,  *ap2,  *ap3;
    SYM *sp;
	int areg, sreg;
	return 0;
#if 0
    int xchg = FALSE;
    if (prm_intrinsic && (node->v.p[1]->v.p[0]->nodetype == en_nacon || node
        ->v.p[1]->v.p[0]->nodetype == en_napccon) && ((sp = node->v.p[1]
        ->v.p[0]->v.sp)->tp->cflags &DF_INTRINS))
    {
        switch (sp->value.i)
        {
            case 1:
                 /* abs */
                if (!oneparm(parms))
                    return 0;
                ap1 = gen_expr_external(parms->v.p[0], 0, 0, 4);
				ChooseRegs(&areg, &sreg);
                ap1 = do_extend(ap1, areg, sreg, ap1->length, F_DREG | F_VOL);
                if (ap1->preg != EAX)
                {
                    gen_code(op_xchg, ap1, makedreg(EAX));
                    xchg = TRUE;
                }
                if (regs[2])
                    GenPush(EDX, am_dreg, 0);
                gen_code(op_cdq, 0, 0);
                gen_code(op_xor, makedreg(EAX), edxreg);
                gen_code(op_sub, makedreg(EAX), edxreg);
                if (regs[2])
                    GenPop(EDX, am_dreg, 0);
                if (xchg)
                    gen_code(op_xchg, ap1, makedreg(EAX));
                return ap1;
            case 2:
                 /* div */
            case 3:
                 /* ldiv */
                break;
            case 4:
                 /* _rotl */
                return roll(parms, 4, op_rol);
            case 5:
                 /* _rotr */
                return roll(parms, 4, op_ror);
            case 6:
                 /* memcmp */
                if (!threeparms(parms))
                    return 0;
                GenPush(ESI, am_dreg, 0);
                GenPush(EDI, am_dreg, 0);
                LoadCX(parms->v.p[0]);
                LoadSIDI(parms->v.p[1], TRUE, FALSE);
                ap1 = xstrcmp();

                return popSIDI(ap1, 0);
            case 7:
                 /* memcpy */
                if (!threeparms(parms))
                    return 0;
                GenPush(ESI, am_dreg, 0);
                GenPush(EDI, am_dreg, 0);
                LoadCX(parms->v.p[0]);
                LoadSIDI(parms->v.p[1], FALSE, FALSE);
                xmove();

                return popSIDI(0, 0);
            case 8:
                 /* memmove */
                break;
            case 9:
                 /* memset */
                if (!threeparms(parms))
                    return 0;
                GenPush(EDI, am_dreg, 0);
                LoadDI(parms->v.p[1]->v.p[1], FALSE);
                ap2 = gen_expr_external(parms->v.p[1]->v.p[0], FALSE, FALSE, 4);
				ChooseRegs(&areg, &sreg);
                ap2 = do_extend(ap2, areg, sreg, 4, F_DREG | F_VOL);
                if (ap2->preg != EAX)
                    gen_code(op_mov, makedreg(EAX), ap2);
                regs[0]++;
                ap1 = gen_expr_external(parms->v.p[0], FALSE, FALSE, 4);
				ChooseRegs(&areg, &sreg);
                ap1 = do_extend(ap1, areg, sreg, 4, F_DREG | F_VOL);
                if (ap1->preg != ECX)
                    gen_code(op_mov, makedreg(ECX), ap1);
                regs[0]--;
                //freeop(ap1);
                //freeop(ap2);
                xset();
                return popDI(0, 0);
            case 10:
                 /* strcat */
                if (!twoparms(parms))
                    return 0;
                GenPush(ESI, am_dreg, 0);
                GenPush(EDI, am_dreg, 0);
                LoadSIDI(parms, FALSE, TRUE);
                skipdi();
                gen_code(op_xchg, makedreg(ESI), makedreg(EDI));
                GenPush(EDI, am_dreg, 0);
                xstrlen(FALSE);
                GenPop(EDI, am_dreg, 0);
                gen_code(op_xchg, makedreg(ESI), makedreg(EDI));
                xmove();
                popSIDI(0, 0);
                ap1 = gen_expr_external(parms->v.p[1]->v.p[0], FALSE, FALSE, 0);
                return ap1;
            case 11:
                 /* strcmp */
                if (!twoparms(parms))
                    return 0;
                GenPush(ESI, am_dreg, 0);
                GenPush(EDI, am_dreg, 0);
                LoadSIDI(parms, TRUE, TRUE);
                GenPush(EDI, am_dreg, 0);
                xstrlen(FALSE);
                GenPop(EDI, am_dreg, 0);
                ap1 = xstrcmp();
                return popSIDI(ap1, FALSE);
            case 12:
                 /* strcpy */
                if (!twoparms(parms))
                    return 0;
                GenPush(ESI, am_dreg, 0);
                GenPush(EDI, am_dreg, 0);
                LoadSIDI(parms, TRUE, TRUE);
                GenPush(EDI, am_dreg, 0);
                xstrlen(FALSE);
                GenPop(EDI, am_dreg, 0);
                gen_code(op_xchg, makedreg(ESI), makedreg(EDI));
                xmove();
                popSIDI(0, 0);
                ap1 = gen_expr_external(parms->v.p[1]->v.p[0], FALSE, FALSE, 0);
                return ap1;
            case 13:
                 /* strlen */
                if (!oneparm(parms))
                    return 0;
                GenPush(EDI, am_dreg, 0);
                LoadDI(parms, FALSE);
                ap1 = xstrlen(TRUE);
                return popDI(ap1, TRUE);
            case 14:
                 /* strncat */
            case 15:
                 /* strncmp */
            case 16:
                 /* strncpy */
            case 17:
                 /* memchr */
            case 18:
                 /* strchr */
            case 19:
                 /* strrchr */
                break;
            case 20:
                 /* inportb */
                return inport(parms, 1);
            case 21:
                 /* inport */
                return inport(parms, 2);
            case 22:
                 /* inp */
                return inport(parms, 1);
            case 23:
                 /* inpw */
                return inport(parms, 2);
            case 24:
                 /* outportb */
                return outport(parms, 1);
            case 25:
                 /* outport */
                return outport(parms, 2);
            case 26:
                 /* outp */
                return outport(parms, 1);
            case 27:
                 /* outpw */
                return outport(parms, 2);
            case 28:
                 /* _crotl */
                return roll(parms, 1, op_rol);
            case 29:
                 /* _crotr */
                return roll(parms, 1, op_ror);
            case 30:
                 /* _disable */
                if (parms)
                    return 0;
                gen_code(op_cli, 0, 0);
                return 1;
            case 31:
                 /* __enable */
                if (parms)
                    return 0;
                gen_code(op_sti, 0, 0);
                return 1;
            case 32:
                 /* inportd */
                return inport(parms, 4);
            case 33:
                 /* outportd */
                return outport(parms, 4);
        }
    }
    if (node->v.p[1]->v.p[0]->nodetype == en_nacon || node->v.p[1]->v.p[0]
        ->nodetype == en_napccon)
        node->v.p[1]->v.p[0]->v.sp->tp->cflags &= ~DF_INTRINS;
    return 0;
#endif
}
