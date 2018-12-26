[bits 64]
	mfence	
nop
nop
nop
	monitor	
nop
nop
nop
	addps	xmm1, xmm10
nop
nop
nop
	addps	xmm1, [rcx]
nop
nop
nop
	addps	xmm10, xmm1
nop
nop
nop
	addps	xmm10, [rcx]
nop
nop
nop
	addps	xmm2, xmm1
nop
nop
nop
	andps	xmm1, xmm10
nop
nop
nop
	andps	xmm1, [rcx]
nop
nop
nop
	andps	xmm10, xmm1
nop
nop
nop
	andps	xmm10, [rcx]
nop
nop
nop
	andps	xmm2, xmm1
nop
nop
nop
	divps	xmm1, xmm10
nop
nop
nop
	divps	xmm1, [rcx]
nop
nop
nop
	divps	xmm10, xmm1
nop
nop
nop
	divps	xmm10, [rcx]
nop
nop
nop
	divps	xmm2, xmm1
nop
nop
nop
	cvtdq2ps	xmm1, xmm10
nop
nop
nop
	cvtdq2ps	xmm1, [rcx]
nop
nop
nop
	cvtdq2ps	xmm10, xmm1
nop
nop
nop
	cvtdq2ps	xmm10, [rcx]
nop
nop
nop
	cvtdq2ps	xmm2, xmm1
nop
nop
nop
	comiss	xmm1, xmm10
nop
nop
nop
	comiss	xmm1, [rcx]
nop
nop
nop
	comiss	xmm10, xmm1
nop
nop
nop
	comiss	xmm10, [rcx]
nop
nop
nop
	comiss	xmm2, xmm1
nop
nop
nop
	comisd	xmm1, xmm10
nop
nop
nop
	comisd	xmm1, [rcx]
nop
nop
nop
	comisd	xmm10, xmm1
nop
nop
nop
	comisd	xmm10, [rcx]
nop
nop
nop
	comisd	xmm2, xmm1
nop
nop
nop
	ucomiss	xmm1, xmm10
nop
nop
nop
	ucomiss	xmm1, [rcx]
nop
nop
nop
	ucomiss	xmm10, xmm1
nop
nop
nop
	ucomiss	xmm10, [rcx]
nop
nop
nop
	ucomiss	xmm2, xmm1
nop
nop
nop
	ucomisd	xmm1, xmm10
nop
nop
nop
	ucomisd	xmm1, [rcx]
nop
nop
nop
	ucomisd	xmm10, xmm1
nop
nop
nop
	ucomisd	xmm10, [rcx]
nop
nop
nop
	ucomisd	xmm2, xmm1
nop
nop
nop
	maxps	xmm1, xmm10
nop
nop
nop
	maxps	xmm1, [rcx]
nop
nop
nop
	maxps	xmm10, xmm1
nop
nop
nop
	maxps	xmm10, [rcx]
nop
nop
nop
	maxps	xmm2, xmm1
nop
nop
nop
	minps	xmm1, xmm10
nop
nop
nop
	minps	xmm1, [rcx]
nop
nop
nop
	minps	xmm10, xmm1
nop
nop
nop
	minps	xmm10, [rcx]
nop
nop
nop
	minps	xmm2, xmm1
nop
nop
nop
	mulps	xmm1, xmm10
nop
nop
nop
	mulps	xmm1, [rcx]
nop
nop
nop
	mulps	xmm10, xmm1
nop
nop
nop
	mulps	xmm10, [rcx]
nop
nop
nop
	mulps	xmm2, xmm1
nop
nop
nop
	orps	xmm1, xmm10
nop
nop
nop
	orps	xmm1, [rcx]
nop
nop
nop
	orps	xmm10, xmm1
nop
nop
nop
	orps	xmm10, [rcx]
nop
nop
nop
	orps	xmm2, xmm1
nop
nop
nop
	sqrtps	xmm1, xmm10
nop
nop
nop
	sqrtps	xmm1, [rcx]
nop
nop
nop
	sqrtps	xmm10, xmm1
nop
nop
nop
	sqrtps	xmm10, [rcx]
nop
nop
nop
	sqrtps	xmm2, xmm1
nop
nop
nop
	subps	xmm1, xmm10
nop
nop
nop
	subps	xmm1, [rcx]
nop
nop
nop
	subps	xmm10, xmm1
nop
nop
nop
	subps	xmm10, [rcx]
nop
nop
nop
	subps	xmm2, xmm1
nop
nop
nop
	unpckhps	xmm1, xmm10
nop
nop
nop
	unpckhps	xmm1, [rcx]
nop
nop
nop
	unpckhps	xmm10, xmm1
nop
nop
nop
	unpckhps	xmm10, [rcx]
nop
nop
nop
	unpckhps	xmm2, xmm1
nop
nop
nop
	unpcklps	xmm1, xmm10
nop
nop
nop
	unpcklps	xmm1, [rcx]
nop
nop
nop
	unpcklps	xmm10, xmm1
nop
nop
nop
	unpcklps	xmm10, [rcx]
nop
nop
nop
	unpcklps	xmm2, xmm1
nop
nop
nop
	xorps	xmm1, xmm10
nop
nop
nop
	xorps	xmm1, [rcx]
nop
nop
nop
	xorps	xmm10, xmm1
nop
nop
nop
	xorps	xmm10, [rcx]
nop
nop
nop
	xorps	xmm2, xmm1
nop
nop
nop
	addpd	xmm1, xmm10
nop
nop
nop
	addpd	xmm1, [rcx]
nop
nop
nop
	addpd	xmm10, xmm1
nop
nop
nop
	addpd	xmm10, [rcx]
nop
nop
nop
	addpd	xmm2, xmm1
nop
nop
nop
	addsd	xmm1, xmm10
nop
nop
nop
	addsd	xmm1, [rcx]
nop
nop
nop
	addsd	xmm10, xmm1
nop
nop
nop
	addsd	xmm10, [rcx]
nop
nop
nop
	addsd	xmm2, xmm1
nop
nop
nop
	addss	xmm1, xmm10
nop
nop
nop
	addss	xmm1, [rcx]
nop
nop
nop
	addss	xmm10, xmm1
nop
nop
nop
	addss	xmm10, [rcx]
nop
nop
nop
	addss	xmm2, xmm1
nop
nop
nop
	addsubpd	xmm1, xmm10
nop
nop
nop
	addsubpd	xmm1, [rcx]
nop
nop
nop
	addsubpd	xmm10, xmm1
nop
nop
nop
	addsubpd	xmm10, [rcx]
nop
nop
nop
	addsubpd	xmm2, xmm1
nop
nop
nop
	addsubps	xmm1, xmm10
nop
nop
nop
	addsubps	xmm1, [rcx]
nop
nop
nop
	addsubps	xmm10, xmm1
nop
nop
nop
	addsubps	xmm10, [rcx]
nop
nop
nop
	addsubps	xmm2, xmm1
nop
nop
nop
	andpd	xmm1, xmm10
nop
nop
nop
	andpd	xmm1, [rcx]
nop
nop
nop
	andpd	xmm10, xmm1
nop
nop
nop
	andpd	xmm10, [rcx]
nop
nop
nop
	andpd	xmm2, xmm1
nop
nop
nop
	divpd	xmm1, xmm10
nop
nop
nop
	divpd	xmm1, [rcx]
nop
nop
nop
	divpd	xmm10, xmm1
nop
nop
nop
	divpd	xmm10, [rcx]
nop
nop
nop
	divpd	xmm2, xmm1
nop
nop
nop
	divsd	xmm1, xmm10
nop
nop
nop
	divsd	xmm1, [rcx]
nop
nop
nop
	divsd	xmm10, xmm1
nop
nop
nop
	divsd	xmm10, [rcx]
nop
nop
nop
	divsd	xmm2, xmm1
nop
nop
nop
	divss	xmm1, xmm10
nop
nop
nop
	divss	xmm1, [rcx]
nop
nop
nop
	divss	xmm10, xmm1
nop
nop
nop
	divss	xmm10, [rcx]
nop
nop
nop
	divss	xmm2, xmm1
nop
nop
nop
	cvtdq2pd	xmm1, xmm10
