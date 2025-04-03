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

%include  "opcodes.asi"
%include  "opcom.ase"

    global	FindOpcode
    global	mnemonicMatchCount
    global	mnemonicMatchAddrTable
    global	LookupOpName
    global  x86pfx

    segment data USE32

x86pfx	db	0
;
; Following is a table of fill_opcodes.  Each entry consists of a mask value,
; a comparison value, a pointer to the name, the addressing mode to be
; used in dissassembly, and the base length of the instruction (possibly
; modified by the exact addressing mode encountered)
;
; added two new bytes: first is evalutation flags, second is unused
;
;  The groups are selected based on the high order 3 bits of the first
;  byte of the fill_opcode, which are translated into a table offset. These
;  bits probably encode something that all fill_opcodes have in common if the
;  high order 3 bits are the same, but I don't know what it might be.
;	In any case, from the lookup logic, it appears that these structures
;  are broken into groups only for performance, which is hardly necessary
;  since this program won't run on anything less than a P24T - it has a
;  CPUID instruction in it.

group000xxxxx	EQU $
    fill_opcode	0FCh,0,opn_add,OP_REGRMREG,2,0,0	;ADD RM
    fill_opcode	0e7h,06h,opn_push,OP_SEG35,1,0,0	;PUSH
    fill_opcode	0feh,04h,opn_add,OP_ACCIMM,1,0,0	;ADD immediate
    fill_opcode	0e7h,07h,opn_pop,OP_SEG35,1,0,0	;POP
    fill_opcode	0fch,08h,opn_or,OP_REGRMREG,2,0,0	;OR RM
    fill_opcode	0feh,0ch,opn_or,OP_ACCIMM,1,0,0	;OR immediate
    fill_opcode	0fch,010h,opn_adc,OP_REGRMREG,2,0,0	;ADC RM
    fill_opcode	0feh,014h,opn_adc,OP_ACCIMM,1,0,0	;ADC immediate
    fill_opcode	0fch,018h,opn_sbb,OP_REGRMREG,2,0,0	;SBB RM
    fill_opcode	0feh,01ch,opn_sbb,OP_ACCIMM,1,0,0	;SBB immediate
    fill_opcode	0,0,0,0,0,0,0				;group terminator

group001xxxxx	EQU $
    fill_opcode	0fch,020h,opn_and,OP_REGRMREG,2,0,0	;AND RM
    fill_opcode	0feh,024h,opn_and,OP_ACCIMM,1,0,0	;AND immediate
    fill_opcode	0ffh,027h,opn_daa,OP_CODEONLY,1,0,0	;DAA
    fill_opcode	0fch,028h,opn_sub,OP_REGRMREG,2,0,0	;SUB RM
    fill_opcode	0feh,02ch,opn_sub,OP_ACCIMM,1,0,0	;SUB immediate
    fill_opcode	0ffh,02fh,opn_das,OP_CODEONLY,1,0,0	;DAS
    fill_opcode	0fch,030h,opn_xor,OP_REGRMREG,2,0,0	;XOR RM
    fill_opcode	0feh,034h,opn_xor,OP_ACCIMM,1,0,0	;XOR immediate
    fill_opcode	0ffh,037h,opn_aaa,OP_CODEONLY,1,0,0	;AAA
    fill_opcode	0fch,038h,opn_cmp,OP_REGRMREG,2,0,0	;CMP RM
    fill_opcode	0feh,03ch,opn_cmp,OP_ACCIMM,1,0,0	;CMP immediate
    fill_opcode	0ffh,03fh,opn_aas,OP_CODEONLY,1,0,0	;AAS
    fill_opcode	0,0,0,0,0,0,0				;group terminator

group010xxxxx	EQU $
    fill_opcode	0f8h,040h,opn_inc,OP_WREG02,1,0,0	;INC
    fill_opcode	0f8h,048h,opn_dec,OP_WREG02,1,0,0	;DEC
    fill_opcode	0f8h,050h,opn_push,OP_WREG02,1,0,0	;PUSH
    fill_opcode	0f8h,058h,opn_pop,OP_WREG02,1,0,0	;POP
    fill_opcode	0,0,0,0,0,0,0				;group terminator

group011xxxxx	EQU $
    fill_opcode	0ffh,060h,opn_pusha,OP_INSWORDSIZE,1,1,0	;PUSHA
    fill_opcode	0ffh,061h,opn_popa,OP_INSWORDSIZE,1,1,0	;POPA
    fill_opcode	0ffh,062h,opn_bound,OP_WORDREGRM,2,1,0	;BOUND
    fill_opcode	0ffh,063h,opn_arpl,OP_WORDRMREG,2,1,0		;ARPL
    fill_opcode	0fdh,068h,opn_push,OP_IMM,1,1,0		;PUSH imm
    fill_opcode	0fdh,069h,opn_imul,OP_IMUL,2,0,0		;IMUL
    fill_opcode	0ffh,06ch,opn_insb,OP_CODEONLY,1,1,0		;INSB
    fill_opcode	0ffh,06dh,opn_ins,OP_INSWORDSIZE,1,1,0	;INS
    fill_opcode	0ffh,06eh,opn_outsb,OP_CODEONLY,1,1,0		;OUTSB
    fill_opcode	0ffh,06fh,opn_outs,OP_INSWORDSIZE,1,1,0	;OUTS
    fill_opcode	0ffh,070h,opn_jo,OP_SHORTBRANCH,2,0,0		;JO
    fill_opcode	0ffh,071h,opn_jno,OP_SHORTBRANCH,2,0,0	;JNO
    fill_opcode	0ffh,072h,opn_jb,OP_SHORTBRANCH,2,0,0		;JB
    fill_opcode	0ffh,073h,opn_jnb,OP_SHORTBRANCH,2,0,0	;JNB
    fill_opcode	0ffh,074h,opn_jz,OP_SHORTBRANCH,2,0,0		;JZ
    fill_opcode	0ffh,075h,opn_jnz,OP_SHORTBRANCH,2,0,0	;JNZ
    fill_opcode	0ffh,076h,opn_jbe,OP_SHORTBRANCH,2,0,0	;JBE
    fill_opcode	0ffh,077h,opn_ja,OP_SHORTBRANCH,2,0,0		;JA
    fill_opcode	0ffh,078h,opn_js,OP_SHORTBRANCH,2,0,0		;JS
    fill_opcode	0ffh,079h,opn_jns,OP_SHORTBRANCH,2,0,0	;JNS
    fill_opcode	0ffh,07ah,opn_jp,OP_SHORTBRANCH,2,0,0		;JP
    fill_opcode	0ffh,07bh,opn_jnp,OP_SHORTBRANCH,2,0,0	;JNP
    fill_opcode	0ffh,07ch,opn_jl,OP_SHORTBRANCH,2,0,0		;JL
    fill_opcode	0ffh,07dh,opn_jge,OP_SHORTBRANCH,2,0,0	;JGE
    fill_opcode	0ffh,07eh,opn_jle,OP_SHORTBRANCH,2,0,0	;JLE
    fill_opcode	0ffh,07fh,opn_jg,OP_SHORTBRANCH,2,0,0		;JG
    fill_opcode	0,0,0,0,0,0,0					;terminator

