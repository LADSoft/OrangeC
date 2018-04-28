; Software License Agreement
; 
;     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version, with the addition of the 
;     Orange C "Target Code" exception.
; 
;     The Orange C Compiler package is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>

%ifdef __BUILDING_LSCRTL_DLL
[export _strncat]
%endif
[global _strncat]
[extern strncat_fin]

SECTION code CLASS=CODE USE32

_strncat:
		mov	ah,3
		mov	edx,[esp+4]	;	str1
        mov ecx,[esp+12]
        jecxz exit
TestEdx:
		test	dl,ah
		jnz	DoAlign

		;
		; do multiple of 4 bytes at a time
		;
QuadLoop:
        cmp ecx,4
        jc  exit
		mov	eax,[edx]	;	Load the bytes
		test	al,al	;	Look for null
		jz	CpyDone
        dec ecx
        jecxz exit
        inc edx
		test	ah,ah
		jz	CpyDone
        dec ecx
        jecxz exit
        inc edx
		test eax,0xFF0000
		jz	CpyDone
        dec ecx
        jecxz exit
        inc edx
		test eax,0xFF000000
        jz  CpyDone
        dec ecx
        jecxz exit
        inc edx
        jmp QuadLoop
    
CpyDone:
        mov [esp+12],ecx
        mov ecx,edx
		Jmp strncat_fin


DoAlign:
        dec ecx
        jecxz   exit
		mov	al,[edx]
		inc	edx
		test	al,al
		jnz	TestEdx
        inc ecx
        mov [esp+12],ecx
        mov ecx,edx
        dec ecx
		Jmp strncat_fin
exit:
        mov eax,[esp+4]
        ret