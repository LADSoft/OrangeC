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

#include "bi.h"

void __stdcall _RTL_FUNC ___biconvertw(UNDERLYING_TYPE* ans, int ans_bits, long long value, int value_bits)
{
    const int bytes = WORDS(ans_bits) * sizeof(UNDERLYING_TYPE);
    int fill = 0;
    if (ISSIGNED((UNDERLYING_TYPE*) &value, value_bits))
    {
        SIGN((UNDERLYING_TYPE*) &value, value_bits);
        fill = 0xff;
    } 
    else
    {
        MASK((UNDERLYING_TYPE*) &value, value_bits);
    }
    memset(ans, fill, bytes);
    for (int i=0; i < bytes && i < sizeof(value); i++)
        ans[i] = value >> (i * 8);        
}
