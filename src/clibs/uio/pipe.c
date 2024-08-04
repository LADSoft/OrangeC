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
#include <windows.h>

extern int __uimodes[HANDLE_MAX], __uiflags[HANDLE_MAX], __uihandles[HANDLE_MAX];
extern char __uinames[HANDLE_MAX][256], __uidrives[HANDLE_MAX];

int _RTL_FUNC pipe(int pipeEnds[2]) { return _pipe(pipeEnds, 4 * 1024 * 1024, !!(_fmode & O_TEXT)); }
int _RTL_FUNC _pipe(int* pipeEnds, unsigned int psize, int text)
{
    int oshr, oshw;
    if (__ll_openpipe(&oshr, &oshw, psize) == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    int __amode, handle;
    int i, hr, hw;
    __ll_enter_critical();
    hr = __uinewhandpos();
    if (hr == -1)
    {
        __ll_close(oshw);
        __ll_close(oshr);
        __ll_exit_critical();
        return hr;
    }
    __uihandles[hr] = oshr;
    hw = __uinewhandpos();
    if (hw == -1)
    {
        __ll_close(oshw);
        __ll_close(oshr);
        __uihandles[hr] = -1;
        __ll_exit_critical();
        return hw;
    }
    __uihandles[hw] = oshw;
    __uiflags[hr] = 0;
    __uiflags[hw] = 0;
    __uidrives[hr] = 0;
    __uidrives[hw] = 0;
    strcpy(&__uinames[hr][0], "pipe read");
    strcpy(&__uinames[hw][0], "pipe write");
    int __access = text ? O_TEXT : O_BINARY;
    __uiflags[hw] |= UIF_WRITEABLE;
    __uimodes[hr] = __access | O_RDONLY | 0x80000000;
    __uimodes[hw] = __access | O_WRONLY | 0x80000000;
    pipeEnds[0] = hr;
    pipeEnds[1] = hw;
    __ll_exit_critical();
    return 0;
}
