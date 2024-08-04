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

#include <ctype.h>
int _RTL_FUNC wmemicmp(const void* __s1, const void* __s2, size_t __n)
{
    const wchar_t* s1 = __s1;
    const wchar_t* s2 = __s2;
    while (__n)
    {
        if (towupper(*s1) != towupper(*s2))
            return towupper(*s1) > towupper(*s2) ? 1 : -1;
        s1++;
        s2++;
        __n--;
    }
    return 0;
}
int _RTL_FUNC _wmemicmp(const void* __s1, const void* __s2, size_t __n) { return wmemicmp(__s1, __s2, __n); }
