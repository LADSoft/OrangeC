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

%include "asm.asi"
%include "opcodes.asi"
%include "aoperand.ase"
%include "opcodes.ase"
%include "opcom.ase"
%include "prints.ase"
%include "input.ase"
%include "mtrap.ase"
%include "dispatch.ase"
%include "operands.ase"
%include "exec.ase"
%include "dump.ase"
%include "entry.ase"
    global	asm
    global	arg1
    global	arg2
    global	arg3
    global	RepPfxBitmap
    global	lastofs
    global	lastbyte
    global	PrefixBitmapWord
        global  EnteredMnemonic
        global  farptr
        global  slash, slashcont, slashcmd

    segment	data	use32
mtoofew		db	"Not enough operands",0
slashcmd        db      0
lastbyte	db	0		;last char of valid mnemonic
lastofs		dd	0		;current disassembly offset
slastofs        dd      0
slasteip        dd      0
asmcount        dd      0
say_repne       db      "repnz",0       ;prefix strings to look for
say_repe        db      "repz",0
say_rep		db	"rep",0
say_lock        db      "lock",0
say_word	db	"word",0	;opcode size overrides
say_byte	db	"byte",0
say_ptr		db	"ptr"
say_far         db      "far"
arg1		resb	ASMOPSIZE		;three args
arg2		resb	ASMOPSIZE		;three args
arg3		resb	ASMOPSIZE		;three args
arg4		resb	ASMOPSIZE		;three args
AsmbldInstrsBuf       times 40 [DB 0]      ;temporary to hold assembled instructions
OpSizeTable		times 16 [DB 0]	;sizes returned by aop routines
OpSizeTblIndex	dd	0		;pointer into OpSizeTable
RepPfxBitmap	db	0		;bitmap of which rep prefix found
EnteredMnemonic		times 16 [DB 0]	;bucket to hold mnemonic as typed
farptr          db      0
;DefaultSeg appears to hold an index into SegmentPfxBytes.

DefaultSeg	db	0		;current default seg (DS or SS)
SegmentPfxBytes	db	26h,2eh,36h,3eh,64h,65h	;table of seg prefixes
    SEGPFXLISTSIZE	equ	$-SegmentPfxBytes
OverridePfxList db      "osasgsfsdssscses"      ;list of prefixes with colon
PrefixBitmapWord	dw	0	;bitmap of which prefix found
    segment code USE32
slashbuf              times 32 [db 0]
slash:
        mov     eax,[dreip]
        mov     [slasteip],eax
        mov     eax,[lastofs]
        mov     [slastofs],eax
        mov     dword [lastofs],slashbuf
        mov     dword [dreip],slashbuf
        mov     dword [asmcount],0
        call    doasm2                   ; do assembly
        inc     byte [slashcmd]
        call    proceedone
        ; never gets here
slashcont:
        dec     byte [slashcmd]
        mov     eax,[slasteip]
        mov     [dreip],eax
        mov     eax,[slastofs]
        mov     [lastofs],eax
        ret
;
; interpreter stub to get params
; INPUT: DS:SI points at the user input line, just past the command char.
; I am assuming that someone issued the A command.  This can be by itself, 
; or it can be followed by some addresses
; The syntax ins A [[segment:]offset]
;	where 
;	[segment:] can be any segment register, or it can be any hex
;		number of up to 4 digits.
;	[offset] can be any hex number of up to 8 digits
; OUTPUT: lastseg and lastofs set up to assemble at if user provided a legal 
;	address. Segment is also in FS
;	CY if invalid address
;	code address saved in case an unqualified 'A' command later
;
;
asm:
    call	WadeSpace		; see if address given
    jnz	readaddr		; yep, get it (not CR past spaces)
    mov	eax,[lastofs]
    or	eax,eax
    jz	usehere			;if not, go from where we are
    mov	ebx,[lastofs]
    jmp	gotaddr			;no need to set up our own
usehere:
    mov	ebx,[dreip]		;else load our CS:IP
    jmp	gotaddr			;and use that

readaddr:
    call	ReadAddress		; read address from input line
    jc	aserrm			; out on err
    call	WadeSpace		; make sure nothing else
    jnz	aserrm			; NZ means we didn't hit a CR
gotaddr:
    mov	esi,ebx			; load address
    mov	[lastofs],esi		;and current 16-bit offset
        mov     dword [asmcount],-1
    call	doasm			; do assembly
    clc				; exit
    ret
aserrm:
    stc				; exit with err
    ret
;
; prompt for a line, parse and assemble
;
; main assembler
;
; OK, the user pressed A <CR>. Here we solicit each entry, one line
; at a time, and convert the valid ones to opcodes. Invalid entries are
; complained about, and we return to the top of this routine to get
; another attempt.
; INPUT: address to assemble at is in FS (and lastseg): lastofs
; OUTPUT: Assembled code placed at target memory location and code ptr
;	  incremented
; PROCESSING: 
;	1) Display the segment:for the next line 
;	2) Get an instruction from the user
;		If CR only, we are done
;	3) If error, report and goto step 1, else assemble the instruction
;		into a temporary assembly buffer
;	4) Add any prefixes or overrides to the buffer as required
;	5) Copy the buffer to the target memory location
;	6) Return to step 1 until done.
; NOT
;	1) GetInputLine places the user input into InputBuffer, an 80-char
;		buffer. It handles backspacing. It returns with SI pointing
;		to the beginning of the edited input, which may not make
;		sense
;	2) getCode points DI at a 16-char bucket I call EnteredMnemonic,
;		and returns the first delineated string
;
doasm:
        test    dword [asmcount],-1
        jz      near doasx
        dec     dword [asmcount]
    call	crlf			;move to new line
    mov	eax,[lastofs]		;get last offset
    call	PrintDword		;print it
    call	PrintSpace		;and a couple of spaces
    call	PrintSpace
    call	GetInputLine           	; get input line
doasm2:
    call	WadeSpace		; if nothing there we are done
    jz	near doasx			;since we hit the CR
        cmp     byte [esi+2],' '
        jnz     normproc
        cmp     word [esi],'dd'
        jz      near dodd
        cmp     word [esi],'wd'
        jz      near dodw
        cmp     word [esi],'bd'
        jz      near dodb
normproc:
    sub	al,al			;clear out AL
    call	setsize			;put 0 into these 3 fields
    mov	word [PrefixBitmapWord],0	;say no prefix found
    mov	byte [DefaultSeg],3
    call	getcode			; get the opcode
    jc	badop
    mov	edi,arg1		; get first arg
    call	parsearg
    jc	badarg
    mov	edi,arg2		; get second arg
    call	parsearg
    jc	badarg
    mov	edi,arg3		; get third arg
    call	parsearg
    jc	badarg
    jz	assemble		;if no more, to assemble it
manyerr:
    Call	printAlignedErrorMsg	;else bitch about too many operands
    db	"Too many operands",0
    jmp	doasm
badarg:
    call	printAlignedErrorMsg	;complain about invalid operand
    db	"Unknown operand",0
    jmp	doasm
