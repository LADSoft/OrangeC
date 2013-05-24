;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; mmx.asm
;
; Function: Handle MMX register display and input
;

%include "prints.ase"
%include "mtrap.ase"
%include "input.ase" 
%include "dis.ase"
%include "regs.ase"

        ;extern cpufeatures
        global ModifyMMXRegisters

        segment data use32
readsel db      0
floatstat times 54 [dw 0]
ten     dd      10

    segment code USE32
mmxdump :
        mov     al,cl
        or      al,30h
        mov     byte [mmdat],al
        push    ecx
        call    Message
        db      13,10,"MM"
mmdat   db 0,":",0
        
        pop     ebx
        imul    ebx,[ten]
        mov     ecx,floatstat+28
        add		ebx,ecx
        mov     ecx,8
mdl:
        test    cl,1
        jnz     msp
        cmp     cl,8
        jae     msp
        mov     dl,'-'
        call    PutChar
        jmp     mjn
msp:
        call    PrintSpace
mjn:
;		mov		dl,C_NUMBER
;		call	PutChar
        mov     edi,ecx
        mov     al,[ebx+edi-1]
        call    PrintByte
;		mov		dl,C_DEFAULT
;		call	PutChar
        loop    mdl
        ret
readmmx :
        movzx   ebx,byte [readsel]
        imul    ebx,[ten]
        mov     ecx,floatstat + 28
        add		ebx,ecx
        fnsave  [floatstat]
        mov     ecx,8
rmxl:
        call    ReadNumber
        jc      errx
        mov     edi,ecx
        mov     [ebx+edi-1],al
        call    WadeSpace
        jz      rmxret
        loop    rmxl
rmxret:
        frstor  [floatstat]
        clc
        ret
errx:
        stc
        ret        
ModifyMMXRegisters :
;        test    [cpufeatures],0800000h
;        jnz     mmxavail
;        PRINT_MESSAGE <13,10,"Processor does not support MMX">
;        clc
;        ret
mmxavail:
        inc esi
        call    WadeSpace
        jnz     modreg
        fnsave  [floatstat]
        frstor  [floatstat]
        sub     ecx,ecx
mmxdl:
        push    ecx
        call    mmxdump
        pop     ecx
        inc     cl
        cmp     cl,8
        jc      mmxdl
        clc
        ret
modreg:
        lodsb
        cmp     al,'m'
        jnz      mrn
        lodsb
mrn:
        sub     al,'0'
        jc      rr_out
        cmp     al,8
        jnc     rr_out
        mov     [readsel],al
readregvalue:
    call	WadeSpace
    jz	doregprompt
    inc	si
    cmp	al,':'
    jz	ReadRegValue
    cmp	al,'='
    jz	ReadRegValue
    dec	si
        call    readmmx
    ret
doregprompt:
    push	ebx			; Else put up prompt
    push	ecx			;
    Call	Message
    db	13,10,": ", 0
    call	GetInputLine		; Get input line
    pop	ecx			;
    pop	ebx			;
    call	WadeSpace		; Ignore spaces
    jz	short rr_out		; Quit if so
        call    readmmx
    ret
rr_out:
    stc
    ret