group100xxxxx	EQU $
    fill_opcode	038fch,080h,opn_add,OP_RMIMMSIGNED,2,0,0	;ADD
    fill_opcode	038feh,0880h,opn_or,OP_RMIMMSIGNED,2,0,0	;OR
    fill_opcode	038fch,0880h,opn_or,OP_RMIMMSIGNED,2,0,0	;OR
    fill_opcode	038fch,01080h,opn_adc,OP_RMIMMSIGNED,2,0,0	;ADC
    fill_opcode	038fch,01880h,opn_sbb,OP_RMIMMSIGNED,2,0,0	;SBB
    fill_opcode	038feh,02080h,opn_and,OP_RMIMMSIGNED,2,0,0	;AND
    fill_opcode	038fch,02080h,opn_and,OP_RMIMMSIGNED,2,0,0	;AND
    fill_opcode	038fch,02880h,opn_sub,OP_RMIMMSIGNED,2,0,0	;SUB
    fill_opcode	038feh,03080h,opn_xor,OP_RMIMMSIGNED,2,0,0	;XOR
    fill_opcode	038fch,03080h,opn_xor,OP_RMIMMSIGNED,2,0,0	;XOR
    fill_opcode	038fch,03880h,opn_cmp,OP_RMIMMSIGNED,2,0,0	;CMP
    fill_opcode	0feh,084h,opn_test,OP_REGMOD,2,0,0		;TEST
    fill_opcode	0feh,086h,opn_xchg,OP_REGRM,2,0,0		;XCHG
    fill_opcode	0fch,088h,opn_mov,OP_REGRMREG,2,0,0		;MOV
    fill_opcode	020fdh,08ch,opn_mov,OP_SEGRMSEG,2,0,0		;MOV
    fill_opcode	0fdh,08ch,opn_mov,OP_SEGRMSEG,2,0,0		;MOV
    fill_opcode	0ffh,08dh,opn_lea,OP_WORDREGRM,2,0,0		;LEA
    fill_opcode	038ffh,08fh,opn_pop,OP_PUSHW,2,0,0		;POP
    fill_opcode	0ffh,090h,opn_nop,OP_CODEONLY,1,0,0		;NOP
    fill_opcode	0f8h,090h,opn_xchg,OP_ACCREG02,1,0,0		;XCHG
    fill_opcode	0ffh,098h,opn_cbw,OP_CBW,1,0,0		;CBW
        fill_opcode  0ffh,099h,opn_cwd,OP_CWD,1,0,0                ;CWD
    fill_opcode	0ffh,09ah,opn_call,OP_SEGBRANCH,5,0,0		;CALL
    fill_opcode	0ffh,09bh,opn_wait,OP_CODEONLY,1,0,0		;WAIT
    fill_opcode	0ffh,09ch,opn_pushf,OP_INSWORDSIZE,1,0,0	;PUSHF
    fill_opcode	0ffh,09dh,opn_popf,OP_INSWORDSIZE,1,0,0	;POPF
    fill_opcode	0ffh,09eh,opn_sahf,OP_CODEONLY,1,0,0		;SAHF
    fill_opcode	0ffh,09fh,opn_lahf,OP_CODEONLY,1,0,0		;LAHF
    fill_opcode	0,0,0,0,0,0,0					;terminator

group101xxxxx	EQU $
    fill_opcode	0feh,0a0h,opn_mov,OP_ACCABS,3,0,0		;MOV
    fill_opcode	0feh,0a2h,opn_mov,OP_ABSACC,3,0,0		;MOV
    fill_opcode	0ffh,0a5h,opn_movs,OP_INSWORDSIZE,1,0,0	;MOVS
    fill_opcode	0ffh,0a7h,opn_cmps,OP_INSWORDSIZE,1,0,0	;CMPS
    fill_opcode	0feh,0a8h,opn_test,OP_ACCIMM,1,0,0		;TEST
    fill_opcode	0ffh,0abh,opn_stos,OP_INSWORDSIZE,1,0,0	;STOS
    fill_opcode	0ffh,0adh,opn_lods,OP_INSWORDSIZE,1,0,0	;LODS
    fill_opcode	0ffh,0afh,opn_scas,OP_INSWORDSIZE,1,0,0	;SCAS
    fill_opcode	0ffh,0a4h,opn_movsb,OP_CODEONLY,1,0,0		;MOVSB
    fill_opcode	0ffh,0a6h,opn_cmpsb,OP_CODEONLY,1,0,0		;CMPSB
    fill_opcode	0ffh,0aah,opn_stosb,OP_CODEONLY,1,0,0		;STOSB
    fill_opcode	0ffh,0ach,opn_lodsb,OP_CODEONLY,1,0,0		;LODSB
    fill_opcode	0ffh,0aeh,opn_scasb,OP_CODEONLY,1,0,0		;SCASB
    fill_opcode	0f0h,0b0h,opn_mov,OP_ACCIMMB3,1,0,0		;MOV
    fill_opcode	0,0,0,0,0,0,0					;terminator

group110xxxxx	EQU $
    fill_opcode	038feh,0c0h,opn_rol,OP_RMSHIFT,2,1,0		;ROL
    fill_opcode	038feh,08c0h,opn_ror,OP_RMSHIFT,2,1,0		;ROR
    fill_opcode	038feh,010c0h,opn_rcl,OP_RMSHIFT,2,1,0	;RCL
    fill_opcode	038feh,018c0h,opn_rcr,OP_RMSHIFT,2,1,0	;RCR
    fill_opcode	038feh,020c0h,opn_shl,OP_RMSHIFT,2,1,0	;SHL
    fill_opcode	038feh,028c0h,opn_shr,OP_RMSHIFT,2,1,0	;SHR
    fill_opcode	038feh,038c0h,opn_sar,OP_RMSHIFT,2,1,0	;SAR
    fill_opcode	0ffh,0c2h,opn_ret,OP_RET,3,0,0		;RET
    fill_opcode	0ffh,0c3h,opn_ret,OP_CODEONLY,1,0,0		;RET
    fill_opcode	0ffh,0c4h,opn_les,OP_WORDREGRM,2,0,0		;LES
    fill_opcode	0ffh,0c5h,opn_lds,OP_WORDREGRM,2,0,0		;LDS
    fill_opcode	038feh,0c6h,opn_mov,OP_RMIMM,2,0,0		;MOV
    fill_opcode	0ffh,0c8h,opn_enter,OP_ENTER,4,1,0		;ENTER
    fill_opcode	0ffh,0cah,opn_retf,OP_RET,3,0,0		;RETF
    fill_opcode	0ffh,0cbh,opn_retf,OP_CODEONLY,1,0,0		;RETF
    fill_opcode	0ffh,0cch,opn_int,OP_INTR,1,0,0		;INT
    fill_opcode	0ffh,0cdh,opn_int,OP_INTR,2,0,0		;INT
    fill_opcode	0ffh,0ceh,opn_into,OP_CODEONLY,1,0,0		;INTO
    fill_opcode	0ffh,0cfh,opn_iret,OP_CODEONLY,1,0,0		;IRET
    fill_opcode	0ffh,0c9h,opn_leave,OP_CODEONLY,1,1,0		;LEAVE
    fill_opcode	038fch,0d0h,opn_rol,OP_RMSHIFT,2,0,0		;ROL
    fill_opcode	038fch,08d0h,opn_ror,OP_RMSHIFT,2,0,0		;ROR
    fill_opcode	038fch,010d0h,opn_rcl,OP_RMSHIFT,2,0,0	;RCL
    fill_opcode	038fch,018d0h,opn_rcr,OP_RMSHIFT,2,0,0	;RCR
    fill_opcode	038fch,020d0h,opn_shl,OP_RMSHIFT,2,0,0	;SHL
    fill_opcode	038fch,028d0h,opn_shr,OP_RMSHIFT,2,0,0	;SHR
    fill_opcode	038fch,038d0h,opn_sar,OP_RMSHIFT,2,0,0	;SAR
        fill_opcode  0ffh,0d4h,opn_aam,OP_AAM,1,0,0                ;AAM
        fill_opcode  0ffh,0d5h,opn_aad,OP_AAM,1,0,0                ;AAD
        fill_opcode  0ffh,0d6h,opn_salc,OP_CODEONLY,1,0,0          ;SALC
        fill_opcode  0ffh,0d7h,opn_xlat,OP_XLAT,1,0,0              ;XLAT
    fill_opcode	0,0,0,0,0,0,0

group111xxxxx	EQU $
        fill_opcode  0ffh,0e0h,opn_loopnz,OP_LOOP,2,0,0            ;LOOPNZ
        fill_opcode  0ffh,0e1h,opn_loopz,OP_LOOP,2,0,0             ;LOOPZ
        fill_opcode  0ffh,0e2h,opn_loop,OP_LOOP,2,0,0              ;LOOP
        fill_opcode  0ffh,0e3h,opn_jcxz,OP_LOOP,2,0,0              ;JCXZ
    fill_opcode	0feh,0e4h,opn_in,OP_PORTACCPORT,2,0,0		;IN
    fill_opcode	0feh,0e6h,opn_out,OP_PORTACCPORT,2,0,0	;OUT
    fill_opcode	0ffh,0e8h,opn_call,OP_BRANCH,3,0,0		;CALL
    fill_opcode	0ffh,0e9h,opn_jmp,OP_BRANCH,3,0,0		;JMP
    fill_opcode	0ffh,0eah,opn_jmp,OP_SEGBRANCH,5,0,0		;JMP
    fill_opcode	0ffh,0ebh,opn_jmp,OP_SHORTBRANCH,2,0,0	;JMP
    fill_opcode	0feh,0ech,opn_in,OP_ACCDX,1,0,0		;IN
    fill_opcode	0feh,0eeh,opn_out,OP_DXACC,1,0,0		;OUT
