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

#include <stack>

#include "compiler.h"
#include "config.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "initbackend.h"
#include "unmangle.h"
#include "OptUtils.h"
#include "lex.h"
#include "help.h"
#include "ccerr.h"
#include "declare.h"
#include "declcpp.h"
#include "namespace.h"
#include "symtab.h"
#include "stmt.h"
#include "declare.h"
#include "beinterf.h"
#include "memory.h"
#include "mangle.h"
#include "constopt.h"
#include "expr.h"
#include "types.h"
#include "ListFactory.h"
#include "libcxx.h"
#include "Property.h"
#include "lambda.h"
#include "init.h"
#include "overload.h"
#include "class.h"
#include <cassert>
#include "exprpacked.h"
#include "Utils.h"
#include "SymbolProperties.h"
#include "using.h"
#include "attribs.h"

namespace Parser
{
static Type* RootType(Type* tp)
{
    assert(tp);
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::const_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
            case BasicType::volatile_:
            case BasicType::restrict_:
            case BasicType::static_:
            case BasicType::atomic_:
            case BasicType::typedef_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            default:
                return tp;
        }
    }
    return nullptr;
}
bool Type::IsAutoType()
{
    Type* tp = this;
    assert(tp);
    if (tp->IsRef())
        tp = tp->BaseType()->btp;
    while (tp->IsPtr())
        tp = tp->BaseType()->btp;
    return tp->BaseType()->type == BasicType::auto_;
}
bool Type::IsUnsigned()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    switch (tp->type)
    {
        case BasicType::bool_:
        case BasicType::unsigned_:
        case BasicType::unsigned_short_:
        case BasicType::unsigned_char_:
        case BasicType::unsigned_long_:
        case BasicType::unsigned_long_long_:
        case BasicType::wchar_t_:
        case BasicType::unsigned_bitint_:
        case BasicType::char32_t_:
            return true;
        default:
            return false;
    }
}
bool Type::IsBitInt()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    return tp->type == BasicType::bitint_ || tp->type == BasicType::unsigned_bitint_;
}
bool Type::IsInt()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    switch (tp->type)
    {
        case BasicType::bool_:
        case BasicType::int_:
        case BasicType::char8_t_:
        case BasicType::char16_t_:
        case BasicType::char32_t_:
        case BasicType::unsigned_:
        case BasicType::short_:
        case BasicType::unsigned_short_:
        case BasicType::char_:
        case BasicType::unsigned_char_:
        case BasicType::signed_char_:
        case BasicType::long_:
        case BasicType::unsigned_long_:
        case BasicType::long_long_:
        case BasicType::unsigned_long_long_:
        case BasicType::wchar_t_:
        case BasicType::inative_:
        case BasicType::unative_:
        case BasicType::bitint_:
        case BasicType::unsigned_bitint_:
            return true;
        case BasicType::templateparam_:
            if (tp->templateParam->second->type == TplType::int_)
                return tp->templateParam->second->byNonType.tp->IsInt();
            return false;
        default:
            if (tp->type == BasicType::enum_ && !Optimizer::cparams.prm_cplusplus)
                return true;

            return false;
    }
}
bool Type::IsFloat()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    switch (tp->type)
    {
        case BasicType::float_:
        case BasicType::double_:
        case BasicType::long_double_:
            return true;
        default:
            return false;
    }
    return false;
}
bool Type::IsComplex()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    switch (tp->type)
    {
        case BasicType::float__complex_:
        case BasicType::double__complex_:
        case BasicType::long_double_complex_:
            return true;
        default:
            return false;
    }
    return false;
}
bool Type::IsImaginary()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    switch (tp->type)
    {
        case BasicType::float__imaginary_:
        case BasicType::double__imaginary_:
        case BasicType::long_double_imaginary_:
            return true;
        default:
            return false;
    }
}
bool Type::IsArithmetic()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    return tp->IsInt() || tp->IsFloat() || tp->IsComplex() || tp->IsImaginary();
}
bool Type::IsMsil()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    return tp->type == BasicType::string_ || tp->type == BasicType::object_;
    return false;
}
bool Type::IsConst()
{
    Type* tp = this;
    assert(tp);
    bool done = false;
    bool rv = false;
    while (!done && tp)
    {
        switch (tp->type)
        {
            case BasicType::restrict_:
            case BasicType::volatile_:
            case BasicType::static_:
            case BasicType::atomic_:
            case BasicType::typedef_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
                tp = tp->btp;
                break;
            case BasicType::const_:
                rv = true;
                done = true;
                break;
            default:
                done = true;
                break;
        }
    }
    return rv;
}

