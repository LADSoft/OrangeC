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

extern char *overloadNameTab[];
extern TYPE stdint;

static HASHTABLE *intrinsicHash;

typedef BOOLEAN INTRINS_FUNC(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_base_of(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_class(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_constructible(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_convertible_to(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_empty(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_enum(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_final(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_literal(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_nothrow_constructible(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_pod(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_polymorphic(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_standard_layout(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_trivial(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_trivially_assignable(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_trivially_constructible(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_trivially_copyable(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static BOOLEAN is_union(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp);
static struct _ihash
{
    char *name;
    INTRINS_FUNC *func;
    
} defaults[] = {
    { "__is_base_of", is_base_of },
    { "__is_class", is_class },
    { "__is_constructible", is_constructible },
    { "__is_convertible_to", is_convertible_to },
    { "__is_empty", is_empty },
    { "__is_enum", is_enum },
    { "__is_final", is_final },
    { "__is_literal", is_literal },
    { "__is_nothrow_constructible", is_nothrow_constructible },
    { "__is_pod", is_pod },
    { "__is_polymorphic", is_polymorphic },
    { "__is_standard_layout", is_standard_layout },
    { "__is_trivial", is_trivial },
    { "__is_trivially_assignable", is_trivially_assignable },
    { "__is_trivially_constructible", is_trivially_constructible },
    { "__is_trivially_copyable", is_trivially_copyable },
    { "__is_union", is_union },
};
void libcxx_init(void)
{
    int i;
    intrinsicHash = CreateHashTable(32);
    for (i=0; i < sizeof(defaults)/sizeof(defaults[0]); i++)
        AddName((SYMBOL *)&defaults[i], intrinsicHash);
           
}
BOOLEAN parseBuiltInTypelistFunc(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    HASHREC **hr = LookupName(sym->name, intrinsicHash);
    if (hr)
    {
        struct _ihash *p = (struct _ihash *)(*hr)->p;
        return p->func(lex, funcsp, sym, tp, exp);
    }
    return FALSE;
           
}
static LEXEME *getTypeList(LEXEME *lex, SYMBOL *funcsp, INITLIST **lptr)
{
    *lptr = NULL;
    do
    {
        TYPE *tp = NULL;
        lex = getsym(); /* past ( or , */
        lex = get_type_id(lex, &tp, funcsp, sc_cast, FALSE, TRUE);
        if (!tp)
            break;
        if (basetype(tp)->type != bt_templateparam)
        {
            *lptr = Alloc(sizeof(INITLIST));
            (*lptr)->tp = tp;
            (*lptr)->exp = intNode(en_c_i, 0);
            lptr = &(*lptr)->next;
        }
        else
        {
            tp = basetype(tp);
            if (tp->templateParam->p->packed)
            {
                TEMPLATEPARAMLIST *tpl = tp->templateParam->p->byPack.pack;
                needkw(&lex, ellipse);
                while (tpl)
                {
                    if (tpl->p->byClass.val)
                    {
                        *lptr = Alloc(sizeof(INITLIST));
                        (*lptr)->tp = tpl->p->byClass.val;
                        (*lptr)->exp = intNode(en_c_i, 0);
                        lptr = &(*lptr)->next;
                    }
                    tpl = tpl->next;
                }

            }
            else
            {
                if (tp->templateParam->p->byClass.val)
                {
                    *lptr = Alloc(sizeof(INITLIST));
                    (*lptr)->tp = tp->templateParam->p->byClass.val;
                    (*lptr)->exp = intNode(en_c_i, 0);
                    lptr = &(*lptr)->next;
                }
            }
        }
        // this may be a bit naive...
        if (MATCHKW(lex, ellipse))
            lex = getsym();
    } while (MATCHKW(lex, comma));
    needkw(&lex, closepa);
    return lex;
}
static int FindBaseClassWithData(SYMBOL *sym, SYMBOL **result)
{
    HASHREC *hr;
    int n = 0;
    BASECLASS *bc = sym->baseClasses;
    while (bc)
    {
        n+= FindBaseClassWithData(bc->cls, result);
        bc = bc->next;
    }
    hr = basetype(sym->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->storage_class == sc_mutable || sp->storage_class == sc_member)
        {
            if (result)
                *result = sym;
            return n + 1;
        }
        hr = hr->next;
    }
    return n;
}
static BOOLEAN isStandardLayout(TYPE *tp, SYMBOL **result)
{
    if (isstructured(tp) && !hasVTab(basetype(tp)->sp) && !basetype(tp)->sp->vbaseEntries )
    {
        int n;
        int access = -1;
        SYMBOL *found = NULL, *first;
        HASHREC *hr;
        n = FindBaseClassWithData(tp->sp, &found);
        if (n > 1)
            return FALSE;
        if (n)
        {
            SYMBOL *first = NULL;
            hr = basetype(found->tp)->syms->table[0];
            while (hr)
            {
                SYMBOL *sp = (SYMBOL *)hr->p;
                if (!first)
                    first = sp;
                if (sp->storage_class == sc_member || sp->storage_class == sc_mutable)
                {
                    if (isstructured(sp->tp) && !isStandardLayout(sp->tp, NULL))
                        return FALSE;
                    if (access != -1)
                    {
                        if (access != sp->access)
                            return FALSE;
                    }
                    access = sp->access;
                }
                hr = hr->next;
            }
            if (first && isstructured(first->tp))
            {
                BASECLASS *bc = found->baseClasses;
                while (bc)
                {
                    if (comparetypes(bc->cls->tp, first->tp, TRUE))
                        return FALSE;
                    bc = bc->next;
                }
            }
        }
        if (result)
            *result = found;
        return TRUE;
    }
    return FALSE;
    /*
 hasnonon-staticdatamembersoftypenon-standard-layoutclass(orarrayofsuchtypes)orreference, 
 — has no virtual functions (10.3) and no virtual base classes (10.1), 
 — has the same access control (Clause 11) for all non-static data members, 
 — has no non-standard-layout base classes, 
 — either has no non-static data members in the most derived class and at most one base class with non-static data members, 
    or has no base classes with non-static data members, and 
 — has no base classes of the same type as the ?rst non-static data member.     
 */
}
static BOOLEAN trivialFunc(SYMBOL *func, BOOLEAN move)
{
    HASHREC *hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (matchesCopy(sp, move))
        {
            return sp->defaulted;
        }
        hr = hr->next;
    }
    return TRUE;
}
static BOOLEAN trivialCopyConstructible(TYPE *tp)
{
    if (isstructured(tp))
    {
        HASHREC *hr;
        SYMBOL *ovl;
        BASECLASS *bc;
        ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            if (!trivialFunc(ovl, FALSE) || !trivialFunc(ovl, TRUE))
                return FALSE;
        }
        bc = basetype(tp)->sp->baseClasses;
        while (bc)
        {
            if (!trivialCopyConstructible(bc->cls->tp))
                return FALSE;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialCopyConstructible(sym->tp))
                    return FALSE;
            hr = hr->next;
        }
    }
    return TRUE;
}
static BOOLEAN trivialCopyAssignable(TYPE *tp)
{
    if (isstructured(tp))
    {
        HASHREC *hr;
        SYMBOL *ovl;
        BASECLASS * bc;
        ovl = search(overloadNameTab[assign - kw_new + CI_NEW ], basetype(tp)->syms);
        if (ovl)
        {
            if (!trivialFunc(ovl, FALSE) || !trivialFunc(ovl, TRUE))
                return FALSE;
        }
        bc = basetype(tp)->sp->baseClasses;
        while (bc)
        {
            if (!trivialCopyAssignable(bc->cls->tp))
                return FALSE;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialCopyAssignable(sym->tp))
                    return FALSE;
            hr = hr->next;
        }
    }
    return TRUE;
}
static BOOLEAN trivialDestructor(TYPE *tp)
{
    if (isstructured(tp))
    {
        HASHREC *hr;
        SYMBOL *ovl;
        BASECLASS *bc;
        ovl = search(overloadNameTab[CI_DESTRUCTOR ], basetype(tp)->syms);
        if (ovl)
        {
            ovl = (SYMBOL *)ovl->tp->syms->table[0]->p;
            if (!ovl->defaulted)
                return FALSE;
        }
        bc = basetype(tp)->sp->baseClasses;
        while (bc)
        {
            if (!trivialDestructor(bc->cls->tp))
                return FALSE;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialDestructor(sym->tp))
                    return FALSE;
            hr = hr->next;
        }
    }
    return TRUE;
}
static BOOLEAN trivialDefaultConstructor(TYPE *tp)
{
    if (isstructured(tp))
    {
        HASHREC *hr;
        SYMBOL *ovl;
        BASECLASS *bc;
        ovl = search(overloadNameTab[CI_CONSTRUCTOR ], basetype(tp)->syms);
        if (ovl)
        {
            HASHREC *hr = ovl->tp->syms->table[0];
            while (hr)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                HASHREC *hr1 = basetype(sym->tp)->syms->table[0];
                
                if (!hr1->next || !hr1->next->next || ((SYMBOL *)hr1->next->next->p)->tp->type == bt_void)
                    if (!sym->defaulted)
                        return FALSE;
                    else
                        break;
                hr = hr->next;
            }
        }
        bc = basetype(tp)->sp->baseClasses;
        while (bc)
        {
            if (!trivialDefaultConstructor(bc->cls->tp))
                return FALSE;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialDefaultConstructor(sym->tp))
                    return FALSE;
            hr = hr->next;
        }
    }
    return TRUE;
}
static BOOLEAN triviallyCopyable(TYPE *tp)
{
    return trivialCopyConstructible(tp) && trivialCopyAssignable(tp) && trivialDestructor(tp);
}
static BOOLEAN trivialStructure(TYPE *tp)
{
    if (isref(tp))
        tp = basetype(tp)->btp;
    if (isstructured(tp))
    {
        return triviallyCopyable(tp) && trivialDefaultConstructor(tp);
    }
    return FALSE;
}
    /*
A trivially copyable class is a class that: 
— has no non-trivial copy constructors (12.8),
— has no non-trivial move constructors (12.8), 
— has no non-trivial copy assignment operators (13.5.3, 12.8), 
— has no non-trivial move assignment operators (13.5.3, 12.8), and 
— has a trivial destructor (12.4). 

A trivial class is a class that has a trivial default constructor (12.1) and is trivially copyable.     
*/
static BOOLEAN trivialStructureWithBases(TYPE *tp)
{
    if (isstructured(tp))
    {
        BASECLASS *bc;
        if (!trivialStructure(tp))
            return FALSE;
        bc = basetype(tp)->sp->baseClasses;
        while (bc)
        {
            if (!trivialStructureWithBases(bc->cls->tp))
                return FALSE;
            bc = bc->next;
        }
    }
    return TRUE;
}
static BOOLEAN isPOD(TYPE *tp)
{
    SYMBOL *found = NULL;
    if (isStandardLayout(tp, found) && trivialStructureWithBases(tp))
    {
        if (found)
        {
            HASHREC *hr = basetype(found->tp)->syms->table;
            while (hr)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                if (isstructured(sym->tp) && !trivialStructureWithBases(sym->tp))
                    return FALSE;
                hr = hr->next;
            }
        }
        return TRUE;
    }
    return FALSE;
}
static BOOLEAN nothrowConstructible(TYPE *tp)
{
    if (isstructured(tp))
    {
        SYMBOL *ovl;
        // recursion will have been taken care of elsewhere...
        ovl = search(overloadNameTab[CI_CONSTRUCTOR ], basetype(tp)->syms);
        if (ovl)
        {
            HASHREC *hr = ovl->tp->syms->table[0];
            hr = basetype(ovl->tp)->syms->table[0];
            while (hr)
            {
                SYMBOL *sp = (SYMBOL *)hr->p;
                HASHREC *hr1 = basetype(sp->tp)->syms->table[0];
                if (matchesCopy(sp, FALSE) || matchesCopy(sp, TRUE) || !hr1->next || !hr1->next->next || ((SYMBOL *)hr1->next->next->p)->tp->type == bt_void)
                {
                    if (sp->xcMode != xc_none)
                        return FALSE;
                }
                hr = hr->next;
            }
        }
    }
    return TRUE;
}
static BOOLEAN is_base_of(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && funcparams.arguments-> next && !funcparams.arguments->next->next)
    {
        if (isstructured(funcparams.arguments->tp) && isstructured(funcparams.arguments->next->tp))
            rv = classRefCount(basetype(funcparams.arguments->tp)->sp, basetype(funcparams.arguments->next->tp)->sp) != 0;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_class(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        rv = isstructured(funcparams.arguments->tp) && basetype(funcparams.arguments->tp)->type != bt_union;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_constructible(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments)        
    {
        TYPE *tp2 = funcparams.arguments->tp;
        if (isarray(tp2))
        {
            while (isarray(tp2) && tp2->size != 0)
                tp2 = tp2->btp;
                
            if (isarray(tp2))
            {
                tp2 = FALSE;
            }
        }
        if (tp2)
        {
            TYPE *tpf = tp2;
            if (isref(tp2))
                tp2 = basetype(tp2)->btp;
            if (isfunction(tp2))
            {
                if (funcparams.arguments->next && !funcparams.arguments->next->next)
                {
                    TYPE *tpy = funcparams.arguments->next->tp;
                    if (isref(tpf))
                    {
                        if (isref(tpy))
                            tpy = basetype(tpy)->btp;
                        rv = comparetypes(tp2, tpy, TRUE);
                    }
                }
            }
            else if (isfuncptr(tp2))
            {
                if (funcparams.arguments->next && !funcparams.arguments->next->next)
                {
                    if (isfunction(funcparams.arguments->next->tp))
                    {
                        rv = comparetypes(basetype(tp2)->btp, funcparams.arguments->next->tp, TRUE);
                    }
                    else if (isstructured(funcparams.arguments->next->tp))
                    {
                        // look for operator () with args from tp2
                        HASHREC *hr;
                        EXPRESSION *cexp = NULL;
                        INITLIST **arg = &funcparams.arguments;
                        SYMBOL *bcall = search(overloadNameTab[CI_FUNC], basetype(funcparams.arguments->next->tp)->syms);
                        funcparams.thisptr = intNode(en_c_i, 0);
                        funcparams.thistp = Alloc(sizeof(TYPE));
                        funcparams.thistp->type = bt_pointer;
                        funcparams.thistp->btp = basetype(funcparams.arguments->next->tp);
                        funcparams.thistp->rootType = funcparams.thistp;
                        funcparams.thistp->size = getSize(bt_pointer);
                        funcparams.ascall = TRUE;
                        funcparams.arguments = NULL;
                        funcparams.sp = NULL;
                        hr = basetype(basetype(tp2)->btp)->syms->table[0];
                        while (hr)
                        {
                            *arg = (INITLIST *)Alloc(sizeof(INITLIST));
                            (*arg)->tp = ((SYMBOL *)hr->p)->tp;
                            (*arg)->exp = intNode(en_c_i, 0);
                            arg = &(*arg)->next;
                            hr = hr->next;
                        }
                        rv = GetOverloadedFunction(tp, &funcparams.fcall, bcall, &funcparams, NULL, FALSE,
                            FALSE, FALSE, _F_SIZEOF) != NULL;
                    }
                    else
                    {
                        rv = comparetypes(tp2, funcparams.arguments->next->tp, TRUE);
                    }
                }
            }
            else if (isarithmetic(tp2) || ispointer(tp2) || basetype(tp2)->type == bt_enum)
            {
                if (!funcparams.arguments->next)
                {
                    rv = TRUE;
                }
                else if (!funcparams.arguments->next->next)
                {
                    rv = comparetypes(tp2, funcparams.arguments->next->tp, TRUE);
                }
            }
            else if (isstructured(tp2))
            {
                if (funcparams.arguments->next && isstructured(funcparams.arguments->next->tp)
                    && (comparetypes(tp2, funcparams.arguments->next->tp, TRUE) || sameTemplate(tp2, funcparams.arguments->next->tp)))
                {
                    rv = TRUE;
                }
                else
                {
                    int i = 0;
                    char holdl[100], holdr[100];
                    INITLIST *temp;
                    EXPRESSION *cexp = NULL;
                    SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp2)->syms);
                    funcparams.thisptr = intNode(en_c_i, 0);
                    funcparams.thistp = Alloc(sizeof(TYPE));
                    funcparams.thistp->type = bt_pointer;
                    funcparams.thistp->btp = basetype(tp2);
                    funcparams.thistp->rootType = funcparams.thistp;
                    funcparams.thistp->size = getSize(bt_pointer);
                    funcparams.ascall = TRUE;
                    funcparams.arguments = funcparams.arguments->next;
                    temp = funcparams.arguments;
                    while (temp)
                    {
                        holdl[i] = temp->tp->lref;
                        holdr[i] = temp->tp->rref;
                        if (!temp->tp->rref && basetype(temp->tp) != bt_rref)
                        {
                            temp->tp->lref = TRUE;
                        }
                        temp = temp->next;
                    }
                    rv = GetOverloadedFunction(tp, &funcparams.fcall, cons, &funcparams, NULL, FALSE,
                        FALSE, FALSE, _F_SIZEOF) != NULL;
                    temp = funcparams.arguments;
                    while (temp)
                    {
                        temp->tp->lref = holdl[i];
                        temp->tp->rref = holdr[i];
                        temp = temp->next;
                    }
                }
            }
        }
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_convertible_to(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = TRUE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && funcparams.arguments->next && !funcparams.arguments->next->next) 
    {
        TYPE *from = funcparams.arguments->tp;
        TYPE *to = funcparams.arguments->next->tp;
        if (isref(from) && isref(to))
        {
            if (basetype(to)->type == bt_lref)
            {
                if (basetype(from)->type == bt_rref)
                    rv = FALSE;
            }
        }
        else if (isref(from))
            rv = FALSE;
        if (isfunction(from))
            from = basetype(from)->btp;
        if (rv)
        {
            while (isref(from))
                from = basetype(from)->btp;
            while (isref(to))
                to = basetype(to)->btp;
            rv = comparetypes(to, from, FALSE);
            if (!rv && isstructured(from) && isstructured(to))
			{
               if (classRefCount(basetype(to)->sp, basetype(from)->sp) == 1)
                   rv = TRUE;
			}
			if (!rv && isstructured(from))
			{
				SYMBOL *sp = search("$bcall", basetype(from)->syms);
				if (sp)
				{
					HASHREC *hr = sp->tp->syms->table[0];
					while (hr)
					{
						if (comparetypes(basetype(((SYMBOL *)hr->p)->tp)->btp, to, FALSE))
						{
							rv= TRUE;
							break;
						}
						hr = hr->next;
					}
				}
			}
        }
    }
    else
    {
        rv = FALSE;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_empty(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = !basetype(funcparams.arguments->tp)->syms->table[0] || !basetype(funcparams.arguments->tp)->syms->table[0]->next;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_enum(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        rv = basetype(funcparams.arguments->tp)->type == bt_enum;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_final(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = basetype(funcparams.arguments->tp)->sp->isfinal;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_literal(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        // yes references are literal types...
        rv = !isstructured(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_nothrow_constructible(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        TYPE *tp = funcparams.arguments->tp;
        if (isref(tp))
            tp = basetype(tp)->btp;
        if (isstructured(tp) && !basetype(tp)->sp->trivialCons)
            rv = nothrowConstructible(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_pod(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        rv = !!isPOD(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_polymorphic(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        // yes references are literal types...
        if (isstructured(funcparams.arguments->tp))
            rv = !! hasVTab(basetype(funcparams.arguments->tp)->sp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_standard_layout(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        rv = !!isStandardLayout(funcparams.arguments->tp, NULL);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_trivial(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        rv = !isstructured(funcparams.arguments->tp) || !!trivialStructure(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_trivially_assignable(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = trivialCopyAssignable(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_trivially_constructible(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        rv = trivialCopyConstructible(funcparams.arguments->tp) && trivialDefaultConstructor(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_trivially_copyable(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
        lst = lst->next;
        
    }
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = triviallyCopyable(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_union(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    INITLIST *lst;
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments-> next)
    {
        rv = basetype(funcparams.arguments->tp)->type == bt_union;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
