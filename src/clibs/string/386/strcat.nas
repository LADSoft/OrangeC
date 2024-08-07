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
[export _strcat]
%endif
[global _strcat]
[extern strcat_fin]

SECTION code CLASS=CODE USE32

_strcat:
        push ebp
		mov ebp, esp
		mov	ah,3
		mov	edx,[ebp+8]	;	str1
TestEdx:
		test	dl,ah
		jnz	DoAlign

		;
		; do multiple of 4 bytes at a time
		;
QuadLoop:
		mov	eax,[edx]	;	Load the bytes
		test	al,al	;	Look for null
		jz	CpyDone
        inc edx
		test	ah,ah
		jz	CpyDone
        inc edx
		test eax,0xFF0000
		jz	CpyDone
        inc edx
		test eax,0xFF000000
        jz  CpyDone
        inc edx
        jmp QuadLoop
    
CpyDone:
        mov ecx,edx
		Jmp strcat_fin


DoAlign:
		mov	al,[edx]
		inc	edx
		test	al,al
		jnz	TestEdx
        mov ecx,edx
        dec ecx
		Jmp strcat_fin
