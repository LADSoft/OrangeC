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
        case sc_member:
        case sc_mutable:
        case sc_virtual:
            return true;
        default:
            return false;
    }
}
bool istype(SYMBOL* sym)
{
    if (!sym->sb || sym->sb->storage_class == sc_templateparam)
    {
        return sym->tp->templateParam->second->type == kw_typename || sym->tp->templateParam->second->type == kw_template;
    }
    return (sym->tp->type != bt_templateselector && sym->sb->storage_class == sc_type) || sym->sb->storage_class == sc_typedef;
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
            member = MATCHKW(lex, classsel);
            if (member)
            {
                lex = getsym();
                member = MATCHKW(lex, star);
            }
            lex = prevsym(placeHolder);
            if (!member)
            {
                lines = old;
                return tparam->second->type == kw_typename || tparam->second->type == kw_template;
            }
        }
    }
    if (lex->data->type == l_id || MATCHKW(lex, classsel) || MATCHKW(lex, kw_decltype))
    {
        bool isdecltype = MATCHKW(lex, kw_decltype);
        SYMBOL *sym, *strSym = nullptr;
        LEXLIST* placeholder = lex;
        bool dest = false;
        nestedSearch(lex, &sym, &strSym, nullptr, &dest, nullptr, false, sc_global, false, false);
        if (Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL))
            prevsym(placeholder);
        lines = old;
        if (structured && sym && istype(sym))
        {
            if (sym->tp->type == bt_templateparam)
            {
                if (sym->tp->templateParam->second->type == kw_typename && sym->tp->templateParam->second->byClass.val)
                    *structured = isstructured(sym->tp->templateParam->second->byClass.val);
                else if (sym->tp->templateParam->second->type == kw_template)
                    *structured = true;
            }
            else
            {
                *structured = isstructured(sym->tp);
            }
        }
        return (!sym && isdecltype) || (sym && sym->tp && istype(sym)) ||
               (assumeType && strSym && (strSym->tp->type == bt_templateselector || strSym->tp->type == bt_templatedecltype));
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
            case bt_far:
            case bt_near:
            case bt_const:
            case bt_va_list:
            case bt_objectArray:
            case bt_volatile:
            case bt_restrict:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
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
            case bt_pointer:
            case bt_func:
            case bt_ifunc:
                return false;
            case bt_derivedfromtemplate:
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
    return basetype(tp)->type == bt_auto;
}
bool isunsigned(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
            case bt_bool:
            case bt_unsigned:
            case bt_unsigned_short:
            case bt_unsigned_char:
            case bt_unsigned_long:
            case bt_unsigned_long_long:
            case bt_wchar_t:
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
            case bt_bool:
            case bt_int:
            case bt_char16_t:
            case bt_char32_t:
            case bt_unsigned:
            case bt_short:
            case bt_unsigned_short:
            case bt_char:
            case bt_unsigned_char:
            case bt_signed_char:
            case bt_long:
            case bt_unsigned_long:
            case bt_long_long:
            case bt_unsigned_long_long:
            case bt_wchar_t:
            case bt_inative:
            case bt_unative:
                return true;
            case bt_templateparam:
                if (tp->templateParam->second->type == kw_int)
                    return isint(tp->templateParam->second->byNonType.tp);
                return false;
            default:
                if (tp->type == bt_enum && !Optimizer::cparams.prm_cplusplus)
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
            case bt_float:
            case bt_double:
            case bt_long_double:
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
            case bt_float_complex:
            case bt_double_complex:
            case bt_long_double_complex:
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
            case bt_float_imaginary:
            case bt_double_imaginary:
            case bt_long_double_imaginary:
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
    return tp->type == bt___string || tp->type == bt___object;
}
bool isconstraw(const TYPE* tp)
{
    bool done = false;
    bool rv = false;
    while (!done && tp)
    {
        switch (tp->type)
        {
            case bt_restrict:
            case bt_volatile:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
            case bt_va_list:
            case bt_objectArray:
                tp = tp->btp;
                break;
            case bt_const:
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
            case en_global:
            case en_auto:
            case en_absolute:
            case en_pc:
            case en_threadlocal:
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
            case bt_restrict:
            case bt_const:
            case bt_va_list:
            case bt_objectArray:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_volatile:
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
            case bt_volatile:
            case bt_const:
            case bt_va_list:
            case bt_objectArray:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
            case bt_rrqual:
            case bt_restrict:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_lrqual:
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
            case bt_volatile:
            case bt_const:
            case bt_va_list:
            case bt_objectArray:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
            case bt_lrqual:
            case bt_restrict:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_rrqual:
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
            case bt_volatile:
            case bt_const:
            case bt_va_list:
            case bt_objectArray:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_restrict:
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
            case bt_volatile:
            case bt_const:
            case bt_va_list:
            case bt_objectArray:
            case bt_static:
            case bt_restrict:
            case bt_typedef:
            case bt_far:
            case bt_near:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
                tp = tp->btp;
                break;
            case bt_atomic:
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
        return tp->type == bt_void;
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
        return tp->type == bt_union;
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
            if ((*pat)->type == bt_rref && !isconst((*pat)->btp) && !isvolatile((*pat)->btp))
            {
                // forwarding?  unadorned rref!
                if (!nt->rref && basetype(nt)->type != bt_rref && !isarithmeticconst(exp))
                {
                    // lref
                    TYPE* t = basetype(nt);
                    *pat = MakeType(bt_lref, isref(t) ? t->btp : t);
                    return;
                }
                else
                {
                    // rref, get rid of qualifiers and return an rref
                    TYPE* tp1;
                    if (nt->type == bt_rref)
                        tp1 = basetype(nt->btp);
                    else
                        tp1 = basetype(nt);
                    // rref
                    *pat = MakeType(bt_lref, isref(tp1) ? tp1->btp : tp1);
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
                    *pat = MakeType(bt_lref, nt);
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
            while ((*pat)->type != bt_auto)
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
    else if (btp->type == bt_aggregate)
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
LEXLIST* concatStrings(LEXLIST* lex, EXPRESSION** expr, enum e_lexType* tp, int* elems)
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
        case en_c_u16:
        case en_c_u32:
        case en_c_c:
        case en_c_wc:
        case en_c_uc:
        case en_c_s:
        case en_c_us:
        case en_c_i:
        case en_c_ui:
        case en_c_l:
        case en_c_ul:
        case en_c_ll:
        case en_c_ull:
        case en_c_bit:
        case en_c_bool:
        case en_const:
            return true;
        default:
            return false;
    }
}
bool isfloatconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case en_c_f:
        case en_c_d:
        case en_c_ld:
            return true;
        default:
            return false;
    }
}
bool isimaginaryconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            return true;
        default:
            return false;
    }
}
bool iscomplexconst(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            return true;
        default:
            return false;
    }
}
EXPRESSION* anonymousVar(enum e_sc storage_class, TYPE* tp)
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
        InsertSymbol(rv, storage_class, lk_none, false);
    SetLinkerNames(rv, lk_none);
    return varNode(storage_class == sc_auto || storage_class == sc_parameter ? en_auto : en_global, rv);
}
void deref(TYPE* tp, EXPRESSION** exp)
{
    enum e_node en = en_l_i;
    tp = basetype(tp);
    switch ((tp->type == bt_enum && tp->btp) ? tp->btp->type : tp->type)
    {
        case bt_lref: /* only used during initialization */
            en = en_l_ref;
            break;
        case bt_rref: /* only used during initialization */
            en = en_l_ref;
            break;
        case bt_bit:
            en = en_l_bit;
            break;
        case bt_bool:
            en = en_l_bool;
            break;
        case bt_char:
            if (Optimizer::cparams.prm_charisunsigned)
                en = en_l_uc;
            else
                en = en_l_c;
            break;
        case bt_signed_char:
            en = en_l_c;
            break;
        case bt_char16_t:
            en = en_l_u16;
            break;
        case bt_char32_t:
            en = en_l_u32;
            break;
        case bt_unsigned_char:
            en = en_l_uc;
            break;
        case bt_short:
            en = en_l_s;
            break;
        case bt_unsigned_short:
            en = en_l_us;
            break;
        case bt_wchar_t:
            en = en_l_wc;
            break;
        case bt_int:
            en = en_l_i;
            break;
        case bt_unsigned:
            en = en_l_ui;
            break;
        case bt_long:
            en = en_l_l;
            break;
        case bt_unsigned_long:
            en = en_l_ul;
            break;
        case bt_long_long:
            en = en_l_ll;
            break;
        case bt_unsigned_long_long:
            en = en_l_ull;
            break;
        case bt_float:
            en = en_l_f;
            break;
        case bt_double:
            en = en_l_d;
            break;
        case bt_long_double:
            en = en_l_ld;
            break;
        case bt_float_complex:
            en = en_l_fc;
            break;
        case bt_double_complex:
            en = en_l_dc;
            break;
        case bt_long_double_complex:
            en = en_l_ldc;
            break;
        case bt_float_imaginary:
            en = en_l_fi;
            break;
        case bt_double_imaginary:
            en = en_l_di;
            break;
        case bt_long_double_imaginary:
            en = en_l_ldi;
            break;
        case bt___string:
            en = en_l_string;
            break;
        case bt___object:
            en = en_l_object;
            break;
        case bt_pointer:
            if (tp->array || tp->vla)
                return;
            en = en_l_p;
            break;
        case bt_inative:
            en = en_l_inative;
            break;
        case bt_unative:
            en = en_l_unative;
            break;
        case bt_struct:
        case bt_class:
        case bt_union:
        case bt_func:
        case bt_ifunc:
        case bt_any:
        case bt_templateparam:
        case bt_templateselector:
        case bt_templatedecltype:
        case bt_memberptr:
        case bt_aggregate:
            return;
        default:
            diag("deref error");
            break;
    }
    *exp = exprNode(en, *exp, nullptr);
    if (en == en_l_object)
        (*exp)->v.tp = tp;
}
int sizeFromType(TYPE* tp)
{
    int rv = -ISZ_UINT;
    tp = basetype(tp);
    switch (tp->type == bt_enum ? tp->btp->type : tp->type)
    {
        case bt_void:
        case bt_templateparam:
        case bt_templateselector:
        case bt_templatedecltype:
            rv = ISZ_UINT;
            break;
        case bt_bool:
            rv = ISZ_BOOLEAN;
            break;
        case bt_char:
            if (Optimizer::cparams.prm_charisunsigned)
                rv = ISZ_UCHAR;
            else
                rv = -ISZ_UCHAR;
            break;
        case bt_signed_char:
            rv = -ISZ_UCHAR;
            break;
        case bt_char16_t:
            rv = ISZ_U16;
            break;
        case bt_char32_t:
            rv = ISZ_U32;
            break;
        case bt_unsigned_char:
            rv = ISZ_UCHAR;
            break;
        case bt_short:
            rv = -ISZ_USHORT;
            break;
        case bt_unsigned_short:
            rv = ISZ_USHORT;
            break;
        case bt_wchar_t:
            rv = ISZ_WCHAR;
            break;
        case bt_int:
            rv = -ISZ_UINT;
            break;
        case bt_inative:
            rv = -ISZ_UNATIVE;
            break;
        case bt_unsigned:
            rv = ISZ_UINT;
            break;
        case bt_unative:
            rv = ISZ_UNATIVE;
            break;
        case bt_long:
            rv = -ISZ_ULONG;
            break;
        case bt_unsigned_long:
            rv = ISZ_ULONG;
            break;
        case bt_long_long:
            rv = -ISZ_ULONGLONG;
            break;
        case bt_unsigned_long_long:
            rv = ISZ_ULONGLONG;
            break;
        case bt_float:
            rv = ISZ_FLOAT;
            break;
        case bt_double:
            rv = ISZ_DOUBLE;
            break;
        case bt_long_double:
            rv = ISZ_LDOUBLE;
            break;
        case bt_float_complex:
            rv = ISZ_CFLOAT;
            break;
        case bt_double_complex:
            rv = ISZ_CDOUBLE;
            break;
        case bt_long_double_complex:
            rv = ISZ_CLDOUBLE;
            break;
        case bt_float_imaginary:
            rv = ISZ_IFLOAT;
            break;
        case bt_double_imaginary:
            rv = ISZ_IDOUBLE;
            break;
        case bt_long_double_imaginary:
            rv = ISZ_ILDOUBLE;
            break;
        case bt_pointer:
            if (isarray(tp) && basetype(tp)->msil)
            {
                rv = ISZ_OBJECT;
                break;
            }
        case bt_func:
        case bt_ifunc:
        case bt_lref:
        case bt_rref:
        case bt_memberptr:
        case bt_aggregate:
            rv = ISZ_ADDR;
            break;
        case bt___string:
            rv = ISZ_STRING;
            break;
        case bt___object:
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
    enum e_node en = en_x_i;
    tp = basetype(tp);
    switch (tp->type == bt_enum ? tp->btp->type : tp->type)
    {
        case bt_lref:
        case bt_rref:
            en = en_x_p;
            break;
        case bt_func:
        case bt_ifunc:
            en = en_x_p;
            break;
        case bt_bit:
            en = en_x_bit;
            break;
        case bt_bool:
            en = en_x_bool;
            break;
        case bt_char:
            if (Optimizer::cparams.prm_charisunsigned)
                en = en_x_uc;
            else
                en = en_x_c;
            break;
        case bt_signed_char:
            en = en_x_c;
            break;
        case bt_unsigned_char:
            en = en_x_uc;
            break;
        case bt_char16_t:
            en = en_x_u16;
            break;
        case bt_char32_t:
            en = en_x_u32;
            break;
        case bt_short:
            en = en_x_s;
            break;
        case bt_unsigned_short:
            en = en_x_us;
            break;
        case bt_wchar_t:
            en = en_x_wc;
            break;
        case bt_int:
            en = en_x_i;
            break;
        case bt_inative:
            en = en_x_inative;
            break;
        case bt_unsigned:
            en = en_x_ui;
            break;
        case bt_unative:
            en = en_x_unative;
            break;
        case bt_long:
            en = en_x_l;
            break;
        case bt_unsigned_long:
            en = en_x_ul;
            break;
        case bt_long_long:
            en = en_x_ll;
            break;
        case bt_unsigned_long_long:
            en = en_x_ull;
            break;
        case bt_float:
            en = en_x_f;
            break;
        case bt_double:
            en = en_x_d;
            break;
        case bt_long_double:
            en = en_x_ld;
            break;
        case bt_float_complex:
            en = en_x_fc;
            break;
        case bt_double_complex:
            en = en_x_dc;
            break;
        case bt_long_double_complex:
            en = en_x_ldc;
            break;
        case bt_float_imaginary:
            en = en_x_fi;
            break;
        case bt_double_imaginary:
            en = en_x_di;
            break;
        case bt_long_double_imaginary:
            en = en_x_ldi;
            break;
        case bt___string:
            en = en_x_string;
            break;
        case bt___object:
            en = en_x_object;
            break;
        case bt_pointer:
        case bt_aggregate:
            en = en_x_p;
            break;
        case bt_void:
            return;
        case bt_templateparam:
        case bt_templateselector:
        case bt_templatedecltype:
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
        case en_x_bit:
        case en_x_bool:
        case en_x_wc:
        case en_x_c:
        case en_x_uc:
        case en_x_u16:
        case en_x_u32:
        case en_x_s:
        case en_x_us:
        case en_x_i:
        case en_x_ui:
        case en_x_inative:
        case en_x_unative:
        case en_x_l:
        case en_x_ul:
        case en_x_ll:
        case en_x_ull:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_p:
        case en_x_string:
        case en_x_object:
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
        case en_lvalue:
        case en_l_bit:
        case en_l_bool:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_c:
        case en_l_uc:
        case en_l_s:
        case en_l_us:
        case en_l_i:
        case en_l_ui:
        case en_l_inative:
        case en_l_unative:
        case en_l_l:
        case en_l_ul:
        case en_l_ll:
        case en_l_ull:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_fi:

        case en_l_di:
        case en_l_ldi:
        case en_l_p:
        case en_l_string:
        case en_l_object:
            return true;
        case en_l_ref:
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
                                exps = exprNode(en_structadd, exps, varNode(en_structelem, sp));
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
                    expsym = varNode(en_auto, sym);  // this ptr
                else
                    expsym = anonymousVar(sc_auto, tp);
            }
            else  // global initialization, make a temporary variable
            {
                char sanon[256];
                sprintf(sanon, "sanon_%d", staticanonymousIndex++);
                // this is probably for a structured cast that is being converted to a pointer with ampersand...
                SYMBOL* sym = makeID(sc_static, tp, nullptr, sanon);
                SetLinkerNames(sym, lk_cdecl);
                insertDynamicInitializer(sym, init);
                expsym = varNode(en_global, sym);
                insertInitSym(sym);
            }
        }
        else
        {
            switch (sym->sb->storage_class)
            {
            case sc_auto:
            case sc_register:
            case sc_parameter:
                local = true;
                expsym = varNode(en_auto, sym);
                break;
            case sc_localstatic:
                if (sym->sb->attribs.inheritable.linkage3 == lk_threadlocal)
                {
                    expsym = thisptr;
                }
                else
                {
                    local = true;
                    expsym = varNode(en_global, sym);
                }
                break;
            case sc_static:
            case sc_global:
                if (sym->sb->attribs.inheritable.linkage3 == lk_threadlocal)
                {
                    expsym = thisptr;
                }
                else
                {
                    local = true;
                    expsym = varNode(en_global, sym);
                }
                break;
            case sc_member:
            case sc_mutable:
                if (thisptr)
                    expsym = thisptr;
                else if (funcsp)
                    expsym = varNode(en_auto, (SYMBOL*)basetype(funcsp->tp)->syms->front());  // this ptr
                else
                {
                    expsym = intNode(en_c_i, 0);
                    diag("convertInitToExpression: no this ptr");
                }
                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    expsym = exprNode(en_structadd, expsym, varNode(en_structelem, sym));
                else
                    expsym = exprNode(en_structadd, expsym, intNode(en_c_i, sym->sb->offset));
                break;
            case sc_external:
                /*			expsym = varNode(en_global, sym);
                            local = true;
                            break;
                */
            case sc_constant:
                return nullptr;
            default:
                diag("convertInitToExpression: unknown sym type");
                expsym = intNode(en_c_i, 0);
                break;
            }
        }
    }
    base = copy_expression(expsym);
    if (sym && isarray(sym->tp) && sym->tp->msil && !init->front()->noassign)
    {
        exp = intNode(en_msil_array_init, 0);
        exp->v.tp = sym->tp;
        // plop in a newarr call
        *pos = exprNode(en_assign, expsym, exp);
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
                if (exp->type == en_thisref)
                    exp = exp->left;
                if (thisptr && exp->type == en_func)
                {
                    EXPRESSION* exp1 = initItem->offset || (Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND)
                        ? exprNode(en_add, copy_expression(expsym), intNode(en_c_i, initItem->offset))
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
                exp = exprNode(en_blockclear, copy_expression(expsym), nullptr);
                exp->size = MakeType(bt_struct);
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
                            exp = exprNode(en_add, exp, intNode(en_c_i, initItem->offset));
                        deref(basetype(initItem->basetp)->sp->sb->structuredAliasType, &exp);
                        exp = exprNode(en_assign, exp, initItem->exp);
                        noClear = true;
                    }
                    else
                    {
                        EXPRESSION* exp2 = initItem->exp;
                        while (exp2->type == en_not_lvalue)
                            exp2 = exp2->left;
                        if (exp2->type == en_func && exp2->v.func->returnSP)
                        {
                            exp2->v.func->returnSP->sb->allocate = false;
                            exp2->v.func->returnEXP = copy_expression(expsym);
                            exp = exp2;
                            noClear = true;
                        }
                        else if (exp2->type == en_thisref && exp2->left->v.func->returnSP)
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
                                exp = exprNode(en_add, exp, intNode(en_c_i, initItem->offset));
                            exp = exprNode(en_blockassign, exp, exp2);
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
                        if (!Optimizer::cparams.prm_c99 && !Optimizer::cparams.prm_cplusplus)
                            error(ERR_C99_NON_CONSTANT_INITIALIZATION);
                        if (!sym)
                        {
                            expsym = anonymousVar(sc_auto, initItem->basetp);
                            sym = expsym->v.sp;
                        }
                        if (!isstructured(btp) || btp->sp->sb->trivialCons)
                        {
                            exp = exprNode(en_blockclear, copy_expression(expsym), nullptr);
                            exp->size = initItem->basetp;
                            exp = exprNode(en_void, exp, nullptr);
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
                                    asn = exprNode(en__sizeof, typeNode(btp), nullptr);
                                    EXPRESSION* exp4 = intNode(en_c_i, n);
                                    asn = exprNode(en_umul, exp4, asn);
                                }
                                else
                                {
                                    asn = exprNode(en_add, copy_expression(expsym), intNode(en_c_i, initItem->offset));
                                }
                                deref(initItem->basetp, &asn);
                                cast(initItem->basetp, &right);
                                right = exprNode(en_assign, asn, right);
                            }
                            if (*expp)
                                *expp = exprNode(en_void, *expp, right);
                            else
                                *expp = right;
                            expp = &(*expp)->right;
                        }
                    }
                    else
                    {
                        /* constant expression */
                        SYMBOL* spc;
                        exp = anonymousVar(sc_localstatic, initItem->basetp);
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
                                exp = exprNode(en_blockassign, copy_expression(expsym), exp);
                                exp->size = initItem->basetp;
                                exp->altdata = (void*)(initItem->basetp);
                            }
                        }
                    }
                }
            }
            else if (basetype(initItem->basetp)->type == bt_memberptr)
            {
                EXPRESSION* exp2 = initItem->exp;
                ;
                while (exp2->type == en_not_lvalue)
                    exp2 = exp2->left;
                if (exp2->type == en_func && exp2->v.func->returnSP)
                {
                    exp2->v.func->returnSP->sb->allocate = false;
                    exp2->v.func->returnEXP = copy_expression(expsym);
                    exp = exp2;
                }
                else
                {
                    if (exp2->type == en_memberptr)
                    {
                        int lab = dumpMemberPtr(exp2->v.sp, initItem->basetp, true);
                        exp2 = intNode(en_labcon, lab);
                    }
                    exp = exprNode(en_blockassign, copy_expression(expsym), exp2);
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
                    exps = exprNode(en_msil_array_access, nullptr, nullptr);
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
                        exps->v.msilArray->indices[i] = intNode(en_c_i, n);
                        q = q - n * btp->size;

                        btp = btp->btp;
                    }
                }
                else if ((Optimizer::architecture == ARCHITECTURE_MSIL) && initItem->fieldsp)
                {
                    if (initItem->fieldoffs)
                    {
                        exps = exprNode(en_add, exps, initItem->fieldoffs);
                    }
                    exps = msilThunkSubStructs(exps, expsym, initItem->fieldsp, initItem->offset);
                    exps = exprNode(en_structadd, exps, varNode(en_structelem, initItem->fieldsp));
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
                            exps = exprNode(en_add, exps, intNode(en_c_i, initItem->offset));
                            exps->init = true;
                        }
                    }
                }
                if (exps->type != en_msil_array_access)
                    deref(initItem->basetp, &exps);
                optimize_for_constants(&exps);
                exp = initItem->exp;
                if (exp->type == en_void)
                {
                    cast(initItem->basetp, &exp->right);
                    if (expsym)
                        exp->right = exprNode(en_assign, exps, exp->right);
                }
                else
                {
                    if (isarithmetic(initItem->basetp) || ispointer(initItem->basetp))
                        cast(initItem->basetp, &exp);
                    if (exps)
                        exp = exprNode(en_assign, exps, exp);
                }
            }
            if (sym && sym->sb->init && isatomic(initItem->basetp) && needsAtomicLockFromType(initItem->basetp))
            {
                EXPRESSION* p1 = exprNode(en_add, expsym->left, intNode(en_c_i, initItem->basetp->size - ATOMIC_FLAG_SPACE));
                deref(&stdint, &p1);
                p1 = exprNode(en_assign, p1, intNode(en_c_i, 0));
                exp = exprNode(en_void, exp, p1);
            }
        }
        if (exp)
        {
            if (*pos)
            {
                *pos = exprNode(en_void, *pos, exp);
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
        if (fexp->type == en_thisref)
            fexp = fexp->left->v.func->thisptr;
        exp = exprNode(en_blockclear, fexp, nullptr);
        exp->size = tp;
        rv = exprNode(en_void, exp, rv);
    }
    if (sym && sym->sb->storage_class == sc_localstatic && !(Optimizer::architecture == ARCHITECTURE_MSIL))
    {
        SYMBOL* guardfunc = namespacesearch("__static_guard", globalNameSpace, false, false);
        if (guardfunc)
        {
            guardfunc = guardfunc->tp->syms->front();
            EXPRESSION* guard = anonymousVar(sc_localstatic, &stdint);
            insertInitSym(guard->v.sp);
            deref(&stdpointer, &guard);
            optimize_for_constants(&rv);
            rv = destructLocal(rv);
            rv = addLocalDestructor(rv, sym);
            EXPRESSION* guardexp = exprNode(en_func, nullptr, nullptr);
            guardexp->v.func = Allocate<FUNCTIONCALL>();
            guardexp->v.func->sp = guardfunc;
            guardexp->v.func->functp = guardfunc->tp;
            guardexp->v.func->fcall = varNode(en_pc, guardfunc);
            guardexp->v.func->ascall = true;
            guardexp->v.func->arguments = initListListFactory.CreateList();
            auto arg = Allocate<INITLIST>();
            arg->tp = &stdpointer;
            arg->exp = guard->left;
            guardexp->v.func->arguments->push_back(arg);
            rv = exprNode(en_voidnz,
                          exprNode(en_void,
                                   exprNode(en_land, exprNode(en_ne, guard, intNode(en_c_i, -1)),
                                            exprNode(en_ne, guardexp, intNode(en_c_i, 0))),
                                   exprNode(en_void, rv, exprNode(en_assign, guard, intNode(en_c_i, -1)))),
                          intNode(en_c_i, 0));
        }
    }
    if (isstructured(tp))
    {
        if (*pos)
        {
            *pos = exprNode(en_void, *pos, expsym);
            pos = &(*pos)->right;
        }
        else
        {
            *pos = expsym;
        }
    }
    if (!rv)
        rv = intNode(en_c_i, 0);

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
                case bt_const:
                    destc = true;
                case bt_va_list:
                case bt_objectArray:
                case bt_restrict:
                case bt_volatile:
                case bt_static:
                case bt_typedef:
                case bt_lrqual:
                case bt_rrqual:
                case bt_derivedfromtemplate:
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
                case bt_const:
                    sourcc = true;
                case bt_va_list:
                case bt_objectArray:
                case bt_restrict:
                case bt_volatile:
                case bt_static:
                case bt_typedef:
                case bt_lrqual:
                case bt_rrqual:
                    source = source->btp;
                    break;
                default:
                    done = true;
                    break;
            }
        }
        if (sourcc && !destc)
            return true;
        if (source->type != bt_pointer || dest->type != bt_pointer)
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
        case en_add:
        case en_arrayadd:
        case en_structadd:
            return (isconstaddress(exp->left) || isintconst(exp->left)) && (isconstaddress(exp->right) || isintconst(exp->right));
        case en_global:
        case en_pc:
        case en_labcon:
            return true;
        case en_func:
            return !exp->v.func->ascall;
        case en_threadlocal:
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
static TYPE* inttype(enum e_bt t1)
{
    switch (t1)
    {
        case bt_char:
            return &stdchar;
        case bt_unsigned_char:
            return &stdunsignedchar;
        case bt_signed_char:
            return &stdsignedchar;
        case bt_short:
            return &stdshort;
        case bt_unsigned_short:
            return &stdunsignedshort;
        case bt_wchar_t:
            return &stdwidechar;
        default:
        case bt_int:
        case bt_inative:
            return &stdint;
        case bt_char16_t:
            return &stdchar16t;
        case bt_char32_t:
            return &stdchar32t;
        case bt_unsigned:
        case bt_unative:
            return &stdunsigned;
        case bt_long:
            return &stdlong;
        case bt_unsigned_long:
            return &stdunsignedlong;
        case bt_long_long:
            return &stdlonglong;
        case bt_unsigned_long_long:
            return &stdunsignedlonglong;
    }
}
inline e_bt btmax(e_bt left, e_bt right) { return left > right ? left : right; }
TYPE* destSize(TYPE* tp1, TYPE* tp2, EXPRESSION** exp1, EXPRESSION** exp2, bool minimizeInt, TYPE* atp)
/*
 * compare two types and determine if they are compatible for purposes
 * of the current operation.  Return an appropriate type.  Also checks for
 * dangerous pointer conversions...
 */
{
    int isctp1, isctp2;
    if (tp1->type == bt_any)
        return tp1;
    if (tp2->type == bt_any)
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
    isctp1 = isarithmetic(tp1) || tp1->type == bt_enum;
    isctp2 = isarithmetic(tp2) || tp2->type == bt_enum;

    /*    if (isctp1 && isctp2 && tp1->type == tp2->type)
            return tp1 ;
    */
    if (tp1->type >= bt_float || tp2->type >= bt_float)
    {
        TYPE* tp = nullptr;
        int isim1 = tp1->type >= bt_float_imaginary && tp1->type <= bt_long_double_imaginary;
        int isim2 = tp2->type >= bt_float_imaginary && tp2->type <= bt_long_double_imaginary;
        int iscx1 = tp1->type >= bt_float_complex && tp1->type <= bt_long_double_complex;
        int iscx2 = tp2->type >= bt_float_complex && tp2->type <= bt_long_double_complex;
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
                if (tp1->type - bt_float_complex >= tp2->type - bt_float_imaginary)
                    tp = tp1;
                else
                    tp = &stddoublecomplex;
            }
            else if (tp2->type >= bt_float)
            {
                if (tp1->type - bt_float_complex >= tp2->type - bt_float)
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
                if (tp1->type - bt_float_imaginary <= tp2->type - bt_float_complex)
                    tp = tp2;
                else
                    tp = &stddoublecomplex;
            }
            else if (tp2->type >= bt_float)
            {
                if (tp1->type - bt_float <= tp2->type - bt_float_complex)
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
            else if (tp2->type >= bt_float)
            {
                if (tp1->type - bt_float_imaginary >= tp2->type - bt_float)
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
            else if (tp1->type >= bt_float)
            {
                if (tp1->type - bt_float <= tp2->type - bt_float_imaginary)
                    tp = tp2;
                else
                    tp = &stddoubleimaginary;
            }
            else
            {
                tp = tp2;
            }
        }
        else if (tp1->type >= bt_float && tp2->type >= bt_float)
        {
            if (tp1->type > tp2->type)
                tp = tp1;
            else
                tp = tp2;
        }
        else if (tp1->type >= bt_float)
            tp = tp1;
        else
            tp = tp2;
        if (exp1 && tp->type != tp1->type && exp1 && tp2->type != bt_pointer)
            cast(tp, exp1);
        if (exp2 && tp->type != tp2->type && exp1 && tp1->type != bt_pointer)
            cast(tp, exp2);
        return tp;
    }
    if (isctp1 && isctp2)
    {
        TYPE* rv;
        enum e_bt t1, t2;
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
        if (t1 == bt_enum)
            t1 = bt_int;
        if (t2 == bt_enum)
            t2 = bt_int;
        if (t1 == bt_wchar_t)
            t1 = bt_unsigned;
        if (t2 == bt_wchar_t)
            t2 = bt_unsigned;
        if (t1 < bt_int)
            t1 = bt_int;
        if (t2 < bt_int)
            t2 = bt_int;
        t1 = btmax(t1, t2);
        rv = inttype(t1);
        if (exp1 && rv->type != tp1->type && exp1)
            cast(rv, exp1);
        if (exp2 && rv->type != tp2->type && exp2)
            cast(rv, exp2);
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions)
        {
            if (tp1->type == bt_enum)
                return tp1;
            else if (tp2->type == bt_enum)
                return tp2;
        }
        return rv;
    }
    else
    { /* have a pointer or other exceptional case*/
        if (tp1->type == bt_void && tp2->type == bt_void)
            return tp1;
        if (tp1->type <= bt_unsigned_long_long && ispointer(tp2))
        {
            if (!ispointer(tp1))
                cast(tp2, exp1);
            return tp2;
        }
        if (tp2->type <= bt_unsigned_long_long && ispointer(tp1))
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
    if (exp->preincdec || exp->type == en_autoinc || exp->type == en_autodec)
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
            if (c->type == en_func)
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
            result = anonymousVar(sc_auto, &stdpointer);
            deref(&stdpointer, &result);
            exp2 = exprNode(en_assign, result, exp2);
            exp2 = exprNode(exp->type, exp2, nullptr);
            result = exprNode(exp->type, result, nullptr);
            result = exprNode(en_void, exp2, result);
        }
    }
    return result;
}
}  // namespace Parser
