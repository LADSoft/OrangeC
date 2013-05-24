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
[export _powf]
[export _pow]
[export _powl]
%endif
[global _powf]
[global _pow]
[global _powl]
[extern exp2]

SECTION data CLASS=DATA USE32
nm	db	"pow",0

SECTION code CLASS=CODE USE32

_powf:
    fld dword [esp+8]
    fld	dword [esp+4]
    sub dl,dl
    jmp short pow
_powl:
    fld tword [esp+16]
    fld	tword [esp+4]
    mov dl,2
    jmp short pow
_pow:
    fld qword [esp+12] ; y
    fld	qword [esp+4]  ; x
    mov dl,1
pow:
    sub cl,cl
    lea eax,[nm]
    call clearmath
    fxam
    fstsw ax
    and ah,45h
    cmp ah,40h
    jz  zero
    fstsw ax
    test ah,2
    jz nonneg
    fld st1
    sub esp,4
    fnstcw	[esp]
    mov	ax,[esp]
    and	ah,0f3h
    or	ah,4
    mov	[esp+2],ax
    fldcw	[esp+2]
    frndint
    fldcw	[esp]
    add esp,4
    fsub st2
    ftst
    fstsw ax
    sahf
    jnz err
    popone
    fld st1
    sub esp,8
    fistp qword [esp]
    call checkinvalid
    jc err2
    mov cl,[esp] ; sign bit in cl
    fabs
    add esp,8    
nonneg:
    fyl2x
    fld1
    push ecx
    call exp2
    pop ecx
    test cl,1
    jz wrapmath
    fchs
    jmp wrapmath
zero:
    fxch
    popone
    jmp wrapmath
err2:
    add esp,8
    popone
    jmp domainerr
err:
    poptwo
    jmp domainerr
