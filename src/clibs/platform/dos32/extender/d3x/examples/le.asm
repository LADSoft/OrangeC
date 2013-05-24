;께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께
;			  D3X DOS-Extender application
;					
;				  (c) GrayWolf
;께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께께

        .386

Stack_size	=	1000h

call16		macro	ofs
        db	9ah
        dp	ofs
        endm

retfd		macro	n
        db	66h
    ifb	<n>
        retf
    else
        retf	n
    endif
        endm

_CODE16		segment	byte public use16 'CODE'
            ASSUME	cs:_CODE16

start16:
        jmp	far begin

msg16		db	'WATCOM WASM: ',0

proc16:
        mov	eax,offset msg16
        call	far puts16
        retfd

puts16		proc	far
        push	esi
        xchg	eax,esi
        mov	ah,02h
    puts16_loop:
        lods	byte ptr ds:[esi]
        test	al,al
        jz	puts16_end
        mov	dl,al
        int	21h
        jmp	puts16_loop
    puts16_end:
        pop	esi
        ret
puts16		endp

_CODE16		ends

_TEXT		segment	byte public use32 'CODE'
            ASSUME	cs:_TEXT,ds:_TEXT

begin:
        call16	proc16

        mov	ah,09h
        mov	edx,offset msg
        mov	dx,offset msg
        int	21h
exit:
        mov	ax,4c00h
        int	21h

msg		db	'Hello, world',13,10,'$'

_TEXT		ends

_STACK		segment	para stack use32 'STACK'
        db	Stack_size dup (?)
_STACK		ends

            end	start16
