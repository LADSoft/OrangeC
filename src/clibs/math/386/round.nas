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

%ifdef __BUILDING_CRTL_DLL
[export _round]
[export _roundf]
[export _roundl]
%endif
[global _round]
[global _roundf]
[global _roundl]
[global round]
SECTION data CLASS=DATA USE32
nm	db	"round",0

two dd 2
SECTION code CLASS=CODE USE32
round:
    fld1
    fld st1
    fscale
    fxch
    popone ; input, *2
    fld st0
    frndint
    call checkinvalid
    jc xit1
    fsubp st1 ; input, possibly 0
    ftst
    fstsw ax
    sahf
    popone
    mov cl,0
    jnz nearest
    ftst
    fstsw ax
    sahf
    mov cl,8
    ja nearest
    mov cl,4
nearest:
    sub esp,4
    fnstcw	[esp]
    mov	ax,[esp]
    and	ah,0f3h
    or ah,cl
    mov	[esp+2],ax
    fldcw	[esp+2]
    frndint
    call checkinvalid
xit:
    fldcw	[esp]
    add esp,4
    ret
xit1:
    fxch
    popone
    jmp xit
_roundf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short roundx
_roundl:
    fld	tword[esp+4]
    mov dl,2
    jmp short roundx
_round:
    fld	qword[esp+4]
    mov dl,1
roundx:    
    lea eax,[nm]
    call clearmath
    call round
    ret