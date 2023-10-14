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

#include <stack>
namespace Parser
{
extern attributes basisAttribs;

extern std::list<SYMBOL*> nameSpaceList;
extern char anonymousNameSpaceName[512];
extern int noNeedToSpecialize;
extern int parsingUsing;

typedef struct
{
    VTABENTRY* entry;
    SYMBOL* func;
    SYMBOL* name;
} THUNK;

bool MustSpecialize(const char* name);
void SpecializationError(char* str);
void SpecializationError(SYMBOL* sym);
void dumpVTab(SYMBOL* sym);
void internalClassRefCount(SYMBOL* base, SYMBOL* derived, int* vcount, int* ccount, bool isVirtual);
int classRefCount(SYMBOL* base, SYMBOL* derived);
void CheckCalledException(SYMBOL* cst, EXPRESSION* exp);
void calculateVTabEntries(SYMBOL* sym, SYMBOL* base, std::list<VTABENTRY*>** pos, int offset);
void calculateVirtualBaseOffsets(SYMBOL* sym);
void deferredCompileOne(SYMBOL* cur);
void deferredInitializeDefaultArg(SYMBOL* arg, SYMBOL* func);
void deferredInitializeStructFunctions(SYMBOL* cur);
void deferredInitializeStructMembers(SYMBOL* cur);
TYPE* PerformDeferredInitialization(TYPE* tp, SYMBOL* funcsp);
void warnCPPWarnings(SYMBOL* sym, bool localClassWarnings);
bool usesVTab(SYMBOL* sym);
BASECLASS* innerBaseClass(SYMBOL* declsym, SYMBOL* bcsym, bool isvirtual, AccessLevel currentAccess);
LEXLIST* baseClasses(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* declsym, AccessLevel defaultAccess);
void checkPackedType(SYMBOL* sym);
std::stack<EXPRESSION*> iterateToPostOrder(EXPRESSION* exp);
bool hasPackedExpression(EXPRESSION* exp, bool useAuto);
void checkPackedExpression(EXPRESSION* exp);
void checkUnpackedExpression(EXPRESSION* exp);
bool declaringTemplate(SYMBOL* sym);    
void GatherPackedTypes(int* count, SYMBOL** arg, TYPE* tp);
void GatherPackedVars(int* count, SYMBOL** arg, EXPRESSION* packedExp);
void ReplicatePackedExpression(EXPRESSION* pattern, int count, SYMBOL** arg, std::list<TEMPLATEPARAMPAIR>** dest);
int CountPacks(std::list<TEMPLATEPARAMPAIR>* packs);
void expandPackedInitList(std::list<INITLIST*>** lptr, SYMBOL* funcsp, LEXLIST* start, EXPRESSION* packedExp);
void expandPackedBaseClasses(SYMBOL* cls, SYMBOL* funcsp, std::list<MEMBERINITIALIZERS*>::iterator& init,
                             std::list<MEMBERINITIALIZERS*>::iterator& initend, std::list<MEMBERINITIALIZERS*>* mi,
                             std::list<BASECLASS*>* bc, std::list<VBASEENTRY*>* vbase);
void expandPackedMemberInitializers(SYMBOL* cls, SYMBOL* funcsp,
                                                                       std::list<TEMPLATEPARAMPAIR>* templatePack,
                                                                       std::list<MEMBERINITIALIZERS*>** p,
                                    LEXLIST* start, std::list<INITLIST*>* list);
void checkOperatorArgs(SYMBOL* sp, bool asFriend);
LEXLIST* handleStaticAssert(LEXLIST* lex);
LEXLIST* insertNamespace(LEXLIST* lex, Linkage linkage, StorageClass storage_class, bool* linked);
void unvisitUsingDirectives(NAMESPACEVALUEDATA* v);
LEXLIST* insertUsing(LEXLIST* lex, SYMBOL** sp_out, AccessLevel access, StorageClass storage_class, bool inTemplate, bool hasAttributes);
TYPE* AttributeFinish(SYMBOL* sym, TYPE* tp);
void ParseOut___attribute__(LEXLIST** lex, SYMBOL* funcsp);
bool ParseAttributeSpecifiers(LEXLIST** lex, SYMBOL* funcsp, bool always);
bool isConstexprConstructor(SYMBOL* sym);
bool MatchesConstFunction(SYMBOL* sym);
LEXLIST* getDeclType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tn);
void CollapseReferences(TYPE* tp_in);
EXPRESSION* addLocalDestructor(EXPRESSION* exp, SYMBOL* decl);
void CheckIsLiteralClass(TYPE* tp);
LEXLIST* GetStructuredBinding(LEXLIST* lex, SYMBOL* funcsp, StorageClass storage_class, Linkage linkage,
                              std::list<BLOCKDATA*>& block);
}  // namespace Parser