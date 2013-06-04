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
[export ___nancompare]
%endif
[global ___nancompare]

SECTION data CLASS=DATA USE32

section code CLASS=CODE USE32

___nancompare:
    mov ecx,[esp + 28]
    mov eax,0
    call clearmath
    fld tword [esp+4]  ; x
    fnclex
    fxam
    fnclex
    fstsw ax
    and ah,45h
    cmp ah,1
    jz retunordered1
    fld tword [esp+16] ; y
    fnclex
    fxam
    fnclex
    fstsw ax
    and ah,45h
    cmp ah,1
    jz retunordered2
    or ecx,ecx      ; unorder check?
    jz retfalse2
    fcompp
    fstsw ax
    test cl,1
    jnz notequal
    sahf
    jz rettrue
notequal:
    test cl,cl
    jns less
    sahf
    ja rettrue
    cmp cl,-3 ; less or greater
    je less
    jmp retfalse
less:
    sahf
    jb rettrue
retfalse:
    sub eax,eax
    ret
retfalse2:
    poptwo
    jmp retfalse
retunordered2:
    ffree st1
retunordered1:
    ffree st0
    or ecx,ecx
    jnz retfalse
rettrue:
    mov eax,1
    ret
