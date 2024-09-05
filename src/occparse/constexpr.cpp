/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "cpplookup.h"
#include "constopt.h"
#include "declare.h"
#include "declcpp.h"
#include "lex.h"
#include "help.h"
#include "memory.h"
#include "expr.h"
#include "initbackend.h"
#include "declcons.h"
#include "stmt.h"
#include "beinterf.h"
#include "iexpr.h"
#include "floatconv.h"
#include "symtab.h"
#include "ListFactory.h"
#include "inline.h"
#include "mangle.h"
#include "types.h"

namespace Parser
{
static int functionNestingCount;

std::stack<std::unordered_map<SYMBOL*, EXPRESSION*>*> nestedMaps;

static EXPRESSION* InstantiateStruct(Type* tp, EXPRESSION* thisptr, EXPRESSION* ths);
static EXPRESSION* LookupStruct(EXPRESSION* exp);

static int anonvp;
static int expressionNesting;


void constexprinit()
{
    while (!nestedMaps.empty())
        nestedMaps.pop();
    functionNestingCount = 0;
    anonvp = 1;
}
bool checkconstexprfunc(EXPRESSION* node)
{
    if (node->type == ExpressionNode::thisref_)
        node = node->left;
    if (node->type == ExpressionNode::callsite_ && node->v.func->sp)
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
        (!allowFunc || (node->type != ExpressionNode::callsite_ && node->type != ExpressionNode::thisref_)))  // in some error conditions nodes can get into a loop
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
            case ExpressionNode::global_:
            case ExpressionNode::pc_:
            case ExpressionNode::absolute_:
            case ExpressionNode::threadlocal_:
                if (!exp->v.sp->sb->constexpression)
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
            case ExpressionNode::l_bitint_:
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
            case ExpressionNode::l_ubitint_:
            case ExpressionNode::l_string_:
            case ExpressionNode::l_object_:
                if (exp->left->type == ExpressionNode::auto_)
                {
                    if (!(exp->left->v.sp->sb->constexpression || exp->left->v.sp->sb->retblk ||
                          (allowParams && exp->left->v.sp->sb->storage_class == StorageClass::parameter_)))
                        return false;
                }
                else
                {
                    switch (exp->left->type)
                    {
                        case ExpressionNode::global_:
                        case ExpressionNode::pc_:
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
            case ExpressionNode::comma_:
            case ExpressionNode::check_nz_:
                stk.push(exp->left);
                if (exp->right->type == ExpressionNode::comma_ || !fromFunc)
                {
                    stk.push(exp->right);
                }
                break;
            case ExpressionNode::callsite_:
                if (!(!exp->v.func->ascall || (allowFunc && checkconstexprfunc(exp))))
                    return false;
                break;
            case ExpressionNode::thisref_:
                if (exp->left->type == ExpressionNode::callsite_ && (!exp->left->v.func->thisptr && (exp->left->v.func->sp->sb->storage_class == StorageClass::member_ || exp->left->v.func->sp->sb->storage_class == StorageClass::virtual_)) && exp->left->v.func->sp->sb->parentClass)
                    return false;
                stk.push(exp->left);
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
// these tests fall flat because they don't test the specific constructor
// used to construct things...
static bool hasNoBody(std::list<Statement*>* stmts)
{
    if (stmts)
    {
        for (auto stmt : *stmts)
        {
            if (stmt->type != StatementNode::line_ && stmt->type != StatementNode::varstart_ && stmt->type != StatementNode::dbgblock_)
                return false;
            // modified this next line to use 'lower'
            if (stmt->type == StatementNode::block_ && !hasNoBody(stmt->lower))
                return false;
        }
    }
    return true;
}
bool isConstexprConstructor(SYMBOL* sym)
{
    if (sym->sb->constexpression)
        return true;
    if (!sym->sb->deleted && !sym->sb->defaulted && !hasNoBody(sym->sb->inlineFunc.stmt))
        return false;
    for (auto sp : *sym->sb->parentClass->tp->syms)
    {
        if (ismemberdata(sp) && !sp->sb->init)
        {
            bool found = false;
            if (sym->sb->memberInitializers)
                for (auto memberInit : *sym->sb->memberInitializers)
                    if (!strcmp(memberInit->name, sp->name))
                    {
                        found = true;
                        break;
                    }
            if (!found)
                return false;
        }
    }
    return true;
}
bool ResolveConstExprLval(EXPRESSION** node)
{
    bool rv = false;

    auto node1 = *node;
    if (TakeAddress(&node1))
    {
        while (IsCastValue(node1))
            node1 = node1->left;
        if (node1->type == ExpressionNode::structadd_ || node1->type == ExpressionNode::add_ || node1->type == ExpressionNode::sub_ ||  node1->type == ExpressionNode::auto_ || node1->type == ExpressionNode::cvarpointer_)
        {
            int i = 0;
            EXPRESSION* spe = relptr(node1, i);

            if (spe)
            {
                while (IsCastValue(spe)) spe = spe->left;
                if (spe->type == ExpressionNode::cvarpointer_)
                {
                    int n = i / spe->v.constexprData.multiplier;
                    if (n < spe->v.constexprData.size && spe->v.constexprData.data[n])
                    {
                        spe = spe->v.constexprData.data[n];
                        if (IsConstantExpression(spe, false, false, false))
                        {
                            *node = spe;
                            rv = true;
                        }
                    }
                }
                if (!rv)
                {
                    if (spe && spe->type == ExpressionNode::auto_)
                    {
                        auto node2 = LookupStruct(spe);
                        if (node2 && node2 != (EXPRESSION*)-1)
                        {
                            int n = i;
                            n /= node2->v.constexprData.multiplier;
                            if (n < node2->v.constexprData.size)
                            {
                                node2 = node2->v.constexprData.data[n];
                                if (node2)
                                {
                                    *node = node2;
                                    rv = true;
                                }
                            }
                        }
                        else if (!(*node)->left->init && inConstantExpression)
                        {
                            if (spe->v.sp->sb->init)
                            {
                                for (auto t : *spe->v.sp->sb->init)
                                {
                                    if (t->offset == i)
                                    {
                                        *node = t->exp;
                                        break;
                                    }
                                }
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
static char* anonvpname()
{
    char buf[256];
    sprintf(buf, "$ce %d", anonvp++);
    return litlate(buf);
}

static EXPRESSION* MakeVarPtr(bool byRef, unsigned size, unsigned multiplier, SYMBOL* sp, EXPRESSION* base)
{
    auto exp = base;
    if (sp == nullptr)
    {
        sp = makeID(StorageClass::auto_, &stdint, nullptr, anonvpname());
    }
    if (!base || !byRef || base->type != ExpressionNode::cvarpointer_ || !base->v.constexprData.ref)
    {
        exp = MakeExpression(ExpressionNode::cvarpointer_);
        exp->v.sp = sp;
        exp->v.constexprData = { (unsigned short)size, (unsigned short)multiplier, byRef, Allocate<EXPRESSION*>(size) };
        exp->v.constexprData.data[0] = base;
    }
    return exp;
}
static EXPRESSION* LookupStruct(EXPRESSION* exp)
{
    if (!exp)
        return nullptr;
    while (exp->type == ExpressionNode::comma_)
        exp = exp->right;
    while (TakeAddress(&exp));
    if (exp->type == ExpressionNode::auto_ && !nestedMaps.empty())
    {
        auto it = (*nestedMaps.top()).find(exp->v.sp);
        if (it != (*nestedMaps.top()).end())
        {
            auto val = it->second;
            if (val)
                return val;
        }
    }
    return nullptr;
}
static EXPRESSION* EvaluateExpression(EXPRESSION* node, EXPRESSION* ths, EXPRESSION* retblk, bool restart = false);

static EXPRESSION* ConstExprInitializeMembers(SYMBOL* sym, EXPRESSION* thisptr, std::list<Argument*>& args)
{
    auto exp = MakeVarPtr(false, sym->sb->parentClass->tp->size, 1, sym, nullptr);
    for (auto sp : *sym->sb->parentClass->tp->syms)
    {
        if (ismemberdata(sp) && sp->sb->init)
        {
            auto expx = sp->sb->init->front()->exp;
            EvaluateExpression(expx, exp, nullptr, true);
            exp->v.constexprData.data[sp->sb->offset] = expx;
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
                    for (auto init : *m->init)
                        if (init->exp)
                        {
                            auto expx = init->exp;
                            expx = EvaluateExpression(expx, thisptr, nullptr, true);
                            exp->v.constexprData.data[sp->sb->offset] = expx;
                        }
                }
            }
    }
    return exp;
}
static EXPRESSION* InstantiateStruct(Type* tp, EXPRESSION* thisptr, EXPRESSION* ths)
{
    if (ths->type == ExpressionNode::comma_)
    {
        if (thisptr)
        {
            auto exp = &ths;
            while (exp && *exp)
            {
                EXPRESSION** exp1 = nullptr;
                if ((*exp)->type == ExpressionNode::comma_)
                {
                    if ((*exp)->left->type == ExpressionNode::assign_)
                    {
                        exp1 = &(*exp)->left->left;
                    }
                    else if ((*exp)->left->type == ExpressionNode::blockclear_)
                    {
                        exp1 = &(*exp)->left->left;

                    }
                    exp = &(*exp)->right;
                }
                else
                {
                    exp1 = exp;
                    exp = nullptr;
                }
                if (exp1 && (*exp1)->type == ExpressionNode::l_p_ && (*exp1)->left->type == ExpressionNode::auto_)
                {
                    if ((*exp1)->left->v.sp->sb->retblk)
                    {
                        *exp1 = thisptr;
                    }
                }
            }
        }
        return ths;
    }
    else
    {
        EXPRESSION* rv = nullptr;
        EXPRESSION** last = &rv;
        SYMBOL* varsp = nullptr;
        EXPRESSION* varptr;
        // returning a structure as a return value or this pointer
        if (thisptr->type == ExpressionNode::auto_ && thisptr->v.sp->sb->stackblock)
        {
            thisptr = MakeExpression(ExpressionNode::substack_, MakeIntExpression(ExpressionNode::c_i_, thisptr->v.sp->tp->size));
        }
        varptr = AnonymousVar(StorageClass::auto_, &stdpointer);
        varptr->v.sp->sb->constexpression = true;
        varptr->isStructAddress = true;
        varsp = varptr->v.sp;
        Dereference(&stdpointer, &varptr);
        rv = MakeExpression(ExpressionNode::assign_, varptr, thisptr);

//        for (auto sp : *ths->v.sp->sb->parentClass->tp->syms)
        for (auto sp : *tp->BaseType()->syms)
        {
            if (ismemberdata(sp))
            {
                if (ths->v.constexprData.data[sp->sb->offset] == nullptr)
                {
                    if (varsp)
                        undoAnonymousVar(varsp);
                    return nullptr;
                }
                auto data = ths->v.constexprData.data[sp->sb->offset];
                if (data->type == ExpressionNode::thisref_)
                    if (data->left->type == ExpressionNode::cvarpointer_)
                        data = data->left;
                if (sp->tp->IsStructured() && data->type == ExpressionNode::cvarpointer_)
                {
                    for (auto sp1 : *sp->tp->BaseType()->syms)
                    {
                        if (ismemberdata(sp1))
                        {
                            EXPRESSION* next = MakeExpression(ExpressionNode::structadd_, varptr, MakeIntExpression(ExpressionNode::c_i_, sp->sb->offset + sp1->sb->offset));
                            Dereference(sp1->tp, &next);
                            next = MakeExpression(ExpressionNode::assign_, next,
                                EvaluateExpression(data->v.constexprData.data[sp1->sb->offset], data, nullptr, true));
                            if (next->right == nullptr || !IsConstantExpression(next->right, false, false))
                            {
                                if (varsp)
                                    undoAnonymousVar(varsp);
                                return nullptr;
                            }
                            inConstantExpression++;
                            optimize_for_constants(&next->right);
                            inConstantExpression--;
                            *last = MakeExpression(ExpressionNode::comma_, *last, next);
                            last = &(*last)->right;
                        }
                    }
                }
                else
                {
                    EXPRESSION* next = MakeExpression(ExpressionNode::structadd_, varptr, MakeIntExpression(ExpressionNode::c_i_, sp->sb->offset));
                    Dereference(sp->tp, &next);
                    next = MakeExpression(ExpressionNode::assign_, next,
                    EvaluateExpression(data, ths, nullptr, true));
                    if (next->right == nullptr || !IsConstantExpression(next->right, false, false))
                    {
                        if (varsp)
                            undoAnonymousVar(varsp);
                        return nullptr;
                    }
                    inConstantExpression++;
                    optimize_for_constants(&next->right);
                    inConstantExpression--;
                    *last = MakeExpression(ExpressionNode::comma_, *last, next);
                    last = &(*last)->right;
                }
            }
        }
        if (thisptr->type == ExpressionNode::substack_)
        {
            *last = MakeExpression(ExpressionNode::comma_, *last, varptr);
        }
        else
        {
            *last = MakeExpression(ExpressionNode::comma_, *last, thisptr);
        }
        return rv;
    }
}
static bool pushArrayOrStruct(SYMBOL* arg, EXPRESSION* exp, std::unordered_map<SYMBOL*, EXPRESSION*>& argmap)
{
    bool rv = false;
    SYMBOL* finalsym = nullptr;
    if (exp->type == ExpressionNode::stackblock_ && exp->left->type == ExpressionNode::comma_)
    {
        auto exp2 = exp->left;
        while (exp2->type == ExpressionNode::comma_ && exp2->right)
            exp2 = exp2->right;
        if (exp2->type == ExpressionNode::auto_)
            finalsym = exp2->v.sp;
    }
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
        if (node->type == ExpressionNode::auto_ || node->type == ExpressionNode::global_ || node->type == ExpressionNode::add_)
        {
            int offset = 0;
            auto rel = relptr(node, offset);
            if (rel && rel->v.sp->tp->IsArray() && rel->v.sp->sb->init)
            {
                // this is an array that is passed as other than an initializer list
                int n = rel->v.sp->sb->init->size()-1;
                if (n && rel->v.sp->sb->init)
                {
                    if (argmap.find(rel->v.sp) != argmap.end())
                    {
                        auto expx = argmap[rel->v.sp];
                        if (offset)
                            expx = MakeExpression(ExpressionNode::add_, expx, MakeIntExpression(ExpressionNode::c_i_, offset));
                        if (arg->tp->IsRef() && !arg->tp->BaseType()->btp->IsArray())
                            argmap[arg] = MakeVarPtr(true, 1, 1, rel->v.sp, MakeVarPtr(false, 1, 1, rel->v.sp, expx));
                        else
                            argmap[arg] = MakeVarPtr(false, 1, 1, rel->v.sp, expx);
                    }
                    else
                    {
                        int sz = rel->v.sp->tp->BaseType()->btp->size;
                        auto expx = MakeVarPtr(false, n, sz, rel->v.sp, nullptr);
                        argmap[rel->v.sp] = expx;
                        auto target = expx->v.constexprData.data;
                        if (offset)
                            expx = MakeExpression(ExpressionNode::add_, expx, MakeIntExpression(ExpressionNode::c_i_, offset));
                        if (arg->tp->IsRef() && !arg->tp->BaseType()->btp->IsArray())
                            argmap[arg] = MakeVarPtr(true, 1, 1, rel->v.sp, MakeVarPtr(false, 1, 1, rel->v.sp, expx));
                        else
                            argmap[arg] = MakeVarPtr(false, 1, 1, rel->v.sp, expx);
                        int i = 0;
                        for (auto init : *rel->v.sp->sb->init)
                            target[i++] = init->exp;
                        rv = true;
                    }
                    break;
                }
            }
            else if (rel)
            {
                rel = LookupStruct(rel);
                if (rel)
                {
                    rv = true;
                    argmap[arg] = MakeVarPtr(false, 1, 1, arg, rel);
                }
            }
        }
    }
    if (!rv && exp->type == ExpressionNode::comma_)
    {
        while (exp->type == ExpressionNode::comma_)
            exp = exp->right;
        if (exp->type == ExpressionNode::auto_)
        {
            exp = LookupStruct(exp);
            if (exp)
            {
                argmap[arg] = exp;
            }
        }
    }
    return rv;
}
static void pushArrayOrStruct(SYMBOL* arg, std::list<Argument*>& il, std::unordered_map<SYMBOL*, EXPRESSION*>& argmap)
{
    if (arg->tp->IsStructured() && !arg->tp->BaseType()->sp->sb->initializer_list)
    {
        argmap[arg] = MakeVarPtr(false, arg->tp->size, 1, arg, nullptr);
        auto target = argmap[arg]->v.constexprData.data;
        auto sit = arg->tp->BaseType()->syms->begin();
        auto site = arg->tp->BaseType()->syms->end();
        int count = 0;
        for (auto&& it : il)
        {
            while (sit != site && !ismemberdata(*sit)) ++sit;
            if (sit == site)
                break;
            target[(*sit)->sb->offset] = it->exp;
            ++sit;
            ++count;
        }
        if (count == il.size())
        {
            while (sit != site)
            {
                while (sit != site && !ismemberdata(*sit)) ++sit;
                if (sit != site)
                {
                    target[(*sit)->sb->offset] = MakeIntExpression(ExpressionNode::c_i_, 0);
                    ++sit;
                }
            }
        }
        else
        {
            argmap.erase(arg);
        }
    }
    else
    {
        // this is an initializer list for an array
        int n = il.size();
        argmap[arg] = MakeVarPtr(false, getSize(BasicType::pointer_) + getSize(BasicType::int_), 1, arg, nullptr);
        auto& listDeclarator = argmap[arg]->v.constexprData;
        listDeclarator.data[0] = MakeVarPtr(false, n, il.front()->tp->size, arg, nullptr);
        listDeclarator.data[getSize(BasicType::pointer_)] = MakeIntExpression(ExpressionNode::c_i_, n);
        auto target = listDeclarator.data[0]->v.constexprData.data;

        n = 0;
        for (auto&& xil : il)
        {
            target[n++] = xil->exp;
        }
    }
}
static void pushStruct(SYMBOL* arg, EXPRESSION* exp, std::unordered_map<SYMBOL*, EXPRESSION*>& argmap)
{
    if (arg->tp->IsStructured() || (arg->tp->IsRef() && arg->tp->BaseType()->btp->IsStructured()))
    {
        if (exp->type == ExpressionNode::comma_ && exp->left->type == ExpressionNode::thisref_)
        {
            exp = exp->left;
        }
        if (exp->type == ExpressionNode::thisref_)
        {
            exp = exp->left;
        }
        if (exp->type == ExpressionNode::callsite_)
        {
            exp = EvaluateExpression(exp, nullptr, nullptr, true);
        }
    }
    if (exp->type == ExpressionNode::cvarpointer_)
    {
        if (arg->tp->IsRef())
        {
            argmap[arg] = MakeVarPtr(true, 1, 1, arg, exp);
        }
        else
        {
            argmap[arg] = exp;
        }
    }
    else if (exp->type == ExpressionNode::comma_ || (exp->type == ExpressionNode::stackblock_ && arg->tp->IsStructured() && arg->tp->BaseType()->sp->sb->initializer_list))
    {
        if (arg->tp->IsStructured() || (arg->tp->IsRef() && arg->tp->BaseType()->btp->IsStructured()))
        {
            EXPRESSION** target;
            if (exp->type == ExpressionNode::stackblock_)
            {
                // initializer list
                // get the size out of the initializer list backing stream....
                auto backing = exp->left->left->right->left->right;
                auto size = exp->left->left->right->right->right;
                argmap[arg] = MakeVarPtr(false, arg->tp->size, 1, arg, nullptr);
                argmap[arg]->v.constexprData.data[0] = MakeVarPtr(false, backing->v.sp->tp->size, 1, arg, nullptr);
                argmap[arg]->v.constexprData.data[getSize(BasicType::pointer_)] = size;
                target = argmap[arg]->v.constexprData.data[0]->v.constexprData.data;
                exp = exp->left->left->left;
            }
            else if (arg->tp->IsRef())
            {
                argmap[arg] = MakeVarPtr(true, 1, 1, arg, MakeVarPtr(false, arg->tp->BaseType()->btp->BaseType()->size, 1, arg, nullptr));
                target = argmap[arg]->v.constexprData.data[0]->v.constexprData.data;
            }
            else
            {
                argmap[arg] = MakeVarPtr(false, arg->tp->size, 1, arg, nullptr);
                target = argmap[arg]->v.constexprData.data;
            }
            while (exp->type == ExpressionNode::comma_)
            {
                if (exp->left->type == ExpressionNode::assign_)
                {
                    int m = -1;
                    auto node = exp->left->left;
                    if (IsLValue(node))
                    {
                        if (node->left->type == ExpressionNode::structadd_)
                        {
                            auto rel = relptr(node->left, m);
                            if (rel && rel->type == ExpressionNode::auto_)
                            {
                                m = node->left->right->v.i;
                            }
                            else
                            {
                                auto node1 = node->left->left;
                                while (TakeAddress(&node1));
                                if (node1->type == ExpressionNode::auto_)
                                {
                                    m = node->left->right->v.i;
                                }
                            }
                        }
                        else
                        {
                            node = exp->left;
                            EvaluateExpression(node, nullptr, nullptr, true);
                        }
                    }
                    if (m >= 0)
                    {
                        node = exp->left->right;
                        EvaluateExpression(node, nullptr, nullptr, true);
                        optimize_for_constants(&node);
                        target[m] = node;
                    }
                }
                exp = exp->right;
            }
            if (exp->type == ExpressionNode::assign_)
            {
                int m = -1;
                auto node = exp->left;
                if (IsLValue(node))
                {
                    if (node->left->type == ExpressionNode::structadd_)
                    {
                        auto rel = relptr(node->left, m);
                        if (rel && rel->type == ExpressionNode::auto_)
                        {
                            m = node->left->right->v.i;
                        }
                        else
                        {
                            auto node1 = node->left->left;
                            while (TakeAddress(&node1));
                            if (node1->type == ExpressionNode::auto_)
                            {
                                m = node->left->right->v.i;
                            }
                        }
                    }
                    else
                    {
                        node = exp;
                        EvaluateExpression(node, nullptr, nullptr, true);
                    }
                }
                if (m >= 0)
                {
                    node = exp->right;
                    EvaluateExpression(node, nullptr, nullptr, true);
                    optimize_for_constants(&node);
                    target[m] = node;
                }
            }
        }
        else
        {
            if (exp->type == ExpressionNode::comma_ && exp->right->type != ExpressionNode::comma_)
            {
                if (exp->left->type == ExpressionNode::assign_)
                {
                    auto exp1 = EvaluateExpression(exp->left->right, nullptr, nullptr, true);
                    optimize_for_constants(&exp1);
                    argmap[arg] = MakeVarPtr(true, 1, 1, arg, MakeVarPtr(false, 1, 1, arg, exp1));
                }
            }
        }
    }
    else if ((exp->type == ExpressionNode::auto_ || exp->type == ExpressionNode::global_))
    {
        EXPRESSION** target;
        if (exp->v.sp->sb->init)
        {
            if (arg->tp->IsRef())
            {
                argmap[arg] = MakeVarPtr(true, 1, 1, exp->v.sp, MakeVarPtr(false, exp->v.sp->tp->size, 1, exp->v.sp, nullptr));
                target = argmap[arg]->v.constexprData.data[0]->v.constexprData.data;
            }
            else
            {
                argmap[arg] = MakeVarPtr(false, exp->v.sp->tp->size, 1, exp->v.sp, nullptr);
                target = argmap[arg]->v.constexprData.data;
            }
            for (auto init : *exp->v.sp->sb->init)
            {
                if (init->exp)
                {
                    target[init->offset] = init->exp;
                }
            }
        }
        else if (arg->tp->IsRef())
        {
            argmap[arg] = MakeVarPtr(true, 1, 1, nullptr, exp);
        }
    }
    else if (exp->type == ExpressionNode::l_p_ && arg->tp->IsRef())
    {
        argmap[arg] = MakeVarPtr(true, 1, 1, nullptr, MakeVarPtr(false, 1, 1, nullptr, exp));
    }
}
static bool pushThis(EXPRESSION* thisptr, EXPRESSION* ths)
{
    bool rv = false;
    if ((thisptr->type == ExpressionNode::auto_ || thisptr->type == ExpressionNode::global_) && thisptr->v.sp->sb->init)
    {
        rv = true;
        for (auto init : *thisptr->v.sp->sb->init)
        {
            if (init->exp)
            {
                auto node = init->exp;
                ths->v.constexprData.data[init->offset] = EvaluateExpression(node, nullptr, nullptr);
            }
        }
    }
    else if (thisptr->type == ExpressionNode::comma_)
    {
        while (thisptr && thisptr->type == ExpressionNode::comma_ && thisptr->left->type == ExpressionNode::assign_)
        {
            auto exp1 = thisptr->left->left;
            if (TakeAddress(&exp1))
            {
                if (exp1->type == ExpressionNode::structadd_)
                {
                    ths->v.constexprData.data[exp1->right->v.i] = EvaluateExpression(thisptr->left->right, nullptr, nullptr);
                    rv = true;
                }
            }

            thisptr = thisptr->right;
        }
    }
    return rv;
}
static bool HandleAssign(EXPRESSION* exp, EXPRESSION* ths, EXPRESSION* retblk)
{
    EXPRESSION* rv = nullptr;
    if (exp->type == ExpressionNode::auto_inc_ || exp->type == ExpressionNode::auto_dec_)
    {
        if (IsLValue(exp->left) && !IsLValue(exp->left->left))
        {
            auto assn = EvaluateExpression(exp->left->left, ths, retblk);
            if (assn->type == ExpressionNode::cvarpointer_)
            {
                rv = copy_expression(assn->v.constexprData.data[0]);
                EXPRESSION* inced = MakeExpression(exp->type == ExpressionNode::auto_inc_ ? ExpressionNode::add_ : ExpressionNode::sub_, assn->v.constexprData.data[0], exp->right);
                optimize_for_constants(&inced);
                if (inced->type == ExpressionNode::add_  && isintconst(inced->left))
                {
                    auto t = inced->left;
                    inced->left = inced->right;
                    inced->right = t;
                }
                assn->v.constexprData.data[0] = inced;
            }
        }
    }
    else if (exp->type == ExpressionNode::assign_)
    {
        auto node1 = exp->left;
        if (TakeAddress(&node1))
        {
            auto assn = EvaluateExpression(node1, ths, retblk, true);
            if (assn->type == ExpressionNode::auto_ && !nestedMaps.empty())
            {
                auto sp = assn->v.sp;
                // new variable
                assn = MakeVarPtr(false, 1, 1, sp, nullptr);
                (*nestedMaps.top())[sp] = assn;

            }
            if (assn->type == ExpressionNode::cvarpointer_)
            {
                rv = copy_expression(EvaluateExpression(exp->right, ths, retblk));
                optimize_for_constants(&rv);
                assn->v.constexprData.data[0] = rv;
            }
            else if (assn->type == ExpressionNode::structadd_)
            {
                int offset = 0;
                auto rel = relptr(assn, offset);
                if (rel && rel->type == ExpressionNode::cvarpointer_)
                {
                    rv = copy_expression(EvaluateExpression(exp->right, ths, retblk));
                    optimize_for_constants(&rv);
                    rel->v.constexprData.data[offset] = rv;
                }

            }
        }
    }
    if (rv)
    {
        *exp = *rv;
    }
    return !!rv;
}
static bool HandleLoad(EXPRESSION* exp, EXPRESSION* ths, EXPRESSION* retblk)
{
    EXPRESSION* rv = nullptr;
    if (IsLValue(exp))
    {
        auto exp3 = exp;
        while (IsCastValue(exp3))
            exp3 = exp3->left;
        auto exp4 = exp3;
        exp3 = exp3->left;
        while (IsCastValue(exp3))
            exp3 = exp3->left;
        if (ths && exp4->type == ExpressionNode::l_p_ && exp3->type == ExpressionNode::auto_ && exp3->v.sp->sb->thisPtr)
        {
            rv = ths;
        }
        else
        {
            optimize_for_constants(&exp3);
            int offset = 0;
            auto rel = relptr(exp3, offset);
            if (rel && rel->type == ExpressionNode::cvarpointer_)
            {
                offset /= rel->v.constexprData.multiplier;
                if (offset < rel->v.constexprData.size)
                {
                    rv = rel->v.constexprData.data[offset];
                }
            }
        }
    }
    else
    {
        auto exp3 = exp;
        while (IsCastValue(exp3))
            exp3 = exp3->left;
        if (exp3->type == ExpressionNode::auto_)
        {
            auto ths1 = LookupStruct(exp3);
            if (ths1)
            {
                rv = ths1;
            }
        }
        else if (exp->type == ExpressionNode::blockclear_)
        {
            if (exp->left->type == ExpressionNode::auto_)
            {
                int n, sz;
                if (exp->left->v.sp->tp->array)
                {
                    sz = exp->left->v.sp->tp->btp->size;
                    n = exp->left->v.sp->tp->size / sz;
                }
                else
                {
                    sz = 1;
                    n = exp->left->v.sp->tp->size;
                }
                auto ths1 = LookupStruct(exp);
                if (!ths1 && nestedMaps.size())
                {
                    ths1 = MakeVarPtr(false, n, sz, nullptr, nullptr);
                    (*nestedMaps.top())[exp->left->v.sp] = ths1;

                }
                else if (ths1)
                {
                    memset(ths1->v.constexprData.data, 0, n * sizeof(EXPRESSION*));
                }
            }
        }
        else if (exp->type == ExpressionNode::callsite_)
        {
            auto func = Allocate<CallSite>();
            EXPRESSION temp = *exp, * temp1 = &temp;
            *func = *exp->v.func;
            temp1->v.func = func;
            std::list<Argument*>* argslst = func->arguments;
            func->arguments = argumentListFactory.CreateList();
            bool failed = false;
            if (argslst)
                for (auto args : *argslst)
                {
                    auto lst = Allocate<Argument>();
                    *lst = *args;
                    func->arguments->push_back(lst);
                    auto exp2 = args->exp;
                    if (exp2->type == ExpressionNode::comma_ && exp2->left->type == ExpressionNode::assign_ && 
                        args->tp->IsRef() && !args->tp->BaseType()->btp->IsStructured())
                    {
                        // temporary passed by-ref
                        exp2 = copy_expression(exp2);
                        exp2->left->right = EvaluateExpression(exp2->left->right, ths, retblk);
                    }
                    else if (!lst->tp->IsStructured())
                    {
                        lst->exp = EvaluateExpression(lst->exp, ths, retblk, true);
                        if (!lst->exp)
                            return false;
                        optimize_for_constants(&lst->exp);
                        if (lst->exp->type == ExpressionNode::thisref_ || lst->exp->type == ExpressionNode::callsite_)
                            failed = true;
                        while (lst->exp->type == ExpressionNode::comma_ && lst->exp->right)
                            lst->exp = lst->exp->right;
                    }
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
            else if (ths && func->thisptr && func->thisptr->type == ExpressionNode::l_p_ && func->thisptr->left->type == ExpressionNode::auto_ &&
                func->thisptr->left->v.sp->sb->thisPtr)
            {
                func->thisptr = ths;
            }
            else if (func->thisptr)
            {
                auto str = func->thisptr;
                if (str->type == ExpressionNode::thisref_)
                {
                    str = EvaluateExpression(str, ths, retblk, true);
                }
                else
                {
                    str = LookupStruct(func->thisptr);
                    if (str)
                    {
                        if (str && str->type == ExpressionNode::cvarpointer_ && str->v.constexprData.ref)
                            str = str->v.constexprData.data[0];
                    }
                }
                if (str)
                {
                    func->thisptr = str;
                }
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
            if (temp1->type != ExpressionNode::callsite_ || temp1->v.func->sp != exp->v.func->sp)
            {
                rv = temp1;
            }
        }
        else if (exp->type == ExpressionNode::hook_)
        {
            auto select = EvaluateExpression(exp->left, ths, retblk);
            optimize_for_constants(&select);
            if (select && isintconst(select))
            {
                if (select->v.i)
                {
                    select = exp->right->left;
                }
                else
                {
                    select = exp->right->right;
                }
                select = EvaluateExpression(select, ths, retblk);
                optimize_for_constants(&select);
                if (select && IsConstantExpression(select, false, false))
                {
                    rv = select;
                }
            }
        }
    }
    if (rv)
    {
        *exp = *rv;
    }
    return !!rv;
}

static EXPRESSION* EvaluateExpression(EXPRESSION* node, EXPRESSION* ths, EXPRESSION* retblk, bool restart)
{
    if (node && node->type == ExpressionNode::select_)
        node = node->left;
    int oldNesting = expressionNesting;
    if (restart)
        expressionNesting = 0;
    if (!expressionNesting++)
        node = copy_expression(node);
    std::stack<EXPRESSION*> stk;
    std::deque<EXPRESSION*> working;
    working.push_back(node);
    while (!working.empty())
    {
        auto node1 = working.front();
        working.pop_front();
        switch (node1->type)
        {
            // let these evaluate their own args
            case ExpressionNode::assign_:
            case ExpressionNode::auto_dec_:
            case ExpressionNode::auto_inc_:
                stk.push(node1);
                break;
            case ExpressionNode::comma_:
                if (node1->right->type == ExpressionNode::comma_)
                {
                    working.push_back(node1->right);
                }
                working.push_back(node1->left);
                break;
            default:
                stk.push(node1);
                if (node1->right)
                    working.push_back(node1->right);
                if (node1->left)
                    working.push_back(node1->left);
                break;
        }
    }
    while (!stk.empty())
    {
        auto node1 = stk.top();
        stk.pop();
        if (!HandleAssign(node1, ths, retblk))
            HandleLoad(node1, ths, retblk);
    }
    expressionNesting = oldNesting;
    return node;
}
static EXPRESSION* ParseRetBlock(EXPRESSION* funcNode, EXPRESSION* retNode)
{
    if (funcNode->v.func->returnEXP && retNode->type == ExpressionNode::comma_)
    {
        if (retNode->left->type == ExpressionNode::l_p_ && retNode->right->type == ExpressionNode::comma_)
        {
            auto exp = retNode;
            while (exp->type == ExpressionNode::comma_) exp = exp->right;
            if (exp->type == ExpressionNode::l_p_ && exp->left->type == ExpressionNode::auto_ && exp->left->v.sp->sb->retblk)
            {
                auto rv = MakeVarPtr(false, funcNode->v.func->returnSP->tp->size, 1, funcNode->v.func->returnSP, Allocate<EXPRESSION>(funcNode->v.func->returnSP->tp->size));


                while (retNode->type == ExpressionNode::comma_ && retNode->left->type == ExpressionNode::l_p_) retNode = retNode->right;
                while (retNode->type == ExpressionNode::comma_ && retNode->left->type == ExpressionNode::assign_)
                {
                    auto lhs = retNode->left->left;
                    while (TakeAddress(&lhs));
                    if (lhs->type != ExpressionNode::structadd_ || lhs->left->type != ExpressionNode::l_p_ || lhs->left->left->type != ExpressionNode::auto_ || !lhs->left->left->v.sp->sb->retblk)
                    {
                        break;
                    }
                    rv->v.constexprData.data[lhs->right->v.i] = retNode->left->right;
                    retNode = retNode->right;
                }
                return rv;
            }
        }
    }
    return retNode;
}
static EXPRESSION* EvaluateStatements(EXPRESSION* node, std::list<Statement*>* stmt, EXPRESSION* ths, EXPRESSION* retblk)
{
    std::unordered_map<int, std::list<Statement*>::iterator> labels;
    std::stack<std::list<Statement*>*> stk;
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
    std::stack<std::pair<std::list<Statement*>*, std::list<Statement*>::iterator>> blockList;
    blockList.push(std::pair<std::list<Statement*>*, std::list<Statement*>::iterator>(stmt, stmt->begin()));
    while (!blockList.empty())
    {
        bool breakout = false;
        auto s = blockList.top();
        blockList.pop();
        for (auto it = s.second; it != s.first->end(); ++it)
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
            case StatementNode::asmgoto_:case StatementNode::asmcond_:
            case StatementNode::genword_:
            case StatementNode::passthrough_:
            case StatementNode::datapassthrough_:
                return nullptr;
            case StatementNode::select_:
            case StatementNode::notselect_: {
                if (Optimizer::cparams.prm_debug)
                    return nullptr;
                auto node1 = stmt->select;
                node1 = EvaluateExpression(node1, ths, retblk, true);
                optimize_for_constants(&node1);
                if (!isarithmeticconst(node1))
                    return nullptr;
                if (!isintconst(node1))
                {
                    node1->v.i = reint(node1->left);
                    node1->type = ExpressionNode::c_i_;
                    node1->left = nullptr;
                }
                if ((node1->v.i && stmt->type == StatementNode::notselect_) || (!node1->v.i && stmt->type == StatementNode::select_))
                    break;
                it = labels[stmt->label];
                break;
            }
            case StatementNode::goto_:
            case StatementNode::loopgoto_:
                if (Optimizer::cparams.prm_debug)
                    return nullptr;
                if (stmt->explicitGoto || stmt->indirectGoto)
                    return nullptr;
                it = labels[stmt->label];
                break;
            case StatementNode::switch_: {
                if (Optimizer::cparams.prm_debug)
                    return nullptr;
                auto node1 = stmt->select;
                node1 = EvaluateExpression(node1, ths, retblk, true);
                optimize_for_constants(&node1);
                if (!isintconst(node1))
                    return nullptr;
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
                break;
            }
            case StatementNode::block_:
            case StatementNode::seh_try_:
            case StatementNode::try_:
                // return value, next statement this block
                blockList.push(std::pair<std::list<Statement*>*, std::list<Statement*>::iterator>(s.first, ++it));
                // next program counter, next lower block
                blockList.push(std::pair<std::list<Statement*>*, std::list<Statement*>::iterator>(stmt->lower, stmt->lower->begin()));
                // break out of loop to activate
                breakout = true;
                break;
            case StatementNode::expr_:
                if (stmt->select)
                {
                    if (Optimizer::cparams.prm_debug)
                        return nullptr;
                    auto node1 = copy_expression(stmt->select);
                    node1 = EvaluateExpression(node1, ths, retblk, true);
                    optimize_for_constants(&node1);
                }
                break;
            case StatementNode::return_:
                if (stmt->select)
                {
                    auto node1 = copy_expression(stmt->select);
                    node1 = EvaluateExpression(node1, ths, node->v.func->returnEXP, true);
                    optimize_for_constants(&node1);
                    node1 = ParseRetBlock(node, node1);
                    if (node1 && IsConstantExpression(node1, false, false) && node1->type != ExpressionNode::callsite_ && node1->type != ExpressionNode::funcret_ &&
                        node1->type != ExpressionNode::thisref_)
                    {
                        if (node1->type == ExpressionNode::comma_ && node1->left->type == ExpressionNode::cvarpointer_)
                            node1 = node1->left;
                        if (node1->type == ExpressionNode::cvarpointer_)
                        {
                            if (node->v.func->sp->tp->BaseType()->btp->IsStructured() && node->v.func->sp->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType)
                            {
                                node1 = node1->v.constexprData.data[0];
                                node1 = EvaluateExpression(node1, ths, nullptr, true);
                                optimize_for_constants(&node1);
                                if (!IsConstantExpression(node1, false, false))
                                    node1 = nullptr;
                            }
                            else if (node->v.func->sp->sb->isConstructor)
                            {
                                for (int i = 0; i < node1->v.constexprData.size; i++)
                                {
                                    auto node2 = node1->v.constexprData.data[i];
                                    if (node2 && !IsConstantExpression(node2, false, false))
                                    {
                                        node1 = nullptr;
                                        break;
                                    }
                                }
                            }
                        }
                        if (node1)
                            node1->noexprerr = true;
                        return node1;
                    }
                    return nullptr;
                }
                break;
            }
            if (breakout)
                break;
        }
    }
    return nullptr;
}
bool EvaluateConstexprFunction(EXPRESSION*& node)
{
    bool rv = false;
    if (!definingTemplate || instantiatingTemplate)
    {
        if (node->v.func->sp->sb->isConstructor)
        {
            // we don't completely support constexpr constructors for classes with base classes right now...t 
            if (node->v.func->sp->sb->parentClass->sb->baseClasses && node->v.func->sp->sb->parentClass->sb->baseClasses->size())
            {
                for (auto&& v : *node->v.func->sp->sb->parentClass->sb->baseClasses)
                {
                    if (!v->cls->sb->trivialCons)
                    {
                        return false;
                    }
                }
                if (nestedMaps.size() == 0)
                {
                    auto members = false;
                    for (auto&& v : *node->v.func->sp->sb->parentClass->tp->syms)
                    {
                        members |= v->sb->storage_class == StorageClass::mutable_;
                        members |= v->sb->storage_class == StorageClass::member_;
                        if (members)
                            break;
                    }
                    if (!members)
                        return false;
                }
            }
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
        if (exp && (exp->type == ExpressionNode::callsite_ || exp->type == ExpressionNode::thisref_))
        {
            auto** exp = &node->v.func->thisptr;
            if ((*exp)->type == ExpressionNode::thisref_)
                exp = &(*exp)->left;
            if (!EvaluateConstexprFunction(*exp))
                return false;
        }
        if (node->v.func->returnEXP)
        {
            int offset = 0;
            auto exp1 = relptr(node->v.func->returnEXP, offset);
            if (exp1)
            {
                if (exp1->type == ExpressionNode::global_ || (exp1->type == ExpressionNode::auto_ && !exp1->v.sp->sb->anonymous))
                    if (!exp1->v.sp->sb->constexpression)
                        return false;
            }
        }
        if (exp && exp->type == ExpressionNode::auto_)
        {
            if (inLoopOrConditional)
                return false;
            if (!exp->v.sp->sb->constexpression && !exp->v.sp->sb->retblk)
            {
                return false;
            }
        }
        if (exp && exp->type == ExpressionNode::global_ && !exp->v.sp->sb->constexpression)
            return false;
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
                else {
                    if (!IsConstantExpression(args->exp, false, true, true))
                    {
                        found = true;
                        break;
                    }
                }
            }
        if (!found)
        {
            SYMBOL* found1 = node->v.func->sp;
            if (found1->tp->IsFunction())
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
                                found1 = detemplate(found1, nullptr, node->v.func->thistp->BaseType()->btp);
                            }
                            else
                            {
                                found1 = detemplate(found1, node->v.func, nullptr);
                            }
                        }
                    }
                    if (found1)
                    {
                        enclosingDeclarations.Mark();
                        pushContext(found1, false);
                        if (found1->sb->templateLevel && !definingTemplate && node->v.func->templateParams)
                        {
                            found1 = TemplateFunctionInstantiate(found1, false);
                        }
                        CompileInline(found1, false);
                        enclosingDeclarations.Release();
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
                            std::unordered_map<SYMBOL*, EXPRESSION*> argmap;
                            std::unordered_map<SYMBOL*, EXPRESSION*> tempmap;
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
                                auto it = found1->tp->BaseType()->syms->begin();
                                auto ite = found1->tp->BaseType()->syms->end();
                                if ((*it)->sb->thisPtr)
                                    ++it;
                                auto arglist = node->v.func->arguments->begin();
                                auto arglistend = node->v.func->arguments->end();
                                while (it != ite && arglist != arglistend)
                                {
                                    auto exp = (*arglist)->exp;
                                    int offset = 0;
                                    EXPRESSION* rel = nullptr;
                                    if (exp)
                                        rel = relptr(exp, offset);
                                    if (rel && rel->type == ExpressionNode::cvarpointer_)
                                    {
                                        // being passed from a higher constexpr function
                                        if ((*it)->tp->IsStructured())
                                            tempmap[*it] = exp;
                                        else
                                            tempmap[*it] = MakeVarPtr((*arglist)->tp->IsRef(), 1, 1, *it, exp);
                                    }
                                    else
                                    {
                                        // entry level...
                                        if ((*arglist)->nested)
                                        {
                                            // an initializer list for an array or structure
                                            pushArrayOrStruct(*it, *(*arglist)->nested, tempmap);
                                        }
                                        else if ((*it)->tp->IsRef())
                                        {
                                            if ((*it)->tp->BaseType()->btp->IsPtr())
                                            {
                                                // could be either a void node with an assign or initialized data...
                                                pushArrayOrStruct(*it, exp, tempmap);
                                            }
                                            else
                                            {
                                                // this could be a void... sequence for reference or struct, or an auto with initializers 
                                                pushStruct(*it, exp, tempmap);
                                            }
                                        }
                                        else if ((*it)->tp->IsStructured())
                                        {
                                            // this could be a void... sequence for reference or struct, or an auto with initializers 
                                            pushStruct(*it, exp, tempmap);
                                        }
                                        else if ((*it)->tp->IsPtr())
                                        {
                                            // could be either a void node with an assign or initialized data...
                                            pushArrayOrStruct(*it, exp, tempmap);
                                        }
                                        else
                                        {
                                            tempmap[*it] = MakeVarPtr(false, 1, 1, (*it), exp);
                                        }
                                    }
                                    ++it;
                                    ++arglist;
                                }
                            }
                            for (auto s : tempmap)
                            {
                                argmap[s.first] = s.second;
                                if (!nestedMaps.empty())
                                    (*nestedMaps.top())[s.first] = s.second;
                            }
                            nestedMaps.push(&argmap);
                            EXPRESSION* ths = nullptr;
                            if (found1->sb->isConstructor)
                            {
                                ths = ConstExprInitializeMembers(found1, node->v.func->thisptr, *node->v.func->arguments);
                            }
                            else if (node->v.func->thisptr)
                            {
                                ths = node->v.func->thisptr;
                                if (ths->type != ExpressionNode::cvarpointer_)
                                {
                                    auto ths1 = node->v.func->thisptr;
                                    while (ths1->type == ExpressionNode::comma_) ths1 = ths1->right;
                                    ths = LookupStruct(ths1);
                                    if (ths == (EXPRESSION*)-1)
                                        return false;
                                    if (!ths)
                                    {
                                        if (ths1->type == ExpressionNode::auto_ || ths1->type == ExpressionNode::global_)
                                            ths = MakeVarPtr(false, node->v.func->thistp->BaseType()->btp->size, 1, ths1->v.sp, nullptr);
                                        else
                                            ths = MakeVarPtr(false, node->v.func->thistp->BaseType()->btp->size, 1, nullptr, nullptr);
                                        if (!pushThis(node->v.func->thisptr, ths))
                                            ths = nullptr;
                                    }
                                }
                            }
                            if (node->v.func->returnEXP && node->v.func->returnEXP->type == ExpressionNode::auto_)
                            {
                                argmap[node->v.func->returnEXP->v.sp] = MakeVarPtr(false, node->v.func->returnSP->tp->size, 1, node->v.func->returnEXP->v.sp, nullptr);
                            }
                            EXPRESSION* newNode = nullptr;
                            if (!ths || !structLevel)
                            {
                                newNode = EvaluateStatements(node, (*its)->lower, ths, nullptr);
                                rv = !!newNode;
                            }
                            if (rv && functionNestingCount == 1)
                            {
                                newNode = EvaluateExpression(newNode, ths, nullptr, true);
                                optimize_for_constants(&newNode);
                                if (node->v.func->sp->sb->isConstructor)
                                {
                                    newNode = InstantiateStruct(node->v.func->thistp->BaseType()->btp, node->v.func->thisptr, newNode);
                                    rv = !!newNode;
                                }
                                else if (node->v.func->returnEXP && !node->v.func->returnSP->tp->BaseType()->sp->sb->structuredAliasType)
                                {
                                    newNode = InstantiateStruct(node->v.func->returnSP->tp, node->v.func->returnEXP, newNode);
                                    rv = !!newNode;
                                }
                                expressionNesting = 0;
                            }
                            nestedMaps.pop();
                            if (rv && nestedMaps.size() && node->v.func->sp->sb->isConstructor)
                            {
                                int offset = 0;
                                auto rel = relptr(node->v.func->thisptr, offset);
                                if (!offset && rel && rel->type == ExpressionNode::auto_)
                                {
                                    (*nestedMaps.top())[rel->v.sp] = copy_expression(newNode);
                                }
                            }
                            if (rv)
                            {
                                *node = *newNode;
                            }
                        }
                        --functionNestingCount;
                    }
                }
            }
        }
    }
    return rv;
}
}  // namespace Parser