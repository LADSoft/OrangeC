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

#include "compiler.h"
#include <cstdarg>
#include "ccerr.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "declare.h"
#include "OptUtils.h"
#include "expr.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "init.h"
#include "mangle.h"
#include "lex.h"
#include "constopt.h"
#include "memory.h"
#include "cpplookup.h"
#include "beinterf.h"
#include "declcons.h"
#include "help.h"
#include "declcpp.h"
#include "ildata.h"
#include "symtab.h"
#include "ListFactory.h"
#include "types.h"
namespace Parser
{

int anonymousNotAlloc;

static int staticanonymousIndex;

void helpinit()
{
    anonymousNotAlloc = 0;
    staticanonymousIndex = 1;
}
void deprecateMessage(SYMBOL* sym)
{
    char buf[1024];
    Optimizer::my_sprintf(buf, "%s deprecated", sym->name);
    if (sym->sb->attribs.uninheritable.deprecationText && sym->sb->attribs.uninheritable.deprecationText != (char*)-1)
        Optimizer::my_sprintf(buf + strlen(buf), "; %s", sym->sb->attribs.uninheritable.deprecationText);
    errorstr(ERR_WARNING, buf);
}
// well this is really only nonstatic data members...
bool ismember(SYMBOL* sym)
{
    switch (sym->sb->storage_class)
    {
        case StorageClass::member_:
        case StorageClass::mutable_:
        case StorageClass::virtual_:
            return true;
        default:
            return false;
    }
}
bool istype(SYMBOL* sym)
{
    if (!sym->sb || sym->sb->storage_class == StorageClass::templateparam_)
    {
        return sym->tp->templateParam->second->type == TplType::typename_ || sym->tp->templateParam->second->type == TplType::template_;
    }
    return (sym->tp->type != BasicType::templateselector_ && sym->sb->storage_class == StorageClass::type_) || sym->sb->storage_class == StorageClass::typedef_;
}
bool ismemberdata(SYMBOL* sym) { return !isfunction(sym->tp) && ismember(sym); }
bool startOfType(LEXLIST* lex, bool* structured, bool assumeType)
{
    if (structured)
        *structured = false;
    auto old = lines;
    if (!lex)
        return false;

    if (lex->data->type == l_id)
    {
        auto tparam = TemplateLookupSpecializationParam(lex->data->value.s.a);
        if (tparam)
        {
            LEXLIST* placeHolder = lex;
            bool member;
            lex = getsym();
            member = MATCHKW(lex, Keyword::classsel_);
            if (member)
            {
                lex = getsym();
                member = MATCHKW(lex, Keyword::star_);
            }
            lex = prevsym(placeHolder);
            if (!member)
            {
                lines = old;
                return tparam->second->type == TplType::typename_ || tparam->second->type == TplType::template_;
            }
        }
    }
    if (lex->data->type == l_id || MATCHKW(lex, Keyword::classsel_) || MATCHKW(lex, Keyword::decltype_))
    {
        bool isdecltype = MATCHKW(lex, Keyword::decltype_);
        SYMBOL *sym, *strSym = nullptr;
        LEXLIST* placeholder = lex;
        bool dest = false;
        nestedSearch(lex, &sym, &strSym, nullptr, &dest, nullptr, false, StorageClass::global_, false, false);
        if (Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL))
            prevsym(placeholder);
        lines = old;
        if (structured && sym && istype(sym))
        {
            if (sym->tp->type == BasicType::templateparam_)
            {
                if (sym->tp->templateParam->second->type == TplType::typename_ && sym->tp->templateParam->second->byClass.val)
                    *structured = isstructured(sym->tp->templateParam->second->byClass.val);
                else if (sym->tp->templateParam->second->type == TplType::template_)
                    *structured = true;
            }
            else
            {
                *structured = isstructured(sym->tp);
            }
        }
        return (!sym && isdecltype) || (sym && sym->tp && istype(sym)) ||
               (assumeType && strSym && (strSym->tp->type == BasicType::templateselector_ || strSym->tp->type == BasicType::templatedecltype_));
    }
    else
    {
        lines = old;
        return KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS | TT_TYPENAME);
    }
}
static TYPE* rootType(TYPE* tp)
{
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
void UpdateRootTypes(TYPE* tp)
{
    while (tp)
    {
        TYPE* tp1 = rootType(tp);
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
bool isDerivedFromTemplate(TYPE* tp)
{
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::pointer_:
            case BasicType::func_:
            case BasicType::ifunc_:
                return false;
            case BasicType::derivedfromtemplate_:
                return true;
            default:
                break;
        }
        tp = tp->btp;
    }
    return false;
}
bool isautotype(TYPE* tp)
{
    if (isref(tp))
        tp = basetype(tp)->btp;
    while (ispointer(tp))
        tp = basetype(tp)->btp;
    return basetype(tp)->type == BasicType::auto_;
}
bool isunsigned(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
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
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool isbitint(TYPE* tp) 
{
    tp = basetype(tp);
    if (tp)
    {
        return tp->type == BasicType::bitint_ || tp->type == BasicType::unsigned_bitint_;
    }
    return false;
}
bool isint(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
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
                    return isint(tp->templateParam->second->byNonType.tp);
                return false;
            default:
                if (tp->type == BasicType::enum_ && !Optimizer::cparams.prm_cplusplus)
                    return true;

                return false;
        }
    }
    return false;
}
bool isfloat(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
            case BasicType::float_:
            case BasicType::double_:
            case BasicType::long_double_:
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool iscomplex(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
            case BasicType::float__complex_:
            case BasicType::double__complex_:
            case BasicType::long_double_complex_:
                return true;
            default:
                return false;
        }
    }
    return false;
}
bool isimaginary(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
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
    return false;
}
bool isarithmetic(TYPE* tp)
{
    tp = basetype(tp);
    return isint(tp) || isfloat(tp) || iscomplex(tp) || isimaginary(tp);
}
bool ismsil(TYPE* tp)
{
    tp = basetype(tp);
    return tp->type == BasicType::string_ || tp->type == BasicType::object_;
}
bool isconstraw(const TYPE* tp)
{
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
bool isconst(const TYPE* tp) { return isconstraw(tp); }
// prefer a const function when the expression is a constexpression variable
bool isconstexpr(const EXPRESSION* expa)
{
    if (expa)
    {
        switch (expa->type)
        {
            case ExpressionNode::global_:
            case ExpressionNode::auto_:
            case ExpressionNode::absolute_:
            case ExpressionNode::pc_:
            case ExpressionNode::threadlocal_:
                return expa->v.sp->sb->constexpression;
        }
    }
    return false;
}
bool isvolatile(const TYPE* tp)
{
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
bool islrqual(TYPE* tp)
{
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
bool isrrqual(TYPE* tp)
{
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
bool isrestrict(TYPE* tp)
{
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
bool isatomic(TYPE* tp)
{
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
bool isvoid(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
        return tp->type == BasicType::void_;
    return false;
}
bool isvoidptr(TYPE* tp)
{
    tp = basetype(tp);
    return ispointer(tp) && isvoid(tp->btp);
}
bool isarray(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
        return ispointer(tp) && basetype(tp)->array;
    return false;
}
bool isunion(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
        return tp->type == BasicType::union_;
    return false;
}
void DeduceAuto(TYPE** pat, TYPE* nt, EXPRESSION* exp)
{
    TYPE* patin = *pat;
    TYPE* in = nt;
    if (isautotype(*pat))
    {
        bool pointerOrRef = false;
        bool err = false;
        if (isref(*pat))
        {
            if ((*pat)->type == BasicType::rref_ && !isconst((*pat)->btp) && !isvolatile((*pat)->btp))
            {
                // forwarding?  unadorned rref!
                if (!nt->rref && basetype(nt)->type != BasicType::rref_ && (!isarithmeticconst(exp) || exp->type == ExpressionNode::const_))
                {
                    // lref
                    TYPE* t = nt;
                    *pat = MakeType(BasicType::lref_, isref(t) ? t->btp : t);
                    return;
                }
                else
                {
                    // rref, get rid of qualifiers and return an rref
                    TYPE* tp1;
                    if (nt->type == BasicType::rref_)
                        tp1 = basetype(nt->btp);
                    else
                        tp1 = basetype(nt);
                    // rref
                    *pat = MakeType(BasicType::lref_, isref(tp1) ? tp1->btp : tp1);
                    return;
                }
            }
            if (isref(nt))
            {
                nt = basetype(nt)->btp;
            }
            pointerOrRef = true;
            pat = &basetype(*pat)->btp;
        }
        while (ispointer(*pat) && ispointer(nt))
        {
            pointerOrRef = true;
            pat = &basetype(*pat)->btp;
            nt = basetype(nt)->btp;
        }
        if (ispointer(*pat))
            err = true;
        nt = basetype(nt);
        if (err)
        {
            errortype(ERR_CANNOT_DEDUCE_AUTO_TYPE, patin, in);
        }
        else if (!pointerOrRef)
        {
            if ((*pat)->decltypeauto)
                if ((*pat)->decltypeautoextended)
                {
                    *pat = MakeType(BasicType::lref_, nt);
                }
                else
                {
                    *pat = nt;
                }
            else if (isref(nt))
            {
                *pat = basetype(nt)->btp;
            }
            else
            {
                *pat = nt;
            }
        }
        else if (ispointer(in))
        {
            *pat = nt;
        }
        else
        {
            while ((*pat)->type != BasicType::auto_)
                pat = &(*pat)->btp;
            *pat = nt;
        }
    }
}
SYMBOL* getFunctionSP(TYPE** tp)
{
    TYPE* btp = basetype(*tp);
    bool pointer = ispointer(btp);
    if (pointer)
    {
        btp = basetype(btp)->btp;
    }
    if (isfunction(btp))
    {
        *tp = btp;
        return basetype(btp)->sp;
    }
    else if (btp->type == BasicType::aggregate_)
    {
        return btp->sp;
    }
    return nullptr;
}
LEXLIST* concatStringsInternal(LEXLIST* lex, STRING** str, int* elems)
{
    Optimizer::SLCHAR** list;
    char* suffix = nullptr;
    int count = 3;
    int pos = 0;
    e_lexType type = l_astr;
    STRING* string;
    list = Allocate<Optimizer::SLCHAR*>(count);
    while (lex && (lex->data->type == l_astr || lex->data->type == l_wstr || lex->data->type == l_ustr ||
                   lex->data->type == l_Ustr || lex->data->type == l_msilstr || lex->data->type == l_u8str ))
    {
        if (lex->data->type == l_u8str)
            type = l_u8str;
        else if (lex->data->type == l_msilstr)
            type = l_msilstr;
        else if (lex->data->type == l_Ustr)
            type = l_Ustr;
        else if (type != l_Ustr && type != l_msilstr && lex->data->type == l_ustr)
            type = l_ustr;
        else if (type != l_Ustr && type != l_ustr && type != l_msilstr && lex->data->type == l_wstr)
            type = l_wstr;
        if (lex->data->suffix)
        {
            if (suffix)
            {
                if (strcmp(lex->data->suffix, suffix) != 0)
                    error(ERR_LITERAL_SUFFIX_MISMATCH);
            }
            else
            {
                suffix = lex->data->suffix;
            }
        }
        if (pos >= count)
        {
            Optimizer::SLCHAR** h = Allocate<Optimizer::SLCHAR*>(count + 10);
            memcpy(h, list, sizeof(Optimizer::SLCHAR*) * count);
            list = h;
            count += 10;
        }
        if (elems)
            *elems += ((Optimizer::SLCHAR*)lex->data->value.s.w)->count;
        list[pos++] = (Optimizer::SLCHAR*)lex->data->value.s.w;
        lex = getsym();
    }
    string = Allocate<STRING>();
    string->strtype = type;
    string->size = pos;
    string->pointers = Allocate<Optimizer::SLCHAR*>(pos);
    string->suffix = suffix;
    memcpy(string->pointers, list, pos * sizeof(Optimizer::SLCHAR*));
    *str = string;
    return lex;
}
LEXLIST* concatStrings(LEXLIST* lex, EXPRESSION** expr, e_lexType* tp, int* elems)
{
    STRING* data;
    lex = concatStringsInternal(lex, &data, elems);
    *expr = stringlit(data);
    *tp = data->strtype;
    return lex;
}
bool isintconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::c_u16_:
        case ExpressionNode::c_u32_:
        case ExpressionNode::c_c_:
        case ExpressionNode::c_wc_:
        case ExpressionNode::c_uc_:
        case ExpressionNode::c_s_:
        case ExpressionNode::c_us_:
        case ExpressionNode::c_i_:
        case ExpressionNode::c_ui_:
        case ExpressionNode::c_l_:
        case ExpressionNode::c_ul_:
        case ExpressionNode::c_ll_:
        case ExpressionNode::c_ull_:
        case ExpressionNode::c_bit_:
        case ExpressionNode::c_bool_:
        case ExpressionNode::const_:
            return true;
        default:
            return false;
    }
}
bool isfloatconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::c_f_:
        case ExpressionNode::c_d_:
        case ExpressionNode::c_ld_:
            return true;
        default:
            return false;
    }
}
bool isimaginaryconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
            return true;
        default:
            return false;
    }
}
bool iscomplexconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::c_fc_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::c_ldc_:
            return true;
        default:
            return false;
    }
}
EXPRESSION* anonymousVar(StorageClass storage_class, TYPE* tp)
{
    static int anonct = 1;
    char buf[256];
    SYMBOL* rv = SymAlloc();
    if (tp->size == 0 && isstructured(tp))
        tp = basetype(tp)->sp->tp;
    rv->sb->storage_class = storage_class;
    rv->tp = tp;
    rv->sb->anonymous = true;
    rv->sb->allocate = !anonymousNotAlloc;
    rv->sb->assigned = true;
    rv->sb->attribs.inheritable.used = true;
    if (theCurrentFunc)
        rv->sb->value.i = theCurrentFunc->sb->value.i;
    Optimizer::my_sprintf(buf, "$anontemp%d", anonct++);
    rv->name = litlate(buf);
    if (!isatomic(tp))
        tp->size = basetype(tp)->size;
    if (theCurrentFunc && localNameSpace->front()->syms && !inDefaultParam && !anonymousNotAlloc)
        InsertSymbol(rv, storage_class, Linkage::none_, false);
    SetLinkerNames(rv, Linkage::none_);
    return varNode(storage_class == StorageClass::auto_ || storage_class == StorageClass::parameter_ ? ExpressionNode::auto_ : ExpressionNode::global_, rv);
}
EXPRESSION* anonymousBits(StorageClass storageClass, bool issigned, int bits)
{
    TYPE* tp = MakeType(issigned ? BasicType::bitint_ : BasicType::unsigned_bitint_);
    tp->size = bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
    tp->size /= Optimizer::chosenAssembler->arch->bitintunderlying;
    tp->size *= Optimizer::chosenAssembler->arch->bitintunderlying;
    tp->size /= CHAR_BIT;
    tp->bitintbits = bits;
    return anonymousVar(storageClass, tp);
}
void deref(TYPE* tp, EXPRESSION** exp)
    {
    ExpressionNode en = ExpressionNode::l_i_;
    tp = basetype(tp);
    switch ((tp->type == BasicType::enum_ && tp->btp) ? tp->btp->type : tp->type)
    {
        case BasicType::lref_: /* only used during initialization */
            en = ExpressionNode::l_ref_;
            break;
        case BasicType::rref_: /* only used during initialization */
            en = ExpressionNode::l_ref_;
            break;
        case BasicType::bit_:
            en = ExpressionNode::l_bit_;
            break;
        case BasicType::bool_:
            en = ExpressionNode::l_bool_;
            break;
        case BasicType::char_:
            if (Optimizer::cparams.prm_charisunsigned)
                en = ExpressionNode::l_uc_;
            else
                en = ExpressionNode::l_c_;
            break;
        case BasicType::char8_t_:
            en = ExpressionNode::l_uc_;
            break;
        case BasicType::signed_char_:
            en = ExpressionNode::l_c_;
            break;
        case BasicType::char16_t_:
            en = ExpressionNode::l_u16_;
            break;
        case BasicType::char32_t_:
            en = ExpressionNode::l_u32_;
            break;
        case BasicType::unsigned_char_:
            en = ExpressionNode::l_uc_;
            break;
        case BasicType::short_:
            en = ExpressionNode::l_s_;
            break;
        case BasicType::unsigned_short_:
            en = ExpressionNode::l_us_;
            break;
        case BasicType::wchar_t_:
            en = ExpressionNode::l_wc_;
            break;
        case BasicType::int_:
            en = ExpressionNode::l_i_;
            break;
        case BasicType::unsigned_:
            en = ExpressionNode::l_ui_;
            break;
        case BasicType::long_:
            en = ExpressionNode::l_l_;
            break;
        case BasicType::unsigned_long_:
            en = ExpressionNode::l_ul_;
            break;
        case BasicType::long_long_:
            en = ExpressionNode::l_ll_;
            break;
        case BasicType::unsigned_long_long_:
            en = ExpressionNode::l_ull_;
            break;
        case BasicType::bitint_:
            en = ExpressionNode::l_bitint_;
            break;
        case BasicType::unsigned_bitint_:
            en = ExpressionNode::l_ubitint_;
            break;
        case BasicType::float_:
            en = ExpressionNode::l_f_;
            break;
        case BasicType::double_:
            en = ExpressionNode::l_d_;
            break;
        case BasicType::long_double_:
            en = ExpressionNode::l_ld_;
            break;
        case BasicType::float__complex_:
            en = ExpressionNode::l_fc_;
            break;
        case BasicType::double__complex_:
            en = ExpressionNode::l_dc_;
            break;
        case BasicType::long_double_complex_:
            en = ExpressionNode::l_ldc_;
            break;
        case BasicType::float__imaginary_:
            en = ExpressionNode::l_fi_;
            break;
        case BasicType::double__imaginary_:
            en = ExpressionNode::l_di_;
            break;
        case BasicType::long_double_imaginary_:
            en = ExpressionNode::l_ldi_;
            break;
        case BasicType::string_:
            en = ExpressionNode::l_string_;
            break;
        case BasicType::object_:
            en = ExpressionNode::l_object_;
            break;
        case BasicType::pointer_:
            if (tp->array || tp->vla)
                return;
            en = ExpressionNode::l_p_;
            break;
        case BasicType::inative_:
            en = ExpressionNode::l_inative_;
            break;
        case BasicType::unative_:
            en = ExpressionNode::l_unative_;
            break;
        case BasicType::struct_:
        case BasicType::class_:
        case BasicType::union_:
        case BasicType::func_:
        case BasicType::ifunc_:
        case BasicType::any_:
        case BasicType::templateparam_:
        case BasicType::templateselector_:
        case BasicType::templatedecltype_:
        case BasicType::memberptr_:
        case BasicType::aggregate_:
            return;
        default:
            diag("deref error");
            break;
    }
    *exp = exprNode(en, *exp, nullptr);
    if (isbitint(tp))
        (*exp)->v.b.bits = tp->bitintbits;
    if (en == ExpressionNode::l_object_)
        (*exp)->v.tp = tp;
}
int sizeFromType(TYPE* tp)
{
    int rv = -ISZ_UINT;
    tp = basetype(tp);
    switch (tp->type == BasicType::enum_ ? tp->btp->type : tp->type)
    {
        case BasicType::void_:
        case BasicType::templateparam_:
        case BasicType::templateselector_:
        case BasicType::templatedecltype_:
            rv = ISZ_UINT;
            break;
        case BasicType::bool_:
            rv = ISZ_BOOLEAN;
            break;
        case BasicType::char_:
            if (Optimizer::cparams.prm_charisunsigned)
                rv = ISZ_UCHAR;
            else
                rv = -ISZ_UCHAR;
            break;
        case BasicType::char8_t_:
            rv = ISZ_UCHAR;
            break;
        case BasicType::signed_char_:
            rv = -ISZ_UCHAR;
            break;
        case BasicType::char16_t_:
            rv = ISZ_U16;
            break;
        case BasicType::char32_t_:
            rv = ISZ_U32;
            break;
        case BasicType::unsigned_char_:
            rv = ISZ_UCHAR;
            break;
        case BasicType::short_:
            rv = -ISZ_USHORT;
            break;
        case BasicType::unsigned_short_:
            rv = ISZ_USHORT;
            break;
        case BasicType::wchar_t_:
            rv = ISZ_WCHAR;
            break;
        case BasicType::int_:
            rv = -ISZ_UINT;
            break;
        case BasicType::inative_:
            rv = -ISZ_UNATIVE;
            break;
        case BasicType::unsigned_:
            rv = ISZ_UINT;
            break;
        case BasicType::unative_:
            rv = ISZ_UNATIVE;
            break;
        case BasicType::long_:
            rv = -ISZ_ULONG;
            break;
        case BasicType::unsigned_long_:
            rv = ISZ_ULONG;
            break;
        case BasicType::long_long_:
            rv = -ISZ_ULONGLONG;
            break;
        case BasicType::unsigned_long_long_:
            rv = ISZ_ULONGLONG;
            break;
        case BasicType::bitint_:
            rv = -ISZ_BITINT;
            break;
        case BasicType::unsigned_bitint_:
            rv = ISZ_BITINT;
            break;
        case BasicType::float_:
            rv = ISZ_FLOAT;
            break;
        case BasicType::double_:
            rv = ISZ_DOUBLE;
            break;
        case BasicType::long_double_:
            rv = ISZ_LDOUBLE;
            break;
        case BasicType::float__complex_:
            rv = ISZ_CFLOAT;
            break;
        case BasicType::double__complex_:
            rv = ISZ_CDOUBLE;
            break;
        case BasicType::long_double_complex_:
            rv = ISZ_CLDOUBLE;
            break;
        case BasicType::float__imaginary_:
            rv = ISZ_IFLOAT;
            break;
        case BasicType::double__imaginary_:
            rv = ISZ_IDOUBLE;
            break;
        case BasicType::long_double_imaginary_:
            rv = ISZ_ILDOUBLE;
            break;
        case BasicType::pointer_:
            if (isarray(tp) && basetype(tp)->msil)
            {
                rv = ISZ_OBJECT;
                break;
            }
        case BasicType::func_:
        case BasicType::ifunc_:
        case BasicType::lref_:
        case BasicType::rref_:
        case BasicType::memberptr_:
        case BasicType::aggregate_:
            rv = ISZ_ADDR;
            break;
        case BasicType::string_:
            rv = ISZ_STRING;
            break;
        case BasicType::object_:
            rv = ISZ_OBJECT;
            break;
        default:
            diag("sizeFromType error");
            break;
    }
    return rv;
}
void cast(TYPE* tp, EXPRESSION** exp)
{
    ExpressionNode en = ExpressionNode::x_i_;
    tp = basetype(tp);
    switch (tp->type == BasicType::enum_ ? tp->btp->type : tp->type)
    {
        case BasicType::lref_:
        case BasicType::rref_:
            en = ExpressionNode::x_p_;
            break;
        case BasicType::func_:
        case BasicType::ifunc_:
            en = ExpressionNode::x_p_;
            break;
        case BasicType::bit_:
            en = ExpressionNode::x_bit_;
            break;
        case BasicType::bool_:
            en = ExpressionNode::x_bool_;
            break;
        case BasicType::char_:
            if (Optimizer::cparams.prm_charisunsigned)
                en = ExpressionNode::x_uc_;
            else
                en = ExpressionNode::x_c_;
            break;
        case BasicType::signed_char_:
            en = ExpressionNode::x_c_;
            break;
        case BasicType::char8_t_:
        case BasicType::unsigned_char_:
            en = ExpressionNode::x_uc_;
            break;
        case BasicType::char16_t_:
            en = ExpressionNode::x_u16_;
            break;
        case BasicType::char32_t_:
            en = ExpressionNode::x_u32_;
            break;
        case BasicType::short_:
            en = ExpressionNode::x_s_;
            break;
        case BasicType::unsigned_short_:
            en = ExpressionNode::x_us_;
            break;
        case BasicType::wchar_t_:
            en = ExpressionNode::x_wc_;
            break;
        case BasicType::int_:
            en = ExpressionNode::x_i_;
            break;
        case BasicType::inative_:
            en = ExpressionNode::x_inative_;
            break;
        case BasicType::unsigned_:
            en = ExpressionNode::x_ui_;
            break;
        case BasicType::unative_:
            en = ExpressionNode::x_unative_;
            break;
        case BasicType::long_:
            en = ExpressionNode::x_l_;
            break;
        case BasicType::unsigned_long_:
            en = ExpressionNode::x_ul_;
            break;
        case BasicType::long_long_:
            en = ExpressionNode::x_ll_;
            break;
        case BasicType::unsigned_long_long_:
            en = ExpressionNode::x_ull_;
            break;
        case BasicType::bitint_:
            en = ExpressionNode::x_bitint_;
            break;
        case BasicType::unsigned_bitint_:
            en = ExpressionNode::x_ubitint_;
            break;
        case BasicType::float_:
            en = ExpressionNode::x_f_;
            break;
        case BasicType::double_:
            en = ExpressionNode::x_d_;
            break;
        case BasicType::long_double_:
            en = ExpressionNode::x_ld_;
            break;
        case BasicType::float__complex_:
            en = ExpressionNode::x_fc_;
            break;
        case BasicType::double__complex_:
            en = ExpressionNode::x_dc_;
            break;
        case BasicType::long_double_complex_:
            en = ExpressionNode::x_ldc_;
            break;
        case BasicType::float__imaginary_:
            en = ExpressionNode::x_fi_;
            break;
        case BasicType::double__imaginary_:
            en = ExpressionNode::x_di_;
            break;
        case BasicType::long_double_imaginary_:
            en = ExpressionNode::x_ldi_;
            break;
        case BasicType::string_:
            en = ExpressionNode::x_string_;
            break;
        case BasicType::object_:
            en = ExpressionNode::x_object_;
            break;
        case BasicType::pointer_:
        case BasicType::aggregate_:
            en = ExpressionNode::x_p_;
            break;
        case BasicType::void_:
            return;
        case BasicType::templateparam_:
        case BasicType::templateselector_:
        case BasicType::templatedecltype_:
            return;
        default:
            diag("cast error");
            break;
    }
    *exp = exprNode(en, *exp, nullptr);
    (*exp)->v.b.bits = tp->bitintbits;
}
bool castvalue(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::x_bit_:
        case ExpressionNode::x_bool_:
        case ExpressionNode::x_wc_:
        case ExpressionNode::x_c_:
        case ExpressionNode::x_uc_:
        case ExpressionNode::x_u16_:
        case ExpressionNode::x_u32_:
        case ExpressionNode::x_s_:
        case ExpressionNode::x_us_:
        case ExpressionNode::x_i_:
        case ExpressionNode::x_ui_:
        case ExpressionNode::x_inative_:
        case ExpressionNode::x_unative_:
        case ExpressionNode::x_l_:
        case ExpressionNode::x_ul_:
        case ExpressionNode::x_ll_:
        case ExpressionNode::x_ull_:
        case ExpressionNode::x_bitint_:
        case ExpressionNode::x_ubitint_:
        case ExpressionNode::x_f_:
        case ExpressionNode::x_d_:
        case ExpressionNode::x_ld_:
        case ExpressionNode::x_fc_:
        case ExpressionNode::x_dc_:
        case ExpressionNode::x_ldc_:
        case ExpressionNode::x_fi_:
        case ExpressionNode::x_di_:
        case ExpressionNode::x_ldi_:
        case ExpressionNode::x_p_:
        case ExpressionNode::x_string_:
        case ExpressionNode::x_object_:
            return true;
        default:
            return false;
    }
}
bool xvalue(EXPRESSION* exp)
{
    // fixme...
    return false;
}
bool lvalue(EXPRESSION* exp)
{
    if (!Optimizer::cparams.prm_ansi)
        while (castvalue(exp))
            exp = exp->left;
    switch (exp->type)
    {
        case ExpressionNode::lvalue_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_s_:
        case ExpressionNode::l_us_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_bitint_:
        case ExpressionNode::l_ubitint_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_fi_:

        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_p_:
        case ExpressionNode::l_string_:
        case ExpressionNode::l_object_:
            return true;
        case ExpressionNode::l_ref_:
            return true;
        default:
            return false;
    }
}
static EXPRESSION* msilThunkSubStructs(EXPRESSION* exps, EXPRESSION* expsym, SYMBOL* fieldsp, int offset)
{
    if (fieldsp->sb->parentClass->sb->parentClass)
    {
        bool done = false;
        TYPE* tp = expsym->v.sp->tp;
        while (!done)
        {
            while (ispointer(tp))
            {
                tp = tp->btp;
            }
            offset %= tp->size;  // in case of array
            if (isstructured(tp))
            {
                for (auto sp : * basetype(tp)->syms)
                    if (offset >= sp->sb->offset && offset < sp->sb->offset + sp->tp->size)
                    {
                        if (ismemberdata(sp))
                        {
                            if (isstructured(sp->tp))
                            {
                                offset -= sp->sb->offset;
                                exps = exprNode(ExpressionNode::structadd_, exps, varNode(ExpressionNode::structelem_, sp));
                                tp = sp->tp;
                            }
                            else
                            {
                                done = true;
                            }
                            break;
                        }
                    }
            }
            else
            {
                done = true;
            }
        }
    }
    return exps;
}
EXPRESSION* convertInitToExpression(TYPE* tp, SYMBOL* sym, EXPRESSION* expsym, SYMBOL* funcsp, std::list<INITIALIZER*>* init,
                                    EXPRESSION* thisptr, bool isdest)
{
    bool local = false;
    EXPRESSION *rv = nullptr, **pos = &rv;
    EXPRESSION *exp = nullptr, **expp;
    EXPRESSION *expsymin = expsym, *base;
    bool noClear = false;
    if (sym)
        sym->sb->destructed = false;

    if (isstructured(tp) || isarray(tp))
    {
        initInsert(&init, nullptr, nullptr, tp->size, false);
    }
    if (!expsym)
    {
        if (!sym)
        {
            if (thisptr)
                expsym = thisptr;
            else if (funcsp)
            {
                SYMBOL* sym =
                    basetype(funcsp->tp)->syms->size() > 0 ? (SYMBOL*)basetype(funcsp->tp)->syms->front() : nullptr;
                if (sym && sym->sb->thisPtr)
                    expsym = varNode(ExpressionNode::auto_, sym);  // this ptr
                else
                    expsym = anonymousVar(StorageClass::auto_, tp);
            }
            else  // global initialization, make a temporary variable
            {
                char sanon[256];
                sprintf(sanon, "sanon_%d", staticanonymousIndex++);
                // this is probably for a structured cast that is being converted to a pointer with ampersand...
                SYMBOL* sym = makeID(StorageClass::static_, tp, nullptr, sanon);
                SetLinkerNames(sym, Linkage::cdecl_);
                insertDynamicInitializer(sym, init);
                expsym = varNode(ExpressionNode::global_, sym);
                insertInitSym(sym);
            }
        }
        else
        {
            switch (sym->sb->storage_class)
            {
            case StorageClass::auto_:
            case StorageClass::register_:
            case StorageClass::parameter_:
                local = true;
                expsym = varNode(ExpressionNode::auto_, sym);
                break;
            case StorageClass::localstatic_:
                if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                {
                    expsym = thisptr;
                }
                else
                {
                    local = true;
                    expsym = varNode(ExpressionNode::global_, sym);
                }
                break;
            case StorageClass::static_:
            case StorageClass::global_:
                if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                {
                    expsym = thisptr;
                }
                else
                {
                    local = true;
                    expsym = varNode(ExpressionNode::global_, sym);
                }
                break;
            case StorageClass::member_:
            case StorageClass::mutable_:
                if (thisptr)
                    expsym = thisptr;
                else if (funcsp)
                    expsym = varNode(ExpressionNode::auto_, (SYMBOL*)basetype(funcsp->tp)->syms->front());  // this ptr
                else
                {
                    expsym = intNode(ExpressionNode::c_i_, 0);
                    diag("convertInitToExpression: no this ptr");
                }
                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    expsym = exprNode(ExpressionNode::structadd_, expsym, varNode(ExpressionNode::structelem_, sym));
                else
                    expsym = exprNode(ExpressionNode::structadd_, expsym, intNode(ExpressionNode::c_i_, sym->sb->offset));
                break;
            case StorageClass::external_:
                /*			expsym = varNode(ExpressionNode::global_, sym);
                            local = true;
                            break;
                */
            case StorageClass::constant_:
                return nullptr;
            default:
                diag("convertInitToExpression: unknown sym type");
                expsym = intNode(ExpressionNode::c_i_, 0);
                break;
            }
        }
    }
    base = copy_expression(expsym);
    if (sym && isarray(sym->tp) && sym->tp->msil && !init->front()->noassign)
    {
        exp = intNode(ExpressionNode::msil_array_init_, 0);
        exp->v.tp = sym->tp;
        // plop in a newarr call
        *pos = exprNode(ExpressionNode::assign_, expsym, exp);
        noClear = true;
    }
    if (sym && isstructured(sym->tp) && basetype(sym->tp)->sp->sb->structuredAliasType)
    {
        noClear = true;
    }
    for (auto it = init->begin(); it != init->end(); ++it)
    {
        auto initItem = *it;
        exp = nullptr;
        if (initItem->basetp)
        {
            if (initItem->noassign && initItem->exp)
            {
                exp = initItem->exp;
                if (exp->type == ExpressionNode::thisref_)
                    exp = exp->left;
                if (thisptr && exp->type == ExpressionNode::func_)
                {
                    EXPRESSION* exp1 = initItem->offset || (Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND)
                        ? exprNode(ExpressionNode::add_, copy_expression(expsym), intNode(ExpressionNode::c_i_, initItem->offset))
                        : copy_expression(expsym);
                    if (isarray(tp))
                    {
                        exp->v.func->arguments->front()->exp = exp1;
                    }
                    else
                    {
                        exp->v.func->thisptr = exp1;
                    }
                    GetAssignDestructors(&exp->v.func->destructors, exp);
                    exp = initItem->exp;
                }
                else
                {
                    if (exp->type == ExpressionNode::func_)
                        GetAssignDestructors(&exp->v.func->destructors, exp);
                    exp = initItem->exp;
                }
            }
            else if (!initItem->exp)
            {
                // usually empty braces, coudl be an error though
                exp = exprNode(ExpressionNode::blockclear_, copy_expression(expsym), nullptr);
                exp->size = MakeType(BasicType::struct_);
                exp->size->size = initItem->offset;                
            }
            else if (isstructured(initItem->basetp) || isarray(initItem->basetp))
            {
                if (isstructured(initItem->basetp))
                {
                    if (0 && basetype(initItem->basetp)->sp->sb->structuredAliasType)
                    {
                        exp = copy_expression(expsym);
                        if (initItem->offset)
                            exp = exprNode(ExpressionNode::add_, exp, intNode(ExpressionNode::c_i_, initItem->offset));
                        deref(basetype(initItem->basetp)->sp->sb->structuredAliasType, &exp);
                        exp = exprNode(ExpressionNode::assign_, exp, initItem->exp);
                        noClear = true;
                    }
                    else
                    {
                        EXPRESSION* exp2 = initItem->exp;
                        while (exp2->type == ExpressionNode::not__lvalue_)
                            exp2 = exp2->left;
                        if (exp2->type == ExpressionNode::func_ && exp2->v.func->returnSP)
                        {
                            exp2->v.func->returnSP->sb->allocate = false;
                            exp2->v.func->returnEXP = copy_expression(expsym);
                            exp = exp2;
                            noClear = true;
                        }
                        else if (exp2->type == ExpressionNode::thisref_ && exp2->left->v.func->returnSP)
                        {
                            exp2->left->v.func->returnSP->sb->allocate = false;
                            exp2->left->v.func->returnEXP = copy_expression(expsym);
                            exp = exp2;
                            noClear = true;
                        }
                        else if ((Optimizer::cparams.prm_cplusplus) && !basetype(initItem->basetp)->sp->sb->trivialCons)
                        {
                            TYPE* ctype = initItem->basetp;
                            callConstructorParam(&ctype, &expsym, ctype, exp2, true, false, false, false, true);
                            exp = expsym;
                        }
                        else
                        {
                            exp = copy_expression(expsym);
                            if (initItem->offset)
                                exp = exprNode(ExpressionNode::add_, exp, intNode(ExpressionNode::c_i_, initItem->offset));
                            exp = exprNode(ExpressionNode::blockassign_, exp, exp2);
                            exp->size = initItem->basetp;
                            exp->altdata = (void*)(initItem->basetp);
                            noClear = comparetypes(initItem->basetp, tp, true);
                        }
                    }
                }
                else
                {
                    TYPE* btp = initItem->basetp;
                    while (isarray(btp))
                        btp = basetype(btp)->btp;
                    btp = basetype(btp);
                    bool found = false;
                    for (auto elm : *init)
                    {
                        if (elm->exp)
                            if (!isarithmeticconst(elm->exp) && !isconstaddress(elm->exp))
                            {
                                found = true;
                                break;
                            }
                    }
                    if (found)
                    {
                        /* some members are non-constant expressions */
                        if (!Optimizer::cparams.prm_cplusplus)
                            RequiresDialect::Feature(Dialect::c99, "Field initialization with non-constant");
                        if (!sym)
                        {
                            expsym = anonymousVar(StorageClass::auto_, initItem->basetp);
                            sym = expsym->v.sp;
                        }
                        if (!isstructured(btp) || btp->sp->sb->trivialCons)
                        {
                            exp = exprNode(ExpressionNode::blockclear_, copy_expression(expsym), nullptr);
                            exp->size = initItem->basetp;
                            exp = exprNode(ExpressionNode::comma_, exp, nullptr);
                            expp = &exp->right;
                        }
                        else
                        {
                            expp = &exp;
                        }
                        {
                            EXPRESSION* right = initItem->exp;
                            if (!isstructured(btp))
                            {
                                EXPRESSION* asn;
                                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                                {
                                    int n = initItem->offset / btp->size;
                                    asn = exprNode(ExpressionNode::sizeof_, typeNode(btp), nullptr);
                                    EXPRESSION* exp4 = intNode(ExpressionNode::c_i_, n);
                                    asn = exprNode(ExpressionNode::umul_, exp4, asn);
                                }
                                else
                                {
                                    asn = exprNode(ExpressionNode::add_, copy_expression(expsym), intNode(ExpressionNode::c_i_, initItem->offset));
                                }
                                deref(initItem->basetp, &asn);
                                cast(initItem->basetp, &right);
                                right = exprNode(ExpressionNode::assign_, asn, right);
                            }
                            if (*expp)
                                *expp = exprNode(ExpressionNode::comma_, *expp, right);
                            else
                                *expp = right;
                            expp = &(*expp)->right;
                        }
                    }
                    else
                    {
                        /* constant expression */
                        SYMBOL* spc;
                        exp = anonymousVar(StorageClass::localstatic_, initItem->basetp);
                        spc = exp->v.sp;
                        if (!spc->sb->init)
                            spc->sb->init = initListFactory.CreateList();
                        else
                            spc->sb->init->clear();
                        spc->sb->init->push_back(initItem);
                        insertInitSym(spc);
                        localNameSpace->front()->syms->Add(spc);
                        spc->sb->label = Optimizer::nextLabel++;
                        if (expsym)
                        {
                            if (Optimizer::cparams.prm_cplusplus && isstructured(initItem->basetp) &&
                                !initItem->basetp->sp->sb->trivialCons)
                            {
                                TYPE* ctype = initItem->basetp;
                                callConstructorParam(&ctype, &expsym, ctype, exp, true, false, false, false, true);
                                exp = expsym;
                            }
                            else
                            {
                                exp = exprNode(ExpressionNode::blockassign_, copy_expression(expsym), exp);
                                exp->size = initItem->basetp;
                                exp->altdata = (void*)(initItem->basetp);
                            }
                        }
                    }
                }
            }
            else if (basetype(initItem->basetp)->type == BasicType::memberptr_)
            {
                EXPRESSION* exp2 = initItem->exp;
                ;
                while (exp2->type == ExpressionNode::not__lvalue_)
                    exp2 = exp2->left;
                if (exp2->type == ExpressionNode::func_ && exp2->v.func->returnSP)
                {
                    exp2->v.func->returnSP->sb->allocate = false;
                    exp2->v.func->returnEXP = copy_expression(expsym);
                    exp = exp2;
                }
                else
                {
                    if (exp2->type == ExpressionNode::memberptr_)
                    {
                        int lab = dumpMemberPtr(exp2->v.sp, initItem->basetp, true);
                        exp2 = intNode(ExpressionNode::labcon_, lab);
                    }
                    exp = exprNode(ExpressionNode::blockassign_, copy_expression(expsym), exp2);
                    exp->size = initItem->basetp;
                    exp->altdata = (void*)(initItem->basetp);
                }
            }
            else
            {
                exp = initItem->exp;
                EXPRESSION* exps = copy_expression(expsym);
                exps->init = true;
                if (isarray(tp) && tp->msil)
                {
                    TYPE* btp = tp;
                    exps = exprNode(ExpressionNode::msil_array_access_, nullptr, nullptr);
                    int count = 0, i;
                    int q = initItem->offset;
                    while (isarray(btp) && btp->msil)
                    {
                        count++;
                        btp = btp->btp;
                    }
                    exps->v.msilArray = (MSIL_ARRAY*)Alloc(sizeof(MSIL_ARRAY) + count * sizeof(EXPRESSION*));
                    exps->v.msilArray->max = count;
                    exps->v.msilArray->count = count;
                    exps->v.msilArray->base = copy_expression(expsym);
                    exps->v.msilArray->tp = tp;
                    btp = tp->btp;
                    for (i = 0; i < count; i++)
                    {
                        int n = q / btp->size;
                        exps->v.msilArray->indices[i] = intNode(ExpressionNode::c_i_, n);
                        q = q - n * btp->size;

                        btp = btp->btp;
                    }
                }
                else if ((Optimizer::architecture == ARCHITECTURE_MSIL) && initItem->fieldsp)
                {
                    if (initItem->fieldoffs)
                    {
                        exps = exprNode(ExpressionNode::add_, exps, initItem->fieldoffs);
                    }
                    exps = msilThunkSubStructs(exps, expsym, initItem->fieldsp, initItem->offset);
                    exps = exprNode(ExpressionNode::structadd_, exps, varNode(ExpressionNode::structelem_, initItem->fieldsp));
                }
                else
                {
                    if (initItem->basetp->bits)
                    {
                        auto it1 = it;
                        exp = exprNode(ExpressionNode::and_, exp, intNode(ExpressionNode::c_ui_, Optimizer::mod_mask((*it)->basetp->bits)));
                        if ((*it)->basetp->startbit)
                            exp = exprNode(ExpressionNode::lsh_, exp, intNode(ExpressionNode::c_i_, (*it)->basetp->startbit));
                        while (true)
                        {
                            ++it1;
                            if (it1 == init->end() || !(*it1)->exp || !(*it1)->basetp || !(*it1)->basetp->bits || (*it1)->offset != initItem->offset)
                            {
                                break;
                            }
			    auto exp1 = (*it1)->exp;
                            exp1 = exprNode(ExpressionNode::and_, exp1, intNode(ExpressionNode::c_ui_, Optimizer::mod_mask((*it1)->basetp->bits)));
                            if ((*it1)->basetp->startbit)
                                exp1 = exprNode(ExpressionNode::lsh_, exp1, intNode(ExpressionNode::c_i_, (*it1)->basetp->startbit));
                            exp = exprNode(ExpressionNode::or_, exp, exp1);
                            ++it;                            
                        }
                    }
                    if (initItem->offset || initItem != init->back())
                    {
                        std::list<INITIALIZER*>::iterator last;
                        for (auto itx = init->end(); itx != init->begin();)
                        {
                            --itx;
                            last = itx;
                            if (*itx == initItem)
                                break;
                        }
                        if (initItem->offset ||
                            (last != init->end() && (*last)->basetp && (Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND)))
                        {
                            exps = exprNode(ExpressionNode::add_, exps, intNode(ExpressionNode::c_i_, initItem->offset));
                            exps->init = true;
                        }
                    }
                }
                if (exps->type != ExpressionNode::msil_array_access_)
                    deref(initItem->basetp, &exps);
                optimize_for_constants(&exps);
                if (exp->type == ExpressionNode::comma_)
                {
                    cast(initItem->basetp, &exp->right);
                    if (expsym)
                    {
                        exp->right = exprNode(ExpressionNode::assign_, exps, exp->right);
                        // unallocated var for destructor
                        GetAssignDestructors(&exp->right->v.logicaldestructors.left, exp->right);
                    }
                }
                else
                {
                    if ((isarithmetic(initItem->basetp) || ispointer(initItem->basetp))
                        && !isbitint(initItem->basetp))
                        cast(initItem->basetp, &exp);
                    if (exps)
                    {
                        exp = exprNode(ExpressionNode::assign_, exps, exp);
                        // unallocated var for destructor
                        GetAssignDestructors(&exp->v.logicaldestructors.left, exp);
                    }
                }
            }
            if (sym && sym->sb->init && isatomic(initItem->basetp) && needsAtomicLockFromType(initItem->basetp))
            {
                EXPRESSION* p1 = exprNode(ExpressionNode::add_, expsym->left, intNode(ExpressionNode::c_i_, initItem->basetp->size - ATOMIC_FLAG_SPACE));
                deref(&stdint, &p1);
                p1 = exprNode(ExpressionNode::assign_, p1, intNode(ExpressionNode::c_i_, 0));
                exp = exprNode(ExpressionNode::comma_, exp, p1);
            }
        }
        if (exp)
        {
            if (*pos)
            {
                *pos = exprNode(ExpressionNode::comma_, *pos, exp);
                pos = &(*pos)->right;
            }
            else
            {
                *pos = exp;
            }
        }
    }
    // plop in a clear block if necessary
    // this needs some work, if the structure was entirely created we shouldn't do this...
    if ((sym || expsymin) && !noClear && !isdest &&
        ((isarray(tp) && (!isstructured(basetype(tp)->btp) || !basetype(basetype(tp)->btp)->sp->sb->hasUserCons)) ||
         (isstructured(tp) && ((!Optimizer::cparams.prm_cplusplus && (Optimizer::architecture != ARCHITECTURE_MSIL)) ||
                               !basetype(tp)->sp->sb->hasUserCons))))
    {
        EXPRESSION* fexp = base;
        EXPRESSION* exp;
        optimize_for_constants(&fexp);
        if (fexp->type == ExpressionNode::thisref_)
            fexp = fexp->left->v.func->thisptr;
        exp = exprNode(ExpressionNode::blockclear_, fexp, nullptr);
        exp->size = tp;
        rv = exprNode(ExpressionNode::comma_, exp, rv);
    }
    if (sym && sym->sb->storage_class == StorageClass::localstatic_ && !(Optimizer::architecture == ARCHITECTURE_MSIL))
    {
        SYMBOL* guardfunc = namespacesearch("__static_guard", globalNameSpace, false, false);
        if (guardfunc)
        {
            guardfunc = guardfunc->tp->syms->front();
            EXPRESSION* guard = anonymousVar(StorageClass::localstatic_, &stdint);
            insertInitSym(guard->v.sp);
            deref(&stdpointer, &guard);
            optimize_for_constants(&rv);
            rv = destructLocal(rv);
            rv = addLocalDestructor(rv, sym);
            EXPRESSION* guardexp = exprNode(ExpressionNode::func_, nullptr, nullptr);
            guardexp->v.func = Allocate<FUNCTIONCALL>();
            guardexp->v.func->sp = guardfunc;
            guardexp->v.func->functp = guardfunc->tp;
            guardexp->v.func->fcall = varNode(ExpressionNode::pc_, guardfunc);
            guardexp->v.func->ascall = true;
            guardexp->v.func->arguments = initListListFactory.CreateList();
            auto arg = Allocate<INITLIST>();
            arg->tp = &stdpointer;
            arg->exp = guard->left;
            guardexp->v.func->arguments->push_back(arg);
            rv = exprNode(ExpressionNode::check_nz_,
                          exprNode(ExpressionNode::comma_,
                                   exprNode(ExpressionNode::land_, exprNode(ExpressionNode::ne_, guard, intNode(ExpressionNode::c_i_, -1)),
                                            exprNode(ExpressionNode::ne_, guardexp, intNode(ExpressionNode::c_i_, 0))),
                                   exprNode(ExpressionNode::comma_, rv, exprNode(ExpressionNode::assign_, guard, intNode(ExpressionNode::c_i_, -1)))),
                          intNode(ExpressionNode::c_i_, 0));
        }
    }
    if (isstructured(tp))
    {
        if (*pos)
        {
            *pos = exprNode(ExpressionNode::comma_, *pos, expsym);
            pos = &(*pos)->right;
        }
        else
        {
            *pos = expsym;
        }
    }
    if (!rv)
        rv = intNode(ExpressionNode::c_i_, 0);

    return rv;
}
bool assignDiscardsConst(TYPE* dest, TYPE* source)
{
    source = basetype(source);
    dest = basetype(dest);
    if (!ispointer(source) || !ispointer(dest))
        return false;
    while (true)
    {
        bool destc = false;
        bool sourcc = false;
        bool done = false;
        while (!done)
        {
            switch (dest->type)
            {
                case BasicType::const_:
                    destc = true;
                case BasicType::va_list_:
                case BasicType::objectArray_:
                case BasicType::restrict_:
                case BasicType::volatile_:
                case BasicType::static_:
                case BasicType::typedef_:
                case BasicType::lrqual_:
                case BasicType::rrqual_:
                case BasicType::derivedfromtemplate_:
                    dest = dest->btp;
                    break;
                default:
                    done = true;
                    break;
            }
        }
        done = false;
        while (!done)
        {
            switch (source->type)
            {
                case BasicType::const_:
                    sourcc = true;
                case BasicType::va_list_:
                case BasicType::objectArray_:
                case BasicType::restrict_:
                case BasicType::volatile_:
                case BasicType::static_:
                case BasicType::typedef_:
                case BasicType::lrqual_:
                case BasicType::rrqual_:
                    source = source->btp;
                    break;
                default:
                    done = true;
                    break;
            }
        }
        if (sourcc && !destc)
            return true;
        if (source->type != BasicType::pointer_ || dest->type != BasicType::pointer_)
            return false;
        dest = dest->btp;
        source = source->btp;
    }
}
bool isconstzero(TYPE* tp, EXPRESSION* exp)
{
    (void)tp;
    return (isintconst(exp) && exp->v.i == 0);
}
bool fittedConst(TYPE* tp, EXPRESSION* exp)
{
    int n;
    if (!isint(tp) || !isintconst(exp))
        return false;
    n = getSize(basetype(tp)->type);
    switch (n)
    {
        case 8:
            if (isunsigned(tp))
            {
                if (exp->v.i < 0 || exp->v.i > 255)
                    return false;
            }
            else
            {
                if (exp->v.i < -128 || exp->v.i > 127)
                    return false;
            }
            break;
        case 16:
            if (isunsigned(tp))
            {
                if (exp->v.i < 0 || exp->v.i > 65535)
                    return false;
            }
            else
            {
                if (exp->v.i < -32768 || exp->v.i > 32767)
                    return false;
            }
            break;
        default:
            return true;
    }
    return false;
}
bool isarithmeticconst(EXPRESSION* exp)
{
    return isintconst(exp) || isfloatconst(exp) || isimaginaryconst(exp) || iscomplexconst(exp);
}
bool isconstaddress(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::add_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::structadd_:
            return (isconstaddress(exp->left) || isintconst(exp->left)) && (isconstaddress(exp->right) || isintconst(exp->right));
        case ExpressionNode::global_:
        case ExpressionNode::pc_:
        case ExpressionNode::labcon_:
            return true;
        case ExpressionNode::func_:
            return !exp->v.func->ascall;
        case ExpressionNode::threadlocal_:
        default:
            return false;
    }
}
SYMBOL*(CopySymbol)(SYMBOL* sym_in, bool full)
{
    SYMBOL* rv = nzAllocate<SYMBOL>();
    *rv = *sym_in;
    if (rv->sb)
    {
        if (full)
        {
            rv->sb = nzAllocate<sym::_symbody>();
            *rv->sb = *sym_in->sb;
        }
        rv->sb->symRef = nullptr;
    }
    return rv;
}
static TYPE* inttype(BasicType t1)
{
    switch (t1)
    {
        case BasicType::char_:
            return &stdchar;
        case BasicType::unsigned_char_:
            return &stdunsignedchar;
        case BasicType::signed_char_:
            return &stdsignedchar;
        case BasicType::short_:
            return &stdshort;
        case BasicType::unsigned_short_:
            return &stdunsignedshort;
        case BasicType::wchar_t_:
            return &stdwidechar;
        default:
        case BasicType::int_:
        case BasicType::inative_:
            return &stdint;
        case BasicType::char8_t_:
            return &stdchar8_t;
            break;
        case BasicType::char16_t_:
            return &stdchar16t;
        case BasicType::char32_t_:
            return &stdchar32t;
        case BasicType::unsigned_:
        case BasicType::unative_:
            return &stdunsigned;
        case BasicType::long_:
            return &stdlong;
        case BasicType::unsigned_long_:
            return &stdunsignedlong;
        case BasicType::long_long_:
            return &stdlonglong;
        case BasicType::unsigned_long_long_:
            return &stdunsignedlonglong;
    }
}
inline BasicType btmax(BasicType left, BasicType right) { return left > right ? left : right; }
TYPE* destSize(TYPE* tp1, TYPE* tp2, EXPRESSION** exp1, EXPRESSION** exp2, bool minimizeInt, TYPE* atp)
/*
 * compare two types and determine if they are compatible for purposes
 * of the current operation.  Return an appropriate type.  Also checks for
 * dangerous pointer conversions...
 */
{
    int isctp1, isctp2;
    if (tp1->type == BasicType::any_)
        return tp1;
    if (tp2->type == BasicType::any_)
        return tp2;
    if (isvoid(tp1) || isvoid(tp2) || ismsil(tp1) || ismsil(tp2))
    {
        if (exp1 && exp2)
            error(ERR_NOT_AN_ALLOWED_TYPE);
        return tp1;
    }
    if (isref(tp1))
        tp1 = basetype(tp1)->btp;
    if (isref(tp2))
        tp2 = basetype(tp2)->btp;
    tp1 = basetype(tp1);
    tp2 = basetype(tp2);
    isctp1 = isarithmetic(tp1) || tp1->type == BasicType::enum_;
    isctp2 = isarithmetic(tp2) || tp2->type == BasicType::enum_;

    /*    if (isctp1 && isctp2 && tp1->type == tp2->type)
            return tp1 ;
    */
    if (tp1->type >= BasicType::float_ || tp2->type >= BasicType::float_)
    {
        TYPE* tp = nullptr;
        int isim1 = tp1->type >= BasicType::float__imaginary_ && tp1->type <= BasicType::long_double_imaginary_;
        int isim2 = tp2->type >= BasicType::float__imaginary_ && tp2->type <= BasicType::long_double_imaginary_;
        int iscx1 = tp1->type >= BasicType::float__complex_ && tp1->type <= BasicType::long_double_complex_;
        int iscx2 = tp2->type >= BasicType::float__complex_ && tp2->type <= BasicType::long_double_complex_;
        if (iscx1)
        {
            if (iscx2)
            {
                if (tp1->type > tp2->type)
                    tp = tp1;
                else
                    tp = tp2;
            }
            else if (isim2)
            {
                if ((int)tp1->type - (int)BasicType::float__complex_ >= (int)tp2->type - (int)BasicType::float__imaginary_)
                    tp = tp1;
                else
                    tp = &stddoublecomplex;
            }
            else if (tp2->type >= BasicType::float_)
            {
                if ((int)tp1->type - (int)BasicType::float__complex_ >= (int)tp2->type - (int)BasicType::float_)
                    tp = tp1;
                else
                    tp = &stddoublecomplex;
            }
            else
            {
                tp = tp1;
            }
        }
        else if (iscx2)
        {
            if (iscx1)
            {
                if (tp1->type > tp2->type)
                    tp = tp1;
                else
                    tp = tp2;
            }
            else if (isim1)
            {
                if ((int)tp1->type - (int)BasicType::float__imaginary_ <= (int)tp2->type - (int)BasicType::float__complex_)
                    tp = tp2;
                else
                    tp = &stddoublecomplex;
            }
            else if (tp2->type >= BasicType::float_)
            {
                if ((int)tp1->type - (int)BasicType::float_ <= (int)tp2->type - (int)BasicType::float__complex_)
                    tp = tp2;
                else
                    tp = &stddoublecomplex;
            }
            else
            {
                tp = tp2;
            }
        }
        else if (isim1)
        {
            if (isim2)
            {
                if (tp1->type > tp2->type)
                    tp = tp1;
                else
                    tp = tp2;
            }
            else if (tp2->type >= BasicType::float_)
            {
                if ((int)tp1->type - (int)BasicType::float__imaginary_ >= (int)tp2->type - (int)BasicType::float_)
                    tp = tp1;
                else
                    tp = &stddoubleimaginary;
            }
            else
            {
                tp = tp1;
            }
        }
        else if (isim2)
        {
            if (isim1)
            {
                if (tp1->type > tp2->type)
                    tp = tp1;
                else
                    tp = tp2;
            }
            else if (tp1->type >= BasicType::float_)
            {
                if ((int)tp1->type - (int)BasicType::float_ <= (int)tp2->type - (int)BasicType::float__imaginary_)
                    tp = tp2;
                else
                    tp = &stddoubleimaginary;
            }
            else
            {
                tp = tp2;
            }
        }
        else if (tp1->type >= BasicType::float_ && tp2->type >= BasicType::float_)
        {
            if (tp1->type > tp2->type)
                tp = tp1;
            else
                tp = tp2;
        }
        else if (tp1->type >= BasicType::float_)
            tp = tp1;
        else
            tp = tp2;
        if (exp1 && tp->type != tp1->type && exp1 && tp2->type != BasicType::pointer_)
            cast(tp, exp1);
        if (exp2 && tp->type != tp2->type && exp1 && tp1->type != BasicType::pointer_)
            cast(tp, exp2);
        return tp;
    }
    if (isctp1 && isctp2)
    {
        TYPE* rv;
        BasicType t1, t2;
        t1 = tp1->type;
        t2 = tp2->type;
        if (tp1->size == tp2->size)
            if (isunsigned(tp1) != isunsigned(tp2))
            {
                if (isunsigned(tp1))
                    t2 = t1;
                else
                    t1 = t2;
            }
        if (t1 == BasicType::enum_)
            t1 = BasicType::int_;
        if (t2 == BasicType::enum_)
            t2 = BasicType::int_;
        if (t1 == BasicType::wchar_t_)
            t1 = BasicType::unsigned_;
        if (t2 == BasicType::wchar_t_)
            t2 = BasicType::unsigned_;
        if (t1 < BasicType::int_ || ((t1 == BasicType::bitint_ || t1 == BasicType::unsigned_bitint_) && tp1->bitintbits < getSize(BasicType::int_) * CHAR_BIT))
            t1 = BasicType::int_;
        if (t2 < BasicType::int_ || ((t2 == BasicType::bitint_ || t2 == BasicType::unsigned_bitint_) &&
                                     tp2->bitintbits < getSize(BasicType::int_) * CHAR_BIT))
            t2 = BasicType::int_;
        t1 = btmax(t1, t2);
        if (t1 == BasicType::bitint_ || t1 == BasicType::unsigned_bitint_)
        {
            rv = MakeType(t1);
            int bits = tp1->bitintbits;
            bits = bits > tp2->bitintbits ? bits : tp2->bitintbits;
            rv->bitintbits = bits;
        }
        else
        {
            rv = inttype(t1);
        }
        if (exp1 && (rv->type != tp1->type || rv->bitintbits != tp1->bitintbits) && exp1)
            cast(rv, exp1);
        if (exp2 && (rv->type != tp2->type || rv->bitintbits != tp2->bitintbits) && exp2)
            cast(rv, exp2);
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions)
        {
            if (tp1->type == BasicType::enum_)
                return tp1;
            else if (tp2->type == BasicType::enum_)
                return tp2;
        }
        return rv;
    }
    else
    { /* have a pointer or other exceptional case*/
        if (tp1->type == BasicType::void_ && tp2->type == BasicType::void_)
            return tp1;
        if (tp1->type <= BasicType::unsigned_long_long_ && ispointer(tp2))
        {
            if (!ispointer(tp1))
                cast(tp2, exp1);
            return tp2;
        }
        if (tp2->type <= BasicType::unsigned_long_long_ && ispointer(tp1))
        {
            if (!ispointer(tp2))
                cast(tp1, exp2);
            return tp1;
        }
        if (isstructured(tp1))
        {
            return tp2;
        }
        if (isstructured(tp2))
        {
            return tp1;
        }

        if (isfunction(tp1))
            if (isfunction(tp2) || ispointer(tp2))
                return tp1;
        if (isfunction(tp2))
            if (isfunction(tp1) || ispointer(tp1))
                return tp2;
        if (ispointer(tp1))
            if (ispointer(tp2))
            {
                return tp1;
            }
    }
    return tp1;
}
EXPRESSION* RemoveAutoIncDec(EXPRESSION* exp)
{
    EXPRESSION* newExp;
    if (exp->preincdec || exp->type == ExpressionNode::auto_inc_ || exp->type == ExpressionNode::auto_dec_)
        return RemoveAutoIncDec(exp->left);
    newExp = Allocate<EXPRESSION>();
    *newExp = *exp;
    if (newExp->left)
        newExp->left = RemoveAutoIncDec(newExp->left);
    if (newExp->right)
        newExp->right = RemoveAutoIncDec(newExp->right);
    return newExp;
}
EXPRESSION* EvaluateDest(EXPRESSION*exp, TYPE* tp)
{
    EXPRESSION* result = nullptr;
    if (!basetype(tp)->hasbits && !castvalue(exp) && lvalue(exp))
    {
        bool doit = false;
        std::stack<EXPRESSION*> stk;
        stk.push(exp->left);
        while (stk.size() && !doit)
        {
            EXPRESSION* c = stk.top();
            stk.pop();
            if (c->type == ExpressionNode::func_)
            {
                doit = true;
                break;
            }
            if (c->left)
                stk.push(c->left);
            if (c->right)
                stk.push(c->right);
        }
        if (doit)
        {
            auto exp2 = exp->left;
            result = anonymousVar(StorageClass::auto_, &stdpointer);
            deref(&stdpointer, &result);
            exp2 = exprNode(ExpressionNode::assign_, result, exp2);
            exp2 = exprNode(exp->type, exp2, nullptr);
            result = exprNode(exp->type, result, nullptr);
            result = exprNode(ExpressionNode::comma_, exp2, result);
        }
    }
    return result;
}
}  // namespace Parser
