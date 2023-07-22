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
[export ___buffer_overflow_check]
[export ___buffer_overflow_init]
-[export ___uninit_var_check]
%endif
[global ___buffer_overflow_check]
[global ___buffer_overflow_init]
[global ___uninit_var_check]

[extern ___buffer_overflow_abort]
[extern ___uninitialized_variable_abort]
[extern _memset]

STACKCHECK_MAGIC_NUMBER EQU 0cccccccch

SECTION code CLASS=CODE USE32

___buffer_overflow_init;
    push ecx
    push edx
    push eax
    mov eax,[esp + 16]; length
    mov ecx,eax
    neg ecx
    push ecx
    push STACKCHECK_MAGIC_NUMBER
    add eax,ebp
    push eax
    call _memset
    add esp,12
    pop eax
    pop edx
    pop ecx
    ret 4
___buffer_overflow_check:
    push esi
    mov esi,[esp + 8];
boc_lp:
    test dword[esi], 0ffffffffh
    jz boc_ok
    mov ecx,[esi]
    add ecx, ebp
    cmp dword[ecx], STACKCHECK_MAGIC_NUMBER
    jz boc_cont
    push dword [esi+8];
    push dword [esi +4]
    call ___buffer_overflow_abort
    // never returns;
boc_cont:
    add esi, 12
    jmp boc_lp
    
boc_ok:
    pop esi
    ret 4
___uninit_var_check:
    push eax
    mov eax, [esp + 8]
    mov eax,[eax]
    add eax,ebp
    test dword [eax],0ffffffffh
    pop eax
    jne uninit_err
    ret 4
uninit_err:
    mov eax, [esp + 4]
    push dword [eax + 12]
    push dword [eax + 8]
    push dword [eax + 4]
    call ___uninitialized_variable_abort
    // never returns;
   