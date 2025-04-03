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
[export _wcpcpy]
%endif
[global _wcpcpy]
SECTION code CLASS=CODE USE32

_wcpcpy:
    push	ebp
    mov	ebp,esp
    push	esi
    push	edi
    mov	esi,[ebp+12]
    mov	edi,esi
    sub	ax,ax
    mov	ecx,-1
    cld
    repne	scasw
    not	ecx
    mov	edi,[ebp+8]
    mov	eax,edi
    rep	movsw
    lea eax,[edi-2]
    pop	edi
    pop	esi
    pop	ebp
    ret
