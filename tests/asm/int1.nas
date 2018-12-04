[bits 64]
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
