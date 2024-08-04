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
#include <uchar.h>
#include "libp.h"

size_t c16rtomb(char* restrict s, char16_t c16, mbstate_t* restrict ps)
{
    char xx[10];

    if (s == NULL)
    {
        return c16rtomb(xx, u'\0', ps);
    }
    if (ps == NULL)
        ps = &__getRtlData()->wcrtomb_st;

    if (c16 < 0x80)
    {
        if (s != NULL)
            *s = (char)c16;
        return 1;
    }

    if ((unsigned)c16 <= 0x7ff)
    {
        if (s != NULL)
        {
            *s++ = 0xc0 + (c16 >> 6);
            *s++ = (c16 & 0x3f) | 0x80;
        }
        return 2;
    }
    if (s != NULL)
    {
        *s++ = 0xe0 + (c16 >> 12);
        *s++ = ((c16 >> 6) & 0x3f) | 0x80;
        *s++ = (c16 & 0x3f) | 0x80;
    }
    return 3;
}