badop:		
    call	printAlignedErrorMsg	;complain about invalid opcode
    db	"Unknown opcode",0
    jmp	doasm
assemble:
    call	validops		;size check and set
    jc	doasm			;size mismatch, ignore
    mov	esi,mnemonicMatchAddrTable	;find table we built
        mov   ecx,[mnemonicMatchCount] ;number of valid table entries
    mov	dword [OpSizeTblIndex],0	;init index to top of table

;mnemonicMatchAddrTable contains a list of up to 10h addresses. Each address
;points to a structure. Each structure contains (among other things) a
;pointer to a string for this mnemonic, the base opcode for the
;mnemonic, and an index to the routine used to assemble the code
;  This loop is examining every valid element of the table we build to
;isolate those instances of this mnemonic that are valid in this case (that
;is, valid for size, addressing mode, etc).

assl:
    push	ecx			;save count
    push	esi			;save table address
    mov	esi,[esi]			;find structure address
    mov	edi,AsmbldInstrsBuf	;where to put binary 
    call	oneasm			;dispatch for this operand
                    ; copies opcode for this instruction
                    ; into the assembled code buffer
    mov	cl,0			;assume we didn't do anything
    jc	assx2			;and if true, cl is right
    mov	ecx,edi			;else get we ended up at
    sub	ecx,AsmbldInstrsBuf	;minus starting offset
assx2:		   	
    mov	ebx,[OpSizeTblIndex]	;pointer into opsize table
    mov	[ebx+OpSizeTable],cl	;save how many we did
    inc	dword [OpSizeTblIndex]		;bump to next location
    pop	esi			;restore mnemonic ptr table address
    pop	ecx			;and how many to examine
    add	esi,4			;point to next possibility
    loop	assl			;do them all
    movzx	ecx,byte [OpSizeTblIndex]	;see if we did anything
    or	ecx,ecx
    jz		nomatch
    sub	ebx,ebx			;else init for next loop


;at this point we have at least one match between the disassembly
;strucutre and the mneominc/addressmode / size data we accrued earlier.
;We are going to search the matched entries for the one with the
;smallest possible byte sequence

szllp:
    or	bh,bh			;if bh is zero
    jz	szlg			;skip this stuff
    cmp	bh,[ecx+OpSizeTable-1]	;see if high byte of table value
                    ;is greater than bh
    jb	sslc			;go here if it is
    test	byte [ecx+OpSizeTable-1],0FFh	;see if table value is 0
    jz	sslc			;and if it is, same place

;OK, BH is keeping track of the length of the sequence, and BL is keeping
;track of the table to that sequence.  We get here in case a) this
;is the only valid sequence we found, or b) this sequence is shorter than
;the prior shortest sequence we found.

szlg:
    mov	bh,[ecx+OpSizeTable-1]	;get high byte of tbl element->bh
    mov	bl,cl			;track the index to the shortest
sslc:
    loop	szllp			;do for as many valid match instances
                    ;as we found
ssgot:
    or	bh,bh			;did we find anything?
    jz	nomatch			;no, we did not
    sub	bh,bh			;convert BL into a word index
    shl	ebx,2			;into the match table
    mov	esi,[ebx+mnemonicMatchAddrTable-4]	;get an address
    mov	edi,AsmbldInstrsBuf	;point to buffer
    call	oneasm				;build our favorite sequence
    mov	ecx,edi				;DI is new buffer offset
    mov	esi,AsmbldInstrsBuf	;switch top to SI
    sub	ecx,esi			;get buffer bytecount
    mov	edi,[lastofs]		;point to last assemble offset
    push	ecx			;save count

;Now DI points to the actual location in memory where we want to put our
;assembled buffer.  Since prefixes are not in the assembly buffer, we first
;stick in as many prefixes as we found, then paste the remainder of the
;buffer beyond them.

    call	InsertPrefixes
    pop	ecx			;restore count
    jc	pfxerr			;if carry, too many prefixes
    rep	movsb			;copy to memory location for asm
    mov	[lastofs],edi		;update assemble in mem location
doasmn:
    jmp	doasm			;and get next instruction
pfxerr:
    call	printAlignedErrorMsg
    db	"Too many prefixes",0
    jmp	doasm

nomatch:
    call	printAlignedErrorMsg
    db	"Invalid opcode/operand combo",0
    jmp	doasm
doasx:
    ret
dodd   :
        mov     al,[memsize]
        push    eax
        mov     byte [memsize],2
        jmp     dbcont

dodw   :
        mov     al,[memsize]
        push    eax
        mov     byte [memsize],1
        jmp     dbcont

dodb   :
        mov     al,[memsize]
        push    eax
        mov     byte [memsize],0
dbcont :
        mov     al,2
        mov     cl,[memsize]
        shl     al,cl
        mov     [diglen],al
        add     esi,4
        mov     edi,AsmbldInstrsBuf
        call    entrytobuf    
        jnc     dbnoerr
        call    printAlignedErrorMsg
        db      "Invalid data",0
        jmp     dberrx
dbnoerr:
        mov     ecx,edi
        mov     esi,AsmbldInstrsBuf
        sub     ecx,esi
        mov     edi,[lastofs]
        cld
        rep     movsb
        mov     [lastofs],edi
dberrx:      
        pop     eax
        mov     [memsize],al
        jmp     doasm

;
;this routine is the shell which assembles an instruction based
;on the opcode/operand/size data
;
;INPUT: SI points to the opcode structure
;       DI points to a temp buffer into which we do the assembly
;OUTPUT: BUFFER FILLED.  This routine does the 0F prefix but none of the
;        other prefixes

oneasm:
    test	byte [esi+opcode.flags],prefix0F	;0F prefix on this guy?
    jz	no386p				;nope
    mov	al,0fh				;else stash the 0F
    stosb					;into the buffer
no386p:

;The syntax here works, but it can be clarified a little. opcode.operands
;was never explicitly written to.  Instead, we build a table of pointers
;into a table of opcode structures. SI contains one of the pointers out
;of that table. Maybe it's just because I'm more used to it, but I prefer
;the MASM syntax to indicate this:
;	mov	al,(opcode ptr [esi]).operands

    mov	al,[esi+opcode.operands]		;get addressing mode
    push	0				;TableDispatch calls this subkey
    call	TableDispatch			;and dispatch it
        dd      70                              ;length of table

    dd	aop0,  aop1,  aop2,  aop3,  aop4,  aop5,  aop6,  aop7
    dd	aop8,  aop9,  aop10, aop11, aop12, aop13, aop14, aop15
    dd	aop16, aop17, aop18, aop19, aop20, aop21, aop22, aop23
    dd	aop24, aop25, aop26, aop27, aop28, aop29, aop30, aop31
    dd	aop32, aop33, aop34, aop35, aop36, aop37, aop38, aop39
    dd	aop40, aop41, aop42, aop43, aop44, aop45, aop46, aop47
        dd      aop48, aop49, aop50, aop51, aop52, aop53, aop54, aop55
        dd      aop56, aop57, aop58, aop59, aop60, aop61, aop62, aop63
        dd      aop64, aop65, aop66, aop67, aop68, aop69, aop70
    ret