;        fill_opcode  0ffh,0f0h,opn_lock,OP_CODEONLY,1,0,0         ;LOCK
        fill_opcode  0ffh,0f1h,opn_icebp,OP_CODEONLY,1,0,0         ;INT1
        fill_opcode  0ffh,0f2h,opn_repnz,OP_CODEONLY,1,0,0         ;REPNZ
        fill_opcode  0ffh,0f3h,opn_repz,OP_CODEONLY,1,0,0          ;REPZ
    fill_opcode	0ffh,0f4h,opn_hlt,OP_CODEONLY,1,0,0		;HLT
    fill_opcode	0ffh,0f5h,opn_cmc,OP_CODEONLY,1,0,0		;CMC
    fill_opcode	038feh,0f6h,opn_test,OP_RMIMM,2,0,0		;TEST
    fill_opcode	038feh,010f6h,opn_not,OP_RM ,2,0,0		;NOT
    fill_opcode	038feh,018f6h,opn_neg,OP_RM ,2,0,0		;NEG
    fill_opcode	038feh,020f6h,opn_mul,OP_RM ,2,0,0		;MUL
    fill_opcode	038feh,028f6h,opn_imul,OP_RM ,2,0,0		;IMUL
    fill_opcode	038feh,030f6h,opn_div,OP_RM ,2,0,0		;DIV
    fill_opcode	038feh,038f6h,opn_idiv,OP_RM ,2,0,0		;IDIV
    fill_opcode	0ffh,0f8h,opn_clc,OP_CODEONLY,1,0,0		;CLC
    fill_opcode	0ffh,0f9h,opn_stc,OP_CODEONLY,1,0,0		;STC
    fill_opcode	0ffh,0fah,opn_cli,OP_CODEONLY,1,0,0		;CLI
    fill_opcode	0ffh,0fbh,opn_sti,OP_CODEONLY,1,0,0		;STI
    fill_opcode	0ffh,0fch,opn_cld,OP_CODEONLY,1,0,0		;CLD
    fill_opcode	0ffh,0fdh,opn_std,OP_CODEONLY,1,0,0		;STD
    fill_opcode	038feh,0feh,opn_inc,OP_RM ,2,0,0		;INC
    fill_opcode	038feh,08feh,opn_dec,OP_RM ,2,0,0		;DEC
    fill_opcode	038ffh,010ffh,opn_call,OP_RM ,2,0,0		;CALL
    fill_opcode	038ffh,018ffh,opn_call,OP_FARRM ,2,0,0	;CALL
    fill_opcode	038ffh,020ffh,opn_jmp,OP_RM ,2,0,0		;JMP
    fill_opcode	038ffh,028ffh,opn_jmp,OP_FARRM ,2,0,0		;JMP
    fill_opcode	038ffh,030ffh,opn_push,OP_PUSHW,2,0,0		;PUSH
    fill_opcode	0,0,0,0,0,0,0

