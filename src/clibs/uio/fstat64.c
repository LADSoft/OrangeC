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

#define _DEFINING_STAT
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <dos.h>
#include "libp.h"
#include <sys/stat.h>
#include <dir.h>
extern char __uidrives[HANDLE_MAX];
extern int __uiflags[HANDLE_MAX];
extern int __uihandles[HANDLE_MAX];

int _RTL_FUNC _fstat64(int handle, struct _stat64* __statbuf)
{
    int hand, rv;

    __ll_enter_critical();
    hand = __uiohandle(handle);
    if (hand < 0)
    {
        __ll_exit_critical();
        return -1;
    }

    memset(__statbuf, 0, sizeof(*__statbuf));
    struct _stat64 stat64 = { 0 };
    rv = __ll_stat(hand, &stat64);

    __statbuf->st_nlink = 1;
    __statbuf->st_ino = stat64.st_ino;
    __statbuf->st_mode = stat64.st_mode;
    __statbuf->st_uid = stat64.st_uid;
    __statbuf->st_gid = stat64.st_gid;
    __statbuf->st_atime = stat64.st_atime;
    __statbuf->st_mtime = stat64.st_mtime;
    __statbuf->st_ctime = stat64.st_ctime;
    __statbuf->st_size = stat64.st_size;
    if (__ll_isatty(hand))
    {
        __statbuf->st_mode |= S_IFCHR;
        __statbuf->st_rdev = __statbuf->st_dev = handle;
    }
    else
    {
        __statbuf->st_mode = S_IREAD;
        if (__uiflags[handle] & UIF_WRITEABLE)
            __statbuf->st_mode |= S_IWRITE;
        __statbuf->st_mode |= S_IFREG;
        __statbuf->st_rdev = __statbuf->st_dev = __uidrives[handle];
        if ((__statbuf->st_size = filelength(handle)) == -1)
        {
            errno = EBADF;
            __ll_exit_critical();
            return -1;
        }
    }

    /* llstat will return times */
    __ll_exit_critical();
    return rv;
}
