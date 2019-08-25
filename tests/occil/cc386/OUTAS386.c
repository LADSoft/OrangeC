/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "gen386.h"
#include "diag.h"
#include "lists.h"                       
#include <float.h>

extern int prm_fardefault;
extern int prm_cplusplus;
extern HASHREC **templateFuncs;
extern struct _templatelist *templateList;
extern LIST *libincludes;
extern int prm_nodos;
extern char outfile[];
extern int prm_flat, prm_optmult;
extern SYM *currentfunc;
extern int prm_cmangle;
extern TABLE *gsyms;
extern int phiused;
extern long nextlabel;
extern FILE *outputFile;
extern int global_flag;
extern LIST *localfuncs,  *localdata;
extern OPCODE popn_aaa[];
extern OPCODE popn_aad[];
extern OPCODE popn_aam[];
extern OPCODE popn_aas[];
extern OPCODE popn_add[];
extern OPCODE popn_adc[];
extern OPCODE popn_and[];
extern OPCODE popn_arpl[];
extern OPCODE popn_bound[];
extern OPCODE popn_bsf[];
extern OPCODE popn_bsr[];
extern OPCODE popn_bswap[];
extern OPCODE popn_btc[];
extern OPCODE popn_bt[];
extern OPCODE popn_btr[];
extern OPCODE popn_bts[];
extern OPCODE popn_call[];
extern OPCODE popn_cbw[];
extern OPCODE popn_cwde[];
extern OPCODE popn_cwd[];
extern OPCODE popn_cdq[];
extern OPCODE popn_clc[];
extern OPCODE popn_cld[];
extern OPCODE popn_cli[];
extern OPCODE popn_clts[];
extern OPCODE popn_cmc[];
extern OPCODE popn_cmp[];
extern OPCODE popn_cmps[];
extern OPCODE popn_cmpsb[];
extern OPCODE popn_cmpsw[];
extern OPCODE popn_cmpsd[];
extern OPCODE popn_cpuid[];
extern OPCODE popn_daa[];
extern OPCODE popn_das[];
extern OPCODE popn_dec[];
extern OPCODE popn_div[];
extern OPCODE popn_enter[];
extern OPCODE popn_hlt[];
extern OPCODE popn_idiv[];
extern OPCODE popn_imul[];
extern OPCODE popn_in[];
extern OPCODE popn_inc[];
extern OPCODE popn_ins[];
extern OPCODE popn_insb[];
extern OPCODE popn_insw[];
extern OPCODE popn_insd[];
extern OPCODE popn_int[];
extern OPCODE popn_int3[];
extern OPCODE popn_into[];
extern OPCODE popn_invd[];
extern OPCODE popn_iret[];
extern OPCODE popn_iretd[];
extern OPCODE popn_jcxz[];
extern OPCODE popn_ja[];
extern OPCODE popn_jnbe[];
extern OPCODE popn_jae[];
extern OPCODE popn_jnb[];
extern OPCODE popn_jnc[];
extern OPCODE popn_jb[];
extern OPCODE popn_jc[];
extern OPCODE popn_jnae[];
extern OPCODE popn_jbe[];
extern OPCODE popn_jna[];
extern OPCODE popn_je[];
extern OPCODE popn_jz[];
extern OPCODE popn_jg[];
extern OPCODE popn_jnle[];
extern OPCODE popn_jl[];
extern OPCODE popn_jnge[];
extern OPCODE popn_jge[];
extern OPCODE popn_jnl[];
extern OPCODE popn_jle[];
extern OPCODE popn_jng[];
extern OPCODE popn_jne[];
extern OPCODE popn_jnz[];
extern OPCODE popn_jo[];
extern OPCODE popn_jno[];
extern OPCODE popn_jp[];
extern OPCODE popn_jnp[];
extern OPCODE popn_jpe[];
extern OPCODE popn_jpo[];
extern OPCODE popn_js[];
extern OPCODE popn_jns[];
extern OPCODE popn_jmp[];
extern OPCODE popn_lahf[];
extern OPCODE popn_lar[];
extern OPCODE popn_lds[];
extern OPCODE popn_les[];
extern OPCODE popn_lfs[];
extern OPCODE popn_lgs[];
extern OPCODE popn_lss[];
extern OPCODE popn_lea[];
extern OPCODE popn_leave[];
extern OPCODE popn_lgdt[];
extern OPCODE popn_lidt[];
extern OPCODE popn_lldt[];
extern OPCODE popn_lmsw[];
extern OPCODE popn_lock[];
extern OPCODE popn_lods[];
extern OPCODE popn_lodsb[];
extern OPCODE popn_lodsw[];
extern OPCODE popn_lodsd[];
extern OPCODE popn_loop[];
extern OPCODE popn_loope[];
extern OPCODE popn_loopz[];
extern OPCODE popn_loopne[];
extern OPCODE popn_loopnz[];
extern OPCODE popn_lsl[];
extern OPCODE popn_ltr[];
extern OPCODE popn_mov[];
extern OPCODE popn_movs[];
extern OPCODE popn_movsb[];
extern OPCODE popn_movsw[];
extern OPCODE popn_movsd[];
extern OPCODE popn_movsx[];
extern OPCODE popn_movzx[];
extern OPCODE popn_mul[];
extern OPCODE popn_neg[];
extern OPCODE popn_not[];
extern OPCODE popn_nop[];
extern OPCODE popn_or[];
extern OPCODE popn_out[];
extern OPCODE popn_outs[];
extern OPCODE popn_outsb[];
extern OPCODE popn_outsw[];
extern OPCODE popn_outsd[];
extern OPCODE popn_pop[];
extern OPCODE popn_popa[];
extern OPCODE popn_popad[];
extern OPCODE popn_popf[];
extern OPCODE popn_popfd[];
extern OPCODE popn_push[];
extern OPCODE popn_pusha[];
extern OPCODE popn_pushad[];
extern OPCODE popn_pushf[];
extern OPCODE popn_pushfd[];
extern OPCODE popn_rcl[];
extern OPCODE popn_rcr[];
extern OPCODE popn_rdmsr[];
extern OPCODE popn_rdpmc[];
extern OPCODE popn_rdtsc[];
extern OPCODE popn_rol[];
extern OPCODE popn_ror[];
extern OPCODE popn_repnz[];
extern OPCODE popn_repz[];
extern OPCODE popn_ret[];
extern OPCODE popn_retf[];
extern OPCODE popn_sahf[];
extern OPCODE popn_sal[];
extern OPCODE popn_sar[];
extern OPCODE popn_shl[];
extern OPCODE popn_shr[];
extern OPCODE popn_sbb[];
extern OPCODE popn_scas[];
extern OPCODE popn_scasb[];
extern OPCODE popn_scasw[];
extern OPCODE popn_scasd[];
extern OPCODE popn_seta[];
extern OPCODE popn_setnbe[];
extern OPCODE popn_setae[];
extern OPCODE popn_setnb[];
extern OPCODE popn_setnc[];
extern OPCODE popn_setb[];
extern OPCODE popn_setc[];
extern OPCODE popn_setnae[];
extern OPCODE popn_setbe[];
extern OPCODE popn_setna[];
extern OPCODE popn_sete[];
extern OPCODE popn_setz[];
extern OPCODE popn_setg[];
extern OPCODE popn_setnle[];
extern OPCODE popn_setl[];
extern OPCODE popn_setnge[];
extern OPCODE popn_setge[];
extern OPCODE popn_setnl[];
extern OPCODE popn_setle[];
extern OPCODE popn_setng[];
extern OPCODE popn_setne[];
extern OPCODE popn_setnz[];
extern OPCODE popn_seto[];
extern OPCODE popn_setno[];
extern OPCODE popn_setp[];
extern OPCODE popn_setnp[];
extern OPCODE popn_setpe[];
extern OPCODE popn_setpo[];
extern OPCODE popn_sets[];
extern OPCODE popn_setns[];
extern OPCODE popn_sgdt[];
extern OPCODE popn_sidt[];
extern OPCODE popn_sldt[];
extern OPCODE popn_smsw[];
extern OPCODE popn_shld[];
extern OPCODE popn_shrd[];
extern OPCODE popn_stc[];
extern OPCODE popn_std[];
extern OPCODE popn_sti[];
extern OPCODE popn_stos[];
extern OPCODE popn_stosb[];
extern OPCODE popn_stosw[];
extern OPCODE popn_stosd[];
extern OPCODE popn_str[];
extern OPCODE popn_sub[];
extern OPCODE popn_test[];
extern OPCODE popn_verr[];
extern OPCODE popn_verw[];
extern OPCODE popn_wait[];
extern OPCODE popn_wbinvd[];
extern OPCODE popn_wrmsr[];
extern OPCODE popn_xchg[];
extern OPCODE popn_xlat[];
extern OPCODE popn_xlatb[];
extern OPCODE popn_xor[];
extern OPCODE popn_f2xm1[];
extern OPCODE popn_fabs[];
extern OPCODE popn_fadd[];
extern OPCODE popn_faddp[];
extern OPCODE popn_fiadd[];
extern OPCODE popn_fchs[];
extern OPCODE popn_fclex[];
extern OPCODE popn_fnclex[];
extern OPCODE popn_fcom[];
extern OPCODE popn_fcomp[];
extern OPCODE popn_fcompp[];
extern OPCODE popn_fcos[];
extern OPCODE popn_fdecstp[];
extern OPCODE popn_fdiv[];
extern OPCODE popn_fdivp[];
extern OPCODE popn_fidiv[];
extern OPCODE popn_fdivr[];
extern OPCODE popn_fdivrp[];
extern OPCODE popn_fidivr[];
extern OPCODE popn_ffree[];
extern OPCODE popn_ficom[];
extern OPCODE popn_ficomp[];
extern OPCODE popn_fild[];
extern OPCODE popn_fincstp[];
extern OPCODE popn_finit[];
extern OPCODE popn_fninit[];
extern OPCODE popn_fist[];
extern OPCODE popn_fistp[];
extern OPCODE popn_fld[];
extern OPCODE popn_fldz[];
extern OPCODE popn_fldpi[];
extern OPCODE popn_fld1[];
extern OPCODE popn_fldl2t[];
extern OPCODE popn_fldl2e[];
extern OPCODE popn_fldlg2[];
extern OPCODE popn_fldln2[];
extern OPCODE popn_fldcw[];
extern OPCODE popn_fldenv[];
extern OPCODE popn_fmul[];
extern OPCODE popn_fmulp[];
extern OPCODE popn_fimul[];
extern OPCODE popn_fpatan[];
extern OPCODE popn_fprem[];
extern OPCODE popn_fprem1[];
extern OPCODE popn_fptan[];
extern OPCODE popn_frndint[];
extern OPCODE popn_frstor[];
extern OPCODE popn_fsave[];
extern OPCODE popn_fnsave[];
extern OPCODE popn_fscale[];
extern OPCODE popn_fsin[];
extern OPCODE popn_fsincos[];
extern OPCODE popn_fsqrt[];
extern OPCODE popn_fst[];
extern OPCODE popn_fstp[];
extern OPCODE popn_fstcw[];
extern OPCODE popn_fstsw[];
extern OPCODE popn_fnstcw[];
extern OPCODE popn_fnstsw[];
extern OPCODE popn_fstenv[];
extern OPCODE popn_fnstenv[];
extern OPCODE popn_fsub[];
extern OPCODE popn_fsubp[];
extern OPCODE popn_fisub[];
extern OPCODE popn_fsubr[];
extern OPCODE popn_fsubrp[];
extern OPCODE popn_fisubr[];
extern OPCODE popn_ftst[];
extern OPCODE popn_fucom[];
extern OPCODE popn_fucomp[];
extern OPCODE popn_fucompp[];
extern OPCODE popn_fwait[];
extern OPCODE popn_fxam[];
extern OPCODE popn_fxch[];
extern OPCODE popn_fxtract[];
extern OPCODE popn_fyl2x[];
extern OPCODE popn_fyl2xp1[];
extern LIST *mpthunklist;


