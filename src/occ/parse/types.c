/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"

extern int templateNestingCount;
extern TYPE stdvoid;

extern char *tn_void;
extern char *tn_bool;
extern char *tn_char;
extern char *tn_int;
extern char *tn_long;
extern char *tn_longlong;
extern char *tn_short;
extern char *tn_unsigned;
extern char *tn_signed;
extern char *tn_ellipse;
extern char *tn_floatimaginary;
extern char *tn_doubleimaginary;
extern char *tn_longdoubleimaginary;
extern char *tn_floatcomplex;
extern char *tn_doublecomplex;
extern char *tn_longdoublecomplex;
extern char *tn_float;
extern char *tn_double;
extern char *tn_longdouble;
extern char *tn_const;
extern char *tn_volatile;
extern char *tn_class;
extern char *tn_union;
extern char *tn_struct;
extern char *tn_wchar_t;
extern char *tn_char16_t;
extern char *tn_char32_t;

TYPE *typenum(char *buf, TYPE *tp);

static TYPE *replaceTemplateSelector (TYPE *tp)
{
    if (!templateNestingCount && tp->type == bt_templateselector && tp->sp->templateSelector->next->isTemplate)
    {
        SYMBOL *sp2 = tp->sp->templateSelector->next->sym;
        if (sp2)
        {
            SYMBOL *sp1 = GetClassTemplate(sp2, tp->sp->templateSelector->next->templateParams, TRUE);
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
BOOLEAN comparetypes(TYPE *typ1, TYPE *typ2, int exact)
{
    if (typ1->type == bt_any || typ2->type == bt_any)
        return TRUE;
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
            int arr = FALSE;
            int first = TRUE;
            while (ispointer(typ1) && ispointer(typ2))
            {
                if (!first && (exact == 1))
                    if ((isconst(typ2) && !isconst(typ1)) || (isvolatile(typ2) && !isvolatile(typ1)))
                        return FALSE;
                first = FALSE;
                typ1 = basetype(typ1);
                typ2 = basetype(typ2);
                if (typ1->type != typ2->type)
                    return FALSE;
                if (arr && typ1->array != typ2->array)	
                    return FALSE;
                if (arr && typ1->size != typ2->size)
                    return FALSE;
                arr |= typ1->array | typ2->array;
                typ1 = typ1->btp;
                typ2 = typ2->btp;				
            }
            if (exact == 1 && ((isconst(typ2) && !isconst(typ1)) || (isvolatile(typ2) && !isvolatile(typ1))))
                return FALSE;
            return comparetypes(typ1, typ2, TRUE);
        }
            
        else
            return TRUE;
    }
    typ1 = basetype(typ1);
    typ2 = basetype(typ2);
    if (exact && (isfunction(typ1) || isfuncptr(typ1)) && (isfunction(typ2) || isfuncptr(typ2)))
    {
        HASHREC *hr1;
        HASHREC *hr2;
        typ1 = basetype(typ1);
        typ2 = basetype(typ2);
        if (ispointer(typ1))
            typ1 = basetype(typ1)->btp;
        if (ispointer(typ2))
            typ2 = basetype(typ2)->btp;
        if (!comparetypes(typ1->btp, typ2->btp, exact))
            return FALSE;
        if (!matchOverload(typ1, typ2, TRUE))
            return FALSE;
        return TRUE;
    }
    if (cparams.prm_cplusplus)
    {
        if (typ1->scoped != typ2->scoped)
            return FALSE;
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
                    return FALSE;
            }
            return comparetypes(typ1->btp, typ2->btp, exact);
        }
    }
    if (typ1->type == typ2->type && typ1->type == bt___string)
        return TRUE;
    if (typ1->type == bt___object) // object matches anything
        return TRUE;
    if (typ1->type == typ2->type && (isstructured(typ1) || (exact && typ1->type == bt_enum)))
        return typ1->sp == typ2->sp;
    if (typ1->type == typ2->type || (!exact && isarithmetic(typ2) && isarithmetic(typ1)))
        return TRUE;
    if (isfunction(typ1) && isfunction(typ2) && 
        typ1->sp->linkage == typ2->sp->linkage)
        return TRUE;
    else if (!exact && ((ispointer(typ1) && (isfuncptr(typ2) || isfunction(typ2) || isint(typ2)))
             || (ispointer(typ2) && (isfuncptr(typ1) || isfunction(typ1) || isint(typ1)))))
            return (TRUE);
    else if (typ1->type == bt_enum && isint(typ2))
    {
        return TRUE;
    }
    else if (typ2->type == bt_enum && isint(typ1))
    {
        return TRUE;
    }
    return FALSE;
}
BOOLEAN matchingCharTypes(TYPE *typ1, TYPE *typ2)
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
                return TRUE;
        }
        else
        {
            if (typ2->type == bt_signed_char)
                return TRUE;
        }
    }
    else if (typ2->type == bt_char)
    {   
        if (cparams.prm_charisunsigned)
        {
            if (typ1->type == bt_unsigned_char)
                return TRUE;
        }
        else
        {
            if (typ1->type == bt_signed_char)
                return TRUE;
        }
    }
    return FALSE;
}
static char *putpointer(char *p, TYPE *tp)
{
    *p = 0;
    if (tp->type == bt_far)
        my_sprintf(p,"far ");
    p = p +strlen(p);
    if (tp->array)
        if (tp->btp->size && (! tp->esize || tp->esize->type == en_c_i))
        {
            my_sprintf(p,"[%d]",tp->size/tp->btp->size);
        }
        else
        {
            my_sprintf(p,"[]");
        }
    else if (tp->vla)
        my_sprintf(p, "[*]");
    else
        my_sprintf(p," *");
    return p + strlen(p);
}
    

