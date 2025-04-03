;  Software License Agreement
;  
;      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
;  
;      This file is part of the Orange C Compiler package.
;  
;      The Orange C Compiler package is free software: you can redistribute it and/or modify
;      it under the terms of the GNU General Public License as published by
;      the Free Software Foundation, either version 3 of the License, or
;      (at your option) any later version.
;  
;      The Orange C Compiler package is distributed in the hope that it will be useful,
;      but WITHOUT ANY WARRANTY; without even the implied warranty of
;      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;      GNU General Public License for more details.
;  
;      You should have received a copy of the GNU General Public License
;      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
;  
;      contact information:
;          email: TouchStone222@runbox.com <David Lindauer>
;  

%ifdef __BUILDING_LSCRTL_DLL
[export _strcmp]
%endif
[global _strcmp]
SECTION code CLASS=CODE USE32
_strcmp:
    push	ebp
	mov		ebp, esp
    mov		ecx,[ebp+8]
    mov		edx,[ebp+12]

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
	pop	ebp
    ret
