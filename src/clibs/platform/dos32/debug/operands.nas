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

%include	"dispatch.ase"
%include	"opcodes.asi"
%include	"operands.asi"
%include	"opcom.ase"
%include 	"mtrap.ase"

    global TabTo, ReadOverrides, DispatchOperands
    global	FormatDisassembly, FormatValue, code_address
        global  PutDword, PutWord,PutByte, source,dest, segs
        global  put2,put3,put4, strlen, GetSeg, PutWordox,PutByte

    extern x86pfx
SZ_QWORD	equ	2
SZ_TBYTE	equ	4

    segment data USE32
code_address	dd	0		;disassembly address within segment
dest2		dd	0		;temps to aid decoding
source2		dd	0
segs		dw	0		
extraoperand	times OPERANDSIZE [db 0]	;third operand
source		times OPERANDSIZE [db 0]		;second operand
dest		times OPERANDSIZE [db 0]		;first operand
nmmnemonic      times      20 [db 0]      ;mnemonic (may be modified here)
reqsPtrSizeOvride	db	0	;flag set if xxx ptr required
realopsize      db      0               ;flag set if 0x66 byte detected
unksse          db      "???SSE???",0
hasputoverride  db      0               ; flag set if an override has been displayed
    segment code USE32

;Find the length of a string in a buffer pointed to by esi, and return that
;length in AX.  This is a strange way to do this. I think I'll change it
;just a little.
;  If SI comes in pointing to a 0, we return a string length of 0

strlen	:	
    push	edi
    push	esi
    push	ecx
    mov	edi,esi
    mov	ecx,-1
    sub	al,al
    repnz	scasb
    mov	eax,ecx
    not	eax
    dec	eax
    pop	ecx
    pop	esi
    pop	edi
    ret

strcpy	:	
strcpylp:
    lodsb
    stosb
    or	al,al
    jnz	strcpylp
    ret

strcat	:	
    mov	al,[edi]
    inc	edi
    or	al,al
    jnz	strcat
    dec	edi
    jmp	strcpy

CopyExtra	:	
    push	esi
    push	edi
    mov	esi,extraoperand
    xchg	esi,edi
    mov	ecx,OPERANDSIZE
    rep	movsb
    pop	edi
    pop	esi
    ret

put2	:	
    mov	[esi],ah
    inc	esi
    mov	[esi],al
    inc	esi
    mov	byte [esi],0
    ret

put3	:	
    push	eax
    shr	eax,8
    mov	[esi],ah
    inc	esi
    pop	eax
    call	put2
    ret

put4	:	
    push	eax
    shr	eax,16
    call	put2
    pop	eax
    call	put2
    ret

setseg	:	
        mov     byte [reqsPtrSizeOvride],FALSE
    mov	byte [edi + operand.typecode],OM_SEGMENT
    mov	byte [edi + operand.thereg],al
    ret

setreg	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    mov	byte [edi + operand.typecode],OM_REG
    mov	byte [edi + operand.thereg],al
    ret

readrm	:	
    push	ecx
    sub	ecx,ecx
    mov	cl,2
    RM	esi
    mov	byte [edi + operand.thereg],al
    MODX	esi
    mov	ch,al
    cmp	ch,MOD_REG
    jnz	short notregreg
    mov	byte [edi + operand.typecode],OM_REG
    mov	byte [reqsPtrSizeOvride],FALSE
    sub	eax,eax
    pop	ecx
    ret
notregreg:
    bt	dword [edi + operand.oeflags],OMF_ADR32
    jnc	adr16
    cmp	byte [edi + operand.thereg],RM_32_SCALED
    jnz	notscaled
    inc	cl
    RM	esi+1
    mov	byte [edi + operand.thereg],al
    REG	esi+1
    mov	byte [edi + operand.scalereg],al
    MODX	esi+1
    mov	byte [edi + operand.scale],al
    cmp	byte [edi + operand.scalereg],RM_32_STACKINDEX
    jz	hassp
    bts	dword [edi + operand.oeflags],OMF_SCALED
hassp:
    cmp	byte [edi + operand.thereg],RM_32_ABSOLUTE
    jnz	basedAndscaled
    cmp	ch,MOD_NOOFS
    jnz	short basedAndscaled
    mov	byte [edi + operand.typecode],OM_ABSOLUTE
    LONG	esi+3
    mov	[edi+operand.address],eax
    sub	eax,eax
    mov	al,5
    pop	ecx
    ret
notscaled:
    
    cmp	ch,MOD_NOOFS
    jnz	basedAndscaled
    cmp	byte [edi + operand.thereg], RM_32_ABSOLUTE
    jnz	basedAndscaled
    mov	byte [edi + operand.typecode], OM_ABSOLUTE
    LONG	esi+2
    mov	[edi+operand.address],eax
    sub	eax,eax
    sub	eax,eax
    mov	al,4
    pop	ecx
    ret
adr16:
    cmp	ch,MOD_NOOFS
    jnz	basedAndscaled
    cmp	byte [edi + operand.thereg], RM_16_ABSOLUTE
    jnz	basedAndscaled
    mov	byte [edi + operand.typecode], OM_ABSOLUTE
    UINT	esi+2
    mov	[edi+operand.address],eax
    sub	eax,eax
    mov	al,2
    pop	ecx
    ret
basedAndscaled:
    mov	byte [edi + operand.typecode], OM_BASED
    cmp	ch,MOD_ADDR
    jnz	short checksigned
    bts	dword [edi + operand.oeflags], OMF_WORD_OFFSET
    push	ecx
    sub	ch,ch
    movzx	ecx,cl
    mov	eax,[esi+ecx]
    pop	ecx
    bt	dword [edi + operand.oeflags], OMF_ADR32
    jc	dwordx
    and	eax,0ffffh
    sub	cl,2
dwordx:
    mov	[edi+operand.address],eax
    add	cl,4
    jmp	short readrmdone
checksigned:
    cmp	ch, MOD_SIGNED
    jnz	short readrmdone
    bts	dword [edi + operand.oeflags],OMF_SIGNED_OFFSET
    movzx	ecx,cl
        movsx   eax,byte [esi+ecx]
    mov	[edi+operand.address],eax
    inc	cl
readrmdone:
    movzx	eax,cl
    sub	al,2
    pop	ecx
    ret

RegRM	:	
    mov	edi,[dest2]
    REG	esi
    call	setreg
    mov	edi,[source2]
    call	readrm
    ret

Immediate	:	
    push	ecx
    sub	ecx,ecx
    mov	byte [edi + operand.typecode],OM_IMMEDIATE
    bt	dword [edi + operand.oeflags],OMF_BYTE
    jnc	short inotbyte
    inc	cl
    sub	eax,eax
    mov	al,[esi]
    jmp	short i_ret
inotbyte:
    bt	dword [edi + operand.oeflags], OMF_OP32
    jnc	iword
    add	cl,4
    LONG	esi
    jmp	short i_ret
iword:
    add	cl,2
    UINT	esi
i_ret:
    mov	[edi+operand.address],eax
    mov	eax,ecx
    pop	ecx
    ret
;Find a binary 0, stick whatever is in AL there, and put a 0 after that.

MnemonicChar	:	
    push	edi
    mov	edi,nmmnemonic
mc2:
    inc	edi
    cmp	byte [edi-1],0
    jnz	mc2
    mov	[edi-1],al
    mov	byte [edi],0
    pop	edi
    ret
issse :
        test    byte [realopsize],1
        jz      notsse
        push    esi
        push    edi
        mov     esi,unksse
        mov     edi,nmmnemonic
        call    strcpy
        pop     edi
        pop     esi
notsse:
        ret

;/* op 1- word reg from bits 0 - 2 of opcode */

op1	:	
    btr	dword [edi + operand.oeflags],OMF_BYTE
    mov	al,[esi]
    B02
    call	setreg
    sub	eax,eax
    ret


