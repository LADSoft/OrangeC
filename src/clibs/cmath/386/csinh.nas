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
[export _csinh]
[export _csinhf]
[export _csinhl]
%endif
[global _csinh]
[global _csinhf]
[global _csinhl]
[extern _cosl]
[extern _coshl]
[extern _sinl]
[extern _sinhl]
SECTION data CLASS=DATA USE32
nm	db	"csinh",0

SECTION code CLASS=CODE USE32

_csinhf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short csinh
_csinhl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    mov dl,2
    jmp short csinh
_csinh:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
csinh:
    lea eax,[nm]
    call    clearmath
    push edx
    sub esp,12
    fld st0
    fstp    tword [esp]
    call _coshl
    fld st2
    fstp tword [esp]
    call _sinl
    fmulp st1
    fxch st2
    fstp tword [esp]
    call _cosl
    fxch
    fstp tword [esp]
    call _sinhl
    fmulp st1
    add esp,12
    pop edx
    jmp wrapcomplex
