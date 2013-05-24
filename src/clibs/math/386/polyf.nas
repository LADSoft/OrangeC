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
[export _polyf]
[export _polevlf]
[export _p1evlf]
%endif
[global _polyf]
[global _polevlf]
[global _p1evlf]

SECTION data CLASS=DATA USE32
nm	db	"polyf",0
nmo db  "polevlf",0
nm1 db  "p1evlf" , 0
SECTION code CLASS=CODE USE32
_polyf:
    lea	eax,[nm]
    call clearmath
    mov	ecx,[esp+8]
    mov	eax,[esp+12]
    lea	eax,[eax + ecx * 4]
    fld	dword [esp+4]
    fld	dword [eax]
    sub	eax,4
lp:
    fmul	st0,st1
    fld		dword [eax]
    sub		eax,4
    faddp	st1
    loop	lp
    fxch
    popone
    mov dl,1
    jmp wrapmath

_polevlf:
    lea	eax,[nmo]
    call clearmath
    mov	eax,[esp+8]
    mov	ecx,[esp+12]
    fld	dword [esp+4]
    fld	dword [eax]
    add	eax,4
lpo:
    fmul	st0,st1
    fld		dword [eax]
    add		eax,4
    faddp	st1
    loop	lpo
    fxch
    popone
    mov dl,1
    jmp wrapmath
_p1evlf:
    lea	eax,[nm1]
    call clearmath
    mov	eax,[esp+8]
    mov	ecx,[esp+12]
    fld	dword [esp+4]
    fld	st0
    jmp	intoo
lp1:
    fmul	st0,st1
intoo:
    fld		dword [eax]
    add		eax,4
    faddp	st1
    loop	lp1
    fxch
    popone
    mov dl,1
    jmp wrapmath
