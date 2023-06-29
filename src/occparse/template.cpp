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
 * pu
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include "compiler.h"
#include "assert.h"
#include <stack>
#include "PreProcessor.h"
#include <malloc.h>
#include "ccerr.h"
#include "initbackend.h"
#include "declare.h"
#include "declcpp.h"
#include "stmt.h"
#include "expr.h"
#include "lambda.h"
#include "occparse.h"
#include "help.h"
#include "cpplookup.h"
#include "mangle.h"
#include "lex.h"
#include "constopt.h"
#include "memory.h"
#include "init.h"
#include "rtti.h"
#include "declcons.h"
#include "exprcpp.h"
#include "inline.h"
#include "beinterf.h"
#include "types.h"
#include "template.h"
#include "libcxx.h"
#include "constexpr.h"
#include "symtab.h"
#include "ListFactory.h"
namespace Parser
{

int dontRegisterTemplate;
int instantiatingTemplate;
int inTemplateBody;
int templateNestingCount = 0;
int templateHeaderCount;
int inTemplateSpecialization = 0;
int inDeduceArgs;
bool parsingSpecializationDeclaration;
bool inTemplateType;
int noTypeNameError;
int inTemplateHeader;
SYMBOL* instantiatingMemberFuncClass;
int instantiatingFunction;
int instantiatingClass;
int parsingDefaultTemplateArgs;
int count1;
int inTemplateArgs;

static int templateNameTag;
static std::unordered_map<SYMBOL*, std::unordered_map<std::string, SYMBOL*>> classTemplateMap;
std::unordered_map<std::string, SYMBOL*, StringHash> classTemplateMap2;

struct templateListData* currents;

static LEXLIST* TemplateArg(LEXLIST* lex, SYMBOL* funcsp, TEMPLATEPARAMPAIR& arg, std::list<TEMPLATEPARAMPAIR>** lst);
std::list<TEMPLATEPARAMPAIR>* copyParams(std::list<TEMPLATEPARAMPAIR>* t, bool alsoSpecializations);
static bool valFromDefault(std::list<TEMPLATEPARAMPAIR>* params, bool usesParams, INITLIST** args);
std::list<TEMPLATEPARAMPAIR>* ResolveTemplateSelectors(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, bool byVal);
std::list<TEMPLATEPARAMPAIR>* ResolveDeclType(SYMBOL* sp, TEMPLATEPARAMPAIR* tpx, bool returnNull = false);
static std::list<TEMPLATEPARAMPAIR>* GetTypeAliasArgs(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                                           std::list<TEMPLATEPARAMPAIR>* origUsing);
static void TransferClassTemplates(std::list<TEMPLATEPARAMPAIR>* dflt, std::list<TEMPLATEPARAMPAIR>* val, std::list<TEMPLATEPARAMPAIR>* params);

void templateInit(void)
{
    inTemplateBody = false;
    inTemplateHeader = false;
    templateNestingCount = 0;
    templateHeaderCount = 0;
    instantiatingTemplate = 0;
    instantiatingClass = 0;
    currents = nullptr;
    inTemplateArgs = 0;
    inTemplateType = false;
    dontRegisterTemplate = 0;
    inTemplateSpecialization = 0;
    instantiatingMemberFuncClass = nullptr;
    parsingSpecializationDeclaration = false;
    instantiatingFunction = 0;
    parsingDefaultTemplateArgs = 0;
    inDeduceArgs = 0;
    classTemplateMap.clear();
    classTemplateMap2.clear();
    templateNameTag = 1;
}
EXPRESSION* GetSymRef(EXPRESSION* n)
{
    EXPRESSION* rv = nullptr;
    std::stack<EXPRESSION*> st;
    st.push(n);
    while (!st.empty())
    {
        EXPRESSION* exp = st.top();
        st.pop();
        switch (exp->type)
        {
            case en_labcon:
            case en_global:
            case en_auto:
            case en_absolute:
            case en_pc:
            case en_threadlocal:
                return exp;
            default:
                if (!isintconst(exp) && !isfloatconst(exp))
                {
                    if (exp->right)
                    {
                        st.push(exp->right);
                    }
                    if (exp->left)
                    {
                        st.push(exp->left);
                    }
                }
                break;
        }
    }
    return rv;
}
bool equalTemplateIntNode(EXPRESSION* exp1, EXPRESSION* exp2)
{
    if (exp1->type == en_templateparam)
        exp1 = exp1->v.sp->tp->templateParam->second->byNonType.val;
    if (exp2->type == en_templateparam)
        exp2 = exp2->v.sp->tp->templateParam->second->byNonType.val;
    if (exp1 && exp2)
    {
        if (equalnode(exp1, exp2))
            return true;
        if (isintconst(exp1) && isintconst(exp2) && exp1->v.i == exp2->v.i)
            return true;
    }
    if (!exp1 && !exp2)
        return true;
    return false;
}
bool templatecompareexpressions(EXPRESSION* exp1, EXPRESSION* exp2)
{
    if (isintconst(exp1) && isintconst(exp2))
        return exp1->v.i == exp2->v.i;
    if (exp1->type != exp2->type)
        return false;
    switch (exp1->type)
    {
        case en_global:
        case en_auto:
        case en_labcon:
        case en_absolute:
        case en_pc:
        case en_const:
        case en_threadlocal:
            return comparetypes(exp1->v.sp->tp, exp2->v.sp->tp, true) || sameTemplate(exp1->v.sp->tp, exp1->v.sp->tp);
        case en_func: {
            TYPE* tp1 = basetype(exp1->v.sp->tp);
            TYPE* tp2 = basetype(exp2->v.sp->tp);
            if (isfunction(tp1) || isfunction(tp2))
            {
                tp1 = tp1->btp;
                tp2 = tp2->btp;
            }
            else if (tp1->type == bt_aggregate || tp2->type == bt_aggregate)
            {
                return true;
            }
            else if (tp1->type != tp2->type)
            {
                return false;
            }
            if ((basetype(tp1)->type == bt_templateparam && tp2->type == bt_int) ||
                (basetype(tp2)->type == bt_templateparam && tp1->type == bt_int))  // undefined
                return true;
            return comparetypes(tp1, tp2, false) || sameTemplate(tp1, tp2);
        }
        case en_templateselector:
            return templateselectorcompare(exp1->v.templateSelector, exp2->v.templateSelector);
        default:
            break;
    }
    if (exp1->left && exp2->left)
        if (!templatecompareexpressions(exp1->left, exp2->left))
            return false;
    if (exp1->right && exp2->right)
        if (!templatecompareexpressions(exp1->right, exp2->right))
            return false;
    return true;
}
bool templateselectorcompare(std::vector<TEMPLATESELECTOR>* tsin1, std::vector<TEMPLATESELECTOR>* tsin2)
{
    auto& ts1 = (*tsin1)[1];
    auto& ts2 = (*tsin2)[1];
    if (ts1.isTemplate != ts2.isTemplate || ts1.sp != ts2.sp)
        return false;
    auto tss1 = (*tsin1).begin();
    auto tss2 = (*tsin2).begin();
    ++tss1;
    ++tss1;
    ++tss2;
    ++tss2;
    for ( ; tss1 != (*tsin1).end() && tss2 != (*tsin2).end(); ++tss1, ++tss2)
        if (strcmp(tss1->name, tss2->name))
            return false;
    if (tss1 != (*tsin1).end() || tss2 != (*tsin2).end())
        return false;
    if (ts1.isTemplate)
    {
        if (!exactMatchOnTemplateParams(ts1.templateParams, ts2.templateParams))
            return false;
    }
    return true;
}
bool templatecomparetypes(TYPE* tp1, TYPE* tp2, bool exact, bool sameType)
{
    if (!tp1 || !tp2)
        return false;
    if (basetype(tp1)->type == bt_templateselector && basetype(tp2)->type == bt_templateselector)
    {
        auto left = basetype(tp1)->sp->sb->templateSelector;
        auto right = basetype(tp2)->sp->sb->templateSelector;
        if ((*left)[1].isDeclType ^ (*right)[1].isDeclType)
            return false;
        auto tss1 = (*left).begin();
        auto tss2 = (*right).begin();
        ++tss1;
        ++tss1;
        ++tss2;
        ++tss2;
        for (; tss1 != (*left).end() && tss2 != (*right).end(); ++tss1, ++tss2)
            if (strcmp(tss1->name, tss2->name))
                return false;
        return tss1 == (*left).end() && tss2 == (*right).end();
    }
    else
    {
        if (basetype(tp1)->type == bt_templateselector || basetype(tp2)->type == bt_templateselector)
            return true;
    }
    if (sameType && (isref(tp1) != isref(tp2) || (isref(tp1) && basetype(tp1)->type != basetype(tp2)->type)))
        return false;
    if (!comparetypes(tp1, tp2, exact) && (!sameType || !sameTemplate(tp1, tp2)))
        return false;
    if (isint(tp1) && basetype(tp1)->btp && basetype(tp1)->btp->type == bt_enum)
        tp1 = basetype(tp1)->btp;
    if (isint(tp2) && basetype(tp2)->btp && basetype(tp2)->btp->type == bt_enum)
        tp2 = basetype(tp2)->btp;
    if (basetype(tp1)->type != basetype(tp2)->type)
        if (isref(tp1) || !isref(tp2))
            return false;
    if (basetype(tp1)->type == bt_enum)
    {
        if (basetype(tp1)->sp != basetype(tp2)->sp)
            return false;
    }
    return true;
}
void TemplateGetDeferred(SYMBOL* sym)
{
    if (currents)
    {
        if (currents->bodyHead)
        {
            sym->sb->deferredCompile = currents->bodyHead;
            for (auto v = currents->bodyHead; v; v = v->next)
                v->data->registered = false;
        }
    }
}
TEMPLATEPARAMPAIR* TemplateLookupSpecializationParam(const char* name)
{
    if (parsingSpecializationDeclaration && currents->args)
        for (auto&& search : *currents->args)
            if (search.first && !strcmp(search.first->name, name))
                return &search;
    return nullptr;
}
std::list<TEMPLATEPARAMPAIR>* TemplateGetParams(SYMBOL* sym)
{
    std::list<TEMPLATEPARAMPAIR>* params = nullptr;
    if (currents)
    {
        int n = -1;
        params = (std::list<TEMPLATEPARAMPAIR>*)(*currents->plast);
        while (sym)
        {
            if (sym->sb->templateLevel && !sym->sb->instantiated)
                n++;
            sym = sym->sb->parentClass;
        }
        if (n > 0 && params)
        {
            while (n-- && params->front().second->bySpecialization.next)
            {
                params = params->front().second->bySpecialization.next;
            }
        }
    }
    if (!params)
    {
        params = templateParamPairListFactory.CreateList();
        params->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
    }
    return params;
}
void TemplateRegisterDeferred(LEXLIST* lex)
{
    if (lex && templateNestingCount && !dontRegisterTemplate)
    {
        if (!lex->data->registered)
        {
            LEXLIST* cur = globalAllocate<LEXLIST>();
            if (lex->data->type == l_id)
                lex->data->value.s.a = litlate(lex->data->value.s.a);
            *cur = *lex;
            cur->next = nullptr;
            if (inTemplateBody)
            {
                if (currents->bodyHead)
                {
                    cur->prev = currents->bodyTail;
                    currents->bodyTail = currents->bodyTail->next = cur;
                }
                else
                {
                    cur->prev = nullptr;
                    currents->bodyHead = currents->bodyTail = cur;
                }
            }
            else
            {
                if (currents->head)
                {
                    cur->prev = currents->tail;
                    currents->tail = currents->tail->next = cur;
                }
                else
                {
                    cur->prev = nullptr;
                    currents->head = currents->tail = cur;
                }
            }
            lex->data->registered = true;
        }
    }
}
bool exactMatchOnTemplateParams(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym)
{
    if (old && sym)
    {
        auto ito = old->begin();
        auto itoe = old->end();
        auto its = sym->begin();
        auto itse = sym->end();
        if (ito->second->type == kw_new)
            ++ito;
        if (its->second->type == kw_new)
            ++its;
        for ( ; ito != itoe && its != itse; ++ito, ++its)
        {
            if (ito->second->type != its->second->type)
                break;
            if (its->second->packed)
            {
                if (ito->second->packed)
                {
                    if (ito->second->byPack.pack)
                        if (!exactMatchOnTemplateParams(ito->second->byPack.pack, its->second->byPack.pack))
                            return false;
                }
                else if (its->second->byPack.pack)
                {
                    for ( ; ito != itoe; ++ ito)
                        if (ito->second->type != its->second->type)
                            return false;
                    return ++its == itse;
                }
                else
                {
                    return ++its == itse;
                }
            }
            else if (ito->second->type == kw_template)
            {
                if (!exactMatchOnTemplateParams(ito->second->byTemplate.args, its->second->byTemplate.args))
                    break;
            }
            else if (ito->second->type == kw_int)
            {
                if (!templatecomparetypes(ito->second->byNonType.tp, its->second->byNonType.tp, true))
                    if (ito->second->byNonType.tp->type != bt_templateparam && its->second->byNonType.tp->type != bt_templateparam)
                        break;
                if (ito->second->byNonType.dflt && its->second->byNonType.dflt &&
                    !templatecompareexpressions(ito->second->byNonType.dflt, its->second->byNonType.dflt))
                    break;
            }
        }
        if (ito != itoe && ito->second->packed)
            ito = itoe;
        return ito == itoe && its == itse;
    }
    return !old && !sym;
}
bool exactMatchOnTemplateArgs(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym)
{
    if (old && sym)
    {
        auto ito = old->begin();
        auto itoe = old->end();
        auto its = sym->begin();
        auto itse = sym->end();
        if (ito != itoe && ito->second->type == kw_new)
            ++ito;
        if (its != itse && its->second->type == kw_new)
            ++its;
        for (; ito != itoe && its != itse; ++ito, ++its)
        {
            if (ito->second->type != its->second->type)
                return false;
            if (ito->second->packed)
            {
                return false;
            }
            switch (ito->second->type)
            {
            case kw_typename:
                if (sameTemplate(ito->second->byClass.dflt, its->second->byClass.dflt))
                {
                    auto to = ito->second->byClass.dflt;
                    auto ts = its->second->byClass.dflt;
                    if (isref(to))
                        to = basetype(to)->btp;
                    if (isref(ts))
                        ts = basetype(ts)->btp;
                    if (!exactMatchOnTemplateArgs(basetype(to)->sp->templateParams,
                        basetype(ts)->sp->templateParams))
                        return false;
                }
                else
                {
                    if (!templatecomparetypes(ito->second->byClass.dflt, its->second->byClass.dflt, true))
                        return false;
                    if (!templatecomparetypes(its->second->byClass.dflt, ito->second->byClass.dflt, true))
                        return false;
                    if (isarray(ito->second->byClass.dflt) != isarray(its->second->byClass.dflt))
                        return false;
                    if (isarray(ito->second->byClass.dflt))
                        if (!!basetype(ito->second->byClass.dflt)->esize != !!basetype(its->second->byClass.dflt)->esize)
                            return false;
                }
                {
                    TYPE* ts = its->second->byClass.dflt;
                    TYPE* to = ito->second->byClass.dflt;
                    if (isref(ts))
                        ts = basetype(ts)->btp;
                    if (isref(to))
                        to = basetype(to)->btp;
                    if (isconst(ts) != isconst(to))
                        return false;
                    if (isvolatile(ts) != isvolatile(to))
                        return false;
                }
                break;
            case kw_template:
                if (ito->second->byTemplate.dflt != its->second->byTemplate.dflt)
                    return false;
                break;
            case kw_int:
                if (!templatecomparetypes(ito->second->byNonType.tp, its->second->byNonType.tp, true))
                    return false;
                if (!!ito->second->byNonType.dflt != !!its->second->byNonType.dflt)
                    return false;
                if (ito->second->byNonType.dflt && its->second->byNonType.dflt &&
                    !templatecompareexpressions(ito->second->byNonType.dflt, its->second->byNonType.dflt))
                    return false;
                break;
            default:
                break;
            }
        }
        return ito == itoe && its == itse;
    }
    return !old && !sym;
}
static std::list<TEMPLATEPARAMPAIR>* mergeTemplateDefaults(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym, bool definition)
{
    std::list<TEMPLATEPARAMPAIR>* rv = sym;
    if (old && sym)
    {
        auto ito = old->begin();
        auto itoe = old->end();
        auto its = sym->begin();
        auto itse = sym->end();
        for (; ito != itoe && its != itse; ++ito, ++its)
        {
            if (!definition && ito->first)
            {
                its->first = ito->first;
                its->first->tp->templateParam = &*its;
            }
            switch (its->second->type)
            {
            case kw_template:
                its->second->byTemplate.args = mergeTemplateDefaults(ito->second->byTemplate.args, its->second->byTemplate.args, definition);
                if (ito->second->byTemplate.txtdflt && its->second->byTemplate.txtdflt)
                {
                    if (!CompareLex(ito->second->byNonType.txtdflt, its->second->byNonType.txtdflt))
                        errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, its->first);
                }
                else if (!its->second->byTemplate.txtdflt)
                {
                    its->second->byTemplate.txtdflt = ito->second->byTemplate.txtdflt;
                    its->second->byTemplate.txtargs = ito->second->byTemplate.txtargs;
                }
                break;
            case kw_typename:
                if (ito->second->byClass.txtdflt && its->second->byClass.txtdflt)
                {
                    if (!CompareLex(ito->second->byNonType.txtdflt, its->second->byNonType.txtdflt))
                        errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, its->first);
                }
                else if (!its->second->byClass.txtdflt)
                {
                    its->second->byClass.txtdflt = ito->second->byClass.txtdflt;
                    its->second->byClass.txtargs = ito->second->byClass.txtargs;
                }
                break;
            case kw_int:
                if (ito->second->byNonType.txtdflt && its->second->byNonType.txtdflt)
                {
                    if (!CompareLex(ito->second->byNonType.txtdflt, its->second->byNonType.txtdflt))
                        errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, its->first);
                }
                else if (!its->second->byNonType.txtdflt)
                {
                    its->second->byNonType.txtdflt = ito->second->byNonType.txtdflt;
                    its->second->byNonType.txttype = ito->second->byNonType.txttype;
                    its->second->byNonType.txtargs = ito->second->byNonType.txtargs;
                }
                break;
            case kw_new:  // specialization
                break;
            default:
                break;
            }
        }
    }
    return rv;
}
static void checkTemplateDefaults(std::list<TEMPLATEPARAMPAIR>* args)
{
    SYMBOL* last = nullptr;
    if (args)
    {
        for (auto&& arg : *args)
        {
            void* txtdflt = nullptr;
            switch (arg.second->type)
            {
            case kw_template:
                checkTemplateDefaults(arg.second->byTemplate.args);
                txtdflt = arg.second->byTemplate.txtdflt;
                break;
            case kw_typename:
                txtdflt = arg.second->byClass.txtdflt;
                break;
            case kw_int:
                txtdflt = arg.second->byNonType.txtdflt;
                break;
            default:
                break;
            }
            if (last && !txtdflt)
            {
                errorsym(ERR_MISSING_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, last);
                break;
            }
            if (txtdflt)
                last = arg.first;
        }
    }
}
bool matchTemplateSpecializationToParams(std::list<TEMPLATEPARAMPAIR>* param, std::list<TEMPLATEPARAMPAIR>* special, SYMBOL* sp)
{
    if (param && special)
    {
        auto itp = param->begin();
        auto itpe = param->end();
        auto its = special->begin();
        auto itse = special->end();
        if (itp->second->type == kw_new)
            ++itp;
        for ( ; itp != itpe && !itp->second->packed && its != itse; ++itp, ++its)
        {
            if (itp->second->type != its->second->type)
            {
                if (itp->second->type != kw_typename || its->second->type != kw_template)
                    errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
            }
            else if (itp->second->type == kw_template)
            {
                if (!exactMatchOnTemplateParams(itp->second->byTemplate.args, its->second->byTemplate.dflt->templateParams))
                    errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
            }
            else if (itp->second->type == kw_int)
            {
                if (itp->second->byNonType.tp->type != bt_templateparam &&
                    !comparetypes(itp->second->byNonType.tp, its->second->byNonType.tp, false) &&
                    (!ispointer(itp->second->byNonType.tp) || !isconstzero(itp->second->byNonType.tp, its->second->byNonType.dflt)))
                    errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
            }
        }
        if (itp != itpe)
        {
            if (!itp->second->packed)
            {
                errorsym(ERR_TOO_FEW_ARGS_PASSED_TO_TEMPLATE, sp);
            }
            else
            {
                param = nullptr;
                special = nullptr;
            }
        }
        else if (its != itse)
        {
            if (its->second->packed)
                special = nullptr;
            else
                errorsym(ERR_TOO_MANY_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        return itp == itpe && its == itse;
    }
    return !param && !special;
}
static void checkMultipleArgs(std::list<TEMPLATEPARAMPAIR>* sym)
{
    if (sym)
    {
        for (auto it = sym->begin(); it != sym->end(); ++it)
        {
            if (it->first)
            {
                auto it1 = it;
                for (++it1; it1 != sym->end(); ++it1)
                {
                    if (it1->first && !strcmp(it->first->name, it1->first->name))
                    {
                        currentErrorLine = 0;
                        errorsym(ERR_DUPLICATE_IDENTIFIER, it->first);
                    }
                }
            }
            if (it->second->type == kw_template)
            {
                checkMultipleArgs(it->second->byTemplate.args);
            }
        }
    }
}
std::list<TEMPLATEPARAMPAIR>* TemplateMatching(LEXLIST* lex, std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym, SYMBOL* sp, bool definition)
{
    (void)lex;
    std::list<TEMPLATEPARAMPAIR>* rv = nullptr;
    currents->sp = sp;
    if (old)
    {
        if (sym->front().second->bySpecialization.types)
        {
            auto ito = old->begin();
            auto itoe = old->end();
            std::list<TEMPLATEPARAMPAIR>* transfer;
            matchTemplateSpecializationToParams(old, sym->front().second->bySpecialization.types, sp);
            rv = sym;
            transfer = sym->front().second->bySpecialization.types;
            auto ittransfer = sym->front().second->bySpecialization.types->begin();
            auto itetransfer = sym->front().second->bySpecialization.types->end();
            for (++ito; ito != itoe  && ittransfer != itetransfer && !ito->second->packed; ++ito, ++ ittransfer)
            {
                if (ittransfer->second->type != kw_typename || basetype(ittransfer->second->byClass.dflt)->type != bt_templateselector)
                {
                    ittransfer->second->byClass.txtdflt = ito->second->byClass.txtdflt;
                    ittransfer->second->byClass.txtargs = ito->second->byClass.txtargs;
                    if (ittransfer->second->type == kw_int)
                        ittransfer->second->byNonType.txttype = ito->second->byNonType.txttype;
                }
            }
        }
        else if (sym->size() > 1 )
        {
            if (!exactMatchOnTemplateParams(old, sym))
            {
                error(ERR_TEMPLATE_DEFINITION_MISMATCH);
            }
            else
            {
                rv = mergeTemplateDefaults(old, sym, definition);
                checkTemplateDefaults(rv);
            }
        }
        else
        {
            rv = sym;
        }
    }
    else
    {
        rv = sym;
        checkTemplateDefaults(sym);
    }
    checkMultipleArgs(sym);
    return rv;
}
bool typeHasTemplateArg(TYPE* t);
static bool structHasTemplateArg(std::list<TEMPLATEPARAMPAIR>* tplx)
{
    if (tplx)
    {
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tps;
        auto itl = tplx->begin();
        auto itle = tplx->end();
        for (; itl != itle;)
        {
            if (itl->second->type == kw_typename)
            {
                if (itl->second->packed)
                {
                    if (itl->second->byPack.pack)
                    {
                        tps.push(itl);
                        tps.push(itle);
                        itl = itl->second->byPack.pack->begin();
                        itle = itl->second->byPack.pack->end();
                        continue;
                    }
                }
                else
                {
                    if (typeHasTemplateArg(itl->second->byClass.dflt))
                        return true;
                }
            }
            else if (itl->second->type == kw_template)
            {
                if (structHasTemplateArg(itl->second->byTemplate.args))
                    return true;
            }
            ++itl;
            if (itl == itle && tps.size())
            {
                itle = tps.top();
                tps.pop();
                itl = tps.top();
                tps.pop();
                ++itl;
            }
        }
    }
    return false;
}
bool typeHasTemplateArg(TYPE* t)
{
    if (t)
    {
        while (ispointer(t) || isref(t))
            t = t->btp;
        if (isfunction(t))
        {
            t = basetype(t);
            if (typeHasTemplateArg(t->btp))
                return true;
            for (auto sym : *t->syms)
            {
                if (typeHasTemplateArg(sym->tp))
                    return true;
            }
        }
        else if (basetype(t)->type == bt_templateparam)
            return true;
        else if (isstructured(t))
        {
            std::list<TEMPLATEPARAMPAIR>* tpx = basetype(t)->sp->templateParams;
            if (structHasTemplateArg(tpx))
                return true;
        }
    }
    return false;
}
void TemplateValidateSpecialization(std::list<TEMPLATEPARAMPAIR>* arg)
{
    if (arg && arg->front().second->bySpecialization.types)
    {
        bool found = false;
        for (auto&& t : *arg->front().second->bySpecialization.types)
        {
            if (t.second->type == kw_typename && typeHasTemplateArg((TYPE*)t.second->byClass.dflt))
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            error(ERR_PARTIAL_SPECIALIZATION_MISSING_TEMPLATE_PARAMETERS);
        }
    }
}
static void GetPackedTypes(TEMPLATEPARAMPAIR** packs, int* count, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (args)
    {
        for (auto&& arg : *args)
        {
            if (arg.second->type == kw_typename)
            {
                if (arg.second->packed)
                {
                    packs[(*count)++] = &arg;
                }
            }
            else if (arg.second->type == kw_delete)
            {
                GetPackedTypes(packs, count, arg.second->byDeferred.args);
            }
        }
    }
}
static std::list<TEMPLATEPARAMPAIR>** expandArgs(std::list<TEMPLATEPARAMPAIR>** lst, LEXLIST* start, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* select, bool packable)
{
    int beginning = 0;
    if (*lst)
        beginning = (*lst)->size();
    // this is going to presume that the expression involved
    // is not too long to be cached by the LEXLIST mechanism.
    int oldPack = packIndex;
    int count = 0;
    TEMPLATEPARAMPAIR* arg[500];
    if (!packable)
    {
        if (!*lst)
            *lst = templateParamPairListFactory.CreateList();
        if (select->front().second->packed && packIndex >= 0)
        {
            std::list<TEMPLATEPARAMPAIR>* templateParam = select->front().second->byPack.pack;
            int i;
            auto it = select->front().second->byPack.pack->begin();
            auto ite = select->front().second->byPack.pack->end();
            for (i = 0; i < packIndex && it != ite; i++, ++it);
            if (it != ite)
            {
                (*lst)->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
                *(*lst)->back().second = *it->second;
                (*lst)->back().second->ellipsis = false;
                (*lst)->back().second->byClass.dflt = (*lst)->back().second->byClass.val;
                return lst;
            }
        }
        if (select->front().second->ellipsis)
        {
            (*lst)->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
            *(*lst)->back().second = *select->front().second;
            (*lst)->back().second->ellipsis = false;
        }
        else
        {
            (*lst)->push_back(TEMPLATEPARAMPAIR(nullptr, select->front().second));
        }
        (*lst)->back().first = select->front().first;
        return lst;
    }
    std::list<TEMPLATEPARAMPAIR> temp = { {select->front().first, select->front().second } };
    GetPackedTypes(arg, &count, &temp);
    expandingParams++;
    if (count)
    {
        int i;
        int n = CountPacks(arg[0]->second->byPack.pack);  // undefined in local context
        for (i = 1; i < count; i++)
        {
            if (CountPacks(arg[i]->second->byPack.pack) != n)
            {
                error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                break;
            }
        }
        for (i = 0; i < n; i++)
        {
            LEXLIST* lex = SetAlternateLex(start);
            TYPE* tp;
            packIndex = i;
            lex = get_type_id(lex, &tp, funcsp, sc_parameter, false, true, false);
            SetAlternateLex(nullptr);
            if (tp)
            {
                if (!*lst)
                    *lst = templateParamPairListFactory.CreateList();
                (*lst)->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
                (*lst)->back().second->type = kw_typename;
                (*lst)->back().second->byClass.dflt = tp;
            }
        }
    }
    else if (select)
    {
        if (!*lst)
            *lst = templateParamPairListFactory.CreateList();
        (*lst)->push_back(select->front());
    }
    expandingParams--;
    packIndex = oldPack;
    // make it packed again...   we aren't flattening at this point.
    if (select->front().second->packed)
    {
        TEMPLATEPARAMPAIR next;
        next.second = Allocate<TEMPLATEPARAM>();
        *next.second = *select->front().second;
        next.first = select->front().first;
        if (*lst && beginning != (*lst)->size())
        {
            next.second->byPack.pack = templateParamPairListFactory.CreateList();
            auto itbeginning = (*lst)->begin();
            int n = (*lst)->size() - beginning;
            while (beginning--)
                ++itbeginning;
            (*next.second->byPack.pack) = std::move(std::list<TEMPLATEPARAMPAIR>(itbeginning, (*lst)->end()));
            while (n--)
                (*lst)->pop_back();
        }
        next.second->resolved = true;
        if (!*lst)
            (*lst) = templateParamPairListFactory.CreateList();
        (*lst)->push_back(next);
    }
    return lst;
}
void UnrollTemplatePacks(std::list<TEMPLATEPARAMPAIR>* tplx)
{ 
    if (tplx)
    {
        for (auto&& tpx : *tplx)
        {
            if (tpx.second->type == kw_typename)
            {
                auto tpl2 = tpx;
                if (tpx.second->packed)
                {
                    if (tpx.second->byPack.pack && tpx.second->byPack.pack->size() == 1 && tpx.second->byPack.pack->front().second->packed)
                        tpx.second->byPack.pack = tpx.second->byPack.pack->front().second->byPack.pack;
                }
                else if (tpx.second->byClass.dflt)
                {
                    auto quals = tpx.second->byClass.dflt;
                    auto end = quals;
                    while (end->btp)
                        end = end->btp;
                    if (end->type == bt_templateparam)
                    {
                        auto ths = end->templateParam;
                        if (ths->second->packed)
                        {
                            auto tpl2 = ths->second->byPack.pack;
                            if (tpl2)
                            {
                                tpx.second = ths->second;
                                if (quals != end)
                                {
                                    for (auto&& lst :*tpl2)
                                    {
                                        TYPE* hold = CopyType(quals, true, [&lst](TYPE*& old, TYPE*& newx) {
                                            if (newx->type == bt_templateparam)
                                                newx = lst.second->byClass.val ? lst.second->byClass.val : lst.second->byClass.dflt;
                                            });
                                        UpdateRootTypes(hold);
                                        CollapseReferences(hold);
                                        lst.second->byClass.dflt = hold;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
static std::list<TEMPLATEPARAMPAIR>* nextExpand(std::list<TEMPLATEPARAMPAIR>* inx, int n)
{
    bool packed = false;
    if (inx)
    {
        for (auto&& t : *inx)
        {
            if (t.second->packed)
            {
                packed = true;
                break;
            }
        }
    }
    if (!packed)
    {
        if (!n)
            return inx;
        return nullptr;
    }
    bool found = false;
    if (inx)
    {
        std::list<TEMPLATEPARAMPAIR>* out = templateParamPairListFactory.CreateList();
        for (auto&& in : *inx)
        {
            out->push_back(TEMPLATEPARAMPAIR{ in.first, in.second });
            if (in.second->packed && in.second->byPack.pack)
            {
                out->back().second = Allocate<TEMPLATEPARAM>();
                out->back().second->type = in.second->type;
                out->back().second->packed = true;
                auto it = in.second->byPack.pack->begin();
                auto ite = in.second->byPack.pack->end();
                for (; n && it != ite; --n, ++it);
                if (it != ite)
                {
                    found = true;
                    out->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                    out->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{ nullptr, it->second });
                }
            }
        }
        return found ? out : nullptr;
    }
    return nullptr;
}
std::list<TEMPLATEPARAMPAIR>** expandTemplateSelector(std::list<TEMPLATEPARAMPAIR>** lst, std::list<TEMPLATEPARAMPAIR>* orig, TYPE* tp)
{
    if (tp->sp && tp->sp->sb->templateSelector)
    {
        TEMPLATEPARAMPAIR* cptr = nullptr;

        for (auto&& s : structSyms)
        {
            if (s.tmpl)
            {
                SYMBOL* sym = templatesearch((*tp->sp->sb->templateSelector)[1].sp->name, s.tmpl);
                if (sym)
                {
                    cptr = sym->tp->templateParam;
                    if (cptr)
                        break;
                }
            }
        }
        // i'm not sure why this first clause is in here.   I couldn't find anything we compile that uses it.
        // i'm adding necessary things in the if's else.
        if (cptr && cptr->second->packed && cptr->second->byPack.pack)
        {
            auto xlst = cptr->second->byPack.pack;
            if (!*lst)
                *lst = templateParamPairListFactory.CreateList();
            (*lst)->push_back(TEMPLATEPARAMPAIR(orig->front().first, Allocate<TEMPLATEPARAM>()));
            (*lst)->back().second->type = kw_typename;
            (*lst)->back().second->packed = true;
            std::list<TEMPLATEPARAMPAIR> *last = nullptr;
            for (auto&& clst : *xlst)
            {
                auto sel = (*tp->sp->sb->templateSelector).begin();
                auto sele = (*tp->sp->sb->templateSelector).end();
                TYPE* base = clst.second->byClass.val, *base1 = base;
                SYMBOL* s = nullptr;
                for ( ; sel != sele; ++ sel)
                {
                    base = base1;
                    s = search(base->syms, sel->name);
                    auto it1 = sel;
                    ++it1;
                    // it was written this way but could be optimized
                    if (!s || (s && it1 != sele && !isstructured(s->tp)))
                        break;
                    base = basetype(s->tp);
                }
                if (sel == sele && s)
                {
                    if (!last)
                        last = templateParamPairListFactory.CreateList();
                    last->push_back(TEMPLATEPARAMPAIR(nullptr, Allocate<TEMPLATEPARAM>()));
                    if (s->sb->storage_class == sc_constant || s->sb->storage_class == sc_constexpr ||
                        s->sb->storage_class == sc_enumconstant)
                    {
                        (*lst)->back().second->type = last->back().second->type = kw_int;
                        last->back().second->byNonType.dflt = s->sb->init->front()->exp;
                        last->back().second->byNonType.tp = s->tp;
                    }
                    else
                    {
                        (*lst)->back().second->type = last->back().second->type = kw_typename;
                        last->back().second->byClass.dflt = s->tp;
                    }
                }
            }
            (*lst)->back().second->byPack.pack = last;
        }
        else
        {
            if (templateNestingCount && (!inTemplateBody || !instantiatingTemplate))
            {
                if (!*lst)
                    *lst = templateParamPairListFactory.CreateList();
                (*lst)->push_back(TEMPLATEPARAMPAIR(orig->front().first, Allocate<TEMPLATEPARAM>()));
                (*lst)->back().second->type = kw_typename;
                (*lst)->back().second->byClass.dflt = tp;
            }
            else if (!templateNestingCount && (*tp->sp->sb->templateSelector)[1].isTemplate)
            {
                if (!*lst)
                    *lst = templateParamPairListFactory.CreateList();
                (*lst)->push_back(TEMPLATEPARAMPAIR(orig->front().first, Allocate<TEMPLATEPARAM>()));
                (*lst)->back().second->type = kw_typename;
                (*lst)->back().second->packed = true;
                std::list<TEMPLATEPARAMPAIR>* last = nullptr;
                for (int i = 0; i < INT_MAX; i++)
                {
                    auto clst = nextExpand((*tp->sp->sb->templateSelector)[1].templateParams, i);
                    if (!clst)
                    {
                        break;
                    }
                    SYMBOL* sp = GetClassTemplate((*tp->sp->sb->templateSelector)[1].sp, clst, false);
                    if (sp)
                    {
                        sp = TemplateClassInstantiate(sp, clst, false, sc_global);
                    }
                    if (sp)
                    {
                        auto sel = (*tp->sp->sb->templateSelector).begin();
                        auto sele = (*tp->sp->sb->templateSelector).end();
                        TYPE* base = sp->tp, *base1 = base;
                        SYMBOL* s = nullptr;
                        for (++sel, ++sel; sel != sele; ++sel)
                        {
                            base = base1;
                            STRUCTSYM ss;
                            ss.str = basetype(base)->sp;
                            addStructureDeclaration(&ss);
                            s = classsearch(sel->name, false, false);
                            dropStructureDeclaration();
                            auto it1 = sel;
                            ++it1;
                            // could be optimized
                            if (!s || (s && it1 != sele && !isstructured(s->tp)))
                                break;
                            base1 = basetype(s->tp);
                        }
                        if (sel == sele && s)
                        {
                            if (!last)
                                last = templateParamPairListFactory.CreateList();
                            last->push_back(TEMPLATEPARAMPAIR(nullptr, Allocate<TEMPLATEPARAM>()));
                            if (s->sb->storage_class == sc_constant || s->sb->storage_class == sc_constexpr ||
                                s->sb->storage_class == sc_enumconstant)
                            {
                                (*lst)->back().second->type = last->back().second->type = kw_int;
                                last->back().second->byNonType.dflt = s->sb->init->front()->exp;
                                last->back().second->byNonType.tp = s->tp;
                            }
                            else
                            {
                                (*lst)->back().second->type = last->back().second->type = kw_typename;
                                last->back().second->byClass.dflt = s->tp;
                            }
                        }
                    }
                }
                (*lst)->back().second->byPack.pack = last;
            }
        }
    }
    return lst;
}

bool constructedInt(LEXLIST* lex, SYMBOL* funcsp)
{
    // depends on this starting a type
    bool rv = false;
    TYPE* tp;
    LEXLIST* placeholder = lex;
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    bool defd = false;
    bool notype = false;
    bool cont = false;
    tp = nullptr;

    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
    if (lex->data->type == l_id || MATCHKW(lex, classsel))
    {
        SYMBOL *sp, *strSym = nullptr;
        LEXLIST* placeholder = lex;
        bool dest = false;
        nestedSearch(lex, &sp, &strSym, nullptr, &dest, nullptr, false, sc_global, false, false);
        if (Optimizer::cparams.prm_cplusplus)
            prevsym(placeholder);
        if (sp && sp->sb && sp->sb->storage_class == sc_typedef)
            cont = true;
    }
    else
    {
        if (KWTYPE(lex, TT_BASETYPE))
            cont = true;
    }
    if (cont)
    {
        lex = getBasicType(lex, funcsp, &tp, nullptr, false, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3,
                           ac_public, &notype, &defd, nullptr, nullptr, false, false, false, false, false);
        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
        if (isint(tp))
        {
            if (MATCHKW(lex, openpa))
            {
                rv = true;
            }
        }
    }
    lex = prevsym(placeholder);
    return rv;
}
LEXLIST* GetTemplateArguments(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* templ, std::list<TEMPLATEPARAMPAIR>** lst)
{
    std::list<TEMPLATEPARAMPAIR>** start = lst;
    int oldnoTn = noTypeNameError;
    noTypeNameError = 0;
    bool first = true;
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    std::list<TEMPLATEPARAMPAIR>::iterator itorig, iteorig = itorig;
    if (templ)
    {
        auto templ1 = templ;
        if (templ1->sb && templ1->sb->maintemplate)
            templ1 = templ1->sb->maintemplate;
        if (templ->tp && templ1->tp->type == bt_aggregate)
            templ1 = templ1->tp->syms->front();
        if (templ1->templateParams)
        {
            if (templ1->templateParams->front().second->bySpecialization.types)
            {
                itorig = templ1->templateParams->front().second->bySpecialization.types->begin();
                iteorig = templ1->templateParams->front().second->bySpecialization.types->end();
            }
            else
            {
                itorig = templ1->templateParams->begin();
                iteorig = templ1->templateParams->end();
                ++itorig;
            }
        }
    }
    // entered with lex set to the opening <
    inTemplateArgs++;
    lex = getsym();
    if (!MATCHKW(lex, rightshift) && !MATCHKW(lex, gt))
    {
        do
        {
            tp = nullptr;
            if (MATCHKW(lex, kw_typename) || (((itorig != iteorig && itorig->second->type != kw_int) ||
                                               (itorig == iteorig && startOfType(lex, nullptr, true) && !constructedInt(lex, funcsp))) &&
                                              !MATCHKW(lex, kw_sizeof)))
            {
                LEXLIST* start = lex;
                noTypeNameError++;
                int old = noNeedToSpecialize;
                noNeedToSpecialize = itorig != iteorig && itorig->second->type == kw_template;
                lex = get_type_id(lex, &tp, funcsp, sc_parameter, false, true, false);
                noNeedToSpecialize = old;
                noTypeNameError--;
                if (!tp)
                    tp = &stdint;
                if (structLevel && isstructured(tp))
                {
                    if ((!basetype(tp)->sp->sb->templateLevel || basetype(tp)->sp->sb->instantiating) &&
                        basetype(tp)->sp->sb->declaring)
                    {
                        basetype(tp)->sp->sb->declaringRecursive = true;
                    }
                }
                if (!templateNestingCount && tp->type == bt_any)
                {
                    error(ERR_UNKNOWN_TYPE_TEMPLATE_ARG);
                }
                else if (tp && !templateNestingCount)
                {
                    tp = PerformDeferredInitialization(tp, nullptr);
                }
                if (MATCHKW(lex, begin))  // initializer list?
                {
                    if (templateNestingCount)
                    {
                        exp = exprNode(en_construct, nullptr, nullptr);
                        exp->v.construct.tp = tp;
                        lex = getDeferredData(lex, &exp->v.construct.deferred, true);
                    }
                    else
                    {
                        lex = expression_func_type_cast(lex, funcsp, &tp, &exp, _F_NOEVAL);
                    }
                    goto initlistjoin;
                    // makes it an expression
                }
                if (MATCHKW(lex, ellipse))
                {
                    lex = getsym();
                    if (tp)
                    {
                        TYPE* tp1 = tp;
                        while (isref(tp))
                            tp = basetype(tp)->btp;
                        if (templateNestingCount && tp->type == bt_templateparam)
                        {
                            if (!*lst)
                                *lst = templateParamPairListFactory.CreateList();
                            (*lst)->push_back(TEMPLATEPARAMPAIR{tp->templateParam->first, tp->templateParam->second});
                            if (isref(tp1))
                            {
                                if (basetype(tp1)->type == bt_lref)
                                {
                                    (*lst)->back().second->lref = true;
                                    (*lst)->back().second->rref = false;
                                }
                                else
                                {
                                    (*lst)->back().second->rref = true;
                                    (*lst)->back().second->lref = false;
                                }
                            }
                            if (inTemplateSpecialization && !tp->templateParam->second->packed)
                                error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                        }
                        else if (tp->type == bt_templateparam)
                        {
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back(*tp->templateParam);
                            lst = expandArgs(lst, start, funcsp, &a, true);
                        }
                        else if (tp->type == bt_templateselector)
                        {
                            tp->sp->sb->postExpansion = true;
                            std::list<TEMPLATEPARAMPAIR> a(itorig, iteorig);
                            lst = expandTemplateSelector(lst, &a, tp1);
                        }
                        else if (itorig != iteorig && itorig->second->type == kw_typename && itorig->second->packed && isstructured(tp))
                        {
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            a.back().second->type = kw_typename;
                            a.back().second->byClass.dflt = tp1;
                            lst = expandArgs(lst, start, funcsp, &a, true);
                        }
                        else
                        {
                            if (!*lst)
                                *lst = templateParamPairListFactory.CreateList();
                            (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            if (itorig != iteorig && itorig->second->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                            {
                                (*lst)->back().second->type = kw_template;
                                (*lst)->back().second->byTemplate.dflt = basetype(tp)->sp;
                            }
                            else
                            {
                                (*lst)->back().second->type = kw_typename;
                                (*lst)->back().second->byClass.dflt = tp1;
                            }
                        }
                    }
                    (*lst)->back().second->ellipsis = true;
                    if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                    {
                        for (auto&& tpx : *(*lst)->back().second->byPack.pack)
                            tpx.second->ellipsis = true;
                    }
                }
                else if (tp && tp->type == bt_templateparam)
                {
                    if (inTemplateArgs > 1 && tp->templateParam->second->packed)
                    {
                        // unpacked pack gets treated as a single template param
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(TEMPLATEPARAMPAIR{tp->templateParam->first, Allocate<TEMPLATEPARAM>()});
                        *(*lst)->back().second = *tp->templateParam->second;
                        (*lst)->back().second->ellipsis = false;
                        (*lst)->back().second->usedAsUnpacked = true;
                    }
                    else if (inTemplateSpecialization)
                    {
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(*tp->templateParam);
                        if (tp->templateParam->second->packed)
                        {
                            if (tp->templateParam->second->byPack.pack)
                                for (auto&& find : *tp->templateParam->second->byPack.pack)
                                    if (!find.second->byClass.dflt)
                                        find.second->byClass.dflt = tp;
                        }
                        else
                        {
                            if (!tp->templateParam->second->byClass.dflt)
                            {
                                if (tp->templateParam->second->type == kw_template)
                                    tp->templateParam->second->byTemplate.dflt = basetype(tp)->sp;
                                else
                                    tp->templateParam->second->byClass.dflt = tp;
                            }
                        }
                    }
                    else
                    {
                        std::list<TEMPLATEPARAMPAIR> a;
                        a.push_back(*tp->templateParam);
                        lst = expandArgs(lst, start, funcsp, &a, false);
                    }
                }
                else if (itorig != iteorig && itorig->second->packed)
                {
                    if (first)
                    {
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                        if (itorig != iteorig && itorig->second->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                        {
                            (*lst)->back().second->type = kw_template;
                            (*lst)->back().second->packed = true;
                        }
                        else
                        {
                            (*lst)->back().second->type = kw_typename;
                            (*lst)->back().second->packed = true;
                        }
                        if (itorig != iteorig)
                            (*lst)->back().first = itorig->first;
                        first = false;
                    }
                    if (!(*lst)->back().second->byPack.pack)
                        (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                    auto last = (*lst)->back().second->byPack.pack;
                    last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                    if (itorig != iteorig && itorig->second->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                    {
                        last->back().second->type = kw_template;
                        last->back().second->byTemplate.dflt = basetype(tp)->sp;
                    }
                    else
                    {
                        last->back().second->type = kw_typename;
                        last->back().second->byClass.dflt = tp;
                    }
                    if (last->back().second->type != (*lst)->back().second->type)
                    {
                        (*lst)->back().second->byPack.pack = nullptr;
                        error(ERR_PACKED_TEMPLATE_TYPE_MISMATCH);
                    }
                }
                else
                {
                    if (!*lst)
                        *lst = templateParamPairListFactory.CreateList();
                    (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});

                    if (itorig != iteorig)
                        (*lst)->back().first = itorig->first;
                    if (itorig != iteorig && itorig->second->type == kw_template && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                    {
                        (*lst)->back().second->type = kw_template;
                        (*lst)->back().second->byTemplate.dflt = basetype(tp)->sp;
                    }
                    else
                    {
                        (*lst)->back().second->type = kw_typename;
                        (*lst)->back().second->byClass.dflt = tp;
                    }
                }
            }
            else
            {
                exp = nullptr;
                tp = nullptr;
                if (inTemplateSpecialization)
                {
                    if (lex->data->type == l_id)
                    {
                        SYMBOL* sp;
                        LEXLIST* last = lex;
                        lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, sc_global, false, false);
                        if (sp && sp->tp->templateParam)
                        {
                            lex = getsym();
                            if (!MATCHKW(lex, rightshift) && !MATCHKW(lex, gt) && !MATCHKW(lex, comma))
                            {
                                lex = prevsym(last);
                                goto join;
                            }
                            else
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, sp->tp->templateParam->second});
                            }
                        }
                        else
                        {
                            lex = prevsym(last);
                            goto join;
                        }
                    }
                    else
                    {
                        goto join;
                    }
                }
                else
                {
                    SYMBOL* name;
                    LEXLIST* start;
                    bool skip;
                join:
                    skip = false;
                    name = nullptr;
                    start = lex;
                    if (ISID(lex))
                    {
                        for (auto&& s : structSyms)
                        {
                            if (s.tmpl)
                            {
                                name = templatesearch(lex->data->value.s.a, s.tmpl);
                                if (name)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    if (name)
                    {
                        if (name->tp->type == bt_templateparam)
                        {
                            bool found = true;
                            lex = getsym();

                            if (MATCHKW(lex, classsel))
                            {
                                std::list<NAMESPACEVALUEDATA*>* nsv;
                                lex = prevsym(start);
                                lex = nestedPath(lex, &name, &nsv, nullptr, false, sc_parameter, false);
                                if (name && name->tp->type == bt_templateselector)
                                {
                                    lex = getsym();
                                    if (MATCHKW(lex, ellipse))
                                    {
                                        lex = getsym();
                                        std::list<TEMPLATEPARAMPAIR> a(itorig, iteorig);
                                        lst = expandTemplateSelector(lst, &a, tp);
                                        (*lst)->back().second->ellipsis = true;
                                        if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                                        {
                                            for (auto&& tpl1 : *(*lst)->back().second->byPack.pack)
                                                tpl1.second->ellipsis = true;
                                        }
                                        skip = true;
                                        first = false;
                                    }
                                    else
                                    {
                                        lex = prevsym(start);
                                    }
                                }
                                else
                                {
                                    lex = prevsym(start);
                                }
                            }
                            else if (name->tp->templateParam->second->type == kw_int)
                            {
                                if (MATCHKW(lex, ellipse))
                                {
                                    {
                                        if (!*lst)
                                            *lst = templateParamPairListFactory.CreateList();
                                        (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                        *(*lst)->back().second = *name->tp->templateParam->second;
                                        (*lst)->back().second->packed = true;
                                        auto last = (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList(); 
                                        if (name->tp->templateParam->second->packed)
                                        {
                                            if (name->tp->templateParam->second->byPack.pack)
                                                for (auto&& tpx : *name->tp->templateParam->second->byPack.pack)
                                                {
                                                    last->push_back(TEMPLATEPARAMPAIR{nullptr, tpx.second});
                                                }
                                        }
                                        else
                                        {
                                            last->push_back(TEMPLATEPARAMPAIR(nullptr, name->tp->templateParam->second));
                                        }
                                        lex = getsym();
                                        skip = true;
                                        first = false;
                                    }
                                }
                                else
                                {
                                    lex = prevsym(start);
                                }
                            }
                            else
                            {
                                lex = prevsym(start);
                            }
                        }
                        if (!skip)
                        {
                            lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
                            if (tp && tp->type == bt_templateparam)
                            {
                                if (parsingTrailingReturnOrUsing)
                                {
                                    std::list<TEMPLATEPARAMPAIR> a;
                                    a.push_back(*exp->v.sp->tp->templateParam);
                                    lst = expandArgs(lst, start, funcsp, &a, false);
                                }
                                else
                                {
                                    std::list<TEMPLATEPARAMPAIR> a;
                                    a.push_back(*tp->templateParam);
                                    lst = expandArgs(lst, start, funcsp, &a, false);
                                }
                                skip = true;
                                first = false;
                            }
                        }
                    }
                    else
                    {
                        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
                        if (!tp)
                        {
                            error(ERR_EXPRESSION_SYNTAX);
                        }
                    }
                    if (MATCHKW(lex, begin))
                    {
                        error(ERR_EXPECTED_TYPE_NEED_TYPENAME);
                        lex = getsym();
                        errskim(&lex, skim_end);
                        if (lex)
                            needkw(&lex, end);
                    }
                    if (!skip)
                    {
                        if (0)
                        {
                        initlistjoin:
                            start = nullptr;
                            name = nullptr;
                        }
                        if (MATCHKW(lex, ellipse))
                        {
                            // lose p
                            std::list<TEMPLATEPARAMPAIR>** tpx = lst;
                            lex = getsym();
                            if (templateNestingCount && tp->type == bt_templateparam)
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                *(*lst)->back().second = *tp->templateParam->second;
                                if (!tp->templateParam->second->packed)
                                {
                                    error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                                }
                                else
                                {
                                    (*lst)->back().second->packed = false;
                                    (*lst)->back().second->ellipsis = true;
                                    (*lst)->back().second->byNonType.dflt = exp;
                                    (*lst)->back().second->byNonType.tp = tp;
                                }
                            }
                            else if (templateNestingCount)
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                (*lst)->back().second->packed = true;
                                (*lst)->back().second->type = kw_int;
                                if (itorig != iteorig)
                                    (*lst)->back().first = itorig->first;
                                auto last = (*lst)->back().second->bySpecialization.types =
                                    templateParamPairListFactory.CreateList();
                                last->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                last->back().second->type = kw_int;
                                last->back().second->byNonType.tp = tp;
                                last->back().second->byNonType.dflt = exp;
                            }
                            else if (exp->type != en_packedempty)
                            {
                                // this is going to presume that the expression involved
                                // is not too long to be cached by the LEXLIST mechanism.
                                int oldPack = packIndex;
                                int count = 0;
                                SYMBOL* arg[200];
                                GatherPackedVars(&count, arg, exp);
                                expandingParams++;
                                if (count)
                                {
                                    int i;
                                    int n = CountPacks(arg[0]->tp->templateParam->second->byPack.pack);
                                    for (i = 1; i < count; i++)
                                    {
                                        if (CountPacks(arg[i]->tp->templateParam->second->byPack.pack) != n)
                                        {
                                            error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                                            break;
                                        }
                                    }
                                    for (i = 0; i < n; i++)
                                    {
                                        LEXLIST* lex = SetAlternateLex(start);
                                        packIndex = i;
                                        expression_assign(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_PACKABLE);
                                        if (exp)
                                        {
                                            optimize_for_constants(&exp);
                                            while (exp->type == en_void && exp->right)
                                                exp = exp->right;
                                        }
                                        ConstExprPatch(&exp);
                                        SetAlternateLex(nullptr);
                                        if (tp)
                                        {
                                            if (!*lst)
                                                *lst = templateParamPairListFactory.CreateList();
                                            (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                            (*lst)->back().second->type = kw_int;
                                            (*lst)->back().second->byNonType.dflt = exp;
                                            (*lst)->back().second->byNonType.tp = tp;
                                            if (itorig != iteorig)
                                                (*lst)->back().first = itorig->first;
                                        }
                                    }
                                }
                                expandingParams--;
                                packIndex = oldPack;
                            }
                            if (*lst)
                            {
                                (*lst)->back().second->ellipsis = true;
                                if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                                {
                                    for (auto&& tpl1 : *(*tpx)->back().second->byPack.pack)
                                        tpl1.second->ellipsis = true;
                                }
                            }
                        }
                        else
                        {
                            if (exp)
                            {
                                optimize_for_constants(&exp);
                                while (exp->type == en_void && exp->right)
                                    exp = exp->right;
                            }
                            if (tp && tp->type == en_templateparam)
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                *(*lst)->back().second = *tp->templateParam->second;
                                (*lst)->back().second->ellipsis = false;

                                if ((*lst)->back().second->packed)
                                {
                                    auto last = (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                    last->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                    last->back().second->type = kw_int;
                                    last->back().second->byNonType.dflt = exp;
                                    last->back().second->byNonType.val = nullptr;
                                }
                                else
                                {

                                    (*lst)->back().second->type = kw_int;
                                    (*lst)->back().second->byNonType.dflt = exp;
                                    (*lst)->back().second->byNonType.val = nullptr;
                                    (*lst)->back().second->byNonType.tp = tp;
                                }
                                if (exp->type == en_templateparam)
                                {
                                    (*lst)->back().first = exp->v.sp->tp->templateParam->first;
                                }
                                else if (itorig != iteorig)
                                {
                                    (*lst)->back().first = itorig->first;
                                }
                            }
                            else if (itorig != iteorig && itorig->second->packed)
                            {
                                std::list<TEMPLATEPARAMPAIR>** p;
                                if (first)
                                {
                                    if (!*lst)
                                        *lst = templateParamPairListFactory.CreateList();
                                    (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                    (*lst)->back().second->type = kw_int;
                                    (*lst)->back().second->packed = true;
                                    if (itorig != iteorig)
                                        (*lst)->back().first = itorig->first;
                                    first = false;
                                }
                                auto last = (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                last->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                last->back().second->type = kw_int;
                                last->back().second->byNonType.dflt = exp;
                                last->back().second->byNonType.tp = tp;
                            }
                            else
                            {
                                //                                checkUnpackedExpression(exp);
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                if (parsingTrailingReturnOrUsing && exp->type == en_templateparam && exp->v.sp)
                                {
                                    (*lst)->back().first = exp->v.sp;
                                }
                                else if (itorig != iteorig)
                                {
                                    (*lst)->back().first = itorig->first;
                                }
                                if (exp->type == en_templateparam && exp->v.sp->tp->templateParam->second->byClass.dflt)
                                {
                                    *(*lst)->back().second = *exp->v.sp->tp->templateParam->second;
                                }
                                else
                                {
                                    (*lst)->back().second->type = kw_int;
                                    (*lst)->back().second->byNonType.dflt = exp;
                                    (*lst)->back().second->byNonType.tp = tp;
                                }
                            }
                        }
                    }
                }
            }
            if (MATCHKW(lex, comma))
                lex = getsym();
            else
                break;
            if (itorig != iteorig && !itorig->second->packed)
            {
                ++itorig;
            }
        } while (true);
    }
    if (MATCHKW(lex, rightshift))
    {
        lex = getGTSym(lex);
    }
    else
    {
        if (!MATCHKW(lex, gt) && (tp && tp->type == bt_any && tp->sp))
        {
            errorsym(ERR_EXPECTED_END_OF_TEMPLATE_ARGUMENTS_NEAR_UNDEFINED_TYPE, tp->sp);
            errskim(&lex, skim_templateend);
        }
        else
        {
            needkw(&lex, gt);
        }
    }
    UnrollTemplatePacks(*start);
    inTemplateArgs--;
    noTypeNameError = oldnoTn;
    return lex;
}
static bool sameTemplateSpecialization(TYPE* P, TYPE* A)
{
    if (!P || !A)
        return false;
    P = basetype(P);
    A = basetype(A);
    if (isref(P))
        P = basetype(P->btp);
    if (isref(A))
        A = basetype(A->btp);
    if (!isstructured(P) || !isstructured(A))
        return false;
    if (P->sp->sb->parentClass != A->sp->sb->parentClass || strcmp(P->sp->name, A->sp->name) != 0)
        return false;
    if (P->sp->sb->templateLevel != A->sp->sb->templateLevel)
        return false;
    // this next if stmt is a horrible hack.
    if (P->size == 0 && !strcmp(P->sp->sb->decoratedName, A->sp->sb->decoratedName))
        return true;
    if (!P->sp->templateParams || !A->sp->templateParams)
        return false;
    auto itPL = P->sp->templateParams->begin();
    auto itePL = P->sp->templateParams->end();
    auto itPA = A->sp->templateParams->begin();
    auto itePA = A->sp->templateParams->end();
    if (itPL == itePL || itPA == itePA)  // errors
        return false;
    if (itPL->second->bySpecialization.types || !itPA->second->bySpecialization.types)
        return false;
    ++itPL;
    itePA = itPA->second->bySpecialization.types->end();
    itPA = itPA->second->bySpecialization.types->begin();

    if (itPL != itePL  && itPA != itePA)
    {
        for ( ; itPL != itePL && itPA != itePA; ++itPL, ++itPA)
        {
            if (itPL->second->type != itPA->second->type)
            {
                break;
            }
            else if (P->sp->sb->instantiated || A->sp->sb->instantiated)
            {
                if (itPL->second->type == kw_typename)
                {
                    if (!templatecomparetypes(itPL->second->byClass.dflt, itPA->second->byClass.val, true))
                        break;
                }
                else if (itPL->second->type == kw_template)
                {
                    if (!exactMatchOnTemplateParams(itPL->second->byTemplate.args, itPA->second->byTemplate.args))
                        break;
                }
                else if (itPL->second->type == kw_int)
                {
                    if (!templatecomparetypes(itPL->second->byNonType.tp, itPA->second->byNonType.tp, true))
                        break;
                    if (itPL->second->byNonType.dflt && !equalTemplateIntNode(itPL->second->byNonType.dflt, itPA->second->byNonType.val))
                        break;
                }
            }
        }
        return itPL == itePL && itPA == itePA;
    }
    return false;
}
bool exactMatchOnTemplateSpecialization(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym)
{
    if (old && sym)
    {
        auto itold = old->begin();
        auto iteold = old->end();
        auto itsym = sym->begin();
        auto itesym = sym->end();
        if (itsym != itesym && itsym->second->type == kw_new)
            ++itsym;
        for ( ; itold != iteold && itsym != itesym; ++itold, ++itsym)
        {
            if (itold->second->type != itsym->second->type)
                return false;
            switch (itold->second->type)
            {
                case kw_typename:
                    if (!sameTemplateSpecialization(itold->second->byClass.dflt, itsym->second->byClass.val))
                    {
                        if (!templatecomparetypes(itold->second->byClass.dflt, itsym->second->byClass.val, true))
                            return false;
                        if (!templatecomparetypes(itsym->second->byClass.val, itold->second->byClass.dflt, true))
                            return false;
                    }
                    break;
                case kw_template:
                    if (itold->second->byTemplate.dflt != itsym->second->byTemplate.val)
                        return false;
                    break;
                case kw_int:
                    if (!templatecomparetypes(itold->second->byNonType.tp, itsym->second->byNonType.tp, true))
                        return false;
                    if (itold->second->byNonType.dflt && !equalTemplateIntNode(itold->second->byNonType.dflt, itsym->second->byNonType.val))
                        return false;
                    break;
                default:
                    break;
            }
        }
        return itold == iteold && itsym == itesym;
    }
    return !old && !sym;
}
SYMBOL* FindSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams)
{
    if (sym->sb->specializations)
    {
        for (auto candidate : *sym->sb->specializations)
        {
            if (candidate->templateParams &&
                exactMatchOnTemplateArgs(templateParams, candidate->templateParams->front().second->bySpecialization.types))
            {
                return candidate;
            }
        }
    }
    return nullptr;
}
SYMBOL* LookupSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams)
{
    TYPE* tp;
    // maybe we know this specialization
    if (sym->sb->specializations)
    {
        for (auto candidate : *sym->sb->specializations)
        {
            if (candidate->templateParams && exactMatchOnTemplateArgs(templateParams->front().second->bySpecialization.types,
                                         candidate->templateParams->front().second->bySpecialization.types))
            {
                if (templateParams->size() == candidate->templateParams->size())
                {
                    return candidate;
                }
            }
        }
    }
    // maybe we know this as an instantiation
    bool found = false;
    SYMBOL* candidate = nullptr;
    if (sym->sb->instantiations)
    {
        for (auto it = sym->sb->instantiations->begin(); it != sym->sb->instantiations->end(); ++it)
        {
            if ((*it)->templateParams &&
                exactMatchOnTemplateSpecialization(templateParams->front().second->bySpecialization.types, (*it)->templateParams))
            {
                found = true;
                candidate = *it;
                sym->sb->instantiations->erase(it);
                break;
            }
        }
    }
    if (!found)
    {
        candidate = CopySymbol(sym);
        candidate->tp = CopyType(sym->tp);
        candidate->tp->sp = candidate;
        UpdateRootTypes(candidate->tp);
    }
    else
    {
        candidate->sb->maintemplate = candidate;
    }
    candidate->templateParams = templateParams;
    if (!sym->sb->specializations)
        sym->sb->specializations = symListFactory.CreateList();
    sym->sb->specializations->push_front(candidate);
    candidate->sb->overloadName = sym->sb->overloadName;
    candidate->sb->specialized = true;
    if (!candidate->sb->parentTemplate)
        candidate->sb->parentTemplate = sym;
    candidate->sb->baseClasses = nullptr;
    candidate->sb->vbaseEntries = nullptr;
    candidate->sb->vtabEntries = nullptr;
    tp = CopyType(candidate->tp);
    UpdateRootTypes(tp);
    candidate->tp = tp;
    candidate->tp->syms = nullptr;
    candidate->tp->tags = nullptr;
    candidate->sb->baseClasses = nullptr;
    candidate->sb->declline = candidate->sb->origdeclline = preProcessor->GetErrLineNo();
    candidate->sb->realdeclline = preProcessor->GetRealLineNo();
    candidate->sb->declfile = candidate->sb->origdeclfile = litlate(preProcessor->GetErrFile().c_str());
    candidate->sb->trivialCons = false;
    SetLinkerNames(candidate, lk_cdecl);
    return candidate;
}
static bool matchTemplatedType(TYPE* old, TYPE* sym, bool strict)
{
    while (1)
    {
        if (isconst(old) == isconst(sym) && isvolatile(old) == isvolatile(sym))
        {
            old = basetype(old);
            sym = basetype(sym);
            if (old->type == sym->type || (isfunction(old) && isfunction(sym)))
            {
                switch (old->type)
                {
                    case bt_struct:
                    case bt_class:
                    case bt_union:
                        return old->sp == sym->sp;
                    case bt_func:
                    case bt_ifunc:
                        if (!matchTemplatedType(old->btp, sym->btp, strict))
                            return false;
                        {
                            auto ito = old->syms->begin();
                            auto itoend = old->syms->end();
                            auto its = sym->syms->begin();
                            auto itsend = sym->syms->end();
                            if ((*ito)->sb->thisPtr)
                                ++ito;
                            if ((*its)->sb->thisPtr)
                                ++its;
                            while (ito != itoend && its != itsend)
                            {
                                if (!matchTemplatedType((*ito)->tp, (*its)->tp, strict))
                                    return false;
                                ++ito;
                                ++its;
                            }
                            return ito == itoend && its == itsend;
                        }
                    case bt_pointer:
                    case bt_lref:
                    case bt_rref:
                        if (old->array == sym->array && old->size == sym->size)
                        {
                            old = old->btp;
                            sym = sym->btp;
                            break;
                        }
                        return false;
                    case bt_templateparam:
                        return old->templateParam->second->type == sym->templateParam->second->type;
                    default:
                        return true;
                }
            }
            else
            {
                return !strict && old->type == bt_templateparam;
            }
        }
        else
        {
            return false;
        }
    }
}
SYMBOL* SynthesizeResult(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
static SYMBOL* ValidateArgsSpecified(std::list<TEMPLATEPARAMPAIR>* params, SYMBOL* func, std::list<INITLIST*>* args, std::list<TEMPLATEPARAMPAIR>* nparams);
static void saveParams(SYMBOL** table, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (table[i] && table[i]->templateParams)
        {
            for (auto&& param : *table[i]->templateParams)
            {
                if (param.second->type != kw_new)
                    param.second->hold = param.second->byClass.val;
            }
        }
    }
}
static void restoreParams(SYMBOL** table, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        if (table[i] && table[i]->templateParams)
        {
            for (auto&& param : *table[i]->templateParams)
            {
                if (param.second->type != kw_new)
                    param.second->byClass.val = (TYPE*)param.second->hold;
            }
        }
    }
}
SYMBOL* LookupFunctionSpecialization(SYMBOL* overloads, SYMBOL* sp)
{
    SYMBOL* found1 = nullptr;
    SYMBOL* sym = (SYMBOL*)overloads->tp->syms->front();
    SYMBOL* sd = getStructureDeclaration();
    saveParams(&sd, 1);
    if (sym->sb->templateLevel && !sym->sb->instantiated &&
        (!sym->sb->parentClass || sym->sb->parentClass->sb->templateLevel != sym->sb->templateLevel))
    {
        found1 = detemplate(overloads->tp->syms->front(), nullptr, sp->tp);
        if (found1 &&
            allTemplateArgsSpecified(
                found1, found1->templateParams))  // && exactMatchOnTemplateArgs(found1->templateParams, sp->templateParams))
        {
            std::list<TEMPLATEPARAMPAIR>* tpx;
            for (auto sp1 : *overloads->tp->syms)
            {
                if (exactMatchOnTemplateArgs(found1->templateParams, sp1->templateParams))
                    if (matchOverload(found1->tp, sp->tp, true))
                        return sp1;
            }
            if (matchOverload(found1->tp, sp->tp, true))
            {
                sp->templateParams->front().second->bySpecialization.types = copyParams(found1->templateParams, false);
                sp->templateParams->front().second->bySpecialization.types->pop_front();
                for (auto&& tpx : *sp->templateParams->front().second->bySpecialization.types)
                {
                    tpx.second->byClass.dflt = tpx.second->byClass.val;
                    tpx.second->byClass.val = nullptr;
                }
                SetLinkerNames(sp, lk_cdecl);
                found1 = sp;
            }
            else
            {
                found1 = nullptr;
            }
        }
        else
        {
            found1 = nullptr;
        }
    }
    restoreParams(&sd, 1);
    return found1;
}
LEXLIST* TemplateArgGetDefault(LEXLIST** lex, bool isExpression)
{
    LEXLIST *rv = nullptr, **cur = &rv;
    LEXLIST *current = *lex, *end = current;
    // this presumes that the template or expression is small enough to be cached...
    // may have to adjust it later
    // have to properly parse the default value, because it may have
    // embedded expressions that use '<'
    if (isExpression)
    {
        TYPE* tp;
        EXPRESSION* exp;
        end = expression_no_comma(current, nullptr, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
    }
    else
    {
        TYPE* tp;
        end = get_type_id(current, &tp, nullptr, sc_cast, false, true, false);
    }
    while (current && current != end)
    {
        *cur = Allocate<LEXLIST>();
        **cur = *current;
        (*cur)->next = nullptr;
        if (ISID(current))
            (*cur)->data->value.s.a = litlate((*cur)->data->value.s.a);
        current = current->next;
        cur = &(*cur)->next;
    }
    *lex = end;
    return rv;
}
static SYMBOL* templateParamId(TYPE* tp, const char* name, int tag )
{
    SYMBOL* rv = Allocate<SYMBOL>();
    rv->tp = tp;
    int len = strlen(name);
    char* buf = Allocate<char>(len + 5);
    strcpy(buf, name);
    *(int*)(buf + len + 1) = tag;
    rv->name = buf;
    return rv;
}
static LEXLIST* TemplateHeader(LEXLIST* lex, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* args)
{
    inTemplateHeader++;
    STRUCTSYM* structSyms = nullptr;
    std::list<TEMPLATEPARAMPAIR>* lst = args;
    if (needkw(&lex, lt))
    {
        while (1)
        {
            if (MATCHKW(lex, gt) || MATCHKW(lex, rightshift))
                break;
            args->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            lex = TemplateArg(lex, funcsp, args->back(), &lst);
            if (args)
            {
                if (!structSyms)
                {
                    structSyms = Allocate<STRUCTSYM>();
                    structSyms->tmpl = args;
                    addTemplateDeclaration(structSyms);
                }
            }
            if (!MATCHKW(lex, comma))
                break;
            lex = getsym();
        }
        for (auto&& search : *lst)
        {
            if (search.second->byClass.txtdflt)
            {
                std::list<SYMBOL*>* txtargs = symListFactory.CreateList();
                for (auto&& srch : *lst)
                    txtargs->push_back(srch.first);
                for (auto&& srch : *lst)
                    if (search.second->byClass.txtdflt)
                        search.second->byClass.txtargs = txtargs;
                break;
            }
        }
        if (MATCHKW(lex, rightshift))
            lex = getGTSym(lex);
        else
            needkw(&lex, gt);
    }
    inTemplateHeader--;
    return lex;
}
static LEXLIST* TemplateArg(LEXLIST* lex, SYMBOL* funcsp, TEMPLATEPARAMPAIR& arg, std::list<TEMPLATEPARAMPAIR>** lst)
{
    LEXLIST* current = lex;
    LEXLIST* txttype = nullptr;
    switch (KW(lex))
    {
        TYPE *tp, *tp1;
        EXPRESSION* exp1;
        SYMBOL* sp;
        case kw_class:
        case kw_typename:
            arg.second->type = kw_typename;
            arg.second->packed = false;
            lex = getsym();
            if (MATCHKW(lex, ellipse))
            {
                arg.second->packed = true;
                lex = getsym();
            }
            if (ISID(lex) || MATCHKW(lex, classsel))
            {
                SYMBOL *sym = nullptr, *strsym = nullptr;
                std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;

                lex = nestedPath(lex, &strsym, &nsv, nullptr, false, sc_global, false);
                if (strsym)
                {
                    if (strsym->tp->type == bt_templateselector)
                    {
                        sp = sym = templateParamId(strsym->tp, strsym->sb->templateSelector->back().name, 0);
                        lex = getsym();
                        tp = strsym->tp;
                        goto non_type_join;
                    }
                    else if (ISID(lex))
                    {
                        tp = MakeType(bt_templateselector);
                        sp = sym = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                        tp->sp = sym;
                        auto last = sym->sb->templateSelector = templateSelectorListFactory.CreateVector();
                        last->push_back(TEMPLATESELECTOR{});
                        last->back().sp = nullptr;
                        last->push_back(TEMPLATESELECTOR{});
                        last->back().sp = strsym;
                        if (strsym->sb->templateLevel)
                        {
                            last->back().isTemplate = true;
                            last->back().templateParams = strsym->templateParams;
                        }
                        last->push_back(TEMPLATESELECTOR{});
                        last->back().name = litlate(lex->data->value.s.a);
                        lex = getsym();
                        goto non_type_join;
                    }
                    else
                    {
                        lex = getsym();
                        error(ERR_TYPE_NAME_EXPECTED);
                        break;
                    }
                }
                else if (ISID(lex))
                {
                    TYPE* tp = MakeType(bt_templateparam);
                    tp->templateParam = &arg;
                    arg.first = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                    lex = getsym();
                }
                else
                {
                    lex = getsym();
                    error(ERR_TYPE_NAME_EXPECTED);
                    break;
                }
            }
            else
            {
                TYPE* tp = MakeType(bt_templateparam);
                tp->templateParam = &arg;
                arg.first = templateParamId(tp, AnonymousName(), templateNameTag++);
            }
            if (MATCHKW(lex, assign))
            {
                if (arg.second->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                lex = getsym();
                arg.second->byClass.txtdflt = TemplateArgGetDefault(&lex, false);
                if (!arg.second->byClass.txtdflt)
                {
                    error(ERR_CLASS_TEMPLATE_DEFAULT_MUST_REFER_TO_TYPE);
                }
            }
            if (!MATCHKW(lex, gt) && !MATCHKW(lex, leftshift) && !MATCHKW(lex, comma))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        case kw_template:
            arg.second->type = kw_template;
            lex = getsym();
            arg.second->byTemplate.args = templateParamPairListFactory.CreateList();
            lex = TemplateHeader(lex, funcsp, arg.second->byTemplate.args);
            if (arg.second->byTemplate.args)
                dropStructureDeclaration();
            arg.second->packed = false;
            if (!MATCHKW(lex, kw_class))
            {
                error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MUST_NAME_CLASS);
            }
            else
            {
                lex = getsym();
            }
            if (MATCHKW(lex, ellipse))
            {
                arg.second->packed = true;
                lex = getsym();
            }
            if (ISID(lex))
            {
                TYPE* tp = MakeType(bt_templateparam);
                tp->templateParam = &arg;
                arg.first = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                lex = getsym();
            }
            else
            {
                TYPE* tp = MakeType(bt_templateparam);
                tp->templateParam = &arg;
                arg.first = templateParamId(tp, AnonymousName(), templateNameTag++);
            }
            if (MATCHKW(lex, assign))
            {
                if (arg.second->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                arg.second->byTemplate.txtdflt = TemplateArgGetDefault(&lex, false);
                if (!arg.second->byTemplate.txtdflt)
                {
                    error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MISSING_DEFAULT);
                }
            }
            if (!MATCHKW(lex, gt) && !MATCHKW(lex, leftshift) && !MATCHKW(lex, comma))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        default:  // non-type
        {
            enum e_lk linkage, linkage2, linkage3;
            bool defd;
            bool notype;
            linkage = lk_none;
            linkage2 = lk_none;
            linkage3 = lk_none;
            defd = false;
            notype = false;
            arg.second->type = kw_int;
            arg.second->packed = false;
            tp = nullptr;
            sp = nullptr;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            noTypeNameError++;
            lex = getBasicType(lex, funcsp, &tp, nullptr, false, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3,
                               ac_public, &notype, &defd, nullptr, nullptr, false, true, false, false, false);
            noTypeNameError--;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            // get type qualifiers
            if (!ISID(lex) && !MATCHKW(lex, ellipse))
            {
                lex = getBeforeType(lex, funcsp, &tp, &sp, nullptr, nullptr, false, sc_cast, &linkage, &linkage2, &linkage3,
                                    nullptr, false, false, true, false); /* fixme at file scope init */
            }
            if (MATCHKW(lex, ellipse))
            {
                arg.second->packed = true;
                lex = getsym();
            }
            // get the name
            lex = getBeforeType(lex, funcsp, &tp, &sp, nullptr, nullptr, false, sc_cast, &linkage, &linkage2, &linkage3, nullptr,
                                false, false, false, false); /* fixme at file scope init */
            sizeQualifiers(tp);
            if (!tp || notype)
            {
                auto itlist = (*lst)->begin();
                auto itelist = (*lst)->end();
                if (sp && itlist != itelist && itlist->first)
                {
                    for ( ; itlist != itelist; ++itlist)
                    {
                        if (!itlist->first)
                            break;
                        if (!strcmp(itlist->first->name, sp->name))
                        {
                            tp = itlist->first->tp;
                            if (ISID(lex))
                            {
                                sp = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                                lex = getsym();
                            }
                            else
                            {
                                sp = templateParamId(tp, AnonymousName(), templateNameTag++);
                            }
                            goto non_type_join;
                        }
                    }
                }
                error(ERR_INVALID_TEMPLATE_PARAMETER);
            }
            else
            {
                if (!sp)
                {
                    sp = templateParamId(nullptr, AnonymousName(), templateNameTag++);
                }
            non_type_join:
                if (sp->sb)
                    sp->sb->storage_class = sc_templateparam;
                sp->tp = MakeType(bt_templateparam);
                sp->tp->templateParam = &arg;
                arg.second->type = kw_int;
                arg.first = Allocate<SYMBOL>();
                *arg.first = *sp;
                arg.first->sb = nullptr;
                if (isarray(tp) || isfunction(tp))
                {
                    if (isarray(tp))
                        tp = tp->btp;
                    tp = MakeType(bt_pointer, tp);
                }
                arg.second->byNonType.tp = tp;
                if (!isint(tp) && !ispointer(tp))
                {
                    LEXLIST* end = lex;
                    LEXLIST** cur = &txttype;
                    // if the type didn't resolve, we want to cache it then evaluate it later...
                    while (current && current != end)
                    {
                        *cur = Allocate<LEXLIST>();
                        **cur = *current;
                        (*cur)->next = nullptr;
                        if (ISID(current))
                            (*cur)->data->value.s.a = litlate((*cur)->data->value.s.a);
                        current = current->next;
                        cur = &(*cur)->next;
                    }
                }
                arg.second->byNonType.txttype = txttype;
                if (basetype(tp)->type != bt_templateparam && basetype(tp)->type != bt_templateselector &&
                    basetype(tp)->type != bt_enum && !isint(tp) && !ispointer(tp) && basetype(tp)->type != bt_lref &&
                    (!templateNestingCount || basetype(tp)->type != bt_any))
                {
                    error(ERR_NONTYPE_TEMPLATE_PARAMETER_INVALID_TYPE);
                }
                if (sp)
                {
                    if (MATCHKW(lex, assign))
                    {
                        tp1 = nullptr;
                        exp1 = nullptr;
                        lex = getsym();
                        arg.second->byNonType.txtdflt = TemplateArgGetDefault(&lex, true);
                        if (!arg.second->byNonType.txtdflt)
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                        else
                        {
                            TYPE* tp = nullptr;
                            EXPRESSION* exp = nullptr;
                            LEXLIST* lex = SetAlternateLex(arg.second->byNonType.txtdflt);
                            lex = expression_no_comma(lex, nullptr, nullptr, &tp, &exp, nullptr, 0);
                            SetAlternateLex(nullptr);
                            if (tp && isintconst(exp))
                            {
                                arg.second->byNonType.dflt = exp;
                            }
                        }
                    }
                }
            }
            break;
        }
    }

    return lex;
}
static bool matchArg(TEMPLATEPARAMPAIR& param, TEMPLATEPARAMPAIR& arg)
{
    if (param.second->type != arg.second->type)
    {
        return false;
    }
    else if (param.second->type == kw_template)
    {
        if (arg.second->byTemplate.dflt)
            if (!exactMatchOnTemplateParams(param.second->byTemplate.args, arg.second->byTemplate.dflt->templateParams))
                return false;
    }
    return true;
}
bool TemplateIntroduceArgs(std::list<TEMPLATEPARAMPAIR>* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (sym && args)
    {
        auto its = sym->begin();
        auto ita = args->begin();
        if (its != sym->end())
            ++its;
        for ( ; its != sym->end() && ita != args->end(); ++its, ++ita)
        {
            if (its->second->type == kw_template && ita->second->type == kw_typename)
            {
                TYPE* tp1 = ita->second->byClass.dflt;
                while (tp1 && tp1->type != bt_typedef && tp1->btp)
                    tp1 = tp1->btp;
                if (tp1->type != bt_typedef && !isstructured(tp1))
                    return false;
                its->second->byTemplate.val = tp1->sp;
            }
            else
            {
                if (!matchArg(*its, *ita))
                    return false;
                switch (ita->second->type)
                {
                    case kw_typename:
                        its->second->byClass.val = ita->second->byClass.dflt;
                        break;
                    case kw_template:
                        its->second->byTemplate.val = ita->second->byTemplate.dflt;
                        break;
                    case kw_int:
                        its->second->byNonType.val = ita->second->byNonType.dflt;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return true;
}
TYPE* SolidifyType(TYPE* tp)
{
    for (auto v = tp; v; v = v->btp)
    {
        if (isstructured(v))
        {
            auto rv = CopyType(tp, true);
            UpdateRootTypes(rv);
            for (auto v = rv; v; v = v->btp)
                if (isstructured(v))
                {
                    SYMBOL* old = basetype(v)->sp;
                    basetype(v)->sp = CopySymbol(old);
                    SYMBOL* sym = basetype(v)->sp;
                    if (!sym->sb->mainsym)
                        sym->sb->mainsym = old;
                    sym->tp = CopyType(sym->tp);
                    sym->tp->sp = sym;
                    sym->templateParams = SolidifyTemplateParams(copyParams(sym->templateParams, true));
                    UpdateRootTypes(sym->tp);
                    break;
                }
            return rv;
        }
    }
    return tp;
}
std::list<TEMPLATEPARAMPAIR>* SolidifyTemplateParams(std::list<TEMPLATEPARAMPAIR>* in)
{
    if (in)
    {
    for (auto&& v : *in)
    {
        if (v.second->type == kw_typename)
        {
            if (v.second->packed)
            {
                if (v.second->byPack.pack)
                    for (auto&& q : *v.second->byPack.pack)
                        q.second->byClass.val = SolidifyType(q.second->byClass.val);
            }
            else
            {
                v.second->byClass.val = SolidifyType(v.second->byClass.val);
            }
        }
    }
    }
    return in;
}

std::list<TEMPLATEPARAMPAIR>* copyParams(std::list<TEMPLATEPARAMPAIR>* t, bool alsoSpecializations)
{
    if (t)
    {
        std::list<TEMPLATEPARAMPAIR>* rv = templateParamPairListFactory.CreateList();
        for (auto&& parse : *t)
        {
            rv->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            *rv->back().second = *parse.second;
            auto sp = rv->back().first = parse.first ? CopySymbol(parse.first) : nullptr;
            if (sp)
            {
                sp->tp = MakeType(bt_templateparam);
                sp->tp->templateParam = &rv->back();
                rv->back().first = sp;
            }
        }
        if (t->front().second->type == kw_new && alsoSpecializations && t->front().second->bySpecialization.types)
        {
            auto last = rv->front().second->bySpecialization.types = templateParamPairListFactory.CreateList();
            for (auto&& parse : *t->front().second->bySpecialization.types)
            {
                last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                *last->back().second = *parse.second;
                last->back().first = parse.first ? CopySymbol(parse.first) : nullptr;
            }
        }
        auto it1 = rv->begin();
        for (auto&& parse : *t)
        {
            if (parse.second->type == kw_int)
            {
                if (parse.second->byNonType.tp && parse.second->byNonType.tp->type == bt_templateparam)
                {
                    auto it2 = rv->begin();
                    for (auto&& t1 : *t)
                    {
                        if (t1.second->type == kw_typename)
                        {
                            if (t1.second == parse.second->byNonType.tp->templateParam->second)
                            {
                                it1->second->byNonType.tp = CopyType(it1->second->byNonType.tp);
                                UpdateRootTypes(it1->second->byNonType.tp);
                                it1->second->byNonType.tp->templateParam = &(*it2);
                                break;
                            }
                        }
                        ++it2;
                    }
                }
            }
            ++it1;
        }
        return rv;
    }
    return t;
}
static SYMBOL* SynthesizeTemplate(TYPE* tp, SYMBOL* rvt, sym::_symbody* rvs, TYPE* tpt)
{
    SYMBOL* rv;
    std::list<TEMPLATEPARAMPAIR>* r = templateParamPairListFactory.CreateList();
    std::list<TEMPLATEPARAMPAIR>::iterator itp, itpe = itp;
    if (tp->sp->templateParams->front().second->bySpecialization.types)
    {
        itp = tp->sp->templateParams->front().second->bySpecialization.types->begin();
        itpe = tp->sp->templateParams->front().second->bySpecialization.types->end();
    }
    else
    {
        itp = tp->sp->templateParams->begin();
        itpe = tp->sp->templateParams->end();
        ++itp;
    }
    for ( ; itp != itpe; ++itp)
    {
        r->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
        *r->back().second = *itp->second;
    }
    rv = rvt;
    *rv = *tp->sp;
    rv->sb = rvs;
    *rv->sb = *tp->sp->sb;
    rv->sb->symRef = nullptr;
    rv->tp = tpt;
    *rv->tp = *tp;
    UpdateRootTypes(rv->tp);
    rv->tp->sp = rv;
    rv->templateParams = templateParamPairListFactory.CreateList();
    rv->templateParams->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
    rv->templateParams->back().second->type = kw_new;
    rv->templateParams->back().second->bySpecialization.types = r;
    return rv;
}

void SynthesizeQuals(TYPE*** last, TYPE** qual, TYPE*** lastQual)
{
    if (*qual)
    {
        TYPE* p = **last;
        TYPE* v = *qual;
        int sz = basetype(**last)->size;
        while (p && p->type == bt_derivedfromtemplate)
        {
            **last = CopyType(p);
            *last = &(**last)->btp;
            p = p->btp;
        }
        if ((*qual)->rootType && isref(*qual))
        {
            while (p && p != basetype(p))
            {
                **last = CopyType(p);
                *last = &(**last)->btp;
                p = p->btp;
            }
        }
        while (v)
        {
            **last = CopyType(v);
            if (!(**last)->rootType || !isref(**last))
                (**last)->size = sz;
            *last = &(**last)->btp;
            v = v->btp;
        }
        **last = nullptr;
        **last = CopyType(p, true);
        *lastQual = qual;
        *qual = nullptr;
    }
}

static EXPRESSION* copy_expression_data(EXPRESSION* exp)
{
    EXPRESSION* n = nzAllocate<EXPRESSION>();
    *n = *exp;
    // Easiest way to do a deep copy without including the rest of the tree
    n->left = nullptr;
    n->right = nullptr;
    return n;
}
// A better method to do this overall would probably be in Knuth's work, I did find something in the first volume (specifically
// section 2.3.2), however I was unable to replicate it and could not properly get the termination condition. This specific piece of
// code is translated from: https://stackoverflow.com/a/9647910 Stackoverflow user: kasavbere
EXPRESSION* copy_expression(EXPRESSION* head)
{
    if (head == nullptr)
        return nullptr;
    std::stack<EXPRESSION*> q;
    q.push(head);
    std::stack<EXPRESSION*> q2;
    EXPRESSION* nRoot = copy_expression_data(head);
    q2.push(nRoot);
    while (!q.empty())
    {
        EXPRESSION* n = q.top();
        q.pop();
        EXPRESSION* fresh = q2.top();
        q2.pop();
        if (n->left != nullptr)
        {
            q.push(n->left);
            fresh->left = copy_expression_data(n->left);
            q2.push(fresh->left);
        }
        if (n->right != nullptr)
        {
            q.push(n->right);
            fresh->right = copy_expression_data(n->right);
            q2.push(fresh->right);
        }
    }
    return nRoot;
}
static std::list<TEMPLATEPARAMPAIR>* paramsToDefault(std::list<TEMPLATEPARAMPAIR>* templateParams)
{
    if (templateParams)
    {
        auto params = templateParamPairListFactory.CreateList();
        for (auto&& find : *templateParams)
        {
            if (find.second->type != kw_new)
            {
                params->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                *params->back().second = *find.second;
                if (find.second->packed)
                {
                    params->back().second->byPack.pack = paramsToDefault(find.second->byPack.pack);
                    if ((find.second->lref || find.second->rref) && params->back().second->byPack.pack)
                    {
                        for (auto&& tpx : *params->back().second->byPack.pack)
                        {
                            // don't cascade references here...
                            if (isref(tpx.second->byClass.dflt))
                            {
                                TYPE* cursor = tpx.second->byClass.dflt;
                                TYPE *newType = nullptr, **tp1 = &newType;
                                while (1)
                                {
                                    *tp1 = CopyType(cursor);
                                    if (cursor == cursor->rootType)
                                    {
                                        // the reference collapsing rules say that if either ref is an lref we get an lref,
                                        // else we get an rref
                                        (*tp1)->type = find.second->lref || (*tp1)->type == bt_lref ? bt_lref : bt_rref;
                                        break;
                                    }
                                    tp1 = &(*tp1)->btp;
                                    cursor = cursor->btp;
                                };
                                (*tp1)->btp = cursor->btp;
                                tpx.second->byClass.dflt = newType;
                            }
                            else
                            {
                                tpx.second->byClass.dflt = MakeType(find.second->lref ? bt_lref : bt_rref, tpx.second->byClass.dflt);
                            }
                        }
                    }
                }
                else
                {
                    params->back().second->byClass.dflt = find.second->byClass.val;
                    params->back().second->byClass.val = nullptr;
                }
            }
        }
        return params;
    }
    return nullptr;
}

static std::list<TEMPLATEPARAMPAIR>** addStructParam(std::list<TEMPLATEPARAMPAIR>** pt, TEMPLATEPARAMPAIR& search, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    if (!search.second->byClass.val)
    {
        if (!search.first || search.second->byClass.dflt)
        {
            // DAL this next line is buggy, we should probably handled packed a little better...
            if (search.second->packed || !search.second->byClass.dflt)
                return nullptr;
            if (!*pt)
                *pt = templateParamPairListFactory.CreateList();
            (*pt)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            *(*pt)->back().second = *search.second;
            if (!templateNestingCount || instantiatingTemplate)
                (*pt)->back().second->byClass.dflt = SynthesizeType((*pt)->back().second->byClass.dflt, enclosing, false);
        }
        else
        {
            TEMPLATEPARAM* second = nullptr;
            for (auto&& find : *enclosing)
                if (find.first && strcmp(search.first->name, find.first->name) == 0)
                {
                    second = find.second;
                }
            if (!second)
            {
                SYMBOL* sym = nullptr;
                for (auto&& s : structSyms)
                {
                    if (s.tmpl)
                    {
                        sym = templatesearch(search.first->name, s.tmpl);
                        if (sym)
                            break;
                    }
                }
                if (!sym)
                    return nullptr;
                if (sym->tp->type != bt_templateparam || sym->tp->templateParam->second->type != kw_typename)
                    return nullptr;
                second = sym->tp->templateParam->second;
            }
            if (!*pt)
                *pt = templateParamPairListFactory.CreateList();
            (*pt)->push_back(TEMPLATEPARAMPAIR{nullptr, second});
        }
    }
    else
    {
        if (!*pt)
            *pt = templateParamPairListFactory.CreateList();
        (*pt)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
        *(*pt)->back().second = *search.second;
    }
    return pt;
}
static TYPE* SynthesizeStructure(TYPE* tp_in, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    TYPE* tp = basetype(tp_in);
    if (isref(tp))
        tp = basetype(tp->btp);
    if (isstructured(tp))
    {
        SYMBOL* sp = basetype(tp)->sp;
        if (sp->sb->templateLevel && !sp->sb->instantiated)
        {
            if (!allTemplateArgsSpecified(sp, sp->templateParams))
            {
                if (!templateNestingCount && sp->templateParams)
                {
                    for (auto it = sp->templateParams->begin(); it != sp->templateParams->end(); ++it)
                    {
                        auto l = *it;
                        if (l.second->byClass.txtdflt && !l.second->byClass.val)
                        {
                            std::list<TEMPLATEPARAMPAIR> a(it, sp->templateParams->end());
                            if (!TemplateParseDefaultArgs(sp, nullptr, &a, &a, &a))
                                return nullptr;
                            break;
                        }
                    }
                }
                if (!allTemplateArgsSpecified(sp, sp->templateParams))
                {
                    std::list<TEMPLATEPARAMPAIR> *pt = nullptr;
                    for (auto&& search : *sp->templateParams)
                    {
                        if (search.second->type != kw_new)
                        {
                            if (search.second->type == kw_typename)
                            {
                                if (search.second->byClass.dflt && search.second->byClass.dflt->type == bt_templateselector &&
                                    search.second->byClass.dflt->sp->sb->postExpansion)
                                {
                                    auto temp = (*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams;
                                    // this may needs some work with recursing templateselectors inside templateselectors...
                                    (*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams =
                                        paramsToDefault((*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams);
                                    expandTemplateSelector(&pt, enclosing, search.second->byClass.dflt);
                                    (*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams = temp;
                                }
                                else if (search.second->byClass.dflt && (search.second->byClass.dflt)->type == bt_memberptr)
                                {
                                    if (!pt)
                                        pt = templateParamPairListFactory.CreateList();
                                    pt->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                    *pt->back().second = *search.second;
                                    pt->back().second->byClass.dflt = SynthesizeType(search.second->byClass.dflt, enclosing, false);
                                }
                                else
                                {
                                    addStructParam(&pt, search, enclosing);
                                    if (!pt)
                                        return nullptr;
                                }
                            }
                            else
                            {
                                if (!pt)
                                    pt = templateParamPairListFactory.CreateList();
                                pt->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                *pt->back().second = *search.second;
                            }
                        }
                    }
                    sp = GetClassTemplate(sp, pt, false);
                }
                else
                {
                    sp = GetClassTemplate(sp, sp->templateParams, false);
                }
            }
            else
            {
                std::list<TEMPLATEPARAMPAIR>* params = paramsToDefault(sp->templateParams);
                SYMBOL* sp1 = CopySymbol(sp);
                sp1->tp = CopyType(sp->tp);
                UpdateRootTypes(sp1->tp);
                sp1->tp->sp = sp1;
                sp = sp1;
                sp = GetClassTemplate(sp, params, false);
            }

            if (sp)
            {
                auto tp1 = CopyType(sp->tp);
                if (isconst(tp_in))
                {
                    tp1 = MakeType(bt_const, tp1);
                }
                if (isvolatile(tp_in))
                {
                    tp1 = MakeType(bt_volatile, tp1);
                }
                if (isref(tp_in))
                {
                    tp1 = MakeType(basetype(tp_in)->type, tp1);
                }
                return tp1;
            }
        }
    }
    return nullptr;
}
static std::list<INITLIST*>* ExpandArguments(EXPRESSION* exp)
{
    std::list<INITLIST *>*rv = nullptr;
    bool dofunc = false;
    bool doparam = false;
    if (exp->v.func->arguments)
    {
        for (auto arg : *exp->v.func->arguments)
        {
            if (arg->exp && (arg->exp->type == en_func || arg->exp->type == en_funcret))
            {
                dofunc = true;
            }
            if (arg->tp && basetype(arg->tp)->type == bt_templateparam)
            {
                doparam |= !templateNestingCount || instantiatingTemplate;
            }
        }
        if (doparam)
        {
            TYPE* tp = nullptr;
            for (auto arg : *exp->v.func->arguments)
            {
                TYPE* tp1 = basetype(arg->tp);
                if (tp1 && tp1->type == bt_templateparam)
                {
                    if (tp1->templateParam->second->packed)
                    {
                        if (tp1->templateParam->second->byPack.pack)
                        {
                            for (auto&& tpx : *tp1->templateParam->second->byPack.pack)
                            {
                                auto dflt = tpx.second->byClass.val;
                                if (!dflt)
                                    dflt = tpx.second->byClass.dflt;
                                if (dflt)
                                {
                                    tp = tpx.second->byClass.val;
                                    if (isconst(arg->tp))
                                        tp = MakeType(bt_const, tp);
                                    if (isvolatile(arg->tp))
                                        tp = MakeType(bt_volatile, tp);
                                    if (!rv)
                                        rv = initListListFactory.CreateList();
                                    auto arg1 = Allocate<INITLIST>();
                                    arg1->tp = tp;
                                    arg1->exp = intNode(en_c_i, 0);
                                    rv->push_back(arg1);
                                }
                            }
                        }
                    }
                    else
                    {
                        auto arg1 = Allocate<INITLIST>();
                        *arg1 = *arg;
                        tp = tp1->templateParam->second->byClass.val;
                        if (tp)
                        {
                            if (isconst(arg->tp))
                                tp = MakeType(bt_const, tp);
                            if (isvolatile(arg->tp))
                                tp = MakeType(bt_volatile, tp);
                            arg1->tp = tp;
                        }
                        if (!rv)
                            rv = initListListFactory.CreateList();
                        rv->push_back(arg1);
                    }
                }
                else
                {
                    auto arg1 = Allocate<INITLIST>();
                    *arg1 = *arg;
                    if (!rv)
                        rv = initListListFactory.CreateList();
                    rv->push_back(arg1);
                }
            }
        }
        if (dofunc)
        {
            for (auto arg : *exp->v.func->arguments)
            {
                if (arg->exp)
                {
                    SYMBOL* syms[200];
                    int count = 0, n = 0;
                    GatherPackedVars(&count, syms, arg->exp);
                    if (count)
                    {
                        for (int i = 0; i < count; i++)
                        {
                            int n1 = CountPacks(syms[i]->tp->templateParam->second->byPack.pack);
                            if (n1 > n)
                                n = n1;
                        }
                        int oldIndex = packIndex;
                        for (int i = 0; i < n; i++)
                        {
                            std::deque<TEMPLATEPARAM*> defaults;
                            std::deque<std::pair<TYPE**, TYPE*>> types;
                            packIndex = i;
                            auto arg1 = Allocate<INITLIST>();
                            *arg1 = *arg;
                            if (!rv)
                                rv = initListListFactory.CreateList();
                            rv->push_back(arg1);
                            if (arg1->exp->type == en_func)
                            {
                                if (arg1->exp->v.func->templateParams)
                                {
                                    for (auto&& tpx : *arg1->exp->v.func->templateParams)
                                    {
                                        if (tpx.second->type != kw_new)
                                        {
                                            defaults.push_back(tpx.second);
                                            if (tpx.second->packed && tpx.second->byPack.pack)
                                            {
                                                auto it = tpx.second->byPack.pack->begin();
                                                auto ite = tpx.second->byPack.pack->end();
                                                for (int j = 0; j < packIndex && it != ite; ++j, ++it)
                                                    ;
                                                if (it != ite)
                                                    tpx.second = it->second;
                                            }
                                        }
                                    }
                                }
                                if (arg1->exp->v.func->arguments)
                                {
                                    for (auto il : *arg1->exp->v.func->arguments)
                                    {
                                        TYPE** tp = &il->tp;
                                        while ((*tp)->btp)
                                            tp = &(*tp)->btp;
                                        if ((*tp)->type == bt_templateparam)
                                        {
                                            auto tpx = (*tp)->templateParam;
                                            if (tpx->second->packed && tpx->second->byPack.pack)
                                            {
                                                auto it = tpx->second->byPack.pack->begin();
                                                auto ite = tpx->second->byPack.pack->end();
                                                for (int j = 0; j < packIndex && it != ite; ++j, ++it)
                                                    ;
                                                if (it != ite && it->second->type == kw_typename && it->second->byClass.val)
                                                {
                                                    types.push_back(std::pair<TYPE**, TYPE*>(tp, *tp));
                                                    (*tp) = it->second->byClass.val;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            arg1->tp = LookupTypeFromExpression(arg1->exp, nullptr, false);
                            if (arg1->tp && isref(arg1->tp))
                            {
                                bool rref = basetype(arg1->tp)->type == bt_rref;
                                arg1->tp = basetype(arg1->tp)->btp;
                                if (rref)
                                    (arg1->tp)->rref = true;
                                else
                                    (arg1->tp)->lref = true;
                            }
                            if (arg1->tp == nullptr)
                                arg1->tp = arg->tp;
                            if (arg1->exp->type == en_func)
                            {
                                if (arg1->exp->v.func->templateParams)
                                {
                                    for (auto&& tpx : *arg1->exp->v.func->templateParams)
                                    {
                                        if (tpx.second->type != kw_new)
                                        {
                                            tpx.second = defaults.front();
                                            defaults.pop_front();
                                        }
                                    }
                                }
                                for (auto&& t : types)
                                {
                                    *(t.first) = t.second;
                                }
                                types.clear();
                            }
                        }
                        packIndex = oldIndex;
                    }
                    else
                    {
                        auto arg1 = Allocate<INITLIST>();
                        *arg1 = *arg;
                        arg1->tp = LookupTypeFromExpression(arg1->exp, nullptr, false);
                        if (arg1->tp == nullptr)
                            arg1->tp = &stdany;
                        if (!rv)
                            rv = initListListFactory.CreateList();
                        rv->push_back(arg1);
                    }
                }
                else
                {
                    auto arg1 = Allocate<INITLIST>();
                    *arg1 = *arg;
                    if (!rv)
                        rv = initListListFactory.CreateList();
                    rv->push_back(arg1);
                }
            }
        }
        else if (!rv)
        {
            rv = exp->v.func->arguments;
        }
    }
    return rv;
}
void PushPopDefaults(std::deque<TYPE*>& defaults, std::list<TEMPLATEPARAMPAIR>* tpx, bool dflt, bool push);
static void PushPopDefaults(std::deque<TYPE*>& defaults, EXPRESSION* exp, bool dflt, bool push)
{
    std::stack<EXPRESSION*> stk;
    stk.push(exp);
    while (!stk.empty())
    {
        auto top = stk.top();
        stk.pop();
        if (top->type == en_templateselector)
        {
            auto ts = top->v.templateSelector->begin();
            ++ts;
            if (ts->isTemplate && ts->templateParams)
                PushPopDefaults(defaults, ts->templateParams, dflt, push);
        }
        else
        {
            if (top->left)
                stk.push(top->left);
            if (top->right)
                stk.push(top->right);
        }
    }
}
void PushPopDefaults(std::deque<TYPE*>& defaults, std::list<TEMPLATEPARAMPAIR>* tpx, bool dflt, bool push)
{
    std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> stk;
    if (tpx)
    {
        for (auto&& item : *tpx)
        {
            if (item.second->type != kw_new)
            {
                if (push)
                {
                    defaults.push_back(item.second->packed ? (TYPE*)1 : (TYPE*)0);
                    defaults.push_back(dflt ? item.second->byClass.dflt : item.second->byClass.val);
                    if (item.second->packed)
                    {
                        PushPopDefaults(defaults, item.second->byPack.pack, dflt, push);

                    }
                }
                else if (defaults.size())
                {
                    if (dflt)
                    {
                        item.second->packed = !! defaults.front();
                        defaults.pop_front();
                        if (dflt)
                            item.second->byClass.dflt = defaults.front();
                        else
                            item.second->byClass.val = defaults.front();
                    }
                    else
                    {
                        item.second->byClass.val = defaults.front();
                    }
                    defaults.pop_front();
                    if (item.second->packed)
                    {
                        PushPopDefaults(defaults, item.second->byPack.pack, dflt, push);
                    }
                }
                else
                {
                    if (dflt)
                    {
                        item.second->byClass.dflt = nullptr;
                    }
                    else
                    {
                        item.second->byClass.val = nullptr;
                    }
                }
                if (!item.second->packed && ((dflt && item.second->type == kw_typename && item.second->byClass.dflt &&
                        isstructured(item.second->byClass.dflt) && basetype(item.second->byClass.dflt)->sp->templateParams) ||
                    (!dflt && item.second->type == kw_typename && item.second->byClass.val &&
                        isstructured(item.second->byClass.val) && basetype(item.second->byClass.val)->sp->templateParams)))
                {
                    PushPopDefaults(defaults,
                                    basetype(dflt ? item.second->byClass.dflt : item.second->byClass.val)->sp->templateParams,
                                    dflt, push);
                }
                if (!item.second->packed && ((dflt && item.second->type == kw_int && item.second->byClass.dflt) ||
                    (!dflt && item.second->type == kw_typename && item.second->byClass.val)))
                {
                    PushPopDefaults(defaults, dflt ? item.second->byNonType.dflt : item.second->byNonType.val, dflt, push);
                }
            }
        }
    }
}
std::list<TEMPLATEPARAMPAIR>* ExpandParams(EXPRESSION* exp)
{
    if (templateNestingCount && !instantiatingTemplate)
        return exp->v.func->templateParams;
    if (!exp->v.func->templateParams)
        return nullptr;
    bool found = false;
    for (auto&& tpx : *exp->v.func->templateParams)
    {
        if (tpx.second->packed)
        {
            found = true;
            break;
        }
    }
    if (!found)
        return exp->v.func->templateParams;
    std::list<TEMPLATEPARAMPAIR>* rv = nullptr;
    for (auto&& tpx : *exp->v.func->templateParams)
    {
        if (tpx.second->packed)
        {
            if (tpx.second->byPack.pack)
            {
                if (!rv)
                    rv = templateParamPairListFactory.CreateList();
                for (auto tpx1 : *tpx.second->byPack.pack)
                {
                    rv->push_back(tpx1);
                    rv->back().second = tpx1.second;
                    if (tpx1.second->byClass.val)
                        rv->back().second->byClass.dflt = tpx1.second->byClass.val;
                }
            }
        }
        else
        {
            if (!rv)
                rv = templateParamPairListFactory.CreateList();
            rv->push_back(tpx);
            if (tpx.second->byClass.val)
                rv->back().second->byClass.dflt = tpx.second->byClass.val;
        }
    }
    return rv;
}
static TYPE* LookupUnaryMathFromExpression(EXPRESSION* exp, e_kw kw, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt) 
{
    TYPE* tp1 = LookupTypeFromExpression(exp->left, enclosing, alt);
    if (!tp1)
        return nullptr;
    if (isref(tp1))
        tp1 = basetype(tp1)->btp;
    auto exp1 = exp->left;
    ResolveTemplateVariable(&tp1, &exp1, nullptr, nullptr);
    if (!insertOperatorFunc(ovcl_binary_numericptr, kw, nullptr, &tp1, &exp1, nullptr, nullptr, nullptr, _F_SIZEOF))
    {
        castToArithmetic(false, &tp1, &exp1, kw, nullptr, true);
        if (isstructured(tp1))
            return nullptr;
        if (ispointer(tp1))
            return nullptr;
    }
    return tp1;
}
static TYPE* LookupBinaryMathFromExpression(EXPRESSION* exp, e_kw kw, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt)
{
    TYPE* tp1 = LookupTypeFromExpression(exp->left, enclosing, alt);
    if (!tp1)
        return nullptr;
    TYPE* tp2 = LookupTypeFromExpression(exp->right, enclosing, alt);
    if (!tp2)
        return nullptr;
    if (isref(tp1))
        tp1 = basetype(tp1)->btp;
    if (isref(tp2))
        tp2 = basetype(tp2)->btp;
    auto exp1 = exp->left;
    auto exp2 = exp->right;
    ResolveTemplateVariable(&tp1, &exp1, tp2, nullptr);
    ResolveTemplateVariable(&tp2, &exp2, tp1, nullptr);
    if (!insertOperatorFunc(ovcl_binary_numericptr, kw, nullptr, &tp1, &exp1, tp2, exp2, nullptr, _F_SIZEOF))
    {
        if (kw == leftshift || kw == rightshift)
        {
            castToArithmetic(false, &tp1, &exp1, kw, tp2, true);
            if (isstructured(tp1) || isstructured(tp2))
                return nullptr;
            if (ispointer(tp1) || ispointer(tp2))
                return nullptr;
        }
        else if ((kw != plus && kw != minus) || (!ispointer(tp1) && !ispointer(tp2)))
        {
            castToArithmetic(false, &tp1, &exp1, kw, tp2, true);
            castToArithmetic(false, &tp2, &exp2, (enum e_kw) - 1, tp1, true);
            if (isstructured(tp1) || isstructured(tp2))
                return nullptr;
            if (ispointer(tp1) || ispointer(tp2))
                return nullptr;
            tp1 = destSize(tp1, tp2, nullptr, nullptr, false, nullptr);
        }
        else
        {
            if (isstructured(tp1) || isstructured(tp2))
                return nullptr;
            if (ispointer(tp1) && ispointer(tp2))
            {
                tp1 = &stdint; // ptrdiff_t
            }
            else
            {      
                if (ispointer(tp2))
                    tp1 = tp2;
            }
        }
    }
    return tp1;
}
TYPE* LookupTypeFromExpression(EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt)
{
    EXPRESSION* funcList[100];
    int count = 0;
    switch (exp->type)
    {
        case en_dot:
        case en_pointsto: {
            TYPE* tp = LookupTypeFromExpression(exp->left, nullptr, false);
            if (!tp)
                return tp;
            while (exp->type == en_dot || exp->type == en_pointsto)
            {
                if (exp->type == en_pointsto)
                {
                    if (!ispointer(tp))
                        return nullptr;
                    tp = basetype(tp)->btp;
                }
                EXPRESSION* next = exp->right;
                if (next->type == en_dot || next->type == en_pointsto)
                {
                    next = exp->left;
                }
                STRUCTSYM s;
                while (isref(tp))
                    tp = basetype(tp)->btp;
                //                tp = PerformDeferredInitialization(tp, nullptr);
                s.str = basetype(tp)->sp;
                addStructureDeclaration(&s);
                while (next->type == en_funcret)
                    next = next->left;
                if (next->type == en_thisref)
                    next = next->left;
                if (next->type == en_func)
                {
                    TYPE* ctype = tp;
                    SYMBOL* sym = classsearch(next->v.func->sp->name, false, false);
                    if (!sym)
                    {
                        dropStructureDeclaration();
                        break;
                    }
                    FUNCTIONCALL* func = Allocate<FUNCTIONCALL>();
                    *func = *next->v.func;
                    func->sp = sym;
                    func->thistp = MakeType(bt_pointer, tp);
                    func->thisptr = intNode(en_c_i, 0);
                    func->arguments = ExpandArguments(next);
                    auto oldnoExcept = noExcept;
                    sym = GetOverloadedFunction(&ctype, &func->fcall, sym, func, nullptr, true, false, 0);
                    noExcept = oldnoExcept;
                    if (!sym)
                    {
                        dropStructureDeclaration();
                        break;
                    }
                    EXPRESSION* temp = varNode(en_func, sym);
                    temp->v.func = func;
                    temp = exprNode(en_thisref, temp, nullptr);
                    temp->v.t.thisptr = intNode(en_c_i, 0);
                    temp->v.t.tp = tp;
                    tp = LookupTypeFromExpression(temp, nullptr, false);
                }
                else
                {
                    SYMBOL* sym = classsearch(GetSymRef(next)->v.sp->name, false, false);
                    if (!sym)
                    {
                        dropStructureDeclaration();
                        break;
                    }
                    tp = sym->tp;
                }
                dropStructureDeclaration();
                exp = exp->right;
            }
            if (exp->type != en_dot && exp->type != en_pointsto)
                return tp;
            return nullptr;
        }
        case en_void:
            while (exp->type == en_void && exp->right)
            {
                if (!LookupTypeFromExpression(exp->left, enclosing, alt))
                    return nullptr;
                exp = exp->right;
            }
            if (exp)
            {
                return LookupTypeFromExpression(exp, enclosing, alt);
            }
            return nullptr;
        case en_not_lvalue:
        case en_lvalue:
        case en_argnopush:
        case en_voidnz:
        case en_shiftby:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_global:
        case en_auto:
        case en_labcon:
        case en_absolute:
        case en_pc:
        case en_const:
        case en_threadlocal: {
            TYPE* rv = exp->v.sp->tp;
            if (rv->type == bt_templateparam || (isref(rv) && basetype(rv->btp)->type == bt_templateparam))
                rv = SynthesizeType(rv, nullptr, false);
            return rv;
        }
        case en_x_label:
            return &stdpointer;
        case en_c_bit:  
        case en_c_bool:
        case en_x_bool:
        case en_x_bit:
        case en_l_bool:
        case en_l_bit:
            return &stdbool;
        case en_c_c:
        case en_x_c:
        case en_l_c:
            return &stdchar;
        case en_c_uc:
        case en_x_uc:
        case en_l_uc:
            return &stdunsignedchar;
        case en_c_wc:
        case en_x_wc:
        case en_l_wc:
            return &stdwidechar;
        case en_c_s:
        case en_x_s:
        case en_l_s:
            return &stdshort;
        case en_c_u16:
        case en_x_u16:
        case en_l_u16:
            return &stdchar16t;
        case en_c_us:
        case en_x_us:
        case en_l_us:
            return &stdunsignedshort;
        case en_c_i:
        case en_x_i:
        case en_l_i:
        case en_structelem:
            return &stdint;
        case en_c_ui:
        case en_x_ui:
        case en_l_ui:
        case en_sizeofellipse:
            return &stdunsigned;
        case en_x_inative:
        case en_l_inative:
            return &stdinative;
        case en_x_unative:
        case en_l_unative:
            return &stdunative;
        case en_c_u32:
        case en_x_u32:
        case en_l_u32:
            return &stdchar32t;
        case en_c_l:
        case en_x_l:
        case en_l_l:
            return &stdlong;
        case en_c_ul:
        case en_x_ul:
        case en_l_ul:
            return &stdunsignedlong;
        case en_c_ll:
        case en_x_ll:
        case en_l_ll:
            return &stdlonglong;
        case en_c_ull:
        case en_x_ull:
        case en_l_ull:
            return &stdunsignedlonglong;
        case en_c_f:
        case en_x_f:
        case en_l_f:
            return &stdfloat;
        case en_c_d:
        case en_x_d:
        case en_l_d:
            return &stddouble;
        case en_c_ld:
        case en_x_ld:
        case en_l_ld:
            return &stdlongdouble;
        case en_c_p:
        case en_x_p:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_l_ref: {
            TYPE* tp = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (tp && isref(tp))
                tp = basetype(tp)->btp;
            return tp;
        }
        case en_c_string:
        case en_l_string:
        case en_x_string:
            return &std__string;
        case en_x_object:
        case en_l_object:
            return &std__object;
        case en_l_p: {
            TYPE* tp = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (tp && ispointer(tp))
                tp = basetype(tp)->btp;
            return tp;
        }

        case en_c_sp:
        case en_x_sp:
        case en_l_sp:
            return &stdchar16t;
        case en_c_fp:
        case en_x_fp:
        case en_l_fp:
            return &stdpointer;  // fixme
        case en_c_fc:
        case en_x_fc:
        case en_l_fc:
            return &stdfloatcomplex;
        case en_c_dc:
        case en_x_dc:
        case en_l_dc:
            return &stddoublecomplex;
        case en_c_ldc:
        case en_x_ldc:
        case en_l_ldc:
            return &stdlongdoublecomplex;
        case en_c_fi:
        case en_x_fi:
        case en_l_fi:
            return &stdfloatimaginary;
        case en_c_di:
        case en_x_di:
        case en_l_di:
            return &stddoubleimaginary;
        case en_c_ldi:
        case en_x_ldi:
        case en_l_ldi:
            return &stdlongdoubleimaginary;
        case en_nullptr:
            return &stdnullpointer;
        case en_memberptr:
            return &stdpointer;
        case en_mp_as_bool:
            return &stdbool;
        case en_mp_compare:
            return &stdbool;
        case en_trapcall:
        case en_intcall:
            return &stdvoid;
        case en_construct:
            return exp->v.construct.tp;
        case en_funcret:
            while (exp->type == en_funcret)
            {
                funcList[count++] = exp;
                exp = exp->left;
            }
            /* fall through */
        case en_func: {
            TYPE* rv;
            EXPRESSION* exp1 = nullptr;
            if (basetype(exp->v.func->functp)->type != bt_aggregate && !isstructured(exp->v.func->functp) &&
                !basetype(exp->v.func->functp)->sp->sb->externShim)
            {
                if (exp->v.func->asaddress)
                {
                    rv = MakeType(bt_pointer, exp->v.func->functp);
                }
                else if (exp->v.func->sp->name == overloadNameTab[CI_CONSTRUCTOR])
                {
                    return basetype(exp->v.func->thistp)->btp;
                }
                else
                {
                    rv = basetype(exp->v.func->functp)->btp;
                }
            }
            else
            {
                TYPE* tp1 = nullptr;
                SYMBOL* sp;
                std::deque<TYPE*> defaults;
                if (exp->v.func->templateParams)
                {
                    for (auto&& tpx : * exp->v.func->templateParams)
                    {
                        if (tpx.second->type != kw_new)
                        {
                            defaults.push_back(tpx.second->byClass.dflt);
                            defaults.push_back(tpx.second->byClass.val);
                            if (tpx.second->byClass.val)
                                tpx.second->byClass.dflt = tpx.second->byClass.val;
                        }
                    }
                }
                std::list<INITLIST*>* old = exp->v.func->arguments;
                std::list<TEMPLATEPARAMPAIR>* oldp = exp->v.func->templateParams;
                exp->v.func->arguments = ExpandArguments(exp);
                exp->v.func->templateParams = ExpandParams(exp);
                auto oldnoExcept = noExcept;
                sp = GetOverloadedFunction(&tp1, &exp1, exp->v.func->sp, exp->v.func, nullptr, false, false, 0);
                noExcept = oldnoExcept;
                exp->v.func->arguments = old;
                exp->v.func->templateParams = oldp;
                if (exp->v.func->templateParams)
                {
                    for (auto&& tpx : *exp->v.func->templateParams)
                    {
                        if (tpx.second->type != kw_new)
                        {
                            tpx.second->byClass.dflt = defaults.front();
                            defaults.pop_front();
                            tpx.second->byClass.val = defaults.front();
                            defaults.pop_front();
                        }
                    }
                }
                if (sp)
                {
                    rv = basetype(sp->tp)->btp;
                }
                else
                {
                    rv = nullptr;
                }
            }
            while (count > 1 && rv)
            {
                TYPE* rve = rv;
                exp = funcList[--count];
                while (isref(rve))
                    rve = basetype(rve)->btp;
                if (isfuncptr(rve) || isfunction(rve) || isstructured(rve))
                {
                    std::list<INITLIST*>* old = nullptr;
                    if (exp->v.func)
                    {
                        old = exp->v.func->arguments;
                        exp->v.func->arguments = ExpandArguments(exp);
                    }
                    if (isstructured(rve))
                    {
                        rv = rve;
                        if (!exp->v.func || !insertOperatorParams(nullptr, &rv, &exp1, exp->v.func, 0))
                            rv = &stdany;
                        if (isfunction(rv))
                            rv = basetype(rv)->btp;
                    }
                    else if (isfunction(rve))
                    {
                        bool ascall = exp->v.func->ascall;
                        exp->v.func->ascall = true;
                        TYPE* tp1 = nullptr;
                        SYMBOL* sym = rve->sp;
                        if (sym->tp->type != bt_aggregate)
                            sym = sym->sb->overloadName;
                        rv = basetype(rve)->btp;
                        auto oldnoExcept = noExcept;
                        sym = GetOverloadedFunction(&tp1, &exp1, sym, exp->v.func, nullptr, false, false, 0);
                        noExcept = oldnoExcept;
                        if (!sym)
                            rv = &stdany;
                        else
                            rv = basetype(sym->tp)->btp;
                        exp->v.func->ascall = ascall;
                    }
                    else
                    {
                        rv = basetype(basetype(rve)->btp)->btp;
                    }
                    if (exp->v.func)
                    {
                        exp->v.func->arguments = old;
                    }
                    if (isconst(rve))
                    {
                        // to make LIBCXX happy
                        rv = MakeType(bt_const, rv);
                    }
                }
                else
                    break;
            }
            return rv;
        }
        case en_lt:
        case en_ult:
        {
            auto tp = LookupBinaryMathFromExpression(exp, lt, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_le:
        case en_ule:
        {
            auto tp = LookupBinaryMathFromExpression(exp, leq, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_gt:
        case en_ugt:
        {
            auto tp = LookupBinaryMathFromExpression(exp, gt, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_ge:
        case en_uge:
        {
            auto tp = LookupBinaryMathFromExpression(exp, geq, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_eq:
        {
            auto tp = LookupBinaryMathFromExpression(exp, eq, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_ne:
        {
            auto tp = LookupBinaryMathFromExpression(exp, neq, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_land:
        {
            auto tp = LookupBinaryMathFromExpression(exp, land, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_lor:
        {
            auto tp = LookupBinaryMathFromExpression(exp, lor, enclosing, alt);
            if (tp)
                tp = &stdbool;
            return tp;
        }
        case en_uminus:
            return LookupUnaryMathFromExpression(exp, minus, enclosing, alt);
        case en_not:
            return LookupUnaryMathFromExpression(exp, notx, enclosing, alt);
        case en_compl:
            return LookupUnaryMathFromExpression(exp, complx, enclosing, alt);
        case en_autoinc:
            return LookupUnaryMathFromExpression(exp, autoinc, enclosing, alt);
        case en_autodec:
            return LookupUnaryMathFromExpression(exp, autodec, enclosing, alt);
        case en_bits:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_assign: {
            TYPE* tp1 = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (tp1)
            {
                while (isref(tp1))
                    tp1 = basetype(tp1)->btp;
                if (isconst(tp1))
                    return nullptr;
                if (isstructured(tp1))
                {
                    SYMBOL* cons = search(basetype(tp1)->syms, overloadNameTab[CI_ASSIGN]);
                    if (!cons)
                        return nullptr;
                    TYPE* tp2 = LookupTypeFromExpression(exp->left, enclosing, alt);
                    TYPE* ctype = cons->tp;
                    TYPE thistp = {};
                    FUNCTIONCALL funcparams = {};
                    INITLIST a = {};
                    std::list<INITLIST*> args = { &a };
                    EXPRESSION x = {}, *xx = &x;
                    x.type = en_auto;
                    x.v.sp = cons;
                    a.tp = tp2;
                    a.exp = &x;
                    funcparams.arguments = &args;
                    MakeType(thistp, bt_pointer, basetype(tp1));
                    funcparams.thistp = &thistp;
                    funcparams.thisptr = &x;
                    funcparams.ascall = true;
                    auto oldnoExcept = noExcept;
                    cons = GetOverloadedFunction(&ctype, &xx, cons, &funcparams, nullptr, false, true, _F_SIZEOF);
                    noExcept = oldnoExcept;
                    if (!cons || cons->sb->deleted)
                    {
                        return nullptr;
                    }
                    tp1 = basetype(cons->tp)->btp;
                    while (isref(tp1))
                        tp1 = basetype(tp1)->btp;
                }
            }
            return tp1;
        }
        case en_templateparam:
            if (exp->v.sp->tp->templateParam->second->type == kw_typename)
            {
                if (exp->v.sp->tp->templateParam->second->packed)
                {
                    TYPE* rv = &stdany;
                    if (packIndex < 0)
                    {
                        if (exp->v.sp->tp->templateParam->second->byPack.pack)
                            rv = exp->v.sp->tp->templateParam->second->byPack.pack->front().second->byClass.val;
                    }
                    else if (exp->v.sp->tp->templateParam->second->byPack.pack)
                    {
                        auto itl = exp->v.sp->tp->templateParam->second->byPack.pack->begin();
                        auto itel = exp->v.sp->tp->templateParam->second->byPack.pack->end();
                        for (int i = 0; i < packIndex && itl != itel; i++, ++itl)
                            ;
                        if (itl != itel)
                            rv = itl->second->byClass.val;
                    }
                    return rv;
                }
                return exp->v.sp->tp->templateParam->second->byClass.val;
            }
            return nullptr;
        case en_templateselector: {
            EXPRESSION* exp1 = copy_expression(exp);
            optimize_for_constants(&exp1);
            if (exp1->type != en_templateselector)
                return LookupTypeFromExpression(exp1, enclosing, alt);
            return nullptr;
        }
        // the following several work because the front end should have cast both expressions already
        case en_cond:
        {
            TYPE* tl = LookupTypeFromExpression(exp->right->left, enclosing, alt);
            TYPE* tr = LookupTypeFromExpression(exp->right->right, enclosing, alt);
            if (isarithmetic(tl))
            {
                return destSize(tl, tr, &exp->right->left, &exp->right->right, false, nullptr);
            }
            else
            {
                return tl;
            }
        }
        case en_lsh:
            return LookupBinaryMathFromExpression(exp, leftshift, enclosing, alt);
        case en_rsh:
        case en_ursh:
            return LookupBinaryMathFromExpression(exp, rightshift, enclosing, alt);
        case en_arraymul:
        case en_arraylsh:
        case en_arraydiv:
        case en_arrayadd:
        case en_structadd:
        {
            TYPE* tp1 = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (!tp1)
                return nullptr;
            TYPE* tp1a = tp1;
            if (isref(tp1a))
                tp1a = basetype(tp1a)->btp;
            TYPE* tp2 = LookupTypeFromExpression(exp->right, enclosing, alt);
            if (!tp2)
                return nullptr;
            TYPE* tp2a = tp2;
            if (isref(tp2a))
                tp2a = basetype(tp2a)->btp;
            return destSize(tp1a, tp2a, nullptr, nullptr, false, nullptr);
            break;
        }
        case en_mul:
        case en_umul:
            return LookupBinaryMathFromExpression(exp, star, enclosing, alt);
        case en_mod:
        case en_umod:
            return LookupBinaryMathFromExpression(exp, mod, enclosing, alt);
        case en_div:
        case en_udiv:
            return LookupBinaryMathFromExpression(exp, divide, enclosing, alt);
        case en_and:
            return LookupBinaryMathFromExpression(exp, andx, enclosing, alt);
        case en_or:
            return LookupBinaryMathFromExpression(exp, orx, enclosing, alt);
        case en_xor:
            return LookupBinaryMathFromExpression(exp, uparrow, enclosing, alt);
        case en_add:
            return LookupBinaryMathFromExpression(exp, plus, enclosing, alt);
        case en_sub:
            return LookupBinaryMathFromExpression(exp, minus, enclosing, alt);
        case en_blockclear:
        case en_stackblock:
        case en_blockassign:
            switch (exp->left->type)
            {
                case en_global:
                case en_auto:
                case en_labcon:
                case en_absolute:
                case en_pc:
                case en_const:
                case en_threadlocal:
                    return exp->left->v.sp->tp;
                default:
                    break;
            }
            if (exp->right)
                switch (exp->right->type)
                {
                    case en_global:
                    case en_auto:
                    case en_labcon:
                    case en_absolute:
                    case en_pc:
                    case en_const:
                    case en_threadlocal:
                        return exp->right->v.sp->tp;
                    default:
                        break;
                }
            return nullptr;
        case en_thisref:
        case en_select:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        default:
            diag("LookupTypeFromExpression: unknown expression type");
            return nullptr;
    }
}
static bool HasUnevaluatedTemplateSelectors(EXPRESSION* exp)
{
    if (exp)
    {
        if (exp->left && HasUnevaluatedTemplateSelectors(exp->left))
            return true;
        if (exp->left && HasUnevaluatedTemplateSelectors(exp->right))
            return true;
        if (exp->type == en_templateselector)
        {
            optimize_for_constants(&exp);
            return exp->type == en_templateselector;
        }
    }
    return false;
}
TYPE* TemplateLookupTypeFromDeclType(TYPE* tp)
{
    static int nested;
    if (nested >= 10)
    {
        return nullptr;
    }
    nested++;
    EXPRESSION* exp = tp->templateDeclType;
    auto rv = LookupTypeFromExpression(exp, nullptr, false);
    nested--;
    return rv;
}

TYPE* SynthesizeType(TYPE* tp, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt)
{
    auto oldnoExcept = noExcept;
    TYPE *rv = &stdany, **last = &rv;
    TYPE *qual = nullptr, **lastQual = &qual;
    TYPE* tp_in = tp;
    while (true)
    {
        switch (tp->type)
        {
            case bt_typedef:
                if (tp->sp->sb->typeAlias)
                {
                    auto sp = GetTypeAliasSpecialization(tp->sp, tp->sp->templateParams);
                    if (sp)
                        tp = sp->tp;
                    else
                        tp = tp->btp;  // safety
                }
                else
                {
                    tp = tp->btp;
                }
                break;
            case bt_pointer:
                if (isarray(tp) && tp->etype)
                {
                    tp->etype = SynthesizeType(tp->etype, enclosing, alt);
                }
                {
                    TYPE* tp3 = tp->btp;
                    tp->btp = nullptr;
                    SynthesizeQuals(&last, &qual, &lastQual);
                    tp->btp = tp3;
                    *last = CopyType(tp);
                    (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                    UpdateRootTypes(rv);
                }
                return rv;
            case bt_templatedecltype:
                *last = LookupTypeFromExpression(tp->templateDeclType, enclosing, alt);
                if (!*last || (*last)->type == bt_any)
                {
                    return &stdany;
                }
                *last = SynthesizeType(*last, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                    UpdateRootTypes(rv);
                return rv;
            case bt_templateselector: {
                SYMBOL* sp;
                auto rvsit = tp->sp->sb->templateSelector->begin();
                auto rvsite = tp->sp->sb->templateSelector->end();
                rvsit++;
                SYMBOL* ts = rvsit->sp;
                for ( ; rvsit != rvsite ; )
                {
                    bool contin = false;
                    auto find = rvsit;
                    ++find;
                    if (rvsit->isTemplate && ts->templateParams)
                    {
                        std::list<TEMPLATEPARAMPAIR>::iterator current, currente;
                        if (rvsit->templateParams)
                        {
                            current = rvsit->templateParams->begin();
                            currente = rvsit->templateParams->end();
                        }
                        auto symit = ts->templateParams->begin();
                        auto symite = ts->templateParams->end();
                        ++symit;
                        std::deque<TYPE*> defaults;
                        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tps;
                        PushPopDefaults(defaults, rvsit->templateParams, true, true);
                        bool failed = false;
                        std::list<TEMPLATEPARAMPAIR>* args = nullptr;
                        if (current != currente)
                            args = templateParamPairListFactory.CreateList();
                        for ( ;current != currente;)
                        {
                            bool gennedPack = false;
                            if (current->second->packed && current->second->byPack.pack)
                            {
                                tps.push(current);
                                tps.push(currente);
                                currente = current->second->byPack.pack->end();
                                current = current->second->byPack.pack->begin();
                            }
                            if (current != currente)
                            {
                                if (current->second->type == kw_typename && current->second->byClass.dflt)
                                {
                                    if (current->second->byClass.dflt->type == bt_templateselector &&
                                        current->second->byClass.dflt->sp->sb->postExpansion)
                                    {
                                        auto dflt = current->second->byClass.dflt;
                                        current->second->byPack.pack = nullptr;
                                        current->second->packed = true;
                                        std::list<TEMPLATEPARAMPAIR>* pt = nullptr;
                                        expandTemplateSelector(&pt, enclosing, dflt);
                                        if (pt)
                                        {
                                            current->second->byPack.pack = pt->back().second->byPack.pack;
                                        }
                                        gennedPack = true;
                                    }
                                    else
                                    {
                                        current->second->byClass.dflt =
                                            SynthesizeType(current->second->byClass.dflt, enclosing, alt);
                                        if (!current->second->byClass.dflt || current->second->byClass.dflt->type == bt_any)
                                        {
                                            failed = true;
                                            break;
                                        }
                                    }
                                }
                                else if (current->second->type == kw_int)
                                {
                                    if (current->second->byNonType.dflt)
                                    {
                                        current->second->byNonType.dflt = copy_expression(current->second->byNonType.dflt);
                                        optimize_for_constants(&current->second->byNonType.dflt);
                                        if (HasUnevaluatedTemplateSelectors(current->second->byNonType.dflt))
                                        {
                                            failed = true;
                                            break;
                                        }
                                    }
                                    else if (current->second->byNonType.val)
                                    {
                                        current->second->byNonType.dflt = copy_expression(current->second->byNonType.val);
                                        optimize_for_constants(&current->second->byNonType.dflt);
                                        if (HasUnevaluatedTemplateSelectors(current->second->byNonType.val))
                                        {
                                            failed = true;
                                            break;
                                        }
                                    }
                                }
                                if (!gennedPack)
                                {
                                    // this assumes there will only be one pack...
                                    if (args->size() && args->back().second->packed)
                                    {
                                        args->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR(nullptr, current->second));
                                    }
                                    else
                                    {
                                        if (symit != symite)
                                        {
                                            if (symit->second->packed)
                                            {
                                                auto x = Allocate<TEMPLATEPARAM>();
                                                *x = *symit->second;
                                                x->byClass.val = nullptr;
                                                x->byClass.dflt = nullptr;
                                                if (current->second->packed)
                                                {
                                                    if (current->second->byPack.pack)
                                                    {
                                                        x->byPack.pack = templateParamPairListFactory.CreateList();
                                                        for (auto&& t : *current->second->byPack.pack)
                                                        {
                                                            x->byPack.pack->push_back(TEMPLATEPARAMPAIR(nullptr, t.second));
                                                        }
                                                    }
                                                }
                                                else
                                                {
                                                    x->byPack.pack = templateParamPairListFactory.CreateList();
                                                    x->byPack.pack->push_back(TEMPLATEPARAMPAIR(nullptr, current->second));
                                                }
                                                args->push_back(TEMPLATEPARAMPAIR(symit->first, x));
                                            }
                                            else
                                            {    
                                                args->push_back(TEMPLATEPARAMPAIR(symit->first, current->second));
                                            }
                                            ++symit;
                                        }
                                        else
                                        {
                                            args->push_back(TEMPLATEPARAMPAIR(nullptr, current->second));
                                        }
                                    }
                                }
                                else
                                {
                                    if (symit != symite)
                                    {
                                        args->push_back(TEMPLATEPARAMPAIR(symit->first, current->second));
                                        ++symit;
                                    }
                                    else
                                    {
                                        args->push_back(TEMPLATEPARAMPAIR(nullptr, current->second));
                                    }
                                }
                                ++current;
                                if (current == currente && tps.size())
                                {
                                    currente = tps.top();
                                    tps.pop();
                                    current = tps.top();
                                    tps.pop();
                                    ++current;
                                }
                            }
                        }
                        if (failed)
                        {
                            PushPopDefaults(defaults, rvsit->templateParams, true, false);
                            return &stdany;
                        }
                        if (ts->tp->type == bt_typedef)
                            sp = GetTypeAliasSpecialization(ts, args);
                        else
                            sp = GetClassTemplate(ts, args, true);
                        if (sp)
                            sp = TemplateClassInstantiateInternal(sp, rvsit->templateParams, false);
                        PushPopDefaults(defaults, rvsit->templateParams, true, false);
                        if (sp)
                            tp = sp->tp;
                        else
                            tp = &stdany;
                    }
                    else if (rvsit->isDeclType)
                    {
                        tp = TemplateLookupTypeFromDeclType(rvsit->tp);
                        if (!tp || tp->type == bt_any || !isstructured(tp))
                        {
                            return &stdany;
                        }
                        sp = basetype(tp)->sp;
                    }
                    else
                    {
                        tp = basetype(ts->tp);
                        if (tp)
                        {
                            if (tp->type == bt_templateselector)
                            {
                                tp = SynthesizeType(tp, enclosing, alt);
                                if (tp->type == bt_any)
                                {
                                    return tp;
                                }
                                sp = basetype(tp)->sp;
                            }
                            else if (tp->type == bt_templateparam)
                            {
                                if (tp->templateParam->second->type != kw_typename)
                                {
                                    return &stdany;
                                }
                                tp = alt ? tp->templateParam->second->byClass.temp : tp->templateParam->second->byClass.val;
                                if (!tp)
                                {
                                    return &stdany;
                                }
                                sp = basetype(tp)->sp;
                            }
                            else
                            {
                                sp = tp->sp;
                            }
                        }
                        else
                        {
                            sp = nullptr;
                        }
                    }
                    while (find != rvsite && sp)
                    {
                        if (!isstructured(tp))
                            break;

                        sp = search(basetype(tp)->syms, find->name);
                        if (!sp)
                        {
                            sp = classdata(find->name, basetype(tp)->sp, nullptr, false, false);
                            if (sp == (SYMBOL*)-1)
                                sp = nullptr;
                        }
                        if (sp &&
                            
                            sp->sb->access != ac_public && !resolvingStructDeclarations)
                        {
                            sp = nullptr;
                            break;
                        }
                        if (sp)
                        {
                            if (rvsit->asCall)
                            {
                                if (rvsit->arguments)
                                    for (auto i : *rvsit->arguments)
                                    {
                                        i->tp = SynthesizeType(i->tp, nullptr, false);
                                    }
                                TYPE* ctype = sp->tp;
                                EXPRESSION* exp = intNode(en_c_i, 0);
                                FUNCTIONCALL funcparams = { };
                                funcparams.arguments = rvsit->arguments;
                                auto oldnoExcept = noExcept;
                                auto sp1 = GetOverloadedFunction(&ctype, &exp, sp, &funcparams, nullptr, false, false, 0);
                                noExcept = oldnoExcept;
                                if (sp1)
                                {
                                    tp = basetype(sp1->tp)->btp;
                                }
                            }
                            auto it = rvsit;
                            ++it;
                            if (it != rvsite && it->isTemplate && it->templateParams)
                            {
                                ++rvsit;
                                ts = sp;
                                contin = true;
                                break;
                            }
                            tp = sp->tp;
                        }
                        else
                        {
                            break;
                        }
                        ++find;
                        ++rvsit;
                    }
                    if (contin)
                        continue;
                    if (find == rvsite && tp)
                    {
                        while (tp->type == bt_typedef && !tp->sp->sb->typeAlias && tp->btp->type != bt_templatedecltype)
                            tp = tp->btp;
                        TYPE* tp1 = tp;
                        while (ispointer(tp1))
                            tp1 = basetype(tp1)->btp;
                        tp1 = basetype(tp1);
                        if (tp1->type == bt_templateselector)
                        {
                            tp = SynthesizeType(tp, nullptr, false);
                        }
                        if (tp->type == bt_templateparam)
                        {
                            *last = tp->templateParam->second->byClass.dflt;
                            if (!*last)
                                *last = &stdany;
                        }
                        else
                        {
                            *last = tp;
                        }
                        if (rv)
                        {
                            rv = MakeType(bt_derivedfromtemplate, rv);
                            SynthesizeQuals(&last, &qual, &lastQual);
                        }
                        UpdateRootTypes(rv);
                        return rv;
                    }
                    return &stdany;
                }
            }
            case bt_rref:
                if (qual == nullptr && tp->btp->type == bt_templateparam && tp->btp->templateParam->second->byClass.val &&
                    tp->btp->templateParam->second->byClass.val->type == bt_lref)
                {
                    TYPE* tp1 = tp->btp->templateParam->second->byClass.val;
                    tp = basetype(tp1);
                    break;
                }
                // fallthrough
            case bt_lref:
                SynthesizeQuals(&last, &qual, &lastQual);
                *last = CopyType(tp);
                last = &(*last)->btp;
                tp = tp->btp;
                break;
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                *lastQual = CopyType(tp);
                (*lastQual)->btp = nullptr;
                lastQual = &(*lastQual)->btp;
                tp = tp->btp;
                break;
            case bt_memberptr:
                *last = CopyType(tp);
                {
                    TYPE* tp1 = tp->sp->tp;
                    if (tp1->type == bt_templateparam)
                    {
                        tp1 = tp1->templateParam->second->byClass.val;
                        (*last)->sp = tp1->sp;
                    }
                }
                (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;

            case bt_ifunc:
            case bt_func: {
                TYPE* func;
                *last = CopyType(tp);
                (*last)->syms = symbols.CreateSymbolTable();
                (*last)->btp = nullptr;
                func = *last;
                SynthesizeQuals(&last, &qual, &lastQual);
                if (*last)
                    last = &(*last)->btp;
                for (auto sp : *tp->syms)
                {
                    if (sp->packed && !sp->synthesized)
                    {
                        NormalizePacked(sp->tp);
                        if (sp->tp->templateParam && sp->tp->templateParam->second->packed)
                        {
                            std::list<TEMPLATEPARAMPAIR>* templateParams = sp->tp->templateParam->second->byPack.pack;
                            bool first = true;
                            sp->tp->templateParam->second->index = 0;
                            if (templateParams)
                            {
                                TYPE **last1, *qual1, *temp, **lastQual1 = &temp;
                                TYPE **btp = nullptr, *next;
                                next = sp->tp;
                                while (ispointer(next) || isref(next))
                                    next = basetype(next)->btp;
                                next = basetype(next);
                                qual1 = sp->tp;
                                if (qual1 == next)
                                    qual1 = nullptr;
                                else
                                {
                                    while (qual1->btp != next)
                                        qual1 = qual1->btp;
                                    btp = &qual1->btp;
                                    qual1 = sp->tp;
                                }
                                for (auto&& tpx : *templateParams)
                                {
                                    SYMBOL* clone = CopySymbol(sp);
                                    TYPE* tp1;
                                    clone->tp = SynthesizeType(tpx.second->byClass.val, enclosing, alt);
                                    if (!first)
                                    {
                                        clone->name = clone->sb->decoratedName = AnonymousName();
                                        clone->packed = false;
                                    }
                                    else
                                    {
                                        clone->tp->templateParam = sp->tp->templateParam;
                                    }
                                    TYPE *current = nullptr, **last = &current;
                                    if (qual1 && btp)
                                    {
                                        for (auto tpx = qual1; tpx != *btp; tpx = tpx->btp)
                                        {
                                            *last = Allocate<TYPE>();
                                            *(*last) = *tpx;
                                            last = &(*last)->btp;
                                        }
                                    }
                                    *last = clone->tp;
                                    tp1 = MakeType(bt_derivedfromtemplate, current);
                                    tp1->templateParam = clone->tp->templateParam;
                                    clone->tp = tp1;
                                    UpdateRootTypes(tp1);
                                    tpx.second->packsym = clone;
                                    func->syms->Add(clone);
                                    first = false;
                                    sp->tp->templateParam->second->index++;
                                }
                            }
                            else
                            {
                                SYMBOL* clone = CopySymbol(sp);
                                clone->tp = SynthesizeType(&stdany, enclosing, alt);
                                clone->tp->templateParam = sp->tp->templateParam;
                                func->syms->Add(clone);
                            }
                        }
                    }
                    else
                    {
                        SYMBOL* clone = CopySymbol(sp);
                        TYPE* tp1;
                        func->syms->Add(clone);
                        clone->tp = SynthesizeType(clone->tp, enclosing, alt);
                        if (clone->tp->type != bt_void && clone->tp->type != bt_any)
                        {
                            clone->tp = MakeType(bt_derivedfromtemplate, clone->tp);
                            UpdateRootTypes(clone->tp);
                        }
                    }
                }
                tp = tp->btp;
                break;
            }
            case bt_templateparam: {
                TEMPLATEPARAMPAIR* tpa = tp->templateParam;
                if (tpa->second->packed)
                {
                    if (!tpa->second->byPack.pack || tpa->second->index >= tpa->second->byPack.pack->size())
                    {
                        UpdateRootTypes(rv);
                        return rv;
                    }
                    auto it = tpa->second->byPack.pack->begin();
                    for (int i = 0; i < tpa->second->index; i++)
                        ++it;
                    tpa = &*it;
                }
                if (tpa->second->type == kw_typename)
                {
                    TYPE *type = alt ? tpa->second->byClass.temp : tpa->second->byClass.val, *typx = type;
                    while (type && type->type == bt_templateparam)
                    {
                        type = type->templateParam->second->byClass.val;
                        if (type == typx)
                        {
                            return &stdany;
                        }
                    }
                    if (type)
                    {
                        *last = CopyType(type);
                        (*last)->templateTop = true;
                        tp = MakeType(bt_derivedfromtemplate, rv);
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
                    else
                    {
                        if (!templateNestingCount && tpa->first)
                        {
                            bool found = false;
                            for (auto&& p : structSyms)
                            {
                                if (p.tmpl)
                                {
                                    SYMBOL* s = templatesearch(tpa->first->name, p.tmpl);
                                    if (s && s->tp->templateParam->second->byClass.val)
                                    {
                                        found = true;
                                        *last = CopyType(s->tp->templateParam->second->byClass.val);
                                        break;
                                    }
                                }
                            }
                            if (!found)
                            {
                                *last = CopyType(tp);
                            }
                        }
                        else
                        {
                            *last = CopyType(tp);
                        }
                    }
                    UpdateRootTypes(rv);
                    return rv;
                }
                else if (tpa->second->type == kw_template)
                {
                    TYPE* type = alt ? tpa->second->byTemplate.temp->tp : tpa->second->byTemplate.val->tp;
                    if (type)
                    {
                        *last = CopyType(type);
                        (*last)->templateTop = true;
                        rv = MakeType(bt_derivedfromtemplate, rv);
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
                    UpdateRootTypes(rv);
                    return rv;
                }
                else
                {
                    return &stdany;
                }
            }
            default:
                if (alt && isstructured(tp))
                {
                    tp_in = CopyType(tp);
                    tp_in->sp = CopySymbol(tp_in->sp);
                    if (tp->sp->templateParams)
                    {
                        auto last = tp_in->sp->templateParams = templateParamPairListFactory.CreateList();
                        for (auto&& tpx : *tp_in->sp->templateParams)
                        {
                            if (tpx.second->type == kw_typename && tpx.second->byClass.temp)
                            {
                                last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                *last->back().second = *tpx.second;
                                last->back().second->byClass.val = SynthesizeType(tpx.second->byClass.temp, enclosing, false);
                            }
                            else
                            {
                                last->push_back(TEMPLATEPARAMPAIR{nullptr, tpx.second});
                            }
                        }
                    }
                    else
                    {
                        tp_in->sp->templateParams = nullptr;
                    }
                    tp = tp_in;
                }
                else if (enclosing)
                {
                    tp_in = SynthesizeStructure(tp, /*basetype(tp)->sp ? basetype(tp)->sp->templateParams :*/ enclosing);
                    if (tp_in)
                    {
                        rv = MakeType(bt_derivedfromtemplate, rv);
                        tp = tp_in;
                    }
                }
                *last = tp;
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;
        }
    }
}
static bool hasPack(TYPE* tp)
{
    bool rv = false;
    while (ispointer(tp))
        tp = tp->btp;
    if (isfunction(tp))
    {
        for (auto sym : *tp->syms)
        {
            if (rv)
                break;
            if (sym->packed)
            {
                rv = true;
            }
            else if (isfunction(sym->tp) || isfuncptr(sym->tp))
            {
                rv = hasPack(sym->tp);
            }
        }
    }
    return rv;
}
static SYMBOL* SynthesizeParentClass(SYMBOL* sym)
{
    SYMBOL* rv = sym;
    SYMBOL* syms[500];
    int count = 0;
    if (templateNestingCount)
        return sym;
    while (sym)
    {
        syms[count++] = sym;
        sym = sym->sb->parentClass;
    }
    if (count)
    {
        int i;
        for (i = count - 1; i >= 0; i--)
        {
            if (syms[i]->sb->templateLevel && !syms[i]->sb->instantiated)
            {
                break;
            }
        }
        if (i >= 0)
        {
            SYMBOL* last = nullptr;
            rv = nullptr;

            // has templated classes
            for (i = count - 1; i >= 0; i--)
            {
                if (syms[i]->sb->templateLevel)
                {
                    SYMBOL* found = TemplateClassInstantiateInternal(syms[i], syms[i]->templateParams, true);
                    if (!found)
                    {
                        diag("SynthesizeParentClass mismatch 1");
                        return sym;
                    }
                    found = CopySymbol(found);
                    found->templateParams = copyParams(found->templateParams, true);
                    found->sb->parentClass = last;
                    last = found;
                }
                else
                {
                    if (last)
                    {
                        SYMBOL* found = search(last->tp->syms, syms[i]->name);
                        if (!found || !isstructured(found->tp))
                        {
                            diag("SynthesizeParentClass mismatch 2");
                            return sym;
                        }
                        found->sb->parentClass = last;
                        last = found;
                    }
                    else
                    {
                        last = syms[i];
                    }
                }
                rv = last;
            }
        }
    }
    return rv;
}
SYMBOL* SynthesizeResult(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params)
{
    SYMBOL* rsv = CopySymbol(sym);
    STRUCTSYM s, s1;

    if (sym->sb->parentClass)
    {
        s.str = sym->sb->parentClass;
        addStructureDeclaration(&s);
    }
    s1.tmpl = sym->templateParams;
    addTemplateDeclaration(&s1);
    rsv->sb->parentTemplate = sym;
    rsv->sb->mainsym = sym;
    rsv->sb->parentClass = SynthesizeParentClass(rsv->sb->parentClass);
    rsv->tp = SynthesizeType(sym->tp, params, false);
    if (isfunction(rsv->tp))
    {
        basetype(rsv->tp)->btp = PerformDeferredInitialization(basetype(rsv->tp)->btp, nullptr);
    }
    if (isfunction(rsv->tp))
    {
        basetype(rsv->tp)->sp = rsv;
    }
    rsv->templateParams = params;
    if (sym->sb->parentClass)
    {
        dropStructureDeclaration();
    }
    dropStructureDeclaration();
    return rsv;
}
static TYPE* RemoveCVQuals(TYPE* A) { return basetype(A); }
static TYPE* rewriteNonRef(TYPE* A)
{
    if (isarray(A))
    {
        while (isarray(A))
            A = basetype(A)->btp;
        A = MakeType(bt_pointer, A);
    }
    else if (isfunction(A))
    {
        A = MakeType(bt_pointer, basetype(A));
    }
    return A;
}
static bool hastemplate(EXPRESSION* exp)
{
    if (!exp)
        return false;
    if (exp->type == en_templateparam || exp->type == en_templateselector)
        return true;
    return hastemplate(exp->left) || hastemplate(exp->right);
}
static void clearoutDeduction(TYPE* tp)
{
    while (1)
    {
        switch (tp->type)
        {
            case bt_pointer:
                if (isarray(tp) && tp->etype)
                {
                    clearoutDeduction(tp->etype);
                }
                tp = tp->btp;
                break;
            case bt_templateselector:
                clearoutDeduction((*tp->sp->sb->templateSelector)[1].sp->tp);
                return;
            case bt_const:
            case bt_volatile:
            case bt_lref:
            case bt_rref:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_memberptr:
                clearoutDeduction(tp->sp->tp);
                tp = tp->btp;
                break;
            case bt_func:
            case bt_ifunc: {
                for (auto sym : *tp->syms)
                {
                    clearoutDeduction(sym->tp);
                }
                tp = tp->btp;
                break;
            }
            case bt_templateparam:
                tp->templateParam->second->byClass.temp = nullptr;
                return;
            default:
                return;
        }
    }
}
static void ClearArgValues(std::list<TEMPLATEPARAMPAIR>* params, bool specialized)
{
    if (params)
    {
        for (auto&& param: *params)
        {
            param.second->deduced = false;
            param.second->initialized = false;
            if (param.second->type != kw_new)
            {
                if (param.second->packed)
                    param.second->byPack.pack = nullptr;
                else
                    param.second->byClass.val = param.second->byClass.temp = nullptr;
                if (param.second->byClass.txtdflt && !specialized && !param.second->specializationParam)
                    param.second->byClass.dflt = nullptr;
                if (param.second->byClass.dflt)
                {
                    if (param.second->type == kw_typename)
                    {
                        TYPE* tp = param.second->byClass.dflt;
                        while (ispointer(tp))
                            tp = basetype(tp)->btp;
                        tp = basetype(tp);
                        if (tp->type == bt_templateparam)
                        {
                            tp->templateParam->second->byClass.val = nullptr;
                        }
                        else if (isstructured(tp) && (tp->sp)->sb->attribs.inheritable.linkage4 != lk_virtual)
                        {
                            ClearArgValues(tp->sp->templateParams, specialized);
                        }
                        else if (tp->type == bt_templateselector)
                        {
                            ClearArgValues((*tp->sp->sb->templateSelector)[1].templateParams, specialized);
                        }
                    }
                    else
                    {
                        param.second->byClass.val = nullptr;
                    }
                }
            }
        }
    }
}
static void PushPopValues(std::list<TEMPLATEPARAMPAIR>* params, bool push)
{
    if (params)
    {
        for (auto&& param : *params)
        {
            if (param.second->type != kw_new)
            {
                Optimizer::LIST* lst;
                if (push)
                {
                    lst = Allocate<Optimizer::LIST>();
                    lst->next = param.second->stack;
                    param.second->stack = lst;
                }
                else
                {
                    lst = (Optimizer::LIST*)param.second->stack->data;
                    param.second->stack = param.second->stack->next;
                }
                if (param.second->packed)
                {
                    if (push)
                    {
                        lst->data = param.second->byPack.pack;
                        PushPopValues(param.second->byPack.pack, push);
                    }
                    else
                    {
                        param.second->byPack.pack = (std::list<TEMPLATEPARAMPAIR>*)lst;
                    }
                }
                else
                {
                    if (push)
                    {
                        lst->data = param.second->byClass.val;
                        if (param.second->type == kw_typename)
                        {
                            TYPE* tp = param.second->byClass.val;
                            if (tp)
                            {
                                while (ispointer(tp) || isref(tp))
                                    tp = basetype(tp)->btp;
                                if (isstructured(tp))
                                {
                                    PushPopValues(basetype(tp)->sp->templateParams, push);
                                }
                                else if (basetype(tp)->type == bt_templateselector)
                                {
                                    PushPopValues((*basetype(tp)->sp->sb->templateSelector)[1].templateParams, push);
                                }
                            }
                        }
                    }
                    else
                    {
                        param.second->byClass.val = (TYPE*)lst;
                    }
                }
            }
        }
    }
}
static bool Deduce(TYPE* P, TYPE* A, EXPRESSION* exp, bool change, bool byClass, bool allowSelectors, bool baseClasses);
static bool DeduceFromTemplates(TYPE* P, TYPE* A, bool change, bool byClass)
{
    TYPE* pP = basetype(P);
    TYPE* pA = basetype(A);
    if (pP->sp && pA->sp && pP->sp->sb && pA->sp->sb && pP->sp->sb->parentTemplate == pA->sp->sb->parentTemplate)
    {
        if (!pA->sp->templateParams || !pP->sp->templateParams)
            return false;
        auto itTP = pP->sp->templateParams->begin();
        auto itTPe = pP->sp->templateParams->end();
        auto itTA = pA->sp->templateParams->begin();
        auto itTAe = pA->sp->templateParams->end();
        auto itTAo = itTA;
        auto isspecialized = itTP->second->bySpecialization.types ? itTP : itTPe;
        auto isespecialized = itTPe;
        if (itTA != itTAe)
        {
            if (itTA->second->bySpecialization.types)
            {
                itTAe = itTA->second->bySpecialization.types->end();
                itTA = itTA->second->bySpecialization.types->begin();
            }
            else
            {
                ++itTA;
            }
        }
        if (itTP != itTPe)
        {
            if (itTP->second->bySpecialization.types)
            {
                itTPe = itTP->second->bySpecialization.types->end();
                itTP = itTP->second->bySpecialization.types->begin();
            }
            else
            {
                ++itTP;
            }
        }
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
        for (; itTP != itTPe && itTA != itTAe; ++itTP)
        {
            auto to = itTP;
            if (itTA->second->packed)
            {
                if (itTA->second->byPack.pack == nullptr)
                {
                    if (itTA->second->type == itTP->second->type)
                    {
                        if (itTP->second->packed)
                        {
                            if (itTP->second->byPack.pack == nullptr)
                            {
                                ++itTA;
                                continue;
                            }
                        }
                        else
                        {
                            if (!itTP->second->byClass.val)
                            {
                                ++itTA;
                                continue;
                            }
                        }
                    }
                }
                if (itTA->second->byPack.pack)
                {
                    tas.push(itTA);
                    tas.push(itTAe);
                    itTAe = itTA->second->byPack.pack->end();
                    itTA = itTA->second->byPack.pack->begin();
                }
            }
            if (itTA == itTAe)
            {
                if (!tas.empty())
                {
                    itTAe = tas.top();
                    tas.pop();
                    itTA = tas.top();
                    tas.pop();
                    ++itTA;
                    continue;
                }
                break;
            }
            if (itTP->second->type != itTA->second->type)
                return false;
            if (itTP->second->packed)
            {
                break;
            }
            if (isspecialized != isespecialized && itTP->first)
            {
                auto il = isspecialized;
                for (++il; il != isespecialized; ++il)
                {
                    if (il->first && !strcmp(il->first->name, itTP->first->name))
                    {
                        to = il;
                        break;
                    }
                }
            }
            switch (itTP->second->type)
            {
                case kw_typename: {
                    TYPE** tp = change ? &to->second->byClass.val : &to->second->byClass.temp;
                    if (*tp)
                    {
                        if (!templatecomparetypes(*tp, itTA->second->byClass.val, true))
                            return false;
                    }
                    else
                    {
                        *tp = itTA->second->byClass.val;
                    }
                    to->second->deduced = true;
                    if (to->second->byClass.dflt && to->second->byClass.val &&
                        !Deduce(to->second->byClass.dflt, to->second->byClass.val, nullptr, change, byClass, false, false))
                        return false;
                    break;
                }
                case kw_template: {
                    std::list<TEMPLATEPARAMPAIR>* paramT = to->first->templateParams;
                    std::list<TEMPLATEPARAMPAIR>* paramA = itTA->first->templateParams;
                    if (!paramT || !paramA || paramT->size() != paramA->size())
                        return false;
                    auto it1 = paramT->begin();
                    auto it2 = paramA->begin();
                    for (int i = 0; i < paramT->size(); ++i, ++it1, ++it2)
                    {
                        if (it1->second->type != it2->second->type)
                            return false;
                    }
                    if (!to->second->byTemplate.val)
                        to->second->byTemplate.val = itTA->second->byTemplate.val;
                    else if (!DeduceFromTemplates(to->second->byTemplate.val->tp, itTA->second->byTemplate.val->tp, change, byClass))
                        return false;
                    break;
                }
                case kw_int: {
                    EXPRESSION** exp;
                    if (itTAo->second->bySpecialization.types)
                    {
                    }
                    exp = change ? &to->second->byNonType.val : &to->second->byNonType.temp;
                    if (itTA->second->byNonType.val && to->second->byNonType.val && !equalTemplateIntNode(to->second->byNonType.val, *exp))
                        return false;
                    to->second->deduced = true;
                    *exp = itTA->second->byNonType.val;
                    break;
                }
                default:
                    break;
            }
            ++itTA;
        }
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tps;
        while (!tas.empty())
            tas.pop();
        if (itTP != itTPe && itTP->second->packed)
        {
            if (isspecialized != itTPe && itTP->first)
            {
                auto il = isspecialized;
                for (++il; il != itTPe; ++il)
                {
                    if (il->first && !strcmp(il->first->name, itTP->first->name))
                    {
                        itTP = il;
                        break;
                    }
                }
            }
            if (itTP->second->byPack.pack)
            {
                tps.push(itTP);
                tps.push(itTPe);
                itTPe = itTP->second->byPack.pack->end();
                itTP = itTP->second->byPack.pack->begin();
                for (;  itTP != itTPe && itTA != itTAe;)
                {
                    if (itTP->second->type != itTA->second->type)
                    {
                        return false;
                    }
                    if (itTA->second->packed && itTA->second->byPack.pack)
                    {
                        tas.push(itTA);
                        tas.push(itTAe);
                        itTAe = itTA->second->byPack.pack->end();
                        itTA = itTA->second->byPack.pack->begin();
                    }
                    if (itTA != itTAe)
                    {
                        switch (itTP->second->type)
                        {
                            case kw_typename: {
                                TYPE** tp = change ? &itTP->second->byClass.val : &itTP->second->byClass.temp;
                                if (*tp)
                                {
                                    if (!templatecomparetypes(*tp, itTA->second->byClass.val, true))
                                    {
                                        return false;
                                    }
                                }
                                else
                                {
                                    *tp = itTA->second->byClass.val;
                                }
                                itTP->second->deduced = true;
                                break;
                            }
                            case kw_template: {
                                std::list<TEMPLATEPARAMPAIR>* paramT = itTP->first->templateParams;
                                std::list<TEMPLATEPARAMPAIR>* paramA = itTA->first->templateParams;
                                if (!paramT || !paramA || paramT->size() != paramA->size())
                                    return false;
                                auto it1 = paramT->begin();
                                auto it2 = paramA->begin();
                                for (int i = 0; i < paramT->size(); ++i, ++it1, ++it2)
                                {
                                    if (it1->second->type != it2->second->type)
                                        return false;
                                }
                                if (!DeduceFromTemplates(itTP->second->byTemplate.val->tp, itTA->second->byTemplate.val->tp, change,
                                                         byClass))
                                {
                                    return false;
                                }
                                break;
                            }
                            case kw_int: {
                                break;
                            }
                            default:
                                break;
                        }
                        ++itTP;
                        ++itTA;
                        if (itTP == itTPe && itTA == itTAe && !tps.empty() && !tas.empty())
                        {
                            itTPe = tps.top();
                            tps.pop();
                            itTP = tps.top();
                            tps.pop();
                            ++itTP;
                            itTAe = tas.top();
                            tas.pop();
                            itTA = tas.top();
                            tas.pop();
                            ++itTA;
                        }
                    }
                }
            }
            else
            {
                auto last = itTP->second->byPack.pack = templateParamPairListFactory.CreateList();
                while (!tas.empty())
                    tas.pop();
                while (itTA != itTAe)
                {
                    if (itTP->second->type != itTA->second->type)
                    {
                        return false;
                    }
                    if (itTA->second->packed)
                    {
                        tas.push(itTA);
                        tas.push(itTAe);
                        itTAe = itTA->second->byPack.pack->end();
                        itTA = itTA->second->byPack.pack->begin();
                    }
                    if (itTA != itTAe)
                    {
                        last->push_back(TEMPLATEPARAMPAIR{nullptr, itTA->second});
                        ++itTA;
                    }
                    if (itTA == itTAe && !tas.empty())
                    {
                        itTAe = tas.top();
                        tas.pop();
                        itTA = tas.top();
                        tas.pop();
                        ++itTA;
                    }
                }
                itTP = itTPe;
            }
        }
        return (itTP == itTPe && itTA == itTAe);
    }
    return false;
}
static bool DeduceFromBaseTemplates(TYPE* P, SYMBOL* A, bool change, bool byClass)
{
    if (A->sb && A->sb->baseClasses)
    {
        for (auto lst : *A->sb->baseClasses)
        {
            if (DeduceFromBaseTemplates(P, lst->cls, change, byClass))
                return true;
            if (DeduceFromTemplates(P, lst->cls->tp, change, byClass))
                return true;
        }
    }
    return false;
}
static bool DeduceFromMemberPointer(TYPE* P, TYPE* A, bool change, bool byClass)
{
    TYPE* Pb = basetype(P);
    TYPE* Ab = basetype(A);
    if (Ab->type == bt_memberptr)
    {
        if (Pb->type != bt_memberptr || !Deduce(Pb->sp->tp, Ab->sp->tp, nullptr, change, byClass, false, false))
            return false;
        if (!Deduce(Pb->btp, Ab->btp, nullptr, change, byClass, false, false))
            return false;
        return true;
    }
    else  // should only get here for functions
    {
        if (ispointer(Ab))
            Ab = basetype(Ab)->btp;
        if (!isfunction(Ab))
            return false;
        if (basetype(Ab)->sp->sb->parentClass == nullptr || !ismember(basetype(Ab)->sp) || Pb->type != bt_memberptr ||
            !Deduce(Pb->sp->tp, basetype(Ab)->sp->sb->parentClass->tp, nullptr, change, byClass, false, false))
            return false;
        if (!Deduce(Pb->btp, Ab, nullptr, change, byClass, false, false))
            return false;
        return true;
    }
}
static TYPE* FixConsts(TYPE* P, TYPE* A)
{
    int pn = 0, an = 0;
    TYPE* Pb = P;
    TYPE *q = P, **last = &q;
    int i;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        pn++;
    }
    q = A;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        an++;
    }
    *last = nullptr;
    if (pn > an)
    {
        for (i = 0; i < pn - an; i++)
            P = basetype(P)->btp;
    }
    while (P && A)
    {
        bool constant = false;
        bool vol = false;
        if (isconst(P) && !isconst(A))
            constant = true;
        if (isvolatile(P) && !isvolatile(A))
            vol = true;
        while (isconst(P) || isvolatile(P))
        {
            if ((constant && isconst(P)) || (vol && isvolatile(P)))
            {
                *last = CopyType(P);
                last = &(*last)->btp;
                *last = nullptr;
            }
            P = P->btp;
        }
        bool foundconst = false;
        bool foundvol = false;
        while (A != basetype(A))
        {
            if (A->type == bt_const && !isconst(Pb))
            {
                foundconst = true;
                *last = CopyType(A);
                last = &(*last)->btp;
                *last = nullptr;
            }
            else if (A->type == bt_volatile && !isvolatile(Pb))
            {
                *last = CopyType(A);
                last = &(*last)->btp;
                *last = nullptr;
            }
            A = A->btp;
        }
        A = basetype(A);
        *last = CopyType(A);
        last = &(*last)->btp;
        *last = nullptr;
        A = A->btp;
        P = P->btp;
    }
    (*last) = A;
    UpdateRootTypes(q);
    return q;
}
static bool TemplateConstExpr(TYPE* tp, EXPRESSION* exp)
{
    if (!isstructured(tp))
        return false;
    if (exp)
    {
        while (exp->type == en_void && exp->right)
            exp = exp->right;
        int offset;
        exp = relptr(exp, offset);
        if (exp)
        {
            return exp->v.sp->sb->constexpression;
        }
    }
    return false;
}

static bool DeduceTemplateParam(TEMPLATEPARAMPAIR* Pt, TYPE* P, TYPE* A, EXPRESSION* exp, bool change)
{
    if (Pt->second->type == kw_typename)
    {
        TYPE** tp = change ? &Pt->second->byClass.val : &Pt->second->byClass.temp;
        if (*tp)
        {
            if (/*!Pt->p->sb->initialized &&*/ !templatecomparetypes(*tp, A, true))
                return false;
            if (isconst(P))
            {
                if (isconst(*tp) || !isconst(A))
                    return false;
            }
            if (isvolatile(P))
            {
                if (!isvolatile(A))
                    return false;
            }
        }
        else
        {
            if (P)
            {
                TYPE* q = A;
                // functions are never const...   this supports the methodology for 'std::is_function'
                if (isfunction(A) && (isconst(P) || isvolatile(P)))
                    return false;
                while (q)
                {
                    if (isconst(q))
                    {
                        *tp = FixConsts(P, A);
                        Pt->second->deduced = true;
                        return true;
                    }
                    q = basetype(q)->btp;
                }
            }
            if (TemplateConstExpr(A, exp) && !isconst(P))
                *tp = MakeType(bt_const, A);
            else
                *tp = A;
            Pt->second->deduced = true;
        }
        return true;
    }
    else if (Pt->second->type == kw_template && isstructured(A) && basetype(A)->sp->sb->templateLevel)
    {
        SYMBOL* sp = basetype(A)->sp;
        std::list<TEMPLATEPARAMPAIR>::iterator itPrimary;
        std::list<TEMPLATEPARAMPAIR>::iterator itePrimary = itPrimary;
        if (Pt->second->byTemplate.args)
        {
            itPrimary = Pt->second->byTemplate.args->begin();
            itePrimary = Pt->second->byTemplate.args->end();
        }
        std::list<TEMPLATEPARAMPAIR>* matchx = sp->templateParams->front().second->bySpecialization.types
                                                  ? sp->templateParams->front().second->bySpecialization.types
                                                                                 : sp->templateParams;
        std::list<TEMPLATEPARAMPAIR>::iterator itMatch;
        std::list<TEMPLATEPARAMPAIR>::iterator iteMatch = itMatch;
        if (matchx)
        {
            itMatch = matchx->begin();
            iteMatch = matchx->end();
            if (itMatch->second->type == kw_new)
                ++itMatch;
        }
        for ( ; itPrimary != itePrimary && itMatch != iteMatch; ++itPrimary, ++itMatch)
        {
            if (itPrimary->second->type != itMatch->second->type)
                return false;
            if (itPrimary->second->packed)
            {
                if (itMatch->second->packed)
                {
                    itPrimary->second->byPack.pack = itMatch->second->byPack.pack;
                }
                else
                {
                    itPrimary->second->byPack.pack = templateParamPairListFactory.CreateList();
                    itPrimary->second->byPack.pack->insert(itPrimary->second->byPack.pack->begin(),itMatch, iteMatch);
                    itMatch = iteMatch;
                    ++itPrimary;
                    break;
                }
            }
            else if (!DeduceTemplateParam(&*itPrimary, itPrimary->second->byClass.val, itMatch->second->byClass.val, nullptr, change))
                return false;
        }
        if (itPrimary != itePrimary && itPrimary->second->packed && itMatch == iteMatch)
        {
            itPrimary = itePrimary;
        }
        if (itPrimary == itePrimary && itMatch == iteMatch)
        {
            SYMBOL** sp = change ? &Pt->second->byTemplate.val : &Pt->second->byTemplate.temp;
            Pt->second->deduced = true;
            *sp = basetype(A)->sp;
            sp = change ? &Pt->second->byTemplate.orig->second->byTemplate.val : &Pt->second->byTemplate.orig->second->byTemplate.temp;
            Pt->second->byTemplate.orig->second->deduced = true;
            *sp = basetype(A)->sp;
            return true;
        }
    }
    return false;
}
static bool Deduce(TYPE* P, TYPE* A, EXPRESSION* exp, bool change, bool byClass, bool allowSelectors, bool baseClasses)
{
    TYPE *Pin = P, *Ain = A;
    if (!P || !A)
        return false;
    while (1)
    {
        TYPE* Ab = basetype(A);
        TYPE* Pb = basetype(P);
        if (isref(Ab))
            Ab = basetype(Ab->btp);
        if (isstructured(Pb) && Pb->sp->sb->templateLevel && Pb->sp->sb->attribs.inheritable.linkage4 != lk_virtual &&
            isstructured(Ab))
        {
            if (DeduceFromTemplates(P, Ab, change, byClass))
                return true;
            else
                return DeduceFromBaseTemplates(P, basetype(Ab)->sp, change, byClass);
        }
        if (Pb->type == bt_memberptr)
        {
            return DeduceFromMemberPointer(P, Ab, change, byClass);
        }
        Ab = basetype(A);
        Pb = basetype(P);
        if (Pb->type == bt_enum)
        {
            if (Ab->type == bt_enum && Ab->sp == Pb->sp)
                return true;
            if (isint(Ab))  // && Ab->enumConst)
                return true;
            return false;
        }
        if (isfunction(Ab) && isfuncptr(Pb))
        {
            Pb = basetype(Pb)->btp;
            Pin = basetype(Pin);  // assume any qualifiers went with an enclosing ref
            // can't have a const qualified non-member function anyway I think...
        }
        if (isref(Ab) && !isref(Pb))
            Ab = basetype(Ab->btp);
        if (Ab->type != Pb->type && (!isfunction(Ab) || !isfunction(Pb)) && Pb->type != bt_templateparam &&
            (!allowSelectors || Pb->type != bt_templateselector))
            // this next allows long and int to be considered the same, on architectures where there is no size difference
            if (!isint(Ab) || !isint(Pb) || basetype(Ab)->type == bt_bool || basetype(Pb)->type == bt_bool ||
                isunsigned(Ab) != isunsigned(Pb) || getSize(basetype(Ab)->type) != getSize(basetype(Pb)->type))
                return false;
        switch (Pb->type)
        {
            case bt_pointer:
                if (isarray(Pb))
                {
                    if (!!basetype(Pb)->esize != !!basetype(Ab)->esize)
                        return false;
                    if (basetype(Pb)->esize && basetype(Pb)->esize->type == en_templateparam)
                    {
                        SYMBOL* sym = basetype(Pb)->esize->v.sp;
                        if (sym->tp->type == bt_templateparam)
                        {
                            sym->tp->templateParam->second->byNonType.val = basetype(Ab)->esize;
                        }
                    }
                }
                if (isarray(Pb) != isarray(Ab))
                    return false;
                P = Pb->btp;
                A = Ab->btp;
                break;
            case bt_templateselector:
                return true;
            case bt_templatedecltype:
                return false;
            case bt_rref:
            case bt_lref:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
                P = Pb->btp;
                A = Ab->btp;
                break;
            case bt_func:
            case bt_ifunc: {
                auto itp = Pb->syms->begin();
                auto itpend = Pb->syms->end();
                auto ita = Ab->syms->begin();
                auto itaend = Ab->syms->end();
                if (islrqual(Pin) != islrqual(A) || isrrqual(Pin) != isrrqual(Ain))
                    return false;
                if (isconst(Pin) != isconst(Ain) || isvolatile(Pin) != isvolatile(Ain))
                    return false;
                if (itp != itpend && (*itp)->sb->thisPtr)
                    ++itp;
                if (ita != itaend && (*ita)->sb->thisPtr)
                    ++ita;
                clearoutDeduction(P);
                if (Pb->btp->type != bt_auto && !Deduce(Pb->btp, Ab->btp, nullptr, change, byClass, allowSelectors, baseClasses))
                    return false;
                while (ita != itaend && itp != itpend)
                {
                    SYMBOL* sp = *itp;
                    if (!Deduce(sp->tp, (*ita)->tp, nullptr, change, byClass, allowSelectors, baseClasses))
                        return false;
                    if (sp->tp->type == bt_templateparam)
                    {
                        if (sp->tp->templateParam->second->packed)
                        {
                            auto sra = Ab->syms->back();
                            auto srp = Pb->syms->back();
                            while ((*ita) != sra)
                                ++ita;
                            while ((*itp) != srp)
                                ++itp;
                            if ((sra->tp->type != bt_ellipse && srp->tp->type != bt_ellipse) || sra->tp->type == srp->tp->type)
                            {
                                itp = itpend;
                                ita = itaend;
                            }
                            break;
                        }
                    }
                    ++itp;
                    ++ita;
                }
                if (ita != itaend)
                    return false;
                if (itp != itpend && !(*itp)->sb->init)
                    return false;
                return true;
            }
            case bt_templateparam:
                return DeduceTemplateParam(Pb->templateParam, P, A, exp, change);
            case bt_struct:
            case bt_union:
            case bt_class:
                if (templatecomparetypes(Pb, Ab, true))
                    return true;
                if (baseClasses && classRefCount(Pb->sp, Ab->sp) == 1)
                {
                    return true;
                }
                {
                    SYMBOL* cons = search(basetype(Pb)->syms, overloadNameTab[CI_CONSTRUCTOR]);
                    if (cons)
                    {
                        for (auto sp : *cons->tp->syms)
                        {
                            if (sp->tp->syms->size() == 2)
                            {
                                auto it = sp->tp->syms->begin();
                                ++it;
                                if (comparetypes((*it)->tp, Ab, true) || sameTemplate((*it)->tp, Ab))
                                    return true;
                            }
                        }
                    }
                }
                return false;
            default:

                return true;
        }
    }
}
static int eval(EXPRESSION* exp)
{
    optimize_for_constants(&exp);
    if (IsConstantExpression(exp, false, false))
        return exp->v.i;
    return 0;
}
static bool ValidExp(EXPRESSION** exp_in)
{
    bool rv = true;
    EXPRESSION* exp = *exp_in;
    if (exp->type == en_templateselector)
        return false;
    if (exp->left)
        rv &= ValidExp(&exp->left);
    if (exp->right)
        rv &= ValidExp(&exp->right);
    if (exp->type == en_templateparam)
        if (!exp->v.sp->templateParams || !exp->v.sp->templateParams->front().second->byClass.val)
            return false;
    return rv;
}
static bool ValidArg(TYPE* tp)
{
    while (1)
    {
        switch (tp->type)
        {
            case bt_pointer:
                if (isarray(tp))
                {
                    while (isarray(tp))
                    {
                        tp = basetype(tp)->btp;
                        if (tp->etype)
                        {
                            int n = eval(tp->esize);
                            if (n <= 0)
                                return false;
                        }
                    }
                    if (tp->type == bt_templateparam)
                    {
                        if (tp->templateParam->second->type != kw_typename)
                            return false;
                        tp = tp->templateParam->second->byClass.val;
                        if (!tp)
                            return false;
                    }
                    if (tp->type == bt_void || isfunction(tp) || isref(tp) ||
                        (isstructured(tp) && basetype(tp)->sp->sb->isabstract))
                        return false;
                }
                if (ispointer(tp))
                {
                    while (ispointer(tp))
                        tp = tp->btp;
                    if (tp->type == bt_templateparam)
                    {
                        if (tp->templateParam->second->type != kw_typename)
                            return false;
                        return ValidArg(tp);
                    }
                    else if (tp->type == bt_templateselector)
                    {
                        return ValidArg(tp);
                    }
                    if (isref(tp))
                        return false;
                }
                return true;
            case bt_templatedecltype:
                tp = TemplateLookupTypeFromDeclType(tp);
                return !!tp;
                break;
            case bt_templateselector: {
                SYMBOL* ts = (*tp->sp->sb->templateSelector)[1].sp;
                SYMBOL* sp = nullptr;
                auto find = tp->sp->sb->templateSelector->begin();
                auto finde = tp->sp->sb->templateSelector->end();
                if ((*tp->sp->sb->templateSelector)[1].isDeclType)
                {
                    TYPE* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
                    if (!tp1 || tp1->type == bt_any || !isstructured(tp1))
                        return false;
                    sp = basetype(tp1)->sp;
                }
                else if ((*tp->sp->sb->templateSelector)[1].isTemplate)
                {
                    std::list<TEMPLATEPARAMPAIR>* current = templateParamPairListFactory.CreateList();
                    *current = *(*tp->sp->sb->templateSelector)[1].templateParams;
                    if (ts->tp->type == bt_typedef)
                    {
                        sp = GetTypeAliasSpecialization(ts, current);
                    }
                    else
                    {
                        sp = GetClassTemplate(ts, current, false);
                    }
                    tp = nullptr;
                }
                else if (basetype(ts->tp)->templateParam->second->type == kw_typename)
                {
                    tp = basetype(ts->tp)->templateParam->second->byClass.val;
                    if (!tp)
                        return false;
                    sp = basetype(tp)->sp;
                }
                else if (basetype(ts->tp)->templateParam->second->type == kw_delete)
                {
                    std::list<TEMPLATEPARAMPAIR>* args = basetype(ts->tp)->templateParam->second->byDeferred.args;
                    std::list<TEMPLATEPARAMPAIR>* val = nullptr;
                    sp = tp->templateParam->first;
                    sp = TemplateClassInstantiateInternal(sp, args, true);
                }
                if (sp)
                {
                    sp->tp = PerformDeferredInitialization(sp->tp, nullptr);
                    sp = basetype(sp->tp)->sp;
                    ++find;
                    ++find;
                    for ( ; find != finde && sp ; ++find)
                    {
                        SYMBOL* spo = sp;
                        if (!isstructured(spo->tp))
                            break;

                        sp = search(spo->tp->syms, find->name);
                        if (!sp)
                        {
                            sp = classdata(find->name, spo, nullptr, false, false);
                            if (sp == (SYMBOL*)-1)
                                sp = nullptr;
                        }
                        if (sp && sp->sb->access != ac_public && !resolvingStructDeclarations)
                        {
                            sp = nullptr;
                            break;
                        }
                    }
                    return find == finde && sp && istype(sp);
                }
                return false;
            }
            case bt_lref:
            case bt_rref:
                tp = basetype(tp)->btp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->second->type != kw_typename)
                        return false;
                    return ValidArg(tp);
                }
                if (!tp || isref(tp))
                    return false;
                break;
            case bt_memberptr: {
                TYPE* tp1 = tp->sp->tp;
                if (tp1->type == bt_templateparam)
                {
                    if (tp1->templateParam->second->type != kw_typename)
                        return false;
                    tp1 = tp1->templateParam->second->byClass.val;
                    if (!tp1)
                        return false;
                }
                if (!isstructured(tp1))
                    return false;
            }
                tp = tp->btp;
                break;
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_func:
            case bt_ifunc: {
                for (auto sp : *tp->syms)
                {
                    if (!ValidArg(sp->tp))
                        return false;
                }
                tp = tp->btp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->second->type != kw_typename)
                        return false;
                    tp = tp->templateParam->second->byClass.val;
                    if (!tp)
                        return false;
                }
                if (isfunction(tp) || isarray(tp) || (isstructured(tp) && basetype(tp)->sp->sb->isabstract))
                    return false;
                break;
            }
            case bt_templateparam:
                if (tp->templateParam->second->type == kw_template)
                {
                    std::list<TEMPLATEPARAMPAIR>* tpx;
                    if (tp->templateParam->second->packed)
                        return true;
                    if (tp->templateParam->second->byTemplate.val == nullptr)
                        return false;
                    if (tp->templateParam->second->byTemplate.args)
                    {
                        for (auto&& tpx : *tp->templateParam->second->byTemplate.args)
                        {
                            if (tpx.second->type == kw_typename)
                            {
                                if (tpx.second->packed && tpx.second->byPack.pack)
                                {
                                    // this should be recursive...
                                    for (auto&& tpl1 : *tpx.second->byPack.pack)
                                    {
                                        if (tpl1.second->type == kw_typename && !tpl1.second->packed)
                                        {
                                            if (!ValidArg(tpl1.second->byClass.val))
                                                return false;
                                        }
                                    }
                                }
                                else if (!ValidArg(tpx.second->byClass.val))
                                    return false;
                            }
                            // this really should check nested templates...
                        }
                    }
                }
                else
                {
                    if (tp->templateParam->second->type != kw_typename)
                        return false;
                    if (tp->templateParam->second->packed)
                        return true;
                    if (tp->templateParam->second->byClass.val == nullptr)
                        return false;
                    if (tp->templateParam->second->byClass.val->type == bt_void)
                        return false;
                    if (tp->templateParam->second->byClass.val == tp)  // error catcher
                        return false;
                    return ValidArg(tp->templateParam->second->byClass.val);
                }
            default:
                return true;
        }
    }
}
static bool valFromDefault(std::list<TEMPLATEPARAMPAIR>* params, bool usesParams, std::list<INITLIST*>* args)
{
    if (params)
    {
        for (auto&& param : *params)
        {
            if (usesParams || !(args && args->size()))
                break;
            if (param.second->type != kw_new)
            {
                if (param.second->packed)
                {
                    if (!valFromDefault(param.second->byPack.pack, usesParams, args))
                        return false;
                }
                else
                {
                    if (!param.second->byClass.val)
                        param.second->byClass.val = param.second->byClass.dflt;
                    if (!param.second->byClass.val)
                        return false;
                    if (args && args->size())
                        args->pop_front();
                }
            }
        }
    }
    return true;
}
static void FillNontypeTemplateParamDefaults(std::list<TEMPLATEPARAMPAIR>* fill, std::list<TEMPLATEPARAMPAIR>* enclosing);
static void FillNontypeExpressionDefaults(EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    if (exp->left)
        FillNontypeExpressionDefaults(exp->left, enclosing);
    if (exp->right)
        FillNontypeExpressionDefaults(exp->right, enclosing);
    if (exp->v.templateParam)
    {
        std::list<TEMPLATEPARAMPAIR> a{*exp->v.templateParam};
        FillNontypeTemplateParamDefaults(&a, enclosing);
    }
    if (exp->v.templateSelector)
    {
        for (auto&& t : *exp->v.templateSelector)
        {
            if (t.templateParams)
                FillNontypeTemplateParamDefaults(t.templateParams, enclosing);
        }
    }
}
static void FillNontypeTypeDefaults(TYPE* tp, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    if (tp->type == bt_templateselector)
    {
        for (auto&& t : *tp->sp->sb->templateSelector)
        {
            if (t.templateParams)
                FillNontypeTemplateParamDefaults(t.templateParams, enclosing);
        }
    }
    else
    {
        std::list<TEMPLATEPARAMPAIR> a{*tp->templateParam};
        FillNontypeTemplateParamDefaults(&a, enclosing);
    }
}
static bool SetTemplateParamValue(TEMPLATEPARAMPAIR* p, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    if (p->first && enclosing)
    {
        for (auto&& enc : *enclosing)
        {
            if (enc.first && enc.second->type != kw_new)
                if (!strcmp(p->first->name, enc.first->name))
                {
                    if (p->second->packed)
                    {
                        if (enc.second->packed)
                        {
                            p->second->byPack.pack = enc.second->byPack.pack;
                        }
                        else
                        {
                            p->second->byPack.pack = templateParamPairListFactory.CreateList();
                            p->second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            *p->second->byPack.pack->back().second = *enc.second;
                        }
                    }
                    else
                    {
                        if (enc.second->byClass.val)
                            p->second->byClass.val = enc.second->byClass.val;
                        else
                            p->second->byClass.val = enc.second->byClass.dflt;
                    }
                    return true;
                }
        }
    }
    return false;
}
static void FillNontypeTemplateParamDefaults(std::list<TEMPLATEPARAMPAIR>* fills, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    if (fills)
    {
        for (auto&& fill : *fills)
        {
            if (fill.second->packed)
            {
                if (!SetTemplateParamValue(&fill, enclosing))
                    FillNontypeTemplateParamDefaults(fill.second->byPack.pack, enclosing);
            }
            else if (fill.second->type == kw_int)
            {
                if (fill.second->byNonType.dflt)
                    FillNontypeExpressionDefaults(fill.second->byNonType.dflt, enclosing);
                else
                    SetTemplateParamValue(&fill, enclosing);
            }
            else if (fill.second->type == kw_typename)
            {
                if (!fill.second->byClass.dflt)
                {
                    SetTemplateParamValue(&fill, enclosing);
                }
                else
                {
                    switch (fill.second->byClass.dflt->type)
                    {
                        case bt_class:
                            FillNontypeTemplateParamDefaults(fill.second->byClass.dflt->sp->templateParams, enclosing);
                            break;
                        case bt_templateparam:
                            SetTemplateParamValue(fill.second->byClass.dflt->templateParam, enclosing);
                            break;
                        case bt_templateselector:
                            FillNontypeTemplateParamDefaults(
                                (*fill.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams, enclosing);
                            break;
                    }
                }
            }
        }
    }
}
static bool checkNonTypeTypes(std::list<TEMPLATEPARAMPAIR>* params, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    if (params)
    {
        for (auto&& param : *params)
        {
            if (param.second->type == kw_int && (param.second->byNonType.tp->type == bt_templateselector ||
                                                   param.second->byNonType.tp->type == bt_templateparam))
            {
                FillNontypeTypeDefaults(param.second->byNonType.tp, enclosing);
                TYPE* tp1 = SynthesizeType(param.second->byNonType.tp, enclosing, false);
                if (!tp1 || tp1->type == bt_any || isstructured(tp1) || isref(tp1) || isfloat(tp1) || iscomplex(tp1) ||
                    isimaginary(tp1))
                    return false;
            }
        }
    }
    return true;
}
static SYMBOL* ValidateArgsSpecified(std::list<TEMPLATEPARAMPAIR>* params, SYMBOL* func, std::list<INITLIST*>* args, std::list<TEMPLATEPARAMPAIR>* nparams)
{
    bool usesParams = !!args && args->size();
    auto it = basetype(func->tp)->syms->begin();
    auto ite = basetype(func->tp)->syms->end();
    STRUCTSYM s, s1;
    if (func->sb->isConstructor && args)
    {
        // get rid of potential constructor calls
        // that would involve a by_val template the same class as the constructor is in
        auto it1 = it;
        if (it1 != it && (*it1)->sb->thisPtr)
            ++it1;
        auto ita = args->begin();
        auto itae = args->end();
        while (it1 != ite && ita != itae)
        {
            if ((*ita)->tp && isstructured((*ita)->tp) && basetype((*ita)->tp)->sp->sb->templateLevel && sameTemplate(func->sb->parentClass->tp, (*ita)->tp, true))
                return nullptr;
            ++it1;
            ++ita;
        }

    }
    inDefaultParam++;
    std::list<INITLIST*> arg1;
    if (args)
        arg1 = *args;
    if (!valFromDefault(params, usesParams, &arg1))
    {
        inDefaultParam--;
        return nullptr;
    }
    if (!checkNonTypeTypes(params, params))
    {    
        inDefaultParam--;
        return nullptr;
    }

    if (params)
    {
        for (auto&& param : *params)
        {
            if (param.second->type == kw_typename || param.second->type == kw_template || param.second->type == kw_int)
                if (!param.second->packed && !param.second->byClass.val)
                {
                    inDefaultParam--;
                    return nullptr;
                }
        }
    }
    if (it != ite && (*it)->tp->type == bt_void)
    {
        inDefaultParam--;
        return SynthesizeResult(func, nparams);
    }
    if (it != ite)
    {
        bool packedOrEllipse = false;
        if (func->sb->parentClass)
        {
            s1.str = func->sb->parentClass;
            addStructureDeclaration(&s1);
        }
        s.tmpl = func->templateParams;
        addTemplateDeclaration(&s);
        std::list<INITLIST*>::iterator ita, itae;
        if (args)
        {
            ita = args->begin();
            itae = args->end();
        }
        while (ita != itae && it != ite)
        {
            SYMBOL* sp1 = *it;
            if (basetype(sp1->tp)->type == bt_ellipse || sp1->packed)
                packedOrEllipse = true;
            ++ita;
            ++it;
        }
        if (ita != itae && !packedOrEllipse)
        {
            dropStructureDeclaration();
            if (func->sb->parentClass)
                dropStructureDeclaration();
            inDefaultParam--;
            return nullptr;
        }
        while (it != ite)
        {
            SYMBOL* sp = *it;
            if (sp->sb->deferredCompile)
            {
                LEXLIST* lex;
                dontRegisterTemplate += templateNestingCount != 0;
                lex = SetAlternateLex(sp->sb->deferredCompile);
                sp->sb->init = nullptr;
                lex = initialize(lex, func, sp, sc_parameter, true, false, _F_TEMPLATEARGEXPANSION);
                SetAlternateLex(nullptr);
                dontRegisterTemplate -= templateNestingCount != 0;
                if (sp->sb->init && sp->sb->init->front()->exp && !ValidExp(&sp->sb->init->front()->exp))
                {
                    dropStructureDeclaration();
                    if (func->sb->parentClass)
                        dropStructureDeclaration();
                    inDefaultParam--;
                    return nullptr;
                }
            }
            ++it;
        }
        dropStructureDeclaration();
        if (func->sb->parentClass)
            dropStructureDeclaration();
    }
    s.tmpl = func->templateParams;
    addTemplateDeclaration(&s);
    std::list<INITLIST*>::iterator check, checke;
    if (args)
    {
        check = args->begin();
        checke = args->end();
    }
    for (auto sp1 : *basetype(func->tp)->syms)
    {
        if (!ValidArg(sp1->tp))
        {
            dropStructureDeclaration();
            inDefaultParam--;
            return nullptr;
        }
        if (args && check != checke)
            ++check;
    }
    if (isstructured(basetype(func->tp)->btp))
    {
        std::list<TEMPLATEPARAMPAIR>* params = basetype(basetype(func->tp)->btp)->sp->templateParams;
        if (params)
        {
            auto itparams = params->begin();
            auto iteparams = params->end();
            auto itspecial = itparams->second->bySpecialization.types ? itparams->second->bySpecialization.types->begin() : itparams;
            auto itespecial = itparams->second->bySpecialization.types ? itparams->second->bySpecialization.types->end() : iteparams;
            for (;itspecial != itespecial; ++itspecial)
            {
                if (itspecial->second->type != kw_new)
                {
                    std::list<TEMPLATEPARAMPAIR> a(itspecial, itespecial);
                    TransferClassTemplates(func->templateParams, func->templateParams, &a);
                }
            }
        }
    }
    std::vector<char> hold;
    std::vector<TYPE**> tav;
    std::vector<TYPE*> tpp;
    if (nparams)
    {
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
        auto ittpl = nparams->begin();
        auto itetpl = nparams->end();
        for ( ; ittpl != itetpl ;)
        {
            if (ittpl->second->packed && ittpl->second->byPack.pack)
            {
                tas.push(ittpl);
                tas.push(itetpl);
                itetpl = ittpl->second->byPack.pack->end();
                ittpl = ittpl->second->byPack.pack->begin();
            }
            if (ittpl != itetpl)
            {
                if (ittpl->second->type == kw_typename)
                {
                    if (ittpl->second->byClass.val)
                    {
                        tav.push_back(&ittpl->second->byClass.val);
                        tpp.push_back(ittpl->second->byClass.val);
                        hold.push_back(ittpl->second->byClass.val->lref);
                        hold.push_back(ittpl->second->byClass.val->rref);
                        ittpl->second->byClass.val->lref = false;
                        ittpl->second->byClass.val->rref = false;
                    }
                }
                else if (ittpl->second->type == kw_int)
                {
                    if (ittpl->second->byClass.val)
                    {
                        tav.push_back(&ittpl->second->byNonType.tp);
                        tpp.push_back(ittpl->second->byNonType.tp);
                        hold.push_back(ittpl->second->byNonType.tp->lref);
                        hold.push_back(ittpl->second->byNonType.tp->rref);
                        ittpl->second->byNonType.tp->lref = false;
                        ittpl->second->byNonType.tp->rref = false;
                    }
                }
                ++ittpl;
            }
            if (ittpl == itetpl && !tas.empty())
            {
                itetpl = tas.top();
                tas.pop();
                ittpl = tas.top();
                tas.pop();
                ++ittpl;
            }
        }
    }
    SYMBOL* rv = SynthesizeResult(func, nparams);
    for (int i = 0; i < tpp.size(); i++)
    {
        TYPE* tp = tpp[i];
        tp->lref = hold[i * 2];
        tp->rref = hold[i * 2 + 1];
    }
    rv->sb->maintemplate = func;
    if (!ValidArg(basetype(func->tp)->btp))
    {
        dropStructureDeclaration();
        inDefaultParam--;
        return nullptr;
    }
    for (int i = 0; i < tpp.size(); i++)
    {
        TYPE* tp = tpp[i];
        *(tav[i]) = tp;
    }
    dropStructureDeclaration();
    inDefaultParam--;
    return rv;
}
static TYPE* GetForwardType(TYPE* P, TYPE* A, EXPRESSION* exp)
{
    bool lref = false;
    bool rref = false;
    GetRefs(P, A, exp, lref, rref);
    if (rref)
    {
        // rvalue to rref, remove reference...
        if (isref(A))
            A = basetype(A)->btp;
    }
    else if (lref)
    {
        // lvalue to rref, result is lvalue&...
        if (basetype(A)->type != bt_lref)
        {
            A = MakeType(bt_lref, A);
        }
    }
    return A;
}
static bool TemplateDeduceFromArg(TYPE* orig, TYPE* sym, EXPRESSION* exp, bool allowSelectors, bool baseClasses)
{
    TYPE *P = orig, *A = sym;
    if (!isref(P))
    {
        A = rewriteNonRef(A);
        A = RemoveCVQuals(A);
    }
    P = RemoveCVQuals(P);
    if (isref(P))
    {
        int type = basetype(P)->type;
        P = basetype(P)->btp;
        if (type == bt_rref)
        {
            if (exp && !isconst(P) && !isvolatile(P))
            {
                // unadorned rref in arg, forwarding...
                A = GetForwardType(P, A, exp);
            }
            else if (isref(A))
                A = basetype(A)->btp;
        }
        else
        {
            if (isref(A))
                A = basetype(A)->btp;
        }
    }
    if (ispointer(P) && (isint(A) || (ispointer(A) && A->nullptrType)) && exp && isconstzero(A, exp))
    {
        // might get in here with a non-template argument that needs to be matched
        // usually the two types would just match and it would be fine
        // but in the case where we have pointer and a 'zero' constant
        // we need to act specially
        while (ispointer(P))
            P = basetype(P)->btp;
        if (isvoid(P) || isarithmetic(P) || isfunction(P) ||
            (isstructured(P) && (!basetype(P)->sp->sb->templateLevel || basetype(P)->sp->sb->instantiated)))
            return true;
        return false;
    }
    if (Deduce(P, A, exp, true, false, allowSelectors, baseClasses))
    {
        return true;
    }
    if (isstructured(P) && !isstructured(A))
    {
        // this is basical a poor man's way to do GetOverloadedFunction on a constructor,
        // with an arithmetic or pointer arg
        SYMBOL* cons = search(basetype(P)->syms, overloadNameTab[CI_CONSTRUCTOR]);
        if (cons)
        {
            for (auto sp1 : *basetype(cons->tp)->syms)
            {
                if (basetype(sp1->tp)->syms->front()->sb->thisPtr)
                {
                    if (basetype(sp1->tp)->syms->size() == 2)
                    {
                        if (comparetypes(basetype(sp1->tp)->syms->back()->tp, A, true))
                            return true;
                    }
                    else if (basetype(sp1->tp)->syms->size() > 2)
                    {
                        auto it = basetype(sp1->tp)->syms->begin();
                        ++it;
                        ++it;
                        if ((*it)->sb->defaulted)
                        {
                            --it;
                            if (comparetypes((*it)->tp, A, true))
                                return true;
                        }
                    }
                }
            }
        }
    }
    if (isfuncptr(P) || (isref(P) && isfunction(basetype(P)->btp)))
    {
        if (exp->type == en_func)
        {
            if (exp->v.func->sp->sb->storage_class == sc_overloads)
            {
                SYMBOL* candidate = nullptr;
                for (auto sym : *basetype(exp->v.func->sp->tp)->syms)
                {
                    if (sym->sb->templateLevel)
                        return false;
                }
                // no templates, we can try each function one at a time
                for (auto sym : *basetype(exp->v.func->sp->tp)->syms)
                {
                    clearoutDeduction(P);
                    if (Deduce(P->btp, sym->tp, nullptr, false, false, allowSelectors, baseClasses))
                    {
                        if (candidate)
                            return false;
                        else
                            candidate = sym;
                    }
                }
                if (candidate)
                    return Deduce(P, candidate->tp, nullptr, true, false, allowSelectors, baseClasses);
            }
        }
    }
    return false;
}
void NormalizePacked(TYPE* tpo)
{
    TYPE* tp = tpo;
    while (isref(tp) || ispointer(tp))
        tp = basetype(tp)->btp;
    if (basetype(tp)->templateParam)
        tpo->templateParam = basetype(tp)->templateParam;
}
static bool TemplateDeduceArgList(SymbolTable<SYMBOL>::iterator funcArgs, SymbolTable<SYMBOL>::iterator funcArgsEnd, SymbolTable<SYMBOL>::iterator templateArgs, SymbolTable<SYMBOL>::iterator templateArgsEnd,std::list<INITLIST*>::iterator its,std::list<INITLIST*>::iterator itse, bool allowSelectors,
                                  bool baseClasses)
{
    bool rv = true;
    while (templateArgs != templateArgsEnd && its != itse)
    {
        SYMBOL* sp = *templateArgs;
        if (sp->packed)
        {
            NormalizePacked(sp->tp);
            if (sp->tp->templateParam && sp->tp->templateParam->second->packed)
            {
                auto itparams = sp->tp->templateParam->second->byPack.pack->begin();
                auto iteparams = sp->tp->templateParam->second->byPack.pack->end();
                for ( ; itparams != iteparams && its != itse; ++itparams, ++its)
                {
                    if (!TemplateDeduceFromArg(itparams->second->byClass.val, (*its)->tp, (*its)->exp, allowSelectors, baseClasses))
                    {
                        rv = false;
                    }
                    if (funcArgs != funcArgsEnd)
                        ++funcArgs;
                }
            }
            else
            {
                ++its;
                if (funcArgs != funcArgsEnd)
                    ++funcArgs;
            }
        }
        else if (((*its)->nested || (!(*its)->tp && !(*its)->exp)) && funcArgs != funcArgsEnd)
        {
            if ((*its)->nested && isstructured(sp->tp) && basetype(sp->tp)->sp->sb->templateLevel &&
                basetype(sp->tp)->sp->sb->initializer_list)
            {
                if (basetype(sp->tp)->sp->templateParams->size() > 1)
                {
                    auto ittpx = basetype(sp->tp)->sp->templateParams->begin();
                    ++ittpx;
                    if (!TemplateDeduceFromArg(ittpx->first->tp, (*its)->nested->front()->tp,
                                               (*its)->nested->front()->exp, allowSelectors, baseClasses))
                        rv = false;
                }
            }
            ++its;
            if (funcArgs != funcArgsEnd)
                ++funcArgs;
        }
        else
        {
            if (!TemplateDeduceFromArg(sp->tp, (*its)->tp, (*its)->exp, allowSelectors, baseClasses))
                rv = false;
            ++its;
            if (funcArgs != funcArgsEnd)
                ++funcArgs;
        }
        ++templateArgs;
    }
    return rv && (its == itse || ((*its)->tp && (*its)->tp->type == bt_templateparam && (*its)->tp->templateParam->second->packed &&
                               !(*its)->tp->templateParam->second->byPack.pack));
}
bool TemplateParseDefaultArgs(SYMBOL* declareSym, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* dest, std::list<TEMPLATEPARAMPAIR>* src,
                              std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    std::list<TEMPLATEPARAMPAIR>* defaults = nullptr;
    Optimizer::LIST* oldOpenStructs = openStructs;
    int oldStructLevel = structLevel;
    STRUCTSYM s, primary;
    LEXLIST* head = nullptr;
    LEXLIST* tail = nullptr;
    SYMBOL* oldMemberClass = instantiatingMemberFuncClass;
    memset(&primary, 0, sizeof(primary));
    std::list<TEMPLATEPARAMPAIR>::iterator itPrimary, itePrimary = itPrimary;
    if (declareSym->sb->specialized && declareSym->sb->parentTemplate &&
        !declareSym->sb->parentTemplate->templateParams->front().second->bySpecialization.types)
    {
        itPrimary = declareSym->sb->parentTemplate->templateParams->begin();
        itePrimary = declareSym->sb->parentTemplate->templateParams->end();
        ++itPrimary;
    }

    instantiatingMemberFuncClass = declareSym->sb->parentClass;
    if (currents)
    {
        head = currents->bodyHead;
        tail = currents->bodyTail;
    }
    s.tmpl = enclosing;
    addTemplateDeclaration(&s);
    parsingDefaultTemplateArgs++;

    std::list<TEMPLATEPARAMPAIR>::iterator itSrc, iteSrc = itSrc;
    std::list<TEMPLATEPARAMPAIR>::iterator itDest, iteDest = itDest;
    std::list<TEMPLATEPARAMPAIR>::iterator itArgs, iteArgs = itArgs;
    if (src)
    {
        itSrc = src->begin();
        iteSrc = src->end();
        if (itSrc->second->type == kw_new)
            ++itSrc;
    }
    if (dest)
    {
        itDest = dest->begin();
        iteDest = dest->end();
        if (itDest->second->type == kw_new)
            ++itDest;
    }
    if (args)
    {
        itArgs = args->begin();
        iteArgs = args->end();
        if (itArgs != args->end() && itArgs->second->type == kw_new)
            ++itArgs;
    }
    for (; itSrc != iteSrc && itDest != iteDest; ++itSrc, ++itDest)
    {
        if ((!args || itArgs == iteArgs) && !itDest->second->byClass.val && !itDest->second->packed &&
            (itPrimary == itePrimary || !itPrimary->second->packed))
        {
            LEXLIST* lex;
            int n, pushCount;
            if (!itSrc->second->byClass.txtdflt)
            {
                parsingDefaultTemplateArgs--;
                dropStructureDeclaration();
                instantiatingMemberFuncClass = oldMemberClass;
                return false;
            }
            n = PushTemplateNamespace(declareSym);
            if (itPrimary != itePrimary && itPrimary->second->byClass.txtdflt &&
                itPrimary->second->byClass.txtdflt == itSrc->second->byClass.txtdflt)
            {
                if (!defaults)
                {
                    defaults = templateParamPairListFactory.CreateList();

                    std::list<TEMPLATEPARAMPAIR>* one = declareSym->sb->parentTemplate->templateParams;
                    std::list<TEMPLATEPARAMPAIR>* two = declareSym->templateParams->front().second->bySpecialization.types;
                    auto itone = one->begin();
                    auto iteone = one->end();
                    auto ittwo = two->begin();
                    auto itetwo = two->end();
                    for (++itone; itone != iteone && ittwo != itetwo; ++itone, ++ittwo)
                        defaults->push_back(TEMPLATEPARAMPAIR{itone->first, ittwo->second});
                }
                primary.tmpl = defaults;
                addTemplateDeclaration(&primary);
                pushCount = 1;
            }
            else
            {
                pushCount = pushContext(declareSym, false);
            }
            itDest->second->byClass.txtdflt = itSrc->second->byClass.txtdflt;
            itDest->second->byClass.txtargs = itSrc->second->byClass.txtargs;
            itDest->second->byNonType.txttype = itSrc->second->byNonType.txttype;
            lex = SetAlternateLex(itSrc->second->byClass.txtdflt);
            switch (itDest->second->type)
            {
                case kw_typename: {
                    noTypeNameError++;
                    lex = get_type_id(lex, &itDest->second->byClass.val, nullptr, sc_cast, false, true, false);
                    noTypeNameError--;
                    if (!itDest->second->byClass.val || itDest->second->byClass.val->type == bt_any ||
                        (!templateNestingCount && itDest->second->byClass.val->type == bt_templateselector))
                    {
                        parsingDefaultTemplateArgs--;
                        while (pushCount--)
                            dropStructureDeclaration();
                        PopTemplateNamespace(n);
                        SetAlternateLex(nullptr);
                        dropStructureDeclaration();
                        instantiatingMemberFuncClass = oldMemberClass;
                        return false;
                    }
                    break;
                }
                case kw_template: {
                    char buf[256];
                    strcpy(buf, lex->data->value.s.a);
                    lex = id_expression(lex, nullptr, &itDest->second->byTemplate.val, nullptr, nullptr, nullptr, false, false, buf);

                    if (!itDest->second->byTemplate.val)
                    {
                        parsingDefaultTemplateArgs--;
                        while (pushCount--)
                            dropStructureDeclaration();
                        PopTemplateNamespace(n);
                        SetAlternateLex(nullptr);
                        dropStructureDeclaration();
                        instantiatingMemberFuncClass = oldMemberClass;
                        return false;
                    }
                }
                break;
                case kw_int: {
                    TYPE* tp1;
                    EXPRESSION* exp1 = nullptr;
                    if (itDest->second->byNonType.txttype)
                    {
                        LEXLIST* start = lex;
                        lex = SetAlternateLex(itSrc->second->byNonType.txttype);
                        openStructs = nullptr;
                        structLevel = 0;
                        noTypeNameError++;
                        lex = get_type_id(lex, &tp1, nullptr, sc_parameter, true, false, false);
                        noTypeNameError--;
                        openStructs = oldOpenStructs;
                        structLevel = oldStructLevel;
                        SetAlternateLex(nullptr);
                        lex = start;
                        if (tp1->type == bt_any)
                        {
                            parsingDefaultTemplateArgs--;
                            while (pushCount--)
                                dropStructureDeclaration();
                            PopTemplateNamespace(n);
                            SetAlternateLex(nullptr);
                            dropStructureDeclaration();
                            instantiatingMemberFuncClass = oldMemberClass;
                            return false;
                        }
                        itDest->second->byNonType.tp = tp1;
                    }
                    openStructs = nullptr;
                    structLevel = 0;
                    lex = expression_no_comma(lex, nullptr, nullptr, &tp1, &exp1, nullptr, _F_INTEMPLATEPARAMS);
                    optimize_for_constants(&exp1);
                    openStructs = oldOpenStructs;
                    structLevel = oldStructLevel;
                    itDest->second->byNonType.val = exp1;
                    if (!templatecomparetypes(itDest->second->byNonType.tp, tp1, true))
                    {
                        if (!ispointer(tp1) && !isint(tp1) && !isconstzero(tp1, exp1))
                        {
                            parsingDefaultTemplateArgs--;
                            while (pushCount--)
                                dropStructureDeclaration();
                            PopTemplateNamespace(n);
                            SetAlternateLex(nullptr);
                            dropStructureDeclaration();
                            instantiatingMemberFuncClass = oldMemberClass;
                            return false;
                        }
                    }
                    TYPE* tp2 = LookupTypeFromExpression(exp1, nullptr, false);
                    if (!tp2 || tp2->type == bt_any)
                    {
                        parsingDefaultTemplateArgs--;
                        while (pushCount--)
                            dropStructureDeclaration();
                        PopTemplateNamespace(n);
                        SetAlternateLex(nullptr);
                        dropStructureDeclaration();
                        instantiatingMemberFuncClass = oldMemberClass;
                        return false;
                    }
                }
                break;
                default:
                    break;
            }
            while (pushCount--)
                dropStructureDeclaration();
            PopTemplateNamespace(n);
            SetAlternateLex(nullptr);
        }
        if (itArgs != iteArgs)
            ++itArgs;
        if (itPrimary != itePrimary)
            ++itPrimary;
    }
    if (currents)
    {
        currents->bodyHead = head;
        currents->bodyTail = tail;
    }
    parsingDefaultTemplateArgs--;
    dropStructureDeclaration();
    instantiatingMemberFuncClass = oldMemberClass;
    return true;
}
void ScrubTemplateValues(SYMBOL* func)
{
    ClearArgValues(func->templateParams, func->sb->specialized);
    for (auto sym : *basetype(func->tp)->syms)
    {
        TYPE* tp = sym->tp;
        while (isref(tp) || ispointer(tp))
            tp = basetype(tp)->btp;
        if (isstructured(tp) && basetype(tp)->sp->templateParams && !basetype(tp)->sp->sb->instantiated &&
            !basetype(tp)->sp->sb->declaring)
            ClearArgValues(basetype(tp)->sp->templateParams, basetype(tp)->sp->sb->specialized);
    }
    TYPE* retval = basetype(basetype(func->tp)->btp);
    if (isstructured(retval) && retval->sp->templateParams && !retval->sp->sb->instantiated && !retval->sp->sb->declaring)
        ClearArgValues(retval->sp->templateParams, retval->sp->sb->specialized);
}
void PushPopTemplateArgs(SYMBOL* func, bool push)
{
    PushPopValues(func->templateParams, push);
    for (auto sym : *basetype(func->tp)->syms)
    {
        TYPE* tp = sym->tp;
        while (isref(tp) || ispointer(tp))
            tp = basetype(tp)->btp;
        if (isstructured(tp) && basetype(tp)->sp->templateParams && !basetype(tp)->sp->sb->declaring)
            PushPopValues(basetype(tp)->sp->templateParams, push);
    }
    TYPE* retval = basetype(basetype(func->tp)->btp);
    if (isstructured(retval) && retval->sp->templateParams && !retval->sp->sb->instantiated && !retval->sp->sb->declaring)
        PushPopValues(retval->sp->templateParams, push);
}
SYMBOL* TemplateDeduceArgsFromArgs(SYMBOL* sym, FUNCTIONCALL* args)
{
    std::list<TEMPLATEPARAMPAIR>* nparams = sym->templateParams;
    TYPE* thistp = args->thistp;
    std::list<INITLIST*>::iterator ita, itae;

    if (args->arguments)
    {
        if (args->arguments->size() == 1 && args->arguments->front()->nested)
        {
            auto itx = basetype(sym->tp)->syms->begin();
            if ((*itx)->sb->thisPtr)
                ++itx;
            bool il = false;
            if (isstructured((*itx)->tp))
            {
                SYMBOL* sym1 = basetype((*itx)->tp)->sp;
                il = sym1->sb->initializer_list && sym1->sb->templateLevel;
            }
            if (!il)
            {
                ita = args->arguments->front()->nested->begin();
                itae = args->arguments->front()->nested->end();
            }
            else
            {
                ita = args->arguments->begin();
                itae = args->arguments->end();
            }
        }
        else
        {
            ita = args->arguments->begin();
            itae = args->arguments->end();
        }
    }
    SYMBOL* rv;

    if (!thistp && ismember(sym) && args->arguments)
    {
        thistp = (*ita)->tp;
        ++ita;
    }
    if (args && thistp && sym->sb->parentClass && !nparams)
    {
        TYPE* tp = basetype(basetype(thistp)->btp);
        std::list<TEMPLATEPARAMPAIR>* src = tp->sp->templateParams;
        std::list<TEMPLATEPARAMPAIR>* dest = sym->sb->parentClass->templateParams;
        if (src && dest)
        {
            auto itSrc = src->begin();
            auto iteSrc = src->end();
            auto itDest = dest->begin();
            auto iteDest = dest->end();
            if (itSrc != iteSrc && itDest != iteDest)
            {
                ++itSrc;
                ++itDest;
            }
            for ( ; itSrc != iteSrc && itDest != iteDest; ++ itSrc, ++itDest)
            {
                itDest->second->byNonType.dflt = itSrc->second->byNonType.dflt;
                itDest->second->byNonType.val = itSrc->second->byNonType.val;
                itDest->second->byNonType.temp = itSrc->second->byNonType.temp;
                itDest->second->byNonType.tp = itSrc->second->byNonType.tp;
            }

            if (itSrc != iteSrc || itDest != iteDest)
                return nullptr;
        }
    }
    if (nparams)
    {
        std::list<TEMPLATEPARAMPAIR>* params = nparams;
        auto templateArgs = basetype(sym->tp)->syms->begin();
        auto templateArgsEnd = basetype(sym->tp)->syms->end();
        auto symArgs = ita;
        std::list<TEMPLATEPARAMPAIR>* initial = args->templateParams;
        ScrubTemplateValues(sym);
        // fill in params that have been initialized in the arg list
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tis;
        if (initial && params)
        {
            auto itInitial = initial->begin();
            auto iteInitial = initial->end();
            auto itParams = params->begin();
            auto iteParams = params->end();
            for (++itParams; itInitial != iteInitial && itParams != iteParams;)
            {
                if (itInitial->second->type != itParams->second->type)
                    if (itInitial->second->type != kw_typename || itParams->second->type != kw_template)
                        return nullptr;
                itParams->second->initialized = true;
                if (itInitial->second->packed)
                {
                    if (itInitial->second->byPack.pack)
                    {
                        if (packIndex >= 0)
                        {
                            auto itx = itInitial->second->byPack.pack->begin();
                            auto itxe = itInitial->second->byPack.pack->end();
                            for (int i = 0; i < packIndex && itx != itxe; i++)
                                ++itx;
                            if (itx == itxe)
                                return nullptr;
                            itParams->second->byClass.val = itx->second->byClass.dflt;
                            ++itParams;
                            ++itInitial;
                            if (itInitial == iteInitial && tis.size())
                            {
                                iteInitial = tis.top();
                                tis.pop();
                                itInitial = tis.top();
                                tis.pop();
                                ++itInitial;
                            }
                            continue;
                        }
                        else
                        {
                            tis.push(itInitial);
                            tis.push(iteInitial);
                            iteInitial = itInitial->second->byPack.pack->end();
                            itInitial = itInitial->second->byPack.pack->begin();
                            if (itParams->second->packed)
                                itParams->second->byPack.pack = templateParamPairListFactory.CreateList();
                        }
                    }
                    else
                    {
                        ++itInitial;
                        if (itParams->second->packed)
                            ++itParams;
                    }
                }
                if (itInitial == iteInitial)
                    break;
                if (itParams->second->packed)
                {
                    auto it = itParams;
                    ++it;
                    if (it != iteParams || itParams->second->type != kw_typename || itInitial->second->byClass.dflt->type != bt_void)
                    {
                        if (!itParams->second->byPack.pack)
                            itParams->second->byPack.pack = templateParamPairListFactory.CreateList();
                        itParams->second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                        itParams->second->byPack.pack->back().second->type = itParams->second->type;
                        itParams->second->byPack.pack->back().second->byClass.val = itInitial->second->byClass.dflt;
                        if (itInitial->second->type == kw_int)
                            itParams->second->byPack.pack->back().second->byNonType.tp = itInitial->second->byNonType.tp;
                        itParams->second->byPack.pack->back().second->initialized = true;
                    }
                }
                else if (itInitial->second->type == kw_typename && itParams->second->type == kw_template)
                {
                    TYPE* tp1 = itInitial->second->byClass.dflt;
                    while (tp1 && tp1->type != bt_typedef && tp1->btp)
                        tp1 = tp1->btp;
                    if (tp1->type != bt_typedef && !isstructured(tp1))
                        return nullptr;
                    itParams->second->byTemplate.val = tp1->sp;
                    ++itParams;
                }
                else
                {
                    itParams->second->byClass.val = itInitial->second->byClass.dflt;
                    ++itParams;
                }
                ++itInitial;
                if (itInitial == iteInitial && tis.size())
                {
                    iteInitial = tis.top();
                    tis.pop();
                    itInitial = tis.top();
                    tis.pop();
                    ++itInitial;
                }
            }
        }
        bool allArgsSpecified = params == nullptr;

        // check the specialization list for validity
        params = nparams->front().second->bySpecialization.types;
        initial = args->templateParams;
        if (initial && params)
        {
            auto itInitial = initial->begin();
            auto iteInitial = initial->end();
            auto itParams = params->begin();
            auto iteParams = params->end();
            for (; itInitial != iteInitial && itParams != iteParams;)
            {
                if (itInitial->second->type != itParams->second->type)
                    return nullptr;
                switch (itInitial->second->type)
                {
                    case kw_typename:
                        if (!templatecomparetypes(itInitial->second->byClass.dflt, itParams->second->byClass.dflt, true))
                            return nullptr;
                        break;
                    case kw_template:
                        if (!exactMatchOnTemplateParams(itInitial->second->byTemplate.dflt->templateParams,
                                                        itParams->second->byTemplate.dflt->templateParams))
                            return nullptr;
                        break;
                    case kw_int:
                        if (!templatecomparetypes(itInitial->second->byNonType.tp, itParams->second->byNonType.tp, true) &&
                            (!ispointer(itParams->second->byNonType.tp) ||
                             !isconstzero(itInitial->second->byNonType.tp, itParams->second->byNonType.dflt)))
                            return nullptr;
                        break;
                    default:
                        break;
                }
                ++itInitial;
                if (!itParams->second->packed)
                    ++itParams;
            }
        }
        // Deduce any args that we can
        if ((*templateArgs)->sb->thisPtr)
            ++templateArgs;
        auto temp = templateArgs;
        while (temp != templateArgsEnd)
        {
            auto* sp1 = (*temp);
            if (sp1->packed)
            {
                TYPE* tpx = sp1->tp;
                while (isref(tpx))
                    tpx = basetype(tpx)->btp;
                auto base = basetype(tpx)->templateParam;
                if (!base || base->second->type != kw_typename)
                    temp = templateArgsEnd;
                break;
            }
            ++temp;
        }
        if (temp != templateArgsEnd)
        {
            // we have to gather the args list
            std::list<TEMPLATEPARAMPAIR>::iterator itParams, iteParams;
            itParams = nparams->begin();
            iteParams = nparams->end();
            ++itParams;
            for (; templateArgs != basetype(sym->tp)->syms->end() && symArgs != itae;)
            {
                SYMBOL* sp = *templateArgs;
                if (sp->packed)
                    break;
                if (itParams == iteParams || !itParams->second->byClass.dflt)
                {
                    if (TemplateDeduceFromArg(sp->tp, (*symArgs)->tp, (*symArgs)->exp, false, false))
                    {
                        if (isstructured(sp->tp) && basetype(sp->tp)->sp->templateParams)
                        {
                            std::list<TEMPLATEPARAMPAIR>* params2 = basetype(sp->tp)->sp->templateParams;
                            std::list<TEMPLATEPARAMPAIR>* special =
                                params2->front().second->bySpecialization.types ? params2->front().second->bySpecialization.types : params2;
                            std::list<TEMPLATEPARAMPAIR> xxparams(itParams, iteParams);
                            TransferClassTemplates(special, special, &xxparams);
                        }
                    }
                    ++symArgs;
                }
                ++templateArgs;
                if (itParams != iteParams)
                    ++itParams;
            }
            if (templateArgs != templateArgsEnd)
            {
                SYMBOL* sp = *templateArgs;
                TYPE* tp = sp->tp;
                TEMPLATEPARAMPAIR* base;
                bool forward = false;
                if (isref(tp))
                {
                    if (basetype(tp)->type == bt_rref)
                    {
                        tp = basetype(tp)->btp;
                        if (!isconst(tp) && !isvolatile(tp))
                            forward = true;
                    }
                    else
                    {
                        tp = basetype(tp)->btp;
                    }
                }
                base = basetype(tp)->templateParam;
                if (base && base->second->type == kw_typename && symArgs != itae)
                {
                    auto last = base->second->byPack.pack = templateParamPairListFactory.CreateList();
                    for ( ;symArgs != itae; ++symArgs)
                    {
                        last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});

                        last->back().second->type = kw_typename;
                        last->back().second->byClass.val = rewriteNonRef((*symArgs)->tp);
                        if (TemplateConstExpr(last->back().second->byClass.val, (*symArgs)->exp))
                            last->back().second->byClass.val = MakeType(bt_const, last->back().second->byClass.val);
                        if (forward && !templateNestingCount)
                        {
                            last->back().second->byClass.val =
                                GetForwardType(nullptr, last->back().second->byClass.val, (*symArgs)->exp);
                            if (isref(last->back().second->byClass.val))
                            {
                                basetype(basetype(last->back().second->byClass.val)->btp)->rref = false;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            bool rv = TemplateDeduceArgList(basetype(sym->tp)->syms->begin(), basetype(sym->tp)->syms->end(),  templateArgs, templateArgsEnd, symArgs, itae, basetype(sym->tp)->type == bt_templateselector, true);
            for (auto sp : *basetype(sym->tp)->syms)
            {
                TYPE* tp = sp->tp;
                while (isref(tp) || ispointer(tp))
                    tp = basetype(tp)->btp;

                if (isstructured(tp) && basetype(tp)->sp->templateParams)
                {
                    std::list<TEMPLATEPARAMPAIR>* params = basetype(tp)->sp->templateParams;
                    std::list<TEMPLATEPARAMPAIR>* special =
                        params->front().second->bySpecialization.types ? params->front().second->bySpecialization.types : params;
                    if (special)
                    {
                        std::list<TEMPLATEPARAMPAIR>* tpx = sym->templateParams->front().second->bySpecialization.types
                                                     ? sym->templateParams->front().second->bySpecialization.types
                                                     : sym->templateParams;
                        if (tpx)
                        {
                            for (auto&& tplx : *tpx)
                            {if (tplx.second->type != kw_new)
                                {
                                    std::list<TEMPLATEPARAMPAIR> a{tplx};
                                    TransferClassTemplates(special, special, &a);
                                }
                            }
                        }
                    }
                }
            }
            if (!rv)
            {
                if (!allTemplateArgsSpecified(sym, nparams, true))
                    return nullptr;
            }
        }
        // set up default values for non-deduced and non-initialized args

        if (TemplateParseDefaultArgs(sym, nullptr, nparams, nparams, nparams))
            if ((rv = ValidateArgsSpecified(sym->templateParams, sym, args->arguments, nparams)))
            {
                return rv;
            }
        return nullptr;
    }
    rv = SynthesizeResult(sym, nparams);
    rv->sb->maintemplate = sym;
    return rv;
}
static bool TemplateDeduceFromType(TYPE* P, TYPE* A)
{
    if (P->type == bt_templatedecltype)
        P = LookupTypeFromExpression(P->templateDeclType, nullptr, false);
    if (P)
        return Deduce(P, A, nullptr, true, false, false, false);
    return false;
}
SYMBOL* TemplateDeduceWithoutArgs(SYMBOL* sym)
{
    std::list<TEMPLATEPARAMPAIR>* nparams = sym->templateParams;
    SYMBOL* rv;
    if (TemplateParseDefaultArgs(sym, nullptr, nparams, nparams, nparams) &&
        (rv = ValidateArgsSpecified(sym->templateParams, sym, nullptr, nparams)))
    {
        return rv;
    }
    return nullptr;
}
static bool TemplateDeduceFromConversionType(TYPE* orig, TYPE* tp)
{
    TYPE *P = orig, *A = tp;
    if (isref(P))
        P = basetype(P)->btp;
    if (!isref(A))
    {
        P = rewriteNonRef(P);
    }
    A = RemoveCVQuals(A);
    if (TemplateDeduceFromType(P, A))
        return true;
    if (ispointer(P))
    {
        bool doit = false;
        while (ispointer(P) && ispointer(A))
        {
            if ((isconst(P) && !isconst(A)) || (isvolatile(P) && !isvolatile(A)))
                return false;
            P = basetype(P)->btp;
            A = basetype(A)->btp;
        }
        P = basetype(P);
        A = basetype(A);
        if (doit && TemplateDeduceFromType(P, A))
            return true;
    }
    return false;
}
SYMBOL* TemplateDeduceArgsFromType(SYMBOL* sym, TYPE* tp)
{
    std::list<TEMPLATEPARAMPAIR>* nparams = sym->templateParams;
    ClearArgValues(nparams, sym->sb->specialized);
    if (sym->sb->castoperator)
    {
        TemplateDeduceFromConversionType(basetype(sym->tp)->btp, tp);
        return SynthesizeResult(sym, nparams);
    }
    else
    {
        auto templateArgs = basetype(tp)->syms->begin();
        auto templateArgsEnd = basetype(tp)->syms->end();
        auto symArgs = basetype(sym->tp)->syms->begin();
        auto symArgsEnd = basetype(sym->tp)->syms->end();
        std::list<TEMPLATEPARAMPAIR>* params;
        while (templateArgs != templateArgsEnd && symArgs != symArgsEnd)
        {
            SYMBOL* sp = *symArgs;
            if (sp->packed)
                break;
            TemplateDeduceFromType(sp->tp, (*templateArgs)->tp);
            ++templateArgs;
            ++symArgs;
        }
        if (templateArgs != templateArgsEnd && symArgs != symArgsEnd)
        {
            SYMBOL* sp = *symArgs;
            TYPE* tp = sp->tp;
            TEMPLATEPARAMPAIR* base;
            if (isref(tp))
                tp = basetype(tp)->btp;
            base = tp->templateParam;
            if (base->second->type == kw_typename)
            {
                auto last = base->second->byPack.pack = templateParamPairListFactory.CreateList();
                for (; symArgs != symArgsEnd ; ++symArgs)
                {
                    last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                    last->back().second->type = kw_typename;
                    last->back().second->byClass.val = sp->tp;
                }
            }
        }
        TemplateDeduceFromType(basetype(sym->tp)->btp, basetype(tp)->btp);
        if (nparams)
        {
            SYMBOL* rv;
            if (TemplateParseDefaultArgs(sym, nullptr, nparams, nparams, nparams) &&
                (rv = ValidateArgsSpecified(sym->templateParams, sym, nullptr, nparams)))
            {
                return rv;
            }
        }
    }
    return nullptr;
}
int TemplatePartialDeduceFromType(TYPE* orig, TYPE* sym, bool byClass)
{
    TYPE *P = orig, *A = sym;
    int which = -1;
    if (isref(P))
        P = basetype(P)->btp;
    if (isref(A))
        A = basetype(A)->btp;
    if (isref(orig) && isref(sym))
    {
        bool p = false, a = false;
        if ((isconst(P) && !isconst(A)) || (isvolatile(P) && !isvolatile(A)))
            p = true;
        if ((isconst(A) && !isconst(P)) || (isvolatile(A) && !isvolatile(P)))
            a = true;
        if (a && !p)
            which = 1;
    }
    A = RemoveCVQuals(A);
    P = RemoveCVQuals(P);
    if (!Deduce(P, A, nullptr, true, byClass, false, false))
        return 0;
    if (comparetypes(P, A, false))
        return 0;
    return which;
}
int TemplatePartialDeduce(TYPE* origl, TYPE* origr, TYPE* syml, TYPE* symr, bool byClass)
{
    int n = TemplatePartialDeduceFromType(origl, symr, byClass);
    int m = TemplatePartialDeduceFromType(origr, syml, byClass);
    if (n && m)
    {
        if (basetype(origl)->type == bt_lref)
        {
            if (basetype(origr)->type != bt_lref)
                return -1;
            else
                return -1;  // originally checked n & m but since that's already checked just do this, pointing this out since
                            // it's GAURENTEED to return -1
            return 1;
        }
        else if (basetype(origr)->type == bt_lref)
        {
            return 1;
        }
        if (n > 0 && m <= 0)
            return -1;
        else if (m > 0 && n <= 0)
            return 1;
        n = m = 0;
    }
    if (n)
        return -1;
    if (m)
        return 1;
    if (isref(origl))
        origl = basetype(origl)->btp;
    if (isref(origr))
        origr = basetype(origr)->btp;
    auto left = basetype(origl)->templateParam;
    auto right = basetype(origr)->templateParam;
    if (left && right)
    {
        if (left->second->packed && !right->second->packed)
            return -1;
        if (right->second->packed && !left->second->packed)
            return 1;
    }
    return 0;
}
int TemplatePartialDeduceArgsFromType(SYMBOL* syml, SYMBOL* symr, TYPE* tpx, TYPE* tpr, FUNCTIONCALL* fcall)
{
    int which = 0;
    int arr[200], n;
    ClearArgValues(syml->templateParams, syml->sb->specialized);
    ClearArgValues(symr->templateParams, symr->sb->specialized);
    if (isstructured(syml->tp))
    {
        which = TemplatePartialDeduce(syml->tp, symr->tp, tpx, tpr, true);
    }
    else if (syml->sb->castoperator)
    {
        which =
            TemplatePartialDeduce(basetype(syml->tp)->btp, basetype(symr->tp)->btp, basetype(tpx)->btp, basetype(tpr)->btp, false);
    }
    else if (!isfunction(syml->tp))
    {
        which = TemplatePartialDeduce(syml->tp, symr->tp, tpx, tpr, true);
    }
    else
    {
        int i;
        auto tArgsl = basetype(tpx)->syms->begin();
        auto tArgsle = basetype(tpx)->syms->end();
        auto sArgsl = basetype(syml->tp)->syms->begin();
        auto sArgsle = basetype(syml->tp)->syms->end();
        auto tArgsr = basetype(tpr)->syms->begin();
        auto tArgsre = basetype(tpr)->syms->end();
        auto sArgsr = basetype(symr->tp)->syms->begin();
        auto sArgsre = basetype(symr->tp)->syms->end();
        bool usingargs = fcall && fcall->ascall;
        auto ita = fcall && fcall->arguments ? fcall->arguments->begin() : std::list<INITLIST*>::iterator();
        auto itae = fcall && fcall->arguments ? fcall->arguments->end() : std::list<INITLIST*>::iterator();
        if (fcall && fcall->thisptr)
        {

            ++tArgsl;
            ++sArgsl;
            ++tArgsr;
            ++sArgsr;
        }
        n = 0;
        while (tArgsl != tArgsle && tArgsr != tArgsre && sArgsl != sArgsle && sArgsr != sArgsre && (!usingargs || ita != itae))
        {
            arr[n++] = TemplatePartialDeduce((*sArgsl)->tp, (*sArgsr)->tp, (*tArgsl)->tp,
                                             (*tArgsr)->tp, false);
            if (ita != itae)
                ++ita;
            ++tArgsl;
            ++sArgsl;
            ++tArgsr;
            ++sArgsr;
        }
        for (i = 0; i < n; i++)
            if (arr[i] == 100)
                return 0;
        for (i = 0; i < n; i++)
            if (!which)
                which = arr[i];
            else if (which && arr[i] && which != arr[i])
                return 0;
    }
    if (which == 100)
        which = 0;
    if (!which)
    {
        std::list<TEMPLATEPARAMPAIR>* l = syml->templateParams->front().second->bySpecialization.types ? syml->templateParams->front().second->bySpecialization.types
                                                                                : syml->templateParams;
        std::list<TEMPLATEPARAMPAIR>* r = symr->templateParams->front().second->bySpecialization.types ? symr->templateParams->front().second->bySpecialization.types
                                                                                : symr->templateParams;
        if (l && r)
        {
            int i;
            n = 0;
            auto itl = l->begin();
            auto itel = l->end();
            if (itl->second->type == kw_new)
                ++itl;
            auto itr = r->begin();
            auto iter = r->end();
            if (itr->second->type == kw_new)
                ++itr;
            for (;itl != itel && itr != iter; ++itl, ++itr)
            {
                int l1 = itl->second->type == kw_typename ? !!itl->second->byClass.val : 0;
                int r1 = itr->second->type == kw_typename ? !!itr->second->byClass.val : 0;
                if (l1 && !r1)
                    arr[n++] = -1;
                else if (r1 && !l1)
                    arr[n++] = 1;
            }
            for (i = 0; i < n; i++)
                if (!which)
                    which = arr[i];
                else if (which && which != arr[i])
                    return 0;
        }
    }
    return which;
}
void TemplatePartialOrdering(SYMBOL** table, int count, FUNCTIONCALL* funcparams, TYPE* atype, bool asClass, bool save)
{
    (void)atype;
    int i, j, c = 0;
    int cn = 0;
    for (i = 0; i < count; i++)
        if (table[i])
            c++;
    if (c)
    {
        if (funcparams && funcparams->templateParams)
        {
            cn = funcparams->templateParams->size();
        }
    }
    if (c > 1)
    {
        int len = 0;
        for (i = 0; i < count; i++)
            if (table[i] && table[i]->sb->templateLevel)
                len++;
        Optimizer::LIST *types = nullptr, *exprs = nullptr, *classes = nullptr;
        TYPE** typetab = (TYPE**)alloca(sizeof(TYPE*) * count);
        SYMBOL* allocedSyms = (SYMBOL*)alloca(sizeof(SYMBOL) * len);
        sym::_symbody* allocedBodies = (sym::_symbody*)alloca(sizeof(sym::_symbody) * len);
        TYPE* allocedTypes = (TYPE*)alloca(sizeof(TYPE) * len);
        int j = 0;
        if (save)
            saveParams(table, count);
        for (i = 0; i < count; i++)
        {
            if (table[i] && table[i]->sb->templateLevel)
            {
                SYMBOL* sym = table[i];
                std::list<TEMPLATEPARAMPAIR>* params;
                Optimizer::LIST *typechk, *exprchk, *classchk;
                if (!asClass)
                {
                    sym = sym->sb->parentTemplate;
                }
                params = sym->templateParams;
                typechk = types;
                exprchk = exprs;
                classchk = classes;
                if (params)
                {
                    for (auto&& param : *params)
                    {
                        switch (param.second->type)
                        {
                            case kw_typename:
                                if (typechk)
                                {
                                    param.second->byClass.temp = (TYPE*)typechk->data;
                                    typechk = typechk->next;
                                }
                                else
                                {
                                    Optimizer::LIST* lst = Allocate<Optimizer::LIST>();
                                    TYPE* tp = CopyType(param.first->tp);
                                    param.second->byClass.temp = tp;
                                    lst->data = tp;
                                    lst->next = types;
                                    types = lst;
                                }
                                break;
                            case kw_template:
                                param.second->byTemplate.temp = param.first;
                                break;
                            case kw_int:
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (isstructured(sym->tp))
                    typetab[i] = SynthesizeTemplate(sym->tp, &allocedSyms[j], &allocedBodies[j], &allocedTypes[j])->tp;
                else
                    typetab[i] = SynthesizeType(sym->tp, nullptr, true);
                if (typetab[i]->type == bt_any)
                    table[i] = nullptr;
                j++;
            }
        }
        for (i = 0; i < count - 1; i++)
        {
            if (table[i])
            {
                for (j = i + 1; table[i] && j < count; j++)
                {
                    if (table[j])
                    {
                        int which = TemplatePartialDeduceArgsFromType(asClass ? table[i] : table[i]->sb->parentTemplate,
                                                                      asClass ? table[j] : table[j]->sb->parentTemplate,
                                                                      asClass || !basetype(typetab[i])->sp->sb->parentTemplate
                                                                          ? typetab[i]
                                                                          : basetype(typetab[i])->sp->sb->parentTemplate->tp,
                                                                      asClass || !basetype(typetab[j])->sp->sb->parentTemplate
                                                                          ? typetab[j]
                                                                          : basetype(typetab[j])->sp->sb->parentTemplate->tp,
                                                                      funcparams);
                        if (which < 0)
                        {
                            table[i] = 0;
                        }
                        else if (which > 0)
                        {
                            table[j] = 0;
                        }
                    }
                }
            }
        }
        if (save)
            restoreParams(table, count);
    }
}
static bool comparePointerTypes(TYPE* tpo, TYPE* tps)
{
    TYPE* lastIndirect = nullptr;
    while (tpo && tps)
    {
        tpo = basetype(tpo);
        tps = basetype(tps);
        if (!tpo || !tps)
            return false;
        if (tpo->type == bt_templateparam)
        {
            if (tpo == lastIndirect)
                return true;
            lastIndirect = tpo;
            tpo = tpo->templateParam->second->byClass.dflt;
        }
        if (tps->type == bt_templateparam)
            tps = tps->templateParam->second->byClass.dflt;
        if (!tpo || !tps || tpo->type == bt_templateparam || tps->type == bt_templateparam)
            return false;
        if (tpo->type != tps->type)
            return false;
        tpo = tpo->btp;
        tps = tps->btp;
    }
    return tpo == tps;
}
static bool TemplateInstantiationMatchInternal(std::list<TEMPLATEPARAMPAIR>* porig, std::list<TEMPLATEPARAMPAIR>* psym, bool dflt, bool bySpecialization)
{
    if (porig && psym)
    {
        auto itOrig = porig->begin();
        auto iteOrig = porig->end();
        auto itSym = psym->begin();
        auto iteSym = psym->end();
        if (bySpecialization && itOrig->second->bySpecialization.types)
        {
            iteOrig = itOrig->second->bySpecialization.types->end();
            itOrig = itOrig->second->bySpecialization.types->begin();
        }
        else
        {
            ++itOrig;
        }

        if (bySpecialization && itSym->second->bySpecialization.types)
        {
            iteSym = itSym->second->bySpecialization.types->end();
            itSym = itSym->second->bySpecialization.types->begin();
        }
        else
        {
            ++itSym;
        }
        for ( ; itOrig != iteOrig && itSym != iteSym; ++itOrig, ++itSym)
        {
            void *xorig, *xsym;
            if (dflt)
            {
                xorig = itOrig->second->byClass.dflt;
                xsym = itSym->second->byClass.dflt;
                if (!xsym)
                    xsym = itSym->second->byClass.val;
            }
            else
            {
                xorig = itOrig->second->byClass.val;
                xsym = itSym->second->byClass.val;
            }
            if (itOrig->second->type != itSym->second->type)
                return false;
            switch (itOrig->second->type)
            {
                case kw_typename: {
                    if (itOrig->second->packed != itSym->second->packed)
                        return false;
                    if (itOrig->second->packed)
                    {
                        std::list<TEMPLATEPARAMPAIR>* packorig = itOrig->second->byPack.pack;
                        std::list<TEMPLATEPARAMPAIR>* packsym = itSym->second->byPack.pack;
                        if (packorig && packsym)
                        {
                            auto itPOrig = packorig->begin();
                            auto itePOrig = packorig->end();
                            auto itPSym = packsym->begin();
                            auto itePSym = packsym->end();
                            for ( ; itPOrig != itePOrig && itPSym != itePSym; ++itPOrig, ++itPSym)
                            {
                                TYPE* torig;
                                TYPE* tsym;
                                if (dflt)
                                {
                                    torig = itPOrig->second->byClass.dflt;
                                    if (!torig)
                                        torig = itPOrig->second->byClass.val;
                                    tsym = itPSym->second->byClass.dflt;
                                    if (!tsym)
                                        tsym = itPSym->second->byClass.val;
                                }
                                else
                                {
                                    torig = itPOrig->second->byClass.val;
                                    tsym = itPSym->second->byClass.val;
                                }
                                if (basetype(torig)->nullptrType != basetype(tsym)->nullptrType)
                                    return false;
                                if (isref(torig) != isref(tsym))
                                    return false;
                                if (basetype(torig)->array != basetype(tsym)->array)
                                    return false;
                                if (basetype(torig)->array && !!basetype(torig)->esize != !!basetype(tsym)->esize)
                                    return false;
                                if ((basetype(torig)->type == bt_enum) != (basetype(tsym)->type == bt_enum))
                                    return false;
                                if (tsym->type == bt_templateparam)
                                    tsym = tsym->templateParam->second->byClass.val;
                                if ((!templatecomparetypes(torig, tsym, true, false) ||
                                     !templatecomparetypes(tsym, torig, true, false)) &&
                                    !sameTemplate(torig, tsym, true))
                                    break;
                                if (isref(torig))
                                    torig = basetype(torig)->btp;
                                if (isref(tsym))
                                    tsym = basetype(tsym)->btp;
                                if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
                                    return false;
                            }
                            if (itPOrig != itePOrig || itPSym != itePSym)
                                return false;
                        }
                        else if (packorig || packsym)
                            return false;
                    }
                    else if (xorig && xsym)
                    {
                        TYPE* torig = (TYPE*)xorig;
                        TYPE* tsym = (TYPE*)xsym;
                        TYPE* btorig = basetype(torig);
                        TYPE* btsym = basetype(tsym);
                        if (btorig->nullptrType != btsym->nullptrType)
                            return false;
                        if (isref(torig) != isref(tsym))
                            return false;
                        if (btorig->array != btsym->array)
                            return false;
                        if (btorig->array && !!btorig->esize != !!btsym->esize)
                            return false;
                        if ((btorig->type == bt_enum) != (btsym->type == bt_enum))
                            return false;
                        if ((!templatecomparetypes(torig, tsym, true, false) || !templatecomparetypes(tsym, torig, true, false)) &&
                            !sameTemplate(torig, tsym, true))
                            return false;
                        if (!comparePointerTypes(torig, tsym))
                            return false;
                        if (isref(torig))
                            torig = btorig->btp;
                        if (isref(tsym))
                            tsym = btsym->btp;
                        if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
                            return false;
                        if (btsym->type == bt_enum || btsym->enumConst)
                        {
                            if (btorig->sp != btsym->sp)
                                return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                    break;
                }
                case kw_template:
                    if (xorig != xsym)
                        return false;
                    break;
                case kw_int:
                    if (itOrig->second->packed != itSym->second->packed)
                        return false;
                    if (itOrig->second->packed)
                    {
                        std::list<TEMPLATEPARAMPAIR>* packorig = itOrig->second->byPack.pack;
                        std::list<TEMPLATEPARAMPAIR>* packsym = itSym->second->byPack.pack;
                        if (packorig && packsym)
                        {
                            auto itPOrig = packorig->begin();
                            auto itePOrig = packorig->end();
                            auto itPSym = packsym->begin();
                            auto itePSym = packsym->end();
                            for (; itPOrig != itePOrig && itPSym != itePSym; ++itPOrig, ++itPSym)
                            {
                                EXPRESSION* torig;
                                EXPRESSION* tsym;
                                if (dflt)
                                {
                                    torig = itPOrig->second->byNonType.dflt;
                                    if (!torig)
                                        torig = itPSym->second->byNonType.val;
                                    tsym = itPSym->second->byNonType.dflt;
                                    if (!tsym)
                                        tsym = itPSym->second->byNonType.val;
                                }
                                else
                                {
                                    torig = itPOrig->second->byNonType.val;
                                    tsym = itPSym->second->byNonType.val;
                                }
                                if (!templatecomparetypes(itPOrig->second->byNonType.tp, itPSym->second->byNonType.tp, true))
                                    return false;
                                if (tsym && !equalTemplateIntNode((EXPRESSION*)torig, (EXPRESSION*)tsym))
                                    return false;
                            }
                            if (itPOrig != itePOrig || itPSym != itePSym)
                                return false;
                        }
                        else if (packorig || packsym)
                            return false;
                    }
                    else
                    {
                        if (!templatecomparetypes(itOrig->second->byNonType.tp, itSym->second->byNonType.tp, true))
                            return false;
                        if (xsym && xorig && !equalTemplateIntNode((EXPRESSION*)xorig, (EXPRESSION*)xsym))
                            return false;
                    }
                    break;
                default:
                    break;
            }
        }
        if (itSym != iteSym && itSym->second->packed && !itSym->second->byPack.pack)
            return true;
        return itOrig == iteOrig && itSym == iteSym;
    }
    return !porig && !psym;
}
bool TemplateInstantiationMatch(SYMBOL* orig, SYMBOL* sym, bool bySpecialization)
{
    if (orig && orig->sb->parentTemplate == sym->sb->parentTemplate)
    {
        if (!TemplateInstantiationMatchInternal(orig->templateParams, sym->templateParams, false, bySpecialization))
            return false;
        while (orig->sb->parentClass && sym->sb->parentClass)
        {
            orig = orig->sb->parentClass;
            sym = sym->sb->parentClass;
        }
        if (orig->sb->parentClass || sym->sb->parentClass)
            return false;
        return true;
    }
    return false;
}
void TemplateTransferClassDeferred(SYMBOL* newCls, SYMBOL* tmpl)
{
    if (newCls->tp->syms && (!newCls->templateParams || !newCls->templateParams->front().second->bySpecialization.types))
    {
        if (newCls->tp->syms && tmpl->tp->syms)
        {
            auto ns = newCls->tp->syms->begin();
            auto nse = newCls->tp->syms->end();
            auto os = tmpl->tp->syms->begin();
            auto ose = tmpl->tp->syms->end();
            while (ns != nse && os != ose)
            {
                SYMBOL* ss = *ns;
                SYMBOL* ts = *os;
                if (strcmp(ss->name, ts->name) != 0)
                {
                    ts = search(tmpl->tp->syms, ss->name);
                    // we might get here with ts = nullptr for example when a using statement inside a template
                    // references base class template members which aren't defined yet.
                }
                if (ts)
                {
                    if (ss->tp->type == bt_aggregate && ts->tp->type == bt_aggregate)
                    {
                        auto os2 = ts->tp->syms->begin();
                        auto os2e = ts->tp->syms->end();
                        auto ns2 = ss->tp->syms->begin();
                        auto ns2e = ss->tp->syms->end();

                        // these lists may be mismatched, in particular the old symbol table
                        // may have partial specializations for templates added after the class was defined...
                        while (ns2 != ns2e && os2 != os2e)
                        {
                            SYMBOL* ts2 = *os2;
                            SYMBOL* ss2 = *ns2;
                            if (ts2->sb->defaulted || ss2->sb->defaulted)
                                break;
                            ss2->sb->copiedTemplateFunction = true;
                            if (ts2->sb->deferredCompile && !ss2->sb->deferredCompile)
                            {
                                auto tsf = basetype(ts2->tp)->syms->begin();
                                auto tsfe = basetype(ts2->tp)->syms->end();
                                auto ssf = basetype(ss2->tp)->syms->begin();
                                auto ssfe = basetype(ss2->tp)->syms->end();
                                while (tsf != tsfe && ssf != ssfe)
                                {
                                    if (!(*ssf)->sb->anonymous || !(*tsf)->sb->anonymous)
                                        (*ssf)->name = (*tsf)->name;
                                    ++tsf;
                                    ++ssf;
                                }
                                ss2->sb->deferredCompile = ts2->sb->deferredCompile;
                                ss2->sb->attribs.inheritable.linkage4 = lk_virtual;
                            }
                            ss2->sb->maintemplate = ts2;
                            ++ns2;
                            ++os2;
                        }
                    }
                }
                ++ns;
                ++os;
            }
        }
        if (newCls->tp->tags && tmpl->tp->tags)
        {
            auto ns = newCls->tp->tags->begin();
            auto nse = newCls->tp->tags->end();
            auto os = tmpl->tp->tags->begin();
            auto ose = tmpl->tp->tags->end();
            ++ns, ++os; // past embedded reference to self
            while (ns != nse && os != ose)
            {
                SYMBOL* ss = *ns;
                SYMBOL* ts = *os;
                if (strcmp(ss->name, ts->name) != 0)
                {
                    ts = search(tmpl->tp->syms, ss->name);
                    // we might get here with ts = nullptr for example when a using statement inside a template
                    // references base class template members which aren't defined yet.
                }
                if (ts)
                {
                    if (isstructured(ss->tp))
                    {
                        if (!ss->sb->deferredCompile)
                        {
                            ss->sb->deferredCompile = ts->sb->deferredCompile;
                            PerformDeferredInitialization(ss->tp, nullptr);
                        }
                        TemplateTransferClassDeferred(ss, ts);
                    }
                }
                ++ns;
                ++os;
            }
        }
    }
}
static bool ValidSpecialization(std::list<TEMPLATEPARAMPAIR>* special, std::list<TEMPLATEPARAMPAIR>* args, bool templateMatch)
{
    if (special && args)
    {
        auto itSpecial = special->begin();
        auto itArgs = args->begin();
        if (itSpecial->second->type == kw_new)
            ++itSpecial;
        for (; itSpecial != special->end() && itArgs != args->end(); ++ itSpecial, ++itArgs)
        {
            if (itSpecial->second->type != itArgs->second->type)
            {
                if (itArgs->second->type != kw_typename || (itArgs->second->byClass.dflt->type != bt_templateselector &&
                                                          itArgs->second->byClass.dflt->type != bt_templatedecltype))
                    return false;
            }
            if (!templateMatch)
            {
                if ((itSpecial->second->byClass.val && !itArgs->second->byClass.dflt) ||
                    (!itSpecial->second->byClass.val && itArgs->second->byClass.dflt))
                    return false;
                switch (itArgs->second->type)
                {
                    case kw_typename:
                        if (itArgs->second->byClass.dflt &&
                            !templatecomparetypes(itSpecial->second->byClass.val, itArgs->second->byClass.dflt, true))
                            return false;
                        break;
                    case kw_template:
                        if (itArgs->second->byTemplate.dflt &&
                            !ValidSpecialization(itSpecial->second->byTemplate.args, itArgs->second->byTemplate.dflt->templateParams,
                                                 true))
                            return false;
                        break;
                    case kw_int:
                        if (!templatecomparetypes(itSpecial->second->byNonType.tp, itArgs->second->byNonType.tp, true))
                            if (!isint(itSpecial->second->byNonType.tp) || !isint(itArgs->second->byNonType.tp))
                                return false;
                        break;
                    default:
                        break;
                }
            }
        }
        return (itSpecial == special->end() || itSpecial->second->byClass.txtdflt) && !args;
    }
    return (!special || special->front().second->byClass.txtdflt) && !args;
}
static SYMBOL* MatchSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (sym->sb->specialized)
    {
        if (ValidSpecialization(sym->templateParams->front().second->bySpecialization.types, args, false))
            return sym;
    }
    else
    {
        if (ValidSpecialization(sym->templateParams, args, true))
            return sym;
    }
    return nullptr;
}
int pushContext(SYMBOL* cls, bool all)
{
    STRUCTSYM* s;
    int rv;
    if (!cls)
        return 0;
    rv = pushContext(cls->sb->parentClass, true);
    if (cls->sb->templateLevel)
    {
        s = Allocate<STRUCTSYM>();
        s->tmpl = copyParams(cls->templateParams, false);
        addTemplateDeclaration(s);
        rv++;
    }
    if (all)
    {
        s = Allocate<STRUCTSYM>();
        s->str = cls;
        addStructureDeclaration(s);
        rv++;
    }
    return rv;
}
void SetTemplateNamespace(SYMBOL* sym)
{
    sym->sb->templateNameSpace = symListFactory.CreateList();
    sym->sb->templateNameSpace->insert(sym->sb->templateNameSpace->begin(), nameSpaceList.begin(), nameSpaceList.end());
    sym->sb->templateNameSpace->reverse(); //   will be reversed back when used
}
int PushTemplateNamespace(SYMBOL* sym)
{
    int rv = 0;
    for (auto sp : nameSpaceList)
        sp->sb->value.i++;
    if (sym && sym->sb->templateNameSpace)
    {
        for (auto it = sym->sb->templateNameSpace->begin(); it != sym->sb->templateNameSpace->end(); ++it)
        {
            auto itnext = it;
            ++itnext;
            auto sp = *it;
            if (!sp->sb->value.i || (itnext == sym->sb->templateNameSpace->end() && nameSpaceList.front() != sp))
            {
                if (sp->sb->nameSpaceValues)
                {
                    sp->sb->value.i++;

                    nameSpaceList.push_front(sp);
                    globalNameSpace->push_front(sp->sb->nameSpaceValues->front());

                    rv++;
                }
            }
        }
    }
    return rv;
}
void PopTemplateNamespace(int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        Optimizer::LIST* nlist;
        SYMBOL* sp;
        globalNameSpace->pop_front();
        nameSpaceList.front()->sb->value.i--;
        nameSpaceList.pop_front();
    }
    for (auto sp : nameSpaceList)
        sp->sb->value.i--;
}
static void SetTemplateArgAccess(SYMBOL* sym, bool accessible)
{
    if (accessible)
    {
        sym->sb->accessibleTemplateArgument++;
    }
    else
    {
        sym->sb->accessibleTemplateArgument--;
    }
}
static void SetAccessibleTemplateArgs(std::list<TEMPLATEPARAMPAIR>* args, bool accessible)
{
    if (args)
    {
        for (auto&& arg : *args)
        {
            if (arg.second->packed)
            {
                SetAccessibleTemplateArgs(arg.second->byPack.pack, accessible);
            }
            else
            {
                switch (arg.second->type)
                {
                    case kw_int: {
                        EXPRESSION* exp = arg.second->byNonType.val;
                        if (exp)
                            exp = GetSymRef(exp);
                        if (exp)
                        {
                            SetTemplateArgAccess(exp->v.sp, accessible);
                        }
                        break;
                    }
                    case kw_template: {
                        if (!allTemplateArgsSpecified(nullptr, arg.second->byTemplate.args))
                            return;
                        if (arg.second->byTemplate.val)
                            SetTemplateArgAccess(arg.second->byTemplate.val, accessible);
                    }
                    break;
                    case kw_typename:
                        if (arg.second->byClass.val)
                        {
                            if (isstructured(arg.second->byClass.val))
                            {
                                SetTemplateArgAccess(basetype(arg.second->byClass.val)->sp, accessible);
                            }
                            else if (basetype(arg.second->byClass.val)->type == bt_enum)  // DAL FIXED
                            {
                                SetTemplateArgAccess(basetype(arg.second->byClass.val)->sp, accessible);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
void SwapMainTemplateArgs(SYMBOL* cls)
{
    if (cls->sb->maintemplate)
    {
        std::list<TEMPLATEPARAMPAIR>* old = cls->sb->maintemplate->templateParams;
        std::list<TEMPLATEPARAMPAIR>* nw = cls->templateParams;
        auto ito = old->begin();
        auto itn = nw->begin();
        for (; ito != old->end() && itn != nw->end(); ++ito, ++itn)
        {
            SYMBOL* sp = ito->first;
            ito->first = itn->first;
            itn->first = sp;
        }
    }
}
SYMBOL* TemplateClassInstantiateInternal(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern)
{
    std::list<TEMPLATEPARAMPAIR>::iterator ita, itae;
    if (args)
    {
        ita = args->begin();
        itae = args->end();
    }
    if (ita != itae && ita->second->type == kw_new)
        ++ita;
    (void)args;
    LEXLIST* lex = nullptr;
    SYMBOL* cls = sym;
    int pushCount;
    if (cls->sb->attribs.inheritable.linkage4 == lk_virtual)
        return cls;
    if (packIndex == -1 && sym->sb->maintemplate)
    {
        auto tl = sym->templateParams;
        auto ts = sym->sb->maintemplate->templateParams;
        if (tl && ts)
        {
            auto itl = tl->begin();
            auto its = ts->begin();
            for (; itl != tl->end() && its != ts->end(); ++itl, ++its)
            {
                if (itl->second->packed && !its->second->packed)
                    return cls;
            }
        }
    }

    if (!isExtern)
    {
        if (sym->sb->maintemplate && (!sym->sb->specialized || sym->sb->maintemplate->sb->specialized))
        {
            lex = sym->sb->maintemplate->sb->deferredCompile;
            if (lex)
                sym->tp = sym->sb->maintemplate->tp;
        }
        if (!lex)
            lex = sym->sb->deferredCompile;
        if (!lex && sym->sb->parentTemplate && (!sym->sb->specialized || sym->sb->parentTemplate->sb->specialized))
            lex = sym->sb->parentTemplate->sb->deferredCompile;
        if (lex)
        {
            EnterInstantiation(lex, sym);
            int oldHeaderCount = templateHeaderCount;
            Optimizer::LIST* oldDeferred = deferred;
            bool defd = false;
            SYMBOL old;
            int nsl = PushTemplateNamespace(sym);
            LEXLIST* reinstateLex = lex;
            bool oldTemplateType = inTemplateType;
            auto oldLambdas = lambdas;
            int oldPackIndex = packIndex;
            int oldExpandingParams = expandingParams;
            int oldRegisterTemplate = dontRegisterTemplate;
            std::list<NAMESPACEVALUEDATA*>* oldLocalNameSpace = localNameSpace;
            int oldInArgs = inTemplateArgs;
            int oldArgumentNesting = argumentNesting;
            int oldFuncLevel = funcLevel;
            int oldintypedef = inTypedef;
            int oldTypeNameError = noTypeNameError;
            int oldParsingUsing = parsingUsing;
            int oldSearchingFunctions = inSearchingFunctions;
            int oldInAssignRHS = inAssignRHS;
            int oldResolvingStructDeclarations = resolvingStructDeclarations;
            int oldBodyIsDestructor = bodyIsDestructor;
            bodyIsDestructor = 0;
            resolvingStructDeclarations = 0;
            inAssignRHS = 0;
            inSearchingFunctions = 0;
            parsingUsing = 0;

            instantiatingClass++;

            noTypeNameError = 0;
            inTypedef = 0;
            funcLevel = 0;
            argumentNesting = 0;
            inTemplateArgs = 0;
            expandingParams = 0;
            NAMESPACEVALUEDATA nsvd = { };
            std::list<NAMESPACEVALUEDATA*> ns{ &nsvd };
            localNameSpace = &ns;
            SetAccessibleTemplateArgs(cls->templateParams, true);
            packIndex = -1;
            deferred = nullptr;
            templateHeaderCount = 0;
            lambdas.clear();
            old = *cls;
            cls->parserSet = false;
            cls->sb->parentClass = SynthesizeParentClass(cls->sb->parentClass);
            SwapMainTemplateArgs(cls);
            pushCount = pushContext(cls, false);
            cls->sb->attribs.inheritable.linkage4 = lk_virtual;
            cls->tp = CopyType(old.tp);
            UpdateRootTypes(cls->tp);
            cls->tp->syms = nullptr;
            cls->tp->tags = nullptr;
            cls->tp->sp = cls;
            cls->sb->baseClasses = nullptr;
            cls->sb->vbaseEntries = nullptr;
            instantiatingTemplate++;
            dontRegisterTemplate = templateNestingCount != 0;
            SwapMainTemplateArgs(cls);
            lex = SetAlternateLex(lex);
            cls->sb->instantiating = true;
            lex = innerDeclStruct(lex, nullptr, cls, false, cls->tp->type == bt_class ? ac_private : ac_public, cls->sb->isfinal,
                                  &defd, nullptr);
            cls->sb->instantiating = false;
            SetAlternateLex(nullptr);
            SwapMainTemplateArgs(cls);
            lex = reinstateLex;
            while (lex)
            {
                lex->data->registered = false;
                lex = lex->next;
            }
            SetAccessibleTemplateArgs(cls->templateParams, false);
            if (old.tp->syms)
                TemplateTransferClassDeferred(cls, &old);
            PopTemplateNamespace(nsl);
            instantiatingClass--;
            bodyIsDestructor = oldBodyIsDestructor;
            resolvingStructDeclarations = oldResolvingStructDeclarations;
            inAssignRHS = oldInAssignRHS;
            inSearchingFunctions = oldSearchingFunctions;
            parsingUsing = oldParsingUsing;
            noTypeNameError = oldTypeNameError;
            inTypedef = oldintypedef;
            dontRegisterTemplate = oldRegisterTemplate;
            packIndex = oldPackIndex;
            lambdas = oldLambdas;
            instantiatingTemplate--;
            inTemplateType = oldTemplateType;
            deferred = oldDeferred;
            cls->sb->instantiated = true;
            localNameSpace = oldLocalNameSpace;
            inTemplateArgs = oldInArgs;
            argumentNesting = oldArgumentNesting;
            expandingParams = oldExpandingParams;
            funcLevel = oldFuncLevel;
            templateHeaderCount = oldHeaderCount;
            while (pushCount--)
                dropStructureDeclaration();
            SwapMainTemplateArgs(cls);
            LeaveInstantiation();
        }
    }
    return cls;
}
SYMBOL* TemplateClassInstantiate(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern, enum e_sc storage_class)
{
    if (templateNestingCount)
    {
        SYMBOL* sym1 = MatchSpecialization(sym, args);
        if (sym1 && (storage_class == sc_parameter || !inTemplateBody))
        {
            auto last = templateParamPairListFactory.CreateList();
            last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            last->back().second->type = kw_new;
            last->insert(last->end(), args->begin(), args->end());
            sym1 = CopySymbol(sym1);
            sym1->templateParams = last;
            auto tp = sym1->tp;
            auto tpx = &sym1->tp;
            while (tp)
            {
                *tpx = CopyType(tp);
                UpdateRootTypes(*tpx);
                if (!tp->btp)
                {
                    (*tpx)->sp = sym1;
                    (*tpx)->templateParam = &last->front();
                }
                else
                {
                    tpx = &(*tpx)->btp;
                }
                tp = tp->btp;
            }
        }
        return sym1;
    }
    else
    {
        return TemplateClassInstantiateInternal(sym, args, isExtern);
    }
}
void TemplateDataInstantiate(SYMBOL* sym, bool warning, bool isExtern)
{
    (void)isExtern;
    if (!sym->sb->gentemplate)
    {
        InsertInlineData(sym);
        sym->sb->gentemplate = true;
    }
    else if (warning)
    {
        errorsym(ERR_TEMPLATE_ALREADY_INSTANTIATED, sym);
    }
}
SYMBOL* TemplateFunctionInstantiate(SYMBOL* sym, bool warning)
{
    SYMBOL* orig = sym;
    bool found = false;
    STRUCTSYM s;
    for (auto data : *sym->sb->overloadName->tp->syms)
    {
        if (data->sb->instantiated && TemplateInstantiationMatch(data, sym, true) && matchOverload(sym->tp, data->tp, true))
        {
            if (data->sb->attribs.inheritable.linkage4 == lk_virtual)
            {
                return data;
            }
            if (!data->sb->deferredCompile && sym->sb->deferredCompile)
            {
                data->sb->deferredCompile = sym->sb->deferredCompile;
                auto hrs = basetype(sym->tp)->syms->begin();
                for (auto sym : *basetype(data->tp)->syms)
                {
                    sym->name = (*hrs)->name;
                    ++hrs;
                }
            }
            sym = data;
            found = true;
            break;
        }
    }
    auto tp = basetype(basetype(sym->tp)->btp);
    sym->templateParams = copyParams(sym->templateParams, true);
    sym->sb->instantiated = true;
    SetLinkerNames(sym, lk_cdecl);
    sym->sb->gentemplate = true;
    sym->parserSet = false;
    if (sym->sb->parentClass && sym->sb->parentClass->templateParams)
    {
        auto oldParent = sym->sb->parentClass;
        sym->sb->parentClass = CopySymbol(sym->sb->parentClass);
        sym->sb->parentClass->sb->mainsym = oldParent;
        sym->sb->parentClass->templateParams = copyParams(sym->sb->parentClass->templateParams, true);
    }
    if (!found)
    {
        bool ok = true;
        for (auto sp1 : *sym->sb->overloadName->tp->syms)
        {
            if (matchOverload(sym->tp, sp1->tp, true))
            {
                ok = false;
                break;
            }
        }
        if (ok)
        {
            if (!inNoExceptHandler)
                sym->sb->overloadName->tp->syms->insertOverload(sym);
        }

        if (ismember(sym))
        {
            injectThisPtr(sym, basetype(sym->tp)->syms);
        }
    }
    return sym;
}
static bool CheckConstCorrectness(TYPE* P, TYPE* A, bool byClass)
{
    while (P && A)
    {
        P = basetype(P);
        A = basetype(A);
        if (P->type != A->type)
            break;
        P = P->btp;
        A = A->btp;
        if (P && A)
        {
            if (byClass)
            {
                if ((isconst(A) != isconst(P)) || (isvolatile(A) != isvolatile(P)))
                    return false;
            }
            else
            {
                if ((isconst(A) && !isconst(P)) || (isvolatile(A) && !isvolatile(P)))
                    return false;
            }
        }
    }
    return true;
}
static void TemplateConstOrdering(SYMBOL** spList, int n, std::list<TEMPLATEPARAMPAIR>* params)
{
    int i;
    char hconst[200];
    char hvolatile[200];
    int count = 0;
    for (i = 0; i < n; i++)
    {
        if (spList[i])
        {
            int current = 0;
            std::list<TEMPLATEPARAMPAIR>* P = spList[i]->templateParams->front().second->bySpecialization.types;
            if (P)
            {
                for (auto&& pP : *P)
                {
                    if (current >= count)
                    {
                        hconst[current] = 0;
                        hvolatile[current] = 0;
                        count++;
                    }
                    if (pP.second->type == kw_typename)
                    {
                        TYPE* tp = pP.second->byClass.dflt;
                        if (tp)
                        {
                            while (isref(tp))
                                tp = basetype(tp)->btp;

                            if (isconst(tp))
                                hconst[current] = 1;
                            if (isvolatile(tp))
                                hvolatile[current] = 1;
                        }
                    }
                    current++;
                }
            }
        }
    }
    for (i = 0; i < n; i++)
        if (spList[i])
        {
            std::list<TEMPLATEPARAMPAIR>* P = spList[i]->templateParams->front().second->bySpecialization.types;
            std::list<TEMPLATEPARAMPAIR>* A = params;
            int current = 0;
            std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
            if (P && A)
            {
                auto itP = P->begin();
                auto itA = A->begin();
                auto iteA = A->end();
                for ( ;itP != P->end() && itA != iteA;)
                {
                    if (hconst[current] || hvolatile[current])
                    {
                        if (itA->second->packed && itA->second->byPack.pack)
                        {
                            tas.push(itA);
                            tas.push(iteA);
                            iteA = itA->second->byPack.pack->end();
                            itA = itA->second->byPack.pack->begin();
                        }
                        if (A && itP->second->type == kw_typename)
                        {
                            TYPE* ta = itA->second->byClass.dflt;
                            TYPE* tp = itP->second->byClass.dflt;
                            if (ta && tp)
                            {
                                while (isref(ta))
                                    ta = basetype(ta)->btp;
                                while (isref(tp))
                                    tp = basetype(tp)->btp;
                                if ((isconst(ta) && !isconst(tp) && hconst[current]) ||
                                    (isvolatile(ta) && !isvolatile(tp) && hvolatile[current]) ||
                                    !CheckConstCorrectness(tp, ta, true))
                                {
                                    spList[i] = 0;
                                    break;
                                }
                            }
                        }
                    }
                    current++;
                    if (itA != iteA)
                        ++itA;
                    ++itP;
                    if (itA == iteA && tas.size())
                    {
                        iteA = tas.top();
                        tas.pop();
                        itA = tas.top();
                        tas.pop();
                        ++itA;
                    }
                }
            }
        }
}
static bool TemplateConstMatchingInternal(std::list<TEMPLATEPARAMPAIR>* P)
{
    bool found = true;
    if (P)
    {
        for (auto pP : *P)
        {
            if (pP.second->type == kw_typename)
            {
                if (!pP.second->packed)
                {
                    TYPE* td = pP.second->byClass.dflt;
                    TYPE* tv = pP.second->byClass.val;
                    if (!tv && isstructured(pP.second->byClass.dflt) && basetype(pP.second->byClass.dflt)->sp->sb->templateLevel)
                    {
                        if (!TemplateConstMatchingInternal(basetype(pP.second->byClass.dflt)->sp->templateParams))
                        {
                            found = false;
                            break;
                        }
                    }
                    else if (!tv)
                    {
                        return false;
                    }
                    else
                    {
                        if (isref(td))
                            td = basetype(td)->btp;
                        if (isref(tv))
                            tv = basetype(tv)->btp;
                        if (td->type == bt_memberptr)
                            td = basetype(td)->btp;
                        if (tv->type == bt_memberptr)
                            tv = basetype(tv)->btp;
                        if ((isconst(td) != isconst(tv)) || ((isvolatile(td) != isvolatile(tv))) ||
                            !CheckConstCorrectness(td, tv, true))
                        {
                            found = false;
                            break;
                        }
                    }
                }
            }
        }
    }
    return found;
}
static void TemplateConstMatching(SYMBOL** spList, int n, std::list<TEMPLATEPARAMPAIR>* params)
{
    (void)params;
    int i;
    bool found = false;
    for (i = 0; i < n && !found; i++)
        if (spList[i])
        {
            std::list<TEMPLATEPARAMPAIR>* P;
            found = true;
            if (i == 0)
            {
                P = spList[i]->templateParams;
                if (P)
                {
                    for (auto pP : *P)
                    {
                        if (pP.second->type == kw_typename)
                        {
                            if (!pP.second->packed)
                            {
                                if (pP.second->byClass.val)
                                {
                                    TYPE* tv = pP.second->byClass.val;
                                    if (isref(tv))
                                        tv = basetype(tv)->btp;
                                    if (isconst(tv) || isvolatile(tv))
                                    {
                                        found = false;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                found = TemplateConstMatchingInternal(spList[i]->templateParams->front().second->bySpecialization.types);
            }
        }
    if (found)
    {
        for (i = 0; i < n; i++)
            if (spList[i])
            {
                std::list<TEMPLATEPARAMPAIR>* P;
                if (i == 0)
                {
                    P = spList[i]->templateParams;
                    if (P)
                    {
                        for (auto pP : *P)
                        {
                            if (pP.second->type == kw_typename)
                            {
                                if (!pP.second->packed)
                                {
                                    if (pP.second->byClass.val)
                                    {
                                        TYPE* tv = pP.second->byClass.val;
                                        if (isref(tv))
                                            tv = basetype(tv)->btp;
                                        if (isconst(tv) || isvolatile(tv))
                                        {
                                            spList[i] = 0;
                                        }
                                    }
                                    else
                                    {
                                        spList[i] = 0;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (!TemplateConstMatchingInternal(spList[i]->templateParams->front().second->bySpecialization.types))
                        spList[i] = 0;
                }
            }
    }
}
static void TransferClassTemplates(std::list<TEMPLATEPARAMPAIR>* dflt, std::list<TEMPLATEPARAMPAIR>* val, std::list<TEMPLATEPARAMPAIR>* params)
{
    if (!dflt || !val)
        return;
    auto itdflt = dflt->begin();
    auto itval = val->begin();
    if (itdflt->second->type == kw_new)
        ++itdflt;
    if (itval->second->type == kw_new)
        ++itval;
    bool ptr = false;
    TYPE *tdv = nullptr, *tdd = nullptr;
    TYPE *tvv = nullptr, *tvd = nullptr;
    if (itdflt != dflt->end() && itval != val->end() && itdflt->second->type == kw_typename && itdflt->second->byClass.val && itval->second->byClass.val && ispointer(itdflt->second->byClass.val) &&
        ispointer(itval->second->byClass.val))
    {
        tdv = itdflt->second->byClass.val;
        tdd = itdflt->second->byClass.dflt;
        tvv = itval->second->byClass.val;
        tvd = itval->second->byClass.dflt;
        ptr = true;
        while (ispointer(itdflt->second->byClass.val))
        {
            itdflt->second->byClass.val = basetype(itdflt->second->byClass.val)->btp;
            if (itdflt->second->byClass.dflt && ispointer(itdflt->second->byClass.dflt))
                itdflt->second->byClass.dflt = basetype(itdflt->second->byClass.dflt)->btp;
        }
        while (ispointer(itval->second->byClass.val))
        {
            itval->second->byClass.val = basetype(itval->second->byClass.val)->btp;
            if (itval->second->byClass.dflt && ispointer(itval->second->byClass.dflt))
                itval->second->byClass.dflt = basetype(itval->second->byClass.dflt)->btp;
        }
    }

    if (params && params->front().second->packed)
    {
        if (dflt)
        {
            std::list<TEMPLATEPARAMPAIR>* find = dflt;
            for (auto find : *dflt)
            {
                if (find.first && !strcmp(find.first->name, params->front().first->name))
                {
                    if (find.second->packed && find.second->type == params->front().second->type)
                    {
                        params->front().second->byPack.pack = find.second->byPack.pack;
                    }
                    break;
                }
            }
        }
    }
    else if (itval != val->end() && !itval->second->packed && itval->second->type == kw_typename && itval->second->byClass.dflt && itval->second->byClass.val &&
             itval->second->byClass.dflt->type == bt_templateparam)
    {
        if (!params->front().second->byClass.val && params->front().second->type == kw_typename)
            params->front().second->byClass.val = itval->second->byClass.val;
    }
    else if (itval != val->end() && !itval->second->packed && itval->second->type == kw_int && itval->second->byNonType.dflt &&
             itval->second->byNonType.val &&
             itval->second->byNonType.dflt->type == en_templateparam)
    {
        if (!params->front().second->byNonType.val && params->front().second->type == kw_int)
            params->front().second->byNonType.val = itval->second->byNonType.val;
    }
    else if (itval != val->end() && !itval->second->packed && itval->second->type == kw_typename && itval->second->byClass.dflt &&
             itval->second->byClass.val &&
             isstructured(itval->second->byClass.dflt) && isstructured(itval->second->byClass.val))
    {
        std::list<TEMPLATEPARAMPAIR>* tpdflt = basetype(itval->second->byClass.dflt)->sp->templateParams;
        std::list<TEMPLATEPARAMPAIR>* tpval = basetype(itval->second->byClass.val)->sp->templateParams;
        if (tpdflt && tpval)
        {
            auto ittpdflt = tpdflt->begin();
            auto ittpval = tpval->begin();
            for (; ittpdflt != tpdflt->end() && ittpval != tpval->end(); ++ ittpdflt, ++ittpval)
            {
                std::list<TEMPLATEPARAMPAIR> dflt{*ittpdflt}, val{*ittpval};
                TransferClassTemplates(&dflt, &val, params);
            }
        }
    }
    else if (itval != val->end() && !itval->second->packed && itval->second->type == kw_typename && itval->second->byClass.dflt &&
             itval->second->byClass.val &&
             isfunction(itval->second->byClass.dflt) && isfunction(itval->second->byClass.val))
    {
        TYPE* tpd = basetype(itval->second->byClass.dflt)->btp;
        TYPE* tpv = basetype(itval->second->byClass.val)->btp;
        if (tpd->type == bt_templateparam && tpd->templateParam->first)
        {
            if (params)
            {
                for (auto&& find : *params)
                {
                    if (find.first && !strcmp(find.first->name, tpd->templateParam->first->name))
                    {
                        if (tpd->templateParam->second->packed && !find.second->byPack.pack)
                        {
                            if (tpv->type == bt_templateparam)
                            {
                                find.second->byPack.pack = tpv->templateParam->second->byPack.pack;
                            }
                            else
                            {
                                find.second->byPack.pack = templateParamPairListFactory.CreateList();
                                find.second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                find.second->byPack.pack->back().second->type = kw_typename;
                                find.second->byPack.pack->back().second->byClass.val = tpv;
                            }
                        }
                        if (!find.second->byClass.val)
                            find.second->byClass.val =
                                tpv->type == bt_templateparam ? tpv->templateParam->second->byClass.val : tpv;
                        break;
                    }
                }
            }
        }
        auto hrd = basetype(itval->second->byClass.dflt)->syms->begin();
        auto hrde = basetype(itval->second->byClass.dflt)->syms->end();
        auto hrv = basetype(itval->second->byClass.val)->syms->begin();
        auto hrve = basetype(itval->second->byClass.val)->syms->end();
        while (hrd != hrde && hrv != hrve)
        {
            tpd = (*hrd)->tp;
            tpv = (*hrv)->tp;
            if (tpd->type == bt_templateparam && tpd->templateParam->first)
            {
                std::list<TEMPLATEPARAMPAIR>* find = params;
                if (params)
                {
                    for (auto&& find : *params)
                    {
                        if (find.first && !strcmp(find.first->name, tpd->templateParam->first->name))
                        {
                            if (tpd->templateParam->second->packed)
                            {
                                if (!find.second->byPack.pack)
                                {
                                    if (tpv->type == bt_templateparam)
                                    {
                                        find.second->byPack.pack = tpv->templateParam->second->byPack.pack;
                                        ++hrd;
                                    }
                                    else
                                    {
                                        find.second->byPack.pack = templateParamPairListFactory.CreateList();
                                        find.second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                        find.second->byPack.pack->back().second->type = kw_typename;
                                        find.second->byPack.pack->back().second->byClass.val = tpv;
                                    }
                                }
                                else
                                {
                                    auto next = find.second->byPack.pack;
                                    next->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                    next->back().second->type = kw_typename;
                                    next->back().second->byClass.val = tpv;
                                }
                            }
                            else
                            {
                                ++hrd;
                            }
                            if (!find.second->byClass.val)
                                find.second->byClass.val =
                                    tpv->type == bt_templateparam ? tpv->templateParam->second->byClass.val : tpv;
                            break;
                        }
                    }
                }
            }
            else
            {
                ++hrd;
            }
            ++hrv;
        }
    }
    // here to support return types, templateselectors would otherwise be resolved by now
    else
    {
        if (itdflt != dflt->end() && itdflt->first && params && !params->front().second->byNonType.val)
        {
            if (params->front().second->type == kw_int)
            {
                for (auto param1 : *dflt)
                {
                    if (param1.second->type == kw_int && params->front().second->type == kw_int && param1.second->byNonType.dflt &&
                        param1.second->byNonType.dflt->type == en_templateparam)
                    {
                        if (!strcmp(params->front().first->name,
                                    param1.second->byNonType.dflt->v.sp->tp->templateParam->first->name))
                        {
                            params->front().second->deduced = true;
                            params->front().second->byNonType.val = param1.second->byNonType.val;
                            return;
                        }
                    }
                }
            }
            for (auto param : *params)
            {
                if (param.first && !strcmp(itdflt->first->name, param.first->name))
                {
                    if (itdflt->second->type == param.second->type && itdflt->second->packed == param.second->packed)
                    {
                        if (itdflt->second->packed && !param.second->byPack.pack)
                            param.second->byPack.pack = itval->second->byPack.pack;
                        if (!param.second->byClass.val)
                            param.second->byClass.val = itval->second->byClass.val;
                    }
                    break;
                }
            }
        }
        if (params && params->front().second->type == kw_typename && !params->front().second->packed && params->front().second->byClass.dflt &&
            basetype(params->front().second->byClass.dflt)->type == bt_templateselector &&
            (*basetype(params->front().second->byClass.dflt)->sp->sb->templateSelector)[1].isTemplate)
        {
            std::list<TEMPLATEPARAMPAIR>* param1 = (*basetype(params->front().second->byClass.dflt)->sp->sb->templateSelector)[1].templateParams;
            if (param1)
            {
                for (auto&& param : *param1)
                {
                    std::list<TEMPLATEPARAMPAIR> a{param};
                    TransferClassTemplates(dflt, val, &a);
                }
            }
        }
    }
    if (ptr)
    {
        itdflt->second->byClass.val = tdv;
        itdflt->second->byClass.dflt = tdd;
        itval->second->byClass.val = tvv;
        itval->second->byClass.dflt = tvd;
    }
}
static SYMBOL* ValidateClassTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* unspecialized, std::list<TEMPLATEPARAMPAIR>* args)
{
    (void)unspecialized;
    SYMBOL* rv = nullptr;
    std::list<TEMPLATEPARAMPAIR>* nparams = sp->templateParams;
    if (nparams)
    {
        std::list<TEMPLATEPARAMPAIR>* spsyms = nparams->front().second->bySpecialization.types;
        std::list<TEMPLATEPARAMPAIR>* params = spsyms ? spsyms : nparams, *origParams = params;
        std::list<TEMPLATEPARAMPAIR>* primary = spsyms ? spsyms : nparams;
        std::list<TEMPLATEPARAMPAIR>* initial = args;
        std::list<TEMPLATEPARAMPAIR>* max = nparams->size() > 1 ? nparams : spsyms;
        rv = sp;
        if (!spsyms)
        {
            ClearArgValues(params, sp->sb->specialized);
        }
        ClearArgValues(spsyms, sp->sb->specialized);
        ClearArgValues(sp->templateParams, sp->sb->specialized);
        if (args)
        {
            for (auto a : *args)
            {
                if (a.second->type == kw_template && a.second->byTemplate.dflt && a.second->byTemplate.dflt->sb)
                    ClearArgValues(a.second->byTemplate.dflt->templateParams, a.second->byTemplate.dflt->sb->specialized);
            }
        }
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tis;
        std::list<TEMPLATEPARAMPAIR>::iterator itInitial, iteInitial;
        if (initial)
        {
            itInitial = initial->begin();
            iteInitial = initial->end();
        }
        auto itParams = params->begin();
        std::list<TEMPLATEPARAMPAIR>::iterator itmax;
        if (max)
            itmax = max->begin();
        auto itPrimary = primary->begin();
        if (itParams->second->type == kw_new)
            ++itParams;
        if (max && itmax->second->type == kw_new)
            ++itmax;
        if (itPrimary->second->type == kw_new)
            ++itPrimary;
        for (; itInitial != iteInitial && itParams != params->end();)
        {
            if (itInitial->second->packed)
            {
                tis.push(itInitial);
                tis.push(iteInitial);
                if (itInitial->second->byPack.pack)
                {
                    iteInitial = itInitial->second->byPack.pack->end();
                    itInitial = itInitial->second->byPack.pack->begin();
                }
                else
                {
                    iteInitial = std::list<TEMPLATEPARAMPAIR>::iterator();
                    itInitial = iteInitial;
                }
                auto it = itParams;
                ++it;
                if (itInitial == iteInitial && itParams->second->packed && it != params->end())
                    ++itParams;
            }
            if (itInitial != iteInitial && itParams != params->end())
            {
                if (itParams->second->packed)
                {
                    auto it = itParams;
                    ++it;
                    while (itInitial != iteInitial && (!itInitial->second->packed || itParams == params->end()) &&
                           itParams->second->type == itInitial->second->type)
                    {
                        void* dflt;
                        dflt = itInitial->second->byClass.val;
                        if (!dflt)
                            dflt = itInitial->second->byClass.dflt;
                        if (dflt)
                        {
                            if (!itParams->second->byPack.pack)
                                itParams->second->byPack.pack = templateParamPairListFactory.CreateList();
                            auto last = itParams->second->byPack.pack;
                            last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            last->back().second->type = itParams->second->type;
                            last->back().second->byClass.val = (TYPE*)dflt;
                            if (itParams->second->type == kw_int)
                            {
                                last->back().second->byNonType.tp = itParams->second->byNonType.tp;
                            }
                            itParams->second->initialized = true;
                        }
                        ++itInitial;
                        auto it = itParams;
                        ++it;
                        if (itInitial == iteInitial && it == params->end() && !tis.empty())
                        {
                            iteInitial = tis.top();
                            tis.pop();
                            itInitial = tis.top();
                            tis.pop();
                            ++itInitial;
                        }
                        if (itInitial != iteInitial && itInitial->second->packed && itInitial->second->byPack.pack)
                        {
                            tis.push(itInitial);
                            tis.push(iteInitial);
                            iteInitial = itInitial->second->byPack.pack->end();
                            itInitial = itInitial->second->byPack.pack->begin();
                        }
                        if (itmax != max->end())
                            ++itmax;
                    }
                    auto it1 = itParams;
                    ++it1;
                    if (it1 != params->end())
                    {
                        ++itParams;
                    }
                    if (itInitial != iteInitial && tis.size())
                    {
                        rv = nullptr;
                        break;
                    }
                }
                else if (itInitial->second->type != itParams->second->type)
                {
                    if (itInitial->second->type == kw_typename && itParams->second->type == kw_template)
                    {
                        void* dflt;
                        dflt = itInitial->second->byClass.val;
                        if (!dflt)
                            dflt = itInitial->second->byClass.dflt;
                        if (dflt && isstructured((TYPE*)dflt))
                        {
                            if (!DeduceTemplateParam(&*itParams, nullptr, (TYPE*)dflt, nullptr, true))
                            {
                                rv = nullptr;
                            }

                            itParams->second->initialized = true;
                            ++itParams;
                            ++itPrimary;
                            ++itInitial;
                            if (itmax != max->end())
                                ++itmax;
                        }
                        else
                        {
                            rv = nullptr;
                            break;
                        }
                    }
                    else if (itInitial->second->type == kw_template && itParams->second->type == kw_typename)
                    {
                        void* dflt;
                        dflt = itInitial->second->byTemplate.val;
                        if (!dflt)
                            dflt = itInitial->second->byTemplate.dflt;
                        if (dflt)
                        {
                            itParams->second->byClass.val = ((SYMBOL*)dflt)->tp;
                            itParams->second->initialized = true;
                            ++itParams;
                            ++itPrimary;
                            ++itInitial;
                            if (itmax != max->end())
                                ++itmax;
                        }
                        else
                        {
                            rv = nullptr;
                            break;
                        }
                    }
                    else
                    {
                        rv = nullptr;
                        break;
                    }
                }
                else
                {
                    void* dflt;
                    dflt = itInitial->second->byClass.val;
                    if (!dflt)
                        dflt = itInitial->second->byClass.dflt;
                    if (itInitial->second->type == kw_template)
                    {
                        if (dflt && !exactMatchOnTemplateParams(((SYMBOL*)dflt)->templateParams, itParams->second->byTemplate.args))
                            rv = nullptr;
                    }
                    if (itParams->second->byClass.val)
                    {
                        switch (itInitial->second->type)
                        {
                            case kw_typename:
                                if (!templatecomparetypes(itParams->second->byClass.val, (TYPE*)dflt, true) || (isstructured(itParams->second->byClass.val) && basetype(itParams->second->byClass.val)->sp->sb->templateLevel && 
                                    !sameTemplate(itParams->second->byClass.val, (TYPE*)dflt, true)))
                                    rv = nullptr;
                                break;
                            case kw_int: {
                                EXPRESSION* exp = copy_expression(itParams->second->byNonType.val);
                                optimize_for_constants(&exp);
                                if (itParams->second->byNonType.val && !equalTemplateIntNode(exp, (EXPRESSION*)dflt))
                                    rv = nullptr;
                            }
                            break;
                            default:
                                break;
                        }
                    }
                    if (!max && itParams->second->byClass.dflt && itParams->second->byClass.dflt->type == bt_templateselector)
                    {
                        auto temp = ResolveDeclType(sp, &*itParams);
                        temp = ResolveTemplateSelectors(sp, temp, false);
                        itParams->second->byClass.val = temp->front().second->byClass.dflt;
                        ++itParams;
                    }
                    else
                    {
                        itParams->second->byClass.val = (TYPE*)dflt;
                        if (spsyms)
                        {
                            if (itParams->second->type == kw_typename)
                            {
                                if (itParams->second->byClass.dflt &&
                                    !Deduce(itParams->second->byClass.dflt, itParams->second->byClass.val, nullptr, true, true,
                                            false, false))
                                {
                                    rv = nullptr;
                                }
                                else if (nparams->size() > 1)
                                {
                                    auto it = nparams->begin();
                                    ++it;
                                    std::list<TEMPLATEPARAMPAIR> a{*itParams}, b(it, nparams->end());
                                    TransferClassTemplates(&a, &a, &b);
                                }
                            }
                            else if (itParams->second->type == kw_template)
                            {
                                if (itParams->second->byClass.dflt->type == bt_templateparam)
                                {
                                    if (!DeduceTemplateParam(itParams->second->byClass.dflt->templateParam, nullptr,
                                                             itParams->second->byTemplate.dflt->tp, nullptr, true))
                                        rv = nullptr;
                                }
                                else
                                {
                                    rv = nullptr;
                                }
                            }
                            else if (itParams->second->type == kw_int)
                            {
                                EXPRESSION* exp = itParams->second->byNonType.val;
                                if (exp && !isintconst(exp))
                                {
                                    exp = copy_expression(exp);
                                    optimize_for_constants(&exp);
                                }
                                if (exp && itParams->second->byNonType.dflt && itParams->second->byNonType.dflt->type != en_templateparam &&
                                    !equalTemplateIntNode(itParams->second->byNonType.dflt, exp))
                                    rv = nullptr;
                            }
                        }
                    }
                    itParams->second->initialized = true;
                    ++itParams;
                    ++itPrimary;
                    ++itInitial;
                    if (itmax != max->end())
                        ++itmax;
                }
            }
            if ((itInitial == iteInitial || (itParams != params->end() && itInitial->second->type != itParams->second->type)) &&
                tis.size())
            {
                iteInitial = tis.top();
                tis.pop();
                itInitial = tis.top();
                tis.pop();
                ++itInitial;
            }
        }
        if (itInitial != iteInitial && itInitial->second->packed && (!itInitial->second->byPack.pack || !itInitial->second->byPack.pack->size()))
            itInitial = iteInitial;
        if (itInitial != iteInitial && (max && itmax != max->end() || !spsyms))
            rv = nullptr;
        if (spsyms)
        {
            for (auto its = itParams; its != params->end(); ++its)
            {
                auto&& primary = *its;
                if (primary.second->type == kw_typename)
                {
                    auto temp = ResolveDeclType(sp, &primary);
                    temp = ResolveTemplateSelectors(sp, temp, false);
                    if (primary.second->byClass.dflt != temp->front().second->byClass.dflt)
                    {
                        primary.second->byClass.val = temp->front().second->byClass.dflt;
                    }
                }
            }
        }
        bool found = false;
        if (itParams != params->end())
        {
            for (auto its = itParams; its != params->end(); ++its)
            {      
                if ((*its).second->usedAsUnpacked)
                {
                    found = true;
                    break;
                }
            }
        }
        if ((!templateNestingCount || instantiatingTemplate || (inTemplateHeader && templateNestingCount == 1)) &&
            (inTemplateArgs < 1 || !found))
        {
            primary = spsyms ? spsyms : nparams;
            if (!TemplateParseDefaultArgs(sp, args, origParams, primary, primary))
                rv = nullptr;
            if (spsyms && params)
            {
                auto iteParams = params->end();
                for ( ; itParams != iteParams; )
                {
                    if (itParams->second->packed)
                    {
                        if (!itParams->second->byPack.pack)
                        {
                            ++itParams;
                            continue;
                        }
                        else
                        {
                            tis.push(itParams);
                            tis.push(iteParams);
                            iteParams = itParams->second->byPack.pack->end();
                            itParams = itParams->second->byPack.pack->begin();
                        }
                    }
                    if (itParams->second->byClass.val && itParams->second->byClass.dflt)
                    {
                        switch (itParams->second->type)
                        {
                            case kw_typename:
                                if (itParams->second->byClass.dflt->type != bt_templateparam &&
                                    itParams->second->byClass.dflt->type != bt_templateselector &&
                                    itParams->second->byClass.dflt->type != bt_templatedecltype &&
                                    !templatecomparetypes(itParams->second->byClass.val, itParams->second->byClass.dflt, true))
                                    rv = nullptr;
                                break;
                            case kw_int: {
                                EXPRESSION* exp = copy_expression(itParams->second->byNonType.val);
                                optimize_for_constants(&exp);
                                if (itParams->second->byNonType.dflt && !equalTemplateIntNode(exp, itParams->second->byNonType.dflt))
                                    rv = nullptr;
                            }
                            break;
                            default:
                                break;
                        }
                    }
                    ++itParams;
                    if (itParams == iteParams && !tis.empty())
                    {
                        iteParams = tis.top();
                        tis.pop();
                        itParams = tis.top();
                        tis.pop();
                        ++itParams;
                    }
                }
            }
            params = args;
            std::list<TEMPLATEPARAMPAIR>::iterator iteParams;
            if (params)
            {
                itParams = params->begin();
                iteParams = params->end();
            }
            else
            {
                itParams = iteParams;
            }
            itPrimary = primary->begin();
            if (itPrimary->second->type == kw_new)
                ++itPrimary;
            for ( ; itParams != iteParams && itPrimary != primary->end(); ++itParams, ++itPrimary)
            {
                if (!itPrimary->second->byClass.val && !itPrimary->second->packed)
                {
                    rv = nullptr;
                    break;
                }
                auto it = itPrimary;
                ++it;
                if (it == primary->end() && itPrimary->second->packed)
                    break;
            }
            if (itParams != iteParams && !itParams->second->packed && !itParams->second->byPack.pack && itPrimary == primary->end())
            {
                rv = nullptr;
            }
        }
        else if (inTemplateSpecialization)
        {
            bool packed = false;
            params = origParams;
            itParams = params->begin();
            if (itParams->second->type == kw_new)
                ++itParams;
            auto itArgs = args->begin();
            for (; itParams != params->end() && itArgs != args->end(); ++ itParams, ++itArgs)
            {
                if (itParams->second->packed)
                    packed = true;
                switch (itParams->second->type)
                {
                    case kw_typename:
                        if (itParams->second->byClass.dflt && !itParams->second->packed && itParams->second->byClass.dflt->type != bt_templateparam &&
                            (!itArgs->second->byClass.dflt || !templatecomparetypes(itParams->second->byClass.dflt, itArgs->second->byClass.dflt, true)))
                            rv = nullptr;
                        break;
                    case kw_int: {
                        if (itParams->second->byNonType.dflt &&
                            (!itArgs->second->byNonType.dflt ||
                             !templatecomparetypes(itParams->second->byNonType.tp, itArgs->second->byNonType.tp, true)))
                            rv = nullptr;
                    }
                    break;
                    default:
                        break;
                }
            }
            if (itParams != params->end())
            {
                if (itParams->second->packed || !itParams->second->byClass.txtdflt || (spsyms && itParams->second->byClass.dflt))
                    rv = nullptr;
            }
            else if (itArgs != args->end() && !packed)
            {
                rv = nullptr;
            }
        }
        else
        {
            bool packed = false;
            params = origParams;
            itParams = params->begin();
            if (itParams->second->type == kw_new)
                ++itParams;
            std::list<TEMPLATEPARAMPAIR>::iterator itArgs;
            if (args)
                itArgs = args->begin();
            for (; itParams != params->end() && args && itArgs != args->end(); ++itParams, ++itArgs)
            {
                if (itParams->second->packed)
                    packed = true;
            }
            if (itParams != params->end())
            {
                if (itParams->second->packed || !itParams->second->byClass.txtdflt || (spsyms && itParams->second->byClass.dflt))
                    rv = nullptr;
            }
            else if (args && itArgs != args->end() && !packed)
            {
                rv = nullptr;
            }
        }
    }
    return rv;
}
static bool checkArgType(TYPE* tp, bool checkDeduced, bool checkDeclaring)
{
    while (ispointer(tp) || isref(tp))
        tp = basetype(tp)->btp;
    if (isfunction(tp))
    {
        SYMBOL* sym = basetype(tp)->sp;
        if (!checkArgType(basetype(tp)->btp, checkDeduced, checkDeclaring))
            return false;
        if (sym->tp->syms)
        {
            for (auto sp1 : *sym->tp->syms)
            {
                if (!checkArgType(sp1->tp, checkDeduced, checkDeclaring))
                    return false;
            }
        }
    }
    else if (isstructured(tp))
    {
        if (basetype(tp)->sp->sb)
        {
            if (basetype(tp)->sp->sb->instantiated && basetype(tp)->sp->sb->attribs.inheritable.linkage4 == lk_virtual)
                return true;
            if (basetype(tp)->sp->sb->templateLevel)
            {
                return allTemplateArgsSpecified(basetype(tp)->sp, basetype(tp)->sp->templateParams, checkDeduced,
                                                checkDeclaring);
            }
            if (checkDeclaring && basetype(tp)->sp->sb->declaringRecursive)
                return false;
        }
    }
    else if (basetype(tp)->type == bt_templateparam)
    {
        if (!basetype(tp)->templateParam->second->byClass.val)
            return false;
        if (basetype(tp)->templateParam->second->byClass.val->type == bt_templateparam)
            return false;
    }
    else if (basetype(tp)->type == bt_templatedecltype)
        return false;
    else if (basetype(tp)->type == bt_templateselector)
    {
        return false;
    }
    else if (basetype(tp)->type == bt_memberptr)
    {
        if (!checkArgType(basetype(tp)->sp->tp, checkDeduced, checkDeclaring))
            return false;
        if (!checkArgType(basetype(tp)->btp, checkDeduced, checkDeclaring))
            return false;
    }
    return true;
}
static bool checkArgSpecified(TEMPLATEPARAMPAIR* arg, bool checkDeduced, bool checkDeclaring)
{
    if (!arg->second->byClass.val)
        return false;
    switch (arg->second->type)
    {
        case kw_int:
            if (arg->second->byNonType.val && !isarithmeticconst(arg->second->byNonType.val))
            {
                EXPRESSION* exp = copy_expression(arg->second->byNonType.val);
                optimize_for_constants(&exp);
                std::stack<EXPRESSION*> working;
                while (exp)
                {
                    if (exp->left)
                        working.push(exp->left);
                    if (exp->right)
                        working.push(exp->right);
                    while (castvalue(exp) || lvalue(exp))
                        exp = exp->left;
                    if (!isarithmeticconst(exp))
                    {
                        switch (exp->type)
                        {
                            case en_pc:
                            case en_global:
                            case en_func:
                            case en_void:
                                break;
                            default:
                                return false;
                                break;
                        }
                    }
                    if (!working.empty())
                    {
                        exp = working.top();
                        working.pop();
                    }
                    else
                    {
                        exp = nullptr;
                    }
                }
            }
            break;
        case kw_template: {
            return true;
        }
        case kw_typename: {
            return checkArgType(arg->second->byClass.val, checkDeduced, checkDeclaring);
        }
        default:
            break;
    }
    return true;
}
bool allTemplateArgsSpecified(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool checkDeduced, bool checkDeclaring)
{
    if (!args)
        return true;
    for (auto it = args->begin(); it != args->end(); ++it)
    {
        if (it->second->type != kw_new)
        {
            if (it->second->packed)
            {
                if ((templateNestingCount && !instantiatingTemplate && (!it->second->byPack.pack || !it->second->byPack.pack->size())) ||
                    !allTemplateArgsSpecified(sym, it->second->byPack.pack, checkDeduced, checkDeclaring))
                    return false;
            }
            else
            {
                if (sym)
                {
                    if (it->second->type == kw_typename)
                    {
                        TYPE* tp = it->second->byClass.val;
                        if (tp && basetype(tp)->type == bt_any)
                            return false;
                        if (sameTemplate(tp, sym->tp))
                            return false;
                    }
                }
                if (!checkArgSpecified(&*it, checkDeduced, checkDeclaring))
                {
                    return false;
                }
            }
            if (checkDeduced && !it->second->deduced && !it->second->initialized)
            {
                return false;
            }
        }
    }
    return true;
}
void TemplateArgsAdd(TEMPLATEPARAMPAIR* current, TEMPLATEPARAMPAIR* dflt, std::list<TEMPLATEPARAMPAIR>* basetpl)
{
    if (basetpl)
    {
        for (auto base : *basetpl)
        {
            if ((base.second->packed || !base.second->byClass.val) && base.first && dflt->first &&
                !strcmp(base.first->name, dflt->first->name))
            {
                if (base.second->packed == current->second->packed)
                {
                    if (base.second->packed)
                        base.second->byPack.pack = current->second->byPack.pack;
                    else
                        base.second->byClass.val = current->second->byClass.val;
                }
                else if (!current->second->packed && current->second->byClass.val)
                {
                    if (base.second->byPack.pack)
                    {
                        /*
                        std::list<TEMPLATEPARAMPAIR>* last = base.second->byPack.pack;
                        std::list<TEMPLATEPARAMPAIR>* cur = current;
                        auto itl = last->begin();
                        auto itc = cur->begin();
                        for (;itl != last->end() && itc != cur->end(); ++itl, ++itc)
                        {
                            itl->second->byClass.val = itc->second->byClass.val;
                        }
                        */
                    }
                    else
                    {
                        base.second->byPack.pack = templateParamPairListFactory.CreateList();

                        base.second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                        *base.second->byPack.pack->back().second = *current->second;
                    }
                }
            }
        }
    }
}
void TemplateArgsTemplateAdd(TEMPLATEPARAMPAIR* current, TEMPLATEPARAMPAIR* special, std::list<TEMPLATEPARAMPAIR>* base)
{
    // first move the template instance
    if (base)
    {
        for (auto tpb : *base)
        {
            if (!tpb.second->byTemplate.val && tpb.first && current->second->byTemplate.val &&
                !strcmp(tpb.first->name, current->second->byTemplate.dflt->name))
            {
                if (tpb.second->packed)
                    tpb.second->byPack.pack = current->second->byPack.pack;
                else
                    tpb.second->byTemplate.val = current->second->byTemplate.val;
            }
        }
    }
    // now move any arguments...
    auto tpx = current->second->byTemplate.val->templateParams;
    auto spl = special->second->byTemplate.args;
    auto itt = tpx->begin();
    auto its = spl->begin();
    for (++itt; itt != tpx->end() && its != spl->end(); ++itt, ++its)
    {
        TemplateArgsAdd(&*itt, &*its, base);
    }
}
void TemplateArgsScan(std::list<TEMPLATEPARAMPAIR>* currents, std::list<TEMPLATEPARAMPAIR>* base)
{
    if (currents)
    {
        for (auto&& current : *currents)
        {
            if (current.first && current.second->byClass.val)
            {
                TemplateArgsAdd(&current, &current, base);
            }
            if (current.second->type == kw_template)
            {
                if (current.second->byTemplate.val && base->front().second->bySpecialization.types)
                {
                    for (auto&& tpx : *base->front().second->bySpecialization.types)
                    {
                        if (tpx.second->type == kw_template && tpx.second->byTemplate.dflt &&
                            !strcmp(tpx.second->byTemplate.dflt->name, current.second->byTemplate.dflt->name))
                        {
                            TemplateArgsTemplateAdd(&current, &tpx, base);
                            break;
                        }
                    }
                }
            }
            else if (current.second->type == kw_typename)
            {
                if (current.second->byClass.val)
                {
                    if (current.second->byClass.dflt && isstructured(current.second->byClass.dflt))
                    {
                        if (isstructured(current.second->byClass.val))
                        {
                            auto tpv = basetype(current.second->byClass.dflt)->sp->templateParams;
                            auto tpx = basetype(current.second->byClass.val)->sp->templateParams;
                            auto itv = tpv->begin();
                            auto itl = tpx->begin();
                            for (; itv != tpv->end() && itl != tpx->end(); ++itv, ++itl)
                            {
                                TemplateArgsAdd(&*itl, &*itv, base);
                            }
                        }
                    }
                    else if (isstructured(current.second->byClass.val))
                    {
                        if (basetype(current.second->byClass.val)->sp->templateParams)
                            for (auto&& tpx : *basetype(current.second->byClass.val)->sp->templateParams)
                            {
                                TemplateArgsAdd(&tpx, &tpx, base);
                            }
                    }
                    else if (current.second->byClass.val->type == bt_templateselector)
                    {
                        TemplateArgsScan((*current.second->byClass.val->sp->sb->templateSelector)[1].templateParams, base);
                    }
                }
            }
        }
    }
}
void TemplateArgsCopy(std::list<TEMPLATEPARAMPAIR>* base)
{
    if (base->front().second->bySpecialization.types)
    {
        for (auto&& p : *base)
        {
            if (p.second->type != kw_new && !p.second->byClass.val)
            {
                TemplateArgsScan(base->front().second->bySpecialization.types, base);
                break;
            }
        }
    }
}
void DuplicateTemplateParamList(std::list<TEMPLATEPARAMPAIR>** pptr)
{
    if (*pptr)
    {
        std::list<TEMPLATEPARAMPAIR>* params1 = *pptr;
        *pptr = nullptr;
        for (auto param : *params1)
        {
            if (!*pptr)
                *pptr = templateParamPairListFactory.CreateList();
            (*pptr)->push_back(TEMPLATEPARAMPAIR{nullptr, nullptr});
            if (param.second->type == kw_typename)
            {
                (*pptr)->back().second = Allocate<TEMPLATEPARAM>();
                *(*pptr)->back().second = *param.second;
                if (param.second->packed)
                {
                    DuplicateTemplateParamList(&(*pptr)->back().second->byPack.pack);
                }
                else
                {
                    (*pptr)->back().second->byClass.dflt = SynthesizeType(param.second->byClass.val, nullptr, false);
                }
            }
            else
            {
                (*pptr)->back().second = param.second;
            }
        }
    }
}

static bool constOnly(SYMBOL** spList, SYMBOL** origList, int n)
{
    int i;
    for (i = 1; i < n; i++)
    {
        if (spList[i])
        {
            std::list<TEMPLATEPARAMPAIR>* tpx = origList[i]->templateParams->front().second->bySpecialization.types;
            bool found = false;
            if (tpx)
            {
                for (auto tpx1 : *tpx)
                {
                    if (tpx1.second->type == kw_typename && tpx1.second->byClass.dflt)
                    {
                        TYPE* tp1 = tpx1.second->byClass.dflt;
                        if (isconst(tp1) || isvolatile(tp1))
                        {
                            found = true;
                            break;
                        }
                    }
                }
            }
            if (!found)
                return false;
        }
    }
    return true;
}
static int SpecializationComplexity(std::list<TEMPLATEPARAMPAIR>* tpx)
{
    int count = 0;
    if (tpx)
    {
        for (auto tpl : *tpx)
        {
            if (tpl.second->type == kw_typename)
            {
                TYPE* tp = tpl.second->byClass.dflt;
                if (tpl.second->packed)
                {
                    count += 1 + SpecializationComplexity(tpl.second->byPack.pack);
                }
                else
                {
                    while (tp && tp->btp)
                    {
                        if (tp->type != bt_typedef)
                            count++;
                        tp = tp->btp;
                    }
                    if (tp && isstructured(tp))
                        count += 1 + SpecializationComplexity(tp->sp->templateParams);
                }
            }
            else if (tpl.second->type == kw_int)
            {
                EXPRESSION* exp = tpl.second->byNonType.dflt;
                if (exp)
                {
                    std::stack<EXPRESSION*> working;
                    working.push(exp);
                    while (working.size())
                    {
                        exp = working.top();
                        working.pop();
                        count++;
                        if (exp->type == en_templateselector)
                        {
                            TEMPLATESELECTOR* ts = &(*exp->v.templateSelector)[1];
                            count += SpecializationComplexity(ts->templateParams);
                            count += exp->v.templateSelector->size() - 1;
                        }
                        if (exp->left)
                        {
                            working.push(exp->left);
                        }
                        if (exp->right)
                        {
                            working.push(exp->right);
                        }
                    }
                }
            }
        }
    }
    return count;
}
static int MoreSpecialized(SYMBOL* left, SYMBOL* right)
{
    std::list<TEMPLATEPARAMPAIR>* pl = left->templateParams->front().second->bySpecialization.types;
    std::list<TEMPLATEPARAMPAIR>* pr = right->templateParams->front().second->bySpecialization.types;
    int il = SpecializationComplexity(pl);
    int ir = SpecializationComplexity(pr);
    if (il < ir)
        return 1;
    if (il > ir)
        return -1;
    return 0;
}
static bool LessParams(SYMBOL* left, SYMBOL* right)
{
    std::list<TEMPLATEPARAMPAIR>* pl = left->templateParams->front().second->bySpecialization.types;
    std::list<TEMPLATEPARAMPAIR>* pr = right->templateParams->front().second->bySpecialization.types;
    int il = pl ? pl->size() : 0;
    int ir = pr ? pr->size() : 0;
    return il < ir;
}
static void ChooseMoreSpecialized(SYMBOL** list, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            if (list[i] && list[j])
            {
                int which = MoreSpecialized(list[i], list[j]);
                if (which < 0)
                    list[j] = nullptr;
                else if (which > 0)
                    list[i] = nullptr;
                else
                {
                    if (LessParams(list[i], list[j]))
                        list[i] = nullptr;
                    else
                        list[j] = nullptr;
                }
            }
        }
    }
}
static SYMBOL* FindTemplateSelector(std::vector<TEMPLATESELECTOR>* tso)
{
    if (!templateNestingCount)
    {
        SYMBOL* ts = (*tso)[1].sp;
        SYMBOL* sp = nullptr;
        TYPE* tp;

        if (ts && ts->sb && ts->sb->instantiated)
        {
            sp = ts;
        }
        else
        {
            auto tp = ts->tp;
            if (basetype(ts->tp)->type == bt_templateparam && basetype(ts->tp)->templateParam->second->type == kw_typename)
            {
                tp = basetype(ts->tp)->templateParam->second->byClass.val;
            }
            if (!tp || !isstructured(tp))
            {
                sp = nullptr;
            }
            else
            {
                ts = basetype(tp)->sp;
                if ((*tso)[1].isTemplate)
                {
                    if ((*tso)[1].templateParams)
                    {
                        std::list<TEMPLATEPARAMPAIR>* currentx = (*tso)[1].templateParams;
                        std::deque<TYPE*> types;
                        std::deque<EXPRESSION*> expressions;
                        for (auto &&current : *currentx)
                        {
                            if (current.second->type == kw_typename)
                            {
                                types.push_back(current.second->byClass.dflt);
                                if (current.second->byClass.val)
                                    current.second->byClass.dflt = current.second->byClass.val;
                            }
                            else if (current.second->type == kw_int)
                            {
                                expressions.push_back(current.second->byNonType.dflt);
                                if (current.second->byNonType.val)
                                    current.second->byNonType.dflt = current.second->byNonType.val;
                            }
                        }
                        sp = GetClassTemplate(ts, (*tso)[1].templateParams, false);
                        tp = nullptr;
                        if (sp)
                            sp = TemplateClassInstantiateInternal(sp, (*tso)[1].templateParams, false);
                        for (auto&& current : *currentx)
                        {
                            if (current.second->type == kw_typename)
                            {
                                if (types.size())
                                {
                                    current.second->byClass.dflt = types.front();
                                    types.pop_front();
                                }
                            }
                            else if (current.second->type == kw_int)
                            {
                                if (expressions.size())
                                {
                                    current.second->byNonType.dflt = expressions.front();
                                    expressions.pop_front();
                                }
                            }
                        }
                    }
                    else
                    {
                        sp = nullptr;
                    }
                }
                else if (basetype(ts->tp)->type == bt_templateselector)
                {
                    sp = nullptr;
                }
                else if (isstructured(ts->tp))
                {
                    sp = ts;
                }
            }
        }
        if (sp)
        {
            sp = basetype(PerformDeferredInitialization(sp->tp, nullptr))->sp;
            if ((sp->sb->templateLevel == 0 || sp->sb->instantiated) &&
                (!sp->templateParams || allTemplateArgsSpecified(sp, sp->templateParams)))
            {
                auto find = tso->begin();
                ++find;
                ++find;
                for ( ; find != tso->end() && sp ; ++find)
                {
                    SYMBOL* spo = sp;
                    if (!isstructured(spo->tp))
                        break;

                    sp = search(spo->tp->syms, find->name);
                    if (!sp)
                    {
                        sp = classdata(find->name, spo, nullptr, false, false);
                        if (sp == (SYMBOL*)-1)
                            sp = nullptr;
                        if (sp && find->isTemplate)
                        {
                            sp = GetClassTemplate(sp, find->templateParams, theCurrentFunc);
                            if (sp)
                                sp->tp = PerformDeferredInitialization(sp->tp, theCurrentFunc);
                        }
                    }
                    if (sp && sp->sb->access != ac_public && !resolvingStructDeclarations)
                    {
                        sp = nullptr;
                        break;
                    }
                }
                if (sp && find == tso->end())
                    return sp;
            }
        }
    }
    return nullptr;
}
static void FixIntSelectors(EXPRESSION** exp)
{
    if ((*exp)->left)
        FixIntSelectors(&(*exp)->left);
    if ((*exp)->right)
        FixIntSelectors(&(*exp)->right);
    if ((*exp)->type == en_templateselector ||
        ((*exp)->type == en_construct && (*exp)->v.construct.tp->type == bt_templateselector))
    {
        std::list<TEMPLATEPARAMPAIR>* currentx;
        if ((*exp)->type == en_templateselector)
            currentx = (*(*exp)->v.templateSelector)[1].templateParams;
        else
            currentx = (*(*exp)->v.construct.tp->sp->sb->templateSelector)[1].templateParams;
        std::list<TEMPLATEPARAMPAIR>* orig = currentx;
        std::deque<TYPE*> types;
        std::deque<EXPRESSION*> expressions;
        if (currentx)
        {
            for (auto current : *currentx)
            {
                if (current.second->type == kw_typename)
                {
                    types.push_back(current.second->byClass.dflt);
                    if (current.second->byClass.val)
                        current.second->byClass.dflt = current.second->byClass.val;
                }
                else if (current.second->type == kw_int)
                {
                    expressions.push_back(current.second->byNonType.dflt);
                    if (current.second->byNonType.val)
                        current.second->byNonType.dflt = current.second->byNonType.val;
                }
            }
        }
        optimize_for_constants(exp);
        if (orig)
        {
            for (auto current : *orig)
            {
                if (current.second->type == kw_typename)
                {
                    if (!types.empty())
                    {
                        current.second->byClass.dflt = types.front();
                        types.pop_front();
                    }
                }
                else if (current.second->type == kw_int)
                {
                    if (!expressions.empty())
                    {
                        current.second->byNonType.dflt = expressions.front();
                        expressions.pop_front();
                    }
                }
            }
        }
    }
}
static std::list<TEMPLATEPARAMPAIR>* ResolveTemplateSelector(SYMBOL* sp, TEMPLATEPARAMPAIR* arg, bool byVal)
{
    std::list<TEMPLATEPARAMPAIR>* rv = nullptr;
    if (arg)
    {
        bool toContinue = false;
        TYPE* tp;
        if (byVal)
            tp = arg->second->byClass.val;
        else
            tp = arg->second->byClass.dflt;
        if (arg->second->type == kw_typename && tp)
        {
            while (ispointer(tp) || isref(tp))
                tp = basetype(tp)->btp;
            if (basetype(tp)->type == bt_templateselector)
                toContinue = true;
        }
        if (arg->second->type == kw_int && tp)
        {
            if (byVal)
            {
                if (!isintconst(arg->second->byNonType.val) && !isfloatconst(arg->second->byNonType.val))
                    toContinue = true;
            }
            else
            {
                if (!isintconst(arg->second->byNonType.dflt) && !isfloatconst(arg->second->byNonType.dflt))
                    toContinue = true;
            }
        }
        if (toContinue)
        {
            std::vector<TEMPLATESELECTOR>* tso = nullptr;
            TYPE* tp = arg->second->byClass.dflt;
            rv = templateParamPairListFactory.CreateList();
            rv->push_back(TEMPLATEPARAMPAIR{nullptr, nullptr});
            if (arg->second->type == kw_typename && tp)
            {
                while (ispointer(tp) || isref(tp))
                    tp = basetype(tp)->btp;
                if (basetype(tp)->type == bt_templateselector)
                    tso = basetype(tp)->sp->sb->templateSelector;
                if (tso)
                {
                    SYMBOL* sp = FindTemplateSelector(tso);
                    if (sp)
                    {
                        if (istype(sp))
                        {
                            TYPE** txx;
                            rv->back().second = Allocate<TEMPLATEPARAM>();
                            *rv->back().second = *arg->second;
                            rv->back().first = arg->first;
                            if (byVal)
                            {
                                txx = &rv->back().second->byClass.val;
                            }
                            else
                            {
                                txx = &rv->back().second->byClass.dflt;
                                rv->back().second->byClass.val = nullptr;
                            }
                            *txx = CopyType(arg->second->byClass.dflt, true, [sp, tso](TYPE*& old, TYPE*& newx) {
                                if (newx->type == bt_templateselector)
                                {
                                    newx = sp->tp;
                                    if (isstructured(newx) && !templateNestingCount && basetype(newx)->sp->sb->templateLevel &&
                                        !basetype(newx)->sp->sb->instantiated)
                                    {
                                        SYMBOL* sp1 = basetype(newx)->sp;
                                        sp1 = GetClassTemplate((*tso)[1].sp, sp1->templateParams, false);
                                    }
                                }
                            });

                            UpdateRootTypes(byVal ? rv->back().second->byClass.val : rv->back().second->byClass.dflt);
                        }
                        else
                        {
                            rv->back().second = arg->second;
                            rv->back().first = arg->first;
                        }
                    }
                    else
                    {
                        rv->back().second = arg->second;
                        rv->back().first = arg->first;
                    }
                }
                else
                {
                    rv->back().second = arg->second;
                    rv->back().first = arg->first;
                }
            }
            else if (arg->second->type == kw_int && tp)
            {
                rv->back().second = Allocate<TEMPLATEPARAM>();
                *rv->back().second = *arg->second;
                rv->back().first = arg->first;
                if (arg->second->packed)
                {
                    auto packed = arg->second->byPack.pack;
                    if (packed)
                    {
                        auto newv = arg->second->byPack.pack = templateParamPairListFactory.CreateList();
                        for (auto tpx : *packed)
                        {
                            newv->push_back(TEMPLATEPARAMPAIR(tpx.first, Allocate<TEMPLATEPARAM>()));
                            newv->back().second = Allocate<TEMPLATEPARAM>();
                            *newv->back().second = *tpx.second;
                        }
                    }
                }
                else
                {
                    rv->back().second->byNonType.dflt = copy_expression(arg->second->byNonType.dflt);
                    FixIntSelectors(&rv->back().second->byNonType.dflt);
                    optimize_for_constants(&rv->back().second->byNonType.dflt);
                }
            }
            else
            {
                rv->back().second = arg->second;
                rv->back().first = arg->first;
            }
        }
    }
    return rv;
}
static std::list<TEMPLATEPARAMPAIR>* CopyArgsBack(std::list<TEMPLATEPARAMPAIR>* args, TEMPLATEPARAMPAIR* hold[], int k1)
{
    auto orig = args;
    int k = 0;
    std::list<TEMPLATEPARAMPAIR>* rv = args;
    std::list<TEMPLATEPARAMPAIR>::iterator t;
    std::list<TEMPLATEPARAMPAIR>::iterator te = t;
    std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
    if (args)
    {
        t = args->begin();
        te = args->end();
        for (; t != te;)
        {
            if (t->second->packed)
            {
                if (t->second->byPack.pack && t->second->byPack.pack->size())
                {
                    tas.push(t);
                    tas.push(te);
                    te = t->second->byPack.pack->end();
                    t = t->second->byPack.pack->begin();
                }
                else
                {
                    ++t;
                    continue;
                }
            }
            if (hold[k++] != &*t)
                break;
            ++t;
            if (t == te && !tas.empty())
            {
                if (hold[k++] != nullptr)
                    break;
                te = tas.top();
                tas.pop();
                t = tas.top();
                tas.pop();
                ++t;
            }
        }
    }
    if (t != te)
    {
        k = 0;
        rv = templateParamPairListFactory.CreateList();
        for (auto&& old : *args)
        {
            if (old.second->packed)
            {
                rv->push_back(TEMPLATEPARAMPAIR{old.first, Allocate<TEMPLATEPARAM>()});
                *rv->back().second = *old.second;
                rv->back().first = old.first;
                rv->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                while (hold[k] != nullptr)
                {
                    rv->back().second->byPack.pack->push_back(*hold[k++]);
                }
                k++;
            }
            else
            {
                rv->push_back(*hold[k++]);
            }
        }
        // this should be looked at.   It is part of the problem with GetClassTemplate modifying the input argument list
        if (rv->begin()->second->type == kw_new)
        {
            auto t1 = args->begin();
            ++t1;
            if (&*t1 == hold[1])
            {
                *args = *rv;
                rv = args;
            }
        }
        else
        {
            if (&*args->begin() == hold[0])
            {
                *args = *rv;
                rv = args;
            }
        }
    }
    return rv;
}
std::list<TEMPLATEPARAMPAIR>* ResolveTemplateSelectors(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, bool byVal)
{
    std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
    int k = 0;
    TEMPLATEPARAMPAIR* hold[200];
    if (args)
    {
        auto t = args->begin();
        auto te = args->end();
        for (; t != te;)
        {
            if (t->second->packed)
            {
                if (t->second->byPack.pack && t->second->byPack.pack->size())
                {
                    tas.push(t);
                    tas.push(te);
                    te = t->second->byPack.pack->end();
                    t = t->second->byPack.pack->begin();
                }
                else
                {
                    hold[k++] = nullptr;
                    ++t;
                    continue;
                }
            }
            auto a = ResolveTemplateSelector(sp, &*t, byVal);
            hold[k++] = a ? &*a->begin() : &*t;
            ++t;
            if (t == te && !tas.empty())
            {
                hold[k++] = nullptr;
                te = tas.top();
                tas.pop();
                t = tas.top();
                tas.pop();
                ++t;
            }
        }
    }
    return CopyArgsBack(args, hold, k);
}
TYPE* ResolveTemplateSelectors(SYMBOL* sp, TYPE* tp)
{
    TEMPLATEPARAM tpa = {};
    tpa.type = kw_typename;
    tpa.byClass.dflt = tp;
    std::list<TEMPLATEPARAMPAIR> tpx = { {nullptr, &tpa} };
    auto tpl2 = ResolveTemplateSelectors(sp, &tpx, false);
    return tpl2->front().second->byClass.dflt;
    ;
}
std::list<TEMPLATEPARAMPAIR>* ResolveDeclType(SYMBOL* sp, TEMPLATEPARAMPAIR* tpx, bool returnNull)
{
    if (tpx->second->type == kw_typename && tpx->second->byClass.dflt && tpx->second->byClass.dflt->type == bt_templatedecltype)
    {
        std::list<TEMPLATEPARAMPAIR>* rv = templateParamPairListFactory.CreateList();
        rv->push_back(TEMPLATEPARAMPAIR{tpx->first, Allocate<TEMPLATEPARAM>()});
        *rv->back().second = *tpx->second;
        rv->back().second->byClass.dflt = TemplateLookupTypeFromDeclType(rv->back().second->byClass.dflt);
        if (!rv->back().second->byClass.dflt)
            rv->back().second->byClass.dflt = &stdany;
        return rv;
    }
    else
    {
        if (returnNull)
            return nullptr;
        std::list<TEMPLATEPARAMPAIR>* rv = templateParamPairListFactory.CreateList();
        rv->push_back(*tpx);
        return rv;
    }
}
std::list<TEMPLATEPARAMPAIR>* ResolveDeclTypes(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (!templateNestingCount)
    {
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
        STRUCTSYM s;
        s.tmpl = args;
        addTemplateDeclaration(&s);
        int k = 0;
        TEMPLATEPARAMPAIR* hold[200];
        if (args)
        {
            auto t = args->begin();
            auto te = args->end();
            for (; t != te;)
            {
                if (t->second->packed)
                {
                    if (t->second->byPack.pack && t->second->byPack.pack->size())
                    {
                        tas.push(t);
                        tas.push(te);
                        te = t->second->byPack.pack->end();
                        t = t->second->byPack.pack->begin();
                    }
                    else
                    {
                        hold[k++] = nullptr;
                        ++t;
                        continue;
                    }
                }
                hold[k] = &*t;
                auto t1 = ResolveDeclType(sp, &*t, true);
                if (t1)
                    hold[k] = &*t1->begin();
                k++;
                ++t;
                if (t == te && !tas.empty())
                {
                    hold[k++] = nullptr;
                    te = tas.top();
                    tas.pop();
                    t = tas.top();
                    tas.pop();
                    ++t;
                }
            }
        }
        dropStructureDeclaration();
        return CopyArgsBack(args, hold, k);
    }
    return args;
}
static std::list<TEMPLATEPARAMPAIR>* ResolveConstructor(SYMBOL* sym, TEMPLATEPARAMPAIR* tpx)
{
    std::list<TEMPLATEPARAMPAIR>* rv = nullptr;
    if (tpx->second->type == kw_int && tpx->second->byNonType.dflt && tpx->second->byNonType.dflt->type == en_construct)
    {
        rv = templateParamPairListFactory.CreateList();
        rv->push_back(TEMPLATEPARAMPAIR{tpx->first, Allocate<TEMPLATEPARAM>()});
        *rv->back().second = *tpx->second;
        if (rv->back().second->byNonType.dflt->v.construct.tp->type == bt_templateselector)
        {
            SYMBOL* sp = FindTemplateSelector(rv->back().second->byNonType.dflt->v.construct.tp->sp->sb->templateSelector);
            if (sp)
                rv->back().second->byNonType.dflt->v.construct.tp = sp->tp;
        }
        optimize_for_constants(&rv->back().second->byNonType.dflt);
    }
    return rv;
}
static TEMPLATEPARAMPAIR* TypeAliasSearch(const char* name, bool toponly)
{
    for (auto&& s : structSyms)
    {
        std::list<TEMPLATEPARAMPAIR>* arg = s.tmpl;
        if (s.tmpl)
        {
            for (auto&& arg : *s.tmpl)
            {
                if (arg.first && !strcmp(arg.first->name, name))
                {
                    int len = strlen(name) + 1;
                    if (*(int*)(name + len) == *(int*)(arg.first->name + len))
                        return &arg;
                }
            }
            if (toponly)
                break;
        }
    }
    return nullptr;
}

std::list<TEMPLATEPARAMPAIR>* ResolveClassTemplateArgs(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    std::list<TEMPLATEPARAMPAIR>* rv = args;
    std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
    TEMPLATEPARAMPAIR* hold[200];
    int k = 0;
    if (args)
    {
        auto t = args->begin();
        auto te = args->end();
        for (; t != te;)
        {
            bool ellipsis = t->second->ellipsis;
            if (t->second->packed)
            {
                if (t->second->byPack.pack && t->second->byPack.pack->size())
                {
                    tas.push(t);
                    tas.push(te);
                    te = t->second->byPack.pack->end();
                    t = t->second->byPack.pack->begin();
                }
                else
                {
                    hold[k++] = nullptr;
                    ++t;
                    continue;
                }
            }
            int count = 0, n = 0;
            SYMBOL* syms[200];
            if (ellipsis)
            {
                if (t->second->type == kw_int)
                {
                    if (t->second->packed)
                    {
                        if (t->second->byPack.pack)
                            for (auto tpx : *t->second->byPack.pack)
                                GatherPackedVars(&count, syms, tpx.second->byNonType.dflt);
                    }
                    else
                    {
                        GatherPackedVars(&count, syms, t->second->byNonType.dflt);
                    }
                }
                else if (t->second->type == kw_typename)
                {
                    if (t->second->packed)
                    {
                        if (t->second->byPack.pack)
                            for (auto&& tpx : *t->second->byPack.pack)
                                GatherPackedTypes(&count, syms, tpx.second->byClass.dflt);
                    }
                    else
                    {

                        GatherPackedTypes(&count, syms, t->second->byClass.dflt);
                    }
                }
                for (int i = 0; i < count; i++)
                {
                    auto rv = TypeAliasSearch(syms[i]->name, false);
                    if (rv && rv->second->packed)
                    {
                        int n1 = CountPacks(rv->second->byPack.pack);
                        if (n1 > n)
                            n = n1;
                    }
                }
            }
            n--;
            int oldIndex = packIndex;
            for (int i = n < 0 ? -1 : 0; i <= n; i++)
            {
                if (n >= 0)
                    packIndex = i;
                hold[k] = &*t;
                auto t1 = ResolveDeclType(sp, &*hold[k], true);
                if (t1)
                    hold[k] = &*t1->begin();
                auto t2 = ResolveTemplateSelector(sp, &*hold[k], false);
                if (t2)
                    hold[k] = &*t2->begin();
                auto t3 = ResolveConstructor(sp, &*hold[k]);
                if (t3)
                    hold[k] = &*t3->begin();
                k++;
            }
            packIndex = oldIndex;
            ++t;
            if (t == te && !tas.empty())
            {
                hold[k++] = nullptr;
                te = tas.top();
                tas.pop();
                t = tas.top();
                tas.pop();
                ++t;
            }
        }
    }
    return CopyArgsBack(args, hold, k);
}
static void copySyms(SYMBOL* found1, SYMBOL* sym)
{
    auto src = sym->templateParams;
    auto dest = found1->templateParams;
    auto itsrc = src->begin();
    auto itdest = dest->begin();
    for ( ; itsrc != src->end() && itdest != dest->end(); ++itsrc, ++itdest)
    {
        if (itsrc->second->type != kw_new)
        {
            SYMBOL* hold = itdest->first;
            TYPE* tp = CopyType(itsrc->first->tp);
            itdest->first = CopySymbol(itsrc->first);
            itdest->first->tp = tp;
            if (hold)
            {
                itdest->first->name = hold->name;
            }
            UpdateRootTypes(itdest->first->tp);
            itdest->first->tp->templateParam = &*itdest;
        }
    }
}
SYMBOL* GetClassTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, bool noErr)
{
    // quick check for non-template
    if (!sp->sb->templateLevel)
        return sp;
    int n = 1, i = 0;
    std::list<TEMPLATEPARAMPAIR>* unspecialized = sp->templateParams;
    SYMBOL *found1 = nullptr, *found2 = nullptr;
    SYMBOL **spList, **origList;
    std::list<TEMPLATEPARAMPAIR>* search = args;
    int count = 0;
    noErr |= matchOverloadLevel;
    args = ResolveClassTemplateArgs(sp, args);
    auto argsorig = args;
    if (args && args->front().second->type == kw_new)
    {
        auto it = args->begin();
        ++it;
        auto args1 = templateParamPairListFactory.CreateList();
        args1->insert(args1->begin(), it, args->end());
        args = args1;
    }
    if (sp->sb->parentTemplate && sp)
        sp = sp->sb->parentTemplate;

    std::string argumentName;
    if (GetTemplateArgumentName(args, argumentName, false))
    {
        SYMBOL* found1 = classTemplateMap[sp][argumentName];
        if (found1)
        {
            return found1;
        }
    }
    if (sp->sb->specializations)
        n += sp->sb->specializations->size();
    spList = Allocate<SYMBOL*>(n);
    origList = Allocate<SYMBOL*>(n);
    origList[i++] = sp;
    if (sp->sb->specializations)
        for (auto s : *sp->sb->specializations)
            origList[i++] = s;
    saveParams(origList, n);
    for (i = 0; i < n; i++)
    {
        spList[i] = ValidateClassTemplate(origList[i], unspecialized, args);
    }
    for (i = 0, count = 0; i < n; i++)
    {
        if (spList[i])
            count++;
    }
    if (count > 1)
    {
        int count1 = 0;
        bool match0 = !!spList[0];
        spList[0] = 0;
        for (i = 0; i < n; i++)
            if (spList[i])
                count1++;
        if (count1 > 1)
        {
            TemplatePartialOrdering(spList, n, nullptr, nullptr, true, false);
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                    count1++;
        }
        if (count1 > 1 || (count1 == 1 && match0 && constOnly(spList, origList, n)))
        {
            for (i = 0; i < n; i++)
            {
                if ((match0 && i == 0) || spList[i])
                {
                    spList[i] = ValidateClassTemplate(origList[i], unspecialized, args);
                }
            }
            TemplateConstMatching(spList, n, args);
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                    count1++;
        }
        if (count1 > 1)
        {
            spList[0] = 0;
            TemplateConstOrdering(spList, n, args);
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                    count1++;
        }
        if (count1 > 1 && templateNestingCount)
        {
            // if it is going to be ambiguous but we are gathering a template, just choose the first one
            for (i = 0; i < n; i++)
                if (spList[i])
                    break;
            for (i = i + 1; i < n; i++)
                spList[i] = 0;
            count1 = 0;
            for (i = 0; i < n; i++)
                if (spList[i])
                    count1++;
        }
        if (count1 > 1)
        {
            ChooseMoreSpecialized(spList, n);
        }
    }
    for (i = 0; i < n && !found1; i++)
    {
        int j;
        found1 = spList[i];
        for (j = i + 1; j < n && found1 && !found2; j++)
        {
            if (spList[j])
            {
                found2 = spList[j];
            }
        }
    }
    if (found1 && !found2)
    {
        found1 = ValidateClassTemplate(origList[i - 1], unspecialized, args);
    }
    if (!found1 && !templateNestingCount && spList[0])
    {
        if (!noErr)
        {
            errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        }
        // might get more error info by procedeing;
        if (!sp->sb->specializations && sp->templateParams)
        {
            bool found = false;
            for (auto param : *sp->templateParams)
            {
                if (!param.second->byClass.val)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                found1 = sp;
        }
    }
    else if (found2)
    {
        restoreParams(origList, n);
        errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        return nullptr;
    }
    if (found1 && !found2)
    {
        SYMBOL* sym = found1;
        TemplateArgsCopy(found1->templateParams);
        if (found1->sb->parentTemplate && allTemplateArgsSpecified(found1, found1->templateParams))
        {
            bool partialCreation = false;
            SYMBOL test = *found1;
            SYMBOL* parent = found1->sb->parentTemplate;

            for (auto&& dflts : *found1->templateParams)
            {
                if (dflts.second->type == kw_int && dflts.second->byNonType.val)
                {
                    partialCreation = !isarithmeticconst(dflts.second->byNonType.val);
                    if (partialCreation)
                        break;
                }
            }
            if (partialCreation)
            {
                test.templateParams = copyParams(test.templateParams, true);
                for (auto&& dflts : *test.templateParams)
                {
                    if (dflts.second->type == kw_int && dflts.second->byNonType.val)
                        if (!isarithmeticconst(dflts.second->byNonType.val))
                        {
                            dflts.second->byNonType.val = copy_expression(dflts.second->byNonType.val);
                            optimize_for_constants(&dflts.second->byNonType.val);
                        }
                }
            }

            found1 = CopySymbol(&test);
            found1->sb->maintemplate = sym;

            found1->tp = CopyType(sym->tp);
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;
            found1->sb->gentemplate = true;
            found1->sb->instantiated = true;
            found1->sb->performedStructInitialization = false;
            if (!partialCreation)
                found1->templateParams = copyParams(found1->templateParams, true);

            if (found1->templateParams->front().second->bySpecialization.types)
            {
                std::list<TEMPLATEPARAMPAIR>** pptr = &found1->templateParams->front().second->bySpecialization.types;
                DuplicateTemplateParamList(pptr);
            }
            copySyms(found1, sym);
            SetLinkerNames(found1, lk_cdecl);

            auto found2 = classTemplateMap2[found1->sb->decoratedName];
            if (found2 && (found2->sb->specialized || !found1->sb->specialized))
            {
                restoreParams(origList, n);
                return found2;
            }

            if (!parent->sb->instantiations)
                parent->sb->instantiations = symListFactory.CreateList();
            parent->sb->instantiations->push_back(found1);
            if (found1->templateParams->size() - 1 == (args ? args->size() : 0))
            {
                if (found1->sb->deferredCompile || (found1->sb->maintemplate && found1->sb->maintemplate->sb->deferredCompile) ||
                    (found1->sb->parentTemplate && found1->sb->parentTemplate->sb->deferredCompile))
                {
                    classTemplateMap[sp][argumentName] = found1;
                }
            }
            classTemplateMap2[found1->sb->decoratedName] = found1;
        }
        else
        {
            found1 = CopySymbol(found1);
            found1->sb->maintemplate = sym;
            found1->tp = CopyType(sym->tp);
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;

            found1->templateParams = templateParamPairListFactory.CreateList();
            found1->templateParams->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            *found1->templateParams->back().second = *sym->templateParams->front().second; // the kw_new entry
            auto it = sym->templateParams->begin();
            ++it;
            if (args)
            {
                found1->templateParams->insert(found1->templateParams->end(), args->begin(), args->end());
                for (int i = 0; i < args->size() && it != sym->templateParams->end(); ++i, ++it)
                    ;
                if (it != sym->templateParams->end() && it->second->byClass.txtdflt)
                {
                    argsorig->insert(argsorig->end(), it, sym->templateParams->end());
                    found1->templateParams->insert(found1->templateParams->end(), it, sym->templateParams->end());
                }
                copySyms(found1, sym);
            }
            else
            {
                found1->templateParams->insert(found1->templateParams->end(), it, sym->templateParams->end());
            }
        }
    }

    restoreParams(origList, n);
    return found1;
}
SYMBOL* GetVariableTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    // this implementation does simple variables and pointers, but not arrays/function pointers
    std::list<TEMPLATEPARAMPAIR>* unspecialized = sp->templateParams;
    SYMBOL **origList, **spList, *found1, *found2;
    int n = 1;
    int count1, i;
    TYPE** tpi;
    if (sp->sb->specializations)
        n += sp->sb->specializations->size();
    spList = Allocate<SYMBOL*>(n);
    origList = Allocate<SYMBOL*>(n);
    origList[0] = sp;
    spList[0] = ValidateClassTemplate(sp, unspecialized, args);
    tpi = &spList[0]->tp;
    while (isref(*tpi) || ispointer(*tpi))
        tpi = &basetype(*tpi)->btp;
    if (isstructured(*tpi) && basetype(*tpi)->sp->sb->templateLevel)
    {
        SYMBOL* sym = GetClassTemplate(basetype(*tpi)->sp, args, true);
        if (sym)
        {
            *tpi = TemplateClassInstantiate(sym, args, false, sc_global)->tp;
        }
    }
    n = 1;
    if (sp->sb->specializations)
    {
        for (auto l : *sp->sb->specializations)
        {
            origList[n] = l;
            spList[n] = ValidateClassTemplate(origList[n], unspecialized, args);
            tpi = &spList[n]->tp;
            while (isref(*tpi) || ispointer(*tpi))
                tpi = &basetype(*tpi)->btp;
            if (isstructured(*tpi) && basetype(*tpi)->sp->sb->templateLevel)
            {
                SYMBOL* sym = GetClassTemplate(basetype(*tpi)->sp, args, true);
                if (sym)
                {
                    *tpi = TemplateClassInstantiate(sym, args, false, sc_global)->tp;
                }
            }
            n++;
        }
    }
    saveParams(spList, n);
    if (n > 1)
        TemplatePartialOrdering(spList, n, nullptr, nullptr, true, false);
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    if (count1 > 1)
    {
        spList[0] = 0;
        TemplateConstOrdering(spList, n, args);
    }
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    if (count1 > 1 && templateNestingCount)
    {
        // if it is going to be ambiguous but we are gathering a template, just choose the first one
        for (i = 0; i < n; i++)
            if (spList[i])
                break;
        for (i = i + 1; i < n; i++)
            spList[i] = 0;
    }
    count1 = 0;
    for (i = 0; i < n; i++)
        if (spList[i])
            count1++;
    if (count1 > 1)
        ChooseMoreSpecialized(spList, n);
    found1 = found2 = nullptr;
    for (i = 0; i < n && !found1; i++)
    {
        int j;
        found1 = spList[i];
        for (j = i + 1; j < n && found1 && !found2; j++)
        {
            if (spList[j])
            {
                found2 = spList[j];
            }
        }
    }
    if (found1 && !found2)
    {
        SYMBOL* sym = found1;
        if (found1->sb->parentTemplate && allTemplateArgsSpecified(found1, found1->templateParams))
        {
            bool partialCreation = false;
            SYMBOL test = *found1;
            SYMBOL* parent = found1->sb->parentTemplate;
            std::list<TEMPLATEPARAMPAIR>* dflts = found1->templateParams;

            for (auto &&dflts : *found1->templateParams)
            {
                if (dflts.second->type == kw_int && dflts.second->byNonType.val)
                {
                    partialCreation = !isarithmeticconst(dflts.second->byNonType.val);
                    if (partialCreation)
                        break;
                }
            }
            if (partialCreation)
            {
                test.templateParams = copyParams(test.templateParams, true);

                for (auto &&dflts : *test.templateParams)
                {
                    if (dflts.second->type == kw_int && dflts.second->byNonType.val)
                        if (!isarithmeticconst(dflts.second->byNonType.val))
                        {
                            dflts.second->byNonType.val = copy_expression(dflts.second->byNonType.val);
                            optimize_for_constants(&dflts.second->byNonType.val);
                        }
                }
            }
            found1 = CopySymbol(&test);
            found1->sb->maintemplate = sym;
            found1->tp = CopyType(sym->tp);
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;
            found1->sb->gentemplate = true;
            found1->sb->instantiated = true;
            if (!partialCreation)
                found1->templateParams = copyParams(found1->templateParams, true);
            if (found1->templateParams->front().second->bySpecialization.types)
            {
                DuplicateTemplateParamList(&found1->templateParams->front().second->bySpecialization.types);
            }
            copySyms(found1, sym);
            SetLinkerNames(found1, lk_cdecl);
            auto found2 = classTemplateMap2[found1->sb->decoratedName];
            if (found2)
            {
                return found2;
            }

            if (!parent->sb->instantiations)
                parent->sb->instantiations = symListFactory.CreateList();
            parent->sb->instantiations->push_back(found1);
            found1->tp = SynthesizeType(found1->tp, nullptr, false);
            if (found1->sb->init)
            {
                std::list<INITIALIZER*>* init = initListFactory.CreateList();
                auto begin = found1->sb->init->begin();
                auto end = found1->sb->init->end();
                RecalculateVariableTemplateInitializers(begin, end, &init, found1->tp, 0);
                found1->sb->init = init;
            }
            found1->sb->attribs.inheritable.linkage4 = lk_virtual;
            classTemplateMap2[found1->sb->decoratedName] = found1;
            InsertInlineData(found1);
        }
        else
        {
            found1 = CopySymbol(found1);
            found1->sb->maintemplate = sym;
            found1->tp = CopyType(sym->tp);
            UpdateRootTypes(found1->tp);
            found1->tp->sp = found1;

            found1->templateParams = templateParamPairListFactory.CreateList();
            found1->templateParams->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            *found1->templateParams->back().second = *sym->templateParams->front().second;
            if (args)
            {
                found1->templateParams->insert(found1->templateParams->end(), args->begin(), args->end());
                copySyms(found1, sym);
            }
            else
            {
                auto it = sym->templateParams->begin();
                ++it;
                found1->templateParams->insert(found1->templateParams->end(), it, sym->templateParams->end());
            }
            found1->tp = &stdint;
        }
    }
    restoreParams(spList, n);
    return found1;
}

void SpecifyTemplateSelector(std::vector<TEMPLATESELECTOR>** rvs, std::vector<TEMPLATESELECTOR>* old, bool expression, SYMBOL* sym,
                             std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
bool ReplaceIntAliasParams(EXPRESSION** exp, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                           std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    bool rv = false;
    if ((*exp)->left)
        rv |= ReplaceIntAliasParams(&(*exp)->left, sym, origTemplate, origUsing);
    if ((*exp)->right)
        rv |= ReplaceIntAliasParams(&(*exp)->right, sym, origTemplate, origUsing);
    if ((*exp)->type == en_templateparam)
    {
        const char* name = (*exp)->v.sp->name;
        TEMPLATEPARAMPAIR* found = TypeAliasSearch(name, false);
        if (found && found->second->byNonType.dflt)
        {
            *exp = found->second->byNonType.dflt;
        }
        rv = true;
    }
    else if ((*exp)->type == en_sizeofellipse)
    {
        const char* name = (*exp)->v.templateParam->first->name;
        TEMPLATEPARAMPAIR* found = TypeAliasSearch(name, false);
        if (found)
        {
            (*exp)->v.templateParam->second = found->second;
        }
        rv = true;
    }
    else if ((*exp)->type == en_templateselector)
    {
        SpecifyTemplateSelector(&(*exp)->v.templateSelector, (*exp)->v.templateSelector, true, sym, origTemplate, origUsing);
    }
    return rv;
}
static EXPRESSION* SpecifyArgInt(SYMBOL* sym, EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* orig,
                                 std::list<TEMPLATEPARAMPAIR>* origTemplate,
                                 std::list<TEMPLATEPARAMPAIR>* origUsing);
static TYPE* SpecifyArgType(SYMBOL* sym, TYPE* tp, TEMPLATEPARAM* tpt, std::list<TEMPLATEPARAMPAIR>* orig,
                            std::list<TEMPLATEPARAMPAIR>* origTemplate,
                            std::list<TEMPLATEPARAMPAIR>* origUsing);
static void SpecifyOneArg(SYMBOL* sym, TEMPLATEPARAMPAIR* temp,
                          std::list<TEMPLATEPARAMPAIR>* origTemplate,
                          std::list<TEMPLATEPARAMPAIR>* origUsing);
void SearchAlias(const char* name, TEMPLATEPARAMPAIR* x, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                 std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    TEMPLATEPARAMPAIR* rv = TypeAliasSearch(name, false);
    if (rv)
    {
        if (x->second->packed && !rv->second->packed)
        {
            x->second->byPack.pack = templateParamPairListFactory.CreateList();
            x->second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, rv->second});
        }
        else if (rv->second->packed && packIndex >= 0 && !x->second->ellipsis)
        {
            if (rv->second->byPack.pack && packIndex < rv->second->byPack.pack->size())
            {
                auto it = rv->second->byPack.pack->begin();
                auto ite = rv->second->byPack.pack->end();
                for (int i = 0; i < packIndex && it != ite; ++i, ++it)
                    ;
                if (it != ite)
                    x->second = it->second;
            }
        }
        else
        {
            x->second = rv->second;
        }
        if (x->second->packed)
        {
            if (x->second->byPack.pack)
            {
            for (auto&& tpx : *x->second->byPack.pack)
                if (tpx.second->byClass.val)
                    tpx.second->byClass.dflt = tpx.second->byClass.val;
            }
        }
        else if (x->second->byClass.val)
        {
            x->second->byClass.dflt = x->second->byClass.val;
        }
        if (x->second->packed && x->second->byPack.pack)
        {
            for (auto&& tpx : *x->second->byPack.pack)
            {
                if (tpx.second->byNonType.dflt)
                {
                    switch (tpx.second->type)
                    {
                        case kw_int:

                            tpx.second->byNonType.dflt =
                                SpecifyArgInt(sym, tpx.second->byNonType.dflt, nullptr, origTemplate, origUsing);
                            optimize_for_constants(&tpx.second->byNonType.dflt);
                            break;
                        case kw_typename:
                            tpx.second->byClass.dflt =
                                SpecifyArgType(sym, tpx.second->byClass.dflt, nullptr, nullptr, origTemplate, origUsing);
                            break;
                    }
                }
            }
        }
        else if (x->second->byClass.dflt)
        {
            SpecifyOneArg(sym, x, origTemplate, origUsing);
        }
        x->second->replaced = true;
    }
}
static TYPE* ReplaceTemplateParam(TYPE* in)
{
    TYPE* find = in;
    while (find && find->type != bt_templateparam)
        find = find->btp;
    if (find)
    {
        if (!find->templateParam->second->packed && find->templateParam->first)
        {
            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(find->templateParam->first->name, false);
            if (rv && rv->second->byClass.dflt)
            {
                TYPE** last = &find;
                while (in && in->type != bt_templateparam)
                {
                    *last = CopyType(in);
                    last = &(*last)->btp;
                    in = in->btp;
                }
                *last = rv->second->byClass.dflt;
                UpdateRootTypes(find);
                return find;
            }
        }
    }
    return in;
}
void SpecifyTemplateSelector(std::vector<TEMPLATESELECTOR>** rvs, std::vector<TEMPLATESELECTOR>* old, bool expression, SYMBOL* sym,
                             std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    bool first = true;
    if (old && old->size())
    {
        *rvs = templateSelectorListFactory.CreateVector();
        for (auto&& oldItem : *old)
        {
            (*rvs)->push_back(oldItem);
            if (oldItem.isDeclType)
            {
                first = false;
                if (!templateNestingCount)
                {
                    TYPE* basetp = oldItem.tp;
                    (*rvs)->back().tp = SpecifyArgType(basetp->sp, basetp, nullptr, nullptr, origTemplate, origUsing);
                }
            }
            else
            {
                if (first && oldItem.sp)
                {
                    first = false;
                    if (oldItem.sp->tp->type == bt_templateparam)
                    {
                        TEMPLATEPARAMPAIR* rv = TypeAliasSearch(oldItem.sp->name, false);
                        if (rv && rv->second->type == kw_typename)
                        {
                            auto tpx = rv->second;
                            if (tpx->packed)
                            {
                                if (tpx->byPack.pack && tpx->byPack.pack->size())
                                    tpx = tpx->byPack.pack->front().second;
                                else
                                    tpx = nullptr;
                            }
                            if (tpx)
                            {
                                TYPE* tp = tpx->byClass.val ? tpx->byClass.val : tpx->byClass.dflt;
                                if (tp && isstructured(tp))
                                    (*rvs)->back().sp = basetype(tp)->sp;
                            }
                        }
                    }
                }
                if (oldItem.isTemplate)
                {
                    std::list<TEMPLATEPARAMPAIR>* tpx = nullptr, *x;
                    if (oldItem.templateParams)
                    {
                        tpx = (*rvs)->back().templateParams;
                        x = (*rvs)->back().templateParams = templateParamPairListFactory.CreateList();
                    }
                    else
                    {
                        (*rvs)->back().sp = CopySymbol((*rvs)->back().sp);
                        tpx = (*rvs)->back().sp->templateParams;
                        x = (*rvs)->back().sp->templateParams = templateParamPairListFactory.CreateList();
                    }
                    std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> stk;
                    auto ittp = tpx->begin();
                    auto itetp = tpx->end();
                    for ( ; ittp != itetp ; )
                    {
                        if (ittp->second->packed && ittp->second->byPack.pack)
                        {
                            stk.push(ittp);
                            stk.push(itetp);
                            itetp = ittp->second->byPack.pack->end();
                            ittp = ittp->second->byPack.pack->begin();
                        }
                        x->push_back(TEMPLATEPARAMPAIR{ittp->first, ittp->second});
                        if (ittp->second->type != kw_new)
                        {
                            bool replaced = false;
                            x->back().second = Allocate<TEMPLATEPARAM>();
                            *x->back().second = *ittp->second;
                            if (!expression && ittp->second->type == kw_int && ittp->second->byNonType.dflt)
                            {
                                x->back().second->byNonType.dflt = copy_expression(x->back().second->byNonType.dflt);
                                replaced =
                                    ReplaceIntAliasParams(&x->back().second->byNonType.dflt, sym, origTemplate, origUsing);
                                if (replaced)
                                    optimize_for_constants(&x->back().second->byNonType.dflt);
                            }
                            if (!replaced && ittp->first &&
                                (expression || (ittp->second->type == kw_int || !ittp->second->byClass.dflt)))
                            {
                                const char* name = ittp->first->name;
                                if (!expression && ittp->second->type == kw_int && ittp->second->byNonType.dflt &&
                                    ittp->second->byNonType.dflt->type == en_templateparam)
                                {
                                    name = ittp->second->byNonType.dflt->v.sp->name;
                                    SearchAlias(name, &x->back(), sym, origTemplate, origUsing);
                                }
                                else if (expression && ittp->second->type == kw_int && x->back().second->byNonType.dflt)
                                {
                                    if (!IsConstantExpression(x->back().second->byNonType.dflt, false, false))
                                        SearchAlias(name, &x->back(), sym, origTemplate, origUsing);
                                }
                                else if (x->back().second->type == kw_typename && x->back().second->byClass.dflt)
                                {
                                    // this is because a 'default' can either be from
                                    // a really defaulted value, or it can be from a previous
                                    // replacement.   We keep track of when a previous replacement
                                    // occurs and check it here.
                                    // it would be better if we set this up during the declaration phase,
                                    // however I don't want to duplicate the massive amounts of code that
                                    // go through the type & expression trees to locate such things...
                                    if (x->back().second->replaced)
                                    {
                                        SearchAlias(name, &x->back(), sym, origTemplate, origUsing);
                                    }
                                    else
                                    {
                                        SpecifyOneArg(sym, &x->back(), origTemplate, origUsing);
                                    }
                                }
                                else
                                {
                                    SearchAlias(name, &x->back(), sym, origTemplate, origUsing);
                                }
                            }
                            if (x->back().second->type == kw_typename)
                                if (x->back().second->packed)
                                {
                                    if (x->back().second->byPack.pack)
                                        for (auto&& tpl : *x->back().second->byPack.pack)
                                            tpl.second->byClass.dflt = ReplaceTemplateParam(tpl.second->byClass.dflt);
                                }
                                else
                                {
                                    x->back().second->byClass.dflt = ReplaceTemplateParam(x->back().second->byClass.dflt);
                                }
                        }
                        ++ittp;
                        if (ittp == itetp && !stk.empty())
                        {
                            itetp = stk.top();
                            stk.pop();
                            ittp = stk.top();
                            stk.pop();
                            ++ittp;
                        }
                    }
                }
                if (oldItem.asCall && oldItem.arguments && oldItem.arguments->size())
                {
                    (*rvs)->back().arguments = initListListFactory.CreateList();
                    for (auto ilx : *oldItem.arguments)
                    {
                        auto arg = Allocate<INITLIST>();
                        *arg = *ilx;
                        (*rvs)->back().arguments->push_back(arg);
                        arg->tp = SpecifyArgType(sym, arg->tp, nullptr, nullptr, origTemplate, origUsing);
                        arg->exp = SpecifyArgInt(sym, arg->exp, nullptr, origTemplate, origUsing);
                    }
                }
            }
        }
    }
}
static EXPRESSION* SpecifyArgInt(SYMBOL* sym, EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* orig,
                                 std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    if (exp)
    {
        if (exp->left || exp->right)
        {
            EXPRESSION* exp1 = Allocate<EXPRESSION>();
            *exp1 = *exp;
            exp = exp1;
            if (exp->left)
            {
                exp->left = SpecifyArgInt(sym, exp->left, orig, origTemplate, origUsing);
            }
            if (exp->right)
            {
                exp->right = SpecifyArgInt(sym, exp->right, orig, origTemplate, origUsing);
            }
        }
        else if (exp->type == en_templateparam || exp->type == en_auto && exp->v.sp->packed)
        {
            TEMPLATEPARAMPAIR* rv;
            if (exp->type == en_templateparam)
                rv = TypeAliasSearch(exp->v.sp->tp->templateParam->first->name, false);
            else
                rv = TypeAliasSearch(exp->v.sp->name, false);
            if (rv)
            {
                if (rv->second->type == kw_int)
                {
                    if (packIndex >= 0 && rv->second->packed && !exp->v.sp->tp->templateParam->second->ellipsis)
                    {
                        if (rv->second->byPack.pack && packIndex < rv->second->byPack.pack->size())
                        {
                            auto it = rv->second->byPack.pack->begin();
                            for (int i = 0; i < packIndex; ++i, ++it)
                                ;
                            auto exp1 = it->second->byNonType.val ? it->second->byNonType.val : it->second->byNonType.dflt;
                            if (exp1)
                                exp = exp1; // dal fixme
                        }
                    }
                    else
                    {
                        if (rv->second->byNonType.dflt)
                            exp = rv->second->byNonType.dflt;
                    }
                }
                else
                {
                    if (rv->second->byClass.dflt)
                    {
                        TYPE* dflt = rv->second->byClass.dflt;
                        if (packIndex >= 0 && rv->second->packed && !exp->v.sp->tp->templateParam->second->ellipsis)
                        {
                            if (rv->second->byPack.pack && packIndex < rv->second->byPack.pack->size())
                            {
                                auto it = rv->second->byPack.pack->begin();
                                for (int i = 0; i < packIndex; ++i, ++it)
                                    ;
                                dflt = rv->second->byClass.val ? rv->second->byClass.val : rv->second->byClass.dflt;
                            }
                            else
                            {
                                dflt = nullptr;
                            }
                        }
                        if (dflt)
                        {
                            // typename, allocate space for a type...
                            if (isstructured(dflt))
                            {
                                exp = anonymousVar(sc_auto, dflt);
                            }
                            else
                            {
                                exp = anonymousVar(sc_auto, dflt);
                                if (isref(dflt))
                                {
                                    TYPE* tp1 = basetype(dflt)->btp;
                                    deref(tp1, &exp);
                                }
                                else
                                {
                                    deref(dflt, &exp);
                                }
                            }
                        }
                    }
                }
            }
            if (exp)
                optimize_for_constants(&exp);
        }
        else if (exp->type == en_templateselector)
        {
            EXPRESSION* exp1 = Allocate<EXPRESSION>();
            *exp1 = *exp;
            exp = exp1;
            SpecifyTemplateSelector(&exp->v.templateSelector, exp->v.templateSelector, true, sym, origTemplate, origUsing);
            optimize_for_constants(&exp);
        }
        else if (exp->type == en_auto)
        {
            if (packIndex >= 0)
            {
                TEMPLATEPARAMPAIR* rv = TypeAliasSearch(exp->v.sp->name, false);
                if (rv && rv->second->packed)
                {
                    if (rv->second->byPack.pack && packIndex < rv->second->byPack.pack->size())
                    {
                        auto it = rv->second->byPack.pack->begin();
                        for (int i = 0; i < packIndex; ++i, ++it)
                            ;
                        exp = it->second->byNonType.dflt;
                    }
                }
            }
        }
        else if (exp->type == en_func || exp->type == en_funcret)
        {
            EXPRESSION *exp1 = nullptr, **last = &exp1;
            ;
            while (exp->type == en_funcret)
            {
                *last = Allocate<EXPRESSION>();
                **last = *exp;
                exp = exp->left;
                last = &(*last)->left;
            }
            *last = Allocate<EXPRESSION>();
            **last = *exp;
            (*last)->v.func = Allocate<FUNCTIONCALL>();
            *(*last)->v.func = *exp->v.func;

            std::list<TEMPLATEPARAMPAIR>* tpxx = (*last)->v.func->templateParams;
            if (tpxx)
            {
                auto x1 = (*last)->v.func->templateParams = templateParamPairListFactory.CreateList();
                for (auto tpx : *tpxx )
                {
                    x1->push_back(TEMPLATEPARAMPAIR{tpx.first, Allocate<TEMPLATEPARAM>()});
                    *x1->back().second = *tpx.second;
                    if (x1->back().second->type == kw_int || x1->back().second->type == kw_typename || x1->back().second->type == kw_template)
                    {
                        if (x1->back().second->byClass.dflt)
                        {
                            if (x1->back().second->packed)
                            {
                                if (x1->back().second->byPack.pack)
                                {
                                    for (auto&& t : *x1->back().second->byPack.pack)
                                    {
                                        SpecifyOneArg(sym, &t, origTemplate, origUsing);
                                    }
                                }
                            }
                            else
                            {
                                SpecifyOneArg(sym, &x1->back(), origTemplate, origUsing);
                            }
                        }
                        else if (x1->back().first)
                        {
                            SearchAlias(x1->back().first->name, &x1->back(), sym, origTemplate, origUsing);
                        }
                    }
                }
            }
            if ((*last)->v.func->arguments)
            {
                auto old = (*last)->v.func->arguments;
                (*last)->v.func->arguments = initListListFactory.CreateList();
                for (auto arg : *old)
                {
                    auto arg1 = Allocate<INITLIST>();
                    *arg1 = *arg;
                    (*last)->v.func->arguments->push_back(arg1);
                    arg1->exp = SpecifyArgInt(sym, arg1->exp, orig, origTemplate, origUsing);
                    arg1->tp = LookupTypeFromExpression(arg1->exp, nullptr, false);
                    arg1->tp = SpecifyArgType(sym, arg1->tp, nullptr, orig, origTemplate, origUsing);
                }
            }
            exp = exp1;
        }
        else if (exp->type == en_construct)
        {
            EXPRESSION* exp1 = Allocate<EXPRESSION>();
            *exp1 = *exp;
            exp = exp1;
            exp->v.construct.tp = SpecifyArgType(sym, exp->v.construct.tp, nullptr, orig, origTemplate, origUsing);
            optimize_for_constants(&exp);
        }
        else if (exp->type == en_sizeofellipse)
        {
            EXPRESSION* exp1 = Allocate<EXPRESSION>();
            *exp1 = *exp;
            exp = exp1;
            const char* name = exp->v.templateParam->first->name;
            TEMPLATEPARAMPAIR* found = TypeAliasSearch(name, false);
            if (found)
            {
                exp->v.templateParam->second = found->second;
            }
        }
    }
    return exp;
}
static TYPE* SpecifyArgType(SYMBOL* sym, TYPE* tp, TEMPLATEPARAM* tpt, std::list<TEMPLATEPARAMPAIR>* orig,
                            std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    if (!tp)
        return nullptr;
    auto rv = CopyType(tp, true);
    UpdateRootTypes(rv);
    tp = rv;
    while (ispointer(tp) || isref(tp))
        tp = basetype(tp)->btp;
    if (tp->type == bt_typedef && tp->sp->templateParams)
    {
        tp->sp = CopySymbol(tp->sp);
        auto temp = tp->sp->templateParams;
        auto tpr = tp->sp->templateParams = templateParamPairListFactory.CreateList();
        auto tps = tp->sp->sb->mainsym->templateParams;
        auto ittps = tps->begin();
        for (auto&& tpl : *temp)
        {
            tpr->push_back(TEMPLATEPARAMPAIR{tpl.first, Allocate<TEMPLATEPARAM>()});
            *tpr->back().second = *tpl.second;
            if (tpr->back().second->packed)
            {
                if (packIndex >= 0 && !tpr->back().second->ellipsis)
                {
                    if (tpr->back().second->byPack.pack && packIndex < tpr->back().second->byPack.pack->size())
                    {
                        auto it = tpr->back().second->byPack.pack->begin();
                        for (int i = 0; i < packIndex; ++i, ++it)
                            ;
                        *tpr->back().second = *it->second;
                    }
                }
                else if (tpr->back().second->ellipsis)
                {
                    if (tpr->back().second->byPack.pack)
                    {
                        for (auto&& t : *tpr->back().second->byPack.pack)
                        {
                            SpecifyOneArg(sym, &t, origTemplate, origUsing);
                        }
                    }
                }
                else if (tpr->back().second->byPack.pack)
                {
                    for (auto&& t : *tpr->back().second->byPack.pack)
                    {
                        SpecifyOneArg(sym, &t, origTemplate, origUsing);
                    }
                }
            }
            else
            {
                if (tpr->back().second->type != kw_new && !tpr->back().second->byClass.dflt && !tpr->back().second->byClass.val)
                {
                    auto t = TypeAliasSearch(tpr->back().first->name, true);
                    if (t)
                    {
                        tpr->back().second->byClass.dflt = t->second->byClass.dflt;
                        tpr->back().first->name = ittps->first->name;
                    }
                }
                SpecifyOneArg(sym, &tpr->back(), origTemplate, origUsing);
            }
            ++ittps;
        }
        auto sp = GetTypeAliasSpecialization(rv->sp, tp->sp->templateParams);
        return sp->tp;
    }
    else if (basetype(tp)->type == bt_templateparam)
    {
        tp = basetype(tp);
        TEMPLATEPARAMPAIR* rv = TypeAliasSearch(tp->templateParam->first->name, false);
        if (rv)
        {
            TEMPLATEPARAM* tpp = Allocate<TEMPLATEPARAM>();
            *tpp = *tp->templateParam->second;
            tp->templateParam->second = tpp;
            if (rv->second->packed && !tp->templateParam->second->ellipsis)
            {
                if (rv->second->byPack.pack && packIndex < rv->second->byPack.pack->size())
                {
                    auto it = rv->second->byPack.pack->begin();
                    for (int i = 0; i < packIndex; ++i, ++it)
                        ;
                    tp->templateParam->second->byClass.dflt = it->second->byClass.dflt;
                }
            }
            else
            {
                tp->templateParam->second->byClass.dflt = rv->second->byClass.dflt;
            }
        }
    }
    else if (isstructured(tp))
    {
        if (basetype(tp)->sp->sb->templateLevel && !basetype(tp)->sp->sb->instantiated)
        {
            basetype(tp)->sp = CopySymbol(basetype(tp)->sp);
            std::list<TEMPLATEPARAMPAIR>* tpx = basetype(tp)->sp->templateParams;
            auto args1 = templateParamPairListFactory.CreateList();
            if (tpx)
            {
                for (auto&& tpl : *tpx)
                {
                    if (tpl.second->type != kw_new)
                    {
                        args1->push_back(TEMPLATEPARAMPAIR{tpl.first, Allocate<TEMPLATEPARAM>()});
                        *args1->back().second = *tpl.second;
                        if (args1->back().second->type == kw_int || args1->back().second->type == kw_typename)
                        {
                            if (args1->back().second->packed && (!args1->back().second->byPack.pack || !args1->back().second->byPack.pack->size()))
                            {
                                SearchAlias(args1->back().first->name, &args1->back(), sym, origTemplate, origUsing);                            
                            }
                            else if (args1->back().second->byClass.dflt)
                            {
                                if (args1->back().second->packed)
                                {
                                    if (packIndex >= 0 && !args1->back().second->ellipsis)
                                    {
                                        std::list<TEMPLATEPARAMPAIR>* tpx = args1->back().second->byPack.pack;
                                        if (tpx && packIndex < tpx->size())
                                        {
                                            auto it = tpx->begin();
                                            for (int i = 0; i < packIndex; i++, ++it)
                                                ;
                                            *args1->back().second = *it->second;
                                        }
                                    }
                                    else if (args1->back().second->ellipsis)
                                    {
                                        if (args1->back().second->byPack.pack)
                                        {
                                            *args1->back().second = *args1->back().second->byPack.pack->front().second;
                                            SpecifyOneArg(sym, &args1->back(), origTemplate, origUsing);
                                        }
                                    }
                                    else if (args1->back().second->byPack.pack)
                                    {
                                        for (auto&& t : *args1->back().second->byPack.pack)
                                        {
                                            SpecifyOneArg(sym, &t, origTemplate, origUsing);
                                        }
                                    }
                                }
                                else
                                {
                                    SpecifyOneArg(sym, &args1->back(), origTemplate, origUsing);
                                }
                            }
                            else if (args1->back().first)
                            {
                                SearchAlias(args1->back().first->name, &args1->back(), sym, origTemplate, origUsing);
                            }
                        }
                    }
                }
            }
            SYMBOL* sp1 = GetClassTemplate(basetype(tp)->sp, args1, true);
            if (sp1)
            {
                sp1->tp = PerformDeferredInitialization(sp1->tp, nullptr);
                if (sp1->templateParams)
                {
                    std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tas;
                    auto ita = sp1->templateParams->begin();
                    auto itea = sp1->templateParams->end();
                    for (++ita; ita != itea;)
                    {
                        if (ita->second->packed && ita->second->byPack.pack)
                        {
                            tas.push(ita);
                            tas.push(itea);
                            itea = ita->second->byPack.pack->end();
                            ita = ita->second->byPack.pack->begin();
                        }
                        if (ita != itea)
                        {
                            ita->second->byClass.dflt = ita->second->byClass.val;
                            ++ita;
                        }
                        if (ita == itea && !tas.empty())
                        {
                            itea = tas.top();
                            tas.pop();
                            ita = tas.top();
                            tas.pop();
                            ++ita;
                        }
                    }
                }
                basetype(tp)->sp = sp1;
            }
        }
    }
    else if (tp->type == bt_templatedecltype)
    {
        static int nested;
        if (nested >= 10)
            return rv;
        nested++;
        tp->templateDeclType = SpecifyArgInt(sym, tp->templateDeclType, orig, origTemplate, origUsing);
        nested--;
    }
    else if (basetype(tp)->type == bt_templateselector)
    {
        std::list<TEMPLATEPARAMPAIR>** tplp = nullptr;
        basetype(tp)->sp = CopySymbol(basetype(tp)->sp);
        auto oldx = basetype(tp)->sp->sb->templateSelector;
        if (oldx)
        {
            auto rvs = basetype(tp)->sp->sb->templateSelector = templateSelectorListFactory.CreateVector();
            bool first = true;
            for (auto&& old : *oldx)
            {
                rvs->push_back(old);
                if (old.isDeclType)
                {
                    first = false;
                    rvs->back().tp = CopyType(old.tp);
                    rvs->back().tp->templateDeclType =
                        SpecifyArgInt(sym, rvs->back().tp->templateDeclType, orig, origTemplate, origUsing);
                    auto tp1 = TemplateLookupTypeFromDeclType(rvs->back().tp);
                    rvs->back().isDeclType = false;
                    rvs->back().sp = makeID(sc_auto, tp1, nullptr, AnonymousName());
                }
                else
                {
                    if (first && old.sp)
                    {
                        first = false;
                        if (old.sp->tp->type == bt_templateparam)
                        {
                            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(old.sp->name, false);
                            if (rv && rv->second->type == kw_typename)
                            {
                                TYPE* tp = rv->second->byClass.val ? rv->second->byClass.val : rv->second->byClass.dflt;
                                if (tp && isstructured(tp))
                                    rvs->back().sp = basetype(tp)->sp;
                            }
                        }
                    }
                    if (old.templateParams)
                    {
                        auto tpr = rvs->back().templateParams = templateParamPairListFactory.CreateList();
                        for (auto&& temp : *old.templateParams)
                        {
                            tpr->push_back(TEMPLATEPARAMPAIR{temp.first, Allocate<TEMPLATEPARAM>()});
                            *tpr->back().second = *temp.second;
                            if (tpr->back().second->packed)
                            {
                                if (packIndex >= 0 && !tpr->back().second->ellipsis)
                                {
                                    std::list<TEMPLATEPARAMPAIR>* tpx = tpr->back().second->byPack.pack;
                                    if (tpx && packIndex < tpx->size())
                                    {
                                        auto it = tpx->begin();
                                        for (int i = 0; i < packIndex; ++i, ++it)
                                            ;
                                        *tpr->back().second = *it->second;
                                    }
                                }
                                else if (tpr->back().second->ellipsis)
                                {
                                    if (tpr->back().second->byPack.pack)
                                    {
                                        *tpr->back().second = *tpr->back().second->byPack.pack->front().second;
                                        SpecifyOneArg(sym, &tpr->back(), origTemplate, origUsing);
                                    }
                                }
                                else if (tpr->back().second->byPack.pack)
                                {
                                    for (auto&& t : *tpr->back().second->byPack.pack)
                                    {
                                        SpecifyOneArg(sym, &t, origTemplate, origUsing);
                                    }
                                }
                            }
                            else
                            {
                                SpecifyOneArg(sym, &tpr->back(), origTemplate, origUsing);
                            }
                        }
                    }
                }
            }
        }
        rv = SynthesizeType(rv, nullptr, false);
    }
    return rv;
}
static void SpecifyOneArg(SYMBOL* sym, TEMPLATEPARAMPAIR* temp, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                          std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    SYMBOL* syms[200];
    int count = 0, n = 0;
    if (temp && temp->second->ellipsis && temp->second->type != kw_template)
    {
        if (temp->second->type == kw_typename)
        {
            TYPE* tp1 = temp->second->packed ? (temp->second->byPack.pack && temp->second->byPack.pack->size()
                                                    ? temp->second->byPack.pack->front().second->byClass.dflt
                                                    : nullptr)
                                             : temp->second->byClass.dflt;
            GatherPackedTypes(&count, syms, basetype(tp1));
        }
        else
        {
            EXPRESSION* exp1 = temp->second->packed ? (temp->second->byPack.pack && temp->second->byPack.pack->size()
                                                           ? temp->second->byPack.pack->front().second->byNonType.dflt
                                                           : nullptr)
                                                    : temp->second->byNonType.dflt;
            GatherPackedVars(&count, syms, exp1);
        }
        for (int i = 0; i < count; i++)
        {
            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(syms[i]->name, false);
            // only care about unbound ellipsis at this level
            if (!syms[i]->tp->templateParam->second->ellipsis)
            {
                if (rv && rv->second->packed)
                {
                    int n1 = CountPacks(rv->second->byPack.pack);
                    if (n1 > n)
                        n = n1;
                }
            }
        }
    }
    n--;
    int oldIndex = packIndex;
    void* hold[200];
    TEMPLATEPARAMPAIR* tpx = temp;
    if (tpx->second->packed && tpx->second->byPack.pack && tpx->second->byPack.pack->size())
        tpx = &tpx->second->byPack.pack->front();
    for (int i = count == 0 ? -1 : 0; i <= n; i++)
    {
        if (n >= 0)
            packIndex = i;
        if (i >= 0)
            hold[i] = 0;
        switch (tpx->second->type)
        {
            case kw_int: {
                std::list<TEMPLATEPARAMPAIR> a{*tpx};
                // the checked for pack here wasn't done before, it relied on dflt being null
                // should be looked at more closely...
                auto rv = SpecifyArgInt(sym,
                                       tpx->second->byNonType.dflt && (!tpx->second->packed || !tpx->second->byPack.pack)
                                           ? tpx->second->byNonType.dflt
                                           : tpx->second->byNonType.val,
                                       &a,
                                        origTemplate, origUsing);
                if (rv)
                {
                    optimize_for_constants(&rv);
                    while (rv->type == en_void && rv->right)
                        rv = rv->right;
                }
                if (i >= 0)
                    hold[i] = rv;
                else
                    tpx->second->byNonType.dflt = rv;
                break;
            }
            case kw_template: {
                break;
            }
            case kw_typename: {
                std::list<TEMPLATEPARAMPAIR> a{*tpx};
                auto rv = SpecifyArgType(sym, tpx->second->byClass.dflt ? tpx->second->byClass.dflt : tpx->second->byClass.val,
                                         tpx->second, &a,
                                         origTemplate, origUsing);
                if (i >= 0)
                    hold[i] = rv;
                else
                    tpx->second->byClass.dflt = rv;
                break;
            }
            default:
                break;
        }
    }
    packIndex = oldIndex;
    if (count != 0)
    {
        std::list<TEMPLATEPARAMPAIR>* packList = templateParamPairListFactory.CreateList();
        for (int i = 0; i <= n; i++)
        {
            if (hold[i])
            {
                packList->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                *packList->back().second = *temp->second;
                if (temp->second->type == kw_template)
                    packList->back().second->byTemplate.args = (std::list<TEMPLATEPARAMPAIR>*)hold[i];
                else
                    packList->back().second->byClass.dflt = (TYPE*)hold[i];
                packList->back().second->packed = false;
            }
        }
        temp->second->byPack.pack = packList;
        temp->second->packed = true;
    }
}
static bool ParseTypeAliasDefaults(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                                   std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    if (origTemplate && args)
    {
        std::list<TEMPLATEPARAMPAIR>::iterator tpl2, tpl3;
        for (tpl2 = origTemplate->begin(), tpl3 = args->begin(); tpl2 != origTemplate->end();
             ++tpl2, tpl3 == args->end() ? tpl3 : ++tpl3)
            if ((tpl3 == args->end() || !tpl3->second->byClass.dflt) && tpl2->second->byClass.txtdflt)
                break;
        if (tpl2 != origTemplate->end())
        {
            auto ita = args->begin();
            auto ittp = origTemplate->begin();
            ++ittp;
            for ( ; ita != args->end() && ittp != origTemplate->end() ; ++ita, ++ittp)
            {
                if (ittp->second->packed)
                {
                    std::list<TEMPLATEPARAMPAIR>* tpl1 = ita->second->byPack.pack;
                    std::list<TEMPLATEPARAMPAIR>* tplp1 = ittp->second->byPack.pack = templateParamPairListFactory.CreateList();
                    if (ita->second->packed)
                    {
                        for (auto&& tplx : *tpl1)
                        {
                            tplp1->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            *tplp1->back().second = *tplx.second;
                        }
                    }
                    else
                    {
                        tplp1->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                        *tplp1->back().second = *ita->second;
                        tplp1->back().second->byClass.val = tplp1->back().second->byClass.dflt;
                    }
                }
                else
                {
                    if (ittp->second->byClass.txtdflt)
                        ittp->second->byClass.val = nullptr;
                    else
                        ittp->second->byClass.val =
                            ita->second->byClass.val ? ita->second->byClass.val : ita->second->byClass.dflt;
                }
            }
            for (;ittp != origTemplate->end(); ++ittp)
            {
                args->push_back(TEMPLATEPARAMPAIR{ittp->first, Allocate<TEMPLATEPARAM>()});
                *args->back().second = *ittp->second;
                if (ittp->second->byClass.txtdflt)
                    ittp->second->byClass.val = nullptr;
            }
            if (!templateNestingCount && !TemplateParseDefaultArgs(sp, nullptr, sp->templateParams, sp->templateParams,
                                                                   sp->templateParams))
            {
                return false;
            }
            ittp = origTemplate->begin();
            ++ittp;
            ita = args->begin();
            for ( ; ittp != origTemplate->end(); ++ita, ++ittp)
            {
                if (ita->second->packed)
                {
                    std::list<TEMPLATEPARAMPAIR>* tpl1 = ita->second->byPack.pack;
                    std::list<TEMPLATEPARAMPAIR>* tplp1 = ittp->second->byPack.pack;
                    if (ittp->second->packed)
                    {
                        if (tpl1 && tplp1)
                        {
                            auto itpl = tpl1->begin();
                            auto itplp = tplp1->begin();
                            for ( ; itpl != tpl1->end(); ++ itpl, ++itplp)
                            {
                                if (itplp->second->byClass.val)
                                    itpl->second->byClass.dflt = itplp->second->byClass.val;
                            }
                        }
                    }
                    else
                    {
                        if (tplp1 && tplp1->front().second->byClass.val)
                        {
                            ittp->second->byClass.dflt = tplp1->front().second->byClass.val;
                        }
                    }
                }
                else
                {
                    if (ittp->second->byClass.val)
                    {
                        ita->second->byClass.dflt = ittp->second->byClass.val;
                    }
                }
            }
        }
    }
    return true;
}
std::list<TEMPLATEPARAMPAIR>* GetTypeAliasArgs(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                                    std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    if (!ParseTypeAliasDefaults(sp, args, origTemplate, origUsing))
        return args;
    auto args1 = templateParamPairListFactory.CreateList();
    for (auto temp : *origUsing)
    {
        args1->push_back(TEMPLATEPARAMPAIR{temp.first, Allocate<TEMPLATEPARAM>()});
        *args1->back().second = *temp.second;
        if (!args1->back().second->byClass.dflt)
        {
            args1->back().second->byClass.dflt = args1->back().second->byClass.val;
        }
        if (args1->back().first && !args1->back().second->byClass.dflt)
        {
            std::list<TEMPLATEPARAMPAIR>::iterator itargs2 = args->begin();
            for (auto test : *origTemplate)
            {
                if (test.second->type != kw_new)
                {
                    if (test.first && !strcmp(test.first->name, args1->back().first->name))
                    {
                        if (itargs2 == args->end())
                        {
                            if (!test.second->packed)
                                Utils::fatal("internal error");
                        }
                        else if (args1->back().second->packed)
                        {
                            if (itargs2->second->packed)
                            {
                                args1->back().second->byPack.pack = itargs2->second->byPack.pack;
                            }
                            else
                            {
                                if (!args1->back().second->byPack.pack)
                                    args1->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                args1->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, itargs2->second});
                            }
                        }
                        else
                        {
                            args1->back().second->byClass.dflt = itargs2->second->byClass.dflt ? itargs2->second->byClass.dflt : itargs2->second->byClass.val;
                        }
                    }
                    if (itargs2 != args->end())
                    {
                        ++itargs2;
                    }
                }
            }
        }
    }
    if (!templateNestingCount || instantiatingTemplate)
    {
        for (auto&& temp : *args1)
        {
            if (temp.second->packed)
            {
                if (temp.second->byPack.pack)
                {
                    TEMPLATEPARAM temp1 = *temp.second;
                    temp.second = Allocate<TEMPLATEPARAM>();
                    *(temp.second) = temp1;
                    for (auto t : *temp.second->byPack.pack)
                    {
                        SpecifyOneArg(sp, &t, origTemplate, args1);
                    }
                }
            }
            else
            {
                SpecifyOneArg(sp, &temp, origTemplate, args1);
            }
        }
    }
    args1 = ResolveDeclTypes(sp, args1);
    args1 = ResolveTemplateSelectors(sp, args1, false);
    return args1;
}
static std::list<TEMPLATEPARAMPAIR>* TypeAliasAdjustArgs(std::list<TEMPLATEPARAMPAIR>* tpx, std::list<TEMPLATEPARAMPAIR>* args)
{
    int n = args->size();
    std::list<TEMPLATEPARAMPAIR>* t;
    if (tpx->size() > args->size())
    {
        auto ita = args->begin();
        auto itt = tpx->begin();
        if (itt->second->type == kw_new)
            ++itt;
        for ( ; itt != tpx->end() && ita != args->end(); ++itt, ++ita)
        {
            if (ita->second->packed)
                break;
        }
        if (itt != tpx->end() && ita != args->end() && (*ita).second->byPack.pack)
        {
            auto packed = *ita;
            auto tpn = packed.second->byPack.pack->begin();
            args->erase(ita, args->end());
            while (itt != tpx->end() && !itt->second->packed && itt->second->type == packed.second->type)
            {
                TEMPLATEPARAMPAIR tpx = TEMPLATEPARAMPAIR();
                if (tpn != packed.second->byPack.pack->end())
                {
                    tpx.second = tpn->second;
                    ++tpn;
                }
                else
                {
                    tpx.second = Allocate<TEMPLATEPARAM>();
                    tpx.second->type = packed.second->type;
                }
                tpx.first = itt->first;
                args->push_back(tpx);
                ++itt;
            }
            args->push_back(packed);
        }
    }
    auto ita = args->begin();
    auto itt = tpx->begin();
    if (itt->second->type == kw_new)
        ++itt;
    for (; itt != tpx->end() && n; ++itt, ++ita, n--)
    {
        ita->first = itt->first;
    }
    for (;itt != tpx->end(); ++itt)
    {
        if (itt->second->type != kw_new)
        {
            itt->second->byClass.dflt = nullptr;
            itt->second->byClass.val = nullptr;
        }
    }
    return args;
}
SYMBOL* GetTypeAliasSpecialization(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    bool checked = false;
    TEMPLATEPARAMPAIR old;
    std::list<TEMPLATEPARAMPAIR> temp;
    if (!args)
    {
        args = &temp;
    }
    else if (args->front().second->type == kw_new)
    {
        checked = true;
        old = args->front();
        args->pop_front();
    }
    SYMBOL* rv;
    // if we get here we have a templated typedef
    args = TypeAliasAdjustArgs(sp->templateParams, args);
    for (auto&& a : *args)
    {
        SpecifyOneArg(sp, &a, sp->templateParams, sp->sb->typeAlias);
    }
    STRUCTSYM t1;
    if (sp->sb->parentClass && sp->sb->parentClass->templateParams)
    {
        t1.tmpl = sp->sb->parentClass->templateParams;
        addTemplateDeclaration(&t1);
    }
    STRUCTSYM t;
    t.tmpl = args;
    if (sp->name[0] == '_' && sp->name[1] == '_')
    {
        if (sp->name[2] == 't' && !strcmp(sp->name, "__type_pack_element"))
        {
            if (checked)
                args->push_front(old);
            return TypePackElement(sp, args);
        }
        if (sp->name[2] == 'm' && !strcmp(sp->name, "__make_integer_seq"))
        {
            if (checked)
                args->push_front(old);
            return MakeIntegerSeq(sp, args);
        }
    }
    addTemplateDeclaration(&t);
    TYPE* basetp = sp->tp->btp;
    while (ispointer(basetp))
        basetp = basetp->btp;
    if (basetp->type == bt_templatedecltype)
    {
        rv = CopySymbol(sp);
        rv->sb->mainsym = sp;
        if (!templateNestingCount)
        {
            basetp = SpecifyArgType(basetp->sp, basetp, nullptr, nullptr, sp->templateParams, sp->sb->typeAlias);
            rv->tp = TemplateLookupTypeFromDeclType(basetp);
            if (!rv->tp)
                rv->tp = &stdany;
        }
    }
    else if (basetp->type == bt_templateselector)
    {
        rv = CopySymbol(sp);
        rv->sb->mainsym = sp;
        if (!ParseTypeAliasDefaults(rv, args, sp->templateParams, sp->sb->typeAlias))
        {
            dropStructureDeclaration();
            if (sp->sb->parentClass && sp->sb->parentClass->templateParams)
            {
                dropStructureDeclaration();
            }
            if (checked)
                args->push_front(old);
            return rv;
        }
        SpecifyTemplateSelector(&rv->sb->templateSelector, basetp->sp->sb->templateSelector, false, sp, sp->templateParams,
                                sp->sb->typeAlias);
        dropStructureDeclaration();
        if (sp->sb->parentClass && sp->sb->parentClass->templateParams)
        {
            dropStructureDeclaration();
        }
        if (!inTemplateHeader)
        {
            TYPE tp1 = {};
            MakeType(tp1, bt_templateselector);
            tp1.sp = rv;
            rv->tp = SynthesizeType(&tp1, args, false);
            if (isstructured(rv->tp))
                rv = basetype(rv->tp)->sp;
            basetp = sp->tp->btp;
            if (ispointer(basetp))
            {
                rv = makeID(sc_type, rv->tp, nullptr, AnonymousName());
                TYPE* tpr = nullptr, ** last = &tpr;
                while (ispointer(basetp))
                {
                    *last = Allocate<TYPE>();
                    **last = *basetp;
                    basetp = basetp->btp;
                    last = &(*last)->btp;
                }
                *last = rv->tp;
                rv->tp = tpr;
            }
        }
        else
        {
            TYPE* tp1 = MakeType(bt_templateselector);
            tp1->sp = rv;
            rv->tp = tp1;
        }
        if (checked)
            args->push_front(old);
        return rv;
    }
    else if (basetp->type == bt_typedef || (isstructured(basetp) && basetype(basetp)->sp->sb->templateLevel))
    {
        std::list<TEMPLATEPARAMPAIR>* newParams = GetTypeAliasArgs(sp, args, sp->templateParams, sp->sb->typeAlias);
        if (basetp->type == bt_typedef)
        {
            rv = GetTypeAliasSpecialization(basetp->sp, newParams);
        }
        else
        {
            rv = GetClassTemplate(basetype(basetp)->sp, newParams, false);
        }
    }
    else
    {
        rv = CopySymbol(sp);
        rv->sb->mainsym = sp;
        rv->tp = rv->tp->btp;
        TYPE** tp = &rv->tp;
        while (ispointer(*tp) || isref(*tp))
        {
            *tp = CopyType(*tp);
            tp = &(*tp)->btp;
        }
        while (*tp != basetype(*tp))
        {
            *tp = CopyType(*tp);
            tp = &(*tp)->btp;
        }
        if ((*tp)->type == bt_templateparam)
        {
            auto itarg = args->begin();
            auto itorig = sp->templateParams->begin();
            ++itorig;
            for (; itarg != args->end() && itorig != sp->templateParams->end(); ++ itarg, ++itorig)
            {
                if (itarg->second->type == kw_typename && strcmp((*tp)->templateParam->first->name, itorig->first->name) == 0)
                {
                    if (!itarg->second->packed)
                    {
                        *tp = itarg->second->byClass.dflt;
                    }
                    break;
                }
            }
        }
        dropStructureDeclaration();
        if (sp->sb->parentClass && sp->sb->parentClass->templateParams)
        {
            dropStructureDeclaration();
        }
        if (checked)
            args->push_front(old);
        return rv;
    }
    basetp = sp->tp->btp;
    if (ispointer(basetp))
    {
        TYPE* tpr = nullptr, ** last = &tpr;
        rv = makeID(sc_type, rv->tp, rv, AnonymousName());
        while (ispointer(basetp))
        {
            *last = Allocate<TYPE>();
            **last = *basetp;
            basetp = basetp->btp;
            last = &(*last)->btp;
        }
        *last = rv->tp;
        rv->tp = tpr;
    }
    dropStructureDeclaration();
    if (sp->sb->parentClass && sp->sb->parentClass->templateParams)
    {
        dropStructureDeclaration();
    }
    if (checked)
        args->push_front(old);
    return rv;
}
void DoInstantiateTemplateFunction(TYPE* tp, SYMBOL** sp, std::list<NAMESPACEVALUEDATA*>* nsv, SYMBOL* strSym,
                                   std::list<TEMPLATEPARAMPAIR>* templateParams, bool isExtern)
{
    SYMBOL* sym = *sp;
    SYMBOL *spi = nullptr, *ssp;
    SYMLIST** p = nullptr;
    if (nsv)
    {
        auto rvl = tablesearchone(sym->name, nsv->front(), false);
        if (rvl)
            spi = rvl;
        else
            errorNotMember(strSym, nsv->front(), sym->name);
    }
    else
    {
        ssp = getStructureDeclaration();
        if (ssp)
        {
            spi = ssp->tp->syms->Lookup(sym->name);
        }
        if (!spi)
        {
            spi = namespacesearch(sym->name, globalNameSpace, false, false);
        }
    }
    if (spi)
    {
        if (spi->sb->storage_class == sc_overloads)
        {
            FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
            SYMBOL* instance;
            auto hr = basetype(tp)->syms->begin();
            auto hre = basetype(tp)->syms->end();
            funcparams->arguments = initListListFactory.CreateList();
            funcparams->templateParams = templateParams->front().second->bySpecialization.types;
            funcparams->ascall = true;
            if (templateParams->front().second->bySpecialization.types)
                funcparams->astemplate = true;
            if ((*hr)->sb->thisPtr)
                ++hr;
            while (hr != hre)
            {
                auto init = Allocate<INITLIST>();
                init->tp = (*hr)->tp;
                init->exp = intNode(en_c_i, 0);
                funcparams->arguments->push_back(init);
                ++hr;
            }
            if (spi->sb->parentClass)
            {
                funcparams->thistp = MakeType(bt_pointer, spi->sb->parentClass->tp);
                funcparams->thisptr = intNode(en_c_i, 0);
            }
            instance = GetOverloadedTemplate(spi, funcparams);
            if (instance)
            {
                if (instance->sb->templateLevel)
                {
                    if (!isExtern)
                    {
                        instance = TemplateFunctionInstantiate(instance, true);
                        spi->sb->attribs.inheritable.linkage4 = lk_virtual;
                        InsertInline(instance);
                    }
                }
                *sp = instance;
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
}
static void referenceInstanceMembers(SYMBOL* cls, bool excludeFromExplicitInstantiation)
{
    if (Optimizer::cparams.prm_xcept)
    {
        RTTIDumpType(cls->tp);
    }
    if (cls->sb->vtabsp)
    {
        Optimizer::SymbolManager::Get(cls->sb->vtabsp);
    }
    if (cls->tp->syms)
    {
        BASECLASS* lst;
        for (auto sym : *cls->tp->syms)
        {
            if (sym->sb->storage_class == sc_overloads)
            {
                auto sp = sym;
                for (auto sym : *sp->tp->syms)
                {
                    if (sym->sb->templateLevel <= cls->sb->templateLevel && !sym->templateParams)
                    {
                        sym->sb->dontinstantiate = false;
                        sym->sb->attribs.inheritable.linkage4 = lk_virtual;
                        Optimizer::SymbolManager::Get(sym)->dontinstantiate = false;
                        if (!excludeFromExplicitInstantiation && !sym->sb->attribs.inheritable.excludeFromExplicitInstantiation)
                        {
                            if (sym->sb->defaulted && !sym->sb->deleted && !sym->sb->inlineFunc.stmt)
                            {
                                createConstructor(cls, sym);
                            }
                            if (!sym->sb->deleted)
                            {
                                InsertInline(sym);
                            }
                        }
                    }
                }
            }
            else if (!ismember(sym) && !istype(sym))
            {
                if (cls->sb->templateLevel || sym->sb->templateLevel)
                    InsertInlineData(sym);
            }
        }

        if (cls->tp->tags)
        {
            for (auto sym : *cls->tp->tags)
            {
                if (isstructured(sym->tp))
                {
                    sym = basetype(sym->tp)->sp;
                    if (sym->sb->parentClass == cls && !sym->templateParams)
                        referenceInstanceMembers(sym, excludeFromExplicitInstantiation ||
                                                          sym->sb->attribs.inheritable.excludeFromExplicitInstantiation);
                }
            }
        }
    }
}
static void dontInstantiateInstanceMembers(SYMBOL* cls, bool excludeFromExplicitInstantiation)
{
    if (cls->tp->syms)
    {
        BASECLASS* lst;
        for (auto sym : *cls->tp->syms)
        {
            if (sym->sb->storage_class == sc_overloads)
            {
                auto sp1 = sym;
                for (auto sym : *sp1->tp->syms)
                {
                    if (sym->sb->templateLevel <= cls->sb->templateLevel && !sym->templateParams)
                    {
                        if (!excludeFromExplicitInstantiation && !sym->sb->attribs.inheritable.excludeFromExplicitInstantiation)
                        {
                            sym->sb->dontinstantiate = true;
                        }
                    }
                }
            }
        }
        if (cls->tp->tags)
        {
            bool first = true;
            for (auto sym : *cls->tp->tags)
            {
                if (!first) // past the definition of self
                {
                    if (isstructured(sym->tp))
                    {
                        sym = basetype(sym->tp)->sp;
                        if (sym->sb->parentClass == cls && !sym->templateParams)
                            dontInstantiateInstanceMembers(sym, excludeFromExplicitInstantiation ||
                                sym->sb->attribs.inheritable.excludeFromExplicitInstantiation);
                    }
                }
                first = false;
            }
        }
    }
}

static bool fullySpecialized(TEMPLATEPARAMPAIR* tpx)
{
    switch (tpx->second->type)
    {
        case kw_typename:
            return !typeHasTemplateArg(tpx->second->byClass.dflt);
        case kw_template: {
            if (tpx->second->byTemplate.args)
            {
                for (auto&& tpxl : *tpx->second->byTemplate.args)
                    if (!fullySpecialized(tpx))
                        return false;
            }
            return true;
        }
        case kw_int:
            if (!tpx->second->byNonType.dflt)
                return false;
            if (!isarithmeticconst(tpx->second->byNonType.dflt))
            {
                EXPRESSION* exp = tpx->second->byNonType.dflt;
                if (exp && tpx->second->byNonType.tp->type != bt_templateparam)
                {
                    while (castvalue(exp) || lvalue(exp))
                        exp = exp->left;
                    switch (exp->type)
                    {
                        case en_pc:
                        case en_global:
                        case en_func:
                            return true;
                        default:
                            break;
                    }
                }
                return false;
            }
            else
            {
                return true;
            }
            break;
        default:
            return false;
    }
}
bool TemplateFullySpecialized(SYMBOL* sp)
{
    if (sp)
    {
        if (sp->templateParams && sp->templateParams->front().second->bySpecialization.types)
        {
            for (auto&& tpx : *sp->templateParams->front().second->bySpecialization.types)
                if (!fullySpecialized(&tpx))
                    return false;
            return true;
        }
    }
    return false;
}

void propagateTemplateDefinition(SYMBOL* sym)
{
    int oldCount = templateNestingCount;
    struct templateListData* oldList = currents;
    templateNestingCount = 0;
    currents = nullptr;
    if (!sym->sb->deferredCompile && !sym->sb->inlineFunc.stmt)
    {
        SYMBOL* parent = sym->sb->parentClass;
        if (parent)
        {
            SYMBOL* old = parent->sb->maintemplate;
            if (!old)
                old = parent;
            if (old && old->tp->syms)
            {
                SYMBOL* p = old->tp->syms->Lookup(sym->name);
                if (p)
                {
                    for (auto cur : *basetype(p->tp)->syms)
                    {
                        if (sym && sym->sb->origdeclline == cur->sb->origdeclline &&
                            !strcmp(sym->sb->origdeclfile, cur->sb->origdeclfile) && cur->sb->deferredCompile)
                        {
                            if (matchesCopy(cur, false) == matchesCopy(sym, false) &&
                                matchesCopy(cur, true) == matchesCopy(sym, true))
                            {
                                sym->sb->deferredCompile = cur->sb->deferredCompile;
                                sym->sb->memberInitializers = cur->sb->memberInitializers;
                                sym->sb->pushedTemplateSpecializationDefinition = 1;
                                sym->sb->attribs.inheritable.linkage4 = lk_virtual;
                                if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                                {
                                    auto src = basetype(cur->tp)->syms->begin();
                                    auto srce = basetype(cur->tp)->syms->end();
                                    auto dest = basetype(sym->tp)->syms->begin();
                                    auto deste = basetype(sym->tp)->syms->end();
                                    while (src != srce && dest != deste)
                                    {
                                        (*dest)->name = (*src)->name;
                                        ++src;
                                        ++dest;
                                    }
                                }
                                if (cur->templateParams && sym->templateParams)
                                {
                                    auto itsrc = cur->templateParams->begin();
                                    auto itdest = sym->templateParams->begin();
                                    for (; itsrc != cur->templateParams->end() && itdest != sym->templateParams->end(); ++itsrc, ++itdest)
                                        if (itsrc->first && itdest->first)
                                            itdest->first->name = itsrc->first->name;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            SYMBOL* old;
            int tns = PushTemplateNamespace(sym);
            old = namespacesearch(sym->name, globalNameSpace, false, false);
            if (old)
            {
                for (auto cur : *basetype(old->tp)->syms)
                {
                    if (sym && sym->sb->origdeclline == cur->sb->origdeclline && !strcmp(sym->sb->origdeclfile, cur->sb->origdeclfile) &&
                        cur->sb->deferredCompile)
                    {
                        sym->sb->attribs.inheritable.linkage4 = lk_virtual;
                        sym->sb->deferredCompile = cur->sb->deferredCompile;
                        cur->sb->pushedTemplateSpecializationDefinition = 1;
                        if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                        {
                            auto src = basetype(cur->tp)->syms->begin();
                            auto srce = basetype(cur->tp)->syms->end();
                            auto dest = basetype(sym->tp)->syms->begin();
                            auto deste = basetype(sym->tp)->syms->end();
                            while (src != srce && dest != deste)
                            {
                                (*dest)->name = (*src)->name;
                                ++src;
                                ++dest;
                            }
                        }
                    }
                }
            }
            PopTemplateNamespace(tns);
        }
    }
    currents = oldList;
    templateNestingCount = oldCount;
}
static void MarkDllLinkage(SYMBOL* sp, enum e_lk linkage)
{
    if (linkage != lk_none && sp->sb->attribs.inheritable.linkage2 != linkage)
    {
        if (sp->sb->attribs.inheritable.linkage2 != lk_none)
        {
            errorsym(ERR_ATTEMPING_TO_REDEFINE_DLL_LINKAGE, sp);
        }
        else
        {
            sp->sb->attribs.inheritable.linkage2 = linkage;
            Optimizer::SymbolManager::Get(sp)->isexport = linkage == lk_export;
            Optimizer::SymbolManager::Get(sp)->isimport = linkage == lk_import;
            Optimizer::SymbolManager::Get(sp)->isinternal = linkage == lk_internal;
            if (sp->sb->vtabsp)
            {
                sp->sb->vtabsp->sb->attribs.inheritable.linkage2 = linkage;
                if (sp->sb->vtabsp->sb->symRef)
                {
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isexport = linkage == lk_export;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isimport = linkage == lk_import;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isinternal = linkage == lk_internal;
                }
                if (sp->sb->vtabsp->sb->attribs.inheritable.linkage2 == lk_import)
                {
                    sp->sb->vtabsp->sb->dontinstantiate = true;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->dontinstantiate = true;
                }
            }
            if (sp->tp->syms)
            {
                for (auto sym : *sp->tp->syms)
                {
                    if (sym->sb->storage_class == sc_overloads)
                    {
                        for (auto sp1 : *sym->tp->syms)
                        {
                            if (!(sp1)->templateParams)
                            {
                                (sp1)->sb->attribs.inheritable.linkage2 = linkage;
                                (sp1)->sb->attribs.inheritable.isInline = false;
                                Optimizer::SymbolManager::Get(sp1)->isexport = linkage == lk_export;
                                Optimizer::SymbolManager::Get(sp1)->isimport = linkage == lk_import;
                                Optimizer::SymbolManager::Get(sp1)->isinternal = linkage == lk_internal;
                            }
                        }
                    }
                    else if (!ismember(sym) && !istype(sym))
                    {
                        sym->sb->attribs.inheritable.linkage2 = linkage;
                        Optimizer::SymbolManager::Get(sym)->isexport = linkage == lk_export;
                        Optimizer::SymbolManager::Get(sym)->isimport = linkage == lk_import;
                        Optimizer::SymbolManager::Get(sym)->isinternal = linkage == lk_internal;
                    }
                }
            }
            if (sp->tp->tags)
            {
                for (auto sym : *sp->tp->tags)
                {
                    if (isstructured(sym->tp))
                    {
                        sym = basetype(sym->tp)->sp;
                        if (sym->sb->parentClass == sp && !sym->templateParams)
                            MarkDllLinkage(sym, linkage);
                    }
                }
            }
        }
    }
}
static void DoInstantiate(SYMBOL* strSym, SYMBOL* sym, TYPE* tp, std::list<NAMESPACEVALUEDATA*>* nsv, bool isExtern)
{
    STRUCTSYM s;
    if (strSym)
    {
        s.str = strSym;
        addStructureDeclaration(&s);
    }
    if (isfunction(tp))
    {
        SYMBOL* sp = sym;
        std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sym->sb->parentClass);
        DoInstantiateTemplateFunction(tp, &sp, nsv, strSym, templateParams, isExtern);
        sp->sb->attribs.inheritable.linkage2 = sym->sb->attribs.inheritable.linkage2;
        sym = sp;
        sym->sb->parentClass = strSym;
        SetLinkerNames(sym, lk_cdecl);
        if (!comparetypes(basetype(sp->tp)->btp, basetype(tp)->btp, true))
        {
            errorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp);
        }
        if (isExtern)
        {
            sp->sb->dontinstantiate = true;
        }
        else
        {
            sp->sb->dontinstantiate = false;
            sp->sb->attribs.inheritable.linkage4 = lk_virtual;
            InsertInline(sp);
        }
    }
    else
    {
        SYMBOL *spi = nullptr, *ssp;
        SYMLIST** p = nullptr;
        if (nsv)
        {
            auto rvl = tablesearchone(sym->name, nsv->front(), false);
            if (rvl)
                spi = rvl;
            else
                errorNotMember(strSym, nsv->front(), sym->name);
        }
        else
        {
            ssp = getStructureDeclaration();
            if (ssp)
                spi = ssp->tp->syms->Lookup(sym->name);
            else
                spi = globalNameSpace->front()->syms->Lookup(sym->name);
        }
        if (spi)
        {
            SYMBOL* tmpl = spi;
            while (tmpl)
                if (tmpl->sb->templateLevel)
                    break;
                else
                    tmpl = tmpl->sb->parentClass;
            if ((tmpl && spi->sb->storage_class == sc_static) || spi->sb->storage_class == sc_external)
            {
                TemplateDataInstantiate(spi, true, isExtern);
                spi->sb->dontinstantiate = isExtern;
                spi->sb->attribs.inheritable.linkage4 = lk_virtual;
                if (!comparetypes(sym->tp, spi->tp, true))
                    preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, spi, sym->sb->declfile, sym->sb->declline);
            }
            else
            {
                errorsym(ERR_NOT_A_TEMPLATE, sym);
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
    if (strSym)
    {
        dropStructureDeclaration();
    }
}
bool inCurrentTemplate(const char* name)
{
    if (*currents->ptail)
    {
        for (auto t : **currents->ptail)
            if (t.first && !strcmp(name, t.first->name))
                return true;
    }
    return false;
}
bool definedInTemplate(const char* name)
{
    for (auto&& s : structSyms)
    {
        if (s.str && s.str->templateParams)
            for (auto&& t : *s.str->templateParams)
                if (t.first && !strcmp(t.first->name, name))
                {
                    if (t.second->packed)
                    {
                        if (s.str->sb->instantiated && !t.second->byPack.pack)
                            return true;
                        bool rv = true;
                        if (t.second->byPack.pack)
                        {
                            for (auto r : *t.second->byPack.pack)
                                if (!r.second->byClass.val)
                                {
                                    rv = false;
                                    break;
                                }
                        }
                        if (rv)
                            return true;
                    }
                    else
                    {
                        if (t.second->byClass.val)
                            return true;
                    }
                }
    }
    return false;
}

LEXLIST* TemplateDeclaration(LEXLIST* lex, SYMBOL* funcsp, enum e_ac access, enum e_sc storage_class, bool isExtern)
{
    SymbolTable<SYMBOL>* oldSyms = localNameSpace->front()->syms;
    lex = getsym();
    localNameSpace->front()->syms = nullptr;
    if (MATCHKW(lex, lt))
    {
        int lasttemplateHeaderCount = templateHeaderCount;
        TYPE* tp = nullptr;
        struct templateListData l;
        int count = 0;
        int oldInstantiatingTemplate = instantiatingTemplate;
        lex = backupsym();
        if (isExtern)
            error(ERR_DECLARE_SYNTAX);

        if (templateNestingCount == 0)
        {
            l.args = nullptr;
            l.ptail = &l.args;
            l.sp = nullptr;
            l.head = l.tail = nullptr;
            l.bodyHead = l.bodyTail = nullptr;
            currents = &l;
        }

        std::stack<std::list<TEMPLATEPARAMPAIR>**> currentHold;

        currentHold.push(currents->plast);
        currents->plast = currents->ptail;
        templateNestingCount++;
        instantiatingTemplate = 0;
        while (MATCHKW(lex, kw_template))
        {
            std::list<TEMPLATEPARAMPAIR>* temp;
            templateHeaderCount++;
            temp = (*currents->ptail) = templateParamPairListFactory.CreateList();
            temp->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            temp->back().second->type = kw_new;
            lex = getsym();
            currents->ptail = &(*currents->ptail)->back().second->bySpecialization.next;

            lex = TemplateHeader(lex, funcsp, temp);
            if (temp->size() > 1)
            {
                count++;
            }
        }
        templateNestingCount--;
        if (lex)
        {
            templateNestingCount++;
            inTemplateType = count != 0;  // checks for full specialization...
            lex = declare(lex, funcsp, &tp, storage_class, lk_none, emptyBlockdata, true, false, true, access);
            inTemplateType = false;
            templateNestingCount--;
            instantiatingTemplate = oldInstantiatingTemplate;
            if (!templateNestingCount)
            {
                if (!tp)
                {
                    error(ERR_TEMPLATES_MUST_BE_CLASSES_OR_FUNCTIONS);
                }
                if (l.sp)
                {
                    if (l.sp->sb->storage_class == sc_typedef && !l.sp->sb->typeAlias)
                    {
                        errorat(ERR_TYPEDEFS_CANNOT_BE_TEMPLATES, "", l.sp->sb->declfile, l.sp->sb->declline);
                    }
                    if (l.sp && isfunction(l.sp->tp) && l.sp->sb->parentClass && !l.sp->sb->deferredCompile)
                    {
                        SYMBOL* srch = l.sp->sb->parentClass;
                        while (srch)
                        {
                            if (srch->sb->deferredCompile)
                                break;
                            srch = srch->sb->parentClass;
                        }
                        if (srch)
                        {
                            std::list<TEMPLATEPARAMPAIR>** srch1 = currents->plast;
                            while (srch1 && srch1 != currents->ptail)
                            {
                                if ((*srch1)->size() > 1)
                                    break;
                                srch1 = &(*srch1)->front().second->bySpecialization.next;
                            }
                            if (srch1 == currents->ptail)
                            {
                                DoInstantiate(l.sp->sb->parentClass, l.sp, l.sp->tp, l.sp->sb->nameSpaceValues, false);
                            }
                        }
                    }
                }
                FlushLineData("", INT_MAX);
            }
        }
        while (count--)
            dropStructureDeclaration();
        templateHeaderCount = lasttemplateHeaderCount;
        (*currents->plast) = nullptr;
        currents->ptail = currents->plast;
        currents->plast = currentHold.top();
        currentHold.pop();
        if (templateNestingCount == 0)
            currents = nullptr;
    }
    else  // instantiation
    {
        if (KWTYPE(lex, TT_STRUCT))
        {
            enum e_lk linkage1 = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            lex = getsym();
            if (MATCHKW(lex, kw__declspec))
            {
                lex = getsym();
                lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
            }
            if (!ISID(lex))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else
            {
                char idname[512];
                SYMBOL* cls = nullptr;
                SYMBOL* strSym = nullptr;
                std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
                lex = id_expression(lex, funcsp, &cls, &strSym, &nsv, nullptr, false, false, idname);
                if (!cls || !isstructured(cls->tp))
                {
                    if (!cls)
                    {
                        errorstr(ERR_NOT_A_TEMPLATE, idname);
                    }
                    else
                    {
                        errorsym(ERR_CLASS_TYPE_EXPECTED, cls);
                    }
                }
                else
                {
                    std::list<TEMPLATEPARAMPAIR>* templateParams = nullptr;
                    SYMBOL* instance;
                    lex = getsym();
                    lex = GetTemplateArguments(lex, funcsp, cls, &templateParams);
                    instance = GetClassTemplate(cls, templateParams, false);
                    if (instance)
                    {
                        MarkDllLinkage(instance, linkage2);
                        if (!isExtern)
                        {
                            instance->sb->explicitlyInstantiated = true;
                            instance->sb->dontinstantiate = false;
                            instance = TemplateClassInstantiate(instance, templateParams, false, sc_global);
                            referenceInstanceMembers(instance, false);
                        }
                        else if (!instance->sb->explicitlyInstantiated)
                        {
                            instance->sb->dontinstantiate = true;
                            instance = TemplateClassInstantiate(instance, templateParams, false, sc_global);
                            dontInstantiateInstanceMembers(instance, false);
                        }
                    }
                    else
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, cls);
                    }
                }
            }
        }
        else
        {
            SYMBOL* sym = nullptr;
            enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            TYPE* tp = nullptr;
            bool defd = false;
            bool notype = false;
            std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            int consdest = 0;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            lex = getBasicType(lex, funcsp, &tp, &strSym, true, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3,
                               ac_public, &notype, &defd, &consdest, nullptr, false, true, false, false, false);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            lex = getBeforeType(lex, funcsp, &tp, &sym, &strSym, &nsv, true, sc_cast, &linkage, &linkage2, &linkage3, nullptr,
                                false, consdest, false, false);
            sizeQualifiers(tp);
            if (!sym)
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else if (notype && !consdest)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else
            {
                if (sym->sb->attribs.inheritable.linkage2 == lk_none)
                    sym->sb->attribs.inheritable.linkage2 = linkage2;
                DoInstantiate(strSym, sym, tp, nsv, false);
            }
        }
    }
    localNameSpace->front()->syms = oldSyms;
    return lex;
}
}  // namespace Parser