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
 * inline assembler (386)
 */
#include <stdio.h> 
#include <string.h>
#include "lists.h"
#include "expr.h" 
#include "c.h" 
#include "ccerr.h"
#include "gen386.h"

extern long lc_maxauto;

extern TABLE labelsyms;
extern int asmline;
extern int lastch;
extern enum e_sym lastst;
extern LLONG_TYPE ival;
extern short *lptr;
extern TABLE lsyms;
extern long nextlabel;
extern char lastid[];
extern int prm_asmopt;
extern int prm_nasm, prm_asmfile;

ASMNAME *keyimage;
ASMREG *regimage;
int asmMask, asmRMask;
static SYM *lastsym;
static enum e_op op;

ASMREG reglst[] = 
{
    {
        "cs", am_seg, 1, 2
    } , 
    {
        "ds", am_seg, 2, 2
    }
    , 
    {
        "es", am_seg, 3, 2
    }
    , 
    {
        "fs", am_seg, 4, 2
    }
    , 
    {
        "gs", am_seg, 5, 2
    }
    , 
    {
        "ss", am_seg, 6, 2
    }
    , 
    {
        "al", am_dreg, 0, 1
    }
    , 
    {
        "cl", am_dreg, 1, 1
    }
    , 
    {
        "dl", am_dreg, 2, 1
    }
    , 
    {
        "bl", am_dreg, 3, 1
    }
    , 
    {
        "ah", am_dreg, 4, 1
    }
    , 
    {
        "ch", am_dreg, 5, 1
    }
    , 
    {
        "dh", am_dreg, 6, 1
    }
    , 
    {
        "bh", am_dreg, 7, 1
    }
    , 
    {
        "ax", am_dreg, 0, 2
    }
    , 
    {
        "cx", am_dreg, 1, 2
    }
    , 
    {
        "dx", am_dreg, 2, 2
    }
    , 
    {
        "bx", am_dreg, 3, 2
    }
    , 
    {
        "sp", am_dreg, 4, 2
    }
    , 
    {
        "bp", am_dreg, 5, 2
    }
    , 
    {
        "si", am_dreg, 6, 2
    }
    , 
    {
        "di", am_dreg, 7, 2
    }
    , 
    {
        "eax", am_dreg, 0, 4
    }
    , 
    {
        "ecx", am_dreg, 1, 4
    }
    , 
    {
        "edx", am_dreg, 2, 4
    }
    , 
    {
        "ebx", am_dreg, 3, 4
    }
    , 
    {
        "esp", am_dreg, 4, 4
    }
    , 
    {
        "ebp", am_dreg, 5, 4
    }
    , 
    {
        "esi", am_dreg, 6, 4
    }
    , 
    {
        "edi", am_dreg, 7, 4
    }
    , 
    {
        "st", am_freg, 0, 10
    }
    , 
    {
        "cr0", am_screg, 0, 4
    }
    , 
    {
        "cr1", am_screg, 1, 4
    }
    , 
    {
        "cr2", am_screg, 2, 4
    }
    , 
    {
        "cr3", am_screg, 3, 4
    }
    , 
    {
        "cr4", am_screg, 4, 4
    }
    , 
    {
        "cr5", am_screg, 5, 4
    }
    , 
    {
        "cr6", am_screg, 6, 4
    }
    , 
    {
        "cr7", am_screg, 7, 4
    }
    , 
    {
        "dr0", am_sdreg, 0, 4
    }
    , 
    {
        "dr1", am_sdreg, 1, 4
    }
    , 
    {
        "dr2", am_sdreg, 2, 4
    }
    , 
    {
        "dr3", am_sdreg, 3, 4
    }
    , 
    {
        "dr4", am_sdreg, 4, 4
    }
    , 
    {
        "dr5", am_sdreg, 5, 4
    }
    , 
    {
        "dr6", am_sdreg, 6, 4
    }
    , 
    {
        "dr7", am_sdreg, 7, 4
    }
    , 
    {
        "tr0", am_streg, 0, 4
    }
    , 
    {
        "tr1", am_streg, 1, 4
    }
    , 
    {
        "tr2", am_streg, 2, 4
    }
    , 
    {
        "tr3", am_streg, 3, 4
    }
    , 
    {
        "tr4", am_streg, 4, 4
    }
    , 
    {
        "tr5", am_streg, 5, 4
    }
    , 
    {
        "tr6", am_streg, 6, 4
    }
    , 
    {
        "tr7", am_streg, 7, 4
    }
    , 
    {
        "st", am_freg, 0, 4
    }
    , 
    {
        "byte", am_ext, akw_byte, 0
    }
    , 
    {
        "word", am_ext, akw_word, 0
    }
    , 
    {
        "dword", am_ext, akw_dword, 0
    }
    , 
    {
        "fword", am_ext, akw_fword, 0
    }
    , 
    {
        "qword", am_ext, akw_qword, 0
    }
    , 
    {
        "tbyte", am_ext, akw_tbyte, 0
    }
    , 
    {
        "ptr", am_ext, akw_ptr, 0
    }
    , 
    {
        "offset", am_ext, akw_offset, 0
    }
    , 
    {
        0, 0, 0
    }
    , 
};

