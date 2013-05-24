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
[export _cpow]
[export _cpowf]
[export _cpowl]
%endif
[global _cpow]
[global _cpowf]
[global _cpowl]
[extern exp]
[extern _cosl]
[extern _sinl]
[extern _cargl]
[extern _powl]
[extern _hypotl]
[extern _logl]
SECTION data CLASS=DATA USE32
nm	db	"cpow",0

SECTION code CLASS=CODE USE32

_cpowf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short xexp
_cpowl:
    lea	ecx,[esp+4]
    fld tword[ecx+10]
    fld	tword[ecx]
    mov dl,2
    jmp short xexp
_cpow:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
xexp:
    lea eax,[nm]
    call clearmath
    push edx
    push ebp
    mov ebp,esp
    sub esp,24
    fstp tword [esp]
    fstp tword [esp+12]
    call _hypotl
    ftst
    fnstsw ax
    sahf
    jnz  xnotzero
    popone
    fldz
    fldz
    leave
    pop edx
    jmp wrapcomplex
xnotzero:
    sub esp,24
    fstp tword [esp] ;absa
    cmp byte [ebp+4],1 ; get x
    js float1
    je dbl1
    fld tword [ebp + 12 + 20]
    jmp join1
float1:
    fld dword [ebp + 12 + 8]
    jmp join1
dbl1:
    fld qword [ebp + 12 + 16] 
join1:
    fstp tword [esp+12]   
    call _powl  ;r
    fld tword [esp+12] ; x
    fld  tword [esp] ; absa
    fld tword [esp+24]
    fstp tword [esp]
    fstp tword [esp+24] ; absa new location
    fld tword [esp+36]
    fstp tword [esp+10]
    call _cargl ;arga
    fld st0
    fstp tword [ esp + 36] ; arga
    fmulp st1
    fstp tword [esp+12] ; theta
    cmp byte [ebp+4],1 ; y
    js float2
    je dbl2
    fld tword [ebp + 12 + 30]
    jmp join2
float2:
    fld dword [ebp + 12 + 12]
    jmp join2
dbl2:
    fld qword [ebp + 12 + 24] 
join2:
    ftst    ; r,y
    fnstsw ax
    sahf
    jz yzero
    fld st0
    fchs
    fld tword [esp+36]
    fmulp st1
    call exp
    fmulp st2       ; r,y
    fld tword [esp+24]
    fstp tword [esp]
    call _logl
    fmulp st1     ; r,y*logl(arga)
    fld tword [esp+12]
    faddp st1          ; r,theta
    fstp tword [esp + 12] ; r
    jmp join3
yzero:
    popone
join3:
    fld tword [esp + 12]
    fstp tword [esp]
    call _sinl
    fmul st1
    fxch
    call _cosl
    fmulp st1
    leave
    pop edx
    jmp wrapcomplex
