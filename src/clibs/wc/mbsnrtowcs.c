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

#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

size_t mbsnrtowcs(wchar_t* restrict dst, const char** restrict src, size_t nms, size_t len, mbstate_t* restrict p)
{
    unsigned char b;
    size_t used = 0;
    const char* r = *src;

    if (!p)
        p = &__getRtlData()->mbsrtowcs_st;

    while (used < len && p->left <= nms)
    {
        b = (unsigned char)*r++;
        nms--;
        if (p->left == 0)
        {
            if (!nms)
            {
                *src = r - 1;
                return used;
            }
            if (dst)
                *dst++ = (wchar_t)b;
            if (b == L'\0')
            {
                *src = NULL;
                return used;
            }
            used++;
        }
    }

    *src = r;

    return used;
}
