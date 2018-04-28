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
[export _sqrt]
[export _sqrtf]
[export _sqrtl]
%endif
[global _sqrt]
[global _sqrtf]
[global _sqrtl]
SECTION data CLASS=DATA USE32
nm	db	"sqrt",0

SECTION code CLASS=CODE USE32
_sqrtf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short sqrt
_sqrtl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short sqrt
_sqrt:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
sqrt:
    lea	eax,[nm]
    call clearmath
    fldz
    fcomp st1
    fstsw ax
    sahf
    ja domainerr
    fsqrt
    jmp wrapmath
