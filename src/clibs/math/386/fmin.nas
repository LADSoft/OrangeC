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
[export _fmin]
[export _fminf]
[export _fminl]
%endif
[global _fmin]
[global _fminf]
[global _fminl]
SECTION data CLASS=DATA USE32
nm	db	"fmin",0

SECTION code CLASS=CODE USE32
_fminf:
    lea	ecx,[esp+4]
    fld	dword [ecx]
    lea	edx,[esp+8]
    fld	dword [edx]
    sub dl,dl
    jmp short ffmin
_fminl:
_fmin:
    lea	ecx,[esp+4]
    fld	qword [ecx]
    lea	edx,[esp+12]
    fld	qword [edx]
    mov dl,1
ffmin:
    lea eax,[nm]
    call clearmath
    fcom st1
    fstsw   ax
    sahf
    ja  short noxchg
    fxch
noxchg:
    popone
    jmp wrapmath