static TYPE *enumConst(char *buf, TYPE *tp)
{
    while (tp && (isconst(tp) || isvolatile(tp) || isrestrict(tp) || tp->type == bt_derivedfromtemplate))
    {
        switch(tp->type)
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
void typenumptr(char *buf, TYPE *tp)
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
void RenderExpr(char *buf, EXPRESSION *exp)
{
    strcpy (buf, "decltype(...)");
}
TYPE *typenum(char *buf, TYPE *tp)
{
    SYMBOL *sp;
    HASHREC *hr;
    char name[4096];
    if (tp == NULL)
    {
        diag("typenum - NULL type");
        return &stdvoid;
    }
    if (tp->type == bt_derivedfromtemplate)
        tp = tp->btp;
    tp = enumConst(buf, tp);
    if (!tp)
        return NULL;
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
            sp = (SYMBOL *)hr->p;
            if (hr->next || !strcmp(sp->name, tp->sp->name)) // the tail is to prevent a problem when there are a lot of errors
            {
                strcpy(buf," (*)(\?\?\?)");
                break;
            }
            tp = sp->tp;
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
                    if (((SYMBOL *)hr->p)->thisPtr)
                    {
                        SYMBOL *thisptr = (SYMBOL *)hr->p;
                        *buf++ = ' ';
                        *buf++='(';
                        getcls(buf, basetype (basetype(thisptr->tp)->btp)->sp);
                        strcat(buf, "::*)(");
                        buf += strlen(buf);
                        hr = hr->next;
                    }
                    else
                    {
                       strcat(buf," (*)(");
                        buf += strlen(buf);
                    }
                }
                else
                {
                    strcat(buf," (*)(");
                    buf += strlen(buf);
                }
                while (hr)
                {
                    sp = (SYMBOL *)hr->p;
                    *buf = 0;
                    typenum(buf, sp->tp);
                    buf = buf + strlen(buf);
                    hr = hr->next;
                    if (hr)
                        *buf++ = ',';
                }
            }
            else
            {
                strcat(buf," (*)(");
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
                TYPE *func = basetype(tp)->btp;
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
                        sp = (SYMBOL *)hr->p;
                        *buf = 0;
                        typenum(buf, sp->tp);
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
            TEMPLATESELECTOR *ts = tp->sp->templateSelector->next;
            if (ts->sym)
            {
                strcpy(buf, ts->sym->name);
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
void typeToString(char *buf, TYPE *typ)
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
