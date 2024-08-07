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