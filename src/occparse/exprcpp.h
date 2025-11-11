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
// expr group
void qualifyForFunc(SYMBOL* sym, Type** tp, bool isMutable);
void getThisType(SYMBOL* sym, Type** tp);
EXPRESSION* getMemberBase(SYMBOL* memberSym, SYMBOL* strSym, SYMBOL* funcsp, bool toError);
EXPRESSION* getMemberNode(SYMBOL* memberSym, SYMBOL* strSym, Type** tp, SYMBOL* funcsp);
EXPRESSION* getMemberPtr(SYMBOL* memberSym, SYMBOL* strSym, Type** tp, SYMBOL* funcsp);
EXPRESSION* substitute_params_for_constexpr(EXPRESSION* exp, CallSite* funcparams, SymbolTable<SYMBOL>* syms);
Statement* do_substitute_for_function(Statement* block, CallSite* funcparams, SymbolTable<SYMBOL>* syms);
EXPRESSION* substitute_params_for_function(CallSite* funcparams, SymbolTable<SYMBOL>* syms);
void expression_typeid( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags);
void expression_new( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool global, int flags);
void expression_delete( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool global, int flags);
void expression_noexcept( SYMBOL* funcsp, Type** tp, EXPRESSION** exp);

//
EXPRESSION* baseClassOffset(SYMBOL* base, SYMBOL* derived, EXPRESSION* en);
void expression_func_type_cast( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags);
bool insertOperatorParams(SYMBOL* funcsp, Type** tp, EXPRESSION** exp, CallSite* funcparams, int flags);
bool FindOperatorFunction(ovcl cls, Keyword kw, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, Type* tp1, EXPRESSION* exp1,
                          std::list<Argument*>* args, int flags);
void ResolveTemplateVariable(Type** ttype, EXPRESSION** texpr, Type* rtype, Type* atype);
}  // namespace Parser