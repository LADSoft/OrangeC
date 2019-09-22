
{hello}{goodbye} mov $55,%eax
	imul $4,%eax
        imul $5,%eax,%ebx
	mov %ecx,(%eax)
        mov %ecx, (%eax, %ebx)
        mov %ecx, (%eax, %ebx, 4)
        mov %ecx, (,%ebx,4)
	mov %ecx,mm(%eax)
        mov %ecx, mm(%eax, %ebx)
        mov %ecx, mm(%eax, %ebx, 4)
        mov %ecx, mm(,%ebx,4)
        mov %ecx, mm(,1)
	mov %ecx,4(%eax)
        mov %ecx, 5(%eax, %ebx)
        mov %ecx, 4096(%eax, %ebx, 4)
        mov %ecx, -512(,%ebx,4)
aa:
	movb $5,(%edx) # hello movb $5,mm
	fsub %st,%st(1) ; fsub %st(1), %st(0) ; mov %es:mm,%edx ; mov %edx, %es:(%eax)
	call *mm
	jne aa
	jmp aa
	ljmp mm
	lcall mm
mm	.4byte 0