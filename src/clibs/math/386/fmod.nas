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
[export _fmod]
[export _fmodf]
[export _fmodl]
%endif
[global _fmod]
[global _fmodf]
[global _fmodl]
SECTION data CLASS=DATA USE32
nm	db	"fmod",0

SECTION code CLASS=CODE USE32
_fmodf:
    lea	ecx,[esp+4]
    lea	edx,[esp+8]
    fld	dword[ecx]
    fld	dword[edx]
    sub dl,dl
    jmp short fmod
_fmodl:
    lea	ecx,[esp+4]
    lea	edx,[esp+16]
    fld	tword[ecx]
    fld	tword[edx]
    mov dl,2
    jmp short fmod
_fmod:
    lea	ecx,[esp+4]
    lea	edx,[esp+12]
    fld	qword[ecx]
    fld	qword[edx]
    mov dl,1
fmod:
    lea eax,[nm]
    call clearmath
    ftst
    fnstsw ax
    sahf
    mov cl,ah
    jz moderr
    fxch
    fld st1
    fld st1
reprem:
    fprem1
    fstsw ax
    sahf
    jp reprem
    fxch
    ftst    ;y
    fnstsw ax
    mov ch,ah
    popone
    ftst    ;result
    fnstsw ax
    mov cl,ah
    fxch
    ftst    ;x
    fnstsw ax
    popone
    test cl,40h ; result zero?
    jnz zeradj
    xor ah,cl
    test ah,1 ; carry bit = sign
    jz noadj
    xor cl,ch
    test cl,1
    jnz addadj
    fxch
    fsubp st1
    jmp wrapmath
zeradj:
    test ah,1
    jz noadj
    fchs
    jmp noadj
addadj:
    fadd st1
noadj:
    fxch
    popone
    jmp wrapmath
moderr:
    popone
    jmp domainerr