nop
nop
nop
	cvtdq2pd	xmm1, [rcx]
nop
nop
nop
	cvtdq2pd	xmm10, xmm1
nop
nop
nop
	cvtdq2pd	xmm10, [rcx]
nop
nop
nop
	cvtdq2pd	xmm2, xmm1
nop
nop
nop
	cvtpd2dq	xmm1, xmm10
nop
nop
nop
	cvtpd2dq	xmm1, [rcx]
nop
nop
nop
	cvtpd2dq	xmm10, xmm1
nop
nop
nop
	cvtpd2dq	xmm10, [rcx]
nop
nop
nop
	cvtpd2dq	xmm2, xmm1
nop
nop
nop
	cvtpd2ps	xmm1, xmm10
nop
nop
nop
	cvtpd2ps	xmm1, [rcx]
nop
nop
nop
	cvtpd2ps	xmm10, xmm1
nop
nop
nop
	cvtpd2ps	xmm10, [rcx]
nop
nop
nop
	cvtpd2ps	xmm2, xmm1
nop
nop
nop
	cvtps2dq	xmm1, xmm10
nop
nop
nop
	cvtps2dq	xmm1, [rcx]
nop
nop
nop
	cvtps2dq	xmm10, xmm1
nop
nop
nop
	cvtps2dq	xmm10, [rcx]
nop
nop
nop
	cvtps2dq	xmm2, xmm1
nop
nop
nop
	cvtps2pd	xmm1, xmm10
nop
nop
nop
	cvtps2pd	xmm1, [rcx]
nop
nop
nop
	cvtps2pd	xmm10, xmm1
nop
nop
nop
	cvtps2pd	xmm10, [rcx]
nop
nop
nop
	cvtps2pd	xmm2, xmm1
nop
nop
nop
	cvtsd2ss	xmm1, xmm10
nop
nop
nop
	cvtsd2ss	xmm1, [rcx]
nop
nop
nop
	cvtsd2ss	xmm10, xmm1
nop
nop
nop
	cvtsd2ss	xmm10, [rcx]
nop
nop
nop
	cvtsd2ss	xmm2, xmm1
nop
nop
nop
	cvtss2sd	xmm1, xmm10
nop
nop
nop
	cvtss2sd	xmm1, [rcx]
nop
nop
nop
	cvtss2sd	xmm10, xmm1
nop
nop
nop
	cvtss2sd	xmm10, [rcx]
nop
nop
nop
	cvtss2sd	xmm2, xmm1
nop
nop
nop
	cvttpd2dq	xmm1, xmm10
nop
nop
nop
	cvttpd2dq	xmm1, [rcx]
nop
nop
nop
	cvttpd2dq	xmm10, xmm1
nop
nop
nop
	cvttpd2dq	xmm10, [rcx]
nop
nop
nop
	cvttpd2dq	xmm2, xmm1
nop
nop
nop
	cvttps2dq	xmm1, xmm10
nop
nop
nop
	cvttps2dq	xmm1, [rcx]
nop
nop
nop
	cvttps2dq	xmm10, xmm1
nop
nop
nop
	cvttps2dq	xmm10, [rcx]
nop
nop
nop
	cvttps2dq	xmm2, xmm1
nop
nop
nop
	hsubpd	xmm1, xmm10
nop
nop
nop
	hsubpd	xmm1, [rcx]
nop
nop
nop
	hsubpd	xmm10, xmm1
nop
nop
nop
	hsubpd	xmm10, [rcx]
nop
nop
nop
	hsubpd	xmm2, xmm1
nop
nop
nop
	hsubps	xmm1, xmm10
nop
nop
nop
	hsubps	xmm1, [rcx]
nop
nop
nop
	hsubps	xmm10, xmm1
nop
nop
nop
	hsubps	xmm10, [rcx]
nop
nop
nop
	hsubps	xmm2, xmm1
nop
nop
nop
	maxpd	xmm1, xmm10
nop
nop
nop
	maxpd	xmm1, [rcx]
nop
nop
nop
	maxpd	xmm10, xmm1
nop
nop
nop
	maxpd	xmm10, [rcx]
nop
nop
nop
	maxpd	xmm2, xmm1
nop
nop
nop
	maxsd	xmm1, xmm10
nop
nop
nop
	maxsd	xmm1, [rcx]
nop
nop
nop
	maxsd	xmm10, xmm1
nop
nop
nop
	maxsd	xmm10, [rcx]
nop
nop
nop
	maxsd	xmm2, xmm1
nop
nop
nop
	maxss	xmm1, xmm10
nop
nop
nop
	maxss	xmm1, [rcx]
nop
nop
nop
	maxss	xmm10, xmm1
nop
nop
nop
	maxss	xmm10, [rcx]
nop
nop
nop
	maxss	xmm2, xmm1
nop
nop
nop
	minpd	xmm1, xmm10
nop
nop
nop
	minpd	xmm1, [rcx]
nop
nop
nop
	minpd	xmm10, xmm1
nop
nop
nop
	minpd	xmm10, [rcx]
nop
nop
nop
	minpd	xmm2, xmm1
nop
nop
nop
	minsd	xmm1, xmm10
nop
nop
nop
	minsd	xmm1, [rcx]
nop
nop
nop
	minsd	xmm10, xmm1
nop
nop
nop
	minsd	xmm10, [rcx]
nop
nop
nop
	minsd	xmm2, xmm1
nop
nop
nop
	minss	xmm1, xmm10
nop
nop
nop
	minss	xmm1, [rcx]
nop
nop
nop
	minss	xmm10, xmm1
nop
nop
nop
	minss	xmm10, [rcx]
nop
nop
nop
	minss	xmm2, xmm1
nop
nop
nop
	movddup	xmm1, xmm10
nop
nop
nop
	movddup	xmm1, [rcx]
nop
nop
nop
	movddup	xmm10, xmm1
nop
nop
nop
	movddup	xmm10, [rcx]
nop
nop
nop
	movddup	xmm2, xmm1
nop
nop
nop
	movshdup	xmm1, xmm10
nop
nop
nop
	movshdup	xmm1, [rcx]
nop
nop
nop
	movshdup	xmm10, xmm1
nop
nop
nop
	movshdup	xmm10, [rcx]
nop
nop
nop
	movshdup	xmm2, xmm1
nop
nop
nop
	movsldup	xmm1, xmm10
nop
nop
nop
	movsldup	xmm1, [rcx]
nop
nop
nop
	movsldup	xmm10, xmm1
nop
nop
nop
	movsldup	xmm10, [rcx]
nop
nop
nop
	movsldup	xmm2, xmm1
nop
nop
nop
	mulpd	xmm1, xmm10
nop
nop
nop
	mulpd	xmm1, [rcx]
nop
nop
nop
	mulpd	xmm10, xmm1
nop
nop
nop
	mulpd	xmm10, [rcx]
nop
nop
nop
	mulpd	xmm2, xmm1
nop
nop
nop
	mulsd	xmm1, xmm10
nop
nop
nop
	mulsd	xmm1, [rcx]
nop
nop
nop
	mulsd	xmm10, xmm1
nop
nop
nop
	mulsd	xmm10, [rcx]
nop
nop
nop
	mulsd	xmm2, xmm1
nop
nop
nop
	mulss	xmm1, xmm10
nop
nop
nop
	mulss	xmm1, [rcx]
nop
nop
nop
	mulss	xmm10, xmm1
nop
nop
nop
	mulss	xmm10, [rcx]
nop
nop
nop
	mulss	xmm2, xmm1
nop
nop
nop
	orpd	xmm1, xmm10
nop
nop
nop
	orpd	xmm1, [rcx]
nop
nop
nop
	orpd	xmm10, xmm1
nop
nop
nop
	orpd	xmm10, [rcx]
nop
nop
nop
	orpd	xmm2, xmm1
nop
nop
nop
	punpckhqdq	xmm1, xmm10
nop
nop
nop
	punpckhqdq	xmm1, [rcx]
nop
nop
nop
	punpckhqdq	xmm10, xmm1
nop
nop
nop
	punpckhqdq	xmm10, [rcx]
nop
nop
nop
	punpckhqdq	xmm2, xmm1
