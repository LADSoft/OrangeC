/* Software License Agreement
 *
 *     Copyright(C) 1994`-2025 David Lindauer, (LADSoft)
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
 *     GNU General Public License for more details.hj
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
#include <algorithm>
#include <stack>
#include <list>
#include <set>
#include <functional>
#include <unordered_map>
#include <array>
#include "config.h"
#include "OptUtils.h"
#include "lex.h"
#include "help.h"
#include "types.h"
#include "initbackend.h"
#include "Utils.h"
#include "templatedecl.h"
#include "templateinst.h"
#include "templateutil.h"
#include "sha1.h"
#include "templateHash.h"
#include "mangle.h"
#include "constopt.h"
#include "overload.h"

#define PUTCH(ch) { unsigned char a = ch; SHA1Input(&context,&a, sizeof(a)); }
#define PUTSTRING(str) SHA1Input(&context, (const unsigned char *)str, strlen(str));
#define PUTTYPE(type) {unsigned a = (unsigned)type; SHA1Input(&context, (const unsigned char *)&a, sizeof(a)); }

namespace
{
    using namespace Parser;

    std::unordered_map<std::array<unsigned char, SHA1_DIGEST_SIZE>, SYMBOL*, OrangeC::Utils::fnv1a32_arr<SHA1_DIGEST_SIZE>,
        OrangeC::Utils::arr_eql<SHA1_DIGEST_SIZE>> classHash, functionHash;

    void hashType(DotNetPELib::SHA1Context& context, Type* tp, EXPRESSION* exp, bool first);
    int uniqueId;
    void hashNameSpaces(DotNetPELib::SHA1Context& context, SYMBOL* sym)
    {
        if (!sym)
            return;
        hashNameSpaces(context, sym->sb->parentNameSpace);
        if (sym->sb->parentNameSpace && !strcmp(sym->name, "__1") && !strcmp(sym->sb->parentNameSpace->name, "std"))
            return;
        PUTSTRING(sym->name);
    }

    void hashTemplate(DotNetPELib::SHA1Context& context, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params, bool bydflt = true);

    void getName(DotNetPELib::SHA1Context& context, SYMBOL* sym);

    void hashParent(DotNetPELib::SHA1Context& context, SYMBOL* sym)
    {
        PUTSTRING(sym->sb->parent->name);
        if (sym->sb->parent->sb->templateLevel && sym->sb->parent->templateParams)
        {
            hashTemplate(context, sym->sb->parent, sym->sb->parent->templateParams);
        }
    }

    void hashClasses(DotNetPELib::SHA1Context& context, SYMBOL* sym)
    {
        if (!sym)
            return;
        if (sym->sb->parentClass)
            hashClasses(context, sym->sb->parentClass);
        if (sym->sb->parent)
        {
            hashParent(context, sym);
        }
        if (sym->sb->castoperator)
        {
            PUTCH('@');
        }
        else if (sym->sb->templateLevel && sym->templateParams)
        {
            PUTCH('@');
            hashTemplate(context, sym, sym->templateParams);
        }
        else
        {
            PUTSTRING(sym->name);
        }
    }

    void hashExpressionInternal(DotNetPELib::SHA1Context& context, EXPRESSION* exp)
    {
        while (IsCastValue(exp))
            exp = exp->left;
        if (isintconst(exp))
        {
            char buf[64];
            buf[0] = 0;
            if (exp->type == ExpressionNode::const_)
            {
                Optimizer::my_sprintf(buf, sizeof(buf), "%lld?", exp->v.sp->sb->value.i);
            }
            else
            {
                Optimizer::my_sprintf(buf, sizeof(buf), "%lld?", exp->v.i);
            }
            PUTSTRING(buf);
        }
        else
        {
            bool nonpointer = false;
            while (IsLValue(exp))
            {
                nonpointer = true;
                exp = exp->left;
            }
            switch (exp->type)
            {
            case ExpressionNode::nullptr_:
                PUTCH('n');
                break;
            case ExpressionNode::arrayadd_:
            case ExpressionNode::structadd_:
            case ExpressionNode::add_:
                PUTCH('p');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::sub_:
                PUTCH('s');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::mul_:
            case ExpressionNode::umul_:
            case ExpressionNode::arraymul_:
                PUTCH('m');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::umod_:
            case ExpressionNode::mod_:
                PUTCH('o');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::dot_:
                PUTCH('D');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::pointsto_:
                PUTCH('P');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::div_:
            case ExpressionNode::udiv_:
            case ExpressionNode::arraydiv_:
                PUTCH('d');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::lsh_:
            case ExpressionNode::arraylsh_:
                PUTCH('h');
                PUTCH('l');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::rsh_:
            case ExpressionNode::ursh_:
                PUTCH('h');
                PUTCH('r');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::hook_:
                PUTCH('C');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right->left);
                hashExpressionInternal(context, exp->right->right);
                break;
            case ExpressionNode::assign_:
                PUTCH('a');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::eq_:
                PUTCH('c');
                PUTCH('e');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::ne_:
                PUTCH('c');
                PUTCH('n');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::uminus_:
                PUTCH('u');
                hashExpressionInternal(context, exp->left);
                break;
            case ExpressionNode::not_:
                PUTCH('l');
                PUTCH('n');
                hashExpressionInternal(context, exp->left);
                break;
            case ExpressionNode::compl_:
                PUTCH('b');
                PUTCH('n');
                hashExpressionInternal(context, exp->left);
                break;
            case ExpressionNode::ult_:
            case ExpressionNode::lt_:
                PUTCH('c');
                PUTCH('l');
                PUTCH('t');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::ule_:
            case ExpressionNode::le_:
                PUTCH('c');
                PUTCH('l');
                PUTCH('e');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::ugt_:
            case ExpressionNode::gt_:
                PUTCH('c');
                PUTCH('g');
                PUTCH('t');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::uge_:
            case ExpressionNode::ge_:
                PUTCH('c');
                PUTCH('g');
                PUTCH('e');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::and_:
                PUTCH('b');
                PUTCH('a');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::land_:
                PUTCH('l');
                PUTCH('a');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::or_:
                PUTCH('b');
                PUTCH('o');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::lor_:
                PUTCH('l');
                PUTCH('o');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::xor_:
                PUTCH('b');
                PUTCH('x');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::auto_inc_:
                PUTCH('i');
                PUTCH('p');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::auto_dec_:
                PUTCH('i');
                PUTCH('s');
                hashExpressionInternal(context, exp->left);
                hashExpressionInternal(context, exp->right);
                break;
            case ExpressionNode::select_:
                hashExpressionInternal(context, exp->left);
                break;
            case ExpressionNode::templateselector_: {
                auto tsl = exp->v.templateSelector;
                SYMBOL* ts = (*tsl)[1].sp;
                PUTCH('t');
                PUTCH('s');
                if ((*tsl)[1].isTemplate && (*tsl)[1].templateParams)  // may be an empty variadic
                {
                    hashTemplate(context, ts, (*tsl)[1].templateParams);
                }
                auto find = (*tsl).begin();
                ++find;
                ++find;
                for (; find != (*tsl).end(); ++find)
                {
                    PUTCH('t');
                    if (find->isTemplate && find->templateParams)
                    {
                        SYMBOL s = {};
                        s.name = find->name;
                        s.tp = &stdint;
                        hashTemplate(context, &s, find->templateParams);
                    }
                    else
                    {
                        PUTSTRING(find->name);
                    }
                }
                break;
            }
            case ExpressionNode::templateparam_:
            case ExpressionNode::auto_:
                PUTCH('t');
                PUTCH('p');
                PUTSTRING(exp->v.sp->name);
                break;
            case ExpressionNode::thisref_:
            case ExpressionNode::funcret_:
            case ExpressionNode::constexprconstructor_:
                hashExpressionInternal(context, exp->left);
                break;
            case ExpressionNode::callsite_: 
            {
                if (exp->v.func->ascall)
                {
                    PUTCH('f');
                    getName(context, exp->v.func->sp);
                    if (exp->v.func->arguments)
                    {
                        for (auto arg : *exp->v.func->arguments)
                            if (arg->exp)
                            {
                                PUTCH('F');
                                hashExpressionInternal(context, arg->exp);
                            }
                    }
                }
                else
                {
                    PUTCH('e');
                    PUTCH('?');
                    getName(context, exp->v.func->sp);
                    PUTCH('.');
                    hashType(context, exp->v.func->sp->tp, nullptr, true);
                }
                break;
            }
            case ExpressionNode::pc_:
            case ExpressionNode::global_:
            case ExpressionNode::const_:
                if (exp->v.sp->tp->IsFunction())
                {
                    PUTCH('e');
                    PUTCH('?');
                    PUTSTRING(exp->v.sp->name);
                    PUTCH('.');
                    hashType(context, exp->v.sp->tp, nullptr, true);
                }
                else
                {
                    PUTCH('g');
                    if (!nonpointer)
                        PUTCH('?');
                    PUTSTRING(exp->v.sp->name);
                    PUTCH('.');
                }
                break;
            case ExpressionNode::sizeofellipse_:
                PUTCH('z');
                getName(context, exp->v.templateParam->first);
                break;
            case ExpressionNode::comma_:
                PUTCH('v');
                // ignoring args to void...
                break;
            default:
                break;
            }
        }
    }

    void hashExpression(DotNetPELib::SHA1Context& context, EXPRESSION* exp)
    {
        if (exp)
        {
            PUTCH('?');
            hashExpressionInternal(context, exp);
        }
    }

    void hashTemplate(DotNetPELib::SHA1Context& context, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params, bool byDflt)
    {
        bool bySpecial = false;
        if (params && params->size() && params->front().second->type == TplType::new_ &&
            ((sym->sb && (sym->sb->instantiated && !sym->sb->templateLevel)) || (params && params->front().second->bySpecialization.types)))
        {
            params = params->front().second->bySpecialization.types;
            bySpecial = true;
        }
        if (sym->tp->type == BasicType::templateparam_ && sym->tp->templateParam->second->type == TplType::template_)
        {
            auto sp = sym->tp->templateParam->second->byTemplate.val;
            if (sp)
                sym = sp;
        }
        if (byDflt)
        {
            PUTSTRING(sym->name);
        }
        else if (sym->sb && (sym->sb->isConstructor || sym->sb->isDestructor) &&
            sym->sb->templateLevel == sym->sb->parentClass->sb->templateLevel)
        {
            PUTSTRING(sym->name);
            PUTCH('.');
        }
        else
        {
            PUTCH('#');
            PUTSTRING(sym->name);
            PUTCH('.');
        }
        static StackList<std::list<TEMPLATEPARAMPAIR>::iterator> nestedStack;
        NestedStack<std::list<TEMPLATEPARAMPAIR>::iterator> tps(nestedStack);

        if (params)
        {
            auto it = params->begin();
            auto ite = params->end();
            for (; it != ite;)
            {
                switch (it->second->type)
                {
                case TplType::typename_:
                    if (it->second->packed)
                    {
                        if (it->second->byPack.pack)
                        {
                            for (auto pack : *it->second->byPack.pack)
                                hashType(context, pack.second->byClass.val ? pack.second->byClass.val : pack.second->byClass.dflt, nullptr, true);
                        }
                        else
                        {
                            PUTCH('e');
                            getName(context, it->first);
                        }
                    }
                    else if (bySpecial)
                    {
                        if (it->second->byClass.val)
                        {
                            hashType(context, it->second->byClass.val, nullptr, true);
                        }
                        else if (it->second->byClass.dflt)
                        {
                            hashType(context, it->second->byClass.dflt, nullptr, true);
                        }
                        else
                        {
                            getName(context, it->first);
                        }
                    }
                    else if (sym->sb && sym->sb->instantiated && it->second->byClass.val)
                    {
                        hashType(context, it->second->byClass.val, nullptr, true);
                    }
                    else
                    {
                        if (it->second->byClass.dflt)
                        {
                            hashType(context, it->second->byClass.dflt, nullptr, true);
                        }
                        else if (byDflt && it->second->byClass.val)
                        {
                            hashType(context, it->second->byClass.val, nullptr, true);
                        }
                        else
                        {
                            getName(context, it->first);
                        }
                    }
                    break;
                case TplType::template_:
                    if (sym->sb && sym->sb->instantiated && it->second->byTemplate.val)
                    {
                        hashTemplate(context, it->second->byTemplate.val, it->second->byTemplate.val->templateParams);
                    }
                    else if (it->first)
                    {
                        getName(context, it->first);
                    }
                    else
                    {
                        getName(context, it->second->byTemplate.dflt);
                    }
                    break;
                case TplType::int_:
                    if (it->second->packed)
                    {
                        if (it->second->byPack.pack)
                        {
                            tps.push(it);
                            tps.push(ite);
                            ite = it->second->byPack.pack->end();
                            it = it->second->byPack.pack->begin();
                            continue;
                        }
                    }
                    else
                    {
                        if (it->second->byNonType.tp->IsInt())
                        {
                            PUTCH('i');
                        }
                        else if (it->second->byNonType.tp->IsFloat())
                        {
                            PUTCH('f');
                        }
                        else
                        {
                            hashType(context, it->second->byNonType.tp, nullptr, true);
                        }
                        if ((bySpecial || sym->sb && sym->sb->instantiated) && it->second->byNonType.val)
                        {
                            EXPRESSION* exp =
                                bySpecial && it->second->byNonType.dflt ? it->second->byNonType.dflt : it->second->byNonType.val;
                            hashExpression(context, exp);
                        }
                        else if (it->second->byNonType.dflt)
                        {
                            hashExpression(context, it->second->byNonType.dflt);
                        }
                    }
                    break;
                default:
                    break;
                }
                ++it;
                if (it == ite && !tps.empty())
                {
                    ite = tps.top();
                    tps.pop();
                    it = tps.top();
                    tps.pop();
                    ++it;
                }
            }
        }
        if (!byDflt)
            PUTCH('~');
    }

    void getName(DotNetPELib::SHA1Context& context, SYMBOL* sym)
    {
        if (!sym)
        {
            PUTSTRING("????");
        }
        else if (!sym->sb)
        {
            PUTSTRING(sym->name);
        }
        else
        {
            hashNameSpaces(context, sym->sb->parentNameSpace);
            hashClasses(context, sym->sb->parentClass);
            if (sym->sb->parent)
            {
                hashParent(context, sym);
            }
            PUTCH('@');
            if (sym->sb->templateLevel && sym->templateParams)
            {
                hashTemplate(context, sym, sym->templateParams);
            }
            else
            {
                PUTSTRING(sym->name);
            }
        }
    }

    void hashType(DotNetPELib::SHA1Context& context, Type* tp, EXPRESSION* exp, bool first)
    {
        int i;
        if (!tp)
        {
            PUTSTRING("initializer-list");
            return;
        }
        while (tp)
        {
            while (tp->type == BasicType::typedef_ && !tp->sp->templateParams)
                tp = tp->btp;
            if (tp->type == BasicType::typedef_)
            {
                {
                    if (tp->IsConst())
                        PUTCH('x');
                    if (tp->IsVolatile())
                        PUTCH('y');
                    if (tp->IsLRefQual())
                        PUTCH('r');
                    if (tp->IsRRefQual())
                        PUTCH('R');
                }
                hashTemplate(context, tp->sp, tp->sp->templateParams);
                return;
            }
            if (tp->IsStructured() && tp->BaseType()->sp->sb && tp->BaseType()->sp->sb->templateLevel)
            {
                {
                    if (tp->IsConst())
                        PUTCH('x');
                    if (tp->IsVolatile())
                        PUTCH('y');
                    if (tp->IsLRefQual())
                        PUTCH('r');
                    if (tp->IsRRefQual())
                        PUTCH('R');
                    if (exp)
                    {
                        bool lref = false, rref = false;
                        GetRefs(nullptr, tp, exp, lref, rref);
                        if (lref)
                            PUTCH('s');
                        if (rref)
                            PUTCH('S');
                    }
                }
                hashClasses(context, tp->BaseType()->sp->sb->parentClass);
                if (tp->BaseType()->sp->sb->parent)
                {
                    hashParent(context, tp->BaseType()->sp);
                }
                hashTemplate(context, tp->BaseType()->sp, tp->BaseType()->sp->templateParams);
                return;
            }
            else
            {
                if (tp->IsConst())
                    PUTCH('x');
                if (tp->IsVolatile())
                    PUTCH('y');
                if (tp->IsLRefQual())
                    PUTCH('r');
                if (tp->IsRRefQual())
                    PUTCH('R');
                if (exp)
                {
                    if (tp->lref)
                        PUTCH('s');
                    if (tp->rref)
                        PUTCH('S');
                }
                tp = tp->BaseType();
                if (exp)
                {
                    if (tp->lref)
                        PUTCH('s');
                    if (tp->rref)
                        PUTCH('S');
                }
                if (tp->IsInt() && tp->btp && tp->btp->type == BasicType::enum_)
                    tp = tp->btp;
                switch (tp->type)
                {
                    case BasicType::derivedfromtemplate_:
                    case BasicType::typedef_:
                    case BasicType::templateparam_:
                        break;
                    case BasicType::func_:
                    case BasicType::ifunc_:
                        PUTTYPE(BasicType::func_);
                    default:
                        PUTTYPE(tp->type);
                        break;
                }
                switch (tp->type)
                {
                case BasicType::func_:
                case BasicType::ifunc_:
                    if (tp->BaseType()->sp && ismember(tp->BaseType()->sp) && !first)
                    {
                        PUTCH('M');
                        getName(context, tp->sp->sb->parentClass);
                    }
                    PUTCH('q');
                    for (auto sym : *tp->syms)
                    {
                        if (!sym->sb->thisPtr)
                            hashType(context, sym->tp, exp, true);
                    }
                    PUTCH('.');
                    // return value comes next
                    break;
                case BasicType::memberptr_:
                    PUTCH('M');
                    getName(context, tp->sp);
                    if (tp->btp->IsFunction())
                    {
                        PUTCH('q');
                        for (auto sym : *tp->btp->BaseType()->syms)
                        {
                            if (!sym->sb->thisPtr)
                                hashType(context, sym->tp, exp, true);
                        }
                        PUTCH('.');
                        tp = tp->btp;  // so we can get to tp->btp->btp
                    }
                    else
                    {
                        PUTCH('.');
                    }
                    break;
                case BasicType::enum_:
                case BasicType::struct_:
                case BasicType::union_:
                case BasicType::class_:
                    getName(context, tp->sp);
                    return;
                case BasicType::unsigned_bitint_:
                case BasicType::bitint_:
                {
                    char buf[64];
                    Optimizer::my_sprintf(buf, sizeof(buf), "%d?", tp->bitintbits);
                    PUTSTRING(buf);
                    break;
                case BasicType::pointer_:
                    if (tp->nullptrType)
                    {
                        PUTSTRING("nullptr_t");
                        return;
                    }
                    else
                    {
                        if (!tp->array)
                        {
                            PUTCH('p');
                        }
                        else
                        {
                            PUTCH('A');
                        }
                    }
                    break;
                case BasicType::templateparam_:
                    if (tp->templateParam->second->type == TplType::typename_)
                    {
                        if (tp->templateParam->second->packed)
                        {
                            if (tp->templateParam->second->byPack.pack)
                            {
                                for (auto&& tpl : *tp->templateParam->second->byPack.pack)
                                {
                                    auto dflt = tpl.second->byClass.dflt;
                                    if (!dflt)
                                        dflt = tpl.second->byClass.val;
                                    auto tp1 = dflt;
                                    while (tp1->IsRef() || tp1->IsPtr())
                                        tp1 = tp1->BaseType()->btp;
                                    if (tp1 && tp1->BaseType()->type != BasicType::templateparam_)
                                    {
                                        hashType(context, dflt, exp, false);
                                    }
                                    else
                                    {
                                        getName(context, tpl.first);
                                    }

                                }
                            }
                        }
                        else
                        {
                            auto dflt = tp->templateParam->second->byClass.dflt;
                            if (!dflt)
                                dflt = tp->templateParam->second->byClass.val;
                            if (dflt && dflt->BaseType()->type != BasicType::templateparam_)
                            {
                                hashType(context, dflt, exp, false);
                            }
                            else
                            {
                                getName(context, tp->templateParam->first);
                            }
                        }
                    }
                    else
                    {
                        getName(context, tp->templateParam->first);
                    }
                    break;
                case BasicType::templateselector_: {
                    auto s = (*tp->sp->sb->templateSelector).begin();
                    auto se = (*tp->sp->sb->templateSelector).end();
                    ++s;
                    if (s->isTemplate)
                        hashTemplate(context, s->sp, s->sp->sb && s->sp->sb->instantiated ? s->sp->templateParams : s->templateParams);
                    else
                        getName(context, s->sp);
                    for (++s; s != se; ++s)
                    {
                        PUTCH('@');
                        if (s->isTemplate && s->templateParams)
                        {
                            SYMBOL s2 = {};
                            s2.name = s->name;
                            s2.tp = &stdint;
                            hashTemplate(context, &s2, s->templateParams);
                        }
                        else
                        {
                            PUTSTRING(s->name);
                        }
                    }
                    return;
                }
                                                 break;
                case BasicType::templatedecltype_:
                    PUTCH('E');
                    hashExpression(context, tp->templateDeclType);
                    break;
                case BasicType::aggregate_:
                    getName(context, tp->sp);
                    break;
                case BasicType::templatedeferredtype_:
                    // we want something the linkerwill treat as an error,
                    // so we can detect problems with these not being properly replaced
                    PUTCH(MANGLE_DEFERRED_TYPE_CHAR);
                    break;
                default:
                    break;
                }
                }
                tp = tp->btp;
            }
        }
    }
    void hashCandidates(DotNetPELib::SHA1Context& context,  SymbolTable<SYMBOL>* syms)
    { 
        struct HashIndex
        {
            std::string name;
            int declline;
            const char* declfile;
        };
        struct HashCompare
        {
            bool operator()(const HashIndex& left, const HashIndex& right) const
            {
                if (left.declline < right.declline)
                {
                    return true;
                }
                else if (left.declline == right.declline)
                {
                    int n = strcmp(left.declfile, right.declfile);
                    if (n < 0)
                        return left.name < right.name;
                }
                return false;
            }
        };
        if (syms->size())
        {
            std::set<HashIndex, HashCompare> hashSet;
            for (auto s : *syms)
            {
                if (s->sb)
                {
                    hashSet.insert(HashIndex{ s->name, s->sb->declline, s->sb->declfile });
                }
                else
                {
                    hashSet.insert(HashIndex{ s->name, -1, ""});
                }
            }
            for (auto&& s : hashSet)
            {
                DotNetPELib::SHA1Input(&context, (const unsigned char*)&s.declline, sizeof(s.declline));
                DotNetPELib::SHA1Input(&context, (const unsigned char*)s.name.c_str(), s.name.size());
            }
        }
    }
    void hashCandidates(DotNetPELib::SHA1Context& context, SYMBOL* sym)
    {
        SymbolTable<SYMBOL> syms;
        syms.Add(sym);
        if (sym->sb->specializations)
        {
            for (auto elem : *sym->sb->specializations)
            {
                syms.AddName(elem);
            }
        }
        hashCandidates(context, &syms);
    }
     void hashTemplateParents(DotNetPELib::SHA1Context& context, SYMBOL* sym)
    {
        SYMBOL* lastParent = sym->sb->parent ? sym->sb->parent : sym;
        while (lastParent->sb->parentClass)
            lastParent = lastParent->sb->parentClass;
        hashNameSpaces(context, lastParent->sb->parentNameSpace);
        hashClasses(context, (sym->sb->parent ? sym->sb->parent : sym)->sb->parentClass);
        if (sym->sb->parent)
        {
            hashParent(context, sym);
        }
    }
    void hashFunctionArgs(DotNetPELib::SHA1Context& context, std::list<Argument*>* args)
    {
        if (!args)
            return;
        for (auto arg : *args)
        {
            if (!arg->tp)
            {
                hashFunctionArgs(context, arg->nested);
            }
            else
            {
                hashType(context, arg->tp, arg->exp, true);
            }
        }
    }
    bool hasAllArgs(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params);
    bool checkDfltType(Type* tp)
    {
        while (tp->IsPtr() || tp->IsRef())
            tp = tp->BaseType()->btp;
        if (tp->IsFunction())
        {
            SYMBOL* sym = tp->BaseType()->sp;
            if (!checkDfltType(tp->BaseType()->btp))
                return false;
            if (sym->tp->syms)
            {
                for (auto sp1 : *sym->tp->syms)
                {
                    if (!checkDfltType(sp1->tp))
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
                    return hasAllArgs(nullptr, tp->BaseType()->sp->templateParams);
                }
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
            if (!checkDfltType(tp->BaseType()->sp->tp))
                return false;
            if (!checkDfltType(tp->BaseType()->btp))
                return false;
        }
        else if (tp->type == BasicType::typedef_ && tp->sp->templateParams)
        {
            return hasAllArgs(nullptr, tp->sp->templateParams);
        }
        return true;
    }
    bool checkDfltSpecified(TEMPLATEPARAMPAIR* arg)
    {
        if (!arg->second->byClass.dflt)
            return false;
        switch (arg->second->type)
        {
        case TplType::int_:
            if (arg->second->byNonType.dflt && !isarithmeticconst(arg->second->byNonType.dflt))
            {
                EXPRESSION* exp = copy_expression(arg->second->byNonType.dflt);
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
            return checkDfltType(arg->second->byNonType.tp);
        case TplType::template_: {
            return true;
        }
        case TplType::typename_: {
            return checkDfltType(arg->second->byClass.dflt);
        }
        default:
            break;
        }
        return true;
    }
    bool hasAllArgs(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params)
    {
        if (params)
        {
            for (auto it = params->begin(); it != params->end(); ++it)
            {
                if (it->second->type != TplType::new_)
                {
                    if (it->second->packed)
                    {
                        if ((IsDefiningTemplate() &&
                            (!it->second->byPack.pack || !it->second->byPack.pack->size())) ||
                            !hasAllArgs(nullptr, it->second->byPack.pack))
                            return false;
                    }
                    else
                    {
                        if (sym)
                        {
                            if (it->second->type == TplType::typename_)
                            {
                                Type* tp = it->second->byClass.dflt;
                                if (tp && tp->BaseType()->type == BasicType::any_)
                                    return false;
                                if (SameTemplate(tp, sym->tp))
                                    return false;
                            }
                        }
                        if (!checkDfltSpecified(&*it))
                        {
                            return false;
                        }
                    }
                }
            }
        }
        if (sym)
        {
            for (sym = sym->sb->parentClass; sym; sym = sym->sb->parentClass)
            {
                if (sym->templateParams)
                    if (!allTemplateArgsSpecified(sym, sym->templateParams))
                        return false;
            }
        }
        return true;
    }
} // anonymous namespace
namespace Parser
{
SYMBOL* LookupTemplateClass(DotNetPELib::SHA1Context& context, SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* params)
{
    context.Computed = false;
    if (hasAllArgs(sym, params))
    {
        DotNetPELib::SHA1Reset(&context);
        PUTCH('@');
        PUTSTRING(sym->name);
        hashTemplate(context, sym, params, true);
        hashCandidates(context, sym);
        hashTemplateParents(context, sym);
        DotNetPELib::SHA1Result(&context);
        std::array<unsigned char, SHA1_DIGEST_SIZE> array;
        std::copy(context.Message_Digest_Bytes, context.Message_Digest_Bytes + SHA1_DIGEST_SIZE, array.begin());
        auto it = classHash.find(array);
        if (it != classHash.end())
        {
            return it->second;
        }
    }
    return nullptr;
}
SYMBOL* LookupGeneratedTemplateClass(DotNetPELib::SHA1Context& context, SYMBOL* sym)
{
    DotNetPELib::SHA1Reset(&context);
    PUTSTRING(sym->sb->decoratedName);
    DotNetPELib::SHA1Result(&context);
    std::array<unsigned char, SHA1_DIGEST_SIZE> array;
    std::copy(context.Message_Digest_Bytes, context.Message_Digest_Bytes + SHA1_DIGEST_SIZE, array.begin());
    auto it = classHash.find(array);
    if (it != classHash.end())
    {
        return it->second;
    }
    return nullptr;
}
void RegisterTemplateClass(DotNetPELib::SHA1Context& context, SYMBOL* cls)
{
    if (context.Computed)
    {
        std::array<unsigned char, SHA1_DIGEST_SIZE> array;
        std::copy(context.Message_Digest_Bytes, context.Message_Digest_Bytes + SHA1_DIGEST_SIZE, array.begin());
        classHash[array] = cls;
    }
}
SYMBOL* LookupTemplateFunction(DotNetPELib::SHA1Context& context, SYMBOL* sym, std::list<SYMBOL*>* gather, CallSite* callSite)
{
    context.Computed = false;
    if (!IsDefiningTemplate())
    {
        DotNetPELib::SHA1Reset(&context);
        PUTCH('@');
        PUTSTRING(sym->name);
        if (callSite->templateParams)
            hashTemplate(context, sym, callSite->templateParams);
        if (callSite->arguments)
            hashFunctionArgs(context, callSite->arguments);
        if (callSite->thistp)
        {
            hashType(context, callSite->thistp, callSite->thisptr, true);
        }
        if (callSite->returnSP)
        {
            hashType(context, callSite->returnSP->tp, callSite->returnEXP, true);
        }
        for (auto&& overloadHolder : *gather)
        {
            for (auto func : *overloadHolder->tp->BaseType()->syms)
            {
                if (!func->sb->instantiated)
                {
                    hashTemplateParents(context, func);
                    hashType(context, func->tp->BaseType()->btp, nullptr, true);
                    for (auto arg : *func->tp->BaseType()->syms)
                    {
                        hashType(context, arg->tp, nullptr, true);
                    }
                }
            }
        }
        DotNetPELib::SHA1Result(&context);
        std::array<unsigned char, SHA1_DIGEST_SIZE> array;
        std::copy(context.Message_Digest_Bytes, context.Message_Digest_Bytes + SHA1_DIGEST_SIZE, array.begin());
        auto it = functionHash.find(array);
        if (it != functionHash.end())
        {
            return it->second;
        }
    }
    return nullptr;
}
void RegisterTemplateFunction(DotNetPELib::SHA1Context& context, SYMBOL* func)
{
    if (context.Computed)
    {
        std::array<unsigned char, SHA1_DIGEST_SIZE> array;
        std::copy(context.Message_Digest_Bytes, context.Message_Digest_Bytes + SHA1_DIGEST_SIZE, array.begin());
        functionHash[array] = func;
    }
}
void templateHashInit()
{
    uniqueId = 0;
    classHash.clear();
    functionHash.clear();
}

}  // namespace Parser