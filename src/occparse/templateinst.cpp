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
#include "lex.h"
#include "help.h"
#include "mangle.h"
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
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "overload.h"
#include "class.h"
#include "expr.h"
#include "exprpacked.h"
#include "sha1.h"
#include "templatehash.h"
#include "SymbolProperties.h"
#include "using.h"

namespace Parser
{
int templateDeclarationLevel;
SYMBOL* defaultParsingContext;

Type* SolidifyType(Type* tp)
{
    for (auto v = tp; v; v = v->btp)
    {
        if (v->IsStructured())
        {
            auto rv = tp->CopyType(true);
            rv->UpdateRootTypes();
            for (auto v = rv; v; v = v->btp)
                if (v->IsStructured())
                {
                    SYMBOL* old = v->BaseType()->sp;
                    v->BaseType()->sp = CopySymbol(old);
                    SYMBOL* sym = v->BaseType()->sp;
                    if (!sym->sb->mainsym)
                        sym->sb->mainsym = old;
                    sym->tp = sym->tp->CopyType();
                    sym->tp->sp = sym;
                    sym->templateParams = SolidifyTemplateParams(copyParams(sym->templateParams, true));
                    sym->tp->UpdateRootTypes();
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

std::list<TEMPLATEPARAMPAIR>* copyParams(std::list<TEMPLATEPARAMPAIR>* t, bool alsoSpecializations, bool unique)
{
    if (t)
    {
        std::list<TEMPLATEPARAMPAIR>* rv = templateParamPairListFactory.CreateList();
        for (auto&& parse : *t)
        {
            rv->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            *rv->back().second = *parse.second;
            if (unique && rv->back().second->type == TplType::typename_)
            {
                rv->back().second->byClass.val = rv->back().second->byClass.val->CopyType(true);
            }
            auto sp = rv->back().first = parse.first ? CopySymbol(parse.first) : nullptr;
            if (sp)
            {
                sp->tp = Type::MakeType(BasicType::templateparam_);
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
                if (unique && last->back().second->type == TplType::typename_)
                {
                    last->back().second->byClass.val = last->back().second->byClass.val->CopyType(true);
                }
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
                                it1->second->byNonType.tp = it1->second->byNonType.tp->CopyType();
                                it1->second->byNonType.tp->UpdateRootTypes();
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

void SynthesizeQuals(Type*** last, Type** qual, Type*** lastQual)
{
    if (*qual)
    {
        Type* p = **last;
        Type* v = *qual;
        int sz = (**last) ? (**last)->BaseType()->size : 0;
        while (p && p->type == BasicType::derivedfromtemplate_)
        {
            **last = p->CopyType();
            *last = &(**last)->btp;
            p = p->btp;
        }
        if ((*qual)->rootType && (*qual)->IsRef())
        {
            while (p && p != p->BaseType())
            {
                **last = p->CopyType();
                *last = &(**last)->btp;
                p = p->btp;
            }
        }
        while (v)
        {
            **last = v->CopyType();
            if (!(**last)->rootType || !(**last)->IsRef())
                (**last)->size = sz;
            *last = &(**last)->btp;
            v = v->btp;
        }
        **last = nullptr;
        if (p)
        {
            **last = p->CopyType(true);
        }
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
                            if (tpx.second->byClass.dflt->IsRef())
                            {
                                Type* cursor = tpx.second->byClass.dflt;
                                Type *newType = nullptr, **tp1 = &newType;
                                while (1)
                                {
                                    *tp1 = cursor->CopyType();
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
                                tpx.second->byClass.dflt = Type::MakeType(find.second->lref ? BasicType::lref_ : BasicType::rref_,
                                                                          tpx.second->byClass.dflt);
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
            if ( !search.second->byClass.dflt)
                return nullptr;
            if (!*pt)
                *pt = templateParamPairListFactory.CreateList();
            (*pt)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            *(*pt)->back().second = *search.second;
            if (search.second->packed)
            {
                auto tpl = (*pt)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                if (search.second->byPack.pack && search.second->byPack.pack->size())
                {
                    for (auto& tpx : *search.second->byPack.pack)
                    {
                        tpl->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
                        *tpl->back().second = *tpx.second;
                        if ((!IsDefiningTemplate()) && tpl->back().second->byClass.dflt)
                            tpl->back().second->byClass.dflt = SynthesizeType(tpl->back().second->byClass.dflt, enclosing, false);
                    }
                }
            }
            else
            {
                *(*pt)->back().second = *search.second;
                if (!IsDefiningTemplate())
                    (*pt)->back().second->byClass.dflt = SynthesizeType((*pt)->back().second->byClass.dflt, enclosing, false);
            }
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
                for (auto&& s : enclosingDeclarations)
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
static Type* SynthesizeStructure(Type* tp_in, std::list<TEMPLATEPARAMPAIR>* enclosing)
{
    Type* tp = tp_in->BaseType();
    if (tp->IsRef())
        tp = tp->btp->BaseType();
    if (tp->IsStructured())
    {
        SYMBOL* sp = tp->BaseType()->sp;
        if (sp->sb->templateLevel && !sp->sb->instantiated && sp->templateParams->size() > 1)
        {
            if (!allTemplateArgsSpecified(sp, sp->templateParams))
            {
                if (!templateDefinitionLevel && sp->templateParams)
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
                                else if (search.second->byClass.dflt && ! search.second->packed &&
                                         (search.second->byClass.dflt ->type == BasicType::memberptr_ || search.second->byClass.dflt->IsStructured()))
                                {
                                    if (!pt)
                                        pt = templateParamPairListFactory.CreateList();
                                    pt->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                    *pt->back().second = *search.second;
                                    {
                                        pt->back().second->byClass.dflt = SynthesizeType(search.second->byClass.dflt, enclosing, false);
                                    }
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
                {
                    sp->tp = sp->tp->InitializeDeferred();

                    for (auto&& search : *sp->templateParams)
                    {
                        if (search.second->byClass.dflt && search.second->byClass.dflt->type == BasicType::any_)
                            return &stdany;
                    }
                }
            }
            else
            {
                std::list<TEMPLATEPARAMPAIR>* params = paramsToDefault(sp->templateParams);
                SYMBOL* sp1 = CopySymbol(sp);
                sp1->tp = sp->tp->CopyType();
                sp1->tp->UpdateRootTypes();
                sp1->tp->sp = sp1;
                sp = sp1;
                sp = GetClassTemplate(sp, params, false);
            }

            if (sp)
            {
                auto tp1 = sp->tp->CopyType();
                if (tp_in->IsConst())
                {
                    tp1 = Type::MakeType(BasicType::const_, tp1);
                }
                if (tp_in->IsVolatile())
                {
                    tp1 = Type::MakeType(BasicType::volatile_, tp1);
                }
                if (tp_in->IsRef())
                {
                    tp1 = Type::MakeType(tp_in->BaseType()->type, tp1);
                }
                return tp1;
            }
        }
    }
    return nullptr;
}
Type* SynthesizeType(Type* tp, std::list<TEMPLATEPARAMPAIR>* enclosing, bool alt, bool arg)
{
    auto oldnoExcept = noExcept;
    Type *rv = &stdany, **last = &rv;
    Type *qual = nullptr, **lastQual = &qual;
    Type* tp_in = tp;
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
                else if (arg && tp->sp->templateParams && (!IsDefiningTemplate()))
                {
                    std::list<Type*> lst;
                    for (auto&& tpl : *tp->sp->templateParams)
                    {
                        if (tpl.second->packed)
                        {
                            if (tpl.second->byPack.pack)
                                for (auto&& t : *tpl.second->byPack.pack)
                                {
                                    lst.push_back(t.second->byClass.dflt);
                                    t.second->byClass.dflt = t.second->byClass.val;
                                }
                        }
                        else
                        {
                            lst.push_back(tpl.second->byClass.dflt);
                            tpl.second->byClass.dflt = tpl.second->byClass.val;
                        }
                    }
                    auto sp1 = ParseLibcxxAliases(tp->sp, tp->sp->templateParams);
                    for (auto&& tpl : *tp->sp->templateParams)
                    {
                        if (tpl.second->packed)
                        {
                            if (tpl.second->byPack.pack)
                                for (auto&& t : *tpl.second->byPack.pack)
                                {
                                    t.second->byClass.dflt = lst.front();
                                    lst.pop_front();
                                }
                        }
                        else
                        {
                            tpl.second->byClass.dflt = lst.front();
                            lst.pop_front();
                            ;
                        }
                    }
                    if (sp1 && sp1 != tp->sp)
                    {
                        tp = sp1->tp;
                    }
                    else
                    {
                        tp = tp->btp;
                    }
                }
                else
                {
                    tp = tp->btp;
                }
                break;
            case BasicType::pointer_: {
                Type* tp3 = tp->btp;
                tp->btp = nullptr;
                SynthesizeQuals(&last, &qual, &lastQual);
                tp->btp = tp3;
                *last = tp->CopyType();
                if ((*last)->IsArray())
                {
                    if ((*last)->etype)
                        (*last)->etype = SynthesizeType((*last)->etype, enclosing, alt);
                    if ((*last)->esize && !isintconst((*last)->esize))
                    {
                        (*last)->esize = copy_expression((*last)->esize);
                        optimize_for_constants(&(*last)->esize);
                    }
                }
                if (!(*last)->IsFunctionPtr())
                {
                    (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                }
                rv->UpdateRootTypes();
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
                rv->UpdateRootTypes();
                return rv;
                break;
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
                        std::deque<Type*> defaults;
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
                                        if (!args->back().second->byPack.pack)
                                            args->back().second->byPack.pack = templateParamPairListFactory.CreateList();
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
                        {
                            sp = GetTypeAliasSpecialization(ts, args);
                        }
                        else
                        {
                            sp = GetClassTemplate(ts, args, true);
                        }
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
                        if (!tp || tp->type == BasicType::any_ || !tp->IsStructured())
                        {
                            return &stdany;
                        }
                        sp = tp->BaseType()->sp;
                    }
                    else
                    {
                        tp = ts->tp->BaseType();
                        if (tp)
                        {
                            tp->InstantiateDeferred();
                            if (tp->type == BasicType::templateselector_)
                            {
                                tp = SynthesizeType(tp, enclosing, alt);
                                if (tp->type == BasicType::any_)
                                {
                                    return tp;
                                }
                                sp = tp->BaseType()->sp;
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
                                sp = tp->BaseType()->sp;
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
                        if (!tp->IsStructured())
                            break;

                        sp = search(tp->BaseType()->syms, find->name);
                        if (!sp)
                        {
                            sp = classdata(find->name, tp->BaseType()->sp, nullptr, false, false);
                            if (sp == (SYMBOL*)-1)
                                sp = nullptr;
                        }
                        if (0 && sp &&

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
                                Type* ctype = sp->tp;
                                EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                                CallSite funcparams = {};
                                funcparams.arguments = rvsit->arguments;
                                auto oldnoExcept = noExcept;
                                auto sp1 = GetOverloadedFunction(&ctype, &exp, sp, &funcparams, nullptr, false, false, 0);
                                noExcept = oldnoExcept;
                                if (sp1)
                                {
                                    tp = sp1->tp->BaseType()->btp;
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
                        Type* tp1 = tp;
                        while (tp1->IsPtr())
                            tp1 = tp1->BaseType()->btp;
                        tp1 = tp1->BaseType();
                        tp1->InstantiateDeferred();
                        tp1 = tp1->InitializeDeferred();
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
                            rv = Type::MakeType(BasicType::derivedfromtemplate_, rv);
                            SynthesizeQuals(&last, &qual, &lastQual);
                        }
                        rv->UpdateRootTypes();
                        return rv;
                    }
                    return &stdany;
                }
                return &stdany;
            }
            case BasicType::rref_:
                if (qual == nullptr && tp->btp->type == BasicType::templateparam_ && tp->btp->templateParam->second->byClass.val &&
                    tp->btp->templateParam->second->byClass.val->type == BasicType::lref_)
                {
                    Type* tp1 = tp->btp->templateParam->second->byClass.val;
                    tp = tp1->BaseType();
                    break;
                }
                // fallthrough
            case BasicType::lref_:
                SynthesizeQuals(&last, &qual, &lastQual);
                *last = tp->CopyType();
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
                *lastQual = tp->CopyType();
                (*lastQual)->btp = nullptr;
                lastQual = &(*lastQual)->btp;
                tp = tp->btp;
                break;
            case BasicType::memberptr_:
                *last = tp->CopyType();
                {
                    Type* tp1 = tp->sp->tp;
                    if (tp1->type == BasicType::templateparam_)
                    {
                        tp1 = tp1->templateParam->second->byClass.val;
                        (*last)->sp = tp1->sp;
                    }
                }
                (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                rv->UpdateRootTypes();
                return rv;

            case BasicType::ifunc_:
            case BasicType::func_: {
                Type* func;
                *last = tp->CopyType();
                (*last)->syms = symbols->CreateSymbolTable();
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
                                Type **last1, *qual1, *temp, **lastQual1 = &temp;
                                Type **btp = nullptr, *next;
                                next = sp->tp;
                                while (next->IsPtr() || next->IsRef())
                                    next = next->BaseType()->btp;
                                next = next->BaseType();
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
                                    Type* tp1;
                                    clone->tp = SynthesizeType(tpx.second->byClass.val, enclosing, alt, true);
                                    if (!first)
                                    {
                                        clone->name = clone->sb->decoratedName = AnonymousName();
                                        clone->packed = false;
                                    }
                                    else
                                    {
                                        clone->synthesized = true;
                                        clone->tp->templateParam = sp->tp->templateParam;
                                    }
                                    Type *current = nullptr, **last = &current;
                                    if (qual1 && btp)
                                    {
                                        for (auto tpx = qual1; tpx != *btp; tpx = tpx->btp)
                                        {
                                            *last = Allocate<Type>();
                                            *(*last) = *tpx;
                                            last = &(*last)->btp;
                                        }
                                    }
                                    *last = clone->tp;
                                    tp1 = Type::MakeType(BasicType::derivedfromtemplate_, current);
                                    tp1->templateParam = clone->tp->templateParam;
                                    clone->tp = tp1;
                                    tp1->UpdateRootTypes();
                                    tpx.second->packsym = clone;
                                    func->syms->Add(clone);
                                    first = false;
                                    sp->tp->templateParam->second->index++;
                                }
                            }
                            else
                            {
                                SYMBOL* clone = CopySymbol(sp);
                                clone->tp = SynthesizeType(&stdany, enclosing, alt, true);
                                clone->tp->templateParam = sp->tp->templateParam;
                                func->syms->Add(clone);
                            }
                        }
                    }
                    else
                    {
                        SYMBOL* clone = CopySymbol(sp);
                        Type* tp1;
                        func->syms->Add(clone);
                        auto xx = clone->tp;
                        clone->tp = SynthesizeType(clone->tp, enclosing, alt, true);
                        if (clone->tp->type != BasicType::void_ && clone->tp->type != BasicType::any_)
                        {
                            clone->tp = Type::MakeType(BasicType::derivedfromtemplate_, clone->tp);
                            clone->tp->UpdateRootTypes();
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
                        rv->UpdateRootTypes();
                        return rv;
                    }
                    auto it = tpa->second->byPack.pack->begin();
                    for (int i = 0; i < tpa->second->index; i++)
                        ++it;
                    tpa = &*it;
                }
                if (tpa->second->type == TplType::typename_)
                {
                    Type *type = alt ? tpa->second->byClass.temp : tpa->second->byClass.val, *typx = type;
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
                        *last = type->CopyType();
                        (*last)->templateTop = true;
                        tp = Type::MakeType(BasicType::derivedfromtemplate_, rv);
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
                    else
                    {
                        if (!templateDefinitionLevel && tpa->first)
                        {
                            bool found = false;
                            for (auto&& p : enclosingDeclarations)
                            {
                                if (p.tmpl)
                                {
                                    SYMBOL* s = templatesearch(tpa->first->name, p.tmpl);
                                    if (s && s->tp->templateParam->second->byClass.val)
                                    {
                                        found = true;
                                        *last = s->tp->templateParam->second->byClass.val->CopyType();
                                        break;
                                    }
                                }
                            }
                            if (!found)
                            {
                                *last = tp->CopyType();
                            }
                        }
                        else
                        {
                            *last = tp->CopyType();
                        }
                    }
                    rv->UpdateRootTypes();
                    return rv;
                }
                else if (tpa->second->type == TplType::template_)
                {
                    Type* type = alt ? tpa->second->byTemplate.temp->tp : tpa->second->byTemplate.val->tp;
                    if (type)
                    {
                        *last = type->CopyType();
                        (*last)->templateTop = true;
                        rv = Type::MakeType(BasicType::derivedfromtemplate_, rv);
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
                    rv->UpdateRootTypes();
                    return rv;
                }
                else
                {
                    return &stdany;
                }
            }
            case BasicType::templatedeferredtype_:
                *last = tp->CopyType();
                (*last)->templateArgs = templateParamPairListFactory.CreateList();
                for (auto&& tpl : *tp->templateArgs)
                {
                    auto second = tpl.second;
                    if ((tpl.second->type == TplType::typename_ || tpl.second->type == TplType::int_) && tpl.second->byClass.dflt)
                    {
                        second = Allocate<TEMPLATEPARAM>();
                        *second = *tpl.second;
                        if (tpl.second->packed)
                        {
                            if (tpl.second->byPack.pack)
                            {
                                second->byPack.pack = templateParamPairListFactory.CreateList();
                                for (auto&& tpl1 : *tpl.second->byPack.pack)
                                {
                                    auto second2 = Allocate<TEMPLATEPARAM>();
                                    *second2 = *tpl1.second;
                                    if (tpl.second->type == TplType::typename_)
                                    {
                                        second2->byClass.dflt = SynthesizeType(tpl1.second->byClass.dflt, enclosing, alt, arg);
                                    }
                                    else
                                    {
                                        second2->byNonType.dflt = copy_expression(second2->byNonType.dflt);
                                        optimize_for_constants(&second2->byNonType.dflt);
                                    }
                                    second->byPack.pack->push_back({ nullptr, second2 });
                                }
                                continue;
                            }
                        }
                        else
                        {
                            if (tpl.second->type == TplType::typename_)
                            {
                                second->byClass.dflt = SynthesizeType(tpl.second->byClass.dflt, enclosing, alt, arg);
                            }
                            else
                            {
                                second->byNonType.dflt = copy_expression(second->byNonType.dflt);
                                optimize_for_constants(&second->byNonType.dflt);
                            }
                        }
                    }
                    (*last)->templateArgs->push_back({ tpl.first, second });
                }
                *last = Type::MakeType(BasicType::derivedfromtemplate_, *last);
                SynthesizeQuals(&last, &qual, &lastQual);
                rv->UpdateRootTypes();
                return rv;
                break;
            default:
                if (alt && tp->IsStructured())
                {
                    tp_in = tp->CopyType();
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
                    tp_in = SynthesizeStructure(tp, /*tp->BaseType()->sp ? tp->BaseType()->sp->templateParams :*/ enclosing);
                    if (tp_in)
                    {
                        rv = Type::MakeType(BasicType::derivedfromtemplate_, rv);
                        tp = tp_in;
                    }
                }
                *last = tp;
                SynthesizeQuals(&last, &qual, &lastQual);
                rv->UpdateRootTypes();
                return rv;
        }
    }
}
static bool hasPack(Type* tp)
{
    bool rv = false;
    while (tp->IsPtr())
        tp = tp->btp;
    if (tp->IsFunction())
    {
        for (auto sym : *tp->syms)
        {
            if (rv)
                break;
            if (sym->packed)
            {
                rv = true;
            }
            else if (sym->tp->IsFunction() || sym->tp->IsFunctionPtr())
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
    if (templateDefinitionLevel)
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
                        if (!found || !found->tp->IsStructured())
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
    DeclarationScope scope;

    if (sym->sb->parentClass)
    {
        enclosingDeclarations.Add(sym->sb->parentClass);
    }
    enclosingDeclarations.Add(sym->templateParams);
    rsv->sb->parentTemplate = sym;
    rsv->sb->mainsym = sym;
    rsv->sb->parentClass = SynthesizeParentClass(rsv->sb->parentClass);
    rsv->tp = SynthesizeType(sym->tp, params, false);
    if (rsv->tp->IsFunction())
    {
        if (!IsDefiningTemplate())
        {
            rsv->tp->BaseType()->btp->InstantiateDeferred();
            for (auto& sp1 : *rsv->tp->BaseType()->syms)
            {
                bool found = false;
                sp1->tp->InstantiateDeferred();
                if (sp1->tp->IsFunctionPtr() || sp1->tp->IsFunction())
                {
                    auto tp1 = sp1->tp;
                    if (tp1->IsPtr())
                        tp1 = tp1->BaseType()->btp;
                    auto& retval = tp1->BaseType()->btp;
                    if (retval->type == BasicType::templateparam_ && retval->templateParam->second->byClass.val)
                    {
                        found = true;
                    }
                    for (auto a : *tp1->BaseType()->syms)
                    {
                        auto& argval = a->tp;
                        if (argval->type == BasicType::templateparam_)
                        {
                            found = true;
                        }
                    }
                    if (found)
                    {
                        sp1 = CopySymbol(sp1);
                        sp1->tp = sp1->tp->CopyType(true);
                        auto syms = symbols->CreateSymbolTable();
                        tp1 = sp1->tp;
                        if (tp1->IsPtr())
                            tp1 = tp1->BaseType()->btp;
                        for (auto s : *tp1->BaseType()->syms)
                            syms->Add(CopySymbol(s));
                        tp1->BaseType()->syms = syms;
                        auto& retval = tp1->BaseType()->btp;
                        if (retval->type == BasicType::templateparam_ && retval->templateParam->second->byClass.val)
                        {
                            retval = retval->templateParam->second->byClass.val;
                        }
                        for (auto a : *tp1->BaseType()->syms)
                        {
                            auto& argval = a->tp;
                            if (argval->type == BasicType::templateparam_)
                            {
                                if (argval->templateParam->second->packed)
                                {
                                    if (argval->templateParam->second->byPack.pack)
                                    {
                                        bool first = true;
                                        for (auto& tpl : *argval->templateParam->second->byPack.pack)
                                        {
                                            if (tpl.second->byClass.val)
                                            {
                                                if (first)
                                                {
                                                    a->tp = tpl.second->byClass.val;
                                                    first = false;
                                                }
                                                else
                                                {
                                                    auto sym = CopySymbol(a);
                                                    sym->name = AnonymousName();
                                                    sym->packed = false;
                                                    sym->tp = tpl.second->byClass.val;
                                                    tp1->BaseType()->syms->Add(sym);
                                                }
                                            }
                                        }
                                        break;
                                    }
                                    else if (argval->templateParam->second->byClass.val)
                                    {
                                        argval = argval->templateParam->second->byClass.val;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        rsv->tp->BaseType()->sp = rsv;
    }
    rsv->templateParams = params;
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
static bool ValidArg(Type* tp)
{
    while (1)
    {
        switch (tp->type)
        {
            case BasicType::pointer_:
                if (tp->IsArray())
                {
                    while (tp->IsArray())
                    {
                        tp = tp->BaseType()->btp;
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
                    if (tp->type == BasicType::void_ || tp->IsFunction() || tp->IsRef() ||
                        (tp->IsStructured() && tp->BaseType()->sp->sb->isabstract))
                        return false;
                }
                if (tp->IsPtr())
                {
                    while (tp->IsPtr())
                        tp = tp->btp;
                    tp = tp->BaseType();
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
                    if (tp->IsRef())
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
                    Type* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
                    if (!tp1 || tp1->type == BasicType::any_ || !tp1->IsStructured())
                        return false;
                    sp = tp1->BaseType()->sp;
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
                else if (ts->tp->BaseType()->templateParam->second->type == TplType::typename_)
                {
                    tp = ts->tp->BaseType()->templateParam->second->byClass.val;
                    if (!tp)
                        return false;
                    sp = tp->BaseType()->sp;
                }
                else if (ts->tp->BaseType()->templateParam->second->type == TplType::delete_)
                {
                    std::list<TEMPLATEPARAMPAIR>* args = ts->tp->BaseType()->templateParam->second->byDeferred.args;
                    std::list<TEMPLATEPARAMPAIR>* val = nullptr;
                    sp = tp->templateParam->first;
                    sp = TemplateClassInstantiateInternal(sp, args, true);
                }
                if (sp)
                {
                    sp->tp = sp->tp->InitializeDeferred();
                    sp = sp->tp->BaseType()->sp;
                    ++find;
                    ++find;
                    for (; find != finde && sp; ++find)
                    {
                        SYMBOL* spo = sp;
                        if (!spo->tp->IsStructured())
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
                tp = tp->BaseType()->btp;
                if (tp->type == BasicType::templateparam_)
                {
                    if (tp->templateParam->second->type != TplType::typename_)
                        return false;
                    return ValidArg(tp);
                }
                if (tp->IsRef())
                    return false;
                break;
            case BasicType::memberptr_: {
                Type* tp1 = tp->sp->tp;
                if (tp1->type == BasicType::templateparam_)
                {
                    if (tp1->templateParam->second->type != TplType::typename_)
                        return false;
                    tp1 = tp1->templateParam->second->byClass.val;
                    if (!tp1)
                        return false;
                }
                if (!tp1->IsStructured())
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
                if (tp->IsFunction() || tp->IsArray() || (tp->IsStructured() && tp->BaseType()->sp->sb->isabstract))
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
static bool valFromDefault(std::list<TEMPLATEPARAMPAIR>* params, bool usesParams, std::list<Argument*>* args)
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
static void FillNontypeTypeDefaults(Type* tp, std::list<TEMPLATEPARAMPAIR>* enclosing)
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
                Type* tp1 = SynthesizeType(param.second->byNonType.tp, enclosing, false);
                if (!tp1 || tp1->type == BasicType::any_ || tp1->IsStructured() || tp1->IsRef() || tp1->IsFloat() ||
                    tp1->IsComplex() || tp1->IsImaginary())
                    return false;
            }
        }
    }
    return true;
}
SYMBOL* ValidateArgsSpecified(std::list<TEMPLATEPARAMPAIR>* params, SYMBOL* func, std::list<Argument*>* args,
                              std::list<TEMPLATEPARAMPAIR>* nparams)
{
    bool usesParams = !!args && args->size();
    auto it = func->tp->BaseType()->syms->begin();
    auto ite = func->tp->BaseType()->syms->end();
    inDefaultParam++;
    std::list<Argument*> arg1;
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
        DeclarationScope scope;
        if (func->sb->parentClass)
        {
            enclosingDeclarations.Add(func->sb->parentClass);
        }
        enclosingDeclarations.Add(func->templateParams);
        std::list<Argument*>::iterator ita, itae;
        if (args)
        {
            ita = args->begin();
            itae = args->end();
        }
        while (ita != itae && it != ite)
        {
            SYMBOL* sp1 = *it;
            if (sp1->tp->BaseType()->type == BasicType::ellipse_ || sp1->packed)
                packedOrEllipse = true;
            ++ita;
            ++it;
        }
        if (ita != itae && !packedOrEllipse)
        {
            inDefaultParam--;
            return nullptr;
        }
        while (it != ite)
        {
            SYMBOL* sp = *it;
            if (initTokenStreams.get(sp))
            {
                dontRegisterTemplate += templateDeclarationLevel != 0;
                ParseOnStream(initTokenStreams.get(sp), [=]() {
                    sp->sb->init = nullptr;
                    initialize(func, sp, StorageClass::parameter_, true, false, false, _F_TEMPLATEARGEXPANSION);
                });
                dontRegisterTemplate -= templateDeclarationLevel != 0;
                if (sp->sb->init && sp->sb->init->front()->exp && !ValidExp(&sp->sb->init->front()->exp))
                {
                    inDefaultParam--;
                    return nullptr;
                }
            }
            ++it;
        }
    }
    DeclarationScope scope(func->templateParams);
    std::list<Argument*>::iterator check, checke;
    if (args)
    {
        check = args->begin();
        checke = args->end();
    }
    for (auto sp1 : *func->tp->BaseType()->syms)
    {
        if (!ValidArg(sp1->tp))
        {
            inDefaultParam--;
            return nullptr;
        }
        auto tp1 = sp1->tp;
        while (tp1->IsPtr() || tp1->IsRef())
            tp1 = tp1->btp;
        if (tp1->IsStructured() && tp1->BaseType()->sp->templateParams)
        {
            for (auto& tpl : *tp1->BaseType()->sp->templateParams)
            {
                if (tpl.second->type == TplType::int_ && !tpl.second->byClass.val && tpl.second->byClass.dflt)
                {
                    Type* tpRight = SynthesizeType(tpl.second->byNonType.tp, nullptr, false);
                    if (tpl.second->packed)
                    {
                        if (tpl.second->byPack.pack)
                        {
                            for (auto& tpl1 : *tpl.second->byPack.pack)
                            {
                                if (tpl1.second->byNonType.dflt && !tpl1.second->byNonType.val)
                                {
                                    Type* tpLeft = LookupTypeFromExpression(tpl1.second->byNonType.dflt, nullptr, false);
                                    if (!tpLeft || !tpRight || !tpLeft->CompatibleType(tpRight))
                                    {
                                        inDefaultParam--;
                                        return nullptr;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        Type* tpLeft = LookupTypeFromExpression(tpl.second->byNonType.dflt, nullptr, false);
                        if (!tpLeft || !tpRight || !tpLeft->CompatibleType(tpRight))
                        {
                            inDefaultParam--;
                            return nullptr;
                        }
                    }
                }
            }
        }
        if (args && check != checke)
            ++check;
    }
    auto tpbt = func->tp->BaseType()->btp;
    if (tpbt->IsRef())
        tpbt = tpbt->BaseType()->btp;
    if (tpbt->IsDeferred())
    {
        auto args = tpbt->BaseType()->templateArgs;
        std::list<TEMPLATEPARAMPAIR>* params = func->templateParams;
        for (auto t : *args)
        {
            if (t.second->type != TplType::new_)
            {
                if (t.second->packed)
                {
                    if (t.second->byPack.pack)
                    {
                        for (auto t1 : *t.second->byPack.pack)
                        {
                            if (t1.second->byClass.val)
                            {
                                t1.second->byClass.dflt = t1.second->byClass.val;
                            }
                        }
                    }
                }
                else
                {
                    if (t.second->byClass.val)
                    {
                        t.second->byClass.dflt = t.second->byClass.val;
                    }
                    else
                    {
                        for (auto&& a : *params)
                            if (a.first && !strcmp(a.first->name, t.first->name))
                            {
                                    t.second->byClass.dflt = a.second->byClass.val;
                            }
                    }
                }
            }
        }
    }
    if (func->tp->BaseType()->btp->IsStructured())
    {
        std::list<TEMPLATEPARAMPAIR>* params = func->tp->BaseType()->btp->BaseType()->sp->templateParams;
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
    std::vector<Type**> tav;
    std::vector<Type*> tpp;
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
        Type* tp = tpp[i];
        tp->lref = hold[i * 2];
        tp->rref = hold[i * 2 + 1];
    }
    rv->sb->maintemplate = func;
    if (!ValidArg(func->tp->BaseType()->btp))
    {
        inDefaultParam--;
        return nullptr;
    }
    for (int i = 0; i < tpp.size(); i++)
    {
        Type* tp = tpp[i];
        *(tav[i]) = tp;
    }
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
    LexemeStream* head = nullptr;
    SYMBOL* oldMemberClass = instantiatingMemberFuncClass;
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
        head = currents->bodyTokenStream;
    }
    DeclarationScope scope(enclosing);
    auto oldContext = defaultParsingContext;
    defaultParsingContext = declareSym;

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
        DeclarationScope scope;
        if ((!args || itArgs == iteArgs || (!itArgs->second->byClass.val && itArgs->second->byClass.txtdflt)) && !itDest->second->byClass.val && !itDest->second->packed &&
            (itPrimary == itePrimary || !itPrimary->second->packed))
        {
            if (!itSrc->second->byClass.txtdflt)
            {
                defaultParsingContext = oldContext;
                instantiatingMemberFuncClass = oldMemberClass;
                return false; 
            }
            TemplateNamespaceScope namespaceScope(declareSym);
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
                enclosingDeclarations.Add(defaults);
            }
            else
            {
                pushContext(declareSym, false);
            }
            itDest->second->byClass.txtdflt = itSrc->second->byClass.txtdflt;
            itDest->second->byClass.txtargs = itSrc->second->byClass.txtargs;
            itDest->second->byNonType.txttype = itSrc->second->byNonType.txttype;
            bool failed = false;
            ParseOnStream(itSrc->second->byClass.txtdflt, [&]() {
                switch (itDest->second->type)
                {
                case TplType::typename_: {
                    noTypeNameError++;
                    itDest->second->byClass.val = TypeGenerator::TypeId(nullptr, StorageClass::cast_, false, true, false);
                    noTypeNameError--;
                    itDest->second->byClass.val->InstantiateDeferred();
                    if (!itDest->second->byClass.val || itDest->second->byClass.val->type == BasicType::any_ ||
                        (!templateDefinitionLevel && itDest->second->byClass.val->BaseType()->type == BasicType::templateselector_))
                    {
                        failed = true;
                        return;
                    }
                    break;
                }
                case TplType::template_: {
                    char buf[256];
                    Utils::StrCpy(buf, currentLex->value.s.a);
                    id_expression(nullptr, &itDest->second->byTemplate.val, nullptr, nullptr, nullptr, false, false, buf, sizeof(buf),
                        0);

                    if (!itDest->second->byTemplate.val)
                    {
                        failed = true;
                        return;
                    }
                }
                                       break;
                case TplType::int_: {
                    Type* tp1;
                    EXPRESSION* exp1 = nullptr;
                    if (itDest->second->byNonType.txttype)
                    {
                        int oldNesting = argumentNestingLevel;
                        LexemeStreamPosition start(currentStream);
                        ParseOnStream(itSrc->second->byNonType.txttype, [&]() {
                            openStructs = nullptr;
                            structLevel = 0;
                            argumentNestingLevel = 0;
                            noTypeNameError++;
                            tp1 = TypeGenerator::TypeId(nullptr, StorageClass::parameter_, true, false, false);
                            tp1->InstantiateDeferred();
                            noTypeNameError--;
                            argumentNestingLevel = oldNesting;
                            openStructs = oldOpenStructs;
                            structLevel = oldStructLevel;
                        });
                        start;
                        Type* tp2 = tp1;
                        while (tp2->IsPtr())
                            tp2 = tp2->BaseType()->btp;
                        if (tp2->type == BasicType::any_)
                        {
                            failed = true;
                            return;
                        }
                        itDest->second->byNonType.tp = tp1;
                        }
                    openStructs = nullptr;
                        structLevel = 0;
                        expression_no_comma(nullptr, nullptr, &tp1, &exp1, nullptr, _F_INTEMPLATEPARAMS);
                        optimize_for_constants(&exp1);
                        openStructs = oldOpenStructs;
                        structLevel = oldStructLevel;
                        itDest->second->byNonType.val = exp1;
                        if (!templateCompareTypes(itDest->second->byNonType.tp, tp1, true))
                        {
                            if (itDest->second->byNonType.tp->type == BasicType::auto_)
                                itDest->second->byNonType.tp = tp1;
                            if (!tp1->IsPtr() && !tp1->IsInt() && !isconstzero(tp1, exp1))
                            {
                                failed = true;
                                return;
                            }
                        }
                        Type* tp2 = LookupTypeFromExpression(exp1, nullptr, false);
                        if (!tp2 || tp2->type == BasicType::any_)
                        {
                            failed = true;
                            return;
                        }
                    }
                    break;
                default:
                    break;
                }
            });
            if (failed)
            {
                defaultParsingContext = oldContext;
                instantiatingMemberFuncClass = oldMemberClass;
                return false;
            }
        }
        if (itArgs != iteArgs)
            ++itArgs;
        if (itPrimary != itePrimary)
            ++itPrimary;
    }
    if (currents)
    {
        currents->bodyTokenStream = head;
    }
    defaultParsingContext = oldContext;
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
                            if (arg.second->byClass.val->IsStructured())
                            {
                                SetTemplateArgAccess(arg.second->byClass.val->BaseType()->sp, accessible);
                            }
                            else if (arg.second->byClass.val->BaseType()->type == BasicType::enum_)  // DAL FIXED
                            {
                                SetTemplateArgAccess(arg.second->byClass.val->BaseType()->sp, accessible);
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
void SwapMaprocessingTemplateArgs(SYMBOL* cls)
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
    SYMBOL* cls = sym;
    if (cls->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
        return cls;

    if (!isExtern)
    {
        if (sym->sb->decoratedName)
        {
            auto itx = classInstantiationMap.find(sym->sb->decoratedName);
            if (itx != classInstantiationMap.end())
                return itx->second;
        }
        LexemeStream* lex = nullptr;
        if (sym->sb->maintemplate && (!sym->sb->specialized || sym->sb->maintemplate->sb->specialized))
        {
            lex = bodyTokenStreams.get(sym->sb->maintemplate);
            if (lex)
                sym->tp = sym->sb->maintemplate->tp;
        }
        if (!lex)
            lex = bodyTokenStreams.get(sym);
        if (!lex && sym->sb->parentTemplate && (!sym->sb->specialized || sym->sb->parentTemplate->sb->specialized))
            lex = bodyTokenStreams.get(sym->sb->parentTemplate);
        if (lex)
        {
            if (sym->sb->decoratedName)
            {
                if (!strchr(sym->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
                    classInstantiationMap[sym->sb->decoratedName] = cls;
            }
            EnterInstantiation({ sym, currentLex ? currentLex->sourceFileName : 0, currentLex ? currentLex->sourceLineNumber : 0 });
            EnterPackedContext();
            int oldHeaderCount = templateHeaderCount;
            Optimizer::LIST* oldDeferred = deferred;
            bool defd = false;
            SYMBOL old;
            {
                TemplateNamespaceScope namespaceScope(sym);
                bool oldTemplateType = inTemplateType;
                auto oldLambdas = lambdas;
                int oldExpandingParams = isExpandingParams;
                std::list<NAMESPACEVALUEDATA*>* oldLocalNameSpace = localNameSpace;
                int oldInArgs = processingTemplateArgs;
                int oldArgumentNestingLevel = argumentNestingLevel;
                int oldFuncLevel = funcNestingLevel;
                int oldintypedef = processingTypedef;
                int oldTypeNameError = noTypeNameError;
                int oldParsingUsing = processingUsingStatement;
                int oldSearchingFunctions = currentlyInsertingFunctions;
                int oldInAssignRHS = assigningRHS;
                int oldResolvingStructDeclarations = resolvingStructDeclarations;
                int oldBodyIsDestructor = bodyIsDestructor;
                int oldTemplateDeclarationLevel = templateDeclarationLevel;
                int oldDontRegister = dontRegisterTemplate;
                bool oldFullySpecialized = isFullySpecialized;
                int oldInTemplateBody = processingTemplateBody;
                int oldInLoop = processingLoopOrConditional;
                processingLoopOrConditional = 0;
                processingTemplateBody = 0;
                isFullySpecialized = false;
                dontRegisterTemplate = 0;
                templateDeclarationLevel = 0;
                bodyIsDestructor = 0;
                resolvingStructDeclarations = 0;
                assigningRHS = 0;
                currentlyInsertingFunctions = 0;
                processingUsingStatement = 0;

                instantiatingClass++;

                noTypeNameError = 0;
                processingTypedef = 0;
                funcNestingLevel = 0;
                argumentNestingLevel = 0;
                processingTemplateArgs = 0;
                isExpandingParams = 0;
                NAMESPACEVALUEDATA nsvd = {};
                std::list<NAMESPACEVALUEDATA*> ns{&nsvd};
                localNameSpace = &ns;
                SetAccessibleTemplateArgs(cls->templateParams, true);
                deferred = nullptr;
                templateHeaderCount = 0;
                lambdas.clear();
                old = *cls;
                cls->parserSet = false;
                cls->sb->parentClass = SynthesizeParentClass(cls->sb->parentClass);
                SwapMaprocessingTemplateArgs(cls);
                {
                    DeclarationScope scope;
                    pushContext(cls, false);
                    cls->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                    cls->tp = old.tp->CopyType();
                    cls->tp->UpdateRootTypes();
                    cls->tp->syms = nullptr;
                    cls->tp->tags = nullptr;
                    cls->tp->sp = cls;
                    cls->sb->baseClasses = nullptr;
                    cls->sb->vbaseEntries = nullptr;
                    templateInstantiationLevel++;
                    SwapMaprocessingTemplateArgs(cls);
                    // making a copy because these things can be unraveled recursively
                    // and we don't want to mess with the status of older copies.
                    ParseOnStream(lex, [&]() {
                        cls->sb->instantiating = true;
                        innerDeclStruct(nullptr, cls, false,
                            cls->tp->type == BasicType::class_ ? AccessLevel::private_ : AccessLevel::public_,
                            cls->sb->isfinal, &defd, false, nullptr);
                        cls->sb->instantiating = false;
                        });
                    SwapMaprocessingTemplateArgs(cls);
                    SetAccessibleTemplateArgs(cls->templateParams, false);
                }
                instantiatingClass--;
                processingLoopOrConditional = oldInLoop;
                processingTemplateBody = oldInTemplateBody;
                isFullySpecialized = oldFullySpecialized;
                dontRegisterTemplate = oldDontRegister;
                templateDeclarationLevel = oldTemplateDeclarationLevel;
                bodyIsDestructor = oldBodyIsDestructor;
                resolvingStructDeclarations = oldResolvingStructDeclarations;
                assigningRHS = oldInAssignRHS;
                currentlyInsertingFunctions = oldSearchingFunctions;
                processingUsingStatement = oldParsingUsing;
                noTypeNameError = oldTypeNameError;
                processingTypedef = oldintypedef;
                lambdas = std::move(oldLambdas);
                templateInstantiationLevel--;
                inTemplateType = oldTemplateType;
                deferred = oldDeferred;
                cls->sb->instantiated = true;
                localNameSpace = oldLocalNameSpace;
                processingTemplateArgs = oldInArgs;
                argumentNestingLevel = oldArgumentNestingLevel;
                isExpandingParams = oldExpandingParams;
                funcNestingLevel = oldFuncLevel;
                templateHeaderCount = oldHeaderCount;
            }
            SwapMaprocessingTemplateArgs(cls);
            LeavePackedContext();
            LeaveInstantiation();
        }
    }
    return cls;
}
SYMBOL* TemplateClassInstantiate(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args, bool isExtern, StorageClass storage_class)
{
    if (templateDefinitionLevel)
    {
        SYMBOL* sym1 = MatchSpecialization(sym, args);
        if (sym1 && (storage_class == StorageClass::parameter_ || !processingTemplateBody))
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
                *tpx = tp->CopyType();
                (*tpx)->UpdateRootTypes();
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
    for (auto data : *sym->sb->overloadName->tp->syms)
    {
        if (data->sb->instantiated && TemplateInstantiationMatch(data, sym, true) && matchOverload(sym->tp, data->tp))
        {
            if (data->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
            {
                return data;
            }
//            if (!data->sb->deferredCompile && sym->sb->deferredCompile)
            {
 //               data->sb->deferredCompile = sym->sb->deferredCompile;
                auto hrs = sym->tp->BaseType()->syms->begin();
                for (auto sym : *data->tp->BaseType()->syms)
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
    auto tp = sym->tp->BaseType()->btp->BaseType();
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
            Type* tp2 = nullptr;

            if (matchOverload(sym->tp, sp1->tp))
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
            injectThisPtr(sym, sym->tp->BaseType()->syms);
        }
    }
    return sym;
}
static bool CheckConstCorrectness(Type* P, Type* A, bool byClass)
{
    while (P && A)
    {
        P = P->BaseType();
        A = A->BaseType();
        if (P->type != A->type)
            break;
        P = P->btp;
        A = A->btp;
        if (P && A)
        {
            if (byClass)
            {
                if ((A->IsConst() != P->IsConst()) || (A->IsVolatile() != P->IsVolatile()))
                    return false;
            }
            else
            {
                if ((A->IsConst() && !P->IsConst()) || (A->IsVolatile() && !P->IsVolatile()))
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
    TEMPLATEPARAM* hold[200];
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
                    if (pP.second->type == TplType::typename_ && pP.second->packed)
                    {
                        if (pP.second->byPack.pack)
                        {
                            for (auto&& tpl : *pP.second->byPack.pack)
                            {
                                if (current >= count)
                                {
                                    hconst[current] = 0;
                                    hvolatile[current] = 0;
                                    hold[current] = tpl.second;
                                    count++;
                                }
                                if (tpl.second->type == TplType::typename_)
                                {
                                    Type* tp = tpl.second->byClass.dflt;
                                    if (tp)
                                    {
                                        while (tp->IsRef())
                                            tp = tp->BaseType()->btp;

                                        if (tp->IsConst())
                                            hconst[current] = 1;
                                        if (tp->IsVolatile())
                                            hvolatile[current] = 1;
                                    }
                                }
                                current++;
                            }
                        }
                    }
                    else
                    {
                        if (current >= count)
                        {
                            hconst[current] = 0;
                            hvolatile[current] = 0;
                            hold[current] = pP.second;
                            count++;
                        }
                        if (pP.second->type == TplType::typename_)
                        {
                            Type* tp = pP.second->byClass.dflt;
                            if (tp)
                            {
                                while (tp->IsRef())
                                    tp = tp->BaseType()->btp;

                                if (tp->IsConst())
                                    hconst[current] = 1;
                                if (tp->IsVolatile())
                                    hvolatile[current] = 1;
                            }
                        }
                        current++;
                    }
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
                auto itA = A->begin();
                auto iteA = A->end();
                int n = 0;
                for (; n < count && itA != iteA;)
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
                        if (A && hold[n]->type == TplType::typename_)
                        {
                            Type* ta = itA->second->byClass.dflt;
                            Type* tp = hold[n]->byClass.dflt;
                            if (ta && tp)
                            {
                                while (ta->IsRef())
                                    ta = ta->BaseType()->btp;
                                while (tp->IsRef())
                                    tp = tp->BaseType()->btp;
                                if ((ta->IsConst() && !tp->IsConst() && hconst[current]) ||
                                    (ta->IsVolatile() && !tp->IsVolatile() && hvolatile[current]) ||
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
                    n++;
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
                    Type* td = pP.second->byClass.dflt;
                    Type* tv = pP.second->byClass.val;
                    if (!tv && pP.second->byClass.dflt->IsStructured() &&
                        pP.second->byClass.dflt->BaseType()->sp->sb->templateLevel)
                    {
                        if (!TemplateConstMatchingInternal(pP.second->byClass.dflt->BaseType()->sp->templateParams))
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
                        if (td->IsRef())
                            td = td->BaseType()->btp;
                        if (tv->IsRef())
                            tv = tv->BaseType()->btp;
                        if (td->type == BasicType::memberptr_)
                            td = td->BaseType()->btp;
                        if (tv->type == BasicType::memberptr_)
                            tv = tv->BaseType()->btp;
                        bool matches = false;
                        if ((td->IsConst() != tv->IsConst()) || ((td->IsVolatile() != tv->IsVolatile())) ||
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
                                    Type* tv = pP.second->byClass.val;
                                    if (tv->IsRef())
                                        tv = tv->BaseType()->btp;
                                    if (tv->IsConst() || tv->IsVolatile())
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
                                        Type* tv = pP.second->byClass.val;
                                        if (tv->IsRef())
                                            tv = tv->BaseType()->btp;
                                        if (tv->IsConst() || tv->IsVolatile())
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
    Type *tdv = nullptr, *tdd = nullptr;
    Type *tvv = nullptr, *tvd = nullptr;
    if (itdflt != dflt->end() && itval != val->end() && itdflt->second->type == TplType::typename_ && itdflt->second->byClass.val &&
        itval->second->byClass.val && itdflt->second->byClass.val->IsPtr() && itval->second->byClass.val->IsPtr())
    {
        tdv = itdflt->second->byClass.val;
        tdd = itdflt->second->byClass.dflt;
        tvv = itval->second->byClass.val;
        tvd = itval->second->byClass.dflt;
        ptr = true;
        while (itdflt->second->byClass.val->IsPtr())
        {
            itdflt->second->byClass.val = itdflt->second->byClass.val->BaseType()->btp;
            if (itdflt->second->byClass.dflt && itdflt->second->byClass.dflt->IsPtr())
                itdflt->second->byClass.dflt = itdflt->second->byClass.dflt->BaseType()->btp;
        }
        while (itval->second->byClass.val->IsPtr())
        {
            itval->second->byClass.val = itval->second->byClass.val->BaseType()->btp;
            if (itval->second->byClass.dflt && itval->second->byClass.dflt->IsPtr())
                itval->second->byClass.dflt = itval->second->byClass.dflt->BaseType()->btp;
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
                    if (find.second->type == params->front().second->type)
                    {
                        if (find.second->packed)
                        {
                            if (find.second->byPack.pack)
                            {
                                params->front().second->byPack.pack = find.second->byPack.pack;
                            }
                        }
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
             itval->second->byClass.dflt && itval->second->byClass.val && itval->second->byClass.dflt->IsStructured() &&
             itval->second->byClass.val->IsStructured())
    {
        std::list<TEMPLATEPARAMPAIR>* tpdflt = itval->second->byClass.dflt->BaseType()->sp->templateParams;
        std::list<TEMPLATEPARAMPAIR>* tpval = itval->second->byClass.val->BaseType()->sp->templateParams;
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
             itval->second->byClass.dflt && itval->second->byClass.val && itval->second->byClass.dflt->IsFunction() &&
             itval->second->byClass.val->IsFunction())
    {
        Type* tpd = itval->second->byClass.dflt->BaseType()->btp;
        Type* tpv = itval->second->byClass.val->BaseType()->btp;
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
        auto hrd = itval->second->byClass.dflt->BaseType()->syms->begin();
        auto hrde = itval->second->byClass.dflt->BaseType()->syms->end();
        auto hrv = itval->second->byClass.val->BaseType()->syms->begin();
        auto hrve = itval->second->byClass.val->BaseType()->syms->end();
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
#if 0
                                else
                                {
                                    auto next = find.second->byPack.pack;
                                    next->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                                    next->back().second->type = TplType::typename_;
                                    next->back().second->byClass.val = tpv;
                                }
#endif
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
            if (itval != val->end())
            {
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
        }
        if (params && params->front().second->type == TplType::typename_ && !params->front().second->packed &&
            params->front().second->byClass.dflt &&
            params->front().second->byClass.dflt->BaseType()->type == BasicType::templateselector_ &&
            (*params->front().second->byClass.dflt->BaseType()->sp->sb->templateSelector)[1].isTemplate)
        {
            std::list<TEMPLATEPARAMPAIR>* param1 =
                (*params->front().second->byClass.dflt->BaseType()->sp->sb->templateSelector)[1].templateParams;
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
        auto itnParams = nparams->begin();
        ++itnParams;
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
                {
                    ++itParams;
                    if (itnParams != nparams->end())
                        ++itnParams;
                }
            }
            if (itInitial != iteInitial && itParams != params->end())
            {
                if (itParams->second->packed)
                {
                    auto it = itParams;
                    ++it;
                    while (itInitial != iteInitial && (!itInitial->second->packed || itParams == params->end()) &&
                           (itParams->second->type == itInitial->second->type || (itInitial->second->type != TplType::int_ && itParams->second->type != TplType::int_)))
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
                            if (itParams->second->type == itInitial->second->type)
                            {
                                last->back().second->byClass.val = (Type*)dflt;
                            }
                            else if (itParams->second->type == TplType::typename_)
                            {
                                // itInitial must be template
                                last->back().second->byClass.val = ((SYMBOL*)dflt)->tp;
                            }
                            else
                            {
                                // itInitial must be typename, itParams template
                                if (((Type*)dflt)->IsStructured())
                                {
                                    if (!DeduceTemplateParam(&*itParams, nullptr, (Type*)dflt, nullptr, true, true))
                                    {
                                        rv = nullptr;
                                    }
                                }
                            }
                            if (itParams->second->type == TplType::int_)
                            {
                                auto nonTypeTp = itInitial->second->byNonType.tp;
                                if (nonTypeTp->type == BasicType::templateparam_)
                                {
                                    if (nonTypeTp->templateParam->second->packed)
                                    {
                                        if (nonTypeTp->templateParam->second->byPack.pack &&
                                            nonTypeTp->templateParam->second->byPack.pack->size())
                                        {
                                            nonTypeTp = nonTypeTp->templateParam->second->byPack.pack->front().second->byNonType.tp;
                                        }
                                    }
                                    else
                                    {
                                        nonTypeTp = nonTypeTp->templateParam->second->byClass.dflt;
                                    }
                                }
                                last->back().second->byNonType.tp = nonTypeTp;
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
                        if (max && itmax != max->end())
                            ++itmax;
                    }
                    auto it1 = itParams;
                    ++it1;
                    if (it1 != params->end())
                    {
                        ++itParams;
                        if (itnParams != nparams->end())
                            ++itnParams;
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
                        if (dflt && ((Type*)dflt)->IsStructured())
                        {
                            if (!DeduceTemplateParam(&*itParams, nullptr, (Type*)dflt, nullptr, true, true))
                            {
                                rv = nullptr;
                            }

                            itParams->second->initialized = true;
                            ++itParams;
                            if (itnParams != nparams->end())
                                ++itnParams;
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
                            if (itnParams != nparams->end())
                                ++itnParams;
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
                                if (!templateCompareTypes(itParams->second->byClass.val, (Type*)dflt, true) ||
                                    (itParams->second->byClass.val->IsStructured() &&
                                     itParams->second->byClass.val->BaseType()->sp->sb->templateLevel &&
                                     !SameTemplate(itParams->second->byClass.val, (Type*)dflt, true)))
                                    rv = nullptr;
                                break;
                            case TplType::int_: {
                                EXPRESSION* exp = copy_expression(itParams->second->byNonType.val);
                                optimize_for_constants(&exp);
                                if (itParams->second->byNonType.val && (!dflt || !equalTemplateMakeIntExpression(exp, (EXPRESSION*)dflt)))
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
                        itParams->second->byClass.val = (Type*)dflt;
                        if (spsyms)
                        {
                            if (itParams->second->type == TplType::typename_)
                            {
                                if (itParams->second->byClass.dflt && itParams->second->byClass.dflt->IsDeferred())
                                {
                                    Type** tp1 = &itParams->second->byClass.dflt;
                                    while ((*tp1)->type != BasicType::templatedeferredtype_)
                                        tp1 = &(*tp1)->btp;
                                    auto tp = Type::MakeType(BasicType::struct_);
                                    tp->sp = (*tp1)->sp;
                                    itParams->second->byClass.dflt = tp;
                                }
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
                                if (itParams->second->byClass.dflt &&
                                    itParams->second->byClass.dflt->type == BasicType::templateparam_)
                                {
                                    if (!DeduceTemplateParam(itParams->second->byClass.dflt->templateParam, nullptr,
                                                             itParams->second->byTemplate.dflt->tp, nullptr, true, true))
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
                                    !equalTemplateMakeIntExpression(itParams->second->byNonType.dflt, exp))
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
        if (itInitial != iteInitial &&
            (max && itmax != max->end() || !spsyms ||
             (itInitial->second->packed && itInitial->second->byPack.pack && itInitial->second->byPack.pack->size())))
            rv = nullptr;
        if (spsyms)
        {
            for (auto its = itParams; its != params->end(); ++its)
            {
                auto&& primary = *its;
                if (primary.second->type == TplType::typename_)
                {
                    bool doit = true;
                    if (primary.second->byClass.dflt && primary.second->byClass.dflt->type == BasicType::typedef_ && primary.second->byClass.dflt->sp && primary.second->byClass.dflt->sp->templateParams)
                    {
                        auto it1 = primary.second->byClass.dflt->sp->templateParams->begin();
                        ++it1;
                        // <...>
                        doit = it1 == primary.second->byClass.dflt->sp->templateParams->end() ||
                            !(*it1).second->packed  || (*it1).second->byPack.pack ;
                    }
                    if (doit)
                    {
                        auto temp = ResolveDeclType(sp, &primary);
                        temp = ResolveTemplateSelectors(sp, temp, false);
                        if (primary.second->byClass.dflt != temp->front().second->byClass.dflt)
                        {
                            primary.second->byClass.val = temp->front().second->byClass.dflt;
                        }
                    }
                    else
                    {
                        rv = nullptr;
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
        if ((!IsDefiningTemplate() || isFullySpecialized || (processingTemplateHeader && templateDefinitionLevel == 1)) &&
            (processingTemplateArgs < 1 || !found))
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
                            if (itnParams != nparams->end())
                                ++itnParams;
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
                            case TplType::typename_: {
                                auto tpx = itParams->second->byClass.dflt;
                                while (tpx && tpx->IsPtr())
                                    tpx = tpx->BaseType()->btp;
                                tpx = tpx->BaseType();
                                if (tpx->type != BasicType::templateparam_ && tpx->type != BasicType::templateselector_ &&
                                    tpx->type != BasicType::templatedecltype_ && tpx->type != BasicType::templatedeferredtype_ &&
                                    !templateCompareTypes(itParams->second->byClass.val, itParams->second->byClass.dflt, true))
                                    rv = nullptr;
                                break;
                            }
                            case TplType::int_: {
                                EXPRESSION* exp = copy_expression(itParams->second->byNonType.val);
                                optimize_for_constants(&exp);
                                if (itParams->second->byNonType.dflt &&
                                    !equalTemplateMakeIntExpression(exp, itParams->second->byNonType.dflt))
                                    rv = nullptr;
                            }
                            break;
                            default:
                                break;
                        }
                    }
                    if (rv && itnParams != nparams->end())
                    {
                        std::list<TEMPLATEPARAMPAIR> b(itnParams, nparams->end());
                        TransferClassTemplates(params, params, &b);
                    }
                    ++itParams;
                    if (itnParams != nparams->end())
                        ++itnParams;
                    if (itParams == iteParams && !tis.empty())
                    {
                        iteParams = tis.top();
                        tis.pop();
                        itParams = tis.top();
                        tis.pop();
                        ++itParams;
                        if (itnParams != nparams->end())
                            ++itnParams;
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
                if ((!IsDefiningTemplate()) && itPrimary->second->type == TplType::typename_ &&
                    itPrimary->second->byClass.val && itPrimary->second->byClass.val->type == BasicType::templateselector_)
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
            decltype(args->begin()) itArgs;
            if (args)
            {
                itArgs = args->begin();
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
                                 !templateCompareTypes(itParams->second->byClass.dflt, itArgs->second->byClass.dflt, true)))
                                rv = nullptr;
                            break;
                        case TplType::int_: {
                            if (itParams->second->byNonType.dflt &&
                                (!itArgs->second->byNonType.dflt ||
                                 !templateCompareTypes(itParams->second->byNonType.tp, itArgs->second->byNonType.tp, true)))
                                rv = nullptr;
                        }
                        break;
                        default:
                            break;
                    }
                }
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
static bool checkArgType(Type* tp, bool checkDeduced, bool checkDeclaring)
{
    while (tp->IsPtr() || tp->IsRef())
        tp = tp->BaseType()->btp;
    if (tp->IsFunction())
    {
        SYMBOL* sym = tp->BaseType()->sp;
        if (!checkArgType(tp->BaseType()->btp, checkDeduced, checkDeclaring))
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
    else if (tp->IsStructured())
    {
        if (tp->BaseType()->sp->sb)
        {
            if (tp->BaseType()->sp->sb->instantiated && tp->BaseType()->sp->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
                return true;
            if (tp->BaseType()->sp->sb->templateLevel)
            {
                return allTemplateArgsSpecified(tp->BaseType()->sp, tp->BaseType()->sp->templateParams, false,
                                                checkDeclaring);
            }
            if (checkDeclaring && tp->BaseType()->sp->sb->declaringRecursive && tp->BaseType()->sp->sb->templateLevel)
                return false;
        }
    }
    else if (tp->BaseType()->type == BasicType::templateparam_)
    {
        if (tp->BaseType()->templateParam->second->packed)
        {
            auto pack = tp->BaseType()->templateParam->second->byPack.pack;
            if (pack)
            {
                for (auto&& tpl : *pack)
                {
                    if (!tp->BaseType()->templateParam->second->byClass.val)
                        return false;
                    if (tp->BaseType()->templateParam->second->byClass.val->type == BasicType::templateparam_)
                        return false;
                }
            }
        }
        else
        {
            if (!tp->BaseType()->templateParam->second->byClass.val)
                return false;
            if (tp->BaseType()->templateParam->second->byClass.val->type == BasicType::templateparam_)
                return false;
        }
    }
    else if (tp->BaseType()->type == BasicType::templatedecltype_)
        return false;
    else if (tp->BaseType()->type == BasicType::templateselector_)
    {
        return false;
    }
    else if (tp->BaseType()->type == BasicType::memberptr_)
    {
        if (!checkArgType(tp->BaseType()->sp->tp, checkDeduced, checkDeclaring))
            return false;
        if (!checkArgType(tp->BaseType()->btp, checkDeduced, checkDeclaring))
            return false;
    }
    else if (tp->type == BasicType::typedef_ && tp->sp->templateParams)
    {
        return allTemplateArgsSpecified(tp->sp, tp->sp->templateParams, checkDeduced, checkDeclaring);
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
                    while (exp->left && (IsCastValue(exp) || IsLValue(exp)))
                    {
                        exp = exp->left;
                    }
                    if (!isarithmeticconst(exp))
                    {
                        switch (exp->type)
                        {
                            case ExpressionNode::pc_:
                            case ExpressionNode::global_:
                            case ExpressionNode::callsite_:
                            case ExpressionNode::comma_:
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
            if (!arg->second->byNonType.tp)
                return false;
            return checkArgType(arg->second->byNonType.tp, checkDeduced, checkDeclaring);
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
                if ((IsDefiningTemplate() &&
                     (!it->second->byPack.pack || !it->second->byPack.pack->size())) ||
                    !allTemplateArgsSpecified(sym, it->second->byPack.pack, false, checkDeclaring))
                    return false;
                if (checkDeduced && !it->second->deduced && !it->second->initialized && !it->second->byClass.txtdflt)
                {
                    return false;
                }
            }
            else
            {
                if (sym)
                {
                    if (it->second->type == TplType::typename_)
                    {
                        Type* tp = it->second->byClass.val;
                        if (tp && tp->BaseType()->type == BasicType::any_)
                            return false;
                        if (SameTemplate(tp, sym->tp))
                            return false;
                    }
                }
                if (!checkArgSpecified(&*it, checkDeduced, checkDeclaring))
                {
                    return false;
                }
            }
            if (checkDeduced && !it->second->deduced && !it->second->initialized && ! it->second->byClass.txtdflt)
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
                    if (current.second->byClass.dflt && current.second->byClass.dflt->IsStructured())
                    {
                        if (current.second->byClass.val->IsStructured())
                        {
                            auto tpv = current.second->byClass.dflt->BaseType()->sp->templateParams;
                            auto tpx = current.second->byClass.val->BaseType()->sp->templateParams;
                            auto itv = tpv->begin();
                            auto itl = tpx->begin();
                            for (; itv != tpv->end() && itl != tpx->end(); ++itv, ++itl)
                            {
                                TemplateArgsAdd(&*itl, &*itv, base);
                            }
                        }
                    }
                    else if (current.second->byClass.val->IsStructured())
                    {
                        if (current.second->byClass.val->BaseType()->sp->templateParams)
                            for (auto&& tpx : *current.second->byClass.val->BaseType()->sp->templateParams)
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
                else if (param.second->byClass.val)
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
                        Type* tp1 = tpx1.second->byClass.dflt;
                        if (tp1->IsConst() || tp1->IsVolatile())
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
                Type* tp = tpl.second->byClass.dflt;
                if (tpl.second->packed)
                {
                    count += 1 + SpecializationComplexity(tpl.second->byPack.pack);
                }
                else if (tp)
                {
                    bool isfunc = tp->IsFunction() || tp->IsFunctionPtr();
                    while (tp && tp->btp)
                    {
                        if (tp->type != BasicType::typedef_)
                            count++;
                        tp = tp->btp;
                    }
                    if (tp && tp->IsStructured() && tp->sp->templateParams)
                        count += 1 + SpecializationComplexity(tp->sp->templateParams);
                    else if (isfunc && tp && (tp->IsArithmetic() || tp->type == BasicType::void_))
                        count++;
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
    SYMBOL* found1 = nullptr, * found2 = nullptr;
    SYMBOL** spList, ** origList;
    std::list<TEMPLATEPARAMPAIR>* search = args;
    int count = 0;
    noErr |= matchOverloadLevel;
    args = ResolveClassTemplateArgs(sp, args);
    TemplateArgInstantiateDeferred(args);
    auto argsorig = args;
    if (args && args->front().second->type == TplType::new_)
    {
        auto it = args->begin();
        ++it;
        auto args1 = templateParamPairListFactory.CreateList();
        args1->insert(args1->begin(), it, args->end());
        args = args1;
    }
    if (sp->sb->parentTemplate)
        sp = sp->sb->parentTemplate;

    DotNetPELib::SHA1Context context;
    found1 = LookupTemplateClass(context, sp, args);
    if (found1)
    {
        if (allTemplateArgsSpecified(found1, found1->templateParams))
        {
             return found1;
        }
        found1 = nullptr;
    }
    if (sp->sb->specializations)
        n += sp->sb->specializations->size();
    spList = (SYMBOL**)alloca(n * sizeof(SYMBOL*));
    origList = (SYMBOL**)alloca(n * sizeof(SYMBOL*));
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
        if (count1 > 1 && templateDefinitionLevel)
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
    if (!found1 && !templateDefinitionLevel && spList[0])
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

            found1->tp = sym->tp->CopyType();
            found1->tp->UpdateRootTypes();
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
            DotNetPELib::SHA1Context generatedContext;
            auto found2 = LookupGeneratedTemplateClass(generatedContext, found1);
            if (found2 && (found2->sb->specialized || !found1->sb->specialized) &&
                allTemplateArgsSpecified(found2, found2->templateParams))
            {
                // may get here if a specialization list is often being added to....
                restoreParams(origList, n);
                return found2;
            }

            if (!parent->sb->instantiations)
                parent->sb->instantiations = symListFactory.CreateList();
            parent->sb->instantiations->push_back(found1);
            if (!strchr(found1->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
            {
                RegisterTemplateClass(context, found1);
                RegisterTemplateClass(generatedContext, found1);
            }
        }
        else
        {
            found1 = CopySymbol(found1);
            found1->sb->maintemplate = sym;
            found1->tp = sym->tp->CopyType();
            found1->tp->UpdateRootTypes();
            found1->tp->sp = found1;

            found1->templateParams = templateParamPairListFactory.CreateList();
            found1->templateParams->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
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
            SetLinkerNames(found1, Linkage::cdecl_);
        }
    }

    restoreParams(origList, n);
    return found1;
}
SYMBOL* GetVariableTemplate(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    TemplateArgInstantiateDeferred(args);
    // this implementation does simple variables and pointers, but not arrays/function pointers
    std::list<TEMPLATEPARAMPAIR>* unspecialized = sp->templateParams;
    SYMBOL **origList, **spList, *found1, *found2;
    int n = 1;
    int count1, i;
    Type** tpi;
    if (sp->sb->specializations)
        n += sp->sb->specializations->size();
    spList = (SYMBOL**)alloca(n * sizeof(SYMBOL*));
    origList = (SYMBOL**)alloca(n * sizeof(SYMBOL*));
    origList[0] = sp;
    spList[0] = ValidateClassTemplate(sp, unspecialized, args);
    if (!spList[0])
    {
        diag("splist[0] == 0");
        return nullptr;
    }
    tpi = &spList[0]->tp;
    while ((*tpi)->IsRef() || (*tpi)->IsPtr())
        tpi = &(*tpi)->BaseType()->btp;
    if ((*tpi)->IsStructured() && (*tpi)->BaseType()->sp->sb->templateLevel)
    {
        SYMBOL* sym = GetClassTemplate((*tpi)->BaseType()->sp, args, true);
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
            while ((*tpi)->IsRef() || (*tpi)->IsPtr())
                tpi = &(*tpi)->BaseType()->btp;
            if ((*tpi)->IsStructured() && (*tpi)->BaseType()->sp->sb->templateLevel)
            {
                SYMBOL* sym = GetClassTemplate((*tpi)->BaseType()->sp, args, true);
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
    if (count1 > 1 && templateDefinitionLevel)
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
            found1->tp = sym->tp->CopyType();
            found1->tp->UpdateRootTypes();
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
            DotNetPELib::SHA1Context generatedContext;
            auto found2 = LookupGeneratedTemplateClass(generatedContext, found1);
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
                std::list<Initializer*>* init = initListFactory.CreateList();
                auto begin = found1->sb->init->begin();
                auto end = found1->sb->init->end();
                RecalculateVariableTemplateInitializers(begin, end, &init, found1->tp, 0);
                found1->sb->init = init;
                if (found1->tp->IsArithmetic() && isarithmeticconst(init->front()->exp))
                {
                    found1->sb->storage_class = StorageClass::constant_;
                }
            }
            found1->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
            if (!strchr(found1->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
                RegisterTemplateClass(generatedContext, found1);
            if (found1->sb->storage_class != StorageClass::constant_)
                if (!IsDefiningTemplate())
                    InsertInlineData(found1);
        }
        else
        {
            found1 = CopySymbol(found1);
            found1->sb->maintemplate = sym;
            found1->tp = sym->tp->CopyType();
            found1->tp->UpdateRootTypes();

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

TEMPLATEPARAMPAIR* TypeAliasSearch(SYMBOL* sym,  bool toponly)
{
    for (auto&& s : enclosingDeclarations)
    {
        if (s.tmpl)
        {
            for (auto&& arg : *s.tmpl)
            {
                if (arg.first && !strcmp(arg.first->name, sym->name))
                {
                    if (sym->templateId == arg.first->templateId)
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
        TEMPLATEPARAMPAIR* found = TypeAliasSearch((*exp)->v.sp, false);
        if (found && found->second->byNonType.dflt)
        {
            *exp = found->second->byNonType.dflt;
        }
        rv = true;
    }
    else if ((*exp)->type == ExpressionNode::sizeofellipse_)
    {
        TEMPLATEPARAMPAIR* found = TypeAliasSearch((*exp)->v.templateParam->first, false);
        if (found)
        {
            (*exp)->v.templateParam->second = found->second;
        }
        rv = true;
    }
    else if ((*exp)->type == ExpressionNode::templateselector_)
    {
        SpecifyTemplateSelector(&(*exp)->v.templateSelector, (*exp)->v.templateSelector, true, sym, origTemplate, origUsing);
        rv = true;
    }
    return rv;
}
static EXPRESSION* SpecifyArgInt(SYMBOL* sym, EXPRESSION* exp, std::list<TEMPLATEPARAMPAIR>* orig,
                                 std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static Type* SpecifyArgType(SYMBOL* sym, Type* tp, TEMPLATEPARAM* tpt, std::list<TEMPLATEPARAMPAIR>* orig,
                            std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing);
static void SpecifyOneArg(SYMBOL* sym, TEMPLATEPARAMPAIR* temp, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                          std::list<TEMPLATEPARAMPAIR>* origUsing);
void SearchAlias(SYMBOL* srch, TEMPLATEPARAMPAIR* x, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* origTemplate,
                 std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    TEMPLATEPARAMPAIR* rv = TypeAliasSearch(srch, false);
    if (rv)
    {
        if (x->second->packed && !rv->second->packed)
        {
            x->second->byPack.pack = templateParamPairListFactory.CreateList();
            x->second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, rv->second});
        }
        else if (rv->second->packed && unpackingTemplate && !x->second->ellipsis && !x->second->resolved)
        {
            auto tpl = LookupPackedInstance(*rv);
            if (tpl)
            {
                x->second = tpl;
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
                    SpecifyOneArg(sym, &tpx, origTemplate, origUsing);
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
static Type* ReplaceTemplateParam(Type* in)
{
    Type* find = in;
    while (find && find->type != BasicType::templateparam_)
        find = find->btp;
    if (find)
    {
        if (!find->templateParam->second->packed && find->templateParam->first)
        {
            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(find->templateParam->first, false);
            if (rv && !rv->second->packed && rv->second->byClass.dflt)
            {
                Type** last = &find;
                while (in && in->type != BasicType::templateparam_)
                {
                    *last = in->CopyType();
                    last = &(*last)->btp;
                    in = in->btp;
                }
                *last = rv->second->byClass.dflt;
                find->UpdateRootTypes();
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
                if (!templateDefinitionLevel)
                {
                    Type* basetp = oldItem.tp;
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
                        TEMPLATEPARAMPAIR* rv = TypeAliasSearch(oldItem.sp, false);
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
                                Type* tp = tpx->byClass.val ? tpx->byClass.val : tpx->byClass.dflt;
                                if (tp && tp->IsStructured())
                                    (*rvs)->back().sp = tp->BaseType()->sp;
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
                            if (ittp->second->type == TplType::int_ && ittp->second->byNonType.dflt)
                            {
                                x->back().second->byNonType.dflt = copy_expression(x->back().second->byNonType.dflt);
                                replaced = ReplaceIntAliasParams(&x->back().second->byNonType.dflt, sym, origTemplate, origUsing);
                                if (replaced)
                                    optimize_for_constants(&x->back().second->byNonType.dflt);
                            }
                            if (!replaced &&  (ittp->first || (ittp->second->type == TplType::typename_ && ittp->second->byClass.dflt)) &&
                                    (expression || (ittp->second->type == TplType::int_ || !ittp->second->byClass.dflt)) ||
                                ((!IsDefiningTemplate()) && ittp->second->type == TplType::typename_ &&
                                 ittp->second->byClass.dflt->IsDeferred()))
                            {
                                if (!expression && ittp->second->type == TplType::int_ && ittp->second->byNonType.dflt &&
                                    ittp->second->byNonType.dflt->type == ExpressionNode::templateparam_)
                                {
                                    SearchAlias(ittp->second->byNonType.dflt->v.sp, &x->back(), sym, origTemplate, origUsing);
                                }
                                else if (expression && ittp->second->type == TplType::int_ && x->back().second->byNonType.dflt)
                                {
                                    if (!IsConstantExpression(x->back().second->byNonType.dflt, false, false))
                                        SearchAlias(ittp->first, &x->back(), sym, origTemplate, origUsing);
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
                                        SearchAlias(ittp->first, &x->back(), sym, origTemplate, origUsing);
                                    }
                                    else if (!x->back().second->byClass.val && !x->back().second->byClass.dflt)
                                    {
                                        SearchAlias(ittp->first, &x->back(), sym, origTemplate, origUsing);
                                    }
                                    else
                                    {
                                        SpecifyOneArg(sym, &x->back(), origTemplate, origUsing);
                                    }
                                }
                                else
                                {
                                    SearchAlias(ittp->first, &x->back(), sym, origTemplate, origUsing);
                                }
                            }
                            if (x->back().second->type == TplType::typename_)
                            {
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
                    (*rvs)->back().arguments = argumentListFactory.CreateList();
                    for (auto ilx : *oldItem.arguments)
                    {
                        auto arg = Allocate<Argument>();
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
        while (exp->type == ExpressionNode::comma_)
            exp = exp->right;
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
            {
                if (exp->v.sp->tp->BaseType()->templateParam)
                    rv = TypeAliasSearch(exp->v.sp->tp->BaseType()->templateParam->first, false);
                else
                    rv = nullptr;
            }
            else
            {
                rv = TypeAliasSearch(exp->v.sp, false);
            }
            if (rv)
            {
                if (rv->second->type == TplType::int_)
                {
                    if (rv->second->packed && !exp->v.sp->tp->BaseType()->templateParam->second->ellipsis)
                    {
                        if (unpackingTemplate)
                        {
                            auto tpl = LookupPackedInstance(*rv);
                            if (tpl)
                            {
                                auto exp1 = tpl->byNonType.val ? tpl->byNonType.val : tpl->byNonType.dflt;
                                if (exp1)
                                    exp = exp1;  // dal fixme
                            }
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
                        Type* dflt = rv->second->byClass.dflt;
                        if (unpackingTemplate && !rv->second->resolved && rv->second->packed &&
                            !exp->v.sp->tp->BaseType()->templateParam->second->ellipsis)
                        {
                            auto tpl = LookupPackedInstance(*rv);
                            if (tpl)
                            {
                                dflt = tpl->byClass.val ? tpl->byClass.val : tpl->byClass.dflt;
                                //  fixme                               dflt = rv->second->byClass.val ? rv->second->byClass.val :
                                //  rv->second->byClass.dflt;
                            }
                            else
                            {
                                dflt = nullptr;
                            }
                        }
                        if (dflt)
                        {
                            // typename, allocate space for a type...
                            if (dflt->IsStructured())
                            {
                                exp = AnonymousVar(StorageClass::auto_, dflt);
                            }
                            else
                            {
                                exp = AnonymousVar(StorageClass::auto_, dflt);
                                if (dflt->IsRef())
                                {
                                    Type* tp1 = dflt->BaseType()->btp;
                                    Dereference(tp1, &exp);
                                }
                                else
                                {
                                    Dereference(dflt, &exp);
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
            if (unpackingTemplate)
            {
                TEMPLATEPARAMPAIR* rv = TypeAliasSearch(exp->v.sp, false);
                if (rv && rv->second->packed && !rv->second->resolved)
                {
                    auto tpl = LookupPackedInstance(*rv);
                    if (tpl)
                    {
                        exp = tpl->byNonType.dflt;
                    }
                }
            }
        }
        else if (exp->type == ExpressionNode::callsite_ || exp->type == ExpressionNode::funcret_)
        {
            EXPRESSION *exp1 = nullptr, **last = &exp1;
            ;
            while (exp && exp->type == ExpressionNode::funcret_)
            {
                *last = Allocate<EXPRESSION>();
                **last = *exp;
                exp = exp->left;
                last = &(*last)->left;
            }
            if (exp)
            {
                *last = Allocate<EXPRESSION>();
                **last = *exp;
                (*last)->v.func = Allocate<CallSite>();
                *(*last)->v.func = *exp->v.func;
            }
            else
            {
                diag("SpecifyArgInt: nullptr"); 
                return MakeIntExpression(ExpressionNode::c_i_, 0);
            }
            std::list<TEMPLATEPARAMPAIR>* tpxx = (*last)->v.func->templateParams;
            if (tpxx)
            {
            if ((*last)->v.func->sp->tp->type != BasicType::aggregate_)
                {
                    (*last)->v.func->sp = (*last)->v.func->sp->sb->overloadName;
                    (*last)->v.func->functp = (*last)->v.func->sp->tp;
                    /*
                        auto ctype = (*last)->v.func->functp;
                    auto sym = GetOverloadedFunction(&ctype, &(*last)->v.func->fcall, (*last)->v.func->sp, (*last)->v.func, nullptr,
                    false, false, 0); if (sym)
                    {
                        (*last)->v.func->sp = sym;
                        (*last)->v.func->functp = sym->tp;
                    }
                    */
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
                            SearchAlias(x1->back().first, &x1->back(), sym, origTemplate, origUsing);
                        }
                    }
                }
            }
            if ((*last)->v.func->arguments)
            {
                auto old = (*last)->v.func->arguments;
                (*last)->v.func->arguments = argumentListFactory.CreateList();
                for (auto arg : *old)
                {
                    if (arg->nested)
                    {
                        for (auto arg2 : *arg->nested)
                        {
                            auto arg1 = Allocate<Argument>();
                            *arg1 = *arg2;
                            (*last)->v.func->arguments->push_back(arg1);
                            arg1->exp = SpecifyArgInt(sym, arg1->exp, orig, origTemplate, origUsing);
                            arg1->tp = LookupTypeFromExpression(arg1->exp, nullptr, false);
                            arg1->tp = SpecifyArgType(sym, arg1->tp, nullptr, orig, origTemplate, origUsing);
                        }
                    }
                    else
                    {
                        auto arg1 = Allocate<Argument>();
                        *arg1 = *arg;
                        (*last)->v.func->arguments->push_back(arg1);
                        arg1->exp = SpecifyArgInt(sym, arg1->exp, orig, origTemplate, origUsing);
                        arg1->tp = LookupTypeFromExpression(arg1->exp, nullptr, false);
                        arg1->tp = SpecifyArgType(sym, arg1->tp, nullptr, orig, origTemplate, origUsing);
                    }
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
            TEMPLATEPARAMPAIR* found = TypeAliasSearch(exp->v.templateParam->first, false);
            if (found)
            {
                exp->v.templateParam->second = found->second;
            }
        }
    }
    return exp;
}
static Type* SpecifyArgType(SYMBOL* sym, Type* tp, TEMPLATEPARAM* tpt, std::list<TEMPLATEPARAMPAIR>* orig,
                            std::list<TEMPLATEPARAMPAIR>* origTemplate, std::list<TEMPLATEPARAMPAIR>* origUsing)
{
    if (!tp)
        return nullptr;
    auto rv = tp->CopyType(true);
    rv->UpdateRootTypes();
    tp = rv;
    while (tp->IsPtr() || tp->IsRef())
        tp = tp->BaseType()->btp;
    if (tp->IsDeferred())
    {
        std::list<TEMPLATEPARAMPAIR>* tpx = tp->BaseType()->templateArgs;
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
                            SearchAlias(args1->back().first, &args1->back(), sym, origTemplate, origUsing);
                        }
                        else if (args1->back().second->byClass.dflt)
                        {
                            if (args1->back().second->packed)
                            {
                                if (unpackingTemplate && !args1->back().second->resolved && !args1->back().second->ellipsis)
                                {
                                    auto tpl = LookupPackedInstance(args1->back());
                                    if (tpl)
                                    {
                                        *args1->back().second = *tpl;
                                    }
                                }
                                else if (args1->back().second->ellipsis)
                                {
                                    if (args1->back().second->byPack.pack)
                                    {
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
                            SearchAlias(args1->back().first, &args1->back(), sym, origTemplate, origUsing);
                        }
                    }
                }
            }
        }

        SYMBOL* sp1;
        if (tp->BaseType()->sp->tp->type == BasicType::typedef_)
            sp1 = GetTypeAliasSpecialization(tp->BaseType()->sp, args1);
        else
            sp1 = GetClassTemplate(tp->BaseType()->sp, args1, true);
        if (sp1)
        {
            rv = sp1->tp = sp1->tp->InitializeDeferred();
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
                    if (ita != itea && ita->second->byClass.val)
                    {
                        ita->second->byClass.dflt = ita->second->byClass.val;
                    }
                    ++ita;
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
        }
    }
    else if (tp->type == BasicType::typedef_ && tp->sp->templateParams)
    {
        tp->sp = CopySymbol(tp->sp);
        auto temp = tp->sp->templateParams;
        auto tpr = tp->sp->templateParams = templateParamPairListFactory.CreateList();
        auto tps = tp->sp->sb->mainsym ? tp->sp->sb->mainsym->templateParams : temp;
        auto ittps = tps->begin();
        for (auto&& tpl : *temp)
        {
            tpr->push_back(TEMPLATEPARAMPAIR{tpl.first, Allocate<TEMPLATEPARAM>()});
            *tpr->back().second = *tpl.second;
            if (tpr->back().second->packed)
            {
                if (unpackingTemplate && !tpr->back().second->ellipsis && !tpr->back().second->resolved)
                {
                    auto tpl = LookupPackedInstance(tpr->back());
                    if (tpl)
                    {
                        *tpr->back().second = *tpl;
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
                    auto t = TypeAliasSearch(tpr->back().first, true);
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
        return sp ? sp->tp : tp;
    }
    else if (tp->BaseType()->type == BasicType::templateparam_)
    {
        tp = tp->BaseType();
        TEMPLATEPARAMPAIR* rv = TypeAliasSearch(tp->templateParam->first, false);
        if (rv)
        {
            TEMPLATEPARAM* tpp = Allocate<TEMPLATEPARAM>();
            *tpp = *tp->templateParam->second;
            tp->templateParam->second = tpp;
            if (rv->second->packed && !tp->templateParam->second->ellipsis)
            {
                auto tpl = LookupPackedInstance(*rv);
                if (tpl)
                {
                    tp->templateParam->second->byClass.dflt = tpl->byClass.dflt;
                }
            }
            else
            {
                tp->templateParam->second->byClass.dflt = rv->second->byClass.dflt;
            }
        }
    }
    else if (tp->IsStructured())
    {
        if (tp->BaseType()->sp->sb->templateLevel && !tp->BaseType()->sp->sb->instantiated)
        {
            tp->BaseType()->sp = CopySymbol(tp->BaseType()->sp);
            std::list<TEMPLATEPARAMPAIR>* tpx = tp->BaseType()->sp->templateParams;
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
                                SearchAlias(args1->back().first, &args1->back(), sym, origTemplate, origUsing);
                                auto a1 = args1->back();
                                if (a1.second->packed && a1.second->byPack.pack && a1.second->byPack.pack->size() == 1 &&
                                    a1.second->byPack.pack->front().second->packed)
                                {
                                    a1.second->byPack.pack = a1.second->byPack.pack->front().second->byPack.pack;
                                }
                            }
                            else if (args1->back().second->byClass.dflt)
                            {
                                if (args1->back().second->packed)
                                {
                                    if (unpackingTemplate && !args1->back().second->ellipsis && !args1->back().second->resolved)
                                    {
                                        auto tpl = LookupPackedInstance(args1->back());
                                        if (tpl)
                                        {
                                            *args1->back().second = *tpl;
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
                                SearchAlias(args1->back().first, &args1->back(), sym, origTemplate, origUsing);
                            }
                        }
                    }
                }
            }
            SYMBOL* sp1 = GetClassTemplate(tp->BaseType()->sp, args1, true);
            if (sp1)
            {
                rv = sp1->tp = sp1->tp->InitializeDeferred();
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
    else if (tp->BaseType()->type == BasicType::templateselector_)
    {
        std::list<TEMPLATEPARAMPAIR>** tplp = nullptr;
        tp->BaseType()->sp = CopySymbol(tp->BaseType()->sp);
        auto oldx = tp->BaseType()->sp->sb->templateSelector;
        if (oldx)
        {
            auto rvs = tp->BaseType()->sp->sb->templateSelector = templateSelectorListFactory.CreateVector();
            bool first = true;
            for (auto&& old : *oldx)
            {
                rvs->push_back(old);
                if (old.isDeclType)
                {
                    first = false;
                    rvs->back().tp = old.tp->CopyType();
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
                            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(old.sp, false);
                            if (rv && rv->second->type == TplType::typename_)
                            {
                                Type* tp = rv->second->byClass.val ? rv->second->byClass.val : rv->second->byClass.dflt;
                                if (tp && tp->IsStructured())
                                    rvs->back().sp = tp->BaseType()->sp;
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
                                if (unpackingTemplate && !tpr->back().second->ellipsis && !tpr->back().second->resolved)
                                {
                                    auto tpl = LookupPackedInstance(tpr->back());
                                    if (tpl)
                                    {
                                        *tpr->back().second = *tpl;
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
                            else if (tpr->back().first && !tpr->back().second->byClass.val && !tpr->back().second->byClass.dflt)
                            {
                                SearchAlias(tpr->back().first, &tpr->back(), sym, origTemplate, origUsing);
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
    if (temp->second->ellipsis && temp->second->type != TplType::template_)
    {
        if (temp->second->type == TplType::typename_)
        {
            Type* tp1 = temp->second->packed ? (temp->second->byPack.pack && temp->second->byPack.pack->size()
                                                    ? temp->second->byPack.pack->front().second->byClass.dflt
                                                    : nullptr)
                                             : temp->second->byClass.dflt;
            if (tp1)
                GatherPackedTypes(&count, syms, tp1->BaseType());
        }
        else
        {
            EXPRESSION* exp1 = temp->second->packed ? (temp->second->byPack.pack && temp->second->byPack.pack->size()
                                                           ? temp->second->byPack.pack->front().second->byNonType.dflt
                                                           : nullptr)
                                                    : temp->second->byNonType.dflt;
            if (exp1)
                GatherPackedVars(&count, syms, exp1);
        }
        for (int i = 0; i < count; i++)
        {
            TEMPLATEPARAMPAIR* rv = TypeAliasSearch(syms[i], false);
            // only care about unbound ellipsis at this level
            if (rv && (!rv->second->ellipsis || rv->second->replaced))
            {
                if (rv && rv->second->packed && rv->second->byPack.pack)
                {
                    int n1 = CountPacks(rv->second->byPack.pack);
                    if (n1 > n)
                        n = n1;
                }
            }
        }
    }
    n--;
    if (n >= 0)
    {
        PushPackIndex();
    }
    void* hold[200];
    TEMPLATEPARAMPAIR* tpx = temp;
    if (tpx->second->packed && tpx->second->byPack.pack && tpx->second->byPack.pack->size())
        tpx = &tpx->second->byPack.pack->front();
    for (int i = count == 0 ? -1 : 0; i <= n; i++)
    {
        if (n >= 0)
            SetPackIndex(i);
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
                    while (rv->type == ExpressionNode::comma_ && rv->right)
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
                {
                    hold[i] = rv;
                }
                else
                {
                    if (rv && rv->type == BasicType::any_)
                        rv = nullptr;
                    tpx->second->byClass.dflt = rv;
                }
                break;
            }
            default:
                break;
        }
    }
    if (n >= 0)
    {
        PopPackIndex();
    }
    if (count != 0 && (n >= 0 || !temp->second->packed))
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
                    packList->back().second->byClass.dflt = (Type*)hold[i];
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
        std::list<TEMPLATEPARAMPAIR>::iterator tpl2 = origTemplate->begin(), tpl3;
        if (tpl2->second->type == TplType::new_)
            ++tpl2;
        for (tpl3 = args->begin(); tpl2 != origTemplate->end(); ++tpl2, tpl3 == args->end() ? tpl3 : ++tpl3)
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
            if (!templateDefinitionLevel &&
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
    for (auto ittemp = origUsing->begin(); ittemp != origUsing->end(); ++ittemp)
    {
        args1->push_back(TEMPLATEPARAMPAIR{ittemp->first, Allocate<TEMPLATEPARAM>()});
        *args1->back().second = *ittemp->second;
        if (!args1->back().second->byClass.dflt)
        {
            args1->back().second->byClass.dflt = args1->back().second->byClass.val;
        }
        if (args1->back().first && (!args1->back().second->byClass.dflt || args1->back().second->packed))
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
                        else
                        {
                            if (itargs2->second->packed)
                                args1->back().second->packed = true;
                            if (args1->back().second->packed)
                            {
                                if (itargs2->second->packed)
                                {
                                    args1->back().second->byPack.pack = itargs2->second->byPack.pack;
                                }
                                else
                                {
                                    if (!args1->back().second->byPack.pack)
                                        args1->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                    args1->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{ nullptr, itargs2->second });
                                }
                                auto it = ittemp;
                                ++it;
                                if (it == origUsing->end())
                                {
                                    ++itargs2;
                                    for (; itargs2 != args->end() && ittemp->second->type == itargs2->second->type; ++itargs2)
                                    {
                                        if (itargs2->second->packed)
                                        {
                                            if (itargs2->second->byPack.pack)
                                            {
                                                for (auto&& tpl : *itargs2->second->byPack.pack)
                                                {
                                                    if (!args1->back().second->byPack.pack)
                                                        args1->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                                    args1->back().second->byPack.pack->push_back(
                                                        TEMPLATEPARAMPAIR{ nullptr, tpl.second });
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if (!args1->back().second->byPack.pack)
                                                args1->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                            args1->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{ nullptr, itargs2->second });
                                        }
                                    }
                                }
                            }
                            else
                            {
                                args1->back().second->byClass.dflt =
                                    itargs2->second->byClass.dflt ? itargs2->second->byClass.dflt : itargs2->second->byClass.val;
                            }
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
    if (!IsDefiningTemplate())
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
static std::list<TEMPLATEPARAMPAIR>* TypeAliasAdjustArgs(std::list<TEMPLATEPARAMPAIR>* tpx, std::list<TEMPLATEPARAMPAIR>* args,
                                                         std::list<std::pair<SYMBOL*, SYMBOL*>>& newNames)
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
            if (ita->second->packed && (unpackingTemplate || itt->second->packed))
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
    {
        ++itt;
    }
    if (ita->second->type == TplType::new_)
    {
        ++ita;
    }
    for (; itt != tpx->end() && ita != args->end() && n; ++itt, ++ita, n--)
    {
        if (itt->first != ita->first)
        {
            newNames.push_back(std::pair<SYMBOL*, SYMBOL*>(itt->first, ita->first));
            ita->first = itt->first;
        }
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
SYMBOL* ParseLibcxxAliases(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (sp->name[0] == '_' && sp->name[1] == '_')
    {
        if (sp->name[2] == 't' && strcmp(sp->name, "__type_pack_element") == 0)
        {
            return TypePackElement(sp, args);
        }
        if (sp->name[2] == 'm' && strcmp(sp->name, "__make_integer_seq") == 0)
        {
            return MakeIntegerSeq(sp, args);
        }
#ifdef LIBCXX17
        if (sp->name[2] == 'd' && (strcmp(sp->name, "__decay") == 0 || strcmp(sp->name, "__decay_t") == 0))
        {
            return Decay(sp, args);
        }
        if (sp->name[2] == 'r' && strcmp(sp->name, "__remove_reference_t") == 0)
        {
            return RemoveReference(sp, args);
        }
        if (sp->name[2] == 'l' && strcmp(sp->name, "__libcpp_remove_reference_t") == 0)
        {
            return RemoveReference(sp, args);
        }
#endif
    }
    return nullptr;
}
SYMBOL* GetTypeAliasSpecialization(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    std::list<TEMPLATEPARAMPAIR> temp;
    std::list<std::pair<SYMBOL*, SYMBOL*>> newNames;
    if (!args)
    {
        args = &temp;
    }
    sp->tp->InstantiateDeferred();
    TemplateArgInstantiateDeferred(args);
    SYMBOL* rv;
    DotNetPELib::SHA1Context context;
    rv = LookupTemplateClass(context, sp, args);
    if (rv)
    {
        if (!rv->templateParams || allTemplateArgsSpecified(rv, rv->templateParams))
        {
            return rv;
        }
    }
    // if we get here we have a templated typedef
    if (args->size())
    {
        args = TypeAliasAdjustArgs(sp->templateParams, args, newNames);
        for (auto&& a : *args)
        {
            SpecifyOneArg(sp, &a, sp->templateParams, sp->sb->typeAlias);
        }
    }
    enclosingDeclarations.Mark();
    if (sp->sb->parentClass && sp->sb->parentClass->templateParams)
    {
        enclosingDeclarations.Add(sp->sb->parentClass->templateParams);
    }
    auto sp1 = ParseLibcxxAliases(sp, args);
    if (sp1)
    {
        enclosingDeclarations.Release();
        return sp1;
    }
    enclosingDeclarations.Add(args);
    Type* basetp = sp->tp->btp;
    while (basetp->IsPtr())
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
        if (!templateDefinitionLevel)
        {
            basetp = SpecifyArgType(basetp->sp, basetp, nullptr, nullptr, args, sp->sb->typeAlias);
            rv->tp = TemplateLookupTypeFromDeclType(basetp);
            if (!rv->tp)
                rv->tp = &stdany;
            else if (!strchr(rv->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
                RegisterTemplateClass(context, rv);
        }
    }
    else if (basetp->type == BasicType::templateselector_)
    {
        rv = CopySymbol(sp);
        rv->sb->mainsym = sp;
        if (!ParseTypeAliasDefaults(rv, args, sp->templateParams, sp->sb->typeAlias))
        {
            enclosingDeclarations.Release();
            return rv;
        }
        SpecifyTemplateSelector(&rv->sb->templateSelector, basetp->sp->sb->templateSelector, false, sp, sp->templateParams,
                                sp->sb->typeAlias);
        enclosingDeclarations.Release();
        if (!processingTemplateHeader)
        {
            Type tp1 = {};
            Type::MakeType(tp1, BasicType::templateselector_);
            tp1.sp = rv;
            if (!parsingSpecializationDeclaration)
                rv->tp = SynthesizeType(&tp1, args, false);
            if (rv->tp->IsStructured())
                rv = rv->tp->BaseType()->sp;
            basetp = sp->tp->btp;
            if (basetp->IsPtr())
            {
                rv = makeID(StorageClass::type_, rv->tp, nullptr, AnonymousName());
                Type *tpr = nullptr, **last = &tpr;
                while (basetp->IsPtr())
                {
                    *last = Allocate<Type>();
                    **last = *basetp;
                    basetp = basetp->btp;
                    last = &(*last)->btp;
                }
                *last = rv->tp;
                rv->tp = tpr;
            }
            SetLinkerNames(rv, Linkage::cdecl_);
            if (!strchr(rv->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
                RegisterTemplateClass(context, rv);
        }
        else
        {
            Type* tp1 = Type::MakeType(BasicType::templateselector_);
            tp1->sp = rv;
            rv->tp = tp1;
        }
        return rv;
    }
    else if (basetp->type == BasicType::typedef_ || (basetp->IsStructured() && basetp->BaseType()->sp->sb->templateLevel))
    {
        if (sp->sb->typeAlias)
        {
            std::list<TEMPLATEPARAMPAIR>* newParams = GetTypeAliasArgs(sp, args, sp->templateParams, sp->sb->typeAlias);
            if (basetp->type == BasicType::typedef_)
            {
                rv = GetTypeAliasSpecialization(basetp->sp, newParams);
            }
            else
            {
                rv = GetClassTemplate(basetp->BaseType()->sp, newParams, false);
            }
            if (rv && !strchr(rv->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
                RegisterTemplateClass(context, rv);
            if (rv && newNames.size())
            {
                for (auto&& t : *rv->templateParams)
                {
                    for (auto&& n : newNames)
                    {
                        if (t.first && !strcmp(t.first->name, n.first->name))
                        {
                            t.first = n.second;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            rv = CopySymbol(sp);
            rv->sb->mainsym = sp;
            rv->tp = sp->tp->BaseType();
        }
    }
    else
    {
        auto newval = sp->tp->btp;
        // if we get here we don't have any args for an alias template
        // so check to make sure at least that we can evaluate them...
        for (auto t : *args)
            if (t.second->packed)
            {
                if (t.second->byPack.pack)
                {
                    for (auto t1 : *t.second->byPack.pack)
                    {
                        if (!t1.second->byClass.dflt)
                        {
                            newval = &stdany;
                            break;
                        }
                    }
                }
            }
            else if (!t.second->byClass.dflt)
            {
                newval = &stdany;
                break;
            }
        rv = CopySymbol(sp);
        rv->sb->mainsym = sp;
        rv->tp = newval;
        Type** tp = &rv->tp;
        while ((*tp)->IsPtr() || (*tp)->IsRef())
        {
            *tp = (*tp)->CopyType();
            tp = &(*tp)->btp;
        }
        while (*tp != (*tp)->BaseType())
        {
            *tp = (*tp)->CopyType();
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
                        itorig->second->byClass.val = *tp = itarg->second->byClass.dflt;
                    }
                    break;
                }
            }
        }
        enclosingDeclarations.Release();
        return rv;
    }
    basetp = sp->tp->btp;
    if (basetp->IsPtr())
    {
        Type *tpr = nullptr, **last = &tpr;
        rv = makeID(StorageClass::type_, rv->tp, rv, AnonymousName());
        while (basetp->IsPtr())
        {
            *last = Allocate<Type>();
            **last = *basetp;
            basetp = basetp->btp;
            last = &(*last)->btp;
        }
        *last = rv->tp;
        rv->tp = tpr;
    }
    enclosingDeclarations.Release();
    return rv;
}

}  // namespace Parser