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

#include <stdlib.h>

char* _RTL_FUNC _ui64toa(unsigned __int64 __value, char* __stringValue, int __radix)
{
    char buf2[68];
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
#if __STDC_VERSION__ >= 199901L
char* _RTL_FUNC _ulltoa(unsigned long long __value, char* __stringValue, int __radix)
{
    return _ui64toa((unsigned __int64)__value, __stringValue, __radix);
}
char* _RTL_FUNC ulltoa(unsigned long long __value, char* __stringValue, int __radix)
{
    return _ui64toa((unsigned __int64)__value, __stringValue, __radix);
}
#endif
