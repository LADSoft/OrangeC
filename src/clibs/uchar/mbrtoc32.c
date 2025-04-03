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

#include <errno.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <uchar.h>
#include "libp.h"

size_t mbrtoc32(char32_t* restrict pc32, const char* restrict s, size_t n, mbstate_t* restrict p)
{
    size_t used = 0;
    unsigned char b;

    if (s == NULL)
    {
        return mbrtoc32(NULL, "", 1, p);
    }
    if (p == NULL)
        p = &__getRtlData()->mbrtowc_st;

    if (n <= 0)
        return (size_t)-2;

    if (p->left == 0)
    {
        b = (unsigned char)*s++;
        used++;
        if (b < 0x80)
        {
            if (pc32)
                *pc32 = (wchar_t)b;
            return b ? 1 : 0;
        }

        if ((b & 0xc0) == 0x80 || b == 0xfe || b == 0xff)
        {
            errno = EILSEQ;
            return (size_t)-1;
        }
        b <<= 1;
        while (b & 0x80)
        {
            p->left++;
            b <<= 1;
        }
        p->value = b >> (p->left + 1);
    }
    while (used < n)
    {
        b = (unsigned char)*s++;
        used++;
        if ((b & 0xc0) != 0x80)
        {
            errno = EILSEQ;
            return (size_t)-1;
        }

        p->value <<= 6;
        p->value += b & 0x3f;

        if (--p->left == 0)
        {
            if (pc32 != NULL)
                *pc32 = (char32_t)p->value;
            return p->value ? used : 0;
        }
    }

    return (size_t)-2;
}
