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
[export _catan]
[export _catanf]
[export _catanl]
%endif
[global _catan]
[global _catanf]
[global _catanl]
[extern _logl]
[extern _atan2l]
[extern redupil]

SECTION data CLASS=DATA USE32
nm	db	"catan",0

two dd 2
four dd 4
SECTION code CLASS=CODE USE32

_catanf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short catan
_catanl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    mov dl,2
    jmp short catan
_catan:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
catan:
    lea eax,[nm]
    call clearmath
    push edx
    sub esp,20
    fldz
    fcomp st1
    fnstsw ax
    sahf
    jnz  ok1
    fld1
    fcomp st2
    fnstsw ax
    sahf
    jae ok1
ovrf:
    add esp,20
    pop edx
    poptwo
    fld tword [large_t]
    fld st0
    call overflow
    jmp wrapcomplex
ovrf2:
    popone
    jmp ovrf
ok1:
    fld st0
    fmul st0
    fld st0
    fstp tword [esp+10] ; x^2
    fld st2
    fmul st0
    faddp st1
    fchs
    fld1
    faddp st1
    ftst
    fnstsw ax
    sahf
    jz ovrf2
    sub esp,24
    fstp tword [esp+12]
    fimul dword [two]
    fstp tword [esp]
    call _atan2l
    add esp,24
    fidiv dword [two]
    call redupil
    fld st1
    fld1
    fsubp st1
    fld st0
    fmulp st1
    fld tword [esp+10]
    faddp st1
    
    fxch st2
    fld1
    faddp st1
    fld st0
    fmulp st1
    fld tword [esp+10]
    faddp st1
    fdivp st2
    fxch
    fstp tword [esp]
    call _logl
    fidiv dword[four]
    fxch
    add esp,20
    pop edx
    jmp wrapcomplex    
