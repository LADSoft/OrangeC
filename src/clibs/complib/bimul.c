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

#include "bi.h"

extern void __mmul(unsigned a, unsigned b, unsigned* high, unsigned* low);
void __stdcall _RTL_FUNC ___bimul(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, UNDERLYING_TYPE* right, int bits)
{
    const int words = WORDS(bits);
    unsigned high, low;
    ans[0] = 0;
    int i;
    
    memset(ans, 0, words * sizeof(UNDERLYING_TYPE));
    for (i=0; i < words; i++)
    {
        int j;
        for (j=0; j < words-i-1; j++)
        {
            __mmul(left[i], right[j], &high, &low);
              
            long long x = (long long)ans[i+j] + low;
            ans[i+j] = (unsigned)x;
            ans[i+1+j] += high + (unsigned)(x >> 32);
        }
        __mmul(left[i], right[j], &high, &low);
        ans[i+j] += low;
    }
}

void __stdcall _RTL_FUNC ___biumul(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, UNDERLYING_TYPE* right, int bits)
{
    ___bimul(ans, left, right, bits);
}