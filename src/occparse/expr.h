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
extern std::list<StringData*> strtab;

void expr_init(void);
void thunkForImportTable(EXPRESSION** exp);
void ValidateMSILFuncPtr(Type* dest, Type* src, EXPRESSION** exp);
EXPRESSION* MakeExpression(ExpressionNode type, EXPRESSION* left = nullptr, EXPRESSION* right = nullptr);
EXPRESSION* MakeExpression(ExpressionNode type, SYMBOL* sym);
EXPRESSION* MakeExpression(Type* tp);
EXPRESSION* MakeIntExpression(ExpressionNode type, long long val);
EXPRESSION* MakeExpression(CallSite* callSite);
EXPRESSION* stringlit(StringData* s);
EXPRESSION* ConvertToMSILString(EXPRESSION* val);
void GetLogicalDestructors(std::list<EXPRESSION*>**lst, EXPRESSION* cur);
void GetAssignDestructors(std::list<EXPRESSION*>** rv, EXPRESSION* exp);
Type* LookupSingleAggregate(Type* tp, EXPRESSION** exp, bool memberptr = false);
LexList* getInitList(LexList* lex, SYMBOL* funcsp, std::list<Argument*>** owner);
LexList* getArgs(LexList* lex, SYMBOL* funcsp, CallSite* funcparams, Keyword finish, bool allowPack, int flags);
LexList* getMemberInitializers(LexList* lex, SYMBOL* funcsp, CallSite* funcparams, Keyword finish, bool allowPack);
void CreateInitializerList(SYMBOL* func, Type* initializerListTemplate, Type* initializerListType, std::list<Argument*>** lptr, bool operands,
    bool asref);
void PromoteConstructorArgs(SYMBOL* cons1, CallSite* params);
void AdjustParams(SYMBOL* func, SymbolTable<SYMBOL>::iterator it, SymbolTable<SYMBOL>::iterator itend, std::list<Argument*>** lptr, bool operands, bool implicit);
LexList* expression_assign(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
LexList* expression_comma(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
LexList* expression_no_comma(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
LexList* expression_no_check(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags);
LexList* expression(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags);
#if 1
// expressions
EXPRESSION* DerivedToBase(Type* tpn, Type* tpo, EXPRESSION* exp, int flags);
LexList* expression_arguments(LexList* lex, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags);
LexList* expression_cast(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
#endif

}  // namespace Parser