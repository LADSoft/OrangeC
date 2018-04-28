/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <errno.h>
#include "libp.h"

int _RTL_FUNC vfwprintf(FILE *restrict stream, const wchar_t *restrict format, 
        va_list arglist)
{
	int written=0;
	int i = 0;
	if (stream->token != FILTOK) {
		errno = _dos_errno = ENOENT;
		return 0;
	}
    if (stream->extended->orient == __or_narrow) {
        errno = EINVAL;
        return 0;
    }
    stream->extended->orient = __or_wide;
	while (*format) {
		short *p = wcschr(format, '%');
		if (!p)
			p = format + wcslen(format);
		if (p != format)
		{
	      	if (fwrite(format,2,p - format,stream) != p-format)
			    return EOF;
	        written += p-format;
			format = p;
		}
		if (*format == '%') {
            if  (*(format+1)) {
                format = __wonetostr(stream, format+1,((char **)arglist+i),&i, &written);
            }
			else
			{
                format++;
			}
        }
	}
	return written;
}
int _RTL_FUNC fwprintf(FILE *restrict stream, const wchar_t *restrict format, ...)
{
	return vfwprintf(stream,format,((char *)&format+sizeof(wchar_t *)));
}
