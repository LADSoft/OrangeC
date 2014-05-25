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

extern int currentErrorLine;
extern LIST *externals;
extern NAMESPACEVALUES *globalNameSpace;
extern TYPE stdany;
extern STRUCTSYM *structSyms;
extern LIST *deferred;
extern int structLevel;
extern LIST *nameSpaceList;

int dontRegisterTemplate;
int instantiatingTemplate;
int inTemplateBody;
int templateNestingCount =0 ;
int templateHeaderCount ;
BOOLEAN inTemplateType;

static int inTemplateArgs;

struct templateListData *currents;

static LEXEME *TemplateArg(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST *arg, TEMPLATEPARAMLIST **lst);

void templateInit(void)
{
    inTemplateBody = FALSE;
    templateNestingCount = 0;
    templateHeaderCount = 0;
    instantiatingTemplate = 0;
    currents = NULL;
    inTemplateArgs = 0;
    inTemplateType = FALSE;
    dontRegisterTemplate = 0;
}
int templatecomparetypes(TYPE *tp1, TYPE *tp2, BOOLEAN exact)
{
    if (!tp1 || !tp2)
        return FALSE;
    if (!comparetypes(tp1, tp2, exact))
        return FALSE;
    if (basetype(tp1)->type != basetype(tp2)->type)
        return FALSE;
    return TRUE;
}
void TemplateGetDeferred(SYMBOL *sym)
{
    if (currents)
    {
        sym->deferredTemplateHeader = currents->head;
        if (currents->bodyHead)
            sym->deferredCompile = currents->bodyHead;
    }
}
TEMPLATEPARAMLIST *TemplateGetParams(SYMBOL *sym)
{
    TEMPLATEPARAMLIST *params = NULL;
    if (currents)
    {
        int n = -1;
        params = (TEMPLATEPARAMLIST *)currents->args;
        while (sym)
        {
            if (sym->templateLevel && !sym->instantiated)
                n++;
            sym = sym->parentClass;
        }
        if (n > 0 && params)
            while (n-- && params->p->bySpecialization.next)
            {
                params = params->p->bySpecialization.next;
            }
    }
    if (!params)
    {
        params = Alloc(sizeof(TEMPLATEPARAMLIST));
        params->p = Alloc(sizeof(TEMPLATEPARAM));
    }
    return params;    
}
void TemplateRegisterDeferred(LEXEME *lex)
{
    if (lex && templateNestingCount && !dontRegisterTemplate)
    {
        if (!lex->registered)
        {
            LEXEME *cur = globalAlloc(sizeof(LEXEME));
            if (lex->type == l_id)
                lex->value.s.a = litlate(lex->value.s.a);
            *cur = *lex;
            cur->next = NULL;
            if (inTemplateBody)
            {
                if (currents->bodyHead)
                {
                    cur->prev = currents->bodyTail;
                    currents->bodyTail = currents->bodyTail->next = cur;
                }
                else
                {
                    cur->prev = NULL;
                    currents->bodyHead = currents->bodyTail = cur;
                }
            }
            else
            {
                if (currents->head)
                {
                    cur->prev = currents->tail;
                    currents->tail = currents->tail->next = cur;
                }
                else
                {
                    cur->prev = NULL;
                    currents->head = currents->tail = cur;
                }
            }
            lex->registered = TRUE;
        }
    }
}
BOOLEAN exactMatchOnTemplateParams(TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym)
{
    while (old && sym)
    {
        if (old->p->type != sym->p->type)
            break;
        if (old->p->type == kw_template)
        {
            if (!exactMatchOnTemplateParams(old->p->byTemplate.args, sym->p->byTemplate.args))
                break;
        }
        else if (old->p->type == kw_int)
        {
            if (!templatecomparetypes(old->p->byNonType.tp, sym->p->byNonType.tp, TRUE))
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    return !(old || sym);
}
BOOLEAN exactMatchOnTemplateArgs(TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym)
{
    while (old && sym)
    {
        if (old->p->type != sym->p->type)
            return FALSE;
        switch (old->p->type)
        {
            case kw_typename:
                if (!templatecomparetypes(old->p->byClass.dflt, sym->p->byClass.dflt, TRUE))
                    return FALSE;
                if (!templatecomparetypes(sym->p->byClass.dflt, old->p->byClass.dflt, TRUE))
                    return FALSE;
                break;
            case kw_template:
                if (old->p->byTemplate.dflt != sym->p->byTemplate.dflt)
                    return FALSE;
                break;
            case kw_int:
                if (!templatecomparetypes(old->p->byNonType.tp, sym->p->byNonType.tp, TRUE))
                    return FALSE;
#ifndef PARSER_ONLY
                if (!equalnode(old->p->byNonType.dflt, sym->p->byNonType.dflt))
                    return FALSE;
#endif
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    return !old && !sym;
}
static TEMPLATEPARAMLIST * mergeTemplateDefaults(TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym, BOOLEAN definition)
{
    TEMPLATEPARAMLIST *rv = sym;
#ifndef PARSER_ONLY
    while (old && sym)
    {
        if (!definition && old->p->sym)
        {
            sym->p->sym = old->p->sym;
            sym->p->sym->tp->templateParam = sym;
        }
        switch (sym->p->type)
        {
            case kw_template:
                sym->p->byTemplate.args = mergeTemplateDefaults(old->p->byTemplate.args, sym->p->byTemplate.args, definition);
                if (old->p->byTemplate.txtdflt && sym->p->byTemplate.txtdflt)
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->p->sym);
                else if (!sym->p->byTemplate.txtdflt)
                    sym->p->byTemplate.txtdflt = old->p->byTemplate.txtdflt;
                break;
            case kw_typename:
                if (old->p->byClass.txtdflt && sym->p->byClass.txtdflt)
                {
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->p->sym);
                }
                else if (!sym->p->byClass.txtdflt)
                    sym->p->byClass.txtdflt = old->p->byClass.txtdflt;
                break;
            case kw_int:
                if (old->p->byNonType.txtdflt && sym->p->byNonType.txtdflt)
                {
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->p->sym);
                }
                else if (!sym->p->byNonType.txtdflt)
                {
                    sym->p->byNonType.txtdflt = old->p->byNonType.txtdflt;
                }
                break;
            case kw_new: // specialization
                break;
        }
        old = old->next;
        sym = sym->next;
    }
#endif
    return rv;
}
static void checkTemplateDefaults(TEMPLATEPARAMLIST *args)
{
    SYMBOL *last = NULL;
    while (args)
    {
        void *txtdflt = NULL;
        switch (args->p->type)
        {
            case kw_template:
                checkTemplateDefaults(args->p->byTemplate.args);
                txtdflt = args->p->byTemplate.txtdflt;
                break;
            case kw_typename:
                txtdflt = args->p->byClass.txtdflt;
                break;
            case kw_int:
                txtdflt = args->p->byNonType.txtdflt;
                break;
        }
        if (last && !txtdflt)
        {
            errorsym(ERR_MISSING_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, last);
            break;
        }
        if (txtdflt)
            last = args->p->sym;
        args = args->next;
    }
}
static BOOLEAN matchTemplateSpecializationToParams(TEMPLATEPARAMLIST *param, TEMPLATEPARAMLIST *special, SYMBOL *sp)
{
    while (param && special)
    {
        if (param->p->type != special->p->type)
        {
            errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else if (param->p->type == kw_template)
        {
            if (!exactMatchOnTemplateParams(param->p->byTemplate.args, special->p->byTemplate.dflt->templateParams->next))
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else if (param->p->type == kw_int)
        {
            if (!templatecomparetypes(param->p->byNonType.tp, special->p->byNonType.tp, TRUE) && (!ispointer(param->p->byNonType.tp) || !isconstzero(param->p->byNonType.tp, special->p->byNonType.dflt)))
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        param = param->next;
        special = special->next;
    }
    if (param)
    {
        errorsym(ERR_TOO_FEW_ARGS_PASSED_TO_TEMPLATE, sp);
    }
    else if (special)
    {
        errorsym(ERR_TOO_MANY_ARGS_PASSED_TO_TEMPLATE, sp);
    }
    return !param && !special;
}
static void checkMultipleArgs(TEMPLATEPARAMLIST *sym)
{
    while (sym)
    {
        TEMPLATEPARAMLIST *next = sym->next;
        while (next)
        {
            if (next->p->sym && !strcmp(sym->p->sym->name, next->p->sym->name))
            {
                currentErrorLine = 0;
                errorsym(ERR_DUPLICATE_IDENTIFIER, sym->p->sym);
            }
            next = next->next;
        }
        if (sym->p->type == kw_template)
        {
            checkMultipleArgs(sym->p->byTemplate.args);
        }
        sym = sym->next;
    }
}
TEMPLATEPARAMLIST * TemplateMatching(LEXEME *lex, TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym, SYMBOL *sp, BOOLEAN definition)
{
    TEMPLATEPARAMLIST *rv = NULL;
    currents->sym = sp;
    if (old)
    {
        if (sym->p->bySpecialization.types)
        {
            matchTemplateSpecializationToParams(old->next, sym->p->bySpecialization.types, sp);
            rv = sym;
        }
        else if (!exactMatchOnTemplateParams(old->next, sym->next))
        {
            error(ERR_TEMPLATE_DEFINITION_MISMATCH);
        }
        else 
        {
            rv = mergeTemplateDefaults(old, sym, definition);
            checkTemplateDefaults(rv);
        }
    }
    else
    {
        checkTemplateDefaults(sym->next);
    }
    checkMultipleArgs(sym->next);
    return rv;
}
BOOLEAN typeHasTemplateArg(TYPE *t)
{
    while (ispointer(t) || isref(t))
        t = t->btp;
    if (isfunction(t))
    {
        HASHREC *hr;
        t = basetype(t);
        if (typeHasTemplateArg(t->btp))
            return TRUE;
        hr = t->syms->table[0];
        while (hr)
        {
            if (typeHasTemplateArg(((SYMBOL *)hr->p)->tp))
                return TRUE;
            hr = hr->next;
        }            
    }
    else if (basetype(t)->type == bt_templateparam)
        return TRUE;
    return FALSE;
}
void TemplateValidateSpecialization(TEMPLATEPARAMLIST *arg)
{
    TEMPLATEPARAMLIST *t = arg->p->bySpecialization.types;
    while (t)
    {
        if (t->p->type == kw_typename && typeHasTemplateArg((TYPE *)t->p->byClass.dflt))
            break;
        t = t->next;
    }
    if (!t)
    {
        error (ERR_PARTIAL_SPECIALIZATION_MISSING_TEMPLATE_PARAMETERS);
    }
}
static LEXEME *templateName(LEXEME *lex, SYMBOL **sym)
{
    if (!lex)
        return lex;
    if (lex->type == l_id || MATCHKW(lex, classsel))
    {
        SYMBOL *sp;
        LEXEME *placeholder = lex;
        lex = nestedSearch(lex, &sp, NULL, NULL, NULL, NULL, FALSE, sc_global);
        if (sp && sp->templateLevel)
        {
            lex = getsym();
            if (!MATCHKW(lex,lt))
                *sym = sp;
            else
                lex = prevsym(placeholder);
        }
        else
        {
            lex = prevsym(placeholder);
        }
    }
    return lex;
    
}
void getPackedArgs(TEMPLATEPARAMLIST **packs, int *count , TEMPLATEPARAMLIST *args)
{
    while (args)
    {
        if (args->p->type == kw_typename)
        {
            if (args->p->packed)
            {
                packs[(*count)++] = args;
            }
            else
            {
                TYPE *tp = args->p->byClass.dflt;
                if (tp)
                {
                    if (tp->type == bt_templateparam && tp->templateParam->p->packed)
                    {
                        packs[(*count)++] = tp->templateParam;
                    }
                }
            }
        }
        else if (args->p->type == kw_delete)
        {
            getPackedArgs(packs, count, args->p->byDeferred.args);
        }
        args = args->next;
    }
}
TEMPLATEPARAMLIST *expandPackedArg(TEMPLATEPARAMLIST *select, int index)
{
    int i;
    if (select->p->packed)
    {
        TEMPLATEPARAMLIST *rv = Alloc(sizeof(TEMPLATEPARAMLIST));
        select = select->p->byPack.pack;
        for (i=0; i < index && select; i++)
            select = select->next;
        if (select)
        {
            rv->p = Alloc(sizeof(TEMPLATEPARAM));
            *(rv->p) = *(select->p);
            rv->p->byClass.dflt = rv->p->byClass.val;
            rv->p->byClass.val = NULL;
            rv->next = NULL;
            return rv;
        }
        return select;
    }
    else if (select->p->type == kw_delete)
    {
        SYMBOL *sym = select->p->sym;
        TEMPLATEPARAMLIST *args = select->p->byDeferred.args;
        TEMPLATEPARAMLIST *val = NULL, **lst = &val;
        while (args)
        {
            TEMPLATEPARAMLIST *templateParam = expandPackedArg(args, index);
            *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
            (*lst)->p = templateParam->p;
            lst = &(*lst)->next;            
            args = args->next;
        }
        sym = TemplateClassInstantiateInternal(sym, val, TRUE);
        val = Alloc(sizeof(TEMPLATEPARAMLIST));
        val->p = Alloc(sizeof(TEMPLATEPARAM));
        val->p->type = kw_typename;
        val->p->byClass.dflt = sym->tp;
        return val;
    }
    else
    {
        TEMPLATEPARAMLIST *rv = Alloc(sizeof(TEMPLATEPARAMLIST));
        rv->p = select->p;
        return rv;
    }
}
TEMPLATEPARAMLIST **expandArgs(TEMPLATEPARAMLIST **lst, TEMPLATEPARAMLIST *select, BOOLEAN packable)
{
    TEMPLATEPARAMLIST *packs[500];
    int count = 0;
    int n,i;
    getPackedArgs(packs, &count, select);
    if (packable)
    {
        if (!count)
        {
            error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
        }
        
    }
    else if (count)
    {
        error(ERR_PACKED_TEMPLATE_PARAMETER_NOT_ALLOWED_HERE);
    }
    for (i=0, n=-1; i < count; i++)
    {
        int j;
        TEMPLATEPARAMLIST *pack = packs[i]->p->byPack.pack;
        for (j=0; pack; j++, pack = pack->next) ;
        if (n == -1)
        {
            n = j;
        }
        else if (n != j)
        {
            if (!templateNestingCount)
                error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
            if (j < n)
                n = j;
        }
    }
    if (n <= 0)
    {
        *lst = expandPackedArg(select, 0);
        lst = &(*lst)->next;
    }
    else
    {
        for (i=0; i < n; i++)
        {
            *lst = expandPackedArg(select, i);
            lst = &(*lst)->next;
        }
    }
    return lst;
}
LEXEME *GetTemplateArguments(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST **lst)
{
    TEMPLATEPARAMLIST **start = lst;
    // entered with lex set to the opening <
    inTemplateArgs++;
    lex = getsym();
    if (!MATCHKW(lex, rightshift) && !MATCHKW(lex, gt))
    {
        do
        {
            TYPE *tp = NULL;
            if (startOfType(lex, TRUE))
            {        
                SYMBOL *sym = NULL;
                lex = templateName(lex, &sym);
                if (sym)
                {
                    *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                    (*lst)->p->type = kw_template;
                    (*lst)->p->byTemplate.dflt = sym;
                    lst = &(*lst)->next;
                }
                else
                {
                    lex = get_type_id(lex, &tp, funcsp, FALSE);
                    if (MATCHKW(lex, ellipse))
                    {
                        lex = getsym();
                        lst = expandArgs(lst, tp->templateParam, TRUE);
                    }
                    else if (inTemplateArgs == 1 && tp->type == bt_templateparam)
                    {
                        lst = expandArgs(lst, tp->templateParam, FALSE);
                    }
                    else
                    {
                        *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                        (*lst)->p->type = kw_typename;
                        (*lst)->p->byClass.dflt = tp;
                        lst = &(*lst)->next;   
                    }
                }
            }
            else
            {
                EXPRESSION *exp;
                TYPE *tp;
    join:
                exp = NULL;
                tp = NULL;
                lex = expression_no_comma(lex, funcsp, NULL, &tp, &exp, NULL, _F_NOINLINE | _F_INTEMPLATEPARAMS);
                optimize_for_constants(&exp);
                if (!tp)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                    
                }
                else
                {
                    *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                    (*lst)->p->type = kw_int;
                    (*lst)->p->byNonType.dflt = exp;
                    (*lst)->p->byNonType.tp = tp;
                    lst = &(*lst)->next;
                    if (exp && tp->type !=bt_templateparam && !isarithmeticconst(exp))
                    {
                        while (castvalue(exp) || lvalue(exp))
                            exp = exp->left;
                        switch (exp->type)
                        {
                            case en_pc:
                            case en_global:
                            case en_label:
                            case en_func:
                                break;
                            default:
                                error(ERR_TEMPLATE_ARGUMENT_MUST_BE_CONSTANT);
                                break;
                        }
                    }
                }
            }
            if (MATCHKW(lex, comma))
                lex = getsym();
            else
                break;
        } while (TRUE);
    }
    if (MATCHKW(lex, rightshift))
        lex = getGTSym(lex);
    else
        needkw(&lex, gt);
    inTemplateArgs--;
    return lex;
}
SYMBOL *LookupSpecialization(SYMBOL *sym, TEMPLATEPARAMLIST *templateParams)
{
    SYMBOL *candidate;
    LIST *lst = sym->specializations;
    while (lst)
    {
        candidate = (SYMBOL *)lst->data;
        if (candidate->templateParams && exactMatchOnTemplateArgs(templateParams->p->bySpecialization.types, candidate->templateParams->p->bySpecialization.types))
            return candidate;
        lst = lst->next;
    }
    candidate = clonesym(sym);
    candidate->tp = (TYPE *)Alloc(sizeof(TYPE));
    *candidate->tp = *sym->tp;
    candidate->tp->sp = candidate;
    candidate->templateParams = templateParams;
    lst = Alloc(sizeof(LIST));
    lst->data = candidate;
    lst->next = sym->specializations;
    sym->specializations = lst;
    candidate->overloadName = sym->overloadName;
    candidate->specialized = TRUE;
    if (!candidate->parentTemplate)
        candidate->parentTemplate = sym;
    candidate->baseClasses = NULL;
    candidate->vbaseEntries = NULL;
    candidate->vtabEntries = NULL;
    return candidate;
}
static BOOLEAN matchTemplatedType(TYPE *old, TYPE *sym, BOOLEAN strict)
{
    while (1)
    {
        if (isconst(old) == isconst(sym) && isvolatile(old) == isvolatile(sym))
        {
            old = basetype(old);
            sym = basetype(sym);
            if (old->type == sym->type || isfunction(old) && isfunction(sym))
            {
                switch (old->type)
                {
                    case bt_struct:
                    case bt_class:
                    case bt_union:                
                        return old->sp == sym->sp;
                    case bt_func:
                    case bt_ifunc:
                        if (!matchTemplatedType(old->btp, sym->btp, strict))
                            return FALSE;
                        {
                            HASHREC *hro = old->syms->table[0];
                            HASHREC *hrs = sym->syms->table[0];
                            if (((SYMBOL *)hro->p)->thisPtr)
                                hro = hro->next;
                            if (((SYMBOL *)hrs->p)->thisPtr)
                                hrs = hrs->next;
                            while (hro && hrs)
                            {
                                if (!matchTemplatedType(((SYMBOL *)hro->p)->tp, ((SYMBOL *)hrs->p)->tp, strict))
                                    return FALSE;
                                hro = hro->next;
                                hrs = hrs->next;
                            }
                            return !hro && !hrs;
                        }
                    case bt_pointer:
                    case bt_lref:
                    case bt_rref:
                        if (old->array == sym->array && old->size == sym->size)
                        {
                            old = old->btp;
                            sym = sym->btp;
                            break;
                        }
                        return FALSE;
                    case bt_templateparam:
                        return old->templateParam->p->type == sym->templateParam->p->type;
                    default:
                        return TRUE;
                }
            }
            else
            {
                return !strict && old->type == bt_templateparam;
            }
        }
        else
        {
            return FALSE;
        }
    }
}
SYMBOL *SynthesizeResult(SYMBOL *sym, TEMPLATEPARAMLIST *params);
static BOOLEAN TemplateParseDefaultArgs(TEMPLATEPARAMLIST *dest, TEMPLATEPARAMLIST *src, TEMPLATEPARAMLIST *enclosing);
static BOOLEAN ValidateArgsSpecified(TEMPLATEPARAMLIST *params, SYMBOL *func, INITLIST *args);
SYMBOL *LookupFunctionSpecialization(SYMBOL *overloads, SYMBOL *sp, TEMPLATEPARAMLIST *templateParams)
{
    HASHREC *hr = overloads->tp->syms->table[0];
    TEMPLATEPARAMLIST *params = templateParams->next;
    if (params)
    {
        while (hr)
        {
            SYMBOL *hrs = (SYMBOL *)hr->p;
            if (hrs->templateLevel == sp->templateLevel)
            {
                if (hrs->templateParams)
                {
                    if (exactMatchOnTemplateParams(hrs->templateParams->next, templateParams->next) && matchTemplatedType(hrs->tp, sp->tp, TRUE))
                            return hrs;
                    else 
                        if (matchOverload(hrs, sp) && comparetypes(basetype(sp->tp)->btp, basetype(hrs->tp)->btp, TRUE))
                            return hrs;
                }
            }
            else {
                if (matchOverload(sp, hrs) && comparetypes(basetype(sp->tp)->btp, basetype(hrs->tp)->btp, TRUE))
                    return hrs;
            }
            hr = hr->next;
        }
        if (TemplateParseDefaultArgs(params,params,params) && ValidateArgsSpecified(params, sp, NULL))
        {
            SYMBOL * deduced = SynthesizeResult(sp, templateParams);
            deduced->overloadName = overloads;
            deduced = TemplateFunctionInstantiate(deduced, FALSE, TRUE);
            return deduced;
        }
    }
    return NULL;
}
LEXEME *TemplateArgGetDefault(LEXEME **lex)
{
    LEXEME *rv = NULL, **cur = &rv;
    int inbracket = 0;
    while (*lex != NULL)
    {
        if (MATCHKW(*lex, comma))
            break;
        if (MATCHKW(*lex, gt) || MATCHKW(*lex, rightshift))
            if (!inbracket)
                break;
            else
                inbracket--;
            
        if (MATCHKW(*lex, lt))
            inbracket++;
        *cur = Alloc(sizeof(LEXEME));
        **cur = **lex;
        if (ISID(*lex))
            (*cur)->value.s.a = litlate((*cur)->value.s.a);
        cur = &(*cur)->next;
        if (MATCHKW(*lex, rightshift))
            *lex = getGTSym(*lex);
        else
            *lex = getsym();
    }
    return rv;
}
static LEXEME *TemplateHeader(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST **args)
{
    TEMPLATEPARAMLIST **lst = args;
    if (needkw(&lex, lt))
    {
        while (1)
        {
            if (MATCHKW(lex, gt) || MATCHKW(lex, rightshift))
               break;
            *args = Alloc(sizeof(TEMPLATEPARAMLIST));
            (*args)->p = Alloc(sizeof(TEMPLATEPARAM));
            lex = TemplateArg(lex, funcsp, *args, lst);
            if (*args)
                args = &(*args)->next;
            if (!MATCHKW(lex, comma))
                break;
            lex = getsym();
        }
        if (MATCHKW(lex, rightshift))
            lex = getGTSym(lex);
        else
            needkw(&lex, gt);
    }
    return lex;
}
static LEXEME *TemplateArg(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST *arg, TEMPLATEPARAMLIST **lst)
{
    switch (KW(lex))
    {
        TYPE *tp, *tp1;
        EXPRESSION *exp1;
        SYMBOL *sp;
        case kw_class:
        case kw_typename:
            arg->p->type = kw_typename;
            arg->p->packed = FALSE;
            lex = getsym();
            if (MATCHKW(lex, ellipse))
            {
                arg->p->packed = TRUE;
                lex = getsym();
            }
            if (ISID(lex) || MATCHKW(lex, classsel))
            {
                SYMBOL *sym = NULL, *strsym = NULL;
                NAMESPACEVALUES *nsv = NULL;
                BOOLEAN qualified = MATCHKW(lex, classsel);
                
                lex = nestedSearch(lex, &sym, &strsym, &nsv, NULL, NULL, FALSE, sc_global);
                if (qualified || strsym || nsv)
                {
                    lex = getsym();
                    if (sym && istype(sym))
                    {
                        if (ISID(lex))
                        {
                            tp = sym->tp;
                            sp = makeID(sc_templateparam, tp, NULL, litlate(lex->value.s.a));
                            lex = getsym();
                            goto non_type_join;
                        }
                        else
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                            break;
                        }
                    }
                    else
                    {
                        error(ERR_TYPE_NAME_EXPECTED);
                        break;
                    }
                }
                else
                {
                    TYPE *tp = Alloc(sizeof(TYPE));
                    tp->type = bt_templateparam;
                    tp->templateParam = arg;
                    arg->p->sym = makeID(sc_templateparam, tp, NULL, litlate(lex->value.s.a));
                    lex = getsym();
                }
            }
            else
            {
                TYPE *tp = Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->templateParam = arg;
                arg->p->sym = makeID(sc_templateparam, tp, NULL, "-tpl-");
            }
            if (MATCHKW(lex, assign))
            {
                if (arg->p->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                lex = getsym();
                arg->p->byClass.txtdflt = TemplateArgGetDefault(&lex);
                if (!arg->p->byClass.txtdflt)
                {
                    error(ERR_CLASS_TEMPLATE_DEFAULT_MUST_REFER_TO_TYPE);
                }
            }
            if (!MATCHKW(lex, gt) && !MATCHKW(lex, leftshift) && !MATCHKW(lex, comma))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        case kw_template:
            arg->p->type = kw_template;
            lex = getsym();
            lex = TemplateHeader(lex, funcsp, &arg->p->byTemplate.args);
            arg->p->packed = FALSE;
            if (!MATCHKW(lex, kw_class))
            {
                error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MUST_NAME_CLASS);
            }       
            else
            {
                lex = getsym();
            }
            if (MATCHKW(lex, ellipse))
            {
                arg->p->packed = TRUE;
                lex = getsym();
            }
            if (ISID(lex))
            {
                TYPE *tp = Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->templateParam = arg;
                arg->p->sym = makeID(sc_templateparam, tp, NULL, litlate(lex->value.s.a));
                lex = getsym();
            }
            else
            {
                TYPE *tp = Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->templateParam = arg;
                arg->p->sym = makeID(sc_templateparam, tp, NULL, "-tpl-");
            }
            if (MATCHKW(lex, assign))
            {
                if (arg->p->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                arg->p->byTemplate.txtdflt = TemplateArgGetDefault(&lex);
                if (!arg->p->byTemplate.txtdflt)
                {
                    error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MISSING_DEFAULT);
                }
            }
            if (!MATCHKW(lex, gt) && !MATCHKW(lex, leftshift) && !MATCHKW(lex, comma))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        default: // non-type
        {
            enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            BOOLEAN defd = FALSE;
            BOOLEAN notype = FALSE;
            arg->p->type = kw_int;
            arg->p->packed = FALSE;
            tp = NULL;
            sp = NULL;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
            lex = getBasicType(lex, funcsp, &tp, NULL, FALSE, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, FALSE);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
            lex = getBeforeType(lex, funcsp, &tp, &sp, NULL, NULL, FALSE, sc_cast, &linkage, &linkage2, &linkage3, FALSE, FALSE, FALSE); /* fixme at file scope init */
            sizeQualifiers(tp);
            if (!tp || notype)
            {
                if (sp && (*lst)->p->sym)
                {
                    while (*lst)
                    {
                        if (!(*lst)->p->sym)
                            break;
                        if (!strcmp((*lst)->p->sym->name, sp->name))
                        {
                            tp = (*lst)->p->sym->tp;
                            if (ISID(lex))
                            {
                                sp = makeID(funcsp? sc_auto : sc_global, tp, NULL, litlate(lex->value.s.a));
                                lex = getsym();
                            }
                            else
                            {
                                sp = makeID(funcsp? sc_auto : sc_global, tp, NULL, AnonymousName());
                            }
                            goto non_type_join;
                        }
                        lst = &(*lst)->next;
                    }
                }
                 error(ERR_INVALID_TEMPLATE_PARAMETER);
            }
            else if (!sp)
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else
            {
                TYPE *tpa;
non_type_join:
                tpa = Alloc(sizeof(TYPE));
                tpa->type = bt_templateparam;
                tpa->templateParam = arg;
                sp->storage_class = sc_templateparam;
                sp->tp = tpa;
                arg->p->sym = sp;
                if (isarray(tp) || isfunction(tp))
                {
                    if (isarray(tp))
                        tp = tp->btp;
                    tp1 = Alloc(sizeof(TYPE));
                    tp1->type = bt_pointer;
                    tp1->size = getSize(bt_pointer);
                    tp1->btp = tp;
                    tp = tp1;
                }
                arg->p->byNonType.tp = tp;
                if (tp->type != bt_templateparam && tp->type != bt_enum && !isint(tp) && !ispointer(tp) && basetype(tp)->type != bt_lref)
                {
                    error(ERR_NONTYPE_TEMPLATE_PARAMETER_INVALID_TYPE);
                }
                if (sp)
                {
                    if (MATCHKW(lex, assign))
                    {
                        tp1 = NULL;
                        exp1 = NULL;     
                        lex = getsym();
                        arg->p->byNonType.txtdflt = TemplateArgGetDefault(&lex);
                        if (!arg->p->byNonType.txtdflt)
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                    }
                }
            }
            break;
        }
    }
    
    return lex;
}
static BOOLEAN matchArg(TEMPLATEPARAMLIST *param, TEMPLATEPARAMLIST *arg)
{
    if (param->p->type != arg->p->type)
    {
        return FALSE;
    }
    else if (param->p->type == kw_template)
    {
        if (!exactMatchOnTemplateParams(param->p->byTemplate.args, arg->p->byTemplate.dflt->templateParams->next))
            return FALSE;
    }
    else if (param->p->type == kw_int)
    {
        if (param->p->byNonType.tp->type != bt_templateparam)
            if (!comparetypes(param->p->byNonType.tp, arg->p->byNonType.tp, TRUE) && (!ispointer(param->p->byNonType.tp) || !isconstzero(param->p->byNonType.tp, arg->p->byNonType.dflt)))
                return FALSE;
    }
    return TRUE;
}
BOOLEAN TemplateIntroduceArgs(TEMPLATEPARAMLIST *sym, TEMPLATEPARAMLIST *args)
{
    if (sym)
        sym = sym->next;
    while (sym && args)
    {
        if (!matchArg(sym, args))
            return FALSE;
        switch(args->p->type)
        {
            case kw_typename:
                sym->p->byClass.val = args->p->byClass.dflt;
                break;
            case kw_template:
                sym->p->byTemplate.val = args->p->byTemplate.dflt;
                break;
            case kw_int:
                sym->p->byNonType.val = args->p->byNonType.dflt;
                break;
        }
        sym = sym->next;
        args = args->next;
    }
    return TRUE;
}
static TEMPLATEPARAMLIST *copyParams(TEMPLATEPARAMLIST *t, BOOLEAN alsoSpecializations)
{
    if (t)
    {
        TEMPLATEPARAMLIST *rv = NULL, **last = &rv, *parse;
        parse = t;
        while (parse)
        {
            SYMBOL *sp;
            *last = Alloc(sizeof(TEMPLATEPARAMLIST));
            (*last)->p = Alloc(sizeof(TEMPLATEPARAM));
            *((*last)->p) = *(parse->p);
            sp = (*last)->p->sym;
            if (sp)
            {
                sp = clonesym(sp);
                sp->tp = Alloc(sizeof(TYPE));
                sp->tp->type = bt_templateparam;
                sp->tp->templateParam = *last;
                (*last)->p->sym = sp;
            }
            /*
            if (parse->p->type == kw_typename)
            {
                TYPE *tp = parse->p->byClass.val;
                if (tp && isstructured(tp) && basetype(tp)->sp->templateLevel)
                {
                    TYPE *tpx = Alloc(sizeof(TYPE));
                    *tpx = *tp;
                    tpx->sp = clonesym(tpx->sp);
                    tpx->sp->tp = tpx;
                    tpx->sp->templateParams = copyParams(tpx->sp->templateParams);
                    (*last)->p->byClass.val = tpx;
                }
            }
            */
            last = &(*last)->next;
            parse = parse->next;
        }
        if (t->p->type == kw_new && alsoSpecializations)
        {
            last = &rv->p->bySpecialization.types;
            parse = t->p->bySpecialization.types;
            while (parse)
            {
                *last = Alloc(sizeof(TEMPLATEPARAMLIST));
                (*last)->p = Alloc(sizeof(TEMPLATEPARAM));
                *((*last)->p) = *(parse->p);
                last = &(*last)->next;
                parse = parse->next;
            }
        }
        return rv;
    }
    return t;
}
static TYPE *SynthesizeTemplate(TYPE *tp, BOOLEAN alt)
{
    SYMBOL *rv;
    TEMPLATEPARAMLIST *r = NULL, **last = &r;
    TEMPLATEPARAMLIST *p = tp->sp->templateParams->p->bySpecialization.types;
    if (!p)
        p = tp->sp->templateParams->next;
    while (p)
    {
        *last = Alloc(sizeof(TEMPLATEPARAMLIST));
        (*last)->p = Alloc(sizeof(TEMPLATEPARAM));
        *((*last)->p) = *(p->p);
        last = &(*last)->next;
        p = p->next;
    }
    while (p)
    {
        switch(p->p->type)
        {
            case kw_typename:
                if (p->p->byClass.val->type == bt_templateparam)
                {
                    p->p->byClass.val = p->p->byClass.val->templateParam->p->byClass.val;
                }
                break;
            case kw_template:
                if (p->p->byTemplate.val->tp->type == bt_templateparam)
                {
                    p->p->byTemplate.val = SynthesizeTemplate( p->p->byTemplate.val->tp, TRUE);
                }
                break;
            case kw_int:
                if (p->p->byNonType.tp->type == bt_templateparam)
                {
                    p->p->byNonType.val = p->p->byNonType.tp->templateParam->p->byNonType.val;
                }
                break;
        }
        p = p->next;
    }
    rv = clonesym(tp->sp);
    rv->tp = Alloc(sizeof(TYPE));
    *rv->tp = *tp;
    rv->tp->sp = rv;
    rv->templateParams = Alloc(sizeof(TEMPLATEPARAMLIST));
    rv->templateParams->p = Alloc(sizeof(TEMPLATEPARAM));
    rv->templateParams->p->type = kw_new; // specialization
    rv->templateParams->p->bySpecialization.types = r;
    return rv->tp;
}

void SynthesizeQuals(TYPE ***last, TYPE **qual, TYPE ***lastQual)
{
    if (*qual)
    {
        TYPE *p = **last, *p1 = p;
        TYPE *v = *qual;
        int sz = basetype(**last)->size;
        if (p1)
        {
            TYPE *b = basetype(p1);
            while (b->type == bt_pointer)
            {
                p = b->btp;
                b = basetype(p);
            }
            **last = NULL;
            while (p1 != b)
            {
                **last = Alloc(sizeof(TYPE));
                ***last = *p1;
                (**last)->size = sz;
                *last = &(**last)->btp;
                p1 = p1->btp; 
            }
        }
        else
        {
            p = NULL;
        }
        while (v)
        {
            TYPE *q = p;
            while (q)
            {
                if (q->type == v->type)
                    break;
                q = q->btp;
            }
            if (!q)
            {
                **last = v;
                (**last)->size = sz;
                *last = &(**last)->btp;
            }
            v = v->btp;
        }
        **last = NULL;
        while (p1)
        {
            **last = Alloc(sizeof(TYPE));
            ***last = *p1;
            *last = &(**last)->btp;
            p1 = p1->btp; 
        }
        *lastQual = qual;
        *qual = NULL;
    }
}
static TYPE * SynthesizeStructure(TYPE *tp_in, TEMPLATEPARAMLIST *enclosing)
{
    TYPE *tp = basetype(tp_in);
    if (isref(tp))
        tp = basetype(tp->btp);
    if (isstructured(tp))
    {
        SYMBOL *sp = basetype(tp)->sp;
        if (sp->templateLevel)
        {
            TEMPLATEPARAMLIST *params = NULL, **pt = &params, *search = sp->templateParams->next;
            while (search)
            {
                if (search->p->type == kw_typename)
                {
                    TEMPLATEPARAMLIST *find = enclosing->next;
                    if (!search->p->sym)
                    {
                        if (!search->p->byClass.dflt)   
                            return NULL;
                        *pt = Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*pt)->p = search->p;
                        pt = &(*pt)->next;
                    }
                    else
                    {
                        while (find && strcmp(search->p->sym->name, find->p->sym->name) != 0)
                        {
                            find = find->next;
                        }
                        if (!find)
                            return NULL;
                        *pt = Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*pt)->p = find->p;
                        pt = &(*pt)->next;
                    }
                }
                else
                {
                    *pt = Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*pt)->p = search->p;
                    pt = &(*pt)->next;
                }
                search = search->next;
            }
            sp = GetClassTemplate(sp, params, TRUE, sc_register);
            if (sp)
            {
                TYPE *tp1 = NULL, **tpp = &tp1;
                int sz = sp->tp->size;
                if (isref(tp_in))
                    sz = tp->size;
                if (isconst(tp_in))
                {
                    *tpp = Alloc(sizeof(TYPE));
                    (*tpp)->size = sz;
                    (*tpp)->type = bt_const;
                    tpp = &(*tpp)->btp;
                }
                if (isvolatile(tp_in))
                {
                    *tpp = Alloc(sizeof(TYPE));
                    (*tpp)->size = sz;
                    (*tpp)->type = bt_volatile;
                    tpp = &(*tpp)->btp;
                }
                if (isref(tp_in))
                {
                    *tpp = Alloc(sizeof(TYPE));
                    (*tpp)->size = sz;
                    (*tpp)->type = basetype(tp_in)->type;
                    tpp = &(*tpp)->btp;
                }
                *tpp = sp->tp;
               return tp1;
            }
        }
    }
    return NULL;
}
TYPE *SynthesizeType(TYPE *tp, TEMPLATEPARAMLIST *enclosing, BOOLEAN alt)
{
    TYPE *rv = &stdany, **last = &rv;
    TYPE *qual = NULL, **lastQual = &qual;
    TYPE *tp_in = tp;
    while (1)
    {
        switch(tp->type)
        {
            case bt_typedef:
                tp = tp->btp;
                break;
            case bt_pointer:
                SynthesizeQuals(&last, &qual, &lastQual);
                *last = Alloc(sizeof(TYPE));
                **last = *tp;
                last = &(*last)->btp;
                if (isarray(tp) && tp->etype)
                {
                    tp->etype = SynthesizeType(tp->etype, enclosing, alt);
                }
                tp = tp->btp;
                break;
            case bt_templateselector:
            {
                SYMBOL *sp;
                SYMBOL *ts = tp->sp->templateSelector->next->sym;
                TEMPLATESELECTOR *find = tp->sp->templateSelector->next->next;
                if (tp->sp->templateSelector->next->isTemplate)
                {
                    TEMPLATEPARAMLIST *current = tp->sp->templateSelector->next->templateParams;
                    sp = GetClassTemplate(ts, current, FALSE, sc_global);
                    if (sp)
                        tp = sp->tp;
                    else
                        tp = &stdany;
                        
                }
                else
                {
                    if (ts->tp->templateParam->p->type != kw_typename)
                    {
                        return &stdany;
                    }
                    tp = ts->tp;
                    tp = alt ? tp->templateParam->p->byClass.temp : tp->templateParam->p->byClass.val;
                    sp = tp->sp;
                }
                while (find && sp)
                {
                    if (!isstructured(tp))
                        break;
                    
                    sp = search(find->name, tp->syms);
                    if (sp)
                        tp = sp->tp;
                    find = find->next;
                }
                if (!find && tp)
                {
                    if (tp->type == bt_typedef)
                        tp = tp->btp;
                    *last = tp;
                    SynthesizeQuals(&last, &qual, &lastQual);
                    return rv;
                }
                return &stdany;
                
            }         
            case bt_lref:
            case bt_rref:
                *last = Alloc(sizeof(TYPE));
                **last = *tp;
                last = &(*last)->btp;
                tp = tp->btp;
                break;
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
                *lastQual = Alloc(sizeof(TYPE));
                **lastQual = *tp;
                (*lastQual)->btp = NULL;
                lastQual = &(*lastQual)->btp;
                tp = tp->btp;
                break;
            case bt_memberptr:
                SynthesizeQuals(&last, &qual, &lastQual);
                
                *last = Alloc(sizeof(TYPE));
                **last = *tp;
                last = &(*last)->btp;
                if (tp->sp->tp->type == bt_templateparam)
                {
                    tp->sp = basetype(tp->sp->tp->templateParam->p->byClass.val)->sp;
                    tp->size = tp->sp->tp->size;
                }
                tp = tp->btp;
                break;
            case bt_func:
            case bt_ifunc:
            {
                TYPE *func;
                HASHREC *hr = tp->syms->table[0], **store;
                *last = Alloc(sizeof(TYPE));
                **last = *tp;
                (*last)->syms = CreateHashTable(1);
                func = *last;
                SynthesizeQuals(&last, &qual, &lastQual);
                if (*last)
                    last = &(*last)->btp;
                while (hr)
                {
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    if (sp->packed)
                    {
                        NormalizePacked(sp->tp);
                        if (sp->tp->templateParam && sp->tp->templateParam->p->packed)
                        {
                            TEMPLATEPARAMLIST *templateParams = sp->tp->templateParam->p->byPack.pack;
                            BOOLEAN first = TRUE;
                            sp->tp->templateParam->p->index = 0;
                            if (templateParams)
                            {
                                while (templateParams)
                                {
                                    SYMBOL *clone = clonesym(sp);
                                    clone->tp = SynthesizeType(sp->tp, enclosing, alt);
                                    if (!first)
                                    {
                                        clone->name = clone->decoratedName = clone->errname = AnonymousName();
                                        clone->packed = FALSE;
                                    }
                                    else
                                    {
                                        clone->tp->templateParam = sp->tp->templateParam;
                                    }
                                    templateParams->p->packsym = clone;
                                    insert(clone, (*last)->syms);
                                    first = FALSE;
                                    templateParams = templateParams->next;
                                    sp->tp->templateParam->p->index++;
                                }
                            }
                            else
                            {
                                SYMBOL *clone = clonesym(sp);
                                clone->tp = SynthesizeType(&stdany, enclosing, alt);
                                clone->tp->templateParam = sp->tp->templateParam;
                                insert(clone, func->syms);
                            }
                        }
                    }
                    else
                    {
                        SYMBOL *clone = clonesym(sp);
                        insert(clone, func->syms);
                        clone->tp = SynthesizeType(clone->tp, enclosing, alt);
                    }
                    hr = hr->next;
                }
                tp = tp->btp;
                break;
            }
            case bt_templateparam:
            {
                TEMPLATEPARAMLIST *tpa = tp->templateParam;
                if (tpa->p->packed)
                {
                    int i;
                    int index = tpa->p->index;
                    tpa = tpa->p->byPack.pack;
                    for (i=0; i < index; i++)
                        tpa = tpa->next;
                }
                if (tpa->p->type == kw_typename)
                {
                    TYPE *tx = qual;
                    *last = Alloc(sizeof(TYPE));
                    **last =  alt ? *tpa->p->byClass.temp : *tpa->p->byClass.val;
                    (*last)->templateTop = TRUE;
                    while (tx)
                    {
                        if (tx->type == bt_const)
                            (*last)->templateConst = TRUE;
                        if (tx->type == bt_volatile)
                            (*last)->templateVol = TRUE;
                        tx = tx->btp;
                    }
                    SynthesizeQuals(&last, &qual, &lastQual);
                    return rv;
                }
                else
                {
                    return &stdany;
                }
            }
            default:
                if (enclosing)
                {
                    tp_in = SynthesizeStructure(tp, enclosing);
                    if (tp_in)
                        tp = tp_in;
                }
                *last = tp;
                SynthesizeQuals(&last, &qual, &lastQual);
                return rv;
        }
    }
}
static BOOLEAN hasPack(TYPE *tp)
{
    BOOLEAN rv = FALSE;
    while (ispointer(tp))
        tp = tp->btp;
    if (isfunction(tp))
    {
        HASHREC *hr = tp->syms->table[0];
        while (hr && !rv)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->packed)
            {
                rv = TRUE;
            }
            else if (isfunction(sym->tp) || isfuncptr(sym->tp))
            {
                rv = hasPack(sym->tp);
            }
            hr = hr->next;
        }
    }
    return rv;
}
static TYPE * FillInRV(TYPE *tp, TEMPLATEPARAMLIST *enclosing)
{
    TYPE *tp_in = tp;
    STRUCTSYM s;
    TEMPLATEPARAMLIST *dest, *src;
    TEMPLATESELECTOR *sel;
    SYMBOL *sp;
    s.tmpl = enclosing;
    addTemplateDeclaration(&s);
    tp = basetype(tp);
    if (isref(tp))
        tp = basetype(tp->btp);
    switch (tp->type)
    {
        case bt_templateparam:
            dest = tp->templateParam;
            sp = classsearch(dest->p->sym->name, FALSE);
            if (sp->tp->type == bt_templateparam)
            {
                src = sp->tp->templateParam;
                if (src)
                {
                    dest->p->byTemplate.dflt = src->p->byTemplate.dflt;
                    dest->p->byTemplate.val = src->p->byTemplate.val;
                    dest->p->byTemplate.txtdflt = src->p->byTemplate.txtdflt;
                    dest->p->byTemplate.temp = src->p->byTemplate.temp;
                    dest->p->byTemplate.args = src->p->byTemplate.args;
                }
            }
            break;
        case bt_templateselector:
            sel = tp->sp->templateSelector->next;
            while (sel)
            {
                if (sel->isTemplate)
                {
                    dest = sel->templateParams;
                    while (dest)
                    {
                        sp = classsearch(dest->p->sym->name, FALSE);
                        if (sp->tp->type == bt_templateparam)
                        {
                            src = sp->tp->templateParam;
                            if (src)
                            {
                                dest->p->byTemplate.dflt = src->p->byTemplate.dflt;
                                dest->p->byTemplate.val = src->p->byTemplate.val;
                                dest->p->byTemplate.txtdflt = src->p->byTemplate.txtdflt;
                                dest->p->byTemplate.temp = src->p->byTemplate.temp;
                                dest->p->byTemplate.args = src->p->byTemplate.args;
                            }
                        }
                        dest = dest->next;
                    }
                }
                sel = sel->next;
            }
            break;
        default:
        {
            TYPE *tp1 = SynthesizeStructure(tp_in, enclosing);
            if (tp1)
                tp_in = tp1;
            break;
        }
    }
    dropStructureDeclaration();
    return tp_in;
}
static SYMBOL *SynthesizeParentClass(SYMBOL *sym)
{
    SYMBOL *rv = sym;
    SYMBOL *syms[500];
    int count = 0;
    if (templateNestingCount)
        return sym;
    while (sym)
    {
        syms[count++] = sym;
        sym = sym->parentClass;
    }
    if (count)
    {
        int i;
        for (i=count-1; i >=0 ;i--)
        {
            if (syms[i]->templateLevel && !syms[i]->instantiated)
            {
                break;
            }
        }
        if (i >= 0)
        {
            SYMBOL *last = NULL;
            rv = NULL;
            
            // has templated classes
            for (i=count-1; i >=0 ;i--)
            {
                if (syms[i]->templateLevel)
                {
                    SYMBOL *found = TemplateClassInstantiateInternal(syms[i], syms[i]->templateParams, FALSE);
                    if (!found)
                    {
                        diag("SynthesizeParentClass mismatch 1");
                        return sym;
                    }
                    found = clonesym(found);
                    found->templateParams = copyParams(found->templateParams, TRUE);
                    found->parentClass = last;
                    last = found;
                }
                else
                {
                    if (last)
                    {
                        SYMBOL *found = search(syms[i]->name, last->tp->syms);
                        if (!found || !isstructured(found->tp))
                        {
                            diag("SynthesizeParentClass mismatch 2");
                            return sym;
                        }
                        found->parentClass = last;
                        last = found;
                    }
                    else
                    {
                        last = syms[i];
                    }
                }
                rv = last;
            }
        }
    }
    return rv;
}
SYMBOL *SynthesizeResult(SYMBOL *sym, TEMPLATEPARAMLIST *params)
{
    SYMBOL *rsv = clonesym(sym);
    rsv->parentTemplate = sym;
    rsv->mainsym = sym;
    rsv->parentClass = SynthesizeParentClass(rsv->parentClass);
    if (isfunction(sym->tp))
    {
        basetype(sym->tp)->btp = FillInRV(basetype(sym->tp)->btp, params);
    }
    rsv->tp = SynthesizeType(sym->tp, params, FALSE);
    if (isfunction(rsv->tp))
    {
        basetype(rsv->tp)->sp = rsv;
    }
    rsv->templateParams = params;
    return rsv;
}
static TYPE *removeTLQuals(TYPE *A)
{
    /*
    TYPE *x = A, *rv=NULL, **last = &rv;
    while (ispointer(x))
        x = basetype(x)->btp;
    if (!isconst(x) && !isvolatile(x))
        return A;
    x = A;
    while (ispointer(x))
    {
        *last = Alloc(sizeof(TYPE));
        **last = x;
        last = &(*last)->next;
        x = x->btp; // no basetype to get CV quals
    }
    x = basetype(x); // ignore CV quals
    *last = x;
    return rv;
    */
    return basetype(A);
}
static TYPE *rewriteNonRef(TYPE *A)
{
    if (isarray(A))
    {
        TYPE *x = Alloc(sizeof(TYPE));
        x->type = bt_pointer;
        x->size = getSize(bt_pointer);
        while (isarray(A))
            A = basetype(A)->btp;
        x->btp = A;
        A = x;
    } 
    else if (isfunction(A))
    {
        TYPE *x = Alloc(sizeof(TYPE));
        x->type = bt_pointer;
        x->size = getSize(bt_pointer);
        A=basetype(A);
        x->btp = A;
        return x;
    }
    return removeTLQuals(A);
}
static BOOLEAN hastemplate(EXPRESSION *exp)
{
    if (!exp)
        return FALSE;
    if (exp->type == en_templateparam)
        return TRUE;
    return hastemplate(exp->left)  || hastemplate(exp->right);
}
static void clearoutDeduction(TYPE *tp)
{
    while (1)
    {
        switch(tp->type)
        {
            case bt_pointer:
                if (isarray(tp) && tp->etype)
                {
                    clearoutDeduction(tp->etype);
                }
                tp = tp->btp;
                break;
            case bt_templateselector:
                clearoutDeduction(tp->sp->templateSelector->next->sym->tp);
                return;
            case bt_const:
            case bt_volatile:
            case bt_lref:
            case bt_rref:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
                tp = tp->btp;
                break;
            case bt_memberptr:
                clearoutDeduction(tp->sp->tp);
                tp = tp->btp;
                break;
            case bt_func:
            case bt_ifunc:
            {
                HASHREC *hr = tp->syms->table[0];
                while (hr)
                {
                    clearoutDeduction(((SYMBOL *)hr->p)->tp);
                    hr = hr->next;
                }
                tp = tp->btp;
                break;
            }
            case bt_templateparam:
                tp->templateParam->p->byClass.temp = NULL;
                return ;
            default:
                return;
        }
    }
}
static void ClearArgValues(TEMPLATEPARAMLIST *params)
{
    while (params)
    {
        if (params->p->type != kw_new)
        {
            if (params->p->packed)
                params->p->byPack.pack = NULL;
            else
                params->p->byClass.val = params->p->byClass.temp = NULL;
            if (params->p->byClass.txtdflt)
                params->p->byClass.dflt = NULL;
            if (params->p->byClass.dflt)
            {
                TYPE *tp = params->p->byClass.dflt;
                while (ispointer(tp))
                    tp = basetype(tp)->btp;
                if (tp ->type == bt_templateparam)
                {
                    TEMPLATEPARAMLIST *t = tp->templateParam;
                    t->p->byClass.val = NULL;
                }
            }
        }
        params = params->next;
    }
}
static BOOLEAN Deduce(TYPE *P, TYPE *A, BOOLEAN change, BOOLEAN byClass);
static BOOLEAN DeduceFromTemplates(TYPE *P, TYPE *A, BOOLEAN change, BOOLEAN byClass)
{
    if (basetype(P)->sp && basetype(A)->sp)
    {
        TEMPLATEPARAMLIST *TP = basetype(P)->sp->templateParams->next;
        TEMPLATEPARAMLIST *TA = basetype(A)->sp->templateParams;
        if (!TA || !TP)
            return FALSE;
        if (TA->p->bySpecialization.types)
            TA = TA->p->bySpecialization.types;
        else
            TA = TA->next;
//        if (byClass && P->sp->parentTemplate != A->sp->parentTemplate || !byClass && P->sp != A->sp)
//            return FALSE;
        while (TP && TA)
        {
            if (TP->p->type != TA->p->type)
                return FALSE;
            switch (TP->p->type)
            {
                case kw_typename:
                {
                    TYPE **tp = change ? &TP->p->byClass.val : &TP->p->byClass.temp;
                    if (*tp)
                    {
                        if (!templatecomparetypes(*tp, TA->p->byClass.val, TRUE))
                            return FALSE;
                    }
                    *tp = TA->p->byClass.val;
                    if (!TP->p->byClass.txtdflt)
                        TP->p->byClass.txtdflt = TA->p->byClass.txtdflt;
                    break;
                }
                case kw_template:
                {
                    TEMPLATEPARAMLIST *paramT = TP->p->sym->templateParams;
                    TEMPLATEPARAMLIST *paramA = TA->p->sym->templateParams;
                    while (paramT && paramA)
                    {
                        if (paramT->p->type != paramA->p->type)
                            return FALSE;
                            
                        paramT = paramT->next;
                        paramA = paramA->next;
                    }
                    if (paramT || paramA)
                        return FALSE;
                    if (!TP->p->byTemplate.txtdflt)
                        TP->p->byTemplate.txtdflt = TA->p->byTemplate.txtdflt;
                    if (!TP->p->byTemplate.val)
                        TP->p->byTemplate.val = TA->p->byTemplate.val;
                    else if (!DeduceFromTemplates(TP->p->byTemplate.val, TA->p->byTemplate.val, change, byClass))
                        return FALSE;
                    break;
                }
                case kw_int:
                {
                    TYPE **tp = TP->p->byNonType.tp;
                    EXPRESSION **exp = change ? &TP->p->byNonType.val : &TP->p->byNonType.val;
                    if (*exp)
                    {
                        if (!templatecomparetypes(*tp, TA->p->byNonType.tp, TRUE))
                            return FALSE;
    #ifndef PARSER_ONLY
                        if (!equalnode(*exp, TA->p->byNonType.val))
                            return FALSE;
    #endif
                    }
                    *tp = TA->p->byNonType.tp;
                    *exp = TA->p->byNonType.val;
                    if (!TP->p->byNonType.txtdflt)
                        TP->p->byNonType.txtdflt = TA->p->byNonType.txtdflt;
                    break;
                }
            }
            TP = TP->next;
            TA = TA->next;
        }
        return !TP && !TA;
    }
    return FALSE;
}
static TYPE *FixConsts(TYPE *P, TYPE *A)
{
    int pn=0, an=0;
    TYPE *q = P , **last = &q;
    int i;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        pn++;
    }
    q = A;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        an++;
    }
    *last = NULL;
    if (pn < an)
    {
        return A;
    }
    else if (pn > an)
    {
        for (i=0; i < pn - an; i++)
            P = basetype(P)->btp;
    }
    while (P && A)
    {
        TYPE *m;
        BOOLEAN constant = FALSE;
        BOOLEAN vol = FALSE;
        if (isconst(P) && !isconst(A))
            constant = TRUE;
        if (isvolatile(P) && !isvolatile(A))
            vol = TRUE;
        while (isconst(P) || isvolatile(P))
        {
            if (constant && isconst(P) || vol && isvolatile(P))
            {
                *last = Alloc(sizeof(TYPE));
                **last = *P;
                last = &(*last)->btp;
                *last = NULL;
            }
            P = P->btp;
        }
        A = basetype(A);
        *last = Alloc(sizeof(TYPE));
        **last = *A;
        last = &(*last)->btp;
        *last = NULL;
        A = A->btp;
    }
    return q;
}
static BOOLEAN DeduceTemplateParam(TEMPLATEPARAMLIST *Pt, TYPE *P, TYPE *A, BOOLEAN change)
{
    if (Pt->p->type == kw_typename)
    {
        TYPE **tp = change ? &Pt->p->byClass.val : &Pt->p->byClass.temp;
        if (*tp)
        {
            if (!Pt->p->initialized && !templatecomparetypes(*tp, A, TRUE))
                return FALSE;
        }
        else
        {
            if (P)
            {
                TYPE *q = A;
                while (q)
                {
                    if (isconst(q))
                    {
                        *tp = FixConsts(P, A);
                        return TRUE;
                    }
                    q = basetype(q)->btp;
                }
            }
            *tp = A;
        }
        return TRUE;
    }
    else if (Pt->p->type == kw_template && isstructured(A) && basetype(A)->sp->templateLevel)
    {
        TEMPLATEPARAMLIST *primary = Pt->p->byTemplate.args;
        TEMPLATEPARAMLIST *match = basetype(A)->sp->templateParams->next;
        while (primary &&match)
        {
            if (primary->p->type != match->p->type)
                return FALSE;
            if (!DeduceTemplateParam(primary, NULL, match->p->byClass.val, change))
                return FALSE;
            primary = primary->next;
            match = match->next;
        }
        return !primary && !match;
    }
    else
    {
        return FALSE;
    }
}
static BOOLEAN Deduce(TYPE *P, TYPE *A, BOOLEAN change, BOOLEAN byClass)
{
    BOOLEAN constant = FALSE;
    if (!P || !A)
        return FALSE;
    while (1)
    {
        TYPE *Ab = basetype(A);
        TYPE *Pb = basetype(P);
        if (isstructured(Pb) && Pb->sp->templateLevel && isstructured(Ab))
            return DeduceFromTemplates(P, A, change, byClass);
        if (Ab->type != Pb->type && Pb->type != bt_templateparam)
            return FALSE;
        switch(Pb->type)
        {
            case bt_pointer:
                if (isarray(Pb) && Pb->etype)
                {
                    if (hastemplate(Pb->esize))
                        return FALSE;
                }
                P = Pb->btp;
                A = Ab->btp;
                break;
            case bt_templateselector:
                return FALSE;
            case bt_lref:
            case bt_rref:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
                P = Pb->btp;
                A = Ab->btp;
                break;
            case bt_memberptr:
                return Deduce(Pb->sp->tp, Ab->sp->tp, change, byClass) && Deduce(Pb->btp, Ab->btp, change, byClass);
            case bt_func:
            case bt_ifunc:
            {
                HASHREC *hrp = Pb->syms->table[0];
                HASHREC *hra = Ab->syms->table[0];
                clearoutDeduction(P);
                if (!Deduce(Pb->btp, Ab->btp, FALSE, byClass))
                    return FALSE;
                
                while (hra && hrp)
                {
                    if (!Deduce(((SYMBOL *)hra->p)->tp, ((SYMBOL *)hra->p)->tp, FALSE, byClass))
                        return FALSE;
                    hrp = hrp->next;
                    hra = hra->next;
                }
                if (hra)
                    return FALSE;
                if (hrp && !((SYMBOL *)hrp->p)->init)
                    return FALSE;
                hrp = Pb->syms->table[0];
                hra = Ab->syms->table[0];
                clearoutDeduction(P);
                Deduce(Pb->btp, Ab->btp, TRUE, byClass);
                
                while (hra && hrp)
                {
                    Deduce(((SYMBOL *)hra->p)->tp, ((SYMBOL *)hra->p)->tp, TRUE, byClass);
                    hrp = hrp->next;
                    hra = hra->next;
                }
                return TRUE;
            }
            case bt_templateparam:
                return DeduceTemplateParam(Pb->templateParam, P, A, change);
            default:
                
                return TRUE;
        }
    }
}
static int eval(EXPRESSION *exp)
{
    optimize_for_constants(&exp);
    if (IsConstantExpression(exp, FALSE))
        return exp->v.i;
    return 0;
}
static BOOLEAN ValidExp(EXPRESSION **exp_in)
{
    BOOLEAN rv = TRUE;
    EXPRESSION *exp = *exp_in;
    if (exp->left)
        rv &= ValidExp(&exp->left);
    if (exp->right)
        rv &= ValidExp(&exp->right);
    if (exp->type == en_templateparam)
        if (!exp->v.sp->templateParams || !exp->v.sp->templateParams->p->byClass.val)
            return FALSE;
    return rv;
}
static BOOLEAN ValidArg(TYPE *tp)
{
    while (1)
    {
        switch(tp->type)
        {
            case bt_pointer:
                if (isarray(tp))
                {
                    while (isarray(tp))
                    {
                        tp = basetype(tp)->btp;
                        if (tp->etype)
                        {
                            int n = eval(tp->esize);
                            if (n <= 0)
                                return FALSE;
                        }
                    }
                    if (tp->type == bt_templateparam)
                    {
                        if (tp->templateParam->p->type != kw_typename)
                            return FALSE;
                        tp = tp->templateParam->p->byClass.val;
                    }
                    if (tp->type == bt_void || isfunction(tp) || isref(tp) || isstructured(tp) && tp->sp->isabstract)
                        return FALSE;
                }
                if (ispointer(tp))
                {
                    while (ispointer(tp))
                        tp = tp->btp;
                    if (tp->type == bt_templateparam)
                    {
                        if (tp->templateParam->p->type != kw_typename)
                            return FALSE;
                        tp = tp->templateParam->p->byClass.val;
                        if (!tp)
                            return FALSE;
                    }
                    if (isref(tp))
                        return FALSE;
                }
                return TRUE;
            case bt_templateselector:
            {
                SYMBOL *ts = tp->sp->templateSelector->next->sym;
                SYMBOL *sp;
                TEMPLATESELECTOR *find = tp->sp->templateSelector->next->next;
                if (tp->sp->templateSelector->next->isTemplate)
                {
                    TEMPLATEPARAMLIST *current = tp->sp->templateSelector->next->templateParams;
                    sp = GetClassTemplate(ts, current, FALSE, sc_global);
                    tp = NULL;
                }
                else if (tp->templateParam->p->type == kw_typename)
                {
                    tp = ts->tp->templateParam->p->byClass.val;
                    sp = tp->sp;
                }
                else if (tp->templateParam->p->type == kw_delete)
                {
                    TEMPLATEPARAMLIST *args = ts->tp->templateParam->p->byDeferred.args;
                    TEMPLATEPARAMLIST *val = NULL, **lst = &val;
                    sp = tp->templateParam->p->sym;
                    sp = TemplateClassInstantiateInternal(sp, args, TRUE);
                }
                if (sp)
                {
                    while (find && sp)
                    {
                        if (!isstructured(sp->tp))
                            break;
                        
                        sp = search(find->name, sp->tp->syms);
                        find = find->next;
                    }
                    return !find && sp && istype(sp) ;
                }
                return FALSE;                
                break;
            }
            case bt_lref:
            case bt_rref:
                tp = basetype(tp)->btp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return FALSE;
                    tp = tp->templateParam->p->byClass.val;
                }
                if (isref(tp))
                    return FALSE;
                break;
            case bt_memberptr:
                tp = tp->sp->tp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return FALSE;
                    tp = tp->templateParam->p->byClass.val;
                }
                if (!isstructured(tp))
                    return FALSE;
                tp = tp->btp;
                break;
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
                tp = tp->btp;
                break;
            case bt_func:
            case bt_ifunc:
            {
                HASHREC *hr = tp->syms->table[0];
                while (hr)
                {
                    if (!ValidArg(((SYMBOL *)hr->p)->tp))
                        return FALSE;
                    hr = hr->next;
                }
                tp = tp->btp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return FALSE;
                    tp = tp->templateParam->p->byClass.val;
                }
                if (isfunction(tp) || isarray(tp) || isstructured(tp) && tp->sp->isabstract)
                    return FALSE;                
                break;
            }
            case bt_templateparam:
                if (tp->templateParam->p->type != kw_typename)
                    return FALSE;
                 if (tp->templateParam->p->packed)
                     return TRUE;
                 if (tp->templateParam->p->byClass.val == NULL)
                     return FALSE;
                 if (tp->templateParam->p->byClass.val->type == bt_void)
                     return FALSE;
                return ValidArg(tp->templateParam->p->byClass.val);
            default:
                return TRUE;
        }
    }
}
static INITLIST * valFromDefault(TEMPLATEPARAMLIST *params, BOOLEAN usesParams, INITLIST **args)
{
    while (params && (!usesParams || *args))
    {
        if (params->p->packed)
        {
            if (!valFromDefault(params->p->byPack.pack, usesParams, args))
                return FALSE;
        }
        else
        {
            if (!params->p->byClass.val)
                params->p->byClass.val = params->p->byClass.dflt;
            if (!params->p->byClass.val)
                return FALSE;
            if (*args)
                *args = (*args)->next;
        }
        params = params->next;
    }
    return TRUE;
}
static BOOLEAN ValidateArgsSpecified(TEMPLATEPARAMLIST *params, SYMBOL *func, INITLIST *args)
{
    BOOLEAN usesParams = !!args;
    INITLIST *check = args;
    HASHREC *hr = basetype(func->tp)->syms->table[0];
    if (!valFromDefault(params, usesParams, &args))
        return FALSE;
    while (params)
    {
        if (params->p->type == kw_typename || params->p->type == kw_template || params->p->type == kw_int)
            if (!params->p->packed && !params->p->byClass.val)
                return FALSE;
        params = params->next;
    }
    if (hr)
    {
        STRUCTSYM s;
        s.tmpl = func->templateParams;
        addTemplateDeclaration(&s);
        args = check;
        while (args && hr)
        {
            args = args->next;
            hr = hr->next;
        }
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->deferredCompile)
            {
                LEXEME *lex = SetAlternateLex(sp->deferredCompile);
                sp->init = NULL;
                lex = initialize(lex, func, sp, sc_parameter, TRUE);
                SetAlternateLex(NULL);
                if (sp->init && sp->init->exp && !ValidExp(&sp->init->exp))
                {
                    dropStructureDeclaration();
                    return FALSE;
                }
            }
            hr = hr->next;
        }
        dropStructureDeclaration();
    }
