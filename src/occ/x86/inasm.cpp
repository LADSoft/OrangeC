/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

/*
 * inline assembler (386)
 */
#include <stdio.h>
#include <string.h>
#include "be.h"

extern "C" int codeLabel;
extern "C" int prm_assembler;
extern "C" HASHTABLE* labelSyms;
extern "C" int usingEsp;

static ASMREG* regimage;
static ASMNAME* insdata;
static LEXEME* lex;

static SYMBOL* lastsym;
static enum e_opcode op;

#define ERR_LABEL_EXPECTED 0
#define ERR_ILLEGAL_ADDRESS_MODE 1
#define ERR_ADDRESS_MODE_EXPECTED 2
#define ERR_INVALID_OPCODE 3
#define ERR_INVALID_SIZE 4
#define ERR_INVALID_INDEX_MODE 5
#define ERR_INVALID_SCALE_SPECIFIER 6
#define ERR_USE_LEA 7
#define ERR_TOO_MANY_SEGMENTS 8
static char* errors[] = {"Lable expected",
                         "Illegal address mode",
                         "Address mode expected",
                         "Invalid opcode",
                         "Invalid instruction size",
                         "Invalid index mode",
                         "Invalid scale specifier",
                         "Use LEA to take address of auto variable",
                         "Too many segment specifiers"};
ASMNAME directiveLst[] = {{"db", op_reserved, ISZ_UCHAR, 0},
                          {"dw", op_reserved, ISZ_USHORT, 0},
                          {"dd", op_reserved, ISZ_ULONG, 0},
                          {"dq", op_reserved, ISZ_ULONGLONG, 0},
                          {"dt", op_reserved, ISZ_LDOUBLE, 0},
                          {"label", op_label, 0, 0},
                          {0}};
extern "C" ASMREG reglst[] = {{"cs", am_seg, 1, ISZ_USHORT},     {"ds", am_seg, 2, ISZ_USHORT},
                   {"es", am_seg, 3, ISZ_USHORT},     {"fs", am_seg, 4, ISZ_USHORT},
                   {"gs", am_seg, 5, ISZ_USHORT},     {"ss", am_seg, 6, ISZ_USHORT},
                   {"al", am_dreg, 0, ISZ_UCHAR},     {"cl", am_dreg, 1, ISZ_UCHAR},
                   {"dl", am_dreg, 2, ISZ_UCHAR},     {"bl", am_dreg, 3, ISZ_UCHAR},
                   {"ah", am_dreg, 4, ISZ_UCHAR},     {"ch", am_dreg, 5, ISZ_UCHAR},
                   {"dh", am_dreg, 6, ISZ_UCHAR},     {"bh", am_dreg, 7, ISZ_UCHAR},
                   {"ax", am_dreg, 0, ISZ_USHORT},    {"cx", am_dreg, 1, ISZ_USHORT},
                   {"dx", am_dreg, 2, ISZ_USHORT},    {"bx", am_dreg, 3, ISZ_USHORT},
                   {"sp", am_dreg, 4, ISZ_USHORT},    {"bp", am_dreg, 5, ISZ_USHORT},
                   {"si", am_dreg, 6, ISZ_USHORT},    {"di", am_dreg, 7, ISZ_USHORT},
                   {"eax", am_dreg, 0, ISZ_UINT},     {"ecx", am_dreg, 1, ISZ_UINT},
                   {"edx", am_dreg, 2, ISZ_UINT},     {"ebx", am_dreg, 3, ISZ_UINT},
                   {"esp", am_dreg, 4, ISZ_UINT},     {"ebp", am_dreg, 5, ISZ_UINT},
                   {"esi", am_dreg, 6, ISZ_UINT},     {"edi", am_dreg, 7, ISZ_UINT},
                   {"st", am_freg, 0, ISZ_LDOUBLE},   {"cr0", am_screg, 0, ISZ_UINT},
                   {"cr1", am_screg, 1, ISZ_UINT},    {"cr2", am_screg, 2, ISZ_UINT},
                   {"cr3", am_screg, 3, ISZ_UINT},    {"cr4", am_screg, 4, ISZ_UINT},
                   {"cr5", am_screg, 5, ISZ_UINT},    {"cr6", am_screg, 6, ISZ_UINT},
                   {"cr7", am_screg, 7, ISZ_UINT},    {"dr0", am_sdreg, 0, ISZ_UINT},
                   {"dr1", am_sdreg, 1, ISZ_UINT},    {"dr2", am_sdreg, 2, ISZ_UINT},
                   {"dr3", am_sdreg, 3, ISZ_UINT},    {"dr4", am_sdreg, 4, ISZ_UINT},
                   {"dr5", am_sdreg, 5, ISZ_UINT},    {"dr6", am_sdreg, 6, ISZ_UINT},
                   {"dr7", am_sdreg, 7, ISZ_UINT},    {"tr0", am_streg, 0, ISZ_UINT},
                   {"tr1", am_streg, 1, ISZ_UINT},    {"tr2", am_streg, 2, ISZ_UINT},
                   {"tr3", am_streg, 3, ISZ_UINT},    {"tr4", am_streg, 4, ISZ_UINT},
                   {"tr5", am_streg, 5, ISZ_UINT},    {"tr6", am_streg, 6, ISZ_UINT},
                   {"tr7", am_streg, 7, ISZ_UINT},    {"byte", am_ext, akw_byte, 0},
                   {"word", am_ext, akw_word, 0},     {"dword", am_ext, akw_dword, 0},
                   {"fword", am_ext, akw_fword, 0},   {"qword", am_ext, akw_qword, 0},
                   {"tbyte", am_ext, akw_tbyte, 0},   {"ptr", am_ext, akw_ptr, 0},
                   {"offset", am_ext, akw_offset, 0}, {0, 0, 0}};
