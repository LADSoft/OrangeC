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

#include <cstdio>
#include <cstring>
#include <cfloat>
#include "compiler.h"
#include "ppPragma.h"
#include <stack>
#include <deque>
#include "config.h"
#include "ccerr.h"
#include "constexpr.h"
#include "template.h"
#include "cpplookup.h"
#include "constopt.h"
#include "declare.h"
#include "declcpp.h"
#include "help.h"
#include "memory.h"
#include "expr.h"
#include "initbackend.h"
#include "declcons.h"
#include "stmt.h"
#include "lex.h"
#include "beinterf.h"
#include "iexpr.h"
#include "floatconv.h"
#include "symtab.h"
#include "ListFactory.h"
#include "inline.h"

namespace Parser
{
static int functionNestingCount = 0;

struct ConstExprThisPtr
{
    EXPRESSION* oldval;
    EXPRESSION* newval;
};
std::deque<ConstExprThisPtr> globalMap;

std::deque<std::deque<ConstExprThisPtr>> localMap;
std::stack<std::unordered_map<SYMBOL*, ConstExprArgArray>*> nestedMaps;
std::deque<EXPRESSION*> clearedInitializations;

static EXPRESSION* InstantiateStructure(EXPRESSION* thisptr, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap,
                                        EXPRESSION* ths);
static bool ExactExpression(EXPRESSION* exp1, EXPRESSION* exp2);
static EXPRESSION* LookupThis(EXPRESSION* exp, const std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap);
bool hascshim(EXPRESSION* node);

void constexprinit()
{
    globalMap.clear();
    localMap.clear();
    while (!nestedMaps.empty())
        nestedMaps.pop();
    functionNestingCount = 0;
}
void constexprfunctioninit(bool start)
{
    if (start)
    {
        localMap.push_back(std::deque<ConstExprThisPtr>());
    }
    else
    {
        localMap.pop_back();
    }
}
bool checkconstexprfunc(EXPRESSION* node)
{
    if (node->type == ExpressionNode::thisref_)
        node = node->left;
    if (node->type == ExpressionNode::func_ && node->v.func->sp)
    {
        if (node->v.func->sp->sb->constexpression &&
            (node->v.func->sp->sb->inlineFunc.stmt || node->v.func->sp->sb->deferredCompile))
        {
            if (node->v.func->arguments)
                for (auto t : *node->v.func->arguments)
                    if (!IsConstantExpression(t->exp, true, true, true))
                        return false;
            return true;
        }
    }
    return false;
}
bool IsConstantExpression(EXPRESSION* node, bool allowParams, bool allowFunc, bool fromFunc)
{
    if (TotalErrors() && !fromFunc &&
        (!allowFunc || (node->type != ExpressionNode::func_ && node->type != ExpressionNode::thisref_)))  // in some error conditions nodes can get into a loop
        // for purposes of this function...  guard against it.   Consider everything
        // CONST to avoid more errors..
        return true;
    if (!node)
        return false;
    std::stack<EXPRESSION*> stk;
    stk.push(node);
    while (!stk.empty())
    {
        auto exp = stk.top();
        stk.pop();
        switch (exp->type)
        {
            case ExpressionNode::stmt_:
            case ExpressionNode::atomic_:
            case ExpressionNode::dot_:
            case ExpressionNode::pointsto_:
            case ExpressionNode::construct_:
                return false;
            case ExpressionNode::auto_:
                if (!exp->v.sp->sb->constexpression &&
                    (nestedMaps.empty() || nestedMaps.top()->find(exp->v.sp) == nestedMaps.top()->end()))
                    return false;
                break;
            case ExpressionNode::l_sp_:
            case ExpressionNode::l_fp_:
            case ExpressionNode::bits_:
            case ExpressionNode::l_f_:
            case ExpressionNode::l_d_:
            case ExpressionNode::l_ld_:
            case ExpressionNode::l_fi_:
            case ExpressionNode::l_di_:
            case ExpressionNode::l_ldi_:
            case ExpressionNode::l_fc_:
            case ExpressionNode::l_dc_:
            case ExpressionNode::l_ldc_:
            case ExpressionNode::l_c_:
            case ExpressionNode::l_wc_:
            case ExpressionNode::l_u16_:
            case ExpressionNode::l_u32_:
            case ExpressionNode::l_s_:
            case ExpressionNode::l_ul_:
            case ExpressionNode::l_l_:
            case ExpressionNode::l_p_:
            case ExpressionNode::l_ref_:
            case ExpressionNode::l_i_:
            case ExpressionNode::l_ui_:
            case ExpressionNode::l_inative_:
            case ExpressionNode::l_unative_:
            case ExpressionNode::l_uc_:
            case ExpressionNode::l_us_:
            case ExpressionNode::l_bool_:
            case ExpressionNode::l_bit_:
            case ExpressionNode::l_ll_:
            case ExpressionNode::l_ull_:
            case ExpressionNode::l_string_:
            case ExpressionNode::l_object_:
                if (exp->left->type == ExpressionNode::auto_)
                {
                    if (!(exp->left->v.sp->sb->constexpression ||
                          (allowParams && exp->left->v.sp->sb->storage_class == StorageClass::parameter_)))
                        return false;
                }
                else
                {
                    switch (exp->left->type)
                    {
                        case ExpressionNode::global_:
                        case ExpressionNode::pc_:
                        case ExpressionNode::labcon_:
                        case ExpressionNode::absolute_:
                        case ExpressionNode::threadlocal_:
                            if (!exp->left->v.sp->sb->constexpression)
                                return false;
                            break;
                        default:
                            break;
                    }
                    stk.push(exp->left);
                }
                break;
            case ExpressionNode::void_:
            case ExpressionNode::void_nz_:
                stk.push(exp->left);
                if (exp->right->type == ExpressionNode::void_ || !fromFunc)
                {
                    stk.push(exp->right);
                }
                break;
            case ExpressionNode::func_:
                if (!(!exp->v.func->ascall || (allowFunc && checkconstexprfunc(exp))))
                    return false;
                break;

            default:
                if (exp->left)
                    stk.push(exp->left);
                if (exp->right)
                    stk.push(exp->right);
                break;
        }
    }
    return true;
}
void ConstExprPatch(EXPRESSION** node)
{
    if (*node && !clearedInitializations.empty())
    {
        EXPRESSION *rv = nullptr, **last = &rv;
        for (auto c : clearedInitializations)
        {
            if (c)
            {
                *last = exprNode(ExpressionNode::void_, c, nullptr);
                last = &(*last)->right;
            }
        }
        *last = *node;
        *node = rv;
        clearedInitializations.clear();
    }
}
void ConstExprPromote(EXPRESSION* node, bool isconst)
{
    if (!templateNestingCount || instantiatingTemplate)
    {
        if (lvalue(node))
            node = node->left;
        if (node->type == ExpressionNode::structadd_)
            node = node->left;
        int ofs;
        EXPRESSION* rel = relptr(node, ofs);
        if (!rel || rel->type != ExpressionNode::global_)
        {
            EXPRESSION* found = nullptr;
            if (!localMap.empty())
                for (auto it = localMap.back().begin(); it != localMap.back().end(); ++it)
                    if (ExactExpression((*it).oldval, node))
                    {
                        found = (*it).newval;
                        if (!isconst)
                            localMap.back().erase(it);
                        break;
                    }
            if (!found)
                for (auto it = globalMap.begin(); it != globalMap.end(); ++it)
                    if (ExactExpression((*it).oldval, node))
                    {
                        found = (*it).newval;
                        if (!isconst)
                            globalMap.erase(it);
                        break;
                    }
            if (found)
            {
                std::unordered_map<SYMBOL*, ConstExprArgArray> argmap;
                clearedInitializations.push_back(InstantiateStructure(node, argmap, found));
            }
        }
    }
}
void ConstExprStructElemEval(EXPRESSION** node)
{
    if (lvalue(*node))
    {
        auto node1 = (*node)->left;
        if (node1->type == ExpressionNode::structadd_)
        {
            std::unordered_map<SYMBOL*, ConstExprArgArray> argmap;
            EXPRESSION *spe, *i;
            if (isintconst(node1->left))
            {
                spe = node1->right;
                i = node1->left;
            }
            else
            {
                spe = node1->left;
                i = node1->right;
            }
            if (spe->type == ExpressionNode::cshimthis_)
            {
                spe = spe->v.constexprData.data[i->v.i];
                if (spe)
                {
                    *node = spe;
                }
            }
            else
            {
                auto node2 = LookupThis(spe, argmap);
                if (node2 && node2 != (EXPRESSION*)-1)
                {
                    node2 = node2->v.constexprData.data[i->v.i];
                    if (node2)
                    {
                        *node = node2;
                    }
                }
                else if (spe->type == ExpressionNode::auto_ && spe->v.sp->sb->constexpression && spe->v.sp->sb->init)
                {
                    if (spe->v.sp->sb->init)
                    {
                        for (auto t : *spe->v.sp->sb->init)
                        {
                            if (t->offset == i->v.i)
                            {
                                *node = t->exp;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}
bool hascshim(EXPRESSION* node)
{
    if (!node)
        return false;
    if (hascshim(node->left) || hascshim(node->right))
        return true;
    if (node->type == ExpressionNode::cshimref_)
        return true;
    if (node->type == ExpressionNode::func_)
    {
        if (node->v.func->arguments)
            for (auto t : *node->v.func->arguments)
            {
                if (hascshim(t->exp))
                    return true;
            }
    }
    return false;
}
static bool ExactExpression(EXPRESSION* exp1, EXPRESSION* exp2)
{
    std::stack<EXPRESSION*> stk;
    stk.push(exp1);
    stk.push(exp2);
    while (!stk.empty())
    {
        exp2 = stk.top();
        stk.pop();
        exp1 = stk.top();
        stk.pop();
        if (exp1->type == ExpressionNode::add_ && exp1->right->type == ExpressionNode::c_i_ && exp1->right->v.i == 0)
            exp1 = exp1->left;
        if (exp2->type == ExpressionNode::add_ && exp2->right->type == ExpressionNode::c_i_ && exp2->right->v.i == 0)
            exp2 = exp2->left;
        if (exp1->type != exp2->type)
            return false;
        if (isintconst(exp1) || exp1->type == ExpressionNode::labcon_)
            if (exp1->v.i != exp2->v.i)
                return false;
        switch (exp1->type)
        {
            case ExpressionNode::global_:
            case ExpressionNode::pc_:
            case ExpressionNode::const_:
            case ExpressionNode::threadlocal_:
            case ExpressionNode::auto_:
                if (exp1->v.sp != exp2->v.sp)
                    return false;
                break;
        }
        if (exp1->left)
        {
            stk.push(exp1->left);
            stk.push(exp2->left);
        }
        if (exp1->right)
        {
            stk.push(exp1->right);
            stk.push(exp2->right);
        }
    }
    return true;
}
static EXPRESSION* LookupThis(EXPRESSION* exp, const std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap)
{
    if (!exp)
        return nullptr;
    while (exp->type == ExpressionNode::void_)
        exp = exp->right;
    if (localMap.size())
    {
        for (auto l : localMap.back())
        {
            if (ExactExpression(exp, l.oldval))
                return l.newval;
        }
    }
    for (auto g : globalMap)
    {
        if (ExactExpression(exp, g.oldval))
            return (EXPRESSION*)-1;
    }
    if (exp->type == ExpressionNode::auto_)
    {
        for (auto t : argmap)
        {
            // expecting an initializer-list structure...
            if (exp->v.sp == t.first && t.second.data && t.second.size != 1)
            {
                EXPRESSION* exp1 = Allocate<EXPRESSION>();
                exp1->type = ExpressionNode::cshimthis_;
                exp1->v.sp = nullptr;
                exp1->v.constexprData = t.second;
                if (nestedMaps.size() <= 1)
                    localMap.back().push_front(ConstExprThisPtr{exp, exp1});
                return exp1;
            }
        }
    }
    return nullptr;
}
static EXPRESSION* EvaluateExpression(EXPRESSION* node, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap, EXPRESSION* ths,
                                      EXPRESSION* retblk, bool arg);
static EXPRESSION* ConstExprInitializeMembers(SYMBOL* sym, EXPRESSION* thisptr, std::list<INITLIST*>& args,
                                              std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap)
{
    EXPRESSION* exp = Allocate<EXPRESSION>();
    exp->type = ExpressionNode::cshimthis_;
    exp->v.sp = sym;
    exp->v.constexprData = {sym->sb->parentClass->tp->size, Allocate<EXPRESSION*>(sym->sb->parentClass->tp->size)};
    for (auto sp : *sym->sb->parentClass->tp->syms)
    {
        if (ismemberdata(sp) && sp->sb->init)
        {
            exp->v.constexprData.data[sp->sb->offset] = EvaluateExpression(sp->sb->init->front()->exp, argmap, exp, nullptr, false);
        }
    }
    if (sym->sb->memberInitializers)
    {
        for (auto m : *sym->sb->memberInitializers)

            if (m->init)
            {
                SYMBOL* sp = search(sym->sb->parentClass->tp->syms, m->name);
                if (sp)
                {
                    if (m->init)
                        for (auto init : *m->init)
                            if (init->exp)
                            {
                                exp->v.constexprData.data[sp->sb->offset + init->offset] =
                                    EvaluateExpression(init->exp, argmap, nullptr, nullptr, false);
                            }
                }
            }
    }
    if (localMap.size())
    {
        localMap.back().push_front(ConstExprThisPtr{thisptr, exp});
    }
    else
    {
        globalMap.push_back(ConstExprThisPtr{thisptr, exp});
    }
    return exp;
}
EXPRESSION* ConstExprRetBlock(SYMBOL* sym, EXPRESSION* node)
{
    EXPRESSION* exp = Allocate<EXPRESSION>();
    exp->type = ExpressionNode::cshimthis_;
    exp->v.sp = sym;
    exp->v.constexprData = {sym->tp->size, Allocate<EXPRESSION*>(sym->tp->size)};
    if (localMap.size())
    {
        localMap.back().push_front(ConstExprThisPtr{node, exp});
    }
    else
    {
        globalMap.push_back(ConstExprThisPtr{node, exp});
    }
    return exp;
}
static void ReplaceShimref(EXPRESSION** node)
{
    std::stack<EXPRESSION**> stk;
    stk.push(node);
    while (!stk.empty())
    {
        EXPRESSION** node1 = stk.top();
        stk.pop();
        if ((*node1)->type == ExpressionNode::cshimref_)
        {
            *node1 = (*node)->v.exp;
        }
        else if ((*node1)->type == ExpressionNode::void_)
        {
            *node1 = (*node1)->right;
            stk.push(&(*node1)->right);
        }
    }

    stk.push(node);
    while (!stk.empty())
    {
        EXPRESSION** node1 = stk.top();
        stk.pop();
        if ((*node1)->type == ExpressionNode::void_ && (*node1)->left->type == ExpressionNode::cshimref_)
        {
            (*node1) = (*node1)->right;
            stk.push(node1);
        }
        else
        {
            if ((*node1)->left)
                stk.push(&(*node1)->left);
            if ((*node1)->right)
                stk.push(&(*node1)->right);
        }
    }
}
static EXPRESSION* InstantiateStructure(EXPRESSION* thisptr, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap,
                                        EXPRESSION* ths)
{
    if (thisptr->type == ExpressionNode::auto_ && thisptr->v.sp->sb->stackblock)
    {
        thisptr = exprNode(ExpressionNode::substack_, intNode(ExpressionNode::c_i_, thisptr->v.sp->tp->size), nullptr);
    }

    EXPRESSION* varptr = anonymousVar(StorageClass::auto_, &stdpointer);
    varptr->v.sp->sb->constexpression = true;
    deref(&stdpointer, &varptr);
    varptr->isStructAddress = true;

    EXPRESSION* rv = exprNode(ExpressionNode::assign_, varptr, thisptr);
    EXPRESSION** last = &rv;
    for (auto sp : *ths->v.sp->sb->parentClass->tp->syms)
    {
        if (ismemberdata(sp))
        {
            EXPRESSION* next = exprNode(ExpressionNode::structadd_, varptr, intNode(ExpressionNode::c_i_, sp->sb->offset));
            deref(sp->tp, &next);
            next = exprNode(ExpressionNode::assign_, next,
                            EvaluateExpression(ths->v.constexprData.data[sp->sb->offset], argmap, ths, nullptr, false));
            if (next->right == nullptr || !IsConstantExpression(next->right, false, false))
                return nullptr;
            inConstantExpression++;
            optimize_for_constants(&next->right);
            inConstantExpression--;
            ReplaceShimref(&next->right);
            *last = exprNode(ExpressionNode::void_, *last, next);
            last = &(*last)->right;
        }
    }
    if (thisptr->type == ExpressionNode::substack_)
        *last = exprNode(ExpressionNode::void_, *last, varptr);
    else
        *last = exprNode(ExpressionNode::void_, *last, thisptr);
    return rv;
}
static void pushArray(SYMBOL* arg, EXPRESSION* exp, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap)
{
    SYMBOL* finalsym = nullptr;
    if (exp->type == ExpressionNode::stackblock_ && exp->left->type == ExpressionNode::void_)
    {
        auto exp2 = exp->left;
        while (exp2->type == ExpressionNode::void_ && exp2->right)
            exp2 = exp2->right;
        if (exp2->type == ExpressionNode::auto_)
            finalsym = exp2->v.sp;
    }
    if (finalsym && isstructured(finalsym->tp) && basetype(finalsym->tp)->sp->sb->initializer_list && finalsym->sb->init)
    {
        int n = finalsym->sb->init ? finalsym->sb->init->size() : 0;
        auto it = finalsym->tp->sp->templateParams->begin();
        ++it;
        auto tp = MakeType(BasicType::pointer_, it->second->byClass.val);
        tp->size = n * tp->btp->size;
        tp->array = true;
        auto sym = makeID(StorageClass::global_, tp, nullptr, AnonymousName());
        sym->sb->constexpression = true;
        sym->sb->init = finalsym->sb->init;
        auto listDeclarator = Allocate<EXPRESSION*>(getSize(BasicType::pointer_) + getSize(BasicType::int_));

        listDeclarator[0] = varNode(ExpressionNode::auto_, sym);
        listDeclarator[getSize(BasicType::pointer_)] = intNode(ExpressionNode::c_i_, n);

        argmap[finalsym] = {getSize(BasicType::pointer_) + getSize(BasicType::int_), listDeclarator};
        if (arg)
            argmap[arg] = argmap[finalsym];
        if (n)
            for (auto t : *finalsym->sb->init)
            {
                t->basetp = tp->btp;
                cast(t->basetp, &t->exp);
                optimize_for_constants(&t->exp);
            }
    }
    else
    {
        if (exp->type == ExpressionNode::void_ && isintconst(exp->left))
        {
            TYPE* tp = basetype(arg->tp);
            if (isref(tp))
                tp = basetype(tp->btp);
            int n = tp->size;
            auto arr = Allocate<EXPRESSION*>(n + 1);
            argmap[arg] = {n, arr};
            auto it = tp->syms->begin();
            while (exp->right)
            {
                while (it != tp->syms->end() && !ismember((*it)))
                    ++it;
                if (isarray((*it)->tp))
                {
                    int ofs = basetype((*it)->tp)->btp->size;
                    n = basetype((*it)->tp)->size;
                    for (int i = 0; i < n && exp->right; i += ofs)
                    {
                        arr[i + (*it)->sb->offset] = exp->left;
                        exp = exp->right;
                    }
                }
                else
                {
                    arr[(*it)->sb->offset] = exp->left;
                    exp = exp->right;
                }
            }
        }
        else
        {
            std::stack<EXPRESSION*> stk;

            stk.push(exp);
            while (!stk.empty())
            {
                auto node = stk.top();
                stk.pop();
                if (node->right)
                    stk.push(node->right);
                if (node->left)
                    stk.push(node->left);
                if (node->type == ExpressionNode::auto_ || node->type == ExpressionNode::global_)
                {
                    if (isarray(node->v.sp->tp) && node->v.sp->sb->init)
                    {
                        int n = node->v.sp->sb->init->size();//node->v.sp->tp->size / node->v.sp->tp->btp->size;
                        auto arr = Allocate<EXPRESSION*>(n + 1);
                        argmap[node->v.sp] = {n, arr};
                        if (finalsym)
                            argmap[finalsym] = {n, arr};
                        if (n && node->v.sp->sb->init)
                        {
                            int i = 0;

                            for (auto init : *node->v.sp->sb->init)
                                arr[i++] = init->exp;
                        }
                    }
                }
            }
        }
    }
}
static void pushArray(SYMBOL* arg, std::list<INITLIST*>& il, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap)
{
    int n = il.size();
    auto tp = MakeType(BasicType::pointer_, il.front()->tp);
    tp->size = n * il.front()->tp->size;
    tp->array = true;
    auto sym = makeID(StorageClass::global_, tp, nullptr, AnonymousName());
    sym->sb->constexpression = true;
    auto listDeclarator = Allocate<EXPRESSION*>(getSize(BasicType::pointer_) + getSize(BasicType::int_));

    listDeclarator[0] = varNode(ExpressionNode::auto_, sym);
    listDeclarator[getSize(BasicType::pointer_)] = intNode(ExpressionNode::c_i_, n);

    argmap[arg] = {getSize(BasicType::pointer_) + getSize(BasicType::int_), listDeclarator};
    argmap[sym] = {n, Allocate<EXPRESSION*>(tp->size)};
    sym->sb->init = initListFactory.CreateList();
    int offs = 0;
    tp = il.front()->tp;
    EXPRESSION* exp = il.front()->exp;
    n = 0;
    for (auto xil : il)
    {
        auto p = Allocate<INITIALIZER>();
        p->basetp = tp;
        p->offset = offs;
        sym->sb->init->push_back(p);
            offs += xil->tp->size;
        p->exp = xil->exp;
        if (xil->exp->type != exp->type)
        {
            cast(tp, &p->exp);
        }
        argmap[sym].data[n++] = p->exp;
    }
}
static EXPRESSION* HandleAssign(EXPRESSION* exp, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap, EXPRESSION* ths,
                                EXPRESSION* retblk)
{
    EXPRESSION* rv = nullptr;
    if (exp->type == ExpressionNode::auto_inc_ || exp->type == ExpressionNode::auto_dec_)
    {
        rv = EvaluateExpression(exp->left, argmap, ths, retblk, false);
        EXPRESSION* inced = exprNode(exp->type == ExpressionNode::auto_inc_ ? ExpressionNode::add_ : ExpressionNode::sub_, rv, exp->right);
        EXPRESSION* exp1 = exp->left;
        while (lvalue(exp1->left))
            exp1 = exp1->left;
        if (ths && exp1->left->type == ExpressionNode::structadd_ && exp1->left->left->type == ExpressionNode::l_p_ &&
            exp1->left->left->left->type == ExpressionNode::auto_ && exp1->left->left->left->v.sp->sb->thisPtr)
        {
            int i = exp1->left->right->v.i;
            ths->v.constexprData.data[i] = inced;
        }
        else if (ths && exp1->left->type == ExpressionNode::auto_ && exp1->left->v.sp->sb->thisPtr)
        {
            ths->v.constexprData.data[0] = inced;
        }
        else if (exp1->left && exp1->left->type == ExpressionNode::auto_)
        {
            if (argmap.find(exp1->left->v.sp) == argmap.end())
                argmap[exp1->left->v.sp] = {1, Allocate<EXPRESSION*>()};
            argmap[exp1->left->v.sp].data[0] = inced;
        }
    }
    else if (exp->type == ExpressionNode::assign_)
    {
        if (!lvalue(exp->left))
            return nullptr;
        rv = EvaluateExpression(exp->right, argmap, ths, retblk, false);
        optimize_for_constants(&rv);
        EXPRESSION* exp1 = exp->left;
        while (lvalue(exp1->left))
            exp1 = exp1->left;
        if (ths && exp1->left->type == ExpressionNode::structadd_ && exp1->left->left->type == ExpressionNode::l_p_ &&
            exp1->left->left->left->type == ExpressionNode::auto_ && exp1->left->left->left->v.sp->sb->thisPtr)
        {
            int i = exp1->left->right->v.i;
            ths->v.constexprData.data[i] = rv;
        }
        else if (exp1->left->type == ExpressionNode::structadd_ && exp1->left->left->type == ExpressionNode::auto_)
        {
            auto val = EvaluateExpression(exp1->left->right, argmap, ths, retblk, false);
            optimize_for_constants(&val);
            auto ths1 = LookupThis(exp1->left->left, argmap);
            if (ths1)
            {
                ths1->v.constexprData.data[val->v.i] = rv;
            }
        }
        else if (ths && exp1->left->type == ExpressionNode::auto_ && exp1->left->v.sp->sb->thisPtr)
        {
            ths->v.constexprData.data[0] = rv;
        }
        else if (exp1->left->type == ExpressionNode::add_ && exp1->left->left->type == ExpressionNode::auto_)
        {
            auto val = EvaluateExpression(exp1->left->right, argmap, ths, retblk, false);
            optimize_for_constants(&val);
            if (isintconst(val) && isarray(exp1->left->left->v.sp->tp) && val->v.i < exp1->left->left->v.sp->tp->size)
            {
                auto n = val->v.i / exp1->left->left->v.sp->tp->btp->size;
                auto xx = argmap[exp1->left->left->v.sp].data;
                if (xx)
                {
                    xx[n] = rv;
                }
            }
        }
        else if (exp1->left && exp1->left->type == ExpressionNode::auto_)
        {
            if (argmap[exp1->left->v.sp].data == nullptr)
                argmap[exp1->left->v.sp] = {1, Allocate<EXPRESSION*>()};
            argmap[exp1->left->v.sp].data[0] = rv;
        }
    }
    return rv;
}
static bool HandleLoad(EXPRESSION* exp, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap, EXPRESSION* ths, EXPRESSION* retblk,
                       bool arg)
{
    bool rv = false;
    if (exp->left && lvalue(exp))
    {
        auto exp1 = exp;
        if (lvalue(exp1->left))
            exp1 = exp1->left;
        auto exp3 = exp1->left;
        while (castvalue(exp3))
            exp3 = exp3->left;
        if (exp3->type == ExpressionNode::auto_inc_ || exp3->type == ExpressionNode::auto_dec_)
            exp1->left = EvaluateExpression(exp1->left, argmap, ths, retblk, false);
        if (ths && exp3->type == ExpressionNode::structadd_ && exp3->left->type == ExpressionNode::l_p_ && exp3->left->left->type == ExpressionNode::auto_ &&
            exp3->left->left->v.sp->sb->thisPtr)
        {
            int i = exp3->right->v.i;
            if (ths->v.constexprData.data[i])
            {
                exp1->left->type = ExpressionNode::cshimref_;
                exp1->left->v.exp = ths->v.constexprData.data[i];
                exp1->left->left = nullptr;
                rv = true;
            }
        }
        else if (ths && exp3->type == ExpressionNode::auto_ && exp3->v.sp->sb->thisPtr)
        {
            if (exp1->type == ExpressionNode::l_p_)
            {
                exp1->type = ExpressionNode::cshimthis_;
                exp1->v.constexprData = ths->v.constexprData;
                exp1->left = nullptr;
                rv = true;
            }
            else if (ths->v.constexprData.data[0])
            {
                exp1->type = ExpressionNode::cshimref_;
                exp1->v.exp = ths->v.constexprData.data[0];
                exp1->left = nullptr;
                rv = true;
            }
        }
        else if (exp3->type == ExpressionNode::structadd_)
        {
            if (exp3->left->type == ExpressionNode::cshimthis_)
            {
                if (isintconst(exp3->right) && exp3->left->v.constexprData.data[exp3->right->v.i])
                {
                    *exp1 = *exp3->left->v.constexprData.data[exp3->right->v.i];
                    rv = true;
                }
            }
            else if (exp3->right->type == ExpressionNode::cshimthis_ && exp3->right->v.constexprData.data[exp3->left->v.i])
            {
                if (isintconst(exp3->left))
                {
                    *exp1 = *exp3->right->v.constexprData.data[exp3->left->v.i];
                    rv = true;
                }
            }
        }
        else if (exp3->type == ExpressionNode::add_)
        {
            auto exp2 = exp3->left;
            while (castvalue(exp2))
                exp2 = exp2->left;
            if (exp2->type == ExpressionNode::auto_)
            {
                auto val = EvaluateExpression(exp3->right, argmap, ths, retblk, false);
                optimize_for_constants(&val);
                if (isintconst(val) && val->v.i < exp2->v.sp->tp->size)
                {
                    auto n = val->v.i / exp2->v.sp->tp->btp->size;
                    auto xx = argmap[exp2->v.sp].data;
                    if (xx)
                    {
                        auto node1 = xx[n];
                        if (node1)
                        {
                            exp1->left->type = ExpressionNode::cshimref_;
                            exp1->left->v.exp = node1;
                            exp1->left->left = nullptr;
                            rv = true;
                        }
                    }
                }
            }
        }
        else if (exp3 && exp3->type == ExpressionNode::auto_)
        {
            if (exp1->type == ExpressionNode::l_ref_ && argmap[exp3->v.sp].size > 1)
            {
                exp1->type = ExpressionNode::cshimthis_;
                exp1->v.constexprData = argmap[exp3->v.sp];
                exp1->left = nullptr;
                rv = true;
            }
            else
            {
                auto xx = argmap[exp3->v.sp].data;
                if (xx)
                {
                    auto node1 = xx[0];
                    if (node1)
                    {
                        if (node1->type == ExpressionNode::void_ && node1->left->type == ExpressionNode::assign_ && node1->right->type == ExpressionNode::auto_ &&
                            node1->right->v.sp->sb->constexpression && IsConstantExpression(node1->left->right, true, true))
                        {
                            // an argument which has been made into a temp variable
                            // we have to shim it up so that we can handle the upcoming dereference
                            exp1->type = ExpressionNode::cshimref_;
                            exp1->v.exp = node1->left->right;
                            exp1->left = nullptr;
                            optimize_for_constants(&exp1->v.exp);
                        }
                        else if (IsConstantExpression(node1, true, true))
                        {
                            node1 = copy_expression(node1);
                            optimize_for_constants(&node1);
                            *exp1 = *node1;
                        }
                        else
                        {
                            *exp1 = *node1;
                        }
                        rv = true;
                    }
                }
            }
        }
    }
    else if (exp->type == ExpressionNode::auto_)
    {
        auto xx = argmap[exp->v.sp].data;
        if (xx && xx[0])
        {
            if (arg)
            {
                exp->type = ExpressionNode::cshimref_;
                exp->v.exp = xx[0];
                exp->left = nullptr;
            }
            else
            {
                *exp = *(xx[0]);
            }
            rv = true;
        }
    }
    else if (exp->type == ExpressionNode::blockclear_)
    {
        if (exp->left->type == ExpressionNode::auto_)
        {
            int n;
            if (exp->left->v.sp->tp->array)
                n = exp->left->v.sp->tp->size / exp->left->v.sp->tp->btp->size;
            else
                n = exp->left->v.sp->tp->size;
            argmap[exp->left->v.sp] = {n, Allocate<EXPRESSION*>(n)};
            rv = true;
        }
    }
    else if (exp->type == ExpressionNode::func_)
    {
        auto func = Allocate<FUNCTIONCALL>();
        EXPRESSION temp = *exp, * temp1 = &temp;
        *func = *exp->v.func;
        temp1->v.func = func;
        std::list<INITLIST*>* argslst = func->arguments;
        func->arguments = initListListFactory.CreateList();
        bool failed = false;
        if (argslst)
            for (auto args : *argslst)
            {
                auto lst = Allocate<INITLIST>();
                *lst = *args;
                func->arguments->push_back(lst);
                auto exp2 = args->exp;
                bool initlist = false;
                if (exp2->type == ExpressionNode::stackblock_)
                {
                    lst->exp = exp2->left;
                    initlist = true;
                }
                if (!initlist || exp2->left->type != ExpressionNode::auto_ || !isstructured(exp2->left->v.sp->tp) ||
                    !basetype(exp2->left->v.sp->tp)->sp->sb->initializer_list)
                {
                    lst->exp = EvaluateExpression(lst->exp, argmap, ths, retblk, true);
                    if (!lst->exp)
                        return false;
                    optimize_for_constants(&lst->exp);
                }
                if (lst->exp->type == ExpressionNode::thisref_ || lst->exp->type == ExpressionNode::func_)
                    failed = true;
                while (lst->exp->type == ExpressionNode::void_ && lst->exp->right)
                    lst->exp = lst->exp->right;
            }
        if (retblk && func->returnEXP && func->returnEXP->type == ExpressionNode::l_p_ && func->returnEXP->left->type == ExpressionNode::auto_ &&
            func->returnEXP->left->v.sp->sb->retblk)
        {
            func->returnEXP = retblk;
        }
        if (retblk && func->thisptr && func->thisptr->type == ExpressionNode::l_p_ && func->thisptr->left->type == ExpressionNode::auto_ &&
            func->thisptr->left->v.sp->sb->retblk)
        {
            func->thisptr = retblk;
        }
        if (func->thisptr)
        {
            int ofs;

            auto xx = relptr(func->thisptr, ofs);
            if (xx && xx->type == ExpressionNode::auto_)
            {
                if (func->sp->sb->constexpression)
                    xx->v.sp->sb->constexpression = true;
            }
        }
        if (!failed)
            optimize_for_constants(&temp1);
        if (temp1->type != ExpressionNode::func_ || (!hascshim(temp1) && temp1->v.func->sp != exp->v.func->sp))
        {
            *exp = *temp1;
            rv = true;
        }
    }
    else if (exp->type == ExpressionNode::cond_)
    {
        auto select = EvaluateExpression(exp->left, argmap, ths, retblk, false);
        if (select && isintconst(select))
        {
            if (select->v.i)
            {
                select = EvaluateExpression(exp->right->left, argmap, ths, retblk, false);
            }
            else
            {
                select = EvaluateExpression(exp->right->right, argmap, ths, retblk, false);
            }
            if (select && IsConstantExpression(select, false, false))
            {
                *exp = *select;
                rv = true;
            }
        }
    }
    return rv;
}
static EXPRESSION* EvaluateExpression(EXPRESSION* node, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap, EXPRESSION* ths,
                                      EXPRESSION* retblk, bool arg)
{
    if (node && node->type == ExpressionNode::select_)
        node = node->left;
    EXPRESSION* rv = copy_expression(node);
    std::stack<EXPRESSION*> stk;
    EXPRESSION* root = rv;
    do
    {
        while (root)
        {
            auto exp = HandleAssign(root, argmap, ths, retblk);
            if (exp)
            {
                *root = *exp;
                root = nullptr;
            }
            else if (HandleLoad(root, argmap, ths, retblk, arg))
            {
                root = nullptr;
            }
            else
            {
                if (root->type != ExpressionNode::func_)
                {
                    stk.push(root->right);
                    stk.push(root);
                    root = root->left;
                }
                else
                {
                    if (!stk.empty() && stk.top() && stk.top()->type == ExpressionNode::thisref_)
                    {
                        stk.pop();
                        stk.pop();
                    }
                    root = nullptr;
                }
            }
        }
        if (stk.empty())
            break;
        root = stk.top();
        stk.pop();
        if (root)
        {
            if (root->right && !stk.empty() && root->right == stk.top())
            {
                stk.pop();
                stk.push(root);
                root = root->right;
                auto exp = HandleAssign(root, argmap, ths, retblk);
                if (exp)
                {
                    *root = *exp;
                    root = nullptr;
                }
                else if (HandleLoad(root, argmap, ths, retblk, arg))
                {
                    root = nullptr;
                }
            }
            else
            {
                auto exp = HandleAssign(root, argmap, ths, retblk);
                if (exp)
                {
                    *root = *exp;
                    root = nullptr;
                }
                else if (HandleLoad(root, argmap, ths, retblk, arg))
                {
                    root = nullptr;
                }
                else
                {
                    root = nullptr;
                }
            }
        }
    } while (!stk.empty());
    if (rv && (rv->type == ExpressionNode::add_ && isintconst(rv->left)))
    {
        auto exp2 = rv->left;
        rv->left = rv->right;
        rv->right = exp2;
    }
    return rv;
}
static bool EvaluateStatements(EXPRESSION*& node, std::list<STATEMENT*>* stmt, std::unordered_map<SYMBOL*, ConstExprArgArray>& argmap,
                               EXPRESSION* ths, EXPRESSION* retblk)
{
    std::unordered_map<int, std::list<STATEMENT*>::iterator> labels;
    std::stack<std::list<STATEMENT*>*> stk;
    stk.push(stmt);
    while (!stk.empty())
    {
        auto s = stk.top();
        stk.pop();
        for (auto it = (*s).begin(); it != (*s).end(); ++it)
        {
            switch ((*it)->type)
            {
                case StatementNode::label_:
                    labels[(*it)->label] = it;
                    break;
                case StatementNode::block_:
                case StatementNode::seh_try_:
                case StatementNode::try_:
                    stk.push((*it)->lower);
                    break;
            }
        }
    }
    std::stack<std::pair<std::list<STATEMENT*>*, std::list<STATEMENT*>::iterator>> blockList;
    blockList.push(std::pair<std::list<STATEMENT*>*, std::list<STATEMENT*>::iterator>( stmt, stmt->begin()));
    while (!blockList.empty())
    {
        bool breakout = false;
        auto s = blockList.top();
        blockList.pop();
        for (auto it = s.second; it != s.first->end();++it)
        {
            auto stmt = *it;
            switch (stmt->type)
            {
                case StatementNode::line_:
                case StatementNode::nop_:
                case StatementNode::declare_:
                case StatementNode::dbgblock_:
                case StatementNode::label_:
                case StatementNode::varstart_:
                case StatementNode::catch_:
                case StatementNode::seh_catch_:
                case StatementNode::seh_finally_:
                case StatementNode::seh_fault_:
                    break;
                case StatementNode::throw_:
                case StatementNode::asmgoto_:
                case StatementNode::asmcond_:
                case StatementNode::genword_:
                case StatementNode::passthrough_:
                case StatementNode::datapassthrough_:
                    return false;
                case StatementNode::select_:
                case StatementNode::notselect_: {
                    if (Optimizer::cparams.prm_debug)
                        return false;
                    auto node1 = EvaluateExpression(stmt->select, argmap, ths, retblk, false);
                    optimize_for_constants(&node1);
                    if (!isarithmeticconst(node1))
                        return false;
                    if (!isintconst(node1))
                    {
                        node1->v.i = reint(node1->left);
                        node1->type = ExpressionNode::c_i_;
                        node1->left = nullptr;
                    }
                    if ((node1->v.i && stmt->type == StatementNode::notselect_) || (!node1->v.i && stmt->type == StatementNode::select_))
                        break;
                    it = labels[stmt->label];
//                    if (!stmt)
//                        return false;
                    break;
                }
                case StatementNode::goto_:
                case StatementNode::loopgoto_:
                    if (Optimizer::cparams.prm_debug)
                        return false;
                    if (stmt->explicitGoto || stmt->indirectGoto)
                        return false;
                    it = labels[stmt->label];
//                    if (!stmt)
//                        return false;
                    break;
                case StatementNode::switch_: {
                    if (Optimizer::cparams.prm_debug)
                        return false;
                    auto node1 = EvaluateExpression(stmt->select, argmap, ths, retblk, false);
                    optimize_for_constants(&node1);
                    if (!isintconst(node1))
                        return false;
                    int label = stmt->label;
                    for (auto c : *stmt->cases)
                    {
                        if (c->val == node1->v.i)
                        {
                            label = c->label;
                            break;
                        }
                    }
                    it = labels[label];
//                    if (!stmt)
//                        return false;
                    break;
                }
                case StatementNode::block_:
                case StatementNode::seh_try_:
                case StatementNode::try_:
                    // return value, next statement this block
                    blockList.push(std::pair<std::list<STATEMENT*>*, std::list<STATEMENT*>::iterator>(s.first, ++it));
                    // next program counter, next lower block
                    blockList.push(std::pair<std::list<STATEMENT*>*, std::list<STATEMENT*>::iterator>(stmt->lower, stmt->lower->begin()));
                    // break out of loop to activate
                    breakout = true;
                    break;
                case StatementNode::expr_:
                    if (stmt->select)
                    {
                        if (Optimizer::cparams.prm_debug)
                            return false;
                        auto node1 = EvaluateExpression(stmt->select, argmap, ths, retblk, false);
                        optimize_for_constants(&node1);
                    }
                    break;
                case StatementNode::return_:
                    if (stmt->select)
                    {
                        if (node->v.func->returnEXP)
                        {
                            //                        ConstExprRetBlock(node->v.func->sp, node->v.func->returnEXP);
                        }
                        auto node1 = EvaluateExpression(stmt->select, argmap, ths, node->v.func->returnEXP, false);
                        optimize_for_constants(&node1);

                        if (node->v.func->sp->sb->isConstructor)
                        {
                            auto newNode = InstantiateStructure(node->v.func->thisptr, argmap, ths);
                            if (newNode)
                            {
                                *node = *newNode;
                                return true;
                            }
                        }
                        else if (IsConstantExpression(node1, false, false) && node1->type != ExpressionNode::func_ && node1->type != ExpressionNode::funcret_ &&
                                 node1->type != ExpressionNode::thisref_)
                        {
                            *node = *node1;
                            node->noexprerr = true;
                            return true;
                        }
                        return false;
                    }
                    break;
            }
            if (breakout)
                break;
        }
    }
    return false;
}
bool EvaluateConstexprFunction(EXPRESSION*& node)
{
    if (node->v.func->sp->sb->isConstructor)
    {
        // we don't support constexpr constructors for classes with base classes right now...
        if (node->v.func->sp->sb->parentClass->sb->baseClasses && node->v.func->sp->sb->parentClass->sb->baseClasses->size())
            return false;
    }

    auto exp = node->v.func->thisptr;
    if (exp && exp->type == ExpressionNode::add_)
        exp = exp->left;
    if (exp && exp->type == ExpressionNode::l_p_)
    {
        int offset = 0;
        exp = relptr(exp->left, offset);
        if (exp && (exp->v.sp->sb->thisPtr || (!functionNestingCount && exp->v.sp->sb->retblk)))
        {
            return false;
        }
    }
    if (exp && exp->type == ExpressionNode::auto_)
    {
        if (inLoopOrConditional)
            return false;
        if (!exp->v.sp->sb->constexpression)
        {
            return false;
        }
    }
    if (exp && exp->type == ExpressionNode::global_)
        return false;
    bool rv = false;
    bool found = false;
    if (node->v.func->arguments)
        for (auto args : *node->v.func->arguments)
        {
            if (args->nested)
            {
                bool found1 = false;
                for (auto args1 : *args->nested)
                {
                    if (!IsConstantExpression(args1->exp, false, true, true))
                    {
                        found1 = true;
                        break;
                    }
                }
                if (found1)
                {
                    found = true;
                    break;
                }
            }
            else if (!IsConstantExpression(args->exp, false, true, true))
            {
                found = true;
                break;
            }
        }
    if (!found ||
        (node->v.func->sp->sb->isConstructor && (matchesCopy(node->v.func->sp, false) || matchesCopy(node->v.func->sp, true))))
    {
        SYMBOL* found1 = node->v.func->sp;
        if (isfunction(found1->tp))
        {
            if (!found1->sb->inlineFunc.stmt && found1->sb->deferredCompile)
            {
                if (found1->sb->templateLevel && (found1->templateParams || found1->sb->isDestructor))
                {
                    found1 = found1->sb->mainsym;
                    if (found1)
                    {
                        if (found1->sb->castoperator)
                        {
                            found1 = detemplate(found1, nullptr, basetype(node->v.func->thistp)->btp);
                        }
                        else
                        {
                            found1 = detemplate(found1, node->v.func, nullptr);
                        }
                    }
                }
                if (found1)
                {
                    int pushCount = pushContext(found1, false);
                    if (found1->sb->templateLevel && !templateNestingCount && node->v.func->templateParams)
                    {
                        found1 = TemplateFunctionInstantiate(found1, false);
                    }
                    CompileInline(found1, false);
                    while (pushCount--)
                        dropStructureDeclaration();
                }
            }
            if (found1 && found1->sb->inlineFunc.stmt)
            {
                int i;
                auto its = found1->sb->inlineFunc.stmt->begin();
                auto itse = found1->sb->inlineFunc.stmt->end();
                while (its != itse && (*its)->type == StatementNode::expr_)
                    ++its;
                if (its != itse && (*its)->type == StatementNode::block_ && (*its)->lower)
                {
                    if (++functionNestingCount >= 1000)
                    {
                        diag("EvaluateConstexprFunction: recursion level too high");
                    }
                    else
                    {
                        std::unordered_map<SYMBOL*, ConstExprArgArray> argmap;
                        std::unordered_map<SYMBOL*, ConstExprArgArray> tempmap;
                        if (!nestedMaps.empty())
                        {
                            auto&& o = *nestedMaps.top();
                            for (auto s : o)
                            {
                                //      if (s.first->sb->storage_class != StorageClass::parameter_)
                                {
                                    argmap[s.first] = s.second;
                                }
                            }
                        }
                        if (node->v.func->arguments)
                        {
                            auto it = basetype(found1->tp)->syms->begin();
                            auto ite = basetype(found1->tp)->syms->end();
                            if ((*it)->sb->thisPtr)
                                ++it;
                            auto arglist = node->v.func->arguments->begin();
                            auto arglistend = node->v.func->arguments->end();
                            while (it != ite && arglist != arglistend)
                            {
                                auto exp = (*arglist)->exp;
                                if (exp && exp->type == ExpressionNode::auto_ && isstructured(exp->v.sp->tp) &&
                                    basetype(exp->v.sp->tp)->sp->sb->initializer_list && argmap.find(exp->v.sp) != argmap.end())
                                {
                                    argmap[*it] = argmap[exp->v.sp];
                                }
                                else
                                {
                                    argmap[*it] = { 1, Allocate<EXPRESSION*>() };
                                    argmap[*it].data[0] = exp;
                                }

                                if ((*arglist)->nested)
                                    pushArray(*it, *(*arglist)->nested, tempmap);
                                else
                                    pushArray(*it, exp, tempmap);
                                ++it;
                                ++ arglist;
                            }
                        }
                        for (auto s : tempmap)
                        {
                            //      if (s.first->sb->storage_class != StorageClass::parameter_)
                            {
                                argmap[s.first] = s.second;
                                if (!nestedMaps.empty())
                                    (*nestedMaps.top())[s.first] = s.second;
                            }
                        }
                        nestedMaps.push(&argmap);
                        EXPRESSION* ths = nullptr;
                        if (found1->sb->isConstructor)
                        {
                            ths = ConstExprInitializeMembers(found1, node->v.func->thisptr, *node->v.func->arguments, argmap);
                        }
                        else if (node->v.func->thisptr)
                        {
                            ths = LookupThis(node->v.func->thisptr, argmap);
                            if (!ths)
                            {
                                if (node->v.func->thisptr->type == ExpressionNode::auto_ && node->v.func->thisptr->v.sp->sb->init)
                                {
                                    ths = Allocate<EXPRESSION>();
                                    ths->type = ExpressionNode::cshimthis_;
                                    ths->v.sp = found1;
                                    ths->v.constexprData = {found1->sb->parentClass->tp->size,
                                                            Allocate<EXPRESSION*>(found1->sb->parentClass->tp->size)};
                                    if (node->v.func->thisptr->v.sp->sb->init)
                                    {
                                        for (auto init : *node->v.func->thisptr->v.sp->sb->init)
                                        {
                                            if (init->exp)
                                            {
                                                ths->v.constexprData.data[init->offset] =
                                                    EvaluateExpression(init->exp, argmap, exp, nullptr, false);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (ths == (EXPRESSION*)-1)
                        {
                            // cant do constexpr on nonstatic global struct instances
                            // but don't know the difference between static and nonstatic here
                            // so we just don't do constexpr on global struct instances...
                            rv = false;
                        }
                        else
                        {

                            if (!ths || !getStructureDeclaration())
                                rv = EvaluateStatements(node, (*its)->lower, argmap, ths, nullptr);
                            if (rv && nestedMaps.size() == 1)
                            {
                                node = EvaluateExpression(node, argmap, ths, nullptr, false);
                                optimize_for_constants(&node);
                            }
                        }

                        nestedMaps.pop();
                    }
                    --functionNestingCount;
                }
            }
        }
    }
    return rv;
}
}  // namespace Parser