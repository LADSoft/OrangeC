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

namespace Parser
{
extern int inGetUserConversion;
extern int inSearchingFunctions;
extern int inNothrowHandler;
extern int matchOverloadLevel;

#define F_WITHCONS 1
#define F_INTEGER 2
#define F_ARITHMETIC 4
#define F_STRUCTURE 8
#define F_POINTER 16
#define F_CONVERSION 32

#define F_GOFERR 1
#define F_GOFDELETEDERR 2

bool matchOverload(Type* tnew, Type* told, bool argsOnly);
SYMBOL* searchOverloads(SYMBOL* sym, SymbolTable<SYMBOL>* table);
SYMBOL* lookupGenericConversion(SYMBOL* sym, Type* tp);
SYMBOL* lookupSpecificCast(SYMBOL* sym, Type* tp);
SYMBOL* lookupNonspecificCast(SYMBOL* sym, Type* tp);
SYMBOL* lookupIntCast(SYMBOL* sym, Type* tp, bool implicit);
SYMBOL* lookupArithmeticCast(SYMBOL* sym, Type* tp, bool implicit);
SYMBOL* lookupPointerCast(SYMBOL* sym, Type* tp);
void InitializeFunctionArguments(SYMBOL* sym, bool initialize = false);
SYMBOL* getUserConversion(int flags, Type* tpp, Type* tpa, EXPRESSION* expa, int* n, e_cvsrn* seq, SYMBOL* candidate_in,
    SYMBOL** userFunc, bool honorExplicit);
void GetRefs(Type* tpp, Type* tpa, EXPRESSION* expa, bool& lref, bool& rref);
void getQualConversion(Type* tpp, Type* tpa, EXPRESSION* exp, int* n, e_cvsrn* seq);
void getSingleConversion(Type* tpp, Type* tpa, EXPRESSION* expa, int* n, e_cvsrn* seq, SYMBOL* candidate, SYMBOL** userFunc,
                         bool allowUser, bool ref = false);
SYMBOL* detemplate(SYMBOL* sym, CallSite* args, Type* atp);
SYMBOL* GetOverloadedTemplate(SYMBOL* sp, CallSite* args);
void weedgathering(Optimizer::LIST** gather);
SYMBOL* ClassTemplateArgumentDeduction(Type** tp, EXPRESSION** exp, SYMBOL* sp, CallSite* args, int flags);
SYMBOL* GetOverloadedFunction(Type** tp, EXPRESSION** exp, SYMBOL* sp, CallSite* args, Type* atp, int toErr,
                              bool maybeConversion, int flags);
SYMBOL* MatchOverloadedFunction(Type* tp, Type** mtp, SYMBOL* sym, EXPRESSION** exp, int flags);
void ResolveArgumentFunctions(CallSite* args, bool toErr);

}  // namespace Parser