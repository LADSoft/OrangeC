/* Software License Agreement
 * 
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#include <stdlib.h>
#include <windows.h>
#include <errno.h>
#include <io.h>

const char* realpath(const char* path, char* resolved_path)
{
    if (path == NULL)
    {
        errno = EINVAL;
        return NULL;
    }
    char int_path_lookup[MAX_PATH];
    int size = GetFullPathName(path, MAX_PATH - 1, int_path_lookup, NULL);
    if (size == 0)
    {
        errno = GetLastError();
        return NULL;
    }
    if (size >= MAX_PATH)
    {
        errno = ENAMETOOLONG;
        return NULL;
    }
    if (access(path, R_OK))
    {
        errno = ENOENT;
        return NULL;
    }
    if (resolved_path == NULL)
    {
        resolved_path = malloc(size + 1);
    }
    strcpy(resolved_path, int_path_lookup);
    return resolved_path;
}

const char* canonicalize_file_name(const char* path) { return realpath(path, NULL); }