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
[export __bswap]
[export __bswap16]
[export __bswap64]
%endif
[global __bswap]
[global __bswap16]
[global __bswap64]

SECTION code CLASS=CODE USE32
__bswap:
    mov eax, [esp + 4]
    mov ecx, eax
    and eax, 0xff00ff00
    and ecx, 0x00ff00ff
    rol eax, 8
    ror ecx, 8
    or eax, ecx
    ret
__bswap16:
    movzx eax, WORD [esp + 4]
    xchg al, ah
    ret
__bswap64:
    push ebx
    mov ecx, 4
    lea eax, [esp + 8]
    lea edx, [esp + 8 + 7]
bsl:
    mov bl,[eax]
    mov bh,[edx]
    mov [eax],bh
    mov [edx], bl
    inc eax
    dec edx
    loop bsl
    mov eax,[esp + 8]
    mov edx,[esp + 12]
    pop ebx
    ret