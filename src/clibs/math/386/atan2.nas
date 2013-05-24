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
[export _atan2]
[export _atan2f]
[export _atan2l]
%endif
[global _atan2]
[global _atan2f]
[global _atan2l]

SECTION data CLASS=DATA USE32
nm	db	"atan2",0

SECTION code CLASS=CODE USE32
_atan2f:
    lea	ecx,[esp+4]
    lea	edx,[esp+8]
    fld	dword[edx]
    fld	dword[ecx]
    sub dl,dl
    jmp short atan2
_atan2l:
    lea	ecx,[esp+4]
    lea	edx,[esp+16]
    fld	tword[edx]
    fld	tword[ecx]
    mov dl,2
    jmp short atan2
_atan2:
    lea	ecx,[esp+4]
    lea	edx,[esp+12]
    fld	qword[edx]
    fld	qword[ecx]
    mov dl,1
atan2:
    lea	eax,[nm]
    call    clearmath
    ftst
    fstsw ax
    sahf
    fxch
    jnz ok
    ftst
    fstsw ax
    sahf
    jz baddomain
ok:
    fpatan
    jmp wrapmath
baddomain:
    popone
    jmp domainerr
