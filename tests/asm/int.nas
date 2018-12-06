[bits 16]
	aaa	
nop
nop
nop
	aad	
nop
nop
nop
	aam	
nop
nop
nop
	aas	
nop
nop
nop
	daa	
nop
nop
nop
	das	
nop
nop
nop
	into	
nop
nop
nop
	popa	
nop
nop
nop
	popaw	
nop
nop
nop
	popad	
nop
nop
nop
	pusha	
nop
nop
nop
	pushaw	
nop
nop
nop
	pushad	
nop
nop
nop
	popfw	
nop
nop
nop
	popfd	
nop
nop
nop
	pushfw	
nop
nop
nop
	pushfd	
nop
nop
nop
	cbw	
nop
nop
nop
	cdq	
nop
nop
nop
	clc	
nop
nop
nop
	cld	
nop
nop
nop
	cli	
nop
nop
nop
	clts	
nop
nop
nop
	cmc	
nop
nop
nop
	cpuid	
nop
nop
nop
	cwd	
nop
nop
nop
	cwde	
nop
nop
nop
	hlt	
nop
nop
nop
	icebp	
nop
nop
nop
	int1	
nop
nop
nop
	int3	
nop
nop
nop
	invd	
nop
nop
nop
	iret	
nop
nop
nop
	iretw	
nop
nop
nop
	iretd	
nop
nop
nop
	lfence	
nop
nop
nop
	popf	
nop
nop
nop
	pushf	
nop
nop
nop
	rsm	
nop
nop
nop
	lahf	
nop
nop
nop
	sahf	
nop
nop
nop
	sfence	
nop
nop
nop
	stc	
nop
nop
nop
	std	
nop
nop
nop
	sti	
nop
nop
nop
	syscall	
nop
nop
nop
	sysenter	
nop
nop
nop
	sysexit	
nop
nop
nop
	sysret	
nop
nop
nop
	ud2	
nop
nop
nop
	wait	
nop
nop
nop
	wbinvd	
nop
nop
nop
	wrmsr	
nop
nop
nop
	xsetbv	
nop
nop
nop
	adc	al,5
nop
nop
nop
	adc	ax,0x1234
nop
nop
nop
	adc	eax,0x12345678
nop
nop
nop
	adc	al,byte 5
nop
nop
nop
	adc	ax,word 0x1234
nop
nop
nop
	adc	eax,dword 0x12345678
nop
nop
nop
	adc	bl,[ecx]
nop
nop
nop
	adc	bx,[ecx]
nop
nop
nop
	adc	ebx,[ecx]
nop
nop
nop
	adc	bl,byte [ecx]
nop
nop
nop
	adc	bx,word [ecx]
nop
nop
nop
	adc	ebx,dword [ecx]
nop
nop
nop
	adc	[ecx],bl
nop
nop
nop
	adc	[ecx],bx
nop
nop
nop
	adc	[ecx],ebx
nop
nop
nop
	adc	byte [ecx],bl
nop
nop
nop
	adc	word [ecx],bx
nop
nop
nop
	adc	dword [ecx],ebx
nop
nop
nop
	adc	byte [ecx],44
nop
nop
nop
	adc	word [ecx],44
nop
nop
nop
	adc	dword [ecx],44
nop
nop
nop
	adc	byte [ecx],byte 44
nop
nop
nop
	adc	word [ecx],byte 44
nop
nop
nop
	adc	dword [ecx],byte 44
nop
nop
nop
	adc	byte [ecx],byte 44
nop
nop
nop
	adc	word [ecx],word 44
nop
nop
nop
	adc	dword [ecx],dword 44
nop
nop
nop
	adc	bl,44
nop
nop
nop
	adc	bx,44
nop
nop
nop
	adc	ebx,44
nop
nop
nop
	adc	bl,byte 44
nop
nop
nop
	adc	bx,byte 44
nop
nop
nop
	adc	ebx,byte 44
nop
nop
nop
	adc	bl,byte 44
nop
nop
nop
	adc	bx,word 44
nop
nop
nop
	adc	ebx,dword 44
nop
nop
nop
	add	al,5
nop
nop
nop
	add	ax,0x1234
nop
nop
nop
	add	eax,0x12345678
nop
nop
nop
	add	al,byte 5
nop
nop
nop
	add	ax,word 0x1234
nop
nop
nop
	add	eax,dword 0x12345678
nop
nop
nop
	add	bl,[ecx]
nop
nop
nop
	add	bx,[ecx]
nop
nop
nop
	add	ebx,[ecx]
nop
nop
nop
	add	bl,byte [ecx]
nop
nop
nop
	add	bx,word [ecx]
nop
nop
nop
	add	ebx,dword [ecx]
nop
nop
nop
	add	[ecx],bl
nop
nop
nop
	add	[ecx],bx
nop
nop
nop
	add	[ecx],ebx
nop
nop
nop
	add	byte [ecx],bl
nop
nop
nop
	add	word [ecx],bx
nop
nop
nop
	add	dword [ecx],ebx
nop
nop
nop
	add	byte [ecx],44
nop
nop
nop
	add	word [ecx],44
nop
nop
nop
	add	dword [ecx],44
nop
nop
nop
	add	byte [ecx],byte 44
nop
nop
nop
	add	word [ecx],byte 44
nop
nop
nop
	add	dword [ecx],byte 44
nop
nop
nop
	add	byte [ecx],byte 44
nop
nop
nop
	add	word [ecx],word 44
nop
nop
nop
	add	dword [ecx],dword 44
nop
nop
nop
	add	bl,44
nop
nop
nop
	add	bx,44
nop
nop
nop
	add	ebx,44
nop
nop
nop
	add	bl,byte 44
nop
nop
nop
	add	bx,byte 44
nop
nop
nop
	add	ebx,byte 44
nop
nop
nop
	add	bl,byte 44
nop
nop
nop
	add	bx,word 44
nop
nop
nop
	add	ebx,dword 44
nop
nop
nop
	and	al,5
nop
nop
nop
	and	ax,0x1234
nop
nop
nop
	and	eax,0x12345678
nop
nop
nop
	and	al,byte 5
nop
nop
nop
	and	ax,word 0x1234
nop
nop
nop
	and	eax,dword 0x12345678
nop
nop
nop
	and	bl,[ecx]
nop
nop
nop
	and	bx,[ecx]
nop
nop
nop
	and	ebx,[ecx]
nop
nop
nop
	and	bl,byte [ecx]
nop
nop
nop
	and	bx,word [ecx]
nop
nop
nop
	and	ebx,dword [ecx]
nop
nop
nop
	and	[ecx],bl
nop
nop
nop
	and	[ecx],bx
nop
nop
nop
	and	[ecx],ebx
nop
nop
nop
	and	byte [ecx],bl
nop
nop
nop
	and	word [ecx],bx
nop
nop
nop
	and	dword [ecx],ebx
nop
nop
nop
	and	byte [ecx],44
nop
nop
nop
	and	word [ecx],44
nop
nop
nop
	and	dword [ecx],44
nop
nop
nop
	and	byte [ecx],byte 44
nop
nop
nop
	and	word [ecx],byte 44
nop
nop
nop
	and	dword [ecx],byte 44
nop
nop
nop
	and	byte [ecx],byte 44
nop
nop
nop
	and	word [ecx],word 44
nop
nop
nop
	and	dword [ecx],dword 44
nop
nop
nop
	and	bl,44
nop
nop
nop
	and	bx,44
nop
nop
nop
	and	ebx,44
nop
nop
nop
	and	bl,byte 44
nop
nop
nop
	and	bx,byte 44
nop
nop
nop
	and	ebx,byte 44
nop
nop
nop
	and	bl,byte 44
nop
nop
nop
	and	bx,word 44
nop
nop
nop
	and	ebx,dword 44
nop
nop
nop
	cmp	al,5
nop
nop
nop
	cmp	ax,0x1234
nop
nop
nop
	cmp	eax,0x12345678
nop
nop
nop
	cmp	al,byte 5
nop
nop
nop
	cmp	ax,word 0x1234
nop
nop
nop
	cmp	eax,dword 0x12345678
nop
nop
nop
	cmp	bl,[ecx]
nop
nop
nop
	cmp	bx,[ecx]
nop
nop
nop
	cmp	ebx,[ecx]
nop
nop
nop
	cmp	bl,byte [ecx]
nop
nop
nop
	cmp	bx,word [ecx]
nop
nop
nop
	cmp	ebx,dword [ecx]
nop
nop
nop
	cmp	[ecx],bl
nop
nop
nop
	cmp	[ecx],bx
nop
nop
nop
	cmp	[ecx],ebx
nop
nop
nop
	cmp	byte [ecx],bl
nop
nop
nop
	cmp	word [ecx],bx
nop
nop
nop
	cmp	dword [ecx],ebx
nop
nop
nop
	cmp	byte [ecx],44
nop
nop
nop
	cmp	word [ecx],44
nop
nop
nop
	cmp	dword [ecx],44
nop
nop
nop
	cmp	byte [ecx],byte 44
nop
nop
nop
	cmp	word [ecx],byte 44
nop
nop
nop
	cmp	dword [ecx],byte 44
nop
nop
nop
	cmp	byte [ecx],byte 44
nop
nop
nop
	cmp	word [ecx],word 44
nop
nop
nop
	cmp	dword [ecx],dword 44
nop
nop
nop
	cmp	bl,44
nop
nop
nop
	cmp	bx,44
nop
nop
nop
	cmp	ebx,44
nop
nop
nop
	cmp	bl,byte 44
nop
nop
nop
	cmp	bx,byte 44
nop
nop
nop
	cmp	ebx,byte 44
nop
nop
nop
	cmp	bl,byte 44
nop
nop
nop
	cmp	bx,word 44
nop
nop
nop
	cmp	ebx,dword 44
nop
nop
nop
	or	al,5
nop
nop
nop
	or	ax,0x1234
nop
nop
nop
	or	eax,0x12345678
nop
nop
nop
	or	al,byte 5
nop
nop
nop
	or	ax,word 0x1234
nop
nop
nop
	or	eax,dword 0x12345678
nop
nop
nop
	or	bl,[ecx]
nop
nop
nop
	or	bx,[ecx]
nop
nop
nop
	or	ebx,[ecx]
nop
nop
nop
	or	bl,byte [ecx]
nop
nop
nop
	or	bx,word [ecx]
nop
nop
nop
	or	ebx,dword [ecx]
nop
nop
nop
	or	[ecx],bl
nop
nop
nop
	or	[ecx],bx
nop
nop
nop
	or	[ecx],ebx
nop
nop
nop
	or	byte [ecx],bl
nop
nop
nop
	or	word [ecx],bx
nop
nop
nop
	or	dword [ecx],ebx
nop
nop
nop
	or	byte [ecx],44
nop
nop
nop
	or	word [ecx],44
nop
nop
nop
	or	dword [ecx],44
nop
nop
nop
	or	byte [ecx],byte 44
nop
nop
nop
	or	word [ecx],byte 44
nop
nop
nop
	or	dword [ecx],byte 44
nop
nop
nop
	or	byte [ecx],byte 44
nop
nop
nop
	or	word [ecx],word 44
nop
nop
nop
	or	dword [ecx],dword 44
nop
nop
nop
	or	bl,44
nop
nop
nop
	or	bx,44
nop
nop
nop
	or	ebx,44
nop
nop
nop
	or	bl,byte 44
nop
nop
nop
	or	bx,byte 44
nop
nop
nop
	or	ebx,byte 44
nop
nop
nop
	or	bl,byte 44
nop
nop
nop
	or	bx,word 44
nop
nop
nop
	or	ebx,dword 44
nop
nop
nop
	sbb	al,5
nop
nop
nop
	sbb	ax,0x1234
nop
nop
nop
	sbb	eax,0x12345678
nop
nop
nop
	sbb	al,byte 5
nop
nop
nop
	sbb	ax,word 0x1234
nop
nop
nop
	sbb	eax,dword 0x12345678
nop
nop
nop
	sbb	bl,[ecx]
nop
nop
nop
	sbb	bx,[ecx]
nop
nop
nop
	sbb	ebx,[ecx]
nop
nop
nop
	sbb	bl,byte [ecx]
nop
nop
nop
	sbb	bx,word [ecx]
nop
nop
nop
	sbb	ebx,dword [ecx]
nop
nop
nop
	sbb	[ecx],bl
nop
nop
nop
	sbb	[ecx],bx
nop
nop
nop
	sbb	[ecx],ebx
nop
nop
nop
	sbb	byte [ecx],bl
nop
nop
nop
	sbb	word [ecx],bx
nop
nop
nop
	sbb	dword [ecx],ebx
nop
nop
nop
	sbb	byte [ecx],44
nop
nop
nop
	sbb	word [ecx],44
nop
nop
nop
	sbb	dword [ecx],44
nop
nop
nop
	sbb	byte [ecx],byte 44
nop
nop
nop
	sbb	word [ecx],byte 44
nop
nop
nop
	sbb	dword [ecx],byte 44
nop
nop
nop
	sbb	byte [ecx],byte 44
nop
nop
nop
	sbb	word [ecx],word 44
nop
nop
nop
	sbb	dword [ecx],dword 44
nop
nop
nop
	sbb	bl,44
nop
nop
nop
	sbb	bx,44
nop
nop
nop
	sbb	ebx,44
nop
nop
nop
	sbb	bl,byte 44
nop
nop
nop
	sbb	bx,byte 44
nop
nop
nop
	sbb	ebx,byte 44
nop
nop
nop
	sbb	bl,byte 44
nop
nop
nop
	sbb	bx,word 44
nop
nop
nop
	sbb	ebx,dword 44
nop
nop
nop
	sub	al,5
nop
nop
nop
	sub	ax,0x1234
nop
nop
nop
	sub	eax,0x12345678
nop
nop
nop
	sub	al,byte 5
nop
nop
nop
	sub	ax,word 0x1234
nop
nop
nop
	sub	eax,dword 0x12345678
nop
nop
nop
	sub	bl,[ecx]
nop
nop
nop
	sub	bx,[ecx]
nop
nop
nop
	sub	ebx,[ecx]
nop
nop
nop
	sub	bl,byte [ecx]
nop
nop
nop
	sub	bx,word [ecx]
nop
nop
nop
	sub	ebx,dword [ecx]
nop
nop
nop
	sub	[ecx],bl
nop
nop
nop
	sub	[ecx],bx
nop
nop
nop
	sub	[ecx],ebx
nop
nop
nop
	sub	byte [ecx],bl
nop
nop
nop
	sub	word [ecx],bx
nop
nop
nop
	sub	dword [ecx],ebx
nop
nop
nop
	sub	byte [ecx],44
nop
nop
nop
	sub	word [ecx],44
nop
nop
nop
	sub	dword [ecx],44
nop
nop
nop
	sub	byte [ecx],byte 44
nop
nop
nop
	sub	word [ecx],byte 44
nop
nop
nop
	sub	dword [ecx],byte 44
nop
nop
nop
	sub	byte [ecx],byte 44
nop
nop
nop
	sub	word [ecx],word 44
nop
nop
nop
	sub	dword [ecx],dword 44
nop
nop
nop
	sub	bl,44
nop
nop
nop
	sub	bx,44
nop
nop
nop
	sub	ebx,44
nop
nop
nop
	sub	bl,byte 44
nop
nop
nop
	sub	bx,byte 44
nop
nop
nop
	sub	ebx,byte 44
nop
nop
nop
	sub	bl,byte 44
nop
nop
nop
	sub	bx,word 44
nop
nop
nop
	sub	ebx,dword 44
nop
nop
nop
	xor	al,5
nop
nop
nop
	xor	ax,0x1234
nop
nop
nop
	xor	eax,0x12345678
nop
nop
nop
	xor	al,byte 5
nop
nop
nop
	xor	ax,word 0x1234
nop
nop
nop
	xor	eax,dword 0x12345678
nop
nop
nop
	xor	bl,[ecx]
nop
nop
nop
	xor	bx,[ecx]
nop
nop
nop
	xor	ebx,[ecx]
nop
nop
nop
	xor	bl,byte [ecx]
nop
nop
nop
	xor	bx,word [ecx]
nop
nop
nop
	xor	ebx,dword [ecx]
nop
nop
nop
	xor	[ecx],bl
nop
nop
nop
	xor	[ecx],bx
nop
nop
nop
	xor	[ecx],ebx
nop
nop
nop
	xor	byte [ecx],bl
nop
nop
nop
	xor	word [ecx],bx
nop
nop
nop
	xor	dword [ecx],ebx
nop
nop
nop
	xor	byte [ecx],44
nop
nop
nop
	xor	word [ecx],44
nop
nop
nop
	xor	dword [ecx],44
nop
nop
nop
	xor	byte [ecx],byte 44
nop
nop
nop
	xor	word [ecx],byte 44
nop
nop
nop
	xor	dword [ecx],byte 44
nop
nop
nop
	xor	byte [ecx],byte 44
nop
nop
nop
	xor	word [ecx],word 44
nop
nop
nop
	xor	dword [ecx],dword 44
nop
nop
nop
	xor	bl,44
nop
nop
nop
	xor	bx,44
nop
nop
nop
	xor	ebx,44
nop
nop
nop
	xor	bl,byte 44
nop
nop
nop
	xor	bx,byte 44
nop
nop
nop
	xor	ebx,byte 44
nop
nop
nop
	xor	bl,byte 44
nop
nop
nop
	xor	bx,word 44
nop
nop
nop
	xor	ebx,dword 44
nop
nop
nop
	bt	word [ecx],44
nop
nop
nop
	bt	dword [ecx],44
nop
nop
nop
	bt	word [ecx],bx
nop
nop
nop
	bt	dword [ecx],ebx
nop
nop
nop
	btc	word [ecx],44
nop
nop
nop
	btc	dword [ecx],44
nop
nop
nop
	btc	word [ecx],bx
nop
nop
nop
	btc	dword [ecx],ebx
nop
nop
nop
	btr	word [ecx],44
nop
nop
nop
	btr	dword [ecx],44
nop
nop
nop
	btr	word [ecx],bx
nop
nop
nop
	btr	dword [ecx],ebx
nop
nop
nop
	bts	word [ecx],44
nop
nop
nop
	bts	dword [ecx],44
nop
nop
nop
	bts	word [ecx],bx
nop
nop
nop
	bts	dword [ecx],ebx
nop
nop
nop
	inc	bl
nop
nop
nop
	inc	bx
nop
nop
nop
	inc	ebx
nop
nop
nop
	inc	byte [ecx]
nop
nop
nop
	inc	word [ecx]
nop
nop
nop
	inc	dword [ecx]
nop
nop
nop
	dec	bl
nop
nop
nop
	dec	bx
nop
nop
nop
	dec	ebx
nop
nop
nop
	dec	byte [ecx]
nop
nop
nop
	dec	word [ecx]
nop
nop
nop
	dec	dword [ecx]
nop
nop
nop
	neg	bl
nop
nop
nop
	neg	bx
nop
nop
nop
	neg	ebx
nop
nop
nop
	neg	byte [ecx]
nop
nop
nop
	neg	word [ecx]
nop
nop
nop
	neg	dword [ecx]
nop
nop
nop
	not	bl
nop
nop
nop
	not	bx
nop
nop
nop
	not	ebx
nop
nop
nop
	not	byte [ecx]
nop
nop
nop
	not	word [ecx]
nop
nop
nop
	not	dword [ecx]
nop
nop
nop
	div	bl
nop
nop
nop
	div	bx
nop
nop
nop
	div	ebx
nop
nop
nop
	div	byte [ecx]
nop
nop
nop
	div	word [ecx]
nop
nop
nop
	div	dword [ecx]
nop
nop
nop
	idiv	bl
nop
nop
nop
	idiv	bx
nop
nop
nop
	idiv	ebx
nop
nop
nop
	idiv	byte [ecx]
nop
nop
nop
	idiv	word [ecx]
nop
nop
nop
	idiv	dword [ecx]
nop
nop
nop
	mul	bl
nop
nop
nop
	mul	bx
nop
nop
nop
	mul	ebx
nop
nop
nop
	mul	byte [ecx]
nop
nop
nop
	mul	word [ecx]
nop
nop
nop
	mul	dword [ecx]
nop
nop
nop
	invlpg	[ecx]
nop
nop
nop
	ltr	[ecx]
nop
nop
nop
	prefetchnta	[ecx]
nop
nop
nop
	prefetcht0	[ecx]
nop
nop
nop
	prefetcht1	[ecx]
nop
nop
nop
	prefetcht2	[ecx]
nop
nop
nop
	xrstor	[ecx]
nop
nop
nop
	xsave	[ecx]
nop
nop
nop
	lmsw	bx
nop
nop
nop
	smsw	bx
nop
nop
nop
	verr	bx
nop
nop
nop
	verw	bx
nop
nop
nop
	str	bx
nop
nop
nop
	str	ebx
nop
nop
nop
	bswap	ebx
nop
nop
nop
	pop	bx
nop
nop
nop
	pop	ebx
nop
nop
nop
	pop	word [ecx]
nop
nop
nop
	pop	dword [ecx]
nop
nop
nop
	pop	fs
nop
nop
nop
	pop	gs
nop
nop
nop
	pop	ds
nop
nop
nop
	pop	es
nop
nop
nop
	pop	ss
nop
nop
nop
	pop	cs
nop
nop
nop
	push	bx
nop
nop
nop
	push	ebx
nop
nop
nop
	push	word [ecx]
nop
nop
nop
	push	dword [ecx]
nop
nop
nop
	push	fs
nop
nop
nop
	push	gs
nop
nop
nop
	push	ds
nop
nop
nop
	push	es
nop
nop
nop
	push	ss
nop
nop
nop
	push	cs
nop
nop
nop
	push	44
nop
nop
nop
	push	-3
nop
nop
nop
	push	0x1234
nop
nop
nop
	push	byte 44
nop
nop
nop
	push	word 1234
nop
nop
nop
	push	dword 0x12345678
nop
nop
nop
	call	[fs:0x4444]
nop
nop
nop
	call	 lbl0
	nop
	nop
lbl0: nop
	call	bx
nop
nop
nop
	call	ebx
nop
nop
nop
	call	word [ecx]
nop
nop
nop
	call	dword [ecx]
nop
nop
nop
	call	far word [0x4444]
nop
nop
nop
	call	far word [ecx]
nop
nop
nop
	call	far dword [0x4444]
nop
nop
nop
	call	far dword [ecx]
nop
nop
nop
	jmp	[fs:0x4444]
nop
nop
nop
	jmp	 lbl1
	nop
	nop
lbl1: nop
	jmp	bx
nop
nop
nop
	jmp	ebx
nop
nop
nop
	jmp	word [ecx]
nop
nop
nop
	jmp	dword [ecx]
nop
nop
nop
	jmp	far word [0x4444]
nop
nop
nop
	jmp	far word [ecx]
nop
nop
nop
	jmp	far dword [0x4444]
nop
nop
nop
	jmp	far dword [ecx]
nop
nop
nop
	jmp	short lbl2
	nop
	nop
lbl2: nop
	jmp	near lbl3
	nop
	nop
lbl3: nop
	ja	short lbl4
	nop
	nop
lbl4: nop
	ja	near lbl5
	nop
	nop
lbl5: nop
	ja	 lbl6
	nop
	nop
lbl6: nop
	jae	short lbl7
	nop
	nop
lbl7: nop
	jae	near lbl8
	nop
	nop
lbl8: nop
	jae	 lbl9
	nop
	nop
lbl9: nop
	jb	short lbl10
	nop
	nop
lbl10: nop
	jb	near lbl11
	nop
	nop
lbl11: nop
	jb	 lbl12
	nop
	nop
lbl12: nop
	jbe	short lbl13
	nop
	nop
lbl13: nop
	jbe	near lbl14
	nop
	nop
lbl14: nop
	jbe	 lbl15
	nop
	nop
lbl15: nop
	jc	short lbl16
	nop
	nop
lbl16: nop
	jc	near lbl17
	nop
	nop
lbl17: nop
	jc	 lbl18
	nop
	nop
lbl18: nop
	je	short lbl19
	nop
	nop
lbl19: nop
	je	near lbl20
	nop
	nop
lbl20: nop
	je	 lbl21
	nop
	nop
lbl21: nop
	jg	short lbl22
	nop
	nop
lbl22: nop
	jg	near lbl23
	nop
	nop
lbl23: nop
	jg	 lbl24
	nop
	nop
lbl24: nop
	jge	short lbl25
	nop
	nop
lbl25: nop
	jge	near lbl26
	nop
	nop
lbl26: nop
	jge	 lbl27
	nop
	nop
lbl27: nop
	jl	short lbl28
	nop
	nop
lbl28: nop
	jl	near lbl29
	nop
	nop
lbl29: nop
	jl	 lbl30
	nop
	nop
lbl30: nop
	jle	short lbl31
	nop
	nop
lbl31: nop
	jle	near lbl32
	nop
	nop
lbl32: nop
	jle	 lbl33
	nop
	nop
lbl33: nop
	jna	short lbl34
	nop
	nop
lbl34: nop
	jna	near lbl35
	nop
	nop
lbl35: nop
	jna	 lbl36
	nop
	nop
lbl36: nop
	jnae	short lbl37
	nop
	nop
lbl37: nop
	jnae	near lbl38
	nop
	nop
lbl38: nop
	jnae	 lbl39
	nop
	nop
lbl39: nop
	jnb	short lbl40
	nop
	nop
lbl40: nop
	jnb	near lbl41
	nop
	nop
lbl41: nop
	jnb	 lbl42
	nop
	nop
lbl42: nop
	jnbe	short lbl43
	nop
	nop
lbl43: nop
	jnbe	near lbl44
	nop
	nop
lbl44: nop
	jnbe	 lbl45
	nop
	nop
lbl45: nop
	jnc	short lbl46
	nop
	nop
lbl46: nop
	jnc	near lbl47
	nop
	nop
lbl47: nop
	jnc	 lbl48
	nop
	nop
lbl48: nop
	jne	short lbl49
	nop
	nop
lbl49: nop
	jne	near lbl50
	nop
	nop
lbl50: nop
	jne	 lbl51
	nop
	nop
lbl51: nop
	jng	short lbl52
	nop
	nop
lbl52: nop
	jng	near lbl53
	nop
	nop
lbl53: nop
	jng	 lbl54
	nop
	nop
lbl54: nop
	jnge	short lbl55
	nop
	nop
lbl55: nop
	jnge	near lbl56
	nop
	nop
lbl56: nop
	jnge	 lbl57
	nop
	nop
lbl57: nop
	jnl	short lbl58
	nop
	nop
lbl58: nop
	jnl	near lbl59
	nop
	nop
lbl59: nop
	jnl	 lbl60
	nop
	nop
lbl60: nop
	jnle	short lbl61
	nop
	nop
lbl61: nop
	jnle	near lbl62
	nop
	nop
lbl62: nop
	jnle	 lbl63
	nop
	nop
lbl63: nop
	jno	short lbl64
	nop
	nop
lbl64: nop
	jno	near lbl65
	nop
	nop
lbl65: nop
	jno	 lbl66
	nop
	nop
lbl66: nop
	jnp	short lbl67
	nop
	nop
lbl67: nop
	jnp	near lbl68
	nop
	nop
lbl68: nop
	jnp	 lbl69
	nop
	nop
lbl69: nop
	jns	short lbl70
	nop
	nop
lbl70: nop
	jns	near lbl71
	nop
	nop
lbl71: nop
	jns	 lbl72
	nop
	nop
lbl72: nop
	jnz	short lbl73
	nop
	nop
lbl73: nop
	jnz	near lbl74
	nop
	nop
lbl74: nop
	jnz	 lbl75
	nop
	nop
lbl75: nop
	jo	short lbl76
	nop
	nop
lbl76: nop
	jo	near lbl77
	nop
	nop
lbl77: nop
	jo	 lbl78
	nop
	nop
lbl78: nop
	jp	short lbl79
	nop
	nop
lbl79: nop
	jp	near lbl80
	nop
	nop
lbl80: nop
	jp	 lbl81
	nop
	nop
lbl81: nop
	jpe	short lbl82
	nop
	nop
lbl82: nop
	jpe	near lbl83
	nop
	nop
lbl83: nop
	jpe	 lbl84
	nop
	nop
lbl84: nop
	jpo	short lbl85
	nop
	nop
lbl85: nop
	jpo	near lbl86
	nop
	nop
lbl86: nop
	jpo	 lbl87
	nop
	nop
lbl87: nop
	js	short lbl88
	nop
	nop
lbl88: nop
	js	near lbl89
	nop
	nop
lbl89: nop
	js	 lbl90
	nop
	nop
lbl90: nop
	jz	short lbl91
	nop
	nop
lbl91: nop
	jz	near lbl92
	nop
	nop
lbl92: nop
	jz	 lbl93
	nop
	nop
lbl93: nop
	jcxz	 lbl94
	nop
	nop
lbl94: nop
	jecxz	 lbl95
	nop
	nop
lbl95: nop
	loop	 lbl96
	nop
	nop
lbl96: nop
	loopne	 lbl97
	nop
	nop
lbl97: nop
	loope	 lbl98
	nop
	nop
lbl98: nop
	loopz	 lbl99
	nop
	nop
lbl99: nop
	loopnz	 lbl100
	nop
	nop
lbl100: nop
	seta	[ecx]
nop
nop
nop
	seta	byte [ecx]
nop
nop
nop
	setae	[ecx]
nop
nop
nop
	setae	byte [ecx]
nop
nop
nop
	setb	[ecx]
nop
nop
nop
	setb	byte [ecx]
nop
nop
nop
	setbe	[ecx]
nop
nop
nop
	setbe	byte [ecx]
nop
nop
nop
	setc	[ecx]
nop
nop
nop
	setc	byte [ecx]
nop
nop
nop
	sete	[ecx]
nop
nop
nop
	sete	byte [ecx]
nop
nop
nop
	setg	[ecx]
nop
nop
nop
	setg	byte [ecx]
nop
nop
nop
	setge	[ecx]
nop
nop
nop
	setge	byte [ecx]
nop
nop
nop
	setl	[ecx]
nop
nop
nop
	setl	byte [ecx]
nop
nop
nop
	setle	[ecx]
nop
nop
nop
	setle	byte [ecx]
nop
nop
nop
	setna	[ecx]
nop
nop
nop
	setna	byte [ecx]
nop
nop
nop
	setnae	[ecx]
nop
nop
nop
	setnae	byte [ecx]
nop
nop
nop
	setnb	[ecx]
nop
nop
nop
	setnb	byte [ecx]
nop
nop
nop
	setnbe	[ecx]
nop
nop
nop
	setnbe	byte [ecx]
nop
nop
nop
	setnc	[ecx]
nop
nop
nop
	setnc	byte [ecx]
nop
nop
nop
	setne	[ecx]
nop
nop
nop
	setne	byte [ecx]
nop
nop
nop
	setng	[ecx]
nop
nop
nop
	setng	byte [ecx]
nop
nop
nop
	setnge	[ecx]
nop
nop
nop
	setnge	byte [ecx]
nop
nop
nop
	setnl	[ecx]
nop
nop
nop
	setnl	byte [ecx]
nop
nop
nop
	setnle	[ecx]
nop
nop
nop
	setnle	byte [ecx]
nop
nop
nop
	setno	[ecx]
nop
nop
nop
	setno	byte [ecx]
nop
nop
nop
	setnp	[ecx]
nop
nop
nop
	setnp	byte [ecx]
nop
nop
nop
	setns	[ecx]
nop
nop
nop
	setns	byte [ecx]
nop
nop
nop
	setnz	[ecx]
nop
nop
nop
	setnz	byte [ecx]
nop
nop
nop
	seto	[ecx]
nop
nop
nop
	seto	byte [ecx]
nop
nop
nop
	setp	[ecx]
nop
nop
nop
	setp	byte [ecx]
nop
nop
nop
	setpe	[ecx]
nop
nop
nop
	setpe	byte [ecx]
nop
nop
nop
	setpo	[ecx]
nop
nop
nop
	setpo	byte [ecx]
nop
nop
nop
	sets	[ecx]
