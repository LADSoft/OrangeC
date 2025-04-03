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
%include  "dis.ase" 
%include  "fp.ase"
%include  "mmx.ase"
    global	DisplayRegisters
    global ModifyRegisters
    global ReadReg
    global ReadRegValue
    
    segment code USE32
;
; This is a list corresponding ASCII names for general purpose regs
; with the address the value can be found at;
;

peax	dd	dreax
    db	13,10,"eax:",0
pebx	dd	drebx
    db	"ebx:",0
pecx	dd	drecx
    db	"ecx:",0
pedx	dd	dredx
    db	"edx:",0
pesi	dd	dresi
    db	"esi:",0
pedi	dd	dredi
    db	"edi:",0
pebp	dd	drebp
    db	13,10,"ebp:",0
pesp	dd	dresp
    db	"esp:",0
peip	dd	dreip
    db	"eip:",0
    dd	0
;
; a list of 8-bit register names
;
pal	dd	dreax
    db	13,10,"al:",0
    dd	dreax+1
    db	"ah:",0
    dd	drebx
    db	"bl:",0
    dd	drebx+1
    db	"bh:",0
    dd	drecx
    db	"cl:",0
    dd	drecx+1
    db	"ch:",0
    dd	dredx
    db	13,10,"dl:",0
    dd	dredx+1
    db	"dh:",0
    dd	0
;
; a list of 16-bit register names
;
pax	dd	dreax
    db	13,10,"ax:",0
    dd	drebx
    db	"bx:",0
    dd	drecx
    db	"cx:",0
    dd	dredx
    db	"dx:",0
    dd	dresi
    db	"si:",0
    dd	dredi
    db	"di:",0
    dd	drebp
    db	13,10,"bp:",0
    dd	dresp
    db	"sp:",0
    dd	dreip
    db	"ip:",0
    dd	0
peflags	dd	reflags
    db	"eflags:",0
    dd	0
flm	EQU $
    db	11
    db	"NVOV"
    db	10
    db	"UPDN"
    db	9
    db	"DIEI"
    db	7
    db	"PLMI"
    db	6
    db	"NZZR"
    db	4
    db	"NAAC"
    db	2
    db	"POPE"
    db	0
    db	"NCCY"
    db	-1
    db	0
;
; Print a general purpose reg and it's value
;
PutDword	:	
    lodsd				; Get pointer to val
    push	dword [eax]
    mov	ebx,esi		; Get text pointer
    call	olMessage
    pop	eax		;
    call	PrintDword	; Print value
    call	PrintSpace	;
    ret
;
; Print a segment reg and its value
;
PutWord	:	
    lodsd			; Get pointer to value
    push	dword [eax]
    mov	ebx,esi		; Pointer to text
    call	olMessage
    pop	eax		;
    call	PrintWord	; Print value
    call	PrintSpace	;
    ret
;
; Print either the GP regs or the SEG regs
;

PrintAFew	:	
    call	edx		; Call the print routine
pf_lp:
    lodsb			; Wade past the text
    or	al,al		;
    jnz	pf_lp		;
    test	dword [esi],-1 ; See if trailer found
    jnz	PrintAFew	; Go print another
    ret
;
; try to find a match for a register spec
;
skimreg :
    push	edi
    push	esi
    xchg	esi,edi
    add	esi,4
srlp:
    cmp	byte [esi],32
    jnc	oktry
    inc	esi
    jmp	srlp
oktry:
    cmp	byte [esi],':'
    jz	match
    cmpsb
    jz	oktry
    pop	edi
    add	sp,4
srlp2:
    lodsb
    or	al,al
    jnz	srlp2
    test 	word [esi],0ffffh
    xchg	esi,edi
    jnz	skimreg
    sub	al,al
    inc	al
    ret
match:
    add	esp,4
    sub	eax,eax	
    xchg	esi,edi
    pop	edi
    mov	edi,[edi]
    stc
    ret
;
; search all the name tables for a match
;
FindRegister	:
    mov	edi, peax
    call	skimreg
    mov	cl,4
    jc	frnoseg
    mov	edi, pal
    call	skimreg
    mov	cl,1
    jc	frnoseg
    mov	edi, peflags
    call	skimreg
    mov	cl,4
    jc	frnoseg
    mov	bl,[esi+1]
    mov	edi, pax
    call	skimreg
    mov	cl,2
    jnc	frnotfound
    cmp	bl,'s'
    stc
    jnz	frnoseg
    clc
