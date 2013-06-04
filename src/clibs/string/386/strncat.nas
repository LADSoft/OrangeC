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
[export _strncat]
%endif
[global _strncat]
section code CLASS=CODE USE32

_strncat:
	push	ebx
	mov		ecx,[esp+16]
	jecxz	x2
	mov	ebx,[esp+8]
	dec	ebx
lp1:
	inc	ebx
	test bl,3
	jnz x1
lp2:
	mov	ecx,[ebx]
	add	ebx,4
	mov edx,ecx
	sub	edx,001010101h
	not ecx
	and edx,080808080h
	and edx,ecx
	jz lp2
	sub ebx,4
x1:
	cmp	byte [ebx],0
	jne	lp1

	dec		ebx
	mov		edx,[esp+12]
	mov		ecx,[esp+16]
lp:
	inc		ebx
	mov		al,[edx]
	inc		edx
	mov		[ebx],al
	or		al,al
	loopnz	lp
	jz		x2
	inc		ebx
	mov		byte [ebx],0
x2:	
	mov	eax,[esp+8]
	pop	ebx
	ret
	
