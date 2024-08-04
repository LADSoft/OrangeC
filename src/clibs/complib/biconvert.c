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

long long __stdcall _RTL_FUNC ___biconvert(UNDERLYING_TYPE* left, int left_bits, int value_bits)
{
    const int bytes = WORDS(value_bits) * sizeof(UNDERLYING_TYPE);
    const int bytes2 = WORDS(left_bits) * sizeof(UNDERLYING_TYPE);
    long long rv = 0;
    for (int i=0; i < bytes && i < bytes2; i++)
        rv |= (long long)(((unsigned char *)left)[i]) << (i * 8);
    if (ISSIGNED((UNDERLYING_TYPE*)&rv, value_bits))
        SIGN((UNDERLYING_TYPE*)&rv, value_bits);
    else
        MASK((UNDERLYING_TYPE*)&rv, value_bits);
    return rv;
}
