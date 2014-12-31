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
