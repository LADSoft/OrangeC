;  Software License Agreement
;  
;      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

global	floatcheck, fpcommand

%include "prints.ase" 
%include "input.ase"
%include "mtrap.ase"
%include "breaks.ase"
%include "exec.ase"

;
; hack to help MASM versions that die on pop-fmath
;
    segment	data	USE32
ten		dd	10
pointone	dt	0.1
nan		dd	7fffffffh	; quiet nan
inf		dd	7f800000h	; plus infinity
curdig		dd	0		;current digit for input
work		dw	0		;used in detecting FP hardware
fpflag		db	0		;flag if we can use FP commands
floatstat 	times	54 [dw 0]	;FNSAVE/FNSTOR buffer
enames		db	"IDZOUP"	;exception names


    segment code USE32
prsing	db	"Single",0
prdbl	db	"Double",0
prxt	db	"Extended",0
reserved db	"Reserved",0
rdnear	db	"Nearest",0
rdzer	db	"Zero",0
rdplus	db	"Plus Infinity",0
rdminus db	"Minus Infinity",0
precmsg	dd	prsing,reserved
    dd	prdbl,prxt
roundmsg dd	rdnear,rdminus
    dd	rdplus,rdzer

;
; check for floating point unit
;
floatcheck	:
        mov	byte [fpflag],1
        mov	word [work],4567h
        fninit
        fstsw 	word [work]
        cmp	word [work],0
        jz	hasfp
        mov	byte [fpflag],0
hasfp:
        ret
stackpos :
        mov     ebx,ecx
        imul    ebx,[ten]
        push	eax
        mov     eax,floatstat+28
        add		ebx,eax
        pop		eax
        ret
;
; fp commands
;
fpcommand	:
    test	byte [fpflag],1
    jnz	hasfloat
    call	Message
    db	13,10,"No FP unit", 0
    clc
    ret
hasfloat:
    fnsave	[floatstat]
;        fninit
    fwait
    inc		esi
    call	WadeSpace
    jz	near dumpstack
    cmp	al,'s'
    jnz	near flread
;
; status command
;
    inc	si
    call	WadeSpace
    jnz	near flerr
        frstor [floatstat]
        call	Message
        db	13,10,"Control Word:      ", 0
        mov     ax,[floatstat]
        call    PrintWord
        Call	Message
        db	13,10, "Status Word:       ", 0
        mov     ax,[floatstat+4]
        call    PrintWord
        Call	Message
    db 13,10, "Masked exceptions: ", 0
    mov	dx,word [floatstat]
    call	except
        Call	Message
    db	13,10, "Active exceptions: ", 0
    mov	dx,word [floatstat+4]
    call	except
    Call	Message
    db	13,10, "Precision:         ", 0
    movzx	ebx,byte [floatstat+1]
    and	bl,3
    shl	bl,2
    mov	ebx,[ebx+precmsg]
    call	olMessage
    Call	Message
    db	13,10, "Rounding:          ", 0
    movzx	ebx,byte [floatstat+1]
    and	bl,0ch
    shr	bl,2
    mov	ebx,[ebx+roundmsg]
    call	olMessage
    Call	Message
    db	13,10, "FPU Status flags:  ", 0
    test	word [floatstat+4],100h
    jz	notc0
    Call	Message
    db	"C0 ",0
notc0:
    test	word [floatstat+4],200h
    jz	notc1
    Call	Message
    db	"C1 ",0
notc1:
    test	word [floatstat+4],400h
    jz	notc2
    Call	Message
    db	"C2 ",0
notc2:
    test	word [floatstat+4],4000h
    jz	notc3
    Call	Message
    db	"C3 ",0
notc3:
;	mov	dl,C_DEFAULT
;	call PutChar
    clc
    ret
;
; dump the FP stack
;
dumpstack:
    Call	Message
    db	13,10, "Control Word:      ", 0
        mov     ax,[floatstat]
        call    PrintWord
    Call	Message
    db	13,10,"Status Word:       ", 0
        mov     ax,[floatstat+4]
        call    PrintWord
    Call	Message
    db	13,10,0
    mov	ecx,8
dsl:
    push	ecx
        sub     ecx,8
        neg     ecx
        call    stackpos
    call	dumpval
    pop	ecx
    loop	dsl
        frstor [floatstat]
