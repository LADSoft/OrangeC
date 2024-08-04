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

%include  "prints.ase"
%include  "input.ase"
%include  "mtrap.ase" 
%include  "dump.ase"

        global  entry, entrytobuf, diglen

    segment data USE32
InsideQuote	db	0	;flag if we're inside a quote
diglen  db      0               ; number of digits

    segment code USE32
;
; function to read a string and fill a buf with bytes based on the string's
; data
;
entrytobuf :
        call    WadeSpace
readlp:
        inc     si
        mov     bl,al
        cmp     al,"'"
        jz      readstr
        cmp     al,'"'
        jz      readstr
        dec     esi
        push    edi
    call	ReadNumber		; Else read number off command line
        pop     edi
        jc      eb_err                  ; Quit if error
;        push    eax
;        cli
;		call	swapintpage
;        pop     eax
        cmp     byte [memsize],1
        jl      rbyte
        je      rword
        jg      rdword
rbyte:
    mov	[edi],al		; Save value
        jmp     rjoin
rword:
        mov     [edi],ax
        jmp     rjoin
rdword:
        mov     [edi],eax
rjoin:
;		call	swapintpage
;        sti
        movzx   eax,byte [diglen]
        shr     eax,1
        add     edi,eax
readclcont:
    call	WadeSpace		; Wade through spaces
        jz      eb_ret           ;
    jmp	readlp			; Else get next value
readstr:
        lodsb
        cmp     al,bl
        jz      readclcont
        cmp     al,13
        jz      eb_ret
;        push    ax
;        cli
;		call	swapintpage
;        pop     ax
        mov     [edi],al
;		call	swapintpage
;        sti
        inc     edi
        jmp     readstr
eb_ret:
        clc
eb_err:
        ret
;
; Input function for a number
;
InputNumber	:	
    mov	byte [InsideQuote],0
    push	edx
    push	ecx
        push    ebx
    sub	ecx,ecx 		; Number of digits = 0
        sub     ebx,ebx                   ; Data = 0
lp:
    call	GetKey
    mov	ah,al			; AH = data
    cmp	al,' '			; Space, data is complete
    jz	near space		;
    cmp	al,13			;
    jz	near isenter		; ENTER = quit entering data
    cmp	al,8			; BACKSPACE or RUBOUT, handle it
    jz	bckspc		;
    cmp	al,7fh			;
    jz	bckspc		;
    cmp	al,'"'
    jz	near quote
    cmp	al,"'"
    jz	near quote
    test	byte [InsideQuote],1
    jnz	isquote
    cmp	al,60h
    jc	notlower
    and	al,~ 20h
notlower:
    sub	al,'0'			; Convert to binary, ignore if not valid
    jc	lp			;
    cmp	al,10			;
    jc	gotdigit		;
    sub	al,7			;
    cmp	al,10			;
    jc	lp			;
    cmp	al,16			;
    jnc	lp			;
gotdigit:
        cmp     cl,[diglen]             ; If got two digits don't accept
    jz	lp
        shl     ebx,4                    ; Add in the digit
        or      bl,al                   ;
writechar:
    mov	dl,ah			;
    call	PutChar
    inc	ecx			; Inc digit count
    jmp	lp			; Next digit
isquote:
        cmp     cl,[diglen]
    jz	lp
    mov	bl,al
    jmp	writechar
bckspc:
    or	ecx,ecx			; Get next digit if nothing in buffer
    jz	lp			;
    test	byte [InsideQuote],1
    jz	delnoquote
        cmp     cl,[diglen]
    jz	delnoquote
    xor	byte [InsideQuote],1
delnoquote:
        shr     ebx,4
    mov	dl,8			; Erase echoed char
    call	PutChar
    mov	dl,' '			;
    call	PutChar
    mov	dl,8			; Point at next echo space
    call	PutChar
    dec	cx			; Dec digit count
    jmp	lp
isenter:
    or	ecx,ecx			; Enter key, set carry and get out
    stc				;
    jmp	getout
quote:
    test	byte [InsideQuote],1
    jnz	lp
    xor	byte [InsideQuote],1
    jmp	writechar
space:
    or	cl,cl			; Space key, clear carry and get out
getout:
    pushf
        mov     al,[diglen]             ; Space to line up in columns
        inc     al
    sub	al,cl			;
    mov	cl,al			;
pslp:			
    call	PrintSpace		;
    loop	pslp			;
    popf				;
        mov     eax,ebx                   ; AX = number input
        pop     ebx
    pop	ecx
    pop	edx
    ret
;
; Number entry with prompt
;
entry	:	
        call    memsizechk              ; get size
        jc      near enterr
        mov     al,2
        mov     cl,[memsize]
        shl     al,cl
        mov     [diglen],al
    call	WadeSpace		; Wade through commad spaces
    jz	near enterr
    call	ReadAddress		; Read the address
    jc	near enterr			; Bad address ,error
    mov	edi,ebx
    mov	ecx,-1			;
    call	WadeSpace		; Wade through spaces
    jz	promptx		; Go do prompt version
        call    entrytobuf
        jc      near enterr2
        jmp     retok
promptx:
    call	crlf
    mov	eax,ebx                 ;
    call	PrintDword	; Print address
elp:
    call	PrintSpace		; Space over two spaces
    call	PrintSpace		;
;        cli
;		call	swapintpage
        cmp     byte [memsize],1
        jl      r3byte
        je      r3word
        jg      r3dword
r3byte:
        mov     al,[edi]             ; Save value
    call	PrintByte		;
        jmp     r3join
r3word:
        mov     ax,[edi]
        call    PrintWord
        jmp     r3join
r3dword:
        mov     eax,[edi]
        call    PrintDword
r3join:
;		call	swapintpage
;        sti
    mov	dl,'.'			; Print '.'
    call	PutChar
    push	ecx
    call	InputNumber		; Get a number
    pop	ecx
    jz	nextitem		; No number, go do next
        pushf
;        push    eax
;        cli
;		call	swapintpage
;        pop     eax
        cmp     byte [memsize],1
        jl      r2byte
        je      r2word
        jg      r2dword
r2byte:
    mov	[edi],al		; Save value
        jmp     r2join
r2word:
        mov     [edi],ax
        jmp     r2join
r2dword:
        mov     [edi],eax
r2join:
;		call	swapintpage
        popf
nextitem:
    jc	retok		; Quit if ENTER key pressed
    dec	cx			; Quit if end of segment
    jz	retok		;
        movzx   eax,byte [diglen]
        shr     eax,1
        add     edi,eax
        add     eax,ebx                 ; new in eax,old in ebx
        xchg    ebx,eax
        xor     eax,ebx                 ; pass a boundary where lower bits are 0?
        test    eax,8
        jnz     promptx
    jmp	elp
retok:
    clc				; No errors
    ret
enterr2:
enterr:		
    stc     			; Errors
dudone:
    ret
