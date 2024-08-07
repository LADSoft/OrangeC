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
%include "asm.ase"
%include "opcodes.asi"
%include "opcodes.ase"
%include "opcom.ase"
%include "prints.ase" 
%include "input.ase"
%include "mtrap.ase"
%include "dispatch.ase"
%include "operands.ase"

    global	aop0,  aop1,  aop2,  aop3,  aop4,  aop5,  aop6,  aop7
        global  aop8,  aop9,  aop10, aop11, aop12, aop13, aop14, aop15
    global	aop16, aop17, aop18, aop19, aop20, aop21, aop22, aop23
    global	aop24, aop25, aop26, aop27, aop28, aop29, aop30, aop31
    global	aop32, aop33, aop34, aop35, aop36, aop37, aop38, aop39
    global	aop40, aop41, aop42, aop43, aop44, aop45, aop46, aop47
    global	aop48, aop49, aop50, aop51, aop52, aop53, aop54, aop55
        global  aop56, aop57, aop58, aop59, aop60, aop61, aop62, aop63
        global  aop64, aop65, aop66, aop67, aop68, aop69, aop70

    segment data	USE32
base16c	dw	0ff03h,0ff05h,0ff07h,0ff06h,0705h,0605h, 0703h,0603h
    dw	0ffffh,0ffffh,0ffffh,0ffffh,0507h,0506h, 0307h,0306h

    segment code	USE32

;Check if arg1 + asmop.asize is 4 or less.  If greater, return error to caller's
;caller by mucking with the stack.

intsize:
    cmp	byte [arg1 + asmop.asize],DWORDSIZE	;maybe this means the first argument
    ja	rtnDirectlyToDispatcherWithCY	;can't exceed dword size
    ret

;See if the mode field for this structure references a memory operand
;and return error to caller's caller if not

chkmem:
    cmp	byte [ebx+asmop.mode],AM_MEM		;this is 3
    jne	rtnDirectlyToDispatcherWithCY	;bomb if ~memory operand
    ret

;See if the mode field for this structure references an immediate operand
;and return error to caller's caller if not

chkimm:
    cmp	byte [ebx+asmop.mode],AM_IMM		;check if immediate
    jne	rtnDirectlyToDispatcherWithCY	;if not, bomb
    ret

;See if the mode field for this structure references a segment
;and return error to caller's caller if not

chkseg:
    cmp	byte [ebx+asmop.mode],AM_SEG
    jne	rtnDirectlyToDispatcherWithCY
    ret

;This is a little more complicated. Let's hypothesize that chkregw is 
;shorthand for checking a 16-bit register type opcode. In this case,maybe the
;opcode must contain more than a single byte.  So before we check if it
;is a register-type opcode, we must make sure it has extra byte(s). So
;we stop here first, and if the length makes us happy, then we check mode
;  Next guess - if this is a byte argument, we bomb since a word register
;requires a word operand.

chkregw:
    cmp	byte [ebx+asmop.asize],BYTESIZE	;is this a byte opcode
    je	rtnDirectlyToDispatcherWithCY	;if so, bomb
    jmp	chkreg			;else check mode for register type

;Possibly this is used for in and out, where DX is the only register
;allowed?

chkdx:
    cmp	byte [ebx+asmop.asize],WORDSIZE	;if ~a word register
    jne	rtnDirectlyToDispatcherWithCY	;then bomb
    cmp	byte [ebx+asmop.areg1],isEDX		;else see if DX register
    jne	rtnDirectlyToDispatcherWithCY	;if not, bomb
    jmp	chkreg

;~sure about the AX part of this, since all that is being checked is
;a size of byte

chkaxw:
    cmp	byte [ebx+asmop.asize],BYTESIZE	;if a byte register
    je	rtnDirectlyToDispatcherWithCY	;then bomb

;This IS a check for AX (actually, 0 is EAX)

chkax:
    cmp	byte [ebx+asmop.areg1],0	    	;if ~EAX (maybe AX or AL)
    jne	rtnDirectlyToDispatcherWithCY	;then bomb

;Check for a register?  As opposed to ??

chkreg:
    cmp	byte [ebx+asmop.mode],AM_REG		;is this flag set?
    jne	rtnDirectlyToDispatcherWithCY	;if not, bomb
    ret

;Check for a mmx register?  As opposed to ??

chkmmx:
        cmp     byte [ebx+asmop.mode],AM_MMX          ;is this flag set?
    jne	rtnDirectlyToDispatcherWithCY	;if not, bomb
    ret

;We bomb if arg3 + asmop.mode isn't none, and if it is, we still bomb if arg2 + asmop.mode
;is none.  So arg3 is required to have a 'mode', whatever that is, I guess

noarg3:
    cmp	byte [arg3 + asmop.mode],AM_NONE
    jne	rtnDirectlyToDispatcherWithCY
    cmp	byte [arg2 + asmop.mode],AM_NONE
    je	rtnDirectlyToDispatcherWithCY
    ret

;Same as above, except it applies to arg2

noarg2:
    cmp	byte [arg2 + asmop.mode],AM_NONE
    jne	rtnDirectlyToDispatcherWithCY
    cmp	byte [arg1 + asmop.mode],AM_NONE
    je	rtnDirectlyToDispatcherWithCY
    ret

;Whoa, lets see here.  If arg1 + asmop.mode, whatever that is, is ~0, we jnz to
;rtnDirectlyToDispatcherWithCY. Since this routine was called, the return 
;address is on the stack. Which means rtnDirectlyToDispatcherWithCY strips 
;off the return address, and returns to the caller's caller. 

noarg1:
    cmp	byte [arg1 + asmop.mode],AM_NONE
    jne	rtnDirectlyToDispatcherWithCY
    ret
;
; pops the ret address from the aop caller
; and goes directly back to the parser with a mismatch error
;
rtnDirectlyToDispatcherWithCY:
    pop	eax
    stc
    ret


;see if size of this opcode is 1. If not, we are returning NZ and AL=1. I
;assume that 1 is important, since without the OR line, we would also be
;returning NZ if the size was ~1. In fact, several callers immediately
;do a stosb, so at the very least we are ensuring an odd number???
;  Next guess: AL will eventually be used as part of the stored, assembled
;byte.  So apparently the purpose of this routine is to set bit 0 of the
;untimate assembled value if this is ~a byte operand.

sizeb0:
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	szb0x
    or	al,1
szb0x:
    ret

;In which case, the purpose of this routine is to set bit 3 of AL if the
;operand is ~a byte operand.

sizeb3:
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	szb3x
    or	al,8
szb3x:
    ret
;
; main RM handler
;
; within this routine, CX = pointer to mod/RM byte and DX = flag for
; <e>bp based indexing
;
; all MOD/RM fields are set up assuming no  and updated 
; later...
;
asmfrm:
    cmp	byte [ebx+asmop.mode],AM_FPREG ; check for register direct mode
    jne	asmrm2
    or	al,0c0h			; modrm = 0c0+al+regval
    or	al,[ebx+asmop.areg1]
    stosb
    ret
    

