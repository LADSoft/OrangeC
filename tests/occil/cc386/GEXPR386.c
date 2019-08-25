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
extern OCODE *peep_tail,  *frame_ins;
extern SYM *currentfunc;
extern int retlab;
extern int prm_stackalign;
extern int noRelease;

#define ABS(x) ( (x) < 0 ? -(x) : (x))
/*
 *      this module contains all of the code generation routines
 *      for evaluating expressions and conditions.
 */
extern int prm_optmult;
extern SYM *declclass;
extern int stdinttype, stdunstype, stdintsize, stdldoublesize;
extern int stdaddrsize;
extern int stackadd, stackmod;
extern int prm_largedata;
extern AMODE push[], pop[];
extern int prm_68020, prm_intrinsic;
extern int regs[3], sregs[3];
extern long nextlabel;
extern long lc_maxauto;
extern int prm_cmangle;
extern int prm_cplusplus;
extern int prm_farkeyword;
extern unsigned char pushedregs[], pushed[];
extern int regpos;

extern int floatstack_mode;
AMODE freg0[] = 
{
    {
        am_freg, 0
    }
};
AMODE sreg[] = 
{
    {
        am_dreg, BESZ_DWORD 
    }
};

int genning_inline;
static int ldx[] = 
{
    op_mov, 0, op_lds, op_les, op_lfs, op_lgs, op_lss
};

static int last_align;

static AMODE *gen_expr(ENODE *node, int areg, int sreg, int novalue, int flags, int size);
AMODE *reuseop(AMODE *ap, int areg, int *n);
void reuseaxdx(AMODE *ap);
/* little ditty to allocate stack space for floating point
 * conversions and return a pointer to it
 * it will be at the bottom for standard stack frames; top for 
 * using ESP for frames
 * 0-7 used for fistp, 8-BESZ_IFLOAT used for long intermediate values
 */
AMODE *floatconvpos(void)
{
    AMODE *ap1;
    int constv = 16;
    //   if (prm_cplusplus)
    //      constv = 12;
    if (!floatstack_mode)
    {
        if (!lc_maxauto)
        {
            OCODE *new = xalloc(sizeof(OCODE));
            new->opcode = op_sub;
            new->oper1 = makedreg(ESP);
            new->oper2 = make_immed(constv);
            new->back = frame_ins;
            new->fwd = frame_ins->fwd;
            frame_ins->fwd->back = new;
            frame_ins->fwd = new;
        }
        else
        {
            frame_ins->oper2->offset->v.i += constv;
			if (frame_ins->oper2->offset->v.i >= 0x80)
			{
				frame_ins->oper2->length = 0;
			}
        }
        floatstack_mode += constv;
		lc_maxauto += constv;
    }
    ap1 = xalloc(sizeof(AMODE));
    ap1->mode = am_indisp;
    ap1->preg = EBP;
    if (prm_farkeyword)
        ap1->seg = SS;
    ap1->offset = makeintnode(en_icon,  - lc_maxauto);
    ap1->length = BESZ_DWORD ;
    return ap1;
}

//-------------------------------------------------------------------------

int defseg(ENODE *node)
{
    int rv = e_default; // default DS
    if (!prm_farkeyword)
        return rv;
    while (castvalue(node))
        node = node->v.p[0];
    if (node->nodetype == en_add)
    {
        if ((rv = defseg(node->v.p[0])) != e_default)
            return rv;
        return defseg(node->v.p[1]);
    }
    else if (node->nodetype == en_napccon)
        rv = e_cs;
	else if (node->nodetype == en_labcon)
		rv = e_fs;
    else if (node->nodetype == en_autocon)
        rv = e_ss;
	else if (node->nodetype == en_nacon)
	{
		SYM *sp = node->v.p[0];
        if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
			rv = e_fs;
	}
    return rv;
}

//-------------------------------------------------------------------------

AMODE *truncateFloat(AMODE *ap1, int size)
{
    AMODE *ap = floatconvpos();
    ap1 = do_extend(ap1, 0, 0, size, F_FREG);
    gen_codefs(op_fstp, size, ap, 0);
    gen_codefs(op_fld, size, ap, 0);
    return ap1;
}

//-------------------------------------------------------------------------

int chksize(int lsize, int rsize)
{
    int l, r;
    l = lsize;
    r = rsize;
    if (l < 0)
        l =  - l;
    if (r < 0)
        r =  - r;
    if (rsize == BESZ_BOOL)
     /* BESZ_BOOL is used for bools, which are the smallest type now */
        if (lsize == BESZ_BOOL)
            return FALSE;
        else
            return TRUE;
    if (lsize == BESZ_BOOL)
        return FALSE;
    if (rsize ==  BESZ_FARPTR)
        if (lsize ==  BESZ_FARPTR)
            return FALSE;
        else
            return l >= BESZ_QWORD;
        else
            if (lsize ==  BESZ_FARPTR)
                return r < BESZ_QWORD;
    return (l > r);
}

//-------------------------------------------------------------------------

AMODE *fstack(void)
{
    AMODE *ap = xalloc(sizeof(AMODE));
    ap->mode = am_freg;
    ap->length = BESZ_LDOUBLE;
    ap->preg = 0;
    ap->sreg = 0;
    ap->offset = 0;
    ap->tempflag = TRUE;
    return (ap);
}

//-------------------------------------------------------------------------

AMODE *make_muldivval(AMODE *ap)
{
    int temp;
    AMODE *ap1 = make_label(queue_muldivval(ap->offset->v.i));
    ap1->mode = am_direct;
    return (ap1);
}

//-------------------------------------------------------------------------

void make_floatconst(AMODE *ap)
{
    int size = ap->length;
    if (isintconst(ap->offset->nodetype))
    {
        if (size > 0)
            UnsignedLongLongToFPF(&ap->offset->v.f, ap->offset->v.i);
        else
            LongLongToFPF(&ap->offset->v.f, ap->offset->v.i);
        ap->offset->nodetype = en_rcon;
        size = BESZ_DOUBLE;
    }
    else
    {
        switch (ap->offset->nodetype)
        {
            case en_fcon:
                size = BESZ_FLOAT;
                break;
            case en_rcon:
                size = BESZ_DOUBLE;
                break;
            case en_lrcon:
                size = BESZ_LDOUBLE;
                break;
            case en_fimaginarycon:
                size = BESZ_IFLOAT;
                break;
            case en_rimaginarycon:
                size = BESZ_IDOUBLE;
                break;
            case en_lrimaginarycon:
                size = BESZ_ILDOUBLE;
                break;
            case en_fcomplexcon:
                size = BESZ_CFLOAT;
                break;
            case en_rcomplexcon:
                size = BESZ_CDOUBLE;
                break;
            case en_lrcomplexcon:
                size = BESZ_CLDOUBLE;
                break;
			default:
				DIAG("invalid floating constant");
				return;
        }
    }
    if (ValueIsOne(&ap->offset->v.f))
    {
        ap->mode = am_fconst;
        ap->preg = fcone;
		ap->length = size;
    }
    else if (ap->offset->v.f.type == IFPF_IS_ZERO)
    {
        ap->mode = am_fconst;
        ap->preg = fczero;
		ap->length = size;
    }
    else if (size >= BESZ_CFLOAT)
    {
        AMODE *ap1;
        int siz1 = size -13;
        int o = prm_optmult ;
        prm_optmult = FALSE;
        if (siz1>BESZ_DOUBLE)
            siz1++;
        ap1 = make_label(queue_floatval(&ap->offset->v.c.r, siz1));
        queue_floatval(&ap->offset->v.c.i, siz1);
        ap->mode = am_direct;
        ap->length = size;
        ap->offset = ap1->offset;
        prm_optmult = o ;
    }
    else
    {
        AMODE *ap1 = make_label(queue_floatval(&ap->offset->v.f, size));
        ap->mode = am_direct;
        ap->length = size;
        ap->offset = ap1->offset;
    }
}

//-------------------------------------------------------------------------

AMODE *make_label(int lab)
/*
 *      construct a reference node for an internal label number.
 */
{
    ENODE *lnode;
    AMODE *ap;
    lnode = xalloc(sizeof(ENODE));
    lnode->nodetype = en_labcon;
    lnode->v.i = lab;
    ap = xalloc(sizeof(AMODE));
    ap->mode = am_immed;
    ap->offset = lnode;
    return ap;
}

//-------------------------------------------------------------------------

AMODE *makesegreg(int seg)
{
    AMODE *ap = xalloc(sizeof(AMODE));
    ap->mode = am_seg;
    ap->seg = seg;
    return ap;
}

//-------------------------------------------------------------------------

AMODE *make_immed(long i)
/*
 *      make a node to reference an immediate value i.
 */
{
    AMODE *ap;
    ENODE *ep;
    ep = xalloc(sizeof(ENODE));
    ep->nodetype = en_icon;
    ep->v.i = i;
    ap = xalloc(sizeof(AMODE));
    ap->mode = am_immed;
    ap->offset = ep;
    return ap;
}

//-------------------------------------------------------------------------

AMODE *make_immedt(long i, int size)
/*
 *      make a node to reference an immediate value i.
 */
{
    switch (size)
    {
        case BESZ_BYTE:
            case  - 1: i &= 0xff;
            break;
        case BESZ_WORD:
            case  - BESZ_WORD: i &= 0xffff;
            break;
        case BESZ_DWORD :
            case  - BESZ_DWORD : i &= 0xffffffff;
            break;
    }
    return make_immed(i);
}

//-------------------------------------------------------------------------

AMODE *make_offset(ENODE *node)
/*
 *      make a direct reference to a node.
 */
{
    AMODE *ap;
    ap = xalloc(sizeof(AMODE));
    ap->mode = am_direct;
    ap->offset = node;
    ap->seg = 0;
    return ap;
}

//-------------------------------------------------------------------------

AMODE *make_stack(int number)
{
    AMODE *ap = xalloc(sizeof(AMODE));
    ENODE *ep = xalloc(sizeof(ENODE));
    ep->nodetype = en_icon;
    ep->v.i =  - number;
    ap->mode = am_indisp;
    ap->preg = ESP;
    ap->offset = ep;
    return (ap);
}

//-------------------------------------------------------------------------

void do_ftol(void)
{
    if (prm_intrinsic)
    {
        AMODE *sp, vv,  *e8,  *e0,  *ax;
        gen_code(op_sub, sp = makedreg(ESP), make_immed(12));
        memset(&vv, 0, sizeof(vv));
        vv.mode = am_indisp;
        vv.preg = ESP;
        vv.offset = e8 = makeintnode(en_icon, 8);
        gen_code(op_fnstcw, copy_addr(&vv), 0);
        gen_codes(op_mov, BESZ_WORD, ax = makedreg(EAX), copy_addr(&vv));
        gen_codes(op_or, 1, makedreg(4), make_immed(0xc));
        vv.offset = makeintnode(en_icon, BESZ_LDOUBLE);
        gen_codes(op_mov, BESZ_WORD, copy_addr(&vv), ax);
        gen_code(op_fldcw, copy_addr(&vv), 0);
        vv.offset = e0 = makeintnode(en_icon, 0);
        gen_codes(op_fistp, BESZ_DOUBLE, copy_addr(&vv), 0);
        vv.offset = e8;
        gen_code(op_fldcw, copy_addr(&vv), 0);
        vv.offset = e0;
        gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), copy_addr(&vv));
        gen_code(op_add, sp, make_immed(12));
    }
    else
        call_library("__ftol");
}

//-------------------------------------------------------------------------

AMODE *complexstore(ENODE *node, AMODE *apr, int novalue, int size)
{
    AMODE *ap2 ;
    int offs = 0, siz = BESZ_BYTE;
    if (apr->mode == am_frfr)
    {
        DIAG("complexstore: double fregs");
        return apr ;
    }
    ap2 = copy_addr(apr);
    switch(apr->length)
    {
        case BESZ_CFLOAT:
            offs = 4;
            siz = BESZ_FLOAT;
            break;
        case BESZ_CDOUBLE:
            offs = 8;
            siz = BESZ_DOUBLE;
            break;
        case BESZ_CLDOUBLE:
            offs = 10;
            siz = BESZ_LDOUBLE;
            break;
    }
    ap2->offset = makenode(en_add,ap2->offset,makeintnode(en_icon,offs));
	if (novalue)
	{
	    gen_codefs(op_fstp,siz,apr,0);
    	gen_codefs(op_fstp,siz,ap2,0);
	}
	else if (siz == BESZ_LDOUBLE)
	{
		gen_codefs(op_fld, siz, makefreg(0), 0);
	    gen_codefs(op_fstp,siz,apr,0);
		gen_codefs(op_fxch, siz, makefreg(1), 0);
		gen_codefs(op_fld, siz, makefreg(0), 0);
    	gen_codefs(op_fstp,siz,ap2,0);
		gen_codefs(op_fxch, siz, makefreg(1), 0);
	}
	else
	{
	    gen_codefs(op_fst,siz,apr,0);
		gen_codefs(op_fxch, siz, makefreg(1), 0);
    	gen_codefs(op_fst,siz,ap2,0);
		gen_codefs(op_fxch, siz, makefreg(1), 0);
	}
    apr->mode = am_frfr;
    apr->length = BESZ_CLDOUBLE;
    return apr;
}
//-------------------------------------------------------------------------

AMODE *floatstore(ENODE *node, AMODE *apr, int novalue, int size)
{
    if (isbit(node))
    {
        AMODE *ap1 = fstack();
		int areg, sreg;
		ChooseRegs(&areg, &sreg);
        ap1 = do_extend(ap1, areg, sreg, BESZ_DWORD , F_DREG | F_VOL);
		ChooseRegs(&areg, &sreg);
        bit_store(apr, ap1, areg, sreg, node, novalue);
        return ap1;
    }
    else if (size <= BESZ_DWORD && size >= - BESZ_DWORD)
    {
        int pushed = FALSE;
        if (regs[0] && apr->preg != 0)
        {
            pushed = 1;
            GenPush(EAX, am_dreg, 0);
        }
        do_ftol();
        if (apr->mode != am_dreg || apr->preg != 0)
            gen_codes(op_mov, size, apr, makedreg(EAX));
        if (pushed)
            GenPop(EAX, am_dreg, 0);
        apr->length = size;
        return apr;
    }
    else if (size == BESZ_QWORD || size == - BESZ_QWORD || size == BESZ_BOOL)
    {
        AMODE *ap = temp_axdx();
        call_library("__ftoll");
        ap->length = size;
        return ap;
    }
    else
    {
        if (novalue || apr->length == BESZ_LDOUBLE)
        {
            gen_codef(op_fstp, apr, 0);
            return apr;
        }
        else
        {
            gen_codef(op_fst, apr, 0);
            return makefreg(0);
        }
    }
}

//-------------------------------------------------------------------------

void complexload(AMODE *ap, int flag)
{
    AMODE *ap2 ;
    int offs = 0, siz = ap->length;
    if (ap->mode == am_frfr)
        return;
    if (ap->mode == am_fconst)
    {
        AMODE *ap1;
        int siz1 = ap->length -13;
        int o = prm_optmult ;
        prm_optmult = FALSE;
        if (siz1>BESZ_DOUBLE)
            siz1++;
        ap1 = make_label(queue_floatval(&ap->offset->v.c.r, siz1));
        queue_floatval(&ap->offset->v.c.i, siz1);
        ap->mode = am_direct;
        ap->offset = ap1->offset;
		ap->length = siz1;
        prm_optmult = o ;
    }
    ap2 = copy_addr(ap);
    switch(ap->length)
    {
        case BESZ_CFLOAT:
		case BESZ_FLOAT:
            offs = 4;
            siz = BESZ_FLOAT;
            break;
        case BESZ_CDOUBLE:
		case BESZ_DOUBLE:
            offs = 8;
            siz = BESZ_DOUBLE;
            break;
        case BESZ_CLDOUBLE:
		case BESZ_LDOUBLE:
            offs = 10;
            siz = BESZ_LDOUBLE;
            break;
    }
    ap2->offset = makenode(en_add,ap2->offset,makeintnode(en_icon,offs));
    gen_codefs(op_fld,siz,ap2,0);
    gen_codefs(op_fld,siz,ap,0);
    ap->mode = am_frfr;
    ap->length = BESZ_CLDOUBLE;
}
//-------------------------------------------------------------------------

void floatload(AMODE *ap, int flag)
{
    if (ap->mode == am_fconst)
    {
        switch (ap->preg)
        {
            case fczero:
                gen_codef(op_fldz, 0, 0);
                if (ap->offset->v.f.sign)
                    gen_codef(op_fchs, 0, 0);
                break;
            case fcone:
                gen_codef(op_fld1, 0, 0);
                if (ap->offset->v.f.sign)
                    gen_codef(op_fchs, 0, 0);
                break;
        }
        ap->mode = am_freg;
        ap->preg = 0;
    }
    else
    if (flag && ap->mode != am_freg)
    {
        if (ap->mode == am_dreg)
        {
            AMODE *ap1 = floatconvpos();
			int areg, sreg;
			MarkRegs();
			ChooseRegs(&areg, &sreg);
            ap = do_extend(ap, areg, sreg, BESZ_DWORD , 0);
            gen_codes(op_mov, BESZ_DWORD , ap1, ap);
            gen_codefs(op_fild, BESZ_DWORD, ap1, 0);
			ReleaseRegs();
        }
        else
        {
            gen_codef(op_fld, ap, 0);
            ap->mode = am_freg;
            ap->preg = 0;
        }
    }
}

//-------------------------------------------------------------------------

int is_memory(AMODE *ap)
{
    switch (ap->mode)
    {
        case am_indisp:
        case am_indispscale:
        case am_direct:
            return TRUE;
    }
    return FALSE;
}

//-------------------------------------------------------------------------

AMODE *bit_load(AMODE *ap, int areg, int sreg, ENODE *node)
{
	int sz = ap->length;
    ap = do_extend(ap, areg, sreg, BESZ_DWORD, F_DREG | F_VOL);
	if (sz < 0 && node->bits != 1)
	{
		if (node->startbit != (32 - node->bits))
		    gen_code(op_shl, ap, make_immed(32 - node->startbit - node->bits));
		if (node->bits != 32)
		    gen_code(op_sar, ap, make_immed(32 - node->bits));
	}
	else
	{
		if (node->startbit)
		    gen_code(op_shr, ap, make_immed(node->startbit));
		if (node->startbit != (32 - node->bits))
		    gen_code(op_and, ap, make_immed(mod_mask(node->bits)));
	}
    return ap;
}

//-------------------------------------------------------------------------

void bit_store(AMODE *ap2, AMODE *ap1, int areg, int sreg, ENODE *node, int novalue)
{
    AMODE *ap3;
    if (ap1->mode == am_immed)
    {
        int vb;
		int len = ap2->length;
        ap3 = make_immed(ap1->offset->v.i &mod_mask(node->bits));
		
            if (node->bits == 1 && len > 1)
                gen_codes(op_btr, len , ap2, make_immed(node->startbit));
            else
	            gen_code(op_and, ap2, make_immed(~(mod_mask(node->bits) << node
                ->startbit)));
            if (len > 1 && (vb = single_bit(ap1->offset->v.i)) !=  - 1)
            {
                gen_codes(op_bts, len , ap2, make_immed(vb + node->startbit));
            }
            else
        if (ap3->offset->v.i)
        {
            ap3->offset->v.i <<= node->startbit;
            gen_codes(op_or, ap2->length, ap2, ap3);
        }
    }
    else
    {
        ap1 = do_extend(ap1, areg, sreg, ap2->length, F_DREG | F_VOL);
        gen_code(op_and, ap1, make_immed(mod_mask(node->bits)));
        if (!novalue)
            gen_codes(op_push, BESZ_DWORD , ap1, 0);
        if (node->startbit)
            gen_code(op_shl, ap1, make_immed(node->startbit));
        gen_code(op_and, ap2, make_immed(~(mod_mask(node->bits) << node
            ->startbit)));
        gen_code(op_or, ap2, ap1);
        if (!novalue)
            gen_codes(op_pop, BESZ_DWORD , ap1, 0);
    }
}

//-------------------------------------------------------------------------

AMODE * FloatToInt(int areg, int isize, int osize)
{
    // assumed in freg at this point
    if (osize == BESZ_QWORD || osize ==  - BESZ_QWORD || osize == BESZ_BOOL)
    {
        AMODE *ap2 = temp_axdx();

		if (osize == BESZ_BOOL)
		{
			call_library("__ftoll");
			gen_codes(op_or, BESZ_DWORD, makedreg(EAX), makedreg(EDX));
			gen_codes(op_setne, BESZ_BOOL, makedreg(EAX), 0);
			ap2 = makedreg(EAX);
		}
		else
		{
#if 0
		    if (osize > 0)
		    {
		        AMODE *ap = floatconvpos();
		        gen_codes(op_mov, BESZ_DWORD, ap, make_immed(0));
		        ap->offset = makenode(en_add, ap->offset, makeintnode(en_icon, 4));
		        gen_codes(op_mov, BESZ_DWORD, ap, make_immed(1<<31));
		        ap->offset = ap->offset->v.p[0];
		        gen_codefs(op_fild, BESZ_DOUBLE, ap, 0);
		        gen_codef(op_faddp, makefreg(1), 0);
		    }
#endif
		    call_library("__ftoll");
#if 0
		    if (osize > 0)
		        gen_codes(op_add, BESZ_DWORD, makedreg(EDX), make_immed(1<<31));
#endif
		}
		ap2->length = osize;
		return ap2;
    }
    else if (osize < BESZ_QWORD)
    {
        int pushed = FALSE;
		AMODE *ap2 = RealizeDreg(areg);
        if (regs[0] && areg != 0)
        {
            pushed = 1;
            GenPush(EAX, am_dreg, 0);
        }
#if 0
        if (osize > 0)
        {
            AMODE *ap = floatconvpos();
            gen_codes(op_mov, BESZ_DWORD, ap, make_immed(1<<31));
            gen_codefs(op_fild, BESZ_DWORD, ap, 0);
            gen_codef(op_faddp, makefreg(1), 0);
        }
#endif
        do_ftol();
#if 0
        if (osize > 0)
            gen_codes(op_add, BESZ_DWORD, makedreg(EAX), make_immed(1<<31));
#endif
        if (areg != 0)
            gen_codes(op_mov, BESZ_DWORD , ap2, makedreg(EAX));
        if (pushed)
            GenPop(EAX, am_dreg, 0);
        ap2->length = osize;
		return ap2;
    }
}
AMODE * FloatToLower(AMODE *ap, int areg, int isize, int osize)
{
	AMODE *rv = ap;
    if (isize >= BESZ_CFLOAT) 
    {
        complexload(ap,TRUE) ;
        if (osize < BESZ_CFLOAT)
            if (osize >= BESZ_IFLOAT)
            {
				rv = makefreg(0);
				rv->length = osize;
                ap->length = osize;
				gen_codefs(op_fstp, BESZ_LDOUBLE, makefreg(0), 0);
				if (osize < BESZ_FLOAT)
				{
					gen_codefs(op_fstp, BESZ_LDOUBLE, makefreg(0), 0);
					rv = make_immed(0);
				}
            }
            else
            {
				rv = makefreg(0);
				rv->length = osize;
				gen_code(op_fxch, 0, 0);
                gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0);
				if (osize < BESZ_FLOAT)
					rv = FloatToInt(areg, isize, osize);
            }
    }
    else if (isize >= BESZ_IFLOAT)
    {
        if (osize < BESZ_IFLOAT)
        {
            if (ap->mode == am_freg)
                gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0);
            gen_codef(op_fldz,0,0);
			rv = makefreg(0);
			rv->length = osize;
			if (osize < BESZ_FLOAT)
				rv = FloatToInt(areg, isize, osize);
        } 
        else
        {
            if (ap->mode != am_freg)
                floatload(rv, TRUE);
            rv->length = osize;
        }
    }
    else 
    {
        if (rv->mode != am_freg)
            floatload(rv, TRUE);
        rv->length = osize;
		if (osize < BESZ_FLOAT)
			rv = FloatToInt(areg, isize, osize);
	}
	return rv;
}
AMODE *IntToLower(AMODE *ap, int areg, int sreg, int isize, int osize, int flags)
{
	AMODE *rv = ap, *ap2;
    int seg = 0;
	if (isize  == BESZ_FARPTR && osize != BESZ_FARPTR)
	{
		seg = sreg;
		RealizeSreg(seg);
	}
    if (osize ==  BESZ_FARPTR)
    {
        seg = sreg;
		RealizeSreg(sreg);
		ap2 = RealizeDreg(areg);
        if (ap->mode == am_immed)
        {
			AMODE *ap3;
            #if sizeof(ULLONG_TYPE) == 4
                gen_code(op_mov, ap2, make_immed(0));
            #else 
                gen_code(op_mov, ap2, make_immed(ap->offset->v.i >>
                    32));
            #endif 
            ap3 = xalloc(sizeof(AMODE));
            ap3->mode = am_seg;
            ap3->seg = seg;
            gen_code2(op_mov, BESZ_WORD, BESZ_DWORD , ap3, ap2);
            gen_code(op_mov, ap2, make_immed(ap->offset->v.i
                &0xffffffff));
			ap2->seg = seg;
        }
		else if (ap->mode == am_axdx)
		{
			AMODE *ap3;
            ap3 = xalloc(sizeof(AMODE));
            ap3->mode = am_seg;
            ap3->seg = seg;
			gen_code2(op_mov, BESZ_WORD, BESZ_DWORD, ap3, makedreg(EDX));
			if (ap2->preg != EAX)
				gen_code(op_mov, ap2, makedreg(EAX));
			ap2->seg = seg;
		}
        else
        {
            ap2->length = BESZ_DWORD ;
            ap->length = 0;
            gen_code(ldx[seg], ap2, ap);
            ap->length = BESZ_DWORD ;
        }
		return ap2;
    }
    else if (ap->mode != am_dreg)
    {
        if (isize == BESZ_QWORD || isize ==  - BESZ_QWORD)
	        if (ap->mode == am_axdx)
	        {
				ap2 = axdx_tolower(areg);
	        }
	        else if (ap->mode == am_immed)
	        {
				ap2 = make_immed(ap->offset->v.i & 0xffffffff);
	        }
	        else
	        {
				int n;
				ap2 = reuseop(ap, areg, &n);
	            if (seg == 0)
	                ap2->length = ap->length = BESZ_DWORD ;
	            else
	            {
	                ap2->length = BESZ_DWORD ;
	                ap->length = 0;
	            }
	            gen_code(ldx[seg], ap2, ap);
				FreeUnused(n);
	        }
        else
        {
			int n;
			ap2 = reuseop(ap, areg, &n);
            if (seg == 0)
                ap2->length = ap->length = osize;
            else
            {
                ap2->length = BESZ_DWORD ;
                ap->length = 0;
            }
            gen_code(ldx[seg], ap2, ap);
            if (seg)
                ap->length = BESZ_DWORD ;
			FreeUnused(n);
        }
//	    ap2->length = BESZ_DWORD ;
		return ap2;
	}
	else if (ap->preg < EBX)
	{
		ap->length = osize;
		return ap;
	}
	else
	{
		ap2 = RealizeDreg(areg);
		gen_code(op_mov, ap2, ap);
		ap2->length = osize;
		return ap2;
	}
}
AMODE *FloatToHigher(AMODE *ap, int areg, int isize, int osize)
{
    if (isize >= BESZ_CFLOAT) {
        complexload(ap,TRUE);
    } else if (isize >=BESZ_IFLOAT)
        if (osize >= BESZ_CFLOAT)
        {
            if (ap->mode != am_freg)
                floatload(ap, TRUE);
            gen_codef(op_fldz,0,0);
            ap->mode = am_frfr;
            ap->length = BESZ_CLDOUBLE;
        }
        else if (osize < BESZ_IFLOAT)
        {
            if (ap->mode == am_freg)
                gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0);
            gen_codef(op_fldz,0,0) ;
            ap->mode = am_freg;
            ap->length = BESZ_LDOUBLE;
        }
        else 
        {
            if (ap->mode != am_freg) 
            {
                floatload(ap, TRUE);
                ap->length = osize;
            }
        }
    else if (osize >= BESZ_CFLOAT)
    {
        gen_codef(op_fldz,0,0);
        if (ap->mode != am_freg)
            floatload(ap, TRUE);
        else
            gen_codef(op_fxch,0,0);
        ap->mode = am_frfr;
        ap->length = BESZ_CLDOUBLE;
    }
    else if (osize >= BESZ_IFLOAT)
    {
        if (ap->mode == am_freg)
                gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0);
        gen_codef(op_fldz,0,0);
        ap->mode = am_freg;
        ap->length = osize;
    }
    else
    {
        if (ap->mode != am_freg)
            floatload(ap, TRUE);
        ap->mode = am_freg;
        ap->length = osize;
    }
	return ap;
}
AMODE *QuadToHigher(AMODE *ap, int areg, int isize, int osize)
{	
	AMODE *ap1;
    if (ap->mode == am_immed)
    {
        gen_codes(op_mov, BESZ_DWORD, makedreg(EDX),
                  make_immed((ap->offset->v.i >> 32) & 0xffffffff));
        gen_codes(op_mov, BESZ_DWORD, makedreg(EAX),
                  make_immed(ap->offset->v.i & 0xffffffff));
        ap->mode = am_axdx;
    }
    if (ap->mode == am_axdx)
    {
        ap1 = floatconvpos();
        gen_codes(op_mov, BESZ_DWORD, ap1, makedreg(EAX));
        ap1->offset = makenode(en_add, ap1->offset, makeintnode(en_icon, 4));
        gen_codes(op_mov, BESZ_DWORD, ap1, makedreg(EDX));
        ap1->offset = ap1->offset->v.p[0];
    }
    else
    {
        ap1 = ap;
    }
    if (osize >= BESZ_CFLOAT) 
    {
        gen_codef(op_fldz,0,0);
        gen_codes(op_fild, BESZ_DOUBLE, ap1, 0);
        ap->mode = am_frfr;
        ap->length = BESZ_CLDOUBLE;
    }
    else if (osize >= BESZ_IFLOAT)
    {
        gen_codef(op_fldz,0,0);
        ap->mode = am_freg;
        ap->length = osize;
    } 
    else 
    {
        gen_codes(op_fild, BESZ_DOUBLE, ap1, 0);
        ap->mode = am_freg;
        ap->length = osize;
    }
#if 0
    if (isize > 0)
    {
        AMODE *ap = floatconvpos();
        gen_codef(op_ftst, 0, 0);
        gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
        gen_codef(op_fwait, 0, 0);
        gen_code(op_sahf, 0, 0);
        gen_code(op_jae, make_label(nextlabel), 0);
        gen_codes(op_mov, BESZ_DWORD, ap, make_immed(0));
        ap->offset = makenode(en_add, ap->offset, makeintnode(en_icon, 4));
        gen_codes(op_mov, BESZ_DWORD, ap, make_immed(1<<31));
        ap->offset = ap->offset->v.p[0];
        gen_codefs(op_fild, BESZ_DOUBLE, ap, 0);
        gen_codef(op_fsub, makefreg(1), makefreg(0));
        gen_codef(op_fsubp, makefreg(1), 0);
        gen_label(nextlabel++);
    }
#endif
	return ap;
}
AMODE *FarPtrToHigher(AMODE *ap, int areg, int isize, int osize)
{
	AMODE *ap3;
    if (ap->mode == am_dreg)
    {
        if (ap->preg != EAX)
            gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), ap);
        ap3 = xalloc(sizeof(AMODE));
        ap3->mode = am_seg;
        ap3->seg = ap->seg;
        gen_codes(op_mov, BESZ_DWORD , makedreg(EDX), ap3);
    }
    else if (ap->mode == am_immed)
    {
        #if sizeof(ULLONG_TYPE) == 4
            gen_codes(op_mov, BESZ_DWORD , makedreg(EDX), make_immed(0));
        #else 
            gen_codes(op_mov, BESZ_DWORD , makedreg(EDX), make_immed((ap
                ->offset->v.i >> 32) &0xffffffff));
        #endif 
        gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), make_immed(ap->offset
            ->v.i &0xffffffff));
    }
    else
    {
        ap3 = copy_addr(ap);
        ap3->offset = makenode(en_add, ap3->offset, makeintnode
            (en_icon, 4));
        gen_code2(op_movzx, BESZ_DWORD , BESZ_WORD, makedreg(EDX), ap3);
        gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), ap);
    }
    ap->seg = 0;
    ap->mode = am_axdx;
	if (osize > BESZ_QWORD)
		QuadToHigher(ap, areg, BESZ_QWORD, osize);
}
AMODE *xIntToFloat(AMODE *ap, int areg, int isize, int osize)
{
	AMODE *ap1, *ap2;
    if (osize >= BESZ_IFLOAT && osize < BESZ_CFLOAT) 
    {
        gen_codef(op_fldz,0,0);
        ap->mode = am_freg;
        ap->length = osize;
    }
    else
    {
        if (isize != BESZ_DWORD  && isize !=  - BESZ_DWORD )
        {
			int n;
            ap1 = floatconvpos();
			ap2 = reuseop(ap, areg, &n);
            if (ap->mode == am_seg || ap2->mode == am_seg)
                gen_codes(op_mov, BESZ_DWORD , ap2, ap);
            else if (isize < 0)
                gen_code2(op_movsx, BESZ_DWORD , isize, ap2, ap);
            else
                gen_code2(op_movzx, BESZ_DWORD , isize , ap2, ap);
			FreeUnused(n);
            gen_codes(op_mov, BESZ_DWORD , ap1, ap2);
            gen_codefs(op_fild, BESZ_DWORD, ap1, 0);
        }
        else
            if (ap->mode == am_direct || ap->mode == am_indisp || ap
                ->mode == am_indispscale)
                gen_codefs(op_fild, BESZ_DWORD , ap, 0);
            else
	        {
	            ap1 = floatconvpos();
	            gen_codes(op_mov, BESZ_DWORD , ap1, ap);
	            gen_codefs(op_fild, BESZ_DWORD , ap1, 0);
	        }
#if 0
        if (isize > 0)
        {
            AMODE *ap = floatconvpos();
            gen_codef(op_ftst, 0, 0);
            gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
            gen_codef(op_fwait, 0, 0);
            gen_code(op_sahf, 0, 0);
            gen_code(op_jae, make_label(nextlabel), 0);
            gen_codes(op_mov, BESZ_DWORD, ap, make_immed(1<<31));
            gen_codefs(op_fild, BESZ_DWORD, ap, 0);
            gen_codef(op_fsub, makefreg(1), makefreg(0));
            gen_codef(op_fsubp, makefreg(1), 0);
            gen_label(nextlabel++);
        }
#endif
        if (osize >= BESZ_CFLOAT)
        {
            gen_codef(op_fldz,0,0);
            gen_codef(op_fxch,0,0);
            ap->mode = am_frfr;
            ap->length = BESZ_CLDOUBLE;
        }
        else
        {
            ap->mode = am_freg;
            ap->length = osize;
        }
    }
	return ap;
}
AMODE *IntToHigher(AMODE *ap, int areg, int isize, int osize)
{
	AMODE *ap2;
	if (osize > BESZ_QWORD)
    {
		ap = xIntToFloat(ap, areg, isize, osize);	
    }
    else
    {
        int size6 = osize == BESZ_QWORD || osize ==  - BESZ_QWORD, oosize = osize;
			int n = -1;
		if (size6)
		{
            osize = BESZ_DWORD ;
            reuseaxdx(ap);
			ap2 = makedreg(EAX);
		}
		else
		{
			ap2 = reuseop(ap, areg, &n);
		}
        if (ap->mode == am_dreg && ap->preg > 3 && (isize == BESZ_BYTE || isize
            ==  - BESZ_BYTE))
            gen_codes(op_mov, BESZ_DWORD , ap2, ap);
        if (size6 && (isize == BESZ_DWORD  || isize ==  - BESZ_DWORD ))
		{
            gen_codes(op_mov, isize, ap2, ap);
		}
        else if (ap->mode == am_seg)
        {
            gen_codes(op_mov, BESZ_DWORD , ap2, ap);
        }
        else if (isize ==  - BESZ_DWORD  || isize == BESZ_DWORD)
        {
            gen_codes(op_mov, BESZ_DWORD , ap2, ap);
            ; // nothing here
        }
        else
        {
            if (osize ==  BESZ_FARPTR)
                osize = BESZ_DWORD ;
            if (ap->mode == am_seg || ap2->mode == am_seg)
                gen_codes(op_mov, BESZ_DWORD , ap2, ap);
            else if (isize == osize || isize == -osize || ap->mode == am_immed)
				gen_codes(op_mov, osize, ap2, ap);
            else if (isize < 0)
                gen_code2(op_movsx, osize, isize, ap2, ap);
            else
                gen_code2(op_movzx, osize, isize, ap2, ap);
        }
        if (size6)
        {
            if (ap2->preg != EAX)
                gen_codes(op_mov, BESZ_DWORD , makedreg(EAX), ap2);
            if (isize < 0)
                gen_code(op_cdq, 0, 0);
            else
                gen_codes(op_sub, BESZ_DWORD , makedreg(EDX), makedreg(EDX));
            ap->mode = am_axdx;
            ap->length = oosize;
            ap->seg = 0;
        }
        else
        {
            ap->seg = 0;
            ap->mode = am_dreg;
            ap->preg = ap2->preg;
            ap->tempflag = ap2->tempflag;
			ap->length = osize;
        }
		FreeUnused(n);
    }
	return ap;
}
AMODE *Normalize(AMODE *ap, int areg, int sreg, int isize, int osize, int flags)
{
	if (flags & F_MEM)
		if (ap->mode == am_direct || ap->mode == am_indisp || ap->mode == am_indispscale)
			return ap;
	if (flags & F_IMMED)
		if (ap->mode == am_immed || ap->mode == am_fconst)
			return ap;
	if ((flags & F_COMPLEX) && (flags & F_VOL))
	{
		if (ap->mode != am_frfr)
		{
			ap = FloatToHigher(ap, areg, isize, osize);
		}
		return ap;
			
	}
	if (flags & F_FREG)
	{
		if (ap->mode == am_freg || ap->mode == am_frfr)
			return ap;
		if (osize >= BESZ_FLOAT || !(flags &(F_DREG | F_AXDX)))
		{
			if (ap->length < BESZ_FLOAT)
			{
				if (ap->length == BESZ_QWORD || ap->length == - BESZ_QWORD)
				{
					ap = QuadToHigher(ap, areg, isize, osize);
				}
				else
				{
					ap = IntToHigher(ap, areg, isize, osize);
				}
			}
			else
			{
	            if (ap->mode == am_immed)
	                make_floatconst(ap);
			if (ap->length <= BESZ_LDOUBLE)
				floatload(ap, 1);
			}
			return ap;
		}
	}
	if (osize == BESZ_FARPTR)
	{
		if (ap->mode == am_direct || ap->mode == am_indisp || ap->mode == am_indispscale)
		{
			AMODE *ap2;
			ChooseRegs(&areg, &sreg);
			ap2 = RealizeDreg(areg);
			RealizeSreg(sreg);
			ap2->seg = sreg;
			ap2->length = BESZ_DWORD;
            ap->length = 0;
            gen_code(ldx[sreg], ap2, ap);
            ap->length = BESZ_DWORD ;
			ap = ap2;
		}
		else if (ap->mode == am_immed)
		{
			ap->seg = defseg(ap->offset);
			ap->length = BESZ_FARPTR;
		}
		return ap;
	}
	if (flags & F_AXDX)
	{
		if (ap->mode == am_axdx)
			return ap;
		if (ap->mode == am_freg)
		{
			ap = FloatToInt(areg, isize, osize);
		}
		else if (ap->length >= BESZ_FLOAT)
		{
			floatload(ap, 0);
			ap = FloatToInt(areg, isize, osize);
		}
		if (osize == BESZ_QWORD || osize == - BESZ_QWORD || (!(flags & F_DREG)))
		{
			loadaxdx(ap);
			ap->length = osize;
			return ap;
		}
	}
	if (flags & F_DREG /*&& osize != BESZ_BOOL*/)
	{
		AMODE *ap2;
	    if (ap->mode == am_screg || ap->mode == am_sdreg || ap->mode == am_streg ||
	        ap->mode == am_seg)
	    {
	        ap2 = RealizeDreg(areg);
	        gen_codes(op_mov, BESZ_DWORD , ap2, ap);
	        ap->mode = ap2->mode;
	        ap->tempflag = ap2->tempflag;
	        ap->length = ap2->length;
	        ap->preg = ap2->preg;
	        ap->seg = 0;
	    }
		else if (ap->mode == am_dreg && (!(flags & F_VOL) || ap->preg < EBX))
		{
			ap->length = osize;
			return ap;
		}
		else if (ap->mode == am_dreg || ap->mode == am_immed || ap->length <= BESZ_DWORD || ap->length >= - BESZ_DWORD)
		{
			int n = -1;
			if (ap->mode == am_indisp || ap->mode == am_indispscale)
				ap2 = reuseop(ap, areg, &n);
			else
				ap2 = RealizeDreg(areg);
			ap2->length = ap->length;
			gen_code(op_mov, ap2, ap);
			ap = ap2;
			ap->length = osize;
			FreeUnused(n);
		}
		else if (ap->length == BESZ_QWORD || ap->length == - BESZ_QWORD)
		{
			if (ap->mode == am_axdx)
			{
				ap = makedreg(EAX);
			}
			else
			{
				int n = -1;
				if (ap->mode == am_indisp || ap->mode == am_indispscale)
					ap2 = reuseop(ap, areg, &n);
				else
					ap2 = RealizeDreg(areg);
				gen_codes(op_mov, BESZ_DWORD, ap2, ap);
				FreeUnused(n);
				ap = ap2;
			}
				
		}
		else if (ap->mode == am_freg)
		{
			ap2 = FloatToLower(ap, areg, isize, osize);
			ap = ap2;
		}
		else if (ap->length >= BESZ_FLOAT)
		{
			floatload(ap, 0);
			ap2 = FloatToLower(ap, areg, isize, osize);
			ap = ap2;
		}				
	}
    if (osize != isize && osize == BESZ_BOOL)
    {
		AMODE *ap2;
		if (ap->mode == am_freg)
		{
			ap2 = FloatToLower(ap, areg, isize, BESZ_DWORD);
		}
		else if (ap->length >= BESZ_FLOAT)
		{
			floatload(ap, 0);
			ap2 = FloatToLower(ap, areg, isize, BESZ_DWORD);
		}				
        if (ap->length != BESZ_QWORD && ap->length !=  - BESZ_QWORD)
        {
            ap2 = RealizeDreg(areg);
            gen_codes(op_mov, ap->length, ap2, ap);
            gen_codes(op_or, ap->length, ap2, ap2);
        }
        else
        {
            if (ap->mode != am_axdx)
                loadaxdx(ap);
            gen_codes(op_or, BESZ_DWORD , makedreg(EAX), makedreg(EDX));
            ap2 = RealizeDreg(areg);
        }
        ap2->length = BESZ_BYTE;
        gen_code(op_setne, ap2, 0);
        ap->mode = ap2->mode;
        ap->tempflag = ap2->tempflag;
        ap->length = osize;
        ap->preg = ap2->preg;
        ap->seg = 0;
    }
	return ap;
}
AMODE * do_extend(AMODE *ap, int areg, int sreg, int osize, int flags)
/*
 *      if isize is not equal to osize then the operand ap will be
 *      loaded into a register (if not already) and if osize is
 *      greater than isize it will be extended to match.
 */
{
    AMODE *ap2,  *ap1,  *ap3, *rv = ap;

    int isize = ap->length;
	if (ap->mode == am_immed && (flags & F_IMMED))
		return rv;
    if (isize ==  - BESZ_BOOL)
        isize = BESZ_BOOL;
    if (osize ==  - BESZ_BOOL)
        osize = BESZ_BOOL;
	if (ap->mode == am_direct || ap->mode == am_indisp || ap->mode == am_indispscale)
		if (flags & F_MEM)
			if (osize == isize || osize == -isize)
			{
				ap->length = osize;
				return rv;
			}
    if (isize && isize != osize && isize !=  - osize || osize == BESZ_CLDOUBLE && isize == BESZ_CLDOUBLE)
    {
        if (ap->mode == am_dreg && ap->preg > 3 && (osize == BESZ_BYTE || osize ==  - BESZ_BYTE))
            flags |= F_VOL;
        if (chksize(isize, osize))
        {
            /* moving to a lower type */
            if (isize > BESZ_QWORD)
            {
				rv = FloatToLower(ap, areg, isize, osize);
            }
            else
            {
				rv = IntToLower(ap, areg, sreg, isize, osize, flags);
            }
        }
        else
        {
            /* moving up in type */
            if (isize > BESZ_QWORD)
            {
				rv = FloatToHigher(ap, areg, isize, osize);
            }
            else if (isize == BESZ_QWORD || isize ==  - BESZ_QWORD)
            {
				rv = QuadToHigher(ap, areg, isize, osize);
            }
            else if (isize ==  BESZ_FARPTR)
            {
				rv = FarPtrToHigher(ap, areg, isize, osize);
            }
            else 
			{
				rv = IntToHigher(ap, areg, isize, osize);
			}
        }
    }
	
	rv = Normalize(rv, areg, sreg, isize, osize, flags);
	return rv;
}

