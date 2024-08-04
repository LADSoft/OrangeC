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
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

long _RTL_FUNC filelength(int __handle)
{
    long old, rv, newv;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    old = __ll_getpos(__handle);
    if (old == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    if (__ll_seek(__handle, 0, SEEK_END) == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    newv = __ll_getpos(__handle);
    if (__ll_seek(__handle, old, SEEK_SET) == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    __ll_exit_critical();
    return newv;
}
long _RTL_FUNC _filelength(int __handle) { return filelength(__handle); }