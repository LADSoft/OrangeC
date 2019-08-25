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
 * this module combines constants at compile time.  It is used e.g. 
 * for evaluating static assignments, but an initial folding of variable
 * expressions is done as well to slightly improve the code generation
 */
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "lists.h"
#include "rtti.h"
#include "diag.h"

extern int stdpragmas;

static int globstatvar;
static ENODE *asidehead,  **asidetail;
ULLONG_TYPE reint(ENODE *node);
LLONG_TYPE mod_mask(int i);

static unsigned LLONG_TYPE shifts[sizeof(LLONG_TYPE)*8] ;

void constoptinit(void)
{
    int i;
    for (i=0; i < sizeof(LLONG_TYPE) * 8; i++)
        shifts[i] = ((ULLONG_TYPE)1) << i;
}
static int optimizerfloatconst(ENODE *en)
{
    if ((en->cflags &DF_FENV_ACCESS) && !globstatvar)
        return FALSE;

    return isfloatconst(en->nodetype) || isimaginaryconst(en->nodetype) || iscomplexconst(en->nodetype);
}

/*-------------------------------------------------------------------------*/

static int isoptconst(ENODE *en)
{
    if (!en)
        return FALSE;
    return isintconst(en->nodetype) || optimizerfloatconst(en);
}

/*-------------------------------------------------------------------------*/

static int maxinttype(ENODE *ep1, ENODE *ep2)
{
    int type1 = ep1->nodetype;
    int type2 = ep2->nodetype;
    if (type1 == en_llucon || type2 == en_llucon)
        return en_llucon;
    if (type1 == en_llcon || type2 == en_llcon)
        return en_llcon;
    if (type1 == en_lucon || type2 == en_lucon)
        return en_lucon;
    if (type1 == en_lcon || type2 == en_lcon)
        return en_lcon;
    if (type1 == en_iucon || type2 == en_iucon)
        return en_iucon;
    if (type1 == en_icon || type2 == en_icon)
        return en_icon;
    return en_boolcon;
}

/*-------------------------------------------------------------------------*/

static int isunsignedexpr(ENODE *ep1)
{
    switch (ep1->nodetype)
    {
        case en_lucon:
        case en_iucon:
        case en_llucon:
            return TRUE;
        default:
            return FALSE;
    }
}

/*-------------------------------------------------------------------------*/

static int maxfloattype(ENODE *ep1, ENODE *ep2)
{
    int type1 = ep1->nodetype;
    int type2 = ep2->nodetype;
    if (type1 == en_lrcon || type2 == en_lrcon || type1 == en_lrimaginarycon ||
        type2 == en_lrimaginarycon)
        return en_lrcon;
    if (type1 == en_rcon || type2 == en_rcon || type1 == en_rimaginarycon ||
        type2 == en_rimaginarycon)
        return en_rcon;
    return en_fcon;
}

/*-------------------------------------------------------------------------*/

static int maximaginarytype(ENODE *ep1, ENODE *ep2)
{
    int type1 = ep1->nodetype;
    int type2 = ep2->nodetype;
    if (type1 == en_lrcon || type2 == en_lrcon || type1 == en_lrimaginarycon ||
        type2 == en_lrimaginarycon)
        return en_lrimaginarycon;
    if (type1 == en_rcon || type2 == en_rcon || type1 == en_rimaginarycon ||
        type2 == en_rimaginarycon)
        return en_rimaginarycon;
    return en_fimaginarycon;
}

/*-------------------------------------------------------------------------*/

static int maxcomplextype(ENODE *ep1, ENODE *ep2)
{
    int type1 = ep1->nodetype;
    int type2 = ep2->nodetype;
    if (type1 == en_lrcon || type2 == en_lrcon || type1 == en_lrimaginarycon ||
        type2 == en_lrimaginarycon || type1 == en_lrcomplexcon ||
        type2 == en_lrcomplexcon)
        return en_lrcomplexcon;
    if (type1 == en_rcon || type2 == en_rcon || type1 == en_rimaginarycon ||
        type2 == en_rimaginarycon || type1 == en_rcomplexcon || type2 == en_rcomplexcon)
        return en_rcomplexcon;
    return en_fcomplexcon;
}
static int hasFloats(ENODE *node)
/*
 * Go through a node and see if it will be promoted to type FLOAT
 */
{
    if (!node)
        return 0;
    switch (node->nodetype)
    {
		case en_cf:
		case en_cd:
		case en_cld:
		case en_cfc:
		case en_crc:
		case en_clrc:
		case en_cfi:
		case en_cri:
		case en_clri:
		case en_floatref:
		case en_doubleref:
		case en_longdoubleref:
		case en_fcomplexref:
		case en_rcomplexref:
		case en_lrcomplexref:
		case en_fimaginaryref:
		case en_rimaginaryref:
		case en_lrimaginaryref:
		case en_fcon:
		case en_rcon:
		case en_lrcon:
		case en_fcomplexcon:
		case en_rcomplexcon:
		case en_lrcomplexcon:
		case en_fimaginarycon:
		case en_rimaginarycon:
		case en_lrimaginarycon:
            return 1;
        case en_substack:
		case en_loadstack:
		case en_savestack:
        case en_nalabcon:
		case en_labcon:
        case en_trapcall:
        case en_nacon:
        case en_autocon:
        case en_napccon:
        case en_absacon:
        case en_cl_reg:
        case en_bits:
        case en_sp_ref:
        case en_fp_ref:
        case en_csp:
        case en_cfp:
		case en_cbit:
		case en_cbool:
		case en_cb:
		case en_cub:
		case en_cw:
		case en_cuw:
		case en_ci:
		case en_cui:
		case en_cl:
		case en_cul:
		case en_cll:
		case en_cull:
		case en_cp:
		case en_bool_ref:
		case en_b_ref:
		case en_ub_ref:
		case en_w_ref:
		case en_uw_ref:
		case en_i_ref:
		case en_ui_ref:
		case en_l_ref:
		case en_ul_ref:
		case en_ll_ref:
		case en_ull_ref:
		case en_a_ref:
		case en_ua_ref:
		case en_boolcon:
		case en_icon:
		case en_iucon:
		case en_lcon:
		case en_lucon:
		case en_llcon:
		case en_llucon:
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
        case en_not:
        case en_compl:
        case en_uminus:
        case en_stackblock:
		case en_moveblock:
        case en_clearblock:
            return 0;
        case en_assign:
		case en_asadd:
        case en_assub:
		case en_asmul:
		case en_asdiv:
		case en_asmod:
		case en_asrsh:
		case en_asumul:
		case en_asudiv:
        case en_asumod:
		case en_aslsh:
		case en_asarsh: 
		case en_asarshd:
		case en_asalsh:
		case en_asand:
		case en_asor:
		case en_asxor:
        case en_asuminus:
		case en_ascompl:
		case en_ainc:
        case en_adec:
        case en_add:
		case en_addstruc:
        case en_sub:
		case en_array:
		case en_arrayindex:
        case en_umul:
		case en_pmul:
        case en_udiv:
        case en_umod:
        case en_mul:
        case en_div:
		case en_pdiv:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_rsh:
        case en_arshd:
        case en_lsh:
		case en_alsh:
        case en_arsh:
        case en_land:
        case en_lor:
        case en_void:
		case en_dvoid:
		case en_pfcallb:
		case en_pcallblock:
		case en_sfcallb:
		case en_scallblock:
		case en_fcallb:
		case en_callblock:
            return (hasFloats(node->v.p[0]) || hasFloats(node->v.p[1]));
        case en_cond:
            return hasFloats(node->v.p[1]);
		case en_pfcall:
		case en_sfcall:
		case en_fcall:
//		case en_argnopush:
//		case en_not_lvalue:
            return hasFloats(node->v.p[0]);
		default:
			DIAG("hasfloats: unknown node type");
			break;
    }
    return (0);
}
/*-------------------------------------------------------------------------*/

static int getmode(ENODE *ep1, ENODE *ep2)
/*
 * get the constant mode of a pair of nodes
 * 0 = Neither node is a constant
 * 1 = icon,icon
 * 2 = icon,rcon
 * 3 = rcon,icon
 * 4 = rcon,rcon
 * 5 = icon,nothing
 * 6 = rcon,nothing
 * 7 = nothing,icon
 * 8 = nothing,rcon
 * 9 = rcon, ricon
 * 10 = ricon, rcon
 * 11 = ricon, ricon
 * 12 = ricon, nothing
 * 13 = nothing, ricon
 * 14 = ricon, icon
 * 15 = icon, ricon
 * 16 = ricon, cmplxcon
 * 17 = cmplxcon, ricon
 * 18 = rcon, cmplxcon
 * 19 = cmplxcon, rcon
 * 20 = cmplxcon, cmplxcon
 * 21 = cmplxcon, nothing
 * 22 = nothing, cmplxcon
 * 23 = cmplxcon, icon
 * 24 = icon, cmplxcon
 */
{
    int mode = 0;
    if (!globstatvar && (ep1->cflags &DF_FENV_ACCESS) && (isfloatconst(ep1->nodetype) ||
        isimaginaryconst(ep1->nodetype) || iscomplexconst(ep1->nodetype)))
        return 0;
    if (!globstatvar && ep2 && (ep2->cflags &DF_FENV_ACCESS) && (isfloatconst(ep2->nodetype) ||
        isimaginaryconst(ep2->nodetype) || iscomplexconst(ep2->nodetype)))
        return 0;
    if (isintconst(ep1->nodetype))
        if (ep2)
        {
            if (isintconst(ep2->nodetype))
                mode = 1;
            else if (isfloatconst(ep2->nodetype))
            {
                    mode = 2;
            }
            else if (isimaginaryconst(ep2->nodetype))
                    mode = 15;
            else if (iscomplexconst(ep2->nodetype))
                    mode = 24;
            else
                mode = 5;
        }
        else
            mode = 5;
    else if (isfloatconst(ep1->nodetype))
        if (ep2)
        {
            if (isintconst(ep2->nodetype))
                    mode = 3;
            else if (isfloatconst(ep2->nodetype))
                    mode = 4;
            else if (isimaginaryconst(ep2->nodetype))
                    mode = 9;
            else if (iscomplexconst(ep2->nodetype))
                    mode = 18;
            else
                mode = 6;
        }
        else
            mode = 6;
    else if (isimaginaryconst(ep1->nodetype))
        if (ep2)
        {
            if (isfloatconst(ep2->nodetype))
                    mode = 10;
            else if (isimaginaryconst(ep2->nodetype))
                    mode = 11;
            else if (iscomplexconst(ep2->nodetype))
                    mode = 16;
            else if (isintconst(ep2->nodetype))
                    mode = 14;
            else
                mode = 12;
        } else
            mode = 12;
    else if (iscomplexconst(ep1->nodetype))
        if (ep2)
        {
            if (isfloatconst(ep2->nodetype))
                    mode = 17;
            else if (isimaginaryconst(ep2->nodetype))
                    mode = 19;
            else if (iscomplexconst(ep2->nodetype))
                    mode = 20;
            else if (isintconst(ep2->nodetype))
                    mode = 23;
            else
                mode = 21;
        } else
            mode = 21;
    else if (ep2)
	{
        if (isintconst(ep2->nodetype))
            mode = 7;
        else if (ep2->nodetype == en_rcon || ep2->nodetype == en_lrcon || ep2
            ->nodetype == en_fcon)
                mode = 8;
        else if (ep2->nodetype == en_rimaginarycon || ep2->nodetype == en_fimaginarycon ||
            ep2->nodetype == en_lrimaginarycon)
                mode = 13;
        else if (ep2->nodetype == en_rcomplexcon || ep2->nodetype == en_fcomplexcon ||
            ep2->nodetype == en_lrcomplexcon)
                mode = 22;
	}
    return (mode);
}

