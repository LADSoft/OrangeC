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

%ifdef _BUILDING_LSCRTL_DLL
[export _remquo]
[export _remquof]
[export _remquol]
%endif
[global _remquo]
[global _remquof]
[global _remquol]
SECTION data CLASS=DATA USE32
nm	db	"remquo",0

eight dd 8

SECTION code CLASS=CODE USE32
_remquof:
    lea	ecx,[esp+4]
    lea	edx,[esp+8]
    fld	dword[edx]
    fld	dword[ecx]
    mov ecx,[esp+12]
    sub dl,dl
    jmp short remquo
_remquol:
    lea	ecx,[esp+4]
    lea	edx,[esp+16]
    fld	tword[edx]
    fld	tword[ecx]
    mov ecx,[esp+28]
    mov dl,2
    jmp short remquo
_remquo:
    lea	ecx,[esp+4]
    lea	edx,[esp+12]
    fld	qword[edx]
    fld	qword[ecx]
    mov ecx,[esp+20]
    mov dl,1
remquo:
    lea eax,[nm]
    call clearmath
    fxch
    ftst
    fxch
    fnstsw ax
    sahf
    jz moderr
    fld st1
    fld st1
reprem:
    fprem1
    fstsw ax
    sahf
    jp reprem
    fxch
    popone
    fxch st2
    fdivp st1
    fild dword [eight]
    fld st1
reprem2:
    fprem1 
    fstsw ax
    sahf
    jp reprem2
    frndint
    fxch
    fcomp st1
    fxch
    ftst
    fstsw ax
    mov dh,ah
    fxch
    ftst
    fstsw ax
    fxch
    popone
    sahf
    jae noadjust
    fild dword [eight]
    faddp st1
noadjust:
    je noadj2
    mov ah,dh
    sahf
    jae noadj2
    fild dword [eight]
    fchs
    faddp st1
noadj2:
    fistp dword [ecx]    
    jmp wrapmath
moderr:
    popone
    jmp domainerr