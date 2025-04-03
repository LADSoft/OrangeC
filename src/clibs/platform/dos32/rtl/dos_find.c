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
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

static unsigned getdta(void)
{
    DPMI_REGS regs;
    regs.b.ah = 0x2f;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
   dpmi_simulate_real_interrupt(0x21,&regs);   return (regs.h.es << 16) + regs.h.bx;

}
static void setdta(unsigned dta)
{
    DPMI_REGS regs;
    regs.b.ah = 0x1a;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    regs.h.ds = dta >> 16;
    regs.h.dx = dta & 0xffff;
    dpmi_simulate_real_interrupt(0x21,&regs);
}
int _RTL_FUNC _dos_findfirst(char *string, int attr, struct find_t *buf)
{
    DPMI_REGS regs;
    SELECTOR sel,para;
    SELECTOR nsel,npara;
   int dta = getdta();
    if (dpmi_alloc_real_memory(&sel,&para,0x3))
        return 8;
    if (dpmi_alloc_real_memory(&nsel,&npara,0x8)) {
        dpmi_dealloc_real_memory(sel);
        return 8;
    }
   setdta((int)para << 16);
    dpmi_copy_from_ds(nsel,0,string,strlen(string)+1);
    regs.b.ah = 0x4e;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    regs.h.ds = npara;
    regs.h.dx = 0;
    regs.h.cx = attr;
    dpmi_simulate_real_interrupt(0x21,&regs);
    dpmi_copy_to_ds(buf,sel,0,48);
    dpmi_dealloc_real_memory(sel);
    dpmi_dealloc_real_memory(nsel);
    setdta(dta);
    if (regs.h.flags & 1)
        return regs.b.al;
    else
        return 0;
}
int _RTL_FUNC _dos_findnext(struct find_t *buf)
{
    DPMI_REGS regs;
    SELECTOR sel,para;
   int dta = getdta();
    if (dpmi_alloc_real_memory(&sel,&para,0x3))
        return 8;
   setdta((int)para << 16);
    dpmi_copy_from_ds(sel,0,buf,48);
    regs.b.ah = 0x4f;
    regs.h.flags = 0x72;
    regs.h.ss = 0;
    regs.h.sp = 0;
    dpmi_simulate_real_interrupt(0x21,&regs);
    dpmi_copy_to_ds(buf,sel,0,48);
    dpmi_dealloc_real_memory(sel);
    setdta(dta);
    if (regs.h.flags & 1)
        return regs.b.al;
    else
        return 0;
}
void __ll_findclose (void *buf)
{
   (void)buf ;
}
