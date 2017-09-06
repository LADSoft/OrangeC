/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"

#define IEEE

extern ARCH_ASM *chosenAssembler;
extern int prm_nodos;
extern int prm_flat;
extern OPCODE popn_aaa;
extern OPCODE popn_aad;
extern OPCODE popn_aam;
extern OPCODE popn_aas;
extern OPCODE popn_add;
extern OPCODE popn_adc;
extern OPCODE popn_and;
extern OPCODE popn_arpl;
extern OPCODE popn_bound;
extern OPCODE popn_bsf;
extern OPCODE popn_bsr;
extern OPCODE popn_bswap;
extern OPCODE popn_btc;
extern OPCODE popn_bt;
extern OPCODE popn_btr;
extern OPCODE popn_bts;
extern OPCODE popn_call;
extern OPCODE popn_cbw;
extern OPCODE popn_cwde;
extern OPCODE popn_cwd;
extern OPCODE popn_cdq;
extern OPCODE popn_clc;
extern OPCODE popn_cld;
extern OPCODE popn_cli;
extern OPCODE popn_clts;
extern OPCODE popn_cmc;
extern OPCODE popn_cmp;
extern OPCODE popn_cmpxchg;
extern OPCODE popn_cmpxchg8b;
extern OPCODE popn_cmps;
extern OPCODE popn_cmpsb;
extern OPCODE popn_cmpsw;
extern OPCODE popn_cmpsd;
extern OPCODE popn_daa;
extern OPCODE popn_das;
extern OPCODE popn_dec;
extern OPCODE popn_div;
extern OPCODE popn_enter;
extern OPCODE popn_hlt;
extern OPCODE popn_idiv;
extern OPCODE popn_imul;
extern OPCODE popn_in;
extern OPCODE popn_inc;
extern OPCODE popn_ins;
extern OPCODE popn_insb;
extern OPCODE popn_insw;
extern OPCODE popn_insd;
extern OPCODE popn_int;
extern OPCODE popn_int3;
extern OPCODE popn_into;
extern OPCODE popn_invd;
extern OPCODE popn_iret;
extern OPCODE popn_iretd;
extern OPCODE popn_jcxz;
extern OPCODE popn_ja;
extern OPCODE popn_jnbe;
extern OPCODE popn_jae;
extern OPCODE popn_jnb;
extern OPCODE popn_jnc;
extern OPCODE popn_jb;
extern OPCODE popn_jc;
extern OPCODE popn_jnae;
extern OPCODE popn_jbe;
extern OPCODE popn_jna;
extern OPCODE popn_je;
extern OPCODE popn_jz;
extern OPCODE popn_jg;
extern OPCODE popn_jnle;
extern OPCODE popn_jl;
extern OPCODE popn_jnge;
extern OPCODE popn_jge;
extern OPCODE popn_jnl;
extern OPCODE popn_jle;
extern OPCODE popn_jng;
extern OPCODE popn_jne;
extern OPCODE popn_jnz;
extern OPCODE popn_jo;
extern OPCODE popn_jno;
extern OPCODE popn_jp;
extern OPCODE popn_jnp;
extern OPCODE popn_jpe;
extern OPCODE popn_jpo;
extern OPCODE popn_js;
extern OPCODE popn_jns;
extern OPCODE popn_jmp;
extern OPCODE popn_lahf;
extern OPCODE popn_lar;
extern OPCODE popn_lds;
extern OPCODE popn_les;
extern OPCODE popn_lfs;
extern OPCODE popn_lgs;
extern OPCODE popn_lss;
extern OPCODE popn_lea;
extern OPCODE popn_leave;
extern OPCODE popn_lfence;
extern OPCODE popn_lgdt;
extern OPCODE popn_lidt;
extern OPCODE popn_lldt;
extern OPCODE popn_lmsw;
extern OPCODE popn_lock;
extern OPCODE popn_lods;
extern OPCODE popn_lodsb;
extern OPCODE popn_lodsw;
extern OPCODE popn_lodsd;
extern OPCODE popn_loop;
extern OPCODE popn_loope;
extern OPCODE popn_loopz;
extern OPCODE popn_loopne;
extern OPCODE popn_loopnz;
extern OPCODE popn_lsl;
extern OPCODE popn_ltr;
extern OPCODE popn_mfence;
extern OPCODE popn_mov;
extern OPCODE popn_movs;
extern OPCODE popn_movsb;
extern OPCODE popn_movsw;
extern OPCODE popn_movsd;
extern OPCODE popn_movsx;
extern OPCODE popn_movzx;
extern OPCODE popn_mul;
extern OPCODE popn_neg;
extern OPCODE popn_not;
extern OPCODE popn_nop;
extern OPCODE popn_or;
extern OPCODE popn_out;
extern OPCODE popn_outs;
extern OPCODE popn_outsb;
extern OPCODE popn_outsw;
extern OPCODE popn_outsd;
extern OPCODE popn_pop;
extern OPCODE popn_popa;
extern OPCODE popn_popad;
extern OPCODE popn_popf;
extern OPCODE popn_popfd;
extern OPCODE popn_push;
extern OPCODE popn_pusha;
extern OPCODE popn_pushad;
extern OPCODE popn_pushf;
extern OPCODE popn_pushfd;
extern OPCODE popn_rcl;
extern OPCODE popn_rcr;
extern OPCODE popn_rdmsr;
extern OPCODE popn_rdpmc;
extern OPCODE popn_rdtsc;
extern OPCODE popn_rol;
extern OPCODE popn_ror;
extern OPCODE popn_repnz;
extern OPCODE popn_repz;
extern OPCODE popn_ret;
extern OPCODE popn_retf;
extern OPCODE popn_sahf;
extern OPCODE popn_sal;
extern OPCODE popn_sar;
extern OPCODE popn_shl;
extern OPCODE popn_shr;
extern OPCODE popn_sbb;
extern OPCODE popn_scas;
extern OPCODE popn_scasb;
extern OPCODE popn_scasw;
extern OPCODE popn_scasd;
extern OPCODE popn_seta;
extern OPCODE popn_setnbe;
extern OPCODE popn_setae;
extern OPCODE popn_setnb;
extern OPCODE popn_setnc;
extern OPCODE popn_setb;
extern OPCODE popn_setc;
extern OPCODE popn_setnae;
extern OPCODE popn_setbe;
extern OPCODE popn_setna;
extern OPCODE popn_sete;
extern OPCODE popn_setz;
extern OPCODE popn_setg;
extern OPCODE popn_setnle;
extern OPCODE popn_setl;
extern OPCODE popn_setnge;
extern OPCODE popn_setge;
extern OPCODE popn_setnl;
extern OPCODE popn_setle;
extern OPCODE popn_setng;
extern OPCODE popn_setne;
extern OPCODE popn_setnz;
extern OPCODE popn_seto;
extern OPCODE popn_setno;
extern OPCODE popn_setp;
extern OPCODE popn_setnp;
extern OPCODE popn_setpe;
extern OPCODE popn_setpo;
extern OPCODE popn_sets;
extern OPCODE popn_setns;
extern OPCODE popn_sfence;
extern OPCODE popn_sgdt;
extern OPCODE popn_sidt;
extern OPCODE popn_sldt;
extern OPCODE popn_smsw;
extern OPCODE popn_shld;
extern OPCODE popn_shrd;
extern OPCODE popn_stc;
extern OPCODE popn_std;
extern OPCODE popn_sti;
extern OPCODE popn_stos;
extern OPCODE popn_stosb;
extern OPCODE popn_stosw;
extern OPCODE popn_stosd;
extern OPCODE popn_str;
extern OPCODE popn_sub;
extern OPCODE popn_test;
extern OPCODE popn_verr;
extern OPCODE popn_verw;
extern OPCODE popn_wait;
extern OPCODE popn_wbinvd;
extern OPCODE popn_wrmsr;
extern OPCODE popn_xchg;
extern OPCODE popn_xlat;
extern OPCODE popn_xlatb;
extern OPCODE popn_xor;
extern OPCODE popn_f2xm1;
extern OPCODE popn_fabs;
extern OPCODE popn_fadd;
extern OPCODE popn_faddp;
extern OPCODE popn_fiadd;
extern OPCODE popn_fchs;
extern OPCODE popn_fclex;
extern OPCODE popn_fnclex;
extern OPCODE popn_fcom;
extern OPCODE popn_fcomp;
extern OPCODE popn_fcompp;
extern OPCODE popn_fcos;
extern OPCODE popn_fdecstp;
extern OPCODE popn_fdiv;
extern OPCODE popn_fdivp;
extern OPCODE popn_fidiv;
extern OPCODE popn_fdivr;
extern OPCODE popn_fdivrp;
extern OPCODE popn_fidivr;
extern OPCODE popn_ffree;
extern OPCODE popn_ficom;
extern OPCODE popn_ficomp;
extern OPCODE popn_fild;
extern OPCODE popn_fincstp;
extern OPCODE popn_finit;
extern OPCODE popn_fninit;
extern OPCODE popn_fist;
extern OPCODE popn_fistp;
extern OPCODE popn_fld;
extern OPCODE popn_fldz;
extern OPCODE popn_fldpi;
extern OPCODE popn_fld1;
extern OPCODE popn_fldl2t;
extern OPCODE popn_fldl2e;
extern OPCODE popn_fldlg2;
extern OPCODE popn_fldln2;
extern OPCODE popn_fldcw;
extern OPCODE popn_fldenv;
extern OPCODE popn_fmul;
extern OPCODE popn_fmulp;
extern OPCODE popn_fimul;
extern OPCODE popn_fpatan;
extern OPCODE popn_fprem;
extern OPCODE popn_fprem1;
extern OPCODE popn_fptan;
extern OPCODE popn_frndint;
extern OPCODE popn_frstor;
extern OPCODE popn_fsave;
extern OPCODE popn_fnsave;
extern OPCODE popn_fscale;
extern OPCODE popn_fsin;
extern OPCODE popn_fsincos;
extern OPCODE popn_fsqrt;
extern OPCODE popn_fst;
extern OPCODE popn_fstp;
extern OPCODE popn_fstcw;
extern OPCODE popn_fstsw;
extern OPCODE popn_fnstcw;
extern OPCODE popn_fnstsw;
extern OPCODE popn_fstenv;
extern OPCODE popn_fnstenv;
extern OPCODE popn_fsub;
extern OPCODE popn_fsubp;
extern OPCODE popn_fisub;
extern OPCODE popn_fsubr;
extern OPCODE popn_fsubrp;
extern OPCODE popn_fisubr;
extern OPCODE popn_ftst;
extern OPCODE popn_fucom;
extern OPCODE popn_fucomp;
extern OPCODE popn_fucompp;
extern OPCODE popn_fwait;
extern OPCODE popn_fxam;
extern OPCODE popn_fxch;
extern OPCODE popn_fxtract;
extern OPCODE popn_fyl2x;
extern OPCODE popn_fyl2xp1;


