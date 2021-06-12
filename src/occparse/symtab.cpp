/* Software License Agreement
 * 
 *     Copyright(C) 1994-2021 David Lindauer, (LADSoft)
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

namespace CompletionCompiler
{
extern Parser::HASHTABLE* ccHash;
void ccSetSymbol(Parser::SYMBOL* s);
}  // namespace CompletionCompiler
namespace Parser
{

    NAMESPACEVALUELIST* globalNameSpace, * localNameSpace;
    HASHTABLE* labelSyms;

    HASHTABLE* CreateHashTable(int size);
    int matchOverloadLevel;

    static Optimizer::LIST* usingDirectives;

    void syminit(void)
    {
        globalNameSpace = Allocate<NAMESPACEVALUELIST>();
        globalNameSpace->valueData = Allocate<NAMESPACEVALUEDATA>();
        globalNameSpace->valueData->syms = CreateHashTable(GLOBALHASHSIZE);
        globalNameSpace->valueData->tags = CreateHashTable(GLOBALHASHSIZE);

        localNameSpace = Allocate<NAMESPACEVALUELIST>();
        localNameSpace->valueData = Allocate<NAMESPACEVALUEDATA>();
        usingDirectives = nullptr;
        matchOverloadLevel = 0;
    }
    HASHTABLE* CreateHashTable(int size)
    {
        HASHTABLE* rv = Allocate<HASHTABLE>();
        rv->table = Allocate<SYMLIST*>(size);
        rv->size = size;
        return rv;
    }
    void AllocateLocalContext(BLOCKDATA* block, SYMBOL* sym, int label)
    {
        HASHTABLE* tn = CreateHashTable(1);
        STATEMENT* st;
        Optimizer::LIST* l;
        st = stmtNode(nullptr, block, st_dbgblock);
        st->label = 1;
        if (block && Optimizer::cparams.prm_debug)
        {
            st = stmtNode(nullptr, block, st_label);
            st->label = label;
        }
        tn->next = tn->chain = localNameSpace->valueData->syms;
        localNameSpace->valueData->syms = tn;
        tn = CreateHashTable(1);
        tn->next = tn->chain = localNameSpace->valueData->tags;
        localNameSpace->valueData->tags = tn;
        if (sym)
            localNameSpace->valueData->tags->blockLevel = sym->sb->value.i++;

        l = Allocate<Optimizer::LIST>();
        l->data = localNameSpace->valueData->usingDirectives;
        l->next = usingDirectives;
        usingDirectives = l;
    }
    void TagSyms(HASHTABLE* syms)
    {
        int i;
        for (i = 0; i < syms->size; i++)
        {
            SYMLIST* hr = syms->table[i];
            while (hr)
            {
                SYMBOL* sym = hr->p;
                sym->sb->ccEndLine = preProcessor->GetRealLineNo() + 1;
                hr = hr->next;
            }
        }
    }
    void FreeLocalContext(BLOCKDATA* block, SYMBOL* sym, int label)
    {
        HASHTABLE* locals = localNameSpace->valueData->syms;
        HASHTABLE* tags = localNameSpace->valueData->tags;
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
        destructBlock(&st->select, localNameSpace->valueData->syms->table[0], true);
        localNameSpace->valueData->syms = localNameSpace->valueData->syms->next;
        localNameSpace->valueData->tags = localNameSpace->valueData->tags->next;

        localNameSpace->valueData->usingDirectives = (Optimizer::LIST*)usingDirectives->data;
        usingDirectives = usingDirectives->next;

        if (!IsCompiler())
        {
            TagSyms(locals);
            TagSyms(tags);
        }
        if (sym)
        {
            locals->next = sym->sb->inlineFunc.syms;
            tags->next = sym->sb->inlineFunc.tags;
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
    SYMLIST** GetHashLink(HASHTABLE* t, const char* string)
    {
        unsigned i;
        if (t->size == 1)
            return &t->table[0];
        for (i = 0; *string; string++)
            i = ((i << 7) + (i << 1) + i) ^ *string;
        return &t->table[i % t->size];
    }
    /* Add a hash item to the table */
    SYMLIST* AddName(SYMBOL* item, HASHTABLE* table)
    {
        SYMLIST** p = GetHashLink(table, item->name);
        SYMLIST* newRec;

        if (*p)
        {
            SYMLIST* q = *p, * r = *p;
            while (q)
            {
                r = q;
                if (!strcmp(r->p->name, item->name))
                    return (r);
                q = q->next;
            }
            newRec = Allocate<SYMLIST>();
            r->next = newRec;
            newRec->p = (SYMBOL*)item;
        }
        else
        {
            newRec = Allocate<SYMLIST>();
            *p = newRec;
            newRec->p = (SYMBOL*)item;
        }
        return (0);
    }
    SYMLIST* AddOverloadName(SYMBOL* item, HASHTABLE* table)
    {
        SYMLIST** p = GetHashLink(table, item->sb->decoratedName);
        SYMLIST* newRec;
        if (!IsCompiler() && !item->parserSet)
        {
            item->parserSet = true;
            CompletionCompiler::ccSetSymbol(item);
        }

        if (*p)
        {
            SYMLIST* q = *p, * r = *p;
            while (q)
            {
                r = q;
                if (!strcmp(((SYMBOL*)r->p)->sb->decoratedName, item->sb->decoratedName))
                    return (r);
                q = q->next;
            }
            newRec = Allocate<SYMLIST>();
            r->next = newRec;
            newRec->p = (SYMBOL*)item;
        }
        else
        {
            newRec = Allocate<SYMLIST>();
            *p = newRec;
            newRec->p = (SYMBOL*)item;
        }
        return (0);
    }

    /*
     * Find something in the hash table
     */
    SYMLIST** LookupName(const char* name, HASHTABLE* table)
    {
        if (table->fast)
        {
            table = table->fast;
        }
        SYMLIST** p = GetHashLink(table, name);

        while (*p)
        {
            if (!strcmp((*p)->p->name, name))
            {
                return p;
            }
            p = (SYMLIST**)*p;
        }
        return (0);
    }
    SYMBOL* search(const char* name, HASHTABLE* table)
    {
        while (table)
        {
            SYMLIST** p = LookupName(name, table);
            if (p)
                return (SYMBOL*)(*p)->p;
            table = table->next;
        }
        return nullptr;
    }
    bool matchOverload(TYPE* tnew, TYPE* told, bool argsOnly)
    {
        SYMLIST* hnew = basetype(tnew)->syms->table[0];
        SYMLIST* hold = basetype(told)->syms->table[0];
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
        while (hnew && hold)
        {
            SYMBOL* snew = (SYMBOL*)hnew->p;
            SYMBOL* sold = (SYMBOL*)hold->p;
            TYPE* tnew, * told;
            if (sold->sb->thisPtr)
            {
                hold = hold->next;
                if (!hold)
                    break;
                sold = (SYMBOL*)hold->p;
            }
            if (snew->sb->thisPtr)
            {
                hnew = hnew->next;
                if (!hnew)
                    break;
                snew = (SYMBOL*)hnew->p;
            }
            tnew = basetype(snew->tp);
            told = basetype(sold->tp);
            if (told->type != bt_any || tnew->type != bt_any)  // packed template param
            {
                if ((!comparetypes(told, tnew, true) && !sameTemplatePointedTo(told, tnew)) || told->type != tnew->type)
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
            hold = hold->next;
            hnew = hnew->next;
        }
        matchOverloadLevel--;
        if (!hold && !hnew)
        {
            if (tCount)
            {
                int i, j;
                SYMBOL* fnew = basetype(tnew)->sp->sb->parentClass;
                SYMBOL* fold = basetype(told)->sp->sb->parentClass;
                TEMPLATEPARAMLIST* tplNew, * tplOld;
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

                                    fsp = search(find->name, basetype(sym->tp)->syms);
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
                        else if (tpn->type == bt_templatedecltype && tps->type == bt_templatedecltype)
                        {
                            return templatecompareexpressions(tpn->templateDeclType, tps->templateDeclType);
                        }
                        else if ((tpn->type == bt_templateparam || tps->type == bt_templateparam) && tpn->type != tps->type)
                        {
                            return false;
                        }
                        return true;
                    }
                    if (tpn->type == bt_templateselector && tps->type == bt_templateselector)
                    {
                        TEMPLATESELECTOR* ts1 = tpn->sp->sb->templateSelector->next, * tss1;
                        TEMPLATESELECTOR* ts2 = tps->sp->sb->templateSelector->next, * tss2;
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
                        return templateselectorcompare(tpn->sp->sb->templateSelector, tps->sp->sb->templateSelector);  // unreachable
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
    SYMBOL* searchOverloads(SYMBOL* sym, HASHTABLE* table)
    {
        SYMLIST* p = table->table[0];
        if (Optimizer::cparams.prm_cplusplus)
        {
            while (p)
            {
                SYMBOL* spp = (SYMBOL*)p->p;
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
                p = p->next;
            }
        }
        else
        {
            return (SYMBOL*)p->p;
        }
        return (0);
    }
    SYMBOL* gsearch(const char* name)
    {
        SYMBOL* sym = search(name, localNameSpace->valueData->syms);
        if (sym)
            return sym;
        return search(name, globalNameSpace->valueData->syms);
    }
    SYMBOL* tsearch(const char* name)
    {
        SYMBOL* sym = search(name, localNameSpace->valueData->tags);
        if (sym)
            return sym;
        return search(name, globalNameSpace->valueData->tags);
    }
    void baseinsert(SYMBOL* in, HASHTABLE* table)
    {
        if (Optimizer::cparams.prm_extwarning)
            if (in->sb->storage_class == sc_parameter || in->sb->storage_class == sc_auto || in->sb->storage_class == sc_register)
            {
                SYMBOL* sym;
                if ((sym = gsearch(in->name)) != nullptr)
                    preverror(ERR_VARIABLE_OBSCURES_VARIABLE_AT_HIGHER_SCOPE, in->name, sym->sb->declfile, sym->sb->declline);
            }
        if (AddName(in, table) && (IsCompiler() || table != CompletionCompiler::ccHash))
        {
            if (!IsCompiler())
            {
                SYMBOL* sym = search(in->name, table);
                    if (!sym || !sym->sb->wasUsing || !in->sb->wasUsing)
                        preverrorsym(ERR_DUPLICATE_IDENTIFIER, in, in->sb->declfile, in->sb->declline);
            }
            else
            {
                if (!structLevel || !templateNestingCount)
                {
                    SYMBOL* sym = search(in->name, table);
                        if (!sym || !sym->sb->wasUsing || !in->sb->wasUsing)
                            preverrorsym(ERR_DUPLICATE_IDENTIFIER, in, in->sb->declfile, in->sb->declline);
                }
            }
        }
    }
    void insert(SYMBOL* in, HASHTABLE* table)
    {
        if (table)
        {
            if (!IsCompiler())
            {
                if (table != kwhash && table != CompletionCompiler::ccHash)
                    if (!in->parserSet)
                    {
                        in->parserSet = true;
                        CompletionCompiler::ccSetSymbol(in);
                    }
            }
            baseinsert(in, table);
        }
        else
        {
            diag("insert: cannot insert");
        }
    }

void insertOverload(SYMBOL* in, HASHTABLE* table)
{

    if (Optimizer::cparams.prm_extwarning)
        if (in->sb->storage_class == sc_parameter || in->sb->storage_class == sc_auto || in->sb->storage_class == sc_register)
        {
            SYMBOL* sym;
            if ((sym = gsearch(in->name)) != nullptr)
                preverror(ERR_VARIABLE_OBSCURES_VARIABLE_AT_HIGHER_SCOPE, in->name, sym->sb->declfile, sym->sb->declline);
        }
    if (AddOverloadName(in, table))
    {
        SetLinkerNames(in, lk_cdecl);
        preverrorsym(ERR_DUPLICATE_IDENTIFIER, in, in->sb->declfile, in->sb->declline);
    }
}
}  // namespace Parser