asmrmmmx:
        cmp     byte [ebx+asmop.mode],AM_MMX
        jne     asmrm
        shl     al,3
        or      al,0c0h
        or      al,[ebx + asmop.areg1]
        stosb
        ret


asmrm:
    shl	al,3			; shift reg val into place


asmrm2:
;
; registers are the same in both 16 & 32-bit
;
    cmp	byte [ebx+asmop.mode],AM_REG	; 1check for register direct mode
    jne	rmnreg
    or	al,0c0h			; modrm = 0c0+al+regval
    or	al,[ebx+asmop.areg1]
    stosb
    ret
rmnreg:
    cmp	byte [ebx+asmop.msize],DWORDMODE	; see which style of addressing
    je	asmrm32
;
; 16-bit MEM handler
;
    cmp	byte [ebx+asmop.mode],AM_MEM	; 16 bit, check for mem
    jne	rm16nmem
    or	al,6			; MODRM = 6+al
    stosb
    jmp	rmwoffs
rm16nmem:
    cmp	byte [ebx+asmop.mode],AM_BASED ; check for 16 bit based
    jne	rtnDirectlyToDispatcherWithCY
;
; 16-bit base handler, restricted combos and no scaling
;
    push	eax			; yes, get the reg vals
    sub	eax,eax
    mov	al,[ebx+asmop.areg1]
    mov	ah,[ebx+asmop.areg2]
    mov	edx,eax
    push	edi			; scan for a match and to get index
    mov	edi, base16c
    mov	ecx,16
    repne	scasw
    pop	edi
    pop	eax
    jnz	rtnDirectlyToDispatcherWithCY	; scan failed, exit
    bt	ecx,3
    jc	rm16nswap
    xchg	dl,dh
rm16nswap:
    and	cl,7
    or	al,cl
    mov	ecx,edi			; cx points at modrm byte
    stosb
    jmp	rmanyoffs		; check for offs
;
; 32-bit modes start here
;
asmrm32:
    cmp	byte [ebx+asmop.mode],AM_MEM	; 32 bit, check for mem
    jne	rm32nmem
;
; generic mem handler
;
    or	al,5			; MODRM = 5+al
    stosb
    jmp	rmwoffs
rm32nmem:
    cmp	byte [ebx+asmop.mode],AM_BASED 	; check for based
    jne	rtnDirectlyToDispatcherWithCY	; ~legal if not
    cmp	byte [ebx+asmop.areg2],0FFh	; else see if second reg defined
    je	rm32bbonly
    cmp	byte [ebx+asmop.areg1],0FFh	; else see if first reg defined
    je	rm32bionly
;
; both a base AND an index
;
; the parser should have checked that ESP is ~the scond arg
; and that we don't have two EBP args
;
    cmp	byte [ebx+asmop.areg1],isEBP	; check for BP based
    je	rm32bbsebp
    or	al,4			; flag MOD for SIB
    mov	ecx,edi
    stosb
    bsf	ax,[ebx+asmop.ascale]	; calculate SIB byte
    shl	ax,3
    or	al,[ebx +asmop.areg2]
    shl	ax,3
    or	al,[ebx+asmop.areg1]
    stosb
    sub	edx,edx
    jmp	rmanyoffs
;
; base and index, base is ebp
rm32bbsebp:
    or	al,4
    mov	ecx,edi
    stosb
    bsf	ax,[ebx+asmop.ascale]	; calculate SIB byte
    shl	eax,3
    or	al,[ebx+asmop.areg2]
    shl	eax,3
    or	eax,5			; bp based
    stosb
    mov	edx,0ff05h
    jmp	rmanyoffs
;
; index only
rm32bionly:
    or	al,4 			; select SIB byte
    stosb
    bsf	ax,word  [ebx+asmop.ascale] ; calculate scale factor
    shl	eax,3			; shift over
    or	ax,word  [ebx+asmop.areg2] ; add in index
    shl	eax,3			; shift over
    or	eax,5			; base = 5, that is none since mod = 0
    stosb
    mov	eax,[ebx+asmop.addrx]	; always 32-bit ofs in this mode
    stosd
    ret
;
; only a base reg
;
rm32bbonly:
    ; shouldn't get here if no regs
    cmp	byte [ebx+asmop.areg1],isESP	; only one reg, check for ESP
    je	rm32besp
    cmp	byte [ebx+asmop.areg1],isEBP	; only one reg, check for EBP
    je	rm32bebp
    or	al,[ebx+asmop.areg1]	; no, normal MODRM byte with no SIB
    mov	ecx,edi
    stosb
    mov	dh,0FFh			; pretend 16 bit
    mov	dl,al
    and	dl,7
    jmp	rmanyoffs
;
; based ESP, no index
;
rm32besp:
    or	al,4    		; select SIB byte (sib needed
    mov	cx,di			; for indexing off ESP)
    stosb
    mov	al,24h			; sib byte for [esp] mode
    stosb
    sub	edx,edx
    jmp	rmanyoffs
;
; based ebp, no index
;
rm32bebp:
    or	al,5
    mov	ecx,edi			; so we can adjust mod later
    stosb
    mov	edx,0ff05h		; MUST have , this is BP base reg
    jmp	rmanyoffs
;
; now the auto  routine.
; if (E)BP, we ALWAYS have an 	
; otherwise we see if the  is zero before encoding anything
    
rmanyoffs:
    mov	eax,[ebx+asmop.addrx]	; get offs
    cmp	dx,0ff05h		; BP reg ALWAYS has offs
    je	rmaomho

    or	eax,eax 		; else no offs if offs = 0
    jz	rmaook
;
; if we get here we have an , fix the RM field and embed the
; 
;
rmaomho:
    add	byte  [ecx],40h	; else set MOD field = 1
    and	eax,~7FH		; else see if fits in a byte
    jz	rmboffs
    cmp	eax,~7fh
    je	rmboffs    		; yep, go do it
    add	byte  [ecx],40h	; else MOD field = 2
    jmp	rmwoffs			; and go do it

rmboffs:
    mov	eax,[ebx+asmop.addrx]	; get offs
    stosb      			; store a byte offs
rmaook:
    clc
    ret
rmszoffs:
    mov	eax,[ebx+asmop.addrx]
    cmp	byte [ebx+asmop.msize],WORDMODE
    jae	rmwoffs
    stosb
    clc
    ret
rmwoffs:
    mov	eax,[ebx+asmop.addrx]	; check size
    cmp 	byte [ebx+asmop.msize],WORDMODE
    ja	rmdwoffs
    stosw				; word offs
    clc
    ret
rmdwoffs:
;	test	[Disassemble32Bit],1
;	jz	rtnDirectlyToDispatcherWithCY
    stosd				; dword offs
    clc
    ret
