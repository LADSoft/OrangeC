;  Software License Agreement
;  
;      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

%include "dispatch.ase" 
%include "prints.ase" 
%include "mtrap.ase"
%include "regs.ase" 
%include "dump.ase" 
%include "entry.ase" 
%include "exec.ase"
%include "breaks.ase"
%include "dis.ase" 
%include "inout.ase"
%include "asm.asi"
%include "asm.ase"

    global	qerr, ReadNumber, ReadAddress, InputHandler
    global	WadeSpace, PageTrapErr, PageTrapUnerr, GetInputLine
    global	InputBuffer

    extern _exit
    extern domath
    ;extern drive
    extern fill 
    extern Help 
    extern search 
    extern fpcommand 
    extern move
    extern compare 
    extern Proceed

IBSIZE EQU 80

    segment data USE32
oldpagetrap	times 3 [dw	0]		; Temp store for user page trap
InputBuffer times IBSIZE [db	0]		; Input buffer
Commands db	"abcdefghimopqrstu?/"	; List of Commands
comlen	EQU $ - Commands			; Length of list
inquote	db	0
lastcmd db 0
    segment code USE32
;
; Print an error if command wrong
;
qerr	:	
    call	crlf
    sub	esi,InputBuffer-2; Calculate error pos
    mov	ecx,esi			;
    jecxz	qestart		;
    dec	ecx			;
    jecxz	qestart		;
qelp:					
    call	PrintSpace              ; Space over to error pos
         loop	qelp
qestart:
    mov	dl,'^'			; Display error
    call	PureChar
    stc				; Did an error
    ret	
;
; If paging traps, it comes here
;
PageTrapped	:	
    mov	esp,[rtoss]		;
    call	PageTrapUnerr		; Turn page trap off
    Msg	10,13,"Invalid paging"	; Message for page trap
    jmp	InputHandler		; Go do more input
;
; Set up monitor page trap error
;
PageTrapErr	:	
    pushad
    mov 	bl,14			; get user trap vect
    mov 	ax,204h
    int	31h
    
    mov	dword [oldpagetrap],edx;
    mov	word [oldpagetrap + 4],cx;
    mov	ecx,cs			; Set MONITOR trap interrupt
    mov	edx,PageTrapped	;
    mov	ax,205h
    int 	31h
    popad
    ret
;
; Set user page trap error ( unset monitor error)
;
PageTrapUnerr	:	
    pushad
    mov	edx,dword [oldpagetrap]	; Restore user value
    mov	cx,word [oldpagetrap + 4]	;
    mov	ax,205h
    int 	31h
    popad
    ret
;
; Read in a number
;
ReadNumber	:	
    push	ebx
    push	ecx
    push	edx
    call	ReadReg
    clc
    jz	gotnum
    sub	ebx,ebx			; Number = 0
    sub	ecx,ecx			; digits = 0
    mov	al,[esi]
    cmp	al,"'"
    jz	getstring
    cmp	al,'"'
    jz	getstring
rnlp:
    lodsb				; Get char & convert to uppercase
    cmp	al,60h
    jc	notlower
    and	al,~ 20h
notlower:
    sub	al,'0'    		; Convert to binary
    jc	short rn_done		; < '0' is an error
    cmp	al,10			; See if is a digit
    jc	short gotdigit		; Yes, got it
    sub	al,7			; Convert letters to binary
    cmp	al,16			; Make sure is < 'G'
    jnc	short rn_done		; Quit if not
    cmp	al,10			; MAke sure not < 'A'
    jc	short rn_done
gotdigit:
    shl	ebx,4			; It is a hex digit, add in
    or	bl,al			;
    inc	ecx			; Set flag to indicate we got digits
    jmp	rnlp
rn_done:
    dec	esi			; Point at first non-digit
rm_done2:
    mov	eax,ebx
    test	cl,-1			; See if got any
    jnz	gotnum			;
    stc				; No, error
gotnum:
    pop	edx
    pop	ecx
    pop	ebx
    ret	
getstring:
    inc si
getstringl:
    lodsb
    cmp	al,13
    jz	rn_done
    cmp	al,"'"
    jz	rm_done2
    cmp	al,'"'
    jz	rm_done2
    inc	cl
    shl	ebx,8
    mov	bl,al
    jmp	getstringl
;
; Read an address, composed of a number and a possible selector
;
ReadAddress	:	
    call	ReadNumber
    mov	ebx,eax
    ret
;
; Get an input line
;
GetInputLine	:	
    mov	byte [inquote],0
    mov	edi,InputBuffer	; Get input buffer
    mov	esi,edi			; Return buffer pointer
    mov	ecx,IBSIZE		; Size of buffer
moreinput:
    call	GetKey
    cmp	al,8			; Is delete or rubout?
    jz	short bkspc		; Yes - go do it
    cmp	al,7fh			;
    jz	short bkspc		; yes - go do it
    cmp	al,'"'
    jz	doquote
    cmp	al,"'"
    jnz	dochar
doquote:
    xor	byte [inquote],1
dochar:
    push	eax
    test	byte [inquote],1
    jnz	nolc
    cmp	al,'A'
    jc	nolc
    cmp	al,'Z'
    ja	nolc
    or	al,20h
nolc:
    stosb
    pop	eax
    cmp	al,13			; Is CR
    jz	short endinput		; Yes, return
    mov	dl,al			; Echo character
    call	PutChar
    loop	moreinput		; Loop till buffer full
endinput:
    ret
bkspc:
    cmp	edi,InputBuffer	; Quit if nothing in buffer
    jz	moreinput		; And get more input
    mov	dl,8			; Erase last echoed char
    call	PutChar
    mov	dl,' '			;
    call	PutChar
    mov	dl,8			; Reset pointer
    call	PutChar
    dec	di			; Point at last char
    jmp	moreinput		; Get more input
;
; Wade pasth spaces
;
WadeSpace	:	
    lodsb				; Get char
    cmp	al,' '			; if ' ' or ',' go again
    jz	short WadeSpace		;
    cmp	al,','			;
    jz	short WadeSpace		;
    dec	esi			; Point at last space char
    cmp	al,13			; check for end of line
    ret
;
; Main Input routine
;
InputHandler:
    sti
    Msg	10,13,"* "		; MONITOR prompt
    call	GetInputLine		; Get an input line
    call	WadeSpace		; Wade through spaces
    jnz     docmd
    mov     byte [si],13
    dec     si
    mov     al,[lastcmd]
    and     al,05fh
    or      al,20h
    cmp     al,'t'
    jz      docmd
    cmp     al,'p'
    jz      docmd
    cmp     al,'d'
    jz      docmd
    cmp     al,'u'
    jnz     InputHandler           ; blank line, so try again
docmd:
    mov     [lastcmd],al            ; 
    inc	esi			; Point at first non-space char
    mov	edi,Commands	; Get command list
    mov	ecx,comlen		; Length of list
    repne	scasb			;
    jnz	ierr			; Error if not in list
    mov	eax,comlen-1		; Calculate position
    sub	eax,ecx			;
    push	0			; command arg = 0
    call	TableDispatch		; Dispatch command
    dd	comlen-1
    dd	asm
    dd	breaks
    dd	compare
    dd	dump
    dd	entry
    dd	fill
    dd	go
    dd	domath
    dd	doin
    dd	move
    dd	doout
    dd	Proceed
    dd	_exit
    dd	ModifyRegisters
    dd	search
    dd	trap
    dd	diss
    dd	Help
    dd	slash
    jnc	InputHandler		; Get more input if no err
ierr:
    call	qerr			; Display error
    jmp	InputHandler		; Get more input
