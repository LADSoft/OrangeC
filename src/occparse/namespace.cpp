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

#include <unordered_set>
#include "compiler.h"
#include <stack>
#include "ccerr.h"
#include "overload.h"
#include "namespace.h"
#include "class.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "declare.h"
#include "mangle.h"
#include "lambda.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "declcpp.h"
#include "expr.h"
#include "lex.h"
#include "help.h"
#include "unmangle.h"
#include "types.h"
#include "OptUtils.h"
#include "memory.h"
#include "beinterf.h"
#include "exprcpp.h"
#include "inline.h"
#include "iexpr.h"
#include "libcxx.h"
#include "declcons.h"
#include "symtab.h"
#include "ListFactory.h"
#include "constopt.h"
#include "occparse.h"
#include "crc32.h"
namespace Parser
{
SymbolTable<SYMBOL>* labelSyms;
std::list<NAMESPACEVALUEDATA*>*globalNameSpace, *localNameSpace, *rootNameSpace;

void namespaceinit()
{
    labelSyms = nullptr;
    globalNameSpace = namespaceValueDataListFactory.CreateList();
    globalNameSpace->push_front(Allocate<NAMESPACEVALUEDATA>());
    globalNameSpace->front()->syms = symbols->CreateSymbolTable();
    globalNameSpace->front()->tags = symbols->CreateSymbolTable();

    rootNameSpace = namespaceValueDataListFactory.CreateList();
    rootNameSpace->push_front(globalNameSpace->front());

    localNameSpace = namespaceValueDataListFactory.CreateList();
    localNameSpace->push_front(Allocate<NAMESPACEVALUEDATA>());

    matchOverloadLevel = 0;
}
void unvisitUsingDirectives(NAMESPACEVALUEDATA* v)
{
    if (v->usingDirectives)
    {
        for (auto sym : *v->usingDirectives)
        {
            sym->sb->visited = false;
            unvisitUsingDirectives(sym->sb->nameSpaceValues->front());
        }
    }
    if (v->inlineDirectives)
    {
        for (auto sym : *v->inlineDirectives)
        {
            sym->sb->visited = false;
            unvisitUsingDirectives(sym->sb->nameSpaceValues->front());
        }
    }
}

SYMBOL* tablesearchone(const char* name, NAMESPACEVALUEDATA* ns, bool tagsOnly)
{
    SYMBOL* rv = nullptr;
    if (!tagsOnly)
        rv = search(ns->syms, name);
    if (!rv)
        rv = search(ns->tags, name);
    if (rv)
    {
        return rv;
    }
    return nullptr;
}
static void namespacesearchone(const char* name, NAMESPACEVALUEDATA* ns, std::list<SYMBOL*>& gather, bool tagsOnly,
                               bool allowUsing);
std::list<SYMBOL*> tablesearchinline(const char* name, NAMESPACEVALUEDATA* ns, bool tagsOnly, bool allowUsing)
{
    // main namespace
    std::list<SYMBOL*> rv;
    SYMBOL* find = tablesearchone(name, ns, tagsOnly);
    if (find)
        rv.insert(rv.begin(), find);
    // included inlines
    if (ns->inlineDirectives)
    {
        for (auto x : *ns->inlineDirectives)
        {
            if (!x->sb->visited)
            {
                x->sb->visited = true;
                auto rv1 = tablesearchinline(name, x->sb->nameSpaceValues->front(), tagsOnly, allowUsing);
                if (rv1.size())
                {
                    rv.insert(rv.begin(), rv1.begin(), rv1.end());
                }
            }
        }
    }
    // any using definitions in this inline namespace
    if (allowUsing && ns->usingDirectives)
    {
        for (auto x : *ns->usingDirectives)
        {
            if (!x->sb->visited)
            {
                x->sb->visited = true;
                namespacesearchone(name, x->sb->nameSpaceValues->front(), rv, tagsOnly, allowUsing);
            }
        }
    }
    // enclosing ns if this one is inline
    if (ns->name && !ns->name->sb->visited && ns->name->sb->attribs.inheritable.linkage == Linkage::inline_)
    {
        ns->name->sb->visited = true;
        auto rv1 = tablesearchinline(name, ns->name->sb->nameSpaceValues->front(), tagsOnly, allowUsing);
        if (rv1.size())
        {
            rv.insert(rv.begin(), rv1.begin(), rv1.end());
        }
    }
    return rv;
}
static void namespacesearchone(const char* name, NAMESPACEVALUEDATA* ns, std::list<SYMBOL*>& gather, bool tagsOnly, bool allowUsing)
{
    auto rv = tablesearchinline(name, ns, tagsOnly, allowUsing);
    if (rv.size())
    {
        rv.insert(rv.end(), gather.begin(), gather.end());
    }
    else
    {
        rv = gather;
    }
    if (allowUsing && ns->usingDirectives)
    {
        for (auto x : *ns->usingDirectives)
        {
            if (!x->sb->visited)
            {
                x->sb->visited = true;
                namespacesearchone(name, x->sb->nameSpaceValues->front(), rv, tagsOnly, allowUsing);
            }
        }
    }
    gather = rv;
}
static std::list<SYMBOL*> namespacesearchInternal(const char* name, std::list<NAMESPACEVALUEDATA*>* ns, bool qualified,
                                                  bool tagsOnly, bool allowUsing)
{
    std::list<SYMBOL*> lst;

    for (auto ns1 : *ns)
    {
        unvisitUsingDirectives(ns1);
        namespacesearchone(name, ns1, lst, tagsOnly, allowUsing);
        if (qualified || lst.size())
            break;
    }
    return lst;
}
SYMBOL* namespacesearch(const char* name, std::list<NAMESPACEVALUEDATA*>* ns, bool qualified, bool tagsOnly)
{
    auto lst = namespacesearchInternal(name, ns, qualified, tagsOnly, true);

    if (lst.size())
    {
        if (lst.size() > 1)
        {
            bool found = false;
            for (auto a : lst)
            {
                if (a->sb->storage_class != StorageClass::overloads_)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                Type* tp = Type::MakeType(BasicType::aggregate_);
                SYMBOL* sym = makeID(StorageClass::overloads_, tp, nullptr, lst.front()->name);
                tp->sp = sym;
                tp->syms = symbols->CreateSymbolTable();
                for (auto a : lst)
                {
                    for (auto b : *a->tp->syms)
                    {
                        tp->syms->AddName(b);
                    }
                }
                return sym;
            }
        }
        for (auto it = lst.begin(); it != lst.end(); ++it)
        {
            // collision
            auto it1 = it;
            for (++it1; it1 != lst.end(); ++it1)
            {
                if ((*it) != (*it1) && (*it)->sb->mainsym != (*it1) && ((SYMBOL*)(*it1))->sb->mainsym != (*it))
                {
                    if ((*it)->sb->mainsym && (*it)->sb->mainsym != ((SYMBOL*)(*it1))->sb->mainsym)
                        errorsym2(ERR_AMBIGUITY_BETWEEN, (*it), (SYMBOL*)(*it1));
                }
            }
        }
        return (SYMBOL*)lst.front();
    }
    return nullptr;
}
void searchNS(SYMBOL* sym, SYMBOL* nssp, std::list<SYMBOL*>& gather)
{
    if (nssp)
    {
        auto x = namespacesearchInternal(sym->name, nssp->sb->nameSpaceValues, true, false, false);
        if (x.size())
        {
            gather.insert(gather.end(), x.begin(), x.end());
        }
    }
}
SYMBOL* gsearch(const char* name)
{
    SYMBOL* sym = search(localNameSpace->front()->syms, name);
    if (sym)
        return sym;
    return search(globalNameSpace->front()->syms, name);
}
SYMBOL* tsearch(const char* name)
{
    SYMBOL* sym = search(localNameSpace->front()->tags, name);
    if (sym)
        return sym;
    return search(globalNameSpace->front()->tags, name);
}
LexList* insertNamespace(LexList* lex, Linkage linkage, StorageClass storage_class, bool* linked)
{
    bool anon = false;
    char buf[256], *p;
    SYMBOL* sym;
    Optimizer::LIST* list;
    *linked = false;
    if (ISID(lex))
    {
        strcpy(buf, lex->data->value.s.a);
        lex = getsym();
        if (MATCHKW(lex, Keyword::assign_))
        {
            lex = getsym();
            if (ISID(lex))
            {
                char buf1[512];
                strcpy(buf1, lex->data->value.s.a);
                lex = nestedSearch(lex, &sym, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, true, false);
                if (sym)
                {
                    if (sym->sb->storage_class != StorageClass::namespace_)
                    {
                        errorsym(ERR_NOT_A_NAMESPACE, sym);
                    }
                    else
                    {
                        SYMBOL* src = sym;
                        SYMBOL* sym = nullptr;
                        Type* tp;
                        SYMLIST** p;
                        if (storage_class == StorageClass::auto_)
                            sym = localNameSpace->front()->syms->Lookup(buf);
                        else
                            sym = globalNameSpace->front()->syms->Lookup(buf);
                        if (sym)
                        {
                            // already exists, bug check it
                            if (sym->sb->storage_class == StorageClass::namespace_alias_ &&
                                sym->sb->nameSpaceValues->front()->origname == src)
                            {
                                if (linkage == Linkage::inline_)
                                {
                                    error(ERR_INLINE_NOT_ALLOWED);
                                }
                                lex = getsym();
                                return lex;
                            }
                        }
                        tp = Type::MakeType(BasicType::void_);
                        sym = makeID(StorageClass::namespace_alias_, tp, nullptr, litlate(buf));
                        if (nameSpaceList.size())
                        {
                            sym->sb->parentNameSpace = nameSpaceList.front();
                        }
                        SetLinkerNames(sym, Linkage::none_);
                        if (storage_class == StorageClass::auto_)
                        {
                            localNameSpace->front()->syms->Add(sym);
                            localNameSpace->front()->tags->Add(sym);
                        }
                        else
                        {
                            globalNameSpace->front()->syms->Add(sym);
                            globalNameSpace->front()->tags->Add(sym);
                        }
                        sym->sb->nameSpaceValues = namespaceValueDataListFactory.CreateList();
                        *sym->sb->nameSpaceValues = *src->sb->nameSpaceValues;
                        *sym->sb->nameSpaceValues->begin() = Allocate<NAMESPACEVALUEDATA>();
                        **sym->sb->nameSpaceValues->begin() = **src->sb->nameSpaceValues->begin();
                        sym->sb->nameSpaceValues->front()->name = sym;  // this is to rename it with the alias e.g. for errors
                    }
                }
                if (linkage == Linkage::inline_)
                {
                    error(ERR_INLINE_NOT_ALLOWED);
                }
                lex = getsym();
            }
            else
            {
                error(ERR_EXPECTED_NAMESPACE_NAME);
            }
            return lex;
        }
    }
    else
    {
        anon = true;
        if (!anonymousNameSpaceName[0])
        {
            p = (char*)strrchr(infile, '\\');
            if (!p)
            {
                p = (char*)strrchr(infile, '/');
                if (!p)
                    p = infile;
                else
                    p++;
            }
            else
                p++;

            sprintf(anonymousNameSpaceName, "__%s__%08x", p, Utils::CRC32((unsigned char*)infile, strlen(infile)));
            while ((p = (char*)strchr(anonymousNameSpaceName, '.')) != 0)
                *p = '_';
        }
        strcpy(buf, anonymousNameSpaceName);
    }
    if (storage_class != StorageClass::global_)
    {
        error(ERR_NO_NAMESPACE_IN_FUNCTION);
    }
    SYMBOL* sp = globalNameSpace->front()->syms->Lookup(buf);
    if (!sp)
    {
        sym = makeID(StorageClass::namespace_, Type::MakeType(BasicType::void_), nullptr, litlate(buf));
        sym->sb->nameSpaceValues = namespaceValueDataListFactory.CreateList();
        *sym->sb->nameSpaceValues = *globalNameSpace;
        sym->sb->nameSpaceValues->push_front(Allocate<NAMESPACEVALUEDATA>());
        sym->sb->nameSpaceValues->front()->syms = symbols->CreateSymbolTable();
        sym->sb->nameSpaceValues->front()->tags = symbols->CreateSymbolTable();
        sym->sb->nameSpaceValues->front()->origname = sym;
        sym->sb->nameSpaceValues->front()->name = sym;
        sym->sb->parentNameSpace = globalNameSpace->front()->name;
        sym->sb->attribs.inheritable.linkage = linkage;
        if (nameSpaceList.size())
        {
            sym->sb->parentNameSpace = nameSpaceList.front();
        }
        SetLinkerNames(sym, Linkage::none_);
        globalNameSpace->front()->syms->Add(sym);
        globalNameSpace->front()->tags->Add(sym);
        if (anon || linkage == Linkage::inline_)
        {
            // plop in a using directive for the anonymous namespace we are declaring
            if (linkage == Linkage::inline_)
            {
                if (!globalNameSpace->front()->inlineDirectives)
                    globalNameSpace->front()->inlineDirectives = symListFactory.CreateList();
                globalNameSpace->front()->inlineDirectives->push_front(sym);
            }
            else
            {
                if (!globalNameSpace->front()->usingDirectives)
                    globalNameSpace->front()->usingDirectives = symListFactory.CreateList();
                globalNameSpace->front()->usingDirectives->push_front(sym);
            }
        }
    }
    else
    {
        sym = sp;
        if (sym->sb->storage_class != StorageClass::namespace_)
        {
            errorsym(ERR_NOT_A_NAMESPACE, sym);
            return lex;
        }
        if (linkage == Linkage::inline_)
            if (sym->sb->attribs.inheritable.linkage != Linkage::inline_)
                errorsym(ERR_NAMESPACE_NOT_INLINE, sym);
    }
    sym->sb->value.i++;

    nameSpaceList.push_front(sym);

    globalNameSpace->push_front(sym->sb->nameSpaceValues->front());

    *linked = true;
    return lex;
}
}  // namespace Parser