int skipsize = 0;
int addsize = 0;

/*      variable initialization         */

char segregs[] = "csdsesfsgsss";

extern int prm_assembler;

static int uses_float;

MULDIV *muldivlink = 0;
static enum e_gt oa_gentype = nogen; /* Current DC type */
enum e_sg oa_currentSeg = noseg; /* Current seg */
static int oa_outcol = 0; /* Curront col (roughly) */
int newlabel;
int needpointer;
static int nosize = 0;
static int virtual_mode;

/* List of opcodes
 * This list MUST be in the same order as the op_ enums 
 */
ASMNAME oplst[] = 
{
    {
        "reserved", op_reserved, 0, 0
    } , 
    {
        "line#", op_reserved, 0, 0
    }
    , 
    {
        "blks#", op_reserved, 0, 0
    }
    , 
    {
        "blke#", op_reserved, 0, 0
    }
    , 
    {
        "vars#", op_reserved, 0, 0
    }
    , 
    {
        "funcs#", op_reserved, 0, 0
    }
    , 
    {
        "funce#", op_reserved, 0, 0
    }
    , 
    {
        "void#", op_void, 0, 0
    }
    , 
    {
        "cmt#", op_reserved, 0, 0
    }
    , 
    {
        "label#", op_reserved, 0, 0
    }
    , 
    {
        "flabel#", op_reserved, 0, 0
    }
    , 
    {
        "seq@", op_reserved, 0, 0
    }
    , 
    {
        "db", op_reserved, 0, 0
    }
    , 
    {
        "dd", op_reserved, 0, 0
    }
    , 
    {
        "aaa", op_aaa, 0, &popn_aaa
    }
    , 
    {
        "aad", op_aad, 0, &popn_aad
    }
    , 
    {
        "aam", op_aam, 0, &popn_aam
    }
    , 
    {
        "aas", op_aas, 0, &popn_aas
    }
    , 
    {
        "add", op_add, OPE_MATH, &popn_add
    }
    , 
    {
        "adc", op_adc, OPE_MATH, &popn_adc
    }
    , 
    {
        "and", op_and, OPE_MATH, &popn_and
    }
    , 
    {
        "arpl", op_arpl, OPE_ARPL, &popn_arpl
    }
    , 
    {
        "bound", op_bound, OPE_BOUND, &popn_bound
    }
    , 
    {
        "bsf", op_bsf, OPE_BITSCAN, &popn_bsf
    }
    , 
    {
        "bsr", op_bsr, OPE_BITSCAN, &popn_bsr
    }
    , 
    {
        "bswap", op_bswap, OPE_REG32, &popn_bswap
    }
    , 
    {
        "btc", op_btc, OPE_BIT, &popn_btc
    }
    , 
    {
        "bt", op_bt, OPE_BIT, &popn_bt
    }
    , 
    {
        "btr", op_btr, OPE_BIT, &popn_btr
    }
    , 
    {
        "bts", op_bts, OPE_BIT, &popn_bts
    }
    , 
    {
        "call", op_call, OPE_CALL, &popn_call
    }
    , 
    {
        "cbw", op_cbw, 0, &popn_cbw
    }
    , 
    {
        "cwde", op_cwde, 0, &popn_cwde
    }
    , 
    {
        "cwd", op_cwd, 0, &popn_cwd
    }
    , 
    {
        "cdq", op_cdq, 0, &popn_cdq
    }
    , 
    {
        "clc", op_clc, 0, &popn_clc
    }
    , 
    {
        "cld", op_cld, 0, &popn_cld
    }
    , 
    {
        "cli", op_cli, 0, &popn_cli
    }
    , 
    {
        "clts", op_clts, 0, &popn_clts
    }
    , 
    {
        "cmc", op_cmc, 0, &popn_cmc
    }
    , 
    {
        "cmp", op_cmp, OPE_MATH, &popn_cmp
    }
    , 
    {
        "cmpxchg", op_cmp, OPE_MATH, &popn_cmpxchg
    }
    , 
    {
        "cmpxchg8b", op_cmp, 0, &popn_cmpxchg8b // fixme, the inline assembler is broke
    }
    , 
    {
        "cmps", op_cmps, OPE_CMPS, &popn_cmps
    }
    , 
    {
        "cmpsb", op_cmpsb, 0, &popn_cmpsb
    }
    , 
    {
        "cmpsw", op_cmpsw, 0, &popn_cmpsw
    }
    , 
    {
        "cmpsd", op_cmpsd, 0, &popn_cmpsd
    }
    , 
    {
        "daa", op_daa, 0, &popn_daa
    }
    , 
    {
        "das", op_das, 0, &popn_das
    }
    , 
    {
        "dec", op_dec, OPE_INCDEC, &popn_dec
    }
    , 
    {
        "div", op_div, OPE_RM, &popn_div
    }
    , 
    {
        "enter", op_enter, OPE_ENTER, &popn_enter
    }
    , 
    {
        "hlt", op_hlt, 0, &popn_hlt
    }
    , 
    {
        "idiv", op_idiv, OPE_RM, &popn_idiv
    }
    , 
    {
        "imul", op_imul, OPE_IMUL, &popn_imul
    }
    , 
    {
        "in", op_in, OPE_IN, &popn_in
    }
    , 
    {
        "inc", op_inc, OPE_INCDEC, &popn_inc
    }
    , 
    {
        "ins", op_ins, OPE_INS, &popn_ins
    }
    , 
    {
        "insb", op_insb, 0, &popn_insb
    }
    , 
    {
        "insw", op_insw, 0, &popn_insw
    }
    , 
    {
        "insd", op_insd, 0, &popn_insd
    }
    , 
    {
        "int", op_int, OPE_IMM8, &popn_int
    }
    , 
    {
        "int3", op_int3, 0, &popn_int3
    }
    , 
    {
        "into", op_into, 0, &popn_into
    }
    , 
    {
        "invd", op_invd, 0, &popn_invd
    }
    , 
    {
        "iret", op_iret, 0, &popn_iret
    }
    , 
    {
        "iretd", op_iretd, 0, &popn_iretd
    }
    , 
    {
        "jecxz", op_jecxz, OPE_RELBR8, &popn_jcxz
    }
    , 
    {
        "ja", op_ja, OPE_RELBRA, &popn_ja
    }
    , 
    {
        "jnbe", op_jnbe, OPE_RELBRA, &popn_jnbe
    }
    , 
    {
        "jae", op_jae, OPE_RELBRA, &popn_jae
    }
    , 
    {
        "jnb", op_jnb, OPE_RELBRA, &popn_jnb
    }
    , 
    {
        "jnc", op_jnc, OPE_RELBRA, &popn_jnc
    }
    , 
    {
        "jb", op_jb, OPE_RELBRA, &popn_jb
    }
    , 
    {
        "jc", op_jc, OPE_RELBRA, &popn_jc
    }
    , 
    {
        "jnae", op_jnae, OPE_RELBRA, &popn_jnae
    }
    , 
    {
        "jbe", op_jbe, OPE_RELBRA, &popn_jbe
    }
    , 
    {
        "jna", op_jna, OPE_RELBRA, &popn_jna
    }
    , 
    {
        "je", op_je, OPE_RELBRA, &popn_je
    }
    , 
    {
        "jz", op_jz, OPE_RELBRA, &popn_jz
    }
    , 
    {
        "jg", op_jg, OPE_RELBRA, &popn_jg
    }
    , 
    {
        "jnle", op_jnle, OPE_RELBRA, &popn_jnle
    }
    , 
    {
        "jl", op_jl, OPE_RELBRA, &popn_jl
    }
    , 
    {
        "jnge", op_jnge, OPE_RELBRA, &popn_jnge
    }
    , 
    {
        "jge", op_jge, OPE_RELBRA, &popn_jge
    }
    , 
    {
        "jnl", op_jnl, OPE_RELBRA, &popn_jnl
    }
    , 
    {
        "jle", op_jle, OPE_RELBRA, &popn_jle
    }
    , 
    {
        "jng", op_jng, OPE_RELBRA, &popn_jng
    }
    , 
    {
        "jne", op_jne, OPE_RELBRA, &popn_jne
    }
    , 
    {
        "jnz", op_jnz, OPE_RELBRA, &popn_jnz
    }
    , 
    {
        "jo", op_jo, OPE_RELBRA, &popn_jo
    }
    , 
    {
        "jno", op_jno, OPE_RELBRA, &popn_jno
    }
    , 
    {
        "jp", op_jp, OPE_RELBRA, &popn_jp
    }
    , 
    {
        "jnp", op_jnp, OPE_RELBRA, &popn_jnp
    }
    , 
    {
        "jpe", op_jpe, OPE_RELBRA, &popn_jpe
    }
    , 
    {
        "jpo", op_jpo, OPE_RELBRA, &popn_jpo
    }
    , 
    {
        "js", op_js, OPE_RELBRA, &popn_js
    }
    , 
    {
        "jns", op_jns, OPE_RELBRA, &popn_jns
    }
    , 
    {
        "jmp", op_jmp, OPE_JMP, &popn_jmp
    }
    , 
    {
        "lahf", op_lahf, 0, &popn_lahf
    }
    , 
    {
        "lar", op_lar, OPE_REGRM, &popn_lar
    }
    , 
    {
        "lds", op_lds, OPE_LOADSEG, &popn_lds
    }
    , 
    {
        "les", op_les, OPE_LOADSEG, &popn_les
    }
    , 
    {
        "lfs", op_lfs, OPE_LOADSEG, &popn_lfs
    }
    , 
    {
        "lgs", op_lgs, OPE_LOADSEG, &popn_lgs
    }
    , 
    {
        "lss", op_lss, OPE_LOADSEG, &popn_lss
    }
    , 
    {
        "lea", op_lea, OPE_REGRM, &popn_lea
    }
    , 
    {
        "leave", op_leave, 0, &popn_leave
    }
    , 
    {
        "lfence", op_lfence, 0, &popn_lfence
    }
    , 
    {
        "lgdt", op_lgdt, OPE_LGDT, &popn_lgdt
    }
    , 
    {
        "lidt", op_lidt, OPE_LIDT, &popn_lidt
    }
    , 
    {
        "lldt", op_lldt, OPE_RM16, &popn_lldt
    }
    , 
    {
        "lmsw", op_lmsw, OPE_RM16, &popn_lmsw
    }
    , 
    {
        "lock", op_lock, 0, &popn_lock
    }
    , 
    {
        "lods", op_lods, OPE_LODS, &popn_lods
    }
    , 
    {
        "lodsb", op_lodsb, 0, &popn_lodsb
    }
    , 
    {
        "lodsw", op_lodsw, 0, &popn_lodsw
    }
    , 
    {
        "lodsd", op_lodsd, 0, &popn_lodsd
    }
    , 
    {
        "loop", op_loop, OPE_RELBR8, &popn_loop
    }
    , 
    {
        "loope", op_loope, OPE_RELBR8, &popn_loope
    }
    , 
    {
        "loopz", op_loopz, OPE_RELBR8, &popn_loopz
    }
    , 
    {
        "loopne", op_loopne, OPE_RELBR8, &popn_loopne
    }
    , 
    {
        "loopnz", op_loopnz, OPE_RELBR8, &popn_loopnz
    }
    , 
    {
        "lsl", op_lsl, OPE_REGRM, &popn_lsl
    }
    , 
    {
        "ltr", op_ltr, OPE_RM16, &popn_ltr
    }
    , 
    {
        "mfence", op_mfence, 0, &popn_mfence
    }
    , 
    {
        "mov", op_mov, OPE_MOV, &popn_mov
    }
    , 
    {
        "movs", op_movs, OPE_MOVS, &popn_movs
    }
    , 
    {
        "movsb", op_movsb, 0, &popn_movsb
    }
    , 
    {
        "movsw", op_movsw, 0, &popn_movsw
    }
    , 
    {
        "movsd", op_movsd, 0, &popn_movsd
    }
    , 
    {
        "movsx", op_movsx, OPE_MOVSX, &popn_movsx
    }
    , 
    {
        "movzx", op_movzx, OPE_MOVSX, &popn_movzx
    }
    , 
    {
        "mul", op_mul, OPE_RM, &popn_mul
    }
    , 
    {
        "neg", op_neg, OPE_RM, &popn_neg
    }
    , 
    {
        "not", op_not, OPE_RM, &popn_not
    }
    , 
    {
        "nop", op_nop, 0, &popn_nop
    }
    , 
    {
        "or", op_or, OPE_MATH, &popn_or
    }
    , 
    {
        "out", op_out, OPE_OUT, &popn_out
    }
    , 
    {
        "outs", op_outs, OPE_OUTS, &popn_outs
    }
    , 
    {
        "outsb", op_outsb, 0, &popn_outsb
    }
    , 
    {
        "outsw", op_outsw, 0, &popn_outsw
    }
    , 
    {
        "outsd", op_outsd, 0, &popn_outsd
    }
    , 
    {
        "pop", op_pop, OPE_PUSHPOP, &popn_pop
    }
    , 
    {
        "popa", op_popa, 0, &popn_popa
    }
    , 
    {
        "popad", op_popad, 0, &popn_popad
    }
    , 
    {
        "popf", op_popf, 0, &popn_popf
    }
    , 
    {
        "popfd", op_popfd, 0, &popn_popfd
    }
    , 
    {
        "push", op_push, OPE_PUSHPOP, &popn_push
    }
    , 
    {
        "pusha", op_pusha, 0, &popn_pusha
    }
    , 
    {
        "pushad", op_pushad, 0, &popn_pushad
    }
    , 
    {
        "pushf", op_pushf, 0, &popn_pushf
    }
    , 
    {
        "pushfd", op_pushfd, 0, &popn_pushfd
    }
    , 
    {
        "rcl", op_rcl, OPE_SHIFT, &popn_rcl
    }
    , 
    {
        "rcr", op_rcr, OPE_SHIFT, &popn_rcr
    }
    , 
    {
        "rdmsr", op_rdmsr, 0, &popn_rdmsr
    }
    , 
    {
        "rdpmc", op_rdpmc, 0, &popn_rdpmc
    }
    , 
    {
        "rdtsc", op_rdtsc, 0, &popn_rdtsc
    }
    , 
    {
        "rol", op_rol, OPE_SHIFT, &popn_rol
    }
    , 
    {
        "ror", op_ror, OPE_SHIFT, &popn_ror
    }
    , 
    {
        "rep", op_rep, 0, &popn_repz
    }
    , 
    {
        "repne", op_repne, 0, &popn_repnz
    }
    , 
    {
        "repe", op_repe, 0, &popn_repz
    }
    , 
    {
        "repnz", op_repnz, 0, &popn_repnz
    }
    , 
    {
        "repz", op_repz, 0, &popn_repz
    }
    , 
    {
        "ret", op_ret, OPE_RET, &popn_ret
    }
    , 
    {
        "retf", op_retf, OPE_RET, &popn_retf
    }
    , 
    {
        "sahf", op_sahf, 0, &popn_sahf
    }
    , 
    {
        "sal", op_sal, OPE_SHIFT, &popn_sal
    }
    , 
    {
        "sar", op_sar, OPE_SHIFT, &popn_sar
    }
    , 
    {
        "shl", op_shl, OPE_SHIFT, &popn_shl
    }
    , 
    {
        "shr", op_shr, OPE_SHIFT, &popn_shr
    }
    , 
    {
        "sbb", op_sbb, OPE_MATH, &popn_sbb
    }
    , 
    {
        "scas", op_scas, OPE_SCAS, &popn_scas
    }
    , 
    {
        "scasb", op_scasb, 0, &popn_scasb
    }
    , 
    {
        "scasw", op_scasw, 0, &popn_scasw
    }
    , 
    {
        "scasd", op_scasd, 0, &popn_scasd
    }
    , 
    {
        "seta", op_seta, OPE_SET, &popn_seta
    }
    , 
    {
        "setnbe", op_setnbe, OPE_SET, &popn_setnbe
    }
    , 
    {
        "setae", op_setae, OPE_SET, &popn_setae
    }
    , 
    {
        "setnb", op_setnb, OPE_SET, &popn_setnb
    }
    , 
    {
        "setnc", op_setnc, OPE_SET, &popn_setnc
    }
    , 
    {
        "setb", op_setb, OPE_SET, &popn_setb
    }
    , 
    {
        "setc", op_setc, OPE_SET, &popn_setc
    }
    , 
    {
        "setnae", op_setnae, OPE_SET, &popn_setnae
    }
    , 
    {
        "setbe", op_setbe, OPE_SET, &popn_setbe
    }
    , 
    {
        "setna", op_setna, OPE_SET, &popn_setna
    }
    , 
    {
        "sete", op_sete, OPE_SET, &popn_sete
    }
    , 
    {
        "setz", op_setz, OPE_SET, &popn_setz
    }
    , 
    {
        "setg", op_setg, OPE_SET, &popn_setg
    }
    , 
    {
        "setnle", op_setnle, OPE_SET, &popn_setnle
    }
    , 
    {
        "setl", op_setl, OPE_SET, &popn_setl
    }
    , 
    {
        "setnge", op_setnge, OPE_SET, &popn_setnge
    }
    , 
    {
        "setge", op_setge, OPE_SET, &popn_setge
    }
    , 
    {
        "setnl", op_setnl, OPE_SET, &popn_setnl
    }
    , 
    {
        "setle", op_setle, OPE_SET, &popn_setle
    }
    , 
    {
        "setng", op_setng, OPE_SET, &popn_setng
    }
    , 
    {
        "setne", op_setne, OPE_SET, &popn_setne
    }
    , 
    {
        "setnz", op_setnz, OPE_SET, &popn_setnz
    }
    , 
    {
        "seto", op_seto, OPE_SET, &popn_seto
    }
    , 
    {
        "setno", op_setno, OPE_SET, &popn_setno
    }
    , 
    {
        "setp", op_setp, OPE_SET, &popn_setp
    }
    , 
    {
        "setnp", op_setnp, OPE_SET, &popn_setnp
    }
    , 
    {
        "setpe", op_setpe, OPE_SET, &popn_setpe
    }
    , 
    {
        "setpo", op_setpo, OPE_SET, &popn_setpo
    }
    , 
    {
        "sets", op_sets, OPE_SET, &popn_sets
    }
    , 
    {
        "setns", op_setns, OPE_SET, &popn_setns
    }
    , 
    {
        "sfence", op_sfence, 0, &popn_sfence
    }
    , 
    {
        "sgdt", op_sgdt, OPE_LGDT, &popn_sgdt
    }
    , 
    {
        "sidt", op_sidt, OPE_LIDT, &popn_sidt
    }
    , 
    {
        "sldt", op_sldt, OPE_RM16, &popn_sldt
    }
    , 
    {
        "smsw", op_smsw, OPE_RM16, &popn_smsw
    }
    , 
    {
        "shld", op_shld, OPE_SHLD, &popn_shld
    }
    , 
    {
        "shrd", op_shrd, OPE_SHLD, &popn_shrd
    }
    , 
    {
        "stc", op_stc, 0, &popn_stc
    }
    , 
    {
        "std", op_std, 0, &popn_std
    }
    , 
    {
        "sti", op_sti, 0, &popn_sti
    }
    , 
    {
        "stos", op_stos, OPE_STOS, &popn_stos
    }
    , 
    {
        "stosb", op_stosb, 0, &popn_stosb
    }
    , 
    {
        "stosw", op_stosw, 0, &popn_stosw
    }
    , 
    {
        "stosd", op_stosd, 0, &popn_stosd
    }
    , 
    {
        "str", op_str, OPE_RM16, &popn_str
    }
    , 
    {
        "sub", op_sub, OPE_MATH, &popn_sub
    }
    , 
    {
        "test", op_test, OPE_TEST, &popn_test
    }
    , 
    {
        "verr", op_verr, OPE_RM16, &popn_verr
    }
    , 
    {
        "verw", op_verw, OPE_RM16, &popn_verw
    }
    , 
    {
        "wait", op_wait, 0, &popn_wait
    }
    , 
    {
        "wbinvd", op_wbinvd, 0, &popn_wbinvd
    }
    , 
    {
        "wrmsr", op_wrmsr, 0, &popn_wrmsr
    }
    , 
    {
        "xchg", op_xchg, OPE_XCHG, &popn_xchg
    }
    , 
    {
        "xlat", op_xlat, OPE_XLAT, &popn_xlat
    }
    , 
    {
        "xlatb", op_xlatb, 0, &popn_xlatb
    }
    , 
    {
        "xor", op_xor, OPE_MATH, &popn_xor
    }
    , 
    {
        "f2xm1", op_f2xm1, 0, &popn_f2xm1
    }
    , 
    {
        "fabs", op_fabs, 0, &popn_fabs
    }
    , 
    {
        "fadd", op_fadd, OPE_FMATH, &popn_fadd
    }
    , 
    {
        "faddp", op_faddp, OPE_FMATHP, &popn_faddp
    }
    , 
    {
        "fiadd", op_fiadd, OPE_FMATHI, &popn_fiadd
    }
    , 
    {
        "fchs", op_fchs, 0, &popn_fchs
    }
    , 
    {
        "fclex", op_fclex, 0, &popn_fclex
    }
    , 
    {
        "fnclex", op_fnclex, 0, &popn_fnclex
    }
    , 
    {
        "fcom", op_fcom, OPE_FCOM, &popn_fcom
    }
    , 
    {
        "fcomp", op_fcomp, OPE_FCOM, &popn_fcomp
    }
    , 
    {
        "fcompp", op_fcompp, 0, &popn_fcompp
    }
    , 
    {
        "fcos", op_fcos, 0, &popn_fcos
    }
    , 
    {
        "fdecstp", op_fdecstp, 0, &popn_fdecstp
    }
    , 
    {
        "fdiv", op_fdiv, OPE_FMATH, &popn_fdiv
    }
    , 
    {
        "fdivp", op_fdivp, OPE_FMATHP, &popn_fdivp
    }
    , 
    {
        "fidiv", op_fidiv, OPE_FMATHI, &popn_fidiv
    }
    , 
    {
        "fdivr", op_fdivr, OPE_FMATH, &popn_fdivr
    }
    , 
    {
        "fdivrp", op_fdivrp, OPE_FMATHP, &popn_fdivrp
    }
    , 
    {
        "fidivr", op_fidivr, OPE_FMATHI, &popn_fidivr
    }
    , 
    {
        "ffree", op_ffre, OPE_FREG, &popn_ffree
    }
    , 
    {
        "ficom", op_ficom, OPE_FICOM, &popn_ficom
    }
    , 
    {
        "ficomp", op_ficomp, OPE_FICOM, &popn_ficomp
    }
    , 
    {
        "fild", op_fild, OPE_FILD, &popn_fild
    }
    , 
    {
        "fincstp", op_fincstp, 0, &popn_fincstp
    }
    , 
    {
        "finit", op_finit, 0, &popn_finit
    }
    , 
    {
        "fninit", op_fninit, 0, &popn_fninit
    }
    , 
    {
        "fist", op_fist, OPE_FIST, &popn_fist
    }
    , 
    {
        "fistp", op_fistp, OPE_FILD, &popn_fistp
    }
    , 
    {
        "fld", op_fld, OPE_FLD, &popn_fld
    }
    , 
    {
        "fldz", op_fldz, 0, &popn_fldz
    }
    , 
    {
        "fldpi", op_fldpi, 0, &popn_fldpi
    }
    , 
    {
        "fld1", op_fld1, 0, &popn_fld1
    }
    , 
    {
        "fldl2t", op_fld2t, 0, &popn_fldl2t
    }
    , 
    {
        "fldl2e", op_fld2e, 0, &popn_fldl2e
    }
    , 
    {
        "fldlg2", op_fldlg2, 0, &popn_fldlg2
    }
    , 
    {
        "fldln2", op_fldln2, 0, &popn_fldln2
    }
    , 
    {
        "fldcw", op_fldcw, OPE_M16, &popn_fldcw
    }
    , 
    {
        "fldenv", op_fldenv, OPE_MN, &popn_fldenv
    }
    , 
    {
        "fmul", op_fmul, OPE_FMATH, &popn_fmul
    }
    , 
    {
        "fmulp", op_fmulp, OPE_FMATHP, &popn_fmulp
    }
    , 
    {
        "fimul", op_fimul, OPE_FMATHI, &popn_fimul
    }
    , 
    {
        "fpatan", op_fpatan, 0, &popn_fpatan
    }
    , 
    {
        "fprem", op_fprem, 0, &popn_fprem
    }
    , 
    {
        "fprem1", op_fprem1, 0, &popn_fprem1
    }
    , 
    {
        "fptan", op_fptan, 0, &popn_fptan
    }
    , 
    {
        "frndint", op_frndint, 0, &popn_frndint
    }
    , 
    {
        "frstor", op_frstor, OPE_MN, &popn_frstor
    }
    , 
    {
        "fsave", op_fsave, OPE_MN, &popn_fsave
    }
    , 
    {
        "fnsave", op_fnsave, OPE_MN, &popn_fnsave
    }
    , 
    {
        "fscale", op_fscale, 0, &popn_fscale
    }
    , 
    {
        "fsin", op_fsin, 0, &popn_fsin
    }
    , 
    {
        "fsincos", op_fsincos, 0, &popn_fsincos
    }
    , 
    {
        "fsqrt", op_fsqrt, 0, &popn_fsqrt
    }
    , 
    {
        "fst", op_fst, OPE_FST, &popn_fst
    }
    , 
    {
        "fstp", op_fstp, OPE_FSTP, &popn_fstp
    }
    , 
    {
        "fstcw", op_fstcw, OPE_M16, &popn_fstcw
    }
    , 
    {
        "fstsw", op_fstsw, OPE_M16, &popn_fstsw
    }
    , 
    {
        "fnstcw", op_fnstcw, OPE_M16, &popn_fnstcw
    }
    , 
    {
        "fnstsw", op_fnstsw, OPE_M16, &popn_fnstsw
    }
    , 
    {
        "fstenv", op_fstenv, OPE_MN, &popn_fstenv
    }
    , 
    {
        "fnstenv", op_fsntenv, OPE_MN, &popn_fnstenv
    }
    , 
    {
        "fsub", op_fsub, OPE_FMATH, &popn_fsub
    }
    , 
    {
        "fsubp", op_fsubp, OPE_FMATHP, &popn_fsubp
    }
    , 
    {
        "fisub", op_fisub, OPE_FMATHI, &popn_fisub
    }
    , 
    {
        "fsubr", op_fsubr, OPE_FMATH, &popn_fsubr
    }
    , 
    {
        "fsubrp", op_fsubrp, OPE_FMATHP, &popn_fsubrp
    }
    , 
    {
        "fisubr", op_fisubr, OPE_FMATHI, &popn_fisubr
    }
    , 
    {
        "ftst", op_ftst, 0, &popn_ftst
    }
    , 
    {
        "fucom", op_fucom, OPE_FUCOM, &popn_fucom
    }
    , 
    {
        "fucomp", op_fucomp, OPE_FUCOM, &popn_fucomp
    }
    , 
    {
        "fucompp", op_fucompp, 0, &popn_fucompp
    }
    , 
    {
        "fwait", op_fwait, 0, &popn_fwait
    }
    , 
    {
        "fxam", op_fxam, 0, &popn_fxam
    }
    , 
    {
        "fxch", op_fxch, OPE_FXCH, &popn_fxch
    }
    , 
    {
        "fxtract", op_fxtract, 0, &popn_fxtract
    }
    , 
    {
        "fyl2x", op_fyl2x, 0, &popn_fyl2x
    }
    , 
    {
        "fyl2xp1", op_fyl2xp1, 0, &popn_fyl2xp1
    }
    , 
    {
        0, 0, 0
    }
    , 
};
/* Init module */
void oa_ini(void)
{
    oa_gentype = nogen;
    oa_currentSeg = noseg;
    oa_outcol = 0;
    newlabel = FALSE;
    muldivlink = 0;
}

