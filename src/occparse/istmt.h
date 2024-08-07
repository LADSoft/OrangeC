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

extern Optimizer::SimpleSymbol* currentFunction;

namespace Parser
{
extern Optimizer::IMODE* returnImode;
extern int retcount;
extern int consIndex;
extern EXPRESSION* xcexp;
extern int catchLevel;
extern int codeLabelOffset;

void genstmtini(void);
Optimizer::QUAD* gen_xcexp_expression(int n);
Optimizer::IMODE* imake_label(int label);
Optimizer::IMODE* make_direct(int i);
void gen_genword(Statement* stmt, SYMBOL* funcsp);
Optimizer::IMODE* set_symbol(const char* name, int isproc);
Optimizer::IMODE* call_library(const char* lib_name, int size);
void SubProfilerData(void);
EXPRESSION* tempVar(Type* tp, bool global = false);
EXPRESSION *makeParamSubs(EXPRESSION* left, Optimizer::IMODE* im);
int gcs_compare(void const* left, void const* right);
void genxswitch(Statement* stmt, SYMBOL* funcsp);
void genselect(Statement* stmt, SYMBOL* funcsp, bool jmptrue);
void genreturn(Statement* stmt, SYMBOL* funcsp, int flags, Optimizer::IMODE* allocaAP);
void gen_varstart(void* exp);
void gen_func(void* exp, int start);
void gen_dbgblock(int start);
void gen_asm(Statement* stmt);
void gen_asmdata(Statement* stmt);
Optimizer::IMODE* genstmt(std::list<Statement*>* stmt, SYMBOL* funcsp, int flags);
void CopyVariables(SYMBOL* funcsp);
void genfunc(SYMBOL* funcsp, bool doOptimize);
void genASM(std::list<Statement*>* st);

}  // namespace Parser