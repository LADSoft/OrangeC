/*
 *                        D3X DOS-Extender application
 *                                      
 *                                (c) GrayWolf
 */

		.text

		.global	start
start:
		call	_main
		pushl	%eax
		call	_exit

		.balign	4
		.global	_exit
_exit:
		popl	%eax
		popl	%eax
		movb	$0x4c, %ah
		int	$0x21

		.balign	4
		.global	_main
_main:
		pushl	$msg
		call	_puts
		popl	%eax
		xorl	%eax, %eax
		ret

		.balign	4
		.global	_puts
_puts:
		popl	%edx
		popl	%ecx
		pushl	%ecx
		pushl	%edx
		movb	$0x02, %ah
	print_string:
		movb	(%ecx), %al
		testb	%al, %al
		jz	end_string
		cmpb	$'\n', %al
		jne	print_char
		movb	$'\r', %dl
		int	$0x21
		movb	$'\n', %al
	print_char:
		movb	%al, %dl
		int	$0x21
		incl	%ecx
		jmp	print_string
	end_string:
		movb	$'\r', %dl
		int	$0x21
		movb	$'\n', %dl
		int	$0x21
		movzbl	%al, %eax
		ret

		.data

msg:		.asciz	"DJGPP AS: Hello, world"
