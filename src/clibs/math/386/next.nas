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
[export _nextafter]
[export _nextafterf]
[export _nextafterl]
[export _nexttoward]
[export _nexttowardf]
[export _nexttowardl]
%endif
[global _nextafter]
[global _nextafterf]
[global _nextafterl]
[global _nexttoward]
[global _nexttowardf]
[global _nexttowardl]
SECTION data CLASS=DATA USE32
nm	db	"nexttoward",0
;1111111
;0010111
;1101000
; 340

; 3ff
;  34
; 3dc

; 3fff
;   40
; 3fbf

really_small dd 1,0

SECTION code CLASS=CODE USE32
_nextafterf:
    lea eax,[nm]
    call clearmath
    fld dword [esp+4]
    fld dword [esp+8]
    jmp join
_nexttowardf:
    lea eax,[nm]
    call clearmath
    fld dword [esp+4]
    fld qword [esp+8]
join:
    fcomp st1
    fstsw ax
    sahf
    je wrapmath
    fld st0
    jc fdown
    fadd dword [really_small]
    fxch st1
    fcomp st1
    fstsw ax
    sahf
    jnz wrapmath
    popone
    add dword [esp + 4], 1
    fld dword [esp + 4]
    jmp wrapmath
fdown:
    fsub dword [really_small]
    fxch st1
    fcomp st1
    fstsw ax
    sahf
    jnz wrapmath
    popone
    sub dword [esp + 4],1
    fld dword [esp + 4]
    jmp wrapmath
_nexttowardl:
_nexttoward:
_nextafterl:
_nextafter:
    lea eax,[nm]
    call clearmath
    mov dl,1
    fld qword [esp+4]
    fld qword [esp+12]
    fcomp st1
    fstsw ax
    sahf
    je wrapmath
    fld st0
    jc down
    fadd qword [really_small]
    fxch st1
    fcomp st1
    fstsw ax
    sahf
    jnz wrapmath
    popone
    add dword [esp + 4], 1
    adc dword [esp + 8], 0
    fld qword [esp + 4]
    jmp wrapmath
down:
    fsub qword [really_small]
    fxch st1
    fcomp st1
    fstsw ax
    sahf
    jnz wrapmath
    popone
    sub dword [esp + 4], 1
    sbb dword [esp + 8], 0
    fld qword [esp + 4]
    jmp wrapmath
