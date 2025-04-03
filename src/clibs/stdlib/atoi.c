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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#define nextchar          \
    {                     \
        *ch = fgetc_unlocked(fil); \
        (*chars)++;       \
        if (!count)       \
            return val;   \
        else              \
            count--;      \
    }

LLONG_TYPE __xstrtoimax(FILE* fil, int count, int* ch, int* chars, int radix, unsigned LLONG_TYPE max, int full, int errmode)
{
    int sign = 0;
    unsigned LLONG_TYPE val = 0;
    unsigned LLONG_TYPE imax;
    int lim;
    int temp;
    int err = 0;
    char* bpos = fil->curp;
    int ch1, i1;
    count--;
    if (radix > 36 || radix < 0)
        return 0;

    while (isspace(*ch))
        nextchar;

    ch1 = *ch;
    i1 = *chars;
    if (*ch == '-')
    {
        sign++;
        nextchar;
    }
    else if (*ch == '+')
        nextchar;
    if (*ch == '0')
    {
        if (!radix)
        {
            radix = 8;
            nextchar;
            if (*ch == 'x' || *ch == 'X')
            {
                radix = 16;
                nextchar;
                if (!isxdigit(*ch))
                {
                    fil->curp = bpos;
                    return 0;
                }
            }
            if (*ch == 'b' || *ch == 'B')
            {
                radix = 2;
                nextchar;
                if (*ch != '0' && *ch != '1')
                {
                    fil->curp = bpos;
                    return 0;
                }
            }
        }
        else if (radix == 16)
        {
            nextchar;
            if (*(ch) == 'x' || *(ch) == 'X')
            {
                nextchar;
                if (!isxdigit(*ch))
                {
                    fil->curp = bpos;
                    return 0;
                }
            }
        }
        else if (radix == 2)
        {
            nextchar;
            if (*(ch) == 'b' || *(ch) == 'B')
            {
                nextchar;
                if (*ch != '0' && *ch != '1')
                {
                    fil->curp = bpos;
                    return 0;
                }
            }
        }
    }
    if (!radix)
        radix = 10;
    lim = (ULLONG_MAX % radix);
    imax = ULLONG_MAX / radix;
    while (isalnum(*ch))
    {
        unsigned temp = toupper(*ch) - '0';
        if (temp >= 10)
            temp -= 7;
        if (temp >= radix)
            break;
        else if (val > imax || val == imax && temp > lim)
        {
            err = 1;
        }
        val *= radix;
        val += temp;
        nextchar;
    }
    if (*chars - i1 == 1 && (ch1 == '+' || ch1 == '-'))
    {
        fil->curp = bpos;
        return 0;
    }
    switch (errmode)
    {
        case STRT_ERR_RET_CV:
            if (sign)
                val = -val;
            break;
        case STRT_ERR_RET_SIGNED:
            if (err || val > max && (!sign || val != max))
            {
                errno = ERANGE;
                if (sign)
                    val = -(max + 1);
                else
                    val = max;
            }
            else if (sign)
                val = -val;
            break;
        case STRT_ERR_RET_UNSIGNED:
            if (err || val > max)
            {
                errno = ERANGE;
                val = max;
            }
            else if (sign)
                val = -val;
            break;
    }
    return val;
}
static LLONG_TYPE __xstrtol(const char* buf, int width, char** endptr, int radix, unsigned LLONG_TYPE max, int full, int errmode)
{
    LLONG_TYPE rv;
    int ch;
    FILE fil;
    struct __file2 fil2;
    int chars = 0;
    memset(&fil, 0, sizeof(fil));
    memset(&fil2, 0, sizeof(fil2));
    fil.level = strlen(buf) + 2;
    fil.flags = _F_IN | _F_READ | _F_BUFFEREDSTRING;
    fil.bsize = strlen(buf);
    fil.buffer = fil.curp = buf;
    fil.token = FILTOK;
    fil.extended = &fil2;
    ch = fgetc_unlocked(&fil);
    rv = __xstrtoimax(&fil, width, &ch, &chars, radix, max, full, errmode);
    if (endptr)
    {
        *endptr = fil.curp;
        if (fil.curp != fil.buffer)
            (*endptr)--;
    }
    return rv;
}
LLONG_TYPE _RTL_FUNC strtoimax(const char* restrict nptr, char** restrict endptr, int base)
{
    return __xstrtol(nptr, INT_MAX, endptr, base, LLONG_MAX, 1, STRT_ERR_RET_SIGNED);
}
unsigned LLONG_TYPE _RTL_FUNC strtoumax(const char* restrict nptr, char** restrict endptr, int base)
{
    return __xstrtol(nptr, INT_MAX, endptr, base, ULLONG_MAX, 1, STRT_ERR_RET_UNSIGNED);
}
LLONG_TYPE _RTL_FUNC strtoll(const char* restrict nptr, char** restrict endptr, int base)
{
    return __xstrtol(nptr, INT_MAX, endptr, base, LLONG_MAX, 1, STRT_ERR_RET_SIGNED);
}
unsigned LLONG_TYPE _RTL_FUNC strtoull(const char* restrict nptr, char** restrict endptr, int base)
{
    return __xstrtol(nptr, INT_MAX, endptr, base, ULLONG_MAX, 1, STRT_ERR_RET_UNSIGNED);
}
long _RTL_FUNC strtol(const char* restrict s, char** restrict endptr, int radix)
{
    return (long)__xstrtol(s, INT_MAX, endptr, radix, LONG_MAX, 1, STRT_ERR_RET_SIGNED);
}
unsigned long _RTL_FUNC strtoul(const char* restrict s, char** restrict endptr, int radix)
{
    return (unsigned long)__xstrtol(s, INT_MAX, endptr, radix, ULONG_MAX, 1, STRT_ERR_RET_UNSIGNED);
}
LLONG_TYPE _RTL_FUNC _IMPORT _strtoi64(const char* restrict __s, char** restrict __endptr, int __radix)
{
    return strtoll(__s, __endptr, __radix);
}
LLONG_TYPE _RTL_FUNC atoll(const char* s) { return (LLONG_TYPE)__xstrtol(s, INT_MAX, 0, 10, LLONG_MAX, 1, STRT_ERR_RET_CV); }
__int64 _RTL_FUNC _atoi64(const char* s) { return (__int64)__xstrtol(s, INT_MAX, 0, 10, _I64_MAX, 1, STRT_ERR_RET_CV); }
long _RTL_FUNC atol(const char* s) { return (long)__xstrtol(s, INT_MAX, 0, 10, LONG_MAX, 1, STRT_ERR_RET_CV); }
int _RTL_FUNC atoi(const char* s) { return (int)__xstrtol(s, INT_MAX, 0, 10, INT_MAX, 1, STRT_ERR_RET_CV); }
