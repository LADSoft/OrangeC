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
[export _asinh]
[export _asinhf]
[export _asinhl]
%endif
[global _asinh]
[global _asinhf]
[global _asinhl]
[extern _hypotl]
[extern lexp1]
SECTION data CLASS=DATA USE32
nm	db	"asinh",0
small dt 4.7683715820312499e-07
medium dt 2.44140625e-4
big dq	4294967296.0
threeforty dt 0.075
m16 dt - 1.66666666666666666667E-1
SECTION code CLASS=CODE USE32
_asinhf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short asinh
_asinhl:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short asinh
_asinh:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
asinh:
    lea	eax,[nm]
    call    clearmath
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
    fld	tword [medium]
    fcomp	st1
    fstsw	ax
    sahf
    jbe	nomedium
    fld	st0
    fmul	st0,st0
    ; z z^2
    fld		tword [threeforty]
    fmul	st0,st1
    ; z z^2 z^2a
    fld		tword [m16]
    faddp	st1
    fmulp	st1
    fld1
    faddp	st1
    fmulp	st1
    jmp	fini3
nomedium:
    fld	qword [big]
    fcomp	st1
    fstsw	ax
    sahf
    jae		nobig
    fadd	st0,st0
    jmp		fini
nobig:
    fld	st0
    fmul	st0,st0
    fld1
    faddp	st1
    fsqrt
    faddp st1
fini:
    fldln2
    fxch
    fyl2x
fini3:
    or	ecx,ecx
    jz	fini2
    fchs
fini2:
    jmp	wrapmath
