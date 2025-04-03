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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <assert.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>

extern int __uihandles[HANDLE_MAX], __uimodes[HANDLE_MAX], __uiflags[HANDLE_MAX];
extern char __uinames[HANDLE_MAX][256];
extern struct flock* __uilocks[HANDLE_MAX];

static int fcntl__uinewhandpos(int lowest)
{
    int i;
    for (i = lowest; i < HANDLE_MAX; i++)
        if (__uihandles[i] == -1)
            return i;
    errno = EMFILE;
    return -1;
}
static int _RTL_FUNC fcntl_dup(int __handle, int lowest)
{
    int h;
    int i, ohand = __handle;
    __ll_enter_critical();
    h = fcntl__uinewhandpos(lowest);
    if (h == -1)
    {
        __ll_exit_critical();
        return h;
    }
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return __handle;
    }
    i = __ll_dup(__handle);
    if (i == 0)
    {
        __ll_exit_critical();
        return -1;
    }
    __uimodes[h] = __uimodes[ohand] & ~FD_CLOEXEC;
    __uihandles[h] = i;
    __uiflags[h] = __uiflags[ohand];
    strcpy(__uinames[h], __uinames[ohand]);
    __ll_exit_critical();
    return h;
}
static int fcntl_getlock(int __handle, struct flock* lock)
{
    int istart = lock->l_len < 0 ? lock->l_start - lock->l_len : lock->l_start;
    int iend = lock->l_len < 0 ? lock->l_start : lock->l_start + lock->l_len;
    struct flock* locks = __uilocks[__handle];
    while (locks)
    {
        int start = locks->l_len < 0 ? locks->l_start - locks->l_len : locks->l_start;
        int end = locks->l_len < 0 ? locks->l_start : locks->l_start + locks->l_len;
        if (start <= istart && end > istart || start < iend && end > istart)
        {
            *lock = *locks;
            return 0;
        }
        locks = locks->l_next;
    }
    lock->l_type = F_UNLCK;
    // pid not supported
    lock->l_pid = 0;
    return 0;
}
static int fcntl_getpos(int __handle, int whence)
{
    if (whence == SEEK_SET)
    {
        return 0;
    }
    else if (whence == SEEK_CUR)
    {
        return __ll_getpos(__uihandles[__handle]);
    }
    else
    {
        int n = __ll_getpos(__uihandles[__handle]);
        if (n == -1)
            return n;
        if (__ll_seek(__uihandles[__handle], 0, SEEK_END) == -1)
            return -1;
        int rv = __ll_getpos(__uihandles[__handle]);
        if (rv == -1)
            return rv;
        if (__ll_seek(__uihandles[__handle], 0, SEEK_SET) == -1)
            return -1;
        return rv;
    }
}
static int fcntl_setlock(int __handle, struct flock* lock, int wait)
{
    if (lock->l_type == F_UNLCK)
    {
        struct flock ilock = *lock;
        int pos = fcntl_getpos(__handle, lock->l_whence);
        ilock.l_start += pos;
        ilock.l_whence = SEEK_SET;
        int start = ilock.l_len < 0 ? ilock.l_start - ilock.l_len : ilock.l_start;
        int end = ilock.l_len < 0 ? ilock.l_start : ilock.l_start + ilock.l_len;
        struct flock** search = &__uilocks[__handle];
        while (*search)
        {
            int sstart = (*search)->l_len < 0 ? (*search)->l_start - (*search)->l_len : (*search)->l_start;
            int send = (*search)->l_len < 0 ? (*search)->l_start : (*search)->l_start + (*search)->l_len;
            if (start == sstart && end == send)
                break;
            search = &(*search)->l_next;
        }
        if (*search)
        {
            struct flock* p = *search;
            *search = (*search)->l_next;
            free(p);
            return __ll_unlock(__uihandles[__handle], start, end);
        }
        return -1;
    }
    struct flock* ilock = calloc(1, sizeof(struct flock));
    if (!ilock)
    {
        errno = ENOMEM;
        return -1;
    }
    *ilock = *lock;
    int pos = fcntl_getpos(__handle, lock->l_whence);
    ilock->l_start += pos;
    ilock->l_whence = SEEK_SET;
    struct flock** search = &__uilocks[__handle];
    int start = ilock->l_len < 0 ? ilock->l_start - ilock->l_len : ilock->l_start;
    int end = ilock->l_len < 0 ? ilock->l_start : ilock->l_start + ilock->l_len;
    while (*search)
    {
        int sstart = (*search)->l_len < 0 ? (*search)->l_start - (*search)->l_len : (*search)->l_start;
        if (sstart > start)
            break;
        search = &(*search)->l_next;
    }
    ilock->l_next = *search;
    *search = ilock;
    return __ll_lock(__uihandles[__handle], start, end, lock->l_type == F_WRLCK, wait);
}
int _RTL_FUNC fcntl(int __handle, int type, ...)
{
    int rv = 0;
    va_list args;
    va_start(args, type);
    int mode;
    __ll_enter_critical();
    switch (type)
    {
        case F_DUPFD:
            rv = fcntl_dup(__handle, va_arg(args, int));
            break;
        case F_GETFD:
            rv = __uimodes[__handle] & FD_CLOEXEC;
            break;
        case F_SETFD:
            __uimodes[__handle] &= ~FD_CLOEXEC;
            __uimodes[__handle] |= va_arg(args, int) & FD_CLOEXEC;
            break;
        case F_GETFL:
            rv = __uimodes[__handle];
            break;
        case F_SETFL:
            __uimodes[__handle] &= 0xf00 | O_ACCMODE;
            __uimodes[__handle] |= ~(0xf00 | O_ACCMODE) & va_arg(args, int);
            break;
        case F_GETOWN:
        case F_SETOWN:
            // noop
            break;
        case F_GETLK:
            rv = fcntl_getlock(__handle, va_arg(args, struct flock*));
            break;
        case F_SETLK:
            rv = fcntl_setlock(__handle, va_arg(args, struct flock*), 0);
            break;
        case F_SETLKW:
            rv = fcntl_setlock(__handle, va_arg(args, struct flock*), 1);
            break;
    }
    __ll_exit_critical();
    va_end(args);
    return rv;
}
