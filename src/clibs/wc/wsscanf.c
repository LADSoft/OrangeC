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

#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int _RTL_FUNC wscanf(const wchar_t* format, ...) { return __wscanf(stdin, format, ((char*)&format + sizeof(wchar_t*))); }
int _RTL_FUNC vwscanf(const wchar_t* restrict format, va_list arglist) { vfwscanf(stdin, format, arglist); }
int _RTL_FUNC vswscanf(wchar_t* restrict buf, const wchar_t* restrict format, va_list list)
{
    int rv;
    FILE fil;
    struct __file2 fil2;
    memset(&fil, 0, sizeof(fil));
    memset(&fil2, 0, sizeof(fil2));
    fil.level = wcslen(buf) * sizeof(wchar_t) + sizeof(wchar_t);
    fil.flags = _F_IN | _F_READ | _F_BUFFEREDSTRING;
    fil.bsize = wcslen(buf) * sizeof(wchar_t);
    fil.buffer = fil.curp = buf;
    fil.token = FILTOK;
    fil.extended = &fil2;
    return __wscanf_unlocked(&fil, format, list);
}
int _RTL_FUNC swscanf(wchar_t* restrict buf, const wchar_t* restrict format, ...)
{
    return vswscanf(buf, format, (((char*)&format) + sizeof(wchar_t*)));
}
int vfwscanf(FILE* restrict fil, const wchar_t* restrict format, va_list arglist) { return __wscanf(fil, format, arglist); }
int fwscanf(FILE* restrict fil, const wchar_t* restrict format, ...)
{
    return __wscanf(fil, format, ((char*)&format + sizeof(wchar_t*)));
}