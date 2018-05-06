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
[export _cacos]
[export _cacosf]
[export _cacosl]
%endif
[global _cacos]
[global _cacosf]
[global _cacosl]
[extern _casinl]
SECTION data CLASS=DATA USE32
nm	db	"cacos",0

two dd 2
SECTION code CLASS=CODE USE32

_cacosf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short cacos
_cacosl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    mov dl,2
    jmp short cacos
_cacos:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
cacos:
    lea eax,[nm]
    call clearmath
    push edx
    sub esp,20
    fstp tword [esp]
    fstp tword [esp+10]
    call _casinl
    fxch
    fchs
    fxch
    fldpi
    fidiv dword [two]
    fsubrp st1
    add esp,20
    pop edx
    jmp wrapcomplex
