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

#include <errno.h>
#include <windows.h>
#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

unsigned _RTL_FUNC _dos_getdiskfree(unsigned __drive, struct diskfree_t* __dtable)
{
    char *s = 0, buf[4];
    DWORD secperclust, bytepersect, availclust, totalclust;
    strcpy(buf, "@:\\");
    if (__drive)
    {
        buf[0] += __drive;
        s = buf;
    }
    if (GetDiskFreeSpace(s, &secperclust, &bytepersect, &availclust, &totalclust))
    {
        __dtable->sectors_per_cluster = secperclust;
        __dtable->bytes_per_sector = bytepersect;
        __dtable->total_clusters = totalclust;
        __dtable->avail_clusters = availclust;
        return 0;
    }
    errno = EINVAL;
    return GetLastError();
}
unsigned _RTL_FUNC _getdiskfree(unsigned __drive, struct _diskfree_t* __dtable) { return _dos_getdiskfree(__drive, __dtable); }