//    if (!ValidArg(basetype(func->tp)->btp))
//        return FALSE;
    hr = basetype(func->tp)->syms->table[0];
    while (hr && (!usesParams || check))
    {
        if (!ValidArg(((SYMBOL *)hr->p)->tp))
            return FALSE;
        if (check)
            check = check->next;
        hr = hr->next;
    }
    return TRUE;
}
static BOOLEAN TemplateDeduceFromArg(TYPE *orig, TYPE *sym, EXPRESSION *exp, BOOLEAN byClass)
{
    TYPE *P=orig, *A=sym;
    if (!isref(P))
    {
        A = rewriteNonRef(A);
    }
    P = removeTLQuals(P);
    if (isref(P))
        P = basetype(P)->btp;
    if (basetype(orig)->type == bt_rref)
    {
        if (lvalue(exp))
        {
            TYPE *x = Alloc(sizeof(TYPE));
            if (isref(A))
                A = basetype(A)->btp;
            x->type = bt_lref;
            x->size = getSize(bt_pointer);
            x->btp = A;
        }
    }
    if (Deduce(P, A, TRUE, byClass))
        return TRUE;
    if (isfuncptr(P) || isref(P) && isfunction(basetype(P)->btp))
    {
        if (exp->type == en_func)
        {
            if (exp->v.sp->storage_class == sc_overloads)
            {
                HASHREC *hr = basetype(exp->v.sp->tp)->syms->table[0];
                SYMBOL *candidate = FALSE;
                while (hr)
                {
                    SYMBOL *sym = (SYMBOL *)hr->p;
                    if (sym->templateLevel)
                        return FALSE;
                    hr = hr->next;
                }
                // no templates, we can try each function one at a time
                hr = basetype(exp->v.sp->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL *sym = (SYMBOL *)hr->p;
                    clearoutDeduction(P);
                    if (Deduce(P, sym, FALSE, byClass))
                        if (candidate)
                            return FALSE;
                        else
                            candidate = sym;
                    hr = hr->next;
                }
                if (candidate)
                    return Deduce(P, candidate, TRUE, byClass);
            }
        }       
    }
    return FALSE;
}
void NormalizePacked(TYPE *tpo)
{
    TYPE *tp = tpo;
    while (isref(tp) || ispointer(tp))
        tp = basetype(tp)->btp;
    if (basetype(tp)->templateParam)
        tpo->templateParam = basetype(tp)->templateParam;
}
INITLIST *TemplateDeduceArgList(HASHREC *templateArgs, INITLIST *symArgs)
{
    while (templateArgs && symArgs)
    {
        SYMBOL *sp = (SYMBOL *)templateArgs->p;
        if (sp->packed)
        {
            NormalizePacked(sp->tp);
            if (sp->tp->templateParam && sp->tp->templateParam->p->packed)
            {
                TEMPLATEPARAMLIST *params = sp->tp->templateParam->p->byPack.pack;
                while (params && symArgs)
                {
                    TemplateDeduceFromArg(params->p->byClass.val, symArgs->tp, symArgs->exp, FALSE);
                    params = params->next;
                    symArgs = symArgs->next;
                }
            }
            else
            {
                symArgs = symArgs->next;
            }
        }    
        else
        {
            TemplateDeduceFromArg(sp->tp, symArgs->tp, symArgs->exp, FALSE);
            symArgs = symArgs->next;
        }
        templateArgs = templateArgs->next;
    }
    return symArgs;
}
static BOOLEAN TemplateParseDefaultArgs(TEMPLATEPARAMLIST *dest, TEMPLATEPARAMLIST *src, TEMPLATEPARAMLIST *enclosing)
{
    STRUCTSYM s;
    s.tmpl = enclosing;
    addTemplateDeclaration(&s);
    while (src && dest)
    {
        if (!dest->p->byClass.val && !dest->p->packed)
        {
            LEXEME *lex;
            if (!src->p->byClass.txtdflt)
            {
                dropStructureDeclaration();
                return FALSE;
            }
            dest->p->byClass.txtdflt = src->p->byClass.txtdflt;
            lex = SetAlternateLex(src->p->byClass.txtdflt);
            switch(dest->p->type)
            {
                case kw_typename:
                    lex = get_type_id(lex, &dest->p->byClass.val, NULL, FALSE);
                    if (!dest->p->byClass.val)    
                    {
                        error(ERR_CLASS_TEMPLATE_DEFAULT_MUST_REFER_TO_TYPE);
                        dropStructureDeclaration();
                        return FALSE;
                    }
                    break;
                case kw_template:
                    lex = id_expression(lex, NULL, &dest->p->byTemplate.val, NULL, NULL, NULL, FALSE, FALSE, lex->value.s.a);
                    if (!dest->p->byTemplate.val)
                    {
                        errorstr(ERR_UNDEFINED_IDENTIFIER,lex->value.s.a);
                        dropStructureDeclaration();
                        return FALSE;
                    }
                    break;
                case kw_int:
                {
                    TYPE *tp1;
                    EXPRESSION *exp1;
                    lex = expression_no_comma(lex, NULL, NULL, &tp1, &exp1, NULL, _F_NOINLINE | _F_INTEMPLATEPARAMS);
                    dest->p->byNonType.val = exp1;
                    if (!templatecomparetypes(dest->p->byNonType.tp, tp1, TRUE))
                    {
                        if (!ispointer(tp1) || !isint(tp1) || !isconstzero(tp1, exp1))
                            error(ERR_TYPE_NONTYPE_TEMPLATE_PARAMETER_DEFAULT_TYPE_MISMATCH);
                    }
                }
                    break;
            }
            SetAlternateLex(NULL);
        }
        src = src->next;
        dest = dest->next;
    }
    dropStructureDeclaration();
    return TRUE;
}
SYMBOL *TemplateDeduceArgsFromArgs(SYMBOL *sym, FUNCTIONCALL *args)
{
    TEMPLATEPARAMLIST *nparams = sym->templateParams;
    if (args && args->thistp && sym->parentClass && !nparams)
    {
        TYPE *tp = basetype(basetype(args->thistp)->btp);
        TEMPLATEPARAMLIST *src = tp->sp->templateParams;
        TEMPLATEPARAMLIST *dest = sym->parentClass->templateParams;
        if (src && dest)
        {
            src = src->next;
            dest = dest->next;
        }
        while (src && dest)
        {
            dest->p->byNonType.dflt = src->p->byNonType.dflt;
            dest->p->byNonType.val = src->p->byNonType.val;
            dest->p->byNonType.txtdflt = src->p->byNonType.txtdflt;
            dest->p->byNonType.temp = src->p->byNonType.temp;
            dest->p->byNonType.tp = src->p->byNonType.tp;
            src = src->next;
            dest = dest->next;
        }
        
        if (src || dest)
            return NULL;
    }
    if (nparams)
    {
        TEMPLATEPARAMLIST *params = nparams->next;
        HASHREC *templateArgs = basetype(sym->tp)->syms->table[0], *temp;
        INITLIST *symArgs = args->arguments;
        TEMPLATEPARAMLIST *initial = args->templateParams;
        ClearArgValues(params);
        // fill in params that have been initialized in the arg list
        while (initial && params)
        {
            if (initial->p->type != params->p->type)
                return NULL;
            params->p->initialized = TRUE;
            if (params->p->packed)
            {
                TEMPLATEPARAMLIST *nparam = Alloc(sizeof(TEMPLATEPARAMLIST));
                TEMPLATEPARAMLIST **p = & params->p->byPack.pack;
                nparam->p = Alloc(sizeof(TEMPLATEPARAM));
                while (*p)
                    p = &(*p)->next;
                nparam->p->type = params->p->type;
                nparam->p->byClass.val = initial->p->byClass.dflt;
                *p = nparam;
            }
            else
            {
                params->p->byClass.val = initial->p->byClass.dflt;
                params = params->next;
            }
            initial = initial->next;
        }
        
        // check the specialization list for validity
        params = nparams->p->bySpecialization.types;
        initial = args->templateParams;
        while (initial && params)
        {
            if (initial->p->type != params->p->type)
                return NULL;
            switch(initial->p->type)
            {
                case kw_typename:
                    if (!templatecomparetypes(initial->p->byClass.dflt, params->p->byClass.dflt, TRUE))
                        return FALSE;
                    break;
                case kw_template:
                    if (!exactMatchOnTemplateParams(initial->p->byTemplate.dflt, params->p->byTemplate.dflt->templateParams->next))
                        return FALSE;
                    break;
                case kw_int:
                    if (!templatecomparetypes(initial->p->byNonType.tp, params->p->byNonType.tp, TRUE) && (!ispointer(params->p->byNonType.tp) || !isconstzero(initial->p->byNonType.tp, params->p->byNonType.dflt)))
                        return FALSE;
                    break;
            }
            initial = initial->next;
            if (!params->p->packed)
                params = params->next;
        }
        // Deduce any args that we can
        if (((SYMBOL *)(templateArgs->p))->thisPtr)
            templateArgs = templateArgs->next;
        temp = templateArgs;
        while (temp)
        {
            if (((SYMBOL *)temp->p)->packed)
                break;
            temp = temp->next;
        }
        if (temp && !args->astemplate)
        {
            // we have to gather the args list
            while (templateArgs && symArgs )
            {
                SYMBOL *sp = (SYMBOL *)templateArgs->p;
                if (sp->packed)
                    break;
                TemplateDeduceFromArg(sp->tp, symArgs->tp, symArgs->exp, FALSE);
                symArgs = symArgs->next;
                templateArgs = templateArgs->next;
            }
            if (templateArgs)
            {
                SYMBOL *sp = (SYMBOL *)templateArgs->p;
                TEMPLATEPARAMLIST *base = sp->tp->templateParam;
                if (base->p->type == kw_typename)
                {
                    TEMPLATEPARAMLIST **p = &base->p->byPack.pack;
                    while (symArgs)
                    {
                        *p = Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*p)->p = Alloc(sizeof(TEMPLATEPARAM));
                        (*p)->p->type = kw_typename;
                        (*p)->p->byClass.val = symArgs->tp;
                        p = &(*p)->next;
                        symArgs = symArgs->next;
                    }
                }
            }
        }
        else
        {
            TemplateDeduceArgList(templateArgs, symArgs);
        }
        // set up default values for non-deduced and non-initialized args
        params = nparams->next;
        if (TemplateParseDefaultArgs(params, params, params) && 
            ValidateArgsSpecified(sym->templateParams->next, sym, args->arguments))
        {
            return SynthesizeResult(sym, nparams);
        }
        return NULL;
    }
    return SynthesizeResult(sym, nparams);
}
static BOOLEAN TemplateDeduceFromType(TYPE* P, TYPE *A)
{
    return Deduce(P, A, TRUE, FALSE);    
}
SYMBOL *TemplateDeduceWithoutArgs(SYMBOL *sym)
{
    TEMPLATEPARAMLIST *nparams = sym->templateParams;
    TEMPLATEPARAMLIST *params = nparams->next;
    if (TemplateParseDefaultArgs(params, params, params) && ValidateArgsSpecified(sym->templateParams->next, sym, NULL))
    {
        return SynthesizeResult(sym, nparams);
    }
    return NULL;
}
static BOOLEAN TemplateDeduceFromConversionType(TYPE* orig, TYPE *tp)
{
    TYPE *P = orig, *A = tp;
    if (isref(P))
        P = basetype(P)->btp;
    if (!isref(A))
    {
        P = rewriteNonRef(P);
    }
    A = removeTLQuals(A);
    if (TemplateDeduceFromType(P, A))
        return TRUE;
    if (ispointer(P))
    {
        BOOLEAN doit = FALSE;
        while (ispointer(P) && ispointer(A))
        {
            if (isconst(P) && !isconst(A) || isvolatile(P) && !isvolatile(A))
                return FALSE;
            P = basetype(P)->btp;
            A = basetype(A)->btp;
        }
        P = basetype(P);
        A = basetype(A);
        if (doit && TemplateDeduceFromType(P, A))
            return TRUE;
    }
    return FALSE;
}
SYMBOL *TemplateDeduceArgsFromType(SYMBOL *sym, TYPE *tp)
{
    TEMPLATEPARAMLIST *nparams = sym->templateParams;
    ClearArgValues(nparams);
    if (sym->castoperator)
    {
        TEMPLATEPARAMLIST *params;
        TemplateDeduceFromConversionType(basetype(sym->tp)->btp, basetype(tp)->btp);
        params = nparams->next;
        if (TemplateParseDefaultArgs(params, params, params) && ValidateArgsSpecified(sym->templateParams->next, sym, NULL))
            return SynthesizeResult(sym, nparams);
    }
    else
    {
        HASHREC *templateArgs = basetype(tp)->syms->table[0];
        HASHREC *symArgs = basetype(sym->tp)->syms->table[0];
        TEMPLATEPARAMLIST *params;
        while (templateArgs && symArgs)
        {
            SYMBOL  *sp = (SYMBOL *)symArgs->p;
            if (sp->packed)
                break;
            TemplateDeduceFromType(sp->tp, ((SYMBOL *)templateArgs->p)->tp);
            templateArgs = templateArgs->next;
            symArgs = symArgs->next;
        }
        if (templateArgs && symArgs)
        {
            SYMBOL  *sp = (SYMBOL *)symArgs->p;
            TEMPLATEPARAMLIST *base = sp->tp->templateParam;
            if (base->p->type == kw_typename)
            {
                TEMPLATEPARAMLIST **p = &base->p->byPack.pack;
                while (symArgs)
                {
                    *p = Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*p)->p = Alloc(sizeof(TEMPLATEPARAM));
                    (*p)->p->type = kw_typename;
                    (*p)->p->byClass.val = sp->tp;
                    symArgs = symArgs->next;
                }
            }
        }
        params = nparams->next;
        if (TemplateParseDefaultArgs(params, params, params) && ValidateArgsSpecified(sym->templateParams->next, sym, NULL))
        {
            return SynthesizeResult(sym, nparams);
        }
    }
    return NULL;
}
int TemplatePartialDeduceFromType(TYPE *orig, TYPE *sym, BOOLEAN byClass)
{
    TYPE *P = orig, *A=sym;
    int which = -1;
    if (isref(P))
        P= basetype(P)->btp;
    if (isref(A))
        A= basetype(A)->btp;
    if (isref(orig) && isref(sym))
    {
        BOOLEAN p=FALSE, a = FALSE;
        if (isconst(P) && !isconst(A) || isvolatile(P) && !isvolatile(A))
            p = TRUE;
        if (isconst(A) && !isconst(P) || isvolatile(A) && !isvolatile(P))
            a = TRUE;
        if (a && !p)
            which = 1;
    }
    A = removeTLQuals(A);
    P = removeTLQuals(P);
    if (!Deduce(P, A, TRUE, byClass))
        return 0;
    return which;
}
int TemplatePartialDeduce(TYPE *origl, TYPE *origr, TYPE *syml, TYPE *symr, BOOLEAN byClass)
{
    int n = TemplatePartialDeduceFromType(origl, symr, byClass);
    int m = TemplatePartialDeduceFromType(origr, syml, byClass);
    if (n && m)
    {
        if (basetype(origl)->type == bt_lref)
        {
            if (basetype(origr)->type != bt_lref)
                return -1;
        }
        else if (basetype(origr)->type == bt_lref)
        {
            return 1;
        }
        if (n > 0 && m <= 0)
            return -1;
        else if (m > 0 && n <= 0)
            return 1;
        return 0;
    }
    if (n)
        return -1;
    if (m)
        return 1;
    return 100;
}
int TemplatePartialDeduceArgsFromType(SYMBOL *syml, SYMBOL *symr, TYPE *tpl, TYPE *tpr, FUNCTIONCALL *fcall)
{
    int which = 0;
    int arr[200], n;
    ClearArgValues(syml->templateParams);
    ClearArgValues(symr->templateParams);
    if (isstructured(syml->tp))
    {
        which = TemplatePartialDeduce(syml->tp, symr->tp, tpl, tpr, TRUE);
    }
    else if (syml->castoperator)
    {
        which = TemplatePartialDeduce(basetype(syml->tp)->btp, basetype(symr->tp)->btp, basetype(tpl)->btp, basetype(tpr)->btp, FALSE);
    }
    else
    {
        int i;
        HASHREC *tArgsl = basetype(tpl)->syms->table[0];
        HASHREC *sArgsl = basetype(syml->tp)->syms->table[0];
        HASHREC *tArgsr = basetype(tpr)->syms->table[0];
        HASHREC *sArgsr = basetype(symr->tp)->syms->table[0];
        BOOLEAN usingargs = fcall && fcall->ascall;
        INITLIST *args = fcall->arguments;
        if (fcall->thisptr)
        {
            tArgsl = tArgsl->next;
            sArgsl = sArgsl->next;
            tArgsr = tArgsr->next;
            sArgsr = sArgsr->next;
        }
        n = 0;
        while (tArgsl && tArgsr && sArgsl && sArgsr && (!usingargs || args))
        {
            arr[n++] = TemplatePartialDeduce(((SYMBOL *)sArgsl->p)->tp, ((SYMBOL *)sArgsr->p)->tp,
                                  ((SYMBOL *)tArgsl->p)->tp, ((SYMBOL *)tArgsr->p)->tp, FALSE);
            if (args)
                args = args->next;
            tArgsl = tArgsl->next;
            sArgsl = sArgsl->next;
            tArgsr = tArgsr->next;
            sArgsr = sArgsr->next;
        }
        for (i=0; i < n; i++)
            if (arr[i] == 100)
                return 0;
        for (i=0; i < n; i++)
            if (!which)
                which = arr[i];
            else if (which && which != arr[i])
                return 0;
    }
    if (!which)
    {
        if (!syml->specialized && symr->specialized)
        {
            return -1;
        }
        else
        {
            TEMPLATEPARAMLIST *l = syml->templateParams->p->bySpecialization.types;
            TEMPLATEPARAMLIST *r = symr->templateParams->p->bySpecialization.types;
            int i;
            n = 0;
            while (l && r)
            {
                int l1 = l->p->type == kw_typename ? typeHasTemplateArg(l->p->byClass.dflt) : 0;
                int r1 = r->p->type == kw_typename ? typeHasTemplateArg(r->p->byClass.dflt) : 0;
                if (l1 && !r1)
                    arr[n++] = 1;
                else if (r1 && !l1)
                    arr[n++] = -1;
                l = l->next;
                r= r->next;
            }
            for (i=0; i < n; i++)
                if (!which)
                    which = arr[i];
                else if (which && which != arr[i])
                    return 0;
        }
    }
    return which;
}
static void saveParams(SYMBOL **table, int count)
{
    int i;
    for (i=0; i < count; i++)
    {
        if (table[i])
        {
            TEMPLATEPARAMLIST *params = table[i]->templateParams;
            while (params)
            {
                params->p->hold = params->p->byClass.val;
                params = params->next;
            }
        }
    }
}
static void restoreParams(SYMBOL **table, int count)
{
    int i;
    for (i=0; i < count; i++)
    {
        if (table[i])
        {
            TEMPLATEPARAMLIST *params = table[i]->templateParams;
            while (params)
            {
                params->p->byClass.val = params->p->hold;
                params = params->next;
            }
        }
    }
}
void TemplatePartialOrdering(SYMBOL **table, int count, FUNCTIONCALL *funcparams, TYPE *atype, BOOLEAN asClass, BOOLEAN save)
{
    int i,j, n = 47, c = 0;
    int cn = 0, cn2;
    for (i=0; i < count; i++)
        if (table[i])
            c++;
    if (c)
    {
        if (funcparams && funcparams->templateParams)
        {
            TEMPLATEPARAMLIST * t = funcparams->templateParams;
            while (t)
            {
                cn++;
                t = t->next;
            }
        }
    }
    if (c > 1)
    {
        LIST *types = NULL, *exprs = NULL, *classes = NULL;
        TYPE **typetab = Alloc(sizeof(TYPE *) * count);
        if (save)
            saveParams(table, count);
        for (i=0; i < count; i++)
        {
            if (table[i] && table[i]->templateLevel)
            {
                TEMPLATEPARAMLIST * t;
                cn2 = 0;
                if (cn)
                {
                    t = table[i]->templateParams->p->bySpecialization.types;
                    if (!t)
                    {
                        t = table[i]->templateParams->next;
                        while (t && !t->p->byClass.dflt)
                        {
                            t = t->next;
                            cn2++;
                        }
                        while (t && cn2 < cn)
                        {
                            t = t->next;
                            cn2++;
                        }
                    }
                    else
                    {
                        while (t)
                        {
                            t = t->next;
                            cn2++;
                        }
                    }
                }
                if (cn != cn2)
                {
                    table[i] = NULL;
                }
                else
                {
                    SYMBOL *sym = table[i];
                    TEMPLATEPARAMLIST *params;
                    LIST *typechk, *exprchk, *classchk;
                    if (!asClass)
                        sym = sym->parentTemplate;
                    params = sym->templateParams->next;
                    typechk = types;
                    exprchk = exprs;
                    classchk = classes;
                    while(params)
                    {
                        switch(params->p->type)
                        {
                            case kw_typename:
                                if (typechk)
                                {
                                    params->p->byClass.temp = (EXPRESSION *)typechk->data;
                                    typechk = typechk->next;
                                    
                                }
                                else
                                {
                                    LIST *lst = Alloc(sizeof(LIST));
                                    TYPE *tp = Alloc(sizeof(TYPE));
                                    tp->type = bt_class;
                                    tp->sp = params->p->sym;
                                    tp->size = tp->sp->tp->size;
                                    params->p->byClass.temp = tp;
                                    lst->data = tp;
                                    lst->next = types;
                                    types = lst;                        
                                }
                                break;
                            case kw_template:
                                params->p->byTemplate.temp = params->p->sym;
                                break;
                            case kw_int:
                                if (exprchk)
                                {
                                    params->p->byNonType.temp = (EXPRESSION *)exprchk->data;
                                    exprchk = exprchk->next;
                                    
                                }
                                else
                                {
                                    LIST *lst = Alloc(sizeof(LIST));
                                    EXPRESSION *exp = intNode(en_c_i, 47);
                                    params->p->byNonType.temp = exp;
                                    lst->data = exp;
                                    lst->next = exprs;
                                    exprs = lst; 
                                }
                                break;
                        }
                        params = params->next;
                    }
                    if (isstructured(sym->tp))
                        typetab[i] = SynthesizeTemplate(sym->tp, TRUE);
                    else
                        typetab[i] = SynthesizeType(sym->tp, NULL, TRUE);
                }
            }
        }
        for (i=0; i < count-1; i++)
        {
            if (table[i])
            {
                for (j=i+1; table[i] && j < count; j++)
                {
                    if (table[j])
                    {
                        int which = TemplatePartialDeduceArgsFromType(asClass ? table[i] : table[i]->parentTemplate, 
                                                                      asClass ? table[j] : table[j]->parentTemplate,
                                                                      typetab[i], typetab[j], funcparams);
                        if (which < 0)
                        {
                            table[i] = 0;
                        }
                        else if (which > 0)
                        {
                            table[j] = 0;
                        }
                    }
                }
            }
        }
        if (save)
            restoreParams(table, count);
    }
}
static BOOLEAN TemplateInstantiationMatchInternal(TEMPLATEPARAMLIST *porig, TEMPLATEPARAMLIST *psym)
{
    if (porig && psym)
    {
        BOOLEAN origdflt = TRUE;
        BOOLEAN symdflt = TRUE;
        if (porig->p->bySpecialization.types)
        {
            porig = porig->p->bySpecialization.types;
        }
        else
        {
            porig = porig->next;
            origdflt = FALSE;
        }

        if (psym->p->bySpecialization.types)
        {
            psym = psym->p->bySpecialization.types;
        }
        else
        {
            psym = psym->next;
            symdflt = FALSE;
        }
        while (porig && psym)
        {
            void *xorig, *xsym;
            xorig = origdflt ? porig->p->byClass.dflt : porig->p->byClass.val;
            xsym = symdflt ? psym->p->byClass.dflt : psym->p->byClass.val;
            switch (porig->p->type)
            {
                case kw_typename:
                {
                    if (porig->p->packed != psym->p->packed)
                        return FALSE;
                    if (porig->p->packed)
                    {
                        TEMPLATEPARAMLIST *packorig = porig->p->byPack.pack;
                        TEMPLATEPARAMLIST *packsym = psym->p->byPack.pack;
                        while (packorig && packsym)
                        {
                            TYPE *torig = (TYPE *)xorig;
                            TYPE *tsym =  (TYPE *)xsym;
                            if (!templatecomparetypes(torig, tsym, TRUE))
                                return FALSE;
                            
                            packorig = packorig->next;
                            packsym = packsym->next;
                        }
                        if (packorig || packsym)
                            return FALSE;
                    }
                    else
                    {
                        TYPE *torig = (TYPE *)xorig;
                        TYPE *tsym =  (TYPE *)xsym;
                        if (!templatecomparetypes(torig, tsym, TRUE))
                            return FALSE;
                    }
                    break;
                }
                case kw_template:
                    if (xorig != xsym)
                        return FALSE;
                    break;
                case kw_int:
                    if (!templatecomparetypes(porig->p->byNonType.tp, psym->p->byNonType.tp, TRUE))
                        return FALSE;
#ifndef PARSER_ONLY
                    if (!equalnode((EXPRESSION *)xorig, (EXPRESSION *)xsym))
                        return FALSE;
#endif
                    break;
            }
            porig = porig->next;
            psym = psym->next;
        }
        return TRUE;
    }
    return !porig && !psym;
}
static BOOLEAN TemplateInstantiationMatch(SYMBOL *orig, SYMBOL *sym)
{
    if (orig->parentTemplate == sym->parentTemplate)
    {
        if (!TemplateInstantiationMatchInternal(orig->templateParams, sym->templateParams))
            return FALSE;
        while (orig->parentClass && sym->parentClass)
        {
            orig = orig->parentClass;
            sym = sym->parentClass;
        }
        if (orig->parentClass || sym->parentClass)
            return FALSE;
        if (!TemplateInstantiationMatchInternal(orig->templateParams, sym->templateParams))
            return FALSE;
        return TRUE;
    }
}
static void TemplateTransferClassDeferred(SYMBOL *newCls, SYMBOL *tmpl)
{
    HASHREC *ns = newCls->tp->syms->table[0];
    HASHREC *os = tmpl->tp->syms->table[0];
    while (ns && os)
    {
        SYMBOL *ss = (SYMBOL *)ns->p;
        SYMBOL *ts = (SYMBOL *)os->p;
        if (ss->tp->type != ts->tp->type && ts->tp->type != bt_templateparam && ts->tp->type != bt_templateselector)
        {
            diag("TemplateClassFunctionInstantiate invalid signature match");
            break;
        }
        else if (ss->tp->type == bt_aggregate && ts->tp->type == bt_aggregate)
        {
            HASHREC *os2 = ts->tp->syms->table[0];
            HASHREC *ns2 = ss->tp->syms->table[0];
            // these lists may be mismatched, in particular the old symbol table
            // may have partial specializations for templates added after the class was defined...
            while (ns2 && os2)
            {
                SYMBOL *ts2 = (SYMBOL *)os2->p;
                SYMBOL *ss2 = (SYMBOL *)ns2->p;
                HASHREC *tsf = basetype(ts2->tp)->syms->table[0];
                HASHREC *ssf = basetype(ss2->tp)->syms->table[0];
                while (tsf && ssf)
                {
                    ssf->p->name = tsf->p->name;
                    tsf = tsf->next;
                    ssf = ssf->next;
                }
                ss2->deferredCompile = ts2->deferredCompile;
                ns2 = ns2->next;
                os2 = os2->next;
            }
        }
        else
        {
            ss->deferredCompile = ts->deferredCompile;
        }
        ns = ns->next;
        os = os->next;
    }
}
static BOOLEAN ValidSpecialization( TEMPLATEPARAMLIST *special, TEMPLATEPARAMLIST *args, BOOLEAN templateMatch)
{
    while (special && args)
    {
        if (special->p->type != args->p->type)
        {
            if (args->p->type != kw_typename || args->p->byClass.dflt->type != bt_templateselector)
                return FALSE;
        }
        if (!templateMatch)
        {
            if (special->p->byClass.val && !args->p->byClass.dflt || !special->p->byClass.val && args->p->byClass.dflt)
                return FALSE;
            switch (args->p->type)
            {
                case kw_typename:
                    if (args->p->byClass.dflt && !templatecomparetypes(special->p->byClass.val, args->p->byClass.dflt, TRUE))
                        return FALSE;
                    break;
                case kw_template:
                    if (args->p->byTemplate.dflt && !ValidSpecialization(special->p->byTemplate.args, args->p->byTemplate.dflt, TRUE));
                        return FALSE;
                    break;
                case kw_int:
                    if (!templatecomparetypes(special->p->byNonType.tp, args->p->byNonType.tp, TRUE))
                        return FALSE;
                    break;
            }
        }
        special = special->next;
        args = args->next;
    }
    return (!special || special->p->byClass.txtdflt) && !args;
}
static SYMBOL *MatchSpecialization(SYMBOL *sym, TEMPLATEPARAMLIST *args)
{
    LIST *lst;
    if (sym->specialized)
    {
        if (ValidSpecialization(sym->templateParams->p->bySpecialization.types, args, FALSE))
            return sym;
    }
    else
    {
        if (ValidSpecialization(sym->templateParams->next, args, TRUE))
            return sym;
    }
    return NULL;
}
static int pushContext(SYMBOL *cls, BOOLEAN all)
{
    STRUCTSYM *s;
    int rv;
    if (!cls)
        return 0;
    rv = pushContext(cls->parentClass, TRUE);
    if (cls->templateLevel)
    {
        s = Alloc(sizeof(STRUCTSYM));
        s->tmpl = copyParams(cls->templateParams, FALSE);
        addTemplateDeclaration(s);
        rv++;
    }
    if (all)
    {
        s = Alloc(sizeof(STRUCTSYM));
        s->str = cls;
        addStructureDeclaration(s);
        rv++;
    }
    return rv;
}
SYMBOL *TemplateClassInstantiateInternal(SYMBOL *sym, TEMPLATEPARAMLIST *args, BOOLEAN isExtern)
{
    LEXEME *lex;
    SYMBOL *cls= sym;
    int pushCount;
    if (cls->linkage == lk_inline)
        return cls;
    if (!isExtern)
    {
        lex = sym->deferredCompile;
        if (!lex)
            lex = sym->parentTemplate->deferredCompile;
        if (lex)
        {
            LIST *oldDeferred = deferred;
            int oldStructLevel = structLevel;
            BOOLEAN defd = FALSE;
            SYMBOL old;
//                STRUCTSYM s;
            struct templateListData l;
            deferred = NULL;
            structLevel = 0;
            old = *cls;
            cls->linkage = lk_inline;
//            if (cls->tp->syms)
//                cls = clonesym(cls);
            cls->parentClass = SynthesizeParentClass(cls->parentClass);
            pushCount = pushContext(cls, FALSE);
            cls->linkage = lk_inline;
            cls->tp = Alloc(sizeof(TYPE));
            *cls->tp = *old.tp;
            cls->tp->syms = NULL;
            cls->tp->sp = cls;
            cls->baseClasses = NULL;
            instantiatingTemplate++;
            lex = SetAlternateLex(lex);
            lex = innerDeclStruct(lex, NULL, cls, NULL, cls->tp->type == bt_class ? ac_private : ac_public, cls->isfinal, &defd);
            FinishStruct(cls, NULL);
            SetAlternateLex(NULL);
            lex = sym->deferredCompile;
            while (lex)
            {
                lex->registered = FALSE;
                lex = lex->next;
            }
            if (old.tp->syms)
                TemplateTransferClassDeferred(cls, &old);
            /*
            s.str = cls;
            addStructureDeclaration(&s);
            TemplateClassFunctionInstantiate(cls, sym);
            dropStructureDeclaration();
            */
            instantiatingTemplate --;
            deferred = oldDeferred;
            structLevel = oldStructLevel;
            cls->instantiated = TRUE;
            cls->genreffed = TRUE;
            while (pushCount--)
                dropStructureDeclaration();
        }
        else
        {
//            errorsym(ERR_TEMPLATE_CANT_INSTANTIATE_NOT_DEFINED, sym->parentTemplate);
        }
    }
    return cls;
}
SYMBOL *TemplateClassInstantiate(SYMBOL *sym, TEMPLATEPARAMLIST *args, BOOLEAN isExtern, enum e_sc storage_class)
{
    if (templateNestingCount)
    {
        SYMBOL *sym1 = MatchSpecialization(sym, args);
        if (sym1 && (storage_class == sc_parameter || !inTemplateBody))
        {
            TEMPLATEPARAMLIST *tpm = Alloc(sizeof(TEMPLATEPARAMLIST));
            TYPE **tpx, *tp = sym1->tp;
            tpm->p = Alloc(sizeof(TEMPLATEPARAM));
            tpm->p->type = kw_new;
            tpm->next = args;
            sym1 = clonesym(sym1);
            sym1->templateParams = tpm;
            tpx = &sym1->tp;
            while (tp)
            {
                *tpx = Alloc(sizeof(TYPE));
                **tpx = *tp;
                if (!tp->btp)
                {
                    (*tpx)->sp = sym1;
                    (*tpx)->templateParam = tpm;
                }
                else
                {
                    tpx = &(*tpx)->btp;
                }
                tp = tp->btp;
            }
        }
        return sym1;
    }
    else
    {
        return TemplateClassInstantiateInternal(sym, args, isExtern);
    }
}
void TemplateDataInstantiate(SYMBOL *sym, BOOLEAN warning, BOOLEAN isExtern)
{
    if (!sym->gentemplate)
    {
        if (isExtern)
        {
            sym->storage_class = sc_external;
            InsertExtern(sym);
        }
        else
        {
            InsertInlineData(sym);
            sym->genreffed = TRUE;
        }
        sym->gentemplate = TRUE;
    }
    else if (warning)
    {
        errorsym(ERR_TEMPLATE_ALREADY_INSTANTIATED, sym);
    }
}
SYMBOL *TemplateFunctionInstantiate(SYMBOL *sym, BOOLEAN warning, BOOLEAN isExtern)
{
    STRUCTSYM *old;
    HASHREC *hr;
    TEMPLATEPARAMLIST *params = sym->templateParams;
    LEXEME *lex;
    SYMBOL *push;
    int pushCount ;
    BOOLEAN found = FALSE;
    STRUCTSYM s;
    hr = sym->overloadName->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *data = (SYMBOL *)hr->p;
        if (data->instantiated && TemplateInstantiationMatch(data, sym) && matchOverload(sym, data))
        {
            sym = data;
            if (sym->linkage == lk_inline || isExtern)
                return sym;
            found = TRUE;
            break;
        }
        hr = hr->next;
    }
    old = structSyms;
    structSyms = 0;
    sym->templateParams = copyParams(sym->templateParams, TRUE);
    sym->instantiated = TRUE;
    SetLinkerNames(sym, lk_cdecl);
    sym->gentemplate = TRUE;
    pushCount = pushContext(sym->parentClass, TRUE);
    s.tmpl = sym->templateParams;
    addTemplateDeclaration(&s);
    pushCount++;
    if (!found)
    {
        insertOverload(sym, sym->overloadName->tp->syms);
        if (sym->storage_class == sc_member || sym->storage_class == sc_virtual)
        {
            injectThisPtr(sym, basetype(sym->tp)->syms);
        }
    }
    if (!isExtern)
    {
        lex = sym->deferredCompile;
        if (lex)
        {
            if (sym->templateNameSpace && !sym->templateNameSpace->value.i)
            {
                SYMBOL *sp = sym->templateNameSpace;
                LIST *list;
                sp->value.i++;
                list = Alloc(sizeof(LIST));
                list->next = globalNameSpace->childNameSpaces;
                list->data = sp;
                globalNameSpace->childNameSpaces = list;
            
                list = Alloc(sizeof(LIST));
                list->next = nameSpaceList;
                list->data = sp;
                nameSpaceList = list;
                
                sp->nameSpaceValues->next = globalNameSpace;
                globalNameSpace = sp->nameSpaceValues;
            }
            if (sym->storage_class != sc_member && sym->storage_class != sc_mutable)
                sym->storage_class = sc_global;
            sym->linkage = lk_inline;
            sym->xc = NULL;
            instantiatingTemplate++;
    //        backFillDeferredInitializersForFunction(sym, sym);
            lex = SetAlternateLex(sym->deferredCompile);
            if (MATCHKW(lex, kw_try) || MATCHKW(lex, colon))
            {
                BOOLEAN viaTry = MATCHKW(lex, kw_try);
                int old = GetGlobalFlag();
                if (viaTry)
                {
                    sym->hasTry = TRUE;
                    lex = getsym();                                
                }
                if (MATCHKW(lex, colon))
                {
                    lex = getsym();                                
                    sym->memberInitializers = GetMemberInitializers(&lex, NULL, sym);
                }
            }
            lex = body(lex, sym);
            lex = sym->deferredCompile;
            while (lex)
            {
                lex->registered = FALSE;
                lex = lex->next;
            }
            SetAlternateLex(NULL);
            if (sym->templateNameSpace && !sym->templateNameSpace->value.i)
            {
                sym->templateNameSpace->value.i--;
                nameSpaceList = nameSpaceList->next;
                globalNameSpace = globalNameSpace->next;
            }
            instantiatingTemplate --;
            sym->genreffed |= warning;
        }
        else
        {
            sym->storage_class = sc_external;
            InsertExtern(sym);
        }
    }
    while (pushCount--)
        dropStructureDeclaration();
    structSyms = old;
    return sym;
}
static BOOLEAN CheckConstCorrectness(TYPE *P, TYPE *A, BOOLEAN byClass)
{
    int pn=0, an=0;
    TYPE *q = P;
    int i;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        pn++;
    }
    q = A;
    while (ispointer(q))
    {
        q = basetype(q)->btp;
        an++;
    }
    if (pn < an)
    {
        for (i=0; i < an - pn; i++)
            A = basetype(A)->btp;
    }
    else if (pn > an)
    {
        return FALSE;
        for (i=0; i < pn - an; i++)
            P = basetype(P)->btp;
    }
    while (P && A)
    {
        if (byClass)
        {
            if (isconst(A) != isconst(P) || isvolatile(A) != isvolatile(P))
                return FALSE;
        }
        else
        {
            if (isconst(A) && !isconst(P) || isvolatile(A) && !isvolatile(P))
                return FALSE;
        }
        P = basetype(P)->btp;
        A = basetype(A)->btp;
    }
    return TRUE;
}
static void TemplateConstOrdering(SYMBOL **spList, int n, TEMPLATEPARAMLIST *params)
{
    int i;
    for (i=0; i < n; i++)
        if (spList[i])
        {
            TEMPLATEPARAMLIST *P = spList[i]->templateParams->p->bySpecialization.types;
            while (P)
            {
                if (P->p->type != kw_typename)
                {
                    spList[i] = 0;
                    break;
                }
                if (!CheckConstCorrectness(P->p->byClass.dflt, P->p->byClass.val, TRUE))
                {
                    spList[i] = 0;
                    break;
                }
                P = P->next;
            }
        }
}
static SYMBOL *ValidateClassTemplate(SYMBOL *sp, TEMPLATEPARAMLIST *unspecialized, TEMPLATEPARAMLIST *args)

