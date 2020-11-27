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

#include <stack>
namespace Parser
{
extern attributes basisAttribs;

extern Optimizer::LIST* nameSpaceList;
extern char anonymousNameSpaceName[512];

typedef struct
{
    VTABENTRY* entry;
    SYMBOL* func;
    SYMBOL* name;
} THUNK;

void SpecializationError(char* str);
void SpecializationError(SYMBOL* sym);
void dumpVTab(SYMBOL* sym);
void internalClassRefCount(SYMBOL* base, SYMBOL* derived, int* vcount, int* ccount, bool isVirtual);
int classRefCount(SYMBOL* base, SYMBOL* derived);
void CheckCalledException(SYMBOL* cst, EXPRESSION* exp);
void calculateVTabEntries(SYMBOL* sym, SYMBOL* base, VTABENTRY** pos, int offset);
void calculateVirtualBaseOffsets(SYMBOL* sym);
void deferredCompileOne(SYMBOL* cur);
void deferredInitializeStructFunctions(SYMBOL* cur);
void deferredInitializeStructMembers(SYMBOL* cur);
TYPE* PerformDeferredInitialization(TYPE* tp, SYMBOL* funcsp);
void warnCPPWarnings(SYMBOL* sym, bool localClassWarnings);
bool usesVTab(SYMBOL* sym);
BASECLASS* innerBaseClass(SYMBOL* declsym, SYMBOL* bcsym, bool isvirtual, enum e_ac currentAccess);
LEXEME* baseClasses(LEXEME* lex, SYMBOL* funcsp, SYMBOL* declsym, enum e_ac defaultAccess);
void checkPackedType(SYMBOL* sym);
std::stack<EXPRESSION*> iterateToPostOrder(EXPRESSION* exp);
bool hasPackedExpression(EXPRESSION* exp, bool useAuto);
void checkPackedExpression(EXPRESSION* exp);
void checkUnpackedExpression(EXPRESSION* exp);
void GatherPackedTypes(int* count, SYMBOL** arg, TYPE* tp);
void GatherPackedVars(int* count, SYMBOL** arg, EXPRESSION* packedExp);
void ReplicatePackedExpression(EXPRESSION* pattern, int count, SYMBOL** arg, TEMPLATEPARAMLIST** dest);
int CountPacks(TEMPLATEPARAMLIST* packs);
INITLIST** expandPackedInitList(INITLIST** lptr, SYMBOL* funcsp, LEXEME* start, EXPRESSION* packedExp);
MEMBERINITIALIZERS* expandPackedBaseClasses(SYMBOL* cls, SYMBOL* funcsp, MEMBERINITIALIZERS** init, BASECLASS* bc,
                                            VBASEENTRY* vbase);
void expandPackedMemberInitializers(SYMBOL* cls, SYMBOL* funcsp, TEMPLATEPARAMLIST* templatePack, MEMBERINITIALIZERS** p,
                                    LEXEME* start, INITLIST* list);
void checkOperatorArgs(SYMBOL* sp, bool asFriend);
LEXEME* handleStaticAssert(LEXEME* lex);
LEXEME* insertNamespace(LEXEME* lex, enum e_lk linkage, enum e_sc storage_class, bool* linked);
void unvisitUsingDirectives(NAMESPACEVALUELIST* v);
LEXEME* insertUsing(LEXEME* lex, SYMBOL** sp_out, enum e_ac access, enum e_sc storage_class, bool inTemplate, bool hasAttributes);
TYPE* AttributeFinish(SYMBOL* sym, TYPE* tp);
void ParseOut__attribute__(LEXEME** lex, SYMBOL* funcsp);
bool ParseAttributeSpecifiers(LEXEME** lex, SYMBOL* funcsp, bool always);
bool isConstexprConstructor(SYMBOL* sym);
bool MatchesConstFunction(SYMBOL* sym);
LEXEME* getDeclType(LEXEME* lex, SYMBOL* funcsp, TYPE** tn);
void CollapseReferences(TYPE* tp_in);

}  // namespace Parser