rmszimm:
    mov	eax,[ebx+asmop.addrx]	; check size
    cmp	byte [ebx+asmop.asize],WORDSIZE
    jae	rmszimmw
    stosb
    clc
    ret
rmszimmw:
    jne	rmszimmd
    stosw
    ret
rmszimmd:
    cmp	byte [ebx+asmop.asize],DWORDSIZE
    jne	rtnDirectlyToDispatcherWithCY
;	test	[Disassemble32Bit],1
;	jz	rtnDirectlyToDispatcherWithCY
    stosd
    ret

shiftopsize:
        cmp     byte [arg1 + asmop.asize],DWORDSIZE
        jz      sozdword
        cmp     byte [arg1 + asmop.asize],WORDSIZE
        jnz     sozret
        or      word [PrefixBitmapWord],AS_OPSIZESET
sozret:
        ret
sozdword:
        or      word [PrefixBitmapWord],AS_OPSIZE
        ret
;
; no operands. In this case, the opcode length is 1, so we go through this
;	weird locution to rep mov a single byte out of the structure and
;	into [di], wherever that points. Apparently it points into an opcode
;	buffer somewhere. If the opcode length is greater than one, what
;	happens?
;
aop0	:
    call	noarg1			;see if arg1 + asmop.mode=0, whatever that means
    push	esi			;only return here if it IS 0
    movzx	ecx,byte [esi+opcode.oclength]	;so get the length of the opcode
    lea	esi,[esi+opcode.compare]	;find the opcode in the structure
    rep	movsb			;copy from structure to buffer
    pop	esi			;restore pointer to structure
    clc				;say no problem
    ret
;
; word reg, bits 0-2 of opcode = reg num
;
aop1	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkregw
    mov	al,byte  [esi+opcode.compare]
    or	al,byte [arg1 + asmop.areg1]
    stosb
    clc
    ret
;
; word acc,reg... reg = bits 0-2 of opcode
;
aop2	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    call	chkaxw
    mov	ebx, arg2
    call	chkregw
    mov	al,byte  [esi+opcode.compare]
    or	al,[arg2 + asmop.areg1]
    stosb
    ret
;
; one arg, seg goes in b3-4 of opcode
;
aop3	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkseg
    mov	ah,byte  [esi+opcode.compare]
    mov	al,byte [arg1 + asmop.areg1]
    cmp	ah,8
    jae	aop3ext
    cmp	al,4
    jae	aop3errx
aop3c:
    shl	al,3
    or	al,ah
    stosb
    ret
aop3ext:
    sub	al,4
    jnc	aop3c
aop3errx:
    stc
    ret
    
;
; either combo of a reg & rm... bit 1 of opcode set if reg is dest
; bit 0 set if size = word
;
aop4	:
    call	noarg3
    call	intsize
    sub	al,al
    mov	ebx, arg1
    call	sizeb0
     cmp	byte [arg1 + asmop.mode],AM_REG
    jne	aop4rs
    or	al,2
    mov	ebx, arg2
    mov	ah,byte [arg1 + asmop.areg1]
    jmp	aop4j

aop4rs:
     cmp	byte [arg2 + asmop.mode],AM_REG
    jne	aop4err
    mov	ebx, arg1
    mov	ah,[arg2 + asmop.areg1]
aop4j:
    or	al,byte  [esi+opcode.compare]
    stosb
    mov	al,ah
    call	asmrm
    ret
aop4err:
    stc
    ret
;
; use only rm, bit 0 = size (exception : jmp/call)
;
aop5	:
    call	noarg2
    call	intsize
        test    byte [farptr],1
        jnz     aop5err
    mov	ebx, arg1
    mov	ax,[esi+opcode.compare]
    test	al,1
    jz	aop5cansize
        cmp     byte [ebx+asmop.asize],DWORDSIZE
        je      aop5unsized
    cmp	byte [ebx+asmop.asize],WORDSIZE
    je	aop5unsized
    cmp	byte [ebx+asmop.asize],NOSIZE
    je	aop5unsized
aop5err:
    stc
    ret
aop5cansize:
    call	sizeb0
aop5unsized:
    stosb
    xchg	al,ah
    and	al,38h
    mov	ebx, arg1
    call	asmrm2
    ret
;
; rm,count or rm,cl (shifts) bit 0 = size
; bit 1 set if size = 1, bit 4 set if size = cl, otherwise follow rm with
; a count byte
;
aop6	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    mov	ax,[esi+opcode.compare]
    call	sizeb0
    cmp	byte [arg2 + asmop.mode],AM_REG
    je	aop6cl
    cmp	byte [arg2 + asmop.mode],AM_IMM
    je	aop6imm
aop6b:
    stc
    ret
aop6cl:
    cmp	byte [arg2 + asmop.asize],BYTESIZE
    jne	aop6b
    cmp	byte [arg2 + asmop.areg1],isECX
    jne	aop6b
    or	al,12h
    stosb
        call    shiftopsize
    jmp	aop6rm
aop6imm:
    cmp	dword [arg2 + asmop.addrx],1
    je	aop6shift1
    stosb
    xchg	al,ah
    call	asmrm2
    mov	al,byte  [arg2 + asmop.addrx]
    stosb
    ret
aop6shift1:
    or	al,10h
    stosb
aop6rm:
    xchg	al,ah
    call	asmrm2
    ret
;
; unordered version of aop4.  (any combo of reg and RM)
; This is XCHG instructions,
; this is unordered, so, it doesn't
; need to know which comes first
;
; bit 0 = size
;
aop7	:
    call	noarg3
    call	intsize
    sub	al,al
    mov	ebx, arg1
    call	sizeb0
     cmp	byte [arg1 + asmop.mode],AM_REG
    jne	aop7rs
    mov	ebx, arg2
    mov	ah,byte [arg1 + asmop.areg1]
    jmp	aop7j

aop7rs:
     cmp	byte [arg2 + asmop.mode],AM_REG
    jne	aop7err
    mov	ebx, arg1
    mov	ah,[arg2 + asmop.areg1]
aop7j:
    or	al,byte  [esi+opcode.compare]
    stosb
    mov	al,ah
    call	asmrm
    ret
aop7err:
    stc
    ret
;
; word regrm, reg = dest.
;
aop8	:
        call    noarg3
        call    intsize
    cmp	byte [arg1 + asmop.asize],BYTESIZE
        je      aop8errx
    mov	ebx, arg1
        call    chkregw
    mov	al,byte  [esi+opcode.compare]
    stosb
    mov	al,[ebx+asmop.areg1]
    mov	ebx, arg2
        call    asmrm
        clc
    ret
aop8errx:
        stc
        ret
;
; interrupts (imm byte)
;
aop9	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkimm
    cmp	byte  [arg1 + asmop.addrx],3
    je	aop9i3
        cmp     byte  [arg1 + asmop.addrx],1
        je      aop9i1
    mov	al,byte  [esi+opcode.compare]
    or	al,1
    stosb
    mov	al,byte  [ebx+asmop.addrx]
    stosb
    clc
    ret
