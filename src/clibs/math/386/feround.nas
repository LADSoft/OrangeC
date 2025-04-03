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
[export _fegetround]
[export _fesetround]
%endif
[global _fegetround]
[global _fesetround]
SECTION data CLASS=DATA USE32

SECTION code CLASS=CODE USE32

_fegetround:
    sub esp,4
    fstcw [esp]
    and word [esp],0f3ffh
    movzx eax,word [esp]
    add esp,4
    shr eax,10
    and eax,3
    ret
_fesetround:
    mov eax,[esp + 4]
    cmp eax,4
    jae srerr
    sub esp,4
    fstcw [esp]
    and word [esp],0f3ffh
    shl eax,10
    or word [esp],ax
    fldcw [esp]
    add esp,4
    sub eax,eax
    ret
srerr:
    mov eax,1
    ret