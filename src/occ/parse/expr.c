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
#include "rtti.h"

extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;
extern int stdpragmas;
extern enum e_kw skim_closepa[], skim_end[];
extern enum e_kw skim_closebr[];
extern enum e_kw skim_semi[];
extern TYPE stdpointer, stdnullpointer, stdchar;
extern TYPE stdint, stdany;
extern TYPE stddouble;
extern TYPE stdvoid, stdfunc;
extern TYPE stdwcharptr;
extern TYPE stdcharptr;
extern TYPE stdfloatimaginary;
extern TYPE stdbool;
extern TYPE stdunsigned;
extern TYPE stdunsignedlong;
extern TYPE stdlong;
extern TYPE stdunsignedlonglong;
extern TYPE stdlonglong;
extern TYPE stdfloat;
extern TYPE stdlongdouble;
extern TYPE stdchar;
extern TYPE stdwidechar;
extern TYPE std__func__;
extern TYPE std__string;
extern TYPE stdchar16tptr;
extern TYPE stdchar32tptr;
extern BOOLEAN setjmp_used;
extern char *overloadNameTab[];
extern NAMESPACEVALUES *localNameSpace;
extern LAMBDA *lambdas;
extern int instantiatingTemplate;
extern int currentErrorLine;
extern int templateNestingCount;
extern INCLUDES *includes;
extern NAMESPACEVALUES *globalNameSpace;
extern BOOLEAN hasXCInfo;
extern STRUCTSYM *structSyms;
extern int anonymousNotAlloc;
extern int expandingParams;
extern BOOLEAN functionCanThrow;
int packIndex;

int argument_nesting;

LIST *importThunks;
/* lvaule */
/* handling of const int */
/*-------------------------------------------------------------------------------------------------------------------------------- */
static EXPRESSION *nodeSizeof(TYPE *tp, EXPRESSION *exp);
static LEXEME *expression_primary(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags);
LEXEME *expression_assign(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags);
static LEXEME *expression_comma(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags);
static LEXEME *expression_msilfunc(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags);