frnoseg:
    mov	al,0ffH
    inc	al
    ret
frnotfound:
    sub	eax,eax
    inc	eax
    stc
    ret
;
; read the value of a reg (used by input routines)
;
ReadReg		:
    push	ecx
    call	FindRegister
    jnz	notreg
    pushfd
    mov	eax,[edi]
    cmp	cl,4
    jz	rr_exit
    movzx	eax,ax
    cmp	cl,2
    jz	rr_exit
    movzx	eax,al
rr_exit:
    popfd
notreg:
    pop	ecx
    ret
;
; Read value for a register back into memory (R) command
;
ReadRegValue	:
    call	WadeSpace
    jz	doregprompt
    inc	esi
    cmp	al,':'
    jz	ReadRegValue
    cmp	al,'='
    jz	ReadRegValue
    dec	esi
    call	ReadNumber
    ret
doregprompt:
    push	ebx			; Else put up prompt
    push	ecx			;
    Msg	13,10,": "
    call	GetInputLine		; Get input line
    pop	ecx			;
    pop	ebx			;
    call	WadeSpace		; Ignore spaces
    jz	short rr_out		; Quit if so
    call	ReadNumber
    ret
rr_out:
    stc
    ret
;
; DisplayFlags command
;
EditFlags :
        sub     edx,edx
        inc     esi                      ; past the 'f'
        call    WadeSpace
        jnz     readflags
        call	crlf
        call    putflags
        call	Message
        db	"- ", 0
    call	GetInputLine		; Get input line
        mov     edx,1
readflags:
        call    WadeSpace
        jz      efx
        cmp     al,','
        jz      efskipcomma
        mov     ax,[esi]
        xchg    al,ah
        and     ax,0DFDFh               ; make upper case
        mov     bx,ax
        push    esi
        mov     esi,flm
efl:
        lodsb
        test    al,al
        jns     efc
        pop     esi
        add     esi,2    ; bump for no prompt mode
        test    edx,1
        jz      efnoadd
        add     esi,24   ; remainder of bump for prompt mode
efnoadd:
        stc
        ret
efc:
        movzx   ecx,al
        lodsd
        cmp     ax,bx
        jnz     nosetmatch
        bts     [reflags],ecx
        jmp     short effin
nosetmatch:
        shr     eax,16
        cmp     ax,bx
        jnz     efl
        btr     [reflags],ecx
effin:
        pop     esi
        inc     esi
efskipcomma:
        inc     esi
        jmp     readflags

efx:
        clc
        ret
        
;
; main 'Reg' command
;
ModifyRegisters	:	
    mov		al,[esi]
    cmp		al,'n'
    jz		fpcommand
    cmp		al,'m'
    jz		ModifyMMXRegisters
        cmp     al,'f'
        jnz     nEditFlags
        cmp     byte [si+1],'s'
        jnz     EditFlags
nEditFlags:		
    call	WadeSpace		; Wade through spaces
    jz	DisplayRegisters	; Display regs
    call	FindRegister
    jnz	badreg
    push	edi
    call	ReadRegValue
    pop	edi
    jc	badreg2
    cmp	cl,4
    jnz	wordreg
    mov	[edi],eax
    clc
    ret
wordreg:
    cmp	cl,2
    jnz	bytereg
    mov	[edi],ax
badreg2:
    clc
    ret
badreg:
    stc
    ret
bytereg:
    mov	[edi],al
    clc
    ret
putflags :
    mov	esi, flm
putflags2:
    lodsb
    or	al,al
    js	pfdone
    movzx	ax,al
    bt	word [reflags],ax
    lodsd
    mov	edx,eax
    jc	isclr
    shr	edx,16
isclr:
    xchg	dh,dl
    call	PutChar
    xchg	dl,dh
    call	PutChar
    call	PrintSpace
    jmp	putflags2
pfdone:
    ret
;
; Display the processor regs
;
DisplayRegisters	:	
    mov	esi,  peax		; Print GP regs
    mov	edx,PutDword	; with the DWORD function
    call	PrintAFew		; Print them
    mov	esi, peflags	; Put the flags
    call    PutDword		;
    call	putflags
    mov	ebx,dword [dreip]		; Dissassemble at current code pointer
    call	DisOneLine		;
    clc
    ret