//-------------------------------------------------------------------------

int isshort(ENODE *node)
/*
 *      return true if the node passed can be generated as a short
 *      offset.
 */
{
    return (isintconst(node->nodetype) && (node->v.i >=  - 32768L && node->v.i
        <= 32767L));
}

//-------------------------------------------------------------------------

int isbyte(ENODE *node)
/*
 *      return true if the node passed can be evaluated as a byte
 *      offset.
 */
{
    return isintconst(node->nodetype) && ( - 128 <= node->v.i && node->v.i <=
        127);
}

//-------------------------------------------------------------------------
AMODE *indx_data(AMODE *ap, int areg)
{
    AMODE *ap3 = 0;
	int n;
	if (ap->mode != am_indispscale)
    {
        DIAG("indx_data: unscaled mode");
    }
	ap3 = reuseop(ap, areg, &n);
    ap3->length = BESZ_DWORD ;
    gen_codes(op_lea, BESZ_DWORD , ap3, ap);
    ap3->seg = ap->seg;
    ap3->mode = am_indisp;
    ap3->offset = makeintnode(en_icon, 0);
	FreeUnused(n);
    return ap3;
}

AMODE *doindex(ENODE *node, int areg, int sreg, enum e_node type)
{
    AMODE *ap,  *ap3;
    ENODE node2;
    int scale;
    switch (node->nodetype)
    {
        case en_icon:
            ap = gen_expr(node, areg, sreg, FALSE, F_IMMED, BESZ_DWORD );
            break;
        case en_lsh:
            if ((scale = node->v.p[1]->v.i) < 4 && scale)
            {
                ap = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_IMMED, BESZ_DWORD );
                if (node->v.p[0]->nodetype == en_bits)
                    ap = bit_load(ap, areg, sreg, node->v.p[0]);
                if (ap->mode == am_immed)
                {
                    while (scale--)
                        ap->offset->v.i <<= 1;
                }
                else
                {
                    ap->mode = am_indispscale;
                    ap->sreg = ap->preg;
                    ap->preg =  - 1;
                    ap->scale = scale;
                    ap->offset = makeintnode(en_icon, 0);
                }
                break;
            }
        default:
            node2.v.p[0] = node;
            node2.nodetype = type;
            ap = gen_deref(&node2, areg, sreg);
            switch (ap->mode)
            {
            default:
                break;
            case am_indispscale:
                if (ap->sreg >= 0 && ap->preg >= 0)
                {
					int n;
					ChooseRegs(&areg, &sreg);
					ap3 = reuseop(ap, areg, &n);
				    ap3->length = BESZ_DWORD ;
				    gen_codes(op_lea, BESZ_DWORD , ap3, ap);
				    ap3->seg = ap->seg;
				    ap3->mode = am_indisp;
				    ap3->offset = makeintnode(en_icon, 0);
                    ap = ap3;
					FreeUnused(n);
                }
            }

            break;
    }
    return ap;
}

//-------------------------------------------------------------------------

AMODE *gen_index(ENODE *node, int areg, int sreg)
/*
 *      generate code to evaluate an index node (^+) and return
 *      the addressing mode of the result. This routine takes no
 *      flags since it always returns either am_ind or am_indx.
 */
{
    AMODE *ap1,  *ap2,  *ap,  *ap3;
    ENODE node2;

    ap1 = doindex(node->v.p[0], areg, sreg, node->nodetype);
	if (ap1->mode == am_indispscale && !isintconst(node->v.p[1]->nodetype))
	{
		int n;
		ap3 = reuseop(ap1, areg, &n);
		gen_code(op_lea, ap3, ap1);
		ap1 = ap3;
		FreeUnused(n);
		if (ap1->preg == areg)
			ChooseRegs(&areg, &sreg);
	}
	else if (ap1->mode == am_dreg || ap1->mode == am_indisp)
	{
		if (ap1->preg == areg)
			ChooseRegs(&areg, &sreg);
	}
    ap2 = doindex(node->v.p[1], areg, sreg, node->nodetype);
    switch (ap1->mode)
    {
        case am_dreg:
            switch (ap2->mode)
            {
            case am_dreg:
                ap1->sreg = ap2->preg;
                ap1->scale = 0;
                ap1->mode = am_indispscale;
                ap1->offset = makeintnode(en_icon, 0);
                if (!ap1->seg)
                    ap1->seg = ap2->seg;
                return ap1;
            case am_immed:
            case am_direct:
                ap2->preg = ap1->preg;
                ap2->mode = am_indisp;
                if (!ap2->seg)
                    ap2->seg = ap1->seg;
                return ap2;
            case am_indisp:
                ap2->sreg = ap2->preg;
                ap2->preg = ap1->preg;
                ap2->mode = am_indispscale;
                ap2->scale = 0;
                if (!ap2->seg)
                    ap2->seg = ap1->seg;
                return ap2;
            case am_indispscale:
                if (ap2->preg ==  - 1)
                {
                    ap2->preg = ap1->preg;
                    if (!ap2->seg)
                        ap2->seg = ap1->seg;
                    return ap2;
                }
                ap = indx_data(ap2, areg);
                ap->sreg = ap1->preg;
                ap->mode = am_indispscale;
                ap->scale = 0;
                ap->offset = makeintnode(en_icon, 0);
                ap->seg = ap2->seg;

                if (!ap->seg)
                    ap->seg = ap1->seg;
                return ap;
            }
            break;
        case am_direct:
        case am_immed:
            switch (ap2->mode)
            {
            case am_dreg:
                ap2->mode = am_indisp;
                ap2->offset = ap1->offset;
                if (!ap2->seg)
                    ap2->seg = ap1->seg;
                return ap2;
            case am_immed:
            case am_direct:
                if (ap1->offset->nodetype == en_icon && ap2->offset->nodetype 
                    == en_icon)
                    ap1->offset->v.i += ap2->offset->v.i;
                else
                    ap1->offset = makenode(en_add, ap1->offset, ap2->offset);
                ap1->mode = am_direct;
                if (!ap1->seg)
                    ap1->seg = ap2->seg;
                return ap1;
            case am_indisp:
            case am_indispscale:
                if (ap1->offset->nodetype == en_icon && ap2->offset->nodetype 
                    == en_icon)
                    ap2->offset->v.i += ap1->offset->v.i;
                else
                    ap2->offset = makenode(en_add, ap1->offset, ap2->offset);
                if (!ap2->seg)
                    ap2->seg = ap1->seg;
                return ap2;
            }
            break;
        case am_indisp:
            switch (ap2->mode)
            {
            case am_dreg:
                ap1->mode = am_indispscale;
                ap1->sreg = ap2->preg;
                ap1->scale = 0;
                if (!ap1->seg)
                    ap1->seg = ap2->seg;
                return ap1;
            case am_immed:
            case am_direct:
                if (ap1->offset->nodetype == en_icon && ap2->offset->nodetype 
                    == en_icon)
                    ap1->offset->v.i += ap2->offset->v.i;
                else
                {
                    ap1->offset = makenode(en_add, ap1->offset, ap2->offset);
                }
                if (!ap1->seg)
                    ap1->seg = ap2->seg;
                return ap1;
            case am_indisp:
                ap1->mode = am_indispscale;
                ap1->sreg = ap2->preg;
                ap1->scale = 0;
                if (ap1->offset->nodetype == en_icon && ap2->offset->nodetype 
                    == en_icon)
                    ap1->offset->v.i += ap2->offset->v.i;
                else
                    ap1->offset = makenode(en_add, ap1->offset, ap2->offset);
                if (!ap1->seg)
                    ap1->seg = ap2->seg;
                return ap1;
            case am_indispscale:
                if (ap2->preg ==  - 1)
                {
                    ap2->preg = ap1->preg;
                    if (ap1->offset->nodetype == en_icon && ap2->offset
                        ->nodetype == en_icon)
                        ap2->offset->v.i += ap1->offset->v.i;
                    else
                        ap2->offset = makenode(en_add, ap1->offset, ap2->offset)
                            ;
                    if (!ap2->seg)
                        ap2->seg = ap1->seg;
                    return ap2;
                }
                ap = indx_data(ap2, areg);
                ap1->sreg = ap->preg;
                ap1->scale = 0;
                ap1->mode = am_indispscale;
                if (ap1->offset->nodetype == en_icon && ap2->offset->nodetype 
                    == en_icon)
                    ap1->offset->v.i += ap2->offset->v.i;
                else
                    ap1->offset = makenode(en_add, ap1->offset, ap2->offset);
                if (!ap1->seg)
                    ap1->seg = ap2->seg;
                return ap1;
            }
            break;
        case am_indispscale:
            switch (ap2->mode)
            {
            case am_dreg:
                if (ap1->preg ==  - 1)
                {
                    ap1->preg = ap2->preg;
                    if (!ap1->seg)
                        ap1->seg = ap2->seg;
                    return ap1;
                }
                ap = indx_data(ap1, areg);
                ap->sreg = ap2->preg;
                ap->scale = 0;
                ap->offset = ap1->offset;
                ap->seg = ap1->seg;
                if (!ap->seg)
                    ap->seg = ap2->seg;
                return ap;
            case am_immed:
            case am_direct:
                if (ap1->offset->nodetype == en_icon && ap2->offset->nodetype 
                    == en_icon)
                    ap1->offset->v.i += ap2->offset->v.i;
                else
                    ap1->offset = makenode(en_add, ap1->offset, ap2->offset);
                if (!ap1->seg)
                    ap1->seg = ap2->seg;
                return ap1;
            case am_indisp:
                if (ap1->preg ==  - 1)
                {
                    ap1->preg = ap2->preg;
                    if (ap1->offset->nodetype == en_icon && ap2->offset
                        ->nodetype == en_icon)
                        ap1->offset->v.i += ap2->offset->v.i;
                    else
                        ap1->offset = makenode(en_add, ap1->offset, ap2->offset)
                            ;
                    if (!ap1->seg)
                        ap1->seg = ap2->seg;
                    return ap1;
                }
                ap = indx_data(ap1, areg);
                ap->sreg = ap2->preg;
                ap->scale = 0;
                ap->mode = am_indispscale;
                ap->seg = ap2->seg;
                if (ap1->offset->nodetype == en_icon && ap2->offset->nodetype 
                    == en_icon)
                    ap->offset->v.i = ap2->offset->v.i + ap1->offset->v.i;
                else
                    ap->offset = makenode(en_add, ap1->offset, ap2->offset);
                if (!ap->seg)
                    ap->seg = ap2->seg;
                return ap;
            case am_indispscale:
                if (ap1->preg ==  - 1 && ap2->preg ==  - 1)
                {
                    if (ap1->scale == 0)
                    {
                        ap2->preg = ap1->sreg;
                        if (ap1->offset->nodetype == en_icon && ap2->offset
                            ->nodetype == en_icon)
                            ap2->offset->v.i += ap1->offset->v.i;
                        else
                            ap2->offset = makenode(en_add, ap1->offset, ap2
                                ->offset);
                        if (!ap2->seg)
                            ap2->seg = ap1->seg;
                        return ap2;
                    }
                    else if (ap2->scale == 0)
                    {
                        ap1->preg = ap2->sreg;
                        if (ap1->offset->nodetype == en_icon && ap2->offset
                            ->nodetype == en_icon)
                            ap1->offset->v.i += ap2->offset->v.i;
                        else
                            ap1->offset = makenode(en_add, ap1->offset, ap2
                                ->offset);
                        if (!ap1->seg)
                            ap1->seg = ap2->seg;
                        return ap1;
                    }
                }
                if (ap1->preg ==  - 1)
                {
                    ap = indx_data(ap2, areg);
                    ap1->preg = ap->preg;
                    if (!ap1->seg)
                        ap1->seg = ap2->seg;
                    return ap1;
                }
                else if (ap2->preg ==  - 1)
                {
                    ap = indx_data(ap1, areg);
                    ap2->preg = ap->preg;
                    if (!ap2->seg)
                        ap2->seg = ap1->seg;
                    return ap2;
                }
                ap = indx_data(ap1, areg);
				ChooseRegs(&areg, &sreg);
                ap1 = indx_data(ap2, areg);
                ap->mode = am_indispscale;
                ap->sreg = ap1->preg;
                ap->scale = 0;
                ap->offset = makeintnode(en_icon, 0);
                ap->seg = ap1->seg;
                if (!ap->seg)
                    ap->seg = ap2->seg;

                return ap;
            }
            break;
    }
    DIAG("invalid index conversion");
    return 0;
}

//-------------------------------------------------------------------------

AMODE *gen_deref(ENODE *node, int areg, int sreg)
/*
 *      return the addressing mode of a dereferenced node.
 */
{
    AMODE *ap1;
    int siz1;
	int psize = BESZ_DWORD;
    switch (node->nodetype) /* get load size */
    {
        case en_substack:
        case en_structret:
		case en_loadstack:
		case en_savestack:
            siz1 = BESZ_DWORD ;
            break;
        case en_fp_ref:
            siz1 =  BESZ_FARPTR;
            break;
        case en_ub_ref:
            siz1 = BESZ_BYTE;
            break;
        case en_b_ref:
            siz1 =  - 1;
            break;
        case en_bool_ref:
            siz1 = BESZ_BOOL;
            break;
        case en_uw_ref:
            siz1 = BESZ_WORD;
            break;
        case en_w_ref:
            siz1 =  - BESZ_WORD;
            break;
        case en_l_ref:
		case en_i_ref:
		case en_a_ref:
            siz1 =  - BESZ_DWORD;
            break;

        case en_ul_ref:
		case en_ui_ref:
		case en_ua_ref:
            siz1 = BESZ_DWORD;
            break;
        case en_ull_ref:
	            siz1 = BESZ_QWORD;
            break;
        case en_ll_ref:
            siz1 =  - BESZ_QWORD;
            break;
        case en_floatref:
            siz1 = BESZ_FLOAT;
            break;
        case en_doubleref:
            siz1 = BESZ_DOUBLE;
            break;
        case en_longdoubleref:
            siz1 = BESZ_LDOUBLE;
            break;
        case en_fimaginaryref:
            siz1 = BESZ_IFLOAT;
            break;
        case en_rimaginaryref:
            siz1 = BESZ_IDOUBLE;
            break;
        case en_lrimaginaryref:
            siz1 = BESZ_ILDOUBLE;
            break;
        case en_fcomplexref:
            siz1 = BESZ_CFLOAT;
            break;
        case en_rcomplexref:
            siz1 = BESZ_CDOUBLE;
            break;
        case en_lrcomplexref:
            siz1 = BESZ_CLDOUBLE;
            break;
        default:
            siz1 = BESZ_DWORD ;
    }
    if (node->v.p[0]->nodetype == en_add || node->v.p[0]->nodetype ==
        en_addstruc)
    {
        ap1 = gen_index(node->v.p[0], areg, sreg);
        ap1->length = siz1;
        return ap1;
    }
    else if (node->v.p[0]->nodetype == en_autocon || node->v.p[0]->nodetype ==
        en_autoreg)
    {
        SYM *sp = node->v.p[0]->v.sp;
        int i = sp->value.i;
        ap1 = xalloc(sizeof(AMODE));
        ap1->mode = am_indisp;
        ap1->preg = EBP;
        //                if (prm_farkeyword)
            //								if ((currentfunc->value.classdata.cppflags & PF_MEMBER) &&
            //											!(currentfunc->value.classdata.cppflags & PF_STATIC) && i > 0)
            //									i += 4;
        ap1->offset = makeintnode(en_icon, i);
        ap1->length = siz1;
        return ap1;
    }
    else if (node->v.p[0]->nodetype == en_nacon || node->v.p[0]->nodetype ==
        en_napccon)
    {
        ap1 = xalloc(sizeof(AMODE));
        ap1->offset = makenode(node->v.p[0]->nodetype, (void*)node->v.p[0]
            ->v.sp, 0);
        ap1->mode = am_direct;
        ap1->length = siz1;
        return ap1;
    }
    else if (node->v.p[0]->nodetype == en_nalabcon)
    {
        ap1 = xalloc(sizeof(AMODE));
        ap1->offset = makenode(node->v.p[0]->nodetype, node->v.p[0]->v.sp, 0);
        ap1->mode = am_direct;
        ap1->length = siz1;
        return ap1;
    }
    else if (node->v.p[0]->nodetype == en_labcon)
    {
        ap1 = xalloc(sizeof(AMODE));
        ap1->mode = am_direct;
        ap1->offset = makeintnode(node->v.p[0]->nodetype, node->v.p[0]->v.i);
        ap1->length = siz1;
        return ap1;
    }
    else if (node->v.p[0]->nodetype == en_absacon)
    {
        ap1 = xalloc(sizeof(AMODE));
        ap1->mode = am_direct;
        ap1->seg = 0;
        ap1->offset = makeintnode(en_absacon, node->v.p[0]->v.sp->value.i);
        ap1->length = siz1;
        return ap1;

    }
    else if (node->v.p[0]->nodetype == en_regref)
    {
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG, siz1);
        return ap1;
    }
	if (natural_size(node->v.p[0]) == BESZ_FARPTR)
		psize = BESZ_FARPTR;
			
    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_IMMED | F_DREG | F_AXDX, psize ); /* generate address */
    if (prm_farkeyword && ap1->mode == am_axdx)
    {
        AMODE *ap2 = makedreg(0);
        ap2->seg = sreg;
        ap2->mode = am_seg;
        gen_codes(op_mov, BESZ_WORD, ap2, makedreg(EDX));
		regs[0]++;
        ap1->mode = am_dreg;
        ap1->seg = ap2->seg;
    }
    else if (ap1->mode != am_immed && psize != BESZ_FARPTR)
    {
        ap1 = do_extend(ap1, areg, sreg, BESZ_DWORD , F_DREG);
    }
    ap1->length = siz1;
    if (ap1->mode == am_dreg)
    {
        ap1->mode = am_indisp;
        ap1->offset = makeintnode(en_icon, 0);
        //                ap1->seg = defseg(node->v.p[0]) ;
        return ap1;
    }
    else if (ap1->mode == am_stackedtemp)
    {
        ap1->mode = am_stackedtempaddr;
        if (prm_farkeyword)
            ap1->seg = SS;
        return ap1;
    }
    ap1->mode = am_direct;
    return ap1;
}

//-------------------------------------------------------------------------

static int isbit(ENODE *node)
{
    return node->nodetype == en_bits;
}



//-------------------------------------------------------------------------

AMODE *gen_unary(ENODE *node, int areg, int sreg, int op, int fop)
/*
 *      generate code to evaluate a unary minus or complement.
 */
{
    AMODE *ap;
    int xchg;
	int size = natural_size(node->v.p[0]);
    if (size > BESZ_QWORD)
    {
		MarkRegs();
	    ap = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG | F_VOL, size);
		ReleaseRegs();
        if (size >= BESZ_CFLOAT)
        {
            gen_code(fop, 0, 0);
            gen_code(op_fxch, 0, 0);
            gen_code(fop, 0, 0);
            gen_code(op_fxch, 0, 0);
            ap = fstack();
            ap->mode = am_frfr;
            ap->length = BESZ_CLDOUBLE;
        }
        else if (size >= BESZ_IFLOAT)
        {
            gen_code(fop, 0, 0);
            ap = fstack();
            ap->length = BESZ_ILDOUBLE;
        }
        else 
        {
            gen_code(fop, 0, 0);
            ap = fstack();
            ap->length = BESZ_LDOUBLE;
        }
    }
    else
    {
        if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
        {
		    ap = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, size);
            gen_code(op, makedreg(EDX), 0);
            gen_code(op, makedreg(EAX), 0);
            if (op == op_neg)
                gen_code(op_sbb, makedreg(EDX), make_immed(0));
        }
        else
        {
		    ap = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL, size);
            gen_code(op, ap, 0);
        }
    }
    return ap;
}

//-------------------------------------------------------------------------

