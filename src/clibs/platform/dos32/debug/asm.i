;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
; ASM.ASM
;
; Function: Assembler parser
;
; not very efficient, but, fast enough!
;


;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	AM_NONE    EQU  0		;no mode, like [44h], or no arg, like CLI
	AM_REG	   EQU  1		;register, like AX
	AM_BASED   EQU  2		;based, as in [bx+label]
	AM_MEM	   EQU  3		;memory, as in byte ptr [44h], or label
	AM_IMM	   EQU  4		;immediate, as in mov ax,5
	AM_SEGOFFS  EQU  5		;seg:ofs, as in es:[44h]
	AM_CR	 EQU  6		;control register used
	AM_DR	 EQU  7		;debug register used
	AM_TR	 EQU  8		;test register used
	AM_FPREG  EQU  9		;floating point register used
	AM_SEG	 EQU  10		;segment, maybe as in mov ax,cs
  AM_MMX  EQU  11   ; MMX register used
;These equates apply to areg1 and areg2. areg1 is a simple index register,
;like [bx+nnn], areg2 is for two cases: 1) two index registers used, as
;in [bx+si+nnn], and 2) scaled registers, like [ebx+eax*4+nnn]
;  In case 2), the scale applied to areg2 is in ascale

	isEAX	equ	0	;reference to this register in areg1,2
	isECX	equ	1
	isEDX	equ	2
	isEBX	equ	3
	isESP	equ	4
	isEBP	equ	5
	isESI	equ	6
	isEDI	equ	7
	TIMES1	equ	1
	TIMES2	equ	2
	TIMES4	equ	4
	TIMES8	equ	8
	NOSIZE		equ	0
	BYTESIZE	equ	1
	WORDSIZE	equ	2
	DWORDSIZE	equ	4
  PBYTESIZE equ 5
	FWORDSIZE	equ	6
	QWORDSIZE	equ	8
	TBYTESIZE	equ	10
	BYTEMODE	equ	1
	WORDMODE	equ	2
	DWORDMODE	equ	4
struc asmop
	.addrx	resd	1	;address of offset (or seg for segmented jump)
	.addrx2	resd	1	;offset for segmented jump if addrx is segment
	.mode	resb	1	;AM flag showing addressing mode

	.areg1	resb	1	;base register
	.areg2	resb	1	;index register (pertains to scale)

	.ascale	resw	1	;scale of areg2
	.asize	resb	1	;size of memory/register 1 EQU byte, 10-tbyte, etc
	.msize	resb	1	;addressing mode size 2 EQU 16-bit, 4 EQU 32-bit
endstruc

ASMOPSIZE	EQU 15
TRUE	equ	1


;These next flags get put in the mode field


;These values are stored in RepPfxBitmap, and indicate which one of these
;prefixes was identified

AF_REP	 EQU  1
AF_REPNE  EQU  2
AF_REPE   EQU  4
AF_LOCK   EQU  8

;These are keyed to offsets of segment overrides (or 66h,67h overrides)
;in the string of possible override strings.  They work in reverse, so
;that AS is the first two chars of the string, OS is next, then GS etc.