/*-------------------------------------------------------------------------*/
ULLONG_TYPE CastToInt(int size, LLONG_TYPE value)
{
	int bits;
	switch(size) {
		default:
			return value ;
		case ISZ_BOOL:
			bits = 1;
			break ;
		case -ISZ_UCHAR:
		case ISZ_UCHAR:
			bits = 8;
			break;
		case -ISZ_USHORT:
		case ISZ_USHORT:
			bits = 2 * 8;
			break;
		case -ISZ_UINT:
		case ISZ_UINT:
			bits = 4 * 8;
			break ;
		case -ISZ_ULONG:
		case ISZ_ULONG:
			bits = 4 * 8;
			break ;
		case -ISZ_ULONGLONG:
		case ISZ_ULONGLONG:
			bits = 8 * 8;
			break ;
	}
	value &= mod_mask(bits);
	if (size < 0)
		if (value & (((ULLONG_TYPE)1) << (bits - 1)))
			value |= ~mod_mask(bits);
	return value;
}
FPF CastToFloat(int size, FPF *value)
{
	switch(size) {
		case ISZ_FLOAT:
		case ISZ_IFLOAT:
            FPFTruncate(value, 24, 128, -126);
			break;
		case ISZ_DOUBLE:
		case ISZ_IDOUBLE:
            FPFTruncate(value, 53, 1024, -1022);
			break;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            FPFTruncate(value, 64, 16384, -16382);
            break ;
	}
	return *value;
}
FPF *IntToFloat(FPF* temp, int size, LLONG_TYPE value)
{
    LLONG_TYPE t = CastToInt(size,value);
    if (size > 0)
        UnsignedLongLongToFPF(temp,t);
    else
        LongLongToFPF(temp,t);
    return temp;
}
FPF refloat(ENODE *node)
{
    FPF rv;
    FPF temp;
    switch (node->nodetype)
    {
        case en_icon:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,-ISZ_ULONGLONG,node->v.i));
            break;
        case en_lcon:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,-ISZ_ULONGLONG,node->v.i));
            break;
        case en_iucon:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_ULONGLONG,node->v.i));
			break;
        case en_lucon:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_ULONGLONG,node->v.i));
			break;
        case en_boolcon:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_BOOL,node->v.i));
			break;
        case en_llucon:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,ISZ_ULONGLONG,node->v.i));
			break;
        case en_llcon:
            rv = CastToFloat(ISZ_LDOUBLE,IntToFloat(&temp,-ISZ_ULONGLONG,node->v.i));
			break;
        case en_fcon:
        case en_fimaginarycon:
			rv = CastToFloat(ISZ_FLOAT,&node->v.f);
            break;
        case en_rcon:
        case en_rimaginarycon:
			rv = CastToFloat(ISZ_DOUBLE,&node->v.f);
            break;

        case en_lrcon:
        case en_lrimaginarycon:
			rv = CastToFloat(ISZ_LDOUBLE,&node->v.f);
            break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

ULLONG_TYPE reint(ENODE *node)
{
    ULLONG_TYPE rv;
    switch (node->nodetype)
    {
        case en_lcon:
			rv = CastToInt(-ISZ_ULONG, node->v.i);
            break;
        case en_icon:
			rv = CastToInt(-ISZ_UINT, node->v.i);
            break;
        case en_lucon:
			rv = CastToInt(ISZ_ULONG, node->v.i);
			break;
        case en_iucon:
			rv = CastToInt(ISZ_UINT, node->v.i);
            break;
        case en_boolcon:
			rv = CastToInt(ISZ_BOOL, node->v.i);
            break;
        case en_llucon:
			rv = CastToInt(ISZ_ULONGLONG, node->v.i);
            break;
        case en_llcon:
			rv = CastToInt(-ISZ_ULONGLONG, node->v.i);
            break;
        case en_fcon:
			rv = CastToInt(-ISZ_ULONGLONG, FPFToLongLong(&node->v.f));
            break;
        case en_rcon:
			rv = CastToInt(-ISZ_ULONGLONG, FPFToLongLong(&node->v.f));
            break;

        case en_lrcon:
			rv = CastToInt(-ISZ_ULONGLONG, FPFToLongLong(&node->v.f));
            break;
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
            rv = (LLONG_TYPE)0;
            break;
    }
    return rv;
}
/*-------------------------------------------------------------------------*/