;/* Op2 acc, reg bits 0-2 of opcode */
op2	:	
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
    mov	al,REG_EAX
    call	setreg
    mov	edi,ebx
    mov	al,[esi]
    and	al,7
    call	setreg
    sub	eax,eax
    ret


;/* op3 - seg from b3-5 of opcode */
op3	:	
    mov	al,[esi]
    B35
    call	setseg
    sub	eax,eax
    ret


;/* op4 - REGRM with b1 of opcode set reg is dest else source */
op4	:	
    bt	dword [esi],1
    jc	short op4nox
    xchg	ebx,edi
op4nox:
    mov	[dest2],edi
    mov	[source2],ebx
    call	RegRM
    ret

;/* op5 - use RM only */
op5	:	
    call	readrm
    ret

;/* op6 readrm for shift */
op6	:	
    call	readrm
    sub	ecx,ecx
    mov	cl,al
    mov	edi,ebx
    mov	byte [edi + operand.typecode],OM_SHIFT
    bt	dword [esi],4
    jnc	short op6cnt
    bt	dword [esi],1
        jnc     short op61a
    bts	dword [edi + operand.oeflags],OMF_CL
    jmp	short op6done
op61a:
    mov	dword [edi+operand.address],1
    jmp	short op6done
op6cnt:
    movzx	eax,byte [esi+ecx+2]
    inc	cl
    mov	[edi+operand.address],eax
op6done:
    mov	eax,ecx
    ret

;/* op 7 regrm with reg dest */
op7	:	
    mov	[dest2],edi
    mov	[source2],ebx
    call	RegRM
    ret

;/* OP8 - word regrm with reg dest */
op8	:	
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
    jmp	op7

;/* op 9 - interrupts */
op9	:	
        btr     dword [edi + operand.oeflags],OMF_ADR32
        btr     dword [edi + operand.oeflags],OMF_OP32
    mov	byte [reqsPtrSizeOvride],FALSE
    sub	eax,eax
    mov	al,3
    bt	dword [esi],0
    jnc	short op9int3
    mov	al,[esi+1]
op9int3:
    mov	[edi+operand.address],eax
    mov	byte [edi + operand.typecode],OM_INT
    sub	al,al
    ret

;/* op 10, short relative branch */
op10	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    mov	byte [edi + operand.typecode],OM_SHORTBRANCH
    movsx	eax,byte [esi+1]
        inc     eax
        inc     eax
        mov     edx,[code_address]
        add     eax,edx
    mov	[edi+operand.address],eax
    bt	dword [edi + operand.oeflags],OMF_OP32
    jc	short op10notword
    and	dword [edi+operand.address],0ffffh
op10notword:
    sub	eax,eax
    ret

;/* op 11 RM, immediate */
op11	:	
    call	readrm
    movzx	ecx,al
    mov	edi,ebx
    push	esi
    add	esi,ecx
    add	esi,2
    call	Immediate
    add	cl,al
    pop	esi
    mov	eax,ecx
    ret

;/* op 12 - acc, immediate */
op12	:	
    mov	al,REG_EAX
    call	setreg
    mov	edi,ebx
    inc	esi
    call	Immediate
    dec	esi
    ret

;/* op 13 absolute, acc*/
op13	:	
    sub	ecx,ecx
    mov	byte [edi + operand.typecode],OM_ABSOLUTE
    bt	dword [edi + operand.oeflags],OMF_ADR32
    jnc	short op13word
    LONG	esi+1
    inc	cl
    inc	cl
    jmp	short op13fin
op13word:
    UINT	esi+1
op13fin:
    mov	[edi+operand.address],eax
    mov	edi,ebx
    mov	al,REG_EAX
    call	setreg
    mov	eax,ecx
    ret

;/* op 14 - RM, immediate, b01 of opcode != 1 for byte */
op14	:	
    call	readrm
    movzx	ecx,al
    mov	al,[esi]
    B01
        test    al,1
    jnz	short op14checkbyte
    bts	dword [ebx + operand.oeflags],OMF_BYTE
    bts	dword [edi + operand.oeflags],OMF_BYTE
    jmp	short op14source
op14checkbyte:
    btr	dword [ebx + operand.oeflags],OMF_BYTE
    cmp	al,1
    jz	short op14check2
    bts	dword [ebx + operand.oeflags],OMF_BYTE
op14check2:
    btr	dword [edi + operand.oeflags],OMF_BYTE
op14source:
    mov	edi,ebx
    push	esi
    add	esi,ecx
    add	esi,2
    call	Immediate
    pop	esi
    add	cl,al
    mov	al,[esi]
    B01
    cmp	al,3
    jnz	op14done
    bt	dword [edi + operand.oeflags],OMF_BYTE
    jnc	op14done
    bts	dword [edi + operand.oeflags],OMF_SIGNED
    movzx	eax,byte [edi+operand.address]
    mov	[edi+operand.address],eax
op14done:
    mov	eax,ecx
    ret

;/* op 15 - acc, immediate, B3 of opcode clear for byte */
op15	:	
    mov	al,[esi]
    B02
    call	setreg
    bt	dword [esi],3
    jnc	op15byte
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
    jmp	short op15source
op15byte:
    bts	dword [edi + operand.oeflags],OMF_BYTE
    bts	dword [ebx + operand.oeflags],OMF_BYTE
op15source:
    mov	edi,ebx
    inc	esi
    call	Immediate
    dec	esi
    ret

;/* op 16 - seg,readrm, if B1 of opcode seg is dest else source */
op16	:	
    bt	dword [esi],1
    jc	noswap
    xchg	ebx,edi
noswap:
    REG	esi
    call	setseg
    mov	edi,ebx
    btr	dword [edi + operand.oeflags],OMF_BYTE
    call	readrm
    ret

;/* op 17, far return */
op17	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    mov	byte [edi + operand.typecode],OM_RETURN
    btr	dword [edi + operand.oeflags],OMF_ADR32
    btr	dword [edi + operand.oeflags],OMF_OP32
    btr	dword [edi + operand.oeflags],OMF_BYTE
    UINT	esi+1
    mov	[edi+operand.address],eax
    sub	eax,eax
    ret

;/* op 18, far branch/call */
op18	:	
    sub	ecx,ecx
    mov	byte [reqsPtrSizeOvride],FALSE
    mov	byte [edi + operand.typecode],OM_FARBRANCH
    btr	dword [edi + operand.oeflags],OMF_BYTE
    bt	dword [edi + operand.oeflags],OMF_OP32
    jnc	short op18word
    inc	cl
    inc	cl
    LONG	esi+1
    jmp	short	op18fin
op18word:
    UINT	esi+1
op18fin:
    mov	[edi+operand.address],eax
    movzx	ecx,cx
    UINT	esi+ecx+3
    mov	[edi+operand.oeseg],ax
    mov	eax,ecx
    ret

;/* op 19 - ESC, mnem of bits 0-2 of opcode, imm,readrm */
op19	:	
    mov	byte [edi + operand.typecode],OM_IMMEDIATE
    bts	dword [edi + operand.oeflags],OMF_BYTE
    mov	al,[esi]
    and	al,7
    shl	al,3
    mov	ah,[esi+1]
    shr	ah,3
    and	ah,7
    or	al,ah
    sub	ah,ah
    cwde
    mov	[edi+ operand.address],eax
    mov	edi,ebx
    call	readrm
    ret

;/* op 20 - long branch */
op20	:	
    sub	eax,eax
    mov	byte [reqsPtrSizeOvride],FALSE
    sub	ecx,ecx
    mov	byte [edi + operand.typecode],OM_LONGBRANCH
    bt	dword [edi + operand.oeflags],OMF_OP32
    jnc	short op20word
    LONG	esi+1
    inc	cl
    inc	cl
    jmp	short op20fin
