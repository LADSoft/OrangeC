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

#include <stack>
namespace Parser
{
extern int noNeedToSpecialize;

// decl group
void deferredInitializeStructFunctions(SYMBOL* cur);
void deferredInitializeStructMembers(SYMBOL* cur);
void baseClasses( SYMBOL* funcsp, SYMBOL* declsym, AccessLevel defaultAccess);
void checkOperatorArgs(SYMBOL* sp, bool asFriend);
void CheckIsLiteralClass(Type* tp);
void GetStructuredBinding( SYMBOL* funcsp, StorageClass storage_class, Linkage linkage,
                              std::list<FunctionBlock*>& block);
// templatedecl.cpp
bool declaringTemplate(SYMBOL* sym);
// cpplookup.cpp
void CollapseReferences(Type* tp_in);
// stmt.cpp
EXPRESSION* addLocalDestructor(EXPRESSION* exp, SYMBOL* decl);
//
int classRefCount(SYMBOL* base, SYMBOL* derived);
void CheckCalledException(SYMBOL* cst, EXPRESSION* exp);
void DeferredCompileFunction(SYMBOL* cur);
void deferredInitializeDefaultArg(SYMBOL* arg, SYMBOL* func);
bool MatchesConstFunction(SYMBOL* sym);
void getDeclType( SYMBOL* funcsp, Type** tn);
}  // namespace Parser