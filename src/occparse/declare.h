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

#include <functional>
#include <map>

#define CT_NONE 0
#define CT_CONS 1
#define CT_DEST 2
namespace Parser
{

extern int inDefaultParam;
extern char deferralBuf[100000];
extern SYMBOL* enumSyms;
extern std::list<STRUCTSYM> structSyms;
extern int expandingParams;
extern Optimizer::LIST* deferred;
extern int structLevel;
extern Optimizer::LIST* openStructs;
extern int parsingTrailingReturnOrUsing;
extern int inTypedef;
extern int resolvingStructDeclarations;
extern std::map<int, SYMBOL*> localAnonymousUnions;

void declare_init(void);
void InsertGlobal(SYMBOL* sp);
void WeedExterns(void);
const char* AnonymousName(void);
const char* AnonymousTypeName(void);
const char* NewUnnamedID(void);
SYMBOL* SymAlloc(void);
SYMBOL* makeID(StorageClass storage_class, TYPE* tp, SYMBOL* spi, const char* name);
SYMBOL* makeUniqueID(StorageClass storage_class, TYPE* tp, SYMBOL* spi, const char* name);
void addStructureDeclaration(STRUCTSYM* decl);
void addTemplateDeclaration(STRUCTSYM* decl);
void dropStructureDeclaration(void);
SYMBOL* getStructureDeclaration(void);
void InsertSymbol(SYMBOL* sp, StorageClass storage_class, Linkage linkage, bool allowDups);
LEXLIST* tagsearch(LEXLIST* lex, char* name, SYMBOL** rsp, SymbolTable<SYMBOL>** table, SYMBOL** strSym_out, std::list<NAMESPACEVALUEDATA*>** nsv_out,
                   StorageClass storage_class);
SYMBOL* calculateStructAbstractness(SYMBOL* top, SYMBOL* sp);
void calculateStructOffsets(SYMBOL* sp);
void resolveAnonymousUnions(SYMBOL* sp);
LEXLIST* innerDeclStruct(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* sp, bool inTemplate, AccessLevel defaultAccess, bool isfinal,
                         bool* defd, SymbolTable<SYMBOL>* anonymousTable);
LEXLIST* declstruct(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, bool inTemplate, bool asfriend, StorageClass storage_class,
    Linkage linkage2_in, AccessLevel access, bool* defd, bool constexpression);
LEXLIST* declenum(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, StorageClass storage_class, AccessLevel access, bool opaque,
    bool* defd);
void sizeQualifiers(TYPE* tp);
LEXLIST* parse_declspec(LEXLIST* lex, Linkage* linkage, Linkage* linkage2, Linkage* linkage3);
LEXLIST* getQualifiers(LEXLIST* lex, TYPE** tp, Linkage* linkage, Linkage* linkage2, Linkage* linkage3, bool* asFriend);
void injectThisPtr(SYMBOL* sp, SymbolTable<SYMBOL>* syms);
bool intcmp(TYPE* t1, TYPE* t2);
void checkIncompleteArray(TYPE* tp, const char* errorfile, int errorline);
LEXLIST* getDeferredData(LEXLIST* lex, LEXLIST** savePos, bool braces);

LEXLIST* getStorageAndType(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** strSym, bool inTemplate, bool assumeType,
    StorageClass* storage_class, StorageClass* storage_class_in, Optimizer::ADDRESS* address, bool* blocked,
    bool* isExplicit, bool* constexpression, TYPE** tp, Linkage* linkage, Linkage* linkage2,
    Linkage* linkage3, AccessLevel access, bool* notype, bool* defd, int* consdest, bool* templateArg,
    bool* asFriend);

LEXLIST* declare(LEXLIST* lex, SYMBOL* funcsp, TYPE** tprv, StorageClass storage_class, Linkage defaultLinkage, std::list<BLOCKDATA*>& block,
                 bool needsemi, int asExpression, bool inTemplate, AccessLevel access);
}  // namespace Parser