AS_ES  EQU  	01h
AS_CS  EQU  	02h
AS_SS  EQU  	04h
AS_DS  EQU  	08h
AS_FS  EQU  	10h
AS_GS  EQU  	20h
AS_OPSIZE  EQU    0c0h
AS_ADDRSIZE  EQU   300h
AS_OPSIZESET  EQU  40h
AS_OPSIZETEST  EQU  80h
AS_ADDRSIZESET  EQU  100h
AS_ADDRSIZETEST  EQU  200h

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
OP_CODEONLY EQU 0
OP_WREG02 EQU 1
OP_ACCREG02 EQU 2
OP_SEG35 EQU 3
OP_REGRMREG EQU 4
OP_RM EQU 5
OP_RMSHIFT EQU 6
OP_REGRM EQU 7
OP_WORDREGRM EQU 8
OP_INTR EQU 9
OP_SHORTBRANCH EQU 10
OP_RMIMM EQU 11
OP_ACCIMM EQU 12
OP_ABSACC EQU 13
OP_RMIMMSIGNED EQU 14
OP_ACCIMMB3 EQU 15
OP_SEGRMSEG EQU 16
OP_RET EQU 17
OP_SEGBRANCH EQU 18
OP_ESC EQU 19
OP_BRANCH EQU 20
OP_ACCDX EQU 21
OP_DXACC EQU 22
OP_PORTACCPORT EQU 23
OP_ACCABS EQU 24
OP_IMM EQU 25
OP_ENTER EQU 26
OP_INSWORDSIZE EQU 27
OP_REGMOD EQU 28
OP_NOSTRICTRM EQU 29
OP_RMSHIFTB3 EQU 30
OP_IMUL EQU 31
OP_386REG EQU 32
OP_REGRMSHIFT EQU 33
OP_PUSHW EQU 34
OP_FLOATRM EQU 35
OP_FLOATMATHP EQU 36
OP_FLOATMATH EQU 37
OP_FARRM EQU 38
OP_WORDRMREG EQU 39
OP_RMREG EQU 40
OP_BITNUM EQU 41
OP_MIXEDREGRM EQU 42
OP_CBW EQU 43
OP_REG02 EQU 44
OP_BYTERMREG EQU 45
OP_FLOATNOPTR EQU 46
OP_AX	EQU 47
OP_BSWAP EQU 48
OP_FST EQU 49
OP_FLD EQU 50
OP_FBLD EQU 51
OP_FILD EQU 52
OP_FIST EQU 53
OP_FREG EQU 54
OP_FREGMAYBE EQU 55
OP_FLOATIMATH EQU 56
OP_ADDRSIZE1BYTE EQU 57 ; assembler only
OP_OPSIZE1BYTE EQU 58	; same
OP_MMXSTD EQU 59
OP_MMXMOVDLD EQU 60
OP_MMXMOVDST EQU 61
OP_MMXMOVQLD EQU 62
OP_MMXMOVQST EQU 63
OP_MMXSHIFT EQU 64
OP_LOOP EQU 65
OP_CWD EQU 66
OP_AAM EQU 67
OP_XLAT EQU 68
OP_SETS EQU 69
OP_LGDT EQU 70
 
struc opcode
	.msk		resw	1	;mask value for opcode
	.compare	resw	1	;opcode itself after masking
	.mnemonic	resd	1	;pointer to mnemonic string
	.operands	resb	1	;addressing mode
	.oclength	resb	1	;base length of instruction
	.flags		resb	1
	.opcd		resb	1	;not used right now
endstruc