;
; inserts prefixes into buffer
;
;INPUT: DI points to buffer
;OUTPUT: all prefixes inserted and DI updated
;
; now we do the remainder of the 8086 repeate and lock prefixes
;
;
;
; now comes the 386 prefixes

InsertPrefixes:
    sub	edx,edx
    test	byte [RepPfxBitmap],AF_LOCK	;see if lock set
    jz	nlock			;nope, no lock
    mov	al,0f0h			;else stash lock prefix
    stosb
nlock:
        test    byte [RepPfxBitmap],AF_REPNE   ;see if REPNE set
    jz	nrepne			; nope, no REPNE
    mov	al,0f2h			;stick in repne prefix
    stosb
nrepne:
        test    byte [RepPfxBitmap],AF_REPE | AF_REP    ; See if REPE
    jz	nrepe			; noe, no repe
    mov	al,0f3h			;stick in repe prefix
    stosb
nrepe:
;        test    [optass32],255
;        jz      ip_noinv
        xor     word [PrefixBitmapWord],AS_OPSIZETEST + AS_ADDRSIZETEST
ip_noinv:
        test    word [PrefixBitmapWord],AS_ADDRSIZESET  ;see if 67 override
        jz     naddrsizna
        test    word [PrefixBitmapWord],AS_ADDRSIZETEST
        jz      naddrsizna
    mov	al,67h
    stosb

naddrsizna:
        test    word [PrefixBitmapWord],AS_OPSIZESET    ;see if 66 override
        jz      nopsizna
        test    word [PrefixBitmapWord],AS_OPSIZETEST
        jz      nopsizna
    mov	al,66h
    stosb
nopsizna:

;
; now we do segment overrid prefixes by scanning the prefix bitmap
; word
    sub	edx,edx			;start with no override byte count
    mov	dh,byte [PrefixBitmapWord]	;get prefix bitmap lo byte
    mov	ebx,SegmentPfxBytes	;list of prefix bytes
    mov	ecx,SEGPFXLISTSIZE		;there are 6 of these

sl2:
    shr	dh,1			;see if this one required
    jnc	nsl2			; no, skip
    mov	al,[ebx]			; else load the prefix from the table
    stosb				; save it
    inc	edx			; increment prefix count
nsl2:
    inc	ebx			; point to next prefix
    loop	sl2			; next prefix
    
    cmp	dl,2			;count of segment prefixes added
    jb	lpnerr			;can't exceed 1
    stc
    ret
lpnerr:
    clc
    ret
;
; routine displays error if operands are mismatched
;
; INPUT: none
; OUTPUT: message displayed
operr:
    call	printAlignedErrorMsg
    db	"Unusable operand combination",0
    stc
    ret
segdwordreg:
        cmp     byte [arg3 + asmop.mode],AM_NONE
        jnz     sdrx
        cmp     byte [arg1 + asmop.mode],AM_REG
        jnz     sdrb
        cmp     byte [arg1 + asmop.asize],DWORDSIZE
        jnz     sdrx
        cmp     byte [arg2 + asmop.mode],AM_SEG
        ret
sdrb:
        cmp     byte [arg2 + asmop.mode],AM_REG
        jnz     sdrx
        cmp     byte [arg2 + asmop.asize],DWORDSIZE
        jnz     sdrx
        cmp     byte [arg1 + asmop.mode],AM_SEG
sdrx:
        ret
;
; check for size mismatches and get a size.
; INPUT: SI points to user's input buffer/
; OK, a little bit of clarification has happened on this one, thankfully.
;  Turns out that opcodes fall into several categories with respect to this
; routine. In general, for any opcode with more than one operand, all
; subsequent operands must match one another in size except for the
; exceptions. The exceptions to be permitted are:
;	1) movzx and movsx, which by definition have mismatched operands
;	2) In and Out instructions, where DX holds the port, and we can
;	   read or write any size operand through that port
;	3) SHR,SAR,SHL,SAL,RCR,RCL CL, since any size operand can be shifted 
;	   by CL bits
;	4) Immediate operands, which can be smaller but not larger than
;	   their targets
;	5) Memory, if sizes to any memory mode are given they must match
;	   the rest of the arguments.
;

validops:
    cmp	byte [lastbyte],"x" 		;if movzx or movsx
    je	near vox			;then this is OK


;OK, the logic here is really hosed. What it is supposed to say is this:
;IF any operand is provided
;	IF more than one operand
;		IF all operands are not equal in size
;			IF not an allowed exception
;				THEN error
;			ELSE OK
;		ELSE OK
;	ELSE OK
;ELSE OK

;The problem here appears to be, immediates have size 0 unless they are
;prefixed with byte, word, etc.  So for example, any size operand
;can be stuck into [44], like AL, AX, or EAX.  Furthermore, it was a
;conscious design decision that mov al,FFFF is allowed as an editing
;function. That is, you can enter as many hex digits as you want, if
; the number is too big then digits on the left are truncated.  This
; allows changing the number without using the backspace key.


chsize:					; collect a size
    mov	al,[arg1 + asmop.asize]		;see if first arg size is 0
    or	al,al			;used later, so load into AL
    jnz	szch			;if non0 size, more checking
    mov	al,[arg2 + asmop.asize]		;else load up size of 2d arg
    or	al,al			;used again
    jnz	szch			;if non0, keep checking?
    mov	al,[arg3 + asmop.asize]		;check size of final arg
    or	al,al
    jnz	szch			;if it has non0 size
;        mov     al,WORDSIZE
;        test    [optass32],255
;        jz      szndw
        mov     al,DWORDSIZE
szndw:
        jmp     finalsize
szch:
    test	byte [arg1 + asmop.asize],0FFh		;if arg1 has a size
    jz	noa1			;
    cmp	al,[arg1 + asmop.asize]		;it must match the collected size
    jnz	absx			; or check for special cases
noa1:
    test	byte [arg2 + asmop.asize],0FFh		; if arg2 has a size
    jz	noa2			
    cmp	al,[arg2 + asmop.asize]		; it must match the collected size
    jne	absx  			; or we check for special cases
noa2:	
;Near as I can tell, we get here if:
;1) Both arg1 + asmop.asize and arg2 + asmop.asize are 0 
;2) Either or both is nonzero but matches the collected size
;
;We have to check the arg3 byte, this is necessary for example in
; the 386 imul instruction.  If both arg1 and arg2 had no size this
; extra compare makes no difference as there won't be an arg3 and the
; size will have been initialized to zero
    test	byte [arg3 + asmop.asize],0FFh		; if arg3 has a size
    jz	finalsize
    cmp	al,[arg3 + asmop.asize]		; it must match the collect size
    jne	absx			; or we check for special cases
finalsize:
    call	setsize			;set the size we found in all ops
    jmp	chimmsize		;chk immediates and based addressing
