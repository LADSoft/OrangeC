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
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <string.h>
#include "_locale.h"

extern LOCALE_HEADER _C_locale_data;
static void pnum(char* str, int num, int fwidth, int zerofil)
{
    int i;
    char* p = str + fwidth - 1;
    if (zerofil)
        memset(str, '0', fwidth);
    while (num && fwidth--)
    {
        *p-- = (char)(num % 10) + '0';
        num = num / 10;
    }
}

char* _RTL_FUNC asctime(struct tm* timeptr)
{
    char* rv = __getRtlData()->asctime_buf;
    TIME_DATA* td = _C_locale_data.td;
    memset(rv, ' ', 25);
    memcpy(rv, td->shortdays[timeptr->tm_wday], 3);
    memcpy(rv + 4, td->shortmonths[timeptr->tm_mon], 3);
    pnum(rv + 7, timeptr->tm_mday, 3, 0);
    pnum(rv + 11, timeptr->tm_hour, 2, 1);
    rv[13] = ':';
    pnum(rv + 14, timeptr->tm_min, 2, 1);
    rv[16] = ':';
    pnum(rv + 17, timeptr->tm_sec, 2, 1);
    pnum(rv + 20, timeptr->tm_year + 1900, 4, 1);
    rv[24] = '\n';
    rv[25] = 0;
    return rv;
}