op20word:
    UINT	esi+1
op20fin:

        mov   edx,[code_address]
        add     eax,edx
    test	word [segs],SG_TWOBYTEOP
    jz	op20xx
    inc	ax
op20xx:
        test    word [segs],SG_OPSIZ
        jz      op20xy
        inc     eax
op20xy:
    add	eax,ecx
    add	eax,3
    bt	dword [edi + operand.oeflags],OMF_OP32
    jc	op20done
    and	eax,0ffffh
op20done:
    mov	[edi+operand.address],eax
    mov	eax,ecx
    ret

;/* op21 acc,dx */
op21	:	
    mov	al,REG_EAX
    call	setreg
    mov	edi,ebx
    btr	dword [edi + operand.oeflags],OMF_OP32
    btr	dword [edi + operand.oeflags],OMF_BYTE
    mov	al,REG_DX
    call	setreg
    sub	eax,eax
    ret

;/* op22 - dx,acc */
op22	:	
    btr	dword [edi + operand.oeflags],OMF_OP32
    btr	dword [edi + operand.oeflags],OMF_BYTE
    mov	al,REG_DX
    call	setreg
    mov	edi,ebx
    mov	al,REG_EAX
    call	setreg
    sub	eax,eax
    ret

;/* op23 - port,acc where B1 of opcode set is port dest */
op23	:	
    bt	dword [esi],1
    jc	short op20noswap
    xchg	ebx,edi
op20noswap:
    bts	dword [edi + operand.oeflags],OMF_BYTE
    mov	byte [edi + operand.typecode],OM_PORT
    movzx	eax,byte [esi+1]
    mov	[edi+operand.address],eax
    mov	edi,ebx
    mov	al,REG_EAX
    call	setreg
    sub	eax,eax
    ret

;/* op 24 acc, absolute */
op24	:	
    sub	ecx,ecx
    mov	al,REG_EAX
    call	setreg
    mov	edi,ebx
    mov	byte [edi + operand.typecode],OM_ABSOLUTE
    bt	dword [edi + operand.oeflags],OMF_ADR32
    jnc	short op24word
    inc	cl
    inc	cl
    LONG	esi+1
    jmp	short op24done
op24word:
    UINT	esi+1
op24done:
    mov	[edi+operand.address],eax
    mov	ax,cx
    ret

;/* op 25 - immediate byte or word */
op25	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    bts	dword [edi + operand.oeflags],OMF_BYTE
    bt	dword [esi],1
    jc	short op25fin
    btr	dword [edi + operand.oeflags],OMF_BYTE
op25fin:
    push	esi
    inc	esi
        call    Immediate
    pop	esi
        bts     dword [edi + operand.oeflags],OMF_SIGNED
    ret

;/* op 26, immediate 2byte,byte */
op26	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [edi + operand.oeflags],OMF_OP32
    push	esi
    inc	esi
    call	Immediate
    mov	edi,ebx
    bts	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [edi + operand.oeflags],OMF_OP32
    inc	esi
    inc	esi
    call	Immediate
    pop	esi
    sub	eax,eax
    ret

;/* op 27 - string */
op27	:	
    mov	al,'d'
    bt	dword [edi + operand.oeflags],OMF_OP32
    jc	short op27pc
    mov	al,'w'
op27pc:
    call	MnemonicChar
    sub	eax,eax
    ret

;/* op 28 - source = REG, dest = RM */
op28	:	
    xchg	edi,ebx
    jmp	op7

;/* op 29 - rm, size don't care */
op29	:	
    btr	dword [edi + operand.oeflags],OMF_OP32
    btr	dword [edi + operand.oeflags],OMF_BYTE
        mov     byte [reqsPtrSizeOvride],FALSE
    call	readrm
    ret

;/* op30 - RM, shift with B3 of stream selecting COUNT or CL*/
op30	:	
    call	readrm
        movzx   ecx,al
    mov	edi,ebx
    mov	byte [edi + operand.typecode],OM_SHIFT
    bt	dword [esi],3
    jnc	op30cl
    movzx	ecx,cx
    movzx	eax,byte [esi+ecx+2]
    mov	[edi+operand.address],eax
    inc	ecx
    jmp	short op30done
op30cl:
    bts	dword [edi + operand.oeflags],OMF_CL
op30done:
    mov	eax,ecx
    ret

;/* op 31- reg, rm, count where B1 of opcode = byte/word */
op31	:	
    call	CopyExtra
    REG	esi
    call	setreg
    mov	edi,ebx
    call	readrm
        movzx   ecx,al
    mov	edi,extraoperand
    btr	dword [edi + operand.oeflags],OMF_BYTE
    bt	dword [esi],1
        jnc     short op31byte
    bts	dword [edi + operand.oeflags],OMF_BYTE
        bts     dword [edi + operand.oeflags],OMF_SIGNED
op31byte:
    push	esi
    inc	esi
        inc     esi
        add     esi,ecx
    call	Immediate
    pop	esi
    add	eax,ecx
    ret

;/* op32 - 386 special regs */
op32	:	
    mov	cx,word [esi]
    and	cx,0c005h
    cmp	cx,0c000h
    mov	al,OM_CRX
    jz	short op32gotype
    cmp	cx,0c001h
    mov	al,OM_DRX
    jz	short op32gotype
    cmp	cx,0c004h
    mov	al,OM_TRX
    jz	short op32gotype
    mov	al,OM_SUD
op32gotype:
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
        btr     dword [edi + operand.oeflags],OMF_ADR32
        btr     dword [ebx + operand.oeflags],OMF_ADR32
    bts	dword [edi + operand.oeflags],OMF_OP32
    bts	dword [ebx + operand.oeflags],OMF_OP32
    bt	dword [esi],1
    jc	op32noswap
    xchg	ebx,edi
op32noswap:
    mov	byte [edi + operand.typecode],al
    REG	esi
    mov	byte [edi + operand.thereg],al
    mov	edi,ebx
    RM	esi
    call	setreg
    sub	eax,eax
    ret

;/* op33 - reg,rm,shiftcnt where B3 = reg source, b0 = shift cl */
op33	:	
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
    call	CopyExtra
    call	readrm
        movzx   ecx,al
    REG	esi
    mov	edi,ebx
    call	setreg
    mov	edi,extraoperand
    mov	byte [edi + operand.typecode],OM_SHIFT
    bt	dword [esi],0
    jnc	short getofs
    bts	dword [edi + operand.oeflags],OMF_CL
    jmp	short op33done
getofs:
    movzx	ecx,cx
    movzx	eax,byte [esi+ecx+2]
    mov	[edi+operand.address],eax
op33done:
    mov	eax,ecx
    ret

;/* op 34 - push & pop word, also no reqsPtrSizeOvride single args */
op34	:	
    call	readrm
    ret

;/* op 35 -floating RM */
op35	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    MODX	esi
    cmp	al,3
    jnz	short op35fsttab
    bts	dword [edi + operand.oeflags],OMF_FST
    call	readrm
    ret

op35fsttab	:
    bts	dword [edi + operand.oeflags],OMF_FSTTAB
    movzx	eax,byte [esi]
    B12
    shl	eax, OM_FTAB
        or      dword [edi + operand.oeflags],eax
    call	readrm
    ret

;/* op 36 - floating math & pop */

op36	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    mov	al,[esi+1]
    and	al,0c7h
    cmp	al,0c1h
    jz	op36noarg
    bts	dword [edi + operand.oeflags],OMF_FST
    and	al,7
    call	setreg
    sub	al,al
    xchg	ebx,edi
    bts	dword [edi + operand.oeflags],OMF_FST
    call	setreg
op36noarg:
    sub	eax,eax
    ret

;/* OP 37 - floating MATH */
op37	:	
    mov	byte [reqsPtrSizeOvride],FALSE
    MODX	esi
    cmp	al,3
    jc	op35fsttab
    test	byte [esi],4
    jnz	op37noflop
    xchg	edi,ebx
