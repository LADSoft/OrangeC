# Software License Agreement
# 
#     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
# 
#     This file is part of the Orange C Compiler package.
# 
#     The Orange C Compiler package is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
# 
#     The Orange C Compiler package is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
# 
#     You should have received a copy of the GNU General Public License
#     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
# 
#     contact information:
#         email: TouchStone222@runbox.com <David Lindauer>
# 
# 

/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

namespace occmsil
{
void include_start(char* name, int num);
void increment_stack(void);
void decrement_stack(void);
Instruction* gen_code(Instruction::iop op, Operand* operand);
void msil_oa_gen_label(int labno);
Operand* make_constant(int sz, Optimizer::SimpleExpression* exp);
bool isauto(Optimizer::SimpleExpression* ep);
void msil_oa_gen_vtt(int dataOffset, Optimizer::SimpleSymbol* func);
void msil_oa_gen_vc1(Optimizer::SimpleSymbol* func);
void msil_oa_gen_importThunk(Optimizer::SimpleSymbol* func);
Operand* getCallOperand(Optimizer::QUAD* q, bool& virt);
Operand* getOperand(Optimizer::IMODE* oper);
void load_ind(Optimizer::IMODE* im);
void store_ind(Optimizer::IMODE* im);
void load_arithmetic_constant(int sz, Operand* operand);
void load_constant(int sz, Optimizer::SimpleExpression* exp);
void gen_load(Optimizer::IMODE* im, Operand* dest, bool retval);
void gen_store(Optimizer::IMODE* im, Operand* dest);
void gen_convert(Operand* dest, Optimizer::IMODE* im, int sz);
void gen_branch(Instruction::iop op, int label, bool decrement);
void asm_expressiontag(Optimizer::QUAD* q);
void asm_tag(Optimizer::QUAD* q);
void asm_line(Optimizer::QUAD* q);
void asm_blockstart(Optimizer::QUAD* q);
void asm_blockend(Optimizer::QUAD* q);
void asm_varstart(Optimizer::QUAD* q);
void asm_func(Optimizer::QUAD* q);
void asm_passthrough(Optimizer::QUAD* q);
void asm_datapassthrough(Optimizer::QUAD* q);
void asm_label(Optimizer::QUAD* q);
void asm_goto(Optimizer::QUAD* q);
BoxedType* boxedType(int isz);
void box(Optimizer::IMODE* im);
void unbox(int isz);
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
void compactgen(Instruction* i, int lab);
void bingen(int lower, int avg, int higher);
void asm_coswitch(Optimizer::QUAD* q);
void asm_swbranch(Optimizer::QUAD* q);
void asm_dc(Optimizer::QUAD* q);
void asm_assnblock(Optimizer::QUAD* q);
void asm_clrblock(Optimizer::QUAD* q);
void asm_initblock(Optimizer::QUAD* q);
void asm_initobj(Optimizer::QUAD* q);
void asm_sizeof(Optimizer::QUAD* q);
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
void asm_pushcontext(Optimizer::QUAD* q);
void asm_popcontext(Optimizer::QUAD* q);
void asm_loadcontext(Optimizer::QUAD* q);
void asm_unloadcontext(Optimizer::QUAD* q);
void asm_tryblock(Optimizer::QUAD* q);
void asm_seh(Optimizer::QUAD* q);
void asm_stackalloc(Optimizer::QUAD* q);
void asm_loadstack(Optimizer::QUAD* q);
void asm_savestack(Optimizer::QUAD* q);
void asm_functail(Optimizer::QUAD* q, int begin, int size);
void asm_atomic(Optimizer::QUAD* q);
void asm_nop(Optimizer::QUAD* q);
}  // namespace occmsil