;
; get here on size mismatch, must check for special cases
;
absx:
        call    segdwordreg
        jz      vox
    mov	eax,dword [EnteredMnemonic] ; get ASCII for mnemonic
    and	eax,0ffffffh
    cmp	eax,"tuo"		; out?
    je	vox			; yes, no size error
    cmp	ax,"ni"			; in?
    je	vox			; yes, no size error
    mov	edi,arg2		; is arg2 cl?
    call	chkcl			;
    jz	vox                     ; yes, no size error
    mov	edi,arg3		; is arg3 cl?
    call	chkcl
    jz	vox
    call	printAlignedErrorMsg	; otherwise print an error and get out
    db	"Bad size",0
    stc
vox:
    ret
;
; get here if we had an immediate, must check sizing
;
chimmsize:
         mov	edi,arg1
    call	immsize			;see if immediate that will fit
        jc      badimm                     ;nope, won't fit
    call	chkbase			;else chk for valid based addressing
        jc      badbase                     ;not valid
         mov	edi,arg2		;
    call	immsize			; see if immed that will fit
        jc      badimm                     ; nope, won't fit
    call	chkbase			; check for valid based mode
        jc      badbase                     ; not valid
         mov	edi,arg3
    call	immsize			; see if immed that will fit
        jc      badimm                     ; nope, won't fit
    call	chkbase			; check for valid based mode
        jc      badbase                     ; not valid
    ret
badimm:
        call    printAlignedErrorMsg
        db      "Immediate out of range",0
        stc
        ret
badbase:
        call    printAlignedErrorMsg
        db      "Invalid based addressing",0
        stc
        ret


;One of the allowed exceptions is a shift or rotate of a register exceding
;8 bits by CL, which is 8 bits.  Here we check for that CL, and allow the
;exception if we find it
;INPUT: DI points to this structure
;OUTPUT: ZF if we found CL, NZ if we didn't

chkcl:
    cmp	byte [edi+asmop.mode],AM_REG	; check if register involved
    jne	cclerr			; if not, can't be CL
    cmp	byte [arg2 + asmop.areg1],isECX	; else see if ECX involved at all
    jne	cclerr			; if not, can't be CL
    cmp	byte [arg2 + asmop.asize],BYTESIZE	; if ECX is byte, must be CL
cclerr:
    ret


;INPUT: AL is an argument size 1=byte, 2=word, 4=dword
;	EnteredMnemonic is a bucket containing what the user typed
;	PrefixBitmapWord containing a bit for each type of prefix allowed.
;		AS_OPSIZE is for operand size override prefix OS:, which
;		means stick in a 66h
;	arg1,2, and 3 are instances of structure ASMOP. asize is the size
;		of the operand.  An instruction can have up to 3 operands
; What we do here is set the passed size as the size of all 3 operands,
; setting the 66 override if dword size (and not FP), and returning ZF
; if it was a dword, and NZ if it was not

setsize:
    cmp	byte [EnteredMnemonic],'f'	; floating point instruction?
    je	ssnoop				; yes, no opsize prefix
    cmp	al,DWORDSIZE			; is it a dword?
        jne     ssnoop2                          ; no, no opsize checking
    or	word [PrefixBitmapWord],AS_OPSIZE	;include 'OS:' prefix
ssnoop2:
        cmp     al,WORDSIZE
        jne     ssnoop
        or      word [PrefixBitmapWord],AS_OPSIZESET
ssnoop:
    mov	[arg1 + asmop.asize],al			;set all sizes the same
    mov	[arg2 + asmop.asize],al
    mov	[arg3 + asmop.asize],al
    cmp	byte [arg1 + asmop.asize],DWORDSIZE		;rtn NZ if NOT a dword
    clc
    ret
;
; Check the size of an immediate
;
;INPUT: DI points to structure built for this instruction
;	AL contains size of first argument
;If immediate operand, make sure that the size of the argument passed in
;AL is valid. Apparently an immediate dword is always OK, but it is necessary
;in that case to set a bit indicating the operand size prefix, 
;Otherwise, in cases of byte or word, it is necessary to make sure that
;the target (or segment if seg:ofs) does not exceed the immediate
;value in size.  This is not nearly well enough understood to draw any
;conclusions yet.
;  My guess is that if we have something like mov eax, immediate, then any
;immediate is OK.  If it is mov ax, immediate, then the value to be moved in
;must be a byte or word. Finally, if mov al, immediate, then the immediate
;value must be a byte
;OUTPUT: NC if immediate operand will fit in target, CY if not.

immsize:
    cmp	byte [edi+asmop.mode],AM_IMM		;see if immediate value
    clc					;assume not
    jne	immok				;and if not, we're OK
    cmp	al,DWORDSIZE			;else chk for dword size
    jae	immokl				;go if AL >=4 (dword)
    cmp	al,WORDSIZE			;else if AL is word size
    clc					;assume it is
    jne	bytech				;if not, go chk byte offset
    test	dword [edi+asmop.addrx],0ffff0000h	;else test for word offset
    jz	immok				;if so, we're ok
        cmp     dword [edi+asmop.addrx],0ffff0000h
        ; flags set right at this point
immok:
    ret
immokl:
    or	word [PrefixBitmapWord],AS_OPSIZE	;set this
    ret
bytech:
    test	dword [edi+asmop.addrx],0ffffff00h	;test for byte offset
    jz	immok				;OK if byte
        cmp     dword [edi+asmop.addrx],0ffffff00h
        ; flags set right at this point
    ret

;
; subroutine to verify that a based/indexed mode has a correct
; register combination
;
; INPUT: DI = pointer to operand (asmop) structure
; OUPUT: CY set on error, clear if ok
;
chkbase:
    cmp	byte [edi+asmop.mode],AM_BASED	;is it base+something?
    jne	near cbxnb				;if not, get out
    cmp	byte [edi+asmop.msize],BYTEMODE	;see if byte-mode addressing
    je	cberr				;no can do this
    cmp	byte [edi+asmop.msize],DWORDMODE	;how about dword?
    je	cb32				;go check 32-bit addressing
;
; if we get here we have 16-bit addressing.  No scale factors allowed.

    cmp	word [edi+asmop.ascale],TIMES1	;check scale factor against 1
    jne	cberr				;error if not 1
    cmp	byte [edi+asmop.areg1],isESP		;check for sp
    je	cberr				;error if trying to index off sp

    cmp	byte [edi+asmop.areg1],isEBX		;Carry clear if eax,ecx,edx
    jb	cberr				;error if trying to index off those

;areg2 is any second register (like [ebx+si+nnnn]

    cmp	byte [edi+asmop.areg2],0FFh		;any second register

;A table is emerging from the following.  It tells us:
;

    je	cbx				;didn't get to second base
    cmp	byte [edi+asmop.areg2],isESP		;is 2d base ESP
    je	cberr				;if so, illegal
    cmp	byte [edi+asmop.areg2],isEBX		;compare with EBX value
    jb	cberr				;error is ax,cx,dx
    cmp	byte [edi+asmop.areg1],isESI		;compare with ESI
    jae	cbdown				;ok for  si,di
cbup:
    cmp	byte [edi+asmop.areg2],isESI		; check second if si or di
    jz	cbx				; ok if so
    jmp	cberr				;err if anything else

;
; we got here if the first arg is si/di, in which case the second arg
; must be bx or bp

cbdown:
    cmp	byte [edi+asmop.areg2],isESI		;if bx or bp
    jb	cbx				;we're OK

;Errors go here. By implication, these errors are:
;1) using ESP at all for a base register
;2) using EAX, ECX or EDX as a base register
;3) using a register combo other than [esi + bx] [esi + bp] [edi + bx] [edi+bp]