nop
nop
nop
	punpcklqdq	xmm1, xmm10
nop
nop
nop
	punpcklqdq	xmm1, [rcx]
nop
nop
nop
	punpcklqdq	xmm10, xmm1
nop
nop
nop
	punpcklqdq	xmm10, [rcx]
nop
nop
nop
	punpcklqdq	xmm2, xmm1
nop
nop
nop
	packusdw	xmm1, xmm10
nop
nop
nop
	packusdw	xmm1, [rcx]
nop
nop
nop
	packusdw	xmm10, xmm1
nop
nop
nop
	packusdw	xmm10, [rcx]
nop
nop
nop
	packusdw	xmm2, xmm1
nop
nop
nop
	rcpps	xmm1, xmm10
nop
nop
nop
	rcpps	xmm1, [rcx]
nop
nop
nop
	rcpps	xmm10, xmm1
nop
nop
nop
	rcpps	xmm10, [rcx]
nop
nop
nop
	rcpps	xmm2, xmm1
nop
nop
nop
	rcpss	xmm1, xmm10
nop
nop
nop
	rcpss	xmm1, [rcx]
nop
nop
nop
	rcpss	xmm10, xmm1
nop
nop
nop
	rcpss	xmm10, [rcx]
nop
nop
nop
	rcpss	xmm2, xmm1
nop
nop
nop
	rsqrtps	xmm1, xmm10
nop
nop
nop
	rsqrtps	xmm1, [rcx]
nop
nop
nop
	rsqrtps	xmm10, xmm1
nop
nop
nop
	rsqrtps	xmm10, [rcx]
nop
nop
nop
	rsqrtps	xmm2, xmm1
nop
nop
nop
	rsqrtss	xmm1, xmm10
nop
nop
nop
	rsqrtss	xmm1, [rcx]
nop
nop
nop
	rsqrtss	xmm10, xmm1
nop
nop
nop
	rsqrtss	xmm10, [rcx]
nop
nop
nop
	rsqrtss	xmm2, xmm1
nop
nop
nop
	sqrtpd	xmm1, xmm10
nop
nop
nop
	sqrtpd	xmm1, [rcx]
nop
nop
nop
	sqrtpd	xmm10, xmm1
nop
nop
nop
	sqrtpd	xmm10, [rcx]
nop
nop
nop
	sqrtpd	xmm2, xmm1
nop
nop
nop
	sqrtsd	xmm1, xmm10
nop
nop
nop
	sqrtsd	xmm1, [rcx]
nop
nop
nop
	sqrtsd	xmm10, xmm1
nop
nop
nop
	sqrtsd	xmm10, [rcx]
nop
nop
nop
	sqrtsd	xmm2, xmm1
nop
nop
nop
	sqrtss	xmm1, xmm10
nop
nop
nop
	sqrtss	xmm1, [rcx]
nop
nop
nop
	sqrtss	xmm10, xmm1
nop
nop
nop
	sqrtss	xmm10, [rcx]
nop
nop
nop
	sqrtss	xmm2, xmm1
nop
nop
nop
	subpd	xmm1, xmm10
nop
nop
nop
	subpd	xmm1, [rcx]
nop
nop
nop
	subpd	xmm10, xmm1
nop
nop
nop
	subpd	xmm10, [rcx]
nop
nop
nop
	subpd	xmm2, xmm1
nop
nop
nop
	subsd	xmm1, xmm10
nop
nop
nop
	subsd	xmm1, [rcx]
nop
nop
nop
	subsd	xmm10, xmm1
nop
nop
nop
	subsd	xmm10, [rcx]
nop
nop
nop
	subsd	xmm2, xmm1
nop
nop
nop
	subss	xmm1, xmm10
nop
nop
nop
	subss	xmm1, [rcx]
nop
nop
nop
	subss	xmm10, xmm1
nop
nop
nop
	subss	xmm10, [rcx]
nop
nop
nop
	subss	xmm2, xmm1
nop
nop
nop
	unpckhpd	xmm1, xmm10
nop
nop
nop
	unpckhpd	xmm1, [rcx]
nop
nop
nop
	unpckhpd	xmm10, xmm1
nop
nop
nop
	unpckhpd	xmm10, [rcx]
nop
nop
nop
	unpckhpd	xmm2, xmm1
nop
nop
nop
	unpcklpd	xmm1, xmm10
nop
nop
nop
	unpcklpd	xmm1, [rcx]
nop
nop
nop
	unpcklpd	xmm10, xmm1
nop
nop
nop
	unpcklpd	xmm10, [rcx]
nop
nop
nop
	unpcklpd	xmm2, xmm1
nop
nop
nop
	xorpd	xmm1, xmm10
nop
nop
nop
	xorpd	xmm1, [rcx]
nop
nop
nop
	xorpd	xmm10, xmm1
nop
nop
nop
	xorpd	xmm10, [rcx]
nop
nop
nop
	xorpd	xmm2, xmm1
nop
nop
nop
	 lddqu	xmm1, [rcx]
nop
nop
nop
	 lddqu	xmm10, [rcx]
nop
nop
nop
	movhpd	xmm1, [rcx]
nop
nop
nop
	movhpd	xmm10, [rcx]
nop
nop
nop
	movhps	xmm1, [rcx]
nop
nop
nop
	movhps	xmm10, [rcx]
nop
nop
nop
	movlpd	xmm1, [rcx]
nop
nop
nop
	movlpd	xmm10, [rcx]
nop
nop
nop
	movlps	xmm1, [rcx]
nop
nop
nop
	movlps	xmm10, [rcx]
nop
nop
nop
	cvtsi2sd	xmm1, dword [rcx]
nop
nop
nop
	cvtsi2sd	xmm1, qword [rcx]
nop
nop
nop
	cvtsi2sd	xmm10, dword [rcx]
nop
nop
nop
	cvtsi2sd	xmm10, qword [rcx]
nop
nop
nop
	cvtsi2ss	xmm1, dword [rcx]
nop
nop
nop
	cvtsi2ss	xmm1, qword [rcx]
nop
nop
nop
	cvtsi2ss	xmm10, dword [rcx]
nop
nop
nop
	cvtsi2ss	xmm10, qword [rcx]
nop
nop
nop
	blendpd	xmm1, xmm10,3
nop
nop
nop
	blendpd	xmm1, [rcx],5
nop
nop
nop
	blendpd	xmm10, xmm1,3
nop
nop
nop
	blendpd	xmm10, [rcx],5
nop
nop
nop
	blendpd	xmm1, xmm3, 2
nop
nop
nop
	blendps	xmm1, xmm10,3
nop
nop
nop
	blendps	xmm1, [rcx],5
nop
nop
nop
	blendps	xmm10, xmm1,3
nop
nop
nop
	blendps	xmm10, [rcx],5
nop
nop
nop
	blendps	xmm1, xmm3, 2
nop
nop
nop
	cmpps	xmm1, xmm10,3
nop
nop
nop
	cmpps	xmm1, [rcx],5
nop
nop
nop
	cmpps	xmm10, xmm1,3
nop
nop
nop
	cmpps	xmm10, [rcx],5
nop
nop
nop
	cmpps	xmm1, xmm3, 2
nop
nop
nop
	cmppd	xmm1, xmm10,3
nop
nop
nop
	cmppd	xmm1, [rcx],5
nop
nop
nop
	cmppd	xmm10, xmm1,3
nop
nop
nop
	cmppd	xmm10, [rcx],5
nop
nop
nop
	cmppd	xmm1, xmm3, 2
nop
nop
nop
	mpsadbw	xmm1, xmm10,3
nop
nop
nop
	mpsadbw	xmm1, [rcx],5
nop
nop
nop
	mpsadbw	xmm10, xmm1,3
nop
nop
nop
	mpsadbw	xmm10, [rcx],5
nop
nop
nop
	mpsadbw	xmm1, xmm3, 2
nop
nop
nop
	pblendw	xmm1, xmm10,3
nop
nop
nop
	pblendw	xmm1, [rcx],5
nop
nop
nop
	pblendw	xmm10, xmm1,3
nop
nop
nop
	pblendw	xmm10, [rcx],5
