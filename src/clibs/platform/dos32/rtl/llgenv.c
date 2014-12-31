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
