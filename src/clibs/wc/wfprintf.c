/*
	Software License Agreement (BSD License)
	
	Copyright (c) 1997-2008, David Lindauer, (LADSoft).
	All rights reserved.
	
	Redistribution and use of this software in source and binary forms, with or without modification, are
	permitted provided that the following conditions are met:
	
	* Redistributions of source code must retain the above
	  copyright notice, this list of conditions and the
	  following disclaimer.
	
	* Redistributions in binary form must reproduce the above
	  copyright notice, this list of conditions and the
	  following disclaimer in the documentation and/or other
	  materials provided with the distribution.
	
	* Neither the name of LADSoft nor the names of its
	  contributors may be used to endorse or promote products
	  derived from this software without specific prior
	  written permission of LADSoft.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
	TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
