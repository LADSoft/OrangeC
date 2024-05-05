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
                        TYPE* tp = param.second->byClass.dflt;
                        while (ispointer(tp))
                            tp = basetype(tp)->btp;
                        tp = basetype(tp);
                        if (tp->type == BasicType::templateparam_)
                        {
                            tp->templateParam->second->byClass.val = nullptr;
                        }
                        else if (isstructured(tp) && (tp->sp)->sb->attribs.inheritable.linkage4 != Linkage::virtual_)
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
                            TYPE* tp = param.second->byClass.val;
                            if (tp)
                            {
                                while (ispointer(tp) || isref(tp))
                                    tp = basetype(tp)->btp;
                                if (isstructured(tp))
                                {
                                    PushPopValues(basetype(tp)->sp->templateParams, push);
                                }
                                else if (basetype(tp)->type == BasicType::templateselector_)
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
                case TplType::typename_: {
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
                    if (itTA->second->byNonType.val && to->second->byNonType.val &&
                        !equalTemplateIntNode(to->second->byNonType.val, *exp))
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
        if (ispointer(Ab))
            Ab = basetype(Ab)->btp;
        if (!isfunction(Ab))
            return false;
        if (basetype(Ab)->sp->sb->parentClass == nullptr || !ismember(basetype(Ab)->sp) || Pb->type != BasicType::memberptr_ ||
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
            if (A->type == BasicType::const_ && !isconst(Pb))
            {
                foundconst = true;
                *last = CopyType(A);
                last = &(*last)->btp;
                *last = nullptr;
            }
            else if (A->type == BasicType::volatile_ && !isvolatile(Pb))
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

bool DeduceTemplateParam(TEMPLATEPARAMPAIR* Pt, TYPE* P, TYPE* A, EXPRESSION* exp, bool change)
{
    if (Pt->second->type == TplType::typename_)
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
/*            if (TemplateConstExpr(A, exp) && !isconst(P))
                *tp = MakeType(BasicType::const_, A);
            else
            */
                *tp = A;
            Pt->second->deduced = true;
        }
        return true;
    }
    else if (Pt->second->type == TplType::template_ && isstructured(A) && basetype(A)->sp->sb->templateLevel)
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
                                          change))
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
            sp = change ? &Pt->second->byTemplate.orig->second->byTemplate.val
                        : &Pt->second->byTemplate.orig->second->byTemplate.temp;
            Pt->second->byTemplate.orig->second->deduced = true;
            *sp = basetype(A)->sp;
            return true;
        }
    }
    return false;
}
bool Deduce(TYPE* P, TYPE* A, EXPRESSION* exp, bool change, bool byClass, bool allowSelectors, bool baseClasses)
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
        if (isstructured(Pb) && Pb->sp->sb->templateLevel && Pb->sp->sb->attribs.inheritable.linkage4 != Linkage::virtual_ &&
            isstructured(Ab))
        {
            if (DeduceFromTemplates(P, Ab, change, byClass))
                return true;
            else
                return DeduceFromBaseTemplates(P, basetype(Ab)->sp, change, byClass);
        }
        if (Pb->type == BasicType::memberptr_)
        {
            return DeduceFromMemberPointer(P, Ab, change, byClass);
        }
        Ab = basetype(A);
        Pb = basetype(P);
        if (Pb->type == BasicType::enum_)
        {
            if (Ab->type == BasicType::enum_ && Ab->sp == Pb->sp)
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
        if (Ab->type != Pb->type && (!isfunction(Ab) || !isfunction(Pb)) && Pb->type != BasicType::templateparam_ &&
            (!allowSelectors || Pb->type != BasicType::templateselector_))
            // this next allows long and int to be considered the same, on architectures where there is no size difference
            if (!isint(Ab) || !isint(Pb) || basetype(Ab)->type == BasicType::bool_ || basetype(Pb)->type == BasicType::bool_ ||
                isunsigned(Ab) != isunsigned(Pb) || getSize(basetype(Ab)->type) != getSize(basetype(Pb)->type))
                return false;
        switch (Pb->type)
        {
            case BasicType::pointer_:
                if (isarray(Pb))
                {
                    if (!!basetype(Pb)->esize != !!basetype(Ab)->esize)
                        return false;
                    if (basetype(Pb)->esize && basetype(Pb)->esize->type == ExpressionNode::templateparam_)
                    {
                        SYMBOL* sym = basetype(Pb)->esize->v.sp;
                        if (sym->tp->type == BasicType::templateparam_)
                        {
                            sym->tp->templateParam->second->byNonType.val = basetype(Ab)->esize;
                        }
                    }
                }
                if (isarray(Pb) != isarray(Ab))
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
                if (islrqual(Pin) != islrqual(A) || isrrqual(Pin) != isrrqual(Ain))
                    return false;
                if (isconst(Pin) != isconst(Ain) || isvolatile(Pin) != isvolatile(Ain))
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
                return DeduceTemplateParam(Pb->templateParam, P, A, exp, change);
            case BasicType::struct_:
            case BasicType::union_:
            case BasicType::class_:
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
        if (basetype(A)->type != BasicType::lref_)
        {
            A = MakeType(BasicType::lref_, A);
        }
    }
    return A;
}
static TYPE* RemoveCVQuals(TYPE* A) { return basetype(A); }
static TYPE* rewriteNonRef(TYPE* A)
{
    if (isarray(A))
    {
        while (isarray(A))
            A = basetype(A)->btp;
        A = MakeType(BasicType::pointer_, A);
    }
    else if (isfunction(A))
    {
        A = MakeType(BasicType::pointer_, basetype(A));
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
        BasicType type = basetype(P)->type;
        P = basetype(P)->btp;
        if (type == BasicType::rref_)
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
        if (exp->type == ExpressionNode::func_)
        {
            if (exp->v.func->sp->sb->storage_class == StorageClass::overloads_)
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
static bool TemplateDeduceArgList(SymbolTable<SYMBOL>::iterator funcArgs, SymbolTable<SYMBOL>::iterator funcArgsEnd,
                                  SymbolTable<SYMBOL>::iterator templateArgs, SymbolTable<SYMBOL>::iterator templateArgsEnd,
                                  std::list<INITLIST*>::iterator its, std::list<INITLIST*>::iterator itse, bool allowSelectors,
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
            if ((*its)->nested && isstructured(sp->tp) && basetype(sp->tp)->sp->sb->templateLevel &&
                basetype(sp->tp)->sp->sb->initializer_list)
            {
                if (basetype(sp->tp)->sp->templateParams->size() > 1)
                {
                    auto ittpx = basetype(sp->tp)->sp->templateParams->begin();
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
    for (auto sym : *basetype(func->tp)->syms)
    {
        TYPE* tp = sym->tp;
        while (isref(tp) || ispointer(tp))
            tp = basetype(tp)->btp;
        if (isstructured(tp) && basetype(tp)->sp->templateParams && !basetype(tp)->sp->sb->instantiated &&
            !basetype(tp)->sp->sb->declaring)
        {
            ClearArgValues(basetype(tp)->sp->templateParams, basetype(tp)->sp->sb->specialized);
            if (basetype(tp)->sp->sb->specialized)
            {
                ClearArgValues(basetype(tp)->sp->templateParams->front().second->bySpecialization.types,
                               basetype(tp)->sp->sb->specialized);
            }
        }
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
                    if (itInitial->second->type != TplType::typename_ || itParams->second->type != TplType::template_)
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
                    if (it != iteParams || itParams->second->type != TplType::typename_ ||
                        itInitial->second->byClass.dflt->type != BasicType::void_)
                    {
                        if (!itParams->second->byPack.pack)
                            itParams->second->byPack.pack = templateParamPairListFactory.CreateList();
                        itParams->second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                        itParams->second->byPack.pack->back().second->type = itParams->second->type;
                        itParams->second->byPack.pack->back().second->byClass.val = itInitial->second->byClass.dflt;
                        if (itInitial->second->type == TplType::int_)
                            itParams->second->byPack.pack->back().second->byNonType.tp = itInitial->second->byNonType.tp;
                        itParams->second->byPack.pack->back().second->initialized = true;
                    }
                }
                else if (itInitial->second->type == TplType::typename_ && itParams->second->type == TplType::template_)
                {
                    TYPE* tp1 = itInitial->second->byClass.dflt;
                    while (tp1 && tp1->type != BasicType::typedef_ && tp1->btp)
                        tp1 = tp1->btp;
                    if (tp1->type != BasicType::typedef_ && !isstructured(tp1))
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
                    case TplType::typename_:
                        if (!templatecomparetypes(itInitial->second->byClass.dflt, itParams->second->byClass.dflt, true))
                            return nullptr;
                        break;
                    case TplType::template_:
                        if (!exactMatchOnTemplateParams(itInitial->second->byTemplate.dflt->templateParams,
                                                        itParams->second->byTemplate.dflt->templateParams))
                            return nullptr;
                        break;
                    case TplType::int_:
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
                            std::list<TEMPLATEPARAMPAIR>* special = params2->front().second->bySpecialization.types
                                                                        ? params2->front().second->bySpecialization.types
                                                                        : params2;
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
                    if (basetype(tp)->type == BasicType::rref_)
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
                if (base && base->second->type == TplType::typename_ && symArgs != itae)
                {
                    auto last = base->second->byPack.pack = templateParamPairListFactory.CreateList();
                    for (; symArgs != itae; ++symArgs)
                    {
                        last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});

                        last->back().second->type = TplType::typename_;
                        last->back().second->byClass.val = rewriteNonRef((*symArgs)->tp);
                        if (TemplateConstExpr(last->back().second->byClass.val, (*symArgs)->exp))
                            last->back().second->byClass.val = MakeType(BasicType::const_, last->back().second->byClass.val);
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
            bool rv = TemplateDeduceArgList(basetype(sym->tp)->syms->begin(), basetype(sym->tp)->syms->end(), templateArgs,
                                            templateArgsEnd, symArgs, itae, basetype(sym->tp)->type == BasicType::templateselector_,
                                            true);
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
                            if (special->front().second->type == TplType::typename_ && special->front().second->byClass.dflt &&
                                isfunction(special->front().second->byClass.dflt))
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
static bool TemplateDeduceFromType(TYPE* P, TYPE* A)
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
        if (basetype(origl)->type == BasicType::lref_)
        {
            if (basetype(origr)->type != BasicType::lref_)
                return -1;
            else
                return -1;  // originally checked n & m but since that's already checked just do this, pointing this out since
                            // it's GAURENTEED to return -1
            return 1;
        }
        else if (basetype(origr)->type == BasicType::lref_)
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
        if (l && r)
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
    UpdateRootTypes(rv->tp);
    rv->tp->sp = rv;
    rv->templateParams = templateParamPairListFactory.CreateList();
    rv->templateParams->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
    rv->templateParams->back().second->type = TplType::new_;
    rv->templateParams->back().second->bySpecialization.types = r;
    return rv;
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
                            case TplType::typename_:
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
                if (isstructured(sym->tp))
                    typetab[i] = SynthesizeTemplate(sym->tp, &allocedSyms[j], &allocedBodies[j], &allocedTypes[j])->tp;
                else
                    typetab[i] = SynthesizeType(sym->tp, nullptr, true);
                if (typetab[i]->type == BasicType::any_)
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

}  // namespace Parser