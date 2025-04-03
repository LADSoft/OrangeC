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
[export _strcpy]
%endif
[global _strcpy]
[global strcat_fin]

SECTION code CLASS=CODE USE32

_strcpy:
		push ebp
		mov ebp, esp
		mov	ecx,[ebp+8]	;	str1
strcat_fin:
		mov	ah,3
		mov	edx,[ebp+12]	;  str2
TestEdx:
		test	dl,ah
		jnz	DoAlign

		;
		; do multiple of 4 bytes at a time
		;
QuadLoop:
		mov	eax,[edx]	;	Load the bytes
		add	edx,4		;	Update pointer
		test	al,al	;	Look for null
		jz	CpyDoneLoLo
		test	ah,ah
		jz	CpyDoneLoHi
		test eax,0xFF0000
		jz	CpyDoneHiLo
		mov	[ecx],eax
		add	ecx,4		;	(or 4 if misaligned)
		test eax,0xFF000000
		jnz	QuadLoop
    
CpyDone:
		mov	eax,[esp+8]
		pop ebp
		ret

CpyDoneLoLo:
		mov	[ecx],al	; this is a zero
		mov	eax,[ebp+8]
		pop ebp
		ret

CpyDoneLoHi:
		xor	dl,dl
		mov	[ecx],al
		mov	[ecx+1],dl
		mov	eax,[ebp+8]
		pop ebp
		ret

CpyDoneHiLo:
		mov	[ecx],al
		xor	dl,dl
		mov	[ecx+1],ah
		mov	eax,[ebp+8]
		mov	[ecx+2],dl
		pop ebp
		ret

DoAlign:
		mov	al,[edx]
		inc	edx
		mov	[ecx],al
		inc	ecx
		test	al,al
		jnz	TestEdx
		mov	eax,[ebp+8]
		pop ebp
		ret
