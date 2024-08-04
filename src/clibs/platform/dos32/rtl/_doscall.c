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

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <dpmi.h>
 #include "llp.h"

extern int _breakflag;

extern UWORD __dtaseg;

int __doscall(int func, DPMI_REGS *regs)
{
    if (_breakflag)
        raisecb();
    regs->b.ah = func;
   regs->h.ds = __dtaseg;
   regs->h.es = __dtaseg;
    regs->h.ss = 0;
    regs->h.sp = 0;
    regs->h.flags = 0x42;
    dpmi_simulate_real_interrupt(0x21,regs);
    if (regs->h.flags & 1) {
        DPMI_REGS reg2;
        reg2.b.ah = 0x59;
        reg2.h.bx = 0;
        reg2.h.ss = 0;
        reg2.h.sp = 0;
        reg2.h.flags = 0x42;
        dpmi_simulate_real_interrupt(0x21,&reg2);
        _dos_errno = errno = reg2.b.al;
        return 0;
    }
    return regs->h.ax;
}
