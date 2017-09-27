/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
    
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
extern ARCH_ASM *chosenAssembler;
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern HASHTABLE *labelSyms;
extern TYPE stdint, stdpointer;
extern SYMBOL *enumSyms;
extern char *overloadNameTab[];
extern LAMBDA *lambdas;
extern STRUCTSYM *structSyms;
extern int currentErrorLine;
extern int templateNestingCount;
extern int total_errors;
#ifndef CPREPROCESSOR
extern ARCH_DEBUG *chosenDebugger;
extern FILE *listFile;
#endif
extern int noSpecializationError;
extern int instantiatingTemplate;
extern INCLUDES *includes;
extern BOOLEAN inTemplateType;
extern LIST *nameSpaceList;

static int insertFuncs(SYMBOL **spList, SYMBOL **spFilterList, LIST *gather, FUNCTIONCALL *args, TYPE *atp);

#define DEBUG

HASHTABLE *CreateHashTable(int size);
#define F_WITHCONS 1
#define F_INTEGER 2
#define F_ARITHMETIC 4
#define F_STRUCTURE 8
#define F_POINTER 16
#define F_CONVERSION 32
static const int rank[] = 
{
//    0,0,0,0,0,1,1,2,2,2,2,2,2,3,4,5,6,6,7
    0,1,1,1,1,2,2,3,3,3,3,3,3,4,4,5,6,7,7,8
};
static SYMBOL *getUserConversion(int flags,
                              TYPE *tpp, TYPE *tpa, EXPRESSION *expa,
                              int *n, enum e_cvsrn *seq, SYMBOL *candidate_in, SYMBOL **userFunc, BOOLEAN honorExplicit);
static BOOLEAN getFuncConversions(SYMBOL *sp, FUNCTIONCALL *f, TYPE *atp, SYMBOL *parent, 
                                  enum e_cvsrn arr[], int *sizes, int count, 
                                  SYMBOL **userFunc, BOOLEAN usesInitList);
static void WeedTemplates(SYMBOL **table, int count, FUNCTIONCALL *args, TYPE *atp);