nop
nop
nop
	sets	byte [ecx]
nop
nop
nop
	setz	[ecx]
nop
nop
nop
	setz	byte [ecx]
nop
nop
nop
	cmova	bx,[ecx]
nop
nop
nop
	cmova	ebx,[ecx]
nop
nop
nop
	cmova	bx,word [ecx]
nop
nop
nop
	cmova	ebx,dword [ecx]
nop
nop
nop
	cmovae	bx,[ecx]
nop
nop
nop
	cmovae	ebx,[ecx]
nop
nop
nop
	cmovae	bx,word [ecx]
nop
nop
nop
	cmovae	ebx,dword [ecx]
nop
nop
nop
	cmovb	bx,[ecx]
nop
nop
nop
	cmovb	ebx,[ecx]
nop
nop
nop
	cmovb	bx,word [ecx]
nop
nop
nop
	cmovb	ebx,dword [ecx]
nop
nop
nop
	cmovbe	bx,[ecx]
nop
nop
nop
	cmovbe	ebx,[ecx]
nop
nop
nop
	cmovbe	bx,word [ecx]
nop
nop
nop
	cmovbe	ebx,dword [ecx]
nop
nop
nop
	cmovc	bx,[ecx]
nop
nop
nop
	cmovc	ebx,[ecx]
nop
nop
nop
	cmovc	bx,word [ecx]
nop
nop
nop
	cmovc	ebx,dword [ecx]
nop
nop
nop
	cmove	bx,[ecx]
nop
nop
nop
	cmove	ebx,[ecx]
nop
nop
nop
	cmove	bx,word [ecx]
nop
nop
nop
	cmove	ebx,dword [ecx]
nop
nop
nop
	cmovg	bx,[ecx]
nop
nop
nop
	cmovg	ebx,[ecx]
nop
nop
nop
	cmovg	bx,word [ecx]
nop
nop
nop
	cmovg	ebx,dword [ecx]
nop
nop
nop
	cmovge	bx,[ecx]
nop
nop
nop
	cmovge	ebx,[ecx]
nop
nop
nop
	cmovge	bx,word [ecx]
nop
nop
nop
	cmovge	ebx,dword [ecx]
nop
nop
nop
	cmovl	bx,[ecx]
nop
nop
nop
	cmovl	ebx,[ecx]
nop
nop
nop
	cmovl	bx,word [ecx]
nop
nop
nop
	cmovl	ebx,dword [ecx]
nop
nop
nop
	cmovle	bx,[ecx]
nop
nop
nop
	cmovle	ebx,[ecx]
nop
nop
nop
	cmovle	bx,word [ecx]
nop
nop
nop
	cmovle	ebx,dword [ecx]
nop
nop
nop
	cmovna	bx,[ecx]
nop
nop
nop
	cmovna	ebx,[ecx]
nop
nop
nop
	cmovna	bx,word [ecx]
nop
nop
nop
	cmovna	ebx,dword [ecx]
nop
nop
nop
	cmovnae	bx,[ecx]
nop
nop
nop
	cmovnae	ebx,[ecx]
nop
nop
nop
	cmovnae	bx,word [ecx]
nop
nop
nop
	cmovnae	ebx,dword [ecx]
nop
nop
nop
	cmovnb	bx,[ecx]
nop
nop
nop
	cmovnb	ebx,[ecx]
nop
nop
nop
	cmovnb	bx,word [ecx]
nop
nop
nop
	cmovnb	ebx,dword [ecx]
nop
nop
nop
	cmovnbe	bx,[ecx]
nop
nop
nop
	cmovnbe	ebx,[ecx]
nop
nop
nop
	cmovnbe	bx,word [ecx]
nop
nop
nop
	cmovnbe	ebx,dword [ecx]
nop
nop
nop
	cmovnc	bx,[ecx]
nop
nop
nop
	cmovnc	ebx,[ecx]
nop
nop
nop
	cmovnc	bx,word [ecx]
nop
nop
nop
	cmovnc	ebx,dword [ecx]
nop
nop
nop
	cmovne	bx,[ecx]
nop
nop
nop
	cmovne	ebx,[ecx]
nop
nop
nop
	cmovne	bx,word [ecx]
nop
nop
nop
	cmovne	ebx,dword [ecx]
nop
nop
nop
	cmovng	bx,[ecx]
nop
nop
nop
	cmovng	ebx,[ecx]
nop
nop
nop
	cmovng	bx,word [ecx]
nop
nop
nop
	cmovng	ebx,dword [ecx]
nop
nop
nop
	cmovnge	bx,[ecx]
nop
nop
nop
	cmovnge	ebx,[ecx]
nop
nop
nop
	cmovnge	bx,word [ecx]
nop
nop
nop
	cmovnge	ebx,dword [ecx]
nop
nop
nop
	cmovnl	bx,[ecx]
nop
nop
nop
	cmovnl	ebx,[ecx]
nop
nop
nop
	cmovnl	bx,word [ecx]
nop
nop
nop
	cmovnl	ebx,dword [ecx]
nop
nop
nop
	cmovnle	bx,[ecx]
nop
nop
nop
	cmovnle	ebx,[ecx]
nop
nop
nop
	cmovnle	bx,word [ecx]
nop
nop
nop
	cmovnle	ebx,dword [ecx]
nop
nop
nop
	cmovno	bx,[ecx]
nop
nop
nop
	cmovno	ebx,[ecx]
nop
nop
nop
	cmovno	bx,word [ecx]
nop
nop
nop
	cmovno	ebx,dword [ecx]
nop
nop
nop
	cmovnp	bx,[ecx]
nop
nop
nop
	cmovnp	ebx,[ecx]
nop
nop
nop
	cmovnp	bx,word [ecx]
nop
nop
nop
	cmovnp	ebx,dword [ecx]
nop
nop
nop
	cmovns	bx,[ecx]
nop
nop
nop
	cmovns	ebx,[ecx]
nop
nop
nop
	cmovns	bx,word [ecx]
nop
nop
nop
	cmovns	ebx,dword [ecx]
nop
nop
nop
	cmovnz	bx,[ecx]
nop
nop
nop
	cmovnz	ebx,[ecx]
nop
nop
nop
	cmovnz	bx,word [ecx]
nop
nop
nop
	cmovnz	ebx,dword [ecx]
nop
nop
nop
	cmovo	bx,[ecx]
nop
nop
nop
	cmovo	ebx,[ecx]
nop
nop
nop
	cmovo	bx,word [ecx]
nop
nop
nop
	cmovo	ebx,dword [ecx]
nop
nop
nop
	cmovp	bx,[ecx]
nop
nop
nop
	cmovp	ebx,[ecx]
nop
nop
nop
	cmovp	bx,word [ecx]
nop
nop
nop
	cmovp	ebx,dword [ecx]
nop
nop
nop
	cmovpe	bx,[ecx]
nop
nop
nop
	cmovpe	ebx,[ecx]
nop
nop
nop
	cmovpe	bx,word [ecx]
nop
nop
nop
	cmovpe	ebx,dword [ecx]
nop
nop
nop
	cmovpo	bx,[ecx]
nop
nop
nop
	cmovpo	ebx,[ecx]
nop
nop
nop
	cmovpo	bx,word [ecx]
nop
nop
nop
	cmovpo	ebx,dword [ecx]
nop
nop
nop
	cmovs	bx,[ecx]
nop
nop
nop
	cmovs	ebx,[ecx]
nop
nop
nop
	cmovs	bx,word [ecx]
nop
nop
nop
	cmovs	ebx,dword [ecx]
nop
nop
nop
	cmovz	bx,[ecx]
nop
nop
nop
	cmovz	ebx,[ecx]
nop
nop
nop
	cmovz	bx,word [ecx]
nop
nop
nop
	cmovz	ebx,dword [ecx]
nop
nop
nop
	movsx	bx,cl
nop
nop
nop
	movsx	ebx,al
nop
nop
nop
	movsx	ebx,cx
nop
nop
nop
	movsx	bx,byte [ecx]
nop
nop
nop
	movsx	ebx,byte [ecx]
nop
nop
nop
	movsx	ebx,word [ecx]
nop
nop
nop
	movzx	bx,cl
nop
nop
nop
	movzx	ebx,al
nop
nop
nop
	movzx	ebx,cx
nop
nop
nop
	movzx	bx,byte [ecx]
nop
nop
nop
	movzx	ebx,byte [ecx]
nop
nop
nop
	movzx	ebx,word [ecx]
nop
nop
nop
	rcl	bl,1
nop
nop
nop
	rcl	bx,1
nop
nop
nop
	rcl	ebx,1
nop
nop
nop
	rcl	byte [ecx],1
nop
nop
nop
	rcl	word [ecx],1
nop
nop
nop
	rcl	dword [ecx],1
nop
nop
nop
	rcl	bl,3
nop
nop
nop
	rcl	bx,3
nop
nop
nop
	rcl	ebx,3
nop
nop
nop
	rcl	byte [ecx],3
nop
nop
nop
	rcl	word [ecx],3
nop
nop
nop
	rcl	dword [ecx],3
nop
nop
nop
	rcl	bl,cl
nop
nop
nop
	rcl	bx,cl
nop
nop
nop
	rcl	ebx,cl
nop
nop
nop
	rcl	byte [ecx],cl
nop
nop
nop
	rcl	word [ecx],cl
nop
nop
nop
	rcl	dword [ecx],cl
nop
nop
nop
	rcr	bl,1
nop
nop
nop
	rcr	bx,1
nop
nop
nop
	rcr	ebx,1
nop
nop
nop
	rcr	byte [ecx],1
nop
nop
nop
	rcr	word [ecx],1
nop
nop
nop
	rcr	dword [ecx],1
nop
nop
nop
	rcr	bl,3
nop
nop
nop
	rcr	bx,3
nop
nop
nop
	rcr	ebx,3
nop
nop
nop
	rcr	byte [ecx],3
nop
nop
nop
	rcr	word [ecx],3
nop
nop
nop
	rcr	dword [ecx],3
nop
nop
nop
	rcr	bl,cl
nop
nop
nop
	rcr	bx,cl
nop
nop
nop
	rcr	ebx,cl
nop
nop
nop
	rcr	byte [ecx],cl
nop
nop
nop
	rcr	word [ecx],cl
nop
nop
nop
	rcr	dword [ecx],cl
nop
nop
nop
	rol	bl,1
nop
nop
nop
	rol	bx,1
nop
nop
nop
	rol	ebx,1
nop
nop
nop
	rol	byte [ecx],1
nop
nop
nop
	rol	word [ecx],1
nop
nop
nop
	rol	dword [ecx],1
nop
nop
nop
	rol	bl,3
nop
nop
nop
	rol	bx,3
nop
nop
nop
	rol	ebx,3
nop
nop
nop
	rol	byte [ecx],3
nop
nop
nop
	rol	word [ecx],3
nop
nop
nop
	rol	dword [ecx],3
nop
nop
nop
	rol	bl,cl
nop
nop
nop
	rol	bx,cl
nop
nop
nop
	rol	ebx,cl
nop
nop
nop
	rol	byte [ecx],cl
nop
nop
nop
	rol	word [ecx],cl
nop
nop
nop
	rol	dword [ecx],cl
nop
nop
nop
	ror	bl,1
nop
nop
nop
	ror	bx,1
nop
nop
nop
	ror	ebx,1
nop
nop
nop
	ror	byte [ecx],1
nop
nop
nop
	ror	word [ecx],1
nop
nop
nop
	ror	dword [ecx],1
nop
nop
nop
	ror	bl,3
nop
nop
nop
	ror	bx,3
nop
nop
nop
	ror	ebx,3
nop
nop
nop
	ror	byte [ecx],3
nop
nop
nop
	ror	word [ecx],3
nop
nop
nop
	ror	dword [ecx],3
nop
nop
nop
	ror	bl,cl
nop
nop
nop
	ror	bx,cl
nop
nop
nop
	ror	ebx,cl
nop
nop
nop
	ror	byte [ecx],cl
nop
nop
nop
	ror	word [ecx],cl
nop
nop
nop
	ror	dword [ecx],cl
nop
nop
nop
	sal	bl,1
nop
nop
nop
	sal	bx,1
nop
nop
nop
	sal	ebx,1
nop
nop
nop
	sal	byte [ecx],1
nop
nop
nop
	sal	word [ecx],1
nop
nop
nop
	sal	dword [ecx],1
nop
nop
nop
	sal	bl,3
nop
nop
nop
	sal	bx,3
nop
nop
nop
	sal	ebx,3
nop
nop
nop
	sal	byte [ecx],3
nop
nop
nop
	sal	word [ecx],3
nop
nop
nop
	sal	dword [ecx],3
nop
nop
nop
	sal	bl,cl
nop
nop
nop
	sal	bx,cl
nop
nop
nop
	sal	ebx,cl
nop
nop
nop
	sal	byte [ecx],cl
nop
nop
nop
	sal	word [ecx],cl
nop
nop
nop
	sal	dword [ecx],cl
nop
nop
nop
	sar	bl,1
nop
nop
nop
	sar	bx,1
nop
nop
nop
	sar	ebx,1
nop
nop
nop
	sar	byte [ecx],1
nop
nop
nop
	sar	word [ecx],1
nop
nop
nop
	sar	dword [ecx],1
nop
nop
nop
	sar	bl,3
nop
nop
nop
	sar	bx,3
nop
nop
nop
	sar	ebx,3
nop
nop
nop
	sar	byte [ecx],3
nop
nop
nop
	sar	word [ecx],3
nop
nop
nop
	sar	dword [ecx],3
nop
nop
nop
	sar	bl,cl
nop
nop
nop
	sar	bx,cl
nop
nop
nop
	sar	ebx,cl
nop
nop
nop
	sar	byte [ecx],cl
nop
nop
nop
	sar	word [ecx],cl
nop
nop
nop
	sar	dword [ecx],cl
nop
nop
nop
	shl	bl,1
nop
nop
nop
	shl	bx,1
nop
nop
nop
	shl	ebx,1
nop
nop
nop
	shl	byte [ecx],1
nop
nop
nop
	shl	word [ecx],1
nop
nop
nop
	shl	dword [ecx],1
nop
nop
nop
	shl	bl,3
nop
nop
nop
	shl	bx,3
nop
nop
nop
	shl	ebx,3
nop
nop
nop
	shl	byte [ecx],3
nop
nop
nop
	shl	word [ecx],3
nop
nop
nop
	shl	dword [ecx],3
nop
nop
nop
	shl	bl,cl
nop
nop
nop
	shl	bx,cl
nop
nop
nop
	shl	ebx,cl
nop
nop
nop
	shl	byte [ecx],cl
nop
nop
nop
	shl	word [ecx],cl
nop
nop
nop
	shl	dword [ecx],cl
nop
nop
nop
	shr	bl,1
nop
nop
nop
	shr	bx,1
nop
nop
nop
	shr	ebx,1
nop
nop
nop
	shr	byte [ecx],1
nop
nop
nop
	shr	word [ecx],1
nop
nop
nop
	shr	dword [ecx],1
nop
nop
nop
	shr	bl,3
nop
nop
nop
	shr	bx,3
nop
nop
nop
	shr	ebx,3
nop
nop
nop
	shr	byte [ecx],3
nop
nop
nop
	shr	word [ecx],3
nop
nop
nop
	shr	dword [ecx],3
nop
nop
nop
	shr	bl,cl
nop
nop
nop
	shr	bx,cl
nop
nop
nop
	shr	ebx,cl
nop
nop
nop
	shr	byte [ecx],cl
nop
nop
nop
	shr	word [ecx],cl
nop
nop
nop
	shr	dword [ecx],cl
nop
nop
nop
	shld	word [ecx],bx,1
nop
nop
nop
	shld	dword [ecx],ebx,1
nop
nop
nop
	shld	word [ecx],bx,cl
nop
nop
nop
	shld	dword [ecx],ebx,cl
nop
nop
nop
	shrd	word [ecx],bx,1
nop
nop
nop
	shrd	dword [ecx],ebx,1
nop
nop
nop
	shrd	word [ecx],bx,cl
nop
nop
nop
	shrd	dword [ecx],ebx,cl
nop
nop
nop
	bound	bx,[ecx]
nop
nop
nop
	bound	ebx,[ecx]
nop
nop
nop
	lds	bx,[ecx]
nop
nop
nop
	lds	ebx,[ecx]
nop
nop
nop
	les	bx,[ecx]
nop
nop
nop
	les	ebx,[ecx]
nop
nop
nop
	lss	bx,[ecx]
nop
nop
nop
	lss	ebx,[ecx]
nop
nop
nop
	lar	bx,[ecx]
nop
nop
nop
	lar	ebx,[ecx]
nop
nop
nop
	lea	bx,[ecx]
nop
nop
nop
	lea	ebx,[ecx]
nop
nop
nop
	lfs	bx,[ecx]
nop
nop
nop
	lfs	ebx,[ecx]
nop
nop
nop
	lgs	bx,[ecx]
nop
nop
nop
	lgs	ebx,[ecx]
nop
nop
nop
	lsl	bx,[ecx]
nop
nop
nop
	lsl	ebx,[ecx]
nop
nop
nop
	popcnt	bx,[ecx]
nop
nop
nop
	popcnt	ebx,[ecx]
nop
nop
nop
	bsf	bx,cx
nop
nop
nop
	bsf	bx,[ecx]
nop
nop
nop
	bsf	ebx,[ecx]
nop
nop
nop
	bsf	ebx,ecx
nop
nop
nop
	bsr	bx,cx
nop
nop
nop
	bsr	bx,[ecx]
nop
nop
nop
	bsr	ebx,[ecx]
nop
nop
nop
	bsr	ebx,ecx
nop
nop
nop
	lgdt	[ecx]
nop
nop
nop
	lidt	[ecx]
nop
nop
nop
	lldt	[ecx]
nop
nop
nop
	sidt	[ecx]
nop
nop
nop
	sldt	[ecx]
nop
nop
nop
	movbe	bx,[ecx]
nop
nop
nop
	movbe	ebx,[ecx]
nop
nop
nop
	movbe	bx,word [ecx]
nop
nop
nop
	movbe	ebx,dword [ecx]
nop
nop
nop
	movbe	[ecx],bx
nop
nop
nop
	movbe	[ecx],ebx
nop
nop
nop
	movbe	word [ecx],bx
nop
nop
nop
	movbe	dword [ecx],ebx
nop
nop
nop
	ENTER	4,4
nop
nop
nop
	IN	al,dx
nop
nop
nop
	IN	ax,dx
nop
nop
nop
	IN	eax,dx
nop
nop
nop
	INT	22
nop
nop
nop
	OUT	dx,al
nop
nop
nop
	OUT	dx,ax
nop
nop
nop
	OUT	dx,eax
nop
nop
nop
	 cmpsb	
nop
nop
nop
	rep cmpsb	
nop
nop
nop
	repnz cmpsb	
nop
nop
nop
	repne cmpsb	
nop
nop
nop
	repz cmpsb	
nop
nop
nop
	repe cmpsb	
nop
nop
nop
	 cmpsw	
nop
nop
nop
	rep cmpsw	
nop
nop
nop
	repnz cmpsw	
nop
nop
nop
	repne cmpsw	
nop
nop
nop
	repz cmpsw	
nop
nop
nop
	repe cmpsw	
nop
nop
nop
	 cmpsd	
nop
nop
nop
	rep cmpsd	
nop
nop
nop
	repnz cmpsd	
nop
nop
nop
	repne cmpsd	
nop
nop
nop
	repz cmpsd	
nop
nop
nop
	repe cmpsd	
nop
nop
nop
	 insb	
nop
nop
nop
	rep insb	
nop
nop
nop
	repnz insb	
nop
nop
nop
	repne insb	
nop
nop
nop
	repz insb	
nop
nop
nop
	repe insb	
nop
nop
nop
	 insw	
nop
nop
nop
	rep insw	
nop
nop
nop
	repnz insw	
nop
nop
nop
	repne insw	
nop
nop
nop
	repz insw	
nop
nop
nop
	repe insw	
nop
nop
nop
	 insd	
nop
nop
nop
	rep insd	
nop
nop
nop
	repnz insd	
nop
nop
nop
	repne insd	
nop
nop
nop
	repz insd	
nop
nop
nop
	repe insd	
nop
nop
nop
	 lodsb	
nop
nop
nop
	rep lodsb	
nop
nop
nop
	repnz lodsb	
nop
nop
nop
	repne lodsb	
nop
nop
nop
	repz lodsb	
nop
nop
nop
	repe lodsb	
nop
nop
nop
	 lodsw	
nop
nop
nop
	rep lodsw	
nop
nop
nop
	repnz lodsw	
nop
nop
nop
	repne lodsw	
nop
nop
nop
	repz lodsw	
nop
nop
nop
	repe lodsw	
nop
nop
nop
	 lodsd	
nop
nop
nop
	rep lodsd	
nop
nop
nop
	repnz lodsd	
nop
nop
nop
	repne lodsd	
nop
nop
nop
	repz lodsd	
nop
nop
nop
	repe lodsd	
nop
nop
nop
	 lodsw	
nop
nop
nop
	rep lodsw	
nop
nop
nop
	repnz lodsw	
nop
nop
nop
	repne lodsw	
nop
nop
nop
	repz lodsw	
nop
nop
nop
	repe lodsw	
nop
nop
nop
	 movsb	
nop
nop
nop
	rep movsb	
nop
nop
nop
	repnz movsb	
nop
nop
nop
	repne movsb	
nop
nop
nop
	repz movsb	
nop
nop
nop
	repe movsb	
nop
nop
nop
	 movsw	
nop
nop
nop
	rep movsw	
nop
nop
nop
	repnz movsw	
nop
nop
nop
	repne movsw	
nop
nop
nop
	repz movsw	
nop
nop
nop
	repe movsw	
nop
nop
nop
	 movsd	
nop
nop
nop
	rep movsd	
nop
nop
nop
	repnz movsd	
nop
nop
nop
	repne movsd	
nop
nop
nop
	repz movsd	
nop
nop
nop
	repe movsd	
nop
nop
nop
	 outsb	
nop
nop
nop
	rep outsb	
nop
nop
nop
	repnz outsb	
nop
nop
nop
	repne outsb	
nop
nop
nop
	repz outsb	
nop
nop
nop
	repe outsb	
nop
nop
nop
	 outsw	
nop
nop
nop
	rep outsw	
nop
nop
nop
	repnz outsw	
nop
nop
nop
	repne outsw	
nop
nop
nop
	repz outsw	
nop
nop
nop
	repe outsw	
nop
nop
nop
	 outsd	
nop
nop
nop
	rep outsd	
nop
nop
nop
	repnz outsd	
nop
nop
nop
	repne outsd	
nop
nop
nop
	repz outsd	
nop
nop
nop
	repe outsd	
nop
nop
nop
	 scasb	
nop
nop
nop
	rep scasb	
nop
nop
nop
	repnz scasb	
nop
nop
nop
	repne scasb	
nop
nop
nop
	repz scasb	
nop
nop
nop
	repe scasb	
nop
nop
nop
	 scasw	
nop
nop
nop
	rep scasw	
nop
nop
nop
	repnz scasw	
nop
nop
nop
	repne scasw	
nop
nop
nop
	repz scasw	
nop
nop
nop
	repe scasw	
nop
nop
nop
	 scasd	
nop
nop
nop
	rep scasd	
nop
nop
nop
	repnz scasd	
nop
nop
nop
	repne scasd	
nop
nop
nop
	repz scasd	
nop
nop
nop
	repe scasd	
nop
nop
nop
	 stosb	
nop
nop
nop
	rep stosb	
nop
nop
nop
	repnz stosb	
nop
nop
nop
	repne stosb	
nop
nop
nop
	repz stosb	
nop
nop
nop
	repe stosb	
nop
nop
nop
	 stosw	
nop
nop
nop
	rep stosw	
nop
nop
nop
	repnz stosw	
nop
nop
nop
	repne stosw	
nop
nop
nop
	repz stosw	
nop
nop
nop
	repe stosw	
nop
nop
nop
	 stosd	
nop
nop
nop
	rep stosd	
nop
nop
nop
	repnz stosd	
nop
nop
nop
	repne stosd	
nop
nop
nop
	repz stosd	
nop
nop
nop
	repe stosd	
nop
nop
nop
	cmpxchg	[ecx],bl
nop
nop
nop
	cmpxchg	[ecx],bx
nop
nop
nop
	cmpxchg	[ecx],ebx
nop
nop
nop
	xadd	[ecx],bl
nop
nop
nop
	xadd	[ecx],bx
nop
nop
nop
	xadd	[ecx],ebx
nop
nop
nop
	cmpxchg8b	[ecx]
nop
nop
nop
	imul	ax, 44
nop
nop
nop
	imul	ax, 0x1234
nop
nop
nop
	imul	ax, byte 44
nop
nop
nop
	imul	ax, word 0x1234
nop
nop
nop
	imul	eax, 44
nop
nop
nop
	imul	eax, 0x1234
nop
nop
nop
	imul	eax, byte 44
nop
nop
nop
	imul	eax, dword 0x1234
nop
nop
nop
	imul	ax, [ecx], 44
nop
nop
nop
	imul	ax, [ecx], byte 44
nop
nop
nop
	imul	ax, [ecx]
nop
nop
nop
	imul	eax, [ecx], 123444
nop
nop
nop
	imul	eax, [ecx], byte 44
nop
nop
nop
	imul	eax, [ecx]
nop
nop
nop
	imul	byte [ecx]
nop
nop
nop
	imul	word [ecx]
nop
nop
nop
	imul	dword [ecx]
nop
nop
nop
	mov	cr4,ecx
nop
nop
nop
	mov	dr5,ecx
nop
nop
nop
	mov	tr3,ecx
nop
nop
nop
	mov	ecx,cr4
nop
nop
nop
	mov	ecx,dr5
nop
nop
nop
	mov	ecx,tr3
nop
nop
nop
	mov	al, [0x1234]
nop
nop
nop
	mov	al, [fs:0x1234]
nop
nop
nop
	mov	al, [0x12345678]
nop
nop
nop
	mov	al, [fs:0x12345678]
nop
nop
nop
	mov	al, byte [0x1234]
nop
nop
nop
	mov	al, byte [fs:0x1234]
nop
nop
nop
	mov	al, byte [0x12345678]
nop
nop
nop
	mov	al, byte [fs:0x12345678]
nop
nop
nop
	mov	ax, [0x1234]
nop
nop
nop
	mov	ax, [fs:0x1234]
nop
nop
nop
	mov	ax, [0x12345678]
nop
nop
nop
	mov	ax, [fs:0x12345678]
nop
nop
nop
	mov	ax, word [0x1234]
nop
nop
nop
	mov	ax, word [fs:0x1234]
nop
nop
nop
	mov	ax, word [0x12345678]
nop
nop
nop
	mov	ax, word [fs:0x12345678]
nop
nop
nop
	mov	eax, [0x1234]
nop
nop
nop
	mov	eax, [fs:0x1234]
nop
nop
nop
	mov	eax, [0x12345678]
nop
nop
nop
	mov	eax, [fs:0x12345678]
nop
nop
nop
	mov	eax, dword [0x1234]
nop
nop
nop
	mov	eax, dword [fs:0x1234]
nop
nop
nop
	mov	eax, dword [0x12345678]
nop
nop
nop
	mov	eax, dword [fs:0x12345678]
nop
nop
nop
	mov	[0x1234], al
nop
nop
nop
	mov	[fs:0x1234], al
nop
nop
nop
	mov	[0x12345678], al
nop
nop
nop
	mov	[fs:0x12345678], al
nop
nop
nop
	mov	byte [0x1234], al
nop
nop
nop
	mov	byte [fs:0x1234], al
nop
nop
nop
	mov	byte [0x12345678], al
nop
nop
nop
	mov	byte [fs:0x12345678], al
nop
nop
nop
	mov	[0x1234], ax
nop
nop
nop
	mov	[fs:0x1234], ax
nop
nop
nop
	mov	[0x12345678], ax
nop
nop
nop
	mov	[fs:0x12345678], ax
nop
nop
nop
	mov	word [0x1234], ax
nop
nop
nop
	mov	word [fs:0x1234], ax
nop
nop
nop
	mov	word [0x12345678], ax
nop
nop
nop
	mov	word [fs:0x12345678], ax
nop
nop
nop
	mov	[0x1234], eax
nop
nop
nop
	mov	[fs:0x1234], eax
nop
nop
nop
	mov	[0x12345678], eax
nop
nop
nop
	mov	[fs:0x12345678], eax
nop
nop
nop
	mov	dword [0x1234], eax
nop
nop
nop
	mov	dword [fs:0x1234], eax
nop
nop
nop
	mov	dword [0x12345678], eax
nop
nop
nop
	mov	dword [fs:0x12345678], eax
nop
nop
nop
	mov	al,5
nop
nop
nop
	mov	ax,0x1234
nop
nop
nop
	mov	eax,0x12345678
nop
nop
nop
	mov	al,byte 5
nop
nop
nop
	mov	ax,word 0x1234
nop
nop
nop
	mov	eax,dword 0x12345678
nop
nop
nop
	mov	bl,[ecx]
nop
nop
nop
	mov	bx,[ecx]
nop
nop
nop
	mov	ebx,[ecx]
nop
nop
nop
	mov	bl,byte [ecx]
nop
nop
nop
	mov	bx,word [ecx]
nop
nop
nop
	mov	ebx,dword [ecx]
nop
nop
nop
	mov	[ecx],bl
nop
nop
nop
	mov	[ecx],bx
nop
nop
nop
	mov	[ecx],ebx
nop
nop
nop
	mov	byte [ecx],bl
nop
nop
nop
	mov	word [ecx],bx
nop
nop
nop
	mov	dword [ecx],ebx
nop
nop
nop
	mov	byte [ecx],44
nop
nop
nop
	mov	word [ecx],44
nop
nop
nop
	mov	dword [ecx],44
nop
nop
nop
	mov	byte [ecx],byte 44
nop
nop
nop
	mov	word [ecx],word 44
nop
nop
nop
	mov	dword [ecx],dword 44
nop
nop
nop
	mov	bl,44
nop
nop
nop
	mov	bx,44
nop
nop
nop
	mov	ebx,44
nop
nop
nop
	mov	bl,byte 44
nop
nop
nop
	mov	bx,word 44
nop
nop
nop
	mov	ebx,dword 44
nop
nop
nop
	mov	fs, bx
nop
nop
nop
	mov	fs, ebx
nop
nop
nop
	mov	fs, [ecx]
nop
nop
nop
	mov	fs, word [ecx]
nop
nop
nop
	mov	bx, fs
nop
nop
nop
	mov	ebx, fs
nop
nop
nop
	mov	[ecx], fs
nop
nop
nop
	mov	word [ecx], fs
nop
nop
nop
	ret	
nop
nop
nop
	ret	0x1234
nop
nop
nop
	retf	
nop
nop
nop
	retf	0x1234
nop
nop
nop
	test	al,5
nop
nop
nop
	test	ax,0x1234
nop
nop
nop
	test	eax,0x12345678
nop
nop
nop
	test	bl,[ecx]
nop
nop
nop
	test	bx,[ecx]
nop
nop
nop
	test	ebx,[ecx]
nop
nop
nop
	test	bl,byte [ecx]
nop
nop
nop
	test	bx,word [ecx]
nop
nop
nop
	test	ebx,dword [ecx]
nop
nop
nop
	test	[ecx],bl
nop
nop
nop
	test	[ecx],bx
nop
nop
nop
	test	[ecx],ebx
nop
nop
nop
	test	byte [ecx],bl
nop
nop
nop
	test	word [ecx],bx
nop
nop
nop
	test	dword [ecx],ebx
nop
nop
nop
	test	bl,44
nop
nop
nop
	test	bx,44
nop
nop
nop
	test	ebx,44
nop
nop
nop
	test	bl,byte 44
nop
nop
nop
	test	bx,word 44
nop
nop
nop
	test	ebx,dword 44
nop
nop
nop
	test	byte [ecx],44
nop
nop
nop
	test	word [ecx],44
nop
nop
nop
	test	dword [ecx],44
