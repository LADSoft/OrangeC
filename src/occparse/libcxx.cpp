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

namespace Parser
{
int inNoExceptHandler;


typedef bool INTRINS_FUNC(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_abstract(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_base_of(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_class(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_constructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_convertible_to(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_empty(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_enum(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_final(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_literal(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_nothrow_constructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_nothrow_assignable(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_pod(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_polymorphic(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_standard_layout(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_trivial(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_trivially_assignable(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_trivially_constructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_trivially_destructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_trivially_copyable(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_union(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);
static bool is_literal_type(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp);

static struct _ihash
{
    const char* name;
    INTRINS_FUNC* func;

} defaults[] = {
    {"__is_abstract", is_abstract},
    {"__is_base_of", is_base_of},
    {"__is_class", is_class},
    {"__is_constructible", is_constructible},
    {"__is_convertible_to", is_convertible_to},
    {"__is_empty", is_empty},
    {"__is_enum", is_enum},
    {"__is_final", is_final},
    {"__is_literal", is_literal},
    {"__is_nothrow_constructible", is_nothrow_constructible},
    {"__is_nothrow_assignable", is_nothrow_assignable},
    {"__is_pod", is_pod},
    {"__is_polymorphic", is_polymorphic},
    {"__is_standard_layout", is_standard_layout},
    {"__is_trivial", is_trivial},
    {"__is_trivially_assignable", is_trivially_assignable},
    {"__is_trivially_constructible", is_trivially_constructible},
    {"__is_trivially_destructible", is_trivially_destructible},
    {"__is_trivially_copyable", is_trivially_copyable},
    {"__is_union", is_union},
    {"__is_literal_type", is_literal_type},
};
static SymbolTable<_ihash> intrinsicHash;

static std::unordered_map<std::string, std::unordered_map<unsigned, SYMBOL*>, StringHash> integerSequences;

void libcxx_init(void)
{
    intrinsicHash = SymbolTable<_ihash>();
    int i;
    integerSequences.clear();
    for (int i = 0; i < sizeof(defaults) / sizeof(defaults[0]); i++)
        intrinsicHash.Add(&defaults[i]);
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
        preProcessor->Define("__has_virtual_destructor_xx", "0");
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
        preProcessor->Define("__has___is_trivially_destructible", "1");
        preProcessor->Define("__has___is_nothrow_constructible", "1");
        preProcessor->Define("__has___is_nothrow_assignable", "1");
        preProcessor->Define("__has___nullptr", "0");
        preProcessor->Define("__has__Atomic", "1");
        preProcessor->Define("__has___is_aggregate", "0");
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
    }
}

bool parseBuiltInTypelistFunc(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    _ihash* p = intrinsicHash.Lookup(sym->name);
    if (p)
    {
        return p->func(lex, funcsp, sym, tp, exp);
    }
    return false;
}
static LexList* getTypeList(LexList* lex, SYMBOL* funcsp, std::list<Argument*>**lptr, bool initialize = false)
{
    if (!*lptr)
        *lptr = initListListFactory.CreateList();
    do
    {
        Type* tp = nullptr;
        lex = getsym(); /* past ( or , */
        tp = TypeGenerator::TypeId(lex, funcsp, StorageClass::cast_, false, true, false);
        if (!tp)
            break;
        if (tp->BaseType()->type != BasicType::templateparam_)
        {
            auto arg = Allocate<Argument>();
            arg->tp = tp;
            if (initialize)
                arg->tp = PerformDeferredInitialization(arg->tp, funcsp);
//            arg->exp = MakeIntExpression(ExpressionNode::c_i_, 1);
            (*lptr)->push_back(arg);
        }
        else
        {
            tp = tp->BaseType();
            if (tp->templateParam->second->packed)
            {
                std::list<TEMPLATEPARAMPAIR>* tpl = tp->templateParam->second->byPack.pack;
                needkw(&lex, Keyword::ellipse_);
                if (tp->templateParam->second->byPack.pack)
                    for (auto&& tpl : *tp->templateParam->second->byPack.pack)
                    {
                        if (tpl.second->byClass.val)
                        {
                            auto arg = Allocate<Argument>();
                            arg->tp = tpl.second->byClass.val;
                            if (initialize)
                                arg->tp = PerformDeferredInitialization(arg->tp, funcsp);
//                            arg->exp = MakeIntExpression(ExpressionNode::c_i_, 1);
                            (*lptr)->push_back(arg);
                        }
                    }
            }
            else
            {
                if (tp->templateParam->second->byClass.val)
                {
                    auto arg = Allocate<Argument>();
                    arg->tp = tp->templateParam->second->byClass.val;
                    if (initialize)
                        arg->tp = PerformDeferredInitialization(arg->tp, funcsp);
//                    arg->exp = MakeIntExpression(ExpressionNode::c_i_, 1);
                    (*lptr)->push_back(arg);
                }
            }
        }
        // this may be a bit naive...
        if (MATCHKW(lex, Keyword::ellipse_))
            lex = getsym();
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
static bool __is_nothrow(Type* tp, std::list<Argument*>* args, SYMBOL* ovl)
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
            if (args->size() == 2)
            {
                Type* tpy = second(args)->tp;
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
        funcparams.arguments = args;
        for (auto arg : *args)
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
        for (auto arg : *funcparams.arguments)
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
static bool is_abstract(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        rv = first(funcparams.arguments)->tp->IsStructured() && first(funcparams.arguments)->tp->BaseType()->sp->sb->isabstract;
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_base_of(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 2)
    {
        if (first(funcparams.arguments)->tp->IsStructured() && second(funcparams.arguments)->tp->IsStructured())
            rv = classRefCount(first(funcparams.arguments)->tp->BaseType()->sp, second(funcparams.arguments)->tp->BaseType()->sp) != 0;
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_class(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        rv = first(funcparams.arguments)->tp->IsStructured() && first(funcparams.arguments)->tp->BaseType()->type != BasicType::union_;
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_constructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments && funcparams.arguments->size())
    {
        Type* tp2 = first(funcparams.arguments)->tp;
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
                if (funcparams.arguments->size() == 2)
                {
                    Type* tpy = second(funcparams.arguments)->tp;
                    if (tpy->IsRef())
                        tpy = tpy->BaseType()->btp;
                    if (tpy->IsConst() && !tp2->IsConst() || tpy->IsVolatile() && !tp2->IsVolatile())
                    {
                        rv = false;
                        *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
                        *tp = &stdint;
                        return true;
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
                        *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
                        *tp = &stdint;
                        return rv;
                    }
                }
            }
            if (tp2->IsFunction())
            {
                if (funcparams.arguments->size() == 2)
                {
                    Type* tpy = second(funcparams.arguments)->tp;
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
                if (funcparams.arguments->size() == 2)
                {
                    Type* tp3 = second(funcparams.arguments)->tp;
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
                        funcparams.thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
                        funcparams.thistp = Type::MakeType(BasicType::pointer_, tp3->BaseType());
                        funcparams.ascall = true;
                        funcparams.arguments->clear();
                        funcparams.sp = nullptr;
                        for (auto sym : *tp2->BaseType()->btp->BaseType()->syms)
                        {
                            auto arg = Allocate<Argument>();
                            arg->tp = sym->tp;
                            arg->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                            funcparams.arguments->push_back(arg);
                        }
                        auto spx = GetOverloadedFunction(tp, &funcparams.fcall, bcall, &funcparams, nullptr, false, false,
                                                         _F_SIZEOF | _F_RETURN_DELETED);
                        rv = spx && spx->sb->access == AccessLevel::public_ && !spx->sb->deleted;
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
                if (funcparams.arguments->size() == 2)
                {
                    Type* tp3 = second(funcparams.arguments)->tp;
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
                if (funcparams.arguments->size() == 1)
                {
                    rv = true;
                }
                else if (funcparams.arguments->size() == 2)
                {
                    rv = tp2->SameType(second(funcparams.arguments)->tp);
                }
            }
            else if (tp2->IsStructured())
            {
                Type* tp3 = NULL;
                tp2 = tp2->BaseType()->sp->tp;
                if (funcparams.arguments->size()  > 1)
                {
                    tp3 = second(funcparams.arguments)->tp;
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
                        funcparams.thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
                        funcparams.thistp = Type::MakeType(BasicType::pointer_, tp2->BaseType());
                        funcparams.ascall = true;
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
                        auto sym = GetOverloadedFunction(tp, &funcparams.fcall, cons, &funcparams, nullptr, false, false,
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
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_convertible_to(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = true;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 2)
    {
        Type* from = first(funcparams.arguments)->tp;
        Type* to = second(funcparams.arguments)->tp;
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
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_empty(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 1)
    {
        if (first(funcparams.arguments)->tp->IsStructured())
            rv = first(funcparams.arguments)->tp->BaseType()->syms->size() <= 1;
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_enum(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        rv = first(funcparams.arguments)->tp->BaseType()->type == BasicType::enum_;
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_final(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        if (first(funcparams.arguments)->tp->IsStructured())
            rv = first(funcparams.arguments)->tp->BaseType()->sp->sb->isfinal;
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_literal(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        // yes references are literal types...
        rv = !first(funcparams.arguments)->tp->IsStructured();
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_nothrow_constructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments && funcparams.arguments->size())
    {
        Type* tp2 = first(funcparams.arguments)->tp;
        if (tp2->IsRef() && funcparams.arguments->size() > 1)
        {
            tp2 = tp2->BaseType()->btp;
            Type* tpy = second(funcparams.arguments)->tp;
            if (tpy->IsRef())
                tpy = tpy->BaseType()->btp;
            if (tpy->IsConst() && !tp2->IsConst() || tpy->IsVolatile() && !tp2->IsVolatile())
            {
                rv = false;
                *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
                *tp = &stdint;
                return true;
            }
        }
        if (tp2->IsStructured())
        {
            if (!tp2->BaseType()->sp->sb->trivialCons)
            {
                auto tp = first(funcparams.arguments)->tp;
                funcparams.arguments->pop_front();
                rv = nothrowConstructible(tp, funcparams.arguments);
            }
            else if (funcparams.arguments->size() > 1)
            {
                rv = tp2->ExactSameType(second(funcparams.arguments)->tp);
            }
            else
            {
                rv = true;
            }
        }
        else if (funcparams.arguments->size()  > 1)
        {
            rv = tp2->ExactSameType(second(funcparams.arguments)->tp);
        }
        else
        {
            rv = true;
        }
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_nothrow_assignable(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments && funcparams.arguments->size())
    {
        Type* tp2 = first(funcparams.arguments)->tp;
        if (tp2->IsRef() && funcparams.arguments->size() > 1)
        {
            tp2 = tp2->BaseType()->btp;
            Type* tpy = second(funcparams.arguments)->tp;
            if (tpy->IsRef())
                tpy = tpy->BaseType()->btp;
            if (tpy->IsConst() && !tp2->IsConst() || tpy->IsVolatile() && !tp2->IsVolatile())
            {
                rv = false;
                *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
                *tp = &stdint;
                return true;
            }
        }
        if (tp2->IsStructured())
        {
            funcparams.arguments->pop_front();
            rv = nothrowAssignable(tp2, funcparams.arguments);
        }
        else if (funcparams.arguments->size()  > 1)
        {
            rv = tp2->ExactSameType(second(funcparams.arguments)->tp);
        }
        else
        {
            rv = true;
        }
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_pod(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 1)
    {
        rv = first(funcparams.arguments)->tp->IsArithmetic() || !!isPOD(first(funcparams.arguments)->tp);
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_polymorphic(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 1)
    {
        // yes references are literal types...
        if (first(funcparams.arguments)->tp->IsStructured())
            rv = !!hasVTab(first(funcparams.arguments)->tp->BaseType()->sp);
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_standard_layout(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 1)
    {
        rv = first(funcparams.arguments)->tp->IsArithmetic() || !!isStandardLayout(first(funcparams.arguments)->tp, nullptr);
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivial(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 1)
    {
        rv = !first(funcparams.arguments)->tp->IsStructured() || !!trivialStructure(first(funcparams.arguments)->tp);
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivially_assignable(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments && funcparams.arguments->size() && first(funcparams.arguments)->tp->IsStructured())
    {
        if (funcparams.arguments->size() == 2)
        {
           Type* tp1 = second(funcparams.arguments)->tp;
           if (tp1->IsRef())
               tp1 = tp1->BaseType()->btp;
           if (tp1->ExactSameType(first(funcparams.arguments)->tp) || sameTemplate(tp1, first(funcparams.arguments)->tp))
              rv = trivialAssignable(first(funcparams.arguments)->tp, second(funcparams.arguments)->tp->BaseType()->type== BasicType::rref_);
        }
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivially_constructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments && funcparams.arguments->size() && first(funcparams.arguments)->tp->IsStructured())
    {
        if (funcparams.arguments->size() == 1)
           rv = trivialDefaultConstructor(first(funcparams.arguments)->tp);
        else if (funcparams.arguments->size() == 2)
        {
           Type* tp1 = second(funcparams.arguments)->tp;
           if (tp1->IsRef())
               tp1 = tp1->BaseType()->btp;
           if (tp1->ExactSameType(first(funcparams.arguments)->tp) || sameTemplate(tp1, first(funcparams.arguments)->tp))
              rv = trivialCopyConstructible(first(funcparams.arguments)->tp, second(funcparams.arguments)->tp->BaseType()->type== BasicType::rref_);
        }
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivially_destructible(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 1)
    {
        rv = trivialDestructor(first(funcparams.arguments)->tp);
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivially_copyable(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments, true);
    if (funcparams.arguments->size() == 1)
    {
        if (first(funcparams.arguments)->tp->IsStructured())
            rv = triviallyCopyable(first(funcparams.arguments)->tp);
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
static bool is_union(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        rv = first(funcparams.arguments)->tp->BaseType()->type == BasicType::union_;
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
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
static bool is_literal_type(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        rv = is_literal_type(first(funcparams.arguments)->tp);
    }
    *exp = MakeIntExpression(ExpressionNode::c_i_, rv);
    *tp = &stdint;
    return true;
}
bool underlying_type(LexList** lex, SYMBOL* funcsp, SYMBOL* sym, Type** tp, EXPRESSION** exp)
{
    bool rv = false;
    CallSite funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments->size() == 1)
    {
        *tp = first(funcparams.arguments)->tp;
        if ((*tp)->BaseType()->type == BasicType::enum_)
            *tp = (*tp)->BaseType()->btp;
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
    if (it->second->packed)
    {
        if (!it->second->byPack.pack)
            return &stdany;
        ite = it->second->byPack.pack->end();
        it = it->second->byPack.pack->begin();
        if (it->second->type == TplType::new_)
            ++it;
    }
    auto e = it->second->byNonType.val;
    if (!e)
        e = it->second->byNonType.dflt;
    if (e && isintconst(e))
    {
        int n = e->v.i;
        ++it;
        if (it->second->packed)
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