/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */
#include "bi.h"

int __stdcall _RTL_FUNC ___bicompareu(UNDERLYING_TYPE* left, UNDERLYING_TYPE* right, int bits)
{
    const int words = WORDS(bits);
    if (words == 0)
        return 0;
    int mask = abs(bits) % UTBITS;
    if (mask == 0) mask = -1;
    else mask = (1 << mask) -1;
    if ((left[words-1] & mask) == (right[words-1] & mask))
    {
        int rv = 0;
        for (int i = words-2; i >=0 && !rv; i--)
        {
            if (left[i] != right[i])
                if (left[i] < right[i]) rv = -1;
                else rv = 1;
        }
        return rv;
    }
    else
    {
        // relies on UNDERLYING_TYPE being unsigned 
        return (left[words-1] & mask) < (right[words-1] & mask) ? -1 : 1;
    }
}
