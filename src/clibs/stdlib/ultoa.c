/* Software License Agreement
 * 
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdlib.h>

char* _RTL_FUNC ultoa(unsigned long __value, char* __stringValue, int __radix)
{
    char buf2[36];
    int len = 0, pos = 0;
    if (__radix < 2 || __radix > 36)
        __stringValue[pos] = 0;
    else if (!__value)
    {
        __stringValue[pos++] = '0';
        __stringValue[pos] = 0;
    }
    else
    {
        while (__value)
        {
            buf2[len++] = __value % __radix;
            __value /= __radix;
        }
        while (len)
        {
            int ch = buf2[--len];
            ch += '0';
            if (ch > '9')
                ch += 7 + 'a' - 'A';
            __stringValue[pos++] = ch;
        }
        __stringValue[pos] = 0;
    }
    return __stringValue;
}
char* _RTL_FUNC _ultoa(unsigned long __value, char* __stringValue, int __radix) { return ultoa(__value, __stringValue, __radix); }
char* _RTL_FUNC _utoa(unsigned __value, char* __stringValue, int __radix)
{
    return ultoa((unsigned long)__value, __stringValue, __radix);
}
char* _RTL_FUNC utoa(unsigned __value, char* __stringValue, int __radix)
{
    return ultoa((unsigned long)__value, __stringValue, __radix);
}