{
    SYMBOL *rv = NULL;
    TEMPLATEPARAMLIST *nparams = sp->templateParams;
    if (nparams)
    {
        TEMPLATEPARAMLIST *spsyms = nparams->p->bySpecialization.types;
        TEMPLATEPARAMLIST *params = spsyms ? spsyms : nparams->next, *origParams = params;
        TEMPLATEPARAMLIST *primary = spsyms ? spsyms : nparams->next;
        TEMPLATEPARAMLIST *initial = args;
        rv = sp;
        ClearArgValues(params);
        ClearArgValues(spsyms);
        ClearArgValues(sp->templateParams);
        while (initial && params)
        {
            void *dflt = initial->p->byClass.dflt;
            TEMPLATEPARAMLIST *test = initial;
            if (!dflt)
                dflt = initial->p->byClass.val;
            while (test->p->type == kw_typename && dflt && ((TYPE *)dflt)->type == bt_templateparam)
            {
                test = test->p->byClass.dflt->templateParam;
                dflt = test->p->byClass.val;
                if (!dflt)
                    dflt = test->p->byClass.val;
            }
            if (test->p->type != params->p->type)
            {
                if (test->p->type != kw_typename || test->p->byClass.dflt->type != bt_templateselector)
                    rv = NULL;
                params = params->next;
            }   
            else 
            {
                if (params->p->packed)
                {
                    TEMPLATEPARAMLIST *nparam = Alloc(sizeof(TEMPLATEPARAMLIST));
                    TEMPLATEPARAMLIST **p = & params->p->byPack.pack;
                    nparam->p = Alloc(sizeof(TEMPLATEPARAM));
                    while (*p)
                        p = &(*p)->next;
                    nparam->p->type = params->p->type;
                    nparam->p->byClass.val = dflt;
                    *p = nparam;
                    params->p->initialized = TRUE;
                }
                else
                {
                    if (test->p->type == kw_template)
                    {
                        if (dflt && !exactMatchOnTemplateParams(((SYMBOL *)dflt)->templateParams->next, params->p->byTemplate.args))
                            rv = NULL;
                    }
                    params->p->byClass.val = dflt;
                    if (spsyms)
                        if (params->p->type == kw_typename)
                        {
                            if (!Deduce(params->p->byClass.dflt, params->p->byClass.val, TRUE, TRUE))
                                rv = NULL;
                        }
                        else if (params->p->type == kw_template)
                        {
                            if (params->p->byClass.dflt->type == bt_templateparam)
                            {
                                if (!DeduceTemplateParam(params->p->byClass.dflt->templateParam, NULL, params->p->byTemplate.dflt->tp, TRUE))
                                    rv = NULL;
                            }
                            else
                            {
                                rv = NULL;
                            }
                        }
                    params->p->initialized = TRUE;
                    params = params->next;
                    primary = primary->next;
                }
            }
            initial = initial->next;
        }
        if (!templateNestingCount)
        {
            params = origParams;
            primary = spsyms ? spsyms : nparams->next;
            if (!TemplateParseDefaultArgs(params, primary, nparams->next))
                rv = NULL;
            while (params && primary)
            {
                if (!primary->p->byClass.val && !primary->p->packed)
                {
                    rv = NULL;
                    break;
                }
                primary = primary->next;
                params = params->next;
            }
            if (params)
                rv = NULL;
        }
    }
    return rv;
}
SYMBOL *GetClassTemplate(SYMBOL *sp, TEMPLATEPARAMLIST *args, BOOLEAN isExtern, enum e_sc storage_class)
{
    int n = 1, i=0;
    TEMPLATEPARAMLIST *unspecialized = sp->templateParams->next;
    SYMBOL *found1 = NULL, *found2 = NULL;
    SYMBOL **spList, **origList;
    int count;
    LIST *l = sp->specializations;
    if (sp->parentTemplate)
        sp = sp->parentTemplate;
    while (l)
    {
        n++;
        l = l->next;
    }
    spList = Alloc(sizeof(SYMBOL *) * n);
    origList = Alloc(sizeof(SYMBOL *) * n);
    origList[i++] = sp;
    l = sp->specializations;
    while (i < n)
    {
        origList[i++] = (SYMBOL *)l->data;
        l = l->next;
    }
    saveParams(origList, n);
    for (i=0; i < n; i++)
    {
        spList[i] = ValidateClassTemplate(origList[i], unspecialized, args);
    }
    for (i=0,count=0; i < n; i++)
    {
        if (spList[i])
            count++;
    }
    if (count > 1)
    {
        TemplatePartialOrdering(spList, n, NULL, NULL, TRUE, FALSE);
        if (!spList[0])
        {
            int count = 0;
            for (i=0; i < n; i++)
                if (spList[i])
                    count++;
            if (count > 1)
                TemplateConstOrdering(spList, n, args);
        }
    }
    for (i=0; i < n && !found1; i++)
    {
        int j;
        found1 = spList[i];
        for (j=i+1; j < n && found1 && !found2; j++)
        {
            if (spList[j])
            {
                found2 = spList[j];
            }
        }
    }
    if (count > 1 && found1 && !found2)
    {
        found1 = ValidateClassTemplate(origList[i-1], unspecialized, args);
    }
    if (!found1 && !templateNestingCount)
    {
        TEMPLATEPARAMLIST *orig = sp->templateParams->next;
        while (orig && args)
        {
            orig = orig->next;
            args = args->next;
        }
        if (orig)
        {
            errorsym(ERR_TOO_FEW_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else if (args)
        {
            errorsym(ERR_TOO_MANY_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else
        {
            errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
            // might get more error info by procedeing;
            if (!sp->specializations)
            {
                TEMPLATEPARAMLIST *params = sp->templateParams->next;
                while (params)
                {
                    if (!params->p->byClass.val)
                        break;
                    params = params->next;
                }
                if (!params)
                    found1 = sp;
            }
        }
    }
    else if (found2)
    {
        errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        return NULL;
    }
    if (found1 && !found2 && !templateNestingCount)
    {
        SYMBOL *sym = found1;
        SYMBOL *parent = found1->parentTemplate;
        TEMPLATEPARAMLIST *dflts;
        TEMPLATEPARAMLIST *orig;
        LIST *instants = parent->instantiations;
        while (instants)
        {
            if (TemplateInstantiationMatch(instants->data, found1))
            {
                return (SYMBOL *)instants->data;
            }
            instants = instants->next;
        }
    
        found1 = clonesym(found1);
        found1->tp = Alloc(sizeof(TYPE));
        *found1->tp = *sym->tp;
        found1->tp->sp = found1;
        found1->gentemplate = TRUE;
        found1->instantiated = TRUE;
        found1->performedDeferred = FALSE;
        found1->templateParams = copyParams(found1->templateParams, TRUE);
        if (found1->templateParams->p->bySpecialization.types)
        {
            TEMPLATEPARAMLIST **pptr = &found1->templateParams->p->bySpecialization.types;
            TEMPLATEPARAMLIST *params = *pptr;
            while (params)
            {
                *pptr = Alloc(sizeof(TEMPLATEPARAMLIST));
                if (params->p->type == kw_typename)
                {
                    (*pptr)->p = Alloc(sizeof(TEMPLATEPARAM));
                    *(*pptr)->p = *params->p;
                    (*pptr)->p->byClass.dflt = SynthesizeType(params->p->byClass.dflt, sp->templateParams, FALSE);
                }
                else
                {
                    (*pptr)->p = params->p;
                }
                params = params->next;
                pptr = &(*pptr)->next;
            }
        }
        SetLinkerNames(found1, lk_cdecl);
        instants = Alloc(sizeof(LIST));
        instants->data = found1;
        instants->next = parent->instantiations;
        parent->instantiations = instants;
    }
    if (found1 && !found2 && storage_class != sc_none)
    {
        found1 = TemplateClassInstantiate(found1, args, isExtern, storage_class);
    }
    restoreParams(origList, n);
    return found1;
}
void DoInstantiateTemplateFunction(TYPE *tp, SYMBOL **sp, NAMESPACEVALUES *nsv, SYMBOL *strSym, TEMPLATEPARAMLIST *templateParams, BOOLEAN isExtern)
{
    SYMBOL *sym = *sp;
    SYMBOL *spi, *ssp;
    HASHREC **p = NULL;
    if (nsv)
    {
        LIST *rvl = tablesearchone(sym->name, nsv, FALSE);
        if (rvl)
            spi = (SYMBOL *)rvl->data;
        else
            errorNotMember(strSym, nsv, sym->name);
    }
    else {
        ssp = getStructureDeclaration();
        if (ssp)
            p = LookupName(sym->name, ssp->tp->syms);
        if (!p)
            p = LookupName(sym->name, globalNameSpace->syms);
        if (p)
        {
            spi = (SYMBOL *)(*p)->p;
        }
    }
    if (spi)
    {
        if (spi->storage_class == sc_overloads)
        {
            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
            SYMBOL *instance;
            HASHREC *hr = basetype(tp)->syms->table[0];
            INITLIST **init = &funcparams->arguments;
            funcparams->templateParams = templateParams->p->bySpecialization.types;
            funcparams->ascall = TRUE;
            if (templateParams->p->bySpecialization.types)
                funcparams->astemplate = TRUE;
            if (((SYMBOL *)hr->p)->thisPtr)
                hr = hr->next;
            while (hr)
            {
                *init = Alloc(sizeof(INITLIST));
                (*init)->tp = ((SYMBOL *)hr->p)->tp;
                hr = hr->next;
            }
            instance = GetOverloadedTemplate(spi, funcparams);
            if (instance)
            {
                    
                instance = TemplateFunctionInstantiate(instance, TRUE, isExtern);
                if (!comparetypes(sym->tp, instance->tp, TRUE))
                    preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, instance, sym->declfile, sym->declline);
                *sp = instance;
            }
            else if (!templateNestingCount)
            {
                errorsym(ERR_NOT_A_TEMPLATE, sym);
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
}
LEXEME *TemplateDeclaration(LEXEME *lex, SYMBOL *funcsp, enum e_ac access, enum e_sc storage_class, BOOLEAN isExtern)
{
    lex = getsym();
    if (MATCHKW(lex, lt))
    {
        int lasttemplateHeaderCount = templateHeaderCount;
        TEMPLATEPARAMLIST **tap; // for the specialization list
        TYPE *tp = NULL;
        struct templateListData l;
        int count = 0;
            extern INCLUDES *includes;
        lex = backupsym();
        if (isExtern)
            error(ERR_DECLARE_SYNTAX);

        if (templateNestingCount == 0)
        {
            l.args = NULL;
            l.ptail = &l.args;
            l.sym = NULL;
            l.head = l.tail = NULL;
            l.bodyHead = l.bodyTail = NULL;
            currents = &l;
        }
        currents->plast = currents->ptail;
        while (MATCHKW(lex, kw_template)) 
        {
            templateHeaderCount++;
            (*currents->ptail) = Alloc(sizeof(TEMPLATEPARAMLIST));
            (*currents->ptail)->p = Alloc(sizeof(TEMPLATEPARAM));
            (*currents->ptail)->p->type = kw_new;
            lex = getsym();
            lex = TemplateHeader(lex, funcsp, &(*currents->ptail)->next); 
            if ((*currents->ptail)->next)
            {
                STRUCTSYM *s = Alloc(sizeof(STRUCTSYM));
                s->tmpl = (*currents->ptail)->next;
                addTemplateDeclaration(s);
                count ++;
            }
            currents->ptail = &(*currents->ptail)->p->bySpecialization.next;
        }
        if (MATCHKW(lex, kw_friend))
        {
            lex = getsym();
            templateNestingCount++;
            inTemplateType = TRUE;
            lex = declare(lex, NULL, NULL, sc_global, lk_none, NULL, TRUE, FALSE, TRUE, TRUE, access);
            inTemplateType = FALSE;
            templateNestingCount--;
        }
        else if (lex)
        {
            templateNestingCount++;
            inTemplateType = TRUE;
            lex = declare(lex, funcsp, &tp, storage_class, lk_none, NULL, TRUE, FALSE, FALSE, TRUE, access);
            inTemplateType = FALSE;
            templateNestingCount--;
            if (!templateNestingCount)
            {
                if (!tp)
                {
                    error(ERR_TEMPLATES_MUST_BE_CLASSES_OR_FUNCTIONS);
                }
                else if (!isfunction(tp) && !isstructured(tp) )
                {
                    if (!l.sym || !l.sym->parentClass || ismember(l.sym))
                    {
                        error(ERR_TEMPLATES_MUST_BE_CLASSES_OR_FUNCTIONS);
                    }
                    else 
                    {
                        SYMBOL *s = l.sym->parentClass;
                        while (s)
                            if (s->specialized)
                                break;
                            else
                                s = s->parentClass;
                        if (s)
                        {
                            errorsym(ERR_INVALID_EXPLICIT_SPECIALIZATION, l.sym);
                        }
                    }
                }
                FlushLineData("", INT_MAX);
            }
        }
        while (count--)
            dropStructureDeclaration();
        templateHeaderCount = lasttemplateHeaderCount;
        (*currents->plast) = NULL;
        currents->ptail = currents->plast;
        if (templateNestingCount == 0)
            currents = NULL;
    }
    else // instantiation
    {
        SYMBOL *sym = NULL;
        enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
        TYPE *tp = NULL;
        BOOLEAN defd = FALSE;
        BOOLEAN notype = FALSE;
        NAMESPACEVALUES *nsv = NULL;
        SYMBOL *strSym = NULL;
        STRUCTSYM s;
        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
        lex = getBasicType(lex, funcsp, &tp, &strSym, TRUE, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, FALSE);
        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
        lex = getBeforeType(lex, funcsp, &tp, &sym, &strSym, &nsv, TRUE, sc_cast, &linkage, &linkage2, &linkage3, FALSE, FALSE, FALSE);
        sizeQualifiers(tp);
        if (strSym)
        {
            s.str = strSym;
            addStructureDeclaration(&s);
            
        }
        if (notype)
        {
            error(ERR_TYPE_NAME_EXPECTED);
        }
        else if (isfunction(tp))
        {
            SYMBOL *sp = sym;
            TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sym);
            DoInstantiateTemplateFunction(tp, &sp, nsv, strSym, &templateParams, isExtern);
            sym = sp;
            if (isExtern)
            {
                insertOverload(sym, sym->overloadName->tp->syms);
                sym->storage_class = sc_external;
                InsertExtern(sym);
            }
        }
        else if (isstructured(tp))
        {
            if (sym && !sym->anonymous)
            {
                error(ERR_TOO_MANY_IDENTIFIERS);
            }
            else 
            {
                sym = basetype(tp)->sp;
                if (!sym->templateLevel)
                {
                    errorsym(ERR_NOT_A_TEMPLATE, sym);
                }
                else
                {
                    TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sym);
                    SYMBOL *instance = GetClassTemplate(sym, templateParams->p->bySpecialization.types, FALSE, sc_global);
                    if (instance)
                    {
                        if (isExtern)
                            error(ERR_EXTERN_NOT_ALLOWED);
                    }    
                    else
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, sym);
                    }                
                }
            }
        }
        else
        {
            SYMBOL *spi=NULL, *ssp;
            HASHREC **p = NULL;
            if (nsv)
            {
                LIST *rvl = tablesearchone(sym->name, nsv, FALSE);
                if (rvl)
                    spi = (SYMBOL *)rvl->data;
                else
                    errorNotMember(strSym, nsv, sym->name);
            }
            else {
                ssp = getStructureDeclaration();
                if (ssp)
                    p = LookupName(sym->name, ssp->tp->syms);				
                else
                    p = LookupName(sym->name, globalNameSpace->syms);
                if (p)
                {
                    spi = (SYMBOL *)(*p)->p;
                }
            }
            if (spi)
            {
                SYMBOL *tmpl = spi;
                while (tmpl)
                    if (tmpl->templateLevel)
                        break;
                    else
                        tmpl = tmpl->parentClass;
                if (tmpl && spi->storage_class == sc_static || spi->storage_class == sc_external)
                {
                    TemplateDataInstantiate(spi, TRUE, isExtern);
                    if (!comparetypes(sym->tp, spi->tp, TRUE))
                        preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, spi, sym->declfile, sym->declline);
                }
                else
                {
                    errorsym(ERR_NOT_A_TEMPLATE, sym);
                }                    
            }
            else
            {
                errorsym(ERR_NOT_A_TEMPLATE, sym);
            }
        }
        if (strSym)
        {
            dropStructureDeclaration();
        }
    }
    return lex;
}