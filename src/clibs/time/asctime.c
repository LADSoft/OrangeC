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
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <string.h>
#include "_locale.h"

extern LOCALE_HEADER _C_locale_data;
static int pnum(char *str, int num, int fwidth, int zerofil)
{
    int i;
    char *p = str + fwidth-1;
    if (zerofil)
        memset(str,'0',fwidth);
    while (num && fwidth--) {
        *p-- = (char)(num %10)+'0';
        num = num / 10;
    }
}

char *_RTL_FUNC asctime(struct tm *timeptr)
{
    char *rv = __getRtlData()->asctime_buf;
    TIME_DATA *td = _C_locale_data.td;
    memset(rv,' ',25);
    memcpy(rv,td->shortdays[timeptr->tm_wday],3);
    memcpy(rv+4,td->shortmonths[timeptr->tm_mon],3);
    pnum(rv+7,timeptr->tm_mday,3,0);
    pnum(rv+11,timeptr->tm_hour,2,1);
    rv[13] = ':';
    pnum(rv+14,timeptr->tm_min,2,1);
    rv[16] = ':';
    pnum(rv+17,timeptr->tm_sec,2,1);
    pnum(rv+20,timeptr->tm_year + 1900,4,1);
    rv[24] = '\n';
    rv[25] = 0;
    return rv;
}