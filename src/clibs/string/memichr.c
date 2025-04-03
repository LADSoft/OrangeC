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
void* _RTL_FUNC memichr(const void* __s1, int ch, size_t __n)
{
    const char* s1 = __s1;
    ch = toupper(ch);
    while (__n)
    {
        if (toupper(*s1) == ch)
            return s1;
        s1++;
        __n--;
    }
    return NULL;
}
void* _RTL_FUNC _memichr(const void* __s1, int ch, size_t __n) { return memichr(__s1, ch, __n); }
