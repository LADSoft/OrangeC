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
[export _llround]
[export _llroundf]
[export _llroundl]
%endif
[global _llround]
[global _llroundf]
[global _llroundl]
[extern round]
SECTION data CLASS=DATA USE32
nm	db	"llround",0

SECTION code CLASS=CODE USE32
_llroundf:
    fld	dword[esp+4]
    sub dl,dl
    jmp short llround
_llroundl:
    fld	tword[esp+4]
    mov dl,2
    jmp short llround
_llround:
    fld	qword[esp+4]
    mov dl,1
llround:    
    lea eax,[nm]
    call clearmath
    sub esp,12
    call round
    fistp qword [esp]
    call checkinvalid
    jc maxint
    mov edx,[esp+4]
    mov eax,[esp]
    add esp,12
    ret
maxint:
    mov eax,0ffffffffh
    mov edx,07fffffffh
    add esp,12
    ret