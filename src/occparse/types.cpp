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
#include "template.h"
#include "initbackend.h"
#include "unmangle.h"
#include "OptUtils.h"
#include "help.h"
#include "ccerr.h"
#include "template.h"
#include "declare.h"
#include "declcpp.h"
#include "symtab.h"

namespace Parser
{
TYPE* typenum(char* buf, TYPE* tp);

static TYPE* replaceTemplateSelector(TYPE* tp)
{
    if (!templateNestingCount && tp->type == BasicType::templateselector && (*tp->sp->sb->templateSelector)[1].isTemplate)
    {
        SYMBOL* sp2 = (*tp->sp->sb->templateSelector)[1].sp;
        if ((*tp->sp->sb->templateSelector)[1].isDeclType)
        {
            TYPE* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
            if (tp1 && isstructured(tp1))
                sp2 = basetype(tp1)->sp;
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
bool comparetypes(TYPE* typ1, TYPE* typ2, int exact)
{
    if (typ1->type == BasicType::any || typ2->type == BasicType::any)
        return true;
    while (typ1->type == BasicType::typedef_)
        typ1 = basetype(typ1);
    while (typ2->type == BasicType::typedef_)
        typ2 = basetype(typ2);
    if (typ1->type == BasicType::derivedfromtemplate)
        typ1 = typ1->btp;
    if (typ2->type == BasicType::derivedfromtemplate)
        typ2 = typ2->btp;
    while (isref(typ1))
        typ1 = basetype(typ1)->btp;
    while (isref(typ2))
        typ2 = basetype(typ2)->btp;
    while (typ1->type == BasicType::typedef_)
        typ1 = basetype(typ1);
    while (typ2->type == BasicType::typedef_)
        typ2 = basetype(typ2);
    if (typ1->type == BasicType::templateselector && typ2->type == BasicType::templateselector)
        return templateselectorcompare(typ1->sp->sb->templateSelector, typ2->sp->sb->templateSelector);
    if (typ1->type == BasicType::templatedecltype && typ2->type == BasicType::templatedecltype)
        return templatecompareexpressions(typ1->templateDeclType, typ2->templateDeclType);
    if (ispointer(typ1) && ispointer(typ2))
    {
        if (exact)
        {
            int arr = false;
            int first = true;
            while (ispointer(typ1) && ispointer(typ2))
            {
                if (!first && (exact == 1))
                    if ((isconst(typ2) && !isconst(typ1)) || (isvolatile(typ2) && !isvolatile(typ1)))
                        return false;
                first = false;
                typ1 = basetype(typ1);
                typ2 = basetype(typ2);
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
            if (exact == 1 && ((isconst(typ2) && !isconst(typ1)) || (isvolatile(typ2) && !isvolatile(typ1))))
                return false;
            return comparetypes(typ1, typ2, true);
        }

        else
            return true;
    }
    typ1 = basetype(typ1);
    typ2 = basetype(typ2);
    if (exact && (isfunction(typ1) || isfuncptr(typ1)) && (isfunction(typ2) || isfuncptr(typ2)))
    {
        typ1 = basetype(typ1);
        typ2 = basetype(typ2);
        if (ispointer(typ1))
            typ1 = basetype(typ1)->btp;
        if (ispointer(typ2))
            typ2 = basetype(typ2)->btp;
        if (isref(typ1->btp) != isref(typ2->btp) || !comparetypes(typ1->btp, typ2->btp, exact))
            return false;
        if (!matchOverload(typ1, typ2, true))
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
                return isint(typ2);
        }
        else if (typ2->type == BasicType::enum_)
        {
            return isint(typ1);
        }
        if (typ1->type == typ2->type && typ1->type == BasicType::memberptr)
        {
            if (typ1->sp != typ2->sp)
            {
                if (classRefCount(typ1->sp, typ2->sp) != 1)
                    return false;
            }
            return comparetypes(typ1->btp, typ2->btp, exact);
        }
    }
    if (typ1->type == typ2->type && typ1->type == BasicType::__string)
        return true;
    if (typ1->type == BasicType::__object)  // object matches anything
        return true;
    if (typ1->type == typ2->type && (isstructured(typ1) || (exact && typ1->type == BasicType::enum_)))
    {
        SYMBOL* s1 = typ1->sp;
        SYMBOL* s2 = typ2->sp;
        if (s1->sb && s1->sb->mainsym)
            s1 = s1->sb->mainsym;
        if (s2->sb && s2->sb->mainsym)
            s2 = s2->sb->mainsym;
        return s1 == s2;
    }
    if (typ1->type == typ2->type || (!exact && isarithmetic(typ2) && isarithmetic(typ1)))
        return true;
    if (isfunction(typ1) && isfunction(typ2) &&
        typ1->sp->sb->attribs.inheritable.linkage == typ2->sp->sb->attribs.inheritable.linkage)
        return true;
    else if (!exact && ((ispointer(typ1) && (isfuncptr(typ2) || isfunction(typ2) || isint(typ2))) ||
                        (ispointer(typ2) && (isfuncptr(typ1) || isfunction(typ1) || isint(typ1)))))
        return (true);
    else if (typ1->type == BasicType::enum_ && isint(typ2))
    {
        return true;
    }
    else if (typ2->type == BasicType::enum_ && isint(typ1))
    {
        return true;
    }
    return false;
}
bool matchingCharTypes(TYPE* typ1, TYPE* typ2)
{
    if (isref(typ1))
        typ1 = basetype(typ1)->btp;
    if (isref(typ2))
        typ2 = basetype(typ2)->btp;

    while (ispointer(typ1) && ispointer(typ2))
    {
        typ1 = basetype(typ1)->btp;
        typ2 = basetype(typ2)->btp;
    }
    typ1 = basetype(typ1);
    typ2 = basetype(typ2);
    if (typ1->type == BasicType::char_)
    {
        if (Optimizer::cparams.prm_charisunsigned)
        {
            if (typ2->type == BasicType::unsigned_char)
                return true;
        }
        else
        {
            if (typ2->type == BasicType::signed_char)
                return true;
        }
    }
    else if (typ2->type == BasicType::char_)
    {
        if (Optimizer::cparams.prm_charisunsigned)
        {
            if (typ1->type == BasicType::unsigned_char)
                return true;
        }
        else
        {
            if (typ1->type == BasicType::signed_char)
                return true;
        }
    }
    else if (typ1->type == BasicType::unsigned_short || typ1->type == BasicType::wchar_t_)
        return typ2->type == BasicType::unsigned_short || typ2->type == BasicType::wchar_t_;
    return false;
}
static char* putpointer(char* p, TYPE* tp)
{
    *p = 0;
    if (tp->type == BasicType::far)
        Optimizer::my_sprintf(p, "far ");
    p = p + strlen(p);
    if (tp->array)
        if (tp->btp->size && (!tp->esize || tp->esize->type == ExpressionNode::c_i))
        {
            Optimizer::my_sprintf(p, "[%d]", tp->size / tp->btp->size);
        }
        else
        {
            Optimizer::my_sprintf(p, "[]");
        }
    else if (tp->vla)
        Optimizer::my_sprintf(p, "[*]");
    else
        Optimizer::my_sprintf(p, " *");
    return p + strlen(p);
}

static TYPE* enumConst(char* buf, TYPE* tp)
{
    while (tp && (isconst(tp) || isvolatile(tp) || isrestrict(tp) || tp->type == BasicType::derivedfromtemplate))
    {
        switch (tp->type)
        {
            case BasicType::lrqual:
                strcat(buf, "& ");
                break;
            case BasicType::rrqual:
                strcat(buf, "&& ");
                break;
            case BasicType::const_:
                strcat(buf, tn_const);
                break;
            case BasicType::volatile_:
                strcat(buf, tn_volatile);
                break;
            case BasicType::restrict_:
            case BasicType::derivedfromtemplate:
                /*				strcat(buf, tn_restrict); */
                break;
            default:
                break;
        }
        tp = tp->btp;
    }
    return tp;
}
void typenumptr(char* buf, TYPE* tp)
{
    char bf[256], *p = bf;
    p = putpointer(p, tp);
    tp = enumConst(buf, tp->btp);
    if (!tp)
        return;
    while (ispointer(tp))
    {
        p = putpointer(p, tp);
        tp = enumConst(buf, tp->btp);
        //		tp = tp->btp;
    }
    typenum(buf, tp);
    strcat(buf, bf);
}
void RenderExpr(char* buf, EXPRESSION* exp)
{
    (void)exp;
    strcpy(buf, "decltype(...)");
}
TYPE* typenum(char* buf, TYPE* tp)
{
    SYMBOL* sym;
    char name[4096];
    if (tp == nullptr)
    {
        diag("typenum - nullptr type");
        return &stdvoid;
    }
    if (tp->type == BasicType::derivedfromtemplate)
        tp = tp->btp;
    tp = enumConst(buf, tp);
    if (!tp)
        return nullptr;
    buf += strlen(buf);
    switch (tp->type)
    {
        case BasicType::typedef_:
            return typenum(buf, tp->btp);
            break;
        case BasicType::aggregate:
            if (!tp->syms)
                break;
            sym = tp->syms->front();
            if (tp->syms->size() > 1 || !strcmp(sym->name, tp->sp->name))  // the tail is to prevent a problem when there are a lot of errors
            {
                strcpy(buf, " (*)(\?\?\?)");
                break;
            }
            tp = sym->tp;
            /* fall through */
        case BasicType::func:
        case BasicType::ifunc:
            typenum(buf, tp->btp);
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
                        getcls(buf, basetype(basetype(thisptr->tp)->btp)->sp);
                        strcat(buf, "::*)(");
                        buf += strlen(buf);
                        ++it;
                    }
                    else
                    {
                        strcat(buf, " (*)(");
                        buf += strlen(buf);
                    }
                }
                else
                {
                    strcat(buf, " (*)(");
                    buf += strlen(buf);
                }
                bool cleanup = it != tp->syms->end();
                while (it != tp->syms->end())
                {
                    sym = *it;
                    *buf = 0;
                    typenum(buf, sym->tp);
                    buf = buf + strlen(buf);
                    ++it;
                }
                if (cleanup)
                    buf--;
            }
            else
            {
                strcat(buf, " (*)(");
                buf += strlen(buf);
            }
            *buf++ = ')';
            *buf = 0;
            break;
        case BasicType::float__complex:
            strcpy(buf, tn_floatcomplex);
            break;
        case BasicType::double__complex:
            strcpy(buf, tn_doublecomplex);
            break;
        case BasicType::long_double_complex:
            strcpy(buf, tn_longdoublecomplex);
            break;
        case BasicType::float__imaginary:
            strcpy(buf, tn_floatimaginary);
            break;
        case BasicType::double__imaginary:
            strcpy(buf, tn_doubleimaginary);
            break;
        case BasicType::long_double_imaginary:
            strcpy(buf, tn_longdoubleimaginary);
            break;
        case BasicType::float_:
            strcpy(buf, tn_float);
            break;
        case BasicType::double_:
            strcpy(buf, tn_double);
            break;
        case BasicType::long_double:
            strcpy(buf, tn_longdouble);
            break;
        case BasicType::unsigned_:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::int_:
            strcpy(buf, tn_int);
            break;
        case BasicType::char16_t_:
            strcpy(buf, tn_char16_t);
            break;
        case BasicType::char32_t_:
            strcpy(buf, tn_char32_t);
            break;
        case BasicType::unsigned_long_long:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::long_long:
            strcpy(buf, tn_longlong);
            break;
        case BasicType::unsigned_long:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::long_:
            strcpy(buf, tn_long);
            break;
        case BasicType::wchar_t_:
            strcpy(buf, tn_wchar_t);
            break;
        case BasicType::unsigned_short:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::short_:
            strcpy(buf, tn_short);
            break;
        case BasicType::signed_char:
            strcpy(buf, tn_signed);
            buf = buf + strlen(buf);
            strcpy(buf, tn_char);
            break;
        case BasicType::unsigned_char:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::char_:
            strcpy(buf, tn_char);
            break;
        case BasicType::bool_:
            strcpy(buf, tn_bool);
            break;
        case BasicType::bit:
            strcpy(buf, "bit");
            break;
        case BasicType::inative:
            strcpy(buf, "native int");
            break;
        case BasicType::unative:
            strcpy(buf, "native unsigned int");
            break;
        case BasicType::void_:
            strcpy(buf, tn_void);
            break;
        case BasicType::__string:
            strcpy(buf, "__string");
            break;
        case BasicType::__object:
            strcpy(buf, "__object");
            break;
        case BasicType::pointer:
            if (tp->nullptrType)
            {
                strcpy(buf, "nullptr_t");
            }
            else
            {
                typenumptr(buf, tp);
            }
            break;
        case BasicType::memberptr:
            if (isfunction(basetype(tp)->btp))
            {
                TYPE* func = basetype(tp)->btp;
                typenum(buf, basetype(func)->btp);
                strcat(buf, " (");
                buf += strlen(buf);
                getcls(buf, tp->sp);
                buf += strlen(buf);
                strcpy(buf, "::*)(");
                buf += strlen(buf);
                if (basetype(func)->syms)
                {
                    for (auto sym : *basetype(func)->syms)
                    {
                        *buf = 0;
                        typenum(buf, sym->tp);
                        buf = buf + strlen(buf);
                    }
                    if (basetype(func)->syms->size())
                        buf--;
                }
                *buf++ = ')';
                *buf = 0;
            }
            else
            {
                typenum(buf, tp->btp);
                strcat(buf, " ");
                buf += strlen(buf);
                getcls(buf, tp->sp);
                buf += strlen(buf);
                strcpy(buf, "::*");
            }
            break;
        case BasicType::seg:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            strcpy(buf, " _seg *");
            break;
        case BasicType::lref:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf = 0;
            break;
        case BasicType::rref:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf++ = '&';
            *buf = 0;
            break;
        case BasicType::ellipse:
            strcpy(buf, tn_ellipse);
            break;
        case BasicType::any:
            strcpy(buf, "???");
            break;
        case BasicType::class_:
            /*                strcpy(buf, tn_class); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::struct_:
            /*                strcpy(buf, tn_struct); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::union_:
            /*                strcpy(buf, tn_union); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::enum_:
            /*                strcpy(buf, tn_enum);  */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::templateselector: {
            auto itts = tp->sp->sb->templateSelector->begin();
            ++itts;
            if (itts->sp)
            {
                strcpy(buf, itts->sp->name);
                for (++itts; itts != tp->sp->sb->templateSelector->end(); ++itts)
                {
                    strcat(buf, "::");
                    strcat(buf, itts->name);
                }
            }
            break;
        }
        case BasicType::templatedecltype:
            RenderExpr(buf, tp->templateDeclType);
            break;
        case BasicType::auto_:
            strcpy(buf, "auto ");
            break;
        default:
            strcpy(buf, "\?\?\?");
    }
    return 0;
}
void typeToString(char* buf, TYPE* typ)
{
    *buf = 0;
    while (typ)
    {
        typ = typenum(buf, typ);
        buf = buf + strlen(buf);
        if (typ)
            *buf++ = ',';
    }
    *buf = 0;
}
}  // namespace Parser