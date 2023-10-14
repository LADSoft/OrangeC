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

#include "compiler.h"
#include "PreProcessor.h"
#include "ccerr.h"
#include "config.h"
#include "declare.h"
#include "template.h"
#include "initbackend.h"
#include "occparse.h"
#include "memory.h"
#include "stmt.h"
#include "mangle.h"
#include "cpplookup.h"
#include "help.h"
#include "declcons.h"
#include "types.h"
#include "lex.h"
#include "symtab.h"
#include "ListFactory.h"
namespace CompletionCompiler
{
extern Parser::SymbolTable<Parser::SYMBOL>* ccSymbols;
void ccSetSymbol(Parser::SYMBOL* s);
}  // namespace CompletionCompiler
namespace Parser
{

std::list<NAMESPACEVALUEDATA*>*globalNameSpace, *localNameSpace, * rootNameSpace;
SymbolTable<SYMBOL>* labelSyms;

int matchOverloadLevel;
SymbolTableFactory<SYMBOL> symbols;

static std::list<std::list<SYMBOL*>*> usingDirectives;

void syminit(void)
{
    symbols.Reset();
    labelSyms = nullptr;
    globalNameSpace = namespaceValueDataListFactory.CreateList();
    globalNameSpace->push_front(Allocate<NAMESPACEVALUEDATA>());
    globalNameSpace->front()->syms = symbols.CreateSymbolTable();
    globalNameSpace->front()->tags = symbols.CreateSymbolTable();

    rootNameSpace = namespaceValueDataListFactory.CreateList();
    rootNameSpace->push_front(globalNameSpace->front());

    localNameSpace = namespaceValueDataListFactory.CreateList();
    localNameSpace->push_front(Allocate<NAMESPACEVALUEDATA>());

    usingDirectives.clear();
    matchOverloadLevel = 0;
}
void AllocateLocalContext(std::list<BLOCKDATA*>& block, SYMBOL* sym, int label)
{
    SymbolTable<SYMBOL>* tn = symbols.CreateSymbolTable();
    STATEMENT* st;
    Optimizer::LIST* l;
    st = stmtNode(nullptr, block, StatementNode::dbgblock_);
    st->label = 1;
    if (block.size() && Optimizer::cparams.prm_debug)
    {
        st = stmtNode(nullptr, block, StatementNode::label_);
        st->label = label;
    }
    tn->Next(localNameSpace->front()->syms);
    tn->Chain(localNameSpace->front()->syms);

    localNameSpace->front()->syms = tn;
    tn = symbols.CreateSymbolTable();
    tn->Next(localNameSpace->front()->tags);
    tn->Chain(localNameSpace->front()->tags);
    localNameSpace->front()->tags = tn;
    if (sym)
        localNameSpace->front()->tags->Block(sym->sb->value.i++);

    usingDirectives.push_front(localNameSpace->front()->usingDirectives);
}
void TagSyms(SymbolTable<SYMBOL>* syms)
{
    int i;
    for (auto sym : *syms)
    {
        sym->sb->ccEndLine = preProcessor->GetRealLineNo() + 1;
    }
}
void FreeLocalContext(std::list<BLOCKDATA*>& block, SYMBOL* sym, int label)
{
    SymbolTable<SYMBOL>* locals = localNameSpace->front()->syms;
    SymbolTable<SYMBOL>* tags = localNameSpace->front()->tags;
    STATEMENT* st;
    if (block.size() && Optimizer::cparams.prm_debug)
    {
        st = stmtNode(nullptr, block, StatementNode::label_);
        st->label = label;
    }
    checkUnused(localNameSpace->front()->syms);
    if (sym)
        sym->sb->value.i--;

    st = stmtNode(nullptr, block, StatementNode::expr_);
    destructBlock(&st->select, localNameSpace->front()->syms, true);
    localNameSpace->front()->syms = localNameSpace->front()->syms->Next();
    localNameSpace->front()->tags = localNameSpace->front()->tags->Next();


    localNameSpace->front()->usingDirectives = usingDirectives.front();
    usingDirectives.pop_front();

    if (!IsCompiler())
    {
        TagSyms(locals);
        TagSyms(tags);
    }
    if (sym)
    {
        locals->Next(sym->sb->inlineFunc.syms);
        tags->Next(sym->sb->inlineFunc.tags);
        sym->sb->inlineFunc.syms = locals;
        sym->sb->inlineFunc.tags = tags;
    }
    st = stmtNode(nullptr, block, StatementNode::dbgblock_);
    st->label = 0;
}

/* SYMBOL tab Keyword::hash_ function */
static int GetHashValue(const char* string)
{
    unsigned i;
    for (i = 0; *string; string++)
        i = ((i << 7) + (i << 1) + i) ^ *string;
    return i;
}

bool matchOverload(TYPE* tnew, TYPE* told, bool argsOnly)
{
    auto hnew = basetype(tnew)->syms->begin();
    auto hnewe = basetype(tnew)->syms->end();
    auto hold = basetype(told)->syms->begin();
    auto holde = basetype(told)->syms->end();
    unsigned tableOld[100], tableNew[100];
    int tCount = 0;
    if (!Optimizer::cparams.prm_cplusplus)
        argsOnly = true;
    if (isconst(tnew) != isconst(told))
        return false;
    if (isvolatile(tnew) != isvolatile(told))
        return false;
    if (islrqual(tnew) != islrqual(told))
        return false;
    if (isrrqual(tnew) != isrrqual(told))
        return false;
    matchOverloadLevel++;
    while (hnew != hnewe && hold != holde)
    {
        SYMBOL* snew = *hnew;
        SYMBOL* sold = *hold;
        TYPE *tnew, *told;
        if (sold->sb->thisPtr)
        {
            ++hold;
            if (hold == holde)
                break;
            sold = *hold;
        }
        if (snew->sb->thisPtr)
        {
            ++hnew;
            if (hnew == hnewe)
                break;
            snew = *hnew;
        }
        tnew = basetype(snew->tp);
        told = basetype(sold->tp);
        if (told->type != BasicType::any_ || tnew->type != BasicType::any_)  // packed template param
        {
            if ((told->type != tnew->type || (!comparetypes(told, tnew, true) && !sameTemplatePointedTo(told, tnew, true))) &&
                !sameTemplateSelector(told, tnew))
                break;
            else
            {
                bool matchconst = false;
                TYPE* tps = sold->tp;
                TYPE* tpn = snew->tp;
                if (isref(tps))
                {
                    matchconst = true;
                    tps = basetype(tps)->btp;
                }
                if (isref(tpn))
                {
                    matchconst = true;
                    tpn = basetype(tpn)->btp;
                }
                while (tps != tps->rootType && tps->type != BasicType::typedef_ && tps->type != BasicType::const_ && tps->type != BasicType::volatile_)
                    tps = tps->btp;
                while (tpn != tpn->rootType && tpn->type != BasicType::typedef_ && tpn->type != BasicType::const_ && tpn->type != BasicType::volatile_)
                    tpn = tpn->btp;
                if (tpn->type != BasicType::typedef_ && tps->type != BasicType::typedef_ && (ispointer(tpn) || ispointer(tps)))
                {
                    while (ispointer(tpn) && ispointer(tps) && tpn->type != BasicType::typedef_ && tps->type != BasicType::typedef_)
                    {
                        if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                        {
                            matchOverloadLevel--;
                            return false;
                        }
                        tpn = basetype(tpn)->btp;
                        tps = basetype(tps)->btp;
                    }
                    if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                    {
                        matchOverloadLevel--;
                        return false;
                    }
                }
                else if (matchconst)
                {
                    if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                    {
                        matchOverloadLevel--;
                        return false;
                    }                
                }
                tpn = basetype(tpn);
                tps = basetype(tps);
                if (tpn->type == BasicType::templateparam_)
                {
                    if (tps->type != BasicType::templateparam_)
                        break;
                    if (tpn->templateParam->second->packed != tps->templateParam->second->packed)
                        break;
                    tableOld[tCount] = GetHashValue(tps->templateParam->first->name);
                    tableNew[tCount] = GetHashValue(tpn->templateParam->first->name);
                    tCount++;
                }
            }
        }
        ++hold;
        ++hnew;
    }
    matchOverloadLevel--;
    if (hold == holde && hnew == hnewe)
    {
        if (tCount)
        {
            int i, j;
            SYMBOL* fnew = basetype(tnew)->sp->sb->parentClass;
            SYMBOL* fold = basetype(told)->sp->sb->parentClass;
            std::list<TEMPLATEPARAMPAIR>::iterator itNew, iteNew = itNew, itOld, iteOld = itOld;
            int iCount = 0;
            unsigned oldIndex[100], newIndex[100];

            if (fnew && fnew->templateParams)
            {
                itNew = fnew->templateParams->begin();
                iteNew = fnew->templateParams->end();
            }
            if (fold && fold->templateParams)
            {
                itOld = fold->templateParams->begin();
                iteOld = fold->templateParams->end();
            }
            if (itNew != iteNew)
            {
                if (itNew->second->bySpecialization.types)
                {
                    iteNew = itNew->second->bySpecialization.types->end();
                    itNew = itNew->second->bySpecialization.types->begin();
                }
                else
                {
                    ++itNew;
                }
            }
            if (itOld != iteOld)
            {
                if (itOld->second->bySpecialization.types)
                {
                    iteOld = itOld->second->bySpecialization.types->end();
                    itOld = itOld->second->bySpecialization.types->begin();
                }
                else
                {
                    ++itOld;
                }
            }
            for ( ; itNew != iteNew && itOld != iteOld ; ++itNew, ++itOld)
            {
                if (itOld->first && itNew->first)
                {
                    oldIndex[iCount] = GetHashValue(itOld->first->name);
                    newIndex[iCount] = GetHashValue(itNew->first->name);
                    iCount++;
                }
            }
            for (i = 0; i < tCount; i++)
            {
                int k, l;
                for (k = 0; k < iCount; k++)
                    if (tableOld[i] == oldIndex[k])
                        break;
                for (l = 0; l < iCount; l++)
                    if (tableNew[i] == newIndex[l])
                        break;
                if (k != l)
                {
                    return false;
                }
                for (j = i + 1; j < tCount; j++)
                    if (tableOld[i] == tableOld[j])
                    {
                        if (tableNew[i] != tableNew[j])
                            return false;
                    }
                    else
                    {
                        if (tableNew[i] == tableNew[j])
                            return false;
                    }
            }
        }
        if (basetype(tnew)->sp && basetype(told)->sp)
        {
            if (basetype(tnew)->sp->sb->templateLevel || basetype(told)->sp->sb->templateLevel)
            {
                TYPE* tps = basetype(told)->btp;
                TYPE* tpn = basetype(tnew)->btp;
                if ((!templatecomparetypes(tpn, tps, true) ||
                     ((tps->type == BasicType::templateselector_ || tpn->type == BasicType::templateselector_) && tpn->type != tps->type)) &&
                    !sameTemplate(tpn, tps))
                {
                    if (isref(tps))
                        tps = basetype(tps)->btp;
                    if (isref(tpn))
                        tpn = basetype(tpn)->btp;
                    while (ispointer(tpn) && ispointer(tps))
                    {
                        if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                            return false;
                        tpn = basetype(tpn)->btp;
                        tps = basetype(tps)->btp;
                    }
                    if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                        if (basetype(tpn)->type != BasicType::templateselector_)
                            return false;
                    tpn = basetype(tpn);
                    tps = basetype(tps);
                    if (comparetypes(tpn, tps, true) || (tpn->type == BasicType::templateparam_ && tps->type == BasicType::templateparam_))
                    {
                        return true;
                    }
                    else if (isarithmetic(tpn) && isarithmetic(tps))
                    {
                        return false;
                    }
                    else if (tpn->type == BasicType::templateselector_)
                    {
                        if (tps->type == BasicType::templateselector_)
                        {
                            if (!templateselectorcompare(tpn->sp->sb->templateSelector, tps->sp->sb->templateSelector))
                            {
                                auto ts1 = (*tpn->sp->sb->templateSelector).begin();
                                auto ts1e = (*tpn->sp->sb->templateSelector).end();
                                auto ts2 = (*tps->sp->sb->templateSelector).begin();
                                auto ts2e = (*tps->sp->sb->templateSelector).end();
                                ++ts1;
                                ++ts2;
                                if (ts2->sp->sb && ts2->sp->sb->typedefSym)
                                {
                                    ++ts1;
                                    if (!strcmp(ts1->name, ts2->sp->sb->typedefSym->name))
                                    {
                                        ++ts1;
                                        ++ts2;
                                        for (++ts1, ++ts2; ts1 != ts1e && ts2 != ts2e; ++ts1, ++ts2)
                                            if (strcmp(ts1->name, ts2->name))
                                                return false;
                                        if (ts1 != ts1e || ts2 != ts2e)
                                            return false;
                                    }
                                }
                                else if (!strcmp((*tpn->sp->sb->templateSelector)[1].sp->name,
                                                 (*tps->sp->sb->templateSelector)[1].sp->name))
                                {
                                    if ((*tpn->sp->sb->templateSelector)[2].name[0])
                                        return false;
                                }
                                else if (basetype(told)->btp->type == BasicType::typedef_ &&
                                         strcmp(basetype(told)->btp->sp->name, (*tpn->sp->sb->templateSelector)[2].name) ==
                                             0)
                                {
                                }
                                else
                                {
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            auto tpl = basetype(tpn)->sp->sb->templateSelector;
                            SYMBOL* sym = (*tpl)[1].sp;
                            if ((*tpl)[1].isDeclType)
                            {
                                TYPE* tp1 = TemplateLookupTypeFromDeclType((*tpl)[1].tp);
                                if (tp1 && isstructured(tp1))
                                    sym = basetype(tp1)->sp;
                                else
                                    sym = nullptr;
                            }
                            auto find = (*tpl).begin();
                            auto finde = (*tpl).end();
                            for (++find, ++find; sym && find != finde; ++find)
                            {
                                SYMBOL* fsp;
                                if (!isstructured(sym->tp))
                                    break;

                                fsp = search(basetype(sym->tp)->syms, find->name);
                                if (!fsp)
                                {
                                    fsp = classdata(find->name, basetype(sym->tp)->sp, nullptr, false, false);
                                    if (fsp == (SYMBOL*)-1)
                                        fsp = nullptr;
                                }
                                sym = fsp;
                            }
                            if (find != finde || !sym || (!comparetypes(sym->tp, tps, true) && !sameTemplate(sym->tp, tps)))
                                return false;
                        }
                        return true;
                    }
                    else if ((tpn->type == BasicType::templateparam_ || tps->type == BasicType::templateparam_) && tpn->type != tps->type)
                    {
                        return false;
                    }
                    return true;
                }
                if (tpn->type == BasicType::templateselector_ && tps->type == BasicType::templateselector_)
                {
                    auto ts1 = tpn->sp->sb->templateSelector;
                    auto ts2 = tps->sp->sb->templateSelector;
                    if ((*ts1)[1].isTemplate != (*ts2)[1].isTemplate ||
                        strcmp((*ts1)[1].sp->sb->decoratedName, (*ts2)[1].sp->sb->decoratedName))
                        return false;
                    auto tss1 = (*ts1).begin();
                    auto tss2 = (*ts2).begin();
                    for (++tss1, ++tss2; tss1 != (*ts1).end() && tss2 != (*ts2).end(); ++ tss1, ++ tss2)
                    {
                        if (strcmp(tss1->name, tss2->name))
                            return false;
                    }
                    if (tss1 != (*ts1).end() || tss2 != (*ts2).end())
                        return false;
                    if ((*ts1)[1].isTemplate)
                    {
                        if (!exactMatchOnTemplateParams((*ts1)[1].templateParams, (*ts2)[1].templateParams))
                            return false;
                    }
                    return true;
                }
            }
            else if (basetype(tnew)->sp->sb->castoperator)
            {
                TYPE* tps = basetype(told)->btp;
                TYPE* tpn = basetype(tnew)->btp;
                if (!templatecomparetypes(tpn, tps, true) && !sameTemplate(tpn, tps))
                    return false;
            }
            return true;
        }
        else
        {
            return true;
        }
    }
    return false;
}
SYMBOL* searchOverloads(SYMBOL* sym, SymbolTable<SYMBOL>* table)
{
    if (Optimizer::cparams.prm_cplusplus)
    {
        for (auto spp : *table)
        { 
            if (matchOverload(sym->tp, spp->tp, false))
            {
                if (!spp->templateParams)
                    return spp;
                if (sym->sb->templateLevel == spp->sb->templateLevel ||
                    (sym->sb->templateLevel && !spp->sb->templateLevel && sym->templateParams->size() == 1))
                        return spp;

                if (!!spp->templateParams == !!sym->templateParams)
                {
                    auto tpl = spp->templateParams->begin();
                    auto tple = spp->templateParams->end();
                    ++tpl;
                    auto tpr = sym->templateParams->begin();
                    auto tpre = sym->templateParams->end();
                    ++tpr;
                    for  ( ; tpl != tple  && tpr != tpre; ++tpl, ++tpr)
                    {
                        if (tpl->second->type == TplType::int_ && tpl->second->byNonType.tp->type == BasicType::templateselector_)
                            break;
                        if (tpr->second->type == TplType::int_ && tpr->second->byNonType.tp->type == BasicType::templateselector_)
                            break;
                    }
                    if (tpl == tple && tpr == tpre)
                        return spp;
                }
            }
        }
    }
    else
    {
        return table->front();
    }
    return (0);
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
    return search(globalNameSpace->front()->tags,name);
}
}  // namespace Parser