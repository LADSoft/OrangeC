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

%include "asm.asi"
%include "asm.ase"
%include "prints.ase" 
%include "input.ase" 
%include "mtrap.ase"
%include "breaks.ase"
%include "regs.ase"
%include "swift.ase"

TRAPFLAG EQU 100h			; 80386 trap enable flag

    global go,trap, Proceed, ReTrap, TrapCount, callcheck, cmpstring, WadePrefix, proceedone
    global proceedCount
    extern _winxcept
    segment data USE32
TrapCount	dd	0
proceedCount dd 0
pff	dw	0		; flag if 67h prefix found

pretab	db	0f0h,0f2h,0f3h,026h,02eh,036h,03eh,064h,065h,066h,067h
stringtab db	0a6h,0a7h,6ch,6dh
    db	0ach,0adh,0a4h,0a5h
    db	06eh,06fh,0aeh,0afh
    db	0aah,0abh

    segment code USE32
;
callcheck	:
    mov	ah,[ebx]	; Load the first byte of the instruction
    cmp	ah,0ceh		; into?
    mov	al,1
    jz	near chx
    cmp	ah,0cdh		; Interrupt ?
    mov	al,2		; two bytes
    jz	near chx		; I do this because some int calls are
                ; misbehaved and won't restore the trap flag
    cmp	ah,0e8h		; Near Call?
    mov	al,5		; Yes, this is 3 bytes
    jz	near chx		; And execute it
    cmp	ah,09ah		; Far call
    mov	al,7		; This one is 5 bytes
    jz	near chx		; Not either of these, just trap
    mov	ax,[ebx]
    and	ax,038FFH
    cmp	ax,010ffh
    jz	gotind
    cmp	ax,018ffh
    jz	gotind
    ret
gotind:
    mov	al,2  		; two bytes base
    mov	ah,[ebx+1]
           and	ah,0c7h
    cmp	ah,0c0h		; mod 3, through reg is 2 bytes
    jnc	chx
    test	word [pff],0ffh	; check 16/32 address mode flag
    jz	call32
    cmp	ah,8		; 16 bit, MOD=0
    jnc	chk16m1
    cmp	ah,6		; yes, direct offset?
    jnz	chx
    add	al,2		; yes 4 bytes total
    jmp	chx
chk16m1:
    and	ah,0c0h		; check for mod 1 or 2
    js	is16m2
    inc	al		; mod 1, add a byte for offset
    jmp	chx
is16m2:
    add	al,2		; else mod 2, two byte offset
    jmp	chx
call32:
    cmp	ah,6		; 32 bit mode, check for 32-bit offset
    jnz	ch32m0
    add	al,4		; yep, add 4
    jmp	ch32sib
ch32m0:
    cmp	ah,8   		; MOD = 0?
    jc	ch32sib		; yep, check for sib
ch32m1:
    or	ah,ah		; MOD = 1?
    js	ch32m2
    add	al,1   		; yep, 1 byte offset
    jmp	ch32sib
ch32m2:
    add	al,4   		; else mod 2
ch32sib:
    and	ah,7
    cmp	ah,4
    jnz	chx
    inc	al		; add in the SIB byte
    mov	ah,byte [ebx+1]	; test for MOD 3
    and	ah,0c0h
    cmp	ah,0c0h
    jz	chx		; yep, no further bytes
    mov	ah,byte [ebx+2] ; get SIB byte
    and	ah,7		; check for EBP special cases
    cmp	ah,5
    jnz	chx
    inc	al
    test	byte [ebx+1],40h ; check if one or 4 bytes disp
    jz	chx
    add	al,3		; MOD 1,3, need 4 bytes
chx:
    movzx	eax,al
    sub	ah,ah		; clear Z flag...
    ret
; used by P command to wade through prefixes to find out if
; we have a string instruction
;
WadePrefix:
    mov	word [pff],0
    sub	edx,edx
wp_lp:
    mov	al,[ebx]
    mov	edi,pretab
    mov	ecx,11
    repnz	scasb
    jnz	wp_done
    inc	ebx
    inc	edx
    cmp	al,67h
    jnz	wp_lp
    bts	word [pff],0		; flag the addrsiz in case of call
    jmp	wp_lp
