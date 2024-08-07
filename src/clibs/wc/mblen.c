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

#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <stdlib.h>

static mbstate_t current;
int _RTL_FUNC mblen(const char* s, size_t n)
{
    size_t rv;
    if (!s || !*s)
        return 0;
    if (n == 0)
        return -1;
    rv = mbrlen(s, n, &__getRtlData()->mblen_st);
    if (rv == (size_t)-2)
        rv = (size_t)-1;
    return (int)rv;
}
