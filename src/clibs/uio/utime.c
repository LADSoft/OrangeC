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

#include <time.h>
#include <utime.h>
#include <io.h>
#include <fcntl.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <wchar.h>
#include "libp.h"

extern char __uinames[HANDLE_MAX][256];

int _RTL_FUNC _futime(int __handle, struct utimbuf* times)
{
    int rv;
    struct tm *p, stm, sta;

    if (!times)
    {
        time_t now = time(NULL);
        stm = sta = *localtime(&now);
    }
    else
    {
        p = localtime(&times->modtime);
        if (!p)
            return -1;
        stm = *p;
        p = localtime(&times->actime);
        if (!p)
            return -1;
        sta = *p;
    }

    __ll_enter_critical();
    __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    rv = __ll_utime(__handle, &sta, &stm);
    __ll_exit_critical();
    return rv;
}
int _RTL_FUNC _utime(const char* path, struct _utimbuf* times)
{
    int handle;
    int rv;

    if ((handle = open(path, O_RDWR)) == -1)
        return -1;

    rv = _futime(handle, times);

    close(handle);
    return rv;
}
int _RTL_FUNC _wutime(const wchar_t* path, struct _utimbuf* times)
{
    int handle;
    int rv;

    if ((handle = _wsopen(path, O_RDWR, S_IREAD | S_IWRITE, 0)) == -1)
        return -1;

    rv = _futime(handle, times);

    close(handle);
    return rv;
}
int _RTL_FUNC utime(const char* path, struct utimbuf* times) { return _utime(path, times); }
int _RTL_FUNC _utime32(const char* path, struct utimbuf* times) { return _utime(path, times); }

// resolution is nearest second
int _RTL_FUNC utimes(const char* path, struct timeval times[2]) 
{
    __ll_enter_critical();
    int __handle = __uiohandle(__handle);
    if (__handle == -1)
    {
        __ll_exit_critical();
        return -1;
    }
    struct utimbuf utimex[2] = { times[0].tv_sec, times[1].tv_sec };
    int rv = _utime(path, utimex); 
    __ll_exit_critical;
    return rv;
}