static int floating;

void inasmini(void){}
static void asm_err(int errnum)
{
    *lptr = 0;
    lastch = ' ';
    generror(errnum, 0, 0);
    getsym();
}

static void allocReg(ASMREG *regimage)
{
	if (regimage->regtype == am_dreg)
	{
		if (regimage->size >= 2)
		{
			if (regimage->regnum == EBX)
			{
                asmRMask = asmRMask | (1 << (15-EBX));
                asmMask = asmMask | (1 << EBX);
            }
			else if (regimage->regnum == ESI || regimage->regnum == EDI)
			{
                asmRMask = asmRMask | (1 << (7-(regimage->regnum-4)));
                asmMask = asmMask | (1 << (8 + regimage->regnum - 4));
			}
		}
	}
}
//-------------------------------------------------------------------------

static ENODE *asm_ident(void)
{
    ENODE *node = 0;
    char *nm;
    int fn = FALSE;
    if (lastst != id)
        asm_err(ERR_IDEXPECT);
    else
    {
        SYM *sp;
        ENODE *qnode = 0;
        nm = litlate(lastid);
        getsym();
        /* No such identifier */
        /* label, put it in the symbol table */
        if ((sp = search(nm, &labelsyms)) == 0 && (sp = gsearch(nm)) == 0)
        {
            sp = makesym(sc_ulabel);
            sp->name = nm;
            sp->tp = xalloc(sizeof(TYP));
            sp->tp->type = bt_unsigned;
            sp->tp->uflags = UF_USED;
            sp->tp->bits = sp->tp->startbit =  - 1;
            sp->value.i = nextlabel++;
            insert(sp, &labelsyms);
            node = xalloc(sizeof(ENODE));
            node->nodetype = en_labcon;
            node->v.i = sp->value.i;
        }
        else
        {
            /* If we get here the symbol was already in the table
             */
            foundsp: sp->tp->uflags |= UF_USED;
            switch (sp->storage_class)
            {
                case sc_static:
                case sc_global:
                case sc_external:
                case sc_externalfunc:
                case sc_abs:
                    sp->mainsym->extflag = TRUE;
                        if (sp->value.classdata.gdeclare)
                            sp->value.classdata.gdeclare->mainsym->extflag =
                                TRUE;
                    if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
                    {
                        /* make a function node */
                        node = makenode(en_napccon, sp, 0);
                        isfunc: if (sp->tp->type != bt_func && sp->tp->type !=
                            bt_ifunc)
                            generror(ERR_MISMATCH, 0, 0);
                    }
                    else
                    /* otherwise make a node for a regular variable */
                        if (sp->absflag)
                            node = makenode(en_absacon, sp, 0);
                        else
                    if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
                    {
                        fn = TRUE;
                        node = makenode(en_napccon, sp, 0);
                    }
                        else
                            if (sp->staticlabel)
                                node = makenode(en_nalabcon, sp, 0);
                            else
                                node = makenode(en_nacon, sp, 0);
                    break;
                case sc_const:
                    /* constants and enums */
                    node = makeintnode(en_icon, sp->value.i);
                    break;
                case sc_label:
                case sc_ulabel:
                    node = xalloc(sizeof(ENODE));
                    node->nodetype = en_labcon;
                    node->v.i = sp->value.i;
                    break;
                default:
                     /* auto and any errors */
                    if (sp->storage_class != sc_auto && sp->storage_class !=
                        sc_autoreg)
                    {
                        gensymerror(ERR_ILLCLASS2, sp->name);
                    }
                    else
                    {
                        /* auto variables */
                        if (sp->storage_class == sc_auto)
                            node = makenode(en_autocon, sp, 0);
                        else if (sp->storage_class == sc_autoreg)
                            node = makenode(en_autoreg, sp, 0);
                        if (fn)
                            goto isfunc;
                    }
                    break;
            }

            (node)->cflags = 0;
        }
        lastsym = sp;
    }
    return node;
}

//-------------------------------------------------------------------------

static ENODE *asm_label(void)
{
    char *nm = litlate(lastid);
    ENODE *node;
    SYM *sp;
    getsym();
    /* No such identifier */
    /* label, put it in the symbol table */
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
        if (sp->storage_class == sc_label)
        {
            *lptr = 0;
            lastch = ' ';
            gensymerror(ERR_DUPLABEL, sp->name);
            getsym();
            return 0;
        }
        if (sp->storage_class != sc_ulabel)
        {
            asm_err(ERR_ALABEXPECT);
            return 0;
        }
        sp->storage_class = sc_label;
    }
    if (lastst != colon)
    {
        asm_err(ERR_ALABEXPECT);
        return 0;
    }
    getsym();
    node = xalloc(sizeof(ENODE));
    node->nodetype = en_labcon;
    node->v.i = sp->value.i;
    return node;
}

