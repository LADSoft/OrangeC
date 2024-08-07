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
[export _feclearexcept]
[export _fegetexceptflag]
[export _feraiseexcept]
[export _fesetexceptflag]
[export _fetestexcept]
%endif
[global _feclearexcept]
[global _fegetexceptflag]
[global _feraiseexcept]
[global _fesetexceptflag]
[global _fetestexcept]
[global feflagshold]
SECTION data CLASS=DATA USE32

feflagshold dw    0
section code CLASS=CODE USE32

_feclearexcept:
    mov ax,[esp+4]
    not ax
    or ax,0ff00h
    sub esp,28
    fnstenv [esp]
    and word [esp+4],ax
    fldenv [esp]
    add esp,28
    and [feflagshold],ax
    sub eax,eax
    ret
_fegetexceptflag:
    fnstsw ax
    and ax, 0ffh
    and ax, word [esp+ 8]
    or  ax,[feflagshold]
    mov ecx, [esp+4]
    mov word [ecx],ax
    sub eax,eax
    ret
_fesetexceptflag:
    mov ax,[esp+8]
    mov ecx,[esp+4]
    and ax,[ecx]
    and ax,0ffh
    mov [feflagshold],ax
    sub eax,eax
    ret
_feraiseexcept:
    mov eax,[esp+4]
    and ax,0ffh
    sub esp,28
    fnstenv [esp]
    or word [esp+4],ax
    fldenv [esp]
    fwait
    add esp,28
    sub eax,eax
    ret
_fetestexcept:
    fstsw ax
    and ax,[esp+4]
    and eax,0ffh
    or ax,[feflagshold]
    ret
