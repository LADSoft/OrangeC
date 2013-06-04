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
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define imin(x,y)  ((x) < 0 ? 0 : ((x) < (y) ? (x) : (y)))

int _RTL_FUNC vswprintf(wchar_t *restrict buf, size_t n, 
                            const wchar_t *restrict format, va_list arglist)
{
   wchar_t buffer[8192];
   FILE fil ;
   int written;
   memset(&fil,0,sizeof(fil)) ;
   fil.level = -sizeof(buffer);
   fil.flags = _F_OUT | _F_WRIT | _F_BUFFEREDSTRING ;
   fil.bsize = sizeof(buffer) - sizeof(wchar_t);
   fil.buffer = fil.curp = buffer;
   fil.token = FILTOK ;
   written = vfwprintf(&fil, format, arglist);
   if (buf && n)
   {
       int copied = imin(written, n-1);
       memcpy(buf, buffer, copied * sizeof(wchar_t));
       buf[copied] = 0;
   }
   return written;
}
int _RTL_FUNC swprintf(wchar_t *restrict buffer, size_t n, const wchar_t *restrict format, ...)
{
	return vswprintf(buffer,n,format,(((char *)&format) + sizeof(wchar_t **)));
}
int _RTL_FUNC vsnwprintf(wchar_t *restrict buf, size_t n, 
                            const wchar_t *restrict format, va_list arglist)
{
	return vswprintf(buf,n,format,(((char *)&format) + sizeof(wchar_t **)));
}
int _RTL_FUNC snwprintf(wchar_t *restrict buffer, size_t n, const wchar_t *restrict format, ...)
{
	return vswprintf(buffer,n,format,(((char *)&format) + sizeof(wchar_t **)));
}
int _RTL_FUNC _vsnwprintf(wchar_t *restrict buf, size_t n, 
                            const wchar_t *restrict format, va_list arglist)
{
	return vswprintf(buf,n,format,(((char *)&format) + sizeof(wchar_t **)));
}
int _RTL_FUNC _snwprintf(wchar_t *restrict buffer, size_t n, const wchar_t *restrict format, ...)
{
	return vswprintf(buffer,n,format,(((char *)&format) + sizeof(wchar_t **)));
}
