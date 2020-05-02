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

void ConsDestDeclarationErrors(SYMBOL* sp, bool notype);
LEXEME* FindClass(LEXEME* lex, SYMBOL* funcsp, SYMBOL** sym);
MEMBERINITIALIZERS* GetMemberInitializers(LEXEME** lex2, SYMBOL* funcsp, SYMBOL* sym);
void SetParams(SYMBOL* cons);
SYMBOL* insertFunc(SYMBOL* sp, SYMBOL* ovl);
bool matchesCopy(SYMBOL* sp, bool move);
SYMBOL* getCopyCons(SYMBOL* base, bool move);
bool hasVTab(SYMBOL* sp);
void createConstructorsForLambda(SYMBOL* sp);
void createDefaultConstructors(SYMBOL* sp);
EXPRESSION* destructLocal(EXPRESSION* exp);
void destructBlock(EXPRESSION** exp, SYMLIST* hr, bool mainDestruct);
SYMBOL* findClassName(const char* name, SYMBOL* cls, BASECLASS* bc, VBASEENTRY* vbase, int* offset);
void ParseMemberInitializers(SYMBOL* cls, SYMBOL* cons);
EXPRESSION* thunkConstructorHead(BLOCKDATA* b, SYMBOL* sym, SYMBOL* cons, HASHTABLE* syms, bool parseInitializers, bool doCopy);
void createConstructor(SYMBOL* sp, SYMBOL* consfunc);
void asnVirtualBases(BLOCKDATA* b, SYMBOL* sp, VBASEENTRY* vbe, EXPRESSION* thisptr, EXPRESSION* other, bool move, bool isconst);
void createAssignment(SYMBOL* sym, SYMBOL* asnfunc);
void thunkDestructorTail(BLOCKDATA* b, SYMBOL* sp, SYMBOL* dest, HASHTABLE* syms);
void makeArrayConsDest(TYPE** tp, EXPRESSION** exp, SYMBOL* cons, SYMBOL* dest, EXPRESSION* count);
void callDestructor(SYMBOL* sp, SYMBOL* against, EXPRESSION** exp, EXPRESSION* arrayElms, bool top, bool pointer, bool skipAccess);
bool callConstructor(TYPE** tp, EXPRESSION** exp, FUNCTIONCALL* params, bool checkcopy, EXPRESSION* arrayElms, bool top,
    bool maybeConversion, bool implicit, bool pointer, bool usesInitList, bool isAssign);
bool callConstructorParam(TYPE** tp, EXPRESSION** exp, TYPE* paramTP, EXPRESSION* paramExp, bool top, bool maybeConversion,
    bool implicit, bool pointer);
void PromoteConstructorArgs(SYMBOL* cons1, FUNCTIONCALL* params);