aop9i3:
    mov	al,0cch
    stosb
    ret
aop9i1:
        mov     al,0f1h
        stosb
        ret
;
; short relative branches
;
aop10	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkimm
    mov	eax,[lastofs]
    sub	eax,[ebx+asmop.addrx]
    neg	eax
    sub	eax,2
    cmp	eax,-128
    jl	aop10errx
    cmp	eax,127
    jle	aop10ok
aop10errx:
    stc
    ret
aop10ok:
    push	eax
    mov	al,byte  [esi+opcode.compare]
    stosb
    pop	eax
    stosb
    clc
    ret
;
; RM, IMMEDIATE
; bit 0 = size
;
aop11	:
    call	noarg3
    call	intsize
    mov	ebx, arg2
    call	chkimm
    mov	ebx, arg1
    mov	ax,[esi+opcode.compare]
    call	sizeb0
    stosb
    xchg	al,ah
    call	asmrm2
    mov	ebx, arg2
    call	rmszimm
    ret
;
; ACC,immediate
; bit 0 = size
;
aop12	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    call	chkax
    mov	ebx, arg2
    call	chkimm
    mov	al,byte  [esi+opcode.compare]
    call	sizeb0
    stosb
    call	rmszimm
    ret
;
; mem,acc
; bit 0 = size
;
aop13	:
    call	noarg3
    call	intsize
    mov	ebx, arg2
    call	chkax
    mov	ebx, arg1
    call	chkmem
    mov	al,byte  [esi+opcode.compare]
    call	sizeb0
    stosb
    call	rmszoffs
    ret
;
; sign-extended RM/IMM
; b1 = 0, treat as normal RM/IMM (aop11)
; else b01=11 means sign-extend byte to word
;
aop14	:
    call	noarg3
    call	intsize
    mov	ebx, arg2
    call	chkimm
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	aop11
    mov	eax,[ebx +asmop.addrx]
    cmp	eax,-128
    jl	aop11
    cmp	eax,127
    jge	aop11
;
; get here for signed extension
;
    mov	ebx, arg1
    mov	ax,[esi+opcode.compare]
    or	al,3
    stosb
    xchg	al,ah
    call	asmrm2
    mov	al,byte  [arg2 + asmop.addrx]
    stosb
    ret
;
; acc,imm
; b3 of opcode = size
; 
aop15	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
        call    chkreg
    mov	al,byte  [esi+opcode.compare]
        or      al,[ebx+asmop.areg1] 
    call	sizeb3
    stosb
    mov	ebx, arg2
    call	chkimm
    call	rmszimm
    ret
;
; seg,regrm or regrm,seg
; b1 set if seg is dest
;
aop16	:
    call	noarg3
        call    intsize
    mov	ebx, arg1
    cmp	byte [ebx+asmop.mode],AM_SEG
    jne	aop16seg2
    mov	al,byte  [esi+opcode.compare]
    or	al,2
    mov	ah,[ebx+asmop.areg1]
    mov	ebx, arg2
    cmp	byte [ebx+asmop.mode],AM_REG		;is this flag set?
        jnz     aop16nw1
        cmp     byte [ebx + asmop.asize],DWORDSIZE     ; auto prefixing on
        jne     aop16nw1                ; ins & outs is disabled
        or      word [PrefixBitmapWord],AS_OPSIZE
aop16nw1:
    jmp	aop16rm
aop16seg2:
    mov	ebx, arg2
    call	chkseg
    mov	al,byte  [esi+opcode.compare]
    mov	ah,[ebx+asmop.areg1]
    mov	ebx, arg1
    cmp	byte [ebx+asmop.mode],AM_REG		;is this flag set?
        jnz     aop16nw2
        cmp     byte [ebx + asmop.asize],DWORDSIZE     ; auto prefixing on
        jne     aop16nw2                ; ins & outs is disabled
        or      word [PrefixBitmapWord],AS_OPSIZE
aop16nw2:
aop16rm:
    stosb
    xchg	al,ah
    call	asmrm
    ret
;
; returns which pop the stack
;
aop17	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkimm
    mov	al,byte  [esi+opcode.compare]
    stosb
    mov	ax,word  [ebx+asmop.addrx]
    stosw
    clc
    ret
;
; far branch or call
;
aop18	:
    call	noarg2
    call	intsize
    cmp	byte [arg1 + asmop.mode],AM_SEGOFFS
    jne	aop18bad
    mov	al,byte  [esi+opcode.compare]
    stosb
    mov	eax,dword  [arg1 + asmop.addrx2]
;        test    [optass32],255
;        jnz     fbcw
;	stosw
;        jmp     fbcjoin
;fbcw:
        stosd
fbcjoin:
    mov	ax,word  [arg1 + asmop.addrx]
    stosw
    clc
    ret
aop18bad:
    stc
    ret
;
; ESC instruction
; imm,rm... imm is six bits and fills the low three bits of the
; opcode and the reg field
;
aop19	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    call	chkimm
    mov	al,byte  [ebx+asmop.addrx]
    shr	al,3
    and	al,7
    or	al,byte  [esi+opcode.compare]
    stosb
    mov	al,byte  [ebx+asmop.addrx]
    and	al,7
    mov	ebx, arg2
    call	asmrm
    ret
;
; long relative branch
;  (the parser fills in the 0f starter)
;
aop20	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkimm
    mov	eax,[lastofs]
    sub	eax,[ebx+asmop.addrx]
    neg	eax
    sub	eax,3
;        test    [optass32],255
;        jz      lrbnot32
        sub     eax,2
;lrbnot32:
    cmp	byte  [esi+opcode.compare],0e0h
    jae	aop20got
    dec	eax
aop20got:
    jmp aop20ok
;        test    [optass32],255
;        jnz     aop20ok
;        test    eax,0ffff0000h
;        jz      aop20ok
;        cmp     eax,0ffff0000h
;        jnc     aop20ok
aop20errx:
    stc
    ret
aop20ok:
    push	eax
    mov	al,byte  [esi+opcode.compare]
    stosb
    pop	eax
;        test    [optass32],255
;        jnz     lrbw
;	stosw
;        jmp     lrbjoin
lrbw:
        stosd
lrbjoin:
    clc
    ret
;
;	acc,dx (in instructions)
;	bit 0 = size
;
aop21	:
    call	noarg3
    call	intsize
    mov	ebx, arg2
    call	chkdx
    mov	ebx, arg1
    call	chkax
    mov	al,byte  [esi+opcode.compare]
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	aop21nw
    or	al,1
aop21nw:
    cmp	byte [ebx +asmop.asize],DWORDSIZE	; auto prefixing on
    jne	aop21nw2		; ins & outs is disabled
        or      word [PrefixBitmapWord],AS_OPSIZE
aop21nw2:
    stosb
    clc
    ret