groupx386	EQU $
        fill_opcode  0ffh,07,opn_loadall386,OP_CODEONLY,1,3,0         ;LOADALL
        fill_opcode  0ffh,05,opn_loadall286,OP_CODEONLY,1,3,0         ;LOADALL
    fill_opcode	0feh,0beh,opn_movsx,OP_MIXEDREGRM,2,3,0	;MOVSX
    fill_opcode	0feh,0b6h,opn_movzx,OP_MIXEDREGRM,2,3,0	;MOVZX
    fill_opcode	0f8h,0c8h,opn_bswap,OP_BSWAP,1,3,0		;BSWAP
    fill_opcode	0f7h,0a0h,opn_push,OP_SEG35,1,3,0		;PUSH
    fill_opcode	0f7h,0a1h,opn_pop,OP_SEG35,1,3,0		;POP
    fill_opcode	0ffh,0b4h,opn_lfs,OP_WORDREGRM,2,3,0		;LFS
    fill_opcode	0ffh,0b5h,opn_lgs,OP_WORDREGRM,2,3,0		;LGS
    fill_opcode	0ffh,0b2h,opn_lss,OP_WORDREGRM,2,3,0		;LSS
    fill_opcode	0ffh,06h,opn_clts,OP_CODEONLY,1,3,0		;CLTS
    fill_opcode	0ffh,08h,opn_invd,OP_CODEONLY,1,3,0		;INVD
    fill_opcode	0ffh,09h,opn_wbinvd,OP_CODEONLY,1,3,0		;WBINVD
    fill_opcode	0ffh,0afh,opn_imul,OP_WORDREGRM,2,3,0		;IMUL
    fill_opcode	0ffh,0a4h,opn_shld,OP_REGRMSHIFT,3,3,0	;SHLD
    fill_opcode	0ffh,0a5h,opn_shld,OP_REGRMSHIFT,2,3,0	;SHLD
    fill_opcode	0ffh,0ach,opn_shrd,OP_REGRMSHIFT,3,3,0	;SHRD
    fill_opcode	0ffh,0adh,opn_shrd,OP_REGRMSHIFT,2,3,0	;SHRD
    fill_opcode	0ffh,0bch,opn_bsf,OP_WORDREGRM,2,3,0		;BSF
    fill_opcode	0ffh,0bdh,opn_bsr,OP_WORDREGRM,2,3,0		;BSR
    fill_opcode	0ffh,0a3h,opn_bt,OP_WORDRMREG,2,3,0		;BT
    fill_opcode	0ffh,0abh,opn_bts,OP_WORDRMREG,2,3,0		;BTS
    fill_opcode	0ffh,0b3h,opn_btr,OP_WORDRMREG,2,3,0		;BTR
    fill_opcode	0ffh,0bbh,opn_btc,OP_WORDRMREG,2,3,0		;BTC
    fill_opcode	0ffh,040h,opn_cmovo,OP_WORDREGRM,2,3,0	;CMOVO
    fill_opcode	0ffh,041h,opn_cmovno,OP_WORDREGRM,2,3,0	;CMOVNO
    fill_opcode	0ffh,042h,opn_cmovb,OP_WORDREGRM,2,3,0	;CMOVB
    fill_opcode	0ffh,043h,opn_cmovnb,OP_WORDREGRM,2,3,0	;CMOVNB
    fill_opcode	0ffh,044h,opn_cmovz,OP_WORDREGRM,2,3,0	;CMOVZ
    fill_opcode	0ffh,045h,opn_cmovnz,OP_WORDREGRM,2,3,0	;CMOVNZ
    fill_opcode	0ffh,046h,opn_cmovbe,OP_WORDREGRM,2,3,0	;CMOVBE
    fill_opcode	0ffh,047h,opn_cmova,OP_WORDREGRM,2,3,0	;CMOVA
    fill_opcode	0ffh,048h,opn_cmovs,OP_WORDREGRM,2,3,0	;CMOVS
    fill_opcode	0ffh,049h,opn_cmovns,OP_WORDREGRM,2,3,0	;CMOVNS
    fill_opcode	0ffh,04ah,opn_cmovp,OP_WORDREGRM,2,3,0	;CMOVP
    fill_opcode	0ffh,04bh,opn_cmovnp,OP_WORDREGRM,2,3,0	;CMOVNP
    fill_opcode	0ffh,04ch,opn_cmovl,OP_WORDREGRM,2,3,0	;CMOVL
    fill_opcode	0ffh,04dh,opn_cmovge,OP_WORDREGRM,2,3,0	;CMOVGE
    fill_opcode	0ffh,04eh,opn_cmovle,OP_WORDREGRM,2,3,0	;CMOVLE
    fill_opcode	0ffh,04fh,opn_cmovg,OP_WORDREGRM,2,3,0	;CMOVG
    fill_opcode	038ffh,020bah,opn_bt,OP_BITNUM,3,3,0		;BT
    fill_opcode	038ffh,028bah,opn_bts,OP_BITNUM,3,3,0		;BTS
    fill_opcode	038ffh,030bah,opn_btr,OP_BITNUM,3,3,0		;BTR
    fill_opcode	038ffh,038bah,opn_btc,OP_BITNUM,3,3,0		;BTC
    fill_opcode	0ffh,0a7h,opn_ibts,OP_CODEONLY,1,3,0		;IBTS
    fill_opcode	0ffh,0aah,opn_rsm,OP_CODEONLY,1,3,0		;RSM
    fill_opcode	0ffh,0a6h,opn_xbts,OP_CODEONLY,1,3,0		;XBTS
    fill_opcode	0ffh,0a2h,opn_cpuid,OP_CODEONLY,1,3,0		;CPUID
    fill_opcode	0ffh,080h,opn_jo,OP_BRANCH,3,3,0		;JO
    fill_opcode	0ffh,081h,opn_jno,OP_BRANCH,3,3,0		;JNO
    fill_opcode	0ffh,082h,opn_jb,OP_BRANCH,3,3,0		;JB
    fill_opcode	0ffh,083h,opn_jnb,OP_BRANCH,3,3,0		;JNB
    fill_opcode	0ffh,084h,opn_jz,OP_BRANCH,3,3,0		;JZ
    fill_opcode	0ffh,085h,opn_jnz,OP_BRANCH,3,3,0		;JNZ
    fill_opcode	0ffh,086h,opn_jbe,OP_BRANCH,3,3,0		;JBE
    fill_opcode	0ffh,087h,opn_ja,OP_BRANCH,3,3,0		;JA
    fill_opcode	0ffh,088h,opn_js,OP_BRANCH,3,3,0		;JS
    fill_opcode	0ffh,089h,opn_jns,OP_BRANCH,3,3,0		;JNS
    fill_opcode	0ffh,08ah,opn_jp,OP_BRANCH,3,3,0		;JP
    fill_opcode	0ffh,08bh,opn_jnp,OP_BRANCH,3,3,0		;JNP
    fill_opcode	0ffh,08ch,opn_jl,OP_BRANCH,3,3,0		;JL
    fill_opcode	0ffh,08dh,opn_jge,OP_BRANCH,3,3,0		;JGE
    fill_opcode	0ffh,08eh,opn_jle,OP_BRANCH,3,3,0		;JLE
    fill_opcode	0ffh,08fh,opn_jg,OP_BRANCH,3,3,0		;JG
        fill_opcode  0ffh,031h,opn_rdtsc,OP_CODEONLY,1,3,0         ;RDTSC
        fill_opcode  0ffh,033h,opn_rdpmc,OP_CODEONLY,1,3,0         ;RDPMC
    fill_opcode	0ffh,032h,opn_rdmsr,OP_CODEONLY,1,3,0		;RDMSR
    fill_opcode	0ffh,030h,opn_wrmsr,OP_CODEONLY,1,3,0		;WRMSR
        fill_opcode  038ffh,000aeh,opn_fxsave,OP_FLOATNOPTR,2,3,0  ;FXSAVE
        fill_opcode  038ffh,008aeh,opn_fxrstor,OP_FLOATNOPTR,2,3,0  ;FXSAVE
        fill_opcode  038ffh,090h,opn_seto,OP_SETS ,2,3,0     ;SETO
        fill_opcode  038ffh,091h,opn_setno,OP_SETS ,2,3,0    ;SETNO
        fill_opcode  038ffh,092h,opn_setb,OP_SETS ,2,3,0     ;SETB
        fill_opcode  038ffh,093h,opn_setnb,OP_SETS ,2,3,0    ;SETNB
        fill_opcode  038ffh,094h,opn_setz,OP_SETS ,2,3,0     ;SETZ
        fill_opcode  038ffh,095h,opn_setnz,OP_SETS ,2,3,0    ;SETNZ
        fill_opcode  038ffh,096h,opn_setbe,OP_SETS ,2,3,0    ;SETBE
        fill_opcode  038ffh,097h,opn_seta,OP_SETS ,2,3,0     ;SETA
        fill_opcode  038ffh,098h,opn_sets,OP_SETS ,2,3,0     ;SETS
        fill_opcode  038ffh,099h,opn_setns,OP_SETS ,2,3,0    ;SETNS
        fill_opcode  038ffh,09ah,opn_setp,OP_SETS ,2,3,0     ;SETP
        fill_opcode  038ffh,09bh,opn_setnp,OP_SETS ,2,3,0    ;SETNP
        fill_opcode  038ffh,09ch,opn_setl,OP_SETS ,2,3,0     ;SETL
        fill_opcode  038ffh,09dh,opn_setge,OP_SETS ,2,3,0    ;SETGE
        fill_opcode  038ffh,09eh,opn_setle,OP_SETS ,2,3,0    ;SETLE
        fill_opcode  038ffh,09fh,opn_setg,OP_SETS ,2,3,0     ;SETG
    fill_opcode	0feh,0c0h,opn_xadd,OP_BYTERMREG,2,3,0		;XADD
    fill_opcode	0feh,0b0h,opn_cmpxchg,OP_BYTERMREG,2,3,0	;CMPXCHG
    fill_opcode	0c0fdh,0c020h,opn_mov,OP_386REG,2,3,0		;MOV
    fill_opcode	0c0fdh,0c021h,opn_mov,OP_386REG,2,3,0		;MOV
    fill_opcode	0c0fdh,0c024h,opn_mov,OP_386REG,2,3,0		;MOV
    fill_opcode	0ffh,00bh,opn_ud2,OP_CODEONLY,1,3,0		;UD2
    fill_opcode	0ffh,02h,opn_lar,OP_WORDREGRM,2,3,0		;LAR
    fill_opcode	0ffh,03h,opn_lsl,OP_WORDREGRM,2,3,0		;LSL
        fill_opcode  038ffh,01001h,opn_lgdt,OP_LGDT ,2,3,0         ;LGDT
    fill_opcode	038ffh,01801h,opn_lidt,OP_NOSTRICTRM ,2,3,0	;LIDT
    fill_opcode	038ffh,008c7h,opn_cmpxchg8b,OP_NOSTRICTRM,2,3,0 ;CMPXCHG8B
    fill_opcode	038ffh,01000h,opn_lldt,OP_NOSTRICTRM ,2,3,0	;LIDT
    fill_opcode	038ffh,03001h,opn_lmsw,OP_NOSTRICTRM ,2,3,0	;LMSW
    fill_opcode	038ffh,01800h,opn_ltr,OP_NOSTRICTRM ,2,3,0	;LTR
        fill_opcode  038ffh,01h,opn_sgdt,OP_LGDT ,2,3,0            ;SGDT
    fill_opcode	038ffh,0801h,opn_sidt,OP_NOSTRICTRM ,2,3,0	;SIDT
    fill_opcode	038ffh,00h,opn_sldt,OP_NOSTRICTRM ,2,3,0	;SLDT
    fill_opcode	038ffh,02001h,opn_smsw,OP_NOSTRICTRM ,2,3,0	;SMSW
    fill_opcode	038ffh,0800h,opn_str,OP_NOSTRICTRM ,2,3,0	;STR
    fill_opcode	038ffh,02000h,opn_verr,OP_NOSTRICTRM ,2,3,0	;VERR
    fill_opcode	038ffh,02800h,opn_verw,OP_NOSTRICTRM ,2,3,0	;VERW