nop
nop
nop
	test	byte [ecx],byte 44
nop
nop
nop
	test	word [ecx],word 44
nop
nop
nop
	test	dword [ecx],dword 44
nop
nop
nop
	test	bl, cl
nop
nop
nop
	test	bl, al
nop
nop
nop
	test	bx, cx
nop
nop
nop
	test	ebx, ecx
nop
nop
nop
	xchg	ax,cx
nop
nop
nop
	xchg	eax,ecx
nop
nop
nop
	xchg	cx,ax
nop
nop
nop
	xchg	ecx,eax
nop
nop
nop
	xchg	bl,[ecx]
nop
nop
nop
	xchg	bx,[ecx]
nop
nop
nop
	xchg	ebx,[ecx]
nop
nop
nop
	xchg	bl, al
nop
nop
nop
	xchg	al, bl
nop
nop
nop
	xchg	bl,byte [ecx]
nop
nop
nop
	xchg	bx,word [ecx]
nop
nop
nop
	xchg	ebx,dword [ecx]
nop
nop
nop
	xchg	[ecx],bl
nop
nop
nop
	xchg	[ecx],bx
nop
nop
nop
	xchg	[ecx],ebx
nop
nop
nop
	xlat	
nop
nop
nop
cld
nop
nop
[bits 32]
	aaa	
nop
nop
nop
	aad	
nop
nop
nop
	aam	
nop
nop
nop
	aas	
nop
nop
nop
	daa	
nop
nop
nop
	das	
nop
nop
nop
	into	
nop
nop
nop
	popa	
nop
nop
nop
	popaw	
nop
nop
nop
	popad	
nop
nop
nop
	pusha	
nop
nop
nop
	pushaw	
nop
nop
nop
	pushad	
nop
nop
nop
	popfw	
nop
nop
nop
	popfd	
nop
nop
nop
	pushfw	
nop
nop
nop
	pushfd	
nop
nop
nop
	cbw	
nop
nop
nop
	cdq	
nop
nop
nop
	clc	
nop
nop
nop
	cld	
nop
nop
nop
	cli	
nop
nop
nop
	clts	
nop
nop
nop
	cmc	
nop
nop
nop
	cpuid	
nop
nop
nop
	cwd	
nop
nop
nop
	cwde	
nop
nop
nop
	hlt	
nop
nop
nop
	icebp	
nop
nop
nop
	int1	
nop
nop
nop
	int3	
nop
nop
nop
	invd	
nop
nop
nop
	iret	
nop
nop
nop
	iretw	
nop
nop
nop
	iretd	
nop
nop
nop
	lfence	
nop
nop
nop
	popf	
nop
nop
nop
	pushf	
nop
nop
nop
	rsm	
nop
nop
nop
	lahf	
nop
nop
nop
	sahf	
nop
nop
nop
	sfence	
nop
nop
nop
	stc	
nop
nop
nop
	std	
nop
nop
nop
	sti	
nop
nop
nop
	syscall	
nop
nop
nop
	sysenter	
nop
nop
nop
	sysexit	
nop
nop
nop
	sysret	
nop
nop
nop
	ud2	
nop
nop
nop
	wait	
nop
nop
nop
	wbinvd	
nop
nop
nop
	wrmsr	
nop
nop
nop
	xsetbv	
nop
nop
nop
	adc	al,5
nop
nop
nop
	adc	ax,0x1234
nop
nop
nop
	adc	eax,0x12345678
nop
nop
nop
	adc	al,byte 5
nop
nop
nop
	adc	ax,word 0x1234
nop
nop
nop
	adc	eax,dword 0x12345678
nop
nop
nop
	adc	bl,[ecx]
nop
nop
nop
	adc	bx,[ecx]
nop
nop
nop
	adc	ebx,[ecx]
nop
nop
nop
	adc	bl,byte [ecx]
nop
nop
nop
	adc	bx,word [ecx]
nop
nop
nop
	adc	ebx,dword [ecx]
nop
nop
nop
	adc	[ecx],bl
nop
nop
nop
	adc	[ecx],bx
nop
nop
nop
	adc	[ecx],ebx
nop
nop
nop
	adc	byte [ecx],bl
nop
nop
nop
	adc	word [ecx],bx
nop
nop
nop
	adc	dword [ecx],ebx
nop
nop
nop
	adc	byte [ecx],44
nop
nop
nop
	adc	word [ecx],44
nop
nop
nop
	adc	dword [ecx],44
nop
nop
nop
	adc	byte [ecx],byte 44
nop
nop
nop
	adc	word [ecx],byte 44
nop
nop
nop
	adc	dword [ecx],byte 44
nop
nop
nop
	adc	byte [ecx],byte 44
nop
nop
nop
	adc	word [ecx],word 44
nop
nop
nop
	adc	dword [ecx],dword 44
nop
nop
nop
	adc	bl,44
nop
nop
nop
	adc	bx,44
nop
nop
nop
	adc	ebx,44
nop
nop
nop
	adc	bl,byte 44
nop
nop
nop
	adc	bx,byte 44
nop
nop
nop
	adc	ebx,byte 44
nop
nop
nop
	adc	bl,byte 44
nop
nop
nop
	adc	bx,word 44
nop
nop
nop
	adc	ebx,dword 44
nop
nop
nop
	add	al,5
nop
nop
nop
	add	ax,0x1234
nop
nop
nop
	add	eax,0x12345678
nop
nop
nop
	add	al,byte 5
nop
nop
nop
	add	ax,word 0x1234
nop
nop
nop
	add	eax,dword 0x12345678
nop
nop
nop
	add	bl,[ecx]
nop
nop
nop
	add	bx,[ecx]
nop
nop
nop
	add	ebx,[ecx]
nop
nop
nop
	add	bl,byte [ecx]
nop
nop
nop
	add	bx,word [ecx]
nop
nop
nop
	add	ebx,dword [ecx]
nop
nop
nop
	add	[ecx],bl
nop
nop
nop
	add	[ecx],bx
nop
nop
nop
	add	[ecx],ebx
nop
nop
nop
	add	byte [ecx],bl
nop
nop
nop
	add	word [ecx],bx
nop
nop
nop
	add	dword [ecx],ebx
nop
nop
nop
	add	byte [ecx],44
nop
nop
nop
	add	word [ecx],44
nop
nop
nop
	add	dword [ecx],44
nop
nop
nop
	add	byte [ecx],byte 44
nop
nop
nop
	add	word [ecx],byte 44
nop
nop
nop
	add	dword [ecx],byte 44
nop
nop
nop
	add	byte [ecx],byte 44
nop
nop
nop
	add	word [ecx],word 44
nop
nop
nop
	add	dword [ecx],dword 44
nop
nop
nop
	add	bl,44
nop
nop
nop
	add	bx,44
nop
nop
nop
	add	ebx,44
nop
nop
nop
	add	bl,byte 44
nop
nop
nop
	add	bx,byte 44
nop
nop
nop
	add	ebx,byte 44
nop
nop
nop
	add	bl,byte 44
nop
nop
nop
	add	bx,word 44
nop
nop
nop
	add	ebx,dword 44
nop
nop
nop
	and	al,5
nop
nop
nop
	and	ax,0x1234
nop
nop
nop
	and	eax,0x12345678
nop
nop
nop
	and	al,byte 5
nop
nop
nop
	and	ax,word 0x1234
nop
nop
nop
	and	eax,dword 0x12345678
nop
nop
nop
	and	bl,[ecx]
nop
nop
nop
	and	bx,[ecx]
nop
nop
nop
	and	ebx,[ecx]
nop
nop
nop
	and	bl,byte [ecx]
nop
nop
nop
	and	bx,word [ecx]
nop
nop
nop
	and	ebx,dword [ecx]
nop
nop
nop
	and	[ecx],bl
nop
nop
nop
	and	[ecx],bx
nop
nop
nop
	and	[ecx],ebx
nop
nop
nop
	and	byte [ecx],bl
nop
nop
nop
	and	word [ecx],bx
nop
nop
nop
	and	dword [ecx],ebx
nop
nop
nop
	and	byte [ecx],44
nop
nop
nop
	and	word [ecx],44
nop
nop
nop
	and	dword [ecx],44
nop
nop
nop
	and	byte [ecx],byte 44
nop
nop
nop
	and	word [ecx],byte 44
nop
nop
nop
	and	dword [ecx],byte 44
nop
nop
nop
	and	byte [ecx],byte 44
nop
nop
nop
	and	word [ecx],word 44
nop
nop
nop
	and	dword [ecx],dword 44
nop
nop
nop
	and	bl,44
nop
nop
nop
	and	bx,44
nop
nop
nop
	and	ebx,44
nop
nop
nop
	and	bl,byte 44
nop
nop
nop
	and	bx,byte 44
nop
nop
nop
	and	ebx,byte 44
nop
nop
nop
	and	bl,byte 44
nop
nop
nop
	and	bx,word 44
nop
nop
nop
	and	ebx,dword 44
nop
nop
nop
	cmp	al,5
nop
nop
nop
	cmp	ax,0x1234
nop
nop
nop
	cmp	eax,0x12345678
nop
nop
nop
	cmp	al,byte 5
nop
nop
nop
	cmp	ax,word 0x1234
nop
nop
nop
	cmp	eax,dword 0x12345678
nop
nop
nop
	cmp	bl,[ecx]
nop
nop
nop
	cmp	bx,[ecx]
nop
nop
nop
	cmp	ebx,[ecx]
nop
nop
nop
	cmp	bl,byte [ecx]
nop
nop
nop
	cmp	bx,word [ecx]
nop
nop
nop
	cmp	ebx,dword [ecx]
nop
nop
nop
	cmp	[ecx],bl
nop
nop
nop
	cmp	[ecx],bx
nop
nop
nop
	cmp	[ecx],ebx
nop
nop
nop
	cmp	byte [ecx],bl
nop
nop
nop
	cmp	word [ecx],bx
nop
nop
nop
	cmp	dword [ecx],ebx
nop
nop
nop
	cmp	byte [ecx],44
nop
nop
nop
	cmp	word [ecx],44
nop
nop
nop
	cmp	dword [ecx],44
nop
nop
nop
	cmp	byte [ecx],byte 44
nop
nop
nop
	cmp	word [ecx],byte 44
nop
nop
nop
	cmp	dword [ecx],byte 44
nop
nop
nop
	cmp	byte [ecx],byte 44
nop
nop
nop
	cmp	word [ecx],word 44
nop
nop
nop
	cmp	dword [ecx],dword 44
nop
nop
nop
	cmp	bl,44
nop
nop
nop
	cmp	bx,44
nop
nop
nop
	cmp	ebx,44
nop
nop
nop
	cmp	bl,byte 44
nop
nop
nop
	cmp	bx,byte 44
nop
nop
nop
	cmp	ebx,byte 44
nop
nop
nop
	cmp	bl,byte 44
nop
nop
nop
	cmp	bx,word 44
nop
nop
nop
	cmp	ebx,dword 44
nop
nop
nop
	or	al,5
nop
nop
nop
	or	ax,0x1234
nop
nop
nop
	or	eax,0x12345678
nop
nop
nop
	or	al,byte 5
nop
nop
nop
	or	ax,word 0x1234
nop
nop
nop
	or	eax,dword 0x12345678
nop
nop
nop
	or	bl,[ecx]
nop
nop
nop
	or	bx,[ecx]
nop
nop
nop
	or	ebx,[ecx]
nop
nop
nop
	or	bl,byte [ecx]
nop
nop
nop
	or	bx,word [ecx]
nop
nop
nop
	or	ebx,dword [ecx]
nop
nop
nop
	or	[ecx],bl
nop
nop
nop
	or	[ecx],bx
nop
nop
nop
	or	[ecx],ebx
nop
nop
nop
	or	byte [ecx],bl
nop
nop
nop
	or	word [ecx],bx
nop
nop
nop
	or	dword [ecx],ebx
nop
nop
nop
	or	byte [ecx],44
nop
nop
nop
	or	word [ecx],44
nop
nop
nop
	or	dword [ecx],44
nop
nop
nop
	or	byte [ecx],byte 44
nop
nop
nop
	or	word [ecx],byte 44
nop
nop
nop
	or	dword [ecx],byte 44
nop
nop
nop
	or	byte [ecx],byte 44
nop
nop
nop
	or	word [ecx],word 44
nop
nop
nop
	or	dword [ecx],dword 44
nop
nop
nop
	or	bl,44
nop
nop
nop
	or	bx,44
nop
nop
nop
	or	ebx,44
nop
nop
nop
	or	bl,byte 44
nop
nop
nop
	or	bx,byte 44
nop
nop
nop
	or	ebx,byte 44
nop
nop
nop
	or	bl,byte 44
nop
nop
nop
	or	bx,word 44
nop
nop
nop
	or	ebx,dword 44
nop
nop
nop
	sbb	al,5
nop
nop
nop
	sbb	ax,0x1234
nop
nop
nop
	sbb	eax,0x12345678
nop
nop
nop
	sbb	al,byte 5
nop
nop
nop
	sbb	ax,word 0x1234
nop
nop
nop
	sbb	eax,dword 0x12345678
nop
nop
nop
	sbb	bl,[ecx]
nop
nop
nop
	sbb	bx,[ecx]
nop
nop
nop
	sbb	ebx,[ecx]
nop
nop
nop
	sbb	bl,byte [ecx]
nop
nop
nop
	sbb	bx,word [ecx]
nop
nop
nop
	sbb	ebx,dword [ecx]
nop
nop
nop
	sbb	[ecx],bl
nop
nop
nop
	sbb	[ecx],bx
nop
nop
nop
	sbb	[ecx],ebx
nop
nop
nop
	sbb	byte [ecx],bl
nop
nop
nop
	sbb	word [ecx],bx
nop
nop
nop
	sbb	dword [ecx],ebx
nop
nop
nop
	sbb	byte [ecx],44
nop
nop
nop
	sbb	word [ecx],44
nop
nop
nop
	sbb	dword [ecx],44
nop
nop
nop
	sbb	byte [ecx],byte 44
nop
nop
nop
	sbb	word [ecx],byte 44
nop
nop
nop
	sbb	dword [ecx],byte 44
nop
nop
nop
	sbb	byte [ecx],byte 44
nop
nop
nop
	sbb	word [ecx],word 44
nop
nop
nop
	sbb	dword [ecx],dword 44
nop
nop
nop
	sbb	bl,44
nop
nop
nop
	sbb	bx,44
nop
nop
nop
	sbb	ebx,44
nop
nop
nop
	sbb	bl,byte 44
nop
nop
nop
	sbb	bx,byte 44
nop
nop
nop
	sbb	ebx,byte 44
nop
nop
nop
	sbb	bl,byte 44
nop
nop
nop
	sbb	bx,word 44
nop
nop
nop
	sbb	ebx,dword 44
nop
nop
nop
	sub	al,5
nop
nop
nop
	sub	ax,0x1234
nop
nop
nop
	sub	eax,0x12345678
nop
nop
nop
	sub	al,byte 5
nop
nop
nop
	sub	ax,word 0x1234
nop
nop
nop
	sub	eax,dword 0x12345678
nop
nop
nop
	sub	bl,[ecx]
nop
nop
nop
	sub	bx,[ecx]
nop
nop
nop
	sub	ebx,[ecx]
nop
nop
nop
	sub	bl,byte [ecx]
nop
nop
nop
	sub	bx,word [ecx]
nop
nop
nop
	sub	ebx,dword [ecx]
nop
nop
nop
	sub	[ecx],bl
nop
nop
nop
	sub	[ecx],bx
nop
nop
nop
	sub	[ecx],ebx
nop
nop
nop
	sub	byte [ecx],bl
nop
nop
nop
	sub	word [ecx],bx
nop
nop
nop
	sub	dword [ecx],ebx
nop
nop
nop
	sub	byte [ecx],44
nop
nop
nop
	sub	word [ecx],44
nop
nop
nop
	sub	dword [ecx],44
nop
nop
nop
	sub	byte [ecx],byte 44
nop
nop
nop
	sub	word [ecx],byte 44
nop
nop
nop
	sub	dword [ecx],byte 44
nop
nop
nop
	sub	byte [ecx],byte 44
nop
nop
nop
	sub	word [ecx],word 44
nop
nop
nop
	sub	dword [ecx],dword 44
nop
nop
nop
	sub	bl,44
nop
nop
nop
	sub	bx,44
nop
nop
nop
	sub	ebx,44
nop
nop
nop
	sub	bl,byte 44
nop
nop
nop
	sub	bx,byte 44
nop
nop
nop
	sub	ebx,byte 44
nop
nop
nop
	sub	bl,byte 44
nop
nop
nop
	sub	bx,word 44
nop
nop
nop
	sub	ebx,dword 44
nop
nop
nop
	xor	al,5
nop
nop
nop
	xor	ax,0x1234
nop
nop
nop
	xor	eax,0x12345678
nop
nop
nop
	xor	al,byte 5
nop
nop
nop
	xor	ax,word 0x1234
nop
nop
nop
	xor	eax,dword 0x12345678
nop
nop
nop
	xor	bl,[ecx]
nop
nop
nop
	xor	bx,[ecx]
nop
nop
nop
	xor	ebx,[ecx]
nop
nop
nop
	xor	bl,byte [ecx]
nop
nop
nop
	xor	bx,word [ecx]
nop
nop
nop
	xor	ebx,dword [ecx]
nop
nop
nop
	xor	[ecx],bl
nop
nop
nop
	xor	[ecx],bx
nop
nop
nop
	xor	[ecx],ebx
nop
nop
nop
	xor	byte [ecx],bl
nop
nop
nop
	xor	word [ecx],bx
nop
nop
nop
	xor	dword [ecx],ebx
nop
nop
nop
	xor	byte [ecx],44
nop
nop
nop
	xor	word [ecx],44
nop
nop
nop
	xor	dword [ecx],44
nop
nop
nop
	xor	byte [ecx],byte 44
nop
nop
nop
	xor	word [ecx],byte 44
nop
nop
nop
	xor	dword [ecx],byte 44
nop
nop
nop
	xor	byte [ecx],byte 44
nop
nop
nop
	xor	word [ecx],word 44
nop
nop
nop
	xor	dword [ecx],dword 44
nop
nop
nop
	xor	bl,44
nop
nop
nop
	xor	bx,44
nop
nop
nop
	xor	ebx,44
nop
nop
nop
	xor	bl,byte 44
nop
nop
nop
	xor	bx,byte 44
nop
nop
nop
	xor	ebx,byte 44
nop
nop
nop
	xor	bl,byte 44
nop
nop
nop
	xor	bx,word 44
nop
nop
nop
	xor	ebx,dword 44
nop
nop
nop
	bt	word [ecx],44
nop
nop
nop
	bt	dword [ecx],44
nop
nop
nop
	bt	word [ecx],bx
nop
nop
nop
	bt	dword [ecx],ebx
nop
nop
nop
	btc	word [ecx],44
nop
nop
nop
	btc	dword [ecx],44
nop
nop
nop
	btc	word [ecx],bx
nop
nop
nop
	btc	dword [ecx],ebx
nop
nop
nop
	btr	word [ecx],44
nop
nop
nop
	btr	dword [ecx],44
nop
nop
nop
	btr	word [ecx],bx
nop
nop
nop
	btr	dword [ecx],ebx
nop
nop
nop
	bts	word [ecx],44
nop
nop
nop
	bts	dword [ecx],44
nop
nop
nop
	bts	word [ecx],bx
nop
nop
nop
	bts	dword [ecx],ebx
nop
nop
nop
	inc	bl
nop
nop
nop
	inc	bx
nop
nop
nop
	inc	ebx
nop
nop
nop
	inc	byte [ecx]
nop
nop
nop
	inc	word [ecx]
nop
nop
nop
	inc	dword [ecx]
nop
nop
nop
	dec	bl
nop
nop
nop
	dec	bx
nop
nop
nop
	dec	ebx
nop
nop
nop
	dec	byte [ecx]
nop
nop
nop
	dec	word [ecx]
nop
nop
nop
	dec	dword [ecx]
nop
nop
nop
	neg	bl
nop
nop
nop
	neg	bx
nop
nop
nop
	neg	ebx
nop
nop
nop
	neg	byte [ecx]
nop
nop
nop
	neg	word [ecx]
nop
nop
nop
	neg	dword [ecx]
nop
nop
nop
	not	bl
nop
nop
nop
	not	bx
nop
nop
nop
	not	ebx
nop
nop
nop
	not	byte [ecx]
nop
nop
nop
	not	word [ecx]
nop
nop
nop
	not	dword [ecx]
nop
nop
nop
	div	bl
nop
nop
nop
	div	bx
nop
nop
nop
	div	ebx
nop
nop
nop
	div	byte [ecx]
nop
nop
nop
	div	word [ecx]
nop
nop
nop
	div	dword [ecx]
nop
nop
nop
	idiv	bl
nop
nop
nop
	idiv	bx
nop
nop
nop
	idiv	ebx
nop
nop
nop
	idiv	byte [ecx]
nop
nop
nop
	idiv	word [ecx]
nop
nop
nop
	idiv	dword [ecx]
nop
nop
nop
	mul	bl
nop
nop
nop
	mul	bx
nop
nop
nop
	mul	ebx
nop
nop
nop
	mul	byte [ecx]
nop
nop
nop
	mul	word [ecx]
nop
nop
nop
	mul	dword [ecx]
nop
nop
nop
	invlpg	[ecx]
nop
nop
nop
	ltr	[ecx]
nop
nop
nop
	prefetchnta	[ecx]
nop
nop
nop
	prefetcht0	[ecx]
nop
nop
nop
	prefetcht1	[ecx]
nop
nop
nop
	prefetcht2	[ecx]
nop
nop
nop
	xrstor	[ecx]
nop
nop
nop
	xsave	[ecx]
nop
nop
nop
	lmsw	bx
nop
nop
nop
	smsw	bx
nop
nop
nop
	verr	bx
nop
nop
nop
	verw	bx
nop
nop
nop
	str	bx
nop
nop
nop
	str	ebx
nop
nop
nop
	bswap	ebx
nop
nop
nop
	pop	bx
nop
nop
nop
	pop	ebx
nop
nop
nop
	pop	word [ecx]
nop
nop
nop
	pop	dword [ecx]
nop
nop
nop
	pop	fs
nop
nop
nop
	pop	gs
nop
nop
nop
	pop	ds
nop
nop
nop
	pop	es
nop
nop
nop
	pop	ss
nop
nop
nop
	pop	cs
nop
nop
nop
	push	bx
nop
nop
nop
	push	ebx
nop
nop
nop
	push	word [ecx]
nop
nop
nop
	push	dword [ecx]
nop
nop
nop
	push	fs
nop
nop
nop
	push	gs
nop
nop
nop
	push	ds
nop
nop
nop
	push	es
nop
nop
nop
	push	ss
nop
nop
nop
	push	cs
nop
nop
nop
	push	44
nop
nop
nop
	push	-3
nop
nop
nop
	push	0x1234
nop
nop
nop
	push	byte 44
nop
nop
nop
	push	word 1234
nop
nop
nop
	push	dword 0x12345678
nop
nop
nop
	call	[fs:0x4444]
nop
nop
nop
	call	 lbl101
	nop
	nop
lbl101: nop
	call	bx
nop
nop
nop
	call	ebx
nop
nop
nop
	call	word [ecx]
nop
nop
nop
	call	dword [ecx]
nop
nop
nop
	call	far word [0x4444]
nop
nop
nop
	call	far word [ecx]
nop
nop
nop
	call	far dword [0x4444]
nop
nop
nop
	call	far dword [ecx]
nop
nop
nop
	jmp	[fs:0x4444]
nop
nop
nop
	jmp	 lbl102
	nop
	nop
lbl102: nop
	jmp	bx
nop
nop
nop
	jmp	ebx
nop
nop
nop
	jmp	word [ecx]
nop
nop
nop
	jmp	dword [ecx]
nop
nop
nop
	jmp	far word [0x4444]
nop
nop
nop
	jmp	far word [ecx]
nop
nop
nop
	jmp	far dword [0x4444]
nop
nop
nop
	jmp	far dword [ecx]
nop
nop
nop
	jmp	short lbl103
	nop
	nop
lbl103: nop
	jmp	near lbl104
	nop
	nop
lbl104: nop
	ja	short lbl105
	nop
	nop
lbl105: nop
	ja	near lbl106
	nop
	nop
lbl106: nop
	ja	 lbl107
	nop
	nop
lbl107: nop
	jae	short lbl108
	nop
	nop
lbl108: nop
	jae	near lbl109
	nop
	nop
lbl109: nop
	jae	 lbl110
	nop
	nop
lbl110: nop
	jb	short lbl111
	nop
	nop
lbl111: nop
	jb	near lbl112
	nop
	nop
lbl112: nop
	jb	 lbl113
	nop
	nop
lbl113: nop
	jbe	short lbl114
	nop
	nop
lbl114: nop
	jbe	near lbl115
	nop
	nop
lbl115: nop
	jbe	 lbl116
	nop
	nop
lbl116: nop
	jc	short lbl117
	nop
	nop
lbl117: nop
	jc	near lbl118
	nop
	nop
lbl118: nop
	jc	 lbl119
	nop
	nop
lbl119: nop
	je	short lbl120
	nop
	nop
lbl120: nop
	je	near lbl121
	nop
	nop
lbl121: nop
	je	 lbl122
	nop
	nop
lbl122: nop
	jg	short lbl123
	nop
	nop
lbl123: nop
	jg	near lbl124
	nop
	nop
lbl124: nop
	jg	 lbl125
	nop
	nop
lbl125: nop
	jge	short lbl126
	nop
	nop
lbl126: nop
	jge	near lbl127
	nop
	nop
lbl127: nop
	jge	 lbl128
	nop
	nop
lbl128: nop
	jl	short lbl129
	nop
	nop
lbl129: nop
	jl	near lbl130
	nop
	nop
lbl130: nop
	jl	 lbl131
	nop
	nop
lbl131: nop
	jle	short lbl132
	nop
	nop
lbl132: nop
	jle	near lbl133
	nop
	nop
lbl133: nop
	jle	 lbl134
	nop
	nop
lbl134: nop
	jna	short lbl135
	nop
	nop
lbl135: nop
	jna	near lbl136
	nop
	nop
lbl136: nop
	jna	 lbl137
	nop
	nop
lbl137: nop
	jnae	short lbl138
	nop
	nop
lbl138: nop
	jnae	near lbl139
	nop
	nop
lbl139: nop
	jnae	 lbl140
	nop
	nop
lbl140: nop
	jnb	short lbl141
	nop
	nop
lbl141: nop
	jnb	near lbl142
	nop
	nop
lbl142: nop
	jnb	 lbl143
	nop
	nop
lbl143: nop
	jnbe	short lbl144
	nop
	nop
lbl144: nop
	jnbe	near lbl145
	nop
	nop
lbl145: nop
	jnbe	 lbl146
	nop
	nop
lbl146: nop
	jnc	short lbl147
	nop
	nop
lbl147: nop
	jnc	near lbl148
	nop
	nop
lbl148: nop
	jnc	 lbl149
	nop
	nop
lbl149: nop
	jne	short lbl150
	nop
	nop
lbl150: nop
	jne	near lbl151
	nop
	nop
lbl151: nop
	jne	 lbl152
	nop
	nop
lbl152: nop
	jng	short lbl153
	nop
	nop
lbl153: nop
	jng	near lbl154
	nop
	nop
lbl154: nop
	jng	 lbl155
	nop
	nop
lbl155: nop
	jnge	short lbl156
	nop
	nop
lbl156: nop
	jnge	near lbl157
	nop
	nop
lbl157: nop
	jnge	 lbl158
	nop
	nop
lbl158: nop
	jnl	short lbl159
	nop
	nop
lbl159: nop
	jnl	near lbl160
	nop
	nop
lbl160: nop
	jnl	 lbl161
	nop
	nop
lbl161: nop
	jnle	short lbl162
	nop
	nop
lbl162: nop
	jnle	near lbl163
	nop
	nop
lbl163: nop
	jnle	 lbl164
	nop
	nop
lbl164: nop
	jno	short lbl165
	nop
	nop
lbl165: nop
	jno	near lbl166
	nop
	nop
lbl166: nop
	jno	 lbl167
	nop
	nop
lbl167: nop
	jnp	short lbl168
	nop
	nop
lbl168: nop
	jnp	near lbl169
	nop
	nop
lbl169: nop
	jnp	 lbl170
	nop
	nop
lbl170: nop
	jns	short lbl171
	nop
	nop
lbl171: nop
	jns	near lbl172
	nop
	nop
lbl172: nop
	jns	 lbl173
	nop
	nop
lbl173: nop
	jnz	short lbl174
	nop
	nop
lbl174: nop
	jnz	near lbl175
	nop
	nop
lbl175: nop
	jnz	 lbl176
	nop
	nop
lbl176: nop
	jo	short lbl177
	nop
	nop
lbl177: nop
	jo	near lbl178
	nop
	nop
lbl178: nop
	jo	 lbl179
	nop
	nop
lbl179: nop
	jp	short lbl180
	nop
	nop
lbl180: nop
	jp	near lbl181
	nop
	nop
lbl181: nop
	jp	 lbl182
	nop
	nop
lbl182: nop
	jpe	short lbl183
	nop
	nop
lbl183: nop
	jpe	near lbl184
	nop
	nop
lbl184: nop
	jpe	 lbl185
	nop
	nop
lbl185: nop
	jpo	short lbl186
	nop
	nop
lbl186: nop
	jpo	near lbl187
	nop
	nop
lbl187: nop
	jpo	 lbl188
	nop
	nop
lbl188: nop
	js	short lbl189
	nop
	nop
lbl189: nop
	js	near lbl190
	nop
	nop
lbl190: nop
	js	 lbl191
	nop
	nop
lbl191: nop
	jz	short lbl192
	nop
	nop
lbl192: nop
	jz	near lbl193
	nop
	nop
lbl193: nop
	jz	 lbl194
	nop
	nop
lbl194: nop
	jcxz	 lbl195
	nop
	nop
lbl195: nop
	jecxz	 lbl196
	nop
	nop
lbl196: nop
	loop	 lbl197
	nop
	nop
lbl197: nop
	loopne	 lbl198
	nop
	nop
lbl198: nop
	loope	 lbl199
	nop
	nop
lbl199: nop
	loopz	 lbl200
	nop
	nop
lbl200: nop
	loopnz	 lbl201
	nop
	nop
lbl201: nop
	seta	[ecx]
nop
nop
nop
	seta	byte [ecx]
nop
nop
nop
	setae	[ecx]
nop
nop
nop
	setae	byte [ecx]
nop
nop
nop
	setb	[ecx]
nop
nop
nop
	setb	byte [ecx]
nop
nop
nop
	setbe	[ecx]
nop
nop
nop
	setbe	byte [ecx]
nop
nop
nop
	setc	[ecx]
nop
nop
nop
	setc	byte [ecx]
nop
nop
nop
	sete	[ecx]
nop
nop
nop
	sete	byte [ecx]
nop
nop
nop
	setg	[ecx]
nop
nop
nop
	setg	byte [ecx]
nop
nop
nop
	setge	[ecx]
nop
nop
nop
	setge	byte [ecx]
nop
nop
nop
	setl	[ecx]
nop
nop
nop
	setl	byte [ecx]
nop
nop
nop
	setle	[ecx]
nop
nop
nop
	setle	byte [ecx]
nop
nop
nop
	setna	[ecx]
nop
nop
nop
	setna	byte [ecx]
nop
nop
nop
	setnae	[ecx]
nop
nop
nop
	setnae	byte [ecx]
nop
nop
nop
	setnb	[ecx]
nop
nop
nop
	setnb	byte [ecx]
nop
nop
nop
	setnbe	[ecx]
nop
nop
nop
	setnbe	byte [ecx]
nop
nop
nop
	setnc	[ecx]
nop
nop
nop
	setnc	byte [ecx]
nop
nop
nop
	setne	[ecx]
nop
nop
nop
	setne	byte [ecx]
nop
nop
nop
	setng	[ecx]
