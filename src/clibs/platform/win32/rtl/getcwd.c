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

#include <windows.h>
#include <dos.h>
#include <dir.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wchar.h>
wchar_t* _RTL_FUNC _wgetcwd(wchar_t* __buf, int __buflen)
{
    wchar_t ibuf[265];
    GetCurrentDirectoryW(256, ibuf);
    if (!__buf)
        __buf = wcsdup(ibuf);
    else
    {
        wmemcpy(__buf, ibuf, __buflen);
        __buf[__buflen - 1] = 0;
    }
    return __buf;
}
char* _RTL_FUNC getcwd(char* __buf, int __buflen)
{
    wchar_t buf1[260], *p = buf1;
    _wgetcwd(buf1, __buflen > 259 ? 260 : __buflen);
    if (!__buf)
    {
        __buf = malloc(wcslen(p) + 1);
        if (!__buf)
            return __buf;
    }
    while (*p)
        *__buf++ = *p++;
    *__buf = *p;
    return __buf - (p - buf1);
}
char* _RTL_FUNC _getcwd(char* __buf, int __buflen) { return getcwd(__buf, __buflen); }
