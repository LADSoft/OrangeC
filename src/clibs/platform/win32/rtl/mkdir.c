/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <errno.h>
#include <windows.h>
#include <dos.h>
#include <dir.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int _RTL_FUNC _wmkdir(const wchar_t* __path)
{
    if (!CreateDirectoryW(__path, 0))
    {
        errno = ENOENT;
        return -1;
    }
    return 0;
}
int _RTL_FUNC _mkdir(const char* __path)
{
    wchar_t buf[260], *p = buf;
    while (*__path)
        *p++ = *__path++;
    *p = *__path;
    return _wmkdir(buf);
}
int _RTL_FUNC mkdir(const char* __path, int __amode) 
{ 
    wchar_t buf[260], *p = buf;
    while (*__path)
        *p++ = *__path++;
    *p = *__path;
    int rv = _wmkdir(buf); 
    if (!rv)
        return __ll_chmod(buf, __amode);
    return rv;
}
