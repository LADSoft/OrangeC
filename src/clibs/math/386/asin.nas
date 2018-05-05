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
[export _asin]
[export _asinf]
[export _asinl]
%endif
[global _asin]
[global _asinf]
[global _asinl]
[global lasin]
SECTION data CLASS=DATA USE32
nm	db	"asin",0

SECTION code CLASS=CODE USE32
_asinf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short asin
_asinl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short asin
_asin:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
asin:
    lea eax,[nm]
    call    clearmath
lasin:
    fld1
    fcomp st1
    fstsw ax
    sahf
    jb domainerr
    je retpi2
    fld1
    fchs
    fcomp st1
    fstsw ax
    sahf
    ja domainerr
    je retminpi2
    fld	st0
    fld st0
    fmulp	st1
    fld1
    fxch 	st1
    fsubp	st1,st0
    fsqrt
    fdivp	st1,st0
    fld1
    fpatan
    jmp 	wrapmath
retminpi2:
    popone
    fld1
    fchs
    fldpi
    fchs
    fscale ; won't overflow
    fxch
    popone
    jmp wrapmath
retpi2:
    popone
    fld1
    fchs
    fldpi
    fscale ; won't overflow
    fxch
    popone
    jmp wrapmath