AMODE *gen_complexadd(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr)
{
    AMODE *ap1, *ap2;
	MarkRegs();
    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_VOL, natural_size(node->v.p[0]));
    if (apr)
    {
	noRelease = TRUE;
        *apr = copy_addr(ap1);
    }
	ReleaseRegs();
	MarkRegs();
	ChooseRegs(&areg, &sreg);
	ReleaseRegs();
    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_VOL, natural_size(node->v.p[1]));
    
    if (ap1->length < BESZ_CFLOAT && ap2->length < BESZ_CFLOAT)
    {
        int iap1 = ap1->mode == am_freg,iap2 = ap2->mode == am_freg ;
        if (!iap1 && !iap2) 
        {
            if (ap1->length < BESZ_IFLOAT) 
            {
                ap2 = do_extend(ap2, areg, sreg, ap2->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE, F_FREG);
                ap1 = do_extend(ap1, areg, sreg, ap1->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
            } 
            else
            {
                ap1 = do_extend(ap1, areg, sreg, ap1->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
                ap2 = do_extend(ap2, areg, sreg, ap2->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE, F_FREG);
            }
        } 
        else if (iap1 && !iap2)
        {
            ap2 = do_extend(ap2, areg, sreg, ap2->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE, F_FREG);
            if (ap1->length < BESZ_IFLOAT)
                gen_codef(op_fxch,0,0);
        } 
        else if (iap2 && !iap1)
        {
            ap1 = do_extend(ap1, areg, sreg, ap1->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE, F_FREG);
            if (ap2->length < BESZ_IFLOAT)
                gen_codef(op_fxch,0,0);
        } 
        else 
        {
            if (ap1->length < BESZ_IFLOAT)
                gen_codef(op_fxch,0,0);
        }
        ap1 = fstack();
        ap1->mode = am_frfr;
        ap1->length = BESZ_CLDOUBLE;
        return ap1;
    } 
    else if (ap1->length < BESZ_CFLOAT && ap2->length >= BESZ_CFLOAT)
    {
        int iap1 = ap1->mode == am_freg,iap2 = ap2->mode == am_frfr ;
        if (!iap1 && !iap2) 
        {
            ap1 = do_extend(ap1, areg, sreg, ap1->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
            ap2 = do_extend(ap2, areg, sreg, BESZ_CLDOUBLE, 0);
            if (ap1->length >= BESZ_IFLOAT) 
            {
                gen_codef(op_fxch,0,0);
                gen_codef(fopp,makefreg(2),0);
            } 
            else 
            {
                gen_codef(fopp,makefreg(2),0);
                gen_codef(op_fxch,0,0);
            }
        } 
        else if (iap1 && !iap2)
        {
            ap2 = do_extend(ap2, areg, sreg, BESZ_CLDOUBLE,0);
            if (ap1->length >= BESZ_IFLOAT) 
            {
                gen_codef(op_fxch,0,0);
                gen_codef(fopp,makefreg(2),0);
            } 
            else 
            {
                gen_codef(fopp,makefreg(2),0);
                gen_codef(op_fxch,0,0);
            }
        } 
        else if (iap2 && !iap1)
        {
            ap1 = do_extend(ap1, areg, sreg, ap1->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE, F_FREG);
            gen_codef(op_fxch,makefreg(2),0);
            if (ap1->length >= BESZ_IFLOAT) 
            {
                gen_codef(fopp,makefreg(2),0);
            } 
            else 
            {
                gen_codef(op_fxch,0,0);
                gen_codef(fopp,makefreg(2),0);
                gen_codef(op_fxch,0,0);
            }
        } 
        else 
        {
            if (ap1->length >= BESZ_IFLOAT) 
            {
                gen_codef(op_fxch,0,0);
                gen_codef(fopp,makefreg(2),0);
            } 
            else 
            {
                gen_codef(fopp,makefreg(2),0);
                gen_codef(op_fxch,0,0);
            }
        }
        return ap2;
    } 
    else if (ap1->length >= BESZ_CFLOAT && ap2->length < BESZ_CFLOAT)
    {
        int iap1 = ap1->mode == am_frfr,iap2 = ap2->mode == am_freg ;
        if (!iap1 && !iap2) 
        {
            ap1 = do_extend(ap1, areg, sreg, BESZ_CLDOUBLE, 0);
            ap2 = do_extend(ap2, areg, sreg, ap2->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
            if (ap2->length >= BESZ_IFLOAT) 
            {
                gen_codef(fopp,makefreg(2),0);
            } 
            else 
            {
                gen_codef(fopp,makefreg(1),0);
            }
        } 
        else if (iap1 && !iap2)
        {
            ap2 = do_extend(ap2, areg, sreg, ap2->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
            if (ap2->length >= BESZ_IFLOAT) 
            {
                gen_codef(fopp,makefreg(2),0);
            } 
            else 
            {
                gen_codef(fopp,makefreg(1),0);
            }
        } 
        else if (iap2 && !iap1)
        {
            ap1 = do_extend(ap1, areg, sreg, BESZ_CLDOUBLE, 0);
            if (ap2->length >= BESZ_IFLOAT) 
            {
                gen_codef(fop,makefreg(2),0);
                gen_codef(op_fxch,makefreg(2),0);
            } 
            else 
            {
                gen_codef(fop,makefreg(1),0);
                gen_codef(op_fxch,makefreg(1),0);
            }
            gen_codefs(op_fstp,BESZ_FLOAT,makefreg(0),0);
        } 
        else 
        {
            if (ap2->length >= BESZ_IFLOAT) 
            {
                gen_codef(fopp,makefreg(2),0);
            } 
            else 
            {
                gen_codef(fopp,makefreg(1),0);
            }
        }
        return ap1;
    } 
    else
    {
        ap1 = do_extend(ap1, areg, sreg, BESZ_CLDOUBLE, 0);
        ap2 = do_extend(ap2, areg, sreg, BESZ_CLDOUBLE, 0);
        gen_codef(fopp, makefreg(2),0);
        gen_codef(fopp, makefreg(2),0);
    }
	ReleaseRegs();
    return ap1;
}
AMODE *gen_fbinary(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr)
{
    AMODE *ap1,  *ap2;
    int size = natural_size(node);
    int sz1 = natural_size(node->v.p[0]), sz2 = natural_size(node->v.p[1]) ;
    if (sz1 <= BESZ_LDOUBLE)
        sz1 = BESZ_LDOUBLE;
    else
        sz1 = BESZ_ILDOUBLE;
    if (sz2 <= BESZ_LDOUBLE)
        sz2 = BESZ_LDOUBLE;
    else
        sz2 = BESZ_ILDOUBLE;
	MarkRegs();
    if (apr)
    {
      AMODE *ap;
      int sz = natural_size(node->v.p[0]);
      ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_FREG, size);
	if (sz2 <= BESZ_QWORD)
	{
		ChooseRegs(&areg, &sreg);
		ap2 = do_extend(ap2, areg, sreg, sz1, F_FREG | F_VOL);
	}
	ReleaseRegs();
      ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, size);
      gen_codes(op_lea, BESZ_DWORD, *apr = ap = makedreg(ECX), ap1);
      if (sz > - BESZ_DWORD && sz < BESZ_DWORD)
          GenPush(ECX, am_dreg, 0);
      if (isbit(node->v.p[0]))
          ap1 = bit_load(ap1, areg, sreg, node->v.p[0]);
      else
      {
          ap->length = sz;
          ap->mode = am_indisp;
          ap1 = copy_addr(ap);
      }
      ap1 = do_extend(ap1, areg, sreg, size, F_FREG);
      if (sz > - BESZ_DWORD && sz < BESZ_DWORD)
          GenPop(ECX, am_dreg, 0);
    }
    else
    {
      ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG, size);
	ChooseRegs(&areg, &sreg);
      ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_FREG | F_MEM, size);
	if (sz2 <= BESZ_QWORD)
	{
		ChooseRegs(&areg, &sreg);
		ap2 = do_extend(ap2, areg, sreg, sz1, F_FREG | F_VOL);
	}
    }
    if (ap1->mode == am_immed)
        make_floatconst(ap1);
	if (ap1->mode == am_fconst)
		floatload(ap1, 1);
    if (ap2->mode == am_immed)
        make_floatconst(ap2);
	if (ap2->mode == am_fconst)
		floatload(ap2, 1);
    if (ap2->length == BESZ_LDOUBLE)
        ap1 = do_extend(ap1, areg, sreg, sz1, F_FREG | F_VOL);
    if (ap1->mode == am_freg)
    {
        int op = apr ? foppr : fopp;
        if (ap2->mode == am_freg)
        {
            gen_codef(op, makefreg(1), 0);
        }
        else if (ap2->length == BESZ_LDOUBLE)
        {
            ap2 = do_extend(ap2, areg, sreg, sz2, F_FREG | F_VOL);
            gen_codef(op, makefreg(1), 0);
        }
        else if (ap2->mode == am_dreg)
        {
            ap2 = do_extend(ap2, areg, sreg, sz2, F_FREG | F_VOL);
            gen_codef(op, makefreg(1), 0);
        }
        else if (ap2->length < BESZ_FLOAT)
        {
            ap2 = do_extend(ap2, areg, sreg, sz2, F_FREG | F_VOL);
            gen_codef(op, makefreg(1), 0);
        }
        else
            gen_code(fop, ap2, 0);
    }
    else if (ap2->mode == am_freg)
    {
        if (ap1->length == BESZ_LDOUBLE)
        {
            ap1 = do_extend(ap1, areg, sreg, sz1, F_FREG | F_VOL);
            gen_codef(foppr, makefreg(1), 0);
        }
        else if (ap1->mode == am_dreg)
        {
            ap1 = do_extend(ap1, areg, sreg, sz1, F_FREG | F_VOL);
            gen_codef(foppr, makefreg(1), 0);
        }
        else if (ap1->length < BESZ_FLOAT)
        {
            ap1 = do_extend(ap1, areg, sreg, sz1, F_FREG | F_VOL);
            gen_codef(fopp, makefreg(1), 0);
        }
        else
            gen_code(fopr, ap1, 0);
    }
	ReleaseRegs();
    ap1 = fstack();
    return ap1;
}

//-------------------------------------------------------------------------

int xlvalue(ENODE *node)
{
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
			switch(node->v.p[0]->nodetype)
			{
				case en_autocon:
				case en_labcon:
				case en_napccon:
				case en_nacon:
					return 1;
				default:
					return 0;
			}
			break;
		default:
			return 0;
	}
}
AMODE *gen_64bin(ENODE *node, int areg, int sreg, int op, int size)
{
    AMODE *ap1,  *ap2,  *ap3,  *ap4;

	if (op != op_sub && (isintconst(node->v.p[0]->nodetype) || xlvalue(node->v.p[0]) ))
	{
	    ap1 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_VOL, size);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
    	ap2 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_IMMED | F_MEM, size);
	}
	else 
	{
		if (isintconst(node->v.p[1]->nodetype) || xlvalue(node->v.p[1]))
		{
		    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, size);
			MarkRegs();
			ChooseRegs(&areg, &sreg);
    		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_IMMED | F_MEM, size);
		}
		else
		{
			MarkRegs();
		    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, size);
			ReleaseRegs();
			GenPush(EDX, am_dreg, FALSE);
			GenPush(EAX, am_dreg, FALSE);
			MarkRegs();
		    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_VOL, size);
	        ap3 = make_stack(0);
	        ap4 = make_stack( - 4);
		    gen_code(op, ap3, makedreg(EAX));
		    if (op == op_add)
		        gen_code(op_adc, ap4, makedreg(EDX));
		    else if (op == op_sub)
		        gen_code(op_sbb, ap4, makedreg(EDX));
		    else
		        gen_code(op, ap4, makedreg(EDX));
			ReleaseRegs();
			GenPop(EAX, am_dreg, FALSE);
			GenPop(EDX, am_dreg, FALSE);
			ap1 = temp_axdx();			
			ap1->length = size;			
			return ap1;			
		}
	}
    if (ap2->mode == am_immed)
    {
        ap3 = make_immed(ap2->offset->v.i &0xffffffffL);
        #if sizeof(ULLONG_TYPE) == 4
            ap4 = make_immed(ap2->offset->v.i < 0 ?  - 1: 0);
        #else 
            ap4 = make_immed(ap2->offset->v.i >> 32);
        #endif 
	    gen_code(op, makedreg(EAX), ap3);
	    if (op == op_add)
	        gen_code(op_adc, makedreg(EDX), ap4);
	    else if (op == op_sub)
	        gen_code(op_sbb, makedreg(EDX), ap4);
	    else
	        gen_code(op, makedreg(EDX), ap4);
    }
    else
    {
        ap2->length = BESZ_DWORD ;
        ap3 = ap2;
        ap4 = copy_addr(ap3);
        ap4->offset = makenode(en_add, ap4->offset, makeintnode(en_icon, 4));
		noRelease = TRUE;
	    gen_code(op, makedreg(EAX), ap3);
	    if (op == op_add)
	        gen_code(op_adc, makedreg(EDX), ap4);
	    else if (op == op_sub)
	        gen_code(op_sbb, makedreg(EDX), ap4);
	    else
	        gen_code(op, makedreg(EDX), ap4);
    }
	ReleaseRegs();
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *gen_segadd(ENODE *node, int areg, int sreg)
{
    ENODE *seg,  *ofs;
    AMODE *ap1,  *ap2,  *ap3;
    if (node->v.p[0]->nodetype == en_csp)
    {
        seg = node->v.p[0];
        ofs = node->v.p[1];
    }
    else
    {
        seg = node->v.p[1];
        ofs = node->v.p[0];
    }
    ap2 = gen_expr(ofs, areg, sreg, FALSE, F_DREG | F_VOL, BESZ_DWORD );
	MarkRegs();
	ChooseRegs(&areg, &sreg);
    ap1 = gen_expr(seg, areg, sreg, FALSE, F_IMMED | F_DREG | F_VOL, BESZ_DWORD );
    if (ap1->mode == am_seg)
    {
        ap2->seg = ap1->seg;
        ap2->length =  BESZ_FARPTR;
    }
    else
    {
        if (ap1->mode == am_immed)
        {
			gen_code(op_push, ap1, NULL);
			RealizeSreg(sreg);
			GenPop(sreg + 24, am_dreg, 0);
			ap2->seg = sreg;
        }
        else
        {
			AMODE *sseg = makedreg(0);
			sseg->mode = am_seg;
			sseg->seg = sreg;
			RealizeSreg(sreg);
			gen_code(op_mov, sseg, ap1);
			ap2->seg = sreg;
        }
        ap2->length =  BESZ_FARPTR;
    }
	ReleaseRegs();
    return ap2;
}

//-------------------------------------------------------------------------

AMODE *gen_xbin(ENODE *node, int areg, int sreg, int op, int op2)
/*
 *      generate code to evaluate a restricted binary node and return 
 *      the addressing mode of the result.
 *		  restriction is: integers only.  We also use this for add,sub,mul
 */
{
    AMODE *ap1,  *ap2,  *ap3,  *ap4;
    int nsize = natural_size(node);
    if (prm_farkeyword && (op == op_add && (node->v.p[0]->nodetype == en_csp ||
        node->v.p[1]->nodetype == en_csp)))
    {
        return gen_segadd(node, areg, sreg);
	}
    if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)	        
		return gen_64bin(node, areg, sreg, op, nsize);
	if (op != op_sub && isintconst(node->v.p[0]->nodetype))
	{
		ap1 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_VOL, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		ap2 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_IMMED | F_DREG | F_MEM, nsize);
	}
	else
	{
		ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG | F_MEM, nsize);
	}
    if (ap1->length ==  BESZ_FARPTR)
    {
        ap3 = copy_addr(ap1);
        ap3->length = BESZ_DWORD ;
    }
    else
        ap3 = ap1;
    if (ap2->length ==  BESZ_FARPTR)
    {
        ap4 = copy_addr(ap2);
        ap4->length =  BESZ_DWORD ;
    }
    else
        ap4 = ap2;
    gen_code(op, ap3, ap4);
	ReleaseRegs();
    return (ap1);
}

//-------------------------------------------------------------------------

AMODE *gen_binary(ENODE *node, int areg, int sreg, int op, int fopp, int fop, int foppr, int fopr)
/*
 *      generate code to evaluate a binary node and return 
 *      the addressing mode of the result.
 */
{
	int size = natural_size(node);
    if (size > BESZ_QWORD) 
    {
        int n1 = natural_size(node->v.p[0]), n2 = natural_size(node->v.p[1]);
        if (n1 < BESZ_IFLOAT && n2 < BESZ_IFLOAT)
            return gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 >=BESZ_IFLOAT && n2 < BESZ_CFLOAT) 
        {
            AMODE *ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
            ap1->length = BESZ_ILDOUBLE;
            return ap1;
        } 
		else
            return gen_complexadd(node, areg, sreg, fop, fopp, fopr, foppr, 0);
    }
    else
        return gen_xbin(node, areg, sreg, op, 0);
}

//-------------------------------------------------------------------------

void doshift(AMODE *ap1, AMODE *ap2, int op, int div, int size)
{
    AMODE *ecx = makedreg(ECX),  *ap3;
    if (div)
    {
        switch (size)
        {
            case BESZ_BYTE:
                case  - 1: gen_code(op_test, ap1, make_immed(0x80));
                gen_code(op_je, make_label(nextlabel), 0);
				break;
            case BESZ_WORD:
                case  - 2: gen_code(op_test, ap1, make_immed(0x8000));
                gen_code(op_je, make_label(nextlabel), 0);
                break;
            case BESZ_DWORD :
            case  - BESZ_DWORD : default:
                if (ap2->mode == am_immed && ap2->offset->v.i == 1 && ap1->mode
                    == am_dreg)
                {
                    gen_code(op_sar, ap1, ap2);
                    gen_code(op_jns, make_label(nextlabel), 0);
                    gen_code(op_adc, ap1, make_immed(0));
                    gen_label(nextlabel++);
                    return ;
                }
                else
                {
                    if (ap1->mode == am_dreg)
                        gen_code(op_test, ap1, ap1);
                    else
                        gen_code(op_test, ap1, make_immed(0x80000000));
                    gen_code(op_jns, make_label(nextlabel), 0);
                    break;
                }
        }
        if (ap2->mode == am_immed)
        {
            AMODE *ap3 = copy_addr(ap2);
            ap3->offset = makeintnode(en_icon, ((1 << ap2->offset->v.i) - 1));
            ap3->length = ap2->length;
            gen_code(op_add, ap1, ap3);
        }
        else
        {
            /* FIXME - this won't work. */
            gen_code(op_add, ap1, ap2);
            gen_code(op_dec, ap1, 0);
        }
        gen_label(nextlabel++);
    }
    if (ap2->mode == am_immed)
    {
        gen_codes2(op, 1, ap1, ap2);
    }
    else if (ap2->mode == am_dreg)
    {
        if (ap2->preg != ECX)
        {
            if (regs[1])
            {
                gen_codes(op_xchg, BESZ_DWORD , ap2, ecx);
				if (ap1->mode == am_dreg && ap1->preg == ECX)
	                gen_code2(op, ap1->length, 1, ap2, ecx);
				else
	                gen_code2(op, ap1->length, 1, ap1, ecx);
                gen_codes(op_xchg, BESZ_DWORD , ap2, ecx);
            }
            else
            {
                gen_codes(op_mov, BESZ_DWORD , ecx, ap2);
                if (equal_address(ap1, ap2))
                    gen_code2(op, ap1->length, 1, ap2, ecx);
                else
                    gen_code2(op, ap1->length , 1, ap1, ecx);
            }
        }
        else
            gen_codes2(op, 1, ap1, ecx);
    }
    else
    {
        if (regs[1])
            GenPush(ECX, am_dreg, 0);
        gen_codes(op_mov, 1, ecx, ap2);
        if (ap1->mode == am_dreg && ap1->preg == ECX)
        {
            AMODE *ap3 = xalloc(sizeof(AMODE));
            ap3->mode = am_indisp;
            ap3->preg = ESP;
            ap3->offset = makeintnode(en_icon, 0);
            ap3->length = ap1->length;
            gen_codes2(op, 1, ap3, ecx);
        }
        else
        {
            gen_codes2(op, 1, ap1, ecx);
        }

        if (regs[1])
            GenPop(ECX, am_dreg, 0);
    }
}

//-------------------------------------------------------------------------

AMODE *do64shift(ENODE *node, int areg, int sreg, int novalue, int op, int size, AMODE **apr)
{
    AMODE *ap1, *ap2, *ap;
	int pushed = FALSE;
    if (isintconst(node->v.p[1]->nodetype))
    {
        ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED, BESZ_DWORD );
        if (apr)
        {
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, size);
            gen_codes(op_lea, BESZ_DWORD, *apr = ap = makedreg(ECX), ap1);
            ap->length = BESZ_DWORD;
            ap->mode = am_indisp;
            ap->offset = makeintnode(en_icon, 0);
            ap1 = copy_addr(ap);
            ap1->length = size;
            ap1 = do_extend(ap1, areg, sreg, size, F_AXDX | F_VOL);
        }
        else
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, size);
        if (size == BESZ_QWORD || size ==  - BESZ_QWORD || ap1->mode == am_axdx)
        {
            if (ap2->offset->v.i >= 32)
            {
                if (op == op_shr || op == op_sar)
                {
                    gen_code(op_mov, makedreg(EAX), makedreg(EDX));
                    if (ap2->offset->v.i != 32)
                        gen_code(op, makedreg(EAX), make_immed(ap2->offset->v.i
                            - 32));
                    if (op == op_sar)
                        gen_code(op_cdq, 0, 0);
                    else
                        gen_code(op_sub, makedreg(EDX), makedreg(EDX));
                }
                else
                {
                    gen_code(op_mov, makedreg(EDX), makedreg(EAX));
                    if (ap2->offset->v.i != 32)
                        gen_code(op, makedreg(EDX), make_immed(ap2->offset->v.i
                            - 32));
                    gen_code(op_sub, makedreg(EAX), makedreg(EAX));
                }
            }
            else if (ap2->offset->v.i)
            {
                if (op == op_shr || op == op_sar)
                {
                    gen_code3(op_shrd, makedreg(EAX), makedreg(EDX), make_immed
                        (ap2->offset->v.i));
                    gen_code(op, makedreg(EDX), make_immed(ap2->offset->v.i));
                }
                else
                {
                    gen_code3(op_shld, makedreg(EDX), makedreg(EAX), make_immed
                        (ap2->offset->v.i));
                    gen_code(op, makedreg(EAX), make_immed(ap2->offset->v.i));
                }
            }
        }
    }
    else
    {
		int regax, regdx;
		AMODE *ap3;
		ap3 = temp_axdx();
		ap3->length = size;
		regax = regs[0];
		regdx = regs[2];
		regs[0] = regs[2] = 0;
    	SetFuncMode(1);
		if (regs[ECX])
		{
			GenPush(ECX, am_dreg, 0);
			pushed = regs[ECX];
			regs[ECX] = 0;
		}
		MarkRegs();
	    ap2 = gen_expr(node->v.p[1], ECX, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
		ReleaseRegs();
		gen_codes(op_push, BESZ_DWORD, ap2, 0);
		MarkRegs();
        if (apr)
        {
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, size);
            ap = copy_addr(ap1);
            ap1 = do_extend(ap1, areg, sreg, size, F_AXDX | F_VOL);
        }
        else
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, size);
		ReleaseRegs();
        GenPush(EDX, am_dreg, 0);
        GenPush(EAX, am_dreg, 0);
        if (apr)
        {
            gen_codes(op_lea, BESZ_DWORD, *apr = makedreg(ECX), ap);
            gen_codes(op_mov, BESZ_DWORD, ap = floatconvpos(), makedreg(ECX));
            (*apr)->length = BESZ_DWORD;
            (*apr)->mode = am_indisp;
            (*apr)->offset = makeintnode(en_icon, 0);
        }
        if (op == op_shr)
            call_library("__LXSHR");
        else if (op == op_sar)
            call_library("__LXSAR");
        else
            call_library("__LXSHL");
        if (apr)
            gen_codes(op_mov, BESZ_DWORD, makedreg(ECX), ap);
		if (pushed)
		{
			GenPop(ECX, am_dreg, 0);
			regs[ECX] = pushed;
		}
    	SetFuncMode(0);
		ap1 = ap3;
		regs[0] = regax;
		regs[2] = regdx;
    }
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *gen_shift(ENODE *node, int areg, int sreg, int op, int div)
/*
 *      generate code to evaluate a shift node and return the
 *      address mode of the result.
 */
{
    AMODE *ap1,  *ap2;
	int size = natural_size(node->v.p[0]);
    if (size == BESZ_QWORD || size ==  - BESZ_QWORD)	
	{
	        return do64shift(node, areg, sreg, FALSE, op, size, 0);
	}
    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL, size);
	MarkRegs();
	ChooseRegs(&areg, &sreg);
    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG, BESZ_DWORD);
    doshift(ap1, ap2, op, div, size);
	ReleaseRegs();
    return ap1;
}

void push_complex_param(ENODE *node, int areg, int sreg, int ofs, int size)
{
	AMODE *ap;
	MarkRegs();
    ap = gen_expr(node, areg, sreg, FALSE, F_VOL | F_COMPLEX, size);
    gen_codefs(op_fstp,BESZ_LDOUBLE,make_stack(-ofs),0);
    gen_codefs(op_fstp,BESZ_LDOUBLE,make_stack(-ofs-10),0);
	ReleaseRegs();
}
//-------------------------------------------------------------------------

AMODE *gen_complexdiv(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr)
{
        AMODE *ap1, *ap2;
        int n0 = natural_size(node->v.p[0]);
        int n1 = natural_size(node->v.p[1]);
        if (n1 >= BESZ_CFLOAT)
        {
            int limited = FALSE;
            if ((node->v.p[0]->cflags &DF_CXLIMITED) &&
                (node->v.p[1]->cflags &DF_CXLIMITED))
                limited = TRUE;
        
            gen_codes(op_sub,BESZ_DWORD ,makedreg(ESP),make_immed(40)) ;
			push_complex_param(node->v.p[1], areg, sreg, 20, natural_size(node));
			push_complex_param(node->v.p[0], areg, sreg, 0, natural_size(node));
            if (apr)
                *apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, n0);

            if (limited)
                call_library("__CPLXDIVLIM") ;
            else    
                call_library("__CPLXDIV") ;    
			ap1 = makefreg(0);
			ap1->mode = am_frfr;
			ap1->length = BESZ_CLDOUBLE;
            return ap1;
        }
        if (n0 < BESZ_CFLOAT && n1 < BESZ_CFLOAT)
        {
            int s1 = ap1->length, s2 = ap2->length;
            AMODE *ap3 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, apr);
            if (n1 >= BESZ_IFLOAT)
                gen_codef(op_fchs,0,0);
            if (ap3->length <= BESZ_FLOAT)
                ap3->length = BESZ_IFLOAT;
            else if (ap3->length == BESZ_DOUBLE)
                ap3->length = BESZ_IDOUBLE;
            else
                ap3->length = BESZ_ILDOUBLE;
            return ap3;
        } 
        else if (n0 >= BESZ_CFLOAT)
        {
            int iap1, iap2;
			MarkRegs();
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_VOL, n0);
            if (apr)
            {
			noRelease = TRUE;
                *apr = copy_addr(ap1);
            }
			ReleaseRegs();
			MarkRegs();
            ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_VOL, n1);
			ReleaseRegs();
            iap1 = ap1->mode == am_frfr;
            iap2 = ap2->mode == am_freg ;
            if (iap1 || !iap1 && !iap2) 
            {
                if (!iap1)
                    ap1 = do_extend(ap1, areg, sreg, BESZ_CLDOUBLE,0);
                if (!iap2)
                    ap2 = do_extend(ap2, areg, sreg, ap2->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
                if (ap2->length < BESZ_IFLOAT)
                {
                    gen_codef(op_fld,makefreg(0),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(fopp,makefreg(2),0);
                } 
                else
                {
                    gen_codef(op_fld,makefreg(0),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(op_fchs,0,0);
                    gen_codef(op_fxch,0,0);
                }
            }
            else 
            {
                ap1 = do_extend(ap1, areg, sreg, BESZ_CLDOUBLE,0);
                if (ap2->length < BESZ_IFLOAT)
                {
                    gen_codef(fop,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(foppr,makefreg(2),0);
                } 
                else
                {
                    gen_codef(fop,makefreg(2),0);
                    gen_codef(op_fchs,0,0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(foppr,makefreg(2),0);
                }
            }
            return ap1;
        } 
}

//-------------------------------------------------------------------------

void divprepare(int op, int size)
{
    AMODE *ah;
    if (op == op_idiv)
    switch (size)
    {
        case BESZ_BYTE:
            gen_code(op_cbw, 0, 0);
            break;
        case BESZ_WORD:
            gen_code(op_cwd, 0, 0);
            break;
        case BESZ_DWORD:
            gen_code(op_cdq, 0, 0);
            break;
    }
    else
    switch (size)
    {
        case BESZ_BYTE:
            ah = makedreg(4);
            gen_codes(op_sub, BESZ_BYTE, ah, ah);
            break;
        case BESZ_WORD:
            gen_codes(op_sub, BESZ_WORD, makedreg(EDX), makedreg(EDX));
            break;
        case BESZ_DWORD:
            gen_codes(op_sub, BESZ_DWORD , makedreg(EDX), makedreg(EDX));
            break;
    }
}

//-------------------------------------------------------------------------

void divintoeax(AMODE *ap1, int size, int op, int modflag)
{
    AMODE *eax = makedreg(EAX),  *edx = makedreg(EDX),  *ecx = makedreg(ECX);
    AMODE *ah = makedreg(4);
    int pushdx = FALSE, pushcx = FALSE;
    if ((ap1->mode == am_dreg || ap1->mode == am_indisp || ap1->mode ==
        am_indispscale) && ap1->preg == EDX || ap1->mode == am_indispscale &&
        ap1->sreg == EDX)
    {
        if (size > 1)
        {
/*
            if (regs[1])
            {
                GenPush(ECX, am_dreg, 1);
                pushcx = 1;
            }
*/
            gen_codes(op_mov, size, makedreg(ECX), ap1);
        }
        divprepare(op, size);
        gen_codes(op, size, ecx, 0);
/*
        if (pushcx)
            GenPop(ECX, am_dreg, 0);
*/
    }
    else
    {
        if (regs[2] && size > 1)
        {
            GenPush(EDX, am_dreg, 1);
            pushdx = 1;
        }
        divprepare(op, size);
        gen_codes(op, size, ap1, 0);
    }
    if (modflag)
	    if (size == BESZ_BYTE)
	    {
	        gen_codes(op_mov, 1, eax, ah);
	    }
	    else
		{
	        gen_code(op_mov, eax, edx);
		}
    if (pushdx)
        GenPop(EDX, am_dreg, 0);
}

//-------------------------------------------------------------------------

void dodiv(AMODE *ap1, AMODE *ap2, int op, int modflag)
{
    AMODE *eax = makedreg(EAX),  *edx = makedreg(EDX),  *ecx = makedreg(ECX);
    AMODE *ah = makedreg(4);
    int size = ap1->length, pushax = FALSE;
    ah->length = BESZ_BYTE;
    if (size < 0)
        size =  - size;
    if (ap2->mode == am_immed)
    {
        ap2 = make_muldivval(ap2);
    }
    /* divide by self */
    if (ap2->mode == am_dreg && ap1->preg == ap2->preg)
    {
        if (modflag)
            gen_codes(op_sub, ap2->length , ap2, ap2);
        else
            gen_codes(op_mov, ap2->length , ap2, make_immed(1));
        return ;
    }
    switch (ap1->preg)
    {
        case ESI:
        case EDI:
        case EBX:
            /* only gets here if /= with source reg */
        case EDX:
        case ECX:
            if (ap2->mode == am_dreg && ap2->preg == EAX)
            {
                gen_codes(op_xchg, BESZ_DWORD , ap2, ap1);
                divintoeax(ap1, size, op, modflag);
                gen_codes(op_xchg, BESZ_DWORD , ap2, ap1);
                break;
            }
            if (regs[0])
            {

                GenPush(EAX, am_dreg, 1);
                pushax = TRUE;
            }
            gen_codes(op_xchg, BESZ_DWORD , eax, ap1);
            if (ap2->mode == am_indisp && ap2->preg == EAX)
                ap2->preg = ap1->preg;
            else if (ap2->mode == am_indispscale)
            {
                if (ap2->preg == EAX)
                    ap2->preg = ap1->preg;
                if (ap2->sreg == EAX)
                    ap2->sreg = ap1->preg;
            }
            divintoeax(ap2, size, op, modflag);
            gen_codes(op_xchg, BESZ_DWORD , eax, ap1);
            if (pushax)
                GenPop(EAX, am_dreg, 0);
            break;
        case EAX:
            /* case for eax/val */
            divintoeax(ap2, size, op, modflag);
            break;
    }
}

//-------------------------------------------------------------------------

AMODE *do64div(ENODE *node, int areg, int sreg, int op, int modflag, int size, AMODE **apr)
{
    AMODE *ap1, *ap2, *ap;
    int regax, regdx, regcx;
    int pushed = 0;
	ap2 = temp_axdx();
	ap2->length = size;
	regax = regs[0];
	regdx = regs[2];
	regs[0] = regs[2] = 0;
    SetFuncMode(1);
	if (regs[ECX])
	{
		GenPush(ECX, am_dreg, 0);
		pushed = 1;
	}
	MarkRegs();
    ap1 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_VOL, size);
	ReleaseRegs();
	GenPush(EDX, am_dreg, FALSE);
	GenPush(EAX, am_dreg, FALSE);
	MarkRegs();
    if (apr)
    {
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, size);
        ap = copy_addr(ap1);
        ap1 = do_extend(ap1, areg, sreg, size, F_AXDX | F_VOL);
    }
    else
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, size);
	ReleaseRegs();
	GenPush(EDX, am_dreg, FALSE);
	GenPush(EAX, am_dreg, FALSE);
    if (apr)
    {
        gen_codes(op_lea, BESZ_DWORD, *apr = makedreg(ECX), ap);
        gen_codes(op_mov, BESZ_DWORD, ap = floatconvpos(), makedreg(ECX));
        (*apr)->length = BESZ_DWORD;
        (*apr)->mode = am_indisp;
        (*apr)->offset = makeintnode(en_icon, 0);
    }
    if (op == op_idiv)
        if (modflag)
            call_library("__LXMODS");
        else
            call_library("__LXDIVS");
    else
        if (modflag)
            call_library("__LXMODU");
        else
            call_library("__LXDIVU");
    if (apr)
        gen_codes(op_mov, BESZ_DWORD, makedreg(ECX), ap);
    SetFuncMode(0);
	if (pushed)
		GenPop(ECX, am_dreg, 0);
	regs[0] = regax;
	regs[2] = regdx;
    return ap2;
}

