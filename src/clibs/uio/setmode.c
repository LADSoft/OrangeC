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
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __uimodes[HANDLE_MAX];

int __getmode(int __handle)
{
    int old, ohand = __handle;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        errno = EINVAL;
        __ll_exit_critical();
        return -1;
    }
    old = __uimodes[ohand];
    __ll_exit_critical();

    return (old & (O_TEXT | O_BINARY));
}
int _RTL_FUNC setmode(int __handle, int __amode)
{
    int old, ohand = __handle;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        errno = EINVAL;
        __ll_exit_critical();
        return -1;
    }
    old = __uimodes[ohand];
    __amode &= O_TEXT | O_BINARY;
    if ((__amode != O_TEXT) && (__amode != O_BINARY))
    {
        errno = EINVAL;
        __ll_exit_critical();
        return -1;
    }
    __uimodes[ohand] &= ~(O_TEXT | O_BINARY);
    __uimodes[ohand] |= __amode;

    __ll_exit_critical();

    return (old & (O_TEXT | O_BINARY));
}
int _RTL_FUNC _setmode(int __handle, int __amode) { return setmode(__handle, __amode); }