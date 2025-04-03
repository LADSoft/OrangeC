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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <dpmi.h>
 #include "llp.h"

extern short __envseg;

int __ll_getenv(char *buf, int id)
{
    int answer;
    __asm push esi
    __asm push edi
    __asm push ebx
    __asm push es
    __asm cld
   __asm mov es,[__envseg]
    __asm mov edx,[ebp+8]
    __asm or edx,edx
    __asm jz lbl_count
    __asm mov edx,[ebp+12]
    __asm sub	eax,eax			/* set up for scan */
    __asm mov edi,eax
    __asm mov ecx,-1
    __asm test 	byte ptr es:[edi],0xff
    __asm jz	lbl_errenv
lbl_lp1:
    __asm dec edx
    __asm jz	lbl_gotenv
    __asm repnz	scasb			/* scan for end of environment */
    __asm test	byte ptr es:[edi],0xff
    __asm jnz 	lbl_lp1
    __asm jmp	lbl_errenv
lbl_gotenv:
    __asm mov	esi,[ebp+8]
    __asm xchg	esi,edi
    __asm cli
    __asm push	ds
    __asm push	ds
    __asm push	es
    __asm pop	ds
    __asm pop	es
lbl_mvlp:
    __asm lodsb
    __asm stosb
    __asm or al,al
    __asm jnz lbl_mvlp
    __asm pop ds
    __asm sti
    __asm sub eax,eax
    __asm inc eax
    __asm jmp	lbl_exit
lbl_errenv:
    __asm sub	eax,eax
    __asm jmp	lbl_exit
    

lbl_count:
    __asm sub	eax,eax			/* set up for scan */
    __asm mov 	edi,eax
    __asm mov 	ecx,-1
    __asm mov edx,eax
lbl_lp:
    __asm inc edx
    __asm repnz	scasb			/* scan for end of environment */
    __asm test	byte ptr es:[edi],0xff
    __asm jnz 	lbl_lp
lbl_noenv:
    __asm mov eax,edx
lbl_exit:
    __asm pop es
    __asm pop ebx
    __asm pop edi
    __asm pop esi
    __asm mov [answer],eax
    return answer;
}	
