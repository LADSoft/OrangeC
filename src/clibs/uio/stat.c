/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

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

int _RTL_FUNC _fstat(int handle, struct _stat* __statbuf)
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

    __statbuf->st_mode = 0;

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
    __statbuf->st_ino = 0;
    __statbuf->st_uid = 0;
    __statbuf->st_nlink = 1;

    /* llstat will return times */
    rv = __ll_stat(hand, __statbuf);
    __ll_exit_critical();
    return rv;
}
int _RTL_FUNC fstat(int handle, struct stat* __statbuf) { return _fstat(handle, (struct _stat*)__statbuf); }
int _RTL_FUNC _stat(char* path, struct _stat* __statbuf)
{
    int fd, rv, rootdir;
    char pbuf[265];
    memset(__statbuf, 0, sizeof(__statbuf));
    if (!strcmp(path, "."))
        path = getcwd(pbuf, 265);
    rootdir = ((path[1] == ':' && (path[2] == '\\' || path[2] == '/') && (path[3] == '.' || path[3] == '\0')) ||
               ((path[0] == '\\' || path[0] == '/') && (path[1] == '.' || path[1] == '\0')));
    if (!rootdir)
    {
        int l = strlen(path);
        if ((path[l - 1] == '/' || path[l - 1] == '\\'))
        {
            path = strcpy(pbuf, path);
            path[l - 1] = '\0';
        }
    }
    rv = __ll_namedstat(path, __statbuf);
    if (rv == -1)
    {
        if (rootdir)
        {
            memset(__statbuf, 0, sizeof(*__statbuf));
            __statbuf->st_atime = __statbuf->st_ctime = __statbuf->st_mtime = (10 * 365 + 2) * 60 * 60 * 24 - _timezone;
            __statbuf->st_mode = S_IFDIR;
            rv = 0;
        }
        else
        {
            fd = open(path, O_RDONLY, 0);
            if (fd != -1)
            {
                if (__ll_isatty(__uihandles[fd]))
                {
                    __statbuf->st_mode |= S_IFREG | S_IREAD | S_IWRITE;
                    rv = 0;
                }
                close(fd);
            }
        }
    }
    if (rv == 0)
    {
        int drive;
        if (path[1] == ':')
        {
            drive = toupper(path[0]) - 'A';
        }
        else
        {
            _dos_getdrive(&drive);
            drive--;
        }
        __statbuf->st_ino = 0;
        __statbuf->st_uid = 0;
        __statbuf->st_nlink = 1;
        __statbuf->st_rdev = __statbuf->st_dev = drive;
    }
    if (rv == -1)
        errno = ENOENT;
    return rv;
}
int _RTL_FUNC stat(char* path, struct stat* __statbuf) { return _stat(path, (struct _stat*)__statbuf); }
int _RTL_FUNC _stat32(char* path, struct stat* __statbuf) { return _stat(path, (struct _stat*)__statbuf); }
