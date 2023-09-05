#pragma once
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
#include <stdlib.h>
#include <limits.h>
#define UNDERLYING_TYPE unsigned
#define UTBITS (sizeof(UNDERLYING_TYPE) * CHAR_BIT)
#define WORDS(x)  (( abs(x) + UTBITS -1)/ UTBITS)
inline void MASK(UNDERLYING_TYPE* arr, int bits)
{
if (bits %UTBITS == 0)
    return;
const int words = WORDS(bits);
const int mask = (1 << (abs(bits) % UTBITS))-1;
if (words)
    arr[words-1] &= mask;
}
inline void SIGN(UNDERLYING_TYPE* arr, int bits)
{
if (bits %UTBITS == 0)
    return;
const int words = WORDS(bits);
const int sign = ~((1 << (abs(bits) % UTBITS))-1);
if (words)
    arr[words-1] |= sign;
}
inline int ISSIGNED(UNDERLYING_TYPE* arr, int bits)
{
if (bits < 0)
{
    bits = abs(bits);
    const int words = (bits-1) / UTBITS;
    const int bit = (1 << (bits-1 % UTBITS));
    return (arr[words] & bit);
}
return 0;
}