int skipsize = 0;
int addsize = 0;

/*      variable initialization         */

char segregs[] = "CSDSESFSGSSS";

extern int prm_asmfile;
extern int prm_lines, prm_nasm, prm_masm, prm_fasm;
extern char *segnames[], *segclasses[];

struct slit *strtab;

static int hasheader;
static int uses_float;

MULDIV *muldivlink = 0;
enum e_gt gentype = nogen; /* Current DC type */
enum e_sg curseg = noseg; /* Current seg */
int outcol = 0; /* Curront col (roughly) */
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
        "blockstart#", op_reserved, 0, 0
    }
    , 
    {
        "blockend#", op_reserved, 0, 0
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
        "cpuid", op_cpuid, 0, &popn_cpuid
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
void outcodeini(void)
{
    strtab = 0;
    gentype = nogen;
    curseg = noseg;
    outcol = 0;
    newlabel = FALSE;
    muldivlink = 0;
    mpthunklist = 0;
	hasheader = FALSE;
}

//-------------------------------------------------------------------------

void nl(void)
/*
 * New line
 */
{
    if (prm_asmfile)
    {
        if (outcol > 0)
        {
            oputc('\n', outputFile);
            outcol = 0;
            gentype = nogen;
        }
    }
}

/* Put an opcode
 */
void outop(char *name)
{
    oputc('\t', outputFile);
    while (*name)
        oputc(toupper(*name++), outputFile);
}

//-------------------------------------------------------------------------

void putop(enum e_op op, AMODE *aps, AMODE *apd, int nooptx)
{
    if (prm_nasm)
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
                    aps->length = BESZ_DWORD;
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
                        aps->length = BESZ_DWORD;
                }
                break;
        }
        if (op == op_fwait)
        {
            outop(oplst[op_fwait].word + 1);
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
            }
        }

    }
    if (op > op_fyl2xp1)
        DIAG("illegal opcode.");
    else
        outop(oplst[op].word);
    uses_float = (op >= op_f2xm1);
}

//-------------------------------------------------------------------------

