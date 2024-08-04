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

#include <stddef.h>
#include <limits.h>

template <class T>
static inline unsigned int leading_ones(T arg)
{
    unsigned rv = 0;
    for (T val = ((T)1) << sizeof(arg) * CHAR_BIT - 1; val && (val & arg); rv++, val >>= 1);
    return rv;
}

extern "C"
{

unsigned int _RTL_FUNC stdc_leading_ones_uc(unsigned char value) { return leading_ones(value); }
unsigned int _RTL_FUNC stdc_leading_ones_us(unsigned short value) { return leading_ones(value); }
unsigned int _RTL_FUNC stdc_leading_ones_ui(unsigned int value) { return leading_ones(value); }
unsigned int _RTL_FUNC stdc_leading_ones_ul(unsigned long value) { return leading_ones(value); }
unsigned int _RTL_FUNC stdc_leading_ones_ull(unsigned long long value) { return leading_ones(value); }

}