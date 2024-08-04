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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __uihandles[HANDLE_MAX], __uimodes[HANDLE_MAX], __uiflags[HANDLE_MAX];
extern char __uinames[HANDLE_MAX][256];

int _RTL_FUNC dup(int __handle)
{
    int h;
    int i, ohand = __handle;
    __ll_enter_critical();
    h = __uinewhandpos();
    if (h == -1)
    {
        __ll_exit_critical();
        return h;
    }
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return __handle;
    }
    i = __ll_dup(__handle);
    if (i == 0)
    {
        __ll_exit_critical();
        return -1;
    }
    __uimodes[h] = __uimodes[ohand];
    __uihandles[h] = i;
    __uiflags[h] = __uiflags[ohand];
    strcpy(__uinames[h], __uinames[ohand]);
    __ll_exit_critical();
    return h;
}
int _RTL_FUNC _dup(int __handle) { return dup(__handle); }