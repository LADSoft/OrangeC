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

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _modf]
[export _modff]
[export _modfl]
%endif
[global _modf]
[global _modff]
[global _modfl]

SECTION data CLASS=DATA USE32
nm	db	"modf",0

SECTION code CLASS=CODE USE32
_modff:
    push ebp
	mov ebp,esp
    fld	dword[ebp+8]
    mov	ecx,[ebp+12]
    sub dl,dl
    jmp short modf
_modfl:
_modf:
    push ebp
	mov ebp,esp
    fld	qword[ebp+8]
    mov	ecx,[ebp+16]
    mov dl,1
modf:
    lea eax,[nm]
    call clearmath
    sub esp,4
    fnstcw	[esp]
    mov	ax,[esp]
    and	ah,0f3h
    or	ah,0ch
    mov	[esp+2],ax
    fldcw	[esp+2]
    fld	st0
    frndint
    push ecx
    call wrapmath
    pop ecx
    cmp dl,1
    jz  double
    js  float
    fld st0
    fstp   tword[ecx]
    jmp short join
float:
    fst	dword[ecx]
    jmp short join
double:
    fst   qword[ecx]
join:

    fsubp	st1

    fldcw	[esp]
    add esp,4
    call wrapmath
	pop ebp
	ret