nop
nop
nop
	pblendw	xmm1, xmm3, 2
nop
nop
nop
	pcmpestri	xmm1, xmm10,3
nop
nop
nop
	pcmpestri	xmm1, [rcx],5
nop
nop
nop
	pcmpestri	xmm10, xmm1,3
nop
nop
nop
	pcmpestri	xmm10, [rcx],5
nop
nop
nop
	pcmpestri	xmm1, xmm3, 2
nop
nop
nop
	pcmpestrm	xmm1, xmm10,3
nop
nop
nop
	pcmpestrm	xmm1, [rcx],5
nop
nop
nop
	pcmpestrm	xmm10, xmm1,3
nop
nop
nop
	pcmpestrm	xmm10, [rcx],5
nop
nop
nop
	pcmpestrm	xmm1, xmm3, 2
nop
nop
nop
	pcmpestri	xmm1, xmm10,3
nop
nop
nop
	pcmpestri	xmm1, [rcx],5
nop
nop
nop
	pcmpestri	xmm10, xmm1,3
nop
nop
nop
	pcmpestri	xmm10, [rcx],5
nop
nop
nop
	pcmpestri	xmm1, xmm3, 2
nop
nop
nop
	pcmpestrm	xmm1, xmm10,3
nop
nop
nop
	pcmpestrm	xmm1, [rcx],5
nop
nop
nop
	pcmpestrm	xmm10, xmm1,3
nop
nop
nop
	pcmpestrm	xmm10, [rcx],5
nop
nop
nop
	pcmpestrm	xmm1, xmm3, 2
nop
nop
nop
	roundpd	xmm1, xmm10,3
nop
nop
nop
	roundpd	xmm1, [rcx],5
nop
nop
nop
	roundpd	xmm10, xmm1,3
nop
nop
nop
	roundpd	xmm10, [rcx],5
nop
nop
nop
	roundpd	xmm1, xmm3, 2
nop
nop
nop
	roundps	xmm1, xmm10,3
nop
nop
nop
	roundps	xmm1, [rcx],5
nop
nop
nop
	roundps	xmm10, xmm1,3
nop
nop
nop
	roundps	xmm10, [rcx],5
nop
nop
nop
	roundps	xmm1, xmm3, 2
nop
nop
nop
	roundsd	xmm1, xmm10,3
nop
nop
nop
	roundsd	xmm1, [rcx],5
nop
nop
nop
	roundsd	xmm10, xmm1,3
nop
nop
nop
	roundsd	xmm10, [rcx],5
nop
nop
nop
	roundsd	xmm1, xmm3, 2
nop
nop
nop
	roundss	xmm1, xmm10,3
nop
nop
nop
	roundss	xmm1, [rcx],5
nop
nop
nop
	roundss	xmm10, xmm1,3
nop
nop
nop
	roundss	xmm10, [rcx],5
nop
nop
nop
	roundss	xmm1, xmm3, 2
nop
nop
nop
	pshufd	xmm1, xmm10,3
nop
nop
nop
	pshufd	xmm1, [rcx],5
nop
nop
nop
	pshufd	xmm10, xmm1,3
nop
nop
nop
	pshufd	xmm10, [rcx],5
nop
nop
nop
	pshufd	xmm1, xmm3, 2
nop
nop
nop
	pshufhw	xmm1, xmm10,3
nop
nop
nop
	pshufhw	xmm1, [rcx],5
nop
nop
nop
	pshufhw	xmm10, xmm1,3
nop
nop
nop
	pshufhw	xmm10, [rcx],5
nop
nop
nop
	pshufhw	xmm1, xmm3, 2
nop
nop
nop
	pshuflw	xmm1, xmm10,3
nop
nop
nop
	pshuflw	xmm1, [rcx],5
nop
nop
nop
	pshuflw	xmm10, xmm1,3
nop
nop
nop
	pshuflw	xmm10, [rcx],5
nop
nop
nop
	pshuflw	xmm1, xmm3, 2
nop
nop
nop
	shufpd	xmm1, xmm10,3
nop
nop
nop
	shufpd	xmm1, [rcx],5
nop
nop
nop
	shufpd	xmm10, xmm1,3
nop
nop
nop
	shufpd	xmm10, [rcx],5
nop
nop
nop
	shufpd	xmm1, xmm3, 2
nop
nop
nop
	shufps	xmm1, xmm10,3
nop
nop
nop
	shufps	xmm1, [rcx],5
nop
nop
nop
	shufps	xmm10, xmm1,3
nop
nop
nop
	shufps	xmm10, [rcx],5
nop
nop
nop
	shufps	xmm1, xmm3, 2
nop
nop
nop
	pslld	xmm1, xmm10
nop
nop
nop
	pslld	xmm1, [rcx]
nop
nop
nop
	pslld	xmm1, 3
nop
nop
nop
	pslld	xmm10, xmm1
nop
nop
nop
	pslld	xmm10, [rcx]
nop
nop
nop
	pslld	xmm10, 3
nop
nop
nop
	pslld	xmm1, xmm2
nop
nop
nop
	pslld	mm1, mm5
nop
nop
nop
	pslld	mm1, [rcx]
nop
nop
nop
	pslld	mm1, 3
nop
nop
nop
	psllq	xmm1, xmm10
nop
nop
nop
	psllq	xmm1, [rcx]
nop
nop
nop
	psllq	xmm1, 3
nop
nop
nop
	psllq	xmm10, xmm1
nop
nop
nop
	psllq	xmm10, [rcx]
nop
nop
nop
	psllq	xmm10, 3
nop
nop
nop
	psllq	xmm1, xmm2
nop
nop
nop
	psllq	mm1, mm5
nop
nop
nop
	psllq	mm1, [rcx]
nop
nop
nop
	psllq	mm1, 3
nop
nop
nop
	psllw	xmm1, xmm10
nop
nop
nop
	psllw	xmm1, [rcx]
nop
nop
nop
	psllw	xmm1, 3
nop
nop
nop
	psllw	xmm10, xmm1
nop
nop
nop
	psllw	xmm10, [rcx]
nop
nop
nop
	psllw	xmm10, 3
nop
nop
nop
	psllw	xmm1, xmm2
nop
nop
nop
	psllw	mm1, mm5
nop
nop
nop
	psllw	mm1, [rcx]
nop
nop
nop
	psllw	mm1, 3
nop
nop
nop
	psrad	xmm1, xmm10
nop
nop
nop
	psrad	xmm1, [rcx]
nop
nop
nop
	psrad	xmm1, 3
nop
nop
nop
	psrad	xmm10, xmm1
nop
nop
nop
	psrad	xmm10, [rcx]
nop
nop
nop
	psrad	xmm10, 3
nop
nop
nop
	psrad	xmm1, xmm2
nop
nop
nop
	psrad	mm1, mm5
nop
nop
nop
	psrad	mm1, [rcx]
nop
nop
nop
	psrad	mm1, 3
nop
nop
nop
	psraw	xmm1, xmm10
nop
nop
nop
	psraw	xmm1, [rcx]
nop
nop
nop
	psraw	xmm1, 3
nop
nop
nop
	psraw	xmm10, xmm1
nop
nop
nop
	psraw	xmm10, [rcx]
nop
nop
nop
	psraw	xmm10, 3
nop
nop
nop
	psraw	xmm1, xmm2
nop
nop
nop
	psraw	mm1, mm5
nop
nop
nop
	psraw	mm1, [rcx]
nop
nop
nop
	psraw	mm1, 3
nop
nop
nop
	psrld	xmm1, xmm10
nop
nop
nop
	psrld	xmm1, [rcx]
nop
nop
nop
	psrld	xmm1, 3
nop
nop
nop
	psrld	xmm10, xmm1
nop
nop
nop
	psrld	xmm10, [rcx]
nop
nop
nop
	psrld	xmm10, 3
nop
nop
nop
	psrld	xmm1, xmm2
nop
nop
nop
	psrld	mm1, mm5
nop
nop
nop
	psrld	mm1, [rcx]
nop
nop
nop
	psrld	mm1, 3
nop
nop
nop
	psrlq	xmm1, xmm10
nop
nop
nop
	psrlq	xmm1, [rcx]
