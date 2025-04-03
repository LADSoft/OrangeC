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

#include <dos.h>
#include <errno.h>
#include <dpmi.h>
#include <time.h>

unsigned _RTL_FUNC   _dos_settime(struct dostime_t *__timep)
{
    DPMI_REGS regs;
   regs.b.ch = __timep->hour;
   regs.b.cl = __timep->minute;
   regs.b.dh = __timep->second ;
   regs.b.dl = __timep->hsecond ;
   if (!__doscall(0x2d,&regs)) {
      errno = EINVAL ;
        return 2;
   }
    return 0;
}
