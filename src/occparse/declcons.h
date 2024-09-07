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
extern bool noExcept;

// decl group
std::list<MEMBERINITIALIZERS*>* GetMemberInitializers(LexList** lex2, SYMBOL* funcsp, SYMBOL* sym);
void SetParams(SYMBOL* cons);
void ConditionallyDeleteClassMethods(SYMBOL* sp);
void createConstructorsForLambda(SYMBOL* sp);
void createDefaultConstructors(SYMBOL* sp);
void createDestructor(SYMBOL* sp);
SYMBOL* insertFunc(SYMBOL* sp, SYMBOL* ovl);
//
bool matchesCopy(SYMBOL* sp, bool move);
SYMBOL* getCopyCons(SYMBOL* base, bool move);
bool hasVTab(SYMBOL* sp);
SYMBOL* findClassName(const char* name, SYMBOL* cls, std::list<BASECLASS*>* bc, std::list<VBASEENTRY*>* vbase, int* offset);
void ParseMemberInitializers(SYMBOL* cls, SYMBOL* cons);
EXPRESSION* thunkConstructorHead(std::list<FunctionBlock*>& b, SYMBOL* sym, SYMBOL* cons, SymbolTable<SYMBOL>* syms, bool parseInitializers, bool doCopy,
                                 bool defaulted);
void createConstructor(SYMBOL* sp, SYMBOL* consfunc);
void createAssignment(SYMBOL* sym, SYMBOL* asnfunc);
void thunkDestructorTail(std::list<FunctionBlock*>& b, SYMBOL* sp, SYMBOL* dest, SymbolTable<SYMBOL>* syms, bool defaulted);
bool CallDestructor(SYMBOL* sp, SYMBOL* against, EXPRESSION** exp, EXPRESSION* arrayElms, bool top, bool pointer, bool skipAccess,
                    bool novtab);
bool CallConstructor(Type** tp, EXPRESSION** exp, CallSite* params, bool checkcopy, EXPRESSION* arrayElms, bool top,
                     bool maybeConversion, bool implicit, bool pointer, bool usesInitList, bool isAssign, bool toErr);
bool callConstructorParam(Type** tp, EXPRESSION** exp, Type* paramTP, EXPRESSION* paramExp, bool top, bool maybeConversion,
                          bool implicit, bool pointer, bool toErr);
}  // namespace Parser