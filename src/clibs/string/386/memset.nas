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
[export _memset]
[export _memset_explicit]
%endif
[global _memset]
[global _memset_explicit]

SECTION code CLASS=CODE USE32
_memset:
_memset_explicit:
    push ebp
	mov ebp, esp
    mov	ecx,[ebp+16]
    jecxz	x2
    mov	eax,[ebp+12]
    mov     ah,al
    movzx   edx,ax
    shl     eax,16
    add     eax,edx
join:
    cld
    mov	edx,[ebp + 8]
    push	edi
    mov	edi,edx
    xchg ecx,edx
    and	ecx,3		; all this up to notwoa is to align on a dword boundary
    jecxz	aligned
    neg	ecx
    add	ecx,4
allp:
    stosb
    dec	edx
    jz	x1
    loop	allp
aligned:
    mov	ecx,edx
    push 	ecx
    shr	ecx,2
    rep	stosd
    pop	ecx
    shr	cl,1
    jnc	noone
    stosb
noone:
    shr	cl,1
    jnc	notwo
    stosw
notwo:
x1:
    pop	edi
x2:
    mov eax,[ebp+8]
	pop ebp
    ret
    