x386_OPCODE	equ	1
prefix0F	equ	2
OPCODESIZE EQU 12			; size of opcode struct

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern	aop0
	extern	aop1
	extern	aop2
	extern	aop3
	extern	aop4
	extern	aop5
	extern	aop6
	extern	aop7
	extern	aop8
	extern	aop9
	extern	aop10 
	extern	aop11
	extern	aop12
	extern	aop13
	extern	aop14
	extern	aop15
	extern	aop16
	extern	aop17
	extern	aop18
	extern	aop19
	extern	aop20 
	extern	aop21
	extern	aop22
	extern	aop23
	extern	aop24
	extern	aop25
	extern	aop26
	extern	aop27
	extern	aop28
	extern	aop29
	extern	aop30 
	extern	aop31
	extern	aop32
	extern	aop33
	extern	aop34
	extern	aop35
	extern	aop36
	extern	aop37
	extern	aop38
	extern	aop39
	extern	aop40 
	extern	aop41
	extern	aop42
	extern	aop43
	extern	aop44
	extern	aop45
	extern	aop46 
	extern	aop47
	extern	aop48
	extern	aop49
	extern	aop50
	extern	aop51
	extern	aop52
	extern	aop53
	extern	aop54
	extern	aop55
	extern	aop56
	extern	aop57
	extern	aop58
  extern aop59
  extern aop60
  extern aop61
  extern aop62
  extern aop63
  extern aop64
  extern aop65
  extern aop66
  extern aop67
  extern aop68
  extern aop69
  extern aop70

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern	FindOpcode
	extern	LookupOpName
	extern	mnemonicMatchCount
	extern	mnemonicMatchAddrTable

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern	opn_cwde
	extern	opn_cdq
	extern	opn_add
	extern	opn_push
	extern	opn_pop
	extern	opn_or
	extern	opn_adc
	extern	opn_sbb
	extern	opn_and
	extern	opn_daa
	extern	opn_sub
	extern	opn_das
	extern	opn_xor
	extern	opn_aaa
	extern	opn_cmp
	extern	opn_aas
	extern	opn_inc
	extern	opn_dec
	extern	opn_pusha
	extern	opn_popa
	extern	opn_pushad
	extern	opn_popad
  extern opn_pushaw
  extern opn_popaw
	extern	opn_bound
	extern	opn_arpl
	extern	opn_imul
	extern	opn_insb
	extern	opn_ins
	extern	opn_outsb
	extern	opn_outs
	extern	opn_insd
	extern	opn_outsd
	extern	opn_insw
	extern	opn_outsw
	extern	opn_jo
	extern	opn_jno
	extern	opn_jc
	extern	opn_jnae
	extern	opn_jnc
	extern	opn_jna
	extern	opn_jnbe
	extern	opn_jae
	extern	opn_je
	extern	opn_jne
	extern	opn_jpe
	extern	opn_jpo
	extern	opn_jnle
	extern	opn_jnl
	extern	opn_jnge
	extern	opn_jng
	extern	opn_jb
	extern	opn_jnb
	extern	opn_jz
	extern	opn_jnz
	extern	opn_jbe
	extern	opn_ja
	extern	opn_js
	extern	opn_jns
	extern	opn_jp
	extern	opn_jnp
	extern	opn_jl
	extern	opn_jge
	extern	opn_jle
	extern	opn_jg
	extern	opn_test
	extern	opn_xchg
	extern	opn_mov
	extern	opn_lea
  extern opn_loadall
  extern opn_loadall386
  extern opn_loadall286
	extern	opn_nop
	extern	opn_cbw
	extern	opn_cwd
	extern	opn_call
	extern	opn_wait
	extern	opn_fwait
	extern	opn_pushf
	extern	opn_popf
	extern	opn_pushfd
	extern	opn_popfd
  extern opn_pushfw
  extern opn_popfw
	extern	opn_sahf
	extern	opn_lahf
	extern	opn_movs
	extern	opn_cmps
	extern	opn_stos
	extern	opn_lods
	extern	opn_scas
	extern	opn_movsb
	extern	opn_cmpsb
	extern	opn_stosb
	extern	opn_lodsb
	extern	opn_scasb
	extern	opn_movsw
	extern	opn_cmpsw
	extern	opn_stosw
	extern	opn_lodsw
	extern	opn_scasw
	extern	opn_movsd
	extern	opn_cmpsd
	extern	opn_stosd
	extern	opn_lodsd
	extern	opn_scasd
	extern	opn_rol
	extern	opn_ror
	extern	opn_rcl
	extern	opn_rcr
	extern	opn_shl
	extern	opn_shr
	extern	opn_sar
	extern	opn_ret
	extern	opn_les
	extern	opn_lds
	extern	opn_enter
	extern	opn_retf
	extern	opn_int
  extern opn_icebp
	extern	opn_into
	extern	opn_iret
	extern	opn_leave
	extern	opn_aam
	extern	opn_aad
	extern	opn_xlat
	extern	opn_xlatb
	extern	opn_loopnz
	extern	opn_loopz
	extern	opn_loopne
	extern	opn_loope
	extern	opn_loop
  extern opn_loopnzw
  extern opn_loopzw
  extern opn_loopnew
  extern opn_loopew
  extern opn_loopw
  extern opn_loopnzd
  extern opn_loopzd
  extern opn_loopned
  extern opn_looped
  extern opn_loopd
	extern	opn_jcxz
  extern opn_jcxzw
  extern opn_jcxzd
  extern opn_jecxz
	extern	opn_in
	extern	opn_out
	extern	opn_jmp
	extern	opn_lock
	extern	opn_repnz
	extern	opn_repz
	extern	opn_hlt
	extern	opn_cmc
	extern	opn_not
	extern	opn_neg
	extern	opn_mul
	extern	opn_div
	extern	opn_idiv
	extern	opn_clc
	extern	opn_stc
	extern	opn_cli
	extern	opn_sti
	extern	opn_cld
	extern	opn_std
	extern	opn_movsx
	extern	opn_movzx
	extern	opn_lfs
	extern	opn_lgs
	extern	opn_lss
	extern	opn_clts
	extern	opn_shld
	extern	opn_shrd
	extern	opn_bsf
	extern	opn_bsr
	extern	opn_bt
	extern	opn_bts
	extern	opn_btr
	extern	opn_btc
	extern	opn_ibts
	extern	opn_xbts
  extern opn_salc
  extern opn_setalc
	extern	opn_setc
	extern	opn_setnae
	extern	opn_setnc
	extern	opn_setna
	extern	opn_setnbe
	extern	opn_setae
	extern	opn_sete
	extern	opn_setne
	extern	opn_setpe
	extern	opn_setpo
	extern	opn_setnle
	extern	opn_setnl
	extern	opn_setnge
	extern	opn_setng
	extern	opn_seto
	extern	opn_setno
	extern	opn_setb
	extern	opn_setnb
	extern	opn_setz
	extern	opn_setnz
	extern	opn_setbe
	extern	opn_seta
	extern	opn_sets
	extern	opn_setns
	extern	opn_setp
	extern	opn_setnp
	extern	opn_setl
	extern	opn_setge
	extern	opn_setle
	extern	opn_setg
	extern	opn_lar
	extern	opn_lsl
	extern	opn_lgdt
	extern	opn_lidt
	extern	opn_lldt
	extern	opn_lmsw
	extern	opn_ltr
	extern	opn_sgdt
	extern	opn_sidt
	extern	opn_sldt
	extern	opn_smsw
	extern	opn_str
	extern	opn_verr
	extern	opn_verw
	extern	opn_fnop
	extern	opn_fchs
	extern	opn_fabs
	extern	opn_ftst
	extern	opn_fxam
	extern	opn_fld1
	extern	opn_fldl2t
	extern	opn_fldl2e
	extern	opn_fldpi
	extern	opn_fldlg2
	extern	opn_fldln2
	extern	opn_fldz
	extern	opn_f2xm1
	extern	opn_fyl2x
	extern	opn_fptan
	extern	opn_fpatan
	extern	opn_fprem1
	extern	opn_fxtract
	extern	opn_fdecstp
	extern	opn_fincstp
	extern	opn_fprem
	extern	opn_fyl2xp1
	extern	opn_fsqrt
	extern	opn_fsincos
	extern	opn_frndint
	extern	opn_fscale
	extern	opn_fsin
	extern	opn_fcos
	extern	opn_fucompp
	extern	opn_feni
	extern	opn_fdisi
	extern	opn_fclex
	extern	opn_finit
	extern	opn_fsetpm
	extern	opn_fcompp
	extern	opn_fld
	extern	opn_fxch
	extern	opn_fstp
	extern	opn_esc
	extern	opn_fldenv
	extern	opn_fldcw
	extern	opn_fnstenv
	extern	opn_fnstcw
	extern	opn_ffree
  extern opn_ffreep
	extern	opn_fst
	extern	opn_fucom
	extern	opn_fucomp
	extern	opn_frstor
	extern	opn_fnsave
	extern	opn_fnstsw
	extern	opn_fbld
	extern	opn_fild
	extern	opn_fbstp
	extern	opn_fistp
	extern	opn_fmul
	extern	opn_fcom
	extern	opn_fsub
	extern	opn_fsubr
	extern	opn_fdiv
	extern	opn_fdivr
	extern	opn_fadd
	extern	opn_fcomp
	extern	opn_fiadd
	extern	opn_fimul
	extern	opn_ficom
	extern	opn_ficomp
	extern	opn_fisub
	extern	opn_fisubr
	extern	opn_fidiv
	extern	opn_fidivr
	extern	opn_fist
	extern	opn_faddp
	extern	opn_fmulp
	extern	opn_fdivp
	extern	opn_fdivrp
	extern	opn_fsubp
	extern	opn_fsubrp
	extern	opn_wbinvd
	extern	opn_invd
	extern	opn_bswap
	extern	regs
	extern	psegs
	extern	crreg
	extern	drreg
	extern	trreg
	extern	sudreg
	extern	scales
	extern	stalone
	extern	st_repz
	extern	st_repnz
  extern st_lock
	extern	base0
	extern	base1
	extern	base2
	extern	base3
	extern	base4
	extern	base5
	extern	base6
	extern	base7
	extern	xst0
	extern	xst1
	extern	xst2
	extern	xst3
	extern	xst4
	extern	xst5
	extern	byptr
	extern	dwptr
	extern	woptr
	extern	theptr
  extern pbptr
  extern fwptr
	extern	stsreg
  extern mmxreg
	extern	based
	extern	sts
	extern	opn_cmovo
	extern	opn_cmovno
	extern	opn_cmovb
	extern	opn_cmovnb
	extern	opn_cmovz
	extern	opn_cmovnz
	extern	opn_cmovbe
	extern	opn_cmova
	extern	opn_cmovs
	extern	opn_cmovns
	extern	opn_cmovp
	extern	opn_cmovnp
	extern	opn_cmovl
	extern	opn_cmovge
	extern	opn_cmovle
	extern	opn_cmovg
	extern  opn_cmpxchg
	extern  opn_cmpxchg8b
	extern	opn_cpuid
	extern	opn_rdmsr
	extern	opn_rdtsc
	extern	opn_rdpmc
	extern	opn_rsm
	extern	opn_ud2
	extern	opn_wrmsr
	extern	opn_xadd
        extern opn_emms
        extern opn_movd
        extern opn_movq
        extern opn_packssdw
        extern opn_packsswb
        extern opn_packuswb
        extern opn_paddb
        extern opn_paddd
        extern opn_paddsb
        extern opn_paddsw
        extern opn_paddusb
        extern opn_paddusw
        extern opn_paddw
        extern opn_pand
        extern opn_pandn
        extern opn_pcmpeqb
        extern opn_pcmpeqd
        extern opn_pcmpeqw
        extern opn_pcmpgtb
        extern opn_pcmpgtd
        extern opn_pcmpgtw
        extern opn_pmaddwd
        extern opn_pmulhw
        extern opn_pmullw
        extern opn_por
        extern opn_pslld
        extern opn_psllq
        extern opn_psllw
        extern opn_psrad
        extern opn_psraw
        extern opn_psrld
        extern opn_psrlq
        extern opn_psrlw
        extern opn_psubb
        extern opn_psubd
        extern opn_psubsb
        extern opn_psubsw
        extern opn_psubusb
        extern opn_psubusw
        extern opn_psubw
        extern opn_punpckhbw
        extern opn_punpckhwd
        extern opn_punpckhdq
        extern opn_punpcklbw
        extern opn_punpcklwd
        extern opn_punpckldq
        extern opn_pxor
        extern opn_fcomi
        extern opn_fcomip
        extern opn_fucomi
        extern opn_fucomip
        extern opn_fxsave
        extern opn_fxrstor
        extern opn_fcmovb
        extern opn_fcmovbe
        extern opn_fcmove
        extern opn_fcmovnb
        extern opn_fcmovnbe
        extern opn_fcmovne
        extern opn_fcmovnu
        extern opn_fcmovu
        extern opn_farptr

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	extern	PrintSpace
	extern	PrintDword
	extern PrintWord
	extern PrintByte
	extern	ScanKey
	extern crlf

	extern PureChar 
	extern PutChar
	extern GetKey
	extern Message
	extern olMessage
	extern dgroupMessage

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern qerr
	extern ReadNumber
	extern ReadAddress
	extern InputHandler
	extern WadeSpace
	extern PageTrapErr
	extern PageTrapUnerr
	extern GetInputLine
	extern InputBuffer

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern reflags
	extern dreax
	extern drebx
	extern drecx 
	extern dredx
	extern dresi
	extern dredi
	extern drebp 
	extern dresp
	extern dreip
	extern drds
	extern dres 
	extern drcs
	extern drss
	extern drfs
	extern drgs
	extern rtoss 
	extern monitor_init
	extern sysds 

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern TableDispatch
	extern nofunction

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  extern TabTo
	extern	ReadOverrides
	extern	DispatchOperands
	extern	FormatDisassembly
	extern	FormatValue
	extern	PutDword
	extern	PutWord
	extern	PutByte
	extern	put2
	extern	put3
	extern	put4
	extern	strlen
  extern GetSeg
  extern segs
  extern code_address

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern trap
	extern go
	extern Proceed
	extern callcheck
	extern cmpstring
	extern WadePrefix
	extern ReTrap
	extern TrapCount
	extern proceedone
	extern proceedCount

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern	dump
  extern memsizechk
	extern	index
  extern memsize

;Software License Agreement (BSD License)
;
;Copyright (c) 1997-2008, David Lindauer, (LADSoft).
;All rights reserved.
;
;Redistribution and use of this software in source and binary forms, with or without modification, are
;permitted provided that the following conditions are met:
;
;* Redistributions of source code must retain the above
;  copyright notice, this list of conditions and the
;  following disclaimer.
;
;* Redistributions in binary form must reproduce the above
;  copyright notice, this list of conditions and the
;  following disclaimer in the documentation and/or other
;  materials provided with the distribution.
;
;* Neither the name of LADSoft nor the names of its
;  contributors may be used to endorse or promote products
;  derived from this software without specific prior
;  written permission of LADSoft.
;
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
;WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
;PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
;ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
;LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
;TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
;ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	extern	entry
  extern entrytobuf
  extern diglen
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
