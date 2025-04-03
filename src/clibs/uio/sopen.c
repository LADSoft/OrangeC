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

int __umask_perm = 0;

int _RTL_FUNC _wsopen(const wchar_t* __path, int __access, int __shmode, ... /*unsigned mode*/)
{
    va_list ap;
    int __amode, handle;
    int i, h;
    if (!__path)
    {
        errno = ENOENT;
        return -1;
    }
    __ll_enter_critical();
    h = __uinewhandpos();
    if (h == -1)
    {
        __ll_exit_critical();
        return h;
    }
    __uiflags[h] = 0;
    if (__path[1] == ':')
        __uidrives[h] = toupper(__path[0]) - 'A';
    else
        __uidrives[h] = getdisk();
    memcpy(&__uinames[h][0], __path, strlen(__path));
    if (!(__access & (O_BINARY | O_TEXT)))
        __access |= _fmode;
    if (__ll_writeable(__path))
        __uiflags[h] |= UIF_WRITEABLE;
    if (__access & O_CREAT)
    {
        if (__umask_perm & S_IWRITE)
        {
            errno = EACCES;
            __ll_exit_critical();
            return -1;
        }
        handle = __ll_open(__path, __ll_uioflags(__access), __ll_shflags(__shmode));
        if (__access & O_EXCL)
        {
            if (handle)
            {
                errno = EEXIST;
                __ll_close(handle);
                __ll_exit_critical();
                return -1;
            }
        }
        if (handle)
        {
            __uimodes[h] = __access;
            __uihandles[h] = handle;
            if (__access & O_TRUNC)
                chsize(h, 0);
            if (!__ll_isatty(handle))
                __uiflags[h] |= UIF_EOF;
        }
        else
        {
            errno = 0;
            handle = __ll_creat(__path, __ll_uioflags(__access), __ll_shflags(__shmode));
            if (handle == 0)
            {
                errno = ENOENT;
                __ll_exit_critical();
                return -1;
            }
            __uimodes[h] = __access;
            __uihandles[h] = handle;
            if (__access & O_TRUNC)
                chsize(h, 0);
            va_start(ap, __shmode);
            __amode = *(int*)ap;
            if (!(__amode & S_IWRITE))
                __uiflags[h] |= UIF_RO;
            if (!__ll_isatty(handle))
                __uiflags[h] |= UIF_EOF;
            va_end(ap);
        }
    }
    else
    {
        handle = __ll_open(__path, __ll_uioflags(__access), __ll_shflags(__shmode));
        if (handle == 0)
        {
            errno = ENOENT;
            __ll_exit_critical();
            return -1;
        }
        __uimodes[h] = __access;
        __uihandles[h] = handle;
        if (__access & O_TRUNC)
            chsize(h, 0);
    }
    if (__uimodes[h] & O_APPEND)
    {
        if (lseek(h, 0, SEEK_END) < 0)
        {
            close(h);
            __uihandles[h] = -1;
            __ll_exit_critical();
            return -1;
        }
        if (!__ll_isatty(handle))
            __uiflags[h] |= UIF_EOF;
    }
    __ll_exit_critical();
    return h;
}
int _RTL_FUNC sopen(const char* __path, int __access, int __shmode, ... /*unsigned mode*/)
{
    wchar_t buf[260], *p = buf;
    while (*__path)
        *p++ = *__path++;
    *p = *__path;
    int mode;
    va_list ap;
    va_start(ap, __shmode);
    mode = *(int*)ap;
    va_end(ap);
    return _wsopen(buf, __access, __shmode, mode);
}
int _RTL_FUNC _sopen(const char* __path, int __access, int __shmode, ... /*unsigned mode*/)
{
    int mode;
    va_list ap;
    va_start(ap, __shmode);
    mode = *(int*)ap;
    va_end(ap);
    return sopen(__path, __access, __shmode, mode);
}
