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
[export _exp]
[export _expf]
[export _expl]
%endif
[global _exp]
[global _expf]
[global _expl]
[global exp]
[global exp2]
[global expm1]

SECTION data CLASS=DATA USE32
nm	db	"exp",0

SECTION code CLASS=CODE USE32

expm1:
    fldl2e
    mov cl,1
    jmp exp2x
exp:
    fldl2e
exp2:
    sub cl,cl
exp2x:
    fmulp	st1
    enter	4,0
    fstcw	[ebp-4]
    mov	ax,[ebp-4]
    and	ah,0f3h
    or	ah,0ch
    mov	[ebp-2],ax
    fldcw	[ebp-2]
    fld	st0
    frndint
    fxch	st1
    fsub	st0,st1
    f2xm1
    fxch
    fxam
    fnstsw ax
    fxch
    test ah,40h
    jz doadd1
    test cl,1
    jnz noadd1
doadd1:
    or cl,2
    fld1
    faddp	st1,st0
noadd1:
    call checkedscale
    jc  xit
    cmp cl,3
    jnz nosub1
    fld1
    fsubp st1
nosub1:
    fxch	st1
    popone
xit:
    fldcw	[ebp-4]
    leave
    ret

_expf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short xexp
_expl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short xexp
_exp:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
xexp:    
    lea eax,[nm]
    call    clearmath
    call	exp
    jmp wrapmath