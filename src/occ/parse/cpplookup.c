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
extern int nextLabel;
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern HASHTABLE *labelSyms;
extern TYPE stdint;
extern LIST *structSyms;
extern SYMBOL *enumSyms;
extern char *overloadNameTab[];

#ifndef CPREPROCESSOR
extern ARCH_DEBUG *chosenDebugger;
extern FILE *listFile;
#endif

HASHTABLE *CreateHashTable(int size);
#define F_WITHCONS 1
#define F_INTEGER 2
#define F_ARITHMETIC 4
#define F_STRUCTURE 8

static SYMBOL *getUserConversion(int flags,
                              TYPE *tpp, TYPE *tpa, EXPRESSION *expa,
                              int *n, enum e_cvsrn *seq, SYMBOL **userFunc);
static BOOL getFuncConversions(SYMBOL *sp, FUNCTIONCALL *f, TYPE *atp, SYMBOL *parent, enum e_cvsrn arr[], int *sizes, int count, SYMBOL **userFunc);

LIST *tablesearchone(char *name, NAMESPACEVALUES *ns, BOOL tagsOnly)
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
static LIST *tablesearchinline(char *name, NAMESPACEVALUES *ns, BOOL tagsOnly)
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
static LIST *namespacesearchone(char *name, NAMESPACEVALUES *ns, LIST *gather, BOOL tagsOnly, BOOL allowUsing)
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
    else if (allowUsing)
    {
        LIST *lst = ns->usingDirectives;
        rv = gather;
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
static LIST *namespacesearchInternal(char *name, NAMESPACEVALUES *ns, BOOL qualified, BOOL tagsOnly,
                                     BOOL allowUsing)
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
SYMBOL *namespacesearch(char *name, NAMESPACEVALUES *ns, BOOL qualified, BOOL tagsOnly)
{
    LIST *lst = namespacesearchInternal(name, ns, qualified, tagsOnly, TRUE);
    
    if (lst)
    {
        while (lst->next)
        {
            // collision
            SYMBOL *test = lst->data;
            LIST *lst1 = lst->next;
            while (lst1)
            {
                if (test != lst1->data)
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
                   BOOL *throughClass, BOOL tagsOnly)
{
    NAMESPACEVALUES *nssym = globalNameSpace;
    SYMBOL *strSym = NULL;
    BOOL qualified = FALSE;
    if (sym)
        *sym = NULL;
    marksym();
    if (ns) *ns = NULL;
    if (MATCHKW(lex, classsel))
    {
        while (nssym->next)
            nssym = nssym->next;
        lex = getsym();
        qualified = TRUE;
    }
    while (ISID(lex))
    {
        char buf[512];
        SYMBOL *sp;
        strcpy(buf, lex->value.s.a);
        lex = getsym();
        if (!MATCHKW(lex, classsel))
        {
            lex = backupsym(1);
            break;
        }
        if (!strSym)
        {
            if (!qualified)
            {
                sp = classsearch(buf, TRUE);
                if (sp)
                    *throughClass = TRUE;
            }
            else
            {
                sp = NULL;
            }
            if (!sp && !qualified)
                sp = namespacesearch(buf, localNameSpace, qualified, TRUE);
            if (!sp)
                sp = namespacesearch(buf, nssym, qualified, TRUE);
        }
        else
        {
            sp = search(buf, strSym->tp->tags);
        }
        if (sp && (basetype(sp->tp)->type == bt_enum || isstructured(sp->tp)))
        {
            strSym = sp;
            if (!qualified)
                nssym = NULL;
        }
        else if (sp && (sp->storage_class == sc_namespace || sp->storage_class == sc_namespacealias))
        {
            nssym = sp->nameSpaceValues;                    
        }
        else
        {
            errorstr(ERR_QUALIFIER_NOT_A_CLASS_OR_NAMESPACE , buf);
            lex = backupsym(0);
            break;
        }
        qualified = TRUE;
        lex = getsym();
    }
    if (qualified)
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
typedef struct _vlist
{
    struct _vlist *next;
    SYMBOL *data;
    BOOL isvirtual;
} VLIST;
typedef struct _classmatch
{
    LIST *syms;
    LIST *cls; // cls->data is a VLIST
    BOOL ishold;
} CLASSMATCH;
static CLASSMATCH *subobjMerge(SYMBOL *cls, BOOL isvirtual, CLASSMATCH *p, CLASSMATCH *q)
{
    LIST *lst2;
    lst2 = p->cls;
    while (lst2)
    {
        VLIST *lst3 = (VLIST *)lst2->data;
        SYMBOL *cur = (SYMBOL *)lst3->data;
        if (!p->ishold && !lst3->isvirtual)
        {
            VLIST *inCls = Alloc(sizeof(VLIST));
            inCls->next = lst2->data;
            lst2->data = inCls;
            inCls->data = cls;
            inCls->isvirtual = isvirtual;
        }
        lst2 = lst2->next;
    }
    lst2 = q->cls;
    while (lst2)
    {
        LIST *listx = lst2;
        VLIST *lst3 = (VLIST *)lst2->data;
        SYMBOL *cur = (SYMBOL *)lst3->data;
        if (!p->ishold && !lst3->isvirtual)
        {
            VLIST *inCls = Alloc(sizeof(VLIST));
            inCls->next = lst2->data;
            lst2->data = inCls;
            inCls->data = cls;
            inCls->isvirtual = isvirtual;
        }
        lst2 = lst2->next;
        
        listx->next = p->cls;
        p->cls = listx;
    }
    return p;
}
BOOL singlebase(SYMBOL *base, SYMBOL *of)
{
    BASECLASS *baseClasses = of->baseClasses;
    while (baseClasses)
    {
        if (base == baseClasses->cls || singlebase(base, baseClasses->cls))
            return TRUE;
        baseClasses = baseClasses->next;
    }
    return FALSE;
}
static BOOL isbase(LIST *base, LIST *of)
{
    while (base)
    {
        LIST *lst2 = of;
        while (lst2)
        {
            SYMBOL *sbase = (SYMBOL *)base->data;
            SYMBOL *sof = (SYMBOL *)lst2->data;
            if (singlebase(sbase, sof))
                break;
            lst2 = lst2->next;
        }
        if (!lst2)
            return FALSE;
        base = base->next;
    }
    return TRUE;
}
static BOOL classisbase(CLASSMATCH *base, CLASSMATCH *of)
{
    LIST *lst1, *lst2;
    lst1 = base->cls;
    while (lst1)
    {
        lst2 = of->cls;
        while (lst2)
        {
            if (isbase((LIST *)lst1->data, (LIST *)lst2->data))
                return TRUE;
            lst2 = lst2->next;
        }
        lst1 = lst1->next;
    }
    return FALSE;
}
static CLASSMATCH *classmerge(SYMBOL *cls, BOOL isvirtual, CLASSMATCH *p, CLASSMATCH *q)
{
    if (p->syms == NULL && p->cls == NULL)
        return q;
    if (q->syms == NULL && q->cls == NULL)
        return p;
    if (classisbase(p, q))
        return q;
    if (classisbase(q, p))
        return p;
    if (p->syms->data != q->syms->data || p->syms->data == NULL || q->syms->data == NULL)
    {
        p->syms->data = NULL;
    }
    p = subobjMerge(cls, isvirtual, p, q);
    return p;        
}
static CLASSMATCH *classdata(char *name, SYMBOL *cls, BOOL isvirtual, BOOL tagsOnly)
{
    SYMBOL *rv = NULL;
    CLASSMATCH *p = NULL;
    if (!tagsOnly)
        rv = search(name, cls->tp->syms);
    if (!rv)
        rv = search(name, cls->tp->tags);
    if (rv)
    {
        VLIST *inCls = Alloc(sizeof(VLIST));
        p = Alloc(sizeof(CLASSMATCH));
        inCls->data = (void *)cls;
        inCls->isvirtual = isvirtual;
        p->syms = Alloc(sizeof(LIST));
        p->syms->data = (void *)rv;
        p->cls = Alloc(sizeof(LIST));
        p->cls->data = (void *)inCls;
    }
    else
    {
        BASECLASS *lst = cls->baseClasses;
        p = Alloc(sizeof(CLASSMATCH));
        while (lst)
        {
            CLASSMATCH *q = classdata(name, lst->cls, lst->isvirtual, tagsOnly);
            p = classmerge(cls, lst->isvirtual, p, q);
            if (p->syms)
            {
                if (!p->ishold && !p->syms->next)
                {
                    SYMBOL *sym = (SYMBOL *)p->syms->data;
                    if (sym)
                    {
                        if (sym->storage_class == sc_static || istype(sym->storage_class) || sym->storage_class == sc_enumconstant)
                            p->ishold = TRUE;
                    }
                }
                if (!p->ishold && !lst->isvirtual && p->syms->data)
                {
                    LIST *lst2 = (LIST *)Alloc(sizeof(LIST));
                    lst2->next = p->syms;
                    p->syms = lst2;
                    lst2->data = (void *)cls;
                }
                p->ishold |= lst->isvirtual;
            }
            lst = lst->next;
        }
    }
    return p;
}
SYMBOL *classsearch(char *name, BOOL tagsOnly)
{
    SYMBOL *rv = NULL;
    if (structSyms)
    {
        SYMBOL *cls = (SYMBOL *)structSyms->data;
        /* optimize for the case where the final class has what we need */
        while (cls)
        {
            if (!tagsOnly)
                rv = search(name, cls->tp->syms);
            if (!rv)
                rv = search(name, cls->tp->tags);
            if (!rv && cls->baseClasses)
            {
                CLASSMATCH *matches = classdata(name, (SYMBOL *)structSyms->data, FALSE, tagsOnly);
                if (matches && matches->syms)
                {
                    if (!matches->syms->data)
                    {
                        errorstr(ERR_AMBIGUOUS_MEMBER_DEFINITION, name);
                        break;
                    }
                    while (matches->syms && matches->syms->next)
                        matches->syms = matches->syms->next;
                    rv = matches->syms->data;
                }
            }
            cls = cls->parentClass;
        }
    }
    return rv;
}
static SYMBOL *finishSearch(char *name, SYMBOL *encloser, NAMESPACEVALUES *ns, BOOL tagsOnly, BOOL throughClass)
{
    SYMBOL *rv = NULL;
    if (!encloser && !ns)
    {
        rv = search(name, localNameSpace->syms);
        if (!rv)
            rv = search(name, localNameSpace->tags);
        if (!rv)
            rv = namespacesearch(name, localNameSpace, FALSE, tagsOnly);
        if (!rv && enumSyms)
            rv = search(name, enumSyms->tp->syms);
        if (!rv)
        {
            rv = classsearch(name, tagsOnly);
            if (rv)
                rv->throughClass = TRUE;
        }
        else
        {
            rv->throughClass = FALSE;
        }
        if (!rv && (!structSyms || ((SYMBOL *)structSyms->data)->nameSpaceValues != globalNameSpace))
        {
            rv = namespacesearch(name, globalNameSpace, FALSE, tagsOnly);
            if (rv)
                rv->throughClass = FALSE;
        }
    }
    else
    {
        if (encloser)
        {
            if (!tagsOnly)
                rv = search(name, encloser->tp->syms);
            if (!rv)
                rv = search(name, encloser->tp->tags);
            if (rv)
                rv->throughClass = throughClass;
        }
        else
        {
            LIST *rvl = tablesearchinline(name, ns, tagsOnly);
            if (rvl)
            {
                rv = rvl->data;
                rv->throughClass = FALSE;
            }
        }
    }
}
LEXEME *nestedSearch(LEXEME *lex, SYMBOL **sym, SYMBOL **strSym, NAMESPACEVALUES **nsv, BOOL *destructor, BOOL tagsOnly)
{
    SYMBOL *encloser = NULL;
    NAMESPACEVALUES *ns = NULL;
    BOOL throughClass = FALSE;
    *sym = NULL;
    
    if (!cparams.prm_cplusplus)
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
    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly);
    if (cparams.prm_cplusplus && MATCHKW(lex, compl))
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
    if (ISID(lex))
    {
        *sym = finishSearch(lex->value.s.a, encloser, ns, tagsOnly, throughClass);
    }
    else if (destructor)
    {
        *destructor = FALSE;
        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
    }
    if (encloser&& strSym)
        *strSym = encloser;
    if (nsv)
        if (ns && ns->name)
            *nsv = ns;
        else
            *nsv = NULL;
    else if (!*sym)
        backupsym(0);
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
                        lex = backupsym(1);
                    }
                    break ;
                case openbr:
                    lex = getsym();
                    if (!MATCHKW(lex, closebr))
                    {
                        needkw(&lex, closebr);
                        lex = backupsym(1);
                    }
                    break ;
                case kw_new:
                case kw_delete:
                    lex = getsym();
                    if (!MATCHKW(lex, openbr))
                    {
                        lex = backupsym(1);
                    }
                    else
                    {
                        kw = kw - kw_new + compl + 1;
                        lex = getsym();
                        if (!MATCHKW(lex,closebr))
                        {
                            needkw(&lex, closebr);
                            lex = backupsym(1);
                        }
                    }
                    break;
            }
            strcpy(buf, overloadNameTab[*ov = kw - kw_new + CI_NEW]);
        }
        else if (startOfType(lex)) // potential cast operator
        {
            TYPE *tp = NULL;
            lex = get_type_id(lex, &tp, funcsp, TRUE);
            if (castType)
                *castType = tp;
            strcpy(buf, overloadNameTab[*ov = CI_CAST]);
        }
        else if (lex->type == l_astr)
        {
            SLCHAR *xx = (SLCHAR *)lex->value.s.w;
            if (xx->count)
                error(ERR_OPERATOR_LITERAL_EMPTY_STRING);
            lex = getsym();
            
            if (ISID(lex))
            {
                sprintf(buf, "%s@%s", overloadNameTab[CI_LIT], lex->value.s.a);
                *ov = CI_LIT;
            }
            else
            {
                error(ERR_OPERATOR_LITERAL_NEEDS_ID); 
                backupsym(2);
            }
        }
        else
        {
            if (ISKW(lex))
                errorstr(ERR_INVALID_AS_OPERATOR, lex->kw->name);
            else
                errorstr(ERR_INVALID_AS_OPERATOR,"");
            lex = backupsym(1);
        }        
    }
    return lex;
}
LEXEME *id_expression(LEXEME *lex, SYMBOL *funcsp, SYMBOL **sym, SYMBOL **strSym, NAMESPACEVALUES **nsv, BOOL tagsOnly, char *idname)
{
    SYMBOL *encloser = NULL;
    NAMESPACEVALUES *ns = NULL;
    BOOL throughClass = FALSE;
    TYPE *castType = NULL;
    char buf[512];
    int ov = 0;
    *sym = NULL;
    
    if (!cparams.prm_cplusplus)
    {
        if (ISID(lex))
        {
            if (idname)
                strcpy(idname, lex->value.s.a);
            if (tagsOnly)
                *sym = tsearch(lex->value.s.a);
            else
            {
                if (structSyms)
                {
                    *sym = search(lex->value.s.a, ((SYMBOL *)structSyms->data)->tp->syms);
                }
                if (*sym == NULL)
                    *sym = gsearch(lex->value.s.a);
            }
        }
        return lex;
    }
    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly);
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
                                    encloser, ns, tagsOnly, throughClass);
            }
        }
        else
        {
            error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
        }
    }
    else
    {
        lex = getIdName(lex, funcsp, buf, &ov, &castType);
        if (buf[0])
        {
            *sym = finishSearch(ov == CI_CAST ? overloadNameTab[CI_CAST] : buf, 
                                encloser, ns, tagsOnly, throughClass);
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
        backupsym(0);
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
static BOOL isFriend(SYMBOL *cls, SYMBOL *frnd)
{
    if (frnd)
    {
        LIST *l = cls->friends;
        while (l)
        {
            if ((SYMBOL *)l->data == frnd)
                return TRUE;
            l = l->next;
        }
    }
    return FALSE;
}
// works by searching the tree for the base or member symbol, and stopping any
// time the access wouldn't work.  If the symbol is found it is accessible.
BOOL isAccessible(SYMBOL *derived, SYMBOL *currentBase, 
                                 SYMBOL *member, SYMBOL *funcsp, 
                                 enum e_ac minAccess, BOOL asAddress)
{
    BASECLASS *lst;
    HASHREC *hr;
    enum e_ac consideredAccess = minAccess;
    if (!cparams.prm_cplusplus)
        return TRUE;
    if (isFriend(currentBase, funcsp) || structSyms && isFriend(currentBase, (SYMBOL *)structSyms->data))
        consideredAccess = ac_private;
    if (!currentBase->tp->syms)
        return FALSE;
    hr = currentBase->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym == member)
            break;
        if (sym->storage_class == sc_overloads && isfunction(member->tp))
        {
            HASHREC *hr1 = sym->tp->syms->table[0];
            while (hr1)
            {
                if ((SYMBOL *)hr1->p == member)
                {
                    break;
                }
                hr1 = hr1->next;
            }
            if (hr1)
            {
                hr = hr1;
                break;
            }
        }
        hr = hr->next;
    }
    if (!hr)
    {
        hr = currentBase->tp->tags->table[0];
        while (hr)
        {
            if ((SYMBOL *)hr->p == member)
                break;
            hr = hr->next;
        }
    }
    if (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        BOOL test = TRUE;
        // this is to take care of a special case with member pointers
        if (asAddress && sym->access == ac_protected)
        {
            SYMBOL *xx = sym->parentClass;
            test = FALSE;
            while (xx)
            {
                if (xx == derived)
                {
                    test = TRUE;
                    break;
                }
                xx = xx->parentClass;
            }
        }
        return derived == currentBase && consideredAccess <= ac_protected 
                || sym->access >= consideredAccess && test;
    }
    lst = currentBase->baseClasses;
    while (lst)
    {
        if (lst->accessLevel == ac_private)
            consideredAccess = ac_none;
        else if (lst->accessLevel == ac_protected && minAccess == ac_public)
            consideredAccess = ac_none;
        else
            consideredAccess = minAccess;
        // we have to go through the base classes even if we know that a normal
        // lookup wouldn't work, so we can check their friends lists...
        if (isAccessible(derived, lst->cls, member, funcsp, consideredAccess, asAddress))
            return TRUE;
        lst = lst->next;
    }
    return FALSE;
}
BOOL isExpressionAccessible(SYMBOL *sym, SYMBOL *funcsp, BOOL asAddress)
{
    if (sym->parentClass)
        if (structSyms && sym->throughClass && ((SYMBOL *)structSyms->data == sym->parentClass || classRefCount(sym->parentClass, (SYMBOL *)structSyms->data)))
        {
            if (!isAccessible((SYMBOL *)structSyms->data, (SYMBOL *)structSyms->data,
                              sym, funcsp, ac_protected, asAddress))
                return FALSE;
        }
        else
        {
            if (!isAccessible(sym->parentClass, sym->parentClass, sym, funcsp, ac_public, asAddress))
                return FALSE;
        }
    return TRUE;    
}
BOOL checkDeclarationAccessible(TYPE *tp, SYMBOL *funcsp)
{
    while (tp)
    {
        if (isstructured(tp) || tp->type == bt_enum)
        {
            SYMBOL *sym = basetype(tp)->sp;
            if (sym->parentClass)
            {
                if (structSyms && ((SYMBOL *)structSyms->data == sym->parentClass || classRefCount(sym->parentClass, (SYMBOL *)structSyms->data)))
                {
                    if (!isAccessible((SYMBOL *)structSyms->data, (SYMBOL *)structSyms->data,
                                      sym, funcsp, ac_protected, FALSE))
                    {
                        errorsym(ERR_CANNOT_ACCESS, tp->sp);		
                        return FALSE;
                    }
                }
                else
                {
                    if (!isAccessible(sym->parentClass, sym->parentClass, sym, funcsp, ac_public, FALSE))
                    {
                        errorsym(ERR_CANNOT_ACCESS, tp->sp);		
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
    return getUserConversion(F_STRUCTURE, tp, sp->tp, NULL, NULL, NULL, NULL);
}
SYMBOL *lookupIntCast(SYMBOL *sp, TYPE *tp)
{
    return getUserConversion(F_INTEGER, tp, sp->tp, NULL, NULL, NULL, NULL);
}
SYMBOL *lookupArithmeticCast(SYMBOL *sp, TYPE *tp)
{
    return getUserConversion(F_ARITHMETIC, tp, sp->tp, NULL, NULL, NULL, NULL);
}
static LIST *structuredArg(SYMBOL *sp, LIST *in, TYPE *tp)
{
    return searchNS(sp, basetype(tp)->sp->parentNameSpace, in) ;
}
static LIST *searchOneArg(SYMBOL *sp, LIST *in, TYPE *tp);
static LIST *funcArg(SYMBOL *sp, LIST *in, TYPE *tp)
{
    HASHREC **hr = tp->syms->table;
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
        return searchNS(sp, tp->sp->parentNameSpace, in) ;
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
static const int rank[] = 
{
    0,0,0,0,0,1,1,2,2,2,2,2,2,3,4,5,6,6,7
};
enum e_cvsrn
{
    // tier 1
    CV_IDENTITY,
    CV_LVALUETORVALUE,
    CV_ARRAYTOPOINTER,
    CV_FUNCTIONTOPOINTER,
    CV_QUALS,
    CV_INTEGRALPROMOTION,
    CV_FLOATINGPROMOTION,
    CV_INTEGRALCONVERSION,
    CV_FLOATINGCONVERSION,
    CV_FLOATINGINTEGRALCONVERSION,
    CV_POINTERCONVERSION,
    CV_POINTERTOMEMBERCONVERSION,
    CV_PTRBOOLCONVERSION, 
    CV_DERIVEDFROMBASE,
    // tier 2
    CV_USER,
    // tier 3
    CV_ELLIPSIS,
    // other
    CV_PAD,
    CV_AMBIGUOUS,
    CV_NONE,
} ;
static void  GatherConversions(SYMBOL *sp, SYMBOL **spList, int n, FUNCTIONCALL *args, 
                               TYPE *atp, enum e_cvsrn **icsList, int **lenList, int argCount, SYMBOL **funcList)
{
    int i;
    for (i=0; i < n; i++)
    {
        int j;
        if (spList[i])
        {
            enum e_cvsrn arr[500][3];
            int counts[500];
            SYMBOL **funcs[200];
            BOOL t;
            memset(counts, 0, argCount * sizeof(int));
            for (j = i + 1; j < n; j++)
                if (spList[i] == spList[j])
                    spList[j] = 0;
            memset(funcs, 0, sizeof(funcs));
            t = getFuncConversions(spList[i], args, atp, sp->parentClass, arr, counts, argCount, funcs);
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
static BOOL ismem(EXPRESSION *exp)
{
    switch (exp->type)
    {
        case en_global:
        case en_label:
        case en_pc:
        case en_auto:
        case en_threadlocal:
            return TRUE;
        case en_add:
        case en_sub:
            return ismem(exp->left) || ismem(exp->right);
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
    return tpx;
}
static int compareConversions(SYMBOL *spLeft, SYMBOL *spRight, enum e_cvsrn *seql, enum e_cvsrn *seqr,
                              TYPE *ltype, TYPE *rtype, TYPE *atype, EXPRESSION *expa, 
                              SYMBOL *funcl, SYMBOL *funcr, 
                              int lenl, int lenr, BOOL fromUser)
{
    enum e_ct xl=conv, xr=conv;
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
        for (i=0; i < lenl && i < lenr; i++)
        {
            switch (seql[l])
            {   
                case CV_LVALUETORVALUE:
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    i++;
                    continue;
            }
            switch (seqr[r])
            {   
                case CV_LVALUETORVALUE:
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    r++;
                    continue;
            }
            if (seql[l] != seql[r])
                break;
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
            if (rank[seql[l]] > rankl)
                rankl = rank[seql[l]];
        rankr = CV_IDENTITY;
        for (r=0; r < lenr; r++)
            if (rank[seqr[r]] > rankr)
                rankr = rank[seqr[r]];
        if (rankl < rankr)
            return -1;
        else if (rankr < rankl)
            return 1;
        else // ranks are same, do same rank comparisons
        {
            TYPE *tl = ltype, *tr = rtype, *ta = atype;
            // check if one or the other but not both converts a pointer to bool
            rankl = 0;
            for (l=0; l < lenl; l++)
                if (seql[l] == CV_PTRBOOLCONVERSION)
                    rankl = 1;
            rankr = 0;
            for (r=0; r < lenr; r++)
                if (seqr[r] == CV_PTRBOOLCONVERSION)
                    rankr = 1;
            if (rankl != rankr)
                if (rankl)
                    return 1;
                else 
                    return -1;
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
                int lref = expa && (lvalue(expa) && (expa->left->type != en_l_ref || !expa->left->rref));
                int rref = expa && (expa->type == en_l_ref && expa->left->rref || !lvalue(expa) && (!isstructured(rtype) || !ismem(expa)) );
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
                    if (lref)
                        return 1;
                    else if (rref)
                        return -1;
                else 
                    if (basetype(rtype)->type == bt_lref)
                        if (lref)
                            return 1;
                        else if (rref)
                            return -1;
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
        int l=0, r=0;
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
            if (!comparetypes(ltype, rtype, TRUE))
                return 0;
        }
        if (seql[l] = CV_USER && seqr[r] == CV_USER && funcl && funcr)
        {
            int v1, v2;
            TYPE *atype1, *atype2;
            if (funcl->castoperator)
            {
                ltype = basetype(funcl->tp)->btp;
                atype1 = ((SYMBOL *)basetype(funcl->tp)->syms->table[0]->p)->tp;
            }
            else
            {
                ltype = ((SYMBOL *)basetype(funcl->tp)->syms->table[0]->p)->tp;
                atype1 = basetype(funcl->tp)->btp;
            }
            if (funcr->castoperator)
            {
                rtype = basetype(funcr->tp)->btp;
                atype2 = ((SYMBOL *)basetype(funcr->tp)->syms->table[0]->p)->tp;
            }
            else
            {
                rtype = ((SYMBOL *)basetype(funcr->tp)->syms->table[0]->p)->tp;
                atype2 = basetype(funcr->tp)->btp;
            }
            v1 = compareConversions(funcl, funcr, seql +l, seqr + r, 
                                    ltype, rtype, atype1, NULL, NULL, NULL, lenl-l, lenr-r, TRUE);
            v2 = compareConversions(funcl, funcr, seql +l, seqr + r, 
                                    ltype, rtype, atype2, NULL, NULL, NULL, lenl-l, lenr-r, TRUE);
            if (v1 != v2)
                if (v1 == 0)
                    return 1;
                else if (v2 == 0)
                    return -1;
                else
                    return 0;
            else
                return v1;                    
        }
    }
    // ellipse always returns 0;
    return 0;
}
static void SelectBestFunc(SYMBOL ** spList, enum e_cvsrn **icsList, 
                                int **lenList, FUNCTIONCALL *funcparams,
                                int argCount, int funcCount, SYMBOL ***funcList)
{
    static const enum e_cvsrn identity = CV_IDENTITY;
    char arr[500];
    int i, j;
    for (i=0; i < funcCount; i++)
    {
        for (j=i+1; j < funcCount && spList[i]; j++)
        {
            if (spList[j])
            {
                int left=0, right=0;
                int l=0,r=0;
                int k=0;
                ARGLIST *args = funcparams->arguments;
                HASHREC *hrl = basetype(spList[i]->tp)->syms->table[0];
                HASHREC *hrr = basetype(spList[j]->tp)->syms->table[0];
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
                    if (k == 0 && funcparams->thisptr)
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
                    else if (k == 1 && funcparams->thisptr)
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
                    else
                    {
                        TYPE *tpl = hrl ? ((SYMBOL *)hrl->p)->tp : NULL;
                        TYPE *tpr = hrr ? ((SYMBOL *)hrr->p)->tp : NULL;
                        arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, 
                                                    args? args->tp:0, args?args->exp:0,
                                                    funcList ? funcList[i][k] : NULL, 
                                                    funcList ? funcList[j][k] : NULL,
                                                    lenl, lenr, FALSE);
                        if (hrl) hrl = hrl->next;
                        if (hrr) hrr = hrr->next;
                    }
                    l += lenl;
                    r += lenr;
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
static LIST *GetMemberCasts(LIST *gather, SYMBOL *sp)
{
    if (sp)
    {
        BASECLASS *bcl = sp->baseClasses;
        SYMBOL *sym = sp;
        while (sym)
        {
            // conversion of one class to another
            SYMBOL *find = search(overloadNameTab[CI_CAST], basetype(sym->tp)->syms);
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
static void getSingleConversion(TYPE *tpp, TYPE *tpa, EXPRESSION *expa, int *n, enum e_cvsrn *seq, SYMBOL **userFunc);
static SYMBOL *getUserConversion(int flags,
                          TYPE *tpp, TYPE *tpa, EXPRESSION *expa,
                              int *n, enum e_cvsrn *seq, SYMBOL **userFunc)
{
    static BOOL infunc = FALSE;
    if (!infunc)
    {
        LIST *gather = NULL;
        infunc = TRUE;
        if (flags & F_WITHCONS)
        {
            TYPE *tppp = tpp;
            if (isref(tppp))
                tppp = basetype(tppp)->btp;
            if (isstructured(tppp))
            {
                gather = GetMemberConstructors(gather, basetype(tppp)->sp);
            }
        }
        gather = GetMemberCasts(gather, basetype(tpa)->sp);
        if (gather)
        {
            LIST *lst2;
            int funcs = 0;
            int i;
            SYMBOL **spList;
            enum e_cvsrn **icsList;
            int **lenList;
            int m = 0;
            SYMBOL *found1, *found2;
            FUNCTIONCALL funcparams;
            ARGLIST args;
            TYPE thistp;
            EXPRESSION exp;
            lst2 = gather;
            memset(&funcparams, 0, sizeof(funcparams));
            memset(&args, 0, sizeof(args));
            memset(&thistp, 0, sizeof(thistp));
            memset(&exp, 0, sizeof(exp));
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
            icsList = (enum e_cvsrn **)Alloc(sizeof(enum e_cvsrn *) * funcs);
            lenList = (int **)Alloc(sizeof(int *) * funcs);
            lst2 = gather;
            i = 0 ;
            while (lst2)
            {
                HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                while (*hr)
                {
                    spList[i++] = (SYMBOL *)(*hr)->p;
                    hr = &(*hr)->next;
                }
                lst2 = lst2->next;
            }
            memset(&exp, 0, sizeof(exp));
            exp.type = en_not_lvalue;
            for (i=0; i < funcs; i++)
            {
                SYMBOL *candidate = spList[i];
                int j;
                int n3 = 0, n2 = 0, m1;
                enum e_cvsrn seq3[50];
                if (candidate->castoperator)
                {
                    TYPE *tpc = basetype(candidate->tp)->btp;
                    if (isref(tpc))
                        tpc = basetype(tpc)->btp;
                    if ((flags & F_INTEGER) && !isint(tpc)
                        || (flags & F_ARITHMETIC) && !isarithmetic(tpc)
                        || (flags & F_STRUCTURE) && !isstructured(tpc))
                    {
                        seq3[n2++] = CV_NONE;
                        seq3[n2+n3++] = CV_NONE;
                    }
                    else
                    {
                        HASHREC *args = basetype(candidate->tp)->syms->table[0];
                        BOOL lref = FALSE;
                        TYPE *tpn = basetype(candidate->tp)->btp;
                        if (isref(tpn))
                        {
                            if (basetype(tpn)->type == bt_lref)
                                lref = TRUE;
                        }
                        thistp.btp = tpa;
                        thistp.type = bt_pointer;
                        thistp.size = getSize(bt_pointer);
                        getSingleConversion(((SYMBOL *)args->p)->tp, &thistp, &exp, &n2, seq3, NULL);
                        seq3[n2+ n3++] = CV_USER;
                        getSingleConversion(tpp, basetype(candidate->tp)->btp, lref ? NULL : &exp, &n3, seq3 + n2, NULL);
                    }
                }
                else
                {
                    HASHREC *args = basetype(candidate->tp)->syms->table[0];
                    if (args)
                    {
                        SYMBOL *first, *next=NULL;
                        SYMBOL *th = (SYMBOL *)args->p;
                        args = args->next;
                        first = (SYMBOL *)args->p;
                        if (args->next)
                            next = (SYMBOL *) args->next->p;
                        if (!next || next->init)
                        {
                            getSingleConversion(first->tp, tpa, expa, &n2, seq3, &exp);
                            if (n2 && seq3[n2-1] == CV_IDENTITY)
                            {
                                n2--;
                            }
                            seq3[n2+n3++] = CV_USER;
                            getSingleConversion(tpp, basetype(basetype(th->tp)->btp)->sp->tp, &exp, &n3, seq3+n2, NULL);
                        }
                    }
                }
                for (j=0; j < n2+n3; j++)
                    if (seq3[j] == CV_NONE)
                        break;
                m1 = n2+n3;
                while (m1 && seq3[m1-1] == CV_IDENTITY)
                    m1--;
                if (j >= n2+n3 && m1 < 4)
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
            funcparams.arguments = &args;
            args.tp = tpa;
            funcparams.thisptr = expa;
            funcparams.thistp = &thistp;
            thistp.btp = tpa;
            thistp.type = bt_pointer;
            thistp.size = getSize(bt_pointer);
            SelectBestFunc(spList, icsList, lenList, &funcparams, 2, funcs, NULL);
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
                if (found2)
                {
                     errorsym2(ERR_AMBIGUITY_BETWEEN, found1, found2);
                }
                else
                {
                    if (seq)
                    {
                        int l = lenList[m][0] + lenList[m][1];
                        memcpy(&seq[*n], &icsList[m][0], l * sizeof(enum e_cvsrn));
                        *n += l;
                        if (userFunc)
                            *userFunc = found1;
                    }
                    infunc = FALSE;
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
    BOOL hasconst = TRUE, hasvol = TRUE;
    BOOL sameconst = TRUE, samevol = TRUE;
    BOOL first = TRUE;
    while (tpa && tpp)
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
    if (!tpa && !tpp)
    {
        if (!sameconst || !samevol)
            seq[(*n)++] = CV_QUALS;
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
        if (basetype(tpp)->btp->type == bt_void)
        {
            seq[(*n)++] = CV_POINTERCONVERSION;
        }
        else if (isstructured(basetype(tpp)->btp) && isstructured(basetype(tpa)->btp))
        {
            SYMBOL *base = basetype(basetype(tpp)->btp)->sp;
            SYMBOL *derived = basetype(basetype(tpa)->btp)->sp;
            if (base != derived)
            {
                int v = classRefCount(base, derived);
                if (v != 1)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    seq[(*n)++]= CV_POINTERCONVERSION;
                }
            }
            else
            {
                seq[(*n)++] = CV_IDENTITY;
            }
        }
        else if (!comparetypes(tpp, tpa, TRUE))
        {
            seq[(*n)++] = CV_NONE;
        }
        getQualConversion(tpp, tpa, n, seq);
    }
}
static void getSingleConversion(TYPE *tpp, TYPE *tpa, EXPRESSION *expa, int *n, enum e_cvsrn *seq, SYMBOL **userFunc)
{
    BOOL lref;
    BOOL rref;
    if (tpp->type != tpa->type && (tpp->type == bt_void || tpa->type == bt_void))
    {
         seq[(*n)++] = CV_NONE;
         return;
    }
    lref = expa && (lvalue(expa) && (expa->left->type != en_l_ref || !expa->left->rref));
    rref = expa && (expa->type == en_l_ref && expa->left->rref || !lvalue(expa) && (!isstructured(tpa) || !ismem(expa)) );
    if (expa && expa->type == en_func)
    {
        TYPE *tp = basetype(expa->v.func->sp->tp)->btp;
        if (tp)
        {
            if (tp->type == bt_rref)
            {
                rref = TRUE;
                lref = FALSE;
            }
            else if (tp->type == bt_lref)
            {
                lref = TRUE;
                rref = FALSE;
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
    }
    if (isref(tpp))
    {
        TYPE *tppp = basetype(tpp)->btp;
        lref |= expa && isstructured(tppp) && expa->type != en_not_lvalue;
        if (isconst(tppp) != isconst(tpa)
            || isvolatile(tppp) != isvolatile(tpa))
            seq[(*n)++] = CV_QUALS;
        if (lref && tppp->type == bt_rref)
            seq[(*n)++] = CV_LVALUETORVALUE;
        if (tpp->type == bt_rref && lref && !isfunction(tpa))
        {
            // lvalue to rvalue ref not allowed unless the lvalue is nonvolatile and const
            seq[(*n)++] = CV_NONE;
        }
        else if (tpp->type == bt_lref && rref && (!isconst(tppp) || isvolatile(tppp)))
        {
            // rvalue to lvalue ref not allowed unless the lvalue is a function
            seq[(*n)++] = CV_NONE;
        }
        tpa = basetype(tpa);
        if (isstructured(tpa))
        {
            if (isstructured(tppp))
            {
                if (basetype(tpa)->sp != basetype(tppp)->sp)
                {
                    if (classRefCount(basetype(tppp)->sp, basetype(tpa)->sp) == 1)
                    {
                        seq[(*n)++] = CV_DERIVEDFROMBASE;
                    }
                    else
                    {
                        getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, userFunc);
                    }
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                getUserConversion(0, tpp, tpa, expa, n, seq, userFunc);
            }
        }
        else if (isstructured(tppp))
        {
            seq[(*n)++] = CV_NONE;
        }
        else
        {
            getSingleConversion(tppp, tpa, expa, n, seq, userFunc);
        }
    }
    else
    {
        if (isstructured(tpa))
        {
            if (isstructured(tpp))
            {
                if (basetype(tpa)->sp == basetype(tpp)->sp)
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
                else if (classRefCount(basetype(tpp)->sp, basetype(tpa)->sp) == 1)
                {
                    seq[(*n)++] = CV_DERIVEDFROMBASE;
                }
                else
                {
                    getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, userFunc);
                }
            }
            else
            {
                getUserConversion(0, tpp, tpa, expa, n, seq, userFunc);
            }
        }
        else if (isstructured(tpp))
        {
            seq[(*n)++] = CV_NONE;
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
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (ispointer(tpa))
        {
            if (basetype(tpp)->type == bt_bool)
            {
                seq[(*n)++] = CV_PTRBOOLCONVERSION;
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
                            getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, userFunc);
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
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
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
                    seq[(*n)++] = CV_NONE;
            }
            else
            { 
                if ((basetype(tpp)->type == bt_int || basetype(tpp)->type == bt_unsigned) && !basetype(tpa)->scoped)
                {
                    seq[(*n)++] = CV_INTEGRALPROMOTION;
                }
                else
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
        else if (basetype(tpp)->type == bt_enum)
        {
            if (basetype(tpa)->type == bt_int || basetype(tpa)->type == bt_unsigned)
            {
                if (basetype(tpp)->btp->type != basetype(tpa)->type)
                {
                    seq[(*n)++] = CV_INTEGRALPROMOTION;
                }
            }
            else
            {
                seq[(*n)++] = CV_NONE;                
            }
        }
        else
        {
            if (basetype(tpp)->type != basetype(tpa)->type)
            {
                if (isint(tpa))
                    if ((basetype(tpp)->type == bt_int || basetype(tpp)->type == bt_unsigned) && basetype(tpa)->type < basetype(tpp)->type)
                        seq[(*n)++]= CV_INTEGRALPROMOTION;
                    else if (isint(tpp))
                        seq[(*n)++]= CV_INTEGRALCONVERSION;
                    else
                        seq[(*n)++]= CV_FLOATINGINTEGRALCONVERSION;
                        
                else /* floating */
                    if (basetype(tpp)->type == bt_double && basetype(tpa)->type == bt_float)
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
static BOOL getFuncConversions(SYMBOL *sp, FUNCTIONCALL *f, TYPE *atp, SYMBOL *parent, enum e_cvsrn arr[], int *sizes, int count, SYMBOL **userFunc)
{
    int pos = 0;
    int n = 0;
    int i;
    ARGLIST *a = NULL;
    HASHREC **hr = basetype(sp->tp)->syms->table;
    HASHREC **hrt = NULL;
    enum e_cvsrn seq[100], cur;
    int m = 0,m1;
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
        m = 0;
        getSingleConversion(parent->tp, basetype(sp->tp)->btp, NULL,
                                    &m, seq, userFunc ? &userFunc[n] : NULL);
        m1 = m;
        while (m1 && seq[m1-1] == CV_IDENTITY)
            m1--;
        if (m1 > 3)
        {
            return FALSE;
        }
        for (i=0; i < m; i++)
            if (seq[i] == CV_NONE)
                return FALSE;
        memcpy(arr+pos, seq, 3 * sizeof(enum e_cvsrn));
        sizes[n++] = m;
        pos += m;
        hr = &(*hr)->next;
        tpp = argsym->tp;
        tpx.type = bt_pointer;
        tpx.size = getSize(bt_pointer);
        tpx.btp = f->arguments->tp;
        m = 0;
        getSingleConversion(tpp, &tpx, f->thisptr, &m, seq, userFunc ? &userFunc[n] : NULL);
        m1 = m;
        while (m1 && seq[m1-1] == CV_IDENTITY)
            m1--;
        if (m1 > 3)
        {
            return FALSE;
        }
        for (i=0; i < m; i++)
            if (seq[i] == CV_NONE)
                return FALSE;
        memcpy(arr+pos, seq, 3 * sizeof(enum e_cvsrn));
        sizes[n++] = m;
        pos += m;
        return TRUE;
    }
    else
    {
        if (f)
        {
            if (f->thisptr)
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
                    hr = &(*hr)->next;
                    tpp = argsym->tp;
                    if (sp->castoperator)
                    {
                        tpthis = &tpx;
                        tpx.type = bt_pointer;
                        tpx.size = getSize(bt_pointer);
                        tpx.btp = f->arguments->tp;
                    }
                    m = 0;
                    getSingleConversion(tpp, tpthis, f->thisptr, &m, seq, userFunc ? &userFunc[n] : NULL);
                    m1 = m;
                    while (m1 && seq[m1-1] == CV_IDENTITY)
                        m1--;
                    if (m1 > 3)
                    {
                        return FALSE;
                    }
                    for (i=0; i < m; i++)
                        if (seq[i] == CV_NONE)
                            return FALSE;
                    memcpy(arr+pos, seq, 3 * sizeof(enum e_cvsrn));
                    sizes[n++] = m;
                    pos += m;
                }
            }
        }
        else
        {
            if (sp->storage_class == sc_member || sp->storage_class == sc_virtual)
            {
                TYPE *argtp = sp->parentClass->tp;
                if (!argtp)
                {
                    return FALSE;
                }
                else 
                {
                    getSingleConversion(argtp, a ? a->tp : ((SYMBOL *)(*hrt)->p)->tp, a ? a->exp : NULL, 
                                        &m, seq, userFunc ? &userFunc[n] : NULL);
                    if (a)
                        a = a->next;
                    else if (hrt)
                        hrt = &(*hrt)->next;
                }
            }
        }
        while (*hr && (a || hrt))
        {
            SYMBOL *argsym = (SYMBOL *)(*hr)->p;
            if (argsym->constop)
                break;
            if (argsym->storage_class != sc_parameter)
                return FALSE;
            if (argsym->tp->type == bt_ellipse)
            {
                arr[n++] = CV_ELLIPSIS;
                return TRUE;
            }
            m = 0;
            getSingleConversion(argsym->tp, a ? a->tp : ((SYMBOL *)(*hrt)->p)->tp, a ? a->exp : NULL, &m, seq,
                                userFunc ? &userFunc[n] : NULL);
            m1 = m;
            while (m1 && seq[m1-1] == CV_IDENTITY)
                m1--;
            if (m1 > 3)
            {
                return FALSE;
            }
            for (i=0; i < m; i++)
                if (seq[i] == CV_NONE)
                    return FALSE;
            memcpy(arr+pos, seq, 3 * sizeof(enum e_cvsrn));
            sizes[n++] = m;
            pos += m;
            hr = &(*hr)->next;
            if (a)
                a = a->next;
            else
                hrt = &(*hrt)->next;
        }
        if (*hr)
        {
            SYMBOL *sym = (SYMBOL *)(*hr)->p;
            if (sym->init)
            {
                return TRUE;
            }
            if (sym->tp->type == bt_ellipse)
            {
                sizes[n++] = 1;
                arr[pos++] = CV_ELLIPSIS;
                return TRUE;
            }
            if (sym->tp->type == bt_void)
                return TRUE;
            return FALSE;
        }
        return a == NULL;
    }
}
SYMBOL *GetOverloadedFunction(TYPE **tp, EXPRESSION **exp, SYMBOL *sp, 
                              FUNCTIONCALL *args, TYPE *atp, BOOL toErr, 
                              BOOL maybeConversion)
{
    if (atp && ispointer(atp))
        atp = basetype(atp)->btp;
    if (atp && !isfunction(atp))
        atp = NULL;
    if (!sp || sp->storage_class == sc_overloads)
    {
        LIST *gather = NULL;
        SYMBOL **flatList;    
        SYMBOL *found1 = NULL, *found2 = NULL;
        if (!cparams.prm_cplusplus)
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
                if (!sp->tp || !sp->wasUsing && !sp->parentClass)
                {
                        // ok the sym is a valid candidate for argument search
                    if (args)
                    {
                        ARGLIST *list = args->arguments;
                        while (list)
                        {
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
                LIST *lst = Alloc(sizeof(LIST));
                lst->data = sp;
                lst->next = gather;
                gather = lst;
            }
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
            if (args->arguments && !args->arguments->next) // one arg
                gather = GetMemberCasts(gather, basetype(args->arguments->tp)->sp);
        }
        // pass 3 - the actual argument-based resolution
        if (gather)
        {
            LIST *lst2;
            int n = 0;
            lst2 = gather;
            while (lst2)
            {
                HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                while (*hr)
                {
                    n++;
                    hr = &(*hr)->next;
                }
                lst2 = lst2->next;
            }
            if (args || atp)
            {
                int i;
                SYMBOL **spList;
                SYMBOL ***funcList;
                enum e_cvsrn **icsList;
                int **lenList;
                BOOL done = FALSE;
                int argCount = 0;
                if (args)
                {
                    ARGLIST *v = args->arguments;
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
                    if (*hr && (((SYMBOL *)(*hr)->p)->storage_class == sc_member || ((SYMBOL *)(*hr)->p)->storage_class == sc_virtual))
                        argCount++;
                }
                spList = (SYMBOL **)Alloc(sizeof(SYMBOL *) * n);
                icsList = (enum e_cvsrn **)Alloc(sizeof(enum e_cvsrn *) * n);
                lenList = (int **)Alloc(sizeof(int *) * n);
                funcList = (int **)Alloc(sizeof(SYMBOL **) * n);
                lst2 = gather;
                n = 0;
                while (lst2)
                {
                    HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                    while (*hr)
                    {
                        spList[n++] = (SYMBOL *)(*hr)->p;
                        hr = &(*hr)->next;
                    }
                    lst2 = lst2->next;
                }
                GatherConversions(sp, spList, n, args, atp, icsList, lenList, argCount, funcList);
                SelectBestFunc(spList, icsList, lenList, args, argCount, n, funcList);
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
        if (toErr)
        {
            if (!found1)
            {
                if (args->arguments && !args->arguments->next // one arg
                    && !strcmp(sp->name, overloadNameTab[CI_CONSTRUCTOR])) // conversion constructor
                {
                    errortype(ERR_CANNOT_CONVERT_TYPE, args->arguments->tp, sp->parentClass->tp);
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
                        ARGLIST *a = args->arguments;
                        sym->tp = Alloc(sizeof(TYPE));
                        sym->tp->type = bt_func;
                        sym->tp->size = getSize(bt_pointer);
                        sym->tp->btp = &stdint;
                        sym->tp->syms = CreateHashTable(1);
                        while (a)
                        {
                            SYMBOL *sym1 = Alloc(sizeof(SYMBOL));
                            char nn[10];
                            sprintf(nn, "%d", v++);
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
                errorsym2(ERR_AMBIGUITY_BETWEEN, found1, found2);
            }
            else if (found1->deleted)
            {
                errorsym(ERR_DELETED_FUNCTION_REFERENCED, found1);
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
    return sp;
}
