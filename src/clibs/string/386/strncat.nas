; Software License Agreement
; 
;     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version.
; 
;     The Orange C Compiler package is distributed in the hope that it will be useful,
;     but WITHOUT ANY WARRANTY; without even the implied warranty of
;     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;     GNU General Public License for more details.
; 
;     You should have received a copy of the GNU General Public License
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     As a special exception, if other files instantiate templates or
;     use macros or inline functions from this file, or you compile
;     this file and link it with other works to produce a work based
;     on this file, this file does not by itself cause the resulting
;     work to be covered by the GNU General Public License. However
;     the source code for this file must still be made available in
;     accordance with section (3) of the GNU General Public License.
;     
;     This exception does not invalidate any other reasons why a work
;     based on this file might be covered by the GNU General Public
;     License.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%ifdef __BUILDING_LSCRTL_DLL
[export _strncat]
%endif
[global _strncat]
[extern _strncpy]
SECTION code CLASS=CODE USE32

_strncat:
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
        jmp short fin


DoAlign:
		mov	al,[edx]
		inc	edx
		test	al,al
		jnz	TestEdx
        mov ecx,edx
        dec ecx
fin:
        push dword [ebp+16]
        push dword [ebp + 12]
        push ecx
	call _strncpy
        add esp, 12
        cmp byte [ecx-1], 0
        jz xit
	mov byte [ecx], 0
xit:
    pop ebp
	ret
exit:
        mov eax,[ebp+8]
		pop ebp
        ret