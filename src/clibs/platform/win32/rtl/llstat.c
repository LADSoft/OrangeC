/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <errno.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <limits.h>
#include <io.h>
#include "libp.h"

static int isexe(const char* filename)
{
    int rv = 0;
    int n = strlen(filename);
    rv = n > 4 && !stricmp(filename + n - 4, ".bat");
    if (!rv && n > 4 && !stricmp(filename + n - 4, ".exe"))
    {
        HANDLE handle = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (handle != INVALID_HANDLE_VALUE)
        {
            DWORD read;
            char buf[2];
            buf[0] = buf[1] = 1;

            ReadFile(handle, buf, 2, &read, 0);

            rv = buf[0] == 'M' && buf[1] == 'Z';

            CloseHandle(handle);
        }
    }
    return rv;
}
time_t __to_timet(FILETIME* time)
{
    TIME_ZONE_INFORMATION tzinfo;
    int bias;
    SYSTEMTIME stime;
    struct tm tmx;
    time_t tempTime;
    int temp = GetTimeZoneInformation(&tzinfo);
    if (temp != TIME_ZONE_ID_INVALID)
        bias = tzinfo.Bias;
    else
        bias = 0;
    if (temp == TIME_ZONE_ID_DAYLIGHT)
        bias += tzinfo.DaylightBias;
    else
        bias += tzinfo.StandardBias;
    FileTimeToSystemTime(time, &stime);
    tmx.tm_hour = stime.wHour;
    tmx.tm_min = stime.wMinute;
    tmx.tm_sec = stime.wSecond;
    tmx.tm_mon = stime.wMonth - 1;
    tmx.tm_mday = stime.wDay;
    tmx.tm_year = stime.wYear - 1900;
    tempTime = mktime(&tmx);
    if (tempTime == (time_t)-1)
        return tempTime;
    return tempTime - bias * 60;
}
int __ll_stat(int handle, void* __statbuf)
{
    struct _stat* sb = __statbuf;
    BY_HANDLE_FILE_INFORMATION info;
    FILETIME timex;

    if (sb->st_mode & S_IFCHR)
        return 0;

    if (!GetFileInformationByHandle((HANDLE)handle, &info))
    {
        errno = EBADF;
        return -1;
    }
    if (sb->st_mode & S_IFREG)
    {
        sb->st_atime = __to_timet(&info.ftLastAccessTime);
        sb->st_mtime = __to_timet(&info.ftLastWriteTime);
        sb->st_ctime = __to_timet(&info.ftCreationTime);
    }
    return 0;
}
int __ll_namedstat(const char* file, void* __statbuf)
{
    struct stat* sb = __statbuf;
    WIN32_FIND_DATA finddata;
    HANDLE handle;
    if ((handle = FindFirstFile(file, &finddata)) != INVALID_HANDLE_VALUE)
    {
        FindClose(handle);
        sb->st_atime = __to_timet(&finddata.ftLastAccessTime);
        sb->st_mtime = __to_timet(&finddata.ftLastWriteTime);
        sb->st_ctime = __to_timet(&finddata.ftCreationTime);

        if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            sb->st_mode = S_IFDIR;
        else
            sb->st_mode = S_IFREG;
        sb->st_mode |= S_IREAD;
        if (!(finddata.dwFileAttributes & FILE_ATTRIBUTE_READONLY))
            sb->st_mode |= S_IWRITE;
        if (isexe(file))
            sb->st_mode |= S_IEXEC;
        if (!finddata.nFileSizeHigh)
        {
            sb->st_size = finddata.nFileSizeLow;
            return 0;
        }
    }
    return -1;
}