/*-------------------------------------------------------------------------*/

void oa_nl(void)
/*
 * New line
 */
{
    if (cparams.prm_asmfile)
    {
        if (oa_outcol > 0)
        {
            beputc('\n');
            oa_outcol = 0;
            oa_gentype = nogen;
        }
    }
}

/* Put an opcode
 */
void outop(char *name)
{
    beputc('\t');
    while (*name)
        beputc(*name++);
}

/*-------------------------------------------------------------------------*/

void putop(enum e_op op, AMODE *aps, AMODE *apd, int nooptx)
{
    if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
    {
        skipsize = FALSE;
        addsize = FALSE;
        switch (op)
        {
            case op_lea:
                skipsize = TRUE;
                break;
            case op_push:
                addsize = TRUE;
                if (!aps->length)
                    aps->length = ISZ_UINT;
                break;
            case op_add:
            case op_sub:
            case op_adc:
            case op_sbb:
            case op_imul:
                 /* yes you can size an imul constant !!!! */
            case op_cmp:
            case op_and:
            case op_or:
            case op_xor:
            case op_idiv:
                if (apd)
                    addsize = apd->length != 0;
                else
                {
                    addsize = TRUE;
                    if (!aps->length)
                        aps->length = ISZ_UINT;
                }
                break;
            default:
                break;
        }
        if (op == op_fwait)
        {
            outop(oplst[op_fwait].name + 1);
            return ;
        }
        if (!nooptx)
        {
            switch (op)
            {
                case op_iretd:
                    outop("iret");
                    return ;
                case op_pushad:
                    outop("pusha");
                    return ;
                case op_popad:
                    outop("popa");
                    return ;
                case op_pushfd:
                    outop("pushf");
                    return ;
                case op_popfd:
                    outop("popf");
                    return ;
                default:
                    break;
            }
        }

    }
    if (op > op_fyl2xp1)
        diag("illegal opcode.");
    else
        outop(oplst[op].name);
    uses_float = (op >= op_f2xm1);
}

