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
