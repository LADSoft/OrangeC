; Software License Agreement
; 
;     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version.
; 
;     The Orange C Compiler package is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     As a special exception, if other files instantiate templates or
;     use macros or inline functions from this file, or you compile
;     this file and link it with other works to produce a work based
;     on this file, this file does not by itself cause the resulting
;     work to be covered by the GNU General Public License. However
;     the source code for this file must still be made available in
;     accordance with section (3) of the GNU General Public License.
;     
;     This exception does not invalidate any other reasons why a work
;     based on this file might be covered by the GNU General Public
;     License.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%include  "prints.ase" 
%include  "input.ase"
%include  "mtrap.ase"
%include  "breaks.ase"
    global domath

    segment code USE32
;
; Do math
;
domath:
    call	WadeSpace	; Wade till address
    jc	near readerr
    call	ReadNumber	; first number
    jc	near readerr
    mov	edx,eax
    call	WadeSpace	; get operator
    jz	readerr
    sub	ebx,ebx
    cmp	al,'*'
    jz	got
    inc	bl
    cmp	al,'/'
    jz	got
    inc	bl
    cmp	al,'+'
    jz	got
    inc	bl
    cmp	al,'-'
    jz	got
    inc	bl
    cmp	al,'%'
    jz	got
    dec	esi
    mov	bl,2		; default operator = +
got:
    inc	esi
    call	WadeSpace
    jz	readerr
    call	ReadNumber	; second number
    jc	readerr
    mov	ecx,eax
    call	WadeSpace
    jnz	readerr
    mov	eax,edx
    call	[ebx*4 + jumptab]
    push	eax
    call	crlf
    pop	eax
    call	PrintDword		; print result
    clc
    ret
jumptab	dd	xmul,xdiv,xxadd,xsub,xmod
xmul:
    mul	ecx
    ret
xmod:
    call	xdiv
    mov	eax,edx
    ret
xdiv:
    or	eax,eax
    jz	nodiv
    sub	edx,edx
    div	ecx
nodiv:
    ret
xxadd:
    add	eax,ecx
    ret
xsub:
    sub	eax,ecx
    ret
readerr:
    stc
    ret
