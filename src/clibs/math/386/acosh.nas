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
[export _acosh]
[export _acoshf]
[export _acoshl]
%endif
[global _acosh]
[global _acoshf]
[global _acoshl]
[extern lexp1]

SECTION data CLASS=DATA USE32
nm	db	"acosh",0

pointfive dd 0.5

SECTION code CLASS=CODE USE32
_acoshf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short acosh
_acoshl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short acosh
_acosh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
acosh:
    lea	eax,[nm]
    call    clearmath
    fld1
    fcomp st1
    fstsw ax
    sahf
    ja domainerr
    
    fld st0
    fld1
    fsubp st1
    fsqrt

    fld st1
    fld1
    faddp st1
    fsqrt

    fmulp st1

    faddp	st1
    fldln2
    fxch
    fyl2x
    jmp wrapmath