nop
nop
nop
	psrlq	xmm1, 3
nop
nop
nop
	psrlq	xmm10, xmm1
nop
nop
nop
	psrlq	xmm10, [rcx]
nop
nop
nop
	psrlq	xmm10, 3
nop
nop
nop
	psrlq	xmm1, xmm2
nop
nop
nop
	psrlq	mm1, mm5
nop
nop
nop
	psrlq	mm1, [rcx]
nop
nop
nop
	psrlq	mm1, 3
nop
nop
nop
	psrlw	xmm1, xmm10
nop
nop
nop
	psrlw	xmm1, [rcx]
nop
nop
nop
	psrlw	xmm1, 3
nop
nop
nop
	psrlw	xmm10, xmm1
nop
nop
nop
	psrlw	xmm10, [rcx]
nop
nop
nop
	psrlw	xmm10, 3
nop
nop
nop
	psrlw	xmm1, xmm2
nop
nop
nop
	psrlw	mm1, mm5
nop
nop
nop
	psrlw	mm1, [rcx]
nop
nop
nop
	psrlw	mm1, 3
nop
nop
nop
	pslldq	xmm1, 3
nop
nop
nop
	pslldq	xmm10, 3
nop
nop
nop
	psrldq	xmm1, 3
nop
nop
nop
	psrldq	xmm10, 3
nop
nop
nop
	cvtsd2si	ebx,xmm1
nop
nop
nop
	cvtsd2si	ebx,xmm12
nop
nop
nop
	cvtsd2si	rbx,xmm1
nop
nop
nop
	cvtsd2si	rbx,xmm12
nop
nop
nop
	cvtsd2si	r15,xmm1
nop
nop
nop
	cvtsd2si	r15,xmm12
nop
nop
nop
	cvtss2si	ebx,xmm1
nop
nop
nop
	cvtss2si	ebx,xmm12
nop
nop
nop
	cvtss2si	rbx,xmm1
nop
nop
nop
	cvtss2si	rbx,xmm12
nop
nop
nop
	cvtss2si	r15,xmm1
nop
nop
nop
	cvtss2si	r15,xmm12
nop
nop
nop
	cvttsd2si	ebx,xmm1
nop
nop
nop
	cvttsd2si	ebx,xmm12
nop
nop
nop
	cvttsd2si	rbx,xmm1
nop
nop
nop
	cvttsd2si	rbx,xmm12
nop
nop
nop
	cvttsd2si	r15,xmm1
nop
nop
nop
	cvttsd2si	r15,xmm12
nop
nop
nop
	cvttss2si	ebx,xmm1
nop
nop
nop
	cvttss2si	ebx,xmm12
nop
nop
nop
	cvttss2si	rbx,xmm1
nop
nop
nop
	cvttss2si	rbx,xmm12
nop
nop
nop
	cvttss2si	r15,xmm1
nop
nop
nop
	cvttss2si	r15,xmm12
nop
nop
nop
	movmskpd	ebx,xmm1
nop
nop
nop
	movmskpd	ebx,xmm12
nop
nop
nop
	movmskpd	rbx,xmm1
nop
nop
nop
	movmskpd	rbx,xmm12
nop
nop
nop
	movmskpd	r15,xmm1
nop
nop
nop
	movmskpd	r15,xmm12
nop
nop
nop
	movmskps	ebx,xmm1
nop
nop
nop
	movmskps	ebx,xmm12
nop
nop
nop
	movmskps	rbx,xmm1
nop
nop
nop
	movmskps	rbx,xmm12
nop
nop
nop
	movmskps	r15,xmm1
nop
nop
nop
	movmskps	r15,xmm12
nop
nop
nop
	movapd	xmm1, xmm10
nop
nop
nop
	movapd	xmm1, [rcx]
nop
nop
nop
	movapd	xmm10, xmm1
nop
nop
nop
	movapd	xmm10, [rcx]
nop
nop
nop
	movapd	xmm2, xmm1
nop
nop
nop
	movapd	[rcx], xmm1
nop
nop
nop
	movapd	[rcx], xmm10
nop
nop
nop
	movaps	xmm1, xmm10
nop
nop
nop
	movaps	xmm1, [rcx]
nop
nop
nop
	movaps	xmm10, xmm1
nop
nop
nop
	movaps	xmm10, [rcx]
nop
nop
nop
	movaps	xmm2, xmm1
nop
nop
nop
	movaps	[rcx], xmm1
nop
nop
nop
	movaps	[rcx], xmm10
nop
nop
nop
	movdqa	xmm1, xmm10
nop
nop
nop
	movdqa	xmm1, [rcx]
nop
nop
nop
	movdqa	xmm10, xmm1
nop
nop
nop
	movdqa	xmm10, [rcx]
nop
nop
nop
	movdqa	xmm2, xmm1
nop
nop
nop
	movdqa	[rcx], xmm1
nop
nop
nop
	movdqa	[rcx], xmm10
nop
nop
nop
	movdqu	xmm1, xmm10
nop
nop
nop
	movdqu	xmm1, [rcx]
nop
nop
nop
	movdqu	xmm10, xmm1
nop
nop
nop
	movdqu	xmm10, [rcx]
nop
nop
nop
	movdqu	xmm2, xmm1
nop
nop
nop
	movdqu	[rcx], xmm1
nop
nop
nop
	movdqu	[rcx], xmm10
nop
nop
nop
	movss	xmm1, xmm10
nop
nop
nop
	movss	xmm1, [rcx]
nop
nop
nop
	movss	xmm10, xmm1
nop
nop
nop
	movss	xmm10, [rcx]
nop
nop
nop
	movss	xmm2, xmm1
nop
nop
nop
	movss	[rcx], xmm1
nop
nop
nop
	movss	[rcx], xmm10
nop
nop
nop
	movupd	xmm1, xmm10
nop
nop
nop
	movupd	xmm1, [rcx]
nop
nop
nop
	movupd	xmm10, xmm1
nop
nop
nop
	movupd	xmm10, [rcx]
nop
nop
nop
	movupd	xmm2, xmm1
nop
nop
nop
	movupd	[rcx], xmm1
nop
nop
nop
	movupd	[rcx], xmm10
nop
nop
nop
	movups	xmm1, xmm10
nop
nop
nop
	movups	xmm1, [rcx]
nop
nop
nop
	movups	xmm10, xmm1
nop
nop
nop
	movups	xmm10, [rcx]
nop
nop
nop
	movups	xmm2, xmm1
nop
nop
nop
	movups	[rcx], xmm1
nop
nop
nop
	movups	[rcx], xmm10
nop
nop
nop
	pshufw	mm1, mm3,55
nop
nop
nop
	pshufw	mm2, [rcx],45
nop
nop
nop
	cvtpd2pi	mm3,xmm4
nop
nop
nop
	cvtpd2pi	mm3,xmm10
nop
nop
nop
	cvtpd2pi	mm3,[rcx]
nop
nop
nop
	cvtps2pi	mm3,xmm4
nop
nop
nop
	cvtps2pi	mm3,xmm10
nop
nop
nop
	cvtps2pi	mm3,[rcx]
nop
nop
nop
	cvttpd2pi	mm3,xmm4
nop
nop
nop
	cvttpd2pi	mm3,xmm10
nop
nop
nop
	cvttpd2pi	mm3,[rcx]
nop
nop
nop
	cvttps2pi	mm3,xmm4
nop
nop
nop
	cvttps2pi	mm3,xmm10
nop
nop
nop
	cvttps2pi	mm3,[rcx]
nop
nop
nop
	movdq2q	mm3,xmm4
nop
nop
nop
	movdq2q	mm3,xmm10
nop
nop
nop
	packssdw	xmm1, xmm10
nop
nop
nop
	packssdw	xmm1, [rcx]
nop
nop
nop
	packssdw	xmm10, xmm1
nop
nop
nop
	packssdw	xmm10, [rcx]
nop
nop
nop
	packssdw	xmm1, xmm2
nop
nop
nop
	packssdw	mm1, mm5
nop
nop
nop
	packssdw	mm1, [rcx]
nop
nop
nop
	packsswb	xmm1, xmm10