/*-------------------------------------------------------------------------*/

void oa_putconst(int sz, EXPRESSION *offset, BOOLEAN doSign)
/*
 *      put a constant to the outputFile file.
 */
{
    char buf[4096];
    SYMBOL *sp;
    char buf1[100];
    int toffs;
    switch (offset->type)
    {
        case en_auto:
            if (doSign)
            {
                if ((int)offset->v.sp->offset < 0)
                    bePrintf( "-0%lxh", -offset->v.sp->offset);
                else
                    bePrintf( "+0%lxh", offset->v.sp->offset);
            }
            else
                bePrintf( "0%lxh", offset->v.sp->offset);
                
            break;
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_ll:
        case en_c_ull:
        case en_absolute:
        case en_c_c:
        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_bool:
        case en_c_s:
        case en_c_us:
        case en_c_wc:
            if (doSign)
            {
                if (offset->v.i == 0)
                    break;
                beputc('+');
            }
            {
                int n = offset->v.i;
//				if (sz == ISZ_UCHAR || sz == -ISZ_UCHAR)
//					n &= 0xff;
//				if (sz == ISZ_USHORT || sz == -ISZ_USHORT)
//					n &= 0xffff;
                bePrintf( "0%xh", n);
            }
            break;
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            if (doSign)
                beputc('+');
            FPFToString(buf,&offset->v.c.r);
            FPFToString(buf1, &offset->v.c.i);
            bePrintf( "%s,%s", buf, buf1);
            break;
        case en_c_f:
        case en_c_d:
        case en_c_ld:
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            if (doSign)
                beputc('+');
            FPFToString(buf,&offset->v.f);
            bePrintf( "%s", buf);
            break;
        case en_label:
            if (doSign)
                beputc('+');
            bePrintf( "L_%d", offset->v.sp->label);
            break;
        case en_labcon:
            if (doSign)
                beputc('+');
            bePrintf( "L_%d", offset->v.i);
            break;
        case en_pc:
        case en_global:
        case en_threadlocal:
            if (doSign)
                beputc('+');
            sp = offset->v.sp;
            beDecorateSymName(buf, sp);
            bePrintf( "%s", buf);
            break;
        case en_add:
        case en_structadd:
        case en_arrayadd:
            oa_putconst(ISZ_ADDR, offset->left, doSign);
            oa_putconst(ISZ_ADDR, offset->right, TRUE);
            break;
        case en_sub:
            oa_putconst(ISZ_ADDR, offset->left, doSign);
            bePrintf( "-");
            oa_putconst(ISZ_ADDR, offset->right, FALSE);
            break;
        case en_uminus:
            bePrintf( "-");
            oa_putconst(ISZ_ADDR, offset->left, FALSE);
            break;
        default:
            diag("illegal constant node.");
            break;
    }
}

