/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

double complex _stdcall __CPLXMULLIM(long double complex z, long double complex w)
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
    return x + I * y;
}