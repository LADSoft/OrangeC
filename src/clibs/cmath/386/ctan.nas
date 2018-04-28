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
[export _ctan]
[export _ctanf]
[export _ctanl]
%endif
[global _ctan]
[global _ctanf]
[global _ctanl]
[global ctansl]
[global redupil]
[extern _cosl]
[extern _sinl]
[extern _coshl]
[extern _sinhl]
[extern __ftol]
SECTION data CLASS=DATA USE32
nm	db	"ctan",0

two dd 2
pointfive dq 0.5
onequarter dq 0.25
dp1 dw 0c234h,02168h,0daa2h,0c90fh,04000h
dp2 dw 01cd1h,080dch,0628bh,0c4c6h,03fc0h
dp3 dw 031d0h,0299fh,03822h,0a409h,03f7eh
epsilon dw 0,0,0,8000h,3fffh-64 ; 0x1P-64

SECTION code CLASS=CODE USE32

_ctanf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short ctan
_ctanl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    mov dl,2
    jmp short ctan
_ctan:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
ctan:
    lea eax,[nm]
    call clearmath
    push edx
    sub esp,20
    fimul dword [two]
    fld st0
    fstp tword [esp]
    call _cosl
    fxch st2
    fimul dword [two]
    fld st0
    fstp tword [esp]
    fxch st2
    call _coshl
    faddp st1
    fld st0
    fstp tword [esp+10] ; d
    ftst
    fstsw ax
    sahf
    ja nochs
    fchs
nochs:
    fcomp qword [onequarter]
    fnstsw ax
    sahf
    ja notansl
    call ctansl
    fstp tword [esp+10]
notansl:
    fld tword [esp+10]
    fldz
    fcompp
    fnstsw ax
    sahf
    jnz ok
    poptwo
    call overflow
    fld tword [large_t]
    fld st0
    add esp,20
    pop edx
    jmp wrapcomplex
ok:
    fstp tword [esp]
    call _sinl
    fld tword [esp+10]
    fdivp st1
    fxch
    fstp tword [esp]
    call _sinhl
    fld tword [esp+10]
    fdivp st1
    fxch
    add esp,20
    pop edx
    jmp wrapcomplex

redupil:
    fld st0
    fldpi
    fdivp st1
    ftst
    fstsw ax
    sahf
    jc lt
    fadd qword [pointfive]
    jmp join
lt
    fsub qword [pointfive]
join:
    call __ftol
    push eax
    fld tword [dp1]
    fimul dword [esp]
    fsubp st1
    fld tword [dp2]
    fimul dword [esp]
    fsubp st1
    fld tword [dp3]
    fimul dword [esp]
    fsubp st1
    pop eax
    ret
; series expansion for cosh(2y) - cos(2x)
;
ctansl:
%define y ebp-10
%define x ebp-20
%define n ebp-24
%define f ebp-36
%define d ebp-48
%define zi ebp-58
%define zr ebp-68
    push ebp
    mov ebp,esp
    sub esp,68
    fld st0
    fstp tword [zr]
    call redupil
    fld st0
    fmulp st1
    fstp tword [x]
    fld st0
    fstp tword [zi]
    fld st0
    fmulp st1
    fstp tword [y]
    mov dword [n],0
    fld1
    fstp tword [f]
    fldz
    fstp tword [d]
    fld1
    fld1
slp:
    inc dword [n]
    fld tword [f]
    fimul dword [n]
    fstp tword [f]
    inc dword [n]
    fld tword [f]
    fimul dword [n]
    fstp tword [f]
    fld tword [x]
    fmulp st1
    fxch 
    fld tword [y]
    fmulp st1
    fld st0
    fadd st2
    fld tword [f]
    fdivp st1
    fld tword [d]
    faddp st1
    fstp tword [d]
    fxch
    
    inc dword [n]
    fld tword [f]
    fimul dword [n]
    fstp tword [f]
    inc dword [n]
    fld tword [f]
    fimul dword [n]
    fstp tword [f]
    
    fld tword [x]
    fmulp st1
    fxch 
    fld tword [y]
    fmulp st1
    fld st0
    fsub st2

    fld tword [f]
    fdivp st1    
    fld tword [d]
    fadd st1
    fld st0
    fstp tword [d]
    
    fdivp st1
    fabs
    fld tword [epsilon]
    fcompp
    fstsw ax
    fxch
    sahf
    ja slp
    fcompp
    fld tword [zi]
    fld tword [zr]
    fld tword [d]
    leave
    ret
