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
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <io.h>
#include "libp.h"

time_t __to_timet(FILETIME* time);

static void format_finddata(struct _finddata_t* buf, LPWIN32_FIND_DATA data)
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
    buf->time_access = __to_timet(&data->ftLastAccessTime);
    buf->time_write = __to_timet(&data->ftLastWriteTime);
    buf->time_create = __to_timet(&data->ftCreationTime);
    if (data->nFileSizeHigh)
        *(DWORD*)(&buf->size) = 0xffffffff;
    else
        *(DWORD*)(&buf->size) = data->nFileSizeLow;
    strcpy(buf->name, data->cFileName);
}
long _RTL_FUNC _findfirst(const char* string, struct _finddata_t* buf)
{
    WIN32_FIND_DATA find_data;
    HANDLE val;
    if (!buf)
    {
        errno = EINVAL;
        return -1;
    }
    val = FindFirstFile(string, &find_data);
    memset(buf, 0, sizeof(*buf));
    if (val == INVALID_HANDLE_VALUE)
    {
        errno = ENOENT;
        return -1;
    }
    format_finddata(buf, &find_data);
    return (int)val;
}
int _RTL_FUNC _findnext(long handle, struct _finddata_t* buf)
{
    WIN32_FIND_DATA find_data;
    if (!buf)
    {
        errno = EINVAL;
        return -1;
    }
    if (!FindNextFile((HANDLE)handle, &find_data))
    {
        errno = ENOENT;
        return -1;
    }
    format_finddata(buf, &find_data);

    return 0;
}
int _RTL_FUNC _findclose(long handle)
{
    FindClose((HANDLE)handle);
    return 0;
}