void dooper(ENODE **node, int mode)
/*
 *      dooper will execute a constant operation in a node and
 *      modify the node to be the result of the operation.
 *			It will also cast integers to floating point values when
 *			necessary
 */
{
    ENODE *ep,  *ep1,  *ep2;
    FPF temp;
    ep =  *node;
    ep1 = ep->v.p[0];
    ep2 = ep->v.p[1];
    if (mode == 5)
    {
        if (hasFloats(ep2))
        {
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&ep1->v.f, ep1->v.i);
                else
                    LongLongToFPF(&ep1->v.f, ep1->v.i);
            ep1->nodetype = en_rcon;
        }
        return ;
    }
    else if (mode == 7)
    {
        if (hasFloats(ep1))
        {
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&ep2->v.f, ep2->v.i);
                else
                    LongLongToFPF(&ep2->v.f, ep2->v.i);
            ep2->nodetype = en_rcon;
        }
        return ;
    }
    else if (mode == 6 || mode == 8 || mode == 12 || mode == 13 || mode == 21 || mode == 22)
        return ;
    else
    switch (ep->nodetype)
    {
        case en_add:
		case en_addstruc:
            switch (mode)
            {
            case 1:
                ep->nodetype = maxinttype(ep1, ep2);
                ep->v.i = ep1->v.i + ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case 2:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(0,&temp, &ep2->v.f, &ep->v.f);
                break;
            case 3:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(0,&ep1->v.f, &temp, &ep->v.f);
                break;
            case 4:
                ep->nodetype = maxfloattype(ep1, ep2);
                AddSubFPF(0,&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 9:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.f ;
                ep->v.c.i = ep2->v.f;
                break ;
            case 10:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.f ;
                ep->v.c.i = ep1->v.f;
                break ;
            case 11:
                ep->nodetype = maximaginarytype(ep1,ep2);
                AddSubFPF(0,&ep1->v.f, &ep2->v.f, &ep->v.f);
                break ;
            case 14:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                LongLongToFPF(&ep->v.c.r, ep2->v.i);
                ep->v.c.i = ep1->v.f ;
                break;
            case 15:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                LongLongToFPF(&ep->v.c.r, ep1->v.i);
                ep->v.c.i = ep2->v.f ;
                break;
            case 16:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.c.r ;
                AddSubFPF(0,&ep1->v.f, &ep2->v.c.i, &ep->v.c.i);
                break;
            case 17:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.c.r ;
                AddSubFPF(0,&ep2->v.f, &ep1->v.c.i, &ep->v.c.i);
                break;
            case 18:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(0,&ep1->v.f, &ep2->v.c.r, &ep->v.c.r);
                ep->v.c.i = ep2->v.c.i;
                break;
            case 19:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(0,&ep2->v.f, &ep1->v.c.r, &ep->v.c.r);
                ep->v.c.i = ep1->v.c.i;
                break;
            case 20:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(0,&ep1->v.c.r, &ep2->v.c.r, &ep->v.c.r);
                AddSubFPF(0,&ep1->v.c.i, &ep2->v.c.i, &ep->v.c.i);
                break;
            case 23:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(0,&ep1->v.c.r, &temp, &ep->v.c.r);
                ep->v.c.i = ep1->v.c.i ;
                break;
            case 24:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(0,&ep2->v.c.r, &temp, &ep->v.c.r);
                ep->v.c.i = ep2->v.c.i ;
                break;
            }
            break;
        case en_sub:
            switch (mode)
            {
            case 1:
                ep->nodetype = maxinttype(ep1, ep2);
                ep->v.i = ep1->v.i - ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case 2:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(1,&temp, &ep2->v.f, &ep->v.f);
                break;
            case 3:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(1,&ep1->v.f, &temp, &ep->v.f);
                break;
            case 4:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(1,&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 9:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.f ;
                ep->v.c.i = ep2->v.f;
                ep->v.c.i.sign ^= 1;
                break ;
            case 10:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.f ;
                ep->v.c.r.sign ^= 1;
                ep->v.c.i = ep1->v.f;
                break ;
            case 11:
                ep->nodetype = maximaginarytype(ep1,ep2);
                AddSubFPF(1,&ep1->v.f, &ep2->v.f, &ep->v.f);
                break ;
            case 14:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&ep->v.c.r, ep2->v.i);
                else
                    LongLongToFPF(&ep->v.c.r, ep2->v.i);
                ep->v.c.i = ep1->v.f ;
                break;
            case 15:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&ep->v.c.r, ep1->v.i);
                else
                    LongLongToFPF(&ep->v.c.r, ep1->v.i);
                ep->v.c.i = ep2->v.f ;
                ep->v.c.i.sign ^= 1;
                break;
            case 16:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep2->v.c.r ;
                ep->v.c.r.sign ^= 1;
                AddSubFPF(1,&ep1->v.f, &ep2->v.c.i, & ep->v.c.i);
                break;
            case 17:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                ep->v.c.r = ep1->v.c.r ;
                AddSubFPF(1,&ep1->v.c.i, &ep2->v.f, & ep->v.c.i);
                break;
            case 18:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(1,&ep1->v.f, &ep2->v.c.r, & ep->v.c.r);
                ep->v.c.i = ep2->v.c.i;
                ep->v.c.i.sign ^= 1;
                break;
            case 19:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(1,&ep1->v.c.r, &ep2->v.f, & ep->v.c.r);
                ep->v.c.i = ep1->v.c.i;
                break;
            case 20:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                AddSubFPF(1,&ep1->v.c.r, &ep2->v.c.r, & ep->v.c.r);
                AddSubFPF(1,&ep1->v.c.i, &ep2->v.c.i, & ep->v.c.i);
                break;
            case 23:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                AddSubFPF(1,&ep1->v.c.r, &temp, & ep->v.c.r);
                ep->v.c.i = ep1->v.c.i ;
                break;
            case 24:
                ep->nodetype = maxcomplextype(ep1,ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                AddSubFPF(1,&temp, &ep2->v.c.r, & ep->v.c.r);
                ep->v.c.i = ep2->v.c.i ;
                ep->v.c.i.sign ^= 1;
                break;
            }
            break;
        case en_arrayindex:
        case en_umul:
		case en_pmul:
        case en_mul:
		case en_array:
            switch (mode)
            {
            case 1:
                ep->nodetype = maxinttype(ep1, ep2);
                ep->v.i = ep1->v.i *ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case 2:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                MultiplyFPF(&temp, & ep2->v.f, &ep->v.f);
                break;
            case 3:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                MultiplyFPF(&ep1->v.f, &temp, &ep->v.f);
                break;
            case 4:
                ep->nodetype = maxfloattype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 9:
                ep->nodetype = maximaginarytype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 10:
                ep->nodetype = maximaginarytype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 11:
                ep->nodetype = maxfloattype(ep1, ep2);
                MultiplyFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                ep->v.f.sign ^= 1;
                break;
            case 14:
                ep->nodetype = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                MultiplyFPF(&ep1->v.f, &temp, &ep->v.f);
                break;
            case 15:
                ep->nodetype = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                MultiplyFPF(&temp, &ep2->v.f, &ep->v.f);
                break;
            case 16:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep1->v.f, &ep2->v.c.i, &ep->v.c.r);
                ep->v.c.r.sign ^= 1;
                MultiplyFPF(&ep1->v.f, &ep2->v.c.r, &ep->v.c.i);
                break;
            case 17:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep2->v.f, &ep1->v.c.i, &ep->v.c.r);
                ep->v.c.r.sign ^= 1;
                MultiplyFPF(&ep2->v.f, &ep1->v.c.r, &ep->v.c.i);
                break;
            case 18:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep1->v.f, &ep2->v.c.r, &ep->v.c.r);
                MultiplyFPF(&ep1->v.f, &ep2->v.c.i, &ep->v.c.i);
                break;
            case 19:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                MultiplyFPF(&ep2->v.f, &ep1->v.c.r, &ep->v.c.r);
                MultiplyFPF(&ep2->v.f, &ep1->v.c.i, &ep->v.c.i);
                break;
			case 20:
				if (ep1->v.c.r.type == IFPF_IS_ZERO)
				{
	                ep->nodetype = maxcomplextype(ep1, ep2);
    	            ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
        	            && (ep2->cflags & DF_CXLIMITED)) ?
            	            STD_PRAGMA_CXLIMITED : 0;
                	MultiplyFPF(&ep2->v.c.r, &ep1->v.c.i, &ep->v.c.i);
                	MultiplyFPF(&ep2->v.c.i, &ep1->v.c.i, &ep->v.c.r);
					ep->v.c.r.sign ^= 1;
				}
				else if (ep1->v.c.i.type == IFPF_IS_ZERO)
				{
	                ep->nodetype = maxcomplextype(ep1, ep2);
    	            ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
        	            && (ep2->cflags & DF_CXLIMITED)) ?
            	            STD_PRAGMA_CXLIMITED : 0;
                	MultiplyFPF(&ep2->v.c.r, &ep1->v.c.r, &ep->v.c.r);
                	MultiplyFPF(&ep2->v.c.i, &ep1->v.c.r, &ep->v.c.i);
				}
				else if (ep2->v.c.r.type == IFPF_IS_ZERO)
				{
	                ep->nodetype = maxcomplextype(ep1, ep2);
    	            ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
        	            && (ep2->cflags & DF_CXLIMITED)) ?
            	            STD_PRAGMA_CXLIMITED : 0;
                	MultiplyFPF(&ep1->v.c.r, &ep2->v.c.i, &ep->v.c.i);
                	MultiplyFPF(&ep1->v.c.i, &ep2->v.c.i, &ep->v.c.r);
					ep->v.c.r.sign ^= 1;
				}
				else if (ep2->v.c.i.type == IFPF_IS_ZERO)
				{
	                ep->nodetype = maxcomplextype(ep1, ep2);
    	            ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
        	            && (ep2->cflags & DF_CXLIMITED)) ?
            	            STD_PRAGMA_CXLIMITED : 0;
                	MultiplyFPF(&ep1->v.c.r, &ep2->v.c.r, &ep->v.c.r);
                	MultiplyFPF(&ep1->v.c.i, &ep2->v.c.r, &ep->v.c.i);
				}
				break ;
            case 24:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                MultiplyFPF(&temp, &ep2->v.c.r, &ep->v.c.r);
                MultiplyFPF(&temp, &ep2->v.c.i, &ep->v.c.i);
                break;
            }
            break;
        case en_div:
        case en_udiv:
		case en_pdiv:
            switch (mode)
            {
            case 1:
                ep->nodetype = maxinttype(ep1, ep2);
                if (isunsignedexpr(ep1) || isunsignedexpr(ep2))
                    ep->v.i = (ULLONG_TYPE)ep1->v.i / (ULLONG_TYPE)ep2->v.i;
                else
                    ep->v.i = ep1->v.i / ep2->v.i;
                ep->v.i = reint(ep);
                break;
            case 2: 
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                DivideFPF(&temp, &ep2->v.f, &ep->v.f);
                break;
            case 3:
                ep->nodetype = maxfloattype(ep1, ep2);
                if (isunsignedexpr(ep2)) 
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                DivideFPF(&ep1->v.f, &temp, &ep->v.f);
                break;
            case 4:
                ep->nodetype = maxfloattype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 9:
                ep->nodetype = maximaginarytype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                ep->v.f.sign ^= 1;
                break;
            case 10:
                ep->nodetype = maximaginarytype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 11:
                ep->nodetype = maximaginarytype(ep1, ep2);
                DivideFPF(&ep1->v.f, &ep2->v.f, &ep->v.f);
                break;
            case 14:
                ep->nodetype = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                DivideFPF(&ep1->v.f, &temp, &ep->v.f);
                break;
            case 15:
                ep->nodetype = maximaginarytype(ep1, ep2);
                if (isunsignedexpr(ep1))
                    UnsignedLongLongToFPF(&temp, ep1->v.i);
                else
                    LongLongToFPF(&temp, ep1->v.i);
                DivideFPF(&temp, &ep2->v.f, &ep->v.f);
                ep->v.f.sign ^= 1;
                break;
            case 17:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                DivideFPF(&ep1->v.c.i, &ep2->v.f, &ep->v.c.r);
                DivideFPF(&ep1->v.c.r, &ep2->v.f, &ep->v.c.i);
                ep->v.c.i.sign ^= 1;
                break;
            case 19:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                DivideFPF(&ep1->v.c.i, &ep2->v.f, &ep->v.c.r);
                DivideFPF(&ep1->v.c.r, &ep2->v.f, &ep->v.c.i);
                break;
			case 20:
				if (ep2->v.c.r.type == IFPF_IS_ZERO)
				{
	                ep->nodetype = maxcomplextype(ep1, ep2);
    	            ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
        	            && (ep2->cflags & DF_CXLIMITED)) ?
            	            STD_PRAGMA_CXLIMITED : 0;
                	DivideFPF(&ep1->v.c.r, &ep2->v.c.i, &ep->v.c.i);
                	DivideFPF(&ep1->v.c.i, &ep2->v.c.i, &ep->v.c.r);
					ep->v.c.i.sign ^= 1;
				}
				else if (ep2->v.c.i.type == IFPF_IS_ZERO)
				{
	                ep->nodetype = maxcomplextype(ep1, ep2);
    	            ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
        	            && (ep2->cflags & DF_CXLIMITED)) ?
            	            STD_PRAGMA_CXLIMITED : 0;
                	DivideFPF(&ep1->v.c.r, &ep2->v.c.r, &ep->v.c.r);
                	DivideFPF(&ep1->v.c.i, &ep2->v.c.r, &ep->v.c.i);
				}
				break ;
            case 23:
                ep->nodetype = maxcomplextype(ep1, ep2);
                ep->cflags |= ((ep1->cflags & DF_CXLIMITED)
                    && (ep2->cflags & DF_CXLIMITED)) ?
                        STD_PRAGMA_CXLIMITED : 0;
                if (isunsignedexpr(ep2))
                    UnsignedLongLongToFPF(&temp, ep2->v.i);
                else
                    LongLongToFPF(&temp, ep2->v.i);
                DivideFPF(&ep1->v.c.r, &temp, &ep->v.c.r);
                DivideFPF(&ep1->v.c.i, &temp, &ep->v.c.i);
                break;
            }
            break;
        case en_mod:
            ep->nodetype = maxinttype(ep1, ep2);

            if (isunsignedexpr(ep1) || isunsignedexpr(ep2))
                ep->v.i = (ULLONG_TYPE)ep1->v.i % (ULLONG_TYPE)ep2->v.i;
            else
                ep->v.i = ep1->v.i % ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_lsh:
		case en_alsh:
            ep->nodetype = ep1->nodetype; /* maxinttype(ep1, ep2);*/
            ep->v.i = ep1->v.i << ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_rsh:
            ep->nodetype = ep1->nodetype; /* maxinttype(ep1, ep2);*/
            ep1->v.i = reint(ep1);
            ep->v.i = ((ULLONG_TYPE)ep1->v.i) >> ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_arsh:
            ep->nodetype = ep1->nodetype; /* maxinttype(ep1, ep2);*/
            ep->v.i = ep1->v.i >> ep2->v.i;			
            ep->v.i = reint(ep);
            break;
        case en_and:
            ep->nodetype = maxinttype(ep1, ep2);
            ep->v.i = ep1->v.i &ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_or:
            ep->nodetype = maxinttype(ep1, ep2);
            ep->v.i = ep1->v.i | ep2->v.i;
            ep->v.i = reint(ep);
            break;
        case en_xor:
            ep->nodetype = maxinttype(ep1, ep2);
            ep->v.i = ep1->v.i ^ ep2->v.i;
            ep->v.i = reint(ep);
            break;
    }
}

