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
#include <signal.h>
#include <dpmi.h>
#include "llp.h"
#include <stdlib.h>

extern int _stacktop;
extern int __pm308;
extern int _default87;

int winxcept = 0;
UWORD pmodeds,pmodess, pmodecs;
static int axsave,bxsave,cxsave,dxsave,sisave,disave,spsave,bpsave,ipsave;
static int flagssave;
static short essave,dssave,sssave,cssave,fssave,gssave;
static DPMI_REGS cbRegs, ccRegs ;
static UWORD cbSeg, cbOffs, ccSeg, ccOffs ;
static UWORD old1BSeg, old1BOffs, old23Seg, old23Offs ;

static int xceptretval, xceptxcept, xceptip, xceptcs, xceptflags;

int _breakflag;

#pragma startup llsignal 250
#pragma rundown llsig_rundown 5

// This ditty detects DPMI exception handling.
// it takes us out of the exception stack frame 
// returns us to the program's stack frame so further processing
// is possible
#pragma NOSTACKFRAME
void __clearxcept(void)
{
        asm push eax
        asm push ebp
        asm test cs:[__pm308],1
        asm jz stackswitch
        asm mov ax,ss
        asm cmp ax,cs:[pmodess]
        asm jnz stackswitch
        // if we get here it is plain DOS, tran's PMODE 3.08
        // tran's pmode does not have exception support, and
        // the exception is installed as an interrupt, and this
        // is handled specially
        asm pop ebp
        asm pop eax
        asm jmp done
stackswitch:

        asm push ds
        asm lea ebp,[esp + 16]
        asm mov ds,[ebp + 8 + 16 + 4]
        asm push ecx
        // we have to handle PMODE/W differently
        // because its exception mechanism is non-conformant
        // basically for pmode/w there will be an interrupt frame 
        // just below the SS:ESP stored in the exception frame;
        // if that frame's return address is the same as the return
        // address in the exception frame we have pmodew.  pmodew
        // will ignore changes to the exception frame so we have to
        // go to the interrupt frame to make changes...
        asm mov cx,ss
        asm mov ax,ds
        asm cmp ax,cx
        asm mov eax,[ebp + 8 + 16]
        asm jnz notpmw
        asm mov ecx,[eax - 8]
        asm cmp ecx,[ebp + 8 + 8]
        asm jnz notpmw
        asm mov ecx,[eax - 12]
        asm cmp ecx,[ebp + 8 + 4]
        asm jnz notpmw
        // if we get here it is pmode/w
        asm and word [eax - 4], 0xfeff
        asm mov [eax - 12], resync
        asm mov ax,[pmodecs]
        asm mov [eax - 8],ax
notpmw:
        asm mov eax,[ebp - 4]
        asm mov [xceptretval],eax 
        asm add ebp, 8
        asm mov eax,[ebp]
        asm mov [xceptxcept],eax 
        asm mov eax,[ebp+4]
        asm mov [xceptip],eax 
        asm mov eax,[ebp+8]
        asm mov [xceptcs],eax 
        asm mov eax,[ebp+12]
        asm mov [xceptflags],eax
        
        asm and word [ebp + 12], 0xfeff
        asm mov [ebp +4], resync
        asm mov ax,[pmodecs]
        asm mov [ebp + 8],ax
        asm pop ecx
        asm pop ds
        asm pop ebp
        asm pop eax
        asm add esp,4
        asm retf
resync:
        asm push [xceptflags]
        asm push [xceptcs]
        asm push [xceptip]
        asm push [xceptxcept]
        asm push [xceptretval]
        asm mov [winxcept], 1
done:
        return;
}
#pragma regopt

#pragma NOSTACKFRAME
static void raise12(void)
{
  asm mov ax,ss
    asm mov cx,ds
    asm cmp ax,cx
    asm jz  restack
    asm mov ecx,[_stacktop]
    asm mov dword ptr [esp + 12 + 12],ecx
        __clearxcept();
    asm jmp join
restack:
    asm mov esp,[_stacktop]
    asm sti
join:
    printf("Stack overflow, use larger stack");
    exit(EXIT_FAILURE);
}
    
#pragma NOSTACKFRAME
static void raise3(void)
{
        __clearxcept();
        if (winxcept)
            asm add esp,4;
        asm retf;
}
#pragma NOSTACKFRAME
static void raise6(void)
{
        __clearxcept();
      asm pushad
      asm push ds
      asm push es
        asm mov	ds,cs:[pmodeds]
        asm mov	es,cs:[pmodeds]
      asm sti
      raise(SIGILL) ;
      asm pop es
      asm pop ds
      asm popad
        if (winxcept)
            asm add esp,4;
        asm retf;
}

