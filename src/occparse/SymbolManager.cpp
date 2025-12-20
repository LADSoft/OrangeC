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

/*
 * iexpr.c
 *
 * routies to take an enode list and generate icode
 */
#include <cstdio>
#include <cstdlib>
#include "compiler.h"
#include "assert.h"
#include "ppPragma.h"
#include <string>
#include <vector>
#include <algorithm>
#include "PreProcessor.h"
#include "ccerr.h"
#include "ildata.h"
#include "occparse.h"
#include "lex.h"
#include "help.h"
#include "floatconv.h"
#include "memory.h"
#include "OptUtils.h"
#include "beinterf.h"
#include "initbackend.h"
#include "types.h"
#include "SymbolProperties.h"

namespace Parser
{
const char* beDecorateSymName(SYMBOL* sym)
{
    const char* q;
    if (sym->sb->attribs.uninheritable.alias)
    {
        return litlate(sym->sb->attribs.uninheritable.alias);
    }
    else
    {
        q = preProcessor->LookupAlias(sym->name);
        if (q)
            return litlate(q);
        else if (sym->sb->compilerDeclared || sym->tp->type == BasicType::templateparam_ ||
                 sym->tp->type == BasicType::templateselector_)
        {
            return litlate(sym->name);
        }
        else
        {
            return sym->sb->decoratedName;
        }
    }
}
}  // namespace Parser
using namespace Parser;

void Optimizer::SymbolManager::clear()
{
    if (architecture != ARCHITECTURE_MSIL || (cparams.prm_compileonly && !cparams.prm_asmfile))
        symbols.clear();
    globalSymbols.clear();
    initTokenStreams.clear();
    bodyTokenStreams.clear();
    noExceptTokenStreams.clear();
    functionDefinitions.clear();
    bodyArgs.clear();
}

Optimizer::SimpleSymbol* Optimizer::SymbolManager::Get(struct Parser::sym* sym, bool definingFunction)
{
    if (sym && sym->sb)
    {
        Optimizer::SimpleSymbol* rv;
        rv = Lookup(sym);
        if (!rv)
        {
            rv = Make(sym);
        }
        else
        {
            rv->outputName = sym->sb->decoratedName;
        }
        if (definingFunction && Optimizer::architecture != ARCHITECTURE_MSIL)
        {
            // the backend requires type info for all function params in a generated function
            // but, we don't want to generate all the param types for each call site
            // and each prototype.   So we just get in here for functions that are
            // actually being defined.
            if (sym->tp->BaseType()->syms && rv && !rv->syms)
            {
                Optimizer::LIST** p = &rv->syms;
                for (auto sp : *sym->tp->BaseType()->syms)
                {
                    *p = Allocate<Optimizer::LIST>();
                    (*p)->data = Get(sp);
                    EnterType((Optimizer::SimpleSymbol*)(*p)->data);
                    p = &(*p)->next;
                }
            }
        }
        return rv;
    }
    return nullptr;
}

Optimizer::SimpleSymbol* Optimizer::SymbolManager::Test(struct Parser::sym* sym)
{
    if (sym)
    {
        return Lookup(sym);
    }
    return nullptr;
}

