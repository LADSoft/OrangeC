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
extern SimpleSymbol* baseThisPtr;

extern int calling_inline;

extern IMODE* inlinereturnap;
extern IMODE* structret_imode;

extern SYMBOL* inlinesp_list[MAX_INLINE_NESTING];
extern int inlinesp_count;

void iexpr_init(void);
void iexpr_func_init(void);
void DumpIncDec(SYMBOL* funcsp);
void DumpLogicalDestructors(EXPRESSION* node, SYMBOL* funcsp);
IMODE* LookupExpression(enum i_ops op, int size, IMODE* left, IMODE* right);
int chksize(int lsize, int rsize);
IMODE* make_imaddress(EXPRESSION* node, int size);
IMODE* make_ioffset(EXPRESSION* node);
IMODE* make_bf(EXPRESSION* node, IMODE* ap, int size);
IMODE* gen_deref(EXPRESSION* node, SYMBOL* funcsp, int flags);
IMODE* gen_unary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
IMODE* gen_asrhd(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
IMODE* gen_binary(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
IMODE* gen_udivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op, bool mod);
IMODE* gen_sdivide(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op, bool mod);
IMODE* gen_pdiv(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_pmul(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_hook(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_moveblock(EXPRESSION* node, SYMBOL* funcsp);
IMODE* gen_clearblock(EXPRESSION* node, SYMBOL* funcsp);
IMODE* gen_cpinitblock(EXPRESSION* node, SYMBOL* funcsp, bool cp, int flags);
IMODE* gen_assign(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_aincdec(SYMBOL* funcsp, EXPRESSION* node, int flags, int size, enum i_ops op);
int push_param(EXPRESSION* ep, SYMBOL* funcsp, bool vararg, EXPRESSION* valist, int flags);
IMODE* gen_trapcall(SYMBOL* funcsp, EXPRESSION* node, int flags);
IMODE* gen_stmt_from_expr(SYMBOL* funcsp, EXPRESSION* node, int flags);
IMODE* gen_funccall(SYMBOL* funcsp, EXPRESSION* node, int flags);
IMODE* gen_atomic_barrier(SYMBOL* funcsp, ATOMICDATA* ad, IMODE* addr, IMODE* barrier);
IMODE* gen_atomic(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_expr(SYMBOL* funcsp, EXPRESSION* node, int flags, int size);
IMODE* gen_void(EXPRESSION* node, SYMBOL* funcsp);
int natural_size(EXPRESSION* node);
void gen_compare(EXPRESSION* node, SYMBOL* funcsp, i_ops btype, int label);
IMODE* gen_set(EXPRESSION* node, SYMBOL* funcsp, i_ops btype);
IMODE* defcond(EXPRESSION* node, SYMBOL* funcsp);
IMODE* gen_relat(EXPRESSION* node, SYMBOL* funcsp);
void truejp(EXPRESSION* node, SYMBOL* funcsp, int label);
void falsejp(EXPRESSION* node, SYMBOL* funcsp, int label);