//-------------------------------------------------------------------------

static int asm_getsize(void)
{
    int sz = 0;
    switch (regimage->regnum)
    {
        case akw_byte:
            sz = 1;
            break;
        case akw_word:
            sz = 2;
            break;
        case akw_dword:
            sz = floating ? 7 : 4;
            break;
        case akw_fword:
            sz = 7;
            break;
        case akw_qword:
            sz = 8;
            break;
        case akw_tbyte:
            sz = 10;
            break;
        case akw_offset:
            sz = 4;
            break;
    };
    getsym();
    if (lastst == kw_asmreg)
    {
		allocReg(keyimage);
        regimage = keyimage;
        if (regimage->regtype == am_ext)
        {
            if (regimage->regnum != akw_ptr)
            {
                asm_err(ERR_AILLADDRESS);
                return 0;
            }
            getsym();
        }
    }
    if (lastst != kw_asmreg && lastst != openbr && lastst != id)
    {
        asm_err(ERR_AMODEEXPECT);
        return 0;
    }
    regimage = keyimage;
    return sz;
}

//-------------------------------------------------------------------------

static int getscale(int *scale)
{
    if (lastst == star)
    {
        getsym();
        if (lastst == iconst && ! *scale)
        if (ival == 1 || ival == 2 || ival == 4 || ival == 8)
        {
            if (ival < 3)
                ival--;
            else
                ival = ival / 4+1;
            *scale = ival;
            getsym();
            return 1;
        }
        asm_err(ERR_ASCALE);
        *scale =  - 1;
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------

int asm_enterauto(ENODE *node, int *reg1, int *reg2)
{
    if (node && (node->nodetype == en_autocon || node->nodetype == en_autoreg))
    {
        int *vreg;
        if (*reg1 >= 0 &&  *reg2 >= 0)
        {
            asm_err(ERR_AINVINDXMODE);
            return 0;
        }
        if (*reg1 < 0)
            vreg = reg1;
        else
            vreg = reg2;
        *vreg = EBP;
        return 1;
    }
    return 2;
}

//-------------------------------------------------------------------------

static int asm_structsize(void)
{
    if (lastsym->tp->type == bt_struct)
    {
        if (lastsym->tp->size == 6)
            return 7;
        if (lastsym->tp->size == 4)
            return 4;
        if (lastsym->tp->size == 1)
            return 1;
        if (lastsym->tp->size == 2)
            return 2;
        return 1000;
    }
    else
        return lastsym->tp->size;
}

//-------------------------------------------------------------------------

static AMODE *asm_mem(void)
{
    int reg1 =  - 1, reg2 =  - 1, scale = 0, temp, sz = 0, seg = 0;
    ENODE *node = 0;
    AMODE *rv;
    int first = TRUE;
    int gotident = FALSE, autonode = FALSE;
    while (TRUE)
    {
        int rg;
        cont: getsym();
        regimage = keyimage;
        if (regimage)
            rg = regimage->regnum;
        switch (lastst)
        {
            case kw_asmreg:
				allocReg(keyimage);
                regimage = keyimage;
                if (regimage->regtype == am_seg)
                {
                    if (seg)
                    {
                        asm_err(ERR_AINVINDXMODE);
                        return 0;
                    }
                    seg = rg;
                    getsym();
                    if (lastst != colon)
                    {
                        asm_err(ERR_AINVINDXMODE);
                        return 0;
                    }
                    goto cont;
                }
                if (regimage->regtype != am_dreg || regimage->size != 4)
                {
                    asm_err(ERR_AINVINDXMODE);
                    return 0;
                }
                if (reg1 >= 0)
                {
                    if (reg2 >= 0)
                    {
                        asm_err(ERR_AINVINDXMODE);
                        return 0;
                    }
                    reg2 = rg;
                    getsym();
                    getscale(&scale);
                    if (scale ==  - 1)
                        return 0;
                }
                else
                {
                    getsym();
                    if (getscale(&scale))
                    {
                        if (scale ==  - 1)
                            return 0;
                        if (reg2 >= 0)
                        {
                            reg1 = reg2;
                        }
                        reg2 = rg;
                    }
                    else
                    {
                        reg1 = rg;
                    }
                }
                break;
            case minus:
            case plus:
            case iuconst:
            case lconst:
            case luconst:
            case cconst:
            case iconst:
                if (node)
                    node = makenode(en_add, node, makeintnode(en_icon, intexpr
                        (0)));
                else
                    node = makeintnode(en_icon, intexpr(0));
                break;
            case id:
                if (gotident)
                {
                    asm_err(ERR_AINVINDXMODE);
                    return 0;
                }
                node = asm_ident();
                gotident = TRUE;
                sz = asm_structsize();
                switch (asm_enterauto(node, &reg1, &reg2))
                {
                case 0:
                    return 0;
                case 1:
                    autonode = TRUE;
                    break;
                case 2:
                    autonode = FALSE;
                    break;
                }
                break;
            default:
                asm_err(ERR_AILLADDRESS);
                return 0;
        }
        if (lastst == closebr)
        {
            getsym();
            break;
        }
        first = FALSE;
        if (lastst != plus && lastst != minus)
        {
            asm_err(ERR_AINVINDXMODE);
            return 0;
        }
        backup(lastst);
    }
    if ((reg2 == 4 || reg2 == 5) && scale > 1)
    {
        asm_err(ERR_AINVINDXMODE);
        return 0;
    }
    rv = xalloc(sizeof(AMODE));
    if (node)
    {
        rv->offset = node;
    }
    if (reg1 >= 0)
    {
        rv->preg = reg1;
        if (reg2 >= 0)
        {
            rv->sreg = reg2;
            rv->scale = scale;
            rv->mode = am_indispscale;
        }
        else
        {
            rv->mode = am_indisp;
        }
    }
    else
    if (reg2 >= 0)
    {
        rv->preg =  - 1;
        rv->sreg = reg2;
        rv->scale = scale;
        rv->mode = am_indispscale;
    }
    else
        rv->mode = am_direct;
    rv->seg = seg;
    return rv;
}

//-------------------------------------------------------------------------

static AMODE *asm_amode(int nosegreg)
{
    AMODE *rv = xalloc(sizeof(AMODE));
    int sz = 0, seg = 0;
    lastsym = 0;
    switch (lastst)
    {
        case iconst:
        case iuconst:
        case lconst:
        case luconst:
        case cconst:
        case minus:
        case plus:
        case kw_asmreg:
        case openbr:
        case id:
            break;
        default:
            asm_err(ERR_AMODEEXPECT);
            return 0;
    }
    if (lastst == kw_asmreg)
    {
		allocReg(keyimage);
        regimage = keyimage;
        if (regimage->regtype == am_ext)
        {
            sz = asm_getsize();
            regimage = keyimage;
        }
    }
    loop: switch (lastst)
    {
        case kw_asmreg:
			allocReg(keyimage);
            if (regimage->regtype == am_ext)
            {
                asm_err(ERR_ATOOMANYSPECS);
                return 0;
            }
            if (regimage->regtype == am_freg)
            {
                getsym();
                if (lastst == openpa)
                {
                    getsym();
                    if (lastst != iconst || ival < 0 || ival > 7)
                    {
                        asm_err(ERR_ANEEDFP);
                        return 0;
                    }
                    getsym();
                    needpunc(closepa, 0);
                }
                else
                    ival = 0;
                rv->preg = ival;
                rv->mode = am_freg;
                sz = 10;
            }
            else if (regimage->regtype == am_seg)
            {
                if (rv->seg)
                {
                    asm_err(ERR_ATOOMANYSEGS);
                    return 0;
                }
                rv->seg = seg = regimage->regnum;
                getsym();
                if (lastst == colon)
                {
                    getsym();
                    goto loop;
                }
                rv->mode = am_seg;
                sz = regimage->size;
            }
            else
            {
                rv->preg = regimage->regnum;
                rv->mode = regimage->regtype;
                sz = rv->length = regimage->size;
                getsym();
            }
            break;
        case openbr:
            rv = asm_mem();
            if (rv && rv->seg)
                seg = rv->seg;
            break;
        case id:
            rv->mode = am_immed;
            rv->offset = asm_ident();
            rv->length = 4;
            if (rv->offset->nodetype == en_autocon || rv->offset->nodetype ==
                en_autoreg)
            {
                asm_err(ERR_AUSELEA);
                return 0;
            }
            break;
        case minus:
        case plus:
        case iconst:
        case iuconst:
        case lconst:
        case luconst:
        case cconst:
            rv = make_immed(intexpr(0));
            break;
        default:
            asm_err(ERR_AILLADDRESS);
            return 0;
    }
    if (rv)
    {
        if (rv->seg)
            if (nosegreg || rv->mode != am_dreg)
        if (rv->mode != am_direct && rv->mode != am_indisp && rv->mode !=
            am_indispscale && rv->mode != am_seg)
        {
            asm_err(ERR_ATOOMANYSEGS);
            return 0;
        }
        if (!rv->length)
            if (sz)
                rv->length = sz;
            else if (lastsym)
                rv->length = asm_structsize();
        if (rv->length < 0)
            rv->length =  - rv->length;
        rv->seg = seg;
    }
    return rv;
}

//-------------------------------------------------------------------------

static AMODE *asm_immed(void)
{
    AMODE *rv;
    switch (lastst)
    {
        case iconst:
        case iuconst:
        case lconst:
        case luconst:
        case cconst:
            rv = make_immed(ival);
            rv->length = 4;
            getsym();
            return rv;
    }
    return 0;
}

//-------------------------------------------------------------------------

int isrm(AMODE *ap, int dreg_allowed)
{
    switch (ap->mode)
    {
        case am_dreg:
            return dreg_allowed;
        case am_indisp:
        case am_direct:
        case am_indispscale:
            return 1;
        default:
            return 0;
    }
}

//-------------------------------------------------------------------------

AMODE *getimmed(void)
{
    AMODE *rv;
    switch (lastst)
    {
        case iconst:
        case iuconst:
        case lconst:
        case luconst:
        case cconst:
            rv = make_immed(ival);
            getsym();
            return rv;
        default:
            return 0;
    }
}

//-------------------------------------------------------------------------

enum e_op asm_op(void)
{
    int op;
    if (lastst != kw_asminst)
    {
        asm_err(ERR_AINVOP);
        return  - 1;
    } op = keyimage->atype;
    getsym();
    floating = op >= op_f2xm1;
    return op;
}

//-------------------------------------------------------------------------

static OCODE *make_ocode(AMODE *ap1, AMODE *ap2, AMODE *ap3)
{
    OCODE *o = xalloc(sizeof(OCODE));
    o->oper1 = ap1;
    o->oper2 = ap2;
    o->oper3 = ap3;
    return o;
}

//-------------------------------------------------------------------------

static OCODE *ope_math(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_dreg)
    {
        if (ap2->mode != am_immed && ap2->mode != am_dreg)
            return (OCODE*) - 1;
    }
    else
        if (!isrm(ap2, TRUE) && ap2->mode != am_immed)
            return (OCODE*) - 1;
    if (ap2->mode != am_immed)
        if (ap1->length && ap2->length && ap1->length != ap2->length)
            return (OCODE*) - 2;

    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_arpl(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_dreg)
        return (OCODE*) - 1;
    if (!ap1->length || !ap2->length || ap1->length != ap2->length || ap1
        ->length != 2)
        return (OCODE*) - 2;

    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_bound(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, FALSE))
        return (OCODE*) - 1;
    if (ap2->length)
        return (OCODE*) - 2;
    #ifdef XXXXX
        switch (ap1->length)
        {
            case 1:
                return (OCODE*) - 1;
            case 2:
                if (ap2->length != 4)
                {
                    return (OCODE*) - 2;
                }
                break;
            case 4:
                if (ap2->length != 8)
                {
                    return (OCODE*) - 2;
                }
                break;
        }
    #endif 
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_bitscan(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*) - 1;
    if (ap1->length == 1 || ap2->length != ap1->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);

}

