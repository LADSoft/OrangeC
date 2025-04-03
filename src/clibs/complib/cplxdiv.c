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
/* Divide z / w ... */
long double complex _stdcall __CPLXDIV(long double complex z, long double complex w)
{
#pragma STDC FP_CONTRACT OFF
    long double a, b, c, d, logbw, denom, x, y;
    int ilogbw = 0;
    a = creall(z);
    b = cimagl(z);
    c = creall(w);
    d = cimagl(w);
    logbw = logbl(fmaxl(fabsl(c), fabsl(d)));
    if (isfinite(logbw))
    {
        ilogbw = (int)logbw;
        c = scalbnl(c, -ilogbw);
        d = scalbnl(d, -ilogbw);
    }
    denom = c * c + d * d;
    x = scalbnl((a * c + b * d) / denom, -ilogbw);
    y = scalbnl((b * c - a * d) / denom, -ilogbw);
    /*
     * Recover infinities and zeros that computed
     * as NaN+iNaN; the only cases are non-zero/zero,
     * infinite/finite, and finite/infinite, ...
     */
    if (isnan(x) && isnan(y))
    {
        if ((denom == 0.0) && (!isnan(a) || !isnan(b)))
        {
            x = copysignl(INFINITY, c) * a;
            y = copysignl(INFINITY, c) * b;
        }
        else if ((isinf(a) || isinf(b)) && isfinite(c) && isfinite(d))
        {
            a = copysignl(isinf(a) ? 1.0 : 0.0, a);
            b = copysignl(isinf(b) ? 1.0 : 0.0, b);
            x = INFINITY * (a * c + b * d);
            y = INFINITY * (b * c - a * d);
        }
        else if (isinf(logbw) && isfinite(a) && isfinite(b))
        {
            c = copysignl(isinf(c) ? 1.0 : 0.0, c);
            d = copysignl(isinf(d) ? 1.0 : 0.0, d);
            x = 0.0 * (a * c + b * d);
            y = 0.0 * (b * c - a * d);
        }
    }
    return x + I * y;
}