/* List of opcodes
* This list MUST be in the same order as the op_ enums
*/
#define ENTRY(x,y,z,w) { x, y }
extern "C" ASMNAME oplst[] = {
    ENTRY("reserved", op_reserved, 0, 0),
    ENTRY("line#", op_reserved, 0, 0),
    ENTRY("blks#", op_reserved, 0, 0),
    ENTRY("blke#", op_reserved, 0, 0),
    ENTRY("vars#", op_reserved, 0, 0),
    ENTRY("funcs#", op_reserved, 0, 0),
    ENTRY("funce#", op_reserved, 0, 0),
    ENTRY("void#", op_void, 0, 0),
    ENTRY("cmt#", op_reserved, 0, 0),
    ENTRY("label#", op_reserved, 0, 0),
    ENTRY("flabel#", op_reserved, 0, 0),
    ENTRY("seq@", op_reserved, 0, 0),
    ENTRY("db", op_reserved, 0, 0),
    ENTRY("dd", op_reserved, 0, 0),
    ENTRY("aaa", op_aaa, 0, &popn_aaa),
    ENTRY("aad", op_aad, 0, &popn_aad),
    ENTRY("aam", op_aam, 0, &popn_aam),
    ENTRY("aas", op_aas, 0, &popn_aas),
    ENTRY("add", op_add, OPE_MATH, &popn_add),
    ENTRY("adc", op_adc, OPE_MATH, &popn_adc),
    ENTRY("and", op_and, OPE_MATH, &popn_and),
    ENTRY("arpl", op_arpl, OPE_ARPL, &popn_arpl),
    ENTRY("bound", op_bound, OPE_BOUND, &popn_bound),
    ENTRY("bsf", op_bsf, OPE_BITSCAN, &popn_bsf),
    ENTRY("bsr", op_bsr, OPE_BITSCAN, &popn_bsr),
    ENTRY("bswap", op_bswap, OPE_REG32, &popn_bswap),
    ENTRY("btc", op_btc, OPE_BIT, &popn_btc),
    ENTRY("bt", op_bt, OPE_BIT, &popn_bt),
    ENTRY("btr", op_btr, OPE_BIT, &popn_btr),
    ENTRY("bts", op_bts, OPE_BIT, &popn_bts),
    ENTRY("call", op_call, OPE_CALL, &popn_call),
    ENTRY("cbw", op_cbw, 0, &popn_cbw),
    ENTRY("cwde", op_cwde, 0, &popn_cwde),
    ENTRY("cwd", op_cwd, 0, &popn_cwd),
    ENTRY("cdq", op_cdq, 0, &popn_cdq),
    ENTRY("clc", op_clc, 0, &popn_clc),
    ENTRY("cld", op_cld, 0, &popn_cld),
    ENTRY("cli", op_cli, 0, &popn_cli),
    ENTRY("clts", op_clts, 0, &popn_clts),
    ENTRY("cmc", op_cmc, 0, &popn_cmc),
    ENTRY("cmp", op_cmp, OPE_MATH, &popn_cmp),
    ENTRY("cmpxchg", op_cmpxchg, OPE_MATH, &popn_cmpxchg),
    ENTRY("cmpxchg8b", op_cmpxchg8b, OPE_RM, &popn_cmpxchg8b),
    ENTRY("cmps", op_cmps, OPE_CMPS, &popn_cmps),
    ENTRY("cmpsb", op_cmpsb, 0, &popn_cmpsb),
    ENTRY("cmpsw", op_cmpsw, 0, &popn_cmpsw),
    ENTRY("cmpsd", op_cmpsd, 0, &popn_cmpsd),
    ENTRY("daa", op_daa, 0, &popn_daa),
    ENTRY("das", op_das, 0, &popn_das),
    ENTRY("dec", op_dec, OPE_INCDEC, &popn_dec),
    ENTRY("div", op_div, OPE_RM, &popn_div),
    ENTRY("enter", op_enter, OPE_ENTER, &popn_enter),
    ENTRY("hlt", op_hlt, 0, &popn_hlt),
    ENTRY("idiv", op_idiv, OPE_RM, &popn_idiv),
    ENTRY("imul", op_imul, OPE_IMUL, &popn_imul),
    ENTRY("in", op_in, OPE_IN, &popn_in),
    ENTRY("inc", op_inc, OPE_INCDEC, &popn_inc),
    ENTRY("ins", op_ins, OPE_INS, &popn_ins),
    ENTRY("insb", op_insb, 0, &popn_insb),
    ENTRY("insw", op_insw, 0, &popn_insw),
    ENTRY("insd", op_insd, 0, &popn_insd),
    ENTRY("int", op_int, OPE_IMM8, &popn_int),
    ENTRY("int3", op_int3, 0, &popn_int3),
    ENTRY("into", op_into, 0, &popn_into),
    ENTRY("invd", op_invd, 0, &popn_invd),
    ENTRY("iret", op_iret, 0, &popn_iret),
    ENTRY("iretd", op_iretd, 0, &popn_iretd),
    ENTRY("jecxz", op_jecxz, OPE_RELBR8, &popn_jcxz),
    ENTRY("ja", op_ja, OPE_RELBRA, &popn_ja),
    ENTRY("jnbe", op_jnbe, OPE_RELBRA, &popn_jnbe),
    ENTRY("jae", op_jae, OPE_RELBRA, &popn_jae),
    ENTRY("jnb", op_jnb, OPE_RELBRA, &popn_jnb),
    ENTRY("jnc", op_jnc, OPE_RELBRA, &popn_jnc),
    ENTRY("jb", op_jb, OPE_RELBRA, &popn_jb),
    ENTRY("jc", op_jc, OPE_RELBRA, &popn_jc),
    ENTRY("jnae", op_jnae, OPE_RELBRA, &popn_jnae),
    ENTRY("jbe", op_jbe, OPE_RELBRA, &popn_jbe),
    ENTRY("jna", op_jna, OPE_RELBRA, &popn_jna),
    ENTRY("je", op_je, OPE_RELBRA, &popn_je),
    ENTRY("jz", op_jz, OPE_RELBRA, &popn_jz),
    ENTRY("jg", op_jg, OPE_RELBRA, &popn_jg),
    ENTRY("jnle", op_jnle, OPE_RELBRA, &popn_jnle),
    ENTRY("jl", op_jl, OPE_RELBRA, &popn_jl),
    ENTRY("jnge", op_jnge, OPE_RELBRA, &popn_jnge),
    ENTRY("jge", op_jge, OPE_RELBRA, &popn_jge),
    ENTRY("jnl", op_jnl, OPE_RELBRA, &popn_jnl),
    ENTRY("jle", op_jle, OPE_RELBRA, &popn_jle),
    ENTRY("jng", op_jng, OPE_RELBRA, &popn_jng),
    ENTRY("jne", op_jne, OPE_RELBRA, &popn_jne),
    ENTRY("jnz", op_jnz, OPE_RELBRA, &popn_jnz),
    ENTRY("jo", op_jo, OPE_RELBRA, &popn_jo),
    ENTRY("jno", op_jno, OPE_RELBRA, &popn_jno),
    ENTRY("jp", op_jp, OPE_RELBRA, &popn_jp),
    ENTRY("jnp", op_jnp, OPE_RELBRA, &popn_jnp),
    ENTRY("jpe", op_jpe, OPE_RELBRA, &popn_jpe),
    ENTRY("jpo", op_jpo, OPE_RELBRA, &popn_jpo),
    ENTRY("js", op_js, OPE_RELBRA, &popn_js),
    ENTRY("jns", op_jns, OPE_RELBRA, &popn_jns),
    ENTRY("jmp", op_jmp, OPE_JMP, &popn_jmp),
    ENTRY("lahf", op_lahf, 0, &popn_lahf),
    ENTRY("lar", op_lar, OPE_REGRM, &popn_lar),
    ENTRY("lds", op_lds, OPE_LOADSEG, &popn_lds),
    ENTRY("les", op_les, OPE_LOADSEG, &popn_les),
    ENTRY("lfs", op_lfs, OPE_LOADSEG, &popn_lfs),
    ENTRY("lgs", op_lgs, OPE_LOADSEG, &popn_lgs),
    ENTRY("lss", op_lss, OPE_LOADSEG, &popn_lss),
    ENTRY("lea", op_lea, OPE_REGRM, &popn_lea),
    ENTRY("leave", op_leave, 0, &popn_leave),
    ENTRY("lfence", op_lfence, 0, &popn_lfence),
    ENTRY("lgdt", op_lgdt, OPE_LGDT, &popn_lgdt),
    ENTRY("lidt", op_lidt, OPE_LIDT, &popn_lidt),
    ENTRY("lldt", op_lldt, OPE_RM16, &popn_lldt),
    ENTRY("lmsw", op_lmsw, OPE_RM16, &popn_lmsw),
    ENTRY("lock", op_lock, 0, &popn_lock),
    ENTRY("lods", op_lods, OPE_LODS, &popn_lods),
    ENTRY("lodsb", op_lodsb, 0, &popn_lodsb),
    ENTRY("lodsw", op_lodsw, 0, &popn_lodsw),
    ENTRY("lodsd", op_lodsd, 0, &popn_lodsd),
    ENTRY("loop", op_loop, OPE_RELBR8, &popn_loop),
    ENTRY("loope", op_loope, OPE_RELBR8, &popn_loope),
    ENTRY("loopz", op_loopz, OPE_RELBR8, &popn_loopz),
    ENTRY("loopne", op_loopne, OPE_RELBR8, &popn_loopne),
    ENTRY("loopnz", op_loopnz, OPE_RELBR8, &popn_loopnz),
    ENTRY("lsl", op_lsl, OPE_REGRM, &popn_lsl),
    ENTRY("ltr", op_ltr, OPE_RM16, &popn_ltr),
    ENTRY("mfence", op_mfence, 0, &popn_mfence),
    ENTRY("mov", op_mov, OPE_MOV, &popn_mov),
    ENTRY("movs", op_movs, OPE_MOVS, &popn_movs),
    ENTRY("movsb", op_movsb, 0, &popn_movsb),
    ENTRY("movsw", op_movsw, 0, &popn_movsw),
    ENTRY("movsd", op_movsd, 0, &popn_movsd),
    ENTRY("movsx", op_movsx, OPE_MOVSX, &popn_movsx),
    ENTRY("movzx", op_movzx, OPE_MOVSX, &popn_movzx),
    ENTRY("mul", op_mul, OPE_RM, &popn_mul),
    ENTRY("neg", op_neg, OPE_RM, &popn_neg),
    ENTRY("not", op_not, OPE_RM, &popn_not),
    ENTRY("nop", op_nop, 0, &popn_nop),
    ENTRY("or", op_or, OPE_MATH, &popn_or),
    ENTRY("out", op_out, OPE_OUT, &popn_out),
    ENTRY("outs", op_outs, OPE_OUTS, &popn_outs),
    ENTRY("outsb", op_outsb, 0, &popn_outsb),
    ENTRY("outsw", op_outsw, 0, &popn_outsw),
    ENTRY("outsd", op_outsd, 0, &popn_outsd),
    ENTRY("pop", op_pop, OPE_PUSHPOP, &popn_pop),
    ENTRY("popa", op_popa, 0, &popn_popa),
    ENTRY("popad", op_popad, 0, &popn_popad),
    ENTRY("popf", op_popf, 0, &popn_popf),
    ENTRY("popfd", op_popfd, 0, &popn_popfd),
    ENTRY("push", op_push, OPE_PUSHPOP, &popn_push),
    ENTRY("pusha", op_pusha, 0, &popn_pusha),
    ENTRY("pushad", op_pushad, 0, &popn_pushad),
    ENTRY("pushf", op_pushf, 0, &popn_pushf),
    ENTRY("pushfd", op_pushfd, 0, &popn_pushfd),
    ENTRY("rcl", op_rcl, OPE_SHIFT, &popn_rcl),
    ENTRY("rcr", op_rcr, OPE_SHIFT, &popn_rcr),
    ENTRY("rdmsr", op_rdmsr, 0, &popn_rdmsr),
    ENTRY("rdpmc", op_rdpmc, 0, &popn_rdpmc),
    ENTRY("rdtsc", op_rdtsc, 0, &popn_rdtsc),
    ENTRY("rol", op_rol, OPE_SHIFT, &popn_rol),
    ENTRY("ror", op_ror, OPE_SHIFT, &popn_ror),
    ENTRY("rep", op_rep, 0, &popn_repz),
    ENTRY("repne", op_repne, 0, &popn_repnz),
    ENTRY("repe", op_repe, 0, &popn_repz),
    ENTRY("repnz", op_repnz, 0, &popn_repnz),
    ENTRY("repz", op_repz, 0, &popn_repz),
    ENTRY("ret", op_ret, OPE_RET, &popn_ret),
    ENTRY("retf", op_retf, OPE_RET, &popn_retf),
    ENTRY("sahf", op_sahf, 0, &popn_sahf),
    ENTRY("sal", op_sal, OPE_SHIFT, &popn_sal),
    ENTRY("sar", op_sar, OPE_SHIFT, &popn_sar),
    ENTRY("shl", op_shl, OPE_SHIFT, &popn_shl),
    ENTRY("shr", op_shr, OPE_SHIFT, &popn_shr),
    ENTRY("sbb", op_sbb, OPE_MATH, &popn_sbb),
    ENTRY("scas", op_scas, OPE_SCAS, &popn_scas),
    ENTRY("scasb", op_scasb, 0, &popn_scasb),
    ENTRY("scasw", op_scasw, 0, &popn_scasw),
    ENTRY("scasd", op_scasd, 0, &popn_scasd),
    ENTRY("seta", op_seta, OPE_SET, &popn_seta),
    ENTRY("setnbe", op_setnbe, OPE_SET, &popn_setnbe),
    ENTRY("setae", op_setae, OPE_SET, &popn_setae),
    ENTRY("setnb", op_setnb, OPE_SET, &popn_setnb),
    ENTRY("setnc", op_setnc, OPE_SET, &popn_setnc),
    ENTRY("setb", op_setb, OPE_SET, &popn_setb),
    ENTRY("setc", op_setc, OPE_SET, &popn_setc),
    ENTRY("setnae", op_setnae, OPE_SET, &popn_setnae),
    ENTRY("setbe", op_setbe, OPE_SET, &popn_setbe),
    ENTRY("setna", op_setna, OPE_SET, &popn_setna),
    ENTRY("sete", op_sete, OPE_SET, &popn_sete),
    ENTRY("setz", op_setz, OPE_SET, &popn_setz),
    ENTRY("setg", op_setg, OPE_SET, &popn_setg),
    ENTRY("setnle", op_setnle, OPE_SET, &popn_setnle),
    ENTRY("setl", op_setl, OPE_SET, &popn_setl),
    ENTRY("setnge", op_setnge, OPE_SET, &popn_setnge),
    ENTRY("setge", op_setge, OPE_SET, &popn_setge),
    ENTRY("setnl", op_setnl, OPE_SET, &popn_setnl),
    ENTRY("setle", op_setle, OPE_SET, &popn_setle),
    ENTRY("setng", op_setng, OPE_SET, &popn_setng),
    ENTRY("setne", op_setne, OPE_SET, &popn_setne),
    ENTRY("setnz", op_setnz, OPE_SET, &popn_setnz),
    ENTRY("seto", op_seto, OPE_SET, &popn_seto),
    ENTRY("setno", op_setno, OPE_SET, &popn_setno),
    ENTRY("setp", op_setp, OPE_SET, &popn_setp),
    ENTRY("setnp", op_setnp, OPE_SET, &popn_setnp),
    ENTRY("setpe", op_setpe, OPE_SET, &popn_setpe),
    ENTRY("setpo", op_setpo, OPE_SET, &popn_setpo),
    ENTRY("sets", op_sets, OPE_SET, &popn_sets),
    ENTRY("setns", op_setns, OPE_SET, &popn_setns),
    ENTRY("sfence", op_sfence, 0, &popn_sfence),
    ENTRY("sgdt", op_sgdt, OPE_LGDT, &popn_sgdt),
    ENTRY("sidt", op_sidt, OPE_LIDT, &popn_sidt),
    ENTRY("sldt", op_sldt, OPE_RM16, &popn_sldt),
    ENTRY("smsw", op_smsw, OPE_RM16, &popn_smsw),
    ENTRY("shld", op_shld, OPE_SHLD, &popn_shld),
    ENTRY("shrd", op_shrd, OPE_SHLD, &popn_shrd),
    ENTRY("stc", op_stc, 0, &popn_stc),
    ENTRY("std", op_std, 0, &popn_std),
    ENTRY("sti", op_sti, 0, &popn_sti),
    ENTRY("stos", op_stos, OPE_STOS, &popn_stos),
    ENTRY("stosb", op_stosb, 0, &popn_stosb),
    ENTRY("stosw", op_stosw, 0, &popn_stosw),
    ENTRY("stosd", op_stosd, 0, &popn_stosd),
    ENTRY("str", op_str, OPE_RM16, &popn_str),
    ENTRY("sub", op_sub, OPE_MATH, &popn_sub),
    ENTRY("test", op_test, OPE_TEST, &popn_test),
    ENTRY("verr", op_verr, OPE_RM16, &popn_verr),
    ENTRY("verw", op_verw, OPE_RM16, &popn_verw),
    ENTRY("wait", op_wait, 0, &popn_wait),
    ENTRY("wbinvd", op_wbinvd, 0, &popn_wbinvd),
    ENTRY("wrmsr", op_wrmsr, 0, &popn_wrmsr),
    ENTRY("xchg", op_xchg, OPE_XCHG, &popn_xchg),
    ENTRY("xlat", op_xlat, OPE_XLAT, &popn_xlat),
    ENTRY("xlatb", op_xlatb, 0, &popn_xlatb),
    ENTRY("xor", op_xor, OPE_MATH, &popn_xor),
    ENTRY("f2xm1", op_f2xm1, 0, &popn_f2xm1),
    ENTRY("fabs", op_fabs, 0, &popn_fabs),
    ENTRY("fadd", op_fadd, OPE_FMATH, &popn_fadd),
    ENTRY("faddp", op_faddp, OPE_FMATHP, &popn_faddp),
    ENTRY("fiadd", op_fiadd, OPE_FMATHI, &popn_fiadd),
    ENTRY("fchs", op_fchs, 0, &popn_fchs),
    ENTRY("fclex", op_fclex, 0, &popn_fclex),
    ENTRY("fnclex", op_fnclex, 0, &popn_fnclex),
    ENTRY("fcom", op_fcom, OPE_FCOM, &popn_fcom),
    ENTRY("fcomp", op_fcomp, OPE_FCOM, &popn_fcomp),
    ENTRY("fcompp", op_fcompp, 0, &popn_fcompp),
    ENTRY("fcos", op_fcos, 0, &popn_fcos),
    ENTRY("fdecstp", op_fdecstp, 0, &popn_fdecstp),
    ENTRY("fdiv", op_fdiv, OPE_FMATH, &popn_fdiv),
    ENTRY("fdivp", op_fdivp, OPE_FMATHP, &popn_fdivp),
    ENTRY("fidiv", op_fidiv, OPE_FMATHI, &popn_fidiv),
    ENTRY("fdivr", op_fdivr, OPE_FMATH, &popn_fdivr),
    ENTRY("fdivrp", op_fdivrp, OPE_FMATHP, &popn_fdivrp),
    ENTRY("fidivr", op_fidivr, OPE_FMATHI, &popn_fidivr),
    ENTRY("ffree", op_ffree, OPE_FREG, &popn_ffree),
    ENTRY("ficom", op_ficom, OPE_FICOM, &popn_ficom),
    ENTRY("ficomp", op_ficomp, OPE_FICOM, &popn_ficomp),
    ENTRY("fild", op_fild, OPE_FILD, &popn_fild),
    ENTRY("fincstp", op_fincstp, 0, &popn_fincstp),
    ENTRY("finit", op_finit, 0, &popn_finit),
    ENTRY("fninit", op_fninit, 0, &popn_fninit),
    ENTRY("fist", op_fist, OPE_FIST, &popn_fist),
    ENTRY("fistp", op_fistp, OPE_FILD, &popn_fistp),
    ENTRY("fld", op_fld, OPE_FLD, &popn_fld),
    ENTRY("fldz", op_fldz, 0, &popn_fldz),
    ENTRY("fldpi", op_fldpi, 0, &popn_fldpi),
    ENTRY("fld1", op_fld1, 0, &popn_fld1),
    ENTRY("fldl2t", op_fldl2t, 0, &popn_fldl2t),
    ENTRY("fldl2e", op_fldl2e, 0, &popn_fldl2e),
    ENTRY("fldlg2", op_fldlg2, 0, &popn_fldlg2),
    ENTRY("fldln2", op_fldln2, 0, &popn_fldln2),
    ENTRY("fldcw", op_fldcw, OPE_M16, &popn_fldcw),
    ENTRY("fldenv", op_fldenv, OPE_MN, &popn_fldenv),
    ENTRY("fmul", op_fmul, OPE_FMATH, &popn_fmul),
    ENTRY("fmulp", op_fmulp, OPE_FMATHP, &popn_fmulp),
    ENTRY("fimul", op_fimul, OPE_FMATHI, &popn_fimul),
    ENTRY("fpatan", op_fpatan, 0, &popn_fpatan),
    ENTRY("fprem", op_fprem, 0, &popn_fprem),
    ENTRY("fprem1", op_fprem1, 0, &popn_fprem1),
    ENTRY("fptan", op_fptan, 0, &popn_fptan),
    ENTRY("frndint", op_frndint, 0, &popn_frndint),
    ENTRY("frstor", op_frstor, OPE_MN, &popn_frstor),
    ENTRY("fsave", op_fsave, OPE_MN, &popn_fsave),
    ENTRY("fnsave", op_fnsave, OPE_MN, &popn_fnsave),
    ENTRY("fscale", op_fscale, 0, &popn_fscale),
    ENTRY("fsin", op_fsin, 0, &popn_fsin),
    ENTRY("fsincos", op_fsincos, 0, &popn_fsincos),
    ENTRY("fsqrt", op_fsqrt, 0, &popn_fsqrt),
    ENTRY("fst", op_fst, OPE_FST, &popn_fst),
    ENTRY("fstp", op_fstp, OPE_FSTP, &popn_fstp),
    ENTRY("fstcw", op_fstcw, OPE_M16, &popn_fstcw),
    ENTRY("fstsw", op_fstsw, OPE_M16, &popn_fstsw),
    ENTRY("fnstcw", op_fnstcw, OPE_M16, &popn_fnstcw),
    ENTRY("fnstsw", op_fnstsw, OPE_M16, &popn_fnstsw),
    ENTRY("fstenv", op_fstenv, OPE_MN, &popn_fstenv),
    ENTRY("fnstenv", op_fnstenv, OPE_MN, &popn_fnstenv),
    ENTRY("fsub", op_fsub, OPE_FMATH, &popn_fsub),
    ENTRY("fsubp", op_fsubp, OPE_FMATHP, &popn_fsubp),
    ENTRY("fisub", op_fisub, OPE_FMATHI, &popn_fisub),
    ENTRY("fsubr", op_fsubr, OPE_FMATH, &popn_fsubr),
    ENTRY("fsubrp", op_fsubrp, OPE_FMATHP, &popn_fsubrp),
    ENTRY("fisubr", op_fisubr, OPE_FMATHI, &popn_fisubr),
    ENTRY("ftst", op_ftst, 0, &popn_ftst),
    ENTRY("fucom", op_fucom, OPE_FUCOM, &popn_fucom),
    ENTRY("fucomp", op_fucomp, OPE_FUCOM, &popn_fucomp),
    ENTRY("fucompp", op_fucompp, 0, &popn_fucompp),
    ENTRY("fwait", op_fwait, 0, &popn_fwait),
    ENTRY("fxam", op_fxam, 0, &popn_fxam),
    ENTRY("fxch", op_fxch, OPE_FXCH, &popn_fxch),
    ENTRY("fxtract", op_fxtract, 0, &popn_fxtract),
    ENTRY("fyl2x", op_fyl2x, 0, &popn_fyl2x),
    ENTRY("fyl2xp1", op_fyl2xp1, 0, &popn_fyl2xp1),
    ENTRY(0, 0, 0, 0),
};

