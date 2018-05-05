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
[export _acos]
[export _acosf]
[export _acosl]
%endif
[global _acos]
[global _acosf]
[global _acosl]

SECTION data CLASS=DATA USE32
nm	db	"acos",0

SECTION code CLASS=CODE USE32
_acosf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short acos
_acosl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short acos
_acos:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
acos:
    lea eax,[nm]
    call    clearmath
    fld st0
    fabs
    fld1
    fcompp
    fstsw ax
    sahf
    jb domainerr
    fld	st0
    fld st0
    fmulp	st1
    fld1
    fxch 	st1
    fsubp	st1,st0
    fsqrt
    fxch
    fdivp	st1,st0
    fld1
    fpatan
    fldz
    fcomp st1
    fstsw ax
    sahf
    jc nosub
    fldpi
    faddp st1
nosub:
    jmp	wrapmath
