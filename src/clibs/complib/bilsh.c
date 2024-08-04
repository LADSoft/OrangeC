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

int __stdcall _RTL_FUNC ___bilsh(UNDERLYING_TYPE* ans, UNDERLYING_TYPE* left, int bits, int shift)
{
    int rv = 0;
    const int words = WORDS(bits);
    MASK(left, bits);
    if (shift >= bits)
    {
        memset(ans, 0, words * sizeof(UNDERLYING_TYPE));
    }
    else
    {
        const int ofswords = shift/(sizeof(UNDERLYING_TYPE) * CHAR_BIT);
        const int ofsbits = shift %(sizeof(UNDERLYING_TYPE) * CHAR_BIT);
        const int mibits = sizeof(UNDERLYING_TYPE) * CHAR_BIT - ofsbits;
        if (ofsbits == 0)
        {
            if (ofswords != 0)
                rv = left[words - ofswords];            
            for (int i = words-1; i >= 0; i--)
            {
                int n = i - ofswords;
                if (n >= 0)
                {
                    ans[i] = left[n];       
                }
                else
                {
                    ans[i] = 0;
                }
            }
        }
        else
        {
            const int rvbits = ((unsigned)shift - (unsigned)abs(bits)) %(sizeof(UNDERLYING_TYPE) * CHAR_BIT);
            rv = left[words - ofswords - 1] >> (UTBITS - rvbits);
            if (ofswords != 0)
                rv |= left[words- ofswords] << rvbits;
            for (int i = words-1; i >= 0; i--)
            {
                int n = i - ofswords;
                if (n >= 0)
                {
                    ans[i] = left[n] << ofsbits;
                    if (n >= 1)
                         ans[i] |= left[n-1] >> mibits;
                }
                else
                {
                    ans[i] = 0;
                }
            }
        }
    }
    return rv;
}