nop
nop
nop
	packsswb	xmm1, [rcx]
nop
nop
nop
	packsswb	xmm10, xmm1
nop
nop
nop
	packsswb	xmm10, [rcx]
nop
nop
nop
	packsswb	xmm1, xmm2
nop
nop
nop
	packsswb	mm1, mm5
nop
nop
nop
	packsswb	mm1, [rcx]
nop
nop
nop
	paddb	xmm1, xmm10
nop
nop
nop
	paddb	xmm1, [rcx]
nop
nop
nop
	paddb	xmm10, xmm1
nop
nop
nop
	paddb	xmm10, [rcx]
nop
nop
nop
	paddb	xmm1, xmm2
nop
nop
nop
	paddb	mm1, mm5
nop
nop
nop
	paddb	mm1, [rcx]
nop
nop
nop
	paddd	xmm1, xmm10
nop
nop
nop
	paddd	xmm1, [rcx]
nop
nop
nop
	paddd	xmm10, xmm1
nop
nop
nop
	paddd	xmm10, [rcx]
nop
nop
nop
	paddd	xmm1, xmm2
nop
nop
nop
	paddd	mm1, mm5
nop
nop
nop
	paddd	mm1, [rcx]
nop
nop
nop
	paddq	xmm1, xmm10
nop
nop
nop
	paddq	xmm1, [rcx]
nop
nop
nop
	paddq	xmm10, xmm1
nop
nop
nop
	paddq	xmm10, [rcx]
nop
nop
nop
	paddq	xmm1, xmm2
nop
nop
nop
	paddq	mm1, mm5
nop
nop
nop
	paddq	mm1, [rcx]
nop
nop
nop
	paddsw	xmm1, xmm10
nop
nop
nop
	paddsw	xmm1, [rcx]
nop
nop
nop
	paddsw	xmm10, xmm1
nop
nop
nop
	paddsw	xmm10, [rcx]
nop
nop
nop
	paddsw	xmm1, xmm2
nop
nop
nop
	paddsw	mm1, mm5
nop
nop
nop
	paddsw	mm1, [rcx]
nop
nop
nop
	paddusb	xmm1, xmm10
nop
nop
nop
	paddusb	xmm1, [rcx]
nop
nop
nop
	paddusb	xmm10, xmm1
nop
nop
nop
	paddusb	xmm10, [rcx]
nop
nop
nop
	paddusb	xmm1, xmm2
nop
nop
nop
	paddusb	mm1, mm5
nop
nop
nop
	paddusb	mm1, [rcx]
nop
nop
nop
	paddusw	xmm1, xmm10
nop
nop
nop
	paddusw	xmm1, [rcx]
nop
nop
nop
	paddusw	xmm10, xmm1
nop
nop
nop
	paddusw	xmm10, [rcx]
nop
nop
nop
	paddusw	xmm1, xmm2
nop
nop
nop
	paddusw	mm1, mm5
nop
nop
nop
	paddusw	mm1, [rcx]
nop
nop
nop
	paddw	xmm1, xmm10
nop
nop
nop
	paddw	xmm1, [rcx]
nop
nop
nop
	paddw	xmm10, xmm1
nop
nop
nop
	paddw	xmm10, [rcx]
nop
nop
nop
	paddw	xmm1, xmm2
nop
nop
nop
	paddw	mm1, mm5
nop
nop
nop
	paddw	mm1, [rcx]
nop
nop
nop
	pand	xmm1, xmm10
nop
nop
nop
	pand	xmm1, [rcx]
nop
nop
nop
	pand	xmm10, xmm1
nop
nop
nop
	pand	xmm10, [rcx]
nop
nop
nop
	pand	xmm1, xmm2
nop
nop
nop
	pand	mm1, mm5
nop
nop
nop
	pand	mm1, [rcx]
nop
nop
nop
	pandn	xmm1, xmm10
nop
nop
nop
	pandn	xmm1, [rcx]
nop
nop
nop
	pandn	xmm10, xmm1
nop
nop
nop
	pandn	xmm10, [rcx]
nop
nop
nop
	pandn	xmm1, xmm2
nop
nop
nop
	pandn	mm1, mm5
nop
nop
nop
	pandn	mm1, [rcx]
nop
nop
nop
	pavgb	xmm1, xmm10
nop
nop
nop
	pavgb	xmm1, [rcx]
nop
nop
nop
	pavgb	xmm10, xmm1
nop
nop
nop
	pavgb	xmm10, [rcx]
nop
nop
nop
	pavgb	xmm1, xmm2
nop
nop
nop
	pavgb	mm1, mm5
nop
nop
nop
	pavgb	mm1, [rcx]
nop
nop
nop
	pavgw	xmm1, xmm10
nop
nop
nop
	pavgw	xmm1, [rcx]
nop
nop
nop
	pavgw	xmm10, xmm1
nop
nop
nop
	pavgw	xmm10, [rcx]
nop
nop
nop
	pavgw	xmm1, xmm2
nop
nop
nop
	pavgw	mm1, mm5
nop
nop
nop
	pavgw	mm1, [rcx]
nop
nop
nop
	pcmpeqb	xmm1, xmm10
nop
nop
nop
	pcmpeqb	xmm1, [rcx]
nop
nop
nop
	pcmpeqb	xmm10, xmm1
nop
nop
nop
	pcmpeqb	xmm10, [rcx]
nop
nop
nop
	pcmpeqb	xmm1, xmm2
nop
nop
nop
	pcmpeqb	mm1, mm5
nop
nop
nop
	pcmpeqb	mm1, [rcx]
nop
nop
nop
	pcmpeqd	xmm1, xmm10
nop
nop
nop
	pcmpeqd	xmm1, [rcx]
nop
nop
nop
	pcmpeqd	xmm10, xmm1
nop
nop
nop
	pcmpeqd	xmm10, [rcx]
nop
nop
nop
	pcmpeqd	xmm1, xmm2
nop
nop
nop
	pcmpeqd	mm1, mm5
nop
nop
nop
	pcmpeqd	mm1, [rcx]
nop
nop
nop
	pcmpeqw	xmm1, xmm10
nop
nop
nop
	pcmpeqw	xmm1, [rcx]
nop
nop
nop
	pcmpeqw	xmm10, xmm1
nop
nop
nop
	pcmpeqw	xmm10, [rcx]
nop
nop
nop
	pcmpeqw	xmm1, xmm2
nop
nop
nop
	pcmpeqw	mm1, mm5
nop
nop
nop
	pcmpeqw	mm1, [rcx]
nop
nop
nop
	pcmpgtb	xmm1, xmm10
nop
nop
nop
	pcmpgtb	xmm1, [rcx]
nop
nop
nop
	pcmpgtb	xmm10, xmm1
nop
nop
nop
	pcmpgtb	xmm10, [rcx]
nop
nop
nop
	pcmpgtb	xmm1, xmm2
nop
nop
nop
	pcmpgtb	mm1, mm5
nop
nop
nop
	pcmpgtb	mm1, [rcx]
nop
nop
nop
	pcmpgtd	xmm1, xmm10
nop
nop
nop
	pcmpgtd	xmm1, [rcx]
nop
nop
nop
	pcmpgtd	xmm10, xmm1
nop
nop
nop
	pcmpgtd	xmm10, [rcx]
nop
nop
nop
	pcmpgtd	xmm1, xmm2
nop
nop
nop
	pcmpgtd	mm1, mm5
nop
nop
nop
	pcmpgtd	mm1, [rcx]
nop
nop
nop
	pcmpgtw	xmm1, xmm10
nop
nop
nop
	pcmpgtw	xmm1, [rcx]
nop
nop
nop
	pcmpgtw	xmm10, xmm1
nop
nop
nop
	pcmpgtw	xmm10, [rcx]
nop
nop
nop
	pcmpgtw	xmm1, xmm2
nop
nop
nop
	pcmpgtw	mm1, mm5
nop
nop
nop
	pcmpgtw	mm1, [rcx]
nop
nop
nop
	pmaddwd	xmm1, xmm10
nop
nop
nop
	pmaddwd	xmm1, [rcx]
nop
nop
nop
	pmaddwd	xmm10, xmm1