cberr:
    call	printAlignedErrorMsg
    db	"Invalid base or index register",0
    stc
    ret

;
; we get here if we have a 32-bit address mode with based addressing
;

cb32:
;	test	[edisassemble32Bit],TRUE		;dwords allowed at all?
;	jz	cberr				;if not, bomb
    or	word [PrefixBitmapWord],AS_ADDRSIZE	;else set addrsize prefix
    cmp	byte [edi+asmop.areg1],isEBP		;see if EBP is first reg
    jne	cb32n2bp			;skip if not
    cmp	byte [edi+asmop.areg2],isEBP		;else if second is EBP
    je	cberr				;that's an error
cb32n2bp:
    cmp	byte [edi+asmop.areg2],isESP		;check for [exx + esp]
    jne	cbx				; if not, accept it
    cmp	word [edi+asmop.ascale],TIMES1	; else check for a scale factor
    jne	cberr				; error if not 1
cbx:
    push	eax
    mov	al,byte [edi+asmop.areg1]

;
; now we have to figure out whether DS or SS is the default segment

    and	al,6		; turn ebp into esp
    cmp	al,isESP	; is esp or ebp?
    jne	cbx1		; no
    mov	byte [DefaultSeg],2	; else default to sseg
cbx1:
    mov	al,byte [edi+asmop.areg2]

    and	al,6            ; turn ebp into esp
    cmp	al,isESP	; is esp or ebp?
    jne	cbxnb		; no
    mov	byte [DefaultSeg],2	; else default to sseg
cbxnb:
    pop	eax

    clc
    ret


; print out error message
; INPUT: The error message is embedded in the code immediately following the
;	call to printAlignedErrorMsg
;
; this allows 32 characters for the input string.  It tabs the error
; message over to 32 columns beyond the first column of input.  If the
; input took more than 32 characters the error message cannot be aligned
; and is just tagged right after the input.

printAlignedErrorMsg:
    mov	ecx,-1
    mov	edi,InputBuffer	;in buffer
    mov	al,13		;for a CR
    repne	scasb		;find it
    add	ecx,32		; space to line up errs
    jecxz	nospace
    jns	ok
    mov	ecx,1		;if can't align, use single space
ok:
    call	PrintSpace
    loop	ok
nospace:
    Call	Message
    db	"??? ", 0
    jmp	Message
;
; get the opcode and scan the tables for it
;
;
getcode:
    mov	byte [RepPfxBitmap],0	;No Reps/locks found
getcode3:
    mov	edi,EnteredMnemonic	;point to mnemonic buffer
getcode2:
    lodsb			;get input character
    cmp	al,' '		;see if space or below
    jbe	nomore		;if so, done, don't store
    stosb			;else store it
    cmp	al,':'		;was it a colon?
    je	nomore2		;if so, ignore it and end the name
    jmp	getcode2
nomore:
    dec	si   	       	; all done, backtrack
nomore2:
    mov	ah,[edi-1]      	;get last char we stuffed in buffer
    mov	[lastbyte],ah	; last byte is used by some commands
                ; string &c
    mov	al,0		; store the trailer
    stosb
    push	esi		;save where we left off in input string
    mov	esi,EnteredMnemonic	;point to buffer we just stuffed
    call	strlen		; length of name in buffer into AX
    inc	eax		; plus trailer
    mov	esi,EnteredMnemonic	; check for repeats and lock
    mov	edi,say_repne	;actual string 'repne'
    mov	ecx,eax		;length to compare
    repe	cmpsb		;see if a match, lowercase
        mov     bl,AF_REPNE      ;this is 2
    jz	near reps		;if a match, go to reps to stuff in [RepPfxBitmap]
    mov	esi,EnteredMnemonic	;else lets look for repe
    mov	edi,say_repe
    mov	ecx,eax
    repe	cmpsb
    mov	bl,AF_REPE	;this is 4
    jz	reps
    mov	esi,EnteredMnemonic
    mov	edi,say_rep	;just look for rep
    mov	ecx,eax
    repe	cmpsb
    mov	bl,AF_REP	;this is 1
    jz	reps
    mov	esi,EnteredMnemonic	
    mov	edi,say_lock	;look for lock
    mov	ecx,eax
    repe	cmpsb
    mov	bl,AF_LOCK	;this is 8
    jz	reps
    cmp	eax,4		;is the length 4 (including 0-terminator?)
    jnz	npf		;if not, go look it up
    cmp	byte [EnteredMnemonic+2],':'	;else maybe segment override, so check colon
    jne	npf		;not a colon, so go look it up
    movzx	eax,word [EnteredMnemonic]	;else, get 1st 2 chars in AX
    mov	edi,OverridePfxList	;point to string of possible prefixes
    mov	ecx,8		;there are 8, 2 of which I've never heard of
    repne	scasw		;see if any match
    jnz	npf		;if not, go look it up the hard way
    bts	word [PrefixBitmapWord],cx	;set prefix word bit for this prefix
    pop	esi		;back to our input line
    call	WadeSpace	;find next string
    jnz	getcode3	;got one, so start over
    stc			;else we failed???
    ret
npf:
    mov	esi,EnteredMnemonic	;point to buffer containing instruction
    call	LookupOpName	;and go look it up
    pop	esi		;restore SI ptr to next input
    jc	gcx		; get out if nonexistant
    call	WadeSpace	; see if any more...
gcx:
    ret
reps:
    pop	esi		;restore pointer to input
    or	[RepPfxBitmap],bl	;set bitmap for rep prefix found
    call	WadeSpace	;find next
    jnz	getcode3	;if more, parse that
    stc			;else invalid - something must follow rep
    ret
;
; get an operand
;
; INPUT: DI points to asmop structure for this arg
;	SI points to input buffer past opcode string
; OUTPUT: CY if arg is invalid
; PROCESSING:
;	1) init asmop structure
;
parsearg:
    mov	byte [edi+asmop.asize],NOSIZE
    mov	byte [edi+asmop.areg1],0FFh
    mov	byte [edi+asmop.areg2],0FFh
    mov	word [edi+asmop.ascale],1
    mov	dword [edi+asmop.addrx],0
    mov	byte [edi+asmop.mode],AM_NONE
    mov	byte [edi+asmop.msize],0
    call	WadeSpace      		;see if any more???
    jz	near gax	 		; comma taken care of by WadeSpace
    cmp	byte [esi],'['	;see if opening an indirect addr
    je	near getbrack		;if so, look for contents
        call    parsecontrol            ;else chk for control register
    jc	near gax			;error, bad ctrl reg
    jz	near gaxc			;good ctrol reg, we got it
    call	parsesize		;set width, 1-10
    jc	near gax			;bad width, bomb
    jz	near getbrack		;else found width, get inside bracket
    call	parseseg		;else check for segment arg
    jc	gax			;bad seg arg
    jz	gaxc			;if good one, find more
    js	getbrack		;if SF, seg is inside brackets
    call	parsereg		;so find register
    jz	gaxc			;got it, find mire
    mov	byte [edi+asmop.mode],AM_NONE	;assume it is just a number
    call	parseval		;look for an immediate
    jc	gax			;nope, bomb
        jz      gri1
        test    ebx,0ffff0000h
        jz      gri1
        ror     ebx,16
        mov     ax,fs
        cmp     ax,bx
        jz      gria
        mov     byte [edi + asmop.mode],AM_SEGOFFS
        push    ebx
        movzx   ebx,bx
        mov     dword [edi +asmop.addrx],ebx
        pop     ebx 
        shr     ebx,16
        mov     dword [edi + asmop.addrx2],ebx
        jmp     gaxc
