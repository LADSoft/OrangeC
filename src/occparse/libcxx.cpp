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

#include "compiler.h"
#include <stack>
#include "mangle.h"
#include "initbackend.h"
#include "symtab.h"
#include "cpplookup.h"
#include "template.h"
#include "declcpp.h"
#include "ccerr.h"
#include "declcons.h"
#include "help.h"
#include "expr.h"
#include "declare.h"
#include "lex.h"
#include "beinterf.h"
#include "types.h"
#include "memory.h"

namespace Parser
{

static HASHTABLE* intrinsicHash;

typedef bool INTRINS_FUNC(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_abstract(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_base_of(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_class(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_constructible(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_convertible_to(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_empty(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_enum(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_final(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_literal(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_nothrow_constructible(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_pod(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_polymorphic(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_standard_layout(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_trivial(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_trivially_assignable(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_trivially_constructible(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_trivially_copyable(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static bool is_union(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp);
static struct _ihash
{
    const char* name;
    INTRINS_FUNC* func;

} defaults[] = {
    {"__is_abstract", is_abstract},
    {"__is_base_of", is_base_of},
    {"__is_class", is_class},
    {"__is_constructible", is_constructible},
    {"__is_convertible_to", is_convertible_to},
    {"__is_empty", is_empty},
    {"__is_enum", is_enum},
    {"__is_final", is_final},
    {"__is_literal", is_literal},
    {"__is_nothrow_constructible", is_nothrow_constructible},
    {"__is_pod", is_pod},
    {"__is_polymorphic", is_polymorphic},
    {"__is_standard_layout", is_standard_layout},
    {"__is_trivial", is_trivial},
    {"__is_trivially_assignable", is_trivially_assignable},
    {"__is_trivially_constructible", is_trivially_constructible},
    {"__is_trivially_copyable", is_trivially_copyable},
    {"__is_union", is_union},
};
void libcxx_init(void)
{
    int i;
    intrinsicHash = CreateHashTable(32);
    for (i = 0; i < sizeof(defaults) / sizeof(defaults[0]); i++)
        AddName((SYMBOL*)&defaults[i], intrinsicHash);
}
bool parseBuiltInTypelistFunc(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    SYMLIST** hr = LookupName(sym->name, intrinsicHash);
    if (hr)
    {
        struct _ihash* p = (struct _ihash*)(*hr)->p;
        return p->func(lex, funcsp, sym, tp, exp);
    }
    return false;
}
static LEXEME* getTypeList(LEXEME* lex, SYMBOL* funcsp, INITLIST** lptr)
{
    *lptr = nullptr;
    do
    {
        TYPE* tp = nullptr;
        lex = getsym(); /* past ( or , */
        lex = get_type_id(lex, &tp, funcsp, sc_cast, false, true, false);
        if (!tp)
            break;
        if (basetype(tp)->type != bt_templateparam)
        {
            *lptr = Allocate<INITLIST>();
            (*lptr)->tp = tp;
            (*lptr)->exp = intNode(en_c_i, 0);
            lptr = &(*lptr)->next;
        }
        else
        {
            tp = basetype(tp);
            if (tp->templateParam->p->packed)
            {
                TEMPLATEPARAMLIST* tpl = tp->templateParam->p->byPack.pack;
                needkw(&lex, ellipse);
                while (tpl)
                {
                    if (tpl->p->byClass.val)
                    {
                        *lptr = Allocate<INITLIST>();
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
                    *lptr = Allocate<INITLIST>();
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
static int FindBaseClassWithData(SYMBOL* sym, SYMBOL** result)
{
    SYMLIST* hr;
    int n = 0;
    BASECLASS* bc = sym->sb->baseClasses;
    while (bc)
    {
        n += FindBaseClassWithData(bc->cls, result);
        bc = bc->next;
    }
    hr = basetype(sym->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sym = hr->p;
        if (sym->sb->storage_class == sc_mutable || sym->sb->storage_class == sc_member)
        {
            if (result)
                *result = sym;
            return n + 1;
        }
        hr = hr->next;
    }
    return n;
}
static bool isStandardLayout(TYPE* tp, SYMBOL** result)
{
    if (isstructured(tp) && !hasVTab(basetype(tp)->sp) && !basetype(tp)->sp->sb->vbaseEntries)
    {
        int n;
        int access = -1;
        SYMBOL *found = nullptr, *first;
        SYMLIST* hr;
        n = FindBaseClassWithData(tp->sp, &found);
        if (n > 1)
            return false;
        if (n)
        {
            SYMBOL* first = nullptr;
            hr = basetype(found->tp)->syms->table[0];
            while (hr)
            {
                SYMBOL* sym = hr->p;
                if (!first)
                    first = sym;
                if (sym->sb->storage_class == sc_member || sym->sb->storage_class == sc_mutable)
                {
                    if (isstructured(sym->tp) && !isStandardLayout(sym->tp, nullptr))
                        return false;
                    if (access != -1)
                    {
                        if (access != sym->sb->access)
                            return false;
                    }
                    access = sym->sb->access;
                }
                hr = hr->next;
            }
            if (first && isstructured(first->tp))
            {
                BASECLASS* bc = found->sb->baseClasses;
                while (bc)
                {
                    if (comparetypes(bc->cls->tp, first->tp, true))
                        return false;
                    bc = bc->next;
                }
            }
        }
        if (result)
            *result = found;
        return true;
    }
    return false;
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
static bool trivialFunc(SYMBOL* func, bool move)
{
    SYMLIST* hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sym = hr->p;
        if (matchesCopy(sym, move))
        {
            return sym->sb->defaulted;
        }
        hr = hr->next;
    }
    return true;
}
static bool trivialCopyConstructible(TYPE* tp)
{
    if (isstructured(tp))
    {
        SYMLIST* hr;
        SYMBOL* ovl;
        BASECLASS* bc;
        ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            if (!trivialFunc(ovl, false) || !trivialFunc(ovl, true))
                return false;
        }
        bc = basetype(tp)->sp->sb->baseClasses;
        while (bc)
        {
            if (!trivialCopyConstructible(bc->cls->tp))
                return false;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->sb->storage_class == sc_mutable || sym->sb->storage_class == sc_member)
                if (!trivialCopyConstructible(sym->tp))
                    return false;
            hr = hr->next;
        }
    }
    return true;
}
static bool trivialCopyAssignable(TYPE* tp)
{
    if (isstructured(tp))
    {
        SYMLIST* hr;
        SYMBOL* ovl;
        BASECLASS* bc;
        ovl = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(tp)->syms);
        if (ovl)
        {
            if (!trivialFunc(ovl, false) || !trivialFunc(ovl, true))
                return false;
        }
        bc = basetype(tp)->sp->sb->baseClasses;
        while (bc)
        {
            if (!trivialCopyAssignable(bc->cls->tp))
                return false;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->sb->storage_class == sc_mutable || sym->sb->storage_class == sc_member)
                if (!trivialCopyAssignable(sym->tp))
                    return false;
            hr = hr->next;
        }
    }
    return true;
}
static bool trivialDestructor(TYPE* tp)
{
    if (isstructured(tp))
    {
        SYMLIST* hr;
        SYMBOL* ovl;
        BASECLASS* bc;
        ovl = search(overloadNameTab[CI_DESTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            ovl = (SYMBOL*)ovl->tp->syms->table[0]->p;
            if (!ovl->sb->defaulted)
                return false;
        }
        bc = basetype(tp)->sp->sb->baseClasses;
        while (bc)
        {
            if (!trivialDestructor(bc->cls->tp))
                return false;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->sb->storage_class == sc_mutable || sym->sb->storage_class == sc_member)
                if (!trivialDestructor(sym->tp))
                    return false;
            hr = hr->next;
        }
    }
    return true;
}
static bool trivialDefaultConstructor(TYPE* tp)
{
    if (isstructured(tp))
    {
        SYMLIST* hr;
        SYMBOL* ovl;
        BASECLASS* bc;
        ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
        if (ovl)
        {
            SYMLIST* hr = ovl->tp->syms->table[0];
            while (hr)
            {
                SYMBOL* sym = hr->p;
                SYMLIST* hr1 = basetype(sym->tp)->syms->table[0];

                if (!hr1->next || !hr1->next->next || (hr1->next->next->p)->tp->type == bt_void)
                {
                    if (!sym->sb->defaulted)
                        return false;
                    else
                        break;
                }
                hr = hr->next;
            }
        }
        bc = basetype(tp)->sp->sb->baseClasses;
        while (bc)
        {
            if (!trivialDefaultConstructor(bc->cls->tp))
                return false;
            bc = bc->next;
        }
        hr = basetype(tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->sb->storage_class == sc_mutable || sym->sb->storage_class == sc_member)
                if (!trivialDefaultConstructor(sym->tp))
                    return false;
            hr = hr->next;
        }
    }
    return true;
}
static bool triviallyCopyable(TYPE* tp)
{
    return trivialCopyConstructible(tp) && trivialCopyAssignable(tp) && trivialDestructor(tp);
}
static bool trivialStructure(TYPE* tp)
{
    if (isref(tp))
        tp = basetype(tp)->btp;
    if (isstructured(tp))
    {
        return triviallyCopyable(tp) && trivialDefaultConstructor(tp);
    }
    return false;
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
static bool trivialStructureWithBases(TYPE* tp)
{
    if (isstructured(tp))
    {
        BASECLASS* bc;
        if (!trivialStructure(tp))
            return false;
        bc = basetype(tp)->sp->sb->baseClasses;
        while (bc)
        {
            if (!trivialStructureWithBases(bc->cls->tp))
                return false;
            bc = bc->next;
        }
    }
    return true;
}
static bool isPOD(TYPE* tp)
{
    SYMBOL* found = nullptr;
    if (isStandardLayout(tp, &found) && trivialStructureWithBases(tp))  // DAL fixed
    {
        if (found)
        {
            SYMLIST* hr = basetype(found->tp)->syms->table[0];  // DAL fixed
            while (hr)
            {
                SYMBOL* sym = hr->p;
                if (isstructured(sym->tp) && !trivialStructureWithBases(sym->tp))
                    return false;
                hr = hr->next;
            }
        }
        return true;
    }
    return false;
}
static bool nothrowConstructible(TYPE* tp, INITLIST* args)
{
    if (isstructured(tp))
    {
        SYMBOL* ovl;
        // recursion will have been taken care of elsewhere...
        ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp)->syms);
        int i = 0;
        char holdl[100], holdr[100];
        INITLIST* temp;
        EXPRESSION* cexp = nullptr;
        if (ovl)
        {
            FUNCTIONCALL funcparams = {};
            funcparams.thisptr = intNode(en_c_i, 0);
            funcparams.thistp = Allocate<TYPE>();
            funcparams.thistp->type = bt_pointer;
            funcparams.thistp->btp = basetype(tp);
            funcparams.thistp->rootType = funcparams.thistp;
            funcparams.thistp->size = getSize(bt_pointer);
            funcparams.ascall = true;
            funcparams.arguments = args;
            temp = funcparams.arguments;
            i = 0;
            while (temp)
            {
                bool rref = basetype(temp->tp)->type == bt_rref;
                if (isref(temp->tp))
                    temp->tp = basetype(temp->tp)->btp;
                holdl[i] = temp->tp->lref;
                holdr[i] = temp->tp->rref;
                if (!temp->tp->rref && !rref)
                {
                    temp->tp->lref = true;
                }
                else if (rref)
                {
                    temp->tp->rref = true;
                }
                i++;
                temp = temp->next;
            }
            std::stack<SYMBOL*> stk;
            for (auto spl = ovl->tp->syms->table[0]; spl; spl = spl->next)
            {
                if (spl->p->templateParams)
                {
                    stk.push(spl->p);
                    PushPopTemplateArgs(spl->p, true);
                }
            }
            SYMBOL *sp = GetOverloadedFunction(&tp, &funcparams.fcall, ovl, &funcparams, nullptr, false, false, false, _F_SIZEOF);
            while (stk.size())
            {
                PushPopTemplateArgs(stk.top(), false);
                stk.pop();
            }
            temp = funcparams.arguments;
            i = 0;
            while (temp)
            {
                temp->tp->lref = holdl[i];
                temp->tp->rref = holdr[i];
                temp = temp->next;
                i++;
            }
            if (sp)
            {
                return sp->sb->xcMode == xc_none;
            }
        }
    }
    return true;
}
static bool is_abstract(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = isstructured(funcparams.arguments->tp) && basetype(funcparams.arguments->tp)->sp->sb->isabstract;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_base_of(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && funcparams.arguments->next && !funcparams.arguments->next->next)
    {
        if (isstructured(funcparams.arguments->tp) && isstructured(funcparams.arguments->next->tp))
            rv = classRefCount(basetype(funcparams.arguments->tp)->sp, basetype(funcparams.arguments->next->tp)->sp) != 0;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_class(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
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
    return true;
}
static bool is_constructible(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments)
    {
        TYPE* tp2 = funcparams.arguments->tp;
        if (isarray(tp2))
        {
            while (isarray(tp2) && tp2->size != 0)
                tp2 = tp2->btp;

            if (isarray(tp2))
            {
                tp2 = nullptr;
            }
        }
        if (tp2)
        {
            TYPE* tpf = tp2;
            if (isref(tp2))
            {
                tp2 = basetype(tp2)->btp;
                if (funcparams.arguments->next && !funcparams.arguments->next->next)
                {
                    TYPE* tpy = funcparams.arguments->next->tp;
                    if (isref(tpy))
                        tpy = basetype(tpy)->btp;
                    if (isconst(tpy) && !isconst(tp2) || isvolatile(tpy) && !isvolatile(tp2))
                    {
                        rv = false;
                        *exp = intNode(en_c_i, rv);
                        *tp = &stdint;
                        return true;
                    }
                    if (isstructured(tp2))
                    {                        
                        if (isstructured(tpy))
                        {
                            SYMBOL* sp2 = basetype(tp2)->sp;
                            SYMBOL* spy = basetype(tpy)->sp;
                            if (sp2->sb->mainsym)
                                sp2 = sp2->sb->mainsym;
                            if (spy->sb->mainsym)
                                spy = spy->sb->mainsym;
                            rv = sp2 == spy || sameTemplate(sp2->tp, spy->tp);
                        }
                        *exp = intNode(en_c_i, rv);
                        *tp = &stdint;
                        return true;
                    }
                }
            }
            if (isfunction(tp2))
            {
                if (funcparams.arguments->next && !funcparams.arguments->next->next)
                {
                    TYPE* tpy = funcparams.arguments->next->tp;
                    if (isref(tpf))
                    {
                        if (isref(tpy))
                            tpy = basetype(tpy)->btp;
                        rv = comparetypes(tp2, tpy, true);
                    }
                }
            }
            else if (isfuncptr(tp2))
            {
                if (funcparams.arguments->next && !funcparams.arguments->next->next)
                {
                    TYPE* tp3 = funcparams.arguments->next->tp;
                    if (isref(tp3))
                        tp3 = basetype(basetype(tp3)->btp);
                    if (isfunction(tp3))
                    {
                        rv = comparetypes(basetype(tp2)->btp, tp3, true);
                    }
                    else if (isstructured(tp3))
                    {
                        // look for operator () with args from tp2
                        SYMLIST* hr;
                        EXPRESSION* cexp = nullptr;
                        INITLIST** arg = &funcparams.arguments;
                        SYMBOL* bcall = search(overloadNameTab[CI_FUNC], basetype(tp3)->syms);
                        funcparams.thisptr = intNode(en_c_i, 0);
                        funcparams.thistp = Allocate<TYPE>();
                        funcparams.thistp->type = bt_pointer;
                        funcparams.thistp->btp = basetype(tp3);
                        funcparams.thistp->rootType = funcparams.thistp;
                        funcparams.thistp->size = getSize(bt_pointer);
                        funcparams.ascall = true;
                        funcparams.arguments = nullptr;
                        funcparams.sp = nullptr;
                        hr = basetype(basetype(tp2)->btp)->syms->table[0];
                        while (hr)
                        {
                            *arg = Allocate<INITLIST>();
                            (*arg)->tp = hr->p->tp;
                            (*arg)->exp = intNode(en_c_i, 0);
                            arg = &(*arg)->next;
                            hr = hr->next;
                        }
                        rv = GetOverloadedFunction(tp, &funcparams.fcall, bcall, &funcparams, nullptr, false, false, false,
                                                   _F_SIZEOF) != nullptr;
                    }
                    else
                    {
                        // compare two function pointers...
                        rv = comparetypes(tp2, tp3, true);
                    }
                }
                else
                {
                    rv = true;
                }
            }
            else if (basetype(tp2)->type == bt_memberptr)
            {
                if (funcparams.arguments->next && !funcparams.arguments->next->next)
                {
                    TYPE* tp3 = funcparams.arguments->next->tp;
                    if (isref(tp3))
                        tp3 = basetype(basetype(tp3)->btp);
                    if (tp3->type == bt_memberptr)
                    {
                        tp2 = basetype(tp2);
                        SYMBOL* s1 = tp2->sp;
                        SYMBOL* s2 = tp3->sp;
                        if (s1->sb->mainsym)
                            s1 = s1->sb->mainsym;
                        if (s2->sb->mainsym)
                            s2 = s2->sb->mainsym;
                        if (s1 == s2 || sameTemplate(s1->tp, s2->tp))
                        {
                            rv = comparetypes(tp2->btp, tp3->btp, true);
                        }
                    }
                    else if (isfunction(tp3))
                    {
                        tp2 = basetype(tp2);
                        SYMBOL* s1 = tp2->sp;
                        SYMBOL* s2 = tp3->sp;
                        if (s2)
                        {
                            SYMLIST* hr = s2->tp->syms->table[0];
                            if (hr && hr->p->sb->thisPtr)
                            {
                                s2 = basetype(basetype(hr->p->tp)->btp)->sp;
                                if (s1->sb->mainsym)
                                    s1 = s1->sb->mainsym;
                                if (s2->sb->mainsym)
                                    s2 = s2->sb->mainsym;
                                if (s1 == s2 || sameTemplate(s1->tp, s2->tp))
                                {
                                    rv = comparetypes(tp2->btp, tp3, true);
                                }
                            }
                        }
                    }
                }
                else
                {
                    rv = true;
                }
            }
            else if (isarithmetic(tp2) || ispointer(tp2) || basetype(tp2)->type == bt_enum)
            {
                if (!funcparams.arguments->next)
                {
                    rv = true;
                }
                else if (!funcparams.arguments->next->next)
                {
                    rv = comparetypes(tp2, funcparams.arguments->next->tp, true);
                }
            }
            else if (isstructured(tp2))
            {
                if (funcparams.arguments->next && isstructured(funcparams.arguments->next->tp) &&
                    (comparetypes(tp2, funcparams.arguments->next->tp, true) || sameTemplate(tp2, funcparams.arguments->next->tp)))
                {
                    rv = true;
                }
                else
                {
                    int i = 0;
                    char holdl[100], holdr[100];
                    INITLIST* temp;
                    EXPRESSION* cexp = nullptr;
                    SYMBOL* cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(tp2)->syms);
                    if (cons)
                    {
                        funcparams.thisptr = intNode(en_c_i, 0);
                        funcparams.thistp = Allocate<TYPE>();
                        funcparams.thistp->type = bt_pointer;
                        funcparams.thistp->btp = basetype(tp2);
                        funcparams.thistp->rootType = funcparams.thistp;
                        funcparams.thistp->size = getSize(bt_pointer);
                        funcparams.ascall = true;
                        funcparams.arguments = funcparams.arguments->next;
                        temp = funcparams.arguments;
                        i = 0;
                        while (temp)
                        {
                            bool rref = basetype(temp->tp)->type == bt_rref;
                            if (isref(temp->tp))
                                temp->tp = basetype(temp->tp)->btp;
                            holdl[i] = temp->tp->lref;
                            holdr[i] = temp->tp->rref;
                            if (!temp->tp->rref && !rref)
                            {
                                temp->tp->lref = true;
                            }
                            else if (rref)
                            {
                                temp->tp->rref = true;
                            }
                            i++;
                            temp = temp->next;
                        }
                        std::stack<SYMBOL*> stk;
                        for (auto spl = cons->tp->syms->table[0]; spl; spl = spl->next)
                        {
                            if (spl->p->templateParams)
                            {
                                stk.push(spl->p);
                                PushPopTemplateArgs(spl->p, true);
                            }
                        }
                        rv = GetOverloadedFunction(tp, &funcparams.fcall, cons, &funcparams, nullptr, false, false, false, _F_SIZEOF) !=
                            nullptr;
                        while (stk.size())
                        {
                            PushPopTemplateArgs(stk.top(), false);
                            stk.pop();
                        }
                        temp = funcparams.arguments;
                        i = 0;
                        while (temp)
                        {
                            temp->tp->lref = holdl[i];
                            temp->tp->rref = holdr[i];
                            temp = temp->next;
                            i++;
                        }
                    }
                }
            }
        }
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_convertible_to(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    bool rv = true;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && funcparams.arguments->next && !funcparams.arguments->next->next)
    {
        TYPE* from = funcparams.arguments->tp;
        TYPE* to = funcparams.arguments->next->tp;
        if (isref(from))
        {
            if (isref(to))
            {
                if (basetype(to)->type == bt_lref)
                    if (basetype(from)->type == bt_rref)
                        rv = false;
            }
        }
        if (isfunction(from))
            from = basetype(from)->btp;
        if (rv)
        {
            while (isref(from))
                from = basetype(from)->btp;
            while (isref(to))
                to = basetype(to)->btp;
            while (ispointer(from) && ispointer(to))
            {
                from = basetype(from)->btp;
                to = basetype(to)->btp;
            }
            if (to->type == bt_templateparam)
                to = to->templateParam->p->byClass.val;
            if (from->type == bt_templateparam)
                from = from->templateParam->p->byClass.val;
            rv = comparetypes(to, from, false);
            if (!rv && isstructured(from) && isstructured(to))
            {
                if (classRefCount(basetype(to)->sp, basetype(from)->sp) == 1)
                    rv = true;
                else if (lookupGenericConversion(basetype(from)->sp, basetype(to)))
                    rv = true;
            }
            if (!rv && isstructured(from))
            {
                CI_CONSTRUCTOR;
                SYMBOL* sym = search(overloadNameTab[CI_CAST], basetype(from)->syms);
                if (sym)
                {
                    SYMLIST* hr = sym->tp->syms->table[0];
                    while (hr)
                    {
                        if (comparetypes(basetype(hr->p->tp)->btp, to, false))
                        {
                            rv = true;
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
        rv = false;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_empty(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = !basetype(funcparams.arguments->tp)->syms->table[0] || !basetype(funcparams.arguments->tp)->syms->table[0]->next;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_enum(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
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
    return true;
}
static bool is_final(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = basetype(funcparams.arguments->tp)->sp->sb->isfinal;
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_literal(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
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
    return true;
}
static bool is_nothrow_constructible(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && funcparams.arguments->next)
    {
        TYPE* tp2 = funcparams.arguments->tp;
        if (isref(tp2))
        {
            tp2 = basetype(tp2)->btp;
            TYPE* tpy = funcparams.arguments->next->tp;
            if (isref(tpy))
                tpy = basetype(tpy)->btp;
            if (isconst(tpy) && !isconst(tp2) || isvolatile(tpy) && !isvolatile(tp2))
            {
                rv = false;
                *exp = intNode(en_c_i, rv);
                *tp = &stdint;
                return true;
            }
        }
        if (isstructured(tp2))
        {
            if (!basetype(tp2)->sp->sb->trivialCons)
                rv = nothrowConstructible(funcparams.arguments->tp, funcparams.arguments->next);
            else
	        rv = comparetypes(tp2, funcparams.arguments->next->tp, true);
        }
        else
        {
            rv = comparetypes(tp2, funcparams.arguments->next->tp, true);
        }
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_pod(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = isarithmetic(funcparams.arguments->tp) || !!isPOD(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_polymorphic(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        // yes references are literal types...
        if (isstructured(funcparams.arguments->tp))
            rv = !!hasVTab(basetype(funcparams.arguments->tp)->sp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_standard_layout(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = isarithmetic(funcparams.arguments->tp) || !!isStandardLayout(funcparams.arguments->tp, nullptr);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivial(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = !isstructured(funcparams.arguments->tp) || !!trivialStructure(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivially_assignable(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = trivialCopyAssignable(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivially_constructible(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        rv = trivialCopyConstructible(funcparams.arguments->tp) && trivialDefaultConstructor(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_trivially_copyable(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
    FUNCTIONCALL funcparams;
    memset(&funcparams, 0, sizeof(funcparams));
    funcparams.sp = sym;
    *lex = getTypeList(*lex, funcsp, &funcparams.arguments);
    lst = funcparams.arguments;
    while (lst)
    {
        lst->tp = PerformDeferredInitialization(lst->tp, nullptr);
        lst = lst->next;
    }
    if (funcparams.arguments && !funcparams.arguments->next)
    {
        if (isstructured(funcparams.arguments->tp))
            rv = triviallyCopyable(funcparams.arguments->tp);
    }
    *exp = intNode(en_c_i, rv);
    *tp = &stdint;
    return true;
}
static bool is_union(LEXEME** lex, SYMBOL* funcsp, SYMBOL* sym, TYPE** tp, EXPRESSION** exp)
{
    INITLIST* lst;
    bool rv = false;
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
    return true;
}
}  // namespace Parser