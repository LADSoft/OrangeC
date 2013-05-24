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
#include "dpmi.h"
#include "dos.h"

static void (* harderr_ptr)(unsigned __deverr, unsigned __doserr, unsigned *__hdr);
             
static unsigned hardret ;
static unsigned char critLast[8];
static UWORD holdds;
static _interrupt newError()
{
    unsigned dev;
    unsigned dos;
    unsigned *hdr;
    asm mov [dev],edi
    asm mov [dos],eax
    asm mov eax,[ebp + 12]
    asm shl eax,4
    asm add eax,esi
    asm mov [hdr],eax
    asm push ds
    asm push es
    asm mov ds,cs:[holdds]
    asm mov es,cs:[holdds]
    dev &= 0xff;
    dos &= 0xffff;
    hardret = _HARDERR_FAIL;
    if (harderr_ptr)
        harderr_ptr(dev, dos, hdr);
    asm pop es
    asm pop ds
    asm mov eax,[hardret]
    asm mov [ebp + 32],eax
}
void     _RTL_FUNC  _harderr(void (*__fptr)()) //unsigned __deverr,
                            //unsigned __doserr, unsigned *__hdr))

{
    if (__fptr)
    {
        if (!harderr_ptr)
        {
            UWORD cssel;
            asm mov ax,cs
            asm mov [cssel],ax
            asm mov ax,ds
            asm mov [holdds],ax
            dpmi_get_protected_interrupt(((unsigned char *)&critLast + 4), &critLast, 0x24) ;
            dpmi_set_protected_interrupt(0x24,cssel,(ULONG)newError) ;
        }
        harderr_ptr = __fptr;
    }
    else
    {
        if (harderr_ptr)
        {
            dpmi_set_protected_interrupt(0x24,*(UWORD *)critLast + 4,*(ULONG *)critLast) ;
        }
        harderr_ptr = NULL;
    }
}
void     _RTL_FUNC  _hardresume(int __axret)
{
    hardret = __axret;
}
void     _RTL_FUNC  _hardretn(int __retn)
{
    hardret = __retn;
}