void expr_init(void)
{
    packIndex = -1;
    importThunks = NULL;
    if (chosenAssembler->msil)
        overloadNameTab[CI_CONSTRUCTOR] = ".ctor";
}
void thunkForImportTable(EXPRESSION **exp)
{
    SYMBOL *sp;
    if (chosenAssembler->msil)
        return;
    if ((*exp)->type == en_pc)
        sp = (*exp)->v.sp;
    else
        sp = (*exp)->v.func->sp;
        // order is important here as we might get into this function with an already sanitized symbol
    if (sp && sp->linkage2 == lk_import && isfunction(sp->tp) && ((*exp)->type == en_pc || !(*exp)->v.func->ascall))
    {
        LIST *search = importThunks;
        while (search)
        {
            if (((SYMBOL *)search->data)->mainsym == sp)
            {
                *exp = varNode(en_pc, search->data);
                break; 
            }
            search = search->next;
        }
        if (!search)
        {
            SYMBOL *newThunk;
            char buf[2048];
            my_sprintf(buf, "@$%s", sp->name);
            newThunk = makeID(sc_global,&stdpointer,NULL, litlate(buf));
            newThunk->errname = newThunk->decoratedName = newThunk->name;
            newThunk->mainsym = sp; // mainsym is the symbol this was derived from
            newThunk->linkage = lk_virtual;
            search = (LIST *)Alloc(sizeof(LIST));
            search->next = importThunks;
            search->data = newThunk;
            importThunks = search;   
            *exp = varNode(en_pc, search->data);
        }
    }
}
static EXPRESSION *GetUUIDData(SYMBOL *cls)
{
    if (cls && cls->uuidLabel)
    {
        return intNode(en_labcon, cls->uuidLabel);
    }
    else
    {
        EXPRESSION *rv;
        STRING *data = Alloc(sizeof(STRING));
        data->pointers = Alloc(sizeof(void *));
        data->size = 1;
        data->strtype = l_astr;
        data->pointers[0] = Alloc(sizeof(SLCHAR));
        data->pointers[0]->count = 16;
        data->pointers[0]->str = Alloc(16 * sizeof(LCHAR));
        if (cls && cls->uuid)
        {
            int i;
            for (i=0; i < 16; i++)
                data->pointers[0]->str[i] = cls->uuid[i];
        }
        rv = stringlit(data);
        if (cls)
            cls->uuidLabel = rv->v.i;
        return rv;
    }
}
static EXPRESSION *GetManagedFuncData(TYPE *tp)
{
    // while managed functions seem to be always stdcall, the caller may be
    // either expecting a stdcall function, or a cdecl function
    // we take care of this by using the 'stdcall' attribute on the function
    // to determine what a caller might expect.
    // this function makes a little thunk table to tell the thunk generator
    // how to set up arguments for the call.  The arguments have to be reversed
    // but we cannot do a naive reversal because arguments of 8 bytes or more have to
    // be copied in order rather than reversed.
    char buf[512], *save = buf;
    int i;
    int sz = 0;
    STRING *data = Alloc(sizeof(STRING));
    SYMBOL *sp;
    HASHREC *hr;
    if (ispointer(tp))
        tp = basetype(tp)->btp;
    sp = basetype(tp)->sp;
    *save ++ = 0; // space for the number of dwords
    *save ++ = 0;
    *save++ =  sp->linkage == lk_stdcall;
    hr = basetype(tp)->syms->table[0];
    if (hr &&  ((SYMBOL *)hr->p)->tp->type != bt_void)
    {
        sz ++;
        hr = hr->next;
        if (hr)
        {
            sz++;
            hr = hr->next;
            while (hr)
            {
                int n = ((SYMBOL *)hr->p)->tp->size;
                n += 3;
                n /= 4;
                if (n > 32767)
                    diag("GetManagedFuncData: passing too large structure");
                if (n > 0x7f)
                {
                     *save++ = (n >> 8) | 0x80;
                     *save++ = n & 0xff;
                }
                else
                {
                    *save++ = n;
                }
                sz += n;
                hr = hr->next;
            }
        }
    }
    buf[0] = (sz >> 8) | 0x80;
    buf[1] = sz & 0xff;
    data->pointers = Alloc(sizeof(void *));
    data->size = 1;
    data->strtype = l_astr;
    data->pointers[0] = Alloc(sizeof(SLCHAR));
    data->pointers[0]->count = save - buf;
    data->pointers[0]->str = Alloc((save - buf) * sizeof(SLCHAR));
    for (i=0; i < save - buf; i++)
        data->pointers[0]->str[i] = buf[i];
    return stringlit(data);
}
void ValidateMSILFuncPtr(TYPE *dest, TYPE *src, EXPRESSION **exp)
{
    BOOLEAN managedDest = FALSE;
    BOOLEAN managedSrc = FALSE;
    if ((*exp)->type == en_func && (*exp)->v.func->ascall)
        return;
    // this implementation marshals functions that are put into a pointer variable
    // declared with STDCALL.   If it is not declared with stdcall, then putting it
    // into a structure and passing it to unmanaged code will not work.
    if (isfunction(dest))
    {
        // function arg or assignment to function constant
        managedDest = basetype(dest)->sp->linkage2 != lk_unmanaged && chosenAssembler->msil->managed(basetype(dest)->sp);
    }
    else if (isfuncptr(dest))
    {
        // function var
        managedDest = basetype(basetype(dest)->btp)->sp->linkage2 != lk_unmanaged && chosenAssembler->msil->managed(basetype(basetype(dest)->btp)->sp);
    }
    else
    {
        // unknown
        errortype(ERR_CANNOT_CONVERT_TYPE, src, dest);

    }
    if (isfunction(src))
    {
        // function arg or assignment to function constant
        managedSrc = chosenAssembler->msil->managed(basetype(src)->sp);
    }
    else if (isfuncptr(src))
    {
        // function var
        managedSrc = chosenAssembler->msil->managed(basetype(basetype(src)->btp)->sp);
    }
    else
    {
        // unknown
        errortype(ERR_CANNOT_CONVERT_TYPE, src, dest);
    }
    if (managedDest != managedSrc)
    {
        SYMBOL *sp;
        if (managedSrc)
        {
            sp = gsearch("__OCCMSIL_GetProcThunkToManaged");
            if(sp)
            {
                int n = 0;
                HASHREC *hr;
                char buf[512], *save = buf;
                FUNCTIONCALL *functionCall = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
                TYPE *tp1 = src;
                if (ispointer(tp1))
                    tp1 = basetype(tp1)->btp;

                hr = basetype(tp1)->syms->table[0];
                while (hr)
                {
                    int m = ((SYMBOL *)hr->p)->tp->size;
                    m += 3;
                    m /= 4;
                    n += m;
                     hr = hr->next;
                }
                sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
                functionCall->sp = sp;
                functionCall->functp = sp->tp;
                functionCall->fcall = varNode(en_pc, sp);
                functionCall->arguments = (INITLIST *)Alloc(sizeof(INITLIST));
                functionCall->arguments->tp = &stdpointer;
                functionCall->arguments->exp = *exp;
                functionCall->arguments->next = (INITLIST *)Alloc(sizeof(INITLIST));
                functionCall->arguments->next->tp = & stdpointer;
                functionCall->arguments->next->exp = GetManagedFuncData(tp1);
                functionCall->ascall = TRUE;
                *exp = varNode(en_func, NULL);
                (*exp)->v.func = functionCall;
            }
            else
            {
                diag("ValidateMSILFuncPtr: missing conversion func definition");
            }
        }
        else
        {
            sp = gsearch("__OCCMSIL_GetProcThunkToUnmanaged");
            if(sp)
            {
                FUNCTIONCALL *functionCall = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
                sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
                functionCall->sp = sp;
                functionCall->functp = sp->tp;
                functionCall->fcall = varNode(en_pc, sp);
                functionCall->arguments = (INITLIST *)Alloc(sizeof(INITLIST));
                functionCall->arguments->tp = &stdpointer;
                functionCall->arguments->exp = *exp;
                functionCall->ascall = TRUE;
                *exp = varNode(en_func, NULL);
                (*exp)->v.func = functionCall;
            }
            else
            {
                diag("ValidateMSILFuncPtr: missing conversion func definition");
            }
        }

    }
}
EXPRESSION *exprNode(enum e_node type, EXPRESSION *left, EXPRESSION *right)
{
    EXPRESSION *rv = Alloc(sizeof(EXPRESSION));
    rv->type = type;
    rv->left = left;
    rv->right = right;
    return rv;
}
EXPRESSION *varNode(enum e_node type, SYMBOL *sp)
{
    EXPRESSION *rv = Alloc(sizeof(EXPRESSION));
    rv->type = type;
    rv->v.sp = sp;
    return rv;	
}
static BOOLEAN inreg(EXPRESSION *exp, BOOLEAN first)
{
    while (castvalue(exp))
        exp = exp->left;
    if (first && lvalue(exp))
    {
        exp = exp->left;
        first = FALSE;
    }
    if (exp->type == en_auto)
    {
        return exp->v.sp->storage_class == sc_register;
    }
    else if (exp->type == en_add || exp->type == en_arrayadd 
             || exp->type == en_arraymul || exp->type == en_lsh
             || exp->type == en_structadd || exp->type == en_arraylsh)
        return inreg(exp->left,first) || inreg(exp->right, first);
    else
        return FALSE;		
}
EXPRESSION *typeNode(TYPE *tp)
{
    EXPRESSION *rv = Alloc(sizeof(EXPRESSION));
    rv->type = en_type;
    rv->v.tp = tp;
    return rv;

}
EXPRESSION *intNode(enum e_node type, LLONG_TYPE val)
{
    EXPRESSION *rv = Alloc(sizeof(EXPRESSION));
    rv->type = type;
    rv->v.i = val;
    return rv;
}
void checkauto(TYPE *tp1)
{
    if (basetype(tp1)->type == bt_auto)
    {
        error(ERR_AUTO_NOT_ALLOWED);
        while (tp1->type == bt_const || tp1->type == bt_volatile || tp1->type == bt_lrqual || tp1->type == bt_rrqual)
        {
            tp1->size = getSize(bt_int);
            tp1 = tp1->btp;
        }
        tp1-> type = bt_int;
        tp1->size = getSize(bt_int);
    }
}
static LEXEME *variableName(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    char idname[512];
    FUNCTIONCALL *funcparams = NULL;
    SYMBOL *sp = NULL;
    SYMBOL *strSym = NULL;
    NAMESPACEVALUES *nsv = NULL;
    LEXEME *placeholder = lex;
    if (ismutable)        
        *ismutable = FALSE;
    if (cparams.prm_cplusplus || chosenAssembler->msil)
    {
        lex = id_expression(lex, funcsp, &sp, &strSym, &nsv, NULL, FALSE, FALSE, idname);
    }
    else
    {
        sp = gsearch(lex->value.s.a);
    }
    if (sp)
    {
        SYMBOL *spx;
        HASHREC *hr;
        static int count;
        browse_usage(sp, lex->filenum);
        *tp = sp->tp;
        lex = getsym();
        switch (sp->storage_class)
        {	
            case sc_member:
            case sc_mutable:
                sp = lambda_capture(sp, cmThis, FALSE);
                break;
            case sc_auto:
            case sc_register:
            case sc_parameter:            
                sp = lambda_capture(sp, cmNone, FALSE);
                break;
            case sc_constant:
                if ((flags & _F_AMPERSAND) && sp->parent)
                    sp = lambda_capture(sp, cmNone, FALSE);
                break;
            default:
                break;
        }
        if (/*instantiatingTemplate &&*/ sp->tp->type == bt_templateparam && sp->tp->templateParam->p->byNonType.val)
        {
            switch (sp->tp->templateParam->p->type)
            {
                case kw_typename:
                case kw_template:
                    lex = prevsym(placeholder);
                    *tp = NULL;
                    if ((flags & (_F_SIZEOF | _F_PACKABLE)) == (_F_SIZEOF | _F_PACKABLE))
                    {
                        *exp = varNode(en_templateparam, sp);
                        *tp = sp->tp;
                        lex = getsym();
                    }
                    else
                    {
                        lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
                    }
                    return lex;
                case kw_int:
                    *exp = sp->tp->templateParam->p->byNonType.val;
                    *tp = sp->tp->templateParam->p->byNonType.tp;
                    if ((*tp)->type == bt_templateparam)
                    {
                        TYPE *tp1 = (*tp)->templateParam->p->byClass.val;
                        if (tp1)
                        {
                            *tp = tp1;
                        }
                        else
                        {
                            STRUCTSYM *s = structSyms;
                            SYMBOL *rv = NULL;
                            while (s && !rv)
                            {
                                if (s->tmpl)
                                    rv = templatesearch((*tp)->templateParam->argsym->name, s->tmpl);
                                s = s->next;
                            }
                            if (rv && rv->tp->templateParam->p->type == kw_typename)
                                *tp = rv->tp->templateParam->p->byClass.val;
                        }
                    }
                    return lex;
                default:
                    break;
            }
        }
        else
        { 
            if (sp->tp->type == bt_templateparam)
            {
                
                if ((sp->storage_class == sc_parameter || sp->tp->templateParam->p->type == kw_int) && sp->tp->templateParam->p->packed)
                {
                    if (packIndex >= 0)
                    {
                        TEMPLATEPARAMLIST *templateParam = sp->tp->templateParam->p->byPack.pack;
                        int i;
                        for (i=0; i < packIndex && templateParam; i++)
                            templateParam = templateParam->next;
                        if (templateParam)
                        {
                            if (templateParam->p->type == kw_int && !templateParam->p->packsym)
                            {
                                *tp = templateParam->p->byNonType.tp;
                                *exp = templateParam->p->byNonType.val;
                            }
                            else
                            {
                                sp = templateParam->p->packsym;
                                *tp = sp->tp;
                                *exp = varNode(en_auto, sp);
                            }
                        }
                        else
                        {
                            *exp = intNode(en_packedempty, 0);
                        }
                    }
                    else
                    {
                        sp->packed = TRUE;
                        *exp = varNode(en_auto, sp);
                    }
                }
                else
                {
                    *exp = varNode(en_templateparam, sp);   
                }
            }
            else switch (sp->storage_class)
            {	
                case sc_mutable:
                    if (ismutable)
                        *ismutable = TRUE;
                case sc_member:
                    if (cparams.prm_cplusplus && (flags & _F_AMPERSAND) && strSym)
                    {
                        *exp = getMemberPtr(sp, strSym, tp, funcsp);
                    }
                    else
                    {
                        *exp = getMemberNode(sp, strSym, tp, funcsp);
                    }
                    break;
                case sc_type:
                case sc_typedef:
                    lex = prevsym(placeholder);
                    *tp = NULL;
                    lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
                    return lex;
                case sc_overloads:
                    if (!strcmp(sp->name, "setjmp") && sp->parentClass == NULL && sp->parentNameSpace == NULL)
                        setjmp_used = TRUE;
                    hr = basetype(sp->tp)->syms->table[0];
                    funcparams = Alloc(sizeof(FUNCTIONCALL));
                    if (cparams.prm_cplusplus && MATCHKW(lex, lt))
                    {
                        lex = GetTemplateArguments(lex, funcsp, sp, &funcparams->templateParams);
                        funcparams->astemplate = TRUE;
                    }
                    if (hr->next || cparams.prm_cplusplus)
                    {
                        {
                            
                            SYMBOL *sym = getStructureDeclaration();
                            if (!sym || sym != sp->parentClass)
                                funcparams->noobject = TRUE;
                            funcparams->ascall = MATCHKW(lex, openpa);
                        }
                        funcparams->sp = sp;
                    }
                    else
                    {
                        // we only get here for C language, sadly we have to do
                        // argument based lookup in C++...
                        funcparams->sp = (SYMBOL *)hr->p;
                        funcparams->fcall = varNode(en_pc, funcparams->sp);
                        if (!MATCHKW(lex, openpa))
                            funcparams->sp->dumpInlineToFile = funcparams->sp->isInline;
                    }
                    funcparams->functp = funcparams->sp->tp;
                    *tp = funcparams->sp->tp;
                    funcparams->asaddress = !!(flags & _F_AMPERSAND);
                    if (cparams.prm_cplusplus 
                        && ismember(basetype(*tp)->sp)
                        && !MATCHKW(lex, openpa))
                    {
                        EXPRESSION *exp1 = Alloc(sizeof(EXPRESSION));
                        exp1->type = en_memberptr;
                        exp1->left = *exp;
                        exp1->v.sp = funcparams->sp;
                        *exp = exp1;
                        getMemberPtr(sp, strSym, tp, funcsp);
                    }
                    else
                    {
                        *exp = varNode(en_func, NULL);
                        (*exp)->v.func = funcparams;
                    }
                    break;
                case sc_catchvar:
                    makeXCTab(funcsp);
                    *exp = varNode(en_auto, funcsp->xc->xctab);
                    *exp = exprNode(en_add, *exp, intNode(en_c_i, (LLONG_TYPE)&(((struct _xctab *)0)->instance)));
                    deref(&stdpointer, exp);
                    break;
                case sc_enumconstant:		
                    *exp = intNode(en_c_i, sp->value.i);
                    break;
                case sc_constant:
                    *exp = varNode(en_const, sp);
                    break;
                case sc_auto:
                case sc_register:	/* register variables are treated as 
                                     * auto variables in this compiler
                                     * of course the usage restraints of the
                                     * register keyword are enforced elsewhere
                                     */
                    *exp = varNode(en_auto, sp);
                    break;
                case sc_parameter:
                    if (sp->packed)
                    {
                        if (!(flags & _F_PACKABLE))
                            error(ERR_PACK_SPECIFIER_MUST_BE_USED_IN_ARGUMENT);
                        if (packIndex >= 0)
                        {
                            TYPE *tp1 = sp->tp;
                            TEMPLATEPARAMLIST *templateParam;
                            int i;
                            while (ispointer(tp1) || isref(tp1))
                                tp1 = basetype(tp1)->btp;
                            tp1 = basetype(tp1);
                            if (tp1->type == bt_templateparam)
                            {
                                templateParam = tp1->templateParam->p->byPack.pack;
                                for (i=0; i < packIndex && templateParam; i++)
                                    templateParam = templateParam->next;
                                if (templateParam)
                                {
                                    sp = templateParam->p->packsym;
                                    *tp = sp->tp;
                                    *exp = varNode(en_auto, sp);
                                }
                                else
                                {
                                    *exp = intNode(en_packedempty, 0);
                                }
                            }
                            else
                            {
                                HASHREC *found = NULL;
                                HASHTABLE *tables = localNameSpace->syms;
                                while (tables && !found)
                                {
                                    HASHREC *hr = tables->table[0];
                                    while (hr && !found)
                                    {
                                        if (hr->p == sp)
                                            found = hr;
                                        hr = hr->next;
                                    }
                                    tables = tables->next;
                                }
                                if (found)
                                {
                                    int i;
                                    for (i=0; found && i < packIndex; i++)
                                        found = found->next;
                                    if (found)
                                    {
                                        sp = found->p;
                                    }
                                    *exp = varNode(en_auto, sp);
                                    *tp = sp->tp;
                                }
                                else
                                {
                                    *exp = intNode(en_packedempty, 0);
                                }
                            }
                        }
                        else
                        {
                            *exp = varNode(en_auto, sp);
                        }
                    }
                    else
                    {
                        *exp = varNode(en_auto, sp);
                    }
                    /* derefereance parameters which are declared as arrays */
                    {
                        TYPE *tpa = basetype(sp->tp);
                        if (isref(tpa))
                            tpa = basetype(tpa->btp);
                        if (tpa->array)
                            deref(&stdpointer, exp);
                    }
                    break;
                
                case sc_localstatic:
                    if (!(flags & _F_SIZEOF))
                        GENREF(sp);
                    if (funcsp && funcsp->isInline 
                        && funcsp->storage_class == sc_global)
                    {
                        if (funcsp->promotedToInline)
                        {
                            funcsp->isInline = funcsp->dumpInlineToFile = funcsp->promotedToInline = FALSE;
                        }
                        else
                        {
                            errorsym(ERR_INLINE_CANNOT_REFER_TO_STATIC, sp);
                        }
                    }
                    if (sp->linkage3 == lk_threadlocal)
                        *exp = varNode(en_threadlocal, sp);
                    else
                        *exp = varNode(en_label, sp);
                    sp->used = TRUE;
                    break;
                case sc_absolute:
                    *exp = varNode(en_absolute, sp);
                    break;				
                case sc_static:
                    sp->used = TRUE;
                case sc_global:
                case sc_external:
                    if (strSym)
                    {
                        SYMBOL *tpl = sp;
                        while (tpl)
                        {
                            if (tpl->templateLevel)
                                break;
                            tpl = tpl->parentClass;
                        }
                        if (tpl && tpl->instantiated)
                        {
                            TemplateDataInstantiate(sp, FALSE, FALSE);
                        }
                    }
                    if (!(flags & _F_SIZEOF))
                        GENREF(sp);
                    if (sp->parentClass && !isExpressionAccessible(NULL, sp, funcsp, NULL, FALSE))
                        errorsym(ERR_CANNOT_ACCESS, sp);		
                    if (sp->linkage3 == lk_threadlocal)
                        *exp = varNode(en_threadlocal, sp);
                    else
                        *exp = varNode(en_global, sp);
                    if (sp->linkage2 == lk_import)
                    {
//                        *exp = exprNode(en_add, *exp, intNode(en_c_i, 2));
//                        deref(&stdpointer, exp);
                        deref(&stdpointer, exp);
                    }
                    break;
                case sc_namespace:
                case sc_namespacealias:
                    errorsym(ERR_INVALID_USE_OF_NAMESPACE, sp);
                    *exp = intNode(en_c_i, 1);
                    break;
                default:
                    error(ERR_IDENTIFIER_EXPECTED);
                    *exp = intNode(en_c_i, 1);
                    break;
            }
        }
        sp->tp->used = TRUE;
        if (sp->templateLevel && istype(sp))
        {
            lex = prevsym(placeholder);
            *tp = NULL;
            lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
        }
        else
        {
            if (sp->tp->type == bt_templateparam)
            {
                if (*exp && (*exp)->type != en_packedempty && !sp->tp->templateParam->p->packed)
                {
                    if (sp->tp->templateParam->p->type == kw_int)
                        *tp = sp->tp->templateParam->p->byNonType.tp;
                    else
                        *tp = &stdint;
                    if (!templateNestingCount)
                        *exp = intNode(en_c_i, 0);
                    if (MATCHKW(lex, openpa))
                    {
                        lex = prevsym(placeholder);
                        *tp = NULL;
                        lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
                        return lex;
                    }
                }
            }
            else if (sp->tp->type == bt_any)
                deref(&stdint, exp);
            else
            {
                if (isref(*tp))
                {
                    TYPE *tp1 = *tp;
                    deref(*tp, exp);
                    while (isref(tp1))
                        tp1 = basetype(tp1)->btp;
                    *tp = Alloc(sizeof(TYPE));
                    **tp = *tp1;
                    UpdateRootTypes(*tp);
                    
                }
                if (sp->storage_class != sc_overloads)
                {
                    if (!isstructured(*tp) && basetype(*tp)->type != bt_memberptr && !isfunction(*tp) && 
                        sp->storage_class != sc_constant && sp->storage_class != sc_enumconstant)
                    {
                        if (!(*tp)->array || (*tp)->vla || !(*tp)->msil && sp->storage_class == sc_parameter)
                            if ((*tp)->vla)
                                deref(&stdpointer, exp);
                            else
                                deref(*tp, exp);
                        else if ((*tp)->array && inreg(*exp, TRUE))
                            error(ERR_CANNOT_TAKE_ADDRESS_OF_REGISTER);
                    }
                }
            }
    
            if (lvalue(*exp))
                (*exp)->v.sp = sp; // catch for constexpr
            (*exp)->pragmas = stdpragmas;
            if (isvolatile(*tp))
                (*exp)->isvolatile = TRUE;
            if (isrestrict(*tp))
                (*exp)->isrestrict = TRUE;
            if (isatomic(*tp))
            {
                (*exp)->isatomic = TRUE;
                if (needsAtomicLockFromType(*tp))
                    (*exp)->lockOffset = (*tp)->size - ATOMIC_FLAG_SPACE;
            }
            if (strSym && funcparams)
                funcparams->novtab = TRUE;
            if (cparams.prm_cplusplus && (sp->storage_class == sc_member || sp->storage_class == sc_mutable))
            {
                qualifyForFunc(funcsp, tp, *ismutable);
            }
        }
    }
    else
    {
        char *name ;
        if (strSym && strSym->tp->type == bt_templateselector)
        {
            *exp = exprNode(en_templateselector, NULL, NULL);
            (*exp)->v.templateSelector = strSym->tp->sp->templateSelector;
            *tp = &stdany;
            lex = getsym();
            return lex;
        }
        IncGlobalFlag();
        if (ISID(lex))
            name = litlate(lex->value.s.a);
        else
            name = litlate("__unknown");
        sp = Alloc(sizeof(SYMBOL ));
        sp->name = name;
        sp->used = TRUE;
        sp->declfile = sp->origdeclfile = lex->file;
        sp->declline = sp->origdeclline = lex->line;
        sp->declfilenum = lex->filenum;
        lex = getsym();
        if (MATCHKW(lex, openpa))
        {
            if (cparams.prm_cplusplus)
            {
                sp->storage_class = sc_overloads;
                (*tp) = Alloc(sizeof(TYPE));
                (*tp)->type = bt_aggregate;
                UpdateRootTypes(*tp);
                (*tp)->sp = sp;
            }
            else
            {
                /* no prototype error will be added later */
                sp->storage_class = sc_external;
                sp->linkage = lk_c;
                sp->tp = Alloc(sizeof(TYPE));
                sp->tp->type = bt_func;
                sp->tp->size = getSize(bt_pointer);
                sp->tp->syms=CreateHashTable(1);
                sp->tp->sp = sp;
                sp->tp->btp = Alloc(sizeof(TYPE));
                sp->tp->rootType = sp->tp;
                sp->tp->btp->rootType = sp->tp->btp;
                sp->oldstyle = TRUE;
                sp->tp->btp->type = bt_int;
                sp->tp->btp->size = getSize(bt_int);
                InsertExtern(sp);
                *tp = sp->tp;
            }
            SetLinkerNames(sp, lk_c);
            if (sp->storage_class != sc_overloads)
            {
                InsertSymbol(sp, sp->storage_class, FALSE, FALSE);
                *exp = varNode(sp->storage_class ==sc_auto ? en_auto : en_global, sp);
            }
            else
            {
                sp->tp = Alloc(sizeof(TYPE));
                sp->tp->type = bt_func;
                sp->tp->size = getSize(bt_pointer);
                sp->tp->syms=CreateHashTable(1);
                sp->tp->sp = sp;
                sp->tp->btp = Alloc(sizeof(TYPE));
                sp->oldstyle = TRUE;
                sp->tp->btp->type = bt_int;
                sp->tp->btp->size = getSize(bt_int);
                sp->tp->rootType = sp->tp;
                sp->tp->btp->rootType = sp->tp->btp;
                funcparams = Alloc(sizeof(FUNCTIONCALL));
                funcparams->sp = sp;
                funcparams->functp = sp->tp;
                funcparams->fcall = intNode(en_c_i, 0);
                *tp = sp->tp;
                *exp = varNode(en_func, NULL);
                (*exp)->v.func = funcparams;
            }
        }
        else
        {
            if (cparams.prm_cplusplus && atp && isfuncptr(atp))
            {
                SYMBOL *sym;
                sp->storage_class = sc_overloads;
                (*tp) = Alloc(sizeof(TYPE));
                (*tp)->type = bt_aggregate;
                UpdateRootTypes(*tp);
                (*tp)->sp = sp;
                DecGlobalFlag();
                funcparams = Alloc(sizeof(FUNCTIONCALL));
                funcparams->ascall = TRUE;    
                 sym = GetOverloadedFunction(tp, &funcparams->fcall, sp, NULL, atp, TRUE, FALSE, TRUE, flags);
                 if (sym)
                 {
                     sym->throughClass = sp->throughClass;
                     sp = sym;
                    if (!isExpressionAccessible(NULL, sp, funcsp, NULL, FALSE))
                        errorsym(ERR_CANNOT_ACCESS, sp);		
                 }
                funcparams->sp = sp;
                funcparams->functp = funcparams->sp->tp;
                *exp = varNode(en_func, NULL);
                (*exp)->v.func = funcparams;
                IncGlobalFlag();
            }
            else
            {
                sp->storage_class = funcsp ? sc_auto : sc_global;
                sp->tp = Alloc(sizeof(TYPE));
                sp->tp->type = bt_any;
                sp->tp->rootType = sp->tp;
                sp->parentClass = strSym;
                *tp = sp->tp;
                deref(&stdint, exp);
                SetLinkerNames(sp, lk_c);
                if (!nsv && (!strSym || !templateNestingCount || !strSym->templateLevel && strSym->tp->type != bt_templateselector && strSym->tp->type != bt_templatedecltype ))
                {
                    // no error if there are packed templates and we aren't parsing them
                    BOOLEAN found = FALSE;
                    if (!expandingParams)
                    {
                        SYMBOL *spx = strSym;
                        while (spx && !found)
                        {
                            TEMPLATEPARAMLIST *tpl = spx->templateParams;
                            while (tpl && !found)
                            {
                                if (tpl->p->packed)
                                    found = TRUE;
                                tpl = tpl->next;
                            }
                            spx = spx->parentClass;
                        }
                    }
                    if (!found)
                    {
                        errorstr(ERR_UNDEFINED_IDENTIFIER, name);
                        if (sp->storage_class != sc_overloads && (localNameSpace->syms || sp->storage_class != sc_auto ) )
                            InsertSymbol(sp, sp->storage_class, FALSE, FALSE);
                    }
                }
                if (nsv)
                {
                    errorNotMember(strSym, nsv, sp->name);
                }
                if (sp->storage_class != sc_overloads)
                {
                    *exp = varNode(en_global, sp);
                }
                else
                {
                    funcparams = Alloc(sizeof(FUNCTIONCALL));
                    *exp = varNode(en_func, NULL);
                    (*exp)->v.func = funcparams;
                }
            }
        }
        DecGlobalFlag();
    }
    sp->allocate = TRUE;
    return lex;
}
static LEXEME *expression_member(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    TYPE *typein = *tp;
    BOOLEAN points = FALSE;
    BOOLEAN thisptr = (*exp)->type == en_auto && (*exp)->v.sp->thisPtr;
    char *tokenName = lex->kw->name;
    (void)funcsp;
    // find structured version of arithmetic types for msil member matching
    if (chosenAssembler->msil && (isarithmetic(*tp) || (*tp)->type == bt___string) && chosenAssembler->msil->find_boxed_type)
    {
        // auto-boxing for msil
        TYPE *tp1 = chosenAssembler->msil->find_boxed_type(basetype(*tp));
        if (tp1)
        {
            while (castvalue(*exp))
                *exp = (*exp)->left;
            if (!lvalue(*exp))
                *exp = msilCreateTemporary(*tp, *exp);
            else
                *exp = (*exp)->left;
            *tp = tp1;
        }
    }
    if (MATCHKW(lex, pointsto))
    {
        TYPE *nesting[100];
        int n = 0;
        points = TRUE;
        do
        {
            if (isstructured(*tp))
            {
                TYPE *x = basetype(*tp);
                int i;
                for (i=0; i < n; i++)
                    if (nesting[i] == x)
                    {
                        break;
                    }
                nesting[n++] = x;
                if (n >= sizeof(nesting)/sizeof(nesting[0]))
                {
                    break;
                }
            }
        }
        while (cparams.prm_cplusplus && insertOperatorFunc(ovcl_pointsto, pointsto,
                               funcsp, tp, exp, NULL,NULL, NULL, flags));
        typein = *tp;
        if (ispointer(*tp))
        {
            *tp = basetype(*tp);
            *tp = (*tp)->btp;
        }
     }
    *tp = PerformDeferredInitialization (*tp, funcsp);
    lex = getsym();
    if (cparams.prm_cplusplus && MATCHKW(lex, compl))
    {
        // direct destructor or psuedo-destructor
        enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
        BOOLEAN defd = FALSE;
        SYMBOL *sp = NULL;
        BOOLEAN notype = FALSE;
        TYPE *tp1 = NULL;
        lex = getsym();
        lex = getBasicType(lex, funcsp, &tp1, NULL, FALSE, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL, FALSE, TRUE);
        if (!tp1)
        {
            error(ERR_TYPE_NAME_EXPECTED);
        }
        else if (!comparetypes(*tp, tp1, TRUE))
        {
            if (!templateNestingCount)
                error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
        }
        else if (isstructured(*tp))
        {
            // destructor
            SYMBOL *sp2 = search(overloadNameTab[CI_DESTRUCTOR], (basetype(*tp)->sp)->tp->syms);
            if (sp2)
            {
                callDestructor(basetype(*tp)->sp, NULL, exp, NULL, TRUE, FALSE, FALSE);
            }
            if (needkw(&lex, openpa))
                needkw(&lex, closepa);
        }
        else
        {
            // psuedo-destructor, no further activity required.
            if (needkw(&lex, openpa))
                needkw(&lex, closepa);
        }
        *tp = &stdvoid;
    }
    else if (!isstructured(*tp) || (points && !ispointer(typein)))
    {
        if (cparams.prm_cplusplus && ISKW(lex) && (lex->kw->tokenTypes & TT_BASETYPE))
        {
            // possible psuedo destructor with selector
            enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            BOOLEAN defd = FALSE;
            SYMBOL *sp = NULL;
            BOOLEAN notype = FALSE;
            TYPE *tp1 = NULL;
            lex = getBasicType(lex, funcsp, &tp1, NULL, FALSE, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL, FALSE, TRUE);
            if (!tp1)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else if (!comparetypes(*tp, tp1, TRUE))
            {
                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
            }
            if (!MATCHKW(lex, classsel))
            {
                error(ERR_INVALID_PSUEDO_DESTRUCTOR);
            }
            else
            {
                lex = getsym();
                if (!MATCHKW(lex, compl))
                {
                    error(ERR_INVALID_PSUEDO_DESTRUCTOR);
                }
                else
                {
                    lex = getsym();
                    tp1 = NULL;
                    lex = getBasicType(lex, funcsp, &tp1, NULL, FALSE, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL, FALSE, TRUE);
                    if (!tp1)
                    {
                        error(ERR_TYPE_NAME_EXPECTED);
                    }
                    else if (!comparetypes(*tp, tp1, TRUE))
                    {
                        error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                    }
                    else if (needkw(&lex , openpa))
                        needkw(&lex, closepa);
                }
            }
            *tp = &stdvoid;
        }
        else
        { 
            if (points)
                errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, tokenName);
            else
                errorstr(ERR_STRUCTURED_TYPE_EXPECTED, tokenName);
            while (ISID(lex))
            {
                lex = getsym();
                if (!MATCHKW(lex, pointsto) && !MATCHKW(lex, dot))
                    break;
                lex = getsym();
            }
        }
    }
    else
    {
        BOOLEAN isTemplate = FALSE;
        if (MATCHKW(lex, kw_template))
        {
            lex = getsym();
            isTemplate = TRUE;
        }
        if (!ISID(lex) && !MATCHKW(lex, kw_operator))
        {
            error(ERR_IDENTIFIER_EXPECTED);
        }
        else
        {
            SYMBOL *sp2 = NULL;
            if ((*exp)->type == en_literalclass)
            {
                CONSTEXPRSYM *ces = (CONSTEXPRSYM *)search(lex->value.s.a, (*exp)->v.syms);
                if (ces)
                {
                    lex = getsym();
                    *exp = ces->exp;
                    *tp = ces->sym->tp;
                    return lex;
                }
                else 
                {
                    STRUCTSYM l ;
                    l.str = basetype(*tp)->sp;
                    addStructureDeclaration(&l);
                    lex = id_expression(lex, funcsp, &sp2, NULL, NULL, &isTemplate, FALSE, TRUE, NULL);
                    dropStructureDeclaration();
                    if (!sp2)
                    {
                        errorNotMember(basetype(*tp)->sp, NULL, lex->value.s.a);
                        lex = getsym();
                        while (ISID(lex))
                        {
                            lex = getsym();
                            if (!MATCHKW(lex, pointsto) && !MATCHKW(lex, dot))
                                break;
                            lex = getsym();
                        }
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        return lex;
                    }
                    lex = getsym();
                    if (sp2->tp->type != bt_aggregate)
                    {
                        error(ERR_CONSTANT_FUNCTION_EXPECTED);
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        return lex;
                    }
                    if (MATCHKW(lex, openpa))
                    {
                        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                        SYMBOL *match;
                        TYPE *tp1 = NULL;
                        EXPRESSION *exp1 = NULL;
                        lex = getArgs(lex, funcsp, funcparams, closepa, TRUE, flags);
                        funcparams->thisptr = intNode(en_c_i, 0);
                        funcparams->thistp = Alloc(sizeof(TYPE));
                        funcparams->thistp->type = bt_pointer;
                        funcparams->thistp->size = getSize(bt_pointer);
                        funcparams->thistp->btp = *tp;
                        funcparams->thistp->rootType = funcparams->thistp;
                        funcparams->ascall = TRUE;    
                        match = GetOverloadedFunction(&tp1, &exp1, sp2, funcparams,NULL,TRUE, FALSE, TRUE, flags);
                        if (match)
                        {
                            funcparams->sp = match;
                            funcparams->functp = match->tp;
                            *exp = substitute_params_for_function(funcparams, (*exp)->v.syms);
                            optimize_for_constants(exp);
                            *tp = basetype(match->tp)->btp;
                            if (!match->constexpression || !IsConstantExpression(*exp, TRUE))
                                error(ERR_CONSTANT_FUNCTION_EXPECTED);
                        }
                        else
                        {
                            *exp = intNode(en_c_i, 0);
                            *tp = &stdint;
                        }
                        return lex;
                    }
                    else
                    {
                        needkw(&lex, openpa);
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        return lex;
                    }      
                }   
            }
            if (cparams.prm_cplusplus)
            {
                STRUCTSYM l ;
                l.str = basetype(*tp)->sp;
                addStructureDeclaration(&l);
                lex = id_expression(lex, funcsp, &sp2, NULL, NULL, &isTemplate, FALSE, TRUE, NULL);
                dropStructureDeclaration();
            }
            else
            {
                sp2 = search(lex->value.s.a, (basetype(*tp)->sp)->tp->syms);
            }
            if (!sp2)
            {
                errorNotMember(basetype(*tp)->sp, NULL, lex->value.s.a);
                lex = getsym();
                while (ISID(lex))
                {
                    lex = getsym();
                    if (!MATCHKW(lex, pointsto) && !MATCHKW(lex, dot))
                        break;
                    lex = getsym();
                }
            }
            else
            {
                TYPE *tpb;
                TYPE *basetp = *tp;
                SYMBOL *sp3 = sp2;
                TYPE *typ2 = typein;
                SYMBOL *tpl = sp2;
                browse_usage(sp2,lex->filenum);
                if (ispointer(typ2))
                    typ2 = basetype(typ2)->btp;
                (*exp)->isatomic = FALSE;
                lex = getsym();
                sp2->used = TRUE;
                *tp = sp2->tp;
                tpb = basetype(*tp);
                if (sp2->storage_class == sc_overloads)
                {
                    FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                    if (cparams.prm_cplusplus && MATCHKW(lex, lt))
                    {
                        HASHREC *hr1 = basetype(sp2->tp)->syms->table[0];
                        BOOLEAN isdest = ((SYMBOL *)hr1->p)->isDestructor;
                        while (hr1)
                        {
                             if (((SYMBOL *)hr1->p)->templateLevel)
                                 break;
                             hr1 = hr1->next;
                        }
                        if (hr1)
                        {
                            lex = GetTemplateArguments(lex, funcsp, NULL, &funcparams->templateParams);
                            funcparams->astemplate = TRUE;
                        }
                        else if (isdest && sp2->parentClass->templateLevel)
                        {
                            TEMPLATEPARAMLIST *lst = NULL;
                            lex = GetTemplateArguments(lex, funcsp, NULL, &lst);
                            if (!exactMatchOnTemplateArgs(lst, sp2->parentClass->templateParams->p->bySpecialization.types))
                                error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                        }
                        else if (isTemplate)
                        {
                            errorsym(ERR_NOT_A_TEMPLATE, sp2);
                        }
                    }
                    else if (isTemplate)
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, sp2);
                    }
                    funcparams->sp = sp2;
                    funcparams->thisptr = *exp;
                    funcparams->thistp = Alloc(sizeof(TYPE));
                    funcparams->thistp->size = getSize(bt_pointer);
                    funcparams->thistp->type = bt_pointer;
                    funcparams->thistp->btp = basetp;
                    funcparams->thistp->rootType = funcparams->thistp;
                    
                    if (!points && (*exp)->type != en_l_ref)
                        funcparams->novtab = TRUE;
                    *exp = varNode(en_func, NULL);
                    (*exp)->v.func = funcparams;
                }
                else 
                {
                    SYMBOL *sp3 = sp2->parentClass, *sp4 = basetype(typ2)->sp;
                    if (ismutable && sp2->storage_class == sc_mutable)
                        *ismutable = TRUE;
                    if (sp2->templateLevel)
                    {
                        if (!sp2->instantiated)
                        {
                            TEMPLATEPARAMLIST *lst = NULL;
                            SYMBOL *sp1 = sp2;
                            if (MATCHKW(lex, lt))
                            {
                                lex = GetTemplateArguments(lex, funcsp, sp2, &lst);
                                if (sp1)
                                {
                                    sp1 = GetClassTemplate(sp1, lst, FALSE);
                                    if (sp1)
                                        sp2 = sp1;
                                    else
                                        errorsym(ERR_NO_TEMPLATE_MATCHES, sp2);
                                }
                            }
                            else if (sp2)
                            {
                                sp1 = sp2;
                            }
                        }
                    }
                    else if (isTemplate)
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, sp2);
                    }
                    if (sp2->storage_class == sc_external || sp2->storage_class == sc_static)
                    {
                        SYMBOL *tpl = sp2;
                        while (tpl)
                        {
                            if (tpl->templateLevel)
                                break;
                            tpl = tpl->parentClass;
                        }
                        if (tpl && tpl->instantiated)
                        {
                            TemplateDataInstantiate(sp2, FALSE, FALSE);
                        }
                    }
                    /*
                    if (sp3 && sp3->mainsym)
                        sp3 = sp3->mainsym;
                    if (sp4 && sp4->mainsym)
                        sp4 = sp4->mainsym;
                        
                    if (sp3 && sp4 && sp3 != sp4 && classRefCount(sp3, sp4) != 1)
                    {
                        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, sp3, sp4);
                    }
                    */
                    {
                        enum e_ac access = ac_public;
                        SYMBOL *ssp = getStructureDeclaration();
                        if (ssp == basetype(typ2)->sp)
                            access = ac_protected;
                        if (!isAccessible(basetype(typ2)->sp, basetype(typ2)->sp, sp2, funcsp, access, FALSE))
                        {
                            errorsym(ERR_CANNOT_ACCESS, sp2);
                        }
                    }
                    if (sp2->storage_class == sc_constant)
                    {
                        *exp = varNode(en_const, sp2);
                    }
                    else if (sp2->storage_class == sc_static || sp2->storage_class == sc_external)
                    {
                        if (chosenAssembler->msil)
                        {
                            *exp = varNode(en_global, sp2);
                        }
                        else
                        {
                            EXPRESSION *exp2 = varNode(en_global, sp2);
                            *exp = exprNode(en_void, *exp, exp2);
                        }
                    }
                    else
                    {
                        EXPRESSION *offset;
                        if (chosenAssembler->msil && !isarray(sp2->tp))
                            offset = varNode(en_structelem, sp2); // prepare for the MSIL ldflda instruction
                        else
                            offset = intNode(en_c_i, sp2->offset);
                        if (sp2->parentClass != basetype(typ2)->sp)
                        {
                            *exp = baseClassOffset(sp2->parentClass, basetype(typ2)->sp, *exp);
                        }
                        *exp = exprNode(en_structadd, *exp, offset);
                        if (sp3)
                        {
                            do
                            {
                                if (basetype(sp3->tp)->type == bt_union)
                                {
                                    offset->unionoffset = TRUE;
                                    break;
                                }
                                if (sp3 != sp2 && ispointer(sp3->tp))
                                    break;
                                sp3 = sp3->parent;
                            } while (sp3);
                        }
                    }
                    if (tpb->hasbits)
                    {
                        (*exp) = exprNode(en_bits, *exp, 0);
                        (*exp)->bits = tpb->bits;
                        (*exp)->startbit = tpb->startbit;
                    }
                    if (sp2->storage_class != sc_constant)
                    {
                        if (isref(*tp))
                        {
                            if (!isstructured(basetype(*tp)->btp))
                                deref(*tp, exp);
                            
                        }
                        deref(*tp, exp);
                    }
                    (*exp)->v.sp = sp2; // caching the member symbol in the enode for constexpr handling
                    if (isatomic(basetp))
                    {
                        // note this won't work in C++ because of offset2...
                        (*exp)->isatomic = TRUE;
                        if (needsAtomicLockFromType(*tp))
                            (*exp)->lockOffset = basetp->size - ATOMIC_FLAG_SPACE - sp2->offset;
                    }
                    
                }
                if (!points && ((*exp)->type == en_not_lvalue || (*exp)->type == en_func
                    || (*exp)->type == en_void))
                    if (ISKW(lex) && lex->kw->key >= assign && lex->kw->key <= asxor)
                        error(ERR_LVALUE);
            }
        }
        /* members inherit qualifiers from their parent
         * this mechanism will automatically recurse
         * we don't encode this in the actual type so we don't have to dup
         * types all over the place
         */
        if (points && ispointer(typein))
            typein = basetype(typein)->btp;

        if (isconst(typein) && !isconst(*tp))
        {
            TYPE *p = Alloc(sizeof(TYPE));
            p->type = bt_const;
            p->btp = *tp;
            p->rootType = (*tp)->rootType;
            p->size = p->btp->size;
            (*tp) = p;
        }
        if (isvolatile(typein) && !isvolatile(*tp))
        {
            TYPE *p = Alloc(sizeof(TYPE));
            p->type = bt_volatile;
            p->btp = *tp;
            p->rootType = (*tp)->rootType;
            p->size = p->btp->size;
            (*tp) = p;
        }
    }
    return lex;
}
static LEXEME *expression_bracket(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    TYPE *tp2;
    EXPRESSION *expr2 = NULL;
    lex = getsym();
    if (cparams.prm_cplusplus && MATCHKW(lex, begin))
    {
        INITLIST *args = NULL;
        lex = getInitList(lex, funcsp, &args);
        if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_openbr, openbr,
                               funcsp, tp, exp, NULL, NULL, args, flags))
        {
        }
        else
        {
            error(ERR_CANNOT_USE_INIT_LIST_WITH_BUILTIN_BRACKET);
        }
                
    }
    else
    {
        lex = expression_comma(lex, funcsp, NULL, &tp2, &expr2, NULL, flags);
        if (tp2)
        {
            if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_openbr, openbr,
                                   funcsp, tp, exp, tp2, expr2, NULL, flags))
            {
            }
            else if (isvoid(*tp) || isvoid(tp2) || (*tp)->type == bt_aggregate || tp2->type == bt_aggregate || ismsil(*tp) || ismsil(tp2))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            else if (basetype(tp2)->type == bt_memberptr || basetype(*tp)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            else if (basetype(tp2)->scoped || basetype(*tp)->scoped)
                error(ERR_SCOPED_TYPE_MISMATCH);
            else if (isarray(*tp) && (*tp)->msil)
            {
                if (!isint(tp2) && basetype(tp2)->type != bt_enum)
                {
                    if (ispointer(tp2))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                if ((*exp)->type != en_msil_array_access)
                {
                    EXPRESSION *expr3 = exprNode(en_msil_array_access, NULL, NULL);
                    expr3->v.msilArray = (MSIL_ARRAY *)Alloc(sizeof(MSIL_ARRAY) + 9 * sizeof(EXPRESSION *));
                    expr3->v.msilArray->max = 10;
                    expr3->v.msilArray->count = 0;
                    expr3->v.msilArray->base = (*exp);
                    expr3->v.msilArray->tp = *tp;
                    (*exp) = expr3;
                }
                else if ((*exp)->v.msilArray->count >= (*exp)->v.msilArray->max)
                {
                    EXPRESSION *expr3 = exprNode(en_msil_array_access, NULL, NULL);
                    expr3->v.msilArray = (MSIL_ARRAY *)Alloc(sizeof(MSIL_ARRAY) + ((*exp)->v.msilArray->max * 2 - 1) * sizeof(EXPRESSION *));
                    expr3->v.msilArray->max = (*exp)->v.msilArray->max * 2;
                    expr3->v.msilArray->count = (*exp)->v.msilArray->count;
                    expr3->v.msilArray->base = (*exp)->v.msilArray->base;
                    expr3->v.msilArray->tp = (*exp)->v.msilArray->tp;
                    memcpy(expr3->v.msilArray->indices, (*exp)->v.msilArray->indices,
                          (*exp)->v.msilArray->count * sizeof(EXPRESSION *));
                    (*exp) = expr3;
                }
                (*exp)->v.msilArray->indices[(*exp)->v.msilArray->count++] = expr2;
                *tp = basetype(*tp)->btp;
            }
            else if (ispointer(*tp))
            {
                if (!isint(tp2) && basetype(tp2)->type != bt_enum)
                {
                    if (ispointer(tp2))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                /* unreference pointer to VLA */
                if (!(*tp)->vla && basetype(basetype(*tp)->btp)->vla)
                    (*tp) = basetype(*tp)->btp;
                if ((*tp)->vla)
                {
                    int soa = getSize(bt_pointer);
                    int sou = getSize(bt_unsigned);
                    EXPRESSION *vlanode = varNode(en_auto, (*tp)->sp);
                    EXPRESSION *exp1 = exprNode(en_add, vlanode, intNode(en_c_i, soa + sou *(1 + (*tp)->vlaindex)));
                    deref(&stdint, &exp1);
                    cast(&stdpointer, &expr2);
                    exp1 = exprNode(en_umul, exp1, expr2);
                    *exp = exprNode(en_add, *exp, exp1);
                    *tp = basetype(*tp)->btp;
                }
                else
                {
                    EXPRESSION *exp1 = NULL;
                    *tp = basetype(*tp)->btp;
                    if (isstructured(*tp))
                        *tp = PerformDeferredInitialization(*tp, funcsp);
                    cast( &stdint, &expr2);
                    exp1 = exprNode(en_umul, expr2, 
                                   intNode(en_c_i, (*tp)->size));
                    *exp = exprNode(en_add, *exp, exp1);
                }
                if (!(*tp)->array && !(*tp)->vla)
                    deref(*tp, exp);
            }
            else if (ispointer(tp2))
            {
                if (!isint(*tp))
                {
                    if (ispointer(*tp))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                if (tp2->vla)
                {
                    int soa = getSize(bt_pointer);
                    int sou = getSize(bt_unsigned);
                    EXPRESSION *vlanode = varNode(en_auto, tp2->sp);
                    expr2 = exprNode(en_add, expr2, intNode(en_c_i, soa + sou *( 1 + tp2->vlaindex)));
                    deref(&stdint, exp);
                    *exp = exprNode(en_umul, expr2, *exp);
                    deref(&stdpointer, &vlanode);
                    *exp = exprNode(en_add, *exp, vlanode);
                    *tp = basetype(tp2)->btp;
                }
                else
                {
                    EXPRESSION *exp1 = NULL;
                    *tp = basetype(tp2)->btp;
                    cast(&stdint, exp);
                    exp1 = exprNode(en_umul, *exp, 
                                   intNode(en_c_i, (*tp)->size));
                    *exp = exprNode(en_add, expr2, exp1);
                }
                if (!(*tp)->array && !(*tp)->vla)
                    deref(*tp, exp);
            }
            else
            {
                error(ERR_DEREF);		
            }
        }
        else
            error(ERR_EXPRESSION_SYNTAX);
    }
    if (!MATCHKW(lex, closebr))
    {
        error(ERR_ARRAY_NEED_CLOSEBRACKET);
        errskim(&lex, skim_closebr);
    }
    skip(&lex, closebr);
    *tp = PerformDeferredInitialization (*tp, funcsp);
    return lex;
}
static void checkArgs(FUNCTIONCALL *params, SYMBOL *funcsp)
{
    HASHREC *hr = basetype(params->functp)->syms->table[0];
    INITLIST *list = params->arguments;
    BOOLEAN matching = TRUE;
    BOOLEAN tooshort = FALSE;
    BOOLEAN toolong = FALSE;
    BOOLEAN noproto = FALSE;//params->sp ? params->sp->oldstyle : FALSE;
    BOOLEAN vararg = FALSE;
    BOOLEAN hasEllipse = FALSE;
    int argnum = 0;
 
    if (hr && ((SYMBOL *)hr->p)->thisPtr)
        hr = hr->next;   
    if (!hr)
    {
        matching = FALSE;
        noproto = params->sp->oldstyle;
    }
    if (noproto)
    {
        if (params->sp->hasproto)
            noproto = FALSE;
    }
    if (hr && ((SYMBOL *)hr->p)->tp->type == bt_void)
    {
        if (list)
            errorsym(ERR_FUNCTION_TAKES_NO_PARAMETERS, params->sp);
    }
    else while (hr || list)
    {
        TYPE *dest = NULL;
        SYMBOL *decl = NULL;
        if (!hr || ((SYMBOL *)hr->p)->tp->type != bt_any)
        {               
            argnum++;
            if (matching && hr)
            {
                decl = (SYMBOL *)hr->p;
                if (!decl->tp)
                    noproto = TRUE;
                else if (basetype(decl->tp)->type == bt_ellipse)
                {
                    hasEllipse = TRUE;
                    vararg =  chosenAssembler->msil && chosenAssembler->msil->managed(params->sp);
                    params->vararg = vararg;
                    matching = FALSE;
                    decl = NULL;
                    hr = NULL;
                    if (!list)
                        break;
                }
            }
            if (!decl && !hasEllipse && chosenAssembler->msil)
                toolong = TRUE;
            if (matching)
            {
                if (!decl)
                    toolong = TRUE;
                else if (!list)
                    tooshort = TRUE;
                else
                {
                    if (isref(decl->tp))
                    {
                        TYPE *tpb = basetype(basetype(decl->tp)->btp);
                        TYPE *tpd = list->tp;
                        list->byRef = TRUE;
                        if (isref(tpd))
                            tpd = basetype(tpd)->btp;
                        tpd = basetype(tpd);
                        if (isstructured(tpb) && isstructured(tpd))
                        {
                            SYMBOL *base = basetype(tpb)->sp;
                            SYMBOL *derived = basetype(tpd)->sp;
                            if (base != derived && classRefCount(base, derived) != 1)
                                errortype(ERR_CANNOT_CONVERT_TYPE, list->tp, decl->tp);
                            if (base != derived && !isAccessible(derived, derived, base, funcsp, ac_public, FALSE))
                                errorsym(ERR_CANNOT_ACCESS, base);
                        }
                        else
                        {
                            goto join;
                        }
                    }
                    else
                    {
                        assignmentUsages(list->exp, FALSE);
join:
                        if (!list || !list->tp)
                        {
                        }
                        else if (basetype(decl->tp)->type == bt___string)
                        {
                            if (list->exp->type == en_labcon)
                                list->exp->type = en_c_string;
                        }
                        else if (!comparetypes(list->tp, decl->tp, FALSE))
                        {
                            if (basetype(decl->tp)->type == bt___object)
                            {
                                if (!isstructured(list->tp))
                                {
                                    list->exp = exprNode(en_x_object, list->exp, NULL);
                                }
                            }
                            else if (basetype(decl->tp)->type != bt_memberptr)
                                errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                        }
                        else if (assignDiscardsConst(decl->tp, list->tp))
                        {
                            if (cparams.prm_cplusplus)
                                errortype(ERR_CANNOT_CONVERT_TYPE, list->tp, decl->tp);
                            else
                                error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                        }
                        else if (ispointer(decl->tp))
                        {
                            if (!ispointer(list->tp) && (!isarithmeticconst(list->exp) || !isconstzero(decl->tp, list->exp)))
                            {
                                if (!isfunction(list->tp) || !isvoidptr(decl->tp) && (!isfuncptr(decl->tp) || cparams.prm_cplusplus && !comparetypes(basetype(decl->tp)->btp, list->tp, TRUE)))
                                    errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                            }
                            else if (!comparetypes(decl->tp, list->tp, TRUE))
                            {
                                if (!isconstzero(list->tp, list->exp))
                                {
                                    if (!isvoidptr(decl->tp) && !isvoidptr(list->tp))
                                    {
                                        if (cparams.prm_cplusplus)
                                        {
                                            TYPE *tpb = basetype(decl->tp)->btp;
                                            TYPE *tpd = basetype(list->tp)->btp;
                                            if (isstructured(tpb) && isstructured(tpd))
                                            {
                                                SYMBOL *base = basetype(tpb)->sp;
                                                SYMBOL *derived = basetype(tpd)->sp;
                                                if (base != derived && classRefCount(base, derived) != 1)
                                                    errortype(ERR_CANNOT_CONVERT_TYPE, list->tp, decl->tp);
                                                if (isAccessible(derived, derived, base, funcsp, ac_public, FALSE))
                                                    errorsym(ERR_CANNOT_ACCESS, base);
                                            }
                                            else
                                            {
                                                errortype(ERR_CANNOT_CONVERT_TYPE, list->tp, decl->tp);
                                            }
                                        }
                                        else
                                        {
                                            if (!matchingCharTypes(list->tp, decl->tp))
                                                error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                                        }
                                    }
                                }
                            }
                        } 
                        else if (ispointer(list->tp))
                            if (!ispointer(decl->tp))
                                errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                    }
                      dest = decl->tp;
                }
            }
            else if (isint(list->tp))
            {
                if (basetype(list->tp)->type <= bt_int)
                    dest = &stdint;
                else if (!(chosenAssembler->msil))
                    cast(list->tp, &list->exp);
            }
            else if (isfloat(list->tp))
            {
                if (basetype(list->tp)->type < bt_double)
                    dest = &stddouble;
                else if (!(chosenAssembler->msil))
                    cast(list->tp, &list->exp);
            }
            if (dest && list && list->tp && basetype(dest)->type != bt_memberptr && !comparetypes(dest, list->tp, TRUE))
            {
                cast(basetype(dest), &list->exp);
                list->tp = dest;
            }
            else if (dest && list && basetype(dest)->type == bt_enum)
            {
                // this needs to be revisited to get proper typing in C++
                cast(&stdint, &list->exp);
            }
        }
        if (list)
        {
            BOOLEAN ivararg = vararg;
            if (list->exp->type == en_auto && list->exp->v.sp->va_typeof)
                ivararg = FALSE;
            list->vararg = ivararg;
            if (hr)
            {
                TYPE *tp = ((SYMBOL *)hr->p)->tp;
                while (tp && tp->type != bt_pointer)
                {
                    if (tp->type == bt_va_list)
                    {
                        list->valist = TRUE;
                        break;
                    }
                    tp = tp->btp;
                }
            }
            list = list->next;
        }
        if (hr)
            hr = hr->next;
    }
    if (noproto)
        errorsym(ERR_CALL_FUNCTION_NO_PROTO, params->sp);
    else if (toolong)
        errorsym(ERR_PARAMETER_LIST_TOO_LONG, params->sp);
    else if (tooshort)
        errorsym(ERR_PARAMETER_LIST_TOO_SHORT, params->sp);
}
static LEXEME *getInitInternal(LEXEME *lex, SYMBOL *funcsp, INITLIST **lptr, enum e_kw finish, BOOLEAN allowNesting, BOOLEAN allowPack, BOOLEAN toErr, int flags)
{
    if (finish == end)
    {
        SYMBOL *sp = namespacesearch("std", globalNameSpace, FALSE, FALSE);
        if (sp && sp->storage_class == sc_namespace)
        {
            sp = namespacesearch("initializer_list", sp->nameSpaceValues, TRUE, FALSE);
            if (!sp || !sp->tp->syms)
                error(ERR_NEED_INITIALIZER_LIST_H);
        }
    }
    *lptr = NULL;
    lex = getsym(); /* past ( */
    while (!MATCHKW(lex,finish))
    {
        INITLIST *p = Alloc(sizeof(INITLIST));
        if (MATCHKW(lex, begin))
        {
            lex = getInitInternal(lex, funcsp, &p->nested, end, TRUE, FALSE, FALSE, flags);
            *lptr = p;
            lptr = &(*lptr)->next;
            if (!allowNesting)
                error(ERR_CANNOT_USE_INIT_LIST);
        }
        else
        {
            LEXEME *start = lex;
            lex = expression_assign(lex, funcsp, NULL, &p->tp, &p->exp, NULL, _F_PACKABLE | (finish == closepa ? _F_INARGS : 0) | (flags & _F_SIZEOF));
            if (p->tp && isvoid(p->tp))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            optimize_for_constants(&p->exp);
            if (finish != closepa)
                assignmentUsages(p->exp, FALSE);
            if (p->tp)
            {
                if (p->exp && p->exp->type == en_func && 
                    p->exp->v.func->sp->parentClass && !p->exp->v.func->ascall && !p->exp->v.func->asaddress)
                {
                    HASHREC *hr = basetype(p->exp->v.func->functp)->syms->table[0];
                    while (hr)
                    {
                        SYMBOL *sym = (SYMBOL *)hr->p;
                        if (sym->storage_class == sc_member || sym->storage_class == sc_mutable)
                        {
                            error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                            break;
                        }
                        hr = hr->next;
                    }
                }
                if (allowPack && cparams.prm_cplusplus && MATCHKW(lex, ellipse))
                {
                    // lose p
                    lex = getsym();
                    if (templateNestingCount)
                    {
                        *lptr = p;
                        lptr = &(*lptr)->next;
                    }
                    else if (p->exp && p->exp->type != en_packedempty)
                    {
                        checkPackedExpression(p->exp);  
                        // this is going to presume that the expression involved
                        // is not too long to be cached by the LEXEME mechanism.          
                        lptr = expandPackedInitList(lptr, funcsp, start, p->exp);
                    }
                }
                else
                {
                    if (toErr && argument_nesting <= 1)
                        checkUnpackedExpression(p->exp);
                    *lptr = p;
                    lptr = &(*lptr)->next;
                }
            }
            else
            {
                lex = backupsym();
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(&lex, finish == closepa ? skim_closepa : skim_end);
                return lex;
            }
        }
        if (!MATCHKW(lex, comma))
        {
            break;
        }
        lex = getsym();
        if (MATCHKW(lex, finish))
            error(ERR_IDENTIFIER_EXPECTED);
    }
    if (!needkw(&lex, finish))
    {
        errskim(&lex, finish == closepa ? skim_closepa : skim_end);
        skip(&lex, finish);
    }
	return lex;
}
LEXEME *getInitList(LEXEME *lex, SYMBOL *funcsp, INITLIST **owner)
{
    return getInitInternal(lex, funcsp, owner, end, FALSE, TRUE, TRUE, 0);
}
LEXEME *getArgs(LEXEME *lex, SYMBOL *funcsp, FUNCTIONCALL *funcparams, enum e_kw finish, BOOLEAN allowPack, int flags)
{
    LEXEME *rv;
    argument_nesting++;
    rv = getInitInternal(lex, funcsp, &funcparams->arguments, finish, TRUE,allowPack, argument_nesting == 1, flags);
    argument_nesting--;
    return rv;
}
LEXEME *getMemberInitializers(LEXEME *lex, SYMBOL *funcsp, FUNCTIONCALL *funcparams, enum e_kw finish, BOOLEAN allowPack)
{
    LEXEME *rv;
    argument_nesting++;
    rv = getInitInternal(lex, funcsp, &funcparams->arguments, finish, TRUE,allowPack, FALSE, 0);
    argument_nesting--;
    return rv;
}
static int simpleDerivation(EXPRESSION *exp)
{
    int rv = 0;
    while (castvalue(exp))
        exp = exp->left;
    if (exp->left)
        rv |= simpleDerivation(exp->left);
    if (exp->right)
        rv |= simpleDerivation(exp->right);
    switch (exp->type)
    {
        case en_func:
        case en_thisref:
        case en_stmt:
            rv |= 1;
            break;
        case en_labcon:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_label:
        case en_pc:
        case en_threadlocal:
            rv |= 2;
            break;
        default:
            if (lvalue(exp))
                rv |= 1;
            break;
    }
    return rv;
}
EXPRESSION *DerivedToBase(TYPE *tpn, TYPE *tpo, EXPRESSION *exp, int flags)
{
    if (isref(tpn))
        tpn = basetype(tpn)->btp;
    if (isref(tpo))
        tpo = basetype(tpo)->btp;
    if (isstructured(tpn) && isstructured(tpo))
    {
        SYMBOL *spn = basetype(tpn)->sp;
        SYMBOL *spo = basetype(tpo)->sp;
        if (spo != spn && !sameTemplate(tpn, tpo))
        {
            int n = classRefCount(spn, spo);
            if (n == 1)
            {
                // derived to base
                EXPRESSION q, *v = &q;
                memset(&q, 0, sizeof(q));
                v->type = en_c_i;
                v = baseClassOffset(spn, spo, v);

                optimize_for_constants(&v);
                if (v->type != en_c_i && (flags & _F_NOVIRTUALBASE))
                    return NULL;

                v = baseClassOffset(spn, spo, exp);
                if ((flags & _F_VALIDPOINTER) || v->type != en_c_i || simpleDerivation(exp) == 2)
                {
                    optimize_for_constants(&v);
                }
                else
                {
                    EXPRESSION *varsp = anonymousVar(sc_auto, &stdpointer);
                    EXPRESSION *var = exprNode(en_l_p, varsp, NULL);
                    EXPRESSION *asn = exprNode(en_assign, var, exp);
                    EXPRESSION *left = exprNode(en_add, var, v);
                    EXPRESSION *right = var;
                    v = exprNode(en_cond, var, exprNode(en_void, left, right));
                    v = exprNode(en_void, asn, v);
                }
                return v;
            }
        }
    }
    return exp;
}
static BOOLEAN cloneTempExpr(EXPRESSION **expr, SYMBOL **found, SYMBOL **replace);
BOOLEAN cloneTempStmt(STATEMENT **block, SYMBOL **found, SYMBOL **replace)
{
    BOOLEAN rv = FALSE;
    while (*block != NULL)
    {
        if (found)
        {
            STATEMENT *b = Alloc(sizeof(STATEMENT));
            *b = **block;
            *block = b;
        }
        switch ((*block)->type)
        {
            case st__genword:
                break;
            case st_try:
            case st_catch:
                rv |= cloneTempStmt(&(*block)->lower, found, replace);
                rv |= cloneTempStmt(&(*block)->blockTail, found, replace);
                break;
            case st_return:
            case st_expr:
            case st_declare:
                rv |= cloneTempExpr(&(*block)->select, found, replace);
                break;
            case st_goto:
            case st_label:
                break;
            case st_select:
            case st_notselect:
                rv |= cloneTempExpr(&(*block)->select, found, replace);
                break;
            case st_switch:
                rv |= cloneTempExpr(&(*block)->select, found, replace);
                rv |= cloneTempStmt(&(*block)->lower, found, replace);
                break;
            case st_block:
                rv |= cloneTempStmt(&(*block)->lower, found, replace);
                rv |= cloneTempStmt(&(*block)->blockTail, found, replace);
                break;
            case st_passthrough:
                break;
            case st_datapassthrough:
                break;
            case st_nop:
                break;
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            default:
                diag("Invalid block type in cloneTempStmt");
                break;
        }
        block = &(*block)->next;
    }
    return rv;
}
#define CLONED_SYMS 20
static BOOLEAN cloneTempExpr(EXPRESSION **expr, SYMBOL **found, SYMBOL **replace)
{
    BOOLEAN rv = FALSE;
    if (!*expr)
        return rv;
    if (found)
    {
        EXPRESSION *newval = Alloc(sizeof(EXPRESSION));
        *newval = **expr;
        *expr = newval;
    }
    if ((*expr)->type == en_thisref)
    {
        rv = cloneTempExpr(&(*expr)->left, found, replace);
    }
    else if ((*expr)->type == en_stmt)
    {
        rv = cloneTempStmt(&(*expr)->v.stmt, found, replace);
    }
    else if ((*expr)->type == en_func)
    {
        if ((*expr)->v.func->sp->isConstructor || (*expr)->v.func->sp->isDestructor)
        {
            INITLIST **args;
            if (found)
            {
                FUNCTIONCALL *newval = Alloc(sizeof(FUNCTIONCALL));
                *newval = *(*expr)->v.func;
                (*expr)->v.func = newval;
            }
            args = &(*expr)->v.func->arguments;
            while (*args)
            {
                if (found)
                {
                    INITLIST *newval = Alloc(sizeof(INITLIST));
                    *newval = **args;
                    *args = newval;
                }
                rv |= cloneTempExpr(&(*args)->exp, found, replace);
                args = &(*args)->next;
            }
            rv |= cloneTempExpr(&(*expr)->v.func->thisptr, found, replace); 
        }
    }
    else
    {
        rv |= cloneTempExpr(&(*expr)->left, found, replace);
        rv |= cloneTempExpr(&(*expr)->right, found, replace);
        if ((*expr)->type == en_auto)
        {
            SYMBOL *sp = (*expr)->v.sp;
            if (sp->anonymous && !sp->stackblock)
            {
                if (found)
                {
                    int i = 0;
                    EXPRESSION *newexp = Alloc(sizeof(EXPRESSION));
                    *newexp = **expr;
                    *expr = newexp;
                    for (i=0; i < CLONED_SYMS && found[i]; i++)
                    {
                        if (found[i] == sp)
                            break;
                    }
                    if (i >= CLONED_SYMS)
                    {
                        diag("cloneTempExpr - too many syms");
                    }
                    else
                    {
                        if (!found[i])
                        {
                            found[i] = sp;
                            replace[i] = clonesym(sp);
                            if (theCurrentFunc)
                                replace[i] ->value.i = theCurrentFunc->value.i;
                            replace[i]->name = AnonymousName();
                        }
                        (*expr)->v.sp = replace[i];
                    }
                }
                rv = TRUE;
            }
        }
    }
    return rv;
}
void CreateInitializerList(TYPE *initializerListTemplate, TYPE *initializerListType, 
                           INITLIST **lptr, BOOLEAN operands, BOOLEAN asref)
{
    INITLIST **initial = lptr;
    EXPRESSION *rv = NULL , **pos = &rv;
    int count = 0, i;
    INITLIST *searchx = *lptr;
    TYPE *tp = Alloc(sizeof(TYPE));
    EXPRESSION *data, *initList;
    SYMBOL *start, *end;
    EXPRESSION *dest;
    start = search("_M_start", basetype(initializerListTemplate)->syms);
    end = search("_M_end", basetype(initializerListTemplate)->syms);
    if (!start || !end)
        fatal("Invalid definition of initializer-list");
    if (!(*initial)->nested && comparetypes((*initial)->tp, initializerListTemplate, TRUE))
    {
        *initial = Alloc(sizeof(INITLIST));
        if (asref)
        {
            (*initial)->tp = Alloc(sizeof(TYPE));
            (*initial)->tp->size = getSize(bt_pointer);
            (*initial)->tp->type = bt_lref;
            (*initial)->tp->btp =  initializerListTemplate;
            (*initial)->tp->rootType =  (*initial)->tp;
            (*initial)->exp = searchx->exp;
        }
        else
        {
            (*initial)->tp =  initializerListTemplate;
            (*initial)->exp = exprNode(en_stackblock, searchx->exp, NULL);
            (*initial)->exp->size = basetype(initializerListTemplate)->size;
        }
    }
    else
    {
        tp->type = bt_pointer;
        tp->array = TRUE;
        while (searchx)
            count++, searchx = searchx->next;
        tp->btp = initializerListType;
        tp->rootType = tp;
        tp->size = count * (initializerListType->size);
        tp->esize = intNode(en_c_i, count);
        data = anonymousVar(sc_auto, tp);
        if (isstructured(initializerListType))
        {
            EXPRESSION *exp = data;
            EXPRESSION *elms = intNode(en_c_i, count);
            callDestructor(initializerListType->sp, NULL, &exp, elms, TRUE, FALSE, FALSE );
            initInsert(&data->v.sp->dest, tp, exp, 0, FALSE);
            
        }
        for (i=0 ; i < count; i++, lptr = &(*lptr)->next)
        {
            EXPRESSION *node;
            dest = exprNode(en_add, data, intNode(en_c_i, i * (initializerListType->size)));
            if (isstructured(initializerListType))
            {
                TYPE *ctype = initializerListType;
                EXPRESSION *cdest = dest;
                FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
                INITLIST *arg = Alloc(sizeof(INITLIST));
                params->arguments = arg;
                *arg = (*lptr)->nested ? *(*lptr)->nested : **lptr;
                if (!(*lptr)->nested)
                {
                    arg->next = NULL;
                }
                
                callConstructor(&ctype, &cdest, params, FALSE, NULL, TRUE, FALSE, FALSE, FALSE, _F_INITLIST);
                node = cdest;
    
            }
            else
            {
                EXPRESSION *src = (*lptr)->exp;
                deref(initializerListType, &dest);
                node = exprNode(en_assign, dest, src);
            }
            if (rv)
            {
                *pos = exprNode(en_void, *pos, node);
                pos = &(*pos)->right;
            }
            else
            {
                rv = node;
            }
        }
        initList = anonymousVar(sc_auto, initializerListTemplate);
        dest = exprNode(en_add, initList, intNode( en_c_i, start->offset));
        deref(&stdpointer, &dest);
        dest = exprNode(en_assign, dest, data);
        if (rv)
        {
            *pos = exprNode(en_void, *pos, dest);
            pos = &(*pos)->right;
        }
        else
        {
            rv = dest;
        }
        dest = exprNode(en_add, initList, intNode( en_c_i, end->offset));
        deref(&stdpointer, &dest);
        dest = exprNode(en_assign, dest, exprNode(en_add, data, intNode(en_c_i, tp->size)));
        if (rv)
        {
            *pos = exprNode(en_void, *pos, dest);
            pos = &(*pos)->right;
        }
        else
        {
            rv = dest;
        }
        *initial = Alloc(sizeof(INITLIST));
        if (asref)
        {
            (*initial)->tp = Alloc(sizeof(TYPE));
            (*initial)->tp->size = getSize(bt_pointer);
            (*initial)->tp->type = bt_lref;
            (*initial)->tp->btp =  initializerListTemplate;
            (*initial)->tp->rootType =  (*initial)->tp;
            (*initial)->exp = exprNode(en_void, rv, initList);
        }
        else
        {
            (*initial)->tp =  initializerListTemplate;
            (*initial)->exp = exprNode(en_stackblock, exprNode(en_void, rv, initList), NULL);
            (*initial)->exp->size = basetype(initializerListTemplate)->size;
        }
    }
}
void AdjustParams(SYMBOL *func, HASHREC *hr, INITLIST **lptr, BOOLEAN operands, BOOLEAN implicit)
{
    if (func->storage_class == sc_overloads)
        return;
    if (hr && ((SYMBOL *)hr->p)->thisPtr)
        hr = hr->next;
    while (hr && (*lptr || ((SYMBOL *)hr->p)->init != NULL || ((SYMBOL *)hr->p)->deferredCompile != NULL
                   && (!templateNestingCount || instantiatingTemplate)))
    {
        SYMBOL *sym= (SYMBOL *)hr->p;
        EXPRESSION *exp = NULL;
        INITLIST *p;

        if (sym->deferredCompile && !sym->init)
        {
            LEXEME *lex;
            STRUCTSYM l,m, n;
            TYPE *tp2;
            int count = 0;
            int tns = PushTemplateNamespace(func);
            l.str = func;
            addStructureDeclaration(&l);
            count++;
        
            if (func->templateParams)
            {
                n.tmpl = func->templateParams;
                addTemplateDeclaration(&n);
                count++;
            }
            sym->tp = PerformDeferredInitialization(sym->tp, NULL);
            lex = SetAlternateLex(sym->deferredCompile);

            tp2 = sym->tp;
            if (isref(tp2))
                tp2 = basetype(tp2)->btp;
            if (isstructured(tp2))
            {
                SYMBOL *sym2;
                anonymousNotAlloc++;
                sym2 = anonymousVar(sc_auto, tp2)->v.sp;
                anonymousNotAlloc--;
                sym2->stackblock = !isref(sym->tp);
                lex = initialize(lex, theCurrentFunc, sym2, sc_auto, FALSE, 0); /* also reserves space */
                sym->init = sym2->init;
                if (sym->init->exp->type == en_thisref)
                {
                    EXPRESSION **expr = &sym->init->exp->left->v.func->thisptr;
                    if ((*expr)->type == en_add && isconstzero(&stdint, (*expr)->right))
                        sym->init->exp->v.t.thisptr = (*expr) = (*expr)->left;
                }
            }
            else
            {
                lex = initialize(lex, theCurrentFunc, sym, sc_member, FALSE, 0);
            }
            SetAlternateLex(NULL);
            sym->deferredCompile = NULL;
            while (count--)
            {
                dropStructureDeclaration();
            }
            PopTemplateNamespace(tns);
        }            
        if (!*lptr)
        {
            EXPRESSION *q = sym->init->exp;
            optimize_for_constants(&q);
            *lptr = Alloc(sizeof(INITLIST));
            (*lptr)->exp = q;
            (*lptr)->tp = sym->tp;
            if (cloneTempExpr(&(*lptr)->exp, NULL, NULL))
            {
                int i;
                SYMBOL *ths[CLONED_SYMS], *newval[CLONED_SYMS];
                memset(ths, 0, sizeof(ths));
                memset(newval, 0, sizeof(newval));
                cloneTempExpr(&(*lptr)->exp, &ths[0], &newval[0]);
                for (i = 0; i < CLONED_SYMS && ths[i]; i++)
                    if (ths[i] && ths[i]->dest)
                    {
                        EXPRESSION *exp = ths[i]->dest->exp;
                        cloneTempExpr(&exp, &ths[0], &newval[0]);
                        (*lptr)->dest = exp;
                    }
            }
            if (isstructured(sym->tp))
            {
                hr = hr->next;
                lptr = &(*lptr)->next;
                continue;
            }
        }
        p = *lptr;
        if (p && p->exp && (p->exp->type == en_pc || p->exp->type == en_func))
        {
            if (chosenAssembler->msil)
            {
                ValidateMSILFuncPtr(func->tp, p->tp, &p->exp);
            }
            thunkForImportTable(&p->exp);
        }
        if (cparams.prm_cplusplus)
        {
            BOOLEAN done = FALSE;
            if (!done && !p->tp)
            {
                // initlist
                INITLIST *pinit = p->nested;
                if (isstructured(sym->tp) || (isref(sym->tp) && isstructured(basetype(sym->tp)->btp)))
                {
                    TYPE *stype = sym->tp;
                    SYMBOL *sp;
                    EXPRESSION *thisptr;
                    if (isref(stype))
                        stype = basetype(stype)->btp;
                    thisptr = anonymousVar(theCurrentFunc ? sc_auto : sc_localstatic, stype);
                    sp = thisptr->v.sp;
                    if (!theCurrentFunc)
                    {
                        sp->label = nextLabel++;
                        insertInitSym(sp);
                    }
                    if (stype->sp->trivialCons)
                    {
                        INITIALIZER *init = NULL, **it = &init;
                        HASHREC *hr = stype->syms->table[0];
                        while (pinit && hr)
                        {
                            SYMBOL *shr = (SYMBOL *)hr->p;
                            if (ismemberdata(shr))
                            {
                                initInsert(it, pinit->tp, pinit->exp, shr->offset, FALSE);
                                it = &(*it)->next;
                                pinit = pinit->next;
                            }
                            hr = hr->next;
                        }
                        p->exp = convertInitToExpression(stype, NULL, theCurrentFunc, init, thisptr, FALSE);
                        if (!isref(sym->tp))
                            sp->stackblock = TRUE;
                        done = TRUE;
                    }
                    else
                    {
                        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                        TYPE *ctype = sp->tp;
                        EXPRESSION *dexp = thisptr;
                        funcparams->arguments = pinit;
                        p->exp = thisptr;
                        callConstructor(&ctype, &p->exp, funcparams, FALSE, NULL, TRUE, TRUE, implicit, FALSE, FALSE);
                        if (!isref(sym->tp))
                        {
                            sp->stackblock = TRUE;
                        }
                        else
                        {
                            callDestructor(stype->sp, NULL, &dexp, NULL, TRUE, FALSE, FALSE);
                            if (dexp)
                                p->dest = dexp;
                        }
                        done = TRUE;
                    }
                    p->tp = sym->tp;
                }
                else if (ispointer(sym->tp))
                {
                    INITIALIZER *init = NULL, **it = &init;
                    EXPRESSION *thisptr = anonymousVar(theCurrentFunc ? sc_auto : sc_localstatic, sym->tp);
                    SYMBOL *sp = thisptr->v.sp;
                    int n = 0;
                    TYPE *btp = sym->tp;
                    while (isarray(btp))
                        btp = basetype(btp)->btp;
                    if (!theCurrentFunc)
                    {
                        sp->label = nextLabel++;
                        insertInitSym(sp);
                    }
                    if (!isarray(sym->tp))
                    {
                        TYPE *gtype = Alloc(sizeof(TYPE));
                        INITLIST *xx = pinit;
                        *gtype = *sym->tp;
                        UpdateRootTypes(gtype);
                        gtype->array = TRUE;
                        gtype->esize = intNode(en_c_i, n);
                        while (xx)
                        {
                            n ++;
                            xx = xx->next;
                        }
                        sym->tp = gtype;
                        sym->tp->size = btp->size * n;
                    }
                    n = 0;
                    while (pinit)
                    {
                        initInsert(it, pinit->tp, pinit->exp, n, FALSE);
                        it = &(*it)->next;
                        n += btp->size;
                        pinit = pinit->next;
                    }
                    p->exp = convertInitToExpression(sym->tp, NULL, theCurrentFunc, init, thisptr, FALSE);
                    p->tp = sym->tp;
                    done = TRUE;
                }
                else
                {
                    // defer to below processing
                    if (pinit)
                    {
                        p->exp = pinit->exp;
                        p->tp = pinit->tp;
                    }
                    else
                    {
                        p->exp = intNode(en_c_i, 0);
                        p->tp = &stdint;
                    }
                }
            }
            if (!done && p->exp)
            {
                if (isstructured(sym->tp))
                {
                    BOOLEAN sameType = FALSE;
                    EXPRESSION *temp = p->exp;
                    TYPE *tpx = p->tp;
                    sym->tp = PerformDeferredInitialization(sym->tp, NULL);
                    if (isref(tpx))
                        tpx = basetype(tpx)->btp;
                    // result of a nested constructor
                    if (temp->type == en_thisref)
                    {
                        temp = p->exp->left;
                    }
                    // use constructor or conversion function and push on stack ( no destructor)
                    if (temp->type == en_func && basetype(temp->v.func->sp->tp)->btp && !isref(basetype(temp->v.func->sp->tp)->btp) &&((sameType = comparetypes(sym->tp, tpx, TRUE)) || classRefCount(basetype(sym->tp)->sp, basetype(tpx)->sp) == 1))
                    {
                        EXPRESSION **exp = NULL;
                        SYMBOL *esp;
                        EXPRESSION *consexp;
                        TYPE *tp;
                        // copy constructor...
                        TYPE *ctype = sym->tp;
                        EXPRESSION *exp1 = NULL;
                        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                        INITLIST *arg = Alloc(sizeof(INITLIST));
                        consexp = anonymousVar(sc_auto, sym->tp); // sc_parameter to push it...
                        esp = consexp->v.sp;
                        esp->stackblock = TRUE;
                        consexp = varNode(en_auto, esp);
                        arg->exp = temp->v.func->returnEXP ? temp->v.func->returnEXP : temp->v.func->thisptr;
                        arg->tp = sym->tp;
                        arg->exp = DerivedToBase(sym->tp, tpx, arg->exp, _F_VALIDPOINTER);
                        funcparams->arguments = arg;
                        callConstructor(&ctype, &consexp, funcparams, FALSE, NULL, TRUE, TRUE, implicit, FALSE, FALSE);
                        p->exp = exprNode(en_void, p->exp, consexp);
                    }
                    else
                    {
                        EXPRESSION *x;
                        TYPE *ctype = sym->tp;
                        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                        INITLIST *arg = Alloc(sizeof(INITLIST));
                        EXPRESSION *consexp = anonymousVar(sc_auto, sym->tp); // sc_parameter to push it...
                        SYMBOL *esp = consexp->v.sp;
                        EXPRESSION *destexp = consexp;
                        EXPRESSION *old = p->exp;
                        esp->stackblock = TRUE;
                        arg->exp = p->exp;
                        arg->tp = p->tp;
                        funcparams->arguments = arg;
                        callConstructor(&ctype, &consexp, funcparams, FALSE, NULL, TRUE, TRUE, implicit, FALSE, FALSE);
                        p->exp=consexp;
                    }
                    p->tp = sym->tp;
                }
                else if (isref(sym->tp))
                {
                    if (isstructured(basetype(sym->tp)->btp))
                    {
                        TYPE *tpx = p->tp;
                        if (isref(tpx))
                            tpx = basetype(tpx)->btp;
                        if ((!isconst(basetype(sym->tp)->btp) && isconst(tpx)) || (!comparetypes(sym->tp, tpx, TRUE) && !sameTemplate(sym->tp, tpx) 
                            && !classRefCount(basetype(basetype(sym->tp)->btp)->sp, basetype(tpx)->sp)))
                        { 
                            // make temp via constructor or conversion function
                            EXPRESSION *consexp = anonymousVar(sc_auto, basetype(sym->tp)->btp); // sc_parameter to push it...
                            SYMBOL *esp = consexp->v.sp;
                            EXPRESSION *destexp = consexp;
                            TYPE *ctype = basetype(sym->tp)->btp;
                            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                            INITLIST *arg = Alloc(sizeof(INITLIST));
                            arg->exp = p->exp;
                            arg->tp = basetype(p->tp);
                            funcparams->arguments = arg;
                            p->exp = consexp;
                            callConstructor(&ctype, &p->exp, funcparams, FALSE, NULL, TRUE, TRUE, FALSE, FALSE, FALSE); 
                            if (p->exp->type == en_func)
                            {
                                SYMBOL *spx = p->exp->v.func->sp;
                                TYPE *tpx = basetype(spx->tp);
                                TYPE *tpx1, *tpx2;
                                HASHREC *hr1;
                                if (spx->castoperator)
                                {
                                    tpx1 = spx->parentClass->tp;
                                    tpx2 = tpx->btp;
                                    if (isref(tpx2))
                                        tpx2 = basetype(tpx2)->btp;
                                }
                                else
                                {
                                    tpx1 = ((SYMBOL *)tpx->syms->table[0]->next->p)->tp;
                                    tpx2 = spx->parentClass->tp;
                                }
                                esp->tp = tpx1; // guaranteed to be a structured type or reference to one
                                p->exp->v.func->thisptr = DerivedToBase(tpx1, p->tp, p->exp->v.func->thisptr, _F_VALIDPOINTER);
                                if (isstructured(tpx2) || (isref(tpx2) && isstructured(basetype(tpx2)->btp)))
                                    p->exp = DerivedToBase(sym->tp, tpx2, p->exp, 0);
                                else
                                    cast(sym->tp, &p->exp);
                            }
                        }
                        else
                        {
                            if (!comparetypes(sym->tp, p->tp, TRUE))
                                p->exp = DerivedToBase(sym->tp, p->tp, p->exp, 0);
                        }
                    }
                    else if (basetype(basetype(sym->tp)->btp)->type == bt_memberptr)
                    {
                        TYPE *tp2 = basetype(sym->tp)->btp;
                        if (p->exp->type == en_memberptr)
                        {
                            int lbl = dumpMemberPtr(p->exp->v.sp, tp2, TRUE);
                            p->exp = intNode(en_labcon, lbl);
                        }
                        else if (isconstzero(p->tp, p->exp) || p->exp->type == en_nullptr)
                        {
                            EXPRESSION *dest = createTemporary(tp2, NULL);
                            p->exp = exprNode(en_blockclear, dest, NULL);
                            p->exp->size = tp2->size;
                            p->exp = exprNode(en_void, p->exp, dest);
                        }
                        else if (p->exp->type == en_func && p->exp->v.func->returnSP)
                        {
                            EXPRESSION *dest = anonymousVar(sc_auto, tp2);
                            int lbl = dumpMemberPtr(p->exp->v.sp, tp2, TRUE);
                            p->exp = intNode(en_labcon, lbl);
                            /*
                            EXPRESSION *dest = createTemporary(tp2, NULL);
                            p->exp->v.func->returnSP->allocate = FALSE;
                            p->exp->v.func->returnEXP = dest;
                            p->exp = exprNode(en_void, p->exp, dest) ;
                            p->exp = exprNode(en_stackblock, p->exp, NULL);
                            p->exp->size = tp2->size;
                            */
                        }
                        else if (p->exp->type == en_pc)
                        {
                            int lbl = dumpMemberPtr(p->exp->v.sp, tp2, TRUE);
                            p->exp = intNode(en_labcon, lbl);
                        }
                        p->tp = sym->tp;
                    }
                    else if (comparetypes(sym->tp, p->tp, TRUE))
                    {
                        if (isarithmeticconst(p->exp) || (basetype(sym->tp)->type != bt_rref && !isconst(basetype(sym->tp)->btp) && isconst(p->tp)))
                        {
                            // make numeric temp and perform cast
                            p->exp = createTemporary(sym->tp, p->exp);
                        }
                        else
                        {
                            // pass address
                            EXPRESSION *exp = p->exp;
                            while (castvalue(exp) || exp->type == en_not_lvalue)
                                exp = exp->left;
                            if (exp->type != en_l_ref)
                            {
                                if (!isref(sym->tp) || !isfunction(basetype(sym->tp)->btp))
                                {
                                    if (!lvalue(exp))
                                    {
                                        // make numeric temp and perform cast
                                        exp = createTemporary(sym->tp, exp);
                                    }
                                    else
                                    {
                                        exp = exp->left; // take address
                                    }
                                }
                                p->exp = exp;
                            }
                            else if (ispointer(p->tp) && isstructured(basetype(p->tp)->btp))
                            {
                                // make numeric temp and perform cast
                                p->exp = createTemporary(sym->tp, exp);
                            }
                        }
                    }
                    else if (isstructured(p->tp))
                    {
                        // arithmetic or pointer
                        TYPE *etp = basetype(sym->tp)->btp;
                        if (cppCast(p->tp, &etp, &p->exp))
                            p->tp = etp;
                        p->exp = createTemporary(sym->tp, p->exp);
                    }
                    else
                    {
                        // make numeric temp and perform cast
                        p->exp = createTemporary(sym->tp, p->exp);
                    }
                    p->tp = sym->tp;
                }
                else if (isstructured(p->tp))
                {
                    if (sym->tp->type == bt_ellipse)
                    {
                        p->exp = exprNode(en_stackblock, p->exp, NULL);
                        p->exp->size = p->tp->size;
                    }
                    else
                    {
                        // arithmetic or pointer
                        TYPE *etp = sym->tp;
                        if (cppCast(p->tp, &etp, &p->exp))
                            p->tp = etp;
                    }
                }
                else if (isvoidptr(sym->tp) && p->tp->type == bt_aggregate)
                {
                    HASHREC *hr = p->tp->syms->table[0];
                    p->exp = varNode(en_pc, (SYMBOL *)hr->p);
                }
                else if (ispointer(sym->tp) && ispointer(p->tp))
                {
                    // handle base class conversion
                    TYPE *tpb = basetype(sym->tp)->btp;
                    TYPE *tpd = basetype(p->tp)->btp;
                    if (!comparetypes(basetype(tpb), basetype(tpd), TRUE))
                    {
                        if (isstructured(tpb) && isstructured(tpd))
                        {
                            p->exp = DerivedToBase(tpb, tpd, p->exp, 0);
                        }
                        p->tp = sym->tp;
                    }
                }
                else if (basetype(sym->tp)->type == bt_memberptr)
                {
                    if (p->exp->type == en_memberptr)
                    {
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, TRUE);
                        p->exp = intNode(en_labcon, lbl);
                        p->exp = exprNode(en_stackblock, p->exp, NULL);
                        p->exp->size = sym->tp->size;
                    }
                    else if (isconstzero(p->tp, p->exp) || p->exp->type == en_nullptr)
                    {
                        EXPRESSION *dest = createTemporary(sym->tp, NULL);
                        p->exp = exprNode(en_blockclear, dest, NULL);
                        p->exp->size = sym->tp->size;
                        p->exp = exprNode(en_void, p->exp, dest);
                        p->exp = exprNode(en_stackblock, p->exp, NULL);
                        p->exp->size = sym->tp->size;
                    }
                    else if (p->exp->type == en_func && p->exp->v.func->returnSP)
                    {
                        EXPRESSION *dest = anonymousVar(sc_auto, sym->tp);
                        SYMBOL *esp = dest->v.sp;
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, TRUE);
                        esp->stackblock = TRUE;
                        p->exp = intNode(en_labcon, lbl);
                        p->exp = exprNode(en_stackblock, p->exp, NULL);
                        p->exp->size = sym->tp->size;
                        /*
                        EXPRESSION *dest = createTemporary(sym->tp, NULL);
                        p->exp->v.func->returnSP->allocate = FALSE;
                        p->exp->v.func->returnEXP = dest;
                        p->exp = exprNode(en_void, p->exp, dest) ;
                        p->exp = exprNode(en_stackblock, p->exp, NULL);
                        p->exp->size = sym->tp->size;
                        */
                    }
                    else if (p->exp->type == en_pc)
                    {
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, TRUE);
                        p->exp = intNode(en_labcon, lbl);
                        p->exp = exprNode(en_stackblock, p->exp, NULL);
                        p->exp->size = sym->tp->size;
                    }
                    else
                    {
                        p->exp = exprNode(en_stackblock, p->exp, NULL);
                        p->exp->size = sym->tp->size;
                    }
                    p->tp = sym->tp;
                }
            }
        }
        else if (chosenAssembler->msil)
        {
            if (isref(sym->tp))
            {
                if (comparetypes(sym->tp, p->tp, TRUE))
                {
                    if (isarithmeticconst(p->exp) || (basetype(sym->tp)->type != bt_rref && !isconst(basetype(sym->tp)->btp) && isconst(p->tp)))
                    {
                        // make numeric temp and perform cast
                        p->exp = createTemporary(sym->tp, p->exp);
                    }
                    else
                    {
                        // pass address
                        EXPRESSION *exp = p->exp;
                        while (castvalue(exp) || exp->type == en_not_lvalue)
                            exp = exp->left;
                        if (exp->type != en_l_ref)
                        {
                            if (!isref(sym->tp) || !isfunction(basetype(sym->tp)->btp))
                            {
                                if (!lvalue(exp))
                                {
                                    // make numeric temp and perform cast
                                    exp = createTemporary(sym->tp, exp);
                                }
                                else
                                {
                                    exp = exp->left; // take address
                                }
                            }
                            p->exp = exp;
                        }
                        else if (ispointer(p->tp) && isstructured(basetype(p->tp)->btp))
                        {
                            // make numeric temp and perform cast
                            p->exp = createTemporary(sym->tp, exp);
                        }
                    }
                }
                else
                {
                    // make numeric temp and perform cast
                    p->exp = createTemporary(sym->tp, p->exp);
                }
                p->tp = sym->tp;
            }
            else
            {
                if (basetype(sym->tp)->type == bt___string && (basetype(p->tp)->type == bt___string || p->exp->type == en_labcon && p->exp->string))
                {
                    if (p->exp->type == en_labcon)
                        p->exp->type == en_c_string;
                }
                else if (basetype(sym->tp)->type == bt___object)
                {
                    if (basetype(p->tp)->type != bt___object)
                       p->exp = exprNode(en_x_object, p->exp, NULL);
                }
                else if (ismsil(p->tp))
                    ; // error
                // legacy c language support
                else if (p && p->tp && isstructured(p->tp))
                {
                    p->exp = exprNode(en_stackblock, p->exp, NULL);
                    p->exp->size = p->tp->size;
                }
            }
        }
        else
        {
            // legacy c language support
            if (p && p->tp && isstructured(p->tp))
            {
                p->exp = exprNode(en_stackblock, p->exp, NULL);
                p->exp->size = p->tp->size;
            }
        }   
        hr = hr->next;
        lptr = &(*lptr)->next;
    }
    while (*lptr) // take care of elliptical arguments and arguments without a prototype
    {
        INITLIST *p = *lptr;
        if (func->msil && p->exp->type == en_labcon && p->exp->string)
        {
            p->exp->type = en_c_string;
            p->tp = &(std__string);
        }
        if (isstructured(p->tp))
        {
            p->exp = exprNode(en_stackblock, p->exp, NULL);
            p->exp->size = p->tp->size;
        }
        else if (p->tp->type == bt_float)
        {
            cast(&stddouble, &p->exp);
        }
        lptr = &(*lptr)->next;
    }
}
LEXEME *expression_arguments(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    TYPE *tp_cpp = *tp;
    EXPRESSION *exp_cpp = *exp;
    FUNCTIONCALL *funcparams;
    INITLIST **lptr ;
    EXPRESSION *exp_in = *exp;
    BOOLEAN operands = FALSE;
    BOOLEAN hasThisPtr = FALSE;
    TYPE *initializerListType = NULL;
    TYPE *initializerListTemplate = NULL;
    BOOLEAN initializerRef = FALSE;
    BOOLEAN addedThisPointer = FALSE;
    BOOLEAN memberPtr = FALSE;
    if (exp_in->type != en_func || isfuncptr(*tp) || isstructured(*tp))
    {
        TYPE *tpx = *tp;
        SYMBOL *sym;
        funcparams = Alloc(sizeof(FUNCTIONCALL));
        if (ispointer(tpx))
            tpx = basetype(tpx)->btp;
        sym = basetype(tpx)->sp;
        if (sym)
        {
            funcparams->sp = sym;
            funcparams->functp = sym->tp;
            funcparams->fcall = *exp;
            *exp = varNode(en_func, NULL);
            (*exp)->v.func = funcparams;
        }
        else if (!templateNestingCount)
            error(ERR_CALL_OF_NONFUNCTION);
    }
    else {
        SYMBOL *ss = getStructureDeclaration();
        funcparams = exp_in->v.func;
        hasThisPtr = funcparams->thisptr != NULL;
        if (ss)
        {
            funcparams->functp = ss->tp;
        }
    }
    if ((!templateNestingCount || instantiatingTemplate) && funcparams->sp && funcparams->sp->name[0] == '_' && parseBuiltInTypelistFunc(&lex, funcsp, funcparams->sp, tp, exp))
        return lex;
    if (lex)
    {
        lex = getArgs(lex, funcsp, funcparams, closepa, TRUE, flags);
    }
        
    if (funcparams->astemplate)
    {
        // if we hit a packed template param here, then this is going to be a candidate
        // for some other function's packed expression
        TEMPLATEPARAMLIST *tl = funcparams->templateParams;
        while (tl)
        {
            if (tl->p->packed)
            {
                return lex;
            }
            tl = tl->next;
        }
    }
    if (*tp)
        getFunctionSP(tp);
//    if ((*exp)->type == en_funcret)
//    {
//        (*exp)->v.func = funcparams;
//        *exp = exprNode(en_funcret, *exp, NULL);
//        return lex;
//    }
    if (chosenAssembler->msil && funcparams->sp)
    {
        if (funcparams->sp->storage_class == sc_overloads)
        {
            // note at this pointer the arglist does NOT have the this pointer,
            // it will be added after we select a member function that needs it.
            funcparams->ascall = TRUE;
            SYMBOL *sp = GetOverloadedFunction(tp, &funcparams->fcall, funcparams->sp, funcparams, NULL, TRUE, FALSE, TRUE, flags);
            if (sp)
            {
                funcparams->sp = sp;
                *tp = sp->tp;
            }
        }
    }
    else if ((cparams.prm_cplusplus || chosenAssembler->msil) && funcparams->sp)
    {
        SYMBOL *sp = NULL;
        // add this ptr
        if (!funcparams->thisptr && funcparams->sp->parentClass && !isfuncptr(funcparams->sp->tp))
        {
            TYPE *tp = Alloc(sizeof(TYPE)), *tpx;
            funcparams->thisptr = getMemberBase(funcparams->sp, NULL, funcsp, FALSE);
            funcparams->thistp = tp;
            tp->type = bt_pointer;
            tp->size = getSize(bt_pointer);
            tpx = tp;
            if (funcsp)
            {
                if (isconst(funcsp->tp))
                {
                    tpx = tpx->btp = Alloc(sizeof(TYPE));
                    tpx->size = basetype(funcparams->sp->parentClass->tp)->size;
                    tpx->type = bt_const;
                }
                if (isvolatile(funcsp->tp))
                {
                    tpx = tpx->btp = Alloc(sizeof(TYPE));
                    tpx->size = basetype(funcparams->sp->parentClass->tp)->size;
                    tpx->type = bt_volatile;
                }
            }
            tpx->btp = funcparams->sp->parentClass->tp;
            UpdateRootTypes(tp);
            addedThisPointer = TRUE;
        }
        // we may get here with the overload resolution already done, e.g.
        // for operator or cast function calls...
        if (funcparams->sp->storage_class == sc_overloads)
        {
            TYPE *tp1;
            // note at this pointer the arglist does NOT have the this pointer,
            // it will be added after we select a member function that needs it.
            funcparams->ascall = TRUE;
            sp = GetOverloadedFunction(tp, &funcparams->fcall, funcparams->sp, funcparams, NULL, TRUE, FALSE, TRUE, flags);
            tp1 = *tp;
            while (tp1->btp && tp1->type != bt_bit)
                tp1 = tp1->btp;
            if (sp)
            {
                if (sp->decoratedName[0] == '@' && lex)
                    browse_usage(sp, lex->filenum);
                if (funcparams->astemplate && sp->templateLevel && !sp->specialized)
                {
                    TEMPLATEPARAMLIST *tpln = funcparams->templateParams;
                    TEMPLATEPARAMLIST *tplo = sp->parentTemplate->templateParams->next;
                    while (tpln && tplo)
                    {
                        tpln = tpln->next;
                        tplo = tplo->next;
                    }
                    if (tpln)
                        errorsym(ERR_TOO_MANY_ARGS_PASSED_TO_TEMPLATE, sp);
                }
                sp->throughClass = funcparams->sp->throughClass;
                funcparams->sp = sp;
                if (ismember(sp))
                {
                    if (funcparams->noobject)
                    {
                        if (!funcsp->parentClass || classRefCount(sp->parentClass, funcsp->parentClass) == 0)
                        {
                            errorsym(ERR_USE_DOT_OR_POINTSTO_TO_CALL, sp);
                        }
                        else if (classRefCount(sp->parentClass, funcsp->parentClass) != 1)
                        {
                            errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, sp->parentClass, funcsp->parentClass);
                        }
                        else if (funcsp->storage_class == sc_member || funcsp->storage_class == sc_virtual)
                        {
                            TYPE **cur;
                            funcparams->thisptr = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p);
                            deref(&stdpointer, &funcparams->thisptr);
                            funcparams->thisptr = DerivedToBase(sp->parentClass->tp, basetype(funcparams->thisptr->left->v.sp->tp)->btp, funcparams->thisptr, _F_VALIDPOINTER);
                            funcparams->thistp = Alloc(sizeof(TYPE));
                            cur = &funcparams->thistp->btp;
                            funcparams->thistp->type = bt_pointer;
                            funcparams->thistp->size = getSize(bt_pointer);
                            if (isconst(sp->tp))
                            {
                                (*cur) = Alloc(sizeof(TYPE));
                                (*cur)->type = bt_const;
                                (*cur)->size = sp->parentClass->tp->size;
                               cur = &(*cur)->btp;
                            }
                            if (isvolatile(sp->tp))
                            {
                                (*cur) = Alloc(sizeof(TYPE));
                                (*cur)->type = bt_volatile;
                                (*cur)->size = sp->parentClass->tp->size;
                                cur = &(*cur)->btp;
                            }
                            *cur = sp->parentClass->tp;
                            UpdateRootTypes(funcparams->thistp->btp);
                            cppCast(((SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p)->tp, &funcparams->thistp, &funcparams->thisptr);
                        }
                    }
                }
            }
        }
        else
        {
            operands = !ismember(funcparams->sp) && funcparams->thisptr && !addedThisPointer;
            if (!isExpressionAccessible(funcparams->thistp ? basetype(basetype(funcparams->thistp)->btp)->sp : NULL, funcparams->sp, funcsp, funcparams->thisptr, FALSE))
                if (!isExpressionAccessible(funcparams->thistp ? basetype(basetype(funcparams->thistp)->btp)->sp : NULL, funcparams->sp, funcsp, funcparams->thisptr, FALSE))
                    errorsym(ERR_CANNOT_ACCESS, funcparams->sp);		
        }
        if (sp)
        {
            BOOLEAN test;
            *tp = sp->tp;
            if (hasThisPtr)
            {
                test = isExpressionAccessible(basetype(basetype(funcparams->thistp)->btp)->sp, sp, funcsp, funcparams->thisptr, FALSE );
            }
            else
            {
                test = isExpressionAccessible(funcparams->thistp ? basetype(basetype(funcparams->thistp)->btp)->sp : NULL, sp, funcsp, funcparams->thisptr, FALSE);
            }
            if (!test)
            {
                errorsym(ERR_CANNOT_ACCESS, sp);		
            }
            if (ismember(funcparams->sp))
            {
                if (funcparams->thistp && isconst(basetype(funcparams->thistp)->btp))
                    if (!isconst(*tp))
                        errorsym(ERR_NON_CONST_FUNCTION_CALLED_FOR_CONST_OBJECT, funcparams->sp);
            }
        }
    }

    if (basetype(*tp)->type == bt_memberptr)
    {
        memberPtr = TRUE;
        *tp = basetype(*tp)->btp;
        
    }
    if (!isfunction(*tp))
    {
        // might be operator ()
        if (cparams.prm_cplusplus)
        {
            EXPRESSION *exp_arg = exp_cpp;
            TYPE *tp_arg = tp_cpp;
            if (insertOperatorParams(funcsp, &tp_cpp, &exp_cpp, funcparams, flags))
            {
                hasThisPtr = funcparams->thisptr != NULL;
                *tp = tp_cpp;
                *exp = exp_cpp;
            }
        }
    }
    {
        HASHTABLE *temp = basetype(*tp)->syms;
        if (temp)
        {
            HASHREC *hr = temp->table[0];
            
            if (funcparams->sp && !ismember(funcparams->sp) && !memberPtr)
            {
                if (operands)
                {
                    INITLIST *al = Alloc(sizeof(INITLIST));
                    al->exp = funcparams->thisptr;
                    al->tp = funcparams->thistp;
                    al->next = funcparams->arguments;
                    funcparams->arguments = al;
                }
                funcparams->thisptr = NULL;
            }
            else
            {
                if (!getStructureDeclaration() && !ispointer(tp_cpp) && !hasThisPtr)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, funcparams->sp);
                operands = FALSE;
            }
            lptr = &funcparams->arguments;
            if (funcparams->sp)
            {
                TYPE *tp1 = funcparams->sp->tp;
                if (ispointer(tp1))
                    tp1 = basetype(tp1)->btp;
                if (isfunction(tp1))
                {
                    hr = basetype(tp1)->syms->table[0];
                    if (hr)
                    {
                        if (((SYMBOL *)hr->p)->thisPtr)
                            hr = hr->next;
                        if (!hr->next ||  ((SYMBOL *)hr->next->p)->init)
                        {
                            TYPE *tp = ((SYMBOL *)hr->p)->tp;
                            if (isref(tp))
                            {
                                initializerRef = TRUE;
                                tp = basetype(tp)->btp;
                            }
                            if (isstructured(tp))
                            {
                                SYMBOL *sym = (basetype(tp)->sp);
                                if (sym->parentNameSpace && !strcmp(sym->parentNameSpace->name , "std")) 
                                {
                                    if (!strcmp(sym->name, "initializer_list") && sym->templateLevel)
                                    {
                                        initializerListTemplate = sym->tp;
                                        initializerListType = sym->templateParams->next->p->byClass.val;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (!(flags & _F_SIZEOF))
            {
                if (initializerListType)
                {
                    CreateInitializerList(initializerListTemplate, initializerListType, lptr, operands, initializerRef);
                    if (hr->next)
                        AdjustParams(funcparams->sp, hr->next, &(*lptr)->next, operands, TRUE);

                }
                else
                {
                    AdjustParams(funcparams->sp, hr, lptr, operands, TRUE);
                }
            }
            CheckCalledException(funcparams->sp, funcparams->thisptr);
            if (cparams.prm_cplusplus)
            {
                lptr = &funcparams->arguments;
                while (*lptr)
                {
                    if (isstructured((*lptr)->tp) && (*lptr)->exp)
                    {
                        EXPRESSION *exp = (*lptr)->exp;
                        if (exp-> type == en_not_lvalue)
                            exp = exp->left;
                        if (exp->type == en_auto)
                        {
                            if (!funcparams->callLab)
                                funcparams->callLab = -1;
                        }
                        else if (exp->type == en_thisref)
                        {
                            if (!funcparams->callLab)
                                funcparams->callLab = -1;
                        }
                    }
                    lptr = &(*lptr)->next;
                }
            }
            if (isfunction(*tp))
            {
                if (funcparams->thisptr && !memberPtr)
                {
                    SYMBOL *base = funcparams->sp->parentClass;
                    SYMBOL *derived = basetype(basetype(funcparams->thistp)->btp)->sp;
                    if (base != derived)
                    {
                        funcparams->thisptr = DerivedToBase(base->tp, derived->tp, funcparams->thisptr, _F_VALIDPOINTER);
                    }
                }
                if (isstructured(basetype(*tp)->btp) || basetype(basetype(*tp)->btp)->type == bt_memberptr)
                {
                    if (!(flags & _F_SIZEOF))
                    {
                        funcparams->returnEXP = anonymousVar(sc_auto, basetype(*tp)->btp);
                        funcparams->returnSP = funcparams->returnEXP->v.sp;
                    }
                }
                funcparams->ascall = TRUE;    
                funcparams->functp = *tp;
                {
                    TYPE **tp1;
                    *tp = basetype(*tp)->btp;
                    if (isref(*tp))
                    {
                        TYPE *tp1 = *tp;
                        *tp = Alloc(sizeof(TYPE));
                        **tp = *(tp1->btp);
                        UpdateRootTypes(*tp);
                        if ((*tp)->type == bt_rref)
                        {
                            (*tp)->rref = TRUE;
                            (*tp)->lref = FALSE;
                        }
                        else
                        {
                            (*tp)->lref = TRUE;
                            (*tp)->rref = FALSE;
                        }
                    }
                    else if (ispointer(*tp) && (*tp)->array)
                    {
                        (*tp)->lref = TRUE;
                        (*tp)->rref = FALSE;
                    }
                    tp1 = tp;
                    while (ispointer(*tp1) || basetype(*tp1)->type == bt_memberptr)
                        tp1 = &basetype(*tp1)->btp;
                    while ((*tp1)->btp)
                        tp1 = &(*tp1)->btp;
                    if (isstructured(*tp1))
                    {
                        *tp1 = (*tp1)->sp->tp;
                    }
                }
                if (!(flags & _F_SIZEOF))
                {
                    checkArgs(funcparams, funcsp);
                }
                if (funcparams->returnSP)
                {
                    SYMBOL *sp = basetype(funcparams->returnSP->tp)->sp;
                    if (sp->templateLevel && sp->templateParams && !sp->instantiated)
                    {
                        if (!allTemplateArgsSpecified(sp, sp->templateParams))
                            sp = GetClassTemplate(sp, sp->templateParams->next, FALSE);
                        if (sp)
                            funcparams->returnSP->tp = TemplateClassInstantiate(sp, sp->templateParams, FALSE, sc_global)->tp;
                    }
                }
                if (!funcparams->novtab && funcparams->sp && funcparams->sp->storage_class == sc_virtual)
                {
                    exp_in = funcparams->thisptr;
                    deref(&stdpointer, &exp_in);
                    exp_in = exprNode(en_add, exp_in, intNode(en_c_i, funcparams->sp->vtaboffset));
                    deref(&stdpointer, &exp_in);
                    funcparams->fcall = exp_in;
                }
                else {
                    exp_in = varNode(en_func, NULL);
                    exp_in->v.func = funcparams;
                    if (exp_in && cparams.prm_cplusplus && funcparams->returnEXP)
                    {
                        if (!basetype(funcparams->returnSP->tp)->sp->trivialCons)
                        {
                            EXPRESSION *expx;
                            exp_in = exprNode(en_thisref, exp_in, NULL);
                            exp_in->v.t.thisptr = funcparams->returnEXP;
                            exp_in->v.t.tp = funcparams->returnSP->tp;
                            
                            expx = funcparams->returnEXP;
                            callDestructor(basetype(funcparams->returnSP->tp)->sp, NULL, &expx, NULL, TRUE, FALSE, TRUE);
                            initInsert(&funcparams->returnSP->dest, funcparams->returnSP->tp, expx, 0, TRUE);
                        }
                    }
                    if (exp_in)
                    {
                        TYPE *tp = basetype(funcparams->sp->tp)->btp;
                        if ((flags & _F_AMPERSAND) && isarithmetic(tp))
                        {
                            EXPRESSION *rv = anonymousVar(sc_auto, tp);
                            deref(tp, &rv);
                            exp_in = exprNode(en_void, exprNode(en_assign, rv, exp_in), rv) ;
                            errortype(ERR_CREATE_TEMPORARY, tp, tp);
                        }
                    }
                    if (exp_in)
                        *exp = exp_in;
                }
                if (funcparams->sp && isfunction(funcparams->sp->tp) && isref(basetype(funcparams->sp->tp)->btp))
                {
                    TYPE **tp1;
                    TYPE *tp2 = Alloc(sizeof(TYPE));
                    deref(basetype(basetype(funcparams->sp->tp)->btp)->btp, exp);
                    tp1 = &basetype(funcparams->sp->tp)->btp;
                    *tp2 = *basetype(*tp1)->btp;
                    UpdateRootTypes(tp2);
                    *tp = tp2;
                    if (basetype(*tp1)->type == bt_rref)
                    {
                        (*tp)->rref = TRUE;
                        (*tp)->lref = FALSE;
                    }
                    else
                    {
                        (*tp)->lref = TRUE;
                        (*tp)->rref = FALSE;
                    }
                    while (isref(*tp))
                        *tp = basetype(*tp)->btp;
                }
            }
            else if (templateNestingCount && !instantiatingTemplate && (*tp)->type == bt_aggregate)
            {
                *exp = exprNode(en_funcret, *exp, NULL);
                *tp = &stdvoid;
            }
            else
            {
                if (!templateNestingCount)
                    error(ERR_CALL_OF_NONFUNCTION);
                *tp = &stdvoid;
            }
        }
        else
        {
            *tp = &stdint;
            if (!templateNestingCount)
                error(ERR_CALL_OF_NONFUNCTION);
        }
        
    }
    return lex;
}
static LEXEME *expression_alloca(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        lex = expression_comma(lex, funcsp, NULL, tp, exp, NULL, flags);
        if (*tp)
        {
            if (!isint(*tp))
                error(ERR_NEED_INTEGER_EXPRESSION);
            optimize_for_constants(exp);
        
            funcsp->allocaUsed = TRUE;
            *exp = exprNode(en_alloca, *exp, 0);
            needkw(&lex, closepa);
            *tp = &stdpointer;
        }
        else
        {
            error(ERR_EXPRESSION_SYNTAX);
            *tp = NULL;
        }
    }
    else
    {
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
        *tp = NULL;
    }
    return lex;
}
static LEXEME *expression_msilfunc(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    enum e_kw kw = lex->kw->key;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        FUNCTIONCALL funcparams;
        memset(&funcparams, 0, sizeof(funcparams));
        lex = getArgs(lex, funcsp, &funcparams, closepa, TRUE, flags);
        int n = 0;
        INITLIST *args = funcparams.arguments;
        for (n = 0; args; args = args->next) n++;
        if (n > 3)
            errorstr(ERR_PARAMETER_LIST_TOO_LONG, "__cpblk/__initblk");
        else if (n < 3)
            errorstr(ERR_PARAMETER_LIST_TOO_SHORT, "__cpblk/__initblk");
        switch (kw)
        {
            case kw__cpblk:
                if (!ispointer(funcparams.arguments->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__dest", "__cpblk");
                }
                else if (!ispointer(funcparams.arguments->next->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__src", "__cpblk");
                }
                else if (!isint(funcparams.arguments->next->next->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__size", "__cpblk");
                }
                else
                {
                    *exp = exprNode(en_void, funcparams.arguments->exp, funcparams.arguments->next->exp);
                    *exp = exprNode(en__cpblk, *exp, funcparams.arguments->next->next->exp);
                }
                break;
            case kw__initblk:
                if (!ispointer(funcparams.arguments->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__dest", "__initblk");
                }
                else if (!isint(funcparams.arguments->next->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__value", "__initblk");
                }
                else if (!isint(funcparams.arguments->next->next->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__size", "__initblk");
                }
                else
                {
                    *exp = exprNode(en_void, funcparams.arguments->exp, funcparams.arguments->next->exp);
                    *exp = exprNode(en__initblk, *exp, funcparams.arguments->next->next->exp);
                }
                break;
            default:
                *exp = intNode(en_c_i, 0);
                break;
        }
        *tp = &stdpointer;
    }
    else
    {
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
        *tp = NULL;
    }
    return lex;
}
static LEXEME *expression_string(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN wide;
    int elems = 0;
    STRING *data;
    int i;
    (void)funcsp;
    
    IncGlobalFlag();
    lex = concatStringsInternal(lex, &data, &elems);
    *exp = stringlit(data);
    DecGlobalFlag();
    if (data->suffix)
    {
        enum e_bt tpb = (*tp)->type;
        SYMBOL *sym;
        char name[512];
        switch(data->strtype)
        {
            default:
            case l_astr:
                tpb = stdcharptr.btp->type;
                break;
            case l_wstr:
                tpb = stdwcharptr.btp->type;
                break;
            case l_ustr:
                tpb = stdchar16tptr.btp->type;
                break;
            case l_Ustr:
                tpb = stdchar32tptr.btp->type;
                break;
        }
        my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], data->suffix);
        sym = LookupSym(name);
        if (sym)
        {
            HASHREC *hr = sym->tp->syms->table[0], *hr1;
            SYMBOL *sym1, *sym2;
            while (hr)
            {
                sym1 = (SYMBOL *)hr->p;
                hr1 = sym1->tp->syms->table[0];
                sym2 = (SYMBOL *)hr1->p;
                if (hr1->next && ispointer(sym2->tp))
                    if (isconst(sym2->tp->btp) && basetype(sym2->tp->btp)->type == tpb)
                        break;                
                hr = hr->next;
            }
            if (hr)
            {
                FUNCTIONCALL *f = Alloc(sizeof(FUNCTIONCALL));
                f->sp = sym1;
                f->functp = sym1->tp;
                f->fcall = varNode(en_pc, sym1);
                f->arguments = Alloc(sizeof(INITLIST));
                f->arguments->tp = sym2->tp;
                f->arguments->exp = *exp;
                f->arguments->next = Alloc(sizeof(INITLIST));
                f->arguments->next->tp = &stdunsigned;
                f->arguments->next->exp = intNode(en_c_i, elems);
                *exp = intNode(en_func, 0);
                (*exp)->v.func = f;
                *tp = sym1->tp;
                expression_arguments(NULL, funcsp, tp, exp, FALSE);
                return lex;
            }
        }
        errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, data->suffix);
    }
    *tp = Alloc(sizeof(TYPE));
    (*tp)->type = bt_pointer;
    (*tp)->array = TRUE;
    (*tp)->rootType = (*tp);
    (*tp)->esize = intNode( en_c_i, elems +1);
    switch(data->strtype)
    {
        default:
        case l_astr:
            (*tp)->btp = stdcharptr.btp;
            break;
        case l_wstr:
            (*tp)->btp = stdwcharptr.btp;
            break;
        case l_ustr:
            (*tp)->btp = stdchar16tptr.btp;
            break;
        case l_Ustr:
            (*tp)->btp = stdchar32tptr.btp;
            break;
    }
    (*tp)->rootType = (*tp);
    (*tp)->size = (elems + 1) * (*tp)->btp->size;
    return lex;	
}
static LEXEME *expression_generic(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    lex = getsym();
    if (!needkw(&lex, openpa))
    {
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
        *exp = intNode(en_c_i, 0);
        *tp = &stdint;
    }
    else
    {
        TYPE *selectType = NULL;
        EXPRESSION *throwawayExpression = NULL;
        lex = expression_assign(lex, funcsp, NULL, &selectType, &throwawayExpression, NULL, flags);
        if (MATCHKW(lex, comma))
        {
            BOOLEAN dflt = FALSE;
            struct genericHold
            {
                struct genericHold *next;
                TYPE *selector;
                TYPE *type;
                EXPRESSION *exp;
            };
            struct genericHold *list = NULL, **pos = &list;
            struct genericHold *selectedGeneric = NULL;
            while (MATCHKW(lex, comma))
            {
                struct genericHold *next = Alloc(sizeof(struct genericHold)), *scan;
                lex = getsym();
                if (MATCHKW(lex, kw_default))
                {
                    lex = getsym();
                    if (dflt)
                    {
                        error(ERR_GENERIC_TOO_MANY_DEFAULTS);
                    }
                    dflt = TRUE;
                    next->selector = NULL;
                }
                else
                {
                    lex = get_type_id(lex, &next->selector, funcsp, sc_cast, FALSE, TRUE);
                    if (!next->selector)
                    {
                        error(ERR_GENERIC_MISSING_TYPE);
                        break;
                    }
                }
                if (MATCHKW(lex, colon))
                {
                    lex = getsym();
                    lex = expression_assign(lex, funcsp, NULL, &next->type, &next->exp, NULL, flags);
                    if (!next->type)
                    {
                        error(ERR_GENERIC_MISSING_EXPRESSION);
                        break;
                    }
                    scan = list;
                    while (scan)
                    {
                        if (scan->selector && next->selector && comparetypes(next->selector, scan->selector, TRUE))
                        {
                            if (isconst(next->selector) == isconst(scan->selector) && 
                                isvolatile(next->selector) == isvolatile(scan->selector) &&
                                isrestrict(next->selector) == isrestrict(scan->selector) && 
                                next->selector->alignment == scan->selector->alignment)
                            {
                                error(ERR_DUPLICATE_TYPE_IN_GENERIC);
                            }
                        }
                        scan = scan->next;
                    }
                    if (!selectedGeneric && !next->selector)
                        selectedGeneric = next;
                    else
                    {
                        if (selectType && next->selector && comparetypes(next->selector, selectType, TRUE))
                        {
                            if (isconst(next->selector) == isconst(selectType) && 
                                isvolatile(next->selector) == isvolatile(selectType) &&
                                isrestrict(next->selector) == isrestrict(selectType) && 
                                next->selector->alignment == selectType->alignment)
                            {
                                if (selectedGeneric && selectedGeneric->selector)
                                    error(ERR_DUPLICATE_TYPE_IN_GENERIC);
                                selectedGeneric = next;
                            }
                        }
                    }
                    *pos = next;
                    pos = &(*pos)->next;
                }
                else
                {
                    error(ERR_GENERIC_MISSING_EXPRESSION);
                    break;
                }
            }            
            if (selectedGeneric)
            {
                *tp = selectedGeneric->type;
                *exp = selectedGeneric->exp;
            }
            else
            {
                error(ERR_GENERIC_NOTHING_SELECTED);
                *tp = &stdint;
                *exp = intNode(en_c_i, 0);
            }
            if (!needkw(&lex, closepa))
            {
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
        }
        else
        {
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
    }
    return lex;
}
static BOOLEAN getSuffixedChar(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    char name[512];
    enum e_bt tpb = (*tp)->type;
    SYMBOL *sym;
    my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], lex->suffix);
    sym = LookupSym(name);
    if (sym)
    {
        HASHREC *hr = sym->tp->syms->table[0], *hr1;
        SYMBOL *sym1, *sym2;
        while (hr)
        {
            sym1 = (SYMBOL *)hr->p;
            hr1 = sym1->tp->syms->table[0];
            sym2 = (SYMBOL *)hr1->p;
            if (!hr1->next && sym2->tp->type == tpb)
                break;                
            hr = hr->next;
        }
        if (hr)
        {
            FUNCTIONCALL *f = Alloc(sizeof(FUNCTIONCALL));
            f->sp = sym1;
            f->functp = sym1->tp;
            f->fcall = varNode(en_pc, sym1);
            f->arguments = Alloc(sizeof(INITLIST));
            f->arguments->tp = *tp;
            f->arguments->exp = *exp;
            *exp = intNode(en_func, 0);
            (*exp)->v.func = f;
            *tp = sym1->tp;
            expression_arguments(NULL, funcsp, tp, exp, FALSE);
            return TRUE;
        }
    }
    errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, lex->suffix);
    return FALSE;
}
static BOOLEAN getSuffixedNumber(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    char name[512];
    enum e_bt tpb;
    SYMBOL *sym;
    if (lex->type == l_ull)
        tpb  = bt_unsigned_long_long;
    else
        tpb = bt_long_double;
    my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], lex->suffix);
    sym = LookupSym(name);
    if (sym)
    {
        // look for parameter of type unsigned long long or long double
        HASHREC *hr = sym->tp->syms->table[0], *hr1;
        SYMBOL *sym1, *sym2;
        while (hr)
        {
            sym1 = (SYMBOL *)hr->p;
            hr1 = sym1->tp->syms->table[0];
            sym2 = (SYMBOL *)hr1->p;
            if (!hr1->next && sym2->tp->type == tpb)
                break;                
            hr = hr->next;
        }
        if (hr)
        {
            FUNCTIONCALL *f = Alloc(sizeof(FUNCTIONCALL));
            f->sp = sym1;
            f->functp = sym1->tp;
            f->fcall = varNode(en_pc, sym1);
            f->arguments = Alloc(sizeof(INITLIST));
            if (lex->type == l_ull)
            {
                f->arguments->tp = &stdunsignedlonglong;
                f->arguments->exp = intNode(en_c_ull, lex->value.i);
            }
            else
            {
                f->arguments->tp = &stdlongdouble;
                f->arguments->exp = intNode(en_c_ld, 0);
                f->arguments->exp->v.f = lex->value.f;
            }            
            *exp = intNode(en_func, 0);
            (*exp)->v.func = f;
            *tp = sym1->tp;
            expression_arguments(NULL, funcsp, tp, exp, FALSE);
            return TRUE;
        }
        else
        {
            // not found, look for parameter of type const char *
            hr = sym->tp->syms->table[0];
            while (hr)
            {
                TYPE *tpx;
                sym1 = (SYMBOL *)hr->p;
                hr1 = sym1->tp->syms->table[0];
                sym2 = (SYMBOL *)hr1->p;
                tpx = sym2->tp;
                if (!hr1->next && ispointer(tpx))
                {
                    tpx = basetype(tpx)->btp;
                    if (isconst(tpx) && basetype(tpx)->type == bt_char)
                        break;
                }
                hr = hr->next;
            }
            if (hr)
            {
                FUNCTIONCALL *f = Alloc(sizeof(FUNCTIONCALL));
                STRING *data;
                int i;
                f->sp = sym1;
                f->functp = sym1->tp;
                f->fcall = varNode(en_pc, sym1);
                f->arguments = Alloc(sizeof(INITLIST));
                f->arguments->tp = &stdcharptr;
                f->arguments->tp->size = (strlen(lex->litaslit) + 1) * f->arguments->tp->btp->size;
                IncGlobalFlag();
                data = (STRING *)Alloc(sizeof(STRING));
                data->strtype = l_astr;
                data->size = 1;
                data->pointers = Alloc(sizeof(SLCHAR *));
                data->pointers[0] = Alloc(sizeof(SLCHAR));
                data->pointers[0]->count = strlen(lex->litaslit);
                data->pointers[0]->str = Alloc(sizeof(LCHAR) * (data->pointers[0]->count + 1));
                for (i=0; i < data->pointers[0]->count; i++)
                    data->pointers[0]->str[i] = lex->litaslit[i];
                f->arguments->exp = stringlit(data);
                DecGlobalFlag();
                *exp = intNode(en_func, 0);
                (*exp)->v.func = f;
                *tp = sym1->tp;
                expression_arguments(NULL, funcsp, tp, exp, FALSE);
                return TRUE;
            }
        }
    }
    errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, lex->suffix);
    return FALSE;
}
static LEXEME *expression_atomic_func(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    enum e_kw kw = KW(lex);
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        if (kw == kw_atomic_kill_dependency)
        {
            lex = expression_assign(lex, funcsp, NULL, tp, exp, NULL, flags);
            if (!*tp)
                error(ERR_EXPRESSION_SYNTAX);
            needkw(&lex, closepa);
        }
        else if (kw == kw_atomic_var_init)
        {
            lex = expression_assign(lex, funcsp, NULL, tp, exp, NULL, flags);
            if (!*tp)
                error(ERR_EXPRESSION_SYNTAX);
            if (MATCHKW(lex, comma)) // atomic_init
            {
                TYPE *tp1;
                EXPRESSION *exp1;
                lex = getsym();
                lex = expression_assign(lex, funcsp, NULL, &tp1, &exp1, NULL, flags);
                if (*tp && tp1)
                {
                    ATOMICDATA *d;
                    d = (ATOMICDATA *)Alloc(sizeof(ATOMICDATA));
                    if (!ispointer(tp1))
                    {
                        error(ERR_DEREF);
                        d->tp = *tp = &stdint;
                    }
                    else
                    {
                        TYPE *tp2 = *tp;
                        d->tp = *tp = basetype(tp1)->btp;
                        tp1 = tp2;
                    }
                    if (!comparetypes(*tp, tp1, FALSE))
                    {
                        error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                    }
                    d->address = exp1;
                    d->value = *exp;        
                    d->atomicOp = ao_init;
                    *exp = exprNode(en_atomic, NULL , NULL);
                    (*exp)->v.ad = d;
                }
                else
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
            }
            else // ATOMIC_VAR_INIT
            {
                if (*exp)
                    (*exp)->atomicinit = TRUE; 
            }
            needkw(&lex, closepa);
        }
        else
        {
            TYPE *tpf = NULL, *tpf1;
            ATOMICDATA *d;
            d = (ATOMICDATA *)Alloc(sizeof(ATOMICDATA));
            switch (kw)
            {
                case kw_atomic_flag_test_set:
                    lex = expression_assign(lex, funcsp, NULL, &tpf, &d->flg, NULL, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                        }
                        deref(&stdint, &d->flg);
                        d->tp = *tp = &stdint;
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->memoryOrder1, NULL, flags);
                    }
                    else 
                    {
                        tpf = &stdint;
                        d->memoryOrder1 =intNode(en_c_i, mo_seq_cst);
                    }
                    d->atomicOp = ao_flag_set_test;
                    break;
                case kw_atomic_flag_clear:
                    lex = expression_assign(lex, funcsp, NULL, &tpf, &d->flg, NULL, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                        }
                        deref(&stdint, &d->flg);
                        d->tp = *tp = &stdint;
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->memoryOrder1, NULL, flags);
                    }
                    else 
                    {
                        tpf = &stdint;
                        d->memoryOrder1 =intNode(en_c_i, mo_seq_cst);
                    }
                    d->atomicOp = ao_flag_clear;
                    *tp = &stdvoid;
                    break;
                case kw_atomic_fence:
                    lex = expression_assign(lex, funcsp, NULL, &tpf, &d->memoryOrder1, NULL, flags);
                    d->atomicOp = ao_fence;
                    *tp = &stdvoid;
                    break;
                case kw_atomic_load:
                    lex = expression_assign(lex, funcsp, NULL, &tpf, &d->address, NULL, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                            if (isstructured(*tp))
                                error(ERR_ILL_STRUCTURE_OPERATION);
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL,&tpf, &d->memoryOrder1, NULL, flags);
                    }
                    else 
                    {
                        tpf = &stdint;
                        d->memoryOrder1 =intNode(en_c_i, mo_seq_cst);
                    }
                    d->atomicOp = ao_load;
                    break;
                case kw_atomic_store:
                    lex = expression_assign(lex, funcsp, NULL, &tpf, &d->address, NULL, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                            if (isstructured(*tp))
                                error(ERR_ILL_STRUCTURE_OPERATION);
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->value, NULL, flags);
                        if (!comparetypes(tpf, *tp, FALSE))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else 
                    {
                        *tp = &stdint;
                        d->value = intNode(en_c_i, 0);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->memoryOrder1, NULL, flags);
                    }
                    else 
                    {
                        tpf = &stdint;
                        d->memoryOrder1 =intNode(en_c_i, mo_seq_cst);
                    }
                    d->atomicOp = ao_store;
                    break;
                case kw_atomic_modify:
                    lex = expression_assign(lex, funcsp, NULL, &tpf, &d->address, NULL, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                            if (isstructured(*tp))
                                error(ERR_ILL_STRUCTURE_OPERATION);
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        switch(KW(lex))
                        {
                            default:
                                d->third = intNode(en_c_i, asplus);
                                break;
                            case asplus:
                            case asminus:
                            case asor:
                            case asand:
                            case asxor:
                                d->third = intNode(en_c_i, KW(lex));
                                break;
                        }
                        lex = getsym();
                    }
    
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->value, NULL, flags);
                        if (!comparetypes(tpf, *tp, FALSE))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else 
                    {
                        *tp = &stdint;
                        d->value = intNode(en_c_i, 0);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->memoryOrder1, NULL, flags);
                    }
                    else 
                    {
                        tpf = &stdint;
                        d->memoryOrder1 =intNode(en_c_i, mo_seq_cst);
                    }
                    d->atomicOp = ao_modify;
                    break;
                case kw_atomic_cmpswp:
                    lex = expression_assign(lex, funcsp, NULL, &tpf, &d->address, NULL, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                            if (isstructured(*tp))
                                error(ERR_ILL_STRUCTURE_OPERATION);
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf1, &d->third, NULL, flags);
                        if (!comparetypes(tpf, tpf1, FALSE))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else 
                    {
                        *tp = &stdint;
                        d->third = intNode(en_c_i, 0);
                    }
    
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->value, NULL, flags);
                        if (!comparetypes(tpf, *tp, FALSE))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else 
                    {
                        *tp = &stdint;
                        d->value = intNode(en_c_i, 0);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf, &d->memoryOrder1, NULL, flags);
                    }
                    else 
                    {
                        tpf = &stdint;
                        d->memoryOrder1 =intNode(en_c_i, mo_seq_cst);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, NULL, &tpf1, &d->memoryOrder2, NULL, flags);
                    }
                    else 
                    {
                        tpf1 = &stdint;
                        d->memoryOrder2 =intNode(en_c_i, mo_seq_cst);
                    }
                    d->atomicOp = ao_cmpswp;
                    *tp = &stdint;
                    break;
                default:
                    break;
            }
            if (tpf && !isint(tpf))
            {
                error(ERR_NEED_INTEGER_TYPE);
                d->memoryOrder1 = *exp = intNode(en_c_i, mo_relaxed);
            }
            if (!d->memoryOrder2)
                d->memoryOrder2 = d->memoryOrder1;
            if (!needkw(&lex, closepa))
            {
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
            *exp = exprNode(en_atomic, NULL , NULL);
            (*exp)->v.ad = d;
        }
    }
    else
    {
        *tp = &stdint;
        *exp = intNode(en_c_i, mo_relaxed);
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
        
    }
    return lex;
}
static LEXEME *expression_primary(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    switch(lex ? lex->type : l_none)
    {
        SYMBOL *sym;
        case l_id:
               lex = variableName(lex, funcsp, atp, tp, exp, ismutable, flags);
            break;
        case l_kw:
            switch(KW(lex))
            {
                SYMBOL *sym;
                case openbr:
                    if (cparams.prm_cplusplus)
                        lex = expression_lambda(lex, funcsp, atp, tp, exp, flags);
                    break;
                case classsel:
                case kw_operator:
                    lex = variableName(lex, funcsp, atp, tp, exp, ismutable, flags);
                    break;
                case kw_nullptr:
                    *exp = intNode(en_nullptr, 0);
                    *tp = &stdnullpointer;
                    lex = getsym();
                    break;
                case kw_this:
                    if (lambdas)
                    {
                        lambda_capture(NULL, cmThis, FALSE);
                        if (lambdas->captureThis)
                        {
                            SYMBOL *ths = search("$this", lambdas->cls->tp->syms);
                            if (ths)
                            {
                                TYPE *t1 = Alloc(sizeof(TYPE));
                                t1->type = bt_pointer;
                                t1->size = getSize(bt_pointer);
                                t1->btp = basetype(lambdas->lthis->tp)->btp;
                                t1->rootType = t1;
                                *tp = t1;
                                *exp = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p); // this ptr
                                deref(&stdpointer, exp);
                                *exp = exprNode(en_add, *exp, intNode(en_c_i, ths->offset));
                                deref(&stdpointer, exp);
                            }
                            else
                            {
                                diag("expression_primary: missing lambda this");
                            }
                        }
                        else
                        {
                            *exp = intNode(en_c_i, 0);
                            *tp = &stdint;
                        }
                    }
                    else if (getStructureDeclaration() && funcsp->parentClass)
                    {
                        getThisType(funcsp, tp);
                        *exp = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p); // this ptr
                        deref(&stdpointer, exp);
                    }
                    else
                    {
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        error(ERR_THIS_MEMBER_FUNC);
                    }
                    lex = getsym();
                    break;
                case kw___I:
                    *exp = intNode(en_c_i,0);
                    (*exp)->type = en_c_fi;
                    LongLongToFPF(&(*exp)->v.f, 1);
                    *tp = &stdfloatimaginary;
                    (*exp)->pragmas = stdpragmas;
                    lex = getsym();
                    break;
                case kw_true:
                    lex = getsym();
                    *exp = intNode(en_c_i, 1);
                    (*exp)->type = en_c_bool;
                    *tp = &stdbool;
                    break;
                case kw_false:
                    lex = getsym();
                    *exp = intNode(en_c_i, 0);
                    (*exp)->type = en_c_bool;
                    *tp = &stdbool;
                    return lex;
                case kw_alloca:
                    lex = expression_alloca(lex, funcsp, tp, exp, flags);
                    return lex;
                case kw__initblk:
                case kw__cpblk:
                    lex = expression_msilfunc(lex, funcsp, tp, exp, flags);
                    break;
                case openpa:
                    lex = getsym();
                    lex = expression_comma(lex, funcsp, NULL, tp, exp, ismutable, flags & ~(_F_INTEMPLATEPARAMS | _F_SELECTOR));
                    if (!*tp)
                        error(ERR_EXPRESSION_SYNTAX);
                    needkw(&lex, closepa);
                    break;
                case kw___func__:
                    *tp = &std__func__;
                    if (!funcsp->__func__label)
                    {
                        LCHAR buf[256], *q = buf;
                        char *p = funcsp->name;
                        STRING *string;
                        IncGlobalFlag();
                        string = (STRING *)Alloc(sizeof(STRING));
                        string->strtype = l_astr;
                        string->size = 1;
                        string->pointers = Alloc(sizeof(SLCHAR * ));
                        while (*p)
                            *q++ = *p++;
                        *q = 0;
                        string->pointers[0] = Alloc(sizeof(SLCHAR));
                        string->pointers[0]->str = wlitlate(buf);
                        string->pointers[0]->count = q - buf;
                        *exp = stringlit(string);
                        funcsp->__func__label = string->label;
                        DecGlobalFlag();
                    }
                    else
                        *exp = intNode(en_labcon, funcsp->__func__label);
                    lex = getsym();
                    break;
                case kw__uuidof:
                    lex = getsym();
                    needkw(&lex, openpa);
                    if (ISID(lex))
                    {
                        sym = tsearch(lex->value.s.a);
                        lex = getsym();
                    }
                    else
                    {
                        TYPE *tp1;
                        EXPRESSION *exp1;
                        lex = expression_no_comma(lex, funcsp, NULL, &tp1, &exp1, NULL, 0);
                        if (tp1 && isstructured(tp1))
                        {
                            sym = basetype(tp1)->sp;
                        }
                        else
                        {
                            sym = NULL;
                        }
                    }
                    needkw(&lex, closepa);
                    *exp = GetUUIDData(sym);
                    *tp = &stdpointer;
                    break;
                case kw___va_typeof__:
                    lex = getsym();
                    if (MATCHKW(lex, openpa))
                    {
                        lex = getsym();
                        if (startOfType(lex, FALSE))
                        {
                            SYMBOL *sp;
                            lex = get_type_id(lex, tp, funcsp, sc_cast, FALSE, TRUE);
                            (*tp)->used = TRUE;
                            needkw(&lex, closepa);
                            // don't enter in table, this is purely so we can cache the type info
                            sp = makeID( sc_auto, *tp, NULL, AnonymousName());
                            sp->va_typeof = TRUE;
                            *exp = varNode(en_auto, sp);
                            break;
                        }
                    }
                    error(ERR_TYPE_NAME_EXPECTED);
                    *exp = intNode(en_c_i, 0);
                    *tp = &stdint;
                    break;
                case kw_D0:
                case kw_D1:
                case kw_D2:
                case kw_D3:
                case kw_D4:
                case kw_D5:
                case kw_D6:
                case kw_D7:
                case kw_A0:
                case kw_A1:
                case kw_A2:
                case kw_A3:
                case kw_A4:
                case kw_A5:
                case kw_A6:
                case kw_A7:
                    *exp = intNode(en_c_i, 0);
                    *tp = &stdint;
                    lex = getsym();
                    break;
                case kw__NAN:
                    *exp = intNode(en_c_i,0);
                    (*exp)->type = en_c_f;
                    (*exp)->v.f.type = IFPF_IS_NAN;
                    lex = getsym();
                    *tp = &stdfloat;
                    break;
                case kw__INF:
                    *exp = intNode(en_c_i,0);
                    (*exp)->type = en_c_f;
                    (*exp)->v.f.type = IFPF_IS_INFINITY;
                    lex = getsym();
                    *tp = &stdfloat;
                    break;
                case kw_generic:
                    lex = expression_generic(lex, funcsp, tp, exp, flags);
                    break;
                case kw_atomic_flag_test_set:
                case kw_atomic_flag_clear:
                case kw_atomic_fence:
                case kw_atomic_load:
                case kw_atomic_store:
                case kw_atomic_modify:
                case kw_atomic_cmpswp:
                case kw_atomic_kill_dependency:
                case kw_atomic_var_init:
                    lex = expression_atomic_func(lex, funcsp, tp, exp, flags);
                    break;
                case kw_typename:
					lex = getsym();
                    *tp = NULL;
                    lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
                    break;
                default:
/*					errorstr(ERR_UNEXPECTED_KEYWORD, lex->kw->name); */
                    *tp = NULL;
                    *exp = intNode(en_c_i, 0);
                    lex = getsym();
                    break;
            }
            break;
        case l_i:
            *exp = intNode(en_c_i, lex->value.i);
            *tp = &stdint;
            lex = getsym();
            break;
        case l_ui:
            *exp = intNode(en_c_ui, lex->value.i);
            (*exp)->type = en_c_ui;
            *tp = &stdunsigned;
            lex = getsym();
            break;
        case l_l:
            *exp = intNode(en_c_l, lex->value.i);
            (*exp)->type = en_c_l;
            *tp = &stdlong;
            lex = getsym();
            break;
        case l_ul:
            *exp = intNode(en_c_ul, lex->value.i);
            (*exp)->type = en_c_ul;
            *tp = &stdunsignedlong;
            lex = getsym();
            break;
        case l_ll:
            *exp = intNode(en_c_ll, lex->value.i);
            (*exp)->type = en_c_ll;
            *tp = &stdlonglong;
            lex = getsym();
            break;
        case l_ull:
            if (!lex->suffix || !getSuffixedNumber(lex, funcsp, tp,exp))
            {
                *exp = intNode(en_c_ull, lex->value.i);
                (*exp)->type = en_c_ull;
                *tp = &stdunsignedlonglong;
            }
            lex = getsym();
            break;
        case l_f:
            *exp = intNode(en_c_f, 0);
            (*exp)->v.f = lex->value.f;
            (*exp)->pragmas = stdpragmas;
            *tp = &stdfloat;
            lex = getsym();
            break;
        case l_d:
            *exp = intNode(en_c_d, 0);
            (*exp)->v.f = lex->value.f;
            (*exp)->pragmas = stdpragmas;
            *tp = &stddouble;
            lex = getsym();
            break;
        case l_ld:
            if (!lex->suffix || !getSuffixedNumber(lex, funcsp, tp,exp))
            {
                *exp = intNode(en_c_ld, 0);
                (*exp)->v.f = lex->value.f;
                (*exp)->pragmas = stdpragmas;
                *tp = &stdlongdouble;
            }
            lex = getsym();
            break;
        case l_astr:
        case l_wstr:
        case l_ustr:
        case l_Ustr:
            lex = expression_string(lex, funcsp, tp, exp);
            break;
        case l_wchr:
            *exp = intNode(en_c_wc, lex->value.i);
            (*exp)->type = en_c_wc;
            *tp = &stdwidechar;
            if (lex->suffix)
                getSuffixedChar(lex, funcsp, tp,exp);
            lex = getsym();
            break;
        case l_achr:
            *exp = intNode(en_c_c, lex->value.i);
            (*exp)->type = en_c_c;
            *tp = &stdchar;
            if (lex->suffix)
                getSuffixedChar(lex, funcsp, tp,exp);
            lex = getsym();
            break;
        case l_uchr:
            *exp = intNode(en_c_u16, lex->value.i);
            (*exp)->type = en_c_u16;
            *tp = stdchar16tptr.btp;
            if (lex->suffix)
                getSuffixedChar(lex, funcsp, tp,exp);
            lex = getsym();
            break;
        case l_Uchr:
            *exp = intNode(en_c_u32, lex->value.i);
            (*exp)->type = en_c_u32;
            *tp = stdchar32tptr.btp;
            if (lex->suffix)
                getSuffixedChar(lex, funcsp, tp,exp);
            lex = getsym();
            break;
        default:
            *tp = NULL;
            *exp = intNode(en_c_i, 0);
            break;
    }
    return lex;
}
static EXPRESSION *nodeSizeof(TYPE *tp, EXPRESSION *exp)
{
    tp = PerformDeferredInitialization(basetype(tp), NULL);
    if (isstructured(tp))
        tp = basetype(tp)->sp->tp;
    if (isref(tp))
        tp = basetype(tp)->btp;
    if (exp)
    {
        while (castvalue(exp))
            exp = exp->left;
        if (lvalue(exp) && exp->left->type == en_bits)
            error(ERR_SIZEOF_BITFIELD);
    }
    if (cparams.prm_cplusplus && tp->type == bt_enum && !tp->fixed)
        error(ERR_SIZEOF_UNFIXED_ENUMERATION);
    if (isfunction(tp))
        error(ERR_SIZEOF_NO_FUNCTION);
    if (cparams.prm_cplusplus && tp->size == 0 && !templateNestingCount)
        errortype(ERR_UNSIZED_TYPE, tp, tp); /* second will be ignored in this case */
    /* this tosses exp...  sizeof expressions don't get evaluated at run time */
    /* unless they are size of a vla... */
    if (tp->vla)
    {
        exp = tp->esize;
        tp = basetype(tp)->btp;
        
        while (tp->type == bt_pointer)
        {
            exp = exprNode(en_mul, exp, tp->esize);
            tp = basetype(tp)->btp;
        }
        exp = exprNode(en_mul, exp, intNode(en_c_i, tp->size));
    }
    else 
    {
        exp = NULL;
        if (isstructured(tp))
        {
            if (basetype(tp)->syms)
            {
                HASHREC *hr = basetype(tp)->syms->table[0];
                SYMBOL *cache = NULL;
                TYPE *tpx;
                while (hr)
                {
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    if (ismemberdata(sp))
                        cache = sp;
                    hr = hr->next;
                }
                if (cache)
                {
                    tpx = basetype(cache->tp);
                    if (tpx->size == 0) /* if the last element of a structure is unsized */
                                        /* sizeof doesn't add the size of the padding element */
                        exp = intNode(en_c_i, cache->offset);
                }
            }
        }
    }
    if (!exp)
    {
        exp = intNode(en_c_i, tp->size); 
    }
    return exp;			  
}
static int widelen(LCHAR *s)
{
    int rv = 0;
    while(*s++) rv++;
    return rv;
}
static LEXEME *expression_sizeof(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    int paren = FALSE;
    TYPE *itp;
    *exp = NULL;
    lex = getsym();
    if (cparams.prm_cplusplus && MATCHKW(lex, ellipse))
    {
        lex = getsym();
        if (MATCHKW(lex, openpa))
        {
            paren = TRUE;
            lex = getsym();
        }
        if (!ISID(lex))
        {
            error(ERR_SIZEOFELLIPSE_NEEDS_TEMPLATE_PACK);
            errskim(&lex, skim_closepa);
        }
        else
        {
            EXPRESSION *exp1 = NULL;
            LEXEME *old = lex;
            lex = variableName(lex, funcsp, NULL, tp, &exp1, NULL, _F_PACKABLE | _F_SIZEOF);
            if (!exp1 || !exp1->v.sp->tp->templateParam->p->packed)
            {
//                error(ERR_SIZEOFELLIPSE_NEEDS_TEMPLATE_PACK);
                *tp = &stdunsigned;
                *exp = intNode(en_c_i, 1);
            }
            else if (templateNestingCount)
            {
                *exp = intNode(en_sizeofellipse, 0);
                (*exp)->v.templateParam = (*tp)->templateParam;
                *tp = &stdunsigned;
            }
            else if (!(*tp)->templateParam->p->packed)
            {
                *tp = &stdunsigned;
                *exp = intNode(en_c_i, 1);
            }
            else
            {
                int n = 0;
                TEMPLATEPARAMLIST *templateParam = (*tp)->templateParam->p->byPack.pack;
                while (templateParam)
                {
                    n++;
                    templateParam = templateParam->next;
                }
                *tp = &stdunsigned;
                *exp = intNode(en_c_i, n);
            }
        }
        if (paren)
            needkw(&lex, closepa);
    }
    else
    {
        if (MATCHKW(lex, openpa))
        {
            paren = TRUE;
            lex = getsym();
        }
        if (!paren || !startOfType(lex, FALSE))
        {
            if (paren)
            {
                lex = expression_comma(lex, funcsp, NULL, tp, exp, NULL, _F_SIZEOF);
                needkw(&lex, closepa);
            }
            else
            {
                lex = expression_unary(lex, funcsp, NULL, tp, exp, NULL, _F_SIZEOF);
            }
            if (!*tp)
            {
                *exp = intNode(en_c_i, 1);
                error(ERR_EXPRESSION_SYNTAX);
            }
            else
            {
                *exp = nodeSizeof(*tp, *exp);
            }
        }
        else
        {
            LEXEME *prev = lex;            
            lex = get_type_id(lex, tp, funcsp, sc_cast, cparams.prm_cplusplus, TRUE);
            if (cparams.prm_cplusplus && MATCHKW(lex, openpa))
            {
                lex = prevsym(prev);
                lex = expression_func_type_cast(lex, funcsp, tp, exp, 0);
            }
            if (paren)
                needkw(&lex, closepa);
            if (MATCHKW(lex, ellipse))
            {
                lex = getsym();
                if ((*tp)->type == bt_templateparam)
                {
                    if (!(*tp)->templateParam->p->packed)
                    {
                        error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                    }
                    else if (packIndex != -1)
                    {
                        TEMPLATEPARAMLIST *tpl = (*tp)->templateParam->p->byPack.pack;
                        int i;
                        for (i=0;tpl && i < packIndex;i++)
                            tpl = tpl->next;
                        if (tpl)
                            *tp = tpl->p->byClass.val;
                    }
                }
                else
                {
                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                }
            }
            if (!*tp)
            {
                *exp = intNode(en_c_i, 1);
            }
            else
            {
                checkauto(*tp);
                *exp = nodeSizeof(*tp, *exp);
            }
        }
            
        *tp = &stdint; /* other compilers use sizeof as a signed value so we do too... */
    }
    return lex;
}
static LEXEME *expression_alignof(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    int paren = FALSE;
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        lex = get_type_id(lex, tp, funcsp, sc_cast, FALSE, TRUE);
        needkw(&lex, closepa);
        if (MATCHKW(lex, ellipse))
        {
            lex = getsym();
            if ((*tp)->type == bt_templateparam)
            {
                if (!(*tp)->templateParam->p->packed)
                {
                    error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                }
                else if (packIndex != -1)
                {
                    TEMPLATEPARAMLIST *tpl = (*tp)->templateParam->p->byPack.pack;
                    int i;
                    for (i=0;tpl && i < packIndex;i++)
                        tpl = tpl->next;
                    if (tpl)
                        *tp = tpl->p->byClass.val;
                }
            }
            else
            {
                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
            }
        }
        if (!*tp)
        {
            *exp = intNode(en_c_i, 1);
        }
        else
        {
            TYPE *itp = *tp;
            checkauto(itp);
            if (isref(itp))
                itp = (basetype(itp)->btp);
            while (itp->array)
                itp = itp->btp;
            *exp = intNode(en_c_i, getAlign(sc_global, *tp));
        }
    }
       *tp = &stdint;
    return lex;
}
static LEXEME *expression_ampersand(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, int flags)
{
    lex = getsym();
    lex = expression_cast(lex, funcsp, atp, tp, exp, NULL, (flags) | _F_AMPERSAND);
    if (*tp)
    {
        TYPE *btp, *tp1;
        EXPRESSION *exp1 = *exp;
        while (exp1->type == en_void && exp1->right)
            exp1 = exp1->right;
        if (exp1->type == en_void)
            exp1 = exp1->left;
        btp = basetype(*tp);
        if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_any, and,
                               funcsp, tp, exp, NULL,NULL, NULL, flags))
        {
            return lex;
        }
        else if (isvoid(*tp) || ismsil(*tp))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (btp->hasbits)
            error(ERR_CANNOT_TAKE_ADDRESS_OF_BIT_FIELD);
        else if (btp->msil)
            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
        else if (inreg(*exp, TRUE))
                error(ERR_CANNOT_TAKE_ADDRESS_OF_REGISTER);
        else if ((!ispointer(btp) || !(btp)->array) && !isstructured(btp) &&
            !isfunction(btp) && btp->type != bt_aggregate && (exp1)->type != en_memberptr)
        {
            if ((exp1)->type != en_const && exp1->type != en_assign)
                if (!lvalue(exp1))
                    if (cparams.prm_ansi || !castvalue(exp1))
                        error(ERR_MUST_TAKE_ADDRESS_OF_MEMORY_LOCATION);
        }
        else
            switch ((exp1)->type)
            {
                case en_pc:
                case en_auto:
                case en_label:
                case en_global:
                case en_absolute:
                case en_threadlocal:
                {
                    SYMBOL *sp = (*exp)->v.sp;
                    if (sp->isConstructor || sp->isDestructor)
                        error(ERR_CANNOT_TAKE_ADDRESS_OF_CONSTRUCTOR_OR_DESTRUCTOR);
                    break;
                }
                default:
                    break;
            }
        if ((*exp)->type == en_const)
        {
            /* if a variable propagated silently to an inline constant
             * this will restore it as a static variable in the const section
             * when we take its address
             * but the constant will still be used inline when possible
             */
            SYMBOL *sp = (*exp)->v.sp;
            if (!sp->indecltable)
            { 
                if (!sp->label)
                {
                    TYPE *tp, *tpb, *tpn,**tpnp = &tpn;
                    BOOLEAN done = FALSE;
                    SYMBOL *spold = sp;
                    sp->label = nextLabel++;
                    IncGlobalFlag();
                    sp = clonesym(sp);
                    spold->indecltable = TRUE;
                    tp = sp->tp;
                    tpb = basetype(tp);
                    do
                    {
                        *tpnp = Alloc(sizeof(TYPE));
                        **tpnp = *tp;
                        UpdateRootTypes(*tpnp);
                        tpnp = &(*tpnp)->btp;
                        if (tp != tpb)
                            tp = tp->btp;
                        else
                            done = TRUE;
                    } while (!done);
                    sp->tp = tpn;
                    insertInitSym(sp);
                    DecGlobalFlag();
                }
                else
                {
                    IncGlobalFlag();
                    insertInitSym(sp);
                    DecGlobalFlag();
                }
                if (!sp->parent)
                    sp->parent = funcsp; // this promotion of a global to local is necessary to not make it linkable
//                if (sp->parent)
                    *exp = varNode(en_label, sp);
//                else
//                    *exp = varNode(en_global, sp);
            }
            else
            {
//                if (sp->parent)
                    *exp = varNode(en_label, sp);
//                else
//                    *exp = varNode(en_global, sp);
            }
            tp1 = Alloc(sizeof(TYPE));
            tp1->type = bt_pointer;
            tp1->size = getSize(bt_pointer);
            tp1->btp = *tp;
            tp1->rootType = tp1;
            *tp = tp1;
        }	
        else if (!isfunction(*tp) && (*tp)->type != bt_aggregate)
        {
            EXPRESSION *expasn = NULL;
            while(castvalue(exp1))
                exp1 = (exp1)->left;
            if (exp1->type == en_assign)
            {
                expasn = exp1;
                exp1 = exp1->left;
                while(castvalue(exp1))
                    exp1 = (exp1)->left;
            }
            if (!lvalue(exp1))
            {
                if (!btp->array && !btp->vla && !isstructured(btp) && basetype(btp)->type != bt_memberptr && basetype(btp)->type != bt_templateparam)
                    error(ERR_LVALUE);
            }
            else if (!isstructured(btp) && exp1->type != en_l_ref)
                exp1 = (exp1)->left;
                
            switch ((exp1)->type)
            {
                case en_pc:
                case en_auto:
                case en_label:
                case en_global:
                case en_absolute:
                case en_threadlocal:
                    (exp1)->v.sp->addressTaken = TRUE;
                    break;
                default:
                    break;
            }
            if (basetype(btp)->type != bt_memberptr)
            {
                tp1 = Alloc(sizeof(TYPE));
                tp1->type = bt_pointer;
                tp1->size = getSize(bt_pointer);
                tp1->btp = *tp;
                tp1->rootType = tp1;
                *tp = tp1;
                if (expasn)
                    *exp = exprNode(en_void, expasn, exp1);
                else
                    *exp = exp1;
            }
        }
    }
    return lex;
}
static LEXEME *expression_deref(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    lex = getsym();
    lex = expression_cast(lex, funcsp, NULL, tp, exp, NULL, flags);
    if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_pointer, star,
                           funcsp, tp, exp, NULL,NULL, NULL, flags))
    {
        return lex;
    }
    if (*tp && (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp)))
    {
        error(ERR_NOT_AN_ALLOWED_TYPE);
    }
    else if (*tp && basetype(*tp)->type == bt_memberptr)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    if (*tp && !isfunction(*tp) && !isfuncptr(*tp))
    {
        TYPE *btp = basetype(*tp), *btp2, *btp3;;
        if (!ispointer(*tp))
        {
            if (!templateNestingCount || instantiatingTemplate)
                error(ERR_DEREF);
            deref(&stdpointer, exp);
        }
        else
        {
            btp3 = btp->btp;
            btp2 = basetype(btp->btp);
            if (btp2->type == bt_void)
            {
                error(ERR_DEREF);
                deref(&stdpointer, exp);
            }
            else if (isstructured(btp2))
            {
                *tp = btp3;
                *tp = PerformDeferredInitialization (*tp, funcsp);
            }
            else 
            {
                if (!isfunction(btp2))
                {
                    *tp = btp3;
                    deref(*tp, exp);
                    if (isvolatile(btp3))
                        (*exp)->isvolatile = TRUE;
                    if (isrestrict(btp3))
                        (*exp)->isrestrict = TRUE;
                    if (isatomic(btp3))
                    {
                        (*exp)->isatomic = TRUE;
                        if (needsAtomicLockFromType(btp3))
                            (*exp)->lockOffset = btp3->size - ATOMIC_FLAG_SPACE;
                    }
                }
            }
        }
        
        
    }
    return lex;
}
static LEXEME *expression_postfix(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    TYPE *oldType;
    BOOLEAN done = FALSE;
    BOOLEAN localMutable = FALSE;
    if (KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS))
    {
        lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
    }
    else switch(KW(lex))
    {
        case kw_dynamic_cast:
            oldType = NULL;
            lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
            if (*tp && !doDynamicCast(tp, oldType, exp, funcsp))
                if (!typeHasTemplateArg(*tp))
                    errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
            if (isref(*tp))
                *tp = basetype(*tp)->btp;
            break;
        case kw_static_cast:
            oldType = NULL;
            lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
            if (*tp && !doStaticCast(tp, oldType, exp, funcsp, TRUE))
                if (!typeHasTemplateArg(*tp))
                    errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
            if (isref(*tp))
                *tp = basetype(*tp)->btp;
            break;
        case kw_const_cast:
            oldType = NULL;
            lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
            if (*tp && !doConstCast(tp, oldType, exp, funcsp))
                if (!typeHasTemplateArg(*tp))
                    errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
            if (isref(*tp))
                *tp = basetype(*tp)->btp;
            break;
        case kw_reinterpret_cast:
            oldType = NULL;
            lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
            if (*tp && !doReinterpretCast(tp, oldType, exp, funcsp, TRUE))
                if (!typeHasTemplateArg(*tp))
                    errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
            if (isref(*tp))
                *tp = basetype(*tp)->btp;
            break;
        case kw_typeid:
            lex = expression_typeid(lex, funcsp, tp, exp, (flags & _F_PACKABLE));
            break;
        default:
            lex = expression_primary(lex, funcsp, atp, tp, exp, &localMutable, flags);
            if (ismutable)
                *ismutable = localMutable;
            break;
    }
    if (!*tp)
        return lex;
    while (!done && lex)
    {
        enum e_kw kw;
        switch(KW(lex))
        {
            case openbr:
                lex = expression_bracket(lex, funcsp, tp, exp, flags);
                break;
            case openpa:
                lex = expression_arguments(lex, funcsp, tp, exp, flags);
                break;
            case pointsto:
            case dot:
                lex = expression_member(lex, funcsp, tp, exp, ismutable, flags);
                break;
            case autoinc:
            case autodec:

                kw = KW(lex);
                lex = getsym();
                if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_postfix, kw,
                                       funcsp, tp, exp, NULL,NULL, NULL, flags))
                {
                }
                else
                {
                    castToArithmetic(FALSE, tp, exp, kw, NULL, TRUE);
                    if (isconstraw(*tp, TRUE) && !localMutable)
                        error(ERR_CANNOT_MODIFY_CONST_OBJECT);
                    else if (isstructured(*tp))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (!lvalue(*exp) && basetype(*tp)->type != bt_templateparam)
                    {
                        error(ERR_LVALUE);
                    }
                    else
                    {
                        EXPRESSION *exp3 = NULL, *exp1 = NULL;
                        if ((*exp)->left->type == en_func || (*exp)->left->type == en_thisref)
                        {
                            EXPRESSION *exp2 = anonymousVar(sc_auto, *tp);
                            deref(&stdpointer, &exp2);
                            exp3 = exprNode(en_assign, exp2, (*exp)->left);
                            deref(*tp, &exp2);
                            *exp = exp2;
                        }
                        if (basetype(*tp)->type == bt_pointer)
                        {
                            TYPE *btp = basetype(*tp)->btp;
                            if (basetype(btp)->type == bt_void)
                            {
                                if (cparams.prm_cplusplus)
                                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                                exp1 = nodeSizeof(&stdchar, *exp);
                            }
                            else
                            {
                                exp1 = nodeSizeof(btp, *exp);
                            }
                        }
                        else
                        {
                            if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                                error(ERR_NOT_AN_ALLOWED_TYPE);
                            if (basetype(*tp)->scoped)
                                error(ERR_SCOPED_TYPE_MISMATCH);
                            if (basetype(*tp)->type == bt_memberptr)
                                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                            exp1 = intNode(en_c_i, 1);
                        }
                        if (basetype(*tp)->type == bt_bool)
                        {
                            /* autoinc of a BOOLEAN sets it true.  autodec not allowed
                             * these aren't spelled out in the C99 standard, we are
                             * following the C++ standard here
                             */
                            if (kw== autodec)
                                error(ERR_CANNOT_USE_BOOLEAN_HERE);
                            *exp = exprNode(en_assign, *exp, intNode(en_c_bool, 1));
                        }
                        else
                        {
                            cast(*tp, &exp1);
                            *exp = exprNode(kw == autoinc ? en_autoinc : en_autodec,
                                        *exp, exp1);
                        }
                        if (exp3)
                            *exp = exprNode(en_void, exp3, *exp);
                        while (lvalue(exp1))
                            exp1 = exp1->left;
                        if (exp1->type == en_auto)
                            exp1->v.sp->altered = TRUE;
                    }
                }
                break;
            default:
                done = TRUE;
                break;
        }
    }
