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
[export _atanh]
[export _atanhf]
[export _atanhl]
%endif
[global _atanh]
[global _atanhf]
[global _atanhl]
SECTION data CLASS=DATA USE32
nm	db	"atanh",0
small dt 4.7683715820312499e-07
two	dd	2.0

SECTION code CLASS=CODE USE32
_atanhf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short atanh
_atanhl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short atanh
_atanh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
atanh:
    lea	eax,[nm]
    call    clearmath
    fld	st0
    fabs
    fld1
    fcompp
    fstsw	ax
    sahf
    jbe		domainerr
    sub ecx,ecx
    fldz
    fcomp	st1
    fstsw	ax
    sahf
    jc	noneg
    inc ecx
noneg:
    fld	st0
    fabs
    fld	tword [small]
    fcomp	st1
    fstsw	ax
    sahf
    jbe	nosmall
    popone
    ret
nosmall:
    fxch	st1
    popone
    fld1
    fadd	st0,st1
    fxch	st1
    fld1
    fsubrp	st1
    fdivp	st1	
fini:
    fldln2
    fxch
    fyl2x
    fld	dword [two]
    fdivp	st1
    or	ecx,ecx
    jz	fini2
    fchs
fini2:
    jmp	wrapmath
