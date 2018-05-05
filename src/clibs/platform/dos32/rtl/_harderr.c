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
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
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
