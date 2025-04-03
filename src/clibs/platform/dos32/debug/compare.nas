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
%include "input.ase"
%include "mtrap.ase"

    global	compare
    global	paddr


    segment code USE32

;INPUT: AL has byte (two hex chars) to print

pbyte	:
    call	PrintByte
    call	PrintSpace
    ret

;INPUT: EDX has segment address
;	EAX has offset address
;If flat real mode, print 32-bit offset, else print 16-bit offset

paddr	:
    call	PrintDword		;print 32-bit offset
    call	PrintSpace
    ret
;
; compare command
; syntax is c start,end,start2
;
compare	:
    call	WadeSpace		;find source address
    jz	errx			;not found (end of input)
    call	ReadAddress 			;start seg in DX, offset in EBX
    jc	errx			;not found
    call	WadeSpace		;see if any more
    jz	errx			;nope, didn't find
    call	ReadNumber		; read ending offset
    jc	errx			;not found
    mov	ecx,eax			;get ending offset into ECX
    sub	ecx,ebx			;subtract starting offset
    jb	errx			;error if end < start
    call	WadeSpace		;else keep reading input
    jz	errx			;need a destination
    mov	edx,ebx
    call	ReadAddress			; read dest seg and ofs
    jc	errx			;not found
    call	WadeSpace		;make sure no more
    jnz	errx			;if more, syntax error
gotsz:
    mov	esi,edx
    mov	edi,ebx
clp:
    repe	cmpsb			;cmp ds:[esi] with es:[edi]
    mov	al,[ds:esi-1]		;differing byte from source in AL
    mov	ah,[es:edi-1]		;and dest in AH
    jz	cdone			;no difference in this case
    push	ax			;save differing bytes
    call	crlf			;new line
    mov	eax,esi			;location where different
    dec	eax			;minus cmpsb moved ESI past it
    call	paddr			;show ESI where different
    pop	ax			;restore differing bytes
    push	ax			;and save them again
    call	pbyte			;show source byte
    pop	ax			;get them back
    mov	al,ah			;setup dest byte
    call	pbyte			;and show that
    mov	eax,edi			;get dest offset
    dec	eax			;minus correction for cmps
    call	paddr			;show dest seg:ofs
    jmp	clp			;find next difference
    
cdone:
    clc				; to clean up stack
    ret
errx:
    stc
    ret
