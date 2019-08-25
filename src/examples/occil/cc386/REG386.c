
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
#include "diag.h"

/*
 *      this module handles the allocation and de-allocation of
 *      temporary registers. when a temporary register is allocated
 *      the stack depth is saved in the field deep of the address
 *      mode structure. when validate is called on an address mode
 *      structure the stack is popped until the register is restored
 *      to it's pre-push value.
 */
extern int cf_freedata, cf_freeaddress, cf_freefloat;
extern int prm_stackalign;
extern int prm_fardefault;
extern int noRelease;

AMODE push[1], pop[1];
int max_data; /* Max available */
int max_sreg;
int regs[3], sregs[3];
int pushcount;
int pushpos;
unsigned char pushedregs[1000], pushed[1000];
int marks[100], markedpushes[100];
int regpos, markpos;
static int tsegs[] = 
{
    GS, ES, FS
};
int lastreg1, lastreg2;
int lastsreg1, lastsreg2;

#define EAXEDX 240
#define FUNCHASH 241

void regini(void)
{
    InitRegs();
}

//-------------------------------------------------------------------------

void SetFuncMode(int mode)
{
/*
    if (mode)
        pushedregs[pushcount++] = FUNCHASH;
    else if (pushedregs[pushcount - 1] == FUNCHASH)
        pushcount--;
*/
}

//-------------------------------------------------------------------------

void GenPush(int reg, int rmode, int flag)
/*
 *      this routine generates code to push a register onto the stack
 */
{
    AMODE *ap1;
    ap1 = xalloc(sizeof(AMODE));
    ap1->preg = reg;
    ap1->mode = rmode;
    ap1->length = 4;
    if (rmode == am_freg)
    {
        FLOAT;
    }
    else
    {
        OCODE *new = xalloc(sizeof(OCODE));
        new->opcode = op_push;
        new->oper1 = ap1;
        if (rmode == am_dreg && reg >= 24 && reg < 32)
        {
            ap1->preg = 0;
            ap1->seg = reg - 24;
            ap1->mode = am_seg;
        }
        add_peep(new);
    }
}

//-------------------------------------------------------------------------

void GenPop(int reg, int rmode, int flag)
/*
 *      generate code to pop the primary register in ap from the
 *      stack.
 */
{
    AMODE *ap1;
    ap1 = xalloc(sizeof(AMODE));
    ap1->preg = reg;
    ap1->mode = rmode;
    ap1->length = 4;
    if (rmode == am_freg)
    {
        FLOAT;
    }
    else
    {
        OCODE *new = xalloc(sizeof(OCODE));
        new->opcode = op_pop;
        new->oper1 = ap1;
        if (rmode == am_dreg && reg >= 24 && reg < 32)
        {
            ap1->preg = 0;
            ap1->seg = reg - 24;
            ap1->mode = am_seg;
        }
        add_peep(new);
    }
}
/*
void popready(void)
{
    while (pushcount && readytopop[pushcount - 1] && pushedregs[pushcount-1] != FUNCHASH)
    {
        pushcount--;
		if (pushedregs[pushcount] == EAXEDX)
		{
	        GenPop(EAX, am_dreg, 0);
	        GenPop(EDX, am_dreg, 0);
		}
		else
		{
	        GenPop(pushedregs[pushcount], am_dreg, 0);
		}
    }
}
*/
//-------------------------------------------------------------------------

void PushRegs(unsigned mask)
{
    int i;
    int umask = 0x08000;
	int cnt = 0;
    for (i = 0; i < 4; i++)
    {
        if (umask &mask)
		{
            GenPush(i, am_dreg, 1);
			cnt ++;
		}
        umask >>= 1;
    }
    umask = 0x080;
    for (i = 4; i < 8; i++)
    {
        if (umask &mask)
		{
            GenPush(i, am_dreg, 1);
			cnt++;
		}
        umask >>= 1;
    }
	if (prm_stackalign)
	{
		if (cnt %4)
		{
			gen_code(op_add, makedreg(ESP), make_immed(-(4 - cnt) * 4));
		}
	}
}

