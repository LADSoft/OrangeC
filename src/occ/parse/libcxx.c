/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include "common.h"

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
// clang-format off
// this breaks the clang-format
#define INTRINSIC(x) {"__x", x},
INTRINSIC(is_base_of)
INTRINSIC(is_class)
INTRINSIC(is_constructible)
INTRINSIC(is_convertible_to)
INTRINSIC(is_empty)
INTRINSIC(is_enum)
INTRINSIC(is_final)
INTRINSIC(is_literal)
INTRINSIC(is_nothrow_constructible)
INTRINSIC(is_pod)
INTRINSIC(is_polymorphic)
INTRINSIC(is_standard_layout)
INTRINSIC(is_trivial)
INTRINSIC(is_trivially_assignable)
INTRINSIC(is_trivially_constructible)
INTRINSIC(is_union)
#undef INTRINSIC
    // clang-format on
};
void libcxx_init(void)
{
    int i;
    intrinsicHash = CreateHashTable(32);
    for (i = 0; i < sizeof(defaults) / sizeof(defaults[0]); i++)
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
                needkw(&lex, ellipse);
                for (TEMPLATEPARAMLIST *tpl = tp->templateParam->p->byPack.pack; tpl; tpl = tpl->next)
                {
                    if (tpl->p->byClass.val)
                    {
                        *lptr = Alloc(sizeof(INITLIST));
                        (*lptr)->tp = tpl->p->byClass.val;
                        (*lptr)->exp = intNode(en_c_i, 0);
                        lptr = &(*lptr)->next;
                    }
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
    int n = 0;
    NITERBASECLASS(bc, sym)
    {
        n += FindBaseClassWithData(bc->cls, result);
    }
    NITERSYMTAB(hr, basetype(sym->tp))
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->storage_class == sc_mutable || sp->storage_class == sc_member)
        {
            if (result)
                *result = sym;
            return n + 1;
        }
    }
    return n;
}
static BOOLEAN isStandardLayout(TYPE *tp, SYMBOL **result)
{
    if (isstructured(tp) && !hasVTab(basetype(tp)->sp) && !basetype(tp)->sp->vbaseEntries)
    {
        int n;
        int access = -1;
        SYMBOL *found = NULL, *first;
        n = FindBaseClassWithData(tp->sp, &found);
        if (n > 1)
            return FALSE;
        if (n)
        {
            SYMBOL *first = NULL;
            NITERSYMTAB(hr, basetype(found->tp))
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
            }
            if (first && isstructured(first->tp))
            {
                NITERBASECLASS(bc, found)
                {
                    if (comparetypes(bc->cls->tp, first->tp, TRUE))
                        return FALSE;
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
 � has no virtual functions (10.3) and no virtual base classes (10.1), 
 � has the same access control (Clause 11) for all non-static data members, 
 � has no non-standard-layout base classes, 
 � either has no non-static data members in the most derived class and at most one base class with non-static data members, 
    or has no base classes with non-static data members, and 
 � has no base classes of the same type as the ?rst non-static data member.     
 */
}
static BOOLEAN trivialFunc(SYMBOL *func, BOOLEAN move)
{
    NITERSYMTAB(hr, basetype(func->tp))
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (matchesCopy(sp, move))
        {
            return sp->defaulted;
        }
    }
    return TRUE;
}
static BOOLEAN trivialCopyConstructible(TYPE *tp)
{
    if (isstructured(tp))
    {
        SYMBOL *ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            if (!trivialFunc(ovl, FALSE) || !trivialFunc(ovl, TRUE))
                return FALSE;
        }
        NITERBASECLASS(bc, basetype(tp)->sp)
        {
            if (!trivialCopyConstructible(bc->cls->tp))
                return FALSE;
        }
        NITERSYMTAB(hr, basetype(tp))
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialCopyConstructible(sym->tp))
                    return FALSE;
        }
    }
    return TRUE;
}
static BOOLEAN trivialCopyAssignable(TYPE *tp)
{
    if (isstructured(tp))
    {
        SYMBOL *ovl = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(tp)->syms);
        if (ovl)
        {
            if (!trivialFunc(ovl, FALSE) || !trivialFunc(ovl, TRUE))
                return FALSE;
        }
        NITERBASECLASS(bc, basetype(tp)->sp)
        {
            if (!trivialCopyAssignable(bc->cls->tp))
                return FALSE;
        }
        NITERSYMTAB(hr, basetype(tp))
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialCopyAssignable(sym->tp))
                    return FALSE;
        }
    }
    return TRUE;
}
static BOOLEAN trivialDestructor(TYPE *tp)
{
    if (isstructured(tp))
    {
        SYMBOL *ovl = search(overloadNameTab[CI_DESTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            ovl = (SYMBOL *)SYMTABBEGIN(ovl->tp)->p;
            if (!ovl->defaulted)
                return FALSE;
        }
        NITERBASECLASS(bc, basetype(tp)->sp)
        {
            if (!trivialDestructor(bc->cls->tp))
                return FALSE;
        }
        NITERSYMTAB(hr, basetype(tp))
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialDestructor(sym->tp))
                    return FALSE;
        }
    }
    return TRUE;
}
static BOOLEAN trivialDefaultConstructor(TYPE *tp)
{
    if (isstructured(tp))
    {
        SYMBOL *ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            NITERSYMTAB(hr, ovl->tp)
            {
                SYMBOL *sym = (SYMBOL *)hr->p;
                HASHREC *hr1 = SYMTABBEGIN(basetype(sym->tp));
                if (!hr1->next || !hr1->next->next || ((SYMBOL *)hr1->next->next->p)->tp->type == bt_void)
                    if (!sym->defaulted)
                        return FALSE;
                    else
                        break;
            }
        }
        NITERBASECLASS(bc, basetype(tp)->sp)
        {
            if (!trivialDefaultConstructor(bc->cls->tp))
                return FALSE;
        }
        NITERSYMTAB(hr, basetype(tp))
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->storage_class == sc_mutable || sym->storage_class == sc_member)
                if (!trivialDefaultConstructor(sym->tp))
                    return FALSE;
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
    return isstructured(tp) ? triviallyCopyable(tp) && trivialDefaultConstructor(tp) : FALSE;
}
/*
A trivially copyable class is a class that: 
� has no non-trivial copy constructors (12.8),
� has no non-trivial move constructors (12.8), 
� has no non-trivial copy assignment operators (13.5.3, 12.8), 
� has no non-trivial move assignment operators (13.5.3, 12.8), and 
� has a trivial destructor (12.4). 

A trivial class is a class that has a trivial default constructor (12.1) and is trivially copyable.     
*/
static BOOLEAN trivialStructureWithBases(TYPE *tp)
{
    if (isstructured(tp))
    {
        if (!trivialStructure(tp))
            return FALSE;
        NITERBASECLASS(bc, basetype(tp)->sp)
        {
            if (!trivialStructureWithBases(bc->cls->tp))
                return FALSE;
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
            HASHREC *hr = basetype(found->tp)->syms->table; // ?? I want to ask about this
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
    { // recursion will have been taken care of elsewhere...
        SYMBOL *ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            NITERSYMTAB(hr, basetype(ovl->tp))
            {
                SYMBOL *sp = (SYMBOL *)hr->p;
                HASHREC *hr1 = SYMTABBEGIN(basetype(sp->tp));
                if (matchesCopy(sp, FALSE) || matchesCopy(sp, TRUE) || !hr1->next || !hr1->next->next || ((SYMBOL *)hr1->next->next->p)->tp->type == bt_void)
                {
                    if (sp->xcMode != xc_none)
                        return FALSE;
                }
            }
        }
    }
    return TRUE;
}
static BOOLEAN is_base_of(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && funcparams.arguments->next && !funcparams.arguments->next->next)
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
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = isstructured(funcparams.arguments->tp) && basetype(funcparams.arguments->tp)->type != bt_union;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_constructible(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
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
                        NITERSYMTAB(hr, basetype(basetype(tp2)->btp))
                        {
                            *arg = (INITLIST *)Alloc(sizeof(INITLIST));
                            (*arg)->tp = ((SYMBOL *)hr->p)->tp;
                            (*arg)->exp = intNode(en_c_i, 0);
                            arg = &(*arg)->next;
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
                if (funcparams.arguments->next && isstructured(funcparams.arguments->next->tp) && (comparetypes(tp2, funcparams.arguments->next->tp, TRUE) || sameTemplate(tp2, funcparams.arguments->next->tp)))
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
                    for (temp = funcparams.arguments; temp; temp = temp->next)
                    {
                        holdl[i] = temp->tp->lref;
                        holdr[i] = temp->tp->rref;
                        if (!temp->tp->rref && basetype(temp->tp) != bt_rref)
                        {
                            temp->tp->lref = TRUE;
                        }
                    }
                    rv = GetOverloadedFunction(tp, &funcparams.fcall, cons, &funcparams, NULL, FALSE,
                                               FALSE, FALSE, _F_SIZEOF) != NULL;
                    for (temp = funcparams.arguments; temp; temp = temp->next)
                    {
                        temp->tp->lref = holdl[i];
                        temp->tp->rref = holdr[i];
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
                    NITERSYMTAB(hr, sp->tp)
                    {
                        if (comparetypes(basetype(((SYMBOL *)hr->p)->tp)->btp, to, FALSE))
                        {
                            rv = TRUE;
                            break;
                        }
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
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = !SYMTABBEGIN(basetype(funcparams.arguments->tp)) || !SYMTABBEGIN(basetype(funcparams.arguments->tp))->next;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_enum(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments->next)
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
    if (funcparams.arguments && !funcparams.arguments->next)
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
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments->next)
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
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
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
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = !!isPOD(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_polymorphic(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        // yes references are literal types...
        if (isstructured(funcparams.arguments->tp))
            rv = !!hasVTab(basetype(funcparams.arguments->tp)->sp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_standard_layout(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = !!isStandardLayout(funcparams.arguments->tp, NULL);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_trivial(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = !isstructured(funcparams.arguments->tp) || !!trivialStructure(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_trivially_assignable(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
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
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = trivialCopyConstructible(funcparams.arguments->tp) && trivialDefaultConstructor(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
static BOOLEAN is_trivially_copyable(LEXEME **lex, SYMBOL *funcsp, SYMBOL *sym, TYPE **tp, EXPRESSION **exp)
{
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    for (INITLIST *lst = funcparams.arguments; lst; lst = lst->next)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, NULL);
    }
    if (funcparams.arguments && !funcparams.arguments->next)
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
    BOOLEAN rv = FALSE;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = basetype(funcparams.arguments->tp)->type == bt_union;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return TRUE;
}
