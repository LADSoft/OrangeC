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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern wchar_t _RTL_DATA** __wenviron;

wchar_t* _RTL_FUNC _wgetenv(const wchar_t* varname)
{
    if (!__wenviron)
        __wenvset();
    wchar_t** q = __wenviron;
    int len = wcslen(varname);
    __ll_enter_critical();
    while (*q)
    {
        if ((*q)[len] == '=' && !wcsnicmp(varname, *q, len))
        {
            __ll_exit_critical();
            return (*q + len + 1);
        }
        q++;
    }
    __ll_exit_critical();
    return 0;
}