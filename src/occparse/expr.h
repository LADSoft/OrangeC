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

extern int argumentNestingLevel;
extern int assigningRHS;

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
void GetLogicalDestructors(std::list<EXPRESSION*>** lst, EXPRESSION* cur);
void GetAssignDestructors(std::list<EXPRESSION*>** rv, EXPRESSION* exp);
Type* LookupSingleAggregate(Type* tp, EXPRESSION** exp, bool memberptr = false);
void getInitList( SYMBOL* funcsp, std::list<Argument*>** owner);
void getArgs( SYMBOL* funcsp, CallSite* funcparams, Keyword finish, bool allowPack, int flags);
void GetConstructorInitializers( SYMBOL* funcsp, CallSite* funcparams, Keyword finish, bool allowPack);
void CreateInitializerList(SYMBOL* func, Type* initializerListTemplate, Type* initializerListType, std::list<Argument*>** lptr,
                           bool operands, bool asref);
void PromoteConstructorArgs(SYMBOL* cons1, CallSite* params);
void AdjustParams(SYMBOL* func, SymbolTable<SYMBOL>::iterator it, SymbolTable<SYMBOL>::iterator itend, std::list<Argument*>** lptr,
                  bool operands, bool implicit);
void expression_assign( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
void expression_comma( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
void expression_no_comma( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
void expression_no_check( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags);
void expression( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags);
#if 1
// expressions
EXPRESSION* DerivedToBase(Type* tpn, Type* tpo, EXPRESSION* exp, int flags);
void expression_arguments( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags, bool noLex = false);
void expression_cast( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
#endif

}  // namespace Parser