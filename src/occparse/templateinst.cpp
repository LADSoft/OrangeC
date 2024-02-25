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
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "libcxx.h"
#include "constexpr.h"
#include "symtab.h"
#include "ListFactory.h"
namespace Parser
{
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
            if (v.second->type == TplType::typename_)
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
                sp->tp = MakeType(BasicType::templateparam_);
                sp->tp->templateParam = &rv->back();
                rv->back().first = sp;
            }
        }
        if (t->front().second->type == TplType::new_ && alsoSpecializations && t->front().second->bySpecialization.types)
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
            if (parse.second->type == TplType::int_)
            {
                if (parse.second->byNonType.tp && parse.second->byNonType.tp->type == BasicType::templateparam_)
                {
                    auto it2 = rv->begin();
                    for (auto&& t1 : *t)
                    {
                        if (t1.second->type == TplType::typename_)
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

void SynthesizeQuals(TYPE*** last, TYPE** qual, TYPE*** lastQual)
{
    if (*qual)
    {
        TYPE* p = **last;
        TYPE* v = *qual;
        int sz = basetype(**last)->size;
        while (p && p->type == BasicType::derivedfromtemplate_)
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
            if (find.second->type != TplType::new_)
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
                                        (*tp1)->type = find.second->lref || (*tp1)->type == BasicType::lref_ ? BasicType::lref_
                                                                                                             : BasicType::rref_;
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
                                tpx.second->byClass.dflt =
                                    MakeType(find.second->lref ? BasicType::lref_ : BasicType::rref_, tpx.second->byClass.dflt);
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

static std::list<TEMPLATEPARAMPAIR>** addStructParam(std::list<TEMPLATEPARAMPAIR>** pt, TEMPLATEPARAMPAIR& search,
                                                     std::list<TEMPLATEPARAMPAIR>* enclosing)
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
                if (sym->tp->type != BasicType::templateparam_ || sym->tp->templateParam->second->type != TplType::typename_)
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
        if (sp->sb->templateLevel && !sp->sb->instantiated && sp->templateParams->size() > 1)
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
                    std::list<TEMPLATEPARAMPAIR>* pt = nullptr;
                    for (auto&& search : *sp->templateParams)
                    {
                        if (search.second->type != TplType::new_)
                        {
                            if (search.second->type == TplType::typename_)
                            {
                                if (search.second->byClass.dflt &&
                                    search.second->byClass.dflt->type == BasicType::templateselector_ &&
                                    search.second->byClass.dflt->sp->sb->postExpansion)
                                {
                                    auto temp = (*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams;
                                    // this may needs some work with recursing templateselectors inside templateselectors...
                                    (*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams =
                                        paramsToDefault((*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams);
                                    expandTemplateSelector(&pt, enclosing, search.second->byClass.dflt);
                                    (*search.second->byClass.dflt->sp->sb->templateSelector)[1].templateParams = temp;
                                }
                                else if (search.second->byClass.dflt &&
                                         (search.second->byClass.dflt)->type == BasicType::memberptr_)
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
                if (sp)
                    sp->tp = PerformDeferredInitialization(sp->tp, nullptr);
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
                    tp1 = MakeType(BasicType::const_, tp1);
                }
                if (isvolatile(tp_in))
                {
                    tp1 = MakeType(BasicType::volatile_, tp1);
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
            case BasicType::typedef_:
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
            case BasicType::pointer_:
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
            case BasicType::templatedecltype_:
                *last = LookupTypeFromExpression(tp->templateDeclType, enclosing, alt);
                if (!*last || (*last)->type == BasicType::any_)
                {
                    return &stdany;
                }
                *last = SynthesizeType(*last, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;
            case BasicType::templateselector_: {
                SYMBOL* sp;
                auto rvsit = tp->sp->sb->templateSelector->begin();
                auto rvsite = tp->sp->sb->templateSelector->end();
                rvsit++;
                SYMBOL* ts = rvsit->sp;
                for (; rvsit != rvsite;)
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
                        for (; current != currente;)
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
                                if (current->second->type == TplType::typename_ && current->second->byClass.dflt)
                                {
                                    if (current->second->byClass.dflt->type == BasicType::templateselector_ &&
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
                                        if (!current->second->byClass.dflt ||
                                            current->second->byClass.dflt->type == BasicType::any_)
                                        {
                                            failed = true;
                                            break;
                                        }
                                    }
                                }
                                else if (current->second->type == TplType::int_)
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
                        if (ts->tp->type == BasicType::typedef_)
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
                        if (!tp || tp->type == BasicType::any_ || !isstructured(tp))
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
                            if (tp->type == BasicType::templateselector_)
                            {
                                tp = SynthesizeType(tp, enclosing, alt);
                                if (tp->type == BasicType::any_)
                                {
                                    return tp;
                                }
                                sp = basetype(tp)->sp;
                            }
                            else if (tp->type == BasicType::templateparam_)
                            {
                                if (tp->templateParam->second->type != TplType::typename_)
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

                            sp->sb->access != AccessLevel::public_ && !resolvingStructDeclarations)
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
                                EXPRESSION* exp = intNode(ExpressionNode::c_i_, 0);
                                FUNCTIONCALL funcparams = {};
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
                        while (tp->type == BasicType::typedef_ && !tp->sp->sb->typeAlias &&
                               tp->btp->type != BasicType::templatedecltype_)
                            tp = tp->btp;
                        TYPE* tp1 = tp;
                        while (ispointer(tp1))
                            tp1 = basetype(tp1)->btp;
                        tp1 = basetype(tp1);
                        if (tp1->type == BasicType::templateselector_)
                        {
                            tp = SynthesizeType(tp, nullptr, false);
                        }
                        if (tp->type == BasicType::templateparam_)
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
                            rv = MakeType(BasicType::derivedfromtemplate_, rv);
                            SynthesizeQuals(&last, &qual, &lastQual);
                        }
                        UpdateRootTypes(rv);
                        return rv;
                    }
                    return &stdany;
                }
            }
            case BasicType::rref_:
                if (qual == nullptr && tp->btp->type == BasicType::templateparam_ && tp->btp->templateParam->second->byClass.val &&
                    tp->btp->templateParam->second->byClass.val->type == BasicType::lref_)
                {
                    TYPE* tp1 = tp->btp->templateParam->second->byClass.val;
                    tp = basetype(tp1);
                    break;
                }
                // fallthrough
            case BasicType::lref_:
                SynthesizeQuals(&last, &qual, &lastQual);
                *last = CopyType(tp);
                last = &(*last)->btp;
                tp = tp->btp;
                break;
            case BasicType::const_:
            case BasicType::volatile_:
            case BasicType::restrict_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::seg_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
                *lastQual = CopyType(tp);
                (*lastQual)->btp = nullptr;
                lastQual = &(*lastQual)->btp;
                tp = tp->btp;
                break;
            case BasicType::memberptr_:
                *last = CopyType(tp);
                {
                    TYPE* tp1 = tp->sp->tp;
                    if (tp1->type == BasicType::templateparam_)
                    {
                        tp1 = tp1->templateParam->second->byClass.val;
                        (*last)->sp = tp1->sp;
                    }
                }
                (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                UpdateRootTypes(rv);
                return rv;

            case BasicType::ifunc_:
            case BasicType::func_: {
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
                                    tp1 = MakeType(BasicType::derivedfromtemplate_, current);
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
                        if (clone->tp->type != BasicType::void_ && clone->tp->type != BasicType::any_)
                        {
                            clone->tp = MakeType(BasicType::derivedfromtemplate_, clone->tp);
                            UpdateRootTypes(clone->tp);
                        }
                    }
                }
                tp = tp->btp;
                break;
            }
            case BasicType::templateparam_: {
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
                if (tpa->second->type == TplType::typename_)
                {
                    TYPE *type = alt ? tpa->second->byClass.temp : tpa->second->byClass.val, *typx = type;
                    while (type && type->type == BasicType::templateparam_)
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
                        tp = MakeType(BasicType::derivedfromtemplate_, rv);
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
                else if (tpa->second->type == TplType::template_)
                {
                    TYPE* type = alt ? tpa->second->byTemplate.temp->tp : tpa->second->byTemplate.val->tp;
                    if (type)
                    {
                        *last = CopyType(type);
                        (*last)->templateTop = true;
                        rv = MakeType(BasicType::derivedfromtemplate_, rv);
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
                            if (tpx.second->type == TplType::typename_ && tpx.second->byClass.temp)
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
                        rv = MakeType(BasicType::derivedfromtemplate_, rv);
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
    if (exp->type == ExpressionNode::templateselector_)
        return false;
    if (exp->left)
        rv &= ValidExp(&exp->left);
    if (exp->right)
        rv &= ValidExp(&exp->right);
    if (exp->type == ExpressionNode::templateparam_)
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
            case BasicType::pointer_:
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
                    if (tp->type == BasicType::templateparam_)
                    {
                        if (tp->templateParam->second->type != TplType::typename_)
                            return false;
                        tp = tp->templateParam->second->byClass.val;
                        if (!tp)
                            return false;
                    }
                    if (tp->type == BasicType::void_ || isfunction(tp) || isref(tp) ||
                        (isstructured(tp) && basetype(tp)->sp->sb->isabstract))
                        return false;
                }
                if (ispointer(tp))
                {
                    while (ispointer(tp))
                        tp = tp->btp;
                    if (tp->type == BasicType::templateparam_)
                    {
                        if (tp->templateParam->second->type != TplType::typename_)
                            return false;
                        return ValidArg(tp);
                    }
                    else if (tp->type == BasicType::templateselector_)
                    {
                        return ValidArg(tp);
                    }
                    if (isref(tp))
                        return false;
                }
                return true;
            case BasicType::templatedecltype_:
                tp = TemplateLookupTypeFromDeclType(tp);
                return !!tp;
                break;
            case BasicType::templateselector_: {
                SYMBOL* ts = (*tp->sp->sb->templateSelector)[1].sp;
                SYMBOL* sp = nullptr;
                auto find = tp->sp->sb->templateSelector->begin();
                auto finde = tp->sp->sb->templateSelector->end();
                if ((*tp->sp->sb->templateSelector)[1].isDeclType)
                {
                    TYPE* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
                    if (!tp1 || tp1->type == BasicType::any_ || !isstructured(tp1))
                        return false;
                    sp = basetype(tp1)->sp;
                }
                else if ((*tp->sp->sb->templateSelector)[1].isTemplate)
                {
                    std::list<TEMPLATEPARAMPAIR>* current = templateParamPairListFactory.CreateList();
                    *current = *(*tp->sp->sb->templateSelector)[1].templateParams;
                    if (ts->tp->type == BasicType::typedef_)
                    {
                        sp = GetTypeAliasSpecialization(ts, current);
                    }
                    else
                    {
                        sp = GetClassTemplate(ts, current, false);
                    }
                    tp = nullptr;
                }
                else if (basetype(ts->tp)->templateParam->second->type == TplType::typename_)
                {
                    tp = basetype(ts->tp)->templateParam->second->byClass.val;
                    if (!tp)
                        return false;
                    sp = basetype(tp)->sp;
                }
                else if (basetype(ts->tp)->templateParam->second->type == TplType::delete_)
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
                    for (; find != finde && sp; ++find)
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
                        if (sp && sp->sb->access != AccessLevel::public_ && !resolvingStructDeclarations)
                        {
                            sp = nullptr;
                            break;
                        }
                    }
                    return find == finde && sp && istype(sp);
                }
                return false;
            }
            case BasicType::lref_:
            case BasicType::rref_:
                tp = basetype(tp)->btp;
                if (tp->type == BasicType::templateparam_)
                {
                    if (tp->templateParam->second->type != TplType::typename_)
                        return false;
                    return ValidArg(tp);
                }
                if (!tp || isref(tp))
                    return false;
                break;
            case BasicType::memberptr_: {
                TYPE* tp1 = tp->sp->tp;
                if (tp1->type == BasicType::templateparam_)
                {
                    if (tp1->templateParam->second->type != TplType::typename_)
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
            case BasicType::const_:
            case BasicType::volatile_:
            case BasicType::restrict_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::seg_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            case BasicType::func_:
            case BasicType::ifunc_: {
                for (auto sp : *tp->syms)
                {
                    if (!ValidArg(sp->tp))
                        return false;
                }
                tp = tp->btp;
                if (tp->type == BasicType::templateparam_)
                {
                    if (tp->templateParam->second->type != TplType::typename_)
                        return false;
                    tp = tp->templateParam->second->byClass.val;
                    if (!tp)
                        return false;
                }
                if (isfunction(tp) || isarray(tp) || (isstructured(tp) && basetype(tp)->sp->sb->isabstract))
                    return false;
                break;
            }
            case BasicType::templateparam_:
                if (tp->templateParam->second->type == TplType::template_)
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
                            if (tpx.second->type == TplType::typename_)
                            {
                                if (tpx.second->packed && tpx.second->byPack.pack)
                                {
                                    // this should be recursive...
                                    for (auto&& tpl1 : *tpx.second->byPack.pack)
                                    {
                                        if (tpl1.second->type == TplType::typename_ && !tpl1.second->packed)
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
                    if (tp->templateParam->second->type != TplType::typename_)
                        return false;
                    if (tp->templateParam->second->packed)
                        return true;
                    if (tp->templateParam->second->byClass.val == nullptr)
                        return false;
                    if (tp->templateParam->second->byClass.val->type == BasicType::void_)
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
            if (param.second->type != TplType::new_)
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
    if (tp->type == BasicType::templateselector_)
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
            if (enc.first && enc.second->type != TplType::new_)
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
            else if (fill.second->type == TplType::int_)
            {
                if (fill.second->byNonType.dflt)
                    FillNontypeExpressionDefaults(fill.second->byNonType.dflt, enclosing);
                else
                    SetTemplateParamValue(&fill, enclosing);
            }
            else if (fill.second->type == TplType::typename_)
            {
                if (!fill.second->byClass.dflt)
                {
                    SetTemplateParamValue(&fill, enclosing);
                }
                else
                {
                    switch (fill.second->byClass.dflt->type)
                    {
                        case BasicType::class_:
                            FillNontypeTemplateParamDefaults(fill.second->byClass.dflt->sp->templateParams, enclosing);
                            break;
                        case BasicType::templateparam_:
                            SetTemplateParamValue(fill.second->byClass.dflt->templateParam, enclosing);
                            break;
                        case BasicType::templateselector_:
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
            if (param.second->type == TplType::int_ && (param.second->byNonType.tp->type == BasicType::templateselector_ ||
                                                        param.second->byNonType.tp->type == BasicType::templateparam_))
            {
                FillNontypeTypeDefaults(param.second->byNonType.tp, enclosing);
                TYPE* tp1 = SynthesizeType(param.second->byNonType.tp, enclosing, false);
                if (!tp1 || tp1->type == BasicType::any_ || isstructured(tp1) || isref(tp1) || isfloat(tp1) || iscomplex(tp1) ||
                    isimaginary(tp1))
                    return false;
            }
        }
    }
    return true;
}
SYMBOL* ValidateArgsSpecified(std::list<TEMPLATEPARAMPAIR>* params, SYMBOL* func, std::list<INITLIST*>* args,
                                     std::list<TEMPLATEPARAMPAIR>* nparams)
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
            if ((*ita)->tp && isstructured((*ita)->tp) && basetype((*ita)->tp)->sp->sb->templateLevel &&
                sameTemplate(func->sb->parentClass->tp, (*ita)->tp, true))
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
            if (param.second->type == TplType::typename_ || param.second->type == TplType::template_ ||
                param.second->type == TplType::int_)
                if (!param.second->packed && !param.second->byClass.val)
                {
                    inDefaultParam--;
                    return nullptr;
                }
        }
    }
    if (it != ite && (*it)->tp->type == BasicType::void_)
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
            if (basetype(sp1->tp)->type == BasicType::ellipse_ || sp1->packed)
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
                lex = initialize(lex, func, sp, StorageClass::parameter_, true, false, _F_TEMPLATEARGEXPANSION);
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
            auto itspecial =
                itparams->second->bySpecialization.types ? itparams->second->bySpecialization.types->begin() : itparams;
            auto itespecial =
                itparams->second->bySpecialization.types ? itparams->second->bySpecialization.types->end() : iteparams;
            for (; itspecial != itespecial; ++itspecial)
            {
                if (itspecial->second->type != TplType::new_)
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
        for (; ittpl != itetpl;)
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
                if (ittpl->second->type == TplType::typename_)
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
                else if (ittpl->second->type == TplType::int_)
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
bool TemplateParseDefaultArgs(SYMBOL* declareSym, std::list<TEMPLATEPARAMPAIR>* args, std::list<TEMPLATEPARAMPAIR>* dest,
                              std::list<TEMPLATEPARAMPAIR>* src, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    auto sp = declareSym;
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
        if (itSrc->second->type == TplType::new_)
            ++itSrc;
    }
    if (dest)
    {
        itDest = dest->begin();
        iteDest = dest->end();
        if (itDest->second->type == TplType::new_)
            ++itDest;
    }
    if (args)
    {
        itArgs = args->begin();
        iteArgs = args->end();
        if (itArgs != args->end() && itArgs->second->type == TplType::new_)
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
                case TplType::typename_: {
                    noTypeNameError++;
                    lex = get_type_id(lex, &itDest->second->byClass.val, nullptr, StorageClass::cast_, false, true, false);
                    noTypeNameError--;
                    if (!itDest->second->byClass.val || itDest->second->byClass.val->type == BasicType::any_ ||
                        (!templateNestingCount && itDest->second->byClass.val->type == BasicType::templateselector_))
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
                case TplType::template_: {
                    char buf[256];
                    strcpy(buf, lex->data->value.s.a);
                    lex =
                        id_expression(lex, nullptr, &itDest->second->byTemplate.val, nullptr, nullptr, nullptr, false, false, buf, 0);

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
                case TplType::int_: {
                    TYPE* tp1;
                    EXPRESSION* exp1 = nullptr;
                    if (itDest->second->byNonType.txttype)
                    {
                        LEXLIST* start = lex;
                        lex = SetAlternateLex(itSrc->second->byNonType.txttype);
                        openStructs = nullptr;
                        structLevel = 0;
                        noTypeNameError++;
                        lex = get_type_id(lex, &tp1, nullptr, StorageClass::parameter_, true, false, false);
                        noTypeNameError--;
                        openStructs = oldOpenStructs;
                        structLevel = oldStructLevel;
                        SetAlternateLex(nullptr);
                        lex = start;
                        TYPE* tp2 = tp1;
                        while (ispointer(tp2))
                            tp2 = basetype(tp2)->btp;
                        if (tp2->type == BasicType::any_)
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
                        if (itDest->second->byNonType.tp->type == BasicType::auto_)
                            itDest->second->byNonType.tp = tp1;
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
                    if (!tp2 || tp2->type == BasicType::any_)
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
                    case TplType::int_: {
                        EXPRESSION* exp = arg.second->byNonType.val;
                        if (exp)
                            exp = GetSymRef(exp);
                        if (exp)
                        {
                            SetTemplateArgAccess(exp->v.sp, accessible);
                        }
                        break;
                    }
                    case TplType::template_: {
                        if (!allTemplateArgsSpecified(nullptr, arg.second->byTemplate.args))
                            return;
                        if (arg.second->byTemplate.val)
                            SetTemplateArgAccess(arg.second->byTemplate.val, accessible);
                    }
                    break;
                    case TplType::typename_:
                        if (arg.second->byClass.val)
                        {
                            if (isstructured(arg.second->byClass.val))
                            {
                                SetTemplateArgAccess(basetype(arg.second->byClass.val)->sp, accessible);
                            }
                            else if (basetype(arg.second->byClass.val)->type == BasicType::enum_)  // DAL FIXED
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
    if (ita != itae && ita->second->type == TplType::new_)
        ++ita;
    (void)args;
    LEXLIST* lex = nullptr;
    SYMBOL* cls = sym;
    int pushCount;
    if (cls->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
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
        if (sym->sb->decoratedName)
        {
            auto itx = classInstantiationMap.find(sym->sb->decoratedName);
            if (itx != classInstantiationMap.end())
                return itx->second;
        }
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
            if (sym->sb->decoratedName)
                classInstantiationMap[sym->sb->decoratedName] = cls;
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
            NAMESPACEVALUEDATA nsvd = {};
            std::list<NAMESPACEVALUEDATA*> ns{&nsvd};
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
            cls->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
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
            lex = innerDeclStruct(lex, nullptr, cls, false,
                                  cls->tp->type == BasicType::class_ ? AccessLevel::private_ : AccessLevel::public_,
                                  cls->sb->isfinal, &defd, nullptr);
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
SYMBOL* TemplateClassInstantiate(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern, StorageClass storage_class)
{
    if (templateNestingCount)
    {
        SYMBOL* sym1 = MatchSpecialization(sym, args);
        if (sym1 && (storage_class == StorageClass::parameter_ || !inTemplateBody))
        {
            auto last = templateParamPairListFactory.CreateList();
            last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            last->back().second->type = TplType::new_;
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
            if (data->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
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
    SetLinkerNames(sym, Linkage::cdecl_);
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
                    if (pP.second->type == TplType::typename_)
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
                for (; itP != P->end() && itA != iteA;)
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
                        if (A && itP->second->type == TplType::typename_)
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
            if (pP.second->type == TplType::typename_)
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
                        if (td->type == BasicType::memberptr_)
                            td = basetype(td)->btp;
                        if (tv->type == BasicType::memberptr_)
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
                        if (pP.second->type == TplType::typename_)
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
                            if (pP.second->type == TplType::typename_)
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
void TransferClassTemplates(std::list<TEMPLATEPARAMPAIR>* dflt, std::list<TEMPLATEPARAMPAIR>* val,
                                   std::list<TEMPLATEPARAMPAIR>* params)
{
    if (!dflt || !val)
        return;
    auto itdflt = dflt->begin();
    auto itval = val->begin();
    if (itdflt->second->type == TplType::new_)
        ++itdflt;
    if (itval->second->type == TplType::new_)
        ++itval;
    bool ptr = false;
    TYPE *tdv = nullptr, *tdd = nullptr;
    TYPE *tvv = nullptr, *tvd = nullptr;
    if (itdflt != dflt->end() && itval != val->end() && itdflt->second->type == TplType::typename_ && itdflt->second->byClass.val &&
        itval->second->byClass.val && ispointer(itdflt->second->byClass.val) && ispointer(itval->second->byClass.val))
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
    else if (itval != val->end() && !itval->second->packed && itval->second->type == TplType::typename_ &&
             itval->second->byClass.dflt && itval->second->byClass.val &&
             itval->second->byClass.dflt->type == BasicType::templateparam_)
    {
        if (!params->front().second->byClass.val && params->front().second->type == TplType::typename_)
            params->front().second->byClass.val = itval->second->byClass.val;
    }
    else if (itval != val->end() && !itval->second->packed && itval->second->type == TplType::int_ &&
             itval->second->byNonType.dflt && itval->second->byNonType.val &&
             itval->second->byNonType.dflt->type == ExpressionNode::templateparam_)
    {
        if (!params->front().second->byNonType.val && params->front().second->type == TplType::int_)
            params->front().second->byNonType.val = itval->second->byNonType.val;
    }
    else if (itval != val->end() && !itval->second->packed && itval->second->type == TplType::typename_ &&
             itval->second->byClass.dflt && itval->second->byClass.val && isstructured(itval->second->byClass.dflt) &&
             isstructured(itval->second->byClass.val))
    {
        std::list<TEMPLATEPARAMPAIR>* tpdflt = basetype(itval->second->byClass.dflt)->sp->templateParams;
        std::list<TEMPLATEPARAMPAIR>* tpval = basetype(itval->second->byClass.val)->sp->templateParams;
        if (tpdflt && tpval)
        {
            auto ittpdflt = tpdflt->begin();
            auto ittpval = tpval->begin();
            for (; ittpdflt != tpdflt->end() && ittpval != tpval->end(); ++ittpdflt, ++ittpval)
            {
                std::list<TEMPLATEPARAMPAIR> dflt{*ittpdflt}, val{*ittpval};
                TransferClassTemplates(&dflt, &val, params);
            }
        }
    }
    else if (itval != val->end() && !itval->second->packed && itval->second->type == TplType::typename_ &&
             itval->second->byClass.dflt && itval->second->byClass.val && isfunction(itval->second->byClass.dflt) &&
             isfunction(itval->second->byClass.val))
    {
        TYPE* tpd = basetype(itval->second->byClass.dflt)->btp;
        TYPE* tpv = basetype(itval->second->byClass.val)->btp;
        if (tpd->type == BasicType::templateparam_ && tpd->templateParam->first)
        {
            if (params)
            {
                for (auto&& find : *params)
                {
                    if (find.first && !strcmp(find.first->name, tpd->templateParam->first->name))
                    {
                        if (tpd->templateParam->second->packed && !find.second->byPack.pack)
                        {
                            if (tpv->type == BasicType::templateparam_)
                            {
                                find.second->byPack.pack = tpv->templateParam->second->byPack.pack;
                            }
                            else
                            {
                                find.second->byPack.pack = templateParamPairListFactory.CreateList();
                                find.second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                find.second->byPack.pack->back().second->type = TplType::typename_;
                                find.second->byPack.pack->back().second->byClass.val = tpv;
                            }
                        }
                        if (!find.second->byClass.val)
                            find.second->byClass.val =
                                tpv->type == BasicType::templateparam_ ? tpv->templateParam->second->byClass.val : tpv;
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
            if (tpd->type == BasicType::templateparam_ && tpd->templateParam->first)
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
                                    if (tpv->type == BasicType::templateparam_)
                                    {
                                        find.second->byPack.pack = tpv->templateParam->second->byPack.pack;
                                        ++hrd;
                                    }
                                    else
                                    {
                                        find.second->byPack.pack = templateParamPairListFactory.CreateList();
                                        find.second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                        find.second->byPack.pack->back().second->type = TplType::typename_;
                                        find.second->byPack.pack->back().second->byClass.val = tpv;
                                    }
                                }
                                else
                                {
                                    auto next = find.second->byPack.pack;
                                    next->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                    next->back().second->type = TplType::typename_;
                                    next->back().second->byClass.val = tpv;
                                }
                            }
                            else
                            {
                                ++hrd;
                            }
                            if (!find.second->byClass.val)
                                find.second->byClass.val =
                                    tpv->type == BasicType::templateparam_ ? tpv->templateParam->second->byClass.val : tpv;
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
            if (params->front().second->type == TplType::int_)
            {
                for (auto param1 : *dflt)
                {
                    if (param1.second->type == TplType::int_ && params->front().second->type == TplType::int_ &&
                        param1.second->byNonType.dflt && param1.second->byNonType.dflt->type == ExpressionNode::templateparam_)
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
        if (params && params->front().second->type == TplType::typename_ && !params->front().second->packed &&
            params->front().second->byClass.dflt &&
            basetype(params->front().second->byClass.dflt)->type == BasicType::templateselector_ &&
            (*basetype(params->front().second->byClass.dflt)->sp->sb->templateSelector)[1].isTemplate)
        {
            std::list<TEMPLATEPARAMPAIR>* param1 =
                (*basetype(params->front().second->byClass.dflt)->sp->sb->templateSelector)[1].templateParams;
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
        std::list<TEMPLATEPARAMPAIR>*params = spsyms ? spsyms : nparams, *origParams = params;
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
                if (a.second->type == TplType::template_ && a.second->byTemplate.dflt && a.second->byTemplate.dflt->sb)
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
        if (itParams->second->type == TplType::new_)
            ++itParams;
        if (max && itmax->second->type == TplType::new_)
            ++itmax;
        if (itPrimary->second->type == TplType::new_)
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
                            if (itParams->second->type == TplType::int_)
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
                    if (itInitial->second->type == TplType::typename_ && itParams->second->type == TplType::template_)
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
                    else if (itInitial->second->type == TplType::template_ && itParams->second->type == TplType::typename_)
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
                    if (itInitial->second->type == TplType::template_)
                    {
                        if (dflt && !exactMatchOnTemplateParams(((SYMBOL*)dflt)->templateParams, itParams->second->byTemplate.args))
                            rv = nullptr;
                    }
                    if (itParams->second->byClass.val)
                    {
                        switch (itInitial->second->type)
                        {
                            case TplType::typename_:
                                if (!templatecomparetypes(itParams->second->byClass.val, (TYPE*)dflt, true) ||
                                    (isstructured(itParams->second->byClass.val) &&
                                     basetype(itParams->second->byClass.val)->sp->sb->templateLevel &&
                                     !sameTemplate(itParams->second->byClass.val, (TYPE*)dflt, true)))
                                    rv = nullptr;
                                break;
                            case TplType::int_: {
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
                    if (!max && itParams->second->byClass.dflt &&
                        itParams->second->byClass.dflt->type == BasicType::templateselector_)
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
                            if (itParams->second->type == TplType::typename_)
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
                            else if (itParams->second->type == TplType::template_)
                            {
                                if (itParams->second->byClass.dflt->type == BasicType::templateparam_)
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
                            else if (itParams->second->type == TplType::int_)
                            {
                                EXPRESSION* exp = itParams->second->byNonType.val;
                                if (exp && !isintconst(exp))
                                {
                                    exp = copy_expression(exp);
                                    optimize_for_constants(&exp);
                                }
                                if (exp && itParams->second->byNonType.dflt &&
                                    itParams->second->byNonType.dflt->type != ExpressionNode::templateparam_ &&
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
        if (itInitial != iteInitial && itInitial->second->packed &&
            (!itInitial->second->byPack.pack || !itInitial->second->byPack.pack->size()))
            itInitial = iteInitial;
        if (itInitial != iteInitial && (max && itmax != max->end() || !spsyms))
            rv = nullptr;
        if (spsyms)
        {
            for (auto its = itParams; its != params->end(); ++its)
            {
                auto&& primary = *its;
                if (primary.second->type == TplType::typename_)
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
                for (; itParams != iteParams;)
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
                            case TplType::typename_:
                                if (itParams->second->byClass.dflt->type != BasicType::templateparam_ &&
                                    itParams->second->byClass.dflt->type != BasicType::templateselector_ &&
                                    itParams->second->byClass.dflt->type != BasicType::templatedecltype_ &&
                                    !templatecomparetypes(itParams->second->byClass.val, itParams->second->byClass.dflt, true))
                                    rv = nullptr;
                                break;
                            case TplType::int_: {
                                EXPRESSION* exp = copy_expression(itParams->second->byNonType.val);
                                optimize_for_constants(&exp);
                                if (itParams->second->byNonType.dflt &&
                                    !equalTemplateIntNode(exp, itParams->second->byNonType.dflt))
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
            if (itPrimary->second->type == TplType::new_)
                ++itPrimary;
            for (; itParams != iteParams && itPrimary != primary->end(); ++itParams, ++itPrimary)
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
            if (itParams->second->type == TplType::new_)
                ++itParams;
            auto itArgs = args->begin();
            for (; itParams != params->end() && itArgs != args->end(); ++itParams, ++itArgs)
            {
                if (itParams->second->packed)
                    packed = true;
                switch (itParams->second->type)
                {
                    case TplType::typename_:
                        if (itParams->second->byClass.dflt && !itParams->second->packed &&
                            itParams->second->byClass.dflt->type != BasicType::templateparam_ &&
                            (!itArgs->second->byClass.dflt ||
                             !templatecomparetypes(itParams->second->byClass.dflt, itArgs->second->byClass.dflt, true)))
                            rv = nullptr;
                        break;
                    case TplType::int_: {
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
            if (itParams->second->type == TplType::new_)
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
            if (basetype(tp)->sp->sb->instantiated && basetype(tp)->sp->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
                return true;
            if (basetype(tp)->sp->sb->templateLevel)
            {
                return allTemplateArgsSpecified(basetype(tp)->sp, basetype(tp)->sp->templateParams, checkDeduced, checkDeclaring);
            }
            if (checkDeclaring && basetype(tp)->sp->sb->declaringRecursive)
                return false;
        }
    }
    else if (basetype(tp)->type == BasicType::templateparam_)
    {
        if (!basetype(tp)->templateParam->second->byClass.val)
            return false;
        if (basetype(tp)->templateParam->second->byClass.val->type == BasicType::templateparam_)
            return false;
    }
    else if (basetype(tp)->type == BasicType::templatedecltype_)
        return false;
    else if (basetype(tp)->type == BasicType::templateselector_)
    {
        return false;
    }
    else if (basetype(tp)->type == BasicType::memberptr_)
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
        case TplType::int_:
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
                            case ExpressionNode::pc_:
                            case ExpressionNode::global_:
                            case ExpressionNode::func_:
                            case ExpressionNode::void_:
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
        case TplType::template_: {
            return true;
        }
        case TplType::typename_: {
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
        if (it->second->type != TplType::new_)
        {
            if (it->second->packed)
            {
                if ((templateNestingCount && !instantiatingTemplate &&
                     (!it->second->byPack.pack || !it->second->byPack.pack->size())) ||
                    !allTemplateArgsSpecified(sym, it->second->byPack.pack, checkDeduced, checkDeclaring))
                    return false;
            }
            else
            {
                if (sym)
                {
                    if (it->second->type == TplType::typename_)
                    {
                        TYPE* tp = it->second->byClass.val;
                        if (tp && basetype(tp)->type == BasicType::any_)
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
            if (current.second->type == TplType::template_)
            {
                if (current.second->byTemplate.val && base->front().second->bySpecialization.types)
                {
                    for (auto&& tpx : *base->front().second->bySpecialization.types)
                    {
                        if (tpx.second->type == TplType::template_ && tpx.second->byTemplate.dflt &&
                            !strcmp(tpx.second->byTemplate.dflt->name, current.second->byTemplate.dflt->name))
                        {
                            TemplateArgsTemplateAdd(&current, &tpx, base);
                            break;
                        }
                    }
                }
            }
            else if (current.second->type == TplType::typename_)
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
                    else if (current.second->byClass.val->type == BasicType::templateselector_)
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
            if (p.second->type != TplType::new_ && !p.second->byClass.val)
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
            if (param.second->type == TplType::typename_)
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
                    if (tpx1.second->type == TplType::typename_ && tpx1.second->byClass.dflt)
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
            if (tpl.second->type == TplType::typename_)
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
                        if (tp->type != BasicType::typedef_)
                            count++;
                        tp = tp->btp;
                    }
                    if (tp && isstructured(tp))
                        count += 1 + SpecializationComplexity(tp->sp->templateParams);
                }
            }
            else if (tpl.second->type == TplType::int_)
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
                        if (exp->type == ExpressionNode::templateselector_)
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
    if (args && args->front().second->type == TplType::new_)
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
                if (dflts.second->type == TplType::int_ && dflts.second->byNonType.val)
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
                    if (dflts.second->type == TplType::int_ && dflts.second->byNonType.val)
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
            SetLinkerNames(found1, Linkage::cdecl_);

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
            *found1->templateParams->back().second = *sym->templateParams->front().second;  // the TplType::new_ entry
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
            *tpi = TemplateClassInstantiate(sym, args, false, StorageClass::global_)->tp;
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
                    *tpi = TemplateClassInstantiate(sym, args, false, StorageClass::global_)->tp;
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

            for (auto&& dflts : *found1->templateParams)
            {
                if (dflts.second->type == TplType::int_ && dflts.second->byNonType.val)
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
                    if (dflts.second->type == TplType::int_ && dflts.second->byNonType.val)
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
            SetLinkerNames(found1, Linkage::cdecl_);
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
            found1->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
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

TEMPLATEPARAMPAIR* TypeAliasSearch(const char* name, bool toponly)
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
bool ReplaceIntAliasParams(EXPRESSION** exp, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                           std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    bool rv = false;
    if ((*exp)->left)
        rv |= ReplaceIntAliasParams(&(*exp)->left, sym, origTemplate, origUsing);
    if ((*exp)->right)
        rv |= ReplaceIntAliasParams(&(*exp)->right, sym, origTemplate, origUsing);
    if ((*exp)->type == ExpressionNode::templateparam_)
    {
        const char* name = (*exp)->v.sp->name;
        TEMPLATEPARAMPAIR* found = TypeAliasSearch(name, false);
        if (found && found->second->byNonType.dflt)
        {
            *exp = found->second->byNonType.dflt;
        }
        rv = true;
    }
    else if ((*exp)->type == ExpressionNode::sizeofellipse_)
    {
        const char* name = (*exp)->v.templateParam->first->name;
        TEMPLATEPARAMPAIR* found = TypeAliasSearch(name, false);
        if (found)
        {
            (*exp)->v.templateParam->second = found->second;
        }
        rv = true;
    }
    else if ((*exp)->type == ExpressionNode::templateselector_)
    {
        SpecifyTemplateSelector(&(*exp)->v.templateSelector, (*exp)->v.templateSelector, true, sym, origTemplate, origUsing);
    }
    return rv;
}
static EXPRESSION* SpecifyArgInt(SYMBOL* sym, EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* orig,
                                 std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static TYPE* SpecifyArgType(SYMBOL* sym, TYPE* tp, TEMPLATEPARAM* tpt, std::list<TEMPLATEPARAMPAIR>* orig,
                            std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static void SpecifyOneArg(SYMBOL* sym, TEMPLATEPARAMPAIR* temp, std::list<TEMPLATEPARAMPAIR>* origTemplate,
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
                        case TplType::int_:

                            tpx.second->byNonType.dflt =
                                SpecifyArgInt(sym, tpx.second->byNonType.dflt, nullptr, origTemplate, origUsing);
                            optimize_for_constants(&tpx.second->byNonType.dflt);
                            break;
                        case TplType::typename_:
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
    while (find && find->type != BasicType::templateparam_)
        find = find->btp;
    if (find)
    {
        if (!find->templateParam->second->packed && find->templateParam->first)
        {
            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(find->templateParam->first->name, false);
            if (rv && rv->second->byClass.dflt)
            {
                TYPE** last = &find;
                while (in && in->type != BasicType::templateparam_)
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
                    if (oldItem.sp->tp->type == BasicType::templateparam_)
                    {
                        TEMPLATEPARAMPAIR* rv = TypeAliasSearch(oldItem.sp->name, false);
                        if (rv && rv->second->type == TplType::typename_)
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
                    std::list<TEMPLATEPARAMPAIR>*tpx = nullptr, *x;
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
                    for (; ittp != itetp;)
                    {
                        if (ittp->second->packed && ittp->second->byPack.pack)
                        {
                            stk.push(ittp);
                            stk.push(itetp);
                            itetp = ittp->second->byPack.pack->end();
                            ittp = ittp->second->byPack.pack->begin();
                        }
                        x->push_back(TEMPLATEPARAMPAIR{ittp->first, ittp->second});
                        if (ittp->second->type != TplType::new_)
                        {
                            bool replaced = false;
                            x->back().second = Allocate<TEMPLATEPARAM>();
                            *x->back().second = *ittp->second;
                            if (!expression && ittp->second->type == TplType::int_ && ittp->second->byNonType.dflt)
                            {
                                x->back().second->byNonType.dflt = copy_expression(x->back().second->byNonType.dflt);
                                replaced = ReplaceIntAliasParams(&x->back().second->byNonType.dflt, sym, origTemplate, origUsing);
                                if (replaced)
                                    optimize_for_constants(&x->back().second->byNonType.dflt);
                            }
                            if (!replaced && ittp->first &&
                                (expression || (ittp->second->type == TplType::int_ || !ittp->second->byClass.dflt)))
                            {
                                const char* name = ittp->first->name;
                                if (!expression && ittp->second->type == TplType::int_ && ittp->second->byNonType.dflt &&
                                    ittp->second->byNonType.dflt->type == ExpressionNode::templateparam_)
                                {
                                    name = ittp->second->byNonType.dflt->v.sp->name;
                                    SearchAlias(name, &x->back(), sym, origTemplate, origUsing);
                                }
                                else if (expression && ittp->second->type == TplType::int_ && x->back().second->byNonType.dflt)
                                {
                                    if (!IsConstantExpression(x->back().second->byNonType.dflt, false, false))
                                        SearchAlias(name, &x->back(), sym, origTemplate, origUsing);
                                }
                                else if (x->back().second->type == TplType::typename_ && x->back().second->byClass.dflt)
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
                            if (x->back().second->type == TplType::typename_)
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
        else if (exp->type == ExpressionNode::templateparam_ || exp->type == ExpressionNode::auto_ && exp->v.sp->packed)
        {
            TEMPLATEPARAMPAIR* rv;
            if (exp->type == ExpressionNode::templateparam_)
                rv = TypeAliasSearch(exp->v.sp->tp->templateParam->first->name, false);
            else
                rv = TypeAliasSearch(exp->v.sp->name, false);
            if (rv)
            {
                if (rv->second->type == TplType::int_)
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
                                exp = exp1;  // dal fixme
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
                                exp = anonymousVar(StorageClass::auto_, dflt);
                            }
                            else
                            {
                                exp = anonymousVar(StorageClass::auto_, dflt);
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
        else if (exp->type == ExpressionNode::templateselector_)
        {
            EXPRESSION* exp1 = Allocate<EXPRESSION>();
            *exp1 = *exp;
            exp = exp1;
            SpecifyTemplateSelector(&exp->v.templateSelector, exp->v.templateSelector, true, sym, origTemplate, origUsing);
            optimize_for_constants(&exp);
        }
        else if (exp->type == ExpressionNode::auto_)
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
        else if (exp->type == ExpressionNode::func_ || exp->type == ExpressionNode::funcret_)
        {
            EXPRESSION *exp1 = nullptr, **last = &exp1;
            ;
            while (exp->type == ExpressionNode::funcret_)
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
                if ((*last)->v.func->sp->tp->type != BasicType::aggregate_)
                {
                    (*last)->v.func->sp = (*last)->v.func->sp->sb->overloadName;
                    (*last)->v.func->functp = (*last)->v.func->sp->tp;
                }
                auto x1 = (*last)->v.func->templateParams = templateParamPairListFactory.CreateList();
                for (auto tpx : *tpxx)
                {
                    x1->push_back(TEMPLATEPARAMPAIR{tpx.first, Allocate<TEMPLATEPARAM>()});
                    *x1->back().second = *tpx.second;
                    if (x1->back().second->type == TplType::int_ || x1->back().second->type == TplType::typename_ ||
                        x1->back().second->type == TplType::template_)
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
        else if (exp->type == ExpressionNode::construct_)
        {
            EXPRESSION* exp1 = Allocate<EXPRESSION>();
            *exp1 = *exp;
            exp = exp1;
            exp->v.construct.tp = SpecifyArgType(sym, exp->v.construct.tp, nullptr, orig, origTemplate, origUsing);
            optimize_for_constants(&exp);
        }
        else if (exp->type == ExpressionNode::sizeofellipse_)
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
    if (tp->type == BasicType::typedef_ && tp->sp->templateParams)
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
                if (tpr->back().second->type != TplType::new_ && !tpr->back().second->byClass.dflt &&
                    !tpr->back().second->byClass.val)
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
    else if (basetype(tp)->type == BasicType::templateparam_)
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
                    if (tpl.second->type != TplType::new_)
                    {
                        args1->push_back(TEMPLATEPARAMPAIR{tpl.first, Allocate<TEMPLATEPARAM>()});
                        *args1->back().second = *tpl.second;
                        if (args1->back().second->type == TplType::int_ || args1->back().second->type == TplType::typename_)
                        {
                            if (args1->back().second->packed &&
                                (!args1->back().second->byPack.pack || !args1->back().second->byPack.pack->size()))
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
    else if (tp->type == BasicType::templatedecltype_)
    {
        static int nested;
        if (nested >= 10)
            return rv;
        nested++;
        tp->templateDeclType = SpecifyArgInt(sym, tp->templateDeclType, orig, origTemplate, origUsing);
        nested--;
    }
    else if (basetype(tp)->type == BasicType::templateselector_)
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
                    rvs->back().sp = makeID(StorageClass::auto_, tp1, nullptr, AnonymousName());
                }
                else
                {
                    if (first && old.sp)
                    {
                        first = false;
                        if (old.sp->tp->type == BasicType::templateparam_)
                        {
                            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(old.sp->name, false);
                            if (rv && rv->second->type == TplType::typename_)
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
    if (temp && temp->second->ellipsis && temp->second->type != TplType::template_)
    {
        if (temp->second->type == TplType::typename_)
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
            case TplType::int_: {
                std::list<TEMPLATEPARAMPAIR> a{*tpx};
                // the checked for pack here wasn't done before, it relied on dflt being null
                // should be looked at more closely...
                auto rv = SpecifyArgInt(sym,
                                        tpx->second->byNonType.dflt && (!tpx->second->packed || !tpx->second->byPack.pack)
                                            ? tpx->second->byNonType.dflt
                                            : tpx->second->byNonType.val,
                                        &a, origTemplate, origUsing);
                if (rv)
                {
                    optimize_for_constants(&rv);
                    while (rv->type == ExpressionNode::void_ && rv->right)
                        rv = rv->right;
                }
                if (i >= 0)
                    hold[i] = rv;
                else
                    tpx->second->byNonType.dflt = rv;
                break;
            }
            case TplType::template_: {
                break;
            }
            case TplType::typename_: {
                std::list<TEMPLATEPARAMPAIR> a{*tpx};
                auto rv = SpecifyArgType(sym, tpx->second->byClass.dflt ? tpx->second->byClass.dflt : tpx->second->byClass.val,
                                         tpx->second, &a, origTemplate, origUsing);
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
                if (temp->second->type == TplType::template_)
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
            for (; ita != args->end() && ittp != origTemplate->end(); ++ita, ++ittp)
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
                        ittp->second->byClass.val = ita->second->byClass.val ? ita->second->byClass.val : ita->second->byClass.dflt;
                }
            }
            for (; ittp != origTemplate->end(); ++ittp)
            {
                args->push_back(TEMPLATEPARAMPAIR{ittp->first, Allocate<TEMPLATEPARAM>()});
                *args->back().second = *ittp->second;
                if (ittp->second->byClass.txtdflt)
                    ittp->second->byClass.val = nullptr;
            }
            if (!templateNestingCount &&
                !TemplateParseDefaultArgs(sp, nullptr, sp->templateParams, sp->templateParams, sp->templateParams))
            {
                return false;
            }
            ittp = origTemplate->begin();
            ++ittp;
            ita = args->begin();
            for (; ittp != origTemplate->end(); ++ita, ++ittp)
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
                            for (; itpl != tpl1->end(); ++itpl, ++itplp)
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
std::list<TEMPLATEPARAMPAIR>* GetTypeAliasArgs(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args,
                                               std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing)
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
                if (test.second->type != TplType::new_)
                {
                    if (test.first && !strcmp(test.first->name, args1->back().first->name))
                    {
                        if (itargs2 == args->end())
                        {
                            if (!test.second->packed)
                                Utils::Fatal("internal error");
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
                            args1->back().second->byClass.dflt =
                                itargs2->second->byClass.dflt ? itargs2->second->byClass.dflt : itargs2->second->byClass.val;
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
        if (itt->second->type == TplType::new_)
            ++itt;
        for (; itt != tpx->end() && ita != args->end(); ++itt, ++ita)
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
    if (itt->second->type == TplType::new_)
        ++itt;
    for (; itt != tpx->end() && n; ++itt, ++ita, n--)
    {
        ita->first = itt->first;
    }
    for (; itt != tpx->end(); ++itt)
    {
        if (itt->second->type != TplType::new_)
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
    else if (args->front().second->type == TplType::new_)
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
    if (basetp->type == BasicType::typedef_)
    {
        if (basetp->btp->type == BasicType::templateparam_)
            basetp = basetp->btp;
    }
    if (basetp->type == BasicType::templatedecltype_)
    {
        rv = CopySymbol(sp);
        rv->sb->mainsym = sp;
        if (!templateNestingCount)
        {
            basetp = SpecifyArgType(basetp->sp, basetp, nullptr, nullptr, args, sp->sb->typeAlias);
            rv->tp = TemplateLookupTypeFromDeclType(basetp);
            if (!rv->tp)
                rv->tp = &stdany;
        }
    }
    else if (basetp->type == BasicType::templateselector_)
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
            MakeType(tp1, BasicType::templateselector_);
            tp1.sp = rv;
            rv->tp = SynthesizeType(&tp1, args, false);
            if (isstructured(rv->tp))
                rv = basetype(rv->tp)->sp;
            basetp = sp->tp->btp;
            if (ispointer(basetp))
            {
                rv = makeID(StorageClass::type_, rv->tp, nullptr, AnonymousName());
                TYPE *tpr = nullptr, **last = &tpr;
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
            TYPE* tp1 = MakeType(BasicType::templateselector_);
            tp1->sp = rv;
            rv->tp = tp1;
        }
        if (checked)
            args->push_front(old);
        return rv;
    }
    else if (basetp->type == BasicType::typedef_ || (isstructured(basetp) && basetype(basetp)->sp->sb->templateLevel))
    {
        std::list<TEMPLATEPARAMPAIR>* newParams = GetTypeAliasArgs(sp, args, sp->templateParams, sp->sb->typeAlias);
        if (basetp->type == BasicType::typedef_)
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
        if ((*tp)->type == BasicType::templateparam_)
        {
            auto itarg = args->begin();
            auto itorig = sp->templateParams->begin();
            ++itorig;
            for (; itarg != args->end() && itorig != sp->templateParams->end(); ++itarg, ++itorig)
            {
                if (itarg->second->type == TplType::typename_ &&
                    strcmp((*tp)->templateParam->first->name, itorig->first->name) == 0)
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
        TYPE *tpr = nullptr, **last = &tpr;
        rv = makeID(StorageClass::type_, rv->tp, rv, AnonymousName());
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

}  // namespace Parser