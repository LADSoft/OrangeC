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

%ifdef __BUILDING_LSCRTL_DLL
[export _setjmp]
[export _longjmp]
%endif
[global _setjmp]
[global _longjmp]

SECTION code CLASS=CODE USE32
_setjmp:
    mov	eax,[esp+4]
    mov	[eax],ecx
    mov	[eax+4],edx
    mov	[eax+8],ebx
    mov	[eax+12],esp
    add	dword [eax+12],4
    mov	[eax+16],ebp
    mov	[eax+20],esi
    mov	[eax+24],edi
    mov	ecx,[esp]
    mov	[eax+32],ecx
    mov	ecx,eax
    sub	eax,eax
    lahf
    mov	[ecx+28],eax
    sub	eax,eax
    ret
_longjmp:
    mov	eax,[esp+4]
    mov	ecx,[esp+8]
    mov	esp,[eax+12]
    push	ecx
    mov	ecx,[eax+32]
    xchg	[esp],ecx
    push	ecx
    mov	ecx,[eax]
    mov	edx,[eax+4]
    mov	ebx,[eax+8]
    mov	ebp,[eax+16]
    mov	esi,[eax+20]
    mov	edi,[eax+24]
    mov	eax,[eax+28]
    sahf
    pop	eax
    or eax,eax
    jnz ok
    inc al ; can never return 0...
ok:
    ret