;; mmx stuff here
        fill_opcode 0ffh,77h,opn_emms,OP_CODEONLY,1,3,0
        fill_opcode 0ffh,7eh,opn_movd,OP_MMXMOVDST,2,3,0
        fill_opcode 0ffh,6eh,opn_movd,OP_MMXMOVDLD,2,3,0
        fill_opcode 0ffh,7fh,opn_movq,OP_MMXMOVQST,2,3,0
        fill_opcode 0ffh,6fh,opn_movq,OP_MMXMOVQLD,2,3,0
        fill_opcode 0ffh,6bh,opn_packssdw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,63h,opn_packsswb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,67h,opn_packuswb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0fch,opn_paddb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0feh,opn_paddd,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0ech,opn_paddsb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0edh,opn_paddsw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0dch,opn_paddusb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0ddh,opn_paddusw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0fdh,opn_paddw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0dbh,opn_pand,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0dfh,opn_pandn,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,74h,opn_pcmpeqb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,76h,opn_pcmpeqd,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,75h,opn_pcmpeqw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,64h,opn_pcmpgtb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,66h,opn_pcmpgtd,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,65h,opn_pcmpgtw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0f5h,opn_pmaddwd,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0e5h,opn_pmulhw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0d5h,opn_pmullw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0ebh,opn_por,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0f2h,opn_pslld,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0f3h,opn_psllq,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0f1h,opn_psllw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0e2h,opn_psrad,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0e1h,opn_psraw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0d2h,opn_psrld,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0d3h,opn_psrlq,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0d1h,opn_psrlw,OP_MMXSTD,2,3,0
        fill_opcode 038ffh,03072h,opn_pslld,OP_MMXSHIFT,2,3,0
        fill_opcode 038ffh,03073h,opn_psllq,OP_MMXSHIFT,2,3,0
        fill_opcode 038ffh,03071h,opn_psllw,OP_MMXSHIFT,2,3,0
        fill_opcode 038ffh,02072h,opn_psrad,OP_MMXSHIFT,2,3,0
        fill_opcode 038ffh,02071h,opn_psraw,OP_MMXSHIFT,2,3,0
        fill_opcode 038ffh,01072h,opn_psrld,OP_MMXSHIFT,2,3,0
        fill_opcode 038ffh,01073h,opn_psrlq,OP_MMXSHIFT,2,3,0
        fill_opcode 038ffh,01071h,opn_psrlw,OP_MMXSHIFT,2,3,0
        fill_opcode 0ffh,0f8h,opn_psubb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0fah,opn_psubd,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0e8h,opn_psubsb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0e9h,opn_psubsw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0d8h,opn_psubusb,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0d9h,opn_psubusw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0f9h,opn_psubw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,68h,opn_punpckhbw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,69h,opn_punpckhwd,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,6ah,opn_punpckhdq,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,60h,opn_punpcklbw,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,61h,opn_punpcklwd,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,62h,opn_punpckldq,OP_MMXSTD,2,3,0
        fill_opcode 0ffh,0efh,opn_pxor,OP_MMXSTD,2,3,0

    fill_opcode	0,0,0,0,0,0,0

;/* single byte commands */

floats	EQU $
    fill_opcode	0ffffh,0d0d9h,opn_fnop,OP_CODEONLY,2,0,0	;FNOP
    fill_opcode	0ffffh,0e0d9h,opn_fchs,OP_CODEONLY,2,0,0	;FCHS
    fill_opcode	0ffffh,0e1d9h,opn_fabs,OP_CODEONLY,2,0,0	;FABS
    fill_opcode	0ffffh,0e4d9h,opn_ftst,OP_CODEONLY,2,0,0	;FTST
    fill_opcode	0ffffh,0e5d9h,opn_fxam,OP_CODEONLY,2,0,0	;FXAM
    fill_opcode	0ffffh,0e8d9h,opn_fld1,OP_CODEONLY,2,0,0	;FLD1
    fill_opcode	0ffffh,0e9d9h,opn_fldl2t,OP_CODEONLY,2,0,0	;FLDL2T
    fill_opcode	0ffffh,0ead9h,opn_fldl2e,OP_CODEONLY,2,0,0	;FLDL2E
    fill_opcode	0ffffh,0ebd9h,opn_fldpi,OP_CODEONLY,2,0,0	;FLDPI
    fill_opcode	0ffffh,0ecd9h,opn_fldlg2,OP_CODEONLY,2,0,0	;FLDLG2
    fill_opcode	0ffffh,0edd9h,opn_fldln2,OP_CODEONLY,2,0,0	;FLDLN2
    fill_opcode	0ffffh,0eed9h,opn_fldz,OP_CODEONLY,2,0,0	;FLDZ
    fill_opcode	0ffffh,0f0d9h,opn_f2xm1,OP_CODEONLY,2,0,0	;F2XM1
    fill_opcode	0ffffh,0f1d9h,opn_fyl2x,OP_CODEONLY,2,0,0	;FYL2X
    fill_opcode	0ffffh,0f2d9h,opn_fptan,OP_CODEONLY,2,0,0	;FPTAN
    fill_opcode	0ffffh,0f3d9h,opn_fpatan,OP_CODEONLY,2,0,0	;FPATAN
    fill_opcode	0ffffh,0f5d9h,opn_fprem1,OP_CODEONLY,2,1,0	;FPREM1
    fill_opcode	0ffffh,0f4d9h,opn_fxtract,OP_CODEONLY,2,0,0	;FXTRACT
    fill_opcode	0ffffh,0f6d9h,opn_fdecstp,OP_CODEONLY,2,0,0	;FDECSTP
    fill_opcode	0ffffh,0f7d9h,opn_fincstp,OP_CODEONLY,2,0,0	;FINCSTP
    fill_opcode	0ffffh,0f8d9h,opn_fprem,OP_CODEONLY,2,0,0	;FPREM
    fill_opcode	0ffffh,0f9d9h,opn_fyl2xp1,OP_CODEONLY,2,0,0	;FYL2XP1
    fill_opcode	0ffffh,0fad9h,opn_fsqrt,OP_CODEONLY,2,0,0	;FSQRT
    fill_opcode	0ffffh,0fbd9h,opn_fsincos,OP_CODEONLY,2,1,0	;FSINCOS
    fill_opcode	0ffffh,0fcd9h,opn_frndint,OP_CODEONLY,2,0,0	;FRNDINT
    fill_opcode	0ffffh,0fdd9h,opn_fscale,OP_CODEONLY,2,0,0	;FSCALE
    fill_opcode	0ffffh,0fed9h,opn_fsin,OP_CODEONLY,2,1,0	;FSIN
    fill_opcode	0ffffh,0ffd9h,opn_fcos,OP_CODEONLY,2,1,0	;FCOS
    fill_opcode	0ffffh,0e9dah,opn_fucompp,OP_CODEONLY,2,1,0	;FUCOMPP
    fill_opcode	0ffffh,0e0dbh,opn_feni,OP_CODEONLY,2,0,0	;FENI
    fill_opcode	0ffffh,0e1dbh,opn_fdisi,OP_CODEONLY,2,0,0	;FDISI
    fill_opcode	0ffffh,0e2dbh,opn_fclex,OP_CODEONLY,2,0,0	;FCLEX
    fill_opcode	0ffffh,0e3dbh,opn_finit,OP_CODEONLY,2,0,0	;FINIT
    fill_opcode	0ffffh,0e4dbh,opn_fsetpm,OP_CODEONLY,2,0,0	;FSETPM
    fill_opcode	0ffffh,0d9deh,opn_fcompp,OP_CODEONLY,2,0,0	;FCOMPP
    fill_opcode	0ffffh,0e0dfh,opn_fnstsw,OP_AX,2,0,0		;FNSTSW


;  /* Group 1, RM 3 */

    fill_opcode	0f8ffh,0c0d9h,opn_fld,OP_FLD,2,0,0		;FLD
    fill_opcode	0f8ffh,0c8d9h,opn_fxch,OP_FREGMAYBE,2,0,0	;FXCH
    fill_opcode	0f8fbh,0d8d9h,opn_fstp,OP_FLD,2,0,0		;FSTP
    fill_opcode	0c0ffh,0c0d9h,opn_esc,OP_ESC,2,0,0		;ESC


;  /* Group 1, RM0-2 */

    fill_opcode	038ffh,020d9h,opn_fldenv,OP_FLOATNOPTR,2,0,0	;FLDENV
    fill_opcode	038ffh,028d9h,opn_fldcw,OP_FLOATNOPTR,2,0,0	;FLDCW
    fill_opcode	038ffh,030d9h,opn_fnstenv,OP_FLOATNOPTR,2,0,0	;FNSTENV
    fill_opcode	038ffh,038d9h,opn_fnstcw,OP_FLOATNOPTR,2,0,0	;FNSTCW
    

