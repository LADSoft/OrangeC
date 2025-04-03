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
#include <dos.h>
#include <fcntl.h>

int _RTL_FUNC chsize(int __handle, long __size)
{
    int i;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    for (i = 0; i < 3; i++)
    {
        if (!__ll_chsize(__handle, __size))
        {
            __ll_exit_critical();
            return 0;
        }
        sleep(1);
    }
    __ll_exit_critical();
    return -1;
}
int _RTL_FUNC _chsize(int __handle, long __size) { return chsize(__handle, __size); }
int _RTL_FUNC truncate(const char * __filename, long __size)
{
    int handle = open(__filename, O_RDWR);
    if (handle != -1)
    {
        int rv = _chsize(handle, __size);
        close(handle);
        return rv;
    }
    return -1;
}