op37noflop:
    bts	dword [edi + operand.oeflags],OMF_FST
    RM	esi
    call	setreg
    sub	al,al
    xchg	ebx,edi
    bts	dword [edi + operand.oeflags],OMF_FST
    call	setreg
    sub	eax,eax
    ret

;/* op38 - FAR RM */
op38	:	
;        mov     byte [reqsPtrSizeOvride],FALSE
;        bts     dword [edi + operand.oeflags],OMF_FSTTAB
        bts     dword [edi + operand.oeflags],OMF_FARPTR
    call	readrm
    ret

;/* OP39 - word regrm with reg source */
op39	:	
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
    call	op40
    ret

;/* op 40 regrm with reg source */
op40	:	
    mov	[dest2],ebx
    mov	[source2],edi
    call	RegRM
    ret

;/* op 41 reg, bitnum */
op41	:	
    btr	dword [edi + operand.oeflags],OMF_BYTE
    call	readrm
        movzx   ecx,al
    mov	edi,ebx
    bts	dword [edi + operand.oeflags],OMF_BYTE
    push	esi
    add	esi,ecx
    add	esi,2
    call	Immediate
    pop	esi
    mov	eax,ecx
    ret

;/* op 42 mixed regrm with reg dest & reqsPtrSizeOvrideness enforced */

op42	:	
    mov	[dest2],edi
    mov	[source2],ebx
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_OP32
    call	RegRM
    mov	byte [reqsPtrSizeOvride],TRUE
    ret

;/* op 43 CWDE */

op43	:	
    bt	dword [edi + operand.oeflags],OMF_OP32	
    jnc	short op43nochng		
    push	esi				
    mov	esi,nmmnemonic+1		
    mov	eax,"wde"			
    call	put3				
    mov	byte [esi],0			
    pop	esi				
    sub	eax,eax				
op43nochng:
    ret

;/* op 44 BSWAP */

op44	:
    btr	dword [edi + operand.oeflags],OMF_BYTE
    mov	al,[esi]
    and	al,7
    call	setreg
    sub	eax,eax
    ret

;/* OP45 - any regrm with reg source */

op45	:	
    bt	word [esi-1],0
    jc	op45nb
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
op45nb:
        mov     [dest2],ebx
        mov     [source2],edi
        call    RegRM
;        XCHG    BX,DI
;        REG     si
;        call    setreg
;        XCHG    bx,di
;        call    readrm
        ret

;/* op 46 - floating R/M, pointers to mem */
op46	:	
    jmp	op29

;/* op47 - AX */
op47	:
    btr	dword [edi + operand.oeflags],OMF_OP32
    btr	dword [edi + operand.oeflags],OMF_BYTE
    sub	al,al
    call	setreg
    sub	eax,eax
    ret

;/* op48 - 32-bit reg to r02, no prefix (bswap) */
op48	:
    bts	dword [edi + operand.oeflags],OMF_OP32
    btr	dword [edi + operand.oeflags],OMF_BYTE
    mov	al,[esi]
    B02
    call	setreg
    sub	eax,eax
    ret

;/* op 49, FST */
op49	:
    mov	byte [reqsPtrSizeOvride],FALSE
    MODX	esi
    cmp	al,3
    jnz	op49noreg
    bts	dword [edi + operand.oeflags],OMF_FST
    jmp	short op49fin
op49noreg:
    bts	dword [edi + operand.oeflags],OMF_FSTTAB
    movzx	eax,byte [esi]
    and	al,6
    jz	op49nxor
    xor	al,6
op49nxor:
    shl	eax, OM_FTAB
        or      dword [edi + operand.oeflags],eax
op49fin:
    call	readrm
    ret

;/* op 50, FLD & FSTP */
op50	:
    jmp	op49

;/* op 51, FBLD/FBSTP */
op51	:
    mov	byte [reqsPtrSizeOvride],FALSE
    bts	dword [edi + operand.oeflags],OMF_FSTTAB
    or	dword [edi + operand.oeflags],4 << OM_FTAB
    call	readrm
    ret

;/* op 52, FILD & FISTP */
op52	:
    mov	byte [reqsPtrSizeOvride],FALSE
    mov	ah,1		; assume dword
    mov	al,[esi]
    cmp	al,0dfh
    jnz	op52dw
    mov	ah,2		; assume qword
    test	byte [esi+1],20h
    jnz	op52dw
    mov	ah,3
op52dw:
    mov	al,ah
    sub	ah,ah
    bts	dword [edi + operand.oeflags],OMF_FSTTAB
    shl	eax,OM_FTAB
        or      dword [edi + operand.oeflags],eax
    call	readrm
    ret

;/* op 53, FIST & FISTP */
op53	:
    jmp	op52

;/* op 54, FREG */
op54	:
    mov	byte [reqsPtrSizeOvride],0
    bts	dword [edi + operand.oeflags],OMF_FST
    call	readrm
    ret

;/* op 55, FREG if not one */
op55	:
    mov	al,[esi+1]
    and	al,7
    cmp	al,1
    jnz	op54
    sub	eax,eax
    ret

;/* op 56, FIMATH */
op56	:
    mov	byte [reqsPtrSizeOvride],FALSE
    mov	eax,1
    test	byte [esi],4
    jz	op56go
    mov	al,3
op56go:
    bts	dword [edi + operand.oeflags],OMF_FSTTAB
    shl	eax,OM_FTAB
        or      dword [edi + operand.oeflags],eax
    call	readrm
    ret
;/* op 59 MMXREG,RM */
op59    :
        call    issse
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
        bts     dword [edi + operand.oeflags],OMF_MMXTAB
        bts     dword [ebx + operand.oeflags],OMF_MMXTAB
        mov     [dest2],edi
        mov     [source2],ebx
        call    RegRM
    ret

;/* op60 MMXMOVDLD */
op60    :
        call    issse
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
        bts     dword [edi + operand.oeflags],OMF_MMXTAB
        bts     dword [ebx + operand.oeflags],OMF_OP32
        mov     [dest2],edi
        mov     [source2],ebx
        call    RegRM
    ret

;/* op61 MMXMOVDST */
op61    :
        call    issse
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
        bts     dword [ebx + operand.oeflags],OMF_MMXTAB
        bts     dword [edi + operand.oeflags],OMF_OP32
        mov     [dest2],ebx
        mov     [source2],edi
        call    RegRM
    ret

;/* op62 MMXMOVQLD */
op62    :
        call    issse
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
        bts     dword [edi + operand.oeflags],OMF_MMXTAB
        bts     dword [ebx + operand.oeflags],OMF_MMXTAB
        mov     [dest2],edi
        mov     [source2],ebx
        call    RegRM
    ret

;/* op63 MMXMOVQST */
op63    :
        call    issse
    btr	dword [edi + operand.oeflags],OMF_BYTE
    btr	dword [ebx + operand.oeflags],OMF_BYTE
        bts     dword [ebx + operand.oeflags],OMF_MMXTAB
        bts     dword [edi + operand.oeflags],OMF_MMXTAB
        mov     [dest2],ebx
        mov     [source2],edi
        call    RegRM
    ret

;/* op64 MMXSHIFT */
op64    :
        call    issse
        bts     dword [edi + operand.oeflags],OMF_MMXTAB
        bts     dword [ebx + operand.oeflags],OMF_MMXTAB
        call    readrm
    sub	ecx,ecx
    mov	cl,al
    mov	edi,ebx
    mov	byte [edi + operand.typecode],OM_SHIFT
    movzx	ecx,cx
    movzx	eax,byte [esi+ecx+2]
    inc	cl
    mov	[edi+operand.address],eax
    mov	eax,ecx
    ret

