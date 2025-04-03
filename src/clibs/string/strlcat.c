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

#include <string.h>
#include <sys/types.h>

size_t _RTL_FUNC strlcat(char* dst, const char* src, size_t n)
{
    size_t ldst = strlen(dst), lsrc = strlen(src), lcpy;
    if (n > 0 && ldst < n - 1)
    {
        lcpy = (lsrc >= n - ldst) ? n - ldst - 1 : lsrc;
        memcpy(dst + ldst, src, lcpy);
        dst[ldst + lcpy] = '\0';
    }
    return ldst + lsrc;
}
