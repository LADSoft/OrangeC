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

#define USE_32BIT_TIME_T
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

int _RTL_FUNC _wstat32(wchar_t* path, struct _stat32* __statbuf)
{
    int fd, rv, rootdir;
    char pbuf[265];
    wchar_t ppbuf[260], *q = ppbuf;
    memset(__statbuf, 0, sizeof(__statbuf));
    if (!wcscmp(path, L"."))
    {
         char *path2 = getcwd(pbuf, 265);
         while (*path2)
             *q++ = *path2++;
         *q = *path2;
         path = ppbuf;
          
    }
    rootdir = ((path[1] == ':' && (path[2] == '\\' || path[2] == '/') && (path[3] == '.' || path[3] == '\0')) ||
               ((path[0] == '\\' || path[0] == '/') && (path[1] == '.' || path[1] == '\0')));
    if (!rootdir)
    {
        int l = wcslen(path);
        if ((path[l - 1] == '/' || path[l - 1] == '\\'))
        {
            path = wcscpy(ppbuf, path);
            path[l - 1] = '\0';
        }
    }
    struct _stat64 stat64 = {0};
    rv = __ll_namedstat(path, &stat64);
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
            FILE* fil = _wfopen(path, L"r");
            if (fil)
            {
                fd = fileno(fil);
                if (__ll_isatty(__uihandles[fd]))
                {
                    __statbuf->st_mode |= S_IFREG | S_IREAD | S_IWRITE;
                    rv = 0;
                }
                fclose(fil);
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
        __statbuf->st_nlink = 1;
        __statbuf->st_rdev = __statbuf->st_dev = drive;
        __statbuf->st_ino = stat64.st_ino;
        __statbuf->st_mode = stat64.st_mode;
        __statbuf->st_uid = stat64.st_uid;
        __statbuf->st_gid = stat64.st_gid;
        __statbuf->st_atime = stat64.st_atime;
        __statbuf->st_mtime = stat64.st_mtime;
        __statbuf->st_ctime = stat64.st_ctime;
        __statbuf->st_size = stat64.st_size;

    }
    if (rv == -1)
        errno = ENOENT;
    return rv;
}
int _RTL_FUNC _stat32(char* path, struct _stat32* __statbuf) 
{
    wchar_t buf[256], *q = buf;
    while (*path)
        *q++ = *path++;
    *q = *path;
    return _wstat32(buf, __statbuf);
}
