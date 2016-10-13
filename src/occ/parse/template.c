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
extern NAMESPACEVALUES *localNameSpace;
extern LIST *externals;
extern NAMESPACEVALUES *globalNameSpace;
extern TYPE stdany;
extern STRUCTSYM *structSyms;
extern LIST *deferred;
extern LIST *nameSpaceList;
extern INCLUDES *includes;
extern int inDefaultParam;
extern LINEDATA *linesHead, *linesTail;
extern int packIndex;
extern int expandingParams;
extern TYPE stdpointer;
extern TYPE stdbool;
extern TYPE stdchar;
extern TYPE stdunsignedchar;
extern TYPE stdwidechar;
extern TYPE stdshort;
extern TYPE stdchar16t;
extern TYPE stdunsignedshort;
extern TYPE stdint;
extern TYPE stdunsigned;
extern TYPE stdchar32t;
extern TYPE stdlong;
extern TYPE stdunsignedlong;
extern TYPE stdlonglong;
extern TYPE stdunsignedlonglong;
extern TYPE stdfloat;
extern TYPE stddouble;
extern TYPE stdlongdouble;
extern TYPE stdpointer;
extern TYPE stdchar16t;
extern TYPE stdpointer; // fixme
extern TYPE stdfloatcomplex;
extern TYPE stddoublecomplex;
extern TYPE stdlongdoublecomplex;
extern TYPE stdfloatimaginary;
extern TYPE stddoubleimaginary;
extern TYPE stdlongdoubleimaginary;
extern LIST *openStructs;
extern int structLevel;
extern TYPE stdnullpointer;
extern TYPE stdvoid;
extern int total_errors;
extern BOOLEAN inMatchOverload;
extern int codeLabel;
extern LAMBDA *lambdas;
extern int argument_nesting;
extern LINEDATA *linesHead, *linesTail;

int dontRegisterTemplate;
int instantiatingTemplate;
int inTemplateBody;
int templateNestingCount =0 ;
int templateHeaderCount ;
int inTemplateSpecialization = 0;
BOOLEAN parsingSpecializationDeclaration;
BOOLEAN inTemplateType;
SYMBOL *instantiatingMemberFuncClass;
static int inTemplateArgs;

struct templateListData *currents;

