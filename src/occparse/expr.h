#pragma once
/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, ore
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

extern int argumentNesting;
extern int inAssignRHS;

extern std::list<SYMBOL*> importThunks;
extern int adjustingParams;

void expr_init(void);
void SetRuntimeData(LEXLIST* lex, EXPRESSION* exp, SYMBOL* sym);
void thunkForImportTable(EXPRESSION** exp);
void ValidateMSILFuncPtr(TYPE* dest, TYPE* src, EXPRESSION** exp);
EXPRESSION* exprNode(ExpressionNode type, EXPRESSION* left, EXPRESSION* right);
EXPRESSION* varNode(ExpressionNode type, SYMBOL* sym);
EXPRESSION* typeNode(TYPE* tp);
EXPRESSION* intNode(ExpressionNode type, long long val);
void checkauto(TYPE* tp1, int err);
void GetLogicalDestructors(std::list<EXPRESSION*>**lst, EXPRESSION* cur);
void GetAssignDestructors(std::list<EXPRESSION*>** rv, EXPRESSION* exp);
void checkArgs(FUNCTIONCALL* params, SYMBOL* funcsp);
TYPE* LookupSingleAggregate(TYPE* tp, EXPRESSION** exp, bool memberptr = false);
LEXLIST* getInitList(LEXLIST* lex, SYMBOL* funcsp, std::list<INITLIST*>** owner);
LEXLIST* getArgs(LEXLIST* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, Keyword finish, bool allowPack, int flags);
LEXLIST* getMemberInitializers(LEXLIST* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, Keyword finish, bool allowPack);
EXPRESSION* DerivedToBase(TYPE* tpn, TYPE* tpo, EXPRESSION* exp, int flags);
bool cloneTempStmt(STATEMENT** block, SYMBOL** found, SYMBOL** replace);
TYPE* InitializerListType(TYPE* arg);
EXPRESSION* getFunc(EXPRESSION* exp);
void CreateInitializerList(SYMBOL* func, TYPE* initializerListTemplate, TYPE* initializerListType, std::list<INITLIST*>** lptr, bool operands,
    bool asref);
void AdjustParams(SYMBOL* func, SymbolTable<SYMBOL>::iterator it , SymbolTable<SYMBOL>::iterator itend, std::list<INITLIST*>** lptr, bool operands, bool implicit);
LEXLIST* expression_arguments(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags);
LEXLIST* expression_unary(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXLIST* expression_cast(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXLIST* expression_throw(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp);
LEXLIST* expression_assign(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXLIST* expression_comma(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXLIST* expression_no_comma(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXLIST* expression_no_check(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags);
LEXLIST* expression(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags);

}  // namespace Parser