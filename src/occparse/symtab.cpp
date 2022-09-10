/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
namespace CompletionCompiler
{
extern Parser::SymbolTable<Parser::SYMBOL>* ccSymbols;
void ccSetSymbol(Parser::SYMBOL* s);
}  // namespace CompletionCompiler
namespace Parser
{

NAMESPACEVALUELIST *globalNameSpace, *localNameSpace;
SymbolTable<SYMBOL>* labelSyms;

int matchOverloadLevel;
SymbolTableFactory<SYMBOL> symbols;

static Optimizer::LIST* usingDirectives;

void syminit(void)
{
    globalNameSpace = Allocate<NAMESPACEVALUELIST>();
    globalNameSpace->valueData = Allocate<NAMESPACEVALUEDATA>();
    globalNameSpace->valueData->syms = symbols.CreateSymbolTable();
    globalNameSpace->valueData->tags = symbols.CreateSymbolTable();

    localNameSpace = Allocate<NAMESPACEVALUELIST>();
    localNameSpace->valueData = Allocate<NAMESPACEVALUEDATA>();
    usingDirectives = nullptr;
    matchOverloadLevel = 0;
    symbols.Reset();
}
void AllocateLocalContext(BLOCKDATA* block, SYMBOL* sym, int label)
{
    SymbolTable<SYMBOL>* tn = symbols.CreateSymbolTable();
    STATEMENT* st;
    Optimizer::LIST* l;
    st = stmtNode(nullptr, block, st_dbgblock);
    st->label = 1;
    if (block && Optimizer::cparams.prm_debug)
    {
        st = stmtNode(nullptr, block, st_label);
        st->label = label;
    }
    tn->Next(localNameSpace->valueData->syms);
    tn->Chain(localNameSpace->valueData->syms);

    localNameSpace->valueData->syms = tn;
    tn = symbols.CreateSymbolTable();
    tn->Next(localNameSpace->valueData->tags);
    tn->Chain(localNameSpace->valueData->tags);
    localNameSpace->valueData->tags = tn;
    if (sym)
        localNameSpace->valueData->tags->Block(sym->sb->value.i++);

    l = Allocate<Optimizer::LIST>();
    l->data = localNameSpace->valueData->usingDirectives;
    l->next = usingDirectives;
    usingDirectives = l;
}
void TagSyms(SymbolTable<SYMBOL>* syms)
{
    int i;
    for (auto sym : *syms)
    {
        sym->sb->ccEndLine = preProcessor->GetRealLineNo() + 1;
    }
}
void FreeLocalContext(BLOCKDATA* block, SYMBOL* sym, int label)
{
    SymbolTable<SYMBOL>* locals = localNameSpace->valueData->syms;
    SymbolTable<SYMBOL>* tags = localNameSpace->valueData->tags;
    STATEMENT* st;
    if (block && Optimizer::cparams.prm_debug)
    {
        st = stmtNode(nullptr, block, st_label);
        st->label = label;
    }
    checkUnused(localNameSpace->valueData->syms);
    if (sym)
        sym->sb->value.i--;

    st = stmtNode(nullptr, block, st_expr);
    destructBlock(&st->select, localNameSpace->valueData->syms, true);
    localNameSpace->valueData->syms = localNameSpace->valueData->syms->Next();
    localNameSpace->valueData->tags = localNameSpace->valueData->tags->Next();

    localNameSpace->valueData->usingDirectives = (Optimizer::LIST*)usingDirectives->data;
    usingDirectives = usingDirectives->next;

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
    st = stmtNode(nullptr, block, st_dbgblock);
    st->label = 0;
}

/* SYMBOL tab hash function */
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
        if (told->type != bt_any || tnew->type != bt_any)  // packed template param
        {
            if ((!comparetypes(told, tnew, true) && !sameTemplatePointedTo(told, tnew, true) && !sameTemplateSelector(told, tnew)) || told->type != tnew->type)
                break;
            else
            {
                TYPE* tps = sold->tp;
                TYPE* tpn = snew->tp;
                if (isref(tps))
                    tps = basetype(tps)->btp;
                if (isref(tpn))
                    tpn = basetype(tpn)->btp;
                while (ispointer(tpn) && ispointer(tps))
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
                tpn = basetype(tpn);
                tps = basetype(tps);
                if (tpn->type == bt_templateparam)
                {
                    if (tps->type != bt_templateparam)
                        break;
                    if (tpn->templateParam->p->packed != tps->templateParam->p->packed)
                        break;
                    tableOld[tCount] = GetHashValue(tps->templateParam->argsym->name);
                    tableNew[tCount] = GetHashValue(tpn->templateParam->argsym->name);
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
            TEMPLATEPARAMLIST *tplNew, *tplOld;
            int iCount = 0;
            unsigned oldIndex[100], newIndex[100];
            tplNew = fnew && fnew->templateParams ? fnew->templateParams : nullptr;
            tplOld = fold && fold->templateParams ? fold->templateParams : nullptr;
            if (tplNew)
                tplNew = tplNew->p->bySpecialization.types ? tplNew->p->bySpecialization.types : tplNew->next;
            if (tplOld)
                tplOld = tplOld->p->bySpecialization.types ? tplOld->p->bySpecialization.types : tplOld->next;
            while (tplNew && tplOld)
            {
                if (tplOld->argsym && tplNew->argsym)
                {
                    oldIndex[iCount] = GetHashValue(tplOld->argsym->name);
                    newIndex[iCount] = GetHashValue(tplNew->argsym->name);
                    iCount++;
                }
                tplNew = tplNew->next;
                tplOld = tplOld->next;
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
                     ((tps->type == bt_templateselector || tpn->type == bt_templateselector) && tpn->type != tps->type)) &&
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
                        if (basetype(tpn)->type != bt_templateselector)
                            return false;
                    tpn = basetype(tpn);
                    tps = basetype(tps);
                    if (comparetypes(tpn, tps, true) || (tpn->type == bt_templateparam && tps->type == bt_templateparam))
                    {
                        return true;
                    }
                    else if (isarithmetic(tpn) && isarithmetic(tps))
                    {
                        return false;
                    }
                    else if (tpn->type == bt_templateselector)
                    {
                        if (tps->type == bt_templateselector)
                        {
                            if (!templateselectorcompare(tpn->sp->sb->templateSelector, tps->sp->sb->templateSelector))
                            {
                                TEMPLATESELECTOR* ts1 = tpn->sp->sb->templateSelector->next;
                                TEMPLATESELECTOR* ts2 = tps->sp->sb->templateSelector->next;
                                if (ts2->sp->sb && ts2->sp->sb->typedefSym)
                                {
                                    ts1 = ts1->next;
                                    if (!strcmp(ts1->name, ts2->sp->sb->typedefSym->name))
                                    {
                                        ts1 = ts1->next;
                                        ts2 = ts2->next;
                                        while (ts1 && ts2)
                                        {
                                            if (strcmp(ts1->name, ts2->name))
                                                return false;
                                            ts1 = ts1->next;
                                            ts2 = ts2->next;
                                        }
                                        if (ts1 || ts2)
                                            return false;
                                    }
                                }
                                else if (!strcmp(tpn->sp->sb->templateSelector->next->sp->name,
                                                 tps->sp->sb->templateSelector->next->sp->name))
                                {
                                    if (tpn->sp->sb->templateSelector->next->next->name[0])
                                        return false;
                                }
                                else if (basetype(told)->btp->type == bt_typedef &&
                                         strcmp(basetype(told)->btp->sp->name, tpn->sp->sb->templateSelector->next->next->name) ==
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
                            TEMPLATESELECTOR* tpl = basetype(tpn)->sp->sb->templateSelector->next;
                            SYMBOL* sym = tpl->sp;
                            TEMPLATESELECTOR* find = tpl->next;
                            if (tpl->isDeclType)
                            {
                                TYPE* tp1 = TemplateLookupTypeFromDeclType(tpl->tp);
                                if (tp1 && isstructured(tp1))
                                    sym = basetype(tp1)->sp;
                                else
                                    sym = nullptr;
                            }
                            while (sym && find)
                            {
                                SYMBOL* fsp;
                                if (!isstructured(sym->tp))
                                    break;

                                fsp = basetype(sym->tp)->syms->search(find->name);
                                if (!fsp)
                                {
                                    fsp = classdata(find->name, basetype(sym->tp)->sp, nullptr, false, false);
                                    if (fsp == (SYMBOL*)-1)
                                        fsp = nullptr;
                                }
                                sym = fsp;
                                find = find->next;
                            }
                            if (find || !sym || (!comparetypes(sym->tp, tps, true) && !sameTemplate(sym->tp, tps)))
                                return false;
                        }
                        return true;
                    }
                    else if ((tpn->type == bt_templateparam || tps->type == bt_templateparam) && tpn->type != tps->type)
                    {
                        return false;
                    }
                    return true;
                }
                if (tpn->type == bt_templateselector && tps->type == bt_templateselector)
                {
                    TEMPLATESELECTOR *ts1 = tpn->sp->sb->templateSelector->next, *tss1;
                    TEMPLATESELECTOR *ts2 = tps->sp->sb->templateSelector->next, *tss2;
                    if (ts1->isTemplate != ts2->isTemplate || strcmp(ts1->sp->sb->decoratedName, ts2->sp->sb->decoratedName))
                        return false;
                    tss1 = ts1->next;
                    tss2 = ts2->next;
                    while (tss1 && tss2)
                    {
                        if (strcmp(tss1->name, tss2->name))
                            return false;
                        tss1 = tss1->next;
                        tss2 = tss2->next;
                    }
                    if (tss1 || tss2)
                        return false;
                    if (ts1->isTemplate)
                    {
                        if (!exactMatchOnTemplateParams(ts1->templateParams, ts2->templateParams))
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
                    (sym->sb->templateLevel && !spp->sb->templateLevel && !sym->templateParams->next))
                    return spp;

                if (!!spp->templateParams == !!sym->templateParams)
                {
                    TEMPLATEPARAMLIST* tpl = spp->templateParams->next;
                    TEMPLATEPARAMLIST* tpr = sym->templateParams->next;
                    while (tpl && tpr)
                    {
                        if (tpl->p->type == kw_int && tpl->p->byNonType.tp->type == bt_templateselector)
                            break;
                        if (tpr->p->type == kw_int && tpr->p->byNonType.tp->type == bt_templateselector)
                            break;
                        //    if (tpl->argsym->sb->compilerDeclared || tpr->argsym->sb->compilerDeclared)
                        //        break;
                        tpl = tpl->next;
                        tpr = tpr->next;
                    }
                    if (!tpl && !tpr)
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
    SYMBOL* sym = localNameSpace->valueData->syms->search(name);
    if (sym)
        return sym;
    return globalNameSpace->valueData->syms->search(name);
}
SYMBOL* tsearch(const char* name)
{
    SYMBOL* sym = localNameSpace->valueData->tags->search(name);
    if (sym)
        return sym;
    return globalNameSpace->valueData->tags->search(name);
}
}  // namespace Parser