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
[export __crotl]
[export __crotr]
[export __rotl]
[export __rotr]
[export __lrotl]
[export __lrotr]
%endif
[global __crotl]
[global __crotr]
[global __rotl]
[global __rotr]
[global __lrotl]
[global __lrotr]

SECTION code CLASS=CODE USE32
__rotl:
__lrotl:
    mov 	eax,[esp + 4]
    mov		ecx,[esp + 8]
    rol		eax,cl
    ret

__rotr:
__lrotr:
    mov 	eax,[esp + 4]
    mov		ecx,[esp + 8]
    ror		eax,cl
    ret
__crotl:
    movzx 	eax, BYTE [esp + 4]
    mov		ecx,[esp + 8]
    rol		al,cl
    ret
__crotr:
    movzx 	eax, BYTE [esp + 4]
    mov		ecx,[esp + 8]
    ror		al,cl
    ret
