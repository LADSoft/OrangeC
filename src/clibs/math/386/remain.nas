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
[export _remainder]
[export _remainderf]
[export _remainderl]
%endif
[global _remainder]
[global _remainderf]
[global _remainderl]
SECTION data CLASS=DATA USE32
nm	db	"remainder",0

SECTION code CLASS=CODE USE32
_remainderf:
    lea	ecx,[esp+4]
    lea	edx,[esp+8]
    fld	dword[ecx]
    fld	dword[edx]
    sub dl,dl
    jmp short remainder
_remainderl:
_remainder:
    lea	ecx,[esp+4]
    lea	edx,[esp+12]
    fld	qword[ecx]
    fld	qword[edx]
    mov dl,1
remainder:
    lea eax,[nm]
    call clearmath
    ftst
    fnstsw ax
    sahf
    mov cl,ah
    jz moderr
    fxch
reprem:
    fprem1
    fstsw ax
    sahf
    jp reprem
    fxch
    popone
    jmp wrapmath
moderr:
    popone
    jmp domainerr