;
; dx,acc (out_)
; bit 0 = size
;
aop22	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    call	chkdx
    mov	ebx, arg2
    call	chkax
    mov	al,byte  [esi+opcode.compare]
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	aop22nw
    or	al,1
aop22nw:
    cmp	byte [ebx +asmop.asize],DWORDSIZE	; auto prefixing on
    jne	aop22nw2		; ins & outs is disabled
        or      word [PrefixBitmapWord],AS_OPSIZE
aop22nw2:
    stosb
    clc
    ret
;
; port,acc or acc,port
; b0 =size, b1 = 1 if port is dest
;
aop23	:
    call	noarg3
    call	intsize
    test	byte  [esi+opcode.compare],2
    jz	aop23in
    mov	ebx, arg2
    call	chkax
    mov	ebx, arg1
    call	chkimm
    jmp	aop23j
aop23in:
    mov	ebx, arg1
    call	chkax
    mov	ebx, arg2
    call	chkimm
aop23j:
    mov	al,byte  [esi+opcode.compare]
    cmp	byte [ebx +asmop.asize],WORDSIZE
    jb	aop23b
           or	al,1
aop23b:
    clc
    stosb
    mov	al,byte  [ebx +asmop.addrx]
    stosb
    ret
;
; acc,mem
; bit 0 = size
;
aop24	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    call	chkax
    mov	ebx, arg2
    call	chkmem
    mov	al,byte  [esi+opcode.compare]
    call	sizeb0
    stosb
    call	rmszoffs
    ret
;
; immediate byte or word
; this is push imm, bit 1 set for byte
; we were about due for a departure from the standard...
; anyway the op is sign-extended if it is byte size
;
aop25	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkimm
    mov	al,byte  [esi+opcode.compare]
    mov	ebx,[ebx+asmop.addrx]
;        test    [optass32],255
;        jnz     aop25bsft
;        test    ebx,0ffff0000h
;        jnz     aop25w
;        cmp     bx,-128
;        jl      aop25w
;        cmp     bx,127
;        jg      aop25w
;        jmp     aop25bs
;aop25bsft:
    cmp	ebx,-128
    jl	aop25w
    cmp	ebx,127
    jg	aop25w
;
; byte size
;
aop25bs:
    or	al,2
    stosb
    mov	al,bl
    stosb
    ret
aop25w:
    stosb
        test   word [PrefixBitmapWord],AS_OPSIZETEST
        jnz     aop25dw
;        test    [optass32],255
;        jnz     aop25dw
;        cmp     ebx,0ffffh
;        ja      aop25dw
;	mov	ax,bx
;	stosw
;	clc
;	ret
aop25dw:
    or	word [PrefixBitmapWord],AS_OPSIZE
    mov	eax,ebx
    stosd
    clc
    ret
;
; enter command, we have a word then a byte
;
aop26	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    call	chkimm
    mov	byte [ebx+asmop.asize],WORDSIZE
    mov	ebx, arg2
    call	chkimm
    mov	byte [ebx+asmop.asize],BYTESIZE
    mov	al,byte  [esi+opcode.compare]
    stosb
    mov	ax,word  [arg1 + asmop.addrx]
    stosw
    mov	al,byte  [arg2 + asmop.addrx]
    stosb
    ret
;
; stringu/w/d, pushaw,pushfw, etc
; explicit byte sizing handled elsewhere (aop0)
;
aop27	:
    call	noarg1
    call	intsize
    mov	al,byte  [esi+opcode.compare]
    cmp	byte [lastbyte],'w'
    je	aop27w
    cmp	byte [lastbyte],'d'
    je	aop27dw
;
; get here if no size specced.
    mov	al,byte  [esi+opcode.compare]
    and	al,~1
    cmp	al,60h		; default for push/pop is word
        je      aop27w2
    cmp	al,9ch
        je      aop27w2
    stosb		; else default is byte
    clc
    ret
aop27dw:
    or	word [PrefixBitmapWord],AS_OPSIZE	; come here if specified dword
aop27w:
        or      word [PrefixBitmapWord],AS_OPSIZESET
aop27w2:
    mov	al,byte  [esi+opcode.compare] ; or here if specified word
    stosb
    clc
    ret
;
; rm,reg (test instruction)
; bit 0 = size
; 
aop28	:
    call	noarg3
    call	intsize
    mov	ebx, arg2
    call	chkreg
    mov	al,byte  [esi+opcode.compare]
    call	sizeb0
    stosb
    mov	al,[arg2 + asmop.areg1]
    mov	ebx, arg1
    call	asmrm
    ret
;
; rm, size don't care
;
aop29	:
    call	noarg2
    mov	ax,[esi+opcode.compare]
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmrm2
    ret
;
; RM, shift
; bit 0 & 1 of opcode set if uses CL
; bit 0 & 4 set if uses uses 1
; else nothing set
;
aop30	:
    call	noarg3
    call	intsize
    mov	ax,word  [esi+opcode.compare]
    and	al,0ech	; get rid of extraneous bits
    mov	ebx, arg2
    cmp	byte [ebx+asmop.areg1],isEBX
    je	aop30cl
    call	chkimm
    cmp	dword [ebx+asmop.addrx],1
    jne	aop30xx
    or	al,11h
    stosb
    xchg	al,ah
    call	asmrm2
    ret
aop30xx:
    or	al,3
    stosb
    xchg	al,ah
    call	asmrm2
    mov	al,byte  [arg2 + asmop.addrx]
    stosb
    ret
aop30cl:
    cmp	byte [ebx+asmop.asize],BYTESIZE
    jne	aop30bad
    cmp	byte [ebx+asmop.mode],AM_REG
    jne	aop30bad
    stosb
        call    shiftopsize
    xchg	al,ah
    call	asmrm2
    ret

aop30bad:
    stc
    ret
;
; reg,rm,imm or reg,imm (imul)
; bit 1 = set if immed = signed byte
;
aop31	:
    cmp	byte [arg3 + asmop.mode],AM_NONE
    jne	aop31three
    mov	ebx, arg1
    call	chkreg
    mov	al,byte  [esi+opcode.compare]
    mov	ebx, arg2
    call	chkimm
    call	aop31sb
    stosb
    push	eax
    mov	al,byte [arg1 + asmop.areg1]
    mov	ah,al
    shl	al,3
    or	al,ah
    or	al,0c0h
    stosb
    jmp	aop31sz
aop31three:
    mov	ebx, arg1
    call	chkreg
    mov	al,byte  [esi+opcode.compare]
    mov	ebx, arg3
    call	chkimm
    call	aop31sb
    stosb
    push	eax
    push	ebx
    mov	al,byte [arg1 + asmop.areg1]
    mov	ebx, arg2
    call	asmrm
    pop	ebx
aop31sz:
    pop	eax
    test	al,2
    mov	eax,dword  [ebx +asmop.addrx]
    jnz	aop31sb1
    test	word [PrefixBitmapWord],AS_OPSIZE
    jz	aop31szw
    stosd
    clc
    ret