//-------------------------------------------------------------------------

static OCODE *ope_bit(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_immed && ap2->mode != am_dreg)
        return (OCODE*) - 1;
    if (ap1->length == 1 || ap2->mode == am_dreg && ap2->length == 1)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_call(void)
{
    AMODE *ap1 = asm_amode(TRUE),  *ap2;
    if (!ap1)
        return 0;
    if (ap1->mode == am_immed)
    {
        if (ap1->mode == am_immed && lastst == colon)
        {
            getsym();
            if (prm_asmfile && !prm_nasm)
                return (OCODE*) - 1;
            ap2 = asm_amode(TRUE);
            if (!ap2)
                return (OCODE*) - 1;
            if (ap2->mode != am_immed)
                return (OCODE*) - 1;
            ap1->length = ap2->length = 4;
            return make_ocode(ap1, ap2, 0);
        }
        else if (ap1->offset->nodetype != en_nalabcon && ap1->offset->nodetype
            != en_labcon && ap1->offset->nodetype != en_napccon || ap1->seg)
            return (OCODE*) - 1;
    }
    else
    {
        if (!isrm(ap1, TRUE))
            return (OCODE*) - 1;
        if (ap1->length && (ap1->length != 4) && (ap1->length != 7))
            return (OCODE*) - 2;
    }
    if ((ap1->mode == am_direct || ap1->mode == am_immed) && ap1->offset
        ->nodetype == en_labcon)
        ap1->length = 0;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_incdec(void)
{
    AMODE *ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    if (ap1->length > 4)
    {
        return (OCODE*) - 2;
    }
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_rm(void)
{
    return (ope_incdec());
}

//-------------------------------------------------------------------------

static OCODE *ope_enter(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_immed();
    if (!ap1)
        return 0;
    needpunc(comma, 0);
    ap2 = asm_immed();
    if (!ap2)
        return 0;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_imul(void)
{
    AMODE *ap1 = asm_amode(TRUE),  *ap2 = 0,  *ap3 = 0;
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    if (lastst == comma)
    {
        getsym();
        ap2 = asm_amode(TRUE);
        if (lastst == comma)
        {
            getsym();
            ap3 = asm_amode(TRUE);
        }
    }
    if (ap2)
    {
        if (ap1->mode != am_dreg || ap1->length == 1)
            return (OCODE*) - 1;
        if (!isrm(ap2, TRUE) && ap2->mode != am_immed)
            return (OCODE*) - 1;
        if (ap3)
            if (ap2->mode == am_immed || ap3->mode != am_immed)
                return (OCODE*) - 1;
    }
    return make_ocode(ap1, ap2, ap3);
}

//-------------------------------------------------------------------------

static OCODE *ope_in(void)
{
    AMODE *ap1 = asm_amode(TRUE),  *ap2;
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg || ap1->preg != 0)
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_immed && (ap2->mode != am_dreg || ap2->preg != 2 || ap2
        ->length != 2))
        return (OCODE*) - 1;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_imm8(void)
{
    AMODE *ap1 = asm_immed();
    if (!ap1)
        return 0;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_relbra(void)
{
    AMODE *ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    ap1->length = 0;
    if (ap1->mode != am_immed)
        return (OCODE*) - 1;
    if (ap1->offset->nodetype != en_nalabcon && ap1->offset->nodetype !=
        en_labcon)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_relbr8(void)
{
    return ope_relbra();
}

//-------------------------------------------------------------------------

static OCODE *ope_jmp(void)
{
    return ope_call();
}

//-------------------------------------------------------------------------

static OCODE *ope_regrm(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*) - 1;
    if (op == op_lea && ap2->mode == am_dreg)
        return (OCODE*) - 1;
    if (ap2->length && ap1->length != ap2->length || ap1->length == 1)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_loadseg(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*) - 1;
    if (ap1->length != 4 || ap2->length != 7)
        return (OCODE*) - 1;
    ap2->length = 0;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_lgdt(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE) || ap1->length != 7)
        return (OCODE*) - 1;
    ap1->length = 0;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_lidt(void)
{
    return ope_lgdt();
}

//-------------------------------------------------------------------------

static OCODE *ope_rm16(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) || ap1->length != 2)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_mov(void)
{
    AMODE *ap1 = asm_amode(TRUE),  *ap2;
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) && ap1->mode != am_seg && ap1->mode != am_screg && ap1
        ->mode != am_sdreg && ap1->mode != am_streg)
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode == am_dreg)
    {
        if (!isrm(ap2, TRUE) && ap2->mode != am_immed && ap2->mode != am_seg &&
            (ap2->length != 4 || (ap2->mode != am_screg && ap2->mode !=
            am_sdreg && ap2->mode != am_streg)))
            return (OCODE*) - 1;
    }
    else if (isrm(ap1, TRUE))
    {
        if (ap2->mode != am_dreg && ap2->mode != am_immed && ap2->mode !=
            am_seg)
            return (OCODE*) - 1;
    }
    else if (ap1->mode == am_seg)
    {
        if (!isrm(ap2, TRUE))
            return (OCODE*) - 1;
    }
    else if (ap2->length != 4 || ap2->mode != am_dreg)
        return (OCODE*) - 1;
    if (ap1->length && ap2->length && ap1->length != ap2->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_movsx(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*) - 1;
    if (!ap2->length || ap1->length <= ap2->length)
    {
        asm_err(ERR_AINVSIZE);
    }
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_out(void)
{
    AMODE *ap1 = asm_amode(TRUE),  *ap2;
    if (!ap1)
        return 0;
    if (ap1->mode != am_immed && (ap1->mode != am_dreg || ap1->preg != 2 || ap1
        ->length != 2))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_dreg || ap2->preg != 0)
        return (OCODE*) - 1;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_pushpop(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) && ap1->mode != am_seg && (ap1->mode != am_immed || 
        (ap1->mode == am_immed && op == op_pop)))
        return (OCODE*) - 1;
    if (ap1->mode != am_immed && ap1->length != 2 && ap1->length != 4)
    {
        return (OCODE*) - 2;
    }
    if (op == op_pop && ap1->mode == am_seg && ap1->seg == 1)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_shift(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(2);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_immed && ap2->mode != am_dreg)
        return (OCODE*) - 1;
    if (ap2->mode == am_dreg)
        if (ap2->preg != 1 || ap2->length != 1)
            return (OCODE*) - 1;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_ret(void)
{
    AMODE *ap1;
    if (lastst != iconst)
        return make_ocode(0, 0, 0);
    ap1 = asm_amode(TRUE);
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_set(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) || ap1->length != 1)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_shld(void)
{
    AMODE *ap1,  *ap2,  *ap3;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_dreg || ap2->length == 1)
        return (OCODE*) - 1;
    if (ap1->length && ap1->length != ap2->length)
    {
        asm_err(ERR_AINVSIZE);
    }
    needpunc(comma, 0);
    ap3 = asm_amode(TRUE);
    if (!ap3)
        return 0;
    if (ap3->mode != am_immed && ap3->mode != am_dreg)
        return (OCODE*) - 1;
    if (ap3->mode == am_dreg)
        if (ap3->preg != 1 || ap3->length != 1)
            return (OCODE*) - 1;
    return make_ocode(ap1, ap2, ap3);
}

