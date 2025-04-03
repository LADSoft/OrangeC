/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
