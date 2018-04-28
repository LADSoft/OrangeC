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
[export _polyf]
[export _polevlf]
[export _p1evlf]
%endif
[global _polyf]
[global _polevlf]
[global _p1evlf]

SECTION data CLASS=DATA USE32
nm	db	"polyf",0
nmo db  "polevlf",0
nm1 db  "p1evlf" , 0
SECTION code CLASS=CODE USE32
_polyf:
    lea	eax,[nm]
    call clearmath
    mov	ecx,[esp+8]
    mov	eax,[esp+12]
    lea	eax,[eax + ecx * 4]
    fld	dword [esp+4]
    fld	dword [eax]
    sub	eax,4
lp:
    fmul	st0,st1
    fld		dword [eax]
    sub		eax,4
    faddp	st1
    loop	lp
    fxch
    popone
    mov dl,1
    jmp wrapmath

_polevlf:
    lea	eax,[nmo]
    call clearmath
    mov	eax,[esp+8]
    mov	ecx,[esp+12]
    fld	dword [esp+4]
    fld	dword [eax]
    add	eax,4
lpo:
    fmul	st0,st1
    fld		dword [eax]
    add		eax,4
    faddp	st1
    loop	lpo
    fxch
    popone
    mov dl,1
    jmp wrapmath
_p1evlf:
    lea	eax,[nm1]
    call clearmath
    mov	eax,[esp+8]
    mov	ecx,[esp+12]
    fld	dword [esp+4]
    fld	st0
    jmp	intoo
lp1:
    fmul	st0,st1
intoo:
    fld		dword [eax]
    add		eax,4
    faddp	st1
    loop	lp1
    fxch
    popone
    mov dl,1
    jmp wrapmath