/*	if (isfunction(*tp))
        error(ERR_FUNCTION_ARGUMENTS_EXPECTED);
*/
    return lex;
}
LEXEME *expression_unary(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    BOOLEAN localMutable = FALSE;
    enum e_kw kw= KW(lex);
    /* note some of the math ops are speced to do integer promotions
     * if being stored, the proposed place to store them is not known, so e.g.
     * a ~ on a unsigned char would promote to int to be evaluated, 
     * the int would be further sign
     * extended to a long long though through the assignment promotion
     */
    switch(kw)
    {
        case plus:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, NULL, flags);
            if (*tp)
            {
                if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_numeric, plus,
                                       funcsp, tp, exp, NULL,NULL, NULL, flags))
                {
                }                                       
                else 
                {
                    castToArithmetic(FALSE, tp, exp, kw, NULL, TRUE);
                    if (isstructured(*tp))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    else if (basetype(*tp)->type == bt_memberptr)
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    else if (basetype(*tp)->scoped)
                        error(ERR_SCOPED_TYPE_MISMATCH);
                    else if (ispointer(*tp))
                        error(ERR_ILL_POINTER_OPERATION);
                    else
                        if (atp && basetype(atp)->type < bt_int)
                        {
                            cast(atp, exp);
                            *tp = atp;
                        }
                        else if (basetype(*tp)->type < bt_int)
                        {
                            cast(&stdint, exp);
                            *tp = &stdint;
                        }
                }
            }
            break;
        case minus:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, NULL, flags);
            if (*tp)
            {
                if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_numeric, minus,
                                       funcsp, tp, exp, NULL,NULL, NULL, flags))
                {
                }
                else {
                    castToArithmetic(FALSE, tp, exp, kw, NULL, TRUE);
                    if (isstructured(*tp))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    else if (basetype(*tp)->type == bt_memberptr)
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    else if (basetype(*tp)->scoped)
                        error(ERR_SCOPED_TYPE_MISMATCH);
                    else if (ispointer(*tp))
                        error(ERR_ILL_POINTER_OPERATION);
                    else
                        if (atp && basetype(atp)->type < bt_int)
                        {
                            cast(atp, exp);
                            *tp = atp;
                        }
                        else if (basetype(*tp)->type < bt_int)
                        {
                            cast(&stdint, exp);
                            *tp = &stdint;
                        }
                    *exp = exprNode(en_uminus, *exp, NULL);
                }
            }
            break;
        case star:
            lex = expression_deref(lex, funcsp, tp, exp, flags);
            break;
        case and:
            lex = expression_ampersand(lex, funcsp, atp,tp, exp, flags);
            break;
        case not:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, NULL, flags);
            if (*tp)
            {
                if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_numericptr, not,
                                       funcsp, tp, exp, NULL,NULL, NULL, flags))
                {
                }
                else 
                {
                    castToArithmetic(FALSE, tp, exp, kw, NULL, TRUE);
                    if (isstructured(*tp))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    else if (basetype(*tp)->scoped)
                        error(ERR_SCOPED_TYPE_MISMATCH);
                        /*
                    else
                        if (basetype(*tp)->type < bt_int)
                        {
                            cast(&stdint, exp);
                            *tp = &stdint;
                        }
                        */
                    if ((*tp)->type == bt_memberptr)
                    {
                           *exp = exprNode(en_mp_as_bool, *exp, NULL);
                        (*exp)->size = (*tp)->size;
                           *exp = exprNode(en_not, *exp, NULL);
                    }
                    else
                    {
                        *exp = exprNode(en_not, *exp, NULL);
                    }
                }
                if (cparams.prm_cplusplus)
                    *tp = &stdbool;
                else
                    *tp = &stdint;
            }
            break;
        case compl:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, NULL, flags);
            if (*tp)
            {
                if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_int, compl,
                                       funcsp, tp, exp, NULL,NULL, NULL, flags))
                {
                }
                else {
                    castToArithmetic(TRUE, tp, exp, kw, NULL, TRUE);
                    if (isstructured(*tp))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (iscomplex(*tp))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (isfloat(*tp) || isimaginary(*tp))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (ispointer(*tp))
                        error(ERR_ILL_POINTER_OPERATION);
                    else if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    else if (basetype(*tp)->type == bt_memberptr)
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    else if (basetype(*tp)->scoped)
                        error(ERR_SCOPED_TYPE_MISMATCH);
                    else
                        if (atp && basetype(atp)->type < bt_int)
                        {
                            cast(atp, exp);
                            *tp = atp;
                        }
                        else if (basetype(*tp)->type < bt_int)
                        {
                            cast(&stdint, exp);
                            *tp = &stdint;
                        }
                    *exp = exprNode(en_compl, *exp, NULL);
                }
            }
            break;
        case autoinc:
        case autodec:
            lex = getsym();
               lex = expression_cast(lex, funcsp, atp, tp, exp, &localMutable, flags);
            if (*tp)
            {
                if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_prefix, kw,
                                       funcsp, tp, exp, NULL,NULL, NULL, flags))
                {
                }
                else 
                {
                    castToArithmetic(FALSE, tp, exp, kw, NULL, TRUE);
                    if (kw == autodec && basetype(*tp)->type == bt_bool)
                        error(ERR_CANNOT_USE_BOOLEAN_HERE);
                    else if (isstructured(*tp))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (iscomplex(*tp))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (isconstraw(*tp, TRUE) && !localMutable)
                        error(ERR_CANNOT_MODIFY_CONST_OBJECT);
                    else if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    else if (basetype(*tp)->type == bt_memberptr)
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    else if (basetype(*tp)->scoped)
                        error(ERR_SCOPED_TYPE_MISMATCH);
                    else if (!lvalue(*exp) && basetype(*tp)->type != bt_templateparam)
                    {
                        error(ERR_LVALUE);
                    }
                    else 
                    {
                        EXPRESSION *exp3 = NULL;
                        if ((*exp)->left->type == en_func || (*exp)->left->type == en_thisref)
                        {
                            EXPRESSION *exp2 = anonymousVar(sc_auto, *tp);
                            deref(&stdpointer, &exp2);
                            exp3 = exprNode(en_assign, exp2, (*exp)->left);
                            deref(*tp, &exp2);
                            *exp = exp2;
                        }
                        if (ispointer(*tp))
                        {
                            TYPE *tpx;
                            if (basetype(basetype(*tp)->btp)->type == bt_void)
                            {
                                if (cparams.prm_cplusplus)
                                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                                tpx = &stdchar;
                            }
                            else
                            {
                                tpx = basetype(*tp)->btp;
                            }
                            *exp = exprNode(en_assign, *exp, exprNode(kw == autoinc ? en_add : en_sub, 
                                                                      *exp, nodeSizeof(tpx, *exp)));
                        }
                        else if (kw == autoinc && basetype(*tp)->type == bt_bool)
                        {
                            *exp = exprNode(en_assign, *exp, intNode(en_c_i, 1)); // set to true as per C++
                        }
                        else
                        {
                            EXPRESSION *dest = *exp, *exp1 = intNode(en_c_i, 1);
                            *exp = RemoveAutoIncDec(*exp);                            
                            cast(*tp, &exp1);
                            *exp = exprNode(en_assign, dest, exprNode(kw == autoinc ? en_add : en_sub, *exp, exp1));
                        }
                        if (exp3)
                            *exp = exprNode(en_void, exp3, *exp);
                    }
                }
            }
            break;
        case kw_sizeof:
            lex = expression_sizeof(lex, funcsp, tp, exp);
            break;
        case kw_alignof:
            lex = expression_alignof(lex, funcsp, tp, exp);
            break;
        case kw_new:
            lex = expression_new(lex, funcsp, tp, exp, FALSE, flags);
            break;
        case kw_delete:
            lex = expression_delete(lex, funcsp, tp, exp, FALSE, flags);
            break;
        case kw_noexcept:
            lex = expression_noexcept(lex, funcsp, tp, exp);
            break;
        case classsel:
        {
            LEXEME *placeholder = lex;
            lex = getsym();
            switch(KW(lex))
            {
                case kw_new:
                    return expression_new(lex, funcsp, tp, exp, TRUE, flags);
                case kw_delete:
                    return expression_delete(lex, funcsp, tp, exp, TRUE, flags);
            }
            lex = prevsym(placeholder);
        }
            // fallthrough
        default:
            lex = expression_postfix(lex, funcsp, atp, tp, exp, ismutable, flags);
            break;
    }
    return lex;
}
LEXEME *expression_cast(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    TYPE *throwaway;
    if (MATCHKW(lex, openpa))
    {
        LEXEME *start = lex;
        lex = getsym();
        if (startOfType(lex, FALSE))
        {
            if (!cparams.prm_cplusplus || resolveToDeclaration(lex))
            {
                BOOLEAN done = FALSE;
                lex = get_type_id(lex, tp, funcsp, sc_cast, FALSE, TRUE);
                (*tp)->used = TRUE;
                needkw(&lex, closepa);
                checkauto(*tp);
                if (MATCHKW(lex, begin))
                {
                    INITIALIZER *init = NULL;
                    SYMBOL *sp = NULL;
                    if (!cparams.prm_c99 && !cparams.prm_cplusplus)
                        error(ERR_C99_STYLE_INITIALIZATION_USED);
                    if (cparams.prm_cplusplus)
                    {
                        sp = makeID(sc_auto, *tp, NULL, AnonymousName());
                        insert(sp, localNameSpace->syms);
                    }
                    lex = initType(lex, funcsp, 0, sc_auto, &init, NULL, *tp, sp, FALSE, flags );
                    *exp = convertInitToExpression(*tp, NULL, funcsp, init, NULL, FALSE);
                    while (!done && lex)
                    {
                        enum e_kw kw;
                        switch(KW(lex))
                        {
                            case openbr:
                                lex = expression_bracket(lex, funcsp, tp, exp, flags);
                                break;
                            case openpa:
                                lex = expression_arguments(lex, funcsp, tp, exp, flags);
                                break;
                            case pointsto:
                            case dot:
                                lex = expression_member(lex, funcsp, tp, exp, ismutable, flags);
                                break;
                            case autoinc:
                            case autodec:
                
                                kw = KW(lex);
                                lex = getsym();
                                if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_unary_postfix, kw,
                                                       funcsp, tp, exp, NULL,NULL, NULL, flags))
                                {
                                }
                                else
                                {
                                    castToArithmetic(FALSE, tp, exp, kw, NULL, TRUE);
                                    if (isstructured(*tp))
                                        error(ERR_ILL_STRUCTURE_OPERATION);
                                    else if (!lvalue(*exp) && basetype(*tp)->type != bt_templateparam)
                                        error(ERR_LVALUE);
                                    else
                                    {
                                        EXPRESSION *exp1 = NULL;
                                        if (basetype(*tp)->type == bt_pointer)
                                        {
                                            TYPE *btp = basetype(*tp)->btp;
                                            exp1 = nodeSizeof(btp, *exp);
                                        }
                                        else
                                        {
                                            if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                                                error(ERR_NOT_AN_ALLOWED_TYPE);
                                            if (basetype(*tp)->scoped)
                                                error(ERR_SCOPED_TYPE_MISMATCH);
                                            if (basetype(*tp)->type == bt_memberptr)
                                                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                                            exp1 = intNode(en_c_i, 1);
                                        }
                                        if (basetype(*tp)->type == bt_bool)
                                        {
                                            /* autoinc of a BOOLEAN sets it true.  autodec not allowed
                                             * these aren't spelled out in the C99 standard, we are
                                             * following the C++ standard here
                                             */
                                            if (kw== autodec)
                                                error(ERR_CANNOT_USE_BOOLEAN_HERE);
                                            *exp = exprNode(en_assign, *exp, intNode(en_c_bool, 1));
                                        }
                                        else
                                        {
                                            cast(*tp, &exp1);
                                            *exp = exprNode(kw == autoinc ? en_autoinc : en_autodec,
                                                        *exp, exp1);
                                        }
                                        while (lvalue(exp1))
                                            exp1 = exp1->left;
                                        if (exp1->type == en_auto)
                                            exp1->v.sp->altered = TRUE;
                                    }
                                }
                                break;
                            default:
                                done = TRUE;
                                break;
                        }
                    }
                }
                else
                { 
                    lex = expression_cast(lex, funcsp, NULL, &throwaway, exp, ismutable, flags);
                    if ((*exp)->type == en_pc || (*exp)->type == en_func && !(*exp)->v.func->ascall)
                        thunkForImportTable(exp);
    //                if ((*exp)->type == en_func)
    //                    *exp = (*exp)->v.func->fcall;
                    if (throwaway)
                    {
                        if (basetype(*tp)->type == bt___string)
                        {
                            if ((*exp)->type == en_labcon && (*exp)->string)
                                (*exp)->type = en_c_string;
                            else if (basetype(throwaway)->type != bt___string)
                                *exp = exprNode(en_x_string, *exp, NULL);
                        }
                        else if (basetype(*tp)->type == bt___object)
                        {
                            if (basetype(throwaway)->type != bt___object)
                                *exp = exprNode(en_x_object, *exp, NULL);
                        }
                        else if (isvoid(throwaway) && !isvoid(*tp) || ismsil(*tp))
                        {
                            error(ERR_NOT_AN_ALLOWED_TYPE);
                        }
                        else if (!cparams.prm_cplusplus && ((isstructured(throwaway) && !isvoid(*tp)) 
                                  || basetype(throwaway)->type == bt_memberptr || basetype(*tp)->type == bt_memberptr)
                                  && !comparetypes(throwaway, *tp, TRUE))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        else if (cparams.prm_cplusplus)
                        {
                            if (!doStaticCast(tp, throwaway, exp, funcsp, FALSE) 
                                && !doReinterpretCast(tp, throwaway, exp, funcsp, FALSE))
                            {
                                cast(*tp, exp);
                            }
                        }
                        else
                        {
                            cast(*tp, exp);
                        }
                    }
                }
            }
            else // expression in parenthesis
            {
                lex = prevsym(start);
                lex = expression_unary(lex, funcsp, atp, tp, exp, ismutable, flags);
            }
        }
        else
        {
            lex = prevsym(start);
            lex = expression_unary(lex, funcsp, atp, tp, exp, ismutable, flags);
        }
    }
    else
    {
        lex = expression_unary(lex, funcsp, atp, tp, exp, ismutable, flags);
    }
    return lex;
} 
static LEXEME *expression_pm(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    lex = expression_cast(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while (MATCHKW(lex, dotstar) || MATCHKW(lex, pointstar))
    {
        BOOLEAN points = FALSE;
        enum e_kw kw = KW(lex);
        enum e_node type ;
        TYPE *tp1 = NULL;
        EXPRESSION *exp1 = NULL;
          lex = getsym();
        lex = expression_cast(lex, funcsp, NULL, &tp1, &exp1, NULL, flags);
        if (cparams.prm_cplusplus && kw == pointstar && insertOperatorFunc(ovcl_binary_any, pointstar,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
            continue;
        }
        if (kw == pointstar)
        {
            points = TRUE;
            if (ispointer(*tp))
            {
                *tp = basetype(*tp);
                *tp = (*tp)->btp;
                if (!isstructured(*tp))
                {
                    errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->kw->name);
                }
            }
            else
                errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->kw->name);
        }
        else if (!isstructured(*tp))
        {
            errorstr(ERR_STRUCTURED_TYPE_EXPECTED, lex->kw->name);
        }
        if (basetype(tp1)->type != bt_memberptr)
        {
            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
        }
        else
        {
            if (isstructured(*tp) && basetype(tp1)->type == bt_memberptr)
            {
                if ((*tp)->sp != basetype(tp1)->sp && (*tp)->sp->mainsym != basetype(tp1)->sp && (*tp)->sp != basetype(tp1)->sp->mainsym)
                {
                    if (classRefCount(basetype(tp1)->sp, (*tp)->sp) != 1)
                    {
                        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, basetype(tp1)->sp, (*tp)->sp);
                    }
                    else
                    {
                        *exp = baseClassOffset(basetype(tp1)->sp, (*tp)->sp, *exp);
                    }
                }
                if (exp1->type == en_memberptr)
                {
                    int lab = dumpMemberPtr(exp1->v.sp, *tp, TRUE);
                    exp1 = intNode(en_labcon, lab);
                }
                if (isfunction(basetype(tp1)->btp))
                {
                    FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                    if ((*tp)->sp->vbaseEntries)
                    {
                        EXPRESSION *ec = exprNode(en_add, exp1, intNode(en_c_i,getSize(bt_pointer) + getSize(bt_int)));
                        EXPRESSION *ec1;
                        deref(&stdint, &ec);
                        ec1 = exprNode(en_sub, ec, intNode(en_c_i,1));
                        deref(&stdint, &ec1);
                        ec = exprNode(en_cond, ec, exprNode(en_void, ec1, intNode(en_c_i, 0)));
                        *exp = exprNode(en_add, *exp, ec);
                    }
                    else
                    {
                        EXPRESSION *ec = exprNode(en_add, exp1, intNode(en_c_i, getSize(bt_pointer)));
                        deref(&stdpointer, &ec);
                        *exp = exprNode(en_add, *exp, ec);
                    }
                    funcparams->sp = tp1->btp->sp;
                    funcparams->fcall = exp1;
                    deref(&stdpointer, &funcparams->fcall);
                    funcparams->thisptr = *exp;
                    funcparams->thistp = Alloc(sizeof(TYPE));
                    funcparams->thistp->size = getSize(bt_pointer);
                    funcparams->thistp->type = bt_pointer;
                    funcparams->thistp->btp = *tp;
                    funcparams->thistp->rootType = funcparams->thistp;
                    *exp = varNode(en_func, NULL);
                    (*exp)->v.func = funcparams;
                    *tp = basetype(tp1);
                }
                else
                {
                    if ((*tp)->sp->vbaseEntries)
                    {
                        EXPRESSION *ec = exprNode(en_add, exp1, intNode(en_c_i, getSize(bt_int)));
                        EXPRESSION *ec1;
                        deref(&stdint, &ec);
                        ec1 = exprNode(en_add, ec, intNode(en_c_i,-1));
                        ec1 = exprNode(en_add, *exp, ec1);
                        deref(&stdint, &ec1);
                        *exp = exprNode(en_cond, ec, exprNode(en_void, ec1, *exp));
                    }
                    deref(&stdint, &exp1);
                    *exp = exprNode(en_add, *exp, exp1);
                    *exp = exprNode(en_add, *exp, intNode(en_c_i, -1));
                    if (!isstructured(basetype(tp1)->btp))
                    {
                        deref(basetype(tp1)->btp, exp);                    
                    }
                    *tp = basetype(tp1)->btp;
                }
            }
        }
    }
    return lex;
}
static LEXEME *expression_times(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    lex = expression_pm(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while (MATCHKW(lex, star) || MATCHKW(lex, divide) || MATCHKW(lex, mod))
    {
        enum e_kw kw = KW(lex);
        enum e_node type ;
        TYPE *tp1 = NULL;
        EXPRESSION *exp1 = NULL;
        lex = getsym();
        lex = expression_pm(lex, funcsp, NULL, &tp1, &exp1, NULL, flags);
        if (!tp1)
        {
            *tp = NULL;
            return lex;
        }
        if (cparams.prm_cplusplus
            && insertOperatorFunc(kw == mod ? ovcl_binary_int : ovcl_binary_numeric, kw,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
        }
        else 
        {
            castToArithmetic(kw == mod, tp, exp, kw, tp1, TRUE);
            castToArithmetic(kw == mod, &tp1, &exp1, (enum e_kw)-1, *tp, TRUE);
            if (isstructured(*tp) || isstructured(tp1))
                error(ERR_ILL_STRUCTURE_OPERATION);
            else if (isvoid(*tp) || isvoid(tp1) || (*tp)->type == bt_aggregate  || tp1->type == bt_aggregate || ismsil(*tp) || ismsil(tp1))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            else if (basetype(*tp)->scoped || basetype(tp1)->scoped)
                error(ERR_SCOPED_TYPE_MISMATCH);
            else if (ispointer(*tp) || ispointer(tp1))
                error(ERR_ILL_POINTER_OPERATION);
            else
            {
                *tp = destSize(*tp, tp1, exp, &exp1, FALSE, NULL);
                switch(kw)
                {
                    case star:
                        type = en_mul;
                        break;
                    case divide:
                        type = isunsigned(*tp) ? en_udiv : en_div;
                        break;
                    case mod:
                        type = isunsigned(*tp) ? en_umod : en_mod;
                        break;
                    default:
                        break;
                }
                *exp = exprNode(type, *exp, exp1);
            }
        }
    }
    return lex;
}

static LEXEME *expression_add(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
/* fixme add vlas */
    lex = expression_times(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while (MATCHKW(lex, plus) || MATCHKW(lex, minus))
    {
        BOOLEAN msil = FALSE;
        enum e_kw kw = KW(lex);
        TYPE *tp1 = NULL;
        EXPRESSION *exp1 = NULL;
        lex = getsym();
        lex = expression_times(lex, funcsp, atp, &tp1, &exp1, NULL, flags);
        if (!tp1)
        {
            *tp = NULL;
            return lex;
        }
        if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_binary_numericptr, kw,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
            continue;
        }
        else {
            if (!ispointer(*tp) && !ispointer(tp1))
            {
                castToArithmetic(FALSE, tp, exp, kw, tp1, TRUE);
                castToArithmetic(FALSE, &tp1, &exp1, (enum e_kw)-1, *tp, TRUE);
            }
            if (chosenAssembler->msil && kw == plus && (basetype(*tp)->type == bt___string || basetype(tp1)->type == bt___string || atp&&basetype(atp)->type == bt___string))
            {
                msil = TRUE;
                if ((*exp)->type == en_labcon && (*exp)->string)
                    (*exp)->type = en_c_string;
                else if (!ismsil(*tp))
                    *exp = exprNode(en_x_object, *exp, NULL);
                if (exp1->type == en_labcon && exp1->string)
                    exp1->type = en_c_string;
                else if (!ismsil(tp1))
                    exp1 = exprNode(en_x_object, exp1, NULL);
            }
            else if (kw == plus && ispointer(*tp) && ispointer(tp1))
                error(ERR_ILL_POINTER_ADDITION);
            else if (isvoid(*tp) || isvoid(tp1) || (*tp)->type == bt_aggregate || tp1->type == bt_aggregate ||
                    ismsil(*tp) || ismsil(tp1))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            else if (basetype(*tp)->scoped || basetype(tp1)->scoped)
                error(ERR_SCOPED_TYPE_MISMATCH);
            else if (kw != plus && !ispointer(*tp) && ispointer(tp1))
                error(ERR_ILL_POINTER_SUBTRACTION);
            else if (isstructured(*tp) || isstructured(tp1))
                error(ERR_ILL_STRUCTURE_OPERATION);
            else if (ispointer(*tp))
            {
                if (isarray(*tp) && (*tp)->msil)
                    error(ERR_MANAGED_OBJECT_NO_ADDRESS);
                else if (ispointer(tp1) && !comparetypes(*tp, tp1, TRUE) && !comparetypes(tp1, *tp, TRUE))
                    error(ERR_NONPORTABLE_POINTER_CONVERSION);
                else if (iscomplex(tp1))
                    error(ERR_ILL_USE_OF_COMPLEX);
                else if (isfloat(tp1) || isimaginary(tp1))
                    error(ERR_ILL_USE_OF_FLOATING);
            }
            else if (ispointer(tp1))
            {
                if (iscomplex(*tp))
                    error(ERR_ILL_USE_OF_COMPLEX);
                else if (isfloat(*tp) || isimaginary(*tp))
                    error(ERR_ILL_USE_OF_FLOATING);
                else if (isarray(tp1) && (tp1)->msil)
                    error(ERR_MANAGED_OBJECT_NO_ADDRESS);
            }
        }
        if (msil)
        {
            // MSIL back end will take care of figuring out what function to call
            // to perform the concatenation
            *exp = exprNode(en_add, *exp, exp1);
            *tp = &std__string;
        }
        else if (ispointer(*tp))
        {
            EXPRESSION *ns;
            if (basetype(basetype(*tp)->btp)->type == bt_void)
            {
                if (cparams.prm_cplusplus)
                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                ns = nodeSizeof(&stdchar, *exp);
            }
            else
            {
                ns = nodeSizeof(basetype(*tp)->btp, *exp);
            }
            if (ispointer(tp1))
            {
                *exp = exprNode(en_sub, *exp, exp1);
                *exp = exprNode(en_arraydiv, *exp, ns);
                *tp = &stdint; /* ptrdiff_t */
            }
            else
            {
/*				*tp = tp1 = destSize(*tp, tp1, exp, &exp1, FALSE, NULL); */
                if (basetype(tp1)->type < bt_int)
                    cast(&stdint, &exp1);
                exp1 = exprNode(en_umul, exp1, ns);
                *exp = exprNode(kw == plus ? en_add : en_sub, *exp, exp1);
            }
        }
        else if (ispointer (tp1))
        {
            EXPRESSION *ns;
            if (basetype(basetype(tp1)->btp)->type == bt_void)
            {
                if (cparams.prm_cplusplus)
                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                ns = nodeSizeof(&stdchar, *exp);
            }
            else
            {
                ns  = nodeSizeof(basetype(tp1)->btp, *exp);
            }
/*			*tp = tp1 = destSize(*tp, tp1, exp, &exp1, FALSE, NULL); */
            if (basetype(*tp)->type < bt_int)
                cast(&stdint, exp);
            *exp = exprNode(en_umul, *exp, ns);
            *exp = exprNode(en_add, *exp, exp1);
            *tp = tp1;
        }
        else
        {
            *tp = destSize(*tp, tp1, exp, &exp1, TRUE, atp);
            *exp = exprNode(kw == plus ? en_add : en_sub, *exp, exp1);
        }
    }
    return lex;
}
static LEXEME *expression_shift(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    lex = expression_add(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while ((MATCHKW(lex, rightshift) && !(flags & _F_INTEMPLATEPARAMS)) || MATCHKW(lex, leftshift))
    {
        TYPE *tp1 = NULL;
        EXPRESSION *exp1 = NULL;
        enum e_node type ; 
        enum e_kw kw = KW(lex);
        lex = getsym();
        lex = expression_add(lex, funcsp, NULL, &tp1, &exp1, NULL, flags);
        if (!tp1)
        {
            *tp = NULL;
            return lex;
        }
        if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_binary_int, kw,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
        }
        else {
            castToArithmetic(TRUE, tp, exp, kw, tp1, TRUE);
            castToArithmetic(TRUE, &tp1, &exp1, (enum e_kw)-1, *tp, TRUE);
            if (isstructured(*tp) || isstructured(tp1))
                error(ERR_ILL_STRUCTURE_OPERATION);
            else if (isvoid(*tp) || isvoid(tp1) || (*tp)->type == bt_aggregate  || tp1->type == bt_aggregate || ismsil(*tp) || ismsil(tp1))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            else if (basetype(*tp)->scoped || basetype(tp1)->scoped)
                error(ERR_SCOPED_TYPE_MISMATCH);
            else if (ispointer(*tp) || ispointer(tp1))
                error(ERR_ILL_POINTER_OPERATION);
            else if (isfloat(*tp) || isfloat(tp1) || isimaginary(*tp) || isimaginary(tp1))
                error(ERR_ILL_USE_OF_FLOATING);
            else if (iscomplex(*tp) || iscomplex(tp1))
                error(ERR_ILL_USE_OF_COMPLEX);
            else
            {
                if (kw == rightshift)
                    if (isunsigned(*tp))
                        type = en_ursh;
                    else
                        type = en_rsh;
                else
                    type = en_lsh;
                if (basetype(*tp)->type < bt_int)
                {
                    *tp = &stdint;
                    cast(*tp, exp);
                }
                *exp = exprNode(type, *exp, exprNode(en_shiftby, exp1, 0)); 
            }
        }
    }
    return lex;
}
static LEXEME *expression_inequality(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    BOOLEAN done = FALSE;
    lex = expression_shift(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while (!done && lex)
    {	
        enum e_kw kw = KW(lex);
        enum e_node type ;
        char *opname;
        TYPE *tp1 = NULL;
        EXPRESSION *exp1 = NULL;
        switch(kw)
        {
            case gt:
                opname = lex->kw->name;
                done = (flags & _F_INTEMPLATEPARAMS);
                break;
            case geq:
            case lt:
            case leq:
                opname = lex->kw->name;
                break;
            default:
                done = TRUE;
                break;
        }
        if (!done)
        {
            LEXEME *current = lex;
            lex = getsym();
            
            lex = expression_shift(lex, funcsp, NULL, &tp1, &exp1, NULL, flags);
            if (!tp1)
            {
                *tp = NULL;
                return lex;
            }
            if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_binary_numericptr, kw,
                                   funcsp, tp, exp, tp1, exp1, NULL, flags))
            {
            }
            else
            {
                checkscope(*tp, tp1);
                castToArithmetic(FALSE, tp, exp, kw, tp1, TRUE);
                castToArithmetic(FALSE, &tp1, &exp1, (enum e_kw)-1, *tp, TRUE);
                if (cparams.prm_cplusplus)
                {
                    SYMBOL *funcsp= NULL;
                    if ((ispointer(*tp) || basetype(*tp)->type == bt_memberptr) && tp1->type == bt_aggregate)
                    {
                        if (tp1->syms->table[0]->next)
                            errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL *)tp1->syms->table[0]->p)->name);
                        exp1 = varNode(en_pc, tp1->syms->table[0]->p);
                        tp1 = ((SYMBOL *)tp1->syms->table[0]->p)->tp;
                            
                    }
                    else if ((ispointer(tp1) || basetype(tp1)->type == bt_memberptr) && (*tp)->type == bt_aggregate)
                    {
                        if ((*tp)->syms->table[0]->next)
                            errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL *)(*tp)->syms->table[0]->p)->name);
                        (*exp) = varNode(en_pc, (*tp)->syms->table[0]->p);
                        (*tp) = ((SYMBOL *)(*tp)->syms->table[0]->p)->tp;
                    }
                    if (funcsp)
                        funcsp->genreffed = TRUE;
                }
                if ((*exp)->type == en_pc || (*exp)->type == en_func && !(*exp)->v.func->ascall)
                    thunkForImportTable(exp);
                if (exp1->type == en_pc || exp1->type == en_func && !exp1->v.func->ascall)
                    thunkForImportTable(&exp1);
                if (isstructured(*tp) || isstructured(tp1))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (isvoid(*tp) || isvoid(tp1) || (*tp)->type == bt_aggregate  || tp1->type == bt_aggregate || ismsil(*tp) || ismsil(tp1))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if (ispointer(*tp))
                {
                    if (isintconst(exp1))
                    {
                        if (!isconstzero(tp1, exp1))
                            error(ERR_NONPORTABLE_POINTER_COMPARISON);
                    }
                    else if (isint(tp1))
                        error(ERR_NONPORTABLE_POINTER_COMPARISON);
                    else if (isfloat(tp1) || isimaginary(tp1))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (iscomplex(tp1))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (ispointer(tp1) && !isvoidptr(*tp) && !isvoidptr(tp1) && !comparetypes(*tp, tp1, FALSE))
                        error(ERR_NONPORTABLE_POINTER_COMPARISON);
                }				
                else if (ispointer(tp1))
                {
                    if (isintconst(*exp))
                    {
                        if (!isconstzero(*tp, *exp))
                            error(ERR_NONPORTABLE_POINTER_COMPARISON);
                    }
                    else if (isint(*tp))
                        error(ERR_NONPORTABLE_POINTER_COMPARISON);
                    else if (isfloat(*tp) || isimaginary(*tp))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (iscomplex(*tp))
                        error(ERR_ILL_USE_OF_COMPLEX);
                }
                else if (isint(*tp) && isint(tp1))
                {
                    if ((isunsigned(*tp) && !isunsigned(tp1)) ||
                        (isunsigned(tp1) && !isunsigned(*tp)))
                        errorstr(ERR_SIGNED_UNSIGNED_MISMATCH_RELAT, opname);
                }
                *tp = destSize(*tp, tp1, exp, &exp1, TRUE, NULL);
                switch(kw)
                {
                    case gt:
                        type = isunsigned(*tp) ? en_ugt : en_gt;
                        break;
                    case geq:
                        type = isunsigned(*tp) ? en_uge : en_ge;
                        break;
                    case lt:
                        type = isunsigned(*tp) ? en_ult : en_lt;
                        break;
                    case leq:
                        type = isunsigned(*tp) ? en_ule : en_le;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
                *exp = exprNode(type, *exp, exp1);
                if (cparams.prm_cplusplus)
                    *tp = &stdbool;
                else
                    *tp = &stdint;
            }
        }
    }
    return lex;
}
static LEXEME *expression_equality(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    lex = expression_inequality(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while (MATCHKW(lex, eq) || MATCHKW(lex, neq))
    {
        BOOLEAN done = FALSE;
        TYPE *tp1 = NULL;
        EXPRESSION *exp1 = NULL;
        enum e_kw kw = KW(lex);
        lex = getsym();
        lex = expression_inequality(lex, funcsp, NULL, &tp1, &exp1, NULL, flags);
        if (!tp1)
        {
            *tp = NULL;
            return lex;
        }
        if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_binary_numericptr, kw,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
        }
        else
        {
            extern int total_errors;
            checkscope(*tp, tp1);
            castToArithmetic(FALSE, tp, exp, kw, tp1, TRUE);
            castToArithmetic(FALSE, &tp1, &exp1, (enum e_kw)-1, *tp, TRUE);
            if (total_errors)
            {
                insertOperatorFunc(ovcl_binary_numericptr, kw,
                               funcsp, tp, exp, tp1, exp1, NULL, flags);
            }
            if (cparams.prm_cplusplus)
            {
                SYMBOL *funcsp= NULL;
                if ((ispointer(*tp) || basetype(*tp)->type == bt_memberptr) && tp1->type == bt_aggregate)
                {
                    if (tp1->syms->table[0]->next)
                        errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL *)tp1->syms->table[0]->p)->name);
                    exp1 = varNode(en_pc, tp1->syms->table[0]->p);
                   tp1 = ((SYMBOL *)tp1->syms->table[0]->p)->tp;
                        
                }
                else if ((ispointer(tp1) || basetype(tp1)->type == bt_memberptr) && (*tp)->type == bt_aggregate)
                {
                    if ((*tp)->syms->table[0]->next)
                        errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL *)(*tp)->syms->table[0]->p)->name);
                    (*exp) = varNode(en_pc, (*tp)->syms->table[0]->p);
                    (*tp) = ((SYMBOL *)(*tp)->syms->table[0]->p)->tp;
                }
                if (funcsp)
                    funcsp->genreffed = TRUE;
            }
            if ((*exp)->type == en_pc || (*exp)->type == en_func && !(*exp)->v.func->ascall)
                thunkForImportTable(exp);
            if (exp1->type == en_pc || exp1->type == en_func && !exp1->v.func->ascall)
                thunkForImportTable(&exp1);
            if (isstructured(*tp) || isstructured(tp1))
                error(ERR_ILL_STRUCTURE_OPERATION);
            else if (isvoid(*tp) || isvoid(tp1) || ismsil(*tp) || ismsil(tp1))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            if (ispointer(*tp))
            {
                if (isintconst(exp1))
                {
                    if (!isconstzero(tp1, exp1))
                        error(ERR_NONPORTABLE_POINTER_COMPARISON);
                }
                else if (isint(tp1))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
                else if (isfloat(tp1) || isimaginary(tp1))
                    error(ERR_ILL_USE_OF_FLOATING);
                else if (iscomplex(tp1))
                    error(ERR_ILL_USE_OF_COMPLEX);
                else if (ispointer(tp1) && !isvoidptr(*tp) && !isvoidptr(tp1) && !comparetypes(*tp, tp1, FALSE))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
            }				
            else if (ispointer(tp1))
            {
                if (isintconst(*exp))
                {
                    if (!isconstzero(*tp, *exp))
                        error(ERR_NONPORTABLE_POINTER_COMPARISON);
                }
                else if (isint(*tp))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
                else if (isfloat(*tp) || isimaginary(*tp))
                    error(ERR_ILL_USE_OF_FLOATING);
                else if (iscomplex(*tp))
                    error(ERR_ILL_USE_OF_COMPLEX);
            }
            else if (basetype(*tp)->type == bt_memberptr)
            {
                if (basetype(tp1)->type == bt_memberptr)
                {
                    if (!comparetypes(basetype(*tp)->btp, basetype(tp1)->btp, TRUE))
                    {
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    }
                    *exp = exprNode(en_mp_compare, *exp, exp1);
                    (*exp)->size = (*tp)->size;
                    if (kw == neq)
                        *exp = exprNode(en_not, *exp, NULL);
                    done = TRUE;
                }
                else if (isconstzero(tp1, exp1))
                {
                    *exp = exprNode(en_mp_as_bool, *exp, NULL);
                    (*exp)->size = (*tp)->size;
                    if (kw == eq)
                        *exp = exprNode(en_not, *exp, NULL);
                    done = TRUE;
                }
                else if (comparetypes(basetype(*tp)->btp, tp1, TRUE))
                {
                    int lbl = dumpMemberPtr(exp1->v.sp, *tp, TRUE);
                    exp1 = intNode(en_labcon, lbl);
                    *exp = exprNode(en_mp_compare, *exp, exp1);
                    (*exp)->size = (*tp)->size;
                    if (kw == neq)
                        *exp = exprNode(en_not, *exp, NULL);
                    done = TRUE;
                }
                else
                {
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                }
            }
            else if (basetype(tp1)->type == bt_memberptr)
            {
                if (isconstzero(*tp, *exp))
                {
                    *exp = exprNode(en_mp_as_bool, exp1, NULL);
                    (*exp)->size = (tp1)->size;
                    if (kw == eq)
                        *exp = exprNode(en_not, *exp, NULL);
                    done = TRUE;
                }
                else if (comparetypes(*tp, basetype(tp1)->btp, TRUE))
                {
                    int lbl = dumpMemberPtr((*exp)->v.sp, tp1, TRUE);
                    *(exp) = intNode(en_labcon, lbl);
                    *exp = exprNode(en_mp_compare, *exp, exp1);
                    (*exp)->size = tp1->size;
                    if (kw == neq)
                        *exp = exprNode(en_not, *exp, NULL);
                    done = TRUE;
                }
                else
                {
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                }
            }
            if (!done)
            {
                if (!(chosenAssembler->arch->preferopts & OPT_BYTECOMPARE)
                    || (!fittedConst(*tp, *exp) && !fittedConst(tp1, exp1))
                    || !isint(*tp) || !isint(tp1))
                    destSize(*tp, tp1, exp, &exp1, TRUE, NULL);
                *exp = exprNode(kw == eq ? en_eq : en_ne, *exp, exp1);
            }
            if (cparams.prm_cplusplus)
                *tp = &stdbool;
            else
                *tp = &stdint;
        }
    }
    return lex;
}
void GetLogicalDestructors(EXPRESSION *top, EXPRESSION *cur)
{
    if (!cur || cur->type == land || cur->type == lor || cur->type == hook)
        return;
    if (cur->type == en_func)
    {
        INITLIST *args = cur->v.func->arguments;
        while (args)
        {
            GetLogicalDestructors(top, args->exp);
            args = args->next;
        }
        if (cur->v.func->returnSP)
        {
            SYMBOL *sp = cur->v.func->returnSP;
            if (!sp->destructed && sp->dest && sp->dest->exp)
            {
                LIST *listitem;
                sp->destructed = TRUE;
                listitem = (LIST *)Alloc(sizeof(LIST));
                listitem->data = sp->dest->exp;
                listitem->next = top->destructors;
                top->destructors = listitem;
            }
        }
    }
    if (cur->left)
    {
        GetLogicalDestructors(top, cur->left);
    }
    if (cur->right)
    {
        GetLogicalDestructors(top, cur->right);
    }
    
}
static LEXEME *binop(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE ** tp, EXPRESSION **exp, enum e_kw kw, enum e_node type, 
              LEXEME *(nextFunc)(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags), 
              BOOLEAN *ismutable, int flags)
{
    BOOLEAN first = TRUE;
    lex = (*nextFunc)(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while (MATCHKW(lex, kw))
    {
        TYPE *tp1 = NULL;
        EXPRESSION *exp1 = NULL;
        if (first)
        {
            first = FALSE;
            GetLogicalDestructors(*exp, *exp);
        }
        lex = getsym();
        lex = (*nextFunc)(lex, funcsp, atp, &tp1, &exp1, NULL, flags);
        if (!tp1)
        {
            *tp = NULL;
            break;
        }
        GetLogicalDestructors(exp1, exp1);
        if (cparams.prm_cplusplus 
            && insertOperatorFunc(kw == lor || kw == land ? ovcl_binary_numericptr : ovcl_binary_int, kw,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
            continue;
        }
        castToArithmetic(kw != land && kw!= lor, tp, exp, kw, tp1, TRUE);
        castToArithmetic(kw != land && kw!= lor, &tp1, &exp1, (enum e_kw)-1, *tp, TRUE);
        if (isstructured(*tp) || isstructured(tp1))
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if (isvoid(*tp) || isvoid(tp1) || (*tp)->type == bt_aggregate  || tp1->type == bt_aggregate || ismsil(*tp) || ismsil(tp1))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (basetype(*tp)->scoped || basetype(tp1)->scoped)
            error(ERR_SCOPED_TYPE_MISMATCH);
        if (type != en_lor && type != en_land)
        {
            if (ispointer(*tp) || ispointer(tp1))
                error(ERR_ILL_POINTER_OPERATION);
            else if (isfloat(*tp) || isfloat(tp1) || isimaginary(*tp) || isimaginary(tp1))
                error(ERR_ILL_USE_OF_FLOATING);
            else if (iscomplex(*tp) || iscomplex(tp1))
                error(ERR_ILL_USE_OF_COMPLEX);
            else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            *tp = destSize(*tp, tp1, exp, &exp1, TRUE, atp);
        }
        else
        {
            if (cparams.prm_cplusplus)
                *tp = &stdbool;
            else
                *tp = &stdint;
        }
        if (basetype(*tp)->type == bt_memberptr)
        {
            *exp = exprNode(en_mp_as_bool, *exp, NULL);
            (*exp)->size = (*tp)->size;
        }
        if (basetype(tp1)->type == bt_memberptr)
        {
            exp1 = exprNode(en_mp_as_bool, exp1, NULL);
            exp1->size = (tp1)->size;
        }
        *exp = exprNode(type, *exp, exp1);
    }
    return lex;
}
static LEXEME *expression_and(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, and, en_and, expression_equality, ismutable, flags);
}
static LEXEME *expression_xor(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, uparrow, en_xor, expression_and, ismutable, flags);
}
static LEXEME *expression_or(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, or, en_or, expression_xor, ismutable, flags);
}
static LEXEME *expression_land(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, land, en_land, expression_or, ismutable, flags);
}
static LEXEME *expression_lor(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, lor, en_lor, expression_land, ismutable, flags);
}

