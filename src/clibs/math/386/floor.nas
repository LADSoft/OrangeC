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

%include "matherr.inc"

%ifdef __BUILDING_LSCRTL_DLL
[export _floor]
[export _floorf]
[export _floorl]
%endif
[global _floor]
[global _floorf]
[global _floorl]
SECTION data CLASS=DATA USE32
nm	db	"floor",0

SECTION code CLASS=CODE USE32
_floorf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short floor
_floorl:
    fld	tword[esp+4]
    mov dl,2
    jmp short floor
_floor:
    fld	qword[esp+4]
    mov dl,1
floor:    
    lea eax,[nm]
    call clearmath
    sub esp,4
    fnstcw	[esp]
    mov	ax,[esp]
    and	ah,0f3h
    or	ah,4
    mov	[esp+2],ax
    fldcw	[esp+2]
    frndint
    call    checkinvalid
    fldcw	[esp]
    add esp,4
    ret
