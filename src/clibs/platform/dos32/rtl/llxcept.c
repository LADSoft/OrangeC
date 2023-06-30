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

#pragma startup llsignal 6
#pragma rundown llsig_rundown 5

// This ditty detects DPMI exception handling.
// it takes us out of the exception stack frame 
// returns us to the program's stack frame so further processing
// is possible
#pragma NOSTACKFRAME
void __clearxcept(void)
{
        __asm push eax
        __asm push ebp
        __asm test cs:[__pm308],1
        __asm jz stackswitch
        __asm mov ax,ss
        __asm cmp ax,cs:[pmodess]
        __asm jnz stackswitch
        // if we get here it is plain DOS, tran's PMODE 3.08
        // tran's pmode does not have exception support, and
        // the exception is installed as an interrupt, and this
        // is handled specially
        __asm pop ebp
        __asm pop eax
        __asm jmp done
stackswitch:

        __asm push ds
        __asm lea ebp,[esp + 16]
        __asm mov ds,[ebp + 8 + 16 + 4]
        __asm push ecx
        // we have to handle PMODE/W differently
        // because its exception mechanism is non-conformant
        // basically for pmode/w there will be an interrupt frame 
        // just below the SS:ESP stored in the exception frame;
        // if that frame's return address is the same as the return
        // address in the exception frame we have pmodew.  pmodew
        // will ignore changes to the exception frame so we have to
        // go to the interrupt frame to make changes...
        __asm mov cx,ss
        __asm mov ax,ds
        __asm cmp ax,cx
        __asm mov eax,[ebp + 8 + 16]
        __asm jnz notpmw
        __asm mov ecx,[eax - 8]
        __asm cmp ecx,[ebp + 8 + 8]
        __asm jnz notpmw
        __asm mov ecx,[eax - 12]
        __asm cmp ecx,[ebp + 8 + 4]
        __asm jnz notpmw
        // if we get here it is pmode/w
        __asm and word [eax - 4], 0xfeff
        __asm mov [eax - 12], resync
        __asm mov ax,[pmodecs]
        __asm mov [eax - 8],ax
notpmw:
        __asm mov eax,[ebp - 4]
        __asm mov [xceptretval],eax 
        __asm add ebp, 8
        __asm mov eax,[ebp]
        __asm mov [xceptxcept],eax 
        __asm mov eax,[ebp+4]
        __asm mov [xceptip],eax 
        __asm mov eax,[ebp+8]
        __asm mov [xceptcs],eax 
        __asm mov eax,[ebp+12]
        __asm mov [xceptflags],eax
        
        __asm and word [ebp + 12], 0xfeff
        __asm mov [ebp +4], resync
        __asm mov ax,[pmodecs]
        __asm mov [ebp + 8],ax
        __asm pop ecx
        __asm pop ds
        __asm pop ebp
        __asm pop eax
        __asm add esp,4
        __asm retf
resync:
        __asm push [xceptflags]
        __asm push [xceptcs]
        __asm push [xceptip]
        __asm push [xceptxcept]
        __asm push [xceptretval]
        __asm mov [winxcept], 1
done:
        return;
}
#pragma regopt

#pragma NOSTACKFRAME
static void raise12(void)
{
  __asm mov ax,ss
    __asm mov cx,ds
    __asm cmp ax,cx
    __asm jz  restack
    __asm mov ecx,[_stacktop]
    __asm mov dword ptr [esp + 12 + 12],ecx
        __clearxcept();
    __asm jmp join
restack:
    __asm mov esp,[_stacktop]
    __asm sti
join:
    printf("Stack overflow, use larger stack");
    exit(EXIT_FAILURE);
}
    
#pragma NOSTACKFRAME
static void raise3(void)
{
        __clearxcept();
        if (winxcept)
            __asm add esp,4;
        __asm retf;
}
#pragma NOSTACKFRAME
static void raise6(void)
{
        __clearxcept();
      __asm pushad
      __asm push ds
      __asm push es
        __asm mov	ds,cs:[pmodeds]
        __asm mov	es,cs:[pmodeds]
      __asm sti
      raise(SIGILL) ;
      __asm pop es
      __asm pop ds
      __asm popad
        if (winxcept)
            __asm add esp,4;
        __asm retf;
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
        __asm mov ss:[dssave],ds
        __asm mov ss:[essave],es
        __asm mov	ds,cs:[pmodeds]
        __asm mov	es,cs:[pmodeds]
        __asm mov [axsave],eax
        __asm mov [bxsave],ebx
        __asm mov [cxsave],ecx
        __asm mov [dxsave],edx
        __asm mov [sisave],esi
        __asm mov [disave],edi
        __asm mov [bpsave],ebp
        __asm mov [spsave],esp
        __asm mov [sssave],ss
        __asm mov [fssave],fs
        __asm mov [gssave],gs
        __asm pop eax
        __asm pop eax
        __asm mov [ipsave],eax
        __asm pop eax
        __asm mov [cssave],ax
        __asm pop eax
        __asm mov [flagssave],eax
      __asm pushad
      __asm push ds
      __asm push es
        __asm sti;
      raise(SIGSEGV) ;
      __asm pop es
      __asm pop ds
      __asm popad
        if (winxcept)
            __asm add esp,4;
        __asm retf;
}

#pragma NOSTACKFRAME
static void div0(void)
{
        __clearxcept();
      __asm pushad
      __asm push ds
      __asm push es
        __asm mov	ds,cs:[pmodeds]
        __asm mov	es,cs:[pmodeds]
      __asm sti
      raise(SIGFPE) ;
      __asm pop es
      __asm pop ds
      __asm popad
        if (winxcept)
            __asm add esp,4;
        __asm retf;
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
   __asm push ds
   __asm mov ds,cs:[pmodeds]
   __asm or dword [_breakflag],2

  __asm pop ds
  __asm iretd
}
#pragma NOSTACKFRAME
static void int23(void)
{
   __asm push ds
   __asm mov ds,cs:[pmodeds]
   __asm or dword [_breakflag],1

  __asm pop ds
  __asm iretd
}
void  __lockregion(void *start, void *end)
{
    SELECTOR sel;
    int len = (int)end - (int)start;
    int base ;
    __asm mov [sel],cs
    
    dpmi_get_sel_base(&base,sel);
    base += (int)start;
    __asm mov esi,[len]
    __asm mov ebx,[base]
    __asm mov ecx,ebx
    __asm shr ebx,16
    __asm mov edi,esi
    __asm shr esi,16
    __asm mov eax,0x600
    __asm int 0x31
}
int __setxcept(int num, void *address)
{
  SELECTOR sel;
    __asm mov [sel],cs

    return dpmi_set_protected_except(num,sel,(ULONG) address) ;
}
static void llsignal(void)
{
    SELECTOR sel;
    __asm mov [sel],cs // this local variable is to workaround some issue with
                        // the extender under virtualbox/freedos 
                        // just having it force the stack frame fixes the issue...
      __asm mov [pmodecs],cs
        __asm mov	[pmodeds],ds
        __asm mov [pmodess],ss

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