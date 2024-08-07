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

segment data USE32
; This is a table of mnemonics for the dissassembler
;
    global	opn_cwde
    global	opn_cdq
    global	opn_add
    global	opn_push
    global	opn_pop
    global	opn_or
    global	opn_adc
    global	opn_sbb
    global	opn_and
    global	opn_daa
    global	opn_sub
    global	opn_das
    global	opn_xor
    global	opn_aaa
    global	opn_cmp
    global	opn_aas
    global	opn_inc
    global	opn_dec
    global	opn_pusha
    global	opn_popa
    global	opn_pushad
    global	opn_popad
        global  opn_pushaw
        global  opn_popaw
    global	opn_bound
    global	opn_arpl
    global	opn_imul
    global	opn_insb
    global	opn_ins
    global	opn_outsb
    global	opn_outs
    global	opn_insd
    global	opn_outsd
    global	opn_insw
    global	opn_outsw
    global	opn_jo
    global	opn_jno
    global	opn_jc
    global	opn_jnae
    global	opn_jnc
    global	opn_jna
    global	opn_jnbe
    global	opn_jae
    global	opn_je
    global	opn_jne
    global	opn_jpe
    global	opn_jpo
    global	opn_jnle
    global	opn_jnl
    global	opn_jnge
    global	opn_jng
    global	opn_jb
    global	opn_jnb
    global	opn_jz
    global	opn_jnz
    global	opn_jbe
    global	opn_ja
    global	opn_js
    global	opn_jns
    global	opn_jp
    global	opn_jnp
    global	opn_jl
    global	opn_jge
    global	opn_jle
    global	opn_jg
    global	opn_test
    global	opn_xchg
    global	opn_mov
    global	opn_lea
        global  opn_loadall
        global  opn_loadall386
        global  opn_loadall286
    global	opn_nop
    global	opn_cbw
    global	opn_cwd
    global	opn_call
    global	opn_wait
    global	opn_fwait
    global	opn_pushf
    global	opn_popf
    global	opn_pushfd
    global	opn_popfd
        global  opn_pushfw
        global  opn_popfw
    global	opn_sahf
    global	opn_lahf
    global	opn_movs
    global	opn_cmps
    global	opn_stos
    global	opn_lods
    global	opn_scas
    global	opn_movsb
    global	opn_cmpsb
    global	opn_stosb
    global	opn_lodsb
    global	opn_scasb
    global	opn_movsw
    global	opn_cmpsw
    global	opn_stosw
    global	opn_lodsw
    global	opn_scasw
    global	opn_movsd
    global	opn_cmpsd
    global	opn_stosd
    global	opn_lodsd
    global	opn_scasd
    global	opn_rol
    global	opn_ror
    global	opn_rcl
    global	opn_rcr
    global	opn_shl
    global	opn_shr
    global	opn_sar
    global	opn_ret
    global	opn_les
    global	opn_lds
    global	opn_enter
    global	opn_retf
    global	opn_int
        global  opn_icebp
    global	opn_into
    global	opn_iret
    global	opn_leave
    global	opn_aam
    global	opn_aad
    global	opn_xlat
    global	opn_xlatb
    global	opn_loopnz
    global	opn_loopz
    global	opn_loopne
    global	opn_loope
    global	opn_loop
        global  opn_loopnzw
        global  opn_loopzw
        global  opn_loopnew
        global  opn_loopew
        global  opn_loopw
        global  opn_loopnzd
        global  opn_loopzd
        global  opn_loopned
        global  opn_looped
        global  opn_loopd
    global	opn_jcxz
        global  opn_jcxzw
        global  opn_jcxzd
        global  opn_jecxz
    global	opn_in
    global	opn_out
    global	opn_jmp
    global	opn_lock
    global	opn_repnz
    global	opn_repz
    global	opn_hlt
    global	opn_cmc
    global	opn_not
    global	opn_neg
    global	opn_mul
    global	opn_div
    global	opn_idiv
    global	opn_clc
    global	opn_stc
    global	opn_cli
    global	opn_sti
    global	opn_cld
    global	opn_std
    global	opn_movsx
    global	opn_movzx
    global	opn_lfs
    global	opn_lgs
    global	opn_lss
    global	opn_clts
    global	opn_shld
    global	opn_shrd
    global	opn_bsf
    global	opn_bsr
    global	opn_bt
    global	opn_bts
    global	opn_btr
    global	opn_btc
    global	opn_ibts
    global	opn_xbts
        global  opn_salc
        global  opn_setalc
    global	opn_setc
    global	opn_setnae
    global	opn_setnc
    global	opn_setna
    global	opn_setnbe
    global	opn_setae
    global	opn_sete
    global	opn_setne
    global	opn_setpe
    global	opn_setpo
    global	opn_setnle
    global	opn_setnl
    global	opn_setnge
    global	opn_setng
    global	opn_seto
    global	opn_setno
    global	opn_setb
    global	opn_setnb
    global	opn_setz
    global	opn_setnz
    global	opn_setbe
    global	opn_seta
    global	opn_sets
    global	opn_setns
    global	opn_setp
    global	opn_setnp
    global	opn_setl
    global	opn_setge
    global	opn_setle
    global	opn_setg
    global	opn_lar
    global	opn_lsl
    global	opn_lgdt
    global	opn_lidt
    global	opn_lldt
    global	opn_lmsw
    global	opn_ltr
    global	opn_sgdt
    global	opn_sidt
    global	opn_sldt
    global	opn_smsw
    global	opn_str
    global	opn_verr
    global	opn_verw
    global	opn_fnop
    global	opn_fchs
    global	opn_fabs
    global	opn_ftst
    global	opn_fxam
    global	opn_fld1
    global	opn_fldl2t
    global	opn_fldl2e
    global	opn_fldpi
    global	opn_fldlg2
    global	opn_fldln2
    global	opn_fldz
    global	opn_f2xm1
    global	opn_fyl2x
    global	opn_fptan
    global	opn_fpatan
    global	opn_fprem1
    global	opn_fxtract
    global	opn_fdecstp
    global	opn_fincstp
    global	opn_fprem
    global	opn_fyl2xp1
    global	opn_fsqrt
    global	opn_fsincos
    global	opn_frndint
    global	opn_fscale
    global	opn_fsin
    global	opn_fcos
    global	opn_fucompp
    global	opn_feni
    global	opn_fdisi
    global	opn_fclex
    global	opn_finit
    global	opn_fsetpm
    global	opn_fcompp
    global	opn_fld
    global	opn_fxch
    global	opn_fstp
    global	opn_esc
    global	opn_fldenv
    global	opn_fldcw
    global	opn_fnstenv
    global	opn_fnstcw
    global	opn_ffree
        global  opn_ffreep
    global	opn_fst
    global	opn_fucom
    global	opn_fucomp
    global	opn_frstor
    global	opn_fnsave
    global	opn_fnstsw
    global	opn_fbld
    global	opn_fild
    global	opn_fbstp
    global	opn_fistp
    global	opn_fmul
    global	opn_fcom
    global	opn_fsub
    global	opn_fsubr
    global	opn_fdiv
    global	opn_fdivr
    global	opn_fadd
    global	opn_fcomp
    global	opn_fiadd
    global	opn_fimul
    global	opn_ficom
    global	opn_ficomp
    global	opn_fisub
    global	opn_fisubr
    global	opn_fidiv
    global	opn_fidivr
    global	opn_fist
    global	opn_faddp
    global	opn_fmulp
    global	opn_fdivp
    global	opn_fdivrp
    global	opn_fsubp
    global	opn_fsubrp
    global	opn_wbinvd
    global	opn_invd
    global	opn_bswap
    global	regs
    global	psegs
    global	crreg
    global	drreg
    global	trreg
    global	sudreg
    global	scales
    global	stalone
    global	st_repz
    global	st_repnz
        global  st_lock
    global	base0
    global	base1
    global	base2
    global	base3
    global	base4
    global	base5
    global	base6
    global	base7
    global	xst0
    global	xst1
    global	xst2
    global	xst3
    global	xst4
    global	xst5
    global	byptr
    global	dwptr
    global	woptr
    global	theptr
        global  pbptr
        global  fwptr
    global	stsreg
        global  mmxreg
    global	based
    global	sts
    global	opn_cmovo
    global	opn_cmovno
    global	opn_cmovb
    global	opn_cmovnb
    global	opn_cmovz
    global	opn_cmovnz
    global	opn_cmovbe
    global	opn_cmova
    global	opn_cmovs
    global	opn_cmovns
    global	opn_cmovp
    global	opn_cmovnp
    global	opn_cmovl
    global	opn_cmovge
    global	opn_cmovle
    global	opn_cmovg
    global  opn_cmpxchg
    global  opn_cmpxchg8b
    global	opn_cpuid
    global	opn_rdmsr
    global	opn_rdtsc
    global	opn_rdpmc
    global	opn_rsm
    global	opn_ud2
    global	opn_wrmsr
    global	opn_xadd
        global opn_emms
        global opn_movd
        global opn_movq
        global opn_packssdw
        global opn_packsswb
        global opn_packuswb
        global opn_paddb
        global opn_paddd
        global opn_paddsb
        global opn_paddsw
        global opn_paddusb
        global opn_paddusw
        global opn_paddw
        global opn_pand
        global opn_pandn
        global opn_pcmpeqb
        global opn_pcmpeqd
        global opn_pcmpeqw
        global opn_pcmpgtb
        global opn_pcmpgtd
        global opn_pcmpgtw
        global opn_pmaddwd
        global opn_pmulhw
        global opn_pmullw
        global opn_por
        global opn_pslld
        global opn_psllq
        global opn_psllw
        global opn_psrad
        global opn_psraw
        global opn_psrld
        global opn_psrlq
        global opn_psrlw
        global opn_psubb
        global opn_psubd
        global opn_psubsb
        global opn_psubsw
        global opn_psubusb
        global opn_psubusw
        global opn_psubw
        global opn_punpckhbw
        global opn_punpckhwd
        global opn_punpckhdq
        global opn_punpcklbw
        global opn_punpcklwd
        global opn_punpckldq
        global opn_pxor
        global opn_fcomi
        global opn_fcomip
        global opn_fucomi
        global opn_fucomip
        global opn_fxsave
        global opn_fxrstor
        global opn_fcmovb
        global opn_fcmovbe
        global opn_fcmove
        global opn_fcmovnb
        global opn_fcmovnbe
        global opn_fcmovne
        global opn_fcmovnu
        global opn_fcmovu
        global opn_farptr
