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

section data USE32
%include "..\..\copyrght.asm"
align 4
global ___unaligned_stacktop
___unaligned_stacktop dd 0
section code USE32
extern ___startup
..start:
    push ebp
    mov ebp,esp
    mov [___unaligned_stacktop], esp
    mov	edx, 2
    and esp,-16
    sub esp, 12
    push dword [ebp + 16]
    push dword [ebp + 12]
    push dword [ebp + 8]
    call ___startup
    mov	esp,[___unaligned_stacktop]
    pop ebp
    ret 12