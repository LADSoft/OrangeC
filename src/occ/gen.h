/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#pragma once

namespace occx86
{
extern int pushlevel;
extern int funcstackheight;

extern AMODE *singleLabel, *doubleLabel, *zerolabel;

Optimizer::SimpleExpression* copy_expression(Optimizer::SimpleExpression* node);
AMODE* make_muldivval(AMODE* ap);
AMODE* make_label(int lab);
AMODE* makesegreg(int seg);
AMODE* makeSSE(int reg);
void make_floatconst(AMODE* ap, int sz);
AMODE* moveFP(AMODE* apa, int sza, AMODE* apl, int szl);
AMODE* aimmed(unsigned long long i);
AMODE* aimmedt(long i, int size);
bool isauto(Optimizer::SimpleExpression* ep);
AMODE* make_offset(Optimizer::SimpleExpression* node);
AMODE* make_stack(int number);
AMODE* setSymbol(const char* name);
void oa_gen_vtt(int dataOffset, Optimizer::SimpleSymbol* func);
void oa_gen_vc1(Optimizer::SimpleSymbol* func);
void oa_gen_importThunk(Optimizer::SimpleSymbol* func);
void make_complexconst(AMODE* ap, AMODE* api);
void floatchs(AMODE* ap, int sz);
void zerocleanup(void);
AMODE* floatzero(AMODE* ap);
int beRegFromTempInd(Optimizer::QUAD* q, Optimizer::IMODE* im, int which);
int beRegFromTemp(Optimizer::QUAD* q, Optimizer::IMODE* im);
bool sameTemp(Optimizer::QUAD* head);
int imaginary_offset(int sz);
int samereg(AMODE* ap1, AMODE* ap2);
void getAmodes(Optimizer::QUAD* q, enum e_opcode* op, Optimizer::IMODE* im, AMODE** apl, AMODE** aph);
void bit_store(AMODE* dest, AMODE* src, int size, int bits, int startbit);
void liveQualify(AMODE* reg, AMODE* left, AMODE* right);
bool overlaps(AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh);
void func_axdx(enum e_opcode func, AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh);
void func_axdx_mov(enum e_opcode op, AMODE* apal, AMODE* apah, AMODE* apll, AMODE* aplh, AMODE* aprl, AMODE* aprh);
void gen_lshift(enum e_opcode op, AMODE* aph, AMODE* apl, AMODE* n);
void gen_xset(Optimizer::QUAD* q, enum e_opcode pos, enum e_opcode neg, enum e_opcode flt);
void gen_goto(Optimizer::QUAD* q, enum e_opcode pos, enum e_opcode neg, enum e_opcode llpos, enum e_opcode llneg,
              enum e_opcode llintermpos, enum e_opcode llintermneg, enum e_opcode flt);
void bingen(int lower, int avg, int higher);
int getPushMask(int i);
void asm_line(Optimizer::QUAD* q);
void asm_blockstart(Optimizer::QUAD* q);
void asm_blockend(Optimizer::QUAD* q);
void asm_varstart(Optimizer::QUAD* q);
void asm_func(Optimizer::QUAD* q);
void asm_passthrough(Optimizer::QUAD* q);
void asm_datapassthrough(Optimizer::QUAD* q);
void asm_label(Optimizer::QUAD* q);
void asm_goto(Optimizer::QUAD* q);
void asm_parm(Optimizer::QUAD* q);
void asm_parmblock(Optimizer::QUAD* q);
void asm_parmadj(Optimizer::QUAD* q);
void asm_gosub(Optimizer::QUAD* q);
void asm_fargosub(Optimizer::QUAD* q);
void asm_trap(Optimizer::QUAD* q);
void asm_int(Optimizer::QUAD* q);
void asm_ret(Optimizer::QUAD* q);
void asm_fret(Optimizer::QUAD* q);
void asm_rett(Optimizer::QUAD* q);
void asm_add(Optimizer::QUAD* q);
void asm_sub(Optimizer::QUAD* q);
void asm_udiv(Optimizer::QUAD* q);
void asm_umod(Optimizer::QUAD* q);
void asm_sdiv(Optimizer::QUAD* q);
void asm_smod(Optimizer::QUAD* q);
void asm_muluh(Optimizer::QUAD* q);
void asm_mulsh(Optimizer::QUAD* q);
void asm_mul(Optimizer::QUAD* q);
void asm_lsr(Optimizer::QUAD* q);
void asm_lsl(Optimizer::QUAD* q);
void asm_asr(Optimizer::QUAD* q);
void asm_neg(Optimizer::QUAD* q);
void asm_not(Optimizer::QUAD* q);
void asm_and(Optimizer::QUAD* q);
void asm_or(Optimizer::QUAD* q);
void asm_eor(Optimizer::QUAD* q);
void asm_setne(Optimizer::QUAD* q);
void asm_sete(Optimizer::QUAD* q);
void asm_setc(Optimizer::QUAD* q);
void asm_seta(Optimizer::QUAD* q);
void asm_setnc(Optimizer::QUAD* q);
void asm_setbe(Optimizer::QUAD* q);
void asm_setl(Optimizer::QUAD* q);
void asm_setg(Optimizer::QUAD* q);
void asm_setle(Optimizer::QUAD* q);
void asm_setge(Optimizer::QUAD* q);
void asm_assn(Optimizer::QUAD* q);
void asm_genword(Optimizer::QUAD* q);
void asm_coswitch(Optimizer::QUAD* q);
void asm_swbranch(Optimizer::QUAD* q);
void asm_dc(Optimizer::QUAD* q);
void asm_assnblock(Optimizer::QUAD* q);
void asm_clrblock(Optimizer::QUAD* q);
void asm_cmpblock(Optimizer::QUAD* q);
void asm_jc(Optimizer::QUAD* q);
void asm_ja(Optimizer::QUAD* q);
void asm_je(Optimizer::QUAD* q);
void asm_jnc(Optimizer::QUAD* q);
void asm_jbe(Optimizer::QUAD* q);
void asm_jne(Optimizer::QUAD* q);
void asm_jl(Optimizer::QUAD* q);
void asm_jg(Optimizer::QUAD* q);
void asm_jle(Optimizer::QUAD* q);
void asm_jge(Optimizer::QUAD* q);
void asm_cppini(Optimizer::QUAD* q);
void asm_prologue(Optimizer::QUAD* q);
void asm_epilogue(Optimizer::QUAD* q);
void asm_excbegin(Optimizer::QUAD* q);
void asm_excend(Optimizer::QUAD* q);
void asm_pushcontext(Optimizer::QUAD* q);
void asm_popcontext(Optimizer::QUAD* q);
void asm_loadcontext(Optimizer::QUAD* q);
void asm_unloadcontext(Optimizer::QUAD* q);
void asm_tryblock(Optimizer::QUAD* q);
void asm_stackalloc(Optimizer::QUAD* q);
void asm_loadstack(Optimizer::QUAD* q);
void asm_savestack(Optimizer::QUAD* q);
void asm_functail(Optimizer::QUAD* q, int begin, int size);
void asm_atomic(Optimizer::QUAD* q);
void asm_expressiontag(Optimizer::QUAD* q);
void asm_seh(Optimizer::QUAD* q);
void asm_tag(Optimizer::QUAD* q);

}  // namespace occx86