;  /* Group 5, RM3 */

    fill_opcode	0f8ffh,0c0ddh,opn_ffree,OP_FREG,2,0,0		;FFREE
        fill_opcode  0f8ffh,0c8ddh,opn_fxch,OP_FREGMAYBE,2,0,0     ;FXCH
    fill_opcode	0f8ffh,0d0ddh,opn_fst,OP_FST,2,0,0		;FST
    fill_opcode	0f8ffh,0e0ddh,opn_fucom,OP_FREGMAYBE,2,1,0	;FUCOM
    fill_opcode	0f8ffh,0e8ddh,opn_fucomp,OP_FREGMAYBE,2,1,0	;FUCOMP

        fill_opcode  0f8ffh,0f0dbh,opn_fcomi,OP_FREG,2,1,0         ;FCOM
        fill_opcode  0f8ffh,0c0dfh,opn_ffreep,OP_FREG,2,0,0         ;FFREE
        fill_opcode  0f8ffh,0c8dfh,opn_fxch,OP_FREGMAYBE,2,0,0     ;FXCH
        fill_opcode  0f8ffh,0d0dfh,opn_fst,OP_FLD,2,0,0            ;FST
        fill_opcode  0f8ffh,0d8dfh,opn_fstp,OP_FLD,2,0,0           ;FSTP
        fill_opcode  0f8ffh,0f0dfh,opn_fcomip,OP_FREG,2,1,0        ;FCOM
        fill_opcode  0f8ffh,0e8dbh,opn_fucomi,OP_FREG,2,1,0        ;FCOM
        fill_opcode  0f8ffh,0e8dfh,opn_fucomip,OP_FREG,2,1,0       ;FCOM
        fill_opcode  0f8ffh,0c0dah,opn_fcmovb,OP_FREG,2,1,0        ;FCOM
        fill_opcode  0f8ffh,0d0dah,opn_fcmovbe,OP_FREG,2,1,0       ;FCOM
        fill_opcode  0f8ffh,0c8dah,opn_fcmove,OP_FREG,2,1,0        ;FCOM
        fill_opcode  0f8ffh,0c0dbh,opn_fcmovnb,OP_FREG,2,1,0       ;FCOM
        fill_opcode  0f8ffh,0d0dbh,opn_fcmovnbe,OP_FREG,2,1,0      ;FCOM
        fill_opcode  0f8ffh,0c8dbh,opn_fcmovne,OP_FREG,2,1,0       ;FCOM
        fill_opcode  0f8ffh,0d8dbh,opn_fcmovnu,OP_FREG,2,1,0       ;FCOM
        fill_opcode  0f8ffh,0d8dah,opn_fcmovu,OP_FREG,2,1,0        ;FCOM

        fill_opcode  0c0ffh,0c0ddh,opn_esc,OP_ESC,2,0,0            ;ESC


;  /* Group 5, RM0-2 */

    fill_opcode	038ffh,020ddh,opn_frstor,OP_FLOATNOPTR,2,0,0	;FRSTOR
    fill_opcode	038ffh,030ddh,opn_fnsave,OP_FLOATNOPTR,2,0,0	;FNSAVE
    fill_opcode	038ffh,038ddh,opn_fnstsw,OP_FLOATNOPTR,2,0,0	;FNSTSW


;  /* Group 3 & 7*/

    fill_opcode	0c0fbh,0c0dbh,opn_esc,OP_ESC,2,0,0		;ESC
    fill_opcode	038ffh,028dbh,opn_fld,OP_FLD,2,0,0		;FLD
    fill_opcode	038ffh,038dbh,opn_fstp,OP_FLD,2,0,0		;FSTP


;  /* Group 7 */

    fill_opcode	038ffh,020dfh,opn_fbld,OP_FBLD,2,0,0		;FBLD
    fill_opcode	038ffh,028dfh,opn_fild,OP_FILD,2,0,0		;FILD
    fill_opcode	038ffh,030dfh,opn_fbstp,OP_FBLD,2,0,0		;FBSTP
    fill_opcode	038ffh,038dfh,opn_fistp,OP_FIST,2,0,0		;FISTP


;  /* Math, group 0,2,4,6 special RM 3*/

    fill_opcode	0c0ffh,0c0dah,opn_esc,OP_ESC,2,0,0		;ESC
    fill_opcode	0f8ffh,0c0deh,opn_faddp,OP_FLOATMATHP,2,0,0	;FADDP
    fill_opcode	0f8ffh,0c8deh,opn_fmulp,OP_FLOATMATHP,2,0,0	;FMULP
    fill_opcode	0f8ffh,0d0deh,opn_fcomp,OP_FREG,2,0,0		;RCOMP
    fill_opcode	0f8ffh,0d8deh,opn_esc,OP_ESC,2,0,0		;ESC
    fill_opcode	0f8ffh,0e8deh,opn_fsubp,OP_FLOATMATHP,2,0,0	;FSUBP
    fill_opcode	0f8ffh,0f8deh,opn_fdivp,OP_FLOATMATHP,2,0,0	;FDIVP
    fill_opcode	0f8ffh,0e0deh,opn_fsubrp,OP_FLOATMATHP,2,0,0	;FSUBRP
    fill_opcode	0f8ffh,0f0deh,opn_fdivrp,OP_FLOATMATHP,2,0,0	;FDIVRP


;  /* Math, other */

    fill_opcode	038fbh,0d8h,opn_fadd,OP_FLOATMATH,2,0,0	;FADD
    fill_opcode	038fbh,08d8h,opn_fmul,OP_FLOATMATH,2,0,0	;FMUL
    fill_opcode	038fbh,010d8h,opn_fcom,OP_FLOATRM,2,0,0	;FCOM
    fill_opcode	038fbh,018d8h,opn_fcomp,OP_FLOATRM,2,0,0	;FCOMP
    fill_opcode	038ffh,020d8h,opn_fsub,OP_FLOATMATH,2,0,0	;FSUB
    fill_opcode	0f8ffh,0e8dch,opn_fsub,OP_FLOATMATH,2,0,0	;FSUB
    fill_opcode	038ffh,028dch,opn_fsubr,OP_FLOATMATH,2,0,0	;FSUBR
    fill_opcode	038ffh,028d8h,opn_fsubr,OP_FLOATMATH,2,0,0	;FSUBR
    fill_opcode	0f8ffh,0e0dch,opn_fsubr,OP_FLOATMATH,2,0,0	;FSUBR
    fill_opcode	038ffh,020dch,opn_fsub,OP_FLOATMATH,2,0,0	;FSUB
    fill_opcode	038ffh,030d8h,opn_fdiv,OP_FLOATMATH,2,0,0	;FDIV
    fill_opcode	0f8ffh,0f8dch,opn_fdiv,OP_FLOATMATH,2,0,0	;FDIV
    fill_opcode	038ffh,038dch,opn_fdivr,OP_FLOATMATH,2,0,0	;FDIVR
    fill_opcode	038ffh,038d8h,opn_fdivr,OP_FLOATMATH,2,0,0	;FDIVR
    fill_opcode	0f8ffh,0f0dch,opn_fdivr,OP_FLOATMATH,2,0,0	;FDIVR
    fill_opcode	038ffh,030dch,opn_fdiv,OP_FLOATMATH,2,0,0	;FDIV
    fill_opcode	038fbh,0dah,opn_fiadd,OP_FLOATIMATH,2,0,0	;FIADD
    fill_opcode	038fbh,08dah,opn_fimul,OP_FLOATIMATH,2,0,0	;FIMUL
    fill_opcode	038fbh,010dah,opn_ficom,OP_FLOATIMATH,2,0,0	;FICOM
    fill_opcode	038fbh,018dah,opn_ficomp,OP_FLOATIMATH,2,0,0	;FICOMP
    fill_opcode	038fbh,020dah,opn_fisub,OP_FLOATIMATH,2,0,0	;FISUB
    fill_opcode	038fbh,028dah,opn_fisubr,OP_FLOATIMATH,2,0,0	;FISUBR
    fill_opcode	038fbh,030dah,opn_fidiv,OP_FLOATIMATH,2,0,0	;FIDIV
    fill_opcode	038fbh,038dah,opn_fidivr,OP_FLOATIMATH,2,0,0	;FIDIVR


;  /* groups 1, 3, 5, 7 */
;  /* keep the follwing from going into error, RM3 */

    fill_opcode	0e0f9h,0c0d9h,opn_esc,OP_ESC,2,0,0		;ESC
    fill_opcode	038fbh,0d9h,opn_fld,OP_FLD,2,0,0		;FLD
    fill_opcode	038fbh,010d9h,opn_fst,OP_FST,2,0,0		;FST
    fill_opcode	038fbh,018d9h,opn_fstp,OP_FLD,2,0,0		;FSTP
    fill_opcode	038fbh,0dbh,opn_fild,OP_FILD,2,0,0		;FILD
    fill_opcode	038fbh,010dbh,opn_fist,OP_FIST,2,0,0		;FIST
    fill_opcode	038fbh,018dbh,opn_fistp,OP_FILD,2,0,0		;FISTP


;  /* Catch- all */

    fill_opcode	0f8h,0d8h,opn_esc,OP_ESC,2,0,0		;ESC
    fill_opcode	0,0,0,0,0,0,0
