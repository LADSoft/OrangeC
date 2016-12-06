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

extern int codeLabel;

#ifndef CPREPROCESSOR
extern ARCH_DEBUG *chosenDebugger;
extern FILE *listFile;
extern INCLUDES *includes;
extern int structLevel;
extern int templateNestingCount;
#endif

NAMESPACEVALUES *globalNameSpace, *localNameSpace;
HASHTABLE *labelSyms;

HASHTABLE *CreateHashTable(int size);
BOOLEAN inMatchOverload;

static LIST *usingDirectives;

#ifdef PARSER_ONLY
extern HASHTABLE *kwhash;
extern HASHTABLE *defsyms;
extern HASHTABLE *labelSyms;
extern HASHTABLE *ccHash;
void ccSetSymbol(SYMBOL *s);
#endif

void syminit(void)
{
    globalNameSpace = Alloc(sizeof(NAMESPACEVALUES));
    globalNameSpace->syms = CreateHashTable(GLOBALHASHSIZE);
    globalNameSpace->tags = CreateHashTable(GLOBALHASHSIZE);
    localNameSpace = Alloc(sizeof(NAMESPACEVALUES));
    usingDirectives = NULL;
    inMatchOverload = FALSE;
}
HASHTABLE *CreateHashTable(int size)
{
    HASHTABLE *rv = Alloc(sizeof(HASHTABLE));
    rv->table = Alloc(sizeof(HASHREC *) * size);
    rv->size = size;
    return rv;
}
#ifndef CPREPROCESSOR
void AllocateLocalContext(BLOCKDATA *block, SYMBOL *sp, int label)
{
    HASHTABLE *tn = CreateHashTable(1);
    STATEMENT *st;
    LIST *l;
    st = stmtNode(NULL, block, st_dbgblock);
    st->label = 1;
    if (block && cparams.prm_debug)
    {
        st = stmtNode(NULL, block, st_label);
        st->label = label;
        tn->blocknum = st->blocknum;
    }
    tn->next = tn->chain = localNameSpace->syms;
    localNameSpace->syms = tn;
    tn = CreateHashTable(1);
    if (block && cparams.prm_debug)
    {
        tn->blocknum = st->blocknum;
    }
    tn->next = tn->chain = localNameSpace->tags;
    localNameSpace->tags = tn;
    if (sp)
        localNameSpace->tags->blockLevel = sp->value.i++;
    
    l = Alloc(sizeof(LIST));
    l->data = localNameSpace->usingDirectives;
    l->next = usingDirectives;
    usingDirectives = l;
    
}
#ifdef PARSER_ONLY
void TagSyms(HASHTABLE *syms)
{
    int i;
    for (i=0; i < syms->size; i++)
    {
        HASHREC *hr = syms->table[i];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            sp->ccEndLine = includes->line+1;
            hr = hr->next;
        }    
    }
}
#endif
void FreeLocalContext(BLOCKDATA *block, SYMBOL *sp, int label)
{
    HASHTABLE *locals = localNameSpace->syms;
    HASHTABLE *tags = localNameSpace->tags;
    STATEMENT *st;
    if (block && cparams.prm_debug)
    {
        st = stmtNode(NULL, block, st_label);
        st->label = label;
    }
    checkUnused(localNameSpace->syms);
    if (sp && listFile)
    {
        if (localNameSpace->syms->table[0])
        {
            fprintf(listFile, "******** Local Symbols ********\n");
            list_table(sp->inlineFunc.syms,0);
            fprintf(listFile, "\n");
        }
        if (localNameSpace->tags->table[0])
        {
            fprintf(listFile,"******** Local Tags ********\n");
            list_table(sp->inlineFunc.tags, 0);
            fprintf(listFile, "\n");
        }
    }
    if (sp)
        sp->value.i--;

    st = stmtNode(NULL, block, st_expr);
    destructBlock(&st->select, localNameSpace->syms->table[0], TRUE);    
    localNameSpace->syms = localNameSpace->syms->next;
    localNameSpace->tags = localNameSpace->tags->next;

    localNameSpace->usingDirectives = usingDirectives->data;
    usingDirectives = usingDirectives->next;

#ifdef PARSER_ONLY
    TagSyms(locals);
    TagSyms(tags);
#endif
    if (sp)
    {
        locals->next = sp->inlineFunc.syms;
        tags->next = sp->inlineFunc.tags;
        sp->inlineFunc.syms = locals;
        sp->inlineFunc.tags = tags;
    }
    st = stmtNode(NULL, block, st_dbgblock);
    st->label = 0;
}
#endif
/* SYMBOL tab hash function */
static int GetHashValue(char *string)
{
    unsigned i;
    for (i = 0;  *string; string++)
        i = ((i << 7) + (i << 1) +i )  ^ *string;
    return i;
}
HASHREC **GetHashLink(HASHTABLE *t, char *string)
{
    unsigned i;
    if ( t->size == 1)
        return &t->table[0];
    for (i = 0;  *string; string++)
        i = ((i << 7) + (i << 1) +i )  ^ *string;
    return &t->table[i % t->size];
}
/* Add a hash item to the table */
HASHREC *AddName(SYMBOL *item, HASHTABLE *table)
{
    HASHREC **p = GetHashLink(table, item->name);
    HASHREC *newRec;

    if (*p)
    {
        HASHREC *q =  *p,  *r =  *p;
        while (q)
        {
            r = q;
            if (!strcmp(r->p->name, item->name))
                return (r);
            q = q->next;
        }
        newRec = Alloc(sizeof(HASHREC));
        r->next = newRec;
        newRec->p = (struct _hrintern_ *)item;
    }
    else
    {
        newRec = Alloc(sizeof(HASHREC));
        *p = newRec;
        newRec->p = (struct _hrintern_ *)item;
    }
    return (0);
}
HASHREC *AddOverloadName(SYMBOL *item, HASHTABLE *table)
{
    HASHREC **p = GetHashLink(table, item->decoratedName);
    HASHREC *newRec;
#ifdef PARSER_ONLY
    if (!item->parserSet)
    {
        item->parserSet = TRUE;
        ccSetSymbol(item);
    }
#endif

    if (*p)
    {
        HASHREC *q =  *p,  *r =  *p;
        while (q)
        {
            r = q;
            if (!strcmp(((SYMBOL *)r->p)->decoratedName, item->decoratedName))
                return (r);
            q = q->next;
        }
        newRec = Alloc(sizeof(HASHREC));
        r->next = newRec;
        newRec->p = (struct _hrintern_ *)item;
    }
    else
    {
        newRec = Alloc(sizeof(HASHREC));
        *p = newRec;
        newRec->p = (struct _hrintern_ *)item;
    }
    return (0);
}

