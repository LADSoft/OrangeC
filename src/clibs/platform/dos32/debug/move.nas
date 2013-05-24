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
; MOVE.ASM
;
; Function: move memory regions
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
