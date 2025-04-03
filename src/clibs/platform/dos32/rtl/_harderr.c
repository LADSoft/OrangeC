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
    __asm mov [dev],edi
    __asm mov [dos],eax
    __asm mov eax,[ebp + 12]
    __asm shl eax,4
    __asm add eax,esi
    __asm mov [hdr],eax
    __asm push ds
    __asm push es
    __asm mov ds,cs:[holdds]
    __asm mov es,cs:[holdds]
    dev &= 0xff;
    dos &= 0xffff;
    hardret = _HARDERR_FAIL;
    if (harderr_ptr)
        harderr_ptr(dev, dos, hdr);
    __asm pop es
    __asm pop ds
    __asm mov eax,[hardret]
    __asm mov [ebp + 32],eax
}
void     _RTL_FUNC  _harderr(void (*__fptr)()) //unsigned __deverr,
                            //unsigned __doserr, unsigned *__hdr))

{
    if (__fptr)
    {
        if (!harderr_ptr)
        {
            UWORD cssel;
            __asm mov ax,cs
            __asm mov [cssel],ax
            __asm mov ax,ds
            __asm mov [holdds],ax
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
