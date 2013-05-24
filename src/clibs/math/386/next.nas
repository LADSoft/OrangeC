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
[export _nextafter]
[export _nextafterf]
[export _nextafterl]
[export _nexttoward]
[export _nexttowardf]
[export _nexttowardl]
%endif
[global _nextafter]
[global _nextafterf]
[global _nextafterl]
[global _nexttoward]
[global _nexttowardf]
[global _nexttowardl]
SECTION data CLASS=DATA USE32
nm	db	"nexttoward",0
;1111111
;0010111
;1101000
; 340

; 3ff
;  34
; 3dc

; 3fff
;   40
; 3fbf
small_f dd 34000000h
really_small_f dd 1
small_d dd 0,3cb00000h
really_small_d dd 1,0
small_l dw 0,0,0,8000h,3fc0h
really_small_l dw 1,0,0,0,0

SECTION code CLASS=CODE USE32
_nextafterf:
    fld	dword [esp+4]
    fld	dword [esp+8]
    sub esp,16
    fstp tword[esp+4]
    fstp dword[esp]
    call _nexttowardf
    add esp,16
    ret
_nextafterl:
    fld	tword [esp+4]
    fld	tword [esp+16]
    sub esp,24
    fstp tword[esp+12]
    fstp tword[esp]
    call _nexttowardl
    add esp,24
    ret
_nextafter:
    fld	qword [esp+4]
    fld	qword [esp+12]
    sub esp,20
    fstp tword[esp+8]
    fstp qword[esp]
    call _nexttoward
    add esp,20
    ret
_nexttowardf:
    lea eax,[nm]
    call clearmath
    sub dl,dl
    fld dword [esp+4]
    fld tword [esp+8]
    fcomp st1
    fstsw ax
    sahf
    je wrapmath
    jc fdown
    ftst
    fstsw ax
    sahf
    je frsmall
    fld st0
    fxtract
    popone
    fld dword [small_f]
    fscale
    fxch
    popone
    faddp st1
    jmp wrapmath
frsmall:
    popone
    fld dword [really_small_f]
    jmp wrapmath
fdown:
    ftst
    fstsw ax
    sahf
    je mfrsmall
    fld st0
    fxtract
    popone
    fld dword [small_f]
    fscale
    fxch
    popone
    fsubp st1
    jmp wrapmath
mfrsmall:
    popone
    fld dword [really_small_f]
    fchs
    jmp wrapmath
_nexttowardl:
    lea eax,[nm]
    call clearmath
    mov dl,2
    fld tword [esp+4]
    fld tword [esp+16]
    fcomp st1
    fstsw ax
    sahf
    je wrapmath
    jc ldown
    ftst
    fstsw ax
    sahf
    je lrsmall
    fld st0
    fxtract
    popone
    fld tword [small_l]
    fscale
    fxch
    popone
    faddp st1
    jmp wrapmath
lrsmall:
    popone
    fld tword [really_small_l]
    jmp wrapmath
ldown:
    ftst
    fstsw ax
    sahf
    je mlrsmall
    fld st0
    fxtract
    popone
    fld tword [small_l]
    fscale
    fxch
    popone
    fsubp st1
    jmp wrapmath
mlrsmall:
    popone
    fld tword [really_small_l]
    fchs
    jmp wrapmath
_nexttoward:
    lea eax,[nm]
    call clearmath
    mov dl,1
    fld qword [esp+4]
    fld tword [esp+12]
    fcomp st1
    fstsw ax
    sahf
    je wrapmath
    jc down
    ftst
    fstsw ax
    sahf
    je rsmall
    fld st0
    fxtract
    popone
    fld qword [small_d]
    fscale
    fxch
    popone
    faddp st1
    jmp wrapmath
rsmall:
    popone
    fld qword [really_small_d]
    jmp wrapmath
down:
    ftst
    fstsw ax
    sahf
    je mrsmall
    fld st0
    fxtract
    popone
    fld qword [small_d]
    fscale
    fxch
    popone
    fsubp st1
    jmp wrapmath
mrsmall:
    popone
    fld qword [really_small_d]
    fchs
    jmp wrapmath
