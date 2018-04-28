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
    asm push esi
    asm push edi
    asm push ebx
    asm push es
    asm cld
   asm mov es,[__envseg]
    asm mov edx,[ebp+8]
    asm or edx,edx
    asm jz lbl_count
    asm mov edx,[ebp+12]
    asm sub	eax,eax			/* set up for scan */
    asm mov edi,eax
    asm mov ecx,-1
    asm test 	byte ptr es:[edi],0xff
    asm jz	lbl_errenv
lbl_lp1:
    asm dec edx
    asm jz	lbl_gotenv
    asm repnz	scasb			/* scan for end of environment */
    asm test	byte ptr es:[edi],0xff
    asm jnz 	lbl_lp1
    asm jmp	lbl_errenv
lbl_gotenv:
    asm mov	esi,[ebp+8]
    asm xchg	esi,edi
    asm cli
    asm push	ds
    asm push	ds
    asm push	es
    asm pop	ds
    asm pop	es
lbl_mvlp:
    asm lodsb
    asm stosb
    asm or al,al
    asm jnz lbl_mvlp
    asm pop ds
    asm sti
    asm sub eax,eax
    asm inc eax
    asm jmp	lbl_exit
lbl_errenv:
    asm sub	eax,eax
    asm jmp	lbl_exit
    

lbl_count:
    asm sub	eax,eax			/* set up for scan */
    asm mov 	edi,eax
    asm mov 	ecx,-1
    asm mov edx,eax
lbl_lp:
    asm inc edx
    asm repnz	scasb			/* scan for end of environment */
    asm test	byte ptr es:[edi],0xff
    asm jnz 	lbl_lp
lbl_noenv:
    asm mov eax,edx
lbl_exit:
    asm pop es
    asm pop ebx
    asm pop edi
    asm pop esi
    asm mov [answer],eax
    return answer;
}	
