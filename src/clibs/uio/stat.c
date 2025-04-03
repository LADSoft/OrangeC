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

#undef stat
int _RTL_FUNC stat(char* path, struct stat* __statbuf) 
{ 
    return _stat32(path, (struct _stat32*) __statbuf);
}
#undef _stat
int _RTL_FUNC _stat(char* path, struct stat* __statbuf) 
{
    struct _stat32 stat32 = { 0 };
    int rv = _stat32(path, (struct _stat32*) __statbuf);
    __statbuf->st_dev = stat32.st_dev;
    __statbuf->st_rdev = stat32.st_rdev;
    __statbuf->st_nlink = stat32.st_nlink;
    __statbuf->st_ino = stat32.st_ino;
    __statbuf->st_mode = stat32.st_mode;
    __statbuf->st_uid = stat32.st_uid;
    __statbuf->st_gid = stat32.st_gid;
    __statbuf->st_atime = stat32.st_atime;
    __statbuf->st_mtime = stat32.st_mtime;
    __statbuf->st_ctime = stat32.st_ctime;
    __statbuf->st_size = stat32.st_size;
    return rv;
}


