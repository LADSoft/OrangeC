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
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <sys\stat.h>

static void ConvertBrokendownTime(FILETIME* ft, struct tm* mt)
{
    SYSTEMTIME syst;
    FILETIME ift;
    memset(&syst, 0, sizeof(syst));
    syst.wYear = mt->tm_year + 1900;
    syst.wMonth = mt->tm_mon + 1;
    syst.wDay = mt->tm_mday;
    syst.wHour = mt->tm_hour;
    syst.wMinute = mt->tm_min;
    syst.wSecond = mt->tm_sec;
    SystemTimeToFileTime(&syst, &ift);
    LocalFileTimeToFileTime(&ift, ft);
}
int __ll_utime(int handle, struct tm* access, struct tm* modify)
{
    FILETIME timea, timem;
    ConvertBrokendownTime(&timea, access);
    ConvertBrokendownTime(&timem, modify);
    return SetFileTime((HANDLE)handle, 0, &timea, &timem) ? 0 : -1;
}
