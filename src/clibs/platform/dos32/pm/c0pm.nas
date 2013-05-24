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
section data USE32
%include "..\..\copyrght.asm"

extern STACKTOP

global ___pspseg
global ___envseg
global __stacktop
align 4
___pspseg dw 0
___envseg dw 0
__stacktop	dd	0
stack	dd	0
stackseg dw	0
extern __linear
section code USE32

..start:
    jmp short real_start
    db	"WATCOM",0
real_start:
    sti                             ; Set The Interrupt Flag
    cld                             ; Clear The Direction Flag
    mov WORD [___pspseg],es
    mov ax,[es:02ch]
    mov WORD [___envseg],ax
    mov ax,ds
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov WORD [stackseg],ss
    mov DWORD [stack],esp
    cli
    push ds
    pop ss
    mov esp, STACKTOP
    sti
    mov	[__stacktop],esp
    push 0
    mov ebp, esp

    and esp, -16
    mov bx,ds
    mov ax,6
    int 31h
    shl	ecx,16
    mov cx,dx
    mov [__linear],ecx

    extern	___startup
    mov	edx,1	; PMode308
    call ___startup
    lss esp,[stack]
    retf
%ifndef DEBUG
    global monitor_init
monitor_init:
    ret
%endif