;	mov	dl,C_DEFAULT
;	call PutChar
    clc
    ret
;
; read an FP number into a reg
;
flread:
    sub	al,'0'
    jc	flerr
    cmp	al,8
    jae	flerr
        sub     al,7
        neg     al
    push	eax
    inc	esi
    call	WadeSpace
    jz	flerr2
    call	ReadFpNumber
    jc	flerr2
    pop	eax
    mov	cl,al
    call	testtag
    jz	flerr3
        movzx   ecx,al
        sub     ecx,7
        neg     ecx
        call    stackpos
        fstp    tword [ebx]
        frstor [floatstat]
    clc
    ret
    
testtag	:
    and	cl,7
    add	cl,cl
    movzx	ebx,word [floatstat+8]
    shr	ebx,cl
    and	bl,3
    cmp	bl,3
    ret

flerr3:
        frstor  [floatstat]
    Call	Message
    db	13,10,"err - empty reg", 0
    clc
    ret
flerr2:
    add	sp,4
flerr:
        frstor [floatstat]
    stc
    ret
except	:
    mov	esi,enames
    mov	ecx,6
exlp:
    shr	edx,1
    lodsb
    jnc	nhr
    push	edx
       mov	dl,al
    call	PutChar
    mov	dl,'e'
    call	PutChar
    call	PrintSpace
    pop	edx
nhr:
    loop	exlp
    ret
;
; dump the value of a stack register
;
dumpval	:
    call	crlf
        push    ebx
        push    ecx
    Call	Message
    db	"st",0
        pop     ecx
        pop     ebx
        
    mov	dl,cl
    add	dl,'0'
    call	PutChar
;	mov	dl,	C_DEFAULT
;	call	PutChar
        mov     dl,':'
    call	PutChar
    call	PrintSpace
    call	PrintSpace
    call	PrintSpace
        mov     ax,word [floatstat+4]
        shr     ax,11
        add     cl,al
        and     cl,7
    mov	ax,word [floatstat+8]
    shr	ax,cl
    shr	ax,cl
    and	al,3
    cmp	al,3
    jnz	notempty
    Call	Message
    db	"<Empty>", 0
    clc
    ret

notempty:
    cmp	al,2
    jnz	notnan
    test	byte [ebx + 9],80h
    jz	nsinf
          mov	dl,'-'
    call	PutChar
nsinf:
    fld	tword [ebx]
    fxam
    fnstsw	ax
    fstp	st0
    sahf
    jpe	isinf
    Call	Message
    db	"<Nan>",0
    clc
    ret
isinf:
    Call	Message
    db	"<Inf>",0
    clc
    ret
notnan:
;	push	eax
;	mov	dl,C_NUMBER
;	call	PutChar
;	pop	eax
    cmp	al,1
    jnz	notzero
    test	byte [ebx + 9],80h
    jz	nszer
          mov	dl,'-'
    call	PutChar
nszer:
    Call	Message
    db	"<Zero>", 0
    clc
    ret
notzero:
    fld	tword [ebx]
    call	PrintFloating
    fcomp	st1
    clc
    ret
;
; actual FP print routines
;
PrintFloating	:
    call	fextract
    or	eax,eax
    jz	notdenorm
    Call	Message
    db	"<Denorm>", 0
    ret
notdenorm:
    or	edx,edx
    jns	noneg
    call	putneg
noneg:
    cmp	ebx,8		; getter be less than 16!!!!
    jg	highexp
    cmp	ebx,-4
    jle	lowexp
    or	ebx,ebx
    jge	gte0
;
; print for less than one but not exponential
;
    push	ebx
    call	putzer
    call	putper
    pop	ecx
    not	ecx
    jecxz	nozr
    inc	ecx
zrl:
    call	putzer
    loop	zrl
nozr:
    mov	ecx,16
nl2:
    call	fnd
    call	putdig
    loop	nl2
    ret
;
; print for greater than or equal to one but not exponential
;
gte0:
    push	ebx
    mov	ecx,ebx
    call	fnd
    call	putdig
    jcxz	gte0nl
gtel1:
    call	fnd
    call	putdig
    loop	gtel1
gte0nl:
    call	putper
    pop	ecx
    sub	ecx,16
    neg	ecx
