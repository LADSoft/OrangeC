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
;     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _cbrt]
[export _cbrtf]
[export _cbrtl]
%endif
[global _cbrt]
[global _cbrtf]
[global _cbrtl]
[extern exp2]
SECTION data CLASS=DATA USE32
nm	db	"cbrt",0

oneoverthree   dt 0.33333333333333333
SECTION code CLASS=CODE USE32
_cbrtf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short cbrt
_cbrtl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short cbrt
_cbrt:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
cbrt:
    lea	eax,[nm]
    call clearmath
    fxam
    fstsw ax
    mov al,ah
    and al,45h
    cmp al,40h
    jz retz
    fabs
    fld tword [oneoverthree]
    fxch
    fyl2x
    push eax
    fld1
    call exp2
    pop eax
    and ah,2h
    jz  wrapmath
    fchs
    jmp wrapmath
retz:
    popone
    fldz
    ret
