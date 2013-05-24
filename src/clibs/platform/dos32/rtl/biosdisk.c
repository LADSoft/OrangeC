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
#include <bios.h>
#include <dpmi.h>

extern UWORD __dtaseg;

int _RTL_FUNC      bdll(int __cmd, int __drive, int __head, int __track,
                            int __sector, int __nsects, void *__buffer)
{
      DPMI_REGS regs ;
      regs.b.ah = __cmd ;
      regs.b.al = __drive ;
      regs.b.ch = __track ;
      regs.b.cl = __sector ;
      regs.b.dh = __head ;
      regs.b.dl = __drive ;
      regs.h.ds = __dtaseg;
      regs.h.es = __dtaseg;
      regs.h.ss = 0;
      regs.h.sp = 0;
      regs.h.flags = 0x42;
      __buftodta(__buffer, __nsects * 512) ;
      dpmi_simulate_real_interrupt(0x13,&regs);
      __dtatobuf(__buffer, __nsects * 512) ;
      return regs.h.ax ;
}
int _RTL_FUNC      biosdisk(int __cmd, int __drive, int __head, int __track,
                            int __sector, int __nsects, void *__buffer)
{
   return bdll(__cmd,__drive,__head,__track,__sector,__nsects,__buffer) ;
}
unsigned _RTL_FUNC _bios_disk(unsigned __cmd, struct diskinfo_t *__dinfo)
{
   return bdll(__cmd,__dinfo->drive, __dinfo->head, __dinfo->track,
         __dinfo->sector, __dinfo->nsectors,__dinfo->buffer) ;
}

