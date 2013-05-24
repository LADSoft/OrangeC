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