nop
nop
nop
	setng	byte [ecx]
nop
nop
nop
	setnge	[ecx]
nop
nop
nop
	setnge	byte [ecx]
nop
nop
nop
	setnl	[ecx]
nop
nop
nop
	setnl	byte [ecx]
nop
nop
nop
	setnle	[ecx]
nop
nop
nop
	setnle	byte [ecx]
nop
nop
nop
	setno	[ecx]
nop
nop
nop
	setno	byte [ecx]
nop
nop
nop
	setnp	[ecx]
nop
nop
nop
	setnp	byte [ecx]
nop
nop
nop
	setns	[ecx]
nop
nop
nop
	setns	byte [ecx]
nop
nop
nop
	setnz	[ecx]
nop
nop
nop
	setnz	byte [ecx]
nop
nop
nop
	seto	[ecx]
nop
nop
nop
	seto	byte [ecx]
nop
nop
nop
	setp	[ecx]
nop
nop
nop
	setp	byte [ecx]
nop
nop
nop
	setpe	[ecx]
nop
nop
nop
	setpe	byte [ecx]
nop
nop
nop
	setpo	[ecx]
nop
nop
nop
	setpo	byte [ecx]
nop
nop
nop
	sets	[ecx]
nop
nop
nop
	sets	byte [ecx]
nop
nop
nop
	setz	[ecx]
nop
nop
nop
	setz	byte [ecx]
nop
nop
nop
	cmova	bx,[ecx]
nop
nop
nop
	cmova	ebx,[ecx]
nop
nop
nop
	cmova	bx,word [ecx]
nop
nop
nop
	cmova	ebx,dword [ecx]
nop
nop
nop
	cmovae	bx,[ecx]
nop
nop
nop
	cmovae	ebx,[ecx]
nop
nop
nop
	cmovae	bx,word [ecx]
nop
nop
nop
	cmovae	ebx,dword [ecx]
nop
nop
nop
	cmovb	bx,[ecx]
nop
nop
nop
	cmovb	ebx,[ecx]
nop
nop
nop
	cmovb	bx,word [ecx]
nop
nop
nop
	cmovb	ebx,dword [ecx]
nop
nop
nop
	cmovbe	bx,[ecx]
nop
nop
nop
	cmovbe	ebx,[ecx]
nop
nop
nop
	cmovbe	bx,word [ecx]
nop
nop
nop
	cmovbe	ebx,dword [ecx]
nop
nop
nop
	cmovc	bx,[ecx]
nop
nop
nop
	cmovc	ebx,[ecx]
nop
nop
nop
	cmovc	bx,word [ecx]
nop
nop
nop
	cmovc	ebx,dword [ecx]
nop
nop
nop
	cmove	bx,[ecx]
nop
nop
nop
	cmove	ebx,[ecx]
nop
nop
nop
	cmove	bx,word [ecx]
nop
nop
nop
	cmove	ebx,dword [ecx]
nop
nop
nop
	cmovg	bx,[ecx]
nop
nop
nop
	cmovg	ebx,[ecx]
nop
nop
nop
	cmovg	bx,word [ecx]
nop
nop
nop
	cmovg	ebx,dword [ecx]
nop
nop
nop
	cmovge	bx,[ecx]
nop
nop
nop
	cmovge	ebx,[ecx]
nop
nop
nop
	cmovge	bx,word [ecx]
nop
nop
nop
	cmovge	ebx,dword [ecx]
nop
nop
nop
	cmovl	bx,[ecx]
nop
nop
nop
	cmovl	ebx,[ecx]
nop
nop
nop
	cmovl	bx,word [ecx]
nop
nop
nop
	cmovl	ebx,dword [ecx]
nop
nop
nop
	cmovle	bx,[ecx]
nop
nop
nop
	cmovle	ebx,[ecx]
nop
nop
nop
	cmovle	bx,word [ecx]
nop
nop
nop
	cmovle	ebx,dword [ecx]
nop
nop
nop
	cmovna	bx,[ecx]
nop
nop
nop
	cmovna	ebx,[ecx]
nop
nop
nop
	cmovna	bx,word [ecx]
nop
nop
nop
	cmovna	ebx,dword [ecx]
nop
nop
nop
	cmovnae	bx,[ecx]
nop
nop
nop
	cmovnae	ebx,[ecx]
nop
nop
nop
	cmovnae	bx,word [ecx]
nop
nop
nop
	cmovnae	ebx,dword [ecx]
nop
nop
nop
	cmovnb	bx,[ecx]
nop
nop
nop
	cmovnb	ebx,[ecx]
nop
nop
nop
	cmovnb	bx,word [ecx]
nop
nop
nop
	cmovnb	ebx,dword [ecx]
nop
nop
nop
	cmovnbe	bx,[ecx]
nop
nop
nop
	cmovnbe	ebx,[ecx]
nop
nop
nop
	cmovnbe	bx,word [ecx]
nop
nop
nop
	cmovnbe	ebx,dword [ecx]
nop
nop
nop
	cmovnc	bx,[ecx]
nop
nop
nop
	cmovnc	ebx,[ecx]
nop
nop
nop
	cmovnc	bx,word [ecx]
nop
nop
nop
	cmovnc	ebx,dword [ecx]
nop
nop
nop
	cmovne	bx,[ecx]
nop
nop
nop
	cmovne	ebx,[ecx]
nop
nop
nop
	cmovne	bx,word [ecx]
nop
nop
nop
	cmovne	ebx,dword [ecx]
nop
nop
nop
	cmovng	bx,[ecx]
nop
nop
nop
	cmovng	ebx,[ecx]
nop
nop
nop
	cmovng	bx,word [ecx]
nop
nop
nop
	cmovng	ebx,dword [ecx]
nop
nop
nop
	cmovnge	bx,[ecx]
nop
nop
nop
	cmovnge	ebx,[ecx]
nop
nop
nop
	cmovnge	bx,word [ecx]
nop
nop
nop
	cmovnge	ebx,dword [ecx]
nop
nop
nop
	cmovnl	bx,[ecx]
nop
nop
nop
	cmovnl	ebx,[ecx]
nop
nop
nop
	cmovnl	bx,word [ecx]
nop
nop
nop
	cmovnl	ebx,dword [ecx]
nop
nop
nop
	cmovnle	bx,[ecx]
nop
nop
nop
	cmovnle	ebx,[ecx]
nop
nop
nop
	cmovnle	bx,word [ecx]
nop
nop
nop
	cmovnle	ebx,dword [ecx]
nop
nop
nop
	cmovno	bx,[ecx]
nop
nop
nop
	cmovno	ebx,[ecx]
nop
nop
nop
	cmovno	bx,word [ecx]
nop
nop
nop
	cmovno	ebx,dword [ecx]
nop
nop
nop
	cmovnp	bx,[ecx]
nop
nop
nop
	cmovnp	ebx,[ecx]
nop
nop
nop
	cmovnp	bx,word [ecx]
nop
nop
nop
	cmovnp	ebx,dword [ecx]
nop
nop
nop
	cmovns	bx,[ecx]
nop
nop
nop
	cmovns	ebx,[ecx]
nop
nop
nop
	cmovns	bx,word [ecx]
nop
nop
nop
	cmovns	ebx,dword [ecx]
nop
nop
nop
	cmovnz	bx,[ecx]
nop
nop
nop
	cmovnz	ebx,[ecx]
nop
nop
nop
	cmovnz	bx,word [ecx]
nop
nop
nop
	cmovnz	ebx,dword [ecx]
nop
nop
nop
	cmovo	bx,[ecx]
nop
nop
nop
	cmovo	ebx,[ecx]
nop
nop
nop
	cmovo	bx,word [ecx]
nop
nop
nop
	cmovo	ebx,dword [ecx]
nop
nop
nop
	cmovp	bx,[ecx]
nop
nop
nop
	cmovp	ebx,[ecx]
nop
nop
nop
	cmovp	bx,word [ecx]
nop
nop
nop
	cmovp	ebx,dword [ecx]
nop
nop
nop
	cmovpe	bx,[ecx]
nop
nop
nop
	cmovpe	ebx,[ecx]
nop
nop
nop
	cmovpe	bx,word [ecx]
nop
nop
nop
	cmovpe	ebx,dword [ecx]
nop
nop
nop
	cmovpo	bx,[ecx]
nop
nop
nop
	cmovpo	ebx,[ecx]
nop
nop
nop
	cmovpo	bx,word [ecx]
nop
nop
nop
	cmovpo	ebx,dword [ecx]
nop
nop
nop
	cmovs	bx,[ecx]
nop
nop
nop
	cmovs	ebx,[ecx]
nop
nop
nop
	cmovs	bx,word [ecx]
nop
nop
nop
	cmovs	ebx,dword [ecx]
nop
nop
nop
	cmovz	bx,[ecx]
nop
nop
nop
	cmovz	ebx,[ecx]
nop
nop
nop
	cmovz	bx,word [ecx]
nop
nop
nop
	cmovz	ebx,dword [ecx]
nop
nop
nop
	movsx	bx,cl
nop
nop
nop
	movsx	ebx,al
nop
nop
nop
	movsx	ebx,cx
nop
nop
nop
	movsx	bx,byte [ecx]
nop
nop
nop
	movsx	ebx,byte [ecx]
nop
nop
nop
	movsx	ebx,word [ecx]
nop
nop
nop
	movzx	bx,cl
nop
nop
nop
	movzx	ebx,al
nop
nop
nop
	movzx	ebx,cx
nop
nop
nop
	movzx	bx,byte [ecx]
nop
nop
nop
	movzx	ebx,byte [ecx]
nop
nop
nop
	movzx	ebx,word [ecx]
nop
nop
nop
	rcl	bl,1
nop
nop
nop
	rcl	bx,1
nop
nop
nop
	rcl	ebx,1
nop
nop
nop
	rcl	byte [ecx],1
nop
nop
nop
	rcl	word [ecx],1
nop
nop
nop
	rcl	dword [ecx],1
nop
nop
nop
	rcl	bl,3
nop
nop
nop
	rcl	bx,3
nop
nop
nop
	rcl	ebx,3
nop
nop
nop
	rcl	byte [ecx],3
nop
nop
nop
	rcl	word [ecx],3
nop
nop
nop
	rcl	dword [ecx],3
nop
nop
nop
	rcl	bl,cl
nop
nop
nop
	rcl	bx,cl
nop
nop
nop
	rcl	ebx,cl
nop
nop
nop
	rcl	byte [ecx],cl
nop
nop
nop
	rcl	word [ecx],cl
nop
nop
nop
	rcl	dword [ecx],cl
nop
nop
nop
	rcr	bl,1
nop
nop
nop
	rcr	bx,1
nop
nop
nop
	rcr	ebx,1
nop
nop
nop
	rcr	byte [ecx],1
nop
nop
nop
	rcr	word [ecx],1
nop
nop
nop
	rcr	dword [ecx],1
nop
nop
nop
	rcr	bl,3
nop
nop
nop
	rcr	bx,3
nop
nop
nop
	rcr	ebx,3
nop
nop
nop
	rcr	byte [ecx],3
nop
nop
nop
	rcr	word [ecx],3
nop
nop
nop
	rcr	dword [ecx],3
nop
nop
nop
	rcr	bl,cl
nop
nop
nop
	rcr	bx,cl
nop
nop
nop
	rcr	ebx,cl
nop
nop
nop
	rcr	byte [ecx],cl
nop
nop
nop
	rcr	word [ecx],cl
nop
nop
nop
	rcr	dword [ecx],cl
nop
nop
nop
	rol	bl,1
nop
nop
nop
	rol	bx,1
nop
nop
nop
	rol	ebx,1
nop
nop
nop
	rol	byte [ecx],1
nop
nop
nop
	rol	word [ecx],1
nop
nop
nop
	rol	dword [ecx],1
nop
nop
nop
	rol	bl,3
nop
nop
nop
	rol	bx,3
nop
nop
nop
	rol	ebx,3
nop
nop
nop
	rol	byte [ecx],3
nop
nop
nop
	rol	word [ecx],3
nop
nop
nop
	rol	dword [ecx],3
nop
nop
nop
	rol	bl,cl
nop
nop
nop
	rol	bx,cl
nop
nop
nop
	rol	ebx,cl
nop
nop
nop
	rol	byte [ecx],cl
nop
nop
nop
	rol	word [ecx],cl
nop
nop
nop
	rol	dword [ecx],cl
nop
nop
nop
	ror	bl,1
nop
nop
nop
	ror	bx,1
nop
nop
nop
	ror	ebx,1
nop
nop
nop
	ror	byte [ecx],1
nop
nop
nop
	ror	word [ecx],1
nop
nop
nop
	ror	dword [ecx],1
nop
nop
nop
	ror	bl,3
nop
nop
nop
	ror	bx,3
nop
nop
nop
	ror	ebx,3
nop
nop
nop
	ror	byte [ecx],3
nop
nop
nop
	ror	word [ecx],3
nop
nop
nop
	ror	dword [ecx],3
nop
nop
nop
	ror	bl,cl
nop
nop
nop
	ror	bx,cl
nop
nop
nop
	ror	ebx,cl
nop
nop
nop
	ror	byte [ecx],cl
nop
nop
nop
	ror	word [ecx],cl
nop
nop
nop
	ror	dword [ecx],cl
nop
nop
nop
	sal	bl,1
nop
nop
nop
	sal	bx,1
nop
nop
nop
	sal	ebx,1
nop
nop
nop
	sal	byte [ecx],1
nop
nop
nop
	sal	word [ecx],1
nop
nop
nop
	sal	dword [ecx],1
nop
nop
nop
	sal	bl,3
nop
nop
nop
	sal	bx,3
nop
nop
nop
	sal	ebx,3
nop
nop
nop
	sal	byte [ecx],3
nop
nop
nop
	sal	word [ecx],3
nop
nop
nop
	sal	dword [ecx],3
nop
nop
nop
	sal	bl,cl
nop
nop
nop
	sal	bx,cl
nop
nop
nop
	sal	ebx,cl
nop
nop
nop
	sal	byte [ecx],cl
nop
nop
nop
	sal	word [ecx],cl
nop
nop
nop
	sal	dword [ecx],cl
nop
nop
nop
	sar	bl,1
nop
nop
nop
	sar	bx,1
nop
nop
nop
	sar	ebx,1
nop
nop
nop
	sar	byte [ecx],1
nop
nop
nop
	sar	word [ecx],1
nop
nop
nop
	sar	dword [ecx],1
nop
nop
nop
	sar	bl,3
nop
nop
nop
	sar	bx,3
nop
nop
nop
	sar	ebx,3
nop
nop
nop
	sar	byte [ecx],3
nop
nop
nop
	sar	word [ecx],3
nop
nop
nop
	sar	dword [ecx],3
nop
nop
nop
	sar	bl,cl
nop
nop
nop
	sar	bx,cl
nop
nop
nop
	sar	ebx,cl
nop
nop
nop
	sar	byte [ecx],cl
nop
nop
nop
	sar	word [ecx],cl
nop
nop
nop
	sar	dword [ecx],cl
nop
nop
nop
	shl	bl,1
nop
nop
nop
	shl	bx,1
nop
nop
nop
	shl	ebx,1
nop
nop
nop
	shl	byte [ecx],1
nop
nop
nop
	shl	word [ecx],1
nop
nop
nop
	shl	dword [ecx],1
nop
nop
nop
	shl	bl,3
nop
nop
nop
	shl	bx,3
nop
nop
nop
	shl	ebx,3
nop
nop
nop
	shl	byte [ecx],3
nop
nop
nop
	shl	word [ecx],3
nop
nop
nop
	shl	dword [ecx],3
nop
nop
nop
	shl	bl,cl
nop
nop
nop
	shl	bx,cl
nop
nop
nop
	shl	ebx,cl
nop
nop
nop
	shl	byte [ecx],cl
nop
nop
nop
	shl	word [ecx],cl
nop
nop
nop
	shl	dword [ecx],cl
nop
nop
nop
	shr	bl,1
nop
nop
nop
	shr	bx,1
nop
nop
nop
	shr	ebx,1
nop
nop
nop
	shr	byte [ecx],1
nop
nop
nop
	shr	word [ecx],1
nop
nop
nop
	shr	dword [ecx],1
nop
nop
nop
	shr	bl,3
nop
nop
nop
	shr	bx,3
nop
nop
nop
	shr	ebx,3
nop
nop
nop
	shr	byte [ecx],3
nop
nop
nop
	shr	word [ecx],3
nop
nop
nop
	shr	dword [ecx],3
nop
nop
nop
	shr	bl,cl
nop
nop
nop
	shr	bx,cl
nop
nop
nop
	shr	ebx,cl
nop
nop
nop
	shr	byte [ecx],cl
nop
nop
nop
	shr	word [ecx],cl
nop
nop
nop
	shr	dword [ecx],cl
nop
nop
nop
	shld	word [ecx],bx,1
nop
nop
nop
	shld	dword [ecx],ebx,1
nop
nop
nop
	shld	word [ecx],bx,cl
nop
nop
nop
	shld	dword [ecx],ebx,cl
nop
nop
nop
	shrd	word [ecx],bx,1
nop
nop
nop
	shrd	dword [ecx],ebx,1
nop
nop
nop
	shrd	word [ecx],bx,cl
nop
nop
nop
	shrd	dword [ecx],ebx,cl
nop
nop
nop
	bound	bx,[ecx]
nop
nop
nop
	bound	ebx,[ecx]
nop
nop
nop
	lds	bx,[ecx]
nop
nop
nop
	lds	ebx,[ecx]
nop
nop
nop
	les	bx,[ecx]
nop
nop
nop
	les	ebx,[ecx]
nop
nop
nop
	lss	bx,[ecx]
nop
nop
nop
	lss	ebx,[ecx]
nop
nop
nop
	lar	bx,[ecx]
nop
nop
nop
	lar	ebx,[ecx]
nop
nop
nop
	lea	bx,[ecx]
nop
nop
nop
	lea	ebx,[ecx]
nop
nop
nop
	lfs	bx,[ecx]
nop
nop
nop
	lfs	ebx,[ecx]
nop
nop
nop
	lgs	bx,[ecx]
nop
nop
nop
	lgs	ebx,[ecx]
nop
nop
nop
	lsl	bx,[ecx]
nop
nop
nop
	lsl	ebx,[ecx]
nop
nop
nop
	popcnt	bx,[ecx]
nop
nop
nop
	popcnt	ebx,[ecx]
nop
nop
nop
	bsf	bx,cx
nop
nop
nop
	bsf	bx,[ecx]
nop
nop
nop
	bsf	ebx,[ecx]
nop
nop
nop
	bsf	ebx,ecx
nop
nop
nop
	bsr	bx,cx
nop
nop
nop
	bsr	bx,[ecx]
nop
nop
nop
	bsr	ebx,[ecx]
nop
nop
nop
	bsr	ebx,ecx
nop
nop
nop
	lgdt	[ecx]
nop
nop
nop
	lidt	[ecx]
nop
nop
nop
	lldt	[ecx]
nop
nop
nop
	sidt	[ecx]
nop
nop
nop
	sldt	[ecx]
nop
nop
nop
	movbe	bx,[ecx]
nop
nop
nop
	movbe	ebx,[ecx]
nop
nop
nop
	movbe	bx,word [ecx]
nop
nop
nop
	movbe	ebx,dword [ecx]
nop
nop
nop
	movbe	[ecx],bx
nop
nop
nop
	movbe	[ecx],ebx
nop
nop
nop
	movbe	word [ecx],bx
nop
nop
nop
	movbe	dword [ecx],ebx
nop
nop
nop
	ENTER	4,4
nop
nop
nop
	IN	al,dx
nop
nop
nop
	IN	ax,dx
nop
nop
nop
	IN	eax,dx
nop
nop
nop
	INT	22
nop
nop
nop
	OUT	dx,al
nop
nop
nop
	OUT	dx,ax
nop
nop
nop
	OUT	dx,eax
nop
nop
nop
	 cmpsb	
nop
nop
nop
	rep cmpsb	
nop
nop
nop
	repnz cmpsb	
nop
nop
nop
	repne cmpsb	
nop
nop
nop
	repz cmpsb	
nop
nop
nop
	repe cmpsb	
nop
nop
nop
	 cmpsw	
nop
nop
nop
	rep cmpsw	
nop
nop
nop
	repnz cmpsw	
nop
nop
nop
	repne cmpsw	
nop
nop
nop
	repz cmpsw	
nop
nop
nop
	repe cmpsw	
nop
nop
nop
	 cmpsd	
nop
nop
nop
	rep cmpsd	
nop
nop
nop
	repnz cmpsd	
nop
nop
nop
	repne cmpsd	
nop
nop
nop
	repz cmpsd	
nop
nop
nop
	repe cmpsd	
nop
nop
nop
	 insb	
nop
nop
nop
	rep insb	
nop
nop
nop
	repnz insb	
nop
nop
nop
	repne insb	
nop
nop
nop
	repz insb	
nop
nop
nop
	repe insb	
nop
nop
nop
	 insw	
nop
nop
nop
	rep insw	
nop
nop
nop
	repnz insw	
nop
nop
nop
	repne insw	
nop
nop
nop
	repz insw	
nop
nop
nop
	repe insw	
nop
nop
nop
	 insd	
nop
nop
nop
	rep insd	
nop
nop
nop
	repnz insd	
nop
nop
nop
	repne insd	
nop
nop
nop
	repz insd	
nop
nop
nop
	repe insd	
nop
nop
nop
	 lodsb	
nop
nop
nop
	rep lodsb	
nop
nop
nop
	repnz lodsb	
nop
nop
nop
	repne lodsb	
nop
nop
nop
	repz lodsb	
nop
nop
nop
	repe lodsb	
nop
nop
nop
	 lodsw	
nop
nop
nop
	rep lodsw	
nop
nop
nop
	repnz lodsw	
nop
nop
nop
	repne lodsw	
nop
nop
nop
	repz lodsw	
nop
nop
nop
	repe lodsw	
nop
nop
nop
	 lodsd	
nop
nop
nop
	rep lodsd	
nop
nop
nop
	repnz lodsd	
nop
nop
nop
	repne lodsd	
nop
nop
nop
	repz lodsd	
nop
nop
nop
	repe lodsd	
nop
nop
nop
	 lodsw	
nop
nop
nop
	rep lodsw	
nop
nop
nop
	repnz lodsw	
nop
nop
nop
	repne lodsw	
nop
nop
nop
	repz lodsw	
nop
nop
nop
	repe lodsw	
nop
nop
nop
	 movsb	
nop
nop
nop
	rep movsb	
nop
nop
nop
	repnz movsb	
nop
nop
nop
	repne movsb	
nop
nop
nop
	repz movsb	
nop
nop
nop
	repe movsb	
nop
nop
nop
	 movsw	
nop
nop
nop
	rep movsw	
nop
nop
nop
	repnz movsw	
nop
nop
nop
	repne movsw	
nop
nop
nop
	repz movsw	
nop
nop
nop
	repe movsw	
nop
nop
nop
	 movsd	
nop
nop
nop
	rep movsd	
nop
nop
nop
	repnz movsd	
nop
nop
nop
	repne movsd	
nop
nop
nop
	repz movsd	
nop
nop
nop
	repe movsd	
nop
nop
nop
	 outsb	
nop
nop
nop
	rep outsb	
nop
nop
nop
	repnz outsb	
nop
nop
nop
	repne outsb	
nop
nop
nop
	repz outsb	
nop
nop
nop
	repe outsb	
nop
nop
nop
	 outsw	
nop
nop
nop
	rep outsw	
nop
nop
nop
	repnz outsw	
nop
nop
nop
	repne outsw	
nop
nop
nop
	repz outsw	
nop
nop
nop
	repe outsw	
nop
nop
nop
	 outsd	
nop
nop
nop
	rep outsd	
nop
nop
nop
	repnz outsd	
nop
nop
nop
	repne outsd	
nop
nop
nop
	repz outsd	
nop
nop
nop
	repe outsd	
nop
nop
nop
	 scasb	
nop
nop
nop
	rep scasb	
nop
nop
nop
	repnz scasb	
nop
nop
nop
	repne scasb	
nop
nop
nop
	repz scasb	
nop
nop
nop
	repe scasb	
nop
nop
nop
	 scasw	
nop
nop
nop
	rep scasw	
nop
nop
nop
	repnz scasw	
nop
nop
nop
	repne scasw	
nop
nop
nop
	repz scasw	
nop
nop
nop
	repe scasw	
nop
nop
nop
	 scasd	
nop
nop
nop
	rep scasd	
nop
nop
nop
	repnz scasd	
nop
nop
nop
	repne scasd	
nop
nop
nop
	repz scasd	
nop
nop
nop
	repe scasd	
nop
nop
nop
	 stosb	
nop
nop
nop
	rep stosb	
nop
nop
nop
	repnz stosb	
nop
nop
nop
	repne stosb	
nop
nop
nop
	repz stosb	
nop
nop
nop
	repe stosb	
nop
nop
nop
	 stosw	
nop
nop
nop
	rep stosw	
nop
nop
nop
	repnz stosw	
nop
nop
nop
	repne stosw	
nop
nop
nop
	repz stosw	
nop
nop
nop
	repe stosw	
nop
nop
nop
	 stosd	
nop
nop
nop
	rep stosd	
nop
nop
nop
	repnz stosd	
nop
nop
nop
	repne stosd	
nop
nop
nop
	repz stosd	
nop
nop
nop
	repe stosd	
nop
nop
nop
	cmpxchg	[ecx],bl
nop
nop
nop
	cmpxchg	[ecx],bx
nop
nop
nop
	cmpxchg	[ecx],ebx
nop
nop
nop
	xadd	[ecx],bl
nop
nop
nop
	xadd	[ecx],bx
nop
nop
nop
	xadd	[ecx],ebx
nop
nop
nop
	cmpxchg8b	[ecx]
nop
nop
nop
	imul	ax, 44
nop
nop
nop
	imul	ax, 0x1234
nop
nop
nop
	imul	ax, byte 44
nop
nop
nop
	imul	ax, word 0x1234
nop
nop
nop
	imul	eax, 44
nop
nop
nop
	imul	eax, 0x1234
nop
nop
nop
	imul	eax, byte 44
nop
nop
nop
	imul	eax, dword 0x1234
nop
nop
nop
	imul	ax, [ecx], 44
nop
nop
nop
	imul	ax, [ecx], byte 44
nop
nop
nop
	imul	ax, [ecx]
nop
nop
nop
	imul	eax, [ecx], 123444
nop
nop
nop
	imul	eax, [ecx], byte 44
nop
nop
nop
	imul	eax, [ecx]
nop
nop
nop
	imul	byte [ecx]
nop
nop
nop
	imul	word [ecx]
nop
nop
nop
	imul	dword [ecx]
nop
nop
nop
	mov	cr4,ecx
nop
nop
nop
	mov	dr5,ecx
nop
nop
nop
	mov	tr3,ecx
nop
nop
nop
	mov	ecx,cr4
nop
nop
nop
	mov	ecx,dr5
nop
nop
nop
	mov	ecx,tr3
nop
nop
nop
	mov	al, [0x1234]
nop
nop
nop
	mov	al, [fs:0x1234]
nop
nop
nop
	mov	al, [0x12345678]
nop
nop
nop
	mov	al, [fs:0x12345678]
nop
nop
nop
	mov	al, byte [0x1234]
nop
nop
nop
	mov	al, byte [fs:0x1234]
nop
nop
nop
	mov	al, byte [0x12345678]
nop
nop
nop
	mov	al, byte [fs:0x12345678]
nop
nop
nop
	mov	ax, [0x1234]
nop
nop
nop
	mov	ax, [fs:0x1234]
nop
nop
nop
	mov	ax, [0x12345678]
nop
nop
nop
	mov	ax, [fs:0x12345678]
nop
nop
nop
	mov	ax, word [0x1234]
nop
nop
nop
	mov	ax, word [fs:0x1234]
nop
nop
nop
	mov	ax, word [0x12345678]
nop
nop
nop
	mov	ax, word [fs:0x12345678]
nop
nop
nop
	mov	eax, [0x1234]
nop
nop
nop
	mov	eax, [fs:0x1234]
nop
nop
nop
	mov	eax, [0x12345678]
nop
nop
nop
	mov	eax, [fs:0x12345678]
nop
nop
nop
	mov	eax, dword [0x1234]
nop
nop
nop
	mov	eax, dword [fs:0x1234]
nop
nop
nop
	mov	eax, dword [0x12345678]
nop
nop
nop
	mov	eax, dword [fs:0x12345678]
nop
nop
nop
	mov	[0x1234], al
nop
nop
nop
	mov	[fs:0x1234], al
nop
nop
nop
	mov	[0x12345678], al
nop
nop
nop
	mov	[fs:0x12345678], al
nop
nop
nop
	mov	byte [0x1234], al
nop
nop
nop
	mov	byte [fs:0x1234], al
nop
nop
nop
	mov	byte [0x12345678], al
nop
nop
nop
	mov	byte [fs:0x12345678], al
nop
nop
nop
	mov	[0x1234], ax
nop
nop
nop
	mov	[fs:0x1234], ax
nop
nop
nop
	mov	[0x12345678], ax
nop
nop
nop
	mov	[fs:0x12345678], ax
nop
nop
nop
	mov	word [0x1234], ax
nop
nop
nop
	mov	word [fs:0x1234], ax
nop
nop
nop
	mov	word [0x12345678], ax
nop
nop
nop
	mov	word [fs:0x12345678], ax
nop
nop
nop
	mov	[0x1234], eax
nop
nop
nop
	mov	[fs:0x1234], eax
nop
nop
nop
	mov	[0x12345678], eax
nop
nop
nop
	mov	[fs:0x12345678], eax
nop
nop
nop
	mov	dword [0x1234], eax
nop
nop
nop
	mov	dword [fs:0x1234], eax
nop
nop
nop
	mov	dword [0x12345678], eax
nop
nop
nop
	mov	dword [fs:0x12345678], eax
nop
nop
nop
	mov	al,5
nop
nop
nop
	mov	ax,0x1234
nop
nop
nop
	mov	eax,0x12345678
nop
nop
nop
	mov	al,byte 5
nop
nop
nop
	mov	ax,word 0x1234
nop
nop
nop
	mov	eax,dword 0x12345678
nop
nop
nop
	mov	bl,[ecx]
nop
nop
nop
	mov	bx,[ecx]
nop
nop
nop
	mov	ebx,[ecx]
nop
nop
nop
	mov	bl,byte [ecx]
nop
nop
nop
	mov	bx,word [ecx]
nop
nop
nop
	mov	ebx,dword [ecx]
nop
nop
nop
	mov	[ecx],bl
nop
nop
nop
	mov	[ecx],bx
nop
nop
nop
	mov	[ecx],ebx
nop
nop
nop
	mov	byte [ecx],bl
nop
nop
nop
	mov	word [ecx],bx
nop
nop
nop
	mov	dword [ecx],ebx
nop
nop
nop
	mov	byte [ecx],44
nop
nop
nop
	mov	word [ecx],44
nop
nop
nop
	mov	dword [ecx],44
nop
nop
nop
	mov	byte [ecx],byte 44
nop
nop
nop
	mov	word [ecx],word 44
nop
nop
nop
	mov	dword [ecx],dword 44
nop
nop
nop
	mov	bl,44
nop
nop
nop
	mov	bx,44
nop
nop
nop
	mov	ebx,44
nop
nop
nop
	mov	bl,byte 44
nop
nop
nop
	mov	bx,word 44
nop
nop
nop
	mov	ebx,dword 44
nop
nop
nop
	mov	fs, bx
nop
nop
nop
	mov	fs, ebx
nop
nop
nop
	mov	fs, [ecx]
nop
nop
nop
	mov	fs, word [ecx]
nop
nop
nop
	mov	bx, fs
nop
nop
nop
	mov	ebx, fs
nop
nop
nop
	mov	[ecx], fs
nop
nop
nop
	mov	word [ecx], fs
nop
nop
nop
	ret	
nop
nop
nop
	ret	0x1234
nop
nop
nop
	retf	
