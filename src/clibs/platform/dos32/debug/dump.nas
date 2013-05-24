;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; Function: Handle the Dump command
    ;MASM MODE
%include  "prints.ase"
%include  "input.ase"
%include  "mtrap.ase"

DUMPLEN EQU 80h
    global	dump, redump
    global  index,memsizechk, memsize
    ;extern w_setwind
    ;extern optfs
    segment data USE32
index	  dd	0	; Default for next dump
charcount db	16
dcr	  db	13
memsize   db	0	;b/w/d = 0/1/2
;Debug reads each memory paragraph twice, once for the hex values and a
;second time for the ASCII values. This screws up at least two types of
;memory: memory-mapped IO when reads change the state of the device, and
;and FIFO devices with internal counters. So we read each location only once
;into this buffer, and then read the buffer to create the ASCII.
linedata times	64 [db 0DEh]	   ;holds line so we read it only once
                ;temporarily 64 to handle screwups, should be 16

    segment code USE32
blanklp        :
blanklplab:
    call	PrintSpace	; Dump spaces
    loop	blanklplab
    ret
;
; get the size of memory chunks to deal with
;
memsizechk  :
    lodsb
    clc		;assume it will come out okay
    mov	bl,000h  ; might be a byte
    cmp	al,'b'
    jz	newsize
    mov	bl,001h  ; might be a word
    cmp	al,'w'
    jz	newsize
    mov	bl,002h  ; might be a doubleword
    cmp	al,'d'
    jz	newsize
    cmp	al,' '
    jz	oldsize
    cmp	al,13	   ; found CR after command letter
    je	memsizecr
    mov	bl,0h	   ; revert size to byte
    stc		   ; was not one of [CR,b,w,d,' '], flag error
newsize:
    mov	[memsize],bl
oldsize:
    ret
memsizecr:
    dec esi	 ; back up pointer so rest of dump routine sees the CR
    ret
space :
    mov	al,[charcount]	; Get count of amount to space over
    sub	al,cl		;
    jz	short spw	; Don't space over any, just put out hex
    movzx	eax,al
    push	ecx		; Else ecx = spacecount * 3
    mov	ecx,eax		;
    imul	ecx,edx		;  5/2	spaces per byte
    xchg	ebx,ecx
    shr		ebx,cl
    xchg	ebx,ecx
    call	blanklp
    pop	cx
spw:
    ret
readlinedata :
    cli
;	call swapintpage
    push	ecx
    mov	edi,linedata	; Now get the data to our buffer
    rep     movsb
    pop	ecx
;	call 	swapintpage
    sti
    mov	esi,linedata
    ret
; Dump one line
dumpline	:
    push	esi
    push	edx
    push	ebx			; EBX MUST be on second of stack
    push	ecx			; ECX MUST be on top of stack
;	push	edx
;	mov		dl,C_CODEBYTES
;	call	PutChar
;	pop		edx
    
    cmp	ecx,16
        jl      doline
        mov     ecx,16
doline:
    sub	[esp],ecx	; Decrement count which is on stack
    add	[esp+4],ecx	; Increment address which is on stack
    cmp	byte [memsize],1	; 0/1/2 = byte/word/dword
    jl	dolineb
    je	dolinew
    jg	near dolined
dolineb:
puthexb:
    call	readlinedata
    push	ecx
hexlpb:
    test	ecx,3
    jnz	hspb
    cmp	cl,[charcount]
    jae	hspb
    mov	dl,'-'
    call	PutChar
    jmp	hjnb
hspb:
    call	PrintSpace	; Print a space
hjnb:
    lodsb
    call	PrintByte	; Print byte in hex
    loop	hexlpb		; Loop till done
        pop     ecx              ;
        cmp     ecx,16
        jz      near duascii
    push ecx
        mov     edx,3
    mov	bl,0
    call	space
    pop	ecx	
    jmp	duascii

dolinew:
puthexw:
    call	readlinedata
    push	ecx
    shr	ecx,1
    test	ecx,ecx
    jnz	hexlpw
        inc     ecx
hexlpw:
    call	PrintSpace	; Print a space
