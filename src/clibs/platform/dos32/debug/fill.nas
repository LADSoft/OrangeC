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

    global fill

    segment code USE32
;
; fill command
;
fill	:
    call	WadeSpace
    jz	errx
    call	ReadAddress		; read start address
    jc	errx
    call	WadeSpace
    jz	errx
    call	ReadNumber    		; read length
    jc	errx
    mov	ecx,eax
    sub	ecx,ebx
    jc	errx
    call	WadeSpace
    mov	al,0           		; default fill = 0
    jz	gotfill
    call	ReadNumber		; else read a fill val
    jc	errx
    push	eax
    call	WadeSpace
    pop	eax
    jnz	errx
gotfill:
    mov	edi,ebx
    rep	stosb
    clc
    ret
errx:
    stc
    ret
