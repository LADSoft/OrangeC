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
[export ___canary_check]
[export ___canary_init]
%endif
[global ___canary_check]
[global ___canary_init]

[extern ___canary_abort]
[extern ___canary]

SECTION code CLASS=CODE USE32

___canary_init;
    push ecx
    mov ecx, [___canary]
    mov [ebp - 4], ecx
    pop ecx
    ret
___canary_check:
    mov ecx,[___canary];
    cmp ecx,[ebp-4]
    jne ___canary_abort // this relies on no pushes this function
    ret 4 ; clear out the name of the function
