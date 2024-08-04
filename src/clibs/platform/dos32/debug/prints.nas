;  Software License Agreement
;  
;      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
;  
;      This file is part of the Orange C Compiler package.
;  
;      The Orange C Compiler package is free software: you can redistribute it and/or modify
;      it under the terms of the GNU General Public License as published by
;      the Free Software Foundation, either version 3 of the License, or
;      (at your option) any later version.
;  
;      The Orange C Compiler package is distributed in the hope that it will be useful,
;      but WITHOUT ANY WARRANTY; without even the implied warranty of
;      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;      GNU General Public License for more details.
;  
;      You should have received a copy of the GNU General Public License
;      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
;  
;      contact information:
;          email: TouchStone222@runbox.com <David Lindauer>
;  

global	PrintSpace,PrintDword,PrintWord,PrintByte
    global	PureChar, PutChar, GetKey, Message, ScanKey
    global  crlf, olMessage, dgroupMessage

    segment data USE32
col	db	0

    segment code USE32

;
; dump a message
;
Message	:
    xchg	ebx,[esp]
    call	olMessage
    xchg	ebx,[esp]
    ret

dgroupMessage	:
olMessage :
    push	edx
mlp:
    mov	dl,[ebx]
    inc	ebx
    or	dl,dl
    jz	nomore
    call	PutChar
    jmp	mlp
nomore:
    pop	edx
    ret

;
; print a (pure) char
;
; chars are limited to ASCII unless the pure video option is set,
; in chich case we display everything but a few control chars the
; BIOS will try to interpret and wreck our display
;
PureChar	:
    cmp	dl,20h
    jnc	PutChar
    cmp	dl,13
    jz	ccr
    cmp	dl,10
    jz	ccr
    cmp	dl,7
    jz	ccr
    cmp	dl,8
    jz	ccr
    cmp	dl,9
    jz	ccr
    jmp	PutChar
ccr:
    mov	dl,'.'
;
; normal put char via bios.  Also logs to disk
;
PutChar	:
    cmp	dl,9
    jz	dotab
ocx:
    push	ebx
occon:
    cmp	dl,10
    jnz	norescol
    mov	byte [col],-1
norescol:
    inc	byte [col]
    mov	ah,2
    int	21h
    pop	ebx
    ret
dotab:
    push	ecx
    movzx	ecx,byte [col]
    inc	cl
    and	cl,7
    neg	cl
    add	cl,8
dtl:
    mov	dl,20h
    call	ocx
    loop	dtl
    pop	ecx
    ret
;
; keyboard input via bios
;
GetKey 	:
    push	edx
getkey2:
    sub	ah,ah
    int	16h
;	mov	ah,6
;	mov	dl,-1
;	int	21h
;	jz	getkey2
    pop	edx
    ret
;
; keyboard scan, used to halt long D and U commands
;
ScanKey	:
    push	edx
    mov	ah,1
    int	16h
    jz	scandone
    pushfd
    call	GetKey
    popfd
scandone:
    pop	edx
    ret
;
; put out a space
;
PrintSpace:
    push	edx
    mov	dl,20h		; Get a space
    call	PutChar
    pop	edx
    ret
;
; put out a CR/LF sequence
;
crlf:
    push	edx
    mov	dl,13		; Get a CR
    call	PutChar
    mov	dl,10		; Get a LF
    call	PutChar
    pop	edx
    ret
;
; print various hex numbers
;
PrintDword:
    push	eax		; To print a dword
    shr	eax,16		; Print the high 16 bits
    call	PrintWord
    pop	eax		; And the low 16 bits
PrintWord:
    push	eax		; To print a word
    mov	al,ah		; Print the high byte
    call	PrintByte
    pop	eax		; And the low byte
PrintByte:
    push	eax		; To print a byte
    shr	al,4		; Print the high nibble
    call	printnibble
    pop	eax		; And the low nibble
printnibble:
    and	al,0fh		; Get a nibble
    add	al,'0'		; Make it numeric
    cmp	al,'9'		; If supposed to be alphabetic
    jle	onib
    add	al,7		; Add 7
onib:
    push	edx		; Save DX through the call
    mov	dl,al
    call	PutChar
    pop	edx		;
    ret
END