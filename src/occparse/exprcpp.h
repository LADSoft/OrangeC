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
namespace Parser
{
    void checkscope(TYPE* tp1, TYPE* tp2);
    EXPRESSION* baseClassOffset(SYMBOL* base, SYMBOL* derived, EXPRESSION* en);
    void qualifyForFunc(SYMBOL* sym, TYPE** tp, bool isMutable);
    void getThisType(SYMBOL* sym, TYPE** tp);
    EXPRESSION* getMemberBase(SYMBOL* memberSym, SYMBOL* strSym, SYMBOL* funcsp, bool toError);
    EXPRESSION* getMemberNode(SYMBOL* memberSym, SYMBOL* strSym, TYPE** tp, SYMBOL* funcsp);
    EXPRESSION* getMemberPtr(SYMBOL* memberSym, SYMBOL* strSym, TYPE** tp, SYMBOL* funcsp);
    bool castToArithmeticInternal(bool integer, TYPE** tp, EXPRESSION** exp, enum e_kw kw, TYPE* other, bool implicit);
    void castToArithmetic(bool integer, TYPE** tp, EXPRESSION** exp, enum e_kw kw, TYPE* other, bool implicit);
    bool castToPointer(TYPE** tp, EXPRESSION** exp, enum e_kw kw, TYPE* other);
    bool cppCast(TYPE* src, TYPE** tp, EXPRESSION** exp);
    EXPRESSION* substitute_params_for_constexpr(EXPRESSION* exp, FUNCTIONCALL* funcparams, HASHTABLE* syms);
    STATEMENT* do_substitute_for_function(STATEMENT* block, FUNCTIONCALL* funcparams, HASHTABLE* syms);
    EXPRESSION* substitute_params_for_function(FUNCTIONCALL* funcparams, HASHTABLE* syms);
    LEXEME* expression_func_type_cast(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags);
    bool doDynamicCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp);
    bool doStaticCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst);
    bool doConstCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp);
    bool doReinterpretCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst);
    LEXEME* GetCastInfo(LEXEME* lex, SYMBOL* funcsp, TYPE** newType, TYPE** oldType, EXPRESSION** oldExp, bool packed);
    LEXEME* expression_typeid(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags);
    bool insertOperatorParams(SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, FUNCTIONCALL* funcparams, int flags);
    bool insertOperatorFunc(enum ovcl cls, enum e_kw kw, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, TYPE* tp1, EXPRESSION* exp1,
        INITLIST* args, int flags);
    LEXEME* expression_new(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, bool global, int flags);
    LEXEME* expression_delete(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, bool global, int flags);
    bool isNoexcept(EXPRESSION* exp);
    LEXEME* expression_noexcept(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp);
    void ResolveTemplateVariable(TYPE** ttype, EXPRESSION** texpr, TYPE* rtype, TYPE* atype);
}