;/* loop */
op65    :
    mov	al,'d'
        bt      dword [edi + operand.oeflags],OMF_ADR32
        jc      short op65pc
    mov	al,'w'
op65pc:
    call	MnemonicChar
    sub	eax,eax
    mov	byte [reqsPtrSizeOvride],FALSE
        mov     byte [edi + operand.typecode],OM_SHORTBRANCH
        movsx   eax,byte [esi+1]

        mov   edx,[code_address]
        add     eax,edx
        add     eax,2
        sub     edx,edx
        bt      dword [edi + operand.oeflags],OMF_ADR32
        jnc     op65notas
        inc     edx
op65notas:
;        test    [optass32],255
;        jz      op65t2
        neg     edx
        inc     edx
op65t2:
        add     eax,edx
    bt	dword [edi + operand.oeflags],OMF_OP32
        jc      op65done
    and	eax,0ffffh
op65done:
    mov	[edi+operand.address],eax
        sub     eax,eax
        clc
    ret

; CDW/CDQ

op66    :    
    bt	dword [edi + operand.oeflags],OMF_OP32	
        jnc     short op66nochng                
    push	esi				
    mov	esi,nmmnemonic+1		
        mov     eax,"dq"                       
        call    put2                            
    mov	byte [esi],0			
    pop	esi				
    sub	eax,eax				
op66nochng:
    ret

;
; AAM
;
op67    :
        inc     esi
        mov     al,[esi]
        bts     dword [edi + operand.oeflags],OMF_BYTE
        call    Immediate
        sub     eax,eax
        ret

;
; XLAT
;
op68    :
        mov     byte [reqsPtrSizeOvride],FALSE
        mov     byte [edi + operand.typecode],OM_BASED
        mov     byte [edi + operand.thereg],3 ; [ebx]
        bt      dword [edi + operand.oeflags],OMF_ADR32
        jc      op68n32
        mov     byte [edi + operand.thereg],7 ; [ebx]
op68n32:
        sub     eax,eax  
        ret

;
; set xx
;
op69    :
    bts	dword [edi + operand.oeflags],OMF_BYTE
    bts	dword [ebx + operand.oeflags],OMF_BYTE
        jmp     op29
;
; lgdt/sgdt
;
op70    :
        btr     dword [edi + operand.oeflags],OMF_BYTE
        bt      dword [edi + operand.oeflags],OMF_OP32
        jc      op70_fword
        bts     dword [edi + operand.oeflags],OMF_PBYTE
        jmp     op70_cont
op70_fword:
        bts     dword [edi + operand.oeflags],OMF_FWORD
op70_cont:
    call	readrm
    ret

ReadOverrides   :    
        mov     word [segs],0
          
ro_lp:
    sub	eax,eax
    lodsb
;	test	[disassemble32Bit],1
;	jz	testseg
    cmp	al,64h
    jc	short testseg
    cmp	al,68h
    jnc	short testseg
    sub	al,64h
    mov	bx,SG_FS
ro_found:
    mov	cl,al
    shl	ebx,cl
    or	word [segs],bx
    jmp	short ro_lp
testseg:
    push	ax
    and	al,0e7h
    cmp	al,026h
    pop	ax
    jnz	testrep
    mov	ebx,1
    shr	ax,3
    and	al,3
    jmp	ro_found
testrep:
        sub     al,0f0h         ; lock
        mov     bx,SG_LOCK
        jz      short ro_found
        sub     al,02h
    cmp	al,2
    jnc	ro_done
    mov	ebx,SG_REPNZ
    jmp	short ro_found
ro_done:
    dec	si
    ret


DispatchOperands	:	
    mov	edx,eax
    push	ebx
    mov	edi,nmmnemonic
    push	esi
    mov	esi,[ebx+opcode.mnemonic]
    call	strcpy
    pop	esi
    mov	byte [reqsPtrSizeOvride],TRUE
    movzx	eax,byte [ebx+opcode.operands]
    push	eax
    mov	edi,dest
    mov	ebx,source
    test	byte [x86pfx],1
    jz	short notwobyte
    or	word [segs],SG_TWOBYTEOP
notwobyte:
    mov	eax,extraoperand
    mov	byte [eax+operand.typecode],0
    mov	byte [edi + operand.typecode],0
    mov	byte [ebx+operand.typecode],0
    mov	dword [edi + operand.oeflags],0
    mov	dword [ebx + operand.oeflags],0
        mov     dword [edi+operand.address],0
        mov     dword [ebx+operand.address],0

    
    bt	dword [esi],0
    jc	notbyte
    bts	dword [edi + operand.oeflags],OMF_BYTE
    bts	dword [ebx + operand.oeflags],OMF_BYTE
notbyte:
    or	edx,edx
    jz	short xadr16
    bts	dword [edi + operand.oeflags],OMF_ADR32
    bts	dword [ebx + operand.oeflags],OMF_ADR32
    bts	dword [edi + operand.oeflags],OMF_OP32
    bts	dword [ebx + operand.oeflags],OMF_OP32
xadr16:
    test	word [segs],SG_ADRSIZ
    jz	do_word1
    btc	dword [edi + operand.oeflags],OMF_ADR32
    btc	dword [ebx + operand.oeflags],OMF_ADR32
do_word1:
        mov     byte [realopsize],FALSE
    test	word [segs],SG_OPSIZ
    jz	do_word2
        mov     byte [realopsize],TRUE
    btc	dword [edi + operand.oeflags],OMF_OP32
    btc	dword [ebx + operand.oeflags],OMF_OP32
do_word2:
    pop	eax
    or	eax,eax
    jz	near nodispatch
    dec	al
    push	0
    call	TableDispatch
        dd      69
    dd	op1,  op2,  op3,  op4,  op5,  op6,  op7,  op8,  op9,  op10
    dd	op11, op12, op13, op14, op15, op16, op17, op18, op19, op20
    dd	op21, op22, op23, op24, op25, op26, op27, op28, op29, op30
    dd	op31, op32, op33, op34, op35, op36, op37, op38, op39, op40
    dd	op41, op42, op43, op44, op45, op46, op47, op48, op49, op50
        dd      op51, op52, op53, op54, op55, op56, op59, op59, op59, op60
        dd      op61, op62, op63, op64, op65, op66, op67, op68, op69, op70
    movzx	eax,al
    add	esi,eax
joindispatch:
    pop	ebx
    movzx	eax,byte [ebx+opcode.oclength]
    add	esi,eax
    ret
nodispatch:
        btr     dword [edi + operand.oeflags],OMF_ADR32
        btr     dword [ebx + operand.oeflags],OMF_ADR32
        btr     dword [edi + operand.oeflags],OMF_OP32
        btr     dword [ebx + operand.oeflags],OMF_OP32
        jmp     joindispatch


DoReqsPtrSizeOvride	:	
;	mov byte [esi],C_RESERVEdword
;	inc esi
    mov byte [esi],0
    push	edi
    push	esi
    test	byte [reqsPtrSizeOvride],-1
    jz	short floatreqsPtrSizeOvride
        bt      dword [edi + operand.oeflags],OMF_FARPTR
        jnc     drpso_nofar
        push    esi
        push    edi
        mov     edi,esi
        mov     esi,opn_farptr
        call    strcat
        pop     edi
        pop     esi
drpso_nofar:

    bt	dword [edi + operand.oeflags],OMF_BYTE
        jnc     chkfwptr
    mov	edi,esi
    mov	esi,byptr
    jmp	short reqsPtrSizeOvrideend
chkfwptr:
        bt      dword [edi + operand.oeflags],OMF_FWORD
        jnc     chkpbyteptr
    mov	edi,esi
        mov     esi,fwptr
    jmp	short reqsPtrSizeOvrideend
chkpbyteptr:
        bt      dword [edi + operand.oeflags],OMF_PBYTE
        jnc     chkdwptr
    mov	edi,esi
        mov     esi,pbptr
    jmp	short reqsPtrSizeOvrideend
