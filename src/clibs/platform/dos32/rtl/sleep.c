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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <dpmi.h>
// #include "llp.h"

#define SECONDSPERDAY (24 * 60 * 60)
#define TIMERFREQUENCY (1193180LL)
#define TIMERCOUNT 65536

void __realint(int val, DPMI_REGS *regs);

static int _cancelsleep;

__int64 __ll_ticks(void)
{
    __int64 rv ;
    int temp;
    div_t divs;
    time_t t = time(0) + *(__getTimezone());
    DPMI_REGS regs;
    regs.b.ah = 0;
    __realint(0x1a,&regs);
    /* grab the days */
    divs = div(t, SECONDSPERDAY);
    /* grab the ticks this day */
    temp = (regs.h.cx << 16) + regs.h.dx;
    
    /* if in the first two seconds of the day */
    if (temp < 2 * 18)
    {
        /* and the time was retrieved within the last two seconds yesterday */
        if (divs.rem >= SECONDSPERDAY - 2)
            /* update yesterday to today */
            divs.quot++ ;
    }
    /* calculate total ticks since 1970 */
    rv = ((__int64)divs.quot) * SECONDSPERDAY * TIMERFREQUENCY / TIMERCOUNT;
    rv = rv + temp;
    // convert to milliseconds
    rv = (rv * CLK_TCK * TIMERCOUNT) / TIMERFREQUENCY;
    return rv;
}
int _RTL_FUNC sleep(int seconds)
{
    __int64 t = __ll_ticks();
    _cancelsleep = 0;
   seconds *= CLK_TCK ;
   while (__ll_ticks() - t < seconds && !_cancelsleep) ;
}
void _RTL_FUNC __ll_cancelsleep(void)
{
    _cancelsleep = 1;
}
void _RTL_FUNC _sleep(unsigned long tm)
{
    __int64 t = __ll_ticks();
    tm = tm * CLK_TCK / 1000;	
   while (__ll_ticks() - t < tm) ;
}
void _RTL_FUNC _beep(unsigned freq, unsigned interval)
{
    _sound(freq);
    if (interval == (unsigned)-1)
    {
        return;
    }
    _sleep(interval);
    _nosound();
}