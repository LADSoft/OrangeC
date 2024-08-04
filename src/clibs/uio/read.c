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
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __uiflags[HANDLE_MAX], __uimodes[HANDLE_MAX];

int _RTL_FUNC read(int __handle, void* __buf, unsigned __len)
{
    char* pos = __buf;
    char* dest = __buf;
    int readlen;
    int ohand = __handle;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    if (__uimodes[ohand] & O_WRONLY)
    {
        __ll_exit_critical();
        errno = EBADF;
        return -1;
    }
    if (__uiflags[ohand] & UIF_EOF)
    {
        __ll_exit_critical();
        return 0;
    }
    if (__uimodes[ohand] & O_BINARY)
    {
        int i;
        for (i = 0; i < 3; i++)
        {
            __ll_exit_critical();
            readlen = __ll_read(__handle, dest, __len);
            __ll_enter_critical();
            if (readlen >= 0)
                break;
            sleep(1);
        }
        if (readlen == -1)
        {
            __ll_exit_critical();
            return -1;
        }
        if (readlen < __len && !__ll_isatty(__handle))
            if (!(__uimodes[ohand] & 0x80000000) || readlen == 0)  // Not a pipe
                __uiflags[ohand] |= UIF_EOF;
        __ll_exit_critical();
        return readlen;
    }
    else
    {
        int i;
        do
        {
            for (i = 0; i < 3; i++)
            {
                __ll_exit_critical();
                readlen = __ll_read(__handle, dest, __len);
                __ll_enter_critical();
                if (readlen >= 0)
                    break;
                sleep(1);
            }
            if (readlen == -1)
            {
                __ll_exit_critical();
                return -1;
            }
            if (readlen < __len && !__ll_isatty(__handle))
                if (!(__uimodes[ohand] & 0x80000000) || readlen == 0)  // not a pipe
                    __uiflags[ohand] |= UIF_EOF;
            pos = __buf;
            for (i = 0; i < readlen; i++)
            {
                switch (*pos)
                {
                    case '\x1a':
                        if (!__ll_isatty(__handle))
                            __uiflags[ohand] |= UIF_EOF;
                        __ll_exit_critical();
                        return dest - __buf;
                    case '\r':
                        pos++;
                        break;
                    default:
                        *dest++ = *pos++;
                        break;
                }
            }
        } while (dest - __buf == 0 && !(__uiflags[ohand] & UIF_EOF));
        __ll_exit_critical();
        return dest - __buf;
    }
}
int _RTL_FUNC _read(int __handle, void* __buf, unsigned __len) { return read(__handle, __buf, __len); }