LIST *tablesearchone(char *name, NAMESPACEVALUES *ns, BOOLEAN tagsOnly)
{
    SYMBOL *rv = NULL;
    if (!tagsOnly)
        rv = search(name, ns->syms);
    if (!rv)
        rv = search(name, ns->tags);
    if (rv)
    {
        LIST *l = Alloc(sizeof(LIST));
        l->data = rv;
        return l;
    }
    return NULL;
}
LIST *tablesearchinline(char *name, NAMESPACEVALUES *ns, BOOLEAN tagsOnly)
{
    // main namespace
    LIST *rv = tablesearchone(name, ns, tagsOnly);
    LIST *lst = ns->inlineDirectives;
    // included inlines
    while (lst)
    {
        SYMBOL *x = lst->data;
        if (!x->visited)
        {
            LIST *rv1;
            x->visited = TRUE;
            rv1 = tablesearchinline(name, x->nameSpaceValues, tagsOnly);
            if (rv1)
            {
                while (rv1->next)
                    rv1 = rv1->next;
                rv1->next = rv;
                rv = rv1;
            }
        }
        lst = lst->next;
    }
    // enclosing ns if this one is inline
    if (ns->name && !ns->name->visited && ns->name->linkage == lk_inline)
    {
        LIST *rv1;
        ns->name->visited = TRUE;
        rv1 = tablesearchinline(name, ns->name->nameSpaceValues, tagsOnly);
        if (rv1)
        {
            while (rv1->next)
                rv1 = rv1->next;
            rv1->next = rv;
            rv = rv1;
        }
        
    }
    return rv;
}
static LIST *namespacesearchone(char *name, NAMESPACEVALUES *ns, LIST *gather, BOOLEAN tagsOnly, BOOLEAN allowUsing)
{
    LIST *rv = tablesearchinline(name, ns, tagsOnly);
    if (rv)
    {
        LIST *rv1 = rv;
        while (rv->next)
            rv = rv->next;
        rv->next = gather;
        rv = rv1;
    }
    else
    {
        rv = gather;
    }
    if (allowUsing)
    {
        LIST *lst = ns->usingDirectives;
        while (lst)
        {
            SYMBOL *x = lst->data;
            if (!x->visited)
            {
                x->visited = TRUE;
                rv = namespacesearchone(name, x->nameSpaceValues, rv, tagsOnly, allowUsing);
            }
            lst = lst->next;
        }
    }
    return rv;
}
static LIST *namespacesearchInternal(char *name, NAMESPACEVALUES *ns, BOOLEAN qualified, BOOLEAN tagsOnly,
                                     BOOLEAN allowUsing)
{
    LIST *lst;
    do
    {
        unvisitUsingDirectives(ns);
        lst = namespacesearchone(name, ns, NULL, tagsOnly, allowUsing);
        ns = ns->next;
    } while (!qualified && !lst && ns);
    return lst;
}
SYMBOL *namespacesearch(char *name, NAMESPACEVALUES *ns, BOOLEAN qualified, BOOLEAN tagsOnly)
{
    LIST *lst = namespacesearchInternal(name, ns, qualified, tagsOnly, TRUE);
    
    if (lst)
    {
        if (lst->next)
        {
            LIST *a = lst;
            while (a)
            {
                if (((SYMBOL *)a->data)->storage_class != sc_overloads)
                    break;
                a = a->next;
            }
            if (!a)
            {
                HASHREC **dest;
                TYPE *tp = Alloc(sizeof(TYPE));
                SYMBOL *sp = makeID(sc_overloads, tp, NULL, ((SYMBOL *)lst->data)->name);
                tp->type = bt_aggregate;
                tp->rootType = tp;
                tp->sp = sp;
                tp->syms = CreateHashTable(1);
                a = lst;
                dest = &tp->syms->table[0];
                while (a)
                {
                    HASHREC *b = ((SYMBOL *)a->data)->tp->syms->table[0];
                    
                    while (b)
                    {
                        *dest = Alloc(sizeof(HASHREC));
                        (*dest)->p = b->p;
                        dest = &(*dest)->next;
                        b = b->next;
                    }
                    a = a->next;
                }
                return sp;
            }
            
        }
        while (lst->next)
        {
            // collision
            SYMBOL *test = lst->data;
            LIST *lst1 = lst->next;
            while (lst1)
            {
                if (test != lst1->data && test->mainsym != lst1->data && ((SYMBOL *)lst1->data)->mainsym != test)
                {
                    errorsym2(ERR_AMBIGUITY_BETWEEN, test, (SYMBOL *)lst1->data);
                }
                lst1 = lst1->next;
            }
            lst = lst->next;
        }
        return (SYMBOL *)lst->data;
    }
    return NULL;
}
LEXEME *nestedPath(LEXEME *lex, SYMBOL **sym, NAMESPACEVALUES **ns, 
                   BOOLEAN *throughClass, BOOLEAN tagsOnly, enum e_sc storage_class, BOOLEAN isType)
{
    NAMESPACEVALUES *nssym = globalNameSpace;
    SYMBOL *strSym = NULL;
    BOOLEAN qualified = FALSE;
    TEMPLATESELECTOR *templateSelector = NULL, **last = &templateSelector;
    LEXEME *placeholder = lex, *finalPos;
    BOOLEAN hasTemplate = FALSE;
    TEMPLATEPARAMLIST *templateParamAsTemplate = NULL;
    TYPE *dependentType = NULL;

    if (sym)
        *sym = NULL;
    if (ns) *ns = NULL;
        
    if (MATCHKW(lex, classsel))
    {
        while (nssym->next)
            nssym = nssym->next;
        lex = getsym();
        qualified = TRUE;
    }
    finalPos = lex;
    while (ISID(lex) || (templateSelector && MATCHKW(lex, kw_operator)))
    {
        char buf[512];
        SYMBOL *sp = NULL;
        int ovdummy;
        lex = getIdName(lex, NULL, buf, &ovdummy, NULL);
        lex = getsym();
        if (templateSelector)
        {
            *last = Alloc(sizeof(TEMPLATESELECTOR));
            (*last)->name = litlate(buf);
            if (hasTemplate)
            {
                (*last)->isTemplate = TRUE;
                if (MATCHKW(lex, lt))
                {
                    lex = GetTemplateArguments(lex, NULL, NULL, &(*last)->templateParams);
                }
                else if (MATCHKW(lex,classsel))
                {
                    errorstr(ERR_NEED_SPECIALIZATION_PARAMETERS, buf);                    
                }
            }
            last = &(*last)->next;
            if (!MATCHKW(lex, classsel))
                break;
            lex = getsym();
            finalPos = lex;
        }
        else
        {
            BOOLEAN hasTemplateArgs = FALSE;
            BOOLEAN deferred = FALSE;
            BOOLEAN istypedef = FALSE;
            TEMPLATEPARAMLIST *current = NULL;
            SYMBOL *currentsp = NULL;
            LEXEME *cur = lex;
            int level = -1;
            if (!strSym)
            {
                TEMPLATEPARAMLIST *tparam = TemplateLookupSpecializationParam(buf);
                if (tparam)
                {
                    sp = tparam->argsym;
                }
                else if (!qualified)
                {
                    sp = classsearch(buf, FALSE, FALSE);
                    if (sp && sp->tp->type == bt_templateparam)
                    {
                        if (sp->tp->templateParam->p->packed)
                        {
                            break;
                        }
                        else
                        {
                            TEMPLATEPARAMLIST *params = sp->tp->templateParam;
                            if (params->p->type == kw_typename)
                            {
                                if (params->p->byClass.val)
                                {
                                    sp = basetype(params->p->byClass.val)->sp;
                                    dependentType = params->p->byClass.val;
                                }
                            }
                            else if (params->p->type == kw_template)
                            {
                                templateParamAsTemplate = params;
                                sp = params->p->byTemplate.val;
                            }
                            else
                                break;
                        }
                    }
                    if (sp && throughClass)
                        *throughClass = TRUE;
                }
                else
                {
                    sp = NULL;
                }
                if (!sp && !templateParamAsTemplate)
                {
                    if (!qualified)
                        sp = namespacesearch(buf, localNameSpace, qualified, FALSE);
                    if (!sp)
                    {
                        sp = namespacesearch(buf, nssym, qualified, FALSE);
                    }
                }
                if (sp && sp->storage_class == sc_typedef)
                {
                    SYMBOL *typedefSym = sp;
                    istypedef = TRUE;
                    if (isstructured(sp->tp) && !sp->templateLevel && throughClass)
                    {
                        sp = basetype(sp->tp)->sp;
                        sp->typedefSym = typedefSym;
                        *throughClass = TRUE;
                    }
                    else if (sp->tp->type == bt_typedef)
                    {
                        SYMBOL *sp1 = clonesym(sp);
                        sp1->mainsym = sp;
                        sp1->tp = sp->tp->btp;
                        sp = sp1;
                    }
                }
            }
            else
            {
                STRUCTSYM s;
                s.str = strSym;
                addStructureDeclaration(&s);
                sp = classsearch(buf, FALSE, FALSE);
                dropStructureDeclaration();
                if (!sp && templateNestingCount)
                {
                    *last = (TEMPLATESELECTOR *)Alloc(sizeof(TEMPLATESELECTOR));
                    (*last)->sym = NULL;
                    last = &(*last)->next;
                    *last = (TEMPLATESELECTOR *)Alloc(sizeof(TEMPLATESELECTOR));
                    (*last)->sym = strSym;
                    (*last)->templateParams = current;
                    (*last)->isTemplate = TRUE;
                    last = &(*last)->next;

                    *last = Alloc(sizeof(TEMPLATESELECTOR));
                    (*last)->name = litlate(buf);
                    if (hasTemplate)
                    {
                        (*last)->isTemplate = TRUE;
                        if (MATCHKW(lex, lt))
                        {
                            lex = GetTemplateArguments(lex, NULL, NULL, &(*last)->templateParams);
                        }
                        else if (MATCHKW(lex, classsel))
                        {
                            errorstr(ERR_NEED_SPECIALIZATION_PARAMETERS, buf);
                        }
                    }
                    last = &(*last)->next;
                    if (!MATCHKW(lex, classsel))
                        break;
                    lex = getsym();
                    finalPos = lex;

                }
            }
            if (sp && basetype(sp->tp)->type == bt_enum)
            {
                if (!MATCHKW(lex, classsel))
                    break;
                lex = getsym();
                finalPos = lex;
                strSym = sp;
                qualified = TRUE;
                break;
            }
            else if (sp)
            {
                if (sp->templateLevel && (!sp->instantiated || MATCHKW(lex, lt)))
                {
                    hasTemplateArgs = TRUE;
                    if (MATCHKW(lex, lt))
                    {
                        lex = GetTemplateArguments(lex, NULL, sp, &current);
                    }
                    else if (MATCHKW(lex, classsel))
                    {
                        currentsp = sp;
                        if (!istypedef && !noSpecializationError)// && !instantiatingTemplate)
                            errorsym(ERR_NEED_SPECIALIZATION_PARAMETERS, sp);
                    }
                    if (!MATCHKW(lex, classsel))
                        break;
                }
                else
                {
                    if (!MATCHKW(lex, classsel))
                        break;
                    if (hasTemplate && (basetype(sp->tp)->type != bt_templateparam || basetype(sp->tp)->templateParam->p->type != kw_template))
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, sp);
                    }
                }
            }
            else if (templateParamAsTemplate)
            {
                hasTemplateArgs = TRUE;
                if (MATCHKW(lex, lt))
                {
                    lex = GetTemplateArguments(lex, NULL, sp, &current);
                }
                else if (MATCHKW(lex, classsel))
                {
                    currentsp = sp;
                    if (!istypedef && !noSpecializationError)// && !instantiatingTemplate)
                        errorsym(ERR_NEED_SPECIALIZATION_PARAMETERS, sp);
                }
                if (!MATCHKW(lex, classsel))
                    break;
            }
            else if (!MATCHKW(lex, classsel))
                break;
            if (templateParamAsTemplate)
            {
                matchTemplateSpecializationToParams(current, templateParamAsTemplate->p->byTemplate.args, templateParamAsTemplate->argsym);
            }
            if (hasTemplateArgs)
            {
                if (currentsp)
                {
                    sp = currentsp;
                    if (inTemplateType)
                    {
                        deferred = TRUE;
                    }
                }
                else if (inTemplateType || sp && sp->tp->type == bt_templateselector)
                {
                    deferred = TRUE;
                }
                else
                {
                    if (isType)
                    {
                        TEMPLATEPARAMLIST *p = current;
                        while (p)
                        {
                            if (!p->p->byClass.dflt)
                                break;
                                
                            p = p->next;
                        }
                        if (p)
                            deferred = TRUE;
                    }
                    if (!deferred)
                    {
                        if (basetype(sp->tp)->type == bt_templateselector)
                        {
                            deferred = TRUE;
                        }
                        else
                        {
                            TEMPLATEPARAMLIST *p = current;
                            while (p)
                            {
                                if (p->p->usedAsUnpacked)
                                    break;

                                p = p->next;
                            }
                            if (p)
                                deferred = TRUE;
                            if (!deferred)
                            {
                                SYMBOL *sp1 = sp;
                                sp = GetClassTemplate(sp, current, FALSE);
                                if (!sp)
                                {
                                    if (templateNestingCount)
                                    {
                                        sp = sp1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (sp && !deferred)
                sp->tp = PerformDeferredInitialization (sp->tp, NULL);
            lex = getsym();
            finalPos = lex;
            if (deferred)
            {
                if (sp && sp->tp->type == bt_templateselector)
                {
                    TEMPLATESELECTOR *s = basetype(sp->tp)->sp->templateSelector;
                    while (s)
                    {
                        *last = (TEMPLATESELECTOR *)Alloc(sizeof(TEMPLATESELECTOR));
                        **last = *s;
                        last = &(*last)->next;
                        s = s->next;
                    }
                    templateSelector->next->templateParams = current;
                    templateSelector->next->isTemplate = TRUE;
                }
                else
                {
                    *last = (TEMPLATESELECTOR *)Alloc(sizeof(TEMPLATESELECTOR));
                    (*last)->sym = strSym;
                    last = &(*last)->next;
                    *last = (TEMPLATESELECTOR *)Alloc(sizeof(TEMPLATESELECTOR));
                    (*last)->sym = sp;
                    (*last)->templateParams = current;
                    (*last)->isTemplate = TRUE;
                    last = &(*last)->next;
                }
            }
            else if (sp && isstructured(sp->tp))
            {
                strSym = sp;
                if (!qualified)
                    nssym = NULL;
            }
            else if (sp && (sp->storage_class == sc_namespace || sp->storage_class == sc_namespacealias))
            {
                nssym = sp->nameSpaceValues;                    
            }
            else if (sp && (basetype(sp->tp)->type == bt_templateparam || basetype(sp->tp)->type == bt_templateselector))
            {
                *last = Alloc(sizeof(TEMPLATESELECTOR));
                (*last)->sym = strSym;
                last = &(*last)->next;
                *last = Alloc(sizeof(TEMPLATESELECTOR));
                (*last)->sym = sp;
                last = &(*last)->next;
            }
            else
            {
                if (!templateNestingCount || !sp)
                {
                    if (dependentType)
                        if (isstructured(dependentType))
                            errorstringtype(ERR_DEPENDENT_TYPE_DOES_NOT_EXIST_IN_TYPE, buf, basetype(dependentType));
                        else
                            errortype(ERR_DEPENDENT_TYPE_NOT_A_CLASS_OR_STRUCT, dependentType, NULL);
                    else
                        errorstr(ERR_QUALIFIER_NOT_A_CLASS_OR_NAMESPACE , buf);
                }
                lex = prevsym(placeholder);
                break;
            }
        }
        hasTemplate = FALSE;
        if (MATCHKW(lex, kw_template))
        {
            hasTemplate = TRUE;
            lex = getsym();
        }
        qualified = TRUE;
    }
    lex = prevsym(finalPos);
    if (templateSelector)
    {
        TYPE *tp = Alloc(sizeof(TYPE));
        tp->type = bt_templateselector;
        tp->rootType = tp;
        *sym = makeID(sc_global, tp, NULL, AnonymousName());
        (*sym)->templateSelector = templateSelector;
        tp->sp = *sym;
    }
    else if (qualified)
    {
        if (strSym && sym)
            *sym = strSym;
            
        if (ns)
            if (nssym && nssym->name)
                *ns = nssym;
            else
                *ns = NULL;
        else
            error(ERR_QUALIFIED_NAME_NOT_ALLOWED_HERE);
    }
    return lex;
}
SYMBOL *classdata(char *name, SYMBOL *cls, SYMBOL *last, BOOLEAN isvirtual, BOOLEAN tagsOnly)
{
    SYMBOL *rv = NULL;
	BASECLASS *bc = cls->baseClasses;
    if (cls->storage_class == sc_typedef)
        cls = basetype(cls->tp)->sp;
    if (cls->templateLevel && cls->templateParams)
    {
        if (!basetype(cls->tp)->syms)
        {
            TemplateClassInstantiate(cls, cls->templateParams, FALSE, sc_global);
        }
    }
	while (bc && !rv)
	{
		if (!strcmp(bc->cls->name, name))
		{
			rv = bc->cls;
			rv->temp = bc->isvirtual;
		}
		bc = bc->next;
	}

    if (!rv && !tagsOnly)
        rv = search(name, basetype(cls->tp)->syms);
    if (!rv)
        rv = search(name, basetype(cls->tp)->tags);
    if (rv)
    {
        if (!last || ((last == rv || sameTemplate(last->tp, rv->tp) || (rv->mainsym && rv->mainsym == last->mainsym)) && (((isvirtual && isvirtual == last->temp) || 
                                                                                     ismember(rv)) 
            || (((last->storage_class == sc_type && rv->storage_class == sc_type) || (last->storage_class == sc_typedef && rv->storage_class == sc_typedef)) 
            && (last->parentClass == rv->parentClass)) || last->parentClass->mainsym == rv->parentClass->mainsym)))
        {
        }
        else
        {
            rv = (SYMBOL *)-1;
        }
    }
    else
    {
        BASECLASS *lst = cls->baseClasses;
        rv = last;
        while (lst)
        {
            rv = classdata(name, lst->cls, rv, isvirtual | lst->isvirtual, tagsOnly);
            if (rv == (SYMBOL *)-1)
                break;
            lst = lst->next;
        }
    }
    return rv;
}
SYMBOL *templatesearch(char *name, TEMPLATEPARAMLIST *arg)
{
    while (arg)
    {
        if (arg->argsym && !strcmp(arg->argsym->name, name))
            if (arg->p->type == kw_template && arg->p->byTemplate.dflt)
                return arg->p->byTemplate.dflt;
            else
                return arg->argsym;
        arg = arg->next;
    }
    return NULL;
}
TEMPLATEPARAMLIST *getTemplateStruct(char *name)
{
    SYMBOL *cls = getStructureDeclaration(), *base = cls;
    while (cls)
    {
        TEMPLATEPARAMLIST *arg = cls->templateParams;
        if (arg)
        {
            while (arg)
            {
                if (!strcmp(arg->argsym->name, name))
                    return arg;
                arg = arg->next;
            }
        }
        cls = cls->parentClass;
    }
    return NULL;
}
SYMBOL *classsearch(char *name, BOOLEAN tagsOnly, BOOLEAN toErr)
{
    SYMBOL *rv = NULL;
    SYMBOL *cls = getStructureDeclaration();
    STRUCTSYM *s = structSyms;
    while (s && s->tmpl && !rv)
    {
        rv = templatesearch(name, s->tmpl);
        s = s->next;
    }
    if (cls && !rv)
    {
        /* optimize for the case where the final class has what we need */
        while (cls && !rv)
        {
            if (!tagsOnly)
                rv = search(name, basetype(cls->tp)->syms);
            if (!rv)
                rv = search(name, basetype(cls->tp)->tags);
            if (!rv && cls->baseClasses)
            {
                rv = classdata(name, cls, NULL, FALSE, tagsOnly);
                if (rv == (SYMBOL *)-1)
                {
                    rv = NULL;
                    if (toErr)
                        errorstr(ERR_AMBIGUOUS_MEMBER_DEFINITION, name);
                    break;
                }
            }
//            if (!rv && cls->templateParams)
//                rv = templatesearch(name, cls->templateParams);
            cls = cls->parentClass;
        }
    }
    while (s && !rv)
    {
        if (s->tmpl)
            rv = templatesearch(name, s->tmpl);
        s = s->next;
    }
        cls = getStructureDeclaration();
    if (cls && !rv)
    {
        /* optimize for the case where the final class has what we need */
        while (cls && !rv)
        {
            if (!rv && cls->templateParams)
                rv = templatesearch(name, cls->templateParams);
            cls = cls->parentClass;
        }
    }
    return rv;
}
SYMBOL *finishSearch(char *name, SYMBOL *encloser, NAMESPACEVALUES *ns, BOOLEAN tagsOnly, BOOLEAN throughClass, BOOLEAN namespaceOnly)
{
    SYMBOL *rv = NULL;
    if (!encloser && !ns && !namespaceOnly)
    {
        SYMBOL *ssp;
        if (cparams.prm_cplusplus || !tagsOnly)
            rv = search(name, localNameSpace->syms);
        if (!rv)
            rv = search(name, localNameSpace->tags);
        if (lambdas)
        {
            LAMBDA *srch = lambdas;
            while (srch && !rv)
            {
                if (cparams.prm_cplusplus || !tagsOnly)
                    rv = search(name, srch->oldSyms);
                if (!rv)
                    rv = search(name, srch->oldTags);
                srch = srch->next;
            }
        }
        if (!rv)
            rv = namespacesearch(name, localNameSpace, FALSE, tagsOnly);
        if (!rv && enumSyms)
            rv = search(name, enumSyms->tp->syms);
        if (!rv)
        {
            if (lambdas)
            {
                if (lambdas->lthis)
                {
                    rv = search(name, basetype(lambdas->lthis->tp)->btp->syms);
                    if (rv)
                        rv->throughClass = TRUE;
                }
            }
            if (!rv)
            {
                rv = classsearch(name, tagsOnly, TRUE);
                if (rv)
                    rv->throughClass = TRUE;
            }
        }
        else
        {
            rv->throughClass = FALSE;
        }
        ssp = getStructureDeclaration();
        if (!rv && (!ssp || ssp->nameSpaceValues != globalNameSpace))
        {
            rv = namespacesearch(name, globalNameSpace, FALSE, tagsOnly);
            if (rv)
                rv->throughClass = FALSE;
        }
    }
    else
    {
        if (namespaceOnly && !ns)
        {
            rv = namespacesearch(name, globalNameSpace, FALSE, tagsOnly);
            if (rv)
                rv->throughClass = FALSE;
        }
        else if (encloser)
        {
            STRUCTSYM l;
            l.str = (void *)encloser;
            addStructureDeclaration(&l);
            rv = classsearch(name, tagsOnly, TRUE);
            dropStructureDeclaration();
            if (rv)
                rv->throughClass = throughClass;
        }
        else
        {
            LIST *rvl;
            unvisitUsingDirectives(ns);
            rvl = tablesearchinline(name, ns, tagsOnly);
            if (rvl)
            {
                rv = rvl->data;
                rv->throughClass = FALSE;
            }
        }
    }
    return rv;
}
LEXEME *nestedSearch(LEXEME *lex, SYMBOL **sym, SYMBOL **strSym, NAMESPACEVALUES **nsv, 
                     BOOLEAN *destructor, BOOLEAN *isTemplate, BOOLEAN tagsOnly, enum e_sc storage_class, BOOLEAN errIfNotFound, BOOLEAN isType)
{
    SYMBOL *encloser = NULL;
    NAMESPACEVALUES *ns = NULL;
    BOOLEAN throughClass = FALSE;
    LEXEME *placeholder = lex;
    BOOLEAN hasTemplate = FALSE;
    BOOLEAN namespaceOnly = FALSE;
    *sym = NULL;
    
    if (!cparams.prm_cplusplus && (!chosenAssembler->msil || !chosenAssembler->msil->allowExtensions))
    {
        if (ISID(lex))
        {
            if (tagsOnly)
                *sym = tsearch(lex->value.s.a);
            else
                *sym = gsearch(lex->value.s.a);
        }
        return lex;
    }
//    if (MATCHKW(lex, classsel))
//        namespaceOnly = TRUE;
    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly, storage_class, isType);
    if (cparams.prm_cplusplus)
    {
            
        if (MATCHKW(lex, compl))
        {
            if (destructor)
            {
                *destructor = TRUE;
            }
            else
            {
                error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
            }
            lex = getsym();
        }
        else if (MATCHKW(lex, kw_template))
        {
            lex = getsym();
            if (isTemplate)
                *isTemplate = TRUE;
            hasTemplate = TRUE;
        }
    }
    if (ISID(lex) || MATCHKW(lex, kw_operator))
    {
        if (encloser && encloser->tp->type == bt_templateselector)
        {
            TEMPLATESELECTOR *l;
            l = encloser->templateSelector;
            while (l->next)
                l = l->next;
            if (destructor && *destructor && !encloser->templateSelector->next->next)
            {
                l->next = Alloc(sizeof(TEMPLATESELECTOR));
                l->next->name = l->sym->name;
                l = l->next;
            }
            *sym = makeID(sc_type, encloser->tp, NULL, l->name);
        }
        else
        {
            if (!ISID(lex))
            {
                char buf[512];
                int ovdummy;
                lex = getIdName(lex, NULL, buf, &ovdummy, NULL);
                *sym = finishSearch(buf, encloser, ns, tagsOnly, throughClass, namespaceOnly);
                if (!*sym)
                    encloser = NULL;
                if (errIfNotFound && !*sym)
                {
                    errorstr(ERR_UNDEFINED_IDENTIFIER, buf);
                } 
            }
            else
            {
                TEMPLATEPARAMLIST *tparam = TemplateLookupSpecializationParam(lex->value.s.a);
                if (tparam)
                {
                    *sym = tparam->argsym;
                }
                else
                {
                    *sym = finishSearch(lex->value.s.a, encloser, ns, tagsOnly, throughClass, namespaceOnly);
                    if (!*sym)
                        encloser = NULL;
                    if (errIfNotFound && !*sym)
                    {
                        errorstr(ERR_UNDEFINED_IDENTIFIER, lex->value.s.a);
                    } 
                }
            }
        }
    }
    else if (destructor && *destructor)
    {
        *destructor = FALSE;
        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
    }
    if (*sym && hasTemplate)
    {
        if (!(*sym)->templateLevel && ((*sym)->tp->type != bt_templateparam || (*sym)->tp->templateParam->p->type != kw_template) && (*sym)->tp->type != bt_templateselector && (*sym)->tp->type != bt_templatedecltype)
        {
            if ((*sym)->storage_class == sc_overloads)
            {
                HASHREC *hr = basetype((*sym)->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    if (sp->templateLevel)
                        break;
                    hr = hr->next;
                }
                if (!hr)
                    errorsym(ERR_NOT_A_TEMPLATE, *sym);
            }
            else
            {
                errorsym(ERR_NOT_A_TEMPLATE, *sym);
            }
        }
    }
    if (encloser&& strSym)
        *strSym = encloser;
    if (nsv)
        if (ns && ns->name)
            *nsv = ns;
        else
            *nsv = NULL;
    else if (!*sym)
        lex = prevsym(placeholder);
    return lex;
}
LEXEME *getIdName(LEXEME *lex, SYMBOL *funcsp, char *buf, int *ov, TYPE **castType)
{
    buf[0] = 0;
    if (ISID(lex))
    {
        strcpy(buf, lex->value.s.a);
    }
    else if (MATCHKW(lex, kw_operator))
    {
        lex = getsym();
        if (ISKW(lex) && lex->kw->key >= kw_new && lex->kw->key <= compl)
        {
            enum e_kw kw = lex->kw->key;
            switch(kw)
            {
                case openpa:
                    lex = getsym();
                    if (!MATCHKW(lex, closepa))
                    {
                        needkw(&lex, closepa);
                        lex = backupsym();
                    }
                    break ;
                case openbr:
                    lex = getsym();
                    if (!MATCHKW(lex, closebr))
                    {
                        needkw(&lex, closebr);
                        lex = backupsym();
                    }
                    break ;
                case kw_new:
                case kw_delete:
                    lex = getsym();
                    if (!MATCHKW(lex, openbr))
                    {
                        lex = backupsym();
                    }
                    else
                    {
                        kw = kw - kw_new + compl + 1;
                        lex = getsym();
                        if (!MATCHKW(lex,closebr))
                        {
                            needkw(&lex, closebr);
                            lex = backupsym();
                        }
                    }
                    break;
                default:
                    break;
            }
            strcpy(buf, overloadNameTab[*ov = kw - kw_new + CI_NEW]);
        }
        else if (ISID(lex) || startOfType(lex, FALSE)) // potential cast operator
        {
            TYPE *tp = NULL;
            noSpecializationError++;
            lex = get_type_id(lex, &tp, funcsp, sc_cast, TRUE, TRUE);
            noSpecializationError--;
            if (!tp)
            {
                errorstr(ERR_INVALID_AS_OPERATOR,"");
                tp = &stdint;
            }
            if (castType)
            {
                *castType = tp;
                if (isautotype(tp) & !lambdas) // make an exception so we can compile templates for lambdas
                    error(ERR_AUTO_NOT_ALLOWED_IN_CONVERSION_FUNCTION);
            }
            strcpy(buf, overloadNameTab[*ov = CI_CAST]);
        }
        else if (lex->type == l_astr)
        {
            LEXEME *placeholder = lex;
            SLCHAR *xx = (SLCHAR *)lex->value.s.w;
            if (xx->count)
                error(ERR_OPERATOR_LITERAL_EMPTY_STRING);
            lex = getsym();
            
            if (ISID(lex))
            {
                my_sprintf(buf, "%s@%s", overloadNameTab[CI_LIT], lex->value.s.a);
                *ov = CI_LIT;
            }
            else
            {
                error(ERR_OPERATOR_LITERAL_NEEDS_ID); 
                prevsym(placeholder);
            }
        }
        else
        {
            if (ISKW(lex))
                errorstr(ERR_INVALID_AS_OPERATOR, lex->kw->name);
            else
                errorstr(ERR_INVALID_AS_OPERATOR,"");
            lex = backupsym();
        }        
    }
    return lex;
}
LEXEME *id_expression(LEXEME *lex, SYMBOL *funcsp, SYMBOL **sym, SYMBOL **strSym, 
                      NAMESPACEVALUES **nsv, BOOLEAN *isTemplate, BOOLEAN tagsOnly, BOOLEAN membersOnly, char *idname)
{
    SYMBOL *encloser = NULL;
    NAMESPACEVALUES *ns = NULL;
    BOOLEAN throughClass = FALSE;
    TYPE *castType = NULL;
    LEXEME *placeholder = lex;
    char buf[512];
    int ov = 0;
    BOOLEAN hasTemplate = FALSE;
    BOOLEAN namespaceOnly = FALSE;
    
    *sym = NULL;
    
    if (MATCHKW(lex, classsel))
        namespaceOnly = TRUE;
    if (!cparams.prm_cplusplus && !chosenAssembler->msil)
    {
        if (ISID(lex))
        {
            if (idname)
                strcpy(idname, lex->value.s.a);
            if (tagsOnly)
                *sym = tsearch(lex->value.s.a);
            else
            {
                SYMBOL *ssp = getStructureDeclaration();
                if (ssp)
                {
                    *sym = search(lex->value.s.a, ssp->tp->syms);
                }
                if (*sym == NULL)
                    *sym = gsearch(lex->value.s.a);
            }
        }
        return lex;
    }
    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly, sc_global, FALSE);
    if (MATCHKW(lex, compl))
    {
        lex = getsym();
        if (ISID(lex))
        {
            if (encloser)
            {
                if (strcmp(encloser->name, lex->value.s.a))
                {
                    error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                }
                *sym = finishSearch(overloadNameTab[CI_DESTRUCTOR], 
                                    encloser, ns, tagsOnly, throughClass, namespaceOnly);
            }
        }
        else
        {
            error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
        }
    }
    else
    {
        if (MATCHKW(lex, kw_template))
        {
            if (isTemplate)
                *isTemplate = TRUE;
            hasTemplate = TRUE;
            lex = getsym();
        }
        lex = getIdName(lex, funcsp, buf, &ov, &castType);
        if (buf[0])
        {
            if (!encloser && membersOnly)
                encloser = getStructureDeclaration();
            *sym = finishSearch(ov == CI_CAST ? overloadNameTab[CI_CAST] : buf, 
                                encloser, ns, tagsOnly, throughClass, namespaceOnly);
            if (*sym && hasTemplate)
            {
                if ((*sym)->storage_class == sc_overloads)
                {
                    HASHREC *hr = basetype((*sym)->tp)->syms->table[0];
                    while (hr)
                    {
                        SYMBOL *sp = (SYMBOL *)hr->p;
                        if (sp->templateLevel)
                            break;
                        hr = hr->next;
                    }
                    if (!hr)
                        errorsym(ERR_NOT_A_TEMPLATE, *sym);
                }
                else
                {
                    errorsym(ERR_NOT_A_TEMPLATE, *sym);
                }
            }
        }
    }
    if (encloser&& strSym)
        *strSym = encloser;
    if (nsv)
        if (ns && ns->name)
            *nsv = ns;
        else
            *nsv = NULL;
    else if (!*sym)
        prevsym(placeholder);
    if (!*sym && idname)
    {
        strcpy(idname, buf);
    }
    return lex;
}
SYMBOL *LookupSym(char *name)
{
    SYMBOL *rv = NULL;
    if (!cparams.prm_cplusplus)
    {
        return gsearch(name);
    }
    rv = search(name, localNameSpace->syms);
    if (!rv)
        rv = search(name, localNameSpace->tags);
    if (!rv)
        rv = namespacesearch(name, localNameSpace, FALSE, FALSE);
    if (!rv)
        rv = namespacesearch(name, globalNameSpace, FALSE, FALSE);
    return rv;
}
static BOOLEAN isFriend(SYMBOL *cls, SYMBOL *frnd)
{
    if (cls && frnd)
    {
        BASECLASS *bases;
        LIST *l = cls->friends;
        while (l)
        {
            SYMBOL *sym = (SYMBOL *)l->data;
            if (sym == frnd || sym == frnd->parentTemplate)
                return TRUE;
            if (sym->templateLevel)
            {
                LIST *instants = sym->instantiations;
                while (instants)
                {
                    if ((SYMBOL *)instants->data == frnd || (SYMBOL *)instants->data == frnd->parentTemplate)
                        return TRUE;
                    instants = instants->next;
                }
            }
            l = l->next;
        }
    }
    return FALSE;
}
// works by searching the tree for the base or member symbol, and stopping any
// time the access wouldn't work.  If the symbol is found it is accessible.
static BOOLEAN isAccessibleInternal(SYMBOL *derived, SYMBOL *currentBase, 
                                 SYMBOL *member, SYMBOL *funcsp, 
                                 enum e_ac minAccess, int level, 
                                 BOOLEAN asAddress, BOOLEAN friendly)
{
    BASECLASS *lst;
    HASHREC *hr;
    SYMBOL *ssp;
    BOOLEAN matched;
    TYPE *tp;
    if (!cparams.prm_cplusplus)
        return TRUE;
    ssp = getStructureDeclaration();
    if (ssp)
    {
        if (ssp == member)
            return TRUE;
    }
    if (isFriend(derived, funcsp) || (funcsp && isFriend(derived, funcsp->parentClass))
        || isFriend(derived, ssp) || isFriend(member->parentClass, funcsp))
        friendly = TRUE;
    if (!basetype(currentBase->tp)->syms)
        return FALSE;
    hr = basetype(currentBase->tp)->syms->table[0];
    matched = FALSE;
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym == member || sym == member->mainsym)
        {
            matched = TRUE;
            break;
        }
        if (sym->storage_class == sc_overloads && isfunction(member->tp) && sym->tp->syms)
        {
            HASHREC *hr1 = sym->tp->syms->table[0];
            while (hr1)
            {
                SYMBOL *sym = (SYMBOL *)hr1->p;
                if (sym == member || sym == member->mainsym)
                {
                    break;
                }
                else if (sym->instantiations)
                {
                    LIST *lst = sym->instantiations;
                    while (lst)
                    {
                        if (lst->data == member)
                            break;
                        lst = lst->next;
                    }
                    if (lst)
                    {
                        break;
                    }
                }
                hr1 = hr1->next;
            }
            if (hr1)
            {
                matched = TRUE;
                break;
            }
        }
        hr = hr->next;
    }
    if (!matched)
    {
        hr = basetype(currentBase->tp)->tags->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym == member || sym == member->mainsym || sameTemplate(sym->tp, member->tp))
            {
                matched = TRUE;
                break;
            }
            else if (sym->instantiations)
            {
                LIST *lst = sym->instantiations;
                while (lst)
                {
                    if (lst->data == member)
                        break;
                    lst = lst->next;
                }
                if (lst)
                {
                    matched = TRUE;
                    break;
                }
            }
            hr = hr->next;
        }
    }
    if (matched)
    {
        SYMBOL *sym = member;
        return friendly || ((level == 0 || level == 1 && (minAccess < ac_public || sym->access == ac_public))
        &&(derived == currentBase || sym->access != ac_private)) || sym->access >= minAccess;
    }
    lst = currentBase->baseClasses;
    while (lst)
    {
        SYMBOL *sym = lst->cls;
        sym = basetype(sym->tp)->sp;
        // we have to go through the base classes even if we know that a normal
        // lookup wouldn't work, so we can check their friends lists...
        if (sym == member || sameTemplate(sym->tp, member->tp))
        {
            return ((level == 0 || level == 1 && (minAccess < ac_public || sym->access == ac_public))
                &&(derived == currentBase || sym->access != ac_private))|| sym->access >= minAccess;
        }
        if (isAccessibleInternal(derived, sym, member, funcsp, level != 0 && (lst->accessLevel == ac_private || minAccess == ac_private) ? ac_none : minAccess, level+1, asAddress, friendly))
            return TRUE;
        lst = lst->next;
    }
    return FALSE;
}
BOOLEAN isAccessible(SYMBOL *derived, SYMBOL *currentBase, 
                                 SYMBOL *member, SYMBOL *funcsp, 
                                 enum e_ac minAccess, BOOLEAN asAddress)
{
    return member->accessibleTemplateArgument || 
        isAccessibleInternal(derived, currentBase, member, funcsp, minAccess, 0, asAddress, FALSE);
}
BOOLEAN isExpressionAccessible(SYMBOL *derived, SYMBOL *sym, SYMBOL *funcsp, EXPRESSION *exp, BOOLEAN asAddress)
{
    if (sym->parentClass)
    {
        SYMBOL *ssp = getStructureDeclaration();
        BOOLEAN throughClass = sym->throughClass;
        if (exp)
        {
                throughClass = TRUE;
        }
        
        if (ssp && throughClass && (ssp == sym->parentClass || classRefCount(sym->parentClass, ssp)))
        {
            if (!isAccessible(ssp, ssp, sym, funcsp, ac_protected, asAddress))
                return FALSE;
        }
        else
        {
            if (!isAccessible(derived ? derived : sym->parentClass, sym->parentClass, sym, funcsp, ac_public, asAddress))
                return FALSE;
        }
    }
    return TRUE;    
}
BOOLEAN checkDeclarationAccessible(TYPE *tp, SYMBOL *funcsp)
{
    while (tp)
    {
        if (isstructured(tp) || tp->type == bt_enum)
        {
            SYMBOL *sym = basetype(tp)->sp;
            if (sym->parentClass)
            {
                SYMBOL *ssp = getStructureDeclaration();
                if (ssp && (ssp == sym->parentClass || classRefCount(sym->parentClass, ssp)))
                {
                    if (!isAccessible(ssp, ssp, sym, funcsp, ac_protected, FALSE))
                    {
                        currentErrorLine = 0;
                        errorsym(ERR_CANNOT_ACCESS, tp->sp);		
                        return FALSE;
                    }
                }
                else
                {
                    if (!isAccessible(sym->parentClass, sym->parentClass, sym, funcsp, ac_public, FALSE))
                    {
                        currentErrorLine = 0;
                        errorsym(ERR_CANNOT_ACCESS, basetype(tp)->sp);		
                        return FALSE;
                    }
                }
            }
        }
        else if (isfunction(tp))
        {
            HASHREC *hr = basetype(tp)->syms->table[0];
            while (hr)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                if (!checkDeclarationAccessible(sym->tp, funcsp))
                    return FALSE;
                hr = hr->next;
            }            
        }
        tp = tp->btp;
    }
    return TRUE;
}
static LIST *searchNS(SYMBOL *sp, SYMBOL *nssp, LIST *in)
{
    if (nssp)
    {
        NAMESPACEVALUES *ns = nssp->nameSpaceValues;
        LIST *x = namespacesearchInternal(sp->name, ns, TRUE, FALSE, FALSE);
        if (x)
        {
            LIST *rv = x;
            if (in)
            {
                while (x->next)
                    x = x->next;
                x->next = in;
            }
            return rv;
        }
    }
    return in;
}
SYMBOL *lookupSpecificCast(SYMBOL *sp, TYPE *tp)
{
    return getUserConversion(F_CONVERSION | F_STRUCTURE, tp, sp->tp, NULL, NULL, NULL, NULL, NULL, FALSE);
}
SYMBOL *lookupNonspecificCast(SYMBOL *sp, TYPE *tp)
{
    return getUserConversion(F_CONVERSION, tp, sp->tp, NULL, NULL, NULL, NULL, NULL, TRUE);
}
SYMBOL *lookupIntCast(SYMBOL *sp, TYPE *tp, BOOLEAN implicit)
{
    return getUserConversion(F_CONVERSION | F_INTEGER, tp, sp->tp, NULL, NULL, NULL, NULL, NULL, implicit);
}
SYMBOL *lookupArithmeticCast(SYMBOL *sp, TYPE *tp, BOOLEAN implicit)
{
    return getUserConversion(F_CONVERSION | F_ARITHMETIC, tp, sp->tp, NULL, NULL, NULL, NULL, NULL, implicit);
}
SYMBOL *lookupPointerCast(SYMBOL *sp, TYPE *tp)
{
    return getUserConversion(F_CONVERSION | F_POINTER, tp, sp->tp, NULL, NULL, NULL, NULL, NULL, TRUE);
}
static LIST *structuredArg(SYMBOL *sp, LIST *in, TYPE *tp)
{
    return searchNS(sp, basetype(tp)->sp->parentNameSpace, in) ;
}
static LIST *searchOneArg(SYMBOL *sp, LIST *in, TYPE *tp);
static LIST *funcArg(SYMBOL *sp, LIST *in, TYPE *tp)
{
    HASHREC **hr = basetype(tp)->syms->table;
    while (*hr)
    {
        SYMBOL *sym = (SYMBOL *)(*hr)->p;
        in = searchOneArg(sp, in, sym->tp);
        hr = &(*hr)->next;
    }
    in = searchOneArg(sp, in , basetype(tp)->btp);
    return in;
}
static LIST *searchOneArg(SYMBOL *sp, LIST *in, TYPE *tp)
{
    if (ispointer(tp)|| isref(tp))
        return searchOneArg(sp, in, basetype(tp)->btp);
    if (isarithmetic(tp))
        return in;
    if (isstructured(tp))
        return structuredArg(sp, in, tp);
    if (basetype(tp)->type  == bt_enum)
        return searchNS(sp, basetype(tp)->sp->parentNameSpace, in) ;
    if (isfunction(tp))
        return funcArg(sp, in, tp);
    // member pointers...
    return in;
}
static void weedToFunctions(LIST **lst)
{
    while (*lst)
    {
        SYMBOL *sp = (SYMBOL *)(*lst)->data;
        if (sp->storage_class != sc_overloads)
            *lst = (*lst)->next;
        else
            lst = &(*lst)->next;
    }
}
static void  GatherConversions(SYMBOL *sp, SYMBOL **spList, int n, FUNCTIONCALL *args, 
                               TYPE *atp, enum e_cvsrn **icsList, int **lenList, 
                               int argCount, SYMBOL ***funcList, BOOLEAN usesInitList)
{
    int i;
    for (i=0; i < n; i++)
    {
        int j;
        if (spList[i])
        {
            enum e_cvsrn arr[500][10];
            int counts[500];
            SYMBOL *funcs[200];
            BOOLEAN t;
            memset(counts, 0, argCount * sizeof(int));
            for (j = i + 1; j < n; j++)
                if (spList[i] == spList[j])
                    spList[j] = 0;
            memset(funcs, 0, sizeof(funcs));
            t = getFuncConversions(spList[i], args, atp, sp->parentClass, (enum e_cvsrn *)arr, counts, argCount, funcs, usesInitList);
            if (!t)
            {
                spList[i] = NULL;
            }
            else
            {
                int n = 0;
                for (j=0; j < argCount; j++)
                    n += counts[j];
                icsList[i] = (enum e_cvsrn *)Alloc(sizeof(enum e_cvsrn) * n);
                memcpy(icsList[i], arr, n * sizeof(enum e_cvsrn));
                lenList[i] = (int *)Alloc(sizeof(int) * argCount);
                memcpy(lenList[i], counts, argCount * sizeof(int));
                funcList[i] = (SYMBOL **)Alloc(sizeof(SYMBOL *) * argCount);
                memcpy(funcList[i], funcs, argCount * sizeof(SYMBOL *));
            }
        }
    }
}
enum e_ct { conv, user, ellipses };
static BOOLEAN ismem(EXPRESSION *exp)
{
    switch (exp->type)
    {
        case en_global:
        case en_label:
        case en_pc:
        case en_auto:
        case en_threadlocal:
            return TRUE;
        case en_thisref:
            exp = exp->left;
            if (exp->v.func->sp->isConstructor || exp->v.func->sp->isDestructor)
                return FALSE;
            /* fallthrough */
        case en_func:
        {
            TYPE *tp = exp->v.func->sp->tp;
            if (tp->type == bt_aggregate || !isfunction(tp))
                return FALSE;
            tp = basetype(tp)->btp;
            return ispointer(tp) || isref(tp);
        }
        case en_add:
        case en_sub:
        case en_structadd:
            return ismem(exp->left) || ismem(exp->right);
        case en_l_p:
            return (exp->left->type == en_auto && exp->left->v.sp->thisPtr);
        default:
            return FALSE;
    }
}
static TYPE *toThis(TYPE *tp)
{
    TYPE *tpx;
    if (ispointer(tp))
        return tp;
    tpx = Alloc(sizeof(TYPE));
    tpx->type = bt_pointer;
    tpx->size = getSize(bt_pointer);
    tpx->btp = tp;
    tpx->rootType = tpx;
    return tpx;
}
static int compareConversions(SYMBOL *spLeft, SYMBOL *spRight, enum e_cvsrn *seql, enum e_cvsrn *seqr,
                              TYPE *ltype, TYPE *rtype, TYPE *atype, EXPRESSION *expa, 
                              SYMBOL *funcl, SYMBOL *funcr, 
                              int lenl, int lenr, BOOLEAN fromUser)
{
    enum e_ct xl=conv, xr=conv;
    int lderivedfrombase = 0, rderivedfrombase = 0;
    int rankl, rankr;
    int i,j;
    int n,q;
    // must be of same general type, types are standard conversion, user defined conversion, ellipses
    for (i=0; i < lenl; i++)
    {
        if (seql[i] == CV_ELLIPSIS)
            xl = ellipses;
        if (xl != ellipses && seql[i] == CV_USER)
            xl = user;
    }
    for (i=0; i < lenr; i++)
    {
        if (seqr[i] == CV_ELLIPSIS)
            xr = ellipses;
        if (xr != ellipses && seqr[i] == CV_USER)
            xr = user;
    }
    if (xl != xr)
    {
        if (xl < xr)
            return -1;
        else
            return 1;
    }
    if (xl == conv)
    {
        // one seq is a subseq of the other
        int l = 0, r = 0;
        for (;l < lenl; l++)
            if (seql[l] == CV_DERIVEDFROMBASE || seql[l] == CV_LVALUETORVALUE)
                lderivedfrombase++;
        for (;r < lenr; r++)
            if (seqr[r] == CV_DERIVEDFROMBASE || seqr[r] == CV_LVALUETORVALUE)
                rderivedfrombase++;
        l=0, r = 0;
        for (;l < lenl && r < lenr;)
        {
            BOOLEAN cont = FALSE;
            switch (seql[l])
            {   
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    l++;
                    cont = TRUE;
                    break;
                default:
                    break;
            }
            switch (seqr[r])
            {   
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    r++;
                    cont = TRUE;
                    break;
                default:
                    break;
            }
            if (cont)
                continue;
            if (seql[l] != seqr[r])
                break;
            l++, r++;
        }
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (l == lenl && r != lenr)
        {
            return -1;
        }
        else if (l != lenl && r == lenr)
        {
            return 1;
        }
        // compare ranks
        rankl = CV_IDENTITY;
        for (l=0; l < lenl; l++)
            if (rank[seql[l]] > rankl && seql[l] != CV_DERIVEDFROMBASE)
                rankl = rank[seql[l]];
        rankr = CV_IDENTITY;
        for (r=0; r < lenr; r++)
            if (rank[seqr[r]] > rankr && seqr[r] != CV_DERIVEDFROMBASE)
                rankr = rank[seqr[r]];
        if (rankl < rankr)
            return -1;
        else if (rankr < rankl)
            return 1;
        else if (lenl < lenr)
        {
            return -1;
        }
        else if (lenr < lenl)
        {
            return 1;
        }
        else // ranks are same, do same rank comparisons
        {
            TYPE *tl = ltype, *tr = rtype, *ta = atype;
            // check if one or the other but not both converts a pointer to bool
            rankl = 0;
            for (l=0; l < lenl; l++)
                if (seql[l] == CV_BOOLCONVERSION)
                    rankl = 1;
            rankr = 0;
            for (r=0; r < lenr; r++)
                if (seqr[r] == CV_BOOLCONVERSION)
                    rankr = 1;
            if (rankl != rankr)
            {
                if (rankl)
                    return 1;
                else 
                    return -1;
            }
            if (fromUser)
            {
                // conversion from pointer to base class to void * is better than pointer
                // to derived class to void *
                if (ispointer(ta) &&basetype(basetype(ta)->btp)->type == bt_void)
                {
                    SYMBOL *second = basetype(basetype(tl)->btp)->sp;
                    SYMBOL *first = basetype(basetype(tr)->btp)->sp;
                    int v;
                    v = classRefCount(first, second);
                    if (v == 1)
                        return 1;
                    v = classRefCount(second, first);
                    if (v == 1)
                        return -1;
                }
                                
            }
            else if (ta)
            {
                // conversion to pointer to base class is better than conversion to void *
                if (ispointer(tl) && ispointer(ta) && basetype(basetype(tl)->btp)->type == bt_void)
                {
                    if (isstructured(basetype(ta)->btp))
                    {
                        if (ispointer(tr) && isstructured(basetype(tr)->btp))
                        {
                            SYMBOL *derived = basetype(basetype(ta)->btp)->sp;
                            SYMBOL *base = basetype(basetype(tr)->btp)->sp;
                            int v = classRefCount(base, derived);
                            if (v == 1)
                                return 1;
                        }
                    }
                }
                else if (ispointer(tr) && ispointer(ta) && basetype(basetype(tr)->btp)->type == bt_void)
                {
                    if (isstructured(basetype(ta)->btp))
                    {
                        if (ispointer(tl) && isstructured(basetype(tl)->btp))
                        {
                            SYMBOL *derived = basetype(basetype(ta)->btp)->sp;
                            SYMBOL *base = basetype(basetype(tl)->btp)->sp;
                            int v = classRefCount(base, derived);
                            if (v == 1)
                                return -1;
                        }
                    }
                }
            }
            // various rules for the comparison of two pairs of structures
            if (ispointer(ta) && ispointer(tr) && ispointer(tl))
            {
                ta = basetype(ta)->btp;
                tl = basetype(tl)->btp;
                tr = basetype(tr)->btp;
            }
            else
            {
                if (isref(ta))
                    ta = basetype(ta)->btp;
                if (isref(tl))
                    tl = basetype(tl)->btp;
                if (isref(tr))
                    tr = basetype(tr)->btp;
            }
            if (isstructured(ta) && isstructured(tl) && isstructured(tr))
            {
                ta = basetype(ta);
                tl = basetype(tl);
                tr = basetype(tr);
                if (fromUser)
                {
                    if (classRefCount(ta->sp, tl->sp ) == 1 && classRefCount(ta->sp, tr->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                                if (lderivedfrombase > rderivedfrombase)
                                    return -1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return 1;
                                else
                                    return 0;
                            return -1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return 1;
                        }
                    }
                }
                else
                {
                    if (classRefCount(tl->sp, ta->sp ) == 1 && classRefCount(tr->sp, ta->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                                if (lderivedfrombase > rderivedfrombase)
                                    return 1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return -1;
                                else
                                    return 0;
                            return 1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return -1;
                        }
                    }
                }
            }
            if (basetype(ta)->type == bt_memberptr && basetype(tl)->type == bt_memberptr && basetype(tr)->type == bt_memberptr)
            {
                ta = basetype(ta);
                tl = basetype(tl);
                tr = basetype(tr);
                if (fromUser)
                {
                    if (classRefCount(tl->sp, ta->sp ) == 1 && classRefCount(tr->sp, ta->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                                if (lderivedfrombase > rderivedfrombase)
                                    return 1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return -1;
                                else
                                    return 0;
                            return 1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return -1;
                        }
                    }
                }
                else
                {
                    if (classRefCount(ta->sp, tl->sp ) == 1 && classRefCount(ta->sp, tr->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                                if (lderivedfrombase > rderivedfrombase)
                                    return -1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return 1;
                                else
                                    return 0;
                            return -1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return 1;
                        }
                    }
                }
            }
        }
        // compare qualifiers at top level
        rankl = isconst(ltype) + isvolatile(ltype) * 2;
        rankr = isconst(rtype) + isvolatile(rtype) * 2;
        if (rankl != rankr)
        {
            if (comparetypes(basetype(ltype), basetype(rtype), TRUE))
            {
                int n = rankl ^ rankr;
                if ((n & rankl) && !(n & rankr))
                    return 1;
                if ((n & rankr) && !(n & rankl))
                    return -1;
            }
        }
        if (atype && isref(rtype) && isref(ltype))
        {
            // rvalue matches an rvalue reference better than an lvalue reference
            
            if (isref(rtype) && isref(ltype) && basetype(ltype)->type != basetype(rtype)->type)
            {
                int lref = expa && lvalue(expa);
                int rref = expa && (!lvalue(expa) && (!isstructured(rtype) || !ismem(expa)) );
                if (expa && expa->type == en_func)
                {
                    TYPE *tp = basetype(expa->v.func->sp->tp)->btp;
                    if (tp)
                    {
                        if (tp->type == bt_rref)
                            rref = TRUE;
                        if (tp->type == bt_lref)
                            lref = TRUE;
                    }
                }
                lref |= expa && isstructured(atype) && expa->type != en_not_lvalue;
                if (basetype(ltype)->type == bt_rref)
                {
                    if (lref)
                        return 1;
                    else if (rref)
                        return -1;
                }
                else 
                    if (basetype(ltype)->type == bt_lref)
                    {
                        if (lref)
                            return -1;
                        else if (rref)
                            return 1;
                    }
            }
            // compare qualifiers at top level
            rankl = isconst(basetype(ltype)->btp) + isvolatile(basetype(ltype)->btp) * 2;
            rankr = isconst(basetype(rtype)->btp) + isvolatile(basetype(rtype)->btp) * 2;
            if (rankl != rankr)
            {
                if (comparetypes(basetype(basetype(ltype)->btp), basetype(basetype(rtype)->btp), TRUE))
                {
                    int n = rankl ^ rankr;
                    if ((n & rankl) && !(n & rankr))
                        return 1;
                    if ((n & rankr) && !(n & rankl))
                        return -1;
                }
            }
        }
    }
    else if (xl == user)
    {
        int l=0, r=0,llvr=0,rlvr=0;
        if (seql[l] == CV_DERIVEDFROMBASE && seqr[r] == CV_DERIVEDFROMBASE)
        {
            HASHREC *hr = basetype(funcl->tp)->syms->table[0];
            if (!funcl->castoperator)
                hr = hr->next;
            ltype = ((SYMBOL *)(hr->p))->tp;
            hr = basetype(funcr->tp)->syms->table[0];
            if (!funcr->castoperator)
                hr = hr->next;
            rtype = ((SYMBOL *)(hr->p))->tp;
            if (isref(ltype))
                ltype = basetype(ltype)->btp;
            if (isref(rtype))
                rtype = basetype(rtype)->btp;
            if (isref(atype))
                atype = basetype(atype)->btp;
            ltype = basetype(ltype);
            rtype = basetype(rtype);
            atype = basetype(atype);
            if (classRefCount(ltype->sp, atype->sp ) == 1 && classRefCount(ltype->sp, atype->sp) == 1)
            {
                if (classRefCount(ltype->sp, rtype->sp) == 1)
                {
                    return 1;
                }
                else if (classRefCount(rtype->sp, ltype->sp) == 1)
                {
                    return -1;
                }
            }
            if (!comparetypes(ltype, rtype, TRUE))
                return 0;
        }
        if (seql[l] == CV_USER && seqr[r] == CV_USER && funcl && funcr)
        {
            return 0;
		}
        l=0, r = 0;
        for (;l < lenl && seql[l] != CV_USER && r < lenr && seqr[r] != CV_USER;)
        {
            BOOLEAN cont = FALSE;
            switch (seql[l])
            {   
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    l++;
                    cont = TRUE;
                    break;
                case CV_LVALUETORVALUE:
                    llvr++;
                    break;
                default:
                    break;
            }
            switch (seqr[r])
            {   
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    r++;
                    cont = TRUE;
                    break;
                case CV_LVALUETORVALUE:
                    rlvr++;
                default:
                    break;
            }
            if (cont)
                continue;
            if (seql[l] != seqr[r])
                break;
            l++, r++;
        }
        if (llvr && !rlvr)
            return -1;
        if (!llvr && rlvr)
            return 1;
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (seql[l] == CV_USER && seqr[r] != CV_USER)
        {
            return -1;
        }
        else if (seql[l] != CV_USER && seqr[r] == CV_USER)
        {
            return 1;
        }
    }
    // ellipse always returns 0;
    return 0;
}
static BOOLEAN ellipsed(SYMBOL *sym)
{
    HASHREC *hr = basetype(sym->tp)->syms->table[0];
    while (hr->next)
        hr = hr->next;
    return basetype(((SYMBOL *)hr->p)->tp)->type == bt_ellipse;
}
static void SelectBestFunc(SYMBOL ** spList, enum e_cvsrn **icsList, 
                                int **lenList, FUNCTIONCALL *funcparams,
                                int argCount, int funcCount, SYMBOL ***funcList)
{
    static enum e_cvsrn identity = CV_IDENTITY;
    char arr[500];
    int i, j;
    int ellipseCount=0, unellipseCount=0;
    for (i=0; i < funcCount; i++)
    {
        for (j=i+1; j < funcCount && spList[i]; j++)
        {
            if (spList[i] && spList[j])
            {
                if (spList[i]->initializer_list != spList[j]->initializer_list)
                    if (spList[i]->initializer_list)
                    {
                        spList[i] = NULL;
                        break;
                    }
                    else
                    {
                        spList[j] = NULL;
                    }
            }
            if (spList[j])
            {
                BOOLEAN leftPacked = FALSE;
                BOOLEAN rightPacked = FALSE;
                HASHREC *hrleft = basetype(spList[i]->tp)->syms->table[0];
                HASHREC *hrright = basetype(spList[j]->tp)->syms->table[0];
                while (hrleft)
                {
                    if (((SYMBOL *)hrleft->p)->packed)
                        leftPacked = TRUE;
                    hrleft = hrleft->next;
                }
                while (hrright)
                {
                    if (((SYMBOL *)hrright->p)->packed)
                        rightPacked = TRUE;
                    hrright = hrright->next;
                }
                if (leftPacked && !rightPacked)
                {
                    spList[i] = 0;
                }
                else if (rightPacked && !leftPacked)
                {
                    spList[j] = 0;
                }
                if (spList[i] && spList[j])
                {
                    int left=0, right=0;
                    int l=0,r=0;
                    int k=0;
                    INITLIST *args = funcparams ? funcparams->arguments : NULL;
                    HASHREC *hrl = basetype(spList[i]->tp)->syms->table[0];
                    HASHREC *hrr = basetype(spList[j]->tp)->syms->table[0];
                    memset(arr, 0, sizeof(arr));
                    for (k=0; k < argCount; k++)
                    {
                        enum e_cvsrn *seql = &icsList[i][l];
                        enum e_cvsrn *seqr = &icsList[j][r];
                        int lenl = lenList[i][k];
                        int lenr = lenList[j][k];
                        if (!lenl)
                        {
                            seql = &identity;
                            lenl = 1;
                        }
                        if (!lenr)
                        {
                            seqr = &identity;
                            lenr = 1;
                        }
                        if (k == 0 && funcparams && funcparams->thisptr)
                        {
                            TYPE *tpl, *tpr;
                            if (spList[i]->castoperator)
                            {
                                tpl = toThis(basetype(spList[i]->tp)->btp);
                            }
                            else
                            {
                                tpl = ((SYMBOL *)(hrl->p))->tp;
                                hrl = hrl->next;
                            }
                            if (spList[j]->castoperator)
                            {
                                tpr = toThis(basetype(spList[j]->tp)->btp);
                            }
                            else
                            {
                                tpr = ((SYMBOL *)(hrr->p))->tp;
                                hrr = hrr->next;
                            }
                            arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, 
                                                        funcparams->thistp, funcparams->thisptr,
                                                        funcList ? funcList[i][k] : NULL, 
                                                        funcList ? funcList[j][k] : NULL,
                                                        lenl, lenr, FALSE);
                        }
                        /*
                        else if (k == 1 && funcparams && funcparams->thisptr)
                        {
                            TYPE *tpl, *tpr;
                            if (spList[i]->castoperator)
                            {
                                tpl = ((SYMBOL *)(hrl->p))->tp;
                                hrl = hrl->next;
                            }
                            else
                            {
                                tpl = toThis(basetype(spList[i]->tp)->btp);
                            }
                            if (spList[j]->castoperator)
                            {
                                tpr = ((SYMBOL *)(hrr->p))->tp;
                                hrr = hrr->next;
                            }
                            else
                            {
                                tpr = toThis(basetype(spList[j]->tp)->btp);
                            }
                            arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, 
                                                        args ? args->tp : 0 , args ? args->exp : 0,
                                                        funcList ? funcList[i][k] : NULL, 
                                                        funcList ? funcList[j][k] : NULL,
                                                        lenl, lenr, FALSE);
                        }
                        */
                        else
                        {
                            TYPE *tpl = hrl ? ((SYMBOL *)hrl->p)->tp : NULL;
                            TYPE *tpr = hrr ? ((SYMBOL *)hrr->p)->tp : NULL;
                            if (tpl && tpr)
                                arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, 
                                                        args? args->tp:0, args?args->exp:0,
                                                        funcList ? funcList[i][k] : NULL, 
                                                        funcList ? funcList[j][k] : NULL,
                                                        lenl, lenr, FALSE);
                            else
                                arr[k] = 0;
                            if (hrl) hrl = hrl->next;
                            if (hrr) hrr = hrr->next;
                            if (args) args = args->next;
                        }
                        l += lenList[i][k];
                        r += lenList[j][k];
                    }
                    for (k=0; k < argCount; k++)
                    {
                        if (arr[k] > 0)
                            right++;
                        else if (arr[k] < 0)
                            left++;
                    }
                    if (left && !right)
                    {
                        spList[j] = NULL;
                    }
                    else if (right && !left)
                    {
                        spList[i] = NULL;
                    }
                }
            }
        }
    }
    for (i=0,j=0; i < funcCount; i++)
    {
        if (spList[i])
            j++;
    }
    if (j > 1)
    {
        int ellipseCount=0, unellipseCount=0;
        for (i=0,j=0; i < funcCount; i++)
        {
            if (spList[i])
                if (ellipsed(spList[i]))
                    ellipseCount++;
                else
                    unellipseCount++;
        }
        if (unellipseCount && ellipseCount)
        {
            for (i=0,j=0; i < funcCount; i++)
            {
                if (spList[i] && ellipsed(spList[i]))
                    spList[i] = 0;
            }
        }
        for (i=0,j=0; i < funcCount; i++)
        {
            if (spList[i])
                j++;
        }
    }
}
static LIST *GetMemberCasts(LIST *gather, SYMBOL *sp)
{    
    if (sp)
    {
        BASECLASS *bcl = sp->baseClasses;
        SYMBOL *find = search(overloadNameTab[CI_CAST], basetype(sp->tp)->syms);
        if (find)
        {
            LIST *lst = Alloc(sizeof(LIST));
            lst->data = find;
            lst->next = gather;
            gather = lst;
        }
        while (bcl)
        {
            gather = GetMemberCasts(gather, bcl->cls);
            bcl = bcl->next;
        }
    }
    return gather;
}
static LIST *GetMemberConstructors(LIST *gather, SYMBOL *sp)
{
    BASECLASS *bcl = sp->baseClasses;
    SYMBOL *sym = sp;
    while (sym)
    {
        // conversion of one class to another
        SYMBOL *find = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sym->tp)->syms);
        if (find)
        {
            LIST *lst = Alloc(sizeof(LIST));
            lst->data = find;
            lst->next = gather;
            gather = lst;
        }
        if (bcl)
        {
            sym = bcl->cls;
            bcl = bcl->next;
        }
        else
        {
            sym = NULL;
        }
    }
    return gather;
}
void getSingleConversion(TYPE *tpp, TYPE *tpa, EXPRESSION *expa, int *n, enum e_cvsrn *seq, 
                                SYMBOL *candidate, SYMBOL **userFunc, BOOLEAN allowUser);
