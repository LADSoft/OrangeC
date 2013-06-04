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
[export _wcsstr]
%endif
[global _wcsstr]
section code CLASS=CODE USE32

_wcsstr:
	push	ebp
	mov	ebp,esp
	push	esi
	push	edi
	mov	esi,[ebp+8]
	mov	edi,[ebp+12]
	test	edi,edi
	jz	exit	
	mov	ax,[edi]
	test	ax,ax
	jz	exit	
	push	edi
	sub	ax,ax
	mov	ecx,-1
	cld
	repne	scasw
	not	ecx
	dec	ecx
	pop	edi
lp:
	mov	ax,[esi]
	or	ax,ax
	je	badexit
	cmp	ax,[edi]
	jne	nocomp
	push	ecx
	push	esi
	push	edi
	rep	cmpsb
	pop	edi
	pop	esi
	pop	ecx
	je	exit
nocomp:
	inc esi
    inc esi
	jmp	lp
badexit:
	sub	esi,esi
exit:
	mov	eax,esi
	pop	edi
	pop	esi
	pop	ebp
	ret