/* This is ONLY called from the return.  Calling from any other place
 * will leave the stack depth unpredictable... */
void PopRegs(unsigned mask)
{
    int i;
    int umask = 0x800;
	int cnt = 0;
	if (prm_stackalign)
	{
		for (i=0; i < 4; i++)
		{
			if (umask & mask)
				cnt++;
			if ((umask >> 8) & mask)
				cnt++;
			umask >>= 1;
		}
		if (cnt %4)
		{
			gen_code(op_add, makedreg(ESP), make_immed((4 - cnt) * 4));
		}
		umask = 0x800;
	}
    for (i = 7; i >= 4; i--)
    {
        if (umask &mask)
        {
            GenPop(i, am_dreg, 1);
        }
        umask >>= 1;
    }
    umask = 0x8;
    for (i = 3; i >= 0; i--)
    {
        if (umask &mask)
        {
            GenPop(i, am_dreg, 1);
        }
        umask >>= 1;
    }
}

//-------------------------------------------------------------------------

//int dregbase;

void InitRegs(void)
{
	regpos = 0;
	markpos = 0;
	pushpos = 0;
	memset(regs, 0, sizeof(regs));
	memset(sregs, 0, sizeof(sregs));
}
void ChooseRegs(int *a, int *s)
{
	int xsregs[3], xregs[3];
	int n = regpos;
	int acnt = 0, scnt = 0;
	int axdx = FALSE;
	int i;
	*a = 0, *s = 0;
	memset(&xregs, 0, sizeof(xregs));
	memset(&xsregs, 0, sizeof(xsregs));
	while (n)
	{
		if (pushedregs[n-1]  < EBX)
		{
			if (acnt < 2)
			{
				if (!xregs[pushedregs[n-1]]++)
					acnt++;
			}
		}
		else if (pushedregs[n-1] == EDXEAX)
		{
			axdx = TRUE;
			if (acnt < 2)
			{
				if (!xregs[EAX]++)
					acnt++;
			}
			if (acnt < 2)
			{
				if (!xregs[EDX]++)
					acnt++;
			}
		}
		else if (pushedregs[n-1] != UNUSED_REG)
		{
			if (scnt < 2)
			{
				if (!xsregs[pushedregs[n-1]-24-ES]++)
					scnt++;
			}
		}
		n--;		
	}
	for (i=EDX; i >= EAX; i--)
	{
		if (!xregs[i])
		{
//			if (axdx && (i != ECX))
//				*a = EDXEAX;
//			else
				*a = i;
		}
		if (!xsregs[2-i])
		{
			*s = GS - i;
		}
	}
}
void MarkRegs(void)
{
	marks[markpos] = regpos;
	markedpushes[markpos++] = pushpos;
}
void unpush()
{
	if (markpos)
	{
		while (regpos > marks[markpos-1])
			if (pushedregs[--regpos] < EBX)
			{
				regs[pushedregs[regpos]]--;
			}
			else if (pushedregs[regpos] == EDXEAX)
			{
				regs[EDX]--;
				regs[EAX]--;
			}
			else if (pushedregs[regpos] != UNUSED_REG)
			{
				sregs[pushedregs[regpos]-24-ES]--;
			}
		--markpos;
		while (pushpos > regpos)
		{
			-- pushpos;
		}
		pushpos = markedpushes[markpos];
	}
}
int ReleaseWouldPop(void)
{
	int top = marks[markpos-1];
	int bottom = regpos;
	while (--bottom >= top)
		if (pushed[bottom])
			return TRUE;
	return FALSE;
}
void ReleaseRegs(void)
{
	if (markpos)
	{
		while (regpos > marks[markpos-1])
			if (pushedregs[--regpos] < 3)
			{
				regs[pushedregs[regpos]]--;
			}
			else if (pushedregs[regpos] == EDXEAX)
			{
				regs[EDX]--;
				regs[EAX]--;
			}
			else if (pushedregs[regpos] != UNUSED_REG)
			{
				sregs[pushedregs[regpos]-24-ES]--;
			}
		--markpos;
		while (pushpos > regpos)
		{
			if (pushed[--pushpos])
				if (pushedregs[pushpos] == EDXEAX)
				{
					GenPop(EAX, am_dreg, 0);
					GenPop(EDX, am_dreg, 0);
				}
				else if (pushedregs[regpos] != UNUSED_REG)
				{
					GenPop(pushedregs[pushpos], am_dreg, 0);
				}
		}
		pushpos = markedpushes[markpos];
	}
}
AMODE *temp_axdx(void)
{
	AMODE *ap = xalloc(sizeof(AMODE));
	RealizeDreg(EDXEAX);
	ap->mode = am_axdx;
	ap->length = BESZ_QWORD;
	return ap;
	
}
void RealizeSreg(int reg)
{
	pushedregs[regpos++] = reg + 24;
	if (sregs[reg-ES])
	{
		pushed[regpos-1] = TRUE;
		GenPush(reg+24, am_dreg, 0);
		pushpos = regpos;
	}
	else
	{
		pushed[regpos-1] = FALSE;
	}
	sregs[reg-ES]++;
}
AMODE * RealizeDreg(int reg)
{
	AMODE *ap = xalloc(sizeof(AMODE));
	if (reg < EBX)
	{
		pushedregs[regpos++] = reg;
		if (regs[reg])
		{
			pushed[regpos-1] = TRUE;
			GenPush(reg, am_dreg, 0);
			pushpos = regpos;
		}
		else
		{
			pushed[regpos-1] = FALSE;
		}
		regs[reg]++;
	}
	else if (reg == EDXEAX)
	{
		pushedregs[regpos++] = reg;
		pushed[regpos-1] = FALSE;
		if (regs[EDX] || regs[EAX])
		{
			pushed[regpos-1] = TRUE;
			GenPush(EDX, am_dreg, 0);
			GenPush(EAX, am_dreg, 0);
			pushpos = regpos;
		}
		regs[EDX]++;
		regs[EAX]++;
	}
	ap->mode = am_dreg;
	ap->preg = reg;
	ap->length = BESZ_DWORD;
	return ap;
}
void loadaxdx(AMODE *ap)
{
	if (ap->length < BESZ_QWORD && ap->length != - BESZ_QWORD)
	{
		IntToHigher(ap, 0, ap->length, BESZ_QWORD);
	}
	else if (ap->mode == am_immed)
	{
		RealizeDreg(EDXEAX);
        #if sizeof(ULLONG_TYPE) == 4
            gen_codes(op_mov, BESZ_DWORD , makedreg(EDX), make_immed(0));
        #else 
            gen_codes(op_mov, BESZ_DWORD , makedreg(EDX), make_immed((ap
                ->offset->v.i >> 32) &0xffffffff));
        #endif 
        gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), make_immed(ap->offset
            ->v.i &0xffffffff));
	}
	else // must be mem
	{
		AMODE *ap2 = copy_addr(ap);
		int edx = 0, eax = 0;
		if (ap->mode == am_indisp)
		{
			if (ap->preg == EDX)
				edx++;
			else if (ap->preg == EAX)
				eax++;
		}
		else if (ap->mode == am_indispscale)
		{
			if (ap->preg == EDX)
				edx++;
			else if (ap->preg == EAX)
				eax++;
			if (ap->sreg == EDX)
				edx++;
			else if (ap->sreg == EAX)
				eax++;
		}
		if (!edx && !eax)
			RealizeDreg(EDXEAX);
		else if (!edx)
			RealizeDreg(EDX);
		else if (!eax)
			RealizeDreg(EAX);
		ap2->offset = makenode(en_add, ap2->offset, makeintnode(en_icon, 4));
		if (eax && edx)
		{
			gen_codes(op_push, BESZ_DWORD, ap2, 0);
			gen_codes(op_mov, BESZ_DWORD, makedreg(EAX), ap);
			gen_codes(op_pop, BESZ_DWORD, makedreg(EDX), 0);
		}
		else if (edx)
		{
			noRelease = TRUE;
			gen_codes(op_mov, BESZ_DWORD, makedreg(EAX), ap);
			gen_codes(op_mov, BESZ_DWORD, makedreg(EDX), ap2);
		}
		else
		{
			noRelease = TRUE;
			gen_codes(op_mov, BESZ_DWORD, makedreg(EDX), ap2);
			gen_codes(op_mov, BESZ_DWORD, makedreg(EAX), ap);
		}	
	}
	ap->mode = am_axdx;
	ap->length = BESZ_QWORD;
}
void reuseaxdx(AMODE *ap)
{
	if (ap->mode == am_dreg || ap->mode == am_indisp)
	{
		if (ap->preg == EAX && !regs[EDX])
		{
			pushedregs[regpos++] = EDX;
			pushed[regpos-1] = FALSE;
			regs[EDX]++;
			return;
		}
		else if (ap->preg == EDX && !regs[EAX])
		{
			pushedregs[regpos++] = EAX;
			pushed[regpos-1] = FALSE;
			regs[EAX]++;
			return;
		}
	}
	else if (ap->mode == am_indispscale)
	{
		if (ap->preg == EAX)
		{
			if (ap->sreg == EDX)
			{
				return;
			}
			else if (!regs[EDX])
			{
				pushedregs[regpos++] = EDX;
				pushed[regpos-1] = FALSE;
				regs[EDX]++;
				return;
			}
		}
		else if (ap->preg == EDX)
		{
			if (ap->sreg == EAX)
			{
				return;
			}
			else if (!regs[EAX])
			{
				pushedregs[regpos++] = EAX;
				pushed[regpos-1] = FALSE;
				regs[EAX]++;
				return;
			}
		}
		else
		{
			if (ap->sreg == EAX && !regs[EDX])
			{
				pushedregs[regpos++] = EDX;
				pushed[regpos-1] = FALSE;
				regs[EDX]++;
				return;
			}
			else if (ap->sreg == EDX && !regs[EAX])
			{
				pushedregs[regpos++] = EAX;
				pushed[regpos-1] = FALSE;
				regs[EAX]++;
				return;
			}
		}
	}
	temp_axdx();
}
AMODE *reuseop(AMODE *ap, int areg, int *n)
{
	AMODE *apr;
	*n = -1;
	if ((ap->mode == am_dreg || ap->mode == am_indisp || ap->mode == am_indispscale) && areg == ap->preg)
		apr = makedreg(areg);
	else if (ap->mode == am_indispscale && areg == ap->sreg)
		apr = makedreg(areg);
	else if ((ap->mode == am_dreg || ap->mode == am_indisp || ap->mode == am_indispscale) && (ap->preg >= EAX && ap->preg < EBX))
		apr = makedreg(ap->preg);
	else if (ap->mode == am_indispscale && ap->sreg >= EAX && ap->sreg < EBX)
		apr = makedreg(ap->sreg);
	else
		return RealizeDreg(areg);
//	pushedregs[regpos++] = apr->preg;
//	pushed[regpos-1] = FALSE;
//	regs[apr->preg]++;
	return apr;
}
void FreeUnused(int n)
{
	if (n != -1)
	{
		GenPop(n, am_dreg, 0);
	}
}
AMODE *axdx_tolower(int areg)
{
	AMODE *rv = makedreg(areg);
	int i = regpos;
	int reg1, reg2;
	int n;
	extern int lineno;
	while (--i >= marks[markpos-1])
	{
		if (pushed[i] || pushedregs[i] == EDXEAX || pushedregs[i] == EAX || pushedregs[i] == EDX)
			break;
	}
//	printf("%d %d %d %d\n", areg, pushedregs[i], pushed[i], lineno);
	if (i >= marks[markpos-1])
	{
		if (pushed[i])
		{
			if (pushedregs[i] == EDXEAX)
			{
				switch (areg)
				{
					case EAX:
						pushedregs[i] = EAX;
						regs[EDX]--;
						gen_code(op_pop, makedreg(EDX), 0);
						gen_code(op_xchg, make_stack(0), makedreg(EDX));
						break;
					case EDX:
						pushedregs[i] = EDX;
						regs[EAX]--;
						gen_code(op_mov, rv, makedreg(EAX));
						gen_code(op_pop, makedreg(EAX), 0);
						break;
					default:
						if (regs[areg])
						{
							gen_code(op_mov, makedreg(EDX), make_stack(-4));
							gen_code(op_mov, make_stack(-4), rv);
							gen_code(op_mov, rv, makedreg(EAX));
							gen_code(op_pop, makedreg(EAX), 0);
							regs[EDX]--;
							regs[EAX]--;
							if (areg < EBX)
							{
								regs[areg]++;
								pushedregs[i] = areg;
							}
							else
							{
								pushedregs[i] = UNUSED_REG;
								pushed[i] = FALSE;
							}
						}
						else
						{
							gen_code(op_mov, rv, makedreg(EAX));
							gen_code(op_pop, makedreg(EAX), 0);
							gen_code(op_pop, makedreg(EDX), 0);
							regs[EDX]--;
							regs[EAX]--;
							regs[areg]++;
							pushed[i] = FALSE;
							pushedregs[i] = areg;
						}
						break;
				}
			}
			else
			{
				switch (areg)
				{
					case EAX:
						
						if (pushedregs[i] == EAX)
						{
							if (pushedregs[i-1] == EDX)
							{
								gen_code(op_pop, makedreg(EDX), NULL);
								gen_code(op_xchg, makedreg(EDX), make_stack(0));
								pushedregs[i] = UNUSED_REG;
								pushedregs[i-1] = EAX;
								pushed[i] = FALSE;
								regs[EDX]--;
							}
						}
						else if (pushedregs[i] == EDX)
						{
							if (pushedregs[i-1] == EAX)
							{
								gen_code(op_pop, makedreg(EDX), NULL);
								pushed[i] = FALSE;
								pushedregs[i] = UNUSED_REG;
								regs[EDX]--;
							}
						}
						break;
					case EDX:
						if (pushedregs[i] == EAX)
						{
							if (pushedregs[i-1] == EDX)
							{
								gen_code(op_mov, makedreg(EDX), makedreg(EAX));
								gen_code(op_pop, makedreg(EAX), NULL);
								pushedregs[i] = UNUSED_REG;
								pushed[i] = FALSE;
								regs[EAX]--;
	
							}
						}
						else if (pushedregs[i] == EDX)
						{
							if (pushedregs[i-1] == EAX)
							{
								gen_code(op_mov, makedreg(EDX), make_stack(0));
								gen_code(op_xchg, makedreg(EDX), make_stack(-4));
								gen_code(op_xchg, makedreg(EDX), makedreg(EAX));
								gen_code(op_add, makedreg(ESP), make_immed(4));
								pushedregs[i] = UNUSED_REG;
								pushed[i] = FALSE;
								pushedregs[i-1] = EDX;
								regs[EAX]--;
							}
						}
						break;
					default:
						if (pushedregs[i] == EAX)
						{
							if (pushedregs[i-1] == EDX)
							{
								if (regs[areg])
								{
									gen_code(op_mov, makedreg(EDX), rv);
									gen_code(op_xchg, make_stack(-4), makedreg(EDX));
									gen_code(op_mov, rv, makedreg(EAX));
									gen_code(op_pop, makedreg(EAX), NULL);
									regs[EAX]--;
									regs[EDX]--;
									regs[areg]++;
									pushedregs[i] = UNUSED_REG;
									pushed[i] = FALSE;
									pushedregs[i-1] = areg;
								}
								else
								{
									regs[EAX]--;
									regs[EDX]--;
									regs[areg]++;
									pushedregs[i] = UNUSED_REG;
									pushed[i] = FALSE;
									pushedregs[i-1] = areg;
									pushed[i-1] = FALSE;
								}
							}
						}
						else if (pushedregs[i] == EDX)
						{
							if (pushedregs[i-1] == EAX)
							{
								if (regs[areg])
								{
									gen_code(op_xchg, make_stack(-4), rv);
									gen_code(op_xchg, rv, makedreg(EAX));
									gen_code(op_pop, makedreg(EDX), NULL);
									regs[EAX]--;
									regs[EDX]--;
									regs[areg]++;
									pushedregs[i] = UNUSED_REG;
									pushed[i] = FALSE;
									pushedregs[i-1] = areg;
								}
								else
								{
									gen_code(op_mov, rv, makedreg(EAX));
									gen_code(op_pop, makedreg(EDX), NULL);
									gen_code(op_pop, makedreg(EAX), NULL);
									regs[EAX]--;
									regs[EDX]--;
									regs[areg]++;
									pushedregs[i] = UNUSED_REG;
									pushed[i] = FALSE;
									pushedregs[i-1] = areg;
									pushed[i-1] = FALSE;
								}
							}
						}
						break;
				}
			}
		}
		else
		{
			if (pushedregs[i] == EDXEAX)
			{
				switch (areg)
				{
					case EAX:
						regs[EDX]--;
						pushedregs[i] = EAX;
						break;
					case EDX:
						regs[EAX]--;
						pushedregs[i] = EDX;
						gen_code(op_mov, makedreg(EDX), makedreg(EAX));
						break;
					default:
						regs[EAX]--;
						regs[EDX]--;
						pushedregs[i] = areg;
						regs[areg]++;
						gen_code(op_mov, rv, makedreg(EAX));	
						break;
				}
			}
			else
			{
				pushedregs[i] = UNUSED_REG;
				pushedregs[i-1] = areg;
				switch (areg)
				{
					case EAX:
						regs[EDX]--;
						break;
					case EDX:
						regs[EAX]--;
						gen_code(op_mov, rv, makedreg(EAX));
						break;
					default:
						regs[EDX]--;
						regs[EAX]--;
						if (areg < EBX && regs[areg])
						{
							gen_code(op_push, rv, NULL);
						
						}
						regs[areg]++;
						pushed[i] = areg;
						gen_code(op_mov, rv, makedreg(EAX));
						break;
				}
			}	
		}
	}
	else
	{
		DIAG("axdx_tolower: invalid stack frame");
	}
	return rv;
}
void FreeReg(int n)
{
	int xpushed = FALSE;
	if (n >= 0 && n < EBX)
	{
		int i = regpos;
		while (i >  0)
		{
			i--;
			if (pushedregs[i] == n)
			{
				if (pushed[i] && xpushed)
					break;
				pushedregs[i] = UNUSED_REG;
				regs[n]--;
				if (pushed[i])
				{
					pushed[i] = FALSE;
					GenPop(n, am_dreg, 0);
				}
				break;
			}
			if (pushed[i])
				xpushed = TRUE;
		}
	}
}
void FreeInd(AMODE *left, AMODE *right)
{
	int n;
	if (left)
		n = left->mode;
	else
		n = am_dreg;
	switch (n)
	{
		case am_indispscale:
			FreeReg(left->sreg);
			// fall through
		case am_indisp:
			FreeReg(left->preg);
			break;
		default:
			if (right)
				switch (right->mode)
				{
					case am_indispscale:
						if (!left || left->mode != am_dreg || left->preg != right->sreg)
							FreeReg(right->sreg);
						// fall through
					case am_indisp:
						if (!left || left->mode != am_dreg || left->preg != right->preg)
							FreeReg(right->preg);
						break;
				}
	}
}