nop
nop
nop
	retf	0x1234
nop
nop
nop
	test	al,5
nop
nop
nop
	test	ax,0x1234
nop
nop
nop
	test	eax,0x12345678
nop
nop
nop
	test	bl,[ecx]
nop
nop
nop
	test	bx,[ecx]
nop
nop
nop
	test	ebx,[ecx]
nop
nop
nop
	test	bl,byte [ecx]
nop
nop
nop
	test	bx,word [ecx]
nop
nop
nop
	test	ebx,dword [ecx]
nop
nop
nop
	test	[ecx],bl
nop
nop
nop
	test	[ecx],bx
nop
nop
nop
	test	[ecx],ebx
nop
nop
nop
	test	byte [ecx],bl
nop
nop
nop
	test	word [ecx],bx
nop
nop
nop
	test	dword [ecx],ebx
nop
nop
nop
	test	bl,44
nop
nop
nop
	test	bx,44
nop
nop
nop
	test	ebx,44
nop
nop
nop
	test	bl,byte 44
nop
nop
nop
	test	bx,word 44
nop
nop
nop
	test	ebx,dword 44
nop
nop
nop
	test	byte [ecx],44
nop
nop
nop
	test	word [ecx],44
nop
nop
nop
	test	dword [ecx],44
nop
nop
nop
	test	byte [ecx],byte 44
nop
nop
nop
	test	word [ecx],word 44
nop
nop
nop
	test	dword [ecx],dword 44
nop
nop
nop
	test	bl, cl
nop
nop
nop
	test	bl, al
nop
nop
nop
	test	bx, cx
nop
nop
nop
	test	ebx, ecx
nop
nop
nop
	xchg	ax,cx
nop
nop
nop
	xchg	eax,ecx
nop
nop
nop
	xchg	cx,ax
nop
nop
nop
	xchg	ecx,eax
nop
nop
nop
	xchg	bl,[ecx]
nop
nop
nop
	xchg	bx,[ecx]
nop
nop
nop
	xchg	ebx,[ecx]
nop
nop
nop
	xchg	bl, al
nop
nop
nop
	xchg	al, bl
nop
nop
nop
	xchg	bl,byte [ecx]
nop
nop
nop
	xchg	bx,word [ecx]
nop
nop
nop
	xchg	ebx,dword [ecx]
nop
nop
nop
	xchg	[ecx],bl
nop
nop
nop
	xchg	[ecx],bx
nop
nop
nop
	xchg	[ecx],ebx
nop
nop
nop
	xlat	
nop
nop
nop
cld
nop
nop
[bits 64]
	cbw	
nop
nop
nop
	cdq	
nop
nop
nop
	clc	
nop
nop
nop
	cld	
nop
nop
nop
	cli	
nop
nop
nop
	clts	
nop
nop
nop
	cmc	
nop
nop
nop
	cpuid	
nop
nop
nop
	cwd	
nop
nop
nop
	cwde	
nop
nop
nop
	hlt	
nop
nop
nop
	icebp	
nop
nop
nop
	int1	
nop
nop
nop
	int3	
nop
nop
nop
	invd	
nop
nop
nop
	iret	
nop
nop
nop
	iretw	
nop
nop
nop
	iretd	
nop
nop
nop
	lfence	
nop
nop
nop
	popf	
nop
nop
nop
	pushf	
nop
nop
nop
	rsm	
nop
nop
nop
	lahf	
nop
nop
nop
	sahf	
nop
nop
nop
	sfence	
nop
nop
nop
	stc	
nop
nop
nop
	std	
nop
nop
nop
	sti	
nop
nop
nop
	syscall	
nop
nop
nop
	sysenter	
nop
nop
nop
	sysexit	
nop
nop
nop
	sysret	
nop
nop
nop
	ud2	
nop
nop
nop
	wait	
nop
nop
nop
	wbinvd	
nop
nop
nop
	wrmsr	
nop
nop
nop
	xsetbv	
nop
nop
nop
	cdqe	
nop
nop
nop
	cqo	
nop
nop
nop
	iretq	
nop
nop
nop
	popfq	
nop
nop
nop
	pushfq	
nop
nop
nop
	adc	al,5
nop
nop
nop
	adc	ax,0x1234
nop
nop
nop
	adc	eax,0x12345678
nop
nop
nop
	adc	rax,0x12345678
nop
nop
nop
	adc	al,byte 5
nop
nop
nop
	adc	ax,word 0x1234
nop
nop
nop
	adc	eax,dword 0x12345678
nop
nop
nop
	adc	rax,qword 0x12345678
nop
nop
nop
	adc	bl,[ecx]
nop
nop
nop
	adc	dil,[ecx]
nop
nop
nop
	adc	r15b,[ecx]
nop
nop
nop
	adc	bx,[ecx]
nop
nop
nop
	adc	r14w,[ecx]
nop
nop
nop
	adc	ebx,[ecx]
nop
nop
nop
	adc	r14d,[ecx]
nop
nop
nop
	adc	rbx,[ecx]
nop
nop
nop
	adc	r14,[ecx]
nop
nop
nop
	adc	bl,byte [ecx]
nop
nop
nop
	adc	dil,byte [ecx]
nop
nop
nop
	adc	r15b,byte [ecx]
nop
nop
nop
	adc	bx,word [ecx]
nop
nop
nop
	adc	r14w,word [ecx]
nop
nop
nop
	adc	ebx,dword [ecx]
nop
nop
nop
	adc	r14d,dword [ecx]
nop
nop
nop
	adc	rbx,qword [ecx]
nop
nop
nop
	adc	r14,qword [ecx]
nop
nop
nop
	adc	[ecx],bl
nop
nop
nop
	adc	[ecx],dil
nop
nop
nop
	adc	[ecx],r15b
nop
nop
nop
	adc	[ecx],bx
nop
nop
nop
	adc	[ecx],r14w
nop
nop
nop
	adc	[ecx],ebx
nop
nop
nop
	adc	[ecx],r14d
nop
nop
nop
	adc	[ecx],rbx
nop
nop
nop
	adc	[ecx],r14
nop
nop
nop
	adc	byte [ecx],bl
nop
nop
nop
	adc	byte [ecx],dil
nop
nop
nop
	adc	byte [ecx],r15b
nop
nop
nop
	adc	word [ecx],bx
nop
nop
nop
	adc	word [ecx],r14w
nop
nop
nop
	adc	dword [ecx],ebx
nop
nop
nop
	adc	dword [ecx],r14d
nop
nop
nop
	adc	qword [ecx],rbx
nop
nop
nop
	adc	qword [ecx],r14
nop
nop
nop
	adc	byte [ecx],44
nop
nop
nop
	adc	word [ecx],44
nop
nop
nop
	adc	dword [ecx],44
nop
nop
nop
	adc	qword [ecx],44
nop
nop
nop
	adc	byte [ecx],byte 44
nop
nop
nop
	adc	word [ecx],byte 44
nop
nop
nop
	adc	dword [ecx],byte 44
nop
nop
nop
	adc	qword [ecx],byte 44
nop
nop
nop
	adc	byte [ecx],byte 44
nop
nop
nop
	adc	word [ecx],word 44
nop
nop
nop
	adc	dword [ecx],dword 44
nop
nop
nop
	adc	qword [ecx],qword 44
nop
nop
nop
	adc	bl,44
nop
nop
nop
	adc	dil,44
nop
nop
nop
	adc	r15b,44
nop
nop
nop
	adc	bx,44
nop
nop
nop
	adc	r14w,44
nop
nop
nop
	adc	ebx,44
nop
nop
nop
	adc	r14d,44
nop
nop
nop
	adc	rbx,44
nop
nop
nop
	adc	r14,44
nop
nop
nop
	adc	bl,byte 44
nop
nop
nop
	adc	dil,byte 44
nop
nop
nop
	adc	r15b,byte 44
nop
nop
nop
	adc	bx,byte 44
nop
nop
nop
	adc	r14w,byte 44
nop
nop
nop
	adc	ebx,byte 44
nop
nop
nop
	adc	r14d,byte 44
nop
nop
nop
	adc	rbx,byte 44
nop
nop
nop
	adc	r14,byte 44
nop
nop
nop
	adc	bl,byte 44
nop
nop
nop
	adc	dil,byte 44
nop
nop
nop
	adc	r15b,byte 44
nop
nop
nop
	adc	bx,word 44
nop
nop
nop
	adc	r14w,word 44
nop
nop
nop
	adc	ebx,dword 44
nop
nop
nop
	adc	r14d,dword 44
nop
nop
nop
	adc	rbx,qword 44
nop
nop
nop
	adc	r14,qword 44
nop
nop
nop
	add	al,5
nop
nop
nop
	add	ax,0x1234
nop
nop
nop
	add	eax,0x12345678
nop
nop
nop
	add	rax,0x12345678
nop
nop
nop
	add	al,byte 5
nop
nop
nop
	add	ax,word 0x1234
nop
nop
nop
	add	eax,dword 0x12345678
nop
nop
nop
	add	rax,qword 0x12345678
nop
nop
nop
	add	bl,[ecx]
nop
nop
nop
	add	dil,[ecx]
nop
nop
nop
	add	r15b,[ecx]
nop
nop
nop
	add	bx,[ecx]
nop
nop
nop
	add	r14w,[ecx]
nop
nop
nop
	add	ebx,[ecx]
nop
nop
nop
	add	r14d,[ecx]
nop
nop
nop
	add	rbx,[ecx]
nop
nop
nop
	add	r14,[ecx]
nop
nop
nop
	add	bl,byte [ecx]
nop
nop
nop
	add	dil,byte [ecx]
nop
nop
nop
	add	r15b,byte [ecx]
nop
nop
nop
	add	bx,word [ecx]
nop
nop
nop
	add	r14w,word [ecx]
nop
nop
nop
	add	ebx,dword [ecx]
nop
nop
nop
	add	r14d,dword [ecx]
nop
nop
nop
	add	rbx,qword [ecx]
nop
nop
nop
	add	r14,qword [ecx]
nop
nop
nop
	add	[ecx],bl
nop
nop
nop
	add	[ecx],dil
nop
nop
nop
	add	[ecx],r15b
nop
nop
nop
	add	[ecx],bx
nop
nop
nop
	add	[ecx],r14w
nop
nop
nop
	add	[ecx],ebx
nop
nop
nop
	add	[ecx],r14d
nop
nop
nop
	add	[ecx],rbx
nop
nop
nop
	add	[ecx],r14
nop
nop
nop
	add	byte [ecx],bl
nop
nop
nop
	add	byte [ecx],dil
nop
nop
nop
	add	byte [ecx],r15b
nop
nop
nop
	add	word [ecx],bx
nop
nop
nop
	add	word [ecx],r14w
nop
nop
nop
	add	dword [ecx],ebx
nop
nop
nop
	add	dword [ecx],r14d
nop
nop
nop
	add	qword [ecx],rbx
nop
nop
nop
	add	qword [ecx],r14
nop
nop
nop
	add	byte [ecx],44
nop
nop
nop
	add	word [ecx],44
nop
nop
nop
	add	dword [ecx],44
nop
nop
nop
	add	qword [ecx],44
nop
nop
nop
	add	byte [ecx],byte 44
nop
nop
nop
	add	word [ecx],byte 44
nop
nop
nop
	add	dword [ecx],byte 44
nop
nop
nop
	add	qword [ecx],byte 44
nop
nop
nop
	add	byte [ecx],byte 44
nop
nop
nop
	add	word [ecx],word 44
nop
nop
nop
	add	dword [ecx],dword 44
nop
nop
nop
	add	qword [ecx],qword 44
nop
nop
nop
	add	bl,44
nop
nop
nop
	add	dil,44
nop
nop
nop
	add	r15b,44
nop
nop
nop
	add	bx,44
nop
nop
nop
	add	r14w,44
nop
nop
nop
	add	ebx,44
nop
nop
nop
	add	r14d,44
nop
nop
nop
	add	rbx,44
nop
nop
nop
	add	r14,44
nop
nop
nop
	add	bl,byte 44
nop
nop
nop
	add	dil,byte 44
nop
nop
nop
	add	r15b,byte 44
nop
nop
nop
	add	bx,byte 44
nop
nop
nop
	add	r14w,byte 44
nop
nop
nop
	add	ebx,byte 44
nop
nop
nop
	add	r14d,byte 44
nop
nop
nop
	add	rbx,byte 44
nop
nop
nop
	add	r14,byte 44
nop
nop
nop
	add	bl,byte 44
nop
nop
nop
	add	dil,byte 44
nop
nop
nop
	add	r15b,byte 44
nop
nop
nop
	add	bx,word 44
nop
nop
nop
	add	r14w,word 44
nop
nop
nop
	add	ebx,dword 44
nop
nop
nop
	add	r14d,dword 44
nop
nop
nop
	add	rbx,qword 44
nop
nop
nop
	add	r14,qword 44
nop
nop
nop
	and	al,5
nop
nop
nop
	and	ax,0x1234
nop
nop
nop
	and	eax,0x12345678
nop
nop
nop
	and	rax,0x12345678
nop
nop
nop
	and	al,byte 5
nop
nop
nop
	and	ax,word 0x1234
nop
nop
nop
	and	eax,dword 0x12345678
nop
nop
nop
	and	rax,qword 0x12345678
nop
nop
nop
	and	bl,[ecx]
nop
nop
nop
	and	dil,[ecx]
nop
nop
nop
	and	r15b,[ecx]
nop
nop
nop
	and	bx,[ecx]
nop
nop
nop
	and	r14w,[ecx]
nop
nop
nop
	and	ebx,[ecx]
nop
nop
nop
	and	r14d,[ecx]
nop
nop
nop
	and	rbx,[ecx]
nop
nop
nop
	and	r14,[ecx]
nop
nop
nop
	and	bl,byte [ecx]
nop
nop
nop
	and	dil,byte [ecx]
nop
nop
nop
	and	r15b,byte [ecx]
nop
nop
nop
	and	bx,word [ecx]
nop
nop
nop
	and	r14w,word [ecx]
nop
nop
nop
	and	ebx,dword [ecx]
nop
nop
nop
	and	r14d,dword [ecx]
nop
nop
nop
	and	rbx,qword [ecx]
nop
nop
nop
	and	r14,qword [ecx]
nop
nop
nop
	and	[ecx],bl
nop
nop
nop
	and	[ecx],dil
nop
nop
nop
	and	[ecx],r15b
nop
nop
nop
	and	[ecx],bx
nop
nop
nop
	and	[ecx],r14w
nop
nop
nop
	and	[ecx],ebx
nop
nop
nop
	and	[ecx],r14d
nop
nop
nop
	and	[ecx],rbx
nop
nop
nop
	and	[ecx],r14
nop
nop
nop
	and	byte [ecx],bl
nop
nop
nop
	and	byte [ecx],dil
nop
nop
nop
	and	byte [ecx],r15b
nop
nop
nop
	and	word [ecx],bx
nop
nop
nop
	and	word [ecx],r14w
nop
nop
nop
	and	dword [ecx],ebx
nop
nop
nop
	and	dword [ecx],r14d
nop
nop
nop
	and	qword [ecx],rbx
nop
nop
nop
	and	qword [ecx],r14
nop
nop
nop
	and	byte [ecx],44
nop
nop
nop
	and	word [ecx],44
nop
nop
nop
	and	dword [ecx],44
nop
nop
nop
	and	qword [ecx],44
nop
nop
nop
	and	byte [ecx],byte 44
nop
nop
nop
	and	word [ecx],byte 44
nop
nop
nop
	and	dword [ecx],byte 44
nop
nop
nop
	and	qword [ecx],byte 44
nop
nop
nop
	and	byte [ecx],byte 44
nop
nop
nop
	and	word [ecx],word 44
nop
nop
nop
	and	dword [ecx],dword 44
nop
nop
nop
	and	qword [ecx],qword 44
nop
nop
nop
	and	bl,44
nop
nop
nop
	and	dil,44
nop
nop
nop
	and	r15b,44
nop
nop
nop
	and	bx,44
nop
nop
nop
	and	r14w,44
nop
nop
nop
	and	ebx,44
nop
nop
nop
	and	r14d,44
nop
nop
nop
	and	rbx,44
nop
nop
nop
	and	r14,44
nop
nop
nop
	and	bl,byte 44
nop
nop
nop
	and	dil,byte 44
nop
nop
nop
	and	r15b,byte 44
nop
nop
nop
	and	bx,byte 44
nop
nop
nop
	and	r14w,byte 44
nop
nop
nop
	and	ebx,byte 44
nop
nop
nop
	and	r14d,byte 44
nop
nop
nop
	and	rbx,byte 44
nop
nop
nop
	and	r14,byte 44
nop
nop
nop
	and	bl,byte 44
nop
nop
nop
	and	dil,byte 44
nop
nop
nop
	and	r15b,byte 44
nop
nop
nop
	and	bx,word 44
nop
nop
nop
	and	r14w,word 44
nop
nop
nop
	and	ebx,dword 44
nop
nop
nop
	and	r14d,dword 44
nop
nop
nop
	and	rbx,qword 44
nop
nop
nop
	and	r14,qword 44
nop
nop
nop
	cmp	al,5
nop
nop
nop
	cmp	ax,0x1234
nop
nop
nop
	cmp	eax,0x12345678
nop
nop
nop
	cmp	rax,0x12345678
nop
nop
nop
	cmp	al,byte 5
nop
nop
nop
	cmp	ax,word 0x1234
nop
nop
nop
	cmp	eax,dword 0x12345678
nop
nop
nop
	cmp	rax,qword 0x12345678
nop
nop
nop
	cmp	bl,[ecx]
nop
nop
nop
	cmp	dil,[ecx]
nop
nop
nop
	cmp	r15b,[ecx]
nop
nop
nop
	cmp	bx,[ecx]
nop
nop
nop
	cmp	r14w,[ecx]
nop
nop
nop
	cmp	ebx,[ecx]
nop
nop
nop
	cmp	r14d,[ecx]
nop
nop
nop
	cmp	rbx,[ecx]
nop
nop
nop
	cmp	r14,[ecx]
nop
nop
nop
	cmp	bl,byte [ecx]
nop
nop
nop
	cmp	dil,byte [ecx]
nop
nop
nop
	cmp	r15b,byte [ecx]
nop
nop
nop
	cmp	bx,word [ecx]
nop
nop
nop
	cmp	r14w,word [ecx]
nop
nop
nop
	cmp	ebx,dword [ecx]
nop
nop
nop
	cmp	r14d,dword [ecx]
nop
nop
nop
	cmp	rbx,qword [ecx]
nop
nop
nop
	cmp	r14,qword [ecx]
nop
nop
nop
	cmp	[ecx],bl
nop
nop
nop
	cmp	[ecx],dil
nop
nop
nop
	cmp	[ecx],r15b
nop
nop
nop
	cmp	[ecx],bx
nop
nop
nop
	cmp	[ecx],r14w
nop
nop
nop
	cmp	[ecx],ebx
nop
nop
nop
	cmp	[ecx],r14d
nop
nop
nop
	cmp	[ecx],rbx
nop
nop
nop
	cmp	[ecx],r14
nop
nop
nop
	cmp	byte [ecx],bl
nop
nop
nop
	cmp	byte [ecx],dil
nop
nop
nop
	cmp	byte [ecx],r15b
nop
nop
nop
	cmp	word [ecx],bx
nop
nop
nop
	cmp	word [ecx],r14w
nop
nop
nop
	cmp	dword [ecx],ebx
nop
nop
nop
	cmp	dword [ecx],r14d
nop
nop
nop
	cmp	qword [ecx],rbx
nop
nop
nop
	cmp	qword [ecx],r14
nop
nop
nop
	cmp	byte [ecx],44
nop
nop
nop
	cmp	word [ecx],44
nop
nop
nop
	cmp	dword [ecx],44
nop
nop
nop
	cmp	qword [ecx],44
nop
nop
nop
	cmp	byte [ecx],byte 44
nop
nop
nop
	cmp	word [ecx],byte 44
nop
nop
nop
	cmp	dword [ecx],byte 44
nop
nop
nop
	cmp	qword [ecx],byte 44
nop
nop
nop
	cmp	byte [ecx],byte 44
nop
nop
nop
	cmp	word [ecx],word 44
nop
nop
nop
	cmp	dword [ecx],dword 44
nop
nop
nop
	cmp	qword [ecx],qword 44
nop
nop
nop
	cmp	bl,44
nop
nop
nop
	cmp	dil,44
nop
nop
nop
	cmp	r15b,44
nop
nop
nop
	cmp	bx,44
nop
nop
nop
	cmp	r14w,44
nop
nop
nop
	cmp	ebx,44
nop
nop
nop
	cmp	r14d,44
nop
nop
nop
	cmp	rbx,44
nop
nop
nop
	cmp	r14,44
nop
nop
nop
	cmp	bl,byte 44
nop
nop
nop
	cmp	dil,byte 44
nop
nop
nop
	cmp	r15b,byte 44
nop
nop
nop
	cmp	bx,byte 44
nop
nop
nop
	cmp	r14w,byte 44
nop
nop
nop
	cmp	ebx,byte 44
nop
nop
nop
	cmp	r14d,byte 44
nop
nop
nop
	cmp	rbx,byte 44
nop
nop
nop
	cmp	r14,byte 44
nop
nop
nop
	cmp	bl,byte 44
nop
nop
nop
	cmp	dil,byte 44
nop
nop
nop
	cmp	r15b,byte 44
nop
nop
nop
	cmp	bx,word 44
nop
nop
nop
	cmp	r14w,word 44
nop
nop
nop
	cmp	ebx,dword 44
nop
nop
nop
	cmp	r14d,dword 44
nop
nop
nop
	cmp	rbx,qword 44
nop
nop
nop
	cmp	r14,qword 44
nop
nop
nop
	or	al,5
nop
nop
nop
	or	ax,0x1234
nop
nop
nop
	or	eax,0x12345678
nop
nop
nop
	or	rax,0x12345678
nop
nop
nop
	or	al,byte 5
nop
nop
nop
	or	ax,word 0x1234
nop
nop
nop
	or	eax,dword 0x12345678
nop
nop
nop
	or	rax,qword 0x12345678
nop
nop
nop
	or	bl,[ecx]
nop
nop
nop
	or	dil,[ecx]
nop
nop
nop
	or	r15b,[ecx]
nop
nop
nop
	or	bx,[ecx]
nop
nop
nop
	or	r14w,[ecx]
nop
nop
nop
	or	ebx,[ecx]
nop
nop
nop
	or	r14d,[ecx]
nop
nop
nop
	or	rbx,[ecx]
nop
nop
nop
	or	r14,[ecx]
nop
nop
nop
	or	bl,byte [ecx]
nop
nop
nop
	or	dil,byte [ecx]
nop
nop
nop
	or	r15b,byte [ecx]
nop
nop
nop
	or	bx,word [ecx]
nop
nop
nop
	or	r14w,word [ecx]
nop
nop
nop
	or	ebx,dword [ecx]
nop
nop
nop
	or	r14d,dword [ecx]
nop
nop
nop
	or	rbx,qword [ecx]
nop
nop
nop
	or	r14,qword [ecx]
nop
nop
nop
	or	[ecx],bl
nop
nop
nop
	or	[ecx],dil
nop
nop
nop
	or	[ecx],r15b
nop
nop
nop
	or	[ecx],bx
nop
nop
nop
	or	[ecx],r14w
nop
nop
nop
	or	[ecx],ebx
nop
nop
nop
	or	[ecx],r14d
nop
nop
nop
	or	[ecx],rbx
nop
nop
nop
	or	[ecx],r14
nop
nop
nop
	or	byte [ecx],bl
nop
nop
nop
	or	byte [ecx],dil
nop
nop
nop
	or	byte [ecx],r15b
nop
nop
nop
	or	word [ecx],bx
nop
nop
nop
	or	word [ecx],r14w
nop
nop
nop
	or	dword [ecx],ebx
nop
nop
nop
	or	dword [ecx],r14d
nop
nop
nop
	or	qword [ecx],rbx
nop
nop
nop
	or	qword [ecx],r14
nop
nop
nop
	or	byte [ecx],44
nop
nop
nop
	or	word [ecx],44
nop
nop
nop
	or	dword [ecx],44
nop
nop
nop
	or	qword [ecx],44
nop
nop
nop
	or	byte [ecx],byte 44
nop
nop
nop
	or	word [ecx],byte 44
nop
nop
nop
	or	dword [ecx],byte 44
nop
nop
nop
	or	qword [ecx],byte 44
nop
nop
nop
	or	byte [ecx],byte 44
nop
nop
nop
	or	word [ecx],word 44
nop
nop
nop
	or	dword [ecx],dword 44
nop
nop
nop
	or	qword [ecx],qword 44
nop
nop
nop
	or	bl,44
nop
nop
nop
	or	dil,44
nop
nop
nop
	or	r15b,44
nop
nop
nop
	or	bx,44
nop
nop
nop
	or	r14w,44
nop
nop
nop
	or	ebx,44
nop
nop
nop
	or	r14d,44
nop
nop
nop
	or	rbx,44
nop
nop
nop
	or	r14,44
nop
nop
nop
	or	bl,byte 44
nop
nop
nop
	or	dil,byte 44
nop
nop
nop
	or	r15b,byte 44
nop
nop
nop
	or	bx,byte 44
nop
nop
nop
	or	r14w,byte 44
nop
nop
nop
	or	ebx,byte 44
nop
nop
nop
	or	r14d,byte 44
nop
nop
nop
	or	rbx,byte 44
nop
nop
nop
	or	r14,byte 44
nop
nop
nop
	or	bl,byte 44
nop
nop
nop
	or	dil,byte 44
nop
nop
nop
	or	r15b,byte 44
nop
nop
nop
	or	bx,word 44
nop
nop
nop
	or	r14w,word 44
nop
nop
nop
	or	ebx,dword 44
nop
nop
nop
	or	r14d,dword 44
nop
nop
nop
	or	rbx,qword 44
nop
nop
nop
	or	r14,qword 44
nop
nop
nop
	sbb	al,5
nop
nop
nop
	sbb	ax,0x1234
nop
nop
nop
	sbb	eax,0x12345678
nop
nop
nop
	sbb	rax,0x12345678
nop
nop
nop
	sbb	al,byte 5
nop
nop
nop
	sbb	ax,word 0x1234
nop
nop
nop
	sbb	eax,dword 0x12345678
nop
nop
nop
	sbb	rax,qword 0x12345678
nop
nop
nop
	sbb	bl,[ecx]
nop
nop
nop
	sbb	dil,[ecx]
nop
nop
nop
	sbb	r15b,[ecx]
nop
nop
nop
	sbb	bx,[ecx]
nop
nop
nop
	sbb	r14w,[ecx]
nop
nop
nop
	sbb	ebx,[ecx]
nop
nop
nop
	sbb	r14d,[ecx]
nop
nop
nop
	sbb	rbx,[ecx]
nop
nop
nop
	sbb	r14,[ecx]
nop
nop
nop
	sbb	bl,byte [ecx]
nop
nop
nop
	sbb	dil,byte [ecx]
nop
nop
nop
	sbb	r15b,byte [ecx]
nop
nop
nop
	sbb	bx,word [ecx]
nop
nop
nop
	sbb	r14w,word [ecx]
nop
nop
nop
	sbb	ebx,dword [ecx]
nop
nop
nop
	sbb	r14d,dword [ecx]
nop
nop
nop
	sbb	rbx,qword [ecx]
nop
nop
nop
	sbb	r14,qword [ecx]
nop
nop
nop
	sbb	[ecx],bl
nop
nop
nop
	sbb	[ecx],dil
nop
nop
nop
	sbb	[ecx],r15b
nop
nop
nop
	sbb	[ecx],bx
nop
nop
nop
	sbb	[ecx],r14w
nop
nop
nop
	sbb	[ecx],ebx
nop
nop
nop
	sbb	[ecx],r14d
nop
nop
nop
	sbb	[ecx],rbx
nop
nop
nop
	sbb	[ecx],r14
nop
nop
nop
	sbb	byte [ecx],bl
nop
nop
nop
	sbb	byte [ecx],dil
nop
nop
nop
	sbb	byte [ecx],r15b
nop
nop
nop
	sbb	word [ecx],bx
nop
nop
nop
	sbb	word [ecx],r14w
nop
nop
nop
	sbb	dword [ecx],ebx
nop
nop
nop
	sbb	dword [ecx],r14d
nop
nop
nop
	sbb	qword [ecx],rbx
nop
nop
nop
	sbb	qword [ecx],r14
nop
nop
nop
	sbb	byte [ecx],44
nop
nop
nop
	sbb	word [ecx],44
nop
nop
nop
	sbb	dword [ecx],44
nop
nop
nop
	sbb	qword [ecx],44
nop
nop
nop
	sbb	byte [ecx],byte 44
nop
nop
nop
	sbb	word [ecx],byte 44
nop
nop
nop
	sbb	dword [ecx],byte 44
nop
nop
nop
	sbb	qword [ecx],byte 44
nop
nop
nop
	sbb	byte [ecx],byte 44
nop
nop
nop
	sbb	word [ecx],word 44
nop
nop
nop
	sbb	dword [ecx],dword 44
nop
nop
nop
	sbb	qword [ecx],qword 44
nop
nop
nop
	sbb	bl,44
nop
nop
nop
	sbb	dil,44
nop
nop
nop
	sbb	r15b,44
nop
nop
nop
	sbb	bx,44
nop
nop
nop
	sbb	r14w,44
nop
nop
nop
	sbb	ebx,44
nop
nop
nop
	sbb	r14d,44
nop
nop
nop
	sbb	rbx,44
nop
nop
nop
	sbb	r14,44
nop
nop
nop
	sbb	bl,byte 44
nop
nop
nop
	sbb	dil,byte 44
nop
nop
nop
	sbb	r15b,byte 44
nop
nop
nop
	sbb	bx,byte 44
nop
nop
nop
	sbb	r14w,byte 44
nop
nop
nop
	sbb	ebx,byte 44
nop
nop
nop
	sbb	r14d,byte 44
nop
nop
nop
	sbb	rbx,byte 44
nop
nop
nop
	sbb	r14,byte 44
nop
nop
nop
	sbb	bl,byte 44
nop
nop
nop
	sbb	dil,byte 44
nop
nop
nop
	sbb	r15b,byte 44
nop
nop
nop
	sbb	bx,word 44
nop
nop
nop
	sbb	r14w,word 44
nop
nop
nop
	sbb	ebx,dword 44
nop
nop
nop
	sbb	r14d,dword 44
nop
nop
nop
	sbb	rbx,qword 44
nop
nop
nop
	sbb	r14,qword 44
nop
nop
nop
	sub	al,5
nop
nop
nop
	sub	ax,0x1234
nop
nop
nop
	sub	eax,0x12345678
nop
nop
nop
	sub	rax,0x12345678
nop
nop
nop
	sub	al,byte 5
nop
nop
nop
	sub	ax,word 0x1234
nop
nop
nop
	sub	eax,dword 0x12345678
nop
nop
nop
	sub	rax,qword 0x12345678
nop
nop
nop
	sub	bl,[ecx]
nop
nop
nop
	sub	dil,[ecx]
nop
nop
nop
	sub	r15b,[ecx]
nop
nop
nop
	sub	bx,[ecx]
nop
nop
nop
	sub	r14w,[ecx]
nop
nop
nop
	sub	ebx,[ecx]
nop
nop
nop
	sub	r14d,[ecx]
nop
nop
nop
	sub	rbx,[ecx]
nop
nop
nop
	sub	r14,[ecx]
nop
nop
nop
	sub	bl,byte [ecx]
nop
nop
nop
	sub	dil,byte [ecx]
nop
nop
nop
	sub	r15b,byte [ecx]
nop
nop
nop
	sub	bx,word [ecx]
nop
nop
nop
	sub	r14w,word [ecx]
nop
nop
nop
	sub	ebx,dword [ecx]
nop
nop
nop
	sub	r14d,dword [ecx]
nop
nop
nop
	sub	rbx,qword [ecx]
nop
nop
nop
	sub	r14,qword [ecx]
