/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
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
#pragma startup llfpini 1
#pragma rundown llfprundown 1
        
unsigned short oldint75[2];

int hasfloat = 0; /* used by DOS debugger */

static void _interrupt raise75(void)
{
    DPMI_REGS regs;
    __asm cli
      __asm push ds
      __asm push es
        __asm mov	ds,cs:[pmodeds]
        __asm mov	es,cs:[pmodeds]

      __asm sti
      // Have to call the original interrupt, there is more to
      // acknowledging the FPU interrupt than just acknowledging the PIC
      //
      memset(&regs,0,sizeof(regs));
      regs.h.flags = 0x72;
      regs.h.ip = oldint75[0];
      regs.h.cs = oldint75[1];
      dpmi_simulate_proc_int_frame(&regs);
      raise(SIGFPE) ;
      __asm cli
      __asm pop es
      __asm pop ds
}
static void raise7(void)
{
        __clearxcept();
      __asm pushad
      __asm push ds
      __asm push es
        __asm mov	ds,cs:[pmodeds]
        __asm mov	es,cs:[pmodeds]
        __asm mov al,0x65
        __asm out 0xa0,al
        __asm mov al,0x62
        __asm out 0xa0,al
      __asm sti
      raise(SIGFPE) ;
      __asm pop es
      __asm pop ds
      __asm popad
        if (winxcept)
            __asm add esp,4;
        __asm retf;
}

short _RTL_FUNC _status87(void)
{
   short work ;
   __asm fnstsw [work] ;
   __asm fwait ;
   return work ;
}
short _RTL_FUNC _clear87(void)
{
    short work ;
    __asm fnstsw	[work ];
   __asm fwait ;
    __asm fnclex
    return work ;
}
void _RTL_FUNC _control87(int mask1, int mask2)
{
    short work ;
    __asm fstcw [work];
    mask1 &= mask2;
    mask2 = ~mask2;
    __asm fwait
    work &= mask2;
    work = mask1 |= work ;
    __asm fldcw [work]
}
void _RTL_FUNC _fpreset(void)
{
   __asm finit ;
   _control87(_default87,0x1fff) ;
}
int __temp;
static void llfprundown(void)
{
    __asm fninit
}
static void llfpini(void)
{
    short work;
    SELECTOR sel ;
    ULONG base,linear;
    __asm mov [sel],cs
    work = 0x4567;

    // enable SSE2
    __asm mov eax,cr0
    __asm and eax, 0xfffffffb // clear CR0.EM
    __asm or eax, 2 // set CR0.MP
    __asm mov cr0,eax
    __asm mov eax,cr4
    __asm or eax, 512 // set OSFXSR
    __asm or eax, 1024 // set OSXMMXCPT
    __asm mov cr4,eax
    //
  __asm fninit
  __asm fnstsw [work]
  dpmi_get_real_interrupt((UWORD *)&oldint75[1],(UWORD *)&oldint75[0],0x75); 
    if (work == 0) {
        /* have fpu coprocessor */
        _fpreset();
        __asm fnstenv [dfltenv]
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
    __asm push esi
    __asm mov ecx,[linear]
    __asm mov esi,100000
    __asm mov dx,cx
    __asm shr ecx,16
    __asm mov di,si
    __asm shr esi,16
    __asm mov eax,0x600
    __asm int 0x31
    __asm pop esi
}