gria:
        shr     ebx,16
        mov     dword [edi+asmop.addrx],ebx
        mov     byte [edi+asmop.mode],AM_IMM
        jmp     gaxc
gri1:
    mov	byte [edi+asmop.mode],AM_IMM	;else say it is an immediate
    mov	dword [edi+asmop.addrx],ebx	;so save that
    call	WadeSpace  		;find next
    cmp	al,':'			;a colon?
    jnz	gaxc			;if not, done
    inc	si			;else move past colon
    call	parseval		;and get target value
    jc	gax			;sorry, no value found
        jz      gri2
        movzx   ebx,bx
gri2:
    mov	dword [edi+asmop.addrx2],ebx	;else stash the value in addrx2

;Aha, I think I dig. This flag indicates that addrx1 contains a segment
;value and addrx2 has the offset.  If this flag is clear, addrx1 has an
;and addrx2 is inoperative.

    mov	dword [edi+asmop.mode],AM_SEGOFFS	;set flag
gaxc:
    call	WadeSpace
    clc
gax:
    ret

;Handle the case where we have something in brackets.
;SI points to the opening bracket character


getbrack:
        or      word [PrefixBitmapWord],AS_ADDRSIZESET
    lodsb			;consume the bracket
    mov	al,byte [edi+asmop.areg1] ;see if any segment reg
    cmp	al,0ffh
    jz	brklp		; none
    mov	byte [edi + asmop.areg1],0ffh ; set it back
    sub	ah,ah
    bts	word [PrefixBitmapWord],ax	; set the prefix bit
brklp:
    call	WadeSpace	;get next string
    jz	near brackerr	;found CR before closing bracket
    cmp	al,'+'		;see if plus sign
    jne	brnp		;might mean bracket-not-plus
    inc	si		;else move past plus sign
    call	WadeSpace	;and find next
    jz	near brackerr	;oops, no closing bracket

;We have ignored any plus sign if it was there

brnp:
    cmp	al,'-'		;is it a minus sign?
    je	brmem		;if so, must be a value?
    cmp	al,']'		;if not, closing bracket already?
    je	near brackx		;if so, go exit
    push	edi		;save pointer to buffer
    mov	edi,arg4	;point to arg4 for base-mode reg gathering
    call	parsereg	;see if a register?
    pop	edi		;restore buffer pointer

;parsereg cannot return CY, and does no bracket check anyway. No idea why 
;this line is even here

    jc	near brackerr	; if invalid fp or CRDRTR reg num
    jz	brreg		;ZF means we found a register
brmem:
    call	parseval		;glom a number into EBX
    jc	near brackerr		;get out if no number
        jz      gri3
        movzx   ebx,bx
gri3:
    add	dword [edi+asmop.addrx],ebx	;stick in as or segment
    call	WadeSpace		;get next
    jz	near brackerr		;still no closing bracket
        cmp     al,']'
        jz      brackx
        cmp     al,'+'
        jz      brklp
        cmp     al,'-'
        jz      brklp
        jmp     brackerr

;We get here if we found a named register inside the brackets, like [ebx

brreg:
    mov	byte [edi+asmop.mode],AM_BASED	;say base reg involved
    mov	ah,[arg4 + asmop.areg1]		;get which register it is
    mov	bl,[arg4 + asmop.asize]		;and width of register
    test	byte [edi+asmop.msize],0FFh	;see if anything assigned yet
    jz	notszyet		;nope, not yet
    cmp	byte [edi+asmop.msize],bl	;ok, bl is the size
    jnz	brackerr		;mismatch, I guess???
notszyet:
    mov	byte [edi+asmop.msize],bl	;else set the size
    call	WadeSpace
    cmp	al,'*'			;multiply operation
    jne	notscale		;nope, no scaling
    cmp	bl,DWORDSIZE		;else dword scaling?
    jne	brackerr		;must be dword reg for multiply?
    inc	esi			;move past *
    call	WadeSpace		;find next
    sub	al,'0'			;last char returned, cvt to decimal?
    cmp	al,TIMES1		;*1 is OK
    je	brackok1
    cmp	al,TIMES2		;*2 is OK
    je	brackok1
    cmp	al,TIMES4		;*4 is OK
    je	brackok1
    cmp	al,TIMES8		;*8 is OK
    jne	brackerr		;else, can't do it
brackok1:
    inc	esi			;bump SI past scaling factor
    mov	byte [edi+asmop.ascale],al	;and set factor in struct
reg2x:
    test	byte [edi+asmop.areg2],0FFh	;initialized to FF
    jns	brackerr		;so bit 7 better be set
    mov	byte [edi+asmop.areg2],ah	;if so stick areg1=reg into it
    jmp	brklp			;get next thing inside brackets

;Found a register that was NOT followed by a scaling factor. The magic code
;for the found register is in AH. We stick this register into areg1 unless
;areg1 is already in use, in which case we stick it in areg2.

notscale:
    test	byte [edi+asmop.areg1],0FFh	;has reg been assigned yet?
    jns	reg2x			;if not, stick reg into areg2
    mov	byte [edi+asmop.areg1],ah	;else, stick reg into areg1
    jmp	brklp

;OK, we found the closing bracket.  Presumably everything between brackets
;was kosher. We also get here with the construct [], with nothing in there.

brackx:
    cmp	byte [edi+asmop.msize],DWORDMODE	;addressing mode size=32?
    jne	brackn32			;if not, skip
    or	word [PrefixBitmapWord],AS_ADDRSIZE	;else set addrsize prefix flag
brackn32:
    inc	esi				;move past ]
    cmp	byte [edi+asmop.mode],AM_NONE		;see if empty
    Jne	gaxc				;if not, cool
    mov	byte [edi+asmop.mode],AM_MEM		;else set mode to memory
    mov	byte [edi+asmop.msize],WORDMODE	;see if word mode
;        test    [optass32],255
;        jnz     brackmdword
;        test    [edi+asmop.addrx],NOT 0FFFFH     ;see if any address
;        jz      gaxc                            ;if not, skip out
brackmdword:
    or	word [PrefixBitmapWord],AS_ADDRSIZE	;else set for ???
    mov	byte [edi+asmop.msize],DWORDMODE	;and say dword assumed??
    jmp	gaxc				;and jmp
brackerr:
    stc
    ret