nop
nop
nop
	sub	[ecx],bl
nop
nop
nop
	sub	[ecx],dil
nop
nop
nop
	sub	[ecx],r15b
nop
nop
nop
	sub	[ecx],bx
nop
nop
nop
	sub	[ecx],r14w
nop
nop
nop
	sub	[ecx],ebx
nop
nop
nop
	sub	[ecx],r14d
nop
nop
nop
	sub	[ecx],rbx
nop
nop
nop
	sub	[ecx],r14
nop
nop
nop
	sub	byte [ecx],bl
nop
nop
nop
	sub	byte [ecx],dil
nop
nop
nop
	sub	byte [ecx],r15b
nop
nop
nop
	sub	word [ecx],bx
nop
nop
nop
	sub	word [ecx],r14w
nop
nop
nop
	sub	dword [ecx],ebx
nop
nop
nop
	sub	dword [ecx],r14d
nop
nop
nop
	sub	qword [ecx],rbx
nop
nop
nop
	sub	qword [ecx],r14
nop
nop
nop
	sub	byte [ecx],44
nop
nop
nop
	sub	word [ecx],44
nop
nop
nop
	sub	dword [ecx],44
nop
nop
nop
	sub	qword [ecx],44
nop
nop
nop
	sub	byte [ecx],byte 44
nop
nop
nop
	sub	word [ecx],byte 44
nop
nop
nop
	sub	dword [ecx],byte 44
nop
nop
nop
	sub	qword [ecx],byte 44
nop
nop
nop
	sub	byte [ecx],byte 44
nop
nop
nop
	sub	word [ecx],word 44
nop
nop
nop
	sub	dword [ecx],dword 44
nop
nop
nop
	sub	qword [ecx],qword 44
nop
nop
nop
	sub	bl,44
nop
nop
nop
	sub	dil,44
nop
nop
nop
	sub	r15b,44
nop
nop
nop
	sub	bx,44
nop
nop
nop
	sub	r14w,44
nop
nop
nop
	sub	ebx,44
nop
nop
nop
	sub	r14d,44
nop
nop
nop
	sub	rbx,44
nop
nop
nop
	sub	r14,44
nop
nop
nop
	sub	bl,byte 44
nop
nop
nop
	sub	dil,byte 44
nop
nop
nop
	sub	r15b,byte 44
nop
nop
nop
	sub	bx,byte 44
nop
nop
nop
	sub	r14w,byte 44
nop
nop
nop
	sub	ebx,byte 44
nop
nop
nop
	sub	r14d,byte 44
nop
nop
nop
	sub	rbx,byte 44
nop
nop
nop
	sub	r14,byte 44
nop
nop
nop
	sub	bl,byte 44
nop
nop
nop
	sub	dil,byte 44
nop
nop
nop
	sub	r15b,byte 44
nop
nop
nop
	sub	bx,word 44
nop
nop
nop
	sub	r14w,word 44
nop
nop
nop
	sub	ebx,dword 44
nop
nop
nop
	sub	r14d,dword 44
nop
nop
nop
	sub	rbx,qword 44
nop
nop
nop
	sub	r14,qword 44
nop
nop
nop
	xor	al,5
nop
nop
nop
	xor	ax,0x1234
nop
nop
nop
	xor	eax,0x12345678
nop
nop
nop
	xor	rax,0x12345678
nop
nop
nop
	xor	al,byte 5
nop
nop
nop
	xor	ax,word 0x1234
nop
nop
nop
	xor	eax,dword 0x12345678
nop
nop
nop
	xor	rax,qword 0x12345678
nop
nop
nop
	xor	bl,[ecx]
nop
nop
nop
	xor	dil,[ecx]
nop
nop
nop
	xor	r15b,[ecx]
nop
nop
nop
	xor	bx,[ecx]
nop
nop
nop
	xor	r14w,[ecx]
nop
nop
nop
	xor	ebx,[ecx]
nop
nop
nop
	xor	r14d,[ecx]
nop
nop
nop
	xor	rbx,[ecx]
nop
nop
nop
	xor	r14,[ecx]
nop
nop
nop
	xor	bl,byte [ecx]
nop
nop
nop
	xor	dil,byte [ecx]
nop
nop
nop
	xor	r15b,byte [ecx]
nop
nop
nop
	xor	bx,word [ecx]
nop
nop
nop
	xor	r14w,word [ecx]
nop
nop
nop
	xor	ebx,dword [ecx]
nop
nop
nop
	xor	r14d,dword [ecx]
nop
nop
nop
	xor	rbx,qword [ecx]
nop
nop
nop
	xor	r14,qword [ecx]
nop
nop
nop
	xor	[ecx],bl
nop
nop
nop
	xor	[ecx],dil
nop
nop
nop
	xor	[ecx],r15b
nop
nop
nop
	xor	[ecx],bx
nop
nop
nop
	xor	[ecx],r14w
nop
nop
nop
	xor	[ecx],ebx
nop
nop
nop
	xor	[ecx],r14d
nop
nop
nop
	xor	[ecx],rbx
nop
nop
nop
	xor	[ecx],r14
nop
nop
nop
	xor	byte [ecx],bl
nop
nop
nop
	xor	byte [ecx],dil
nop
nop
nop
	xor	byte [ecx],r15b
nop
nop
nop
	xor	word [ecx],bx
nop
nop
nop
	xor	word [ecx],r14w
nop
nop
nop
	xor	dword [ecx],ebx
nop
nop
nop
	xor	dword [ecx],r14d
nop
nop
nop
	xor	qword [ecx],rbx
nop
nop
nop
	xor	qword [ecx],r14
nop
nop
nop
	xor	byte [ecx],44
nop
nop
nop
	xor	word [ecx],44
nop
nop
nop
	xor	dword [ecx],44
nop
nop
nop
	xor	qword [ecx],44
nop
nop
nop
	xor	byte [ecx],byte 44
nop
nop
nop
	xor	word [ecx],byte 44
nop
nop
nop
	xor	dword [ecx],byte 44
nop
nop
nop
	xor	qword [ecx],byte 44
nop
nop
nop
	xor	byte [ecx],byte 44
nop
nop
nop
	xor	word [ecx],word 44
nop
nop
nop
	xor	dword [ecx],dword 44
nop
nop
nop
	xor	qword [ecx],qword 44
nop
nop
nop
	xor	bl,44
nop
nop
nop
	xor	dil,44
nop
nop
nop
	xor	r15b,44
nop
nop
nop
	xor	bx,44
nop
nop
nop
	xor	r14w,44
nop
nop
nop
	xor	ebx,44
nop
nop
nop
	xor	r14d,44
nop
nop
nop
	xor	rbx,44
nop
nop
nop
	xor	r14,44
nop
nop
nop
	xor	bl,byte 44
nop
nop
nop
	xor	dil,byte 44
nop
nop
nop
	xor	r15b,byte 44
nop
nop
nop
	xor	bx,byte 44
nop
nop
nop
	xor	r14w,byte 44
nop
nop
nop
	xor	ebx,byte 44
nop
nop
nop
	xor	r14d,byte 44
nop
nop
nop
	xor	rbx,byte 44
nop
nop
nop
	xor	r14,byte 44
nop
nop
nop
	xor	bl,byte 44
nop
nop
nop
	xor	dil,byte 44
nop
nop
nop
	xor	r15b,byte 44
nop
nop
nop
	xor	bx,word 44
nop
nop
nop
	xor	r14w,word 44
nop
nop
nop
	xor	ebx,dword 44
nop
nop
nop
	xor	r14d,dword 44
nop
nop
nop
	xor	rbx,qword 44
nop
nop
nop
	xor	r14,qword 44
nop
nop
nop
	bt	word [ecx],44
nop
nop
nop
	bt	dword [ecx],44
nop
nop
nop
	bt	qword [ecx],44
nop
nop
nop
	bt	word [ecx],bx
nop
nop
nop
	bt	word [ecx],r14w
nop
nop
nop
	bt	dword [ecx],ebx
nop
nop
nop
	bt	dword [ecx],r14d
nop
nop
nop
	bt	qword [ecx],rbx
nop
nop
nop
	bt	qword [ecx],r14
nop
nop
nop
	btc	word [ecx],44
nop
nop
nop
	btc	dword [ecx],44
nop
nop
nop
	btc	qword [ecx],44
nop
nop
nop
	btc	word [ecx],bx
nop
nop
nop
	btc	word [ecx],r14w
nop
nop
nop
	btc	dword [ecx],ebx
nop
nop
nop
	btc	dword [ecx],r14d
nop
nop
nop
	btc	qword [ecx],rbx
nop
nop
nop
	btc	qword [ecx],r14
nop
nop
nop
	btr	word [ecx],44
nop
nop
nop
	btr	dword [ecx],44
nop
nop
nop
	btr	qword [ecx],44
nop
nop
nop
	btr	word [ecx],bx
nop
nop
nop
	btr	word [ecx],r14w
nop
nop
nop
	btr	dword [ecx],ebx
nop
nop
nop
	btr	dword [ecx],r14d
nop
nop
nop
	btr	qword [ecx],rbx
nop
nop
nop
	btr	qword [ecx],r14
nop
nop
nop
	bts	word [ecx],44
nop
nop
nop
	bts	dword [ecx],44
nop
nop
nop
	bts	qword [ecx],44
nop
nop
nop
	bts	word [ecx],bx
nop
nop
nop
	bts	word [ecx],r14w
nop
nop
nop
	bts	dword [ecx],ebx
nop
nop
nop
	bts	dword [ecx],r14d
nop
nop
nop
	bts	qword [ecx],rbx
nop
nop
nop
	bts	qword [ecx],r14
nop
nop
nop
	inc	bl
nop
nop
nop
	inc	dil
nop
nop
nop
	inc	r15b
nop
nop
nop
	inc	bx
nop
nop
nop
	inc	r14w
nop
nop
nop
	inc	ebx
nop
nop
nop
	inc	r14d
nop
nop
nop
	inc	rbx
nop
nop
nop
	inc	r14
nop
nop
nop
	inc	byte [ecx]
nop
nop
nop
	inc	word [ecx]
nop
nop
nop
	inc	dword [ecx]
nop
nop
nop
	inc	qword [ecx]
nop
nop
nop
	dec	bl
nop
nop
nop
	dec	dil
nop
nop
nop
	dec	r15b
nop
nop
nop
	dec	bx
nop
nop
nop
	dec	r14w
nop
nop
nop
	dec	ebx
nop
nop
nop
	dec	r14d
nop
nop
nop
	dec	rbx
nop
nop
nop
	dec	r14
nop
nop
nop
	dec	byte [ecx]
nop
nop
nop
	dec	word [ecx]
nop
nop
nop
	dec	dword [ecx]
nop
nop
nop
	dec	qword [ecx]
nop
nop
nop
	neg	bl
nop
nop
nop
	neg	dil
nop
nop
nop
	neg	r15b
nop
nop
nop
	neg	bx
nop
nop
nop
	neg	r14w
nop
nop
nop
	neg	ebx
nop
nop
nop
	neg	r14d
nop
nop
nop
	neg	rbx
nop
nop
nop
	neg	r14
nop
nop
nop
	neg	byte [ecx]
nop
nop
nop
	neg	word [ecx]
nop
nop
nop
	neg	dword [ecx]
nop
nop
nop
	neg	qword [ecx]
nop
nop
nop
	not	bl
nop
nop
nop
	not	dil
nop
nop
nop
	not	r15b
nop
nop
nop
	not	bx
nop
nop
nop
	not	r14w
nop
nop
nop
	not	ebx
nop
nop
nop
	not	r14d
nop
nop
nop
	not	rbx
nop
nop
nop
	not	r14
nop
nop
nop
	not	byte [ecx]
nop
nop
nop
	not	word [ecx]
nop
nop
nop
	not	dword [ecx]
nop
nop
nop
	not	qword [ecx]
nop
nop
nop
	div	bl
nop
nop
nop
	div	dil
nop
nop
nop
	div	r15b
nop
nop
nop
	div	bx
nop
nop
nop
	div	r14w
nop
nop
nop
	div	ebx
nop
nop
nop
	div	r14d
nop
nop
nop
	div	rbx
nop
nop
nop
	div	r14
nop
nop
nop
	div	byte [ecx]
nop
nop
nop
	div	word [ecx]
nop
nop
nop
	div	dword [ecx]
nop
nop
nop
	div	qword [ecx]
nop
nop
nop
	idiv	bl
nop
nop
nop
	idiv	dil
nop
nop
nop
	idiv	r15b
nop
nop
nop
	idiv	bx
nop
nop
nop
	idiv	r14w
nop
nop
nop
	idiv	ebx
nop
nop
nop
	idiv	r14d
nop
nop
nop
	idiv	rbx
nop
nop
nop
	idiv	r14
nop
nop
nop
	idiv	byte [ecx]
nop
nop
nop
	idiv	word [ecx]
nop
nop
nop
	idiv	dword [ecx]
nop
nop
nop
	idiv	qword [ecx]
nop
nop
nop
	mul	bl
nop
nop
nop
	mul	dil
nop
nop
nop
	mul	r15b
nop
nop
nop
	mul	bx
nop
nop
nop
	mul	r14w
nop
nop
nop
	mul	ebx
nop
nop
nop
	mul	r14d
nop
nop
nop
	mul	rbx
nop
nop
nop
	mul	r14
nop
nop
nop
	mul	byte [ecx]
nop
nop
nop
	mul	word [ecx]
nop
nop
nop
	mul	dword [ecx]
nop
nop
nop
	mul	qword [ecx]
nop
nop
nop
	invlpg	[ecx]
nop
nop
nop
	ltr	[ecx]
nop
nop
nop
	prefetchnta	[ecx]
nop
nop
nop
	prefetcht0	[ecx]
nop
nop
nop
	prefetcht1	[ecx]
nop
nop
nop
	prefetcht2	[ecx]
nop
nop
nop
	xrstor	[ecx]
nop
nop
nop
	xsave	[ecx]
nop
nop
nop
	lmsw	bx
nop
nop
nop
	lmsw	r14w
nop
nop
nop
	smsw	bx
nop
nop
nop
	smsw	r14w
nop
nop
nop
	verr	bx
nop
nop
nop
	verr	r14w
nop
nop
nop
	verw	bx
nop
nop
nop
	verw	r14w
nop
nop
nop
	str	bx
nop
nop
nop
	str	r14w
nop
nop
nop
	str	ebx
nop
nop
nop
	str	r14d
nop
nop
nop
	str	rbx
nop
nop
nop
	str	r14
nop
nop
nop
	bswap	ebx
nop
nop
nop
	bswap	r14d
nop
nop
nop
	bswap	rbx
nop
nop
nop
	bswap	r14
nop
nop
nop
	xrstor64	[ecx]
nop
nop
nop
	xsave64	[ecx]
nop
nop
nop
	pop	rbx
nop
nop
nop
	pop	r14
nop
nop
nop
	pop	qword [ecx]
nop
nop
nop
	pop	fs
nop
nop
nop
	pop	gs
nop
nop
nop
	push	rbx
nop
nop
nop
	push	r14
nop
nop
nop
	push	qword [ecx]
nop
nop
nop
	push	fs
nop
nop
nop
	push	gs
nop
nop
nop
	push	44
nop
nop
nop
	push	-3
nop
nop
nop
	push	0x1234
nop
nop
nop
	push	byte 44
nop
nop
nop
	push	word 1234
nop
nop
nop
	push	dword 0x12345678
nop
nop
nop
	push	qword 0x12345678
nop
nop
nop
	call	[fs:0x4444]
nop
nop
nop
	call	 lbl202
	nop
	nop
lbl202: nop
	call	rbx
nop
nop
nop
	call	r14
nop
nop
nop
	call	qword [ecx]
nop
nop
nop
	jmp	[fs:0x4444]
nop
nop
nop
	jmp	 lbl203
	nop
	nop
lbl203: nop
	jmp	rbx
nop
nop
nop
	jmp	r14
nop
nop
nop
	jmp	qword [ecx]
nop
nop
nop
	jmp	short lbl204
	nop
	nop
lbl204: nop
	jmp	near lbl205
	nop
	nop
lbl205: nop
	ja	short lbl206
	nop
	nop
lbl206: nop
	ja	near lbl207
	nop
	nop
lbl207: nop
	ja	 lbl208
	nop
	nop
lbl208: nop
	jae	short lbl209
	nop
	nop
lbl209: nop
	jae	near lbl210
	nop
	nop
lbl210: nop
	jae	 lbl211
	nop
	nop
lbl211: nop
	jb	short lbl212
	nop
	nop
lbl212: nop
	jb	near lbl213
	nop
	nop
lbl213: nop
	jb	 lbl214
	nop
	nop
lbl214: nop
	jbe	short lbl215
	nop
	nop
lbl215: nop
	jbe	near lbl216
	nop
	nop
lbl216: nop
	jbe	 lbl217
	nop
	nop
lbl217: nop
	jc	short lbl218
	nop
	nop
lbl218: nop
	jc	near lbl219
	nop
	nop
lbl219: nop
	jc	 lbl220
	nop
	nop
lbl220: nop
	je	short lbl221
	nop
	nop
lbl221: nop
	je	near lbl222
	nop
	nop
lbl222: nop
	je	 lbl223
	nop
	nop
lbl223: nop
	jg	short lbl224
	nop
	nop
lbl224: nop
	jg	near lbl225
	nop
	nop
lbl225: nop
	jg	 lbl226
	nop
	nop
lbl226: nop
	jge	short lbl227
	nop
	nop
lbl227: nop
	jge	near lbl228
	nop
	nop
lbl228: nop
	jge	 lbl229
	nop
	nop
lbl229: nop
	jl	short lbl230
	nop
	nop
lbl230: nop
	jl	near lbl231
	nop
	nop
lbl231: nop
	jl	 lbl232
	nop
	nop
lbl232: nop
	jle	short lbl233
	nop
	nop
lbl233: nop
	jle	near lbl234
	nop
	nop
lbl234: nop
	jle	 lbl235
	nop
	nop
lbl235: nop
	jna	short lbl236
	nop
	nop
lbl236: nop
	jna	near lbl237
	nop
	nop
lbl237: nop
	jna	 lbl238
	nop
	nop
lbl238: nop
	jnae	short lbl239
	nop
	nop
lbl239: nop
	jnae	near lbl240
	nop
	nop
lbl240: nop
	jnae	 lbl241
	nop
	nop
lbl241: nop
	jnb	short lbl242
	nop
	nop
lbl242: nop
	jnb	near lbl243
	nop
	nop
lbl243: nop
	jnb	 lbl244
	nop
	nop
lbl244: nop
	jnbe	short lbl245
	nop
	nop
lbl245: nop
	jnbe	near lbl246
	nop
	nop
lbl246: nop
	jnbe	 lbl247
	nop
	nop
lbl247: nop
	jnc	short lbl248
	nop
	nop
lbl248: nop
	jnc	near lbl249
	nop
	nop
lbl249: nop
	jnc	 lbl250
	nop
	nop
lbl250: nop
	jne	short lbl251
	nop
	nop
lbl251: nop
	jne	near lbl252
	nop
	nop
lbl252: nop
	jne	 lbl253
	nop
	nop
lbl253: nop
	jng	short lbl254
	nop
	nop
lbl254: nop
	jng	near lbl255
	nop
	nop
lbl255: nop
	jng	 lbl256
	nop
	nop
lbl256: nop
	jnge	short lbl257
	nop
	nop
lbl257: nop
	jnge	near lbl258
	nop
	nop
lbl258: nop
	jnge	 lbl259
	nop
	nop
lbl259: nop
	jnl	short lbl260
	nop
	nop
lbl260: nop
	jnl	near lbl261
	nop
	nop
lbl261: nop
	jnl	 lbl262
	nop
	nop
lbl262: nop
	jnle	short lbl263
	nop
	nop
lbl263: nop
	jnle	near lbl264
	nop
	nop
lbl264: nop
	jnle	 lbl265
	nop
	nop
lbl265: nop
	jno	short lbl266
	nop
	nop
lbl266: nop
	jno	near lbl267
	nop
	nop
lbl267: nop
	jno	 lbl268
	nop
	nop
lbl268: nop
	jnp	short lbl269
	nop
	nop
lbl269: nop
	jnp	near lbl270
	nop
	nop
lbl270: nop
	jnp	 lbl271
	nop
	nop
lbl271: nop
	jns	short lbl272
	nop
	nop
lbl272: nop
	jns	near lbl273
	nop
	nop
lbl273: nop
	jns	 lbl274
	nop
	nop
lbl274: nop
	jnz	short lbl275
	nop
	nop
lbl275: nop
	jnz	near lbl276
	nop
	nop
lbl276: nop
	jnz	 lbl277
	nop
	nop
lbl277: nop
	jo	short lbl278
	nop
	nop
lbl278: nop
	jo	near lbl279
	nop
	nop
lbl279: nop
	jo	 lbl280
	nop
	nop
lbl280: nop
	jp	short lbl281
	nop
	nop
lbl281: nop
	jp	near lbl282
	nop
	nop
lbl282: nop
	jp	 lbl283
	nop
	nop
lbl283: nop
	jpe	short lbl284
	nop
	nop
lbl284: nop
	jpe	near lbl285
	nop
	nop
lbl285: nop
	jpe	 lbl286
	nop
	nop
lbl286: nop
	jpo	short lbl287
	nop
	nop
lbl287: nop
	jpo	near lbl288
	nop
	nop
lbl288: nop
	jpo	 lbl289
	nop
	nop
lbl289: nop
	js	short lbl290
	nop
	nop
lbl290: nop
	js	near lbl291
	nop
	nop
lbl291: nop
	js	 lbl292
	nop
	nop
lbl292: nop
	jz	short lbl293
	nop
	nop
lbl293: nop
	jz	near lbl294
	nop
	nop
lbl294: nop
	jz	 lbl295
	nop
	nop
lbl295: nop
	jecxz	 lbl296
	nop
	nop
lbl296: nop
	loop	 lbl297
	nop
	nop
lbl297: nop
	loopne	 lbl298
	nop
	nop
lbl298: nop
	loope	 lbl299
	nop
	nop
lbl299: nop
	loopz	 lbl300
	nop
	nop
lbl300: nop
	loopnz	 lbl301
	nop
	nop
lbl301: nop
	seta	[ecx]
nop
nop
nop
	seta	byte [ecx]
nop
nop
nop
	setae	[ecx]
nop
nop
nop
	setae	byte [ecx]
nop
nop
nop
	setb	[ecx]
nop
nop
nop
	setb	byte [ecx]
nop
nop
nop
	setbe	[ecx]
nop
nop
nop
	setbe	byte [ecx]
nop
nop
nop
	setc	[ecx]
nop
nop
nop
	setc	byte [ecx]
nop
nop
nop
	sete	[ecx]
nop
nop
nop
	sete	byte [ecx]
nop
nop
nop
	setg	[ecx]
nop
nop
nop
	setg	byte [ecx]
nop
nop
nop
	setge	[ecx]
nop
nop
nop
	setge	byte [ecx]
nop
nop
nop
	setl	[ecx]
nop
nop
nop
	setl	byte [ecx]
nop
nop
nop
	setle	[ecx]
nop
nop
nop
	setle	byte [ecx]
nop
nop
nop
	setna	[ecx]
nop
nop
nop
	setna	byte [ecx]
nop
nop
nop
	setnae	[ecx]
nop
nop
nop
	setnae	byte [ecx]
nop
nop
nop
	setnb	[ecx]
nop
nop
nop
	setnb	byte [ecx]
nop
nop
nop
	setnbe	[ecx]
nop
nop
nop
	setnbe	byte [ecx]
nop
nop
nop
	setnc	[ecx]
nop
nop
nop
	setnc	byte [ecx]
nop
nop
nop
	setne	[ecx]
nop
nop
nop
	setne	byte [ecx]
nop
nop
nop
	setng	[ecx]
nop
nop
nop
	setng	byte [ecx]
nop
nop
nop
	setnge	[ecx]
nop
nop
nop
	setnge	byte [ecx]
nop
nop
nop
	setnl	[ecx]
nop
nop
nop
	setnl	byte [ecx]
nop
nop
nop
	setnle	[ecx]
nop
nop
nop
	setnle	byte [ecx]
nop
nop
nop
	setno	[ecx]
nop
nop
nop
	setno	byte [ecx]
nop
nop
nop
	setnp	[ecx]
nop
nop
nop
	setnp	byte [ecx]
nop
nop
nop
	setns	[ecx]
nop
nop
nop
	setns	byte [ecx]
nop
nop
nop
	setnz	[ecx]
nop
nop
nop
	setnz	byte [ecx]
nop
nop
nop
	seto	[ecx]
nop
nop
nop
	seto	byte [ecx]
nop
nop
nop
	setp	[ecx]
nop
nop
nop
	setp	byte [ecx]
nop
nop
nop
	setpe	[ecx]
nop
nop
nop
	setpe	byte [ecx]
nop
nop
nop
	setpo	[ecx]
nop
nop
nop
	setpo	byte [ecx]
nop
nop
nop
	sets	[ecx]
nop
nop
nop
	sets	byte [ecx]
nop
nop
nop
	setz	[ecx]
nop
nop
nop
	setz	byte [ecx]
nop
nop
nop
	cmova	bx,[ecx]
nop
nop
nop
	cmova	r14w,[ecx]
nop
nop
nop
	cmova	ebx,[ecx]
nop
nop
nop
	cmova	r14d,[ecx]
nop
nop
nop
	cmova	rbx,[ecx]
nop
nop
nop
	cmova	r14,[ecx]
nop
nop
nop
	cmova	bx,word [ecx]
nop
nop
nop
	cmova	r14w,word [ecx]
nop
nop
nop
	cmova	ebx,dword [ecx]
nop
nop
nop
	cmova	r14d,dword [ecx]
nop
nop
nop
	cmova	rbx,qword [ecx]
nop
nop
nop
	cmova	r14,qword [ecx]
nop
nop
nop
	cmovae	bx,[ecx]
nop
nop
nop
	cmovae	r14w,[ecx]
nop
nop
nop
	cmovae	ebx,[ecx]
nop
nop
nop
	cmovae	r14d,[ecx]
nop
nop
nop
	cmovae	rbx,[ecx]
nop
nop
nop
	cmovae	r14,[ecx]
nop
nop
nop
	cmovae	bx,word [ecx]
nop
nop
nop
	cmovae	r14w,word [ecx]
nop
nop
nop
	cmovae	ebx,dword [ecx]
nop
nop
nop
	cmovae	r14d,dword [ecx]
nop
nop
nop
	cmovae	rbx,qword [ecx]
nop
nop
nop
	cmovae	r14,qword [ecx]
nop
nop
nop
	cmovb	bx,[ecx]
nop
nop
nop
	cmovb	r14w,[ecx]
nop
nop
nop
	cmovb	ebx,[ecx]
nop
nop
nop
	cmovb	r14d,[ecx]
nop
nop
nop
	cmovb	rbx,[ecx]
nop
nop
nop
	cmovb	r14,[ecx]
nop
nop
nop
	cmovb	bx,word [ecx]
nop
nop
nop
	cmovb	r14w,word [ecx]
nop
nop
nop
	cmovb	ebx,dword [ecx]
nop
nop
nop
	cmovb	r14d,dword [ecx]
nop
nop
nop
	cmovb	rbx,qword [ecx]
nop
nop
nop
	cmovb	r14,qword [ecx]
nop
nop
nop
	cmovbe	bx,[ecx]
nop
nop
nop
	cmovbe	r14w,[ecx]
nop
nop
nop
	cmovbe	ebx,[ecx]
nop
nop
nop
	cmovbe	r14d,[ecx]
nop
nop
nop
	cmovbe	rbx,[ecx]
nop
nop
nop
	cmovbe	r14,[ecx]
nop
nop
nop
	cmovbe	bx,word [ecx]
nop
nop
nop
	cmovbe	r14w,word [ecx]
nop
nop
nop
	cmovbe	ebx,dword [ecx]
nop
nop
nop
	cmovbe	r14d,dword [ecx]
nop
nop
nop
	cmovbe	rbx,qword [ecx]
nop
nop
nop
	cmovbe	r14,qword [ecx]
nop
nop
nop
	cmovc	bx,[ecx]
nop
nop
nop
	cmovc	r14w,[ecx]
nop
nop
nop
	cmovc	ebx,[ecx]
nop
nop
nop
	cmovc	r14d,[ecx]
nop
nop
nop
	cmovc	rbx,[ecx]
nop
nop
nop
	cmovc	r14,[ecx]
nop
nop
nop
	cmovc	bx,word [ecx]
nop
nop
nop
	cmovc	r14w,word [ecx]
nop
nop
nop
	cmovc	ebx,dword [ecx]
nop
nop
nop
	cmovc	r14d,dword [ecx]
nop
nop
nop
	cmovc	rbx,qword [ecx]
nop
nop
nop
	cmovc	r14,qword [ecx]
nop
nop
nop
	cmove	bx,[ecx]
nop
nop
nop
	cmove	r14w,[ecx]
nop
nop
nop
	cmove	ebx,[ecx]
nop
nop
nop
	cmove	r14d,[ecx]
nop
nop
nop
	cmove	rbx,[ecx]
nop
nop
nop
	cmove	r14,[ecx]
nop
nop
nop
	cmove	bx,word [ecx]
nop
nop
nop
	cmove	r14w,word [ecx]
nop
nop
nop
	cmove	ebx,dword [ecx]
nop
nop
nop
	cmove	r14d,dword [ecx]
nop
nop
nop
	cmove	rbx,qword [ecx]
nop
nop
nop
	cmove	r14,qword [ecx]
nop
nop
nop
	cmovg	bx,[ecx]
nop
nop
nop
	cmovg	r14w,[ecx]
nop
nop
nop
	cmovg	ebx,[ecx]
nop
nop
nop
	cmovg	r14d,[ecx]
nop
nop
nop
	cmovg	rbx,[ecx]
nop
nop
nop
	cmovg	r14,[ecx]
nop
nop
nop
	cmovg	bx,word [ecx]
nop
nop
nop
	cmovg	r14w,word [ecx]
nop
nop
nop
	cmovg	ebx,dword [ecx]
nop
nop
nop
	cmovg	r14d,dword [ecx]
nop
nop
nop
	cmovg	rbx,qword [ecx]
nop
nop
nop
	cmovg	r14,qword [ecx]
nop
nop
nop
	cmovge	bx,[ecx]
nop
nop
nop
	cmovge	r14w,[ecx]
nop
nop
nop
	cmovge	ebx,[ecx]
nop
nop
nop
	cmovge	r14d,[ecx]
nop
nop
nop
	cmovge	rbx,[ecx]
nop
nop
nop
	cmovge	r14,[ecx]
nop
nop
nop
	cmovge	bx,word [ecx]
nop
nop
nop
	cmovge	r14w,word [ecx]
nop
nop
nop
	cmovge	ebx,dword [ecx]
nop
nop
nop
	cmovge	r14d,dword [ecx]
nop
nop
nop
	cmovge	rbx,qword [ecx]
nop
nop
nop
	cmovge	r14,qword [ecx]
nop
nop
nop
	cmovl	bx,[ecx]
nop
nop
nop
	cmovl	r14w,[ecx]
nop
nop
nop
	cmovl	ebx,[ecx]
nop
nop
nop
	cmovl	r14d,[ecx]
nop
nop
nop
	cmovl	rbx,[ecx]
nop
nop
nop
	cmovl	r14,[ecx]
nop
nop
nop
	cmovl	bx,word [ecx]
nop
nop
nop
	cmovl	r14w,word [ecx]
nop
nop
nop
	cmovl	ebx,dword [ecx]
nop
nop
nop
	cmovl	r14d,dword [ecx]
nop
nop
nop
	cmovl	rbx,qword [ecx]
nop
nop
nop
	cmovl	r14,qword [ecx]
nop
nop
nop
	cmovle	bx,[ecx]
nop
nop
nop
	cmovle	r14w,[ecx]
nop
nop
nop
	cmovle	ebx,[ecx]
nop
nop
nop
	cmovle	r14d,[ecx]
