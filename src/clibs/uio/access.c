/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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

#include <io.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
int _RTL_FUNC access(const char* __file, int __level)
{
    char buf[260];
    struct stat stat_st;
    int mode = 0;
    if (stat(__file, &stat_st) == -1)
    {
        errno = ENOFILE;
        // at this point we MAY have an invalid path, so, check the directory.
        // we are going backwards until nothing is left or we encounter an error,
        // possible errors are ENOENT (path does not exist) or ENOTDIR) path exists
        // but is not a directory
        strcpy(buf, __file);
        char* p;
        do
        {
            p = strrchr(buf, '\\');
            if (!p)
                p = strrchr(buf, '/');
            if (p)
            {
                *p = 0;
                if (stat(buf, &stat_st) == -1)
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
int _RTL_FUNC _access(const char* __file, int __level) { return access(__file, __level); }
