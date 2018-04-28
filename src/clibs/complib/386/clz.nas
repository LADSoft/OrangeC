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
;     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>

%ifdef __BUILDING_LSCRTL_DLL
[export ___builtin_clz]
[export ___builtin_clzl]
[export ___builtin_clzll]
%endif
[global ___builtin_clz]
[global ___builtin_clzl]
[global ___builtin_clzll]
SECTION code CLASS=CODE USE32
___builtin_clz:
___builtin_clzl:
    mov ecx,[esp+4]
    sub eax,eax
join:
    jecxz full
lp:
    shl ecx,1
    jc  done
    inc eax
    jmp lp
full:
    add eax,32
done:
    ret

___builtin_clzll:
    sub     eax,eax
    test    dword [esp+8],0ffffffffh
    mov     ecx,[esp+8]
    jnz     join
    mov     eax,32
    mov     ecx,[esp+4]
    jmp     join
