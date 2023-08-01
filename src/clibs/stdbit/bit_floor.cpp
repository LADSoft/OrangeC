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

#include <stddef.h>
#include <limits.h>

template <class T>
static inline T bit_floor(T arg)
{
    if (!arg)
        return 0;
    T val;
    for (val = ((T)1) << sizeof(arg) * CHAR_BIT - 1; val && !(val & arg); val >>= 1);
    return val;
}

extern "C"
{

unsigned char _RTL_FUNC stdc_bit_floor_uc(unsigned char value) { return bit_floor(value); }
unsigned short _RTL_FUNC stdc_bit_floor_us(unsigned short value) { return bit_floor(value); }
unsigned int _RTL_FUNC stdc_bit_floor_ui(unsigned int value) { return bit_floor(value); }
unsigned long _RTL_FUNC stdc_bit_floor_ul(unsigned long value) { return bit_floor(value); }
unsigned long long _RTL_FUNC stdc_bit_floor_ull(unsigned long long value) { return bit_floor(value); }

}