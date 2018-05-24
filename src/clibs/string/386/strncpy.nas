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
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%ifdef __BUILDING_LSCRTL_DLL
[export _strncpy]
%endif
[global _strncpy]
SECTION code CLASS=CODE USE32

_strncpy:
		mov	ecx,[esp+4]	;	str1
		mov	ah,3
		mov	edx,[esp+8]	;  str2
        cmp dword [esp+12],0
        jz  CpyDone
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
        cmp dword [esp+12],1
        jz  CpyDoneLoLo
		test	ah,ah
		jz	CpyDoneLoHi
        cmp dword [esp+12],2
        jz  CpyDoneLoHi
		test eax,0xFF0000
		jz	CpyDoneHiLo
        cmp dword [esp+12],3
        jz  CpyDoneHiLo
		mov	[ecx],eax
		add	ecx,4		;	(or 4 if misaligned)
        sub dword [esp+12], 4
        jz  CpyDone
		test eax,0xFF000000
		jnz	QuadLoop
    
CpyDone:
		mov	eax,[esp+4]
		ret

CpyDoneLoLo:
		mov	[ecx],al	; this is a zero
                inc     ecx;
		mov	eax,[esp+4]
		ret

CpyDoneLoHi:
		mov	[ecx],ax
                inc     ecx
                inc     ecx
		mov	eax,[esp+4]
		ret

CpyDoneHiLo:
        mov [ecx],ax
        shr eax,16
        mov [ecx+2],al
        add ecx,3
		mov	eax,[esp+4]
		ret

DoAlign:
		mov	al,[edx]
		inc	edx
		mov	[ecx],al
		inc	ecx
        dec dword [esp+12]
        jz  CpyDone
		test	al,al
		jnz	TestEdx
		mov	eax,[esp+4]
		ret
