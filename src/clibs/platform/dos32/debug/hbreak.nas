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
%include  "mtrap.ase" 
%include  "input.ase"

    global hdwebreakcommand
    global hdwebreakenable
    global hdwebreakdisable
    global hdwechk
    segment data USE32
bkaddr	times 4 [dd	0]	;breakpoint address, flat
bkreg	dd	700h		;mirror of breakpoint enable register
bksets	dw	0		;track which breaks we have set

    segment code USE32
;
; enable hardware breaks
;
hdwebreakenable :
        mov     ax,cs
        test    al,3
        jnz     hbex
    mov	eax,[bkreg]	; copy our image to the bp enable reg
    mov	DR7,eax
hbex:
    ret
;
; disable hardware breaks
;
hdwebreakdisable :
        mov     ax,cs
        test    al,3
        jnz     hbdx
    mov	eax,700h		; clear the BP enable reg
    mov	DR7,eax
hbdx:
    ret
;
; display BP data
;
putbpdata2	:
    call	crlf		; CRlf #)
    add	dl,'0'
    call	PutChar
    mov	dl,')'
    call	PutChar
    bt	word [bksets],cx
    jc	putbpdata
    Msg	" Not enabled"
    ret

;this routine prints strings based on the value in BX[1:0]. if 0, it prints
;'Execute', if 1, it prints 'Write' if 2 or 3 it prints 'Read/Write'. Then
;it prints 'at address', prints whatever is in [di+2], a colon, then whatever
;is in [di].
;  NOW, BX contains some kind of mode flags and DI points to the break
;address.

putbpdata :
    mov	al,bl    	; print mode
    and	al,3
    or	al,al
    jnz	npx
    Msg	" Execute   "
    jmp	pbc
npx:
    dec	al
    jnz	nrx
    Msg	" Write     "
    jmp	pbc
nrx:
    Msg	" Read/Write"
pbc:
    Msg	" at Address "	; print address
    mov	eax,[edi]
    call	PrintDword
    ret
;
; first thing called from BP routine (int 1)
;
; slightly buggy as DS on rare occasions gets changed...
;
hdwechk :
    push	eax
        mov     ax,cs
        test    al,3
        jnz     nohdwe
    MOV	eax,DR6			; mask active breakpoints
    and	al,byte [bksets]	; breakpoints set?
    and	al,0fh		;just 4 of them???
    jz	nohdwe		;none set, I guess
    push	es		; got a Breakpoint, load regs
    push	ds		;set ES to our data
    pop	es
    pushad			;save all registers
    push	ax		;ax contains the breakpoint bitmap?
    Msg	10,13,"Hardware breakpoint #"	; message
    pop	ax		;restore bitmap
    mov	edi,bkaddr	; get address
    mov	bx,word [bkreg+2]	; and mode flags
    mov	cx,4		
hdwechks:
    shr	al,1		; find break - bit into carry flat
    jc	found		;yes, this was it
    shr	bx,4		;else BX has mode flags for this break?
    add	di,4		;and DI has break address pointer
    loop	hdwechks	;find the break

;Well, CX will contain the value 1, 2, 3, or 4. When we put this into DX and
;NEG it, DX will contain FFFF, FFFE, FFFD, or FFFC.  When we add 34h to this,
;DX contains the ASCII codes for 3,2,1 and 0 respectively for CX values of
;1,2,3 and 4.

found:
    mov	edx,ecx		; set up for print
    neg	edx		;get 2s complement
    add	edx,'4'		;convert to ascii
    call	PutChar		;paint it

;this routine prints strings based on the value in BX[1:0]. if 0, it prints
;'Execute', if 1, it prints 'Write' if 2 or 3 it prints 'Read/Write'. Then
;it prints 'at address', prints whatever is in [di+2], a colon, then whatever
;is in [di].
;  NOW, BX contains some kind of mode flags and DI points to the break
;address.


    call	putbpdata	; do it
    popad
    pop	es
    stc			; BP found, break unconditionally
nohdwe:
    pop	eax
    ret
;
; command to set/clear breakpoints
;
; bd command comes here from breaks.asm
;
hdwebreakcommand :
        mov     ax,cs
        test    al,3
        jz      hbok
        push    esi

        call    Message
        db "Hardware break points not enabled on DPMI ring 3",13,10,0
        jmp     bccerr3
