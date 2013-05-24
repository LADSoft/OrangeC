;膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊
;			  D3X DOS-Extender application
;					
;				  (c) GrayWolf
;膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊膊

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	INCLUDE
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	DIRECTIVES
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
group		DATA	_TEXT _BSS

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	DEFINES, EQUATES, MACROS & STRUCTURES
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
Stack_size	equ	1000h

%macro		puts	1+
        call	_puts
        db	%1,0
%endmacro

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	EXTERNS & PUBLIX
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	Flat 32bit segment
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
segment		_TEXT	align=16 public use32 class=CODE

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	Main protected mode procedure
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
..start:
        puts	'D3X1 NASM: Hello, world'

        xor	al,al
exit:
        mov	ah,4ch
        int	21h

_puts:
        pop	esi
        mov	ah,02h
    .next:
        lodsb
        test	al,al
        jz	.ends
        mov	dl,al
        int	21h
        jmp	short .next
    .ends:
        mov	dl,0dh
        int	21h
        mov	dl,0ah
        int	21h
        jmp	esi

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	DATA
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	BSS
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
segment		_BSS	align=16 public use32 class=BSS

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	STACK
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
segment		_STACK	align=16 stack use32 class=STACK
        resb	Stack_size
