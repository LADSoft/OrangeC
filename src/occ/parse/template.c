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
extern LIST *nameSpaceList;
extern INCLUDES *includes;
extern int inDefaultParam;
extern LINEDATA *linesHead, *linesTail;

int dontRegisterTemplate;
int instantiatingTemplate;
int inTemplateBody;
int templateNestingCount =0 ;
int templateHeaderCount ;
int inTemplateSpecialization = 0;
BOOLEAN inTemplateType;

static int inTemplateArgs;

struct templateListData *currents;

static LEXEME *TemplateArg(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST *arg, TEMPLATEPARAMLIST **lst);
static BOOLEAN fullySpecialized(TEMPLATEPARAMLIST *tpl);
static TEMPLATEPARAMLIST *copyParams(TEMPLATEPARAMLIST *t, BOOLEAN alsoSpecializations);

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
    inTemplateSpecialization = 0;
}
BOOLEAN equalTemplateIntNode(EXPRESSION *exp1, EXPRESSION *exp2)
{
#ifdef PARSER_ONLY
    return TRUE;
#else
    if (equalnode(exp1, exp2))
        return TRUE;
    if (isintconst(exp1) && isintconst(exp2) && exp1->v.i == exp2->v.i)
        return TRUE;
    return FALSE;
#endif
}
BOOLEAN templatecomparetypes(TYPE *tp1, TYPE *tp2, BOOLEAN exact)
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
        params = (TEMPLATEPARAMLIST *)(*currents->plast);
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
                {
                    TYPE *ts = sym->p->byClass.dflt;
                    TYPE *to = old->p->byClass.dflt;
                    if (isref(ts))
                        ts = basetype(ts)->btp;
                    if (isref(to))
                        to = basetype(to)->btp;
                    if (isconst(ts) != isconst(to))
                        return FALSE;
                    if (isvolatile(ts) != isvolatile(to))
                        return FALSE;
                }
                break;
            case kw_template:
                if (old->p->byTemplate.dflt != sym->p->byTemplate.dflt)
                    return FALSE;
                break;
            case kw_int:
                if (!templatecomparetypes(old->p->byNonType.tp, sym->p->byNonType.tp, TRUE))
                    return FALSE;
#ifndef PARSER_ONLY
                if (!equalTemplateIntNode(old->p->byNonType.dflt, sym->p->byNonType.dflt))
                    return FALSE;
