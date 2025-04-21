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

namespace Parser
{
void ClearArgValues(std::list<TEMPLATEPARAMPAIR>* params, bool specialized)
{
    if (params)
    {
        for (auto&& param : *params)
        {
            param.second->deduced = false;
            param.second->initialized = false;
            if (param.second->type != TplType::new_)
            {
                if (param.second->packed)
                    param.second->byPack.pack = nullptr;
                else
                    param.second->byClass.val = param.second->byClass.temp = nullptr;
                if (param.second->byClass.txtdflt && !specialized && !param.second->specializationParam)
                    param.second->byClass.dflt = nullptr;
                if (param.second->byClass.dflt)
                {
                    if (param.second->type == TplType::typename_)
                    {
                        Type* tp = param.second->byClass.dflt;
                        while (tp->IsPtr())
                            tp = tp->BaseType()->btp;
                        tp = tp->BaseType();
                        if (tp->type == BasicType::templateparam_)
                        {
                            tp->templateParam->second->byClass.val = nullptr;
                        }
                        else if (tp->IsStructured() && (tp->sp)->sb->attribs.inheritable.linkage4 != Linkage::virtual_)
                        {
                            ClearArgValues(tp->sp->templateParams, specialized);
                        }
                        else if (tp->type == BasicType::templateselector_)
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
            if (param.second->type != TplType::new_)
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
                        if (param.second->type == TplType::typename_)
                        {
                            Type* tp = param.second->byClass.val;
                            if (tp)
                            {
                                while (tp->IsPtr() || tp->IsRef())
                                    tp = tp->BaseType()->btp;
                                if (tp->IsStructured())
                                {
                                    PushPopValues(tp->BaseType()->sp->templateParams, push);
                                }
                                else if (tp->BaseType()->type == BasicType::templateselector_)
                                {
                                    PushPopValues((*tp->BaseType()->sp->sb->templateSelector)[1].templateParams, push);
                                }
                            }
                        }
                    }
                    else
                    {
                        param.second->byClass.val = (Type*)lst;
                    }
                }
            }
        }
    }
}
static bool DeduceFromTemplates(Type* P, Type* A, bool change, bool byClass)
{
    Type* pP = P->BaseType();
    Type* pA = A->BaseType();
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
                    if (itTP->second->packed)
                    {
                        if (itTP->second->byPack.pack)
                        {
                            if (itTP->second->byPack.pack->size() == itTA->second->byPack.pack->size())
                            {
                                auto packedP = itTP->second->byPack.pack->begin();
                                auto packedPe = itTP->second->byPack.pack->end();
                                auto packedA = itTA->second->byPack.pack->begin();
                                for (; packedP != packedPe; ++packedP, ++packedA)
                                {
                                    switch (packedP->second->type)
                                    {
                                        case TplType::typename_: {
                                            Type** tp = &packedP->second->byClass.val;
                                            if (*tp || !templateCompareTypes(packedP->second->byClass.val,
                                                                             packedA->second->byClass.val, true))
                                                return false;
                                            break;
                                        }
                                        case TplType::template_: {
                                            std::list<TEMPLATEPARAMPAIR>* paramT = packedP->first->templateParams;
                                            std::list<TEMPLATEPARAMPAIR>* paramA = packedA->first->templateParams;
                                            if (!paramT || !paramA || paramT->size() != paramA->size())
                                                return false;
                                            auto it1 = paramT->begin();
                                            auto it2 = paramA->begin();
                                            for (int i = 0; i < paramT->size(); ++i, ++it1, ++it2)
                                            {
                                                if (it1->second->type != it2->second->type)
                                                    return false;
                                            }
                                            break;
                                        }
                                        case TplType::int_: {
                                            EXPRESSION** exp;
                                            if (itTAo->second->bySpecialization.types)
                                            {
                                            }
                                            exp = &packedP->second->byNonType.val;
                                            if (packedA->second->byNonType.val && packedP->second->byNonType.val &&
                                                !equalTemplateMakeIntExpression(packedA->second->byNonType.val, *exp))
                                                return false;
                                            break;
                                        }
                                        default:
                                            break;
                                    }
                                }
                                ++itTA;
                                continue;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            auto packedP = itTP->second->byPack.pack = templateParamPairListFactory.CreateList();
                            auto packedA = itTA->second->byPack.pack->begin();
                            auto packedAe = itTA->second->byPack.pack->end();
                            for (; packedA != packedAe; ++packedA)
                            {
                                auto p = Allocate<TEMPLATEPARAM>();
                                *p = *packedA->second;
                                p->deduced = true;
                                packedP->push_back(TEMPLATEPARAMPAIR{itTA->first, p});
                            }
                            itTP->second->deduced = true;
                        }
                        ++itTA;
                        continue;
                    }
                    else
                    {
                        tas.push(itTA);
                        tas.push(itTAe);
                        itTAe = itTA->second->byPack.pack->end();
                        itTA = itTA->second->byPack.pack->begin();
                    }
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
                case TplType::typename_: {
                    Type** tp = change ? &to->second->byClass.val : &to->second->byClass.temp;
                    if (*tp)
                    {
                        if (!templateCompareTypes(*tp, itTA->second->byClass.val, true))
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
                case TplType::template_: {
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
                    else if (!DeduceFromTemplates(to->second->byTemplate.val->tp, itTA->second->byTemplate.val->tp, change,
                                                  byClass))
                        return false;
                    break;
                }
                case TplType::int_: {
                    EXPRESSION** exp;
                    if (itTAo->second->bySpecialization.types)
                    {
                    }
                    exp = change ? &to->second->byNonType.val : &to->second->byNonType.temp;
                    if (!itTA->second->byNonType.val)
                        return false;
                    if (itTA->second->byNonType.val && to->second->byNonType.val &&
                        !equalTemplateMakeIntExpression(itTA->second->byNonType.val, *exp))
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
                for (; itTP != itTPe && itTA != itTAe;)
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
                            case TplType::typename_: {
                                Type** tp = change ? &itTP->second->byClass.val : &itTP->second->byClass.temp;
                                if (*tp)
                                {
                                    if (!templateCompareTypes(*tp, itTA->second->byClass.val, true))
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
                            case TplType::template_: {
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
                            case TplType::int_: {
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
static bool DeduceFromBaseTemplates(Type* P, SYMBOL* A, bool change, bool byClass)
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
static bool DeduceFromMemberPointer(Type* P, Type* A, bool change, bool byClass)
{
    Type* Pb = P->BaseType();
    Type* Ab = A->BaseType();
    if (Ab->type == BasicType::memberptr_)
    {
        if (Pb->type != BasicType::memberptr_ || !Deduce(Pb->sp->tp, Ab->sp->tp, nullptr, change, byClass, false, false))
            return false;
        if (!Deduce(Pb->btp, Ab->btp, nullptr, change, byClass, false, false))
            return false;
        return true;
    }
    else  // should only get here for functions
    {
        if (Ab->IsPtr())
            Ab = Ab->BaseType()->btp;
        if (!Ab->IsFunction())
            return false;
        if (Ab->BaseType()->sp->sb->parentClass == nullptr || !ismember(Ab->BaseType()->sp) || Pb->type != BasicType::memberptr_ ||
            !Deduce(Pb->sp->tp, Ab->BaseType()->sp->sb->parentClass->tp, nullptr, change, byClass, false, false))
            return false;
        if (!Deduce(Pb->btp, Ab, nullptr, change, byClass, false, false))
            return false;
        return true;
    }
}
static Type* FixConsts(Type* P, Type* A)
{
    int pn = 0, an = 0;
    Type* Pb = P;
    Type *q = P, **last = &q;
    int i;
    while (q->IsPtr())
    {
        q = q->BaseType()->btp;
        pn++;
    }
    q = A;
    while (q->IsPtr())
    {
        q = q->BaseType()->btp;
        an++;
    }
    *last = nullptr;
    if (pn > an)
    {
        for (i = 0; i < pn - an; i++)
            P = P->BaseType()->btp;
    }
    while (P && A)
    {
        bool constant = false;
        bool vol = false;
        if (P->IsConst() && !A->IsConst())
            constant = true;
        if (P->IsVolatile() && !A->IsVolatile())
            vol = true;
        while (P->IsConst() || P->IsVolatile())
        {
            if ((constant && P->IsConst()) || (vol && P->IsVolatile()))
            {
                *last = P->CopyType();
                last = &(*last)->btp;
                *last = nullptr;
            }
            P = P->btp;
        }
        bool foundconst = false;
        bool foundvol = false;
        while (A != A->BaseType())
        {
            if (A->type == BasicType::const_ && !Pb->IsConst())
            {
                foundconst = true;
                *last = A->CopyType();
                last = &(*last)->btp;
                *last = nullptr;
            }
            else if (A->type == BasicType::volatile_ && !Pb->IsVolatile())
            {
                *last = A->CopyType();
                last = &(*last)->btp;
                *last = nullptr;
            }
            A = A->btp;
        }
        A = A->BaseType();
        *last = A->CopyType();
        last = &(*last)->btp;
        *last = nullptr;
        A = A->btp;
        P = P->btp;
    }
    (*last) = A;
    q->UpdateRootTypes();
    return q;
}
static bool TemplateConstExpr(Type* tp, EXPRESSION* exp)
{
    if (!tp->IsStructured())
        return false;
    if (exp)
    {
        while (exp->type == ExpressionNode::comma_ && exp->right)
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

bool DeduceTemplateParam(TEMPLATEPARAMPAIR* Pt, Type* P, Type* A, EXPRESSION* exp, bool byClass, bool change)
{
    if (Pt->second->type == TplType::typename_)
    {
        Type** tp = change ? &Pt->second->byClass.val : &Pt->second->byClass.temp;
        if (*tp)
        {
            if (/*!Pt->p->sb->initialized &&*/ !templateCompareTypes(*tp, A, true))
                return false;
            if (P->IsConst())
            {
                if ((*tp)->IsConst() || !A->IsConst())
                    return false;
            }
            if (P->IsVolatile())
            {
                if (!A->IsVolatile())
                    return false;
            }
        }
        else
        {
            if (P)
            {
                Type* q = A;
                // functions are never const...   this supports the methodology for 'std::is_function'
                if (byClass && A->IsFunction() && (P->IsConst() || P->IsVolatile()))
                    return false;
                while (q)
                {
                    if (q->IsConst())
                    {
                        *tp = FixConsts(P, A);
                        Pt->second->deduced = true;
                        return true;
                    }
                    q = q->BaseType()->btp;
                }
            }
            /*            if (TemplateConstExpr(A, exp) && !P->IsConst())
                            *tp = Type::MakeType(BasicType::const_, A);
                        else*
                        */
            *tp = A;
            Pt->second->deduced = true;
        }
        return true;
    }
    else if (Pt->second->type == TplType::template_ && A->IsStructured() && A->BaseType()->sp->sb->templateLevel)
    {
        SYMBOL* sp = A->BaseType()->sp;
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
            if (itMatch->second->type == TplType::new_)
                ++itMatch;
        }
        for (; itPrimary != itePrimary && itMatch != iteMatch; ++itPrimary, ++itMatch)
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
                    itPrimary->second->byPack.pack->insert(itPrimary->second->byPack.pack->begin(), itMatch, iteMatch);
                    itMatch = iteMatch;
                    ++itPrimary;
                    break;
                }
            }
            else if (!DeduceTemplateParam(&*itPrimary, itPrimary->second->byClass.val, itMatch->second->byClass.val, nullptr,
                                          byClass, change))
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
            *sp = A->BaseType()->sp;
            sp = change ? &Pt->second->byTemplate.orig->second->byTemplate.val
                        : &Pt->second->byTemplate.orig->second->byTemplate.temp;
            Pt->second->byTemplate.orig->second->deduced = true;
            *sp = A->BaseType()->sp;
            return true;
        }
    }
    return false;
}
bool Deduce(Type* P, Type* A, EXPRESSION* exp, bool change, bool byClass, bool allowSelectors, bool baseClasses)
{
    Type *Pin = P, *Ain = A;
    if (!P || !A)
        return false;
    while (1)
    {
        Type* Ab = A->BaseType();
        Type* Pb = P->BaseType();
        if (Ab->IsRef())
            Ab = Ab->btp->BaseType();
        if (P->type == BasicType::typedef_ && P->sp->templateParams && P->sp->templateParams->size() == 2)
        {
            if (P->sp->templateParams->back().second->type == TplType::typename_)
                return DeduceTemplateParam(&P->sp->templateParams->back(), P, A, exp, byClass, change);
        }
        if (Pb->IsStructured() && Pb->sp->sb->templateLevel && Pb->sp->sb->attribs.inheritable.linkage4 != Linkage::virtual_ &&
            Ab->IsStructured())
        {
            if (DeduceFromTemplates(P, Ab, change, byClass))
                return true;
            else
                return DeduceFromBaseTemplates(P, Ab->BaseType()->sp, change, byClass);
        }
        if (Pb->type == BasicType::memberptr_)
        {
            return DeduceFromMemberPointer(P, Ab, change, byClass);
        }
        Ab = A->BaseType();
        Pb = P->BaseType();
        if (Pb->type == BasicType::enum_)
        {
            if (Ab->type == BasicType::enum_ && Ab->sp == Pb->sp)
                return true;
            if (Pb->scoped)
            {
                auto tp = Ab;
                if (tp->IsInt())
                {
                    tp = tp->btp;
                }
                return tp && Pb->CompatibleType(tp);
            }
            if (Ab->IsInt())  // && Ab->enumConst)
                return true;
            return false;
        }
        if (Ab->IsFunction() && Pb->IsFunctionPtr())
        {
            Pb = Pb->BaseType()->btp;
            Pin = Pin->BaseType();  // assume any qualifiers went with an enclosing ref
            // can't have a const qualified non-member function anyway I think...
        }
        if (Ab->IsRef() && !Pb->IsRef())
            Ab = Ab->btp->BaseType();
        if (Ab->type != Pb->type && (!Ab->IsFunction() || !Pb->IsFunction()) && Pb->type != BasicType::templateparam_ &&
            (!allowSelectors || Pb->type != BasicType::templateselector_))
            // this next allows long and int to be considered the same, on architectures where there is no size difference
            if (!Ab->IsInt() || !Pb->IsInt() || Ab->BaseType()->type == BasicType::bool_ ||
                Pb->BaseType()->type == BasicType::bool_ || (Ab->IsUnsigned() != Pb->IsUnsigned() && (!exp || !isintconst(exp))) ||
                getSize(Ab->BaseType()->type) != getSize(Pb->BaseType()->type))
                return false;
        switch (Pb->type)
        {
            case BasicType::pointer_:
                if (Pb->IsArray())
                {
                    if (!!Pb->BaseType()->esize != !!Ab->BaseType()->esize)
                        return false;
                    if (Pb->BaseType()->esize && Pb->BaseType()->esize->type == ExpressionNode::templateparam_)
                    {
                        SYMBOL* sym = Pb->BaseType()->esize->v.sp;
                        if (sym->tp->type == BasicType::templateparam_)
                        {
                            sym->tp->templateParam->second->byNonType.val = Ab->BaseType()->esize;
                        }
                    }
                }
                if (Pb->IsArray() != Ab->IsArray())
                    return false;
                if (Pb->nullptrType != Ab->nullptrType)
                    return false;
                P = Pb->btp;
                A = Ab->btp;
                break;
            case BasicType::templateselector_:
                return true;
            case BasicType::templatedecltype_:
                return false;
            case BasicType::rref_:
            case BasicType::lref_:
            case BasicType::restrict_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::seg_:
                P = Pb->btp;
                A = Ab->btp;
                break;
            case BasicType::func_:
            case BasicType::ifunc_: {
                auto itp = Pb->syms->begin();
                auto itpend = Pb->syms->end();
                auto ita = Ab->syms->begin();
                auto itaend = Ab->syms->end();
                if (Pin->IsLRefQual() != A->IsLRefQual() || Pin->IsRRefQual() != Ain->IsRRefQual())
                    return false;
                if (Pin->IsConst() != Ain->IsConst() || Pin->IsVolatile() != Ain->IsVolatile())
                    return false;
                if (itp != itpend && (*itp)->sb->thisPtr)
                    ++itp;
                if (ita != itaend && (*ita)->sb->thisPtr)
                    ++ita;
                clearoutDeduction(P);
                if (Pb->btp->type != BasicType::auto_ &&
                    !Deduce(Pb->btp, Ab->btp, nullptr, change, byClass, allowSelectors, baseClasses))
                    return false;
                while (ita != itaend && itp != itpend)
                {
                    SYMBOL* sp = *itp;
                    if (!Deduce(sp->tp, (*ita)->tp, nullptr, change, byClass, allowSelectors, baseClasses))
                        return false;
                    if (sp->tp->type == BasicType::templateparam_)
                    {
                        if (sp->tp->templateParam->second->packed)
                        {
                            auto sra = Ab->syms->back();
                            auto srp = Pb->syms->back();
                            while ((*ita) != sra)
                                ++ita;
                            while ((*itp) != srp)
                                ++itp;
                            if ((sra->tp->type != BasicType::ellipse_ && srp->tp->type != BasicType::ellipse_) ||
                                sra->tp->type == srp->tp->type)
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
            case BasicType::templateparam_:
                return DeduceTemplateParam(Pb->templateParam, P, A, exp, byClass, change);
            case BasicType::struct_:
            case BasicType::union_:
            case BasicType::class_:
                if (templateCompareTypes(Pb, Ab, true))
                    return true;
                if (baseClasses && classRefCount(Pb->sp, Ab->sp) == 1)
                {
                    return true;
                }
                {
                    SYMBOL* cons = search(Pb->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
                    if (cons)
                    {
                        for (auto sp : *cons->tp->syms)
                        {
                            if (sp->tp->syms->size() == 2)
                            {
                                auto it = sp->tp->syms->begin();
                                ++it;
                                if ((*it)->tp->CompatibleType(Ab) || SameTemplate((*it)->tp, Ab))
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
static Type* GetForwardType(Type* P, Type* A, EXPRESSION* exp)
{
    bool lref = false;
    bool rref = false;
    GetRefs(P, A, exp, lref, rref);
    if (rref)
    {
        // rvalue to rref, remove reference...
        if (A->IsRef())
            A = A->BaseType()->btp;
    }
    else if (lref)
    {
        // IsLValue to rref, result is IsLValue&...
        if (A->BaseType()->type != BasicType::lref_)
        {
            A = Type::MakeType(BasicType::lref_, A);
        }
    }
    return A;
}
static Type* RemoveCVQuals(Type* A) { return A->BaseType(); }
static Type* rewriteNonRef(Type* A)
{
    if (A->IsArray())
    {
        while (A->IsArray())
            A = A->BaseType()->btp;
        A = Type::MakeType(BasicType::pointer_, A);
    }
    else if (A->IsFunction())
    {
        A = Type::MakeType(BasicType::pointer_, A->BaseType());
    }
    return A;
}
static bool TemplateDeduceFromArg(Type* orig, Type* sym, EXPRESSION* exp, bool allowSelectors, bool baseClasses)
{
    Type *P = orig, *A = sym;
    if (!P->IsRef())
    {
        A = rewriteNonRef(A);
        A = RemoveCVQuals(A);
    }
    P = RemoveCVQuals(P);
    if (P->IsRef())
    {
        BasicType type = P->BaseType()->type;
        P = P->BaseType()->btp;
        if (type == BasicType::rref_)
        {
            if (exp && !P->IsConst() && !P->IsVolatile())
            {
                // unadorned rref in arg, forwarding...
                A = GetForwardType(P, A, exp);
            }
            else if (A->IsRef())
                A = A->BaseType()->btp;
        }
        else
        {
            if (A->IsRef())
                A = A->BaseType()->btp;
        }
    }
    if (P->IsPtr() && (A->IsInt() || (A->IsPtr() && A->nullptrType)) && exp && isconstzero(A, exp))
    {
        // might get in here with a non-template argument that needs to be matched
        // usually the two types would just match and it would be fine
        // but in the case where we have pointer and a 'zero' constant
        // we need to act specially
        while (P->IsPtr())
            P = P->BaseType()->btp;
        if (P->IsVoid() || P->IsArithmetic() || P->IsFunction() ||
            (P->IsStructured() && (!P->BaseType()->sp->sb->templateLevel || P->BaseType()->sp->sb->instantiated)))
            return true;
        return false;
    }
    if (Deduce(P, A, exp, true, false, allowSelectors, baseClasses))
    {
        return true;
    }
    if (P->IsStructured() && !A->IsStructured())
    {
        // this is basical a poor man's way to do GetOverloadedFunction on a constructor,
        // with an arithmetic or pointer arg
        SYMBOL* cons = search(P->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
        if (cons)
        {
            for (auto sp1 : *cons->tp->BaseType()->syms)
            {
                if (sp1->tp->BaseType()->syms->front()->sb->thisPtr)
                {
                    if (sp1->tp->BaseType()->syms->size() == 2)
                    {
                        if (sp1->tp->BaseType()->syms->back()->tp->CompatibleType(A))
                            return true;
                    }
                    else if (sp1->tp->BaseType()->syms->size() > 2)
                    {
                        auto it = sp1->tp->BaseType()->syms->begin();
                        ++it;
                        ++it;
                        if ((*it)->sb->defaulted)
                        {
                            --it;
                            if ((*it)->tp->CompatibleType(A))
                                return true;
                        }
                    }
                }
            }
        }
    }
    if (exp && (P->IsFunctionPtr() || (P->IsRef() && P->BaseType()->btp->IsFunction())))
    {
        if (exp->type == ExpressionNode::callsite_)
        {
            if (exp->v.func->sp->sb->storage_class == StorageClass::overloads_)
            {
                SYMBOL* candidate = nullptr;
                for (auto sym : *exp->v.func->sp->tp->BaseType()->syms)
                {
                    if (sym->sb->templateLevel)
                        return false;
                }
                // no templates, we can try each function one at a time
                for (auto sym : *exp->v.func->sp->tp->BaseType()->syms)
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
void NormalizePacked(Type* tpo)
{
    Type* tp = tpo;
    while (tp->IsRef() || tp->IsPtr())
        tp = tp->BaseType()->btp;
    if (tp->BaseType()->templateParam)
        tpo->templateParam = tp->BaseType()->templateParam;
}
static bool TemplateDeduceArgList(SymbolTable<SYMBOL>::iterator funcArgs, SymbolTable<SYMBOL>::iterator funcArgsEnd,
                                  SymbolTable<SYMBOL>::iterator templateArgs, SymbolTable<SYMBOL>::iterator templateArgsEnd,
                                  std::list<Argument*>::iterator its, std::list<Argument*>::iterator itse, bool allowSelectors,
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
                for (; itparams != iteparams && its != itse; ++itparams, ++its)
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
            if ((*its)->nested && sp->tp->IsStructured() && sp->tp->BaseType()->sp->sb->templateLevel &&
                sp->tp->BaseType()->sp->sb->initializer_list)
            {
                if (sp->tp->BaseType()->sp->templateParams->size() > 1)
                {
                    auto ittpx = sp->tp->BaseType()->sp->templateParams->begin();
                    ++ittpx;
                    if (!TemplateDeduceFromArg(ittpx->first->tp, (*its)->nested->front()->tp, (*its)->nested->front()->exp,
                                               allowSelectors, baseClasses))
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
    return rv && (its == itse || ((*its)->tp && (*its)->tp->type == BasicType::templateparam_ &&
                                  (*its)->tp->templateParam->second->packed && !(*its)->tp->templateParam->second->byPack.pack));
}
void ScrubTemplateValues(SYMBOL* func)
{
    ClearArgValues(func->templateParams, func->sb->specialized);
    for (auto sym : *func->tp->BaseType()->syms)
    {
        Type* tp = sym->tp;
        while (tp->IsRef() || tp->IsPtr())
            tp = tp->BaseType()->btp;
        if (tp->type == BasicType::typedef_ && tp->sp->templateParams)
            ClearArgValues(tp->sp->templateParams, tp->sp->sb->specialized);
        if (tp->IsStructured() && tp->BaseType()->sp->templateParams && !tp->BaseType()->sp->sb->instantiated &&
            !tp->BaseType()->sp->sb->declaring)
        {
            ClearArgValues(tp->BaseType()->sp->templateParams, tp->BaseType()->sp->sb->specialized);
            if (tp->BaseType()->sp->sb->specialized)
            {
                ClearArgValues(tp->BaseType()->sp->templateParams->front().second->bySpecialization.types,
                               tp->BaseType()->sp->sb->specialized);
            }
        }
    }
    Type* retval = func->tp->BaseType()->btp->BaseType();
    if (retval->IsStructured() && retval->sp->templateParams && !retval->sp->sb->instantiated && !retval->sp->sb->declaring)
        ClearArgValues(retval->sp->templateParams, retval->sp->sb->specialized);
}
void PushPopTemplateArgs(SYMBOL* func, bool push)
{
    PushPopValues(func->templateParams, push);
    for (auto sym : *func->tp->BaseType()->syms)
    {
        Type* tp = sym->tp;
        while (tp->IsRef() || tp->IsPtr())
            tp = tp->BaseType()->btp;
        if (tp->IsStructured() && tp->BaseType()->sp->templateParams && !tp->BaseType()->sp->sb->declaring)
            PushPopValues(tp->BaseType()->sp->templateParams, push);
    }
    Type* retval = func->tp->BaseType()->btp->BaseType();
    if (retval->IsStructured() && retval->sp->templateParams && !retval->sp->sb->instantiated && !retval->sp->sb->declaring)
        PushPopValues(retval->sp->templateParams, push);
}
SYMBOL* TemplateDeduceArgsFromArgs(SYMBOL* sym, CallSite* args)
{
    std::list<TEMPLATEPARAMPAIR>* nparams = sym->templateParams;
    Type* thistp = args->thistp;
    std::list<Argument*>::iterator ita, itae;

    if (args->arguments)
    {
        if (args->arguments->size() == 1 && args->arguments->front()->nested)
        {
            auto itx = sym->tp->BaseType()->syms->begin();
            if ((*itx)->sb->thisPtr)
                ++itx;
            bool il = false;
            if ((*itx)->tp->IsStructured())
            {
                SYMBOL* sym1 = (*itx)->tp->BaseType()->sp;
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
    if (thistp && sym->sb->parentClass && !nparams)
    {
        Type* tp = thistp->BaseType()->btp->BaseType();
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
            for (; itSrc != iteSrc && itDest != iteDest; ++itSrc, ++itDest)
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
        auto templateArgs = sym->tp->BaseType()->syms->begin();
        auto templateArgsEnd = sym->tp->BaseType()->syms->end();
        auto symArgs = ita;
        std::list<TEMPLATEPARAMPAIR>* initial = args->templateParams;
        ScrubTemplateValues(sym);
        // fill in params that have been initialized in the arg list
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tis;
        if (initial && params)
        {
            if (!TemplateIntroduceArgs(params, initial))
                return nullptr;
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
                    case TplType::typename_:
                        if (!templateCompareTypes(itInitial->second->byClass.dflt, itParams->second->byClass.dflt, true))
                            return nullptr;
                        break;
                    case TplType::template_:
                        if (!exactMatchOnTemplateParams(itInitial->second->byTemplate.dflt->templateParams,
                                                        itParams->second->byTemplate.dflt->templateParams))
                            return nullptr;
                        break;
                    case TplType::int_:
                        if (!templateCompareTypes(itInitial->second->byNonType.tp, itParams->second->byNonType.tp, true) &&
                            (!itParams->second->byNonType.tp->IsPtr() ||
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
                Type* tpx = sp1->tp;
                while (tpx->IsRef())
                    tpx = tpx->BaseType()->btp;
                auto base = tpx->BaseType()->templateParam;
                if (!base || base->second->type != TplType::typename_)
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
            for (; templateArgs != sym->tp->BaseType()->syms->end() && symArgs != itae;)
            {
                SYMBOL* sp = *templateArgs;
                if (sp->packed)
                    break;
                if (itParams == iteParams || itParams->second->packed || !itParams->second->byClass.dflt)
                {
                    if (TemplateDeduceFromArg(sp->tp, (*symArgs)->tp, (*symArgs)->exp, false, false))
                    {
                        auto tp1 = sp->tp;
                        if (tp1->IsRef())
                        {
                            tp1 = tp1->BaseType()->btp;
                        }
                        if (tp1->IsStructured() && tp1->BaseType()->sp->templateParams &&
                            (!tp1->BaseType()->sp->sb->instantiated ||
                             tp1->BaseType()->sp->sb->attribs.inheritable.linkage4 != Linkage::virtual_))
                        {
                            std::list<TEMPLATEPARAMPAIR>* params2 = tp1->BaseType()->sp->templateParams;
                            std::list<TEMPLATEPARAMPAIR>* special = params2->front().second->bySpecialization.types
                                                                        ? params2->front().second->bySpecialization.types
                                                                        : params2;
                            for (auto it = itParams; it != iteParams; ++it)
                            {
                                std::list<TEMPLATEPARAMPAIR> xxparams(it, iteParams);
                                TransferClassTemplates(special, special, &xxparams);
                            }
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
                Type* tp = sp->tp;
                TEMPLATEPARAMPAIR* base;
                bool forward = false;
                if (tp->IsRef())
                {
                    if (tp->BaseType()->type == BasicType::rref_)
                    {
                        tp = tp->BaseType()->btp;
                        if (!tp->IsConst() && !tp->IsVolatile())
                            forward = true;
                    }
                    else
                    {
                        tp = tp->BaseType()->btp;
                    }
                }
                base = tp->BaseType()->templateParam;
                if (base && base->second->type == TplType::typename_ && symArgs != itae)
                {
                    auto last = base->second->byPack.pack = templateParamPairListFactory.CreateList();
                    for (; symArgs != itae; ++symArgs)
                    {
                        last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});

                        last->back().second->type = TplType::typename_;
                        if ((*symArgs)->tp->IsArray())
                            last->back().second->byClass.val = (*symArgs)->tp;
                        else
                            last->back().second->byClass.val = rewriteNonRef((*symArgs)->tp);
                        if (TemplateConstExpr(last->back().second->byClass.val, (*symArgs)->exp))
                            last->back().second->byClass.val = Type::MakeType(BasicType::const_, last->back().second->byClass.val);
                        if (forward && !definingTemplate)
                        {
                            last->back().second->byClass.val =
                                GetForwardType(nullptr, last->back().second->byClass.val, (*symArgs)->exp);
                            if (last->back().second->byClass.val->IsRef())
                            {
                                last->back().second->byClass.val->BaseType()->btp->BaseType()->rref = false;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            bool rv = TemplateDeduceArgList(sym->tp->BaseType()->syms->begin(), sym->tp->BaseType()->syms->end(), templateArgs,
                                            templateArgsEnd, symArgs, itae,
                                            sym->tp->BaseType()->type == BasicType::templateselector_, true);
            for (auto sp : *sym->tp->BaseType()->syms)
            {
                Type* tp = sp->tp;
                while (tp->IsRef() || tp->IsPtr())
                    tp = tp->BaseType()->btp;

                if (tp->IsStructured() && tp->BaseType()->sp->templateParams)
                {
                    std::list<TEMPLATEPARAMPAIR>* params = tp->BaseType()->sp->templateParams;
                    std::list<TEMPLATEPARAMPAIR>* special =
                        params->front().second->bySpecialization.types ? params->front().second->bySpecialization.types : params;
                    if (special)
                    {
                        std::list<TEMPLATEPARAMPAIR>* tpx = sym->templateParams->front().second->bySpecialization.types
                                                                ? sym->templateParams->front().second->bySpecialization.types
                                                                : sym->templateParams;
                        if (tpx)
                        {
                            if (special->front().second->type == TplType::typename_ && special->front().second->byClass.dflt &&
                                special->front().second->byClass.dflt->IsFunction())
                            {
                                TransferClassTemplates(special, special, tpx);
                            }
                            else
                            {
                                for (auto&& tplx : *tpx)
                                {
                                    if (tplx.second->type != TplType::new_)
                                    {
                                        std::list<TEMPLATEPARAMPAIR> a{tplx};
                                        TransferClassTemplates(special, special, &a);
                                    }
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
static bool TemplateDeduceFromType(Type* P, Type* A)
{
    if (P->type == BasicType::templatedecltype_)
        P = LookupTypeFromExpression(P->templateDeclType, nullptr, false);
    if (P)
        return Deduce(P, A, nullptr, true, false, false, false);
    return false;
}
SYMBOL* TemplateDeduceWithoutArgs(SYMBOL* sym)
{
    std::list<TEMPLATEPARAMPAIR>* nparams = sym->templateParams;
    SYMBOL* rv;

    for (auto s : *sym->tp->syms)
    {
        // look through parameters for something packed
        if (s->packed)
        {
            auto tp = s->tp;
            while (tp->IsPtr() || tp->IsRef())
                tp = tp->BaseType()->btp;
            tp = tp->BaseType();
            if (tp->type == BasicType::templateparam_)
            {
                if (tp->templateParam->first)
                {
                    for (auto tpl : *nparams)
                    {
                        if (tpl.first && tpl.second->packed && !strcmp(tpl.first->name, tp->templateParam->first->name))
                        {
                            tp->templateParam->second->byPack.pack = tpl.second->byPack.pack;
                            break;
                        }
                    }
                }
            }
            else if (tp->IsStructured() && tp->sp->templateParams)
            {
                TransferClassTemplates(nparams, nparams, tp->sp->templateParams);
            }
        }
    }

    if (TemplateParseDefaultArgs(sym, nullptr, nparams, nparams, nparams) &&
        (rv = ValidateArgsSpecified(sym->templateParams, sym, nullptr, nparams)))
    {
        return rv;
    }
    return nullptr;
}
static bool TemplateDeduceFromConversionType(Type* orig, Type* tp)
{
    Type *P = orig, *A = tp;
    if (P->IsRef())
        P = P->BaseType()->btp;
    if (!A->IsRef())
    {
        P = rewriteNonRef(P);
    }
    A = RemoveCVQuals(A);
    if (TemplateDeduceFromType(P, A))
        return true;
    return false;
}
SYMBOL* TemplateDeduceArgsFromType(SYMBOL* sym, Type* tp)
{
    std::list<TEMPLATEPARAMPAIR>* nparams = sym->templateParams;
    ClearArgValues(nparams, sym->sb->specialized);
    if (sym->sb->castoperator)
    {
        TemplateDeduceFromConversionType(sym->tp->BaseType()->btp, tp);
        return SynthesizeResult(sym, nparams);
    }
    else
    {
        auto templateArgs = tp->BaseType()->syms->begin();
        auto templateArgsEnd = tp->BaseType()->syms->end();
        auto symArgs = sym->tp->BaseType()->syms->begin();
        auto symArgsEnd = sym->tp->BaseType()->syms->end();
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
            Type* tp = sp->tp;
            TEMPLATEPARAMPAIR* base;
            if (tp->IsRef())
                tp = tp->BaseType()->btp;
            base = tp->templateParam;
            if (base->second->type == TplType::typename_)
            {
                auto last = base->second->byPack.pack = templateParamPairListFactory.CreateList();
                for (; symArgs != symArgsEnd; ++symArgs)
                {
                    last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                    last->back().second->type = TplType::typename_;
                    last->back().second->byClass.val = sp->tp;
                }
            }
        }
        TemplateDeduceFromType(sym->tp->BaseType()->btp, tp->BaseType()->btp);
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
int TemplatePartialDeduceFromType(Type* orig, Type* sym, bool byClass)
{
    Type *P = orig, *A = sym;
    int which = -1;
    if (P->IsRef())
        P = P->BaseType()->btp;
    if (A->IsRef())
        A = A->BaseType()->btp;
    if (orig->IsRef() && sym->IsRef())
    {
        bool p = false, a = false;
        if ((P->IsConst() && !A->IsConst()) || (P->IsVolatile() && !A->IsVolatile()))
            p = true;
        if ((A->IsConst() && !P->IsConst()) || (A->IsVolatile() && !P->IsVolatile()))
            a = true;
        if (a && !p)
            which = 1;
    }
    A = RemoveCVQuals(A);
    P = RemoveCVQuals(P);
    if (!Deduce(P, A, nullptr, true, byClass, false, false))
        return 0;
    if (P->SameType(A))
        return 0;
    return which;
}
int TemplatePartialDeduce(Type* origl, Type* origr, Type* syml, Type* symr, bool byClass)
{
    int n = TemplatePartialDeduceFromType(origl, symr, byClass);
    int m = TemplatePartialDeduceFromType(origr, syml, byClass);
    if (n && m)
    {
        if (origl->BaseType()->type == BasicType::lref_)
        {
            return -1;
        }
        else if (origr->BaseType()->type == BasicType::lref_)
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
    if (origl->IsRef())
        origl = origl->BaseType()->btp;
    if (origr->IsRef())
        origr = origr->BaseType()->btp;
    auto left = origl->BaseType()->templateParam;
    auto right = origr->BaseType()->templateParam;
    if (left && right)
    {
        if (left->second->packed && !right->second->packed)
            return -1;
        if (right->second->packed && !left->second->packed)
            return 1;
    }
    return 0;
}
int TemplatePartialDeduceArgsFromType(SYMBOL* syml, SYMBOL* symr, Type* tpx, Type* tpr, CallSite* fcall)
{
    int which = 0;
    int arr[200], n;
    ClearArgValues(syml->templateParams, syml->sb->specialized);
    ClearArgValues(symr->templateParams, symr->sb->specialized);
    if (syml->tp->IsStructured())
    {
        which = TemplatePartialDeduce(syml->tp, symr->tp, tpx, tpr, true);
    }
    else if (syml->sb->castoperator)
    {
        which = TemplatePartialDeduce(syml->tp->BaseType()->btp, symr->tp->BaseType()->btp, tpx->BaseType()->btp,
                                      tpr->BaseType()->btp, false);
    }
    else if (!syml->tp->IsFunction())
    {
        which = TemplatePartialDeduce(syml->tp, symr->tp, tpx, tpr, true);
    }
    else
    {
        int i;
        auto tArgsl = tpx->BaseType()->syms->begin();
        auto tArgsle = tpx->BaseType()->syms->end();
        auto sArgsl = syml->tp->BaseType()->syms->begin();
        auto sArgsle = syml->tp->BaseType()->syms->end();
        auto tArgsr = tpr->BaseType()->syms->begin();
        auto tArgsre = tpr->BaseType()->syms->end();
        auto sArgsr = symr->tp->BaseType()->syms->begin();
        auto sArgsre = symr->tp->BaseType()->syms->end();
        bool usingargs = fcall && fcall->ascall;
        auto ita = fcall && fcall->arguments ? fcall->arguments->begin() : std::list<Argument*>::iterator();
        auto itae = fcall && fcall->arguments ? fcall->arguments->end() : std::list<Argument*>::iterator();
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
            arr[n++] = TemplatePartialDeduce((*sArgsl)->tp, (*sArgsr)->tp, (*tArgsl)->tp, (*tArgsr)->tp, false);
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
        std::list<TEMPLATEPARAMPAIR>* l = syml->templateParams->front().second->bySpecialization.types
                                              ? syml->templateParams->front().second->bySpecialization.types
                                              : syml->templateParams;
        std::list<TEMPLATEPARAMPAIR>* r = symr->templateParams->front().second->bySpecialization.types
                                              ? symr->templateParams->front().second->bySpecialization.types
                                              : symr->templateParams;
        if (l && r && l->size() == r->size())
        {
            int i;
            n = 0;
            auto itl = l->begin();
            auto itel = l->end();
            if (itl->second->type == TplType::new_)
                ++itl;
            auto itr = r->begin();
            auto iter = r->end();
            if (itr->second->type == TplType::new_)
                ++itr;
            for (; itl != itel && itr != iter; ++itl, ++itr)
            {
                int l1 = itl->second->type == TplType::typename_ ? !!itl->second->byClass.val : 0;
                int r1 = itr->second->type == TplType::typename_ ? !!itr->second->byClass.val : 0;
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
static SYMBOL* SynthesizeTemplate(Type* tp, SYMBOL* rvt, sym::_symbody* rvs, Type* tpt)
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
    for (; itp != itpe; ++itp)
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
    rv->tp->UpdateRootTypes();
    rv->tp->sp = rv;
    rv->templateParams = templateParamPairListFactory.CreateList();
    rv->templateParams->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
    rv->templateParams->back().second->type = TplType::new_;
    rv->templateParams->back().second->bySpecialization.types = r;
    return rv;
}
void TemplatePartialOrdering(SYMBOL** table, int count, CallSite* funcparams, Type* atype, bool asClass, bool save)
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
        Type** typetab = (Type**)alloca(sizeof(Type*) * count);
        SYMBOL* allocedSyms = (SYMBOL*)alloca(sizeof(SYMBOL) * len);
        sym::_symbody* allocedBodies = (sym::_symbody*)alloca(sizeof(sym::_symbody) * len);
        Type* allocedTypes = (Type*)alloca(sizeof(Type) * len);
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
                            case TplType::typename_:
                                if (typechk)
                                {
                                    param.second->byClass.temp = (Type*)typechk->data;
                                    typechk = typechk->next;
                                }
                                else
                                {
                                    Optimizer::LIST* lst = Allocate<Optimizer::LIST>();
                                    Type* tp = param.first->tp->CopyType();
                                    param.second->byClass.temp = tp;
                                    lst->data = tp;
                                    lst->next = types;
                                    types = lst;
                                }
                                break;
                            case TplType::template_:
                                param.second->byTemplate.temp = param.first;
                                break;
                            case TplType::int_:
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (sym->tp->IsStructured())
                    typetab[i] = SynthesizeTemplate(sym->tp, &allocedSyms[j], &allocedBodies[j], &allocedTypes[j])->tp;
                else
                    typetab[i] = SynthesizeType(sym->tp, nullptr, true);
                if (typetab[i]->type == BasicType::any_)
                    table[i] = nullptr;
                j++;
            }
            else
            {
                typetab[i] = &stdany;
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
                                                                      asClass || !typetab[i]->BaseType()->sp->sb->parentTemplate
                                                                          ? typetab[i]
                                                                          : typetab[i]->BaseType()->sp->sb->parentTemplate->tp,
                                                                      asClass || !typetab[j]->BaseType()->sp->sb->parentTemplate
                                                                          ? typetab[j]
                                                                          : typetab[j]->BaseType()->sp->sb->parentTemplate->tp,
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
void CTADLookup(SYMBOL* funcsp, EXPRESSION** exp, Type** templateType, CallSite* funcparams, int flags)
{
    if (funcparams->arguments)
    {
        auto exp_in = *exp;
        bool toconst = (*templateType)->IsConst(), tovol = (*templateType)->IsConst();
        Type* thstp = Type::MakeType(BasicType::pointer_, (*templateType)->BaseType());
        if (toconst)
            thstp = Type::MakeType(BasicType::const_, thstp);
        if (tovol)
            thstp = Type::MakeType(BasicType::volatile_, thstp);
        funcparams->thistp = thstp;
        funcparams->ascall = true;
        Type* tp2 = (*templateType);
        SYMBOL* sym = ClassTemplateArgumentDeduction(&tp2, &(*exp), (*templateType)->BaseType()->sp, funcparams, flags);
        if (sym)
        {
            sym->tp->InitializeDeferred();
            *templateType = sym->tp;
            funcparams->thisptr = exp_in ? exp_in : exp_in = AnonymousVar(StorageClass::auto_, sym->tp);
            funcparams->sp = (*exp)->v.sp;
            funcparams->functp = (*exp)->v.sp->tp;
            funcparams->fcall = (*exp);
            int offset = 0;
            auto exp2 = relptr(exp_in, offset);
            if (exp2)
                sym = exp2->v.sp;
            (*exp) = MakeExpression(funcparams);
            (*exp) = MakeExpression(ExpressionNode::thisref_, (*exp));
            (*exp)->v.t.thisptr = funcparams->thisptr;
            (*exp)->v.t.tp = *templateType;
            if (sym)
                sym->sb->constexpression = true;
            optimize_for_constants(&(*exp));
            if (sym && (*exp)->type == ExpressionNode::thisref_ && !(*exp)->left->v.func->sp->sb->constexpression)
                sym->sb->constexpression = false;
            PromoteConstructorArgs(funcparams->sp, funcparams);
            // can't default destruct while deducing a template
        }
        else
        {
            errorstr(ERR_CANNOT_DEDUCE_TEMPLATE, (*templateType)->BaseType()->sp->name);
            EXPRESSION* exp2 = AnonymousVar(StorageClass::auto_, (*templateType)->BaseType());
        }
    }
    else
    {
        errorstr(ERR_CANNOT_DEDUCE_TEMPLATE, (*templateType)->BaseType()->sp->name);
        EXPRESSION* exp2 = AnonymousVar(StorageClass::auto_, (*templateType)->BaseType());
    }
}
}  // namespace Parser