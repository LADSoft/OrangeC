;  Software License Agreement
;  
;      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

    global move


    segment code USE32
;
; move command
;
move:
    call	WadeSpace
    jz	errx
    call	ReadAddress 			; read source
    jc	errx
    call	WadeSpace
    jz	errx
    call	ReadNumber		; read end of source
    jc	errx
    mov	ecx,eax
    sub	ecx,ebx
    jb	errx
    call	WadeSpace
    jz	errx
    mov	edx,ebx
    call	ReadAddress			; read dest
    jc	errx
    call	WadeSpace
    jnz	errx
    mov	edi,ebx
    mov	esi,edx
    cmp	esi,edi
    jz	noop
    jl	sld
sgd:
    rep	movsb
    jmp	noop
sld:
    lea	esi,[esi+ecx-1]
    lea	edi,[edi+ecx-1]
    std
    rep	movsb
    cld
noop:
    clc				; to clean up stack
    ret
errx:
    stc
    ret
