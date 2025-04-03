/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#pragma once

namespace Parser
{
extern Optimizer::SimpleSymbol* baseThisPtr;

extern int calling_inline;

extern Optimizer::IMODE* inlinereturnap;
extern Optimizer::IMODE* structret_imode;

void iexpr_init(void);
void iexpr_func_init(void);
bool HasIncDec();
void DumpIncDec(SYMBOL* funcsp);
void DumpLogicalDestructors(EXPRESSION* node, SYMBOL* funcsp);
Optimizer::IMODE* LookupExpression(Optimizer::i_ops op, int size, Optimizer::IMODE* left, Optimizer::IMODE* right);
int chksize(int lsize, int rsize);
Optimizer::IMODE* make_ioffset(EXPRESSION* node);
Optimizer::IMODE* make_bf(EXPRESSION* node, Optimizer::IMODE* ap, int size);
Optimizer::IMODE* gen_deref(EXPRESSION* node, SYMBOL* funcsp, int flags);
Optimizer::IMODE* gen_unary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op);
Optimizer::IMODE* gen_binary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op);
Optimizer::IMODE* gen_udivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op, bool mod);
Optimizer::IMODE* gen_sdivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op, bool mod);
Optimizer::IMODE* gen_pdiv(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
Optimizer::IMODE* gen_pmul(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
Optimizer::IMODE* gen_hook(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
Optimizer::IMODE* gen_moveblock(EXPRESSION* node, SYMBOL* funcsp);
Optimizer::IMODE* gen_clearblock(EXPRESSION* node, SYMBOL* funcsp);
Optimizer::IMODE* gen_cpinitblock(EXPRESSION* node, SYMBOL* funcsp, bool cp, int flags);
Optimizer::IMODE* gen_assign(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
Optimizer::IMODE* gen_aincdec(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, Optimizer::i_ops op);
int push_param(EXPRESSION* ep, SYMBOL* funcsp, bool vararg, EXPRESSION* valist, int flags);
Optimizer::IMODE* gen_trapcall(SYMBOL* funcsp, EXPRESSION* node, int flags);
Optimizer::IMODE* gen_stmt_from_expr(SYMBOL* funcsp, EXPRESSION* node, int flags);
Optimizer::IMODE* gen_funccall(SYMBOL* funcsp, EXPRESSION* node, int flags);
Optimizer::IMODE* gen_atomic_barrier(SYMBOL* funcsp, ATOMICDATA* ad, Optimizer::IMODE* addr, Optimizer::IMODE* barrier);
Optimizer::IMODE* gen_atomic(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
Optimizer::IMODE* gen_expr(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
Optimizer::IMODE* gen_comma_(EXPRESSION* node, SYMBOL* funcsp);
int natural_size(EXPRESSION* node);
void gen_compare(EXPRESSION* node, SYMBOL* funcsp, Optimizer::i_ops btype, int label);
Optimizer::IMODE* gen_set(EXPRESSION* node, SYMBOL* funcsp, Optimizer::i_ops btype);
Optimizer::IMODE* defcond(EXPRESSION* node, SYMBOL* funcsp);
Optimizer::IMODE* gen_relat(EXPRESSION* node, SYMBOL* funcsp);
void truejmp(EXPRESSION* node, SYMBOL* funcsp, int label);
void falsejmp(EXPRESSION* node, SYMBOL* funcsp, int label);
}  // namespace Parser