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

%include  "prints.ase" 
%include  "input.ase" 
%include  "mtrap.ase" 
%include  "breaks.ase"

    global doin,doout

    segment code USE32
;
; Read from a port
;
doin:
    call	WadeSpace	; Wade till address
    jz	inerr
    call	ReadNumber
    jc	inerr
    mov	edx,eax
    call	WadeSpace
    jnz	inerr
    in	al,dx
    push	eax
    call	crlf
    pop	eax		;
    call	PrintByte	; Print data
    clc
    ret
inerr:
    stc
    ret
;
; Write to a port
;
doout:
    Call	WadeSpace	; Wade till address
    jz	inerr
    call	ReadNumber
    jc	inerr
    mov	edx,eax
    call	WadeSpace
    jz	inerr
    call	ReadNumber
    jc	inerr
    mov	ebx,eax
    call	WadeSpace
    jnz	inerr
    mov	al,bl
    out	dx,al
    clc
    ret