hbok:
    inc	esi
    call	WadeSpace	; if no parms, print all
    jz	near printhwd
    cmp	al,'-'		; check for clear
    pushfd
    jnz	noinc
    inc	esi
    call	WadeSpace
noinc:
    cmp	al,'*'		; clear all
    jz	clearall
    call	ReadNumber	
    jc	near bccerr2         ; err?
    movzx	ecx,al		; move tp cx
    cmp	al,4   		; check range
    jnc	near bccerr2
    popfd			; check set/clear
    jnz	doset
    call	WadeSpace
    jnz	near bccerr
    btr	word [bksets],cx	; reset the masks
    shl	ecx,1
    inc	ecx
    btr	word [bkreg],cx
    clc
    ret
clearall:
    popf         		; make sure -
    jnz	near bccerr
    mov	dword [bkreg],700h	; now kill all masks
    mov	word [bksets],0
    clc
    ret
doset:
    call	WadeSpace	; if nothing else, print it
    jz	near print1

    call	ReadAddress	; now read the address
    jc	near bccerr
    mov	edi,0     	; assume xec
    call	WadeSpace
    jz	gotmode2
    cmp	al,'r'         	; check for r
    jnz	chkw
    mov	edi,3		; is read/write
    jmp	hbrsize
chkw:
    cmp	al,'w'		; check for w
    jnz	chkx
    mov	edi,1		; is write
    jmp	hbrsize
chkx:
    cmp	al,'x'
    jnz	near bccerr
    mov	edi,0
hbrsize:
    inc	esi		; now get the size
    call	WadeSpace
    push	esi
    jz	gotmode2		; no size, di already set
    call	ReadNumber
    jc	near bccerr3
    push	eax
    call	WadeSpace
    pop	eax
    jnz	bccerr3
    movzx	esi,al           ; check for size in range
    cmp	esi, 4
    ja	bccerr3
    cmp	esi,3
    je	bccerr3
    or	esi,esi
    jz	bccerr3
    dec	esi   		; make correct for debug reg
    mov	eax,ebx 		; check for alignment
    and	eax,esi
    jnz	bccerr3
    or	di,di     	; EXEC size must be one
    jnz	gotmode
    or	esi,esi
    jnz	bccerr3
gotmode2:
    sub	esi,esi
gotmode:
    bts	[bksets],cx	; set the first mask
    shl	ecx,1		; the second mask
    inc	ecx
    bts	word [bkreg],cx
    dec	ecx
    shl	esi,2 		; shift the length into place		
    shl	ecx,1
    or	esi,edi		; length & mode shift into place
    shl	esi,cl
    or	word [bkreg + 2],si ; and set it up
    mov	esi,ecx
    mov	dword [esi + bkaddr],ebx
noxbx:
    push	ebx
    push	ecx
    mov	ebx,ds
    mov	ax,6
    int	31h
    mov	ax,cx
    shl	eax,16
    mov	ax,dx
    pop	ecx
    pop	ebx
    add	eax,ebx
    mov	ebx,ecx		; branch to routine to set appropriate bp addr reg
    add	ebx,one
    call	ebx
    pop	esi
    clc
    ret
bccerr3:
    pop	esi
    stc
    ret
one:
    mov	DR0,EaX
    ret
two:
    mov	DR1,EaX
    ret
three:
    mov	DR2,EaX
    ret
four:
    mov	DR3,EaX
    ret
;
; print command for one bp comes here
;
print1:
    mov	edx,ecx			; get the mode to bl
    shl	ecx,2
    movzx	ebx,word [bkreg + 2]
    shr	ebx,cl
    mov	edi,bkaddr	; di = ptr to address
    add	edi,ecx	
    shr	ecx,2
    call	putbpdata2		; print it
    clc
    ret
bccerr2:
    pop	eax
bccerr:
    stc
    ret
;
; print command for all bp comes here
;

printhwd:
    mov	ecx,0			; index  = 0
    mov	edi,bkaddr	; ptr to first bp
    movzx	ebx,word [bkreg + 2]	; grab modes
phwl:
    push	ecx		; display bp
    push	ebx
    push	edi
    mov	dl,cl		
    call	putbpdata2
    pop	edi
    pop	ebx
    pop	ecx
    shr	ebx,4		; shift to next BP
    add	edi,4
    inc	cl
    cmp	cl,4
    jc	phwl
    ret
