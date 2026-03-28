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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int __ll_wgetenv(wchar_t* buf, int id);

static char* empty_array[1] = { NULL };

wchar_t _RTL_DATA** _wenviron = empty_array;

void __wenvset(void)
{
    int count = __ll_wgetenv(0, 0), i, j;
    _wenviron = calloc(count + 1, sizeof(wchar_t*));
    for (i = 1, j = 0; i <= count; i++)
    {
        int n = __ll_wgetenvsize(i - 1);
        wchar_t* p = (wchar_t*)malloc((n + 1) * sizeof(wchar_t));
        __ll_wgetenv(p, i - 1);
        if (p[0] != '=')
            _wenviron[j++] = p;
        else
            free(p);
    }
}

void __wmain_envset(void)
{
    __wenvset();
}