;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; math for sprintf/scanf/atof floating point support
;
;
; this basically does NO checking for exceptions.
; It does clear them after the fact :)
; however, I do try to keep everything in bounds before doing any math...
;
;
; rounding is done towards zero...
;
; later I will fix it...
;
; note I AM using log functions rather than a table-driven approach
;
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
