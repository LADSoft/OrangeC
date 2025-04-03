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
#include <wchar.h>

wchar_t* _RTL_FUNC ultow(unsigned long __value, wchar_t* __stringValue, int __radix)
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
                ch += 7;
            __stringValue[pos++] = ch;
        }
        __stringValue[pos] = 0;
    }
    return __stringValue;
}
wchar_t* _RTL_FUNC ltow(long __value, wchar_t* __stringValue, int __radix)
{
    char buf2[36];
    int len = 0, pos = 0;
    if (__value < 0)
    {
        __stringValue[pos++] = '-';
        __value = -__value;
    }
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
                ch += 7;
            __stringValue[pos++] = ch;
        }
        __stringValue[pos] = 0;
    }
    return __stringValue;
}
wchar_t* _RTL_FUNC itow(int __value, wchar_t* __stringValue, int __radix) { return ltow(__value, __stringValue, __radix); }
wchar_t* _RTL_FUNC _itow(int __value, wchar_t* __stringValue, int __radix) { return ltow(__value, __stringValue, __radix); }
wchar_t* _RTL_FUNC _ultow(unsigned long __value, wchar_t* __stringValue, int __radix) { return ultow(__value, __stringValue, __radix); }
wchar_t* _RTL_FUNC _ltow(long __value, wchar_t* __stringValue, int __radix) { return ltow(__value, __stringValue, __radix); }
