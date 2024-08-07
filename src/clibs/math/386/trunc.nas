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
[export _trunc]
[export _truncf]
[export _truncl]
%endif
[global _trunc]
[global _truncf]
[global _truncl]
SECTION data CLASS=DATA USE32
nm	db	"trunc",0

SECTION code CLASS=CODE USE32
_truncf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short trunc
_truncl:
_trunc:
    fld	qword[esp+4]
    mov dl,1
trunc:    
    lea eax,[nm]
    call clearmath
    sub esp,4
    fnstcw	[esp]
    mov	ax,[esp]
    and	ah,0f3h
    or	ah,0ch
    mov	[esp+2],ax
    fldcw	[esp+2]
    frndint
    call checkinvalid
    fldcw	[esp]
    add esp,4
    ret
