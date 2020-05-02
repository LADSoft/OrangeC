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

void include_start(char* name, int num);
void increment_stack(void);
void decrement_stack(void);
Instruction* gen_code(Instruction::iop op, Operand* operand);
void msil_oa_gen_label(int labno);
Operand* make_constant(int sz, SimpleExpression* exp);
bool isauto(SimpleExpression* ep);
void msil_oa_gen_vtt(int dataOffset, SimpleSymbol* func);
void msil_oa_gen_vc1(SimpleSymbol* func);
void msil_oa_gen_importThunk(SimpleSymbol* func);
Operand* getCallOperand(QUAD* q, bool& virt);
Operand* getOperand(IMODE* oper);
void load_ind(IMODE* im);
void store_ind(IMODE* im);
void load_arithmetic_constant(int sz, Operand* operand);
void load_constant(int sz, SimpleExpression* exp);
void gen_load(IMODE* im, Operand* dest, bool retval);
void gen_store(IMODE* im, Operand* dest);
void gen_convert(Operand* dest, IMODE* im, int sz);
void gen_branch(Instruction::iop op, int label, bool decrement);
void asm_expressiontag(QUAD* q);
void asm_tag(QUAD* q);
void asm_line(QUAD* q);
void asm_blockstart(QUAD* q);
void asm_blockend(QUAD* q);
void asm_varstart(QUAD* q);
void asm_func(QUAD* q);
void asm_passthrough(QUAD* q);
void asm_datapassthrough(QUAD* q);
void asm_label(QUAD* q);
void asm_goto(QUAD* q);
BoxedType* boxedType(int isz);
void box(IMODE* im);
void unbox(int isz);
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
void compactgen(Instruction* i, int lab);
void bingen(int lower, int avg, int higher);
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
void asm_seh(QUAD* q);
void asm_stackalloc(QUAD* q);
void asm_loadstack(QUAD* q);
void asm_savestack(QUAD* q);
void asm_functail(QUAD* q, int begin, int size);
void asm_atomic(QUAD* q);