char *dumpns(NAMESPACE *ns, char *buf, int *done)
{
    if (!ns)
        return buf;
    buf = dumpns(ns->next, buf, done);
    *done = 1;
    *buf++ = '@';
    strcpy(buf, ns->sp->name + prm_cmangle);
    buf += strlen(buf);
    return buf;
}

//-------------------------------------------------------------------------

void putsym(char *buf, SYM *sp, char *p)
{
    int done = 0;
    if (!p)
    {
        buf[0] = 0;
        return ;
    }
    if (sp->staticlabel)
        sprintf(buf, "L_%d", sp->value.i);
    else if (sp->alias)
        strcpy(buf, sp->alias);
    else
    {
        if (sp->pascaldefn)
        {
            char *q = buf;
            if (prm_cmangle)
                p++;
            while (*p)
                *q++ = toupper(*p++);
            *q = 0;
        }
        else if (sp->isstdcall)
        {
            if (prm_cmangle)
                p++;
            strcpy(buf, p);
        }
        else
        {
            char *q;
            if (sp->mangled && !(sp->tp->type == bt_func || sp->tp->type ==
                bt_ifunc))
            {
                q = dumpns(sp->value.classdata.parentns, buf, &done);
                if (done)
                {
                    *q++ = '@';
                    strcpy(q, p + prm_cmangle);
                }
                else
                    strcpy(q, p);
            }
            else
                strcpy(buf, p);
            if (prm_cplusplus && prm_asmfile && !prm_nasm)
            {
                // problematic for compiling templates via ASM and TASM
                q = buf;
                while (q = strchr(q, '#'))
                    *q = '%';
            }
        }
    }
}

//-------------------------------------------------------------------------

void putconst(ENODE *offset, int doSign)
/*
 *      put a constant to the outputFile file.
 */
{
    char buf[512],  buf1[512];
    SYM *sp;
    switch (offset->nodetype)
    {
        case en_autoreg:
        case en_autocon:
        case en_icon:
        case en_lcon:
        case en_iucon:
        case en_lucon:
        case en_llucon:
        case en_llcon:
        case en_ccon:
        case en_cucon:
        case en_absacon:
        case en_boolcon:
			if (doSign)
				if ((int)offset->v.i < 0)
		            oprintf(outputFile, "-0%lXH", -offset->v.i);
				else
        		    oprintf(outputFile, "+0%lXH", offset->v.i);
			else
       		    oprintf(outputFile, "0%lXH", offset->v.i);
            break;
        case en_fcomplexcon:
        case en_rcomplexcon:
        case en_lrcomplexcon:
			if (doSign)
				oputc('+', outputFile);
            FPFToString(buf,&offset->v.c.r);
			FPFToString(buf1, &offset->v.c.i);
            oprintf(outputFile, "%s,%s", buf, buf1);
            break;
        case en_rcon:
        case en_fcon:
        case en_lrcon:
        case en_fimaginarycon:
        case en_rimaginarycon:
        case en_lrimaginarycon:
			if (doSign)
				oputc('+', outputFile);
            FPFToString(buf,&offset->v.f);
            oprintf(outputFile, "%s", buf);
            break;
        case en_nalabcon:
			if (doSign)
				oputc('+', outputFile);
            oprintf(outputFile, "L_%d", offset->v.sp->value.i);
            break;
        case en_labcon:
			if (doSign)
				oputc('+', outputFile);
            oprintf(outputFile, "L_%d", offset->v.i);
            break;
        case en_napccon:
        case en_nacon:
			if (doSign)
				oputc('+', outputFile);
            sp = offset->v.sp;
            putsym(buf, sp, sp->name);
            oprintf(outputFile, "%s", buf);
            break;
        case en_add:
        case en_addstruc:
            putconst(offset->v.p[0], doSign);
            putconst(offset->v.p[1], TRUE);
            break;
        case en_sub:
			if (doSign)
				oputc('+', outputFile);
            putconst(offset->v.p[0], FALSE);
            oprintf(outputFile, "-");
            putconst(offset->v.p[1], FALSE);
            break;
        case en_uminus:
			oputc('-', outputFile);
            putconst(offset->v.p[0], FALSE);
            break;
        default:
            DIAG("illegal constant node.");
            break;
    }
}

//-------------------------------------------------------------------------

void putlen(int l)
/*
 *      append the length field to an instruction.
 */
{
    if (l < 0)
        l =  - l;
	switch(l) {
		case BESZ_BOOL:
		case BESZ_BYTE:
		case BESZ_WORD:
		case BESZ_DWORD:
		case BESZ_QWORD:
		case BESZ_FLOAT:
		case BESZ_IFLOAT:
		case BESZ_CFLOAT:
		case BESZ_DOUBLE:
		case BESZ_IDOUBLE:
		case BESZ_CDOUBLE:
		case BESZ_LDOUBLE:
		case BESZ_ILDOUBLE:
		case BESZ_CLDOUBLE:
		case 0:
			break;
		default:
	        DIAG("illegal length field.");
	}
}

//-------------------------------------------------------------------------

void putsizedreg(char *string, int reg, int size)
{
    static char *byteregs[] = 
    {
        "AL", "CL", "DL", "BL", "AH", "CH", "DH", "BH"
    };
    static char *wordregs[] = 
    {
        "AX", "CX", "DX", "BX", "SP", "BP", "SI", "DI"
    };
    static char *longregs[] = 
    {
        "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"
    };
    if (size < 0)
        size =  - size;
    if (size == 4)
        oprintf(outputFile, string, longregs[reg]);
    else if (size == 1 || size == 5)
    {
        oprintf(outputFile, string, byteregs[reg]);
    }
    else
        oprintf(outputFile, string, wordregs[reg]);
}

//-------------------------------------------------------------------------

void pointersize(int size)
{
    if (prm_nasm && skipsize)
        return ;
    if (size < 0)
        size =  - size;
    /*      if (needpointer)
     */switch (size)
    {
		case BESZ_LDOUBLE:
		case BESZ_ILDOUBLE:
            if (prm_nasm)
                oprintf(outputFile, "TWORD ");
            else
                oprintf(outputFile, "TBYTE ");
            break;
		case BESZ_QWORD:
		case BESZ_DOUBLE:
		case BESZ_IDOUBLE:
            // should never happen                      
            oprintf(outputFile, "QWORD ");
            break;
		case BESZ_FLOAT:
		case BESZ_IFLOAT:
            if (!uses_float)
            {
                if (prm_nasm)
                    oprintf(outputFile, "DWORD FAR ");
                else
                    oprintf(outputFile, "FWORD ");
                break;
            }
        case BESZ_DWORD:
            oprintf(outputFile, "DWORD ");
            break;
        case BESZ_WORD:
            oprintf(outputFile, "WORD ");
            break;
        case BESZ_BOOL:
        case BESZ_BYTE:
            oprintf(outputFile, "BYTE ");
            break;
        case 0:
             /* for NASM with certain FP ops */
            break;
        default:
            DIAG("Bad pointer");
    }
    if (!prm_nasm && size)
        oprintf(outputFile, "PTR ");
}

//-------------------------------------------------------------------------

