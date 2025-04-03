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
[export _fdim]
[export _fdimf]
[export _fdiml]
%endif
[global _fdim]
[global _fdimf]
[global _fdiml]
SECTION data CLASS=DATA USE32
nm	db	"fdim",0

SECTION code CLASS=CODE USE32
_fdimf:
    lea	edx,[esp+8]
    fld	dword [edx]
    lea	ecx,[esp+4]
    fld	dword [ecx]
    sub dl,dl
    jmp short ffdim
_fdiml:
_fdim:
    lea	edx,[esp+12]
    fld	qword [edx]
    lea	ecx,[esp+4]
    fld	qword [ecx]
    mov dl,1
ffdim:
    lea eax,[nm]
    call clearmath
    fcom st1
    fstsw   ax
    sahf
    ja  short doxchg 
    poptwo
    fldz
    jmp wrapmath
doxchg:
    fxch  st1
    fsubp st1
    jmp wrapmath