Optimizer::SimpleExpression* Optimizer::SymbolManager::Get(struct Parser::expr* e)
{
    Optimizer::SimpleExpression* rv = Allocate<Optimizer::SimpleExpression>();
    while (e && (e->type == ExpressionNode::lvalue_ || e->type == ExpressionNode::not__lvalue_ ||
                 e->type == ExpressionNode::x_string_ || e->type == ExpressionNode::x_object_))
        e = e->left;
    if (!e)
    {
        rv->sizeFromType = ISZ_UINT;
        rv->type = Optimizer::se_ui;
        rv->i = 0;
        return rv;
    }
    rv->sizeFromType = natural_size(e);
    if (e->altdata)
        rv->altData = Get((EXPRESSION*)e->altdata);
    switch (e->type)
    {
        case ExpressionNode::global_:
            rv->type = Optimizer::se_global;
            rv->sp = Get(e->v.sp);
            break;
        case ExpressionNode::auto_:
            rv->type = Optimizer::se_auto;
            rv->sp = Get(e->v.sp);
            break;
        case ExpressionNode::labcon_:
            rv->type = Optimizer::se_labcon;
            rv->i = e->v.i;
            break;
        case ExpressionNode::absolute_:
            rv->type = Optimizer::se_absolute;
            rv->sp = Get(e->v.sp);
            break;
        case ExpressionNode::pc_:
            rv->type = Optimizer::se_pc;
            rv->sp = Get(e->v.sp);
            break;
        case ExpressionNode::const_:
            rv->type = Optimizer::se_const;
            rv->sp = Get(e->v.sp);
            break;
        case ExpressionNode::threadlocal_:
            rv->type = Optimizer::se_threadlocal;
            rv->sp = Get(e->v.sp);
            break;
        case ExpressionNode::structelem_:
            rv->type = Optimizer::se_structelem;
            rv->sp = Get(e->v.sp);
            break;

        case ExpressionNode::msil_array_init_:
            rv->type = Optimizer::se_msil_array_init;
            rv->tp = Get(e->v.tp);
            break;
        case ExpressionNode::msil_array_access_:
            rv->type = Optimizer::se_msil_array_access;
            rv->msilArrayTP = Get(e->v.msilArray->tp);
            break;

        case ExpressionNode::callsite_:
            rv->type = Optimizer::se_func;
            rv->ascall = e->v.func->ascall;
            break;
        case ExpressionNode::add_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::structadd_:
            rv->type = Optimizer::se_add;
            rv->left = Get(e->left);
            rv->right = Get(e->right);
            break;

        case ExpressionNode::sub_:
            rv->type = Optimizer::se_sub;
            rv->left = Get(e->left);
            rv->right = Get(e->right);
            break;
        case ExpressionNode::uminus_:
            rv->type = Optimizer::se_uminus;
            rv->left = Get(e->left);
            break;
        case ExpressionNode::c_bit_:
        case ExpressionNode::c_bool_:
        case ExpressionNode::c_c_:
        case ExpressionNode::c_s_:
        case ExpressionNode::c_i_:
        case ExpressionNode::c_l_:
        case ExpressionNode::c_ll_:
            rv->type = Optimizer::se_i;
            rv->i = reint(e);
            break;
        case ExpressionNode::c_uc_:
        case ExpressionNode::c_wc_:
        case ExpressionNode::c_u16_:
        case ExpressionNode::c_us_:
        case ExpressionNode::c_ui_:
        case ExpressionNode::c_u32_:
        case ExpressionNode::c_ul_:
        case ExpressionNode::c_ull_:
            rv->type = Optimizer::se_ui;
            rv->i = reint(e);
            break;
        case ExpressionNode::c_bitint_:
        case ExpressionNode::c_ubitint_:
            diag("SymbolManager::Get(), bitint constant being passed to backend");
            break;
        case ExpressionNode::c_f_:
        case ExpressionNode::c_d_:
        case ExpressionNode::c_ld_:
            rv->type = Optimizer::se_f;
            rv->f = refloat(e);
            break;
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
            rv->type = Optimizer::se_fi;
            rv->f = refloat(e);
            break;
        case ExpressionNode::c_fc_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::c_ldc_:
            rv->type = Optimizer::se_fc;
            rv->c.r = e->v.c->r;
            rv->c.i = e->v.c->i;
            break;
        case ExpressionNode::c_string_:
            rv->type = Optimizer::se_string;
            {
                char buf[50000], *dest = buf;
                if (e->string)
                {
                    int i;
                    bool done = false;
                    for (i = 0; i < e->string->size && !done; i++)
                    {
                        SLCHAR* s = e->string->pointers[i];
                        int j;
                        for (j = 0; j < s->count; j++)
                        {
                            *dest++ = s->str[j];
                            if (dest - buf >= sizeof(buf) - 1)
                            {
                                done = true;
                                break;
                            }
                        }
                    }
                }
                rv->astring.str = Allocate<char>(dest - buf);
                memcpy(rv->astring.str, buf, dest - buf);
                rv->astring.len = dest - buf;
            }
            break;
        case ExpressionNode::type_:
            rv->type = Optimizer::se_typeref;
            rv->tp = Get(e->v.tp);
            break;
        default:
            diag("unknown node in Get(EXPRESSION*)");
            break;
    }
    return rv;
}
Optimizer::SimpleType* Optimizer::SymbolManager::Get(Parser::Type* tp)
{
    int i = 0;
    Optimizer::SimpleType* rv = Allocate<Optimizer::SimpleType>();
    if (tp->IsDeferred())
    {
        rv->type = st_i;
        rv->size = getSize(BasicType::int_);
        rv->sizeFromType = -ISZ_UINT;
        return rv;
    }
    bool isConst = tp->IsConst();
    bool isVolatile = tp->IsVolatile();
    bool isRestrict = tp->IsRestrict();
    bool isAtomic = tp->IsAtomic();
    bool isvalist = false;
    bool istypedef = false;
    while (tp != tp->BaseType())
    {
        if (tp->type == BasicType::va_list_)
            isvalist = true;
        if (tp->type == BasicType::typedef_)
            istypedef = true;
        tp = tp->btp;
    }
    if (tp->IsStructured() && tp->BaseType()->sp && tp->BaseType()->sp->sb->structuredAliasType)
    {
        auto tp1 = tp->BaseType()->sp->sb->structuredAliasType;
        if (tp1->IsPtr())
            tp1 = &stdpointer;
        if (tp1->BaseType()->type == BasicType::enum_)
            tp1 = tp1->btp;
        rv->structuredAlias = Get(tp1);
    }
    if ((tp->IsStructured() && tp->BaseType()->sp->sb->templateLevel && !tp->BaseType()->sp->sb->instantiated) ||
        tp->BaseType()->type == BasicType::auto_)
    {
        rv->type = st_i;
        rv->size = getSize(BasicType::int_);
        rv->sizeFromType = -ISZ_UINT;
    }
    else
    {
        rv->type = Get(tp->type);

        if (tp->BaseType()->sp)
        {
            rv->sp = Get(tp->BaseType()->sp);
        }
        rv->size = tp->size;
        if (!tp->IsStructured() && !tp->IsFunction() && tp->type != BasicType::ellipse_ && tp->BaseType()->type != BasicType::any_)
            rv->sizeFromType = SizeFromType(tp);
        else
            rv->sizeFromType = ISZ_ADDR;
        rv->bits = tp->bits;
        rv->startbit = tp->startbit;
        rv->isarray = tp->array;
        rv->isvla = tp->vla;
        rv->va_list = tp->type == BasicType::va_list_;
        if (rv->sp)
        {
            // have to put functions in the type table if unreferenced int the global tables
            // this ignores pointer-to-functions
            if (rv->type == st_func && rv->sp && rv->sp->tp && rv->sp->tp->type == st_func)
                definedFunctions.insert(rv->sp);
            // have to put static member functions in the type table as well...
            if (rv->sp->storage_class == scc_type || rv->sp->storage_class == scc_parameter ||
                (rv->sp->storage_class == scc_cast && rv->sp->tp))
            {
                EnterType(rv->sp);
            }
            else if (rv->sp->storage_class == scc_typedef)
                typedefs.push_back(rv->sp);
        }
        if (tp->type != BasicType::aggregate_ && (!tp->IsFunction() || Optimizer::architecture == ARCHITECTURE_MSIL) && tp->syms &&
            tp->syms->size() && rv->sp && !rv->sp->syms)
        {
            Optimizer::LIST** p = &rv->sp->syms;
            for (auto sp : *tp->syms)
            {
                *p = Allocate<Optimizer::LIST>();
                if (tp->IsFunction() && sp->sb->parent)
                    sp->sb->parent->sb->decoratedName = tp->BaseType()->sp->sb->decoratedName;
                (*p)->data = Get(sp);
                if (rv->sp->storage_class == scc_type || rv->sp->storage_class == scc_cast)
                {
                    if (sp->sb->storage_class == StorageClass::static_ || sp->tp->IsFunction())
                    {
                        Optimizer::SimpleSymbol* ns = Allocate<Optimizer::SimpleSymbol>();
                        *ns = *(Optimizer::SimpleSymbol*)(*p)->data;
                        (*p)->data = ns;
                    }
                    EnterType((Optimizer::SimpleSymbol*)(*p)->data);
                }
                else if (tp->IsFunction())
                {
                    EnterType((Optimizer::SimpleSymbol*)(*p)->data);
                }
                if (((Optimizer::SimpleSymbol*)(*p)->data)->storage_class == scc_static ||
                    ((Optimizer::SimpleSymbol*)(*p)->data)->storage_class == scc_external)
                    definedFunctions.insert((Optimizer::SimpleSymbol*)(*p)->data);
                p = &(*p)->next;
            }
        }
        if (tp->btp)
            rv->btp = Get(tp->btp);
    }
    rv->msil = tp->msil;
    rv->scoped = tp->scoped;
    rv->isconst = isConst;
    rv->isvolatile = isVolatile;
    rv->isrestrict = isRestrict;
    rv->isatomic = isAtomic;
    rv->va_list = isvalist;
    rv->istypedef = istypedef;
    return rv;
}
namespace Parser
{
void refreshBackendParams(SYMBOL* funcsp)
{
    if (funcsp->tp->IsFunction())
    {
        funcsp->tp->BaseType()->sp = funcsp;
        auto it = funcsp->tp->BaseType()->syms->begin();
        auto ite = funcsp->tp->BaseType()->syms->end();
        Optimizer::LIST* syms = Optimizer::SymbolManager::Get(funcsp)->syms;
        while (it != ite && syms)
        {
            Optimizer::SimpleSymbol* sym = (Optimizer::SimpleSymbol*)syms->data;
            if ((*it)->sb->thisPtr && !sym->thisPtr)
            {
                ++it;
            }
            sym->name = (*it)->name;  // update name to prototype in use...
            sym->offset = (*it)->sb->offset;
            Optimizer::SymbolManager::Get((*it))->offset = sym->offset;
            ++it;
            syms = syms->next;
        }
    }
}
}  // namespace Parser
Optimizer::SimpleSymbol* Optimizer::SymbolManager::Make(struct Parser::sym* sym)
{
    Optimizer::SimpleSymbol* rv = Allocate<Optimizer::SimpleSymbol>();
    rv->name = sym->name;
    rv->align = sym->sb->attribs.inheritable.structAlign ? sym->sb->attribs.inheritable.structAlign
                                                         : getAlign(StorageClass::auto_, sym->tp->BaseType());
    rv->size = sym->tp->BaseType()->size;
    rv->importfile = sym->sb->importfile;
    if (sym->sb->parentNameSpace)
        rv->namespaceName = sym->sb->parentNameSpace->name;
    rv->i = sym->sb->value.i;
    Add(sym, rv);
    rv->storage_class = Get(sym->sb->storage_class);
    if (!sym->tp->IsStructured() && !sym->tp->IsFunction() && sym->tp->type != BasicType::ellipse_ &&
        sym->tp->BaseType()->type != BasicType::any_ && !sym->tp->IsDeferred())
        rv->sizeFromType = SizeFromType(sym->tp);
    else
        rv->sizeFromType = ISZ_ADDR;
    rv->offset = sym->sb->offset;
    BaseList** p = &rv->baseClasses;
    if (sym->sb->baseClasses)
    {
        for (auto src : *sym->sb->baseClasses)
        {
            *p = Allocate<BaseList>();
            (*p)->offset = src->offset;
            (*p)->sym = Get(src->cls);
            if ((*p)->sym->tp && (*p)->sym->tp->type == st_i)
                EnterType((*p)->sym);
            p = &(*p)->next;
        }
    }
    rv->label = sym->sb->label;
    rv->tp = Get(sym->tp);
    // normalize for middle and backend.   Not modifying the front Keyword::end_ data structures because there would be too much
    // ripple...
    if (rv->storage_class == scc_parameter && rv->tp->isarray)
    {
        rv->tp->size = getSize(BasicType::pointer_);
        rv->tp->sizeFromType = ISZ_ADDR;
    }
    rv->parentClass = Get(sym->sb->parentClass);
    rv->msil = sym->sb->msil;
    rv->templateLevel = sym->sb->templateLevel;
    rv->hasParams = sym->sb->paramsize != 0;
    rv->isstructured = sym->tp->IsStructured();
    rv->anonymous = sym->sb->anonymous;
    rv->allocate = sym->sb->allocate;
    rv->thisPtr = sym->sb->thisPtr;
    rv->stackblock = sym->sb->stackblock;
    rv->inasm = sym->sb->inasm;
    rv->isimport = sym->sb->attribs.inheritable.linkage2 == Linkage::import_;
    rv->isexport = sym->sb->attribs.inheritable.linkage2 == Linkage::export_;
    rv->isvirtual = sym->sb->attribs.inheritable.linkage4 == Linkage::virtual_;
    rv->isinternal = sym->sb->attribs.inheritable.linkage2 == Linkage::internal_;
    rv->msil_rtl = sym->sb->attribs.inheritable.linkage2 == Linkage::msil_rtl_;
    rv->isproperty = sym->sb->attribs.inheritable.linkage2 == Linkage::property_;
    rv->unmanaged = sym->sb->attribs.inheritable.linkage2 == Linkage::unmanaged_;
    rv->isstdcall = sym->sb->attribs.inheritable.linkage == Linkage::stdcall_;
    rv->iscdecl = sym->sb->attribs.inheritable.linkage == Linkage::cdecl_;
    rv->ispascal = sym->sb->attribs.inheritable.linkage == Linkage::pascal_;
    rv->isfastcall = sym->sb->attribs.inheritable.linkage == Linkage::fastcall_;
    rv->entrypoint = sym->sb->attribs.inheritable.linkage3 == Linkage::entrypoint_;
    rv->ispure = sym->sb->ispure;
    rv->dontinstantiate = sym->sb->dontinstantiate;
    rv->initialized = !!sym->sb->init;
    rv->importThunk = sym->sb->importThunk;

    rv->inFunc = theCurrentFunc != nullptr;
    rv->temp = sym->sb->temp;
    rv->vbase = !!sym->sb->vbaseEntries;
    rv->anyTry = sym->sb->anyTry;
    rv->xc = !!sym->sb->xc;
    rv->canThrow = sym->sb->canThrow;
    rv->hasInlineFunc = sym->sb->inlineFunc.stmt != nullptr;
    rv->usesEsp = cparams.prm_useesp;

    rv->outputName = beDecorateSymName(sym);
    rv->stackProtectExplicit = sym->sb->attribs.uninheritable.stackProtect;
    if (sym->tp->IsArray())
    {
        // the backend will also check if the address of a variable is being taken...
        // and before we pack up the function we will check theCurrentFunc->sb->usesAlloca
        // we don't know any of that definitively right now.
        rv->stackProtectStrong = true;
        Type* tp1 = sym->tp;
        while (tp1->IsArray())
            tp1 = tp1->BaseType()->btp;
        tp1 = tp1->BaseType();
        rv->stackProtectBasic = (tp1->type == BasicType::char_ || tp1->type == BasicType::unsigned_char_) &&
                                sym->tp->size >= STACK_PROTECT_MINIMUM_CONSIDERED;
    }
    return rv;
}
Optimizer::st_type Optimizer::SymbolManager::Get(Parser::BasicType type)
{

    switch (type)
    {
        case BasicType::char_:
        case BasicType::signed_char_:
        case BasicType::bool_:
        case BasicType::short_:
        case BasicType::int_:
        case BasicType::long_:
        case BasicType::long_long_:
        case BasicType::inative_:
        case BasicType::bitint_:
            return st_i;
        case BasicType::char8_t_:
        case BasicType::unsigned_char_:
        case BasicType::unsigned_short_:
        case BasicType::char16_t_:
        case BasicType::wchar_t_:
        case BasicType::unsigned_:
        case BasicType::unsigned_long_:
        case BasicType::unsigned_long_long_:
        case BasicType::char32_t_:
        case BasicType::unative_:
        case BasicType::unsigned_bitint_:
            return st_ui;
        case BasicType::float_:
        case BasicType::double_:
        case BasicType::long_double_:
            return st_f;
        case BasicType::float__imaginary_:
        case BasicType::double__imaginary_:
        case BasicType::long_double_imaginary_:
            return st_fi;
        case BasicType::float__complex_:
        case BasicType::double__complex_:
        case BasicType::long_double_complex_:
            return st_fc;
        case BasicType::pointer_:
            return st_pointer;
        case BasicType::void_:
            return st_void;
        case BasicType::string_:
            return st___string;
        case BasicType::object_:
            return st___object;
        case BasicType::func_:
        case BasicType::ifunc_:
            return st_func;
        case BasicType::lref_:
            return st_lref;
        case BasicType::rref_:
            return st_rref;
        case BasicType::struct_:
        case BasicType::class_:
            return st_struct;
        case BasicType::union_:
            return st_union;
        case BasicType::enum_:
            return st_enum;
        case BasicType::memberptr_:
            return st_memberptr;
        case BasicType::aggregate_:
            return st_aggregate;
        case BasicType::ellipse_:
            return st_ellipse;
        case BasicType::any_:
        default:
            return st_any;
    }
}

