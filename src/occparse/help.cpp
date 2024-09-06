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
#include "constexpr.h"
#include "iexpr.h"

namespace Parser
{

int anonymousNotAlloc;

static int staticanonymousIndex;
static std::set<std::string> guardFuncs;

void helpinit()
{
    anonymousNotAlloc = 0;
    staticanonymousIndex = 1;
    guardFuncs.clear();
}
bool equalnode(EXPRESSION* node1, EXPRESSION* node2)
/*
 *      equalnode will return 1 if the expressions pointed to by
 *      node1 and node2 are equivalent.
 */
{
    if (node1 == 0 || node2 == 0)
        return 0;
    if (node1->type != node2->type)
        return 0;
    if (natural_size(node1) != natural_size(node2))
        return 0;
    switch (node1->type)
    {
    case ExpressionNode::const_:
    case ExpressionNode::pc_:
    case ExpressionNode::global_:
    case ExpressionNode::auto_:
    case ExpressionNode::absolute_:
    case ExpressionNode::threadlocal_:
    case ExpressionNode::structelem_:
        return node1->v.sp == node2->v.sp;
    case ExpressionNode::labcon_:
        return node1->v.i == node2->v.i;
    default:
        return (!node1->left || equalnode(node1->left, node2->left)) &&
            (!node1->right || equalnode(node1->right, node2->right));
    case ExpressionNode::c_i_:
    case ExpressionNode::c_l_:
    case ExpressionNode::c_ul_:
    case ExpressionNode::c_ui_:
    case ExpressionNode::c_c_:
    case ExpressionNode::c_u16_:
    case ExpressionNode::c_u32_:
    case ExpressionNode::c_bool_:
    case ExpressionNode::c_uc_:
    case ExpressionNode::c_ll_:
    case ExpressionNode::c_ull_:
    case ExpressionNode::c_wc_:
    case ExpressionNode::nullptr_:
        return node1->v.i == node2->v.i;
    case ExpressionNode::c_d_:
    case ExpressionNode::c_f_:
    case ExpressionNode::c_ld_:
    case ExpressionNode::c_di_:
    case ExpressionNode::c_fi_:
    case ExpressionNode::c_ldi_:
        return (*node1->v.f == *node2->v.f);
    case ExpressionNode::c_dc_:
    case ExpressionNode::c_fc_:
    case ExpressionNode::c_ldc_:
        return (node1->v.c->r == node2->v.c->r) && (node1->v.c->i == node2->v.c->i);
    }
}
EXPRESSION* relptr(EXPRESSION* node, int& offset, bool add)
{
    EXPRESSION* rv;
    switch (node->type)
    {
        case ExpressionNode::global_:
        case ExpressionNode::auto_:
        case ExpressionNode::threadlocal_:
        case ExpressionNode::cvarpointer_:
            return node;
        case ExpressionNode::add_: {
        case ExpressionNode::structadd_:
            auto rv1 = relptr(node->left, offset, true);
            auto rv2 = relptr(node->right, offset, true);
            if (rv1)
                return rv1;
            else
                return rv2;
            break;
        }
        case ExpressionNode::sub_: {
            auto rv1 = rv = relptr(node->left, offset, false);
            auto rv2 = relptr(node->right, offset, false);
            if (rv1)
                return rv1;
            else
                return rv2;
            break;
        }
        default:
            if (IsCastValue(node))
            {
                return relptr(node->left, offset, add);
            }
            if (isintconst(node))
            {
                if (add)
                    offset += node->v.i;
                else
                    offset -= node->v.i;
            }
            return nullptr;
    }
    return rv;
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
bool ismemberdata(SYMBOL* sym) { return !sym->tp->IsFunction() && ismember(sym); }
bool isDerivedFromTemplate(Type* tp)
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
EXPRESSION* createTemporary(Type* tp, EXPRESSION* val)
{
    EXPRESSION* rv;
    tp = tp->BaseType()->btp;
    if (tp->type == BasicType::pointer_) // to get around arrays not doing a Dereference...
        tp = &stdpointer;
    rv = AnonymousVar(StorageClass::auto_, tp);
    if (val)
    {
        if (IsConstantExpression(val, true, true))
            rv->v.sp->sb->constexpression = true;
        EXPRESSION* rv1 = copy_expression(rv);
        Dereference(tp, &rv);
        cast(tp, &val);
        rv = MakeExpression(ExpressionNode::comma_, MakeExpression(ExpressionNode::assign_, rv, val), rv1);
    }
    errortype(ERR_CREATE_TEMPORARY, tp, tp);
    return rv;
}
EXPRESSION* msilCreateTemporary(Type* tp, EXPRESSION* val)
{
    EXPRESSION* rv = AnonymousVar(StorageClass::auto_, tp);
    if (val)
    {
        EXPRESSION* rv1 = copy_expression(rv);
        Dereference(tp, &rv);
        cast(tp, &val);
        rv = MakeExpression(ExpressionNode::comma_, MakeExpression(ExpressionNode::assign_, rv, val), rv1);
    }
    errortype(ERR_CREATE_TEMPORARY, tp, tp);
    return rv;
}
void DeduceAuto(Type** pat, Type* nt, EXPRESSION* exp)
{
    Type* patin = *pat;
    Type* in = nt;
    if ((*pat)->IsAutoType())
    {
        bool pointerOrRef = false;
        bool err = false;
        if ((*pat)->IsRef())
        {
            if ((*pat)->type == BasicType::rref_ && !(*pat)->btp->IsConst() && !(*pat)->btp->IsVolatile())
            {
                // forwarding?  unadorned rref!
                if (!nt->rref && nt->BaseType()->type != BasicType::rref_ && (!isarithmeticconst(exp) || exp->type == ExpressionNode::const_))
                {
                    // lref
                    Type* t = nt;
                    *pat = Type::MakeType(BasicType::lref_, t->IsRef() ? t->btp : t);
                    return;
                }
                else
                {
                    // rref, get rid of qualifiers and return an rref
                    Type* tp1;
                    if (nt->type == BasicType::rref_)
                        tp1 = nt->btp->BaseType();
                    else
                        tp1 = nt->BaseType();
                    // rref
                    *pat = Type::MakeType(BasicType::lref_, tp1->IsRef() ? tp1->btp : tp1);
                    return;
                }
            }
            if (nt->IsRef())
            {
                nt = nt->BaseType()->btp;
            }
            pointerOrRef = true;
            pat = &(*pat)->BaseType()->btp;
        }
        while ((*pat)->IsPtr() && nt->IsPtr())
        {
            pointerOrRef = true;
            pat = &(*pat)->BaseType()->btp;
            nt = nt->BaseType()->btp;
        }
        if ((*pat)->IsPtr())
        {
            if (nt->IsFunction())
            {
                while ((*pat)->type != BasicType::auto_)
                    pat = &(*pat)->btp;
                *pat = nt;
                return;
            }
            else if (nt->type == BasicType::aggregate_)
            {
                while ((*pat)->type != BasicType::auto_)
                    pat = &(*pat)->btp;
                LookupSingleAggregate(nt, &exp, false);
                if (exp->type == ExpressionNode::pc_)
                {
                    *pat = exp->v.sp->tp;
                }
                return;
            }
        }
        if ((*pat)->IsPtr())
            err = true;
        nt = nt->BaseType();
        if (err)
        {
            errortype(ERR_CANNOT_DEDUCE_AUTO_TYPE, patin, in);
        }
        else if (!pointerOrRef)
        {
            if ((*pat)->decltypeauto)
                if ((*pat)->decltypeautoextended)
                {
                    *pat = Type::MakeType(BasicType::lref_, nt);
                }
                else
                {
                    *pat = nt;
                }
            else if (nt->IsRef())
            {
                *pat = nt->BaseType()->btp;
            }
            else
            {
                *pat = nt;
            }
        }
        else if (in->IsPtr())
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
SYMBOL* getFunctionSP(Type** tp)
{
    Type* btp = (*tp)->BaseType();
    bool pointer = btp->IsPtr();
    if (pointer)
    {
        btp = btp->BaseType()->btp;
    }
    if (btp->IsFunction())
    {
        *tp = btp;
        return btp->BaseType()->sp;
    }
    else if (btp->type == BasicType::aggregate_)
    {
        return btp->sp;
    }
    return nullptr;
}
LexList* concatStringsInternal(LexList* lex, StringData** str, int* elems)
{
    Optimizer::SLCHAR** list;
    char* suffix = nullptr;
    int count = 3;
    int pos = 0;
    LexType type = LexType::l_astr_;
    StringData* string;
    list = Allocate<Optimizer::SLCHAR*>(count);
    while (lex && (lex->data->type == LexType::l_astr_ || lex->data->type == LexType::l_wstr_ || lex->data->type == LexType::l_ustr_ ||
                   lex->data->type == LexType::l_Ustr_ || lex->data->type == LexType::l_msilstr_ || lex->data->type == LexType::l_u8str_ ))
    {
        if (lex->data->type == LexType::l_u8str_)
            type = LexType::l_u8str_;
        else if (lex->data->type == LexType::l_msilstr_)
            type = LexType::l_msilstr_;
        else if (lex->data->type == LexType::l_Ustr_)
            type = LexType::l_Ustr_;
        else if (type != LexType::l_Ustr_ && type != LexType::l_msilstr_ && lex->data->type == LexType::l_ustr_)
            type = LexType::l_ustr_;
        else if (type != LexType::l_Ustr_ && type != LexType::l_ustr_ && type != LexType::l_msilstr_ && lex->data->type == LexType::l_wstr_)
            type = LexType::l_wstr_;
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
    string = Allocate<StringData>();
    string->strtype = type;
    string->size = pos;
    string->pointers = Allocate<Optimizer::SLCHAR*>(pos);
    string->suffix = suffix;
    memcpy(string->pointers, list, pos * sizeof(Optimizer::SLCHAR*));
    *str = string;
    return lex;
}
LexList* concatStrings(LexList* lex, EXPRESSION** expr, LexType* tp, int* elems)
{
    StringData* data;
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
void undoAnonymousVar(SYMBOL* sp)
{
    if (theCurrentFunc && localNameSpace->front()->syms && !inDefaultParam && !anonymousNotAlloc)
    {
        localNameSpace->front()->syms->remove(sp);
    }
}
EXPRESSION* AnonymousVar(StorageClass storage_class, Type* tp)
{
    static int anonct = 1;
    char buf[256];
    SYMBOL* rv = SymAlloc();
    if (tp->size == 0 && tp->IsStructured())
        tp = tp->BaseType()->sp->tp;
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
    if (!tp->IsAtomic())
        tp->size = tp->BaseType()->size;
    if (theCurrentFunc && localNameSpace->front()->syms && !inDefaultParam && !anonymousNotAlloc)
        InsertSymbol(rv, storage_class, Linkage::none_, false);
    SetLinkerNames(rv, Linkage::none_);
    return MakeExpression(storage_class == StorageClass::auto_ || storage_class == StorageClass::parameter_ ? ExpressionNode::auto_ : ExpressionNode::global_, rv);
}
EXPRESSION* anonymousBits(StorageClass storageClass, bool issigned, int bits)
{
    Type* tp = Type::MakeType(issigned ? BasicType::bitint_ : BasicType::unsigned_bitint_);
    tp->size = bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
    tp->size /= Optimizer::chosenAssembler->arch->bitintunderlying;
    tp->size *= Optimizer::chosenAssembler->arch->bitintunderlying;
    tp->size /= CHAR_BIT;
    tp->bitintbits = bits;
    return AnonymousVar(storageClass, tp);
}
void Dereference(Type* tp, EXPRESSION** exp)
    {
    ExpressionNode en = ExpressionNode::l_i_;
    tp = tp->BaseType();
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
            diag("Dereference error");
            break;
    }
    *exp = MakeExpression(en, *exp);
    if (tp->IsBitInt())
        (*exp)->v.b.bits = tp->bitintbits;
    if (en == ExpressionNode::l_object_)
        (*exp)->v.tp = tp;
}
bool TakeAddress(EXPRESSION** exp, Type* extended)
{
    bool rv = false;
    auto temp = *exp, * last = &temp;
    if (!extended)
    {
        while (IsCastValue(*last))
        {
            *last = (*last)->left;
        }
        if (IsLValue(*last))
        {
            *last = (*last)->left;
            rv = true;
        }
    }
    else
    {
        if (extended->IsRef())
            extended = extended->BaseType()->btp;
        EXPRESSION* assignmentNode = nullptr;
        while ( (*last)->right && (*last)->type == ExpressionNode::comma_)
        {
            (*last) = MakeExpression(ExpressionNode::comma_, (*last)->left, (*last)->right);
            last = &(*last)->right;
        }
        if ((*last)->type == ExpressionNode::comma_)
        {
            (*last) = (*last)->left;
        }
        while (IsCastValue(*last))
        {
            *last = (*last)->left;
        }
        if ((*last)->type == ExpressionNode::assign_)
        {
            assignmentNode = *last;
            *last = (*last)->left;
            while (IsCastValue(*last))
            {
                *last = (*last)->left;
            }
        }
        if (IsLValue(*last))
        {
            if (!extended->IsStructured() && (*last)->type != ExpressionNode::l_ref_)
                *last = (*last)->left;
            rv = true;
        }
        if (assignmentNode && extended->BaseType()->type != BasicType::memberptr_)
        {
            (*last) = MakeExpression(ExpressionNode::comma_, assignmentNode, *last);
        }
    }
    if (rv)
    {
        switch ((*last)->type)
        {
        case ExpressionNode::auto_:
            SetRuntimeData(currentLex, *last, (*last)->v.sp);
            (*last)->v.sp->sb->addressTaken = true;
            break;
        case ExpressionNode::pc_:
        case ExpressionNode::global_:
        case ExpressionNode::absolute_:
        case ExpressionNode::threadlocal_:
            (*last)->v.sp->sb->addressTaken = true;
            break;
        default:
            break;
        }
        *exp = temp;
    }
    return rv;
}
int SizeFromType(Type* tp)
{
    int rv = -ISZ_UINT;
    tp = tp->BaseType();
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
            if (tp->IsArray() && tp->BaseType()->msil)
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
        case BasicType::any_:
            rv = ISZ_ADDR;
            break;
        default:
            diag("SizeFromType error");
            break;
    }
    return rv;
}
void cast(Type* tp, EXPRESSION** exp)
{
    ExpressionNode en = ExpressionNode::x_i_;
    tp = tp->BaseType();
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
    *exp = MakeExpression(en, *exp);
    (*exp)->v.b.bits = tp->bitintbits;
}
bool IsCastValue(EXPRESSION* exp)
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
bool IsLValue(EXPRESSION* exp)
{
    if (!Optimizer::cparams.prm_ansi)
        while (IsCastValue(exp))
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
        Type* tp = expsym->v.sp->tp;
        while (!done)
        {
            while (tp->IsPtr())
            {
                tp = tp->btp;
            }
            offset %= tp->size;  // in case of array
            if (tp->IsStructured())
            {
                for (auto sp : * tp->BaseType()->syms)
                    if (offset >= sp->sb->offset && offset < sp->sb->offset + sp->tp->size)
                    {
                        if (ismemberdata(sp))
                        {
                            if (sp->tp->IsStructured())
                            {
                                offset -= sp->sb->offset;
                                exps = MakeExpression(ExpressionNode::structadd_, exps, MakeExpression(ExpressionNode::structelem_, sp));
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
EXPRESSION* ConverInitializersToExpression(Type* tp, SYMBOL* sym, EXPRESSION* expsym, SYMBOL* funcsp, std::list<Initializer*>* init,
                                    EXPRESSION* thisptr, bool isdest)
{
    bool local = false;
    EXPRESSION *rv = nullptr, **pos = &rv;
    EXPRESSION *exp = nullptr, **expp;
    EXPRESSION *expsymin = expsym, *base;
    bool noClear = false;
    if (sym)
        sym->sb->destructed = false;

    if (tp->IsStructured() || tp->IsArray())
    {
        InsertInitializer(&init, nullptr, nullptr, tp->size, false);
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
                    funcsp->tp->BaseType()->syms->size() > 0 ? (SYMBOL*)funcsp->tp->BaseType()->syms->front() : nullptr;
                if (sym && sym->sb->thisPtr)
                    expsym = MakeExpression(ExpressionNode::auto_, sym);  // this ptr
                else
                    expsym = AnonymousVar(StorageClass::auto_, tp);
            }
            else  // global initialization, make a temporary variable
            {
                char sanon[256];
                sprintf(sanon, "sanon_%d", staticanonymousIndex++);
                // this is probably for a structured cast that is being converted to a pointer with ampersand...
                SYMBOL* sym = makeID(StorageClass::static_, tp, nullptr, sanon);
                SetLinkerNames(sym, Linkage::cdecl_);
                insertDynamicInitializer(sym, init);
                expsym = MakeExpression(ExpressionNode::global_, sym);
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
                expsym = MakeExpression(ExpressionNode::auto_, sym);
                break;
            case StorageClass::localstatic_:
                if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                {
                    expsym = thisptr;
                }
                else
                {
                    local = true;
                    expsym = MakeExpression(ExpressionNode::global_, sym);
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
                    expsym = MakeExpression(ExpressionNode::global_, sym);
                }
                break;
            case StorageClass::member_:
            case StorageClass::mutable_:
                if (thisptr)
                    expsym = thisptr;
                else if (funcsp)
                    expsym = MakeExpression(ExpressionNode::auto_, (SYMBOL*)funcsp->tp->BaseType()->syms->front());  // this ptr
                else
                {
                    expsym = MakeIntExpression(ExpressionNode::c_i_, 0);
                    diag("ConverInitializersToExpression: no this ptr");
                }
                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    expsym = MakeExpression(ExpressionNode::structadd_, expsym, MakeExpression(ExpressionNode::structelem_, sym));
                else
                    expsym = MakeExpression(ExpressionNode::structadd_, expsym, MakeIntExpression(ExpressionNode::c_i_, sym->sb->offset));
                break;
            case StorageClass::external_:
                /*			expsym = MakeExpression(ExpressionNode::global_, sym);
                            local = true;
                            break;
                */
            case StorageClass::constant_:
                return nullptr;
            default:
                diag("ConverInitializersToExpression: unknown sym type");
                expsym = MakeIntExpression(ExpressionNode::c_i_, 0);
                break;
            }
        }
    }
    base = copy_expression(expsym);
    if (sym && sym->tp->IsArray() && sym->tp->msil && !init->front()->noassign)
    {
        exp = MakeIntExpression(ExpressionNode::msil_array_init_, 0);
        exp->v.tp = sym->tp;
        // plop in a newarr call
        *pos = MakeExpression(ExpressionNode::assign_, expsym, exp);
        noClear = true;
    }
    if (sym && sym->tp->IsStructured() && sym->tp->BaseType()->sp->sb->structuredAliasType)
    {
        noClear = true;
    }
    if (init)
    {
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
                    if (thisptr && exp->type == ExpressionNode::callsite_)
                    {
                        EXPRESSION* exp1 = initItem->offset || (Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND)
                            ? MakeExpression(ExpressionNode::add_, copy_expression(expsym), MakeIntExpression(ExpressionNode::c_i_, initItem->offset))
                            : copy_expression(expsym);
                        exp->v.func->thisptr = exp1;
                        /*
                        if (tp->IsArray())
                        {
                            exp->v.func->arguments->front()->exp = exp1;
                        }
                        else
                        {
                            exp->v.func->thisptr = exp1;
                        }
                        */
                        GetAssignDestructors(&exp->v.func->destructors, exp);
                        exp = initItem->exp;
                    }
                    else
                    {
                        if (exp->type == ExpressionNode::callsite_)
                            GetAssignDestructors(&exp->v.func->destructors, exp);
                        exp = initItem->exp;
                    }
                }
                else if (!initItem->exp)
                {
                    // usually empty braces, coudl be an error though
                    exp = MakeExpression(ExpressionNode::blockclear_, copy_expression(expsym));
                    exp->size = Type::MakeType(BasicType::struct_);
                    exp->size->size = initItem->offset;
                }
                else if (initItem->basetp->IsStructured() || initItem->basetp->IsArray())
                {
                    if (initItem->basetp->IsStructured())
                    {
                        EXPRESSION* exp2 = initItem->exp;
                        while (exp2->type == ExpressionNode::not__lvalue_)
                            exp2 = exp2->left;
                        if (exp2->type == ExpressionNode::callsite_ && exp2->v.func->returnSP)
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
                        else if ((Optimizer::cparams.prm_cplusplus) && !initItem->basetp->BaseType()->sp->sb->trivialCons)
                        {
                            Type* ctype = initItem->basetp;
                            callConstructorParam(&ctype, &expsym, ctype, exp2, true, false, false, false, true);
                            exp = expsym;
                        }
                        else
                        {
                            exp = copy_expression(expsym);
                            if (initItem->offset)
                                exp = MakeExpression(ExpressionNode::add_, exp, MakeIntExpression(ExpressionNode::c_i_, initItem->offset));
                            exp = MakeExpression(ExpressionNode::blockassign_, exp, exp2);
                            exp->size = initItem->basetp;
                            exp->altdata = (void*)(initItem->basetp);
                            noClear = initItem->basetp->CompatibleType(tp);
                        }
                    }
                    else
                    {
                        Type* btp = initItem->basetp;
                        while (btp->IsArray())
                            btp = btp->BaseType()->btp;
                        btp = btp->BaseType();
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
                                expsym = AnonymousVar(StorageClass::auto_, initItem->basetp);
                                sym = expsym->v.sp;
                            }
                            if (!btp->IsStructured() || btp->sp->sb->trivialCons)
                            {
                                exp = MakeExpression(ExpressionNode::blockclear_, copy_expression(expsym));
                                exp->size = initItem->basetp;
                                exp = MakeExpression(ExpressionNode::comma_, exp);
                                expp = &exp->right;
                            }
                            else
                            {
                                expp = &exp;
                            }
                            {
                                EXPRESSION* right = initItem->exp;
                                if (!btp->IsStructured())
                                {
                                    EXPRESSION* asn;
                                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                                    {
                                        int n = initItem->offset / btp->size;
                                        asn = MakeExpression(ExpressionNode::sizeof_, MakeExpression(btp));
                                        EXPRESSION* exp4 = MakeIntExpression(ExpressionNode::c_i_, n);
                                        asn = MakeExpression(ExpressionNode::umul_, exp4, asn);
                                    }
                                    else
                                    {
                                        asn = MakeExpression(ExpressionNode::add_, copy_expression(expsym), MakeIntExpression(ExpressionNode::c_i_, initItem->offset));
                                    }
                                    Dereference(initItem->basetp, &asn);
                                    cast(initItem->basetp, &right);
                                    right = MakeExpression(ExpressionNode::assign_, asn, right);
                                }
                                if (*expp)
                                    *expp = MakeExpression(ExpressionNode::comma_, *expp, right);
                                else
                                    *expp = right;
                                expp = &(*expp)->right;
                            }
                        }
                        else
                        {
                            /* constant expression */
                            SYMBOL* spc;
                            exp = AnonymousVar(StorageClass::localstatic_, initItem->basetp);
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
                                if (Optimizer::cparams.prm_cplusplus && initItem->basetp->IsStructured() &&
                                    !initItem->basetp->sp->sb->trivialCons)
                                {
                                    Type* ctype = initItem->basetp;
                                    callConstructorParam(&ctype, &expsym, ctype, exp, true, false, false, false, true);
                                    exp = expsym;
                                }
                                else
                                {
                                    exp = MakeExpression(ExpressionNode::blockassign_, copy_expression(expsym), exp);
                                    exp->size = initItem->basetp;
                                    exp->altdata = (void*)(initItem->basetp);
                                }
                            }
                        }
                    }
                }
                else if (initItem->basetp->BaseType()->type == BasicType::memberptr_)
                {
                    EXPRESSION* exp2 = initItem->exp;
                    ;
                    while (exp2->type == ExpressionNode::not__lvalue_)
                        exp2 = exp2->left;
                    if (exp2->type == ExpressionNode::callsite_ && exp2->v.func->returnSP)
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
                            exp2 = MakeIntExpression(ExpressionNode::labcon_, lab);
                        }
                        exp = MakeExpression(ExpressionNode::blockassign_, copy_expression(expsym), exp2);
                        exp->size = initItem->basetp;
                        exp->altdata = (void*)(initItem->basetp);
                    }
                }
                else
                {
                    exp = initItem->exp;
                    EXPRESSION* exps = copy_expression(expsym);
                    exps->init = true;
                    if (tp->IsArray() && tp->msil)
                    {
                        Type* btp = tp;
                        exps = MakeExpression(ExpressionNode::msil_array_access_);
                        int count = 0, i;
                        int q = initItem->offset;
                        while (btp->IsArray() && btp->msil)
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
                            exps->v.msilArray->indices[i] = MakeIntExpression(ExpressionNode::c_i_, n);
                            q = q - n * btp->size;

                            btp = btp->btp;
                        }
                    }
                    else if ((Optimizer::architecture == ARCHITECTURE_MSIL) && initItem->fieldsp)
                    {
                        if (initItem->fieldoffs)
                        {
                            exps = MakeExpression(ExpressionNode::add_, exps, initItem->fieldoffs);
                        }
                        exps = msilThunkSubStructs(exps, expsym, initItem->fieldsp, initItem->offset);
                        exps = MakeExpression(ExpressionNode::structadd_, exps, MakeExpression(ExpressionNode::structelem_, initItem->fieldsp));
                    }
                    else
                    {
                        if (initItem->basetp->bits)
                        {
                            auto it1 = it;
                            exp = MakeExpression(ExpressionNode::and_, exp, MakeIntExpression(ExpressionNode::c_ui_, Optimizer::mod_mask((*it)->basetp->bits)));
                            if ((*it)->basetp->startbit)
                                exp = MakeExpression(ExpressionNode::lsh_, exp, MakeIntExpression(ExpressionNode::c_i_, (*it)->basetp->startbit));
                            while (true)
                            {
                                ++it1;
                                if (it1 == init->end() || !(*it1)->exp || !(*it1)->basetp || !(*it1)->basetp->bits || (*it1)->offset != initItem->offset)
                                {
                                    break;
                                }
                                auto exp1 = (*it1)->exp;
                                exp1 = MakeExpression(ExpressionNode::and_, exp1, MakeIntExpression(ExpressionNode::c_ui_, Optimizer::mod_mask((*it1)->basetp->bits)));
                                if ((*it1)->basetp->startbit)
                                    exp1 = MakeExpression(ExpressionNode::lsh_, exp1, MakeIntExpression(ExpressionNode::c_i_, (*it1)->basetp->startbit));
                                exp = MakeExpression(ExpressionNode::or_, exp, exp1);
                                ++it;
                            }
                        }
                        if (initItem->offset || initItem != init->back())
                        {
                            std::list<Initializer*>::iterator last;
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
                                exps = MakeExpression(ExpressionNode::add_, exps, MakeIntExpression(ExpressionNode::c_i_, initItem->offset));
                                exps->init = true;
                            }
                        }
                    }
                    if (exps->type != ExpressionNode::msil_array_access_)
                        Dereference(initItem->basetp, &exps);
                    optimize_for_constants(&exps);
                    if (exp->type == ExpressionNode::comma_)
                    {
                        cast(initItem->basetp, &exp->right);
                        if (expsym)
                        {
                            exp->right = MakeExpression(ExpressionNode::assign_, exps, exp->right);
                            // unallocated var for destructor
                            GetAssignDestructors(&exp->right->v.logicaldestructors.left, exp->right);
                        }
                    }
                    else
                    {
                        if ((initItem->basetp->IsArithmetic() || initItem->basetp->IsPtr())
                            && !initItem->basetp->IsBitInt())
                            cast(initItem->basetp, &exp);
                        if (exps)
                        {
                            exp = MakeExpression(ExpressionNode::assign_, exps, exp);
                            // unallocated var for destructor
                            GetAssignDestructors(&exp->v.logicaldestructors.left, exp);
                        }
                    }
                }
                if (sym && sym->sb->init && initItem->basetp->IsAtomic() && needsAtomicLockFromType(initItem->basetp))
                {
                    EXPRESSION* p1 = MakeExpression(ExpressionNode::add_, expsym->left, MakeIntExpression(ExpressionNode::c_i_, initItem->basetp->size - ATOMIC_FLAG_SPACE));
                    Dereference(&stdint, &p1);
                    p1 = MakeExpression(ExpressionNode::assign_, p1, MakeIntExpression(ExpressionNode::c_i_, 0));
                    exp = MakeExpression(ExpressionNode::comma_, exp, p1);
                }
            }
            if (exp)
            {
                if (*pos)
                {
                    *pos = MakeExpression(ExpressionNode::comma_, *pos, exp);
                    pos = &(*pos)->right;
                }
                else
                {
                    *pos = exp;
                }
            }
        }
    }
    // plop in a clear block if necessary
    // this needs some work, if the structure was entirely created we shouldn't do this...
    if ((sym || expsymin) && !noClear && !isdest &&
        ((tp->IsArray() && (!tp->BaseType()->btp->IsStructured() || !tp->BaseType()->btp->BaseType()->sp->sb->hasUserCons)) ||
         (tp->IsStructured() && ((!Optimizer::cparams.prm_cplusplus && (Optimizer::architecture != ARCHITECTURE_MSIL)) ||
                               !tp->BaseType()->sp->sb->hasUserCons))))
    {
        EXPRESSION* fexp = base;
        EXPRESSION* exp;
        optimize_for_constants(&fexp);
        if (fexp->type == ExpressionNode::thisref_)
            fexp = fexp->left->v.func->thisptr;
        exp = MakeExpression(ExpressionNode::blockclear_, fexp);
        exp->size = tp;
        rv = MakeExpression(ExpressionNode::comma_, exp, rv);
    }
    if (sym && sym->sb->storage_class == StorageClass::localstatic_ && !(Optimizer::architecture == ARCHITECTURE_MSIL) && guardFuncs.find(sym->sb->decoratedName) == guardFuncs.end())
    {
        guardFuncs.insert(sym->sb->decoratedName);
        SYMBOL* guardfunc = namespacesearch("__static_guard", globalNameSpace, false, false);
        if (guardfunc)
        {
            guardfunc = guardfunc->tp->syms->front();
            EXPRESSION* guard = AnonymousVar(StorageClass::localstatic_, &stdint);
            insertInitSym(guard->v.sp);
            Dereference(&stdpointer, &guard);
            optimize_for_constants(&rv);
            rv = StatementGenerator::DestructorsForExpression(rv);
            rv = addLocalDestructor(rv, sym);
            EXPRESSION* guardexp = MakeExpression(Allocate<CallSite>());
            guardexp->v.func->sp = guardfunc;
            guardexp->v.func->functp = guardfunc->tp;
            guardexp->v.func->fcall = MakeExpression(ExpressionNode::pc_, guardfunc);
            guardexp->v.func->ascall = true;
            guardexp->v.func->arguments = argumentListFactory.CreateList();
            auto arg = Allocate<Argument>();
            arg->tp = &stdpointer;
            arg->exp = guard->left;
            guardexp->v.func->arguments->push_back(arg);
            rv = MakeExpression(ExpressionNode::check_nz_,
                          MakeExpression(ExpressionNode::comma_,
                                   MakeExpression(ExpressionNode::land_, MakeExpression(ExpressionNode::ne_, guard, MakeIntExpression(ExpressionNode::c_i_, -1)),
                                            MakeExpression(ExpressionNode::ne_, guardexp, MakeIntExpression(ExpressionNode::c_i_, 0))),
                                   MakeExpression(ExpressionNode::comma_, rv, MakeExpression(ExpressionNode::assign_, guard, MakeIntExpression(ExpressionNode::c_i_, -1)))),
                          MakeIntExpression(ExpressionNode::c_i_, 0));
        }
    }
    if (tp->IsStructured() || tp->IsArray())
    {
        if (*pos)
        {
            *pos = MakeExpression(ExpressionNode::comma_, *pos, expsym);
            pos = &(*pos)->right;
        }
        else
        {
            *pos = expsym;
        }
    }
    if (!rv)
        rv = MakeIntExpression(ExpressionNode::c_i_, 0);

    return rv;
}
bool assignDiscardsConst(Type* dest, Type* source)
{
    source = source->BaseType();
    dest = dest->BaseType();
    if (!source->IsPtr() || !dest->IsPtr())
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
bool isconstzero(Type* tp, EXPRESSION* exp)
{
    (void)tp;
    return (isintconst(exp) && exp->v.i == 0);
}
bool fittedConst(Type* tp, EXPRESSION* exp)
{
    int n;
    if (!tp->IsInt() || !isintconst(exp))
        return false;
    n = getSize(tp->BaseType()->type);
    switch (n)
    {
        case 8:
            if (tp->IsUnsigned())
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
            if (tp->IsUnsigned())
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
        case ExpressionNode::callsite_:
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
static Type* inttype(BasicType t1)
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
Type* destSize(Type* tp1, Type* tp2, EXPRESSION** exp1, EXPRESSION** exp2, bool minimizeInt, Type* atp)
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
    if (tp1->IsVoid() || tp2->IsVoid() || tp1->IsMsil() || tp2->IsMsil())
    {
        if (exp1 && exp2)
            error(ERR_NOT_AN_ALLOWED_TYPE);
        return tp1;
    }
    if (tp1->IsRef())
        tp1 = tp1->BaseType()->btp;
    if (tp2->IsRef())
        tp2 = tp2->BaseType()->btp;
    tp1 = tp1->BaseType();
    tp2 = tp2->BaseType();
    isctp1 = tp1->IsArithmetic() || tp1->type == BasicType::enum_;
    isctp2 = tp2->IsArithmetic() || tp2->type == BasicType::enum_;

    /*    if (isctp1 && isctp2 && tp1->type == tp2->type)
            return tp1 ;
    */
    if (tp1->type >= BasicType::float_ || tp2->type >= BasicType::float_)
    {
        Type* tp = nullptr;
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
        Type* rv;
        BasicType t1, t2;
        t1 = tp1->type;
        t2 = tp2->type;
        if (tp1->size == tp2->size)
            if (tp1->IsUnsigned() != tp2->IsUnsigned())
            {
                if (tp1->IsUnsigned())
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
            rv = Type::MakeType(t1);
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
        if (tp1->type <= BasicType::unsigned_long_long_ && tp2->IsPtr())
        {
            if (!tp1->IsPtr())
                cast(tp2, exp1);
            return tp2;
        }
        if (tp2->type <= BasicType::unsigned_long_long_ && tp1->IsPtr())
        {
            if (!tp2->IsPtr())
                cast(tp1, exp2);
            return tp1;
        }
        if (tp1->IsStructured())
        {
            return tp2;
        }
        if (tp2->IsStructured())
        {
            return tp1;
        }

        if (tp1->IsFunction())
            if (tp2->IsFunction() || tp2->IsPtr())
                return tp1;
        if (tp2->IsFunction())
            if (tp1->IsFunction() || tp1->IsPtr())
                return tp2;
        if (tp1->IsPtr())
            if (tp2->IsPtr())
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
EXPRESSION* EvaluateDest(EXPRESSION*exp, Type* tp)
{
    EXPRESSION* result = nullptr;
    if (!tp->BaseType()->hasbits && !IsCastValue(exp) && IsLValue(exp))
    {
        bool doit = false;
        std::stack<EXPRESSION*> stk;
        stk.push(exp->left);
        while (stk.size() && !doit)
        {
            EXPRESSION* c = stk.top();
            stk.pop();
            if (c->type == ExpressionNode::callsite_)
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
            result = AnonymousVar(StorageClass::auto_, &stdpointer);
            Dereference(&stdpointer, &result);
            exp2 = MakeExpression(ExpressionNode::assign_, result, exp2);
            exp2 = MakeExpression(exp->type, exp2);
            result = MakeExpression(exp->type, result);
            result = MakeExpression(ExpressionNode::comma_, exp2, result);
        }
    }
    return result;
}
void SetRuntimeData(LexList* lex, EXPRESSION* exp, SYMBOL* sym)
{
    if ((Optimizer::cparams.prm_stackprotect & STACK_UNINIT_VARIABLE) && sym->sb->runtimeSym && lex->data->errfile)
    {
        auto runtimeData = Allocate<Optimizer::RUNTIMEDATA>();
        const char* p = strrchr(lex->data->errfile, '/');
        if (!p)
            p = strrchr(lex->data->errfile, '\\');
        if (!p)
            p = lex->data->errfile;
        else
            p++;
        runtimeData->fileName = p;
        runtimeData->varName = sym->sb->decoratedName;
        runtimeData->lineno = lex->data->errline;
        runtimeData->runtimeSymOrig = sym->sb->runtimeSym;
        exp->runtimeData = runtimeData;
    }
}
EXPRESSION* getFunc(EXPRESSION* exp)
{
    EXPRESSION* rv = nullptr;
    while (exp->type == ExpressionNode::comma_ && exp->right)
    {
        rv = getFunc(exp->left);
        if (rv)
            return rv;
        exp = exp->right;
    }
    if (exp->type == ExpressionNode::thisref_)
        exp = exp->left;
    if (exp->type == ExpressionNode::add_)
    {
        rv = getFunc(exp->left);
        if (!rv)
            rv = getFunc(exp->right);
    }
    else if (exp->type == ExpressionNode::callsite_)
    {
        return exp;
    }
    return rv;
}
}  // namespace Parser
