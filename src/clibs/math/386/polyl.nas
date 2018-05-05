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

%include "matherr.inc"
%ifdef __BUILDING_LSCRTL_DLL
[export _polyl]
[export _polevll]
[export _p1evll]
%endif
[global _polyl]
[global _polevll]
[global _p1evll]

SECTION data CLASS=DATA USE32
nm	db	"polyl",0
nmo db  "polevll",0
nm1 db  "p1evll" , 0
SECTION code CLASS=CODE USE32
_polyl:
    lea	eax,[nm]
    call clearmath
    mov	ecx,[esp+16]
    mov	eax,[esp+20]
    lea	eax,[eax + ecx * 8]
    lea	eax,[eax + ecx * 2]
    fld	tword [esp+4]
    fld	tword [eax]
    sub	eax,10
lp:
    fmul	st0,st1
    fld		tword [eax]
    sub		eax,10
    faddp	st1
    loop	lp
    fxch
    popone
    mov dl,1
    jmp wrapmath

_polevll:
    lea	eax,[nmo]
    call clearmath
    mov	eax,[esp+16]
    mov	ecx,[esp+20]
    fld	tword [esp+4]
    fld	tword [eax]
    add	eax,10
lpo:
    fmul	st0,st1
    fld		tword [eax]
    add		eax,10
    faddp	st1
    loop	lpo
    fxch
    popone
    mov dl,1
    jmp wrapmath
_p1evll:
    lea	eax,[nm1]
    call clearmath
    mov	eax,[esp+16]
    mov	ecx,[esp+20]
    fld	tword [esp+4]
    fld	st0
    jmp	intoo
lp1:
    fmul	st0,st1
intoo:
    fld		tword [eax]
    add		eax,10
    faddp	st1
    loop	lp1
    fxch
    popone
    mov dl,1
    jmp wrapmath
