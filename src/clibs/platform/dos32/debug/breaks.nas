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

%include  "prints.ase" 
%include  "mtrap.ase" 
%include  "input.ase"

    global	SetBreak, breaks, EnableBreaks, DisableBreaks, enableOneBreak
    extern hdwebreakcommand
    extern hdwebreakenable
    extern hdwebreakdisable

    segment data USE32
breaklist times	16 [dd 0]		; List of breakpoints
breakhold times 16 [db 0]		; List of values where 'int 3' has covered
                ; the opcode
breakenum dw	0		; Flags telling which breakpoints are set
    
    segment code USE32
;
; Command to set a breakpoint comes here
;
SetBreak	:	
    and	eax,0fh		; Set the breakpoint set bit
    bts	word [breakenum],ax	;
    mov	[eax*4+breaklist],ebx	;
    ret
;
; Command to clear a breakpoint comes here
;
clearbreak	:	
    and	ax,0fh		; Reset the flag bit
    btr	word [breakenum],ax	;
    ret
;
; Command to display a breakpoint comes here
;
displaybreak	:	
    and	eax,0fh		; See if set
    bt	word [breakenum],ax	;
    jnc	short nodisplay	; Quit with no disp if no breakpoint set
    push	eax		; CR/LF
    call	crlf
    pop	eax		;
    push	eax		;
    call	PrintByte	; Print breakpoint #
    mov	dl,':'		; Print ':'
    call	PutChar
    call	PrintSpace
    pop	ebx		;
    mov	eax,dword [ebx*4+breaklist]	; Print segment
    call	PrintDword	;
nodisplay:
    ret
enableOneBreak :
    mov	esi,dword [ecx*4 + breaklist]
    mov	al,[esi]	; Get the byte at that location
    mov	byte [esi],0cch	; Put an int 3
    mov	[ecx + breakhold],al	; Save it for restore
    ret
;
; When GO or TRAP or PROCEED commands execute, they call this to
; enable breakpoints
;
EnableBreaks	:	
    mov	ecx,15		; For each breakpoint
eblp:
    bt	word [breakenum],cx	; If not set
    jnc	short ebnn	; Don't do anything
    call	enableOneBreak;
ebnn:
    dec	ecx		; Next breakpoint
    jns	eblp		;
    mov	eax,ecx		;
    call	hdwebreakenable
    ret	
;
; Int 3 or int 1 call this to disable breakpoints and restore the
; values covered by the int 3
;
DisableBreaks	:	
    mov	ecx,15		; For each breakpoint
dblp:
    bt	word [breakenum],cx	; If not set
    jnc	short dbnn	; Go nothing
    mov	al,[ecx + breakhold]
    
    mov	esi,[ecx*4 + breaklist]	;
    mov	[esi],al	;
dbnn:
    dec	ecx
    jns	dblp		; Next breakpoint
    btr	word [breakenum],0   ; Reset breakpoint 0 (the automatic breakpoint)
    call	hdwebreakdisable
    ret
;
; Handle breakpoint-related commands
;
breaks	:	
    call	WadeSpace	; Wade through spaces
    jz	showall	; Show all breakpoints
    cmp	al,'d'
    jz	hdwebreakcommand
    cmp	al,'-'		; Else check for '-'
    pushfd			;
    jnz	noinc		;
    inc	esi		; Skip to next arg
    call	WadeSpace	;
noinc:
    cmp	al,'*'
    jz	clearall
    call    ReadNumber	; Read break number
    jc	near badbreak2	; Exit if error
    cmp	eax,16		; Make sure in range
    jnc	near badbreak2	; Exit if error
    or	eax,eax		; Can't do anything with break #0, it's automatic
    jz	near badbreak2	;
    popfd			;
    push	eax		;
    jz	short unmake	; If was '-', clear break
    call	WadeSpace	; Else wade to next arg
    jz	badbreak2
    call	ReadAddress	; Read the bp address
    pop	eax		;
    jc	short badbreak	; Quit if error
    call	SetBreak	; Set breakpoint at this address
    jmp	short breakdone	; Get out
unmake:
    call	WadeSpace	; Wade to end
    pop	eax		;
    jnz	short badbreak	; If there is more we have an error
    call	clearbreak	; Clear breakpoint
    jmp	short breakdone	; Get out
clearall:
    popf
    jnz	short badbreak
    inc	esi
    call	WadeSpace
    jnz	short	badbreak
    mov	word [breakenum],0
    jmp	breakdone
showall:
    test	word [breakenum],0ffffh
    jnz	doshow
    Msg	13,10,"No breakpoints enabled"
    clc
    ret

doshow:
    mov	ecx,15		; For each breakpoint
salp:
    mov	eax,ecx		; Display it if set
    call	displaybreak	;
    loop	salp		;
breakdone:
    clc			; Exit, no errors
    ret
badbreak2:
    pop	eax		;
badbreak:
    stc			; Exit, errors
    ret
