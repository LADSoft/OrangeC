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

#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include "libp.h"

int _RTL_FUNC vfwprintf(FILE* restrict stream, const wchar_t* restrict format, va_list arglist)
{
    flockfile(stream);
    int rv = vfwprintf_unlocked(stream, format, arglist);
    funlockfile(stream);
    return rv;
}
int _RTL_FUNC vfwprintf_unlocked(FILE* restrict stream, const wchar_t* restrict format, va_list arglist)
{
    int written = 0;
    int i = 0;
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return 0;
    }
    if (stream->flags & _F_BUFFEREDSTRING)
    {
        if (stream->extended->orient == __or_narrow)
        {
            errno = EINVAL;
            return 0;
        }
        stream->extended->orient = __or_wide;
    }
    else
    {
        if (stream->extended->orient == __or_wide)
        {
            errno = EINVAL;
            return 0;
        }
        stream->extended->orient = __or_narrow;
    }
    while (*format)
    {
        short* p = wcschr(format, '%');
        if (!p)
            p = format + wcslen(format);
        if (p != format)
        {
            for (; format < p; format++, written++)
            {
                if (stream->flags & _F_BUFFEREDSTRING)
                {
                    fputwc(*format, stream);
                }
                else
                {
                    char buf[32];
                    int n = wcrtomb(buf, *format, (mbstate_t*)stream->extended->mbstate);
                    if (n > 0)
                    {
                        buf[n] = 0;
                        if (fputs(buf, stream) < 0)
                            return EOF;
                    }
                }
            }
        }
        if (*format == '%')
        {
            if (*(format + 1))
            {
                format = __wonetostr(stream, format + 1, ((char**)arglist + i), &i, &written);
            }
            else
            {
                format++;
            }
        }
    }
    return written;
}
int _RTL_FUNC fwprintf(FILE* restrict stream, const wchar_t* restrict format, ...)
{
    return vfwprintf(stream, format, ((char*)&format + sizeof(wchar_t*)));
}
