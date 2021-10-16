#pragma once
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
#include <stdio.h>
#include <string.h>
#include <float.h>
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
#include "symtab.h"
#include "expr.h"
#include "initbackend.h"
#include "declcons.h"
#include "stmt.h"
#include "lex.h"
#include "beinterf.h"

namespace Parser
{
static int functionnestingcount = 0;

struct ConstExprThisPtr
{
    EXPRESSION* oldval;
    EXPRESSION* newval;
};
struct ArgArray
{
    int size;
    EXPRESSION** data;
};
std::deque<ConstExprThisPtr> globalMap;

std::deque<std::deque<ConstExprThisPtr>> localMap;
std::stack<std::unordered_map<SYMBOL*, ArgArray>*> nestedMaps;
std::deque<EXPRESSION*> clearedInitializations;

static EXPRESSION* InstantiateStructure(EXPRESSION* thisptr, std::unordered_map<SYMBOL*, ArgArray>& argmap, EXPRESSION* ths);
static bool ExactExpression(EXPRESSION* exp1, EXPRESSION* exp2);
static EXPRESSION* LookupThis(EXPRESSION* exp, std::unordered_map<SYMBOL*, ArgArray> argmap);
bool hascshim(EXPRESSION* node);

void constexprinit()
{
    globalMap.clear();
    localMap.clear();
    while (!nestedMaps.empty())
        nestedMaps.pop();
    functionnestingcount = 0;
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
    if (node->type == en_thisref)
        node = node->left;
    if (node->type == en_func && node->v.func->sp)
    {
        if (node->v.func->sp->sb->constexpression && (node->v.func->sp->sb->inlineFunc.stmt || node->v.func->sp->sb->deferredCompile))
        {
            for (auto t = node->v.func->arguments; t; t= t->next)
                if (!IsConstantExpression(t->exp, true, true, true))
                    return false;
            return true;
        }
    }
    return false;
}
bool IsConstantExpression(EXPRESSION* node, bool allowParams, bool allowFunc, bool fromFunc)
{
    if (TotalErrors() && !fromFunc && (!allowFunc || (node->type != en_func && node->type != en_thisref)))  // in some error conditions nodes can get into a loop
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
        switch(exp->type)
        {
            case en_stmt:
            case en_atomic:
            case en_dot:
            case en_pointsto:
            case en_construct:
                return false;
            case en_auto:
                if (!exp->v.sp->sb->constexpression && (nestedMaps.empty() || nestedMaps.top()->find(exp->v.sp) == nestedMaps.top()->end()))
                    return false;
                break;
            case en_l_sp:
            case en_l_fp:
            case en_bits:
            case en_l_f:
            case en_l_d:
            case en_l_ld:
            case en_l_fi:
            case en_l_di:
            case en_l_ldi:
            case en_l_fc:
            case en_l_dc:
            case en_l_ldc:
            case en_l_c:
            case en_l_wc:
            case en_l_u16:
            case en_l_u32:
            case en_l_s:
            case en_l_ul:
            case en_l_l:
            case en_l_p:
            case en_l_ref:
            case en_l_i:
            case en_l_ui:
            case en_l_inative:
            case en_l_unative:
            case en_l_uc:
            case en_l_us:
            case en_l_bool:
            case en_l_bit:
            case en_l_ll:
            case en_l_ull:
            case en_l_string:
            case en_l_object:
                if (exp->left->type == en_auto)
                {
                    if (!(exp->left->v.sp->sb->constexpression || (allowParams && exp->left->v.sp->sb->storage_class == sc_parameter)))
                        return false;
                }
                else
                {
                    switch (exp->left->type)
                    {
                        case en_global:
                        case en_pc:
                        case en_labcon:
                        case en_absolute:
                        case en_threadlocal:
                            if (!exp->left->v.sp->sb->constexpression)
                                return false;
                            break;
                         default:
                            break;
                    }
                    stk.push(exp->left);
                }
                break;
            case en_void:
            case en_voidnz:
                stk.push(exp->left);
                if (exp->right->type == en_void || !fromFunc)
                {
                    stk.push(exp->right);
                }
                break;
            case en_func:
                if (!( !exp->v.func->ascall || (allowFunc && checkconstexprfunc(exp))))
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
        EXPRESSION* rv = nullptr, **last = &rv;
        for (auto c : clearedInitializations)
        {
            if (c)
            {
                *last = exprNode(en_void, c, nullptr);
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
        if (node->type == en_structadd)
            node = node->left;
        int ofs;
        EXPRESSION* rel = relptr(node, ofs);
        if (!rel || rel->type != en_global)
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
                std::unordered_map<SYMBOL*, ArgArray> argmap;
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
        if (node1->type == en_structadd)
        {
            std::unordered_map<SYMBOL*, ArgArray> argmap;
            auto node2 = LookupThis(node1->left, argmap);
            if (node2 && node2 != (EXPRESSION*)-1)
            {
                node2 = node2->v.constexprData[node1->right->v.i];
                if (node2)
                {
                    *node = node2;
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
    if (node->type == en_cshimref)
        return true;
    if (node->type == en_func)
    {
        auto t = node->v.func->arguments;
        while (t)
        {
            if (hascshim(t->exp))
                return true;
            t = t->next;
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
        if (exp1->type == en_add && exp1->right->type == en_c_i && exp1->right->v.i == 0)
            exp1 = exp1->left;
        if (exp2->type == en_add && exp2->right->type == en_c_i && exp2->right->v.i == 0)
            exp2 = exp2->left;
        if (exp1->type != exp2->type)
            return false;
        if (isintconst(exp1) || exp1->type == en_labcon)
            if (exp1->v.i != exp2->v.i)
                return false;
        switch (exp1->type)
        {
        case en_global:
        case en_pc:
        case en_const:
        case en_threadlocal:
        case en_auto:
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
static EXPRESSION* LookupThis(EXPRESSION* exp, std::unordered_map<SYMBOL*, ArgArray> argmap)
{
    if (!exp)
        return nullptr;
    while (exp->type == en_void)
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
    if (exp->type == en_auto)
    {
        for (auto t : argmap)
        {
            // expecting an initializer-list structure...
            if (exp->v.sp == t.first && t.second.data && t.second.size != 1)
            {
                EXPRESSION* exp1 = Allocate<EXPRESSION>();
                exp1->type = en_cshimthis;
                exp1->v.sp = nullptr;
                exp1->v.constexprData = t.second.data;
                localMap.back().push_back(ConstExprThisPtr{ exp, exp1 });
                return exp1;
            }
        }
    }
    return nullptr;
}
static EXPRESSION* EvaluateExpression(EXPRESSION* node, std::unordered_map<SYMBOL*, ArgArray>& argmap, EXPRESSION* ths, EXPRESSION* retblk, bool arg);
static EXPRESSION* ConstExprInitializeMembers(SYMBOL* sym, EXPRESSION* thisptr, INITLIST* args, std::unordered_map<SYMBOL*, ArgArray>& argmap)
{
    EXPRESSION* exp = Allocate<EXPRESSION>();
    exp->type = en_cshimthis;
    exp->v.sp = sym;
    exp->v.constexprData = Allocate<EXPRESSION*>(sym->sb->parentClass->tp->size);
    auto hr = sym->sb->parentClass->tp->syms->table[0];
    while (hr)
    {
        if (ismemberdata(hr->p) && hr->p->sb->init)
        {
            exp->v.constexprData[hr->p->sb->offset] = EvaluateExpression(hr->p->sb->init->exp, argmap, exp, nullptr, false);
        }
        hr = hr->next;
    }
    auto m = sym->sb->memberInitializers;
    while (m)
    {
        if (m->init)
        {
            SYMBOL* sp = search(m->name, sym->sb->parentClass->tp->syms);
            if (sp)
            {
                exp->v.constexprData[sp->sb->offset] = EvaluateExpression(m->init->exp, argmap, exp, nullptr, false);
            }
        }
        m = m->next;

    }
    if (localMap.size())
    {
        localMap.back().push_back(ConstExprThisPtr{ thisptr, exp });
    }
    else
    {
        globalMap.push_back(ConstExprThisPtr{ thisptr, exp });
    }
    return exp;
}
EXPRESSION* ConstExprRetBlock(SYMBOL* sym, EXPRESSION* node)
{
    EXPRESSION* exp = Allocate<EXPRESSION>();
    exp->type = en_cshimthis;
    exp->v.sp = sym;
    exp->v.constexprData = Allocate<EXPRESSION*>(sym->tp->size);
    if (localMap.size())
    {
        localMap.back().push_back(ConstExprThisPtr{ node, exp });
    }
    else
    {
        globalMap.push_back(ConstExprThisPtr{ node, exp });
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
        if ((*node1)->type == en_cshimref)
        {
            *node1 = (*node)->v.exp;
        }
        else if ((*node1)->type == en_void)
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
        if ((*node1)->type == en_void && (*node1)->left->type == en_cshimref)
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
static EXPRESSION* InstantiateStructure(EXPRESSION* thisptr, std::unordered_map<SYMBOL*, ArgArray>& argmap, EXPRESSION* ths)
{
    if (thisptr->type == en_auto && thisptr->v.sp->sb->stackblock)
    {
        thisptr = exprNode(en_substack, intNode(en_c_i, thisptr->v.sp->tp->size), nullptr);
    }

    EXPRESSION* varptr = anonymousVar(sc_auto, &stdpointer);
    varptr->v.sp->sb->constexpression = true;
    deref(&stdpointer, &varptr);

    EXPRESSION* rv = exprNode(en_assign, varptr, thisptr);
    EXPRESSION** last = &rv;
    auto hr = ths->v.sp->sb->parentClass->tp->syms->table[0];
    while (hr)
    {
        if (ismemberdata(hr->p))
        {
            EXPRESSION* next = exprNode(en_structadd, varptr, intNode(en_c_i, hr->p->sb->offset));
            deref(hr->p->tp, &next);
            next = exprNode(en_assign, next, EvaluateExpression(ths->v.constexprData[hr->p->sb->offset], argmap, ths, nullptr, false));
            if (next->right == nullptr || !IsConstantExpression(next->right, false, false))
                return nullptr;
            inConstantExpression++;
            optimize_for_constants(&next->right);
            inConstantExpression--;
            ReplaceShimref(&next->right);
            *last = exprNode(en_void, *last, next);
            last = &(*last)->right;
        }
        hr = hr->next;
    }
    if (thisptr->type == en_substack)
        *last = exprNode(en_void, *last, varptr);
    else
        *last = exprNode(en_void, *last, thisptr);
    return rv;
}
static void pushArray(EXPRESSION *exp, std::unordered_map<SYMBOL*, ArgArray>& argmap)
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
        if (node->type == en_auto || node->type == en_global)
        {
            if (isarray(node->v.sp->tp))
            {
                int n = node->v.sp->tp->size / node->v.sp->tp->btp->size;
                auto arr = Allocate<EXPRESSION*>(n+1);
                argmap[node->v.sp] = { n, arr };
                auto init = node->v.sp->sb->init;
                for (int i=0; i < n && init; i++, init = init->next)
                {
                    arr[i] = init->exp;
                }
            }
        }
    }
}
static void pushArray(SYMBOL* arg, INITLIST* il, std::unordered_map<SYMBOL*, ArgArray>& argmap)
{
    int n = 0;
    INITLIST* ilx = il;
    while (ilx)
    {
        n++, ilx = ilx->next;
    }
    auto tp = Allocate<TYPE>();
    tp->type = bt_pointer;
    tp->size = n * il->tp->size;
    tp->array = true;
    tp->btp = il->tp;
    auto sym = makeID(sc_global, tp, nullptr, AnonymousName());
    sym->sb->constexpression = true;
    auto listDeclarator = Allocate<EXPRESSION*>(getSize(bt_pointer) + getSize(bt_int));

    listDeclarator[0] = varNode(en_auto, sym);
    listDeclarator[getSize(bt_pointer)] = intNode(en_c_i, n);

    argmap[arg] = { getSize(bt_pointer) + getSize(bt_int), listDeclarator };

    INITIALIZER** p = &sym->sb->init;
    int offs = 0;
    tp = il->tp;
    EXPRESSION* exp = il->exp;
    while (il)
    {
        *p = Allocate<INITIALIZER>();
        (*p)->basetp = tp;
        (*p)->offset = offs;
        offs += tp->size;
        (*p)->exp = il->exp;
        if (il->exp->type != exp->type)
        {
            cast(tp, &(*p)->exp);
        }
        il = il->next;
        p = &(*p)->next;
    }
}
static EXPRESSION* HandleAssign(EXPRESSION* exp, std::unordered_map<SYMBOL*, ArgArray>& argmap, EXPRESSION* ths, EXPRESSION* retblk)
{
    EXPRESSION* rv = nullptr;
    if (exp->type == en_autoinc || exp->type == en_autodec)
    {
        rv = EvaluateExpression(exp->left, argmap, ths, retblk, false);
        EXPRESSION* inced = exprNode(exp->type == en_autoinc ? en_add : en_sub, rv, exp->right);
        EXPRESSION* exp1 = exp->left;
        while (lvalue(exp1->left))
            exp1 = exp1->left;
        if (ths && exp1->left->type == en_structadd && exp1->left->left->type == en_l_p && exp1->left->left->left->type == en_auto && exp1->left->left->left->v.sp->sb->thisPtr)
        {
            int i = exp1->left->right->v.i;
            ths->v.constexprData[i] = inced;
        }
        else if (ths && exp1->left->type == en_auto && exp1->left->v.sp->sb->thisPtr)
        {
            ths->v.constexprData[0] = inced;
        }
        else if (exp1->left && exp1->left->type == en_auto)
        {
            if (argmap.find(exp1->left->v.sp) == argmap.end())
                argmap[exp1->left->v.sp] = { 1, Allocate<EXPRESSION*>() };
            argmap[exp1->left->v.sp].data[0] = inced;
        }
    }
    else if (exp->type == en_assign)
    {
        rv = EvaluateExpression(exp->right, argmap, ths, retblk, false);
        optimize_for_constants(&rv);
        EXPRESSION* exp1 = exp->left;
        while (lvalue(exp1->left))
            exp1 = exp1->left;
        if (ths && exp1->left->type == en_structadd && exp1->left->left->type == en_l_p && exp1->left->left->left->type == en_auto && exp1->left->left->left->v.sp->sb->thisPtr)
        {
            int i = exp1->left->right->v.i;
            ths->v.constexprData[i] = rv;
        }
        else if (exp1->left->type == en_structadd && exp1->left->left->type == en_auto)
        {
            auto val = EvaluateExpression(exp1->left->right, argmap, ths, retblk, false);
            optimize_for_constants(&val);
            auto ths1 = LookupThis(exp1->left->left, argmap);
            if (ths1)
            {
                ths1->v.constexprData[val->v.i] = rv;
            }
        }
        else if (ths && exp1->left->type == en_auto && exp1->left->v.sp->sb->thisPtr)
        {
            ths->v.constexprData[0] = rv;
        }
        else if (exp1->left->type == en_add && exp1->left->left->type == en_auto)
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
        else if (exp1->left && exp1->left->type == en_auto)
        {
            if (argmap[exp1->left->v.sp].data == nullptr)
                argmap[exp1->left->v.sp] = { 1, Allocate<EXPRESSION*>() };
            argmap[exp1->left->v.sp].data[0] = rv;
        }
    }
    return rv;
}
static bool HandleLoad(EXPRESSION* exp, std::unordered_map<SYMBOL*, ArgArray>& argmap, EXPRESSION* ths, EXPRESSION* retblk, bool arg)
{
    bool rv = false;
    if (exp->left && lvalue(exp))
    {
        auto exp1 = exp;
        if (lvalue(exp1->left))
            exp1 = exp1->left;
        if (ths && exp1->left->type == en_structadd && exp1->left->left->type == en_l_p && exp1->left->left->left->type == en_auto && exp1->left->left->left->v.sp->sb->thisPtr)
        {
            int i = exp1->left->right->v.i;
            if (ths->v.constexprData[i])
            {
                exp1->left->type = en_cshimref;
                exp1->left->v.exp = ths->v.constexprData[i];
                exp1->left->left = nullptr;
                rv = true;
            }
        }
        else if (ths && exp1->left->type == en_auto && exp1->left->v.sp->sb->thisPtr)
        {
            if (ths->v.constexprData[0])
            {
                exp1->type = en_cshimref;
                exp1->v.exp = ths->v.constexprData[0];
                exp1->left = nullptr;
                rv = true;
            }
        }
        else if (exp1->left->type == en_add && exp1->left->left->type == en_auto)
        {
            auto val = EvaluateExpression(exp1->left->right, argmap, ths, retblk, false);
            optimize_for_constants(&val);
            if (isintconst(val) && val->v.i < exp1->left->left->v.sp->tp->size)
            {
                auto n = val->v.i / exp1->left->left->v.sp->tp->btp->size;
                auto xx = argmap[exp1->left->left->v.sp].data;
                if (xx)
                {
                    auto node1 = xx[n];
                    if (node1)
                    {
                        exp1->left->type = en_cshimref;
                        exp1->left->v.exp = node1;
                        exp1->left->left = nullptr;
                        rv = true;
                    }
                }
            }
        }
        else if (exp1->left && exp1->left->type == en_auto)
        {
            auto xx = argmap[exp1->left->v.sp].data;
            if (xx)
            {
                auto node1 = xx[0];
                if (node1)
                {
                    if (node1->type == en_void && node1->left->type == en_assign && node1->right->type == en_auto &&
                        node1->right->v.sp->sb->constexpression && IsConstantExpression(node1->left->right, true, true))
                    {
                        // an argument which has been made into a temp variable
                        // we have to shim it up so that we can handle the upcoming dereference
                        exp1->type = en_cshimref;
                        exp1->v.exp = node1->left->right;
                        exp1->left = nullptr;
                        optimize_for_constants(&exp1->v.exp);
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
    else if (exp->type == en_auto)
    {
        auto xx = argmap[exp->v.sp].data;
        if (xx && xx[0])
        {
            if (arg)
            {
                exp->type = en_cshimref;
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
    else if (exp->type == en_blockclear)
    {
        if (exp->left->type == en_auto)
        {
            int n;
            if (exp->left->v.sp->tp->array)
                n = exp->left->v.sp->tp->size / exp->left->v.sp->tp->btp->size;
            else
                n = exp->left->v.sp->tp->size;
            argmap[exp->left->v.sp] = { n, Allocate<EXPRESSION*>(n) };
            rv = true;
        }
    }
    else if (exp->type == en_func)
    {
        auto func = Allocate<FUNCTIONCALL>();
        EXPRESSION temp = *exp, * temp1 = &temp;
        *func = *exp->v.func;
        temp1->v.func = func;
        INITLIST* args = func->arguments;
        func->arguments = nullptr;
        INITLIST** list = &func->arguments;
        while (args)
        {
            *list = Allocate<INITLIST>();
            **list = *args;
            (*list)->exp = EvaluateExpression(args->exp, argmap, ths, retblk, true);
            if (!(*list)->exp)
                return false;
            optimize_for_constants(&(*list)->exp);
            while ((*list)->exp->type == en_void && (*list)->exp->right)
                (*list)->exp = (*list)->exp->right;
            list = &(*list)->next;
            args = args->next;
        }
        if (retblk && func->returnEXP && func->returnEXP->type == en_l_p && func->returnEXP->left->type == en_auto && func->returnEXP->left->v.sp->sb->retblk)
        {
            func->returnEXP = retblk;
        }
        if (retblk && func->thisptr && func->thisptr->type == en_l_p && func->thisptr->left->type == en_auto && func->thisptr->left->v.sp->sb->retblk)
        {
            func->thisptr = retblk;
        }
        if (func->thisptr)
        {
            int ofs;

            auto xx = relptr(func->thisptr, ofs);
            if (xx && xx->type == en_auto)
            {
                if (func->sp->sb->constexpression)
                    xx->v.sp->sb->constexpression = true;
            }
        }
        if (!strcmp(func->sp->name, "minmax"))
            printf("hi");
        optimize_for_constants(&temp1);
        if (temp1->type != en_func || (!hascshim(temp1) && temp1->v.sp != exp->v.sp))
        {
            *exp = *temp1;
            rv = true;
        }
    }
    else if (exp->type == en_cond)
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
static EXPRESSION* EvaluateExpression(EXPRESSION* node, std::unordered_map<SYMBOL*, ArgArray>& argmap, EXPRESSION* ths, EXPRESSION* retblk, bool arg)
{
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
                stk.push(root->right);
                stk.push(root);
                root = root->left;
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
    return rv;
}
static bool EvaluateStatements(EXPRESSION*& node, STATEMENT* stmt, std::unordered_map<SYMBOL*, ArgArray>& argmap, EXPRESSION* ths, EXPRESSION* retblk)
{
    std::unordered_map<int, STATEMENT*> labels;
    std::stack<STATEMENT*> stk;
    stk.push(stmt);
    while (!stk.empty())
    {
        auto p = stk.top();
        stk.pop();
        while (p)
        {
            switch (p->type)
            {
            case st_label:
                labels[p->label] = p;
                break;
            case st_block:
            case st___try:
            case st_try:
                stk.push(p->lower);
                break;
            }
            p = p->next;
        }
    }
    std::stack<STATEMENT*> blockList;
    blockList.push(stmt);
    while (!blockList.empty())
    {
        stmt = blockList.top();
        blockList.pop();
        while (stmt)
        {
            switch (stmt->type)
            {
            case st_line:
            case st_nop:
            case st_declare:
            case st_dbgblock:
            case st_label:
            case st_varstart:
            case st_catch:
            case st___catch:
            case st___finally:
            case st___fault:
                break;
            case st_throw:
            case st_asmgoto:
            case st_asmcond:
            case st__genword:
            case st_passthrough:
            case st_datapassthrough:
                return false;
            case st_select:
            case st_notselect:
            {
                if (Optimizer::cparams.prm_debug)
                    return false;
                auto node1 = EvaluateExpression(stmt->select, argmap, ths, retblk, false);
                optimize_for_constants(&node1);
                if (!IsConstantExpression(node1, false, false))
                    return false;
                if ((node1->v.i && stmt->type == st_notselect) || (!node1->v.i && stmt->type == st_select))
                    break;
                stmt = labels[stmt->label];
                if (!stmt)
                    return false;
                break;
            }
            case st_goto:
            case st_loopgoto:
                if (Optimizer::cparams.prm_debug)
                    return false;
                if (stmt->explicitGoto)
                    return false;
                stmt = labels[stmt->label];
                if (!stmt)
                    return false;
                break;
            case st_switch:
            {
                if (Optimizer::cparams.prm_debug)
                    return false;
                auto node1 = EvaluateExpression(stmt->select, argmap, ths, retblk, false);
                optimize_for_constants(&node1);
                if (!IsConstantExpression(node1, false, false))
                    return false;
                int label = stmt->label;
                auto c = stmt->cases;
                while (c)
                {
                    if (c->val == node1->v.i)
                    {
                        label = c->label;
                        break;
                    }
                    c = c->next;
                }
                stmt = labels[label];
                if (!stmt)
                    return false;
                break;
            }
            case st_block:
            case st___try:
            case st_try:
                blockList.push(stmt->next);
                stmt = stmt->lower;
                continue;
            case st_expr:
                if (stmt->select)
                {
                    if (Optimizer::cparams.prm_debug)
                        return false;
                    auto node1 = EvaluateExpression(stmt->select, argmap, ths, retblk, false);
                    optimize_for_constants(&node1);
                }
                break;
            case st_return:
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
                    else if (IsConstantExpression(node1, false, false) && node1->type != en_func && node1->type != en_funcret && node1->type != en_thisref)
                    {
                        *node = *node1;
                        node->noexprerr = true;
                        return true;
                    }
                    return false;
                }
                break;
            }
            stmt = stmt->next;
        }
    }
    return false;
}
bool EvaluateConstexprFunction(EXPRESSION*&node)
{
    if (!strcmp(node->v.func->sp->name, "$bequ"))
        printf("hi");
    if (node->v.func->sp->sb->isConstructor)
    {
        // we don't support constexpr constructors for classes with base classes right now...
        if (node->v.func->sp->sb->parentClass->sb->baseClasses)
            return false;
    }
    auto exp = node->v.func->thisptr;
    if (exp && exp->type == en_add)
        exp = exp->left;
    if (exp && exp->type == en_l_p)
    {
        int offset = 0;
        exp = relptr(exp->left, offset);
        if (exp && (exp->v.sp->sb->thisPtr || exp->v.sp->sb->retblk))
            return false;
    }
    if (exp && exp->type == en_auto)
    {
        if (inLoopOrConditional)
            return false;
        if (!exp->v.sp->sb->constexpression)
        {
            return false;
        }
    }
    bool rv = false;
    auto args = node->v.func->arguments;
    while (args)
    {
        if (args->nested)
        {
            auto args1 = args->nested;
            while (args1)
            {
                if (!IsConstantExpression(args1->exp, false, true, true))
                    break;
                args1 = args1->next;
            }
            if (args1)
                break;
        }
        else if (!IsConstantExpression(args->exp, false, true, true))
            break;
        args = args->next;
    }
    if (!args || (node->v.func->sp->sb->isConstructor && (matchesCopy(node->v.func->sp, false) || matchesCopy(node->v.func->sp, true))))
    {
        SYMBOL* found1 = node->v.func->sp;
        if (!node->v.func->sp->sb->inlineFunc.stmt && node->v.func->sp->sb->deferredCompile)
        {
            if (found1->sb->templateLevel && (found1->templateParams || found1->sb->isDestructor))
            {
                found1 = found1->sb->mainsym;
                if (found1->sb->castoperator)
                {
                    found1 = detemplate(found1, nullptr, basetype(node->v.func->thistp)->btp);
                }
                else
                {
                    found1 = detemplate(found1, node->v.func, nullptr);
                }
            }
            if (found1)
            {
                if (found1->sb->templateLevel && !templateNestingCount && node->v.func->templateParams)
                {
                    int pushCount = pushContext(found1, false);
                    found1 = TemplateFunctionInstantiate(found1, false, false);
                    while (pushCount--)
                        dropStructureDeclaration();
                }
                else
                {
                    if (found1->templateParams)
                        instantiatingTemplate++;
                    deferredCompileOne(found1);
                    if (found1->templateParams)
                        instantiatingTemplate--;
                }
            }
        }
        if (found1 && found1->sb->inlineFunc.stmt)
        {
            int i;
            STATEMENT* stmt = found1->sb->inlineFunc.stmt;
            while (stmt && stmt->type == st_expr)
                stmt = stmt->next;
            if (stmt && stmt->type == st_block && stmt->lower)
            {
                if (++functionnestingcount >= 1000)
                {
                    diag("EvaluateConstexprFunction: recursion level too high");
                }
                else
                {
                    std::unordered_map<SYMBOL*, ArgArray> argmap;
                    if (!nestedMaps.empty())
                    {
                        auto&& o = *nestedMaps.top();
                        for (auto s : o)
                        {
                      //      if (s.first->sb->storage_class != sc_parameter)
                            {
                                argmap[s.first] = s.second;
                            }
                        }
                    }
                    auto hr = basetype(found1->tp)->syms->table[0];
                    if (hr->p->sb->thisPtr)
                        hr = hr->next;
                    auto arglist = node->v.func->arguments;
                    while (hr && arglist)
                    {
                        argmap[hr->p] = { 1, Allocate<EXPRESSION*>() };
                        argmap[hr->p].data[0] = arglist->exp;
                        if (arglist->nested)
                            pushArray(hr->p, arglist->nested, argmap);
                        else
                            pushArray(arglist->exp, argmap);
                        hr = hr->next;
                        arglist = arglist->next;
                    }
                    nestedMaps.push(&argmap);
                    EXPRESSION* ths = nullptr;
                    if (found1->sb->isConstructor)
                        ths = ConstExprInitializeMembers(found1, node->v.func->thisptr, node->v.func->arguments, argmap);
                    else
                        ths = LookupThis(node->v.func->thisptr, argmap);
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
                            rv = EvaluateStatements(node, stmt->lower, argmap, ths, nullptr);
                    }

                    nestedMaps.pop();
                }
                --functionnestingcount;
            }
        }
    }
    return rv;
}
}  // namespace Parser