static LEXEME *expression_hook(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    lex = expression_lor(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    if (MATCHKW(lex, hook))
    {
        TYPE *tph = NULL,*tpc = NULL;
        EXPRESSION *eph=NULL, *epc = NULL;
        castToArithmetic(FALSE, tp, exp, (enum e_kw)-1, &stdint, TRUE);
        GetLogicalDestructors(*exp, *exp);
        if (isstructured(*tp))
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        lex = getsym();
        if (MATCHKW(lex, colon))
        {
            // replicate the selector into the 'true' value
            eph = anonymousVar(sc_auto, *tp);
            deref(*tp, &eph);
            tph = *tp;
            *exp = exprNode(en_assign, eph, *exp);
        }
        else
        {
            lex = expression_comma(lex, funcsp, NULL, &tph, &eph, NULL, flags);
        }
        if (!tph)
        {
            *tp = NULL;
        }
        else if (MATCHKW(lex, colon))
        {
            lex = getsym();
            lex = expression_assign(lex, funcsp, NULL, &tpc, &epc, NULL, flags);
            if (!tpc)
            {
                *tp = NULL;
            }
            else
            {
                if (basetype(*tp)->type == bt_memberptr)
                {
                    *exp = exprNode(en_mp_as_bool, *exp, NULL);
                    (*exp)->size = (*tp)->size;
                }
                if (tph->type == bt_void)
                    tph = tpc;
                else if (tpc->type == bt_void)
                    tpc = tph;
                if (ispointer(tph) || ispointer(tpc))
                    if (!comparetypes(tph, tpc, FALSE))
                        if (!isconstzero(tph,eph) && !isconstzero(tpc, epc))
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                if (isfunction(tph) || isfunction(tpc))
                    if (!comparetypes(tph, tpc, TRUE))
                        if (!isconstzero(tph,eph) && !isconstzero(tpc, epc))
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                if (tph != tpc && (ispointer(tph) || isfunction(tph) || ispointer(tpc) || isfunction(tpc)))
                {
                    if (!comparetypes(tpc, tph, TRUE))
                    {
                        if ((isvoidptr(tpc) && ispointer(tph)) || (isvoidptr(tph) &&ispointer(tpc)))
                        {
                            if (tpc->nullptrType)
                                tph = tpc;
                            else if (tph->nullptrType)
                                tpc = tph;
                            else
                                tpc = tph = &stdpointer;
                        }
                        else if (!((ispointer(tph) || isfunction(tph))) && 
                                 !((ispointer(tpc) || isfunction(tpc))))
                        {
                            if (!comparetypes(tpc, tph, FALSE))
                                if (!(isarithmetic(tpc) && isarithmetic(tph)))
                                    errortype(ERR_TWO_OPERANDS_SAME_TYPE, tpc, tph);
                        }
                        else if ((isfunction(tph) || isfunction(tpc)) && !comparetypes(tpc, tph, TRUE))
                            if (!isarithmetic(tph) && !isarithmetic(tpc)) 
                                errortype(ERR_TWO_OPERANDS_SAME_TYPE, tpc, tph);
                    }
                }
                if (isfunction(tph))
                    *tp = tph;
                else if (isfunction(tpc))
                    *tp = tpc;
                else if (!isvoid(tpc))
                    *tp = destSize(tpc,tph,&epc,&eph, FALSE, NULL);
                else 
                    *tp = tpc;
                *exp = exprNode(en_cond, *exp, exprNode(en_void, eph, epc));
                if (isstructured(*tp))
                    *exp = exprNode(en_not_lvalue, *exp, NULL);
            }
        }
        else
        {
            error(ERR_HOOK_NEEDS_COLON);
            *tp = NULL;
        }
    }
    return lex;
}
static BOOLEAN isTemplatedPointer(TYPE *tp)
{
    TYPE *tpb = basetype(tp)->btp;
    while (tp != tpb)
    {
        if (tp->templateTop)
            return TRUE;
        tp = tp->btp;
    }
    return FALSE;
}
LEXEME *expression_throw(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    TYPE *tp1 = NULL;
    EXPRESSION *exp1 = NULL;
    *tp = &stdvoid;
    hasXCInfo = TRUE;
    lex = getsym();
    functionCanThrow = TRUE;
    if (!MATCHKW(lex, semicolon))
    {
        SYMBOL *sp = namespacesearch("_ThrowException", globalNameSpace, FALSE, FALSE);
        makeXCTab(funcsp);
        lex = expression_assign(lex, funcsp, NULL, &tp1, &exp1, NULL, 0);
        if (!tp1)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }   
        else if (sp)
        {
            FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
            INITLIST *arg1 = Alloc(sizeof(INITLIST)); // exception table
            INITLIST *arg2 = Alloc(sizeof(INITLIST)); // instance
            INITLIST *arg3 = Alloc(sizeof(INITLIST)); // array size
            INITLIST *arg4 = Alloc(sizeof(INITLIST)); // constructor
            INITLIST *arg5 = Alloc(sizeof(INITLIST)); // exception block
            SYMBOL *rtti = RTTIDumpType(tp1);
            SYMBOL *cons = NULL;
            if (isstructured(tp1))
            {
                cons = getCopyCons(basetype(tp1)->sp, FALSE);
                if (!cons->inlineFunc.stmt)
                {
                    if (cons->defaulted)
                        createConstructor(basetype(tp1)->sp, cons);
                    else if (cons->deferredCompile)
                        deferredCompileOne(cons);
                }
                GENREF(cons);
            }
            sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
            arg1->next = arg2;
            arg2->next = arg3;
            arg3->next = arg4;
            arg4->next = arg5;
            arg1->exp = varNode(en_auto, funcsp->xc->xctab);
            arg1->tp = &stdpointer;
            if (isstructured(tp1))
            {
                arg2->exp = exp1;
            }
            else
            {
                EXPRESSION *exp3 = anonymousVar(sc_auto, tp1);
                arg2->exp = exp3;
                deref(tp1->type == bt_pointer ? &stdpointer: tp1, &exp3);
                exp3 = exprNode(en_assign, exp3, exp1);
                arg2->exp = exprNode(en_void, exp3, arg2->exp);
            }
            arg2->tp = &stdpointer;
            arg3->exp = isarray(tp1) ? intNode(en_c_i, tp1->size/(basetype(tp1)->btp->size)) : intNode(en_c_i, 1);
            arg3->tp = &stdint;
            arg4->exp = cons ? varNode(en_pc, cons) : intNode(en_c_i, 0);
            arg4->tp = &stdpointer;
            arg5->exp = rtti ? varNode(en_global, rtti) : intNode(en_c_i, 0);
            arg5->tp = &stdpointer;
            params->arguments = arg1;
            params->ascall = TRUE;
            params->sp = sp;
            params->functp = sp->tp;
            params->fcall = varNode(en_pc, sp);
            *exp = exprNode(en_func, NULL, NULL);
            (*exp)->v.func = params;
        }
    }
    else
    {
        SYMBOL *sp = namespacesearch("_RethrowException", globalNameSpace, FALSE, FALSE);
        if (sp)
        {
            FUNCTIONCALL *parms = Alloc(sizeof(FUNCTIONCALL));
            INITLIST *arg1 = Alloc(sizeof(INITLIST)); // exception table
            makeXCTab(funcsp);
            sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
            parms->ascall = TRUE;
            parms->sp = sp;
            parms->functp = sp->tp;
            parms->fcall = varNode(en_pc, sp);
            parms->arguments = arg1;
            arg1->exp = varNode(en_auto, funcsp->xc->xctab);
            arg1->tp = &stdpointer;
            *exp = exprNode(en_func, NULL, NULL);
            (*exp)->v.func = parms;
        }
    }
    return lex;
}
LEXEME *expression_assign(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    BOOLEAN done = FALSE;
    EXPRESSION *exp1=NULL, **exp2;
    EXPRESSION *asndest = NULL;
    
    BOOLEAN localMutable = FALSE;
    TYPE *tp2;
    if (MATCHKW(lex, kw_throw))
    {
        return expression_throw(lex, funcsp, tp, exp);
    }
    lex = expression_hook(lex, funcsp, atp, tp, exp, &localMutable, flags);
    if (*tp == NULL)
        return lex;
    while (!done && lex)
    {
        enum e_kw kw = KW(lex);
        enum e_node op;
        enum ovcl selovcl;
        TYPE *tp1 = NULL;
        switch(kw)
        {
            case asand:
            case asor:
            case asxor:
            case asmod:
            case asleftshift:
            case asrightshift:
                selovcl = ovcl_assign_int;
                break;
            case astimes:
            case asdivide:
                selovcl = ovcl_assign_numeric;
                break;
            case assign:
                selovcl = ovcl_assign_any;
                break;
            case asplus:
            case asminus:
                selovcl = ovcl_assign_numericptr;
                break;
            default:
                if (ismutable)
                    *ismutable = localMutable;
                done = TRUE;
                continue;
        }
        lex = getsym();
        switch(kw)
        {
            case assign:
                if (cparams.prm_cplusplus && MATCHKW(lex, begin))
                {
                    if (isstructured(*tp))
                    {
                        EXPRESSION *destexp = exp1 = anonymousVar(sc_auto, *tp);
                        SYMBOL *sp = destexp->v.sp;
                        INITIALIZER *init = NULL;
                        SYMBOL *spinit = NULL;
                        tp1 = *tp;
                        spinit = anonymousVar(sc_localstatic, tp1)->v.sp;
                        insert(spinit, localNameSpace->syms);
                        lex = initType(lex, funcsp, 0, sc_auto, &init, NULL, tp1, spinit, FALSE, flags);
                        exp1 = convertInitToExpression(tp1, NULL, funcsp, init, exp1, FALSE);
                    }
                    else
                    {
                        lex = getsym();
                        lex = expression_assign(lex, funcsp, *tp, &tp1, &exp1, NULL, flags);
                        if (!needkw(&lex, end))
                        {
                            errskim(&lex, skim_end);
                            skip(&lex, end);
                        }
                    }
                }
                else
                {
                    lex = expression_assign(lex, funcsp, *tp, &tp1, &exp1, NULL, flags);
                }
                break;
            case asplus:
            case asminus:
            case asand:
            case asor:
            case asxor:
                lex = expression_assign(lex, funcsp, *tp, &tp1, &exp1, NULL, flags);
                break;
            default:
                lex = expression_assign(lex, funcsp, NULL, &tp1, &exp1, NULL, flags);
                break;
        }
        if (!tp1)
        {
            *tp = NULL;
            return lex;
        }
        if (cparams.prm_cplusplus && insertOperatorFunc(selovcl, kw,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
            // unallocated var for destructor
            if (asndest)
            {
                SYMBOL *sp = anonymousVar(sc_auto, tp1)->v.sp;
                callDestructor(sp, NULL, &asndest, NULL, TRUE, FALSE, FALSE);
                initInsert(&sp->dest, tp1, asndest, 0, TRUE);
            }
            
            continue;
        }
        if (kw == assign && (flags & _F_SELECTOR))
        {
            /* a little naive... */
            error(ERR_POSSIBLE_INCORRECT_ASSIGNMENT);
            flags &= ~_F_SELECTOR;
        }
        checkscope(*tp, tp1);
        if (cparams.prm_cplusplus)
        {
            if (isarithmetic(*tp))
            {
                castToArithmetic(FALSE, &tp1, &exp1, (enum e_kw)-1, *tp, TRUE);
            }
            else if (isstructured(tp1))
            {
                cppCast(*tp, &tp1, &exp1);
            }
        }
        exp2 = &exp1;
        while (castvalue(*exp2))
            exp2 = &(*exp2)->left;
        if ((*exp2)->type == en_func && (*exp2)->v.func->sp->storage_class == sc_overloads)
        {
            TYPE *tp2 = NULL;
            SYMBOL *funcsp;
            if ((*exp2)->v.func->sp->parentClass && !(*exp2)->v.func->asaddress)
                error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
            funcsp = MatchOverloadedFunction((*tp), isfuncptr(*tp) || basetype(*tp)->type == bt_memberptr ? & tp1 : &tp2, (*exp2)->v.func->sp, exp2, flags);
            if (funcsp && basetype(*tp)->type == bt_memberptr)
            {
                int lbl = dumpMemberPtr(funcsp, *tp, TRUE);
                exp1 = intNode(en_labcon, lbl);
            }
            if (funcsp)
            {
                tp1 = funcsp->tp;
                if (exp1->type == en_pc || exp1->type == en_func && !exp1->v.func->ascall)
                {
                    thunkForImportTable(&exp1);
                }
            }
            if (basetype(*tp)->btp && !comparetypes(basetype(*tp)->btp, tp1, TRUE))
            {
                if (!isvoidptr(*tp))
                    errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
            }
        }
        if (exp1->type == en_pc || exp1->type == en_func && !exp1->v.func->ascall)
        {
            if (chosenAssembler->msil)
            {
                ValidateMSILFuncPtr(*tp, tp1, &exp1);
            }
        }
        if (isconstraw(*tp, TRUE) && !localMutable)
            error(ERR_CANNOT_MODIFY_CONST_OBJECT);
        else if (isvoid(*tp) || isvoid(tp1) || (*tp)->type == bt_aggregate  || tp1->type == bt_aggregate)
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (!isstructured(*tp) && basetype(*tp)->type != bt_memberptr && basetype(*tp)->type != bt_templateparam && !lvalue(*exp) && (*exp)->type != en_msil_array_access)
            error(ERR_LVALUE);
        else switch(kw)
        {
            case asand:
            case asor:
            case asxor:
            case asleftshift:
            case asrightshift:
                if (ismsil(*tp) || ismsil(tp1))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (iscomplex(*tp) || iscomplex(tp1))
                    error(ERR_ILL_USE_OF_COMPLEX);
                if (isfloat(*tp) || isfloat(tp1) || isimaginary(*tp) || isimaginary(tp1))
                    error(ERR_ILL_USE_OF_FLOATING);
                /* fall through */					
            case astimes:
            case asdivide:
            case asmod:
                if (ismsil(*tp) || ismsil(tp1))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (ispointer(*tp) || ispointer(tp1))
                    error(ERR_ILL_POINTER_OPERATION);
                if (isstructured(*tp) || isstructured(tp1))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                break;
            case asplus:
                if ((*tp)->type == bt___string)
                {
                    if (exp1->type == en_labcon && exp1->string)
                        exp1->type == en_c_string;
                    else if (!ismsil(tp1))
                        exp1 = exprNode(en_x_object, exp1, NULL);
                }
                else if (ismsil(*tp) || ismsil(tp1))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (ispointer(*tp))
                {
                    if (ispointer(tp1))
                        error(ERR_ILL_POINTER_ADDITION);
                    else
                    {
                        EXPRESSION *ns;
                        if (iscomplex(tp1))
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (isfloat(tp1) || isimaginary(tp1))
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (isstructured(tp1))
                            error(ERR_ILL_STRUCTURE_OPERATION);
                        if (basetype(basetype(*tp)->btp)->type == bt_void)
                        {
                            if (cparams.prm_cplusplus)
                                error(ERR_ARITHMETIC_WITH_VOID_STAR);
                            cast(&stdcharptr, &exp1);
                            ns = nodeSizeof(&stdchar, exp1);
                        }
                        else
                        {
                            cast((*tp), &exp1);
                            ns = nodeSizeof(basetype(*tp)->btp, exp1);
                        }
                        exp1 = exprNode(en_umul, exp1, ns);
                    }
                }
                else if (ispointer(tp1))
                {
                    error(ERR_ILL_POINTER_OPERATION);
                }
                else if (isstructured(*tp) || isstructured(tp1))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                break;
            case asminus:
                if (ismsil(*tp) || ismsil(tp1))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (isstructured(*tp) || isstructured(tp1))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (ispointer(tp1))
                {
                    error(ERR_ILL_POINTER_SUBTRACTION);
                }
                else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if (ispointer(*tp))
                {
                    EXPRESSION *ns;
                    if (basetype(basetype(*tp)->btp)->type == bt_void)
                    {
                        if (cparams.prm_cplusplus)
                            error(ERR_ARITHMETIC_WITH_VOID_STAR);
                        cast(&stdcharptr, &exp1);
                        ns = nodeSizeof(&stdchar, exp1);
                    }
                    else
                    {
                        cast((*tp), &exp1);
                        ns = nodeSizeof(basetype(*tp)->btp, exp1);
                    }
                    exp1 = exprNode(en_umul, exp1, ns);
                }
                break;
            case assign:
                if (basetype(*tp)->type == bt___string)
                {
                    if (exp1->type == en_labcon && exp1->string)
                        exp1->type == en_c_string;
                }
                else if (basetype(*tp)->type == bt___object)
                {
                    if (tp1->type != bt___object)
                        exp1 = exprNode(en_x_object, exp1, NULL);
                }
                else if (ismsil(*tp) || ismsil(tp1))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (ispointer(*tp))
                {
                    if (isarray(tp1) && (tp1)->msil)
                        error(ERR_MANAGED_OBJECT_NO_ADDRESS);
                    else if (isarithmetic(tp1))
                    {
                        if (iscomplex(tp1))
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (isfloat(tp1) || isimaginary(tp1))
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (isarithmeticconst(exp1))
                        {
                            if (!isintconst(exp1) || !isconstzero(*tp, exp1))
                                error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                        else if (cparams.prm_cplusplus)
                        {
                            errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                        }
                        else
                        {
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                    }
                    else if (ispointer(tp1))
                    {
                        if (!comparetypes(*tp, tp1, TRUE))
                        {
                            BOOLEAN found = FALSE;
                            if (ispointer(tp1))
                            {
                                TYPE *tpo = basetype(basetype(tp1)->btp);
                                TYPE *tpn = basetype(basetype(*tp)->btp);
                                if (isstructured(tpo) && isstructured(tpn))
                                {
                                    if (classRefCount(tpo->sp, tpn->sp) == 1)
                                    {
                                        EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                                        v->type = en_c_i;
                                        v = baseClassOffset(tpo->sp, tpn->sp, v);
                                        optimize_for_constants(&v);
                                        if (v->type == en_c_i) // check for no virtual base
                                        {
                                            if (isAccessible(tpn->sp, tpn->sp, tpo->sp, funcsp, ac_public, FALSE))
                                            {
                                                *exp = exprNode(en_add, *exp, v);
                                                found = TRUE;
                                            }
                                        }
                                    }
                                    else if (classRefCount(tpn->sp, tpo->sp) == 1)
                                    {
                                        EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                                        v->type = en_c_i;
                                        v = baseClassOffset(tpn->sp, tpo->sp, v);
                                        optimize_for_constants(&v);
                                        if (v->type == en_c_i) // check for no virtual base
                                        {
                                            if (isAccessible(tpo->sp, tpo->sp, tpn->sp, funcsp, ac_public, FALSE))
                                            {
                                                *exp = exprNode(en_sub, *exp, v);
                                                found = TRUE;
                                            }
                                        }
                                    }
                                }
                            }
                            if (!found)
                            {
                                if (cparams.prm_cplusplus)
                                {
                                    if ((!isvoidptr(*tp) || !ispointer(tp1)) && !tp1->nullptrType)
                                        if (!isTemplatedPointer(*tp))
                                        {
                                            errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                                        }
                                }
                                else if (!isvoidptr(*tp) && !isvoidptr(tp1))
                                {
                                    if (!matchingCharTypes(*tp, tp1))
                                        error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                                }
                                else if (cparams.prm_cplusplus && !isvoidptr(*tp) && isvoidptr(tp1) && exp1->type != en_nullptr)
                                {
                                    error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                                }
                            }
                        }
                    }
                    else if (isfunction(tp1))
                    {
                        if (!isvoidptr(*tp) && 
                            (!isfunction(basetype(*tp)->btp) || !comparetypes(basetype(*tp)->btp, tp1, TRUE)))
                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                    }
                    else 
                    {
                        if (tp1->type == bt_memberptr)
                        {
                            errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                        }
                        else
                            error(ERR_INVALID_POINTER_CONVERSION);
                    }
                }
                else if (ispointer(tp1))
                {
                    if (iscomplex(*tp))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (isfloat(*tp) || isimaginary(*tp))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (isint(*tp))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else if (isarray(tp1) && (tp1)->msil)
                        error(ERR_MANAGED_OBJECT_NO_ADDRESS);
                }
                if (isstructured(*tp) && (!isstructured(tp1) || !comparetypes(*tp, tp1, TRUE)))
                    error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                else if (isstructured(*tp) && !(*tp)->size)
                {
                    if (!(flags & _F_SIZEOF))
                        errorsym(ERR_STRUCT_NOT_DEFINED, basetype(*tp)->sp);
                }
                else if (!isstructured(*tp) && isstructured(tp1))
                {
                    error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                }
                else if (basetype(*tp)->type == bt_memberptr)
                {
                    if (exp1->type == en_memberptr)
                    {
                        if (exp1->v.sp != basetype(*tp)->sp 
                            && exp1->v.sp != basetype(*tp)->sp->mainsym 
                            && !sameTemplate(exp1->v.sp, basetype(*tp)->sp))
                                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);

                    }
                    else if ((!isfunction(basetype(*tp)->btp) || !comparetypes(basetype(*tp)->btp, tp1, TRUE)) && !isconstzero(tp1, *exp) && !comparetypes(*tp, tp1, TRUE))
                    {
                        errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                    }
                }
                else if (basetype(tp1)->type == bt_memberptr)
                    errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                break;
            default:
                done = TRUE;
                break;
        }
        switch(kw)
        {
            case asand:
                op = en_and;
                break;
            case asor:
                op = en_or;
                break;
            case asxor:
                op = en_xor;
                break;

            case astimes:
                op = en_mul;
                break;
            case asdivide:
                tp2 = destSize(*tp, tp1, NULL, NULL, FALSE, NULL);
                op = isunsigned(*tp) ? en_udiv : en_div;
                break;
            case asmod:
                tp2 = destSize(*tp, tp1, NULL, NULL, FALSE, NULL);
                op = isunsigned(*tp) ? en_umod : en_mod;
                break;
            case assign:
                op = en_assign;
                break;

            case asplus:
                op = en_add;
                break;
            case asminus:
                op = en_sub;
                break;

            case asleftshift:
                op = en_lsh;
                break;
            case asrightshift:
                tp2 = destSize(*tp, tp1, NULL, NULL, FALSE, NULL);
                op = isunsigned(*tp) ? en_ursh : en_rsh;
                break;
            default:
                break;
        }
        if (ismsil(*tp))
        {
            if (exp1->type == en_labcon && exp1->string)
                exp1->type = en_c_string;
            else if (!ismsil(tp1))
                exp1 = exprNode(en_x_object, exp1, NULL);
            if (op == en_assign)
            {
                *exp = exprNode(op, *exp, exp1);
            }
            else
            {
                EXPRESSION *dest = *exp;
                *exp = exprNode(op, *exp, exp1);
                *exp = exprNode(en_assign, dest, *exp);
            }
        }
        else if (basetype(*tp)->type == bt_memberptr)
        {
            if ((*exp)->type == en_not_lvalue || (*exp)->type == en_func && !(*exp)->v.func->ascall
                || (*exp)->type == en_void || (*exp)->type == en_memberptr)
            {
                if (basetype(*tp)->type != bt_templateparam)
                    error(ERR_LVALUE);
            }
            else if (exp1->type == en_memberptr)
            {
                int lbl = dumpMemberPtr(exp1->v.sp, *tp, TRUE);
                exp1 = intNode(en_labcon, lbl);
                *exp = exprNode(en_blockassign, *exp, exp1);
                (*exp)->size = (*tp)->size;
            }
            else if (isconstzero(tp1, exp1) || exp1->type == en_nullptr)
            {
                *exp = exprNode(en_blockclear, *exp, NULL);
                (*exp)->size = (*tp)->size;
            }
            else if (exp1->type == en_func && exp1->v.func->returnSP)
            {
                exp1->v.func->returnSP->allocate = FALSE;
                exp1->v.func->returnEXP = *exp;
                *exp = exp1;
            }
            else
            {
                *exp = exprNode(en_blockassign, *exp, exp1);
                (*exp)->size = (*tp)->size;
            }
        }
        else if (isstructured(*tp))
        {
            EXPRESSION *exp2 = exp1;
            if (((*exp)->type == en_not_lvalue || (*exp)->type == en_func
                || (*exp)->type == en_void) && !(flags & _F_SIZEOF))
                error(ERR_LVALUE);
            if (lvalue(exp2))
                exp2 = exp2->left;
            if (exp2->type == en_func && exp2->v.func->returnSP)
            {
                exp2->v.func->returnSP->allocate = FALSE;
                exp2->v.func->returnEXP = *exp;
                *exp = exp1;
            }
            else
            {
                *exp = exprNode(en_blockassign, *exp, exp1);
                (*exp)->size = (*tp)->size;
                if (isatomic(*tp) || isatomic(tp1))
                    (*exp)->size -= ATOMIC_FLAG_SPACE;
            }
            *exp = exprNode(en_not_lvalue, *exp, NULL);
        }
        else
        {
            if (kw == assign)
            {
                if ((*exp)->type != en_msil_array_access)
                {
                    int n = natural_size(*exp);
                    if (natural_size(exp1) != n)
                        cast((*tp), &exp1);
                }

                *exp = exprNode(op, *exp, exp1);
            }
            else {
                EXPRESSION *dest = *exp;
                *exp = RemoveAutoIncDec(*exp);
                // we want to optimize the as* operations for the backend
                // but can't do the optimization for divisions
                // otherwise it is fine for the processor we are on
                if (kw == asmod || kw == asdivide)
                {
                    int n = natural_size(*exp);
                    destSize(*tp, tp1, exp, &exp1, FALSE, NULL);
                    *exp = exprNode(op, *exp, exp1);
                    if (natural_size(exp1) != n)
                        cast((*tp), exp);
                    *exp = exprNode(en_assign, dest, *exp);
                }
                else if (kw == asleftshift || kw == asrightshift)
                {
                    int n = natural_size(*exp);
                    if (natural_size(exp1) != n)
                        cast(&stdint, &exp1);
                    *exp = exprNode(op, *exp, exp1);
                    *exp = exprNode(en_assign, dest, *exp);
                }
                else
                {
                    int n = natural_size(*exp);
                    if (natural_size(exp1) != n)
                        destSize(*tp, tp1, exp, &exp1, FALSE, NULL);
                    *exp = exprNode(op, *exp, exp1);
                    if (natural_size(*exp) != n)
                        cast(*tp, exp);
                    *exp = exprNode(en_assign, dest, *exp);
                }
            }
        }
    }
    if ((*exp)->type == en_pc || (*exp)->type == en_func && !(*exp)->v.func->ascall)
        thunkForImportTable(exp);
    return lex;
}
static LEXEME *expression_comma(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    lex = expression_assign(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == NULL)
        return lex;
    while (MATCHKW(lex, comma))
    {
        EXPRESSION *exp1=NULL;
        TYPE *tp1 = NULL;
        lex = getsym();
        lex = expression_assign(lex, funcsp, atp, &tp1, &exp1, NULL, flags);
        if (!tp1)
        {
            break;
        }
        if (cparams.prm_cplusplus && insertOperatorFunc(ovcl_comma, comma,
                               funcsp, tp, exp, tp1, exp1, NULL, flags))
        {
            continue; 
        }
        else
        {
            *tp = tp1;
            *exp = exprNode(en_void, *exp, exp1);
            exp = &(*exp)->right;
        }
    }
    return lex;
}
LEXEME *expression_no_comma(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, BOOLEAN *ismutable, int flags)
{
    lex = expression_assign(lex, funcsp, atp, tp, exp, ismutable, flags);
    assignmentUsages(*exp, FALSE);
    return lex;
}
LEXEME *expression_no_check(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, 
                   int flags)
{
    if (flags & _F_TYPETEST)
        anonymousNotAlloc++;
    lex = expression_comma(lex, funcsp, atp, tp, exp, NULL, flags);
    if (flags & _F_TYPETEST)
        anonymousNotAlloc--;
    return lex;
}

LEXEME *expression(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, 
                   int flags)
{
    lex = expression_comma(lex, funcsp, atp, tp, exp, NULL, flags);
    assignmentUsages(*exp, FALSE);
    return lex;
}
