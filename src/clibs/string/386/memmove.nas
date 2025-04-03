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
[export _memmove]
%endif
[global _memmove]

[extern memcpy_x]

section code CLASS=CODE USE32
_memmove:
	push ebp
	mov ebp, esp
	push	ebx
	mov	ecx,[ebp+16]
	jecxz x1
	mov	ebx,[ebp+12]
	mov	edx,[ebp+8]
join:
	cmp	edx,ebx	
	jbe	memcpy_x
	add	ebx,ecx
	add	edx,ecx
lp2:
	dec	ebx
	dec	edx
	mov	al,[ebx]
	mov	[edx],al
	loop	lp2
x1:
	mov	eax,[esp+8]
	pop	ebx
	pop ebp
	ret
