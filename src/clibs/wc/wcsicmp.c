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

#include <string.h>
#include <wctype.h>
int _RTL_FUNC wcsicmp(const wchar_t* __s1, const wchar_t* __s2)
{
    while (*__s1)
    {
        if (towupper(*__s1) != towupper(*__s2))
            return towupper(*__s1) > towupper(*__s2) ? 1 : -1;
        __s1++;
        __s2++;
    }
    if (*__s2)
        return -1;
    return 0;
}
int _RTL_FUNC _wcsicmp(const wchar_t* __s1, const wchar_t* __s2) { return wcsicmp(__s1, __s2); }