typedef struct
{
    char* name;
    int instruction;
    void* data;
} ASM_HASH_ENTRY;
static int floating;
static HASHTABLE* asmHash;
void inasmini(void)
{
    ASMREG* r = reglst;
    ASM_HASH_ENTRY* s;
    ASMNAME* o = oplst;
    asmHash = CreateHashTable(1021);
    while (r->name)
    {
        s = (ASM_HASH_ENTRY*)Alloc(sizeof(ASM_HASH_ENTRY));
        s->data = r;
        s->name = r->name;
        s->instruction = FALSE;
        insert((SYMBOL*)s, asmHash);
        r++;
    }
    while (o->name)
    {
        s = (ASM_HASH_ENTRY*)Alloc(sizeof(ASM_HASH_ENTRY));
        s->data = o;
        s->name = o->name;
        s->instruction = TRUE;
        insert((SYMBOL*)s, asmHash);
        o++;
    }
    if (cparams.prm_assemble)
    {
        o = directiveLst;
        while (o->name)
        {
            s = (ASM_HASH_ENTRY*)Alloc(sizeof(ASM_HASH_ENTRY));
            s->data = o;
            s->name = o->name;
            s->instruction = TRUE;
            insert((SYMBOL*)s, asmHash);
            o++;
        }
    }
}
static void inasm_err(int errnum)
{
    //    *lptr = 0;
    //    lastch = ' ';
    errorstr(ERR_ASM, errors[errnum]);
    lex = getsym();
}

