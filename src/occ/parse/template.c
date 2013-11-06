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

BOOL inTemplateBody;
int templateNestingCount =0 ;

struct listData
{
    struct listData *next;
    TEMPLATEARG *args;
    LEXEME *head, *tail;
    LEXEME *bodyHead, *bodyTail;
    SYMBOL *sym;
    
} *currents;

static LEXEME *TemplateArg(LEXEME *lex, SYMBOL *funcsp, TEMPLATEARG *arg);

void templateInit(void)
{
    inTemplateBody = FALSE;
    templateNestingCount = 0;
    currents = NULL;
}
void TemplateGetDeferred(SYMBOL *sym)
{
    if (currents)
    {
        sym->deferredTemplateHeader = currents->head;
        sym->deferredCompile = currents->bodyHead;
    }
}
void TemplateRegisterDeferred(LEXEME *lex)
{
    if (templateNestingCount && lex)
    {
        LEXEME *cur = globalAlloc(sizeof(LEXEME));
        *cur = *lex;
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
    }
}
static BOOL exactMatchOnTemplateArgs(TEMPLATEARG *old, TEMPLATEARG *sym)
{
    while (old && sym)
    {
        if (old->type != sym->type)
            break;
        if (old->type == kw_template)
        {
            if (!exactMatchOnTemplateArgs(old->byTemplate.args, sym->byTemplate.args))
                break;
        }
        old = old->next;
        sym = sym->next;
    }
    return !(old || sym);
}
static TEMPLATEARG * mergeTemplateDefaults(TEMPLATEARG *old, TEMPLATEARG *sym)
{
    TEMPLATEARG *rv = sym;
#ifndef PARSER_ONLY
    while (old && sym)
    {
        switch (sym->type)
        {
            case kw_template:
                sym->byTemplate.args = mergeTemplateDefaults(old->byTemplate.args, sym->byTemplate.args);
                if (old->byTemplate.dflt && sym->byTemplate.dflt)
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->sym);
                else if (!sym->byTemplate.dflt)
                    sym->byTemplate.dflt = old->byTemplate.dflt;
                break;
            case kw_typename:
                if (old->byClass.dflt && sym->byClass.dflt)
                {
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->sym);
                }
                else if (!sym->byClass.dflt)
                    sym->byClass.dflt = old->byClass.dflt;
                break;
            case kw_int:
                if (old->byNonType.dflt && sym->byNonType.dflt)
                {
                    errorsym(ERR_MULTIPLE_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, sym->sym);
                }
                else if (!sym->byNonType.dflt)
                    sym->byNonType.dflt = old->byNonType.dflt;
                break;
        }
        old = old->next;
        sym = sym->next;
    }
