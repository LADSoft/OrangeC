[bits 32]
	f2xm1	
nop
nop
nop
	fabs	
nop
nop
nop
	fchs	
nop
nop
nop
	fclex	
nop
nop
nop
	fnclex	
nop
nop
nop
	fcompp	
nop
nop
nop
	fcos	
nop
nop
nop
	fdecstp	
nop
nop
nop
	fdisi	
nop
nop
nop
	feni	
nop
nop
nop
	fincstp	
nop
nop
nop
	finit	
nop
nop
nop
	fninit	
nop
nop
nop
	fld1	
nop
nop
nop
	fldl2e	
nop
nop
nop
	fldl2t	
nop
nop
nop
	fldlg2	
nop
nop
nop
	fldln2	
nop
nop
nop
	fldpi	
nop
nop
nop
	fldz	
nop
nop
nop
	fnop	
nop
nop
nop
	fpatan	
nop
nop
nop
	fprem	
nop
nop
nop
	fprem1	
nop
nop
nop
	fptan	
nop
nop
nop
	frndint	
nop
nop
nop
	fscale	
nop
nop
nop
	fsetpm	
nop
nop
nop
	fsin	
nop
nop
nop
	fsincos	
nop
nop
nop
	fsqrt	
nop
nop
nop
	ftst	
nop
nop
nop
	fucompp	
nop
nop
nop
	fwait	
nop
nop
nop
	fxam	
nop
nop
nop
	fxtract	
nop
nop
nop
	fyl2x	
nop
nop
nop
	fyl2xp1	
nop
nop
nop
	fild	word [ecx]
nop
nop
nop
	fild	dword [ecx]
nop
nop
nop
	fild	qword [ecx]
nop
nop
nop
	fistp	dword [ecx]
nop
nop
nop
	fistp	qword [ecx]
nop
nop
nop
	fldcw	[ecx]
nop
nop
nop
	fldcw	word [ecx]
nop
nop
nop
	fstcw	[ecx]
nop
nop
nop
	fstcw	word [ecx]
nop
nop
nop
	fnstcw	[ecx]
nop
nop
nop
	fnstcw	word [ecx]
nop
nop
nop
	fadd	st0, st5
nop
nop
nop
	fadd	st5, st0
nop
nop
nop
	fadd	st5
nop
nop
nop
	fadd	dword [ecx]
nop
nop
nop
	fadd	qword [ecx]
nop
nop
nop
	fdiv	st0, st5
nop
nop
nop
	fdiv	st5, st0
nop
nop
nop
	fdiv	st5
nop
nop
nop
	fdiv	dword [ecx]
nop
nop
nop
	fdiv	qword [ecx]
nop
nop
nop
	fdivr	st0, st5
nop
nop
nop
	fdivr	st5, st0
nop
nop
nop
	fdivr	st5
nop
nop
nop
	fdivr	dword [ecx]
nop
nop
nop
	fdivr	qword [ecx]
nop
nop
nop
	fmul	st0, st5
nop
nop
nop
	fmul	st5, st0
nop
nop
nop
	fmul	st5
nop
nop
nop
	fmul	dword [ecx]
nop
nop
nop
	fmul	qword [ecx]
nop
nop
nop
	fsub	st0, st5
nop
nop
nop
	fsub	st5, st0
nop
nop
nop
	fsub	st5
nop
nop
nop
	fsub	dword [ecx]
nop
nop
nop
	fsub	qword [ecx]
nop
nop
nop
	fsubr	st0, st5
nop
nop
nop
	fsubr	st5, st0
nop
nop
nop
	fsubr	st5
nop
nop
nop
	fsubr	dword [ecx]
nop
nop
nop
	fsubr	qword [ecx]
nop
nop
nop
	faddp	st5, st0
nop
nop
nop
	faddp	st5
nop
nop
nop
	faddp	
nop
nop
nop
	fdivp	st5, st0
nop
nop
nop
	fdivp	st5
nop
nop
nop
	fdivp	
nop
nop
nop
	fdivrp	st5, st0
nop
nop
nop
	fdivrp	st5
nop
nop
nop
	fdivrp	
nop
nop
nop
	fmulp	st5, st0
nop
nop
nop
	fmulp	st5
nop
nop
nop
	fmulp	
nop
nop
nop
	fsubp	st5, st0
nop
nop
nop
	fsubp	st5
nop
nop
nop
	fsubp	
nop
nop
nop
	fsubrp	st5, st0
nop
nop
nop
	fsubrp	st5
nop
nop
nop
	fsubrp	
nop
nop
nop
	fxch	st5, st0
nop
nop
nop
	fxch	st5
nop
nop
nop
	fxch	
nop
nop
nop
	fiadd	word [ecx]
nop
nop
nop
	fiadd	dword [ecx]
nop
nop
nop
	ficom	word [ecx]
nop
nop
nop
	ficom	dword [ecx]
nop
nop
nop
	ficomp	word [ecx]
nop
nop
nop
	ficomp	dword [ecx]
nop
nop
nop
	fidiv	word [ecx]
nop
nop
nop
	fidiv	dword [ecx]
nop
nop
nop
	fidivr	word [ecx]
nop
nop
nop
	fidivr	dword [ecx]
nop
nop
nop
	fimul	word [ecx]
nop
nop
nop
	fimul	dword [ecx]
nop
nop
nop
	fist	word [ecx]
nop
nop
nop
	fist	dword [ecx]
nop
nop
nop
	fisub	word [ecx]
nop
nop
nop
	fisub	dword [ecx]
nop
nop
nop
	fisubr	word [ecx]
nop
nop
nop
	fisubr	dword [ecx]
nop
nop
nop
	fld	st5
nop
nop
nop
	fld	dword [ecx]
nop
nop
nop
	fld	qword [ecx]
nop
nop
nop
	fld	tword [ecx]
nop
nop
nop
	fbld	[ecx]
nop
nop
nop
	fbld	tword [ecx]
nop
nop
nop
	fbstp	[ecx]
nop
nop
nop
	fbstp	tword [ecx]
nop
nop
nop
	fcom	st0, st5
nop
nop
nop
	fcom	st5
nop
nop
nop
	fcom	dword [ecx]
nop
nop
nop
	fcom	qword [ecx]
nop
nop
nop
	fcomp	st5
nop
nop
nop
	fcomp	dword [ecx]
nop
nop
nop
	fcomp	qword [ecx]
nop
nop
nop
	fst	st5
nop
nop
nop
	fst	dword [ecx]
nop
nop
nop
	fst	qword [ecx]
nop
nop
nop
	fstp	st5
nop
nop
nop
	fstp	dword [ecx]
nop
nop
nop
	fstp	qword [ecx]
nop
nop
nop
	fstp	tword [ecx]
nop
nop
nop
	fstsw	[ecx]
nop
nop
nop
	fstsw	word [ecx]
nop
nop
nop
	fstsw	ax
nop
nop
nop
	fnstsw	[ecx]
nop
nop
nop
	fnstsw	word [ecx]
nop
nop
nop
	fnstsw	ax
nop
nop
nop
