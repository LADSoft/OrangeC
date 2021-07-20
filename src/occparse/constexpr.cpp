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
    bool rv = false;
    if (TotalErrors() && !fromFunc && (!allowFunc || (node->type != en_func && node->type != en_thisref)))  // in some error conditions nodes can get into a loop
        // for purposes of this function...  guard against it.   Consider everything
        // CONST to avoid more errors..
        return true;
    if (node == 0)
        return rv;
    switch (node->type)
    {
        case en_dot:
        case en_pointsto:
            rv = false;
            break;
        case en_const:
            rv = true;
            break;
        case en_memberptr:
            rv = true;
            break;
        case en_templateparam:
        case en_templateselector:
            rv = true;
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:
        case en_c_string:
        case en_nullptr:
        case en_structelem:
            rv = true;
            break;
        case en_global:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            rv = true;
            break;
        case en_auto:
        case en_construct:
            rv = false;
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
            if (node->left->type == en_auto)
                rv = node->left->v.sp->sb->constexpression || (allowParams && node->left->v.sp->sb->storage_class == sc_parameter);
            else
                switch (node->left->type)
                {
                    case en_global:
                    case en_pc:
                    case en_labcon:
                    case en_absolute:
                    case en_threadlocal:
                        return node->left->v.sp->sb->constexpression;
                    default:
                        break;
                }
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_inative:
        case en_x_unative:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_x_string:
        case en_x_object:
        case en_trapcall:
        case en_shiftby:
            /*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en_literalclass:
            rv = IsConstantExpression(node->left, allowParams, allowFunc);
            break;
        case en_assign:
            rv = IsConstantExpression(node->left, allowParams, allowFunc);
//            rv = false;
            break;
        case en_autoinc:
        case en_autodec:
            rv = IsConstantExpression(node->left, allowParams, allowFunc);
            break;
        case en_add:
        case en_sub:
            /*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
            /*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
        case en__initblk:
        case en__cpblk:
            /*		case en_array: */

            rv = IsConstantExpression(node->left, allowParams, allowFunc);
            rv &= IsConstantExpression(node->right, allowParams, allowFunc);
            break;
        case en_void:
        case en_voidnz:
            rv = IsConstantExpression(node->left, allowParams, allowFunc);
            if (!fromFunc || node->right->type == en_void)
                rv &= IsConstantExpression(node->right, allowParams, allowFunc);
            break;
        case en_atomic:
            rv = false;
            break;
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_mp_as_bool:
        case en_thisref:
        case en_lvalue:
        case en_funcret:
        case en__initobj:
            rv = IsConstantExpression(node->left, allowParams, allowFunc);
            break;
        case en_func:
            return !node->v.func->ascall || (allowFunc && checkconstexprfunc(node));
            break;
        case en_stmt:
            rv = false;
            break;
        case en__sizeof:
            rv = true;
            break;
        default:
            rv = false;
            diag("IsConstantExpression");
            break;
    }
    return rv;
}
EXPRESSION* CopyExprWithArgReplacement(EXPRESSION* node, SYMBOL* sym, INITLIST* args)
{
    auto hr = basetype(sym->tp)->syms->table[0];
    if (hr->p->sb->thisPtr)
        hr = hr->next;
    std::unordered_map<SYMBOL*, EXPRESSION*> argmap;
    while (hr && args)
    {
        argmap[hr->p] = args->exp;
        hr = hr->next;
        args = args->next;
    }
    EXPRESSION* rv = copy_expression(node);
    std::stack<EXPRESSION*> stk;
    stk.push(node);
    while (!stk.empty())
    {
        auto exp = stk.top();
        stk.pop();
        if (exp->left)
            stk.push(exp->left);
        if (exp->right)
            stk.push(exp->right);
        if (exp->left && lvalue(exp))
        {
            if (exp->left->type == en_auto)
            {     
                auto node1 = argmap[exp->left->v.sp];
                if (node1)
                {
                    *exp = *node1;
                }
            }
        }
    }
    return rv;
}
bool EvaluateConstexprFunction(EXPRESSION*&node)
{
    bool rv = false;
    /*
    auto args = node->v.func->arguments;
    while (args)
    {
        if (!IsConstantExpression(args->exp, true, true))
            break;
        args = args->next;
    }
    if (!args)
    */
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
                            // st->select = CopyExpressionWithArgs(st->select, found1, node->v.func->arguments);
                            bool optimizeConstants = !node->v.func->arguments;
                            if (optimizeConstants)
                                optimize_for_constants(&st->select);
                            functionnestingcount--;
                            if (IsConstantExpression(st->select, false, false))
                            {
                                *node = *st->select;
                                node->noexprerr = true;
                                rv = true;
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