hjnw:
    lodsw
    call	PrintWord	; Print byte in hex
    loop	hexlpw		; Loop till done
        pop     ecx              ;
        cmp     ecx,16
    jz		duascii
        push    ecx
        shr     ecx,1
        test    ecx,ecx
        jnz     hjnw2
        inc     ecx
hjnw2:
        sub     ecx,8
        neg     ecx
        mov     eax,5
        imul    ecx,eax
        call    blanklp
    pop	ecx	
    jmp	duascii

dolined:
puthexd:
    call	readlinedata
    push	ecx
    shr	ecx,2
    test	ecx,ecx
    jnz	hexlpd
    inc		ecx
hexlpd:
    call	PrintSpace	; Print a space
hjnd:
    lodsd
    call	PrintDword	; Print byte in hex
    loop	hexlpd		; Loop till done
    pop	ecx		;
        cmp     ecx,16
        jz      duascii
        push    ecx
        shr     ecx,2
        test    ecx,ecx
        jnz     hjnd2
        inc     ecx
hjnd2:
        sub     ecx,4
        neg     ecx
        mov     eax,9
        imul    ecx,eax
        call    blanklp
    pop	ecx	
duascii:
;	mov		dl,C_ASCII
;	call	PutChar
    call	PrintSpace	; Print two spaces to seperate ASCII dump
    call	PrintSpace	;
    mov	esi,linedata
    sub	eax,eax		; Calculate amoun to space over
putascii:
    mov	dl,[esi] 	; Get char
    inc	esi		; Increment buffer
    call	PureChar
    loop	putascii
    pop	ecx		; Get count from stack
    pop	ebx		; Get address from stack
    pop	edx
    pop	esi
    ret
;
; Main DUMP routine
redump	:
    mov	esi,dcr
dump	:
    mov	ecx,DUMPLEN	; Default amount to dump
    mov	byte [charcount],16
    call	memsizechk	; parse for byte/word/dword
    jc	dudone		; bad syntax, leave
    call	WadeSpace	; Wade to end of spaces
    jz	short atIndex	;
    call	ReadAddress	; Else read start address and put into ebx
    jc	dudone		; Quit on error
    call	WadeSpace	; Wade through spaces
    jz	short dodump	;
        cmp     al,'l'
        jnz     dumpnl
        inc     esi
        call    ReadNumber
        jc      dudone
        mov     ecx,eax
        jmp     short dodump
dumpnl:
    call	ReadNumber	; Else read end offset and put into eax
    jc	dudone	  ;
    sub	eax,ebx 	; Calculate length of dump
    mov	ecx,eax 	;
        inc     ecx
    jmp	short dodump	; Go do dump
atIndex:
    mov	ebx,[index]	; Assume we want to dump from last index
dodump:
;	push	ecx
;	mov	cl,[memsize]	; round addresses down to nearest byte/word/dword
;	shr	ebx,cl
;	shl	ebx,cl
;	shr	eax,cl
;	shl	eax,cl
;	pop	ecx
    mov	[index],ebx
    cmp	ecx,10000h
    jc	dumpj
    mov	ecx,10000h
dumpj:
    movzx	eax,byte [charcount]
    dec	al
;	test	bl,al
;	jz	dumplp
;	inc	al
;	sub	ecx,eax
dumplp:
    call	ScanKey
    jnz	dusetadr
    push	ebx		;
    call	crlf
    pop	ebx		;
;	push	dx
;	mov		dl,C_ADDRESS
;	call	PutChar
;	pop		dx
;	mov	ax,dx		; Print the selector
;	call	PrintWord	;
;	push	dx		;
;	mov	dl,':'          ; Print a ':'
;	call	PutChar
;	pop	dx		;
    mov	esi,ebx
        mov     eax,ebx
;	test	[optdwordcommand],0ffh
;	jz	adrword
    call	PrintDword	; Print address
;	jmp	adrcmb
;adrword:
;	call	PrintWord
;adrcmb:
    call	dumpline	; Dump a line
    or	ecx,ecx 	; Continue while count > 0
    jg	dumplp
dusetadr:		;
    mov	[index],ebx	; Save new index value
    clc			; No errors
dudone:
;	pushf
;	mov		dl,C_DEFAULT
;	call	PutChar
;	popf
    ret
