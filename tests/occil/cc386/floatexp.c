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
 * this evaluates an expression that may have floating point values in it
 * e.g. the initializer for a module-scoped float or double
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"

extern LLONG_TYPE ival;
extern FPF rval;
extern enum e_sym lastst;
extern char lastid[];
extern TABLE defsyms;
extern int prm_cmangle;
extern SYM *typequal;

FPF floatexpr(void);

/* Primary for constant floats
 *   id
 *   iconst
 *   rconst
 *   defined(MACRO)
 *   (cast) floatexpr
 */
static FPF feprimary(void)
{
    FPF temp;
    SYM *sp;
	temp.type = IFPF_IS_ZERO;
    if (lastst == id)
    {
        char *lid = lastid;
        if (prm_cmangle)
            lid++;
        sp = gsearch(lastid);
        if (sp == NULL)
        {
            gensymerror(ERR_UNDEFINED, lastid);
            getsym();
            return temp;
        } if (sp->storage_class != sc_const && sp->tp->type != bt_enum)
        {
            generror(ERR_NEEDCONST, 0, 0);
            getsym();
            return temp;
        }
        getsym();
		LongLongToFPF(&temp, sp->value.i);
        return temp;
    }
    else if (lastst == iconst)
    {
		LongLongToFPF(&temp, ival);
        getsym();
        return temp;
    }
    else if (lastst == lconst)
    {
		LongLongToFPF(&temp, ival);
        getsym();
        return temp;
    }
    else if (lastst == iuconst)
    {
		UnsignedLongLongToFPF(&temp, ival);
        getsym();
        return temp;
    }
    else if (lastst == luconst)
    {
		UnsignedLongLongToFPF(&temp, ival);
        getsym();
        return temp;
    }
    else if (lastst == rconst || lastst == lrconst || lastst == fconst)
    {
        temp = rval;
        getsym();
        return temp;
    }
    else if (lastst == openpa)
    {
        getsym();
        if (castbegin(lastst))
        {
            typequal = 0;
            decl(0, 0);
            decl1(sc_type, 0);
            needpunc(closepa, 0);
            return floatexpr();
        }
        else
        {
            temp = floatexpr();
            needpunc(closepa, 0);
            return (temp);
        }
    }
    getsym();
    generror(ERR_NEEDCONST, 0, 0);
    return temp;
}

/* Unary for floating const
 *    -unary
 *    !unary
 *    primary
 */
static FPF feunary(void)
{
    FPF temp;
    switch (lastst)
    {
        case minus:
            getsym();
            temp =  feunary();
			temp.sign ^= 1;
            break;
        case en_not:
            getsym();
            temp = feunary();
			if (temp.type == IFPF_IS_ZERO)
				LongLongToFPF(&temp, 1);
			else
				LongLongToFPF(&temp, 0);
            break;
        case plus:
            getsym();
            temp = feunary();
            break;
        default:
            temp = feprimary();
            break;
    }
    return (temp);
}

/* Multiply ops */
static FPF femultops(void)
{
    FPF val1 = feunary(), val2;
    while (lastst == star || lastst == divide)
    {
        int oper = lastst;
        getsym();
        val2 = feunary();
        switch (oper)
        {
			FPF val3;
            case star:
				MultiplyFPF(&val1, &val2, &val3);
				val1 = val3;
                break;
            case divide:
				DivideFPF(&val1, &val2, &val3);
				val1 = val3;
                break;
        }
    }
    return (val1);
}

/* Add ops */
static FPF feaddops(void)
{
    FPF val1 = femultops(), val2;
    while (lastst == plus || lastst == minus)
    {
        int oper = lastst;
		FPF val3;
        getsym();
        val2 = femultops();
        if (oper == plus)
		{
			AddSubFPF(0, &val1, &val2, &val3);
            val1 = val3;
		}
        else
		{
			AddSubFPF(1, &val1, &val2, &val3);
            val1 = val3;
		}
    }
    return (val1);
}

/* Floating point constant expression */
FPF floatexpr(void)
{
    return (feaddops());
}