opn_cwde db	"cwde",0
opn_cdq	db	"cdq",0
opn_add	db	"add",0
opn_push	db	"push",0
opn_pop	db	"pop",0
opn_or	db	"or",0
opn_adc	db	"adc",0
opn_sbb	db	"sbb",0
opn_and	db	"and",0
opn_daa	db	"daa",0
opn_sub	db	"sub",0
opn_das	db	"das",0
opn_xor	db	"xor",0
opn_aaa	db	"aaa",0
opn_cmp	db	"cmp",0
opn_aas	db	"aas",0
opn_inc	db	"inc",0
opn_dec	db	"dec",0
opn_pusha	db	"pusha",0
opn_popa	db	"popa",0
opn_pushad	db	"pushad",0
opn_popad	db	"popad",0
opn_pushaw      db      "pushaw",0
opn_popaw       db      "popaw",0
opn_bound	db	"bound",0
opn_arpl	db	"arpl",0
opn_imul	db	"imul",0
opn_insb	db	"insb",0
opn_insw	db	"insw",0
opn_insd	db	"insd",0
opn_ins	db	"ins",0
opn_outsb	db	"outsb",0
opn_outsw	db	"outsw",0
opn_outsd	db	"outsd",0
opn_outs	db	"outs",0
opn_jc	db	"jc",0
opn_jnae	db	"jnae",0
opn_jnc	db	"jnc",0
opn_jna	db	"jna",0
opn_jnbe	db	"jnbe",0
opn_jae	db	"jae",0
opn_je	db	"je",0
opn_jne	db	"jne",0
opn_jpo	db	"jpo",0
opn_jpe	db	"jpe",0
opn_jnle	db	"jnle",0
opn_jnl	db	"jnl",0
opn_jnge	db	"jnge",0
opn_jng	db	"jng",0
opn_jo	db	"jo",0
opn_jno	db	"jno",0
opn_jb	db	"jb",0
opn_jnb	db	"jnb",0
opn_jz	db	"jz",0
opn_jnz	db	"jnz",0
opn_jbe	db	"jbe",0
opn_ja	db	"ja",0
opn_js	db	"js",0
opn_jns	db	"jns",0
opn_jp	db	"jp",0
opn_jnp	db	"jnp",0
opn_jl	db	"jl",0
opn_jge	db	"jge",0
opn_jle	db	"jle",0
opn_jg	db	"jg",0
opn_test	db	"test",0
opn_xchg	db	"xchg",0
opn_mov	db	"mov",0
opn_lea	db	"lea",0
opn_loadall     db "loadall",0
opn_loadall386     db "loadall386",0
opn_loadall286     db "loadall286",0
opn_nop	db	"nop",0
opn_cbw	db	"cbw",0
opn_cwd	db	"cwd",0
opn_call	db	"call",0
opn_wait	db	"wait",0
opn_fwait	db	"fwait",0
opn_pushf	db	"pushf",0
opn_popf	db	"popf",0
opn_pushfd	db	"pushfd",0
opn_popfd	db	"popfd",0
opn_pushfw      db      "pushfw",0
opn_popfw       db      "popfw",0
opn_sahf	db	"sahf",0
opn_lahf	db	"lahf",0
opn_movs	db	"movs",0
opn_cmps	db	"cmps",0
opn_stos	db	"stos",0
opn_lods	db	"lods",0
opn_scas	db	"scas",0
opn_movsb	db	"movsb",0
opn_cmpsb	db	"cmpsb",0
opn_stosb	db	"stosb",0
opn_lodsb	db	"lodsb",0
opn_scasb	db	"scasb",0
opn_movsw	db	"movsw",0
opn_cmpsw	db	"cmpsw",0
opn_stosw	db	"stosw",0
opn_lodsw	db	"lodsw",0
opn_scasw	db	"scasw",0
opn_movsd	db	"movsd",0
opn_cmpsd	db	"cmpsd",0
opn_stosd	db	"stosd",0
opn_lodsd	db	"lodsd",0
opn_scasd	db	"scasd",0
opn_rol	db	"rol",0
opn_ror	db	"ror",0
opn_rcl	db	"rcl",0
opn_rcr	db	"rcr",0
opn_shl	db	"shl",0
opn_shr	db	"shr",0
opn_sar	db	"sar",0
opn_ret	db	"ret",0
opn_les	db	"les",0
opn_lds	db	"lds",0
opn_enter	db	"enter",0
opn_retf	db	"retf",0
opn_int	db	"int",0
opn_icebp db    "icebp",0
opn_into	db	"into",0
opn_iret	db	"iret",0
opn_leave	db	"leave",0
opn_aam	db	"aam",0
opn_aad	db	"aad",0
opn_xlat	db	"xlat",0
opn_xlatb	db	"xlatb",0
opn_loopnz	db	"loopnz",0
opn_loopz	db	"loopz",0
opn_loopne	db	"loopne",0
opn_loope	db	"loope",0
opn_loop	db	"loop",0
opn_loopnzw     db      "loopnzw",0
opn_loopzw      db      "loopzw",0
opn_loopnew     db      "loopnew",0
opn_loopew      db      "loopew",0
opn_loopw       db      "loopw",0
opn_loopnzd     db      "loopnzd",0
opn_loopzd      db      "loopzd",0
opn_loopned     db      "loopned",0
opn_looped      db      "looped",0
opn_loopd       db      "loopd",0
opn_jcxz	db	"jcxz",0
opn_jcxzw       db      "jcxzw",0
opn_jcxzd       db      "jcxzd",0
opn_jecxz       db      "jecxz",0
opn_in	db	"in",0
opn_out	db	"out",0
opn_jmp	db	"jmp",0
opn_lock	db	"lock",0
opn_repnz	db	"repnz",0
opn_repz	db	"repz",0
opn_hlt	db	"hlt",0
opn_cmc	db	"cmc",0
opn_not	db	"not",0
opn_neg	db	"neg",0
opn_mul	db	"mul",0
opn_div	db	"div",0
opn_idiv	db	"idiv",0
opn_clc	db	"clc",0
opn_stc	db	"stc",0
opn_cli	db	"cli",0
opn_sti	db	"sti",0
opn_cld	db	"cld",0
opn_std	db	"std",0
opn_movsx	db	"movsx",0
opn_movzx	db	"movzx",0
opn_lfs	db	"lfs",0
opn_lgs	db	"lgs",0
opn_lss	db	"lss",0
opn_clts	db	"clts",0
opn_shld	db	"shld",0
opn_shrd	db	"shrd",0
opn_bsf	db	"bsf",0
opn_bsr	db	"bsr",0
opn_bt	db	"bt",0
opn_bts	db	"bts",0
opn_btr	db	"btr",0
opn_btc	db	"btc",0
opn_ibts	db	"ibts",0
opn_xbts	db	"xbts",0
opn_salc        db      "salc",0
opn_setalc      db      "setalc",0
opn_setc	db	"setc",0
opn_setnae	db	"setnae",0
opn_setnc	db	"setnc",0
opn_setna	db	"setna",0
opn_setnbe	db	"setnbe",0
opn_setae	db	"setae",0
opn_sete	db	"sete",0
opn_setne	db	"setne",0
opn_setpo	db	"setpo",0
opn_setpe	db	"setpe",0
opn_setnle	db	"setnle",0
opn_setnl	db	"setnl",0
opn_setnge	db	"setnge",0
opn_setng	db	"setng",0
opn_seto	db	"seto",0
opn_setno	db	"setno",0
opn_setb	db	"setb",0
opn_setnb	db	"setnb",0
opn_setz	db	"setz",0
opn_setnz	db	"setnz",0
opn_setbe	db	"setbe",0
opn_seta	db	"seta",0
opn_sets	db	"sets",0
opn_setns	db	"setns",0
opn_setp	db	"setp",0
opn_setnp	db	"setnp",0
opn_setl	db	"setl",0
opn_setge	db	"setge",0
opn_setle	db	"setle",0
opn_setg	db	"setg",0
opn_lar	db	"lar",0
opn_lsl	db	"lsl",0
opn_lgdt	db	"lgdt",0
opn_lidt	db	"lidt",0
opn_lldt	db	"lldt",0
opn_lmsw	db	"lmsw",0
opn_ltr	db	"ltr",0
opn_sgdt	db	"sgdt",0
opn_sidt	db	"sidt",0
opn_sldt	db	"sldt",0
opn_smsw	db	"smsw",0
opn_str	db	"str",0
opn_verr	db	"verr",0
opn_verw	db	"verw",0
opn_fnop	db	"fnop",0
opn_fchs	db	"fchs",0
opn_fabs	db	"fabs",0
opn_ftst	db	"ftst",0
opn_fxam	db	"fxam",0
opn_fld1	db	"fld1",0
opn_fldl2t	db	"fldl2t",0
opn_fldl2e	db	"fldl2e",0
opn_fldpi	db	"fldpi",0
opn_fldlg2	db	"fldlg2",0
opn_fldln2	db	"fldln2",0
opn_fldz	db	"fldz",0
opn_f2xm1	db	"f2xm1",0
opn_fyl2x	db	"fyl2x",0
opn_fptan	db	"fptan",0
opn_fpatan	db	"fpatan",0
opn_fprem1	db	"fprem1",0
opn_fxtract	db	"fxtract",0
opn_fdecstp	db	"fdecstp",0
opn_fincstp	db	"fincstp",0
opn_fprem	db	"fprem",0
opn_fyl2xp1	db	"fyl2xp1",0
opn_fsqrt	db	"fsqrt",0
opn_fsincos	db	"fsincos",0
opn_frndint	db	"frndint",0
opn_fscale	db	"fscale",0
opn_fsin	db	"fsin",0
opn_fcos	db	"fcos",0
opn_fucompp	db	"fucompp",0
opn_feni	db	"feni",0
opn_fdisi	db	"fdisi",0
opn_fclex	db	"fnclex",0
opn_finit	db	"fninit",0
opn_fsetpm	db	"fsetpm",0
opn_fcompp	db	"fcompp",0
opn_fld	db	"fld",0
opn_fxch	db	"fxch",0
opn_fstp	db	"fstp",0
opn_esc	db	"esc",0
opn_fldenv	db	"fldenv",0
opn_fldcw	db	"fldcw",0
opn_fnstenv	db	"fnstenv",0
opn_fnstcw	db	"fnstcw",0
opn_ffree	db	"ffree",0
opn_ffreep       db     "ffreep",0
opn_fst	db	"fst",0
opn_fucom	db	"fucom",0
opn_fucomp	db	"fucomp",0
opn_frstor	db	"frstor",0
opn_fnsave	db	"fnsave",0
opn_fnstsw	db	"fnstsw",0
opn_fbld	db	"fbld",0
opn_fild	db	"fild",0
opn_fbstp	db	"fbstp",0
opn_fistp	db	"fistp",0
opn_fmul	db	"fmul",0
opn_fcom	db	"fcom",0
opn_fsub	db	"fsub",0
opn_fsubr	db	"fsubr",0
opn_fdiv	db	"fdiv",0
opn_fdivr	db	"fdivr",0
opn_fadd	db	"fadd",0
opn_fcomp	db	"fcomp",0
opn_fiadd	db	"fiadd",0
opn_fimul	db	"fimul",0
opn_ficom	db	"ficom",0
opn_ficomp	db	"ficomp",0
opn_fisub	db	"fisub",0
opn_fisubr	db	"fisubr",0
opn_fidiv	db	"fidiv",0
opn_fidivr	db	"fidivr",0
opn_fist	db	"fist",0
opn_faddp	db	"faddp",0
opn_fmulp	db	"fmulp",0
opn_fsubp	db	"fsubp",0
opn_fdivp	db	"fdivp",0
opn_fsubrp	db	"fsubrp",0
opn_fdivrp	db	"fdivrp",0
opn_wbinvd	db	"wb"
opn_invd	db	"invd",0
opn_bswap	db	"bswap",0
opn_cmovo	db	"cmovo",0
opn_cmovno	db	"cmovno",0
opn_cmovb	db	"cmovb",0
opn_cmovnb	db	"cmovnb",0
opn_cmovz	db	"cmovz",0
opn_cmovnz	db	"cmovnz",0
opn_cmovbe	db	"cmovbe",0
opn_cmova	db	"cmova",0
opn_cmovs	db	"cmovs",0
opn_cmovns	db	"cmovns",0
opn_cmovp	db	"cmovp",0
opn_cmovnp	db	"cmovnp",0
opn_cmovl	db	"cmovl",0
opn_cmovge	db	"cmovge",0
opn_cmovle	db	"cmovle",0
opn_cmovg	db	"cmovg",0
opn_cmpxchg	db	"cmpxchg",0
opn_cmpxchg8b	db	"cmpxchg8b",0
opn_cpuid	db	"cpuid",0
opn_rdmsr	db	"rdmsr",0
opn_rdpmc	db	"rdpmc",0
opn_rdtsc	db	"rdtsc",0
opn_rsm		db	"rsm",0
opn_ud2		db	"ud2",0
opn_wrmsr	db	"wrmsr",0
opn_xadd	db	"xadd",0
opn_emms	db	"emms",0
opn_movd        db      "movd",0
opn_movq        db      "movq",0
opn_packssdw	db	"packssdw",0
opn_packsswb	db	"packsswb",0
opn_packuswb	db	"packuswb",0
opn_paddb	db	"paddb",0
opn_paddd	db	"paddd",0
opn_paddsb	db	"paddsb",0
opn_paddsw	db	"paddsw",0
opn_paddusb	db	"paddusb",0
opn_paddusw	db	"paddusw",0
opn_paddw	db	"paddw",0
opn_pand	db	"pand",0
opn_pandn	db	"pandn",0
opn_pcmpeqb	db	"pcmpeqb",0
opn_pcmpeqd	db	"pcmpeqd",0
opn_pcmpeqw	db	"pcmpeqw",0
opn_pcmpgtb	db	"pcmpgtb",0
opn_pcmpgtd	db	"pcmpgtd",0
opn_pcmpgtw	db	"pcmpgtw",0
opn_pmaddwd	db	"pmaddwd",0
opn_pmulhw	db	"pmulhw",0
opn_pmullw	db	"pmullw",0
opn_por         db      "por",0
opn_pslld	db	"pslld",0
opn_psllq	db	"psllq",0
opn_psllw	db	"psllw",0
opn_psrad	db	"psrad",0
opn_psraw	db	"psraw",0
opn_psrld	db	"psrld",0
opn_psrlq	db	"psrlq",0
opn_psrlw	db	"psrlw",0
opn_psubb	db	"psubb",0
opn_psubd	db	"psubd",0
opn_psubsb	db	"psubsb",0
opn_psubsw	db	"psubsw",0
opn_psubusb	db	"psubusb",0
opn_psubusw	db	"psubusw",0
opn_psubw	db	"psubw",0
opn_punpckhbw	db	"punpckhbw",0
opn_punpckhwd	db	"punpckhwd",0
opn_punpckhdq	db	"punpckhdq",0
opn_punpcklbw	db	"punpcklbw",0
opn_punpcklwd	db	"punpcklwd",0
opn_punpckldq	db	"punpckldq",0
opn_pxor	db	"pxor",0
opn_fcomi       db      "fcomi",0
opn_fcomip      db      "fcomip",0
opn_fucomi      db      "fucomi",0
opn_fucomip     db      "fucomip",0
opn_fxsave      db      "fxsave",0
opn_fxrstor     db      "fxrstor",0
opn_fcmovb      db      "fcmovb",0
opn_fcmovbe     db      "fcmovbe",0
opn_fcmove      db      "fcmove",0
opn_fcmovnb     db      "fcmovnb",0
opn_fcmovnbe    db      "fcmovnbe",0
opn_fcmovne     db      "fcmovne",0
opn_fcmovnu     db      "fcmovnu",0
opn_fcmovu      db      "fcmovu",0

