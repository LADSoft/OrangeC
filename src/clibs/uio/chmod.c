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

#include <io.h>
#include <dos.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int _RTL_FUNC _wchmod(const wchar_t* __path, int __amode)
{
    int i;
    for (i = 0; i < 3; i++)
    {
        if (!__ll_chmod(__path, __amode))
            return 0;
        if (errno == ENOENT)
            return -1;
        sleep(1);
    }
    return -1;
}
int _RTL_FUNC chmod(const char* __path, int __amode)
{
    wchar_t buf[260], *p = buf;
    while (*__path)
        *p++ = *__path++;
    *p = *__path;
    return _wchmod(buf, __amode);
}

int _RTL_FUNC _chmod(const char* __path, int __amode) { return chmod(__path, __amode); }
