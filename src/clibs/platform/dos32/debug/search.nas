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


%include  "prints.ase" 
%include  "input.ase"
%include  "mtrap.ase"

    global search
    segment	data	USE32
count	dw	0
stab	times	128 [db 0]


    .CODE
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
