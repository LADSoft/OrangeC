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

%define DOMAIN 1
%define SING 2
%define OVERFLOW 3
%define UNDERFLOW 4
%define TLOSS 5
%define STACKFAULT 7

%macro popone 0
    fstp	tword [poptemp]
%endmacro
%macro poptwo 0
    fstp	tword [poptemp]
    fstp	tword [poptemp]
%endmacro

[global poptemp]
[global trigdomain]
[global sinularerr]
[global mnegerr]
[global mnegeerr]
[global wrapmath]
[global clearmath]
[global domainerr]
[global checkedscale]
[global checkinvalid]
[global overflow]
[global wrapcomplex]
[global large_t]
[global invalid]
[extern ___matherr]
[extern ___GetErrno]

SECTION data CLASS=DATA USE32
poptemp	dt	0.0
;
; next fields MUST be in this order and with no interruptions!!!
;
sixtythree dd	63.0
large_f     dd 07f7fffffh
large_d     dd 0ffffffffh,07fefffffh
large_t     dw 0ffffh,0ffffh,0ffffh,0ffffh,07ffeh
;
;
funcname    dd  0
xceptcount  dd  0

SECTION code CLASS=CODE USE32
;
; various common errors
;
trigdomain:
    sub	edx,edx
    fldz
    fcomp   st1
    fstsw ax
    sahf
    stc
    jz  tex
    fld st0
    fxtract
    popone
    fabs
    fcomp	dword[sixtythree]
    wait
    fnstsw	ax
    wait
    sahf
    jc	tex
    popone
    push edx
    push    TLOSS
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    call	___GetErrno
    mov	dword [eax],33 ; EDOM
    clc
tex:
    cmc
    ret

sinularerr:
    fcomp st1
    fnstsw	ax
    wait
    sahf
    clc
    jnz	senone
    popone
    push edx
    push    SING
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    call	___GetErrno
    mov	dword [eax],33 ; EDOM
    stc
senone:
    ret

mnegeerr:
    fldz
    fcomp st1
    fnstsw	ax
    wait
    sahf
    jae	ner
    clc
    ret

mnegerr:
    fldz
    fcomp st1
    fnstsw	ax
    wait
    sahf
    ja	ner
    clc
    ret
ner:
    popone
    push edx
    push    DOMAIN
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    call	___GetErrno
    mov	dword [eax],33 ; EDOM
    stc
    ret
checkinvalid:
    fnstsw ax   ; using this function in _ftol, so it is an overflow
    test al,1
    jz cir
    call overflow
    stc
cir:
    ret
checkedscale:
    fscale
    fnstsw ax
    test al,8
    jz nooverflow
    fnclex
    poptwo
    fld tword [large_t]
    call overflow
    stc
nooverflow:
    ret
finalmath:
    fnstsw	ax
    and	al,59h	;SUOI
    jnz	cont
    ret
cont:
    test	al,40h
    jz	notsf
    fnclex
    push edx
    push    STACKFAULT
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    jmp	noaerr
notsf:
    test	al,1
    jz	notie
invalid:
    fnclex
    popone
    push edx
    push    TLOSS
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    call	___GetErrno
    mov	dword [eax],34 ; ERANGE
    jmp	noaerr
notie:
    test	al,8
    jz	notoe
overflow:
    fnclex
    push edx
    push    OVERFLOW
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    call	___GetErrno
    mov	dword [eax],34 ; ERANGE
    popone
    fnclex
    jmp	noaerr

notoe:
    test	al,10h
    jz	noaerr
    fnclex
    popone
    push edx
    push    UNDERFLOW
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    call	___GetErrno
    mov	dword [eax],34 ; ERANGE
noaerr:
    ret
domainerr:
    popone
    push edx
    push    DOMAIN
    push    dword [funcname]
    call    ___matherr
    pop edx
    inc     dword [xceptcount]
    call	___GetErrno
    mov	dword [eax],33 ; EDOM
    ret
wrapmath:
    cmp dl,1
    js mwf
    je mwd
    jmp finalmath
mwf:
    push ecx
    fst dword [esp] ; // force overflow/underflow
    fnstsw ax
    test al,8
    jz mwfldx
    fnclex
    mov ecx,[large_f]
    fxam
    push eax
    fstsw ax
    test ah,2
    pop eax
    jz mwfld2
    or ecx,80000000h
mwfld2:
    mov [esp],ecx
    popone
    fld dword [esp]
mwfldx:
    pop ecx
    jmp mwjoin
mwd:
    sub esp,8
    fst qword [esp] ; // force overflow/underflow
    fnstsw ax
    test al,8
    jz mwdldx
    fnclex
    mov ecx,[large_d]
    mov [esp],ecx
    mov ecx,[large_d+4]
    fxam
    push eax
    fstsw ax
    test ah,2
    pop eax
    jz mwdld2
    or ecx,80000000h
mwdld2:
    mov [esp+4],ecx
    popone
    fld qword [esp]
mwdldx:
    add esp,8
mwjoin:
    test al,8
    jz finalmath
    test dword [xceptcount],-1
    jz overflow
    ret

wrapcomplex:
    call wrapmath
    fxch
    call wrapmath
    fxch
    ret

clearmath:
    mov dword [xceptcount],0
    mov dword [funcname],eax
    fnclex
    ret