int pwrof2(LLONG_TYPE i)
/*
 *      return which power of two i is or -1.
 */
{
	if (i > 1) 
	{
	    int top = sizeof(shifts)/sizeof(LLONG_TYPE);
    	int bottom =  - 1;
    	while (top - bottom > 1)
    	{
        	int mid = (top + bottom) / 2;
	        if (i < shifts[mid])
    	    {
        	    top = mid;
	        }
    	    else
        	{
	            bottom = mid;
    	    }
		}
	    if (bottom <= 0) /* ignore the oth power*/
    	    return -1;
		if (i == shifts[bottom])
		{
			return bottom;
		}
	}
	return -1 ;
}

/*-------------------------------------------------------------------------*/

LLONG_TYPE mod_mask(int i)
/*
 *      make a mod mask for a power of two.
 */
{
	if (i >= sizeof(shifts)/sizeof(LLONG_TYPE))
		return (LLONG_TYPE)-1;
	return shifts[i] - 1;		
}

/*-------------------------------------------------------------------------*/

void addaside(ENODE *node)
{
    *asidetail = makenode(en_void, node, 0);
    asidetail = &(*asidetail)->v.p[1];
}

/*-------------------------------------------------------------------------*/

int opt0(ENODE **node)
/*
 *      opt0 - delete useless expressions and combine constants.
 *
 *      opt0 will delete expressions such as x + 0, x - 0, x * 0,
 *      x * 1, 0 / x, x / 1, x mod 0, etc from the tree pointed to
 *      by node and combine obvious constant operations. It cannot
 *      combine name and label constants but will combine icon type
 *      nodes.
 *
 * en_array is not optimized to keep from optimizing struct members
 * into regs later...  just en: en_array is now equiv to en_add
 * again.  I have fixed the optimize struct thing another way...
 * leaving en_array in in case we need it later.
 */
{
    ENODE *ep;
    LLONG_TYPE val;
    int sc;
    int rv = FALSE;
    int mode;
    FPF dval;
    int negtype = en_uminus;

    ep =  *node;

    if (ep == 0)
        return FALSE ;
    switch (ep->nodetype)
    {
        case en_sp_ref:
        case en_fp_ref:
        case en_bool_ref:
        case en_cbool:
        case en_cbit:
        case en_b_ref:
        case en_w_ref:
             /* optimize unary node */
        case en_ub_ref:
        case en_uw_ref:
             /* optimize unary node */
		case en_a_ref:
		case en_ua_ref:
		case en_i_ref:
		case en_ui_ref:
        case en_l_ref:
        case en_ul_ref:
        case en_ll_ref:
        case en_ull_ref:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_fimaginaryref:
        case en_rimaginarycon:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_cb:
        case en_cub:
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
		case en_crc:
		case en_cfc:
		case en_clrc:
		case en_cri:
		case en_cfi:
		case en_clri:
		case en_cl_reg:
		case en_bits:
            rv |= opt0(&((*node)->v.p[0]));
            return rv ;
        case en_compl:
            rv |= opt0(&(ep->v.p[0]));
            if (isintconst(ep->v.p[0]->nodetype))
            {
                rv = TRUE;
                ep->nodetype = ep->v.p[0]->nodetype;
                ep->v.i = ~ep->v.p[0]->v.i;
                ep->v.i = reint(ep);
            }
            return rv ;

        case en_uminus:
		case en_asuminus:
            rv = opt0(&(ep->v.p[0]));
            if (isintconst(ep->v.p[0]->nodetype))
            {
                rv = TRUE;
                ep->nodetype = ep->v.p[0]->nodetype;
                ep->v.i =  - ep->v.p[0]->v.i;
            }
            else if (ep->v.p[0]->nodetype == en_rcon || ep->v.p[0]->nodetype ==
                en_fcon || ep->v.p[0]->nodetype == en_lrcon ||
                ep->v.p[0]->nodetype == en_rimaginarycon || ep->v.p[0]->nodetype ==
                en_fimaginarycon || ep->v.p[0]->nodetype == en_lrimaginarycon)
            {
                rv = TRUE;
                ep->nodetype = ep->v.p[0]->nodetype;
                ep->v.f =  ep->v.p[0]->v.f;
                ep->v.f.sign ^= 1;
            }
            else if (ep->v.p[0]->nodetype == en_rcomplexcon || ep->v.p[0]->nodetype ==
                en_fcomplexcon || ep->v.p[0]->nodetype == en_lrcomplexcon)
            {
                rv = TRUE;
                ep->nodetype = ep->v.p[0]->nodetype;
                ep->v.c.r =  ep->v.p[0]->v.c.r;
                ep->v.c.r.sign ^= 1;
                ep->v.c.i =  ep->v.p[0]->v.c.i;
                ep->v.c.i.sign ^= 1;
            }
            return rv ;
        case en_add:
		case en_addstruc:
		case en_array:
        case en_sub:
		case en_asadd:
		case en_assub:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
            case 2:
            case 3:
            case 4:
            default:
                rv = TRUE;
                dooper(node, mode);
                break;
            case 5:
                if (ep->v.p[0]->v.i == 0)
                {
                    if (ep->v.p[0]->nodetype != en_autocon)
                    {
                        if (ep->nodetype == en_sub)
                            *node = makenode(en_uminus, ep->v.p[1], 0);
                        else
                            *node = ep->v.p[1];
                        rv = TRUE;
                    }
                }
                else
                {
                    dooper(node, mode);
                    rv = TRUE;
                }
                break;
            case 6:
                if (ep->v.p[0]->v.f.type == IFPF_IS_ZERO)
                {
                    if (ep->nodetype == en_sub)
                    {
                        *node = makenode(en_uminus, ep->v.p[1], 0);
                    }
                    else
                         *node = ep->v.p[1];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->v.p[1]->v.i == 0)
                {
                    if (ep->v.p[0]->nodetype != en_autocon)
                    {
                        *node = ep->v.p[0];
                        rv = TRUE;
                    }
                }
                else
                {
                    dooper(node, mode);
                    rv = TRUE;
                }
                break;
            case 8:
                if (ep->v.p[1]->v.f.type == IFPF_IS_ZERO)
                {
                    *node = ep->v.p[0];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
#ifdef XXXXX
            case 21:
                if (ep->v.p[0]->v.c.r.type == IFPF_IS_ZERO &&
                    ep->v.p[0]->v.c.i.type == IFPF_IS_ZERO)
                {
                    *node = ep->v.p[1];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
#endif
            case 22:
                if (ep->v.p[1]->v.c.r.type == IFPF_IS_ZERO &&
                    ep->v.p[1]->v.c.i.type == IFPF_IS_ZERO)
                {
                    *node = ep->v.p[0];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            return rv ;
        case en_mul:
        case en_umul:
		case en_pmul:
        case en_arrayindex:
		case en_asmul:
		case en_asumul:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
            case 2:
            case 3:
            case 4:
			case 9:
			case 10:
			case 11:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (!hasFloats(ep->v.p[1]))
                {
	                val = ep->v.p[0]->v.i;
	                if (val == 0)
	                {
	                    addaside(ep->v.p[1]);
	                    *node = ep->v.p[0];
	                }
	                else if (val == 1)
	                    *node = ep->v.p[1];
	                else if (val ==  - 1)
	                    *node = makenode(negtype, ep->v.p[1], 0);
	                else
					{
	                    sc = pwrof2(val);
	                    if (sc !=  - 1)
	                    {
	                        ENODE *temp = ep->v.p[0];
	                        ep->v.p[0] = ep->v.p[1];
	                        ep->v.p[1] = temp;
	                        ep->v.p[1]->v.i = sc;
	                        rv = TRUE;
	                        switch (ep->nodetype)
	                        {
							case en_asmul:
							case en_asumul:
								ep->nodetype = en_asalsh;
								break;
	                        case en_mul:
	                            ep->nodetype = en_lsh;
	                            break;
	                        case en_umul:
							case en_pmul:
//	                        case en_arrayindex:
							case en_array:
	                            ep->nodetype = en_lsh;
	                            break;
	                        }
	                        break;
	                    }
					}
                }
                dooper(node, mode);
                rv = TRUE;
                break;
            case 6:
                dval = ep->v.p[0]->v.f;
#ifdef XXXXX
                if (dval.type == IFPF_IS_ZERO)
                {
                    addaside(ep->v.p[1]);
                    *node = ep->v.p[0];
                }
                else 
#endif
				if (!dval.sign && ValueIsOne(&dval))
                    *node = ep->v.p[1];
                else if (dval.sign && ValueIsOne(&dval))
                    *node = makenode(negtype, ep->v.p[1], 0);
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (!hasFloats(ep->v.p[0]))
                {
	                val = ep->v.p[1]->v.i;
	                if (val == 0)
	                {
	                    addaside(ep->v.p[0]);
	                    *node = ep->v.p[1];
	                }
	                else if (val == 1)
	                    *node = ep->v.p[0];
	                else if (val ==  - 1)
	                {
	                    if (ep->nodetype == en_asmul || ep->nodetype == en_asumul)
	                        negtype = en_asuminus;
	                    *node = makenode(negtype, ep->v.p[0], 0);
	                }
	                else
					{
	                    sc = pwrof2(val);
	                    if (sc !=  - 1)
	                    {
	                        rv = TRUE;
	                        ep->v.p[1]->v.i = sc;
	                        switch (ep->nodetype)
	                        {
							case en_asmul:
							case en_asumul:
								ep->nodetype = en_asalsh;
								break;
	                        case en_mul:
	                            ep->nodetype = en_lsh;
	                            break;
	                        case en_umul:
							case en_pmul:
//	                        case en_arrayindex:
							case en_array:
	                            ep->nodetype = en_lsh;
	                            break;
	                        }
	                        break;
	                    }
					}
                }
                dooper(node, mode);
                rv = TRUE;
                break;
            case 8:
                dval = ep->v.p[1]->v.f;
#ifdef XXXXX
                if (dval.type == IFPF_IS_ZERO)
                {
                    *node = ep->v.p[1];
                    addaside(ep->v.p[0]);
                }
                else
#endif
				if (!dval.sign && ValueIsOne(&dval))
                    *node = ep->v.p[0];
                else if (dval.sign && ValueIsOne(&dval))
                {
                    if (ep->nodetype == en_asmul || ep->nodetype == en_asumul)
	                  negtype = en_asuminus;
                    *node = makenode(negtype, ep->v.p[0], 0);
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
#ifdef XXXXX
/* multiply by imaginary const changes the mode of the other value...*/
/* but there is no easy way to do that at this stage.  A simple cast will*/
/* result in conversion to zero, and while we could change simple variable*/
/* references by examination of the enode we could *not* change complicated expressions*/
            case 12:
                dval = ep->v.p[0]->v.f;
                if (dval.type == IFPF_IS_ZERO)
                {
                    addaside(ep->v.p[1]);
                    *node = ep->v.p[0];
                }
                else if (!dval.sign && ValueIsOne(&dval))
                    *node = ep->v.p[1];
                else if (dval.sign && ValueIsOne(&dval))
                    *node = makenode(negtype, (char*)ep->v.p[1], 0);
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 13:
                dval = ep->v.p[1]->v.f;
                if (dval.type == IFPF_IS_ZERO)
                {
                    *node = ep->v.p[1];
                    addaside(ep->v.p[0]);
                }
                else if (!dval.sign && ValueIsOne(&dval))
                    *node = ep->v.p[0];
                else if (dval.sign && ValueIsOne(&dval))
                    *node = makenode(negtype, (char*)ep->v.p[0], 0);
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
#endif
#ifdef XXXXX
            case 21:
                dval = ep->v.p[0]->v.c.r;
                if (ep->v.p[0]->v.c.i.type == IFPF_IS_ZERO)
                {
                    if (!dval.sign && ValueIsOne(&dval))
                        *node = ep->v.p[1];
                    else if (dval.sign && ValueIsOne(&dval))
                        *node = makenode(negtype, ep->v.p[1], 0);
                    else
                        dooper(node, mode);
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
#endif
            case 22:
                dval = ep->v.p[1]->v.c.r;
                if (ep->v.p[1]->v.c.i.type == IFPF_IS_ZERO)
                {
                    if (!dval.sign && ValueIsOne(&dval))
                        *node = ep->v.p[0];
                    else if (dval.sign && ValueIsOne(&dval))
                    {
                        if (ep->nodetype == en_asmul || ep->nodetype == en_asumul)
	                      negtype = en_asuminus;
                        *node = makenode(negtype, ep->v.p[0], 0);
                    }
                    else
                        dooper(node, mode);
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_div:
        case en_udiv:
		case en_pdiv:
		case en_asdiv:
		case en_asudiv:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
            case 2:
            case 3:
            case 4:
			case 20:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->v.p[0]->v.i == 0)
                {
                    addaside(ep->v.p[1]);
                    *node = ep->v.p[0];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 6:
#ifdef XXXXX
                if (ep->v.p[0]->v.f.type == IFPF_IS_ZERO)
                {
                    addaside(ep->v.p[1]);
                    *node = ep->v.p[0];
                }
                else
#endif
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (!hasFloats(ep->v.p[0]))
                {
	                val = ep->v.p[1]->v.i;
	                if (val == 1)
	                    *node = ep->v.p[0];
	                else if (val ==  - 1)
	                {
	                    if (ep->nodetype == en_asdiv || ep->nodetype == en_asudiv)
	                        negtype = en_asuminus;
	                    *node = makenode(negtype, ep->v.p[0], 0);
	                }
	                else
					{
	                    sc = pwrof2(val);
	                    if (sc !=  - 1)
	                    {
							int n = natural_size(ep);
	                        rv = TRUE;
	                        switch (ep->nodetype)
	                        {
							case en_asdiv:
								if (n != ISZ_ULONGLONG  && n != - ISZ_ULONGLONG)
								{
									ep->nodetype = en_asarshd;
		    	                    ep->v.p[1]->v.i = sc;
								}
								break;
							case en_asudiv:
								ep->nodetype = en_asarsh;
		                        ep->v.p[1]->v.i = sc;
								break;
	                        case en_div:
								if (n != ISZ_ULONGLONG  && n != - ISZ_ULONGLONG)
								{
		                            ep->nodetype = en_arshd;
			                        ep->v.p[1]->v.i = sc;
								}
	                            break;
	                        case en_udiv:
							case en_pdiv:
	                            ep->nodetype = en_rsh;
		                        ep->v.p[1]->v.i = sc;
	                            break;
	                        }
	                        break;
	                    }
					}
                }
                dooper(node, mode);
                rv = TRUE;
                break;
            case 8:
                dval = ep->v.p[1]->v.f;
#ifdef XXXXX
                if (dval.type == IFPF_IS_ZERO)
                    *node = ep->v.p[0];
                else 
#endif
				if (!dval.sign && ValueIsOne(&dval))
                    *node = ep->v.p[0];
                else if (dval.sign && ValueIsOne(&dval))
                {
                    if (ep->nodetype == en_asdiv || ep->nodetype == en_asudiv)
	                  negtype = en_asuminus;
                    *node = makenode(negtype, ep->v.p[0], 0);
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 22:
                dval = ep->v.p[1]->v.c.r;
                if (ep->v.p[1]->v.c.i.type == IFPF_IS_ZERO)
                {
                    if (!dval.sign && ValueIsOne(&dval))
                        *node = ep->v.p[0];
                    else if (dval.sign && ValueIsOne(&dval))
                    {
                        if (ep->nodetype == en_asdiv || ep->nodetype == en_asudiv)
	                      negtype = en_asuminus;
                        *node = makenode(negtype, ep->v.p[0], 0);
                    }
                    else
                        dooper(node, mode);
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_mod:
        case en_umod:
		case en_asmod:
		case en_asumod:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 7:
                if (!hasFloats(ep->v.p[0]))
                {
                    sc = pwrof2(ep->v.p[1]->v.i);
                    if (sc !=  - 1)
                    {
                        rv = TRUE;
                        ep->v.p[1]->v.i = mod_mask(sc);
                        switch (ep->nodetype)
                        {
							case en_mod:
							case en_umod:
		                        ep->nodetype = en_and;
		                        break;
							case en_asmod:
							case en_asumod:
		                        ep->nodetype = en_asand;
		                        break;
                        }
                    }
                }
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 8:
                dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_asand:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_and:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->v.p[0]->v.i == 0)
                {
                    addaside(ep->v.p[1]);
                    *node = ep->v.p[0];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->v.p[1]->v.i == 0)
                {
                    addaside(ep->v.p[0]);
                    *node = ep->v.p[1];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_or:
        case en_xor:
		case en_asor:
		case en_asxor:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->v.p[0]->v.i == 0)
                {
                    *node = ep->v.p[1];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->v.p[1]->v.i == 0)
                {
                    *node = ep->v.p[0];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            }
            break;
        case en_lsh:
        case en_rsh:
        case en_arsh:
        case en_alsh:
        case en_arshd:
        case en_aslsh:
        case en_asarsh:
        case en_asalsh:
        case en_asarshd:
            rv |= opt0(&(ep->v.p[0]));
            if (isintconst(ep->v.p[0]->nodetype) && ep->v.p[1]->nodetype ==
                en_cl_reg)
            if (isintconst(ep->v.p[1]->v.p[0]->nodetype))
            {
                ep->v.p[1] = ep->v.p[1]->v.p[0];
                rv = TRUE;
            }
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                dooper(node, mode);
                rv = TRUE;
                break;
            case 5:
                if (ep->v.p[0]->v.i == 0)
                {
                    addaside(ep->v.p[1]);
                    *node = ep->v.p[0];
                }
                else
                    dooper(node, mode);
                rv = TRUE;
                break;
            case 7:
                if (ep->v.p[1]->v.i == 0)
                {
                    *node = ep->v.p[0];
                }
                else
                {
                    dooper(node, mode);
                }
                rv = TRUE;
                break;
            }
            break;
        case en_land:
            rv |= opt0(&(ep->v.p[0]));
			if (isintconst(ep->v.p[0]->nodetype))
			{
				if (!ep->v.p[0]->v.i)
				{
					rv = TRUE;
					*node = makeintnode(en_icon, 0);
					break;
				}
				else
				{
			            rv |= opt0(&(ep->v.p[1]));
					goto join_land;
				}
			}
			rv |= opt0(&(ep->v.p[1]));
			switch(ep->v.p[0]->nodetype)
			{
				case en_nalabcon:
				case en_napccon:
				case en_nacon:
				case en_labcon:
				case en_autocon:
				{
join_land:
					if (isintconst(ep->v.p[1]->nodetype))
					{
						rv = TRUE;
						*node = makeintnode(en_icon, !!ep->v.p[1]->v.i);
					}
					else switch(ep->v.p[1]->nodetype)
					{
						case en_nalabcon:
						case en_napccon:
						case en_nacon:
						case en_labcon:
						case en_autocon:
							/* assumes nothing can be relocated to address 0 */
		    	            *node = makeintnode(en_icon, 1);
							rv = TRUE;
							break;
						default:
							break;
					}
				}
				default:
					break;
			}
            break;
        case en_lor:
            rv |= opt0(&(ep->v.p[0]));
			if (isintconst(ep->v.p[0]->nodetype))
			{
				if (ep->v.p[0]->v.i)
				{
					rv = TRUE;
					*node = makeintnode(en_icon, 1);
					break;
				}
				else
				{
			            rv |= opt0(&(ep->v.p[1]));
					goto join_lor;
				}
			}
            rv |= opt0(&(ep->v.p[1]));
			switch(ep->v.p[0]->nodetype)
			{
				case en_nalabcon:
				case en_napccon:
				case en_nacon:
				case en_labcon:
				case en_autocon:
					/* assumes nothing can be relocated to address 0 */
    	            *node = makeintnode(en_icon, 1);
					rv = TRUE;
					break;
				default:
				{
join_lor:
					if (isintconst(ep->v.p[1]->nodetype))
					{
						rv = TRUE;
						*node = makeintnode(en_icon, !!ep->v.p[1]->v.i);
					}
					else switch(ep->v.p[1]->nodetype)
					{
						case en_nalabcon:
						case en_napccon:
						case en_nacon:
						case en_labcon:
						case en_autocon:
							/* assumes nothing can be relocated to address 0 */
		    	            *node = makeintnode(en_icon, 1);
							rv = TRUE;
							break;
						default:
							break;
					}
				}
			}
            break;
        case en_not:
            rv |= opt0(&(ep->v.p[0]));
            if (isintconst(ep->v.p[0]->nodetype))
            {
                *node = makeintnode(en_icon, (!ep->v.p[0]->v.i));
                rv = TRUE;
            }
            else if (isfloatconst(ep->v.p[0]->nodetype))
            {
                *node = makeintnode(en_icon, ep->v.p[0]->v.f.type == IFPF_IS_ZERO);
                rv = TRUE;
            }
            break;
        case en_eq:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, (ep->v.p[0]->v.i == ep->v.p[1]
                    ->v.i));
                rv = TRUE;
                break;
            case 4:
                *node = makeintnode(en_icon, FPFEQ(&ep->v.p[0]->v.f, &ep->v.p[1]->v.f));
                rv = TRUE;
                break;
            }
            break;
        case en_ne:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, (ep->v.p[0]->v.i != ep->v.p[1]
                    ->v.i));
                rv = TRUE;
                break;
            case 4:
                *node = makeintnode(en_icon, !FPFEQ(&ep->v.p[0]->v.f, &ep->v.p[1]->v.f));
                rv = TRUE;
                break;
            }
            break;
        case en_lt:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, (ep->v.p[0]->v.i < ep->v.p[1]->v.i)
                    );
                rv = TRUE;
                break;
            case 4:
                *node = makeintnode(en_icon, !FPFGTE(&ep->v.p[0]->v.f, &ep->v.p[1]->v.f));
                rv = TRUE;
                break;
            }
            break;
        case en_le:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, (ep->v.p[0]->v.i <= ep->v.p[1]
                    ->v.i));
                rv = TRUE;
                break;
            case 4:
                *node = makeintnode(en_icon, !FPFGT(&ep->v.p[0]->v.f, &ep->v.p[1]->v.f));
                rv = TRUE;
                break;
            }
            break;
        case en_ugt:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, ((ULLONG_TYPE)ep->v.p[0]->v.i > 
                    (ULLONG_TYPE)ep->v.p[1]->v.i));
                rv = TRUE;
                break;
            }
            break;
        case en_uge:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, ((ULLONG_TYPE)ep->v.p[0]->v.i >= 
                    (ULLONG_TYPE)ep->v.p[1]->v.i));
                rv = TRUE;
                break;
            }
            break;
        case en_ult:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, ((ULLONG_TYPE)ep->v.p[0]->v.i < 
                    (ULLONG_TYPE)ep->v.p[1]->v.i));
                rv = TRUE;
                break;
            }
            break;
        case en_ule:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, ((ULLONG_TYPE)ep->v.p[0]->v.i <= 
                    (ULLONG_TYPE)ep->v.p[1]->v.i));
                rv = TRUE;
                break;
            }
            break;
        case en_gt:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, (ep->v.p[0]->v.i > ep->v.p[1]->v.i)
                    );
                rv = TRUE;
                break;
            case 4:
                *node = makeintnode(en_icon, FPFGT(&ep->v.p[0]->v.f, &ep->v.p[1]->v.f));
                rv = TRUE;
                break;
            }
            break;
        case en_ge:
            rv |= opt0(&(ep->v.p[0]));
            rv |= opt0(&(ep->v.p[1]));
            mode = getmode(ep->v.p[0], ep->v.p[1]);
            switch (mode)
            {
            case 1:
                *node = makeintnode(en_icon, (ep->v.p[0]->v.i >= ep->v.p[1]
                    ->v.i));
                rv = TRUE;
                break;
            case 4:
                *node = makeintnode(en_icon, FPFGTE(&ep->v.p[0]->v.f, &ep->v.p[1]->v.f));
                rv = TRUE;
                break;
            }
            break;


        case en_cond:
			opt0(&ep->v.p[0]);
            if (isoptconst(ep->v.p[0])) {
                if (isfloatconst(ep->v.p[0]->nodetype)) {
                    if (ep->v.p[0]->v.f.type != IFPF_IS_ZERO)
                        *node = ep->v.p[1]->v.p[0];
                    else
                        *node = ep->v.p[1]->v.p[1];
                    rv |= opt0(node);
                    break;
                } else if (isintconst(ep->v.p[0]->nodetype)) {
                    if (ep->v.p[0]->v.i)
                        *node = ep->v.p[1]->v.p[0];
                    else
                        *node = ep->v.p[1]->v.p[1];
                    rv |= opt0(node);
                    break;
                }
            }
            /* Fall Through*/
        case en_void:
		case en_dvoid:
        case en_intcall:
        case en_voidnz:
        case en_assign:
		case en_ascompl:
		case en_moveblock:
        case en_stackblock:
        case en_thiscall:
        case en_ainc:
        case en_adec:
		case en_pfcallb:
		case en_pcallblock:
		case en_sfcallb:
		case en_scallblock:
		case en_fcallb:
		case en_callblock:
		case en_pfcall:
		case en_sfcall:
		case en_fcall:
            rv |= opt0(&(ep->v.p[1]));
        case en_trapcall:
        case en_substack:
		case en_loadstack:
		case en_savestack:
        case en_clearblock:
//		case en_argnopush:
//		case en_not_lvalue:
            rv |= opt0(&(ep->v.p[0]));
			break;
		default:
			break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

void enswap(ENODE **one, ENODE **two)
{
    ENODE *temp =  *one;
    *one =  *two;
    *two = temp;
}

/*-------------------------------------------------------------------------*/

int fold_const(ENODE *node)
/*
 *      fold_const will remove constant nodes and return the values to
 *      the calling routines.
 */
{
    int rv = FALSE;
    if (node == 0)
        return 0;
    switch (node->nodetype)
    {
        case en_add:
		case en_addstruc:
		case en_array:
            rv |= fold_const(node->v.p[0]);
            rv |= fold_const(node->v.p[1]);
            if (isoptconst(node->v.p[1]))
            {
                switch (node->v.p[0]->nodetype)
                {
                case en_add:
				case en_addstruc:
				case en_array:
                    if (isoptconst(node->v.p[0]->v.p[0]))
                    {
                        int type = en_add;
                        if (node->nodetype == en_array || node->v.p[0]
   	                        ->nodetype == en_array)
       	                    type = en_array;
                        node->nodetype = type;
                        node->v.p[0]->nodetype = en_add;
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[0]->v.p[1]))
                    {
                        int type = en_add;

                        if (node->nodetype == en_array || node->v.p[0]
   	                        ->nodetype == en_array)
                            type = en_array;
                        node->nodetype = type;
                        node->v.p[0]->nodetype = en_add;
                        enswap(&node->v.p[0]->v.p[0], &node->v.p[1]);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->v.p[0]->v.p[0]))
                    {
                        int type = en_add;
                        if (node->nodetype == en_array)
                            type = en_array;
                        node->nodetype = en_sub;
                        node->v.p[0]->nodetype = type;
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[0]->v.p[1]))
                    {
                        int type = en_add;
                        if (node->nodetype == en_array)
                            type = en_array;
                        node->nodetype = type;
                        node->v.p[0]->nodetype = en_sub;
                        enswap(&node->v.p[0]->v.p[0], &node->v.p[1]);
                        rv = TRUE;
                    }
                    break;
                }
            }
            else if (isoptconst(node->v.p[0]))
            {
                switch (node->v.p[1]->nodetype)
                {
                case en_add:
        		case en_addstruc:
        case en_array:
                    if (isoptconst(node->v.p[1]->v.p[0]))
                    {
                        int type = en_add;
                        if (node->nodetype == en_array || node->v.p[1]
                            ->nodetype == en_array)
                            type = en_array;
                        node->nodetype = type;
                        node->v.p[1]->nodetype = en_add;
                        enswap(&node->v.p[1]->v.p[1], &node->v.p[0]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[1]->v.p[1]))
                    {
                        int type = en_add;
                        if (node->nodetype == en_array || node->v.p[1]
                            ->nodetype == en_array)
                            type = en_array;
                        node->nodetype = type;
                        node->v.p[1]->nodetype = en_add;
                        enswap(&node->v.p[1]->v.p[0], &node->v.p[0]);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->v.p[1]->v.p[0]))
                    {
                        node->nodetype = en_sub;
                        node->v.p[1]->nodetype = en_add;
                        enswap(&node->v.p[0], &node->v.p[1]);
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[1]->v.p[1]))
                    {
                        int type = en_add;
                        if (node->nodetype == en_array)
                            type = en_array;
                        node->nodetype = type;
                        node->v.p[1]->nodetype = en_sub;
                        enswap(&node->v.p[1]->v.p[0], &node->v.p[0]);
                        rv = TRUE;
                    }
                    break;
                }
            }
            break;
        case en_sub:
            rv |= fold_const(node->v.p[0]);
            rv |= fold_const(node->v.p[1]);
            if (isoptconst(node->v.p[1]))
            {
                switch (node->v.p[0]->nodetype)
                {
                case en_add:
				case en_addstruc:
                case en_array:
                    if (isoptconst(node->v.p[0]->v.p[0]))
                    {
                        node->nodetype = en_add;
                        node->v.p[0]->nodetype = en_sub;
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[0]->v.p[1]))
                    {
                        node->nodetype = en_add;
                        node->v.p[0]->nodetype = en_sub;
                        enswap(&node->v.p[0]->v.p[0], &node->v.p[0]->v.p[1]);
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->v.p[0]->v.p[0]))
                    {
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[0]->v.p[1]))
                    {
                        enswap(&node->v.p[0], &node->v.p[1]);
                        enswap(&node->v.p[1]->v.p[0], &node->v.p[0]);
                        node->nodetype = en_add;
                        node->v.p[1]->v.p[0] = makenode(en_uminus, node->v.p[1]
                            ->v.p[0], 0);
                        rv = TRUE;
                    }
                    break;
                }
            }
            else if (isoptconst(node->v.p[0]))
            {
                switch (node->v.p[1]->nodetype)
                {
                case en_add:
        		case en_addstruc:
		        case en_array:
                    if (isoptconst(node->v.p[1]->v.p[0]))
                    {
                        node->nodetype = en_sub;
                        node->v.p[1]->nodetype = en_sub;
                        enswap(&node->v.p[0], &node->v.p[1]);
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        enswap(&node->v.p[0]->v.p[0], &node->v.p[0]->v.p[1]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[1]->v.p[1]))
                    {
                        node->nodetype = en_sub;
                        node->v.p[1]->nodetype = en_sub;
                        enswap(&node->v.p[0], &node->v.p[1]);
                        enswap(&node->v.p[0]->v.p[0], &node->v.p[1]);
                        rv = TRUE;
                    }
                    break;
                case en_sub:
                    if (isoptconst(node->v.p[1]->v.p[0]))
                    {
                        enswap(&node->v.p[1]->v.p[1], &node->v.p[0]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[1]->v.p[1]))
                    {

                        enswap(&node->v.p[0], &node->v.p[1]);
                        enswap(&node->v.p[0]->v.p[0], &node->v.p[1]);
                        node->v.p[0]->v.p[1] = makenode(en_uminus, node->v.p[0]
                            ->v.p[1], 0);
                        rv = TRUE;
                    }
                    break;
                }
            }
            break;
        case en_mul:
        case en_umul:
		case en_pmul:
            rv |= fold_const(node->v.p[0]);
            rv |= fold_const(node->v.p[1]);
            if (isoptconst(node->v.p[1]))
            {
                switch (node->v.p[0]->nodetype)
                {
                case en_mul:
                case en_umul:
				case en_pmul:
                    if (isoptconst(node->v.p[0]->v.p[0]))
                    {
                        node->nodetype = en_mul;
                        node->v.p[0]->nodetype = en_mul;
                        enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[0]->v.p[1]))
                    {
                        node->nodetype = en_mul;
                        node->v.p[0]->nodetype = en_mul;
                        enswap(&node->v.p[0]->v.p[0], &node->v.p[1]);
                        rv = TRUE;
                    }
                    break;
                }
            }
            else if (isoptconst(node->v.p[0]))
            {
                switch (node->v.p[1]->nodetype)
                {
                case en_mul:
                case en_umul:
				case en_pmul:
                    if (isoptconst(node->v.p[1]->v.p[0]))
                    {
                        node->nodetype = en_mul;
                        node->v.p[1]->nodetype = en_mul;
                        enswap(&node->v.p[1]->v.p[1], &node->v.p[0]);
                        rv = TRUE;
                    }
                    else if (isoptconst(node->v.p[1]->v.p[1]))
                    {
                        node->nodetype = en_mul;
                        node->v.p[1]->nodetype = en_mul;
                        enswap(&node->v.p[1]->v.p[0], &node->v.p[0]);
                        rv = TRUE;
                    }
                    break;
                }
            }
            break;
        case en_and:
        case en_or:
        case en_xor:
            rv |= fold_const(node->v.p[0]);
            rv |= fold_const(node->v.p[1]);
            if (node->v.p[0]->nodetype == node->nodetype && isoptconst(node
                ->v.p[1]))
            {
                if (isoptconst(node->v.p[0]->v.p[0]))
                {
                    enswap(&node->v.p[0]->v.p[1], &node->v.p[1]);
                    rv = TRUE;
                }
                else if (isoptconst(node->v.p[0]->v.p[1]))
                {
                    enswap(&node->v.p[0]->v.p[0], &node->v.p[1]);
                    rv = TRUE;
                }
            }
            else if (node->v.p[1]->nodetype == node->nodetype && isoptconst
                (node->v.p[0]))
            {
                if (isoptconst(node->v.p[1]->v.p[0]))
                {
                    enswap(&node->v.p[1]->v.p[1], &node->v.p[0]);
                    rv = TRUE;
                }
                else if (isoptconst(node->v.p[1]->v.p[1]))
                {
                    enswap(&node->v.p[1]->v.p[0], &node->v.p[0]);
                    rv = TRUE;
                }
            }
            break;
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
        case en_nacon:
        case en_absacon:
        case en_napccon:
        case en_llcon:
        case en_llucon:
        case en_nalabcon:
		case en_labcon:
            break;
        case en_sp_ref:
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
        case en_cbit:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_ll_ref:
        case en_ull_ref:
        case en_b_ref:
        case en_w_ref:
		case en_a_ref:
		case en_ua_ref:
		case en_i_ref:
		case en_ui_ref:
        case en_ul_ref:
        case en_l_ref:
        case en_ub_ref:
        case en_uw_ref:
        case en_bool_ref:
        case en_uminus:
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
        case en_substack:
		case en_loadstack:
		case en_savestack:
            rv |= fold_const(node->v.p[0]);
            break;
        case en_cond:
            rv |= fold_const(node->v.p[0]);
            rv |= fold_const(node->v.p[1]->v.p[0]);
            rv |= fold_const(node->v.p[1]->v.p[1]);
            break;
        case en_rsh:
        case en_assign:
		case en_asadd:
        case en_assub:
		case en_asmul:
		case en_asdiv:
		case en_asmod:
		case en_asrsh:
		case en_asumul:
		case en_asudiv:
        case en_asumod:
		case en_aslsh:
		case en_asarsh: 
		case en_asarshd:
		case en_asalsh:
		case en_asand:
		case en_asor:
		case en_asxor:
        case en_asuminus:
		case en_ascompl:
        case en_void:
		case en_dvoid:
		case en_voidnz:
        case en_arrayindex:
        case en_div:
		case en_pdiv:
        case en_udiv:
        case en_umod:
        case en_lsh:
		case en_alsh:
        case en_arsh:
        case en_mod:
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
        case en_intcall:
		case en_moveblock:
        case en_stackblock:
        case en_thiscall:
        case en_ainc:
        case en_adec:
		case en_pfcallb:
		case en_pcallblock:
		case en_sfcallb:
		case en_scallblock:
		case en_fcallb:
		case en_callblock:
		case en_pfcall:
		case en_sfcall:
		case en_fcall:
            rv |= fold_const(node->v.p[1]);
        case en_clearblock:
//		case en_argnopush:
//		case en_not_lvalue:
            rv |= fold_const(node->v.p[0]);
            break;
		default:
			break;
    }
    return rv;
}


/*

 * remove type casts from constant nodes and change their size
 */
int typedconsts(ENODE *node1)
{
    int rv = FALSE;
    if (!node1)
        return rv;
    switch (node1->nodetype)
    {
        case en_lcon:
        case en_iucon:
        case en_icon:
        case en_lucon:
        case en_llucon:
        case en_llcon:
        case en_boolcon:
/*            node1->v.i = reint(node1);*/
            break;
        case en_compl:
        case en_not:
        case en_uminus:
//		case en_argnopush:
//		case en_not_lvalue:

            rv |= typedconsts(node1->v.p[0]);
            break;
        case en_cond:
            rv |= typedconsts(node1->v.p[0]);
            rv |= typedconsts(node1->v.p[1]->v.p[0]);
            rv |= typedconsts(node1->v.p[1]->v.p[1]);
            break;
        case en_add:
		case en_addstruc:
        case en_array:
        case en_sub:
        case en_or:
        case en_xor:
        case en_mul:
        case en_arrayindex:
        case en_umul:
		case en_pmul:
        case en_lsh:
		case en_alsh:
        case en_rsh:
        case en_arsh:
        case en_div:
		case en_pdiv:
        case en_mod:
        case en_land:
        case en_lor:
        case en_and:
        case en_void:
		case en_dvoid:
        case en_voidnz:
        case en_assign:
		case en_asadd:
        case en_assub:
		case en_asmul:
		case en_asdiv:
		case en_asmod:
		case en_asrsh:
		case en_asumul:
		case en_asudiv:
        case en_asumod:
		case en_aslsh:
		case en_asarsh: 
		case en_asarshd:
		case en_asalsh:
		case en_asand:
		case en_asor:
		case en_asxor:
        case en_asuminus:
		case en_ascompl:
		case en_moveblock:
        case en_stackblock:
        case en_intcall:
        case en_udiv:
		case en_umod:
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
        case en_thiscall:
		case en_pfcallb:
		case en_pcallblock:
		case en_sfcallb:
		case en_scallblock:
		case en_fcallb:
		case en_callblock:
		case en_pfcall:
		case en_sfcall:
		case en_fcall:
            rv |= typedconsts(node1->v.p[1]);
        case en_trapcall:
        case en_cl_reg:
        case en_substack:
		case en_loadstack:
		case en_savestack:
        case en_clearblock:
            rv |= typedconsts(node1->v.p[0]);
            break;
        case en_bool_ref:
        case en_b_ref:
        case en_w_ref:
        case en_sp_ref:
        case en_fp_ref:
        case en_ub_ref:
        case en_uw_ref:
		case en_a_ref:
		case en_ua_ref:
		case en_i_ref:
		case en_ui_ref:
        case en_l_ref:
        case en_ul_ref:
        case en_floatref:
        case en_doubleref:
        case en_longdoubleref:
        case en_fimaginaryref:
        case en_rimaginarycon:
        case en_lrimaginaryref:
        case en_fcomplexref:
        case en_rcomplexref:
        case en_lrcomplexref:
        case en_ull_ref:
        case en_ll_ref:
            rv |= typedconsts(node1->v.p[0]);
            break;
        case en_cfp:
        case en_csp:
            break;
        case en_cp:
            rv |= typedconsts(node1->v.p[0]);
			if (node1->v.p[0] && isintconst(node1->v.p[0]->nodetype))
			{
				*node1 = *node1->v.p[0];
                rv = TRUE;
			}
            break;
        case en_cull:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_ULONGLONG,reint(node1->v.p[0]));
                node1->nodetype = en_llucon;
            }
            break;
        case en_cll:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_ULONGLONG,reint(node1->v.p[0]));
                node1->nodetype = en_llcon;
            }
            break;
        case en_cbit:
        case en_cbool:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
				if (isfloatconst(node1->v.p[0]->nodetype) || isimaginaryconst(node1->v.p[0]->nodetype))
					node1->v.i = node1->v.p[0]->v.f.type != IFPF_IS_ZERO;
				else if (iscomplexconst(node1->v.p[0]->nodetype))
					node1->v.i = node1->v.p[0]->v.c.r.type != IFPF_IS_ZERO
							 || node1->v.p[0]->v.c.i.type != IFPF_IS_ZERO;
				else
	                node1->v.i = CastToInt(ISZ_BOOL,!!reint(node1->v.p[0]));
                node1->nodetype = en_boolcon;
            }
            break;
        case en_cb:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_UCHAR,reint(node1->v.p[0]));
                node1->nodetype = en_icon;
            }
            break;
        case en_cub:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_UCHAR,reint(node1->v.p[0]));
                node1->nodetype = en_icon;
            }
            break;
        case en_cw:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_USHORT,reint(node1->v.p[0]));
                node1->nodetype = en_icon;
            }
            break;
        case en_cuw:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_USHORT,reint(node1->v.p[0]));
                node1->nodetype = en_lcon;
            }
            break;
        case en_ci:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_UINT,reint(node1->v.p[0]));
                node1->nodetype = en_icon;
            }
            break;
        case en_cui:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_UINT,reint(node1->v.p[0]));
                node1->nodetype = en_lucon;
            }
            break;
        case en_cl:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(-ISZ_ULONG,reint(node1->v.p[0]));
                node1->nodetype = en_icon;
            }
            break;
        case en_cul:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
                rv = TRUE;
                node1->v.i = CastToInt(ISZ_ULONG,reint(node1->v.p[0]));
                node1->nodetype = en_lucon;
            }
            break;
