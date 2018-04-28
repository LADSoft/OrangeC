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

[global ___fextract]
[global ___fnd]
[global ___fpow]

SECTION code CLASS=CODE USE32
;
; get exponent and mantissa and sign
;
; returns 
;  0 = ok vals
;  -1 = nan
;  1 = infinity or denormal
;
; int _fextract(long double *val, int *exp, int *sign, unsigned char *BCD);
;
___fextract:
    enter	4,0
    push	esi
    push	ebx
    fstcw	[ebp-2]
    mov	ax,[ebp-2]
    or	ah,0ch		; round toward zero
    mov	[ebp-4],ax
    fldcw	[ebp-4]

    mov	esi,[ebp + 8]	; get val
    fld	tword [esi]

    ftst       		; test to get sign
    fstsw	ax
    sahf
    jnz	dosign		; check for zero (can't take log of zero)

    fxam
    fstsw	ax
    and	ah,2
    mov	eax,1
    jz	zsign
    mov	eax,-1
zsign:
    mov	esi,[ebp+16]	; store sign
    mov	[esi],eax

    sub	eax,eax		; if so all vals = 0
    mov	esi,[ebp+12]
    mov	dword [esi],eax
    mov esi,[ebp+20]
    mov dword [esi],eax
    mov dword [esi+4],eax
    mov dword [esi+8],eax
    jmp	fxxr
dosign:
    mov	eax,1		; nonzero, finally get sign
    jnc	pos
    fabs       		; we will work with positive nums hereafter
    mov	eax,-1
pos:
    mov	esi,[ebp+16]	; store sign
    mov	[esi],eax

    fxam			; retest to get class
    fstsw	ax
    mov	bl,ah
    and	bl,45h
    mov	eax,-1		; assume Nan
    cmp	bl,1
    jz	fxxr
    mov	eax,1		; assume infinite
    cmp	bl,5
    jz	fxxr
;	cmp	bl,44h		; denormal
;	jz	fxxr

    fldlg2       		; log to base 10
    fxch
    fyl2x
    
    fld	st0		; get int part
    frndint
    mov	esi,[ebp+12]
    fist	dword [esi]

    fsubp  	st1,st0		; fraction
    fldl2t			; convert back to base 2
    fmulp	st1,st0

    fld	st0		; lovely exponentiation
    frndint
    fxch
    fsub	st0,st1
    f2xm1			;
    fld1
    faddp	st1,st0
    fscale
    fxch
    fcomp	st1

    fld st0    
    mov	esi,[ebp+8]
    fstp	tword [esi]
    push 17
    call ___fpow
    pop eax
    fmulp st1
    mov esi,[ebp+014h]
    fbstp tword [esi+2]
    mov word [esi],18
    
    sub	eax,eax
fxx:
    fnclex
    fldcw	[ebp-2]
    fwait
    pop	ebx
    pop	esi
    leave
    ret
fxxr:
    fstp	st0
    jmp	fxx
;
; get next digit from mantissa
;
; int _fnd(unsigned char *BCD, int index);
;
___fnd:
    push	esi
    mov	esi,[esp+8]
    mov eax,[esp+12]
    mov cl,al
    cmp al,[esi]
    jae ret0
    shr eax,1
    sub eax,8
    neg eax
    mov al,[esi+eax+2]
    test cl,1
    jnz fndx
    shr al,4
fndx:
    and al,0fh
    pop	esi
    ret
ret0:
    sub eax,eax
    pop esi
    ret
;
; calculate a power of 10
;
; long double __fpow(int exp);
;
___fpow:
    enter	4,0
    fstcw	[ebp-2]
    mov	ax,[ebp-2]
    or	ah,0ch		; round toward zero
    mov	[ebp-4],ax
    fldcw	[ebp-4]

    fild	dword [ebp + 8] ; y * log base 2 of 10
    fldl2t
    fmulp   st1,st0

    fld	st0		; integral part
    frndint

    fsub	st1,st0
    fxch 			; fractional part

    f2xm1                 	; exponentiate fraction
    fld1			; base 2
    faddp   st1,st0

    fscale  		; now scale in integral part

    fxch			; aren't intel processors a blast?
    fcomp	st1		; have to clear stack

    fclex
    fldcw	[ebp-2]
    leave
    ret
