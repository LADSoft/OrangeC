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
#include <string.h>

void* _RTL_FUNC lfind(const void* key, void* base, size_t* num, size_t width, int (*compare)(const void* elem1, const void* elem2))
{
    int i;
    for (i = 0; i < *num; i++)
        if ((*compare)(key, (char*)base + i * width) == 0)
            return (void*)((char*)base + i * width);
    return 0;
}
void* _RTL_FUNC _lfind(const void* key, void* base, size_t* num, size_t width, int (*compare)(const void* elem1, const void* elem2))
{
    return lfind(key, base, num, width, compare);
}
void* _RTL_FUNC lsearch(const void* key, void* base, size_t* num, size_t width,
                        int (*compare)(const void* elem1, const void* elem2))
{
    void* rv = lfind(key, base, num, width, compare);
    char* pos;
    if (rv)
        return rv;

    pos = (char*)base + (*num) * width;
    memcpy(pos, key, width);
    (*num)++;
    return pos;
}
void* _RTL_FUNC _lsearch(const void* key, void* base, size_t* num, size_t width,
                         int (*compare)(const void* elem1, const void* elem2))
{
    return lsearch(key, base, num, width, compare);
}