static LEXEME *TemplateArg(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST *arg, TEMPLATEPARAMLIST **lst);
static BOOLEAN fullySpecialized(TEMPLATEPARAMLIST *tpl);
static TEMPLATEPARAMLIST *copyParams(TEMPLATEPARAMLIST *t, BOOLEAN alsoSpecializations);
static BOOLEAN valFromDefault(TEMPLATEPARAMLIST *params, BOOLEAN usesParams, INITLIST **args);
static int pushContext(SYMBOL *cls, BOOLEAN all);

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
    instantiatingMemberFuncClass = NULL;
    parsingSpecializationDeclaration = FALSE;
}
EXPRESSION *GetSymRef(EXPRESSION *n)
{
    EXPRESSION *rv = NULL;
        
    switch (n->type)
    {
        case en_labcon:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_label:
        case en_pc:
        case en_threadlocal:
            return n;
        case en_tempref:
            return NULL;
        default:
            if (n->left)
                rv = GetSymRef(n->left);
            if (!rv && n->right)
                rv = GetSymRef(n->right);
            break;
    }
    return rv;
}
BOOLEAN equalTemplateIntNode(EXPRESSION *exp1, EXPRESSION *exp2)
{
//#ifdef PARSER_ONLY
//    return TRUE;
//#else
    if (equalnode(exp1, exp2))
        return TRUE;
    if (isintconst(exp1) && isintconst(exp2) && exp1->v.i == exp2->v.i)
        return TRUE;
    return FALSE;
//#endif
}
BOOLEAN templatecompareexpressions(EXPRESSION *exp1, EXPRESSION *exp2)
{
    if (isintconst(exp1) && isintconst(exp2))
        return exp1->v.i == exp2->v.i;
    if (exp1->type != exp2->type)
        return FALSE;
    switch (exp1->type)
    {
        case en_global:
        case en_auto:
        case en_labcon:
        case en_absolute:
        case en_pc:
        case en_const:
        case en_threadlocal:
            return exp1->v.sp == exp2->v.sp;
        case en_label:
            return exp1->v.i == exp2->v.i;
        case en_func:
            return exp1->v.func->sp == exp2->v.func->sp;
        case en_templateselector:
            return templateselectorcompare(exp1->v.templateSelector, exp2->v.templateSelector);
    }
    if (exp1->left && exp2->left)
        if (!templatecompareexpressions(exp1->left, exp2->left))
            return FALSE;
    if (exp1->right && exp2->right)
        if (!templatecompareexpressions(exp1->right, exp2->right))
            return FALSE;
    return TRUE;
}
BOOLEAN templateselectorcompare(TEMPLATESELECTOR *tsin1, TEMPLATESELECTOR *tsin2)
{
    TEMPLATESELECTOR *ts1 = tsin1->next, *tss1;
    TEMPLATESELECTOR *ts2 = tsin2->next, *tss2;
    if (ts1->isTemplate != ts2->isTemplate || ts1->sym != ts2->sym)
        return FALSE;
    tss1 = ts1->next;
    tss2 = ts2->next;
    while (tss1 && tss2)
    {
        if (strcmp(tss1->name, tss2->name))
            return FALSE;
        tss1 = tss1->next;
        tss2 = tss2->next;
    }
    if (tss1 || tss2)
        return FALSE;
    if (ts1->isTemplate)
    {
        if (!exactMatchOnTemplateParams(ts1->templateParams, ts2->templateParams))
            return FALSE;
    }
    return TRUE;
}
BOOLEAN templatecomparetypes(TYPE *tp1, TYPE *tp2, BOOLEAN exact)
{
    if (!tp1 || !tp2)
        return FALSE;
    if (basetype(tp1)->type == bt_templateselector && basetype(tp2)->type == bt_templateselector)
    {
        TEMPLATESELECTOR *left = basetype(tp1)->sp->templateSelector;
        TEMPLATESELECTOR *right = basetype(tp2)->sp->templateSelector;
        left = left->next->next;
        right = right->next->next;
        while (left && right)
        {
            if (strcmp(left->name, right->name))
                return FALSE;
            left = left->next;
            right = right->next;
        }
        return !left && !right;
    }
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
        {
            sym->deferredCompile = currents->bodyHead;
        }
    }
}
TEMPLATEPARAM *TemplateLookupSpecializationParam(char *name)
{
    TEMPLATEPARAM *rv = NULL;
    if (parsingSpecializationDeclaration)
    {
        TEMPLATEPARAMLIST *search = currents->args->next;
        while (search)
        {
            if (!strcmp(search->p->sym->name, name))
                break;
            search = search->next;
        }
        if (search)
            rv = search->p;
    }
    return rv;
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
            if (old->p->byNonType.dflt && sym->p->byNonType.dflt && !templatecompareexpressions(old->p->byNonType.dflt, sym->p->byNonType.dflt))
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    if (old && old->p->packed)
        old = NULL;
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
                if (sameTemplate(old->p->byClass.dflt, sym->p->byClass.dflt))
                {
                    if (!exactMatchOnTemplateArgs(basetype(old->p->byClass.dflt)->sp->templateParams, basetype(sym->p->byClass.dflt)->sp->templateParams))
                        return FALSE;
                }
                else
                {
                    if (!templatecomparetypes(old->p->byClass.dflt, sym->p->byClass.dflt, TRUE))
                        return FALSE;
                    if (!templatecomparetypes(sym->p->byClass.dflt, old->p->byClass.dflt, TRUE))
                        return FALSE;
                    if (isarray(old->p->byClass.dflt) != isarray(sym->p->byClass.dflt))
                        return FALSE;
                    if (isarray(old->p->byClass.dflt))
                        if  (!!basetype(old->p->byClass.dflt)->esize != !!basetype(sym->p->byClass.dflt)->esize)
                            return FALSE;
                }
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
                if (!!old->p->byNonType.dflt != !!sym->p->byNonType.dflt)
                    return FALSE;
//#ifndef PARSER_ONLY
                if (old->p->byNonType.dflt && sym->p->byNonType.dflt && !templatecompareexpressions(old->p->byNonType.dflt, sym->p->byNonType.dflt))
                    return FALSE;
//#endif
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
//#ifndef PARSER_ONLY
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
//#endif
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
BOOLEAN matchTemplateSpecializationToParams(TEMPLATEPARAMLIST *param, TEMPLATEPARAMLIST *special, SYMBOL *sp)
{
    while (param && !param->p->packed && special)
    {
        if (param->p->type != special->p->type)
        {
            if (param->p->type != kw_typename || special->p->type != kw_template)
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else if (param->p->type == kw_template)
        {
            if (!exactMatchOnTemplateParams(param->p->byTemplate.args, special->p->byTemplate.dflt->templateParams->next))
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else if (param->p->type == kw_int)
        {
            if (param->p->byNonType.tp->type != bt_templateparam && !comparetypes(param->p->byNonType.tp, special->p->byNonType.tp, FALSE) && (!ispointer(param->p->byNonType.tp) || !isconstzero(param->p->byNonType.tp, special->p->byNonType.dflt)))
                errorsym(ERR_INCORRECT_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        param = param->next;
        special = special->next;
    }
    if (param)
    {
        if (!param->p->packed)
        {
            errorsym(ERR_TOO_FEW_ARGS_PASSED_TO_TEMPLATE, sp);
        }
        else
        {
            param = NULL;
            special = NULL;
        }
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
                transfer->p->byClass.txtdflt = old->p->byClass.txtdflt;
                transfer->p->byClass.txtargs = old->p->byClass.txtargs;
                transfer = transfer->next;
                old = old->next;
            }
        }
        else if (sym->next)
        {
            if (!exactMatchOnTemplateParams(old->next, sym->next))
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
            rv = sym;
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
TEMPLATEPARAMLIST **expandPackedArg(TEMPLATEPARAMLIST **lst, TEMPLATEPARAMLIST *select, int index)
{
    int i;
	TEMPLATEPARAMLIST *check = NULL;
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
            check = rv;
        }
		else
		{
	        check = select;
		}
    }
    else if (select->p->type == kw_delete)
    {
        SYMBOL *sym = select->p->sym;
        TEMPLATEPARAMLIST *args = select->p->byDeferred.args;
        TEMPLATEPARAMLIST *val = NULL, **lst = &val;
        while (args)
        {
            lst = expandPackedArg(lst, args, index);
            args = args->next;
        }
        sym = TemplateClassInstantiateInternal(sym, val, TRUE);
        val = Alloc(sizeof(TEMPLATEPARAMLIST));
        val->p = Alloc(sizeof(TEMPLATEPARAM));
        val->p->type = kw_typename;
        val->p->byClass.dflt = sym->tp;
		check = val;
    }
    else
    {
        TEMPLATEPARAMLIST *rv = Alloc(sizeof(TEMPLATEPARAMLIST));
        rv->p = select->p;
        check = rv;
    }
	if (check)
	{
		if (*lst)
		{
			TEMPLATEPARAMLIST **pos = &(*lst)->p->byPack.pack;
			while (*pos)
				pos = &(*pos)->next;
			*pos = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
			(*pos)->p = check->p;
		}
		else
		{
			*lst = Alloc(sizeof(TEMPLATEPARAMLIST));
			(*lst)->p = check->p;
			lst = &(*lst)->next;
		}
	}
	return lst;
}
TEMPLATEPARAMLIST **expandArgs(TEMPLATEPARAMLIST **lst, TEMPLATEPARAMLIST *select, BOOLEAN packable)
{
    TEMPLATEPARAMLIST *packs[500];
    int count = 0;
    int n,i;
    getPackedArgs(packs, &count, select);
    if (!packable)
    {
        if (select->p->packed && packIndex >= 0)
        {
            TEMPLATEPARAMLIST *templateParam = select->p->byPack.pack;
            int i;
            for (i=0; i < packIndex && templateParam; i++)
                templateParam = templateParam->next;
            if (templateParam)
            {
                *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                (*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                *(*lst)->p = *templateParam->p;
                (*lst)->p->byClass.dflt = (*lst)->p->byClass.val;
                lst = &(*lst)->next;   
                return lst;
            }
        }
        *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
        (*lst)->p = select->p;
        lst = &(*lst)->next;   
        return lst;
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
        if (select)
        {
            lst = expandPackedArg(lst, select, 0);
        }
    }
    else
    {
        for (i=0; i < n; i++)
        {
            lst = expandPackedArg(lst, select, i);
        }
    }
    return lst;
}
static BOOLEAN constructedInt(LEXEME *lex, SYMBOL *funcsp)
{
    // depends on this starting a type
    BOOLEAN rv = FALSE;
    TYPE *tp;
    LEXEME *placeholder = lex;
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    BOOLEAN defd = FALSE;
    SYMBOL *sp = NULL;
    BOOLEAN notype = FALSE;
    BOOLEAN oldTemplateType = inTemplateType;
    tp = NULL;
        
    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
    lex = getBasicType(lex, funcsp, &tp, NULL, FALSE, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL, NULL, FALSE, FALSE);
    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
    if (isint(tp))
    {
        if (MATCHKW(lex, openpa))
        {
            rv = TRUE;
        }
    }
    lex = prevsym(placeholder);
    return rv;
}
LEXEME *GetTemplateArguments(LEXEME *lex, SYMBOL *funcsp, SYMBOL *templ, TEMPLATEPARAMLIST **lst)
{
    TEMPLATEPARAMLIST **start = lst;
    TEMPLATEPARAMLIST *orig = NULL;
    BOOLEAN first = TRUE;
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
            if ((orig && orig->p->type != kw_int) || !orig && startOfType(lex, TRUE) && !constructedInt(lex, funcsp))
            {
                SYMBOL *sym = NULL;
                inTemplateArgs--;
                lex = get_type_id(lex, &tp, funcsp, sc_parameter, FALSE, TRUE);
                inTemplateArgs++;
                if (tp && !templateNestingCount)
                    tp = PerformDeferredInitialization(tp, NULL);
                if (MATCHKW(lex, ellipse))
                {
                    lex = getsym();
					while (isref(tp))
						tp = basetype(tp)->btp;
                    if (templateNestingCount && tp->type == bt_templateparam)
                    {
                        *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->p = tp->templateParam->p;
                        //if (!tp->templateParam->p->packed)
                            //error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                    }
                    else if (tp->type == bt_templateparam)
                    {
                        lst = expandArgs(lst, tp->templateParam, TRUE);
                    }
					else if (tp->type == bt_templateselector)
					{
						lst = expandArgs(lst, tp->sp->templateSelector->next->templateParams, TRUE);
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
                else if (tp && tp->type == bt_templateparam)
                {
                    TEMPLATEPARAMLIST **last = lst;
                    if (inTemplateSpecialization)
                    {
                        *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->p = tp->templateParam->p;
                        lst = &(*lst)->next;
                        if (tp->templateParam->p->packed)
                        {
                            TEMPLATEPARAMLIST *find = tp->templateParam->p->byPack.pack;
                            while (find)
                            {
                                if (!find->p->byClass.dflt)
                                    find->p->byClass.dflt = tp;
                                find = find->next;
                            }
                        }
                        else
                        {
                            if (!tp->templateParam->p->byClass.dflt)
                                tp->templateParam->p->byClass.dflt = tp;
                        }
                    }
                    else
                    {
                        lst = expandArgs(lst, tp->templateParam, FALSE);
                    }
                    /*
                    lst = expandArgs(lst, tp->templateParam, FALSE);
                    if (inTemplateSpecialization)
                    {
                        while (*last)
                        {
                            if ((*last)->p->packed)
                            {
                                last = &(*last)->p->byPack.pack;
                                continue;
                            }   
                            if (!(*last)->p->byClass.dflt)
                                (*last)->p->byClass.dflt = tp;
                            last = &(*last)->next;
                        }
                    }
                    */
                }
                else if (orig && orig->p->packed)
                {
                    TEMPLATEPARAMLIST **p;
                    if (first)
                    {
                        *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                        (*lst)->p->type = kw_typename;
                        (*lst)->p->packed = TRUE;
                        first = FALSE;
                    }
                    p = &(*lst)->p->byPack.pack;
                    while (*p)
                        p = &(*p)->next;
                    *p = Alloc(sizeof(TEMPLATEPARAMLIST));
                    (*p)->p = Alloc(sizeof(TEMPLATEPARAM));
                    (*p)->p->type = kw_typename;
                    (*p)->p->byClass.dflt = tp;
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
                        lex = nestedSearch(lex, &sp, NULL, NULL, NULL, NULL, FALSE, sc_global, FALSE, FALSE);
                        if (sp && sp->tp->templateParam)
                        {
                            lex = getsym();
                            if (!MATCHKW(lex, rightshift) && !MATCHKW(lex, gt) && !MATCHKW(lex, comma))
                            {
                                lex = prevsym(last);
                                goto join;
                            }
                            else
                            {
                                *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = sp->tp->templateParam->p;
                                lst = &(*lst)->next;
                            }
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
                    LEXEME *start;
                    BOOLEAN skip;
join:
                    skip = FALSE;
					s = structSyms;
					name = NULL;
                    start = lex;
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
                        if (name->tp->type == bt_templateparam && name->tp->templateParam->p->type == kw_int)
                        {
                            lex = getsym();
                            if (MATCHKW(lex, ellipse))
                            {
                                TEMPLATEPARAMLIST *tpl = name->tp->templateParam;
                                TEMPLATEPARAMLIST **working;
                                *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                                *(*lst)->p = *tpl->p;
                                (*lst)->p->packed = TRUE;
                                working = &(*lst)->p->byPack.pack;
                                *working = NULL;

                                if (tpl->p->packed)
                                {
                                    tpl = tpl->p->byPack.pack;
                                    while (tpl)
                                    {
                                        *working = Alloc(sizeof(TEMPLATEPARAMLIST));
                                        (*working)->p = tpl->p;
                                        working = &(*working)->next;
                                        tpl = tpl->next;
                                    }
                                }
                                else
                                {
                                    *working = Alloc(sizeof(TEMPLATEPARAMLIST));
                                    (*working)->p = tpl->p;
                                    
                                }
                                lex = getsym();
                                skip = TRUE;
                                first = FALSE;
                            }
                            else
                            {
                                lex = prevsym(start);
                            }
                        }
                        if (!skip)
                        {
    						lex = expression_no_comma(lex, funcsp, NULL, &tp, &exp, NULL, _F_INTEMPLATEPARAMS);
	    					optimize_for_constants(&exp);
                            if (tp && tp->type == bt_templateparam)
                            {
                                lst = expandArgs(lst, tp->templateParam, FALSE);
                                skip = TRUE;
                                first = FALSE;
                            }
                        }
					}
					else
					{
						lex = expression_no_comma(lex, funcsp, NULL, &tp, &exp, NULL, _F_INTEMPLATEPARAMS);
						optimize_for_constants(&exp);
	  					if (!tp)
						{
							error(ERR_EXPRESSION_SYNTAX);
                        
						}
					}
                    if (!skip)
                    {
                        if (MATCHKW(lex, ellipse))
                        {
                            // lose p
                            lex = getsym();
                            if (templateNestingCount && tp->type == bt_templateparam)
                            {
                                *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = tp->templateParam->p;
                                if (!tp->templateParam->p->packed)
                                    error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                            }
                            else if (exp->type != en_packedempty)
                            {
                                // this is going to presume that the expression involved
                                // is not too long to be cached by the LEXEME mechanism.          
                                int oldPack = packIndex;
                                int count = 0;
                                SYMBOL *arg[200];
                                GatherPackedVars(&count, arg, exp);
                                expandingParams++;
                                if (count)
                                {
                                    int i;
                                    int n = CountPacks(arg[0]->tp->templateParam->p->byPack.pack);
                                    for (i=1; i < count; i++)
                                    {
                                        if (CountPacks(arg[i]->tp->templateParam->p->byPack.pack) != n)
                                        {
                                            error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                                            break;
                                        }
                                    }
                                    for (i=0; i < n; i++)
                                    {
                                        INITLIST *p = Alloc(sizeof(INITLIST));
                                        LEXEME *lex = SetAlternateLex(start);
                                        packIndex = i;
                                        expression_assign(lex, funcsp, NULL, &tp, &exp, NULL, _F_PACKABLE);
                                        SetAlternateLex(NULL);
                                        if (tp)
                                        {
                        					*lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                        					(*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                        					(*lst)->p->type = kw_int;
                        					(*lst)->p->byNonType.dflt = exp;
                        					(*lst)->p->byNonType.tp = tp;
                        					lst = &(*lst)->next;
                                        }
                                    }
                                }
                                expandingParams--;
                                packIndex = oldPack;
                            }
                        }
                        else if (tp && tp->type == bt_templateparam)
                        {
    						*lst = Alloc(sizeof(TEMPLATEPARAMLIST));
    						(*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
    						*(*lst)->p = *tp->templateParam->p;
                            if ((*lst)->p->packed)
                            {
                                (*lst)->p->byPack.pack = Alloc(sizeof(TEMPLATEPARAMLIST));
        						(*lst)->p->byPack.pack->p = Alloc(sizeof(TEMPLATEPARAM));
                                (*lst)->p->byPack.pack->p->type = kw_int;
        						(*lst)->p->byPack.pack->p->byNonType.dflt = exp;
    	    					(*lst)->p->byPack.pack->p->byNonType.val = NULL;
                            }
                            else
                            {
        						(*lst)->p->byNonType.dflt = exp;
    	    					(*lst)->p->byNonType.val = NULL;
                            }
    						lst = &(*lst)->next;
                        }
                        else if (orig && orig->p->packed)
                        {
                            TEMPLATEPARAMLIST **p;
                            if (first)
                            {
                                *lst = Alloc(sizeof(TEMPLATEPARAMLIST));
                                (*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
                                (*lst)->p->type = kw_int;
                                (*lst)->p->packed = TRUE;
                                first = FALSE;
                            }
                            p = &(*lst)->p->byPack.pack;
                            while (*p)
                                p = &(*p)->next;
                            *p = Alloc(sizeof(TEMPLATEPARAMLIST));
                            (*p)->p = Alloc(sizeof(TEMPLATEPARAM));
                            (*p)->p->type = kw_int;
        					(*p)->p->byNonType.dflt = exp;
        					(*p)->p->byNonType.tp = tp;
                        }
                        else
                        {
                            checkUnpackedExpression(exp);
        					*lst = Alloc(sizeof(TEMPLATEPARAMLIST));
        					(*lst)->p = Alloc(sizeof(TEMPLATEPARAM));
        					(*lst)->p->type = kw_int;
        					(*lst)->p->byNonType.dflt = exp;
        					(*lst)->p->byNonType.tp = tp;
        					lst = &(*lst)->next;
                        }
                    }
                }
            }
            if (MATCHKW(lex, comma))
                lex = getsym();
            else
                break;
            if (orig && !orig->p->packed)
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
//#ifndef PARSER_ONLY
                    if (PL->p->byNonType.dflt && !equalTemplateIntNode(PL->p->byNonType.dflt, PA->p->byNonType.val))
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
//#ifndef PARSER_ONLY
                if (old->p->byNonType.dflt && !equalTemplateIntNode(old->p->byNonType.dflt, sym->p->byNonType.val))
                    return FALSE;
//#endif
                break;
            default:
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    return !old && !sym;
}
SYMBOL *FindSpecialization(SYMBOL *sym, TEMPLATEPARAMLIST *templateParams)
{
    SYMBOL *candidate;
    LIST *lst = sym->specializations;
    while (lst)
    {
        candidate = (SYMBOL *)lst->data;
        if (candidate->templateParams && exactMatchOnTemplateArgs(templateParams->next, candidate->templateParams->p->bySpecialization.types))
        {
            return candidate;
        }
        lst = lst->next;
    }
    return NULL;
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
        {
            TEMPLATEPARAMLIST *l = templateParams;
            TEMPLATEPARAMLIST *r = candidate->templateParams;
            while (l && r)
            {
                l = l->next;
                r = r->next;
            }
            if (!l && !r)
                return candidate;
        }
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
    candidate->maintemplate = candidate;
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
    candidate->declline = candidate->origdeclline = includes->line;
    candidate->declfile = candidate->origdeclfile = includes->fname;
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
                if (params->p->type != kw_new)
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
                if (params->p->type != kw_new)
                    params->p->byClass.val = params->p->hold;
                params = params->next;
            }
        }
    }
}
SYMBOL *LookupFunctionSpecialization(SYMBOL *overloads, SYMBOL *sp)
{
    SYMBOL *found1 = NULL;
    SYMBOL *sym = (SYMBOL *)overloads->tp->syms->table[0]->p;
    SYMBOL *sd = getStructureDeclaration();
    saveParams(&sd, 1);
	if (sym->templateLevel && !sym->instantiated && (!sym->parentClass || sym->parentClass->templateLevel != sym->templateLevel))
    {
        found1 = detemplate(overloads->tp->syms->table[0]->p, NULL, sp->tp);
    	if (found1 && allTemplateArgsSpecified(found1, found1->templateParams->next))
        {
            TEMPLATEPARAMLIST *tpl;
            HASHREC *hr = overloads->tp->syms->table[0]->next;
            while (hr)
            {
                if (exactMatchOnTemplateArgs(found1->templateParams, ((SYMBOL *)hr->p)->templateParams))
                    return (SYMBOL *)hr->p;
                hr = hr->next;
            }
    		sp->templateParams->p->bySpecialization.types = copyParams(found1->templateParams->next, FALSE);
            tpl = sp->templateParams->p->bySpecialization.types;
            while (tpl)
            {
                tpl->p->byClass.dflt = tpl->p->byClass.val;
                tpl->p->byClass.val = NULL;
                tpl = tpl->next;
            }
    	    SetLinkerNames(sp, lk_cdecl);
            found1 = sp;
        }
        else
        {
            found1= NULL;
        }
    }
    restoreParams(&sd, 1);
    return found1;
    /*
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
		if (sym->templateLevel && !sym->instantiated && (!sym->parentClass || sym->parentClass->templateLevel != sym->templateLevel))
        {
			SYMBOL *sp1 = detemplate(sym, NULL, sp->tp);
            if (sp1 && sp1->tp->type != bt_any)
                spList[n++] = sp1;
        }
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
	if (found1 && !found2 && allTemplateArgsSpecified(found1, found1->templateParams->next))
	{
        TEMPLATEPARAMLIST *tpl;
//		sp->templateParams = copyParams(found1->templateParams, TRUE);
//        sp->templateParams->next = NULL;
		sp->templateParams->p->bySpecialization.types = copyParams(found1->templateParams->next, FALSE);
        tpl = sp->templateParams->p->bySpecialization.types;
        while (tpl)
        {
            tpl->p->byClass.dflt = tpl->p->byClass.val;
            tpl->p->byClass.val = NULL;
            tpl = tpl->next;
        }
	    SetLinkerNames(sp, lk_cdecl);
        found1 = sp;
	}
	else
	{
		found1 = NULL;
	}
    restoreParams(&sd, 1);
    return found1;
    */
}
LEXEME *TemplateArgGetDefault(LEXEME **lex, BOOLEAN isExpression)
{
    LEXEME *rv = NULL, **cur = &rv;
    LEXEME *current = *lex, *end = current;
    // this presumes that the template or expression is small enough to be cached...
    // may have to adjust it later
    // have to properly parse the default value, because it may have
    // embedded expressions that use '<'
    if (isExpression)
    {
        TYPE *tp;
        EXPRESSION *exp;
        end = expression_no_comma(current,NULL, NULL, &tp, &exp, NULL, _F_INTEMPLATEPARAMS);
    }
    else
    {
        TYPE *tp;
        end = get_type_id(current, &tp, NULL, sc_cast, FALSE, TRUE);
    }
    while (current != end)
    {
        *cur = Alloc(sizeof(LEXEME));
        **cur = *current;
        (*cur)->next = NULL;
        if (ISID(current))
            (*cur)->value.s.a = litlate((*cur)->value.s.a);
        current = current->next;
        cur = &(*cur)->next;
    }
    *lex = end;
    return rv;
}
static LEXEME *TemplateHeader(LEXEME *lex, SYMBOL *funcsp, TEMPLATEPARAMLIST **args)
{
    TEMPLATEPARAMLIST **lst = args, **begin = args, *search;
    STRUCTSYM *structSyms = NULL;
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
            {
                if (!structSyms)
                {
                    structSyms = Alloc(sizeof(STRUCTSYM));
                    structSyms->tmpl = *args;
                    addTemplateDeclaration(structSyms);
                }
                args = &(*args)->next;
            }
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
    LEXEME *start = lex;
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
                
                lex = nestedPath(lex, &strsym, &nsv, NULL, FALSE, sc_global, FALSE);
                if (strsym)
                {
                    if (strsym->tp->type == bt_templateselector)
                    {
                        TEMPLATESELECTOR *l;
                        l = strsym->templateSelector;
                        while (l->next)
                            l = l->next;
                        sp = sym = makeID(sc_type, strsym->tp, NULL, l->name);
                        lex = getsym();
                        tp = strsym->tp;
                        goto non_type_join;
                    }
                    else if (ISID(lex))
                    {
                        TEMPLATESELECTOR **last;
                        tp = Alloc(sizeof(TYPE));
                        tp->type = bt_templateselector;
                        sp = sym = makeID(sc_type, tp, NULL, lex->value.s.a);
                        tp->sp = sym;
                        last = &sym->templateSelector;
                        *last = Alloc(sizeof(TEMPLATESELECTOR));
                        (*last)->sym = NULL;
                        last = &(*last)->next;
                        *last = Alloc(sizeof(TEMPLATESELECTOR));
                        (*last)->sym = strsym;
                        if (strsym->templateLevel)
                        {
                            (*last)->isTemplate = TRUE;
                            (*last)->templateParams = strsym->templateParams;
                        }
                        last = &(*last)->next;
                        *last = Alloc(sizeof(TEMPLATESELECTOR));
                        (*last)->name = litlate(lex->value.s.a);
                        last = &(*last)->next;
                        lex = getsym();
                        goto non_type_join;
                    }
                    else
                    {
                        lex = getsym();
                        error(ERR_TYPE_NAME_EXPECTED);
                        break;
                    }
                }
                else if (ISID(lex))
                {
                    TYPE *tp = Alloc(sizeof(TYPE));
                    tp->type = bt_templateparam;
                    tp->templateParam = arg;
                    arg->p->sym = makeID(sc_templateparam, tp, NULL, litlate(lex->value.s.a));
                    lex = getsym();
                }
                else
                {
                    lex = getsym();
                    error(ERR_TYPE_NAME_EXPECTED);
                    break;
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
                arg->p->byClass.txtdflt = TemplateArgGetDefault(&lex, FALSE);
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
                arg->p->byTemplate.txtdflt = TemplateArgGetDefault(&lex, FALSE);
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
            if (MATCHKW(lex, ellipse))
            {           
                arg->p->packed = TRUE;
                lex = getsym();
            }
            lex = getBeforeType(lex, funcsp, &tp, &sp, NULL, NULL, FALSE, sc_cast, &linkage, &linkage2, &linkage3, FALSE, FALSE, FALSE, FALSE); /* fixme at file scope init */
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
                    sp = makeID(sc_templateparam, NULL, NULL, AnonymousName());
                }
non_type_join:
                tpa = Alloc(sizeof(TYPE));
                tpa->type = bt_templateparam;
                tpa->templateParam = arg;
                sp->storage_class = sc_templateparam;
                sp->tp = tpa;
                arg->p->type = kw_int;
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
                if (basetype(tp)->type != bt_templateparam && basetype(tp)->type != bt_templateselector && basetype(tp)->type != bt_enum && !isint(tp) && !ispointer(tp) && basetype(tp)->type != bt_lref)
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
                        arg->p->byNonType.txtdflt = TemplateArgGetDefault(&lex, TRUE);
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
        TEMPLATEPARAMLIST *rv = NULL, **last = &rv, *parse, *rv1;
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
        parse = t;
        rv1 = rv;
        while (parse)
        {
            if (parse->p->type == kw_int)
            {
                if (parse->p->byNonType.tp->type == bt_templateparam)
                {
                    TEMPLATEPARAMLIST *t1 = t;
                    TEMPLATEPARAMLIST *rv2 = rv;
                    while (t1)
                    {
                        if (t1->p->type == kw_typename)
                        {
                            if (t1->p == parse->p->byNonType.tp->templateParam->p)
                            {
                                TYPE * old = rv1->p->byNonType.tp;
                                rv1->p->byNonType.tp = (TYPE *)Alloc(sizeof(TYPE));
                                *rv1->p->byNonType.tp = *old;
                                rv1->p->byNonType.tp->templateParam = rv2;
                                break;
                            }
                        }
                        t1 = t1->next;
                        rv2 = rv2->next;
                    }
                }
            }
            parse = parse->next;
            rv1 = rv1->next;
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
static EXPRESSION *copy_expression(EXPRESSION *exp)
{
    EXPRESSION *rv = (EXPRESSION *)Alloc(sizeof(EXPRESSION));
    *rv = *exp;
    if (rv->left)
        rv->left = copy_expression(rv->left);
    if (rv->right)
        rv->right = copy_expression(rv->right);
    return rv;
}
static TEMPLATEPARAMLIST *paramsToDefault(TEMPLATEPARAMLIST *templateParams)
{
    TEMPLATEPARAMLIST *params = NULL, **pt = &params, *find = templateParams;
    while (find)
    {
        *pt = Alloc(sizeof(TEMPLATEPARAMLIST));
        (*pt)->p = Alloc(sizeof(TEMPLATEPARAM));
        *(*pt)->p = *find->p;
        if (params->p->packed)
        {
            params->p->byPack.pack = paramsToDefault(params->p->byPack.pack);
        }
        else
        {
            (*pt)->p->byClass.dflt = find->p->byClass.val;
            (*pt)->p->byClass.val = NULL;
        }
        pt = &(*pt)->next;
        find = find->next;
    }
    return params;
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
            if (!allTemplateArgsSpecified(sp, sp->templateParams->next))
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
                            (*pt)->next = NULL;
                            if (!templateNestingCount || instantiatingTemplate)
                                (*pt)->p->byClass.dflt = SynthesizeType((*pt)->p->byClass.dflt, enclosing, FALSE);
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
            }
            else
            {
                TEMPLATEPARAMLIST *params = paramsToDefault(sp->templateParams->next);
                SYMBOL *sp1 = clonesym(sp);
                sp1->tp = Alloc(sizeof(TYPE));
                *sp1->tp = *sp->tp;
                sp1->tp->sp = sp1;
                sp = sp1;
                sp = GetClassTemplate(sp, params, FALSE);
            }
            if (sp)
                sp = TemplateClassInstantiate(sp, sp->templateParams, FALSE, sc_global);
               
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
static TYPE *LookupTypeFromExpression(EXPRESSION *exp, TEMPLATEPARAMLIST *enclosing, BOOLEAN alt)
{
    EXPRESSION *funcList[100];
    int count = 0;
    switch (exp->type)
    {
        case en_void:
            return NULL;
        case en_not_lvalue:
        case en_lvalue:
        case en_argnopush:
        case en_voidnz:
        case en_shiftby:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_global:
        case en_auto:
        case en_labcon:
        case en_absolute:
        case en_pc:
        case en_const:
        case en_threadlocal:
            return &stdpointer;
        case en_label: 
        case en_x_label:
        case en_l_ref:
            return &stdpointer;
        case en_c_bit:
        case en_c_bool:
        case en_x_bool:
        case en_x_bit:
        case en_l_bool:
        case en_l_bit:
            return &stdbool;
        case en_c_c:
        case en_x_c:
        case en_l_c:
            return &stdchar;
        case en_c_uc:
        case en_x_uc:
        case en_l_uc:
            return &stdunsignedchar;
        case en_c_wc:
        case en_x_wc:
        case en_l_wc:
            return &stdwidechar;
        case en_c_s:
        case en_x_s:
        case en_l_s:
            return &stdshort;
        case en_c_u16:
        case en_x_u16:
        case en_l_u16:
            return &stdchar16t;
        case en_c_us:
        case en_x_us:
        case en_l_us:
            return &stdunsignedshort;
        case en_c_i:
        case en_x_i:
        case en_l_i:
        case en_structelem:
            return &stdint;
        case en_c_ui:
        case en_x_ui:
        case en_l_ui:
            return &stdunsigned;
        case en_c_u32:
        case en_x_u32:
        case en_l_u32:
            return &stdchar32t;
        case en_c_l:
        case en_x_l:
        case en_l_l:
            return &stdlong;
        case en_c_ul:
        case en_x_ul:
        case en_l_ul:
            return &stdunsignedlong;
        case en_c_ll:
        case en_x_ll:
        case en_l_ll:
            return &stdlonglong;
        case en_c_ull:
        case en_x_ull:
        case en_l_ull:
            return &stdunsignedlonglong;
        case en_c_f:
        case en_x_f:
        case en_l_f:
            return &stdfloat;
        case en_c_d:
        case en_x_d:
        case en_l_d:
            return &stddouble;
        case en_c_ld:
        case en_x_ld:
        case en_l_ld:
            return &stdlongdouble;
        case en_c_p:
        case en_x_p:
            return &stdpointer;
        case en_l_p:
        {
            TYPE *tp = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (tp && ispointer(tp))
                tp = basetype(tp)->btp;
            return tp;
        }
                
        case en_c_sp:
        case en_x_sp:
        case en_l_sp:
            return &stdchar16t;
        case en_c_fp:
        case en_x_fp:
        case en_l_fp:
            return &stdpointer; // fixme
        case en_c_fc:
        case en_x_fc:
        case en_l_fc:
            return &stdfloatcomplex;
        case en_c_dc:
        case en_x_dc:
        case en_l_dc:
            return &stddoublecomplex;
        case en_c_ldc:
        case en_x_ldc:
        case en_l_ldc:
            return &stdlongdoublecomplex;
        case en_c_fi: 
        case en_x_fi: 
        case en_l_fi: 
            return &stdfloatimaginary;
        case en_c_di:
        case en_x_di:
        case en_l_di:
            return &stddoubleimaginary;
        case en_c_ldi:
        case en_x_ldi:
        case en_l_ldi:
            return &stdlongdoubleimaginary;

        case en_nullptr:
            return &stdnullpointer;
        case en_memberptr:
            return &stdpointer;
        case en_mp_as_bool:
            return &stdbool;
        case en_mp_compare:
            return &stdbool;
        case en_trapcall:
        case en_intcall:
            return &stdvoid;
        case en_funcret:
            while (exp->type == en_funcret)
            {
                funcList[count++] = exp;
                exp = exp->left;
            }
            /* fall through */
        case en_func:
        {
            TYPE *rv;
            EXPRESSION *exp1= NULL;
            if (basetype(exp->v.func->functp)->type != bt_aggregate)
            {
                rv = basetype(exp->v.func->functp)->btp;
            }
            else
            {
                TYPE *tp1 = NULL;
                SYMBOL *sp;
                TEMPLATEPARAMLIST *tpl = exp->v.func->templateParams;
                while (tpl)
                {
                    tpl->p->byClass.dflt = tpl->p->byClass.val;
                    tpl = tpl->next;
                }
                sp = GetOverloadedFunction(&tp1, &exp1, exp->v.func->sp, exp->v.func, NULL, FALSE, FALSE, FALSE, 0);
                tpl = exp->v.func->templateParams;
                while (tpl)
                {
                    if (tpl->p->type != kw_new)
                        tpl->p->byClass.dflt = NULL;
                    tpl = tpl->next;
                }
                if (sp)
                {
                    rv= basetype(sp->tp)->btp;
                }
                else
                {
                    rv = NULL;
                }
            }
            while (count && rv)
            {
                TYPE *rve = rv;
                exp = funcList[--count];
                while (isref(rve))
                    rve = basetype(rve)->btp;
                if (isfuncptr(rve))
                {
//                    rv = rve;
                    rv = basetype(basetype(rve)->btp)->btp;
                    if (isconst(rve))
                    {
                        // to make LIBCXX happy
                        rve = Alloc(sizeof(TYPE));
                        rve->type = bt_const;
                        rve->size = rv->size;
                        rve->btp = rv;
                        rv = rve;
                    }
                }
                else if (isfunction(rve))
                {
                    rv = basetype(rve)->btp;
                }
                else if (isstructured(rve))
                {
                    /* this needs some work */
                    rv = rve;
                    if (!exp->v.func || !insertOperatorParams(NULL, &rv, &exp1, exp->v.func, 0))
                        rv = &stdint;
                    if (isconst(rve))
                    {
                        // to make LIBCXX happy
                        rve = Alloc(sizeof(TYPE));
                        rve->type = bt_const;
                        rve->size = rv->size;
                        rve->btp = rv;
                        rv = rve;
                    }
                }
                else
                {
                    rv = rve;
                }
            }
            return rv;
        }
        case en_lt:
        case en_le:
        case en_gt:
        case en_ge:
        case en_eq:
        case en_ne:
        case en_land:
        case en_lor:
        case en_ugt:
        case en_uge:
        case en_ule:
        case en_ult:
            return &stdbool;
        case en_uminus:
        case en_not:
        case en_compl:
        case en_ascompl:
        case en_lsh:
        case en_rsh:
        case en_ursh:
        case en_rshd: 
        case en_autoinc:
        case en_autodec:
        case en_bits:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_assign:
        {
            TYPE *tp1 = LookupTypeFromExpression(exp->left, enclosing, alt);
            if (tp1)
            {
                while (isref(tp1))
                    tp1 = basetype(tp1)->btp;
                // there probably ought to be a bunch of such sanity checks throughout this function,
                // this is the minimum to make LIBCXX happy.
                if (isconst(tp1))
                    return NULL;
            }
            return tp1;
        }
        case en_templateparam:
            if (exp->v.sp->tp->templateParam->p->type == kw_typename)
                return exp->v.sp->tp->templateParam->p->byClass.val;
            return NULL;
        case en_templateselector:
        {
            EXPRESSION *exp1 = copy_expression(exp);
            optimize_for_constants(&exp1);
            if (exp1->type != en_templateselector)
                return LookupTypeFromExpression(exp1, enclosing, alt);
            return NULL;
        }
        // the following several work because the front end should have cast both expressions already
        case en_cond:
            return LookupTypeFromExpression(exp->right->left, enclosing, alt);
        case en_arraymul:
        case en_arraylsh:
        case en_arraydiv:
        case en_arrayadd:
        case en_structadd:
        case en_add:    // these are a little buggy because of the 'shortening' optimization
        case en_sub:
        case en_mul:
        case en_mod:
        case en_div:
        case en_and:
        case en_or:
        case en_xor:
        case en_umul: 
        case en_udiv: 
        case en_umod:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        case en_blockclear:
        case en_stackblock:
        case en_blockassign:
            switch(exp->left->type)
            {
                case en_global:
                case en_auto:
                case en_labcon:
                case en_absolute:
                case en_pc:
                case en_const:
                case en_threadlocal:
                    return exp->left->v.sp->tp;
            }
            if (exp->right)
                switch(exp->right->type)
                {
                    case en_global:
                    case en_auto:
                    case en_labcon:
                    case en_absolute:
                    case en_pc:
                    case en_const:
                    case en_threadlocal:
                        return exp->right->v.sp->tp;
                }
            return NULL;            
        case en_thisref:
            return LookupTypeFromExpression(exp->left, enclosing, alt);
        default:
            diag("LookupTypeFromExpression: unknown expression type");
            return NULL;
    }
}
TYPE *TemplateLookupTypeFromDeclType(TYPE *tp)
{
    EXPRESSION *exp = tp->templateDeclType;
    return LookupTypeFromExpression(exp, NULL, FALSE);
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
                if (isarray(tp) && tp->etype)
                {
                    tp->etype = SynthesizeType(tp->etype, enclosing, alt);
                }
                *last = (TYPE *)Alloc(sizeof(TYPE));
                **last = *tp;
                (*last)->btp = SynthesizeType(tp->btp, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);                
                return rv;
            case bt_templatedecltype:
                *last = LookupTypeFromExpression(tp->templateDeclType, enclosing, alt);
                if (!*last)
                    return &stdany;
                *last = SynthesizeType(*last, enclosing, alt);
                SynthesizeQuals(&last, &qual, &lastQual);
                return rv;
            case bt_templateselector:
            {
                SYMBOL *sp;
                SYMBOL *ts = tp->sp->templateSelector->next->sym;
                TEMPLATESELECTOR *find = tp->sp->templateSelector->next->next;
                if (tp->sp->templateSelector->next->isTemplate)
                {
                    TEMPLATEPARAMLIST *current = tp->sp->templateSelector->next->templateParams;
                    TEMPLATEPARAMLIST *symtp = ts->templateParams->next;
                    void *defaults[200];
                    int count = 0;
                    while (current)
                    {
                        if (current->p->packed)
                            current = current->p->byPack.pack;
                        if (current)
                        {
                            defaults[count++] = current->p->byClass.dflt;
                            if (current->p->type == kw_typename && current->p->byClass.dflt)
                            {
                                current->p->byClass.dflt = SynthesizeType(current->p->byClass.dflt, enclosing, alt);
                                if (!current->p->byClass.dflt || current->p->byClass.dflt->type == bt_any)
                                {
                                    int i=0;
                                    current = tp->sp->templateSelector->next->templateParams;
                                    while (count--)
                                    {
                                        if (current->p->packed)
                                            current = current->p->byPack.pack;
                                        if (current)
                                        {
                                            current->p->byClass.dflt = defaults[i++];
                                            current = current->next;
                                        }
                                    }
                                    return &stdany;
                                }
                                
                            }
                            else if (current->p->type == kw_int)
                                if (current->p->byNonType.dflt)
                                {
                                    current->p->byNonType.dflt = copy_expression(current->p->byNonType.dflt);
                                    optimize_for_constants(&current->p->byNonType.dflt);
                                }
                                else if (current->p->byNonType.val)
                                {
                                    current->p->byNonType.dflt = copy_expression(current->p->byNonType.val);
                                    optimize_for_constants(&current->p->byNonType.dflt);
                                }
                            if (symtp)
                            {
                                if (!current->p->sym)
                                    current->p->sym = symtp->p->sym;
                                symtp = symtp->next;
                            }
                            current = current->next;
                        }
                    }
                    current = tp->sp->templateSelector->next->templateParams;
                    sp = GetClassTemplate(ts, current, TRUE);
					if (sp)
						sp = TemplateClassInstantiateInternal(sp, current, FALSE);
                    current = tp->sp->templateSelector->next->templateParams;
                    count = 0;
                    while (current)
                    {
                        if (current->p->packed)
                            current = current->p->byPack.pack;
                        if (current)
                        {
                            current->p->byClass.dflt = defaults[count++];
                            current = current->next;
                        }
                    }
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
                    
                    sp = search(find->name, basetype(tp)->syms);
                    if (!sp)
                    {
                        sp = classdata(find->name, basetype(tp)->sp, NULL, FALSE, FALSE);
                        if (sp == (SYMBOL *)-1)
                            sp = NULL;
                    }
                    if (sp)
                        tp = sp->tp;
                    else
                        break;
                    find = find->next;
                }
                if (!find && tp)
                {
                    while (tp->type == bt_typedef)
                        tp = tp->btp;
                    if (tp->type == bt_templateparam)
                    {
                        *last = tp->templateParam->p->byClass.dflt;
                    }
                    else
                    {
                        *last = tp;
                    }
                    if (rv)
                    {
                        tp = Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->btp = rv;
                        tp->size = rv->size;
                        rv = tp;
                        SynthesizeQuals(&last, &qual, &lastQual);
                    }
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
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
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
                {
                    TYPE *tp1 = tp->sp->tp;
                    if (tp1->type == bt_templateselector)
                        tp1 = tp1->sp->templateSelector->next->sym->tp;
                    if (tp1->type == bt_templateparam)
                    {
                        tp1 = tp1->templateParam->p->byClass.val;
                        (*last)->sp = tp1->sp;
                    }
                }
                last = &(*last)->btp;
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
                                    TYPE *tp1;
                                    clone->tp = SynthesizeType(templateParams->p->byClass.val, enclosing, alt);
                                    tp1 = Alloc(sizeof(TYPE));
                                    tp1->type = bt_derivedfromtemplate;
                                    tp1->btp = clone->tp;
                                    tp1->size = clone->tp->size;
                                    clone->tp = tp1;
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
                        TYPE *tp1;
                        insert(clone, func->syms);
                        clone->tp = SynthesizeType(clone->tp, enclosing, alt);
                        if (clone->tp->type != bt_void && clone->tp->type != bt_any)
                        {
                            tp1 = Alloc(sizeof(TYPE));
                            tp1->type = bt_derivedfromtemplate;
                            tp1->btp = clone->tp;
                            tp1->size = clone->tp->size;
                            clone->tp = tp1;
                        }
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
                    if (!tpa)
                        return rv;
                }
                if (tpa->p->type == kw_typename)
                {
                    TYPE *type = alt ? tpa->p->byClass.temp : tpa->p->byClass.val, *typx = type;
                    while (type && type->type == bt_templateparam)
                    {
                        type = type->templateParam->p->byClass.val;
                        if (type == typx)
                            return &stdany;
                    }
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
                        tp = Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->btp = rv;
                        tp->size = rv->size;
                        rv = tp;
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
                        tp = Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->btp = rv;
                        tp->size = rv->size;
                        rv = tp;
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
                if (alt && isstructured(tp))
                {
                    TEMPLATEPARAMLIST *p = NULL, **pt = &p, *tpl;
                    tp_in = Alloc(sizeof(TYPE));
                    *tp_in = *tp;
                    tp_in->sp = clonesym(tp_in->sp); 
                    tpl = tp_in->sp->templateParams;
                    while (tpl)
                    {
                        *pt = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
                        if (tpl->p->type == kw_typename && tpl->p->byClass.temp)
                        {
                            (*pt)->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
                            *(*pt)->p = *tpl->p;
                            (*pt)->p->byClass.val = SynthesizeType(tpl->p->byClass.temp, enclosing, FALSE);
                        }
                        else
                        {
                            (*pt)->p = tpl->p;
                        }
                        pt = &(*pt)->next;
                        tpl = tpl->next;
                    }
                    tp_in->sp->templateParams = p;
                    tp = tp_in;
                }
                else if (enclosing)
                {
                    tp_in = SynthesizeStructure(tp, enclosing);
                    if (tp_in)
                    {
                        tp = Alloc(sizeof(TYPE));
                        tp->type = bt_derivedfromtemplate;
                        tp->btp = rv;
                        tp->size = rv->size;
                        rv = tp;
                        tp = tp_in;
                    }
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
    STRUCTSYM s, s1;
    if (sym->parentClass)
    {
        s.str = sym->parentClass;
        addStructureDeclaration(&s);
    }
    s1.tmpl = sym->templateParams;
    addTemplateDeclaration(&s1);
    rsv->parentTemplate = sym;
    rsv->mainsym = sym;
    rsv->parentClass = SynthesizeParentClass(rsv->parentClass);
    rsv->tp = SynthesizeType(sym->tp, params, FALSE);
    if (isfunction(rsv->tp))
    {
        basetype(rsv->tp)->sp = rsv;
    }
    rsv->templateParams = params;
    if (sym->parentClass)
    {
        dropStructureDeclaration();
    }
    dropStructureDeclaration();
    return rsv;
}
static TYPE *removeTLQuals(TYPE *A)
{
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
    if (exp->type == en_templateparam || exp->type == en_templateselector)
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
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
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
static void ClearArgValues(TEMPLATEPARAMLIST *params, BOOLEAN specialized)
{
    while (params)
    {
        if (params->p->type != kw_new)
        {
            if (params->p->packed)
                params->p->byPack.pack = NULL;
            else
                params->p->byClass.val = params->p->byClass.temp = NULL;
            if (params->p->byClass.txtdflt && !specialized)
                params->p->byClass.dflt = NULL;
            if (params->p->byClass.dflt)
            {
				if (params->p->type == kw_typename)
				{
					TYPE *tp = params->p->byClass.dflt;
					while (ispointer(tp))
						tp = basetype(tp)->btp;
					if (tp ->type == bt_templateparam)
					{
						TEMPLATEPARAMLIST *t = tp->templateParam;
						t->p->byClass.val = NULL;
					}
                    else if (isstructured(tp) && (basetype(tp)->sp)->linkage != lk_virtual)
                    {
                        ClearArgValues(basetype(tp)->sp->templateParams, specialized);
                    }
				}
				else
				{
					params->p->byClass.val = NULL;
				}
            }
        }
        params = params->next;
    }
}
static BOOLEAN Deduce(TYPE *P, TYPE *A, BOOLEAN change, BOOLEAN byClass, BOOLEAN allowSelectors);
static BOOLEAN DeduceFromTemplates(TYPE *P, TYPE *A, BOOLEAN change, BOOLEAN byClass)
{
    TYPE *pP = basetype(P);
    TYPE *pA = basetype(A);
    if (pP->sp && pA->sp && pP->sp->parentTemplate == pA->sp->parentTemplate)
    {
        TEMPLATEPARAMLIST *TP = pP->sp->templateParams;
        TEMPLATEPARAMLIST *TA = pA->sp->templateParams;
        TEMPLATEPARAMLIST *TAo = TA;
        TEMPLATEPARAMLIST * isspecialized = TP->p->bySpecialization.types ? TP->next : NULL;
        if (!TA || !TP)
            return FALSE;
        if (TA->p->bySpecialization.types)
            TA = TA->p->bySpecialization.types;
        else
            TA = TA->next;
        if (TP->p->bySpecialization.types)
            TP = TP->p->bySpecialization.types;
        else
            TP = TP->next;
        while (TP && TA)
        {
            TEMPLATEPARAM *to = TP->p;
            if (TA->p->packed)
                TA = TA->p->byPack.pack;
            if (!TA)
                break;
            if (TP->p->type != TA->p->type)
                return FALSE;
            if (TP->p->packed)
            {
                break;
            }
            if (isspecialized && TP->p->sym)
            {
                TEMPLATEPARAMLIST *search = isspecialized;
                while (search)
                {
                    if (search->p->sym && !strcmp(search->p->sym->name, TP->p->sym->name))
                        break;
                    search = search->next;
                }
                if (search)
                    to = search->p;
            }
            switch (TP->p->type)
            {
                case kw_typename:
                {
                    TYPE **tp = change ? &to->byClass.val : &to->byClass.temp;
                    if (*tp)
                    {
                        if (!templatecomparetypes(*tp, TA->p->byClass.val, TRUE))
                            return FALSE;
                    }
                    else
                        *tp = TA->p->byClass.val;
                    if (to->byClass.dflt && TA->p->byClass.dflt && 
                        !Deduce(to->byClass.dflt, TA->p->byClass.dflt, change, byClass, FALSE))
                        return FALSE;
                    break;
                }
                case kw_template:
                {
                    TEMPLATEPARAMLIST *paramT = to->sym->templateParams;
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
                    if (!to->byTemplate.val)
                        to->byTemplate.val = TA->p->byTemplate.val;
                    else if (!DeduceFromTemplates(to->byTemplate.val->tp, TA->p->byTemplate.val->tp, change, byClass))
                        return FALSE;
                    break;
                }
                case kw_int:
                {
                    EXPRESSION **exp;
                    if (TAo->p->bySpecialization.types)
                    {
//    #ifndef PARSER_ONLY
//                        if (TA->p->byNonType.dflt && !equalTemplateIntNode(TA->p->byNonType.dflt, TA->p->byNonType.val))
//                            return FALSE;
//    #endif
                    }
                    exp = change ? &to->byNonType.val : &to->byNonType.temp;
                    if (!*exp)
                        *exp = TA->p->byNonType.val;
                    break;
                }
                default:
                    break;
            }
            TP = TP->next;
            TA = TA->next;
        }
        if (TP && TP->p->packed)
        {
            if (isspecialized && TP->p->sym)
            {
                TEMPLATEPARAMLIST *search = isspecialized;
                while (search)
                {
                    if (search->p->sym && !strcmp(search->p->sym->name, TP->p->sym->name))
                        break;
                    search = search->next;
                }
                if (search)
                    TP = search;
            }
            if (TP->p->byPack.pack)
            {
                TP = TP->p->byPack.pack;
                while (TP && TA)
                {
                    TEMPLATEPARAM *to = TP->p;
                    if (TP->p->type != TA->p->type)
                    {
                        return FALSE;
                    }
                    if (TA->p->packed)
                        TA = TA->p->byPack.pack;
                    if (TA)
                    {
                        switch (TP->p->type)
                        {
                            case kw_typename:
                            {
                                TYPE **tp = change ? &TP->p->byClass.val : &TP->p->byClass.temp;
                                if (*tp)
                                {
                                    if (!templatecomparetypes(*tp, TA->p->byClass.val, TRUE))
                                    {
                                        return FALSE;
                                    }
                                }
                                else
                                {
                                    *tp = TA->p->byClass.val;
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
                                    {
                                        return FALSE;
                                    }
                                        
                                    paramT = paramT->next;
                                    paramA = paramA->next;
                                }
                                if (paramT || paramA)
                                {
                                    return FALSE;
                                }
                                if (!DeduceFromTemplates(TP->p->byTemplate.val->tp, TA->p->byTemplate.val->tp, change, byClass))
                                {
                                    return FALSE;
                                }
                                break;
                            }
                            case kw_int:
                            {
                                EXPRESSION **exp;
                                if (TAo->p->bySpecialization.types)
                                {
//                #ifndef PARSER_ONLY
//                                    if (TA->p->byNonType.dflt && !equalTemplateIntNode(TA->p->byNonType.dflt, TA->p->byNonType.val))
//                                        return FALSE;
//                #endif
                                }
                                break;
                            }
                            default:
                                break;
                        }
                        TP = TP->next;
                        TA = TA->next;
                    }
                }
            }
            else
            {
                TEMPLATEPARAMLIST **newList = &TP->p->byPack.pack;
                while (TA)
                {
                    if (TP->p->type != TA->p->type)
                    {
                        return FALSE;
                    }
                    if (TA->p->packed)
                        TA = TA->p->byPack.pack;
                    if (TA)
                    {
                        *newList = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
                        (*newList)->p = TA->p;
                        newList = &(*newList)->next;
                        TA = TA->next;
                    }
                }
                TP = NULL;
            }
        }
        return (!TP && !TA);
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
static BOOLEAN DeduceFromMemberPointer(TYPE *P, TYPE *A, BOOLEAN change, BOOLEAN byClass)
{
    TYPE *Pb = basetype(P);
    TYPE *Ab = basetype(A);
    if (Ab->type == bt_memberptr)
    {
        if (Pb->sp->tp->type != bt_templateselector ||
             !Deduce(Pb->sp->templateSelector->next->sym->tp, Ab->sp->tp, change, byClass, FALSE))
            return FALSE;
        if (!Deduce(Pb->btp, Ab->btp, change, byClass, FALSE))
            return FALSE;
        return TRUE;
    }
    else // should only get here for functions
    {
         if (!isfunction(Ab))
             return FALSE;
         if (basetype(Ab)->sp->parentClass == NULL || Pb->sp->tp->type != bt_templateselector ||
             !Deduce(Pb->sp->templateSelector->next->sym->tp, basetype(Ab)->sp->parentClass->tp, change, byClass, FALSE))
                 return FALSE;
         if (!Deduce(Pb->btp, Ab, change, byClass, FALSE))
             return FALSE;
         return TRUE;
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
            if (/*!Pt->p->initialized &&*/ !templatecomparetypes(*tp, A, TRUE))
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
static BOOLEAN Deduce(TYPE *P, TYPE *A, BOOLEAN change, BOOLEAN byClass, BOOLEAN allowSelectors)
{
    BOOLEAN constant = FALSE;
    if (!P || !A)
        return FALSE;
    while (1)
    {
        TYPE *Ab = basetype(A);
        TYPE *Pb = basetype(P);
        if (isstructured(Pb) && Pb->sp->templateLevel && Pb->sp->linkage != lk_virtual && isstructured(Ab))
            if (DeduceFromTemplates(P, A, change, byClass))
                return TRUE;
            else
                return DeduceFromBaseTemplates(P, basetype(A)->sp, change, byClass);
        if (Pb->type == bt_memberptr)
            return DeduceFromMemberPointer(P, A, change, byClass); 
        if (Pb->type == bt_enum)
        {
            if (Ab->type == bt_enum && Ab->sp == Pb->sp)
                return TRUE;
            if (isint(Ab))// && Ab->enumConst)
                return TRUE;
            return FALSE;
        }
        if (Ab->type != Pb->type && (!isfunction(Ab) || !isfunction(Pb)) && Pb->type != bt_templateparam && (!allowSelectors || Pb->type != bt_templateselector))
            return FALSE;
        switch(Pb->type)
        {
            case bt_pointer:
                if (isarray(Pb))
                {
                    if (!!basetype(Pb)->esize != !!basetype(Ab)->esize)
                        return FALSE;
                    if (basetype(Pb)->esize && basetype(Pb)->esize->type == en_templateparam)
                    {
                        SYMBOL *sym = basetype(Pb)->esize->v.sp;
                        if (sym ->tp->type == bt_templateparam)
                        {
                            sym->tp->templateParam->p->byNonType.val = basetype(Ab)->esize;
                        }
                    }
                }
                if (isarray(Pb) != isarray(Ab))
                    return FALSE;
                P = Pb->btp;
                A = Ab->btp;
                break;
            case bt_templateselector:
                return TRUE;
            case bt_templatedecltype:
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
            case bt_func:
            case bt_ifunc:
            {
                HASHREC *hrp = Pb->syms->table[0];
                HASHREC *hra = Ab->syms->table[0];
                if (((SYMBOL *)hrp->p)->thisPtr)
                    hrp = hrp->next;
                if (((SYMBOL *)hra->p)->thisPtr)
                    hra = hra->next;
                clearoutDeduction(P);
                if (!Deduce(Pb->btp, Ab->btp, FALSE, byClass, allowSelectors))
                    return FALSE;
                
                while (hra && hrp)
                {
                    SYMBOL *sp = (SYMBOL *)hrp->p;
                    if (!Deduce(sp->tp, ((SYMBOL *)hra->p)->tp, FALSE, byClass, allowSelectors))
                        return FALSE;
                    if (sp->tp->type == bt_templateparam)
                    {
                        if (sp->tp->templateParam->p->packed)
                        {
                            hrp = NULL;
                            hra = NULL;
                            break;
                        }
                    }
                    hrp = hrp->next;
                    hra = hra->next;
                }
                if (hra)
                    return FALSE;
                if (hrp && !((SYMBOL *)hrp->p)->init)
                    return FALSE;
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
    if (exp->type == en_templateselector)
        return FALSE;
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
                        if (!tp)
                            return FALSE;
                    }
                    if (tp->type == bt_void || isfunction(tp) || isref(tp) || (isstructured(tp) && basetype(tp)->sp->isabstract))
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
                        return ValidArg(tp);
                    }
                    else if (tp->type == bt_templateselector)
                    {
                        return ValidArg(tp);
                    }
                    if (isref(tp))
                        return FALSE;
                }
                return TRUE;
            case bt_templatedecltype:
                tp = TemplateLookupTypeFromDeclType(tp);
                return !!tp;
                break;
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
                else if (basetype(ts->tp)->templateParam->p->type == kw_typename)
                {
                    tp = basetype(ts->tp)->templateParam->p->byClass.val;
                    if (!tp)
                        return FALSE;
                    sp = tp->sp;
                }
                else if (basetype(ts->tp)->templateParam->p->type == kw_delete)
                {
                    TEMPLATEPARAMLIST *args = basetype(ts->tp)->templateParam->p->byDeferred.args;
                    TEMPLATEPARAMLIST *val = NULL, **lst = &val;
                    sp = tp->templateParam->p->sym;
                    sp = TemplateClassInstantiateInternal(sp, args, TRUE);
                }
                if (sp)
                {
                    sp = basetype(PerformDeferredInitialization (sp->tp, NULL))->sp;
                    while (find && sp)
                    {
                        SYMBOL *spo = sp;
                        if (!isstructured(spo->tp))
                            break;
                        
                        sp = search(find->name, spo->tp->syms);
                        if (!sp)
                        {
                            sp = classdata(find->name, spo, NULL, FALSE, FALSE);
                            if (sp == (SYMBOL *)-1)
                                sp = NULL;
                        }
                        find = find->next;
                    }
                    return !find && sp && istype(sp);
                }
                return FALSE;                
            }
            case bt_lref:
            case bt_rref:
                tp = basetype(tp)->btp;
                if (tp->type == bt_templateparam)
                {
                    if (tp->templateParam->p->type != kw_typename)
                        return FALSE;
                    return ValidArg(tp);
                }
                if (!tp || isref(tp))
                    return FALSE;
                break;
            case bt_memberptr:
                {
                    TYPE *tp1 = tp->sp->tp;
                    if (tp1->type == bt_templateselector)
                        tp1 = tp1->sp->templateSelector->next->sym->tp;
                    if (tp1->type == bt_templateparam)
                    {
                        if (tp1->templateParam->p->type != kw_typename)
                            return FALSE;
                        tp1 = tp1->templateParam->p->byClass.val;
                        if (!tp1)
                            return FALSE;
                    }
                    if (!isstructured(tp1))
                        return FALSE;
                }
                tp = tp->btp;
                break;
            case bt_const:
            case bt_volatile:
            case bt_restrict:
            case bt_far:
            case bt_near:
            case bt_seg:
            case bt_lrqual:
            case bt_rrqual:
            case bt_derivedfromtemplate:
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
                    if (!tp)
                        return FALSE;
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
static BOOLEAN checkNonTypeTypes(TEMPLATEPARAMLIST *params, TEMPLATEPARAMLIST *enclosing)
{
    while (params)
    {
        if (params->p->type == kw_int && (params->p->byNonType.tp->type == bt_templateselector || params->p->byNonType.tp->type == bt_templateparam))
        {
            TYPE *tp1 = SynthesizeType(params->p->byNonType.tp, enclosing, FALSE);
            if (!tp1 || tp1->type == bt_any || isstructured(tp1) || isref(tp1) || isfloat(tp1) || iscomplex(tp1) || isimaginary(tp1))
                return FALSE;
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
    STRUCTSYM s,s1;
    inDefaultParam++;
    if (!valFromDefault(params, usesParams, &args))
    {
        inDefaultParam--;
        return FALSE;
    }
    if (!checkNonTypeTypes(params, params))
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
        params = params->next;
    }
    if (hr && ((SYMBOL *)hr->p)->tp->type == bt_void)
    {
        inDefaultParam--;
        return TRUE;
    }
    if (hr)
    {
        BOOLEAN packedOrEllipse = FALSE;
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
            SYMBOL *sp1 = (SYMBOL *)(hr->p);
            if (basetype(sp1->tp)->type == bt_ellipse || sp1->packed)
                packedOrEllipse = TRUE;  
            args = args->next;
            hr = hr->next;
        }
        if (args && !packedOrEllipse)
        {
            dropStructureDeclaration();
            if (func->parentClass)
                dropStructureDeclaration();
            inDefaultParam--;
            return FALSE;
        }
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->deferredCompile)
            {
                LEXEME *lex;
    			dontRegisterTemplate+= templateNestingCount != 0;
                lex = SetAlternateLex(sp->deferredCompile);
                sp->init = NULL;
                lex = initialize(lex, func, sp, sc_parameter, TRUE, _F_TEMPLATEARGEXPANSION);
                SetAlternateLex(NULL);
    			dontRegisterTemplate-= templateNestingCount != 0;
                if (sp->init && sp->init->exp && !ValidExp(&sp->init->exp))
                {
                    dropStructureDeclaration();
                    if (func->parentClass)
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
    s.tmpl = func->templateParams;
    addTemplateDeclaration(&s);
//    if (!ValidArg(basetype(func->tp)->btp))
//        return FALSE;
    hr = basetype(func->tp)->syms->table[0];
    while (hr)// && (!usesParams || check))
    {
        if (!ValidArg(((SYMBOL *)hr->p)->tp))
        {
            dropStructureDeclaration();
            inDefaultParam--;
            return FALSE;
        }
        if (check)
            check = check->next;
        hr = hr->next;
    }
    if (!ValidArg(basetype(func->tp)->btp))
    {
        dropStructureDeclaration();
        inDefaultParam--;
        return FALSE;
    }
    dropStructureDeclaration();
    inDefaultParam--;
    return TRUE;
}
static BOOLEAN TemplateDeduceFromArg(TYPE *orig, TYPE *sym, EXPRESSION *exp, BOOLEAN byClass, BOOLEAN allowSelectors)
{
    TYPE *P=orig, *A=sym;
    if (!isref(P))
    {
        A = rewriteNonRef(A);
    }
    P = removeTLQuals(P);
    if (isref(P))
    {
        P = basetype(P)->btp;
        if (isref(A))
            A = basetype(A)->btp;
    }
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
    if (Deduce(P, A, TRUE, byClass, allowSelectors))
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
                    if (Deduce(P->btp, sym->tp, FALSE, byClass, allowSelectors))
                    {
                        if (candidate)
                            return FALSE;
                        else
                            candidate = sym;
                    }
                    hr = hr->next;
                }
                if (candidate)
                    return Deduce(P, candidate->tp, TRUE, byClass, allowSelectors);
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
static BOOLEAN TemplateDeduceArgList(HASHREC *funcArgs, HASHREC *templateArgs, INITLIST *symArgs, BOOLEAN allowSelectors)
{
    BOOLEAN rv = TRUE;
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
                    if (!TemplateDeduceFromArg(params->p->byClass.val, symArgs->tp, symArgs->exp, FALSE, allowSelectors))
                    {
                        rv = FALSE;
                    }
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
                rv &= TemplateDeduceArgList(NULL, b, a, allowSelectors);
            }
            symArgs = symArgs->next;
            if (funcArgs)
                funcArgs = funcArgs->next;
        }
        else
        {
            if (!TemplateDeduceFromArg(sp->tp, symArgs->tp, symArgs->exp, FALSE, allowSelectors))
                rv = FALSE;
            symArgs = symArgs->next;
            if (funcArgs)
                funcArgs = funcArgs->next;
        }
        templateArgs = templateArgs->next;
    }
    return rv && !symArgs;
}
static void SwapDefaultNames(TEMPLATEPARAMLIST *params, LIST *origNames)
{
    while (params && origNames)
    {
        char *temp = ((SYMBOL *)origNames->data)->name;
        if (params->p->sym)
        {
            ((SYMBOL *)origNames->data)->name = (void *)params->p->sym->name;
            params->p->sym->name = temp;
        }
        else if (params->p->sym == (SYMBOL *)origNames->data)
        {
            params->p->sym = NULL;
        }
        else
        {
            params->p->sym = (SYMBOL *)origNames->data;
        }
        params = params->next;
        origNames = origNames->next;
    }
}
static BOOLEAN TemplateParseDefaultArgs(SYMBOL *declareSym, 
                                        TEMPLATEPARAMLIST *dest, 
                                        TEMPLATEPARAMLIST *src, 
                                        TEMPLATEPARAMLIST *enclosing)
{
	LIST *oldOpenStructs = openStructs;
	int oldStructLevel = structLevel;
    STRUCTSYM s;
    LEXEME *head;
    LEXEME *tail;
    SYMBOL *oldMemberClass = instantiatingMemberFuncClass;
    instantiatingMemberFuncClass = declareSym->parentClass;
    if (currents)
    {
        head = currents->bodyHead;
        tail = currents->bodyTail;
    }
    s.tmpl = enclosing;
    addTemplateDeclaration(&s);
    while (src && dest)
    {
        if (!dest->p->byClass.val && !dest->p->packed)
        {
            LEXEME *lex;
            int n, pushCount;
            if (!src->p->byClass.txtdflt)
            {
                dropStructureDeclaration();
                instantiatingMemberFuncClass = oldMemberClass;
                return FALSE;
            }
            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
            n = PushTemplateNamespace(declareSym);
            pushCount = pushContext(declareSym, FALSE);
            dest->p->byClass.txtdflt = src->p->byClass.txtdflt;
            dest->p->byClass.txtargs = src->p->byClass.txtargs;
            lex = SetAlternateLex(src->p->byClass.txtdflt);
            switch(dest->p->type)
            {
                case kw_typename:
                {
                    lex = get_type_id(lex, &dest->p->byClass.val, NULL, sc_cast, FALSE, TRUE);
                    if (!dest->p->byClass.val || dest->p->byClass.val->type == bt_any)    
                    {
                        SwapDefaultNames(enclosing, src->p->byClass.txtargs);
                        while (pushCount--)
                            dropStructureDeclaration();
                        PopTemplateNamespace(n);
                        SetAlternateLex(NULL);
                        dropStructureDeclaration();
                        instantiatingMemberFuncClass = oldMemberClass;
                        return FALSE;
                    }
                    break;
                }
                case kw_template:
                    lex = id_expression(lex, NULL, &dest->p->byTemplate.val, NULL, NULL, NULL, FALSE, FALSE, lex->value.s.a);
                    if (!dest->p->byTemplate.val)
                    {
                        SwapDefaultNames(enclosing, src->p->byClass.txtargs);
                        while (pushCount--)
                            dropStructureDeclaration();
                        PopTemplateNamespace(n);
                        SetAlternateLex(NULL);
                        dropStructureDeclaration();
                        instantiatingMemberFuncClass = oldMemberClass;
                        return FALSE;
                    }
                    break;
                case kw_int:
                {
                    TYPE *tp1;
                    EXPRESSION *exp1;
                	openStructs = NULL;
                	structLevel = 0;
                    lex = expression_no_comma(lex, NULL, NULL, &tp1, &exp1, NULL, _F_INTEMPLATEPARAMS);
					openStructs = oldOpenStructs;
					structLevel = oldStructLevel;
                    dest->p->byNonType.val = exp1;
                    if (!templatecomparetypes(dest->p->byNonType.tp, tp1, TRUE))
                    {
                        if (!ispointer(tp1) && !isint(tp1) && !isconstzero(tp1, exp1))
                        {
                            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
                            while (pushCount--)
                                dropStructureDeclaration();
                            PopTemplateNamespace(n);
                            SetAlternateLex(NULL);
                            dropStructureDeclaration();
                            instantiatingMemberFuncClass = oldMemberClass;
                            return FALSE;
                        }
                    }
                }
                    break;
                default:
                    break;
            }
            SwapDefaultNames(enclosing, src->p->byClass.txtargs);
            while (pushCount--)
                dropStructureDeclaration();
            PopTemplateNamespace(n);
            SetAlternateLex(NULL);
        }
        src = src->next;
        dest = dest->next;
    }
    if (currents)
    {
        currents->bodyHead = head;
        currents->bodyTail = tail;
    }
    dropStructureDeclaration();
    instantiatingMemberFuncClass = oldMemberClass;
    return TRUE;
}
static void TransferClassTemplates(TEMPLATEPARAMLIST *dflt, TEMPLATEPARAMLIST *val, TEMPLATEPARAMLIST *params);
SYMBOL *TemplateDeduceArgsFromArgs(SYMBOL *sym, FUNCTIONCALL *args)
{
    TEMPLATEPARAMLIST *nparams = sym->templateParams;
    TYPE *thistp = args->thistp;
    INITLIST *arguments = args->arguments;
    SYMBOL *rv;
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
        ClearArgValues(params, sym->specialized);
        while (templateArgs)
        {
            TYPE *tp = ((SYMBOL *)templateArgs->p)->tp;
            while (isref(tp) || ispointer(tp))
                tp = basetype(tp)->btp;
            if (isstructured(tp) && basetype(tp)->sp->templateParams &&!basetype(tp)->sp->instantiated)
                ClearArgValues(basetype(tp)->sp->templateParams, basetype(tp)->sp->specialized);
            templateArgs = templateArgs->next;
        }
        templateArgs = basetype(sym->tp)->syms->table[0];
        // fill in params that have been initialized in the arg list
        while (initial && params)
        {
            if (initial->p->type != params->p->type)
                return NULL;
            params->p->initialized = TRUE;
			if (initial->p->packed)
				initial = initial->p->byPack.pack;
			if (!initial)
				break;
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
        if (temp)
        {
            // we have to gather the args list
            params = nparams->next;
            while (templateArgs && symArgs )
            {
                SYMBOL *sp = (SYMBOL *)templateArgs->p;
                if (sp->packed)
                    break;
                if (!params || !params->p->byClass.dflt)
                {
                    if (TemplateDeduceFromArg(sp->tp, symArgs->tp, symArgs->exp, FALSE, FALSE))
                    {
                        if (isstructured(sp->tp) && basetype(sp->tp)->sp->templateParams)
                        {
                            TEMPLATEPARAMLIST *params = basetype(sp->tp)->sp->templateParams;
                            TEMPLATEPARAMLIST *special = params->p->bySpecialization.types ? params->p->bySpecialization.types : params->next;
                           TransferClassTemplates(special, special, sym->templateParams->next);
                        }
                    }
                    symArgs = symArgs->next;
                }
                templateArgs = templateArgs->next;
                if (params)
                    params = params->next;
            }
            if (templateArgs)
            {
                SYMBOL *sp = (SYMBOL *)templateArgs->p;
                TYPE *tp = sp->tp;
                TEMPLATEPARAMLIST *base;
                if (isref(tp))
                    tp = basetype(tp)->btp;
                base = basetype(tp)->templateParam;
                if (base && base->p->type == kw_typename)
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
            BOOLEAN rv = TemplateDeduceArgList(basetype(sym->tp)->syms->table[0], templateArgs, symArgs, FALSE);
            if (!rv)
            {
                params = nparams->next;
                while (params)
                {
                    if (params->p->packed && !params->p->byPack.pack)
                        return NULL;
                    params = params->next;
                }
            }
//            else
            {
                HASHREC *hr = basetype(sym->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    TYPE *tp = sp->tp;
                    while (isref(tp) || ispointer(tp))
                        tp = basetype(tp)->btp;
                    if (isstructured(tp) && basetype(tp)->sp->templateParams)
                    {
                        TEMPLATEPARAMLIST *params = basetype(tp)->sp->templateParams;
                        TEMPLATEPARAMLIST *special = params->p->bySpecialization.types ? params->p->bySpecialization.types : params->next;
                        if (special)
                            TransferClassTemplates(special, special, sym->templateParams->next);
                    }
                    hr = hr->next;                    
                }
            }
        }
        // set up default values for non-deduced and non-initialized args
        params = nparams->next;
        if (TemplateParseDefaultArgs(sym, params, params, params) && 
            ValidateArgsSpecified(sym->templateParams->next, sym, arguments))
        {
            rv = SynthesizeResult(sym, nparams);
            rv->maintemplate = sym;
            return rv;
        }
        return NULL;
    }
    rv = SynthesizeResult(sym, nparams);
    rv->maintemplate = sym;
    return rv;
}
static BOOLEAN TemplateDeduceFromType(TYPE* P, TYPE *A)
{
    return Deduce(P, A, TRUE, FALSE, FALSE);
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
    ClearArgValues(nparams, sym->specialized);
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
            TYPE *tp = sp->tp;
            TEMPLATEPARAMLIST *base;
            if (isref(tp))
                tp = basetype(tp)->btp;
            base = tp->templateParam;
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
        TemplateDeduceFromType(basetype(sym->tp)->btp, basetype(tp)->btp);
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
    if (!Deduce(P, A, TRUE, byClass, FALSE))
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
            else
                if (n)
                    return -1;
                if (m)
                    return 1;
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
    ClearArgValues(syml->templateParams, syml->specialized);
    ClearArgValues(symr->templateParams, symr->specialized);
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
                            if (basetype(torig)->array != basetype(tsym)->array)
                                return FALSE;
                            if (basetype(torig)->array && !!basetype(torig)->esize != !!basetype(tsym)->esize)
                                return FALSE;
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
//                        if (tsym->type == bt_templateparam)
//                            tsym = tsym->templateParam->p->byClass.val;
                        if (basetype(torig)->array != basetype(tsym)->array)
                            return FALSE;
                        if (basetype(torig)->array && !!basetype(torig)->esize != !!basetype(tsym)->esize)
                            return FALSE;
                        if ((!templatecomparetypes(torig, tsym, TRUE) || !templatecomparetypes(tsym, torig, TRUE)) && !sameTemplate(torig, tsym))
                            return FALSE;
                        if (isref(torig))
                            torig = basetype(torig)->btp;
                        if (isref(tsym))
                            tsym = basetype(tsym)->btp;
                        if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
                            return FALSE;
                        if (basetype(tsym)->type == bt_enum || basetype(tsym)->enumConst)
                        {
                            if (basetype(torig)->sp != basetype(tsym)->sp)
                                return FALSE;
                        }
                    }
                    break;
                }
                case kw_template:
                    if (xorig != xsym)
                        return FALSE;
                    break;
                case kw_int:
                    if (porig->p->packed != psym->p->packed)
                        return FALSE;
                    if (porig->p->packed)
                    {
                        TEMPLATEPARAMLIST *packorig = porig->p->byPack.pack;
                        TEMPLATEPARAMLIST *packsym = psym->p->byPack.pack;
                        while (packorig && packsym)
                        {
                            EXPRESSION *torig = (EXPRESSION *)packorig->p->byClass.val;
                            EXPRESSION *tsym =  (EXPRESSION *)packsym->p->byClass.val;
                            if (!templatecomparetypes(packorig->p->byNonType.tp, packsym->p->byNonType.tp, TRUE))
                                return FALSE;
//#ifndef PARSER_ONLY
                            if (tsym && !equalTemplateIntNode((EXPRESSION *)torig, (EXPRESSION *)tsym))
                                return FALSE;
//#endif
                            packorig = packorig->next;
                            packsym = packsym->next;
                        }
                        if (packorig || packsym)
                            return FALSE;
                    }
                    else
                    {
                        if (!templatecomparetypes(porig->p->byNonType.tp, psym->p->byNonType.tp, TRUE))
                            return FALSE;
//#ifndef PARSER_ONLY
                        if (xsym && !equalTemplateIntNode((EXPRESSION *)xorig, (EXPRESSION *)xsym))
                            return FALSE;
//#endif
                    }
                    break;
                default:
                    break;
            }
            porig = porig->next;
            psym = psym->next;
        }
        if (psym && psym->p->packed && !psym->p->byPack.pack)
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
//        if (!TemplateInstantiationMatchInternal(orig->templateParams, sym->templateParams))
//            return FALSE;
        return TRUE;
    }
    return FALSE;
}
static void TemplateTransferClassDeferred(SYMBOL *newCls, SYMBOL *tmpl)
{
    SYMBOL *sym = newCls;
    if (newCls->tp->syms && (!newCls->templateParams || !newCls->templateParams->p->bySpecialization.types))
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
                            if (ts2->deferredCompile && !ss2->deferredCompile)
                            {
                                HASHREC *ssf = basetype(ss2->tp)->syms->table[0];
                                while (tsf && ssf)
                                {
                                    ssf->p->name = tsf->p->name;
                                    tsf = tsf->next;
                                    ssf = ssf->next;
                                }
                                ss2->deferredCompile = ts2->deferredCompile;
                            }
                            ss2->maintemplate = ts2;
                            if (!ss2->instantiatedInlineInClass)
                            {
                                if (tmpl->parentTemplate)
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
        ns = newCls->tp->tags->table[0]->next;
        os = tmpl->tp->tags->table[0]->next;
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
                if (isstructured(ss->tp))
                {
                    if (!ss->deferredCompile)
                    {
                        ss->deferredCompile = ts->deferredCompile;
                        PerformDeferredInitialization(ss->tp, NULL);
                    }
                    TemplateTransferClassDeferred(ss, ts);
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
            if (args->p->type != kw_typename || args->p->byClass.dflt->type != bt_templateselector && args->p->byClass.dflt->type != bt_templatedecltype)
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
//        if (!instantiatingTemplate && !isExpressionAccessible(theCurrentFunc ? theCurrentFunc->parentClass : NULL, sym, theCurrentFunc, NULL, FALSE))
//            errorsym(ERR_CANNOT_ACCESS, sym);

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
//#ifndef PARSER_ONLY
                if (exp)
                    exp = GetSymRef(exp);
                if (exp)
                {
                    SetTemplateArgAccess(exp->v.sp, accessible);
                }
//#endif
                break;
            }
            case kw_template:
            {
                TEMPLATEPARAMLIST *tpl = args->p->byTemplate.args;
                while (tpl)
                {
                    if (!allTemplateArgsSpecified(NULL, tpl))
                        return;
                    tpl = tpl->next;
                }
                if (args->p->byTemplate.val)
                    SetTemplateArgAccess(args->p->byTemplate.val, accessible);
            }
                break;
            case kw_typename:
                if (args->p->byClass.val)
                {
                    if (isstructured(args->p->byClass.val))
                    {
                        SetTemplateArgAccess(basetype(args->p->byClass.val)->sp, accessible);
                    }
                    else if (basetype(args->p->byClass.val) == bt_enum)
                    {
                        SetTemplateArgAccess(basetype(args->p->byClass.val)->sp, accessible);
                    }
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
    if (packIndex == -1 && sym->maintemplate)
    {
        TEMPLATEPARAMLIST *tl, *ts;
        tl = sym->templateParams;
        ts = sym->maintemplate->templateParams;
        while (tl && ts)
        {
            if (tl->p->packed && !ts->p->packed)
                return cls;
            tl = tl->next;
            ts = ts->next;
        }
    }

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
        if (!lex && sym->parentTemplate && (!sym->specialized || sym->parentTemplate->specialized))
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
            BOOLEAN oldTemplateType = inTemplateType;
            LAMBDA *oldLambdas = lambdas;
            int oldPackIndex = packIndex;
            int oldRegisterTemplate = dontRegisterTemplate;
            SetAccessibleTemplateArgs(cls->templateParams, TRUE);
            packIndex = -1;
            deferred = NULL;
			templateHeaderCount = 0;
            lambdas = NULL;
            old = *cls;
#ifdef PARSER_ONLY
            cls->parserSet = FALSE;
#endif
            cls->linkage = lk_virtual;
            cls->parentClass = SynthesizeParentClass(cls->parentClass);
            if (cls->parentTemplate)
            {
                TEMPLATEPARAMLIST *left = cls->templateParams;
                TEMPLATEPARAMLIST *right = cls->parentTemplate->templateParams;
                left = left->next;
                right = right->next;
                while (left && right)
                {
                    if (!left->p->sym)
                       left->p->sym = right->p->sym;
                    left = left->next;
                    right = right->next;
                }
            }
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
			dontRegisterTemplate = templateNestingCount != 0;
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
			dontRegisterTemplate = oldRegisterTemplate;
            packIndex = oldPackIndex;
            lambdas = oldLambdas;
            instantiatingTemplate --;
            inTemplateType = oldTemplateType;
            deferred = oldDeferred;
            cls->instantiated = TRUE;
            GENREF(cls);
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
    SYMBOL *orig = sym;
    STRUCTSYM *old;
    HASHREC *hr;
    TEMPLATEPARAMLIST *params = sym->templateParams;
    LEXEME *lex;
    SYMBOL *push;
    int pushCount ;
    BOOLEAN found = FALSE;
    STRUCTSYM s;
    LAMBDA *oldLambdas;

    hr = sym->overloadName->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *data = (SYMBOL *)hr->p;
        if (data->instantiated && TemplateInstantiationMatch(data, sym) && matchOverload(sym->tp, data->tp, TRUE))
        {
            sym = data;
            if (sym->linkage == lk_virtual || isExtern)
                return sym;
            found = TRUE;
            break;
        }
        hr = hr->next;
    }
    oldLambdas = lambdas;
    lambdas = NULL;
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
#ifdef PARSER_ONLY
    sym->parserSet = FALSE;
#endif
    if (!found)
    {
        BOOLEAN ok = TRUE;
        HASHREC *hr = sym->overloadName->tp->syms->table[0];
        while (hr)
        {
            if (matchOverload(sym->tp, ((SYMBOL *)hr->p)->tp, TRUE))
            {
                ok = FALSE;
                break;
            }
            hr = hr->next;
        }
        if (ok)
        {
            HASHREC *hr = sym->overloadName->tp->syms->table[0];
            insertOverload(sym, sym->overloadName->tp->syms);
            while (hr)
            {
                if (matchOverload(sym->tp, ((SYMBOL *)hr->p)->tp, TRUE))
                {
                    ok = FALSE;
                    break;
                }
                hr = hr->next;
            }
        }

        if (sym->storage_class == sc_member || sym->storage_class == sc_virtual)
        {
            injectThisPtr(sym, basetype(sym->tp)->syms);
        }
    }
    if (!isExtern && !sym->dontinstantiate)
    {
        lex = sym->deferredCompile;
        if (lex)
        {
            int oldLinesHead = linesHead;
            int oldLinesTail = linesTail;
			int oldHeaderCount = templateHeaderCount;
            BOOLEAN oldTemplateType = inTemplateType;
            int oldPackIndex = packIndex;
            int nsl = PushTemplateNamespace(sym);
			int oldArgumentNesting = argument_nesting;
			argument_nesting = 0;
            packIndex = -1;
            linesHead = linesTail = NULL;
            if (sym->storage_class != sc_member && sym->storage_class != sc_mutable)
                sym->storage_class = sc_global;
            sym->linkage = lk_virtual;
            sym->xc = NULL;
            sym->maintemplate = orig;
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
			argument_nesting = oldArgumentNesting;
            packIndex = oldPackIndex;
            inTemplateType = oldTemplateType;
            linesHead = oldLinesHead;
            linesTail = oldLinesTail;
            instantiatingTemplate --;
            sym->genreffed |= warning;
            if (sym->mainsym)
                sym->mainsym->genreffed |= warning;
        }
        else
        {
            sym->storage_class = sc_external;
            InsertExtern(sym);
            InsertInline(sym);
        }
    }
    while (pushCount--)
        dropStructureDeclaration();
    lambdas = oldLambdas;
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
            TEMPLATEPARAMLIST *A = params;
            while (P && A)
            {
                if (A->p->packed)
                    A = A->p->byPack.pack;
                if (P->p->type == kw_typename)
                {
                    TYPE *ta = A->p->byClass.dflt;
                    TYPE *tp = P->p->byClass.val;
                    if (ta && tp)
                    {
                        while (isref(ta))
                            ta = basetype(ta)->btp;
                        while (isref(tp))
                            tp = basetype(tp)->btp;
                        if (isconst(ta) && !isconst(tp) ||
                            isvolatile(ta) && !isvolatile(tp) ||
                            !CheckConstCorrectness(tp, ta, TRUE))
                        {
                            spList[i] = 0;
                            break; 
                        }
                    }
                }
                A = A->next;
                P = P->next;
            }
        }
}
static BOOLEAN TemplateConstMatchingInternal(TEMPLATEPARAMLIST *P)
{
    BOOLEAN found = TRUE;
    while (P)
    {
        if (P->p->type == kw_typename)
        {
            if (!P->p->packed)
            {
                TYPE *td = P->p->byClass.dflt;
                TYPE *tv = P->p->byClass.val;
                if (!tv && isstructured(P->p->byClass.dflt) && basetype(P->p->byClass.dflt)->sp->templateLevel)
                {
                    if (!TemplateConstMatchingInternal(basetype(P->p->byClass.dflt)->sp->templateParams))
                    {
                        found = FALSE;
                        break;
                    }
                }
                else if (!tv)
                {
                    return FALSE;
                }
                else
                {
                    if (isref(td))
                        td = basetype(td)->btp;
                    if (isref(tv))
                        tv = basetype(tv)->btp;
                    if ((isconst(td) != isconst(tv)) ||
                        ((isvolatile(td) != isvolatile(tv)))
                        ||!CheckConstCorrectness(td, tv, TRUE))
                    {
                        found = FALSE;
                        break;
                    }
                }
            }
        }
        P = P->next;
    }
    return found;
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
                    if (P->p->type == kw_typename)
                    {
                        if (!P->p->packed)
                        {
                            TYPE *tv = P->p->byClass.val;
                            if (isref(tv))
                                tv = basetype(tv)->btp;
                            if (isconst(tv) || isvolatile(tv))
                            {
                                found = FALSE;
                                break;
                            }
                        }
                    }
                    P = P->next;
                }
            }
            else
            {
                found = TemplateConstMatchingInternal(spList[i]->templateParams->p->bySpecialization.types);
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
                        if (P->p->type == kw_typename)
                        {
                            if (!P->p->packed)
                            {
                                TYPE *tv = P->p->byClass.val;
                                if (isref(tv))
                                    tv = basetype(tv)->btp;
                                if (isconst(tv) || isvolatile(tv))
                                {
                                    spList[i] = 0;
                                }
                            }
                        }
                        P = P->next;
                    }
                }
                else
                {
                    if (!TemplateConstMatchingInternal(spList[i]->templateParams->p->bySpecialization.types))
                        spList[i] = 0;
                }
            }
    }
}
static void TransferClassTemplates(TEMPLATEPARAMLIST *dflt, TEMPLATEPARAMLIST *val, TEMPLATEPARAMLIST *params)
{
    if (val->p->type == kw_typename && val->p->byClass.dflt && val->p->byClass.val && isstructured(val->p->byClass.dflt) && isstructured(val->p->byClass.val))
    {
        TEMPLATEPARAMLIST *tpdflt = basetype(val->p->byClass.dflt)->sp->templateParams;
        TEMPLATEPARAMLIST *tpval = basetype(val->p->byClass.val)->sp->templateParams;
        while (tpdflt && tpval)
        {
            TransferClassTemplates(tpdflt, tpval, params);
            tpdflt = tpdflt->next;
            tpval = tpval->next;
        }
    }
    else if (val->p->type == kw_typename && val->p->byClass.dflt && val->p->byClass.val && isfunction(val->p->byClass.dflt) && isfunction(val->p->byClass.val))
	{
		HASHREC *hrd, *hrv;
		TYPE *tpd = basetype(val->p->byClass.dflt)->btp;
		TYPE *tpv = basetype(val->p->byClass.val)->btp;
		if (tpd->type == bt_templateparam && tpd->templateParam->p->sym)
		{
			TEMPLATEPARAMLIST *find = params;
			while (find)
			{
				if (find->p->sym && !strcmp(find->p->sym->name, tpd->templateParam->p->sym->name))
					break;
				find = find->next;
			}
			if (find)
			{
				if (tpd->templateParam->p->packed && !find->p->byPack.pack)
				{
					if (tpv->type == bt_templateparam)
					{
						find->p->byPack.pack = tpv->templateParam->p->byPack.pack;
					}
					else
					{
						find->p->byPack.pack = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
						find->p->byPack.pack->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
						find->p->byPack.pack->p->type = kw_typename;
						find->p->byPack.pack->p->byClass.val = tpv;
					}
				}
				if (!find->p->byClass.val)
					find->p->byClass.val = tpv->type == bt_templateparam ? tpv->templateParam->p->byClass.val : tpv;
			}
		}
		hrd = basetype(val->p->byClass.dflt)->syms->table[0];
		hrv = basetype(val->p->byClass.val)->syms->table[0];
		while (hrd && hrv)
		{
			tpd = ((SYMBOL *)hrd->p)->tp;
			tpv = ((SYMBOL *)hrv->p)->tp;
			if (tpd->type == bt_templateparam && tpd->templateParam->p->sym)
			{
				TEMPLATEPARAMLIST *find = params;
				while (find)
				{
					if (find->p->sym && !strcmp(find->p->sym->name, tpd->templateParam->p->sym->name))
						break;
					find = find->next;
				}
				if (find)
				{
					if (tpd->templateParam->p->packed && !find->p->byPack.pack)
					{
						if (tpv->type == bt_templateparam)
						{
							find->p->byPack.pack = tpv->templateParam->p->byPack.pack;
						}
						else
						{
							find->p->byPack.pack = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
							find->p->byPack.pack->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
							find->p->byPack.pack->p->type = kw_typename;
							find->p->byPack.pack->p->byClass.val = tpv;
						}
					}
					if (!find->p->byClass.val)
						find->p->byClass.val = tpv->type == bt_templateparam ? tpv->templateParam->p->byClass.val : tpv;
				}
			}
			hrd = hrd->next;
			hrv = hrv->next;
		}
	}
    else if (dflt->p->sym)
    {
        while (params)
        {
            if (!strcmp(dflt->p->sym->name, params->p->sym->name))
            {
                if (dflt->p->type == params->p->type && dflt->p->packed == params->p->packed)
                {
                    if (dflt->p->packed && !params->p->byPack.pack)
                        params->p->byPack.pack = val->p->byPack.pack;
                    if (!params->p->byClass.val)
                        params->p->byClass.val = val->p->byClass.val;
                }
                break;
            }
            params = params->next;
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
        TEMPLATEPARAMLIST *nested[50];
        int nestedCount = 0;
        rv = sp;
        if (!spsyms)
        {
            ClearArgValues(params, sp->specialized);
        }
        ClearArgValues(spsyms, sp->specialized);
        ClearArgValues(sp->templateParams, sp->specialized);
        while (initial && params)
        {
            if (initial->p->packed)
            {
                nested[nestedCount++] = initial->next;
                initial = initial->p->byPack.pack;
            }
            if (initial && params)
            {
                void *dflt = initial->p->byClass.dflt;
                TEMPLATEPARAMLIST *test = initial;
                if (!dflt)
                    dflt = initial->p->byClass.val;
                if (test->p->type != params->p->type)
                {
                    if (!test->p->byClass.dflt) 
                        rv = NULL;
                    else if (test->p->type != kw_typename || test->p->byClass.dflt->type != bt_templateselector || args->p->byClass.dflt->type != bt_templatedecltype)
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
                        if (params->p->type == kw_int)
                            nparam->p->byNonType.tp = params->p->byNonType.tp;
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
                        if (params->p->byClass.val)
                        {
                            switch (test->p->type)
                            {
                                case kw_typename:
                                    if (!templatecomparetypes(params->p->byClass.val, dflt, TRUE))
                                        rv = NULL;
                                    break;
                                case kw_int:
//#ifndef PARSER_ONLY
                                {
                                    EXPRESSION *exp = copy_expression(params->p->byNonType.val);
                                    optimize_for_constants(&exp);
                                    if (params->p->byNonType.val && !equalTemplateIntNode(exp, dflt))
                                        rv = NULL;
                                }
//#endif
                                    break;
                            }
                        }
                        params->p->byClass.val = dflt;
                        if (spsyms)
                        {
                            if (params->p->type == kw_typename)
                            {
                                if (params->p->byClass.dflt && !Deduce(params->p->byClass.dflt, params->p->byClass.val, TRUE, TRUE, FALSE))
                                    rv = NULL;
                                else
                                    TransferClassTemplates(params, params, nparams->next);
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
    //#ifndef PARSER_ONLY
                                EXPRESSION *exp = params->p->byNonType.val;
                                if (exp && !isintconst(exp))
                                {
                                    exp = copy_expression(exp);
                                    optimize_for_constants(&exp);
                                }
                                if (exp && params->p->byNonType.dflt && params->p->byNonType.dflt->type != en_templateparam && !equalTemplateIntNode(params->p->byNonType.dflt, exp))
                                    rv = NULL;
    //#endif
                            }
                        }
                        params->p->initialized = TRUE;
                        params = params->next;
                        primary = primary->next;
                    }
                }
                initial = initial->next;
                if (!initial && nestedCount)
                {
                    initial = nested[--nestedCount];
                }
            }
        }
        if (initial)
            rv = NULL;
        if (!templateNestingCount || instantiatingTemplate)
        {
            primary = spsyms ? spsyms : nparams->next;
            if (!TemplateParseDefaultArgs(sp, origParams, primary, primary))
                rv = NULL;
            if (spsyms)
            {
                while (params)
                {
                    if (params->p->byClass.val)
                    {
                        switch (params->p->type)
                        {
                            case kw_typename:
                                if (params->p->byClass.dflt->type != bt_templateparam && !templatecomparetypes(params->p->byClass.val, params->p->byClass.dflt, TRUE))
                                    rv = NULL;
                                break;
                            case kw_int:
    //#ifndef PARSER_ONLY
                            {
                                EXPRESSION *exp = copy_expression(params->p->byNonType.val);
                                optimize_for_constants(&exp);
                                if (params->p->byNonType.dflt && !equalTemplateIntNode(exp, params->p->byClass.dflt))
                                    rv = NULL;
                            }
    //#endif
                                break;
                        }
                    }
                    params = params->next;
                }
            }
            params = args;
            while (params && primary)
            {
                if (!primary->p->byClass.val && !primary->p->packed)
                {
                    rv = NULL;
                    break;
                }
                if (!primary->next && primary->p->packed)
                    break;
                primary = primary->next;
                params = params->next;
            }
            if (params && !primary)
            {
                rv = NULL;
            }
            
        }
        else
        {
            BOOLEAN packed = FALSE;
            params = origParams;
            while (params && args)
            {
                if (params->p->packed)
                    packed = TRUE;
                args = args->next;
                params = params->next;
            }
            if (params)
            {
                if (params->p->packed || !params->p->byClass.txtdflt || spsyms && params->p->byClass.dflt)
                    rv = NULL;
            }
            else if (args && !packed)
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
            return allTemplateArgsSpecified(basetype(tp)->sp, basetype(tp)->sp->templateParams->next);
        }
    }
    else if (basetype(tp)->type == bt_templateparam)
    {
        if (!basetype(tp)->templateParam->p->byClass.val)
            return FALSE;
    }
    else if (basetype(tp)->type == bt_templatedecltype)
        return FALSE;
    else if (basetype(tp)->type == bt_templateselector)
    {
        return FALSE;                
    }
    return TRUE;
}
static BOOLEAN checkArgSpecified(TEMPLATEPARAMLIST *args)
{
    if (!args->p->byClass.val)
        return FALSE;
    switch(args->p->type)
    {
        case kw_int:
            if (args->p->byNonType.val && !isarithmeticconst(args->p->byNonType.val))
            {
                EXPRESSION *exp = copy_expression(args->p->byNonType.val);
                optimize_for_constants(&exp);
                if (!isarithmeticconst(exp))
                {
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
            }
            break;
        case kw_template:
        {
            TEMPLATEPARAMLIST *tpl = tpl->p->byTemplate.args;
            while (tpl)
            {
                if (!allTemplateArgsSpecified(NULL, tpl))
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
BOOLEAN allTemplateArgsSpecified(SYMBOL *sym, TEMPLATEPARAMLIST *args)
{    
    while (args)
    {
        if (args->p->packed)
        {
            if (templateNestingCount && !args->p->byPack.pack || !allTemplateArgsSpecified(sym, args->p->byPack.pack))
                return FALSE;
        }
        else
        {
            if (sym)
            {
                if (args->p->type == kw_typename)
                {
                    TYPE *tp = args->p->byClass.val;
                    if (sameTemplate(tp, sym->tp))
                        return FALSE;
                }
            } 
            if (!checkArgSpecified(args))
            {
                return FALSE;
            }
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
static void ChooseShorterParamList(SYMBOL **spList, int n)
{
    int counts[1000];
    int counts1[1000];
    int z = INT_MAX, z1= INT_MAX, i;
    for (i=0; i < n; i++)
    {
        if (spList[i])
        {
            int c = 0;
            TEMPLATEPARAMLIST *tpl = spList[i]->templateParams->next;
            while (tpl)
                c++, tpl = tpl->next;
            counts[i] = c;
            if (c < z)
                z = c;
            c = 0;
            tpl = spList[i]->templateParams->p->bySpecialization.types;
            while (tpl)
                c++, tpl = tpl->next;
            counts1[i] = c;
            if (c < z1)
                z1 = c;
        }
        else
        {
            counts[i] = INT_MAX;
        }
    }
    for (i=0; i < n; i++)
        if (counts[i] != z || counts1[i] != z1)
            spList[i] = NULL;
}
static BOOLEAN constOnly(SYMBOL **spList, SYMBOL **origList, int n)
{
    int i;
    for (i=1; i < n; i++)
    {
        if (spList[i])
        {
            TEMPLATEPARAMLIST *tpl = origList[i]->templateParams->p->bySpecialization.types;
            while (tpl)
            {
                if (tpl->p->type == kw_typename && tpl->p->byClass.dflt)
                {
                    TYPE *tp1 = tpl->p->byClass.dflt;
                    if  (isconst(tp1) || isvolatile(tp1))
                        break;
                }
                tpl = tpl->next;
            }
            if (!tpl)
                return FALSE;
        }
    }
    return TRUE;
}
static TEMPLATEPARAMLIST *ResolveTemplateSelectors(TEMPLATEPARAMLIST *args)
{
    TEMPLATEPARAMLIST *locate = args;
    while (locate)
    {
        TYPE *tp;
        if (locate->p->packed)
            locate = locate->p->byPack.pack;
        if (locate)
        {
            TYPE *tp = locate->p->byClass.dflt;
            if (locate->p->type == kw_typename && tp)
            {
                while (ispointer(tp) || isref(tp))
                    tp = basetype(tp)->btp;
                tp = basetype(tp);
                if (tp->type == bt_templateselector)
                    break;
            }
            locate = locate->next;
        }
    }
    if (locate)
    {
        // this flattens packed args
        TEMPLATEPARAMLIST *rv = NULL, **last = &rv;
        locate = args;
        while (locate)
        {
            TYPE *tp;
            if (locate->p->packed)
                locate = locate->p->byPack.pack;
            if (locate)
            {
                TYPE *tp = locate->p->byClass.dflt;
                *last = Alloc(sizeof(TEMPLATEPARAMLIST));
                if (locate->p->type == kw_typename && tp)
                {
                    while (ispointer(tp) || isref(tp))
                        tp = basetype(tp)->btp;
                    tp = basetype(tp);
                    if (tp->type == bt_templateselector)
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
                        else if (basetype(ts->tp)->templateParam->p->type == kw_typename)
                        {
                            tp = basetype(ts->tp)->templateParam->p->byClass.val;
                            if (!tp)
                                return FALSE;
                            sp = tp->sp;
                        }
                        if (sp)
                        {
                            sp = basetype(PerformDeferredInitialization (sp->tp, NULL))->sp;
                            while (find && sp)
                            {
                                SYMBOL *spo = sp;
                                if (!isstructured(spo->tp))
                                    break;
                                
                                sp = search(find->name, spo->tp->syms);
                                if (!sp)
                                {
                                    sp = classdata(find->name, spo, NULL, FALSE, FALSE);
                                    if (sp == (SYMBOL *)-1)
                                        sp = NULL;
                                }
                                find = find->next;
                            }
                            if (!find && sp && istype(sp))
                            {
                                TYPE **tx;
                                (*last)->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
                                *(*last)->p = *locate->p;
                                tx = &(*last)->p->byClass.dflt;
                                tp = locate->p->byClass.dflt;
                                while (tp->type != bt_templateselector)
                                {
                                    *tx = (TYPE *)Alloc(sizeof(TYPE));
                                    **tx = *tp;
                                    tx = &(*tx)->btp;
                                    tp = tp->btp;
                                }
                                *tx = sp->tp;
                            }
                            else
                            {
                                (*last)->p = locate->p;
                            }
                        }
                        else
                        {
                            (*last)->p = locate->p;
                        }
                    }
                    else
                    {
                        (*last)->p = locate->p;
                    }
                }
                else
                {
                    (*last)->p = locate->p;
                }
                last = &(*last)->next;
                locate = locate->next;
            }
        }
        return rv;
    }
    else
    {
        return args;
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
    noErr |= inMatchOverload;
    if (!templateNestingCount)
        args = ResolveTemplateSelectors(args);
    if (sp->parentTemplate)
        sp = sp->parentTemplate;
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
    for (i=0,count=0; i < n; i++)
    {
        if (spList[i])
            count++;
    }
    if (count > 1)
    {
        int count1 = 0;
        BOOLEAN match0 = !!spList[0];
        spList[0] = 0;
        for (i=0; i < n; i++)
            if (spList[i])
                count1++;
        if (count1 > 1)
            TemplatePartialOrdering(spList, n, NULL, NULL, TRUE, FALSE);
        count1 = 0;
        for (i=0; i < n; i++)
            if (spList[i])
                count1++;
        if (count1 > 1 || count1 == 1 && match0 && constOnly(spList, origList, n))
        {
            for (i=0; i < n; i++)
            {
                if (match0 && i == 0 || spList[i])
                {
                    spList[i] = ValidateClassTemplate(origList[i], unspecialized, args);
                }
            }
            TemplateConstMatching(spList, n, args);
        }
        count1 = 0;
        for (i=0; i < n; i++)
            if (spList[i])
                count1++;
        if (count1 > 1)
        {
            spList[0] = 0;
            TemplateConstOrdering(spList, n, args);
        }
        count1 = 0;
        for (i=0; i < n; i++)
            if (spList[i])
                count1++;
        if (count1 > 1 && templateNestingCount)
        {
            // if it is going to be ambiguous but we are gathering a template, just choose the first one
            for (i=0; i < n; i++)
                if (spList[i])
                    break;
            for (i=i+1; i < n; i++)
                spList[i] = 0;
        }
        count1 = 0;
        for (i=0; i < n; i++)
            if (spList[i])
                count1++;
        if (count1 > 1)
            ChooseShorterParamList(spList, n);
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
        if  (found1->parentTemplate && allTemplateArgsSpecified(found1, found1->templateParams->next))
        {
			BOOLEAN partialCreation = FALSE;
			SYMBOL test = *found1;
            SYMBOL *parent = found1->parentTemplate;
            SYMBOL *sym = found1;
            TEMPLATEPARAMLIST *dflts;
            TEMPLATEPARAMLIST *orig;
            LIST *instants = parent->instantiations;

			dflts = found1->templateParams;

			while (dflts && ! partialCreation)
			{
				if (dflts && dflts->p->type == kw_int && dflts->p->byNonType.val)
					partialCreation = !isarithmeticconst(dflts->p->byNonType.val);
				dflts = dflts->next;
			}
			if (partialCreation)
			{
	            test.templateParams = copyParams(test.templateParams, TRUE);
				dflts = test.templateParams;

				while (dflts)
				{
					if (dflts && dflts->p->type == kw_int && dflts->p->byNonType.val)
						if (!isarithmeticconst(dflts->p->byNonType.val))
						{
							dflts->p->byNonType.val = copy_expression(dflts->p->byNonType.val);
							optimize_for_constants(&dflts->p->byNonType.val);
						}
					dflts = dflts->next;
				}
			}
            while (instants)
            {
                if (TemplateInstantiationMatch(instants->data, &test))
                {
    			    restoreParams(origList, n);
                    return (SYMBOL *)instants->data;
                }
                instants = instants->next;
            }
            found1 = clonesym(&test);
            found1->maintemplate = sym;
            found1->tp = Alloc(sizeof(TYPE));
            *found1->tp = *sym->tp;
            found1->tp->sp = found1;
            found1->gentemplate = TRUE;
            found1->instantiated = TRUE;
            found1->performedStructInitialization = FALSE;
			if (!partialCreation)
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
            /* rework */
			found1->templateParams = (TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
			found1->templateParams->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
			*found1->templateParams->p = *sym->templateParams->p;
            if (args)
                found1->templateParams->next = args;
            else
                found1->templateParams->next = sym->templateParams->next;
            /* end rework */
        }
    }
    restoreParams(origList, n);
    return found1;
}
void DoInstantiateTemplateFunction(TYPE *tp, SYMBOL **sp, NAMESPACEVALUES *nsv, SYMBOL *strSym, TEMPLATEPARAMLIST *templateParams, BOOLEAN isExtern)
{
    SYMBOL *sym = *sp;
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
                (*init)->exp = intNode(en_c_i, 0);
                init = &(*init)->next;
                hr = hr->next;
            }
            if (spi->parentClass)
            {
                TYPE *tp = Alloc(sizeof(TYPE));
                tp->type = bt_pointer;
                tp->size = getSize(bt_pointer);
                tp->btp = spi->parentClass->tp;
                funcparams->thistp = tp;
                funcparams->thisptr = intNode(en_c_i, 0);
            }
            instance = GetOverloadedTemplate(spi, funcparams);
            if (instance)
            {
                if (instance->templateLevel)
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
    if (cparams.prm_xcept)
        RTTIDumpType(cls->tp);
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
                    if (sym->templateLevel <= cls->templateLevel)
                    {
                        if (sym->deferredCompile && !sym->inlineFunc.stmt)
                        {
                            deferredCompileOne(sym);
                        }
                        InsertInline(sym);
                        GENREF(sym);
                    }
                    hr2 = hr2->next;
                }
            }
            else if (!ismember(sym) && !istype(sym))
                GENREF(sym);
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
            if (!tpl->p->byNonType.dflt)
                return FALSE;
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

/*static SYMBOL *matchTemplateFunc(SYMBOL *old, SYMBOL *instantiated)
{
    if (basetype(instantiated->tp)->syms)
    {
        FUNCTIONCALL list;
        INITLIST **next = &list.arguments , *il;
        HASHREC *hr, *hro, *hrs;
        EXPRESSION *exp = intNode(en_c_i, 0);
        TEMPLATEPARAMLIST *src, *dest;
        if (!instantiated->isConstructor && !instantiated->isDestructor && !TemplateDeduceFromType(basetype(old->tp)->btp, basetype(instantiated->tp)->btp))
            return FALSE;

        memset(&list, 0, sizeof(list));
        hr  = basetype(instantiated->tp)->syms->table[0];
        hro = hrs = basetype(old->tp)->syms->table[0];
        if (((SYMBOL *)hr->p)->tp->type == bt_void)
            if (((SYMBOL *)hro->p)->tp->type == bt_void)
                return TRUE;
        while (hr)
        {
            *next = Alloc(sizeof(INITLIST));
            (*next)->tp = ((SYMBOL *)hr->p)->tp;
            (*next)->exp = exp;
            next = &(*next)->next;
            hr = hr->next;
            if (!hro)
                return FALSE;
            hro = hro->next;
        }
        if (hro)
            return FALSE;
        if (instantiated->parentClass)
        {
            src = instantiated->parentClass->templateParams;
            dest = old->parentClass->templateParams;
            while (src && dest)
            {
                if (src->p->type != dest->p->type)
                    return FALSE;
                if (src->p->type != kw_new)
                {
                    dest->p->byClass.val = src->p->byClass.val;
                    dest->p->initialized = FALSE;
                }
                src = src->next;
                dest = dest->next;
            }
            if (src || dest)
                return FALSE;
        }
        return TemplateDeduceArgList(hrs, hrs, list.arguments, TRUE);
    }
    else if (!basetype(old->tp)->syms)
        return TRUE;
    return FALSE;
}
*/
void propagateTemplateDefinition(SYMBOL *sym)
{
    int oldCount = templateNestingCount;
    struct templateListData *oldList = currents;
    templateNestingCount = 0;
    currents = NULL;
    if (!sym->deferredCompile && !sym->inlineFunc.stmt)
    {
        SYMBOL *parent = sym->parentClass;
        if (parent)
        {
            SYMBOL *old = parent->maintemplate;
            if (!old)
                old = parent;
            if (old && old->tp->syms)
            {
                HASHREC **p = LookupName(sym->name, old->tp->syms);				
                if (p)
                {
                    HASHREC *hr;
                    hr = basetype(((SYMBOL *)(*p)->p)->tp)->syms->table[0];
                    while (hr)
                    {
                        SYMBOL *cur = (SYMBOL *)hr->p;
                        //                        if (sym->maintemplate && !strcmp(sym->maintemplate->decoratedName, cur->decoratedName) && cur->parentClass && cur->deferredCompile)// && matchTemplateFunc(cur, sym))
                        if (sym && sym->origdeclline == cur->origdeclline && !strcmp(sym->origdeclfile, cur->origdeclfile) && cur->deferredCompile)
                        {
                            if (matchesCopy(cur, FALSE) == matchesCopy(sym, FALSE) && matchesCopy(cur, TRUE) == matchesCopy(sym, TRUE))
                            {
                                sym->deferredCompile = cur->deferredCompile;
                                sym->memberInitializers = cur->memberInitializers;
                                sym->pushedTemplateSpecializationDefinition = 1;
                                if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                                {
                                    HASHREC *src = basetype(cur->tp)->syms->table[0];
                                    HASHREC *dest = basetype(sym->tp)->syms->table[0];
                                    while (src && dest)
                                    {
                                        dest->p->name = src->p->name;
                                        src = src->next;
                                        dest = dest->next;
                                    }
                                }
                                {
                                    STRUCTSYM t, s,r;
                                    SYMBOL *thsprospect = (SYMBOL *)basetype(sym->tp)->syms->table[0]->p;
                                    t.tmpl = NULL;
                                    r.tmpl = NULL;
                                    if (cur->templateParams)
                                    {
                                        r.tmpl = cur->templateParams;
                                        addTemplateDeclaration(&r);
                                    }
                                    if (thsprospect && thsprospect->thisPtr)
                                    {
                                        SYMBOL *spt = basetype (basetype(thsprospect->tp)->btp)->sp;
                                        t.tmpl = spt->templateParams;
                                        if (t.tmpl)
                                            addTemplateDeclaration(&t);
                                    }
                                    s.str = sym->parentClass;
                                    addStructureDeclaration(&s);
                                    deferredCompileOne(sym);
                                    dropStructureDeclaration();
                                    if (r.tmpl)
                                        dropStructureDeclaration();
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
        else
        {
            SYMBOL *old;
            int tns = PushTemplateNamespace(sym);
            old = namespacesearch(sym->name, globalNameSpace, FALSE, FALSE);
            if (old)
            {
                HASHREC *hr;
                hr = basetype(old->tp)->syms->table[0];
                while (hr)
                {
                    SYMBOL *cur = (SYMBOL *)hr->p;
                        if (sym && sym->declline == cur->declline && !strcmp(sym->declfile, cur->declfile) && cur->deferredCompile)
//                    if (cur->templateLevel && cur->deferredCompile && matchTemplateFunc(cur, sym))
                    {
                        sym->deferredCompile = cur->deferredCompile;
                        cur->pushedTemplateSpecializationDefinition = 1;
                        if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                        {
                            HASHREC *src = basetype(cur->tp)->syms->table[0];
                            HASHREC *dest = basetype(sym->tp)->syms->table[0];
                            while (src && dest)
                            {
                                dest->p->name = src->p->name;
                                src = src->next;
                                dest = dest->next;
                            }
                        }
                        {
                            STRUCTSYM t;
                            SYMBOL *thsprospect = (SYMBOL *)basetype(sym->tp)->syms->table[0]->p;
                            t.tmpl = NULL;
                            if (thsprospect && thsprospect->thisPtr)
                            {
                                SYMBOL *spt = basetype (basetype(thsprospect->tp)->btp)->sp;
                                t.tmpl = spt->templateParams;
                                if (t.tmpl)
                                    addTemplateDeclaration(&t);
                            }
//                                    TemplateFunctionInstantiate(cur, FALSE, FALSE);
                            deferredCompileOne(sym);
                            if (t.tmpl)
                                dropStructureDeclaration();
                        }
                    }
                    hr = hr->next;
                }
            }
            PopTemplateNamespace(tns);
        }
    }
    else if (sym->deferredCompile && !sym->inlineFunc.stmt)
    {
        int n = PushTemplateNamespace(sym);
        deferredCompileOne(sym);
        PopTemplateNamespace(n);
    }
    currents = oldList;
    templateNestingCount = oldCount;
}
LEXEME *TemplateDeclaration(LEXEME *lex, SYMBOL *funcsp, enum e_ac access, enum e_sc storage_class, BOOLEAN isExtern)
{
    HASHTABLE *oldSyms = localNameSpace->syms;
    lex = getsym();
    localNameSpace->syms = NULL;
    if (MATCHKW(lex, lt))
    {
        int lasttemplateHeaderCount = templateHeaderCount;
        TEMPLATEPARAMLIST **tap; // for the specialization list
        TYPE *tp = NULL;
        SYMBOL *declSym = NULL;
        struct templateListData l;
        int count = 0;
        int oldInstantiatingTemplate = instantiatingTemplate;
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
        templateNestingCount++;
        instantiatingTemplate = 0;
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
                count ++;
            }
            currents->ptail = &(*currents->ptail)->p->bySpecialization.next;
        }
        templateNestingCount--;
        if (MATCHKW(lex, kw_friend))
        {
            lex = getsym();
            templateNestingCount++;
            inTemplateType = TRUE;
            lex = declare(lex, NULL, NULL, sc_global, lk_none, NULL, TRUE, FALSE, TRUE, TRUE, access);
            inTemplateType = FALSE;
            templateNestingCount--;
            instantiatingTemplate = oldInstantiatingTemplate;
        }
        else if (lex)
        {
            templateNestingCount++;
            inTemplateType = TRUE;
            lex = declare(lex, funcsp, &tp, storage_class, lk_none, NULL, TRUE, FALSE, FALSE, TRUE, access);
            inTemplateType = FALSE;
            templateNestingCount--;
            instantiatingTemplate = oldInstantiatingTemplate;
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
                        if (!isExtern)
                        {
                            instance->dontinstantiate = FALSE;
    						instance = TemplateClassInstantiate(instance, templateParams, FALSE, sc_global);
                            referenceInstanceMembers(instance);
                        }
                        else
                        {
                            instance->dontinstantiate = TRUE;
                        }
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
            int consdest = 0;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
            lex = getBasicType(lex, funcsp, &tp, &strSym, TRUE, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, &consdest, NULL, FALSE, TRUE);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
            lex = getBeforeType(lex, funcsp, &tp, &sym, &strSym, &nsv, TRUE, sc_cast, &linkage, &linkage2, &linkage3, FALSE, consdest, FALSE, FALSE);
            sizeQualifiers(tp);
            if (strSym)
            {
                s.str = strSym;
                addStructureDeclaration(&s);
                
            }
            if (notype && !consdest)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else if (isfunction(tp))
            {
                SYMBOL *sp = sym;
                TEMPLATEPARAMLIST *templateParams = TemplateGetParams(sym->parentClass);
                DoInstantiateTemplateFunction(tp, &sp, nsv, strSym, templateParams, isExtern);
                sym = sp;
                if (!comparetypes(basetype(sp->tp)->btp, basetype(tp)->btp, TRUE))
                {
                    errorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp);
                }
                if (isExtern)
                {
                    sp->dontinstantiate = TRUE;
                    InsertExtern(sp);
                }
                else
                {
                    sp->dontinstantiate = FALSE;
                    GENREF(sp);
                    InsertInline(sp);
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
                        spi->dontinstantiate = isExtern;
                        if (isExtern)
                            InsertExtern(spi);
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
    localNameSpace->syms = oldSyms;
    return lex;
}