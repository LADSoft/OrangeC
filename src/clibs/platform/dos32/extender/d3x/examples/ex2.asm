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
        .386
DATA		group	_TEXT,_BSS
        assume	cs:_TEXT,ds:DATA

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	DEFINES, EQUATES, MACROS & STRUCTURES
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
Stack_size	=	1000h

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	EXTERNS & PUBLIX
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北

;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
;	Flat 32bit segment
;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北
_TEXT		segment	para public use32 'CODE'

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	Main protected mode procedure
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
start		label	near

        lea	esi,msg
        call	puts

        xor	al,al
exit:
        mov	ah,4ch
        int	21h

puts		proc	near
        mov	ah,02h
    print_string:
        lodsb
        test	al,al
        jz	end_string
        mov	dl,al
        int	21h
        jmp	print_string
    end_string:
        mov	dl,0dh
        int	21h
        mov	dl,0ah
        int	21h
        ret
puts		endp

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	DATA
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
msg		db	'D3X1 TASM: Hello, world',0

_TEXT		ends

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	BSS
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
_BSS		segment	para uninit public use32 'BSS'
_BSS		ends

;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
;	STACK
;氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨氨
_STACK		segment	para uninit stack use32 'STACK'
        db	Stack_size dup (?)
_STACK		ends

        end	start
