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
; COMPARE.ASM
;
; Function: compare memory regions
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