void putseg(int seg, int usecolon)
{
    if (!seg)
        return ;
    seg -= 1;
    seg <<= 1;
    oputc(segregs[seg], outputFile);
    oputc(segregs[seg + 1], outputFile);
    if (usecolon)
        oputc(':', outputFile);
}

//-------------------------------------------------------------------------

int islabeled(ENODE *n)
{
    int rv = 0;
	if (isintconst(n->nodetype))
		return FALSE;
    switch (n->nodetype)
    {
        case en_add:
        case en_addstruc:
            rv |= islabeled(n->v.p[0]);
            rv |= islabeled(n->v.p[1]);
            break;
        case en_icon:
        case en_ccon:
        case en_cucon:
        case en_lcon:
        case en_lucon:
        case en_iucon:
        case en_boolcon:
            return 0;
        case en_labcon:
        case en_nacon:
        case en_autocon:
        case en_absacon:
        case en_nalabcon:
        case en_napccon:
            return 1;
        default:
            DIAG("Unexpected node type in islabeled");
            break;
    }
    return rv;
}

//-------------------------------------------------------------------------

void putamode(AMODE *ap)
/*
 *      output a general addressing mode.
 */
{
    int oldnasm, l;

    switch (ap->mode)
    {
        case am_seg:
            putseg(ap->seg, 0);
            break;
        case am_screg:
            oprintf(outputFile, "CR%d", ap->preg);
            break;
        case am_sdreg:
            oprintf(outputFile, "DR%d", ap->preg);
            break;
        case am_streg:
            oprintf(outputFile, "TR%d", ap->preg);
            break;
        case am_immed:
            if (ap->length > 0 && islabeled(ap->offset))
            {
                if (!prm_nasm)
                    oprintf(outputFile, "OFFSET ");
                else if (!nosize)
                    oprintf(outputFile, "DWORD ");
            }
            else if (prm_nasm && !prm_fasm && addsize)
                pointersize(ap->length);
            putconst(ap->offset, FALSE);
            break;
        case am_direct:
            pointersize(ap->length);
            if (!prm_nasm)
                putseg(ap->seg, TRUE);
            oputc('[', outputFile);
            oldnasm = prm_nasm;
            if (prm_nasm)
                putseg(ap->seg, TRUE);
            prm_nasm = TRUE;
            putconst(ap->offset, FALSE);
            oputc(']', outputFile);
            prm_nasm = oldnasm;
            break;
        case am_dreg:
            putsizedreg("%s", ap->preg, ap->length);
            break;
        case am_freg:
            if (prm_nasm)
                oprintf(outputFile, "ST%d", ap->preg);
            else
                oprintf(outputFile, "ST(%d)", ap->preg);
            break;
        case am_indisp:
            pointersize(ap->length);
            if (!prm_nasm)
                putseg(ap->seg, TRUE);
            oputc('[', outputFile);
            if (prm_nasm)
                putseg(ap->seg, TRUE);
            putsizedreg("%s", ap->preg, 4);
            if (ap->offset)
            {
	            putconst(ap->offset, TRUE);
            }
            oputc(']', outputFile);
            break;
        case am_indispscale:
            {
                int scale = 1, t = ap->scale;

                while (t--)
                    scale <<= 1;
                pointersize(ap->length);
                if (!prm_nasm)
                    putseg(ap->seg, TRUE);
                oputc('[', outputFile);
                if (prm_nasm)
                    putseg(ap->seg, TRUE);
                if (ap->preg !=  - 1)
                    putsizedreg("%s+", ap->preg, 4);
                putsizedreg("%s", ap->sreg, 4);
                if (scale != 1)
                    oprintf(outputFile, "*%d", scale);
                if (ap->offset)
                {
                    putconst(ap->offset, TRUE);
                }
                oputc(']', outputFile);
            }
            break;
        default:
            DIAG("illegal address mode.");
            break;
    }
}

//-------------------------------------------------------------------------

