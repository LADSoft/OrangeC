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

#include <io.h>
#include <sys/stat.h>
#include <errno.h>
#include <wchar.h>
int _RTL_FUNC _waccess(const wchar_t* __file, int __level)
{
    wchar_t buf[260];
    struct stat stat_st;
    int mode = 0;
    if (_wstat(__file, &stat_st) == -1)
    {
        errno = ENOFILE;
        // at this point we MAY have an invalid path, so, check the directory.
        // we are going backwards until nothing is left or we encounter an error,
        // possible errors are ENOENT (path does not exist) or ENOTDIR) path exists
        // but is not a directory
        wcscpy(buf, __file);
        wchar_t* p;
        do
        {
            p = wcsrchr(buf, '\\');
            if (!p)
                p = wcsrchr(buf, '/');
            if (p)
            {
                *p = 0;
                if (_wstat(buf, &stat_st) == -1)
                    errno = ENOENT;
                else if (!S_ISDIR(stat_st.st_mode))
                    errno = ENOTDIR;
            }
        } while (p && errno != ENOTDIR);
        return -1;
    }
    if (__level == F_OK || S_ISDIR(stat_st.st_mode))
        return 0;
    if (__level & R_OK)
        mode |= S_IRUSR;
    if (__level & W_OK)
        mode |= S_IWUSR;
    if (__level & X_OK)
        mode |= S_IXUSR;
    if (stat_st.st_mode & mode)
        return 0;
    errno = ENOFILE;
    return -1;
}
int _RTL_FUNC access(const char* __file, int __level)
{
    wchar_t buf[260], *p = buf;
    while (*__file)
        *p++ = *__file++;
    *p = *__file;
    return _waccess(buf, __level);
}
int _RTL_FUNC _access(const char* __file, int __level) { return access(__file, __level); }
