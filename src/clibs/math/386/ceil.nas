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
[export _ceil]
[export _ceilf]
[export _ceill]
%endif
[global _ceil]
[global _ceilf]
[global _ceill]
SECTION data CLASS=DATA USE32
nm	db	"ceil",0

SECTION code CLASS=CODE USE32
_ceilf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short ceil
_ceill:
    fld	tword[esp+4]
    mov dl,2
    jmp short ceil
_ceil:
    fld	qword[esp+4]
    mov dl,1
ceil:    
    lea eax,[nm]
    call clearmath
    sub esp,4
    fnstcw	[esp]
    mov	ax,[esp]
    and	ah,0f3h
    or	ah,8
    mov	[esp+2],ax
    fldcw	[esp+2]
    frndint
    call checkinvalid
    fldcw	[esp]
    add esp,4
    ret
