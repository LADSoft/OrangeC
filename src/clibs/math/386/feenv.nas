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
[export _fegetenv]
[export _fesetenv]
[export _feupdateenv]
[export _feholdexcept]
%endif
[global _fegetenv]
[global _fesetenv]
[global _feupdateenv]
[global _feholdexcept]

[extern feflagshold]

SECTION data CLASS=DATA USE32


section code CLASS=CODE USE32

_fegetenv:
    mov ecx,[esp + 4]
    fnstenv [ecx]
    fldenv [ecx]
    sub eax,eax
    ret
_fesetenv:
    mov ecx,[esp+4]
    mov ax,[ecx+4]
    mov [feflagshold],ax
    and ax,0ff00h
    mov [ecx + 4],ax
    fldenv [ecx]
    sub eax,eax
    ret
_feupdateenv:
    fstsw ax
    and ax,0ffh
    mov ecx,[esp+4]
    or [ecx+4],ax
    fldenv [ecx]
    sub eax,eax
    ret
_feholdexcept:
    mov ecx,[esp+4]
    fnstenv [ecx]
    fldenv [ecx]
    fnclex
    sub esp,4
    fnstcw [esp]
    or word [esp],3fh
    fldcw [esp]
    add esp,4
    sub eax,eax
    ret
    