//-------------------------------------------------------------------------

AMODE *gen_modiv(ENODE *node, int areg, int sreg, int op, int fopp, int fop, int foppr, int fopr,
    int modflag)
/*
 *      generate code to evaluate a mod operator or a divide
 *      operator. these operations are done on only long
 *      divisors and word dividends so that the 68000 div
 *      instruction can be used.
 */
{
    AMODE *ap1,  *ap2;
    int siz1 = natural_size(node);
    if (siz1 > BESZ_QWORD)
    {
        int n1 = natural_size(node->v.p[0]), n2 = natural_size(node->v.p[1]);
        if (n1 < BESZ_IFLOAT && n2 < BESZ_IFLOAT)
            return gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 >=BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            return gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
        }
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 < BESZ_IFLOAT) 
        {
            AMODE *ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
            ap1->length = BESZ_ILDOUBLE ;
            return ap1 ;
        }
        else if (n1 <BESZ_IFLOAT && n2 >= BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            AMODE *ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
            ap1->length = BESZ_ILDOUBLE ;
            gen_codef(op_fchs,0,0);
            return ap1 ;
        }
        else
            return gen_complexdiv(node, areg, sreg, fop, fopp, fopr, foppr, 0);
    }
    if (siz1 == BESZ_QWORD || siz1 ==  - BESZ_QWORD)
        return do64div(node, areg, sreg, op, modflag, siz1, 0);
	ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL, siz1);
	MarkRegs();
	ChooseRegs(&areg, &sreg);
	ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_MEM | F_IMMED, siz1);
    dodiv(ap1, ap2, op, modflag);
	ReleaseRegs();
    return ap1;

}

//-------------------------------------------------------------------------

AMODE *gen_pdiv(ENODE *node, int areg, int sreg)
{
    return gen_modiv(node, areg, sreg, op_idiv, op_fdivp, op_fdiv, op_fdivrp, op_fdivr,
        FALSE);
}
//-------------------------------------------------------------------------

AMODE *gen_complexmul(ENODE *node, int areg, int sreg, int fop, int fopp, int fopr, int foppr, AMODE **apr)
{
        AMODE *ap1, *ap2;
        int n0 = natural_size(node->v.p[0]);
        int n1 = natural_size(node->v.p[1]);
        if (n0 >= BESZ_CFLOAT && n1 >= BESZ_CFLOAT)
        {
            int limited = FALSE;
            if ((node->v.p[0]->cflags &DF_CXLIMITED) &&
                (node->v.p[1]->cflags &DF_CXLIMITED))
                limited = TRUE;
            gen_codes(op_sub,BESZ_DWORD ,makedreg(ESP),make_immed(40)) ;
			push_complex_param(node->v.p[1], areg, sreg, 20, natural_size(node));
			push_complex_param(node->v.p[0], areg, sreg, 0, natural_size(node));
            if (apr)
                *apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, n0);
            if (limited)
                call_library("__CPLXMULLIM") ;
            else    
                call_library("__CPLXMUL") ;    
			ap1 = makefreg(0);
			ap1->mode = am_frfr;
			ap1->length = BESZ_CLDOUBLE;
            return ap1;
        }
        if (n0 < BESZ_CFLOAT && n1 < BESZ_CFLOAT)
        {
            AMODE *ap3 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, apr);
            if (ap3->length <= BESZ_FLOAT)
                ap3->length = BESZ_IFLOAT;
            else if (ap3->length == BESZ_DOUBLE)
                ap3->length = BESZ_IDOUBLE;
            else
                ap3->length = BESZ_ILDOUBLE;
            return ap3;
        } 
        else if (n0 < BESZ_CFLOAT && n1 >= BESZ_CFLOAT)
        {
            int iap1, iap2;
			MarkRegs();
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_VOL, n0);
			ReleaseRegs();
			MarkRegs();
            ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_VOL, n1);
			ReleaseRegs();
            iap1 = ap1->mode == am_freg;
            iap2 = ap2->mode == am_frfr ;
            if (iap1 || !iap1 && !iap2) 
            {
                if (!iap1)
                    ap1 = do_extend(ap1, areg, sreg, ap1->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
                if (!iap2)
                    ap2 = do_extend(ap2, areg, sreg, BESZ_CLDOUBLE,0);
                if (ap1->length < BESZ_IFLOAT)
                {
                    gen_codef(fop,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(fopp,makefreg(2),0);
                } 
                else
                {
                    gen_codef(fop,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(op_fchs,0,0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                }
            }
            else 
            {
                ap1 = do_extend(ap1, areg, sreg, ap1->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
                if (ap1->length < BESZ_IFLOAT)
                {
                    gen_codef(op_fld,makefreg(0),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(fopp,makefreg(2),0);
                } 
                else
                {
                    gen_codef(op_fld,makefreg(0),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(op_fchs,0,0);
                }
            }
            return ap2;
        } 
        else if (n0 >= BESZ_CFLOAT && n1 < BESZ_CFLOAT)
        {
            int iap1, iap2;
			MarkRegs();
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_VOL, n0);
            if (apr)
            {
			noRelease = TRUE;
                *apr = copy_addr(ap1);
            }
			ReleaseRegs();
			MarkRegs();
            ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_VOL, n1);
			ReleaseRegs();
            iap1 = ap1->mode == am_frfr;
            iap2 = ap2->mode == am_freg ;
            if (iap1 || !iap1 && !iap2) 
            {
                if (!iap1)
                    ap1 = do_extend(ap1, areg, sreg, BESZ_CLDOUBLE,0);
                if (!iap2)
                    ap2 = do_extend(ap2, areg, sreg, ap2->length < BESZ_IFLOAT ? BESZ_LDOUBLE : BESZ_ILDOUBLE,F_FREG);
                if (ap2->length < BESZ_IFLOAT)
                {
                    gen_codef(op_fld,makefreg(0),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(fopp,makefreg(2),0);
                } 
                else
                {
                    gen_codef(op_fld,makefreg(0),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(op_fchs,0,0);
                }
            }
            else 
            {
                ap1 = do_extend(ap1, areg, sreg, BESZ_CLDOUBLE,0);
                if (ap2->length < BESZ_IFLOAT)
                {
                    gen_codef(fop,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(fopp,makefreg(2),0);
                } 
                else
                {
                    gen_codef(fop,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                    gen_codef(op_fchs,0,0);
                    gen_codef(fopp,makefreg(2),0);
                    gen_codef(op_fxch,0,0);
                }
            }
            return ap1;
        } 
}

//-------------------------------------------------------------------------

AMODE *do64mul(ENODE *node, int areg, int sreg, int size, AMODE **apr)
{
    AMODE *ap1, *ap2, *ap;
    int pushed = 0;
	if (regs[ECX])
	{
		GenPush(ECX, am_dreg, 0);
		pushed = 1;
	}
    SetFuncMode(1);
	MarkRegs();
    ap1 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_VOL, size);
	ReleaseRegs();
	GenPush(EDX, am_dreg, FALSE);
	GenPush(EAX, am_dreg, FALSE);
	MarkRegs();
    if (apr)
    {
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, size);
        ap = copy_addr(ap1);
        ap1 = do_extend(ap1, areg, sreg, size, F_AXDX | F_VOL);
    }
    else
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, size);
	ReleaseRegs();
	GenPush(EDX, am_dreg, FALSE);
	GenPush(EAX, am_dreg, FALSE);
    if (apr)
    {
        gen_codes(op_lea, BESZ_DWORD, *apr = makedreg(ECX), ap);
        gen_codes(op_mov, BESZ_DWORD, ap = floatconvpos(), makedreg(ECX));
        (*apr)->length = BESZ_DWORD;
        (*apr)->mode = am_indisp;
        (*apr)->offset = makeintnode(en_icon, 0);
    }
    call_library("__LXMUL");
    if (apr)
        gen_codes(op_mov, BESZ_DWORD, makedreg(ECX), ap);
    SetFuncMode(0);
	if (pushed)
		GenPop(ECX, am_dreg, 0);
	ap2 = temp_axdx();			
	ap2->length = size;
    return ap2;
}

//-------------------------------------------------------------------------

AMODE *gen_pmul(ENODE *node, int areg, int sreg)
{
    return gen_mul(node, areg, sreg, op_mul, op_fmulp, op_fmul, op_fmulp, op_fmul);
}

//-------------------------------------------------------------------------

AMODE *gen_mul(ENODE *node, int areg, int sreg, int op, int fopp, int fop, int foppr, int fopr)
/*
 *      generate code to evaluate a multiply node. both operands
 *      are treated as words and the result is long and is always
 *      in a register so that the 68000 mul instruction can be used.
 */
{
    AMODE *ap1,  *ap2;
	int size = natural_size(node);
    if (size > BESZ_QWORD)
    {
        int n1 = natural_size(node->v.p[0]), n2 = natural_size(node->v.p[1]);
        if (n1 < BESZ_IFLOAT && n2 < BESZ_IFLOAT)
            return gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 >=BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            AMODE *ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
            gen_codef(op_fchs,0,0) ;
            return ap1;
        }
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 < BESZ_IFLOAT) 
        {
            AMODE *ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
            ap1->length = BESZ_ILDOUBLE ;
            return ap1 ;
        }
        else if (n1 <BESZ_IFLOAT && n2 >= BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            AMODE *ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, 0);
            ap1->length = BESZ_ILDOUBLE ;
            return ap1 ;
        }
        else
            return gen_complexmul(node, areg, sreg, fop, fopp, fopr, foppr, 0);
    }
    if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
        return do64mul(node, areg, sreg, size, 0);
	
	if (isintconst(node->v.p[0]->nodetype))
	{
	    ap1 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_VOL, BESZ_DWORD);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
	    ap2 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD);
	}
	else
	{
	    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL, BESZ_DWORD);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
	    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD);
	}
	ChooseRegs(&areg, &sreg);
	gen_codes(op_imul, BESZ_DWORD, ap1, ap2);
	ReleaseRegs();
    return ap1;

}

//-------------------------------------------------------------------------

AMODE *gen_hook(ENODE *node, int areg, int sreg)
/*
 *      generate code to evaluate a condition operator node (?:)
 */
{
    AMODE *ap1,  *ap2;
    int false_label, end_label;
    int sizl, sizr, xsiz, sizl1, sizr1;
	int pushed;
    sizr = natural_size(node->v.p[1]->v.p[1]);
    sizl = natural_size(node->v.p[1]->v.p[0]);
    sizl1 = sizl < 0 ?  - sizl: sizl;
    sizr1 = sizr < 0 ?  - sizr: sizr;
    if (sizl1 < sizr1)
        xsiz = sizr;
    else
        xsiz = sizl;
    false_label = nextlabel++;
    end_label = nextlabel++;
    falsejp(node->v.p[0], areg, sreg, false_label);
    node = node->v.p[1];
	if (xsiz == BESZ_QWORD || xsiz == - BESZ_QWORD)
	{
		int pushedax = regs[EAX];
		int pusheddx = regs[EDX];
		if (pushedax || pusheddx)
			regs[EAX]++, regs[EDX]++;
		temp_axdx();
		regs[EAX] = regs[EDX] = 0;
		MarkRegs();
	    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG | F_DREG | F_AXDX | F_VOL, xsiz);
		ReleaseRegs();
	    gen_codes(op_jmp, 0, make_label(end_label), 0);
	    gen_label(false_label);
		if (pushedax || pusheddx)
		{
			GenPush(EDX, am_dreg, 0);
			GenPush(EAX, am_dreg, 0);
		}
		MarkRegs();
	    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_FREG | F_DREG | F_AXDX | F_VOL, xsiz);
		ReleaseRegs();
		pushedax++, pusheddx++;
		regs[EAX] = pushedax;
		regs[EDX] = pusheddx;
	    gen_label(end_label);
	}
	else
	{
		MarkRegs();
	    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG | F_DREG | F_AXDX | F_VOL, xsiz);
		ReleaseRegs();
		if (ap1->preg != areg && ap1->mode != am_freg)
			gen_code(op_mov, makedreg(areg), ap1);
	    gen_codes(op_jmp, 0, make_label(end_label), 0);
	    gen_label(false_label);
		MarkRegs();
	    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_FREG | F_DREG | F_AXDX | F_VOL, xsiz);
		ReleaseRegs();
		RealizeDreg(areg);
		if (ap2->preg != areg && ap2->mode != am_freg)
			gen_code(op_mov, makedreg(areg), ap2);
	    gen_label(end_label);
		ap1 = makedreg(areg);
		if (xsiz >= BESZ_FLOAT)
			ap1 = makefreg(0);
		ap1->length = xsiz;
	}
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *gen_asunary(ENODE *node, int areg, int sreg, int novalue, int op, int fop)
/*
 *      generate code to evaluate a unary minus or complement of an address
 */
{
    AMODE *apr;
    int nsize = natural_size(node->v.p[0]);
    if (natural_size(node) > BESZ_QWORD)
    {
        AMODE *ap2;
		MarkRegs();
        ap2 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, nsize);
        apr = copy_addr(ap2);
		ReleaseRegs();
        if (nsize >= BESZ_CFLOAT)
        {
            complexload(apr, FALSE);
            gen_code(fop, 0, 0);
            gen_code(op_fxch, 0, 0);
            gen_code(fop, 0, 0);
            gen_code(op_fxch, 0, 0);
            return complexstore(node->v.p[0],ap2,novalue,nsize);
        }
        else 
        {
            apr = do_extend(apr, areg, sreg, nsize, F_FREG);
            gen_code(fop, 0, 0);
            return floatstore(node->v.p[0], ap2, novalue, nsize);
        }
    }
    else
    {
		AMODE *ap2;
        if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)
        {
            AMODE *ap3;
	        ap2 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_VOL, nsize);
            gen_code(op, makedreg(EDX), 0);
            gen_code(op, makedreg(EAX), 0);
            if (op == op_neg)
                gen_code(op_sbb, makedreg(EDX), make_immed(0));
			MarkRegs();
			ChooseRegs(&areg, &sreg);
		    apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, nsize);
            ap3 = copy_addr(apr);
            ap3->offset = makenode(en_add, ap3->offset, makeintnode(en_icon, 4));
			noRelease = TRUE;
			gen_codes(op_mov, BESZ_DWORD, apr, makedreg(EAX));
			gen_codes(op_mov, BESZ_DWORD, ap3, makedreg(EDX));
        }
        else
        {
			if (isbit(node->v.p[0]))
			{
				AMODE *ap1;
			    apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
				MarkRegs();
				ChooseRegs(&areg, &sreg);
				ap1 = copy_addr(apr);
				gen_codes(op_lea, BESZ_DWORD, ap1 = makedreg(ECX), ap1);
				ap2 = bit_load(apr, areg, sreg, node->v.p[0]);
				gen_code(op, ap2, 0);
				ap1->mode = am_indisp;
				ChooseRegs(&areg, &sreg);
				bit_store(ap1, ap2, areg, sreg, node->v.p[0], FALSE);
			}
			else
			{
				MarkRegs();
			    ap2 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
				gen_code(op, ap2, 0);
			}
        }
		ReleaseRegs();
		return ap2;
    }
}

//-------------------------------------------------------------------------

AMODE *gen_asadd(ENODE *node, int areg, int sreg, int novalue, int op, int fopp, int fop, int foppr,
    int fopr)
/*
 *      generate a plus equal or a minus equal node.
 */
{
    AMODE *ap1,  *ap2,  *ap3, *apr;
    int nsize = natural_size(node->v.p[0]);
    if (natural_size(node) > BESZ_QWORD)
    {
        int n1 = natural_size(node->v.p[0]), n2 = natural_size(node->v.p[1]);
        if (n1 < BESZ_IFLOAT && n2 < BESZ_IFLOAT)
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 >=BESZ_IFLOAT && n2 < BESZ_CFLOAT) {
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
            ap1->length = BESZ_ILDOUBLE;
        } else
            ap1 = gen_complexadd(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
        if (nsize >= BESZ_CFLOAT) 
        {
            return complexstore(node->v.p[0], apr, novalue, nsize);
        }
        else if (nsize > BESZ_QWORD)
        {
            ap1 = do_extend(ap1,areg, sreg, nsize,F_FREG | F_VOL);
            return floatstore(node->v.p[0], apr, novalue, nsize);
        } 
        else
        {
	        ap1 = do_extend(ap1, areg, sreg, nsize,F_DREG | F_VOL) ;
            gen_codes(op_mov, apr->length, apr, ap1);
            return ap1;
        }
    }
    if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)
    {
		MarkRegs();
        ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_IMMED, nsize);
		ChooseRegs(&areg, &sreg);
        apr = gen_expr(node->v.p[0], ECX, sreg, FALSE, F_MEM | F_STOREADR, nsize);
        if (ap2->mode == am_immed)
        {
			noRelease = TRUE;
            gen_codes(op, BESZ_DWORD, apr, make_immed(ap2->offset->v.i &0xffffffffUL));
            ap1 = copy_addr(apr);
            ap1->offset = makenode(en_add, ap1->offset, makeintnode(en_icon, 4 ))
                ;
            #if sizeof(ULLONG_TYPE) == 4      
                gen_codes(op == op_add ? op_adc : op_sbb, BESZ_DWORD, ap1, make_immed(ap2
                    ->offset->v.i < 0 ?  - 1: 0));
            #else 
                gen_codes(op == op_add ? op_adc : op_sbb, BESZ_DWORD, ap1, make_immed(ap2
                    ->offset->v.i >> 32));
            #endif 
	      ap2 = copy_addr(apr);
        }
		else
		{
            if (apr->mode != am_indispscale)
                ap1 = copy_addr(apr);
            else
                ap1 = indx_data(apr, areg);
	      ap2 = copy_addr(ap1);
	      ap2->length = apr->length;
			noRelease = TRUE;
            gen_codes(op, BESZ_DWORD, ap1, makedreg(EAX));
            ap1->offset = makenode(en_add, ap1->offset, makeintnode(en_icon, 4 ));
            gen_codes(op == op_add ? op_adc : op_sbb, BESZ_DWORD, ap1, makedreg(EDX));
		}
		ReleaseRegs();
        return ap2;
    }
    if (!isbit(node->v.p[0]))
    {
		if (!novalue)
		{
			ap3 = RealizeDreg(areg);
			ap3->length = nsize;
		}
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
		ChooseRegs(&areg, &sreg);
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG, nsize);
	    if (apr->length ==  BESZ_FARPTR)
	        apr->length =  - BESZ_DWORD ;
	    if (ap2->length ==  BESZ_FARPTR)
	        ap2->length =  - BESZ_DWORD ;
		gen_code(op, apr, ap2);
		if (!novalue)
		{
			gen_code(op_mov, ap3, apr);
			apr = ap3;
		}
		ReleaseRegs();
        return apr;
    }
    else
    {
		ap2 = gen_expr(node->v.p[1], EDX, sreg, FALSE, F_IMMED | F_DREG, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
		ap3 = copy_addr(apr);
		gen_codes(op_lea, BESZ_DWORD, ap3 = makedreg(ECX), ap3);
		ap1 = bit_load(apr, areg, sreg, node->v.p[0]);
	    if (apr->length ==  BESZ_FARPTR)
	        apr->length =  - BESZ_DWORD ;
	    if (ap2->length ==  BESZ_FARPTR)
	        ap2->length =  - BESZ_DWORD ;
		gen_code(op, ap1, ap2);
		ap3->mode = am_indisp;
		ChooseRegs(&areg, &sreg);
        bit_store(ap3, ap1, areg, sreg, node->v.p[0], novalue);
		ReleaseRegs();
        return ap1;
    }
}

//-------------------------------------------------------------------------

AMODE *gen_aslogic(ENODE *node, int areg, int sreg, int novalue, int op, int op2)
/*
 *      generate a and equal or a or equal node.
 */
{
    AMODE *ap1,  *ap2,  *apr,  *ap3;
    int nsize = natural_size(node->v.p[0]);
    if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)
    {
		MarkRegs();
        ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_VOL | F_AXDX | F_IMMED, nsize);
		ChooseRegs(&areg, &sreg);
        apr = gen_expr(node->v.p[0], ECX, sreg, FALSE, F_MEM | F_STOREADR, nsize);
        if (ap2->mode == am_immed)
        {
			noRelease = TRUE;
            gen_codes(op, BESZ_DWORD, apr, make_immed(ap2->offset->v.i &0xffffffffUL));
            ap1 = copy_addr(apr);
            ap1->offset = makenode(en_add, ap1->offset, makeintnode(en_icon, 4))
                ;
            #if sizeof(ULLONG_TYPE) == 4      
                gen_codes(op, BESZ_DWORD, ap1, make_immed(ap2->offset->v.i < 0 ?  - 1: 0));
            #else 
                gen_codes(op, BESZ_DWORD, ap1, make_immed(ap2->offset->v.i >> 32));
            #endif 
	      ap2 = copy_addr(apr);
        }
        else
        {
            if (apr->mode != am_indispscale)
                ap1 = copy_addr(apr);
            else
                ap1 = indx_data(apr, areg);
	      ap2 = copy_addr(ap1);
	      ap2->length = apr->length;
			noRelease = TRUE;
            gen_codes(op, BESZ_DWORD, ap1, makedreg(EAX));
            ap1->offset = makenode(en_add, ap1->offset, makeintnode(en_icon, 4));
            gen_codes(op, BESZ_DWORD, ap1, makedreg(EDX));
        }
		ReleaseRegs();
        return ap2;
    }
    if (isbit(node->v.p[0]))
    {
	    ap2 = gen_expr(node->v.p[1], EDX, sreg, FALSE, F_DREG | F_IMMED | F_VOL | F_MEM, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
	    apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
		ap3 = copy_addr(apr);
		gen_codes(op_lea, BESZ_DWORD, ap3 = makedreg(ECX), ap3);
		ap1 = bit_load(apr, areg, sreg, node->v.p[0]);
		gen_code(op, ap1, ap2);
		ap3->mode = am_indisp;
		ChooseRegs(&areg, &sreg);
		bit_store(ap3, ap1, areg, sreg, node->v.p[0], novalue);
		ReleaseRegs();
		return ap1;
    }
    else
    {
	    apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
	    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG | F_VOL, nsize);
		gen_code(op, apr, ap2);
		ReleaseRegs();
		return apr;
    }
}

//-------------------------------------------------------------------------

AMODE *gen_asshift(ENODE *node, int areg, int sreg, int novalue, int op, int div)
/*
 *      generate shift equals operators.
 */
{
    AMODE *ap1,  *ap2,  *apr, *ap3;
    int nsize = natural_size(node->v.p[0]);
    if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)
    {
        ap1 = do64shift(node, areg, sreg, novalue, op, nsize, &apr);
        ap2 = copy_addr(apr);
        ap2->offset = makenode(en_add, ap2->offset, makeintnode(en_icon, 4));
		noRelease = TRUE;
        gen_codes(op_mov, BESZ_DWORD, apr, makedreg(EAX));
        gen_codes(op_mov, BESZ_DWORD, ap2, makedreg(EDX));
        return ap1;
    }
    if (!isbit(node->v.p[0]))
    {
	ap2 = gen_expr(node->v.p[1], ECX, sreg, FALSE, F_IMMED | F_DREG | F_VOL, nsize);
	MarkRegs();
	ChooseRegs(&areg, &sreg);
	ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
	doshift(ap1, ap2, op, div, nsize);
		ReleaseRegs();
        return ap1;
    }
    else
    {
	ap2 = gen_expr(node->v.p[1], ECX, sreg, FALSE, F_IMMED | F_DREG, nsize);
	MarkRegs();
	ChooseRegs(&areg, &sreg);
	apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
	ap3 = copy_addr(apr);
	gen_codes(op_lea, BESZ_DWORD, ap3 = makedreg(EDX), ap3);
	ap1 = bit_load(apr, areg, sreg, node->v.p[0]);
    doshift(ap1, ap2, op, div, nsize);
	ap3->mode = am_indisp;
		ChooseRegs(&areg, &sreg);
        bit_store(ap3, ap1, areg, sreg, node->v.p[0], novalue);
		ReleaseRegs();
        return ap1;
    }
}

//-------------------------------------------------------------------------

AMODE *gen_asmul(ENODE *node, int areg, int sreg, int novalue, int op, int foppr, int fopr, int
    fopp, int fop)
/*
 *      generate a *= node.
 */
{
    AMODE *ap1,  *ap2,  *apr,  *ap3;
    int nsize = natural_size(node->v.p[0]);
    if (natural_size(node) > BESZ_QWORD)
    {
        int n1 = natural_size(node->v.p[0]), n2 = natural_size(node->v.p[1]);
        if (n1 < BESZ_IFLOAT && n2 < BESZ_IFLOAT)
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 >=BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
            gen_codef(op_fchs,0,0) ;
        }
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 < BESZ_IFLOAT) 
        {
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
            ap1->length = BESZ_ILDOUBLE ;
        }
        else if (n1 <BESZ_IFLOAT && n2 >= BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
            ap1->length = BESZ_ILDOUBLE ;
        }
        else
            ap1 = gen_complexmul(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
        if (nsize >= BESZ_CFLOAT)
        {
            return complexstore(node->v.p[0], apr, novalue, nsize);
        }
        else if (nsize > BESZ_QWORD)
        {
            ap1 = do_extend(ap1, areg, sreg, nsize,F_FREG | F_VOL);
		return floatstore(node->v.p[0], apr, novalue, nsize);
        } 
        else
        {
            ap1 = do_extend(ap1, areg, sreg, nsize, F_DREG | F_VOL | F_AXDX);
            if (isbit(node->v.p[0]))
            {
                ap3 = copy_addr(apr);
                ap3->mode = am_indisp;
                bit_store(ap3, ap1, areg, sreg, node->v.p[0], novalue);
                return ap1;
            }
            if (ap1->length < BESZ_QWORD && ap1->length !=  - BESZ_QWORD)
                gen_codes(op_mov, apr->length, apr, ap1);
            else
            {
                ap2 = copy_addr(apr);
                ap2->mode = am_indisp;
                ap2->offset = makeintnode(en_icon, 0);
                gen_codes(op_mov, BESZ_DWORD, ap2, makedreg(EAX));
                ap2->offset = makenode(en_add, ap2->offset, makeintnode(en_icon, 4));
                gen_codes(op_mov, BESZ_DWORD, ap2, makedreg(EDX));
            }
			return ap1;
        }
    }
    else if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)
    {
        ap1 = do64mul(node, areg, sreg, BESZ_QWORD, &apr);
        if (ap1->length < BESZ_QWORD && ap1->length !=  - BESZ_QWORD)
        {
            gen_codes(op_mov, apr->length, apr, makedreg(EAX));
            return ap1;
        }
        gen_codes(op_mov, BESZ_DWORD, apr, makedreg(EAX));
        apr->offset = makenode(en_add, apr->offset, makeintnode(en_icon, 4));
        gen_codes(op_mov, BESZ_DWORD, apr, makedreg(EDX));
        return ap1;
    }
    if (!isbit(node->v.p[0]))
    {
	ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_VOL, nsize);
	MarkRegs();
	ChooseRegs(&areg, &sreg);
	ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
	gen_codes(op_imul, BESZ_DWORD, ap2, ap1);
        gen_code(op_mov, ap1, ap2);
	ReleaseRegs();
        return ap2;
    }
    else
    {
	ap2 = gen_expr(node->v.p[1], EDX, sreg, FALSE, F_DREG | F_MEM | F_IMMED, nsize);
	MarkRegs();
	ChooseRegs(&areg, &sreg);
	apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
	ap3 = copy_addr(apr);
	gen_codes(op_lea, BESZ_DWORD, ap3 = makedreg(ECX), ap3);
	ap1 = bit_load(apr, areg, sreg, node->v.p[0]);
	gen_codes(op_imul, BESZ_DWORD, ap1, ap2);
	ap3->mode = am_indisp;
	ChooseRegs(&areg, &sreg);
        bit_store(ap3, ap1, areg, sreg, node->v.p[0], novalue);
	ReleaseRegs();
        return ap1;
    }
}

//-------------------------------------------------------------------------

AMODE *gen_asmodiv(ENODE *node, int areg, int sreg, int novalue, int op, int fopp, int fop, int
    foppr, int fopr, int modflag)
/*
 *      generate /= and %= nodes.
 */
{
    AMODE *ap1,  *ap2,  *apr, *ap3;
    int nsize = natural_size(node->v.p[0]);
    if (natural_size(node) > BESZ_QWORD)
    {
        int n1 = natural_size(node->v.p[0]), n2 = natural_size(node->v.p[1]);
        if (n1 < BESZ_IFLOAT && n2 < BESZ_IFLOAT)
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 >=BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
            ap1->length = BESZ_ILDOUBLE ;
            gen_codef(op_fchs,0,0);
        }
        else if (n1 >= BESZ_IFLOAT && n1 < BESZ_CFLOAT && n2 < BESZ_IFLOAT) 
        {
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
            ap1->length = BESZ_ILDOUBLE ;
        }
        else if (n1 <BESZ_IFLOAT && n2 >= BESZ_IFLOAT && n2 < BESZ_CFLOAT)
        {
            ap1 = gen_fbinary(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
            ap1->length = BESZ_ILDOUBLE ;
            gen_codef(op_fchs,0,0);
        }
        else
            ap1 = gen_complexdiv(node, areg, sreg, fop, fopp, fopr, foppr, &apr);
        if (nsize >= BESZ_CFLOAT) 
        {
            return complexstore(node->v.p[0], apr, novalue, nsize);
        }
        else if (nsize > BESZ_QWORD)
        {
            return floatstore(node->v.p[0], apr, novalue, nsize);
        } 
        else
        {
            ap1 = do_extend(ap1, areg, sreg, nsize, F_DREG | F_VOL | F_AXDX);
            if (isbit(node->v.p[0]))
            {
                ap3 = copy_addr(apr);
                ap3->mode = am_indisp;
                bit_store(ap3, ap1, areg, sreg, node->v.p[0], novalue);
                return ap1;
            }
            if (ap1->length < BESZ_QWORD && ap1->length !=  - BESZ_QWORD)
                gen_codes(op_mov, apr->length, apr, ap1);
            else
            {
                ap2 = copy_addr(apr);
                ap2->mode = am_indisp;
                ap2->offset = makeintnode(en_icon, 0);
                gen_codes(op_mov, BESZ_DWORD, ap2, makedreg(EAX));
                ap2->offset = makenode(en_add, ap2->offset, makeintnode(en_icon, 4));
                gen_codes(op_mov, BESZ_DWORD, ap2, makedreg(EDX));
            }
			return ap1;
        }
    }
    else if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)
    {
        ap1 = do64div(node, areg, sreg, op, modflag, natural_size(node), &apr);
        if (ap1->length < BESZ_QWORD && ap1->length !=  - BESZ_QWORD)
        {
            gen_codes(op_mov, apr->length, apr, makedreg(EAX));
            return ap1;
        }
        else
        {
            gen_codes(op_mov, BESZ_DWORD, apr, makedreg(EAX));
            apr->offset = makenode(en_add, apr->offset, makeintnode(en_icon, 4));
            gen_codes(op_mov, BESZ_DWORD, apr, makedreg(EDX));
            return ap1;
        }
    }
    if (!isbit(node->v.p[0]))
    {
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
		if (apr->mode != am_dreg)
		{
			gen_codes(op_lea, BESZ_DWORD, apr = makedreg(ECX), apr);
			apr->mode = am_indisp;
			apr->length = nsize;
		}
        dodiv(apr, ap2, op, modflag);
		ReleaseRegs();
        return apr;
    }
    else
    {
		ap2 = gen_expr(node->v.p[1], ECX, sreg, FALSE, F_DREG | F_MEM | F_IMMED, natural_size(node->v.p[1]));
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
		ap3 = copy_addr(apr);
		gen_codes(op_lea, BESZ_DWORD, ap3 = makedreg(EDX), ap3);
		gen_codes(op_push, BESZ_DWORD, ap3, 0);
		ap1 = bit_load(apr, areg, sreg, node->v.p[0]);
        dodiv(ap1, ap2, op, modflag);
		gen_codes(op_pop, BESZ_DWORD, ap3, 0);
		ap3->mode = am_indisp;
		ChooseRegs(&areg, &sreg);
        bit_store(ap3, ap1, areg, sreg, node->v.p[0], novalue);
		ReleaseRegs();
        return ap1;
    }
}

//-------------------------------------------------------------------------

void mov1(int reg, AMODE *dst, AMODE *src, int size)
{
    AMODE *intermed = makedreg(reg);
    gen_codes(op_mov, size, intermed, src);
    gen_codes(op_mov, size, dst, intermed);
}

