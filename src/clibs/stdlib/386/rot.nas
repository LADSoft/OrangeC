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
[export __crotl]
[export __crotr]
[export __rotl]
[export __rotr]
[export __lrotl]
[export __lrotr]
[export __llrotl]
[export __llrotr]
%endif
[global __crotl]
[global __crotr]
[global __rotl]
[global __rotr]
[global __llrotl]
[global __llrotr]

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


__llrotl:
    movzx  ecx, BYTE[esp + 12]
    mov    eax, [esp + 4]
    mov    edx, [esp + 8]
    jecxz   llrotlx
llrotlloop:
    clc
    rcl    edx, 1
    rcl    eax, 1
    jnc  llrotlcont
    bts    edx, 1
llrotlcont:
    loop llrotlloop
llrotlx:
    ret
__llrotr:
    movzx  ecx, BYTE[esp + 12]
    mov    eax, [esp + 4]
    mov    edx, [esp + 8]
    jecxz   llrotrx
llrotrloop:
    clc	
    rcr    edx, 1
    rcr    eax, 1
    jnc  llrotrcont
    bts    edx, 31
llrotrcont:
    loop llrotrloop
llrotrx:
    ret
    ret
