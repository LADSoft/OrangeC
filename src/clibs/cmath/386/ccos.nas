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
[export _ccos]
[export _ccosf]
[export _ccosl]
%endif
[global _ccos]
[global _ccosf]
[global _ccosl]
[extern exp]
SECTION data CLASS=DATA USE32
nm	db	"ccos",0

SECTION code CLASS=CODE USE32

_ccosf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    fld dword[ecx+4]
    jmp short ccos
_ccosl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    fld tword[ecx+10]
    jmp short ccos
_ccos:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    fld qword[ecx+8]
ccos:
    lea eax,[nm]
    call    clearmath
    push    ebp
    mov     ebp,esp
    sub     esp,20
    call    exp
    fld     st0
    fstp    tword [ebp-20]
    fld1
    fxch
    fdivp   st1
    fstp    tword [ebp-10]
    fld     tword [ebp-10]
    fld     tword [ebp-20]
    fsubp   st1
    fld st1
    fsin
    fmulp st1
    fld1
    fchs
    fxch
    fscale
    fxch
    fcomp   st0
    fxch
    fcos
    fld     tword [ebp-10]
    fld     tword [ebp-20]
    faddp   st1
    fmulp st1
    fld1
    fchs
    fxch
    fscale
    fxch
    fcomp   st0
    leave
    jmp wrapcomplex
