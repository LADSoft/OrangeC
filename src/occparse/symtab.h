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

#ifndef _SYMTAB_H
#define _SYMTAB_H
#include <list>
#include <unordered_map>
#include <iterator>
#include <string>
namespace CompletionCompiler
{
    extern Parser::SymbolTable<Parser::SYMBOL>* ccSymbols;
    void ccSetSymbol(Parser::SYMBOL* s);
};

namespace Parser
{

    extern std::list<NAMESPACEVALUEDATA*>* globalNameSpace, * localNameSpace, *rootNameSpace;
    extern SymbolTable<SYMBOL>* labelSyms;

    extern int matchOverloadLevel;
    extern SymbolTableFactory<SYMBOL> symbols;

    void syminit(void);
    void AllocateLocalContext(std::list<BLOCKDATA*>& block, SYMBOL* sym, int label);
    void TagSyms(SymbolTable<SYMBOL>* syms);
    void FreeLocalContext(std::list<BLOCKDATA*>& block, SYMBOL* sym, int label);
    bool matchOverload(TYPE* tnew, TYPE* told, bool argsOnly);
    SYMBOL* searchOverloads(SYMBOL* sym, SymbolTable<SYMBOL>* table);
    SYMBOL* gsearch(const char* name);
    SYMBOL* tsearch(const char* name);

template <class T>
inline T* SymbolTable<T>::AddOverloadName(T* sym)
{
    for (auto q : *this)
    {
        if (!strcmp(q->sb->decoratedName, sym->sb->decoratedName))
            return (q);
    }
    AddName(sym);
    return (0);
}
template <>
inline SYMBOL* SymbolTable<SYMBOL>::AddOverloadName(SYMBOL* sym)
{
    if (!IsCompiler() && !sym->parserSet)
    {
        sym->parserSet = true;
        CompletionCompiler::ccSetSymbol(sym);
    }

    for (auto q : *this)
    {
        if (!strcmp(q->sb->decoratedName, sym->sb->decoratedName))
            return (q);
    }
    AddName(sym);
    return (0);
}
template<class T>
void SymbolTable<T>::remove(typename SymbolTable<T>::iterator it)
{
    lookupTable_.erase((*it)->name);
    return inOrder_.remove(*it);
}
template<class T>
typename SymbolTable<T>::iterator SymbolTable<T>::insert(typename SymbolTable<T>::iterator it, SYMBOL* sym)
{
    lookupTable_[sym->name] = sym;
    return inOrder_.insert(it, sym);
}
template<class T>
T* SymbolTable<T>::Lookup(const std::string& name) const
{
    auto it = lookupTable_.find(name);
    if (it == lookupTable_.end())
        return nullptr;
    return (*it).second;
}
template<class T>
T* search(const SymbolTable<T>* tbl, const std::string& name)
{
    const SymbolTable<T>* p = tbl;
    while (p)
    {
        T* sym = p->Lookup(name);
        if (sym)
            return sym;
        p = p->Next();
    }
    return nullptr;
}
template <class T>
inline void SymbolTable<T>::AddName(T* sym)
{
    inOrder_.push_back(sym);
    lookupTable_[sym->name] = sym;
}
template <class T>
inline void SymbolTable<T>::baseInsert(T* in)
{
    AddName(in);
}
template <>
inline void SymbolTable<SYMBOL>::baseInsert(SYMBOL* in)
{
    if (Optimizer::cparams.prm_extwarning)
        if (in->sb->storage_class == StorageClass::parameter || in->sb->storage_class == StorageClass::auto_ || in->sb->storage_class == StorageClass::register_)
        {
            SYMBOL* sym;
            if ((sym = gsearch(in->name)) != nullptr &&
                (sym->sb->storage_class == StorageClass::parameter || sym->sb->storage_class == StorageClass::auto_))
                preverror(ERR_VARIABLE_OBSCURES_VARIABLE_AT_HIGHER_SCOPE, in->name, sym->sb->declfile, sym->sb->declline);
        }
    if (Lookup(in->name))
    {
        if (IsCompiler() || this != CompletionCompiler::ccSymbols)
        {
            if (!structLevel || !templateNestingCount)
            {
                SYMBOL* sym = search(this, in->name);
                if (!sym || !sym->sb->wasUsing || !in->sb->wasUsing)
                    preverrorsym(ERR_DUPLICATE_IDENTIFIER, in, in->sb->declfile, in->sb->declline);
            }
        }
    }
    else
    {
        AddName(in);
    }
}

template <class T>
inline void SymbolTable<T>::Add(T* in)
{
    baseInsert(in);
}
template <>
inline void SymbolTable<SYMBOL>::Add(SYMBOL* in)
{
    if (!IsCompiler())
    {
        if (!in->parserSet)
        {
            in->parserSet = true;
            CompletionCompiler::ccSetSymbol(in);
        }
    }
    baseInsert(in);
}
template <class T>
inline void SymbolTable<T>::insertOverload(T* in)
{

    if (Optimizer::cparams.prm_extwarning)
        if (in->sb->storage_class == StorageClass::parameter || in->sb->storage_class == StorageClass::auto_ || in->sb->storage_class == StorageClass::register_)
        {
            SYMBOL* sym;
            if ((sym = gsearch(in->name)) != nullptr)
                preverror(ERR_VARIABLE_OBSCURES_VARIABLE_AT_HIGHER_SCOPE, in->name, sym->sb->declfile, sym->sb->declline);
        }
    if (AddOverloadName(in))
    {
        SetLinkerNames(in, Linkage::cdecl_);
        preverrorsym(ERR_DUPLICATE_IDENTIFIER, in, in->sb->declfile, in->sb->declline);
    }
}

}  // namespace Parser
#endif