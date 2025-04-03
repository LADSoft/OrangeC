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
#include <dos.h>
#include <dpmi.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

int _RTL_FUNC _dos_getpwd(char *buf, int drive)
{
    DPMI_REGS regs;
    SELECTOR sel,para;
    if (dpmi_alloc_real_memory(&sel,&para,0x5))
        return 8;
    regs.b.ah = 0x47;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    regs.h.ds = para;
    regs.h.dx = drive;
    regs.h.si = 0;
    dpmi_simulate_real_interrupt(0x21,&regs);
    dpmi_copy_to_ds(buf,sel,0,65);
    dpmi_dealloc_real_memory(sel);
    return strlen(buf);
}
