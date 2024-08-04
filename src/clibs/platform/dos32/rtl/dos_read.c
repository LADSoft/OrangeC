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
#include <dos.h>
#include <dpmi.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

extern int __dtabuflen ;
unsigned _RTL_FUNC _dos_read(int __fd, void *__buf, unsigned __len, unsigned *__nread)
{
    DPMI_REGS regs;
   int mod = __len %__dtabuflen,i, tsize = 0;
   for (i=0; i < (__len & -__dtabuflen); i += __dtabuflen) {
        regs.h.dx = 0;
      regs.h.bx = __fd;
        regs.h.cx = __dtabuflen;
        __doscall(0x3f,&regs);
        if (regs.h.flags & 1)
         return errno = regs.b.al ;
      __dtatobuf((char *)__buf+i,__dtabuflen);
        tsize += regs.h.ax;
      if (regs.h.ax < __dtabuflen) {
         *__nread = tsize ;
         return 0 ;
      }
    }
    regs.h.dx = 0;
   regs.h.bx = __fd;
    regs.h.cx = mod;
    __doscall(0x3f,&regs);
    tsize += regs.h.ax;
    if (regs.h.flags & 1)
         return errno = regs.b.al ;
   __dtatobuf((char *)__buf+i,mod);
   *__nread = tsize ;
   return 0;
}