nop
nop
nop
	pmaddwd	xmm10, [rcx]
nop
nop
nop
	pmaddwd	xmm1, xmm2
nop
nop
nop
	pmaddwd	mm1, mm5
nop
nop
nop
	pmaddwd	mm1, [rcx]
nop
nop
nop
	pmaxsw	xmm1, xmm10
nop
nop
nop
	pmaxsw	xmm1, [rcx]
nop
nop
nop
	pmaxsw	xmm10, xmm1
nop
nop
nop
	pmaxsw	xmm10, [rcx]
nop
nop
nop
	pmaxsw	xmm1, xmm2
nop
nop
nop
	pmaxsw	mm1, mm5
nop
nop
nop
	pmaxsw	mm1, [rcx]
nop
nop
nop
	pmaxub	xmm1, xmm10
nop
nop
nop
	pmaxub	xmm1, [rcx]
nop
nop
nop
	pmaxub	xmm10, xmm1
nop
nop
nop
	pmaxub	xmm10, [rcx]
nop
nop
nop
	pmaxub	xmm1, xmm2
nop
nop
nop
	pmaxub	mm1, mm5
nop
nop
nop
	pmaxub	mm1, [rcx]
nop
nop
nop
	pminsw	xmm1, xmm10
nop
nop
nop
	pminsw	xmm1, [rcx]
nop
nop
nop
	pminsw	xmm10, xmm1
nop
nop
nop
	pminsw	xmm10, [rcx]
nop
nop
nop
	pminsw	xmm1, xmm2
nop
nop
nop
	pminsw	mm1, mm5
nop
nop
nop
	pminsw	mm1, [rcx]
nop
nop
nop
	pminub	xmm1, xmm10
nop
nop
nop
	pminub	xmm1, [rcx]
nop
nop
nop
	pminub	xmm10, xmm1
nop
nop
nop
	pminub	xmm10, [rcx]
nop
nop
nop
	pminub	xmm1, xmm2
nop
nop
nop
	pminub	mm1, mm5
nop
nop
nop
	pminub	mm1, [rcx]
nop
nop
nop
	pmulhuw	xmm1, xmm10
nop
nop
nop
	pmulhuw	xmm1, [rcx]
nop
nop
nop
	pmulhuw	xmm10, xmm1
nop
nop
nop
	pmulhuw	xmm10, [rcx]
nop
nop
nop
	pmulhuw	xmm1, xmm2
nop
nop
nop
	pmulhuw	mm1, mm5
nop
nop
nop
	pmulhuw	mm1, [rcx]
nop
nop
nop
	pmulhw	xmm1, xmm10
nop
nop
nop
	pmulhw	xmm1, [rcx]
nop
nop
nop
	pmulhw	xmm10, xmm1
nop
nop
nop
	pmulhw	xmm10, [rcx]
nop
nop
nop
	pmulhw	xmm1, xmm2
nop
nop
nop
	pmulhw	mm1, mm5
nop
nop
nop
	pmulhw	mm1, [rcx]
nop
nop
nop
	pmullw	xmm1, xmm10
nop
nop
nop
	pmullw	xmm1, [rcx]
nop
nop
nop
	pmullw	xmm10, xmm1
nop
nop
nop
	pmullw	xmm10, [rcx]
nop
nop
nop
	pmullw	xmm1, xmm2
nop
nop
nop
	pmullw	mm1, mm5
nop
nop
nop
	pmullw	mm1, [rcx]
nop
nop
nop
	pmuludq	xmm1, xmm10
nop
nop
nop
	pmuludq	xmm1, [rcx]
nop
nop
nop
	pmuludq	xmm10, xmm1
nop
nop
nop
	pmuludq	xmm10, [rcx]
nop
nop
nop
	pmuludq	xmm1, xmm2
nop
nop
nop
	pmuludq	mm1, mm5
nop
nop
nop
	pmuludq	mm1, [rcx]
nop
nop
nop
	psadbw	xmm1, xmm10
nop
nop
nop
	psadbw	xmm1, [rcx]
nop
nop
nop
	psadbw	xmm10, xmm1
nop
nop
nop
	psadbw	xmm10, [rcx]
nop
nop
nop
	psadbw	xmm1, xmm2
nop
nop
nop
	psadbw	mm1, mm5
nop
nop
nop
	psadbw	mm1, [rcx]
nop
nop
nop
	psubb	xmm1, xmm10
nop
nop
nop
	psubb	xmm1, [rcx]
nop
nop
nop
	psubb	xmm10, xmm1
nop
nop
nop
	psubb	xmm10, [rcx]
nop
nop
nop
	psubb	xmm1, xmm2
nop
nop
nop
	psubb	mm1, mm5
nop
nop
nop
	psubb	mm1, [rcx]
nop
nop
nop
	psubd	xmm1, xmm10
nop
nop
nop
	psubd	xmm1, [rcx]
nop
nop
nop
	psubd	xmm10, xmm1
nop
nop
nop
	psubd	xmm10, [rcx]
nop
nop
nop
	psubd	xmm1, xmm2
nop
nop
nop
	psubd	mm1, mm5
nop
nop
nop
	psubd	mm1, [rcx]
nop
nop
nop
	psubq	xmm1, xmm10
nop
nop
nop
	psubq	xmm1, [rcx]
nop
nop
nop
	psubq	xmm10, xmm1
nop
nop
nop
	psubq	xmm10, [rcx]
nop
nop
nop
	psubq	xmm1, xmm2
nop
nop
nop
	psubq	mm1, mm5
nop
nop
nop
	psubq	mm1, [rcx]
nop
nop
nop
	psubsb	xmm1, xmm10
nop
nop
nop
	psubsb	xmm1, [rcx]
nop
nop
nop
	psubsb	xmm10, xmm1
nop
nop
nop
	psubsb	xmm10, [rcx]
nop
nop
nop
	psubsb	xmm1, xmm2
nop
nop
nop
	psubsb	mm1, mm5
nop
nop
nop
	psubsb	mm1, [rcx]
nop
nop
nop
	psubsw	xmm1, xmm10
nop
nop
nop
	psubsw	xmm1, [rcx]
nop
nop
nop
	psubsw	xmm10, xmm1
nop
nop
nop
	psubsw	xmm10, [rcx]
nop
nop
nop
	psubsw	xmm1, xmm2
nop
nop
nop
	psubsw	mm1, mm5
nop
nop
nop
	psubsw	mm1, [rcx]
nop
nop
nop
	psubusb	xmm1, xmm10
nop
nop
nop
	psubusb	xmm1, [rcx]
nop
nop
nop
	psubusb	xmm10, xmm1
nop
nop
nop
	psubusb	xmm10, [rcx]
nop
nop
nop
	psubusb	xmm1, xmm2
nop
nop
nop
	psubusb	mm1, mm5
nop
nop
nop
	psubusb	mm1, [rcx]
nop
nop
nop
	psubusw	xmm1, xmm10
nop
nop
nop
	psubusw	xmm1, [rcx]
nop
nop
nop
	psubusw	xmm10, xmm1
nop
nop
nop
	psubusw	xmm10, [rcx]
nop
nop
nop
	psubusw	xmm1, xmm2
nop
nop
nop
	psubusw	mm1, mm5
nop
nop
nop
	psubusw	mm1, [rcx]
nop
nop
nop
	punpckhbw	xmm1, xmm10
nop
nop
nop
	punpckhbw	xmm1, [rcx]
nop
nop
nop
	punpckhbw	xmm10, xmm1
nop
nop
nop
	punpckhbw	xmm10, [rcx]
nop
nop
nop
	punpckhbw	xmm1, xmm2
nop
nop
nop
	punpckhbw	mm1, mm5
nop
nop
nop
	punpckhbw	mm1, [rcx]
nop
nop
nop
	punpckhdq	xmm1, xmm10
nop
nop
nop
	punpckhdq	xmm1, [rcx]
nop
nop
nop
	punpckhdq	xmm10, xmm1
nop
nop
nop
	punpckhdq	xmm10, [rcx]
nop
nop
nop
	punpckhdq	xmm1, xmm2
nop
nop
nop
	punpckhdq	mm1, mm5
