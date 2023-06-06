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

#include "UTF8.h"
#include <ctype.h>
#include <string.h>
int UTF8::Decode(const char* str)
{
    if ((*str & 0x80) == 0)
    {
        return *str;
    }
    else if ((*str & 0xe0) == 0xc0)
    {
        if ((str[1] & 0xc0) == 0x80)
        {
            int rv;
            rv = ((str[0] & 0x1f) << 6) + (str[1] & 0x3f);
            return rv;
        }
    }
    else if ((*str & 0xf0) == 0xe0)
    {
        if ((str[1] & 0xc0) == 0x80 && (str[2] & 0xc0) == 0x80)
        {
            int rv;
            rv = ((str[0] & 0xf) << 12) + ((str[1] & 0x3f) << 6) + (str[2] & 0x3f);
            return rv;
        }
    }
    else if ((*str & 0xf8) == 0xf0)
    {
        if ((str[1] & 0xc0) == 0x80 && (str[2] & 0xc0) == 0x80 && (str[3] & 0xc0) == 0x80)
        {
            int rv;
            rv = ((str[0] & 0x7) << 18) + ((str[1] & 0x3f) << 12) + ((str[2] & 0x3f) << 6) + (str[3] & 0x3f);
            return rv;
        }
    }
    return 0;
}
int UTF8::Encode(char* dest, int val)
{
    if (val < 0x80)
    {
        dest[0] = val;
        return 1;
    }
    else if (val < (1 << (5 + 6)))
    {
        dest[0] = (val >> 6) | 0xc0;
        dest[1] = (val & 0x3f) | 0x80;
        return 2;
    }
    else if (val < (1 << (4 + 6 + 6)))
    {
        dest[0] = (val >> 12) | 0xe0;
        dest[1] = ((val >> 6) & 0x3f) | 0x80;
        dest[2] = (val & 0x3f) | 0x80;
        return 3;
    }
    else
    {
        dest[0] = (val >> 18) | 0xf0;
        dest[1] = ((val >> 12) & 0x3f) | 0x80;
        dest[2] = ((val >> 6) & 0x3f) | 0x80;
        dest[3] = (val & 0x3f) | 0x80;
        return 4;
    }
}
int UTF8::CharSpan(const char* str)
{
    if ((*str & 0x80) == 0)
    {
        return 1;
    }
    else if ((*str & 0xe0) == 0xc0)
    {
        return 2;
    }
    else if ((*str & 0xf0) == 0xe0)
    {
        return 3;
    }
    else if ((*str & 0xf8) == 0xf0)
    {
        return 4;
    }
    else
    {
        return 1;
    }
}
int UTF8::Span(const char* str)
{
    int rv = 0;
    while (*str)
    {
        rv++;
        int n = CharSpan(str);
        for (int i = 0; i < n; i++)
        {
            if (!*++str)
                break;
        }
    }
    return rv;
}
std::string UTF8::ToUpper(const std::string& val)
{
    std::string rv;
    const char* str = val.c_str();
    char buf[10];
    for (int i = 0; i < val.size();)
    {
        if ((str[i] & 0x80) == 0)
        {
            buf[0] = ToUpper(str[i]);
            buf[1] = 0;
            rv += buf;
            i++;
        }
        else
        {
            int spn = CharSpan(str + i);
            int v = Decode(str + i);
            int q = ToUpper(v);
            if (q == v)
            {
                memcpy(buf, str + i, spn);
                buf[spn] = 0;
            }
            else
            {
                buf[Encode(buf, q)] = 0;
            }
            rv += buf;
            i += spn;
        }
    }
    return rv;
}
