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
[export _setjmp]
[export __setjmp]
[export _longjmp]
%endif
[global _setjmp]
[global __setjmp]
[global _longjmp]

SECTION code CLASS=CODE USE32
__setjmp:
_setjmp:
    push ebp
	mov ebp, esp
    mov	eax,[ebp+8]
    mov	[eax],ecx
    mov	[eax+4],edx
    mov	[eax+8],ebx
    mov	[eax+12],esp
    add	dword [eax+12],8
	mov ecx,[ebp] ; ebp
    mov	[eax+16],ecx
    mov	[eax+20],esi
    mov	[eax+24],edi
    mov	ecx,[ebp+4] ; eip
    mov	[eax+32],ecx
    mov	ecx,eax
    sub	eax,eax
    lahf
    mov	[ecx+28],eax
    sub	eax,eax
	pop ebp
    ret
_longjmp:
    push ebp
	mov ebp, esp
    mov	eax,[ebp+8]
    mov	ecx,[ebp+12]
	; reloading esp, the push of ebp above will be discarded
    mov	esp,[eax+12]
    push	ecx
    mov	ecx,[eax+32]
    xchg	[esp],ecx
    push	ecx
    mov	ecx,[eax]
    mov	edx,[eax+4]
    mov	ebx,[eax+8]
    mov	ebp,[eax+16]
    mov	esi,[eax+20]
    mov	edi,[eax+24]
    mov	eax,[eax+28]
    sahf
    pop	eax
    or eax,eax
    jnz ok
    inc al ; can never return 0...
ok:
    ret