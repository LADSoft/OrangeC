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

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <fcntl.h>

extern int __uihandles[HANDLE_MAX], __uiflags[HANDLE_MAX];
extern char __uinames[HANDLE_MAX][256];
extern struct flock* __uilocks[HANDLE_MAX];

int _RTL_FUNC close(int __handle)
{
    int i, ohand = __handle;

    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    __uihandles[ohand] = -1;
    __ll_close(__handle);
    if (__uiflags[ohand] & UIF_RO)
    {
        chmod(__uinames[ohand], 0);
    }
    while (__uilocks[ohand])
    {
        struct flock* next = __uilocks[ohand]->l_next;
        free(__uilocks[ohand]);
        __uilocks[ohand] = next;
    }
    __ll_exit_critical();
    return 0;
}
int _RTL_FUNC _close(int __handle) { return close(__handle); }
