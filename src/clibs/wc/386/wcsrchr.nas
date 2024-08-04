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
[export _wcsrchr]
%endif
[global _wcsrchr]
SECTION code CLASS=CODE USE32

_wcsrchr:
    push	ebp
    mov	ebp,esp
    push	edi
    mov	edi,[ebp+8]
    mov	ecx,-1
    sub	ax,ax
    cld
    repne	scasw
    not	ecx
    dec	edi
    dec edi
    mov	ax,[ebp+12]
    std
    repne	scasw
    cld
    je	okexit
    sub	eax,eax
    jmp	done
okexit:
    add ecx,ecx
    add	ecx,[ebp+8]
    mov	eax,ecx
done:
    pop	edi
    pop	ebp
    ret
