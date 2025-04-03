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
[export _strpbrk]
%endif
[global _strpbrk]
SECTION code CLASS=CODE USE32

_strpbrk:
	push ebp
	mov ebp, esp
    mov	ecx,[ebp + 8]
    dec ecx
lp:
    inc	ecx
    mov	al,[ecx]
    or	al,al
    je	short ex1
    mov	edx,[ebp + 12]
    dec edx
lp1:
    inc	edx
    cmp byte [edx], byte 0
    je	lp
    cmp	al,[edx]
    jne	short lp1
ex2:
    mov eax,ecx
exit:
    pop ebp
    ret
ex1:
    sub	eax,eax
	pop ebp
    ret