void put_code(OCODE *cd)
/*
 *      outputFile a generic instruction.
 */
{
    int op = cd->opcode, len = 0, len2 = 0;
    AMODE *aps = cd->oper1,  *apd = cd->oper2,  *ap3 = cd->oper3;

    if (!prm_asmfile)
        return ;
    if (op == op_line)
    {
        if (!prm_lines)
            return ;
        oprintf(outputFile, ";\n; Line %d:\t%s\n;\n", (int)apd, (char*)aps);
        return ;
    }
    else if (op == op_comment)
    {
        if (!prm_lines)
            return ;
        oprintf(outputFile, "%s", aps);
        return ;
    }
    else if (op == op_void || op == op_blockstart || op == op_blockend)
        return ;
    if (aps)
        len = aps->length;
    if (apd)
        len2 = apd->length;
    needpointer = (len != len2) || ((!aps || aps->mode != am_dreg) && (!apd ||
        apd->mode != am_dreg));
    putop(op, aps, apd, cd->noopt);
    if (prm_nasm && ((op >= op_ja && op <= op_jns && op != op_jecxz) || op ==
        op_jmp && aps->mode == am_immed && !apd))
    {
        if (cd->branched &BR_SHORT)
            oprintf(outputFile, "\tSHORT");
        else
            oprintf(outputFile, "\tNEAR");
        nosize = TRUE;
    }
    else if (op == op_jmp && aps->mode == am_immed && aps->offset->nodetype ==
        en_labcon && (cd->branched &BR_SHORT))
    {
        oprintf(outputFile, "\tSHORT");
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
    putlen(len);
    if (aps != 0)
    {
        int separator;
        oprintf(outputFile, "\t");
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
        putamode(aps);
        nosize = FALSE;
        if (apd != 0)
        {
            oputc(separator, outputFile);
            putamode(apd);
        }
        if (ap3 != 0)
        {
            oputc(separator, outputFile);
            putamode(ap3);
        }
    }
    oprintf(outputFile, "\n");
}

//-------------------------------------------------------------------------

void gen_strlab(SYM *sp)
/*
 *      generate a named label.
 */
{
    char buf[512];
    putsym(buf, sp, sp->name);
    if (prm_asmfile)
    if (curseg == dataseg || curseg == bssxseg)
    {
        newlabel = TRUE;
        oprintf(outputFile, "\n%s", buf);
        outcol = strlen(buf) + 1;
    }
    else
        oprintf(outputFile, "%s:\n", buf);
}

//-------------------------------------------------------------------------

void put_label(int lab)
/*
 *      outputFile a compiler generated label.
 */
{
    if (prm_asmfile)
        oprintf(outputFile, "L_%d:\n", lab);
    else
        outcode_put_label(lab);
}

//-------------------------------------------------------------------------

void put_staticlabel(long label)
{
    if (prm_asmfile)
    {
        nl();
        if (curseg == dataseg || curseg == bssxseg)
        {
            newlabel = TRUE;
            oprintf(outputFile, "\nL_%ld", label);
            outcol = 8;
        }
        else
            oprintf(outputFile, "L_%ld:\n", label);
    }
    else
        outcode_put_label(label);
}

//-------------------------------------------------------------------------

void genfloat(FPF *val)
/*
 * Output a float value
 */
{
    if (prm_asmfile)
    {
		char buf[512];
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        FPFToString(buf,val);
		if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
			|| !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
		{
			unsigned char dta[4];
			int i;
			FPFToFloat(dta, val);
			oprintf(outputFile,"\tDB\t");
			for (i=0; i < 4; i++)
			{
				oprintf(outputFile, "0%02XH", dta[i]);
				if (i != 3)
					oprintf(outputFile,", ");
			}
			oprintf(outputFile,"\n");
		}
		else
		{
            oprintf(outputFile, "\tDD\t%s", buf);
            gentype = floatgen;
            outcol = 19;
        }
    }
    else
        outcode_genfloat(val);
}

//-------------------------------------------------------------------------

void gendouble(FPF *val)
/*
 * Output a double value
 */
{
    if (prm_asmfile)
	{
		char buf[512];
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        FPFToString(buf,val);
		if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
			|| !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
		{
			unsigned char dta[8];
			int i;
			FPFToDouble(dta, val);
			oprintf(outputFile,"\tDB\t");
			for (i=0; i < 8; i++)
			{
				oprintf(outputFile, "0%02XH", dta[i]);
				if (i != 7)
					oprintf(outputFile,", ");
			}
			oprintf(outputFile,"\n");
		}
		else
	    {
	        oprintf(outputFile, "\tDQ\t%s", buf);
	        gentype = doublegen;
	        outcol = 19;
	    }
	}
    else
        outcode_gendouble(val);
}

//-------------------------------------------------------------------------

void genlongdouble(FPF *val)
/*
 * Output a double value
 */
{
    if (prm_asmfile)
	{
		char buf[512];
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        FPFToString(buf,val);
		if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
			|| !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
		{
			unsigned char dta[10];
			int i;
			FPFToLongDouble(dta, val);
			oprintf(outputFile,"\tDB\t");
			for (i=0; i < 10; i++)
			{
				oprintf(outputFile, "0%02XH", dta[i]);
				if (i != 9)
					oprintf(outputFile,", ");
			}
			oprintf(outputFile,"\n");
		}
		else
	    {
	        oprintf(outputFile, "\tDT\t%s", buf);
	        gentype = longdoublegen;
	        outcol = 19;
	    }
	}
    else
        outcode_genlongdouble(val);
}

//-------------------------------------------------------------------------

int genstring(char *str, int uselong, int len)
/*
 * Generate a string literal
 */
{
    if (prm_asmfile)
    if (uselong)
    {
        len /= 2;
        while (len--)
        {
            genword(*((short*)str));
            str += 2;
        }
        return pstrlen(str) *2;
    }
    else
    {
        while (len--)
            genbyte(*str++);
        return strlen(str);
    }
    else
        outcode_genstring(str, len);
}

//-------------------------------------------------------------------------

void genbyte(long val)
/*
 * Output a align=1value
 */
{
    if (prm_asmfile)
    if (gentype == bytegen && outcol < 60)
    {
        oprintf(outputFile, ",0%XH", val &0x00ff);
        outcol += 4;
    }
    else
    {
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        oprintf(outputFile, "\tDB\t0%XH", val &0x00ff);
        gentype = bytegen;
        outcol = 19;
    }
    else
        outcode_genbyte(val);
}
void genbool(int val)
{
	genbyte(val);
}
//-------------------------------------------------------------------------

void genword(long val)
/*
 * Output a word value
 */
{
    if (prm_asmfile)
    if (gentype == wordgen && outcol < 58)
    {
        oprintf(outputFile, ",0%XH", val &0x0ffff);
        outcol += 6;
    }
    else
    {
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        oprintf(outputFile, "\tDW\t0%XH", val &0x0ffff);
        gentype = wordgen;
        outcol = 21;
    }
    else
        outcode_genword(val);
}

//-------------------------------------------------------------------------

void genlong(long val)
/*
 * Output a long value
 */
{
    if (prm_asmfile)
    if (gentype == longgen && outcol < 56)
    {
        oprintf(outputFile, ",0%lXH", val);
        outcol += 10;
    }
    else
    {
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        oprintf(outputFile, "\tDD\t0%lXH", val);
        gentype = longgen;
        outcol = 25;
    }
    else
        outcode_genlong(val);
}
void genint(int val)
{
	genlong(val);
}
void genenum(int val)
{
	genlong(val);
}
//-------------------------------------------------------------------------

void genlonglong(LLONG_TYPE val)
/*
 * Output a long value
 */
{
    if (prm_asmfile)
    if (gentype == longgen && outcol < 56)
    {
        #ifdef BCC32
            oprintf(outputFile, ",0%lXH,0%lXH", val, val < 0 ?  - 1: 0);
        #else 
            oprintf(outputFile, ",0%lXH,0%lXH", val, val >> 32);
        #endif 
        outcol += 20;
    }
    else
    {
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        #ifdef BCC32
            oprintf(outputFile, "\tDD\t0%lXH,0%lXH", val, val < 0 ?  - 1: 0);
        #else 
            oprintf(outputFile, "\tDD\t0%lXH,0%lXH", val, val >> 32);
        #endif 
        gentype = longgen;
        outcol = 36;
    }
    else
        outcode_genlonglong(val);
}

//-------------------------------------------------------------------------

void gensrref(SYM *sp, int val)
{
    char buf[512];
	genword(val << 8);
	genref(sp, 0);
}

//-------------------------------------------------------------------------

void genref(SYM *sp, int offset)
/*
 * Output a reference to the data area (also gens fixups )
 */
{
    char sign;
    char name[512], buf[512];
    if (prm_asmfile)
    {
        putsym(name, sp, sp->name);
        if (offset < 0)
        {
            sign = '-';
            offset =  - offset;
        }
        else
            sign = '+';
        sprintf(buf, "%s%c%d", name, sign, offset);
        if (prm_asmfile)
        {
            if (gentype == longgen && outcol < 55-strlen(buf))
            {
                oprintf(outputFile, ",%s", buf);
                outcol += (11+strlen(buf));
            }
            else
            {
                if (!newlabel)
                    nl();
                else
                    newlabel = FALSE;
                oprintf(outputFile, "\tDD\t%s", buf);
                outcol = 26+strlen(buf);
                gentype = longgen;
            }
        }
    }
    else
        outcode_genref(sp, offset);

}

//-------------------------------------------------------------------------

void genpcref(SYM *sp, int offset)
/*
 * Output a reference to the code area (also gens fixups )
 */
{
    genref(sp, offset);
}

//-------------------------------------------------------------------------

void genstorage(int nbytes)
/*
 * Output bytes of storage
 */
{
    if (prm_asmfile)
    {
        if (!newlabel)
            nl();
        else
            newlabel = FALSE;
        if (prm_nasm)
            oprintf(outputFile, "\tRESB\t0%XH", nbytes);
        else
            oprintf(outputFile, "\tDB\t0%XH DUP (?)", nbytes);
        outcol = 28;
        gentype = storagegen;
    }
    else
        outcode_genstorage(nbytes);
}

//-------------------------------------------------------------------------

void gen_labref(int n)
/*
 * Generate a reference to a label
 */
{
    if (prm_asmfile)
    {
        if (gentype == longgen && outcol < 58)
        {
            oprintf(outputFile, ",L_%d", n);
            outcol += 6;
        }
        else
        {
            if (!newlabel)
                nl();
            else
                newlabel = FALSE;
            oprintf(outputFile, "\tDD\tL_%d", n);
            outcol = 22;
            gentype = longgen;
        }
    }
    else
        outcode_gen_labref(n);
}

//-------------------------------------------------------------------------

void gen_labdifref(int n1, int n2)
{
    if (prm_asmfile)
    {
        if (gentype == longgen && outcol < 58)
        {
            oprintf(outputFile, ",L_%d-L_%d", n1, n2);
            outcol += 6;
        }
        else
        {
            if (!newlabel)
                nl();
            else
                newlabel = FALSE;
            oprintf(outputFile, "\tDD\tL_%d-L_%d", n1, n2);
            outcol = 22;
            gentype = longgen;
        }
    }
    else
        outcode_gen_labdifref(n1, n2);
}

//-------------------------------------------------------------------------

int stringlit(char *s, int uselong, int len)
/*
 *      make s a string literal and return it's label number.
 */
{
    struct slit *lp = strtab;
    if (uselong)
        len *= 2;
    if (prm_optmult)
    {
        while (lp)
        {
            if (len == lp->len && !memcmp(lp->str, s, len))
                return lp->label;
            lp = lp->next;
        } 
    }
    ++global_flag; /* always allocate from global space. */
    lp = xalloc(sizeof(struct slit));
    lp->label = nextlabel++;
    lp->str = xalloc(len);
    memcpy(lp->str, s, len);
    lp->len = len;
    lp->next = strtab;
    lp->type = uselong;
    strtab = lp;
    --global_flag;
    return lp->label;
}

//-------------------------------------------------------------------------

void dumplits(void)
/*
 *      dump the string literal pool.
 */
{
    if (prm_asmfile)
    {
        while (strtab != 0)
        {
            xstringseg();
            nl();
            put_label(strtab->label);
            genstring(strtab->str, strtab->type, strtab->len - 1);
            if (strtab->type)
                genword(0);
            else
                genbyte(0);
            strtab = strtab->next;
        } nl();
    }
    else
        outcode_dumplits();
}

/*
 * Exit if from a special segment
 */
void exitseg(void)
{
    if (prm_asmfile)
    {
        if (!prm_nasm)
        {
            if (curseg == startupxseg)
            {
                curseg = noseg;
                oprintf(outputFile, "cstartup\tENDS\n");
            }
            else if (curseg == rundownxseg)
            {
                curseg = noseg;
                oprintf(outputFile, "crundown\tENDS\n");
            }
            else if (curseg == cppxseg)
            {
                curseg = noseg;
                oprintf(outputFile, "cppinit\tENDS\n");
            }
            else if (curseg == constseg)
            {
                curseg = noseg;
                oprintf(outputFile, "_CONST\tENDS\n");
            }
            else if (curseg == stringseg)
            {
                curseg = noseg;
                oprintf(outputFile, "_STRING\tENDS\n");
            }
        }
    }
    else
        curseg = noseg;
}

/*
 * Switch to cseg 
 */
void cseg(void)
{
    if (prm_asmfile)
    {
		asm_header();
        if (curseg != codeseg)
        {
            nl();
            exitseg();
            if (prm_nasm)
                if (!prm_nodos)
                   oprintf(outputFile, "SECTION %s\n", segnames[codeseg]);
                else
            {
                oprintf(outputFile, "SECTION .text\n");
                oprintf(outputFile, "[BITS 32]\n");
            }
            else
                oprintf(outputFile, "\t.CODE\n");
            curseg = codeseg;
        }
    }
    else
        curseg = codeseg;
}

//-------------------------------------------------------------------------

void xconstseg(void)
{
    if (prm_asmfile)
    {
		asm_header();
        if (curseg != constseg)
        {
            nl();
            exitseg();
            if (prm_nasm)
            {
                if (!prm_nodos)
                    oprintf(outputFile, "SECTION %s\n", segnames[constseg]);
                else
                    cseg();
            }
            else
                oprintf(outputFile, 
                    "_CONST\tSEGMENT USE32 PUBLIC align=16 \042CONST\042\n");
            curseg = constseg;
        }
    }
    else
        curseg = constseg;
}

//-------------------------------------------------------------------------

void xstringseg(void)
{
    if (prm_asmfile)
    {
		asm_header();
        if (curseg != stringseg)
        {
            nl();
            exitseg();
            if (prm_nasm)
            {
                if (!prm_nodos)
                    oprintf(outputFile, "SECTION %s\n", segnames[stringseg]);
                else
                    dseg();
            }
            else
                oprintf(outputFile, 
                    "_STRING\tSEGMENT USE32 PUBLIC align=4 \042STRING\042\n");
            curseg = stringseg;
        }
    }
    else
        curseg = stringseg;
}

/*
 * Switch to deseg
 */
void dseg(void)
{
    if (prm_asmfile)
    {
		asm_header();
        if (curseg != dataseg)
        {
            nl();
            exitseg();
            if (prm_nasm)
                if (!prm_nodos)
                    oprintf(outputFile, "SECTION %s\n", segnames[dataseg]);
                else
                    oprintf(outputFile, "SECTION .data\n");
                else
                    oprintf(outputFile, "\t.DATA\n");
            curseg = dataseg;
        }
    }
    else
        curseg = dataseg;
}

/*
 * Switch to bssseg
 */
void bssseg(void)
{
    if (prm_asmfile)
    {
		asm_header();
        if (curseg != bssxseg)
        {
            nl();
            exitseg();
            if (prm_nasm)
                if (!prm_nodos)
                    oprintf(outputFile, "SECTION %s\n", segnames[bssxseg]);
                else
                    oprintf(outputFile, "SECTION .bss\n");
                else
                    oprintf(outputFile, "\t.DATA?\n");
            curseg = bssxseg;
        }
    }
    else
        curseg = bssxseg;
}

/*
 * Switch to startupseg
 */
void startupseg(void)
{
    if (prm_asmfile)
    {
		asm_header();
        if (curseg != startupxseg)
        {
            nl();
            exitseg();
            if (prm_nasm)
            {
                if (!prm_nodos)
                    oprintf(outputFile, "SECTION %s\n", segnames[startupxseg]);
                else
                    dseg();
            }
            else
                oprintf(outputFile, 
                    "_INIT_\tSEGMENT USE32 PUBLIC align=1 \042INITDATA\042\n");
            curseg = startupxseg;
        }
    }
    else
        curseg = startupxseg;
}

/*
 * Switch to rundownseg
 */
void rundownseg(void)
{
    if (prm_asmfile)
    {
		asm_header();
        if (curseg != rundownxseg)
        {
            nl();
            exitseg();
            if (prm_nasm)
            {
                if (!prm_nodos)
                    oprintf(outputFile, "SECTION %s\n", segnames[rundownxseg]);
                else
                    dseg();
            }
            else
                oprintf(outputFile, 
                    "_EXIT_\tSEGMENT USE32 PUBLIC align=1 \042EXITDATA\042\n");
            curseg = rundownxseg;
        }
    }
    else
        curseg = rundownxseg;
}


//-------------------------------------------------------------------------

    void gen_virtual(SYM *sp, int data)
    {
        if (!data && curseg != codeseg)
            cseg();
        virtual_mode = data;
        curseg = virtseg;
        if (prm_asmfile)
        {
            char buf[512];
            putsym(buf, sp, sp->name);
            nl();
            if (prm_nasm)
            {
                curseg = noseg;
                oprintf(outputFile, "\tSECTION @%s VIRTUAL\n", buf);
            }
            else
                oprintf(outputFile, "@%s\tSEGMENT VIRTUAL\n", buf);
            oprintf(outputFile, "%s:\n", buf);
        }
        else
            outcode_start_virtual_seg(sp, data);
    }
    void gen_endvirtual(SYM *sp)
    {
        if (prm_asmfile)
        {
            nl();
            if (!prm_nasm)
            {
                char buf[512];
                putsym(buf, sp, sp->name);
                oprintf(outputFile, "@%s\tENDS\n", buf);
            }
            else
                if (virtual_mode)
                    dseg();
                else
                    cseg();
        }
        else
            outcode_end_virtual_seg(sp);
        curseg = noseg;
    }
/*
 * Align
 */
void align(int size)
{
    if (curseg == codeseg)
        return ;
    if (prm_asmfile)
    {
        nl();
        if (prm_nasm)
        /* NASM 0.91 wouldn't let me use parenthesis but this should work
         * according to the documented precedence levels
         */
            oprintf(outputFile, "\tTIMES $$-$ & %d NOP\n", 3);
        else
            oprintf(outputFile, "\tALIGN\t%d\n", 4);
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
    if (prm_optmult)
    {
        while (p)
        {
            if (p->size == 0 && p->value == number)
                return p->label;
            p = p->link;
        }
    }
    global_flag++;
    p = xalloc(sizeof(MULDIV));
    global_flag--;
    p->link = 0;
    p->value = number;
    p->size = 0;
    while (*q)
        q = &(*q)->link;
    *q = p;
    return p->label = nextlabel++;
}

//-------------------------------------------------------------------------

long queue_floatval(FPF *number, int size)
{
    MULDIV *p = muldivlink, **q = &muldivlink;
    if (prm_optmult)
    {
        while (p)
        {
            if (p->size == size && !memcmp(&p->floatvalue, number, sizeof(FPF)))
                return p->label;
            p = p->link;
        }
    }
    global_flag++;
    p = xalloc(sizeof(MULDIV));
    global_flag--;
    p->link = 0;
    p->floatvalue = *number;
    p->size = size;
    while (*q)
        q = &(*q)->link;
    *q = p;
    return p->label = nextlabel++;
}

//-------------------------------------------------------------------------

void dump_muldivval(void)
{
    int tag = FALSE;
    xconstseg();
    if (prm_asmfile)
    {
        oprintf(outputFile, "\n");
        if (muldivlink)
        {
            tag = TRUE;
            align(4);
        }
        while (muldivlink)
        {
            put_label(muldivlink->label);
            if (muldivlink->size == 0)
                oprintf(outputFile, "\tDD\t0%xH\n", muldivlink->value);
            else 
			{
				char buf[512];
				int i;
        		FPFToString(buf,&muldivlink->floatvalue);
				if (muldivlink->size == 7 || muldivlink->size == 15)
				{
					if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
						|| !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
					{
						unsigned char dta[4];
						FPFToFloat(dta, &muldivlink->floatvalue);
						oprintf(outputFile,"\tDB\t");
						for (i=0; i < 4; i++)
						{
							oprintf(outputFile, "0%02XH", dta[i]);
							if (i != 3)
								oprintf(outputFile,", ");
						}
						oprintf(outputFile,"\n");
					}
					else
	                	oprintf(outputFile, "\tDD\t%s\n", buf);
				}
	            else if (muldivlink->size == 8 || muldivlink->size == 16)
				{
					if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
						|| !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
					{
						unsigned char dta[8];
						FPFToDouble(dta, &muldivlink->floatvalue);
						oprintf(outputFile,"\tDB\t");
						for (i=0; i < 8; i++)
						{
							oprintf(outputFile, "0%02XH", dta[i]);
							if (i != 7)
								oprintf(outputFile,", ");
						}
						oprintf(outputFile,"\n");
					}
					else
	    	            oprintf(outputFile, "\tDQ\t%s\n", buf);
				}
        	    else
				{
					if (!strcmp(buf,"inf") || !strcmp(buf, "nan")
						|| !strcmp(buf,"-inf") || !strcmp(buf, "-nan"))
					{
						unsigned char dta[10];
						FPFToLongDouble(dta, &muldivlink->floatvalue);
						oprintf(outputFile,"\tDB\t");
						for (i=0; i < 10; i++)
						{
							oprintf(outputFile, "0%02XH", dta[i]);
							if (i != 9)
								oprintf(outputFile,", ");
						}
						oprintf(outputFile,"\n");
					}
					else
	            	    oprintf(outputFile, "\tDT\t%s\n", buf);
				}
			}
            muldivlink = muldivlink->link;
        }
        if (tag)
            oprintf(outputFile, "\n");
    }
    else
        outcode_dump_muldivval();
}

//-------------------------------------------------------------------------

void dump_browsedata(unsigned char *data, int len)
{
    if (!prm_asmfile)
        outcode_dump_browsedata(data, len);
}

//-------------------------------------------------------------------------

void asm_header(void)
{
	if (hasheader++)
		return;
    nl();
    if (prm_nasm)
    {
        if (!prm_nodos)
        {
            oprintf(outputFile, "\tSECTION %s align=1 CLASS=%s USE32\n", segnames[codeseg], segclasses[codeseg]);
            oprintf(outputFile, "\tSECTION %s align=16 CLASS=%s USE32\n", segnames[dataseg], segclasses[dataseg]);
            oprintf(outputFile, "\tSECTION %s align=16 CLASS=%s USE32\n", segnames[bssxseg], segclasses[bssxseg]);
            oprintf(outputFile, "\tSECTION %s align=16 CLASS=%s USE32\n", segnames[constseg], segclasses[constseg]);
            oprintf(outputFile, "\tSECTION %s align=4 CLASS=%s USE32\n", segnames[stringseg], segclasses[stringseg])
                ;
            oprintf(outputFile, 
                "\tSECTION %s align=1 CLASS=%s USE32\n", segnames[startupxseg], segclasses[startupxseg]);
            oprintf(outputFile, 
                "\tSECTION %s align=1 CLASS=%s USE32\n", segnames[rundownxseg], segclasses[rundownxseg]);
            oprintf(outputFile, 
                "\tSECTION %s  align=1 CLASS=%s USE32\n", segnames[cppxseg], segclasses[cppxseg]);
            oprintf(outputFile, 
                "\tSECTION %s  align=1 CLASS=%s USE32\n", segnames[cpprseg], segclasses[cpprseg]);
			if (prm_fardefault)
	            oprintf(outputFile, "\tGROUP DGROUP %s %s\n\n", segnames[dataseg], segnames[bssxseg]);
			else
	            oprintf(outputFile, "\tGROUP DGROUP %s %s %s %s\n\n", segnames[dataseg], segnames[bssxseg], segnames[constseg], segnames[stringseg]);
        }
        else
        {
            oprintf(outputFile, "\tSECTION .text\n");
            oprintf(outputFile, "\tSECTION .data\n");
            oprintf(outputFile, "\tSECTION .bss\n");
        }
    }
    else
    {
        oprintf(outputFile, "\tTITLE\t'%s'\n", outfile);
        if (prm_flat)
            oprintf(outputFile, "\t.486p\n\t.MODEL FLAT\n\n");
        else
            if (prm_masm)
                oprintf(outputFile, "\t.486p\n\t.MODEL SMALL\n\n");
            else
                oprintf(outputFile, "\t.486p\n\t.MODEL USE32 SMALL\n\n");
    }
}

//-------------------------------------------------------------------------

void globaldef(SYM *sp)
{
    char buf[512],  *q = buf,  *p = sp->name;
    if (prm_asmfile)
    {
        putsym(buf, sp, p);
        if (prm_nasm)
            oprintf(outputFile, "[GLOBAL\t%s]\n", buf);
        else
            oprintf(outputFile, "\tPUBLIC\t%s\n", buf);
    }
}

//-------------------------------------------------------------------------

void output_alias(char *name, char *alias)
{
    if (prm_asmfile)
    {
        if (prm_nasm)
            oprintf(outputFile, "%%define %s %s\n", name, alias);
        else
            oprintf(outputFile, "%s EQU\t<%s>\n", name, alias);
    }
}

//-------------------------------------------------------------------------

int put_exdata(SYM *sp, int notyet)
{
    char buf[512],  *q = buf,  *p = sp->name;
    if (sp->absflag)
        return notyet ;
    putsym(buf, sp, p);
    if (notyet)
    {
        dseg();
    }
    if (prm_nasm)
    {
        oprintf(outputFile, "[EXTERN\t%s]\n", buf);
        if (sp->importable && sp->importfile)
            oprintf(outputFile, "\timport %s %s\n", buf, sp->importfile);
    }
    else
    {
        oprintf(outputFile, "\tEXTRN\t%s:BYTE\n", buf); 
            // compiler generates fully qualified references
        if (sp->importable && sp->importfile)
            oprintf(outputFile, "\timport %s %s\n", buf, sp->importfile);
    }
    return notyet;
}

//-------------------------------------------------------------------------

int put_exfunc(SYM *sp, int notyet)
{
    char buf[512],  *q = buf,  *p = sp->name;
    putsym(buf, sp, p);
    if (notyet)
    {
        cseg();
        notyet = FALSE;
    }
    if (prm_nasm)
    {
        oprintf(outputFile, "[EXTERN\t%s]\n", buf);
        if (sp->importable && sp->importfile)
            oprintf(outputFile, "\timport %s %s\n", buf, sp->importfile);
    }
    else
    {
        oprintf(outputFile, "\tEXTRN\t%s:PROC\n", buf);
        if (sp->importable && sp->importfile)
            oprintf(outputFile, "\timport %s %s\n", buf, sp->importfile);
    }
    return notyet;
}

//-------------------------------------------------------------------------

int put_expfunc(SYM *sp, int notyet)
{
    char buf[512];
    if (notyet)
    {
        cseg();
        notyet = FALSE;
    }
    putsym(buf, sp, sp->name);
    if (prm_nasm)
        oprintf(outputFile, "\texport %s\n", buf);
    else
        oprintf(outputFile, "\tpublicdll %s\n", buf);
}

//-------------------------------------------------------------------------

static int dumphashtable(int notyet, HASHREC **syms)
{
    int i;
    SYM *sp;
    char buf[100];
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        if ((sp = (SYM*)syms[i]) != 0)
        {
            while (sp)
            {
                if (sp->storage_class == sc_externalfunc && sp->mainsym
                    ->extflag && !(sp->tp->cflags &DF_INTRINS))
                    notyet = put_exfunc(sp, notyet);
                if (sp->storage_class == sc_external && sp->mainsym->extflag)
                    notyet = put_exfunc(sp, notyet);
                if (sp->storage_class == sc_defunc)
                {
                    SYM *sp1 = sp->tp->lst.head;
                    while (sp1)
                    {
                        if (sp1->mainsym->storage_class == sc_externalfunc &&
                            sp1->mainsym->extflag && !(sp1->tp->cflags
                            &DF_INTRINS))
                        {
                            notyet = put_exfunc(sp1, notyet);
                        }
                        if (sp1->mainsym->exportable && !(sp1->value.classdata.cppflags & PF_INLINE))
                            notyet = put_expfunc(sp1, notyet);
                        sp1 = sp1->next;
                    }
                }
                if (sp->storage_class == sc_namespace && !sp
                    ->value.classdata.parentns->anonymous)
                    notyet = dumphashtable(notyet, sp->value.classdata.parentns
                        ->table);
                if (sp->mainsym->exportable && !(sp->value.classdata.cppflags & PF_INLINE))
                    notyet = put_expfunc(sp, notyet);
                sp = sp->next;
            }
        }
    }
    return notyet;
}

//-------------------------------------------------------------------------

void putexterns(void)
/*
 * Output the fixup tables and the global/external list
 */
{
    SYM *sp;
    int i;
    LIST *l;
    char buf[100];
    if (prm_asmfile)
    {
        int notyet = TRUE;
        nl();
        exitseg();
        notyet = dumphashtable(notyet, gsyms);

        while (localfuncs)
        {
            sp = localfuncs->data;
            if (sp->storage_class == sc_externalfunc && sp->mainsym->extflag &&
                !(sp->tp->cflags &DF_INTRINS))
                notyet = put_exfunc(sp, notyet);
            if (sp->exportable)
                notyet = put_expfunc(sp, notyet);
            localfuncs = localfuncs->link;
        }
        notyet = TRUE;

        for (i = 0; i < HASHTABLESIZE; i++)
        {
            struct _templatelist *tl;
            if ((tl = (struct templatelist*)templateFuncs[i]) != 0)
            {
                while (tl)
                {
                    if (!tl->sp->value.classdata.templatedata->hasbody && tl
                        ->finalsp)
                        notyet = put_exfunc(tl->finalsp, notyet);
                    tl = tl->next;
                } 
            }
        }
        notyet = TRUE;
        exitseg();
        while (localdata)
        {
            sp = localdata->data;
            if (sp->mainsym->extflag)
            {
                notyet = put_exdata(sp, notyet);
            }
            localdata = localdata->link;
        }
        if (!prm_nasm)
        {
            if (libincludes)
            {
                while (libincludes)
                {
                    oprintf(outputFile, "\tINCLUDELIB\t%s\n", libincludes->data)
                        ;
                    libincludes = libincludes->link;
                }
                oputc('\n', outputFile);
            }
            oprintf(outputFile, "\tEND\n");
        }
    }
}