#endif
                break;
            default:
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
                {
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->p->sym);
                }
                else if (!sym->p->byTemplate.txtdflt)
                {
                    sym->p->byTemplate.txtdflt = old->p->byTemplate.txtdflt;
                    sym->p->byTemplate.txtargs = old->p->byTemplate.txtargs;
                }
                break;
            case kw_typename:
                if (old->p->byClass.txtdflt && sym->p->byClass.txtdflt)
                {
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->p->sym);
                }
                else if (!sym->p->byClass.txtdflt)
                {
                    sym->p->byClass.txtdflt = old->p->byClass.txtdflt;
                    sym->p->byClass.txtargs = old->p->byClass.txtargs;
                }
                break;
            case kw_int:
                if (old->p->byNonType.txtdflt && sym->p->byNonType.txtdflt)
                {
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->p->sym);
                }
                else if (!sym->p->byNonType.txtdflt)
                {
                    sym->p->byNonType.txtdflt = old->p->byNonType.txtdflt;
                    sym->p->byNonType.txtargs = old->p->byNonType.txtargs;
                }
                break;
            case kw_new: // specialization
                break;
            default:
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
            default:
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
            if (!comparetypes(param->p->byNonType.tp, special->p->byNonType.tp, FALSE) && (!ispointer(param->p->byNonType.tp) || !isconstzero(param->p->byNonType.tp, special->p->byNonType.dflt)))
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
        if (special->p->packed)
            special = NULL;
        else
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
            TEMPLATEPARAMLIST *transfer;
            matchTemplateSpecializationToParams(old->next, sym->p->bySpecialization.types, sp);
            rv = sym;
            transfer = sym->p->bySpecialization.types;
            old = old->next;
            while (old && transfer && !old->p->packed)
            {
                transfer->p->sym = old->p->sym;
                transfer->p->byClass.txtdflt = old->p->byClass.txtdflt;
                transfer->p->byClass.txtargs = old->p->byClass.txtargs;
                transfer = transfer->next;
                old = old->next;
            }
        }
        else if (!exactMatchOnTemplateParams(old->next, sym->next))
        {
            error(ERR_TEMPLATE_DEFINITION_MISMATCH);
        }
        else 
        {
            rv = mergeTemplateDefaults(old, sym, definition);
            checkTemplateDefaults(rv);
            
            // have to also backpatch any instantiations that have already happened.
			/*
            if (definition)
            {
                LIST *lst = sp->instantiations;
                while (lst)
                {
                    SYMBOL *cur = (SYMBOL *)lst->data;
                    if (!cur->specialized)
                    {
                        TEMPLATEPARAMLIST *orig = sym;
                        TEMPLATEPARAMLIST *xnew = cur->templateParams;
                        while (orig && xnew)
                        {
                            if (orig->p->sym)
                            {
                                xnew->p->sym->name = orig->p->sym->name;
                                xnew->p->sym->decoratedName = orig->p->sym->decoratedName;
                                xnew->p->sym->errname = orig->p->sym->errname;
                            }
                            orig = orig->next;
                            xnew = xnew->next;
                        }
                    }
                    lst = lst->next;
                }
                
            }
			*/
        }
    }
    else
    {
        rv = sym;
        checkTemplateDefaults(sym->next);
    }
    checkMultipleArgs(sym->next);
    return rv;
}
BOOLEAN typeHasTemplateArg(TYPE *t);
static BOOLEAN structHasTemplateArg(TEMPLATEPARAMLIST *tpl)
{
    while (tpl)
    {
        if (tpl->p->type == kw_typename)
        {
            if (typeHasTemplateArg(tpl->p->byClass.dflt))
                return TRUE;
        }
        else if (tpl->p->type == kw_template)
        {
            if (structHasTemplateArg(tpl->p->byTemplate.args))
                return TRUE;
        }
        tpl = tpl->next;
    }
    return FALSE;
}
BOOLEAN typeHasTemplateArg(TYPE *t)
{
    if (t)
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
        else if (isstructured(t))
        {
            TEMPLATEPARAMLIST *tpl = basetype(t)->sp->templateParams;
            if (structHasTemplateArg(tpl))
                return TRUE;
        }
    }
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
        lex = nestedSearch(lex, &sp, NULL, NULL, NULL, NULL, FALSE, sc_global, FALSE);
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
LEXEME *GetTemplateArguments(LEXEME *lex, SYMBOL *funcsp, SYMBOL *templ, TEMPLATEPARAMLIST **lst)
{
    TEMPLATEPARAMLIST **start = lst;
    TEMPLATEPARAMLIST *orig = NULL;
    if (templ)
        orig = templ->templateParams ? (templ->templateParams->p->bySpecialization.types ? templ->templateParams->p->bySpecialization.types : templ->templateParams->next) : NULL;
    // entered with lex set to the opening <
    inTemplateArgs++;
    lex = getsym();
    if (!MATCHKW(lex, rightshift) && !MATCHKW(lex, gt))
    {
        do
        {
            TYPE *tp = NULL;
            if (startOfType(lex, TRUE) && (!orig || orig->p->type != kw_int))
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
                    inTemplateArgs--;
                    lex = get_type_id(lex, &tp, funcsp, FALSE);
                    inTemplateArgs++;
                    if (!templateNestingCount)
                        tp = PerformDeferredInitialization(tp, NULL);
                    if (MATCHKW(lex, ellipse))
                    {
                        lex = getsym();
                        if (templateNestingCount && tp->type == bt_templateparam)
                        {
                            *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                            (*lst)->p = tp->templateParam->p;
                        }
                        else
                        {
                            lst = expandArgs(lst, tp->templateParam, TRUE);
                        }
                    }
                    else if (tp && tp->type == bt_templateparam)
                    {
                        TEMPLATEPARAMLIST **last = lst;
                        lst = expandArgs(lst, tp->templateParam, FALSE);
                        if (inTemplateSpecialization)
                        {
                            while (*last)
                            {
                                if (!(*last)->p->byClass.dflt)
                                    (*last)->p->byClass.dflt = tp;
                                last = &(*last)->next;
                            }
                        }
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
                exp = NULL;
                tp = NULL;
                if (inTemplateSpecialization)
                {
                    TEMPLATEPARAMLIST **last = lst;
                    if (lex->type == l_id)
                    {
                        SYMBOL *sp;
                        LEXEME *last = lex;
                        lex = nestedSearch(lex, &sp, NULL, NULL, NULL, NULL, FALSE, sc_global, FALSE);
                        if (sp && sp->tp->templateParam)
                        {
                            lex = getsym();
                            *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                            (*lst)->p = sp->tp->templateParam->p;
                            lst = &(*lst)->next;
                        }
                        else
                        {
                            lex = prevsym(last);
                            goto join;
                        }
                    }
                    else
                    {
                        goto join;
                    }
                }
                else
                {
					STRUCTSYM *s;
					SYMBOL *name;
join:
					s = structSyms;
					name = NULL;
					if (ISID(lex))
					{
						while (s && !name)
						{
							if (s->tmpl)
								name = templatesearch(lex->value.s.a, s->tmpl);
							s = s->next;
						}
					}
					if (name)
					{
						lex = expression_no_comma(lex, funcsp, NULL, &tp, &exp, NULL, _F_INTEMPLATEPARAMS);
						*lst = Alloc(sizeof(TEMPLATEPARAMLIST));
						(*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
						(*lst)->p->type = kw_int;
						(*lst)->p->byNonType.dflt = exp;
						(*lst)->p->byNonType.tp = name->tp;
						lst = &(*lst)->next;
					}
					else
					{
						lex = expression_no_comma(lex, funcsp, NULL, &tp, &exp, NULL, _F_INTEMPLATEPARAMS);
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
                }
            }
            if (MATCHKW(lex, comma))
                lex = getsym();
            else
                break;
            if (orig)
                orig = orig->next;
        } while (TRUE);
    }
    if (MATCHKW(lex, rightshift))
        lex = getGTSym(lex);
    else
        needkw(&lex, gt);
    inTemplateArgs--;
    return lex;
}
static BOOLEAN sameTemplateSpecialization(TYPE *P, TYPE *A)
{
    TEMPLATEPARAMLIST *PL, *PA;
    if (!P || !A)
        return FALSE;
    P = basetype(P);
    A = basetype(A);
    if (isref(P))
        P = basetype(P->btp);
    if (isref(A))
        A = basetype(A->btp);
    if (!isstructured(P) || !isstructured(A))
        return FALSE;
    if (P->sp->parentClass != A->sp->parentClass || strcmp(P->sp->name, A->sp->name) != 0)
        return FALSE;
    if (P->sp->templateLevel != A->sp->templateLevel)
        return FALSE;
    // this next if stmt is a horrible hack.
    if (P->size == 0 &&!strcmp(P->sp->decoratedName, A->sp->decoratedName))
        return TRUE;
    PL= P->sp->templateParams;
    PA = A->sp->templateParams;
    if (!PL || !PA) // errors
        return FALSE;
    if (PL->p->bySpecialization.types || !PA->p->bySpecialization.types)
        return FALSE;
    PL = PL->next;
    PA = PA->p->bySpecialization.types;

    if (PL && PA)
    {
        while (PL && PA)
        {
            if (PL->p->type != PA->p->type)
            {
                break;
            }
            else if (P->sp->instantiated || A->sp->instantiated)
            {
                if (PL->p->type == kw_typename)
                {
                    if (!templatecomparetypes(PL->p->byClass.dflt, PA->p->byClass.val, TRUE))
                        break;
                }
                else if (PL->p->type == kw_template)
                {
                    if (!exactMatchOnTemplateParams(PL->p->byTemplate.args, PA->p->byTemplate.args))
                        break;
                }
                else if (PL->p->type == kw_int)
                {
                    if (!templatecomparetypes(PL->p->byNonType.tp, PA->p->byNonType.tp, TRUE))
                        break;
#ifndef PARSER_ONLY
                    if (!equalTemplateIntNode(PL->p->byNonType.dflt, PA->p->byNonType.val))
                        break;
#endif
                }
            }
            PL = PL->next;
            PA = PA->next;
        }
        return !PL && !PA;
    }
    return FALSE;
}
BOOLEAN exactMatchOnTemplateSpecialization(TEMPLATEPARAMLIST *old, TEMPLATEPARAMLIST *sym)
{
    while (old && sym)
    {
        if (old->p->type != sym->p->type)
            return FALSE;
        switch (old->p->type)
        {
            case kw_typename:
                if (!sameTemplateSpecialization(old->p->byClass.dflt, sym->p->byClass.val))
                {
                    if (!templatecomparetypes(old->p->byClass.dflt, sym->p->byClass.val, TRUE))
                        return FALSE;
                    if (!templatecomparetypes(sym->p->byClass.val, old->p->byClass.dflt, TRUE))
                        return FALSE;
                }
                break;
            case kw_template:
                if (old->p->byTemplate.dflt != sym->p->byTemplate.val)
                    return FALSE;
                break;
            case kw_int:
                if (!templatecomparetypes(old->p->byNonType.tp, sym->p->byNonType.tp, TRUE))
                    return FALSE;
#ifndef PARSER_ONLY
                if (!equalTemplateIntNode(old->p->byNonType.dflt, sym->p->byNonType.val))
                    return FALSE;
#endif
                break;
            default:
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    return !old && !sym;
}
SYMBOL *LookupSpecialization(SYMBOL *sym, TEMPLATEPARAMLIST *templateParams)
{
    TYPE *tp;
    SYMBOL *candidate;
    LIST *lst = sym->specializations, **last;
    // maybe we know this specialization
    while (lst)
    {
        candidate = (SYMBOL *)lst->data;
        if (candidate->templateParams && exactMatchOnTemplateArgs(templateParams->p->bySpecialization.types, candidate->templateParams->p->bySpecialization.types))
            return candidate;
        lst = lst->next;
    }
    // maybe we know this as an instantiation
    lst = sym->instantiations;
    last = &sym->instantiations;
    while (lst)
    {
        candidate = (SYMBOL *)lst->data;
        if (candidate->templateParams && exactMatchOnTemplateSpecialization(templateParams->p->bySpecialization.types, candidate->templateParams->next))
        {
            *last = (*last)->next;
            break;
        }
        last = &(*last)->next;
        lst = lst->next;
    }
    if (!lst)
    {
        candidate = clonesym(sym);
        candidate->tp = (TYPE *)Alloc(sizeof(TYPE));
        *candidate->tp = *sym->tp;
        candidate->tp->sp = candidate;
    }
    candidate->maintemplate = NULL;
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
    tp = Alloc(sizeof(TYPE));
    *tp = *candidate->tp;
    candidate->tp = tp;
    candidate->tp->syms = NULL;
    candidate->tp->tags = NULL;
    candidate->baseClasses = NULL;
    candidate->declline = includes->line;
    candidate->declfile = includes->fname;
    candidate->trivialCons = FALSE;
    SetLinkerNames(candidate, lk_cdecl);
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
            if (old->type == sym->type || (isfunction(old) && isfunction(sym)))
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
static BOOLEAN TemplateParseDefaultArgs(SYMBOL *declareSym, TEMPLATEPARAMLIST *dest, TEMPLATEPARAMLIST *src, TEMPLATEPARAMLIST *enclosing);
static BOOLEAN ValidateArgsSpecified(TEMPLATEPARAMLIST *params, SYMBOL *func, INITLIST *args);
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
SYMBOL *LookupFunctionSpecialization(SYMBOL *overloads, SYMBOL *sp)
{
	SYMBOL *found1=NULL, *found2= NULL;
	int n = 0,i;
	SYMBOL **spList;
	HASHREC *hr = overloads->tp->syms->table[0];
    SYMBOL *sd = getStructureDeclaration();
    saveParams(&sd, 1);
	while (hr)
	{
		SYMBOL *sym = (SYMBOL *)hr->p;
		if (sym->templateLevel && (!sym->parentClass || sym->parentClass->templateLevel != sym->templateLevel))
			n++;
		hr = hr->next;
	}
    
	spList = (SYMBOL **)Alloc(n * sizeof(SYMBOL *));
	n = 0;
	hr = overloads->tp->syms->table[0];

	while (hr)
	{
		SYMBOL *sym = (SYMBOL *)hr->p;
		if (sym->templateLevel && (!sym->parentClass || sym->parentClass->templateLevel != sym->templateLevel))
			spList[n++] = detemplate(sym, NULL, sp->tp);
		hr = hr->next;
	}
    TemplatePartialOrdering(spList, n, NULL, sp->tp, FALSE, TRUE);
	for (i=0; i < n; i++)
	{
		if (spList[i])
		{
			found1 = spList[i];
			for (++i; i < n && !found2; i++)
				found2 = spList[i];
		}
	}
	if (found1 && !found2 && allTemplateArgsSpecified(found1->templateParams->next))
	{
		sp->templateParams = copyParams(found1->templateParams, FALSE);
		sp->instantiated = TRUE;
		SetLinkerNames(sp, lk_cdecl);
		found1 = sp;
	}
	else
	{
		found1 = NULL;
	}
    restoreParams(&sd, 1);
    return found1;
}
LEXEME *TemplateArgGetDefault(LEXEME **lex)
{
    LEXEME *rv = NULL, **cur = &rv;
    int inbracket = 0;
    int inparen = 0;
    while (*lex != NULL)
    {
        if (!inparen)
        {
            if (!inbracket && MATCHKW(*lex, comma))
                break;
            if (MATCHKW(*lex, gt) || MATCHKW(*lex, rightshift))
            {
                if (!inbracket--)
                    break;
                    /*
                if (!--inbracket)
                {
                    if (MATCHKW(*lex, rightshift))
                    {
                        lex = getGTSym(*lex);
                        *cur = Alloc(sizeof(LEXEME));
                        **cur = **lex;
                        cur = &(*cur)->next;
                    }
                    else
                    {
                        *cur = Alloc(sizeof(LEXEME));
                        **cur = **lex;
                        cur = &(*cur)->next;
                        needkw(lex, gt);
                    }
                    break;
                }
                */
            }   
            if (MATCHKW(*lex, lt))
                inbracket++;
        }
        if (MATCHKW(*lex, openpa))
        {
            inparen++;
        }
        if (MATCHKW(*lex, closepa) && inparen)
            inparen--;
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
    TEMPLATEPARAMLIST **lst = args, **begin = args, *search;
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
        search = *begin;
        while (search)
        {
            if (search->p->byClass.txtdflt)
            {
                LIST *lbegin = NULL, **hold = &lbegin;
                search = *begin;
                while (search)
                {
                    *hold = (LIST *)Alloc(sizeof(LIST));
                    (*hold)->data = search->p->sym;
                    hold = &(*hold)->next;
                    search = search->next;
                }
                search = (*begin);
                while (search)
                {
                    if (search->p->byClass.txtdflt)
                        search->p->byClass.txtargs = lbegin;   
                    search = search->next;
                }
                break;
            }
            search = search->next;
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
                
                lex = nestedSearch(lex, &sym, &strsym, &nsv, NULL, NULL, FALSE, sc_global, FALSE);
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
                            tp = &stdany;
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
                arg->p->sym = makeID(sc_templateparam, tp, NULL, AnonymousName());
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
                arg->p->sym = makeID(sc_templateparam, tp, NULL, AnonymousName());
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
            lex = getBasicType(lex, funcsp, &tp, NULL, FALSE, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL, FALSE, TRUE);
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
            else 
            {
                TYPE *tpa;
                if (!sp)
                {
                    error(ERR_IDENTIFIER_EXPECTED);
                    sp = makeID(sc_templateparam, NULL, NULL, AnonymousName());
                }
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
            default:
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
static SYMBOL *SynthesizeTemplate(TYPE *tp, BOOLEAN alt)
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
            default:
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
    return rv;
}

void SynthesizeQuals(TYPE ***last, TYPE **qual, TYPE ***lastQual)
{
    if (*qual)
    {
        TYPE *p = **last;
        TYPE *v = *qual;
        int sz = basetype(**last)->size;
        while (v)
        {
            **last = v;
            (**last)->size = sz;
            *last = &(**last)->btp;
            v = v->btp;
        }
        **last = NULL;
        while (p)
        {
            **last = Alloc(sizeof(TYPE));
            ***last = *p;
            *last = &(**last)->btp;
            p = p->btp; 
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
        if (sp->templateLevel && !sp->instantiated)
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
                        (*pt)->p = Alloc(sizeof(TEMPLATEPARAM));
                        *(*pt)->p = *search->p;
                        (*pt)->p->byClass.dflt = SynthesizeType(search->p->byClass.dflt, enclosing, FALSE);
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
            sp = GetClassTemplate(sp, params, FALSE);
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
                    sp = GetClassTemplate(ts, current, FALSE);
					if (sp)
						sp = TemplateClassInstantiate(sp, current, FALSE , sc_global);
                    if (sp)
                        tp = sp->tp;
                    else
                        tp = &stdany;
                        
                }
                else
                {
                    tp = basetype(ts->tp);
                    if (tp->templateParam->p->type != kw_typename)
                    {
                        return &stdany;
                    }
                    tp = alt ? tp->templateParam->p->byClass.temp : tp->templateParam->p->byClass.val;
                    if (!tp)
                        return &stdany;
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
                (*last)->btp = NULL;
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
                                    insert(clone, func->syms);
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
                    TYPE *type = alt ? tpa->p->byClass.temp : tpa->p->byClass.val;
                    if (type)
                    {
                        TYPE *tx = qual;
                        *last = Alloc(sizeof(TYPE));
                        **last =  *type;
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
                    }
                    return rv;
                }
                else if (tpa->p->type == kw_template)
                {
                    TYPE *type = alt ? tpa->p->byTemplate.temp->tp : tpa->p->byTemplate.val->tp;
                    if (type)
                    {
                        TYPE *tx = qual;
                        *last = Alloc(sizeof(TYPE));
                        **last =  *type;
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
                    }
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
                    SYMBOL *found = TemplateClassInstantiateInternal(syms[i], syms[i]->templateParams, TRUE);
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
    TYPE *pP = basetype(P);
    TYPE *pA = basetype(A);
    if (pP->sp && pA->sp && pP->sp->parentTemplate == pA->sp->parentTemplate)
    {
        TEMPLATEPARAMLIST *TP = pP->sp->templateParams->next;
        TEMPLATEPARAMLIST *TA = pA->sp->templateParams;
        TEMPLATEPARAMLIST *TAo = TA;
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
                    {
                        TP->p->byClass.txtdflt = TA->p->byClass.txtdflt;
                        TP->p->byClass.txtargs = TA->p->byClass.txtargs;
                    }
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
                    {
                        TP->p->byTemplate.txtdflt = TA->p->byTemplate.txtdflt;
                        TP->p->byTemplate.txtargs = TA->p->byTemplate.txtargs;
                    }
                    if (!TP->p->byTemplate.val)
                        TP->p->byTemplate.val = TA->p->byTemplate.val;
                    else if (!DeduceFromTemplates(TP->p->byTemplate.val->tp, TA->p->byTemplate.val->tp, change, byClass))
                        return FALSE;
                    break;
                }
                case kw_int:
                {
                    EXPRESSION **exp;
                    if (TAo->p->bySpecialization.types)
                    {
    #ifndef PARSER_ONLY
                        if (!equalTemplateIntNode(TA->p->byNonType.dflt, TA->p->byNonType.val))
                            return FALSE;
    #endif
                    }
                    exp = change ? &TP->p->byNonType.val : &TP->p->byNonType.temp;
                    *exp = TA->p->byNonType.val;
                    if (!TP->p->byNonType.txtdflt)
                    {
                        TP->p->byNonType.txtdflt = TA->p->byNonType.txtdflt;
                        TP->p->byNonType.txtargs = TA->p->byNonType.txtargs;
                    }
                    break;
                }
                default:
                    break;
            }
            TP = TP->next;
            TA = TA->next;
        }
        return !TP && !TA;
    }
    return FALSE;
}
static BOOLEAN DeduceFromBaseTemplates(TYPE *P, SYMBOL *A, BOOLEAN change, BOOLEAN byClass)
{
    BASECLASS *lst = A->baseClasses;
    while (lst)
    {
        if (DeduceFromBaseTemplates(P, lst->cls, change, byClass))
            return TRUE;
        if (DeduceFromTemplates(P, lst->cls->tp, change, byClass))
            return TRUE;
        lst = lst->next;
    }
    return FALSE;
}
static TYPE *FixConsts(TYPE *P, TYPE *A)
{
    int pn=0, an=0;
    TYPE *Pb = P;
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
            if ((constant && isconst(P)) || (vol && isvolatile(P)))
            {
                *last = Alloc(sizeof(TYPE));
                **last = *P;
                last = &(*last)->btp;
                *last = NULL;
            }
            P = P->btp;
        }
        while (A != basetype(A))
        {
            if (A->type == bt_const && !isconst(Pb))
            {
                *last = Alloc(sizeof(TYPE));
                **last = *A;
                last = &(*last)->btp;
                *last = NULL;
            }
            else if (A->type == bt_volatile && !isvolatile(Pb))
            {
                    *last = Alloc(sizeof(TYPE));
                    **last = *A;
                    last = &(*last)->btp;
                    *last = NULL;
            }
            A = A->btp;
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
            if (primary->p->packed)
            {
                primary->p->byPack.pack = match;
                match = NULL;
                primary = primary->next;
                break;
            }
            else if (!DeduceTemplateParam(primary, primary->p->byClass.val, match->p->byClass.val, change))
                return FALSE;
            primary = primary->next;
            match = match->next;
        }
        if (!primary && !match)
        {
            SYMBOL **sp = change ? &Pt->p->byTemplate.val : &Pt->p->byTemplate.temp;
            *sp = basetype(A)->sp;
            sp = change ? &Pt->p->byTemplate.orig->p->byTemplate.val : &Pt->p->byTemplate.orig->p->byTemplate.temp;
            *sp = basetype(A)->sp;
            return TRUE;
        }
        
    }
    return FALSE;
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
            if (DeduceFromTemplates(P, A, change, byClass))
                return TRUE;
            else
                return DeduceFromBaseTemplates(P, basetype(A)->sp, change, byClass);
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
            case bt_struct:
            case bt_union:
            case bt_class:
                return templatecomparetypes(Pb, Ab, TRUE);
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
                    if (tp->type == bt_void || isfunction(tp) || isref(tp) || (isstructured(tp) && tp->sp->isabstract))
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
                    sp = GetClassTemplate(ts, current, FALSE);
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
                    sp = basetype(PerformDeferredInitialization (sp->tp, NULL))->sp;
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
                if (!tp || isref(tp))
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
                if (isfunction(tp) || isarray(tp) || (isstructured(tp) && tp->sp->isabstract))
                    return FALSE;                
                break;
            }
            case bt_templateparam:
                if (tp->templateParam->p->type == kw_template)
                {
                    TEMPLATEPARAMLIST *tpl;
                     if (tp->templateParam->p->packed)
                         return TRUE;
                     if (tp->templateParam->p->byTemplate.val == NULL)
                         return FALSE;
                    tpl = tp->templateParam->p->byTemplate.args;
                    while (tpl)
                    {
                        if (tpl->p->type == kw_typename)
                        {
                            if (tpl->p->packed)
                            {
                                // this should be recursive...
                                TEMPLATEPARAMLIST *tpl1 = tpl->p->byPack.pack;
                                while (tpl1)
                                {
                                    if (tpl1->p->type == kw_typename && !tpl1->p->packed)
                                    {
                                        if (!ValidArg(tpl1->p->byClass.val))
                                            return FALSE;
                                    }
                                    tpl1 = tpl1->next;
                                }
                            }
                            else if (!ValidArg(tpl->p->byClass.val))
                                return FALSE;
                        }
                        // this really should check nested templates...
                        tpl = tpl->next;
                    }
                }
                else
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return FALSE;
                     if (tp->templateParam->p->packed)
                         return TRUE;
                     if (tp->templateParam->p->byClass.val == NULL)
                         return FALSE;
                     if (tp->templateParam->p->byClass.val->type == bt_void)
                         return FALSE;
                     if (tp->templateParam->p->byClass.val == tp) // error catcher
                         return FALSE;
                    return ValidArg(tp->templateParam->p->byClass.val);
                }
            default:
                return TRUE;
        }
    }
}
static BOOLEAN valFromDefault(TEMPLATEPARAMLIST *params, BOOLEAN usesParams, INITLIST **args)
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
    inDefaultParam++;
    if (!valFromDefault(params, usesParams, &args))
    {
        inDefaultParam--;
        return FALSE;
    }
    while (params)
    {
        if (params->p->type == kw_typename || params->p->type == kw_template || params->p->type == kw_int)
            if (!params->p->packed && !params->p->byClass.val)
            {
                inDefaultParam--;
                return FALSE;
            }
            else if (params->p->packed && !params->p->byPack.pack)
            {
                inDefaultParam--;
                return FALSE;
            }
        params = params->next;
    }
    if (hr)
    {
        STRUCTSYM s,s1;
        if (func->parentClass)
        {
            s1.str = func->parentClass;
            addStructureDeclaration(&s1);
        }
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
                lex = initialize(lex, func, sp, sc_parameter, TRUE, 0);
                SetAlternateLex(NULL);
                if (sp->init && sp->init->exp && !ValidExp(&sp->init->exp))
                {
                    dropStructureDeclaration();
                    inDefaultParam--;
                    return FALSE;
                }
            }
            hr = hr->next;
        }
        dropStructureDeclaration();
        if (func->parentClass)
            dropStructureDeclaration();
    }
