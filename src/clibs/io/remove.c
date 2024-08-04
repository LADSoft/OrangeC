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

#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <libp.h>
#include <errno.h>

int _RTL_FUNC rmdir(const char* name) 
{ 
    wchar_t buf[260], *p = buf; 
    while (*name)
        *p++ = *name++;
    *p = *name;
    return __ll_rmdir(buf); 
}
int _RTL_FUNC _rmdir(const char* name) { return rmdir(name); }
int _RTL_FUNC _wrmdir(const wchar_t* name) { return __ll_rmdir(name); }
int _RTL_FUNC remove(const char* name)
{
    int rv = unlink(name);
    if (rv)
    {
        int errx = errno;
        rv = rmdir(name);
        if (rv)
        {
            errno = errx;
            return 1;
        }
    }
    return 0;
}
int _RTL_FUNC _wremove(const wchar_t* name)
{
    int rv = __ll_remove(name);
    if (rv)
    {
        int errx = errno;
        rv = __ll_rmdir(name);
        if (rv)
        {
            errno = errx;
            return 1;
        }
    }
    return 0;
}
int _RTL_FUNC _wunlink(const wchar_t* name) { return (__ll_remove(name)); }
int _RTL_FUNC unlink(const char* name) 
{ 
    wchar_t buf[260], *p = buf; 
    while (*name)
        *p++ = *name++;
    *p = *name;
    return (__ll_remove(buf)); 
}
int _RTL_FUNC _unlink(const char* name) { return (unlink(name)); }