//-------------------------------------------------------------------------

static OCODE *ope_test(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_dreg)
        if (ap2->mode != am_dreg && ap2->mode != am_immed)
            return (OCODE*) - 1;
    if (ap2->mode == am_dreg && ap1->length && ap1->length != ap2->length)
    {
        return (OCODE*) - 2;
    }
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_xchg(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode == am_dreg)
    {
        if (!isrm(ap2, TRUE))
            return (OCODE*) - 1;
    }
    else
        if (ap2->mode != am_dreg)
            return (OCODE*) - 1;
    if (ap1->length && ap2->length && ap1->length != ap2->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fmath(void)
{
    AMODE *ap1,  *ap2 = 0;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != 7 && ap1->length != 8)
            return (OCODE*) - 2;
    }
    else
    {
        if (ap1->mode != am_freg)
            return (OCODE*) - 1;
        if (lastst == comma)
        {
            getsym();
            ap2 = asm_amode(TRUE);
            if (ap2->mode != am_freg)
                return (OCODE*) - 1;
            if (ap1->preg && ap2->preg)
                return (OCODE*) - 1;
        }
    }
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fmathp(void)
{
    AMODE *ap1,  *ap2 = 0;
    if (lastst != kw_asmreg)
        return make_ocode(0, 0, 0);
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_freg)
        return (OCODE*) - 1;
    if (lastst == comma)
    {
        getsym();
        ap2 = asm_amode(TRUE);
        if (!ap2)
            return 0;
        if (ap2->mode != am_freg)
            return (OCODE*) - 1;
        if (ap1->preg && ap2->preg)
            return (OCODE*) - 1;
    }
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fmathi(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != 7 && ap1->length != 2 && ap1->length != 8)
            return (OCODE*) - 2;
    }
    else
    {
        return (OCODE*) - 1;
    }
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fcom(void)
{
    AMODE *ap1;
    if (lastst != kw_asmreg)
        return make_ocode(0, 0, 0);
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != 7 && ap1->length != 8)
            return (OCODE*) - 2;
    }
    else
    {
        if (ap1->mode != am_freg)
            return (OCODE*) - 1;
    }
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_freg(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_freg)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_ficom(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*) - 1;
    if (ap1->length != 2 && ap1->length != 7 && ap1->length != 8)
        return (OCODE*) - 2;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fild(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*) - 1;
    if (ap1->length != 2 && ap1->length != 7 && ap1->length != 8)
        return (OCODE*) - 2;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fist(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*) - 1;
    if (ap1->length != 2 && ap1->length != 7)
        return (OCODE*) - 2;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fld(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != 7 && ap1->length != 8 && ap1->length != 10)
            return (OCODE*) - 2;
    }
    else if (ap1->mode != am_freg)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fst(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != 7 && ap1->length != 8)
            return (OCODE*) - 2;
    }
    else if (ap1->mode != am_freg || ap1->preg == 0)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fstp(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != 7 && ap1->length != 8 && ap1->length != 10)
            return (OCODE*) - 2;
    }
    else if (ap1->mode != am_freg || ap1->preg == 0)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_fucom(void)
{
    AMODE *ap1;
    if (lastst != kw_asmreg)
        return make_ocode(0, 0, 0);
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_freg || ap1->preg == 0)
        return (OCODE*) - 1;
    return make_ocode(ap1, 0, 0);

}

