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
[export _cosh]
[export _coshf]
[export _coshl]
%endif
[global _cosh]
[global _coshf]
[global _coshl]
[extern expm1]
SECTION data CLASS=DATA USE32
nm	db	"cosh",0
SECTION code CLASS=CODE USE32
two	dw	2
_coshf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short cosh
_coshl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short cosh
_cosh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
cosh:
    lea eax,[nm]
    call    clearmath
    fld st0
    call	expm1
    fxch
    fchs
    call    expm1
    faddp	st1
    fild	word[two]
    fdivp	st1
    fld1
    faddp   st1
    jmp wrapmath
