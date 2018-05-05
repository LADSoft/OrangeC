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
;     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
; 
;     contact information:
;         email: TouchStone222@runbox.com <David Lindauer>
; 

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export __ftol]
%endif
[global __ftol]
SECTION data CLASS=DATA USE32
nm  db "_ftol",0

SECTION code CLASS=CODE USE32
__ftol:
    lea eax,[nm]
    call clearmath
    sub	esp,12
    fnstcw	[esp+8]
    mov	ax,[esp+8]
    or	ah,0ch
    mov	[esp+10],ax
    fldcw	[esp+10]
    fistp	qword [esp]
    call checkinvalid
    jnc ok
    popone
    mov dword [esp],07fffffffh
ok:
    fldcw	[esp+8]
    mov	eax,[esp]
    add esp,12
    ret
