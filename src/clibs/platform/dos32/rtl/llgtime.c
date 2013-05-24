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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <dpmi.h>
#include <errno.h>
 #include "llp.h"

#define FROMBCD(x) (((x)>>4)*10 +((x)& 0xf))
#define TOBCD(x) ((((x)/10)<<4) + ((x) % 10))
struct tm *__ll_gettime(struct tm *tm2)
{
    DPMI_REGS regs;
    memset(tm2,0,sizeof(struct tm));
    regs.b.ah = 2;
    __realint(0x1a,&regs);

  tm2->tm_sec = FROMBCD(regs.b.dh);
  tm2->tm_min = FROMBCD(regs.b.cl);
  tm2->tm_hour = FROMBCD(regs.b.ch);

    regs.b.ah = 4;
    __realint(0x1a,&regs);

  tm2->tm_mday = FROMBCD(regs.b.dl);
  tm2->tm_mon = FROMBCD(regs.b.dh)-1;
  tm2->tm_year = FROMBCD(regs.b.cl);
    if (tm2->tm_year < 71)
        tm2->tm_year += 100;
    return tm2;
}