//-------------------------------------------------------------------------

void mov2(int reg, AMODE *dst, AMODE *src, int size1, int size2)
{
    AMODE *intermed = makedreg(reg),  *dst1,  *src1;
    dst1 = copy_addr(dst);
    src1 = copy_addr(src);
	noRelease = TRUE;
    gen_codes(op_mov, size1, intermed, src);
	noRelease = TRUE;
    gen_codes(op_mov, size1, dst, intermed);
    src1->offset = makenode(en_add, src->offset, makeintnode(en_icon, size1));
    dst1->offset = makenode(en_add, dst->offset, makeintnode(en_icon, size1));
    gen_codes(op_mov, size2, intermed, src1);
    gen_codes(op_mov, size2, dst1, intermed);
}

//-------------------------------------------------------------------------

void mov3(int reg, AMODE *dst, AMODE *src, int size1, int size2, int size3)
{
    AMODE *intermed = makedreg(reg),  *dst1,  *src1;
	noRelease = TRUE;
    gen_codes(op_mov, size1, intermed, src);
	noRelease = TRUE;
    gen_codes(op_mov, size1, dst, intermed);
    dst1 = copy_addr(dst);
    src1 = copy_addr(src);
    src1->offset = makenode(en_add, src->offset, makeintnode(en_icon, size1));
    dst1->offset = makenode(en_add, dst->offset, makeintnode(en_icon, size1));
	noRelease = TRUE;
    gen_codes(op_mov, size2, intermed, src1);
	noRelease = TRUE;
    gen_codes(op_mov, size2, dst1, intermed);
    src1->offset = makenode(en_add, src1->offset, makeintnode(en_icon, size2));
    dst1->offset = makenode(en_add, dst1->offset, makeintnode(en_icon, size2));
    gen_codes(op_mov, size3, intermed, src1);
    gen_codes(op_mov, size3, dst1, intermed);
}

//-------------------------------------------------------------------------

AMODE *amode_moveblock(AMODE *ap1, AMODE *ap2, int areg, int sreg, int size)
{
    int t = size &3, q = size >> 2;
    int pushds = FALSE, pushes = FALSE, pushcx = FALSE;
    AMODE *ap3;
	int reg = EAX, xchgreg = -1, pushed = FALSE;
	int preg1 = FALSE, sreg1 = FALSE, preg2 = FALSE, sreg2= FALSE;
	if (size <= 12 && size != 11)
	{
		int uregs[8],i;
		memset(uregs, 0 , sizeof(uregs));
		uregs[4] = uregs[5] = 1;
		if (ap1->mode == am_indisp)
			uregs[ap1->preg] = 1;
		if (ap1->mode == am_indispscale)
		{
			uregs[ap1->preg] = 1;
			if (ap1->sreg >= 0)
				uregs[ap1->sreg] = 1;
		}
		if (ap2->mode == am_indisp)
			uregs[ap2->preg] = 1;
		if (ap2->mode == am_indispscale)
		{
			uregs[ap2->preg] = 1;
			if (ap2->sreg >= 0)
				uregs[ap2->sreg] = 1;
		}
		for (i=0; i < sizeof(regs); i++)
			if (!uregs[i])
			{
				reg = i;
				break;
			}
		if (reg >= EBX || regs[reg])
		{
			pushed = TRUE;
			gen_codes(op_push, 4, makedreg(reg), NULL);
		}
		if (reg >= EBX && (size & 1))
		{
			gen_codes(op_xchg, 4, makedreg(EAX), makedreg(reg));
			xchgreg = reg;
			reg = EAX;
			if (ap1->mode == am_indisp)
				if (ap1->preg == EAX)
				{
					preg1 = TRUE;
					ap1->preg = xchgreg;
				}
			if (ap1->mode == am_indispscale)
			{
				if (ap1->preg == EAX)
				{
					preg1 = TRUE;
					ap1->preg = xchgreg;
				}
				if (ap1->sreg == EAX)
				{
					sreg1 = TRUE;
					ap1->sreg = xchgreg;
				}
			}
			if (ap2->mode == am_indisp)
				if (ap2->preg == EAX)
				{
					preg2 = TRUE;
					ap2->preg = xchgreg;
				}
			if (ap2->mode == am_indispscale)
			{
				if (ap2->preg == EAX)
				{
					preg2 = TRUE;
					ap2->preg = xchgreg;
				}
				if (ap2->sreg == EAX)
				{
					sreg2 = TRUE;
					ap2->sreg = xchgreg;
				}
			}
		}
	}
    switch (size)
    {
        case 1:
        case 2:
        case 4 :
            mov1(reg, ap1, ap2, size);
            break;
        case 3:
            mov2(reg, ap1, ap2, 2, 1);
            break;
        case 5:
            mov2(reg, ap1, ap2, BESZ_DWORD , 1);
            break;
        case 6:
            mov2(reg, ap1, ap2, BESZ_DWORD , 2);
            break;
        case 7:
            mov3(reg, ap1, ap2, BESZ_DWORD , 2, 1);
            break;
        case 8:
            mov2(reg, ap1, ap2, BESZ_DWORD , BESZ_DWORD );
            break;
        case 9:
            mov3(reg, ap1, ap2, BESZ_DWORD , BESZ_DWORD , 1);
            break;
        case 10:
            mov3(reg, ap1, ap2, BESZ_DWORD , BESZ_DWORD , 2);
            break;
        case 12:
            mov3(reg, ap1, ap2, BESZ_DWORD , BESZ_DWORD , BESZ_DWORD );
            break;
        default:
            if (ap1->mode == am_indisp && ap1->preg == ESI || (ap1->mode ==
                am_indispscale && (ap1->preg == ESI || ap2->preg == ESI)) || 
                (ap1->mode == am_dreg && ap1->preg == ESI))
            {
				int n;
				ap3 = reuseop(ap1, areg, &n);
                ap3->seg = ap1->seg;
                gen_code(op_lea, ap3, ap1);
				FreeUnused(n);
                ap1 = ap3;
            }
            GenPush(ESI, am_dreg, 0);
            GenPush(EDI, am_dreg, 0);
            if (regs[ECX])
			{
				pushcx = TRUE;
                GenPush(ECX, am_dreg, 0);
			}
            if (ap2->mode == am_dreg)
                gen_codes(op_mov, BESZ_DWORD , makedreg(ESI), ap2);
            else
                gen_codes(op_lea, BESZ_DWORD , makedreg(ESI), ap2);
            if (ap1->mode == am_dreg)
                gen_codes(op_mov, BESZ_DWORD , makedreg(EDI), ap1);
            else
                gen_codes(op_lea, BESZ_DWORD , makedreg(EDI), ap1);
            if (prm_farkeyword)
            {
                if (ap2->seg && ap2->seg != DS)
                {
                    pushds = TRUE;
                    GenPush(DS + 24, am_dreg, FALSE);
                }
                if (ap1->seg != ES)
                {
                    pushes = TRUE;
                    GenPush(ES + 24, am_dreg, FALSE);
                }
                if (pushds)
                    GenPush((ap2->seg ? ap2->seg : DS) + 24, am_dreg, FALSE);
                if (pushes)
                {
                    GenPush((ap1->seg ?ap1->seg : DS) + 24, am_dreg, FALSE);
                    GenPop(ES + 24, am_dreg, FALSE);
                }
                if (pushds)
                    GenPop(DS + 24, am_dreg, FALSE);

            }
            gen_code(op_cld, 0, 0);
            if (q)
            {
                gen_codes(op_mov, BESZ_DWORD , makedreg(ECX), make_immed(q));
                gen_code(op_rep, 0, 0);
                gen_code(op_movsd, 0, 0);
            }
            if (t &2)
                gen_code(op_movsw, 0, 0);
            if (t &1)
                gen_code(op_movsb, 0, 0);
            if (pushes)
                GenPop(ES + 24, am_dreg, FALSE);
            if (pushds)
                GenPop(DS + 24, am_dreg, FALSE);
            if (pushcx)
                GenPop(ECX, am_dreg, 0);
            GenPop(EDI, am_dreg, 0);
            GenPop(ESI, am_dreg, 0);
            break;
    }
	if (size <= 12 && size != 11)
	{
		if (xchgreg >= 0)
		{
			gen_codes(op_xchg, 4, makedreg(reg), makedreg(xchgreg));
			reg = xchgreg;
			if (preg1)
				ap1->preg = xchgreg;
			if (preg2)
				ap2->preg = xchgreg;
			if (sreg1)
				ap2->preg = xchgreg;
			if (sreg2)
				ap2->preg = xchgreg;
		}
		if (pushed)
		{
			gen_codes(op_pop, 4, makedreg(reg), NULL);
		}
			
	}
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *gen_moveblock(ENODE *node, int areg, int sreg)
{
    AMODE *ap1,  *ap2;
    ENODE ep1;
    if (!node->size)
        return (0);
    if (node->v.p[1]->nodetype == en_moveblock)
        ep1 =  *node->v.p[1];
    else if (node->v.p[1]->nodetype == en_movebyref)
        ep1 =  *node->v.p[1]->v.p[0];
    else
    {
        ep1.nodetype = en_a_ref;
        ep1.cflags = 0;
        ep1.v.p[0] = node->v.p[1];
    }
    ap1 = gen_expr(&ep1, areg, sreg, FALSE, F_MEM | F_DREG, BESZ_DWORD);
    if (ap1->offset)
	switch(ap1->offset->nodetype)
	{
		case en_nacon:
		case en_napccon:
		case en_labcon:
		case en_nalabcon:
			ap1->seg = defseg(ep1.v.p[0]);
			break;
	}
	MarkRegs();
	ChooseRegs(&areg, &sreg);
    if (node->v.p[0]->nodetype == en_moveblock)
        ep1 =  *node->v.p[0];
    else if (node->v.p[0]->nodetype == en_movebyref)
        ep1 =  *node->v.p[0]->v.p[0];
    else
    {
        ep1.nodetype = en_a_ref;
        ep1.cflags = 0;
        ep1.v.p[0] = node->v.p[0];
    }
    ap2 = gen_expr(&ep1, areg, sreg, FALSE, F_MEM | F_DREG, BESZ_DWORD);
	switch(ap2->offset->nodetype)
	{
		case en_nacon:
		case en_napccon:
		case en_labcon:
		case en_nalabcon:
			ap2->seg = defseg(ep1.v.p[0]);
			break;
	}
    if (ap2->mode == am_indispscale)
        ap2 = indx_data(ap2, areg);
	ReleaseRegs();
    ap1 = amode_moveblock(ap2, ap1, areg, sreg, node->size);
	return ap1;
}
//-------------------------------------------------------------------------

AMODE *amode_clearblock(AMODE *ap1, int size)
{
    int t = size &3, q = size >> 2;
    int pushds = FALSE, pushes = FALSE;
    AMODE *ap3;

    GenPush(EDI, am_dreg, 0);
    if (regs[1])
        GenPush(ECX, am_dreg, 0);
    if (ap1->mode == am_dreg)
        gen_codes(op_mov, BESZ_DWORD , makedreg(EDI), ap1);
    else
        gen_codes(op_lea, BESZ_DWORD , makedreg(EDI), ap1);
    if (prm_farkeyword)
    {
        if (ap1->seg && ap1->seg != ES)
        {
            pushes = TRUE;
            GenPush(ES + 24, am_dreg, FALSE);
        }
        if (pushes)
        {
            GenPush(ap1->seg + 24, am_dreg, FALSE);
            GenPop(ES + 24, am_dreg, FALSE);
        }
        else
        {
            GenPush(SS + 24, am_dreg, FALSE);
            GenPop(ES + 24, am_dreg, FALSE);
        }
        if (pushds)
            GenPop(DS + 24, am_dreg, FALSE);

    }
    gen_code(op_cld, 0, 0);
    if (q)
    {
        gen_codes(op_sub,BESZ_DWORD , makedreg(EAX),makedreg(EAX)) ;
        gen_codes(op_mov, BESZ_DWORD , makedreg(ECX), make_immed(q));
        gen_code(op_rep, 0, 0);
        gen_code(op_stosd, 0, 0);
    }
    if (t &2)
        gen_code(op_stosw, 0, 0);
    if (t &1)
        gen_code(op_stosb, 0, 0);
    if (pushes)
        GenPop(ES + 24, am_dreg, FALSE);
    if (regs[1])
        GenPop(ECX, am_dreg, 0);
    GenPop(EDI, am_dreg, 0);
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *gen_clearblock(ENODE *node, int areg, int sreg)
{
    AMODE *ap1 ;
    ENODE ep1;
    if (!node->size)
        return (0);
    ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM, 0);
    return amode_clearblock(ap1, node->size);
}


//-------------------------------------------------------------------------

AMODE *gen_assign(ENODE *node, int areg, int sreg, int novalue, int stdc)
/*
 *      generate code for an assignment node. if the size of the
 *      assignment destination is larger than the size passed then
 *      everything below this node will be evaluated with the
 *      assignment size.
 */
{
    AMODE *ap1,  *ap2,  *apr;
    int bits, i;
    int nsize = natural_size(node->v.p[0]);
    if (nsize > BESZ_QWORD)
    {
		MarkRegs();
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_FREG | F_VOL, nsize);
		ReleaseRegs();
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, nsize);
        if (apr->length <= BESZ_LDOUBLE)
        {
            ap2 = do_extend(ap2, areg, sreg, BESZ_LDOUBLE, F_FREG);
            ap2 = floatstore(node->v.p[0], apr, novalue, nsize);
        }
        else if (apr->length < BESZ_CFLOAT)
        {
            ap2 = do_extend(ap2, areg, sreg, BESZ_ILDOUBLE, F_FREG);
			ap2 = floatstore(node->v.p[0], apr, novalue, nsize);
        }
        else
        {
            ap2 = do_extend(ap2, areg, sreg, BESZ_CLDOUBLE, 0);
            ap2 = complexstore(node->v.p[0], apr, novalue, nsize);
        }
		ReleaseRegs();
		return ap2;
    }
	if (nsize == BESZ_FARPTR)
	{
        AMODE *ap3;
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_AXDX | F_IMMED, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, nsize);
        if (ap2->mode == am_axdx)
        {
            ap1 = copy_addr(apr);
            ap1->offset = makenode(en_add, ap1->offset, makeintnode
                (en_icon, 4));
            gen_codes(op_mov, BESZ_DWORD , apr, makedreg(EAX));
            gen_codes(op_mov, 2, ap1, makedreg(EDX));
        }
        else if (ap2->mode == am_dreg)
        {
            ap3 = xalloc(sizeof(AMODE));
            ap3->mode = am_seg;
            ap3->seg = ap2->seg ? ap2->seg: (i = defseg(node
                ->v.p[1])) ? i : DS;
            ap1 = copy_addr(apr);
            ap1->offset = makenode(en_add, ap1->offset, makeintnode
                (en_icon, 4));
            gen_codes(op_mov, BESZ_DWORD, apr, makedreg(ap2->preg));
            gen_codes(op_mov, BESZ_NONE, ap1, ap3);
        }
        else if (ap2->mode == am_immed)
        {
            ap1 = copy_addr(apr);
            ap1->offset = makenode(en_add, ap1->offset, makeintnode
                (en_icon, 4));
			if (prm_farkeyword && !isintconst(ap2->offset->nodetype))
			{
                gen_codes(op_mov, BESZ_DWORD, apr, ap2);
				if (ap2->seg == e_default)
					ap2->seg = e_ds;
				gen_codes(op_mov, BESZ_NONE, ap1, makesegreg(ap2->seg));
			}
			else
			{
                gen_codes(op_mov, BESZ_DWORD, apr, make_immed(ap2->offset->v.i
                    &0xffffffff));
            #if sizeof(ULLONG_TYPE) == 4
                gen_codes(op_mov, 2, ap1, make_immed(0));
            #else 
                gen_codes(op_mov, 2, ap1, make_immed((ap2->offset
                    ->v.i >> 32) &0xffffffff));
            #endif 
			}
        }
		ReleaseRegs();
        return (ap2);
	}
	if (nsize == BESZ_QWORD || nsize == - BESZ_QWORD)
	{
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_IMMED, nsize);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[0], ECX, sreg, FALSE, F_MEM | F_STOREADR, nsize);
		if (apr->mode == am_indispscale)
		{
			if (apr->preg >= 0 && apr->preg < EBX && apr->sreg >= 0  && apr->sreg < EBX)
			{
				if (apr->preg == ECX)
				{
					gen_code(op_lea, makedreg(apr->preg), apr);
					apr = makedreg(apr->preg);
					apr->mode = am_indisp;
					apr->offset = makeintnode(en_icon, 0);
				}
				else
				{
					gen_code(op_lea, makedreg(apr->sreg), apr);
					apr = makedreg(apr->sreg);
					apr->mode = am_indisp;
					apr->offset = makeintnode(en_icon, 0);
				}
			}
		}
        if (ap2->mode == am_axdx)
        {
            ap1 = copy_addr(apr);
            ap1->offset = makenode(en_add, ap1->offset, makeintnode
                (en_icon, 4));
			noRelease = TRUE;
            gen_codes(op_mov, BESZ_DWORD, apr, makedreg(EAX));
            gen_codes(op_mov, BESZ_DWORD, ap1, makedreg(EDX));
        }
        else if (ap2->mode == am_immed)
        {
            ap1 = copy_addr(apr);
            ap1->offset = makenode(en_add, ap1->offset,
                makeintnode(en_icon, 4));
			noRelease = TRUE;
            gen_codes(op_mov, BESZ_DWORD, apr, make_immed(ap2->offset->v.i
                &0xffffffff));
            #if sizeof(ULLONG_TYPE) == 4
                gen_codes(op_mov, BESZ_DWORD, ap1, make_immed(ap2->offset
                    ->v.i < 0 ?  - 1: 0));
            #else 
                gen_codes(op_mov, BESZ_DWORD, ap1, make_immed(ap2->offset
                    ->v.i >> 32));
            #endif 
        }
		ReleaseRegs();
        return (ap2);
	}
	if (node->v.p[0]->nodetype == en_tempref)
	{
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, nsize);
	}
	else
	{
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_IMMED, nsize);
	}

	if (!novalue)
	{
		if (ap2->mode != am_immed && ap2->mode != am_dreg)
		{
			ChooseRegs(&areg, &sreg);
			ap2 = do_extend(ap2, areg, sreg, nsize, F_DREG | F_VOL);
		}
	}
	MarkRegs();
	ChooseRegs(&areg, &sreg);
	apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, nsize);
    bits = isbit(node->v.p[0]);
    if (!equal_address(apr, ap2))
    {
        if (!bits)
        {
            gen_codes(op_mov, apr->length, apr, ap2);
            if (apr->mode == am_dreg && ap2->mode == am_dreg)
                apr->seg = ap2->seg;
        }
        else
        {
			ChooseRegs(&areg, &sreg);
            bit_store(apr, ap2, areg, sreg, node->v.p[0], novalue);
        }
    }
	ReleaseRegs();
    return ap2;
}

//-------------------------------------------------------------------------

AMODE *gen_refassign(ENODE *node, int areg, int sreg, int novalue)
/*
 *      generate code for an assignment node. if the size of the
 *      assignment destination is larger than the size passed then
 *      everything below this node will be evaluated with the
 *      assignment size.
 */
{
    AMODE *ap1,  *ap2,  *apr,  *ap4,  *ap5;
    int nsize = natural_size(node->v.p[0]);
    if (nsize > BESZ_QWORD)
    {
		MarkRegs();
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_VOL, natural_size(node->v.p[1]));
        floatload(ap2, TRUE);
		ReleaseRegs();
		MarkRegs();
		apr = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, BESZ_DWORD);
        ap2 = floatstore(node->v.p[0], apr, novalue, nsize);
		ReleaseRegs();
		if (!novalue)
		{
	        ap4 = RealizeDreg(areg);
    	    gen_code(op_lea, ap4, ap2);
        	ap4->seg = ap2->seg;
		}
		else
		{
			ap4 = NULL;
		}
        return ap4;
    }
    if (!isbit(node->v.p[0]))
    {
		MarkRegs();
        if (apr->length == BESZ_QWORD || apr->length ==  - BESZ_QWORD)
        {
			ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_VOL, natural_size(node->v.p[1]));
            ap2->length = BESZ_QWORD;
			ChooseRegs(&areg, &sreg);
			apr = gen_expr(node->v.p[1], areg, sreg, FALSE, F_MEM | F_STOREADR, BESZ_DWORD);
            ap5 = copy_addr(apr);
            ap5->offset = makenode(en_add, apr->offset, makeintnode(en_icon,
                4));
			noRelease = TRUE;
            gen_codes(op_mov, BESZ_DWORD , apr, makedreg(EAX));
            gen_codes(op_mov, BESZ_DWORD , ap5, makedreg(EDX));
        }
        else
        {
			ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG, natural_size(node->v.p[1]));
            ap2->length = ap1->length;
			ChooseRegs(&areg, &sreg);
			apr = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, BESZ_DWORD);
            gen_code(op_mov, apr, ap2);
        }
		ReleaseRegs();
		if (!novalue)
		{
	        ap4 = RealizeDreg(areg);
    	    gen_code(op_lea, ap4, apr);
        	ap4->seg = apr->seg;
		}
		else
		{
			ap4 = NULL;
		}
        return ap4;
    }
    else
    {
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG, natural_size(node->v.p[1]));
		ChooseRegs(&areg, &sreg);
		apr = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_MEM | F_STOREADR, BESZ_DWORD);
		ChooseRegs(&areg, &sreg);
        bit_store(apr, ap2, areg, sreg, node->v.p[0], novalue);
		ReleaseRegs();
		return NULL;
    }
//    return ap2;
}

//-------------------------------------------------------------------------

AMODE *gen_aincdec(ENODE *node, int areg, int sreg, int novalue, int op)
/*
 *      generate an auto increment or decrement node. op should be
 *      either op_add (for increment) or op_sub (for decrement).
 */
{
    AMODE *ap1,  *ap2,  *ap3,  *ap4;
    int seg = 0;
    int pushed = FALSE;
    int nsize = natural_size(node->v.p[0]), sz;
    if (nsize > BESZ_QWORD)
    {
        if (nsize < BESZ_CFLOAT)
        {
            AMODE apn;
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG | F_VOL, nsize);
			MarkRegs();
            sz = ap1->length;
            apn = *ap1;
            floatload(&apn, TRUE);
		    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_VOL | F_IMMED, BESZ_DWORD );
            make_floatconst(ap2);
            floatload(ap2, TRUE);
            if (novalue)
			{
               gen_codef(op == op_add ? op_faddp : op_fsubp, makefreg(1), 0);
			   ap3 = NULL;
			}
            else
			{
                gen_codef(op == op_add ? op_fadd : op_fsubr, makefreg(1), 0);
				ap3 = fstack();
			}
			ReleaseRegs();
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, nsize);
            floatstore(node->v.p[0], ap1, TRUE, nsize);
        }
        else
        {
            AMODE apn;
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG | F_VOL, nsize);
			MarkRegs();
            sz = ap1->length;
            apn = *ap1;
            complexload(&apn, TRUE);
		    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED, BESZ_DWORD );
            make_floatconst(ap2);
            floatload(ap2, TRUE);
			if (!novalue)
			{
				gen_codef(op_fld, makefreg(1), 0);
				gen_codef(op_fld, makefreg(1), 0);
	            ap3 = fstack();
	            ap3->mode = am_frfr;
	            ap3->length = BESZ_CLDOUBLE;
			}
			else
				ap3 = NULL;
            gen_codef(op == op_add ? op_faddp : op_fsubp, makefreg(1), 0);
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM | F_STOREADR, nsize);
            return complexstore(node->v.p[0], ap1, TRUE, nsize);
        }
    }
    else if (nsize == BESZ_QWORD || nsize ==  - BESZ_QWORD)
    {
		MarkRegs();
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM, nsize);
        ap4 = copy_addr(ap1);
        ap4->offset = makenode(en_add, ap1->offset, makeintnode(en_icon, BESZ_DWORD ));
		if (!novalue)
		{
			gen_codes(op_push, BESZ_DWORD, ap4, 0);
			gen_codes(op_push, BESZ_DWORD, ap1, 0);
		}
        sz = ap1->length;
		ChooseRegs(&areg, &sreg);
	    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG | F_VOL, BESZ_DWORD );
        ap1->length = BESZ_DWORD ;
		noRelease = TRUE;
        gen_code(op, ap1, ap2);
        gen_codes(op == op_add ? op_adc : op_sbb, BESZ_DWORD, ap4, make_immed(0));
		ReleaseRegs();
		if (!novalue)
		{
			GenPop(EAX, am_dreg, 0);
			GenPop(EDX, am_dreg, 0);
			ap3 = temp_axdx();
		}
		else
		{
			ap3 = NULL;
		}
    }
    else if (isbit(node->v.p[0]))
    {
		if (!novalue)
		{
			ap3 = RealizeDreg(areg);
		}
		MarkRegs();
		ChooseRegs(&areg, &sreg);
        ap4 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL,nsize);
		if (!novalue)
			gen_code(op_mov, ap3, ap4);
		MarkRegs();
		ChooseRegs(&areg, &sreg);
	    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG | F_MEM, BESZ_DWORD );
		ReleaseRegs();
        gen_code(op, ap4, ap2);
		ReleaseRegs();
		MarkRegs();
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_MEM, nsize);
		ChooseRegs(&areg, &sreg);
        bit_store(ap1, ap4, areg, sreg, node->v.p[0], novalue);
		ReleaseRegs();
		if (novalue)
		{
			ap3 = NULL;
		}
    }
	else if (nsize == BESZ_FARPTR)
	{
		MarkRegs();
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_IMMED | F_DREG | F_VOL, nsize);
		if (!novalue)
		{
            ap4 = copy_addr(ap1);
            ap4->offset = makenode(en_add, ap4->offset, makeintnode
                (en_icon, 4));
            gen_codes(op_push, BESZ_DWORD , ap4, 0);
            gen_codes(op_push, BESZ_DWORD , ap1, 0);
		}
		ChooseRegs(&areg, &sreg);
	    ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG | F_MEM, BESZ_DWORD );
		gen_code(op, ap1, ap2);
		ReleaseRegs();
		if (novalue)
		{
			ap3 = NULL;
		}
		else
		{
			GenPop(EAX, am_dreg, 0);
			GenPop(EDX, am_dreg, 0);
			ap3 = temp_axdx();
		}
	}
    else
    {
		if (!novalue)
		{
			ap3 = RealizeDreg(areg);
			ap3->length = nsize;
		}
		MarkRegs();
		ChooseRegs(&areg, &sreg);
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM, nsize);
		if (!novalue)
		{
			gen_codes(op_mov, ap3->length, ap3, ap1);
		}
		ChooseRegs(&areg, &sreg);
        ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_IMMED | F_DREG | F_VOL, nsize);
		gen_codes(op, nsize, ap1, ap2);
		ReleaseRegs();
		if (novalue)
			ap3 = NULL;
    }
    return ap3;
}

//-------------------------------------------------------------------------
#ifdef XXXXX
int pushfloatconst(int size, long double aa)
{
    float bb;
    double cc;
    int rv ;
        switch (size)
        {
            case BESZ_FLOAT:
            case BESZ_IFLOAT:
            case BESZ_CFLOAT:
                bb = aa;
                gen_codes(op_push, BESZ_DWORD , make_immed(*(unsigned*) &bb), 0);
                rv = 4;
                break;
            case BESZ_DOUBLE:
            case BESZ_IDOUBLE:
            case BESZ_CDOUBLE:
                rv = 8;
                cc = aa;
                gen_codes(op_push, BESZ_DWORD , make_immed(*(((unsigned*) &cc) + 1)), 0);
                gen_codes(op_push, BESZ_DWORD , make_immed(*(unsigned*) &cc), 0);
                break;
            case BESZ_LDOUBLE:
            case BESZ_ILDOUBLE:
                rv = 12;
                gen_codes(op_push, BESZ_DWORD , make_immed(*(((unsigned short*) &aa) + 4))
                    , 0);
                gen_codes(op_push, BESZ_DWORD , make_immed(*(((unsigned*) &aa) + 1)), 0);
                gen_codes(op_push, BESZ_DWORD , make_immed(*(unsigned*) &aa), 0);
                break;
            case BESZ_CLDOUBLE: /* only happens in pairs */
                rv = 10;
                gen_codes(op_push, 2, make_immed(*(((unsigned short*) &aa) + 4))
                    , 0);
                gen_codes(op_push, BESZ_DWORD , make_immed(*(((unsigned*) &aa) + 1)), 0);
                gen_codes(op_push, BESZ_DWORD , make_immed(*(unsigned*) &aa), 0);
                break;
        }
        return rv;
}
#endif
int push_param(ENODE *ep, int areg, int sreg, int size)
/*
 *      push the operand expression onto the stack.
 */
{
    AMODE *ap,  *ap2;
    int i;
    float cc;
    double bb;
    long double aa;
    int rv;
    int nsize = natural_size(ep);
    switch (ep->nodetype)
    {
        case en_absacon:
            ep->v.i = ep->v.sp->value.i;
            ap = xalloc(sizeof(AMODE));
            ap->mode = am_immed;
            ap->offset = ep; /* use as constant node */
            ap->length = rv = BESZ_DWORD ;
            gen_code(op_push, ap, 0);
            rv = 4;
            break;
        case en_napccon:
        case en_nacon:
            ep->v.p[0] = ep->v.sp;
        case en_labcon:
        case en_nalabcon:
            ap = xalloc(sizeof(AMODE));
            ap->mode = am_immed;
            ap->offset = ep; /* use as constant node */
            ap->length = rv = BESZ_DWORD ;
            gen_code(op_push, ap, 0);
            gen_code(op_void, 0, 0);
            break;
        case en_cfc:
        case en_fcomplexref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_CLDOUBLE);
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(8));
            ap2 = make_stack(0);
            ap2->length = BESZ_FLOAT;
            gen_codef(op_fstp, ap2, 0);
            ap2 = make_stack(-4);
            ap2->length = BESZ_FLOAT;
            gen_codef(op_fstp, ap2, 0); 
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 8;
            break;
        case en_crc:
        case en_rcomplexref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG |F_VOL, BESZ_CLDOUBLE);
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(16));
            ap2 = make_stack(0);
            ap2->length = BESZ_DOUBLE;
            gen_codef(op_fstp, ap2, 0);
            ap2 = make_stack(-8);
            ap2->length = BESZ_DOUBLE;
            gen_codef(op_fstp, ap2, 0); 
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 16;
            break;
        case en_clrc:
        case en_lrcomplexref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_CLDOUBLE);
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(20));
            ap2 = make_stack(0);
            ap2->length = BESZ_LDOUBLE;
            gen_codef(op_fstp, ap2, 0);
            ap2 = make_stack(-10);
            ap2->length = BESZ_LDOUBLE;
            gen_codef(op_fstp, ap2, 0); 
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 20;
            break;
        case en_cfi:
        case en_fimaginaryref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_ILDOUBLE );
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(4));
            ap2 = make_stack(0);
            ap2->length = BESZ_FLOAT;
            gen_codef(op_fstp, ap2, 0);
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = BESZ_DWORD ;
            break;
        case en_cf:
        case en_floatref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_LDOUBLE );
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(4));
            ap2 = make_stack(0);
            ap2->length = BESZ_FLOAT;
            gen_codef(op_fstp, ap2, 0);
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 4;
            break;
        case en_cd:
        case en_doubleref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_LDOUBLE );
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(8));
            ap2 = make_stack(0);
            ap2->length = BESZ_DOUBLE;
            gen_codef(op_fstp, ap2, 0);
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 8;
            break;
        case en_cri:
        case en_rimaginaryref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_ILDOUBLE );
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(8));
            ap2 = make_stack(0);
            ap2->length = BESZ_DOUBLE;
            gen_codef(op_fstp, ap2, 0);
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 8;
            break;
        case en_cld:
        case en_longdoubleref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_VOL | F_FREG, BESZ_LDOUBLE);
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(12));
            ap2 = make_stack(0);
            ap2->length = BESZ_LDOUBLE;
            gen_codef(op_fstp, ap2, 0);
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 12;
            break;
        case en_clri:
        case en_lrimaginaryref:
			MarkRegs();
            ap = gen_expr(ep, areg, sreg, FALSE, F_VOL | F_FREG, BESZ_ILDOUBLE);
			ReleaseRegs();
            gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(12));
            ap2 = make_stack(0);
            ap2->length = BESZ_LDOUBLE;
            gen_codef(op_fstp, ap2, 0);
            gen_codef(op_fwait, 0, 0);
            gen_code(op_void, 0, 0);
            rv = 12;
            break;
        case en_cfp:
        case en_fp_ref:
            ap = gen_expr(ep, areg, sreg, FALSE, F_AXDX | F_DREG | F_IMMED | F_MEM, BESZ_FARPTR);
            if (ap->mode == am_axdx)
            {
                gen_codes(op_push, BESZ_DWORD , makedreg(EDX), 0);
                gen_codes(op_push, BESZ_DWORD , makedreg(EAX), 0);
            }
			else if (ap->mode == am_immed)
			{
				if (isintconst(ap->offset->nodetype))
                    #if sizeof(LLONG_TYPE) == 4
	                    gen_codes(op_push, BESZ_DWORD , make_immed(ap->offset->v.i < 0 ? 
    	                    - 1: 0), 0);
        	        #else 
            	        gen_codes(op_push, BESZ_DWORD , make_immed(ap->offset->v.i >> 32),
                	        0);
                    #endif 
				else
				{
	                if (ap->seg == 0)
    	                ap->seg = (i = defseg(ep->v.p[0])) ? i : DS;
	                GenPush(ap->seg + 24, am_dreg, FALSE);
				}
				gen_codes(op_push, BESZ_DWORD, ap, NULL);
			}
            else if (ap->mode == am_dreg)
            {
                if (ap->seg == 0)
                    ap->seg = ap->seg ? ap->seg : DS;
                GenPush(ap->seg + 24, am_dreg, FALSE);
                GenPush(ap->preg, am_dreg, FALSE);
            }
            else
            {
                ap2 = copy_addr(ap);
                ap2->offset = makenode(en_add, ap2->offset, makeintnode(en_icon,
                    4));
                gen_codes(op_push, BESZ_DWORD , ap2, 0);
                gen_codes(op_push, BESZ_DWORD , ap, 0);
            }
            rv = 8;
            break;
            break;
        case en_csp:
            if (!prm_farkeyword)
            {
                gen_codes(op_push, BESZ_DWORD , make_immed(0), 0);
            }
            else
            {
                ap = gen_expr(ep->v.p[0], areg, sreg, FALSE, F_VOL, BESZ_DWORD );
                if (ap->mode != am_immed && ap->mode != am_seg)
                {
                    if ((ap->mode == am_indisp || ap->mode == am_indispscale ||
                        ap->mode == am_direct) && ap->length ==  BESZ_FARPTR)
                    {
                        ap->offset = makenode(en_add, ap->offset, makeintnode
                            (en_icon, 4));
                        ap->length = BESZ_DWORD ;
                    }
                    else if (ap->length ==  BESZ_FARPTR)
                    {
                        ap->mode = am_seg;
                    }
                    else
                    {
                        ap->length = BESZ_DWORD ;
                    }
                }
                gen_code(op_push, ap, 0);
            }
            rv = 4;
            break;
        default:
            if (ep->nodetype == en_thiscall && ep->v.p[0]->nodetype ==
                en_substack)
            {
                rv = ep->v.p[0]->v.p[0]->v.p[0]->v.i; // assume an icon node
                break;
            }
            if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
            {
	            ap = gen_expr(ep, areg, sreg, FALSE, F_IMMED | F_AXDX | F_MEM, size);
                if (ap->mode == am_axdx)
                {
                    gen_codes(op_push, BESZ_DWORD , makedreg(EDX), 0);
                    gen_codes(op_push, BESZ_DWORD , makedreg(EAX), 0);
                }
                else if (ap->mode == am_immed)
                {
                    #if sizeof(LLONG_TYPE) == 4
	                    gen_codes(op_push, BESZ_DWORD , make_immed(ap->offset->v.i < 0 ? 
    	                    - 1: 0), 0);
        	        #else 
            	        gen_codes(op_push, BESZ_DWORD , make_immed(ap->offset->v.i >> 32),
                	        0);
                    #endif 
                    gen_codes(op_push, BESZ_DWORD , make_immed(ap->offset->v.i), 0);
                }
                else
                {
                    ap2 = copy_addr(ap);
                    ap2->offset = makenode(en_add, ap2->offset, makeintnode
                        (en_icon, BESZ_DWORD ));
					noRelease = TRUE;
                    gen_codes(op_push, BESZ_DWORD , ap2, 0);
                    gen_codes(op_push, BESZ_DWORD , ap, 0);
                }
                rv = 8;
            }
            else if (size <= BESZ_BOOL)
            {
	            ap = gen_expr(ep, areg, sreg, FALSE, F_IMMED | F_DREG | F_MEM, BESZ_DWORD);
//                while (castvalue(ep))
//                    ep = ep->v.p[0];
//                if (isbit(ep))
//                   ap = bit_load(ap, areg, sreg, ep);
                gen_codes(op_push, BESZ_DWORD , ap, 0);
                rv = 4;
            }
            else if (size >= BESZ_CFLOAT)
            {
                int sz ;
	            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_MEM, size);
                complexload(ap ,FALSE);
                rv = 8,sz = BESZ_FLOAT;
                if (size == BESZ_CDOUBLE)
                    rv = 16,sz = BESZ_DOUBLE;
                if (size == BESZ_CLDOUBLE)
                    rv = 20,sz = BESZ_LDOUBLE;
                gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(rv));
                ap2 = make_stack(0);
                ap2->length = sz;
                gen_codef(op_fstp, ap2, 0);
                ap2 = make_stack(-rv/2);
                ap2->length = sz ;
                gen_codef(op_fstp, ap2, 0);
                gen_codef(op_fwait, 0, 0);
            }
            else
            {
	            ap = gen_expr(ep, areg, sreg, FALSE, F_FREG | F_MEM, size);
                floatload(ap, TRUE);
                rv = 8;
                if (size == BESZ_FLOAT || size == BESZ_IFLOAT)
                    rv = 4;
                if (size == BESZ_LDOUBLE || size == BESZ_IDOUBLE)
                    rv = 12;
                gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(rv));
                ap2 = make_stack(0);
                ap2->length = size;
                gen_codef(op_fstp, ap2, 0);
                gen_codef(op_fwait, 0, 0);
            }
            break;
    }
    gen_code(op_void, 0, 0);
    return (rv);
}

