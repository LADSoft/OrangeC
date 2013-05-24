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
#include <dos.h>
#include <dpmi.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"
#include <errno.h>

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
