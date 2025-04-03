/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

/*
    Copied from the C99 standard
*/
#include <math.h>
#include <complex.h>

double complex _stdcall __CPLXMUL(long double complex z, long double complex w)
{
#pragma STDC FP_CONTRACT OFF
    long double a, b, c, d, ac, bd, ad, bc, x, y;
    a = creall(z);
    b = cimagl(z);
    c = creall(w);
    d = cimagl(w);
    ac = a * c;
    bd = b * d;
    ad = a * d;
    bc = b * c;
    x = ac - bd;
    y = ad + bc;
    /* Risinfecover infinities that computed as NaN+iNaN ... */
    if (isnan(x) && isnan(y))
    {
        int recalc = 0;
        if (isinf(a) || isinf(b))
        { /* z is infinite */
            /* "Box" the infinity ... */
            a = copysignl(isinf(a) ? 1.0 : 0.0, a);
            b = copysignl(isinf(b) ? 1.0 : 0.0, b);
            /* Change NaNs in the other factor to 0 ... */
            if (isnan(c))
                c = copysignl(0.0, c);
            if (isnan(d))
                d = copysignl(0.0, d);
            recalc = 1;
        }
        if (isinf(c) || isinf(d))
        { /* w is infinite */
            /* "Box" the infinity ... */
            c = copysignl(isinf(c) ? 1.0 : 0.0, c);
            d = copysignl(isinf(d) ? 1.0 : 0.0, d);
            /* Change NaNs in the other factor to 0 ... */
            if (isnan(a))
                a = copysignl(0.0, a);
            if (isnan(b))
                b = copysignl(0.0, b);
            recalc = 1;
        }
        if (!recalc)
        {
            /* *Recover infinities from overflow cases ... */
            if (isinf(ac) || isinf(bd) || isinf(ad) || isinf(bc))
            {
                /* Change all NaNs to 0 ... */
                if (isnan(a))
                    a = copysignl(0.0, a);
                if (isnan(b))
                    b = copysignl(0.0, b);
                if (isnan(c))
                    c = copysignl(0.0, c);
                if (isnan(d))
                    d = copysignl(0.0, d);
                recalc = 1;
            }
        }
        if (recalc)
        {
            x = INFINITY * (a * c - b * d);
            y = INFINITY * (a * d + b * c);
        }
    }
    return x + I * y;
}