nop
nop
nop
	cmovle	rbx,[ecx]
nop
nop
nop
	cmovle	r14,[ecx]
nop
nop
nop
	cmovle	bx,word [ecx]
nop
nop
nop
	cmovle	r14w,word [ecx]
nop
nop
nop
	cmovle	ebx,dword [ecx]
nop
nop
nop
	cmovle	r14d,dword [ecx]
nop
nop
nop
	cmovle	rbx,qword [ecx]
nop
nop
nop
	cmovle	r14,qword [ecx]
nop
nop
nop
	cmovna	bx,[ecx]
nop
nop
nop
	cmovna	r14w,[ecx]
nop
nop
nop
	cmovna	ebx,[ecx]
nop
nop
nop
	cmovna	r14d,[ecx]
nop
nop
nop
	cmovna	rbx,[ecx]
nop
nop
nop
	cmovna	r14,[ecx]
nop
nop
nop
	cmovna	bx,word [ecx]
nop
nop
nop
	cmovna	r14w,word [ecx]
nop
nop
nop
	cmovna	ebx,dword [ecx]
nop
nop
nop
	cmovna	r14d,dword [ecx]
nop
nop
nop
	cmovna	rbx,qword [ecx]
nop
nop
nop
	cmovna	r14,qword [ecx]
nop
nop
nop
	cmovnae	bx,[ecx]
nop
nop
nop
	cmovnae	r14w,[ecx]
nop
nop
nop
	cmovnae	ebx,[ecx]
nop
nop
nop
	cmovnae	r14d,[ecx]
nop
nop
nop
	cmovnae	rbx,[ecx]
nop
nop
nop
	cmovnae	r14,[ecx]
nop
nop
nop
	cmovnae	bx,word [ecx]
nop
nop
nop
	cmovnae	r14w,word [ecx]
nop
nop
nop
	cmovnae	ebx,dword [ecx]
nop
nop
nop
	cmovnae	r14d,dword [ecx]
nop
nop
nop
	cmovnae	rbx,qword [ecx]
nop
nop
nop
	cmovnae	r14,qword [ecx]
nop
nop
nop
	cmovnb	bx,[ecx]
nop
nop
nop
	cmovnb	r14w,[ecx]
nop
nop
nop
	cmovnb	ebx,[ecx]
nop
nop
nop
	cmovnb	r14d,[ecx]
nop
nop
nop
	cmovnb	rbx,[ecx]
nop
nop
nop
	cmovnb	r14,[ecx]
nop
nop
nop
	cmovnb	bx,word [ecx]
nop
nop
nop
	cmovnb	r14w,word [ecx]
nop
nop
nop
	cmovnb	ebx,dword [ecx]
nop
nop
nop
	cmovnb	r14d,dword [ecx]
nop
nop
nop
	cmovnb	rbx,qword [ecx]
nop
nop
nop
	cmovnb	r14,qword [ecx]
nop
nop
nop
	cmovnbe	bx,[ecx]
nop
nop
nop
	cmovnbe	r14w,[ecx]
nop
nop
nop
	cmovnbe	ebx,[ecx]
nop
nop
nop
	cmovnbe	r14d,[ecx]
nop
nop
nop
	cmovnbe	rbx,[ecx]
nop
nop
nop
	cmovnbe	r14,[ecx]
nop
nop
nop
	cmovnbe	bx,word [ecx]
nop
nop
nop
	cmovnbe	r14w,word [ecx]
nop
nop
nop
	cmovnbe	ebx,dword [ecx]
nop
nop
nop
	cmovnbe	r14d,dword [ecx]
nop
nop
nop
	cmovnbe	rbx,qword [ecx]
nop
nop
nop
	cmovnbe	r14,qword [ecx]
nop
nop
nop
	cmovnc	bx,[ecx]
nop
nop
nop
	cmovnc	r14w,[ecx]
nop
nop
nop
	cmovnc	ebx,[ecx]
nop
nop
nop
	cmovnc	r14d,[ecx]
nop
nop
nop
	cmovnc	rbx,[ecx]
nop
nop
nop
	cmovnc	r14,[ecx]
nop
nop
nop
	cmovnc	bx,word [ecx]
nop
nop
nop
	cmovnc	r14w,word [ecx]
nop
nop
nop
	cmovnc	ebx,dword [ecx]
nop
nop
nop
	cmovnc	r14d,dword [ecx]
nop
nop
nop
	cmovnc	rbx,qword [ecx]
nop
nop
nop
	cmovnc	r14,qword [ecx]
nop
nop
nop
	cmovne	bx,[ecx]
nop
nop
nop
	cmovne	r14w,[ecx]
nop
nop
nop
	cmovne	ebx,[ecx]
nop
nop
nop
	cmovne	r14d,[ecx]
nop
nop
nop
	cmovne	rbx,[ecx]
nop
nop
nop
	cmovne	r14,[ecx]
nop
nop
nop
	cmovne	bx,word [ecx]
nop
nop
nop
	cmovne	r14w,word [ecx]
nop
nop
nop
	cmovne	ebx,dword [ecx]
nop
nop
nop
	cmovne	r14d,dword [ecx]
nop
nop
nop
	cmovne	rbx,qword [ecx]
nop
nop
nop
	cmovne	r14,qword [ecx]
nop
nop
nop
	cmovng	bx,[ecx]
nop
nop
nop
	cmovng	r14w,[ecx]
nop
nop
nop
	cmovng	ebx,[ecx]
nop
nop
nop
	cmovng	r14d,[ecx]
nop
nop
nop
	cmovng	rbx,[ecx]
nop
nop
nop
	cmovng	r14,[ecx]
nop
nop
nop
	cmovng	bx,word [ecx]
nop
nop
nop
	cmovng	r14w,word [ecx]
nop
nop
nop
	cmovng	ebx,dword [ecx]
nop
nop
nop
	cmovng	r14d,dword [ecx]
nop
nop
nop
	cmovng	rbx,qword [ecx]
nop
nop
nop
	cmovng	r14,qword [ecx]
nop
nop
nop
	cmovnge	bx,[ecx]
nop
nop
nop
	cmovnge	r14w,[ecx]
nop
nop
nop
	cmovnge	ebx,[ecx]
nop
nop
nop
	cmovnge	r14d,[ecx]
nop
nop
nop
	cmovnge	rbx,[ecx]
nop
nop
nop
	cmovnge	r14,[ecx]
nop
nop
nop
	cmovnge	bx,word [ecx]
nop
nop
nop
	cmovnge	r14w,word [ecx]
nop
nop
nop
	cmovnge	ebx,dword [ecx]
nop
nop
nop
	cmovnge	r14d,dword [ecx]
nop
nop
nop
	cmovnge	rbx,qword [ecx]
nop
nop
nop
	cmovnge	r14,qword [ecx]
nop
nop
nop
	cmovnl	bx,[ecx]
nop
nop
nop
	cmovnl	r14w,[ecx]
nop
nop
nop
	cmovnl	ebx,[ecx]
nop
nop
nop
	cmovnl	r14d,[ecx]
nop
nop
nop
	cmovnl	rbx,[ecx]
nop
nop
nop
	cmovnl	r14,[ecx]
nop
nop
nop
	cmovnl	bx,word [ecx]
nop
nop
nop
	cmovnl	r14w,word [ecx]
nop
nop
nop
	cmovnl	ebx,dword [ecx]
nop
nop
nop
	cmovnl	r14d,dword [ecx]
nop
nop
nop
	cmovnl	rbx,qword [ecx]
nop
nop
nop
	cmovnl	r14,qword [ecx]
nop
nop
nop
	cmovnle	bx,[ecx]
nop
nop
nop
	cmovnle	r14w,[ecx]
nop
nop
nop
	cmovnle	ebx,[ecx]
nop
nop
nop
	cmovnle	r14d,[ecx]
nop
nop
nop
	cmovnle	rbx,[ecx]
nop
nop
nop
	cmovnle	r14,[ecx]
nop
nop
nop
	cmovnle	bx,word [ecx]
nop
nop
nop
	cmovnle	r14w,word [ecx]
nop
nop
nop
	cmovnle	ebx,dword [ecx]
nop
nop
nop
	cmovnle	r14d,dword [ecx]
nop
nop
nop
	cmovnle	rbx,qword [ecx]
nop
nop
nop
	cmovnle	r14,qword [ecx]
nop
nop
nop
	cmovno	bx,[ecx]
nop
nop
nop
	cmovno	r14w,[ecx]
nop
nop
nop
	cmovno	ebx,[ecx]
nop
nop
nop
	cmovno	r14d,[ecx]
nop
nop
nop
	cmovno	rbx,[ecx]
nop
nop
nop
	cmovno	r14,[ecx]
nop
nop
nop
	cmovno	bx,word [ecx]
nop
nop
nop
	cmovno	r14w,word [ecx]
nop
nop
nop
	cmovno	ebx,dword [ecx]
nop
nop
nop
	cmovno	r14d,dword [ecx]
nop
nop
nop
	cmovno	rbx,qword [ecx]
nop
nop
nop
	cmovno	r14,qword [ecx]
nop
nop
nop
	cmovnp	bx,[ecx]
nop
nop
nop
	cmovnp	r14w,[ecx]
nop
nop
nop
	cmovnp	ebx,[ecx]
nop
nop
nop
	cmovnp	r14d,[ecx]
nop
nop
nop
	cmovnp	rbx,[ecx]
nop
nop
nop
	cmovnp	r14,[ecx]
nop
nop
nop
	cmovnp	bx,word [ecx]
nop
nop
nop
	cmovnp	r14w,word [ecx]
nop
nop
nop
	cmovnp	ebx,dword [ecx]
nop
nop
nop
	cmovnp	r14d,dword [ecx]
nop
nop
nop
	cmovnp	rbx,qword [ecx]
nop
nop
nop
	cmovnp	r14,qword [ecx]
nop
nop
nop
	cmovns	bx,[ecx]
nop
nop
nop
	cmovns	r14w,[ecx]
nop
nop
nop
	cmovns	ebx,[ecx]
nop
nop
nop
	cmovns	r14d,[ecx]
nop
nop
nop
	cmovns	rbx,[ecx]
nop
nop
nop
	cmovns	r14,[ecx]
nop
nop
nop
	cmovns	bx,word [ecx]
nop
nop
nop
	cmovns	r14w,word [ecx]
nop
nop
nop
	cmovns	ebx,dword [ecx]
nop
nop
nop
	cmovns	r14d,dword [ecx]
nop
nop
nop
	cmovns	rbx,qword [ecx]
nop
nop
nop
	cmovns	r14,qword [ecx]
nop
nop
nop
	cmovnz	bx,[ecx]
nop
nop
nop
	cmovnz	r14w,[ecx]
nop
nop
nop
	cmovnz	ebx,[ecx]
nop
nop
nop
	cmovnz	r14d,[ecx]
nop
nop
nop
	cmovnz	rbx,[ecx]
nop
nop
nop
	cmovnz	r14,[ecx]
nop
nop
nop
	cmovnz	bx,word [ecx]
nop
nop
nop
	cmovnz	r14w,word [ecx]
nop
nop
nop
	cmovnz	ebx,dword [ecx]
nop
nop
nop
	cmovnz	r14d,dword [ecx]
nop
nop
nop
	cmovnz	rbx,qword [ecx]
nop
nop
nop
	cmovnz	r14,qword [ecx]
nop
nop
nop
	cmovo	bx,[ecx]
nop
nop
nop
	cmovo	r14w,[ecx]
nop
nop
nop
	cmovo	ebx,[ecx]
nop
nop
nop
	cmovo	r14d,[ecx]
nop
nop
nop
	cmovo	rbx,[ecx]
nop
nop
nop
	cmovo	r14,[ecx]
nop
nop
nop
	cmovo	bx,word [ecx]
nop
nop
nop
	cmovo	r14w,word [ecx]
nop
nop
nop
	cmovo	ebx,dword [ecx]
nop
nop
nop
	cmovo	r14d,dword [ecx]
nop
nop
nop
	cmovo	rbx,qword [ecx]
nop
nop
nop
	cmovo	r14,qword [ecx]
nop
nop
nop
	cmovp	bx,[ecx]
nop
nop
nop
	cmovp	r14w,[ecx]
nop
nop
nop
	cmovp	ebx,[ecx]
nop
nop
nop
	cmovp	r14d,[ecx]
nop
nop
nop
	cmovp	rbx,[ecx]
nop
nop
nop
	cmovp	r14,[ecx]
nop
nop
nop
	cmovp	bx,word [ecx]
nop
nop
nop
	cmovp	r14w,word [ecx]
nop
nop
nop
	cmovp	ebx,dword [ecx]
nop
nop
nop
	cmovp	r14d,dword [ecx]
nop
nop
nop
	cmovp	rbx,qword [ecx]
nop
nop
nop
	cmovp	r14,qword [ecx]
nop
nop
nop
	cmovpe	bx,[ecx]
nop
nop
nop
	cmovpe	r14w,[ecx]
nop
nop
nop
	cmovpe	ebx,[ecx]
nop
nop
nop
	cmovpe	r14d,[ecx]
nop
nop
nop
	cmovpe	rbx,[ecx]
nop
nop
nop
	cmovpe	r14,[ecx]
nop
nop
nop
	cmovpe	bx,word [ecx]
nop
nop
nop
	cmovpe	r14w,word [ecx]
nop
nop
nop
	cmovpe	ebx,dword [ecx]
nop
nop
nop
	cmovpe	r14d,dword [ecx]
nop
nop
nop
	cmovpe	rbx,qword [ecx]
nop
nop
nop
	cmovpe	r14,qword [ecx]
nop
nop
nop
	cmovpo	bx,[ecx]
nop
nop
nop
	cmovpo	r14w,[ecx]
nop
nop
nop
	cmovpo	ebx,[ecx]
nop
nop
nop
	cmovpo	r14d,[ecx]
nop
nop
nop
	cmovpo	rbx,[ecx]
nop
nop
nop
	cmovpo	r14,[ecx]
nop
nop
nop
	cmovpo	bx,word [ecx]
nop
nop
nop
	cmovpo	r14w,word [ecx]
nop
nop
nop
	cmovpo	ebx,dword [ecx]
nop
nop
nop
	cmovpo	r14d,dword [ecx]
nop
nop
nop
	cmovpo	rbx,qword [ecx]
nop
nop
nop
	cmovpo	r14,qword [ecx]
nop
nop
nop
	cmovs	bx,[ecx]
nop
nop
nop
	cmovs	r14w,[ecx]
nop
nop
nop
	cmovs	ebx,[ecx]
nop
nop
nop
	cmovs	r14d,[ecx]
nop
nop
nop
	cmovs	rbx,[ecx]
nop
nop
nop
	cmovs	r14,[ecx]
nop
nop
nop
	cmovs	bx,word [ecx]
nop
nop
nop
	cmovs	r14w,word [ecx]
nop
nop
nop
	cmovs	ebx,dword [ecx]
nop
nop
nop
	cmovs	r14d,dword [ecx]
nop
nop
nop
	cmovs	rbx,qword [ecx]
nop
nop
nop
	cmovs	r14,qword [ecx]
nop
nop
nop
	cmovz	bx,[ecx]
nop
nop
nop
	cmovz	r14w,[ecx]
nop
nop
nop
	cmovz	ebx,[ecx]
nop
nop
nop
	cmovz	r14d,[ecx]
nop
nop
nop
	cmovz	rbx,[ecx]
nop
nop
nop
	cmovz	r14,[ecx]
nop
nop
nop
	cmovz	bx,word [ecx]
nop
nop
nop
	cmovz	r14w,word [ecx]
nop
nop
nop
	cmovz	ebx,dword [ecx]
nop
nop
nop
	cmovz	r14d,dword [ecx]
nop
nop
nop
	cmovz	rbx,qword [ecx]
nop
nop
nop
	cmovz	r14,qword [ecx]
nop
nop
nop
	movsx	bx,cl
nop
nop
nop
	movsx	r14w,sil
nop
nop
nop
	movsx	ebx,al
nop
nop
nop
	movsx	r14d,r11b
nop
nop
nop
	movsx	rbx,al
nop
nop
nop
	movsx	r14,sil
nop
nop
nop
	movsx	ebx,cx
nop
nop
nop
	movsx	r14d,r11w
nop
nop
nop
	movsx	rbx,ax
nop
nop
nop
	movsx	bx,byte [ecx]
nop
nop
nop
	movsx	r14w,byte [ecx]
nop
nop
nop
	movsx	ebx,byte [ecx]
nop
nop
nop
	movsx	r14d,byte [ecx]
nop
nop
nop
	movsx	rbx,byte [ecx]
nop
nop
nop
	movsx	r14,byte [ecx]
nop
nop
nop
	movsx	ebx,word [ecx]
nop
nop
nop
	movsx	r14d,word [ecx]
nop
nop
nop
	movsx	rbx,word [ecx]
nop
nop
nop
	movsx	r14,word [ecx]
nop
nop
nop
	movzx	bx,cl
nop
nop
nop
	movzx	r14w,sil
nop
nop
nop
	movzx	ebx,al
nop
nop
nop
	movzx	r14d,r11b
nop
nop
nop
	movzx	rbx,al
nop
nop
nop
	movzx	r14,sil
nop
nop
nop
	movzx	ebx,cx
nop
nop
nop
	movzx	r14d,r11w
nop
nop
nop
	movzx	rbx,ax
nop
nop
nop
	movzx	bx,byte [ecx]
nop
nop
nop
	movzx	r14w,byte [ecx]
nop
nop
nop
	movzx	ebx,byte [ecx]
nop
nop
nop
	movzx	r14d,byte [ecx]
nop
nop
nop
	movzx	rbx,byte [ecx]
nop
nop
nop
	movzx	r14,byte [ecx]
nop
nop
nop
	movzx	ebx,word [ecx]
nop
nop
nop
	movzx	r14d,word [ecx]
nop
nop
nop
	movzx	rbx,word [ecx]
nop
nop
nop
	movzx	r14,word [ecx]
nop
nop
nop
	rcl	bl,1
nop
nop
nop
	rcl	dil,1
nop
nop
nop
	rcl	r15b,1
nop
nop
nop
	rcl	bx,1
nop
nop
nop
	rcl	r14w,1
nop
nop
nop
	rcl	ebx,1
nop
nop
nop
	rcl	r14d,1
nop
nop
nop
	rcl	rbx,1
nop
nop
nop
	rcl	r14,1
nop
nop
nop
	rcl	byte [ecx],1
nop
nop
nop
	rcl	word [ecx],1
nop
nop
nop
	rcl	dword [ecx],1
nop
nop
nop
	rcl	qword [ecx],1
nop
nop
nop
	rcl	bl,3
nop
nop
nop
	rcl	dil,3
nop
nop
nop
	rcl	r15b,3
nop
nop
nop
	rcl	bx,3
nop
nop
nop
	rcl	r14w,3
nop
nop
nop
	rcl	ebx,3
nop
nop
nop
	rcl	r14d,3
nop
nop
nop
	rcl	rbx,3
nop
nop
nop
	rcl	r14,3
nop
nop
nop
	rcl	byte [ecx],3
nop
nop
nop
	rcl	word [ecx],3
nop
nop
nop
	rcl	dword [ecx],3
nop
nop
nop
	rcl	qword [ecx],3
nop
nop
nop
	rcl	bl,cl
nop
nop
nop
	rcl	dil,cl
nop
nop
nop
	rcl	r15b,cl
nop
nop
nop
	rcl	bx,cl
nop
nop
nop
	rcl	r14w,cl
nop
nop
nop
	rcl	ebx,cl
nop
nop
nop
	rcl	r14d,cl
nop
nop
nop
	rcl	rbx,cl
nop
nop
nop
	rcl	r14,cl
nop
nop
nop
	rcl	byte [ecx],cl
nop
nop
nop
	rcl	word [ecx],cl
nop
nop
nop
	rcl	dword [ecx],cl
nop
nop
nop
	rcl	qword [ecx],cl
nop
nop
nop
	rcr	bl,1
nop
nop
nop
	rcr	dil,1
nop
nop
nop
	rcr	r15b,1
nop
nop
nop
	rcr	bx,1
nop
nop
nop
	rcr	r14w,1
nop
nop
nop
	rcr	ebx,1
nop
nop
nop
	rcr	r14d,1
nop
nop
nop
	rcr	rbx,1
nop
nop
nop
	rcr	r14,1
nop
nop
nop
	rcr	byte [ecx],1
nop
nop
nop
	rcr	word [ecx],1
nop
nop
nop
	rcr	dword [ecx],1
nop
nop
nop
	rcr	qword [ecx],1
nop
nop
nop
	rcr	bl,3
nop
nop
nop
	rcr	dil,3
nop
nop
nop
	rcr	r15b,3
nop
nop
nop
	rcr	bx,3
nop
nop
nop
	rcr	r14w,3
nop
nop
nop
	rcr	ebx,3
nop
nop
nop
	rcr	r14d,3
nop
nop
nop
	rcr	rbx,3
nop
nop
nop
	rcr	r14,3
nop
nop
nop
	rcr	byte [ecx],3
nop
nop
nop
	rcr	word [ecx],3
nop
nop
nop
	rcr	dword [ecx],3
nop
nop
nop
	rcr	qword [ecx],3
nop
nop
nop
	rcr	bl,cl
nop
nop
nop
	rcr	dil,cl
nop
nop
nop
	rcr	r15b,cl
nop
nop
nop
	rcr	bx,cl
nop
nop
nop
	rcr	r14w,cl
nop
nop
nop
	rcr	ebx,cl
nop
nop
nop
	rcr	r14d,cl
nop
nop
nop
	rcr	rbx,cl
nop
nop
nop
	rcr	r14,cl
nop
nop
nop
	rcr	byte [ecx],cl
nop
nop
nop
	rcr	word [ecx],cl
nop
nop
nop
	rcr	dword [ecx],cl
nop
nop
nop
	rcr	qword [ecx],cl
nop
nop
nop
	rol	bl,1
nop
nop
nop
	rol	dil,1
nop
nop
nop
	rol	r15b,1
nop
nop
nop
	rol	bx,1
nop
nop
nop
	rol	r14w,1
nop
nop
nop
	rol	ebx,1
nop
nop
nop
	rol	r14d,1
nop
nop
nop
	rol	rbx,1
nop
nop
nop
	rol	r14,1
nop
nop
nop
	rol	byte [ecx],1
nop
nop
nop
	rol	word [ecx],1
nop
nop
nop
	rol	dword [ecx],1
nop
nop
nop
	rol	qword [ecx],1
nop
nop
nop
	rol	bl,3
nop
nop
nop
	rol	dil,3
nop
nop
nop
	rol	r15b,3
nop
nop
nop
	rol	bx,3
nop
nop
nop
	rol	r14w,3
nop
nop
nop
	rol	ebx,3
nop
nop
nop
	rol	r14d,3
nop
nop
nop
	rol	rbx,3
nop
nop
nop
	rol	r14,3
nop
nop
nop
	rol	byte [ecx],3
nop
nop
nop
	rol	word [ecx],3
nop
nop
nop
	rol	dword [ecx],3
nop
nop
nop
	rol	qword [ecx],3
nop
nop
nop
	rol	bl,cl
nop
nop
nop
	rol	dil,cl
nop
nop
nop
	rol	r15b,cl
nop
nop
nop
	rol	bx,cl
nop
nop
nop
	rol	r14w,cl
nop
nop
nop
	rol	ebx,cl
nop
nop
nop
	rol	r14d,cl
nop
nop
nop
	rol	rbx,cl
nop
nop
nop
	rol	r14,cl
nop
nop
nop
	rol	byte [ecx],cl
nop
nop
nop
	rol	word [ecx],cl
nop
nop
nop
	rol	dword [ecx],cl
nop
nop
nop
	rol	qword [ecx],cl
nop
nop
nop
	ror	bl,1
nop
nop
nop
	ror	dil,1
nop
nop
nop
	ror	r15b,1
nop
nop
nop
	ror	bx,1
nop
nop
nop
	ror	r14w,1
nop
nop
nop
	ror	ebx,1
nop
nop
nop
	ror	r14d,1
nop
nop
nop
	ror	rbx,1
nop
nop
nop
	ror	r14,1
nop
nop
nop
	ror	byte [ecx],1
nop
nop
nop
	ror	word [ecx],1
nop
nop
nop
	ror	dword [ecx],1
nop
nop
nop
	ror	qword [ecx],1
nop
nop
nop
	ror	bl,3
nop
nop
nop
	ror	dil,3
nop
nop
nop
	ror	r15b,3
nop
nop
nop
	ror	bx,3
nop
nop
nop
	ror	r14w,3
nop
nop
nop
	ror	ebx,3
nop
nop
nop
	ror	r14d,3
nop
nop
nop
	ror	rbx,3
nop
nop
nop
	ror	r14,3
nop
nop
nop
	ror	byte [ecx],3
nop
nop
nop
	ror	word [ecx],3
nop
nop
nop
	ror	dword [ecx],3
nop
nop
nop
	ror	qword [ecx],3
nop
nop
nop
	ror	bl,cl
nop
nop
nop
	ror	dil,cl
nop
nop
nop
	ror	r15b,cl
nop
nop
nop
	ror	bx,cl
nop
nop
nop
	ror	r14w,cl
nop
nop
nop
	ror	ebx,cl
nop
nop
nop
	ror	r14d,cl
nop
nop
nop
	ror	rbx,cl
nop
nop
nop
	ror	r14,cl
nop
nop
nop
	ror	byte [ecx],cl
nop
nop
nop
	ror	word [ecx],cl
nop
nop
nop
	ror	dword [ecx],cl
nop
nop
nop
	ror	qword [ecx],cl
nop
nop
nop
	sal	bl,1
nop
nop
nop
	sal	dil,1
nop
nop
nop
	sal	r15b,1
nop
nop
nop
	sal	bx,1
nop
nop
nop
	sal	r14w,1
nop
nop
nop
	sal	ebx,1
nop
nop
nop
	sal	r14d,1
nop
nop
nop
	sal	rbx,1
nop
nop
nop
	sal	r14,1
nop
nop
nop
	sal	byte [ecx],1
nop
nop
nop
	sal	word [ecx],1
nop
nop
nop
	sal	dword [ecx],1
nop
nop
nop
	sal	qword [ecx],1
nop
nop
nop
	sal	bl,3
nop
nop
nop
	sal	dil,3
nop
nop
nop
	sal	r15b,3
nop
nop
nop
	sal	bx,3
nop
nop
nop
	sal	r14w,3
nop
nop
nop
	sal	ebx,3
nop
nop
nop
	sal	r14d,3
nop
nop
nop
	sal	rbx,3
nop
nop
nop
	sal	r14,3
nop
nop
nop
	sal	byte [ecx],3
nop
nop
nop
	sal	word [ecx],3
nop
nop
nop
	sal	dword [ecx],3
nop
nop
nop
	sal	qword [ecx],3
nop
nop
nop
	sal	bl,cl
nop
nop
nop
	sal	dil,cl
nop
nop
nop
	sal	r15b,cl
nop
nop
nop
	sal	bx,cl
nop
nop
nop
	sal	r14w,cl
nop
nop
nop
	sal	ebx,cl
nop
nop
nop
	sal	r14d,cl
nop
nop
nop
	sal	rbx,cl
nop
nop
nop
	sal	r14,cl
nop
nop
nop
	sal	byte [ecx],cl
nop
nop
nop
	sal	word [ecx],cl
nop
nop
nop
	sal	dword [ecx],cl
nop
nop
nop
	sal	qword [ecx],cl
nop
nop
nop
	sar	bl,1
nop
nop
nop
	sar	dil,1
nop
nop
nop
	sar	r15b,1
nop
nop
nop
	sar	bx,1
nop
nop
nop
	sar	r14w,1
nop
nop
nop
	sar	ebx,1
nop
nop
nop
	sar	r14d,1
nop
nop
nop
	sar	rbx,1
nop
nop
nop
	sar	r14,1
nop
nop
nop
	sar	byte [ecx],1
nop
nop
nop
	sar	word [ecx],1
nop
nop
nop
	sar	dword [ecx],1
nop
nop
nop
	sar	qword [ecx],1
nop
nop
nop
	sar	bl,3
nop
nop
nop
	sar	dil,3
nop
nop
nop
	sar	r15b,3
nop
nop
nop
	sar	bx,3
nop
nop
nop
	sar	r14w,3
nop
nop
nop
	sar	ebx,3
nop
nop
nop
	sar	r14d,3
nop
nop
nop
	sar	rbx,3
nop
nop
nop
	sar	r14,3
nop
nop
nop
	sar	byte [ecx],3
nop
nop
nop
	sar	word [ecx],3
nop
nop
nop
	sar	dword [ecx],3
nop
nop
nop
	sar	qword [ecx],3
nop
nop
nop
	sar	bl,cl
nop
nop
nop
	sar	dil,cl
nop
nop
nop
	sar	r15b,cl
nop
nop
nop
	sar	bx,cl
nop
nop
nop
	sar	r14w,cl
nop
nop
nop
	sar	ebx,cl
nop
nop
nop
	sar	r14d,cl
nop
nop
nop
	sar	rbx,cl
nop
nop
nop
	sar	r14,cl
nop
nop
nop
	sar	byte [ecx],cl
nop
nop
nop
	sar	word [ecx],cl
nop
nop
nop
	sar	dword [ecx],cl
nop
nop
nop
	sar	qword [ecx],cl
nop
nop
nop
	shl	bl,1
nop
nop
nop
	shl	dil,1
nop
nop
nop
	shl	r15b,1
nop
nop
nop
	shl	bx,1
nop
nop
nop
	shl	r14w,1
nop
nop
nop
	shl	ebx,1
nop
nop
nop
	shl	r14d,1
nop
nop
nop
	shl	rbx,1
nop
nop
nop
	shl	r14,1
nop
nop
nop
	shl	byte [ecx],1
nop
nop
nop
	shl	word [ecx],1
nop
nop
nop
	shl	dword [ecx],1
nop
nop
nop
	shl	qword [ecx],1
nop
nop
nop
	shl	bl,3
nop
nop
nop
	shl	dil,3
nop
nop
nop
	shl	r15b,3
nop
nop
nop
	shl	bx,3
nop
nop
nop
	shl	r14w,3
nop
nop
nop
	shl	ebx,3
nop
nop
nop
	shl	r14d,3
nop
nop
nop
	shl	rbx,3
nop
nop
nop
	shl	r14,3
nop
nop
nop
	shl	byte [ecx],3
nop
nop
nop
	shl	word [ecx],3
nop
nop
nop
	shl	dword [ecx],3
nop
nop
nop
	shl	qword [ecx],3
nop
nop
nop
	shl	bl,cl
nop
nop
nop
	shl	dil,cl
nop
nop
nop
	shl	r15b,cl
nop
nop
nop
	shl	bx,cl
nop
nop
nop
	shl	r14w,cl
nop
nop
nop
	shl	ebx,cl
nop
nop
nop
	shl	r14d,cl
nop
nop
nop
	shl	rbx,cl
nop
nop
nop
	shl	r14,cl
nop
nop
nop
	shl	byte [ecx],cl
nop
nop
nop
	shl	word [ecx],cl
nop
nop
nop
	shl	dword [ecx],cl
nop
nop
nop
	shl	qword [ecx],cl
nop
nop
nop
	shr	bl,1
nop
nop
nop
	shr	dil,1
nop
nop
nop
	shr	r15b,1
nop
nop
nop
	shr	bx,1
nop
nop
nop
	shr	r14w,1
nop
nop
nop
	shr	ebx,1
nop
nop
nop
	shr	r14d,1
nop
nop
nop
	shr	rbx,1
nop
nop
nop
	shr	r14,1
nop
nop
nop
	shr	byte [ecx],1
nop
nop
nop
	shr	word [ecx],1
nop
nop
nop
	shr	dword [ecx],1
nop
nop
nop
	shr	qword [ecx],1
nop
nop
nop
	shr	bl,3
nop
nop
nop
	shr	dil,3
nop
nop
nop
	shr	r15b,3
nop
nop
nop
	shr	bx,3
nop
nop
nop
	shr	r14w,3
