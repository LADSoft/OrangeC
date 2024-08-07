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
[export _tgamma]
[export _tgammaf]
[export _tgammal]
%endif
[global _tgamma]
[global _tgammaf]
[global _tgammal]

[extern _floorl]
[extern exp]
[extern _powl]
SECTION data CLASS=DATA USE32
nm	db	"tgamma",0
tblP dt  1.60119522476751861407E-4
     dt  1.19135147006586384913E-3
     dt  1.04213797561761569935E-2
     dt  4.76367800457137231464E-2
     dt  2.07448227648435975150E-1
     dt  4.94214826801497100753E-1
     dt  9.99999999999999996796E-1
tblQ dt  -2.31581873324120129819E-5
     dt	 5.39605580493303397842E-4
     dt	-4.45641913851797240494E-3
     dt	 1.18139785222060435552E-2
     dt	 3.58236398605498653373E-2
     dt	-2.34591795718243348568E-1
     dt	 7.14304917030273074085E-2
     dt	 1.00000000000000000320E0
tblstir dt	 7.87311395793093628397E-4
     dt	-2.29549961613378126380E-4
     dt	-2.68132617805781232825E-3
     dt	 3.47222221605458667310E-3
     dt	 8.33333333333482257126E-2
maxgam	dt 171.624376956302725
maxstir dt 143.01608
sqtpi	dt 2.50662827463100050242E0
logpi	dt	1.14472988584940017414
pi		dt  3.14159265358979323846
infinite	dd	7f800000h
p33	dd	33.0
o577	dt	0.5772156649015329
pt5		dd	0.5
two		dd	2.0
three	dd	3.0
quarter	dd	0.25
msmall	dt	-1.0e-9
psmall	dt	1.0e-9

SECTION code CLASS=CODE USE32
_tgammaf:
    lea	ecx,[esp+4]
    fld	dword[ecx]
    sub dl,dl
    jmp short tgamma
_tgammal:
_tgamma:
    lea	ecx,[esp+4]
    fld	qword[ecx]
    mov dl,1
tgamma:
    lea eax,[nm]
    call    clearmath
    push	esi
    mov		esi,1		;sgngam
    call	dotgamma
    pop		esi
    jnc		wrapmath
    ret
dotgamma:
    fld	st0
    fabs
    fld	dword [p33]
    fcomp	st1
    fstsw	ax
    sahf
    jc	near dostir	
    ; x q
    popone
    fld1
    ; x z
l3l:
    fld	dword [three]
    fcomp	st2
    fstsw	ax
    sahf
    ja	l3lx
    fld1
    fsubp	st2
    fmul	st0,st1
    jmp	l3l
l3lx:
l0l:
    fldz
    fcomp	st2
    fstsw	ax
    sahf
    jbe	l0lx
    fld	tword [msmall]
    fcomp	st2
    fstsw	ax
    sahf
    jc	dosmall
    fdiv	st0,st1
    fld1
    faddp	st2
    jmp	l0l
l0lx:
l2l:
    fld	dword [two]
    fcomp	st2
    fstsw	ax
    sahf
    jbe	l2lx
    fld	tword [psmall]
    fcomp	st2
    fstsw	ax
    sahf
    ja	dosmall
    fdiv	st0,st1
    fld1
    faddp	st2
    jmp	l2l
l2lx:
    fld	dword [two]
    fcom	st2
    fnstsw	ax
    sahf
    jne	nottwo
    popone
    fxch st1
    popone
    clc
    ret
nottwo:
    fsubp	st2
    ; x z
    fld	st1
    lea	eax,[tblP]
    mov	ecx,6
    call	ipoly
    fmulp	st1
    fxch	st1
    lea	eax,[tblQ]
    mov	ecx,7
    call	ipoly
    fdivp	st1
    clc
    ret
dosmall:
    ; x z
    fxch	st1
    fldz
    call	sinularerr
    jc		near serr2
    fxch	st1
    fld	tword [o577]
    fmul	st0, st2
    fld1
    faddp	st1
    fmul	st0, st2
    fdivp	st1
    fxch	st1
    popone
    clc
    ret
        
dostir:
    fldz
    fcomp	st2
    fstsw	ax
    sahf
    ja	stirl0
    popone
    call	stirf
    clc
    ret
stirl0:
    sub	esp,12
    fstp	tword [esp]
    call	_floorl
    lea eax,[nm]
    call    clearmath
    fld		tword [esp]
    add		esp,12
    ; x p q
    fld	st1
    call	sinularerr
    jc	serr1
    push	ecx
    fxch	st1
    fist	dword [esp]
    pop		eax
    shr		eax,1
    jc		noneg
    mov		esi,-1
noneg:
    ; x q p
    fld	st1
    fsubr	st1
    ; x q p z
    fld	dword [pt5]
    fcomp	st1
    fstsw	ax
    sahf
    jae	noneg2
    fld1
    fadd	st2, st0
    fsubp	st1
noneg2:
    fld	tword [pi]
    fmulp	st1
    fsin
    fmul	st0,st2
    ; x q p z
    fldz
    fcomp	st1
    fstsw	ax
    sahf
    je	oerr
    fabs
    fxch	st1
    popone
    fxch 	st2
    popone
    call	stirf
    fmulp	st1
    fld	tword [pi]
    fdivrp	st1
    or	esi,esi
    jns	noneg3
    fchs
noneg3:
    clc
    ret
    
oerr:
    popone
    call	overflow
serr1:
    popone
serr2:
    poptwo
    fld	dword [infinite]
    stc
    ret	

stirf:
    fld	st0
    fld1
    fdivrp	st1
    fld	st0
    lea	eax,[tblstir]
    mov	ecx,4
    call	ipoly
    fmulp st1
    fld1
    faddp st1
    ; x w
    fld	st1
    call	exp
    ; x w y
    fld	tword [maxstir]
    fcomp st3
    fstsw ax
    sahf
    ja	stirfsimple
    fld st2
    fld	dword [pt5]
    fmulp st1
    fld dword [quarter]
    fsubp st1
    sub	esp,24
    fstp	tword [esp + 12]
    fld st2
    fstp	tword [esp]
    call	_powl	
    lea eax,[nm]
    call    clearmath
    add	esp,24
    ; x w y v
    fxch	st1
    fdivr	st0,st1
    fmulp	st1
    jmp	stirfn
stirfsimple:
    fld st2
    fld	dword [pt5]
    fsubp st1
    sub	esp,24
    fstp	tword [esp + 12]
    fld st2
    fstp	tword [esp]
    call	_powl	
    lea eax,[nm]
    call    clearmath
    add	esp,24
    ; x w y v
    fdivrp st1
stirfn:
    ; x w y
    fmulp	st1
    fxch	st1
    popone
    fld	tword [sqtpi]
    fmulp st1
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
    