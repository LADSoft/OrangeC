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
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dpmi.h>
#include <fenv.h>
#include "llp.h"


extern int _stacktop;
extern UWORD pmodeds,pmodess;
extern int winxcept;

unsigned int _RTL_DATA _default87 = 0x1332 ;
static fenv_t dfltenv;

fenv_t _RTL_DATA * const __fe_default_env = &dfltenv;

static unsigned oldcr0;
#pragma startup llfpini 255
#pragma rundown llfprundown 1
        
unsigned short oldint75[2];

int hasfloat = 0; /* used by DOS debugger */

static void _interrupt raise75(void)
{
    DPMI_REGS regs;
    asm cli
      asm push ds
      asm push es
        asm mov	ds,cs:[pmodeds]
        asm mov	es,cs:[pmodeds]

      asm sti
      // Have to call the original interrupt, there is more to
      // acknowledging the FPU interrupt than just acknowledging the PIC
      //
      memset(&regs,0,sizeof(regs));
      regs.h.flags = 0x72;
      regs.h.ip = oldint75[0];
      regs.h.cs = oldint75[1];
      dpmi_simulate_proc_int_frame(&regs);
      raise(SIGFPE) ;
      asm cli
      asm pop es
      asm pop ds
}
static void raise7(void)
{
        __clearxcept();
      asm pushad
      asm push ds
      asm push es
        asm mov	ds,cs:[pmodeds]
        asm mov	es,cs:[pmodeds]
        asm mov al,0x65
        asm out 0xa0,al
        asm mov al,0x62
        asm out 0xa0,al
      asm sti
      raise(SIGFPE) ;
      asm pop es
      asm pop ds
      asm popad
        if (winxcept)
            asm add esp,4;
        asm retf;
}

short _RTL_FUNC _status87(void)
{
   short work ;
   asm fnstsw [work] ;
   asm fwait ;
   return work ;
}
short _RTL_FUNC _clear87(void)
{
    short work ;
    asm fnstsw	[work ];
   asm fwait ;
    asm fnclex
    return work ;
}
void _RTL_FUNC _control87(int mask1, int mask2)
{
    short work ;
    asm fstcw [work];
    mask1 &= mask2;
    mask2 = ~mask2;
    asm fwait
    work &= mask2;
    work = mask1 |= work ;
    asm fldcw [work]
}
void _RTL_FUNC _fpreset(void)
{
   asm finit ;
   _control87(_default87,0x1fff) ;
}
int __temp;
static void llfprundown(void)
{
    asm fninit
}
static void llfpini(void)
{
    short work;
    SELECTOR sel ;
    ULONG base,linear;
    asm mov [sel],cs
    work = 0x4567;

  asm fninit
  asm fnstsw [work]
  dpmi_get_real_interrupt((UWORD *)&oldint75[1],(UWORD *)&oldint75[0],0x75); 
    if (work == 0) {
        /* have fpu coprocessor */
        _fpreset();
        asm fnstenv [dfltenv]
        _fpreset();
        dpmi_set_protected_interrupt(0x75,sel,(ULONG)raise75);
        hasfloat = 1;        
//		__setxcept(16,raise16);
    }
    else {
        /* need emulator */
        __setxcept(7,raise7);
    }
    
// lock the region to prevent page faults under windows
    asm push esi
    asm mov ecx,[linear]
    asm mov esi,100000
    asm mov dx,cx
    asm shr ecx,16
    asm mov di,si
    asm shr esi,16
    asm mov eax,0x600
    asm int 0x31
    asm pop esi
}