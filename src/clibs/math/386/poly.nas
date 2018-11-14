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
[export _poly]
[export _polevl]
[export _p1evl]
%endif
[global _poly]
[global _polevl]
[global _p1evl]

SECTION data CLASS=DATA USE32
nm	db	"poly",0
nmo db  "polevl",0
nm1 db  "p1evl" , 0
SECTION code CLASS=CODE USE32
_poly:
	push ebp
	mov ebp,esp
    lea	eax,[nm]
    call clearmath
    mov	ecx,[ebp+16]
    mov	eax,[ebp+20]
    lea	eax,[eax + ecx * 8]
    fld	qword [ebp+8]
    fld	qword [eax]
    sub	eax,8
lp:
    fmul	st0,st1
    fld		qword [eax]
    sub		eax,8
    faddp	st1
    loop	lp
    fxch
    popone
    mov dl,1
    call wrapmath
	pop ebp
	ret

_polevl:
	push ebp
	mov ebp,esp
    lea	eax,[nmo]
    call clearmath
    mov	eax,[ebp+16]
    mov	ecx,[ebp+20]
    fld	qword [ebp+8]
    fld	qword [eax]
    add	eax,8
lpo:
    fmul	st0,st1
    fld		qword [eax]
    add		eax,8
    faddp	st1
    loop	lpo
    fxch
    popone
    mov dl,1
    call wrapmath
	pop ebp
	ret
_p1evl:
	push ebp
	mov ebp,esp
    lea	eax,[nm1]
    call clearmath
    mov	eax,[ebp+16]
    mov	ecx,[ebp+20]
    fld	qword [ebp+8]
    fld	st0
    jmp	intoo
lp1:
    fmul	st0,st1
intoo:
    fld		qword [eax]
    add		eax,8
    faddp	st1
    loop	lp1
    fxch
    popone
    mov dl,1
    call wrapmath
	pop ebp
	ret
