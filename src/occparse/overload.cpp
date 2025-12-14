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
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "constopt.h"
#include "Utils.h"
#include "SymbolProperties.h"

namespace Parser
{
int inGetUserConversion;
int currentlyInsertingFunctions;
int inNothrowHandler;
int matchOverloadLevel;

static int insertFuncs(SYMBOL** spList, std::list<SYMBOL*>& gather, CallSite* args, Type* atp, int flags);

static const int rank[] = {0, 1, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6, 7, 8, 8, 9};
static bool getFuncConversions(SYMBOL* sym, CallSite* f, Type* atp, SYMBOL* parent, e_cvsrn arr[], int* sizes, int count,
                               SYMBOL** userFunc, bool usesInitList, bool deduceTemplate);
static void WeedTemplates(SYMBOL** table, int count, CallSite* args, Type* atp);

/* SYMBOL tab Keyword::hash_ function */
static int GetHashValue(const char* string)
{
    unsigned i;
    for (i = 0; *string; string++)
        i = ((i << 7) + (i << 1) + i) ^ *string;
    return i;
}

static bool EmptyPackedTemplateParam(Type* tnew, Type* told)
{
    while (tnew->IsRef()) tnew = tnew->BaseType()->btp;
    while (told->IsRef()) told = told->BaseType()->btp;
    tnew = tnew->BaseType();
    told = told->BaseType();
    if (tnew->type == BasicType::templateparam_ && told->type == BasicType::templateparam_)
    {
        if (tnew->templateParam->second->packed && told->templateParam->second->packed)
        {
            if ((!tnew->templateParam->second->byPack.pack || !tnew->templateParam->second->byPack.pack->size()) &&
                (!told->templateParam->second->byPack.pack || !told->templateParam->second->byPack.pack->size()))
            {
                return true;
            }
        }
    }
    return false;

}
bool matchOverload(Type* tnew, Type* told)
{
    auto funcold = told->BaseType();
    auto hnew = tnew->BaseType()->syms->begin();
    auto hnewe = tnew->BaseType()->syms->end();
    auto hold = told->BaseType()->syms->begin();
    auto holde = told->BaseType()->syms->end();
    unsigned tableOld[100], tableNew[100];
    int tCount = 0;
    if (tnew->IsConst() != told->IsConst())
        return false;
    if (tnew->IsVolatile() != told->IsVolatile())
        return false;
    if (tnew->IsLRefQual() != told->IsLRefQual())
        return false;
    if (tnew->IsRRefQual() != told->IsRRefQual())
        return false;
    matchOverloadLevel++;
    while (hnew != hnewe && hold != holde)
    {
        SYMBOL* snew = *hnew;
        SYMBOL* sold = *hold;
        Type *tnew, *told;
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
        tnew = snew->tp->BaseType();
        told = sold->tp->BaseType();
        told = sold->tp->BaseType();
        if (funcold->sp && !funcold->sp->sb->templateLevel)
            told->InstantiateDeferred();
        if (told->type != BasicType::any_ || tnew->type != BasicType::any_)  // packed template param
        {
            if ((told->type != tnew->type || (!told->CompatibleType(tnew) && !SameTemplatePointedTo(tnew, told, true))) &&
                !SameTemplateSelector(tnew, told) && !EmptyPackedTemplateParam(tnew, told))
                break;
            else
            {
                bool matchconst = false;
                Type* tps = sold->tp;
                Type* tpn = snew->tp;
                if (tps->IsRef())
                {
                    matchconst = true;
                    tps = tps->BaseType()->btp;
                }
                if (tpn->IsRef())
                {
                    matchconst = true;
                    tpn = tpn->BaseType()->btp;
                }
                while (tps != tps->rootType && tps->type != BasicType::typedef_ && tps->type != BasicType::const_ &&
                       tps->type != BasicType::volatile_)
                    tps = tps->btp;
                while (tpn != tpn->rootType && tpn->type != BasicType::typedef_ && tpn->type != BasicType::const_ &&
                       tpn->type != BasicType::volatile_)
                    tpn = tpn->btp;
                if (tpn->BaseType()->nullptrType != tps->BaseType()->nullptrType)
                {
                    matchOverloadLevel--;
                    return false;
                }
                if (tpn->IsArray() && tps->IsArray())
                {
                    if (!!tpn->BaseType()->esize != !!tps->BaseType()->esize ||
                        (tpn->BaseType()->esize && isintconst(tpn->BaseType()->esize) != isintconst(tps->BaseType()->esize)))
                    {
                        matchOverloadLevel--;
                        return false;
                    }
                }
                if (tpn->type != BasicType::typedef_ && tps->type != BasicType::typedef_ && (tpn->IsPtr() || tps->IsPtr()))
                {

                    while (tpn->IsPtr() && tps->IsPtr() && tpn->type != BasicType::typedef_ && tps->type != BasicType::typedef_)
                    {
                        if (tpn->IsConst() != tps->IsConst() || tpn->IsVolatile() != tps->IsVolatile())
                        {
                            matchOverloadLevel--;
                            return false;
                        }
                        tpn = tpn->BaseType()->btp;
                        tps = tps->BaseType()->btp;
                    }
                    if (tpn->IsConst() != tps->IsConst() || tpn->IsVolatile() != tps->IsVolatile())
                    {
                        matchOverloadLevel--;
                        return false;
                    }
                }
                else if (matchconst)
                {
                    if (tpn->IsConst() != tps->IsConst() || tpn->IsVolatile() != tps->IsVolatile())
                    {
                        matchOverloadLevel--;
                        return false;
                    }
                }
                tpn = tpn->BaseType();
                tps = tps->BaseType();

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
    if (!IsDefiningTemplate())
    {
        if (hold != holde)
        {
            // if  the old one has a packed template parame with no elements we are safe...
            auto tpx = (*hold)->tp;
            while (tpx->IsRef())
                tpx = tpx->BaseType()->btp;
            tpx = tpx->BaseType();
            if (tpx->type == BasicType::void_ || (tpx->type == BasicType::templateparam_ && tpx->templateParam->second->packed))
            {
                if (tpx->type == BasicType::void_ || !tpx->templateParam->second->byPack.pack || !tpx->templateParam->second->byPack.pack->size())
                {
                    if (hnew != hnewe)
                    {
                        tpx = (*hnew)->tp;
                        while (tpx->IsRef())
                            tpx = tpx->BaseType()->btp;
                        tpx = tpx->BaseType();
                        if (tpx->type == BasicType::void_ || tpx->type == BasicType::templateparam_ && tpx->templateParam->second->packed)
                        {
                            if (tpx->type == BasicType::void_ || !tpx->templateParam->second->byPack.pack || !tpx->templateParam->second->byPack.pack->size())
                            {
                                hold = holde;
                                hnew = hnewe;
                            }
                        }
                    }
                    else
                    {
                        hold = holde;
                    }
                }
            }
        }
        else if (hnew != hnewe)
        {
            auto tpx = (*hnew)->tp;
            while (tpx->IsPtr() || tpx->IsRef())
                tpx = tpx->BaseType()->btp;
            tpx = tpx->BaseType();
            if (tpx->type == BasicType::templateparam_ && tpx->templateParam->second->packed)
            {
                if (!tpx->templateParam->second->byPack.pack || !tpx->templateParam->second->byPack.pack->size())
                {
                    hnew = hnewe;
                }
            }
        }
    }
    if (hold == holde && hnew == hnewe)
    {
        if (tCount)
        {
            int i, j;
            SYMBOL* fnew = tnew->BaseType()->sp->sb->parentClass;
            SYMBOL* fold = told->BaseType()->sp->sb->parentClass;
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
            for (; itNew != iteNew && itOld != iteOld; ++itNew, ++itOld)
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
        if (tnew->BaseType()->sp && told->BaseType()->sp)
        {
            if (tnew->BaseType()->sp->sb->templateLevel || told->BaseType()->sp->sb->templateLevel)
            {
                Type* tps = told->BaseType()->btp;
                Type* tpn = tnew->BaseType()->btp;
                if (tps->IsDeferred() == tpn->IsDeferred())
                {
                    if (tps->IsDeferred())
                    {
                        if (!SameTemplate(tps->sp->tp, tpn->sp->tp))
                            return false;
                        if (!exactMatchOnTemplateArgs(tps->templateArgs, tpn->templateArgs))
                            return false;
                    }
                }
                else
                {
                    return false;
                }
                if ((!templateCompareTypes(tpn, tps, true) ||
                     ((tps->type == BasicType::templateselector_ || tpn->type == BasicType::templateselector_) &&
                      tpn->type != tps->type)) &&
                    !SameTemplate(tpn, tps))
                {
                    if (tps->IsRef())
                        tps = tps->BaseType()->btp;
                    if (tpn->IsRef())
                        tpn = tpn->BaseType()->btp;
                    if (tps->type == BasicType::auto_ || tpn->type == BasicType::auto_)
                        return true;
                    while (tpn->IsPtr() && tps->IsPtr())
                    {
                        if (tpn->IsConst() != tps->IsConst() || tpn->IsVolatile() != tps->IsVolatile())
                            return false;
                        tpn = tpn->BaseType()->btp;
                        tps = tps->BaseType()->btp;
                    }
                    if (tpn->IsConst() != tps->IsConst() || tpn->IsVolatile() != tps->IsVolatile())
                        if (tpn->BaseType()->type != BasicType::templateselector_)
                            return false;
                    SYMBOL* typedefSym = nullptr;
                    if (tps->type == BasicType::typedef_)
                    {
                        typedefSym = tps->sp;
                    }
                    tpn = tpn->BaseType();
                    tps = tps->BaseType();
                    if (tpn->CompatibleType(tps) ||
                        (tpn->type == BasicType::templateparam_ && tps->type == BasicType::templateparam_))
                    {
                        return true;
                    }
                    else if (tpn->IsArithmetic() && tps->IsArithmetic())
                    {
                        return false;
                    }
                    else if (tpn->type == BasicType::void_ || tps->type == BasicType::void_)
                    {
                        if (tpn->type != BasicType::templateselector_)
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
                                if (typedefSym)
                                {
                                    if (strcmp((*tpn->sp->sb->templateSelector)[2].name,
                                        typedefSym->name) == 0)
                                    {
                                    }
                                    else
                                    {
                                        return false;
                                    }

                                }
                                else
                                {
                                    if (strcmp((*tpn->sp->sb->templateSelector)[2].name,
                                        (*tps->sp->sb->templateSelector)[2].name) == 0)
                                    {
                                    }
                                    else
                                    {
                                        return false;
                                    }
                                }
                            }
                        }
                        else
                        {
                            auto tpl = tpn->BaseType()->sp->sb->templateSelector;
                            SYMBOL* sym = (*tpl)[1].sp;
                            if ((*tpl)[1].isDeclType)
                            {
                                Type* tp1 = TemplateLookupTypeFromDeclType((*tpl)[1].tp);
                                if (tp1 && tp1->IsStructured())
                                    sym = tp1->BaseType()->sp;
                                else
                                    sym = nullptr;
                            }
                            auto find = (*tpl).begin();
                            auto finde = (*tpl).end();
                            for (++find, ++find; sym && find != finde; ++find)
                            {
                                SYMBOL* fsp;
                                if (!sym->tp->IsStructured())
                                    break;

                                fsp = search(sym->tp->BaseType()->syms, find->name);
                                if (!fsp)
                                {
                                    fsp = classdata(find->name, sym->tp->BaseType()->sp, nullptr, false, false);
                                    if (fsp == (SYMBOL*)-1)
                                        fsp = nullptr;
                                }
                                sym = fsp;
                            }
                            if (find != finde || !sym || (!sym->tp->CompatibleType(tps) && !SameTemplate(sym->tp, tps)))
                                return false;
                        }
                        return true;
                    }
                    else if ((tpn->type == BasicType::templateparam_ || tps->type == BasicType::templateparam_) &&
                             tpn->type != tps->type)
                    {
                        return false;
                    }
                    else if (told->BaseType()->sp->sb->castoperator)
                    {
                        return false;
                    }
                    else if (tpn->IsStructured() || tps->IsStructured())
                    {
                        if (tpn->IsArithmetic() || tps->IsArithmetic())
                        {
                            return false;
                        }
                        if (tpn->IsStructured() && tps->IsStructured() && !SameTemplate(tpn, tps))
                        {
                            return false;
                        }
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
                    for (++tss1, ++tss2; tss1 != (*ts1).end() && tss2 != (*ts2).end(); ++tss1, ++tss2)
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
            else if (tnew->BaseType()->sp->sb->castoperator)
            {
                Type* tps = told->BaseType()->btp;
                Type* tpn = tnew->BaseType()->btp;
                if (!templateCompareTypes(tpn, tps, true) && !SameTemplate(tpn, tps))
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
            if (matchOverload(sym->tp, spp->tp))
            {
                if (!spp->templateParams)
                    return spp;
                if (sym->sb->templateLevel == spp->sb->templateLevel ||
                    (sym->sb->templateLevel && !spp->sb->templateLevel && sym->templateParams && sym->templateParams->size() == 1))
                    return spp;

                if (!!spp->templateParams == !!sym->templateParams)
                {
                    auto tpl = spp->templateParams->begin();
                    auto tple = spp->templateParams->end();
                    ++tpl;
                    auto tpr = sym->templateParams->begin();
                    auto tpre = sym->templateParams->end();
                    ++tpr;
                    for (; tpl != tple && tpr != tpre; ++tpl, ++tpr)
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
SYMBOL* lookupGenericConversion(SYMBOL* sym, Type* tp)
{
    inGetUserConversion -= 3;
    SYMBOL* rv = getUserConversion(F_CONVERSION | F_WITHCONS, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, false);
    inGetUserConversion += 3;
    return rv;
}
SYMBOL* lookupSpecificCast(SYMBOL* sym, Type* tp)
{
    return getUserConversion(F_CONVERSION | F_STRUCTURE, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, false);
}
SYMBOL* lookupNonspecificCast(SYMBOL* sym, Type* tp)
{
    return getUserConversion(F_CONVERSION, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, true);
}
SYMBOL* lookupIntCast(SYMBOL* sym, Type* tp, bool implicit)
{
    return getUserConversion(F_CONVERSION | F_INTEGER, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, implicit);
}
SYMBOL* lookupArithmeticCast(SYMBOL* sym, Type* tp, bool implicit)
{
    return getUserConversion(F_CONVERSION | F_ARITHMETIC, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, implicit);
}
SYMBOL* lookupPointerCast(SYMBOL* sym, Type* tp)
{
    return getUserConversion(F_CONVERSION | F_POINTER, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, true);
}
static void structuredArg(SYMBOL* sym, std::list<SYMBOL*>& gather, Type* tp)
{
    if (tp->BaseType()->sp->sb->parentNameSpace)
    {
        searchNS(sym, tp->BaseType()->sp->sb->parentNameSpace, gather);
        return;
    }
    // a null value means the global namespace
    SYMBOL nssp = {0};
    SYMBOL::_symbody sb = {0};
    nssp.sb = &sb;
    sb.nameSpaceValues = rootNameSpace;
    searchNS(sym, &nssp, gather);
}
static void searchOneArg(SYMBOL* sym, std::list<SYMBOL*>& gather, Type* tp);
static void funcArg(SYMBOL* sp, std::list<SYMBOL*>& gather, Type* tp)
{
    for (auto sym : *tp->BaseType()->syms)
    {
        searchOneArg(sp, gather, sym->tp);
    }
    searchOneArg(sp, gather, tp->BaseType()->btp);
}
static void searchOneArg(SYMBOL* sym, std::list<SYMBOL*>& gather, Type* tp)
{
    if (tp->IsPtr() || tp->IsRef())
    {
        searchOneArg(sym, gather, tp->BaseType()->btp);
    }
    if (tp->IsArithmetic())
    {
        tp = tp->BaseType();
        if (tp->btp && tp->btp->type == BasicType::enum_)
            structuredArg(sym, gather, tp);
        return;
    }
    if (tp->IsStructured() || tp->BaseType()->type == BasicType::enum_)
    {
        structuredArg(sym, gather, tp);
        return;
    }
    if (tp->IsFunction())
    {
        funcArg(sym, gather, tp);
        return;
    }
    // member pointers...
}
static void weedToFunctions(std::list<SYMBOL*>& lst)
{
    std::list<SYMBOL*> remove;
    for (auto sym : lst)
    {
        if (sym->sb->storage_class != StorageClass::overloads_)
            remove.insert(remove.begin(), sym);
    }
    for (auto sym : remove)
    {
        lst.remove(sym);
    }
}
static void GatherConversions(SYMBOL* sym, SYMBOL** spList, int n, CallSite* args, Type* atp, e_cvsrn** icsList, int** lenList,
                              int argCount, SYMBOL*** funcList, bool usesInitList, bool deduceTemplate)
{
    int i;
    for (i = 0; i < n; i++)
    {
        int j;
        if (spList[i])
        {
            e_cvsrn arr[500][10];
            int counts[500];
            SYMBOL* funcs[200];
            bool t;
            memset(counts, 0, argCount * sizeof(int));
            for (j = i + 1; j < n; j++)
                if (spList[i] == spList[j])
                    spList[j] = 0;
            memset(funcs, 0, sizeof(funcs));
            t = getFuncConversions(spList[i], args, atp, sym->sb->parentClass, (e_cvsrn*)arr, counts, argCount, funcs, usesInitList,
                                   deduceTemplate);
            if (!t)
            {
                spList[i] = nullptr;
            }
            else
            {
                int n1 = 0;
                for (j = 0; j < argCount; j++)
                    n1 += counts[j];
                icsList[i] = Allocate<e_cvsrn>(n1);
                memcpy(icsList[i], arr, n1 * sizeof(e_cvsrn));
                lenList[i] = Allocate<int>(argCount);
                memcpy(lenList[i], counts, argCount * sizeof(int));
                funcList[i] = Allocate<SYMBOL*>(argCount);
                memcpy(funcList[i], funcs, argCount * sizeof(SYMBOL*));
            }
        }
    }
}
enum e_ct
{
    conv,
    user,
    ellipses
};
static bool ismath(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::uminus_:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::shiftby_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::add_:
        case ExpressionNode::sub_:
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::arraymul_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::structadd_:
        case ExpressionNode::mul_:
        case ExpressionNode::div_:
        case ExpressionNode::umul_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::ursh_:
        case ExpressionNode::mod_:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::lor_:
        case ExpressionNode::land_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::hook_:
        case ExpressionNode::select_:
            return true;
        default:
            return false;
    }
}
static bool ismem(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::global_:
        case ExpressionNode::pc_:
        case ExpressionNode::auto_:
        case ExpressionNode::threadlocal_:
        case ExpressionNode::construct_:
        case ExpressionNode::labcon_:
            return true;
        case ExpressionNode::thisref_:
            exp = exp->left;
            if (exp->v.func->sp->sb->isConstructor || exp->v.func->sp->sb->isDestructor)
                return false;
            /* fallthrough */
        case ExpressionNode::callsite_: {
            Type* tp = exp->v.func->sp->tp;
            if (tp->type == BasicType::aggregate_ || !tp->IsFunction())
                return false;
            tp = tp->BaseType()->btp;
            return tp->IsPtr() || tp->IsRef();
        }
        case ExpressionNode::add_:
        case ExpressionNode::sub_:
        case ExpressionNode::structadd_:
            return ismem(exp->left) || ismem(exp->right);
        case ExpressionNode::l_p_:
            return (exp->left->type == ExpressionNode::auto_ && exp->left->v.sp->sb->thisPtr);
        default:
            return false;
    }
}
static Type* toThis(Type* tp)
{
    if (tp->IsPtr())
        return tp;
    return Type::MakeType(BasicType::pointer_, tp);
}
static int compareConversions(SYMBOL* spLeft, SYMBOL* spRight, e_cvsrn* seql, e_cvsrn* seqr, Type* ltype, Type* rtype, Type* atype,
                              EXPRESSION* expa, SYMBOL* funcl, SYMBOL* funcr, int lenl, int lenr, bool fromUser)
{
    (void)spLeft;
    (void)spRight;
    e_ct xl = conv, xr = conv;
    int lderivedfrombase = 0, rderivedfrombase = 0;
    int rankl, rankr;
    int i;
    // must be of same general type, types are standard conversion, user defined conversion, ellipses
    for (i = 0; i < lenl; i++)
    {
        if (seql[i] == CV_ELLIPSIS)
            xl = ellipses;
        if (xl != ellipses && seql[i] == CV_USER)
            xl = user;
    }
    for (i = 0; i < lenr; i++)
    {
        if (seqr[i] == CV_ELLIPSIS)
            xr = ellipses;
        if (xr != ellipses && seqr[i] == CV_USER)
            xr = user;
    }
    if (xl != xr)
    {
        if (xl < xr)
            return -1;
        else
            return 1;
    }
    if (xl == conv)
    {
        // one seq is a subseq of the other
        int l = 0, r = 0;
        for (; l < lenl; l++)
            if (seql[l] == CV_DERIVEDFROMBASE || seql[l] == CV_LVALUETORVALUE)
                lderivedfrombase++;
        for (; r < lenr; r++)
            if (seqr[r] == CV_DERIVEDFROMBASE || seqr[r] == CV_LVALUETORVALUE)
                rderivedfrombase++;
        l = 0, r = 0;
        for (; l < lenl && r < lenr;)
        {
            bool cont = false;
            switch (seql[l])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    l++;
                    cont = true;
                    break;
                default:
                    break;
            }
            switch (seqr[r])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    r++;
                    cont = true;
                    break;
                default:
                    break;
            }
            if (cont)
                continue;
            if (seql[l] != seqr[r])
                break;
            l++, r++;
        }
        // special check, const zero to pointer is higher pref than int
        if (expa && isconstzero(ltype, expa))
        {
            auto lt2 = ltype;
            if (lt2->IsRef())
            {
                lt2 = lt2->BaseType()->btp;
                if (lt2->IsPtr())
                {
                    lt2 = rtype;
                    if (lt2->IsRef())
                        lt2 = lt2->BaseType()->btp;
                    if (lt2->IsInt())
                        return -1;
                }
            }
        }
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (l == lenl && r != lenr)
        {
            if (rtype->IsRef())
            {
                // prefere areference type to a class type
                if (ltype->IsStructured())
                    if (rtype->CompatibleType(ltype) || SameTemplate(rtype, ltype))
                        return 1;
            }
            return -1;
        }
        else if (l != lenl && r == lenr)
        {
            if (ltype->IsRef())
            {
                // prefere areference type to a class type
                if (rtype->IsStructured())
                    if (ltype->CompatibleType(rtype) || SameTemplate(ltype, rtype))
                        return -1;
            }
            return 1;
        }
        // compare ranks
        rankl = CV_IDENTITY;
        for (l = 0; l < lenl; l++)
            if (rank[seql[l]] > rankl && seql[l] != CV_DERIVEDFROMBASE)
                rankl = rank[seql[l]];
        rankr = CV_IDENTITY;
        for (r = 0; r < lenr; r++)
            if (rank[seqr[r]] > rankr && seqr[r] != CV_DERIVEDFROMBASE)
                rankr = rank[seqr[r]];
        if (rankl < rankr)
            return -1;
        else if (rankr < rankl)
            return 1;
        else if (lenl < lenr)
        {
            return -1;
        }
        else if (lenr < lenl)
        {
            return 1;
        }
        else
        {

            // ranks are same, do same rank comparisons
            Type *tl = ltype, *tr = rtype, *ta = atype;
            // check if one or the other but not both converts a pointer to bool
            rankl = 0;
            for (l = 0; l < lenl; l++)
                if (seql[l] == CV_BOOLCONVERSION)
                    rankl = 1;
            rankr = 0;
            for (r = 0; r < lenr; r++)
                if (seqr[r] == CV_BOOLCONVERSION)
                    rankr = 1;
            if (rankl != rankr)
            {
                if (rankl)
                    return 1;
                else
                    return -1;
            }
            if (fromUser)
            {
                // conversion from pointer to base class to void * is better than pointer
                // to derived class to void *
                if (ta->IsPtr() && ta->BaseType()->btp->BaseType()->type == BasicType::void_)
                {
                    SYMBOL* second = tl->BaseType()->btp->BaseType()->sp;
                    SYMBOL* first = tr->BaseType()->btp->BaseType()->sp;
                    int v;
                    v = classRefCount(first, second);
                    if (v == 1)
                        return 1;
                    v = classRefCount(second, first);
                    if (v == 1)
                        return -1;
                }
            }
            else if (ta)
            {
                // conversion to pointer to base class is better than conversion to void *
                if (tl->IsPtr() && ta->IsPtr() && tl->BaseType()->btp->BaseType()->type == BasicType::void_)
                {
                    if (ta->BaseType()->btp->IsStructured())
                    {
                        if (tr->IsPtr() && tr->BaseType()->btp->IsStructured())
                        {
                            SYMBOL* derived = ta->BaseType()->btp->BaseType()->sp;
                            SYMBOL* base = tr->BaseType()->btp->BaseType()->sp;
                            int v = classRefCount(base, derived);
                            if (v == 1)
                                return 1;
                        }
                    }
                }
                else if (tr->IsPtr() && ta->IsPtr() && tr->BaseType()->btp->BaseType()->type == BasicType::void_)
                {
                    if (ta->BaseType()->btp->IsStructured())
                    {
                        if (tl->IsPtr() && tl->BaseType()->btp->IsStructured())
                        {
                            SYMBOL* derived = ta->BaseType()->btp->BaseType()->sp;
                            SYMBOL* base = tl->BaseType()->btp->BaseType()->sp;
                            int v = classRefCount(base, derived);
                            if (v == 1)
                                return -1;
                        }
                    }
                }
            }
            // various rules for the comparison of two pairs of structures
            if (ta && ta->IsPtr() && tr->IsPtr() && tl->IsPtr())
            {
                ta = ta->BaseType()->btp;
                tl = tl->BaseType()->btp;
                tr = tr->BaseType()->btp;
                // prefer a const function when the expression is a string literal
                if (expa->type == ExpressionNode::labcon_)
                {
                    if (tl->IsConst())
                    {
                        if (!tr->IsConst())
                            return -1;
                    }
                    else if (tr->IsConst())
                        return 1;
                }
                // if qualifiers are mismatched, choose a matching argument

                bool va = ta->IsVolatile();
                bool vl = tl->IsVolatile();
                bool vr = tr->IsVolatile();
                bool ca = ta->IsConst();
                bool cl = tl->IsConst();
                bool cr = tr->IsConst();
                if (cl == cr && vl != vr)
                {
                    if (va == vl)
                        return -1;
                    else if (va == vr)
                        return 1;
                }
                else if (vl == vr && cl != cr)
                {
                    if (ca == cl)
                        return -1;
                    else if (ca == cr)
                        return 1;
                }
            }
            else
            {
                if (tl->IsRef() && tr->IsRef())
                {
                    BasicType refa = BasicType::rref_;
                    if (ta)
                    {
                        if (ta->lref || ta->BaseType()->lref)
                            refa = BasicType::lref_;
                    }
                    if (ta && refa == BasicType::rref_ && expa && !ta->rref && !ta->BaseType()->rref)
                    {
                        if (expa->type != ExpressionNode::thisref_ && expa->type != ExpressionNode::callsite_)
                            refa = BasicType::lref_;
                    }
                    // const rref is better than const lref
                    BasicType refl = tl->BaseType()->type;
                    BasicType refr = tr->BaseType()->type;
                    if (refl == BasicType::rref_ && refr == BasicType::lref_ && tr->BaseType()->btp->IsConst())
                    {
                        if (refa != BasicType::lref_ || (ta->IsRef() && ta->BaseType()->btp->IsConst()))
                            return -1;
                        else
                            return 1;
                    }
                    if (refr == BasicType::rref_ && refl == BasicType::lref_ && tl->BaseType()->btp->IsConst())
                    {
                        if (refa != BasicType::lref_ || (ta->IsRef() && ta->BaseType()->btp->IsConst()))
                            return 1;
                        else
                            return -1;
                    }
                    if (ta && !ta->IsRef())
                    {
                        // try to choose a const ref when there are two the same
                        if (refl == refr)
                        {
                            bool lc = tl->BaseType()->btp->IsConst();
                            bool rc = tr->BaseType()->btp->IsConst();
                            if (lc && !rc)
                                return -1;
                            if (rc && !lc)
                                return 1;
                        }
                    }
                }
                if (ta && ta->IsRef())
                    ta = ta->BaseType()->btp;
                if (tl->IsRef())
                    tl = tl->BaseType()->btp;
                if (tr->IsRef())
                    tr = tr->BaseType()->btp;
            }

            if (ta && ta->IsStructured() && tl->IsStructured() && tr->IsStructured())
            {
                ta = ta->BaseType();
                tl = tl->BaseType();
                tr = tr->BaseType();
                int cmpl = SameTemplate(tl, ta);
                int cmpr = SameTemplate(tr, ta);
                if (fromUser)
                {
                    if (cmpr || cmpl)
                    {
                        if (cmpr)
                        {
                            if (cmpl)
                                return 0;
                            return -1;
                        }
                        else
                            return 1;
                    }
                    else if (classRefCount(ta->sp, tl->sp) == 1 && classRefCount(ta->sp, tr->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return -1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return 1;
                                else
                                    return 0;
                            }
                            return -1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return 1;
                        }
                    }
                }
                else
                {
                    if (cmpr || cmpl)
                    {
                        if (cmpr)
                        {
                            if (cmpl)
                                return 0;
                            return 1;
                        }
                        else
                            return -1;
                    }
                    else if (classRefCount(tl->sp, ta->sp) == 1 && classRefCount(tr->sp, ta->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return 1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return -1;
                                else
                                    return 0;
                            }
                            return 1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return -1;
                        }
                    }
                }
            }

            if (ta && ta->BaseType()->type == BasicType::memberptr_ && tl->BaseType()->type == BasicType::memberptr_ &&
                tr->BaseType()->type == BasicType::memberptr_)
            {
                ta = ta->BaseType();
                tl = tl->BaseType();
                tr = tr->BaseType();
                if (fromUser)
                {
                    if (classRefCount(tl->sp, ta->sp) == 1 && classRefCount(tr->sp, ta->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return 1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return -1;
                                else
                                    return 0;
                            }
                            return 1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return -1;
                        }
                    }
                }
                else
                {
                    if (classRefCount(ta->sp, tl->sp) == 1 && classRefCount(ta->sp, tr->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return -1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return 1;
                                else
                                    return 0;
                            }
                            return -1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return 1;
                        }
                    }
                }
            }
        }
        // compare qualifiers at top level
        rankl = !!ltype->IsConst() + !!ltype->IsVolatile() * 2;
        rankr = !!rtype->IsConst() + !!rtype->IsVolatile() * 2;
        if (rankl != rankr)
        {
            if (ltype->BaseType()->CompatibleType(rtype->BaseType()))
            {
                int n1 = rankl ^ rankr;
                if ((n1 & rankl) && !(n1 & rankr))
                    return 1;
                if ((n1 & rankr) && !(n1 & rankl))
                    return -1;
            }
        }
        if (atype && rtype->IsRef() && ltype->IsRef())
        {
            // rvalue matches an rvalue reference better than an IsLValue reference

            if (rtype->IsRef() && ltype->IsRef() && ltype->BaseType()->type != rtype->BaseType()->type)
            {
                int lref = expa && IsLValue(expa);
                int rref = expa && (!IsLValue(expa) && (!rtype->IsStructured() || !ismem(expa)));
                if (expa && expa->type == ExpressionNode::callsite_)
                {
                    Type* tp = expa->v.func->sp->tp->BaseType()->btp;
                    if (tp)
                    {
                        if (tp->type == BasicType::rref_)
                            rref = true;
                        if (tp->type == BasicType::lref_)
                            lref = true;
                    }
                }
                lref |= expa && atype->IsStructured() && expa->type != ExpressionNode::not__lvalue_;
                if (ltype->BaseType()->type == BasicType::rref_)
                {
                    if (lref)
                        return 1;
                    else if (rref)
                        return -1;
                }
                else if (ltype->BaseType()->type == BasicType::lref_)
                {
                    if (lref)
                        return -1;
                    else if (rref)
                        return 1;
                }
            }
            // compare qualifiers at top level
            rankl = !!ltype->BaseType()->btp->IsConst() + !!ltype->BaseType()->btp->IsVolatile() * 2;
            rankr = !!rtype->BaseType()->btp->IsConst() + !!rtype->BaseType()->btp->IsVolatile() * 2;
            if (rankl != rankr)
            {
                if (ltype->BaseType()->btp->BaseType()->CompatibleType(rtype->BaseType()->btp->BaseType()))
                {
                    int n1 = rankl ^ rankr;
                    if ((n1 & rankl) && !(n1 & rankr))
                        return 1;
                    if ((n1 & rankr) && !(n1 & rankl))
                        return -1;
                }
            }
        }
        // make sure base types are same
        if (atype)
        {
            while (ltype->IsPtr() || ltype->IsRef())
                ltype = ltype->BaseType()->btp;
            while (rtype->IsPtr() || rtype->IsRef())
                rtype = rtype->BaseType()->btp;
            while (atype->IsPtr() || atype->IsRef())
                atype = atype->BaseType()->btp;
            ltype = ltype->BaseType();
            rtype = rtype->BaseType();
            atype = atype->BaseType();
            if (atype->type == ltype->type)
            {
                if (atype->type != rtype->type)
                    return -1;
            }
            else if (atype->type == rtype->type)
            {
                return 1;
            }
        }
    }
    else if (xl == user)
    {
        Type *ta = atype, *tl = ltype, *tr = rtype;
        if (ltype->IsRef() && rtype->IsRef())
        {
            // rref is better than const lref
            BasicType refl = ltype->BaseType()->type;
            BasicType refr = rtype->BaseType()->type;
            if (refl == BasicType::rref_ && refr == BasicType::lref_ && rtype->BaseType()->btp->IsConst())
                return -1;
            if (refr == BasicType::rref_ && refl == BasicType::lref_ && ltype->BaseType()->btp->IsConst())
                return 1;
        }
        int l = 0, r = 0, llvr = 0, rlvr = 0;
        if (funcl && seql[l] == CV_DERIVEDFROMBASE && seqr[r] == CV_DERIVEDFROMBASE)
        {
            auto it = funcl->tp->BaseType()->syms->begin();
            if (!funcl->sb->castoperator)
                ++it;
            ltype = (*it)->tp;
            it = funcr->tp->BaseType()->syms->begin();
            if (!funcr->sb->castoperator)
                ++it;
            rtype = (*it)->tp;
            if (ltype->IsRef())
                ltype = ltype->BaseType()->btp;
            if (rtype->IsRef())
                rtype = rtype->BaseType()->btp;
            if (atype->IsRef())
                atype = atype->BaseType()->btp;
            ltype = ltype->BaseType();
            rtype = rtype->BaseType();
            atype = atype->BaseType();
            if (classRefCount(ltype->sp, atype->sp) == 1 && classRefCount(ltype->sp, atype->sp) == 1)
            {
                if (classRefCount(ltype->sp, rtype->sp) == 1)
                {
                    return 1;
                }
                else if (classRefCount(rtype->sp, ltype->sp) == 1)
                {
                    return -1;
                }
            }
            if (!ltype->CompatibleType(rtype))
                return 0;
        }
        if (seql[l] == CV_USER && seqr[r] == CV_USER && funcl && funcr)
        {
            return 0;
        }
        l = 0, r = 0;
        for (; l < lenl && seql[l] != CV_USER && r < lenr && seqr[r] != CV_USER;)
        {
            bool cont = false;
            switch (seql[l])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    l++;
                    cont = true;
                    break;
                case CV_LVALUETORVALUE:
                    llvr++;
                    break;
                default:
                    break;
            }
            switch (seqr[r])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    r++;
                    cont = true;
                    break;
                case CV_LVALUETORVALUE:
                    rlvr++;
                default:
                    break;
            }
            if (cont)
                continue;
            if (seql[l] != seqr[r])
                break;
            l++, r++;
        }
        if (llvr && !rlvr)
            return -1;
        if (!llvr && rlvr)
            return 1;
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (seql[l] == CV_USER && seqr[r] != CV_USER)
        {
            return -1;
        }
        else if (seql[l] != CV_USER && seqr[r] == CV_USER)
        {
            return 1;
        }
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (l == lenl && r != lenr)
        {
            return -1;
        }
        else if (l != lenl && r == lenr)
        {
            return 1;
        }
        l++, r++;
        // compare ranks
        rankl = CV_IDENTITY;
        for (; l < lenl; l++)
            if (rank[seql[l]] > rankl && seql[l] != CV_DERIVEDFROMBASE)
                rankl = rank[seql[l]];
        rankr = CV_IDENTITY;
        for (; r < lenr; r++)
            if (rank[seqr[r]] > rankr && seqr[r] != CV_DERIVEDFROMBASE)
                rankr = rank[seqr[r]];
        if (rankl < rankr)
            return -1;
        else if (rankr < rankl)
            return 1;
        else if (lenl < lenr)
        {
            return -1;
        }
        else if (lenr < lenl)
        {
            return 1;
        }
        // if qualifiers are mismatched, choose a matching argument
        if (tl && tr)
        {
            if (ta && (tl->IsRef() || tr->IsRef()))
            {
                bool ll = false;
                bool lr = false;
                if (tl->BaseType()->type == BasicType::rref_)
                    lr = true;
                else
                    ll = true;
                bool rl = false;
                bool rr = false;
                if (tr->BaseType()->type == BasicType::rref_)
                    rr = true;
                else
                    rl = true;
                if (ll != rl)
                {
                    bool lref = !ta->IsRef() || ta->BaseType()->type == BasicType::lref_;
                    if (ll)
                    {
                        if (lref)
                            return -1;
                        else
                            return 1;
                    }
                    else
                    {
                        if (lref)
                            return 1;
                        else
                            return -1;
                    }
                }
            }
            if (tl->IsRef())
                tl = tl->BaseType()->btp;
            if (tr->IsRef())
                tr = tr->BaseType()->btp;
            bool vl = tl->IsVolatile();
            bool vr = tr->IsVolatile();
            bool cl = tl->IsConst();
            bool cr = tr->IsConst();
            if (cl == cr && vl != vr)
            {
                if (vl)
                    return 1;
                else
                    return -1;
            }
            else if (vl == vr && cl != cr)
            {
                if (cl)
                    return 1;
                else
                    return -1;
            }
        }
    }
    // Keyword::ellipse_ always returns 0;
    return 0;
}
static bool ellipsed(SYMBOL* sym)
{
    for (auto sym1 : *sym->tp->BaseType()->syms)
        if (sym1->tp->BaseType()->type == BasicType::ellipse_)
            return true;
    return false;
}
static int ChooseLessConstTemplate(SYMBOL* left, SYMBOL* right)
{
    if (left->templateParams && right->templateParams)
    {
        int lcount = 0, rcount = 0;
        std::list<TEMPLATEPARAMPAIR>::iterator itl, itr, itle, itre;
        auto xx = left->templateParams->front().second->bySpecialization.types;
        if (xx)
        {
            itl = xx->begin();
            itle = xx->end();
        }
        else
        {
            itl = left->templateParams->begin();
            ++itl;
            itle = left->templateParams->end();
        }
        xx = right->templateParams->front().second->bySpecialization.types;
        if (xx)
        {
            itr = xx->begin();
            itre = xx->end();
        }
        else
        {
            itr = right->templateParams->begin();
            ++itr;
            itre = right->templateParams->end();
        }
        for (; itl != itle && itr != itre; ++itl, ++itr)
        {
            auto&& tpl = *itl;
            auto&& tpr = *itr;
            if (tpl.second->packed || tpr.second->packed)
                return 0;
            if (tpl.second->type == tpr.second->type && tpl.second->type == TplType::typename_)
            {
                auto tppl = tpl.second->byClass.val;
                auto tppr = tpr.second->byClass.val;
                if (tppl && tppr)
                {
                    bool lptr = false, rptr = false;
                    while (tppl->IsRef() || tppl->IsPtr())
                    {
                        if (tppl->IsConst())
                            lcount++;
                        if (tppl->IsVolatile())
                            lcount++;
                        lptr = true;
                        tppl = tppl->BaseType()->btp;
                    }
                    while (tppr->IsRef() || tppr->IsPtr())
                    {
                        if (tppr->IsConst())
                            rcount++;
                        if (tppr->IsVolatile())
                            rcount++;
                        rptr = true;
                        tppr = tppr->BaseType()->btp;
                    }
                    if (!lptr)
                    {
                        if (tppl->IsConst())
                            lcount++;
                        if (tppl->IsVolatile())
                            lcount++;
                    }
                    if (!rptr)
                    {
                        if (tppr->IsConst())
                            rcount++;
                        if (tppr->IsVolatile())
                            rcount++;
                    }
                    if (tppl->IsStructured() && tppr->IsStructured())
                    {
                        switch (ChooseLessConstTemplate(tppl->BaseType()->sp, tppr->BaseType()->sp))
                        {
                            case -1:
                                lcount++;
                                break;
                            case 1:
                                rcount++;
                                break;
                        }
                    }
                }
            }
        }
        if (itl == itle && itr == itre)
        {
            if (lcount < rcount)
            {
                return -1;
            }
            if (rcount < lcount)
            {
                return 1;
            }
        }
    }
    else if (left->tp->IsFunction())
    {
        int lcount = 0, rcount = 0;
        auto l = left->tp->BaseType()->syms->begin();
        auto lend = left->tp->BaseType()->syms->end();
        auto r = right->tp->BaseType()->syms->begin();
        auto rend = right->tp->BaseType()->syms->end();
        if (left->tp->IsConst())
            lcount++;
        if (right->tp->IsConst())
            rcount++;
        for (; l != lend && r != rend; ++l, ++r)
        {
            auto ltp = (*l)->tp;
            auto rtp = (*r)->tp;
            while (ltp->IsRef() || ltp->IsPtr())
                ltp = ltp->BaseType()->btp;
            while (rtp->IsRef() || rtp->IsPtr())
                rtp = rtp->BaseType()->btp;
            if (ltp->IsStructured() && rtp->IsStructured())
                switch (ChooseLessConstTemplate(ltp->BaseType()->sp, rtp->BaseType()->sp))
                {
                    case -1:
                        lcount++;
                        break;
                    case 1:
                        rcount++;
                        break;
                }
        }
        if (l == lend && r == rend)
        {
            if (lcount < rcount)
            {
                return -1;
            }
            if (rcount < lcount)
            {
                return 1;
            }
        }
    }
    return 0;
}
static void SelectBestFunc(SYMBOL** spList, e_cvsrn** icsList, int** lenList, CallSite* funcparams, int argCount, int funcCount,
                           SYMBOL*** funcList)
{
    static e_cvsrn identity = CV_IDENTITY;
    char arr[500];
    int i, j;
    for (i = 0; i < funcCount; i++)
    {
        for (j = i + 1; j < funcCount && spList[i]; j++)
        {
            if (spList[j])
            {
                if (spList[i] && spList[j])
                {
                    int bothCast = spList[i]->sb->castoperator && spList[j]->sb->castoperator;
                    int left = 0, right = 0;
                    int l = 0, r = 0;
                    int k = 0;
                    int lk = 0, rk = 0;
                    std::list<Argument*>* args = funcparams ? funcparams->arguments : nullptr;
                    std::list<Argument*>::iterator argit;
                    std::list<Argument*>::iterator argite;
                    std::list<Argument*> dummy;
                    if (args)
                    {
                        argit = args->begin();
                        argite = args->end();
                    }
                    else
                    {
                        argit = dummy.begin();
                        argite = dummy.end();
                    }
                    auto itl = spList[i]->tp->BaseType()->syms->begin();
                    auto itlend = spList[i]->tp->BaseType()->syms->end();
                    auto itr = spList[j]->tp->BaseType()->syms->begin();
                    auto itrend = spList[j]->tp->BaseType()->syms->end();
                    memset(arr, 0, sizeof(arr));
                    int xindex = 0;
                    for (k = 0; k < argCount + xindex; k++)
                    {
                        e_cvsrn* seql = &icsList[i][l];
                        e_cvsrn* seqr = &icsList[j][r];
                        int lenl = lenList[i][k];
                        int lenr = lenList[j][k];
                        if (!lenl)
                        {
                            seql = &identity;
                            lenl = 1;
                        }
                        if (!lenr)
                        {
                            seqr = &identity;
                            lenr = 1;
                        }
                        int bl = 0, br = 0;
                        for (int i = 0; i < lenl; i++)
                            if (seql[i] == CV_USER)
                                bl++;
                        for (int i = 0; i < lenr; i++)
                            if (seqr[i] == CV_USER)
                                br++;
                        if (bl > 1 || br > 1 || !spList[i] || !spList[j])
                        {
                            if (bl > 1)
                                spList[i] = nullptr;
                            if (br > 1)
                                spList[j] = nullptr;
                        }
                        else if (k == 0 && funcparams && funcparams->thisptr &&
                                 (spList[i]->sb->castoperator || (*itl)->sb->thisPtr) &&
                                 (spList[i]->sb->castoperator || (*itr)->sb->thisPtr))
                        {
                            Type *tpl, *tpr;
                            tpl = (*itl)->tp;
                            ++itl;
                            tpr = (*itr)->tp;
                            ++itr;
                            arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, funcparams->thistp,
                                                        funcparams->thisptr, funcList ? funcList[i][k] : nullptr,
                                                        funcList ? funcList[j][k] : nullptr, lenl, lenr, false);
                            xindex++;
                        }
                        else
                        {
                            Type *tpl, *tpr;
                            if (funcparams && funcparams->thisptr)
                            {
                                if (itl != itlend && (*itl)->sb->thisPtr)
                                {
                                    l += lenList[i][k + lk++];
                                    lenl = lenList[i][k + lk];
                                    ++itl;
                                }
                                if (itr != itrend && (*itr)->sb->thisPtr)
                                {
                                    r += lenList[j][k + rk++];
                                    lenr = lenList[j][k + rk];
                                    ++itr;
                                }
                            }
                            if (spList[i]->sb->castoperator)
                                tpl = spList[i]->tp;
                            else
                                tpl = itl != itlend ? (*itl)->tp : nullptr;
                            if (spList[j]->sb->castoperator)
                                tpr = spList[j]->tp;
                            else
                                tpr = itr != itrend ? (*itr)->tp : nullptr;
                            if (tpl && tpr)
                            {
                                arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr,
                                                            argit != argite ? (*argit)->tp : 0, argit != argite ? (*argit)->exp : 0,
                                                            funcList ? funcList[i][k + lk] : nullptr,
                                                            funcList ? funcList[j][k + rk] : nullptr, lenl, lenr, false);
                            }
                            else
                            {
                                arr[k] = 0;
                            }
                            if (bothCast)
                            {
                                tpl = spList[i]->tp->BaseType()->btp;
                                tpr = spList[j]->tp->BaseType()->btp;
                                arr[k + 1] = compareConversions(
                                    spList[i], spList[j], seql, seqr, tpl, tpr, argit != argite ? (*argit)->tp : 0,
                                    argit != argite ? (*argit)->exp : 0, funcList ? funcList[i][k + lk] : nullptr,
                                    funcList ? funcList[j][k + rk] : nullptr, lenl, lenr, false);
                            }
                            if (itl != itlend)
                                ++itl;
                            if (itr != itrend)
                                ++itr;
                            if (argit != argite)
                                ++argit;
                        }
                        l += lenList[i][k + lk];
                        r += lenList[j][k + rk];
                    }
                    for (k = 0; k < argCount + bothCast + xindex; k++)
                    {
                        if (arr[k] > 0)
                            right++;
                        else if (arr[k] < 0)
                            left++;
                    }
                    if (left && !right)
                    {
                        spList[j] = nullptr;
                    }
                    else if (right && !left)
                    {
                        spList[i] = nullptr;
                    }
                    else if (spList[i] && spList[j])
                    {
                        if (spList[i]->sb->castoperator)
                        {
                            if (!spList[j]->sb->castoperator)
                                spList[j] = nullptr;
                        }
                        else
                        {
                            if (spList[j]->sb->castoperator)
                                spList[i] = nullptr;
                        }
                        if (spList[i] && spList[j])
                        {
                            switch (ChooseLessConstTemplate(spList[i], spList[j]))
                            {
                                case -1:
                                    spList[j] = nullptr;
                                    break;
                                case 1:
                                    spList[i] = nullptr;
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }
    for (i = 0, j = 0; i < funcCount; i++)
    {
        if (spList[i])
            j++;
    }
    if (j > 1)
    {
        int ellipseCount = 0, unellipseCount = 0;
        for (i = 0, j = 0; i < funcCount; i++)
        {
            if (spList[i])
            {
                if (ellipsed(spList[i]))
                    ellipseCount++;
                else
                    unellipseCount++;
            }
        }
        if (unellipseCount && ellipseCount)
        {
            for (i = 0, j = 0; i < funcCount; i++)
            {
                if (spList[i] && ellipsed(spList[i]))
                    spList[i] = 0;
            }
        }
        for (i = 0, j = 0; i < funcCount; i++)
        {
            if (spList[i])
                j++;
        }
        if (j > 1)
        {
            if (funcparams->arguments)
            {
                int* match = (int*)alloca(sizeof(int) * 500);
                bool foundArithmetic = false, foundNested = false;
                for (auto arg : *funcparams->arguments)
                {
                    if (!arg->tp)
                    {
                        foundNested = true;
                    }
                    else if (arg->tp->IsArithmetic())
                    {
                        foundArithmetic = true;
                        break;
                    }
                }
                if (foundArithmetic)
                {
                    for (int i = 0; i < funcCount; i++)
                    {
                        match[i] = INT_MIN;
                        if (spList[i] && !spList[i]->sb->templateLevel)
                        {
                            auto it = spList[i]->tp->BaseType()->syms->begin();
                            auto ite = spList[i]->tp->BaseType()->syms->end();
                            if ((*it)->sb->thisPtr)
                                ++it;
                            int n = 0;
                            foundArithmetic = false;
                            for (auto arg : *funcparams->arguments)
                            {
                                if (it == ite)
                                {
                                    foundArithmetic = true;
                                    break;
                                }
                                Type* target = (*it)->tp;
                                Type* current = arg->tp;
                                if (!current)  // initlist, don't finish this screening
                                    return;
                                while (target->IsRef())
                                    target = target->BaseType()->btp;
                                while (current->IsRef())
                                    current = current->BaseType()->btp;
                                if (target->IsArithmetic() && current->IsArithmetic())
                                {
                                    if (target->IsInt())
                                    {
                                        if (current->IsFloat())
                                            current = &stdint;
                                    }
                                    else if (target->IsFloat())
                                    {
                                        if (current->IsInt())
                                            current = &stddouble;
                                    }
                                    if (current->BaseType()->type <= target->BaseType()->type)
                                    {
                                        n++;
                                    }
                                    else if (current->IsInt() && target->IsInt())
                                    {
                                        if (getSize(current->BaseType()->type) == getSize(target->BaseType()->type))
                                            n++;
                                    }
                                }
                                else if (!current->IsPtr() || !target->IsPtr())
                                {
                                    n = INT_MIN;
                                }
                                ++it;
                            }
                            if (!foundArithmetic && (it == ite || (*it)->sb->defaultarg))
                            {
                                match[i] = n;
                            }
                        }
                    }
                    int sum = 0;
                    for (int i = 0; i < funcCount; i++)
                    {
                        if (match[i] > sum)
                        {
                            sum = match[i];
                        }
                    }
                    for (int i = 0; i < funcCount; i++)
                        if (match[i] != sum && match[i] >= 0)
                            spList[i] = nullptr;
                }
                if (foundNested)
                {
                    for (int i = 0; i < funcCount; i++)
                    {
                        if (spList[i] && !spList[i]->sb->templateLevel)
                        {
                            auto it = spList[i]->tp->BaseType()->syms->begin();
                            auto ite = spList[i]->tp->BaseType()->syms->end();
                            if ((*it)->sb->thisPtr)
                                ++it;
                            if ((*it)->tp->IsVoid())
                                ++it;
                            auto ita = funcparams->arguments->begin();
                            auto itae = funcparams->arguments->end();
                            for (; ita != itae; ++ita)
                            {
                                if ((*ita)->nested)
                                {
                                    int n = (*ita)->nested->size();
                                    if (it == ite)
                                    {
                                        if (n != 0)
                                        {
                                            spList[i] = nullptr;
                                        }
                                        break;
                                    }
                                    else if (!(*it)->sb->initializer_list)
                                    {
                                        int m = 0;
                                        while (it != ite) ++it, ++m;
                                        if (m != n)
                                        {
                                            spList[i] = nullptr;
                                        }
                                    }
                                    break;
                                }
                                if (it == ite)
                                    break;
                                ++it;
                            }
                        }
                    }
                }
            }
        }
    }
}
static void GetMemberCasts(std::list<SYMBOL*>& gather, SYMBOL* sym)
{
    if (sym)
    {
        SYMBOL* find = search(sym->tp->BaseType()->syms, overloadNameTab[CI_CAST]);
        if (find)
            gather.insert(gather.begin(), find);
        if (sym->sb->baseClasses)
            for (auto bcl : *sym->sb->baseClasses)
                GetMemberCasts(gather, bcl->cls);
    }
}
static void GetMemberConstructors(std::list<SYMBOL*>& gather, SYMBOL* sym)
{
    SYMBOL* find = search(sym->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (find)
    {
        gather.insert(gather.begin(), find);
    }
    if (sym->sb->baseClasses)
        for (auto bcl : *sym->sb->baseClasses)
            GetMemberConstructors(gather, bcl->cls);
}
void InitializeFunctionArguments(SYMBOL* sym, bool initialize)
{
    sym->tp->BaseType()->btp = ResolveTemplateSelectors(sym, sym->tp->BaseType()->btp);
    sym->tp->BaseType()->btp->InstantiateDeferred();
    if (initialize)
    {
        sym->tp->BaseType()->btp = sym->tp->BaseType()->btp->InitializeDeferred();
    }
    for (auto a : *sym->tp->BaseType()->syms)
    {
        a->tp = ResolveTemplateSelectors(sym, a->tp);
        a->tp->InstantiateDeferred();
        if (initialize)
        {
            a->tp = a->tp->InitializeDeferred();
        }
    }
    if (!sym->sb->decoratedName || strchr(sym->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
        SetLinkerNames(sym, Linkage::cpp_);
}
SYMBOL* getUserConversion(int flags, Type* tpp, Type* tpa, EXPRESSION* expa, int* n, e_cvsrn* seq, SYMBOL* candidate_in,
                          SYMBOL** userFunc, bool honorExplicit)
{
    if (inGetUserConversion < 2)
    {
        std::list<SYMBOL*> gather;
        Type* tppp;
        if (tpp->type == BasicType::typedef_)
            tpp = tpp->btp;
        tpa = tpa->InitializeDeferred();
        tppp = tpp;
        if (tppp->IsRef())
            tppp = tppp->BaseType()->btp;
        inGetUserConversion++;
        if (flags & F_WITHCONS)
        {
            if (tppp->IsStructured() || tpp->IsDeferred())
            {
                if (tpp->IsDeferred())
                    tppp->InstantiateDeferred();
                else
                    tppp = tppp->InitializeDeferred();
                tppp = tppp->BaseType();
                GetMemberConstructors(gather, tppp->sp);
                tppp = tppp->sp->tp;
            }
        }
        GetMemberCasts(gather, tpa->BaseType()->sp);
        if (gather.size())
        {
            int funcs = 0;
            int i;
            SYMBOL** spList;
            e_cvsrn** icsList;
            int** lenList;
            int m = 0;
            SYMBOL *found1, *found2;
            CallSite funcparams;
            std::list<Argument*> args;
            Type thistp;
            EXPRESSION exp;
            memset(&funcparams, 0, sizeof(funcparams));
            memset(&thistp, 0, sizeof(thistp));
            memset(&exp, 0, sizeof(exp));
            funcparams.arguments = &args;
            Argument arg0 = {};
            args.push_back(&arg0);
            arg0.tp = tpa;
            arg0.exp = &exp;
            exp.type = ExpressionNode::c_i_;
            funcparams.ascall = true;
            funcparams.thisptr = expa;
            funcparams.thistp = &thistp;
            Type::MakeType(thistp, BasicType::pointer_, tpp);
            for (auto sp : gather)
            {
                funcs += sp->tp->syms->size();
            }
            spList = (SYMBOL**)alloca(funcs * sizeof(SYMBOL*));
            icsList = (e_cvsrn**)alloca(funcs * sizeof(e_cvsrn*));
            lenList = (int**)alloca(funcs * sizeof(int*));
            i = 0;
            std::set<SYMBOL*> filters;
            for (auto sp : gather)
            {
                for (auto sym : *sp->tp->syms)
                {
                    if (!sym->sb->instantiated && filters.find(sym) == filters.end() &&
                        filters.find(sym->sb->mainsym) == filters.end())
                    {
                        bool found = !sym->sb->isConstructor;
                        if (!found)
                        {
                            auto it = sym->tp->BaseType()->syms->begin();
                            if (it != sym->tp->BaseType()->syms->end())
                            {
                                ++it;
                                if (it != sym->tp->BaseType()->syms->end())
                                {
                                    ++it;
                                    if (it != sym->tp->BaseType()->syms->end())
                                    {
                                        found = (*it)->sb->defaultarg;
                                    }
                                    else
                                    {
                                        found = true;
                                    }
                                }
                            }
                        }
                        if (found)
                        {
                            filters.insert(sym);
                            if (sym->sb->mainsym)
                                filters.insert(sym->sb->mainsym);
                            if (sym->sb->templateLevel && sym->templateParams)
                            {
                                if (sym->sb->castoperator)
                                {
                                    spList[i++] = detemplate(sym, nullptr, tppp);
                                }
                                else
                                {
                                    spList[i++] = detemplate(sym, &funcparams, nullptr);
                                }
                            }
                            else
                            {
                                InitializeFunctionArguments(sym);
                                spList[i++] = sym;
                            }
                        }
                    }
                }
            }
            funcs = i;
            memset(&exp, 0, sizeof(exp));
            exp.type = ExpressionNode::not__lvalue_;
            for (i = 0; i < funcs; i++)
            {
                SYMBOL* candidate = spList[i];
                if (candidate)
                {
                    if (honorExplicit && candidate->sb->isExplicit && !(flags & F_CONVERSION))
                    {
                        spList[i] = nullptr;
                    }
                    else
                    {
                        int j;
                        int n3 = 0, n2 = 0, m1;
                        e_cvsrn seq3[50];
                        if (candidate->sb->castoperator)
                        {
                            Type* tpc = candidate->tp->BaseType()->btp;
                            if (tpc->type == BasicType::typedef_)
                                tpc = tpc->btp;
                            if (tpc->IsRef())
                                tpc = tpc->BaseType()->btp;
                            if (tpc->type != BasicType::auto_ &&
                                (((flags & F_INTEGER) && !tpc->IsInt()) ||
                                 ((flags & F_POINTER) && !tpc->IsPtr() && tpc->BaseType()->type != BasicType::memberptr_) ||
                                 ((flags & F_ARITHMETIC) && !tpc->IsArithmetic()) ||
                                 ((flags & F_STRUCTURE) && !tpc->IsStructured())))
                            {
                                seq3[n2++] = CV_NONE;
                                seq3[n2 + n3++] = CV_NONE;
                            }
                            else
                            {
                                bool lref = false;
                                Type* tpn = candidate->tp->BaseType()->btp;
                                if (tpn->type == BasicType::typedef_)
                                    tpn = tpn->btp;
                                if (tpn->IsRef())
                                {
                                    if (tpn->BaseType()->type == BasicType::lref_)
                                        lref = true;
                                }
                                Type::MakeType(thistp, BasicType::pointer_, tpa);
                                getSingleConversion((*candidate->tp->BaseType()->syms->begin())->tp, &thistp, &exp, &n2, seq3,
                                                    candidate, nullptr, true);
                                seq3[n2 + n3++] = CV_USER;
                                inGetUserConversion--;
                                if (tpc->type == BasicType::auto_)
                                {
                                    seq3[n2 + n3++] = CV_USER;
                                }
                                else if (tppp->IsFunctionPtr())
                                {
                                    int n77 = n3;
                                    getSingleConversion(tppp, candidate->tp->BaseType()->btp, lref ? nullptr : &exp, &n3, seq3 + n2,
                                                        candidate, nullptr, true);
                                    if (n77 != n3 - 1 || seq3[n2 + n77] != CV_IDENTITY)
                                    {
                                        SYMBOL* spf = tppp->BaseType()->btp->BaseType()->sp;
                                        n3 = n77;
                                        if (spf->sb->templateLevel && spf->sb->storage_class == StorageClass::typedef_ &&
                                            !spf->sb->instantiated)
                                        {
                                            auto ita = spf->templateParams->begin();
                                            ++ita;
                                            auto itae = spf->templateParams->end();
                                            spf = spf->sb->mainsym;
                                            if (spf)
                                            {
                                                Type* hold[100];
                                                int count = 0;
                                                for (auto it = ita; it != itae; ++it)
                                                {
                                                    auto&& srch = *it;
                                                    hold[count++] = srch.second->byClass.dflt;
                                                    srch.second->byClass.dflt = srch.second->byClass.val;
                                                }
                                                spf = GetTypeAliasSpecialization(spf, spf->templateParams);
                                                spf->tp = SynthesizeType(spf->tp, nullptr, false);
                                                getSingleConversion(spf->tp, candidate->tp->BaseType()->btp, lref ? nullptr : &exp,
                                                                    &n3, seq3 + n2, candidate, nullptr, true);
                                                count = 0;
                                                for (auto it = ita; it != itae; ++it)
                                                {
                                                    auto&& srch = *it;
                                                    srch.second->byClass.val = srch.second->byClass.dflt;
                                                    srch.second->byClass.dflt = hold[count++];
                                                }
                                            }
                                            else
                                            {
                                                seq3[n2 + n3++] = CV_NONE;
                                            }
                                        }
                                        else
                                        {
                                            getSingleConversion(tppp, candidate->tp->BaseType()->btp, lref ? nullptr : &exp, &n3,
                                                                seq3 + n2, candidate, nullptr, true);
                                        }
                                    }
                                }
                                else if (!candidate->tp->BaseType()->btp->CompatibleType(tpa) &&
                                         !SameTemplate(candidate->tp->BaseType()->btp, tpa))
                                {
                                    if (tppp->IsVoidPtr())
                                    {
                                        if (candidate->tp->BaseType()->btp->IsVoidPtr())
                                            seq3[n3++ + n2] = CV_IDENTITY;
                                        else
                                            seq3[n3++ + n2] = CV_POINTERCONVERSION;
                                    }
                                    else
                                    {
                                        getSingleConversion(tppp, candidate->tp->BaseType()->btp, lref ? nullptr : &exp, &n3,
                                                            seq3 + n2, candidate, nullptr, false);
                                    }
                                }
                                inGetUserConversion++;
                            }
                        }
                        else
                        {
                            auto sparg = candidate->tp->BaseType()->syms->begin();
                            auto spend = candidate->tp->BaseType()->syms->end();
                            if (sparg != spend)
                            {
                                if (candidate_in && candidate_in->sb->isConstructor &&
                                    candidate_in->sb->parentClass == candidate->sb->parentClass)
                                {
                                    seq3[n2++] = CV_NONE;
                                }
                                else
                                {
                                    auto th = *sparg;
                                    ++sparg;
                                    SYMBOL* first = *sparg;
                                    ++sparg;
                                    SYMBOL* next = sparg == spend ? nullptr : *sparg;
                                    if (!next || next->sb->init || initTokenStreams.get(next))
                                    {
                                        if (first->tp->type != BasicType::ellipse_)
                                        {
                                            getSingleConversion(first->tp, tpa, expa, &n2, seq3, candidate, nullptr, true);
                                            if (n2 && seq3[n2 - 1] == CV_IDENTITY)
                                            {
                                                n2--;
                                            }
                                        }
                                        seq3[n2 + n3++] = CV_USER;
                                        getSingleConversion(tppp, th->tp->BaseType()->btp->BaseType()->sp->tp, &exp, &n3, seq3 + n2,
                                                            candidate, nullptr, true);
                                    }
                                    else
                                    {
                                        seq3[n2++] = CV_NONE;
                                    }
                                }
                            }
                        }
                        for (j = 0; j < n2 + n3; j++)
                            if (seq3[j] == CV_NONE)
                                break;
                        m1 = n2 + n3;
                        while (m1 && seq3[m1 - 1] == CV_IDENTITY)
                            m1--;
                        if (j >= n2 + n3 && m1 <= 7)
                        {
                            lenList[i] = Allocate<int>(2);
                            icsList[i] = Allocate<e_cvsrn>(n2 + n3);
                            lenList[i][0] = n2;
                            lenList[i][1] = n3;
                            memcpy(&icsList[i][0], seq3, (n2 + n3) * sizeof(e_cvsrn));
                        }
                        else
                        {
                            spList[i] = nullptr;
                        }
                    }
                }
            }
            SelectBestFunc(spList, icsList, lenList, &funcparams, 2, funcs, nullptr);
            WeedTemplates(spList, funcs, &funcparams, nullptr);
            found1 = found2 = nullptr;
            for (int i = 0; i < funcs && !found1; i++)
            {
                if (spList[i] && spList[i]->sb->isConstructor)
                {
                    auto itParams = spList[i]->tp->syms->begin();
                    auto itParamsEnd = spList[i]->tp->syms->end();
                    ++itParams;
                    if (itParams != itParamsEnd && ((*itParams)->tp->CompatibleType(tpa) && (!tpa->IsArithmetic() || tpa->BaseType()->type == (*itParams)->tp->BaseType()->type) || SameTemplate((*itParams)->tp, tpa)))
                    {
                        ++itParams;
                        if (itParams == itParamsEnd || (*itParams)->sb->defaultarg)
                        {
                            m = i;
                            found1 = spList[i];
                        }
                    }
                }
            }
            if (!found1)
            {
                for (i = 0; i < funcs && !found1; i++)
                {
                    int j;
                    found1 = spList[i];
                    m = i;
                    for (j = i + 1; j < funcs && found1 && !found2; j++)
                    {
                        if (spList[j])
                        {
                            found2 = spList[j];
                        }
                    }
                }
            }
            if (found1)
            {
                if (!found2)
                {
                    if (honorExplicit && found1->sb->isExplicit)
                    {
                        error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
                    }
                    if (seq)
                    {
                        int l = lenList[m][0] + (found1->sb->castoperator ? lenList[m][1] : 1);
                        memcpy(&seq[*n], &icsList[m][0], l * sizeof(e_cvsrn));
                        *n += l;
                        if (userFunc)
                            *userFunc = found1;
                    }
                    inGetUserConversion--;
                    if (flags & F_CONVERSION)
                    {
                        if (found1->sb->templateLevel && !templateDefinitionLevel && found1->templateParams)
                        {
                            if (!currentlyInsertingFunctions || processingTemplateArgs)
                                found1 = TemplateFunctionInstantiate(found1, false);
                        }
                    }
                    return found1;
                }
            }
        }
        inGetUserConversion--;
    }
    if (seq)
        seq[(*n)++] = CV_NONE;
    return nullptr;
}
void getQualConversion(Type* tpp, Type* tpa, EXPRESSION* exp, int* n, e_cvsrn* seq)
{
    bool hasconst = true, hasvol = true;
    bool sameconst = true, samevol = true;
    bool first = true;
    while (exp && IsCastValue(exp))
        exp = exp->left;
    bool strconst = false;
    while (tpa && tpp)  // && tpa->IsPtr() && tpp->IsPtr())
    {
        strconst = exp && exp->type == ExpressionNode::labcon_ && tpa->BaseType()->type == BasicType::char_;
        if (tpp->IsConst() != tpa->IsConst())
        {
            sameconst = false;
            if (tpa->IsConst() && !tpp->IsConst())
                break;
            if (!hasconst)
                break;
        }
        if (tpp->IsVolatile() != tpa->IsVolatile())
        {
            samevol = false;
            if (tpa->IsVolatile() && !tpp->IsVolatile())
                break;
            if (!hasvol)
                break;
        }

        if (!first)
        {
            if (!tpp->IsConst())
                hasconst = false;
            if (!tpp->IsVolatile())
                hasvol = false;
        }
        first = false;
        if (tpa->BaseType()->type == BasicType::enum_)
            tpa = tpa->BaseType()->btp;
        if (tpa->IsArray())
            while (tpa->IsArray())
                tpa = tpa->BaseType()->btp;
        else
            tpa = tpa->BaseType()->btp;
        if (tpp->BaseType()->type == BasicType::enum_)
            tpp = tpp->BaseType()->btp;
        if (tpp->IsArray())
            while (tpp->IsArray())
                tpp = tpp->BaseType()->btp;
        else
            tpp = tpp->BaseType()->btp;
    }
    if ((!tpa && !tpp) || (tpa && tpp && tpa->type != BasicType::pointer_ && tpp->type != BasicType::pointer_))
    {
        if (tpa && tpp && ((hasconst && tpa->IsConst() && !tpp->IsConst()) || (hasvol && tpa->IsVolatile() && !tpp->IsVolatile())))
            seq[(*n)++] = CV_NONE;
        else if (!sameconst || !samevol)
            seq[(*n)++] = CV_QUALS;
        else if (strconst && (!tpp || !tpp->IsConst()))
            seq[(*n)++] = CV_QUALS;
        else
            seq[(*n)++] = CV_IDENTITY;
    }
    else
    {
        seq[(*n)++] = CV_NONE;
    }
}
static void getPointerConversion(Type* tpp, Type* tpa, EXPRESSION* exp, int* n, e_cvsrn* seq)
{
    if (tpa->BaseType()->btp->type == BasicType::void_ && exp &&
        (isconstzero(&stdint, exp) || exp->type == ExpressionNode::nullptr_))
    {
        seq[(*n)++] = CV_POINTERCONVERSION;
        return;
    }
    else
    {
        if (tpa->BaseType()->array)
            seq[(*n)++] = CV_ARRAYTOPOINTER;
        if (tpa->BaseType()->btp->IsFunction())
            seq[(*n)++] = CV_FUNCTIONTOPOINTER;
        if (tpp->BaseType()->btp->BaseType()->type == BasicType::void_)
        {
            if (tpa->BaseType()->btp->BaseType()->type != BasicType::void_)
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            if (tpa->BaseType()->btp->IsPtr())
            {
                if ((tpa->IsConst() && !tpp->IsConst()) || (tpa->IsVolatile() && !tpp->IsVolatile()))
                    seq[(*n)++] = CV_NONE;
                else if ((tpp->IsConst() != tpa->IsConst()) || (tpa->IsVolatile() != tpp->IsVolatile()))
                    seq[(*n)++] = CV_QUALS;
                return;
            }
        }
        else if (tpp->BaseType()->btp->IsStructured() && tpa->BaseType()->btp->IsStructured())
        {
            SYMBOL* base = tpp->BaseType()->btp->BaseType()->sp;
            SYMBOL* derived = tpa->BaseType()->btp->BaseType()->sp;

            if (base != derived && !base->tp->CompatibleType(derived->tp) && !SameTemplate(base->tp, derived->tp))
            {
                int v = classRefCount(base, derived);
                if (v != 1)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    seq[(*n)++] = CV_DERIVEDFROMBASE;
                }
            }
        }
        else
        {
            Type* t1 = tpp;
            Type* t2 = tpa;
            if (t2->IsArray() && t1->IsPtr())
            {
                while (t2->IsArray())
                    t2 = t2->BaseType()->btp;
                if (t1->IsArray())
                    while (t1->IsArray())
                        t1 = t1->BaseType()->btp;
                else
                    t1 = t1->BaseType()->btp;
            }
            if (tpa->BaseType()->nullptrType)
            {
                if (!tpp->BaseType()->nullptrType)
                {
                    if (tpa->IsPtr())
                        seq[(*n)++] = CV_POINTERCONVERSION;
                    else if (!tpp->BaseType()->nullptrType && exp  && !isconstzero(tpa->BaseType(), exp) &&
                             exp->type != ExpressionNode::nullptr_)
                        seq[(*n)++] = CV_NONE;
                }
            }
            else if (!t1->CompatibleType(t2))
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        getQualConversion(tpp, tpa, exp, n, seq);
    }
}

void GetRefs(Type* tpp, Type* tpa, EXPRESSION* expa, bool& lref, bool& rref)
{
    bool func = false;
    bool notlval = false;
    if (expa && expa->type == ExpressionNode::comma_ && expa->left->type == ExpressionNode::blockclear_)
        expa = expa->right;
    if (tpp)
    {
        Type* tpp1 = tpp;
        if (tpp1->IsRef())
            tpp1 = tpp1->BaseType()->btp;
        if (tpp1->IsStructured())
        {
            Type* tpa1 = tpa;
            if (tpa1->IsRef())
                tpa1 = tpa1->BaseType()->btp;
            if (!tpa1->IsStructured())
            {
                lref = false;
                rref = true;
                return;
            }
            else if (classRefCount(tpp1->BaseType()->sp, tpa1->BaseType()->sp) != 1 && !tpp1->CompatibleType(tpa1) &&
                     !SameTemplate(tpp1, tpa1))
            {
                lref = false;
                rref = true;
                return;
            }
        }
    }
    if (expa)
    {
        if (tpa->IsStructured())
        {
            // function call as an argument can result in an rref
            EXPRESSION* expb = expa;
            if (expb->type == ExpressionNode::thisref_)
                expb = expb->left;
            if (expb->type == ExpressionNode::callsite_ && expb->v.func->sp)
                if (expb->v.func->sp->tp->IsFunction())
                {
                    func = expb->v.func->sp->sb->isConstructor || expb->v.func->sp->tp->BaseType()->btp->IsStructured();
                }
            if (expa->type == ExpressionNode::not__lvalue_)
                notlval = true;
        }
    }
    lref = (tpa->BaseType()->type == BasicType::lref_ || tpa->lref || (tpa->IsStructured() && !notlval && !func) ||
            (expa && IsLValue(expa))) &&
           !tpa->rref;
    rref = (tpa->BaseType()->type == BasicType::rref_ || tpa->rref || notlval || func ||
            (expa && (isarithmeticconst(expa) || !IsLValue(expa) && !ismem(expa) && !ismath(expa) && !IsCastValue(expa)))) &&
           !lref && !tpa->lref;
}
void getSingleConversionWrapped(Type* tpp, Type* tpa, EXPRESSION* expa, int* n, e_cvsrn* seq, SYMBOL* candidate, SYMBOL** userFunc,
                                bool ref, bool allowUser)
{
    int rref = tpa->rref;
    int lref = tpa->lref;
    tpa->rref = false;
    tpa->lref = false;
    getSingleConversion(tpp, tpa, expa, n, seq, candidate, userFunc, allowUser, ref);
    tpa->rref = rref;
    tpa->lref = lref;
}
void arg_compare_bitint(Type* tpp, Type* tpa, int* n, e_cvsrn* seq)
{
    if (!tpp->IsInt() || !tpa->IsInt())
    {
        seq[(*n)++] = CV_NONE;
    }
    else
    {
        int tppb = 0, tpab = 0;
        if (tpp->type == BasicType::bitint_ || tpp->type == BasicType::unsigned_bitint_)
        {
            tppb = tpp->bitintbits;
        }
        else
        {
            tppb = CHAR_BIT * getSize(tpp->type);
        }
        if (tpa->type == BasicType::bitint_ || tpa->type == BasicType::unsigned_bitint_)
        {
            tpab = tpa->bitintbits;
        }
        else
        {
            tpab = CHAR_BIT * getSize(tpa->type);
        }
        if (tppb == tpab)
        {
            if (tpp->type == tpa->type)
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else
            {
                seq[(*n)++] = CV_INTEGRALCONVERSION;
            }
        }
        else if (tppb > tpab)
        {
            seq[(*n)++] = CV_INTEGRALPROMOTION;
        }
        else
        {
            seq[(*n)++] = CV_INTEGRALCONVERSION;
        }
    }
}
void getSingleConversion(Type* tpp, Type* tpa, EXPRESSION* expa, int* n, e_cvsrn* seq, SYMBOL* candidate, SYMBOL** userFunc,
    bool allowUser, bool ref)
{
    bool lref = false;
    bool rref = false;
    EXPRESSION* exp = expa;
    Type* tpax = tpa;
    Type* tppx = tpp;
    if (tpax->IsArray())
        tpax = tpax->BaseType();
    tpa = tpa->BaseType();
    tpp = tpp->BaseType();
    if (tpp->type == BasicType::any_)
    {
        seq[(*n)++] = CV_NONE;
        return;
    }
    // when evaluating decltype we sometimes come up with these
    if (tpa->type == BasicType::templateparam_)
        tpa = tpa->templateParam->second->byClass.val;
    if (!tpa)
    {
        seq[(*n)++] = CV_NONE;
        return;
    }
    while (expa && expa->type == ExpressionNode::comma_)
        expa = expa->right;
    if (tpp->type != tpa->type && (tpp->type == BasicType::void_ || tpa->type == BasicType::void_))
    {
        seq[(*n)++] = CV_NONE;
        return;
    }
    GetRefs(tpp, tpa, exp, lref, rref);
    if (exp && exp->type == ExpressionNode::thisref_)
        exp = exp->left;
    if (exp && exp->type == ExpressionNode::callsite_)
    {
        if (exp->v.func->sp->tp->BaseType()->type != BasicType::aggregate_)
        {
            Type* tp = exp->v.func->functp->BaseType()->btp->BaseType();
            if (tp)
            {
                if (tp->type == BasicType::rref_)
                {
                    if (!tpa->lref)
                    {
                        rref = true;
                        lref = false;
                    }
                }
                else if (tp->type == BasicType::lref_)
                {
                    if (!tpa->rref)
                    {
                        lref = true;
                        rref = false;
                    }
                }
            }
        }
    }
    if (tpa->IsRef())
    {
        if (tpa->BaseType()->type == BasicType::rref_)
        {
            rref = true;
            lref = false;
        }
        else if (tpa->BaseType()->type == BasicType::lref_)
        {
            lref = true;
            rref = false;
        }
        tpa = tpa->BaseType()->btp;
        while (tpa->IsRef())
            tpa = tpa->BaseType()->btp;
    }
    if (tpp->IsRef())
    {
        Type* tppp = tpp->BaseType()->btp;
        while (tppp->IsRef())
            tppp = tppp->BaseType()->btp;
        if (tpp->type == BasicType::any_)
        {
            seq[(*n)++] = CV_NONE;
            return;
        }
        if (tppp->BaseType()->type == BasicType::templateselector_)
        {
            seq[(*n)++] = CV_NONE;
            return;
        }
        if (tpp->IsInt() && tppp->BaseType()->btp && tppp->BaseType()->btp->type == BasicType::enum_)
            tppp = tppp->BaseType()->btp;
        if (!rref && expa && tppp->IsStructured() && expa->type != ExpressionNode::not__lvalue_)
        {
            EXPRESSION* expx = expa;
            if (expx->type == ExpressionNode::thisref_)
                expx = expx->left;
            if (expx->type == ExpressionNode::callsite_)
            {
                if (expx->v.func->returnSP)
                {
                    if (!expx->v.func->returnSP->sb->anonymous)
                        lref = true;
                }
            }
            else
            {
                lref = true;
            }
        }
        if (tpax->IsRef())
        {
            if ((tpa->IsConst() != tppp->IsConst()) || (tpa->IsVolatile() != tppp->IsVolatile()))
            {
                seq[(*n)++] = CV_QUALS;
            }
        }
        else
        {
            if (tpax->IsConst() != tppp->IsConst())
            {
                if (!tppp->IsConst() || !rref)
                    seq[(*n)++] = CV_QUALS;
            }
            else if (tpax->IsVolatile() != tppp->IsVolatile())
            {
                seq[(*n)++] = CV_QUALS;
            }
        }
        if (((tpa->IsConst() || tpax->IsConst()) && !tppp->IsConst()) ||
            ((tpa->IsVolatile() || tpax->IsVolatile()) && !tppp->IsVolatile() && !tppp->IsConst()))
        {
            if (tpp->type != BasicType::rref_)
                seq[(*n)++] = CV_NONE;
        }
        if (lref && !rref && tpp->type == BasicType::rref_)
            seq[(*n)++] = CV_LVALUETORVALUE;
        if (tpp->type == BasicType::rref_ && lref && !tpa->IsFunction() && !tpa->IsFunctionPtr() && !tpa->IsPtr() &&
            (expa && !isarithmeticconst(expa)))
        {
            // IsLValue to rvalue ref not allowed unless the IsLValue is nonvolatile and const
            if (!isDerivedFromTemplate(tppx) && (!tpax->IsConst() || tpax->IsVolatile()))
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpp->type == BasicType::lref_ && rref && !lref)
        {
            // rvalue to IsLValue reference not allowed unless the IsLValue is a function or const
            if (!tpp->BaseType()->btp->IsFunction() && tpp->BaseType()->btp->type != BasicType::aggregate_)
            {
                if (!tppp->IsConst())
                    seq[(*n)++] = CV_LVALUETORVALUE;
            }
        }
        tpa = tpa->BaseType();
        if (tpa->IsStructured())
        {
            if (tppp->IsStructured())
            {
                SYMBOL* s1 = tpa->BaseType()->sp;
                SYMBOL* s2 = tppp->BaseType()->sp;
                if (s1->sb->mainsym)
                    s1 = s1->sb->mainsym;
                if (s2->sb->mainsym)
                    s2 = s2->sb->mainsym;
                if (s1 != s2 && !SameTemplate(tppp, tpa))
                {
                    if (classRefCount(s2, s1) == 1)
                    {
                        seq[(*n)++] = CV_DERIVEDFROMBASE;
                    }
                    else if (s2->sb->trivialCons)
                    {
                        seq[(*n)++] = CV_NONE;
                    }
                    else
                    {
                        if (allowUser)
                            getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                        else
                            seq[(*n)++] = CV_NONE;
                    }
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                if (allowUser)
                    getUserConversion(0, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                else
                    seq[(*n)++] = CV_NONE;
            }
        }
        else if (tppp->IsStructured())
        {
            if (allowUser)
                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (tppp->IsFunctionPtr() || tppp->IsFunction())
        {
            if (tppp->IsFunctionPtr())
                tpp = tppp->BaseType()->btp;
            if (tpa->IsFunctionPtr())
                tpa = tpa->BaseType()->btp;
            if (tpp->CompatibleType(tpa))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (tpa->IsInt() && expa && (isconstzero(tpa, expa) || expa->type == ExpressionNode::nullptr_))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else
        {
            if (allowUser)
            {
                getSingleConversionWrapped(tppp, tpa, expa, n, seq, candidate, userFunc,
                                           !tppp->IsConst() && (tpp->BaseType()->type == BasicType::lref_ || !tppp->IsArithmetic()),
                                           allowUser);
            }
            else
                seq[(*n)++] = CV_NONE;
        }
    }
    else
    {
        if ((tpax->IsConst() != tppx->IsConst()) || (tpax->IsVolatile() != tppx->IsVolatile()))
            seq[(*n)++] = CV_QUALS;
        if (tpp->BaseType()->type == BasicType::templateselector_)
        {
            seq[(*n)++] = CV_NONE;
            return;
        }
        if (tpp->BaseType()->type == BasicType::string_)
        {
            if (tpa->BaseType()->type == BasicType::string_ || (expa && expa->type == ExpressionNode::labcon_ && expa->string))
                seq[(*n)++] = CV_IDENTITY;
            else
                seq[(*n)++] = CV_POINTERCONVERSION;
        }
        else if (tpp->BaseType()->type == BasicType::object_)
        {
            if (tpa->BaseType()->type == BasicType::object_)
                seq[(*n)++] = CV_IDENTITY;
            else
                seq[(*n)++] = CV_POINTERCONVERSION;
        }
        else if (tpp->IsPtr() && tpp->BaseType()->nullptrType)
        {
            if ((tpa->IsPtr() && tpa->BaseType()->nullptrType) || (expa && isconstzero(tpa, expa)))
            {
                if (tpa->BaseType()->type == BasicType::bool_)
                    seq[(*n)++] = CV_BOOLCONVERSION;
                else
                    seq[(*n)++] = CV_IDENTITY;
            }
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (tpa->IsStructured())
        {
            if (tpp->IsStructured())
            {
                if (tpa->BaseType()->sp == tpp->BaseType()->sp || SameTemplate(tpp, tpa))
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
                else if (classRefCount(tpp->BaseType()->sp, tpa->BaseType()->sp) == 1)
                {
                    seq[(*n)++] = CV_DERIVEDFROMBASE;
                }
                else if (tpp->BaseType()->sp->sb->trivialCons)
                {
                    if (!candidate->sb->isConstructor && !candidate->sb->castoperator && !candidate->sb->isAssign &&
                        lookupSpecificCast(tpa->BaseType()->sp, tpp))
                        getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                    else
                        seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if (allowUser)
                        getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                    else
                        seq[(*n)++] = CV_NONE;
                }
            }
            else
            {
                if (allowUser)
                    getUserConversion(0, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                else
                    seq[(*n)++] = CV_NONE;
            }
        }
        else if ((Optimizer::architecture == ARCHITECTURE_MSIL) && tpp->IsStructured())
        {
            if (tpa->BaseType()->nullptrType || (expa && isconstzero(tpa, expa)))
                seq[(*n)++] = CV_POINTERCONVERSION;
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (tpp->IsArray() && tpp->BaseType()->msil)
        {
            if (tpa->BaseType()->nullptrType || (expa && isconstzero(tpa, expa)))
                seq[(*n)++] = CV_POINTERCONVERSION;
            else if (tpa->IsArray() && tpa->BaseType()->msil)
                getSingleConversionWrapped(tpp->BaseType()->btp, tpa->BaseType()->btp, nullptr, n, seq, candidate, userFunc, false,
                                           allowUser);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (tpp->IsStructured())
        {
            if (allowUser)
                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (tpp->IsFunctionPtr())
        {
            Type* rv;
            tpp = tpp->BaseType()->btp;
            rv = tpp->BaseType()->btp;
            if (tpa->IsFunctionPtr())
            {
                tpa = tpa->BaseType()->btp;
                if (rv->type == BasicType::auto_)
                    tpp->BaseType()->btp = tpa->BaseType()->btp;
            }
            if (tpp->CompatibleType(tpa))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if ((tpa->IsInt() && expa && (isconstzero(tpa, expa) || expa->type == ExpressionNode::nullptr_)) ||
                     (tpa->type == BasicType::pointer_ && tpa->nullptrType))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
            tpp->BaseType()->btp = rv;
        }
        else if (tpp->BaseType()->nullptrType)
        {
            if (tpa->BaseType()->nullptrType ||
                (tpa->IsPtr() && expa && (isconstzero(tpa, expa) || expa->type == ExpressionNode::nullptr_)))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (tpa->IsInt() && expa && (isconstzero(tpa, expa) || expa->type == ExpressionNode::nullptr_))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpp->IsPtr())
        {
            auto tpx = tpp;
            while (tpx->IsPtr())
                tpx = tpx->BaseType()->btp;
            tpx = tpx->BaseType();
            if (tpx->type == BasicType::any_)
            {
                seq[(*n)++] = CV_NONE;
                return;
            }
            if (tpa->IsPtr())
            {
                if (tpp->IsVoidPtr())
                {
                    if (tpa->IsVoidPtr())
                        seq[(*n)++] = CV_IDENTITY;
                    else
                        seq[(*n)++] = CV_POINTERCONVERSION;
                }
                else
                {
                    // cvqual
                    getPointerConversion(tpp, tpa, expa, n, seq);
                }
            }
            else if (tpa->IsInt() && expa && (isconstzero(tpa, expa) || expa->type == ExpressionNode::nullptr_))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else if (tpp->IsVoidPtr() && (tpa->IsFunction() || tpa->type == BasicType::aggregate_))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpp->BaseType()->type == BasicType::memberptr_)
        {
            if (tpa->BaseType()->type == BasicType::memberptr_)
            {
                if (tpp->BaseType()->btp->CompatibleType(tpa->BaseType()->btp))
                {
                    if (tpa->BaseType()->sp != tpp->BaseType()->sp)
                    {
                        if (classRefCount(tpa->BaseType()->sp, tpp->BaseType()->sp) == 1)
                        {
                            seq[(*n)++] = CV_POINTERTOMEMBERCONVERSION;
                        }
                        else
                        {
                            if (allowUser)
                                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                            else
                                seq[(*n)++] = CV_NONE;
                        }
                    }
                    else
                    {
                        seq[(*n)++] = CV_IDENTITY;
                    }
                }
                else if (tpa->IsInt() && expa && (isconstzero(tpa, expa) || expa->type == ExpressionNode::nullptr_))
                {
                    seq[(*n)++] = CV_POINTERCONVERSION;
                }
                else
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
            else if (expa && ((isconstzero(tpa, expa) || expa->type == ExpressionNode::nullptr_)))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else if (tpa->IsFunction())
            {
                if (!tpp->BaseType()->btp->CompatibleType(tpa))
                    seq[(*n)++] = CV_NONE;

                else if (tpa->BaseType()->sp->sb->parentClass != tpp->BaseType()->sp &&
                         tpa->BaseType()->sp->sb->parentClass->sb->mainsym != tpp->sp &&
                         tpa->BaseType()->sp->sb->parentClass != tpp->BaseType()->sp->sb->mainsym)
                {
                    if (classRefCount(tpa->BaseType()->sp->sb->parentClass, tpp->BaseType()->sp) == 1)
                    {
                        seq[(*n)++] = CV_POINTERTOMEMBERCONVERSION;
                    }
                    else
                    {
                        if (allowUser)
                            getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                        else
                            seq[(*n)++] = CV_NONE;
                    }
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpa->IsFunction())
        {
            if (tpp->IsFunction() && tpp->CompatibleType(tpa))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (tpp->BaseType()->type == BasicType::bool_)
            {
                seq[(*n)++] = CV_BOOLCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpa->IsPtr())
        {
            if (tpp->BaseType()->type == BasicType::bool_)
            {
                seq[(*n)++] = CV_BOOLCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpa->BaseType()->type == BasicType::memberptr_)
        {
            seq[(*n)++] = CV_NONE;
        }
        else if (tpa->BaseType()->type == BasicType::enum_)
        {
            if (tpp->BaseType()->type == BasicType::enum_)
            {
                if (tpa->BaseType()->sp != tpp->BaseType()->sp && !SameTemplate(tpp, tpa))
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if ((tpax->IsConst() != tppx->IsConst()) || (tpax->IsVolatile() != tppx->IsVolatile()))
                        seq[(*n)++] = CV_QUALS;
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                if (tpp->IsInt() && !tpa->BaseType()->scoped)
                {
                    if (tpp->BaseType()->type == tpa->BaseType()->btp->type)
                        seq[(*n)++] = CV_INTEGRALCONVERSIONWEAK;
                    else
                        seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                }
                else
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
        else if (tpp->BaseType()->type == BasicType::enum_)
        {
            if (tpa->enumConst && tpa->btp)
            {
                tpa = tpa->btp;
                if (tpa->BaseType()->sp != tpp->BaseType()->sp)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if ((tpax->IsConst() != tppx->IsConst()) || (tpax->IsVolatile() != tppx->IsVolatile()))
                        seq[(*n)++] = CV_QUALS;
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else if (tpa->IsInt())
            {
                if (tpa->enumConst)
                {
                    if (tpa->sp == tpp->BaseType()->sp)
                        seq[(*n)++] = CV_IDENTITY;
                    else
                        seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if (tpp->scoped)
                    {
                        seq[(*n)++] = CV_NONE;
                    }
                    else
                    {
                        seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                    }
                }
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else
        {
            bool isenumconst = false;
            //            if ((tpax->IsConst() != tppx->IsConst()) || (tpax->IsVolatile() != tppx->IsVolatile()))
            //                seq[(*n)++] = CV_QUALS;
            if (tpa->enumConst)
            {
                seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                isenumconst = true;
            }
            if (tpp->BaseType()->type != tpa->BaseType()->type)
            {
                if (ref)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else if (tpa->IsInt())
                {
                    if (tpp->BaseType()->type == BasicType::bitint_ || tpp->BaseType()->type == BasicType::unsigned_bitint_ ||
                        tpa->BaseType()->type == BasicType::bitint_ || tpa->BaseType()->type == BasicType::unsigned_bitint_)
                    {
                        arg_compare_bitint(tpp, tpa, n, seq);
                    }
                    else if (tpp->BaseType()->type == BasicType::bool_)
                    {
                        seq[(*n)++] = CV_BOOLCONVERSION;
                    }
                    // take char of converting wchar_t to char
                    else if (tpa->BaseType()->type == BasicType::wchar_t_ && tpp->BaseType()->type == BasicType::char_)
                    {
                        seq[(*n)++] = CV_IDENTITY;
                    }
                    else if ((tpp->BaseType()->type == BasicType::int_ || tpp->BaseType()->type == BasicType::unsigned_) &&
                             tpa->BaseType()->type < tpp->BaseType()->type)
                    {
                        if (tpa->IsUnsigned() != tpp->IsUnsigned())
                            seq[(*n)++] = CV_INTEGRALCONVERSION;
                        seq[(*n)++] = CV_INTEGRALPROMOTION;
                    }
                    else if (tpp->IsBitInt() || tpa->IsBitInt())
                    {
                        if (tpa->IsUnsigned() != tpp->IsUnsigned())
                            seq[(*n)++] = CV_INTEGRALCONVERSION;
                        seq[(*n)++] = CV_INTEGRALPROMOTION;
                    }
                    else if (tpp->IsInt())
                    {
                        // this next along with a change in the ranking takes care of the case where
                        // long is effectively the same as int on some architectures.   It prefers a mapping between the
                        // two to a mapping between other integer types...
                        if (tpa->BaseType()->type == BasicType::bool_ || tpa->IsUnsigned() != tpp->IsUnsigned() ||
                            getSize(tpa->BaseType()->type) != getSize(tpp->BaseType()->type))
                            // take char of converting wchar_t to char
                            seq[(*n)++] = CV_INTEGRALCONVERSION;
                        else
                            seq[(*n)++] = CV_INTEGRALCONVERSIONWEAK;
                    }
                    else
                    {
                        seq[(*n)++] = CV_FLOATINGCONVERSION;
                        if (tpp->BaseType()->type == BasicType::float_)
                            seq[(*n)++] = CV_FLOATINGCONVERSION;
                        else if (tpp->BaseType()->type == BasicType::long_double_)
                            seq[(*n)++] = CV_FLOATINGPROMOTION;
                    }
                }
                else /* floating */
                {
                    if (tpp->BaseType()->type == BasicType::bool_)
                        seq[(*n)++] = CV_BOOLCONVERSION;
                    else if (tpp->IsInt())
                        seq[(*n)++] = CV_FLOATINGINTEGRALCONVERSION;
                    else if (tpp->IsFloat())
                    {
                        if (tpp->BaseType()->type == BasicType::double_)
                        {
                            if (tpa->BaseType()->type == BasicType::float_)
                                seq[(*n)++] = CV_FLOATINGPROMOTION;
                            else
                                seq[(*n)++] = CV_FLOATINGCONVERSION;
                        }
                        else
                        {
                            if (tpp->BaseType()->type < tpa->BaseType()->type)
                                seq[(*n)++] = CV_FLOATINGCONVERSION;
                            else
                                seq[(*n)++] = CV_FLOATINGPROMOTION;
                        }
                    }
                    else
                        seq[(*n)++] = CV_NONE;
                }
            }
            else if (!isenumconst)
            {
                if (tpp->IsBitInt())
                {
                    arg_compare_bitint(tpp, tpa, n, seq);
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
        }
    }
}
static void getInitListConversion(Type* tp, std::list<Argument*>* list, Type* tpp, int* n, e_cvsrn* seq, SYMBOL* candidate,
                                  SYMBOL** userFunc)
{
    if (tp->IsStructured() || (tp->IsRef() && tp->BaseType()->btp->IsStructured()))
    {
        if (tp->IsRef())
            tp = tp->BaseType()->btp;
        tp = tp->BaseType();
        if (tp->sp->sb->trivialCons)
        {
            if (list)
            {
                auto it = list->begin();
                auto ite = list->end();
                if (it != ite && !(*it)->nested)
                {
                    while (it != ite)
                    {
                        if ((*it)->tp->SameType(tp) || SameTemplate((*it)->tp, tp))
                        {
                            getSingleConversion(tp, (*it)->tp, (*it)->exp, n, seq, candidate, userFunc, true);
                            ++it;
                        }
                        else
                        {
                            bool changed = false;
                            for (auto member : *tp->syms)
                            {
                                if (it == ite)
                                    break;
                                if (ismemberdata(member))
                                {
                                    getSingleConversion(member->tp, (*it)->tp, (*it)->exp, n, seq, candidate, userFunc, true);
                                    if (*n > 10)
                                        break;
                                    ++it;
                                    changed = true;
                                }
                            }
                            if (!changed)
                                break;
                        }
                        if (*n > 10)
                            break;
                    }
                }
                if (it != ite)
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
        else
        {
            SYMBOL* cons = search(tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
            if (!cons)
            {
                // should never happen
                seq[(*n)++] = CV_NONE;
            }
            else
            {
                std::deque<EXPRESSION*> hold;
                EXPRESSION exp = {}, *expp = &exp;
                Type* ctype = cons->tp;
                Type thistp = {};
                CallSite funcparams = {};
                funcparams.arguments = list;
                exp.type = ExpressionNode::c_i_;
                Type::MakeType(thistp, BasicType::pointer_, tp->BaseType());
                funcparams.thistp = &thistp;
                funcparams.thisptr = &exp;
                funcparams.ascall = true;
                cons = GetOverloadedFunction(&ctype, &expp, cons, &funcparams, nullptr, false, true, _F_SIZEOF);
                if (!cons)
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
    }
    else if (tp->IsPtr())
    {
        if (list)
        {
            Type* btp = tp;
            int x;
            while (btp->IsArray())
                btp = btp->BaseType()->btp;
            x = tp->size / btp->size;
            for (auto a : *list)
            {
                getSingleConversion(btp, a->tp, a->exp, n, seq, candidate, userFunc, true);
                if (*n > 10)
                    break;
                if (--x < 0)  // too many items...
                {
                    seq[(*n)++] = CV_NONE;
                    break;
                }
            }
        }
    }
    else if (list)
    {
        for (auto a : *list)
        {
            if (a->nested)
            {
                for (auto b : *a->nested)
                {
                    getSingleConversion(tp, b->tp, b->exp, n, seq, candidate, userFunc, true);
                }
            }
            else
            {
                getSingleConversion(tp, a->tp, a->exp, n, seq, candidate, userFunc, true);
            }
        }
    }
}
static bool getFuncConversions(SYMBOL* sym, CallSite* f, Type* atp, SYMBOL* parent, e_cvsrn arr[], int* sizes, int count,
                               SYMBOL** userFunc, bool usesInitList, bool deduceTemplate)
{
    (void)usesInitList;
    int pos = 0;
    int n = 0;
    int i;
    std::list<Argument*> a;
    e_cvsrn seq[500];
    Type* initializerListType = nullptr;
    int m = 0, m1;
    if (sym->tp->type == BasicType::any_)
        return false;

    auto it = sym->tp->BaseType()->syms->begin();
    auto ite = sym->tp->BaseType()->syms->end();
    SymbolTable<SYMBOL>::iterator itt;
    if (f)
    {
        if (f->arguments)
            a = std::list<Argument*>(*f->arguments);
    }
    else
        itt = atp->syms->begin();
    auto ita = a.begin();
    auto itae = a.end();
    for (i = 0; i < count; i++)
    {
        arr[i] = CV_PAD;
    }
    /* takes care of THIS pointer */
    if (sym->sb->castoperator)
    {
        Type tpx;
        Type* tpp;
        SYMBOL* argsym = *it;
        memset(&tpx, 0, sizeof(tpx));
        m = 0;
        getSingleConversion(parent->tp, sym->tp->BaseType()->btp, nullptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr, false);
        m1 = m;
        while (m1 && seq[m1 - 1] == CV_IDENTITY)
            m1--;
        if (m1 > 10)
        {
            return false;
        }
        for (i = 0; i < m; i++)
            if (seq[i] == CV_NONE)
                return false;
        memcpy(arr + pos, seq, m * sizeof(e_cvsrn));
        sizes[n++] = m;
        pos += m;
        ++it;
        tpp = argsym->tp;
        // f shouldn't be null here but just in case...
        Type::MakeType(tpx, BasicType::pointer_, f ? f->arguments->front()->tp : &stdint);
        m = 0;
        seq[m++] = CV_USER;
        getSingleConversion(tpp, &tpx, f->thisptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr, true);
        m1 = m;
        while (m1 && seq[m1 - 1] == CV_IDENTITY)
            m1--;
        if (m1 > 10)
        {
            return false;
        }
        for (i = 0; i < m; i++)
            if (seq[i] == CV_NONE)
                return false;
        memcpy(arr + pos, seq, m * sizeof(e_cvsrn));
        sizes[n++] = m;
        pos += m;
        return true;
    }
    else
    {
        if (f)
        {
            if ((f->thistp || (ita != itae && (*ita)->tp)) && ismember(sym))
            {
                // nonstatic function
                Type* argtp = sym->tp;
                if (!argtp)
                {
                    arr[n++] = CV_NONE;
                    return false;
                }
                else if (deduceTemplate)
                {
                    ++it;
                }
                else
                {
                    Type tpx;
                    Type* tpp;
                    Type* tpthis = f->thistp;
                    SYMBOL* argsym = *it;
                    memset(&tpx, 0, sizeof(tpx));
                    ++it;
                    tpp = argsym->tp;
                    if (!tpthis)
                    {
                        tpthis = ita != itae ? (*ita)->tp : nullptr;
                        if (ita != itae)
                            ++ita;
                    }
                    if (sym->sb->castoperator || (tpthis && f->thistp == nullptr))
                    {
                        tpthis = &tpx;
                        auto tpconsider = f->arguments->front()->tp;
                        if (sym->sb->constexpression)
                        {
                            // remove CONST...
                            tpconsider = tpconsider->BaseType();
                        }
                        Type::MakeType(tpx, BasicType::pointer_, tpconsider);
                    }
                    else if (sym->sb->isDestructor)
                    {
                        tpthis = &tpx;
                        Type::MakeType(tpx, BasicType::pointer_, f->thistp->BaseType()->btp->BaseType());
                    }
                    if (sym->tp->IsLRefQual() || sym->tp->IsRRefQual())
                    {
                        auto thisptr = f->thistp ? f->thisptr : f->arguments->size() ? f->arguments->front()->exp : nullptr;
                        if (thisptr)
                        {
                            bool lref = IsLValue(thisptr);
                            auto strtype = tpthis->BaseType()->btp;
                            if (strtype->IsStructured() && thisptr->type != ExpressionNode::not__lvalue_)
                            {
                                if (strtype->lref)
                                    lref = true;
                                else if (!strtype->rref)
                                {
                                    EXPRESSION* expx = thisptr;
                                    if (expx->type == ExpressionNode::thisref_)
                                        expx = expx->left;
                                    if (expx->type == ExpressionNode::callsite_)
                                    {
                                        if (expx->v.func->returnSP)
                                        {
                                            if (!expx->v.func->returnSP->sb->anonymous)
                                                lref = true;
                                        }
                                    }
                                    else
                                    {
                                        lref = true;
                                    }
                                }
                            }
                            if (sym->tp->IsRRefQual())
                            {
                                if (lref)
                                    return false;
                            }
                            else if (!lref)
                                return false;
                        }
                    }
                    m = 0;
                    if (((f->thisptr && isconstexpr(f->thisptr)) ||
                         (!f->thisptr && f->arguments && f->arguments->front()->tp->IsConst())) &&
                        !sym->tp->IsConst())
                        seq[m++] = CV_QUALS;
                    getSingleConversion(tpp, tpthis, f->thisptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr, true);
                    m1 = m;
                    while (m1 && seq[m1 - 1] == CV_IDENTITY)
                        m1--;
                    if (m1 > 10)
                    {
                        return false;
                    }
                    for (i = 0; i < m; i++)
                        if (seq[i] == CV_NONE)
                            return false;
                    memcpy(arr + pos, seq, m * sizeof(e_cvsrn));
                    sizes[n++] = m;
                    pos += m;
                }
            }
        }
        else
        {
            if (ismember(sym))
            {
                Type* argtp = sym->sb->parentClass->tp;
                if (!argtp)
                {
                    return false;
                }
                else if (ita != itae || f && itt != atp->syms->end())
                {
                    getSingleConversion(argtp, ita != itae ? (*ita)->tp : (*itt)->tp, (ita != itae) ? (*ita)->exp : nullptr, &m,
                                        seq, sym, userFunc ? &userFunc[n] : nullptr, true);
                    if (ita != itae)
                        ++ita;
                    else if (f)
                        ++itt;
                }
            }
        }
        std::list<TEMPLATEPARAMPAIR>* tr = nullptr;
        std::list<TEMPLATEPARAMPAIR>::iterator itr, itre;
        while (it != ite && (ita != itae || (!f && itt != atp->syms->end())))
        {
            SYMBOL* argsym = *it;
            if (argsym->tp->type != BasicType::any_)
            {
                Type* tp;
                if (argsym->sb->constop)
                    break;
                if (argsym->sb->storage_class != StorageClass::parameter_)
                    return false;
                if (!tr && argsym->tp->type == BasicType::templateparam_ && argsym->tp->templateParam->second->packed)
                {
                    tr = argsym->tp->templateParam->second->byPack.pack;
                    if (tr)
                    {
                        itr = tr->begin();
                        itre = tr->end();
                    }
                }
                if (tr && itr != itre)
                    tp = itr->second->byClass.val;  // DAL not modified
                else
                    tp = argsym->tp;
                if (tp->BaseType()->type == BasicType::ellipse_)
                {
                    arr[pos] = CV_ELLIPSIS;
                    sizes[n++] = 1;
                    return true;
                }
                m = 0;
                Type* tp1 = tp;
                if (tp1->IsRef())
                    tp1 = tp1->BaseType()->btp;
                tp1->InstantiateDeferred();
                initializerListType = nullptr;
                if (tp1->IsStructured())
                {
                    SYMBOL* sym1 = tp1->BaseType()->sp;
                    if (sym1->sb->initializer_list && sym1->sb->templateLevel)
                    {
                        auto it = sym1->templateParams->begin();
                        ++it;
                        initializerListType = it->second->byClass.val;
                    }
                }
                else if (Optimizer::architecture != ARCHITECTURE_MSIL && tp1->IsArray() && (ita != itae) &&
                         (!(*ita)->tp || !(*ita)->tp->IsArray()))
                {
                    auto tp2 = tp1;
                    while (tp2->IsArray())
                        tp2 = tp2->BaseType()->btp;
                    auto tp3 = (*ita)->tp;
                    if (tp3 && tp3->IsPtr())
                    {
                        tp3 = tp3->BaseType()->btp;
                        if (tp2->BaseType()->type != tp3->BaseType()->type || !tp2->CompatibleType(tp3))
                            initializerListType = tp2;
                    }
                    else
                    {
                        initializerListType = tp2;
                    }
                }
                if (initializerListType)
                {
                    if (ita != itae && (*ita)->nested && (*ita)->nested->size())
                    {
                        if (initializerListType->IsStructured())
                        {
                            std::list<Argument*> nested;
                            if (!(*ita)->initializer_list)
                            {
                                nested.push_back((*ita)->nested->front());
                            }
                            else
                            {
                                nested.insert(nested.begin(), (*ita)->nested->begin(), (*ita)->nested->end());
                            }
                            getInitListConversion(initializerListType, &nested, nullptr, &m, seq, sym,
                                                  userFunc ? &userFunc[n] : nullptr);
                        }
                        else
                        {
                            std::list<Argument*> nested;
                            if (!(*ita)->initializer_list)
                            {
                                nested.push_back((*ita)->nested->front());
                            }
                            else
                            {
                                nested.insert(nested.begin(), (*ita)->nested->begin(), (*ita)->nested->end());
                            }
                            getInitListConversion(initializerListType, &nested, nullptr, &m, seq, sym,
                                                  userFunc ? &userFunc[n] : nullptr);
                            if ((*ita)->initializer_list && (*ita)->nested->front()->nested)
                                ++it;
                        }
                    }
                    else if ((*ita)->initializer_list)
                    {
                        int n1 = m;
                        getSingleConversion(initializerListType, ita != itae ? (*ita)->tp : (*itt)->tp,
                                            ita != itae ? (*ita)->exp : nullptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr,
                                            true);
                        for (int i = n1; i < m; i++)
                        {
                            if (seq[i] == CV_NONE)
                            {
                                seq[n1++] = CV_NONE;
                                m = n1;
                                break;
                            }
                        }
                        for (int i = n1; i < m; i++)
                        {
                            if (seq[i] == CV_USER)
                            {
                                seq[n1++] = CV_IDENTITY;
                                m = n1;
                                break;
                            }
                        }
                    }
                    else if (tp1->IsStructured() && (*ita)->tp && (*ita)->exp)  // might be an empty initializer list...
                    {
                        int n1 = m;
                        getSingleConversion((tp1->BaseType()->sp)->tp, ita != itae ? (*ita)->tp : (*itt)->tp,
                                            ita != itae ? (*ita)->exp : nullptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr,
                                            true);
                        for (int i = n1; i < m; i++)
                        {
                            if (seq[i] == CV_NONE)
                            {
                                seq[n1++] = CV_NONE;
                                m = n1;
                                break;
                            }
                        }
                        for (int i = n1; i < m; i++)
                        {
                            if (seq[i] == CV_USER)
                            {
                                seq[n1++] = CV_IDENTITY;
                                m = n1;
                                break;
                            }
                        }
                    }
                }
                else if (ita != itae && ((*ita)->nested || (!(*ita)->tp && !(*ita)->exp)))
                {
                    seq[m++] = CV_QUALS;  // have to make a distinction between an initializer list and the same func without one...
                    if (tp->BaseType()->type == BasicType::lref_)
                    {
                        seq[m++] = CV_LVALUETORVALUE;
                    }
                    if ((*ita)->nested && (*ita)->nested->size())
                    {
                        auto ita1 = ita;
                        ++ita1;
                        if ((*ita)->nested->front()->nested || (*ita)->initializer_list || ita1 != itae ||
                            (tp1->IsStructured() &&
                             (!sym->sb->isConstructor || (!tp1->BaseType()->CompatibleType(sym->sb->parentClass->tp) &&
                                                          !SameTemplate(tp1->BaseType(), sym->sb->parentClass->tp)))))
                        {
                            initializerListType = tp1->BaseType();
                            if (!sym->sb->parentClass || (!matchesCopy(sym, false) && !matchesCopy(sym, true)))
                            {
                                getInitListConversion(tp1->BaseType(), (*ita)->nested, nullptr, &m, seq, sym,
                                                      userFunc ? &userFunc[n] : nullptr);
                                if ((*ita)->initializer_list)
                                {
                                    ++it;
                                }
                            }
                            else
                            {
                                seq[m++] = CV_NONE;
                            }
                        }
                        else
                        {
                            // this didn't originally reset Keyword::end_
                            itae = (*ita)->nested->end();
                            ita = (*ita)->nested->begin();
                            if (ita != itae)
                            {
                                getSingleConversion(tp1,
                                                    ita != itae               ? (*ita)->tp
                                                    : itt != atp->syms->end() ? (*itt)->tp
                                                                              : tp1,
                                                    ita != itae ? (*ita)->exp : nullptr, &m, seq, sym,
                                                    userFunc ? &userFunc[n] : nullptr, true);
                            }
                        }
                    }
                }
                else
                {
                    if (ita != itae)
                    {
                        if ((*ita)->initializer_list)
                            seq[m++] = CV_QUALS;  // have to make a distinction between an initializer list and the same func
                                                  // without one...
                    }
                    Type* tp2 = tp;
                    if (tp2->IsRef())
                        tp2 = tp2->BaseType()->btp;
                    if (ita != itae && (*ita)->tp->type == BasicType::aggregate_ &&
                        (tp2->IsFunctionPtr() ||
                         (tp2->BaseType()->type == BasicType::memberptr_ && tp2->BaseType()->btp->IsFunction())))
                    {
                        MatchOverloadedFunction(tp2, &(*ita)->tp, (*ita)->tp->sp, &(*ita)->exp, 0);
                    }
                    getSingleConversion(tp, ita != itae ? (*ita)->tp : (*itt)->tp, ita != itae ? (*ita)->exp : nullptr, &m, seq,
                                        sym, userFunc ? &userFunc[n] : nullptr, true);
                }
                m1 = m;
                while (m1 && seq[m1 - 1] == CV_IDENTITY)
                    m1--;
                if (m1 > 10)
                {
                    return false;
                }
                for (i = 0; i < m; i++)
                    if (seq[i] == CV_NONE)
                        return false;
                memcpy(arr + pos, seq, m * sizeof(e_cvsrn));
                sizes[n++] = m;
                pos += m;
            }
            if (tr && itr != itre)
                ++itr;
            if (ita != itae)
                ++ita;
            else
                ++itt;
            if (it != ite && (!initializerListType || ita == itae || !(*ita)->initializer_list) && !tr)
                ++it;
        }
        if (it != ite)
        {
            SYMBOL* sym = *it;
            if (sym->sb->init  || sym->packed || initTokenStreams.get(sym))
            {
                return true;
            }
            if (sym->tp->BaseType()->type == BasicType::ellipse_)
            {
                sizes[n++] = 1;
                arr[pos++] = CV_ELLIPSIS;
                return true;
            }
            if (sym->tp->type == BasicType::void_ || sym->tp->type == BasicType::any_)
                return true;
            return false;
        }
        return ita == itae || ((*ita)->tp && (*ita)->tp->type == BasicType::templateparam_ &&
                               (*ita)->tp->templateParam->second->packed && !(*ita)->tp->templateParam->second->byPack.pack);
    }
}
SYMBOL* detemplate(SYMBOL* sym, CallSite* args, Type* atp)
{
    inDeduceArgs++;
    if (sym->sb->templateLevel)
    {
        if (atp || args)
        {
            bool linked = false;
            if (sym->sb->parentNameSpace && !sym->sb->parentNameSpace->sb->value.i)
            {
                Optimizer::LIST* list;
                SYMBOL* ns = sym->sb->parentNameSpace;
                linked = true;
                ns->sb->value.i++;

                nameSpaceList.push_front(ns);

                globalNameSpace->push_front(ns->sb->nameSpaceValues->front());
            }
            if (args && !FunctionTemplateCandidate(sym->templateParams, args->templateParams))
                sym = nullptr;
            else if (atp)
                sym = TemplateDeduceArgsFromType(sym, atp);
            else if (args && args->ascall)
                sym = TemplateDeduceArgsFromArgs(sym, args);
            else
            {
                if (args && !TemplateIntroduceArgs(sym->templateParams, args->templateParams))
                    sym = nullptr;
                else
                    sym = TemplateDeduceWithoutArgs(sym);
            }
            if (linked)
            {
                SYMBOL* sym = nameSpaceList.front();
                sym->sb->value.i--;
                nameSpaceList.pop_front();
                globalNameSpace->pop_front();
                ;
            }
        }
        else
        {
            sym = nullptr;
        }
    }
    inDeduceArgs--;
    return sym;
}
static int CompareArgs(SYMBOL* left, SYMBOL* right)
{
    int countl = 0, countr = 0;
    auto itl = left->sb->parentTemplate->tp->BaseType()->syms->begin();
    auto itlend = left->sb->parentTemplate->tp->BaseType()->syms->end();
    auto itr = right->sb->parentTemplate->tp->BaseType()->syms->begin();
    auto itrend = right->sb->parentTemplate->tp->BaseType()->syms->end();
    if ((*itl)->sb->thisPtr)
        ++itl;
    if ((*itr)->sb->thisPtr)
        ++itr;
    while (itl != itlend && itr != itrend)
    {
        auto tpl = (*itl)->tp;
        auto tpr = (*itr)->tp;
        if (tpl->IsRef())
            tpl = tpl->BaseType()->btp;
        if (tpr->IsRef())
            tpr = tpr->BaseType()->btp;
        while (tpl->IsPtr() && tpr->IsPtr())
        {
            tpl = tpl->BaseType()->btp;
            tpr = tpr->BaseType()->btp;
        }
        tpl = tpl->BaseType();
        tpr = tpr->BaseType();
        if (tpl->type != BasicType::templateparam_ && tpl->type != BasicType::templateselector_)
            countl++;
        if (tpr->type != BasicType::templateparam_ && tpr->type != BasicType::templateselector_)
            countr++;
        ++itl;
        ++itr;
    }
    if (countl > countr)
        return -1;
    if (countr > countl)
        return 1;
    return 0;
}
static void WeedTemplates(SYMBOL** table, int count, CallSite* args, Type* atp)
{
    int i = count;
    if (atp || !args->astemplate)
    {
        for (i = 0; i < count; i++)
            if (table[i] && (!table[i]->sb->templateLevel || !table[i]->templateParams))
                break;
    }
    else
    {
        for (i = 0; i < count; i++)
            if (table[i] && (!table[i]->sb->templateLevel || !table[i]->templateParams))
                table[i] = nullptr;
    }
    if (i < count)
    {
        // one or more first class citizens, don't match templates
        for (i = 0; i < count; i++)
            if (table[i] && table[i]->sb->templateLevel && table[i]->templateParams)
                table[i] = nullptr;
    }
    else
    {
        TemplatePartialOrdering(table, count, args, atp, false, true);
        // now we out nonspecializations if specializations are present
        int i;
        for (i = 0; i < count; i++)
        {
            if (table[i] && table[i]->sb->specialized)
                break;
        }
        if (i < count)
        {
            for (int i = 0; i < count; i++)
            {
                if (table[i] && !table[i]->sb->specialized)
                    table[i] = 0;
            }
        }
        int argCount = INT_MAX;
        int* counts = (int*)alloca(sizeof(int) * count);
        // choose the template with the smallest argument count
        // on the theory it is more specialized
        for (int i = 0; i < count; i++)
        {
            if (table[i])
            {
                if (table[i]->templateParams)
                    counts[i] = table[i]->templateParams->size();
                else
                    counts[i] = 0;

                if (counts[i] < argCount)
                    argCount = counts[i];
            }
            else
            {
                counts[i] = 0;
            }
        }
        for (int i = 0; i < count; i++)
        {
            if (table[i])
            {
                if (counts[i] > argCount)
                    table[i] = 0;
            }
        }
        // prefer templates that have args with a type that arent templateselectors or templateparams
        for (int i = 0; i < count - 1; i++)
        {
            if (table[i])
            {
                for (int j = i + 1; table[i] && j < count; j++)
                {
                    if (table[j])
                    {
                        switch (CompareArgs(table[i], table[j]))
                        {
                            case -1:
                                table[j] = nullptr;
                                break;
                            case 1:
                                table[i] = nullptr;
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }
    }
}
SYMBOL* GetOverloadedTemplate(SYMBOL* sp, CallSite* args)
{
    SYMBOL *found1 = nullptr, *found2 = nullptr;
    std::vector<SYMBOL*> spList;
    std::vector<e_cvsrn*> icsList;
    std::vector<int*> lenList;
    std::vector<SYMBOL**> funcList;
    std::list<SYMBOL*> gather;
    int n = 0, i, argCount = args->arguments ? args->arguments->size() : 0;
    gather.push_front(sp);
    n += sp->tp->syms->size();
    if (n == 0)
        return nullptr;
    spList.resize(n);
    icsList.resize(n);
    lenList.resize(n);
    funcList.resize(n);
    n = insertFuncs(&spList[0], gather, args, nullptr, 0);
    if (n != 1 || (spList[0] && !spList[0]->sb->isDestructor))
    {
        if (args->ascall)
        {
            GatherConversions(sp, &spList[0], n, args, nullptr, &icsList[0], &lenList[0], argCount, &funcList[0], 0, false);
            SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
        }
        WeedTemplates(&spList[0], n, args, nullptr);
        for (i = 0; i < n && !found1; i++)
        {
            int j;
            found1 = spList[i];
            for (j = i + 1; j < n && found1 && !found2; j++)
            {
                if (spList[j] && found1 != spList[j] && !SameTemplate(found1->tp, spList[j]->tp))
                {
                    found2 = spList[j];
                }
            }
        }
    }
    else
    {
        found1 = spList[0];
    }
    if (!found1 || found2)
    {
        return nullptr;
    }
    return found1;
}
void weedgathering(std::list<SYMBOL*>& gather)
{
    std::unordered_set<SYMBOL*> toRemove;
    std::set<SYMBOL*> set;
    for (auto p : gather)
        if (set.find(p) == set.end())
            set.insert(p);
        else
            toRemove.insert(toRemove.begin(), p);
    for (auto it = gather.begin(); it != gather.end();)
    {
        auto itr = toRemove.find(*it);
        if (itr != toRemove.end())
        {
            it = gather.erase(it);
            toRemove.erase(itr);
        }
        else
        {
            ++it;
        }
    }
}
static int insertFuncs(SYMBOL** spList, std::list<SYMBOL*>& gather, CallSite* args, Type* atp, int flags)
{
    std::set<SYMBOL*> filters;
    currentlyInsertingFunctions++;
    int n = 0;
    for (auto sp : gather)
    {
        for (auto sym : *sp->tp->syms)
        {
            if (filters.find(sym) == filters.end() && filters.find(sym->sb->mainsym) == filters.end() &&
                (!args || !args->astemplate || sym->sb->templateLevel) &&
                (!sym->sb->instantiated || sym->sb->specialized2 || sym->sb->isDestructor))
            {
                auto it1 = sym->tp->BaseType()->syms->begin();
                auto it1end = sym->tp->BaseType()->syms->end();
                bool ellipse = false;
                bool found = false;
                if (sym->name[0] == '.' || sym->sb->templateLevel)
                {
                    it1 = it1end;
                }
                else
                {
                    if ((*it1)->sb->thisPtr)
                        ++it1;
                    if ((*it1)->tp->type == BasicType::void_)
                        ++it1;
                    if (args && args->arguments && args->arguments->size())
                    {
                        auto ita = args->arguments->begin();
                        auto itae = args->arguments->end();
                        if (ita != itae && (*ita)->tp && (*ita)->tp->type == BasicType::void_)
                            ++ita;
                        while (ita != itae && it1 != it1end)
                        {

                            if ((*it1)->tp->type == BasicType::ellipse_ || !(*ita)->tp)  // Keyword::ellipse_ or initializer list
                                ellipse = true;
                            ++ita;
                            ++it1;
                        }
                        found = ita != itae;
                    }
                }
                if ((!found || ellipse) && (it1 == it1end || (*it1)->sb->defaultarg || (*it1)->tp->type == BasicType::ellipse_))
                {
                    if (sym->sb->templateLevel && (sym->templateParams || sym->sb->isDestructor))
                    {
                        if (sym->sb->castoperator)
                        {
                            spList[n] = args ?  detemplate(sym, nullptr, args->thistp->BaseType()->btp) : nullptr;
                        }
                        else
                        {
                            spList[n] = detemplate(sym, args, atp);
                        }
                    }
                    else
                    {
                        // instantiate arguments and the return value here
                        // so we can match the function properly...
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            InitializeFunctionArguments(sym);
                        }

                        spList[n] = sym;
                    }
                }
                filters.insert(sym);
                if (sym->sb->mainsym)
                    filters.insert(sym->sb->mainsym);
                n++;
            }
        }
    }
    currentlyInsertingFunctions--;
    return n;
}
static void doNames(SYMBOL* sym)
{
    if (sym->sb->parentClass)
        doNames(sym->sb->parentClass);
    SetLinkerNames(sym, Linkage::cdecl_);
}
static bool IsMove(SYMBOL* sp)
{
    bool rv = false;
    if (sp->sb->isConstructor)
    {
        auto it = sp->tp->BaseType()->syms->begin();
        auto itend = sp->tp->BaseType()->syms->end();
        auto thisPtr = it != itend ? *it : nullptr;
        if (thisPtr && thisPtr->sb->thisPtr)
            ++it;
        if (it != itend && thisPtr->sb->thisPtr)
        {
            if ((*it)->tp->BaseType()->type == BasicType::rref_)
            {
                auto tp1 = (*it)->tp->BaseType()->btp->BaseType();
                auto tp2 = thisPtr->tp->BaseType()->btp->BaseType();
                if (tp1->IsStructured() && tp2->IsStructured())
                {
                    rv = tp2->CompatibleType(tp1) || SameTemplate(tp2, tp1);
                }
            }
        }
    }
    return rv;
}
static bool ValidForDeduction(SYMBOL* s)
{
    if (s->templateParams)
    {
        for (auto t : *s->templateParams)
            if (t.second->type == TplType::int_)
                return false;
        if (s->templateParams->front().second->bySpecialization.types)
            for (auto t : *s->templateParams->front().second->bySpecialization.types)
            {
                if (t.second->type == TplType::int_)
                    return false;
                if (t.second->type == TplType::typename_)
                {
                    if (!t.second->byClass.dflt ||
                        (!t.second->byClass.dflt->IsArithmetic() && t.second->byClass.dflt->BaseType()->type != BasicType::enum_ &&
                         t.second->byClass.dflt->BaseType()->type != BasicType::templateparam_))
                    {
                        return false;
                    }
                }
            }
    }
    return true;
}
SYMBOL* ClassTemplateArgumentDeduction(Type** tp, EXPRESSION** exp, SYMBOL* sp, CallSite* args, int flags)
{
    std::vector<SYMBOL*> spList;
    SYMBOL* deduced = nullptr;
    SYMBOL* cons = search(sp->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    SYMBOL* deduce = search(sp->tp->BaseType()->syms, DG_NAME);
    int i = 0, firstDeduce = 0;
    sp->utilityIndex = 0;
    if (cons)
    {
        for (auto c : *cons->tp->syms)
        {
            c->utilityIndex = i++;
            if (c->tp->syms->size() == args->arguments->size() + 1)
            {
                if (ValidForDeduction(c))
                    spList.push_back(c);
            }
        }
    }
    if (sp->sb->specializations)
    {
        int j = 1;
        for (auto s : *sp->sb->specializations)
        {
            s->utilityIndex = j++;
            cons = search(s->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
            i = 0;
            for (auto c : *cons->tp->syms)
            {
                c->utilityIndex = i++;
                if (c->tp->syms->size() == args->arguments->size() + 1)
                {
                    if (ValidForDeduction(c))
                        spList.push_back(c);
                }
            }
        }
    }
    firstDeduce = i;
    if (deduce)
    {
        for (auto c : *deduce->tp->syms)
        {
            c->utilityIndex = i++;
            spList.push_back(c);
        }
    }
    if (spList.size())
    {
        std::list<std::list<TEMPLATEPARAMPAIR>*> toFree;
        std::list<std::pair<SYMBOL*, std::list<TEMPLATEPARAMPAIR>*>> hold;
        std::stack<std::list<TEMPLATEPARAMPAIR>*> stk;
        for (auto c : spList)
        {
            auto sp1 = c->sb->parentClass;
            hold.push_back(std::pair<SYMBOL*, std::list<TEMPLATEPARAMPAIR>*>(c, c->templateParams));
            std::list<TEMPLATEPARAMPAIR>* tpl = new std::list<TEMPLATEPARAMPAIR>();
            bool special_s = sp1->templateParams && sp1->templateParams->front().second->bySpecialization.types;
            bool special_c = c->templateParams && c->templateParams->front().second->bySpecialization.types;
            tpl->push_back(TEMPLATEPARAMPAIR(nullptr, Allocate<TEMPLATEPARAM>()));
            tpl->back().second->type = TplType::new_;
            toFree.push_back(tpl);
            if (c->templateParams)
                stk.push(c->templateParams);
            if (!c->sb->deductionGuide)
            {
                auto sp1 = c->sb->parentClass;
                while (sp1)
                {
                    if (sp1->templateParams)
                    {
                        stk.push(sp1->templateParams);
                        hold.push_back(std::pair<SYMBOL*, std::list<TEMPLATEPARAMPAIR>*>(sp1, sp1->templateParams));
                    }
                    sp1 = sp1->sb->parentClass;
                }
            }
            while (!stk.empty())
            {
                auto top = stk.top();
                stk.pop();
                for (auto t : *top)
                {
                    if (t.second->type == TplType::new_)
                    {
                        if (top == c->templateParams && t.second->bySpecialization.types)
                        {
                            auto x = tpl->front().second->bySpecialization.types;
                            if (!x)
                                x = tpl->front().second->bySpecialization.types = templateParamPairListFactory.CreateList();
                            for (auto s : *t.second->bySpecialization.types)
                            {
                                x->push_back(TEMPLATEPARAMPAIR{s.first, Allocate<TEMPLATEPARAM>()});
                                *x->back().second = *s.second;
                                x->back().second->flag = 0;
                            }
                        }
                    }
                    else
                    {
                        if (top == c->templateParams)
                        {
                            tpl->push_back(TEMPLATEPARAMPAIR{t.first, t.second});
                            tpl->back().second->flag = 1;
                        }
                        else
                        {
                            tpl->push_back(TEMPLATEPARAMPAIR{t.first, Allocate<TEMPLATEPARAM>()});
                            *tpl->back().second = *t.second;
                            tpl->back().second->flag = 0;
                        }
                    }
                }
            }
            if (tpl->front().second->bySpecialization.types)
                ClearArgValues(tpl->front().second->bySpecialization.types, false);
            c->templateParams = tpl;
        }
        std::vector<e_cvsrn*> icsList;
        std::vector<int*> lenList;
        std::vector<SYMBOL**> funcList;
        int n = spList.size();
        icsList.resize(n);
        lenList.resize(n);
        funcList.resize(n);

        std::list<std::list<TEMPLATEPARAMPAIR>*> stk2;
        {
            TemplateNamespaceScope namespaceScope(sp);
            for (auto&& s : spList)
            {
                auto sp1 = s->sb->parentClass;
                while (sp1)
                {
                    stk2.push_back(sp1->templateParams ? sp1->templateParams->front().second->bySpecialization.types : nullptr);
                    sp1 = sp1->sb->parentClass;
                }
                auto s1 = detemplate(s, args, nullptr);
                sp1 = s->sb->parentClass;
                for (auto tpl : stk2)
                {
                    if (tpl)
                        sp1->templateParams->front().second->bySpecialization.types = tpl;
                    sp1 = sp1->sb->parentClass;
                }
                stk2.clear();
                s = s1;
            }
        }
        SYMBOL *found1 = nullptr, *found2 = nullptr;
        if (n != 1)
        {
            bool hasDest = false;

            std::unordered_map<int, SYMBOL*> storage;
            GatherConversions(sp, &spList[0], n, args, nullptr, &icsList[0], &lenList[0], args->arguments->size(), &funcList[0], 0,
                              true);
            for (int i = 0; i < n; i++)
            {
                if (spList[i])
                {
                    if (!allTemplateArgsSpecified(spList[i], spList[i]->templateParams, false))
                        spList[i] = nullptr;
                    else
                        hasDest |= spList[i]->sb->deleted;
                }
            }
            SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, args->arguments->size(), n, &funcList[0]);
//            WeedTemplates(&spList[0], n, args, nullptr);
            for (int i = 0; i < n && !found1; i++)
            {
                if (spList[i] && !spList[i]->sb->deleted)
                    found1 = spList[i];
            }
            for (int i = 0; i < n; i++)
            {
                int j;
                if (!found1)
                    found1 = spList[i];
                for (j = i; j < n && found1; j++)
                {
                    if (spList[j] && found1 != spList[j] && !SameTemplate(found1->tp, spList[j]->tp))
                    {
                        // prefer an explicit guide...
                        if (spList[j]->utilityIndex >= firstDeduce && found1->utilityIndex < firstDeduce)
                            found1 = spList[j];
                        else
                            found2 = spList[j];
                    }
                }
                if (found1)
                    break;
            }
            if ((!found1 || (!IsMove(found1) && found1->sb->deleted)) && hasDest)
            {
                auto found3 = found1;
                auto found4 = found2;
                // there were no matches.   But there are deleted functions
                // see if we can find a match among them...
                found1 = found2 = 0;
                for (auto v : storage)
                    if (!v.second || !v.second->sb->deleted)
                        spList[v.first] = v.second;
                    else
                        spList[v.first] = nullptr;
                SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, args->arguments->size(), n, &funcList[0]);
                WeedTemplates(&spList[0], n, args, nullptr);
                for (int i = 0; i < n && !found1; i++)
                {
                    if (spList[i] && !spList[i]->sb->deleted)
                        found1 = spList[i];
                }
                for (int i = 0; i < n; i++)
                {
                    int j;
                    if (!found1)
                        found1 = spList[i];
                    for (j = i; j < n && found1 && !found2; j++)
                    {
                        if (spList[j] && found1 != spList[j] && !SameTemplate(found1->tp, spList[j]->tp))
                        {
                            found2 = spList[j];
                        }
                    }
                    if (found1)
                        break;
                }
                if (!found1)
                {
                    found1 = found3;
                    found2 = found4;
                }
            }
            if (found1 && found2 && !found1->sb->deleted && found2->sb->deleted)
                found2 = nullptr;
        }
        else
        {
            found1 = spList[0];
        }
        if (found1)
        {
            if (found1->sb->deleted)
            {
                errorsym(ERR_DELETED_FUNCTION_REFERENCED, found1);
            }
            else
            {
                if (found1->sb->deductionGuide)
                {
                    deduced = found1->sb->deductionGuide->sp;
                }
                else
                {
                    deduced = found1->sb->parentClass;
                }
                auto itparams = found1->templateParams->begin();
                auto iteparams = found1->templateParams->end();
                auto itspecial =
                    itparams->second->bySpecialization.types ? itparams->second->bySpecialization.types->begin() : itparams;
                auto itespecial =
                    itparams->second->bySpecialization.types ? itparams->second->bySpecialization.types->end() : iteparams;
                for (; itspecial != itespecial; ++itspecial)
                {
                    if (itspecial->second->type != TplType::new_)
                    {
                        std::list<TEMPLATEPARAMPAIR> a(itspecial, itespecial);
                        TransferClassTemplates(deduced->templateParams, deduced->templateParams, &a);
                    }
                }
                deduced = GetClassTemplate(deduced, deduced->templateParams, true);
                if (deduced)
                {
                    deduced->tp->InitializeDeferred();
                    auto ctype = deduced->tp;
                    auto thstp = Type::MakeType(BasicType::pointer_, deduced->tp);
                    if (args->thistp->IsConst())
                        thstp = Type::MakeType(BasicType::const_, thstp);
                    if (args->thistp->IsVolatile())
                        thstp = Type::MakeType(BasicType::volatile_, thstp);
                    args->thistp = thstp;
                    if (!sp->sb->trivialCons)
                    {
                        auto overloads = search(deduced->tp->syms, overloadNameTab[CI_CONSTRUCTOR]);
                        if (overloads)
                        {
                            *exp = nullptr;
                            SYMBOL* s = GetOverloadedFunction(&ctype, exp, overloads, args, nullptr, false, false, 0);
                            if (!*exp)
                                deduced = nullptr;
                        }
                        else
                        {
                            deduced = nullptr;
                        }
                    }
                }
            }
        }

        for (auto x : toFree)
        {
            delete x;
        }
        for (auto&& t : hold)
        {
            t.first->templateParams = t.second;
        }
    }
    return deduced;
}
int count3;
SYMBOL* GetOverloadedFunction(Type** tp, EXPRESSION** exp, SYMBOL* sp, CallSite* args, Type* atp, int toErr, bool maybeConversion,
                              int flags)
{
    if (atp && atp->IsPtr())
        atp = atp->BaseType()->btp;
    if (atp && !atp->IsFunction())
        atp = nullptr;
    DeclarationScope scope;
    if (args)
    {
        if (Optimizer::cparams.prm_cplusplus)
        {
            if (args->thistp)
            {
                args->thistp->InstantiateDeferred();
            }
            if (args->returnSP)
            {
                args->returnSP->tp->InstantiateDeferred();
            }
            if (args->templateParams)
                for (auto&& a : *args->templateParams)
                {
                    if (a.second->type == TplType::typename_)
                    {
                        if (a.second->packed)
                        {
                            if (a.second->byPack.pack)
                            {
                                for (auto&& b : *a.second->byPack.pack)
                                {
                                    if (b.second->byClass.dflt)
                                    {
                                        b.second->byClass.dflt->InstantiateDeferred();
                                        b.second->byClass.dflt = ResolveTemplateSelectors(nullptr, b.second->byClass.dflt);
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (a.second->byClass.dflt)
                            {
                                a.second->byClass.dflt->InstantiateDeferred();
                                //                                a.second->byClass.dflt = ResolveTemplateSelectors(nullptr,
                                //                                a.second->byClass.dflt);
                            }
                        }
                    }
                }
        }
        if (args->thisptr)
        {
            auto tp2 = args->thistp;
            while (tp2->IsPtr() || tp2->IsRef()) tp2 = tp2->BaseType()->btp;
            SYMBOL* spt = tp2->BaseType()->sp;
            if (spt->templateParams)
            {
                enclosingDeclarations.Add(spt->templateParams);
            }
        }
        if (args->templateParams)
        {
            enclosingDeclarations.Add(args->templateParams);
        }
    }
    if (!sp || sp->sb->storage_class == StorageClass::overloads_)
    {
        std::list<SYMBOL*> gather;
        SYMBOL *found1 = nullptr, *found2 = nullptr;
        if (!Optimizer::cparams.prm_cplusplus && ((Optimizer::architecture != ARCHITECTURE_MSIL) ||
                                                  !Optimizer::cparams.msilAllowExtensions || (sp && sp->tp->syms->size() == 1)))
        {
            if (sp->tp->syms->begin() != sp->tp->syms->end())
            {
                sp = *sp->tp->syms->begin();
                *exp = MakeExpression(ExpressionNode::pc_, sp);
                *tp = sp->tp;
            }
            return nullptr;
        }
        if (sp)
        {
            if (args || atp)
            {
                if ((!sp->tp || (!sp->sb->wasUsing && !sp->sb->parentClass && !args->nameSpace)))
                {
                    // ok the sp is a valid candidate for argument search
                    if (args)
                    {
                        if (args->arguments)
                        {
                            for (auto item : *args->arguments)
                            {
                                if (item->tp)
                                    searchOneArg(sp, gather, item->tp);
                            }
                            if (args->thisptr)
                                searchOneArg(sp, gather, args->thistp);
                        }
                    }
                    else
                    {
                        for (auto sp : *atp->syms)
                        {
                            if (sp->sb->storage_class != StorageClass::parameter_)
                                break;
                            searchOneArg(sp, gather, sp->tp);
                        }
                    }
                }
                weedToFunctions(gather);
            }
            if (sp->tp)
            {
                bool found = false;
                if (args->nameSpace)
                {
                    unvisitUsingDirectives(args->nameSpace);
                    tablesearchinline(gather, sp->name, args->nameSpace, false, true);
                }
                found = false;
                for (auto sym : gather)
                    if (sym == sp)
                    {
                        found = true;
                        break;
                    }
                if (!found)
                {
                    gather.push_front(sp);
                }
            }
            weedgathering(gather);
        }
        // ok got the initial list, time for phase 2
        // which is to add any other functions that have to be added...
        // constructors, member operator '()' and so forth...
        if (gather.size())
        {
            // we are only doing global functions for now...
            // so nothing here...
        }
        if (maybeConversion)
        {
            if (args->arguments && args->arguments->size() == 1 && !args->arguments->front()->nested)  // one arg
                GetMemberCasts(gather, args->arguments->front()->tp->BaseType()->sp);
        }
        // pass 3 - the actual argument-based resolution
        if (gather.size())
        {

            Optimizer::LIST* lst2;
            int n = 0;
            ResolveArgumentFunctions(args, toErr);
            for (auto sym1 : gather)
            {
                for (auto sym : *sym1->tp->syms)
                {
                    if ((!args || !args->astemplate || sym->sb->templateLevel) && (!sym->sb->instantiated || sym->sb->isDestructor))
                    {
                        n++;
                    }
                }
            }
            if (args || atp)
            {
                int i;
                std::vector<SYMBOL*> spList;
                std::vector<e_cvsrn*> icsList;
                std::vector<int*> lenList;
                std::vector<SYMBOL**> funcList;
                int argCount = 0;
                if (args)
                {
                    argCount = args->arguments ? args->arguments->size() : 0;
                    if (args->thisptr)
                        argCount++;
                }
                else
                {

                    for (auto sym : *atp->syms)
                    {
                        if (sym->sb->storage_class == StorageClass::parameter_)
                        {
                            argCount++;
                        }
                        else
                        {
                            if (ismember(sym))
                                argCount++;
                            break;
                        }
                    }
                }
                if (n == 0)
                {
                    return nullptr;
                }

                spList.resize(n);
                icsList.resize(n);
                lenList.resize(n);
                funcList.resize(n);
                n = insertFuncs(&spList[0], gather, args, atp, flags);
                if (n != 1 || (spList[0] && !spList[0]->sb->isDestructor && !spList[0]->sb->specialized2))
                {
                    bool hasDest = false;
                    bool hasImplicit = false;
                    std::unordered_map<int, SYMBOL*> storage;
                    if (atp || args->ascall)
                        GatherConversions(sp, &spList[0], n, args, atp, &icsList[0], &lenList[0], argCount, &funcList[0],
                                          flags & _F_INITLIST, false);
                    for (int i = 0; i < n; i++)
                    {
                        storage[i] = spList[i];
                        hasDest |= spList[i] && spList[i]->sb->deleted;
                        hasImplicit |= spList[i] && !spList[i]->sb->isExplicit;
                    }
                    if ((flags & _F_IMPLICIT) && hasImplicit)
                    {
                        for (int i = 0; i < n; i++)
                        {
                            if (spList[i] && spList[i]->sb->isExplicit)
                                spList[i] = nullptr;
                        }
                    }
                    if (atp || args->ascall)
                        SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
                    WeedTemplates(&spList[0], n, args, atp);
                    for (i = 0; i < n && !found1; i++)
                    {
                        if (spList[i] && !spList[i]->sb->deleted && !spList[i]->sb->castoperator)
                            found1 = spList[i];
                    }
                    for (i = 0; i < n && !found1; i++)
                    {
                        if (spList[i] && !spList[i]->sb->deleted)
                            found1 = spList[i];
                    }
                    for (i = 0; i < n; i++)
                    {
                        int j;
                        if (!found1)
                            found1 = spList[i];
                        for (j = i; j < n && found1; j++)
                        {
                            if (spList[j] && found1 != spList[j] && found1->sb->castoperator == spList[j]->sb->castoperator &&
                                !SameTemplate(found1->tp, spList[j]->tp))
                            {
                                found2 = spList[j];
                            }
                        }
                        if (found1)
                            break;
                    }
                    if ((!found1 || (!IsMove(found1) && found1->sb->deleted)) && hasDest)
                    {
                        auto found3 = found1;
                        auto found4 = found2;
                        // there were no matches.   But there are deleted functions
                        // see if we can find a match among them...
                        found1 = found2 = 0;
                        for (auto v : storage)
                            if (!v.second || !v.second->sb->deleted)
                                spList[v.first] = v.second;
                            else
                                spList[v.first] = nullptr;
                        if (atp || args->ascall)
                            SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
                        WeedTemplates(&spList[0], n, args, atp);
                        for (i = 0; i < n && !found1; i++)
                        {
                            if (spList[i] && !spList[i]->sb->deleted && !spList[i]->sb->castoperator)
                                found1 = spList[i];
                        }
                        for (i = 0; i < n && !found1; i++)
                        {
                            if (spList[i] && !spList[i]->sb->deleted)
                                found1 = spList[i];
                        }
                        for (i = 0; i < n; i++)
                        {
                            int j;
                            if (!found1)
                                found1 = spList[i];
                            for (j = i; j < n && found1 && !found2; j++)
                            {
                                if (spList[j] && found1 != spList[j] && found1->sb->castoperator == spList[j]->sb->castoperator &&
                                    !SameTemplate(found1->tp, spList[j]->tp))
                                {
                                    found2 = spList[j];
                                }
                            }
                            if (found1)
                                break;
                        }
                        if (!found1)
                        {
                            found1 = found3;
                            found2 = found4;
                        }
                    }
                    if (found1 && found2 && !found1->sb->deleted && found2->sb->deleted)
                        found2 = nullptr;
#if !NDEBUG
                    // this block to aid in debugging unfound functions...
                    if ((toErr & F_GOFERR) && !inDeduceArgs && (!found1 || (found1 && found2)) && !templateDefinitionLevel)
                    {
                        n = insertFuncs(&spList[0], gather, args, atp, flags);
                        if (atp || args->ascall)
                        {
                            GatherConversions(sp, &spList[0], n, args, atp, &icsList[0], &lenList[0], argCount, &funcList[0],
                                              flags & _F_INITLIST, false);
                            SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
                        }
                        WeedTemplates(&spList[0], n, args, atp);
                    }
#endif
                }
                else
                {
                    found1 = spList[0];
                }
            }
            else
            {
                auto it = gather.front()->tp->syms->begin();
                found1 = *it;
                if (n > 1 && ++it != gather.front()->tp->syms->end())
                    found2 = *it;
            }
        }
        // any errors
        if ((toErr & F_GOFERR) || (found1 && !found2))
        {
            if (!found1)
            {
                bool doit = true;

                // if we are in an argument list and there is an empty packed argument
                // don't generate an error on the theory there will be an ellipsis...
                if (flags & (_F_INARGS | _F_INCONSTRUCTOR))
                {
                    if (args->arguments)
                        for (auto arg : *args->arguments)
                        {
                            if (arg->tp && arg->tp->type == BasicType::templateparam_ && arg->tp->templateParam->second->packed)
                                doit = !!arg->tp->templateParam->second->byPack.pack;
                        }
                }
                if (doit)
                {
                    if (args && args->arguments && args->arguments->size() == 1  // one arg
                        && sp && sp->sb->isConstructor)                          // conversion constructor
                    {
                        errorConversionOrCast(true, args->arguments->front()->tp, sp->sb->parentClass->tp);
                    }
                    else if (!sp)
                    {
                        if (*tp && (*tp)->IsStructured())
                        {
                            char buf[UNMANGLE_BUFFER_SIZE], *p;
                            int n;
                            buf[0] = 0;
                            unmangle(buf, (*tp)->BaseType()->sp->sb->decoratedName);
                            n = strlen(buf);
                            p = (char*)strrchr(buf, ':');
                            if (p)
                                p++;
                            else
                                p = buf;
                            Utils::StrCpy(buf + n + 2, sizeof(buf) - n - 2,  p);
                            buf[n] = buf[n + 1] = ':';
                            Utils::StrCat(buf, "(");
                            if (args->arguments && args->arguments->size())
                            {
                                for (auto a : *args->arguments)
                                {
                                    a->tp->ToString(buf + sizeof(buf), buf + strlen(buf));
                                    Utils::StrCat(buf, ",");
                                }
                            }
                            Utils::StrCat(buf, ")");
                            errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, buf);
                        }
                        else
                        {
                            errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, "unknown");
                        }
                    }
                    else if ((!IsDefiningTemplate()) && !inDeduceArgs)
                    {
                        SYMBOL* sym = SymAlloc();
                        sym->sb->parentClass = sp->sb->parentClass;
                        sym->name = sp->name;
                        sym->sb->parentNameSpace = args->nameSpace ? args->nameSpace->name : nullptr;
                        if (atp)
                        {
                            sym->tp = atp;
                        }
                        else
                        {
                            int v = 1;
                            sym->tp = Type::MakeType(BasicType::func_, &stdint);
                            sym->tp->size = getSize(BasicType::pointer_);
                            sym->tp->syms = symbols->CreateSymbolTable();
                            sym->tp->sp = sym;
                            if (args->arguments)
                            {
                                for (auto a : *args->arguments)
                                {
                                    SYMBOL* sym1 = SymAlloc();
                                    char nn[100];
                                    Optimizer::my_sprintf(nn, "%d", v++);
                                    sym1->name = litlate(nn);
                                    sym1->tp = a->tp;
                                    sym->tp->syms->Add(sym1);
                                }
                            }
                        }
                        SetLinkerNames(sym, Linkage::cpp_);

                        errorsym(ERR_NO_OVERLOAD_MATCH_FOUND, sym);
                    }
                }
            }
            else if (found1 && found2)
            {
                if (toErr && !(flags & _F_INDECLTYPE))
                {
                    errorsym2(ERR_AMBIGUITY_BETWEEN, found1, found2);
                }
                else
                {
                    found1 = found2 = nullptr;
                }
            }
            else if (found1->sb->deleted && !templateDefinitionLevel)
            {
                if (toErr)
                    errorsym(ERR_DELETED_FUNCTION_REFERENCED, found1);
                else if (!(flags & _F_RETURN_DELETED))
                    found1 = nullptr;
            }
            if (found1)
            {
                if (flags & _F_IS_NOTHROW)
                    inNothrowHandler++;
                if (found1->sb->attribs.uninheritable.deprecationText)
                    deprecateMessage(found1);
                auto found10 = found1;
                auto stream = noExceptTokenStreams.get(found1);
                if (!(flags & _F_SIZEOF) || found1->tp->BaseType()->btp->IsAutoType() ||
                    ((flags & _F_IS_NOTHROW) && stream != 0 && stream != (LexemeStream*)-1))
                {
                    if (theCurrentFunc && !(flags & _F_NOEVAL) && !found1->sb->constexpression)
                    {
                        theCurrentFunc->sb->nonConstVariableUsed = true;
                    }
                    if (found1->sb->templateLevel && (found1->templateParams || found1->sb->isDestructor))
                    {
                        found1 = found1->sb->mainsym;
                        currentlyInsertingFunctions++;
                        if (found1->sb->castoperator)
                        {
                            found1 = detemplate(found1, nullptr, args->thistp->BaseType()->btp);
                        }
                        else
                        {
                            found1 = detemplate(found1, args, atp);
                        }
                        currentlyInsertingFunctions--;
                        found1->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                    }
                    else if (!found1->sb->templateLevel && found1->sb->parentClass && found1->sb->parentClass->templateParams &&
                             (!IsDefiningTemplate()) && !found1->sb->isDestructor)
                    {
                        auto old = found1;
                        if (found1->sb->mainsym)
                            found1 = found1->sb->mainsym;
                        found1 = CopySymbol(found1);
                        if (found1->sb->constructorInitializers && *found1->sb->constructorInitializers)
                        {
                            auto mi = *found1->sb->constructorInitializers;
                            found1->sb->constructorInitializers = Allocate<std::list<CONSTRUCTORINITIALIZER*>*>();
                            *found1->sb->constructorInitializers = constructorInitializerListFactory.CreateList();
                            (*found1->sb->constructorInitializers)->insert((*found1->sb->constructorInitializers)->end(), mi->begin(), mi->end());
                        }
                        found1->sb->mainsym = old;
                        found1->sb->parentClass = CopySymbol(found1->sb->parentClass);
                        found1->sb->parentClass->sb->mainsym = old->sb->parentClass;
                        found1->sb->parentClass->templateParams = copyParams(found1->sb->parentClass->templateParams, true);
                    }
                    if (found1->tp->IsFunction())
                    {
                        if (found1->tp->BaseType()->btp->IsStructured())
                        {
                            Type** tp1 = &found1->tp->BaseType()->btp;
                            while ((*tp1)->rootType != *tp1)
                                tp1 = &(*tp1)->btp;
                            *tp1 = (*tp1)->sp->tp;
                        }
                        for (auto sym : *found1->tp->BaseType()->syms)
                        {
                            CollapseReferences(sym->tp);
                        }
                        CollapseReferences(found1->tp->BaseType()->btp);
                        if (found1->sb->templateLevel && (!IsDefiningTemplate()) && found1->templateParams)
                        {
                            if (!currentlyInsertingFunctions || processingTemplateArgs)
                            {
                                found1 = TemplateFunctionInstantiate(found1, false);
                            }
                        }

                        if (found1->tp->BaseType()->btp->IsAutoType() &&
                            !found1->sb->inlineFunc.stmt && (!currentlyInsertingFunctions || processingTemplateArgs || (flags & _F_INDECLTYPE)) && bodyTokenStreams.get(found1))
                        {
                            CompileInlineFunction(found1);
                        }
                        else if ((flags & _F_IS_NOTHROW) ||
                                 (found1->sb->constexpression && (!IsDefiningTemplate())))
                        {
                            if ((!found1->sb->constexpression || IsDefiningTemplate()) && noExceptTokenStreams.get(found1))
                            {
                                if (found1->templateParams)
                                {
                                    enclosingDeclarations.Add(found1->templateParams);
                                }
                                StatementGenerator::ParseNoExceptClause(found1);
                                if (found1->templateParams)
                                {
                                    enclosingDeclarations.Drop();
                                }
                            }
                            else if (!!found1->sb->inlineFunc.stmt && bodyTokenStreams.get(found1))
                            {
                                if (!currentlyInsertingFunctions || processingTemplateArgs)
                                {
                                    CompileInlineFunction(found1);
                                }
                            }
                        }
                        if (found1->sb->inlineFunc.stmt)
                        {
                            noExcept &= found1->sb->noExcept;
                        }
                    }
                    else
                    {
                        found1 = nullptr;
                    }
                }
                else
                {
                    CollapseReferences(found1->tp->BaseType()->btp);
                }
                if (found1)
                {
                    if (found1->templateParams)
                        SetLinkerNames(found1, Linkage::cdecl_);
                    if (found1->tp->BaseType()->btp->IsAutoType())
                        errorsym(ERR_AUTO_FUNCTION_RETURN_TYPE_NOT_DEFINED, found1);
                }
                if (flags & _F_IS_NOTHROW)
                    inNothrowHandler--;
            }
        }
        if (!(toErr & F_GOFERR) && found2)
        {
            sp = nullptr;
        }
        else
        {
            sp = found1;
            if (sp)
            {
                sp->tp->BaseType()->btp->UpdateRootTypes();
                *exp = MakeExpression(ExpressionNode::pc_, sp);
                *tp = sp->tp;
            }
        }
    }
    return sp;
}
SYMBOL* MatchOverloadedFunction(Type* tp, Type** mtp, SYMBOL* sym, EXPRESSION** exp, int flags)
{
    CallSite fpargs;
    std::list<Argument*> args;
    fpargs.arguments = &args;
    EXPRESSION* exp2 = *exp;
    bool found = false;
    SymbolTable<SYMBOL>::iterator itp, itpe;
    tp = tp->BaseType();
    if (tp->IsFunctionPtr() || tp->type == BasicType::memberptr_)
    {
        itp = tp->BaseType()->btp->BaseType()->syms->begin();
        itpe = tp->BaseType()->btp->BaseType()->syms->end();
        found = true;
    }
    else
    {
        if (!*exp)
            return nullptr;
        if ((*exp)->v.func->sp->tp->syms)
        {
            SymbolTable<SYMBOL>* syms = (*exp)->v.func->sp->tp->syms;
            itp = syms->begin();
            if (itp != syms->end() && (*itp)->tp->syms)
            {
                itpe = (*itp)->tp->syms->end();
                itp = (*itp)->tp->syms->begin();
                found = true;
            }
        }
    }
    while (IsCastValue(exp2))
        exp2 = exp2->left;

    memset(&fpargs, 0, sizeof(fpargs));
    if (found && (*itp)->sb->thisPtr)
    {
        fpargs.thistp = (*itp)->tp;
        fpargs.thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
        ++itp;
    }
    else if (tp->type == BasicType::memberptr_)
    {
        fpargs.thistp = Type::MakeType(BasicType::pointer_, tp->sp->tp);
        fpargs.thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
    }
    if (found)
    {
        while (itp != itpe)
        {
            auto il = Allocate<Argument>();
            il->tp = ((*itp))->tp;
            il->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            if (il->tp->IsRef())
                il->tp = il->tp->BaseType()->btp;
            if (!fpargs.arguments)
                fpargs.arguments = argumentListFactory.CreateList();
            fpargs.arguments->push_back(il);
            ++itp;
        }
    }
    if (exp2->type == ExpressionNode::callsite_)
        fpargs.templateParams = exp2->v.func->templateParams;
    fpargs.ascall = true;
    return GetOverloadedFunction(mtp, exp, sym, &fpargs, nullptr, true, false, flags);
}
void ResolveArgumentFunctions(CallSite* args, bool toErr)
{
    if (args->arguments)
    {
        for (auto argl : *args->arguments)
        {
            if (argl->tp && argl->tp->type == BasicType::aggregate_)
            {
                auto it = argl->tp->syms->begin();
                SYMBOL* func = *it;
                if (!func->sb->templateLevel && ++it == argl->tp->syms->end())
                {
                    argl->tp = func->tp;
                    argl->exp = MakeExpression(ExpressionNode::pc_, func);
                }
                else if (argl->exp->type == ExpressionNode::callsite_ && argl->exp->v.func->astemplate &&
                         !argl->exp->v.func->ascall)
                {
                    Type* ctype = argl->tp;
                    EXPRESSION* exp = nullptr;
                    auto sp =
                        GetOverloadedFunction(&ctype, &exp, argl->exp->v.func->sp, argl->exp->v.func, nullptr, toErr, false, 0);
                    if (sp)
                    {
                        argl->tp = ctype;
                        argl->exp = exp;
                    }
                }
            }
        }
    }
}
}  // namespace Parser