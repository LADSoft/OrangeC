/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include <string.h>
#include <ctype.h>

int _RTL_FUNC strnicmp(char *__s1, char *__s2, size_t __n)
{
   while (__n && *__s1 && *__s2) {
        if (toupper(*__s1) != toupper(*__s2))
            return toupper(*__s1) > toupper(*__s2) ? 1 : -1;
        __s1++;
        __s2++;
        __n--;
    }
    if (__n && toupper(*__s1) != toupper(*__s2))
        return toupper(*__s1) > toupper(*__s2) ? 1 : -1;
    return 0;
}
int _RTL_FUNC _strnicmp(char *__s1, char *__s2, size_t __n)
{
    return strnicmp(__s1, __s2, __n);
}
int _RTL_FUNC strncmpi(char *__s1, char *__s2, size_t __n)
{
    return strnicmp(__s1, __s2, __n);
}
int _RTL_FUNC _strncmpi(char *__s1, char *__s2, size_t __n)
{
    return strnicmp(__s1, __s2, __n);
}
