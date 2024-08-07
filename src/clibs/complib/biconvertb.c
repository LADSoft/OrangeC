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

void __stdcall _RTL_FUNC ___biconvertb(UNDERLYING_TYPE* ans, int ans_bits, UNDERLYING_TYPE* left, int left_bits)
{
    const int words = WORDS(ans_bits);
    if (abs(left_bits) < abs(ans_bits))
    {
        const int lwords = WORDS(left_bits);
        int fill = ISSIGNED(left, left_bits)  ? 0xff : 0;
        memset(ans+lwords, fill, (words - lwords) * sizeof(UNDERLYING_TYPE));
        if (fill)
            SIGN(left, left_bits);
        else
            MASK(left, left_bits);
        memcpy(ans, left, lwords * sizeof(UNDERLYING_TYPE));
    }
    else
    {
        memcpy(ans, left, words * sizeof(UNDERLYING_TYPE));
    }
}