; Parse possible control register reference
;
; INPUT: SI points to buffer containing this argument (a string)
; OUTPUT: For this arg, the mode and areg1 fields of the structure are
;	filled in.
;	NZ if we can't recognize the register
;	CY if index for register is out of range
; PROCESSING: Look for any control reg, debug reg, FP reg or TR reg. The
;	TR regs are the Appendix H test registers.
;		Beyond this, we allow up to 8 of each of the ST, CR and DR
;	even though there is no DR1 or DR2, nor any CR4,5,6 or 7. I guess
;	these exist in opcode space, just not in the CPU!
;
parsecontrol:
    mov	ax,word [esi]
    mov	cl,AM_CR
    cmp	ax,"rc"			;Control register CRx
    je	gotcontrol
    mov	cl,AM_DR
    cmp	ax,"rd"			;Debug register DRx
    je	gotcontrol
    mov	cl,AM_TR
    cmp	ax,"rt"			;Test registers, Appendix H
    je	gotcontrol
        mov     cl,AM_MMX
        cmp     ax,"mm"
        je      gotcontrol
    cmp	ax,"ts"			;ST(x for FP
    je	gotfpreg
    or	al,1
    ret

;Um. There are 8 each of the debug registers and FP stack registers, although
;DR1 and DR2 don't exist.  There are only 4 control registers, and I can't
;find any reference at all to any TRx registers.


gotcontrol:
    lodsw				;grab control reg 1st 2 chars
    lodsb				;and number
    sub	al,'0'			;convert to binary
    jc	gcerrx			;oops, below 0
    cmp	al,8			;see if register 8 or above
    jae	gcerrx			;error if so
    mov	byte [edi+asmop.areg1],al	;save which one
        mov     al,[esi]
        and     al,0dfh                 ; make uc
        cmp     al,'0'
        jc      gcg
        cmp     al,'9'
        jbe     gcerrx
        cmp     al,'@'
        jc      gcg
        cmp     al,'Z'
        jbe     gcerrx
        cmp     al,'_'
        je      gcerrx
gcg:
    mov	byte [edi+asmop.mode],cl	;save CL 
    sub	ax,ax			;set ZF
    ret
gcerrx:
        sub     esi,3
        or      al,1
        ret
gotfpreg:
        push    esi
    lodsw				;consume the 'ST'
    call	WadeSpace		;find next 
    cmp	al,'('			;is it (
    jne	asmgotch		;if not, check for NASM syntax
    inc	esi			;bump past (
    call	WadeSpace		;find next
    push	eax			;stack the char found
    inc	esi			;move past it
    call	WadeSpace		;find the next
    cmp	al,')'			;end of stack reference?
    pop	eax			;restore char between ()
    jne	badfpreg		;no close, so bitch

;We get here in two cas we found (x), or we didn't find (
;In the first case, x is in AL, in the second, AL has what we found instead
;By implication, we accept either ST(x) or STx

asmgotch:
    sub	al,'0'			;convert to binary
    jc	badfpreg		;must be some number
    cmp	al,8			;else see if in range
    jae	badfpreg		;if >=8, out of range
    inc	esi			;move to next position anyway
    mov	byte [edi+asmop.mode],AM_FPREG	;set flag for FP
    mov	byte [edi+asmop.areg1],al	;save binary value of this reg
        add     sp,2
    sub	eax,eax			;and return ZF
    ret
badfpreg:
        pop     esi
        or      al,1
    ret


;
; parse a size attribute.  the PTR keyword is optional.
;
; INPUT: SI points to buffer containing this argument (a string)
; OUTPUT: bl has size
;         CY set if error ins size
parsesize:
        mov     byte [farptr],0
chkfar:
    push	esi			;else look for 'ptr'
    push	edi
    mov	ecx,3
        mov     edi,say_far
    repe	cmpsb
    pop	edi
    pop	esi
        jnz     fardone                    ;if not, we're ok
        or      byte [farptr],1              ; far keyword
    add	esi,3			;else skip wasted 'ptr'
        call    WadeSpace
        jz      near operrx
        cmp     al,'['
        je      near opok
fardone:
    mov	ax,[PrefixBitmapWord]	;get bitmap into AX
    call	isbyte			;chk buffer for 'byte'
    mov	bl,BYTESIZE		;assume byte size
    jz	gotsize			;if match, fine
    call	isword			;else chk for word
    mov	bl,WORDSIZE		;assume it was
    jz	gotsize			;if so, fine
    inc	esi			;go past first char (could be t)
    call	isbyte			;and check if it was 'tbyte'
    jnz	notbyte			;if not, really not a byte
    cmp	byte [esi-1],'t'	;else, was it a t
    mov	bl,TBYTESIZE		;assume it was a tbyte
    jz	gotsize			;yes, it was
        cmp     byte [esi-1],'p'     ; else, was it a p
        mov     bl,PBYTESIZE
        jz      gotsize
    or	bl,bl			;else return NZ
    ret

;OK, it's not 'byte', 'word' or 'tbyte'. Maybe it's 'dword'. To get here,
;SI has been moved past the first char, so see if it is dword, qword'or fword

notbyte:
    call	isword			;check for xword
    jnz	notsize			;nope, not it
    mov	al,[esi-1]		;else get that x
    cmp	al,'d'			;was it dword
    mov	bl,DWORDSIZE		;assume it was
    je	gotsize			;yep, got it
    cmp	al,'q'			;was it qword
    mov	bl,QWORDSIZE		;assume it was
    je	gotsize			;yep, that's it
    cmp	al,'f'			;how about fword
    mov	bl,FWORDSIZE		;assume that
    je	gotsize			;yes
notsize:
    dec	esi			;back to beginning of string
    or	bl,1			;set NZ
    ret
gotsize:
    mov	byte [edi+asmop.asize],bl	;set requested size
    add	esi,4			;move past string in buffer

;By implication here, it is legal to type 'ptr' as many times as you want.
;You can say mov word ptr ptr ptr ptr [44],5

gs2:
    call	WadeSpace		;find next nonspace
    jz	operrx			;didn't find, so error
    cmp	al,'['			;'ptr' is optional, so chk bracket
    je	opok			;if so, fine
    push	esi			;else look for 'ptr'
    push	edi
    mov	ecx,3
    mov	edi,say_ptr
    repe	cmpsb
    pop	edi
    pop	esi
        clc
        jnz     opok                    ;if not, we're ok
    add	esi,3			;else skip wasted 'ptr'
    jmp	gs2			;
operrx:
    stc
opok:
    ret

; compare input string with 'byte'
;
; INPUT: SI points to buffer containing this argument (a string)
; OUTPUT: ZF if the argument is 'byte' NZ otherwise
; PROCESSING: Simply compare
; NOTE: AX must be preserved
;
isbyte:
    push	esi
    push	edi
    mov	cx,4
    mov	edi, say_byte
    repe	cmpsb
    pop	edi
    pop	esi
    ret
;
; compare input string with 'word'
;
; INPUT: SI points to buffer containing this argument (a string)
; OUTPUT: ZF if we matched 'word', else NZ
; PROCESSING: just compare
;	AX cannot be modified
;
isword:
    push	esi
    push	edi
    mov	cx,4
    mov	edi, say_word
    repe	cmpsb
    pop	edi
    pop	esi
    ret

;
; INPUT: SI points to buffer containing this argument (a string)
; OUTPUT: CY if bad segment argument
;	ZF if good segment argument on its own
;		In this case, mode and size are set
;	SF if valid segment is followed by [
; PROCESSING: 
;	1) See if string matches list of seg names, return NZ if not
;	2) See if segment is only arg (like mov ax,cs. If so, return ZF
;	3) Else, see if it is seg:[ and if so, return SF
;	4) Else return carry, bad segment argument
;
parseseg:
                                        ; the following screens out ESP and ESI
        mov     al,byte [esi+2]      ; get third char
        and     al,0dfh                 ; make upper case
        cmp     al,'@'                  ; is it a letter
        jc      pssearch                ; no, do search for seg
        cmp     al,'Z'                  ; ?
        jbe     noseg                   ; yes no seg
pssearch:
        cmp     al,'0'
        jc      pssearch2
        cmp     al,'9'
        jbe      noseg
pssearch2:
        cmp     al,'_'
        jz      noseg
        cmp     byte [esi+2],'i'
        jz      noseg
    mov	cx,6	  		;6 possible segments by name
    push	edi			;save ptr to struct
    mov	edi,psegs		;names of registers
    lodsw				;get what user entered
    repne	scasw			;find it in the strings list
    pop	edi			;restore struct ptr
    jz	gotseg			;found a match
    sub	si,2			;not a seg, undo the lodsw
noseg:                          
        or      esi,esi
    ret				;return NZ

;Here we have a significant departure from debug.exe syntax.  If you want
;a segment override in debug, you must put the override like CS: or 
;on a separate line, and the remainder on the next line.  Debug requires you
;to say:
;xxxx:xxxx cs:
;xxxx:xxxx mov al,[44]
;
;Here you are NOT allowed to do this.  Instead, you must enter:
;xxxx:xxxx mov al,cs:[44]
;
;an earlier part of the program also accepted:
;
;xxxx:xxxx cs:mov al,[44]
;

gotseg:
    sub	cx,6			;sub starting value
    not	cx			;convert to index
    mov	byte [edi+asmop.areg1],cl	;save that index in areg1
    call	WadeSpace		;find next non-0
    jz	segalone		;if nothing, fine, just seg
    cmp	al,':'			;else chk for colon
    jne	segalone		;if not, we just got the segment
    inc	esi			;bump past colon
    call	WadeSpace		;find next stuff
    jz	segerr			;nothing else is error
    cmp	al,'['	 		;do we have a fixed address next?
    jne	segerr			;if not, real trouble
    or	al,80h			;else return SF
    ret
segalone:
    mov	byte [edi+asmop.mode],AM_SEG	;say mode is just a segment
    mov	byte [edi+asmop.asize],WORDSIZE	;so size is 2 (all segregs are word)
    sub	ax,ax			;return ZF
    ret
segerr:
    stc				;error so return CY
    ret

;
; parse a register name
;
; INPUT: SI points to input being parsed
;	DI points to structure for this argument, or to arg4 if we are
;	inside brackets.
; OUTPUT: ZF if reg found, and mode and size set
;	NZ if not a name
; PROCESSING: parse for all register names, set size and mode if found any,
;	else return NZ if not found
;NOT
;	1) If a register name was found, SI was bumped past it, else SI
;	   remained unmodified
;	2) At least one caller to this routine checks the carry. I can't
;	   find anything in here that would set or clear the carry as a
;	   return value...	
;
parsereg:
    mov	bl,2			;set word size
    cmp	byte [esi],'e'	;is first char an e
    jne	nextreg			;if not, fine
    mov	bl,4			;else set dword size
    inc	esi			;and move past the e