static void inasm_txsym(void)
{
    if (lex && ISID(lex))
    {
        ASM_HASH_ENTRY* e = (ASM_HASH_ENTRY*)search(lex->value.s.a, asmHash);
        if (e)
        {
            if (e->instruction)
            {
                lex->type = l_asminst;
                insdata = (ASMNAME*)e->data;
            }
            else
            {
                lex->type = l_asmreg;
                regimage = (ASMREG*)e->data;
            }
        }
    }
}
static void inasm_getsym(void)
{
    lex = getsym();
    inasm_txsym();
}
static void inasm_needkw(LEXEME** lex, int Keyword)
{
    needkw(lex, (e_kw)Keyword);
    inasm_txsym();
}
static AMODE* inasm_const(void)
{
    AMODE* rv = NULL;
    TYPE* tp = NULL;
    EXPRESSION* exp = NULL;
    lex = optimized_expression(lex, beGetCurrentFunc, NULL, &tp, &exp, FALSE);
    if (!tp)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (!isint(tp) || !isintconst(exp))
    {
        error(ERR_CONSTANT_VALUE_EXPECTED);
    }
    else
    {
        rv = (AMODE *) Alloc(sizeof(AMODE));
        rv->mode = am_immed;
        rv->offset = exp;
    }
    return rv;
}
/*-------------------------------------------------------------------------*/

static EXPRESSION* inasm_ident(void)
{
    EXPRESSION* node = 0;

    if (lex->type != l_id)
        error(ERR_IDENTIFIER_EXPECTED);
    else
    {
        SYMBOL* sp;
        char nm[256];
        strcpy(nm, lex->value.s.a);
        inasm_getsym();
        /* No such identifier */
        /* label, put it in the symbol table */
        if ((sp = search(nm, labelSyms)) == 0 && (sp = gsearch(nm)) == 0)
        {
            sp = (SYMBOL *)Alloc(sizeof(SYMBOL));
            sp->storage_class = sc_ulabel;
            sp->name = litlate(nm);
            sp->declfile = sp->origdeclfile = lex->file;
            sp->declline = sp->origdeclline = lex->line;
            sp->realdeclline = lex->realline;
            sp->declfilenum = lex->filenum;
            sp->used = TRUE;
            sp->tp = (TYPE *)beLocalAlloc(sizeof(TYPE));
            sp->tp->type = bt_unsigned;
            sp->tp->bits = sp->tp->startbit = -1;
            sp->offset = codeLabel++;
            insert(sp, labelSyms);
            node = intNode(en_labcon, sp->offset);
        }
        else
        {
            /* If we get here the symbol was already in the table
             */
            sp->used = TRUE;
            switch (sp->storage_class)
            {
                case sc_absolute:
                    sp->genreffed = TRUE;
                    node = varNode(en_absolute, sp);
                    break;
                case sc_overloads:
                    node = varNode(en_pc, (SYMBOL*)sp->tp->syms->table[0]->p);
                    ((SYMBOL*)(sp->tp->syms->table[0]->p))->genreffed = TRUE;
                    break;
                case sc_localstatic:
                case sc_global:
                case sc_external:
                case sc_static:
                    sp->genreffed = TRUE;
                    node = varNode(en_global, sp);
                    break;
                case sc_const:
                    /* constants and enums */
                    node = intNode(en_c_i, sp->value.i);
                    break;
                case sc_label:
                case sc_ulabel:
                    node = intNode(en_labcon, sp->offset);
                    break;
                case sc_auto:
                case sc_register:
                    sp->allocate = TRUE;
                case sc_parameter:
                    node = varNode(en_auto, sp);
                    sp->inasm = TRUE;
                    break;
                default:
                    errorstr(ERR_INVALID_STORAGE_CLASS, "");
                    break;
            }
        }
        lastsym = sp;
    }
    return node;
}

/*-------------------------------------------------------------------------*/

static EXPRESSION* inasm_label(void)
{
    EXPRESSION* node;
    SYMBOL* sp;
    if (!ISID(lex))
    {
        lex = getsym();
        return NULL;
    }
    /* No such identifier */
    /* label, put it in the symbol table */
    if ((sp = search(lex->value.s.a, labelSyms)) == 0)
    {
        sp = (SYMBOL *)Alloc(sizeof(SYMBOL));
        sp->storage_class = sc_label;
        sp->name = litlate(lex->value.s.a);
        sp->declfile = sp->origdeclfile = lex->file;
        sp->declline = sp->origdeclline = lex->line;
        sp->realdeclline = lex->realline;
        sp->declfilenum = lex->filenum;
        sp->tp = (TYPE *)beLocalAlloc(sizeof(TYPE));
        sp->tp->type = bt_unsigned;
        sp->tp->bits = sp->tp->startbit = -1;
        sp->offset = codeLabel++;
        SetLinkerNames(sp, lk_none);
        insert(sp, labelSyms);
    }
    else
    {
        if (sp->storage_class == sc_label)
        {
            errorsym(ERR_DUPLICATE_LABEL, sp);
            inasm_getsym();
            return 0;
        }
        if (sp->storage_class != sc_ulabel)
        {
            inasm_err(ERR_LABEL_EXPECTED);
            return 0;
        }
        sp->storage_class = sc_label;
    }
    inasm_getsym();
    if (lex->type == l_asminst)
    {
        if (insdata->atype == op_reserved)
        {
            node = intNode(en_labcon, sp->offset);
            return node;
        }
        else if (insdata->atype != op_label)
        {
            inasm_err(ERR_LABEL_EXPECTED);
            return 0;
        }
    }
    else if (!MATCHKW(lex, colon))
    {
        inasm_err(ERR_LABEL_EXPECTED);
        return 0;
    }
    inasm_getsym();
    node = intNode(en_labcon, sp->offset);
    return node;
}

/*-------------------------------------------------------------------------*/