aop31szw:
    stosw
    clc
    ret
aop31sb1:
    stosb
    clc
    ret
aop31sb	:
    mov	ecx,[ebx+asmop.addrx]
    test	ecx,~7fh
    jnz	aop31sbx
    cmp	ecx,-80h
    jl	aop31sbx
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	aop31sbx
    or	al,2
aop31sbx:
    ret
;
; move to/from a special register
; bit 1 = set if spc reg is the dest
; bit 0 & 2 define the special reg
;
aop32	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    mov	ecx, arg2
    mov	ax,[esi+opcode.compare]
    cmp	byte [arg1 + asmop.asize],DWORDSIZE
    jne	aop32bad
    cmp	byte [arg1 + asmop.mode],AM_REG
    je	aop32crsource
    or	al,2
    xchg	ebx,ecx
aop32crsource:       		
    sub	dl,dl
    cmp	byte [ecx+asmop.mode],AM_CR
    je	aop32c
    mov	dl,1
    cmp	byte [ecx+asmop.mode],AM_DR
    je	aop32c
    mov	dl,4
    cmp	byte [ecx+asmop.mode],AM_TR
    jne	aop32bad
aop32c:
    or	al,dl
    stosb
    
    call	chkreg
    mov	al,[ecx+asmop.areg1]
    shl	al,3
    or	al,[ebx +asmop.areg1]
    or	al,ah		; actually is a MOD/RM which is restricted
                ; to regs
    stosb
;        test    [optass32],255
;        jnz     aop32d
;	and	word [PrefixBitmapWord],~AS_OPSIZE	; don't need opsize for this
;					; and there will be no clash
;					; if we got this far
;        or      word [PrefixBitmapWord],AS_OPSIZESET        ; don't need opsize for this
;	ret
;aop32d:
        or      word [PrefixBitmapWord],AS_OPSIZE        ; don't need opsize for this
        ret
aop32bad:
    stc
    ret
;
; rm,reg,count (shld/shrd)
; bit 0 = set if using CL for count
;
;
aop33	:
    mov	ebx, arg2
    call	chkreg
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	aop33bad
    mov	ah,[ebx+asmop.areg1]
    mov	al,byte  [esi+opcode.compare]
    mov	ebx, arg3
    cmp	byte [ebx+asmop.areg1],isECX
    je	aop33cl
    call	chkimm
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmrm
    mov	al,byte  [arg3 + asmop.addrx]
    stosb
    ret
aop33cl:
    cmp	byte [ebx+asmop.mode],AM_REG
    jne	aop33bad
    or	al,1h
    stosb
        call    shiftopsize
    xchg	al,ah
    mov	ebx, arg1
    call	asmrm
    ret

aop33bad:
    stc
    ret
;
; push & pop rm
;
aop34	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    cmp	byte [ebx+asmop.asize],BYTESIZE
    je	aop34bad
    mov	ax,[esi+opcode.compare]
    stosb
    xchg	al,ah
    call	asmrm2
    ret
aop34bad:
    stc
    ret
;
; floating R/M
; bit two of opcode set if size is qword
; bit 3 of mod/rm set if last ch = 'p'
;
aop35	:
        
         call	noarg2
    mov	ax,[esi+opcode.compare]
    and	ax,010d8h
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    je	aop35fin
    cmp	byte [arg1 + asmop.asize],QWORDSIZE
    jne	aop35fin
    or	al,4
aop35fin:
    cmp	byte [lastbyte],'p'
    jne	aop35fin2
    or	ah,8
aop35fin2:
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmfrm
    ret
;
; fmathp
; sti),st(0) or nothing
;
aop36	:
    cmp	byte [arg3 + asmop.mode],AM_NONE
    jne	aop36errx
    mov	eax,100h
    cmp	byte [arg2 + asmop.mode],AM_NONE
    jne	aop362arg
        cmp     byte [arg1 + asmop.mode],AM_FPREG
        jnz     aop36errx
        mov     ah,byte [arg1 + asmop.areg1]
    jmp	aop36fin
aop362arg:
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    jne	aop36errx
    cmp	byte [arg2 + asmop.mode],AM_FPREG
    jne	aop36errx
    cmp	byte [arg2 + asmop.areg1],isEAX
    jne	aop36errx
    mov	ah,byte [arg1 + asmop.areg1]
aop36fin:
    or	ax,[esi+opcode.compare]
    stosw
    ret
aop36errx:
    stc
    ret
;
; fmath
; st(i),st(0) ; st(0), st(i), mem
; bit two of opcode set for i dest or qword mem
; bit 3 of mod/rm gets flipped if reg & al &6 & bit 5 of mod/rm set
; 
;
aop37	:
    cmp	byte [arg3 + asmop.mode],AM_NONE
    jne	near aop37errx
    mov	ax,[esi+opcode.compare]
    and	al,~4
    cmp	byte [arg2 + asmop.mode], AM_NONE
    je	aop37mem
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    jne	aop37mem
    cmp	byte [arg2 + asmop.mode],AM_FPREG
    jne	aop37errx
    mov	bl,[arg2 + asmop.areg1]
    mov	bh,byte [arg1 + asmop.areg1]
    cmp	bh,0
    je	aop37isrc
    cmp	bl,0
    jne	aop37errx
    xchg	bl,bh
    or	al,4
aop37isrc:
    test	al,6
    jz	aop371
    test	ah,20h
    jz	aop371
    xor	ah,8
aop371:
    or	ah,bl
    or	ah,0c0h
    stosw
    ret
aop37mem:
        cmp     byte [arg1 + asmop.mode],AM_FPREG
        jne     aop37memr
        mov     ah,byte [arg1 + asmop.areg1]
        or      ah,0c0h
        mov     al,4
        or      ax,[esi+opcode.compare]
        stosw
        ret
aop37memr:
        cmp     byte [arg2 + asmop.mode],AM_NONE
    jne	aop37errx
    cmp	byte [arg1 + asmop.asize],QWORDSIZE
    jne	aop37memnq
    or	al,4
aop37memnq:
    stosb
    xchg	al,ah
    mov	ebx, [arg1]
    call	asmfrm
    ret
aop37errx:
    stc
    ret
;
; far RM
;
aop38	:
    call	noarg2
        test    byte [farptr],1
        jz      aop38errx
    cmp	byte [arg1 + asmop.mode],AM_REG
    je	aop38errx
    mov	ax,[esi+opcode.compare]
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmrm2
    ret
aop38errx:
    stc
    ret
;
; word regrm with reg  source
; bug: lets arpl [ebx],eax through
;
aop39	:
    cmp	byte [arg2 + asmop.asize],BYTESIZE
    jne	aop40
    stc
    ret
