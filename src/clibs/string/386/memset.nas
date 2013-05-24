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
[export _memset]
%endif
[global _memset]

SECTION code CLASS=CODE USE32
_memset:
    mov	ecx,[esp+12]
    jecxz	x2
    mov	eax,[esp+8]
    mov     ah,al
    movzx   edx,ax
    shl     eax,16
    add     eax,edx
join:
    cld
    mov	edx,[esp + 4]
    push	edi
    mov	edi,edx
    xchg ecx,edx
    and	ecx,3		; all this up to notwoa is to align on a dword boundary
    jecxz	aligned
    neg	ecx
    add	ecx,4
allp:
    stosb
    dec	edx
    jz	x1
    loop	allp
aligned:
    mov	ecx,edx
    push 	ecx
    shr	ecx,2
    rep	stosd
    pop	ecx
    shr	cl,1
    jnc	noone
    stosb
noone:
    shr	cl,1
    jnc	notwo
    stosw
notwo:
x1:
    pop	edi
x2:
    mov eax,[esp+4]
    ret
    