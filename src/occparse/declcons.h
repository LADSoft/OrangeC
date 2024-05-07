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
extern bool noExcept;

void ConsDestDeclarationErrors(SYMBOL* sp, bool notype);
void ConstexprMembersNotInitializedErrors(SYMBOL* sym);
LexList* FindClass(LexList* lex, SYMBOL* funcsp, SYMBOL** sym);
std::list<MEMBERInitializerS*>* GetMemberInitializers(LexList** lex2, SYMBOL* funcsp, SYMBOL* sym);
void SetParams(SYMBOL* cons);
SYMBOL* insertFunc(SYMBOL* sp, SYMBOL* ovl);
bool matchesDefaultConstructor(SYMBOL* sp);
bool matchesCopy(SYMBOL* sp, bool move);
SYMBOL* getCopyCons(SYMBOL* base, bool move);
bool hasVTab(SYMBOL* sp);
void createConstructorsForLambda(SYMBOL* sp);
void createDefaultConstructors(SYMBOL* sp);
void ConditionallyDeleteClassMethods(SYMBOL* sp);
EXPRESSION* destructLocal(EXPRESSION* exp);
void DestructParams(std::list<Argument*>* il);
void destructBlock(EXPRESSION** exp, SymbolTable<SYMBOL> *table, bool mainDestruct);
SYMBOL* findClassName(const char* name, SYMBOL* cls, std::list<BASECLASS*>* bc, std::list<VBASEENTRY*>* vbase, int* offset);
void ParseMemberInitializers(SYMBOL* cls, SYMBOL* cons);
void createDestructor(SYMBOL* sp);
EXPRESSION* thunkConstructorHead(std::list<FunctionBlock*>& b, SYMBOL* sym, SYMBOL* cons, SymbolTable<SYMBOL>* syms, bool parseInitializers, bool doCopy,
                                 bool defaulted);
void createConstructor(SYMBOL* sp, SYMBOL* consfunc);
void asnVirtualBases(std::list<FunctionBlock*>& b, SYMBOL* sp, std::list<VBASEENTRY*>* vbe, EXPRESSION* thisptr, EXPRESSION* other, bool move, bool isconst);
void createAssignment(SYMBOL* sym, SYMBOL* asnfunc);
void thunkDestructorTail(std::list<FunctionBlock*>& b, SYMBOL* sp, SYMBOL* dest, SymbolTable<SYMBOL>* syms, bool defaulted);
void makeArrayConsDest(Type** tp, EXPRESSION** exp, SYMBOL* cons, SYMBOL* dest, EXPRESSION* count);
bool callDestructor(SYMBOL* sp, SYMBOL* against, EXPRESSION** exp, EXPRESSION* arrayElms, bool top, bool pointer, bool skipAccess,
                    bool novtab);
bool callConstructor(Type** tp, EXPRESSION** exp, CallSite* params, bool checkcopy, EXPRESSION* arrayElms, bool top,
                     bool maybeConversion, bool implicit, bool pointer, bool usesInitList, bool isAssign, bool toErr);
bool callConstructorParam(Type** tp, EXPRESSION** exp, Type* paramTP, EXPRESSION* paramExp, bool top, bool maybeConversion,
                          bool implicit, bool pointer, bool toErr);
void PromoteConstructorArgs(SYMBOL* cons1, CallSite* params);
}  // namespace Parser