nop
nop
nop
	punpckhdq	mm1, [rcx]
nop
nop
nop
	punpckhwd	xmm1, xmm10
nop
nop
nop
	punpckhwd	xmm1, [rcx]
nop
nop
nop
	punpckhwd	xmm10, xmm1
nop
nop
nop
	punpckhwd	xmm10, [rcx]
nop
nop
nop
	punpckhwd	xmm1, xmm2
nop
nop
nop
	punpckhwd	mm1, mm5
nop
nop
nop
	punpckhwd	mm1, [rcx]
nop
nop
nop
	punpcklbw	xmm1, xmm10
nop
nop
nop
	punpcklbw	xmm1, [rcx]
nop
nop
nop
	punpcklbw	xmm10, xmm1
nop
nop
nop
	punpcklbw	xmm10, [rcx]
nop
nop
nop
	punpcklbw	xmm1, xmm2
nop
nop
nop
	punpcklbw	mm1, mm5
nop
nop
nop
	punpcklbw	mm1, [rcx]
nop
nop
nop
	punpckldq	xmm1, xmm10
nop
nop
nop
	punpckldq	xmm1, [rcx]
nop
nop
nop
	punpckldq	xmm10, xmm1
nop
nop
nop
	punpckldq	xmm10, [rcx]
nop
nop
nop
	punpckldq	xmm1, xmm2
nop
nop
nop
	punpckldq	mm1, mm5
nop
nop
nop
	punpckldq	mm1, [rcx]
nop
nop
nop
	punpcklwd	xmm1, xmm10
nop
nop
nop
	punpcklwd	xmm1, [rcx]
nop
nop
nop
	punpcklwd	xmm10, xmm1
nop
nop
nop
	punpcklwd	xmm10, [rcx]
nop
nop
nop
	punpcklwd	xmm1, xmm2
nop
nop
nop
	punpcklwd	mm1, mm5
nop
nop
nop
	punpcklwd	mm1, [rcx]
nop
nop
nop
	pxor	xmm1, xmm10
nop
nop
nop
	pxor	xmm1, [rcx]
nop
nop
nop
	pxor	xmm10, xmm1
nop
nop
nop
	pxor	xmm10, [rcx]
nop
nop
nop
	pxor	xmm1, xmm2
nop
nop
nop
	pxor	mm1, mm5
nop
nop
nop
	pxor	mm1, [rcx]
nop
nop
nop
	movntdq	[rcx], xmm2
nop
nop
nop
	movntdq	[rcx], xmm10
nop
nop
nop
	movntpd	[rcx], xmm2
nop
nop
nop
	movntpd	[rcx], xmm10
nop
nop
nop
	movnti	[rcx], ebx
nop
nop
nop
	movnti	[rcx], rbx
nop
nop
nop
	movntq	[rcx], mm2
nop
nop
nop
	movntq	[rcx], mm2
nop
nop
nop
	cvtpi2ps	xmm4, mm2
nop
nop
nop
	cvtpi2ps	xmm10, mm2
nop
nop
nop
	cvtpi2ps	xmm4, [r15]
nop
nop
nop
	cvtpi2ps	xmm10, [rcx]
nop
nop
nop
	cvtpi2pd	xmm4, mm2
nop
nop
nop
	cvtpi2pd	xmm10, mm2
nop
nop
nop
	cvtpi2pd	xmm4, [r15]
nop
nop
nop
	cvtpi2pd	xmm10, [rcx]
nop
nop
nop
	movq2dq	xmm4, mm2
nop
nop
nop
	movq2dq	xmm10, mm2
nop
nop
nop
	movd	mm3, [rcx]
nop
nop
nop
	movd	xmm1, [rcx]
nop
nop
nop
	movd	xmm10, [rcx]
nop
nop
nop
	movd	[rcx], mm3
nop
nop
nop
	movd	[rcx],xmm2
nop
nop
nop
	movd	[rcx],xmm10
nop
nop
nop
	movd	mm3, ebx
nop
nop
nop
	movd	xmm1, ebx
nop
nop
nop
	movd	xmm10, ebx
nop
nop
nop
	movd	ebx, mm3
nop
nop
nop
	movd	ebx,xmm2
nop
nop
nop
	movd	ebx,xmm10
nop
nop
nop
	movq	mm3, [rcx]
nop
nop
nop
	movq	xmm1, [rcx]
nop
nop
nop
	movq	xmm10, [rcx]
nop
nop
nop
	movq	[rcx], mm3
nop
nop
nop
	movq	[rcx],xmm2
nop
nop
nop
	movq	[rcx],xmm10
nop
nop
nop
	movq	mm3, rbx
nop
nop
nop
	movq	xmm1, rbx
nop
nop
nop
	movq	xmm10, rbx
nop
nop
nop
	movq	rbx, mm3
nop
nop
nop
	movq	r15,xmm2
nop
nop
nop
	movq	rbx,xmm10
nop
nop
nop
	pextrb	[rcx],xmm1,55
nop
nop
nop
	pextrb	[rcx],xmm10,55
nop
nop
nop
	pextrb	ebx,xmm1,55
nop
nop
nop
	pextrb	ebx,xmm10,55
nop
nop
nop
	pextrd	[rcx],xmm1,55
nop
nop
nop
	pextrd	[rcx],xmm10,55
nop
nop
nop
	pextrd	ebx,xmm1,55
nop
nop
nop
	pextrd	ebx,xmm10,55
nop
nop
nop
	pextrw	ebx, xmm1,55
nop
nop
nop
	pextrw	[ecx],xmm8,55
nop
nop
nop
	pextrw	ebx, xmm10,55
nop
nop
nop
	pextrw	ebx,mm3,55
nop
nop
nop
	pextrw	rbx, xmm10,55
nop
nop
nop
	pextrq	[rcx],xmm1,55
nop
nop
nop
	pextrq	[rcx],xmm10,55
nop
nop
nop
	pextrq	rbx,xmm1,55
nop
nop
nop
	pextrq	r15,xmm10,55
nop
nop
nop
	pinsrb	xmm1,[rcx],55
nop
nop
nop
	pinsrb	xmm10,[rcx],55
nop
nop
nop
	pinsrb	xmm1,ebx,55
nop
nop
nop
	pinsrb	xmm6,r12d,55
nop
nop
nop
	pinsrd	xmm1,[rcx],55
nop
nop
nop
	pinsrd	xmm10,[rcx],55
nop
nop
nop
	pinsrd	xmm1,ebx,55
nop
nop
nop
	pinsrd	xmm10,r12d,55
nop
nop
nop
	pinsrw	xmm1,ebx,55
nop
nop
nop
	pinsrw	xmm8,[ecx],55
nop
nop
nop
	pinsrw	xmm10,ebx,55
nop
nop
nop
	pinsrw	mm3,ebx,55
nop
nop
nop
	pinsrw	mm3, [ecx],55
nop
nop
nop
	pinsrw	xmm10,rbx,55
nop
nop
nop
	pinsrq	xmm1,[rcx],55
nop
nop
nop
	pinsrq	xmm10,[rcx],55
nop
nop
nop
	pinsrq	xmm1,rbx,55
nop
nop
nop
	pinsrq	xmm10,r12,55
nop
nop
nop
	movhlps	xmm1,xmm10
nop
nop
nop
	movhlps	xmm10,xmm1
nop
nop
nop
	movhlps	xmm3, xmm6
nop
nop
nop
	movlhps	xmm1,xmm10
nop
nop
nop
	movlhps	xmm10,xmm1
nop
nop
nop
	movlhps	xmm3, xmm6
nop
nop
nop
	palignr	xmm1,xmm10,55
nop
nop
nop
	palignr	xmm8,[ecx],55
nop
nop
nop
	palignr	xmm10,xmm3,55
nop
nop
nop
	palignr	mm3,mm4,55
nop
nop
nop
	palignr	mm3, [ecx],55
nop
nop
nop
	maskmovq	mm1,mm7
nop
nop
nop
	maskmovdqu	xmm1,xmm10
nop
nop
nop
	maskmovdqu	xmm10,xmm1
nop
nop
nop
	maskmovdqu	xmm3, xmm6
nop
nop
nop
