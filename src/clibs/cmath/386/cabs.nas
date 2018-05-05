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

%ifdef __BUILDING_LSCRTL_DLL
[export _cabs]
[export _cabsf]
[export _cabsl]
%endif

[global _cabs]
[global _cabsf]
[global _cabsl]
[extern _hypot]
[extern _hypotf]
[extern _hypotl]

SECTION code CLASS=CODE USE32

_cabsf:
    jmp _hypotf
_cabsl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    fld tword[ecx+10]
    sub esp,24
    fstp tword [esp+12]
    fstp tword [esp]
    call    _hypotl
    add esp,24
    ret
_cabs:
    jmp _hypot
