/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be 
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
#include <stack>
#include "mangle.h"
#include "initbackend.h"
#include "cpplookup.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "declcpp.h"
#include "ccerr.h"
#include "declcons.h"
#include "lex.h"
#include "help.h"
#include "expr.h"
#include "declare.h"
#include "beinterf.h"
#include "types.h"
#include "memory.h"
#include "occparse.h"
#include "symtab.h"
#include "ListFactory.h"
#include "exprcpp.h"

namespace Parser
{
int inNoExceptHandler;


typedef bool INTRINS_FUNC(EXPRESSION* exp);
static bool is_abstract(EXPRESSION* exp);
static bool is_base_of(EXPRESSION* exp);
static bool is_class(EXPRESSION* exp);
static bool is_constructible(EXPRESSION* exp);
static bool is_convertible_to(EXPRESSION* exp);
static bool is_empty(EXPRESSION* exp);
static bool is_enum(EXPRESSION* exp);
static bool is_final(EXPRESSION* exp);
static bool is_literal(EXPRESSION* exp);
static bool is_nothrow_constructible(EXPRESSION* exp);
static bool is_nothrow_assignable(EXPRESSION* exp);
static bool is_pod(EXPRESSION* exp);
static bool is_polymorphic(EXPRESSION* exp);
static bool is_standard_layout(EXPRESSION* exp);
static bool is_trivial(EXPRESSION* exp);
static bool is_trivially_assignable(EXPRESSION* exp);
static bool is_trivially_constructible(EXPRESSION* exp);
static bool is_trivially_destructible(EXPRESSION* exp);
static bool is_trivially_copyable(EXPRESSION* exp);
static bool is_union(EXPRESSION* exp);
static bool is_literal_type(EXPRESSION* exp);
static bool is_same(EXPRESSION* exp);
static bool is_assignable(EXPRESSION* exp);
static bool is_aggregate(EXPRESSION* exp);
static bool is_member_pointer(EXPRESSION* exp);
static bool has_unique_object_representations(EXPRESSION* exp);
static bool has_virtual_destructor(EXPRESSION* exp);

static std::unordered_map<std::string, INTRINS_FUNC*> intrinsicHash {
    {"__has_unique_object_representations", has_unique_object_representations},
    {"__has_virtual_destructor", has_virtual_destructor},
    {"__is_abstract", is_abstract},
    {"__is_aggregate", is_aggregate},
    {"__is_assignable", is_assignable},
    {"__is_base_of", is_base_of},
    {"__is_class", is_class},
    {"__is_constructible", is_constructible},
    {"__is_convertible_to", is_convertible_to},
    {"__is_empty", is_empty},
    {"__is_enum", is_enum},
    {"__is_final", is_final},
    {"__is_literal", is_literal},
    {"__is_literal_type", is_literal_type},
    {"__is_member_pointer", is_member_pointer},
    {"__is_nothrow_constructible", is_nothrow_constructible},
    {"__is_nothrow_assignable", is_nothrow_assignable},
    {"__is_pod", is_pod},
    {"__is_polymorphic", is_polymorphic},
    {"__is_same", is_same},
    {"__is_standard_layout", is_standard_layout},
    {"__is_trivial", is_trivial},
    {"__is_trivially_assignable", is_trivially_assignable},
    {"__is_trivially_constructible", is_trivially_constructible},
    {"__is_trivially_destructible", is_trivially_destructible},
    {"__is_trivially_copyable", is_trivially_copyable},
    {"__is_union", is_union},
};
static std::unordered_map<std::string, std::unordered_map<unsigned, SYMBOL*>, StringHash> integerSequences;

void libcxx_init(void)
{
    int i;
    integerSequences.clear();
}
void libcxx_builtins(void)
{
    if (Optimizer::cparams.prm_cplusplus)
    {
        preProcessor->Define("address_sanitizer", "0");
        preProcessor->Define("__has_cxx_access_control_sfinae", "0");
        preProcessor->Define("__has_cxx_alias_templates", "1");
        preProcessor->Define("__has_cxx_alignas", "1");
        preProcessor->Define("__has_cxx_atomic", "1");
        preProcessor->Define("__has_cxx_attributes", "1");
        preProcessor->Define("__has_cxx_auto_type", "1");
        preProcessor->Define("__has_cxx_constexpr", "1");
        preProcessor->Define("__has_cxx_decltype", "1");
        preProcessor->Define("__has_cxx_defaulted_functions", "1");
        preProcessor->Define("__has_cxx_deleted_functions", "1");
        preProcessor->Define("__has_cxx_exceptions", "1");
        preProcessor->Define("__has_cxx_explicit_conversions", "1");
        preProcessor->Define("__has_cxx_generalized_initializers", "1");
        preProcessor->Define("__has_cxx_lambdas", "1");
        preProcessor->Define("__has_cxx_noexcept", "1");
        preProcessor->Define("__has_cxx_nullptr", "1");
        preProcessor->Define("__has_cxx_reference_qualified_functions", "1");
        preProcessor->Define("__has_cxx_relaxed_constexpr", "1");
        preProcessor->Define("__has_cxx_rtti", "1");
        preProcessor->Define("__has_cxx_rvalue_references", "1");
        preProcessor->Define("__has_cxx_static_assert", "1");
        preProcessor->Define("__has_cxx_strong_enums", "1");
        preProcessor->Define("__has_cxx_trailing_return", "1");
        preProcessor->Define("__has_cxx_variadic_templates", "1");
        preProcessor->Define("__has_nothrow_assign", "1");
        preProcessor->Define("__has_nothrow_constructor", "0");
        preProcessor->Define("__has_nothrow_copy", "0");
        preProcessor->Define("__has_trivial_constructor", "0");
        preProcessor->Define("__has_trivial_destructor", "0");
        preProcessor->Define("__has_virtual_destructor", "1");
        preProcessor->Define("__has_is_base_of", "1");
        preProcessor->Define("__has_is_class", "1");
        preProcessor->Define("__has_is_constructible", "1");
        preProcessor->Define("__has_is_convertible_to", "1");
        preProcessor->Define("__has_is_empty", "1");
        preProcessor->Define("__has_is_enum", "1");
        preProcessor->Define("__has_is_final", "1");
        preProcessor->Define("__has_is_literal", "1");
        preProcessor->Define("__has_is_nothrow_constructible", "1");
        preProcessor->Define("__has_is_nothrow_assignable", "1");
        preProcessor->Define("__has_is_pod", "1");
        preProcessor->Define("__has_is_polymorphic", "1");
        preProcessor->Define("__has_is_standard_layout", "1");
        preProcessor->Define("__has_is_trivial", "1");
        preProcessor->Define("__has_is_trivially_assignable", "1");
        preProcessor->Define("__has_is_trivially_constructible", "1");
        preProcessor->Define("__has_is_trivially_copyable", "1");
        preProcessor->Define("__has_is_union", "1");
        preProcessor->Define("__has___reference_binds_to_temporary", "0");
        preProcessor->Define("__has__is_member_pointer", "1");
        preProcessor->Define("__has___is_trivially_destructible", "1");
        preProcessor->Define("__has__is_trivially_destructible", "1");
        preProcessor->Define("__has___is_nothrow_constructible", "1");
        preProcessor->Define("__has___is_nothrow_assignable", "1");
        preProcessor->Define("__has___nullptr", "0");
        preProcessor->Define("__has__Atomic", "1");
        preProcessor->Define("__has___is_aggregate", "1");
        preProcessor->Define("__has__builtin_isnan", "0");
        preProcessor->Define("__has__builtin_isinf", "0");
        preProcessor->Define("__has__builtin_isfinite", "0");
        preProcessor->Define("__has__atomic_load_n", "1");
        preProcessor->Define("__has__atomic_add_fetch", "1");
        preProcessor->Define("__has__atomic_exchange_n", "1");
        preProcessor->Define("__has__atomic_compare_exchange_n", "1");
        preProcessor->Define("__has__builtin_operator_new", "0");
        preProcessor->Define("__has__builtin_operator_delete", "0");
        preProcessor->Define("__has__make_integer_seq", "1");
        preProcessor->Define("__has__type_pack_element", "1");
        preProcessor->Define("__has__builtin_launder", "0");
        preProcessor->Define("__has__builtin_addressof", "0");
        preProcessor->Define("__has____no_sanitize__", "0");
        preProcessor->Define("__has____type_visibility__", "0");
        preProcessor->Define("__has__internal_linkage", "1");
        preProcessor->Define("__has__exclude_from_explicit_instantiation", "1");
        preProcessor->Define("__has____enable_if__", "0");
        preProcessor->Define("__has__deprecated", "1");
        preProcessor->Define("__has__no_destroy", "0");
        preProcessor->Define("__has__aquire_capability", "0");
        preProcessor->Define("__has__require_constant_initialization", "0");
        preProcessor->Define("__has__diagnose_if", "0");
        preProcessor->Define("__has__fallthrough", "0");
        preProcessor->Define("__has____nodebug__", "0");
        preProcessor->Define("__has__no_thread_safety_analysis", "0");
        preProcessor->Define("__has____using_if_exists__", "0");
        preProcessor->Define("__has__decay", "1");
    }
}
static void GetTypeList(EXPRESSION* exp, std::list<Argument*>** arguments, bool initialize = false);
static LexList* GetTypeList(LexList* lex, SYMBOL* funcsp, std::list<Argument*>** lptr);
void EvaluateLibcxxConstant(EXPRESSION** exp);

bool parseBuiltInTypelistFunc(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    auto it = intrinsicHash.find(sym->name);
    if (it != intrinsicHash.end())
    {
        auto exp1 = MakeExpression(ExpressionNode::cppintrinsic_);
        exp1->v.cppintrinsicName = it->first.c_str();
        *lex = GetTypeList(*lex, funcsp, &(exp1)->v.cppintrinsicArgs);
        *tp = &stdint;
        EvaluateLibcxxConstant(&exp1);
        if (exp1->type == ExpressionNode::c_i_)
            *exp = exp1;
        else
            *exp = MakeIntExpression(ExpressionNode::c_i_, false);
        return true;
    }
    return false;
}
void EvaluateLibcxxConstant(EXPRESSION** exp)
{
    if (!definingTemplate || instantiatingTemplate)
    {
        auto it = intrinsicHash.find((*exp)->v.cppintrinsicName);
        if (it != intrinsicHash.end())
        {
            auto rv = (it->second)(*exp);
            *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
        }
    }
}
static void GetTypeList(EXPRESSION* exp, std::list<Argument*>** arguments, bool initialize)
{
    *arguments = argumentListFactory.CreateList();
    // the assumption is, we won't have anything other than plain template parameters or solid types which aren't templates
    for (auto arg : *exp->v.cppintrinsicArgs)
    {
        if (arg->tp->BaseType()->type == BasicType::templateparam_ && arg->tp->BaseType()->templateParam->first)
        {
            arg->tp = arg->tp->BaseType();
            TEMPLATEPARAMPAIR* tpl = TypeAliasSearch(arg->tp->BaseType()->templateParam->first->name, false);
            if (tpl && tpl->second->packed || tpl->second->byClass.val)
            {
                if (tpl->second->packed)
                {
                    if (tpl->second->byPack.pack)
                        for (auto&& arg2 : *tpl->second->byPack.pack)
                        {
                            if (arg2.second->byClass.val)
                            {
                                auto arg1 = Allocate<Argument>();
                                (*arguments)->push_back(arg1);
                                arg1->tp = arg2.second->byClass.val->CopyType();
                                if (initialize)
                                {
                                    arg1->tp->InstantiateDeferred();
                                    arg1->tp->InitializeDeferred();
                                }
                            }
                        }
                }
                else 
                {
                    auto arg1 = Allocate<Argument>();
                    (*arguments)->push_back(arg1);
                    arg1->tp = arg->tp->templateParam->second->byClass.val->CopyType();
                    if (initialize)
                    {
                        arg1->tp->InstantiateDeferred();
                        arg1->tp->InitializeDeferred();
                    }
                }
            }
        }
        else
        {
            (*arguments)->push_back(arg);
            if (initialize)
            {
                arg->tp->InstantiateDeferred();
                arg->tp->InitializeDeferred();
            }
        }
    }
}
static LexList* GetTypeList(LexList* lex, SYMBOL* funcsp, std::list<Argument*>**lptr)
{
    if (!*lptr)
        *lptr = argumentListFactory.CreateList();
    do
    {
        Type* tp = nullptr;
        lex = getsym(); /* past ( or , */
        tp = TypeGenerator::TypeId(lex, funcsp, StorageClass::cast_, false, true, false);
        if (!tp)
            break;
        auto arg = Allocate<Argument>();
        arg->tp = tp;
        (*lptr)->push_back(arg);
        if (tp->type == BasicType::templateparam_)
        {
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                lex = getsym();
                tp->templateParam->second->packed = true;
            }
        }
    } while (MATCHKW(lex, Keyword::comma_));
    needkw(&lex, Keyword::closepa_);
    return lex;
}
static int FindBaseClassWithData(SYMBOL* sym, SYMBOL** result)
{
    int n = 0;
    if (sym->sb->baseClasses)
        for (auto bc : *sym->sb->baseClasses)
            n += FindBaseClassWithData(bc->cls, result);
    for (auto sym1 : *sym->tp->BaseType()->syms)
    {
        if (sym1->sb->storage_class == StorageClass::mutable_ || sym1->sb->storage_class == StorageClass::member_)
        {
            if (result)
                *result = sym1;
            return n + 1;
        }
    }
    return n;
}
static bool isStandardLayout(Type* tp, SYMBOL** result)
{
    if (tp->IsStructured() && !hasVTab(tp->BaseType()->sp) && !tp->BaseType()->sp->sb->vbaseEntries)
    {
        int n;
        AccessLevel access = AccessLevel::none_;
        SYMBOL *found = nullptr, *first;
        n = FindBaseClassWithData(tp->sp, &found);
        if (n > 1)
            return false;
        if (n)
        {
            SYMBOL* first = nullptr;
            for (auto sym : *found->tp->BaseType()->syms)
            {
                if (!first)
                    first = sym;
                if (sym->sb->storage_class == StorageClass::member_ || sym->sb->storage_class == StorageClass::mutable_)
                {
                    if (sym->tp->IsStructured() && !isStandardLayout(sym->tp, nullptr))
                        return false;
                    if (access != AccessLevel::none_)
                    {
                        if (access != sym->sb->access)
                            return false;
                    }
                    access = sym->sb->access;
                }
            }
            if (first && first->tp->IsStructured())
            {
                if (found->sb->baseClasses)
                    for (auto bc : *found->sb->baseClasses)
                        if (bc->cls->tp->ExactSameType(first->tp))
                            return false;
            }
        }
        if (result)
            *result = found;
        return true;
    }
    return false;
    /*
 hasnonon-staticdatamembersoftypenon-standard-layoutclass(orarrayofsuchtypes)orreference,
   has no virtual functions (10.3) and no virtual base classes (10.1),
   has the same access control (Clause 11) for all non-static data members,
   has no non-standard-layout base classes,
   either has no non-static data members in the most derived class and at most one base class with non-static data members,
    or has no base classes with non-static data members, and
   has no base classes of the same type as the ?rst non-static data member.
 */
}
static bool trivialFunc(SYMBOL* func, bool move)
{
    for (auto sym : *func->tp->BaseType()->syms)
    {
        if (matchesCopy(sym, move))
        {
            return sym->sb->defaulted;
        }
    }
    return true;
}
static bool trivialCopyConstructible(Type* tp, bool rref)
{
    if (tp->IsStructured())
    {
        SYMBOL* ovl;
        ovl = search(tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
        if (ovl)
        {
            if (!trivialFunc(ovl, rref))
                return false;
        }
        if (tp->BaseType()->sp->sb->baseClasses)
            for (auto bc : *tp->BaseType()->sp->sb->baseClasses)
                if (!trivialCopyConstructible(bc->cls->tp, rref))
                    return false;
        for (auto sym : *tp->BaseType()->syms)
        {
            if (sym->sb->storage_class == StorageClass::mutable_ || sym->sb->storage_class == StorageClass::member_)
                if (!trivialCopyConstructible(sym->tp, rref))
                    return false;
        }
    }
    return true;
}
static bool trivialAssignable(Type* tp, bool rref)
{
    if (tp->IsStructured())
    {
        SYMBOL* ovl;
        ovl = search(tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
        if (ovl)
        {
            if (!trivialFunc(ovl, rref))
                return false;
        }
        if (tp->BaseType()->sp->sb->baseClasses)
            for (auto bc : *tp->BaseType()->sp->sb->baseClasses)
                if (!trivialAssignable(bc->cls->tp, rref))
                    return false;
        for (auto sym : *tp->BaseType()->syms)
        {
            if (sym->sb->storage_class == StorageClass::mutable_ || sym->sb->storage_class == StorageClass::member_)
                if (!trivialAssignable(sym->tp, rref))
                    return false;
        }
    }
    return true;
}
static bool trivialDestructor(Type* tp)
{
    if (tp->IsStructured())
    {
        SYMBOL* ovl;
        ovl = search(tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
        if (ovl)
        {
            ovl = ovl->tp->syms->front();
            if (!ovl->sb->defaulted)
                return false;
        }
        if (tp->BaseType()->sp->sb->baseClasses)
            for (auto bc : *tp->BaseType()->sp->sb->baseClasses)
                if (!trivialDestructor(bc->cls->tp))
                    return false;
        for (auto sym : *tp->BaseType()->syms)
        {
            if (sym->sb->storage_class == StorageClass::mutable_ || sym->sb->storage_class == StorageClass::member_)
                if (!trivialDestructor(sym->tp))
                    return false;
        }
    }
    return true;
}
static bool trivialDefaultConstructor(Type* tp)
{
    if (tp->IsStructured())
    {
        SYMBOL* ovl;
        ovl = search(tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
        if (ovl)
        {
            for (auto sym : *ovl->tp->syms)
            {
                auto syms = sym->tp->BaseType()->syms;
              
                if (syms->size() < 3 || (syms->size() == 3 && (syms->back()->tp->type == BasicType::void_)))
                {
                    if (!sym->sb->defaulted)
                        return false;
                    else
                        break;
                }
            }
        }
        if (tp->BaseType()->sp->sb->baseClasses)
            for (auto bc : *tp->BaseType()->sp->sb->baseClasses)
                if (!trivialDefaultConstructor(bc->cls->tp))
                    return false;
        for (auto sym: *tp->BaseType()->syms)
        {
            if (sym->sb->storage_class == StorageClass::mutable_ || sym->sb->storage_class == StorageClass::member_)
                if (!trivialDefaultConstructor(sym->tp))
                    return false;
        }
    }
    return true;
}
static bool triviallyCopyable(Type* tp)
{
    return trivialCopyConstructible(tp, false) && trivialAssignable(tp, false) &&
        trivialCopyConstructible(tp, true) && trivialAssignable(tp, true) &&
        trivialDestructor(tp);
}
static bool trivialStructure(Type* tp)
{
    if (tp->IsRef())
        tp = tp->BaseType()->btp;
    if (tp->IsStructured())
    {
        return triviallyCopyable(tp) && trivialDefaultConstructor(tp);
    }
    return false;
}
/*
A trivially copyable class is a class that:
  has no non-trivial copy constructors (12.8),
  has no non-trivial move constructors (12.8),
  has no non-trivial copy assignment operators (13.5.3, 12.8),
  has no non-trivial move assignment operators (13.5.3, 12.8), and
  has a trivial destructor (12.4).

A trivial class is a class that has a trivial default constructor (12.1) and is trivially copyable.
*/
static bool trivialStructureWithBases(Type* tp)
{
    if (tp->IsStructured())
    {
        if (!trivialStructure(tp))
            return false;
        if (tp->BaseType()->sp->sb->baseClasses)
            for (auto bc : *tp->BaseType()->sp->sb->baseClasses)
                if (!trivialStructureWithBases(bc->cls->tp))
                    return false;
    }
    return true;
}
static bool isPOD(Type* tp)
{
    SYMBOL* found = nullptr;
    if (isStandardLayout(tp, &found) && trivialStructureWithBases(tp)) 
    {
        if (found)
        {
            for (auto sym : *found->tp->BaseType()->syms)
            {
                if (sym->tp->IsStructured() && !trivialStructureWithBases(sym->tp))
                    return false;
            }
        }
        return true;
    }
    return false;
}
inline Argument* first(std::list<Argument*>* args) { return args->front(); }
inline Argument* second(std::list<Argument*>* args) { auto it = args->begin(); return (*++it); }
static bool __is_nothrow(Type* tp, std::list<Argument*>* arguments, SYMBOL* ovl)
{
    // recursion will have been taken care of elsewhere...
    int i = 0;
    std::vector<bool> holdl, holdr;
    EXPRESSION* cexp = nullptr;
    if (ovl)
    {
        if (tp->IsRef())
        {
            tp = tp->BaseType()->btp;
            if (arguments->size() == 2)
            {
                Type* tpy = second(arguments)->tp;
                if (tpy->IsRef())
                    tpy = tpy->BaseType()->btp;
                if (tpy->IsConst() && !tp->IsConst() || tpy->IsVolatile() && !tp->IsVolatile())
                {
                    return false;
                }
                if (tp->IsStructured())
                {
                    if (tpy->IsStructured())
                    {
                        SYMBOL* sp2 = tp->BaseType()->sp;
                        SYMBOL* spy = tpy->BaseType()->sp;
                        if (sp2->sb->mainsym)
                            sp2 = sp2->sb->mainsym;
                        if (spy->sb->mainsym)
                            spy = spy->sb->mainsym;
                        return sp2 == spy || sameTemplate(sp2->tp, spy->tp);
                    }
                    return true;
                }
            }
        }
        CallSite funcparams = {};
        funcparams.thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
        funcparams.thistp = Type::MakeType(BasicType::pointer_, tp->BaseType());
        funcparams.ascall = true;
        funcparams.arguments = arguments;
        for (auto arg : *arguments)
        {
            while (arg->tp->type == BasicType::typedef_)
                arg->tp = arg->tp->btp;
            bool rref = arg->tp->IsStructured();
            if (arg->tp->IsRef() && !arg->tp->BaseType()->btp->IsStructured())
                arg->tp = arg->tp->BaseType()->btp;
            holdl.push_back(arg->tp->lref);
            holdr.push_back(arg->tp->rref);
            if (rref)
            {
                arg->tp->rref = true;
                arg->tp->lref = false;
            }
            else
            {
                arg->tp->lref = false;
                arg->tp->rref = false;
            }
        }
        std::stack<SYMBOL*> stk;
        for (auto spl : *ovl->tp->syms)
        {
            if (spl->templateParams)
            {
                stk.push(spl);
                PushPopTemplateArgs(spl, true);
            }
        }
        int oldSpecialize = inTemplateSpecialization;
        inTemplateSpecialization = 0;
        SYMBOL* sp = GetOverloadedFunction(&tp, &funcparams.fcall, ovl, &funcparams, nullptr, false, false,
                                           _F_SIZEOF | _F_IS_NOTHROW | _F_RETURN_DELETED);
        inTemplateSpecialization = oldSpecialize;
        while (stk.size())
        {
            PushPopTemplateArgs(stk.top(), false);
            stk.pop();
        }
        if (sp && sp->sb->defaulted && !sp->sb->inlineFunc.stmt)
        {
            inNoExceptHandler++;
            if (!strcmp(ovl->name, overloadNameTab[CI_CONSTRUCTOR]))
                createConstructor(sp->sb->parentClass, sp);
            else
                createAssignment(sp->sb->parentClass, sp);
            inNoExceptHandler--;
        }
        i = 0;
        for (auto arg : *arguments)
        {
            arg->tp->lref = holdl[i];
            arg->tp->rref = holdr[i];
            i++;
        }
        bool rv = sp && sp->sb->noExcept && sp->sb->access == AccessLevel::public_;
        return rv;
    }
    return true;
}
static bool nothrowConstructible(Type* tp, std::list<Argument*>* args)
{
    if (tp->IsStructured())
    {
        return __is_nothrow(tp, args, search(tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]));
    }
    return true;
}
static bool nothrowAssignable(Type* tp, std::list<Argument*>* args)
{
    if (tp->IsStructured())
    {
        return __is_nothrow(tp, args, search(tp->BaseType()->syms, overloadNameTab[CI_ASSIGN]));
    }
    return true;
}
static bool is_abstract(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        rv = first(arguments)->tp->IsStructured() && first(arguments)->tp->BaseType()->sp->sb->isabstract;
    }
    return rv;
}
static bool is_base_of(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 2)
    {
        if (first(arguments)->tp->IsStructured() && second(arguments)->tp->IsStructured())
            rv = classRefCount(first(arguments)->tp->BaseType()->sp, second(arguments)->tp->BaseType()->sp) != 0;
    }
    return rv;
}
static bool is_class(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        rv = first(arguments)->tp->IsStructured() && first(arguments)->tp->BaseType()->type != BasicType::union_;
    }
    return rv;
}
static bool is_constructible(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments && arguments->size())
    {
        Type* tp2 = first(arguments)->tp;
        if (tp2->IsArray())
        {
            while (tp2->IsArray() && tp2->size != 0)
                tp2 = tp2->btp;

            if (tp2->IsArray())
            {
                tp2 = nullptr;
            }
        }
        if (tp2)
        {
            Type* tpf = tp2;
            if (tp2->IsRef())
            {
                tp2 = tp2->BaseType()->btp;
                if (arguments->size() == 2)
                {
                    Type* tpy = second(arguments)->tp;
                    if (tpy->IsRef())
                        tpy = tpy->BaseType()->btp;
                    if (tpy->IsConst() && !tp2->IsConst() || tpy->IsVolatile() && !tp2->IsVolatile())
                    {
                        rv = false;
                        return rv;
                    }
                    if (tp2->IsStructured())
                    {
                        bool rv = true;
                        if (tpy->IsStructured())
                        {
                            SYMBOL* sp2 = tp2->BaseType()->sp;
                            SYMBOL* spy = tpy->BaseType()->sp;
                            if (sp2->sb->mainsym)
                                sp2 = sp2->sb->mainsym;
                            if (spy->sb->mainsym)
                                spy = spy->sb->mainsym;
                            rv = sp2 == spy || sameTemplate(sp2->tp, spy->tp);
                        }
                        return rv;
                    }
                }
            }
            if (tp2->IsFunction())
            {
                if (arguments->size() == 2)
                {
                    Type* tpy = second(arguments)->tp;
                    if (tpf->IsRef())
                    {
                        if (tpy->IsRef())
                            tpy = tpy->BaseType()->btp;
                        rv = tp2->ExactSameType(tpy);
                    }
                }
            }
            else if (tp2->IsFunctionPtr())
            {
                if (arguments->size() == 2)
                {
                    Type* tp3 = second(arguments)->tp;
                    if (tp3->IsRef())
                        tp3 = tp3->BaseType()->btp->BaseType();
                    if (tp3->IsFunction())
                    {
                        rv = tp2->BaseType()->btp->ExactSameType(tp3);
                    }
                    else if (tp3->IsStructured())
                    {
                        // look for operator () with args from tp2
                        EXPRESSION* cexp = nullptr;
                        SYMBOL* bcall = search(tp3->BaseType()->syms, overloadNameTab[CI_FUNC]);
                        CallSite funcparams = {};
                        funcparams.thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
                        funcparams.thistp = Type::MakeType(BasicType::pointer_, tp3->BaseType());
                        funcparams.ascall = true;
                        funcparams.arguments = arguments;
                        funcparams.arguments->clear();
                        funcparams.sp = nullptr;
                        for (auto sym : *tp2->BaseType()->btp->BaseType()->syms)
                        {
                            auto arg = Allocate<Argument>();
                            arg->tp = sym->tp;
                            arg->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                            funcparams.arguments->push_back(arg);
                        }
                        Type* tp = nullptr;
                        auto spx = GetOverloadedFunction(&tp, &funcparams.fcall, bcall, &funcparams, nullptr, false, false,
                                                         _F_SIZEOF | _F_RETURN_DELETED);
                        rv = spx && spx->sb->access == AccessLevel::public_ && !spx->sb->deleted;
                    }
                    else if (tp3->BaseType()->nullptrType)
                    {
                        rv = 1;
                    }
                    else
                    {
                        // compare two function pointers...
                        rv = tp2->ExactSameType(tp3);
                    }
                }
                else
                {
                    rv = true;
                }
            }
            else if (tp2->BaseType()->type == BasicType::memberptr_)
            {
                if (arguments->size() == 2)
                {
                    Type* tp3 = second(arguments)->tp;
                    if (tp3->IsRef())
                        tp3 = tp3->BaseType()->btp->BaseType();
                    if (tp3->type == BasicType::memberptr_)
                    {
                        tp2 = tp2->BaseType();
                        SYMBOL* s1 = tp2->sp;
                        SYMBOL* s2 = tp3->sp;
                        if (s1->sb->mainsym)
                            s1 = s1->sb->mainsym;
                        if (s2->sb->mainsym)
                            s2 = s2->sb->mainsym;
                        if (s1 == s2 || sameTemplate(s1->tp, s2->tp))
                        {
                            rv = tp2->btp->ExactSameType(tp3->btp);
                        }
                    }
                    else if (tp3->IsFunction())
                    {
                        tp2 = tp2->BaseType();
                        SYMBOL* s1 = tp2->sp;
                        SYMBOL* s2 = tp3->sp;
                        if (s2 && s2->tp->syms->size())
                        {
                            if (s2->tp->syms->front()->sb->thisPtr)
                            {
                                s2 = s2->tp->syms->front()->tp->BaseType()->btp->BaseType()->sp;
                                if (s1->sb->mainsym)
                                    s1 = s1->sb->mainsym;
                                if (s2->sb->mainsym)
                                    s2 = s2->sb->mainsym;
                                if (s1 == s2 || sameTemplate(s1->tp, s2->tp))
                                {
                                    rv = tp2->btp->ExactSameType(tp3);
                                }
                            }
                        }
                    }
                }
                else
                {
                    rv = true;
                }
            }
            else if (tp2->IsArithmetic() || tp2->IsPtr() || tp2->BaseType()->type == BasicType::enum_)
            {
                if (arguments->size() == 1)
                {
                    rv = true;
                }
                else if (arguments->size() == 2)
                {
                    rv = tp2->SameType(second(arguments)->tp);
                }
            }
            else if (tp2->IsStructured())
            {
                Type* tp3 = NULL;
                tp2 = tp2->BaseType()->sp->tp;
                if (arguments->size()  > 1)
                {
                    tp3 = second(arguments)->tp;
                }
#if 0
                if (tp3 && tp3->IsStructured() &&
                    (tp2->ExactSameType(tp3) || sameTemplate(tp2, tp3)))
                {
                    rv = true;
                }
                else
#endif
                {
                    int i = 0;
                    std::vector<bool>holdl, holdr;
                    Argument* temp;
                    EXPRESSION* cexp = nullptr;
                    SYMBOL* cons = search(tp2->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
                    if (cons)
                    {
                        CallSite funcparams = {};
                        funcparams.thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
                        funcparams.thistp = Type::MakeType(BasicType::pointer_, tp2->BaseType());
                        funcparams.ascall = true;
                        funcparams.arguments = arguments;
                        funcparams.arguments->pop_front();
                        for (auto arg : *funcparams.arguments)
                        {
                            while (arg->tp->type == BasicType::typedef_)
                                arg->tp = arg->tp->btp;
                            bool rref = arg->tp->type == BasicType::rref_;
                            if (arg->tp->IsRef() && !arg->tp->BaseType()->btp->IsStructured())
                                arg->tp = arg->tp->BaseType()->btp;
                            holdl.push_back(arg->tp->lref);
                            holdr.push_back(arg->tp->rref);
                            if (rref)
                            {
                                arg->tp->rref = true;
                                arg->tp->lref = false;
                            }
                            else
                            {
                                arg->tp->lref = false;
                                arg->tp->rref = false;
                            }
                            i++;
                        }
                        std::stack<SYMBOL*> stk;
                        for (auto spl : *cons->tp->syms)
                        {
                            if (spl->templateParams)
                            {
                                stk.push(spl);
                                PushPopTemplateArgs(spl, true);
                            }
                        }
                        Type* tp = nullptr;
                        auto sym = GetOverloadedFunction(&tp, &funcparams.fcall, cons, &funcparams, nullptr, false, false,
                                                         _F_SIZEOF);
                        rv = sym && sym->sb->access == AccessLevel::public_;

                        while (stk.size())
                        {
                            PushPopTemplateArgs(stk.top(), false);
                            stk.pop();
                        }

                        i = 0;
                        for (auto arg : *funcparams.arguments)
                        {
                            arg->tp->lref = holdl[i];
                            arg->tp->rref = holdr[i];
                            i++;
                        }
                    }
                }
            }
        }
    }
    return rv;
}
static bool is_convertible_to(EXPRESSION* exp)
{
    bool rv = true;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 2)
    {
        Type* from = first(arguments)->tp;
        Type* to = second(arguments)->tp;
        if (from->IsRef())
        {
            if (to->IsRef())
            {
                if (to->BaseType()->type == BasicType::lref_)
                    if (from->BaseType()->type == BasicType::rref_)
                        rv = false;
            }
        }
        if (from->IsFunction())
            from = from->BaseType()->btp;
        if (rv)
        {
            while (from->IsRef())
                from = from->BaseType()->btp;
            while (to->IsRef())
                to = to->BaseType()->btp;
            while (from->IsPtr() && to->IsPtr())
            {
                from = from->BaseType()->btp;
                to = to->BaseType()->btp;
            }
            if (to->type == BasicType::templateparam_)
                to = to->templateParam->second->byClass.val;
            if (from->type == BasicType::templateparam_)
                from = from->templateParam->second->byClass.val;
            if (to && from)
            {
                rv = to->SameType(from);
                if (!rv && from->IsStructured() && to->IsStructured())
                {
                    if (classRefCount(to->BaseType()->sp, from->BaseType()->sp) == 1)
                        rv = true;
                    else if (lookupGenericConversion(from->BaseType()->sp, to->BaseType()))
                        rv = true;
                }
                if (!rv && from->IsStructured())
                {
                    CI_CONSTRUCTOR;
                    SYMBOL* sym = search(from->BaseType()->syms, overloadNameTab[CI_CAST]);
                    if (sym)
                    {
                        for (auto sp1 : *sym->tp->syms)
                        {
                            if (sp1->tp->BaseType()->btp->SameType(to))
                            {
                                rv = true;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                rv = false;
            }
        }
    }
    else
    {
        rv = false;
    }
    return rv;
}
static bool is_empty(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 1)
    {
        if (first(arguments)->tp->IsStructured())
            rv = first(arguments)->tp->BaseType()->syms->size() <= 1;
    }
    return rv;
}
static bool is_enum(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        rv = first(arguments)->tp->BaseType()->type == BasicType::enum_;
    }
    return rv;
}
static bool is_final(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        if (first(arguments)->tp->IsStructured())
            rv = first(arguments)->tp->BaseType()->sp->sb->isfinal;
    }
    return rv;
}
static bool is_literal(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        // yes references are literal types...
        rv = !first(arguments)->tp->IsStructured();
    }
    return rv;
}
static bool is_nothrow_constructible(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments && arguments->size())
    {
        Type* tp2 = first(arguments)->tp;
        if (tp2->IsRef() && arguments->size() > 1)
        {
            tp2 = tp2->BaseType()->btp;
            Type* tpy = second(arguments)->tp;
            if (tpy->IsRef())
                tpy = tpy->BaseType()->btp;
            if (tpy->IsConst() && !tp2->IsConst() || tpy->IsVolatile() && !tp2->IsVolatile())
            {
                rv = false;
                return rv;
            }
        }
        if (tp2->IsStructured())
        {
            if (!tp2->BaseType()->sp->sb->trivialCons)
            {
                auto tp = first(arguments)->tp;
                arguments->pop_front();
                rv = nothrowConstructible(tp, arguments);
            }
            else if (arguments->size() > 1)
            {
                rv = tp2->ExactSameType(second(arguments)->tp);
            }
            else
            {
                rv = true;
            }
        }
        else if (arguments->size()  > 1)
        {
            rv = tp2->ExactSameType(second(arguments)->tp);
        }
        else
        {
            rv = true;
        }
    }
    return rv;
}
static bool is_nothrow_assignable(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments && arguments->size())
    {
        Type* tp2 = first(arguments)->tp;
        if (tp2->IsRef() && arguments->size() > 1)
        {
            tp2 = tp2->BaseType()->btp;
            Type* tpy = second(arguments)->tp;
            if (tpy->IsRef())
                tpy = tpy->BaseType()->btp;
            if (tpy->IsConst() && !tp2->IsConst() || tpy->IsVolatile() && !tp2->IsVolatile())
            {
                rv = false;
                return rv;
            }
        }
        if (tp2->IsStructured())
        {
            arguments->pop_front();
            rv = nothrowAssignable(tp2, arguments);
        }
        else if (arguments->size()  > 1)
        {
            rv = tp2->ExactSameType(second(arguments)->tp);
        }
        else
        {
            rv = true;
        }
    }
    return rv;
}
static bool is_pod(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 1)
    {
        rv = first(arguments)->tp->IsArithmetic() || !!isPOD(first(arguments)->tp);
    }
    return rv;
}
static bool is_polymorphic(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 1)
    {
        // yes references are literal types...
        if (first(arguments)->tp->IsStructured())
            rv = !!hasVTab(first(arguments)->tp->BaseType()->sp);
    }
    return rv;
}
static bool is_standard_layout(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 1)
    {
        rv = first(arguments)->tp->IsArithmetic() || !!isStandardLayout(first(arguments)->tp, nullptr);
    }
    return rv;
}
static bool is_trivial(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 1)
    {
        rv = !first(arguments)->tp->IsStructured() || !!trivialStructure(first(arguments)->tp);
    }
    return rv;
}
static bool is_trivially_assignable(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments && arguments->size() && first(arguments)->tp->IsStructured())
    {
        if (arguments->size() == 2)
        {
           Type* tp1 = second(arguments)->tp;
           if (tp1->IsRef())
               tp1 = tp1->BaseType()->btp;
           if (tp1->ExactSameType(first(arguments)->tp) || sameTemplate(tp1, first(arguments)->tp))
              rv = trivialAssignable(first(arguments)->tp, second(arguments)->tp->BaseType()->type== BasicType::rref_);
        }
    }
    return rv;
}
static bool is_trivially_constructible(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments && arguments->size() && first(arguments)->tp->IsStructured())
    {
        if (arguments->size() == 1)
           rv = trivialDefaultConstructor(first(arguments)->tp);
        else if (arguments->size() == 2)
        {
           Type* tp1 = second(arguments)->tp;
           if (tp1->IsRef())
               tp1 = tp1->BaseType()->btp;
           if (tp1->ExactSameType(first(arguments)->tp) || sameTemplate(tp1, first(arguments)->tp))
              rv = trivialCopyConstructible(first(arguments)->tp, second(arguments)->tp->BaseType()->type== BasicType::rref_);
        }
    }
    return rv;
}
static bool is_trivially_destructible(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 1)
    {
        rv = trivialDestructor(first(arguments)->tp);
    }
    return rv;
}
static bool is_trivially_copyable(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments, true);
    if (arguments->size() == 1)
    {
        if (first(arguments)->tp->IsStructured())
            rv = triviallyCopyable(first(arguments)->tp);
    }
    return rv;
}
static bool is_union(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        rv = first(arguments)->tp->BaseType()->type == BasicType::union_;
    }
    return rv;
}
static bool hasConstexprConstructor(Type* tp)
{
    auto ovl = search(tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (ovl)
    {
        for (auto sym : *ovl->tp->syms)
        {
            if (sym->sb->constexpression)
                return true;
        }
    }
    return false;
}
bool is_literal_type(Type* tp);
static void nonStaticLiteralTypes(Type* tp, bool& all, bool& one)
{
    auto it = tp->BaseType()->syms->begin();
    auto ite = tp->BaseType()->syms->end();
    if (it != ite)
    {
        ++it;
        while (it != ite)
        {
            auto sym = *it;
            if (sym->sb->storage_class == StorageClass::member_ || sym->sb->storage_class == StorageClass::mutable_)
            {
                if (sym->tp->IsVolatile())
                {
                    all = false;
                }
                else
                {
                    if (is_literal_type(sym->tp))
                        one = true;
                    else
                        all = false;
                }
            }
            ++it;
        }
    }
}
bool is_literal_type(Type* tp)
{
    if (tp->IsRef() || tp->IsArithmetic() || tp->IsVoid())
        return true;
    if (tp->IsArray())
        return is_literal_type(tp->btp);
    if (tp->IsStructured())
    {
        if (trivialDestructor(tp))
        {
            if (tp->BaseType()->sp->sb->trivialCons || hasConstexprConstructor(tp))
            {
                bool all = true, one = false;
                nonStaticLiteralTypes(tp, all, one);
                if (tp->BaseType()->type == BasicType::union_ && one)
                    return true;
                else if (all)
                    return true;
            }
        }
    }
    return false;
}
static bool is_literal_type(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        rv = is_literal_type(first(arguments)->tp);
    }
    return rv;
}
static bool is_same(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 2)
    {
        auto left = first(arguments)->tp;
        auto right = second(arguments)->tp;
        if (left->IsConst() == right->IsConst() && left->IsVolatile() == right->IsVolatile())
        {
            left = left->BaseType();
            right = right->BaseType();
            if (left->IsRef() && right->IsRef() && left->BaseType()->type == right->BaseType()->type)
            {
                left = left->BaseType()->btp;
                right = right->BaseType()->btp;
            }
            while (left->IsPtr() && right->IsPtr() && left->IsArray() == right->IsArray() && left->IsConst() == right->IsConst() && left->IsVolatile() == right->IsVolatile())
            {
                left = left->BaseType()->btp;
                right = right->BaseType()->btp;
            }
            if (!left->IsPtr() && !right->IsPtr() && !left->IsRef() && !right->IsRef())
            {
                if (left->IsArithmetic())
                {
                    rv = left->type == right->type;   
                }
                else if (left->IsStructured() || left->IsDeferred())
                {
                    left->InstantiateDeferred();
                    right->InstantiateDeferred();
                    rv = left->ExactSameType(right);
                    if (!rv)
                    {
                        rv = sameTemplate(left, right);
                    }
                }
                else
                {
                    rv = left->ExactSameType(right);
                }
            }
        }
    }
    return rv;
}
static bool is_assignable(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 2)
    {
        auto left = first(arguments)->tp;
        auto right = second(arguments)->tp;
        if (!left->IsConst() && (!left->IsRef() || (!left->BaseType()->btp->IsConst() && !(right->IsRef() ? right->BaseType()->btp->IsConst() : right->IsConst() ))))
        {
            left->InstantiateDeferred();
            right->InstantiateDeferred();

            rv = left->ExactSameType(right);
            if (!rv)
            {
                rv = sameTemplate(left, right);
            }
            if (!rv)
            {
                EXPRESSION* exp = nullptr;
                if (left->IsStructured())
                {
                    if (left->IsRef())
                        left = left->BaseType()->btp->BaseType();
                    Type* ctype = left;
                    exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    CallSite funcparams = {};
                    funcparams.thisptr = exp;
                    funcparams.arguments = argumentListFactory.CreateList();
                    Argument* arg = Allocate<Argument>();
                    funcparams.arguments->push_back(arg);
                    arg->exp = anonymousVar(StorageClass::auto_, right);
                    arg->tp = right;
                    rv = callConstructor(&ctype, &exp, &funcparams, true, nullptr, true, true, false, false, false, true, false);
                }
                if (!rv)
                {
                    if (right->IsRef())
                    {
                        right = right->BaseType()->btp;
                    }
                    if (right->IsStructured())
                    {
                        if (!exp)
                            exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        if (left->IsPtr())
                        {
                            rv = castToPointer(&right, &exp, (Keyword)-1, left);
                        }
                        else
                        {
                            rv = castToArithmeticInternal(false, &right, &exp, (Keyword)-1, left, false);
                        }
                    }
                }
            }
        }
    }
    return rv;

}
static bool is_aggregate(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {

        auto tp = first(arguments)->tp;
        while (tp->IsRef()) tp = tp->BaseType()->btp;
        rv = tp->IsArray();
        while (tp->IsArray()) tp = tp->BaseType()->btp;
        tp->InstantiateDeferred();
        if (tp->IsStructured() && tp->BaseType()->syms && !tp->BaseType()->sp->sb->hasUserCons)
        {
            tp = tp->BaseType();
            if (!tp->sp->sb->baseClasses || tp->sp->sb->baseClasses->empty())
            {
                rv = true;
                for (auto s : *tp->syms)
                {
                    if (ismemberdata(s))
                    {
                        if (s->sb->access != AccessLevel::public_ || s->tp->IsRef() || (s->tp->IsStructured() && !s->tp->BaseType()->sp->sb->trivialCons))
                        {
                            rv = false;
                            break;
                        }
                    }
                }
            }
        }
    }
    return rv;
}
static bool is_member_pointer(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        auto tp = first(arguments)->tp;
	rv = tp->BaseType()->type == BasicType::memberptr_;
    }
    return rv;
}
static bool hasUniqueRepresentation(Type* tp)
{
    while (tp->IsRef()) tp = tp->BaseType()->btp; // we want the object type not the representation of the ref
    while (tp->IsArray()) tp = tp->BaseType()->btp; // we want the base type for arrays
    if (tp->IsStructured())
    {
        tp->InstantiateDeferred();
        tp = tp->BaseType();
        if (tp->syms == nullptr || !triviallyCopyable(tp))
            return false;
        // if it is structed it may have padding, and if it does then there is no unique representation
        if (tp->type == BasicType::union_)
        {
            // for a union go through each element in turn and make sure none of them don't have unique representations
            for (auto s : *tp->syms)
                if (ismemberdata(s))
                    if (!hasUniqueRepresentation(s->tp))
                        return false;
        }
        else
        {
            // for a structures we check for padding...
            // start by checking the base classes
            if (tp->sp->sb->baseClasses)
            {
                for (auto b : *tp->sp->sb->baseClasses)
                    if (!hasUniqueRepresentation(b->cls->tp))
                        return false;
            }
            bool first = true;
            int last = 0;
            int lastsz = 0;
            int laststart = 0;
            int lastbits = 0;
            for (auto s : *tp->syms)
            {
                if (ismemberdata(s))
                {
                    if (first)
                    {
                        if (s->sb->offset != 0 && s->sb->offset != getSize(BasicType::pointer_))
                        {
                            return false;
                        }
                        first = false;
                        last = s->sb->offset;
                        lastsz =  s->tp->size;
                        laststart = s->tp->startbit;
                        lastbits = s->tp->bits;
                        if (lastbits && !strncmp(s->name, "Unnamed++", 9))
                            return false;
                    }
                    else
                    {
                        if (s->sb->offset == last)
                        {
                            //bits
                            if (laststart + lastbits != s->tp->startbit)
                                return false;
                            laststart = s->tp->startbit;
                            lastbits = s->tp->bits;
                            if (!strncmp(s->name, "Unnamed++", 9))
                                return false;
                        }
                        else
                        {
                            if (last + lastsz != s->sb->offset)
                                return false;
                            if (lastbits && ((lastbits + laststart)/CHAR_BIT != lastsz))
                				return false;
                            last = s->sb->offset;
                            lastsz =  s->tp->size;
                            laststart = s->tp->startbit;
                            lastbits = s->tp->bits;
                            if (lastbits && !strncmp(s->name, "Unnamed++", 9))
                                return false;
                        }
                    }
                }
            }
            if (last + lastsz != tp->size)
                return false;
            if (lastbits && ((lastbits + laststart) / CHAR_BIT != lastsz))
                return false;
        }
    }
    // integers, enums, memberpointers, etc all have a deterministic layout
    return true;
}
static bool has_unique_object_representations(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        rv = hasUniqueRepresentation(first(arguments)->tp);
    }
    return rv;
}
static bool has_virtual_destructor(EXPRESSION* exp)
{
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;
    GetTypeList(exp, &arguments);
    if (arguments->size() == 1)
    {
        auto tp = first(arguments)->tp;
        while (tp->IsRef()) tp = tp->BaseType()->btp;
        tp->InstantiateDeferred();
        if (tp->IsStructured())
        {
            if (tp->BaseType()->syms)
            {
                SYMBOL* dest = search(tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
                if (dest)
                {
                    rv = dest->tp->syms->front()->sb->storage_class == StorageClass::virtual_;
                }
            }
        }
    }
    return rv;
}
Type* decay(Type *tp)
{
    if (tp->IsFunction())
    {
        tp = Type::MakeType(BasicType::pointer_, tp);
    }            
    else if (tp->IsArray())
    {
        tp = tp->BaseType()->btp;
        tp = Type::MakeType(BasicType::pointer_, tp);
    }
    return tp;
}
bool underlying_type(LexList** lex, SYMBOL* funcsp, Type** tp)
{
    static EXPRESSION* exp = MakeExpression(ExpressionNode::cppintrinsic_);
    bool rv = false;
    std::list<Argument*>* arguments = nullptr;

    *lex = GetTypeList(*lex, funcsp, &arguments);
    if (arguments->size() == 1)
    {
        exp->v.cppintrinsicArgs = arguments;
        GetTypeList(exp, &arguments);
        if (arguments->size())
        {
            *tp = first(arguments)->tp;
            if ((*tp)->BaseType()->type == BasicType::enum_)
                *tp = (*tp)->BaseType()->btp;
        }
        else
        {
            *tp = &stdint;
        }
    }
    else
    {
        *tp = &stdint;
    }
    return true;
}
static SYMBOL* MakeIntegerSeqType(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    EXPRESSION* e = nullptr;
    auto it = args->begin();
    if (args->size() >= 3)
    {
        auto it = args->begin();
        ++it;
        ++it;
        e = it->second->byNonType.dflt;
    }
    if (e && isintconst(e))
    {
        it = args->begin();
        ++it; // second arg
        SYMBOL* tpl = args->front().second->byTemplate.dflt;
        if (tpl->sb->parentTemplate)
            tpl = tpl->sb->parentTemplate;
        int nt = (int)it->second->byClass.dflt->BaseType()->type + e->v.i;
        const char* nm = tpl->name;
        auto sym = integerSequences[nm][nt];
        if (sym)
            return sym;
        int n = e->v.i;
        decltype(args) args1 = templateParamPairListFactory.CreateList();
        auto second = Allocate<TEMPLATEPARAM>();
        second->type = TplType::new_;
        args1->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
        args1->push_back(TEMPLATEPARAMPAIR{ nullptr, it->second });
        second = Allocate<TEMPLATEPARAM>();
        second->type = TplType::int_;
        second->byNonType.tp = it->second->byClass.dflt;
        second->packed = true;
        args1->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
        auto last = args1->back().second->byPack.pack = templateParamPairListFactory.CreateList();
        for (int i = 0; i < n; i++)
        {
            second = Allocate<TEMPLATEPARAM>();
            second->type = TplType::int_;
            second->byNonType.tp = it->second->byClass.dflt;
            second->byNonType.val = MakeIntExpression(ExpressionNode::c_i_, i);
            last->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
        }
        sym = GetClassTemplate(tpl, args1, false);
        if (sym)
        {
            auto sym1 = TemplateClassInstantiateInternal(sym, args1, false);
            if (sym1)
            {
                integerSequences[nm][nt] = sym1;
                return sym1;
            }
            return sym;
        }
        return tpl;
    }
    return nullptr;
}
SYMBOL* MakeIntegerSeq(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    SYMBOL* rv = CopySymbol(sym);
    rv->sb->mainsym = sym;
    auto rs = MakeIntegerSeqType(sym, args);
    if (rs)
        rv->tp = rs->tp;
    return rv;
}
static Type* TypePackElementType(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    auto it = args->begin();
    auto ite = args->end();
    if (it->second->type == TplType::new_)
        ++it;
    EXPRESSION* e;
    if (it->second->packed)
    {
        if (!it->second->byPack.pack)
            return &stdany;
        auto it1 = it->second->byPack.pack->begin();
        if (it1->second->type == TplType::new_)
            ++it1;
        e = it1->second->byNonType.val;
        if (!e)
            e = it1->second->byNonType.dflt;
    }
    else
    {
        e = it->second->byNonType.val;
        if (!e)
            e = it->second->byNonType.dflt;
    }
    if (e && isintconst(e))
    {
        int n = e->v.i;
        ++it;
        if (it != ite && it->second->packed)
        {
            if (!it->second->byPack.pack)
                return &stdany;
            ite = it->second->byPack.pack->end();
            it = it->second->byPack.pack->begin();
            if (it != ite && it->second->type == TplType::new_)
                ++it;        
        }
        while (n-- && it != ite)
        {
            ++it;
        }
        if (it == ite)
            return &stdany;
        return Type::MakeType(BasicType::derivedfromtemplate_, it->second->byClass.val ? it->second->byClass.val : it->second->byClass.dflt);
    }
    else
    {
        return &stdany;
    }
}
SYMBOL* TypePackElementCls(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    SYMBOL* rv = CopySymbol(sym);
    rv->sb->mainsym = sym;
    rv->tp = sym->tp->CopyType();
    rv->tp->syms = symbols.CreateSymbolTable();
    rv->tp->syms->Add(CopySymbol(rv));
    auto tp1 = Type::MakeType(BasicType::typedef_, TypePackElementType(sym, args));
    auto sym1 = makeID(StorageClass::typedef_, tp1, nullptr, "type");
    rv->tp->syms->Add(sym1);
    return rv;
}
SYMBOL* TypePackElement(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    SYMBOL* rv = CopySymbol(sym);
    rv->sb->mainsym = sym;
    rv->tp = TypePackElementType(sym, args);
    return rv;
}
}  // namespace Parser