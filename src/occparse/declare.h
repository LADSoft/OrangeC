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

#include <functional>
#include <map>
#include <stack>
#include <deque>
#define CT_NONE 0
#define CT_CONS 1
#define CT_DEST 2
namespace Parser
{
struct EnclosingDeclaration
{
    SYMBOL* str;
    std::list<TEMPLATEPARAMPAIR>* tmpl;
};
struct EnclosingDeclarations
{
    typedef std::deque<EnclosingDeclaration>::iterator iterator;
    iterator begin() { return declarations.begin(); }
    iterator end() { return declarations.end(); }
    inline SYMBOL* GetFirst()
    {
        for (auto d : declarations)
        {
            if (d.str)
                return d.str;
        }
        return nullptr;
    }
    void Add(SYMBOL* symbol);
    inline void Add(std::list<TEMPLATEPARAMPAIR>* templ) { declarations.push_front(EnclosingDeclaration{nullptr, templ}); }
    void Drop()
    {
        if (!declarations.empty())
            declarations.pop_front();
    }
    inline void Mark() { marks.push(declarations.size()); }
    void Release()
    {
        if (marks.empty())
        {
            declarations.clear();
        }
        else
        {
            while (declarations.size() > marks.top())
                declarations.pop_front();
            marks.pop();
        }
    }
    void clear()
    {
        declarations.clear();
        while (!marks.empty())
            marks.pop();
    }

  private:
    std::deque<EnclosingDeclaration> declarations;
    std::stack<unsigned> marks;
};

extern int inDefaultParam;
extern char deferralBuf[100000];
extern SYMBOL* enumSyms;
extern EnclosingDeclarations enclosingDeclarations;
extern int expandingParams;
extern Optimizer::LIST* deferred;
extern int structLevel;
extern Optimizer::LIST* openStructs;
extern int parsingTrailingReturnOrUsing;
extern int inTypedef;
extern int resolvingStructDeclarations;
extern std::map<int, SYMBOL*> localAnonymousUnions;
extern int declaringInitialType;

// declare.cpp
SYMBOL* calculateStructAbstractness(SYMBOL* top, SYMBOL* sp);
void EnterStructureStaticAssert(SYMBOL* sym, LexList* deferredCompile);
void declare_init(void);
void InsertGlobal(SYMBOL* sp);
void WeedExterns(void);
const char* AnonymousName(void);
const char* NewUnnamedID(void);
SYMBOL* SymAlloc(void);
SYMBOL* makeID(StorageClass storage_class, Type* tp, SYMBOL* spi, const char* name);
SYMBOL* makeUniqueID(StorageClass storage_class, Type* tp, SYMBOL* spi, const char* name);
void InsertSymbol(SYMBOL* sp, StorageClass storage_class, Linkage linkage, bool allowDups);
LexList* innerDeclStruct(LexList* lex, SYMBOL* funcsp, SYMBOL* sp, bool inTemplate, AccessLevel defaultAccess, bool isfinal,
                         bool* defd, bool nobody, SymbolTable<SYMBOL>* anonymousTable);
LexList* declstruct(LexList* lex, SYMBOL* funcsp, Type** tp, bool inTemplate, bool asfriend, StorageClass storage_class,
                    Linkage linkage2_in, AccessLevel access, bool* defd, bool constexpression);
LexList* declenum(LexList* lex, SYMBOL* funcsp, Type** tp, StorageClass storage_class, AccessLevel access, bool opaque, bool* defd);
void sizeQualifiers(Type* tp);
LexList* parse_declspec(LexList* lex, Linkage* linkage, Linkage* linkage2, Linkage* linkage3);
LexList* getQualifiers(LexList* lex, Type** tp, Linkage* linkage, Linkage* linkage2, Linkage* linkage3, bool* asFriend);
void injectThisPtr(SYMBOL* sp, SymbolTable<SYMBOL>* syms);
void checkIncompleteArray(Type* tp, const char* errorfile, int errorline);
LexList* getDeferredData(LexList* lex, LexList** savePos, bool braces);
LexList* getStorageAndType(LexList* lex, SYMBOL* funcsp, SYMBOL** strSym, bool inTemplate, bool assumeType, bool* deduceTemplate,
                           StorageClass* storage_class, StorageClass* storage_class_in, Optimizer::ADDRESS* address, bool* blocked,
                           bool* isExplicit, bool* constexpression, bool* builtin_constexpr, Type** tp, Linkage* linkage,
                           Linkage* linkage2, Linkage* linkage3, AccessLevel access, bool* notype, bool* defd, int* consdest,
                           bool* templateArg, bool* asFriend);

LexList* declare(LexList* lex, SYMBOL* funcsp, Type** tprv, StorageClass storage_class, Linkage defaultLinkage,
                 std::list<FunctionBlock*>& block, bool needsemi, int asExpression, bool inTemplate, AccessLevel access);
}  // namespace Parser
