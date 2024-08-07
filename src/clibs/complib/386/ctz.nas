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
[export ___builtin_ctz]
[export ___builtin_ctzl]
[export ___builtin_ctzll]
%endif
[global ___builtin_ctz]
[global ___builtin_ctzl]
[global ___builtin_ctzll]
SECTION code CLASS=CODE USE32
___builtin_ctz:
___builtin_ctzl:
    mov ecx,[esp+4]
    sub eax,eax
join:
    jecxz full
lp:
    shr ecx,1
    jc  done
    inc eax
    jmp lp
full:
    add eax,32
done:
    ret

___builtin_ctzll:
    test    dword [esp+4],0ffffffffh
    jnz     ___builtin_ctz
    mov     ecx,[esp+8]
    mov     eax,32
    jmp     join