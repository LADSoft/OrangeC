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
[export _csqrt]
[export _csqrtf]
[export _csqrtl]
%endif
[global _csqrt]
[global _csqrtf]
[global _csqrtl]
[extern _cabsl]

SECTION data CLASS=DATA USE32
nm	db	"csqrt",0

two dd  2
four dd 4
twoe66 dw 0,0,0,800h,3fffh+66
twoem33 dw 0,0,0,800h,3fffh-33

SECTION code CLASS=CODE USE32

_csqrtf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    jmp short csqrt
_csqrtl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    jmp short csqrt
_csqrt:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
csqrt:
    lea eax,[nm]
    call clearmath
    ftst
    fstsw ax
    sahf
    jnz xnot0
    popone
    ftst
    fnstsw ax
    fabs
    fidiv dword [two]
    fsqrt
    fld st0
    sahf
    jnc wrapcomplex
    fchs
    fxch
    jmp wrapcomplex
xnot0:
    fxch
    ftst
    fstsw ax
    sahf
    jnz ynot0
    popone
    ftst
    fstsw ax
    sahf
    jnc y0real
    fchs
    fsqrt
    fldz
    jmp wrapcomplex
y0real:
    fsqrt
    fldz
    fxch
    jmp wrapcomplex
ynot0:    
    push edx
    push ebp
    mov ebp,esp
    sub esp,12
    fld st0
    fabs
    ficomp dword [four]
    ja downsize
    fld st1
    fabs
    ficomp dword [four]
    jbe upsize
downsize:
    fidiv dword [four]
    fxch
    fidiv dword [four]
    fild dword [two]
    fstp tword [esp]
    jmp sizejoin
upsize:
    fld tword [twoe66]
    fmulp st1
    fxch
    fld tword [twoe66]
    fmulp st1
    fld dword [twoem33]
    fstp tword [esp]
sizejoin:
    sub esp,20
    fstp tword [esp]
    fstp tword [esp + 10]
    call _cabsl
    fidiv dword [two]
    fld tword [esp]
    ftst
    fnstsw ax
    fidiv dword [two]
    fabs
    faddp st1
    fsqrt
    fld tword [esp + 10]
    fidiv dword [two]
    fdiv st1
    fabs
    fld tword [ebp-12]
    fmulp st1
    fxch
    fld tword [ebp-12]
    fmulp st1
    sahf
    jbe xlessjn
    fxch
xlessjn:
    fld tword [esp+10]
    ftst
    fnstsw ax
    popone
    sahf
    jae ygreatjn
    fchs
ygreatjn:
    fxch
    leave
    pop edx
    jmp wrapcomplex
