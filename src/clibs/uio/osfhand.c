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
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <io.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dir.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __uimodes[HANDLE_MAX], __uiflags[HANDLE_MAX], __uihandles[HANDLE_MAX];
extern char __uinames[HANDLE_MAX][256], __uidrives[HANDLE_MAX];

long _RTL_FUNC _get_osfhandle(int __handle)
{
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    __ll_exit_critical();
    if (__handle == -1)
        errno = EBADF;
    return __handle;
}
int _RTL_FUNC _open_osfhandle(long __handle, int __access)
{
    int h;
    __ll_enter_critical();
    h = __uinewhandpos();
    if (h == -1)
    {
        __ll_exit_critical();
        return h;
    }
    __uiflags[h] = 0;
    __uidrives[h] = 0;
    __uinames[h][0] = 0;
    if (!(__access & O_TEXT))
        __access |= O_BINARY;
    __uimodes[h] = __access;
    __uihandles[h] = __handle;
    if (__uimodes[h] & O_APPEND)
    {
        if (lseek(h, 0, SEEK_END) < 0)
        {
            close(h);
            __uihandles[h] = -1;
            __ll_exit_critical();
            return -1;
        }
        __uiflags[h] |= UIF_EOF;
    }
    __ll_exit_critical();
    return h;
}
