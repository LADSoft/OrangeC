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
[export _catanh]
[export _catanhf]
[export _catanhl]
%endif
[global _catanh]
[global _catanhf]
[global _catanhl]
[extern _catanl]
SECTION data CLASS=DATA USE32
nm	db	"catanh",0

SECTION code CLASS=CODE USE32

_catanhf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short catanh
_catanhl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    mov dl,2
    jmp short catanh
_catanh:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
catanh:
    lea eax,[nm]
    call clearmath
    fxch
    fchs
    push edx
    sub esp,20
    fstp tword [esp]
    fstp tword [esp+10]
    call _catanl
    fchs
    fxch
    add esp,20
    pop edx
    jmp wrapcomplex
