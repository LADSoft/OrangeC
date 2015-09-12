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
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIAahE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
%ifdef __BUILDING_LSCRTL_DLL
[export _strcmp]
%endif
[global _strcmp]
[global strcmp_x]
SECTION code CLASS=CODE USE32
_strcmp:
    mov		ecx,[esp+4]
    mov		edx,[esp+8]
strcmp_x:		; from strstr

	;
	; do multiple of 4 bytes at a time
	;
QuadLoop:
	mov	al,[ecx]	;	Load the bytes (different base
	mov	ah,[edx]	;	registers on purpose)
	

	cmp	al,ah		;	Compare bytes
	jnz	Unequal		

	test	al,al	;	Check end of string
	jz	Equal		

	mov	al,[ecx+1]	;	Repeat 3 more times
	mov	ah,[edx+1]	

	cmp	al,ah
	jnz	Unequal		

	test	al,al
	jz	Equal		

	mov	al,[ecx+2]
	mov	ah,[edx+2]	

	cmp	al,ah
	jnz	Unequal		

	test	al,al
	jz	Equal		

	mov	al,[ecx+3]
	mov	ah,[edx+3]	

	cmp	al,ah
	jnz	Unequal		

    
	add	ecx,4		;	Update pointers
	add	edx,4		
	
	test	al,al
	jnz	QuadLoop	
Equal:
	xor	eax,eax
	jmp End			

Unequal:
	sbb	eax,eax
    or  eax,1
    
End:
    ret
