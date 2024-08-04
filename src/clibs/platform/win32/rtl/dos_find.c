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

#include <errno.h>
#include <windows.h>
#include <dos.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

void __convert_file_time(LPFILETIME timet, unsigned* xtime, unsigned* xdate)
{
    FILETIME timex;
    FileTimeToLocalFileTime(timet, &timex);
    *xtime = 0;
    *xdate = 0;
    FileTimeToDosDateTime(&timex, xdate, xtime);
}
static void format_finddata(struct find_t* buf, LPWIN32_FIND_DATA data)
{
    buf->attrib = 0;
    if (data->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
        buf->attrib |= _A_ARCH;
    if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        buf->attrib |= _A_SUBDIR;
    if (data->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        buf->attrib |= _A_HIDDEN;
    if (data->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        buf->attrib |= _A_RDONLY;
    if (data->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
        buf->attrib |= _A_SYSTEM;
    __convert_file_time(&data->ftLastAccessTime, &buf->wr_time, &buf->wr_date);
    if (data->nFileSizeHigh)
        *(DWORD*)(&buf->size) = 0xffffffff;
    else
        *(DWORD*)(&buf->size) = data->nFileSizeLow;
    strcpy(buf->name, data->cFileName);
}
static BOOL matchedattr(DWORD attr, DWORD mattr)
{
    if ((attr & mattr) || !attr && (!mattr || mattr == _A_ARCH))
        return 1;
    return 0;
}
int _RTL_FUNC _dos_findfirst(char* string, int attr, struct find_t* buf)
{
    WIN32_FIND_DATA find_data;
    HANDLE val = FindFirstFile(string, &find_data);
    memset(buf, 0, sizeof(*buf));
    if (val == INVALID_HANDLE_VALUE)
        return 1;
    *(HANDLE*)(buf->reserved) = val;
    *(DWORD*)(buf->reserved + 4) = attr;
    format_finddata(buf, &find_data);
    if (matchedattr(attr, buf->attrib))
        return 0;
    else
        return _dos_findnext(buf);
}
int _RTL_FUNC _dos_findnext(struct find_t* buf)
{
    WIN32_FIND_DATA find_data;
    HANDLE val = *(HANDLE*)(buf->reserved);
    DWORD attr = *(DWORD*)(buf->reserved + 4);
    do
    {
        if (!FindNextFile(val, &find_data))
        {
            FindClose(val);
            return 1;
        }
        format_finddata(buf, &find_data);
    } while (!matchedattr(attr, buf->attrib));

    return 0;
}
