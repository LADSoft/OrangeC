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
[export _frexp]
[export _frexpf]
[export _frexpl]
%endif
[global _frexp]
[global _frexpf]
[global _frexpl]
SECTION data CLASS=DATA USE32
nm	db	"frexp",0

SECTION code CLASS=CODE USE32

two	dw	2

_frexpf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    mov	ecx,[esp+8]
    sub dl,dl
    jmp short frexp
_frexpl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov	ecx,[esp+16]
    mov dl,2
    jmp short frexp
_frexp:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov	ecx,[esp+12]
    mov dl,1
frexp:
    lea eax,[nm]
    call clearmath
    fldz
    fcomp   st1
    fstsw   ax
    sahf
    jz  zero
    fxtract
    fxch	st1
    fld1
    faddp st1,st0
    jmp join
zero:
    fldz
join:
    fistp   dword[ecx]
    fild	word[two]
    fdivp	st1
    jmp wrapmath