gtel2:
    call	fnd
    call	putdig
    loop	gtel2
    ret
;
; hack to justify negative exponents
;
lowexp:
    dec	ebx
    call	fnd
;
; exponential print
;
highexp:
    push	ebx
    call	fnd
    call	putdig
    call	putper
    mov	ecx,16
hel1:
    call	fnd
    call	putdig
    loop	hel1
    mov	dl,'e'
    call	PutChar
    pop	eax
    or	eax,eax
    jns	nonegx
    push	eax
    call	putneg
    pop	eax
    neg	eax
nonegx:
    sub	ecx,ecx
elp1:
    sub	edx,edx
    div	dword [ten]
    push	edx
    inc	ecx
    or	eax,eax
    jnz	elp1
elp2:
    pop	eax
    call	putdig
    loop	elp2
    ret
putdig:
    mov	dl,al
    add	dl,'0'
    call	PutChar
           ret
putper:
    mov	dl,'.'
    call	PutChar
    ret
putneg:
    mov	dl,'-'
    call	PutChar
    ret
putzer:
    mov	dl,'0'
    call	PutChar
    ret
;
; get exponent and mantissa and sign
;
; enter:
;   st0 = value
;
; returns:
;   ax = 0	; ok val
;   
;     st0 = mantissa
;     bx = exp
;     dx = sign   ( +-1)
;
;   ax = -1	; infinity or dnormal
;
fextract 	:
    enter	8,0
    fstcw	[ebp-2]
    mov	ax,[ebp-2]
    or	ah,0ch		; round toward zero
    mov	[ebp-4],ax
    fldcw	[ebp-4]

    ftst
    fnstsw	ax
    fnclex			; just in case ... 
    sahf
    mov	eax,-1   	; first check for out of range
    jp	fxx
    jnz	dosign		; now check for zero
    sub	ebx,ebx
    sub	edx,edx
    jmp	fxx

dosign:
    mov	edx,1		; nonzero, finally get sign
    jnc	pos
    fabs       		; we will work with positive nums hereafter
    mov	edx,-1
pos:
    fldlg2       		; log to base 10
    fxch
    fyl2x
    
    fld	st0		; get int part
    frndint
    fist	dword [ebp-8]
    pop	ebx

    fsubp	st1,st0			; fraction
    fldl2t			; convert back to base 2
    fmulp	st1,st0

    fld	st0		; lovely exponentiation
    frndint
    fxch
    fld	st1
    fsubp	st1,st0
    f2xm1			;
    fld1
    faddp	st1,st0
    fscale
    fxch
    fcomp st1
    sub	eax,eax
fxx:
    fnclex
    fldcw	[ebp-2]
    fwait
    leave
    ret
;
; get next digit from mantissa
;
; enter:
;   mantissa from fextract on stack
;
; exit:
;   eax = next digit (base 10)
;   stack = new mantissa
;
fnd	:
    ENTER	8,0
    fstcw	[ebp-2]
    mov	ax,[ebp-2]
    or	ah,0ch		; round toward zero
    mov	[ebp-4],ax
    fldcw	[ebp-4]

    fld	st0		; next digit
    frndint
    fist	dword [ebp-8]

    fsubp	st1,st0			; new mantissa
    fimul	word [ten]

    fnclex
    fldcw	[ebp-2]
    fwait
    pop	eax		; cute trick to get result :)
    leave
    ret
;
; read floating point number
ReadFpNumber :
    sub	edx,edx		; sign = +
    cmp	al,'+'		; get sign bit
    jz	rfn_gotsign
    cmp	al,'-'
    jnz	rfn_nosign
    inc	edx		; minus sign, sign = '-'
rfn_gotsign:
    inc	esi		; step past sign
rfn_nosign:
    call	WadeSpace	; skip space
    jz	near rfn_err
    mov	eax,[esi]	; check for nan
    and	eax,0ffffffh
    cmp	eax,"nan"
    jnz	rfn_notnan
    fld	dword [nan]		; nan, load it
    jmp	rfn_ni
rfn_notnan:
    cmp	eax,"fni"	; check for inf
    jnz	rfn_notinf
    fld	dword [inf]		; inf load it
