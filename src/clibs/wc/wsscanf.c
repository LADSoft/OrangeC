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
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <libp.h>
#include <limits.h>
#include <float.h>
#ifdef _i386_
#define USE_FLOAT
#endif


int _RTL_FUNC wscanf(const wchar_t *format, ...)
{
   return __wscanf(stdin,format,((char *)&format+sizeof(wchar_t *)));
}

int _RTL_FUNC vswscanf(wchar_t *restrict buf, const wchar_t *restrict format, 
            va_list list)
{
   int rv ;
   FILE fil ;
   memset(&fil,0,sizeof(fil)) ;
   fil.level = wcslen(buf)*sizeof(wchar_t) + sizeof(wchar_t) ;
   fil.flags = _F_IN | _F_READ | _F_BUFFEREDSTRING ;
   fil.bsize = wcslen(buf)*2 ;
   fil.buffer = fil.curp = buf ;
   fil.token = FILTOK ;
   return __wscanf(&fil,format,list);
}
int _RTL_FUNC swscanf(wchar_t *restrict buf, const wchar_t *restrict format, ...)
{
   return vswscanf(buf,format,(((char *)&format)+sizeof(wchar_t *)));
}
int vfwscanf(FILE *restrict fil, const wchar_t *restrict format, va_list arglist)
{
    return __wscanf(fil,format,arglist);
}
int fwscanf(FILE *restrict fil, const wchar_t *restrict format, ...)
{
   return __wscanf(fil,format,((char *)&format+sizeof(wchar_t *)));
}