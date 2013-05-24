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