static int inasm_getsize(void)
{
    int sz = ISZ_NONE;
    switch (regimage->regnum)
    {
        case akw_byte:
            sz = ISZ_UCHAR;
            break;
        case akw_word:
            sz = ISZ_USHORT;
            break;
        case akw_dword:
            sz = floating ? ISZ_FLOAT : ISZ_UINT;
            break;
        case akw_fword:
            sz = ISZ_FARPTR;
            break;
        case akw_qword:
            sz = ISZ_ULONGLONG;
            break;
        case akw_tbyte:
            sz = ISZ_LDOUBLE;
            break;
        case akw_offset:
            sz = ISZ_UINT; /* not differntiating addresses at this level*/
            break;
    };
    inasm_getsym();
    if (MATCHTYPE(lex, l_asmreg))
    {
        if (regimage->regtype == am_ext)
        {
            if (regimage->regnum != akw_ptr)
            {
                inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                return 0;
            }
            inasm_getsym();
        }
    }
    if (!lex || (lex->type != l_asmreg && !ISID(lex) && !MATCHKW(lex, openbr)))
    {
        inasm_err(ERR_ADDRESS_MODE_EXPECTED);
        return 0;
    }
    return sz;
}

/*-------------------------------------------------------------------------*/

static int getscale(int* scale)
{
    if (MATCHKW(lex, star))
    {
        inasm_getsym();
        if (lex)
        {
            if ((MATCHTYPE(lex, l_i) || MATCHTYPE(lex, l_ui)) && !*scale)
            {
                switch ((int)lex->value.i)
                {
                    case 1:
                        *scale = 0;
                        break;
                    case 2:
                        *scale = 1;
                        break;
                    case 4:
                        *scale = 2;
                        break;
                    case 8:
                        *scale = 3;
                        break;
                    default:
                        inasm_err(ERR_INVALID_SCALE_SPECIFIER);
                        *scale = -1;
                        return 1;
                }
                inasm_getsym();
                return 1;
            }
        }
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

int inasm_enterauto(EXPRESSION* node, int* reg1, int* reg2)
{
    if (node && node->type == en_auto)
    {
        int* vreg;
        if (*reg1 >= 0 && *reg2 >= 0)
        {
            inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
            return 0;
        }
        vreg = *reg1 < 0 ? reg1 : reg2;
        *vreg = EBP;
        return 1;
    }
    return 2;
}

/*-------------------------------------------------------------------------*/

static int inasm_structsize(void)
{
    if (basetype(lastsym->tp)->type == bt_struct)
    {
        if (lastsym->tp->size == 6)
            return ISZ_FARPTR;
        if (lastsym->tp->size == 4)
            return ISZ_UINT;
        if (lastsym->tp->size == 1)
            return ISZ_UCHAR;
        if (lastsym->tp->size == 2)
            return ISZ_USHORT;
        return 1000;
    }
    else
    {
        switch (basetype(lastsym->tp)->type)
        {
            case bt_char:
            case bt_unsigned_char:
            case bt_signed_char:
                return ISZ_UCHAR;
            case bt_bool:
                return ISZ_BOOLEAN;
            case bt_short:
            case bt_unsigned_short:
            case bt_char16_t:
                return ISZ_USHORT;
            case bt_wchar_t:
                return ISZ_USHORT;
            case bt_long:
            case bt_unsigned_long:
                return ISZ_UINT;
            case bt_long_long:
            case bt_unsigned_long_long:
                return ISZ_ULONGLONG;
            case bt_int:
            case bt_unsigned:
            case bt_char32_t:
                return ISZ_UINT;
            case bt_enum:
                return ISZ_UINT;
            case bt_pointer:
                return ISZ_UINT;
            case bt_seg:
                return ISZ_USHORT;
            case bt_far:
                return ISZ_FARPTR;
                //        case bt_memberptr:
                //            return ISZ_UINT;
            default:
                return 1000;
        }
    }
}

/*-------------------------------------------------------------------------*/

static AMODE* inasm_mem(void)
{
    int reg1 = -1, reg2 = -1, scale = 0, seg = 0;
    BOOLEAN subtract = FALSE;
    EXPRESSION* node = 0;
    AMODE* rv;
    int gotident = FALSE; /*, autonode = FALSE;*/
    inasm_getsym();
    while (TRUE)
    {
        int rg = -1;
        if (regimage)
            rg = regimage->regnum;
        if (lex)
        {
            switch (lex->type)
            {
                case l_asmreg:
                    if (subtract)
                    {
                        inasm_err(ERR_INVALID_INDEX_MODE);
                        return 0;
                    }
                    if (regimage->regtype == am_seg)
                    {
                        if (seg)
                        {
                            inasm_err(ERR_INVALID_INDEX_MODE);
                            return 0;
                        }
                        seg = rg;
                        inasm_getsym();
                        if (!MATCHKW(lex, colon))
                        {
                            inasm_err(ERR_INVALID_INDEX_MODE);
                            return 0;
                        }
                        inasm_getsym();
                        continue;
                    }
                    if (regimage->regtype != am_dreg || regimage->size != ISZ_UINT)
                    {
                        inasm_err(ERR_INVALID_INDEX_MODE);
                        return 0;
                    }
                    if (reg1 >= 0)
                    {
                        if (reg2 >= 0)
                        {
                            inasm_err(ERR_INVALID_INDEX_MODE);
                            return 0;
                        }
                        reg2 = rg;
                        inasm_getsym();
                        getscale(&scale);
                        if (scale == -1)
                            return 0;
                    }
                    else
                    {
                        inasm_getsym();
                        if (getscale(&scale))
                        {
                            if (scale == -1)
                                return 0;
                            if (reg2 >= 0)
                            {
                                reg1 = reg2;
                            }
                            reg2 = rg;
                        }
                        else
                        {
                            reg1 = rg;
                        }
                    }
                    break;
                case l_wchr:
                case l_achr:
                case l_i:
                case l_ui:
                case l_l:
                case l_ul:
                    if (node)
                        node = exprNode(subtract ? en_sub : en_add, node, intNode(en_c_i, lex->value.i));
                    else if (subtract)
                        node = intNode(en_c_i, -lex->value.i);
                    else
                        node = intNode(en_c_i, lex->value.i);
                    inasm_getsym();
                    break;
                case l_kw:
                    if (MATCHKW(lex, plus) || MATCHKW(lex, minus))
                    {
                        if (node)
                            node = exprNode(en_add, node, intNode(en_c_i, 0));
                        else
                            node = intNode(en_c_i, 0);
                        break;
                    }
                    /* fallthrough */
                default:
                    inasm_err(ERR_INVALID_INDEX_MODE);
                    return 0;
                case l_id:
                    if (gotident || subtract)
                    {
                        inasm_err(ERR_INVALID_INDEX_MODE);
                        return 0;
                    }
                    node = inasm_ident();
                    gotident = TRUE;
                    inasm_structsize();
                    switch (inasm_enterauto(node, &reg1, &reg2))
                    {
                        case 0:
                            return 0;
                        case 1:
                            /*autonode = TRUE;*/
                            break;
                        case 2:
                            /*autonode = FALSE;*/
                            break;
                    }
                    break;
            }
        }
        if (MATCHKW(lex, closebr))
        {
            inasm_getsym();
            break;
        }
        if (!MATCHKW(lex, plus) && !MATCHKW(lex, minus))
        {
            inasm_err(ERR_INVALID_INDEX_MODE);
            return 0;
        }
        if (MATCHKW(lex, minus))
            subtract = TRUE;
        else
            subtract = FALSE;
        inasm_getsym();
    }
    if ((reg2 == 4 || reg2 == 5) && scale > 1)
    {
        inasm_err(ERR_INVALID_INDEX_MODE);
        return 0;
    }
    rv = (AMODE *)beLocalAlloc(sizeof(AMODE));
    if (node)
    {
        rv->offset = node;
    }
    if (reg1 >= 0)
    {
        rv->preg = reg1;
        if (reg2 >= 0)
        {
            rv->sreg = reg2;
            rv->scale = scale;
            rv->mode = am_indispscale;
        }
        else
        {
            rv->mode = am_indisp;
        }
    }
    else if (reg2 >= 0)
    {
        rv->preg = -1;
        rv->sreg = reg2;
        rv->scale = scale;
        rv->mode = am_indispscale;
    }
    else
        rv->mode = am_direct;
    rv->seg = seg;
    return rv;
}

/*-------------------------------------------------------------------------*/

static AMODE* inasm_amode(int nosegreg)
{
    AMODE* rv = (AMODE *)beLocalAlloc(sizeof(AMODE));
    int sz = 0, seg = 0;
    BOOLEAN done = FALSE;
    lastsym = 0;
    inasm_txsym();
    if (lex)
    {
        switch (lex->type)
        {
            case l_wchr:
            case l_achr:
            case l_i:
            case l_ui:
            case l_l:
            case l_ul:
            case l_id:
            case l_asmreg:
                break;
            case l_kw:
                switch (KW(lex))
                {
                    case openbr:
                    case minus:
                    case plus:
                        break;
                    default:
                        inasm_err(ERR_ADDRESS_MODE_EXPECTED);
                        return 0;
                }
                break;
            default:
                inasm_err(ERR_ADDRESS_MODE_EXPECTED);
                return 0;
        }
    }
    if (MATCHTYPE(lex, l_asmreg))
    {
        if (regimage->regtype == am_ext)
        {
            sz = inasm_getsize();
        }
    }
    while (!done)
    {
        done = TRUE;
        if (lex)
        {
            switch (lex->type)
            {
                case l_asmreg:
                    if (regimage->regtype == am_ext)
                    {
                        inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                        return 0;
                    }
                    if (regimage->regtype == am_freg)
                    {
                        inasm_getsym();
                        if (MATCHKW(lex, openpa))
                        {
                            inasm_getsym();
                            if (!lex || (lex->type != l_i && lex->type != l_ui) || lex->value.i < 0 || lex->value.i > 7)
                            {
                                inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                                return 0;
                            }
                            inasm_getsym();
                            inasm_needkw(&lex, closepa);
                        }
                        else
                            lex->value.i = 0;
                        rv->preg = lex->value.i;
                        rv->mode = am_freg;
                        sz = ISZ_LDOUBLE;
                    }
                    else if (regimage->regtype == am_seg)
                    {
                        if (rv->seg)
                        {
                            inasm_err(ERR_TOO_MANY_SEGMENTS);
                            return 0;
                        }
                        rv->seg = seg = regimage->regnum;
                        inasm_getsym();
                        if (MATCHKW(lex, colon))
                        {
                            inasm_getsym();
                            done = FALSE;
                            continue;
                        }
                        rv->mode = am_seg;
                        sz = regimage->size;
                    }
                    else
                    {
                        rv->preg = regimage->regnum;
                        rv->mode = (e_am)regimage->regtype;
                        sz = rv->length = regimage->size;
                        inasm_getsym();
                    }
                    break;
                case l_id:
                    rv->mode = am_immed;
                    rv->offset = inasm_ident();
                    rv->length = ISZ_UINT;
                    if (rv->offset->type == en_auto)
                    {
                        inasm_err(ERR_USE_LEA);
                        return 0;
                    }
                    break;
                case l_kw:
                    switch (KW(lex))
                    {
                        case openbr:
                            rv = inasm_mem();
                            if (rv && rv->seg)
                                seg = rv->seg;
                            break;
                        case minus:
                        case plus:
                            rv = inasm_const();
                            break;
                        default:
                            inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                            return 0;
                    }
                    break;
                case l_i:
                case l_ui:
                case l_l:
                case l_ul:
                case l_wchr:
                case l_achr:
                    rv = inasm_const();
                    break;
                default:
                    inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                    return 0;
            }
        }
    }
    if (rv)
    {
        if (rv->seg)
            if (nosegreg || rv->mode != am_dreg)
                if (rv->mode != am_direct && rv->mode != am_indisp && rv->mode != am_indispscale && rv->mode != am_seg)
                {
                    inasm_err(ERR_TOO_MANY_SEGMENTS);
                    return 0;
                }
        if (!rv->length)
        {
            if (sz)
                rv->length = sz;
            else if (lastsym)
                rv->length = inasm_structsize();
        }
        if (rv->length < 0)
            rv->length = -rv->length;
        rv->seg = seg;
    }
    return rv;
}

/*-------------------------------------------------------------------------*/

static AMODE* inasm_immed(void)
{
    AMODE* rv;
    if (lex)
        switch (lex->type)
        {
            case l_i:
            case l_ui:
            case l_l:
            case l_ul:
            case l_wchr:
            case l_achr:
                rv = aimmed(lex->value.i);
                rv->length = ISZ_UINT;
                inasm_getsym();
                return rv;
            default:
                break;
        }

    return NULL;
}

/*-------------------------------------------------------------------------*/

int isrm(AMODE* ap, int dreg_allowed)
{
    switch (ap->mode)
    {
        case am_dreg:
            return dreg_allowed;
        case am_indisp:
        case am_direct:
        case am_indispscale:
            return 1;
        default:
            return 0;
    }
}

/*-------------------------------------------------------------------------*/

AMODE* getimmed(void)
{
    AMODE* rv;
    if (lex)
        switch (lex->type)
        {
            case l_i:
            case l_ui:
            case l_l:
            case l_ul:
            case l_wchr:
            case l_achr:
                rv = aimmed(lex->value.i);
                inasm_getsym();
                return rv;
            default:
                break;
        }
    return NULL;
}

/*-------------------------------------------------------------------------*/

enum e_opcode inasm_op(void)
{
    int op;
    if (!lex || lex->type != l_asminst)
    {
        inasm_err(ERR_INVALID_OPCODE);
        return (e_opcode)-1;
    }
    op = insdata->atype;
    inasm_getsym();
    floating = op >= op_f2xm1;
    return (e_opcode)op;
}

/*-------------------------------------------------------------------------*/

static OCODE* make_ocode(AMODE* ap1, AMODE* ap2, AMODE* ap3)
{
    OCODE* o = (OCODE *)beLocalAlloc(sizeof(OCODE));
    if (ap1 && (ap1->length == ISZ_UCHAR || ap1->length == -ISZ_UCHAR))
        if (ap2 && ap2->mode == am_immed)
            ap2->length = ap1->length;
    o->oper1 = ap1;
    o->oper2 = ap2;
    o->oper3 = ap3;
    return o;
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_math(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_dreg)
    {
        if (ap2->mode != am_immed && ap2->mode != am_dreg)
            return (OCODE*)-1;
    }
    else if (!isrm(ap2, TRUE) && ap2->mode != am_immed)
        return (OCODE*)-1;
    if (ap2->mode != am_immed)
        if (ap1->length && ap2->length && ap1->length != ap2->length)
            return (OCODE*)-2;

    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_arpl(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_dreg)
        return (OCODE*)-1;
    if (!ap1->length || !ap2->length || ap1->length != ap2->length || ap1->length != ISZ_USHORT)
        return (OCODE*)-2;

    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_bound(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, FALSE))
        return (OCODE*)-1;
    if (ap2->length)
        return (OCODE*)-2;
#ifdef XXXXX
    switch (ap1->length)
    {
        case ISZ_UCHAR:
            return (OCODE*)-1;
        case ISZ_USHORT:
        case ISZ_U16:
            if (ap2->length != ISZ_UINT && ap2->length != ISZ_U32)
            {
                return (OCODE*)-2;
            }
            break;
        case ISZ_UINT:
        case ISZ_U32:
            if (ap2->length != ISZ_ULONGLONG)
            {
                return (OCODE*)-2;
            }
            break;
    }
#endif
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_bitscan(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*)-1;
    if (ap1->length == 1 || ap2->length != ap1->length)
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_bit(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_immed && ap2->mode != am_dreg)
        return (OCODE*)-1;
    if (ap1->length == ISZ_UCHAR || (ap2->mode == am_dreg && ap2->length == ISZ_UCHAR))
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_call(void)
{
    AMODE *ap1 = inasm_amode(TRUE), *ap2;
    if (!ap1)
        return 0;
    if (ap1->mode == am_immed)
    {
        if (ap1->mode == am_immed && MATCHKW(lex, colon))
        {
            inasm_getsym();
            if (cparams.prm_asmfile && prm_assembler != pa_nasm)
                return (OCODE*)-1;
            ap2 = inasm_amode(TRUE);
            if (!ap2)
                return (OCODE*)-1;
            if (ap2->mode != am_immed)
                return (OCODE*)-1;
            ap1->length = ap2->length = ISZ_UINT;
            return make_ocode(ap1, ap2, 0);
        }
        else if ((ap1->offset->type != en_labcon && ap1->offset->type != en_pc) || ap1->seg)
            return (OCODE*)-1;
    }
    else
    {
        if (!isrm(ap1, TRUE))
            return (OCODE*)-1;
        if (ap1->length && (ap1->length != ISZ_UINT) && (ap1->length != ISZ_FARPTR))
            return (OCODE*)-2;
    }
    if ((ap1->mode == am_direct || ap1->mode == am_immed) && ap1->offset->type == en_labcon)
        ap1->length = ISZ_NONE;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_incdec(void)
{
    AMODE* ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    if (ap1->length > ISZ_ULONG)
    {
        return (OCODE*)-2;
    }
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_rm(void) { return (ope_incdec()); }

/*-------------------------------------------------------------------------*/

static OCODE* ope_enter(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_immed();
    if (!ap1)
        return 0;
    inasm_needkw(&lex, comma);
    ap2 = inasm_immed();
    if (!ap2)
        return 0;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_imul(void)
{
    AMODE *ap1 = inasm_amode(TRUE), *ap2 = 0, *ap3 = 0;
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    if (MATCHKW(lex, comma))
    {
        inasm_getsym();
        ap2 = inasm_amode(TRUE);
        if (MATCHKW(lex, comma))
        {
            inasm_getsym();
            ap3 = inasm_amode(TRUE);
        }
    }
    if (ap2)
    {
        if (ap1->mode != am_dreg || ap1->length == ISZ_UCHAR)
            return (OCODE*)-1;
        if (!isrm(ap2, TRUE) && ap2->mode != am_immed)
            return (OCODE*)-1;
        if (ap3)
            if (ap2->mode == am_immed || ap3->mode != am_immed)
                return (OCODE*)-1;
    }
    return make_ocode(ap1, ap2, ap3);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_in(void)
{
    AMODE *ap1 = inasm_amode(TRUE), *ap2;
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg || ap1->preg != 0)
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_immed && (ap2->mode != am_dreg || ap2->preg != 2 || ap2->length != ISZ_USHORT))
        return (OCODE*)-1;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_imm8(void)
{
    AMODE* ap1 = inasm_immed();
    if (!ap1)
        return 0;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_relbra(void)
{
    AMODE* ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    ap1->length = ISZ_NONE;
    if (ap1->mode != am_immed)
        return (OCODE*)-1;
    if (ap1->offset->type != en_labcon)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_relbr8(void) { return ope_relbra(); }

/*-------------------------------------------------------------------------*/

static OCODE* ope_jmp(void) { return ope_call(); }

/*-------------------------------------------------------------------------*/

static OCODE* ope_regrm(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*)-1;
    if (op == op_lea && ap2->mode == am_dreg)
        return (OCODE*)-1;
    if ((ap2->length && ap1->length != ap2->length) || ap1->length == ISZ_UCHAR)
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_loadseg(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*)-1;
    if (ap1->length != ISZ_USHORT || ap2->length != ISZ_USHORT)
        return (OCODE*)-1;
    ap2->length = 0;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_lgdt(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE) || ap1->length != ISZ_FARPTR)
        return (OCODE*)-1;
    ap1->length = 0;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_lidt(void) { return ope_lgdt(); }

/*-------------------------------------------------------------------------*/

static OCODE* ope_rm16(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) || ap1->length != ISZ_USHORT)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_mov(void)
{
    AMODE *ap1 = inasm_amode(TRUE), *ap2;
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) && ap1->mode != am_seg && ap1->mode != am_screg && ap1->mode != am_sdreg && ap1->mode != am_streg)
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode == am_dreg)
    {
        if (!isrm(ap2, TRUE) && ap2->mode != am_immed && ap2->mode != am_seg &&
            (ap2->length != ISZ_UINT || (ap2->mode != am_screg && ap2->mode != am_sdreg && ap2->mode != am_streg)))
            return (OCODE*)-1;
    }
    else if (isrm(ap1, TRUE))
    {
        if (ap2->mode != am_dreg && ap2->mode != am_immed && ap2->mode != am_seg)
            return (OCODE*)-1;
    }
    else if (ap1->mode == am_seg)
    {
        if (!isrm(ap2, TRUE))
            return (OCODE*)-1;
    }
    else if (ap2->length != ISZ_UINT || ap2->mode != am_dreg)
        return (OCODE*)-1;
    if (ap1->length && ap2->length && ap2->mode != am_immed && ap1->length != ap2->length)
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_movsx(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (!isrm(ap2, TRUE))
        return (OCODE*)-1;
    if (!ap2->length || ap1->length <= ap2->length)
    {
        inasm_err(ERR_INVALID_SIZE);
    }
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_out(void)
{
    AMODE *ap1 = inasm_amode(TRUE), *ap2;
    if (!ap1)
        return 0;
    if (ap1->mode != am_immed && (ap1->mode != am_dreg || ap1->preg != 2 || ap1->length != ISZ_USHORT))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_dreg || ap2->preg != 0)
        return (OCODE*)-1;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_pushpop(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) && ap1->mode != am_seg && (ap1->mode != am_immed || (ap1->mode == am_immed && op == op_pop)))
        return (OCODE*)-1;
    if (ap1->mode != am_immed && ap1->length != ISZ_USHORT && ap1->length != ISZ_UINT)
    {
        return (OCODE*)-2;
    }
    if (op == op_pop && ap1->mode == am_seg && ap1->seg == 1)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_shift(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(2);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_immed && ap2->mode != am_dreg)
        return (OCODE*)-1;
    if (ap2->mode == am_dreg)
        if (ap2->preg != 1 || ap2->length != ISZ_UCHAR)
            return (OCODE*)-1;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_ret(void)
{
    AMODE* ap1;
    if (!lex || (lex->type != l_i && lex->type != l_ui))
        return make_ocode(0, 0, 0);
    ap1 = inasm_amode(TRUE);
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_set(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE) || ap1->length != ISZ_UCHAR)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_shld(void)
{
    AMODE *ap1, *ap2, *ap3;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_dreg || ap2->length == ISZ_UCHAR)
        return (OCODE*)-1;
    if (ap1->length && ap1->length != ap2->length)
    {
        inasm_err(ERR_INVALID_SIZE);
    }
    inasm_needkw(&lex, comma);
    ap3 = inasm_amode(TRUE);
    if (!ap3)
        return 0;
    if (ap3->mode != am_immed && ap3->mode != am_dreg)
        return (OCODE*)-1;
    if (ap3->mode == am_dreg)
        if (ap3->preg != 1 || ap3->length != ISZ_UCHAR)
            return (OCODE*)-1;
    return make_ocode(ap1, ap2, ap3);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_test(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_dreg)
        if (ap2->mode != am_dreg && ap2->mode != am_immed)
            return (OCODE*)-1;
    if (ap2->mode == am_dreg && ap1->length && ap1->length != ap2->length)
    {
        return (OCODE*)-2;
    }
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_xchg(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode == am_dreg)
    {
        if (!isrm(ap2, TRUE))
            return (OCODE*)-1;
    }
    else if (ap2->mode != am_dreg)
        return (OCODE*)-1;
    if (ap1->length && ap2->length && ap1->length != ap2->length)
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fmath(void)
{
    AMODE *ap1, *ap2 = 0;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != ISZ_FLOAT && ap1->length != ISZ_DOUBLE)
            return (OCODE*)-2;
    }
    else
    {
        if (ap1->mode != am_freg)
            return (OCODE*)-1;
        if (MATCHKW(lex, comma))
        {
            inasm_getsym();
            ap2 = inasm_amode(TRUE);
            if (ap2->mode != am_freg)
                return (OCODE*)-1;
            if (ap1->preg && ap2->preg)
                return (OCODE*)-1;
        }
    }
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fmathp(void)
{
    AMODE *ap1, *ap2 = 0;
    if (!lex || lex->type != l_asmreg)
        return make_ocode(0, 0, 0);
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_freg)
        return (OCODE*)-1;
    if (MATCHKW(lex, comma))
    {
        inasm_getsym();
        ap2 = inasm_amode(TRUE);
        if (!ap2)
            return 0;
        if (ap2->mode != am_freg)
            return (OCODE*)-1;
        if (ap1->preg && ap2->preg)
            return (OCODE*)-1;
    }
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fmathi(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != ISZ_FLOAT && ap1->length != ISZ_USHORT && ap1->length != ISZ_DOUBLE)
            return (OCODE*)-2;
    }
    else
    {
        return (OCODE*)-1;
    }
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fcom(void)
{
    AMODE* ap1;
    if (!lex || lex->type != l_asmreg)
        return make_ocode(0, 0, 0);
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != ISZ_FLOAT && ap1->length != ISZ_DOUBLE)
            return (OCODE*)-2;
    }
    else
    {
        if (ap1->mode != am_freg)
            return (OCODE*)-1;
    }
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_freg(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_freg)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_ficom(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*)-1;
    if (ap1->length != ISZ_USHORT && ap1->length != ISZ_FLOAT && ap1->length != ISZ_DOUBLE)
        return (OCODE*)-2;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fild(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*)-1;
    if (ap1->length != ISZ_USHORT && ap1->length != ISZ_FLOAT && ap1->length != ISZ_DOUBLE)
        return (OCODE*)-2;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fist(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*)-1;
    if (ap1->length != ISZ_USHORT && ap1->length != ISZ_FLOAT)
        return (OCODE*)-2;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fld(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != ISZ_FLOAT && ap1->length != ISZ_DOUBLE && ap1->length != ISZ_LDOUBLE)
            return (OCODE*)-2;
    }
    else if (ap1->mode != am_freg)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fst(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != ISZ_FLOAT && ap1->length != ISZ_DOUBLE)
            return (OCODE*)-2;
    }
    else if (ap1->mode != am_freg || ap1->preg == 0)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fstp(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (isrm(ap1, FALSE))
    {
        if (ap1->length != ISZ_FLOAT && ap1->length != ISZ_DOUBLE && ap1->length != ISZ_LDOUBLE)
            return (OCODE*)-2;
    }
    else if (ap1->mode != am_freg || ap1->preg == 0)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fucom(void)
{
    AMODE* ap1;
    if (!lex || lex->type != l_asmreg)
        return make_ocode(0, 0, 0);
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_freg || ap1->preg == 0)
        return (OCODE*)-1;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_fxch(void) { return ope_fucom(); }

/*-------------------------------------------------------------------------*/

static OCODE* ope_mn(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, FALSE))
        return (OCODE*)-1;
    ap1->length = ISZ_NONE;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_m16(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (!isrm(ap1, TRUE))
        return (OCODE*)-1;
    if (ap1->mode == am_dreg)
        if (op != op_fstsw && op != op_fnstsw /* &&  op != op_fldsw */ && ap1->preg != 0)
            return (OCODE*)-1;
    if (ap1->length != ISZ_USHORT)
        return (OCODE*)-2;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_cmps(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_indisp || ap2->mode != am_indisp)
        return (OCODE*)-1;
    if (ap1->preg != 6 || ap2->preg != 7)
        return (OCODE*)-1;
    if (ap1->offset || ap2->offset)
        return (OCODE*)-1;
    if (!ap1->seg || ap2->seg != 3)
        return (OCODE*)-1;
    if (!ap1->length && !ap2->length)
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_ins(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap1->mode != am_indisp || ap2->mode != am_dreg)
        return (OCODE*)-1;
    if (ap1->offset)
        return (OCODE*)-1;
    if (ap1->preg != 7 || ap2->preg != 2)
        return (OCODE*)-1;
    if (ap2->seg || ap1->seg != 3)
        return (OCODE*)-1;
    if (ap2->length != ISZ_USHORT || !ap1->length)
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_lods(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap1->offset)
        return (OCODE*)-1;
    if (ap1->preg != 6)
        return (OCODE*)-1;
    if (!ap1->length)
        return (OCODE*)-2;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_movs(void)
{
    AMODE *ap1, *ap2;
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    inasm_needkw(&lex, comma);
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap2->mode != am_indisp)
        return (OCODE*)-1;
    if (ap1->preg != 6 || ap2->preg != 7)
        return (OCODE*)-1;
    if (ap1->offset || ap2->offset)
        return (OCODE*)-1;
    if (!ap1->seg || ap2->seg != 3)
        return (OCODE*)-1;
    if (!ap1->length && !ap2->length)
        return (OCODE*)-2;
    return make_ocode(ap2, ap1, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_outs(void)
{
    AMODE *ap1, *ap2;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    inasm_needkw(&lex, comma);
    ap2 = inasm_amode(TRUE);
    if (!ap2)
        return 0;
    if (ap2->mode != am_indisp || ap1->mode != am_dreg || ap2->offset)
        return (OCODE*)-1;
    if (ap2->preg != 6 || ap1->preg != 2)
        return (OCODE*)-1;
    if (ap1->seg || ap2->seg != 2)
        return (OCODE*)-1;
    if (ap1->length != ISZ_USHORT || !ap2->length)
        return (OCODE*)-2;
    return make_ocode(ap1, ap2, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_scas(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap1->offset)
        return (OCODE*)-1;
    if (ap1->preg != 7)
        return (OCODE*)-1;
    if (ap1->seg != 3)
        return (OCODE*)-1;
    if (!ap1->length)
        return (OCODE*)-2;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_xlat(void)
{
    AMODE* ap1;
    ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_indisp || ap1->offset)
        return (OCODE*)-1;
    if (ap1->preg != 3)
        return (OCODE*)-1;
    if (ap1->length && ap1->length != ISZ_UCHAR)
        return (OCODE*)-2;
    ap1->length = ISZ_UCHAR;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_reg32(void)
{
    AMODE* ap1 = inasm_amode(TRUE);
    if (!ap1)
        return 0;
    if (ap1->mode != am_dreg)
        return (OCODE*)-1;
    if (ap1->length != ISZ_UINT)
        return (OCODE*)-2;
    return make_ocode(ap1, 0, 0);
}

/*-------------------------------------------------------------------------*/

static OCODE* ope_stos(void) { return ope_scas(); }

/*-------------------------------------------------------------------------*/

void* inlineAsmStmt(void* param)
{
    OCODE* rv = (OCODE *)beLocalAlloc(sizeof(OCODE));
    memcpy(rv, param, sizeof(*rv));
    if (rv->opcode != op_label && rv->opcode != op_line)
    {
        AMODE* ap = rv->oper1;
        if (ap && ap->offset)
            ap->offset = inlineexpr(ap->offset, FALSE);
        ap = rv->oper2;
        if (ap && ap->offset)
            ap->offset = inlineexpr(ap->offset, FALSE);
        ap = rv->oper3;
        if (ap && ap->offset)
            ap->offset = inlineexpr(ap->offset, FALSE);
    }
    return rv;
}
OCODE* (*funcs[])(void) = {
    0,          ope_math, ope_arpl,    ope_bound,  ope_bitscan, ope_bit,   ope_call,    ope_incdec, ope_rm,   ope_enter, ope_imul,
    ope_in,     ope_imm8, ope_relbra,  ope_relbr8, ope_jmp,     ope_regrm, ope_loadseg, ope_lgdt,   ope_lidt, ope_rm16,  ope_mov,
    ope_movsx,  ope_out,  ope_pushpop, ope_shift,  ope_ret,     ope_set,   ope_shld,    ope_test,   ope_xchg, ope_fmath, ope_fmathp,
    ope_fmathi, ope_fcom, ope_freg,    ope_ficom,  ope_fild,    ope_fist,  ope_fld,     ope_fst,    ope_fstp, ope_fucom, ope_fxch,
    ope_mn,     ope_m16,  ope_cmps,    ope_ins,    ope_lods,    ope_movs,  ope_outs,    ope_scas,   ope_stos, ope_xlat,  ope_reg32};
static int getData(STATEMENT* snp)
{
    int size = insdata->amode;
    EXPRESSION** newExpr = &snp->select;
    do
    {
        TYPE* tp;
        EXPRESSION* expr;
        lex = getsym();
        lex = optimized_expression(lex, NULL, NULL, &tp, &expr, FALSE);
        if (tp && (isintconst(expr) || isfloatconst(expr)))
        {
            switch (size)
            {
                case ISZ_UCHAR:
                    expr->v.i = reint(expr);
                    expr->type = en_c_uc;
                    break;
                case ISZ_USHORT:
                    expr->v.i = reint(expr);
                    expr->type = en_c_us;
                    break;
                case ISZ_ULONG:
                    if (isintconst(expr))
                    {
                        expr->v.i = reint(expr);
                        expr->type = en_c_ul;
                    }
                    else
                    {
                        expr->v.f = refloat(expr);
                        expr->type = en_c_f;
                    }
                    break;
                case ISZ_ULONGLONG:
                    if (isintconst(expr))
                    {
                        expr->v.i = reint(expr);
                        expr->type = en_c_ull;
                    }
                    else
                    {
                        expr->v.f = refloat(expr);
                        expr->type = en_c_d;
                    }
                    break;
                case ISZ_LDOUBLE:
                    expr->v.f = refloat(expr);
                    expr->type = en_c_ld;
                    break;
            }
            *newExpr = expr;
            newExpr = &(*newExpr)->left;
        }
        else
        {
            error(ERR_CONSTANT_VALUE_EXPECTED);
            lex = SkipToNextLine();
            break;
        }
    } while (lex && MATCHKW(lex, comma));
    return 1;
}
LEXEME* inasm_statement(LEXEME* inlex, BLOCKDATA* parent)
{
    STATEMENT* snp;
    OCODE* rv;
    EXPRESSION* node;
    lastsym = 0;
    lex = inlex; /* patch to not have to rewrite entire module for new frontend */
    inasm_txsym();
    do
    {
        snp = stmtNode(lex, parent, st_passthrough);
        if (!lex)
        {
            return lex;
        }
        if (lex->type != l_asminst)
        {
            if (MATCHKW(lex, kw_int))
            {
                inasm_getsym();
                op = op_int;
                rv = ope_imm8();
                goto join;
            }
            node = inasm_label();
            if (!node)
                return lex;
            if (MATCHKW(lex, semicolon))
                inasm_getsym();

            snp->type = st_label;
            snp->label = node->v.i;
            return lex;
        }
        if (insdata->atype == op_reserved)
        {
            getData(snp);
            return lex;
        }
        op = inasm_op();
        if (op == (enum e_opcode) - 1)
        {

            return lex;
        }
        if (insdata->amode == 0)
        {
            rv = (OCODE *)beLocalAlloc(sizeof(OCODE));
            rv->oper1 = rv->oper2 = rv->oper3 = 0;
        }
        else
        {
            rv = (*funcs[insdata->amode])();
        join:
            if (!rv || rv == (OCODE*)-1 || rv == (OCODE*)-2)
            {
                if (rv == (OCODE*)-1)
                    inasm_err(ERR_ILLEGAL_ADDRESS_MODE);
                if (rv == (OCODE*)-2)
                    inasm_err(ERR_INVALID_SIZE);
                return lex;
            }
        }
        if (rv->oper1 && rv->oper2)
        {
            if (!rv->oper1->length)
                if (!rv->oper2->length)
                {
                    inasm_err(ERR_INVALID_SIZE);
                }
                else
                    rv->oper1->length = rv->oper2->length;
            else if (!rv->oper2->length && insdata->amode != OPE_BOUND && insdata->amode != OPE_LOADSEG)
                rv->oper2->length = rv->oper1->length;
        }
        rv->noopt = TRUE;
        rv->opcode = op;
        rv->fwd = rv->back = 0;
        snp->select = (EXPRESSION*)rv;
        /*
        switch (rv->opcode)
        {
            case op_jecxz:
            case op_ja:
            case op_jnbe:
            case op_jae:
            case op_jnb:
            case op_jnc:
            case op_jb:
            case op_jc:
            case op_jnae:
            case op_jbe:
            case op_jna:
            case op_je:
            case op_jz:
            case op_jg:
            case op_jnle:
            case op_jl:
            case op_jnge:
            case op_jge:
            case op_jnl:
            case op_jle:
            case op_jng:
            case op_jne:
            case op_jnz:
            case op_jo:
            case op_jno:
            case op_jp:
            case op_jnp:
            case op_jpe:
            case op_jpo:
            case op_js:
            case op_jns:
                if (rv->oper1->offset->type == en_labcon)
                {
                    snp = stmtNode(lex, parent, st_asmcond);
                    snp->label =  rv->oper1->offset->v.i;
                }
                break;
            case op_jmp:
                if (rv->oper1->offset->type == en_labcon)
                {
                    snp = stmtNode(lex, parent, st_asmgoto);
                    snp->label =  rv->oper1->offset->v.i;
                }
                break;
        }
        */
    } while (op == op_rep || op == op_repnz || op == op_repz || op == op_repe || op == op_repne || op == op_lock);
    return lex;
}
