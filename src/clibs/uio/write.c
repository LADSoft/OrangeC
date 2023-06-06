/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <io.h>
//#include <dos.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#define FILTER_BUF_LEN _DTA_BUF_DEFAULT
extern int __uiflags[HANDLE_MAX], __uimodes[HANDLE_MAX];

// write is thread-safe so...
// this needs to not be a local variable, because,
// when compiled as a DLL it sometimes takes too much stack space and
// crashes the DLL_PROCESS_DETACH
static char obuf[FILTER_BUF_LEN];
int _RTL_FUNC write(int __handle, void* __buf, unsigned __len)
{
    char* pos = __buf;
    int olen = 0;
    int lenleft = __len, writelen, ohand = __handle;
    int i;
    int totalwritten = 0;
    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    if (__uimodes[ohand] & O_RDONLY)
    {
        errno = EBADF;
        __ll_exit_critical();
        return -1;
    }
    if (__len == 0)
    {
        __ll_exit_critical();
        return 0;
    }
    if (__uimodes[ohand] & O_APPEND)
    {
        if (lseek(ohand, 0, SEEK_END) < 0)
        {
            __ll_exit_critical();
            return -1;
        }
        if (!__ll_isatty(__handle))
            __uiflags[ohand] |= UIF_EOF;
    }
    if (__uimodes[ohand] & O_BINARY)
    {
        while (lenleft >= FILTER_BUF_LEN)
        {
            for (i = 0; i < 3; i++)
            {
                writelen = __ll_write(__handle, pos, FILTER_BUF_LEN);
                if (writelen)
                {
                    if (writelen != FILTER_BUF_LEN)
                    {
                        __ll_exit_critical();
                        return writelen + totalwritten;
                    }
                    else
                    {
                        totalwritten += FILTER_BUF_LEN;
                        lenleft -= FILTER_BUF_LEN;
                        pos += FILTER_BUF_LEN;
                        break;
                    }
                }
                //            sleep(1) ;
            }
            if (i >= 3)
            {
                __ll_exit_critical();
                return totalwritten;
            }
        }
        if (lenleft)
        {
            for (i = 0; i < 3; i++)
            {
                writelen = __ll_write(__handle, pos, lenleft);
                if (writelen)
                {
                    __ll_exit_critical();
                    return writelen + totalwritten;
                }
                //            sleep(1) ;
            }
        }
        __ll_exit_critical();
        return totalwritten;
    }
    for (i = 0; i < __len; i++)
    {
        if (*pos == '\n')
        {
            obuf[olen++] = '\r';
            totalwritten--;
            if (i < __len - 1 && *(pos + 1) == 0)
            { /* wchar? */
                obuf[olen++] = 0;
                totalwritten--;
            }
        }
        obuf[olen++] = *pos++;
        if (olen >= FILTER_BUF_LEN - 1)
        {
            int j;
            for (j = 0; j < 3; j++)
            {
                writelen = __ll_write(__handle, obuf, olen);
                if (writelen)
                {
                    if (writelen != olen)
                        return totalwritten + writelen;
                    totalwritten += writelen;
                    break;
                }
                //            sleep(1) ;
            }
            if (j >= 3)
            {
                __ll_exit_critical();
                return totalwritten;
            }
            olen = 0;
        }
    }
    if (olen)
    {
        int j;
        for (j = 0; j < 3; j++)
        {
            writelen = __ll_write(__handle, obuf, olen);
            if (writelen)
            {
                __ll_exit_critical();
                return totalwritten + writelen;
            }
            //            sleep(1) ;
        }
    }
    __ll_exit_critical();
    return totalwritten;
}
int _RTL_FUNC _write(int __handle, void* __buf, unsigned __len) { return write(__handle, __buf, __len); }
