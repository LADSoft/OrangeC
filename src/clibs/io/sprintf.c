/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define imin(x, y) ((x) < 0 ? 0 : ((x) < (y) ? (x) : (y)))

int _RTL_FUNC vsnprintf(char* restrict buf, size_t n, const char* restrict format, va_list arglist)
{
    char buffer[8192];
    FILE fil;
    struct __file2 fil2;
    int written;
    memset(&fil, 0, sizeof(fil));
    memset(&fil2, 0, sizeof(fil2));
    fil.level = -sizeof(buffer);
    fil.flags = _F_OUT | _F_WRIT | _F_BUFFEREDSTRING;
    fil.bsize = sizeof(buffer) - 1;
    fil.buffer = fil.curp = buffer;
    fil.token = FILTOK;
    fil.extended = &fil2;
    written = vfprintf_unlocked(&fil, format, arglist);
    if (buf && n)
    {
        int copied = imin(written, n - 1);
        memcpy(buf, buffer, copied);
        buf[copied] = 0;
    }
    return written;
}
int _RTL_FUNC _vsnprintf(char* restrict buf, size_t n, const char* restrict format, va_list arglist)
{
    return vsnprintf(buf, n, format, arglist);
}
int _RTL_FUNC snprintf(char* restrict buf, size_t n, const char* restrict format, ...)
{
    return vsnprintf(buf, n, format, ((char*)&format + sizeof(char*)));
}
int _RTL_FUNC _snprintf(char* restrict buf, size_t n, const char* restrict format, ...)
{
    return vsnprintf(buf, n, format, ((char*)&format + sizeof(char*)));
}
int _RTL_FUNC vsprintf(char* restrict buffer, const char* restrict format, va_list arglist)
{
    return vsnprintf(buffer, INT_MAX, format, arglist);
}
int _RTL_FUNC sprintf(char* restrict buffer, const char* restrict format, ...)
{
    return vsnprintf(buffer, INT_MAX, format, (((char*)&format) + sizeof(char*)));
}
