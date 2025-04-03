;  Software License Agreement
;  
;      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
;  
;      This file is part of the Orange C Compiler package.
;  
;      The Orange C Compiler package is free software: you can redistribute it and/or modify
;      it under the terms of the GNU General Public License as published by
;      the Free Software Foundation, either version 3 of the License, or
;      (at your option) any later version.
;  
;      The Orange C Compiler package is distributed in the hope that it will be useful,
;      but WITHOUT ANY WARRANTY; without even the implied warranty of
;      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;      GNU General Public License for more details.
;  
;      You should have received a copy of the GNU General Public License
;      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
;  
;      contact information:
;          email: TouchStone222@runbox.com <David Lindauer>
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
