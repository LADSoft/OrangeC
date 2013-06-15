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
static BOOL singlebase(SYMBOL *base, SYMBOL *of)
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
LEXEME *getIdName(LEXEME *lex, char *buf, int *ov)
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
LEXEME *id_expression(LEXEME *lex, SYMBOL **sym, SYMBOL **strSym, NAMESPACEVALUES **nsv, BOOL tagsOnly, char *idname)
{
    SYMBOL *encloser = NULL;
    NAMESPACEVALUES *ns = NULL;
    BOOL throughClass = FALSE;
    char buf[512];
    int ov;
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
    lex = getIdName(lex, buf, &ov);
    if (buf[0])
    {
        *sym = finishSearch(buf, encloser, ns, tagsOnly, throughClass);
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
static LIST *structuredArg(SYMBOL *sp, LIST *in, TYPE *tp)
{
    return searchNS(sp, tp->sp->parentNameSpace, in) ;
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
    in = searchOneArg(sp, in , tp->btp);
    return in;
}
static LIST *searchOneArg(SYMBOL *sp, LIST *in, TYPE *tp)
{
    if (ispointer(tp)|| isref(tp))
        return searchOneArg(sp, in, tp->btp);
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
enum e_cvsrn
{
    // tier 1
    CV_IDENTITY,
    CV_LVALUE,
    CV_CONVERSION,
    CV_QUALS,
    CV_PTRBOOLCONVERSION,
    // tier 2
    CV_USER,
    // tier 3
    CV_ELLIPSIS,
    // other
    CV_PAD,
    CV_AMBIGUOUS,
    CV_NONE,
} ;
static void getPointerConversion(TYPE *tpp, TYPE *tpa, EXPRESSION *exp, int *n, 
                                 enum e_cvsrn *seq)
{
    if (basetype(tpa)->btp->type == bt_void && exp && (isconstzero(&stdint, exp) || exp->type == en_nullptr))
    {
        seq[(*n)++] = CV_CONVERSION;
        return;
    }
    else
    {
        if (basetype(tpa)->array)
            seq[(*n)++] = CV_LVALUE;
        if (isfunction(tpa))
            seq[(*n)++] = CV_LVALUE;
        if (basetype(tpp)->btp->type == bt_void)
        {
            seq[(*n)++] = CV_CONVERSION;
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
                    seq[(*n)++] = CV_IDENTITY;
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
        while (tpp && tpa)
        {
            if (basetype(tpp)->type == bt_pointer || basetype(tpp)->type == bt_memberptr)
            {
                if (isconst(tpa) && !isconst(tpp) || isvolatile(tpa) && !isvolatile(tpp))
                    break;
            }
            tpp = basetype(tpp)->btp;
            tpa = basetype(tpa)->btp;
        }
        if (tpp)
            seq[(*n)++] = CV_QUALS;
    }
}
static void getSingleConversion(TYPE *tpp, TYPE *tpa, EXPRESSION *expa, int *n, enum e_cvsrn *seq)
{
    if (isref(tpp))
    {
        TYPE *tppp = tpp->btp;
        if (isstructured(tpa))
        {
            if (isstructured(tppp))
            {
                if (basetype(tpa)->sp != basetype(tppp)->sp)
                {
                    if (classRefCount(basetype(tppp)->sp, basetype(tpa)->sp) == 1)
                    {
                        seq[(*n)++] = CV_CONVERSION;
                    }
                    else
                    {
                        seq[(*n)++] = CV_NONE;
                    }
                }
                else
                    seq[(*n)++] = CV_IDENTITY;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (isstructured(tppp))
        {
             seq[(*n)++] = CV_NONE;
        }
        else if (tpp->type == bt_rref && expa && lvalue(expa))
        {
            // lvalue to rvalue ref not allowed
            seq[(*n)++] = CV_NONE;
        }
        else if (tpp->type == bt_lref && expa && !lvalue(expa) && (!isconst(tpp->btp) || isvolatile(tpp->btp)))
        {
            // rvalue to lvalue ref not allowed unless the lvaule ref is nonvolatile and const
            seq[(*n)++] = CV_NONE;
        }
        else
        {
            getSingleConversion(tpp->btp, tpa, expa, n, seq);
        }
    }
    else
    {
        if (expa && lvalue(expa))
            seq[(*n)++] = CV_LVALUE;
        if (isstructured(tpa))
        {
            if (isstructured(tpp))
            {
                if (tpa->sp != tpp->sp)
                    seq[(*n)++] = CV_NONE;
                else
                    seq[(*n)++] = CV_IDENTITY;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
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
                seq[(*n)++] = CV_CONVERSION;
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
        else if (basetype(tpa)->type == bt_enum)
        {
            if (basetype(tpp)->type == bt_enum)
            {
                if (basetype(tpa)->sp != basetype(tpp)->sp)
                    seq[(*n)++] = CV_NONE;
            }
            else
            { 
                if (isint(tpp))
                {
                    if (basetype(tpp)->type != basetype(tpa)->btp->type)
                        seq[(*n)++] = CV_CONVERSION;
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
                if (basetype(tpp)->btp->type != basetype(tpa)->type)
                    seq[(*n)++] = CV_CONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;                
            }
        }
        else
        {
            if (basetype(tpp)->type != basetype(tpa)->type)
                seq[(*n)++] = CV_CONVERSION;
            else
                seq[(*n)++] = CV_IDENTITY;
        }
    }
}
static BOOL getFuncConversions(SYMBOL *sp, FUNCTIONCALL *f, TYPE *atp, enum e_cvsrn arr[500], int *sizes, int count)
{
    int n = 0;
    int i;
    ARGLIST *a = NULL;
    HASHREC **hr = basetype(sp->tp)->syms->table;
    HASHREC **hrt = NULL;
    enum e_cvsrn seq[100], cur;
    int m = 0;
    if (f)
        a = f->arguments;
    else
        hrt = atp->syms->table;
    for (i=0; i < count; i++)
    {
        arr[i] = CV_PAD;
    }
    /* takes care of THIS pointer */
    if (f)
    {
        if (f->thisptr)
        {
            TYPE *argtp = sp->tp;
            if (!argtp)
            {
                arr[n++] = CV_NONE;
            }
            else
            {
                TYPE tpc, tpv , *tpp, *temp;
                if (sp->parentClass)
                {
                    tpp = sp->parentClass->tp;
                    if (isconst(argtp))
                    {
                        memset(&tpc, 0, sizeof(tpc));
                        tpc.type = bt_const;
                        temp = &tpc;
                        temp->btp = tpp;
                        tpp = temp; 
                    }
                    if (isvolatile(argtp))
                    {
                        memset(&tpv, 0, sizeof(tpv));
                        tpv.type = bt_volatile;
                        temp = &tpv;
                        temp->btp = tpp;
                        tpp = temp; 
                    }
                }
                else
                {
                    SYMBOL *argsym = (SYMBOL *)(*hr)->p;
                    hr = &(*hr)->next;
                    tpp = argsym->tp;
                }
                getSingleConversion(tpp, f->thistp, f->thisptr, &n, arr);
            }
        }
    }
    else
    {
        if (sp->storage_class == sc_member || sp->storage_class == sc_virtual)
        {
            TYPE *argtp = sp->parentClass->tp;
            if (!argtp)
                arr[n++] = CV_NONE;
            else 
            {
                getSingleConversion(argtp, a ? a->tp : ((SYMBOL *)(*hrt)->p)->tp, a ? a->exp : NULL, 
                                    &m, seq);
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
        m = 0;
        if (argsym->storage_class != sc_parameter)
            return FALSE;
        if (argsym->tp->type == bt_ellipse)
        {
            arr[n++] = CV_ELLIPSIS;
            return TRUE;
        }
        getSingleConversion(argsym->tp, a ? a->tp : ((SYMBOL *)(*hrt)->p)->tp, a ? a->exp : NULL, &m, seq);
        if (m > 3)
            cur = CV_NONE;
        else
        {
            sizes[n] = m;
            cur = CV_IDENTITY;
            for ( ;m > 0; m--)
            {
                if (seq[m-1] > cur)
                    cur = seq[m-1];
            }
        }
        arr[n++] = cur;
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
            return TRUE;
        if (sym->tp->type == bt_ellipse)
        {
            arr[n++] = CV_ELLIPSIS;
            return TRUE;
        }
        hr = basetype(sp->tp)->syms->table;
        if (*hr)
        {
            sym = (SYMBOL *)(*hr)->p;
            if (sym->tp->type == bt_void)
                return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}
#define TLT 1
#define TGT 2
#define TN1 16
#define TN2 32
static int icsBetter(int count, enum e_cvsrn *arr1, enum e_cvsrn *arr2, 
                     int *n1, int *n2)
{
    int rv = 0;
    int i;
    int x1 = CV_NONE, x2 = CV_NONE;
    for (i = 0; i < count ; i++)
    {
        if (x1 != CV_ELLIPSIS)
            x1 = arr1[i];
        if (x2 != CV_ELLIPSIS)
            x2 = arr2[i];
        if (x1 == CV_NONE || x1 == CV_PAD && x2 == CV_ELLIPSIS)
            rv |= TN1;
        if (x2 == CV_NONE || x2 == CV_PAD && x1 == CV_ELLIPSIS)
            rv |= TN2;
        if (x1 != x2)
        {
            if (x1 > x2 || n1[i] > n2[i])
            {
                rv |= TGT;
            }
            else if (x1 < x2 || n1[i] < n2[i])
            {
                rv |= TLT;
            }
        }
    }
    return rv;
}
SYMBOL *GetOverloadedFunction(TYPE **tp, EXPRESSION **exp, SYMBOL *sp, 
                              FUNCTIONCALL *args, TYPE *atp, BOOL toErr)
{
    // note for purposes of this lookup, 'args' will not have a this pointer
    // it will be added as soon as we select a member function that needs it
    
    if (atp && ispointer(atp))
        atp = basetype(atp)->btp;
    if (atp && !isfunction(atp))
        atp = NULL;
    if (sp->storage_class == sc_overloads)
    {
        LIST *gather = NULL;
        SYMBOL **flatList;    
        SYMBOL *found1 = NULL, *found2 = NULL;
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
        // ok got the initial list, time for phase 2
        // which is to add any other functions that have to be added...
        // constructors, member operator '()' and so forth...
        if (gather)
        {
            // we are only doing global functions for now...
            // so nothing here...
            
        }
        // pass 3 - the actual argument-based resolution
        if (gather)
        {
            LIST *lst2;
            int n = 0;
            BOOL done = FALSE;
            lst2 = gather;
            while (lst2 && !done)
            {
                HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                while (*hr)
                {
                    n++;
                    hr = &(*hr)->next;
                }
                lst2 = lst2->next;
            }
            if (n > 1 && (args || atp))
            {
                int i;
                SYMBOL **spList;
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
                lst2 = gather;
                n = 0;
                while (lst2 && !done)
                {
                    HASHREC **hr = ((SYMBOL *)lst2->data)->tp->syms->table;
                    while (*hr)
                    {
                        spList[n++] = (SYMBOL *)(*hr)->p;
                        hr = &(*hr)->next;
                    }
                    lst2 = lst2->next;
                }
                for (i=0; i < n; i++)
                {
                    int j;
                    if (spList[i])
                    {
                        enum e_cvsrn arr[500];
                        int counts[500];
                        BOOL t;
                        for (j = i + 1; j < n; j++)
                            if (spList[i] == spList[j])
                                spList[j] = 0;
                        t = getFuncConversions(spList[i], args, atp, arr, counts, argCount);
                        if (!t)
                        {
                            spList[i] = NULL;
                        }
                        else
                        {
                            icsList[i] = (enum e_cvsrn *)Alloc(sizeof(enum e_cvsrn) * argCount);
                            memcpy(icsList[i], arr, argCount * sizeof(enum e_cvsrn));
                            lenList[i] = (int *)Alloc(sizeof(int) * argCount);
                            memcpy(lenList[i], counts, argCount * sizeof(int));
                        }
                    }
                }
                while (!done)
                {
                    int k;
                    done = TRUE;
                    for (i=0; i < n-1; i++)
                    {
                        if (spList[i])
                        {
                            int j;
                            for (k=0; k < argCount; k++)
                                if (icsList[i][k] == CV_ELLIPSIS)
                                    break;
                            if (k >= argCount)
                                for (j=i+1; j < n && spList[i]; j++)
                                    if (spList[j])
                                    {
                                        for (k=0; k < argCount; k++)
                                            if (icsList[j][k] == CV_ELLIPSIS)
                                                break;
                                        if (k >= argCount)
                                        {
                                            int flags = icsBetter(argCount, icsList[i], icsList[j], lenList[i], lenList[j]);
                                            if (flags & TN1)
                                                spList[i] = NULL;
                                            if (flags & TN2)
                                                spList[j] = NULL;
                                            if ((flags & (TGT | TLT)) == TGT)
                                                spList[i] = NULL;
                                            if ((flags & (TGT | TLT)) == TLT)
                                                spList[j] = NULL;
                                        }
                                    }
                        }
                    }
                }
                done = FALSE;
                while (!done)
                {
                    done = TRUE;
                    for (i=0; i < n-1; i++)
                    {
                        int j;
                        for (j=i+1; j < n && spList[i]; j++)
                            if (spList[j])
                            {
                                int flags = icsBetter(argCount, icsList[i], icsList[j], lenList[i], lenList[j]);
                                if (flags & TN1)
                                    spList[i] = NULL;
                                if (flags & TN2)
                                    spList[j] = NULL;
                                if ((flags & (TGT | TLT)) == TGT)
                                    spList[i] = NULL;
                                if ((flags & (TGT | TLT)) == TLT)
                                    spList[j] = NULL;
                            }
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
