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

#include <stdio.h>
#include <errno.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <fcntl.h>

int _fmode = O_BINARY;
int __uihandles[HANDLE_MAX], __uimodes[HANDLE_MAX], __uiflags[HANDLE_MAX];
char __uinames[HANDLE_MAX][256], __uidrives[HANDLE_MAX];
struct flock* __uilocks[HANDLE_MAX];

#pragma startup uio_init 29
#pragma rundown __uio_rundown 9 /* 11 */

static void uio_init(void)
{
    int i;
    __ll_uioinit(); /* sets first three handles */
    for (i = 0; i < 3; i++)
    {
        __uimodes[i] = O_TEXT;
    }
    for (i = 3; i < HANDLE_MAX; i++)
        __uihandles[i] = -1;
}
void __uio_rundown(void)
{
    int i;
    for (i = 3; i < HANDLE_MAX; i++)
        if (__uihandles[i] != -1)
        {
            __ll_close(__uihandles[i]);
            __uihandles[i] = -1;
        }
}
int __uiohandle(int __handle)
{
    if (__handle >= HANDLE_MAX || __handle < 0)
    {
        errno = EBADF;
        return -1;
    }
    if (__uihandles[__handle] == -1)
        errno = EBADF;
    return __uihandles[__handle];
}
int __uinewhandpos(void)
{
    int i;
    for (i = 0; i < HANDLE_MAX; i++)
        if (__uihandles[i] == -1)
            return i;
    errno = EMFILE;
    return -1;
}