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

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _lrint]
[export _lrintf]
[export _lrintl]
%endif
[global _lrint]
[global _lrintf]
[global _lrintl]
SECTION data CLASS=DATA USE32
nm	db	"lrint",0

SECTION code CLASS=CODE USE32
_lrintf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short lrint
_lrintl:
_lrint:
    fld	qword[esp+4]
    mov dl,1
lrint:    
    lea eax,[nm]
    call clearmath
    frndint
    sub esp,4
    call checkinvalid
    jc maxint
    fistp dword [esp]
    call checkinvalid
    jc maxint
    mov eax,[esp]
    add esp,4
    ret
maxint:
    popone
    mov eax,07fffffffh
    add esp,4
    ret