static SYMBOL *getUserConversion(int flags,
                          TYPE *tpp, TYPE *tpa, EXPRESSION *expa,
                              int *n, enum e_cvsrn *seq, SYMBOL *candidate_in, SYMBOL **userFunc, BOOLEAN honorExplicit)
{
    static BOOLEAN infunc = FALSE;
    if (!infunc)
    {
        LIST *gather = NULL;
        TYPE *tppp;
        if (tpp->type == bt_typedef)
            tpp = tpp->btp;
        tppp = tpp;
        if (isref(tppp))
            tppp = basetype(tppp)->btp;
        infunc = TRUE;
        if (flags & F_WITHCONS)
        {
            if (isstructured(tppp))
            {
                SYMBOL *sym = basetype(tppp)->sp;
                if (sym->templateLevel && !templateNestingCount && allTemplateArgsSpecified(sym, sym->templateParams))
                {
                    sym = TemplateClassInstantiate(sym, sym->templateParams, FALSE, sc_global);
                }
                gather = GetMemberConstructors(gather, sym);
                tppp = sym->tp;
            }
        }
        gather = GetMemberCasts(gather, basetype(tpa)->sp);
        if (gather)
        {
            LIST *lst2;
            int funcs = 0;
            int i;
            SYMBOL **spList , ** spFilterList;
            enum e_cvsrn **icsList;
            int **lenList;
            int m = 0;
            SYMBOL *found1, *found2;
            FUNCTIONCALL funcparams;
            INITLIST args;
            TYPE thistp;
            EXPRESSION exp;
            lst2 = gather;
            memset(&funcparams, 0, sizeof(funcparams));
            memset(&args, 0, sizeof(args));
            memset(&thistp, 0, sizeof(thistp));
            memset(&exp, 0, sizeof(exp));
            funcparams.arguments = &args;
            args.tp = tpa;
            args.exp = &exp;
            exp.type = en_c_i;
            funcparams.ascall = TRUE;
            funcparams.thisptr = expa;
            funcparams.thistp = &thistp;
            thistp.btp = tpa;
            thistp.rootType = &thistp;
            thistp.type = bt_pointer;
            thistp.size = getSize(bt_pointer);
            while (lst2)
            {
                HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                while (*hr)
                {
                    funcs++;
                    hr = &(*hr)->next;
                }
                lst2 = lst2->next;
            }
            spList = (SYMBOL **)Alloc(sizeof(SYMBOL *) * funcs);
            spFilterList = (SYMBOL **)Alloc(sizeof(SYMBOL *) * funcs);
            icsList = (enum e_cvsrn **)Alloc(sizeof(enum e_cvsrn *) * funcs);
            lenList = (int **)Alloc(sizeof(int *) * funcs);
            lst2 = gather;
            i = 0 ;
            while (lst2)
            {
                HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                while (*hr)
                {
                    SYMBOL *sym = (SYMBOL *)(*hr)->p;
                    int n;
                    for (n=0; n < i; n++)
                        if (spFilterList[n] == sym || spFilterList[n]->mainsym == sym || spFilterList[n] == sym->mainsym ||
                            matchOverload(sym->tp, spFilterList[n]->tp, FALSE) && sym->overlayIndex == spFilterList[n]->overlayIndex)
                            break;
                    if (!sym->instantiated && n >= i)
                    {
                        spFilterList[i] = sym;
                        if (sym->templateLevel && sym->templateParams)
                        {
                            if (sym->castoperator)
                            {
                                spList[i++] = detemplate(sym, NULL, tppp) ;
                            }
                            else
                            {
                                spList[i++] = detemplate(sym, &funcparams, NULL) ;
                            }
                        }
                        else
                        {
                            spList[i++] = sym;
                        }
                        
                    }
                    hr = &(*hr)->next;
                }
                lst2 = lst2->next;
            }
            memset(&exp, 0, sizeof(exp));
            exp.type = en_not_lvalue;
            for (i=0; i < funcs; i++)
            {
                SYMBOL *candidate = spList[i];
                if (candidate)
                {
                    if (honorExplicit && candidate->isExplicit && !(flags & F_CONVERSION))
                    {
                        spList[i] = NULL;
                    }
                    else
                    {
                        int j;
                        int n3 = 0, n2 = 0, m1;
                        enum e_cvsrn seq3[50];
                        if (candidate->castoperator)
                        {
                            TYPE *tpc = basetype(candidate->tp)->btp;
                            if (isref(tpc))
                                tpc = basetype(tpc)->btp;
                            if (tpc->type != bt_auto && (((flags & F_INTEGER) && !isint(tpc))
                                || ((flags & F_POINTER) && !ispointer(tpc) && basetype(tpc)->type != bt_memberptr)
                                || ((flags & F_ARITHMETIC) && !isarithmetic(tpc))
                                || ((flags & F_STRUCTURE) && !isstructured(tpc))))
                            {
                                seq3[n2++] = CV_NONE;
                                seq3[n2 + n3++] = CV_NONE;
                            }
                            else
                            {
                                HASHREC *args = basetype(candidate->tp)->syms->table[0];
                                BOOLEAN lref = FALSE;
                                TYPE *tpn = basetype(candidate->tp)->btp;
                                if (isref(tpn))
                                {
                                    if (basetype(tpn)->type == bt_lref)
                                        lref = TRUE;
                                }
                                thistp.btp = tpa;
                                thistp.rootType = &thistp;
                                thistp.type = bt_pointer;
                                thistp.size = getSize(bt_pointer);
                                getSingleConversion(((SYMBOL *)args->p)->tp, &thistp, &exp, &n2, seq3, candidate, NULL, TRUE);
                                seq3[n2 + n3++] = CV_USER;
                                if (tpc->type == bt_auto)
                                {
                                    seq3[n2 + n3++] = CV_USER;
                                }
                                else if (isfuncptr(tppp))
                                {
                                    int n77 = n3;
                                    getSingleConversion(basetype(candidate->tp)->btp, tppp, lref ? NULL : &exp, &n3, seq3 + n2, candidate, NULL, TRUE);
                                    if (n77 != n3 - 1 || seq3[n2 + n77] != CV_IDENTITY)
                                    {
                                        SYMBOL *spf = basetype(basetype(tppp)->btp)->sp;
                                        n3 = n77;
                                        if (spf->templateLevel && spf->storage_class == sc_typedef && !spf->instantiated)
                                        {
                                            TEMPLATEPARAMLIST *args = spf->templateParams->next;
                                            spf = spf->mainsym;
                                            if (spf)
                                            {
                                                TEMPLATEPARAMLIST *hold[100];
                                                int count = 0;
                                                TEMPLATEPARAMLIST *srch = args;
                                                while (srch)
                                                {
                                                    hold[count++] = srch->p->byClass.dflt;
                                                    srch->p->byClass.dflt = srch->p->byClass.val;
                                                    srch = srch->next;
                                                }
                                                spf = GetTypedefSpecialization(spf, args);
                                                spf->tp = SynthesizeType(spf->tp, NULL, FALSE);
                                                getSingleConversion(basetype(candidate->tp)->btp, spf->tp, lref ? NULL : &exp, &n3, seq3 + n2, candidate, NULL, TRUE);
                                                srch = args;
                                                count = 0;
                                                while (srch)
                                                {
                                                    srch->p->byClass.val = srch->p->byClass.dflt;
                                                    srch->p->byClass.dflt = hold[count++];
                                                    srch = srch->next;
                                                }

                                            }
                                            else
                                            {
                                                seq3[n2 + n3++] = CV_NONE;
                                            }
                                        }
                                        else
                                        {
                                            getSingleConversion(basetype(candidate->tp)->btp, tppp, lref ? NULL : &exp, &n3, seq3 + n2, candidate, NULL, TRUE);
                                        }
                                    }
                                }
                                else
                                {
                                    getSingleConversion(basetype(candidate->tp)->btp, tppp, lref ? NULL : &exp, &n3, seq3 + n2, candidate, NULL, TRUE);
                                }
                            }
                        }
                        else
                        {
                            HASHREC *args = basetype(candidate->tp)->syms->table[0];
                            if (args)
                            {
                                if (candidate_in->isConstructor && candidate_in->parentClass == candidate->parentClass)
                                {
                                    seq3[n2++] = CV_NONE;
                                }
                                else
                                {
                                    SYMBOL *first, *next = NULL;
                                    SYMBOL *th = (SYMBOL *)args->p;
                                    args = args->next;
                                    first = (SYMBOL *)args->p;
                                    if (args->next)
                                        next = (SYMBOL *)args->next->p;
                                    if (!next || next->init)
                                    {
                                        if (first->tp->type != bt_ellipse)
                                        {
                                            getSingleConversion(first->tp, tpa, expa, &n2, seq3, candidate, NULL, TRUE);
                                            if (n2 && seq3[n2 - 1] == CV_IDENTITY)
                                            {
                                                n2--;
                                            }
                                        }
                                        seq3[n2 + n3++] = CV_USER;
                                        getSingleConversion(tppp, basetype(basetype(th->tp)->btp)->sp->tp, &exp, &n3, seq3 + n2, candidate, NULL, TRUE);
                                    }
                                    else
                                    {
                                        seq3[n2++] = CV_NONE;
                                    }
                                }
                            }
                        }
                        for (j = 0; j < n2 + n3; j++)
                            if (seq3[j] == CV_NONE)
                                break;
                        m1 = n2 + n3;
                        while (m1 && seq3[m1 - 1] == CV_IDENTITY)
                            m1--;
                        if (j >= n2 + n3 && m1 <= 4)
                        {
                            lenList[i] = Alloc(sizeof(int) * 2);
                            icsList[i] = Alloc(sizeof(enum e_cvsrn) * (n2 + n3));
                            lenList[i][0] = n2;
                            lenList[i][1] = n3;
                            memcpy(&icsList[i][0], seq3, (n2 + n3) * sizeof(enum e_cvsrn));
                        }
                        else
                        {
                            spList[i] = NULL;
                        }
                    }
                }
            }
            SelectBestFunc(spList, icsList, lenList, &funcparams, 2, funcs, NULL);
            WeedTemplates(spList, funcs, &funcparams, NULL);
            found1 = found2 = NULL;
            
            for (i=0; i < funcs && !found1; i++)
            {
                int j;
                found1 = spList[i];
                m = i;
                for (j=i+1; j < funcs && found1 && !found2; j++)
                {
                    if (spList[j])
                    {
                        found2 = spList[j];
                    }
                }
            }
            if (found1)
            {
                if (!found2)
//                {
//                     errorsym2(ERR_AMBIGUITY_BETWEEN, found1, found2);
//                }
 //               else
                {
                    if (honorExplicit && found1->isExplicit)
                    {
                        error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
                    }
                    if (seq)
                    {
//                        seq[*n++]= CV_USER;
                        int l = lenList[m][0] +1;// + lenList[m][1];
                        memcpy(&seq[*n], &icsList[m][0], l * sizeof(enum e_cvsrn));
                        *n += l;
                        if (userFunc)
                            *userFunc = found1;
                    }
                    infunc = FALSE;
                    if (flags & F_CONVERSION)
                    {
                        GENREF(found1);
                        if (found1->templateLevel && !templateNestingCount && found1->templateParams)
                        {
                            found1 = TemplateFunctionInstantiate(found1, FALSE, FALSE);
                        }
                        else
                        {
                            if (found1->deferredCompile && !found1->inlineFunc.stmt)
                            {
                                deferredCompileOne(found1);
                            }
                        }
                    }
                    return found1;
                }
            }
        }
        infunc = FALSE;
    }
    if (seq)
        seq[(*n)++] = CV_NONE;
    return NULL;
}
static void getQualConversion(TYPE *tpp, TYPE *tpa, int *n, enum e_cvsrn *seq)
{
    BOOLEAN hasconst = TRUE, hasvol = TRUE;
    BOOLEAN sameconst = TRUE, samevol = TRUE;
    BOOLEAN first = TRUE;
    while (tpa && tpp)// && ispointer(tpa) && ispointer(tpp))
    {
        if (isconst(tpp) != isconst(tpa))
        {
            sameconst = FALSE;
            if (isconst(tpa) && !isconst(tpp))
                break;
            if (!hasconst)
                break;
        }
        if (isvolatile(tpp) != isvolatile(tpa))
        {
            samevol = FALSE;
            if (isvolatile(tpa) && !isvolatile(tpp))
                break;
            if (!hasvol)
                break;
        }
            
        if (!first)
        {
            if (!isconst(tpp))
                hasconst = FALSE;
            if (!isvolatile(tpp))
                hasvol = FALSE;
        }
        first = FALSE;
        tpa = basetype(tpa)->btp;
        tpp = basetype(tpp)->btp;
    }
    if ((!tpa && !tpp) || (tpa && tpp && tpa->type != bt_pointer && tpp->type != bt_pointer))
    {
        if (tpa && tpp && ((hasconst && isconst(tpa) && !isconst(tpp)) 
                           || (hasvol && isvolatile(tpa) && !isvolatile(tpp))))
            seq[(*n)++] = CV_NONE;
        else if (!sameconst || !samevol)
            seq[(*n)++] = CV_QUALS;
        else
            seq[(*n)++] = CV_IDENTITY;
    }
    else
    {
            seq[(*n)++] = CV_NONE;
    }
}
static void getPointerConversion(TYPE *tpp, TYPE *tpa, EXPRESSION *exp, int *n, 
                                 enum e_cvsrn *seq)
{
    if (basetype(tpa)->btp->type == bt_void && exp && (isconstzero(&stdint, exp) || exp->type == en_nullptr))
    {
        seq[(*n)++] = CV_POINTERCONVERSION;
        return;
    }
    else 
    {
        if (basetype(tpa)->array)
            seq[(*n)++] = CV_ARRAYTOPOINTER;
        if (isfunction(basetype(tpa)->btp))
            seq[(*n)++] = CV_FUNCTIONTOPOINTER;
        if (basetype(basetype(tpp)->btp)->type == bt_void)
        {
            if (basetype(basetype(tpa)->btp)->type != bt_void)
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            if (ispointer(basetype(tpa)->btp))
            {
                if ((isconst(tpa) && !isconst(tpp)) || (isvolatile(tpa) && !isvolatile(tpp)))
                    seq[(*n)++] = CV_NONE;
                else if ((isconst(tpp) != isconst(tpa)) || (isvolatile(tpa) != isvolatile(tpp)))
                    seq[(*n)++] = CV_QUALS;                    
                return;
            }
        }
        else if (isstructured(basetype(tpp)->btp) && isstructured(basetype(tpa)->btp))
        {
            SYMBOL *base = basetype(basetype(tpp)->btp)->sp;
            SYMBOL *derived = basetype(basetype(tpa)->btp)->sp;
            
            if (base != derived && !sameTemplate(base->tp, derived->tp))
            {
                int v = classRefCount(base, derived);
                if (v != 1)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    seq[(*n)++]= CV_DERIVEDFROMBASE;
                }
            }
            else
            {
                seq[(*n)++] = CV_IDENTITY;
            }
        }
        else if (!comparetypes(tpp, tpa, TRUE) && !basetype(tpa)->nullptrType)
        {
            seq[(*n)++] = CV_NONE;
        }
        getQualConversion(tpp, tpa, n, seq);
    }
}
BOOLEAN sameTemplatePointedTo(TYPE *tnew, TYPE *told)
{
    if (isconst(tnew) != isconst(told) || isvolatile(tnew) != isvolatile(told))
        return FALSE;
    while (basetype(tnew)->type == basetype(told)->type && basetype(tnew)->type == bt_pointer)
    {
        tnew = basetype(tnew)->btp;
        told = basetype(told)->btp;
        if (isconst(tnew) != isconst(told) || isvolatile(tnew) != isvolatile(told))
            return FALSE;
    }
    return sameTemplate(tnew, told);
}
BOOLEAN sameTemplate(TYPE *P, TYPE *A)
{
    BOOLEAN PLd, PAd;
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
    PL= P->sp->templateParams;
    PA = A->sp->templateParams;
    if (!PL || !PA || !P->sp->specialized && A->sp->specialized) // errors
    {
        if (P->size == 0 &&!strcmp(P->sp->decoratedName, A->sp->decoratedName))
            return TRUE;
        return FALSE;
        
    }
    PLd = PAd = FALSE;
    if (PL->p->bySpecialization.types)
    {
        PL = PL->p->bySpecialization.types;
        PLd = TRUE;
    }    
    else
    {
        PL = PL->next;
    }
    if (PA->p->bySpecialization.types)
    {
        PA = PA->p->bySpecialization.types;
        PAd = TRUE;
    }    
    else
    {
        PA = PA->next;
    }
    if (PL && PA)
    {
        while (PL && PA)
        {
            if (PL->p->packed != PA->p->packed)
                break;
                
            if (PL->p->packed)
            {
                PL = PL->p->byPack.pack;
                PA = PA->p->byPack.pack;
            }
            if (!PL || !PA)
                break;
            if (PL->p->type != PA->p->type)
            {
                break;
            }
            else if (P->sp->instantiated || A->sp->instantiated || PL->p->byClass.dflt && PA->p->byClass.dflt)
            {
                if (PL->p->type == kw_typename)
                {
                    TYPE *pl = PL->p->byClass.val /*&& !PL->p->byClass.dflt*/ ? PL->p->byClass.val : PL->p->byClass.dflt;
                    TYPE *pa = PA->p->byClass.val /*&& !PL->p->byClass.dflt*/ ? PA->p->byClass.val : PA->p->byClass.dflt;
                    if (!pl || !pa)
                        break;
                    if (!templatecomparetypes(pa, pl, TRUE))
                    {
                        if (basetype(pa)->type == bt_enum && isint(pl) || basetype(pl)->type == bt_enum && isint(pa))
                        {
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else if (PL->p->type == kw_template)
                {
                    SYMBOL *plt = PL->p->byTemplate.val && !PL->p->byTemplate.dflt ? PL->p->byTemplate.val : PL->p->byTemplate.dflt;
                    SYMBOL *pat = PA->p->byTemplate.val && !PL->p->byTemplate.dflt ? PA->p->byTemplate.val : PA->p->byTemplate.dflt;
                    if ((plt || pat) && !exactMatchOnTemplateParams(PL->p->byTemplate.args, PA->p->byTemplate.args))
                        break;
                }
                else if (PL->p->type == kw_int)
                {
                    EXPRESSION *plt = PL->p->byNonType.val && !PL->p->byNonType.dflt ? PL->p->byNonType.val : PL->p->byNonType.dflt;
                    EXPRESSION *pat = PA->p->byNonType.val && !PA->p->byNonType.dflt ? PA->p->byNonType.val : PA->p->byNonType.dflt;
                    if (!templatecomparetypes(PL->p->byNonType.tp, PA->p->byNonType.tp, TRUE))
                        break;
//#ifndef PARSER_ONLY
                    if ((!plt || !pat) || !equalTemplateIntNode(plt, pat))
                        break;
//#endif
                }
            }
            PL = PL->next;
            PA = PA->next;
        }
        return !PL && !PA;
    }
    return FALSE;
}
void getSingleConversion(TYPE *tpp, TYPE *tpa, EXPRESSION *expa, int *n, 
                                enum e_cvsrn *seq, SYMBOL *candidate, SYMBOL **userFunc, BOOLEAN allowUser)
{
    BOOLEAN lref=FALSE;
    BOOLEAN rref=FALSE;
    EXPRESSION *exp = expa;
    TYPE *tpax = tpa;
    TYPE *tppx = tpp;
    tpa = basetype(tpa);
    tpp = basetype(tpp);
    // when evaluating decltype we sometimes come up with these
    if (tpa->type == bt_templateparam)
        tpa = tpa->templateParam->p->byClass.val;
    if (!tpa)
    {
         seq[(*n)++] = CV_NONE;
         return;
    }
    while (expa && expa->type == en_void)
        expa = expa->right;
    if (tpp->type != tpa->type && (tpp->type == bt_void || tpa->type == bt_void))
    {
         seq[(*n)++] = CV_NONE;
         return;
    }
    lref = (basetype(tpa)->type == bt_lref || isstructured(tpa) && (!expa || expa->type != en_not_lvalue) || expa && (lvalue(expa) || isarithmeticconst(expa))) && (!expa || expa->type != en_func && expa->type != en_thisref) && !tpa->rref || tpa->lref ;
    rref = (basetype(tpa)->type == bt_rref || (isstructured(tpa) && expa && expa->type == en_not_lvalue) || expa && !lvalue(expa) && !ismem(expa) ) && !tpa->lref || tpa->rref;
    if (exp && exp->type == en_thisref)
        exp = exp->left;
    if (exp && exp->type == en_func)
    {
        if (basetype(exp->v.func->sp->tp)->type != bt_aggregate)
        {
            TYPE *tp = basetype(basetype(exp->v.func->functp)->btp);
            if (tp)
            {
                if (tp->type == bt_rref)
                {
                    if (!tpa->lref)
                    {
                        rref = TRUE;
                        lref = FALSE;
                    }
                }
                else if (tp->type == bt_lref)
                {
                    if (!tpa->rref)
                    {
                        lref = TRUE;
                        rref = FALSE;
                    }
                }
            }
        }
    }
    if (isref(tpa))
    {
        if (basetype(tpa)->type == bt_rref)
        {
            rref = TRUE;
            lref = FALSE;
        }
        else if (basetype(tpa)->type == bt_lref)
        {
            lref = TRUE;
            rref = FALSE;
        }
        tpa = basetype(tpa)->btp;
        while (isref(tpa))
            tpa = basetype(tpa)->btp;
    }
    if (isref(tpp))
    {
        TYPE *tppp = basetype(tpp)->btp;
        while (isref(tppp))
            tppp = basetype(tppp)->btp;
        if (!rref && expa && isstructured(tppp) && expa->type != en_not_lvalue)
        {
            EXPRESSION *expx = expa;
            if (expx->type == en_thisref)
                expx = expx->left;
            if (expx->type == en_func)
            {
                if (expx->v.func->returnSP)
                {
                    if (!expx->v.func->returnSP->anonymous)
                        lref = TRUE;
                }
            }
            else
            {
                lref = TRUE;
            }
        }
        if ((isconst(tpax) != isconst(tppp))
            || (isvolatile(tpax) != isvolatile(tppp)))
            seq[(*n)++] = CV_QUALS;
        if (lref && !rref && tpp->type == bt_rref)
            seq[(*n)++] = CV_LVALUETORVALUE;
        if (tpp->type == bt_rref && lref && !isfunction(tpa) && !isfuncptr(tpa) && !isarithmeticconst(expa))
        {
            // lvalue to rvalue ref not allowed unless the lvalue is nonvolatile and const
            if (!isDerivedFromTemplate(tppx))
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpp->type == bt_lref && rref &&!lref)
        {
            // rvalue to lvalue ref not allowed unless the lvalue is a function
            seq[(*n)++] = CV_LVALUETORVALUE;//CV_NONE;
            if (isconst(tppp) && !isvolatile(tppp))
                seq[(*n)++] = CV_QUALS;
        }
        tpa = basetype(tpa);
        if (isstructured(tpa))
        {
            if (isstructured(tppp))
            {
                if (basetype(tpa)->sp != basetype(tppp)->sp && !sameTemplate(tppp, tpa))
                {
                    if (classRefCount(basetype(tppp)->sp, basetype(tpa)->sp) == 1)
                    {
                        seq[(*n)++] = CV_DERIVEDFROMBASE;
                    }
                    else
                    {
                        if (allowUser)
                            getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
                        else
                            seq[(*n)++] = CV_NONE;
                    }
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                if (allowUser)
                    getUserConversion(0, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
                else
                    seq[(*n)++] = CV_NONE;
            }
        }
        else if (isstructured(tppp))
        {
            if (allowUser)
                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (isfuncptr(tppp))
        {
            tpp = basetype(tppp)->btp;
            if (isfuncptr(tpa))
                tpa = basetype(tpa)->btp;
            if (comparetypes(tpp, tpa, TRUE))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else
        {
            if (allowUser)
                getSingleConversion(tppp, tpa, expa, n, seq, candidate, userFunc, allowUser);
            else
                seq[(*n)++] = CV_NONE;
        }
    }
    else
    {
        if ((isconst(tpax) != isconst(tppx))
            || (isvolatile(tpax) != isvolatile(tppx)))
            seq[(*n)++] = CV_QUALS;
        if (basetype(tpp)->type == bt___string)
        {
            if (basetype(tpa)->type == bt___string || expa && expa->type == en_labcon && expa->string)
                seq[(*n)++] = CV_IDENTITY;
            else
                seq[(*n)++] = CV_POINTERCONVERSION;
        }
        else if (basetype(tpp)->type == bt___object)
        {
            if (basetype(tpa)->type == bt___object)
                seq[(*n)++] = CV_IDENTITY;
            else
                seq[(*n)++] = CV_POINTERCONVERSION;
        }
        else if (isstructured(tpa))
        {
            if (isstructured(tpp))
            {
                if (basetype(tpa)->sp == basetype(tpp)->sp || sameTemplate(tpp, tpa))
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
                else if (classRefCount(basetype(tpp)->sp, basetype(tpa)->sp) == 1)
                {
                    seq[(*n)++] = CV_DERIVEDFROMBASE;
                }
                else
                {
                    if (allowUser)
                        getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
                    else
                        seq[(*n)++] = CV_NONE;
                }
            }
            else
            {
                if (allowUser)
                    getUserConversion(0, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
                else
                    seq[(*n)++] = CV_NONE;
            }
        }
        else if (isstructured(tpp))
        {
            if (allowUser)
                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (isfuncptr(tpp))
        {
            TYPE *rv;
            tpp = basetype(tpp)->btp;
            rv = basetype(tpp)->btp;
            if (isfuncptr(tpa))
            {
                tpa = basetype(tpa)->btp;
                if (rv->type == bt_auto)
                    basetype(tpp)->btp = basetype(tpa)->btp;
            }
            if (comparetypes(tpp, tpa, TRUE))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }                            
            basetype(tpp)->btp = rv;
        }
        else if (basetype(tpp)->nullptrType)
        {
            if (basetype(tpa)->nullptrType || ispointer(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr) )
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (ispointer(tpp))
        {
            if (ispointer(tpa))
            {
                // cvqual
                getPointerConversion(tpp, tpa, expa, n, seq);
            }
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else if (isvoidptr(tpp) && (isfunction(tpa) || tpa->type == bt_aggregate))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (basetype(tpp)->type == bt_memberptr)
        {
            if (basetype(tpa)->type == bt_memberptr)
            {
                if (comparetypes(basetype(tpp)->btp, basetype(tpa)->btp, TRUE))
                {
                    if (basetype(tpa)->sp != basetype(tpp)->sp)
                    {
                        if (classRefCount(basetype(tpa)->sp, basetype(tpp)->sp) == 1)
                        {
                            seq[(*n)++] = CV_POINTERTOMEMBERCONVERSION;
                        }
                        else
                        {
                            if (allowUser)
                                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
                            else
                                seq[(*n)++] = CV_NONE;
                        }
                    }
                    else
                    {
                        seq[(*n)++] = CV_IDENTITY;
                    }
                }
                else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
                {
                    seq[(*n)++] = CV_POINTERCONVERSION;
                }
                else
                {
                    seq[(*n)++] = CV_NONE;            
                }
            }
            else if (expa && ((isconstzero(tpa, expa) || expa->type == en_nullptr)))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else if (isfunction(tpa))
            {
                if (!comparetypes(basetype(tpp)->btp, tpa, TRUE))
                    seq[(*n)++] = CV_NONE;
                    
                else if (basetype(tpa)->sp->parentClass != basetype(tpp)->sp && basetype(tpa)->sp->parentClass->mainsym != tpp->sp && basetype(tpa)->sp->parentClass != basetype(tpp)->sp->mainsym)
                {
                    if (classRefCount(basetype(tpa)->sp->parentClass, basetype(tpp)->sp) == 1)
                    {
                        seq[(*n)++] = CV_POINTERTOMEMBERCONVERSION;
                    }
                    else
                    {
                        if (allowUser)
                            getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, TRUE);
                        else
                            seq[(*n)++] = CV_NONE;
                    }
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                seq[(*n)++] = CV_NONE;            
            }
        }
        else if (isfunction(tpa))
        {
            if (isfunction(tpp) && comparetypes(tpp, tpa, TRUE))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (basetype(tpp)->type == bt_bool)
            {
                seq[(*n)++] = CV_BOOLCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }

        }
        else if (ispointer(tpa))
        {
            if (basetype(tpp)->type == bt_bool)
            {
                seq[(*n)++] = CV_BOOLCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (basetype(tpa)->type == bt_memberptr)
        {
             seq[(*n)++] = CV_NONE;            
        }
        else if (basetype(tpa)->type == bt_enum)
        {
            if (basetype(tpp)->type == bt_enum)
            {
                if (basetype(tpa)->sp != basetype(tpp)->sp)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else 
                {
                    if ((isconst(tpax) != isconst(tppx))
                    || (isvolatile(tpax) != isvolatile(tppx)))
                        seq[(*n)++] = CV_QUALS;
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            { 
                if (isint(tpp) && !basetype(tpa)->scoped)
                {
                    seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                }
                else
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
        else if (basetype(tpp)->type == bt_enum)
        {
            if (isint(tpa))
            {
                if (tpa->enumConst)
                {
                    if (tpa->sp == basetype(tpp)->sp)
                        seq[(*n)++] = CV_IDENTITY;
                    else
                        seq[(*n)++] = CV_NONE;
                }
                else
                {
                    seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                }
            }
            else
            {
                seq[(*n)++] = CV_NONE;                
            }
        }
        else
        {
            if ((isconst(tpax) != isconst(tppx))
                || (isvolatile(tpax) != isvolatile(tppx)))
                seq[(*n)++] = CV_QUALS;
            if (tpa->enumConst)
            {
                seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
            }
            else if (basetype(tpp)->type != basetype(tpa)->type)
            {
                if (isint(tpa))
                    if (basetype(tpp)->type == bt_bool)
                        seq[(*n)++] = CV_BOOLCONVERSION;
                    else if ((basetype(tpp)->type == bt_int || basetype(tpp)->type == bt_unsigned) && basetype(tpa)->type < basetype(tpp)->type)
                        seq[(*n)++]= CV_INTEGRALPROMOTION;
                    else if (isint(tpp))
                        seq[(*n)++]= CV_INTEGRALCONVERSION;
                    else
                        seq[(*n)++]= CV_FLOATINGINTEGRALCONVERSION;
                        
                else /* floating */
                    if (basetype(tpp)->type == bt_bool)
                        seq[(*n)++] = CV_BOOLCONVERSION;
                    else if (basetype(tpp)->type == bt_double && basetype(tpa)->type == bt_float)
                        seq[(*n)++]= CV_FLOATINGPROMOTION;
                    else if (isfloat(tpp))
                        seq[(*n)++]= CV_FLOATINGCONVERSION;
                    else
                        seq[(*n)++]= CV_FLOATINGINTEGRALCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_IDENTITY;
            }
        }
    }
}
static void getInitListConversion(TYPE *tp, INITLIST *list, TYPE *tpp, int *n, enum e_cvsrn *seq, SYMBOL *candidate, SYMBOL **userFunc)
{
    INITLIST *a = list;
    if (isstructured(tp) || (isref(tp) && isstructured(basetype(tp)->btp)))
    {
        if (isref(tp))
            tp = basetype(basetype(tp)->btp);
        if (tp->sp->trivialCons)
        {
            HASHREC *structSyms = tp->syms->table[0];
            while (a && structSyms)
            {
                SYMBOL *member = (SYMBOL *)structSyms->p;
                if (ismemberdata(member))
                {
                    getSingleConversion(member->tp, a->tp, a->exp, n, seq, candidate, userFunc, TRUE); 
                    if (*n > 10)
                        break;
                    a = a->next;
                }
                structSyms = structSyms->next;
            }
            if (a)
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else
        {
            SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
            if (!cons)
            {
                // should never happen
                seq[(*n)++] = CV_NONE;
            }            
            else
            {
                EXPRESSION exp,*expp = &exp;
                TYPE *ctype = cons->tp;
                TYPE thistp;
                FUNCTIONCALL funcparams;
                memset(&exp, 0, sizeof(exp));
                memset(&funcparams, 0, sizeof(funcparams));
                memset(&thistp, 0, sizeof(thistp));
                funcparams.arguments = a;
                exp.type = en_c_i;
                thistp.type = bt_pointer;
                thistp.btp = tp;
                thistp.rootType = &thistp;
                thistp.size = getSize(bt_pointer);
                funcparams.thistp = &thistp;
                funcparams.thisptr = &exp;
                funcparams.ascall = TRUE;
                cons = GetOverloadedFunction(&ctype, &expp, cons, & funcparams, NULL, FALSE, TRUE, TRUE, _F_SIZEOF );
                if (!cons)    
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }     
    }
    else if (ispointer(tp))
    {
        TYPE *btp = tp;
        int x;
        while (isarray(btp))
            btp = basetype(btp)->btp;
        x = tp->size / btp->size;
        while (a)
        {
            getSingleConversion(btp, a->tp, a->exp, n, seq, candidate, userFunc, TRUE);
            if (*n > 10)
                break;
            if (--x < 0) // too many items...
            {
                seq[(*n)++] = CV_NONE;
                break;
            }
            a = a->next;
        }
    }
    else
    {
        if (a && a->next)
            seq[(*n)++] = CV_NONE;
        else if (a)
            getSingleConversion(tp, a ? a->tp : tpp, a ? a->exp : NULL, n, seq, candidate, userFunc, TRUE);
    }
}
static BOOLEAN getFuncConversions(SYMBOL *sp, FUNCTIONCALL *f, TYPE *atp, 
                                  SYMBOL *parent, enum e_cvsrn arr[], int *sizes, 
                                  int count, SYMBOL **userFunc, BOOLEAN usesInitList)
{
    int pos = 0;
    int n = 0;
    int i;
    INITLIST *a = NULL;
    HASHREC **hr;
    HASHREC **hrt = NULL;
    enum e_cvsrn seq[100], cur;
    TYPE *initializerListType = NULL;
    int m = 0,m1;
    TEMPLATEPARAMLIST *tr = NULL;
    if (sp->tp->type == bt_any)
        return FALSE;
        
    hr = basetype(sp->tp)->syms->table;
    if (f)
        a = f->arguments;
    else
        hrt = atp->syms->table;
    for (i=0; i < count; i++)
    {
        arr[i] = CV_PAD;
    }
    /* takes care of THIS pointer */
    if (sp->castoperator)
    {
        TYPE tpx;
        TYPE *tpp;
        SYMBOL *argsym = (SYMBOL *)(*hr)->p;
        memset(&tpx, 0, sizeof(tpx));
        m = 0;
        getSingleConversion(parent->tp, basetype(sp->tp)->btp, NULL,
                                    &m, seq, sp, userFunc ? &userFunc[n] : NULL, FALSE);
        m1 = m;
        while (m1 && seq[m1-1] == CV_IDENTITY)
            m1--;
        if (m1 > 10)
        {
            return FALSE;
        }
        for (i=0; i < m; i++)
            if (seq[i] == CV_NONE)
                return FALSE;
        memcpy(arr+pos, seq, m * sizeof(enum e_cvsrn));
        sizes[n++] = m;
        pos += m;
        hr = &(*hr)->next;
        tpp = argsym->tp;
        tpx.type = bt_pointer;
        tpx.size = getSize(bt_pointer);
        tpx.btp = f->arguments->tp;
        tpx.rootType = &tpx;
        m = 0;
        getSingleConversion(tpp, &tpx, f->thisptr, &m, seq, sp, userFunc ? &userFunc[n] : NULL, TRUE);
        m1 = m;
        while (m1 && seq[m1-1] == CV_IDENTITY)
            m1--;
        if (m1 > 10)
        {
            return FALSE;
        }
        for (i=0; i < m; i++)
            if (seq[i] == CV_NONE)
                return FALSE;
        memcpy(arr+pos, seq, m * sizeof(enum e_cvsrn));
        sizes[n++] = m;
        pos += m;
        return TRUE;
    }
    else
    {
        if (f)
        {
            if ((f->thistp || (a && a->tp)) && ismember(sp))
            {
                // nonstatic function
                TYPE *argtp = sp->tp;
                if (!argtp)
                {
                    arr[n++] = CV_NONE;
                    return FALSE;
                }
                else
                {
                    TYPE tpx;
                    TYPE *tpp;
                    TYPE *tpthis = f->thistp;
                    SYMBOL *argsym = (SYMBOL *)(*hr)->p;
                    memset(&tpx, 0, sizeof(tpx));
                    hr = &(*hr)->next;
                    tpp = argsym->tp;
                    if (!tpthis)
                    {
                        tpthis = a ? a->tp : NULL;
                        if (a)
                            a = a->next;
                    }
                    if (sp->castoperator || (tpthis && f->thistp == NULL))
                    {
                        tpthis = &tpx;
                        tpx.type = bt_pointer;
                        tpx.size = getSize(bt_pointer);
                        tpx.btp = f->arguments->tp;
                        tpx.rootType = &tpx;
                    }
                    else if (theCurrentFunc 
                             && (f->thisptr && f->thisptr->type == en_l_p && f->thisptr->left->type == en_auto && f->thisptr->left->v.sp->thisPtr)
                             && (theCurrentFunc->parentClass == sp->parentClass || sameTemplate(theCurrentFunc->parentClass->tp, sp->parentClass->tp)
                                           || classRefCount(sp->parentClass, theCurrentFunc->parentClass) == 1)
                             && (isconst(theCurrentFunc->tp) || isvolatile(theCurrentFunc->tp)))
                    {
                        tpthis = &tpx;
                        tpx.type = bt_pointer;
                        tpx.size = getSize(bt_pointer);
                        tpx.btp = basetype(f->thistp)->btp;
                        tpx.rootType = &tpx;
                        qualifyForFunc(theCurrentFunc, &tpx.btp, FALSE);
                    }
                    else if (sp->isDestructor)
                    {
                        tpthis = &tpx;
                        tpx.type = bt_pointer;
                        tpx.size = getSize(bt_pointer);
                        tpx.btp = basetype(basetype(f->thistp)->btp);
                        tpx.rootType = &tpx;
                    }
                    if (islrqual(sp->tp) || isrrqual(sp->tp))
                    {
                        BOOLEAN lref = lvalue(f->thisptr);
                        if (isstructured(basetype(f->thistp)->btp) && f->thisptr->type != en_not_lvalue)
                        {
                            EXPRESSION *expx = f->thisptr;
                            if (expx->type == en_thisref)
                                expx = expx->left;
                            if (expx->type == en_func)
                            {
                                if (expx->v.func->returnSP)
                                {
                                    if (!expx->v.func->returnSP->anonymous)
                                        lref = TRUE;
                                }
                            }
                            else
                            {
                                lref = TRUE;
                            }
                        }
                        if (isrrqual(sp->tp))
                        {
                            if (lref)
                                return FALSE;
                        }
                        else if (!lref)
                            return FALSE;
                    }
                    m = 0;
                    getSingleConversion(tpp, tpthis, f->thisptr, &m, seq, sp, userFunc ? &userFunc[n] : NULL, TRUE);
                    m1 = m;
                    while (m1 && seq[m1-1] == CV_IDENTITY)
                        m1--;
                    if (m1 > 10)
                    {
                        return FALSE;
                    }
                    for (i=0; i < m; i++)
                        if (seq[i] == CV_NONE)
                            return FALSE;
                    memcpy(arr+pos, seq, m * sizeof(enum e_cvsrn));
                    sizes[n++] = m;
                    pos += m;
                }
            }
        }
        else
        {
            if (ismember(sp))
            {
                TYPE *argtp = sp->parentClass->tp;
                if (!argtp)
                {
                    return FALSE;
                }
                else if (a || hrt)
                {
                    getSingleConversion(argtp, a ? a->tp : ((SYMBOL *)(*hrt)->p)->tp, a ? a->exp : NULL, 
                                        &m, seq, sp, userFunc ? &userFunc[n] : NULL, TRUE);
                    if (a)
                        a = a->next;
                    else if (hrt)
                        hrt = &(*hrt)->next;
                }
            }
        }
        // before matching the args see if  this function is a constructor and uses initializer-list 
        // as the first and only undefaulted param, and has at least one argument passed
        if (sp->isConstructor && (a || (hrt && *hrt)))
        {
            HASHREC *hr = basetype(sp->tp)->syms->table[0];
            if (((SYMBOL *)hr->p)->thisPtr)
                hr = hr->next;
            if (!hr->next ||  ((SYMBOL *)hr->next->p)->init)
            {
                TYPE *tp = ((SYMBOL *)hr->p)->tp;
                if (basetype(tp)->type == bt_lref)
                    tp = basetype(tp)->btp;
                if (isstructured(tp))
                {
                    SYMBOL *sym = (basetype(tp)->sp);
                    if (sym->parentNameSpace && !strcmp(sym->parentNameSpace->name , "std")) 
                    {
                        if (!strcmp(sym->name, "initializer_list") && sym->templateLevel)
                        {
                            initializerListType = sym->templateParams->next->p->byClass.val;
                            sp->initializer_list = TRUE;
                        }
                    }
                }
            }
        }
        while (*hr && (a || (hrt && *hrt)))
        {
            SYMBOL *argsym = (SYMBOL *)(*hr)->p;
            if (argsym->tp->type != bt_any)
            {
                TYPE *tp;
                if (argsym->constop)
                    break;
                if (argsym->storage_class != sc_parameter)
                    return FALSE;
                if (!tr && argsym->tp->type == bt_templateparam &&  argsym->tp->templateParam->p->packed)
                    tr = argsym->tp->templateParam->p->byPack.pack;
                if (tr)
                    tp = tr->p->byClass.val;
                else
                    tp = argsym->tp;
                if (basetype(tp)->type == bt_ellipse)
                {
                    arr[pos] = CV_ELLIPSIS;
                    sizes[n++] = 1;
                    return TRUE;
                }
                m = 0;
                if (initializerListType)
                {
                    if (a && a->nested)
                    {
                        getInitListConversion(initializerListType, a->nested, NULL, &m, seq, sp, userFunc ? &userFunc[n] : NULL);
                    }
                    else
                    {
                        getSingleConversion(initializerListType, a ? a->tp : ((SYMBOL *)(*hrt)->p)->tp, a ? a->exp : NULL, &m, seq,
                                        sp, userFunc ? &userFunc[n] : NULL, TRUE);
                    }
                }
                else if (a && a->nested)
                {
                    getInitListConversion(basetype(tp), a->nested, NULL, &m, seq, sp, userFunc ? &userFunc[n] : NULL);
                }
                else
                {
                    TYPE *tp2 = tp;
                    if (isref(tp2))
                        tp2 = basetype(tp2)->btp;
                    if (a && a->tp->type == bt_aggregate && (isfuncptr(tp2) || basetype(tp2)->type == bt_memberptr && isfunction(basetype(tp2)->btp)) )
                    {
                        MatchOverloadedFunction(tp2, &a->tp, a->tp->sp, &a->exp, 0);
                    }
                    getSingleConversion(tp, a ? a->tp : ((SYMBOL *)(*hrt)->p)->tp, a ? a->exp : NULL, &m, seq,
                            sp, userFunc ? &userFunc[n] : NULL, TRUE);
                }
                m1 = m;
                while (m1 && seq[m1-1] == CV_IDENTITY)
                    m1--;
                if (m1 > 10)
                {
                    return FALSE;
                }
                for (i=0; i < m; i++)
                    if (seq[i] == CV_NONE)
                        return FALSE;
                memcpy(arr+pos, seq, m * sizeof(enum e_cvsrn));
                sizes[n++] = m;
                pos += m;
            }
            if (tr)
                tr = tr->next;
            if (!initializerListType && !tr)
                hr = &(*hr)->next;
            if (a)
                a = a->next;
            else
                hrt = &(*hrt)->next;
        }
        if (*hr && !initializerListType)
        {
            SYMBOL *sym = (SYMBOL *)(*hr)->p;
            if (sym->init || sym->deferredCompile || sym->packed)
            {
                return TRUE;
            }
            if (basetype(sym->tp)->type == bt_ellipse)
            {
                sizes[n++] = 1;
                arr[pos++] = CV_ELLIPSIS;
                return TRUE;
            }
            if (sym->tp->type == bt_void || sym->tp->type == bt_any)
                return TRUE;
            return FALSE;
        }
        return a == NULL;
    }
}
SYMBOL *detemplate(SYMBOL *sym, FUNCTIONCALL *args, TYPE *atp)
{
    if (sym->templateLevel)
    {
        if (atp || args)
        {
            BOOLEAN linked = FALSE;
            if (sym->parentNameSpace && !sym->parentNameSpace->value.i)
            {
                LIST *list;
                SYMBOL *ns = sym->parentNameSpace;
                linked = TRUE;
                ns->value.i++;
            
                list = Alloc(sizeof(LIST));
                list->next = nameSpaceList;
                list->data = ns;
                nameSpaceList = list;
            
                ns->nameSpaceValues->next = globalNameSpace;
                globalNameSpace = ns->nameSpaceValues;
            }
            if (args && !TemplateIntroduceArgs(sym->templateParams, args->templateParams))
                sym = NULL;
            else if (atp)
                sym = TemplateDeduceArgsFromType(sym, atp);
            else if (args->ascall)
                sym = TemplateDeduceArgsFromArgs(sym, args);
            else
                sym = TemplateDeduceWithoutArgs(sym);
            if (linked)
            {
                SYMBOL *sp = (SYMBOL *)nameSpaceList->data;
                sp->value.i--;
                nameSpaceList = nameSpaceList->next;
                globalNameSpace = globalNameSpace->next;
            }
        }
        else
        {
            sym = NULL;
        }
    }
    return sym;
}
static void WeedTemplates(SYMBOL **table, int count, FUNCTIONCALL *args, TYPE *atp)
{
    int i = count;
    if (atp || !args->astemplate)
    {
        for (i=0; i < count ;i++)
            if (table[i] && (!table[i]->templateLevel || !table[i]->templateParams))
                break;
    }
    else
    {
        for (i=0; i < count ;i++)
            if (table[i] && (!table[i]->templateLevel || !table[i]->templateParams))
                table[i] = NULL;
    }
    if (i < count)
    {
        // one or more first class citizens, don't match templates
        for (i=0; i < count ;i++)
            if (table[i] && table[i]->templateLevel && table[i]->templateParams)
                table[i] = NULL;
    }
    else
    {
        TemplatePartialOrdering(table, count, args, atp, FALSE, TRUE);
    }
}
SYMBOL *GetOverloadedTemplate(SYMBOL *sp, FUNCTIONCALL *args)
{
    SYMBOL *found1 = NULL, *found2 = NULL;
    SYMBOL **spList, **spFilterList;
    LIST gather;
    enum e_cvsrn ** icsList;
    int **lenList;
    SYMBOL *** funcList;
    int n=0,i,argCount=0;
    HASHREC *search = sp->tp->syms->table[0];
    INITLIST *il = args->arguments;
    gather.next = NULL;
    gather.data = sp;
    while (il)
    {
        il = il->next;
        argCount++;
    }
    while (search)
    {
        search = search->next;
        n++;
    }
    spList = (SYMBOL **)Alloc(sizeof(SYMBOL *) * n);
    spFilterList = (SYMBOL **)Alloc(sizeof(SYMBOL *) * n);
    icsList = (enum e_cvsrn **)Alloc(sizeof(enum e_cvsrn *) * n);
    lenList = (int **)Alloc(sizeof(int *) * n);
    funcList = (struct sym ***)Alloc(sizeof(SYMBOL **) * n);
        
    n = insertFuncs(spList, spFilterList, &gather, args, NULL);
    if (n != 1 || (spList[0] && !spList[0]->isDestructor))
    {
        if (args->ascall)
        {
            GatherConversions(sp, spList, n, args, NULL, icsList, lenList, argCount, funcList, 0);
            SelectBestFunc(spList, icsList, lenList, args, argCount, n, funcList);
        }
        WeedTemplates(spList, n, args, NULL);
        for (i=0; i < n && !found1; i++)
        {
            int j;
            found1 = spList[i];
            for (j=i+1; j < n && found1 && !found2; j++)
            {
                if (spList[j] && found1 != spList[j] && !sameTemplate(found1->tp, spList[j]->tp))
                {
                    found2 = spList[j];
                }
            }
        }
    }
    else
    {
        found1 = spList[0];
    }
    if (!found1 || found2)
    {
//        if (!templateNestingCount)
//            errorsym(ERR_NO_TEMPLATE_MATCHES, sp);
        return NULL;
    }
    return found1;
}
void weedgathering(LIST **gather)
{
    while (*gather)
    {
        LIST **p = &(*gather)->next;
        while (*p)
        {
            if ((*p)->data == (*gather)->data)
                *p = (*p)->next;
            else
                p = &(*p)->next;
        }
        gather = &(*gather)->next;
    }
}
static int insertFuncs(SYMBOL **spList, SYMBOL **spFilterList, LIST *gather, FUNCTIONCALL *args, TYPE *atp)
{
    int n = 0;
    while (gather)
    {
        HASHREC **hr = ((SYMBOL *)gather->data)->tp->syms->table;
        while (*hr)
        {
            int i;
            SYMBOL *sym = (SYMBOL *)(*hr)->p;
            for (i=0; i < n; i++)
                if (spFilterList[i] == sym || spFilterList[i]->mainsym == sym || spFilterList[i] == sym->mainsym ||
                    matchOverload(sym->tp, spFilterList[i]->tp, FALSE) && sym->overlayIndex == spFilterList[i]->overlayIndex)
                    break;

            if (i >= n && (!args || !args->astemplate || sym->templateLevel) && (!sym->instantiated || sym->isDestructor))
            {
                if (sym->templateLevel && (sym->templateParams || sym->isDestructor))
                {
                    if (sym->castoperator)
                    {
                        spList[n] = detemplate(sym, NULL, basetype(args->thistp)->btp) ;
                    }
                    else
                    {
                        spList[n] = detemplate(sym, args, atp) ;
                    }
                }
                else
                {
                    spList[n] = sym;
                }
                spFilterList[n++] = sym;
            }
            hr = &(*hr)->next;
        }
        gather = gather->next;
    }
    return n;
}
static void doNames(SYMBOL *sym)
{
    if (sym->parentClass)
        doNames(sym->parentClass);
    SetLinkerNames(sym, lk_cdecl); 
}
SYMBOL *GetOverloadedFunction(TYPE **tp, EXPRESSION **exp, SYMBOL *sp, 
                              FUNCTIONCALL *args, TYPE *atp, BOOLEAN toErr, 
                              BOOLEAN maybeConversion, BOOLEAN toInstantiate, int flags)
{
    STRUCTSYM s;
    s.tmpl = 0;

    if (atp && ispointer(atp))
        atp = basetype(atp)->btp;
    if (atp && !isfunction(atp))
        atp = NULL;

    if (args && args->thisptr)
    {
        SYMBOL *spt = basetype (basetype(args->thistp)->btp)->sp;
        s.tmpl = spt->templateParams;
        if (s.tmpl)
            addTemplateDeclaration(&s);
    }
    if (!sp || sp->storage_class == sc_overloads)
    {
        LIST *gather = NULL;
        SYMBOL **flatList;    
        SYMBOL *found1 = NULL, *found2 = NULL;
        if (!cparams.prm_cplusplus && (!chosenAssembler->msil || !chosenAssembler->msil->allowExtensions || sp && !sp->tp->syms->table[0]->next))
        {
            sp = ((SYMBOL *)sp->tp->syms->table[0]->p);
            if (sp)
            {
                *exp = varNode(en_pc, sp);
                *tp = sp->tp;
            }
            return sp;
        }
        if (sp)
        {
            if (args || atp)
            {
                if (!sp->tp || (!sp->wasUsing && !sp->parentClass))
                {
                        // ok the sym is a valid candidate for argument search
                    if (args)
                    {
                        INITLIST *list = args->arguments;
                        while (list)
                        {
                            if (list->tp)
                                gather = searchOneArg(sp, gather, list->tp);
                            list = list->next;
                        }
                        if (args->thisptr)
                            gather = searchOneArg(sp, gather, args->thistp);
                    }
                    else
                    {
                        HASHREC **hr = atp->syms->table;
                        while (*hr)
                        {
                            SYMBOL *sym = (SYMBOL *)(*hr)->p;
                            if (sym->storage_class != sc_parameter)
                                break;
                            gather = searchOneArg(sp, gather, sym->tp);
                            hr = &(*hr)->next;
                        }
                    }
                }
                weedToFunctions(&gather);
            }
            if (sp->tp)
            {
                LIST *lst = gather;
                while (lst)
                {
                    if (lst->data == sp)
                        break;
                    lst = lst->next;
                }
                if (!lst)
                {
                    lst = Alloc(sizeof(LIST));
                    lst->data = sp;
                    lst->next = gather;
                    gather = lst;
                }
            }
            weedgathering(&gather);
        }
        // ok got the initial list, time for phase 2
        // which is to add any other functions that have to be added...
        // constructors, member operator '()' and so forth...
        if (gather)
        {
            // we are only doing global functions for now...
            // so nothing here...
            
        }
        if (maybeConversion)
        {
            if (args->arguments && !args->arguments->next && !args->arguments->nested) // one arg
                gather = GetMemberCasts(gather, basetype(args->arguments->tp)->sp);
        }
        // pass 3 - the actual argument-based resolution
        if (gather)
        {
            LIST *lst2;
            int n = 0;
            INITLIST *argl = args->arguments;
            while (argl)
            {
                if (argl->tp && argl->tp->type == bt_aggregate)
                {
                    HASHREC *hr = argl->tp->syms->table[0];
                    SYMBOL *func = (SYMBOL *)hr->p;
                    if (!func->templateLevel && !hr->next)
                    {
                        argl->tp = func->tp;
                        argl->exp = varNode(en_pc, func);
                       func->genreffed = TRUE;
                        InsertInline(func);
                        InsertExtern(func);
                    }
                }
                argl = argl->next;
            }

            lst2 = gather;
            while (lst2)
            {
                HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                while (*hr)
                {
                    SYMBOL *sym = (SYMBOL *)(*hr)->p;
                    if ((!args || !args->astemplate || sym->templateLevel) && (!sym->instantiated || sym->isDestructor))
                    {
                        n++;
                    }
                    hr = &(*hr)->next;
                }
                lst2 = lst2->next;
            }
            if (args || atp)
            {
                int i;
                SYMBOL **spList, **spFilterList;
                SYMBOL ***funcList;
                enum e_cvsrn **icsList;
                int **lenList;
                BOOLEAN done = FALSE;
                int argCount = 0;
                if (args)
                {
                    INITLIST *v = args->arguments;
                    while (v)
                    {
                        argCount++;
                        v = v->next;
                    }
                    if (args->thisptr)
                        argCount++;
                }
                else
                {
                    HASHREC **hr = atp->syms->table;
                    while (*hr && ((SYMBOL *)(*hr)->p)->storage_class == sc_parameter)
                    {
                        argCount++;
                        hr = &(*hr)->next;
                    }
                    if (*hr && ismember(((SYMBOL *)(*hr)->p)))
                        argCount++;
                }
                spList = (SYMBOL **)Alloc(sizeof(SYMBOL *) * n);
                spFilterList = (SYMBOL **)Alloc(sizeof(SYMBOL *) * n);
                icsList = (enum e_cvsrn **)Alloc(sizeof(enum e_cvsrn *) * n);
                lenList = (int **)Alloc(sizeof(int *) * n);
                funcList = (struct sym ***)Alloc(sizeof(SYMBOL **) * n);

                n = insertFuncs(spList, spFilterList, gather, args, atp);
                if (n != 1 || (spList[0] && !spList[0]->isDestructor))
                {
                    if (atp || args->ascall)
                    {
                        GatherConversions(sp, spList, n, args, atp, icsList, lenList, argCount, funcList, flags & _F_INITLIST);
                        SelectBestFunc(spList, icsList, lenList, args, argCount, n, funcList);
                    }
                    WeedTemplates(spList, n, args, atp);
                    for (i=0; i < n && !found1; i++)
                    {
                        int j;
                        found1 = spList[i];
                        for (j=i+1; j < n && found1 && !found2; j++)
                        {
                            if (spList[j] && found1 != spList[j] && !sameTemplate(found1->tp, spList[j]->tp))
                            {
                                found2 = spList[j];
                            }
                        }
                    }
    #ifdef DEBUG
                    // this block to aid in debugging unfound functions...
                    if (toErr && (!found1 || (found1 && found2)) && !templateNestingCount)
                    {
                        memset(spFilterList, 0, sizeof(SYMBOL *) * n);
                        n = insertFuncs(spList, spFilterList, gather, args, atp);
                        if (atp || args->ascall)
                        {
                            GatherConversions(sp, spList, n, args, atp, icsList, lenList, argCount, funcList, flags & _F_INITLIST);
                            SelectBestFunc(spList, icsList, lenList, args, argCount, n, funcList);
                        }
                        WeedTemplates(spList, n, args, atp);
                    }
    #endif
                }
                else
                {
                    found1 = spList[0];
                }
            }
            else
            {
                HASHREC **hr = (HASHREC **)((SYMBOL *)gather->data)->tp->syms->table;
                found1 = (SYMBOL *)(*hr)->p;
                if (n > 1)
                    found2 = (SYMBOL *)(*(HASHREC **)(*hr))->p;
            }
        }
        // any errors
        if (toErr || (found1 && !found2))
        {
            if (!found1)
            {
                if (args && args->arguments && !args->arguments->next // one arg
                    && sp && sp->isConstructor) // conversion constructor
                {
                    errortype(ERR_CANNOT_CONVERT_TYPE, args->arguments->tp, sp->parentClass->tp);
                }
                else if (!sp)
                {
                    if (*tp && isstructured(*tp))
                    {
                        char buf[4096], *p;
                        int n;
                        INITLIST *a;
                        memset(buf, 0, sizeof(buf));
                        unmangle(buf, basetype(*tp)->sp->decoratedName);
                        n = strlen(buf);
                        p = strrchr(buf, ':');
                        if (p)
                            p++;
                        else
                            p = buf;
                        strcpy(buf + n + 2, p);
                        buf[n] = buf[n+1] = ':';
                        strcat(buf, "(");
                        a = args->arguments;
                        while (a)
                        {
                            typeToString(buf + strlen(buf), a->tp);
                            if (a->next)
                                strcat(buf, ",");
                            a = a->next;
                        }
                        strcat(buf, ")");
                        errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, buf);                        
                    }
                    else
                    {
                        errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, "unknown");
                    }
                        
                }
                else
                {
                    SYMBOL *sym = Alloc(sizeof(SYMBOL));
                    sym->parentClass = sp->parentClass;
                    sym->name = sp->name;
                    if (atp)
                    {
                        sym->tp = atp;
                    }
                    else
                    {
                        int v = 1;
                        INITLIST *a = args->arguments;
                        sym->tp = Alloc(sizeof(TYPE));
                        sym->tp->type = bt_func;
                        sym->tp->size = getSize(bt_pointer);
                        sym->tp->btp = &stdint;
                        sym->tp->rootType = sym->tp;
                        sym->tp->syms = CreateHashTable(1);
                        sym->tp->sp = sym;
                        while (a)
                        {
                            SYMBOL *sym1 = Alloc(sizeof(SYMBOL));
                            char nn[10];
                            my_sprintf(nn, "%d", v++);
                            sym1->name = litlate(nn);
                            sym1->tp = a->tp;
                            insert(sym1, sym->tp->syms);
                            a = a->next;
                        }
                    }
                    SetLinkerNames(sym, lk_cpp);
    
                    errorsym(ERR_NO_OVERLOAD_MATCH_FOUND, sym);
                }
            }
            else if (found1 && found2)
            {
				if (toErr)
	                errorsym2(ERR_AMBIGUITY_BETWEEN, found1, found2);
				else
					found1 = found2 = NULL;
            }
            else if (found1->deleted && !templateNestingCount)
            {
				if (toErr)
	                errorsym(ERR_DELETED_FUNCTION_REFERENCED, found1);
				else
					found1 = NULL;
            }
            if (found1)
            {
                if (theCurrentFunc && !found1->constexpression)
                {
                    theCurrentFunc->nonConstVariableUsed = TRUE;
                }
                if (found1->deprecationText)
                    deprecateMessage(found1);
                if (!(flags & _F_SIZEOF))
                {
                    if (found1->templateLevel && (found1->templateParams || found1->isDestructor))
                    {
                        found1 = found1->mainsym;
                        if (found1->castoperator)
                        {
                            found1 = detemplate(found1, NULL, basetype(args->thistp)->btp) ;
                        }
                        else
                        {
                            found1 = detemplate(found1, args, atp) ;
                        }
                    }
                    GENREF(found1);
                    if (found1->templateLevel && !templateNestingCount && found1->templateParams)
                    {
                        found1 = TemplateFunctionInstantiate(found1, FALSE, FALSE);
                    }
                    else
                    {
                        if (toInstantiate && found1->deferredCompile && !found1->inlineFunc.stmt)
                        {
                            deferredCompileOne(found1);
                        }
                    }
                }
                if (isautotype(basetype(found1->tp)->btp))
                    errorsym(ERR_AUTO_FUNCTION_RETURN_TYPE_NOT_DEFINED, found1);
            }
        }
        if (!toErr && found2)
        {
            sp = NULL;
        }
        else
        {
            sp = found1;
            if (sp)
            {
                *exp = varNode(en_pc, sp);
                *tp = sp->tp;
            }
        }
    }

    if (s.tmpl)
        dropStructureDeclaration();
    return sp;
}
SYMBOL *MatchOverloadedFunction(TYPE *tp, TYPE **mtp, SYMBOL *sp, EXPRESSION **exp, int flags)
{
    FUNCTIONCALL fpargs;
    INITLIST ** args = &fpargs.arguments;
    EXPRESSION *exp2 = *exp;
    HASHREC *hrp;
    tp = basetype(tp);
    if (isfuncptr(tp) || tp->type == bt_memberptr)
    {
        hrp = basetype(basetype(tp)->btp)->syms->table[0];
    }
    else
    {
        hrp = NULL;
        if (!*exp)
            return FALSE;
        if ((*exp)->v.func->sp->tp->syms)
        {
            HASHTABLE *syms = (*exp)->v.func->sp->tp->syms;
            hrp  = syms->table[0];
            if (hrp && ((SYMBOL *)hrp->p)->tp->syms)
                hrp = ((SYMBOL *)hrp->p)->tp->syms->table[0];
            else
                hrp = NULL;
        }
    }
    while (castvalue(exp2))
        exp2 = exp2->left;
           
    memset(&fpargs, 0, sizeof(fpargs));
    if (hrp && ((SYMBOL *)hrp->p)->thisPtr)
    {
        fpargs.thistp = ((SYMBOL *)hrp->p)->tp;
        fpargs.thisptr = intNode(en_c_i, 0);
        hrp = hrp->next;
    }
    else if (tp->type == bt_memberptr)
    {
        fpargs.thistp = Alloc(sizeof(TYPE));
        fpargs.thistp->type = bt_pointer;
        fpargs.thistp->size = getSize(bt_pointer);
        fpargs.thistp->btp = tp->sp->tp;
        fpargs.thistp->rootType = fpargs.thistp;
        fpargs.thisptr = intNode(en_c_i, 0);
    }
    while (hrp)
    {
        *args = Alloc(sizeof(INITLIST));
        (*args)->tp = ((SYMBOL *)hrp->p)->tp;
        (*args)->exp = intNode(en_c_i, 0);
        if (isref((*args)->tp))
            (*args)->tp = basetype((*args)->tp)->btp;
        args = &(*args)->next;
        hrp = hrp->next;
    }
    if (exp2 && exp2->type == en_func)
       fpargs.templateParams = exp2->v.func->templateParams;
    fpargs.ascall = TRUE;
    return GetOverloadedFunction(mtp, exp, sp, &fpargs, NULL, TRUE, FALSE, TRUE, flags); 
}
                         