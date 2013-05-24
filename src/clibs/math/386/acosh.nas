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
[export _acosh]
[export _acoshf]
[export _acoshl]
%endif
[global _acosh]
[global _acoshf]
[global _acoshl]
[extern lexp1]

SECTION data CLASS=DATA USE32
nm	db	"acosh",0

pointfive dd 0.5

SECTION code CLASS=CODE USE32
_acoshf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short acosh
_acoshl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short acosh
_acosh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
acosh:
    lea	eax,[nm]
    call    clearmath
    fld1
    fcomp st1
    fstsw ax
    sahf
    ja domainerr
    
    fld st0
    fld1
    fsubp st1
    fsqrt

    fld st1
    fld1
    faddp st1
    fsqrt

    fmulp st1

    faddp	st1
    fldln2
    fxch
    fyl2x
    jmp wrapmath
