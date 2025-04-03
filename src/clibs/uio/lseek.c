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

extern int __uiflags[HANDLE_MAX];
long _RTL_FUNC lseek(int __handle, long __offset, int __whence)
{
    int ohand = __handle;
    long rv;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    if (__ll_seek(__handle, __offset, __whence) < 0)
    {
        __ll_exit_critical();
        return -1;
    }
    if (__whence == SEEK_END && __offset >= 0)
        __uiflags[ohand] |= UIF_EOF;
    else
        __uiflags[ohand] &= ~UIF_EOF;
    rv = __ll_getpos(__handle);
    __ll_exit_critical();
    return rv;
}
long _RTL_FUNC _lseek(int __handle, long __offset, int __whence) { return lseek(__handle, __offset, __whence); }
__int64 _RTL_FUNC _lseeki64(int __handle, __int64 __offset, int __whence)
{
    int ohand = __handle;
    __int64 rv;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    if (__ll_seek64(__handle, __offset, __whence) < 0)
    {
        __ll_exit_critical();
        return -1;
    }
    if (__whence == SEEK_END && __offset >= 0)
        __uiflags[ohand] |= UIF_EOF;
    else
        __uiflags[ohand] &= ~UIF_EOF;
    rv = __ll_getpos64(__handle);
    __ll_exit_critical();
    return rv;
}
