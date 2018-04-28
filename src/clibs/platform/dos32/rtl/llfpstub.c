/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
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