/*#ifdef XXXXX */
        case en_cf:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
				FPF temp = refloat(node1->v.p[0]);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_FLOAT, &temp);
                node1->nodetype = en_fcon;
            }
            break;
        case en_cfi:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
				FPF temp = refloat(node1->v.p[0]);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_IFLOAT, &temp);
                node1->nodetype = en_fimaginarycon;
            }
            break;
        case en_cd:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
				FPF temp = refloat(node1->v.p[0]);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_DOUBLE, &temp);
                node1->nodetype = en_rcon;
            }
            break;
        case en_cri:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
				FPF temp = refloat(node1->v.p[0]);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_IDOUBLE, &temp);
                node1->nodetype = en_rimaginarycon;
            }
            break;
        case en_cld:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
				FPF temp = refloat(node1->v.p[0]);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_LDOUBLE, &temp);
                node1->nodetype = en_lrcon;
            }
            break;
        case en_clri:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
            {
				FPF temp = refloat(node1->v.p[0]);
                rv = TRUE;
                node1->v.f = CastToFloat(ISZ_ILDOUBLE, &temp);
                node1->nodetype = en_lrimaginarycon;
            }
            break;
        case en_cfc:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
			{
				rv = TRUE;
				if (isintconst(node1->v.p[0]->nodetype) || isfloatconst(node1->v.p[0]->nodetype))
				{
					FPF temp = refloat(node1->v.p[0]);
	                node1->v.c.r = CastToFloat(ISZ_ILDOUBLE, &temp);
					SetFPFZero(&node1->v.c.i, 0);
				}
				else if (isimaginaryconst(node1->v.p[0]->nodetype))
				{
					FPF temp ;
					node1->v.p[0]->nodetype = en_lrcon;
					temp = refloat(node1->v.p[0]);
	                node1->v.c.i = CastToFloat(ISZ_ILDOUBLE, &temp);
					SetFPFZero(&node1->v.c.r, 0);
				}
				else
				{
					node1->v.c = node1->v.p[0]->v.c;
				}
				node1->nodetype = en_fcomplexcon;					
			}
			break;
        case en_crc:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
			{
				rv = TRUE;
				if (isintconst(node1->v.p[0]->nodetype) || isfloatconst(node1->v.p[0]->nodetype))
				{
					FPF temp = refloat(node1->v.p[0]);
	                node1->v.c.r = CastToFloat(ISZ_ILDOUBLE, &temp);
					SetFPFZero(&node1->v.c.i, 0);
				}
				else if (isimaginaryconst(node1->v.p[0]->nodetype))
				{
					FPF temp ;
					node1->v.p[0]->nodetype = en_lrcon;
					temp = refloat(node1->v.p[0]);
	                node1->v.c.i = CastToFloat(ISZ_ILDOUBLE, &temp);
					SetFPFZero(&node1->v.c.r, 0);
				}
				else
				{
					node1->v.c = node1->v.p[0]->v.c;
				}
				node1->nodetype = en_rcomplexcon;
			}
			break;
        case en_clrc:
            rv |= typedconsts(node1->v.p[0]);
            if (node1->v.p[0] && isoptconst(node1->v.p[0]))
			{
				rv = TRUE;
				if (isintconst(node1->v.p[0]->nodetype) || isfloatconst(node1->v.p[0]->nodetype))
				{
					FPF temp = refloat(node1->v.p[0]);
	                node1->v.c.r = CastToFloat(ISZ_ILDOUBLE, &temp);
					SetFPFZero(&node1->v.c.i, 0);
				}
				else if (isimaginaryconst(node1->v.p[0]->nodetype))
				{
					FPF temp ;
					node1->v.p[0]->nodetype = en_lrcon;
					temp = refloat(node1->v.p[0]);
	                node1->v.c.i = CastToFloat(ISZ_ILDOUBLE, &temp);
					SetFPFZero(&node1->v.c.r, 0);
				}
				else
				{
					node1->v.c = node1->v.p[0]->v.c;
				}
				node1->nodetype = en_lrcomplexcon;
			}
			break;
