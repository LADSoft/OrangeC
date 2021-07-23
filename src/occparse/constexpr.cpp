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
namespace Parser
{
    static EXPRESSION* functionnesting[100];
    static int functionnestingcount = 0;

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
            case en_auto:
            case en_construct:
                return false;
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
EXPRESSION* CopyExpressionWithArgReplacement(EXPRESSION* node, std::unordered_map<SYMBOL*, EXPRESSION*>& argmap)
{
    EXPRESSION* rv = copy_expression(node);
    std::stack<EXPRESSION*> stk;
    stk.push(rv);
    while (!stk.empty())
    {
        auto exp = stk.top();
        stk.pop();
        if (exp->right)
            stk.push(exp->right);
        if (exp->left)
            stk.push(exp->left);
        if (exp->left && lvalue(exp))
        {
            while (lvalue(exp->left))
                exp = exp->left;
            if (exp->left && exp->left->type == en_auto)
            {     
                auto node1 = argmap[exp->left->v.sp];
                if (node1)
                {
                    if (node1->type == en_void && node1->left->type == en_assign && node1->right->type == en_auto && 
                        node1->right->v.sp->sb->constexpression && IsConstantExpression(node1->left->right, true, true))
                    {
                        stk.pop();
                        // an argument which has been made into a temp variable
                        // we have to shim it up so that we can handle the upcoming dereference
                        exp->type = en_cshimref;
                        exp->v.exp = node1->left->right;
                        exp->left = nullptr;
                        optimize_for_constants(&exp->v.exp);
                    }
                    else
                    {
                        *exp = *node1;
                    }
                }
            }
        }
        else if (exp->type == en_func)
        {
            auto func = Allocate<FUNCTIONCALL>();
            EXPRESSION temp = *exp, *temp1 = &temp;
            *func = *exp->v.func;
            temp1->v.func = func;
            INITLIST* args = func->arguments;
            func->arguments = nullptr;
            INITLIST** list = &func->arguments;
            while (args)
            {
                *list = Allocate<INITLIST>();
                **list = *args;
                (*list)->exp = CopyExpressionWithArgReplacement(args->exp, argmap);
                list = &(*list)->next;
                args = args->next;
            }
            if (!strcmp(func->sp->name, "forward"))
                if (hascshim(exp))
                    printf("hi");
            optimize_for_constants(&temp1);
            if (temp1->type != en_func || !hascshim(temp1))
            {
                 *exp = *temp1;
                 if (func->thisptr)
                     func->thisptr = CopyExpressionWithArgReplacement(func->thisptr, argmap);
            }
            else 
            {
                diag("CopyExpressionWithArgReplacement: unreplaced function %s", func->sp->name);
            }
        }
    }
    return rv;
}
bool EvaluateConstexprFunction(EXPRESSION*&node)
{

    bool rv = false;
    auto args = node->v.func->arguments;
    while (args)
    {
        if (!IsConstantExpression(args->exp, false, true, true))
            break;
        args = args->next;
    }
    if (!args)
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
                STATEMENT* st = stmt->lower;
                while (st->type == st_varstart)
                    st = st->next;
                if (st->type == st_block && !st->next)
                {
                    st = st->lower;
                    while (st->type == st_line || st->type == st_dbgblock || st->type == st_label)
                        st = st->next;
                    if (st->type == st_expr || st->type == st_return)
                    {
                        if (st->select)
                        {
                            for (i = 0; i < functionnestingcount; i++)
                                if (functionnesting[i] == st->select)
                                    break;
                            if (i >= functionnestingcount)
                            {
                                functionnesting[functionnestingcount++] = st->select;
                                std::unordered_map<SYMBOL*, EXPRESSION*> argmap;
                                auto hr = basetype(found1->tp)->syms->table[0];
                                if (hr->p->sb->thisPtr)
                                    hr = hr->next;
                                auto arglist = node->v.func->arguments;
                                while (hr && arglist)
                                {
                                    argmap[hr->p] = arglist->exp;
                                    hr = hr->next;
                                    arglist = arglist->next;
                                }
                                if (hascshim(node))
                                    printf("hi");
                                if (node->type == en_func && !strcmp(node->v.func->sp->name, "max"))
                                    printf("hi");
                                auto node1 = CopyExpressionWithArgReplacement(st->select, argmap);
                                optimize_for_constants(&node1);
                                functionnestingcount--;
                                if (IsConstantExpression(node1, false, false) && node1->type != en_func && node1->type != en_funcret && node1->type != en_thisref)
                                {
                                    if (hascshim(node1))
                                        printf("hi");
                                    *node = *node1;
                                    node->noexprerr = true;
                                    rv = true;
                                }
                                if (hascshim(node))
                                    printf("hi");
                            }
                        }
                    }
                }
            }
        }
    }
    return rv;
}
}  // namespace Parser