#pragma once
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

namespace Parser
{
void checkscope(Type* tp1, Type* tp2);
EXPRESSION* baseClassOffset(SYMBOL* base, SYMBOL* derived, EXPRESSION* en);
void qualifyForFunc(SYMBOL* sym, Type** tp, bool isMutable);
void getThisType(SYMBOL* sym, Type** tp);
EXPRESSION* getMemberBase(SYMBOL* memberSym, SYMBOL* strSym, SYMBOL* funcsp, bool toError);
EXPRESSION* getMemberNode(SYMBOL* memberSym, SYMBOL* strSym, Type** tp, SYMBOL* funcsp);
EXPRESSION* getMemberPtr(SYMBOL* memberSym, SYMBOL* strSym, Type** tp, SYMBOL* funcsp);
bool castToArithmeticInternal(bool integer, Type** tp, EXPRESSION** exp, Keyword kw, Type* other, bool implicit);
void castToArithmetic(bool integer, Type** tp, EXPRESSION** exp, Keyword kw, Type* other, bool implicit);
bool castToPointer(Type** tp, EXPRESSION** exp, Keyword kw, Type* other);
bool cppCast(Type* src, Type** tp, EXPRESSION** exp);
EXPRESSION* substitute_params_for_constexpr(EXPRESSION* exp, FUNCTIONCALL* funcparams, SymbolTable<SYMBOL>* syms);
STATEMENT* do_substitute_for_function(STATEMENT* block, FUNCTIONCALL* funcparams, SymbolTable<SYMBOL>* syms);
EXPRESSION* substitute_params_for_function(FUNCTIONCALL* funcparams, SymbolTable<SYMBOL>* syms);
LEXLIST* expression_func_type_cast(LEXLIST* lex, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags);
bool doDynamicCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp);
bool doStaticCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst);
bool doConstCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp);
bool doReinterpretCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst);
LEXLIST* GetCastInfo(LEXLIST* lex, SYMBOL* funcsp, Type** newType, Type** oldType, EXPRESSION** oldExp, bool packed);
LEXLIST* expression_typeid(LEXLIST* lex, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags);
bool insertOperatorParams(SYMBOL* funcsp, Type** tp, EXPRESSION** exp, FUNCTIONCALL* funcparams, int flags);
bool insertOperatorFunc(ovcl cls, Keyword kw, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, Type* tp1, EXPRESSION* exp1,
    std::list<INITLIST*>* args, int flags);
LEXLIST* expression_new(LEXLIST* lex, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool global, int flags);
LEXLIST* expression_delete(LEXLIST* lex, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool global, int flags);
bool isNoexcept(EXPRESSION* exp);
LEXLIST* expression_noexcept(LEXLIST* lex, SYMBOL* funcsp, Type** tp, EXPRESSION** exp);
void ResolveTemplateVariable(Type** ttype, EXPRESSION** texpr, Type* rtype, Type* atype);
}  // namespace Parser