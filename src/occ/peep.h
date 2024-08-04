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

namespace occx86
{
extern OCODE* peep_tail;

void o_peepini(void);
void oa_adjust_codelab(void* select, int offset);
AMODE* makedregSZ(int r, char size);
AMODE* makedreg(int r);
AMODE* makefreg(int r);
AMODE* copy_addr(AMODE* ap);
OCODE* gen_code(int op, AMODE* ap1, AMODE* ap2);
OCODE* gen_code_sse(int op, AMODE* ap1, AMODE* ap2);
OCODE* gen_code_sse(int ops, int opd, int sz, AMODE* ap1, AMODE* ap2);
OCODE* gen_code_sse_imm(int ops, int opd, int sz, AMODE* ap1, AMODE* ap2, AMODE* ap3);
OCODE* gen_code3(int op, AMODE* ap1, AMODE* ap2, AMODE* ap3);
OCODE* gen_codes(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_coden(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_codefs(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_codef(int op, AMODE* ap1, AMODE* ap2);
void gen_codes2(int op, int len, AMODE* ap1, AMODE* ap2);
void gen_code2(int op, int len1, int len2, AMODE* ap1, AMODE* ap2);
void gen_codelab(Optimizer::SimpleSymbol* lab);
void gen_branch(int op, int label);
void gen_comment(char* txt);
void add_peep(OCODE* newitem);
void oa_gen_label(int labno);
void flush_peep(Optimizer::SimpleSymbol* funcsp, Optimizer::QUAD* list);
void peep_add(OCODE* ip);
void peep_sub(OCODE* ip);
OCODE* peep_test(OCODE* ip);
OCODE* peep_neg(OCODE* ip);
void peep_cmpzx(OCODE* ip);
void peep_cmp(OCODE* ip);
void peep_mov(OCODE* ip);
void peep_movzx(OCODE* ip);
void peep_movzx2(OCODE* ip);
void peep_tworeg(OCODE* ip);
void peep_mathdirect(OCODE* ip);
void peep_xor(OCODE* ip);
void peep_or(OCODE* ip);
void peep_and(OCODE* ip);
void peep_lea(OCODE* ip);
void peep_btr(OCODE* ip);
void peep_sar(OCODE* ip);
void peep_mul(OCODE* ip);
int novalue(OCODE* ip);
int equal_address(AMODE* ap1, AMODE* ap2);
void peep_prefixes(OCODE* ip);
OCODE* peep_div(OCODE* ip);
void peep_push(OCODE* ip);
void peep_call(OCODE* ip);
void peep_fmovs(OCODE* ip);
void peep_fld(OCODE* ip);
void insert_peep_entry(OCODE* after, enum e_opcode opcode, int size, AMODE* ap1, AMODE* ap2);
void remove_peep_entry(OCODE* ip);
void oa_peep(void);
}  // namespace occx86