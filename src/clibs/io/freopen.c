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

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <share.h>
#include "libp.h"

FILE* _RTL_FUNC _wfreopen(const wchar_t* restrict name, const wchar_t* restrict mode, FILE* restrict stream)
{
    wchar_t buf[265];
    if (stream->token != FILTOK)
    {
        errno = _dos_errno = ENOENT;
        return 0;
    }
    if (!name)
        wcscpy(buf, stream->extended->name);
    else
        wcscpy(buf, name);
    free(stream->extended->name);
    __basefclose(stream, 0);
    stream->flags &= ~(_F_READ | _F_WRIT | _F_APPEND | _F_EOF | _F_ERR | _F_XEOF | _F_VBUF | _F_IN | _F_OUT);
    if (!__basefopen(buf, mode, stream, -1, SH_COMPAT))
    {
        return 0;
    }
    return stream;
}
FILE* _RTL_FUNC freopen(const char* restrict name, const char* restrict mode, FILE* restrict stream)
{
    wchar_t buf[260], buf1[64], *p = buf, *q = buf1;
    while (*name)
        *p++ = *name++;
    *p = *name; 
    while (*mode)
        *q++ = *mode++;
    *q = *mode; 

    return _wfreopen(buf, buf1, stream);
}