regs	db	"alcldlblahchdhbhaxcxdxbxspbpsidi"
psegs	db	"escsssdsfsgs"
crreg	db	"CR01234567"
drreg	db	"DR01234567"
trreg	db	"TR01234567"
sudreg	db	"?R????????"
scales  db      "1*2*4*8*"
stalone	db	"st",0
st_repz	db	"repz ",0
st_repnz db	"repnz ",0
st_lock db  "lock ",0
;st_repz	db	C_INSTRUCTION, "repz ",0
;st_repnz db	C_INSTRUCTION, "repnz ",0
;st_lock db      C_INSTRUCTION, "lock ",0

base0	db	"bx+si",0
base1	db	"bx+di",0
base2	db	"bp+si",0
base3	db	"bp+di",0
base4	db	"si",0
base5	db	"di",0
base6	db	"bp",0
base7	db	"bx",0
xst0	db	"dword",0	; Should be DWORD for MATH, FWORD for jmp/call
xst1	db	"dword",0
xst2	db	"qword",0
xst3	db	"word",0
xst4	db	"tbyte"
xst5	db	0
byptr   db      "byte ",0,"ptr ",0
dwptr	db	"d"
woptr	db	"word"
theptr  db      " ",0," ptr ",0
pbptr   db      "pbyte ",0,"ptr ",0
fwptr   db      "fword ",0,"ptr ",0
stsreg	db	"st(",0
mmxreg  db      "mm",0
based	dd	base0,base1,base2,base3,base4,base5,base6,base7
sts	dd	xst0,xst1,xst2,xst3,xst4,xst5,xst5,xst5
opn_farptr db   "far ",0
