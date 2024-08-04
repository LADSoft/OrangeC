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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

struct tm* __ll_gettime(struct tm* tm2)
{
    SYSTEMTIME timex;
    memset(tm2, 0, sizeof(*tm2));
    GetLocalTime(&timex);

    tm2->tm_sec = timex.wSecond;
    tm2->tm_min = timex.wMinute;
    tm2->tm_hour = timex.wHour;

    tm2->tm_mday = timex.wDay;
    tm2->tm_mon = timex.wMonth - 1;
    tm2->tm_year = timex.wYear - 1900;
    return tm2;
}