chkdwptr:
    bt	dword [edi + operand.oeflags],OMF_OP32
    mov	edi,esi
        jnc     mkwordptr
    mov	esi,dwptr
    jmp	short reqsPtrSizeOvrideend
mkwordptr:
    mov	esi,woptr
      jmp	short reqsPtrSizeOvrideend
floatreqsPtrSizeOvride:
    bt	dword [edi + operand.oeflags],OMF_FSTTAB
    jnc	reqsPtrSizeOvridedone
        mov     eax,dword [edi + operand.oeflags]
    shr	eax,OM_FTAB
    and	eax,7
    mov	edi,esi
    push	edi
    mov	esi,sts
    movzx	eax,ax
    mov	esi,[esi+eax * 4]
    call	strcat
    mov	esi,theptr
    pop	edi
reqsPtrSizeOvrideend:
    call	strcat
reqsPtrSizeOvridedone:
    pop	esi
    call	strlen
    add	esi,eax
;	mov byte [esi],C_DEFAULT
;	inc esi
;	mov byte [esi],0
    pop	edi
    ret

;Tab over to the next location on the display
; INPUT: AL contains the number of positions to move 
;	SI points to a buffer aparently containing display strings?
; If the buffer contains a string whose length exceeds the passed tab
; position, we just add the requested tab position to esi, which now points
; into the middle of that string somewhere.  Otherwise, we stuff spaces into
; the buffer up to the requested tab position.

TabTo	:	
    movzx	ecx,al		;convert length to dword
ttl:
    lodsb
    cmp	al,20h
    jae	notabcnt
    inc	ecx
notabcnt:
    or	al,al
    loopne	ttl
    jecxz	ttns
    dec		esi
    push	es
    push	ds
    pop		es
    mov		al,' '
    xchg	esi,edi
    cld
    rep		stosb
    xchg	esi,edi
    pop		es
ttns:
    mov		byte [esi], 0
    ret


GetST	:	
;	mov byte [esi],C_REGISTER
;	inc esi
    push	edi
    mov	al,byte [edi + operand.thereg]
    push	eax
    xchg	esi,edi
    mov	esi,stsreg
    call	strcpy
    pop	eax
    add	al,'0'
    dec	di
    stosb
    mov	al,')'
    stosb
    sub	al,al
    stosb
    dec	edi
    xchg	esi,edi
    pop	edi
;	mov byte [esi],C_DEFAULT
;	inc si
    mov byte [esi], 0
    ret

GetMMX   :    
;	mov byte [esi],C_REGISTER
;	inc esi
    push	edi
    mov	al,byte [edi + operand.thereg]
    push	eax
    xchg	esi,edi
        mov     esi,mmxreg
    call	strcpy
    pop	eax
    add	al,'0'
    dec	edi
    stosb
    sub	al,al
    stosb
    dec	edi
    xchg	esi,edi
    pop	edi
;	mov byte [esi],C_DEFAULT
;	inc	si
    mov byte [esi],0
    ret

GetStdReg	:	
;	mov byte[esi],C_REGISTER
;	inc esi
    push	edi
    or	al,al
    jnz	short gsrnoe
    mov	byte [esi],'e'
    inc	esi
gsrnoe:
    mov	edi,regs
    movzx	ecx,cx
    mov	ax,[edi+ecx *2]
    mov	[esi],al
    inc	esi
    mov	[esi],ah
    inc	esi
;	mov byte [esi],C_DEFAULT
;	inc esi
    mov	byte [esi],0
    pop	edi
    ret


GetReg	:	
    movzx	ecx,al
    sub	al,al
    inc	al
    bt	dword [edi + operand.oeflags],OMF_BYTE
    jc	short grno32
    bt	dword [edi + operand.oeflags],OMF_OP32
    jnc	short grno32
    dec	al
grno32:
    bt	dword [edi + operand.oeflags],OMF_BYTE
    jc	short isbyte
    or	cl,8
isbyte:
    call	GetStdReg
    ret


GetSpecial	:	
;	mov byte [esi],C_REGISTER
;	inc esi
    mov	al,[ebx]
    mov	[esi],al
    inc	esi
    inc	ebx
    mov	al,[ebx]
    mov	[esi],al
    inc	esi
    inc	ebx
    movzx	eax,byte [edi + operand.thereg]
    movzx	ebx,bx
    mov	al,[ebx +eax]
    mov	[esi],al
    inc	esi
;	mov byte [esi],C_DEFAULT
;	inc esi
    mov	byte [esi],0
    ret


GetSeg	:	
;	mov byte [esi],C_RESERVEdword
;	inc esi
    push	edi
    push	eax
    mov	edi,psegs
    movzx	ecx,cx
    mov	ax,[edi+ecx *2]
    mov	[esi],al
    inc	esi
    mov	[esi],ah
    inc	esi
;	mov byte [esi],C_DEFAULT
;	inc esi
    pop	eax
    or	al,al
    mov	al,':'
    jz	short nocolon
    mov	[esi],al
    inc	esi
nocolon:
    mov	byte [esi],0
    pop	edi
    ret


SegOverride	:	
    mov	al,1
        mov     byte [hasputoverride],al
    sub	ecx,ecx
    test	word [segs],SG_ES
    jz	short so_testcs
    call	GetSeg
so_testcs:
    inc	ecx
    test	word [segs],SG_CS
    jz	short so_testss
    call	GetSeg
so_testss:
    inc	ecx
    test	word [segs],SG_SS
    jz	short so_testds
    call	GetSeg
so_testds:
    inc	ecx
    test	word [segs],SG_DS
    jz	short so_testfs
    call	GetSeg
so_testfs:
    inc	ecx
    test	word [segs],SG_FS
    jz	short so_testgs
    call	GetSeg
so_testgs:
    inc	ecx
    test	word [segs],SG_GS
    jz	short so_done
    call	GetSeg
so_done:
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


scaled	:	
    push	dword [edi + operand.oeflags]
    btr	dword [edi + operand.oeflags],OMF_BYTE
    bts	dword [edi + operand.oeflags],OMF_OP32
    or	al,al
    jz	short notbased
    sub	al,al
    mov	al,byte [edi + operand.thereg]
    call	GetReg
notbased:
    bt	dword [edi + operand.oeflags],OMF_SCALED
    jnc	short notscaled2
    mov	byte [esi],'+'
    inc	esi
    mov	al,byte [edi + operand.scalereg]
    call	GetReg
    movzx	cx,byte [edi + operand.scale]
;        or      cx,cx
;        jz      notscaled2
    add	ecx,ecx
;	mov byte [esi],C_NUMBER
;	inc	esi

    lea	ecx,[ecx + scales]
    mov	ax,[ecx]
    call	put2
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
notscaled2:
    pop	dword [edi + operand.oeflags]
    ret


FOM_FSTREG	:	
    mov	edi,stalone
    call	strcat
    ret


FOM_CRX	:	
    mov	ebx,crreg
    call	GetSpecial
    ret


FOM_DRX	:	
    mov	ebx,drreg
    call	GetSpecial
    ret


FOM_TRX	:	
    mov	ebx,trreg
    call	GetSpecial
    ret


FOM_SUD	:	
    mov	ebx,sudreg
    call	GetSpecial
    ret


FOM_PORT	:	
    mov	al,SY_PORT
    jmp	FormatValue


FOM_INT	:	
    mov	al,SY_INTR
    jmp	FormatValue


FOM_SHIFT	:	
    bt	dword [edi + operand.oeflags],OMF_CL
    jnc	fos_notcl
;	mov byte [esi],C_REGISTER
;	inc	esi
    mov	ax,"cl"
    call	put2
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret
fos_notcl:
    cmp	dword [edi+operand.address],1
    mov	al,SY_SHIFT
    jnz	near FormatValue
    mov	byte [esi],'1'
    inc	esi
    mov	byte [esi],0
    ret


