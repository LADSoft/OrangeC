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
[export _erf]
[export _erff]
[export _erfl]
[export _erfc]
[export _erfcf]
[export _erfcl]
%endif
[global _erf]
[global _erff]
[global _erfl]
[global _erfc]
[global _erfcf]
[global _erfcl]

[extern exp]

SECTION data CLASS=DATA USE32
nmf	db	"erf",0
nmc	db	"erfc", 0
logpidiv2 dt 0.57236494292470008707
onept5 dd 1.5
two	dd	2.0
pt5	dd	0.5
epsilon	dt 1.0E-30
fpmin	dt 1.0E-4930
SECTION code CLASS=CODE USE32
_erff:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short erf
_erfl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short erf
_erf:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
erf:
    lea eax,[nmf]
    call    clearmath
    fldz
    fcomp st1
    fstsw ax
    sahf
    jb	erfltz
    call	gammp
    jc		xit
    fchs	
    jmp		wrapmath
erfltz:
    call	gammp
    jnc		wrapmath
xit:
    ret
_erfcf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short erfc
_erfcl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short erfc
_erfc:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
erfc:
    lea eax,[nmc]
    call    clearmath
    fldz
    fcomp st1
    fstsw ax
    sahf
    jb	erfcltz
    call	gammp
    jc		xit
    fld1
    faddp	st1
    jmp		wrapmath
erfcltz:
    call	gammq
    jnc		wrapmath
    ret
    
gammp:
    fld	st0
    fmulp	st1
    fld	dword [onept5]
    fcomp st1
    fstsw ax
    sahf
    ja	gser
    call	gcf
    fld1
    fsubrp	st1
    clc
    ret
    
gammq:
    fld	st0
    fmulp	st1
    fld	dword [onept5]
    fcomp st1
    fstsw ax
    sahf
    jbe	near gcf
    call	gser
    fld1
    fsubrp	st1
    clc
    ret

gser:
    fldz
    fcomp	st1
    jz		seris0
    fld		dword [two]
    fld		st0
    fld		dword [pt5]
    mov		cx,1000
gsl:
    fld1
    faddp	st1
    fxch	st1
    
    fmul	st0, st3
    fdiv	st0, st1
    fadd	st2, st0
    fxch	st1
    fld		st1
    fdiv	st0,st3
    fld		tword [epsilon]
    fcompp
    fstsw	ax
    sahf
    ja		gse
    loop	gsl
    jmp		invalid
gse:
    poptwo
expres:
    fxch	st1
    fld	st0
    fldln2
    fxch
    fyl2x
    fld	dword [pt5]
    fmulp	st1
    fsubrp	st1
    fld	tword [logpidiv2]
    fsubp	st1
    call	exp
    fmulp	st1
    clc
    ret	
seris0:
    popone
    fldz
    ret
gcf:
    mov	ecx,1
    sub	esp, 12 * 3	

    fld1
    fld	tword [fpmin]
    fdivp	st1
    fstp	tword [esp + 12]
    
    fld	st0
    fld	dword [pt5]
    faddp	st1
    fld	st0
    fstp	tword [esp]
    
    fld1
    fdivrp	st1
    fld	st0
    fstp	tword [esp + 24]
    ; x h
gcfl:
    push ecx
    fild dword [esp]
    fld dword [pt5]
    fsubp	st1
    fild dword [esp]
    fmulp	st1
    fchs
    pop	ecx
    ; x h an
    fld	tword [esp]
    fld dword [two]
    faddp	st1
    fld	st0
    fstp	tword [esp]
    ; x h an b
    fld tword [esp + 24]
    fmul	st0,st2
    fadd	st0,st1
    ; x h an b d
    fld	st0
    fstp	tword [esp + 24]
    fabs
    fld tword [fpmin]
    fcompp
    fstsw	ax
    sahf
    jc	dok
    fld tword [fpmin]
    fstp tword [esp + 24]
dok:
    fld1
    fld	tword [esp + 24]
    fdivp	st1
    fstp tword [esp + 24]
    ; x h an b
    fld tword [esp + 12]
    fdivr	st0,st2
    fadd	st0,st1
    ; x h an b c
    fxch st2
    poptwo
    fld st0
    fstp	tword [esp + 12]
    fabs
    fld tword [fpmin]
    fcompp
    fstsw	ax
    sahf
    jc	cok
    fld tword [fpmin]
    fstp tword [esp + 12]
cok:
    ; x h
    fld tword [esp + 12]
    fld	tword [esp + 24]
    fmulp st1
    ; x h del
    fmul	st1,st0
    fld1
    fsubp	st1
    fabs
    fld	tword [epsilon]
    fcompp
    fstsw ax
    sahf
    ja	gcfok
    inc	ecx
    cmp	ecx,1000
    jl	gcfl
    add	esp, 12 * 3
    jmp	invalid
gcfok:
    add	esp,12 * 3
    jmp	expres
