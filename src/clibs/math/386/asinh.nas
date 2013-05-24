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
[export _asinh]
[export _asinhf]
[export _asinhl]
%endif
[global _asinh]
[global _asinhf]
[global _asinhl]
[extern _hypotl]
[extern lexp1]
SECTION data CLASS=DATA USE32
nm	db	"asinh",0
small dt 4.7683715820312499e-07
medium dt 2.44140625e-4
big dq	4294967296.0
threeforty dt 0.075
m16 dt - 1.66666666666666666667E-1
SECTION code CLASS=CODE USE32
_asinhf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short asinh
_asinhl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short asinh
_asinh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
asinh:
    lea	eax,[nm]
    call    clearmath
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
    fld	tword [medium]
    fcomp	st1
    fstsw	ax
    sahf
    jbe	nomedium
    fld	st0
    fmul	st0,st0
    ; z z^2
    fld		tword [threeforty]
    fmul	st0,st1
    ; z z^2 z^2a
    fld		tword [m16]
    faddp	st1
    fmulp	st1
    fld1
    faddp	st1
    fmulp	st1
    jmp	fini3
nomedium:
    fld	qword [big]
    fcomp	st1
    fstsw	ax
    sahf
    jae		nobig
    fadd	st0,st0
    jmp		fini
nobig:
    fld	st0
    fmul	st0,st0
    fld1
    faddp	st1
    fsqrt
    faddp st1
fini:
    fldln2
    fxch
    fyl2x
fini3:
    or	ecx,ecx
    jz	fini2
    fchs
fini2:
    jmp	wrapmath