Optimizer::e_scc_type Optimizer::SymbolManager::Get(Parser::StorageClass storageClass)
{
    switch (storageClass)
    {
        default:
        case StorageClass::none_:
            return scc_none;
        case StorageClass::static_:
            return scc_static;
        case StorageClass::localstatic_:
            return scc_localstatic;
        case StorageClass::auto_:
            return scc_auto;
        case StorageClass::register_:
            return scc_register;
        case StorageClass::global_:
            return scc_global;
        case StorageClass::external_:
            return scc_external;
        case StorageClass::templateparam_:
            return scc_templateparam;
        case StorageClass::parameter_:
            return scc_parameter;
        case StorageClass::catchvar_:
            return scc_catchvar;
        case StorageClass::type_:
            return scc_type;
        case StorageClass::typedef_:
            return scc_typedef;
        case StorageClass::member_:
            return scc_member;
        case StorageClass::mutable_:
            return scc_mutable;
        case StorageClass::cast_:
            return scc_cast;
        case StorageClass::defunc_:
            return scc_defunc;
        case StorageClass::label_:
            return scc_label;
        case StorageClass::ulabel_:
            return scc_ulabel;
        case StorageClass::overloads_:
            return scc_overloads;
        case StorageClass::constant_:
            return scc_constant;
        case StorageClass::enumconstant_:
            return scc_enumconstant;
        case StorageClass::absolute_:
            return scc_absolute;
        case StorageClass::friendlist_:
            return scc_friendlist;
        case StorageClass::const_:
            return scc_const;
        case StorageClass::tconst_:
            return scc_tconst;
        case StorageClass::classmember_:
            return scc_classmember;
        case StorageClass::constexpr_:
            return scc_constexpr;
        case StorageClass::memberreg_:
            return scc_memberreg;
        case StorageClass::namespace_:
            return scc_namespace;
        case StorageClass::namespace_alias_:
            return scc_namespacealias;
        case StorageClass::temp_:
            return scc_temp;
        case StorageClass::virtual_:
            return scc_virtual;
    }
}
unsigned long long Optimizer::SymbolManager::Key(struct Parser::sym* old)
{
    char buf[8192];
    buf[0] = 0;
    if (old->sb->parent)
    {
        Utils::StrCat(buf, old->sb->parent->sb->decoratedName);
        int l = strlen(buf);
        my_sprintf(buf + l, sizeof(buf) - l, "%d", old->uniqueId);
    }
    Utils::StrCat(buf, old->sb->decoratedName ? old->sb->decoratedName : old->name);
    if (old->sb->storage_class == StorageClass::static_ && !old->sb->parent)
    {
        int l = strlen(buf);
        my_sprintf(buf + l, sizeof(buf) - l, "%d", old->uniqueId);
    }
    if (old->sb->storage_class == StorageClass::type_)
        Utils::StrCat(buf, "#");
    if (old->sb->attribs.inheritable.linkage == Linkage::stdcall_)
        Utils::StrCat(buf, ".");
    std::hash<std::string> hasher;
    std::string aa(buf);
    size_t key = hasher(aa);
    std::reverse(aa.begin(), aa.end());
    size_t key2 = hasher(aa);
    return ((unsigned long long)key << 32) | key2;
}
Optimizer::SimpleSymbol* Optimizer::SymbolManager::Lookup(struct Parser::sym* old)
{
    if (old->sb->symRef)
        return old->sb->symRef;
    Optimizer::SimpleSymbol* rv = symbols[Key(old)];
    if (rv)
        old->sb->symRef = rv;
    return rv;
}
void Optimizer::SymbolManager::Add(struct Parser::sym* old, Optimizer::SimpleSymbol* sym)
{
    old->sb->symRef = sym;
    symbols[Key(old)] = sym;
    switch (sym->storage_class)
    {
        case scc_member:
            if (sym->tp->type != st_func)
                break;
            // fallthrough
        case scc_global:
        case scc_static:
        case scc_localstatic:
        case scc_external:
        case scc_virtual:
            globalSymbols[sym->outputName] = sym;
            break;
    }
}