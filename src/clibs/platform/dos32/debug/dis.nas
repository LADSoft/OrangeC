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

%include "prints.ase"
%include "operands.asi"
%include "operands.ase" 
%include "opcodes.asi"
%include "opcodes.ase"
%include "mtrap.ase"
%include "input.ase"

    global	diss,DisOneLine

DEFAULTBYTES EQU 32

    segment data USE32
start	dd	0
dend	 dd	0
extrabytes dd	0

    segment code USE32
%define	isNewLine ebp - 4		; Local variables
%define	oldposition ebp - 8
%define	put	ebp -12
%define	bytestomove ebp - 16
;
; Get a dissassembled line of code
;
GetCodeLine:
    ENTER	16,0
    mov	dword [isNewLine],TRUE ; Assume it has an opcode
    mov	byte [edi],0	; Clear output buffer
    mov	[oldposition],esi	; Current position
    test	dword [extrabytes],-1		; See if still printing bytes
    jz	short notextra		; from last instruction
    add	esi,[extrabytes]	; New position to edi
    xchg	esi,edi			;
    mov	byte [esi],0	; Clear buffer
    mov	al,10			; Tab to pos 14
    call	TabTo			;
    xchg	esi,edi			; edi = buffer
    push	edi			;
    mov	ecx,4			; next four DWORDS = 0;
    sub	eax,eax			;
    rep	stosd			; Store the words
    pop	edi			;
    mov	dword [isNewLine],FALSE; Doesn't have an opcode
    jmp	btm
notextra:
    mov	eax,[code_address]	; Get code address
    cmp	eax,[dend]		; See if done
    jnc	near endcodeline		; Quit if nothing left
    xchg	esi,edi			; esi = buffer
    push	esi			;
    mov	eax,[code_address]	; Get code address
    call	PutDword		; Print it out
    mov	byte [esi],':'	; Put a space
    inc	esi			;
    mov	byte [esi],' '	; Put a space
    inc	esi			;
    mov	byte [esi],0	; Put an end-of-buffer
    pop	eax
    xchg	esi,eax
    push	eax
    mov	al,29                   ; Tab to pos 29
    call	TabTo			;
    xchg	esi,edi			; edi = buffer
    call	ReadOverrides		; Read any overrides
    call	FindOpcode		; Find the opcode table
    xchg	esi,edi			; esi = buffer
    jnc	short gotopcode		; Got opcode, go format the text
    push	esi			; Else just put a DB
    mov	ax,"db"			;
    call	put2			;
    pop	esi			;
    mov	al,TAB_ARGPOS		; Tab to the arguments
    call	TabTo			;
    mov	al,[edi]		; Put the byte out
    inc	edi			; Point to next byte
    call	PutByte			;
    mov	byte [esi],0	; End the buffer
    xchg	esi,edi			;
    pop	edi			;
    jmp	short btm		; Go do the byte dump
gotopcode:
    push	esi			; Got opcode, parse operands
    mov	esi,edi			;
    mov	eax,1
    call	DispatchOperands	;
    mov	edi,esi			;
    pop	esi			;
    push	edi			;
    call	FormatDisassembly	; Use the operand parse to format output
    pop	edi			;
    xchg	esi,edi			;
    pop	edi			;
btm:
    mov	byte [edi],0	; End the buffer
    mov	eax,esi			; Calculate number of bytes to dump
    sub	eax,[oldposition]	;
    mov	[bytestomove],eax	;
    mov	dword [extrabytes],0		; Bytes for next round = 0
    cmp	dword [bytestomove],5; See if > 5
    jbe	short notmultiline	; No, not multiline
    mov	eax,[bytestomove]	; Else calculate bytes left
    sub	al,5			;
    mov	[extrabytes],eax	;
    mov	dword [bytestomove],5; Dumping 5 bytes
notmultiline:
    xchg	esi,edi			; esi = buffer
    push	edi			; Save code pointer
     mov	edi,[oldposition]	; Get original code position
    mov	ecx,[bytestomove]	; Get bytes to move
putlp:
    mov	al,[edi]		; Get a byte
    call	PutByte			; Expand to ASCII
    mov	byte [esi],' '	; Put in a space
    inc	esi			; Next buffer pos
    inc	edi			; Next code pos
    LOOP	putlp			; Loop till done
    xchg	esi,edi			; Restore regs
    mov	eax,[bytestomove]	; Codeaddress+=bytes dumped
    add	[code_address],eax	;
endcodeline:
    mov	eax,[isNewLine]		; Return new line flag
    LEAVE				;
    ret
;
; Main disassembler
;
diss:
    ENTER	256,0			; Buffer = 256 bytes long
    call	crlf
    call	WadeSpace		; See if any parms
    jz	short atindex		; No disassemble at index
    call	ReadAddress		; Else read address
    jc	badargs			; Get out bad args
    mov	eax,DEFAULTBYTES	; Number of bytes to disassemble
    add	eax,ebx			; Find end of disassembly
    jnc	okadd
          mov	eax,-1
okadd:
    mov	[dend],eax		; Save it as default
    call	WadeSpace		; See if any more args
    jz	short gotargs		; No, got args
    call	ReadNumber		; Read the end address
    jc	short badargs           ; Out if bad args
    mov	[dend],eax		; Save end
    jmp	short gotargs		; We have args
badargs:
    stc				; Error
    LEAVE
    ret
atindex:
    mov	ebx,[start]		; Get the next address to disassemble
    mov	eax,DEFAULTBYTES	; Default bytes to disassemble
    add	eax,ebx			;
    mov	[dend],eax		; Set up end
gotargs:
    mov	[code_address],ebx	; Save code address for printout
    mov	esi,ebx			;
gcloop:	
    call	ScanKey
    jnz	dusetadr
    lea	edi,[ebp - 256]		; Get the buffer
    call	GetCodeLine		; Get a line of text
    lea	ebx,[ebp - 256]		; Print out the text
    call	dgroupMessage
    call	crlf
    cmp	esi,[dend]		;
    jc	gcloop			; Loop if not
    test	dword [extrabytes],-1		; Loop if not done with dump
    jnz	gcloop			;
dusetadr:
    mov	esi,[code_address]	;
    mov	[start],esi		;
    clc
    LEAVE
    ret
;
; Disassemble one line.  Used by the Reg display command
;
DisOneLine:
    ENTER	256,0			; Space for buffer
    push	ebx
    call	crlf
    pop	ebx
    mov	eax,1
    add	eax,ebx			; One byte to disassemble
    mov	[dend],eax		; ( will disassemble entire instruction)
    mov	[code_address],ebx	;
    mov	esi,ebx
    mov	[start],esi		; Save new index
dol_loop:
    lea	edi,[ebp - 256]		; Get buffer
    call	GetCodeLine		; Get a line of code
    lea	ebx,[ebp -256]		; Display the line
    call	dgroupMessage
    call	crlf
    test	dword [extrabytes],-1		; See if more to dump
    jnz	dol_loop		; Loop if so
    clc				; No errors
    leave
    ret

