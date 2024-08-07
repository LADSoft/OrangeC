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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define imin(x, y) ((x) < 0 ? 0 : ((x) < (y) ? (x) : (y)))

int _RTL_FUNC vswprintf(wchar_t* restrict buf, size_t n, const wchar_t* restrict format, va_list arglist)
{
    wchar_t buffer[8192];
    FILE fil;
    struct __file2 fil2;
    int written;
    memset(&fil, 0, sizeof(fil));
    memset(&fil2, 0, sizeof(fil2));
    fil.level = -sizeof(buffer);
    fil.flags = _F_OUT | _F_WRIT | _F_BUFFEREDSTRING;
    fil.bsize = sizeof(buffer) - sizeof(wchar_t);
    fil.buffer = fil.curp = buffer;
    fil.token = FILTOK;
    fil.extended = &fil2;
    written = vfwprintf_unlocked(&fil, format, arglist);
    if (buf && n)
    {
        int copied = imin(written, n - 1);
        memcpy(buf, buffer, copied * sizeof(wchar_t));
        buf[copied] = 0;
    }
    return written;
}
int _RTL_FUNC swprintf(wchar_t* restrict buffer, size_t n, const wchar_t* restrict format, ...)
{
    return vswprintf(buffer, n, format, (((char*)&format) + sizeof(wchar_t**)));
}
int _RTL_FUNC vsnwprintf(wchar_t* restrict buf, size_t n, const wchar_t* restrict format, va_list arglist)
{
    return vswprintf(buf, n, format, (((char*)&format) + sizeof(wchar_t**)));
}
int _RTL_FUNC snwprintf(wchar_t* restrict buffer, size_t n, const wchar_t* restrict format, ...)
{
    return vswprintf(buffer, n, format, (((char*)&format) + sizeof(wchar_t**)));
}
int _RTL_FUNC _vsnwprintf(wchar_t* restrict buf, size_t n, const wchar_t* restrict format, va_list arglist)
{
    return vswprintf(buf, n, format, (((char*)&format) + sizeof(wchar_t**)));
}
int _RTL_FUNC _snwprintf(wchar_t* restrict buffer, size_t n, const wchar_t* restrict format, ...)
{
    return vswprintf(buffer, n, format, (((char*)&format) + sizeof(wchar_t**)));
}
