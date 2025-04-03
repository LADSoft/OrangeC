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

section data USE32
%include "..\..\copyrght.asm"
unalignedstack dd 0
section code USE32
extern ___lscrtl_startup
..start:
    push ebp
    mov ebp,esp
    mov	edx, 2
    mov [unalignedstack], esp
    and esp,-16
    sub esp, 12
    push dword [ebp + 16]
    push dword [ebp + 12]
    push dword [ebp + 8]
    call ___lscrtl_startup
    mov	esp,[unalignedstack]
    pop ebp
    ret 12
global WinMain
WinMain:
    ret	16
global _main
_main:
    ret