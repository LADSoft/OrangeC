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

    global search
    segment	data	USE32
count	dw	0
stab	times	128 [db 0]


    segment code USE32
search:
    mov	word [count],0
    call	WadeSpace
    jz	near errx
    call	ReadAddress	; read address
    jc	near errx
    mov	ecx,ebx
    call	WadeSpace
    jz	near errx
    push	ecx
    mov	edi,edx
    push	edi
    call	ReadAddress	; read len
    pop	edi
    pop	ecx
    jc	near errx
    push	edi
    mov	edi,stab
grablp:				; read rest of params
    call	WadeSpace
    jz	grabdone
    cmp	al,"'"
    jz	readstring
    cmp	al,'"'
    jnz	readnum
readstring:             	; read a quoted string
    inc	esi
readstringlp:
    lodsb
    cmp	al,13
    jz	grabdone
    cmp	al,'"'
    jz	grablp
    cmp	al,"'"
    jz	grablp
    stosb
    inc	word [count]
    cmp	word [count],080h
    jnc	errx
    jmp	readstringlp

readnum:			; read an (8-bit) number
    push	edi
    push	ecx
    call	ReadNumber
    pop	ecx
    pop	edi
    jc	errx2
    stosb
    inc	word [count]
    cmp	word [count],080h
    jnc	errx
    jmp	grablp

grabdone:                       	; all params got now
    pop	edi
    test	word [count],0ffh		; see if any search
    jz	errx
    sub	ebx,ecx
    push	ebx
    call	crlf
    pop	ebx
    mov	esi,ecx
go1:
    call	ScanKey		; I put this in for debug, but,
    jnz	nofill2		; good for mistakes too...
    movzx	ecx,word [count]
    push	esi
    mov	edi,stab
    repe	cmpsb
    pop	esi
    jnz	nomatch
    push	ebx
    mov	eax,esi		; match, display address
    call	PrintDword
    call	crlf
    pop	ebx
nomatch:
    inc	esi
    dec	ebx
    jne	go1
    clc
    ret
nofill:
nofill2:
    clc
    ret
errx2:
    pop	edi
errx:
    stc
    ret