//-------------------------------------------------------------------------

int push_stackblock(ENODE *ep, int areg, int sreg)
{
    AMODE *ap,  *ap1;
    SYM *sp;
    int x;
    int sz = (ep->size + 3) &-4;
    int pushds = 0;
    if (!sz)
        return (0);
    if (sz > 24)
    {
        gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), make_immed(sz));
        gen_codes(op_push, BESZ_DWORD , makedreg(ESI), 0);
        gen_codes(op_push, BESZ_DWORD , makedreg(EDI), 0);
        switch (ep->nodetype)
        {
            case en_napccon:
            case en_nacon:
                ep->v.p[0] = ep->v.sp;
            case en_nalabcon:
            case en_labcon:
                ap = xalloc(sizeof(AMODE));
                ap->mode = am_direct;
                ap->offset = ep; /* use as constant node */
                gen_codes(op_lea, BESZ_DWORD , ap1 = makedreg(ESI), ap);
                ap1->seg = ap->seg;

                break;
            case en_absacon:
                ep->v.i = ep->v.sp->value.i;
                ap = xalloc(sizeof(AMODE));
                ap->mode = am_direct;
                ap->seg = 0;
                ap->offset = ep; /* use as constant node */
                gen_codes(op_lea, BESZ_DWORD , ap1 = makedreg(ESI), ap);
                ap1->seg = ap->seg;
                break;
            case en_autocon:
                sp = ep->v.p[0] = ep->v.sp;
                x = sp->value.i;
                ap = xalloc(sizeof(AMODE));
                ap->mode = am_indisp;
                ap->preg = EBP;
                ap->offset = makeintnode(en_icon, x);
                gen_codes(op_lea, BESZ_DWORD , ap1 = makedreg(ESI), ap);
                if (prm_farkeyword)
                    ap1->seg = SS;
                break;
            default:
                ep = makenode(en_a_ref, ep, 0);
                ap = gen_expr(ep, areg, sreg, FALSE, F_MEM, BESZ_DWORD);
                if (ap->mode == am_dreg)
                    gen_codes(op_mov, BESZ_DWORD , makedreg(ESI), ap);
                else
                {
                    gen_codes(op_lea, BESZ_DWORD , ap1 = makedreg(ESI), ap);
                    ap1->seg = ap->seg;
                }
                if (ap->length ==  BESZ_FARPTR)
                {
                    AMODE *ap3 = copy_addr(ap);
                    AMODE *ap4 = xalloc(sizeof(AMODE));
                    ap4->mode = am_seg;
                    ap4->seg = 0;
                    ap3->offset = makenode(en_add, ap3->offset, makeintnode
                        (en_icon, 4));
                    pushds = TRUE;
                    GenPush(DS + 24, am_dreg, FALSE);
                    gen_codes(op_mov, 2, ap4, ap3);
                }
                break;
        }
        gen_codes(op_lea, BESZ_DWORD , ap1 = makedreg(EDI), make_stack( - 8));
        if (prm_farkeyword)
            ap1->seg = SS;
        if (prm_farkeyword)
        {
            GenPush(SS + 24, am_dreg, 0);
            GenPop(ES + 24, am_dreg, 0);
        }
        if (regs[ECX])
            GenPush(ECX, am_dreg, 0);
        gen_code(op_cld, 0, 0);
        gen_codes(op_mov, BESZ_DWORD , makedreg(ECX), make_immed(sz >> 2));
        gen_code(op_rep, 0, 0);
        gen_code(op_movsd, 0, 0); // always room for dwords on stack
        if (regs[ECX])
            GenPop(ECX, am_dreg, 0);
        if (pushds)
            GenPop(DS + 24, am_dreg, FALSE);
        gen_codes(op_pop, BESZ_DWORD , makedreg(EDI), 0);
        gen_codes(op_pop, BESZ_DWORD , makedreg(ESI), 0);
        gen_code(op_void, 0, 0);
    }
    else
    {
        int i = ((sz + 3) / 4) *4;
        switch (ep->nodetype)
        {
            case en_napccon:
            case en_nacon:
                ep->v.p[0] = ep->v.sp;
            case en_nalabcon:
            case en_labcon:
                ap = xalloc(sizeof(AMODE));
                ap->mode = am_direct;
                ap->offset = ep; /* use as constant node */
                break;
            case en_absacon:
                ep->v.i = ep->v.sp->value.i;
                ap = xalloc(sizeof(AMODE));
                ap->mode = am_direct;
                ap->seg = 0;
                ap->offset = ep; /* use as constant node */
                break;
            case en_autocon:
                sp = ep->v.p[0] = ep->v.sp;
                x = sp->value.i;
                ap = xalloc(sizeof(AMODE));
                ap->mode = am_indisp;
                ap->preg = EBP;
                ap->offset = makeintnode(en_icon, x);
                break;
            default:
                ep = makenode(en_a_ref, ep, 0);
                ap = gen_expr(ep, areg, sreg, 0, F_DREG | F_MEM, BESZ_DWORD );
                if (ap->mode == am_dreg)
                {
                    ap->mode = am_indisp;
                    ap->offset = makeintnode(en_icon, 0);
                }
                break;
        }
        while (i > 0)
        {
            AMODE *ap1 = xalloc(sizeof(AMODE));
            memcpy(ap1, ap, sizeof(AMODE));
            ap1->offset = makenode(en_addstruc, ap->offset, makeintnode(en_icon,
                (i - 4)));
			noRelease = TRUE;
            gen_codes(op_push, BESZ_DWORD , ap1, 0);
            i = i - 4;
        }

    }
    return (sz);
}

//-------------------------------------------------------------------------

int gen_parms(ENODE *plist, int areg, int sreg)
/*
 *      push a list of parameters onto the stack and return the
 *      size of parameters pushed.
 */
{
    int i;
    i = 0;
    while (plist != 0)
    {
		int l;
		MarkRegs();
        if (plist->nodetype == en_stackblock)
            l = push_stackblock(plist->v.p[0], areg, sreg);
        else
            l = push_param(plist->v.p[0], areg, sreg, natural_size(plist->v.p[0]));
		ReleaseRegs();
		i += l;
		last_align -= l;
		if (last_align < 0)
			last_align -= 16 * (last_align/16 - 1);
        plist = plist->v.p[1];
    }
    return i;
}

int parmsize(ENODE *plist)
{
    int i;
    i = 0;
    while (plist != 0)
    {
        if (plist->nodetype == en_stackblock)
		{
            i += (plist->size + 3) & -4;
		}
        else
		{
			int size = natural_size(plist->v.p[0]);
		    switch (plist->nodetype)
    		{
				
        		case en_absacon:
					i += 4;
					break;
		        case en_napccon:
        		case en_nacon:
		        case en_labcon:
        		case en_nalabcon:
					i += 4;
					break;
		        case en_cfc:
        		case en_fcomplexref:
					i += 8;
					break;
		        case en_crc:
        		case en_rcomplexref:
					i += 16;
		            break;
		        case en_clrc:
        		case en_lrcomplexref:
					i += 20;
					break;
		        case en_cfi:
        		case en_fimaginaryref:
					i += 4;
		            break;
		        case en_cf:
        		case en_floatref:
					i += 4;
					break;
		        case en_cd:
        		case en_doubleref:
					i += 8;
					break;
		        case en_cri:
        		case en_rimaginaryref:
					i += 8;
					break;
		        case en_cld:
        		case en_longdoubleref:
					i += 12;
					break;
		        case en_clri:
        		case en_lrimaginaryref:
					i += 12;
					break;
		        case en_cfp:
        		case en_fp_ref:
					i += 8;
					break;
		        case en_csp:
					i += 4;
		            break;
        		default:
		            if (plist->nodetype == en_thiscall && plist->v.p[0]->nodetype ==
        		        en_substack)
		            {
                		i += plist->v.p[0]->v.p[0]->v.p[0]->v.i; // assume an icon node
		                break;
        		    }
		            if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
        		    {
                		i += 8;
            		}
		            else if (size <= BESZ_BOOL)
        		    {
						i += 4;
		            }
		            else if (size >= BESZ_CFLOAT)
        		    {
						if (size == BESZ_CDOUBLE)
							i += 16;
						else if (size == BESZ_CLDOUBLE)
							i += 24;
						else
							i += 8;
					}
		            else
        		    {
						
		                if (size == BESZ_FLOAT || size == BESZ_IFLOAT)
        		            i += 4;
                		else if (size == BESZ_LDOUBLE || size == BESZ_IDOUBLE)
                    		i += 12;
						else 
							i += 8;
		            }
		            break;
    		}
		}
        plist = plist->v.p[1];
    }
    return i;
}
//-------------------------------------------------------------------------

AMODE *gen_tcall(ENODE *node, int areg, int sreg, int novalue)
/*
 *      generate a trap call node and return the address mode
 *      of the result.
 */
{
    AMODE *ap,  *result;
    int i, siz1;
    int regax, regdx, regcx;
    int regfs, reggs, reges;
    regax = regs[0];
    regcx = regs[1];
    regdx = regs[2];
    regfs = sregs[0];
    reggs = sregs[1];
    reges = sregs[2];
    if (regfs)
        GenPush(FS + 24, am_dreg, 0);
    if (reggs)
        GenPush(GS + 24, am_dreg, 0);
    if (reges)
        GenPush(ES + 24, am_dreg, 0);
    if (regcx)
        GenPush(ECX, am_dreg, 0);
    if (regdx)
        GenPush(EDX, am_dreg, 0);
    if (regax)
        GenPush(EAX, am_dreg, 0);
    regs[0] = regs[1] = regs[2] = 0;
    sregs[0] = sregs[1] = sregs[2] = 0;
    siz1 = node->v.p[0]->v.i;
    gen_code(op_int, make_immed((int)node->v.p[0]->v.p[0]), 0);
    regs[0] = regax;
    regs[1] = regcx;
    regs[2] = regdx;
    sregs[0] = regfs;
    sregs[1] = reggs;
    sregs[2] = reges;
    if (!novalue)
    {
        result = RealizeDreg(areg);
        result->length = siz1;
        if (result->preg != EAX)
            gen_codes(op_mov, siz1, result, makedreg(EAX));
    }
    if (regax)
        GenPop(EAX, am_dreg, 0);
    if (regdx)
        GenPop(EDX, am_dreg, 0);
    if (regcx)
        GenPop(ECX, am_dreg, 0);
    if (reges)
        GenPop(ES + 24, am_dreg, 0);
    if (reggs)
        GenPop(GS + 24, am_dreg, 0);
    if (regfs)
        GenPop(FS + 24, am_dreg, 0);
    return result;
}

//-------------------------------------------------------------------------

