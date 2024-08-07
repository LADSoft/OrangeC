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

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _nearbyint]
[export _nearbyintf]
[export _nearbyintl]
%endif
[global _nearbyint]
[global _nearbyintf]
[global _nearbyintl]
SECTION data CLASS=DATA USE32
nm	db	"nearbyint",0

SECTION code CLASS=CODE USE32
_nearbyintf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short nearbyint
_nearbyintl:
_nearbyint:
    fld	qword[esp+4]
    mov dl,1
nearbyint:    
    lea eax,[nm]
    call clearmath
    frndint
    call checkinvalid
    ret
