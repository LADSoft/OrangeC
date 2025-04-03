/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#define _CRT_SECURE_NO_WARNINGS

#include "ObjUtil.h"
#include <iomanip>
#include <cstdio>
#include <cstring>
/*
 * C++ stream objects are not used here, because these functions are called
 * a lot and the resultant memory allocations fragment memory to such an extent
 * that it really slows down linker and librarian operations...
 */
ObjString ObjUtil::ToHex(ObjInt value, int width)
{
    char buf[256], buf2[256];
    int n;
    sprintf(buf, "%X", value);
    n = strlen(buf);
    if (n < width)
    {
        for (int i = n; i < width; i++)
        {
            buf2[i - n] = '0';
        }
        strcpy(buf2 + width - n, buf);
        return buf2;
    }
    return buf;
}
ObjString ObjUtil::ToDecimal(ObjInt value, int width)
{
    char buf[256], buf2[256];
    int n;
    sprintf(buf, "%d", value);
    n = strlen(buf);
    if (n < width)
    {
        for (int i = n; i < width; i++)
        {
            buf2[i - n] = '0';
        }
        strcpy(buf2 + width - n, buf);
        return buf2;
    }
    return buf;
}
ObjInt ObjUtil::FromInt(const char* data, int* pos, int width, int radix)
{
    int n = 0;
    if (pos)
        n = *pos;
    if (!width)
    {
        width = 10000;
    }
    ObjInt val = 0;
    while (width && data[n])
    {
        int ch = data[n];
        if (ch >= 'a' && ch <= 'z')
            ch = ch - 'a' + 'A';
        ch -= '0';
        if (ch < 0)
            break;
        if (ch > 9)
        {
            ch -= 7;
            if (ch < 10)
                break;
        }
        if (ch >= radix)
            break;
        val *= radix;
        val += ch;
        n++, width--;
    }
    if (pos)
        (*pos) = n;
    return val;
}