/*#endif */
    }
    return rv;
}
static int depth(ENODE *ep)
{
	if (ep == 0)
		return 0;
   switch (ep->nodetype)
    {
        case en_nalabcon:
        case en_labcon:
        case en_autocon:
        case en_nacon:
        case en_napccon:
        case en_autoreg:
        case en_absacon:
        case en_icon:
        case en_lcon:
        case en_lucon:
        case en_iucon:
        case en_ccon:
        case en_boolcon:
        case en_cucon:
        case en_llcon:
        case en_llucon:
        case en_rcon:
        case en_fcon:
        case en_lrcon:
        case en_rimaginarycon:
        case en_fimaginarycon:
        case en_lrimaginarycon:
        case en_rcomplexcon:
        case en_fcomplexcon:
        case en_lrcomplexcon:
        case en_tempref:
		case en_regref:
			return 1;
		case en_cond:
			return 1 + imax(depth(ep->v.p[0]), imax(depth(ep->v.p[1]->v.p[0]), depth(ep->v.p[1]->v.p[1])));
		default:
			return 1 + imax(depth(ep->v.p[0]), depth(ep->v.p[1]));
    }
}
static void rebalance(ENODE *ep)
{
    if (ep == 0)
        return ;
    switch (ep->nodetype)
    {
        case en_nalabcon:
        case en_labcon:
        case en_autocon:
        case en_nacon:
        case en_napccon:
        case en_autoreg:
        case en_absacon:
        case en_icon:
        case en_lcon:
        case en_lucon:
        case en_iucon:
        case en_ccon:
        case en_boolcon:
        case en_cucon:
        case en_llcon:
        case en_llucon:
        case en_rcon:
        case en_fcon:
        case en_lrcon:
        case en_rimaginarycon:
        case en_fimaginarycon:
        case en_lrimaginarycon:
        case en_rcomplexcon:
        case en_fcomplexcon:
        case en_lrcomplexcon:
		case en_regref:
			break;
		case en_addstruc:
        case en_add:
		case en_pmul:
        case en_mul:
        case en_umul:
		case en_and:
		case en_or:
		case en_xor:
        case en_eq:
        case en_ne:
		    rebalance(ep->v.p[0]);
    		rebalance(ep->v.p[1]);
			if (depth(ep->v.p[0]) < depth(ep->v.p[1]))
			{
				ENODE *p = ep->v.p[0];
				ep->v.p[0] = ep->v.p[1];
				ep->v.p[1] = p;
			}
			break;
        case en_sub:
		    rebalance(ep->v.p[0]);
    		rebalance(ep->v.p[1]);
			if (depth(ep->v.p[0]) < depth(ep->v.p[1]))
			{
				ENODE *p = ep->v.p[0];
				ep->v.p[0] = ep->v.p[1];
				ep->v.p[1] = p;
				ep->nodetype = en_add;
				ep->v.p[0] = makenode(en_uminus, ep->v.p[0], 0);
			}
			break;
		case en_cond:
		    rebalance(ep->v.p[0]);
    		rebalance(ep->v.p[1]->v.p[0]);
    		rebalance(ep->v.p[1]->v.p[1]);
			break;
		default:
		    rebalance(ep->v.p[0]);
    		rebalance(ep->v.p[1]);
			break;
    }
}
void opt4(ENODE **expr, int globvar)
{
    int rv = TRUE, count = 8;
    globstatvar = globvar;
    asidehead = 0;
    asidetail = &asidehead;
    while (rv && count--)
        rv = typedconsts(*expr) | fold_const(*expr) | opt0(expr);
    if (asidehead)
    {
        *asidetail =  *expr;
        *expr = asidehead;
    }
	rebalance(*expr);
}