rfn_ni:
    add	esi,3		; skip past inf/nan
    call	WadeSpace	; err if more
    jnz	near rfn_err2
    jmp	rfn_done		; else tag in sign and exit
;
rfn_notinf:
    fldz            	; return val
    call	getdig		; see if leading digits
    jc	near rfn_err2		
    jz	rfn_nostart     ; go check for '.' if not digit
rfn_prelp:
    fimul	dword [ten]   	; else shift digit in
    fild	dword [curdig]
    faddp	st1,st0
    call	getdig     	; loop till all digits got
    jc	near rfn_err2
    jnz	rfn_prelp
    cmp	al,'.'		; now check for '.'
    jz	rfn_dot		; yes, do it
    jmp	rfn_exp
;
rfn_nostart:
    cmp	al,'.'		; no leading digit, must have dot
    jnz	near rfn_err
rfn_dot:
    call	getdig		; if no digits get exponent
    jc	near rfn_err2
    jz	rfn_exp
    fld	tword [pointone]	; else we have digits, load up 0.1
rfn_ptlp:
    fild	dword [curdig]	; load digit
    fmul	st0,st1	; mul by placeholder and add it in
    faddp	st2,st0
    fld	tword [pointone] 	; adjust placeholder
    fmulp	st1,st0
    call	getdig
    jc	near rfn_err3
    jnz	rfn_ptlp
    fstp	st0   	; get rid of placeholder
rfn_exp:
    cmp	al,'e'  	; check for exponent
    jnz	near rfn_done
    sub	ebx,ebx
    sub	ecx,ecx
rfn_exl:
    call	getdig		; first digit of exponent or sign
    jc	near rfn_err2
    jnz	rfn_nexs
    inc	ecx
    call	getdig
    jz	rfn_err2
    jc	rfn_err2
rfn_nexs:
    xchg	eax,ebx
    push	edx
    mul	dword [ten]
    pop	edx
    xchg	eax,ebx
    movzx	eax,al
    add	ebx,eax
    call	getdig
    jc	rfn_err2
    jnz	rfn_nexs
    or	ecx,ecx
    jz	rfn_nexs2
          neg	ebx
rfn_nexs2:
    cmp	ebx,1024		; verify range
    jg	rfn_err2
    cmp	ebx,-1024
    jl	rfn_err2
    enter	4,0		; need temp space now
    mov	[ebp-2],bx	; get exponent to fp stack
    fild	word [ebp-2]
    fldl2t                 	; convert exponent to logarithmic base 2
    fmulp	st1,st0
    fnstcw	[ebp-4]		; set rounding mode to zero
    mov	ax,[ebp-4]
    and	ah,0f3h
    or	ah,0ch
    mov	[ebp-2],ax
    fldcw	[ebp-2]
    fld	st0   	; extract int and fraction of exponent
    frndint
    fxch	st1
    fsub	st0,st1
    f2xm1              	; exponentiate the fraction
    fld1
    faddp	st1,st0
    fscale			; scale in the integer part of exponent
    fxch	st1  		; get rid of int part
    fcomp	st1
    fldcw	[ebp-4]		; restore round mode
    leave			; clear stack
    fmulp	st1,st0	; multiply base * exponent (may result in 
                ; an infinity)
rfn_done:
    or	edx,edx		; get the sign bit
    jz	rfn_x
    fchs			; to the FPU
rfn_x:
    clc
    ret

rfn_err3:
    fstp	st0
rfn_err2:
    fstp	st0
rfn_err:
    stc
    ret
;
; routine to get a digit, returns ZR for a '.' or 'e' or CR
; and Carry for an error, else NZ means a digit
;
getdig	:
    lodsb			; get val
    cmp	al,'0'		; check low end of digits
    jc	gds		; no, check high end
    cmp	al,'9'
    ja	gds
    sub	al,'0'
    mov	byte [curdig],al ; digit, store it for later
    or	esi,esi		; set nz
    ret
gds:
    cmp	al,'-'
    jz	gdsk
    cmp	al,'e'		; check for 'e'
    jz	gdsk
    cmp	al,'.'		; '.'
    jz	gdsk
    cmp	al,13		; CR
    jnz	gdsk
    pushf			; if is CR bump char ptr back so
    dec	esi		; we don't overrun
    popf
gdsk:
    clc
    ret
