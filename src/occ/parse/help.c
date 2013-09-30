/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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

extern COMPILER_PARAMS cparams;

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
extern int nextLabel;

BOOL istype(enum e_sc storageClass)
{
    return storageClass == sc_type || storageClass == sc_typedef;
}
BOOL ismemberdata(SYMBOL *sp)
{
    (void)sp;
    return TRUE;
}
BOOL startOfType(LEXEME *lex)
{
    if (!lex)
        return FALSE;
    if (lex->type == l_id || MATCHKW(lex, classsel))
    {
        SYMBOL *sp;
        nestedSearch(lex, &sp, NULL, NULL, NULL, FALSE);
        if (cparams.prm_cplusplus)
            backupsym(0);
        return sp && istype(sp->storage_class);
    }
    else 
    {
        return KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS);
    }
    
}
TYPE *basetype(TYPE *tp)
{
    do 
    {
        switch(tp->type)
        {
            case bt_far:
            case bt_near:
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
                tp = tp->btp;
                break;
            default:
                return tp;
        }
    }
    while (tp);
    return NULL;
}
BOOL isunsigned(TYPE *tp)
{
    tp = basetype(tp);
    switch (tp->type)
    {
        case bt_bool:
        case bt_unsigned:
        case bt_unsigned_short:
        case bt_unsigned_char:
        case bt_unsigned_long:
        case bt_unsigned_long_long:
            return TRUE;
        default:
            return FALSE;
    }	
}
BOOL isint(TYPE *tp)
{
    tp = basetype(tp);
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
        case bt_long:
        case bt_unsigned_long:
        case bt_long_long:
        case bt_unsigned_long_long:
        case bt_wchar_t:
            return TRUE;
        default:
            if (tp->type == bt_enum && !cparams.prm_cplusplus)
                return TRUE;

            return FALSE;
    }
}
BOOL isfloat(TYPE *tp)
{
    tp = basetype(tp);
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
BOOL iscomplex(TYPE *tp)
{
    tp = basetype(tp);
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
BOOL isimaginary(TYPE *tp)
{
    tp = basetype(tp);
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
BOOL isarithmetic(TYPE *tp)
{
    tp = basetype(tp);
    return isint(tp) || isfloat(tp) || iscomplex(tp) || isimaginary(tp);
}
BOOL isconst(TYPE *tp)
{
    while (TRUE)
    {
        switch(tp->type)
        {
            case bt_restrict:
            case bt_volatile:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
                tp = tp->btp;
                break;
            case bt_const:
                return TRUE;
            default:
                return FALSE;
        }
    }
}
BOOL isvolatile(TYPE *tp)
{
    while (TRUE)
    {
        switch(tp->type)
        {
            case bt_restrict:
            case bt_const:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
                tp = tp->btp;
                break;
            case bt_volatile:
                return TRUE;
            default:
                return FALSE;
        }
    }
}
BOOL isrestrict(TYPE *tp)
{
    while (TRUE)
    {
        switch(tp->type)
        {
            case bt_volatile:
            case bt_const:
            case bt_static:
            case bt_atomic:
            case bt_typedef:
            case bt_far:
            case bt_near:
                tp = tp->btp;
                break;
            case bt_restrict:
                return TRUE;
            default:
                return FALSE;
        }
    }
}
BOOL isatomic(TYPE *tp)
{
    while (tp)
    {
        switch(tp->type)
        {
            case bt_volatile:
            case bt_const:
            case bt_static:
            case bt_restrict:
            case bt_typedef:
            case bt_far:
            case bt_near:
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
BOOL isvoid(TYPE *tp)
{
    tp = basetype(tp);
    return tp->type == bt_void;
}
BOOL ispointer(TYPE *tp)
{
    tp = basetype(tp);
    switch(tp->type)
    {
        case bt_far:
        case bt_pointer:
        case bt_seg:
            return TRUE;
        default:
            return FALSE;
    }
}
BOOL isref(TYPE *tp)
{
    tp = basetype(tp);
    switch(tp->type)
    {
        case bt_lref:
            return TRUE;
        case bt_rref:
            return TRUE;
        default:
            return FALSE;
    }
}
BOOL isvoidptr(TYPE *tp)
{
    tp = basetype(tp);
    return ispointer(tp) && isvoid(tp->btp);
}
BOOL isfunction(TYPE *tp)
{
    tp = basetype(tp);
    return tp->type == bt_func || tp->type == bt_ifunc;
}
BOOL isfuncptr(TYPE *tp)
{
    tp = basetype(tp);
    return ispointer(tp) && tp->btp && isfunction(tp->btp);
}
BOOL isarray(TYPE *tp)
{
    tp = basetype(tp);
    return ispointer(tp) && tp->array;		
}
BOOL isunion(TYPE *tp)
{
    tp = basetype(tp);
    return tp->type == bt_union;
}
BOOL isstructured(TYPE *tp)
{
    tp = basetype(tp);
    switch(tp->type)
    {
        case bt_struct:
        case bt_union:
        case bt_class:
            return TRUE;
        default:
            return FALSE;
    }
}
SYMBOL *getFunctionSP(TYPE **tp)
{
    TYPE *btp = basetype(*tp);
    BOOL pointer = ispointer(btp);
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
    while (lex && (lex->type == l_astr || lex->type == l_wstr || lex->type == l_ustr || lex->type == l_Ustr))
    {
        
        if (lex->type == l_Ustr)
            type = l_Ustr;
        else if (type != l_Ustr && lex->type == l_ustr)
            type = l_ustr;
        else if (type != l_Ustr && type != l_ustr && lex->type == l_wstr)
            type = l_wstr;
        if (lex->suffix)
        {
            if (suffix)
            {
                if (strcmp(lex->suffix, suffix))
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
        list[pos++] = lex->value.s.w;
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
BOOL isintconst(EXPRESSION *exp)
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
BOOL isfloatconst(EXPRESSION *exp)
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
BOOL isimaginaryconst(EXPRESSION *exp)
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
BOOL iscomplexconst(EXPRESSION *exp)
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
SYMBOL *anonymousVar(enum e_sc storage_class, TYPE *tp)
{
    SYMBOL *rv = (SYMBOL *)Alloc(sizeof(SYMBOL));
    rv->storage_class = storage_class;
    rv->tp = tp;
    rv->anonymous = TRUE;
    rv->allocate = TRUE;
    rv->assigned = TRUE;
    rv->name = AnonymousName();
    InsertSymbol(rv, storage_class, FALSE);
    SetLinkerNames(rv, lk_none);
    return rv;
}
void deref(TYPE *tp, EXPRESSION **exp)
{
    enum e_node en = en_l_i;
    BOOL rref = FALSE;
    tp = basetype(tp);
    switch ((tp->type == bt_enum && tp->btp ) ? tp->btp->type : tp->type)
    {
        case bt_lref: /* only used during initialization */
            en = en_l_ref;
            break;
        case bt_rref: /* only used during initialization */
            en = en_l_ref;
            rref = TRUE;
            break;
        case bt_bit:
            en = en_l_bit;
            break;
        case bt_bool:
            en = en_l_bool;
            break;
        case bt_char:
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
        case bt_pointer:
            if (tp->array || tp->vla)
                return;
            en = en_l_p;
            break;
        case bt_struct:
        case bt_class:
        case bt_union:
        case bt_func:
        case bt_ifunc:
        case bt_any:
            return;
        default:
            diag("deref error");
            break;
    }
    *exp = exprNode(en, *exp, NULL);
    (*exp)->rref = rref;
}
int sizeFromType(TYPE *tp)
{
    int rv = -ISZ_UINT;
    tp = basetype(tp);
    switch (tp->type == bt_enum ? tp->btp->type : tp->type)
    {
        case bt_void:
            rv = ISZ_UINT;
            break;
        case bt_bool:
            rv = ISZ_BOOL;
            break;
        case bt_char:
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
        case bt_unsigned:
            rv = ISZ_UINT;
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
            rv = ISZ_ADDR;
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
        case bt_unsigned:
            en = en_x_ui;
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
        case bt_pointer:
            en = en_x_p;
            break;
        case bt_void:
            break;
        default:
            diag("cast error");
            break;
    }
    *exp = exprNode(en, *exp, NULL);
}
BOOL castvalue(EXPRESSION *exp)
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
            return TRUE;
        default:
            return FALSE;
    }
}
BOOL xvalue(EXPRESSION *exp)
{
    // fixme...
    return FALSE;
}
BOOL lvalue(EXPRESSION *exp)
{
    if (!cparams.prm_ansi)
        while (castvalue(exp))
            exp = exp->left;
    switch (exp->type)
    {
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
            return TRUE;
        case en_l_ref:
            return !exp->rref;
        default:
            return FALSE;
    }
}
EXPRESSION *convertInitToExpression(TYPE *tp, SYMBOL *sp, SYMBOL *funcsp, INITIALIZER *init, EXPRESSION *thisptr)
{
    EXPRESSION *exp = NULL, **expp;
    EXPRESSION *expsym;
    if (!init->exp)
        return intNode(en_c_i, 0);// must be an error
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
            expsym = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p); // this ptr
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
            expsym = varNode(en_auto, sp);
            break;
        case sc_localstatic:
            if (sp->linkage3 == lk_threadlocal)
            {
                expsym = exprNode(en_add, thisptr, intNode(en_c_i, sp->offset));
            }
            else
            {
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
                expsym = varNode(en_global, sp);
            }
            break;
        case sc_member:
            if (thisptr)
                expsym = thisptr;
            else if (funcsp)
                expsym = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p); // this ptr
            else
            {
                expsym = intNode(en_c_i, 0);
                diag("convertInitToExpression: no this ptr");
            }
            expsym = exprNode(en_add, expsym, intNode(en_c_i, sp->offset));
            break;
        case sc_external:
/*			expsym = varNode(en_global, sp);
            break;
*/
        default:
            diag("convertInitToExpression: unknown sym type");
            expsym = intNode(en_c_i, 0);
            break;
    }	
    if (init->noassign)
    {
        exp = init->exp;
        if (thisptr && exp->type == en_func)
            exp->v.func->thisptr = expsym;
    }
    else if (isstructured(tp) || isarray(tp))
    {
        INITIALIZER *temp = init;
        if (isstructured(temp->basetp))
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
                exp = exprNode(en_blockassign, expsym, exp2);
                exp->size = tp->size;
            }
        }
        else
        {
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
                    sp = anonymousVar(sc_auto, tp);
                    expsym = varNode(en_auto, sp);
                }
                exp = exprNode(en_blockclear, expsym, NULL);
                exp->size = tp->size;
                exp = exprNode(en_void, exp, NULL);
                expp = &exp->right;
                while (init)
                {
                    if (init->exp)
                    {
                        EXPRESSION *asn = exprNode(en_add, expsym, intNode(en_c_i, init->offset));
                        EXPRESSION *right = init->exp;
                        deref(init->basetp, &asn);
                        cast(init->basetp, &right);
                        *expp = exprNode(en_assign, asn, right);
                        *expp = exprNode(en_void, *expp, NULL);
                        expp = &(*expp)->right;
                    }
                    init = init->next;
                }
            }
            else
            {
                /* constant expression */
                SYMBOL *spc ;
                IncGlobalFlag();
                spc = anonymousVar(sc_localstatic, tp);
                spc->init = init ;
                insertInitSym(spc);
                DecGlobalFlag();
                exp = varNode(en_label, spc);
                spc->label =nextLabel++;
                if (expsym)
                {
                    exp = exprNode(en_blockassign, expsym, exp);
                    exp->size = tp->size;
                }
            }
        }
    }
    else if (basetype(tp)->type == bt_memberptr)
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
                int lab = dumpMemberPtr(exp2->v.sp, tp, TRUE);
                exp2 = intNode(en_labcon, lab);
            }
            exp = exprNode(en_blockassign, expsym, exp2);
            exp->size = tp->size;
        }
    }
    else
    {
        deref(tp, &expsym);
        exp = init->exp;
        if (exp->type == en_void)
        {
            cast(tp, &exp->right);
            if (expsym)
                exp->right = exprNode(en_assign, expsym, exp->right);
        }
        else
        {
            cast(tp, &exp);
            if (expsym)
                exp = exprNode(en_assign, expsym, exp);
        }
    }
    if (sp && sp->init && isatomic(tp) && needsAtomicLockFromType(tp))
    {
        EXPRESSION *p1 = exprNode(en_add, expsym->left, intNode(en_c_i, tp->size - ATOMIC_FLAG_SPACE));
        deref(&stdint, &p1);
        p1 = exprNode(en_assign, p1, intNode(en_c_i, 0));
        exp = exprNode(en_void, exp, p1);
    }
    return exp;
}
BOOL assignDiscardsConst(TYPE *dest, TYPE *source)
{
    if (!ispointer(source) || !ispointer(dest))
        return FALSE;
    while (TRUE)
    {
        BOOL destc = FALSE;
        BOOL sourcc = FALSE;
        BOOL done = FALSE;
        while (!done)
        {
            switch(dest->type)
            {
                case bt_const:
                    destc = TRUE;
                case bt_restrict:
                case bt_volatile:
                case bt_static:
                case bt_typedef:
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
                case bt_restrict:
                case bt_volatile:
                case bt_static:
                case bt_typedef:
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
BOOL isconstzero(TYPE *tp, EXPRESSION *exp)
{
    (void)tp;
    return (isintconst(exp) && exp->v.i == 0);
}
BOOL isarithmeticconst(EXPRESSION *exp)
{
    return isintconst(exp) || isfloatconst(exp) || isimaginaryconst(exp) ||
        iscomplexconst(exp);
}
BOOL isconstaddress(EXPRESSION *exp)
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
            return isconstaddress(exp->v.func->fcall);
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
            return &stdint;
        case bt_char16_t:
            return &stdchar16t;
        case bt_char32_t:
            return &stdchar32t;
        case bt_unsigned:
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
TYPE *destSize(TYPE *tp1, TYPE *tp2, EXPRESSION **exp1, EXPRESSION **exp2, BOOL minimizeInt, TYPE *atp)
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
    if (isvoid(tp1) || isvoid(tp2))
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
        if (t1 < bt_int)
            t1= bt_int;
        if (t2 < bt_int)
            t2= bt_int;
        t1 = max(t1, t2);
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