/*
 * Find something in the hash table
 */
HASHREC **LookupName(char *name, HASHTABLE *table)
{
    HASHREC **p = GetHashLink(table, name);

    while (*p)
    {
        if (!strcmp((*p)->p->name, name))
        {
            return p;
        }
        p =  (HASHREC **)*p;
    }
    return (0);
}
SYMBOL *search(char *name, HASHTABLE *table)
{
    while (table)
    {
        HASHREC **p = LookupName(name, table);
        if (p)
            return (SYMBOL *)(*p)->p;
        table = table->next;
    }
    return NULL;
}
BOOLEAN matchOverload(TYPE *tnew, TYPE *told, BOOLEAN argsOnly)
{
    HASHREC *hnew = basetype(tnew)->syms->table[0];
    HASHREC *hold = basetype(told)->syms->table[0];
    unsigned tableOld[100], tableNew[100];
    int tCount = 0;
    if (!cparams.prm_cplusplus)
        argsOnly = TRUE;
    if (isconst(tnew) != isconst(told))
        return FALSE;
    if (isvolatile(tnew) != isvolatile(told))
        return FALSE;
    if (islrqual(tnew) != islrqual(told))
        return FALSE;
    if (isrrqual(tnew) != isrrqual(told))
        return FALSE;
    inMatchOverload++;
    while (hnew && hold)
    {
        SYMBOL *snew = (SYMBOL *)hnew->p;
        SYMBOL *sold = (SYMBOL *)hold->p;
        TYPE *tnew, *told;
        if (sold->thisPtr)
        {
            hold = hold->next;
            if (!hold)
                break;
            sold = (SYMBOL *)hold->p;
        }
        if (snew->thisPtr)
        {
            hnew = hnew->next;
            if (!hnew)
                break;
            snew = (SYMBOL *)hnew->p;
        }
        tnew = basetype(snew->tp);
        told = basetype(sold->tp);
        if (told->type != bt_any || tnew->type != bt_any) // packed template param
        {
            if ((!comparetypes(told, tnew, TRUE) && !sameTemplatePointedTo(told, tnew)) || told->type != tnew->type)
                break;
            else 
            {
                TYPE *tps = sold->tp;
                TYPE *tpn = snew->tp;
                if (isref(tps))
                    tps = basetype(tps)->btp;
                if (isref(tpn))
                    tpn = basetype(tpn)->btp;
                while (ispointer(tpn) && ispointer(tps))
                {
                    if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                    {
                        inMatchOverload--;
                        return FALSE;
                    }
                    tpn= basetype(tpn)->btp;
                    tps = basetype(tps)->btp;
                }
                if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                {
                    inMatchOverload--;
                    return FALSE;
                }
                tpn = basetype(tpn);
                tps = basetype(tps);
                if (tpn->type == bt_templateparam)
                {
                    if (tps->type != bt_templateparam)
                        break;
					if (tpn->templateParam->p->packed != tps->templateParam->p->packed)
						break;
                    tableOld[tCount] = GetHashValue(tps->templateParam->argsym->name);
                    tableNew[tCount] = GetHashValue(tpn->templateParam->argsym->name);
                    tCount++;
                }
            }
        }
        hold = hold->next;
        hnew = hnew->next;
    }
    inMatchOverload--;
    if (!hold && !hnew)
    {
        int i;
        TEMPLATEPARAMLIST *tplNew, *tplOld;

        if (tCount)
        {
            int i,j;
            SYMBOL *fnew = basetype(tnew)->sp->parentClass;
            SYMBOL *fold = basetype(told)->sp->parentClass;
            TEMPLATEPARAMLIST *tplNew, *tplOld;
            int iCount = 0;
            unsigned oldIndex[100], newIndex[100];
            tplNew = fnew && fnew->templateParams ? fnew->templateParams->next : NULL;
            tplOld = fold && fold->templateParams ? fold->templateParams->next : NULL;
            while (tplNew && tplOld)
            {
                if (tplOld->argsym && tplNew->argsym)
                {
                    oldIndex[iCount] = GetHashValue(tplOld->argsym->name);
                    newIndex[iCount] = GetHashValue(tplNew->argsym->name);
                    iCount++;
                }
                tplNew = tplNew->next;
                tplOld = tplOld->next;
            }
            for (i=0; i < tCount; i++)
            {
                int k, l;
                for (k=0; k < iCount; k++)
                    if (tableOld[i] == oldIndex[k])
                        break;
                for (l=0; l < iCount; l++)
                    if (tableNew[i] == newIndex[l])
                        break;
                if (k != l)
                {
                    return FALSE;
                }
                for (j=i+1; j < tCount; j++)
                    if (tableOld[i] == tableOld[j])
                    {
                        if (tableNew[i] != tableNew[j])
                            return FALSE;
                    }
                    else
                    {
                        if (tableNew[i] == tableNew[j])
                            return FALSE;
                    }
            }
        }
        if (basetype(tnew)->sp && basetype(told)->sp)
        {
            if (basetype(tnew)->sp->templateLevel || basetype(told)->sp->templateLevel)
            {
                TYPE *tps = basetype(told)->btp;
                TYPE *tpn = basetype(tnew)->btp;
                if (!templatecomparetypes(tpn, tps, TRUE) && !sameTemplate(tpn, tps))
                {
                    if (isref(tps))
                        tps = basetype(tps)->btp;
                    if (isref(tpn))
                        tpn = basetype(tpn)->btp;
                    while (ispointer(tpn) && ispointer(tps))
                    {
                        if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                            return FALSE;
                        tpn= basetype(tpn)->btp;
                        tps = basetype(tps)->btp;
                    }
                    if (isconst(tpn) != isconst(tps) || isvolatile(tpn) != isvolatile(tps))
                        if (basetype(tpn)->type != bt_templateselector)
                            return FALSE;
                    tpn = basetype(tpn);
                    tps = basetype(tps);
                    if (comparetypes(tpn, tps, TRUE) || tpn->type == bt_templateparam && tps->type == bt_templateparam)
                    {
                        return TRUE;
                    }
                    else if (isarithmetic(tpn) && isarithmetic(tps))
                    {
                        return FALSE;
                    }
                    else if (tpn->type == bt_templateselector)
                    {
                        if (tps->type == bt_templateselector)
                        {
                            if (!templateselectorcompare(tpn->sp->templateSelector, tps->sp->templateSelector))
                            {
                                TEMPLATESELECTOR *ts1 = tpn->sp->templateSelector->next, *tss1;
                                TEMPLATESELECTOR *ts2 = tps->sp->templateSelector->next, *tss2;
                                if (ts2->sym->typedefSym)
                                {
                                    ts1 = ts1->next;
                                    if (!strcmp(ts1->name, ts2->sym->typedefSym->name))
                                    {
                                        ts1 = ts1->next;
                                        ts2 = ts2->next;
                                        while (ts1 && ts2)
                                        {
                                            if (strcmp(ts1->name, ts2->name))
                                                return FALSE;
                                            ts1 = ts1->next;
                                            ts2 = ts2->next;
                                        }
                                        if (ts1 || ts2)
                                            return FALSE;
                                    }
                                }
                            }
                        }
                        else
                        {
                            TEMPLATESELECTOR *tpl = basetype(tpn)->sp->templateSelector->next;
                            SYMBOL *sp = tpl->sym;
                            TEMPLATESELECTOR *find = tpl->next;
                            while (sp && find)
                            {
                                SYMBOL *fsp;
                                if (!isstructured(sp->tp))
                                    break;
                                
                                fsp = search(find->name, basetype(sp->tp)->syms);
                                if (!fsp)
                                {
                                    fsp = classdata(find->name, basetype(sp->tp)->sp, NULL, FALSE, FALSE);
                                    if (fsp == (SYMBOL *)-1)
                                        fsp = NULL;
                                }
                                sp = fsp;
                                find = find->next;
                            }
                            if (find || !sp || !comparetypes(sp->tp, tps, TRUE) && !sameTemplate(sp->tp, tps))
                                return FALSE;                            
                        }
                        return TRUE;
                    }
                    else if (tpn->type == bt_templatedecltype && tps->type == bt_templatedecltype)
                    {
                        return templatecompareexpressions(tpn->templateDeclType, tps->templateDeclType);
                    }
                    return TRUE; 
                }
                if (tpn->type == bt_templateselector && tps->type == bt_templateselector)
                {
                    TEMPLATESELECTOR *ts1 = tpn->sp->templateSelector->next, *tss1;
                    TEMPLATESELECTOR *ts2 = tps->sp->templateSelector->next, *tss2;
                    if (ts1->isTemplate != ts2->isTemplate || strcmp(ts1->sym->decoratedName, ts2->sym->decoratedName))
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
                    return templateselectorcompare(tpn->sp->templateSelector, tps->sp->templateSelector);
                }
            }
            else if (basetype(tnew)->sp->castoperator)
            {
                TYPE *tps = basetype(told)->btp;
                TYPE *tpn = basetype(tnew)->btp;
                if (!templatecomparetypes(tpn, tps, TRUE) && !sameTemplate(tpn, tps))
                    return FALSE;
            }
            return TRUE;
        }
        else
        {
            return TRUE;
        }
    }
    return FALSE;
}
SYMBOL *searchOverloads(SYMBOL *sp, HASHTABLE *table)
{
    HASHREC *p = table->table[0];
    if (cparams.prm_cplusplus)
    {
        while (p)
        {
            SYMBOL *spp = (SYMBOL *)p->p;
            if (matchOverload(sp->tp, spp->tp, FALSE))
            {
                if (!spp->templateParams)
                    return spp;
                if (sp->templateLevel && !spp->templateLevel && !sp->templateParams->next)
                    return spp;
                if (!!spp->templateParams == !!sp->templateParams)
                {
                    TEMPLATEPARAMLIST *tpl = spp->templateParams->next;
                    TEMPLATEPARAMLIST *tpr = sp->templateParams->next;
                    while (tpl && tpr)
                    {
                        if (tpl->p->type == kw_int && tpl->p->byNonType.tp->type == bt_templateselector)
                            break;
                        if (tpr->p->type == kw_int && tpr->p->byNonType.tp->type == bt_templateselector)
                            break;
                        if (tpl->argsym->compilerDeclared || tpr->argsym->compilerDeclared)
                            break;
                        tpl = tpl->next;
                        tpr = tpr->next;
                    }
                    if (!tpl && !tpr)
                        return spp;
                }
            }
            p = p->next;
        }
    }
    else
    { 
        return (SYMBOL *)p->p;
    }
    return (0);
}
SYMBOL *gsearch(char *name)
{
    SYMBOL *sp = search(name, localNameSpace->syms);
    if (sp)
        return sp;
    return search(name, globalNameSpace->syms);
}
SYMBOL *tsearch(char *name)
{
    SYMBOL *sp = search(name, localNameSpace->tags);
    if (sp)
        return sp;
    return search(name, globalNameSpace->tags);
}
void baseinsert(SYMBOL *in, HASHTABLE *table)
{
    if (cparams.prm_extwarning)
        if (in->storage_class == sc_parameter || in->storage_class == sc_auto ||
            in->storage_class == sc_register)
        {
            SYMBOL *sp;
            if ((sp = gsearch(in->name)) != NULL)
                preverror(ERR_VARIABLE_OBSCURES_VARIABLE_AT_HIGHER_SCOPE, in->name, 
                         sp->declfile, sp->declline);
        }
#if defined (PARSER_ONLY)
    if (AddName(in, table) && table != ccHash)
#else
    if (AddName(in, table))
#endif
    {
#if defined(CPREPROCESSOR) || defined(PARSER_ONLY)
        pperrorstr(ERR_DUPLICATE_IDENTIFIER, in->name);
#else
        if (!structLevel || !templateNestingCount)
        {
            SYMBOL *sym = search(in->name, table);
            if (!sym || !sym->wasUsing || !in->wasUsing)
                preverrorsym(ERR_DUPLICATE_IDENTIFIER, in, in->declfile, in->declline);
        }   
#endif
    }
}
void insert(SYMBOL *in, HASHTABLE *table)
{
    if (table)
    {
#ifdef PARSER_ONLY
        if (table != defsyms && table != kwhash && table != ccHash)
            if (!in->parserSet)
            {
                in->parserSet = TRUE;
                ccSetSymbol(in);
            }
#endif
        baseinsert(in, table);
    }
    else
    {
        diag("insert: cannot insert");
    }
}

void insertOverload(SYMBOL *in, HASHTABLE *table)
{
        
    if (cparams.prm_extwarning)
        if (in->storage_class == sc_parameter || in->storage_class == sc_auto ||
            in->storage_class == sc_register)
        {
            SYMBOL *sp;
            if ((sp = gsearch(in->name)) != NULL)
                preverror(ERR_VARIABLE_OBSCURES_VARIABLE_AT_HIGHER_SCOPE, in->name, 
                         sp->declfile, sp->declline);
        }
    if (AddOverloadName(in, table))
    {
#ifdef CPREPROCESSOR
        pperrorstr(ERR_DUPLICATE_IDENTIFIER, in->name);
#else
        SetLinkerNames(in, lk_cdecl);
    preverrorsym(ERR_DUPLICATE_IDENTIFIER, in, in->declfile, in->declline);
#endif
    }
}