wp_done:
    ret
    
cmpstring:
    mov	al,[ebx]
    mov	edi,stringtab
    mov	ecx,14
    repnz	scasb
    ret
;
; Execute program
;
go	:	
    Call	WadeSpace	; Wade till address
    jz	dogo
    cmp	al,'='
    jnz	checkbreak
    inc	esi
    call	ReadAddress
    jc	near goerr
    mov	dword [dreip],ebx	; Fix CS:EIP for new routine
    call	WadeSpace	; Wade
    jz	short dogo	;
checkbreak:
    call	ReadAddress	; Read break address
    jc	near goerr		; Quit if errir
dogo2:
    sub	ax,ax		; Break 0
    call	SetBreak	; Set the break
dogo:
    call	EnableBreaks	; Enable breaks
    xor	ax,ax		; Not trapping
    jmp	gotrap	; Run the code
;
;
; Limited proceed function
;
Proceed	:	
    call	WadeSpace
    jz	pok
    cmp	al,'r'
    jnz	pcount
    inc	esi
    call	WadeSpace
    jnz	near goerr
    mov	byte [traceon],1
    jmp	gotrap
pcount:
    call    ReadNumber
    jc      near goerr
    dec     eax
    mov     [proceedCount],eax
    call    WadeSpace
    jnz     near goerr
proceedone:
pok:
    mov	ebx,dword [dreip]	;
    call	WadePrefix	; wade through all prefixes
    call	callcheck  	; noew check for near  & far calls
    jz	short pgo	; and execute it
    cmp	ah,0e0h		; Check for loop instructions
    jz	short	pgo
    cmp	ah,0e1h
    jz	short	pgo
    cmp	ah,0e2h
    jz	short	pgo
    call    cmpstring	; see if is string instruction or loop
    jnz	short dotrap	; Not either of these, just trap
    mov	eax,edx
    inc	eax
pgo:
    movzx	eax,al
    mov	ebx,dword [dreip]	;
    add	ebx,eax		; Ebx = breakpoint
    sub	eax,eax		; Use the scratch breakpoint
    call	SetBreak	; Set a break
    test	byte [slashcmd],1		; if slash
    jnz		normalproceed
    test	dword [proceedCount],-1	; or proceed
    jnz		normalproceed		; just enable the temp break
    call	EnableBreaks	; Else Enable all breakpoints
    sub		eax,eax
    jmp 	gotrap
normalproceed:
    sub		ecx,ecx
    call	enableOneBreak
    sub	eax,eax		; No trapping
    jmp	gotrap	; Run the code
;
; Trap command
;
trap	:	
    call	WadeSpace
    jz	dotrap
    call	ReadNumber
    jc	near goerr
    push	eax
    call	WadeSpace
    pop	eax
    jnz	near goerr
           or	eax,eax
    jz	dotrap
    dec	eax
    mov	[TrapCount],eax
dotrap:
    mov	ebx,dword [dreip]	;
    mov	al,2
    cmp	byte [ebx],0cdh	; got to proceed through ints
    jz	pgo			; in case they wreck the trap flag
ReTrap:
    mov	ax,TRAPFLAG	; Are trapping on instruction
gotrap:
    movzx	eax,ax
    mov	esp,[dresp]
    or	eax,dword [reflags]	; Fill stack frame with FLAGS , CS:EIP
    push	eax
    sub	eax,eax
    mov	eax,cs
    push	eax
    push	dword [dreip]
    movzx	eax,word [drds]
    push	eax
    mov	es,[dres]
    mov	eax,[dreax]	; Load regs
    mov	ebx,[drebx]	;
    mov	ecx,[drecx]	;
    mov	edx,[dredx]	;
    mov	esi,[dresi]	;
    mov	edi,[dredi]	;
    mov	ebp,[drebp]	;
    test	byte [traceon],1	; if PR go handle first instruction
    pop	ds
    jz	doiret
    mov	word [_winxcept],0
    call	swiftrace
    call	DisplayRegisters; if it was an int 3 come back
    clc
    ret
doiret:
    iretd
goerr:
    stc
    ret