;
; now comes a table used only by the assembler for various aliased
; names
;
synonymTable	EQU $

        fill_opcode  0ffh,07,opn_loadall,OP_CODEONLY,1,3,0         ;LOADALL
        fill_opcode  0ffh,0d6h,opn_setalc,OP_CODEONLY,1,0,0        ;SALC
    fill_opcode	0ffh,0d7h,opn_xlatb,OP_CODEONLY,1,0,0		;XLATB
        fill_opcode  0ffh,0e0h,opn_loopne,OP_LOOP,2,0,0            ;LOOPNE
        fill_opcode  0ffh,0e1h,opn_loope,OP_LOOP,2,0,0             ;LOOPE
        fill_opcode  0ffh,0e0h,opn_loopnew,OP_LOOP,2,0,0           ;LOOPNE
        fill_opcode  0ffh,0e1h,opn_loopew,OP_LOOP,2,0,0            ;LOOPE
        fill_opcode  0ffh,0e0h,opn_loopned,OP_LOOP,2,0,0           ;LOOPNE
        fill_opcode  0ffh,0e1h,opn_looped,OP_LOOP,2,0,0            ;LOOPE
        fill_opcode  0ffh,0e0h,opn_loopnzw,OP_LOOP,2,0,0           ;LOOPNE
        fill_opcode  0ffh,0e1h,opn_loopzw,OP_LOOP,2,0,0            ;LOOPE
        fill_opcode  0ffh,0e0h,opn_loopnzd,OP_LOOP,2,0,0           ;LOOPNE
        fill_opcode  0ffh,0e1h,opn_loopzd,OP_LOOP,2,0,0            ;LOOPE
        fill_opcode  0ffh,0e2h,opn_loopw,OP_LOOP,2,0,0             ;LOOPE
        fill_opcode  0ffh,0e2h,opn_loopd,OP_LOOP,2,0,0             ;LOOPE
        fill_opcode  0ffh,0e3h,opn_jcxzw,OP_LOOP,2,0,0             ;JCXZ
        fill_opcode  0ffh,0e3h,opn_jcxzd,OP_LOOP,2,0,0             ;JCXZ
        fill_opcode  0ffh,0e3h,opn_jecxz,OP_LOOP,2,0,0             ;JECXZ
        fill_opcode  0ffh,060h,opn_pushad,OP_INSWORDSIZE,1,1,0     ;PUSHAD
        fill_opcode  0ffh,061h,opn_popad,OP_INSWORDSIZE,1,1,0      ;POPAD
        fill_opcode  0ffh,09ch,opn_pushfd,OP_INSWORDSIZE,1,0,0     ;PUSHFD
        fill_opcode  0ffh,09dh,opn_popfd,OP_INSWORDSIZE,1,0,0      ;POPFD
        fill_opcode  0ffh,060h,opn_pushaw,OP_INSWORDSIZE,1,1,0        ;PUSHAW
        fill_opcode  0ffh,061h,opn_popaw,OP_INSWORDSIZE,1,1,0         ;POPAW
        fill_opcode  0ffh,09ch,opn_pushfw,OP_INSWORDSIZE,1,0,0        ;PUSHFW
        fill_opcode  0ffh,09dh,opn_popfw,OP_INSWORDSIZE,1,0,0         ;POPFW
        fill_opcode  0ffh,0a5h,opn_movsw,OP_INSWORDSIZE,1,0,0         ;MOVSW
        fill_opcode  0ffh,0a7h,opn_cmpsw,OP_INSWORDSIZE,1,0,0         ;CMPSW
        fill_opcode  0ffh,0abh,opn_stosw,OP_INSWORDSIZE,1,0,0         ;STOSW
        fill_opcode  0ffh,0adh,opn_lodsw,OP_INSWORDSIZE,1,0,0         ;LODSW
        fill_opcode  0ffh,0afh,opn_scasw,OP_INSWORDSIZE,1,0,0         ;SCASW
        fill_opcode  0ffh,0a5h,opn_movsd,OP_INSWORDSIZE,1,0,0      ;MOVSD
        fill_opcode  0ffh,0a7h,opn_cmpsd,OP_INSWORDSIZE,1,0,0      ;CMPSD
        fill_opcode  0ffh,0abh,opn_stosd,OP_INSWORDSIZE,1,0,0      ;STOSD
        fill_opcode  0ffh,0adh,opn_lodsd,OP_INSWORDSIZE,1,0,0      ;LODSD
        fill_opcode  0ffh,0afh,opn_scasd,OP_INSWORDSIZE,1,0,0      ;SCASD
        fill_opcode  0ffh,06dh,opn_insw,OP_INSWORDSIZE,1,1,0          ;INSW
        fill_opcode  0ffh,06dh,opn_insd,OP_INSWORDSIZE,1,1,0       ;INSD
        fill_opcode  0ffh,06fh,opn_outsw,OP_INSWORDSIZE,1,1,0         ;OUTSW
        fill_opcode  0ffh,06fh,opn_outsd,OP_INSWORDSIZE,1,1,0      ;OUTSD
    fill_opcode	0ffh,072h,opn_jc,OP_SHORTBRANCH,2,0,0		;JC
    fill_opcode	0ffh,082h,opn_jc,OP_BRANCH,2,3,0		;JC
    fill_opcode	0ffh,072h,opn_jnae,OP_SHORTBRANCH,2,0,0	;JNAE
    fill_opcode	0ffh,082h,opn_jnae,OP_BRANCH,2,3,0		;JNAE
    fill_opcode	0ffh,073h,opn_jnc,OP_SHORTBRANCH,2,0,0	;JNC
    fill_opcode	0ffh,083h,opn_jnc,OP_BRANCH,2,3,0		;JNC
    fill_opcode	0ffh,073h,opn_jae,OP_SHORTBRANCH,2,0,0	;JAE
    fill_opcode	0ffh,083h,opn_jae,OP_BRANCH,2,3,0		;JAE
    fill_opcode	0ffh,074h,opn_je,OP_SHORTBRANCH,2,0,0		;JE
    fill_opcode	0ffh,084h,opn_je,OP_BRANCH,2,3,0		;JE
    fill_opcode	0ffh,075h,opn_jne,OP_SHORTBRANCH,2,0,0	;JNE
    fill_opcode	0ffh,085h,opn_jne,OP_BRANCH,2,3,0		;JNE
    fill_opcode	0ffh,076h,opn_jna,OP_SHORTBRANCH,2,0,0	;JNA
    fill_opcode	0ffh,086h,opn_jna,OP_BRANCH,2,3,0		;JNA
    fill_opcode	0ffh,077h,opn_jnbe,OP_SHORTBRANCH,2,0,0	;JNBE
    fill_opcode	0ffh,087h,opn_jnbe,OP_BRANCH,2,3,0		;JNBE
    fill_opcode	0ffh,07ah,opn_jpe,OP_SHORTBRANCH,2,0,0	;JPE
    fill_opcode	0ffh,08ah,opn_jpe,OP_BRANCH,2,3,0		;JPE
    fill_opcode	0ffh,07bh,opn_jpo,OP_SHORTBRANCH,2,0,0	;JPO
    fill_opcode	0ffh,08bh,opn_jpo,OP_BRANCH,2,3,0		;JPO
    fill_opcode	0ffh,07ch,opn_jnge,OP_SHORTBRANCH,2,0,0	;JNGE
    fill_opcode	0ffh,08ch,opn_jnge,OP_BRANCH,2,3,0		;JNGE
    fill_opcode	0ffh,07dh,opn_jnl,OP_SHORTBRANCH,2,0,0	;JNL
    fill_opcode	0ffh,08dh,opn_jnl,OP_BRANCH,2,3,0		;JNL
    fill_opcode	0ffh,07eh,opn_jng,OP_SHORTBRANCH,2,0,0	;JNG
    fill_opcode	0ffh,08eh,opn_jng,OP_BRANCH,2,3,0		;JNG
    fill_opcode	0ffh,07fh,opn_jnle,OP_SHORTBRANCH,2,0,0	;JNLE
    fill_opcode	0ffh,08fh,opn_jnle,OP_BRANCH,2,3,0		;JNLE
        fill_opcode  038ffh,092h,opn_setnae,OP_SETS ,2,3,0   ;SETNAE
        fill_opcode  038ffh,092h,opn_setc,OP_SETS ,2,3,0     ;SETC
        fill_opcode  038ffh,093h,opn_setnc,OP_SETS ,2,3,0    ;SETNC
        fill_opcode  038ffh,093h,opn_setae,OP_SETS ,2,3,0    ;SETAE
        fill_opcode  038ffh,094h,opn_sete,OP_SETS ,2,3,0     ;SETE
        fill_opcode  038ffh,095h,opn_setne,OP_SETS ,2,3,0    ;SETNE
        fill_opcode  038ffh,096h,opn_setna,OP_SETS ,2,3,0    ;SETNA
        fill_opcode  038ffh,097h,opn_setnbe,OP_SETS ,2,3,0   ;SETNBE
        fill_opcode  038ffh,09ah,opn_setpe,OP_SETS ,2,3,0    ;SETPE
        fill_opcode  038ffh,09bh,opn_setpo,OP_SETS ,2,3,0    ;SETPO
        fill_opcode  038ffh,09ch,opn_setnge,OP_SETS ,2,3,0   ;SETNGE
        fill_opcode  038ffh,09dh,opn_setnl,OP_SETS ,2,3,0    ;SETNL
        fill_opcode  038ffh,09eh,opn_setng,OP_SETS ,2,3,0    ;SETNG
        fill_opcode  038ffh,09fh,opn_setnle,OP_SETS ,2,3,0   ;SETNLE
    fill_opcode	0ffh,098h,opn_cwde,OP_OPSIZE1BYTE,1,0,0	;CWDE
    fill_opcode	0ffh,099h,opn_cdq,OP_OPSIZE1BYTE,1,0,0	;CDQ
    fill_opcode	0ffh,09bh,opn_fwait,OP_CODEONLY,1,0,0		;FWAIT
    fill_opcode	0ffffh,00d1d8h,opn_fcom,OP_CODEONLY,2,0,0	;FCOM
    fill_opcode	0ffffh,00d9d8h,opn_fcomp,OP_CODEONLY,2,0,0	;FCOMP
    fill_opcode	0ffffh,00e1ddh,opn_fucom,OP_CODEONLY,2,0,0	;FUCOM
    fill_opcode	0ffffh,00e9ddh,opn_fucomp,OP_CODEONLY,2,0,0	;FUCOMP
    fill_opcode	0ffffh,00c9d9h,opn_fxch,OP_CODEONLY,2,0,0	;FXCH
        fill_opcode  0ffh,0f1h,opn_int,OP_INTR,1,0,0               ;INT1
    fill_opcode	0,0,0,0,0,0,0


