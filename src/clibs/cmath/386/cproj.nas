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
[export _cproj]
[export _cprojf]
[export _cprojl]
%endif
[global _cproj]
[global _cprojf]
[global _cprojl]
SECTION data CLASS=DATA USE32
nm	db	"cproj",0

infinity dw 0,0,0,8000h,7fffh

SECTION code CLASS=CODE USE32

_cprojf:
    lea	ecx,[esp+4]
    fld dword[ecx+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short xexp
_cprojl:
_cproj:
    lea	ecx,[esp+4]
    fld qword[ecx+8]
    fld	qword[ecx]
    mov dl,1
xexp:
    lea eax,[nm]
    call clearmath
    fxam
    fnstsw ax
    and ah,45h
    cmp ah,5
    jz  infin
    fxch
    fxam
    fxch
    fnstsw ax
    and ah,45h
    cmp ah,5
    jnz wrapcomplex
infin:
    popone
    ftst
    fnstsw ax
    popone
    fldz
    jae nochs
    fchs
nochs:
    fld tword [infinity]
    jmp wrapcomplex
