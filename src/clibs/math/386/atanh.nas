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
[export _atanh]
[export _atanhf]
[export _atanhl]
%endif
[global _atanh]
[global _atanhf]
[global _atanhl]
SECTION data CLASS=DATA USE32
nm	db	"atanh",0
small dt 4.7683715820312499e-07
two	dd	2.0

SECTION code CLASS=CODE USE32
_atanhf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short atanh
_atanhl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short atanh
_atanh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
atanh:
    lea	eax,[nm]
    call    clearmath
    fld	st0
    fabs
    fld1
    fcompp
    fstsw	ax
    sahf
    jbe		domainerr
    sub ecx,ecx
    fldz
    fcomp	st1
    fstsw	ax
    sahf
    jc	noneg
    inc ecx
noneg:
    fld	st0
    fabs
    fld	tword [small]
    fcomp	st1
    fstsw	ax
    sahf
    jbe	nosmall
    popone
    ret
nosmall:
    fxch	st1
    popone
    fld1
    fadd	st0,st1
    fxch	st1
    fld1
    fsubrp	st1
    fdivp	st1	
fini:
    fldln2
    fxch
    fyl2x
    fld	dword [two]
    fdivp	st1
    or	ecx,ecx
    jz	fini2
    fchs
fini2:
    jmp	wrapmath