/*-------------------------------------------------------------------------*/

void oa_putlen(int l)
/*
 *      append the length field to an instruction.
 */
{
    if (l < 0)
        l =  - l;
    switch(l) {
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
        case ISZ_USHORT:
        case ISZ_WCHAR:
        case ISZ_UINT:
        case ISZ_U16:
        case ISZ_U32:
        case ISZ_ULONG:
        case ISZ_ULONGLONG:
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
        case ISZ_ADDR:
        case ISZ_SEG:
        case 0:
            break;
        default:
            diag("oa_putlen: illegal length field.");
    }
}

/*-------------------------------------------------------------------------*/

void putsizedreg(char *string, int reg, int size)
{
    static char *byteregs[] = 
    {
        "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"
    };
    static char *wordregs[] = 
    {
        "ax", "cx", "dx", "bx", "sp", "bp", "si", "di"
    };
    static char *longregs[] = 
    {
        "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"
    };
    if (size < 0)
        size =  - size;
    if (size == ISZ_UINT || size == ISZ_ULONG || size == ISZ_ADDR|| size == ISZ_U32)
        bePrintf( string, longregs[reg]);
    else if (size == ISZ_BOOLEAN || size == ISZ_UCHAR)
    {
        bePrintf( string, byteregs[reg]);
    }
    else
        bePrintf( string, wordregs[reg]);
}

/*-------------------------------------------------------------------------*/

void pointersize(int size)
{
    if ((prm_assembler == pa_nasm  || prm_assembler == pa_fasm) && skipsize)
        return ;
    if (size < 0)
        size =  - size;
    /*      if (needpointer)
     */switch (size)
    {
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                bePrintf( "tword ");
            else
                bePrintf( "tbyte ");
            break;
        case ISZ_ULONGLONG:
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            /* should never happen                      */
            bePrintf( "qword ");
            break;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            if (!uses_float)
            {
                if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                    bePrintf( "dword far ");
                else
                    bePrintf( "fword ");
                break;
            }
        case ISZ_U32:
        case ISZ_UINT:
        case ISZ_ULONG:
        case ISZ_ADDR:
            bePrintf( "dword ");
            break;
        case ISZ_U16:
        case ISZ_USHORT:
        case ISZ_WCHAR:
            bePrintf( "word ");
            break;
        case ISZ_BOOLEAN:
        case ISZ_UCHAR:
            bePrintf( "byte ");
            break;
        case ISZ_NONE:
             /* for NASM with certain FP ops */
            break;
    case ISZ_FARPTR:
        bePrintf("far ");
        break;
        default:
            diag("Bad pointer");
            break;
    }
    if (!(prm_assembler == pa_nasm  || prm_assembler == pa_fasm) && size)
        bePrintf( "ptr ");
}

/*-------------------------------------------------------------------------*/

void putseg(int seg, int usecolon)
{
    if (!seg)
        return ;
    seg -= 1;
    seg <<= 1;
    beputc(segregs[seg]);
    beputc(segregs[seg + 1]);
    if (usecolon)
        beputc(':');
}

/*-------------------------------------------------------------------------*/

