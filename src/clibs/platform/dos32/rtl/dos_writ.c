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
unsigned _RTL_FUNC _dos_write(int __fd, const void *__buf, unsigned __len, unsigned *__nread )
{
    DPMI_REGS regs;
   int mod = __len%__dtabuflen,i,tsize = 0;
   for (i=0; i < (__len&-__dtabuflen); i +=__dtabuflen) {
      regs.h.dx = __buftodta((char *)__buf+i,__dtabuflen);
      regs.h.bx = __fd;
        regs.h.cx = __dtabuflen;
        __doscall(0x40,&regs);
        if (regs.h.flags & 1)
         return errno = regs.b.al ;
        tsize += regs.h.ax;
    }
   regs.h.dx = __buftodta((char *)__buf+i,mod);
   regs.h.bx = __fd;
    regs.h.cx = mod;
    __doscall(0x40,&regs);
    if (regs.h.flags & 1)
         return errno = regs.b.al ;
    tsize += regs.h.ax;
   *__nread = tsize ;
   return 0;
}
