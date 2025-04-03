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

extern char _RTL_DATA** _environ;
int _RTL_FUNC putenv(const char* name)
{
    wchar_t *buf = calloc(sizeof(wchar_t), strlen(name)+1), *x = buf;
    const char *y = name;
    while (*y)
        *x++ = *y++;
    int rv = _wputenv(buf);
    if (rv)
        return rv;
    char **q = _environ, **p;
    int len = 0, count = 0;
    char* z = strchr(name, '=');
    if (!z)
        return -1;
    len = z - name;
    __ll_enter_critical();
    while (*q)
    {
        count++;
        if (!strnicmp(name, *q, len) && (*q)[len] == '=')
        {
            *q = name;  // yes this is supposed to be raw
            __ll_exit_critical();
            return 0;
        }
        q++;
    }
    p = (char**)realloc(_environ, (count + 2) * sizeof(char**));
    if (!p)
    {
        __ll_exit_critical();
        errno = ENOMEM;
        return -1;
    }
    p[count + 2 - 1] = NULL;
    p[count + 2 - 2] = name;
    _environ = p;
    __ll_exit_critical();
    return 0;
}
int _RTL_FUNC _putenv(const char* name) { return putenv(name); }
int _RTL_FUNC _putenv_s(const char* name, const char* value)
{
    char* buf = calloc(strlen(name) + strlen(value) + 2, 1);
    if (!buf)
    {
        errno = ENOMEM;
        return -1;
    }
    strcpy(buf, name);
    buf[strlen(buf)] = '=';
    strcat(buf, value);
    int rv = putenv(strdup(buf));
    free(buf);
    return rv;
}
int _RTL_FUNC setenv(const char* name, const char* value, int overwrite)
{
    if (name == 0 || name[0] == 0 || strchr(name, '='))
    {
        errno = EINVAL;
        return -1;
    }
    if (!overwrite && getenv(name))
        return 0;
    return _putenv_s(name, value);
}
int _RTL_FUNC unsetenv(const char* name)
{
    wchar_t buf[260], *x = buf;
    const char *y = name;
    while (*y)
        *x++ = *y++;
    *x= *y;
    int rv = _wunsetenv(buf);
    if (rv)
        return rv;
    char** q = _environ;
    int len = 0;
    len = strlen(name);
    __ll_enter_critical();
    while (*q)
    {
        if (!strnicmp(name, *q, len) && (*q)[len] == '=')
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