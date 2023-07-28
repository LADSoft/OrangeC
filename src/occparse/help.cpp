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
#include "template.h"
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
        case StorageClass::member:
        case StorageClass::mutable_:
        case StorageClass::virtual_:
            return true;
        default:
            return false;
    }
}
bool istype(SYMBOL* sym)
{
    if (!sym->sb || sym->sb->storage_class == StorageClass::templateparam)
    {
        return sym->tp->templateParam->second->type == Keyword::_typename || sym->tp->templateParam->second->type == Keyword::_template;
    }
    return (sym->tp->type != BasicType::templateselector && sym->sb->storage_class == StorageClass::type) || sym->sb->storage_class == StorageClass::typedef_;
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
            member = MATCHKW(lex, Keyword::_classsel);
            if (member)
            {
                lex = getsym();
                member = MATCHKW(lex, Keyword::_star);
            }
            lex = prevsym(placeHolder);
            if (!member)
            {
                lines = old;
                return tparam->second->type == Keyword::_typename || tparam->second->type == Keyword::_template;
            }
        }
    }
    if (lex->data->type == l_id || MATCHKW(lex, Keyword::_classsel) || MATCHKW(lex, Keyword::_decltype))
    {
        bool isdecltype = MATCHKW(lex, Keyword::_decltype);
        SYMBOL *sym, *strSym = nullptr;
        LEXLIST* placeholder = lex;
        bool dest = false;
        nestedSearch(lex, &sym, &strSym, nullptr, &dest, nullptr, false, StorageClass::global, false, false);
        if (Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL))
            prevsym(placeholder);
        lines = old;
        if (structured && sym && istype(sym))
        {
            if (sym->tp->type == BasicType::templateparam)
            {
                if (sym->tp->templateParam->second->type == Keyword::_typename && sym->tp->templateParam->second->byClass.val)
                    *structured = isstructured(sym->tp->templateParam->second->byClass.val);
                else if (sym->tp->templateParam->second->type == Keyword::_template)
                    *structured = true;
            }
            else
            {
                *structured = isstructured(sym->tp);
            }
        }
        return (!sym && isdecltype) || (sym && sym->tp && istype(sym)) ||
               (assumeType && strSym && (strSym->tp->type == BasicType::templateselector || strSym->tp->type == BasicType::templatedecltype));
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
            case BasicType::far:
            case BasicType::near:
            case BasicType::const_:
            case BasicType::va_list:
            case BasicType::objectArray:
            case BasicType::volatile_:
            case BasicType::restrict_:
            case BasicType::static_:
            case BasicType::atomic:
            case BasicType::typedef_:
            case BasicType::lrqual:
            case BasicType::rrqual:
            case BasicType::derivedfromtemplate:
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
            case BasicType::pointer:
            case BasicType::func:
            case BasicType::ifunc:
                return false;
            case BasicType::derivedfromtemplate:
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
            case BasicType::unsigned_short:
            case BasicType::unsigned_char:
            case BasicType::unsigned_long:
            case BasicType::unsigned_long_long:
            case BasicType::wchar_t_:
                return true;
            default:
                return false;
        }
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
            case BasicType::char16_t_:
            case BasicType::char32_t_:
            case BasicType::unsigned_:
            case BasicType::short_:
            case BasicType::unsigned_short:
            case BasicType::char_:
            case BasicType::unsigned_char:
            case BasicType::signed_char:
            case BasicType::long_:
            case BasicType::unsigned_long:
            case BasicType::long_long:
            case BasicType::unsigned_long_long:
            case BasicType::wchar_t_:
            case BasicType::inative:
            case BasicType::unative:
                return true;
            case BasicType::templateparam:
                if (tp->templateParam->second->type == Keyword::_int)
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
            case BasicType::long_double:
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
            case BasicType::float__complex:
            case BasicType::double__complex:
            case BasicType::long_double_complex:
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
            case BasicType::float__imaginary:
            case BasicType::double__imaginary:
            case BasicType::long_double_imaginary:
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
    return tp->type == BasicType::__string || tp->type == BasicType::__object;
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
            case BasicType::atomic:
            case BasicType::typedef_:
            case BasicType::far:
            case BasicType::near:
            case BasicType::lrqual:
            case BasicType::rrqual:
            case BasicType::derivedfromtemplate:
            case BasicType::va_list:
            case BasicType::objectArray:
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
            case ExpressionNode::global:
            case ExpressionNode::auto_:
            case ExpressionNode::absolute:
            case ExpressionNode::pc:
            case ExpressionNode::threadlocal:
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
            case BasicType::va_list:
            case BasicType::objectArray:
            case BasicType::static_:
            case BasicType::atomic:
            case BasicType::typedef_:
            case BasicType::far:
            case BasicType::near:
            case BasicType::lrqual:
            case BasicType::rrqual:
            case BasicType::derivedfromtemplate:
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
            case BasicType::va_list:
            case BasicType::objectArray:
            case BasicType::static_:
            case BasicType::atomic:
            case BasicType::typedef_:
            case BasicType::far:
            case BasicType::near:
            case BasicType::rrqual:
            case BasicType::restrict_:
            case BasicType::derivedfromtemplate:
                tp = tp->btp;
                break;
            case BasicType::lrqual:
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
            case BasicType::va_list:
            case BasicType::objectArray:
            case BasicType::static_:
            case BasicType::atomic:
            case BasicType::typedef_:
            case BasicType::far:
            case BasicType::near:
            case BasicType::lrqual:
            case BasicType::restrict_:
            case BasicType::derivedfromtemplate:
                tp = tp->btp;
                break;
            case BasicType::rrqual:
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
            case BasicType::va_list:
            case BasicType::objectArray:
            case BasicType::static_:
            case BasicType::atomic:
            case BasicType::typedef_:
            case BasicType::far:
            case BasicType::near:
            case BasicType::lrqual:
            case BasicType::rrqual:
            case BasicType::derivedfromtemplate:
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
            case BasicType::va_list:
            case BasicType::objectArray:
            case BasicType::static_:
            case BasicType::restrict_:
            case BasicType::typedef_:
            case BasicType::far:
            case BasicType::near:
            case BasicType::lrqual:
            case BasicType::rrqual:
            case BasicType::derivedfromtemplate:
                tp = tp->btp;
                break;
            case BasicType::atomic:
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
            if ((*pat)->type == BasicType::rref && !isconst((*pat)->btp) && !isvolatile((*pat)->btp))
            {
                // forwarding?  unadorned rref!
                if (!nt->rref && basetype(nt)->type != BasicType::rref && !isarithmeticconst(exp))
                {
                    // lref
                    TYPE* t = basetype(nt);
                    *pat = MakeType(BasicType::lref, isref(t) ? t->btp : t);
                    return;
                }
                else
                {
                    // rref, get rid of qualifiers and return an rref
                    TYPE* tp1;
                    if (nt->type == BasicType::rref)
                        tp1 = basetype(nt->btp);
                    else
                        tp1 = basetype(nt);
                    // rref
                    *pat = MakeType(BasicType::lref, isref(tp1) ? tp1->btp : tp1);
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
                    *pat = MakeType(BasicType::lref, nt);
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
    else if (btp->type == BasicType::aggregate)
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
    enum e_lexType type = l_astr;
    STRING* string;
    list = Allocate<Optimizer::SLCHAR*>(count);
    while (lex && (lex->data->type == l_astr || lex->data->type == l_wstr || lex->data->type == l_ustr ||
                   lex->data->type == l_Ustr || lex->data->type == l_msilstr))
    {
        if (lex->data->type == l_msilstr)
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
        case ExpressionNode::c_u16:
        case ExpressionNode::c_u32:
        case ExpressionNode::c_c:
        case ExpressionNode::c_wc:
        case ExpressionNode::c_uc:
        case ExpressionNode::c_s:
        case ExpressionNode::c_us:
        case ExpressionNode::c_i:
        case ExpressionNode::c_ui:
        case ExpressionNode::c_l:
        case ExpressionNode::c_ul:
        case ExpressionNode::c_ll:
        case ExpressionNode::c_ull:
        case ExpressionNode::c_bit:
        case ExpressionNode::c_bool:
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
        case ExpressionNode::c_f:
        case ExpressionNode::c_d:
        case ExpressionNode::c_ld:
            return true;
        default:
            return false;
    }
}
bool isimaginaryconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::c_fi:
        case ExpressionNode::c_di:
        case ExpressionNode::c_ldi:
            return true;
        default:
            return false;
    }
}
bool iscomplexconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::c_fc:
        case ExpressionNode::c_dc:
        case ExpressionNode::c_ldc:
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
    return varNode(storage_class == StorageClass::auto_ || storage_class == StorageClass::parameter ? ExpressionNode::auto_ : ExpressionNode::global, rv);
}
void deref(TYPE* tp, EXPRESSION** exp)
{
    enum ExpressionNode en = ExpressionNode::l_i;
    tp = basetype(tp);
    switch ((tp->type == BasicType::enum_ && tp->btp) ? tp->btp->type : tp->type)
    {
        case BasicType::lref: /* only used during initialization */
            en = ExpressionNode::l_ref;
            break;
        case BasicType::rref: /* only used during initialization */
            en = ExpressionNode::l_ref;
            break;
        case BasicType::bit:
            en = ExpressionNode::l_bit;
            break;
        case BasicType::bool_:
            en = ExpressionNode::l_bool;
            break;
        case BasicType::char_:
            if (Optimizer::cparams.prm_charisunsigned)
                en = ExpressionNode::l_uc;
            else
                en = ExpressionNode::l_c;
            break;
        case BasicType::signed_char:
            en = ExpressionNode::l_c;
            break;
        case BasicType::char16_t_:
            en = ExpressionNode::l_u16;
            break;
        case BasicType::char32_t_:
            en = ExpressionNode::l_u32;
            break;
        case BasicType::unsigned_char:
            en = ExpressionNode::l_uc;
            break;
        case BasicType::short_:
            en = ExpressionNode::l_s;
            break;
        case BasicType::unsigned_short:
            en = ExpressionNode::l_us;
            break;
        case BasicType::wchar_t_:
            en = ExpressionNode::l_wc;
            break;
        case BasicType::int_:
            en = ExpressionNode::l_i;
            break;
        case BasicType::unsigned_:
            en = ExpressionNode::l_ui;
            break;
        case BasicType::long_:
            en = ExpressionNode::l_l;
            break;
        case BasicType::unsigned_long:
            en = ExpressionNode::l_ul;
            break;
        case BasicType::long_long:
            en = ExpressionNode::l_ll;
            break;
        case BasicType::unsigned_long_long:
            en = ExpressionNode::l_ull;
            break;
        case BasicType::float_:
            en = ExpressionNode::l_f;
            break;
        case BasicType::double_:
            en = ExpressionNode::l_d;
            break;
        case BasicType::long_double:
            en = ExpressionNode::l_ld;
            break;
        case BasicType::float__complex:
            en = ExpressionNode::l_fc;
            break;
        case BasicType::double__complex:
            en = ExpressionNode::l_dc;
            break;
        case BasicType::long_double_complex:
            en = ExpressionNode::l_ldc;
            break;
        case BasicType::float__imaginary:
            en = ExpressionNode::l_fi;
            break;
        case BasicType::double__imaginary:
            en = ExpressionNode::l_di;
            break;
        case BasicType::long_double_imaginary:
            en = ExpressionNode::l_ldi;
            break;
        case BasicType::__string:
            en = ExpressionNode::l_string;
            break;
        case BasicType::__object:
            en = ExpressionNode::l_object;
            break;
        case BasicType::pointer:
            if (tp->array || tp->vla)
                return;
            en = ExpressionNode::l_p;
            break;
        case BasicType::inative:
            en = ExpressionNode::l_inative;
            break;
        case BasicType::unative:
            en = ExpressionNode::l_unative;
            break;
        case BasicType::struct_:
        case BasicType::class_:
        case BasicType::union_:
        case BasicType::func:
        case BasicType::ifunc:
        case BasicType::any:
        case BasicType::templateparam:
        case BasicType::templateselector:
        case BasicType::templatedecltype:
        case BasicType::memberptr:
        case BasicType::aggregate:
            return;
        default:
            diag("deref error");
            break;
    }
    *exp = exprNode(en, *exp, nullptr);
    if (en == ExpressionNode::l_object)
        (*exp)->v.tp = tp;
}
int sizeFromType(TYPE* tp)
{
    int rv = -ISZ_UINT;
    tp = basetype(tp);
    switch (tp->type == BasicType::enum_ ? tp->btp->type : tp->type)
    {
        case BasicType::void_:
        case BasicType::templateparam:
        case BasicType::templateselector:
        case BasicType::templatedecltype:
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
        case BasicType::signed_char:
            rv = -ISZ_UCHAR;
            break;
        case BasicType::char16_t_:
            rv = ISZ_U16;
            break;
        case BasicType::char32_t_:
            rv = ISZ_U32;
            break;
        case BasicType::unsigned_char:
            rv = ISZ_UCHAR;
            break;
        case BasicType::short_:
            rv = -ISZ_USHORT;
            break;
        case BasicType::unsigned_short:
            rv = ISZ_USHORT;
            break;
        case BasicType::wchar_t_:
            rv = ISZ_WCHAR;
            break;
        case BasicType::int_:
            rv = -ISZ_UINT;
            break;
        case BasicType::inative:
            rv = -ISZ_UNATIVE;
            break;
        case BasicType::unsigned_:
            rv = ISZ_UINT;
            break;
        case BasicType::unative:
            rv = ISZ_UNATIVE;
            break;
        case BasicType::long_:
            rv = -ISZ_ULONG;
            break;
        case BasicType::unsigned_long:
            rv = ISZ_ULONG;
            break;
        case BasicType::long_long:
            rv = -ISZ_ULONGLONG;
            break;
        case BasicType::unsigned_long_long:
            rv = ISZ_ULONGLONG;
            break;
        case BasicType::float_:
            rv = ISZ_FLOAT;
            break;
        case BasicType::double_:
            rv = ISZ_DOUBLE;
            break;
        case BasicType::long_double:
            rv = ISZ_LDOUBLE;
            break;
        case BasicType::float__complex:
            rv = ISZ_CFLOAT;
            break;
        case BasicType::double__complex:
            rv = ISZ_CDOUBLE;
            break;
        case BasicType::long_double_complex:
            rv = ISZ_CLDOUBLE;
            break;
        case BasicType::float__imaginary:
            rv = ISZ_IFLOAT;
            break;
        case BasicType::double__imaginary:
            rv = ISZ_IDOUBLE;
            break;
        case BasicType::long_double_imaginary:
            rv = ISZ_ILDOUBLE;
            break;
        case BasicType::pointer:
            if (isarray(tp) && basetype(tp)->msil)
            {
                rv = ISZ_OBJECT;
                break;
            }
        case BasicType::func:
        case BasicType::ifunc:
        case BasicType::lref:
        case BasicType::rref:
        case BasicType::memberptr:
        case BasicType::aggregate:
            rv = ISZ_ADDR;
            break;
        case BasicType::__string:
            rv = ISZ_STRING;
            break;
        case BasicType::__object:
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
    enum ExpressionNode en = ExpressionNode::x_i;
    tp = basetype(tp);
    switch (tp->type == BasicType::enum_ ? tp->btp->type : tp->type)
    {
        case BasicType::lref:
        case BasicType::rref:
            en = ExpressionNode::x_p;
            break;
        case BasicType::func:
        case BasicType::ifunc:
            en = ExpressionNode::x_p;
            break;
        case BasicType::bit:
            en = ExpressionNode::x_bit;
            break;
        case BasicType::bool_:
            en = ExpressionNode::x_bool;
            break;
        case BasicType::char_:
            if (Optimizer::cparams.prm_charisunsigned)
                en = ExpressionNode::x_uc;
            else
                en = ExpressionNode::x_c;
            break;
        case BasicType::signed_char:
            en = ExpressionNode::x_c;
            break;
        case BasicType::unsigned_char:
            en = ExpressionNode::x_uc;
            break;
        case BasicType::char16_t_:
            en = ExpressionNode::x_u16;
            break;
        case BasicType::char32_t_:
            en = ExpressionNode::x_u32;
            break;
        case BasicType::short_:
            en = ExpressionNode::x_s;
            break;
        case BasicType::unsigned_short:
            en = ExpressionNode::x_us;
            break;
        case BasicType::wchar_t_:
            en = ExpressionNode::x_wc;
            break;
        case BasicType::int_:
            en = ExpressionNode::x_i;
            break;
        case BasicType::inative:
            en = ExpressionNode::x_inative;
            break;
        case BasicType::unsigned_:
            en = ExpressionNode::x_ui;
            break;
        case BasicType::unative:
            en = ExpressionNode::x_unative;
            break;
        case BasicType::long_:
            en = ExpressionNode::x_l;
            break;
        case BasicType::unsigned_long:
            en = ExpressionNode::x_ul;
            break;
        case BasicType::long_long:
            en = ExpressionNode::x_ll;
            break;
        case BasicType::unsigned_long_long:
            en = ExpressionNode::x_ull;
            break;
        case BasicType::float_:
            en = ExpressionNode::x_f;
            break;
        case BasicType::double_:
            en = ExpressionNode::x_d;
            break;
        case BasicType::long_double:
            en = ExpressionNode::x_ld;
            break;
        case BasicType::float__complex:
            en = ExpressionNode::x_fc;
            break;
        case BasicType::double__complex:
            en = ExpressionNode::x_dc;
            break;
        case BasicType::long_double_complex:
            en = ExpressionNode::x_ldc;
            break;
        case BasicType::float__imaginary:
            en = ExpressionNode::x_fi;
            break;
        case BasicType::double__imaginary:
            en = ExpressionNode::x_di;
            break;
        case BasicType::long_double_imaginary:
            en = ExpressionNode::x_ldi;
            break;
        case BasicType::__string:
            en = ExpressionNode::x_string;
            break;
        case BasicType::__object:
            en = ExpressionNode::x_object;
            break;
        case BasicType::pointer:
        case BasicType::aggregate:
            en = ExpressionNode::x_p;
            break;
        case BasicType::void_:
            return;
        case BasicType::templateparam:
        case BasicType::templateselector:
        case BasicType::templatedecltype:
            return;
        default:
            diag("cast error");
            break;
    }
    *exp = exprNode(en, *exp, nullptr);
}
bool castvalue(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case ExpressionNode::x_bit:
        case ExpressionNode::x_bool:
        case ExpressionNode::x_wc:
        case ExpressionNode::x_c:
        case ExpressionNode::x_uc:
        case ExpressionNode::x_u16:
        case ExpressionNode::x_u32:
        case ExpressionNode::x_s:
        case ExpressionNode::x_us:
        case ExpressionNode::x_i:
        case ExpressionNode::x_ui:
        case ExpressionNode::x_inative:
        case ExpressionNode::x_unative:
        case ExpressionNode::x_l:
        case ExpressionNode::x_ul:
        case ExpressionNode::x_ll:
        case ExpressionNode::x_ull:
        case ExpressionNode::x_f:
        case ExpressionNode::x_d:
        case ExpressionNode::x_ld:
        case ExpressionNode::x_fc:
        case ExpressionNode::x_dc:
        case ExpressionNode::x_ldc:
        case ExpressionNode::x_fi:
        case ExpressionNode::x_di:
        case ExpressionNode::x_ldi:
        case ExpressionNode::x_p:
        case ExpressionNode::x_string:
        case ExpressionNode::x_object:
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
        case ExpressionNode::lvalue:
        case ExpressionNode::l_bit:
        case ExpressionNode::l_bool:
        case ExpressionNode::l_wc:
        case ExpressionNode::l_u16:
        case ExpressionNode::l_u32:
        case ExpressionNode::l_c:
        case ExpressionNode::l_uc:
        case ExpressionNode::l_s:
        case ExpressionNode::l_us:
        case ExpressionNode::l_i:
        case ExpressionNode::l_ui:
        case ExpressionNode::l_inative:
        case ExpressionNode::l_unative:
        case ExpressionNode::l_l:
        case ExpressionNode::l_ul:
        case ExpressionNode::l_ll:
        case ExpressionNode::l_ull:
        case ExpressionNode::l_f:
        case ExpressionNode::l_d:
        case ExpressionNode::l_ld:
        case ExpressionNode::l_fc:
        case ExpressionNode::l_dc:
        case ExpressionNode::l_ldc:
        case ExpressionNode::l_fi:

        case ExpressionNode::l_di:
        case ExpressionNode::l_ldi:
        case ExpressionNode::l_p:
        case ExpressionNode::l_string:
        case ExpressionNode::l_object:
            return true;
        case ExpressionNode::l_ref:
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
                                exps = exprNode(ExpressionNode::structadd, exps, varNode(ExpressionNode::structelem, sp));
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
                expsym = varNode(ExpressionNode::global, sym);
                insertInitSym(sym);
            }
        }
        else
        {
            switch (sym->sb->storage_class)
            {
            case StorageClass::auto_:
            case StorageClass::register_:
            case StorageClass::parameter:
                local = true;
                expsym = varNode(ExpressionNode::auto_, sym);
                break;
            case StorageClass::localstatic:
                if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                {
                    expsym = thisptr;
                }
                else
                {
                    local = true;
                    expsym = varNode(ExpressionNode::global, sym);
                }
                break;
            case StorageClass::static_:
            case StorageClass::global:
                if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                {
                    expsym = thisptr;
                }
                else
                {
                    local = true;
                    expsym = varNode(ExpressionNode::global, sym);
                }
                break;
            case StorageClass::member:
            case StorageClass::mutable_:
                if (thisptr)
                    expsym = thisptr;
                else if (funcsp)
                    expsym = varNode(ExpressionNode::auto_, (SYMBOL*)basetype(funcsp->tp)->syms->front());  // this ptr
                else
                {
                    expsym = intNode(ExpressionNode::c_i, 0);
                    diag("convertInitToExpression: no this ptr");
                }
                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    expsym = exprNode(ExpressionNode::structadd, expsym, varNode(ExpressionNode::structelem, sym));
                else
                    expsym = exprNode(ExpressionNode::structadd, expsym, intNode(ExpressionNode::c_i, sym->sb->offset));
                break;
            case StorageClass::external:
                /*			expsym = varNode(ExpressionNode::global, sym);
                            local = true;
                            break;
                */
            case StorageClass::const_ant:
                return nullptr;
            default:
                diag("convertInitToExpression: unknown sym type");
                expsym = intNode(ExpressionNode::c_i, 0);
                break;
            }
        }
    }
    base = copy_expression(expsym);
    if (sym && isarray(sym->tp) && sym->tp->msil && !init->front()->noassign)
    {
        exp = intNode(ExpressionNode::msil_array_init, 0);
        exp->v.tp = sym->tp;
        // plop in a newarr call
        *pos = exprNode(ExpressionNode::assign, expsym, exp);
        noClear = true;
    }
    if (sym && isstructured(sym->tp) && basetype(sym->tp)->sp->sb->structuredAliasType)
    {
        noClear = true;
    }
    for (auto initItem : *init)
    {
        exp = nullptr;
        if (initItem->basetp)
        {
            if (initItem->noassign && initItem->exp)
            {
                exp = initItem->exp;
                if (exp->type == ExpressionNode::thisref)
                    exp = exp->left;
                if (thisptr && exp->type == ExpressionNode::func)
                {
                    EXPRESSION* exp1 = initItem->offset || (Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND)
                        ? exprNode(ExpressionNode::add, copy_expression(expsym), intNode(ExpressionNode::c_i, initItem->offset))
                        : copy_expression(expsym);
                    if (isarray(tp))
                    {
                        exp->v.func->arguments->front()->exp = exp1;
                    }
                    else
                    {
                        exp->v.func->thisptr = exp1;
                    }
                    exp = initItem->exp;
                }
                else
                {
                    exp = initItem->exp;
                }
            }
            else if (!initItem->exp)
            {
                // usually empty braces, coudl be an error though
                exp = exprNode(ExpressionNode::blockclear, copy_expression(expsym), nullptr);
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
                            exp = exprNode(ExpressionNode::add, exp, intNode(ExpressionNode::c_i, initItem->offset));
                        deref(basetype(initItem->basetp)->sp->sb->structuredAliasType, &exp);
                        exp = exprNode(ExpressionNode::assign, exp, initItem->exp);
                        noClear = true;
                    }
                    else
                    {
                        EXPRESSION* exp2 = initItem->exp;
                        while (exp2->type == ExpressionNode::not__lvalue)
                            exp2 = exp2->left;
                        if (exp2->type == ExpressionNode::func && exp2->v.func->returnSP)
                        {
                            exp2->v.func->returnSP->sb->allocate = false;
                            exp2->v.func->returnEXP = copy_expression(expsym);
                            exp = exp2;
                            noClear = true;
                        }
                        else if (exp2->type == ExpressionNode::thisref && exp2->left->v.func->returnSP)
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
                                exp = exprNode(ExpressionNode::add, exp, intNode(ExpressionNode::c_i, initItem->offset));
                            exp = exprNode(ExpressionNode::blockassign, exp, exp2);
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
                        if (Optimizer::cparams.c_dialect < Dialect::c99 && !Optimizer::cparams.prm_cplusplus)
                            error(ERR_C99_NON_CONSTANT_INITIALIZATION);
                        if (!sym)
                        {
                            expsym = anonymousVar(StorageClass::auto_, initItem->basetp);
                            sym = expsym->v.sp;
                        }
                        if (!isstructured(btp) || btp->sp->sb->trivialCons)
                        {
                            exp = exprNode(ExpressionNode::blockclear, copy_expression(expsym), nullptr);
                            exp->size = initItem->basetp;
                            exp = exprNode(ExpressionNode::void_, exp, nullptr);
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
                                    asn = exprNode(ExpressionNode::_sizeof, typeNode(btp), nullptr);
                                    EXPRESSION* exp4 = intNode(ExpressionNode::c_i, n);
                                    asn = exprNode(ExpressionNode::umul, exp4, asn);
                                }
                                else
                                {
                                    asn = exprNode(ExpressionNode::add, copy_expression(expsym), intNode(ExpressionNode::c_i, initItem->offset));
                                }
                                deref(initItem->basetp, &asn);
                                cast(initItem->basetp, &right);
                                right = exprNode(ExpressionNode::assign, asn, right);
                            }
                            if (*expp)
                                *expp = exprNode(ExpressionNode::void_, *expp, right);
                            else
                                *expp = right;
                            expp = &(*expp)->right;
                        }
                    }
                    else
                    {
                        /* constant expression */
                        SYMBOL* spc;
                        exp = anonymousVar(StorageClass::localstatic, initItem->basetp);
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
                                exp = exprNode(ExpressionNode::blockassign, copy_expression(expsym), exp);
                                exp->size = initItem->basetp;
                                exp->altdata = (void*)(initItem->basetp);
                            }
                        }
                    }
                }
            }
            else if (basetype(initItem->basetp)->type == BasicType::memberptr)
            {
                EXPRESSION* exp2 = initItem->exp;
                ;
                while (exp2->type == ExpressionNode::not__lvalue)
                    exp2 = exp2->left;
                if (exp2->type == ExpressionNode::func && exp2->v.func->returnSP)
                {
                    exp2->v.func->returnSP->sb->allocate = false;
                    exp2->v.func->returnEXP = copy_expression(expsym);
                    exp = exp2;
                }
                else
                {
                    if (exp2->type == ExpressionNode::memberptr)
                    {
                        int lab = dumpMemberPtr(exp2->v.sp, initItem->basetp, true);
                        exp2 = intNode(ExpressionNode::labcon, lab);
                    }
                    exp = exprNode(ExpressionNode::blockassign, copy_expression(expsym), exp2);
                    exp->size = initItem->basetp;
                    exp->altdata = (void*)(initItem->basetp);
                }
            }
            else
            {
                EXPRESSION* exps = copy_expression(expsym);
                exps->init = true;
                if (isarray(tp) && tp->msil)
                {
                    TYPE* btp = tp;
                    exps = exprNode(ExpressionNode::msil_array_access, nullptr, nullptr);
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
                        exps->v.msilArray->indices[i] = intNode(ExpressionNode::c_i, n);
                        q = q - n * btp->size;

                        btp = btp->btp;
                    }
                }
                else if ((Optimizer::architecture == ARCHITECTURE_MSIL) && initItem->fieldsp)
                {
                    if (initItem->fieldoffs)
                    {
                        exps = exprNode(ExpressionNode::add, exps, initItem->fieldoffs);
                    }
                    exps = msilThunkSubStructs(exps, expsym, initItem->fieldsp, initItem->offset);
                    exps = exprNode(ExpressionNode::structadd, exps, varNode(ExpressionNode::structelem, initItem->fieldsp));
                }
                else
                {
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
                            exps = exprNode(ExpressionNode::add, exps, intNode(ExpressionNode::c_i, initItem->offset));
                            exps->init = true;
                        }
                    }
                }
                if (exps->type != ExpressionNode::msil_array_access)
                    deref(initItem->basetp, &exps);
                optimize_for_constants(&exps);
                exp = initItem->exp;
                if (exp->type == ExpressionNode::void_)
                {
                    cast(initItem->basetp, &exp->right);
                    if (expsym)
                        exp->right = exprNode(ExpressionNode::assign, exps, exp->right);
                }
                else
                {
                    if (isarithmetic(initItem->basetp) || ispointer(initItem->basetp))
                        cast(initItem->basetp, &exp);
                    if (exps)
                        exp = exprNode(ExpressionNode::assign, exps, exp);
                }
            }
            if (sym && sym->sb->init && isatomic(initItem->basetp) && needsAtomicLockFromType(initItem->basetp))
            {
                EXPRESSION* p1 = exprNode(ExpressionNode::add, expsym->left, intNode(ExpressionNode::c_i, initItem->basetp->size - ATOMIC_FLAG_SPACE));
                deref(&stdint, &p1);
                p1 = exprNode(ExpressionNode::assign, p1, intNode(ExpressionNode::c_i, 0));
                exp = exprNode(ExpressionNode::void_, exp, p1);
            }
        }
        if (exp)
        {
            if (*pos)
            {
                *pos = exprNode(ExpressionNode::void_, *pos, exp);
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
        if (fexp->type == ExpressionNode::thisref)
            fexp = fexp->left->v.func->thisptr;
        exp = exprNode(ExpressionNode::blockclear, fexp, nullptr);
        exp->size = tp;
        rv = exprNode(ExpressionNode::void_, exp, rv);
    }
    if (sym && sym->sb->storage_class == StorageClass::localstatic && !(Optimizer::architecture == ARCHITECTURE_MSIL))
    {
        SYMBOL* guardfunc = namespacesearch("__static_guard", globalNameSpace, false, false);
        if (guardfunc)
        {
            guardfunc = guardfunc->tp->syms->front();
            EXPRESSION* guard = anonymousVar(StorageClass::localstatic, &stdint);
            insertInitSym(guard->v.sp);
            deref(&stdpointer, &guard);
            optimize_for_constants(&rv);
            rv = destructLocal(rv);
            rv = addLocalDestructor(rv, sym);
            EXPRESSION* guardexp = exprNode(ExpressionNode::func, nullptr, nullptr);
            guardexp->v.func = Allocate<FUNCTIONCALL>();
            guardexp->v.func->sp = guardfunc;
            guardexp->v.func->functp = guardfunc->tp;
            guardexp->v.func->fcall = varNode(ExpressionNode::pc, guardfunc);
            guardexp->v.func->ascall = true;
            guardexp->v.func->arguments = initListListFactory.CreateList();
            auto arg = Allocate<INITLIST>();
            arg->tp = &stdpointer;
            arg->exp = guard->left;
            guardexp->v.func->arguments->push_back(arg);
            rv = exprNode(ExpressionNode::void_nz,
                          exprNode(ExpressionNode::void_,
                                   exprNode(ExpressionNode::land, exprNode(ExpressionNode::ne, guard, intNode(ExpressionNode::c_i, -1)),
                                            exprNode(ExpressionNode::ne, guardexp, intNode(ExpressionNode::c_i, 0))),
                                   exprNode(ExpressionNode::void_, rv, exprNode(ExpressionNode::assign, guard, intNode(ExpressionNode::c_i, -1)))),
                          intNode(ExpressionNode::c_i, 0));
        }
    }
    if (isstructured(tp))
    {
        if (*pos)
        {
            *pos = exprNode(ExpressionNode::void_, *pos, expsym);
            pos = &(*pos)->right;
        }
        else
        {
            *pos = expsym;
        }
    }
    if (!rv)
        rv = intNode(ExpressionNode::c_i, 0);

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
                case BasicType::va_list:
                case BasicType::objectArray:
                case BasicType::restrict_:
                case BasicType::volatile_:
                case BasicType::static_:
                case BasicType::typedef_:
                case BasicType::lrqual:
                case BasicType::rrqual:
                case BasicType::derivedfromtemplate:
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
                case BasicType::va_list:
                case BasicType::objectArray:
                case BasicType::restrict_:
                case BasicType::volatile_:
                case BasicType::static_:
                case BasicType::typedef_:
                case BasicType::lrqual:
                case BasicType::rrqual:
                    source = source->btp;
                    break;
                default:
                    done = true;
                    break;
            }
        }
        if (sourcc && !destc)
            return true;
        if (source->type != BasicType::pointer || dest->type != BasicType::pointer)
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
        case ExpressionNode::add:
        case ExpressionNode::arrayadd:
        case ExpressionNode::structadd:
            return (isconstaddress(exp->left) || isintconst(exp->left)) && (isconstaddress(exp->right) || isintconst(exp->right));
        case ExpressionNode::global:
        case ExpressionNode::pc:
        case ExpressionNode::labcon:
            return true;
        case ExpressionNode::func:
            return !exp->v.func->ascall;
        case ExpressionNode::threadlocal:
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
        case BasicType::unsigned_char:
            return &stdunsignedchar;
        case BasicType::signed_char:
            return &stdsignedchar;
        case BasicType::short_:
            return &stdshort;
        case BasicType::unsigned_short:
            return &stdunsignedshort;
        case BasicType::wchar_t_:
            return &stdwidechar;
        default:
        case BasicType::int_:
        case BasicType::inative:
            return &stdint;
        case BasicType::char16_t_:
            return &stdchar16t;
        case BasicType::char32_t_:
            return &stdchar32t;
        case BasicType::unsigned_:
        case BasicType::unative:
            return &stdunsigned;
        case BasicType::long_:
            return &stdlong;
        case BasicType::unsigned_long:
            return &stdunsignedlong;
        case BasicType::long_long:
            return &stdlonglong;
        case BasicType::unsigned_long_long:
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
    if (tp1->type == BasicType::any)
        return tp1;
    if (tp2->type == BasicType::any)
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
        int isim1 = tp1->type >= BasicType::float__imaginary && tp1->type <= BasicType::long_double_imaginary;
        int isim2 = tp2->type >= BasicType::float__imaginary && tp2->type <= BasicType::long_double_imaginary;
        int iscx1 = tp1->type >= BasicType::float__complex && tp1->type <= BasicType::long_double_complex;
        int iscx2 = tp2->type >= BasicType::float__complex && tp2->type <= BasicType::long_double_complex;
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
                if ((int)tp1->type - (int)BasicType::float__complex >= (int)tp2->type - (int)BasicType::float__imaginary)
                    tp = tp1;
                else
                    tp = &stddoublecomplex;
            }
            else if (tp2->type >= BasicType::float_)
            {
                if ((int)tp1->type - (int)BasicType::float__complex >= (int)tp2->type - (int)BasicType::float_)
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
                if ((int)tp1->type - (int)BasicType::float__imaginary <= (int)tp2->type - (int)BasicType::float__complex)
                    tp = tp2;
                else
                    tp = &stddoublecomplex;
            }
            else if (tp2->type >= BasicType::float_)
            {
                if ((int)tp1->type - (int)BasicType::float_ <= (int)tp2->type - (int)BasicType::float__complex)
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
                if ((int)tp1->type - (int)BasicType::float__imaginary >= (int)tp2->type - (int)BasicType::float_)
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
                if ((int)tp1->type - (int)BasicType::float_ <= (int)tp2->type - (int)BasicType::float__imaginary)
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
        if (exp1 && tp->type != tp1->type && exp1 && tp2->type != BasicType::pointer)
            cast(tp, exp1);
        if (exp2 && tp->type != tp2->type && exp1 && tp1->type != BasicType::pointer)
            cast(tp, exp2);
        return tp;
    }
    if (isctp1 && isctp2)
    {
        TYPE* rv;
        enum BasicType t1, t2;
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
        if (t1 < BasicType::int_)
            t1 = BasicType::int_;
        if (t2 < BasicType::int_)
            t2 = BasicType::int_;
        t1 = btmax(t1, t2);
        rv = inttype(t1);
        if (exp1 && rv->type != tp1->type && exp1)
            cast(rv, exp1);
        if (exp2 && rv->type != tp2->type && exp2)
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
        if (tp1->type <= BasicType::unsigned_long_long && ispointer(tp2))
        {
            if (!ispointer(tp1))
                cast(tp2, exp1);
            return tp2;
        }
        if (tp2->type <= BasicType::unsigned_long_long && ispointer(tp1))
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
    if (exp->preincdec || exp->type == ExpressionNode::auto_inc || exp->type == ExpressionNode::auto_dec)
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
            if (c->type == ExpressionNode::func)
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
            exp2 = exprNode(ExpressionNode::assign, result, exp2);
            exp2 = exprNode(exp->type, exp2, nullptr);
            result = exprNode(exp->type, result, nullptr);
            result = exprNode(ExpressionNode::void_, exp2, result);
        }
    }
    return result;
}
}  // namespace Parser
