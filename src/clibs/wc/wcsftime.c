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

#include <time.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include "_locale.h"
#include <limits.h>

static int weekbasedoffset(struct tm* t)
{
    struct tm t2 = *t;
    time_t tt;
    int rv = 0;
    t2.tm_mon = 0;  // january 1
    t2.tm_mday = 1;
    tt = mktime(&t2);
    if (tt == -1)
        return INT_MAX;
    if (t2.tm_wday > 0 && t2.tm_wday < 5)
        rv++;
    rv += t->tm_yday / 7 + ((t->tm_yday % 7) > ((t->tm_wday + 6) % 7));
    return rv;
}
static int weekbasedyear(struct tm* t)
{
    int n = weekbasedoffset(t);
    if (n == INT_MAX)
        return 0;
    if (n < 1)
        return t->tm_year + 1900 - 1;
    if (n > 52)
    {
        struct tm t2 = *t;
        time_t tt;
        t2.tm_mon = 11;  // december 31
        t2.tm_mday = 31;
        tt = mktime(&t2);
        if (tt == -1)
            return 0;
        if (t2.tm_wday < 4)
            return t->tm_year + 1900 + 1;
    }
    return t->tm_year + 1900;
}
static int weekbasedyearweek(struct tm* t)
{
    int n = weekbasedoffset(t);
    if (n == INT_MAX)
        return 0;
    if (n < 1)
        return 53;
    if (n > 52)
    {
        struct tm t2 = *t;
        time_t tt;
        t2.tm_mon = 11;  // december 31
        t2.tm_mday = 31;
        tt = mktime(&t2);
        if (tt == -1)
            return 0;
        if (t2.tm_wday < 4)
            return 1;
    }
    return n;
}
static int weeknum(struct tm* t, int startday)
{
    struct tm t2 = *t;
    int rv;
    time_t tt;
    tt = mktime(&t2);
    if (tt == -1)
        return 0;
    rv = t2.tm_yday / 7;
    if (!startday)
        rv += ((t2.tm_yday % 7) >= t2.tm_wday);
    else
        rv += ((t2.tm_yday % 7) >= ((t2.tm_wday + 6) % 7));
    return rv;
}
static int pstr(wchar_t* str, char* astr, int maxsize, int mode, int* cursize)
{
    int l;
    if (*cursize > maxsize)
        return 1;
    l = strlen(astr);
    str += *cursize;
    while (*cursize < maxsize - 1 && l)
    {
        *str++ = *astr++;
        (*cursize)++;
        l--;
    }
    *str = 0;
    if (l)
        return 1;
    return 0;
}
static int pchar(wchar_t* str, char astr, int maxsize, int mode, int* cursize)
{
    if (*cursize > maxsize)
        return 1;
    str += *cursize;
    *str++ = astr;
    (*cursize)++;
    *str = 0;
    return 0;
}
static int pnum(wchar_t* str, int num, int fwidth, int maxsize, int mode, int* cursize)
{
    char buf[11], *p = &buf[9], *nm;
    int i;
    TIME_DATA* td = __locale_data[LC_TIME];
    nm = td->ths;
    buf[0] = 0;
    if (mode == 'O')
    {
        for (i = 0; i < num; i++)
        {
            if (*nm == 0)
                break;
            nm += strlen(nm);
        }
        if (*nm)
            return (pstr(str, nm, maxsize, mode, cursize));
    }
    for (i = 0; i < 10; i++)
        buf[i] = '0';
    buf[10] = 0;
    while (num)
    {
        *p-- = (char)(num % 10) + '0';
        num = num / 10;
    }
    return (pstr(str, buf + 10 - fwidth, maxsize, mode, cursize));
}
static int basic_format(wchar_t* restrict format, const struct tm* restrict t, wchar_t* restrict str, size_t maxsize, int mode,
                        int* restrict cursize)
{
    int temp;
    TIME_DATA* td = __locale_data[LC_TIME];
    switch (*format)
    {
        case 'a':
            if (pstr(str, td->shortdays[t->tm_wday], maxsize, mode, cursize))
                return 0;
            break;
        case 'A':
            if (pstr(str, td->longdays[t->tm_wday], maxsize, mode, cursize))
                return 0;
            break;
        case 'b':
        case 'h':
            if (pstr(str, td->shortmonths[t->tm_mon], maxsize, mode, cursize))
                return 0;
            break;
        case 'B':
            if (pstr(str, td->longmonths[t->tm_mon], maxsize, mode, cursize))
                return 0;
            break;
        case 'C':
            if (pnum(str, (t->tm_year + 1900) / 100, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'd':
            if (pnum(str, t->tm_mday, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'e':
            if (t->tm_mday < 10)
            {
                if (pstr(str, " ", maxsize, mode, cursize))
                    return 0;
                if (pnum(str, t->tm_mday, 1, maxsize, mode, cursize))
                    return 0;
            }
            else if (pnum(str, t->tm_mday, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'g':
            if (pnum(str, weekbasedyear(t) % 100, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'G':
            if (pnum(str, weekbasedyear(t), 4, maxsize, mode, cursize))
                return 0;
            break;
        case 'H':
            if (pnum(str, t->tm_hour, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'I':
        case 'i':
            temp = t->tm_hour % 12;
            if (temp == 0)
                temp = 12;
            if (pnum(str, temp, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'j':
            if (pnum(str, t->tm_yday, 3, maxsize, mode, cursize))
                return 0;
            break;
        case 'm':
            if (pnum(str, t->tm_mon + 1, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'M':
            if (pnum(str, t->tm_min, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'n':
            if (pchar(str, '\n', maxsize, mode, cursize))
                return 0;
            break;
        case 'P':
        case 'p':
        {
            char buf[sizeof(td->am)];
            strcpy(buf, td->am + (t->tm_hour / 12) * sizeof(td->am));
            if (*format == 'p')
                strupr(buf);
            else
                strlwr(buf);
            if (pstr(str, buf, maxsize, mode, cursize))
                return 0;
        }
        break;
        case 'S':
            if (pnum(str, t->tm_sec, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 't':
            if (pchar(str, '\t', maxsize, mode, cursize))
                return 0;
            break;
        case 'U':
            if (pnum(str, weeknum(t, 0), 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'u':
            if (t->tm_wday == 0)
                temp = 6;
            else
                temp = t->tm_wday - 1;
            if (pnum(str, temp, 1, maxsize, mode, cursize))
                return 0;
            break;
        case 'V':
            if (pnum(str, weekbasedyearweek(t), 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'w':
            if (pnum(str, t->tm_wday, 1, maxsize, mode, cursize))
                return 0;
            break;
        case 'W':
            if (pnum(str, weeknum(t, 1), 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'y':
            if (pnum(str, t->tm_year % 100, 2, maxsize, mode, cursize))
                return 0;
            break;
        case 'Y':
            if (pnum(str, t->tm_year + 1900, 4, maxsize, mode, cursize))
                return 0;
            break;
        case 'z':
            if (pstr(str, __getTzName(), maxsize, mode, cursize))
                return 0;
            break;
        case 'Z':
            temp = *__getTimezone() / 60;
            if (temp > 0)
                if (pchar(str, '-', maxsize, mode, cursize))
                    return 0;
            if (pnum(str, temp / 60, 2, maxsize, mode, cursize))
                return 0;
            if (pnum(str, temp % 60, 2, maxsize, mode, cursize))
                return 0;
            break;
        default:  // also catch %%
            if (pchar(str, *format, maxsize, mode, cursize))
                return 0;
            break;
    }
    return 1;
}
static int advanced_format(wchar_t* restrict format, const struct tm* restrict t, wchar_t* restrict str, size_t maxsize,
                           int* restrict cursize)
{
    int i, size, xsize;
    int mode, index;
    TIME_DATA* td = __locale_data[LC_TIME];
    for (i = 0; i < wcslen(format); i++)
    {
        if (format[i] == '%')
        {
            if (!format[++i])
                return cursize;
            mode = 0;
            if (format[i] == 'O' || format[i] == 'E')
            {
                mode = format[i];
                if (!format[++i])
                    return cursize;
            }
            switch (format[i])
            {
                case 'c':
                    index = mode == 'E';
                    if (!strcmp(td->qualname, "C"))
                    {
                        if (!advanced_format(L"%A %B %d %T %Y", t, str, maxsize, cursize))
                            return 0;
                    }
                    else
                    {
                        int l, i, c;
                        wchar_t buf[32], wc;
                        mbstate_t st;
                        memset(&st, 0, sizeof(st));
                        for (l = 1, i = 0; l > 0; i++)
                        {
                            c = td->dates[index][i];
                            l = mbrtowc(&wc, &c, 1, &st);
                            if (l >= 0)
                                buf[i] = wc;
                        }
                        if (!advanced_format(buf, t, str, maxsize, cursize))
                            return 0;
                        if (pchar(str, ' ', maxsize, mode, cursize))
                            return 0;
                        memset(&st, 0, sizeof(st));
                        for (l = 1, i = 0; l > 0; i++)
                        {
                            c = td->times[index][i];
                            l = mbrtowc(&wc, &c, 1, &st);
                            if (l >= 0)
                                buf[i] = wc;
                        }
                        if (!advanced_format(buf, t, str, maxsize, cursize))
                            return 0;
                    }
                    break;
                case 'D':
                    if (!advanced_format(L"%m/%d/%y", t, str, maxsize, cursize))
                        return 0;
                    break;
                case 'F':
                    if (!advanced_format(L"%Y-%m-%d", t, str, maxsize, cursize))
                        return 0;
                    break;
                case 'r':
                {
                    int l, i, c;
                    wchar_t buf[32], wc;
                    mbstate_t st;
                    memset(&st, 0, sizeof(st));
                    for (l = 1, i = 0; l > 0; i++)
                    {
                        c = td->times[1][i];
                        l = mbrtowc(&wc, &c, 1, &st);
                        if (l >= 0)
                            buf[i] = wc;
                    }
                    if (!advanced_format(buf, t, str, maxsize, cursize))
                        return 0;
                }
                break;
                case 'R':
                    if (!advanced_format(L"%H:%M", t, str, maxsize, cursize))
                        return 0;
                    break;
                case 'T':
                    if (!advanced_format(L"%H:%M:%S", t, str, maxsize, cursize))
                        return 0;
                    break;
                case 'x':
                {
                    int l, i, c;
                    wchar_t buf[32], wc;
                    mbstate_t st;
                    index = mode == 'E';
                    memset(&st, 0, sizeof(st));
                    for (l = 1, i = 0; l > 0; i++)
                    {
                        c = td->dates[index][i];
                        l = mbrtowc(&wc, &c, 1, &st);
                        if (l >= 0)
                            buf[i] = wc;
                    }
                    if (!advanced_format(buf, t, str, maxsize, cursize))
                        return 0;
                }
                break;
                case 'X':
                {
                    int l, i, c;
                    wchar_t buf[32], wc;
                    mbstate_t st;
                    index = mode == 'E';
                    memset(&st, 0, sizeof(st));
                    for (l = 1, i = 0; l > 0; i++)
                    {
                        c = td->times[index][i];
                        l = mbrtowc(&wc, &c, 1, &st);
                        if (l >= 0)
                            buf[i] = wc;
                    }
                    if (!advanced_format(buf, t, str, maxsize, cursize))
                        return 0;
                }
                break;
                default:
                    if (!basic_format(format + i, t, str, maxsize, mode, cursize))
                        return 0;
                    break;
            }
        }
        else
        {
            if (pchar(str, format[i], maxsize, mode, cursize))
                return 0;
        }
    }
    return 1;
}
size_t _RTL_FUNC wcsftime(wchar_t* restrict str, size_t maxsize, const wchar_t* restrict format, const struct tm* restrict t)
{
    int rv = 0;
    str[0] = 0;
    if (!advanced_format(format, t, str, maxsize, &rv))
        return 0;
    return rv;
}