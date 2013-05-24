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
[export _llround]
[export _llroundf]
[export _llroundl]
%endif
[global _llround]
[global _llroundf]
[global _llroundl]
[extern round]
SECTION data CLASS=DATA USE32
nm	db	"llround",0

SECTION code CLASS=CODE USE32
_llroundf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short llround
_llroundl:
    fld	tword[esp+4]
    mov dl,2
    jmp short llround
_llround:
    fld	qword[esp+4]
    mov dl,1
llround:    
    lea eax,[nm]
    call clearmath
    sub esp,12
    call round
    fistp qword [esp]
    call checkinvalid
    jc maxint
    mov edx,[esp+4]
    mov eax,[esp]
    add esp,12
    ret
maxint:
    mov eax,0ffffffffh
    mov edx,07fffffffh
    add esp,12
    ret