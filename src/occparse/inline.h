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

void inlineinit(void);
void dumpInlines(void);
void dumpImportThunks(void);
void dumpvc1Thunks(void);
void dumpMemberPointers();

SYMBOL* getvc1Thunk(int offset);
bool CompileInlineFunction(SYMBOL* sym);
bool CompileAndGen(SYMBOL* sym);
void EnterInlineFunctionContext();
void LeaveInlineFunctionContext();
void InsertInline(SYMBOL* sym);
void InsertInlineData(SYMBOL* sym);
void InsertRttiType(SYMBOL* tp);
void InsertMemberPointer(int label, Optimizer::SimpleSymbol* sym, int offset1, int offset2);
EXPRESSION* inlineexpr(EXPRESSION* node, bool* fromlval);
void InsertLocalStaticUnInitializer(SYMBOL* func, EXPRESSION* body);
std::list<Statement*>* inlinestmt(std::list<Statement*>* blocks);
void SetupVariables(SYMBOL* sym);
EXPRESSION* doinline(CallSite* params, SYMBOL* funcsp);
bool IsEmptyFunction(CallSite* params, SYMBOL* funcsp);

}  // namespace Parser