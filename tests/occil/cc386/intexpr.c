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
 * Evaluate an expression which should be known at compile time.
 * This uses recursive descent.  It is roughly analogous to the
 * primary expression handler except it returns a value rather than
 * an enode list
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"


//-------------------------------------------------------------------------

LLONG_TYPE intexpr(TYP **tp)
/* Integer expressions */
{
    TYP *tpx;
    ENODE *en=0;
    LLONG_TYPE val =0;
    tpx = exprnc(&en);
    opt4(&en, TRUE);
    if (en && isintconst(en->nodetype))
    {
        val = en->v.i ;
    }
    else
        generror(ERR_INVCONST,0,0);
    if (tp)
        *tp = tpx;
    return val;
}
