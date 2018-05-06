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
[export _lgamma]
[export _lgammaf]
[export _lgammal]
%endif
[global _lgamma]
[global _lgammaf]
[global _lgammal]

[extern _floorl]
[extern ___GetSignGam]

SECTION data CLASS=DATA USE32
nm	db	"lgamma",0
tbla	dt 	8.11614167470508450300E-4
        dt	-5.95061904284301438324E-4
         dt	7.93650340457716943945E-4
        dt	-2.77777777730099687205E-3
        dt	8.33333333333331927722E-2
tblb	dt	-1.37825152569120859100E3
        dt	-3.88016315134637840924E4
        dt	-3.31612992738871184744E5
        dt	-1.16237097492762307383E6
        dt	-1.72173700820839662146E6
        dt	-8.53555664245765465627E5
tblc	dt	1.0
        dt	-3.51815701436523470549E2
        dt	-1.70642106651881159223E4
        dt	-2.20528590553854454839E5
        dt	-1.13933444367982507207E6
        dt	-2.53252307177582951285E6
        dt	-2.01889141433532773231E6
tbld	dt	7.9365079365079365079365e-4
        dt	-2.7777777777777777777778e-3
        dt	8.3333333333333333333333
ls2pi	dt	0.91893853320467274178
logpi	dt	1.14472988584940017414
pi		dt  3.14159265358979323846
maxlgm	dt	2.556348e305
m34		dd	-34.0
pt5		dd	0.5
thirteen	dd	13.0
three	dd	3.0
two		dd	2.0
onee8	dd	1.0e8
thousand	dd	1000.0
infinite	dd	7f800000h
SECTION code CLASS=CODE USE32
_lgammaf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short lgamma
_lgammal:
    lea	ecx,[esp+4]
    fld	tword[ecx]
    mov dl,2
    jmp short lgamma
_lgamma:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
lgamma:
    lea eax,[nm]
    call    clearmath
    push	esi
    mov		esi,1		;sgngam
    call	dolgamma
    jc	gammaerr
    call	___GetSignGam
    mov		[eax],esi
    pop		esi
    jmp		wrapmath
gammaerr:
    pop		esi
    ret
dolgamma:
    fld	dword	[m34]
    fcomp	st1
    fstsw	ax
    sahf
    jnc		near small
    fld	dword	[thirteen]
    fcomp	st1
    fstsw	ax
    sahf
    jnc		near aroundzero
    fld	dword	[maxlgm]
    fcomp	st1
    fstsw	ax
    sahf
    jc		derr1
    fld	st0
    fldln2
    fxch
    fyl2x
    fld st1
    fld	dword [pt5]
    fsubp	st1
    fmulp	st1
    fsub	st0,st1
    fld	tword	[ls2pi]
    faddp	st1
    fxch	st1
    fld	dword	[onee8]
    fcomp	st1
    fstsw	ax
    sahf
    ja		fin2
    popone
    clc
    ret
derr1:
    popone
    jmp	domainerr
fin2:
    ; q x
    fld	st0
    fmul	st0,st0
    fld1
    fdivrp	st1
    ;q x p
    fld	st1
    fld	dword	[thousand]
    fcompp
    fstsw	ax
    sahf
    lea		eax,[tbld]
    mov		ecx,2
    jbe		fin3
    lea		eax,[tbla]
    mov		ecx,4
fin3:
    call	ipoly
    fdivrp	st1
    faddp	st1
    ret
small:
    sub	esp,4 * 12
    fabs
    fld	st0
    fstp	tword [esp]
    call	dolgamma	; may modify ESI
    fstp	tword [esp + 24]
    call	_floorl
    lea eax,[nm]
    call    clearmath
    fld	st0
    fstp	tword [esp + 12]
    fld		tword [esp]
    call	sinularerr
    jc		serr2
    fld		tword [esp]
    push	ecx
    fxch	st1
    fist	dword [esp]
    pop		eax
    mov		esi, -1
    shr		eax,1
    jnc		ssm1
    mov		esi,1
ssm1:
    fsubp	st1
    fld		dword [pt5]
    fcomp	st1
    fstsw	ax
    sahf
    jae		nozmod
    fld		tword [esp + 12]
    fld1
    faddp	st1
    fld	st0
    fstp	tword [esp + 12]
    fld		tword [esp]
    fsubp	st1
nozmod:
    fld		tword [pi]
    fmulp	st1
    fsin
    fld		tword [esp]
    fmulp	st1
    fldz
    call	sinularerr
    jc		serr2
    fldln2
    fxch
    fyl2x
    fld		tword [esp + 24]	
    faddp	st1
    fld		tword [logpi]
    fsubrp	st1
    add		esp,4*12
    clc
    ret
serr2:
    popone
    fld		dword [infinite]
    add		esp,4*12
    stc
    ret
serr3:
    poptwo
    poptwo
    fld		dword [infinite]
    stc
    ret
aroundzero:
    fld1
    fldz
    fld	st2
n3l:
    fld	dword [three]
    fcomp st1
    fstsw	ax
    sahf
    ja	n3
    popone
    fld1
    fsubp	st1
    fld		st0
    fadd	st0,st3
    fmul	st2,st0
    jmp	n3l
n3:
    fld	dword [two]
    fcomp st1
    fstsw	ax
    sahf
    jbe	n2
    fldz
    call	sinularerr
    jc		serr3
    fdiv	st2,st0
    popone
    fld1
    faddp	st1
    fld	st0
    fadd	st0,st3
    jmp	n3
n2:
    fldz
    fcomp	st3
    mov		esi,1
    fstsw	ax
    sahf
    jb		nosgm1
    neg		esi
    fxch	st2
    fabs
    fxch	st2
nosgm1:
    fld	dword [two]
    fcompp
    fstsw	ax
    sahf
    jne		un2
    popone
    fxch	st1
    popone
    fldln2
    fxch
    fyl2x
    clc
    ret
un2:
    fxch	st1
    fldln2
    fxch
    fyl2x
    fxch	st2	
    faddp	st1
    fld	dword [two]
    fsubp	st1	
    ; logz x
    fld	st0
    lea	eax,[tblb]
    mov	ecx,5
    call	ipoly
    ; logz x polevl
    fld		st1
    fmulp	st1
    fxch	st1
    ; logz polevel*x x
    lea	eax,[tblc]
    mov	ecx,6
    call	ipoly
    fdivp	st1
    faddp	st1
    clc
    ret
ipoly:
    fld		tword [eax]
    add		eax,10
ipl:
    fld		st1
    fmulp	st1
    fld		tword [eax]
    add		eax,10
    faddp	st1
    loop	ipl
    fxch	st1
    popone
    ret
    