/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided tthat the following 
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
#include <stdarg.h>

extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;

extern TYPE stdchar;
extern TYPE stdunsignedchar;
extern TYPE stdshort;
extern TYPE stdunsignedshort;
extern TYPE stdint;
extern TYPE stdlong;
extern TYPE stdlonglong;
extern TYPE stdunsigned;
extern TYPE stdunsignedlong;
extern TYPE stdunsignedlonglong;
extern TYPE stdfloatcomplex;
extern TYPE stddoublecomplex;
extern TYPE stdlongdoublecomplex;
extern TYPE stdchar16t;
extern TYPE stdchar32t;
extern TYPE stdpointer;
extern int nextLabel;
extern SYMBOL *theCurrentFunc;
extern NAMESPACEVALUES *localNameSpace;
extern int inDefaultParam;
extern LINEDATA *linesHead, *linesTail;

int anonymousNotAlloc;

void helpinit()
{
    anonymousNotAlloc = 0;
}
void deprecateMessage(SYMBOL *sp)
{
    char buf[1024];
    my_sprintf(buf, "%s deprecated", sp->name);
    if (sp->deprecationText && sp->deprecationText != (char *)-1)
        my_sprintf(buf + strlen(buf), "; %s", sp->deprecationText);
    errorstr(ERR_WARNING, buf);
}
// well this is really only nonstatic data members...
BOOLEAN ismember(SYMBOL *sym)
{
    switch (sym->storage_class)
    {
        case sc_member:
        case sc_mutable:
        case sc_virtual:
            return TRUE;
        default:
            return FALSE;
    }
}
BOOLEAN istype(SYMBOL *sym)
{
    if (sym->storage_class == sc_templateparam)
    {
        return sym->tp->templateParam->p->type == kw_typename || sym->tp->templateParam->p->type == kw_template;
    }
    return sym->tp->type != bt_templateselector && sym->storage_class == sc_type || sym->storage_class == sc_typedef;
}
BOOLEAN ismemberdata(SYMBOL *sp)
{
    return !isfunction(sp->tp) && ismember(sp);
}
BOOLEAN startOfType(LEXEME *lex, BOOLEAN assumeType)
{
    LINEDATA *oldHead = linesHead, *oldTail = linesTail; 
    if (!lex)
        return FALSE;
       
    if (lex->type == l_id)
    { 
        TEMPLATEPARAMLIST *tparam = TemplateLookupSpecializationParam(lex->value.s.a);
        if (tparam)
        {
            linesHead = oldHead;
            linesTail = oldTail;
            return tparam->p->type == kw_typename || tparam->p->type == kw_template;
        }
    }
    if (lex->type == l_id || MATCHKW(lex, classsel))
    {
        SYMBOL *sp, *strSym = NULL;
        LEXEME *placeholder = lex;
        BOOLEAN dest = FALSE;
        nestedSearch(lex, &sp, &strSym, NULL, &dest, NULL, FALSE, sc_global, FALSE, FALSE);
        if (cparams.prm_cplusplus || chosenAssembler->msil)
            prevsym(placeholder);
        linesHead = oldHead;
        linesTail = oldTail;
        return (sp && istype(sp)) || (assumeType && strSym && (strSym->tp->type == bt_templateselector || strSym->tp->type == bt_templatedecltype));
    }
    else 
    {
        linesHead = oldHead;
        linesTail = oldTail;
        return KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS | TT_TYPENAME);
    }
    
}
static TYPE *rootType(TYPE *tp)
{
    while(tp) 
    {
        switch(tp->type)
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
    return NULL;
}
void UpdateRootTypes(TYPE *tp)
{
    while (tp)
    {
        TYPE *tp1 = rootType(tp);
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
BOOLEAN isDerivedFromTemplate(TYPE *tp)
{
    while (tp)
    {
        switch(tp->type)
        {
            case bt_pointer:
            case bt_func:
            case bt_ifunc:
                return FALSE;
            case bt_derivedfromtemplate:
                return TRUE;
        }
        tp = tp->btp;
    }
    return FALSE;
}
BOOLEAN isautotype(TYPE *tp)
{
    if (isref(tp))
        tp = basetype(tp)->btp;
    return basetype(tp)->type == bt_auto;
}
BOOLEAN isunsigned(TYPE *tp)
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
                return TRUE;
            default:
                return FALSE;
        }	
    }
    return FALSE;
}
BOOLEAN isint(TYPE *tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch(tp->type)
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
                return TRUE;
            case bt_templateparam:
                if (tp->templateParam->p->type == kw_int)
                    return isint(tp->templateParam->p->byNonType.tp);
                return FALSE;
            default:
                if (tp->type == bt_enum && !cparams.prm_cplusplus)
                    return TRUE;
    
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN isfloat(TYPE *tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
            case bt_float:
            case bt_double:
            case bt_long_double:
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN iscomplex(TYPE *tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
            case bt_float_complex:
            case bt_double_complex:
            case bt_long_double_complex:
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN isimaginary(TYPE *tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
            case bt_float_imaginary:
            case bt_double_imaginary:
            case bt_long_double_imaginary:
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN isarithmetic(TYPE *tp)
{
    tp = basetype(tp);
    return isint(tp) || isfloat(tp) || iscomplex(tp) || isimaginary(tp);
}
BOOLEAN ismsil(TYPE *tp)
{
    tp = basetype(tp);
    return tp->type == bt___string || tp->type == bt___object;
}
BOOLEAN isconstraw(TYPE *tp, BOOLEAN useTemplate)
{
	BOOLEAN done = FALSE;
	BOOLEAN rv = FALSE;
    while (!done && tp)
    {
        if (useTemplate)
        {
            if (tp->templateConst)
			{
				rv = TRUE;
				done = TRUE;
			}
        }
        switch(tp->type)
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
				rv = TRUE;
				done = TRUE;
				break;
            default:
				done = TRUE;
				break;
        }
    }
	return rv;
}
BOOLEAN isconst(TYPE *tp)
{
    return isconstraw(tp, FALSE);
}
BOOLEAN isvolatile(TYPE *tp)
{
    while (tp)
    {
        switch(tp->type)
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
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN islrqual(TYPE *tp)
{
    while (tp)
    {
        switch(tp->type)
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
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN isrrqual(TYPE *tp)
{
    while (tp)
    {
        switch(tp->type)
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
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN isrestrict(TYPE *tp)
{
    while (tp)
    {
        switch(tp->type)
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
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN isatomic(TYPE *tp)
{
    while (tp)
    {
        switch(tp->type)
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
                return TRUE;
            default:
                return FALSE;
        }
    }
    return FALSE;
}
BOOLEAN isvoid(TYPE *tp)
{
    tp = basetype(tp);
    if (tp)
        return tp->type == bt_void;
    return FALSE;
}
BOOLEAN isvoidptr(TYPE *tp)
{
    tp = basetype(tp);
    return ispointer(tp) && isvoid(tp->btp);
}
BOOLEAN isarray(TYPE *tp)
{
    tp = basetype(tp);
    if (tp)
        return ispointer(tp) && tp->array;		
    return FALSE;
}
BOOLEAN isunion(TYPE *tp)
{
    tp = basetype(tp);
    if (tp)
        return tp->type == bt_union;
    return FALSE;
}
TYPE *assignauto(TYPE *pat, TYPE *nt)
{
    if (isautotype(pat))
    {
        if (isref(pat))
        {
            if (isref(nt))
                basetype(pat)->btp = basetype(nt)->btp;
            else
                basetype(pat)->btp = nt;
        }
        else
        {
            if (pat->decltypeauto)
                if (pat->decltypeautoextended)
                {
                    pat = (TYPE *)Alloc(sizeof(TYPE));
                    pat->type = bt_lref;
                    pat->size = getSize(bt_pointer);
                    pat->btp = nt;
                }
                else
                {
                    pat = nt;
                }
            else if (isref(nt))
                pat = basetype(nt)->btp;
            else
                pat = nt;
        }
    }
    return pat;
}
SYMBOL *getFunctionSP(TYPE **tp)
{
    TYPE *btp = basetype(*tp);
    BOOLEAN pointer = ispointer(btp);
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
    return NULL;
}
LEXEME *concatStringsInternal(LEXEME *lex, STRING **str, int *elems)
{
    SLCHAR **list;
    char *suffix = NULL;
    int count = 3;
    int pos = 0 ;
    enum e_lexType type = l_astr;
    STRING *string;
    IncGlobalFlag();
    list = (SLCHAR **)Alloc(sizeof(SLCHAR *) * count);
    while (lex && (lex->type == l_astr || lex->type == l_wstr || lex->type == l_ustr || lex->type == l_Ustr || lex->type == l_msilstr))
    {
        if (lex->type == l_msilstr)
            type = l_msilstr;
        else if (lex->type == l_Ustr)
            type = l_Ustr;
        else if (type != l_Ustr && type != l_msilstr && lex->type == l_ustr)
            type = l_ustr;
        else if (type != l_Ustr && type != l_ustr && type != l_msilstr && lex->type == l_wstr)
            type = l_wstr;
        if (lex->suffix)
        {
            if (suffix)
            {
                if (strcmp(lex->suffix, suffix) != 0)
                    error(ERR_LITERAL_SUFFIX_MISMATCH);
            }
            else
            {
                suffix = lex->suffix;
            }
        }
        if (pos >= count)
        {
            SLCHAR **h = (SLCHAR **)Alloc(sizeof(SLCHAR *) * (count + 10));
            memcpy(h, list, sizeof(SLCHAR *) * count);
            list = h;
            count += 10;
        }
        if (elems)
            *elems += ((SLCHAR *)lex->value.s.w)->count;
        list[pos++] = (SLCHAR *)lex->value.s.w;
        lex = getsym();
    }
    string = (STRING *)Alloc(sizeof(STRING));
    string->strtype = type;
    string->size = pos;
    string->pointers = Alloc(pos * sizeof(SLCHAR * ));
    string->suffix = suffix;
    memcpy(string->pointers, list, pos * sizeof(SLCHAR *));
    *str = string;
    DecGlobalFlag();
    return lex;
}
LEXEME *concatStrings(LEXEME *lex, EXPRESSION **expr, enum e_lexType *tp, int *elems)
{
    STRING *data;
    lex = concatStringsInternal(lex, &data, elems);
    IncGlobalFlag();
    *expr = stringlit(data);
    DecGlobalFlag();
    *tp = data->strtype;
    return lex;
}
BOOLEAN isintconst(EXPRESSION *exp)
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
            return TRUE;
        default:
            return FALSE;
    }
}
BOOLEAN isfloatconst(EXPRESSION *exp)
{
    switch(exp->type)
    {
        case en_c_f:
        case en_c_d:
        case en_c_ld:
            return TRUE;
        default:
            return FALSE;
    }
}
BOOLEAN isimaginaryconst(EXPRESSION *exp)
{
    switch(exp->type)
    {
        case en_c_fi:
        case en_c_di:
        case en_c_ldi:
            return TRUE;
        default:
            return FALSE;
    }
}
BOOLEAN iscomplexconst(EXPRESSION *exp)
{
    switch(exp->type)
    {
        case en_c_fc:
        case en_c_dc:
        case en_c_ldc:
            return TRUE;
        default:
            return FALSE;
    }
}
EXPRESSION *anonymousVar(enum e_sc storage_class, TYPE *tp)
{
    static int anonct = 1;
    char buf[256];
    SYMBOL *rv = (SYMBOL *)Alloc(sizeof(SYMBOL));
//    if ((storage_class == sc_localstatic || storage_class == sc_auto) && !theCurrentFunc)
//    {
//        storage_class = sc_static;
//        insertInitSym(rv);
//    }
    if (tp->size == 0 && isstructured(tp))
        tp = basetype(tp)->sp->tp;
    rv->storage_class = storage_class;
    rv->tp = tp;
    rv->anonymous = TRUE;
    rv->allocate = !anonymousNotAlloc;
    rv->assigned = TRUE;
    rv->used = TRUE;
    if (theCurrentFunc)
        rv->value.i = theCurrentFunc->value.i;
    my_sprintf(buf,"$anontemp%d", anonct++);
    rv->name = litlate(buf);
    tp->size = basetype(tp)->size;
    if (theCurrentFunc && localNameSpace->syms && !inDefaultParam && !anonymousNotAlloc)
        InsertSymbol(rv, storage_class, FALSE, FALSE);
    SetLinkerNames(rv, lk_none);
    return varNode(storage_class == sc_auto || storage_class == sc_parameter ? en_auto : storage_class == sc_localstatic ? en_label : en_global, rv);
}
void deref(TYPE *tp, EXPRESSION **exp)
{
    enum e_node en = en_l_i;
    tp = basetype(tp);
    switch ((tp->type == bt_enum && tp->btp ) ? tp->btp->type : tp->type)
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
            if (cparams.prm_charisunsigned)
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
    *exp = exprNode(en, *exp, NULL);
    if (en == en_l_object)
        (*exp)->v.tp = tp;
}
int sizeFromType(TYPE *tp)
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
            if (cparams.prm_charisunsigned)
                rv = ISZ_UCHAR;
            else
                rv = - ISZ_UCHAR;
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
            rv = - ISZ_ULONG;
            break;
        case bt_unsigned_long:
            rv = ISZ_ULONG;
            break;
        case bt_long_long:
            rv = - ISZ_ULONGLONG;
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
void cast(TYPE *tp, EXPRESSION **exp)
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
            if (cparams.prm_charisunsigned)
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
    *exp = exprNode(en, *exp, NULL);
}
BOOLEAN castvalue(EXPRESSION *exp)
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
            return TRUE;
        default:
            return FALSE;
    }
}
BOOLEAN xvalue(EXPRESSION *exp)
{
    // fixme...
    return FALSE;
}
BOOLEAN lvalue(EXPRESSION *exp)
{
    if (!cparams.prm_ansi)
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
            return TRUE;
        case en_l_ref:
            return TRUE;
        default:
            return FALSE;
    }
}
EXPRESSION *convertInitToExpression(TYPE *tp, SYMBOL *sp, SYMBOL *funcsp, INITIALIZER *init, EXPRESSION *thisptr, BOOLEAN isdest)
{
    BOOLEAN local = FALSE;
    EXPRESSION *rv = NULL, **pos = &rv;
    EXPRESSION *exp = NULL, **expp;
    EXPRESSION *expsym;
    BOOLEAN noClear = FALSE;
    if (sp)
        sp->destructed = FALSE;
    if (isstructured(tp) || isarray(tp))
    {
        INITIALIZER **i2 = &init;
        while (*i2)
            i2 = &(*i2)->next;
        initInsert(i2, NULL, NULL, tp->size, FALSE);
    }
    if (!sp)
    {
        if (thisptr)
            expsym = thisptr;
        else if (funcsp)
        {
            SYMBOL *sp = (SYMBOL *)basetype(funcsp->tp)->syms->table[0] ? (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p : NULL;
            if (sp && sp->thisPtr)
                expsym = varNode(en_auto, sp ); // this ptr
            else
                expsym = anonymousVar(sc_auto, tp);
        }
        else
        {
            expsym = intNode(en_c_i, 0);
            diag("convertInitToExpression: no this ptr");
        }
    }
    else switch (sp->storage_class)
    {
        case sc_auto:
        case sc_register:
        case sc_parameter:
            local = TRUE;
            expsym = varNode(en_auto, sp);
            break;
        case sc_localstatic:
            if (sp->linkage3 == lk_threadlocal)
            {
                expsym = exprNode(en_add, thisptr, intNode(en_c_i, sp->offset));
            }
            else
            {
                local = TRUE;
                expsym = varNode(en_label, sp);
            }
            break;
        case sc_static:
        case sc_global:
            if (sp->linkage3 == lk_threadlocal)
            {
                expsym = exprNode(en_add, thisptr, intNode(en_c_i, sp->offset));
            }
            else
            {
                local = TRUE;
                expsym = varNode(en_global, sp);
            }
            break;
        case sc_member:
        case sc_mutable:
            if (thisptr)
                expsym = thisptr;
            else if (funcsp)
                expsym = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p); // this ptr
            else
            {
                expsym = intNode(en_c_i, 0);
                diag("convertInitToExpression: no this ptr");
            }
            if (chosenAssembler->msil)
                expsym = exprNode(en_structadd, expsym, varNode(en_structelem, sp));
            else
                expsym = exprNode(en_add, expsym, intNode(en_c_i, sp->offset));
            break;
        case sc_external:
/*			expsym = varNode(en_global, sp);
            local = TRUE;
            break;
*/
        case sc_constant:
            return NULL;
        default:
            diag("convertInitToExpression: unknown sym type");
            expsym = intNode(en_c_i, 0);
            break;
    }	
    if (sp && isarray(sp->tp) && sp->tp->msil && !init->noassign)
    {
        exp = intNode(en_msil_array_init, 0);
        exp->v.tp = sp->tp;
        // plop in a newarr call
        *pos = exprNode(en_assign, expsym, exp);
        noClear = TRUE;
    }
    while (init)
    {
        exp = NULL;
        if (init->basetp)
        {
            if (init->noassign && init->exp)
            {
                exp = init->exp;
                if (exp->type == en_thisref)
                    exp = exp->left;
                if (thisptr && exp->type == en_func)
                {
                    EXPRESSION *exp1 = init->offset  || (chosenAssembler->arch->denyopts & DO_UNIQUEIND) ? exprNode(en_add, expsym, intNode(en_c_i, init->offset)) : expsym;
                    if (isarray(tp))
                    {
                        exp->v.func->arguments->exp = exp1;
                    }
                    else
                    {
                        exp->v.func->thisptr = exp1;
                    }
                }
                exp = init->exp;
            }
            else if (!init->exp)
            {
                // usually empty braces, coudl be an error though
                exp = exprNode(en_blockclear, expsym, NULL);
                exp->size = init->offset;
            }
            else if (isstructured(init->basetp) || isarray(init->basetp))
            {
                INITIALIZER *temp = init;
                if (isstructured(temp->basetp))
                {
                    EXPRESSION *exp2 = init->exp;
                    while(exp2->type == en_not_lvalue)
                        exp2 = exp2->left;
                    if (exp2->type == en_func && exp2->v.func->returnSP)
                    {
                        exp2->v.func->returnSP->allocate = FALSE;
                        exp2->v.func->returnEXP = expsym;
                        exp = exp2;
                        noClear = TRUE;
                    }
                    else if (exp2->type == en_thisref && exp2->left->v.func->returnSP)
                    {
                        exp2->left->v.func->returnSP->allocate = FALSE;
                        exp2->left->v.func->returnEXP = expsym;
                        exp = exp2;
                        noClear = TRUE;
                    }
                    else if ((cparams.prm_cplusplus) && !basetype(init->basetp)->sp->trivialCons)
                    {
                        TYPE *ctype = init->basetp;
                        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                        funcparams->arguments = Alloc(sizeof(INITLIST));
                        funcparams->arguments->tp = ctype;
                        funcparams->arguments->exp = exp2;
                        callConstructor(&ctype, &expsym, funcparams, FALSE, NULL, TRUE, FALSE, FALSE, FALSE, FALSE); 
                        exp = expsym;
                    }
                    else 
                    {
                        exp = exprNode(en_blockassign, expsym, exp2);
                        exp->size = init->basetp->size;
                        exp->altdata = (long)(init->basetp);
                        noClear = TRUE;
                    }
                }
                else
                {
                    TYPE *btp = init->basetp;
                    while(isarray(btp))
                        btp = basetype(btp)->btp;
                    btp = basetype(btp);
                    while (temp)
                    {
                        if (temp->exp)
                            if (!isarithmeticconst(temp->exp) && !isconstaddress(temp->exp))
                                break;
                        temp = temp->next;
                    }
                    if (temp)
                    {
                        /* some members are non-constant expressions */
                        if (!cparams.prm_c99 && !cparams.prm_cplusplus)
                            error(ERR_C99_NON_CONSTANT_INITIALIZATION);
                        if (!sp)
                        {
                            expsym = anonymousVar(sc_auto, init->basetp);
                            sp = expsym->v.sp;
                        }
                        if (!isstructured(btp) || btp->sp->trivialCons)
                        {
                            exp = exprNode(en_blockclear, expsym, NULL);
                            exp->size = init->basetp->size;
                            exp = exprNode(en_void, exp, NULL);
                            expp = &exp->right;
                        }
                        else
                        {
                            expp = &exp;
                        }
                        {
                            EXPRESSION *right = init->exp;
                            if (!isstructured(btp))
                            {
                                EXPRESSION *asn = exprNode(en_add, expsym, intNode(en_c_i, init->offset));
                                deref(init->basetp, &asn);
                                cast(init->basetp, &right);
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
                        SYMBOL *spc;
                        IncGlobalFlag();
                        exp = anonymousVar(sc_localstatic, init->basetp);
                        spc = exp->v.sp;
                        spc->init = init;
                        insertInitSym(spc);
                        insert(spc, localNameSpace->syms);
                        DecGlobalFlag();
                        spc->label = nextLabel++;
                        if (expsym)
                        {
                            if (cparams.prm_cplusplus && isstructured(init->basetp) && !init->basetp->sp->trivialCons)
                            {
                                TYPE *ctype = init->basetp;
                                FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                funcparams->arguments = Alloc(sizeof(INITLIST));
                                funcparams->arguments->tp = ctype;
                                funcparams->arguments->exp = exp;
                                callConstructor(&ctype, &expsym, funcparams, FALSE, NULL, TRUE, FALSE, FALSE, FALSE, FALSE);
                                exp = expsym;
                            }
                            else
                            {
                                exp = exprNode(en_blockassign, expsym, exp);
                                exp->size = init->basetp->size;
                                exp->altdata = (long)(init->basetp);
                            }
                        }
                    }
                }
            }
            else if (basetype(init->basetp)->type == bt_memberptr)
            {
                EXPRESSION *exp2 = init->exp;;
                while(exp2->type == en_not_lvalue)
                    exp2 = exp2->left;
                if (exp2->type == en_func && exp2->v.func->returnSP)
                {
                    exp2->v.func->returnSP->allocate = FALSE;
                    exp2->v.func->returnEXP = expsym;
                    exp = exp2;
                }
                else
                {
                    if (exp2->type == en_memberptr)
                    {
                        int lab = dumpMemberPtr(exp2->v.sp, init->basetp, TRUE);
                        exp2 = intNode(en_labcon, lab);
                    }
                    exp = exprNode(en_blockassign, expsym, exp2);
                    exp->size = init->basetp->size;
                    exp->altdata = (long)(init->basetp);
                }
            }
            else
            {
                EXPRESSION *exps = expsym;
                if (isarray(tp) && tp->msil)
                {
                    TYPE *btp = tp;
                    exps = exprNode(en_msil_array_access, NULL, NULL);
                    int count = 0, i;
                    int q = init->offset;
                    while (isarray(btp) && btp->msil)
                    {
                        count++;
                        btp = btp->btp;
                    }
                    exps->v.msilArray = (MSIL_ARRAY *)Alloc(sizeof(MSIL_ARRAY) + count * sizeof(EXPRESSION *));
                    exps->v.msilArray->max = count;
                    exps->v.msilArray->count = count;
                    exps->v.msilArray->base = expsym;
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
                else if (chosenAssembler->msil && init->fieldsp)
                {
                    exps = exprNode(en_add, exps, intNode(en_c_i, init->fieldoffs));
                    exps = exprNode(en_structadd, exps, varNode(en_structelem, init->fieldsp));
                }
                else if (init->offset || init->next && init->next->basetp && (chosenAssembler->arch->denyopts & DO_UNIQUEIND))
                    exps = exprNode(en_add, exps, intNode(en_c_i, init->offset));
                if (exps->type != en_msil_array_access)
                    deref(init->basetp, &exps);
                exp = init->exp;
                if (exp->type == en_void)
                {
                    cast(init->basetp, &exp->right);
                    if (expsym)
                        exp->right = exprNode(en_assign, exps, exp->right);
                }
                else
                {
                    cast(init->basetp, &exp);
                    if (exps)
                        exp = exprNode(en_assign, exps, exp);
                }
            }
            if (sp && sp->init && isatomic(init->basetp) && needsAtomicLockFromType(init->basetp))
            {
                EXPRESSION *p1 = exprNode(en_add, expsym->left, intNode(en_c_i, init->basetp->size - ATOMIC_FLAG_SPACE));
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
        init = init->next;
    }
    if (sp && sp->storage_class == sc_localstatic && !(chosenAssembler->msil))
    {
        if (isdest)
        {
            rv = exprNode(en_voidnz, exprNode(en_void, sp->localInitGuard, rv), intNode(en_c_i, 0));        
        }
        else
        {
            EXPRESSION *guard = anonymousVar(sc_static, &stdint);
            insertInitSym(guard->v.sp);
            deref(&stdpointer, &guard);
            optimize_for_constants(&rv);
            rv = exprNode(en_voidnz, exprNode(en_void, 
                                              exprNode(en_not, guard, NULL), 
                                              exprNode(en_void, rv, exprNode(en_autoinc, guard, intNode(en_c_i, 1)))), intNode(en_c_i, 0));        
            sp->localInitGuard = guard;
        }
    }
    // plop in a clear block if necessary
    if (sp && !noClear && !isdest && (isarray(tp) || isstructured(tp) && (!cparams.prm_cplusplus && !chosenAssembler->msil || basetype(tp)->sp->trivialCons)))
    {
        EXPRESSION *fexp = expsym;
        EXPRESSION *exp;
        if (fexp->type == en_thisref)
            fexp = fexp->left->v.func->thisptr;
        exp = exprNode(en_blockclear, fexp, NULL); 
        exp->size = sp->tp->size;
        rv = exprNode(en_void, exp, rv);
    }
    if (isstructured(tp) && !cparams.prm_cplusplus)
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
    return rv;
}
BOOLEAN assignDiscardsConst(TYPE *dest, TYPE *source)
{
    source = basetype(source);
    dest = basetype(dest);
    if (!ispointer(source) || !ispointer(dest))
        return FALSE;
    while (TRUE)
    {
        BOOLEAN destc = FALSE;
        BOOLEAN sourcc = FALSE;
        BOOLEAN done = FALSE;
        while (!done)
        {
            switch(dest->type)
            {
                case bt_const:
                    destc = TRUE;
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
                    done = TRUE;
                    break;
            }
        }
        done = FALSE;
        while (!done)
        {
            switch(source->type)
            {
                case bt_const:
                    sourcc = TRUE;
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
                    done = TRUE;
                    break;
            }
        }
        if (sourcc && !destc)
            return TRUE;
        if (source->type != bt_pointer || dest->type != bt_pointer)
            return FALSE;
        dest = dest->btp;
        source = source->btp;
    }
}
BOOLEAN isconstzero(TYPE *tp, EXPRESSION *exp)
{
    (void)tp;
    return (isintconst(exp) && exp->v.i == 0);
}
BOOLEAN fittedConst(TYPE *tp, EXPRESSION *exp)
{
    int n;
    if (!isint(tp) || !isintconst(exp))
        return FALSE;
    n = getSize(basetype(tp)->type);
    switch (n)
    {
        case 8:
            if (isunsigned(tp))
            {
                if (exp->v.i < 0 || exp->v.i > 255)
                    return FALSE;
            }
            else
            {
                if (exp->v.i < - 128 || exp->v.i > 127)
                    return FALSE;
            }
            break;
        case 16:
            if (isunsigned(tp))
            {
                if (exp->v.i < 0 || exp->v.i > 65535)
                    return FALSE;
            }
            else
            {
                if (exp->v.i < - 32768 || exp->v.i > 32767)
                    return FALSE;
            }
            break;
        default:
            return TRUE;
    }
}
BOOLEAN isarithmeticconst(EXPRESSION *exp)
{
    return isintconst(exp) || isfloatconst(exp) || isimaginaryconst(exp) ||
        iscomplexconst(exp);
}
BOOLEAN isconstaddress(EXPRESSION *exp)
{
    switch(exp->type)
    {
        case en_add:
        case en_arrayadd:
        case en_structadd:
            return (isconstaddress(exp->left) || isintconst(exp->left))
                    && (isconstaddress(exp->right) || isintconst(exp->right));
        case en_global:
        case en_label:
        case en_pc:
        case en_labcon:
            return TRUE;
        case en_func:
            return !exp->v.func->ascall;
        case en_threadlocal:
        default:
            return FALSE;
    }
}
SYMBOL *clonesym(SYMBOL *sym)
{
    SYMBOL *rv = (SYMBOL *)Alloc(sizeof(SYMBOL));
    *rv = *sym;
    return rv;
}
static TYPE *inttype(enum e_bt t1)
{
       switch(t1) {
       case bt_char:
               return &stdchar;
       case bt_unsigned_char:
               return &stdunsignedchar;
       case bt_short:
               return &stdshort;
       case bt_unsigned_short:
               return &stdunsignedshort;
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
TYPE *destSize(TYPE *tp1, TYPE *tp2, EXPRESSION **exp1, EXPRESSION **exp2, BOOLEAN minimizeInt, TYPE *atp)
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
        error(ERR_NOT_AN_ALLOWED_TYPE);
        return tp1;
    }
    if (isref(tp1))
        tp1 = basetype(tp1)->btp;
    if (isref(tp2))
        tp2 = basetype(tp2)->btp;
    tp1 = basetype(tp1);
    tp2 = basetype(tp2);
    isctp1 = isarithmetic(tp1);
    isctp2 = isarithmetic(tp2);
    
/*    if (isctp1 && isctp2 && tp1->type == tp2->type)
        return tp1 ;
*/
    if (tp1->type >= bt_float || tp2->type >= bt_float) {
    
        int isim1 = tp1->type >= bt_float_imaginary && tp1->type <= bt_long_double_imaginary;
        int isim2 = tp2->type >= bt_float_imaginary && tp2->type <= bt_long_double_imaginary;
        if (isim1 && !isim2 && tp2->type < bt_float_imaginary)
        {
            TYPE *tp ;
            if (tp1->type == bt_long_double_imaginary || tp2->type == bt_long_double)
                tp = &stdlongdoublecomplex;
            else if (tp1->type == bt_double_imaginary || tp2->type == bt_double
                || tp1->type == bt_long_long || tp1->type == bt_unsigned_long_long)
                tp = &stddoublecomplex;
            else
                tp = &stdfloatcomplex;
            if (exp1)
                 cast(tp, exp1);
            if (exp2)
                 cast(tp, exp2);
            return tp;
        }
        else if (isim2 && !isim1 && tp1->type < bt_float_imaginary)
        {
            TYPE *tp ;
            if (tp2->type == bt_long_double_imaginary || tp1->type == bt_long_double)
                tp = &stdlongdoublecomplex;
            else if (tp2->type == bt_double_imaginary || tp1->type == bt_double 
                || tp1->type == bt_long_long || tp1->type == bt_unsigned_long_long)
                tp = &stddoublecomplex;
            else
                tp = &stdfloatcomplex;
            if (exp1)
                 cast(tp, exp1);
            if (exp2)
                 cast(tp, exp2);
            return tp;
        }
        else if (tp1->type > tp2->type)
        {
            if (exp2)
                   cast(tp1, exp2);
        }
        else if (tp1->type < tp2->type)
        {
            if (exp1)
                cast(tp2, exp1);
        }

        if (tp1->type == bt_long_double_complex && isctp2)
            return tp1;
        if (tp2->type == bt_long_double_complex && isctp1)
            return tp2;
        
        if (tp1->type == bt_long_double_imaginary && isim2)
            return tp1;
        if (tp2->type == bt_long_double_imaginary && isim1)
            return tp2;
        if (tp1->type == bt_long_double && isctp2)
            return tp1;
        if (tp2->type == bt_long_double && isctp1)
            return tp2;
        if (tp1->type == bt_double_complex && isctp2)
            return tp1;
        if (tp2->type == bt_double_complex && isctp1)
            return tp2;
        if (tp1->type == bt_double_imaginary && isim2)
            return tp1;
        if (tp2->type == bt_double_imaginary && isim1)
            return tp2;
        if (tp1->type == bt_double && isctp2)
            return tp1;
        if (tp2->type == bt_double && isctp1)
            return tp2;
        if (tp1->type == bt_float_complex && isctp2)
            return tp1;
        if (tp2->type == bt_float_complex && isctp1)
            return tp2;
        if (tp1->type == bt_float_imaginary && isim2)
            return tp1;
        if (tp2->type == bt_float_imaginary && isim1)
            return tp2;
        if (tp1->type == bt_float && isctp2)
            return tp1;
        if (tp2->type == bt_float && isctp1)
            return tp2;
    }
    if (isctp1 && isctp2) {
        TYPE *rv ;
       enum e_bt t1, t2;
       t1 = tp1->type;
       t2 = tp2->type;
       /*
       if (cparams.prm_cplusplus && (t1 == bt_enum || t2 == bt_enum))
       {
           if (t1 == t2)
        {
            if (tp1->sp->mainsym == tp2->sp->mainsym)
            {
                return tp1;
            }
            genmismatcherror(ERR_ENUMMISMATCH, tp1, tp2);
        }			
       }
       */
        if (t1 == bt_enum)
            t1= bt_int;
        if (t2 == bt_enum)
            t2= bt_int;
        if (t1 == bt_wchar_t)
            t1 = bt_unsigned;
        if (t2 == bt_wchar_t)
            t2 = bt_unsigned;
        if (t1 < bt_int)
            t1= bt_int;
        if (t2 < bt_int)
            t2= bt_int;
        t1 = imax(t1, t2);
       rv = inttype(t1);
       if (rv->type != tp1->type && exp1)
         cast(rv, exp1);
       if (rv->type != tp2->type && exp2)
         cast(rv,exp2);
       return rv;
    } else { /* have a pointer or other exceptional case*/
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
        if (isstructured(tp1)) {
            return tp2;
/*
            if (comparetypes(tp1, tp2, FALSE))
                return tp1;
            if (cparams.prm_cplusplus) {
                cppcast(tp2, tp1, exp1, FALSE, ERR_CPPMISMATCH);
            } else

                error(ERR_ILL_STRUCTURE_OPERATION);
            return tp2;
*/
        }
        if (isstructured(tp2)) {
            return tp1;
/*			
            if (comparetypes(tp1, tp2, FALSE))
                return tp2;
            if (cparams.prm_cplusplus) {
                cppcast(tp1, tp2, exp1, FALSE, ERR_CPPMISMATCH);
            } else

                error(ERR_ILL_STRUCTURE_OPERATION);
            return tp1;
*/
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
/*				if (tp1->type != tp2->type || !comparetypes(tp1->btp, tp2->btp, TRUE))
                    generror(ERR_SUSPICIOUS, 0, 0);
*/
                 return tp1;
            }
    }
    return tp1;
}
EXPRESSION *RemoveAutoIncDec(EXPRESSION *exp)
{
    EXPRESSION *newExp;
    if (exp->type == en_autoinc || exp->type == en_autodec)
        return RemoveAutoIncDec(exp->left);
    newExp = Alloc(sizeof(EXPRESSION));
    *newExp = *exp;
    if (newExp->left)
        newExp->left = RemoveAutoIncDec(newExp->left);
    if (newExp->right)
        newExp->right = RemoveAutoIncDec(newExp->right);
    return newExp;
}
LLONG_TYPE imax(LLONG_TYPE x, LLONG_TYPE y)
{
    return x > y ? x : y;
}
LLONG_TYPE imin(LLONG_TYPE x, LLONG_TYPE y)
{
    return x < y ? x : y;
}
static char *write_llong(char *dest, ULLONG_TYPE val)
{
    char obuf[256], *p = obuf + sizeof(obuf);
    *--p = 0;
    if (val)
    {
        while (val)
        {
            unsigned aa = val % 10;
            *--p = '0' + aa;
            val = val / 10;
        }
    }
    else
    {
        *--p = '0';
    }
    strcpy(dest, p);
    return dest + strlen(dest);
}
static char *write_int(char *dest, unsigned val)
{
    char obuf[256], *p = obuf + sizeof(obuf);
    *--p = 0;
    if (val)
    {
        while (val)
        {
            unsigned aa = val % 10;
            *--p = '0' + aa;
            val = val / 10;
        }
    }
    else
    {
        *--p = '0';
    }
    strcpy(dest, p);
    return dest + strlen(dest);
}
void my_sprintf(char *dest, const char *fmt, ...)
{
    va_list aa;
    va_start(aa, fmt);
    while (*fmt)
    {
        char *q = strchr(fmt, '%');
        if (!q)
            q = fmt + strlen(fmt);
        memcpy(dest, fmt, q - fmt);
        dest += q - fmt;
        fmt += q - fmt;
        if (*fmt)
        {
            fmt++;
            switch (*fmt++)
            {
                ULLONG_TYPE val;
                unsigned val1;
                char *str;
                case 'l':
                    while (*fmt == 'd' || *fmt == 'l')
                        fmt ++;
                    val = va_arg(aa, ULLONG_TYPE);
                    dest = write_llong(dest, val);
                    break;
                case 'd':
                case 'u':
                    val1 = va_arg(aa, unsigned);
                    dest = write_int(dest, val1);
                    break;
                case 'c':
                    val1 = va_arg(aa, unsigned);
                    *dest++ = val1;
                    break;
                case 's':
                    str = va_arg(aa, char *);
                    strcpy(dest, str);
                    dest += strlen(dest);
                    break;
                default:
                    fmt++;
                    break;
            }
        }
    }
    *dest = 0;
}

