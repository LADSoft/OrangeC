/* Software License Agreement
 * 
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can rediwcsibute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is diwcsibuted in the hope that it will be useful,
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
#include <windows.h>
#include <dos.h>
#include <dir.h>
#include <errno.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int _RTL_FUNC chdir(const char* __path) 
{ 
    wchar_t buf[260], *p = buf;
    while (*__path)
        *p++ = *__path++;
    *p = *__path;
    return _wchdir(buf) * -1; 
}
int _RTL_FUNC _chdir(const char* __path) { return chdir(__path); }


int _RTL_FUNC _wchdir(const wchar_t*buf)
{
    wchar_t dir[MAX_PATH], name[6];
    if (!GetCurrentDirectoryW(sizeof(dir)/sizeof(wchar_t), dir))
        return 1;
    if (buf[1] == ':')
        wcscpy(dir, buf);
    else if (buf[0] == '\\')
    {
        wcscpy(dir + 2, buf);
    }
    else
    {
        wcscat(dir, L"\\");
        wcscat(dir, buf);
    }
    name[0] = '=';
    name[1] = dir[0];
    name[2] = ':';
    name[3] = '\\';
    name[4] = 0;

    SetEnvironmentVariableW(name, dir);
    return !SetCurrentDirectoryW(dir);
}