;
; regrm with reg source
; bit 0 = size
aop40	:
    call	noarg3
    call	intsize
    mov	ebx, arg2
    call	chkreg
    mov	al,byte  [esi+opcode.compare]
    call	sizeb0
    stosb
    mov	al,[arg2 + asmop.areg1]
    mov	ebx, arg1
    call	asmrm
    ret
;
; rm,immediate
;
aop41	:
    call	noarg3
    call	intsize
    mov	ebx, arg2
    call	chkimm
    cmp	byte [arg1 + asmop.asize],BYTESIZE
    je	aop41errx
    mov	ax,[esi+opcode.compare]
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmrm2
    mov	al,byte  [arg2 + asmop.addrx]
    stosb
    ret
aop41errx:
    stc
    ret
;
; regrm with reg dest & forced strictness (MOVZX & MOVSX)
; bit 0 of opcode set if size is word
;
aop42	:
    call	noarg3
    call	intsize
    mov	ebx, arg1
    call	chkreg
    cmp	byte [arg1 + asmop.asize],BYTESIZE
    je	aop42errx
    cmp byte [arg2 + asmop.asize],NOSIZE
    je	aop42errx
    mov	ebx, arg2
    mov	ax,[esi+opcode.compare]
    call	sizeb0
    stosb
        mov     al,byte [arg1 + asmop.areg1]
        shl     al,3
        or      al,ah
    call	asmrm2
    cmp	byte [arg1 + asmop.asize],DWORDSIZE	; another func for which
                    ; auto extend to dword doesn't work
    jne	aop42x
    or	word [PrefixBitmapWord],AS_OPSIZE
aop42x:
        or      word [PrefixBitmapWord],AS_OPSIZESET
    ret
aop42errx:
    stc
    ret
;
; ~needed in assembler, same as no args
; 
aop43	:
;        test    [optass32],255
;        jz      aop43off
        or      word [PrefixBitmapWord],AS_OPSIZESET
;aop43off:
    jmp	aop0
;
; unused
;
aop44	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkreg
    cmp	byte [ebx+asmop.asize],DWORDSIZE
    jne	aop44errx
    or	word [PrefixBitmapWord],AS_OPSIZE
    mov	al,byte  [esi+opcode.compare]
    or	al,[ebx+asmop.areg1]
    stosb
    ret
aop44errx:
    stc
    ret
;
; any regrm with reg source
; same as aop40
;
aop45	:
    jmp	aop40
;
; rm , size don't care, but no regs
;
aop46	:
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    je	aop46err
    cmp	byte [arg1 + asmop.mode],AM_REG
    jne	aop29
aop46err:
    stc
    ret
;
; ax reg only
;
aop47	:
    call	noarg2
    mov	ebx, arg1
    call	chkaxw
    mov	ax,[esi+opcode.compare]
    stosw
    clc
    ret
;
; bswap, 32-bit reg to bits 0-3 of opcode
;
aop48	:
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkreg
    cmp	byte [ebx+asmop.asize],DWORDSIZE
    jne	aop48errx
    or	word [PrefixBitmapWord],AS_OPSIZE
    mov	al,byte  [esi+opcode.compare]
    or	al,byte [ebx+asmop.areg1]
    stosb
    ret
aop48errx:
    stc
    ret
;
; fst
; same as next but no tbyte
;
aop49	:
    cmp	byte [arg1 + asmop.asize],TBYTESIZE
    jb	aop50
    stc
    ret
;
; fld/fstp
; freg
; dword,qword,tybe
; bit 1 of opcode set for tbyte
; bit 2 of opcode set for qword or store
; bit 5 of modrm set if tbyte
; bit 3 of modrm gets cleared if ~tbyte && bit 4 set
;
aop50	:
    call	noarg2
    mov	ax,[esi+opcode.compare]
    and	ax,~0e006h
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    je	aop50reg
    cmp	byte [arg1 + asmop.asize],DWORDSIZE
    jb	aop50errx
aop502:
    cmp	byte [arg1 + asmop.asize],TBYTESIZE
    jne	aop503
    or	ax,2802h
    jmp	aop505

aop503:
    
    cmp	byte [arg1 + asmop.asize],QWORDSIZE
    jne	aop504
    or	al,4
aop504:
    test	ah,10h
    jnz	aop505
    and	ah,~8
aop505:
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmfrm
    ret
aop50reg:
    test	ah,10h
    jnz	aop506
    and	ah,~8
    jmp	aop507
aop506:
    or	al,4
aop507:
    or	ah,0c0h
    or	ah,byte [arg1 + asmop.areg1]
    stosw
    ret
aop50errx:
    stc
    ret
;
;
; fbld/fbstp
; tbyte  mem
;
aop51	:
    call	noarg2
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    je	aop51errx
    cmp	byte [arg1 + asmop.asize],NOSIZE
    je	aop51c
    cmp	byte [arg1 + asmop.asize],TBYTESIZE
    jne	aop51errx
aop51c:
    mov	ax,[esi+opcode.compare]
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmfrm
    ret
aop51errx:
    stc
    ret
;
; fild/fistp
; word,dword,qword mem
; bit 2 of opcode set if word or qword
; bit 3 of modrm gets cleared if ~qword & bit 4 is set
; bit 5 or modrm set if qword
;
aop52	:
    call	noarg2
    mov	ax,[esi+opcode.compare]
    and	ax,~2004h
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    je	aop52errx
    cmp	byte [arg1 + asmop.asize],DWORDSIZE
    je	aop522
           or	al,4
aop522:
    cmp	byte [arg1 + asmop.asize],QWORDSIZE
    ja	aop52errx
    jne	aop523
    or	ah,20h
    jmp	aop524
aop523:
    test	ah,10h
    jnz	aop524
    and	ah, ~8
aop524:	
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmfrm
    ret
aop52errx:
    stc
    ret
;
; fist
; same as above but no qword mode
;
aop53	:
    cmp	byte [arg1 + asmop.asize],QWORDSIZE
    jb	aop52
    stc
    ret
;
; freg
; reg put in mod/rm byte
;
aop54	:
    call	noarg2
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    jne	aop54errx
    mov	ax,[esi+opcode.compare]
    or	ah,byte [arg1 + asmop.areg1]
    stosw
    ret
aop54errx:
    stc
    ret
;
; same as above, deault to reg 1 if no args
aop55	:
    cmp	byte [arg3 + asmop.mode],AM_NONE
    jne	aop55errx
    cmp	byte [arg2 + asmop.mode],AM_NONE
    jne	aop55errx
    call	noarg2
    mov	ax,[esi+opcode.compare]
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    jne	aop55chknone
    or	ah,byte [arg1 + asmop.areg1]
    stosw
    ret
aop55chknone:
    cmp	byte [arg1 + asmop.mode],AM_NONE
    jne  	aop55errx
    or	ah,1
    stosw
    ret	
aop55errx:
    stc
    ret
