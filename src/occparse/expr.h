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
extern int packIndex;

extern int argument_nesting;

extern Optimizer::LIST* importThunks;

void expr_init(void);
void thunkForImportTable(EXPRESSION** exp);
void ValidateMSILFuncPtr(TYPE* dest, TYPE* src, EXPRESSION** exp);
EXPRESSION* exprNode(enum e_node type, EXPRESSION* left, EXPRESSION* right);
EXPRESSION* varNode(enum e_node type, SYMBOL* sym);
EXPRESSION* typeNode(TYPE* tp);
EXPRESSION* intNode(enum e_node type, long long val);
void checkauto(TYPE* tp1, int err);
void checkArgs(FUNCTIONCALL* params, SYMBOL* funcsp);
LEXEME* getInitList(LEXEME* lex, SYMBOL* funcsp, INITLIST** owner);
LEXEME* getArgs(LEXEME* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, enum e_kw finish, bool allowPack, int flags);
LEXEME* getMemberInitializers(LEXEME* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, enum e_kw finish, bool allowPack);
EXPRESSION* DerivedToBase(TYPE* tpn, TYPE* tpo, EXPRESSION* exp, int flags);
bool cloneTempStmt(STATEMENT** block, SYMBOL** found, SYMBOL** replace);
void CreateInitializerList(TYPE* initializerListTemplate, TYPE* initializerListType, INITLIST** lptr, bool operands, bool asref);
void AdjustParams(SYMBOL* func, SYMLIST* hr, INITLIST** lptr, bool operands, bool implicit);
LEXEME* expression_arguments(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags);
LEXEME* expression_unary(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXEME* expression_cast(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
void GetLogicalDestructors(EXPRESSION* top, EXPRESSION* cur);
LEXEME* expression_throw(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp);
LEXEME* expression_assign(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXEME* expression_comma(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXEME* expression_no_comma(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXEME* expression_no_check(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags);
LEXEME* expression(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags);

}  // namespace Parser