void __ll_sigsegv(int a)
{
         fprintf(stderr,"\nGeneral protection fault\n\n");
         fprintf(stderr,"CS:EIP %04X:%08X\n",cssave,ipsave);
         fprintf(stderr,"SS:ESP %04X:%08X\n",sssave,spsave+12);
         fprintf(stderr,"EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X  flags: %08X\n",
               axsave,bxsave,cxsave,dxsave,flagssave);
         fprintf(stderr,"EBP: %08X  ESI: %08X  EDI: %08X\n",bpsave,sisave,disave);
         fprintf(stderr," DS:     %04X   ES:     %04X   FS:     %04X   GS:     %04X\n",
            dssave,essave,fssave,gssave);
         _exit(1);
}
#pragma NOSTACKFRAME
static void raise13(void)
{
        __clearxcept();
        asm mov ss:[dssave],ds
        asm mov ss:[essave],es
        asm mov	ds,cs:[pmodeds]
        asm mov	es,cs:[pmodeds]
        asm mov [axsave],eax
        asm mov [bxsave],ebx
        asm mov [cxsave],ecx
        asm mov [dxsave],edx
        asm mov [sisave],esi
        asm mov [disave],edi
        asm mov [bpsave],ebp
        asm mov [spsave],esp
        asm mov [sssave],ss
        asm mov [fssave],fs
        asm mov [gssave],gs
        asm pop eax
        asm pop eax
        asm mov [ipsave],eax
        asm pop eax
        asm mov [cssave],ax
        asm pop eax
        asm mov [flagssave],eax
      asm pushad
      asm push ds
      asm push es
        asm sti;
      raise(SIGSEGV) ;
      asm pop es
      asm pop ds
      asm popad
        if (winxcept)
            asm add esp,4;
        asm retf;
}

#pragma NOSTACKFRAME
static void div0(void)
{
        __clearxcept();
      asm pushad
      asm push ds
      asm push es
        asm mov	ds,cs:[pmodeds]
        asm mov	es,cs:[pmodeds]
      asm sti
      raise(SIGFPE) ;
      asm pop es
      asm pop ds
      asm popad
        if (winxcept)
            asm add esp,4;
        asm retf;
}
#pragma regopt daf
/* __________________________________________________________________________
 *
 * this next is called from the DOS call routine
 * if there was a ctrl-break while we were calling DOS
 * this is because can't exit DPMI from an interrupt
 */
void raisecb(void)
{
   int old = _breakflag ;
    _breakflag = 0;
   if (old & 2)
      raise(SIGBREAK) ;
   else
      raise(SIGINT) ;
}
#pragma NOSTACKFRAME
static void intcb(void)
{
   asm push ds
   asm mov ds,cs:[pmodeds]
   asm or dword [_breakflag],2

  asm pop ds
  asm iretd
}
#pragma NOSTACKFRAME
static void int23(void)
{
   asm push ds
   asm mov ds,cs:[pmodeds]
   asm or dword [_breakflag],1

  asm pop ds
  asm iretd
}
void  __lockregion(void *start, void *end)
{
    SELECTOR sel;
    int len = (int)end - (int)start;
    int base ;
    asm mov [sel],cs
    
    dpmi_get_sel_base(&base,sel);
    base += (int)start;
    asm mov esi,[len]
    asm mov ebx,[base]
    asm mov ecx,ebx
    asm shr ebx,16
    asm mov edi,esi
    asm shr esi,16
    asm mov eax,0x600
    asm int 0x31
}
int __setxcept(int num, void *address)
{
  SELECTOR sel;
    asm mov [sel],cs

    return dpmi_set_protected_except(num,sel,(ULONG) address) ;
}
static void llsignal(void)
{
    SELECTOR sel;
    asm mov [sel],cs // this local variable is to workaround some issue with
                        // the extender under virtualbox/freedos 
                        // just having it force the stack frame fixes the issue...
      asm mov [pmodecs],cs
        asm mov	[pmodeds],ds
        asm mov [pmodess],ss

//        __setxcept(3,raise3);
        __setxcept(12,raise12);
        __setxcept(13,raise13);
        __setxcept(14,raise13);
      __setxcept(6,raise6);

        __setxcept(0,div0);
        __setxcept(4,div0);

      // tran's pmode doesn't reflect these ints up to pmode
      // so SIGINT and SIGBREAK don't work with that
      // Windows DPMI doesn't reflect the 1B so SIGBREAK won't
      // work in a DOS shell...
      dpmi_set_protected_interrupt(0x23,sel,(ULONG) int23);
      dpmi_set_protected_interrupt(0x1b,sel,(ULONG) intcb);
}
static void llsig_rundown(void)
{
    static volatile int a;
    a = _default87; // to link in the FP stuff
}
void __ll_signal(int signum, sighandler_t func) 
{
    (void)(func);
}