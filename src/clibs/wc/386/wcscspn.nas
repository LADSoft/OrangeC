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
[export _wcscspn]
%endif
[global _wcscspn]
SECTION code CLASS=CODE USE32

_wcscspn:
    push	ebp
    mov	ebp,esp
    push	esi
    push	edi
    mov	esi,[ebp+8]
    mov	edx,[ebp+12]
    sub	eax,eax
lp:
    inc	eax
    test	word [esi],0ffffh
    je	exit
    mov	edi,edx
    mov	cx,[esi]
    inc	esi
    inc esi
lp1:
    test	word [edi],0ffffh
    je	lp
    cmp	cx,[edi]
    je	exit
    inc	edi
    inc edi
    jmp	lp1
exit:
    dec	eax
    pop	edi
    pop	esi
    pop	ebp
    ret