int islabeled(EXPRESSION *n)
{
    int rv = 0;
    switch (n->type)
    {
        case en_add:
        case en_structadd:
        case en_arrayadd:
        case en_sub:
//        case en_addstruc:
            rv |= islabeled(n->left);
            rv |= islabeled(n->right);
            break;
        case en_c_i:
        case en_c_c:
        case en_c_uc:
        case en_c_u16:
        case en_c_u32:
        case en_c_l:
        case en_c_ul:
        case en_c_ui:
        case en_c_bool:
        case en_c_wc:
        case en_c_s:
        case en_c_us:
            return 0;
        case en_labcon:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_label:
        case en_pc:
        case en_threadlocal:
            return 1;
        default:
            diag("Unexpected node type in islabeled");
            break;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

void oa_putamode(AMODE *ap)
/*
 *      output a general addressing mode.
 */
{
    int oldnasm;

    switch (ap->mode)
    {
        case am_seg:
            putseg(ap->seg, 0);
            break;
        case am_screg:
            bePrintf( "cr%d", ap->preg);
            break;
        case am_sdreg:
            bePrintf( "dr%d", ap->preg);
            break;
        case am_streg:
            bePrintf( "tr%d", ap->preg);
            break;
        case am_immed:
            if (ap->length > 0 && islabeled(ap->offset))
            {
                if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                    bePrintf( "offset ");
                else if (!nosize)
                {
                    if (ap->length == -ISZ_UCHAR || ap->length == ISZ_UCHAR)
                    {
                        bePrintf("byte ");
                    }
                    else if (ap->length == -ISZ_USHORT || ap->length == ISZ_USHORT)
                    {
                        bePrintf("word ");
                    }
                    else
                    {
                        bePrintf( "dword ");
                    }
                }
            }
            else if ((prm_assembler == pa_nasm) && addsize)
                pointersize(ap->length);
            oa_putconst(ap->length, ap->offset, FALSE);
            break;
        case am_direct:
            pointersize(ap->length);
            if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                putseg(ap->seg, TRUE);
            beputc('[');
            oldnasm = prm_assembler;
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                putseg(ap->seg, TRUE);
            prm_assembler = pa_nasm ;
            oa_putconst(ap->length, ap->offset, FALSE);
            beputc(']');
            prm_assembler = oldnasm;
            break;
        case am_dreg:
            putsizedreg("%s", ap->preg, ap->length);
            break;
        case am_freg:
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                bePrintf( "st%d", ap->preg);
            else
                bePrintf( "st(%d)", ap->preg);
            break;
        case am_indisp:
            pointersize(ap->length);
            if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                putseg(ap->seg, TRUE);
            beputc('[');
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                putseg(ap->seg, TRUE);
            putsizedreg("%s", ap->preg, ISZ_ADDR);
            if (ap->offset)
            {
                   oa_putconst(ap->length, ap->offset, TRUE);
            }
            beputc(']');
            break;
        case am_indispscale:
            {
                int scale = 1, t = ap->scale;

                while (t--)
                    scale <<= 1;
                pointersize(ap->length);
                if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
                    putseg(ap->seg, TRUE);
                beputc('[');
                if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                    putseg(ap->seg, TRUE);
                if (ap->preg !=  - 1)
                    putsizedreg("%s+", ap->preg, ISZ_ADDR);
                putsizedreg("%s", ap->sreg, ISZ_ADDR);
                if (scale != 1)
                    bePrintf( "*%d", scale);
                if (ap->offset)
                {
                    oa_putconst(ap->length, ap->offset, TRUE);
                }
                beputc(']');
            }
            break;
        default:
            diag("illegal address mode.");
            break;
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_code(OCODE *cd)
/*
 *      outputFile a generic instruction.
 */
{
    int op = cd->opcode, len = 0, len2 = 0;
    AMODE *aps = cd->oper1,  *apd = cd->oper2,  *ap3 = cd->oper3;

    if (!cparams.prm_asmfile)
        return ;
    if (op == op_blockstart || op == op_blockend || op == op_varstart || op == op_funcstart || op == op_funcend)
        return;
    if (op == op_line)
    {
        LINEDATA *ld = (LINEDATA *)aps;
        oa_nl();
        while (ld)
        {
            bePrintf( "; Line %d:\t%s\n", ld->lineno, ld->line);
            ld = ld->next;
        }
        return ;
    }
    else if (op == op_comment)
    {
        if (!cparams.prm_lines)
            return ;
        bePrintf( "%s", aps);
        return ;
    }
    else if (op == op_void)
        return ;
    if (aps)
        len = aps->length;
    if (apd)
        len2 = apd->length;
    needpointer = (len != len2) || ((!aps || aps->mode != am_dreg) && (!apd ||
        apd->mode != am_dreg));
    putop(op, aps, apd, cd->noopt);
    if ((prm_assembler == pa_nasm  || prm_assembler == pa_fasm)
        && ((op >= op_ja && op <= op_jns && op != op_jecxz) || (op ==
        op_jmp && aps->mode == am_immed && !apd)))
    {
        if (cd->branched & BR_SHORT)
            bePrintf( "\tshort");
        else
            bePrintf( "\tnear");
        nosize = TRUE;
    }
    else if (op == op_jmp && aps->mode == am_immed && aps->offset->type ==
        en_labcon && (cd->branched &BR_SHORT))
    {
        bePrintf( "\tshort");
        nosize = TRUE;
    }
    switch (op)
    {
        case op_rep:
        case op_repz:
        case op_repe:
        case op_repnz:
        case op_repne:
        case op_lock:
            return ;
    }
    oa_putlen(len);
    if (aps != 0)
    {
        int separator;
        bePrintf( "\t");
        if ((op == op_jmp || op == op_call) && aps && apd)
        {
            separator = ':';
        }
        else
        {
            separator = ',';
        }
        if (op == op_dd)
            nosize = TRUE;
        oa_putamode(aps);
        nosize = FALSE;
        if (apd != 0)
        {
            beputc(separator);
            oa_putamode(apd);
        }
        if (ap3 != 0)
        {
            beputc(separator);
            oa_putamode(ap3);
        }
    }
    bePrintf( "\n");
}

/*-------------------------------------------------------------------------*/

void oa_gen_strlab(SYMBOL *sp)
/*
 *      generate a named label.
 */
{
    char buf[4096];
    if (sp->storage_class == sc_localstatic)
    {
        oa_put_label(sp->label);
    }
    else
    {
        beDecorateSymName(buf, sp);
        if (cparams.prm_asmfile)
        {
            if (oa_currentSeg == dataseg || oa_currentSeg == bssxseg)
            {
                newlabel = TRUE;
                bePrintf( "\n%s", buf);
                oa_outcol = strlen(buf) + 1;
            }
            else
                bePrintf( "%s:\n", buf);
        }
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_label(int lab)
/*
 *      outputFile a compiler generated label.
 */
{
    if (cparams.prm_asmfile) {
        oa_nl();
        if (oa_currentSeg == dataseg || oa_currentSeg == bssxseg)
        {
            newlabel = TRUE;
            bePrintf( "\nL_%ld", lab);
            oa_outcol = 8;
        }
        else
            bePrintf( "L_%ld:\n", lab);
    }
    else
        outcode_put_label(lab);
}
void oa_put_string_label(int lab, int type)
{
    oa_put_label(lab);
}

/*-------------------------------------------------------------------------*/

void oa_genfloat(enum e_gt type, FPF *val)
/*
 * Output a float value
 */
{
    if (cparams.prm_asmfile)
    {
        char buf[256];
        FPFToString(buf,val);
        switch(type) {
            case floatgen:
                if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
                    || !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
                {
                    UBYTE dta[4];
                    int i;
                    FPFToFloat(dta, val);
                    bePrintf("\tdb\t");
                    for (i=0; i < 4; i++)
                    {
                        bePrintf( "0%02XH", dta[i]);
                        if (i != 3)
                            bePrintf(", ");
                    }
                }
                else
                    bePrintf( "\tdd\t%s\n", buf);
                break;
            case doublegen:
                if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
                    || !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
                {
                    UBYTE dta[8];
                    int i;
                    FPFToDouble(dta, val);
                    bePrintf("\tdb\t");
                    for (i=0; i < 8; i++)
                    {
                        bePrintf( "0%02XH", dta[i]);
                        if (i != 7)
                            bePrintf(", ");
                    }
                }
                else
                    bePrintf( "\tdq\t%s\n", buf);
                break;
            case longdoublegen:
                if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
                    || !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
                {
                    UBYTE dta[10];
                    int i;
                    FPFToLongDouble(dta, val);
                    bePrintf("\tdb\t");
                    for (i=0; i < 10; i++)
                    {
                        bePrintf( "0%02XH", dta[i]);
                        if (i != 9)
                            bePrintf(", ");
                    }
                }
                else
                    bePrintf( "\tdt\t%s\n", buf);
                break;
            default:
                diag("floatgen - invalid type");
                break ;
        }
    }
    else
        switch(type) {
            case floatgen:
                outcode_genfloat(val);
                break;
            case doublegen:
                outcode_gendouble(val);
                break;
            case longdoublegen:
                outcode_genlongdouble(val);
                break;
            default:
                diag("floatgen - invalid type");
                break ;
        }
}
/*-------------------------------------------------------------------------*/

void oa_genstring(LCHAR *str, int len)
/*
 * Generate a string literal
 */
{
    BOOLEAN instring = FALSE;
    if (cparams.prm_asmfile)
    {
        int nlen = len;
        while (nlen--)
            if (*str >= ' ' && *str < 0x7f && *str != '\'' && *str != '\"')
            {	
                if (!instring)
                {
                    oa_gentype = nogen;
                    oa_nl();
                    bePrintf("\tdb\t\"");
                    instring = TRUE;
                }
                bePrintf("%c", *str++);
            }
            else {
                if (instring)
                {
                    bePrintf("\"\n");
                    instring = FALSE;
                }
                oa_genint(chargen, *str++);
            }
        if (instring)
            bePrintf("\"\n");
    }
    else
        outcode_genstring(str, len);
}

/*-------------------------------------------------------------------------*/

void oa_genint(enum e_gt type, LLONG_TYPE val)
{
    if (cparams.prm_asmfile) {
        switch (type) {
            case chargen:
                bePrintf( "\tdb\t0%xh\n", val &0x00ff);
                break ;
            case shortgen:
            case u16gen:
                bePrintf( "\tdw\t0%xh\n", val &0x0ffff);
                break ;
            case longgen:
            case enumgen:
            case intgen:
            case u32gen:
                bePrintf( "\tdd\t0%lxh\n", val);
                break ;
            case longlonggen:
                #ifndef USE_LONGLONG
                    bePrintf( "\tdd\t0%lxh,0%lxh\n", val, val < 0 ?  - 1: 0);
                #else 
                    bePrintf( "\tdd\t0%lxh,0%lxh\n", val, val >> 32);
                #endif 
                break ;
            case wchar_tgen:
                   bePrintf( "\tdw\t0%lxh\n", val);
                break ;
            default:
                diag("genint - unknown type");
                break ;
        }
    } else {
        switch (type) {
/*            case chargen:*/
            case chargen:
                outcode_genbyte(val);
                break ;
            case shortgen:
            case u16gen:
                outcode_genword(val);
                break ;
            case longgen:
            case enumgen:
            case intgen:
            case u32gen:
                outcode_genlong(val);
                break ;
            case longlonggen:
                outcode_genlonglong(val);
                break ;
            case wchar_tgen:
                outcode_genword(val);
                break ;			
            default:
                diag("genint - unknown type");
        }
    }
}
void oa_genaddress(ULLONG_TYPE val)
{
    oa_genint(longgen,val);
}
/*-------------------------------------------------------------------------*/

void oa_gensrref(SYMBOL *sp, int val, int type)
{
    char buf[4096];
    if (cparams.prm_asmfile)
    {
        beDecorateSymName(buf, sp);
        oa_nl();
        bePrintf( "\tdb\t0,%d\n", val);
        bePrintf( "\tdd\t%s\n", buf);
        oa_gentype = srrefgen;
    }
    else
        outcode_gensrref(sp, val);
}

/*-------------------------------------------------------------------------*/

void oa_genref(SYMBOL *sp, int offset)
/*
 * Output a reference to the data area (also gens fixups )
 */
{
    char sign;
    char buf[4096], buf1[4096];
    if (cparams.prm_asmfile)
    {
        if (offset < 0)
        {
            sign = '-';
            offset =  - offset;
        }
        else
            sign = '+';
        beDecorateSymName(buf, sp);
        sprintf(buf1, "%s%c%d", buf, sign, offset);
        if (cparams.prm_asmfile)
        {
            if (!newlabel)
                oa_nl();
            else
                newlabel = FALSE;
            bePrintf( "\tdd\t%s\n", buf1);
            oa_gentype = longgen;
        }
    }
    else
        outcode_genref(sp, offset);

}

/*-------------------------------------------------------------------------*/

void oa_genpcref(SYMBOL *sp, int offset)
/*
 * Output a reference to the code area (also gens fixups )
 */
{
    oa_genref(sp, offset);
}

/*-------------------------------------------------------------------------*/

void oa_genstorage(int nbytes)
/*
 * Output bytes of storage
 */
{
    if (cparams.prm_asmfile)
    {
        if (!newlabel)
            oa_nl();
        else
            newlabel = FALSE;
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf( "\tresb\t0%xh\n", nbytes);
        else
            bePrintf( "\tdb\t0%xh DUP (?)\n", nbytes);
        oa_gentype = nogen;
    }
    else
        outcode_genstorage(nbytes);
}

/*-------------------------------------------------------------------------*/

void oa_gen_labref(int n)
/*
 * Generate a reference to a label
 */
{
    if (cparams.prm_asmfile)
    {

            if (!newlabel)
                oa_nl();
            else
                newlabel = FALSE;
            bePrintf( "\tdd\tL_%d\n", n);
            oa_gentype = longgen;
    }
    else
        outcode_gen_labref(n);
}

/*-------------------------------------------------------------------------*/

void oa_gen_labdifref(int n1, int n2)
{
    if (cparams.prm_asmfile)
    {
            if (!newlabel)
                oa_nl();
            else
                newlabel = FALSE;
            bePrintf( "\tdd\tL_%d-L_%d\n", n1, n2);
            oa_gentype = longgen;
    }
    else
        outcode_gen_labdifref(n1, n2);
}

/*
 * Exit if from a special segment
 */
void oa_exitseg(enum e_sg seg)
{
    if (cparams.prm_asmfile)
    {
        if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
        {
            if (seg == startupxseg)
            {
                bePrintf( "cstartup\tENDS\n");
            }
            else if (seg == rundownxseg)
            {
                bePrintf( "crundown\tENDS\n");
            }
            else if (seg == constseg)
            {
                bePrintf( "_CONST\tENDS\n");
            }
            else if (seg == stringseg)
            {
                bePrintf( "_STRING\tENDS\n");
            }
            else if (seg == tlsseg)
            {
                bePrintf( "_TLS\tENDS\n");
            }
            else if (seg == tlssuseg)
            {
                bePrintf( "tlsstartup\tENDS\n");
            }
            else if (seg == tlsrdseg)
            {
                bePrintf( "tlsrundown\tENDS\n");
            }
        }
        oa_nl();
    }
}

/*
 * Switch to cseg 
 */
void oa_enterseg(enum e_sg seg)
{
    oa_currentSeg = seg ;
    if (cparams.prm_asmfile)
    {
        if (seg == codeseg)
        {
            oa_nl();
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf( "section code\n");
                else
            {
                bePrintf( "section .text\n");
                bePrintf( "[bits 32]\n");
            }
            else
                bePrintf( "\t.code\n");
        } else if (seg == constseg) {
           if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf( "section const\n");
                else
                {
                    bePrintf( "section .text\n");
                    bePrintf( "[bits 32]\n");
                }
            }
            else
                bePrintf( 
                    "_CONST\tsegment use32 public dword \042CONST\042\n");
         } else if (seg == stringseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf( "section string\n");
                else
                {
                    bePrintf( "section .data\n");
                    bePrintf( "[bits 32]\n");
                }
            }
            else
                bePrintf( 
                    "_STRING\tsegment use32 public dword \042STRING\042\n");
         } else if (seg == dataseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf( "section data\n");
                else
                {
                    bePrintf( "section .data\n");
                    bePrintf( "[bits 32]\n");
                }
            else
                bePrintf( "\t.DATA\n");
         } else if (seg == tlsseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf( "section tls\n");
                else
                {
                    bePrintf( "section .data\n");
                    bePrintf( "[bits 32]\n");
                }
            else
                bePrintf( 
                    "_TLS\tsegment use32 public dword \042TLS\042\n");
         } else if (seg == tlssuseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf( "section tlsstartup\n");
                else
                {
                    bePrintf( "section .data\n");
                    bePrintf( "[bits 32]\n");
                }
            else
                bePrintf( 
                    "_TLS\tsegment use32 public dword \042TLS\042\n");
         } else if (seg == tlsrdseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf( "section tlsrundown\n");
                else
                {
                    bePrintf( "section .data\n");
                    bePrintf( "[bits 32]\n");
                }
            else
                bePrintf( 
                    "_TLS\tsegment use32 public dword \042TLS\042\n");
         } else if (seg == bssxseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
                if (!prm_nodos)
                    bePrintf( "section bss\n");
                else
                    bePrintf( "section .bss\n");
            else
                bePrintf( "\t.data?\n");
         } else if (seg == startupxseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf( "section cstartup\n");
                else
                {
                    bePrintf( "section .text\n");
                    bePrintf( "[bits 32]\n");
                }
            }
            else
                bePrintf( 
                    "cstartup\tsegment use32 public dword \042INITDATA\042\n");
         } else if (seg == rundownxseg) {
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                if (!prm_nodos)
                    bePrintf( "section crundown\n");
                else
                {
                    bePrintf( "section .text\n");
                    bePrintf( "[bits 32]\n");
                }
            }
            else
                bePrintf( 
                    "crundown\tsegment use32 public dword \042EXITDATA\042\n");
         }
    }
}

/*-------------------------------------------------------------------------*/

    void oa_gen_virtual(SYMBOL *sp, int data)
    {
        virtual_mode = data;
        oa_currentSeg = virtseg;
        if (cparams.prm_asmfile)
        {
            oa_nl();
            if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            {
                oa_currentSeg = noseg;
#ifdef IEEE
                if (virtual_mode)
                    bePrintf( "\tsection vsd%s virtual\n", sp->decoratedName);
                else
                    bePrintf( "\tsection vsc%s virtual\n", sp->decoratedName);
#else
                bePrintf( "\tSECTION @%s VIRTUAL\n", sp->decoratedName);
#endif
            }
            else
                bePrintf( "@%s\tsegment virtual\n", sp->decoratedName);
            bePrintf( "%s:\n", sp->decoratedName);
        }
        else
            outcode_start_virtual_seg(sp, data);
    }
    void oa_gen_endvirtual(SYMBOL *sp)
    {
        if (cparams.prm_asmfile)
        {
            oa_nl();
            if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
            {
                bePrintf( "@%s\tends\n", sp->decoratedName);
            }
            else
                if (virtual_mode)
                    oa_enterseg(dataseg);
                else
                    oa_enterseg(codeseg);
        }
        else
        {
            if (virtual_mode)
                oa_enterseg(dataseg);
            else
                oa_enterseg(codeseg);
            outcode_end_virtual_seg(sp);
        }
    }
/*
 * Align
 * not really honorign the size... all alignments are mod 4...
 */
void oa_align(int size)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
        /* NASM 0.91 wouldn't let me use parenthesis but this should work
         * according to the documented precedence levels
         */
            bePrintf( "\ttimes $$-$ & %d nop\n", size-1);
        else
            bePrintf( "\talign\t%d\n", size);
    }
    else
        outcode_align(size);
}

