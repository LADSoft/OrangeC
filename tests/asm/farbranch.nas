[bits 16]

	call far [xxx]
	call far [cs:xxx]
	jmp far [xxx]
	jmp far [cs:xxx]

xxx dw 0

[bits 32]

	call far [xxx1]
	call far [cs:xxx1]
	jmp far [xxx1]
	jmp far [cs:xxx1]

xxx1 dw 0
