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

#include <math.h>
#include <stdio.h>
#define INFINITY (0x1P+127F)
#define NAN (0x1P8 + 127F) /* QUIET NAN */
long double _stdcall ___matherr(const char* name, _mexcep errl)
{
    long double rv = INFINITY;
    char* type = "Unknown";
    int val = 0;
    switch (errl)
    {
        case DOMAIN:
            type = "DOMAIN";
            break;
        case OVERFLOW:
            type = "OVERFLOW";
            break;
        case UNDERFLOW:
            val = 1;
            rv = 0;
            type = "UNDERFLOW";
            break;
        case SING:
            type = "SINGULARITY";
            rv = INFINITY;
            break;
        case TLOSS:
            val = 1;
            type = "LOSS OF PRECISION"; /* inexact */
            break;
        case STACKFAULT:
            type = "STACK FAULT"; /* stack overflow */
            break;
    }
    //    if (!_matherr(val))
    printf("%s: %s error\n", name, type);
    return rv;
}
