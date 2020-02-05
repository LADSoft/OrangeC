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
#include <string.h>
#include <time.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include "libp.h"

int _RTL_FUNC vfprintf(FILE *restrict stream, const char *restrict format, 
        va_list arglist)
{
    int written=0;
    int i = 0;
    if (stream->token != FILTOK) {
        errno = _dos_errno = ENOENT;
        return 0;
    }
    if (stream->extended->orient == __or_wide) {
        errno = EINVAL;
        return 0;
    }
    stream->extended->orient = __or_narrow;
    while (*format) {
        char *p = strchr(format, '%');
        if (!p)
            p = format + strlen(format);
        if (p != format)
        {
              if (fwrite(format,1,p - format,stream) != p-format)
                return EOF;
            written += p-format;
            format = p;
        }
        if (*format == '%') {
            if  (*(format+1)) {
                format = __onetostr(stream, format+1,((char **)arglist+i),&i, &written);
            }
            else
            {
                format++;
            }
        }
    }
    return written;
}
int _RTL_FUNC fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    return vfprintf(stream,format,((char *)&format+sizeof(char *)));
}