nextreg:
        mov     al,[esi+2]
        and     al,0dfh                 ; make upper case
        cmp     al,'@'                  ; is it a letter
        jc      prnum                   ; no, do search for seg
        cmp     al,'Z'                  ; ?
        jbe     noreg                   ; yes no seg
prnum:
        cmp     al,'0'
        jc      prnum2
        cmp     al,'9'
        jbe      noreg
prnum2:
        cmp     al,'_'
        jz      noreg
nextreg3:
    lodsw				;get next 2 bytes
    mov	ecx,16			;there are 16 2-char strings for regs
    push	edi			;save di
        mov     edi,regs          ;oto scan list of reg names
    repne	scasw
    pop	edi
    jz	gotreg			;if ZF, we hit a match
    sub	esi,2			;else back to beginning of input
noreg:
    cmp	bl,4			;unless we moved past an e
    jnz	nextreg2		;if bl=4, we did
    dec	esi			;so back up to first char
nextreg2:
    or	esi,esi			;set NZ
    ret				;since not a reg

;It just so happens that the regs string has 16 2-char entries, the first 8
;are byte registers and the last 8 are word registers

gotreg:
        xor     cl,15                   ;really, how about 0Fh?
    mov	byte [edi+asmop.asize],BYTESIZE	;say byte arg
    cmp	ecx,8			;if found in positions 0-7
    jb	gr1			;then it was a byte
    mov	byte [edi+asmop.asize],bl	;else word or dword depending on e
gr1:
    and	cl,7		   	;get mod8 for index
    mov	byte [edi+asmop.areg1],cl	;and save that
    mov	byte [edi+asmop.mode],AM_REG	;say a reg asked for by name
    sub	eax,eax			;return ZF
    ret
    
;
; Make sure that the next string is a number, and return in EBX if so
;
; INPUT: SI points to string to be parsed
; OUTPUT: NC if we find a number
;	EBX contains that number (0=extended if required)
;	CY if not a number, or not found???
; PROCESSING: DI points to the argument structure being built, and must
;	be preserved.
;NOT ReadNumber simply assumes that all bytes in the input buffer are
;	hex values up to some special character like space, comma, colon
;	or CR. So ReadNumber cannot return an error. In this case, we
;	already know that we have no named register at [esi], which is
;	a good thing, because if we did, ReadNumber would return the
;	current contents of that logical register.
;
parseval:
    call	WadeSpace	;find arg string
    jz	noval		;oops, not there
;        push    si
;        push    di
;        call    symReadName
;        push    es
;        push    ecx
;        push    si
;        mov     esi,dgroup:symname
;        call    symLookupByName
;        pop     si
;        pop     ecx
;        jc      ranosym
;        mov     bx,word [edi+4] ; EBX has seg: offset
;        ror     ebx,16
;        mov     bx,word [edi]
;        pop     es
;        pop     di
;        add     sp,2 ; get rid of old si, also NZ means it was a symbol
;        ret
;ranosym:
;        pop     es
;        pop     di
;        pop     si
        
    push	edi		;else save location
    call	ReadNumber	;read a number into eax
    pop	edi		;restore pointer
    mov	ebx,eax		;return number in ebx
        sub     eax,eax         ;ZR means it was a number
    ret
noval:
    stc
    ret