//-------------------------------------------------------------------------

static OCODE *ope_fxch(void)
{
    return ope_fucom();
}

//-------------------------------------------------------------------------

static OCODE *ope_mn(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*) - 1;
    ap1->length = 0;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_m16(void)
{
    AMODE *ap1;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*) - 1;
    if (ap1->mode == am_dreg)
        if (op != op_fstsw && op != op_fnstsw /* &&  op != op_fldsw */ && ap1
            ->preg != 0)
            return (OCODE*) - 1;
    if (ap1->length != 2)
        return (OCODE*) - 2;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_cmps(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_indisp || ap2->mode != am_indisp)
        return (OCODE*) - 1;
    if (ap1->preg != 6 || ap2->preg != 7)
        return (OCODE*) - 1;
    if (ap1->offset || ap2->offset)
        return (OCODE*) - 1;
    if (!ap1->seg || ap2->seg != 3)
        return (OCODE*) - 1;
    if (!ap1->length && !ap2->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_ins(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_indisp || ap2->mode != am_dreg)
        return (OCODE*) - 1;
    if (ap1->offset)
        return (OCODE*) - 1;
    if (ap1->preg != 7 || ap2->preg != 2)
        return (OCODE*) - 1;
    if (ap2->seg || ap1->seg != 3)
        return (OCODE*) - 1;
    if (ap2->length != 2 || !ap1->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_lods(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap1->offset)
        return (OCODE*) - 1;
    if (ap1->preg != 6)
        return (OCODE*) - 1;
    if (!ap1->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_movs(void)
{
    AMODE *ap1,  *ap2;
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    needpunc(comma, 0);
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap2->mode != am_indisp)
        return (OCODE*) - 1;
    if (ap1->preg != 6 || ap2->preg != 7)
        return (OCODE*) - 1;
    if (ap1->offset || ap2->offset)
        return (OCODE*) - 1;
    if (!ap1->seg || ap2->seg != 3)
        return (OCODE*) - 1;
    if (!ap1->length && !ap2->length)
        return (OCODE*) - 2;
    return make_ocode(ap2, ap1, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_outs(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    needpunc(comma, 0);
    ap2 = asm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_indisp || ap1->mode != am_dreg || ap2->offset)
        return (OCODE*) - 1;
    if (ap2->preg != 6 || ap1->preg != 2)
        return (OCODE*) - 1;
    if (ap1->seg || ap2->seg != 2)
        return (OCODE*) - 1;
    if (ap1->length != 2 || !ap2->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, ap2, 0);

}

//-------------------------------------------------------------------------

static OCODE *ope_scas(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap1->offset)
        return (OCODE*) - 1;
    if (ap1->preg != 7)
        return (OCODE*) - 1;
    if (ap1->seg != 3)
        return (OCODE*) - 1;
    if (!ap1->length)
        return (OCODE*) - 2;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_xlat(void)
{
    AMODE *ap1,  *ap2;
    ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap1->offset)
        return (OCODE*) - 1;
    if (ap1->preg != 3)
        return (OCODE*) - 1;
    if (ap1->length && ap1->length != 1)
        return (OCODE*) - 2;
    ap1->length = 1;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_reg32(void)
{
    AMODE *ap1 = asm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*) - 1;
    if (ap1->length != 4)
        return (OCODE*) - 2;
    return make_ocode(ap1, 0, 0);
}

//-------------------------------------------------------------------------

static OCODE *ope_stos(void)
{
    return ope_scas();
}

//-------------------------------------------------------------------------

OCODE *(*funcs[])(void) = 
{
    0, ope_math, ope_arpl, ope_bound, ope_bitscan, ope_bit, ope_call,
        ope_incdec, ope_rm, ope_enter, ope_imul, ope_in, ope_imm8, ope_relbra,
        ope_relbr8, ope_jmp, ope_regrm, ope_loadseg, ope_lgdt, ope_lidt,
        ope_rm16, ope_mov, ope_movsx, ope_out, ope_pushpop, ope_shift, ope_ret,
        ope_set, ope_shld, ope_test, ope_xchg, ope_fmath, ope_fmathp,
        ope_fmathi, ope_fcom, ope_freg, ope_ficom, ope_fild, ope_fist, ope_fld,
        ope_fst, ope_fstp, ope_fucom, ope_fxch, ope_mn, ope_m16, ope_cmps,
        ope_ins, ope_lods, ope_movs, ope_outs, ope_scas, ope_stos, ope_xlat,
        ope_reg32
};
SNODE *asm_statement(int shortfin)
{
    SNODE *snp = 0,  **snpp = &snp;
    OCODE *rv;
    ENODE *node;
    ASMNAME *ki;
    int iserr = 0;
    lastsym = 0;
    do
    {
        (*snpp) = xalloc(sizeof(SNODE));
        (*snpp)->stype = st_asm;
        (*snpp)->label = 0;
        (*snpp)->exp = 0;
        (*snpp)->next = 0;
        if (lastst != kw_asminst)
        {
            if (lastst == kw_int)
            {
                getsym();
                op = op_int;
                rv = ope_imm8();
                goto join;
            }
            node = asm_label();
            if (!node)
                return (*snpp);
            asmline = shortfin;
            if (lastst == semicolon)
                getsym();
            (*snpp)->stype = st_label;
            (*snpp)->label = (SNODE*)node->v.i;
            return (*snpp);
        }
        ki = keyimage;
        op = asm_op();
        if (op ==  - 1)
            return (*snpp);
        if (ki->amode == 0)
        {
            rv = xalloc(sizeof(OCODE));
            rv->oper1 = rv->oper2 = rv->oper3 = 0;
        }
        else
        {
            rv = (*funcs[ki->amode])();
            join: if (!rv || rv == (OCODE*) - 1 || rv == (OCODE*) - 2)
            {
                if (rv == (OCODE*) - 1)
                    asm_err(ERR_AILLADDRESS);
                if (rv == (OCODE*) - 2)
                    asm_err(ERR_AINVSIZE);
                iserr = 1;
                return (snp);
            }
        }
        if (rv->oper1 && rv->oper2)
        {
            if (!rv->oper1->length)
            if (!rv->oper2->length)
            {
                asm_err(ERR_AINVSIZE);
                iserr = 1;
            }
            else
                rv->oper1->length = rv->oper2->length;
            else
                if (!rv->oper2->length && ki->amode != OPE_BOUND && ki->amode 
                    != OPE_LOADSEG)
                    rv->oper2->length = rv->oper1->length;
        }
        rv->noopt = !prm_asmopt;
        rv->opcode = op;
        rv->fwd = rv->back = 0;
        (*snpp)->exp = rv;
        snpp = &(*snpp)->next;
    }
    while (op == op_rep || op == op_repnz || op == op_repz || op == op_repe ||
        op == op_repne || op == op_lock);
    asmline = shortfin;
    if (lastst == semicolon)
        getsym();
    return (snp);
}
