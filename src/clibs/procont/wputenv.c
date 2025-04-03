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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include "errno.h"

extern wchar_t _RTL_DATA** __wenviron;
int _RTL_FUNC _wputenv(const wchar_t* name)
{
    if (!__wenviron)
        __wenvset();
    wchar_t **q = __wenviron, **p;
    int len = 0, count = 0;
    wchar_t* z = wcschr(name, '=');
    if (!z)
        return -1;
    len = z - name;
    __ll_enter_critical();
    while (*q)
    {
        count++;
        if (!wcsnicmp(name, *q, len) && (*q)[len] == '=')
        {
            *q = name;  // yes this is supposed to be raw
            __ll_exit_critical();
            return 0;
        }
        q++;
    }
    p = (wchar_t**)realloc(__wenviron, (count + 2) * sizeof(wchar_t**));
    if (!p)
    {
        __ll_exit_critical();
        errno = ENOMEM;
        return -1;
    }
    p[count + 2 - 1] = NULL;
    p[count + 2 - 2] = name;
    __wenviron = p;
    __ll_exit_critical();
    return 0;
}
int _RTL_FUNC _wputenv_s(const wchar_t* name, const wchar_t* value)
{
    wchar_t* buf = calloc(wcslen(name) + wcslen(value) + 2, sizeof(wchar_t));
    if (!buf)
    {
        errno = ENOMEM;
        return -1;
    }
    wcscpy(buf, name);
    buf[wcslen(buf)] = '=';
    wcscat(buf, value);
    int rv = _wputenv(wcsdup(buf));
    free(buf);
    return rv;
}
int _RTL_FUNC _wsetenv(const wchar_t* name, const wchar_t* value, int overwrite)
{
    if (name == 0 || name[0] == 0 || wcschr(name, '='))
    {
        errno = EINVAL;
        return -1;
    }
    if (!overwrite && _wgetenv(name))
        return 0;
    return _wputenv_s(name, value);
}
int _RTL_FUNC _wunsetenv(const wchar_t* name)
{
    if (!__wenviron)
        __wenvset();
    wchar_t** q = __wenviron;
    int len = 0;
    len = wcslen(name);
    __ll_enter_critical();
    while (*q)
    {
        if (!wcsnicmp(name, *q, len) && (*q)[len] == '=')
        {
            while (*q)
            {
                *q = *(q + 1);
                q++;
            }
            __ll_exit_critical();
            return 0;
        }
        q++;
    }
    __ll_exit_critical();
    return 0;
}