static AMODE *inlinecall(ENODE *node, int areg, int sreg, int novalue)
{
    ENODE *nameref = node,  *thisn = 0;
    SYM *sp;
    int size;
    int dregs[3];
    int selreg;
    int i;
    memcpy(dregs, regs, sizeof(dregs));
    if (nameref->nodetype == en_thiscall)
    {
        thisn = nameref->v.p[0];
        nameref = nameref->v.p[1];
    }

    if (nameref->nodetype == en_callblock || nameref->nodetype == en_scallblock)
    {
        size = 4;
        nameref = nameref->v.p[1];
    }
    else
        size = nameref->v.p[0]->v.i;
    nameref = nameref->v.p[1]->v.p[0]->v.p[0];
    if (nameref->nodetype == en_nacon || nameref->nodetype == en_napccon)
    {
        sp = nameref->v.sp;
        if (sp && (sp->value.classdata.cppflags &PF_INLINE))
        {
            int oldretlab = retlab;
            AMODE *ap;
            SYM *oldcurfunc = currentfunc;
            genning_inline++;
            if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
                temp_axdx();
            currentfunc = sp;
            retlab = nextlabel++;
            genstmt(sp->value.classdata.inlinefunc->stmt);
            genreturn(0, 3);
            genning_inline--;
            selreg = 0;
            for (i = 0; i < 3; i++)
                dregs[i] -= regs[i];
            for (i = 0; i < 3; i++)
                if (dregs[i] < dregs[selreg])
                    selreg = i;
            if (size >= 100)
            {
                ap = makedreg(selreg);
                ;
                ap->length = size - 100000;
                ap->mode = am_indisp;
            }
            else if (size ==  BESZ_FARPTR)
            {
                ap = makedreg(EDX);
                gen_code(op_push, ap, 0);
                ap->mode = am_seg;
                ap->seg = FS;
                gen_code(op_pop, ap, 0);
                ap->preg = EAX;
                ap->mode = am_dreg;
                ap->length =  BESZ_FARPTR;
            }
            else if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
            {
                ap = xalloc(sizeof(AMODE));
                ap->length = size < 0 ?  - BESZ_QWORD: BESZ_QWORD;
                ap->mode = am_axdx;
                regs[0]++;
                regs[2]++;
            }
            else if (size > BESZ_QWORD && size < BESZ_CFLOAT)
            {
                ap = fstack();
                ap->length = size;
                if (novalue)
                    gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
            }
            else if (size >= BESZ_CFLOAT)
            {
                ap = fstack();
                ap->mode = am_frfr;
                ap->length = BESZ_CLDOUBLE;
                if (novalue)
                {
                    gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
                    gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
                }
            }
            else
            {
                if (size != 0)
                {
                    ap = makedreg(selreg);
                    ap->length = size;
                }
                else
                    ap = makedreg(EAX);
            }
            currentfunc = oldcurfunc;
            retlab = oldretlab;
            return ap;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------
static AMODE *gen_fcall(ENODE *node, int areg, int sreg, int novalue)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    AMODE *ap,  *result,  *app = 0;
	OCODE *sa;
    ENODE *pushthis = 0;
	int retreg = -1;
	int save_last_align;
	int parm_size = 0, relsize = 0;
    int refret = FALSE;
    int i = 0, siz1=0;
    int regax, regdx, regcx;
    int regfs, reggs, reges;
    if (ap = inlinecall(node, areg, sreg, novalue))
        return ap;
    if (node->nodetype != en_callblock && node->nodetype != en_scallblock)
        if (ap = HandleIntrins(node, areg))
            return ap;
        if (node->nodetype == en_thiscall)
        {
            pushthis = node->v.p[0];
            node = node->v.p[1];
            i = 4;
        }
    if (node->nodetype == en_callblock || node->nodetype == en_scallblock)
    {
        siz1 = BESZ_DWORD;
    }
    else
    {
        siz1 = node->v.p[0]->v.i;
        if (siz1 > 100)
        {
            siz1 -= 100000;
            refret = TRUE;
        }
    }
    regax = regs[0];
    regcx = regs[1];
    regdx = regs[2];
    regfs = sregs[2];
    reggs = sregs[0];
    reges = sregs[1];
	if (refret || siz1 < BESZ_QWORD && siz1 > - BESZ_QWORD)
		retreg = areg;
	if (prm_stackalign)
	{
		save_last_align = last_align;
		gen_code(op_add, makedreg(ESP), make_immed(0)); // will be filled in later
		sa = peep_tail;
	}
    if (siz1 == BESZ_QWORD || siz1 ==  - BESZ_QWORD)
	{
		int regpos1 = regpos;
		if (retreg == ECX)
		{
			if (regs[retreg])
			{
				parm_size += 4;
			}
			RealizeDreg(ECX);
		}
		if (siz1 == BESZ_QWORD || siz1 == - BESZ_QWORD)
		{
			temp_axdx();
			MarkRegs();
		}
		else
		{
			MarkRegs();
			temp_axdx();
		}
		if (regpos1 != regpos)
		{
			if (pushedregs[regpos1] == EDXEAX)
			{
				regax++, regdx++;
			}
			else if (pushedregs[regpos1] == EDX)
			{
				regdx++;
			}
			else if (pushedregs[regpos1] == EAX)
			{
				regax++;
			}
		}
	}
    else
    {
		if (retreg >= 0)
		{
			if (regs[retreg])
			{
				parm_size += 4;
			}
			RealizeDreg(retreg);
		}
		MarkRegs();
        if (regdx && retreg != EDX)
		{
			RealizeDreg(EDX);
			if (regdx != regs[2])
				parm_size += 4;
		}
        if (regax && retreg != EAX)
		{
			RealizeDreg(EAX);
			if (regax != regs[0])
				parm_size += 4;
		}
    }
    if (regcx && retreg != ECX)
	{
		RealizeDreg(ECX);
		if (regcx != regs[1])
			parm_size += 4;
	}
    if (regfs > 1)
	{
		RealizeSreg(FS);
		if (regfs != sregs[2])
			parm_size += 4;
	}
    if (reggs)
	{
		RealizeSreg(GS);
		if (reggs != sregs[0])
			parm_size += 4;
	}
    if (reges)
	{
		RealizeSreg(ES);
		if (reges != sregs[1])
			parm_size += 4;
	}
    regax = regs[0];
    regcx = regs[1];
    regdx = regs[2];
    regfs = sregs[2];
    reggs = sregs[0];
    reges = sregs[1];
    regs[0] = regs[1] = regs[2] = 0;
    sregs[0] = sregs[1] = sregs[2] = 0;
    SetFuncMode(1);
    if (node->nodetype == en_callblock || node->nodetype == en_scallblock)
    {
		int areg1, sreg1;
		if (prm_stackalign)
		{
			int l;
			parm_size += 4 + parmsize(node->v.p[1]->v.p[1]->v.p[1]->v.p[0]);
			l = 16 - parm_size % 16;
			l = (l + 8 + last_align) % 16;
			relsize = l;
			last_align = 16 - l;
		}
       	i = i + gen_parms(node->v.p[1]->v.p[1]->v.p[1]->v.p[0], areg, sreg);

		MarkRegs();			
		ChooseRegs(&areg1, &sreg1);
        ap = gen_expr(node->v.p[0], areg1, sreg1, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
        gen_codes(op_push, BESZ_DWORD , ap, 0);
		ReleaseRegs();
        i += 4;
        node = node->v.p[1];
    }
    else
    {
		if (prm_stackalign)
		{
			int l;
			parm_size += parmsize(node->v.p[1]->v.p[1]->v.p[0]);
			l = 16 - parm_size % 16;
			l = (l + 8 + last_align) % 16;
			relsize = l;
			last_align = 16 - l;
		}
        i = i + gen_parms(node->v.p[1]->v.p[1]->v.p[0], areg, sreg); /* generate parameters
            */
    }
    if (node->nodetype == en_intcall)
    {
        AMODE *ap2 = xalloc(sizeof(AMODE));
        ap2->mode = am_seg;
        ap2->seg = e_cs;
        gen_code(op_pushfd, 0, 0);
        gen_code(op_push, ap2, 0);
    }
    else if (pushthis)
    {
		MarkRegs();
        app = gen_expr(pushthis, areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
        gen_codes(op_push, BESZ_DWORD , app, 0);
		ReleaseRegs();
    }

    if (node->v.p[1]->v.p[0]->v.p[0]->nodetype == en_nacon || node->v.p[1]
        ->v.p[0]->v.p[0]->nodetype == en_napccon)
    {
        SYM *sp = node->v.p[1]->v.p[0]->v.p[0]->v.sp;
        if (sp->inreg)
        {
            ap = makedreg(sp->value.i);
        }
        else
        {
            node->v.p[1]->v.p[0]->v.p[0]->v.sp = sp;
            ap = make_offset(node->v.p[1]->v.p[0]->v.p[0]);
            if (sp->indirect)
            {
                ap->mode = am_direct;
            }
            else
                ap->mode = am_immed;
        }
		if (node->v.p[1]->v.p[0]->v.p[0]->nodetype != en_autocon)
			ap->seg = defseg(node->v.p[1]->v.p[0]->v.p[0]);
        if (sp->farproc)
            gen_code(op_push, makesegreg(CS), 0);
        gen_codes(op_call, 0, ap, 0);
    }
    else
    {
		MarkRegs();
        ap = gen_expr(node->v.p[1]->v.p[0]->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM, BESZ_DWORD );
        if (ap->mode == am_axdx)
        {
            AMODE *ap1 = floatconvpos();
            AMODE *ap2 = copy_addr(ap1);
            ap2->offset = makenode(en_add, ap2->offset, makeintnode(en_icon, 4))
                ;
            gen_codes(op_mov, BESZ_DWORD , ap2, makedreg(EDX));
            gen_codes(op_mov, BESZ_DWORD , ap1, makedreg(EAX));
            gen_codes(op_call, BESZ_BEFARPTR, ap1, 0);
        }
        else
		{
			if (ap->mode != am_immed)
			{
				ENODE *exp = node->v.p[1]->v.p[0]->v.p[0];
				if (lvalue(exp))
					exp = exp->v.p[0];
				if (exp->nodetype != en_autocon && !ap->seg)
					ap->seg = defseg(exp);
			}
            gen_codes(op_call, ap->length ==  BESZ_FARPTR ? BESZ_BEFARPTR : BESZ_DWORD , ap, 0);
		}
		ReleaseRegs();
    }
    if (node->nodetype == en_fcall || node->nodetype == en_fcallb || node
        ->nodetype == en_callblock)
    if (i != 0)
    {
        if (i == 4)
            gen_codes(op_pop, BESZ_DWORD , makedreg(ECX), 0);
        else
            gen_code(op_add, makedreg(ESP), make_immed(i));
    }
    regs[0] = regax;
    regs[1] = regcx;
    regs[2] = regdx;
    sregs[2] = regfs;
    sregs[0] = reggs;
    sregs[1] = reges;
    SetFuncMode(0);
    if (refret)
    {
        result = makedreg(areg);
        result->length = siz1;
        if (result->preg != EAX)
            gen_codes(op_mov, siz1, result, makedreg(EAX));
        result->mode = am_indisp;
    }
    else if (siz1 ==  BESZ_FARPTR)
    {
        result = makedreg(EDX);
        gen_code(op_push, result, 0);
        result->mode = am_seg;
        result->seg = FS;
        gen_code(op_pop, result, 0);
        result->preg = EAX;
        result->mode = am_dreg;
        result->length =  BESZ_FARPTR;
    }
    else if (siz1 == BESZ_QWORD || siz1 ==  - BESZ_QWORD)
    {
		result = makedreg(EAX);
		result->mode = am_axdx;
		
        result->length = siz1 < 0 ?  - BESZ_QWORD: BESZ_QWORD;
    }
    else if (siz1 > BESZ_QWORD && siz1 < BESZ_CFLOAT)
    {
        result = fstack();
        result->length = siz1;
        if (novalue)
            gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
    }
    else if (siz1 >= BESZ_CFLOAT)
    {
        result = fstack();
        result->mode = am_frfr;
        result->length = BESZ_CLDOUBLE;
        if (novalue) 
        {
            gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
            gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
        }
    }
    else
    {
        if (siz1 != 0)
        {
            result = makedreg(areg);
            result->length = siz1;
            if (areg != EAX)
                gen_codes(op_mov, siz1, result, makedreg(EAX));
        }
        else
		{
            result = makedreg(EAX);
		}
    }
	ReleaseRegs();
	if (prm_stackalign)
	{
		if (relsize)
		{
			sa->oper2->offset->v.i = -relsize;
	        if (relsize == 4)
    	        gen_codes(op_pop, BESZ_DWORD , makedreg(ECX), 0);
        	else
				gen_code(op_add, makedreg(ESP), make_immed(relsize));
		}
		else
			remove_peep_entry(sa);
		last_align = save_last_align;
	}
    return result;
}

//-------------------------------------------------------------------------

static AMODE *gen_repcons(ENODE *node, int areg, int sreg)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    AMODE *ax,  *cx,  *ap,  *ap1;
    ENODE *pushthis = 0,  *onode = node;
    int i = 0, siz1;
    int lab;
    int regax, regdx, regcx;
    node = node->v.p[1];
        if (node->nodetype == en_thiscall)
        {
            pushthis = node->v.p[0];
            node = node->v.p[1];
            i = 4;
        }
	MarkRegs();
    ap1 = gen_expr(onode->v.p[0]->v.p[0], ECX, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
    if (pushthis)
    {
        AMODE *ap2;
		ChooseRegs(&areg, &sreg);
		ap2 = gen_expr(pushthis, EAX, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
    }
    gen_label(lab = nextlabel++);

    gen_codes(op_push, BESZ_DWORD , cx, 0);
    gen_codes(op_push, BESZ_DWORD , ax, 0);

    if (node->v.p[1]->v.p[0]->v.p[0]->nodetype == en_nacon || node->v.p[1]
        ->v.p[0]->v.p[0]->nodetype == en_napccon)
    {
        SYM *sp = node->v.p[1]->v.p[0]->v.p[0]->v.sp;
        if (sp->inreg)
        {
            ap = makedreg(sp->value.i);
        }
        else
        {
            node->v.p[1]->v.p[0]->v.p[0]->v.sp = sp;
            ap = make_offset(node->v.p[1]->v.p[0]->v.p[0]);
            if (sp->indirect)
                ap->mode = am_direct;
            else
                ap->mode = am_immed;
        }
        if (sp->farproc)
            gen_code(op_push, makesegreg(CS), 0);
        gen_codes(op_call, 0, ap, 0);
    }
    else
    {
		ChooseRegs(&areg, &sreg);
        ap = gen_expr(node->v.p[1]->v.p[0]->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM, BESZ_DWORD );

        gen_codes(op_call, BESZ_DWORD , ap, 0);
    }
    gen_codes(op_pop, BESZ_DWORD , makedreg(ECX), 0);
    gen_codes(op_pop, BESZ_DWORD , makedreg(EAX), 0);
    gen_codes(op_add, BESZ_DWORD , ax, make_immed((int)onode->v.p[0]->v.p[1]->v.p[0]));
    gen_code(op_loop, make_label(lab), 0);
	ReleaseRegs();
}

//-------------------------------------------------------------------------

static AMODE *gen_pfcall(ENODE *node, int areg, int sreg, int novalue)
/*
 *      generate a function call node to a pascal function
 *			and return the address mode of the result.
 */
{
    AMODE *ap,  *result,  *app = 0;
    int i = 0, siz1;
    int refret = FALSE;
    ENODE *invnode = 0,  *anode;
    ENODE *pushthis = 0;
    int regax, regdx, regcx;
    int regfs, reggs, reges;
    if (ap = inlinecall(node, areg, sreg, novalue))
        return ap;

    /* invert the parameter list */
    if (node->nodetype == en_pcallblock || node->nodetype == en_scallblock)
        anode = node->v.p[1]->v.p[1]->v.p[1]->v.p[0];
    else
        anode = node->v.p[1]->v.p[1]->v.p[0];
    while (anode)
    {
        invnode = makenode(anode->nodetype, anode->v.p[0], invnode);
        anode = anode->v.p[1];
    }
        if (node->nodetype == en_thiscall)
        {
            pushthis = node->v.p[0];
            node = node->v.p[1];
            i = 4;
        }
    if (node->nodetype == en_pcallblock || node->nodetype == en_scallblock)
    {
        siz1 = 4;
    }
    else
    {
        if (siz1 > 100)
        {
            siz1 -= 100000;
            refret = TRUE;
        }
        siz1 = node->v.p[0]->v.i;
    }
    regax = regs[0];
    regcx = regs[1];
    regdx = regs[2];
    regfs = sregs[2];
    reggs = sregs[0];
    reges = sregs[1];
    SetFuncMode(1);
    if (regfs > 1)
        GenPush(FS + 24, am_dreg, 0);
    if (reggs)
        GenPush(GS + 24, am_dreg, 0);
    if (reges)
        GenPush(ES + 24, am_dreg, 0);
    if ((siz1 == BESZ_QWORD || siz1 ==  - BESZ_QWORD) && (regax && regdx))
    {
        temp_axdx();
    }
    else
    {
        if (regdx)
            GenPush(EDX, am_dreg, 0);
        if (regax)
            GenPush(EAX, am_dreg, 0);
    }
    if (regcx)
        GenPush(ECX, am_dreg, 0);
    regs[0] = regs[1] = regs[2] = 0;
    sregs[0] = sregs[1] = sregs[2] = 0;
    if (pushthis)
    {
		MarkRegs();
        app = gen_expr(pushthis, areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
        gen_codes(op_push, BESZ_DWORD , app, 0);
		ReleaseRegs();
    }
    if (node->nodetype == en_pcallblock || node->nodetype == en_scallblock)
    {
		MarkRegs();
        ap = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
        gen_codes(op_push, BESZ_DWORD , ap, 0);
		ReleaseRegs();
        i = i + 4;
        i += gen_parms(invnode, areg, sreg);
        node = node->v.p[1];
    }
    else
    {
        i = i + gen_parms(invnode, areg, sreg); /* generate parameters */
    }

    if (node->v.p[1]->v.p[0]->v.p[0]->nodetype == en_nacon || node->v.p[1]
        ->v.p[0]->v.p[0]->nodetype == en_napccon)
    {
        SYM *sp = node->v.p[1]->v.p[0]->v.p[0]->v.sp;
        if (sp->inreg)
        {
            ap = makedreg(sp->value.i);
        }
        else
        {
            node->v.p[1]->v.p[0]->v.p[0]->v.sp = sp;
            ap = make_offset(node->v.p[1]->v.p[0]->v.p[0]);
            if (sp->indirect)
            {
                ap->mode = am_direct;
            }
            else
                ap->mode = am_immed;
        }
        if (sp->farproc)
            gen_code(op_push, makesegreg(CS), 0);

		if (node->v.p[1]->v.p[0]->v.p[0]->nodetype != en_autocon)
			ap->seg = defseg(node->v.p[1]->v.p[0]->v.p[0]);
        gen_codes(op_call, 0, ap, 0);
    }
    else
    {
		MarkRegs();
        ap = gen_expr(node->v.p[1]->v.p[0]->v.p[0], areg, sreg, FALSE, F_AXDX | F_IMMED | F_MEM, BESZ_DWORD );
        if (ap->mode == am_axdx)
        {
            AMODE *ap1 = floatconvpos();
            AMODE *ap2 = copy_addr(ap1);
            ap2->offset = makenode(en_add, ap2->offset, makeintnode(en_icon, 4))
                ;
            gen_codes(op_mov, BESZ_DWORD , ap2, makedreg(EDX));
            gen_codes(op_mov, BESZ_DWORD , ap1, makedreg(EAX));
            gen_codes(op_call, BESZ_BEFARPTR, ap1, 0);
        }
        else
		{
			if (ap->mode != am_immed)
			{
				ENODE *exp = node->v.p[1]->v.p[0]->v.p[0];
				if (lvalue(exp))
					exp = exp->v.p[0];
				if (exp->nodetype != en_autocon && !ap->seg)
					ap->seg = defseg(exp);
			}
            gen_codes(op_call, ap->length ==  BESZ_FARPTR ? BESZ_BEFARPTR : BESZ_DWORD , ap, 0);
		}
		ReleaseRegs();
    }
    regs[0] = regax;
    regs[1] = regcx;
    regs[2] = regdx;
    sregs[0] = regfs;
    sregs[1] = reggs;
    sregs[2] = reges;
    SetFuncMode(0);
    if (refret)
    {
        result = RealizeDreg(areg);
        result->length = siz1;
        if (result->preg != EAX)
            gen_codes(op_mov, siz1, result, makedreg(EAX));
        //               result->mode = am_indisp ;
    }
    else if (siz1 ==  BESZ_FARPTR)
    {
        result = makedreg(EDX);
        gen_code(op_push, result, 0);
        result->mode = am_seg;
        result->seg = FS;
        gen_code(op_pop, result, 0);
        result->preg = EAX;
        result->mode = am_dreg;
        result->length =  BESZ_FARPTR;
    }
    else if (siz1 ==  - BESZ_QWORD || siz1 == BESZ_QWORD)
    {
        result = xalloc(sizeof(AMODE));
        result->length = siz1 < 0 ?  - BESZ_QWORD: BESZ_QWORD;
        ;
        result->mode = am_axdx;
        regs[0]++;
        regs[2]++;
    }
    else if (siz1 > BESZ_QWORD && siz1 < BESZ_CFLOAT)
    {
        result = fstack();
        result->length = siz1;
        if (novalue)
            gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
    }
    else if (siz1 >= BESZ_CFLOAT)
    {
        result = fstack();
        result->mode = am_frfr;
        result->length = BESZ_CLDOUBLE;
        if (novalue)
        {
            gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
            gen_codefs(op_fstp,BESZ_LDOUBLE,makefreg(0),0); // avoid exceptions
        }
    }
    else
    {
        if (siz1 != 0)
        {
            result = RealizeDreg(areg);;
            result->length = siz1;
            if (result->preg != EAX)
                gen_codes(op_mov, siz1, result, makedreg(EAX));
        }
        else
            result = makedreg(EAX);
    }
    if (regcx)
        GenPop(ECX, am_dreg, 0);
    if (siz1 != BESZ_QWORD && siz1 !=  - BESZ_QWORD)
    {
        if (regax)
            GenPop(EAX, am_dreg, 0);
        if (regdx)
            GenPop(EDX, am_dreg, 0);
    }
    if (reges)
        GenPop(ES + 24, am_dreg, 0);
    if (reggs)
        GenPop(GS + 24, am_dreg, 0);
    if (regfs > 1)
        GenPop(FS + 24, am_dreg, 0);
    return result;
}


//-------------------------------------------------------------------------

static AMODE *gen_void(ENODE *node, int areg, int sreg)
{
	MarkRegs();
    gen_expr(node, areg, sreg, TRUE, F_DREG | F_AXDX | F_IMMED | F_MEM | F_FREG | F_NOVALUE, natural_size(node));
	ReleaseRegs();
    gen_code(op_void, 0, 0);
    return 0;
}

//-------------------------------------------------------------------------

int natural_size(ENODE *node)
/*
 *      return the natural evaluation size of a node.
 */
{
    int siz0, siz1;
    if (node == 0)
        return 0;
    switch (node->nodetype)
    {
        case en_substack:
        case en_structret:
		case en_loadstack:
		case en_savestack:
            return BESZ_DWORD ;
        case en_bits:
        case en_cl_reg:
            return natural_size(node->v.p[0]);
        case en_icon:
        case en_lcon:
            return  - BESZ_DWORD ;
        case en_lucon:
        case en_iucon:
            return BESZ_DWORD ;
        case en_boolcon:
            return BESZ_BOOL;
        case en_llcon:
            return  - BESZ_QWORD;
        case en_llucon:
            return BESZ_QWORD;
        case en_ccon:
        case en_cucon:
            return BESZ_DWORD ;
        case en_rcon:
        case en_doubleref:
            return BESZ_DOUBLE;
        case en_lrcon:
        case en_longdoubleref:
            return BESZ_LDOUBLE;
        case en_fcon:
        case en_floatref:
            return BESZ_FLOAT;
        case en_fimaginaryref:
        case en_fimaginarycon:
        case en_cfi:
            return BESZ_IFLOAT;
        case en_rimaginaryref:
        case en_rimaginarycon:
        case en_cri:
            return BESZ_IDOUBLE;
        case en_lrimaginaryref:
        case en_lrimaginarycon:
        case en_clri:
            return BESZ_ILDOUBLE;
        case en_fcomplexref:
        case en_fcomplexcon:
        case en_cfc:
            return BESZ_CFLOAT;
        case en_rcomplexref:
        case en_rcomplexcon:
        case en_crc:
            return BESZ_CDOUBLE;
        case en_lrcomplexref:
        case en_lrcomplexcon:
        case en_clrc:
            return BESZ_CLDOUBLE;
        case en_trapcall:
        case en_labcon:
        case en_nacon:
        case en_autocon:
        case en_autoreg:
        case en_napccon:
        case en_absacon:
        case en_nalabcon:
            return stdaddrsize;
        case en_l_ref:
		case en_i_ref:
		case en_a_ref:
        case en_cl:
		case en_ci:
            return  - BESZ_DWORD ;
        case en_thiscall:
            return natural_size(node->v.p[0]);
        case en_sfcallb:
        case en_sfcall:
        case en_pfcall:
        case en_pfcallb:
             /* ignore pascal style now */
        case en_fcall:
        case en_fcallb:
        case en_intcall:
            return node->v.p[0]->v.i;
        case en_callblock:
        case en_pcallblock:
        case en_scallblock:
			return BESZ_DWORD ;
        case en_tempref:
        case en_regref:
            return node->v.i >> 16;
        case en_ul_ref:
        case en_cul:
		case en_cui:
		case en_ui_ref:
		case en_ua_ref:
            return BESZ_DWORD ;
        case en_csp:
            return 2;
        case en_cfp:
            return  BESZ_FARPTR;
        case en_fp_ref:
            return  BESZ_FARPTR;

        case en_cp:
            return stdaddrsize;
        case en_bool_ref:
        case en_cbool:
            return BESZ_BOOL;
        case en_ub_ref:
        case en_cub:
            return 1;
        case en_b_ref:
        case en_cb:
            return  - 1;
        case en_uw_ref:
        case en_cuw:
            return 2;
        case en_cw:
        case en_w_ref:
            return  - 2;
        case en_cd:
            return BESZ_DOUBLE;
        case en_cld:
            return 10;
        case en_cf:
            return BESZ_FLOAT;
        case en_cll:
        case en_ll_ref:
            return  - BESZ_QWORD;
        case en_cull:
        case en_ull_ref:
            return BESZ_QWORD;
        case en_not:
        case en_compl:
        case en_uminus:
        case en_assign:
        case en_refassign:
        case en_lassign:
        case en_ainc:
        case en_adec:
        case en_asuminus:
        case en_ascompl:
        case en_moveblock:
        case en_stackblock:
        case en_movebyref:
        case en_clearblock:
            return natural_size(node->v.p[0]);
        case en_add:
        case en_sub:
        case en_addstruc:
        case en_asadd:
        case en_assub:
            siz0 = natural_size(node->v.p[0]);
            siz1 = natural_size(node->v.p[1]);
            if (siz0 <= 10 && siz1 >= BESZ_IFLOAT && siz1 < BESZ_CFLOAT)
            {
                if (siz0 <= BESZ_FLOAT && siz1 ==BESZ_IFLOAT)
                    return BESZ_CFLOAT;
                if (siz0 <=BESZ_DOUBLE && siz1 <= BESZ_IDOUBLE)
                    return BESZ_CDOUBLE;
                return BESZ_CLDOUBLE;
            }
            if (siz1 <= 10 && siz0 >= BESZ_IFLOAT && siz0 < BESZ_CFLOAT)
            {
                if (siz1 <= BESZ_FLOAT && siz0 ==BESZ_IFLOAT)
                    return BESZ_CFLOAT;
                if (siz1 <=BESZ_DOUBLE && siz0 <= BESZ_IDOUBLE)
                    return BESZ_CDOUBLE;
                return BESZ_CLDOUBLE;
            }
            if (chksize(siz1, siz0))
                return siz1;
            else
                return siz0;
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_pmul:
		case en_arrayindex:
        case en_mul:
        case en_div:
        case en_pdiv:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_alsh:
        case en_arsh:
        case en_arshd:
        case en_asarshd:
        case en_lsh:
        case en_rsh:
            siz0 = natural_size(node->v.p[0]);
            siz1 = natural_size(node->v.p[1]);
            if (chksize(siz1, siz0))
                return siz1;
            else
                return siz0;
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
        case en_land:
        case en_lor:
			return BESZ_DWORD;
        case en_asalsh:
        case en_asarsh:
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
            siz0 = natural_size(node->v.p[0]);
            siz1 = natural_size(node->v.p[1]);
            if (chksize(siz1, siz0))
                return siz1;
            else
                return siz0;
		case en_array:
			return BESZ_DWORD;
        case en_void:
        case en_cond:
        case en_repcons:
            return natural_size(node->v.p[1]);
        case en_voidnz:
        case en_conslabel:
        case en_destlabel:
        case en_addcast:
        case en_dvoid:
            return natural_size(node->v.p[0]);
        default:
            DIAG("natural size error.");
            return BESZ_DWORD ;
    }
    return 0;
}

/*
 * subroutine evaluates a node determining how to test it for
 * non-zero
 */
static int defcond(ENODE *node, int areg, int sreg)
{
    AMODE *ap1;
    int rv = 0;
    int nsiz;
	MarkRegs();
    if ((nsiz = natural_size(node)) > BESZ_QWORD)
    {
        if (nsiz >= BESZ_CFLOAT)
        {
        ap1 = gen_expr(node, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_CLDOUBLE);
        gen_codef(op_fldz, 0, 0);
        gen_codef(op_fucom, makefreg(1), 0);
        if (regs[0])
            GenPush(EAX, am_dreg, 0);
        gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
        gen_codef(op_fwait, 0, 0);
        gen_codes(op_shl, BESZ_DWORD, makedreg(EAX), make_immed(8));
        gen_codef(op_fucomp, makefreg(2), 0);
        gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
        gen_codef(op_fwait, 0, 0);
        gen_codes(op_shr, BESZ_DWORD, makedreg(EAX), make_immed(8));
        gen_comment("; or ah, al (2 bytes)\n");
        gen_codes(op_genword, 0, make_immed(0x08), 0);
        gen_codes(op_genword, 0, make_immed(0xc4), 0);
        gen_codef(op_fstp, makefreg(0), 0);
        gen_codef(op_fstp, makefreg(0), 0);
        }
        else
        {
        ap1 = gen_expr(node, areg, sreg, FALSE, F_FREG | F_VOL, BESZ_LDOUBLE);
        gen_codef(op_fldz, 0, 0);
        gen_codef(op_fucompp, 0, 0);
        if (regs[0])
            GenPush(EAX, am_dreg, 0);
        gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
        gen_codef(op_fwait, 0, 0);
        }
        gen_codes(op_test, BESZ_DWORD, makedreg(EAX), make_immed(1<<10));
        gen_code(op_jz, make_label(nextlabel), 0);
        gen_codes(op_and, BESZ_DWORD, makedreg(EAX), make_immed(~(1<<14)));
        gen_label(nextlabel++);
        gen_code(op_sahf, 0, 0);
        if (regs[0])
            GenPop(EAX, am_dreg, 0);
    }
    else if (nsiz == BESZ_QWORD || nsiz ==  - BESZ_QWORD)
    {
        ap1 = gen_expr(node, areg, sreg, FALSE, F_AXDX | F_MEM, nsiz);
		if (ap1->mode == am_axdx)
		{
    	    gen_codes(op_or, BESZ_DWORD , makedreg(EAX), makedreg(EDX));
		}
		else
		{
			AMODE *ap2;
			AMODE *ap3 = xalloc(sizeof(AMODE));
			ChooseRegs(&areg, &sreg);
			ap2 = RealizeDreg(areg);
			*ap3 = *ap1;
			noRelease = TRUE;
    	    gen_codes(op_mov, BESZ_DWORD , ap2, ap1);
			ap3->offset = makenode(en_add, ap3->offset, makenode(en_icon, (void *)4, 0));
			gen_codes(op_or, BESZ_DWORD, ap2, ap3);
		}
    }
    else
    {
        if (isbit(node))
        {
            ap1 = gen_expr(node, areg, sreg, FALSE, F_MEM | F_DREG | F_STOREADR , nsiz);
            if (node->bits == 1)
            {
                rv = 1;
                gen_codes(op_bt, BESZ_DWORD , ap1, make_immed(node->startbit));
            }
            else
                gen_codes(op_test, nsiz, ap1, make_immed(mod_mask(node->bits) << node
                    ->startbit));
        }
        else
        {
            ap1 = gen_expr(node, areg, sreg, FALSE, F_DREG, nsiz);
            if (ap1->length ==  BESZ_FARPTR)
                ap1->length = - BESZ_DWORD ;
            gen_code(op_cmp, ap1, make_immed(0));
        }
    }
	ReleaseRegs();
    return rv;
}

//-------------------------------------------------------------------------

static AMODE *truerelat(ENODE *node, int areg, int sreg)
{
    AMODE *ap1;
    if (node == 0)
        return 0;
    switch (node->nodetype)
    {
        case en_eq:
            ap1 = gen_compare(node, areg, sreg, op_sete, op_setne, 0, 0, op_sete, op_setne, 0, 1)
                ;
            break;
        case en_ne:
            ap1 = gen_compare(node, areg, sreg, op_setne, op_sete, 0, 0, op_setne,
                op_sete, 0, 1);
            break;
        case en_lt:
            ap1 = gen_compare(node, areg, sreg, op_setl, op_setg, 0, 0, op_seta, op_setb, 0, 1)
                ;
            break;
        case en_le:
            ap1 = gen_compare(node, areg, sreg, op_setle, op_setge, 0, 0, op_setnc,
                op_setbe, 0, 1);
            break;
        case en_gt:
            ap1 = gen_compare(node, areg, sreg, op_setg, op_setl, 0, 0, op_setb, op_seta, 0, 1)
                ;
            break;
        case en_ge:
            ap1 = gen_compare(node, areg, sreg, op_setge, op_setle, 0, 0, op_setbe,
                op_setnc, 0, 1);
            break;
        case en_ult:
            ap1 = gen_compare(node, areg, sreg, op_setb, op_seta, 0, 0, op_seta, op_setb, 0, 1)
                ;
            break;
        case en_ule:
            ap1 = gen_compare(node, areg, sreg, op_setbe, op_setnc, 0, 0, op_setnc,
                op_setbe, 0, 1);
            break;
        case en_ugt:
            ap1 = gen_compare(node, areg, sreg, op_seta, op_setb, 0, 0, op_setb, op_seta, 0, 1)
                ;
            break;
        case en_uge:
            ap1 = gen_compare(node, areg, sreg, op_setnc, op_setbe, 0, 0, op_setbe,
                op_setnc, 0, 1);
            break;
        case en_not:
            if (isintconst(node->nodetype))
            {
                ap1 = gen_expr(node, areg, sreg, FALSE, F_DREG | F_VOL, 0);
                ap1->offset->v.i = !ap1->offset->v.i;
                break;
            }
            if (defcond(node->v.p[0], areg, sreg))
            {
                ap1 = RealizeDreg(areg);
                gen_codes(op_setnc, 1, ap1, 0);
            }
            else
            {
                ap1 = RealizeDreg(areg);
                gen_codes(op_sete, 1, ap1, 0);
            }
            break;
        default:
            DIAG("True-relat error");
            break;
    }
    ap1->length = BESZ_BYTE;
    return ap1;
}

//-------------------------------------------------------------------------

static int complex_relat(ENODE *node)
{
    if (!node)
        return 0;
    switch (node->nodetype)
    {
        case en_substack:
		case en_loadstack:
		case en_savestack:
        case en_structret:
            return 0;
        case en_cl_reg:
            return complex_relat(node->v.p[0]);
        case en_ull_ref:
        case en_ll_ref:
        case en_cull:
        case en_cll:
        case en_llcon:
        case en_llucon:
            return 1;
		case en_a_ref:
		case en_ua_ref:
		case en_i_ref:
		case en_ui_ref:
        case en_bits:
        case en_l_ref:
        case en_cl:
        case en_ul_ref:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cp:
        case en_cfp:
        case en_csp:
        case en_ub_ref:
        case en_cub:
        case en_bool_ref:
        case en_cbool:
        case en_b_ref:
        case en_fp_ref:
        case en_cb:
        case en_uw_ref:
        case en_cuw:
        case en_cw:
        case en_w_ref:
        case en_cd:
        case en_cld:
        case en_cf:
        case en_cfc:
        case en_crc:
        case en_clrc:
        case en_cfi:
        case en_cri:
        case en_clri:
        case en_uminus:
        case en_ainc:
        case en_adec:
        case en_moveblock:
        case en_stackblock:
        case en_movebyref:
        case en_clearblock:
            return complex_relat(node->v.p[0]);
        case en_icon:
        case en_lcon:
        case en_lucon:
        case en_iucon:
        case en_boolcon:
        case en_ccon:
        case en_cucon:
        case en_rcon:
        case en_doubleref:
        case en_lrcon:
        case en_longdoubleref:
        case en_fcon:
        case en_floatref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_fcomplexcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
        case en_absacon:
        case en_trapcall:
        case en_labcon:
        case en_nacon:
        case en_autocon:
        case en_autoreg:
        case en_napccon:
        case en_nalabcon:
        case en_tempref:
        case en_regref:
            return 0;
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
        case en_land:
        case en_lor:
        case en_not:
        case en_compl:
            return 1;
        case en_div:
        case en_udiv:
        case en_pdiv:
        case en_mod:
        case en_umod:
        case en_assign:
        case en_refassign:
        case en_lassign:
        case en_asuminus:
        case en_ascompl:
        case en_add:
        case en_sub:
        case en_addstruc:
        case en_umul:
        case en_pmul:
		case en_arrayindex:
        case en_mul:
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
		case en_array:
            return complex_relat(node->v.p[0]) || complex_relat(node->v.p[1]);
        case en_void:
        case en_cond:
        case en_voidnz:
            return complex_relat(node->v.p[1]);
        case en_dvoid:
            return complex_relat(node->v.p[0]);
        case en_conslabel:
        case en_destlabel:
        case en_addcast:
            return complex_relat(node->v.p[0]);
        case en_sfcall:
        case en_sfcallb:
        case en_scallblock:
        case en_pfcall:
        case en_pfcallb:
        case en_thiscall:
        case en_fcall:
        case en_intcall:
        case en_callblock:
        case en_fcallb:
        case en_pcallblock:
            return 0;
        default:
            DIAG("error in complex_relat routine.");
            return 1;
    }
}

//-------------------------------------------------------------------------

AMODE *gen_relat(ENODE *node, int areg, int sreg)
{
    long lab1;
    AMODE *ap1;
    int size = natural_size(node);
    if (size >= BESZ_QWORD || size ==  - BESZ_QWORD)
        size = BESZ_DWORD ;
    if (node->nodetype != en_land && node->nodetype != en_lor && !complex_relat
        (node->v.p[0]) && !complex_relat(node->v.p[1]))
    {
        ap1 = truerelat(node, areg, sreg);
    }
    else
    {
		OCODE *p, *q;
        lab1 = nextlabel++;
        truejp(node, areg, sreg, lab1);
		p = peep_tail;
        ap1 = RealizeDreg(areg);
		q = peep_tail;
        gen_codes(op_sub, BESZ_DWORD , ap1, ap1);
        gen_comment(
            "; TEST EAX,IMMED (branches around next four bytes of code)\n");
        gen_codes(op_genword, 0, make_immed(0xa9), 0);
        gen_label(lab1);
		if (p != q)
		{
			gen_code(op_push, q->oper1, 0);
		}
		else
		{
	        gen_codes(op_nop, 0, 0, 0);
		}
        gen_codes(op_sub, BESZ_DWORD , ap1, ap1);
        gen_codes(op_inc, BESZ_DWORD , ap1, 0);
        gen_comment("; end TEST EAX,IMMED\n");
        ap1->length = size;
    }
    return ap1;
}

//-------------------------------------------------------------------------

AMODE *gen_compare(ENODE *node, int areg, int sreg, int btype1, int btype2, int btype3, int btype4,
    int fbtype1, int fbtype2, int label, int truecc)
/*
 *      generate code to do a comparison of the two operands of
 *      node.
 */
{
	OCODE *origPeepTail = peep_tail;
    AMODE *ap1,  *ap2,  *ap3,  *ap4,  *ap5;
    int btype = btype1;
    int bitted = FALSE;
    int size = natural_size(node->v.p[0]);
	int size1 = natural_size(node->v.p[1]);
    if (chksize(size1, size))
		size = size1;
	MarkRegs();
    if (size > BESZ_QWORD)
    {
        if (size >= BESZ_CFLOAT)
        {
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG | F_VOL, BESZ_CLDOUBLE);
		ReleaseRegs();
		MarkRegs();
        ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_FREG | F_VOL, BESZ_CLDOUBLE);
		ReleaseRegs();
        gen_codef(op_fucomp, makefreg(2), 0);
        if (regs[0])
            GenPush(EAX, am_dreg, 0);
        gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
        gen_codef(op_fwait, 0, 0);
        gen_codes(op_shl, BESZ_DWORD, makedreg(EAX), make_immed(8));
        gen_codef(op_fucomp, makefreg(2), 0);
        gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
        gen_codef(op_fwait, 0, 0);
        gen_codes(op_shr, BESZ_DWORD, makedreg(EAX), make_immed(8));
        gen_comment("; or ah, al (2 bytes)\n");
        gen_codes(op_genword, 0, make_immed(0x08), 0);
        gen_codes(op_genword, 0, make_immed(0xc4), 0);
        gen_codef(op_fstp, makefreg(0), 0);
        gen_codef(op_fstp, makefreg(0), 0);
        }
        else
        {
        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_FREG | F_VOL, BESZ_LDOUBLE);
		ReleaseRegs();
		MarkRegs();
        ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_FREG | F_VOL, BESZ_LDOUBLE);
		ReleaseRegs();
        gen_codef(op_fucompp, 0, 0);
        if (regs[0])
            GenPush(EAX, am_dreg, 0);
        gen_codefs(op_fstsw, BESZ_WORD, makedreg(EAX), 0);
        gen_codef(op_fwait, 0, 0);
        }
        gen_codes(op_test, BESZ_DWORD, makedreg(EAX), make_immed(1<<10));
        gen_code(op_jz, make_label(nextlabel), 0);
        gen_codes(op_and, BESZ_DWORD, makedreg(EAX), make_immed(~(1<<14)));
        if (truecc)
        {
            if (fbtype1 == op_ja || fbtype1 == op_jae ||
                fbtype1 == op_seta || fbtype1 == op_setae)
                gen_codes(op_or, BESZ_DWORD, makedreg(EAX), make_immed(1<<8));
            else if (fbtype1 == op_jb || fbtype1 == op_jbe ||
                fbtype1 == op_setb || fbtype1 == op_setbe)
                gen_codes(op_and, BESZ_DWORD, makedreg(EAX), make_immed(~(1<<8)));
        }
        else
        {
            if (fbtype1 == op_ja || fbtype1 == op_jae)
                gen_codes(op_and, BESZ_DWORD, makedreg(EAX), make_immed(~(1<<8)));
            else if (fbtype1 == op_jb || fbtype1 == op_jbe)
                gen_codes(op_or, BESZ_DWORD, makedreg(EAX), make_immed(1<<8));
        }
        gen_label(nextlabel++);
        gen_code(op_sahf, 0, 0);
        if (regs[0])
            GenPop(EAX, am_dreg, 0);
        if (label)
        {
            gen_branch(fbtype1, make_label(label));
			ap1 = NULL;
        }
        else
        {
            ap1 = RealizeDreg(areg);
            ap1->length = BESZ_BYTE;
            gen_code(fbtype1, ap1, 0);
        }
        return ap1;
    }
    if (size == BESZ_QWORD || size ==  - BESZ_QWORD)
    {
        if (btype1 == op_je || btype1 == op_sete || btype1 == op_jne || btype1 
            == op_setne)
        {
			if (isintconst(node->v.p[0]->nodetype))
			{
		        ap1 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_AXDX | F_MEM, size);
				if (node->v.p[0]->v.i == 0)
				{
					if (ap1->mode == am_axdx)
					{
						gen_codes(op_or, BESZ_DWORD, makedreg(EAX), makedreg(EDX));
					}
					else
					{
						AMODE *ap3 = xalloc(sizeof(AMODE));
						*ap3 = *ap1;
						ChooseRegs(&areg, &sreg);
						ap2 = RealizeDreg(areg);
						noRelease = TRUE;
						gen_codes(op_mov, BESZ_DWORD, ap2, ap1);
						ap3->offset = makenode(en_add, ap3->offset, makenode(en_icon, (void *)4, 0));
						gen_codes(op_or, BESZ_DWORD, ap2, ap3);
					}
				}
				else
				{
					ChooseRegs(&areg, &sreg);
					ap1 = do_extend(ap1, areg, sreg, size, F_AXDX);
					ap2 = make_immed(node->v.p[0]->v.i);
					ap3 = make_immed(node->v.p[0]->v.i >> 32);
		            gen_code(op_sub, makedreg(EAX), ap2);
		            gen_code(op_sbb, makedreg(EDX), ap3);
	    	        gen_code(op_or, makedreg(EAX), makedreg(EDX));
				}
			}
			else if (isintconst(node->v.p[1]->nodetype))
			{
		        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_AXDX | F_MEM, size);
				if (node->v.p[1]->v.i == 0)
				{
					if (ap1->mode == am_axdx)
					{
						gen_codes(op_or, BESZ_DWORD, makedreg(EAX), makedreg(EDX));
					}
					else
					{
						AMODE *ap3 = xalloc(sizeof(AMODE));
						*ap3 = *ap1;
						ChooseRegs(&areg, &sreg);
						ap2 = RealizeDreg(areg);
						gen_codes(op_mov, BESZ_DWORD, ap2, ap1);
						ap3->offset = makenode(en_add, ap3->offset, makenode(en_icon, (void *)4, 0));
						gen_codes(op_or, BESZ_DWORD, ap2, ap3);
					}
				}
				else
				{
					ChooseRegs(&areg, &sreg);
					ap1 = do_extend(ap1, areg, sreg, size, F_AXDX);
					ap2 = make_immed(node->v.p[1]->v.i);
					ap3 = make_immed(node->v.p[1]->v.i >> 32);
		            gen_code(op_sub, makedreg(EAX), ap2);
		            gen_code(op_sbb, makedreg(EDX), ap3);
	    	        gen_code(op_or, makedreg(EAX), makedreg(EDX));
				}
			}
			else
				goto join1;
			ReleaseRegs();
            if (label)
            {
                gen_branch(fbtype1, make_label(label));
				ap1 = NULL;
            }
            else
            {
				int n;
                ap2 = reuseop(ap1, areg, &n);
                ap2->length = BESZ_BYTE;
                gen_code(fbtype1, ap2, 0);
				ap1 = ap2;
				FreeUnused(n);
            }
        }
        else
        {
            int templab ;
join1:
			templab = nextlabel++;
	        ap2 = gen_expr(node->v.p[1], ECX, sreg, FALSE, F_AXDX | F_IMMED | F_MEM, size);
	        if (ap2->mode == am_axdx )
	        {
	            ap4 = floatconvpos();
	            ap4->offset = makenode(en_add, ap4->offset, makeintnode(en_icon, 8))
	                ;
	            gen_codes(op_mov, BESZ_DWORD , ap4, makedreg(EAX));
	            ap3 = copy_addr(ap4);
	            ap3->offset = makenode(en_add, ap3->offset, makeintnode(en_icon, 4))
	                ;
	            gen_codes(op_mov, BESZ_DWORD , ap3, makedreg(EDX));
	            ap2 = ap4;
				ReleaseRegs();
				MarkRegs();
	        }
	        else
	        {
				if (ap2->mode == am_indispscale && ap2->preg < EBX && ap2->sreg < EBX)
				{
					int reg;
					ap3 = reuseop(ap2, ap2->preg, &reg);
					ap3->offset = makeintnode(en_icon, 0);
					gen_codes(op_lea, BESZ_DWORD, ap3, ap2);
					ap3->mode = am_indisp;
					FreeUnused(reg);
					ap2 = ap3;
				}
	            ap3 = copy_addr(ap2);
	            if (ap2->mode == am_immed)
	            {
	                ap3->offset = copy_enode(ap2->offset);
	                ap3->offset->v.i >>= 32;
	            }
	            else
	                ap3->offset = makenode(en_add, ap2->offset, makeintnode(en_icon,
	                    4));
	        }
	        if (ap2->mode == am_immed && ap2->offset->v.i == 0)
	        {
	            switch (btype1)
	            {
	                case op_setl:
	                    btype1 = op_sets;
	                    break;
	                case op_setge:
	                    btype1 = op_setns;
	                    break;
	                case op_jl:
	                    btype3 = op_js;
	                    break;
	                case op_jge:
	                    btype3 = op_jns;
	                    break;
	            }
	        }
			ChooseRegs(&areg, &sreg);
	        ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_VOL | F_AXDX, BESZ_QWORD);
            if (label)
            {
				if (ReleaseWouldPop())
				{
		            AMODE *ap6 = floatconvpos();
		            ap6->offset = makenode(en_add, ap6->offset, makeintnode(en_icon, 8))
		                ;
	                gen_codes(op_cmp, BESZ_DWORD , makedreg(EAX), ap2);
					gen_code (op_lahf, NULL, NULL);
					gen_code(op_mov, ap6, makedreg(EAX));
	                gen_codes(op_cmp, BESZ_DWORD , makedreg(EDX), ap3);
					ReleaseRegs();
					if (btype1 != op_je)
		                gen_branch(btype3, make_label(label));
					if (btype1 != op_jne)
	                	gen_branch(btype4, make_label(templab));
					if (regs[EAX])
						gen_code(op_push, makedreg(EAX), 0);
					gen_code(op_mov, makedreg(EAX), ap6);
					gen_code(op_sahf, NULL, NULL);
					if (regs[EAX])
						gen_code(op_pop, makedreg(EAX), 0);
        	        gen_branch(fbtype2, make_label(label));
	                gen_label(templab);
				}
				else
				{
					ReleaseRegs();
	                gen_codes(op_cmp, BESZ_DWORD , makedreg(EDX), ap3);
					if (btype1 != op_je)
		                gen_branch(btype3, make_label(label));
					if (btype1 != op_jne)
	                	gen_branch(btype4, make_label(templab));
	                gen_codes(op_cmp, BESZ_DWORD , makedreg(EAX), ap2);
	                gen_branch(fbtype2, make_label(label));
	                gen_label(templab);
				}
            }
            else
            {
                ap4 = floatconvpos();
                ap5 = copy_addr(ap4);
                ap5->offset = makenode(en_add, ap4->offset, makeintnode(en_icon,
                    1));
                gen_codes(op_cmp, BESZ_DWORD , makedreg(EDX), ap3);
                gen_codes(btype1, 1, ap4, 0);
                gen_codes(op_cmp, BESZ_DWORD , makedreg(EAX), ap2);
                gen_codes(btype1, 1, ap5, 0);
				ChooseRegs(&areg, &sreg);
                ap1 = RealizeDreg(areg);
                ap1->length = BESZ_BYTE;
                gen_codes(op_mov, 1, ap1, ap4);
                gen_codes(op_or, 1, ap1, ap1);
				ReleaseRegs();
                gen_branch(op_jz, make_label(templab));
                gen_codes(op_mov, 1, ap1, ap5);
                gen_label(templab);

            }
        }
        return ap1;
    }
    if ((isbit(node->v.p[0]) || isbit(node->v.p[1])) && (btype1 == op_je ||
        btype1 == op_jne || btype1 == op_sete || btype1 == op_setne))
    {
        ENODE *xnode;
        bitted = TRUE;
        if (isbit(node->v.p[0]))
        {
            ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_VOL, BESZ_DWORD );
			ChooseRegs(&areg, &sreg);
            ap1 = gen_expr(xnode = node->v.p[0], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM | F_STOREADR, BESZ_DWORD );
        }
        else
        {
            ap2 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL, BESZ_DWORD );
			ChooseRegs(&areg, &sreg);
            ap1 = gen_expr(xnode = node->v.p[1], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
        }
        if (ap1->length ==  BESZ_FARPTR)
            ap1->length =  - BESZ_DWORD ;
        if (ap2->length ==  BESZ_FARPTR)
            ap2->length =  - BESZ_DWORD ;
        if (ap2->mode == am_immed)
        {
            if (xnode->bits == BESZ_BYTE)
            {
                if (!(ap2->offset->v.i &1))
                {
                    gen_codes(op_bt, BESZ_DWORD , ap1, make_immed(xnode->startbit));
                    switch (btype1)
                    {
                        case op_je:
                            btype = op_jnc;
                            break;
                        case op_jne:
                            btype = op_jb;
                            break;
                        case op_sete:
                            btype = op_setnc;
                            break;
                        case op_setne:
                            btype = op_setb;
                            break;
                    }
                }
                else
                {
                    gen_codes(op_bt, BESZ_DWORD , ap1, make_immed(xnode->startbit));
                    switch (btype1)
                    {
                        case op_je:
                            btype = op_jb;
                            break;
                        case op_jne:
                            btype = op_jnc;
                            break;
                        case op_sete:
                            btype = op_setb;
                            break;
                        case op_setne:
                            btype = op_setnc;
                            break;
                    }
                }
            }
            else
                goto join;
        }
        else
        {
            join: ap1 = bit_load(ap1, areg, sreg, xnode);
            goto join2;
        }
    }
    else
    {
		ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG /*| F_VOL*/, natural_size(node->v.p[0]));
		ChooseRegs(&areg, &sreg);
		ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, natural_size(node->v.p[1]));
        join2: if (ap1->length ==  BESZ_FARPTR)
            ap1->length =  - BESZ_DWORD ;
        if (ap2->length ==  BESZ_FARPTR)
            ap2->length =  - BESZ_DWORD ;
        if (ap1->mode == am_screg || ap1->mode == am_sdreg || ap1->mode ==
            am_streg || ap1->mode == am_seg)
		{
			ChooseRegs(&areg, &sreg);
            ap1 = do_extend(ap1, areg, sreg, BESZ_DWORD , F_DREG);
		}
        if (ap2->mode == am_screg || ap2->mode == am_sdreg || ap2->mode ==
            am_streg || ap2->mode == am_seg)
		{
			ChooseRegs(&areg, &sreg);
            ap2 = do_extend(ap2, areg, sreg, BESZ_DWORD , F_DREG);
		}
        if (is_memory(ap1) && is_memory(ap2) || ap1->mode == am_immed && ap2
            ->mode == am_immed)
        {
			ChooseRegs(&areg, &sreg);
            ap1 = do_extend(ap1, areg, sreg, ap2->length, F_DREG);
            gen_code(op_cmp, ap1, ap2);
        }
        else
        {
            if (ap1->mode == am_immed)
            {
                btype = btype2;
//				ChooseRegs(&areg, &sreg);
//                ap2 = do_extend(ap2, areg, sreg, BESZ_DWORD , F_DREG);
                gen_code(op_cmp, ap2, ap1);
            }
            else
                gen_code(op_cmp, ap1, ap2);
        }
    }
    /* Cmp turns into test int peep386... or goes away altogether
     * if comparing against zero to support this and the
     * sub->dec optimization we have to tiddle with the branch
     * code when comparing unsigneds against zero */
    if (!bitted && ap2->mode == am_immed && ap2->offset->v.i == 0)
    {
        switch (btype)
        {
            case op_seta:
                btype = op_setnz;
                break;
            case op_setae:
                gen_code(op_clc, 0, 0);
                break;
            case op_setb:
                gen_code(op_clc, 0, 0);
                break;
            case op_setbe:
                btype = op_sete;
                break;
            case op_ja:
                btype = op_jnz;
                break;
            case op_jae:
                btype = op_jmp; /* Following code never gets executed */
                break;
            case op_jb:
                 /* Following code always gets executed */
				 ReleaseRegs();
				 peep_tail = origPeepTail;
				 peep_tail->fwd = NULL;
                return ap1;
            case op_jbe:
                btype = op_jz;
                break;
            case op_setl:
                btype = op_sets;
                break;
            case op_setge:
                btype = op_setns;
                break;
            case op_jl:
                btype = op_js;
                break;
            case op_jge:
                btype = op_jns;
                break;
        }
    }
	ReleaseRegs();
    if (label)
    {
        gen_branch(btype, make_label(label));
		ap1 = NULL;
    }
    else
    {
        ap1 = RealizeDreg(areg);
        ap1->length = BESZ_BYTE;
        gen_code(btype, ap1, 0);
    }
    return ap1;
}