bool Type::IsVolatile()
{
    Type* tp = this;
    assert(tp);
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::restrict_:
            case BasicType::const_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
            case BasicType::static_:
            case BasicType::atomic_:
            case BasicType::typedef_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            case BasicType::volatile_:
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool Type::IsLRefQual()
{
    Type* tp = this;
    assert(tp);
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::volatile_:
            case BasicType::const_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
            case BasicType::static_:
            case BasicType::atomic_:
            case BasicType::typedef_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::rrqual_:
            case BasicType::restrict_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            case BasicType::lrqual_:
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool Type::IsRRefQual()
{
    Type* tp = this;
    assert(tp);
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::volatile_:
            case BasicType::const_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
            case BasicType::static_:
            case BasicType::atomic_:
            case BasicType::typedef_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::lrqual_:
            case BasicType::restrict_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            case BasicType::rrqual_:
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool Type::IsRestrict()
{
    Type* tp = this;
    assert(tp);
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::volatile_:
            case BasicType::const_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
            case BasicType::static_:
            case BasicType::atomic_:
            case BasicType::typedef_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            case BasicType::restrict_:
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool Type::IsAtomic()
{
    Type* tp = this;
    assert(tp);
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::volatile_:
            case BasicType::const_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
            case BasicType::static_:
            case BasicType::restrict_:
            case BasicType::typedef_:
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            case BasicType::atomic_:
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool Type::IsVoid()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    return tp->type == BasicType::void_;
}
bool Type::IsVoidPtr()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    return tp->IsPtr() && tp->btp->IsVoid();
}
bool Type::IsArray()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    return tp->IsPtr() && tp->BaseType()->array;
}
bool Type::IsDeferred(bool sym)
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    if (tp && tp->type == BasicType::templatedeferredtype_)
        return true;
    if (sym)
    {
        if (tp->IsStructured() && tp->sp->sb->templateLevel)
        {
            std::stack<SYMBOL*> stk;
            stk.push(tp->sp);
            while (!stk.empty())
            {
                auto sp = stk.top();
                stk.pop();
                if (sp->templateParams)
                {
                    for (auto&& tpl : *sp->templateParams)
                    {
                        if (tpl.second->type == TplType::typename_ && tpl.second->byClass.val)
                        {
                            if (tpl.second->byClass.val->BaseType()->type == BasicType::templatedeferredtype_)
                                return true;
                            if (tpl.second->byClass.val->IsStructured())
                                stk.push(tpl.second->byClass.val->BaseType()->sp);
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Type::IsUnion()
{
    Type* tp = this;
    assert(tp);
    tp = tp->BaseType();
    return tp->type == BasicType::union_;
    return false;
}
inline bool __isref(Type* x) { return (x)->type == BasicType::lref_ || (x)->type == BasicType::rref_; }
bool Type::IsRef()
{
    Type* x = this;
    assert(x);
    return (__isref(x->BaseType()) || (x)->type == BasicType::templateparam_ && (x)->templateParam->second->type == TplType::int_ &&
                                          __isref((x)->templateParam->second->byNonType.tp));
}
inline bool __ispointer(Type* x) { return ((x)->type == BasicType::pointer_ || (x)->type == BasicType::seg_); }
bool Type::IsPtr()
{
    Type* x = this;
    assert(x);
    return (__ispointer(x->BaseType()) || (x)->type == BasicType::templateparam_ &&
                                              (x)->templateParam->second->type == TplType::int_ &&
                                              __ispointer((x)->templateParam->second->byNonType.tp));
}

CONSTEXPR inline bool __isfunction(Type* x) { return ((x)->type == BasicType::func_ || (x)->type == BasicType::ifunc_); }
bool Type::IsFunction()
{
    Type* x = this;
    assert(x);
    return (__isfunction(x->BaseType()));
}

bool Type::IsFunctionPtr()
{
    Type* x = this;
    assert(x);
    return (x->IsPtr() && x->BaseType()->btp && x->BaseType()->btp->IsFunction());
}
CONSTEXPR inline bool __isstructured(Type* x)
{
    return ((x)->type == BasicType::class_ || (x)->type == BasicType::struct_ || (x)->type == BasicType::union_);
}

bool Type::IsStructured()
{
    Type* x = this;
    assert(x);
    return (__isstructured(x->BaseType()));
}
bool Type::IsStructuredMath(Type* tp2)
{
    auto righttp = this;
    if (Optimizer::cparams.prm_cplusplus)
    {
        if (righttp->IsStructured() || righttp->BaseType()->type == BasicType::enum_ || (righttp->IsInt() && righttp->scoped))
        {
            return true;
        }
        if (tp2 && (tp2->IsStructured() || tp2->BaseType()->type == BasicType::enum_ || (tp2->IsInt() && tp2->scoped)))
        {
            return true;
        }
    }
    return false;
}
bool Type::IsSmallInt()
{
    auto tp = this;
    tp = tp->BaseType();
    if (tp->type < BasicType::int_)
    {
        return true;
    }
    else if (tp->IsBitInt())
    {
        int sz = getSize(BasicType::int_) * CHAR_BIT;
        return tp->bitintbits < sz;
    }
    else
    {
        return false;
    }
}
bool Type::IsLargeEnum()
{
    auto tp = this;
    tp = tp->BaseType();
    if (tp->type > BasicType::int_)
    {
        if (tp->IsBitInt())
        {
            int sz = getSize(BasicType::int_) * CHAR_BIT;
            return tp->bitintbits > sz || (tp->bitintbits == sz && tp->type == BasicType::unsigned_bitint_);
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}
bool Type::IsTemplatedPointer()
{
    auto tp = this;
    assert(tp);
    Type* tpb = tp->BaseType()->btp;
    while (tp != tpb)
    {
        if (tp->templateTop)
            return true;
        tp = tp->btp;
    }
    return false;
}
void Type::UpdateRootTypes()
{
    Type* tp = this;
    assert(tp);
    while (tp)
    {
        Type* tp1 = RootType(tp);
        while (tp && tp1 != tp)
        {
            tp->rootType = tp1;
            tp = tp->btp;
        }
        if (tp)
        {
            tp->rootType = tp;
            tp = tp->btp;
        }
    }
}
Type* Type::InitializerListType()
{
    auto arg = this;
    SYMBOL* sym = namespacesearch("std", globalNameSpace, false, false);
    if (sym && sym->sb->storage_class == StorageClass::namespace_)
    {
        sym = namespacesearch("initializer_list", sym->sb->nameSpaceValues, true, false);
        if (sym)
        {
            std::list<TEMPLATEPARAMPAIR>* tplp = templateParamPairListFactory.CreateList();
            auto tpl = Allocate<TEMPLATEPARAM>();
            tpl->type = TplType::typename_;
            tpl->byClass.dflt = arg;
            tplp->push_back(TEMPLATEPARAMPAIR{nullptr, tpl});
            auto sym1 = GetClassTemplate(sym, tplp, true);
            if (sym1)
            {
                sym1 = TemplateClassInstantiate(sym1, tplp, false, StorageClass::auto_);
                if (sym1)
                    sym = sym1;
            }
        }
    }
    Type* rtp;
    if (sym)
    {
        rtp = sym->tp;
    }
    else
    {
        rtp = Type::MakeType(BasicType::struct_);
        rtp->sp = makeID(StorageClass::type_, rtp, nullptr, "initializer_list");
        rtp->sp->sb->initializer_list = true;
    }
    return rtp;
}
bool Type::InstantiateDeferred(bool noErr, bool override)
{
    if (!IsDefiningTemplate() || override)
    {
        auto tp = this;
        assert(tp);
        tp = tp->BaseType();
        assert(tp);
        while (tp && tp->type != BasicType::templatedeferredtype_)
        {
            tp = tp->btp;
            if (tp)
                tp = tp->BaseType();
        }
        if (tp)
        {
            bool asTypeDef = false;
            TemplateArgInstantiateDeferred(tp->templateArgs, true);
            SYMBOL* sym;
            if (tp->sp->sb && tp->sp->sb->storage_class == StorageClass::typedef_ && tp->sp->sb->templateLevel)
            {
                asTypeDef = true;
                sym = GetTypeAliasSpecialization(tp->sp, tp->templateArgs);
            }
            else
            {
                sym = GetClassTemplate(tp->sp, tp->templateArgs, noErr);
            }
            if (sym)
            {
                auto args = sym->templateParams;
                if (!args && tp->sp->sb->parentClass)
                {
                    args = tp->sp->sb->parentClass->templateParams;
                }
                if (args && allTemplateArgsSpecified(sym, sym->templateParams, false, true))
                {
                    auto old = sym->templateParams;
                    if (!asTypeDef)
                    {
                        auto sym1 = sym;
                        sym1->templateParams = args;
                        sym = TemplateClassInstantiateInternal(sym, nullptr, false);
                        sym1->templateParams = old;
                    }
                }
                *tp = *sym->tp;
                this->UpdateRootTypes();

                return true;
            }
        }
        else if (this->IsStructured() && strchr(this->BaseType()->sp->sb->decoratedName, MANGLE_DEFERRED_TYPE_CHAR))
        {
            TemplateArgInstantiateDeferred(this->BaseType()->sp->templateParams);
            this->InitializeDeferred();
            SetLinkerNames(this->BaseType()->sp, Linkage::cpp_);
            return true;
        }
    }
    return false;
}
Type* Type::InitializeDeferred()
{
    Type* tp = this;
    assert(tp);
    Type** tpx = &tp;
    if ((*tpx)->IsRef())
        tpx = &(*tpx)->BaseType()->btp;
    Type** tpx1 = tpx;
    while ((*tpx)->btp && !(*tpx)->IsFunction())
        tpx = &(*tpx)->btp;
    if (Optimizer::cparams.prm_cplusplus && !inTemplateType && (*tpx)->IsStructured())
    {
        SYMBOL* sym = (*tpx)->BaseType()->sp;
        if (sym->templateParams)
            for (auto&& tpl : *sym->templateParams)
                if (tpl.second->usedAsUnpacked)
                    return tp;
        if (declaringTemplate(sym))
        {
            if (!sym->sb->instantiated)
            {
                *tpx = sym->tp;
            }
        }
        else if (sym->sb->templateLevel && (!sym->sb->instantiated || sym->sb->attribs.inheritable.linkage4 != Linkage::virtual_) &&
                 sym->templateParams && allTemplateArgsSpecified(sym, sym->templateParams, false, true))
        {
            sym = TemplateClassInstantiateInternal(sym, nullptr, false);
            if (sym)
                *tpx = sym->tp;
        }
        else if (!sym->sb->templateLevel && sym->sb->parentClass && sym->sb->parentClass->sb->templateLevel &&
                 (!sym->sb->instantiated || sym->sb->attribs.inheritable.linkage4 != Linkage::virtual_) &&
                 sym->sb->parentClass->templateParams &&
                 allTemplateArgsSpecified(sym->sb->parentClass, sym->sb->parentClass->templateParams, false, true))
        {
            std::list<TEMPLATEPARAMPAIR>* tpl = sym->sb->parentClass->templateParams;
            sym->templateParams = tpl;
            sym = TemplateClassInstantiateInternal(sym, nullptr, false);
            sym->templateParams = nullptr;
            *tpx = sym->tp;
        }
        else if (!sym->sb->instantiated || ((*tpx)->size < sym->tp->size && sym->tp->size != 0))
        {
            *tpx = sym->tp;
        }
        tp->UpdateRootTypes();
    }
    return tp;
}
Type* Type::MakeType(Type& tp, BasicType type, Type* base)
{
    tp.type = type;
    tp.btp = base;
    tp.rootType = &tp;
    tp.size = getSize(type);
    switch (type)
    {
        case BasicType::void_:
        case BasicType::ellipse_:
        case BasicType::memberptr_:
            break;
        case BasicType::any_:
            tp.size = getSize(BasicType::int_);
            break;
        case BasicType::far_:
        case BasicType::near_:
        case BasicType::const_:
        case BasicType::va_list_:
        case BasicType::objectArray_:
        case BasicType::volatile_:
        case BasicType::restrict_:
        case BasicType::static_:
        case BasicType::atomic_:
        case BasicType::typedef_:
        case BasicType::lrqual_:
        case BasicType::rrqual_:
        case BasicType::derivedfromtemplate_:
            if (base)
            {
                tp.rootType = base->rootType;
                if (tp.rootType)
                    tp.size = tp.rootType->size;
            }
            break;

        case BasicType::lref_:
        case BasicType::rref_:
        case BasicType::func_:
        case BasicType::ifunc_:
        case BasicType::object_:
            tp.size = getSize(BasicType::pointer_);
            break;
        case BasicType::enum_:
            tp.size = getSize(BasicType::int_);
            break;
        default:
            tp.size = getSize(type);
            break;
    }
    return &tp;
}
Type* Type::MakeType(BasicType type, Type* base)
{
    Type* rv = Allocate<Type>();
    return Type::MakeType(*rv, type, base);
}
Type* Type::MakeType(SYMBOL* sp, std::list<TEMPLATEPARAMPAIR>* args)
{
    Type* rv = Allocate<Type>();
    rv = Type::MakeType(*rv, BasicType::templatedeferredtype_, nullptr);
    rv->sp = sp;
    rv->templateArgs = args;
    return rv;
}
Type* Type::CopyType(bool deep, std::function<void(Type*&, Type*&)> callback)
{
    Type* tp = this;
    Type* rv = nullptr;
    if (deep)
    {
        Type** last = &rv;
        for (; tp; tp = tp->btp, last = &(*last)->btp)
        {
            *last = Allocate<Type>();
            **last = *tp;
            callback ? callback(tp, *last) : (void)0;
        }
        rv->UpdateRootTypes();
    }
    else
    {
        rv = Allocate<Type>();
        *rv = *tp;
        if (tp->rootType == tp)
            rv->rootType = rv;
    }
    return rv;
}
bool Type::CompareTypes(Type* typ1, Type* typ2, int exact)
{
    if (typ1->type == BasicType::any_ || typ2->type == BasicType::any_)
        return true;
    while (typ1->type == BasicType::typedef_)
        typ1 = typ1->BaseType();
    while (typ2->type == BasicType::typedef_)
        typ2 = typ2->BaseType();
    if (typ1->type == BasicType::derivedfromtemplate_)
        typ1 = typ1->btp;
    if (typ2->type == BasicType::derivedfromtemplate_)
        typ2 = typ2->btp;
    while (typ1->IsRef())
        typ1 = typ1->BaseType()->btp;
    while (typ2->IsRef())
        typ2 = typ2->BaseType()->btp;
    while (typ1->type == BasicType::typedef_)
        typ1 = typ1->BaseType();
    while (typ2->type == BasicType::typedef_)
        typ2 = typ2->BaseType();
    if (typ1->type == BasicType::templateselector_ && typ2->type == BasicType::templateselector_)
        return templateselectorcompare(typ1->sp->sb->templateSelector, typ2->sp->sb->templateSelector);
    if (typ1->type == BasicType::templatedecltype_ && typ2->type == BasicType::templatedecltype_)
        return templatecompareexpressions(typ1->templateDeclType, typ2->templateDeclType);
    if (typ1->IsPtr() && typ2->IsPtr())
    {
        if (exact)
        {
            int arr = false;
            int first = true;
            while (typ1->IsPtr() && typ2->IsPtr())
            {
                if (!first && (exact == 1))
                    if ((typ2->IsConst() && !typ1->IsConst()) || (typ2->IsVolatile() && !typ1->IsVolatile()))
                        return false;
                first = false;
                typ1 = typ1->BaseType();
                typ2 = typ2->BaseType();
                if (typ1->type != typ2->type)
                    return false;
                if (typ1->msil != typ2->msil)
                    return false;
                if (arr && (typ1->array != typ2->array))
                    return false;
                if (arr && typ1->size != typ2->size)
                    return false;
                arr |= typ1->array | typ2->array;
                typ1 = typ1->btp;
                typ2 = typ2->btp;
            }
            if (exact == 1 && ((typ2->IsConst() && !typ1->IsConst()) || (typ2->IsVolatile() && !typ1->IsVolatile())))
                return false;
            return typ1->CompatibleType(typ2);
        }

        else
            return true;
    }
    typ1 = typ1->BaseType();
    typ2 = typ2->BaseType();
    if (exact && (typ1->IsFunction() || typ1->IsFunctionPtr()) && (typ2->IsFunction() || typ2->IsFunctionPtr()))
    {
        typ1 = typ1->BaseType();
        typ2 = typ2->BaseType();
        if (typ1->IsPtr())
            typ1 = typ1->BaseType()->btp;
        if (typ2->IsPtr())
            typ2 = typ2->BaseType()->btp;
        if (typ1->btp->IsRef() != typ2->btp->IsRef() || !CompareTypes(typ1->btp, typ2->btp, exact))
            return false;
        if (!matchOverload(typ1, typ2))
            return false;
        return true;
    }
    if (Optimizer::cparams.prm_cplusplus)
    {
        if (typ1->scoped != typ2->scoped)
            return false;
        if (typ1->type == BasicType::enum_)
        {
            if (typ2->type == BasicType::enum_)
                return typ1->sp == typ2->sp;
            else
                return typ2->IsInt();
        }
        else if (typ2->type == BasicType::enum_)
        {
            return typ1->IsInt();
        }
        if (typ1->type == typ2->type && typ1->type == BasicType::memberptr_)
        {
            if (typ1->sp != typ2->sp)
            {
                if (classRefCount(typ1->sp, typ2->sp) != 1)
                    return false;
            }
            return CompareTypes(typ1->btp, typ2->btp, exact);
        }
    }
    if (typ1->type == typ2->type && typ1->type == BasicType::string_)
        return true;
    if (typ1->type == BasicType::object_)  // object matches anything
        return true;
    if (typ1->type == typ2->type && (typ1->IsStructured() || (exact && typ1->type == BasicType::enum_)))
    {
        SYMBOL* s1 = typ1->sp;
        SYMBOL* s2 = typ2->sp;
        if (s1->sb && s1->sb->mainsym)
            s1 = s1->sb->mainsym;
        if (s2->sb && s2->sb->mainsym)
            s2 = s2->sb->mainsym;
        return s1 == s2;
    }
    if (typ1->type == typ2->type && typ1->IsDeferred())
    {
        SYMBOL* s1 = typ1->sp;
        SYMBOL* s2 = typ2->sp;
        if (s1->sb && s1->sb->mainsym)
            s1 = s1->sb->mainsym;
        if (s2->sb && s2->sb->mainsym)
            s2 = s2->sb->mainsym;
        return s1 == s2 && exactMatchOnTemplateArgs(typ2->templateArgs, typ1->templateArgs);
    }
    if (typ1->type == typ2->type || (!exact && typ2->IsArithmetic() && typ1->IsArithmetic()))
        return typ1->bitintbits == typ2->bitintbits;
    if (typ1->IsFunction() && typ2->IsFunction() &&
        typ1->sp->sb->attribs.inheritable.linkage == typ2->sp->sb->attribs.inheritable.linkage)
        return true;
    else if (typ1->type == BasicType::memberptr_ && typ2->IsFunction())
    {
        return typ2->sp->sb->parentClass && CompareTypes(typ1->sp->tp, typ2->sp->sb->parentClass->tp, exact);
    }
    else if (!exact && ((typ1->IsPtr() && (typ2->IsFunctionPtr() || typ2->IsFunction() || typ2->IsInt())) ||
                        (typ2->IsPtr() && (typ1->IsFunctionPtr() || typ1->IsFunction() || typ1->IsInt()))))
        return (true);
    else if (typ1->type == BasicType::enum_ && typ2->IsInt())
    {
        return true;
    }
    else if (typ2->type == BasicType::enum_ && typ1->IsInt())
    {
        return true;
    }
    return false;
}
// this was introduced in c++17, but, since it is a major error if you do this I'm just importing it into all C++ dialects...
bool Type::SameExceptionType(Type* typ2)
{
    Type* typ1 = this;
    if (typ1->IsFunctionPtr())
    {
        if (typ2->IsFunction() || typ2->IsFunctionPtr())
        {
            if (typ2->IsFunction())
            {
                StatementGenerator::ParseNoExceptClause(typ2->BaseType()->sp);
            }
            while (typ2->IsPtr())
            {
                typ2 = typ2->BaseType()->btp;
            }
            while (typ1->IsPtr())
            {
                typ1 = typ1->BaseType()->btp;
            }
            typ1 = typ1->BaseType();
            typ2 = typ2->BaseType();
            if (typ1->IsFunction())
            {
                StatementGenerator::ParseNoExceptClause(typ1->sp);
            }
            if (typ2->IsFunction())
            {
                StatementGenerator::ParseNoExceptClause(typ2->sp);
            }
            if (typ1->sp->sb->noExcept && !typ2->sp->sb->noExcept)
                return false;
        }
    }
    return true;
}
bool Type::SameIntegerType(Type* t2)
{
    auto t1 = this;
    while (t1->IsRef())
        t1 = t1->BaseType()->btp;
    while (t2->IsRef())
        t2 = t2->BaseType()->btp;

    while (t1->IsPtr() && t2->IsPtr())
    {
        t1 = t1->BaseType()->btp;
        t2 = t2->BaseType()->btp;
    }
    return t1->type == t2->type;
}
bool Type::SameCharType(Type* typ2)
{
    Type* typ1 = this;
    if (typ1->IsRef())
        typ1 = typ1->BaseType()->btp;
    if (typ2->IsRef())
        typ2 = typ2->BaseType()->btp;

    while (typ1->IsPtr() && typ2->IsPtr())
    {
        typ1 = typ1->BaseType()->btp;
        typ2 = typ2->BaseType()->btp;
    }
    typ1 = typ1->BaseType();
    typ2 = typ2->BaseType();
    if (typ1->type == BasicType::char_)
    {
        if (Optimizer::cparams.prm_charisunsigned)
        {
            if (typ2->type == BasicType::unsigned_char_)
                return true;
        }
        else
        {
            if (typ2->type == BasicType::signed_char_)
                return true;
        }
    }
    else if (typ2->type == BasicType::char_)
    {
        if (Optimizer::cparams.prm_charisunsigned)
        {
            if (typ1->type == BasicType::unsigned_char_)
                return true;
        }
        else
        {
            if (typ1->type == BasicType::signed_char_)
                return true;
        }
    }
    else if (typ1->type == BasicType::unsigned_short_ || typ1->type == BasicType::wchar_t_)
        return typ2->type == BasicType::unsigned_short_ || typ2->type == BasicType::wchar_t_;
    return false;
}
bool Type::IsConstWithArr()
{
    auto tp = this;
    tp = tp->BaseType();
    while (tp->array)
    {
        tp = tp->btp;
        if (!tp->IsPtr() || !tp->BaseType()->array)
            break;
        else
            tp = tp->BaseType();
    }
    return tp->IsConst();
}
char* Type::PutPointer(char *top, char* p, Type* tp)
{
    *p = 0;
    if (tp->type == BasicType::far_)
        Optimizer::my_sprintf(p, top - p, "far ");
    p = p + strlen(p);
    if (tp->array)
        if (tp->btp->size && (!tp->esize || tp->esize->type == ExpressionNode::c_i_))
        {
            Optimizer::my_sprintf(p, top - p, "[%d]", tp->size / tp->btp->size);
        }
        else
        {
            Optimizer::my_sprintf(p, top - p, "[]");
        }
    else if (tp->vla)
        Optimizer::my_sprintf(p, top - p, "[*]");
    else
        Optimizer::my_sprintf(p, top - p, " *");
    return p + strlen(p);
}

Type* Type::QualifierToString(char* top, char* buf, Type* tp)
{
    while (tp && (tp->IsConst() || tp->IsVolatile() || tp->IsRestrict() || tp->type == BasicType::derivedfromtemplate_))
    {
        switch (tp->type)
        {
            case BasicType::lrqual_:
                Utils::StrCat(buf, top-buf, "& ");
                break;
            case BasicType::rrqual_:
                Utils::StrCat(buf, top-buf, "&& ");
                break;
            case BasicType::const_:
                Utils::StrCat(buf, top-buf, tn_const);
                break;
            case BasicType::volatile_:
                Utils::StrCat(buf, top-buf, tn_volatile);
                break;
            case BasicType::restrict_:
            case BasicType::derivedfromtemplate_:
                /*				Utils::StrCat(buf, tn_restrict); */
                break;
            default:
                break;
        }
        tp = tp->btp;
    }
    return tp;
}
void Type::PointerToString(char *top, char* buf, Type* tp)
{
    char bf[256], *p = bf;
    p = PutPointer(top, bf, tp);
    tp = QualifierToString(top, buf, tp->btp);
    if (!tp)
        return;
    while (tp->IsPtr())
    {
        p = PutPointer(top, p, tp);
        tp = QualifierToString(top, buf, tp->btp);
    }
    BasicTypeToString(top, buf, tp);
    Utils::StrCat(buf, top - buf, bf);
}
void Type::RenderExpr(char *top, char* buf, EXPRESSION* exp)
{
    (void)exp;
    Utils::StrCpy(buf, top - buf, "decltype(...)");
}
Type* Type::BasicTypeToString(char *top, char* buf, Type* tp)
{
    SYMBOL* sym;
    char name[UNMANGLE_BUFFER_SIZE];
    if (tp == nullptr)
    {
        diag("Type::Enum - nullptr type");
        return &stdvoid;
    }
    if (tp->type == BasicType::derivedfromtemplate_)
        tp = tp->btp;
    tp = QualifierToString(top, buf, tp);
    if (!tp)
        return nullptr;
    buf += strlen(buf);
    switch (tp->type)
    {
        case BasicType::typedef_:
            return BasicTypeToString(top, buf, tp->btp);
            break;
        case BasicType::aggregate_:
            if (!tp->syms)
                break;
            sym = tp->syms->front();
            if (tp->syms->size() > 1 ||
                !strcmp(sym->name, tp->sp->name))  // the tail is to prevent a problem when there are a lot of errors
            {
                Utils::StrCpy(buf, top - buf, " (*)(\?\?\?)");
                break;
            }
            tp = sym->tp;
            /* fall through */
        case BasicType::func_:
        case BasicType::ifunc_:
            BasicTypeToString(top, buf, tp->btp);
            buf = buf + strlen(buf);
            if (tp->syms)
            {
                auto it = tp->syms->begin();
                if (it != tp->syms->end() && *it)
                {
                    if ((*it)->sb->thisPtr)
                    {
                        SYMBOL* thisptr = *it;
                        *buf++ = ' ';
                        *buf++ = '(';
                        char temp[4000];
                        getcls(temp, thisptr->tp->BaseType()->btp->BaseType()->sp);
                        Utils::StrCat(buf, top - buf, temp);
                        Utils::StrCat(buf, top - buf, "::*)(");
                        buf += strlen(buf);
                        ++it;
                    }
                    else
                    {
                        Utils::StrCat(buf, top - buf, " (*)(");
                        buf += strlen(buf);
                    }
                }
                else
                {
                    Utils::StrCat(buf, top - buf, " (*)(");
                    buf += strlen(buf);
                }
                bool cleanup = it != tp->syms->end();
                while (it != tp->syms->end())
                {
                    sym = *it;
                    *buf = 0;
                    BasicTypeToString(top, buf, sym->tp);
                    buf = buf + strlen(buf);
                    *buf++ = ',';
                    ++it;
                }
                if (cleanup)
                    buf--;
            }
            else
            {
                Utils::StrCat(buf, top-buf, " (*)(");
                buf += strlen(buf);
            }
            *buf++ = ')';
            *buf = 0;
            break;
        case BasicType::float__complex_:
            Utils::StrCpy(buf, top-buf, tn_floatcomplex);
            break;
        case BasicType::double__complex_:
            Utils::StrCpy(buf, top-buf, tn_doublecomplex);
            break;
        case BasicType::long_double_complex_:
            Utils::StrCpy(buf, top-buf, tn_longdoublecomplex);
            break;
        case BasicType::float__imaginary_:
            Utils::StrCpy(buf, top-buf, tn_floatimaginary);
            break;
        case BasicType::double__imaginary_:
            Utils::StrCpy(buf, top-buf, tn_doubleimaginary);
            break;
        case BasicType::long_double_imaginary_:
            Utils::StrCpy(buf, top-buf, tn_longdoubleimaginary);
            break;
        case BasicType::float_:
            Utils::StrCpy(buf, top-buf, tn_float);
            break;
        case BasicType::double_:
            Utils::StrCpy(buf, top-buf, tn_double);
            break;
        case BasicType::long_double_:
            Utils::StrCpy(buf, top-buf, tn_longdouble);
            break;
        case BasicType::unsigned_:
            Utils::StrCpy(buf, top-buf, tn_unsigned);
            buf = buf + strlen(buf);
            Utils::StrCpy(buf, top-buf, tn_int);
            break;
        case BasicType::int_:
            Utils::StrCpy(buf, top-buf, tn_int);
            break;
        case BasicType::char8_t_:
            Utils::StrCpy(buf, top-buf, tn_char8_t);
            break;
        case BasicType::char16_t_:
            Utils::StrCpy(buf, top-buf, tn_char16_t);
            break;
        case BasicType::char32_t_:
            Utils::StrCpy(buf, top-buf, tn_char32_t);
            break;
        case BasicType::unsigned_long_long_:
            Utils::StrCpy(buf, top-buf, tn_unsigned);
            buf = buf + strlen(buf);
            Utils::StrCpy(buf, top-buf, tn_longlong);
            break;
        case BasicType::long_long_:
            Utils::StrCpy(buf, top-buf, tn_longlong);
            break;
        case BasicType::unsigned_long_:
            Utils::StrCpy(buf, top-buf, tn_unsigned);
            buf = buf + strlen(buf);
            Utils::StrCpy(buf, top-buf, tn_long);
            break;
        case BasicType::long_:
            Utils::StrCpy(buf, top-buf, tn_long);
            break;
        case BasicType::wchar_t_:
            Utils::StrCpy(buf, top-buf, tn_wchar_t);
            break;
        case BasicType::unsigned_short_:
            Utils::StrCpy(buf, top-buf, tn_unsigned);
            buf = buf + strlen(buf);
            Utils::StrCpy(buf, top-buf, tn_short);
            break;
        case BasicType::short_:
            Utils::StrCpy(buf, top-buf, tn_short);
            break;
        case BasicType::signed_char_:
            Utils::StrCpy(buf, top-buf, tn_signed);
            buf = buf + strlen(buf);
            Utils::StrCpy(buf, top-buf, tn_char);
            break;
        case BasicType::unsigned_char_:
            Utils::StrCpy(buf, top-buf, tn_unsigned);
            buf = buf + strlen(buf);
            Utils::StrCpy(buf, top-buf, tn_char);
            break;
        case BasicType::char_:
            Utils::StrCpy(buf, top-buf, tn_char);
            break;
        case BasicType::bool_:
            Utils::StrCpy(buf, top-buf, tn_bool);
            break;
        case BasicType::bit_:
            Utils::StrCpy(buf, top-buf, "bit");
            break;
        case BasicType::inative_:
            Utils::StrCpy(buf, top-buf, "native int");
            break;
        case BasicType::unative_:
            Utils::StrCpy(buf, top-buf, "native unsigned int");
            break;
        case BasicType::bitint_:
            Utils::StrCpy(buf, top-buf, "_Bitint");
            sprintf(buf + strlen(buf), "(%d)", tp->bitintbits);
            break;
        case BasicType::unsigned_bitint_:
            Utils::StrCpy(buf, top-buf, "unsigned _Bitint");
            sprintf(buf + strlen(buf), "(%d)", tp->bitintbits);
            break;
        case BasicType::void_:
            Utils::StrCpy(buf, top-buf, tn_void);
            break;
        case BasicType::string_:
            Utils::StrCpy(buf, top-buf, "__string");
            break;
        case BasicType::object_:
            Utils::StrCpy(buf, top-buf, "__object");
            break;
        case BasicType::pointer_:
            if (tp->nullptrType)
            {
                Utils::StrCpy(buf, top-buf, "nullptr_t");
            }
            else
            {
                PointerToString(top, buf, tp);
            }
            break;
        case BasicType::memberptr_:
            if (tp->BaseType()->btp->IsFunction())
            {
                Type* func = tp->BaseType()->btp;
                BasicTypeToString(top, buf, func->BaseType()->btp);
                Utils::StrCat(buf, top-buf, " (");
                buf += strlen(buf);
                char temp[4000];
                temp[0] = 0;
                getcls(temp, tp->sp);
                Utils::StrCpy(buf, top-buf, temp);
                buf += strlen(buf);
                Utils::StrCpy(buf, top-buf, "::*)(");
                buf += strlen(buf);
                if (func->BaseType()->syms)
                {
                    for (auto sym : *func->BaseType()->syms)
                    {
                        *buf = 0;
                        BasicTypeToString(top, buf, sym->tp);
                        buf = buf + strlen(buf);
                    }
                    if (func->BaseType()->syms->size())
                        buf--;
                }
                *buf++ = ')';
                *buf = 0;
            }
            else
            {
                BasicTypeToString(top, buf, tp->btp);
                Utils::StrCat(buf, top-buf, " ");
                buf += strlen(buf);
                char temp[4000];
                getcls(temp, tp->sp);
                Utils::StrCpy(buf, top - buf, temp);
                buf += strlen(buf);
                Utils::StrCpy(buf, top-buf, "::*");
            }
            break;
        case BasicType::seg_:
            BasicTypeToString(top, buf, tp->btp);
            buf += strlen(buf);
            Utils::StrCpy(buf, top-buf, " _seg *");
            break;
        case BasicType::lref_:
            BasicTypeToString(top, buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf = 0;
            break;
        case BasicType::rref_:
            BasicTypeToString(top, buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf++ = '&';
            *buf = 0;
            break;
        case BasicType::ellipse_:
            Utils::StrCpy(buf, top-buf, tn_ellipse);
            break;
        case BasicType::any_:
            Utils::StrCpy(buf, top-buf, "???");
            break;
        case BasicType::class_:
            /*                Utils::StrCpy(buf, top-buf, tn_class); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            Utils::StrCpy(buf, top-buf, name);
            break;
        case BasicType::struct_:
            /*                Utils::StrCpy(buf, top-buf, tn_struct); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            Utils::StrCpy(buf, top-buf, name);
            break;
        case BasicType::union_:
            /*                Utils::StrCpy(buf, top-buf, tn_union); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            Utils::StrCpy(buf, top-buf, name);
            break;
        case BasicType::enum_:
            /*                Utils::StrCpy(buf, top-buf, tn_enum);  */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            Utils::StrCpy(buf, top-buf, name);
            break;
        case BasicType::templateselector_: {
            auto itts = tp->sp->sb->templateSelector->begin();
            ++itts;
            if (itts->sp)
            {
                Utils::StrCpy(buf, top-buf, itts->sp->name);
                for (++itts; itts != tp->sp->sb->templateSelector->end(); ++itts)
                {
                    Utils::StrCat(buf, top-buf, "::");
                    Utils::StrCat(buf, top-buf, itts->name);
                }
            }
            break;
        }
        case BasicType::templatedecltype_:
            RenderExpr(top, buf, tp->templateDeclType);
            break;
        case BasicType::auto_:
            Utils::StrCpy(buf, top-buf, "auto ");
            break;
        default:
            Utils::StrCpy(buf, top-buf, "\?\?\?");
    }
    return 0;
}
void Type::ToString(char *top, char* buf, Type* typ)
{
    if (typ && (typ->IsFunction() || typ->IsFunctionPtr()))
    {
        while (typ->IsPtr())
            typ = typ->BaseType()->btp;
        typ = typ->BaseType();
        switch (typ->sp->sb->xcMode)
        {
            case xc_all:
                Utils::StrCpy(buf, top - buf, " noexcept(false)");
                break;
            case xc_none:
                Utils::StrCpy(buf, top - buf, " noexcept");
                break;
            case xc_dynamic:
                Utils::StrCpy(buf, top - buf, " throw(...)");
                break;
            case xc_unspecified:
                break;
        }
    }
}
void Type::ToString(char* top, char* buf)
{
    Type* typ = this;
    auto typ2 = typ;
    *buf = 0;
    while (typ)
    {
        typ = BasicTypeToString(top, buf, typ);
        buf = buf + strlen(buf);
        if (typ)
            *buf++ = ',';
    }
    *buf = 0;
    ToString(top, buf, typ2);
}

void TypeGenerator::ExceptionSpecifiers(SYMBOL* funcsp, SYMBOL* sp, StorageClass storage_class)
{
    (void)storage_class;
    switch (KW())
    {
        case Keyword::throw_:
            getsym();
            if (MATCHKW(Keyword::openpa_))
            {
                getsym();
                if (MATCHKW(Keyword::closepa_))
                {
                    sp->sb->xcMode = xc_none;
                    sp->sb->noExcept = true;
                    if (!sp->sb->xc)
                        sp->sb->xc = Allocate<xcept>();
                }
                else
                {
                    sp->sb->xcMode = xc_dynamic;
                    if (!sp->sb->xc)
                        sp->sb->xc = Allocate<xcept>();
                    --*currentStream;
                    do
                    {
                        Type* tp = nullptr;
                        getsym();
                        if (!MATCHKW(Keyword::closepa_))
                        {
                            tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
                            if (!tp)
                            {
                                error(ERR_TYPE_NAME_EXPECTED);
                            }
                            else
                            {
                                // this is reverse order but who cares?
                                Optimizer::LIST* p = Allocate<Optimizer::LIST>();
                                if (tp->type == BasicType::templateparam_ && tp->templateParam->second->packed)
                                {
                                    if (!MATCHKW(Keyword::ellipse_))
                                    {
                                        error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                    }
                                    else
                                    {
                                        getsym();
                                    }
                                }
                                p->next = sp->sb->xc->xcDynamic;
                                p->data = tp;
                                sp->sb->xc->xcDynamic = p;
                            }
                        }
                    } while (MATCHKW(Keyword::comma_));
                }
                needkw(Keyword::closepa_);
            }
            else
            {
                needkw(Keyword::openpa_);
            }
            break;
        case Keyword::noexcept_:
            getsym();
            if (MATCHKW(Keyword::openpa_))
            {
                getsym();
                auto stream = GetTokenStream(false);
                noExceptTokenStreams.set(sp, stream);
                needkw(Keyword::closepa_);
            }
            else
            {
                sp->sb->xcMode = xc_none;
                sp->sb->noExcept = true;
                noExceptTokenStreams.set(sp, (LexemeStream*)-1);
                if (!sp->sb->xc)
                    sp->sb->xc = Allocate<xcept>();
            }
            break;
        default:
            break;
    }
}
Type* TypeGenerator::FunctionQualifiersAndTrailingReturn(SYMBOL* funcsp, SYMBOL** sp, Type* tp,
                                                         StorageClass storage_class)
{
    bool foundFinal = false;
    bool foundOverride = false;
    bool done = false;
    bool foundConst = false;
    bool foundVolatile = false;
    bool foundand = false;
    bool foundland = false;
    while (currentLex && !done)
    {
        if (ISID())
        {
            if (!strcmp(currentLex->value.s.a, "final"))
            {
                if (foundFinal)
                    error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                foundFinal = true;
                (*sp)->sb->isfinal = true;
                getsym();
            }
            else if (!strcmp(currentLex->value.s.a, "override"))
            {
                if (foundOverride)
                    error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                foundOverride = true;
                (*sp)->sb->isoverride = true;
                getsym();
            }
            else
            {
                if (templateDefinitionLevel)
                {
                    while (currentLex && ISID())
                        getsym();
                }
                done = true;
            }
        }
        else
            switch (KW())
            {
                case Keyword::const_:
                    foundConst = true;
                    getsym();
                    break;
                case Keyword::volatile_:
                    foundVolatile = true;
                    getsym();
                    break;
                case Keyword::and_:
                    foundand = true;
                    getsym();
                    break;
                case Keyword::land_:
                    foundland = true;
                    getsym();
                    break;
                case Keyword::throw_:
                case Keyword::noexcept_:
                    if (Optimizer::cparams.prm_cplusplus && *sp)
                    {
                        funcNestingLevel++;
                        TypeGenerator::ExceptionSpecifiers(funcsp, *sp, storage_class);
                        funcNestingLevel--;
                    }
                    break;
                default:
                    done = true;
                    break;
            }
    }
    if (foundand && foundland)
        error(ERR_TOO_MANY_QUALIFIERS);
    if (foundVolatile)
    {
        tp = Type::MakeType(BasicType::volatile_, tp);
    }
    if (foundConst)
    {
        tp = Type::MakeType(BasicType::const_, tp);
    }
    if (foundand)
    {
        tp = Type::MakeType(BasicType::lrqual_, tp);
    }
    else if (foundland)
    {
        tp = Type::MakeType(BasicType::rrqual_, tp);
    }
    ParseAttributeSpecifiers(funcsp, true);
    if (MATCHKW(Keyword::pointsto_))
    {
        Type* tpx = nullptr;
        SymbolTable<SYMBOL>* locals = localNameSpace->front()->syms;
        localNameSpace->front()->syms = tp->BaseType()->syms;
        funcNestingLevel++;
        getsym();
        ParseAttributeSpecifiers(funcsp, true);
        processingTrailingReturnOrUsing++;
        auto old = theCurrentFunc;
        theCurrentFunc = *sp;
        tpx = TypeGenerator::TypeId(*sp, StorageClass::cast_, false, true, false);
        theCurrentFunc = old;
        processingTrailingReturnOrUsing--;
        // weed out temporary syms that were added as part of a decltype; they will be
        // reinstated as stackblock syms later
        auto itp = localNameSpace->front()->syms->begin();
        auto itpe = localNameSpace->front()->syms->end();
        while (itp != itpe)
        {
            SYMBOL* sym = *itp;
            auto it1 = itp;
            ++it1;
            if (sym->sb->storage_class != StorageClass::parameter_)
                localNameSpace->front()->syms->remove(itp);
            itp = it1;
        }
        if (tpx)
        {
            if (!tp->BaseType()->btp->IsAutoType())
                error(ERR_MULTIPLE_RETURN_TYPES_SPECIFIED);
            if (tpx->IsArray())
            {
                Type* tpn = Type::MakeType(BasicType::pointer_, tpx->BaseType()->btp);
                if (tpx->IsConst())
                {
                    tpn = Type::MakeType(BasicType::const_, tpn);
                }
                if (tpx->IsVolatile())
                {
                    tpn = Type::MakeType(BasicType::volatile_, tpn);
                }
                tp->BaseType()->btp = tpn;
                tp->BaseType()->rootType = tpn->rootType;
            }
            else
            {
                tp->BaseType()->btp = tpx;
                tp->BaseType()->rootType = tpx->rootType;
            }
        }
        localNameSpace->front()->syms = locals;
        funcNestingLevel--;
    }
    return tp;
}
void TypeGenerator::ResolveVLAs(Type* tp)
{
    while (tp && tp->type == BasicType::pointer_ && tp->array)
    {
        tp->vla = true;
        tp = tp->btp;
    }
}
Type* TypeGenerator::ArrayType(SYMBOL* funcsp, Type* tp, StorageClass storage_class, bool* vla, Type** quals,
                               bool first, bool msil)
{
    if (tp)
        tp->InstantiateDeferred();
    EXPRESSION* constant = nullptr;
    Type* tpc = nullptr;
    Type* typein = tp;
    bool unsized = false;
    bool empty = false;
    getsym(); /* past '[' */
    *quals = TypeGenerator::PointerQualifiers(*quals, true);
    if (MATCHKW(Keyword::star_))
    {
        RequiresDialect::Feature(Dialect::c99, "Variable Length Array");
        if (storage_class != StorageClass::parameter_)
            error(ERR_UNSIZED_VLA_PARAMETER);
        getsym();
        unsized = true;
    }
    else if (!MATCHKW(Keyword::closebr_))
    {
        expression_no_comma(funcsp, nullptr, &tpc, &constant, nullptr, 0);
        if (tpc)
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                RemoveSizeofOperators(constant);
            }
            optimize_for_constants(&constant);
        }
        *quals = TypeGenerator::PointerQualifiers(*quals, true);
        if (!tpc)
        {
            tpc = &stdint;
            error(ERR_EXPRESSION_SYNTAX);
        }
        if (*quals)
        {
            RequiresDialect::Feature(Dialect::c99, "Array Qualifiers");
            for (auto t = *quals; t; t = t->btp)
                if (t->type == BasicType::static_)
                {
                    RequiresDialect::Feature(Dialect::c11, "'static' array qualifier");
                }
            if (storage_class != StorageClass::parameter_)
                error(ERR_ARRAY_QUAL_PARAMETER_ONLY);
        }
    }
    else
    {
        if (!first) /* previous was empty */
            error(ERR_ONLY_FIRST_INDEX_MAY_BE_EMPTY);
        empty = true;
    }
    if (MATCHKW(Keyword::closebr_))
    {
        Type *tpp, *tpb = tp;
        getsym();
        ParseAttributeSpecifiers(funcsp, true);
        if (MATCHKW(Keyword::openbr_))
        {
            if (*quals)
                error(ERR_QUAL_LAST_ARRAY_ELEMENT);
            tp = TypeGenerator::ArrayType(funcsp, tp, storage_class, vla, quals, false, msil);
        }
        tpp = Type::MakeType(BasicType::pointer_, tp);
        if (tpp->btp)
            tpp->btp->msil = msil;  // tag the base type as managed, e.g. so we can't take address of it
        tpp->array = true;
        tpp->unsized = unsized;
        tpp->msil = msil;
        if (!unsized)
        {
            if (empty)
            {
                if (storage_class == StorageClass::member_)
                    RequiresDialect::Feature(Dialect::c99, "Flexible Arrays");
                tpp->size = 0;
            }
            else
            {
                if (!tpc->IsInt())
                    error(ERR_ARRAY_INDEX_INTEGER_TYPE);
                else if (tpc->type != BasicType::templateparam_ && isintconst(constant) &&
                         constant->v.i <= 0 - !!enclosingDeclarations.GetFirst())
                    if (!templateDefinitionLevel)
                        error(ERR_ARRAY_INVALID_INDEX);
                if (tpc->type == BasicType::templateparam_)
                {
                    tpp->size = tpp->btp->BaseType()->size;
                    tpp->esize = MakeIntExpression(ExpressionNode::c_i_, 1);
                }
                else if (isarithmeticconst(constant))
                {
                    tpp->size = tpp->btp->BaseType()->size;
                    tpp->size *= constant->v.i;
                    tpp->elements = constant->v.i;
                    tpp->esize = MakeIntExpression(ExpressionNode::c_i_, constant->v.i);
                }
                else
                {
                    if (!templateDefinitionLevel && !msil)
                        RequiresDialect::Feature(Dialect::c99, "Variable Length Array");
                    tpp->esize = constant;
                    tpp->etype = tpc;
                    *vla = !msil && !templateDefinitionLevel;
                }
            }
            tp = tpp;
        }
        else
        {
            if (tpp->btp)
            {
                tpp->size = tpp->btp->size;
                tpp->esize = MakeIntExpression(ExpressionNode::c_i_, tpp->btp->size);
            }
            tp = tpp;
        }
        if (typein && typein->IsStructured())
        {
            checkIncompleteArray(typein, currentLex->sourceFileName, currentLex->sourceLineNumber);
        }
    }
    else
    {
        error(ERR_ARRAY_NEED_CLOSEBRACKET);
        errskim(skim_comma);
    }
    return tp;
}
Type* TypeGenerator::AfterName(SYMBOL* funcsp, Type* tp, SYMBOL** sp, bool inTemplate, StorageClass storage_class,
                               int consdest, bool funcptr)
{
    bool isvla = false;
    Type* quals = nullptr;
    Type* tp1 = nullptr;
    EXPRESSION* exp = nullptr;
    if (ISKW())
    {
        switch (KW())
        {
            case Keyword::openpa_:
                if (*sp)
                {
                    tp1 = tp = TypeGenerator::FunctionParams(funcsp, sp, tp, inTemplate, storage_class, funcptr);
                    if (tp1->type == BasicType::func_)
                    {
                        tp = tp->btp;
                        tp = TypeGenerator::AfterName(funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                        tp1->btp = tp;
                        tp = tp1;
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            tp = TypeGenerator::FunctionQualifiersAndTrailingReturn(funcsp, sp, tp, storage_class);
                        }
                    }
                    tp->UpdateRootTypes();
                }
                else
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                }
                break;
            case Keyword::openbr_:
                tp = TypeGenerator::ArrayType(funcsp, tp, (*sp) ? (*sp)->sb->storage_class : storage_class, &isvla, &quals,
                                              true, false);
                if (!tp)
                {
                    error(ERR_TYPE_NAME_EXPECTED);
                    tp = &stdint;
                }
                else if (isvla)
                {
                    TypeGenerator::ResolveVLAs(tp);
                }
                else if (tp->IsPtr() && tp->btp && tp->btp->vla)
                {
                    tp->vla = true;
                }
                if (quals)
                {
                    Type* q2 = quals;
                    while (q2->btp)
                        q2 = q2->btp;
                    q2->btp = tp;
                    tp = quals;
                }
                tp->UpdateRootTypes();
                break;
            case Keyword::colon_:
                if (consdest == CT_CONS)
                {
                    // defer to later
                }
                else if (*sp &&
                         ((*sp)->sb->storage_class == StorageClass::member_ || (*sp)->sb->storage_class == StorageClass::mutable_))
                {
                    // error(ERR_BIT_STRUCT_MEMBER);
                    if ((*sp)->tp)
                    {
                        // we aren't allowing _Bitint to participate as a bit field...
                        if (Optimizer::cparams.prm_ansi)
                        {
                            if ((*sp)->tp->type != BasicType::int_ && (*sp)->tp->type != BasicType::unsigned_ &&
                                (*sp)->tp->type != BasicType::bool_)
                                error(ERR_ANSI_INT_BIT);
                        }
                        else if (!(*sp)->tp->IsInt() && (*sp)->tp->BaseType()->type != BasicType::enum_)
                            error(ERR_BIT_FIELD_INTEGER_TYPE);
                    }
                    getsym();
                    optimized_expression(funcsp, nullptr, &tp1, &exp, false);
                    if (tp1 && exp && isintconst(exp))
                    {
                        int n = tp->size * Optimizer::chosenAssembler->arch->bits_per_mau;
                        Type **tp1 = &tp, *tpb = (*tp1)->BaseType();
                        *tp1 = tpb->CopyType();
                        (*tp1)->bits = exp->v.i;
                        (*tp1)->hasbits = true;
                        (*tp1)->anonymousbits = (*sp)->sb->anonymous;
                        (*tp1)->UpdateRootTypes();
                        if ((*tp1)->bits > n)
                            error(ERR_BIT_FIELD_TOO_LARGE);
                        else if ((*tp1)->bits < 0 || ((*tp1)->bits == 0 && !(*sp)->sb->anonymous))
                            error(ERR_BIT_FIELD_MUST_CONTAIN_AT_LEAST_ONE_BIT);
                    }
                    else
                    {
                        if (tp)
                            tp->bits = 1;
                        error(ERR_CONSTANT_VALUE_EXPECTED);
                        errskim(skim_semi_declare);
                    }
                }
                break;
            case Keyword::lt_:
                if (Optimizer::cparams.prm_cplusplus && inTemplate)
                {
                    std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(*sp);
                    GetTemplateArguments(funcsp, *sp, &templateParams->front().second->bySpecialization.types);
                    tp = TypeGenerator::AfterName(funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                }
                else
                {
                    TEMPLATEPARAM* templateParam = Allocate<TEMPLATEPARAM>();
                    templateParam->type = TplType::new_;
                    GetTemplateArguments(funcsp, *sp, &templateParam->bySpecialization.types);
                    tp = TypeGenerator::AfterName(funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                    if (tp && tp->IsFunction())
                    {
                        std::list<TEMPLATEPARAMPAIR>* lst = templateParamPairListFactory.CreateList();
                        lst->push_back(TEMPLATEPARAMPAIR{nullptr, templateParam});
                        DoInstantiateTemplateFunction(tp, sp, nullptr, nullptr, lst, true);
                        if ((*sp)->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
                            basisAttribs.inheritable.linkage4 = Linkage::virtual_;
                        if (!(*sp)->templateParams)
                        {
                            (*sp)->templateParams = lst;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    return tp;
}
Type* TypeGenerator::BeforeName(SYMBOL* funcsp, Type* tp, SYMBOL** spi, SYMBOL** strSym,
    std::list<NAMESPACEVALUEDATA*>** nsv, bool inTemplate, StorageClass storage_class, Linkage* linkage,
    Linkage* linkage2, Linkage* linkage3, bool* notype, bool asFriend, int consdest, bool beforeOnly,
    bool funcptr)
{
    SYMBOL* sp;
    Type* ptype = nullptr;
    BasicType xtype = BasicType::none_;
    LexemeStreamPosition pos(currentStream);
    bool doneAfter = false;

    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && MATCHKW(Keyword::openbr_))
    {
        // managed array
        bool isvla = false;
        Type* quals = nullptr;
        tp = TypeGenerator::ArrayType(funcsp, tp, storage_class, &isvla, &quals, true, true);
        if (quals)
        {
            Type* q2 = quals;
            while (q2->btp)
                q2 = q2->btp;
            q2->btp = tp;
            tp = quals;
        }
        tp->UpdateRootTypes();
        //        doneAfter = true;
    }
    if (ISID() || MATCHKW(Keyword::classsel_) || MATCHKW(Keyword::operator_) ||
        (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_)))
    {
        SYMBOL* strSymX = nullptr;
        std::list<NAMESPACEVALUEDATA*>* nsvX = nullptr;
        bool oldTemplateSpecialization = inTemplateSpecialization;
        inTemplateSpecialization = inTemplateType;
        inTemplateType = false;
        DeclarationScope scope;
        if (Optimizer::cparams.prm_cplusplus)
        {
            bool throughClass = false;
            bool pack = false;
            if (MATCHKW(Keyword::ellipse_))
            {
                pack = true;
                getsym();
            }
            nestedPath(&strSymX, &nsvX, &throughClass, false, storage_class, false, 0);
            inTemplateSpecialization = oldTemplateSpecialization;
            if (strSymX)
            {
                if (structLevel && strSymX->tp->type == BasicType::templateselector_ && !MATCHKW(Keyword::star_) &&
                    !MATCHKW(Keyword::complx_))
                {
                    tp = strSymX->tp;
                    strSymX = nullptr;
                    getsym();
                    if (notype)
                        *notype = false;
                }
                else
                {
                    if (strSym)
                        *strSym = strSymX;
                    enclosingDeclarations.Add(strSymX);
                }
            }
            if (nsv)
                *nsv = nsvX;
            if (strSymX && MATCHKW(Keyword::star_))
            {
                bool inparen = false;
                if (pack)
                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                getsym();
                if (funcptr && MATCHKW(Keyword::openpa_))
                {
                    inparen = true;
                    getsym();
                }
                ptype = Type::MakeType(BasicType::memberptr_, tp);
                if (strSymX->tp->type == BasicType::templateselector_)
                    ptype->sp = (*strSymX->sb->templateSelector)[1].sp;
                else
                    ptype->sp = strSymX;
                tp = ptype;
                tp->UpdateRootTypes();
                getQualifiers(&tp, linkage, linkage2, linkage3, nullptr);
                if (strSym)
                    *strSym = nullptr;
                if (nsv)
                    *nsv = nullptr;
                tp = TypeGenerator::BeforeName(funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2,
                    linkage3, nullptr, asFriend, false, beforeOnly, false);
                if (tp && (ptype != tp && tp->IsRef()))
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                ptype->size = getSize(BasicType::pointer_) + getSize(BasicType::int_) * 2;
                if (inparen)
                {
                    if (!needkw(Keyword::closepa_))
                    {
                        errskim(skim_closepa);
                        skip(Keyword::closepa_);
                    }
                }
            }
            else
            {
                char buf[512];
                buf[0] = '\0';
                int ov = 0;
                Type* castType = nullptr;
                if (MATCHKW(Keyword::complx_))
                {
                    getsym();
                    if (!ISID() || !*strSym || strcmp((*strSym)->name, currentLex->value.s.a))
                    {
                        error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                    }
                    else
                    {
                        Utils::StrCpy(buf, overloadNameTab[CI_DESTRUCTOR]);
                        consdest = CT_DEST;
                        tp = &stdvoid;
                    }
                }
                else
                {
                    getIdName(funcsp, buf, sizeof(buf), &ov, &castType);
                }
                if (!buf[0])
                {
                    if (!pack)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        sp = makeID(storage_class, tp, *spi, NewUnnamedID());
                        sp->packed = pack;
                        SetLinkerNames(sp, Linkage::none_);
                        *spi = sp;
                    }
                    else
                    {
                        pos.Backup();
                    }
                }
                else
                {
                    //                    if (consdest == CT_DEST)
                    //                    {
                    //                        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                    //                    }
                    if (ov == CI_CAST)
                    {
                        sp = makeID(storage_class, castType, *spi, litlate(buf));
                        sp->packed = pack;
                        tp = castType;
                        sp->sb->castoperator = true;
                    }
                    else
                    {
                        sp = makeID(storage_class, tp, *spi, litlate(buf));
                        sp->packed = pack;
                        getsym();
                    }
                    sp->sb->operatorId = ov;
                    if (currentLex)
                    {
                        sp->sb->declcharpos = currentLex->charindex;
                        sp->sb->realcharpos = currentLex->realcharindex;
                    }
                    *spi = sp;
                }
            }
        }
        else
        {
            inTemplateSpecialization = oldTemplateSpecialization;
            sp = makeID(storage_class, tp, *spi, litlate(currentLex->value.s.a));
            sp->sb->declcharpos = currentLex->charindex;
            sp->sb->realcharpos = currentLex->realcharindex;
            *spi = sp;
            getsym();
        }
        if (inTemplate && *spi)
        {
            std::list<TEMPLATEPARAMPAIR>* templateParams;
            SYMBOL* ssp = strSymX;
            if (!ssp)
                ssp = enclosingDeclarations.GetFirst();
            (*spi)->sb->parentClass = ssp;
            (*spi)->sb->templateLevel = templateDefinitionLevel;
            templateParams = TemplateGetParams(*spi);
            (*spi)->sb->templateLevel = 0;
            (*spi)->sb->parentClass = nullptr;
            if (templateParams)
            {
                enclosingDeclarations.Add(templateParams);
            }
        }
        if (nsvX && nsvX->front()->name)
        {
            Optimizer::LIST* nlist;

            nameSpaceList.push_front(nsvX->front()->name);

            globalNameSpace->push_front(nsvX->front()->name->sb->nameSpaceValues->front());
        }
        else
        {
            nsvX = nullptr;
        }
        ParseAttributeSpecifiers(funcsp, true);
        if (!doneAfter)
            tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, false);
        if (nsvX)
        {
            nameSpaceList.pop_front();
            globalNameSpace->pop_front();
        }
    }
    else
    {
        LexemeStreamPosition start(currentStream);
        switch (KW())
        {
        case Keyword::openpa_:
            if (beforeOnly)
                break;
            getsym();
            /* in a parameter, open paren followed by a type is an  unnamed function */
            if ((storage_class == StorageClass::parameter_ || processingUsingStatement) &&
                (MATCHKW(Keyword::closepa_) || (TypeGenerator::StartOfType(nullptr, false) &&
                    (!ISKW() || !(currentLex->kw->tokenTypes & TT_LINKAGE)))))
            {
                Type* tp1;
                if (!*spi)
                {
                    sp = makeID(storage_class, tp, *spi, NewUnnamedID());
                    SetLinkerNames(sp, Linkage::none_);
                    sp->sb->anonymous = true;
                    sp->sb->declcharpos = currentLex->charindex;
                    sp->sb->realcharpos = currentLex->realcharindex;
                    *spi = sp;
                }
                tp = (*spi)->tp;
                start.Backup();
                tp1 = tp;
                if (!processingUsingStatement)
                {
                    tp = TypeGenerator::FunctionParams(funcsp, spi, tp, inTemplate, storage_class, false);
                    tp1 = tp;
                    tp = tp->btp;
                }
                tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                if (!processingUsingStatement)
                {
                    tp1->btp = tp;
                    tp = tp1;
                }
                tp1->UpdateRootTypes();
                return tp;
            }
            else if (Optimizer::cparams.prm_cplusplus && consdest)
            {
                // constructor or destructor name
                const char* name;
                if (consdest == CT_DEST)
                {
                    tp = &stdvoid;
                    name = overloadNameTab[CI_DESTRUCTOR];
                }
                else
                {
                    tp = Type::MakeType(BasicType::pointer_, tp);
                    name = overloadNameTab[CI_CONSTRUCTOR];
                }
                sp = makeID(storage_class, tp, *spi, name);
                sp->sb->declcharpos = currentLex->charindex;
                sp->sb->realcharpos = currentLex->realcharindex;
                *spi = sp;
                if (*strSym)
                {
                    enclosingDeclarations.Add(*strSym);
                }
                start.Backup();
                tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                if (*strSym)
                {
                    enclosingDeclarations.Drop();
                }
            }
            else if (MATCHKW(Keyword::openbr_))
            {
                tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                needkw(Keyword::closepa_);
            }
            else
            {
                /* stdcall et.al. before the ptr means one thing in borland,
                 * stdcall after means another
                 * we are treating them both the same, e.g. the resulting
                 * pointer-to-function will be stdcall linkage either way
                 */
                attributes oldAttribs = basisAttribs;

                basisAttribs = { 0 };

                ParseAttributeSpecifiers(funcsp, true);
                getQualifiers(&tp, linkage, linkage2, linkage3, nullptr);
                ptype = TypeGenerator::BeforeName(funcsp, ptype, spi, strSym, nsv, inTemplate, storage_class, linkage,
                    linkage2, linkage3, nullptr, asFriend, false, beforeOnly, true);
                basisAttribs = oldAttribs;
                if (!needkw(Keyword::closepa_))
                {
                    errskim(skim_closepa);
                    skip(Keyword::closepa_);
                }
                if (ptype->IsFunctionPtr() && !MATCHKW(Keyword::openpa_) && !MATCHKW(Keyword::openbr_))
                {
                    auto  atype = tp->BaseType();
                    if (atype->type == BasicType::memberptr_ && atype->btp->IsFunction())
                        atype->size = getSize(BasicType::int_) * 2 + getSize(BasicType::pointer_);
                    auto tp1 = ptype;
                    ptype = ptype->BaseType()->btp->BaseType();
                    ptype->btp = tp;
                    tp = tp1;
                    tp->UpdateRootTypes();
                }
                else
                {
                    if (!ptype || (!ptype->IsRef() && !ptype->IsPtr() && !ptype->IsFunction() &&
                        ptype->BaseType()->type != BasicType::memberptr_))
                    {
                        // if here is not a potential pointer to func
                        if (!ptype)
                            tp = &stdint;
                        ptype = nullptr;
                    }
                    tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, true);
                    if (ptype)
                    {
                        // pointer to func or pointer to memberfunc
                        Type* atype = tp;
                        tp = ptype;
                        if (ptype->IsRef() && atype->BaseType()->array && storage_class != StorageClass::parameter_)
                            atype->BaseType()->byRefArray = true;
                        while ((ptype->IsRef() || ptype->IsFunction() || ptype->IsPtr() ||
                            ptype->BaseType()->type == BasicType::memberptr_) &&
                            ptype->btp)
                            if (ptype->btp->type == BasicType::any_)
                            {
                                break;
                            }
                            else
                            {
                                ptype = ptype->btp;
                            }
                        ptype->btp = atype;
                        ptype->rootType = atype->rootType;
                        tp->UpdateRootTypes();

                        atype = tp->BaseType();
                        if (atype->type == BasicType::memberptr_ && atype->btp->IsFunction())
                            atype->size = getSize(BasicType::int_) * 2 + getSize(BasicType::pointer_);
                    }
                }
                if (*spi)
                    (*spi)->tp = tp;
                if (tp && tp->IsFunction())
                    sizeQualifiers(tp->BaseType()->btp);
                if (tp)
                    sizeQualifiers(tp);
            }
            break;
        case Keyword::star_: {
            if (tp && tp->IsRef())
            {
                error(ERR_NO_REF_POINTER_REF);
            }
            bool inparen = false;
            getsym();
            if (funcptr && MATCHKW(Keyword::openpa_))
            {
                getsym();
                // this isn't perfect, it doesn't work with nested parens around the identifier

                if (!ISID())
                    --*currentStream;
                else
                    inparen = true;
            }
            ParseAttributeSpecifiers(funcsp, true);
            ptype = tp;
            while (ptype && ptype->type != BasicType::pointer_ && xtype == BasicType::none_)
            {
                if (ptype->type == BasicType::far_)
                    xtype = BasicType::pointer_;
                else if (ptype->type == BasicType::near_)
                    xtype = BasicType::pointer_;
                else if (ptype->type == BasicType::seg_)
                    xtype = BasicType::seg_;
                ptype = ptype->btp;
            }
            if (xtype == BasicType::none_)
                xtype = BasicType::pointer_;
            tp = Type::MakeType(xtype, tp);
            getQualifiers(&tp, linkage, linkage2, linkage3, nullptr);
            tp = TypeGenerator::BeforeName(funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2,
                linkage3, nullptr, asFriend, false, beforeOnly, false);
            if (inparen)
            {
                if (!needkw(Keyword::closepa_))
                {
                    errskim(skim_closepa);
                    skip(Keyword::closepa_);
                }
            }
        }
                           break;
        case Keyword::and_:
        case Keyword::land_:
            if (storage_class == StorageClass::catchvar_)
            {
                // already a ref;
                getsym();
                ParseAttributeSpecifiers(funcsp, true);
                getQualifiers(&tp, linkage, linkage2, linkage3, nullptr);
                tp = TypeGenerator::BeforeName(funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2,
                    linkage3, nullptr, asFriend, false, beforeOnly, false);
                break;
            }
            // using the C++ reference operator as the ref keyword...
            if (Optimizer::cparams.prm_cplusplus ||
                ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions &&
                    storage_class == StorageClass::parameter_ && KW() == Keyword::and_))
            {
                if (tp && tp->IsRef() && !templateInstantiationLevel && !templateDefinitionLevel)
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                tp = Type::MakeType(MATCHKW(Keyword::and_) ? BasicType::lref_ : BasicType::rref_, tp);
                if (templateInstantiationLevel)
                    CollapseReferences(tp);
                getsym();
                ParseAttributeSpecifiers(funcsp, true);
                getQualifiers(&tp, linkage, linkage2, linkage3, nullptr);
                tp = TypeGenerator::BeforeName(funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2,
                    linkage3, nullptr, asFriend, false, beforeOnly, false);
                if (storage_class != StorageClass::typedef_ && !tp->IsFunction() && !IsDefiningTemplate())
                {
                    auto tp2 = tp;
                    while (tp2 && tp2->type != BasicType::lref_ && tp2->type != BasicType::rref_)
                    {
                        if ((tp2->type == BasicType::const_ || tp2->type == BasicType::volatile_))
                        {
                            error(ERR_REF_NO_QUALIFIERS);
                            break;
                        }
                        tp2 = tp2->btp;
                    }
                }
            }
            break;
        case Keyword::colon_: /* may have unnamed bit fields */
            *spi = makeID(storage_class, tp, *spi, NewUnnamedID());
            SetLinkerNames(*spi, Linkage::none_);
            (*spi)->sb->anonymous = true;
            tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, false);
            break;
        case Keyword::gt_:
        case Keyword::comma_:
            break;
        default:
            if (beforeOnly)
                return tp;
            if (tp && (tp->IsStructured() || tp->type == BasicType::enum_) && KW() == Keyword::semicolon_)
            {
                tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                *spi = nullptr;
                return tp;
            }
            if (storage_class != StorageClass::parameter_ && storage_class != StorageClass::cast_ &&
                storage_class != StorageClass::catchvar_ && !asFriend)
            {
                if (MATCHKW(Keyword::openpa_) || MATCHKW(Keyword::openbr_) || MATCHKW(Keyword::assign_) ||
                    MATCHKW(Keyword::semicolon_))
                    error(ERR_IDENTIFIER_EXPECTED);
                else
                    errortype(ERR_IMPROPER_USE_OF_TYPE, tp, nullptr);
            }
            if (!tp)
            {
                *spi = nullptr;
                return tp;
            }
            *spi = makeID(storage_class, tp, *spi, NewUnnamedID());
            SetLinkerNames(*spi, Linkage::none_);
            (*spi)->sb->anonymous = true;
            tp = TypeGenerator::AfterName(funcsp, tp, spi, inTemplate, storage_class, consdest, false);
            break;
        }
    }
    if (tp && (ptype = tp->BaseType()))
    {
        if (ptype->IsFunctionPtr())
            ptype = ptype->btp->BaseType();
        if (ptype->IsFunction())
        {
            if (ptype->btp->type == BasicType::func_ ||
                (ptype->btp->type == BasicType::pointer_ && ptype->btp->array && !ptype->btp->msil))
                error(ERR_FUNCTION_NO_RETURN_FUNCTION_ARRAY);
        }
    }
    else
    {
        // an error occurred
        tp = &stdint;
    }
    ParseAttributeSpecifiers(funcsp, true);
    return tp;
}
Type* TypeGenerator::UnadornedType(SYMBOL* funcsp, Type* tp, SYMBOL** strSym_out, bool inTemplate,
                                   StorageClass storage_class, Linkage* linkage_in, Linkage* linkage2_in, Linkage* linkage3_in,
                                   AccessLevel access, bool* notype, bool* defd, int* consdest, bool* templateArg,
                                   bool* deduceTemplate, bool isTypedef, bool templateErr, bool inUsing, bool* asfriend,
                                   bool constexpression)
{
    int oldDeclaringInitialType = declaringInitialType;
    BasicType type = BasicType::none_;
    Type* tn = nullptr;
    Type* quals = nullptr;
    Type** tl;
    bool extended;
    bool iscomplex = false;
    bool imaginary = false;
    bool flagerror = false;
    bool foundint = false;
    int foundunsigned = 0;
    int foundsigned = 0;
    bool foundtypeof = false;
    bool foundsomething = false;
    bool int64 = false;
    Linkage linkage = Linkage::none_;
    Linkage linkage2 = Linkage::none_;
    Linkage linkage3 = Linkage::none_;
    int bitintbits = 0;

    *defd = false;
    while (KWTYPE(TT_BASETYPE) || MATCHKW(Keyword::decltype_))
    {
        if (foundtypeof)
            flagerror = true;
        switch (KW())
        {
            case Keyword::int_:
                if (foundint)
                    flagerror = true;
                foundint = true;
                switch (type)
                {
                    case BasicType::unsigned_:
                    case BasicType::short_:
                    case BasicType::unsigned_short_:
                    case BasicType::long_:
                    case BasicType::unsigned_long_:
                    case BasicType::long_long_:
                    case BasicType::unsigned_long_long_:
                    case BasicType::inative_:
                    case BasicType::unative_:
                        break;
                    case BasicType::none_:
                    case BasicType::signed_:
                        type = BasicType::int_;
                        break;
                    default:
                        flagerror = true;
                }
                break;
            case Keyword::native_:
                if (type == BasicType::unsigned_)
                    type = BasicType::unative_;
                else if (type == BasicType::int_ || type == BasicType::none_)
                    type = BasicType::inative_;
                else
                    flagerror = true;
                break;
            case Keyword::char_:
                switch (type)
                {
                    case BasicType::none_:
                        type = BasicType::char_;
                        break;
                    case BasicType::int_:
                    case BasicType::signed_:
                        type = BasicType::signed_char_;
                        break;
                    case BasicType::unsigned_:
                        type = BasicType::unsigned_char_;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::char8_t_:
                if (type != BasicType::none_)
                    flagerror = true;
                type = BasicType::char8_t_;
                break;
            case Keyword::char16_t_:
                if (type != BasicType::none_)
                    flagerror = true;
                type = BasicType::char16_t_;
                break;
            case Keyword::char32_t_:
                if (type != BasicType::none_)
                    flagerror = true;
                type = BasicType::char32_t_;
                break;
            case Keyword::short_:
                switch (type)
                {
                    case BasicType::none_:
                    case BasicType::int_:
                    case BasicType::signed_:
                        type = BasicType::short_;
                        break;
                    case BasicType::unsigned_:
                        type = BasicType::unsigned_short_;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::long_:
                switch (type)
                {
                    case BasicType::double_:
                        if (iscomplex)
                            type = BasicType::long_double_complex_;
                        else if (imaginary)
                            type = BasicType::long_double_imaginary_;
                        else
                            type = BasicType::long_double_;
                        break;
                    case BasicType::none_:
                    case BasicType::int_:
                    case BasicType::signed_:
                        type = BasicType::long_;
                        break;
                    case BasicType::unsigned_:
                        type = BasicType::unsigned_long_;
                        break;
                    case BasicType::long_:
                        type = BasicType::long_long_;
                        break;
                    case BasicType::unsigned_long_:
                        type = BasicType::unsigned_long_long_;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::signed_:
                if (foundsigned || foundunsigned)
                    flagerror = true;
                if (type == BasicType::none_)
                    type = BasicType::signed_;
                else if (type == BasicType::char_)
                    type = BasicType::signed_char_;
                foundsigned++;
                break;
            case Keyword::unsigned_:
                if (foundsigned || foundunsigned)
                    flagerror = true;
                foundunsigned++;
                switch (type)
                {
                    case BasicType::char_:
                        type = BasicType::unsigned_char_;
                        break;
                    case BasicType::short_:
                        type = BasicType::unsigned_short_;
                        break;
                    case BasicType::long_:
                        type = BasicType::unsigned_long_;
                        break;
                    case BasicType::long_long_:
                        type = BasicType::unsigned_long_long_;
                        break;
                    case BasicType::none_:
                    case BasicType::int_:
                        type = BasicType::unsigned_;
                        break;
                    case BasicType::inative_:
                        type = BasicType::unative_;
                        break;
                    case BasicType::bitint_:
                        RequiresDialect::Keyword(Dialect::c23, "_Bitint");
                        type = BasicType::unsigned_bitint_;
                        getsym();
                        needkw(Keyword::openpa_);
                        {
                            Type* tp = nullptr;
                            EXPRESSION* exp = nullptr;
                            optimized_expression(funcsp, nullptr, &tp, &exp, false);
                            if (!tp || !tp->IsInt() || !isintconst(exp))
                                error(ERR_NEED_INTEGER_EXPRESSION);
                            bitintbits = exp->v.i;
                        }
                        if (!MATCHKW(Keyword::closepa_))
                            needkw(Keyword::closepa_);
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::int64_:
                switch (type)
                {
                    case BasicType::unsigned_:
                        int64 = true;
                        type = BasicType::unsigned_long_long_;
                        break;
                    case BasicType::none_:
                    case BasicType::signed_:
                        int64 = true;
                        type = BasicType::long_long_;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::bitint_:
                RequiresDialect::Keyword(Dialect::c23, "_Bitint");
                switch (type)
                {
                    case BasicType::unsigned_:
                        int64 = true;
                        type = BasicType::unsigned_bitint_;
                        break;
                    case BasicType::none_:
                    case BasicType::signed_:
                        int64 = true;
                        type = BasicType::bitint_;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                getsym();
                needkw(Keyword::openpa_);
                {
                    Type* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    optimized_expression(funcsp, nullptr, &tp, &exp, false);
                    if (!tp || !tp->IsInt() || !isintconst(exp))
                        error(ERR_NEED_INTEGER_EXPRESSION);
                    bitintbits = exp->v.i;
                }
                if (!MATCHKW(Keyword::closepa_))
                    needkw(Keyword::closepa_);
                break;
            case Keyword::wchar_t_:
                if (type == BasicType::none_)
                    type = BasicType::wchar_t_;
                else
                    flagerror = true;
                break;
            case Keyword::auto_:
                if (type == BasicType::none_)
                    type = BasicType::auto_;
                else
                    flagerror = true;
                break;
            case Keyword::bool_:
                if (type == BasicType::none_)
                    if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_USESIZE))
                    {
                        if (storage_class == StorageClass::auto_)
                            if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_LOCALBITS))
                            {
                                type = BasicType::bool_;
                            }
                            else
                            {
                                type = BasicType::bit_;
                            }
                        else if (storage_class == StorageClass::global_)
                        {
                            if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_GLOBALBITS))
                            {
                                type = BasicType::bool_;
                            }
                            else
                            {
                                type = BasicType::bit_;
                            }
                        }
                        else
                            type = BasicType::bool_;
                    }
                    else
                        type = BasicType::bool_;
                else
                    flagerror = true;
                break;
            case Keyword::float_:
                if (type == BasicType::none_)
                    if (iscomplex)
                        type = BasicType::float__complex_;
                    else if (imaginary)
                        type = BasicType::float__imaginary_;
                    else
                        type = BasicType::float_;
                else
                    flagerror = true;
                break;
            case Keyword::double_:
                if (type == BasicType::none_)
                    if (iscomplex)
                        type = BasicType::double__complex_;
                    else if (imaginary)
                        type = BasicType::double__imaginary_;
                    else
                        type = BasicType::double_;
                else if (type == BasicType::long_)
                    if (iscomplex)
                        type = BasicType::long_double_complex_;
                    else if (imaginary)
                        type = BasicType::long_double_imaginary_;
                    else
                        type = BasicType::long_double_;
                else
                    flagerror = true;
                break;
            case Keyword::Complex_:
                switch (type)
                {
                    case BasicType::float_:
                        type = BasicType::float__complex_;
                        break;
                    case BasicType::double_:
                        type = BasicType::double__complex_;
                        break;
                    case BasicType::long_double_:
                        type = BasicType::long_double_complex_;
                        break;
                    case BasicType::none_:
                        if (iscomplex || imaginary)
                            flagerror = true;
                        iscomplex = true;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::Imaginary_:
                switch (type)
                {
                    case BasicType::float_:
                        type = BasicType::float__imaginary_;
                        break;
                    case BasicType::double_:
                        type = BasicType::double__imaginary_;
                        break;
                    case BasicType::long_double_:
                        type = BasicType::long_double_imaginary_;
                        break;
                    case BasicType::none_:
                        if (imaginary || iscomplex)
                            flagerror = true;
                        imaginary = true;
                        break;
                    default:
                        flagerror = true;
                        break;
                }
                break;
            case Keyword::object_:
                if (type != BasicType::none_)
                    flagerror = true;
                else
                    type = BasicType::object_;
                break;
            case Keyword::string_:
                if (type != BasicType::none_)
                    flagerror = true;
                else
                    type = BasicType::string_;
                break;
            case Keyword::struct_:
            case Keyword::class_:
            case Keyword::union_:
                inTemplateType = false;
                if (foundsigned || foundunsigned || type != BasicType::none_)
                    flagerror = true;
                declaringInitialType = 0;
                declstruct(funcsp, &tn, inTemplate, asfriend && *asfriend, storage_class, *linkage2_in, access, defd,
                                 constexpression);
                declaringInitialType = oldDeclaringInitialType;
                goto exit;
            case Keyword::enum_:
                if (foundsigned || foundunsigned || type != BasicType::none_)
                    flagerror = true;
                declaringInitialType = 0;
                declenum(funcsp, &tn, storage_class, access, false, defd);
                declaringInitialType = oldDeclaringInitialType;
                goto exit;
            case Keyword::void_:
                if (type != BasicType::none_)
                    flagerror = true;
                type = BasicType::void_;
                break;
            case Keyword::decltype_:
                //                getDeclType(funcsp, &tn);
                type = BasicType::void_; /* won't really be used */
                foundtypeof = true;
                if (foundsomething)
                    flagerror = true;
                goto founddecltype;
            case Keyword::typeof_:
            case Keyword::typeof_unqual_: {
                auto kw = KW();
                type = BasicType::void_; /* won't really be used */
                foundtypeof = true;
                if (foundsomething)
                    flagerror = true;
                getsym();
                if (MATCHKW(Keyword::openpa_))
                {
                    EXPRESSION* exp;
                    getsym();
                    expression_no_check(nullptr, nullptr, &tn, &exp, 0);
                    if (tn)
                    {
                        optimize_for_constants(&exp);
                    }
                    if (!tn)
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    if (!MATCHKW(Keyword::closepa_))
                        needkw(Keyword::closepa_);
                }
                else if (ISID() || MATCHKW(Keyword::classsel_))
                {
                    SYMBOL* sp;
                    nestedSearch(&sp, nullptr, nullptr, nullptr, nullptr, false, storage_class, true, true);
                    if (sp)
                        tn = sp->tp;
                }
                else
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                    errskim(skim_semi_declare);
                }
                if (kw == Keyword::typeof_unqual_)
                    tn = tn->BaseType();
            }
            break;
            case Keyword::atomic_:
                RequiresDialect::Keyword(Dialect::c11, "_Atomic");
                getsym();
                if (needkw(Keyword::openpa_))
                {
                    tn = nullptr;
                    tn = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
                    if (tn)
                    {
                        quals = Type::MakeType(BasicType::atomic_, quals);
                        auto tz = tn;
                        while (tz->type == BasicType::typedef_)
                            tz = tz->btp;
                        if (tz->BaseType() != tz)
                            error(ERR_ATOMIC_TYPE_SPECIFIER_NO_QUALS);
                    }
                    else
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    if (!MATCHKW(Keyword::closepa_))
                    {
                        needkw(Keyword::closepa_);
                        errskim(skim_closepa);
                        skip(Keyword::closepa_);
                    }
                }
                else
                {
                    errskim(skim_closepa);
                    skip(Keyword::closepa_);
                }
                break;
            case Keyword::underlying_type_: {
                getsym();
                underlying_type(funcsp, &tn);
                --*currentStream;
                break;
            }
            default:
                break;
        }
        foundsomething = true;
        getsym();
        getQualifiers(&quals, &linkage, &linkage2, &linkage3, asfriend);
        if (linkage != Linkage::none_)
        {
            *linkage_in = linkage;
        }
        if (linkage2 != Linkage::none_)
            *linkage2_in = linkage2;
        if (linkage3 != Linkage::none_)
            *linkage3_in = linkage3;
    }
    if (type == BasicType::signed_)  // BasicType::signed_ is just an internal placeHolder
        type = BasicType::int_;
founddecltype:
    if (!foundsomething)
    {
        bool typeName = false;
        type = BasicType::int_;
        if (MATCHKW(Keyword::typename_))
        {
            typeName = true;
            //   getsym();
        }
        if (iscomplex || imaginary)
            error(ERR_MISSING_TYPE_SPECIFIER);
        else if (ISID() || MATCHKW(Keyword::classsel_) || MATCHKW(Keyword::complx_) ||
                 MATCHKW(Keyword::decltype_) || MATCHKW(Keyword::typename_))
        {
            std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            SYMBOL* sp = nullptr;
            bool destructor = false;
            LexemeStreamPosition placeHolder(currentStream);
            bool inTemplate = false;  // hides function parameter
            nestedSearch(&sp, &strSym, &nsv, &destructor, &inTemplate, false, storage_class, false, true);
            if (sp && (istype(sp) || (sp->sb && ((sp->sb->storage_class == StorageClass::type_ && inTemplate) ||
                                                 (sp->sb->storage_class == StorageClass::typedef_ && sp->sb->templateLevel)))))
            {
                if (sp->sb && sp->sb->attribs.uninheritable.deprecationText && !sp->tp->IsStructured())
                    deprecateMessage(sp);
                getsym();
                if (sp->sb && sp->sb->storage_class == StorageClass::typedef_ && sp->sb->templateLevel)
                {
                    if (MATCHKW(Keyword::lt_))
                    {
                        // throwaway
                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                        SYMBOL* sp1;
                        declaringInitialType = 0;
                        GetTemplateArguments(funcsp, sp, &lst);
                        declaringInitialType = oldDeclaringInitialType;
                        if (!processingTrailingReturnOrUsing)
                        {
                            if (IsDefiningTemplate() && !processingTemplateHeader && !declaringInitialType &&
                                (!processingTemplateArgs || parsingSpecializationDeclaration))
                            {
                                sp1 = GetTypeAliasSpecialization(sp, lst);
                                if (sp1)
                                {
                                    auto tp1 = sp1->tp;
                                    while (tp1->btp)
                                        tp1 = tp1->btp;
                                    if (tp1->type != BasicType::any_)
                                    {
                                        sp = sp1;
                                    }
                                }
                                if (sp == sp1)
                                    tn = sp->tp;
                                else
                                    tn = Type::MakeType(sp, lst);
                            }
                            else
                            {
                                tn = Type::MakeType(sp, lst);
                            }
                        }
                        else
                        {
                            auto oldsp = sp;
                            sp = CopySymbol(sp);
                            sp->sb->mainsym = oldsp;
                            sp->tp = sp->tp->CopyType();
                            sp->tp->sp = sp;
                            auto templateParams = lst;
                            templateParams->push_front(TEMPLATEPARAMPAIR());
                            templateParams->front().second = Allocate<TEMPLATEPARAM>();
                            templateParams->front().second->type = TplType::new_;
                            tn = Type::MakeType(sp, templateParams);
                        }
                        foundsomething = true;
                    }
                    else if (sp->tp->IsDeferred())
                    {
                        tn = sp->tp;
                    }
                    else
                    {
                        SpecializationError(sp);
                        foundsomething = noNeedToSpecialize;
                        tn = sp->tp;
                    }
                }
                else
                {
                    SYMBOL* ssp = enclosingDeclarations.GetFirst();
                    Type* tpx = sp->tp->BaseType();
                    foundsomething = true;
                    if (tpx->type == BasicType::templateparam_)
                    {
                        tn = nullptr;
                        if (templateArg)
                            *templateArg = true;
                        if (!tpx->templateParam->second->packed)
                        {
                            if (tpx->templateParam->second->type == TplType::typename_)
                            {
                                tn = tpx->templateParam->second->byClass.val;
                                if (tp && tn)
                                {
                                    // should only be const vol specifiers
                                    Type* tpy = tp;
                                    while (tpy->btp)
                                        tpy = tpy->btp;
                                    tpy->btp = tpx;
                                    // used to pass a pointer to tpx->templateParam
                                    tn = SynthesizeType(tp, nullptr, false);
                                    tp = nullptr;
                                }
                                if (inTemplate)
                                {
                                    if (MATCHKW(Keyword::lt_))
                                    {
                                        // throwaway
                                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                        GetTemplateArguments(funcsp, nullptr, &lst);
                                    }
                                    errorsym(ERR_NOT_A_TEMPLATE, sp);
                                }
                            }
                            else if (tpx->templateParam->second->type == TplType::template_)
                            {
                                if (MATCHKW(Keyword::lt_))
                                {

                                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                    SYMBOL* sp1 = tpx->templateParam->second->byTemplate.val;
                                    GetTemplateArguments(funcsp, sp1, &lst);
                                    if (sp1)
                                    {
                                        if (IsDefiningTemplate())
                                        {
                                            sp1 = GetClassTemplate(sp1, lst, !templateErr);
                                            tn = nullptr;
                                            if (sp1)
                                            {
                                                if (sp1->tp->type == BasicType::typedef_)
                                                {
                                                    tn = SynthesizeType(sp1->tp, nullptr, false);
                                                }
                                                else
                                                {
                                                    tn = sp1->tp;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            tn = Type::MakeType(sp1, lst);
                                        }
                                    }
                                    else if (templateDefinitionLevel)
                                    {
                                        sp1 = CopySymbol(sp);
                                        sp1->tp = sp->tp->CopyType();
                                        sp1->tp->sp = sp1;
                                        // used to patch in the remainder of the list in the 'next' field
                                        sp1->tp->templateParam = Allocate<TEMPLATEPARAMPAIR>();
                                        // sp1->tp->templateParam->next = sp->tp->templateParam->next;
                                        sp1->tp->templateParam->second = Allocate<TEMPLATEPARAM>();
                                        *sp1->tp->templateParam->second = *sp->tp->templateParam->second;
                                        sp1->tp->templateParam->second->byTemplate.orig = sp->tp->templateParam;
                                        auto itl = lst->begin();
                                        auto itle = lst->end();
                                        auto ito = sp1->tp->templateParam->second->byTemplate.args->begin();
                                        auto itoe = sp1->tp->templateParam->second->byTemplate.args->end();
                                        auto tnew = templateParamPairListFactory.CreateList();
                                        while (itl != itle && ito != itoe)
                                        {
                                            if (ito->second->type == TplType::new_)
                                            {
                                                tnew->push_back(TEMPLATEPARAMPAIR{nullptr, ito->second});
                                            }
                                            else if (!itl->first)
                                            {
                                                tnew->push_back(TEMPLATEPARAMPAIR{nullptr, itl->second});
                                            }
                                            else
                                            {
                                                SYMBOL* sp = classsearch(itl->first->name, false, false, false);
                                                if (sp && sp->tp->type == BasicType::templateparam_)
                                                {
                                                    tnew->push_back(TEMPLATEPARAMPAIR{sp->tp->templateParam->first,
                                                                                      sp->tp->templateParam->second});
                                                }
                                                else
                                                {
                                                    tnew->push_back(TEMPLATEPARAMPAIR{itl->first, itl->second});
                                                }
                                            }
                                            ++itl;
                                            ++ito;
                                        }
                                        sp1->tp->templateParam->second->byTemplate.args = tnew;
                                        sp = sp1;
                                    }
                                }
                                else
                                {
                                    SYMBOL* sp1 = tpx->templateParam->second->byTemplate.val;
                                    if (sp1 && allTemplateArgsSpecified(sp1, tpx->templateParam->second->byTemplate.args))
                                    {
                                        tn = sp1->tp;
                                    }
                                    else
                                    {

                                        tn = sp->tp;
                                    }
                                }
                            }
                            else
                            {
                                diag("TypeGenerator::UnadornedType: expected typename template param");
                            }
                        }
                        else if (isExpandingParams && tpx->type == BasicType::templateparam_ &&
                                 tpx->templateParam->second->byPack.pack && !tpx->templateParam->second->resolved)
                        {
                            auto tn2 = LookupPackedInstance(*tpx->templateParam);
                            if (tn2)
                                tn = tn2->byClass.val;
                        }
                        if (!tn)
                            tn = sp->tp;
                    }
                    else if (tpx->type == BasicType::templatedecltype_)
                    {
                        if (templateArg)
                            *templateArg = true;
                        if (!templateDefinitionLevel)
                            TemplateLookupTypeFromDeclType(tpx);
                    }
                    else if (tpx->type == BasicType::templateselector_)
                    {
                        if (templateArg)
                            *templateArg = true;
                        //                        if (!templateDefinitionLevel)
                        //                        {
                        //                            tn = SynthesizeType(sp->tp, nullptr, false);
                        //                        }
                        //                        else
                        {
                            tn = sp->tp;
                        }
                        if ((*tpx->sp->sb->templateSelector).size() > 2 && (*tpx->sp->sb->templateSelector)[1].sp &&
                            !(*tpx->sp->sb->templateSelector)[1].isDeclType &&
                            !strcmp((*tpx->sp->sb->templateSelector)[1].sp->name, (*tpx->sp->sb->templateSelector)[2].name))
                        {
                            if (destructor)
                            {
                                *consdest = CT_DEST;
                            }
                            else
                            {
                                *consdest = CT_CONS;
                            }
                            if (strSym_out)
                                *strSym_out = (*tpx->sp->sb->templateSelector)[1].sp;
                            tn = (*tpx->sp->sb->templateSelector)[1].sp->tp;
                            *notype = true;
                            goto exit;
                        }
                        else
                        {
                            // discard template params, they've already been gathered..
                            TEMPLATESELECTOR* l = &(*tpx->sp->sb->templateSelector).back();
                            if (l->isTemplate)
                            {
                                if (MATCHKW(Keyword::lt_))
                                {
                                    std::list<TEMPLATEPARAMPAIR>* current = nullptr;
                                    GetTemplateArguments(nullptr, nullptr, &current);
                                }
                            }
                        }
                    }
                    else
                    {
                        getQualifiers(&quals, &linkage, &linkage2, &linkage3, nullptr);
                        if (linkage != Linkage::none_)
                        {
                            *linkage_in = linkage;
                        }
                        if (linkage2 != Linkage::none_)
                            *linkage2_in = linkage2;
                        if (sp->sb->templateLevel)
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            if (templateDefinitionLevel)
                                tn = sp->tp;
                            else
                                tn = nullptr;
                            if (MATCHKW(Keyword::lt_))
                            {
                                if (sp->sb->parentTemplate)
                                    sp = sp->sb->parentTemplate;
                                GetTemplateArguments(funcsp, sp, &lst);
                                if (IsDefiningTemplate())
                                {
                                    auto sp1 = GetClassTemplate(sp, lst, !templateErr);
                                    if (sp1)
                                    {
                                        sp = sp1;
                                        if (sp && (!templateDefinitionLevel || processingTemplateBody))
                                            sp->tp = sp->tp->InitializeDeferred();
                                    }
                                    else
                                    {
                                        tn = Type::MakeType(sp, lst);
                                        sp = nullptr;
                                    }
                                }
                                else
                                {
                                    tn = Type::MakeType(sp, lst);
                                    sp = nullptr;
                                }
                            }
                            else
                            {
                                if (!sp->sb->mainsym || (sp->sb->mainsym != strSym && sp->sb->mainsym != ssp))
                                {
                                    if (instantiatingMemberFuncClass &&
                                        instantiatingMemberFuncClass->sb->parentClass == sp->sb->parentClass)
                                        sp = instantiatingMemberFuncClass;
                                    else if (deduceTemplate)
                                        *deduceTemplate = MustSpecialize(sp->name);
                                    else if (!allTemplateArgsSpecified(sp, sp->templateParams))
                                        SpecializationError(sp);
                                }
                            }
                            if (sp)
                                tn = sp->tp;
                        }
                        else if (sp->tp->IsStructured() && MATCHKW(Keyword::lt_) && sp->tp->BaseType()->sp->sb->templateLevel)
                        {
                            sp = sp->tp->BaseType()->sp;
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            if (templateDefinitionLevel)
                                tn = sp->tp;
                            else
                                tn = nullptr;
                            if (sp->sb->parentTemplate)
                                sp = sp->sb->parentTemplate;
                            GetTemplateArguments(funcsp, sp, &lst);
                            if (IsDefiningTemplate())
                            {
                                auto sp1 = GetClassTemplate(sp, lst, !templateErr);
                                if (sp1)
                                {
                                    sp = sp1;
                                    if (sp && (!templateDefinitionLevel || processingTemplateBody))
                                        sp->tp = sp->tp->InitializeDeferred();
                                }
                                else
                                {
                                    tn = Type::MakeType(sp, lst);
                                    sp = nullptr;
                                }
                            }
                            else
                            {
                                tn = Type::MakeType(sp, lst);
                                sp = nullptr;
                            }
                        }
                        else
                        {
                            if (inTemplate)
                            {
                                if (MATCHKW(Keyword::lt_))
                                {
                                    // throwaway
                                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                    GetTemplateArguments(funcsp, nullptr, &lst);
                                }
                                errorsym(ERR_NOT_A_TEMPLATE, sp);
                            }
                            if (sp->tp->type == BasicType::typedef_)
                            {
                                tn = sp->tp;
                                if (!Optimizer::cparams.prm_cplusplus)
                                    while (tn != tn->BaseType() && tn->type != BasicType::va_list_)
                                        tn = tn->btp;
                            }
                            else
                            {
                                tn = sp->tp;
                            }
                            if (!templateDefinitionLevel && tn->IsStructured() && tn->BaseType()->sp->sb->templateLevel &&
                                !tn->BaseType()->sp->sb->instantiated)
                            {
                                if (IsDefiningTemplate())
                                {
                                    sp = GetClassTemplate(tn->BaseType()->sp, tn->BaseType()->sp->templateParams, false);
                                    if (sp)
                                        tn = sp->tp;
                                }
                                else
                                {
                                    tn = Type::MakeType(tn->BaseType()->sp, tn->BaseType()->sp->templateParams);
                                }
                            }
                            //                        if (sp->sb->parentClass && !isAccessible(sp->sb->parentClass, ssp ? ssp :
                            //                        sp->sb->parentClass, sp, funcsp, ssp == sp->sb->parentClass ?
                            //                        AccessLevel::protected_ : AccessLevel::public_, false))
                            //                           errorsym(ERR_CANNOT_ACCESS, sp);
                        }
                        // DAL
                        if (Optimizer::cparams.prm_cplusplus && sp && MATCHKW(Keyword::openpa_) &&
                            ((strSym && ((strSym->sb->mainsym && strSym->sb->mainsym == sp->sb->mainsym) ||
                                         strSym == sp->sb->mainsym || SameTemplate(strSym->tp, sp->tp))) ||
                             (!strSym && (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_) &&
                              ssp && (ssp == sp || ssp == sp->sb->mainsym))))
                        {
                            if (destructor)
                            {
                                *consdest = CT_DEST;
                            }
                            else
                            {
                                *consdest = CT_CONS;
                            }
                            if (ssp && strSym && ssp == strSym->sb->mainsym)
                                strSym = nullptr;
                            if (strSym_out)
                                *strSym_out = strSym;
                            *notype = true;
                            goto exit;
                        }
                        else if (destructor)
                        {
                            error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                        }
                    }
                }
            }
            else if (strSym && strSym->tp->BaseType()->type == BasicType::templateselector_)
            {
                if (!tn || tn->type == BasicType::any_ || tn->BaseType()->type == BasicType::templateparam_)
                {
                    SYMBOL* sym = (*strSym->tp->BaseType()->sp->sb->templateSelector)[1].sp;
                    if (!isTypedef && (!IsDefiningTemplate()) && sym->tp->IsStructured() &&
                        (sym->sb && sym->sb->instantiated && !declaringTemplate(sym) &&
                         (!sym->sb->templateLevel ||
                          allTemplateArgsSpecified(sym, (*strSym->tp->sp->sb->templateSelector)[1].templateParams))))
                    {

                        errorNotMember(sym, nsv ? nsv->front() : nullptr, (*strSym->tp->sp->sb->templateSelector)[2].name);
                    }
                    tn = strSym->tp;
                }
                foundsomething = true;
                getsym();
            }
            else if (strSym && strSym->tp->BaseType()->type == BasicType::templatedecltype_)
            {
                tn = strSym->tp;
                foundsomething = true;
                getsym();
            }
            else if (strSym && strSym->sb->templateLevel && !templateDefinitionLevel)
            {
                tn = strSym->tp;
                DeclarationScope scope(tn->sp);
                sp = classsearch(currentLex->value.s.a, false, false, true);
                if (sp)
                {
                    tn = sp->tp;
                    foundsomething = true;
                }
                getsym();
            }
            else if (MATCHKW(Keyword::decltype_))
            {
                getDeclType(funcsp, &tn);
                if (tn)
                    foundsomething = true;
            }
            else if (isTypedef)
            {
                getsym();
                if (sp && sp->tp->type == BasicType::templateselector_)
                {
                    tn = sp->tp;
                    // discard template params, they've already been gathered..
                    TEMPLATESELECTOR* l = &(*tn->sp->sb->templateSelector)[1];
                    if (l->isTemplate)
                    {
                        if (MATCHKW(Keyword::lt_))
                        {
                            std::list<TEMPLATEPARAMPAIR>* current = nullptr;
                            GetTemplateArguments(nullptr, nullptr, &current);
                        }
                    }
                }
                else
                {
                    if (!templateDefinitionLevel && !typeName)
                    {
                        error(ERR_TYPE_NAME_EXPECTED);
                    }
                    tn = Type::MakeType(BasicType::any_);
                }
                foundsomething = true;
            }
            else if (Optimizer::cparams.prm_cplusplus)
            {
                if (typeName || ISID())
                {
                    tn = Type::MakeType(BasicType::any_);
                    if (currentLex->type == LexType::l_id_)
                    {
                        SYMBOL* sp = makeID(StorageClass::global_, tn, nullptr, litlate(currentLex->value.s.a));
                        tn->sp = sp;
                    }
                    getsym();
                    foundsomething = true;
                }
                else
                {
                    if (destructor)
                    {
                        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                    }
                    placeHolder.Backup();
                }
            }
        }
        if (!foundsomething && (Optimizer::cparams.c_dialect >= Dialect::c99 || Optimizer::cparams.prm_cplusplus))
        {
            if (notype)
                *notype = true;
            else
            {
                error(ERR_MISSING_TYPE_SPECIFIER);
            }
        }
    }
exit:
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && tn)
    {
        // select an unboxed variable type for use in compiler
        // will be converted back to boxed as needed
        Type* tnn = find_unboxed_type(tn);
        if (tnn)
        {
            tn = tnn;
            type = tn->type;
        }
    }
    if (Optimizer::cparams.c_dialect < Dialect::c99)
        switch (type)
        {
            case BasicType::bool_:
                if (!Optimizer::cparams.prm_cplusplus)
                    RequiresDialect::Keyword(Dialect::c99, "_Bool");
                break;
            case BasicType::long_long_:
            case BasicType::unsigned_long_long_:
                if (!int64 && !Optimizer::cparams.prm_cplusplus)
                    RequiresDialect::Keyword(Dialect::c99, "long long");
                break;
            case BasicType::float__complex_:
            case BasicType::double__complex_:
            case BasicType::long_double_complex_:
                RequiresDialect::Keyword(Dialect::c99, "_Complex");
                break;
            case BasicType::long_double_imaginary_:
            case BasicType::float__imaginary_:
            case BasicType::double__imaginary_:
                RequiresDialect::Keyword(Dialect::c99, "_Imaginary");
                break;
            default:
                break;
        }
    if (flagerror)
        error(ERR_TOO_MANY_TYPE_SPECIFIERS);
    if (!tn)
    {
        tn = Type::MakeType(type);
        if (type == BasicType::bitint_ || type == BasicType::unsigned_bitint_)
        {
            tn->bitintbits = bitintbits;
            tn->size = (bitintbits + Optimizer::chosenAssembler->arch->bitintunderlying - 1);
            tn->size /= Optimizer::chosenAssembler->arch->bitintunderlying;
            tn->size *= Optimizer::chosenAssembler->arch->bitintunderlying;
            tn->size /= CHAR_BIT;
        }
    }
    if (quals)
    {
        tl = &quals;
        while (*tl)
        {
            (*tl)->size = tn->size;
            tl = &(*tl)->btp;
        }
        *tl = tn;
        tn = quals;
    }
    if (tp)
    {
        tl = &tp;
        while (*tl)
        {
            tl = &(*tl)->btp;
        }
        *tl = tn;
    }
    else
        tp = tn;
    tp->UpdateRootTypes();
    if (tp->IsAtomic())
    {

        int sz = tp->BaseType()->size;
        if (needsAtomicLockFromType(tp))
        {
            int n = getAlign(StorageClass::global_, &stdchar32tptr);
            if (n)
            {
                n = n - tp->size % n;
                if (n != getSize(BasicType::int_))
                {
                    sz += n;
                }
            }
            sz += ATOMIC_FLAG_SPACE;
        }
        tp->size = sz;
    }
    sizeQualifiers(tp);
    return tp;
}
Type* TypeGenerator::PointerQualifiers(Type* tp, bool allowstatic)
{
    while (KWTYPE(TT_TYPEQUAL) || (allowstatic && MATCHKW(Keyword::static_)))
    {
        Type* tpn;
        if (MATCHKW(Keyword::intrinsic_))
        {
            getsym();
            continue;
        }
        tpn = Allocate<Type>();
        if (tp)
            tpn->size = tp->size;
        switch (KW())
        {
            case Keyword::static_:
                tpn->type = BasicType::static_;
                break;
            case Keyword::const_:
                tpn->type = BasicType::const_;
                break;
            case Keyword::atomic_:
                RequiresDialect::Keyword(Dialect::c11, "_Atomic");
                getsym();
                if (MATCHKW(Keyword::openpa_))
                {
                    // being used as  a type specifier not a qualifier
                    --*currentStream;
                    return tp;
                }
                --*currentStream;
                tpn->type = BasicType::atomic_;
                break;
            case Keyword::volatile_:
                tpn->type = BasicType::volatile_;
                break;
            case Keyword::restrict_:
                tpn->type = BasicType::restrict_;
                break;
            case Keyword::far_:
                tpn->type = BasicType::far_;
                break;
            case Keyword::near_:
                tpn->type = BasicType::near_;
                break;
            case Keyword::va_list_:
                tpn->type = BasicType::va_list_;
                break;
            default:
                break;
        }
        tpn->btp = tp;
        if (tp)
            tpn->rootType = tp->rootType;
        tp = tpn;
        getsym();
    }
    return tp;
}
int count4;
Type* TypeGenerator::FunctionParams(SYMBOL* funcsp, SYMBOL** spin, Type* tp, bool inTemplate,
                                    StorageClass storage_class, bool funcptr)
{
    (void)storage_class;
    SYMBOL* sp = *spin;
    SYMBOL* spi;
    Type* tp1;
    bool isvoid = false;
    bool pastfirst = false;
    bool voiderror = false;
    bool hasellipse = false;
    LexemeStreamPosition placeHolder(currentStream);
    NAMESPACEVALUEDATA internalNS = {};
    SymbolTable<SYMBOL>* symbolTable = symbols->CreateSymbolTable();
    getsym();
    if (tp == nullptr)
        tp = &stdint;
    tp1 = Type::MakeType(BasicType::func_, tp);
    tp1->size = getSize(BasicType::pointer_);
    tp1->sp = sp;
    sp->tp = tp = tp1;
    localNameSpace->push_front(&internalNS);
    localNameSpace->front()->syms = tp1->syms = symbolTable;
    attributes oldAttribs = basisAttribs;

    basisAttribs = {0};
    ParseAttributeSpecifiers(funcsp, true);
    bool structured = false;
    if (TypeGenerator::StartOfType(&structured, true) &&
            (!Optimizer::cparams.prm_cplusplus || StatementGenerator::ResolvesToDeclaration(structured)) ||
        MATCHKW(Keyword::constexpr_))
    {
        sp->sb->hasproto = true;
        while (TypeGenerator::StartOfType(nullptr, true) || MATCHKW(Keyword::ellipse_) ||
               MATCHKW(Keyword::constexpr_))
        {
            if (MATCHKW(Keyword::constexpr_))
            {
                getsym();
                error(ERR_CONSTEXPR_NO_PARAM);
            }
            bool templType = inTemplateType;
            if (isFullySpecialized)
                ++templateInstantiationLevel;
            inTemplateType = !!templateDefinitionLevel;
            EnterPackedSequence();
            if (MATCHKW(Keyword::ellipse_))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, Linkage::none_);
                spi->tp = Type::MakeType(BasicType::ellipse_);
                tp->syms->Add(spi);
                getsym();
                hasellipse = true;
                ClearPackedSequence();
            }
            else
            {
                StorageClass storage_class = StorageClass::parameter_;
                bool blocked;
                bool constexpression;
                Optimizer::ADDRESS address;
                Type* tpb;
                Linkage linkage = Linkage::none_;
                Linkage linkage2 = Linkage::none_;
                Linkage linkage3 = Linkage::none_;
                bool defd = false;
                bool notype = false;
                bool clonedParams = false;
                Type* tp2;
                spi = nullptr;
                tp1 = nullptr;
                LexemeStreamPosition start(currentStream);
                noTypeNameError++;
                getStorageAndType(funcsp, nullptr, false, true, nullptr, &storage_class, &storage_class, &address,
                                        &blocked, nullptr, &constexpression, &constexpression, &tp1, &linkage, &linkage2, &linkage3,
                                        AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr);
                noTypeNameError--;
                if (!tp1->BaseType())
                    error(ERR_TYPE_NAME_EXPECTED);
                else if (tp1->IsAutoType() && !lambdas.size())
                    error(ERR_AUTO_NOT_ALLOWED_IN_PARAMETER);
                else if (Optimizer::cparams.prm_cplusplus && tp->btp->IsStructured() &&
                         (MATCHKW(Keyword::openpa_) || MATCHKW(Keyword::begin_)))
                {
                    LexemeStreamPosition cur(currentStream);
                    getsym();
                    if (!MATCHKW(Keyword::star_) && !MATCHKW(Keyword::and_) &&
                        !TypeGenerator::StartOfType(nullptr, true))
                    {
                        if (*spin)
                        {
                            (*spin)->tp = tp = tp->btp;
                            // constructor initialization
                            // will do initialization later...
                        }
                        localNameSpace->pop_front();
                        placeHolder.Backup();
                        return tp;
                    }
                    cur.Backup();
                }
                tp1 = TypeGenerator::BeforeName(funcsp, tp1, &spi, nullptr, nullptr, false, storage_class, &linkage, &linkage2,
                                                &linkage3, nullptr, false, false, false, false);
                if (!spi)
                {
                    spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
                    spi->sb->anonymous = true;
                    SetLinkerNames(spi, Linkage::none_);
                }
                spi->sb->parent = sp;
                tp1 = AttributeFinish(spi, tp1);
                tp2 = tp1;
                while (tp2->IsPtr() || tp2->IsRef())
                    tp2 = tp2->BaseType()->btp;
                tp2 = tp2->BaseType();
                if (spi->sb->anonymous && MATCHKW(Keyword::ellipse_))
                {
                    if (IsPacking() || (tp2->templateParam && tp2->templateParam->second->packed))
                    {
                        spi->packed = true;
                    }
                    getsym();
                }
                spi->tp = tp1;
                spi->sb->attribs.inheritable.linkage = linkage;
                spi->sb->attribs.inheritable.linkage2 = linkage2;
                if (spi->packed)
                {
                    checkPackedType(spi);
                    if (tp2->type == BasicType::templateparam_ && tp2->templateParam->second->packed)
                    {
                        if (!templateDefinitionLevel ||
                            (!funcptr && tp2->type == BasicType::templateparam_ && tp2->templateParam->first &&
                                !inCurrentTemplate(tp2->templateParam->first->name) && definedInTemplate(tp2->templateParam->first->name)))
                        {
                            if (tp2->templateParam && tp2->templateParam->second->packed)
                            {
                                bool forwarding = false;
                                if (tp1->BaseType()->type == BasicType::rref_)
                                {
                                    auto tp3 = tp1;
                                    tp3 = tp3->BaseType()->btp;
                                    // unadorned rref means we are forwading
                                    forwarding = !tp3->IsConst() && !tp3->IsVolatile() && tp3->BaseType()->type == BasicType::templateparam_;
                                }
                                Type* newtp1 = Allocate<Type>();
                                std::list<TEMPLATEPARAMPAIR>* templateParams = tp2->templateParam->second->byPack.pack;
                                auto tpp = Allocate<TEMPLATEPARAMPAIR>();
                                TEMPLATEPARAM* tpnew = Allocate<TEMPLATEPARAM>();
                                std::list<TEMPLATEPARAMPAIR>* newPack = templateParamPairListFactory.CreateList();
                                bool first = true;
                                *tpnew = *tp2->templateParam->second;
                                *tpp = TEMPLATEPARAMPAIR{ tp2->templateParam->first, tpnew };
                                *newtp1 = *tp2;
                                newtp1->templateParam = tpp;
                                tp1 = newtp1;
                                tp1->templateParam->second->byPack.pack = newPack;
                                tp1->templateParam->second->index = 0;
                                if (templateParams)
                                {
                                    for (auto&& tpp : *templateParams)
                                    {
                                        SYMBOL* clone = CopySymbol(spi);

                                        if (forwarding)
                                        {
                                            clone->tp = GetForwardType(nullptr, tpp.second->byClass.val, nullptr)->CopyType(true);
                                        }
                                        else
                                        {
                                            clone->tp = clone->tp->CopyType(true, [&tpp](Type*& old, Type*& newx) {
                                                if (old->type == BasicType::templateparam_)
                                                {
                                                    old = tpp.second->byClass.val;
                                                    *newx = *old;
                                                }
                                                });
                                        }
                                        CollapseReferences(clone->tp);
                                        SetLinkerNames(clone, Linkage::none_);
                                        sizeQualifiers(clone->tp);
                                        if (!first)
                                        {
                                            clone->name = clone->sb->decoratedName = AnonymousName();
                                            clone->packed = false;
                                        }
                                        else
                                        {
                                            clone->synthesized = true;
                                            clone->tp->templateParam = tp1->templateParam;
                                        }
                                        auto second = Allocate<TEMPLATEPARAM>();
                                        *second = *tpp.second;
                                        second->packsym = clone;
                                        newPack->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
                                        tp->syms->Add(clone);
                                        first = false;
                                        tp1->templateParam->second->index++;
                                        clone->tp->UpdateRootTypes();
                                    }
                                }
                                else
                                {
                                    SYMBOL* clone = CopySymbol(spi);
                                    clone->tp = clone->tp->CopyType();
                                    clone->tp->templateParam = tp1->templateParam;
                                    SetLinkerNames(clone, Linkage::none_);
                                    clone->tp->UpdateRootTypes();
                                    sizeQualifiers(clone->tp);
                                    tp->syms->Add(clone);
                                    clone->tp->UpdateRootTypes();
                                }
                                clonedParams = true;
                            }
                        }
                    }
                    else
                    {
                        auto current = currentStream->Index();
                        PushPackIndex();
                        int n = GetPackCount();
                        bool first = true;
                        processingTemplateArgs++;
                        for (int i = 0; i < n; i++)
                        {
                            SetPackIndex(i);
                            start.Replay([&]() {
                                noTypeNameError++;
                                tp1 = nullptr;
                                getStorageAndType(funcsp, nullptr, false, true, nullptr, &storage_class, &storage_class, &address,
                                    &blocked, nullptr, &constexpression, &constexpression, &tp1, &linkage, &linkage2, &linkage3,
                                    AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr);
                                noTypeNameError--;
                            
                                SYMBOL* spi1 = nullptr;;
                                tp1 = TypeGenerator::BeforeName(funcsp, tp1, &spi1, nullptr, nullptr, false, storage_class, &linkage, &linkage2,
                                    &linkage3, nullptr, false, false, false, false);
                            });

                            SYMBOL* clone = CopySymbol(spi);
                            clone->tp = tp1; 
                            CollapseReferences(clone->tp);
                            SetLinkerNames(clone, Linkage::none_);
                            sizeQualifiers(clone->tp);
                            if (!first)
                            {
                                clone->name = clone->sb->decoratedName = AnonymousName();
                                clone->packed = false;
                            }
                            else
                            {
                                clone->synthesized = true;
//                                clone->tp->templateParam = tp1->templateParam;
                            }
                            tp->syms->Add(clone);
                            first = false;
                            clone->tp->UpdateRootTypes();
                        }
                        current;
                        processingTemplateArgs--;
                        PopPackIndex();
                        clonedParams = true;
                    }
                    ClearPackedSequence();
                }
                if (!clonedParams)
                {
                    if (tp1 && tp1->IsFunction())
                    {
                        tp1 = Type::MakeType(BasicType::pointer_, tp1);
                    }
                    if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::assign_))
                    {
                        if (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_ || structLevel ||
                            (templateDefinitionLevel == 1 && !templateInstantiationLevel))
                        {
                            auto stream = GetTokenStream(false);
                            initTokenStreams.set(spi, stream);
                        }
                        else
                        {
                            Type* tp2 = spi->tp;
                            inDefaultParam++;
                            if (tp2->IsRef())
                                tp2 = tp2->BaseType()->btp;
                            if (tp2->IsStructured())
                            {
                                SYMBOL* sym;
                                anonymousNotAlloc++;
                                sym = AnonymousVar(StorageClass::auto_, tp2)->v.sp;
                                anonymousNotAlloc--;
                                sym->sb->stackblock = !spi->tp->IsRef();
                                initialize(funcsp, sym, StorageClass::auto_, true, false, false,
                                                 0); /* also reserves space */
                                spi->sb->init = sym->sb->init;
                                if (spi->sb->init->front()->exp && spi->sb->init->front()->exp->type == ExpressionNode::thisref_)
                                {
                                    EXPRESSION** expr = &spi->sb->init->front()->exp->left->v.func->thisptr;
                                    if (*expr && (*expr)->type == ExpressionNode::add_ && isconstzero(&stdint, (*expr)->right))
                                        spi->sb->init->front()->exp->v.t.thisptr = (*expr) = (*expr)->left;
                                }
                            }
                            else
                            {
                                initialize(funcsp, spi, StorageClass::auto_, true, false, false,
                                                 0); /* also reserves space */
                            }
                            if (spi->sb->init)
                            {
                                checkDefaultArguments(spi);
                            }
                            inDefaultParam--;
                        }
                        spi->sb->defaultarg = true;
                        if (spi->tp->IsFunctionPtr() && spi->sb->init && IsLValue(spi->sb->init->front()->exp))
                            error(ERR_NO_POINTER_TO_FUNCTION_DEFAULT_ARGUMENT);
                        if (sp->sb->storage_class == StorageClass::typedef_)
                            error(ERR_NO_DEFAULT_ARGUMENT_IN_TYPEDEF);
                    }
                    SetLinkerNames(spi, Linkage::none_);
                    spi->tp = tp1;
                    tp->syms->Add(spi);
                    tpb = tp1->BaseType();
                    if (tpb->array)
                    {
                        if (tpb->vla)
                        {
                            auto tpx = Type::MakeType(BasicType::pointer_, tpb);
                            Type* tpn = tpb;
                            tpx->size = tpb->size = getSize(BasicType::pointer_) + getSize(BasicType::unsigned_) * 2;
                            while (tpn->vla)
                            {
                                tpx->size += getSize(BasicType::unsigned_);
                                tpb->size += getSize(BasicType::unsigned_);
                                tpn->sp = spi;
                                tpn = tpn->btp;
                            }
                        }
                    }
                    if (tpb->type == BasicType::void_)
                        if (pastfirst || !spi->sb->anonymous)
                            voiderror = true;
                        else
                            isvoid = true;
                    else if (isvoid)
                        voiderror = true;
                }
            }
            ClearPackedSequence();
            LeavePackedSequence();
            if (isFullySpecialized)
                --templateInstantiationLevel;
            inTemplateType = templType;
            if (!MATCHKW(Keyword::comma_) && (!Optimizer::cparams.prm_cplusplus || !MATCHKW(Keyword::ellipse_)))
                break;
            if (MATCHKW(Keyword::comma_))
                getsym();
            pastfirst = true;
            basisAttribs = {0};
            ParseAttributeSpecifiers(funcsp, true);
        }
        if (!needkw(Keyword::closepa_))
        {
            errskim(skim_closepa);
            skip(Keyword::closepa_);
        }
        // weed out temporary syms that were added as part of the default; they will be
        // reinstated as stackblock syms later
        for (SymbolTable<SYMBOL>::iterator it = tp->syms->begin(); it != tp->syms->end();)
        {
            SYMBOL* sym = *it;
            auto it1 = it;
            it1++;
            if (sym->sb->storage_class != StorageClass::parameter_)
                tp->syms->remove(it);
            it = it1;
        }
    }
    else if (!Optimizer::cparams.prm_cplusplus && !(Optimizer::architecture == ARCHITECTURE_MSIL) && ISID())
    {
        RequiresDialect::Removed(Dialect::c23, "K&R prototypes");
        SYMBOL* spo = nullptr;
        sp->sb->oldstyle = true;
        if (sp->sb->storage_class != StorageClass::member_ && sp->sb->storage_class != StorageClass::mutable_)
        {
            spo = gsearch(sp->name);
            /* temporary for C */
            if (spo && spo->sb->storage_class == StorageClass::overloads_)
                spo = spo->tp->syms->front();
            if (spo && spo->tp->IsFunction() && spo->sb->hasproto)
            {
                if (!spo->tp->btp->CompatibleType(sp->tp->btp))
                {
                    preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
                }
            }
        }
        while (ISID() || MATCHKW(Keyword::ellipse_))
        {
            if (MATCHKW(Keyword::ellipse_))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, Linkage::none_);
                spi->tp = Type::MakeType(BasicType::ellipse_);
                getsym();
                hasellipse = true;
                tp->syms->Add(spi);
            }
            else
            {
                spi = makeID(StorageClass::parameter_, 0, 0, litlate(currentLex->value.s.a));
                SetLinkerNames(spi, Linkage::none_);
                tp->syms->Add(spi);
            }
            getsym();
            if (!MATCHKW(Keyword::comma_))
                break;
            getsym();
        }
        if (!needkw(Keyword::closepa_))
        {
            errskim(skim_closepa);
            skip(Keyword::closepa_);
        }
        if (TypeGenerator::StartOfType(nullptr, false))
        {
            while (TypeGenerator::StartOfType(nullptr, false))
            {
                Optimizer::ADDRESS address;
                bool blocked;
                bool constexpression;
                Linkage linkage = Linkage::none_;
                Linkage linkage2 = Linkage::none_;
                Linkage linkage3 = Linkage::none_;
                StorageClass storage_class = StorageClass::parameter_;
                bool defd = false;
                bool notype = false;
                tp1 = nullptr;
                getStorageAndType(funcsp, nullptr, false, false, nullptr, &storage_class, &storage_class, &address,
                                        &blocked, nullptr, &constexpression, &constexpression, &tp1, &linkage, &linkage2, &linkage3,
                                        AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr);

                while (1)
                {
                    Type* tpx = tp1;
                    spi = nullptr;
                    tpx = TypeGenerator::BeforeName(funcsp, tpx, &spi, nullptr, nullptr, false, StorageClass::parameter_,
                                                    &linkage, &linkage2, &linkage3, nullptr, false, false, false, false);
                    sizeQualifiers(tpx);
                    if (!spi || spi->sb->anonymous)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        errskim(skim_end);
                        break;
                    }
                    else
                    {
                        SYMBOL* spo;
                        Type* tpb;
                        spi->sb->attribs.inheritable.linkage = linkage;
                        spi->sb->attribs.inheritable.linkage2 = linkage2;
                        SetLinkerNames(spi, Linkage::none_);
                        if (tpx && tpx->IsFunction())
                        {
                            tpx = Type::MakeType(BasicType::pointer_, tpx);
                        }
                        spi->tp = tpx;
                        tpb = tpx->BaseType();
                        if (tpb->array)
                        {
                            if (tpb->vla)
                            {
                                auto tpx = Type::MakeType(BasicType::pointer_, tpb);
                                Type* tpn = tpb;
                                tpb = tpx;
                                tpx->size = tpb->size = getSize(BasicType::pointer_) + getSize(BasicType::unsigned_) * 2;
                                while (tpn->vla)
                                {
                                    tpx->size += getSize(BasicType::unsigned_);
                                    tpb->size += getSize(BasicType::unsigned_);
                                    tpn->sp = spi;
                                    tpn = tpn->btp;
                                }
                            }
                            else
                            {
                                //								tpb->array = false;
                                //								tpb->size = getSize(BasicType::pointer_);
                            }
                        }
                        sizeQualifiers(tpx);
                        spo = search(tp->syms, spi->name);
                        if (!spo)
                            errorsym(ERR_UNDEFINED_IDENTIFIER, spi);
                        else
                        {
                            spo->tp = tpx;
                        }
                        if (MATCHKW(Keyword::comma_))
                        {
                            getsym();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                needkw(Keyword::semicolon_);
            }
        }
        if (tp->syms->size())
        {
            for (auto spi : *tp->syms)
            {
                if (spi->tp == nullptr)
                {
                    if (Optimizer::cparams.c_dialect >= Dialect::c99)
                        errorsym(ERR_MISSING_TYPE_FOR_PARAMETER, spi);
                    spi->tp = Type::MakeType(BasicType::int_);
                }
            }
        }
        else if (spo)
            tp->syms = spo->tp->syms;
        if (!MATCHKW(Keyword::begin_))
            error(ERR_FUNCTION_BODY_EXPECTED);
    }
    else if (MATCHKW(Keyword::ellipse_))
    {
        spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
        spi->sb->anonymous = true;
        SetLinkerNames(spi, Linkage::none_);
        spi->tp = Type::MakeType(BasicType::ellipse_);
        tp->syms->Add(spi);
        getsym();
        if (!MATCHKW(Keyword::closepa_))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(skim_closepa);
        }
        skip(Keyword::closepa_);
    }
    else if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c23 ||
             ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions &&
              !MATCHKW(Keyword::closepa_) && *spin))
    {
        // () is a function
        if (MATCHKW(Keyword::closepa_))
        {
            spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
            spi->sb->anonymous = true;
            spi->sb->attribs.inheritable.structAlign = getAlign(StorageClass::parameter_, &stdpointer);
            SetLinkerNames(spi, Linkage::none_);
            spi->tp = Type::MakeType(BasicType::void_);
            tp->syms->Add(spi);
            getsym();
        }
        // else may have a constructor
        else if (*spin)
        {
            (*spin)->tp = tp = tp->btp;
            // constructor initialization
            --*currentStream;
            // will do initialization later...
        }
        else
        {
            currentErrorLine = 0;
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(skim_closepa);
        }
    }
    else
    {
        SYMBOL* spo;
        if (sp->sb->storage_class != StorageClass::member_ && sp->sb->storage_class != StorageClass::mutable_)
        {
            spo = gsearch(sp->name);
            if (spo && spo->sb->storage_class == StorageClass::overloads_)
            {

                if (spo->tp->syms)
                {
                    spo = spo->tp->syms->front();
                    auto symtab = spo->tp->syms;
                    if (symtab && symtab->size())
                    {
                        auto sp2 = symtab->front();
                        if (sp2->tp->type == BasicType::void_)
                        {
                            tp->syms = symtab;
                        }
                    }
                }
            }
        }
        if (!MATCHKW(Keyword::closepa_))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(skim_closepa);
        }
        skip(Keyword::closepa_);
    }
    localNameSpace->pop_front();
    basisAttribs = oldAttribs;
    ParseAttributeSpecifiers(funcsp, true);
    if (voiderror)
        error(ERR_VOID_ONLY_PARAMETER);
    return tp;
}
Type* TypeGenerator::TypeId(SYMBOL* funcsp, StorageClass storage_class, bool beforeOnly, bool toErr, bool inUsing)
{
    Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    bool defd = false;
    SYMBOL* sp = nullptr;
    SYMBOL* strSym = nullptr;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    bool notype = false;
    bool oldTemplateType = inTemplateType;
    Type* tp = nullptr;

    getQualifiers(&tp, &linkage, &linkage2, &linkage3, nullptr);
    tp = TypeGenerator::UnadornedType(funcsp, tp, nullptr, false, funcsp ? StorageClass::auto_ : StorageClass::global_,
                                      &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd, nullptr, nullptr,
                                      nullptr, false, false, inUsing, nullptr, false);
    getQualifiers(&tp, &linkage, &linkage2, &linkage3, nullptr);
    tp = TypeGenerator::BeforeName(funcsp, tp, &sp, &strSym, &nsv, false, storage_class, &linkage, &linkage2, &linkage3,
                                   &notype, false, false, beforeOnly, false); /* fixme at file scope init */
    sizeQualifiers(tp);
    if (notype)
        tp = nullptr;
    else if (sp && !sp->sb->anonymous && toErr)
        if (sp->tp->type != BasicType::templateparam_)
            error(ERR_TOO_MANY_IDENTIFIERS);
    if (sp && sp->sb->anonymous && linkage != Linkage::none_)
    {
        if (sp->sb->attribs.inheritable.linkage != Linkage::none_)
            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
        else
            sp->sb->attribs.inheritable.linkage = linkage;
    }
    inTemplateType = oldTemplateType;
    return tp;
}
bool istype(SYMBOL* sym)
{
    if (!sym->sb || sym->sb->storage_class == StorageClass::templateparam_)
    {
        return sym->tp->templateParam->second->type == TplType::typename_ ||
               sym->tp->templateParam->second->type == TplType::template_;
    }
    return (sym->tp->type != BasicType::templateselector_ && sym->sb->storage_class == StorageClass::type_) ||
           sym->sb->storage_class == StorageClass::typedef_;
}
bool TypeGenerator::StartOfType(bool* structured, bool assumeType)
{
    if (structured)
        *structured = false;
    auto old = lines;
    if (!currentLex)
        return false;

    if (currentLex->type == LexType::l_id_)
    {
        auto tparam = TemplateLookupSpecializationParam(currentLex->value.s.a);
        if (tparam)
        {
            LexemeStreamPosition placeHolder(currentStream);
            bool member;
            getsym();
            member = MATCHKW(Keyword::classsel_);
            if (member)
            {
                getsym();
                member = MATCHKW(Keyword::star_);
            }
            placeHolder.Backup();
            if (!member)
            {
                lines = old;
                return tparam->second->type == TplType::typename_ || tparam->second->type == TplType::template_;
            }
        }
    }
    if (currentLex->type == LexType::l_id_ || MATCHKW(Keyword::classsel_) || MATCHKW(Keyword::decltype_))
    {
        bool isdecltype = MATCHKW(Keyword::decltype_);
        SYMBOL *sym, *strSym = nullptr;
        LexemeStreamPosition placeHolder(currentStream);
        bool dest = false;
        EnterPackedSequence();
        nestedSearch(&sym, &strSym, nullptr, &dest, nullptr, false, StorageClass::global_, false, false);
        ClearPackedSequence();
        LeavePackedSequence();
        if (Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL))
            placeHolder.Backup();
        lines = old;
        if (structured && sym && istype(sym))
        {
            if (sym->tp->type == BasicType::templateparam_)
            {
                if (sym->tp->templateParam->second->type == TplType::typename_ && sym->tp->templateParam->second->byClass.val)
                    *structured = sym->tp->templateParam->second->byClass.val->IsStructured();
                else if (sym->tp->templateParam->second->type == TplType::template_)
                    *structured = true;
            }
            else
            {
                *structured = sym->tp->IsStructured();
            }
        }
        return (!sym && isdecltype) || (sym && sym->tp && istype(sym)) ||
               (assumeType && strSym &&
                (strSym->tp->type == BasicType::templateselector_ || strSym->tp->type == BasicType::templatedecltype_));
    }
    else
    {
        lines = old;
        return KWTYPE(TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS | TT_TYPENAME);
    }
}

static Type* replaceTemplateSelector(Type* tp)
{
    if (!templateDefinitionLevel && tp->type == BasicType::templateselector_ && (*tp->sp->sb->templateSelector)[1].isTemplate)
    {
        SYMBOL* sp2 = (*tp->sp->sb->templateSelector)[1].sp;
        if ((*tp->sp->sb->templateSelector)[1].isDeclType)
        {
            Type* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
            if (tp1 && tp1->IsStructured())
                sp2 = tp1->BaseType()->sp;
            else
                sp2 = nullptr;
        }
        else
        {
            if (sp2)
            {
                SYMBOL* sp1 = GetClassTemplate(sp2, (*tp->sp->sb->templateSelector)[1].templateParams, true);
                if (sp1)
                {
                    sp1 = search(sp1->tp->syms, (*tp->sp->sb->templateSelector)[2].name);
                    if (sp1)
                    {
                        tp = sp1->tp;
                    }
                }
            }
        }
    }
    return tp;
}
}  // namespace Parser