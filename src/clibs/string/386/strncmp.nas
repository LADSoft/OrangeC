; Software License Agreement
; 
;     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%ifdef __BUILDING_LSCRTL_DLL
[export _strncmp]
%endif
[global _strncmp]
SECTION code CLASS=CODE USE32

_strncmp:
    push 	ebp
	mov		ebp, esp
    push	ebx
    mov		ecx,[ebp+16]
    sub		eax,eax
    jecxz	x1
    mov		ebx,[ebp+12]
    dec		ebx
    mov		edx,[ebp+8]
    dec		edx
lp:
    inc		edx
    inc		ebx
    mov		al,[edx]
    or		al,al
    jz		x2
    sub		al,[ebx]
    jc          x3
    loopz	lp
    
x1:
    movsx	eax,al
    pop	ebx
	pop ebp
    ret
x3:
    mov         eax,-1
    pop	ebx
	pop ebp
    ret

x2:
    sub		al,[ebx]
    movsx	eax,al
    pop	ebx
	pop ebp
    ret