//    if (!ValidArg(basetype(func->tp)->btp))
//        return FALSE;
    hr = basetype(func->tp)->syms->table[0];
    while (hr && (!usesParams || check))
    {
        if (!ValidArg(((SYMBOL *)hr->p)->tp))
        {
            inDefaultParam--;
            return FALSE;
        }
        if (check)
            check = check->next;
        hr = hr->next;
    }
    inDefaultParam--;
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
    if (isfuncptr(P) || (isref(P) && isfunction(basetype(P)->btp)))
    {
        if (exp->type == en_func)
        {
            if (exp->v.func->sp->storage_class == sc_overloads)
            {
                HASHREC *hr = basetype(exp->v.func->sp->tp)->syms->table[0];
                SYMBOL *candidate = FALSE;
                while (hr)
                {
                    SYMBOL *sym = (SYMBOL *)hr->p;
                    if (sym->templateLevel)
                        return FALSE;
                    hr = hr->next;
                }
                // no templates, we can try each function one at a time
                hr = basetype(exp->v.func->sp->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL *sym = (SYMBOL *)hr->p;
                    clearoutDeduction(P);
                    if (Deduce(P->btp, sym->tp, FALSE, byClass))
                    {
                        if (candidate)
                            return FALSE;
                        else
                            candidate = sym;
                    }
                    hr = hr->next;
                }
                if (candidate)
                    return Deduce(P, candidate->tp, TRUE, byClass);
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
static INITLIST *TemplateDeduceArgList(HASHREC *funcArgs, HASHREC *templateArgs, INITLIST *symArgs)
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
                    if (funcArgs)
                        funcArgs = funcArgs->next;
                }
            }
            else
            {
                symArgs = symArgs->next;
                if (funcArgs)
                    funcArgs = funcArgs->next;
            }
        }    
        else if (symArgs->nested && funcArgs)
        {
            INITLIST *a = symArgs->nested;
            TEMPLATEPARAMLIST *b = ((SYMBOL *)funcArgs->p)->templateParams;
            while (a && b)
            {
                a = a->next;
                b = b->next;
            }
            if (!a && !b)
            {
                // this only works with one level of nesting...
                INITLIST *a = symArgs->nested;
                TEMPLATEPARAMLIST *b = ((SYMBOL *)funcArgs->p)->templateParams;
                TemplateDeduceArgList(NULL, b, a);
            }
            symArgs = symArgs->next;
            if (funcArgs)
                funcArgs = funcArgs->next;
        }
        else
        {
            TemplateDeduceFromArg(sp->tp, symArgs->tp, symArgs->exp, FALSE);
            symArgs = symArgs->next;
            if (funcArgs)
                funcArgs = funcArgs->next;
        }
        templateArgs = templateArgs->next;
    }
    return symArgs;
}
static void SwapDefaultNames(TEMPLATEPARAMLIST *params, LIST *origNames)
{
    while (params && origNames)
    {
        char *temp = ((SYMBOL *)origNames->data)->name;
        ((SYMBOL *)origNames->data)->name = (void *)params->p->sym->name;
        params->p->sym->name = temp;
        params = params->next;
        origNames = origNames->next;
    }
}
static BOOLEAN TemplateParseDefaultArgs(SYMBOL *declareSym, 
                                        TEMPLATEPARAMLIST *dest, 
                                        TEMPLATEPARAMLIST *src, 
                                        TEMPLATEPARAMLIST *enclosing)
{
    STRUCTSYM s;
    s.tmpl = enclosing;
    addTemplateDeclaration(&s);
    while (src && dest)
    {
        if (!dest->p->byClass.val && !dest->p->packed)
        {
            LEXEME *lex;
            int n;
            if (!src->p->byClass.txtdflt)
            {
                dropStructureDeclaration();
                return FALSE;
            }
            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
            n = PushTemplateNamespace(declareSym);
            dest->p->byClass.txtdflt = src->p->byClass.txtdflt;
            dest->p->byClass.txtargs = src->p->byClass.txtargs;
            lex = SetAlternateLex(src->p->byClass.txtdflt);
            switch(dest->p->type)
            {
                case kw_typename:
                {
                    lex = get_type_id(lex, &dest->p->byClass.val, NULL, FALSE);
                    if (!dest->p->byClass.val)    
                    {
                        error(ERR_CLASS_TEMPLATE_DEFAULT_MUST_REFER_TO_TYPE);
                        dropStructureDeclaration();
                        return FALSE;
                    }
                    break;
                }
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
                    lex = expression_no_comma(lex, NULL, NULL, &tp1, &exp1, NULL, _F_INTEMPLATEPARAMS);
                    dest->p->byNonType.val = exp1;
                    if (!templatecomparetypes(dest->p->byNonType.tp, tp1, TRUE))
                    {
                        if (!ispointer(tp1) || !isint(tp1) || !isconstzero(tp1, exp1))
                            error(ERR_TYPE_NONTYPE_TEMPLATE_PARAMETER_DEFAULT_TYPE_MISMATCH);
                    }
                }
                    break;
                default:
                    break;
            }
            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
            PopTemplateNamespace(n);
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
    TYPE *thistp = args->thistp;
    INITLIST *arguments = args->arguments;
    if (!thistp && ismember(sym))
    {
        arguments = arguments->next;
        thistp = args->arguments->tp;
    }
    if (args && thistp && sym->parentClass && !nparams)
    {
        TYPE *tp = basetype(basetype(thistp)->btp);
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
            dest->p->byNonType.txtargs = src->p->byNonType.txtargs;
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
        INITLIST *symArgs = arguments;
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
                    if (!exactMatchOnTemplateParams(initial->p->byTemplate.dflt->templateParams->next, params->p->byTemplate.dflt->templateParams->next))
                        return FALSE;
                    break;
                case kw_int:
                    if (!templatecomparetypes(initial->p->byNonType.tp, params->p->byNonType.tp, TRUE) && (!ispointer(params->p->byNonType.tp) || !isconstzero(initial->p->byNonType.tp, params->p->byNonType.dflt)))
                        return FALSE;
                    break;
                default:
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
            TemplateDeduceArgList(basetype(sym->tp)->syms->table[0], templateArgs, symArgs);
        }
        // set up default values for non-deduced and non-initialized args
        params = nparams->next;
        if (TemplateParseDefaultArgs(sym, params, params, params) && 
            ValidateArgsSpecified(sym->templateParams->next, sym, arguments))
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
    if (TemplateParseDefaultArgs(sym, params, params, params) && ValidateArgsSpecified(sym->templateParams->next, sym, NULL))
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
            if ((isconst(P) && !isconst(A)) || (isvolatile(P) && !isvolatile(A)))
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
        TemplateDeduceFromConversionType(basetype(sym->tp)->btp, tp);
        params = nparams->next;
        if (TemplateParseDefaultArgs(sym, params, params, params) && ValidateArgsSpecified(sym->templateParams->next, sym, NULL))
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
		if (nparams)
		{
	        params = nparams->next;
			if (TemplateParseDefaultArgs(sym, params, params, params) && ValidateArgsSpecified(sym->templateParams->next, sym, NULL))
			{
				return SynthesizeResult(sym, nparams);
			}
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
        if ((isconst(P) && !isconst(A)) || (isvolatile(P) && !isvolatile(A)))
            p = TRUE;
        if ((isconst(A) && !isconst(P)) || (isvolatile(A) && !isvolatile(P)))
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
    return 0;
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
        INITLIST *args = fcall ? fcall->arguments : NULL;
        if (fcall && fcall->thisptr)
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
            else if (which && arr[i] && which != arr[i])
                return 0;
    }
    if (which == 100)
        which = 0;
    if (!which)
    {
        /*
        if (!syml->specialized && symr->specialized)
        {
            TEMPLATEPARAMLIST *l = syml->templateParams->next;
            TEMPLATEPARAMLIST *r = symr->templateParams->p->bySpecialization.types;
            while (l && r)
            {
                if (!templatecomparetypes(l->p->byClass.val, r->p->byClass.val, TRUE))
                    return -1;
                l = l->next;
                r = r->next;
            }            
            return 0;
        }
        else
            */
        {
            TEMPLATEPARAMLIST *l = syml->templateParams->next;
            TEMPLATEPARAMLIST *r = symr->templateParams->next;
            int i;
            n = 0;
            while (l && r)
            {
                int l1 = l->p->type == kw_typename ? !!l->p->byClass.val : 0;
                int r1 = r->p->type == kw_typename ? !!r->p->byClass.val : 0;
                if (l1 && !r1)
                    arr[n++] = -1;
                else if (r1 && !l1)
                    arr[n++] = 1;
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
                                    params->p->byClass.temp = (TYPE *)typechk->data;
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
                                /*
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
                                */
                                break;
                            default:
                                break;
                        }
                        params = params->next;
                    }
                    if (isstructured(sym->tp))
                        typetab[i] = SynthesizeTemplate(sym->tp, TRUE)->tp;
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
        if (porig->p->bySpecialization.types)
        {
            porig = porig->p->bySpecialization.types;
        }
        else
        {
            porig = porig->next;
        }

        if (psym->p->bySpecialization.types)
        {
            psym = psym->p->bySpecialization.types;
        }
        else
        {
            psym = psym->next;
        }
        while (porig && psym)
        {
            void *xorig, *xsym;
            xorig = porig->p->byClass.val;
            xsym = psym->p->byClass.val;
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
                            TYPE *torig = (TYPE *)packorig->p->byClass.val;
                            TYPE *tsym =  (TYPE *)packsym->p->byClass.val;
                            if (tsym->type == bt_templateparam)
                                tsym = tsym->templateParam->p->byClass.val;
                            if (!templatecomparetypes(torig, tsym, TRUE) && !sameTemplate(torig, tsym))
                                return FALSE;
                            if (isref(torig))
                                torig = basetype(torig)->btp;
                            if (isref(tsym))
                                tsym = basetype(tsym)->btp;
                            if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
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
                        if (tsym->type == bt_templateparam)
                            tsym = tsym->templateParam->p->byClass.val;
                        if ((!templatecomparetypes(torig, tsym, TRUE) || !templatecomparetypes(tsym, torig, TRUE)) && !sameTemplate(torig, tsym))
                            return FALSE;
                        if (isref(torig))
                            torig = basetype(torig)->btp;
                        if (isref(tsym))
                            tsym = basetype(tsym)->btp;
                        if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
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
                    if (!equalTemplateIntNode((EXPRESSION *)xorig, (EXPRESSION *)xsym))
                        return FALSE;
#endif
                    break;
                default:
                    break;
            }
            porig = porig->next;
            psym = psym->next;
        }
        return TRUE;
    }
    return !porig && !psym;
}
BOOLEAN TemplateInstantiationMatch(SYMBOL *orig, SYMBOL *sym)
{
    if (orig && orig->parentTemplate == sym->parentTemplate)
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
    return FALSE;
}
static void TemplateTransferClassDeferred(SYMBOL *newCls, SYMBOL *tmpl)
{
    if (!newCls->templateParams->p->bySpecialization.types)
    {
        HASHREC *ns = newCls->tp->syms->table[0];
        HASHREC *os = tmpl->tp->syms->table[0];
        while (ns && os)
        {
            SYMBOL *ss = (SYMBOL *)ns->p;
            SYMBOL *ts = (SYMBOL *)os->p;
            if (strcmp(ss->name, ts->name) != 0) 
            {
                ts = search(ss->name, tmpl->tp->syms);
                // we might get here with ts = NULL for example when a using statement inside a template
                // references base class template members which aren't defined yet.
            }
            if (ts)
            {
                if (ss->tp->type == bt_aggregate && ts->tp->type == bt_aggregate)
                {
                    HASHREC *os2 = ts->tp->syms->table[0];
                    HASHREC *ns2 = ss->tp->syms->table[0];
                    // these lists may be mismatched, in particular the old symbol table
                    // may have partial specializations for templates added after the class was defined...
                    while (ns2 && os2)
                    {
                        SYMBOL *ts2 = (SYMBOL *)os2->p;
                        SYMBOL *ss2 = (SYMBOL *)ns2->p;
                        if (ts2->defaulted || ss2->defaulted)
                            break;
                        ss2->copiedTemplateFunction = TRUE;
                        if (os2)
                        {
                            HASHREC *tsf = basetype(ts2->tp)->syms->table[0];
                            HASHREC *ssf = basetype(ss2->tp)->syms->table[0];
                            while (tsf && ssf)
                            {
                                ssf->p->name = tsf->p->name;
                                tsf = tsf->next;
                                ssf = ssf->next;
                            }
                            ss2->deferredCompile = ts2->deferredCompile;
                            if (!ss2->instantiatedInlineInClass)
                            {
                                TEMPLATEPARAMLIST *tpo = tmpl->parentTemplate->templateParams;
                                if (tpo)
                                {
                                    TEMPLATEPARAMLIST *tpn = ts2->templateParams, *spo;
                                    while (tpo && tpn)
                                    {
                                        SYMBOL *s = tpn->p->sym;
                                        *tpn->p = *tpo->p;
                                        tpn->p->sym = s;
                                        tpo = tpo->next;
                                        tpn = tpn->next;
                                    }
                                    
                                    if (!ss2->templateParams)
                                        ss2->templateParams = ts2->templateParams;
                                }
                            }
                            ns2 = ns2->next;
                            os2 = os2->next;
                        }
                    }
                }
            }
            ns = ns->next;
            os = os->next;
        }
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
            if ((special->p->byClass.val && !args->p->byClass.dflt) || (!special->p->byClass.val && args->p->byClass.dflt))
                return FALSE;
            switch (args->p->type)
            {
                case kw_typename:
                    if (args->p->byClass.dflt && !templatecomparetypes(special->p->byClass.val, args->p->byClass.dflt, TRUE))
                        return FALSE;
                    break;
                case kw_template:
                    if (args->p->byTemplate.dflt && !ValidSpecialization(special->p->byTemplate.args, args->p->byTemplate.dflt->templateParams, TRUE))
                        return FALSE;
                    break;
                case kw_int:
                    if (!templatecomparetypes(special->p->byNonType.tp, args->p->byNonType.tp, TRUE))
                        if (!isint(special->p->byNonType.tp) || !isint(args->p->byNonType.tp))
                            return FALSE;
                    break;
                default:
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
void SetTemplateNamespace(SYMBOL *sym)
{
    LIST *list = nameSpaceList;
    sym->templateNameSpace = NULL;
    while (list)
    {
        LIST *nlist = Alloc(sizeof(LIST));
        nlist->data = list->data;
        nlist->next = sym->templateNameSpace;
        sym->templateNameSpace = nlist;
        list = list->next;
    }
}
int PushTemplateNamespace(SYMBOL *sym)
{
    int rv = 0;
    LIST *list = nameSpaceList;
    while (list)
    {
        SYMBOL *sp = (SYMBOL *)list->data;
        sp->value.i ++;
        list = list->next;
    } 
    list = sym ? sym->templateNameSpace : NULL;
    while (list)
    {
        SYMBOL *sp = (SYMBOL *)list->data;
        if (!sp->value.i)
        {
            LIST *nlist;
            sp->value.i++;
        
            nlist = Alloc(sizeof(LIST));
            nlist->next = nameSpaceList;
            nlist->data = sp;
            nameSpaceList = nlist;
            
            sp->nameSpaceValues->next = globalNameSpace;
            globalNameSpace = sp->nameSpaceValues;
            
            rv ++;
        }
        list = list->next;
    }
    return rv;
}
void PopTemplateNamespace(int n)
{
    int i;
    LIST *list;
    for (i=0; i < n; i++)
    {
        LIST *nlist;
        SYMBOL *sp;
        globalNameSpace = globalNameSpace->next;
        nlist = nameSpaceList;
        sp = (SYMBOL *)nlist->data;
        sp->value.i--;
        nameSpaceList = nameSpaceList->next;
    }
    list = nameSpaceList;
    while (list)
    {
        SYMBOL *sp = (SYMBOL *)list->data;
        sp->value.i --;
        list = list->next;
    } 
    
}
static void SetTemplateArgAccess(SYMBOL *sym, BOOLEAN accessible)
{
    if (accessible)
    {
        if (!instantiatingTemplate && !isExpressionAccessible(theCurrentFunc ? theCurrentFunc->parentClass : NULL, sym, theCurrentFunc, NULL, FALSE))
            errorsym(ERR_CANNOT_ACCESS, sym);

        sym->accessibleTemplateArgument ++;
    }
    else
    {
        sym->accessibleTemplateArgument --;
    }
}
static void SetAccessibleTemplateArgs(TEMPLATEPARAMLIST *args, BOOLEAN accessible)
{
    while (args)
    {
        if (args->p->packed)
        {
            SetAccessibleTemplateArgs(args->p->byPack.pack, accessible);   
        }
        else switch (args->p->type)
        {
            case kw_int:
            {
                EXPRESSION *exp = args->p->byNonType.val;
#ifndef PARSER_ONLY
                exp = GetSymRef(exp);
                if (exp)
                {
                    SetTemplateArgAccess(exp->v.sp, accessible);
                }
#endif
                break;
            }
            case kw_template:
            {
                TEMPLATEPARAMLIST *tpl = args->p->byTemplate.args;
                while (tpl)
                {
                    if (!allTemplateArgsSpecified(tpl))
                        return;
                    tpl = tpl->next;
                }
                SetTemplateArgAccess(args->p->byTemplate.val, accessible);
            }
                break;
            case kw_typename:
                if (isstructured(args->p->byClass.val))
                {
                    SetTemplateArgAccess(basetype(args->p->byClass.val)->sp, accessible);
                }
                else if (basetype(args->p->byClass.val) == bt_enum)
                {
                    SetTemplateArgAccess(basetype(args->p->byClass.val)->sp, accessible);
                }                    
                break;
        }
        args = args->next;
    }
}
SYMBOL *TemplateClassInstantiateInternal(SYMBOL *sym, TEMPLATEPARAMLIST *args, BOOLEAN isExtern)
{
    LEXEME *lex = NULL;
    SYMBOL *cls= sym;
    int pushCount;
    if (cls->linkage == lk_virtual)
        return cls;
    if (!isExtern)
    {
        if (sym->maintemplate && (!sym->specialized || sym->maintemplate->specialized))
        {
            lex = sym->maintemplate->deferredCompile;
            if (lex)
                sym->tp = sym->maintemplate->tp;
        }
        if (!lex)
            lex = sym->deferredCompile;
        if (!lex && (!sym->specialized || sym->parentTemplate->specialized))
            lex = sym->parentTemplate->deferredCompile;
        if (lex)
        {
			int oldHeaderCount = templateHeaderCount;
            LIST *oldDeferred = deferred;
            BOOLEAN defd = FALSE;
            SYMBOL old;
            struct templateListData l;
            int nsl = PushTemplateNamespace(sym);
            LEXEME *reinstateLex = lex;
            SetAccessibleTemplateArgs(cls->templateParams, TRUE);
            deferred = NULL;
			templateHeaderCount = 0;
            old = *cls;
            cls->linkage = lk_virtual;
            cls->parentClass = SynthesizeParentClass(cls->parentClass);
            pushCount = pushContext(cls, FALSE);
            cls->linkage = lk_virtual;
            cls->tp = Alloc(sizeof(TYPE));
            *cls->tp = *old.tp;
            cls->tp->syms = NULL;
            cls->tp->tags = NULL;
            cls->tp->sp = cls;
            cls->baseClasses = NULL;
            cls->vbaseEntries = NULL;
            instantiatingTemplate++;
			dontRegisterTemplate+= templateNestingCount != 0;
            lex = SetAlternateLex(lex);
            lex = innerDeclStruct(lex, NULL, cls, FALSE, cls->tp->type == bt_class ? ac_private : ac_public, cls->isfinal, &defd);
            SetAlternateLex(NULL);
            lex = reinstateLex;
            while (lex)
            {
                lex->registered = FALSE;
                lex = lex->next;
            }
            SetAccessibleTemplateArgs(cls->templateParams, FALSE);
            if (old.tp->syms)
                TemplateTransferClassDeferred(cls, &old);
            PopTemplateNamespace(nsl);
			dontRegisterTemplate-= templateNestingCount != 0;
            instantiatingTemplate --;
            deferred = oldDeferred;
            cls->instantiated = TRUE;
            cls->genreffed = TRUE;
			templateHeaderCount = oldHeaderCount;
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
            TEMPLATEPARAMLIST *tpm;
            TYPE **tpx, *tp = sym1->tp;
            tpm = Alloc(sizeof(TEMPLATEPARAMLIST));
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
        InsertInlineData(sym);
        InsertExtern(sym);
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
        if (data->instantiated && TemplateInstantiationMatch(data, sym) && matchOverload(sym->tp, data->tp))
        {
            sym = data;
            if (sym->linkage == lk_virtual || isExtern)
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
        BOOLEAN ok = TRUE;
        if (sym->specialized)
        {
            HASHREC *hr = sym->overloadName->tp->syms->table[0];
            while (hr)
            {
                if (matchOverload(sym->tp, ((SYMBOL *)hr->p)->tp))
                {
                    hr->p = (struct _hrintern_ *)sym;
                    ok = FALSE;
                    break;
                }
                hr = hr->next;
            }
        }
        if (ok)
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
            int oldLinesHead = linesHead;
            int oldLinesTail = linesTail;
			int oldHeaderCount = templateHeaderCount;
            int nsl = PushTemplateNamespace(sym);
            linesHead = linesTail = NULL;
            if (sym->storage_class != sc_member && sym->storage_class != sc_mutable)
                sym->storage_class = sc_global;
            sym->linkage = lk_virtual;
            sym->xc = NULL;
            instantiatingTemplate++;

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
			templateHeaderCount = 0;
            lex = body(lex, sym);
			templateHeaderCount = oldHeaderCount;
            lex = sym->deferredCompile;
            while (lex)
            {
                lex->registered = FALSE;
                lex = lex->next;
            }
            SetAlternateLex(NULL);
            PopTemplateNamespace(nsl);
            linesHead = oldLinesHead;
            linesTail = oldLinesTail;
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
    while ( P && A)
    {
        P = basetype(P);
        A = basetype(A);
        if (P->type != A->type)
            break;
        P = P->btp;
        A = A->btp;
        if (P && A)
        {
            if (byClass)
            {
                if ((isconst(A) != isconst(P)) || (isvolatile(A) != isvolatile(P)))
                    return FALSE;
            }
            else
            {
                if ((isconst(A) && !isconst(P)) || (isvolatile(A) && !isvolatile(P)))
                    return FALSE;
            }
        }
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
static void TemplateConstMatching(SYMBOL **spList, int n, TEMPLATEPARAMLIST *params)
{
    int i;
    BOOLEAN found = FALSE;
    for (i=0; i < n && !found; i++)
        if (spList[i])
        {
            TEMPLATEPARAMLIST *P;
            found = TRUE;
            if (i == 0)
            {
                P = spList[i]->templateParams->next;
                while (P)
                {
                    TYPE *tv = P->p->byClass.val;
                    if (isref(tv))
                        tv = basetype(tv)->btp;
                    if (isconst(tv) || isvolatile(tv))
                    {
                        found = FALSE;
                        break;
                    }
                    P = P->next;
                }
            }
            else
            {
                P = spList[i]->templateParams->p->bySpecialization.types;
                while (P)
                {
                    TYPE *td = P->p->byClass.dflt;
                    TYPE *tv = P->p->byClass.val;
                    if (isref(td))
                        td = basetype(td)->btp;
                    if (isref(tv))
                        tv = basetype(tv)->btp;
                    if ((isconst(td) != isconst(tv)) ||
                        ((isvolatile(td) != isvolatile(tv))))
                    {
                        found = FALSE;
                        break;
                    }
                    P = P->next;
                }
            }
        }
    if (found)
    {
        for (i=0; i < n; i++)
            if (spList[i])
            {
                TEMPLATEPARAMLIST *P;
                if (i == 0)
                {
                    P = spList[i]->templateParams->next;
                    while (P)
                    {
                        TYPE *tv = P->p->byClass.val;
                        if (isref(tv))
                            tv = basetype(tv)->btp;
                        if (isconst(tv) || isvolatile(tv))
                        {
                            spList[i] = 0;
                        }
                        P = P->next;
                    }
                }
                else
                {
                    P = spList[i]->templateParams->p->bySpecialization.types;
                    while (P)
                    {
                        TYPE *td = P->p->byClass.dflt;
                        TYPE *tv = P->p->byClass.val;
                        if (isref(td))
                            td = basetype(td)->btp;
                        if (isref(tv))
                            tv = basetype(tv)->btp;
                        if ((isconst(td) != isconst(tv)) ||
                            ((isvolatile(td) != isvolatile(tv))))
                        {
                            spList[i] = 0;
                        }
                        P = P->next;
                    }
                }
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
        if (!spsyms)
        {
            ClearArgValues(params);
            ClearArgValues(spsyms);
        }
        ClearArgValues(sp->templateParams);
        while (initial && params)
        {
            void *dflt = initial->p->byClass.dflt;
            TEMPLATEPARAMLIST *test = initial;
            if (!dflt)
                dflt = initial->p->byClass.val;
			if (initial->p->type == kw_int)
				if (initial->p->byNonType.tp->type == bt_templateparam)
					dflt = NULL;
            while (test && test->p->type == kw_typename && dflt && ((TYPE *)dflt)->type == bt_templateparam)
            {
                if (test->p->byClass.dflt)
                {
                    test = test->p->byClass.dflt->templateParam;
                    dflt = test->p->byClass.val;
                    if (!dflt)
                        dflt = test->p->byClass.val;
                }
                else
                {
                    break;
                }
            }
            if (test->p->type != params->p->type)
            {
                if (!test->p->byClass.dflt)
                    rv = NULL;
                else if (test->p->type != kw_typename || test->p->byClass.dflt->type != bt_templateselector)
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
                    {
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
                        else if (params->p->type == kw_int)
                        {
//                            if (!templatecomparetypes(initial->p->byNonType.tp, params->p->byNonType.tp, TRUE))
//                                rv = NULL;
#ifndef PARSER_ONLY
                            if (!params->p->byNonType.val || !equalTemplateIntNode(params->p->byNonType.dflt, params->p->byNonType.val))
                                rv = NULL;
#endif
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
            if (!TemplateParseDefaultArgs(sp, params, primary, primary))
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
            {
                rv = NULL;
            }
        }
    }
    return rv;
}
static BOOLEAN checkArgType(TYPE *tp)
{
    while (ispointer(tp) || isref(tp))
        tp = basetype(tp)->btp;
    if (isfunction(tp))
    {
        HASHREC *hr;
        SYMBOL *sym = basetype(tp)->sp;
        if (!checkArgType(basetype(tp)->btp))
            return FALSE;
        if (sym->tp->syms)
        {
            hr = sym->tp->syms->table[0];
            while (hr)
            {
                if (!checkArgType(((SYMBOL *)hr->p)->tp))
                    return FALSE;
                hr = hr->next;
            }
        }
    }
    else if (isstructured(tp))
    {
        if (basetype(tp)->sp->templateLevel)
        {
            return allTemplateArgsSpecified(basetype(tp)->sp->templateParams->next);
        }
    }
    else if (basetype(tp)->type == bt_templateparam || basetype(tp)->type == bt_templateselector)
        return FALSE;
    return TRUE;
}
static BOOLEAN checkArgSpecified(TEMPLATEPARAMLIST *args)
{
    if (!args->p->byClass.val)
        return FALSE;
    switch(args->p->type)
    {
        case kw_int:
            if (!isarithmeticconst(args->p->byNonType.val))
            {
                EXPRESSION *exp = args->p->byNonType.val; 
                if (exp && args->p->byNonType.tp->type !=bt_templateparam)
                {
                    while (castvalue(exp) || lvalue(exp))
                        exp = exp->left;
                    switch (exp->type)
                    {
                        case en_pc:
                        case en_global:
                        case en_label:
                        case en_func:
                            return TRUE;
                        default:
                            break;
                    }
                }
                return FALSE;
            }
            break;
        case kw_template:
        {
            TEMPLATEPARAMLIST *tpl = tpl->p->byTemplate.args;
            while (tpl)
            {
                if (!allTemplateArgsSpecified(tpl))
                    return FALSE;
                tpl = tpl->next;
            }
            break;
        }
        case kw_typename:
        {
            return checkArgType(args->p->byClass.val);
        }
    }
    return TRUE;
}
BOOLEAN allTemplateArgsSpecified(TEMPLATEPARAMLIST *args)
{    
    while (args)
    {
        if (args->p->packed)
        {
            if (!allTemplateArgsSpecified(args->p->byPack.pack))
                return FALSE;
        }
        else if (!checkArgSpecified(args))
        {
            return FALSE;
        }
        args = args->next;
    }
                               
    return TRUE;
}
void DuplicateTemplateParamList (TEMPLATEPARAMLIST **pptr)
{
    TEMPLATEPARAMLIST *params = *pptr;
    while (params)
    {
        *pptr = Alloc(sizeof(TEMPLATEPARAMLIST));
        if (params->p->type == kw_typename)
        {
            (*pptr)->p = Alloc(sizeof(TEMPLATEPARAM));
            *(*pptr)->p = *params->p;
            if (params->p->packed)
            {
                TEMPLATEPARAMLIST **pptr1 = &(*pptr)->p->byPack.pack;
                DuplicateTemplateParamList(pptr1);
            }
            else
            {
                (*pptr)->p->byClass.dflt = SynthesizeType(params->p->byClass.val, NULL, FALSE);
            }
        }
        else
        {
            (*pptr)->p = params->p;
        }
        params = params->next;
        pptr = &(*pptr)->next;
    }
}
SYMBOL *GetClassTemplate(SYMBOL *sp, TEMPLATEPARAMLIST *args, BOOLEAN noErr)
{
    int n = 1, i=0;
    TEMPLATEPARAMLIST *unspecialized = sp->templateParams->next;
    SYMBOL *found1 = NULL, *found2 = NULL;
    SYMBOL **spList, **origList;
    TEMPLATEPARAMLIST *orig = sp->templateParams->p->bySpecialization.types ? sp->templateParams->p->bySpecialization.types : sp->templateParams->next, *search = args;
    int count;
    LIST *l;
    if (sp->parentTemplate)
        sp = sp->parentTemplate;
    /*
    while (orig && search)
    {
        if (orig->p->packed)
        {
            search = NULL;
            orig = NULL;
            break;
        }
        orig = orig->next;
        search = search->next;
    }
    if (orig)
    {
        if (!orig->p->byClass.txtdflt && !orig->p->packed)
            errorsym(ERR_TOO_FEW_ARGS_PASSED_TO_TEMPLATE, sp);
    }
    else if (search)
    {
        errorsym(ERR_TOO_MANY_ARGS_PASSED_TO_TEMPLATE, sp);
    }
    */
    l = sp->specializations;
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
    if (n == 1 && spList[0] == 0)
        spList[0] = origList[0];
    for (i=0,count=0; i < n; i++)
    {
        if (spList[i])
            count++;
    }
    if (count > 1)
    {
        int count = 0;
        spList[0] = 0;
        TemplatePartialOrdering(spList, n, NULL, NULL, TRUE, FALSE);
        for (i=0; i < n; i++)
            if (spList[i])
                count++;
        if (count > 1)
            TemplateConstMatching(spList, n, args);
        count = 0;
        for (i=0; i < n; i++)
            if (spList[i])
                count++;
        if (count > 1)
            TemplateConstOrdering(spList, n, args);
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
        if (!noErr)
        {
            errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        }
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
    else if (found2)
    {
		restoreParams(origList, n);
        errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        return NULL;
    }
    if (found1 && !found2)
    {
        if  (allTemplateArgsSpecified(found1->templateParams->next))
        {
            SYMBOL *parent = found1->parentTemplate;
            SYMBOL *sym = found1;
            TEMPLATEPARAMLIST *dflts;
            TEMPLATEPARAMLIST *orig;
            LIST *instants = parent->instantiations;
            while (instants)
            {
                if (TemplateInstantiationMatch(instants->data, found1))
                {
    			    restoreParams(origList, n);
                    return (SYMBOL *)instants->data;
                }
                instants = instants->next;
            }
            found1 = clonesym(found1);
            found1->maintemplate = sym;
            found1->tp = Alloc(sizeof(TYPE));
            *found1->tp = *sym->tp;
            found1->tp->sp = found1;
            found1->gentemplate = TRUE;
            found1->instantiated = TRUE;
            found1->performedStructInitialization = FALSE;
            found1->templateParams = copyParams(found1->templateParams, TRUE);
            if (found1->templateParams->p->bySpecialization.types)
            {
                TEMPLATEPARAMLIST **pptr = &found1->templateParams->p->bySpecialization.types;
                DuplicateTemplateParamList(pptr);
            }
            SetLinkerNames(found1, lk_cdecl);
            instants = Alloc(sizeof(LIST));
            instants->data = found1;
            instants->next = parent->instantiations;
            parent->instantiations = instants;
        }
        else
        {
            SYMBOL *sym = found1;
            found1 = clonesym(found1);
            found1->maintemplate = sym;
            found1->tp = Alloc(sizeof(TYPE));
            *found1->tp = *sym->tp;
            found1->tp->sp = found1;
			found1->templateParams = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
			found1->templateParams->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
			*found1->templateParams->p = *sym->templateParams->p;
            found1->templateParams->next = args;
        }
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
                init = &(*init)->next;
                hr = hr->next;
            }
            instance = GetOverloadedTemplate(spi, funcparams);
            if (instance)
            {
                    
                instance = TemplateFunctionInstantiate(instance, TRUE, isExtern);
                *sp = instance;
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
}
static void referenceInstanceMembers(SYMBOL *cls)
{
    if (cls->tp->syms)
    {
        HASHREC *hr = cls->tp->syms->table[0];
        BASECLASS *lst;
        SYMBOL *sym;
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *) hr->p;
            if (sym->storage_class == sc_overloads)
            {
                HASHREC *hr2 = sym->tp->syms->table[0];
                while (hr2)
                {
                    sym = (SYMBOL *)hr2->p;
                    if (sym->deferredCompile && !sym->inlineFunc.stmt)
                    {
                        deferredCompileOne(sym);
                    }
                    if (sym->inlineFunc.stmt)
                        sym->genreffed = TRUE;
                    hr2 = hr2->next;
                }
            }
            else if (!ismember(sym) && !istype(sym))
                sym->genreffed = TRUE;
            hr = hr->next;
        }
        hr = cls->tp->tags->table[0]->next; // past the definition of self
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *) hr->p;
            if (isstructured(sym->tp))
                referenceInstanceMembers(sym);
            hr = hr->next;
        }
        lst = cls->baseClasses;
        while(lst)
        {
            if (lst->cls->templateLevel)
            {
                referenceInstanceMembers(lst->cls);
            }
            lst = lst->next;
        }
    }
}
static BOOLEAN fullySpecialized(TEMPLATEPARAMLIST *tpl)
{
    switch (tpl->p->type)
    {
        case kw_typename:
            return !typeHasTemplateArg(tpl->p->byClass.dflt);
        case kw_template:
            tpl = tpl->p->byTemplate.args;
            while (tpl)
            {
                if (!fullySpecialized(tpl))
                    return FALSE;
                tpl = tpl->next;
            }
            return TRUE;
        case kw_int:
            if (!isarithmeticconst(tpl->p->byNonType.dflt))
            {
                EXPRESSION *exp = tpl->p->byNonType.dflt; 
                if (exp && tpl->p->byNonType.tp->type !=bt_templateparam)
                {
                    while (castvalue(exp) || lvalue(exp))
                        exp = exp->left;
                    switch (exp->type)
                    {
                        case en_pc:
                        case en_global:
                        case en_label:
                        case en_func:
                            return TRUE;
                        default:
                            break;
                    }
                }
                return FALSE;
            }
            else
            {
                return TRUE;
            }
            break;
        default:
            return FALSE;
    }
}
BOOLEAN TemplateFullySpecialized(SYMBOL *sp)
{
    if (sp)
    {
        if (sp->templateParams && sp->templateParams->p->bySpecialization.types)
        {
            TEMPLATEPARAMLIST *tpl = sp->templateParams->p->bySpecialization.types;
            while (tpl)
            {
                if (!fullySpecialized(tpl))
                    return FALSE;
                tpl = tpl->next;
            }
            return TRUE;
        }
    }
    return FALSE;
}
static SYMBOL *matchTemplateFunc(SYMBOL *sym, SYMBOL *check)
{
    if ((sym->templateParams && !allTemplateArgsSpecified(sym->templateParams->next))
        || (sym->parentClass && sym->parentClass->templateParams && 
                                  !allTemplateArgsSpecified(sym->parentClass->templateParams->next)))
    if (isconst(check->tp) == isconst(sym->tp) && isvolatile(check->tp) == isvolatile(sym->tp))
    {
        HASHREC *hrnew = basetype(check->tp)->syms->table[0];
        HASHREC *hrold = basetype(sym->tp)->syms->table[0];
        while (hrnew && hrold)
        {
            SYMBOL *snew = (SYMBOL *)hrnew->p;
            SYMBOL *sold = (SYMBOL *)hrold->p;
            TYPE *tnew = snew->tp;
            TYPE *told = sold->tp;
            BOOLEAN done = FALSE;
            if (isref(tnew))
                tnew = basetype(tnew)->btp;
            if (isref(told))
                told = basetype(told)->btp;
            while (ispointer(tnew) && ispointer(told))
            {
                if (isconst(tnew) != isconst(told) || isvolatile(tnew) != isvolatile(told))
                {
                    done = TRUE;
                    break;
                }
                while (isconst(tnew) || isvolatile(tnew))
                    tnew = tnew->btp;
                while (isconst(told) || isvolatile(told))
                    told = told->btp;
                tnew = basetype(tnew)->btp;
                told = basetype(told)->btp;
            }
            if (done)
                break;
            if (isconst(tnew) != isconst(told) || isvolatile(tnew) != isvolatile(told))
                break;
            tnew = basetype(tnew);
            told = basetype(told);
            if (tnew->type == bt_templateparam)
                break;
            if (told->type != bt_templateparam && !comparetypes(tnew, told, TRUE) && tnew->type != told->type)
                break;
            hrnew = hrnew->next;
            hrold = hrold->next;
        }
        if (!hrnew && !hrold)
        {
            return check;
        }
    }
    return NULL;
}
void propagateTemplateDefinition(SYMBOL *sym)
{
    LEXEME *lex = sym->deferredCompile;
    int oldCount = templateNestingCount;
    templateNestingCount = 0;
    if (lex)
    {
        SYMBOL *parent = sym->parentClass;
        if (parent)
        {
            LIST *lst = parent->instantiations;
            while (lst)
            {
                SYMBOL *cur = (SYMBOL *)lst->data;
                if (cur ->tp->syms)
                {
                    HASHREC **p = LookupName(sym->name, cur->tp->syms);				
                    if (p)
                    {
                        HASHREC *hr;
                        cur =(SYMBOL *)(*p)->p;
                        hr = basetype(cur->tp)->syms->table[0];
                        while (hr)
                        {
                            SYMBOL *cur = (SYMBOL *)hr->p;
                            if (cur->parentClass && matchTemplateFunc(sym, cur))
                            {
                                if (!cur->deferredCompile && !cur->inlineFunc.stmt)
                                {
                                    cur->deferredCompile = lex;
                                    cur->memberInitializers = sym->memberInitializers;
                                    cur->pushedTemplateSpecializationDefinition = 1;
                                    if (sym->tp->syms && cur->tp->syms)
                                    {
                                        HASHREC *src = sym->tp->syms->table[0];
                                        HASHREC *dest = cur->tp->syms->table[0];
                                        while (src && dest)
                                        {
                                            dest->p->name = src->p->name;
                                            src = src->next;
                                            dest = dest->next;
                                        }
                                    }
                                    {
                                        STRUCTSYM t;
                                        SYMBOL *thsprospect = (SYMBOL *)basetype(cur->tp)->syms->table[0]->p;
                                        t.tmpl = NULL;
                                        if (thsprospect && thsprospect->thisPtr)
                                        {
                                            SYMBOL *spt = basetype (basetype(thsprospect->tp)->btp)->sp;
                                            t.tmpl = spt->templateParams;
                                            if (t.tmpl)
                                                addTemplateDeclaration(&t);
                                        }
    //                                    TemplateFunctionInstantiate(cur, FALSE, FALSE);
                                        deferredCompileOne(cur);
                                        InsertInline(cur);
                                        if (t.tmpl)
                                            dropStructureDeclaration();
                                    }
                                }
                            }
                            hr = hr->next;
                        }
                    }
                }
                lst = lst->next;
            }
        }
        else
        {
            SYMBOL *cur = gsearch(sym->name);				
            if (cur)
            {
                HASHREC *hr;
                hr = basetype(cur->tp)->syms->table[0];
                while (hr)
                {
                    cur = (SYMBOL *)hr->p;
                    if (cur->templateLevel && cur->instantiated && matchTemplateFunc(sym, cur))
                    {
                        if (!cur->deferredCompile && !cur->inlineFunc.stmt)
                        {
                            cur->deferredCompile = lex;
                            cur->pushedTemplateSpecializationDefinition = 1;
                            if (sym->tp->syms && cur->tp->syms)
                            {
                                HASHREC *src = sym->tp->syms->table[0];
                                HASHREC *dest = cur->tp->syms->table[0];
                                while (src && dest)
                                {
                                    dest->p->name = src->p->name;
                                    src = src->next;
                                    dest = dest->next;
                                }
                            }
                            {
                                STRUCTSYM t;
                                SYMBOL *thsprospect = (SYMBOL *)basetype(cur->tp)->syms->table[0]->p;
                                t.tmpl = NULL;
                                if (thsprospect && thsprospect->thisPtr)
                                {
                                    SYMBOL *spt = basetype (basetype(thsprospect->tp)->btp)->sp;
                                    t.tmpl = spt->templateParams;
                                    if (t.tmpl)
                                        addTemplateDeclaration(&t);
                                }
//                                    TemplateFunctionInstantiate(cur, FALSE, FALSE);
                                deferredCompileOne(cur);
                                InsertInline(cur);
                                if (t.tmpl)
                                    dropStructureDeclaration();
                            }
                        }
                    }
                    hr = hr->next;
                }
            }
        }
    }
    templateNestingCount = oldCount;
}
LEXEME *TemplateDeclaration(LEXEME *lex, SYMBOL *funcsp, enum e_ac access, enum e_sc storage_class, BOOLEAN isExtern)
{
    lex = getsym();
    if (MATCHKW(lex, lt))
    {
        int lasttemplateHeaderCount = templateHeaderCount;
        TEMPLATEPARAMLIST **tap; // for the specialization list
        TYPE *tp = NULL;
        SYMBOL *declSym = NULL;
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
            (*currents->ptail)->p->type = kw_new;            lex = getsym();
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
                        declSym = l.sym;
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
        if (KWTYPE(lex, TT_STRUCT))
        {
            lex = getsym();
            if (!ISID(lex))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else
            {
                char idname[512];
                SYMBOL *cls = NULL;
                SYMBOL *strSym = NULL;
                NAMESPACEVALUES *nsv = NULL;
                lex = id_expression(lex, funcsp, &cls, &strSym, &nsv, NULL, FALSE, FALSE, idname);
                if (!cls || !isstructured(cls->tp))
                {
                    if (!cls)
                    {
                        errorstr(ERR_NOT_A_TEMPLATE, idname);
                    }
                    else
                    {
                        errorsym(ERR_CLASS_TYPE_EXPECTED, cls);
                    }
                }
                else
                {
                    TEMPLATEPARAMLIST *templateParams = NULL;
                    SYMBOL *instance; 
                    lex = getsym();
                    lex = GetTemplateArguments(lex, funcsp, cls, &templateParams);
                    instance = GetClassTemplate(cls, templateParams, FALSE);
                    if (instance)
                    {
                        if (isExtern)
                            error(ERR_EXTERN_NOT_ALLOWED);
						instance = TemplateClassInstantiate(instance, templateParams, FALSE, sc_global);
                        referenceInstanceMembers(instance);
                    }    
                    else
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, cls);
                    }                
                }
            }
            
        }
        else
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
            lex = getBasicType(lex, funcsp, &tp, &strSym, TRUE, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL, FALSE, TRUE);
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
                DoInstantiateTemplateFunction(tp, &sp, nsv, strSym, templateParams, isExtern);
                sym = sp;
                if (!comparetypes(basetype(sp->tp)->btp, basetype(tp)->btp, TRUE))
                {
                    errorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp);
                }
                if (isExtern)
                {
                    insertOverload(sym, sym->overloadName->tp->syms);
                    sym->storage_class = sc_external;
                    InsertExtern(sym);
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
                    if ((tmpl && spi->storage_class == sc_static) || spi->storage_class == sc_external)
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
    }
    return lex;
}