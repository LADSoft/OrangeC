#pragma once
/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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
extern int pushlevel;
extern int funcstackheight;

extern AMODE *singleLabel, *doubleLabel, *zerolabel;

SimpleExpression* copy_expression(SimpleExpression* node);
AMODE* make_muldivval(AMODE* ap);
AMODE* make_label(int lab);
AMODE* makesegreg(int seg);
AMODE* makeSSE(int reg);
void make_floatconst(AMODE* ap);
AMODE* moveFP(AMODE* apa, int sza, AMODE* apl, int szl);
AMODE* aimmed(unsigned long long i);
AMODE* aimmedt(long i, int size);
bool isauto(SimpleExpression* ep);
AMODE* make_offset(SimpleExpression* node);
AMODE* make_stack(int number);
AMODE* setSymbol(const char* name);
void oa_gen_vtt(int dataOffset, SimpleSymbol* func);
void oa_gen_vc1(SimpleSymbol* func);
void oa_gen_importThunk(SimpleSymbol* func);
void make_complexconst(AMODE* ap, AMODE* api);
void floatchs(AMODE* ap, int sz);
void zerocleanup(void);
AMODE* floatzero(AMODE* ap);
int beRegFromTempInd(QUAD* q, IMODE* im, int which);
int beRegFromTemp(QUAD* q, IMODE* im);
bool sameTemp(QUAD* head);
int imaginary_offset(int sz);
int samereg(AMODE* ap1, AMODE* ap2);
void getAmodes(QUAD* q, enum e_opcode* op, IMODE* im, AMODE** apl, AMODE** aph);
void bit_store(AMODE* dest, AMODE* src, int size, int bits, int startbit);
void liveQualify(AMODE* reg, AMODE* left, AMODE* right);
bool overlaps(AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh);
void func_axdx(enum e_opcode func, AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh);
void func_axdx_mov(enum e_opcode op, AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh, AMODE* aprl, AMODE* aprh);
void gen_lshift(enum e_opcode op, AMODE* aph, AMODE* apl, AMODE* n);
void gen_xset(QUAD* q, enum e_opcode pos, enum e_opcode neg, enum e_opcode flt);
void gen_goto(QUAD* q, enum e_opcode pos, enum e_opcode neg, enum e_opcode llpos, enum e_opcode llneg, enum e_opcode llintermpos,
    enum e_opcode llintermneg, enum e_opcode flt);
void bingen(int lower, int avg, int higher);
int getPushMask(int i);
void asm_line(QUAD* q);
void asm_blockstart(QUAD* q);
void asm_blockend(QUAD* q);
void asm_varstart(QUAD* q);
void asm_func(QUAD* q);
void asm_passthrough(QUAD* q);
void asm_datapassthrough(QUAD* q);
void asm_label(QUAD* q);
void asm_goto(QUAD* q);
void asm_parm(QUAD* q);
void asm_parmblock(QUAD* q);
void asm_parmadj(QUAD* q);
void asm_gosub(QUAD* q);
void asm_fargosub(QUAD* q);
void asm_trap(QUAD* q);
void asm_int(QUAD* q);
void asm_ret(QUAD* q);
void asm_fret(QUAD* q);
void asm_rett(QUAD* q);
void asm_add(QUAD* q);
void asm_sub(QUAD* q);
void asm_udiv(QUAD* q);
void asm_umod(QUAD* q);
void asm_sdiv(QUAD* q);
void asm_smod(QUAD* q);
void asm_muluh(QUAD* q);
void asm_mulsh(QUAD* q);
void asm_mul(QUAD* q);
void asm_lsr(QUAD* q);
void asm_lsl(QUAD* q);
void asm_asr(QUAD* q);
void asm_neg(QUAD* q);
void asm_not(QUAD* q);
void asm_and(QUAD* q);
void asm_or(QUAD* q);
void asm_eor(QUAD* q);
void asm_setne(QUAD* q);
void asm_sete(QUAD* q);
void asm_setc(QUAD* q);
void asm_seta(QUAD* q);
void asm_setnc(QUAD* q);
void asm_setbe(QUAD* q);
void asm_setl(QUAD* q);
void asm_setg(QUAD* q);
void asm_setle(QUAD* q);
void asm_setge(QUAD* q);
void asm_assn(QUAD* q);
void asm_genword(QUAD* q);
void asm_coswitch(QUAD* q);
void asm_swbranch(QUAD* q);
void asm_dc(QUAD* q);
void asm_assnblock(QUAD* q);
void asm_clrblock(QUAD* q);
void asm_cmpblock(QUAD* q);
void asm_jc(QUAD* q);
void asm_ja(QUAD* q);
void asm_je(QUAD* q);
void asm_jnc(QUAD* q);
void asm_jbe(QUAD* q);
void asm_jne(QUAD* q);
void asm_jl(QUAD* q);
void asm_jg(QUAD* q);
void asm_jle(QUAD* q);
void asm_jge(QUAD* q);
void asm_cppini(QUAD* q);
void asm_prologue(QUAD* q);
void asm_epilogue(QUAD* q);
void asm_pushcontext(QUAD* q);
void asm_popcontext(QUAD* q);
void asm_loadcontext(QUAD* q);
void asm_unloadcontext(QUAD* q);
void asm_tryblock(QUAD* q);
void asm_stackalloc(QUAD* q);
void asm_loadstack(QUAD* q);
void asm_savestack(QUAD* q);
void asm_functail(QUAD* q, int begin, int size);
void asm_atomic(QUAD* q);
void asm_expressiontag(QUAD* q);
void asm_seh(QUAD* q);
void asm_tag(QUAD* q);