FOM_RETURN	:	
    mov	al,SY_RETURN
    jmp	FormatValue


FOM_SHORTBRANCH	:	
    mov	al,SY_SHORTBRANCH
    jmp	FormatValue


FOM_LONGBRANCH	:	
    mov	al,SY_LONGBRANCH
    jmp	FormatValue


FOM_FARBRANCH	:	
;        push    ebx
        mov     dx,[edi+operand.oeseg]
;        mov     ebx,[edi + operand.address]
;        push    es
;        push    edi
;        call    symLookupByValue
;        jc      psa_fb
;	mov byte [esi],C_SYMBOL
;	inc	esi
;        call    putsyms
;	mov byte [esi],C_DEFAULT
;	inc	esi
;        pop     edi
;        pop     es
;        pop     ebx
;        mov     byte [esi],'('
;        inc     si
;	mov	al,SY_SEGMENT
;	call	FormatValue
;	mov	byte [esi],':'
;	inc	esi
;        mov     eax,[edi + operand.address]
;        call    PutWordox
;        mov     byte [esi],')'
;        inc     esi
;	mov	byte [esi],0
;        ret
;psa_fb:
;        pop     edi
;        pop     es
;        pop     ebx
    mov	al,SY_SEGMENT
    call	FormatValue
    mov	byte [esi],':'
    inc	esi
    mov	al,SY_ABSBRANCH
        jmp     FormatValue


FOM_ABSOLUTE	:	
    call	DoReqsPtrSizeOvride
    call	SegOverride
    mov	byte [esi],'['
    inc	esi
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov	byte [esi],0
    bt	dword [edi + operand.oeflags],OMF_SCALED
    jnc	foa_notscaled
    mov	al,SY_WORDOFS
    call	FormatValue
    sub	eax,eax
    call	scaled
    jmp	short foa_finish
foa_notscaled:
    mov	al,SY_ABSOLUTE
    call	FormatValue
foa_finish:
    mov	byte [esi],']'
    inc	esi
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov	byte [esi],0
    ret


FOM_IMMEDIATE	:	
    bt	dword [edi + operand.oeflags],OMF_BYTE
    mov	al,SY_WORDIMM
    jnc	short absformat
;	test	[optsignedimm],1
;	jnz	fi_signed
    push	ebx
    movzx	ebx,byte [edi + operand.address]
        bt      dword [edi + operand.oeflags],OMF_SIGNED
        jnc     fi_nsi
        movsx   ebx,bl
fi_nsi:
    mov	[edi + operand.address],ebx
    pop	ebx
    jmp	FormatValue
;fi_signed:
;	mov	al,SY_BYTEOFS
;	bt	dword [edi + operand.oeflags],OMF_SIGNED
;	jnc	short absformat
;	mov	al,SY_SIGNEDIMM
;	test	[optsignedimm],1
;	jnz	absformat
;        movsx   eax,byte [edi + operand.address]
;	mov	[edi + operand.address],eax
;	mov	al,SY_WORDIMM
absformat:
    jmp	FormatValue


FOM_REG	:	
        bt      dword [edi + operand.oeflags],OMF_MMXTAB
        jnc     isfst
        call    GetMMX
        ret
isfst:
    bt	dword [edi + operand.oeflags],OMF_FST
    jnc	short foreg
    call	GetST
    ret
foreg:
    mov	al,byte [edi + operand.thereg]
    call	GetReg
    ret


FOM_BASED	:	
    call	DoReqsPtrSizeOvride
    call	SegOverride
    mov	byte [esi],'['
    inc	esi
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov	byte [esi],0
    bt	dword [edi + operand.oeflags],OMF_ADR32
    jnc	fob_notscaled
    mov	al,1
    call	scaled
    jmp	short fob2
fob_notscaled:
;	mov 	byte [esi],C_REGISTER
;	inc		esi
    push	edi
    push	esi
    movzx	eax,byte [edi + operand.thereg]
    xchg	esi,edi
    mov	esi,based
    movzx	ecx,cx
    mov	esi,[esi+eax * 4]
    call	strcpy
    pop	esi
    pop	edi
    call	strlen
    add	esi,eax
;	mov		byte [esi],C_DEFAULT
;	inc esi
    mov		byte [esi],0
fob2:
    test	dword [edi + operand.oeflags],OMF_OFFSET
    jz	short fob_noofs
    bt	dword [edi + operand.oeflags],OMF_SIGNED_OFFSET
    mov	al,SY_SIGNEDOFS
    jc	fob_format
    mov	al,SY_WORDOFS
    bt	dword [edi + operand.oeflags],OMF_WORD_OFFSET
    jc	fob_format
    mov	al,SY_BYTEOFS
fob_format:
    call	FormatValue
fob_noofs:
    mov	byte [esi],']'
    inc	esi
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov	byte [esi],0
    ret


FOM_SEGMENT	:	
    movzx	cx,byte [edi + operand.thereg]
    sub	ax,ax
    call	GetSeg
    ret


Putoperand	:	
    call	strlen
    add	esi,eax
    mov	al,byte [edi + operand.typecode]
    dec	al
    js	short po_none
    push	0
    call	TableDispatch
    dd	17
    dd	FOM_BASED
    dd	FOM_SEGMENT
    dd	FOM_REG
    dd	FOM_IMMEDIATE
    dd	FOM_ABSOLUTE
    dd	FOM_FARBRANCH
    dd	FOM_LONGBRANCH
    dd	FOM_SHORTBRANCH
    dd	FOM_RETURN
    dd	FOM_SHIFT
    dd	FOM_INT
    dd	FOM_PORT
    dd	FOM_SUD
    dd	0
    dd	FOM_TRX
    dd	FOM_DRX
    dd	FOM_CRX
    dd	FOM_FSTREG
po_none:
    ret


FormatDisassembly	:	
    ENTER	256,0
        mov     byte [hasputoverride],0
    push	esi
    lea	edi,[ebp-256]
    push	edi
    mov	byte [edi],0
        test    word [segs],SG_LOCK
        jz      fd_notlock
        mov     esi,st_lock
        call    strcpy
fd_notlock:
    test	word [segs],SG_REPZ
    jz	fd_notrepz
    mov	esi,st_repz
        call    strcat
fd_notrepz:
    test	word [segs],SG_REPNZ
    jz	fd_notrepnz
        mov	esi,st_repnz
        call    strcat
fd_notrepnz:
    pop	edi
    xchg	esi,edi
    call	strlen
    add	esi,eax
;	mov	byte [esi], C_INSTRUCTION
;	inc esi
    mov	byte [esi], 0
    xchg	esi,edi
    mov	esi,nmmnemonic
    call	strcat
    lea	esi,[ebp-256]
    sub	eax,eax
    mov	al,TAB_ARGPOS
    call	TabTo
;	mov	byte [esi], C_DEFAULT
;	inc esi
    mov	byte [esi], 0
    mov	edi,dest
    call	Putoperand
    mov	edi,source
    test	byte [edi + operand.typecode],-1
    jz	short nosource
    mov	byte [esi],','
    inc	esi
    mov	byte [esi],0
    call	Putoperand
nosource:
    mov	edi,extraoperand
    test	byte [edi + operand.typecode],-1
    jz	short noextra
    mov	byte [esi],','
    inc	esi
    mov	byte [esi],0
    call	Putoperand
noextra:
    pop	esi	
    mov	byte [esi],0
        test    byte [hasputoverride],255
        jnz     nonewover
    call	SegOverride
nonewover:
    mov	edi,esi
    lea	esi,[ebp-256]
    call	strcat
    LEAVE
    ret


PutDword:
    push	eax		; To print a dword
    shr	eax,16		; Print the high 16 bits
    call	PutWord
    pop	eax		; And the low 16 bits
