/* Software License Agreement
 * 
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

int __ll_settime(struct tm *tm2)
{
    DPMI_REGS regs;
    int v = tm2->tm_year;
    if (v > 170)
        v -= 100;

      regs.b.dh = TOBCD(tm2->tm_sec) ;
      regs.b.cl = TOBCD(tm2->tm_min) ;
      regs.b.ch = TOBCD(tm2->tm_hour) ;
    regs.b.ah = 3;
    __realint(0x1a,&regs);


      regs.b.dl = TOBCD(tm2->tm_mday);
      regs.b.dh = TOBCD(tm2->tm_mon+1);
      regs.b.cl = TOBCD(v);
    if (tm2->tm_year < 71)
        tm2->tm_year += 100;
    regs.b.ah = 5;
    __realint(0x1a,&regs);
    return 0;
}