/*
 * queue muldiv val
 */
long queue_muldivval(long number)
{
    MULDIV *p = muldivlink, **q = &muldivlink;
    if (cparams.prm_mergestrings)
    {
        while (p)
        {
            if (p->size == 0 && p->value == number)
                return p->label;
            p = p->next;
        }
    }
    p = beGlobalAlloc(sizeof(MULDIV));
    p->next = 0;
    p->value = number;
    p->size = 0;
    while (*q)
        q = &(*q)->next;
    *q = p;
    return p->label = beGetLabel;
}

/*-------------------------------------------------------------------------*/

long queue_floatval(FPF *number, int size)
{
    MULDIV *p = muldivlink, **q = &muldivlink;
    if (cparams.prm_mergestrings)
    {
        while (p)
        {
            if (p->size == size && !memcmp(&p->floatvalue, number, sizeof(*number)))
                return p->label;
            p = p->next;
        }
    }
    p = beGlobalAlloc(sizeof(MULDIV));
    p->next = 0;
    p->floatvalue = *number;
    p->size = size;
    while (*q)
        q = &(*q)->next;
    *q = p;
    return p->label = beGetLabel;
}

/*-------------------------------------------------------------------------*/

void dump_muldivval(void)
{
    int tag = FALSE;
    xconstseg();
    if (cparams.prm_asmfile)
    {
        bePrintf( "\n");
        if (muldivlink)
        {
            tag = TRUE;
        }
        while (muldivlink)
        {
            oa_align(8);
            oa_put_label(muldivlink->label);
            if (muldivlink->size == ISZ_NONE)
                bePrintf( "\tdd\t0%xh\n", muldivlink->value);
            else {
                char buf[256];
                if (muldivlink->floatvalue.type == IFPF_IS_INFINITY
                    || muldivlink->floatvalue.type == IFPF_IS_NAN)
                {
                    UBYTE data[12];
                    int len = 0;
                    int i;
                    switch(muldivlink->size)
                    {
                        case ISZ_FLOAT:
                        case ISZ_IFLOAT:
                            FPFToFloat(data, &muldivlink->floatvalue);
                            len = 4;
                            break;
                        case ISZ_DOUBLE:
                        case ISZ_IDOUBLE:
                            FPFToDouble(data, &muldivlink->floatvalue);
                            len = 8;
                            break;
                        case ISZ_LDOUBLE:
                        case ISZ_ILDOUBLE:
                            FPFToLongDouble(data, &muldivlink->floatvalue);
                            len = 10;
                            break;
                    }
                    bePrintf("\tdb\t");
                    for (i=0; i < len; i++)
                    {
                        bePrintf("0%02XH", data[i]);
                        if (i != len-1)
                            bePrintf(",");
                    }
                    bePrintf("\n");
                    
                }
                else
                {	
                    FPFToString(buf, &muldivlink->floatvalue);
                    if (muldivlink->size == ISZ_FLOAT || muldivlink->size == ISZ_IFLOAT)
                        
                    {
                        bePrintf( "\tdd\t%s\n", buf);
                    }
                    else if (muldivlink->size == ISZ_DOUBLE || muldivlink->size == ISZ_IDOUBLE)
                        bePrintf( "\tdq\t%s\n", buf);
                    else
                        bePrintf( "\tdt\t%s\n", buf);
                }
            }
            muldivlink = muldivlink->next;
        }
        if (tag)
            bePrintf( "\n");
    }
    else
        outcode_dump_muldivval();
}

