; Software License Agreement
; 
;     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version, with the addition of the 
;     Orange C "Target Code" exception.
; 
;     The Orange C Compiler package is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _powf]
[export _pow]
[export _powl]
%endif
[global _powf]
[global _pow]
[global _powl]
[extern exp2]

SECTION data CLASS=DATA USE32
nm	db	"pow",0

SECTION code CLASS=CODE USE32

_powf:
    fld dword [esp+8]
    fld	dword [esp+4]
    sub dl,dl
    jmp short pow
_powl:
    fld tword [esp+16]
    fld	tword [esp+4]
    mov dl,2
    jmp short pow
_pow:
    fld qword [esp+12] ; y
    fld	qword [esp+4]  ; x
    mov dl,1
pow:
    sub cl,cl
    lea eax,[nm]
    call clearmath
    fxam
    fstsw ax
    and ah,45h
    cmp ah,40h
    jz  zero
    fstsw ax
    test ah,2
    jz nonneg
    fld st1
    sub esp,4
    fnstcw	[esp]
    mov	ax,[esp]
    and	ah,0f3h
    or	ah,4
    mov	[esp+2],ax
    fldcw	[esp+2]
    frndint
    fldcw	[esp]
    add esp,4
    fsub st2
    ftst
    fstsw ax
    sahf
    jnz err
    popone
    fld st1
    sub esp,8
    fistp qword [esp]
    call checkinvalid
    jc err2
    mov cl,[esp] ; sign bit in cl
    fabs
    add esp,8    
nonneg:
    fyl2x
    fld1
    push ecx
    call exp2
    pop ecx
    test cl,1
    jz wrapmath
    fchs
    jmp wrapmath
zero:
    fxch
    popone
    jmp wrapmath
err2:
    add esp,8
    popone
    jmp domainerr
err:
    poptwo
    jmp domainerr
