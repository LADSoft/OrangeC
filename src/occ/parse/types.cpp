/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

extern int templateNestingCount;
extern TYPE stdvoid;

extern const char* tn_void;
extern const char* tn_bool;
extern const char* tn_char;
extern const char* tn_int;
extern const char* tn_long;
extern const char* tn_longlong;
extern const char* tn_short;
extern const char* tn_unsigned;
extern const char* tn_signed;
extern const char* tn_ellipse;
extern const char* tn_floatimaginary;
extern const char* tn_doubleimaginary;
extern const char* tn_longdoubleimaginary;
extern const char* tn_floatcomplex;
extern const char* tn_doublecomplex;
extern const char* tn_longdoublecomplex;
extern const char* tn_float;
extern const char* tn_double;
extern const char* tn_longdouble;
extern const char* tn_const;
extern const char* tn_volatile;
extern const char* tn_class;
extern const char* tn_union;
extern const char* tn_struct;
extern const char* tn_wchar_t;
extern const char* tn_char16_t;
extern const char* tn_char32_t;

TYPE* typenum(char* buf, TYPE* tp);

static TYPE* replaceTemplateSelector(TYPE* tp)
{
    if (!templateNestingCount && tp->type == bt_templateselector && tp->sp->templateSelector->next->isTemplate)
    {
        SYMBOL* sp2 = tp->sp->templateSelector->next->sp;
        if (sp2)
        {
            SYMBOL* sp1 = GetClassTemplate(sp2, tp->sp->templateSelector->next->templateParams, true);
            if (sp1)
            {
                sp1 = search(tp->sp->templateSelector->next->next->name, sp1->tp->syms);
                if (sp1)
                {
                    tp = sp1->tp;
                }
            }
        }
    }
    return tp;
}
bool comparetypes(TYPE* typ1, TYPE* typ2, int exact)
{
    if (typ1->type == bt_any || typ2->type == bt_any)
        return true;
    while (typ1->type == bt_typedef)
        typ1 = basetype(typ1);
    while (typ2->type == bt_typedef)
        typ2 = basetype(typ2);
    typ1 = replaceTemplateSelector(typ1);
    typ2 = replaceTemplateSelector(typ2);
    if (typ1->type == bt_derivedfromtemplate)
        typ1 = typ1->btp;
    if (typ2->type == bt_derivedfromtemplate)
        typ2 = typ2->btp;
    while (isref(typ1))
        typ1 = basetype(typ1)->btp;
    while (isref(typ2))
        typ2 = basetype(typ2)->btp;
    while (typ1->type == bt_typedef)
        typ1 = basetype(typ1);
    while (typ2->type == bt_typedef)
        typ2 = basetype(typ2);
    if (typ1->type == bt_templateselector && typ2->type == bt_templateselector)
        return templateselectorcompare(typ1->sp->templateSelector, typ2->sp->templateSelector);
    if (typ1->type == bt_templatedecltype && typ2->type == bt_templatedecltype)
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
        if (!comparetypes(typ1->btp, typ2->btp, exact))
            return false;
        if (!matchOverload(typ1, typ2, true))
            return false;
        return true;
    }
    if (cparams.prm_cplusplus)
    {
        if (typ1->scoped != typ2->scoped)
            return false;
        if (typ1->type == bt_enum)
        {
            if (typ2->type == bt_enum)
                return typ1->sp == typ2->sp;
            else
                return isint(typ2);
        }
        else if (typ2->type == bt_enum)
        {
            return isint(typ1);
        }
        if (typ1->type == typ2->type && typ1->type == bt_memberptr)
        {
            if (typ1->sp != typ2->sp)
            {
                if (classRefCount(typ1->sp, typ2->sp) != 1)
                    return false;
            }
            return comparetypes(typ1->btp, typ2->btp, exact);
        }
    }
    if (typ1->type == typ2->type && typ1->type == bt___string)
        return true;
    if (typ1->type == bt___object)  // object matches anything
        return true;
    if (typ1->type == typ2->type && (isstructured(typ1) || (exact && typ1->type == bt_enum)))
        return typ1->sp == typ2->sp;
    if (typ1->type == typ2->type || (!exact && isarithmetic(typ2) && isarithmetic(typ1)))
        return true;
    if (isfunction(typ1) && isfunction(typ2) && typ1->sp->attribs.inheritable.linkage == typ2->sp->attribs.inheritable.linkage)
        return true;
    else if (!exact && ((ispointer(typ1) && (isfuncptr(typ2) || isfunction(typ2) || isint(typ2))) ||
                        (ispointer(typ2) && (isfuncptr(typ1) || isfunction(typ1) || isint(typ1)))))
        return (true);
    else if (typ1->type == bt_enum && isint(typ2))
    {
        return true;
    }
    else if (typ2->type == bt_enum && isint(typ1))
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
    if (typ1->type == bt_char)
    {
        if (cparams.prm_charisunsigned)
        {
            if (typ2->type == bt_unsigned_char)
                return true;
        }
        else
        {
            if (typ2->type == bt_signed_char)
                return true;
        }
    }
    else if (typ2->type == bt_char)
    {
        if (cparams.prm_charisunsigned)
        {
            if (typ1->type == bt_unsigned_char)
                return true;
        }
        else
        {
            if (typ1->type == bt_signed_char)
                return true;
        }
    }
    else if (typ1->type == bt_unsigned_short || typ1->type == bt_wchar_t)
        return typ2->type == bt_unsigned_short || typ2->type == bt_wchar_t;
    return false;
}
static char* putpointer(char* p, TYPE* tp)
{
    *p = 0;
    if (tp->type == bt_far)
        my_sprintf(p, "far ");
    p = p + strlen(p);
    if (tp->array)
        if (tp->btp->size && (!tp->esize || tp->esize->type == en_c_i))
        {
            my_sprintf(p, "[%d]", tp->size / tp->btp->size);
        }
        else
        {
            my_sprintf(p, "[]");
        }
    else if (tp->vla)
        my_sprintf(p, "[*]");
    else
        my_sprintf(p, " *");
    return p + strlen(p);
}