/*-------------------------------------------------------------------------*/

void dump_browsedata(BROWSEINFO *bri)
{
    if (!cparams.prm_asmfile)
        omf_dump_browsedata(bri);
}
void dump_browsefile(BROWSEFILE *brf)
{
    if (!cparams.prm_asmfile)
        omf_dump_browsefile(brf);
}

/*-------------------------------------------------------------------------*/

void oa_header(char *filename, char *compiler_version)
{
    oa_nl();
    bePrintf(";File %s\n",filename);
    bePrintf(";Compiler version %s\n",compiler_version);
    if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
    {
        if (!prm_nodos)
        {
            bePrintf( "\tsection code align=16 class=CODE use32\n");
            bePrintf( "\tsection data align=8 class=DATA use32\n");
            bePrintf( "\tsection bss  align=8 class=BSS use32\n");
            bePrintf( "\tsection const  align=8 class=CONST use32\n");
            bePrintf( "\tsection string  align=2 class=STRING use32\n")
                ;
            bePrintf( "\tsection tls  align=8 class=TLS use32\n")
                ;
            bePrintf( 
                "\tsection cstartup align=2 class=INITDATA use32\n");
            bePrintf( 
                "\tsection crundown align=2 class=EXITDATA use32\n");
//            bePrintf( 
 //               "\tSECTION cppinit  align=4 CLASS=CPPINIT USE32\n");
  //          bePrintf( 
   //             "\tSECTION cppexit  align=4 CLASS=CPPEXIT USE32\n");
    //        bePrintf( "\tGROUP DGROUP _DATA _BSS _CONST _STRING\n\n");
        }
        else
        {
            bePrintf( "\tsection .text\n");
            bePrintf( "\tsection .data\n");
            bePrintf( "\tsection .bss\n");
        }
    }
    else
    {
        bePrintf( "\ttitle\t'%s'\n", filename);
        if (prm_flat)
            bePrintf( "\t.486p\n\t.model flat\n\n");
        else
            if (prm_assembler == pa_masm)
                bePrintf( "\t.486p\n\t.model small\n\n");
            else
                bePrintf( "\t.486p\n\t.model use32 small\n\n");
    }
}
void oa_trailer(void)
{
    if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
        bePrintf("\tend\n");
}
/*-------------------------------------------------------------------------*/
void oa_localdef(SYMBOL *sp)
{
    if (!cparams.prm_asmfile)
    {
        omf_globaldef(sp);
    }
}
void oa_localstaticdef(SYMBOL *sp)
{
    if (!cparams.prm_asmfile)
    {
        omf_globaldef(sp);
    }
}
void oa_globaldef(SYMBOL *sp)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf( "[global\t%s]\n", sp->decoratedName);
        else
            bePrintf( "\tpublic\t%s\n", sp->decoratedName);
    } else
        omf_globaldef(sp);
}

/*-------------------------------------------------------------------------*/

void oa_output_alias(char *name, char *alias)
{
    if (cparams.prm_asmfile)
    {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf( "%%define %s %s\n", name, alias);
        else
            bePrintf( "%s equ\t<%s>\n", name, alias);
    }
}


/*-------------------------------------------------------------------------*/

void oa_put_extern(SYMBOL *sp, int code)
{
    if (cparams.prm_asmfile) {
        oa_nl();
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
        {
            bePrintf( "[extern\t%s]\n", sp->decoratedName);
        }
        else
        {
            if (code)
                bePrintf( "\textrn\t%s:proc\n", sp->decoratedName);
            else
                bePrintf( "\textrn\t%s:byte\n", sp->decoratedName); 
        }
    } else
        omf_put_extern(sp, code);
}
/*-------------------------------------------------------------------------*/

void oa_put_impfunc(SYMBOL *sp, char *file)
{
    if (cparams.prm_asmfile) 
    {
        bePrintf( "\timport %s %s\n", sp->decoratedName, file);
    } 
    else
    {
        omf_put_impfunc(sp, file);
    }
}

/*-------------------------------------------------------------------------*/

void oa_put_expfunc(SYMBOL *sp)
{
    char buf[4096];
    if (cparams.prm_asmfile) {
        beDecorateSymName(buf, sp);
        if (prm_assembler == pa_nasm || prm_assembler == pa_fasm)
            bePrintf( "\texport %s\n", buf);
        else
            bePrintf( "\tpublicdll %s\n", buf);
    } else
        omf_put_expfunc(sp);
}

void oa_output_includelib(char *name)
{
    if (cparams.prm_asmfile)
    {
        if (!(prm_assembler == pa_nasm || prm_assembler == pa_fasm))
            bePrintf( "\tincludelib %s\n", name);
    } else
        omf_put_includelib(name);
}
void oa_end_generation(void)
{
    dump_muldivval();
}