;
; fimath
; word or dword arg
; bit two gets set if word
;
aop56	:
    call	noarg2
    call	intsize
    cmp	byte [arg1 + asmop.asize],BYTESIZE
    je	aop56errx
    cmp	byte [arg1 + asmop.mode],AM_FPREG
    je	aop56errx
    mov	ax,[esi+opcode.compare]
    cmp	byte [arg1 + asmop.asize],WORDSIZE
    jne	aop56c
    or	al,4
aop56c:
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmfrm
    ret
aop56errx:
    ret


;
; addrsize prefix on one-byte
;
aop57	:
;        test    [optass32],255
;        jnz     aop57off
;	or	word [PrefixBitmapWord],AS_ADDRSIZE
;	jmp	aop0
;aop57off:
        and     word [PrefixBitmapWord],~AS_ADDRSIZE
        or      word [PrefixBitmapWord],AS_ADDRSIZESET 
        jmp     aop0
;
; opsize prefix on one-byte
;
aop58	:
;        test    [optass32],255
;        jnz     aop58off
;	or	word [PrefixBitmapWord],AS_OPSIZE
;aop58off:
    jmp	aop0
;
; MMX standard
;              
aop59   :
    call	noarg3
    call	intsize
        mov     ebx, arg1
        call    chkmmx
    mov	al,byte  [esi+opcode.compare]
    stosb
        mov     al,byte [arg1 + asmop.areg1]
        mov     ebx, arg2
        call    asmrmmmx
    ret
;
; MMX MOVD LD
;
aop60   :
    call	noarg3
    call	intsize
        mov     ebx, arg1
        call    chkmmx
    mov	al,byte  [esi+opcode.compare]
        and     word [PrefixBitmapWord],~AS_OPSIZE
    stosb
        mov     al,byte [arg1 + asmop.areg1]
        mov     ebx, arg2
    call	asmrm
    ret
;
; MMX MOVD ST
;
aop61   :
    call	noarg3
    call	intsize
        mov     ebx, arg2
        call    chkmmx
        and     word [PrefixBitmapWord],~AS_OPSIZE
    mov	al,byte  [esi+opcode.compare]
    stosb
        mov     al,[arg2 + asmop.areg1]
        mov     ebx, arg1
    call	asmrm
    ret
;
; MMX MOVQ LD
;
aop62   :
    call	noarg3
    call	intsize
        mov     ebx, arg1
        call    chkmmx
    mov	al,byte  [esi+opcode.compare]
    stosb
        mov     al,byte [arg1 + asmop.areg1]
        mov     ebx, arg2
        call    asmrmmmx
    ret
;
; MMX MOVQ ST
;
aop63   :
    call	noarg3
    call	intsize
        mov     ebx, arg2
        call    chkmmx
    mov	al,byte  [esi+opcode.compare]
    stosb
        mov     al,[arg2 + asmop.areg1]
        mov     ebx, arg1
        call    asmrmmmx
    ret
;         
; MMX SHIFT
;
aop64   :
    call	noarg3
        mov     ebx, arg2
        call    chkimm
        mov     ebx, arg1
        call    chkmmx
        mov     ax,word  [esi+opcode.compare]
        or      ah,0c0h
        or      ah,[ebx+asmop.areg1]
        stosw
        mov     al,byte  [arg2 + asmop.addrx]
        stosb
    ret

;
; loop
;
aop65   :
    call	noarg2
    call	intsize
    mov	ebx, arg1
    call	chkimm
        sub     edx,edx
        mov     al,[lastbyte]
        cmp     al,'d'
        jz      aop65dw
        cmp     al,'w'
        jz      aop65w
        mov     ax,word  [EnteredMnemonic]
        cmp     ax,"je"
        jz      aop65dw
        jmp     aop65okc
aop65w:
        or      word [PrefixBitmapWord],AS_ADDRSIZESET
        jmp     short aop65join
aop65dw:
        or      word [PrefixBitmapWord],AS_ADDRSIZE
        inc     edx
aop65join:
;        test    [optass32],255
;        jz      aop65okc
        neg     edx
        inc     edx
aop65okc:
    mov	eax,[lastofs]
    sub	eax,[ebx+asmop.addrx]
    neg	eax
        sub     eax,2
        sub     eax,edx
aop65lrbnot32:
        cmp     eax,-128
        jl      aop65as32
        cmp     eax,128
        jle     aop65ok
aop65as32:
        stc
        ret
aop65ok:
    push	eax
    mov	al,byte  [esi+opcode.compare]
    stosb
    pop	eax
        test    word [PrefixBitmapWord],AS_OPSIZETEST
        jnz     aop65lrbw
        stosb
        jmp     aop65lrbjoin
aop65lrbw:
        stosd
aop65lrbjoin:
    clc
    ret
;
; CWD
;
aop66   :
        call    noarg1
;        test    [optass32],255
;        jz      aop66off
        or      word [PrefixBitmapWord],AS_OPSIZESET
aop66off:
        jmp     aop0
;
; AAM
;
aop67   :
    cmp	byte [arg2 + asmop.mode],AM_NONE
        stc
        jne     aop67err
        mov     bl,0ah
        cmp     byte [arg1 + asmop.mode],AM_NONE
        je      aop67fin
        mov     ebx, arg1
        call    chkimm
        mov     bl,byte  [arg1 + asmop.addrx]
aop67fin:
        mov     al,byte  [esi + opcode.compare]
        stosb
        mov     al,bl
        stosb
        clc
aop67err:
        ret
;
; xlat
;
aop68   :
        cmp     byte [arg1 + asmop.mode],AM_NONE
        jz      aop68to0
        call    noarg2        
        cmp     byte [arg1 + asmop.areg1],3
        jnz     aop68err
        cmp     byte [arg1 + asmop.areg2],0FFh   ; else see if second reg defined
        jnz     aop68err
        cmp     byte [arg1 + asmop.mode],AM_BASED
        jnz     aop68err
        or      word [PrefixBitmapWord],AS_ADDRSIZESET
        mov     al,byte  [esi + opcode.compare]
        stosb
        ret
aop68err:
        stc
        ret
aop68to0:
        jmp     aop0
;
; sets
;
aop69 :          
        call    noarg2
        cmp     byte [arg1 + asmop.asize],BYTESIZE  ;maybe this means the first argument
        ja      aop69err   ;can't exceed dword size
        jmp     aop29
aop69err:
        stc
        ret
aop70 :
    call	noarg2
        cmp     byte [arg1 + asmop.asize],FWORDSIZE
        jz      aop70dw
        cmp     byte [arg1 + asmop.asize],PBYTESIZE
        jnz     aop70join
aop70w:
        or      word [PrefixBitmapWord],AS_OPSIZESET
        jmp     short aop70join
aop70dw:
        or      word [PrefixBitmapWord],AS_OPSIZE
aop70join:
    mov	ax,[esi+opcode.compare]
    stosb
    xchg	al,ah
    mov	ebx, arg1
    call	asmrm2
    ret