#endif
    return rv;
}
static void checkTemplateDefaults(TEMPLATEARG *args)
{
    SYMBOL *last = NULL;
    while (args)
    {
        void *dflt = NULL;
        switch (args->type)
        {
            case kw_template:
                checkTemplateDefaults(args->byTemplate.args);
                dflt = args->byTemplate.dflt;
                break;
            case kw_typename:
                dflt = args->byClass.dflt;
                break;
            case kw_int:
                dflt = args->byNonType.dflt;
                break;
        }
        if (last && !dflt)
        {
            errorsym(ERR_MISSING_DEFAULT_VALUES_IN_TEMPLATE_DECLARATION, last);
            break;
        }
        if (dflt)
            last = args->sym;
        args = args->next;
    }
}
TEMPLATEARG * TemplateMatching(LEXEME *lex, TEMPLATEARG *old, TEMPLATEARG *sym)
{
    TEMPLATEARG *rv = NULL;
    if (old)
    {
        if (!exactMatchOnTemplateArgs(old, sym))
        {
            error(ERR_TEMPLATE_DEFINITION_MISMATCH);
        }
        else 
        {
            rv = mergeTemplateDefaults(old, sym);
            checkTemplateDefaults(rv);
        }
    }
    else
    {
        checkTemplateDefaults(sym);
    }
    return rv;
}
static LEXEME *TemplateHeader(LEXEME *lex, SYMBOL *funcsp, TEMPLATEARG **args)
{
    if (needkw(&lex, lt))
    {
        while (1)
        {
            if (MATCHKW(lex, gt) || MATCHKW(lex, rightshift))
               break;
            *args = Alloc(sizeof(TEMPLATEARG));
            lex = TemplateArg(lex, funcsp, *args);
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
static LEXEME *TemplateArg(LEXEME *lex, SYMBOL *funcsp, TEMPLATEARG *arg)
{
    switch (KW(lex))
    {
        TYPE *tp, *tp1;
        EXPRESSION *exp1;
        SYMBOL *sp;
        case kw_class:
        case kw_typename:
            arg->type = kw_typename;
            arg->packed = FALSE;
            lex = getsym();
            if (MATCHKW(lex, ellipse))
            {
                arg->packed = TRUE;
                lex = getsym();
            }
            if (ISID(lex) || MATCHKW(lex, classsel))
            {
                SYMBOL *sym = NULL, *strsym = NULL;
                NAMESPACEVALUES *nsv = NULL;
                BOOL qualified = MATCHKW(lex, classsel);
                
                lex = nestedSearch(lex, &sym, &strsym, &nsv, NULL, FALSE);
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
                    tp->templateArg = arg;
                    arg->sym = makeID(sc_templateparam, tp, NULL, litlate(lex->value.s.a));
                    lex = getsym();
                }
            }
            else
            {
                TYPE *tp = Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->templateArg = arg;
                arg->sym = makeID(sc_templateparam, tp, NULL, "-tpl-");
            }
            if (MATCHKW(lex, assign))
            {
                if (arg->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                lex = getsym();
                lex = get_type_id(lex, &arg->byClass.dflt, funcsp, FALSE);
                if (!arg->byClass.dflt)
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
            arg->type = kw_template;
            lex = getsym();
            lex = TemplateHeader(lex, funcsp, &arg->byTemplate.args);
            arg->packed = FALSE;
            if (!MATCHKW(lex, kw_class))
            {
                error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
            }       
            else
            {
                lex = getsym();
            }
            if (MATCHKW(lex, ellipse))
            {
                arg->packed = TRUE;
                lex = getsym();
            }
            if (ISID(lex))
            {
                TYPE *tp = Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->templateArg = arg;
                arg->sym = makeID(sc_templateparam, tp, NULL, litlate(lex->value.s.a));
                lex = getsym();
            }
            else
            {
                TYPE *tp = Alloc(sizeof(TYPE));
                tp->type = bt_templateparam;
                tp->templateArg = arg;
                arg->sym = makeID(sc_templateparam, tp, NULL, "-tpl-");
            }
            if (MATCHKW(lex, assign))
            {
                if (arg->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                lex = getsym();
                if (ISID(lex))
                {
                    lex = id_expression(lex, funcsp, &arg->byTemplate.dflt, NULL, NULL, FALSE, FALSE, lex->value.s.a);
                    if (!arg->byTemplate.dflt)
                    {
                        currentErrorLine = 0;
                        errorstr(ERR_UNDEFINED_IDENTIFIER,lex->value.s.a);
                    }
                    lex = getsym();
                }
                else
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
            BOOL defd = FALSE;
            BOOL notype = FALSE;
            arg->type = kw_int;
            arg->packed = FALSE;
            tp = NULL;
            sp = NULL;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
            lex = getBasicType(lex, funcsp, &tp, NULL, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3);
            lex = getBeforeType(lex, funcsp, &tp, &sp, NULL, NULL, sc_cast, &linkage, &linkage2, &linkage3, FALSE, FALSE, FALSE); /* fixme at file scope init */
            sizeQualifiers(tp);
            if (!tp)
            {
                error(ERR_TYPE_NAME_EXPECTED);
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
                tpa->templateArg = arg;
                sp->storage_class = sc_templateparam;
                sp->tp = tpa;
                arg->sym = sp;
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
                arg->byNonType.tp = tp;
                if (!isint(tp) && !ispointer(tp) && basetype(tp)->type != bt_lref)
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
                        lex = expression_no_comma(lex, funcsp, NULL, &tp1, &exp1, TRUE, TRUE);
                        arg->byNonType.dflt = exp1;
                        if (!comparetypes(tp, tp1, TRUE))
                        {
                            if (!ispointer(tp) || !isint(tp1) || !isconstzero(tp1, exp1))
                                error(ERR_TYPE_NONTYPE_TEMPLATE_PARAMETER_DEFAULT_TYPE_MISMATCH);
                        }
                    }
                }
            }
            break;
        }
    }
    
    return lex;
}
LEXEME *TemplateDeclaration(LEXEME *lex, SYMBOL *funcsp, enum e_ac access)
{
    int max = 0;
    TEMPLATEARG *ta = NULL;
    STRUCTSYM s;
    TYPE *tp = NULL;
    struct listData l;
    
    while (MATCHKW(lex, kw_template)) 
    {
        max++;
        lex = getsym();
        ta = NULL;
        lex = TemplateHeader(lex, funcsp, &ta);
    }
    if (lex)
    {
        LIST *lastExtern = externals;
        s.tmpl = ta;
        addTemplateDeclaration(&s);
        if (templateNestingCount == 0)
        {
            l.args = ta;
            l.next = currents;
            l.sym = NULL;
            l.head = l.tail = NULL;
            l.bodyHead = l.bodyTail = NULL;
            currents = &l;
        }
        templateNestingCount++;
        lex = declare(lex, funcsp, &tp, ta, sc_template, lk_none, NULL, TRUE, FALSE, FALSE, access);
        templateNestingCount--;
        if (!templateNestingCount)
        {
            if (!tp)
            {
                error(ERR_TEMPLATES_MUST_BE_CLASSES_OR_FUNCTIONS);
            }
            else if (!isfunction(tp) && !isstructured(tp) )
            {
                if (!l.sym || !l.sym->parentClass || l.sym->storage_class == sc_member || l.sym->storage_class == sc_virtual)
                {
                    error(ERR_TEMPLATES_MUST_BE_CLASSES_OR_FUNCTIONS);
                }
            }
            dropStructureDeclaration();
            currents = currents->next;
            FlushLineData("", INT_MAX);
            externals = lastExtern;            
        }
    }
    if (max > 1)
        error(ERR_TOO_MANY_TEMPLATE_PARAMETER_SETS_SPECIFIED);
    return lex;
}
