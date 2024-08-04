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