//-------------------------------------------------------------------------

void truejp(ENODE *node, int areg, int sreg, int label)
/*
 *      generate a jump to label if the node passed evaluates to
 *      a true condition.
 */
{
    int lab0;
    if (node == 0)
        return ;
    switch (node->nodetype)
    {
        case en_eq:
            gen_compare(node, areg, sreg, op_je, op_jne, op_je, op_jne, op_je, op_je, label, 1);
            break;
        case en_ne:
            gen_compare(node, areg, sreg, op_jne, op_je, op_jne, op_je, op_jne, op_jne,
                label, 1);
            break;
        case en_lt:
            gen_compare(node, areg, sreg, op_jl, op_jg, op_jl, op_jg, op_ja, op_jb, label, 1);
            break;
        case en_le:
            gen_compare(node, areg, sreg, op_jle, op_jge, op_jl, op_jg, op_jae, op_jbe,
                label, 1);
            break;
        case en_gt:
            gen_compare(node, areg, sreg, op_jg, op_jl, op_jg, op_jl, op_jb, op_ja, label, 1);
            break;
        case en_ge:
            gen_compare(node, areg, sreg, op_jge, op_jle, op_jg, op_jl, op_jbe, op_jnc,
                label, 1);
            break;
        case en_ult:
            gen_compare(node, areg, sreg, op_jb, op_ja, op_jb, op_ja, op_ja, op_jb, label, 1);
            break;
        case en_ule:
            gen_compare(node, areg, sreg, op_jbe, op_jnc, op_jb, op_ja, op_jae, op_jbe,
                label, 1);
            break;
        case en_ugt:
            gen_compare(node, areg, sreg, op_ja, op_jb, op_ja, op_jb, op_jb, op_ja, label, 1);
            break;
        case en_uge:
            gen_compare(node, areg, sreg, op_jnc, op_jbe, op_ja, op_jb, op_jbe, op_jnc,
                label, 1);
            break;
        case en_land:
            lab0 = nextlabel++;
            falsejp(node->v.p[0], areg, sreg, lab0);
            truejp(node->v.p[1], areg, sreg, label);
            gen_label(lab0);
            break;
        case en_lor:
            truejp(node->v.p[0], areg, sreg, label);
            truejp(node->v.p[1], areg, sreg, label);
            break;
        case en_not:
            falsejp(node->v.p[0], areg, sreg, label);
            break;
        default:
            if (isintconst(node->nodetype))
            {
                if (node->v.i != 0)
                    gen_code(op_jmp, make_label(label), 0);
                break;
            }
            if (defcond(node, areg, sreg))
                gen_branch(op_jc, make_label(label));
            else
                gen_branch(op_jne, make_label(label));
            break;
    }
}

//-------------------------------------------------------------------------

void falsejp(ENODE *node, int areg, int sreg, int label)
/*
 *      generate code to execute a jump to label if the expression
 *      passed is false.
 */
{
    int lab0;
    if (node == 0)
        return ;
    switch (node->nodetype)
    {
        case en_eq:
            gen_compare(node, areg, sreg, op_jne, op_je, op_jne, op_je, op_jne, op_jne,
                label, 0);
            break;
        case en_ne:
            gen_compare(node, areg, sreg, op_je, op_jne, op_je, op_jne, op_je, op_je, label, 0);
            break;
        case en_lt:
            gen_compare(node, areg, sreg, op_jge, op_jle, op_jg, op_jl, op_jbe, op_jnc,
                label, 0);
            break;
        case en_le:
            gen_compare(node, areg, sreg, op_jg, op_jl, op_jg, op_jl, op_jb, op_ja, label, 0);
            break;
        case en_gt:
            gen_compare(node, areg, sreg, op_jle, op_jge, op_jl, op_jg, op_jae, op_jbe,
                label, 0);
            break;
        case en_ge:
            gen_compare(node, areg, sreg, op_jl, op_jg, op_jl, op_jg, op_ja, op_jb, label, 0);
            break;
        case en_ult:
            gen_compare(node, areg, sreg, op_jnc, op_jbe, op_ja, op_jb, op_jbe, op_jnc,
                label, 0);
            break;
        case en_ule:
            gen_compare(node, areg, sreg, op_ja, op_jb, op_ja, op_jb, op_jb, op_ja, label, 0);
            break;
        case en_ugt:
            gen_compare(node, areg, sreg, op_jbe, op_jnc, op_jb, op_ja, op_jae, op_jbe,
                label, 0);
            break;
        case en_uge:
            gen_compare(node, areg, sreg, op_jb, op_ja, op_jb, op_ja, op_ja, op_jb, label, 0);
            break;
        case en_land:
            falsejp(node->v.p[0], areg, sreg, label);
            falsejp(node->v.p[1], areg, sreg, label);
            break;
        case en_lor:
            lab0 = nextlabel++;
            truejp(node->v.p[0], areg, sreg, lab0);
            falsejp(node->v.p[1], areg, sreg, label);
            gen_label(lab0);
            break;
        case en_not:
            truejp(node->v.p[0], areg, sreg, label);
            break;
        default:
            if (isintconst(node->nodetype))
            {
                if (node->v.i == 0)
                    gen_code(op_jmp, make_label(label), 0);
                break;
            }
            if (defcond(node, areg, sreg))
                gen_branch(op_jnc, make_label(label));
            else
                gen_branch(op_je, make_label(label));
            break;
    }
}
//-------------------------------------------------------------------------

static AMODE *gen_expr(ENODE *node, int areg, int sreg, int novalue, int flags, int size)
/*
 *      general expression evaluation. returns the addressing mode
 *      of the result.
 */
{
    AMODE *ap1,  *ap2;
    int lab0, lab1, i;
    SYM *sp;
    if (node == 0)
    {
        // CPP generates null nodes for cons & des sometimes
        //                DIAG("null node in gen_expr.");
        return 0;
    }
    switch (node->nodetype)
    {
        case en_cl_reg:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags, size);
			break;
        case en_bits:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags, natural_size(node->v.p[0]));
			if (!(flags & F_STOREADR))
	            ap1 = bit_load(ap1, areg, sreg, node);
			break;
        case en_cf:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags | F_FREG | F_VOL, natural_size(node));
            if (ap1->length > BESZ_FLOAT && ap1->length <=BESZ_LDOUBLE)
            {
//                ap1 = truncateFloat(ap1, BESZ_FLOAT);
            }
			break;
        case en_cfi:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags | F_FREG | F_VOL, natural_size(node));
            if (ap1->length > BESZ_IFLOAT && ap1->length <= BESZ_ILDOUBLE)
            {
//                ap1 = truncateFloat(ap1, BESZ_IFLOAT);
            }
			break;
        case en_cd:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags | F_FREG | F_VOL, natural_size(node));
            if (ap1->length == BESZ_LDOUBLE)
            {
//                ap1 = truncateFloat(ap1, BESZ_DOUBLE);
            }
			break;
        case en_cri:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags | F_FREG | F_VOL, natural_size(node));
            if (ap1->length == BESZ_IFLOAT)
            {
                ap1 = do_extend(ap1, areg, sreg, size, F_VOL);
            }
            else if (ap1->length == BESZ_ILDOUBLE)
            {
//                ap1 = truncateFloat(ap1, BESZ_IDOUBLE);
            }
			break;
        case en_cld:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags | F_FREG | F_VOL, natural_size(node));
            if (ap1->length >= BESZ_FLOAT && ap1->length < BESZ_LDOUBLE)
            {
                ap1->length = ap1->length == BESZ_FLOAT ? BESZ_FLOAT : BESZ_DOUBLE ;
                ap1 = do_extend(ap1, areg, sreg, size, F_VOL);
            }
			break;
        case en_clri:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags | F_FREG | F_VOL, natural_size(node));
            if (ap1->length >= BESZ_IFLOAT && ap1->length < BESZ_ILDOUBLE)
            {
                ap1->length = ap1->length == BESZ_IFLOAT ? BESZ_IFLOAT : BESZ_IDOUBLE ; //(ap1->length == BESZ_FLOAT || ap1->length == BESZ_IFLOAT) ? BESZ_IFLOAT : BESZ_IDOUBLE;
                ap1 = do_extend(ap1, areg, sreg, size, F_VOL);
            }
			break;
        case en_csp:
            if (!prm_farkeyword)
            {
                ap1 = make_immed(0);
            }
            else
            {
                ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, flags, BESZ_DWORD );

                if ((ap1->mode != am_immed) && (ap1->mode != am_seg))
                {
                    if ((ap1->mode == am_indisp || ap1->mode == am_indispscale 
                        || ap1->mode == am_direct) && ap1->length ==  BESZ_FARPTR)
                    {
                        ap1->offset = makenode(en_add, ap1->offset, makeintnode
                            (en_icon, 4));
                        ap1->length = BESZ_WORD;
						ChooseRegs(&areg, &sreg);
                        ap1 = do_extend(ap1, areg, sreg, BESZ_DWORD , F_DREG | F_VOL);
                    }
                    else if (ap1->length ==  BESZ_FARPTR)
                    {
                        int seg = ap1->seg;
                        ap1->length = BESZ_DWORD ;
                        ap1->seg = 0;
                        ap1->mode = am_seg;
                        ap1->seg = seg;
                    }
                }
            }
			break;
        case en_cfp:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags | F_AXDX, BESZ_FARPTR);
			break;
        case en_cbool:
        case en_cb:
        case en_cub:
        case en_cw:
        case en_cuw:
        case en_cl:
        case en_cul:
		case en_ci:
		case en_cui:
        case en_cp:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, F_DREG | F_IMMED | F_MEM, natural_size(node));
			break;
        case en_cll:
        case en_cull:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, F_AXDX | F_IMMED | F_MEM, natural_size(node));
			break;
        case en_cfc:
        case en_crc:
        case en_clrc:
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, F_VOL, natural_size(node));
			break;

        case en_napccon:
        case en_nacon:
            node->v.p[0] = node->v.sp;
            ap1 = RealizeDreg(areg);
			ap2 = xalloc(sizeof(AMODE));
			ap2->mode = am_direct;
			ap2->offset = node;
            gen_codes(op_lea, BESZ_DWORD , ap1, ap2);
			ap1->seg = defseg(node);
            if (prm_farkeyword)
			{
				ap1->length = BESZ_FARPTR;
			}
			break;
        case en_nalabcon:
        case en_labcon:
            ap1 = RealizeDreg(areg);
            ap2 = xalloc(sizeof(AMODE));
            ap2->mode = am_direct;
            ap2->offset = node; /* use as constant node */
            gen_codes(op_lea, BESZ_DWORD , ap1, ap2);
            ap1->seg = defseg(node);
            if (prm_farkeyword)
			{
				ap1->length = BESZ_FARPTR;
			}
			break;
        case en_rcon:
        case en_lrcon:
        case en_fcon:
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
        case en_fcomplexcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
            ap1 = xalloc(sizeof(AMODE));
            ap1->mode = am_immed;
            ap1->offset = node;
            ap1->length = natural_size(node);
            make_floatconst(ap1);
			break;
        case en_icon:
        case en_llcon:
        case en_llucon:
        case en_lcon:
        case en_lucon:
        case en_iucon:
        case en_ccon:
        case en_boolcon:
        case en_cucon:
            ap1 = xalloc(sizeof(AMODE));
            ap1->mode = am_immed;
            ap1->offset = node;
            ap1->length = natural_size(node);
			break;
        case en_absacon:
            node->v.i = node->v.sp->value.i;
            ap1 = RealizeDreg(areg);
            ap2 = xalloc(sizeof(AMODE));
            ap2->mode = am_direct;
            ap2->seg = 0;
            ap2->offset = node; /* use as constant node */
            gen_codes(op_lea, BESZ_DWORD , ap1, ap2);
            ap1->seg = ap2->seg;
			break;
        case en_autocon:
        case en_autoreg:
            ap1 = RealizeDreg(areg);
            ap2 = xalloc(sizeof(AMODE));
            ap2->mode = am_indisp;
            ap2->preg = EBP;
            sp = node->v.sp;
            i = sp->value.i;
                //												if (i > 0 && (currentfunc->value.classdata.cppflags & PF_MEMBER) &&
                //															!(currentfunc->value.classdata.cppflags & PF_STATIC))
                //													i += 4;
            ap2->offset = makeintnode(en_icon, i);
            gen_codes(op_lea, BESZ_DWORD , ap1, ap2);
            if (prm_farkeyword)
			{
                ap1->seg = SS;
				ap1->length = BESZ_FARPTR;
			}
			break;
        case en_substack:
			MarkRegs();
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, BESZ_DWORD );
            if (ap1->mode == am_immed && (ap1->offset->v.i < 4096))
            {
                if (ap1->offset->v.i < 0)
                    ap1->offset->v.i -= stackadd;
                else
                    ap1->offset->v.i += stackadd;
                ap1->offset->v.i &= stackmod;
                gen_codes(op_sub, BESZ_DWORD , makedreg(ESP), ap1);
            }
            else
            {
                gen_codes(op_mov, BESZ_DWORD , makedreg(ECX), ap1);
                call_library("___substack");
            }
			ReleaseRegs();
            ap1 = RealizeDreg(areg);
            gen_codes(op_mov, BESZ_DWORD , ap1, makedreg(ESP));
			break;
		case en_loadstack:
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM, BESZ_DWORD );
			gen_codes(op_mov, BESZ_DWORD, makedreg(ESP), ap1);
			break;
		case en_savestack:
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_MEM, BESZ_DWORD );
			gen_codes(op_mov, BESZ_DWORD, ap1, makedreg(ESP));
			break;
        case en_structret:
            ap1 = xalloc(sizeof(AMODE));
            ap1->preg = EBP;
            ap1->mode = am_indisp;
            if ((currentfunc->pascaldefn || currentfunc->isstdcall) &&
                currentfunc->tp->lst.head && currentfunc->tp->lst.head != (SYM*)
                - 1)
                ap1->offset = makeintnode(en_icon, (currentfunc->tp->lst.head
                    ->value.i + ((currentfunc->tp->lst.head->tp->size + 3)
                    &0xFFFFFFFCL)));
            else
                    if (declclass && !(currentfunc->value.classdata.cppflags
                        &PF_STATIC))
                        ap1->offset = makeintnode(en_icon, 12+(currentfunc
                            ->farproc *4));
                    else
                    ap1->offset = makeintnode(en_icon, 8+(currentfunc->farproc
                        *4));
			ap1->length = size;
			break;
        case en_fp_ref:
        case en_bool_ref:
        case en_b_ref:
        case en_w_ref:
        case en_ub_ref:
        case en_uw_ref:
        case en_l_ref:
        case en_ul_ref:
		case en_a_ref:
		case en_ua_ref:
		case en_i_ref:
		case en_ui_ref:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_fimaginaryref:
        case en_rimaginaryref:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_ll_ref:
        case en_ull_ref:
            ap1 = gen_deref(node, areg, sreg);
			break;			
        case en_tempref:
        case en_regref:
            ap1 = xalloc(sizeof(AMODE));
            ap1->tempflag = 0;
            if ((node->v.i &0xff) < 16)
            {
                ap1->mode = am_dreg;
                ap1->preg = node->v.i &0xff;
				ap1->seg = (node->v.i >> 8) & 0xff;
                ap1->tempflag = 1;
            }
            else
            if ((node->v.i &0xff) < 24)
            {
                ap1->mode = am_dreg;
                ap1->preg = (node->v.i &0xff) - 12;
            }
            else if ((node->v.i &0xff) < 32)
            {
                ap1->mode = am_seg;
                ap1->seg = (node->v.i &0xff) - 24+1;
            }
            else if ((node->v.i &0xff) >= 64)
            {
                ap1->mode = am_streg;
                ap1->preg = (node->v.i &0xff) - 64;
            }
            else if ((node->v.i &0xff) >= 56)
            {
                ap1->mode = am_sdreg;
                ap1->preg = (node->v.i &0xff) - 56;
            }
            else if ((node->v.i &0xff) >= 48)
            {
                ap1->mode = am_screg;
                ap1->preg = (node->v.i &0xff) - 48;
            }
            else
            {
                ap1->mode = am_freg;
                ap1->preg = (node->v.i &0xff) - 32;
            }
            ap1->tempflag = 0; /* not a temporary */
            ap1->length = (node->v.i >> 16);
            if (ap1->mode != am_seg)
                ap1->seg = (node->v.i >> 8) &0xff;
			break;
        case en_asuminus:
            ap1 = gen_asunary(node, areg, sreg, novalue, op_neg, op_fchs);
			break;
        case en_ascompl:
            ap1 = gen_asunary(node, areg, sreg, novalue, op_not, op_not);
			break;
        case en_uminus:
            ap1 = gen_unary(node, areg, sreg, op_neg, op_fchs);
			break;
        case en_compl:
            ap1 = gen_unary(node, areg, sreg, op_not, op_not);
			break;
        case en_add:
        case en_addstruc:
		case en_array:
            ap1 = gen_binary(node, areg, sreg, op_add, op_faddp, op_fadd, op_faddp,
                op_fadd);
			break;
        case en_addcast:
            ap1 = gen_expr(node->v.p[0], areg, sreg, FALSE, F_DREG | F_VOL, size);
			MarkRegs();
            gen_codes(op_test, BESZ_DWORD , ap1, ap1);
            lab0 = nextlabel++;
            gen_branch(op_je, make_label(lab0));
			ChooseRegs(&areg, &sreg);
            ap2 = gen_expr(node->v.p[1], areg, sreg, FALSE, F_DREG | F_IMMED | F_MEM, size);
                // will always be constant
            gen_codes(op_add, BESZ_DWORD , ap1, ap2);
            gen_label(lab0);
			ReleaseRegs();
			break;
        case en_sub:
            ap1 = gen_binary(node, areg, sreg, op_sub, op_fsubp, op_fsub, op_fsubrp,
                op_fsubr);
			break;
        case en_and:
            ap1 = gen_xbin(node, areg, sreg, op_and, op_btr);
			break;
        case en_or:
            ap1 = gen_xbin(node, areg, sreg, op_or, op_bts);
			break;
        case en_xor:
            ap1 = gen_xbin(node, areg, sreg, op_xor, op_btc);
			break;
        case en_pmul:
		case en_arrayindex:
            ap1 = gen_pmul(node, areg, sreg);
			break;
        case en_pdiv:
            ap1 = gen_pdiv(node, areg, sreg);
			break;
        case en_mul:
            ap1 = gen_mul(node, areg, sreg, op_imul, op_fmulp, op_fmul, op_fmulp, op_fmul);
			break;
        case en_umul:
            ap1 = gen_mul(node, areg, sreg, op_mul, op_fmulp, op_fmul, op_fmulp, op_fmul);
			break;
        case en_div:
            ap1 = gen_modiv(node, areg, sreg, op_idiv, op_fdivp, op_fdiv, op_fdivrp,
                op_fdivr, 0);
			break;
        case en_udiv:
            ap1 = gen_modiv(node, areg, sreg, op_div, op_fdivp, op_fdiv, op_fdivrp,
                op_fdivr, 0);
			break;
        case en_mod:
            ap1 = gen_modiv(node, areg, sreg, op_idiv, op_fdivp, op_fdiv, op_fdivrp,
                op_fdivr, 1);
			break;
        case en_umod:
            ap1 = gen_modiv(node, areg, sreg, op_div, op_fdivp, op_fdiv, op_fdivrp,
                op_fdivr, 1);
			break;
        case en_alsh:
            ap1 = gen_shift(node, areg, sreg, op_sal, FALSE);
			break;
        case en_arsh:
            ap1 = gen_shift(node, areg, sreg, op_sar, FALSE);
			break;
        case en_arshd:
            ap1 = gen_shift(node, areg, sreg, op_sar, TRUE);
			break;
        case en_lsh:
            ap1 = gen_shift(node, areg, sreg, op_shl, FALSE);
			break;
        case en_rsh:
            ap1 = gen_shift(node, areg, sreg, op_shr, FALSE);
			break;
        case en_asadd:
            ap1 = gen_asadd(node, areg, sreg, novalue, op_add, op_faddp, op_fadd, op_faddp,
                op_fadd);
			break;
        case en_assub:
            ap1 = gen_asadd(node, areg, sreg, novalue, op_sub, op_fsubp, op_fsub,
                op_fsubrp, op_fsubr);
			break;
        case en_asand:
            ap1 = gen_aslogic(node, areg, sreg, novalue, op_and, op_btr);
			break;
        case en_asor:
            ap1 = gen_aslogic(node, areg, sreg, novalue, op_or, op_bts);
			break;
        case en_asxor:
            ap1 = gen_aslogic(node, areg, sreg, novalue, op_xor, op_btc);
			break;
        case en_aslsh:
            ap1 = gen_asshift(node, areg, sreg, novalue, op_shl, FALSE);
			break;
        case en_asrsh:
            ap1 = gen_asshift(node, areg, sreg, novalue, op_shr, FALSE);
			break;
        case en_asalsh:
            ap1 = gen_asshift(node, areg, sreg, novalue, op_sal, FALSE);
			break;
        case en_asarsh:
            ap1 = gen_asshift(node, areg, sreg, novalue, op_sar, FALSE);
			break;
        case en_asarshd:
            ap1 = gen_asshift(node, areg, sreg, novalue, op_sar, TRUE);
			break;
        case en_asmul:
            ap1 = gen_asmul(node, areg, sreg, novalue, op_imul, op_fmulp, op_fmul,
                op_fmulp, op_fmul);
			break;
        case en_asumul:
            ap1 = gen_asmul(node, areg, sreg, novalue, op_mul, op_fmulp, op_fmul, op_fmulp,
                op_fmul);
			break;
        case en_asdiv:
            ap1 = gen_asmodiv(node, areg, sreg, novalue, op_idiv, op_fdivp, op_fdiv,
                op_fdivrp, op_fdivr, FALSE);
			break;
        case en_asudiv:
            ap1 = gen_asmodiv(node, areg, sreg, novalue, op_div, op_fdivp, op_fdiv,
                op_fdivrp, op_fdivr, FALSE);
			break;
        case en_asmod:
            ap1 = gen_asmodiv(node, areg, sreg, novalue, op_idiv, op_fdivp, op_fdiv,
                op_fdivrp, op_fdivr, TRUE);
			break;
        case en_asumod:
            ap1 = gen_asmodiv(node, areg, sreg, novalue, op_div, op_fdivp, op_fdiv,
                op_fdivrp, op_fdivr, TRUE);
			break;
        case en_assign:
            ap1 = gen_assign(node, areg, sreg, novalue, TRUE);
			break;
        case en_lassign:
            ap1 = gen_assign(node, areg, sreg, novalue, FALSE);
			break;
        case en_refassign:
            ap1 = gen_refassign(node, areg, sreg, novalue);
			break;
        case en_moveblock:
            ap1 = gen_moveblock(node, areg, sreg);
			break;
        case en_clearblock:
            ap1 = gen_clearblock(node, areg, sreg);
			break;
        case en_ainc:
            ap1 = gen_aincdec(node, areg, sreg, novalue, op_add);
			break;
        case en_adec:
            ap1 = gen_aincdec(node, areg, sreg, novalue, op_sub);
			break;
        case en_land:
        case en_lor:
        case en_eq:
        case en_ne:
        case en_lt:
        case en_le:
        case en_gt:
        case en_ge:
        case en_ult:
        case en_ule:
        case en_ugt:
        case en_uge:
        case en_not:
            ap1 = gen_relat(node, areg, sreg);
			break;
        case en_cond:
            ap1 = gen_hook(node, areg, sreg);
			break;
        case en_voidnz:
            lab0 = nextlabel++;
			MarkRegs();
            falsejp(node->v.p[0]->v.p[0], areg, sreg, lab0);
			ReleaseRegs();
            gen_void(node->v.p[1], areg, sreg);
            gen_label(lab0);
            ap1 = gen_expr(node->v.p[0]->v.p[1], areg, sreg, FALSE, flags, BESZ_DWORD ); /* will
                typically be part of a void tree, or top of tree */
			break;
        case en_void:
			MarkRegs();
            gen_void(node->v.p[0],areg, sreg);
			ReleaseRegs();
            ap1 = gen_expr(node->v.p[1], areg, sreg, TRUE, flags, natural_size(node));
			break;
        case en_dvoid:
            ap1 = gen_expr(node->v.p[0], areg, sreg, TRUE, flags, natural_size(node));
            gen_void(node->v.p[1], areg, sreg);
			break;
        case en_pfcall:
        case en_pfcallb:
        case en_pcallblock:
            ap1 = gen_pfcall(node, areg, sreg, novalue);
			break;
        case en_sfcall:
        case en_sfcallb:
        case en_scallblock:
        case en_fcall:
        case en_callblock:
        case en_fcallb:
        case en_intcall:
        case en_thiscall:
            ap1 = gen_fcall(node, areg, sreg, novalue);
			break;
        case en_trapcall:
            ap1 = gen_tcall(node, areg, sreg, novalue);
			break;
        case en_repcons:
            InitRegs();
            ap1 = gen_repcons(node, areg, sreg);
			break;
        case en_conslabel:
            if (node->v.p[1]->v.sp->nullsym)
                return 0;
            node->v.p[1]->v.sp->value.classdata.conslabel = lab0 = nextlabel++;
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags, size);
            gen_label(lab0);
			break;
        case en_destlabel:
            if (node->v.p[1]->v.sp->nullsym)
                return 0;
            node->v.p[1]->v.sp->value.classdata.destlabel = nextlabel;
            gen_label(nextlabel++);
            ap1 = gen_expr(node->v.p[0], areg, sreg, novalue, flags, size);
			break;
        case en_movebyref:
             /* explicitly uncoded */
        default:
            DIAG("uncoded node in gen_expr.");
            ap1 = makedreg(0);
			break;
	}

    if (ap1 && !(flags & F_STOREADR) && !(flags & F_NOVALUE) && !novalue)
	{
        ap1 = do_extend(ap1, areg, sreg, size, flags);
	}
	return ap1;
}
AMODE *gen_expr_external(ENODE *node, int novalue, int flags, int sz)
{
	AMODE *ap;
	int areg, sreg;
	InitRegs();
	ChooseRegs(&areg, &sreg);
	ap = gen_expr(node, areg, sreg, novalue, flags, sz);
	return ap;
}
void gen_void_external(ENODE *node)
{
	int areg, sreg;
	InitRegs();
	ChooseRegs(&areg, &sreg);
	gen_void(node, areg, sreg);
}
