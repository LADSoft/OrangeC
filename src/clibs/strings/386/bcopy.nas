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

%ifdef __BUILDING_LSCRTL_DLL
[export _bcopy]
%endif
[global _bcopy]
[extern _memmove]
_bcopy:
; void bcopy(const void* src, void* dest, size_t n) { memmove(dest, src, n); } 
;       We want to order it so that we grab the 2nd param, then the first param, then the last param
;       Remember, order is from RTL
;       3 pushes == 4 bytes on stack
;       Current location of src is esp+04
;       Current location of dest is esp+08
;       Current location of n is esp+12
;       Grab n and push it onto the stack
        push    dword [esp+0ch+00h]
;       Grab src and put it onto the stack, remember, we just pushed to the stack, so add 4
        push    dword [esp+04h+04h]
;       Grab dest and push it to the stack 
        push    dword [esp+08h+08h]
        call    _memmove ; memmove
        add     esp,byte 0ch
        ret