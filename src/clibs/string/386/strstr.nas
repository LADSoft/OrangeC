;  Software License Agreement
;  
;      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
[export _strstr]
%endif
[global _strstr]

section code CLASS=CODE USE32

_strstr:
	push	ebp
	mov		ebp, esp
	push	ebx
	mov		ebx,[ebp+8]
	mov		ecx,ebx
	dec		ebx
lp:
	mov		edx,[ebp+12]
	test		edx,edx
	jz		x1
	mov		al,[edx]
	test		al,al
	jz		x1
lp2:
	inc		ebx
	mov		ah, [ebx]
	or		ah,ah
	jz		none
	cmp		al,	ah
	jnz		lp2
	mov		ecx,ebx

lp3:
	inc		edx
	inc		ebx
	mov		ah,[edx]
	and		ah,ah
	jz		x1
	cmp		ah,[ebx]
	jz		lp3
	mov		ebx,ecx
	jmp		lp
x1:
	mov		eax,ecx
	pop		ebx
	pop		ebp
	ret
none:
	sub		eax,eax
	pop		ebx
	pop     ebp
	ret
