/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include <ctype.h>
char* _RTL_FUNC stristr(const char* __s1, const char *__s2)
{
    int n = strlen(__s1);
    int n1 = strlen(__s2);
    if (n1 > n)
        return NULL;
    int start = toupper(*__s2);
    while (n)
    {
        if (toupper(*__s1) == start)
        {
            if (!strnicmp(__s1, __s2, n1))
                return __s1;
        }
        __s1++;
        n--;
    }
    return NULL;
}
char* _RTL_FUNC _stristr(const char* __s1, const char *__s2) { return stristr(__s1, __s2); }
