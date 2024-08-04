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

#include <stdlib.h>

void _RTL_FUNC swab(char* __from, char* __to, int bytes)
{
    int i;
    bytes &= ~1;
    for (i = 0; i < bytes; i += 2)
    {
        int n1 = __from[i];
        int n2 = __from[i + 1];
        __to[i] = n2;
        __to[i + 1] = n1;
    }
}
void _RTL_FUNC _swab(char* __from, char* __to, int bytes) { swab(__from, __to, bytes); }