static TYPE* enumConst(char* buf, TYPE* tp)
{
    while (tp && (isconst(tp) || isvolatile(tp) || isrestrict(tp) || tp->type == bt_derivedfromtemplate))
    {
        switch (tp->type)
        {
            case bt_lrqual:
                strcat(buf, "& ");
                break;
            case bt_rrqual:
                strcat(buf, "&& ");
                break;
            case bt_const:
                strcat(buf, tn_const);
                break;
            case bt_volatile:
                strcat(buf, tn_volatile);
                break;
            case bt_restrict:
            case bt_derivedfromtemplate:
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
    SYMLIST* hr;
    char name[4096];
    if (tp == nullptr)
    {
        diag("typenum - nullptr type");
        return &stdvoid;
    }
    if (tp->type == bt_derivedfromtemplate)
        tp = tp->btp;
    tp = enumConst(buf, tp);
    if (!tp)
        return nullptr;
    buf += strlen(buf);
    switch (tp->type)
    {
        case bt_typedef:
            strcpy(buf, tp->sp->name);
            break;
        case bt_aggregate:
            if (!tp->syms)
                break;
            hr = tp->syms->table[0];
            sym = hr->p;
            if (hr->next || !strcmp(sym->name, tp->sp->name))  // the tail is to prevent a problem when there are a lot of errors
            {
                strcpy(buf, " (*)(\?\?\?)");
                break;
            }
            tp = sym->tp;
            /* fall through */
        case bt_func:
        case bt_ifunc:
            typenum(buf, tp->btp);
            buf = buf + strlen(buf);
            if (tp->syms)
            {
                hr = tp->syms->table[0];
                if (hr && hr->p)
                {
                    if (hr->p->thisPtr)
                    {
                        SYMBOL* thisptr = hr->p;
                        *buf++ = ' ';
                        *buf++ = '(';
                        getcls(buf, basetype(basetype(thisptr->tp)->btp)->sp);
                        strcat(buf, "::*)(");
                        buf += strlen(buf);
                        hr = hr->next;
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
                while (hr)
                {
                    sym = hr->p;
                    *buf = 0;
                    typenum(buf, sym->tp);
                    buf = buf + strlen(buf);
                    hr = hr->next;
                    if (hr)
                        *buf++ = ',';
                }
            }
            else
            {
                strcat(buf, " (*)(");
                buf += strlen(buf);
            }
            *buf++ = ')';
            *buf = 0;
            break;
        case bt_float_complex:
            strcpy(buf, tn_floatcomplex);
            break;
        case bt_double_complex:
            strcpy(buf, tn_doublecomplex);
            break;
        case bt_long_double_complex:
            strcpy(buf, tn_longdoublecomplex);
            break;
        case bt_float_imaginary:
            strcpy(buf, tn_floatimaginary);
            break;
        case bt_double_imaginary:
            strcpy(buf, tn_doubleimaginary);
            break;
        case bt_long_double_imaginary:
            strcpy(buf, tn_longdoubleimaginary);
            break;
        case bt_float:
            strcpy(buf, tn_float);
            break;
        case bt_double:
            strcpy(buf, tn_double);
            break;
        case bt_long_double:
            strcpy(buf, tn_longdouble);
            break;
        case bt_unsigned:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case bt_int:
            strcpy(buf, tn_int);
            break;
        case bt_char16_t:
            strcpy(buf, tn_char16_t);
            break;
        case bt_char32_t:
            strcpy(buf, tn_char32_t);
            break;
        case bt_unsigned_long_long:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case bt_long_long:
            strcpy(buf, tn_longlong);
            break;
        case bt_unsigned_long:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case bt_long:
            strcpy(buf, tn_long);
            break;
        case bt_wchar_t:
            strcpy(buf, tn_wchar_t);
            break;
        case bt_unsigned_short:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case bt_short:
            strcpy(buf, tn_short);
            break;
        case bt_signed_char:
            strcpy(buf, tn_signed);
            buf = buf + strlen(buf);
            strcpy(buf, tn_char);
            break;
        case bt_unsigned_char:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case bt_char:
            strcpy(buf, tn_char);
            break;
        case bt_bool:
            strcpy(buf, tn_bool);
            break;
        case bt_bit:
            strcpy(buf, "bit");
            break;
        case bt_inative:
            strcpy(buf, "native int");
            break;
        case bt_unative:
            strcpy(buf, "native unsigned int");
            break;
        case bt_void:
            strcpy(buf, tn_void);
            break;
        case bt___string:
            strcpy(buf, "__string");
            break;
        case bt___object:
            strcpy(buf, "__object");
            break;
        case bt_pointer:
            if (tp->nullptrType)
            {
                strcpy(buf, "nullptr_t");
            }
            else
            {
                typenumptr(buf, tp);
            }
            break;
        case bt_memberptr:
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
                    hr = basetype(func)->syms->table[0];
                    while (hr)
                    {
                        sym = hr->p;
                        *buf = 0;
                        typenum(buf, sym->tp);
                        buf = buf + strlen(buf);
                        hr = hr->next;
                        if (hr)
                            *buf++ = ',';
                    }
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
        case bt_seg:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            strcpy(buf, " _seg *");
            break;
        case bt_lref:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf = 0;
            break;
        case bt_rref:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf++ = '&';
            *buf = 0;
            break;
        case bt_ellipse:
            strcpy(buf, tn_ellipse);
            break;
        case bt_any:
            strcpy(buf, "???");
            break;
        case bt_class:
            /*                strcpy(buf, tn_class); */
            unmangle(name, tp->sp->errname ? tp->sp->errname : tp->sp->name);
            strcpy(buf, name);
            break;
        case bt_struct:
            /*                strcpy(buf, tn_struct); */
            unmangle(name, tp->sp->errname ? tp->sp->errname : tp->sp->name);
            strcpy(buf, name);
            break;
        case bt_union:
            /*                strcpy(buf, tn_union); */
            unmangle(name, tp->sp->errname ? tp->sp->errname : tp->sp->name);
            strcpy(buf, name);
            break;
        case bt_enum:
            /*                strcpy(buf, tn_enum);  */
            unmangle(name, tp->sp->errname ? tp->sp->errname : tp->sp->name);
            strcpy(buf, name);
            break;
        case bt_templateselector:
        {
            TEMPLATESELECTOR* ts = tp->sp->templateSelector->next;
            if (ts->sp)
            {
                strcpy(buf, ts->sp->name);
                ts = ts->next;
                while (ts)
                {
                    strcat(buf, "::");
                    strcat(buf, ts->name);
                    ts = ts->next;
                }
            }
            break;
        }
        case bt_templatedecltype:
            RenderExpr(buf, tp->templateDeclType);
            break;
        case bt_auto:
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
