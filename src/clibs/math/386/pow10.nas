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
[export _pow10]
[export _pow10f]
[export _pow10l]
%endif
[global _pow10]
[global _pow10f]
[global _pow10l]

[extern exp2]
SECTION data CLASS=DATA USE32
nm	db	"pow10",0

SECTION code CLASS=CODE USE32

_pow10f:
    sub dl,dl
    jmp pow10
_pow10l:
    mov dl,2
    jmp pow10
_pow10:
    mov dl,1
pow10:
    lea	eax,[nm]
    call clearmath
    lea	ecx,[esp+4]
    fild	dword[ecx]
    fldl2t
    call	exp2
    jmp wrapmath
