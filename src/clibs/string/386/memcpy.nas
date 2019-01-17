; Software License Agreement
; 
;     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
; 
;     This file is part of the Orange C Compiler package.
; 
;     The Orange C Compiler package is free software: you can redistribute it and/or modify
;     it under the terms of the GNU General Public License as published by
;     the Free Software Foundation, either version 3 of the License, or
;     (at your option) any later version, with the addition of the 
;     Orange C "Target Code" exception.
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
[export _memcpy]
%endif
[global _memcpy]
[global memcpy_x]
SECTION code CLASS=CODE USE32

_memcpy:
    push edi
    push esi
    mov ecx, [esp + 20]	; cnt
    mov eax, [esp + 12]	; dest
    mov esi, [esp + 16]	; src
    
    cmp ecx, 4
    jnb aligned
    
    test ecx, ecx
    jne doSmall
    
return:
    pop esi
    pop edi
    ret
    
    lea esi, [esi + 0]	; Align by 16 bytes (correct when changing code or just replace this with the appropriate align directive)
    
aligned:
    mov edx, [esi]
    lea edi, [eax + 4]
    and edi, -4
    mov [eax], edx
    mov edx, [esi + ecx - 4]
    mov [eax + ecx - 4], edx
    mov edx, eax
    sub edx, edi
    add ecx, edx
    sub esi, edx
    shr ecx, 2
    rep movsd
    
    pop esi
    pop edi
    ret
    
doSmall:
    movzx edx, byte [esi]
    test cl, 2
    mov [eax], dl
    jz return
    
    movzx edx, word [esi + ecx - 2]
    mov [eax + ecx - 2], dx
    jmp return

    nop
    nop
    nop
    nop 
    nop
    nop
    nop
    nop	; 8 nops to align by 16 (correct when changing code or just use appropriate align directive


memcpy_x:	; from MEMMOVE
    dec	edx
lp1:
    inc	edx
    test	dl,3
    jz	lp
    mov	al,[ebx]
    inc	ebx
    mov	[edx],al
    loop lp1
    jecxz	x2
lp:
    cmp	ecx,BYTE 4
    jb	c1
    mov	eax,[ebx]
    add	ebx,BYTE 4
    mov [edx],eax
    sub	ecx,BYTE 4
    add	edx,BYTE 4
    jmp short lp
c1:
    jecxz	x2
    dec	edx
lp2:
    inc	edx
    mov	al,[ebx]
    inc	ebx
    mov [edx],al
    loop	lp2
x2:
    mov eax,[esp+8]
    pop	ebx
	pop ebp
    ret
    
