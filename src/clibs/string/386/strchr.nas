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
[export _strchr]
%endif
[global _strchr]
SECTION code CLASS=CODE USE32

_strchr:
    push ebp
	mov ebp,esp
    mov  eax,[ebp + 8]
    dec eax
    mov edx,[ ebp + 12]
lp:
    inc eax
    mov dh,[eax]
    or	dh,dh
    jz	x1
    cmp	dh,dl
    jnz	lp
	pop ebp
    ret
x1:
    cmp	dh,dl
        jz      xx
    sub	eax,eax
xx:
	pop ebp
    ret