PutWord:
    push	eax		; To print a word
    mov	al,ah		; Print the high byte
    call	PutByte
    pop	eax		; And the low byte
PutByte:
    push	eax		; To print a byte
    shr	eax,4		; Print the high nibble
    call	putnibble
    pop	eax		; And the low nibble
putnibble:
    and	al,0fh		; Get a nibble
    add	al,'0'		; Make it numeric
    cmp	al,'9'		; If supposed to be alphabetic
    jle	onib
    add	al,7		; Add 7
onib:
    mov	[esi],al
    inc	esi
    ret

PutWordox:
;	mov byte [esi],C_NUMBER
;	inc	esi
    bt	dword [edi + operand.oeflags],OMF_OP32
    jnc	PutWord
    jmp	PutDword
PutWordax:
;	mov byte [esi],C_NUMBER
;	inc	esi
    bt	dword [edi + operand.oeflags],OMF_ADR32
    jnc	PutWord
    jmp	PutDword

;putsyms :
;        push    ecx
;        xchg    esi,edi
;        push    ds
;        push    es
;        push    ds
;        push    es
;        pop     ds
;        pop     es
;        add     esi,6
;        lodsb
;        movzx   ecx,al
;        rep     movsb
;        xchg    esi,edi
;        pop     es
;        pop     ds
;        pop     ecx
;        ret
;putsyms ENDP
defseg :
        test    word [segs],SG_ES
        jnz     defes
        test    word [segs],SG_DS
        jnz     defds
        test    word [segs],SG_CS
        jnz     defcs
        test    word [segs],SG_SS
        jnz     defss
        test    word [segs],SG_FS
        jnz     deffs
        test    word [segs],SG_GS
        jnz     defgs
defds:
        mov     dx,[drds]
        ret
defes:
        mov     dx,[dres]
        ret
defcs:
        mov     dx,[drcs]
        ret
defss:
        mov     dx,[drss]
        ret
deffs:
        mov     dx,[drfs]
        ret
defgs:
        mov     dx,[drgs]
        ret
        
putsymax :
;        push    ebx
;        mov     ebx,eax
;        call    defseg
;        push    es
;        push    edi
;        call    symLookupByValue
;        jc      psa_n
;		mov byte [esi],C_SYMBOL
;		inc	esi
;        call    putsyms
;        pop     edi
;        pop     es
;		mov byte [esi],C_NUMBER
;		inc	esi
;        mov     byte [esi],'('
;        inc     esi
;        mov     eax,ebx
;        pop     ebx
;        call    PutWordax
;        mov     byte [esi],')'
;        inc     esi
;;		mov byte [esi],C_DEFAULT
;;		inc	esi
;		mov byte [esi],0
;        ret
;psa_n:
;        pop     edi
;        pop     es
;        mov     eax,ebx
;        pop     ebx
        jmp     PutWordax
putsymox :
;        push    ebx
;        mov     ebx,eax
;        call    defseg
;        push    es
;        push    edi
;        call    symLookupByValue
;        jc      pso_n
;		mov byte [esi],C_SYMBOL
;		inc	esi
;        call    putsyms
;        pop     edi
;        pop     es
;		mov byte [esi],C_NUMBER
;		inc	esi
;        mov     byte [esi],'('
;        inc     esi
;        mov     eax,ebx
;        pop     ebx
;        call    PutWordox
;        mov     byte [esi],')'
;        inc     esi
;		mov byte [esi],C_DEFAULT
;		inc	esi
;		mov byte [esi],0
;        ret
;pso_n:
;        pop     edi
;        pop     es
;        mov     eax,ebx
;        pop     ebx
        jmp     PutWordox
putsymbr :
;        push    ebx
;        mov     ebx,eax
;        mov     dx,[drCS]
;        push    es
;        push    edi
;        call    symLookupByValue
;        jc      pso_b
;		mov byte [esi],C_SYMBOL
;		inc	esi
;        call    putsyms
;        pop     edi
;        pop     es
;		mov byte [esi],C_NUMBER
;		inc	esi
;        mov     byte [esi],'('
;        inc     esi
;        mov     eax,ebx
;        pop     ebx
;        call    PutWordox
;        mov     byte [esi],')'
;        inc     esi
;		mov byte [esi],C_DEFAULT
;		inc	esi
;		mov byte [esi],0
;        ret
;pso_b:
;        pop     edi
;        pop     es
;        mov     eax,ebx
;        pop     ebx
        jmp     PutWordox
FSY_SIGNEDOFS   :    
    push	ebx
    mov	eax,[edi+operand.address]
    bt	eax,7
    mov	bl,'+'
    jnc	fso_pos
    mov	bl,'-'
    neg	al
fso_pos:
    mov	byte [esi],bl
    inc esi
;	mov byte [esi],C_NUMBER
;	inc	esi
    call	PutByte
    pop	ebx
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_WORDOFS	:	
    push	ebx
    mov	eax,[edi+operand.address]
        bt      dword [edi + operand.oeflags],OMF_ADR32
        jnc     wordchk
        cmp     eax,-40000000h
        jmp     fwo_join
wordchk:
    cmp	ax,-4000h
fwo_join:
    mov	bl,'+'
    jc	fsy_wouseplus
        neg     eax
    mov	bl,'-'
fsy_wouseplus:
    mov	byte [esi],bl
    inc esi
;	mov byte [esi],C_NUMBER
;	inc	esi
    call    putsymax
    pop	ebx
    ret


FSY_BYTEOFS	:	
;	mov byte [esi],C_NUMBER
;	inc	esi
    mov	eax,[edi+operand.address]
    call	PutByte
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_ABSOLUTE	:	
    mov	eax,[edi+operand.address]
        call    putsymax
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_SIGNEDIMM	:	
    jmp	FSY_SIGNEDOFS


FSY_WORDIMM	:	
    mov	eax,[edi+operand.address]
        call    putsymox
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_BYTEIMM	:	
;	mov byte [esi],C_NUMBER
;	inc	esi
    mov	eax,[edi+operand.address]
    call	PutByte
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_PORT	:	
;	mov byte [esi],C_NUMBER
;	inc	esi
    mov	eax,[edi+operand.address]
    call	PutByte
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_INTR	:	
;	mov byte [esi],C_NUMBER
;	inc	esi
    mov	eax,[edi+operand.address]
    call	PutByte
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_RETURN	:	
;	mov byte [esi],C_NUMBER
;	inc	esi
    mov	eax,[edi+operand.address]
    call	PutWord
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_ABSBRANCH	:	
    mov	eax,[edi+operand.address]
        call    putsymbr
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_LONGBRANCH	;	
    mov	eax,[edi+operand.address]
        call    putsymbr
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_SHORTBRANCH	:	
    mov	eax,[edi+operand.address]
        call    putsymbr
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_SHIFT	:	
;	mov byte [esi],C_NUMBER
;	inc	esi
    mov	eax,[edi+operand.address]
    call	PutByte
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FSY_SEGMENT	:	
;	mov byte [esi],C_NUMBER
;	inc	esi
    mov	ax,[edi+operand.oeseg]
    call	PutWord
;	mov byte [esi],C_DEFAULT
;	inc	esi
    mov byte [esi],0
    ret


FormatValue	:	
    dec	al
    push	0
    call	TableDispatch
    dd	14
    dd	FSY_SIGNEDOFS,FSY_WORDOFS,FSY_BYTEOFS,FSY_ABSOLUTE
    dd	FSY_SIGNEDIMM,FSY_WORDIMM,FSY_BYTEIMM,FSY_PORT
    dd	FSY_INTR,FSY_RETURN,FSY_ABSBRANCH,FSY_LONGBRANCH
    dd	FSY_SHORTBRANCH,FSY_SHIFT,FSY_SEGMENT
    mov	byte [esi],0
    ret
