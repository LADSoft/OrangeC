;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