nop
nop
nop
	shr	ebx,3
nop
nop
nop
	shr	r14d,3
nop
nop
nop
	shr	rbx,3
nop
nop
nop
	shr	r14,3
nop
nop
nop
	shr	byte [ecx],3
nop
nop
nop
	shr	word [ecx],3
nop
nop
nop
	shr	dword [ecx],3
nop
nop
nop
	shr	qword [ecx],3
nop
nop
nop
	shr	bl,cl
nop
nop
nop
	shr	dil,cl
nop
nop
nop
	shr	r15b,cl
nop
nop
nop
	shr	bx,cl
nop
nop
nop
	shr	r14w,cl
nop
nop
nop
	shr	ebx,cl
nop
nop
nop
	shr	r14d,cl
nop
nop
nop
	shr	rbx,cl
nop
nop
nop
	shr	r14,cl
nop
nop
nop
	shr	byte [ecx],cl
nop
nop
nop
	shr	word [ecx],cl
nop
nop
nop
	shr	dword [ecx],cl
nop
nop
nop
	shr	qword [ecx],cl
nop
nop
nop
	shld	word [ecx],bx,1
nop
nop
nop
	shld	word [ecx],r14w,1
nop
nop
nop
	shld	dword [ecx],ebx,1
nop
nop
nop
	shld	dword [ecx],r14d,1
nop
nop
nop
	shld	qword [ecx],rbx,1
nop
nop
nop
	shld	qword [ecx],r14,1
nop
nop
nop
	shld	word [ecx],bx,cl
nop
nop
nop
	shld	word [ecx],r14w,cl
nop
nop
nop
	shld	dword [ecx],ebx,cl
nop
nop
nop
	shld	dword [ecx],r14d,cl
nop
nop
nop
	shld	qword [ecx],rbx,cl
nop
nop
nop
	shld	qword [ecx],r14,cl
nop
nop
nop
	shrd	word [ecx],bx,1
nop
nop
nop
	shrd	word [ecx],r14w,1
nop
nop
nop
	shrd	dword [ecx],ebx,1
nop
nop
nop
	shrd	dword [ecx],r14d,1
nop
nop
nop
	shrd	qword [ecx],rbx,1
nop
nop
nop
	shrd	qword [ecx],r14,1
nop
nop
nop
	shrd	word [ecx],bx,cl
nop
nop
nop
	shrd	word [ecx],r14w,cl
nop
nop
nop
	shrd	dword [ecx],ebx,cl
nop
nop
nop
	shrd	dword [ecx],r14d,cl
nop
nop
nop
	shrd	qword [ecx],rbx,cl
nop
nop
nop
	shrd	qword [ecx],r14,cl
nop
nop
nop
	lar	bx,[ecx]
nop
nop
nop
	lar	r14w,[ecx]
nop
nop
nop
	lar	ebx,[ecx]
nop
nop
nop
	lar	r14d,[ecx]
nop
nop
nop
	lar	rbx,[ecx]
nop
nop
nop
	lar	r14,[ecx]
nop
nop
nop
	lea	bx,[ecx]
nop
nop
nop
	lea	r14w,[ecx]
nop
nop
nop
	lea	ebx,[ecx]
nop
nop
nop
	lea	r14d,[ecx]
nop
nop
nop
	lea	rbx,[ecx]
nop
nop
nop
	lea	r14,[ecx]
nop
nop
nop
	lfs	bx,[ecx]
nop
nop
nop
	lfs	r14w,[ecx]
nop
nop
nop
	lfs	ebx,[ecx]
nop
nop
nop
	lfs	r14d,[ecx]
nop
nop
nop
	lfs	rbx,[ecx]
nop
nop
nop
	lfs	r14,[ecx]
nop
nop
nop
	lgs	bx,[ecx]
nop
nop
nop
	lgs	r14w,[ecx]
nop
nop
nop
	lgs	ebx,[ecx]
nop
nop
nop
	lgs	r14d,[ecx]
nop
nop
nop
	lgs	rbx,[ecx]
nop
nop
nop
	lgs	r14,[ecx]
nop
nop
nop
	lsl	bx,[ecx]
nop
nop
nop
	lsl	r14w,[ecx]
nop
nop
nop
	lsl	ebx,[ecx]
nop
nop
nop
	lsl	r14d,[ecx]
nop
nop
nop
	lsl	rbx,[ecx]
nop
nop
nop
	lsl	r14,[ecx]
nop
nop
nop
	popcnt	bx,[ecx]
nop
nop
nop
	popcnt	r14w,[ecx]
nop
nop
nop
	popcnt	ebx,[ecx]
nop
nop
nop
	popcnt	r14d,[ecx]
nop
nop
nop
	popcnt	rbx,[ecx]
nop
nop
nop
	popcnt	r14,[ecx]
nop
nop
nop
	bsf	bx,cx
nop
nop
nop
	bsf	bx,[ecx]
nop
nop
nop
	bsf	r14w,[ecx]
nop
nop
nop
	bsf	bx, r14w
nop
nop
nop
	bsf	ebx,[ecx]
nop
nop
nop
	bsf	ebx,ecx
nop
nop
nop
	bsf	ebx,r14d
nop
nop
nop
	bsf	r14d,[ecx]
nop
nop
nop
	bsf	rbx,[ecx]
nop
nop
nop
	bsf	r14,[ecx]
nop
nop
nop
	bsf	r15, rcx
nop
nop
nop
	bsf	rcx, r15
nop
nop
nop
	bsf	rcx,rdx
nop
nop
nop
	bsr	bx,cx
nop
nop
nop
	bsr	bx,[ecx]
nop
nop
nop
	bsr	r14w,[ecx]
nop
nop
nop
	bsr	bx, r14w
nop
nop
nop
	bsr	ebx,[ecx]
nop
nop
nop
	bsr	ebx,ecx
nop
nop
nop
	bsr	ebx,r14d
nop
nop
nop
	bsr	r14d,[ecx]
nop
nop
nop
	bsr	rbx,[ecx]
nop
nop
nop
	bsr	r14,[ecx]
nop
nop
nop
	bsr	r15, rcx
nop
nop
nop
	bsr	rcx, r15
nop
nop
nop
	bsr	rcx,rdx
nop
nop
nop
	lgdt	[ecx]
nop
nop
nop
	lidt	[ecx]
nop
nop
nop
	lldt	[ecx]
nop
nop
nop
	sidt	[ecx]
nop
nop
nop
	sldt	[ecx]
nop
nop
nop
	movbe	bx,[ecx]
nop
nop
nop
	movbe	r14w,[ecx]
nop
nop
nop
	movbe	ebx,[ecx]
nop
nop
nop
	movbe	r14d,[ecx]
nop
nop
nop
	movbe	rbx,[ecx]
nop
nop
nop
	movbe	r14,[ecx]
nop
nop
nop
	movbe	bx,word [ecx]
nop
nop
nop
	movbe	r14w,word [ecx]
nop
nop
nop
	movbe	ebx,dword [ecx]
nop
nop
nop
	movbe	r14d,dword [ecx]
nop
nop
nop
	movbe	rbx,qword [ecx]
nop
nop
nop
	movbe	r14,qword [ecx]
nop
nop
nop
	movbe	[ecx],bx
nop
nop
nop
	movbe	[ecx],r14w
nop
nop
nop
	movbe	[ecx],ebx
nop
nop
nop
	movbe	[ecx],r14d
nop
nop
nop
	movbe	[ecx],rbx
nop
nop
nop
	movbe	[ecx],r14
nop
nop
nop
	movbe	word [ecx],bx
nop
nop
nop
	movbe	word [ecx],r14w
nop
nop
nop
	movbe	dword [ecx],ebx
nop
nop
nop
	movbe	dword [ecx],r14d
nop
nop
nop
	movbe	qword [ecx],rbx
nop
nop
nop
	movbe	qword [ecx],r14
nop
nop
nop
	ENTER	4,4
nop
nop
nop
	IN	al,dx
nop
nop
nop
	IN	ax,dx
nop
nop
nop
	IN	eax,dx
nop
nop
nop
	INT	22
nop
nop
nop
	OUT	dx,al
nop
nop
nop
	OUT	dx,ax
nop
nop
nop
	OUT	dx,eax
nop
nop
nop
	 cmpsb	
nop
nop
nop
	rep cmpsb	
nop
nop
nop
	repnz cmpsb	
nop
nop
nop
	repne cmpsb	
nop
nop
nop
	repz cmpsb	
nop
nop
nop
	repe cmpsb	
nop
nop
nop
	 cmpsw	
nop
nop
nop
	rep cmpsw	
nop
nop
nop
	repnz cmpsw	
nop
nop
nop
	repne cmpsw	
nop
nop
nop
	repz cmpsw	
nop
nop
nop
	repe cmpsw	
nop
nop
nop
	 cmpsd	
nop
nop
nop
	rep cmpsd	
nop
nop
nop
	repnz cmpsd	
nop
nop
nop
	repne cmpsd	
nop
nop
nop
	repz cmpsd	
nop
nop
nop
	repe cmpsd	
nop
nop
nop
	 insb	
nop
nop
nop
	rep insb	
nop
nop
nop
	repnz insb	
nop
nop
nop
	repne insb	
nop
nop
nop
	repz insb	
nop
nop
nop
	repe insb	
nop
nop
nop
	 insw	
nop
nop
nop
	rep insw	
nop
nop
nop
	repnz insw	
nop
nop
nop
	repne insw	
nop
nop
nop
	repz insw	
nop
nop
nop
	repe insw	
nop
nop
nop
	 insd	
nop
nop
nop
	rep insd	
nop
nop
nop
	repnz insd	
nop
nop
nop
	repne insd	
nop
nop
nop
	repz insd	
nop
nop
nop
	repe insd	
nop
nop
nop
	 lodsb	
nop
nop
nop
	rep lodsb	
nop
nop
nop
	repnz lodsb	
nop
nop
nop
	repne lodsb	
nop
nop
nop
	repz lodsb	
nop
nop
nop
	repe lodsb	
nop
nop
nop
	 lodsw	
nop
nop
nop
	rep lodsw	
nop
nop
nop
	repnz lodsw	
nop
nop
nop
	repne lodsw	
nop
nop
nop
	repz lodsw	
nop
nop
nop
	repe lodsw	
nop
nop
nop
	 lodsd	
nop
nop
nop
	rep lodsd	
nop
nop
nop
	repnz lodsd	
nop
nop
nop
	repne lodsd	
nop
nop
nop
	repz lodsd	
nop
nop
nop
	repe lodsd	
nop
nop
nop
	 lodsw	
nop
nop
nop
	rep lodsw	
nop
nop
nop
	repnz lodsw	
nop
nop
nop
	repne lodsw	
nop
nop
nop
	repz lodsw	
nop
nop
nop
	repe lodsw	
nop
nop
nop
	 movsb	
nop
nop
nop
	rep movsb	
nop
nop
nop
	repnz movsb	
nop
nop
nop
	repne movsb	
nop
nop
nop
	repz movsb	
nop
nop
nop
	repe movsb	
nop
nop
nop
	 movsw	
nop
nop
nop
	rep movsw	
nop
nop
nop
	repnz movsw	
nop
nop
nop
	repne movsw	
nop
nop
nop
	repz movsw	
nop
nop
nop
	repe movsw	
nop
nop
nop
	 movsd	
nop
nop
nop
	rep movsd	
nop
nop
nop
	repnz movsd	
nop
nop
nop
	repne movsd	
nop
nop
nop
	repz movsd	
nop
nop
nop
	repe movsd	
nop
nop
nop
	 outsb	
nop
nop
nop
	rep outsb	
nop
nop
nop
	repnz outsb	
nop
nop
nop
	repne outsb	
nop
nop
nop
	repz outsb	
nop
nop
nop
	repe outsb	
nop
nop
nop
	 outsw	
nop
nop
nop
	rep outsw	
nop
nop
nop
	repnz outsw	
nop
nop
nop
	repne outsw	
nop
nop
nop
	repz outsw	
nop
nop
nop
	repe outsw	
nop
nop
nop
	 outsd	
nop
nop
nop
	rep outsd	
nop
nop
nop
	repnz outsd	
nop
nop
nop
	repne outsd	
nop
nop
nop
	repz outsd	
nop
nop
nop
	repe outsd	
nop
nop
nop
	 scasb	
nop
nop
nop
	rep scasb	
nop
nop
nop
	repnz scasb	
nop
nop
nop
	repne scasb	
nop
nop
nop
	repz scasb	
nop
nop
nop
	repe scasb	
nop
nop
nop
	 scasw	
nop
nop
nop
	rep scasw	
nop
nop
nop
	repnz scasw	
nop
nop
nop
	repne scasw	
nop
nop
nop
	repz scasw	
nop
nop
nop
	repe scasw	
nop
nop
nop
	 scasd	
nop
nop
nop
	rep scasd	
nop
nop
nop
	repnz scasd	
nop
nop
nop
	repne scasd	
nop
nop
nop
	repz scasd	
nop
nop
nop
	repe scasd	
nop
nop
nop
	 stosb	
nop
nop
nop
	rep stosb	
nop
nop
nop
	repnz stosb	
nop
nop
nop
	repne stosb	
nop
nop
nop
	repz stosb	
nop
nop
nop
	repe stosb	
nop
nop
nop
	 stosw	
nop
nop
nop
	rep stosw	
nop
nop
nop
	repnz stosw	
nop
nop
nop
	repne stosw	
nop
nop
nop
	repz stosw	
nop
nop
nop
	repe stosw	
nop
nop
nop
	 stosd	
nop
nop
nop
	rep stosd	
nop
nop
nop
	repnz stosd	
nop
nop
nop
	repne stosd	
nop
nop
nop
	repz stosd	
nop
nop
nop
	repe stosd	
nop
nop
nop
	 cmpsq	
nop
nop
nop
	rep cmpsq	
nop
nop
nop
	repnz cmpsq	
nop
nop
nop
	repne cmpsq	
nop
nop
nop
	repz cmpsq	
nop
nop
nop
	repe cmpsq	
nop
nop
nop
	 lodsq	
nop
nop
nop
	rep lodsq	
nop
nop
nop
	repnz lodsq	
nop
nop
nop
	repne lodsq	
nop
nop
nop
	repz lodsq	
nop
nop
nop
	repe lodsq	
nop
nop
nop
	 movsq	
nop
nop
nop
	rep movsq	
nop
nop
nop
	repnz movsq	
nop
nop
nop
	repne movsq	
nop
nop
nop
	repz movsq	
nop
nop
nop
	repe movsq	
nop
nop
nop
	 scasq	
nop
nop
nop
	rep scasq	
nop
nop
nop
	repnz scasq	
nop
nop
nop
	repne scasq	
nop
nop
nop
	repz scasq	
nop
nop
nop
	repe scasq	
nop
nop
nop
	 stosq	
nop
nop
nop
	rep stosq	
nop
nop
nop
	repnz stosq	
nop
nop
nop
	repne stosq	
nop
nop
nop
	repz stosq	
nop
nop
nop
	repe stosq	
nop
nop
nop
	cmpxchg	[ecx],bl
nop
nop
nop
	cmpxchg	[ecx],sil
nop
nop
nop
	cmpxchg	[ecx],r14b
nop
nop
nop
	cmpxchg	[ecx],bx
nop
nop
nop
	cmpxchg	[ecx],r14w
nop
nop
nop
	cmpxchg	[ecx],ebx
nop
nop
nop
	cmpxchg	[ecx],r14d
nop
nop
nop
	cmpxchg	[ecx],rbx
nop
nop
nop
	cmpxchg	[ecx],r14
nop
nop
nop
	xadd	[ecx],bl
nop
nop
nop
	xadd	[ecx],sil
nop
nop
nop
	xadd	[ecx],r14b
nop
nop
nop
	xadd	[ecx],bx
nop
nop
nop
	xadd	[ecx],r14w
nop
nop
nop
	xadd	[ecx],ebx
nop
nop
nop
	xadd	[ecx],r14d
nop
nop
nop
	xadd	[ecx],rbx
nop
nop
nop
	xadd	[ecx],r14
nop
nop
nop
	cmpxchg8b	[ecx]
nop
nop
nop
	cmpxchg16b	[ecx]
nop
nop
nop
	imul	ax, 44
nop
nop
nop
	imul	ax, 0x1234
nop
nop
nop
	imul	ax, byte 44
nop
nop
nop
	imul	ax, word 0x1234
nop
nop
nop
	imul	eax, 44
nop
nop
nop
	imul	eax, 0x1234
nop
nop
nop
	imul	eax, byte 44
nop
nop
nop
	imul	eax, dword 0x1234
nop
nop
nop
	imul	rax, 44
nop
nop
nop
	imul	rax, 0x1234
nop
nop
nop
	imul	rax, byte 44
nop
nop
nop
	imul	rax, qword 0x1234
nop
nop
nop
	imul	ax, [ecx], 44
nop
nop
nop
	imul	ax, [ecx], byte 44
nop
nop
nop
	imul	ax, [ecx]
nop
nop
nop
	imul	eax, [ecx], 123444
nop
nop
nop
	imul	eax, [ecx], byte 44
nop
nop
nop
	imul	eax, [ecx]
nop
nop
nop
	imul	rax, [ecx], 123444
nop
nop
nop
	imul	rax, [ecx], byte 44
nop
nop
nop
	imul	rax, [ecx]
nop
nop
nop
	imul	r15w, 44
nop
nop
nop
	imul	r15w, 0x1234
nop
nop
nop
	imul	r15w, byte 44
nop
nop
nop
	imul	r15w, word 0x1234
nop
nop
nop
	imul	r15d, 44
nop
nop
nop
	imul	r15d, 0x1234
nop
nop
nop
	imul	r15d, byte 44
nop
nop
nop
	imul	r15d, dword 0x1234
nop
nop
nop
	imul	r15, 44
nop
nop
nop
	imul	r15, 0x1234
nop
nop
nop
	imul	r15, byte 44
nop
nop
nop
	imul	r15, qword 0x1234
nop
nop
nop
	imul	r15w, [ecx], 44
nop
nop
nop
	imul	r15w, [ecx], byte 44
nop
nop
nop
	imul	r15w, [ecx]
nop
nop
nop
	imul	r15d, [ecx], 123444
nop
nop
nop
	imul	r15d, [ecx], byte 44
nop
nop
nop
	imul	r15d, [ecx]
nop
nop
nop
	imul	r15, [ecx], 123444
nop
nop
nop
	imul	r15, [ecx], byte 44
nop
nop
nop
	imul	r15, [ecx]
nop
nop
nop
	imul	byte [ecx]
nop
nop
nop
	imul	word [ecx]
nop
nop
nop
	imul	dword [ecx]
nop
nop
nop
	imul	qword [ecx]
nop
nop
nop
	imul	byte [rcx]
nop
nop
nop
	imul	word [rcx]
nop
nop
nop
	imul	dword [rcx]
nop
nop
nop
	imul	qword [rcx]
nop
nop
nop
	imul	byte [r13]
nop
nop
nop
	imul	word [r13]
nop
nop
nop
	imul	dword [r13]
nop
nop
nop
	imul	qword [r13]
nop
nop
nop
	mov	al, [0x12345678]
nop
nop
nop
	mov	al, [fs:0x12345678]
nop
nop
nop
	mov	al, byte [0x12345678]
nop
nop
nop
	mov	al, byte [fs:0x12345678]
nop
nop
nop
	mov	ax, [0x12345678]
nop
nop
nop
	mov	ax, [fs:0x12345678]
nop
nop
nop
	mov	ax, word [0x12345678]
nop
nop
nop
	mov	ax, word [fs:0x12345678]
nop
nop
nop
	mov	eax, [0x12345678]
nop
nop
nop
	mov	eax, [fs:0x12345678]
nop
nop
nop
	mov	eax, dword [0x12345678]
nop
nop
nop
	mov	eax, dword [fs:0x12345678]
nop
nop
nop
	mov	rax, [0x12345678]
nop
nop
nop
	mov	rax, [fs:0x12345678]
nop
nop
nop
	mov	rax, qword [0x12345678]
nop
nop
nop
	mov	rax, qword [fs:0x12345678]
nop
nop
nop
	mov	[0x12345678], al
nop
nop
nop
	mov	[fs:0x12345678], al
nop
nop
nop
	mov	byte [0x12345678], al
nop
nop
nop
	mov	byte [fs:0x12345678], al
nop
nop
nop
	mov	[0x12345678], ax
nop
nop
nop
	mov	[fs:0x12345678], ax
nop
nop
nop
	mov	word [0x12345678], ax
nop
nop
nop
	mov	word [fs:0x12345678], ax
nop
nop
nop
	mov	[0x12345678], eax
nop
nop
nop
	mov	[fs:0x12345678], eax
nop
nop
nop
	mov	dword [0x12345678], eax
nop
nop
nop
	mov	dword [fs:0x12345678], eax
nop
nop
nop
	mov	[0x12345678], rax
nop
nop
nop
	mov	[fs:0x12345678], rax
nop
nop
nop
	mov	qword [0x12345678], rax
nop
nop
nop
	mov	qword [fs:0x12345678], rax
nop
nop
nop
	mov	al,5
nop
nop
nop
	mov	ax,0x1234
nop
nop
nop
	mov	eax,0x12345678
nop
nop
nop
	mov	rax,0x12345678
nop
nop
nop
	mov	al,byte 5
nop
nop
nop
	mov	ax,word 0x1234
nop
nop
nop
	mov	eax,dword 0x12345678
nop
nop
nop
	mov	rax,qword 0x12345678
nop
nop
nop
	mov	bl,[ecx]
nop
nop
nop
	mov	dil,[ecx]
nop
nop
nop
	mov	r15b,[ecx]
nop
nop
nop
	mov	bx,[ecx]
nop
nop
nop
	mov	r14w,[ecx]
nop
nop
nop
	mov	ebx,[ecx]
nop
nop
nop
	mov	r14d,[ecx]
nop
nop
nop
	mov	rbx,[ecx]
nop
nop
nop
	mov	r14,[ecx]
nop
nop
nop
	mov	bl,byte [ecx]
nop
nop
nop
	mov	dil,byte [ecx]
nop
nop
nop
	mov	r15b,byte [ecx]
nop
nop
nop
	mov	bx,word [ecx]
nop
nop
nop
	mov	r14w,word [ecx]
nop
nop
nop
	mov	ebx,dword [ecx]
nop
nop
nop
	mov	r14d,dword [ecx]
nop
nop
nop
	mov	rbx,qword [ecx]
nop
nop
nop
	mov	r14,qword [ecx]
nop
nop
nop
	mov	[ecx],bl
nop
nop
nop
	mov	[ecx],dil
nop
nop
nop
	mov	[ecx],r15b
nop
nop
nop
	mov	[ecx],bx
nop
nop
nop
	mov	[ecx],r14w
nop
nop
nop
	mov	[ecx],ebx
nop
nop
nop
	mov	[ecx],r14d
nop
nop
nop
	mov	[ecx],rbx
nop
nop
nop
	mov	[ecx],r14
nop
nop
nop
	mov	byte [ecx],bl
nop
nop
nop
	mov	byte [ecx],dil
nop
nop
nop
	mov	byte [ecx],r15b
nop
nop
nop
	mov	word [ecx],bx
nop
nop
nop
	mov	word [ecx],r14w
nop
nop
nop
	mov	dword [ecx],ebx
nop
nop
nop
	mov	dword [ecx],r14d
nop
nop
nop
	mov	qword [ecx],rbx
nop
nop
nop
	mov	qword [ecx],r14
nop
nop
nop
	mov	byte [ecx],44
nop
nop
nop
	mov	word [ecx],44
nop
nop
nop
	mov	dword [ecx],44
nop
nop
nop
	mov	qword [ecx],44
nop
nop
nop
	mov	byte [ecx],byte 44
nop
nop
nop
	mov	word [ecx],word 44
nop
nop
nop
	mov	dword [ecx],dword 44
nop
nop
nop
	mov	qword [ecx],qword 44
nop
nop
nop
	mov	bl,44
nop
nop
nop
	mov	dil,44
nop
nop
nop
	mov	r15b,44
nop
nop
nop
	mov	bx,44
nop
nop
nop
	mov	r14w,44
nop
nop
nop
	mov	ebx,44
nop
nop
nop
	mov	r14d,44
nop
nop
nop
	mov	rbx,44
nop
nop
nop
	mov	r14,44
nop
nop
nop
	mov	bl,byte 44
nop
nop
nop
	mov	dil,byte 44
nop
nop
nop
	mov	r15b,byte 44
nop
nop
nop
	mov	bx,word 44
nop
nop
nop
	mov	r14w,word 44
nop
nop
nop
	mov	ebx,dword 44
nop
nop
nop
	mov	r14d,dword 44
nop
nop
nop
	mov	rbx,qword 44
nop
nop
nop
	mov	r14,qword 44
nop
nop
nop
	mov	fs, bx
nop
nop
nop
	mov	fs, r15w
nop
nop
nop
	mov	fs, ebx
nop
nop
nop
	mov	fs, r15d
nop
nop
nop
	mov	fs, rbx
nop
nop
nop
	mov	fs, r15
nop
nop
nop
	mov	fs, [ecx]
nop
nop
nop
	mov	fs, word [ecx]
nop
nop
nop
	mov	bx, fs
nop
nop
nop
	mov	r15w, fs
nop
nop
nop
	mov	ebx, fs
nop
nop
nop
	mov	r15d, fs
nop
nop
nop
	mov	[ecx], fs
nop
nop
nop
	mov	word [ecx], fs
nop
nop
nop
	ret	
nop
nop
nop
	ret	0x1234
nop
nop
nop
	retf	
nop
nop
nop
	retf	0x1234
nop
nop
nop
	test	al,5
nop
nop
nop
	test	ax,0x1234
nop
nop
nop
	test	eax,0x12345678
nop
nop
nop
	test	rax,0x12345678
nop
nop
nop
	test	bl,[ecx]
nop
nop
nop
	test	dil,[ecx]
nop
nop
nop
	test	r15b,[ecx]
nop
nop
nop
	test	bx,[ecx]
nop
nop
nop
	test	r14w,[ecx]
nop
nop
nop
	test	ebx,[ecx]
nop
nop
nop
	test	r14d,[ecx]
nop
nop
nop
	test	rbx,[ecx]
nop
nop
nop
	test	r14,[ecx]
nop
nop
nop
	test	bl,byte [ecx]
nop
nop
nop
	test	dil,byte [ecx]
nop
nop
nop
	test	r15b,byte [ecx]
nop
nop
nop
	test	bx,word [ecx]
nop
nop
nop
	test	r14w,word [ecx]
nop
nop
nop
	test	ebx,dword [ecx]
nop
nop
nop
	test	r14d,dword [ecx]
nop
nop
nop
	test	rbx,qword [ecx]
nop
nop
nop
	test	r14,qword [ecx]
nop
nop
nop
	test	[ecx],bl
nop
nop
nop
	test	[ecx],dil
nop
nop
nop
	test	[ecx],r15b
nop
nop
nop
	test	[ecx],bx
nop
nop
nop
	test	[ecx],r14w
nop
nop
nop
	test	[ecx],ebx
nop
nop
nop
	test	[ecx],r14d
nop
nop
nop
	test	[ecx],rbx
nop
nop
nop
	test	[ecx],r14
nop
nop
nop
	test	byte [ecx],bl
nop
nop
nop
	test	byte [ecx],dil
nop
nop
nop
	test	byte [ecx],r15b
nop
nop
nop
	test	word [ecx],bx
nop
nop
nop
	test	word [ecx],r14w
nop
nop
nop
	test	dword [ecx],ebx
nop
nop
nop
	test	dword [ecx],r14d
nop
nop
nop
	test	qword [ecx],rbx
nop
nop
nop
	test	qword [ecx],r14
nop
nop
nop
	test	bl,44
nop
nop
nop
	test	dil,44
nop
nop
nop
	test	r15b,44
nop
nop
nop
	test	bx,44
nop
nop
nop
	test	r14w,44
nop
nop
nop
	test	ebx,44
nop
nop
nop
	test	r14d,44
nop
nop
nop
	test	rbx,44
nop
nop
nop
	test	r14,44
nop
nop
nop
	test	bl,byte 44
nop
nop
nop
	test	dil,byte 44
nop
nop
nop
	test	r15b,byte 44
nop
nop
nop
	test	bx,word 44
nop
nop
nop
	test	r14w,word 44
nop
nop
nop
	test	ebx,dword 44
nop
nop
nop
	test	r14d,dword 44
nop
nop
nop
	test	rbx,qword 44
nop
nop
nop
	test	r14,qword 44
nop
nop
nop
	test	byte [ecx],44
nop
nop
nop
	test	word [ecx],44
nop
nop
nop
	test	dword [ecx],44
nop
nop
nop
	test	qword [ecx],44
nop
nop
nop
	test	byte [ecx],byte 44
nop
nop
nop
	test	word [ecx],word 44
nop
nop
nop
	test	dword [ecx],dword 44
nop
nop
nop
	test	qword [ecx],qword 44
nop
nop
nop
	test	bl, cl
nop
nop
nop
	test	bl, al
nop
nop
nop
	test	bl, sil
nop
nop
nop
	test	bx, cx
nop
nop
nop
	test	bx, r14w
nop
nop
nop
	test	r14w, bx
nop
nop
nop
	test	ebx, ecx
nop
nop
nop
	test	ebx, r14d
nop
nop
nop
	test	r14d, ebx
nop
nop
nop
	test	rbx, rcx
nop
nop
nop
	test	rbx, r14
nop
nop
nop
	test	r14, rbx
nop
nop
nop
	xchg	ax,cx
nop
nop
nop
	xchg	eax,ecx
nop
nop
nop
	xchg	rax,rcx
nop
nop
nop
	xchg	ax,r11w
nop
nop
nop
	xchg	eax,r10d
nop
nop
nop
	xchg	rax,r15
nop
nop
nop
	xchg	cx,ax
nop
nop
nop
	xchg	ecx,eax
nop
nop
nop
	xchg	rcx,rax
nop
nop
nop
	xchg	r11w,ax
nop
nop
nop
	xchg	r10d,eax
nop
nop
nop
	xchg	r15,rax
nop
nop
nop
	xchg	bl,[ecx]
nop
nop
nop
	xchg	dil,[ecx]
nop
nop
nop
	xchg	r15b,[ecx]
nop
nop
nop
	xchg	bx,[ecx]
nop
nop
nop
	xchg	r14w,[ecx]
nop
nop
nop
	xchg	ebx,[ecx]
nop
nop
nop
	xchg	r14d,[ecx]
nop
nop
nop
	xchg	rbx,[ecx]
nop
nop
nop
	xchg	r14,[ecx]
nop
nop
nop
	xchg	r15b, al
nop
nop
nop
	xchg	sil,al
nop
nop
nop
	xchg	bl, al
nop
nop
nop
	xchg	al, r15b
nop
nop
nop
	xchg	al, sil
nop
nop
nop
	xchg	al, bl
nop
nop
nop
	xchg	bl,byte [ecx]
nop
nop
nop
	xchg	dil,byte [ecx]
nop
nop
nop
	xchg	r15b,byte [ecx]
nop
nop
nop
	xchg	bx,word [ecx]
nop
nop
nop
	xchg	r14w,word [ecx]
nop
nop
nop
	xchg	ebx,dword [ecx]
nop
nop
nop
	xchg	r14d,dword [ecx]
nop
nop
nop
	xchg	rbx,qword [ecx]
nop
nop
nop
	xchg	r14,qword [ecx]
nop
nop
nop
	xchg	[ecx],bl
nop
nop
nop
	xchg	[ecx],dil
nop
nop
nop
	xchg	[ecx],r15b
nop
nop
nop
	xchg	[ecx],bx
nop
nop
nop
	xchg	[ecx],r14w
nop
nop
nop
	xchg	[ecx],ebx
nop
nop
nop
	xchg	[ecx],r14d
nop
nop
nop
	xchg	[ecx],rbx
nop
nop
nop
	xchg	[ecx],r14
nop
nop
nop
	xlat	
nop
nop
nop