indexes dd group000xxxxx, group001xxxxx, group010xxxxx, group011xxxxx
    dd group100xxxxx, group101xxxxx, group110xxxxx, group111xxxxx

mnemonicMatchCount	dd	0
mnemonicMatchAddrTable	times	16 [dd 0]
;
    segment code USE32
;
; find an opcode. FS:SI points to the current byte of code, which is assumed 
; to be the first byte of this opcode.
;  Oops, actually SI points to the first byte of the fill_opcode PAST any 
;  override bytes
;OUTPUT: AX contains the fill_opcode, BX points to the fill_opcode structure for it
;	Apparently there is an option, which can be disabled, to permit
;	disassembling instructions found on the 386 and above. So if we find
;	an instruction and THEN find that this option is disabled, we
;	decide that we didn't find the instruction after all.
;	CY if not found
;
FindOpcode
    mov	byte [x86pfx],0
    mov	ebx,groupx386	; Assume it is an 0F fill_opcode
    inc	esi			; Point to next byte
    cmp	byte [esi-1],0fh	; Is it 0F?
    jnz	short not0ftable  	; No, try another table
    or	byte [x86pfx],1		; yes, flag as a prefix byte
    jmp	gotable
not0ftable:
    mov	byte [x86pfx],0		; not the 0f prefix table
    dec	esi			; Else point back to first byte
    mov	ebx,floats	; Assume floating
    movzx	eax,byte [esi]	; Get the fill_opcode
    and	al,0f8h			; Apply the FLOAT mask
    cmp	al,0d8h			; Apply FLOAT compare
    jz	short gotable		; Yes, go look for fill_opcode
    shr	al,5			; get top 3 bits of fill_opcode
    mov	ebx,[eax * 4 + indexes]	; use as WORD table index
gotable:
    test	word [ebx],-1	; See if at end of table
    jz	short noentry		; Yes, not found
    mov	ax,[esi]		; Get the fill_opcode
    and	ax,[ebx + opcode.msk]	; Mask it
    cmp	ax,[ebx + opcode.compare]; Compare with the compare value
    jz	short gotentry		; Quit if found
    add	ebx,OPCODESIZE		; Else go to next entry
    jmp	gotable			;
gotentry:
;	test	[Disassemble32Bit],1	;are 386+ instructions supported?
;	jnz	okentry			;if so, we got it
;	test	[ebx + opcode.FLAGS],1	;else report not found
;	jnz	noentry
okentry:
    clc				; Found, exit
    ret
noentry:
    stc				; Not found, exit
    ret

;
; scan a table for matches with this fill_opcode
; INPUT:Well, let's see...
;	SI appears to point to a mnemonic for an fill_opcode, somewere in memory
;	DI appears to point to the beginning of a table of fill_opcode structures
;	AX appears to contain the length of the mnemonic string at [si]
;	BX appears to contain some index, but the syntax confuses me.
;		for mnemonicMatchAddrTable, it is some even value which 
;			cannot exceed 1Eh or else mnemonicMatchAddrTable will 
;			overflow
;		for opcode.flags, it is apparently an from the top
;			of the data segment to the start of this fill_opcode 
;			structure. It appears to hold BOTH OF THESE AT ONCE! 
;			HOW?
;		My best guess is that BX comes in as 0
;PROCESSING:
;	Compare the string pointed to by DS:SI with the mnemonic pointed
;	to by the current fill_opcode structure pointed to in ES:DI. Each time
;	we get a string match, we store the address of the structure
;	containing the pointer to the matching string in a table.  No table
;	of fill_opcode structures better have more than 10h matches, since there
;	is no check for this.
;
;OUTPUT: The mnemonicMatchAddrTable table contains some number of addresses 
;	of fill_opcode structures, possibly 0.  My best guess is that the number 
;	of validentries in this table is found in (BX-2) SHR 1
;
;	
findMnemonicMatches:
    test	word [edi],-1	; end of table?
    jz	nomorent
    mov	ecx,eax			; get len passed in AX
    push	edi			;save pointer to structure
    mov	edi,[edi+opcode.mnemonic]	; get mnemonic from structure
    push	esi			;save pointer to found mnemonic
    repe	cmpsb			; scan for match
    pop	esi			;restore pointers
    pop	edi
    jnz	nomatch			; no match?
;	test	[Disassemble32Bit],1	; yes match, check for 386 match
;	jnz	okentrymatch
;	test	[edi + opcode.FLAGS],1	;see if 386+ instruction
;	jnz	nomatch			; not allowing 386 fill_opcodes
okentrymatch:
    mov	[ebx+mnemonicMatchAddrTable],edi	; match, put it in match tab
    add	bx,4
nomatch:
    add	edi,OPCODESIZE		; next entry
    jmp	findMnemonicMatches
nomorent:
    ret
;
; main fill_opcode lookup outine
; INPUT: By implication at findMnemonicMatches, AX must contain the length of
;	the string being parsed, and SI points to a mnemonic string.
;	These registers must be preserved until at least after the last
;	call to findMnemonicMatches.
; OUTPUT: mnemonicMatCount contains the number of valid matches
;	mnemonicMatchAddrTable contains near pointers to the fill_opcode
;	structures where we ac`tually found the matches to the names
;
LookupOpName:
    sub	ebx,ebx			;initial match count = 0
    mov	ecx,8			;look through 8 tables
    mov	edi,indexes	; match the main tabs
lp:
    push	edi			;save pointer to indexes
    push	ecx			;save table count
    mov	edi,[edi]			;find next table
    call	findMnemonicMatches 	;get match list
    pop	ecx			;restore loop count
    pop	edi			;and index pointer
    add	edi,4			;goto next table pointer
    loop	lp			;for all tables

    mov	edi,floats	; match the float tab
    call	findMnemonicMatches   	;and seach floats too
    mov	edi,groupx386	; match the x386 tab (0F prefix)
    call	findMnemonicMatches   	;search that for matches
    mov	edi,synonymTable	; match the assembly extras tab
    call	findMnemonicMatches   	;search synonyms

    shr	ebx,2			;restore count of finds
    mov	[mnemonicMatchCount],ebx	; and save count
    jnz	lox			;we found at least one match
    stc				; no match, error
lox:
    ret				; back to caller
