/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
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
#include "rtti.h"
#include <stack>
#include <unordered_set>
#include "ccerr.h"
#include "config.h"
#include "template.h"
#include "stmt.h"
#include "mangle.h"
#include "initbackend.h"
#include "expr.h"
#include "help.h"
#include "lex.h"
#include "cpplookup.h"
#include "declcpp.h"
#include "declare.h"
#include "constopt.h"
#include "memory.h"
#include "inline.h"
#include "init.h"
#include "beinterf.h"
#include "types.h"
#include "lex.h"
#include "declcons.h"
#include "libcxx.h"
#include "symtab.h"
#include "ListFactory.h"
namespace Parser
{
std::set<SYMBOL*> defaultRecursionMap;
bool noExcept = true;

static void genAsnCall(std::list<BLOCKDATA*>& b, SYMBOL* cls, SYMBOL* base, int offset, EXPRESSION* thisptr, EXPRESSION* other, bool move,
                       bool isconst);
void createDestructor(SYMBOL* sp);

void ConsDestDeclarationErrors(SYMBOL* sp, bool notype)
{
    if (sp->sb->isConstructor)
    {
        if (!notype)
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_TYPE);
        else if (sp->sb->storage_class == sc_virtual)
            errorstr(ERR_INVALID_STORAGE_CLASS, "virtual");
        else if (sp->sb->storage_class == sc_static)
            errorstr(ERR_INVALID_STORAGE_CLASS, "static");
        else if (isconst(sp->tp) || isvolatile(sp->tp))
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_CONST_VOLATILE);
    }
    else if (sp->sb->isDestructor)
    {
        if (!notype)
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_TYPE);
        else if (sp->sb->storage_class == sc_static)
            errorstr(ERR_INVALID_STORAGE_CLASS, "static");
        else if (isconst(sp->tp) || isvolatile(sp->tp))
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_CONST_VOLATILE);
    }
    else if (sp->sb->parentClass && !strcmp(sp->name, sp->sb->parentClass->name))
    {
        error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_TYPE);
    }
}
static bool HasConstexprConstructorInternal(SYMBOL* sym)
{
    sym = search(sym->tp->syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (sym)
    {
        for (auto sp : *sym->tp->syms)
        {
            if (sp->sb->constexpression)
                return true;
        }
    }
    return false;
}
static bool HasConstexprConstructor(TYPE* tp)
{
    auto sym = basetype(tp)->sp;
    if (HasConstexprConstructorInternal(sym))
        return true;
    if (sym->sb->specializations)
        for (auto specialize : *sym->sb->specializations)
            if (HasConstexprConstructorInternal(specialize))
                return true;
     return false;
}
void ConstexprMembersNotInitializedErrors(SYMBOL* cons)
{
    if (!templateNestingCount || instantiatingTemplate)
    {
        for (auto sym : *cons->tp->syms)
        {
            if (sym->sb->constexpression)
            {
                std::unordered_set<std::string> initialized;
                if (sym->sb->memberInitializers)
                    for (auto m : *sym->sb->memberInitializers)
                     initialized.insert(m->name);
                for (auto sp : *sym->sb->parentClass->tp->syms)
                {
                    if (!sp->sb->init && ismemberdata(sp))
                    {
                        if (initialized.find(sp->name) == initialized.end())
                        {
                            // this should check the actual base class constructor in use
                            // but that would be difficult to get at this point.
                            if (!isstructured(sp->tp) || !HasConstexprConstructor(sp->tp))
                                errorsym(ERR_CONSTEXPR_MUST_INITIALIZE, sp);
                        }
                    }
                }
            }
        }
    }
}
LEXLIST* FindClass(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** sym)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    TYPE* castType = nullptr;
    char buf[512];
    int ov = 0;
    bool namespaceOnly = false;

    *sym = nullptr;

    if (MATCHKW(lex, classsel))
        namespaceOnly = true;
    lex = nestedPath(lex, &encloser, &ns, &throughClass, true, sc_global, false);
    lex = getIdName(lex, funcsp, buf, &ov, &castType);
    if (buf[0])
    {
        *sym = finishSearch(buf, encloser, ns, false, throughClass, namespaceOnly);  // undefined in local context
    }
    return lex;
}
std::list<MEMBERINITIALIZERS*>* GetMemberInitializers(LEXLIST **lex2, SYMBOL* funcsp, SYMBOL* sym)
{
    (void)sym;
    std::list<MEMBERINITIALIZERS*>* rv = memberInitializersListFactory.CreateList();
    LEXLIST *lex = *lex2, *last = nullptr;
    //    if (sym->name != overloadNameTab[CI_CONSTRUCTOR])
    //        error(ERR_INITIALIZER_LIST_REQUIRES_CONSTRUCTOR);
    while (lex != nullptr)
    {
        if (ISID(lex) || MATCHKW(lex, classsel))
        {
            SYMBOL* sym = nullptr;
            lex = FindClass(lex, funcsp, &sym);
            LEXLIST** mylex;
            char name[1024];
            auto v = Allocate<MEMBERINITIALIZERS>();
            v->line = lex->data->errline;
            v->file = lex->data->errfile;
            mylex = &v->initData;
            name[0] = 0;
            if (ISID(lex))
            {
                strcpy(name, lex->data->value.s.a);
                lex = getsym();
            }
            v->name = litlate(name);
            if (sym && istype(sym))
                v->basesym = sym;
            if (MATCHKW(lex, lt))
            {
                int paren = 0, tmpl = 0;
                *mylex = Allocate<LEXLIST>();
                **mylex = *lex;
                (*mylex)->prev = last;
                last = *mylex;
                mylex = &(*mylex)->next;
                lex = getsym();
                while (lex && (!MATCHKW(lex, gt) || paren || tmpl))
                {
                    if (MATCHKW(lex, openpa))
                        paren++;
                    if (MATCHKW(lex, closepa))
                        paren--;
                    if (!paren && MATCHKW(lex, lt))
                        tmpl++;
                    if (!paren && (MATCHKW(lex, gt) || MATCHKW(lex, rightshift)))
                        tmpl--;
                    if (lex->data->type == l_id)
                        lex->data->value.s.a = litlate(lex->data->value.s.a);
                    *mylex = Allocate<LEXLIST>();
                    if (MATCHKW(lex, rightshift))
                    {
                        lex = getGTSym(lex);
                        **mylex = *lex;
                        (*mylex)->data = Allocate<LEXEME>();
                        *(*mylex)->data = *lex->data;
                    }
                    else
                    {
                        **mylex = *lex;
                        lex = getsym();
                    }
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                }
                if (MATCHKW(lex, gt))
                {
                    *mylex = Allocate<LEXLIST>();
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
            }
            if (MATCHKW(lex, openpa) || MATCHKW(lex, begin))
            {
                enum e_kw open = KW(lex), close = open == openpa ? closepa : end;
                int paren = 0;
                *mylex = Allocate<LEXLIST>();
                **mylex = *lex;
                (*mylex)->prev = last;
                last = *mylex;
                mylex = &(*mylex)->next;
                lex = getsym();
                while (lex && (!MATCHKW(lex, close) || paren))
                {
                    if (MATCHKW(lex, open))
                        paren++;
                    if (MATCHKW(lex, close))
                        paren--;
                    if (lex->data->type == l_id)
                        lex->data->value.s.a = litlate(lex->data->value.s.a);
                    *mylex = Allocate<LEXLIST>();
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
                if (MATCHKW(lex, close))
                {
                    *mylex = Allocate<LEXLIST>();
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
                if (MATCHKW(lex, ellipse))
                {
                    v->packed = true;
                    lex = getsym();
                }
                (*mylex) = nullptr;
            }
            else
            {
                error(ERR_MEMBER_INITIALIZATION_REQUIRED);
                skip(&lex, closepa);
                break;
            }
            rv->push_back(v);
        }
        else
        {
            error(ERR_MEMBER_NAME_REQUIRED);
        }
        if (!MATCHKW(lex, comma))
            break;
        lex = getsym();
    }
    *lex2 = lex;
    return rv;
}
void SetParams(SYMBOL* cons)
{
    // c style only
    int base = Optimizer::chosenAssembler->arch->retblocksize;
    if (isstructured(basetype(cons->tp)->btp) || basetype(basetype(cons->tp)->btp)->type == bt_memberptr)
    {
        // handle structured return values
        base += getSize(bt_pointer);
        if (base % Optimizer::chosenAssembler->arch->parmwidth)
            base += Optimizer::chosenAssembler->arch->parmwidth - base % Optimizer::chosenAssembler->arch->parmwidth;
    }
    for (auto sp : *basetype(cons->tp)->syms)
    {
        assignParam(cons, &base, sp);
    }
    cons->sb->paramsize = base - Optimizer::chosenAssembler->arch->retblocksize;
}
SYMBOL* insertFunc(SYMBOL* sp, SYMBOL* ovl)
{
    SYMBOL* funcs = search(basetype(sp->tp)->syms, ovl->name);
    ovl->sb->parentClass = sp;
    ovl->sb->internallyGenned = true;
    ovl->sb->attribs.inheritable.linkage4 = lk_virtual;
    ovl->sb->defaulted = true;
    ovl->sb->access = ac_public;
    ovl->sb->templateLevel = templateNestingCount;
    if (!ovl->sb->decoratedName)
        SetLinkerNames(ovl, lk_cdecl);
    if (!funcs)
    {
        TYPE* tp = MakeType(bt_aggregate);
        funcs = makeID(sc_overloads, tp, 0, ovl->name);
        funcs->sb->parentClass = sp;
        tp->sp = funcs;
        SetLinkerNames(funcs, lk_cdecl);
        basetype(sp->tp)->syms->Add(funcs);
        funcs->sb->parent = sp;
        funcs->tp->syms = symbols.CreateSymbolTable();
        funcs->tp->syms->Add(ovl);
        ovl->sb->overloadName = funcs;
    }
    else if (funcs->sb->storage_class == sc_overloads)
    {
        funcs->tp->syms->insertOverload(ovl);
        ovl->sb->overloadName = funcs;
    }
    else
    {
        diag("insertFunc: invalid overload tab");
    }
    injectThisPtr(ovl, basetype(ovl->tp)->syms);
    SetParams(ovl);
    return ovl;
}
static bool BaseWithVirtualDestructor(SYMBOL* sp)
{
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
        {
            SYMBOL* dest = search(b->cls->tp->syms, overloadNameTab[CI_DESTRUCTOR]);
            if (dest)
            {
                dest = dest->tp->syms->front();
                if (dest->sb->storage_class == sc_virtual)
                    return true;
            }
        }
    return false;
}
static SYMBOL* declareDestructor(SYMBOL* sp)
{
    SYMBOL* rv;
    SYMBOL *func, *sp1;
    TYPE* tp = MakeType(bt_func, MakeType(bt_void));
    func = makeID(BaseWithVirtualDestructor(sp) ? sc_virtual : sc_member, tp, nullptr, overloadNameTab[CI_DESTRUCTOR]);
    func->sb->xcMode = xc_none;
    func->sb->noExcept = true;
    //    func->sb->attribs.inheritable.linkage2 = sp->sb->attribs.inheritable.linkage2;
    tp->syms = symbols.CreateSymbolTable();
    sp1 = makeID(sc_parameter, tp->btp, nullptr, AnonymousName());
    tp->syms->Add(sp1);
    if (sp->sb->vbaseEntries)
    {
        sp1 = makeID(sc_parameter, &stdint, nullptr, AnonymousName());
        sp1->sb->isDestructor = true;
        tp->syms->Add(sp1);
    }
    rv = insertFunc(sp, func);
    rv->sb->isDestructor = true;
    bool found = false;
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
            if (!b->cls->sb->pureDest)
            {
                found = true;
                break;
            }
    if (!found)
    {
        if (sp->sb->vbaseEntries)
            for (auto e : *sp->sb->vbaseEntries)
                if (!e->cls->sb->pureDest)
                {
                    found = true;
                    break;
                }
        if (!found)
        {
            for (auto cls : *basetype(sp->tp)->syms)
            {
                TYPE* tp = cls->tp;
                while (isarray(tp))
                    tp = basetype(tp)->btp;
                if (isstructured(tp) && !basetype(tp)->sp->sb->pureDest)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                sp->sb->pureDest = true;
            }
        }
    }

    return rv;
}
static bool hasConstFunc(SYMBOL* sp, int type, bool move)
{
    SYMBOL* ovl = search(basetype(sp->tp)->syms, overloadNameTab[type]);
    if (ovl)
    {
        for (auto func : *basetype(ovl->tp)->syms)
        {
            auto it = basetype(func->tp)->syms->begin();
            ++it;
            if (it != basetype(func->tp)->syms->end())
            {
                auto arg = *it;
                ++it;
                if (it == basetype(func->tp)->syms->end() || (*it)->sb->init || (*it)->sb->deferredCompile)
                {
                    if (isref(arg->tp))
                    {
                        if (isstructured(basetype(arg->tp)->btp))
                        {
                            if (basetype(basetype(arg->tp)->btp)->sp == sp || sameTemplate(basetype(basetype(arg->tp)->btp), sp->tp))
                            {
                                if ((basetype(arg->tp)->type == bt_lref && !move) || (basetype(arg->tp)->type == bt_rref && move))
                                {
                                    return isconst(basetype(arg->tp)->btp);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
static bool constCopyConstructor(SYMBOL* sp)
{
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
            if (!b->isvirtual && !hasConstFunc(b->cls, CI_CONSTRUCTOR, false))
                return false;
    if (sp->sb->vbaseEntries)
        for (auto e : *sp->sb->vbaseEntries)
            if (e->alloc && !hasConstFunc(e->cls, CI_CONSTRUCTOR, false))
                return false;
    for (auto cls : *basetype(sp->tp)->syms)
        if (isstructured(cls->tp) && cls->sb->storage_class != sc_typedef && !cls->sb->trivialCons)
            if (!hasConstFunc(basetype(cls->tp)->sp, CI_CONSTRUCTOR, false))
                return false;
    return true;
}
static SYMBOL* declareConstructor(SYMBOL* sp, bool deflt, bool move)
{
    SYMBOL *func, *sp1;
    TYPE* tp = MakeType(bt_func, MakeType(bt_void));
    func = makeID(sc_member, tp, nullptr, overloadNameTab[CI_CONSTRUCTOR]);
    func->sb->isConstructor = true;
    //    func->sb->attribs.inheritable.linkage2 = sp->sb->attribs.inheritable.linkage2;
    sp1 = makeID(sc_parameter, nullptr, nullptr, AnonymousName());
    tp->syms = symbols.CreateSymbolTable();
    tp->syms->Add(sp1);
    if (deflt)
    {
        sp1->tp = MakeType(bt_void);
    }
    else
    {
        sp1->tp = MakeType(move ? bt_rref : bt_lref, basetype(sp->tp));
        if (!move && constCopyConstructor(sp))
        {
            sp1->tp->btp = MakeType(bt_const, sp1->tp->btp);
        }
    }
    UpdateRootTypes(sp1->tp);
    return insertFunc(sp, func);
}
static bool constAssignmentOp(SYMBOL* sp, bool move)
{
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
            if (!b->isvirtual && !hasConstFunc(b->cls, assign - kw_new + CI_NEW, move))
                return false;
    if (sp->sb->vbaseEntries)
        for (auto e : *sp->sb->vbaseEntries)
            if (e->alloc && !hasConstFunc(e->cls, assign - kw_new + CI_NEW, move))
                return false;
    for (auto cls : *basetype(sp->tp)->syms)
        if (isstructured(cls->tp) && cls->sb->storage_class != sc_typedef && !cls->sb->trivialCons)
            if (!hasConstFunc(basetype(cls->tp)->sp, assign - kw_new + CI_NEW, move))
                return false;
    return true;
}
static SYMBOL* declareAssignmentOp(SYMBOL* sp, bool move)
{
    SYMBOL *func, *sp1;
    TYPE* tp = MakeType(bt_func, basetype(sp->tp));
    TYPE* tpx;
    if (isstructured(sp->tp))
    {
        tp->btp = MakeType(move ? bt_rref : bt_lref, tp->btp);
    }
    UpdateRootTypes(tp);
    func = makeID(sc_member, tp, nullptr, overloadNameTab[assign - kw_new + CI_NEW]);
    sp1 = makeID(sc_parameter, nullptr, nullptr, AnonymousName());
    tp->syms = symbols.CreateSymbolTable();
    tp->syms->Add(sp1);
    sp1->tp = MakeType(move ? bt_rref : bt_lref, basetype(sp->tp));
    if (constAssignmentOp(sp, move))
    {
        sp1->tp->btp = MakeType(bt_const, sp1->tp->btp);
    }
    UpdateRootTypes(sp1->tp);
    return insertFunc(sp, func);
}
bool matchesDefaultConstructor(SYMBOL* sp)
{
    auto it = basetype(sp->tp)->syms->begin();
    ++it;
    if (it != basetype(sp->tp)->syms->end())
    {
        SYMBOL* arg1 = *it;
        if (arg1->tp->type == bt_void || arg1->sb->init || arg1->sb->deferredCompile)
            return true;
    }
    return false;
}
bool matchesCopy(SYMBOL* sp, bool move)
{
    auto it = basetype(sp->tp)->syms->begin();
    ++it;
    if (it != basetype(sp->tp)->syms->end())
    {
        SYMBOL* arg1 = *it;
        ++it;
        if (it == basetype(sp->tp)->syms->end() || (*it)->sb->init || (*it)->sb->deferredCompile || (*it)->sb->constop)
        {
            if (basetype(arg1->tp)->type == (move ? bt_rref : bt_lref))
            {
                TYPE* tp = basetype(arg1->tp)->btp;
                if (isstructured(tp))
                    if (basetype(tp)->sp == sp->sb->parentClass || basetype(tp)->sp == sp->sb->parentClass->sb->mainsym ||
                        basetype(tp)->sp->sb->mainsym == sp->sb->parentClass || sameTemplate(tp, sp->sb->parentClass->tp))
                        return true;
            }
        }
    }
    return false;
}
static bool hasCopy(SYMBOL* func, bool move)
{
    for (auto sp : *basetype(func->tp)->syms)
    {
        if (!sp->sb->internallyGenned && matchesCopy(sp, move))
            return true;
    }
    return false;
}
static bool checkDest(SYMBOL* sp, SYMBOL* parent, SymbolTable<SYMBOL>* syms, enum e_ac access)
{
    SYMBOL* dest = search(syms, overloadNameTab[CI_DESTRUCTOR]);

    if (dest)
    {
        dest = basetype(dest->tp)->syms->front();
        if (dest->sb->deleted)
            return true;
        if (dest->sb->access < access)
            return true;
    }
    return false;
}
static bool noexceptDest(SYMBOL* sp) 
{
    SYMBOL* dest = search(sp->tp->syms, overloadNameTab[CI_DESTRUCTOR]);

    if (dest)
    {
        dest = basetype(dest->tp)->syms->front();
        return dest->sb->noExcept;
    }
    return true;
}

static bool checkDefaultCons(SYMBOL* sp, SymbolTable<SYMBOL>* syms, enum e_ac access)
{
    SYMBOL* cons = search(syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (cons)
    {
        SYMBOL* dflt = nullptr;
        for (auto cur : *basetype(cons->tp)->syms)
        {
            if (matchesDefaultConstructor(cur))
            {
                if (dflt)
                    return true;  // ambiguity
                dflt = cur;
            }
        }
        if (dflt)
        {
            if (dflt->sb->deleted)
                return true;
            if (dflt->sb->access < access)
                return true;
        }
        else
        {
            return true;
        }
    }
    return false;
}
SYMBOL* getCopyCons(SYMBOL* base, bool move)
{
    (void)move;
    SYMBOL* ovl = search(basetype(base->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (ovl)
    {
        for (auto sym2 : *basetype(ovl->tp)->syms)
        {
            auto itArgs = basetype(sym2->tp)->syms->begin();
            auto itArgsend = basetype(sym2->tp)->syms->end();
            auto sym = *itArgs;
            SYMBOL* sym1 = nullptr;
            if (sym->sb->thisPtr)
            {
                ++itArgs;
                if (itArgs != itArgsend)
                    sym = *itArgs;
            }
            auto it1 = itArgs;
            ++it1;
            if (itArgs != itArgsend && it1 != itArgsend )
            {
                sym1 = *it1;
            }
            if (itArgs != itArgsend && (!sym1 || sym1->sb->init || sym1->sb->deferredCompile))
            {
                TYPE* tp = basetype(sym->tp);
                if (tp->type == (move ? bt_rref : bt_lref))
                {
                    tp = basetype(tp->btp);
                    if (isstructured(tp))
                    {
                        if (!base->tp->sp)
                        {
                            continue;
                        }
                        if (tp->sp == base->tp->sp || tp->sp == base->tp->sp->sb->mainsym || sameTemplate(tp, base->tp))
                        {
                            return sym2;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}
static SYMBOL* GetCopyAssign(SYMBOL* base, bool move)
{
    (void)move;
    SYMBOL* ovl = search(basetype(base->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
    if (ovl)
    {
        for (auto sym2 : *basetype(ovl->tp)->syms)
        {
            auto itArgs = basetype(sym2->tp)->syms->begin();
            auto itArgsend = basetype(sym2->tp)->syms->end();
            auto sym = *itArgs;
            SYMBOL* sym1 = nullptr;
            if (sym->sb->thisPtr)
            {
                ++itArgs;
                if (itArgs != itArgsend)
                    sym = *itArgs;
            }
            auto it1 = itArgs;
            ++it1;
            if (itArgs != itArgsend && it1 != itArgsend)
            {
                sym1 = *it1;
            }
            if (itArgs != itArgsend && (!sym1 || sym1->sb->init || sym1->sb->deferredCompile))
            {
                TYPE* tp = basetype(sym->tp);
                if (tp->type == (move ? bt_rref : bt_lref))
                {
                    tp = basetype(tp->btp);
                    if (isstructured(tp))
                    {
                        if (comparetypes(tp, base->tp, true) || sameTemplate(tp, base->tp))
                        {
                            return sym2;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}
bool hasVTab(SYMBOL* sp)
{
    if (sp->sb->vtabEntries)
        for (auto vt : *sp->sb->vtabEntries)
            if (vt->virtuals)
                return true;
    return false;
}
static bool hasTrivialCopy(SYMBOL* sp, bool move)
{
    SYMBOL* dflt;
    if (sp->sb->vbaseEntries || hasVTab(sp))
        return false;
    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            dflt = getCopyCons(base->cls, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    for (auto cls : *basetype(sp->tp)->syms)
    {
        if (isstructured(cls->tp))
        {
            dflt = getCopyCons(basetype(cls->tp)->sp, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    }
    return true;
}
static bool hasTrivialAssign(SYMBOL* sp, bool move)
{
    SYMBOL* dflt;
    if (sp->sb->vbaseEntries || hasVTab(sp))
        return false;
    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            dflt = GetCopyAssign(base->cls, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    for (auto cls : *basetype(sp->tp)->syms)
    {
        if (isstructured(cls->tp))
        {
            dflt = getCopyCons(basetype(cls->tp)->sp, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    }
    return true;
}
static bool checkCopyCons(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = getCopyCons(base, false);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    else
    {
        return true;
    }
    return false;
}
static bool checkCopyAssign(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = GetCopyAssign(base, false);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    else
    {
        return true;
    }
    return false;
}
static bool checkMoveCons(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = getCopyCons(base, true);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    return false;
}
static bool checkMoveAssign(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = GetCopyAssign(base, true);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    else
    {
        if (!hasTrivialAssign(sp, true))
            return true;
    }
    return false;
}
static bool isDefaultDeleted(SYMBOL* sp)
{
    if (basetype(sp->tp)->type == bt_union)
    {
        bool allconst = true;
        for (auto sp1 : *basetype(sp->tp)->syms)
        {
            if (!isconst(sp1->tp) && sp1->tp->type != bt_aggregate)
                allconst = false;
            if (isstructured(sp1->tp))
            {
                SYMBOL* consovl = search(basetype(sp1->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
                for (auto cons : *basetype(consovl->tp)->syms)
                {
                    if (matchesDefaultConstructor(cons))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
        if (allconst)
            return true;
    }
    for (auto sp1 : *basetype(sp->tp)->syms)
    {
        TYPE* m;
        if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
        {
            if (isref(sp1->tp))
                if (!sp1->sb->init)
                    return true;
            if (basetype(sp1->tp)->type == bt_union)
            {
                bool found = false;
                for (auto member : *basetype(sp1->tp)->syms)
                {
                    if (!isconst(member->tp) && basetype(member->tp)->type != bt_aggregate)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    return true;
            }
            if (isstructured(sp1->tp))
            {
                TYPE* tp = basetype(sp1->tp);
                if (checkDest(sp, tp->sp, basetype(tp->sp->tp)->syms, ac_public))
                    return true;
            }
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                TYPE* tp = basetype(m);
                if (checkDefaultCons(sp, basetype(tp->sp->tp)->syms, ac_public))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            if (checkDest(sp, base->cls, basetype(base->cls->tp)->syms, ac_protected))
                return true;
            if (checkDefaultCons(sp, basetype(base->cls->tp)->syms, ac_protected))
                return true;
        }
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                if (checkDest(sp, vbase->cls, basetype(vbase->cls->tp)->syms, ac_protected))
                    return true;
                if (checkDefaultCons(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                    return true;
            }
        }
    return false;
}
static bool isCopyConstructorDeleted(SYMBOL* sp)
{
    if (basetype(sp->tp)->type == bt_union)
    {
        for (auto sp1 : *basetype(sp->tp)->syms)
        {
            if (isstructured(sp1->tp))
            {
                SYMBOL* consovl = search(basetype(sp1->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
                for (auto cons : *basetype(consovl->tp)->syms)
                {
                    if (matchesCopy(cons, false))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
    }
    for (auto sp1 :*basetype(sp->tp)->syms)
    {
        TYPE* m;
        if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
        {
            if (basetype(sp1->tp)->type == bt_rref)
                return true;
            if (isstructured(sp1->tp))
            {
                TYPE* tp = basetype(sp1->tp);
                if (checkDest(sp, tp->sp, basetype(tp->sp->tp)->syms, ac_public))
                    return true;
            }
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                if (checkCopyCons(sp, basetype(m)->sp, ac_public))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            if (checkDest(sp, base->cls, basetype(base->cls->tp)->syms, ac_protected))
                return true;
            if (checkCopyCons(sp, base->cls, ac_protected))
                return true;
        }
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                if (checkDest(sp, vbase->cls, basetype(vbase->cls->tp)->syms, ac_protected))
                    return true;
                if (checkCopyCons(sp, vbase->cls, ac_protected))
                    return true;
            }
        }
    return false;
}
static bool isCopyAssignmentDeleted(SYMBOL* sp)
{
    if (basetype(sp->tp)->type == bt_union)
    {
        for (auto sp1 : *basetype(sp->tp)->syms)
        {
            if (isstructured(sp1->tp))
            {
                SYMBOL* consovl = search(basetype(sp1->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
                for (auto cons : *basetype(consovl->tp)->syms)
                {
                    if (matchesCopy(cons, false))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
    }
    for (auto sp1 : *basetype(sp->tp)->syms)
    {
        TYPE* m;
        if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
        {
            if (isref(sp1->tp))
                return true;
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (!isstructured(m) && isconst(m) && m->type != bt_aggregate)
                return true;


            if (isstructured(m))
            {
                if (checkCopyAssign(sp, basetype(m)->sp, ac_public))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (checkCopyAssign(sp, base->cls, ac_protected))
                return true;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc && checkCopyAssign(sp, vbase->cls, ac_protected))
                return true;
    return false;
}
static bool isMoveConstructorDeleted(SYMBOL* sp)
{
    if (basetype(sp->tp)->type == bt_union)
    {
        for (auto sp1 : *basetype(sp->tp)->syms)
        {
            if (isstructured(sp1->tp))
            {
                SYMBOL* consovl = search(basetype(sp1->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
                for (auto cons : *basetype(consovl->tp)->syms)
                {
                    if (matchesCopy(cons, true))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
    }
    for (auto sp1 : *basetype(sp->tp)->syms)
    {
        TYPE* m;
        if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
        {
            if (isstructured(sp1->tp))
            {
                TYPE* tp = basetype(sp1->tp);
                if (checkDest(sp, tp->sp, basetype(tp->sp->tp)->syms, ac_public))
                    return true;
            }
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                if (checkMoveCons(sp, basetype(m)->sp, ac_public))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            if (checkDest(sp, base->cls, basetype(base->cls->tp)->syms, ac_protected))
                return true;
            if (checkMoveCons(sp, base->cls, ac_protected))
                return true;
        }
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                if (checkDest(sp, vbase->cls, basetype(vbase->cls->tp)->syms, ac_protected))
                    return true;
                if (checkMoveCons(sp, vbase->cls, ac_protected))
                    return true;
            }
        }
    return false;
}
static bool isMoveAssignmentDeleted(SYMBOL* sp)
{
    if (basetype(sp->tp)->type == bt_union)
    {
        for (auto sp1 : *basetype(sp->tp)->syms)
        {
            if (isstructured(sp1->tp))
            {
                SYMBOL* consovl = search(basetype(sp1->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
                for (auto cons : *basetype(consovl->tp)->syms)
                {
                    if (matchesCopy(cons, true))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
    }
    for (auto sp1 : *basetype(sp->tp)->syms)
    {
        TYPE* m;
        if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
        {
            if (isref(sp1->tp))
                return true;
            if (!isstructured(sp1->tp) && isconst(sp1->tp) && sp1->tp->type != bt_aggregate)
                return true;
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                if (checkMoveAssign(sp, basetype(m)->sp, ac_public))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (checkMoveAssign(sp, base->cls, ac_protected))
                return true;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc && checkMoveAssign(sp, vbase->cls, ac_protected))
                return true;
    return false;
}
static bool isDestructorDeleted(SYMBOL* sp)
{
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        return false;
    }
    for (auto sp1 : *basetype(sp->tp)->syms)
    {
        TYPE* m;
        if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
        {
            if (isstructured(sp1->tp))
            {
                TYPE* tp = basetype(sp1->tp);
                if (checkDest(sp, tp->sp, basetype(tp->sp->tp)->syms, ac_public))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (checkDest(sp, base->cls, basetype(base->cls->tp)->syms, ac_protected))
                return true;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc)
                if (checkDest(sp, vbase->cls, basetype(vbase->cls->tp)->syms, ac_protected))
                    return true;
    return false;
}
static bool isDestructorNoexcept(SYMBOL* sp)
{
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        return false;
    }
    for (auto sp1 : *basetype(sp->tp)->syms)
    {
        TYPE* m;
        if (sp1->sb->storage_class == sc_member || sp1->sb->storage_class == sc_mutable)
        {
            if (isstructured(sp1->tp))
            {
                if (!noexceptDest(basetype(sp1->tp)->sp))
                    return false;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (!noexceptDest(base->cls))
                return false;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc)
                if (!noexceptDest(vbase->cls))
                    return false;
    return true;
}
static void conditionallyDeleteDefaultConstructor(SYMBOL* func)
{
    for (auto sp : *basetype(func->tp)->syms)
    {
        if (sp->sb->defaulted && matchesDefaultConstructor(sp))
        {
            if (isDefaultDeleted(sp->sb->parentClass))
            {
                sp->sb->deleted = true;
            }
        }
    }
}
static bool conditionallyDeleteCopyConstructor(SYMBOL* func, bool move)
{
    for (auto sp : *basetype(func->tp)->syms)
    {
        if (sp->sb->defaulted && matchesCopy(sp, move))
        {
            if (move && isMoveConstructorDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
            else if (!move && isCopyConstructorDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
        }
    }
    return false;
}
static bool conditionallyDeleteCopyAssignment(SYMBOL* func, bool move)
{
    for (auto sp : *basetype(func->tp)->syms)
    {
        if (sp->sb->defaulted && matchesCopy(sp, move))
        {
            if (move && isMoveAssignmentDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
            else if (!move && isCopyAssignmentDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
        }
    }
    return false;
}
static bool conditionallyDeleteDestructor(SYMBOL* sp)
{
    if (isDestructorDeleted(sp->sb->parentClass))
    {
        sp->sb->deleted = true;
    }
    return false;
}
static void SetDestructorNoexcept(SYMBOL* sp)
{
    if (!templateNestingCount || instantiatingTemplate)
    {
        if (sp->sb->deferredNoexcept && sp->sb->deferredNoexcept != (LEXLIST*)-1)
        {
            parseNoexcept(sp);
        }
        else if (sp->sb->deferredNoexcept == 0)
        {
            sp->sb->noExcept = isDestructorNoexcept(sp->sb->parentClass);
        }
    }
}
void createConstructorsForLambda(SYMBOL* sp)
{
    SYMBOL* newcons;
    declareDestructor(sp);
    newcons = declareConstructor(sp, true, false);  // default
    newcons->sb->deleted = true;
    newcons = declareConstructor(sp, false, false);  // copy
    conditionallyDeleteCopyConstructor(newcons, false);
    newcons = declareAssignmentOp(sp, false);
    newcons->sb->deleted = true;
    if (!isMoveConstructorDeleted(sp))
    {
        newcons = declareConstructor(sp, false, true);
        //        newcons->sb->deleted = isMoveAssignmentDeleted(sp);
    }
}
// if there is a constructor with defaulted args that would match the
// default constructor,   make a shim default constructor that wraps it...
//
static void shimDefaultConstructor(SYMBOL* sp, SYMBOL* cons)
{
    SYMBOL* match = nullptr;
    for (auto sym : *basetype(cons->tp)->syms)
    {
        if (matchesDefaultConstructor(sym))
        {
            if (match)
                return;  // will get an error elsewhere because of the duplicate
            match = sym;
        }
    }
    if (match)
    {
        auto it = basetype(match->tp)->syms->begin();
        auto it1 = it;
        auto itend = basetype(match->tp)->syms->end();
        ++it1;
        if (it1 != itend && ((*it1)->sb->init || (*it1)->sb->deferredCompile))
        {
            if (sp->templateParams == nullptr)
            {
                // will match a default constructor but has defaulted args
                SYMBOL* consfunc = declareConstructor(sp, true, false);  // default
                SymbolTable<SYMBOL>* syms;
                BLOCKDATA bd = {};
                std::list<BLOCKDATA*> b = { &bd };
                STATEMENT* st;
                EXPRESSION* thisptr = varNode(en_auto, *it);
                EXPRESSION* e1;
                FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                (*it)->sb->offset = Optimizer::chosenAssembler->arch->retblocksize;
                deref(&stdpointer, &thisptr);
                bd.type = begin;
                syms = localNameSpace->front()->syms;
                localNameSpace->front()->syms = basetype(consfunc->tp)->syms;
                params->thisptr = thisptr;
                params->thistp = MakeType(bt_pointer, sp->tp);
                params->fcall = varNode(en_pc, match);
                params->functp = match->tp;
                params->sp = match;
                params->ascall = true;
                params->arguments = initListListFactory.CreateList();
                AdjustParams(match, basetype(match->tp)->syms->begin(), basetype(match->tp)->syms->end(), &params->arguments, false, true);
                if (sp->sb->vbaseEntries)
                {
                    INITLIST *x = Allocate<INITLIST>(), **p;
                    x->tp = MakeType(bt_int);
                    x->exp = intNode(en_c_i, 1);
                    params->arguments->push_back(x);
                }
                e1 = varNode(en_func, nullptr);
                e1->v.func = params;
                if (e1)  // could probably remove this, only null if ran out of memory.
                {
                    e1 = exprNode(en_thisref, e1, nullptr);
                    e1->v.t.thisptr = params->thisptr;
                    e1->v.t.tp = sp->tp;
                    // hasXCInfo = true;
                }
                st = stmtNode(nullptr, b, st_return);
                st->select = e1;
                consfunc->sb->xcMode = cons->sb->xcMode;
                if (consfunc->sb->xc)
                    consfunc->sb->xc->xcDynamic = cons->sb->xc->xcDynamic;
                consfunc->sb->inlineFunc.stmt = stmtListFactory.CreateList();
                auto stmt = stmtNode(nullptr, emptyBlockdata, st_block);
                consfunc->sb->inlineFunc.stmt->push_back(stmt);
                stmt->lower = bd.statements;
                consfunc->sb->inlineFunc.syms = basetype(consfunc->tp)->syms;
                consfunc->sb->retcount = 1;
                consfunc->sb->attribs.inheritable.isInline = true;
                // now get rid of the first default arg
                // leave others so the old constructor can be considered
                // under other circumstances
                ++it;
                (*it)->sb->init = nullptr;
                localNameSpace->front()->syms = syms;
            }
        }
    }
}
void createDefaultConstructors(SYMBOL* sp)
{
    SYMBOL* cons = search(basetype(sp->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
    SYMBOL* dest = search(basetype(sp->tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
    SYMBOL* asgn = search(basetype(sp->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
    SYMBOL* newcons = nullptr;
    if (!dest)
    {
        declareDestructor(sp);
        auto dest1 = search(basetype(sp->tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
        SetDestructorNoexcept(dest1->tp->syms->front());
    }
    else
    {
        sp->sb->hasDest = true;
        SetDestructorNoexcept(dest->tp->syms->front());
    }
    if (cons)
    {
        bool defaulted = true;
        for (auto sp1 : *cons->tp->syms)
            if (!sp1->sb->defaulted || sp1->sb->isExplicit)
            {
                defaulted = false;
                break;
            }
        sp->sb->hasUserCons = !defaulted;
        shimDefaultConstructor(sp, cons);
    }
    else
    {
        // create the default constructor
        newcons = declareConstructor(sp, true, false);
        cons = search(basetype(sp->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
    }
    // see if the default constructor could be trivial
    if (!hasVTab(sp) && sp->sb->vbaseEntries == nullptr && !dest)
    {
        bool found = false;
        if (sp->sb->baseClasses)
            for (auto base : *sp->sb->baseClasses)
                if (!base->cls->sb->trivialCons || base->accessLevel != ac_public)
                {
                    found = true;
                    break;
                }
        if (!found)
        {
            bool trivialCons = true;
            bool trivialDest = true;
            for (auto pcls : *basetype(sp->tp)->syms)
            {
                TYPE* tp = pcls->tp;
                while (isarray(tp))
                    tp = basetype(tp)->btp;
                if (pcls->sb->storage_class == sc_member || pcls->sb->storage_class == sc_mutable ||
                    pcls->sb->storage_class == sc_overloads)
                {
                    if (pcls->sb->memberInitializers)
                        trivialCons = false;
                    if (isstructured(tp))
                    {
                        if (!basetype(tp)->sp->sb->trivialCons)
                            trivialCons = false;
                    }
                    else if (pcls->sb->storage_class == sc_overloads)
                    {
                        bool err = false;
                        for (auto s : *basetype(tp)->syms)
                        {
                            if (err)
                                break;
                            if (s->sb->storage_class != sc_static)
                            {
                                err |= s->sb->isConstructor && !s->sb->defaulted;
                                err |= s->sb->deleted;
                                err |= s->sb->access != ac_public;
                                err |= s->sb->isConstructor && s->sb->isExplicit;
                                if (s->sb->isDestructor && !s->sb->defaulted)
                                    trivialDest = false;
                            }
                        }
                        if (err)
                            trivialCons = false;
                    }
                    else if (pcls->sb->access != ac_public)
                        trivialCons = false;
                }
            }
            sp->sb->trivialCons = trivialCons;
            sp->sb->trivialDest = trivialDest;
        }
        for (auto s : *cons->tp->syms)
        {
            if (s->sb->constexpression | s->sb->defaulted)
                sp->sb->literalClass = true;
        }
    }
    if (newcons)
        newcons->sb->trivialCons = true;
    // now if there is no copy constructor or assignment operator declare them
    if (!hasCopy(cons, false))
    {
        SYMBOL* newcons = declareConstructor(sp, false, false);
        newcons->sb->trivialCons = hasTrivialCopy(sp, false);
        if (hasCopy(cons, true) || (asgn && hasCopy(asgn, true)))
            newcons->sb->deleted = true;
        if (!asgn)
            asgn = search(basetype(sp->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
        sp->sb->deleteCopyCons = true;
    }
    if (!asgn || !hasCopy(asgn, false))
    {
        SYMBOL* newsp = declareAssignmentOp(sp, false);
        newsp->sb->trivialCons = hasTrivialAssign(sp, false);
        if (hasCopy(cons, true) || (asgn && hasCopy(asgn, true)))
            newsp->sb->deleted = true;
        if (!asgn)
            asgn = search(basetype(sp->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
        sp->sb->deleteCopyAssign = true;
    }
    // now if there is no move constructor, no copy constructor,
    // no copy assignment, no move assignment, no destructor
    // and wouldn't be defined as deleted
    // declare a move constructor and assignment operator
    if (!dest && !hasCopy(cons, false) && !hasCopy(cons, true) && !hasCopy(asgn, false) && (!asgn || !hasCopy(asgn, true)))
    {
        bool b = isMoveConstructorDeleted(sp);
        SYMBOL* newcons;
        if (!b)
        {
            newcons = declareConstructor(sp, false, true);
            newcons->sb->trivialCons = hasTrivialCopy(sp, true);
        }
        newcons = declareAssignmentOp(sp, true);
        newcons->sb->trivialCons = hasTrivialAssign(sp, true);
        newcons->sb->deleted |= isMoveAssignmentDeleted(sp);
    }
    else
    {
        sp->sb->deleteMove = true;
    }
    // now tag the copy/move assignment operators
    for (auto a : *asgn->tp->syms)
    {
        a->sb->isAssign = true;
    }
}
void ConditionallyDeleteClassMethods(SYMBOL* sp) 
{ 
    auto cons = search(basetype(sp->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
    auto asgn = search(basetype(sp->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
    conditionallyDeleteDefaultConstructor(cons);
    if (sp->sb->deleteCopyCons)
    {
        conditionallyDeleteCopyConstructor(cons, false);
    }
    if (sp->sb->deleteCopyAssign)
    {
        conditionallyDeleteCopyAssignment(asgn, false);
    }
    if (sp->sb->deleteMove)
    {
        conditionallyDeleteCopyConstructor(cons, true);
        conditionallyDeleteCopyAssignment(asgn, true);
    }
    if (sp->sb->defaulted)
    {
        auto dest = search(basetype(sp->tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
        conditionallyDeleteDestructor(dest->tp->syms->front());
    }
}
EXPRESSION* destructLocal(EXPRESSION* exp)
    {
    std::stack<SYMBOL*> destructList;
    std::stack<EXPRESSION*> stk;
    stk.push(exp);
    while (stk.size())
    {
        EXPRESSION* e = stk.top();
        stk.pop();
        if (!isintconst(e) && !isfloatconst(e))
        {
            if (e->left)
                stk.push(e->left);
            if (e->right)
                stk.push(e->right);
        }
        if (e->type == en_thisref)
            e = e->left;
        if (e->type == en_func)
        {
            if (e->v.func->arguments)
                for (auto il : *e->v.func->arguments)
                    stk.push(il->exp);
        }
        if (e->type == en_auto && e->v.sp->sb->allocate && !e->v.sp->sb->destructed)
        {
            TYPE* tp = e->v.sp->tp;
            while (isarray(tp))
                tp = basetype(tp)->btp;
            if (isstructured(tp) && !isref(tp))
            {
                e->v.sp->sb->destructed = true;
                destructList.push(e->v.sp);
            }
        }
    }

    EXPRESSION* rv = exp;
    while (destructList.size())
    {
        SYMBOL* sp = destructList.top();
        destructList.pop();
        if (sp->sb->dest && sp->sb->dest->front()->exp)
            rv = exprNode(en_void, rv, sp->sb->dest->front()->exp);
    }
    return rv;
}
void DestructParams(std::list<INITLIST*>* il)
{
    if (Optimizer::cparams.prm_cplusplus)
        for (auto first : *il)
        {
            TYPE* tp = first->tp;
            if (tp)
            {
                bool ref = false;
                if (isref(tp))
                {
                    ref = true;
                    tp = basetype(tp)->btp;
                }
                else if (tp->lref || tp->rref)
                {
                    ref = true;
                }
                if (ref || !isstructured(tp))
                {
                    std::stack<EXPRESSION*> stk;
                    
                    stk.push(first->exp);
                    while (!stk.empty())
                    {
                        auto tst = stk.top();
                        stk.pop();
                        if (tst->type == en_thisref)
                            tst = tst->left;
                        if (tst->type == en_func)
                        {
                            if (tst->v.func->sp->sb->isConstructor)
                            {
                                EXPRESSION* iexp = tst->v.func->thisptr;
                                auto sp = basetype(basetype(tst->v.func->thistp)->btp)->sp;
                                int offs;
                                auto xexp = relptr(iexp, offs);
                                if (xexp)
                                    xexp->v.sp->sb->destructed = true;
                                if (callDestructor(sp, nullptr, &iexp, nullptr, true, false, false, true))
                                {
                                    if (!first->destructors)
                                        first->destructors = exprListFactory.CreateList();
                                    first->destructors->push_front(iexp);
                                }
                            }
                        }
                        else if (tst->type == en_void)
                        {
                            if (tst->right)
                                stk.push(tst->right);
                            if (tst->left)
                                stk.push(tst->left);
                        }
                    }
                }
            }
        }
}
void destructBlock(EXPRESSION** exp, SymbolTable<SYMBOL> *table, bool mainDestruct)
{
    for (auto sp : *table)
    {
        if ((sp->sb->allocate || sp->sb->storage_class == sc_parameter) && !sp->sb->destructed && !isref(sp->tp))
        {
            sp->sb->destructed = mainDestruct;
            if (sp->sb->storage_class == sc_parameter)
            {
                if (isstructured(sp->tp))
                {
                    EXPRESSION* iexp = getThisNode(sp);
                    if (callDestructor(basetype(sp->tp)->sp, nullptr, &iexp, nullptr, true, false, false, true))
                    {
                        optimize_for_constants(&iexp);
                        if (*exp)
                        {
                            *exp = exprNode(en_void, iexp, *exp);
                        }
                        else
                        {
                            *exp = iexp;
                        }
                    }
                }
            }
            else if (sp->sb->storage_class != sc_localstatic && sp->sb->dest)
            {

                EXPRESSION* iexp = sp->sb->dest->front()->exp;
                if (iexp)
                {
                    optimize_for_constants(&iexp);
                    if (*exp)
                    {
                        *exp = exprNode(en_void, iexp, *exp);
                    }
                    else
                    {
                        *exp = iexp;
                    }
                }
            }
        }
    }
}
static void genConsData(std::list<BLOCKDATA*>& b, SYMBOL* cls, std::list<MEMBERINITIALIZERS*>* mi, SYMBOL* member, int offset, EXPRESSION* thisptr,
                        EXPRESSION* otherptr, SYMBOL* parentCons, bool doCopy)
{
    (void)cls;
    (void)mi;
    if (doCopy && (matchesCopy(parentCons, false) || matchesCopy(parentCons, true)))
    {
        thisptr = exprNode(en_structadd, thisptr, intNode(en_c_i, offset));
        otherptr = exprNode(en_structadd, otherptr, intNode(en_c_i, offset));
        thisptr->right->keepZero = true;
        otherptr->right->keepZero = true;
        if (isstructured(member->tp) || isarray(member->tp) || basetype(member->tp)->type == bt_memberptr)
        {
            EXPRESSION* exp = exprNode(en_blockassign, thisptr, otherptr);
            STATEMENT* st = stmtNode(nullptr, b, st_expr);
            exp->size = member->tp;
            exp->altdata = (void*)member->tp;
            optimize_for_constants(&exp);
            st->select = exp;
        }
        else
        {
            STATEMENT* st = stmtNode(nullptr, b, st_expr);
            EXPRESSION* exp;
            deref(member->tp, &thisptr);
            deref(member->tp, &otherptr);
            exp = exprNode(en_assign, thisptr, otherptr);
            optimize_for_constants(&exp);
            st->select = exp;
        }
    }
    else if (member->sb->init)
    {
        EXPRESSION* exp;
        STATEMENT* st = stmtNode(nullptr, b, st_expr);
        exp = convertInitToExpression(member->tp, member, nullptr, nullptr, member->sb->init, thisptr, false);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void genConstructorCall(std::list<BLOCKDATA*>& b, SYMBOL* cls, std::list<MEMBERINITIALIZERS*>* mi, SYMBOL* member, int memberOffs, bool top,
                               EXPRESSION* thisptr, EXPRESSION* otherptr, SYMBOL* parentCons, bool baseClass, bool doCopy,
                               bool useDefault)
{
    STATEMENT* st = nullptr;
    if (cls != member && member->sb->init)
    {
        EXPRESSION* exp;
        if (member->sb->init->front()->exp)
        {
            exp = convertInitToExpression(member->tp, member, nullptr, nullptr, member->sb->init, thisptr, false);
            if (mi->front() && mi->front()->valueInit)
            {
                auto ths = exprNode(en_add, thisptr, intNode(en_c_i, member->sb->offset));
                auto clr = exprNode(en_blockclear, ths, 0);
                clr->size = member->tp;
                exp = exprNode(en_void, clr, exp);
            }
        }
        else
        {
            exp = exprNode(en_add, thisptr, intNode(en_c_i, member->sb->offset));
            exp = exprNode(en_blockclear, exp, 0);
            exp->size = member->tp;
        }
        st = stmtNode(nullptr, b, st_expr);
        optimize_for_constants(&exp);
        st->select = exp;
    }
    else
    {
        TYPE* ctype = member->tp;
        EXPRESSION* exp = exprNode(en_add, thisptr, intNode(en_c_i, memberOffs));
        if (doCopy && matchesCopy(parentCons, false))
        {
            if (useDefault)
            {
                if (!callConstructor(&ctype, &exp, nullptr, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
            else
            {
                EXPRESSION* other = exprNode(en_add, otherptr, intNode(en_c_i, memberOffs));

                TYPE* tp = MakeType(bt_lref, member->tp);
                tp->size = getSize(bt_pointer);

                auto it = basetype(parentCons->tp)->syms->begin();
                ++it;
                if (isconst((*it)->tp->btp))
                {
                    tp->btp = MakeType(bt_const, tp->btp);
                }
                UpdateRootTypes(tp);
                if (!callConstructorParam(&ctype, &exp, tp, other, top, false, false, false, true))
                    errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
            }
        }
        else if (doCopy && matchesCopy(parentCons, true))
        {
            if (useDefault)
            {
                if (!callConstructor(&ctype, &exp, nullptr, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
            else
            {
                EXPRESSION* other = exprNode(en_add, otherptr, intNode(en_c_i, memberOffs));
                other = exprNode(en_not_lvalue, other, nullptr);
                TYPE* tp = MakeType(bt_rref, member->tp);

                auto it = basetype(parentCons->tp)->syms->begin();
                ++it;
                if (isconst((*it)->tp->btp))
                {
                    tp->btp = MakeType(bt_const, tp->btp);
                }
                UpdateRootTypes(tp);
                if (!callConstructorParam(&ctype, &exp, tp, other, top, false, false, false, true))
                    errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
            }
        }
        else
        {
            MEMBERINITIALIZERS* mix = nullptr;
            if (mi && mi->size() && mi->front() && mi->front()->sp && baseClass)
            {
                for (auto mi2 : *mi)
                {
                    if (mi2->sp && isstructured(mi2->sp->tp) &&
                        (basetype(mi2->sp->tp)->sp == member || basetype(mi2->sp->tp)->sp == member->sb->maintemplate || sameTemplate(mi2->sp->tp, member->tp)))
                    {
                        mix = mi2;
                        break;
                    }
                }
            }
            if (mix)
            {
                FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                if (!funcparams->arguments)
                    funcparams->arguments = initListListFactory.CreateList();
                if (mix->init)
                {
                    for (auto init : *mix->init)
                    {
                        if (!init->exp)
                            break;
                        auto arg = Allocate<INITLIST>();
                        arg->tp = init->basetp;
                        arg->exp = init->exp;
                        funcparams->arguments->push_back(arg);
                    }
                }
                if (!callConstructor(&ctype, &exp, funcparams, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
                if (mix->sp && !mix->init)
                {
                    EXPRESSION* clr = exprNode(en_blockclear, exp, nullptr);
                    clr->size = mix->sp->tp;
                    exp = exprNode(en_void, clr, exp);
                }
                // previously, callConstructor can return false here, meaning that funcparams->sp is null
                // This used to create a nullptr dereference in PromoteConstructorArgs
                // Why this is only being found NOW is somewhat dumb, but it is.
                PromoteConstructorArgs(funcparams->sp, funcparams);
            }
            else
            {
                if (!callConstructor(&ctype, &exp, nullptr, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
        }
        st = stmtNode(nullptr, b, st_expr);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void virtualBaseThunks(std::list<BLOCKDATA*>& b, SYMBOL* sp, EXPRESSION* thisptr)
{
    EXPRESSION *first = nullptr, **pos = &first;
    STATEMENT* st;
    if (sp->sb->vbaseEntries)
    {
        for (auto entry : *sp->sb->vbaseEntries)
        {
            EXPRESSION* left = exprNode(en_add, thisptr, intNode(en_c_i, entry->pointerOffset));
            EXPRESSION* right = exprNode(en_add, thisptr, intNode(en_c_i, entry->structOffset));
            EXPRESSION* asn;
            deref(&stdpointer, &left);
            asn = exprNode(en_assign, left, right);
            if (!*pos)
            {
                *pos = asn;
            }
            else
            {
                *pos = exprNode(en_void, *pos, asn);
                pos = &(*pos)->right;
            }
        }
    }
    if (first)
    {
        st = stmtNode(nullptr, b, st_expr);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void HandleEntries(EXPRESSION** pos, std::list<VTABENTRY*>* entries, EXPRESSION* thisptr, EXPRESSION* vtabBase, bool isvirtual)
{
    std::list<VTABENTRY*>* children = entries->front()->children;
    for (auto entry : *entries)
    {
        if (!entry->isdead && entry->isvirtual == isvirtual && hasVTab(entry->cls))
        {
            EXPRESSION* left = exprNode(en_add, thisptr, intNode(en_c_i, entry->dataOffset));
            EXPRESSION* right =
                exprNode(en_add, exprNode(en_add, vtabBase, intNode(en_c_i, entry->vtabOffset)), intNode(en_c_i, VTAB_XT_OFFS));
            EXPRESSION* asn;
            deref(&stdpointer, &left);
            asn = exprNode(en_assign, left, right);
            if (!*pos)
            {
                *pos = asn;
            }
            else
            {
                *pos = exprNode(en_void, *pos, asn);
                pos = &(*pos)->right;
            }
        }
    }
    if (children)
    {
        HandleEntries(pos, children, thisptr, vtabBase, isvirtual);
    }
}
static void dovtabThunks(std::list<BLOCKDATA*>& b, SYMBOL* sym, EXPRESSION* thisptr, bool isvirtual)
{
    auto entries = sym->sb->vtabEntries;
    EXPRESSION* first = nullptr;
    STATEMENT* st;
    SYMBOL* localsp;
    localsp = sym->sb->vtabsp;
    EXPRESSION* vtabBase = varNode(en_global, localsp);
    if (localsp->sb->attribs.inheritable.linkage2 == lk_import)
        deref(&stdpointer, &vtabBase);
    HandleEntries(&first, entries, thisptr, vtabBase, isvirtual);
    if (first)
    {
        st = stmtNode(nullptr, b, st_expr);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void doVirtualBases(std::list<BLOCKDATA*>& b, SYMBOL* sp, std::list<MEMBERINITIALIZERS*>* mi, std::list<VBASEENTRY*>* vbe, EXPRESSION* thisptr,
                           EXPRESSION* otherptr, SYMBOL* parentCons, bool doCopy)
{
    if (vbe && vbe->size())
    {
        for (auto it = vbe->end(); it != vbe->begin() ;)
        {
            --it;
            if ((*it)->alloc)
                genConstructorCall(b, sp, mi, (*it)->cls, (*it)->structOffset, false, thisptr, otherptr, parentCons, true, doCopy, false);
        }
    }
}
static EXPRESSION* unshim(EXPRESSION* exp, EXPRESSION* ths);
static std::list<STATEMENT*>* unshimstmt(std::list<STATEMENT*>* block_in, EXPRESSION* ths)
{
    if (block_in)
    {
        std::list<STATEMENT*>* rv = stmtListFactory.CreateList();
        for (auto block : *block_in)
        {
            auto v = Allocate<STATEMENT>();
            *v = *block;
            block = v;
            rv->push_back(block);
            switch (block->type)
            {
            case st__genword:
                break;
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                block->lower = unshimstmt(block->lower, ths);
                block->blockTail = unshimstmt(block->blockTail, ths);
                break;
            case st_return:
            case st_expr:
            case st_declare:
                block->select = unshim(block->select, ths);
                break;
            case st_goto:
            case st_label:
                break;
            case st_select:
            case st_notselect:
                block->select = unshim(block->select, ths);
                break;
            case st_switch:
                block->select = unshim(block->select, ths);
                block->lower = unshimstmt(block->lower, ths);
                break;
            case st_block:
                block->lower = unshimstmt(block->lower, ths);
                block->blockTail = unshimstmt(block->blockTail, ths);
                break;
            case st_passthrough:
                break;
            case st_datapassthrough:
                break;
            case st_nop:
                break;
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            default:
                diag("Invalid block type in unshimstmt");
                break;
            }
        }
        return rv;
    }
    return nullptr;
}

static EXPRESSION* unshim(EXPRESSION* exp, EXPRESSION* ths)
{
    EXPRESSION* nw = nullptr;
    if (!exp)
        return exp;
    if (exp->type == en_thisshim)
    {
        return ths;
    }
    nw = Allocate<EXPRESSION>();
    *nw = *exp;
    nw->left = unshim(nw->left, ths);
    nw->right = unshim(nw->right, ths);
    if (nw->type == en_stmt)
    {
        nw->v.stmt = unshimstmt(nw->v.stmt, ths);
    }
    return nw;
}
SYMBOL* findClassName(const char* name, SYMBOL* cls, std::list<BASECLASS*>* bc, std::list<VBASEENTRY*>* vbases, int* offset)
{
    int n = 0;
    char str[1024];
    char* clslst[100];
    char* p = str, * c;
    SYMBOL* sp = nullptr;
    int vcount = 0, ccount = 0;
    strcpy(str, name);
    while ((c = (char*)strstr(p, "::")))
    {
        clslst[n++] = p;
        p = c;
        *p = 0;
        p += 2;
    }
    clslst[n++] = p;

    if (bc)
    {
        for (auto base : *bc)
        {
            if (!base->isvirtual)
            {
                SYMBOL* parent = base->cls;
                int i;
                if (parent->tp->type == bt_typedef)
                    parent = basetype(parent->tp)->sp;
                for (i = n - 1; i >= 0 && parent;
                    i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                    if (strcmp(parent->name, clslst[i]))
                        break;
                if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
                {
                    ccount++;
                    sp = base->cls;
                    *offset = base->offset;
                }
            }
        }
    }
    if (vbases)
        for (auto vbase : *vbases)
        {
            SYMBOL* parent = vbase->cls;
            int i;
            if (parent->tp->type == bt_typedef)
                parent = basetype(parent->tp)->sp;
            for (i = n - 1; i >= 0 && parent;
                i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                if (strcmp(parent->name, clslst[i]))
                    break;
            if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
            {
                vcount++;
                sp = vbase->cls;
                *offset = vbase->structOffset;
            }
        }
    if ((ccount && vcount) || ccount > 1)
    {
        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, sp, cls);
    }
    return sp;
}
void ParseMemberInitializers(SYMBOL* cls, SYMBOL* cons)
{
    bool first = true;
    bool hasDelegate = false;
    if (cons->sb->memberInitializers)
    {
        int pushcount = pushContext(cls, true);
        auto ite = cons->sb->memberInitializers->end();
        STRUCTSYM tpl;
        if (cons->templateParams)
        {
            tpl.tmpl = cons->templateParams;
            addTemplateDeclaration(&tpl);
            pushcount++;
        }
        for (auto it = cons->sb->memberInitializers->begin(); it != ite;)
        {
            auto init = *it;
            LEXLIST* lex;
            if (!first && hasDelegate)
                error(ERR_DELEGATING_CONSTRUCTOR_ONLY_INITIALIZER);
            init->sp = search(basetype(cls->tp)->syms, init->name);
            if (init->sp && (!init->basesym || !istype(init->sp)))
            {
                if (init->sp->sb->storage_class == sc_typedef)
                {
                    TYPE* tp = basetype(init->sp->tp);
                    if (isstructured(tp))
                    {
                        init->name = basetype(tp)->sp->name;
                        init->sp = search(basetype(cls->tp)->syms, init->name);
                    }
                }
            }
            if (init->sp && (!init->basesym || !istype(init->sp)))
            {
                if (init->sp != cls && init->sp->sb->storage_class != sc_member && init->sp->sb->storage_class != sc_mutable)
                {
                    errorsym(ERR_NEED_NONSTATIC_MEMBER, init->sp);
                }
                else
                {
                    bool done = false;
                    lex = SetAlternateLex(init->initData);
                    if (MATCHKW(lex, lt))
                    {
                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                        lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                        if (init->sp->sb->templateLevel)
                        {
                            init->sp = TemplateClassInstantiate(init->sp, lst, false, sc_global);
                        }
                        else
                        {
                            errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                        }
                    }
                    if (!isstructured(init->sp->tp))
                    {
                        bool bypa = true;
                        if (MATCHKW(lex, openpa) || MATCHKW(lex, begin))
                        {
                            bypa = MATCHKW(lex, openpa);
                            lex = getsym();
                            if ((bypa && MATCHKW(lex, closepa)) || (!bypa && MATCHKW(lex, end)))
                            {
                                lex = getsym();
                                init->init = nullptr;
                                initInsert(&init->init, init->sp->tp, intNode(en_c_i, 0), 0, false);
                                done = true;
                            }
                            else
                            {
                                lex = backupsym();
                            }
                        }
                        if (!done)
                        {
                            needkw(&lex, bypa ? openpa : begin);
                            init->init = nullptr;
                            argumentNesting++;
                            lex = initType(lex, cons, 0, sc_auto, &init->init, nullptr, init->sp->tp, init->sp, false, 0);
                            argumentNesting--;
                            done = true;
                            needkw(&lex, bypa ? closepa : end);
                        }
                    }
                    else
                    {
                        bool empty = false;
                        if (MATCHKW(lex, openpa))
                        {
                            lex = getsym();
                            if (MATCHKW(lex, closepa))
                                empty = true;
                            lex = backupsym();
                        }
                        if (MATCHKW(lex, openpa) && basetype(init->sp->tp)->sp->sb->trivialCons)
                        {
                            init->init = nullptr;
                            argumentNesting++;
                            lex = initType(lex, cons, 0, sc_auto, &init->init, nullptr, init->sp->tp, init->sp, false, 0);
                            argumentNesting--;
                            done = true;
                            if (init->packed || MATCHKW(lex, ellipse))
                                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        }
                        else
                        {
                            init->init = nullptr;
                            lex = initType(lex, cons, 0, sc_auto, &init->init, nullptr, init->sp->tp, init->sp, false, 0);
                            if (init->packed)
                                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        }
                        init->valueInit = empty;
                    }
                    SetAlternateLex(nullptr);
                }
            }
            else
            {
                SYMBOL* sp = init->basesym;
                if (!sp)
                    sp = classsearch(init->name, false, false, true);
                else
                    init->sp = sp;
                if (sp && sp->tp->type == bt_templateparam)
                {
                    if (sp->tp->templateParam->second->type == kw_typename)
                    {
                        if (sp->tp->templateParam->second->packed)
                        {
                            FUNCTIONCALL shim;
                            lex = SetAlternateLex(init->initData);
                            shim.arguments = nullptr;
                            getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                            if (!init->packed)
                                error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                            SetAlternateLex(nullptr);
                            expandPackedMemberInitializers(cls, cons, sp->tp->templateParam->second->byPack.pack, &cons->sb->memberInitializers, init->initData,
                                shim.arguments);
                            init->sp = cls;
                        }
                        else if (sp->tp->templateParam->second->byClass.val && isstructured(sp->tp->templateParam->second->byClass.val))
                        {
                            TYPE* tp = sp->tp->templateParam->second->byClass.val;
                            int offset = 0;
                            int vcount = 0, ccount = 0;
                            init->name = basetype(tp)->sp->name;
                            if (cls->sb->baseClasses)
                            {
                                for (auto bc : *cls->sb->baseClasses)
                                {
                                    if (!strcmp(bc->cls->name, init->name))
                                    {
                                        if (bc->isvirtual)
                                            vcount++;
                                        else
                                            ccount++;
                                        init->sp = bc->cls;
                                        offset = bc->offset;
                                    }
                                }
                            }
                            if ((ccount && vcount) || ccount > 1)
                            {
                                errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, init->sp, cls);
                            }
                            if (init->sp && init->sp == basetype(tp)->sp)
                            {
                                SYMBOL* sp = makeID(sc_member, init->sp->tp, nullptr, init->sp->name);
                                FUNCTIONCALL shim;
                                sp->sb->offset = offset;
                                init->sp = sp;
                                lex = SetAlternateLex(init->initData);
                                shim.arguments = nullptr;
                                getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                                if (init->packed)
                                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                                SetAlternateLex(nullptr);
                                if (shim.arguments)
                                {
                                    init->init = initListFactory.CreateList();
                                    for (auto a : *shim.arguments)
                                    {
                                        auto xinit = Allocate<INITIALIZER>();
                                        xinit->basetp = a->tp;
                                        xinit->exp = a->exp;
                                        init->init->push_back(xinit);
                                    }
                                }
                            }
                            else
                            {
                                init->sp = nullptr;
                            }
                        }
                        else
                        {
                            error(ERR_STRUCTURED_TYPE_EXPECTED_IN_TEMPLATE_PARAMETER);
                        }
                    }
                    else
                    {
                        error(ERR_CLASS_TEMPLATE_PARAMETER_EXPECTED);
                    }
                }
                else if (init->packed)
                {
                    expandPackedBaseClasses(cls, cons, it, ite, cons->sb->memberInitializers, cls->sb->baseClasses, cls->sb->vbaseEntries);
                    continue;
                }
                else
                {
                    int offset = 0;
                    // check for a delegating constructor call
                    if (!strcmp(init->name, cls->name))
                    {
                        init->sp = cls;
                        init->delegating = true;
                        if (!first)
                            error(ERR_DELEGATING_CONSTRUCTOR_ONLY_INITIALIZER);
                        hasDelegate = true;
                        cons->sb->delegated = true;
                    }
                    if (!init->sp)
                        init->sp = findClassName(init->name, cls, cls->sb->baseClasses, cls->sb->vbaseEntries, &offset);
                    if (init->sp)
                    {
                        // have to make a *real* variable as a fudge...
                        SYMBOL* sp;
                        FUNCTIONCALL shim;
                        lex = SetAlternateLex(init->initData);
                        if (MATCHKW(lex, lt))
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                            if (init->sp->sb->templateLevel)
                            {
                                init->sp = TemplateClassInstantiate(init->sp, lst, false, sc_global);
                            }
                            else
                            {
                                errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                            }
                        }
                        sp = makeID(sc_member, init->sp->tp, nullptr, init->sp->name);
                        sp->sb->offset = offset;
                        init->sp = sp;
                        shim.arguments = nullptr;
                        getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                        SetAlternateLex(nullptr);
                        if (init->packed)
                        {
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        }
                        if (shim.arguments)
                        {
                            init->init = initListFactory.CreateList();
                            for (auto a : *shim.arguments)
                            {
                                auto xinit = Allocate<INITIALIZER>();
                                xinit->basetp = a->tp;
                                xinit->exp = a->exp;
                                init->init->push_back(xinit);
                            }
                        }
                    }
                }
            }
            if (!init->sp)
            {
                // might be a typedef?
                init->sp = finishSearch(init->name, nullptr, nullptr, false, false, false);
                if (init->sp && init->sp->sb->storage_class == sc_typedef)
                {
                    int offset = 0;
                    TYPE* tp = init->sp->tp;
                    tp = basetype(tp);
                    if (isstructured(tp))
                    {
                        bool found = false;
                        if (cls->sb->baseClasses)
                            for (auto bc : *cls->sb->baseClasses)
                            {
                                if (!comparetypes(bc->cls->tp, init->sp->tp, true) || sameTemplate(bc->cls->tp, init->sp->tp))
                                {
                                    found = true;
                                    break;
                                }
                            }
                        if (found)
                        {
                            // have to make a *real* variable as a fudge...
                            SYMBOL* sp;
                            FUNCTIONCALL shim;
                            lex = SetAlternateLex(init->initData);
                            if (MATCHKW(lex, lt))
                            {
                                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                                if (init->sp->sb->templateLevel)
                                {
                                    init->sp = TemplateClassInstantiate(init->sp, lst, false, sc_global);
                                }
                                else
                                {
                                    errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                                }
                            }
                            sp = makeID(sc_member, init->sp->tp, nullptr, init->sp->name);
                            sp->sb->offset = offset;
                            init->sp = sp;
                            shim.arguments = nullptr;
                            getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                            if (init->packed)
                                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                            SetAlternateLex(nullptr);
                            if (shim.arguments)
                            {
                                init->init = initListFactory.CreateList();
                                for (auto a : *shim.arguments)
                                {
                                    auto xinit = Allocate<INITIALIZER>();
                                    xinit->basetp = a->tp;
                                    xinit->exp = a->exp;
                                    init->init->push_back(xinit);
                                }
                            }
                        }
                    }
                    else
                    {
                        init->sp = nullptr;
                    }
                }
                else
                {
                    init->sp = nullptr;
                }
            }
            if (!init->sp)
            {
                errorstrsym(ERR_NOT_A_MEMBER_OR_BASE_CLASS, init->name, cls);
            }
            ++it;
            first = false;
        }
        for (int i = 0; i < pushcount; i++)
            dropStructureDeclaration();
    }
}
static void allocInitializers(SYMBOL* cls, SYMBOL* cons, EXPRESSION* ths)
{
    for (auto sp : *basetype(cls->tp)->syms)
    {
        if (sp->sb->storage_class == sc_member || sp->sb->storage_class == sc_mutable)
        {
            sp->sb->lastInit = sp->sb->init;
            if (sp->sb->init)
            {
                sp->sb->init = initListFactory.CreateList();
                *sp->sb->init = *sp->sb->lastInit;
                sp->sb->init->front()->exp = unshim(sp->sb->init->front()->exp, ths);
            }
        }
    }
    if (cons->sb->memberInitializers)
    {
        for (auto init : *cons->sb->memberInitializers)
        {
            if (init->init)
            {
                init->sp->sb->init = init->init;
                if (init->init->size() && init->init->front()->exp)
                    init->init->front()->exp = unshim(init->init->front()->exp, ths);
            }
        }
    }
    if (!cons->sb->delegated)
    {
        for (auto sp : *basetype(cls->tp)->syms)
        {
            if (!sp->sb->init && ismember(sp))
            {
                if (isref(sp->tp))
                    errorsym(ERR_REF_MEMBER_MUST_INITIALIZE, sp);
                else if (isconst(sp->tp))
                    errorsym(ERR_CONSTANT_MEMBER_MUST_BE_INITIALIZED, sp);
            }
        }
    }
}
static void releaseInitializers(SYMBOL* cls, SYMBOL* cons)
{
    (void)cons;
    for (auto sp : *basetype(cls->tp)->syms)
    {
        if (sp->sb->storage_class == sc_member || sp->sb->storage_class == sc_mutable)
            sp->sb->init = sp->sb->lastInit;
    }
}
EXPRESSION* thunkConstructorHead(std::list<BLOCKDATA*>& b, SYMBOL* sym, SYMBOL* cons, SymbolTable<SYMBOL>* syms, bool parseInitializers, bool doCopy,
                                 bool defaulted)
{
    BASECLASS* bc;
    EXPRESSION* thisptr = varNode(en_auto, syms->front());
    EXPRESSION* otherptr = nullptr;
    int oldCodeLabel = codeLabel;
    if (defaulted)
        codeLabel = INT_MIN;
    auto it = syms->begin();
    ++it;
    if (it != syms->end())
        otherptr = varNode(en_auto, *it);
    deref(&stdpointer, &thisptr);
    deref(&stdpointer, &otherptr);
    if (parseInitializers)
        allocInitializers(sym, cons, thisptr);
    if (cons->sb->memberInitializers && cons->sb->memberInitializers->size() && cons->sb->memberInitializers->front()->delegating)
    {
        genConstructorCall(b, sym, cons->sb->memberInitializers, sym, 0, false, thisptr, otherptr, cons, true, doCopy,
                           !cons->sb->defaulted);
    }
    else
    {
        if (sym->tp->type == bt_union)
        {
            AllocateLocalContext(emptyBlockdata, cons, codeLabel++);
            for (auto sp : *basetype(sym->tp)->syms)
            {
                if ((sp->sb->storage_class == sc_member || sp->sb->storage_class == sc_mutable) && sp->tp->type != bt_aggregate)
                {
                    if (sp->sb->init)
                    {
                        if (isstructured(sp->tp))
                        {
                            genConstructorCall(b, basetype(sp->tp)->sp, cons->sb->memberInitializers, sp, sp->sb->offset, true,
                                               thisptr, otherptr, cons, false, doCopy, !cons->sb->defaulted);
                        }
                        else
                        {
                            genConsData(b, sym, cons->sb->memberInitializers, sp, sp->sb->offset, thisptr, otherptr, cons, doCopy);
                        }
                    }
                }
            }
            FreeLocalContext(emptyBlockdata, cons, codeLabel++);
        }
        else
        {
            if (sym->sb->vbaseEntries)
            {
                SYMBOL* sp = makeID(sc_parameter, &stdint, nullptr, AnonymousName());
                EXPRESSION* val = varNode(en_auto, sp);
                int lbl = codeLabel++;
                STATEMENT* st;
                sp->sb->constop = true;
                sp->sb->decoratedName = sp->name;
                sp->sb->offset = Optimizer::chosenAssembler->arch->retblocksize + cons->sb->paramsize;
                localNameSpace->front()->syms->Add(sp);

                deref(&stdint, &val);
                st = stmtNode(nullptr, b, st_notselect);
                optimize_for_constants(&val);
                st->select = val;
                st->label = lbl;
                virtualBaseThunks(b, sym, thisptr);
                doVirtualBases(b, sym, cons->sb->memberInitializers, sym->sb->vbaseEntries, thisptr, otherptr, cons, doCopy);
                if (hasVTab(sym))
                    dovtabThunks(b, sym, thisptr, true);
                st = stmtNode(nullptr, b, st_label);
                st->label = lbl;
            }
            AllocateLocalContext(emptyBlockdata, cons, codeLabel++);
            if (sym->sb->baseClasses)
                for (auto bc : *sym->sb->baseClasses)
                    if (!bc->isvirtual)
                        genConstructorCall(b, sym, cons->sb->memberInitializers, bc->cls, bc->offset, false, thisptr, otherptr, cons,
                                           true, doCopy || !cons->sb->memberInitializers, !cons->sb->defaulted);
            if (hasVTab(sym))
                dovtabThunks(b, sym, thisptr, false);
            for (auto sp : * sym->tp->syms)
            {
                if ((sp->sb->storage_class == sc_member || sp->sb->storage_class == sc_mutable) && sp->tp->type != bt_aggregate && !sp->sb->wasUsing)
                {
                    if (isstructured(sp->tp))
                    {
                        genConstructorCall(b, basetype(sp->tp)->sp, cons->sb->memberInitializers, sp, sp->sb->offset, true, thisptr,
                                           otherptr, cons, false, doCopy, !cons->sb->defaulted);
                    }
                    else
                    {
                        genConsData(b, sym, cons->sb->memberInitializers, sp, sp->sb->offset, thisptr, otherptr, cons, doCopy);
                    }
                }
            }
            FreeLocalContext(emptyBlockdata, cons, codeLabel++);
        }
    }
    if (parseInitializers)
        releaseInitializers(sym, cons);
    cons->sb->labelCount = codeLabel - INT_MIN;
    if (defaulted)
        codeLabel = oldCodeLabel;
    return thisptr;
}
static bool DefaultConstructorConstExpression(SYMBOL* sp)
{
    if (sp->sb->constexpression)
        return true;
    if (sp->sb->vbaseEntries)
        return false;
    if (sp->tp->type != bt_union)
    {
        for (auto sp1 : *basetype(sp->tp)->syms)
        {
            TYPE* m;
            if (sp1->sb->storage_class == sc_mutable)
                return false;
            if (sp1->sb->storage_class == sc_member)
            {
                if (!is_literal_type(sp1->tp))
                    return false;
                if (!sp1->sb->memberInitializers)
                    return false;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (!DefaultConstructorConstExpression(base->cls))
                return false;
    sp->sb->constexpression = true;
    return true;
}

void createConstructor(SYMBOL* sp, SYMBOL* consfunc)
{
    SymbolTable<SYMBOL>* syms;
    BLOCKDATA bd = {};
    std::list<BLOCKDATA*> b{ &bd };
    STATEMENT* st;
    EXPRESSION* thisptr;
    bool oldNoExcept = noExcept;
    noExcept = true;
    bd.type = begin;
    syms = localNameSpace->front()->syms;
    localNameSpace->front()->syms = basetype(consfunc->tp)->syms;
    thisptr = thunkConstructorHead(b, sp, consfunc, basetype(consfunc->tp)->syms, false, true, true);
    st = stmtNode(nullptr, b, st_return);
    st->select = thisptr;
    if (!inNoExceptHandler)
    {
        auto stmt = stmtNode(nullptr, emptyBlockdata, st_block);
        consfunc->sb->inlineFunc.stmt = stmtListFactory.CreateList();
        stmt->lower = bd.statements;
        consfunc->sb->inlineFunc.stmt->push_back(stmt);
        consfunc->sb->inlineFunc.syms = basetype(consfunc->tp)->syms;
        consfunc->sb->retcount = 1;
        consfunc->sb->attribs.inheritable.isInline = true;
        //    consfunc->sb->inlineFunc.stmt->blockTail = b.tail;
        defaultRecursionMap.clear();
        if (noExcept)
        {
            consfunc->sb->xcMode = xc_none;
            consfunc->sb->noExcept = true;
        }
        else
        {
            consfunc->sb->xcMode = xc_unspecified;
            consfunc->sb->noExcept = false;
        }
    }
    else
    {
        defaultRecursionMap.clear();
        if (noExcept)
        {
            consfunc->sb->noExcept = true;
        }
        else
        {
            consfunc->sb->noExcept = false;
        }
    }
    consfunc->sb->constexpression =
        DefaultConstructorConstExpression(sp) || matchesCopy(consfunc, false) || matchesCopy(consfunc, true);
    sp->sb->literalClass |= consfunc->sb->constexpression;
    localNameSpace->front()->syms = syms;
    noExcept &= oldNoExcept;
}
void asnVirtualBases(std::list<BLOCKDATA*>& b, SYMBOL* sp, std::list<VBASEENTRY*>* vbe, EXPRESSION* thisptr, EXPRESSION* other, bool move, bool isconst)
{
    if (vbe && vbe->size())
    {
        for (auto it = vbe->end(); it != vbe->begin() ;)
        {
            --it;
            if ((*it)->alloc)
                genAsnCall(b, sp, (*it)->cls, (*it)->structOffset, thisptr, other, move, isconst);
        }
    }
}
static void genAsnData(std::list<BLOCKDATA*>& b, SYMBOL* cls, SYMBOL* member, int offset, EXPRESSION* thisptr, EXPRESSION* other)
{
    EXPRESSION* left = exprNode(en_structadd, thisptr, intNode(en_c_i, offset));
    EXPRESSION* right = exprNode(en_structadd, other, intNode(en_c_i, offset));
    left->right->keepZero = true;
    right->right->keepZero = true;
    STATEMENT* st;
    (void)cls;
    if (isstructured(member->tp) || isarray(member->tp))
    {
        left = exprNode(en_blockassign, left, right);
        left->size = member->tp;
        left->altdata = (void*)member->tp;
    }
    else
    {
        deref(member->tp, &left);
        deref(member->tp, &right);
        left = exprNode(en_assign, left, right);
    }
    st = stmtNode(nullptr, b, st_expr);
    optimize_for_constants(&left);
    st->select = left;
}
static void genAsnCall(std::list<BLOCKDATA*>& b, SYMBOL* cls, SYMBOL* base, int offset, EXPRESSION* thisptr, EXPRESSION* other, bool move,
                       bool isconst)
{
    (void)cls;
    EXPRESSION* exp = nullptr;
    STATEMENT* st;
    FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
    TYPE* tp = CopyType(base->tp);
    SYMBOL* asn1;
    SYMBOL* cons = search(basetype(base->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
    EXPRESSION* left = exprNode(en_add, thisptr, intNode(en_c_i, offset));
    EXPRESSION* right = exprNode(en_add, other, intNode(en_c_i, offset));
    if (move)
    {
        right = exprNode(en_not_lvalue, right, nullptr);
    }
    if (isconst)
    {
        tp = MakeType(bt_const, tp);
    }
    if (move)
    {
        tp->rref = true;
        tp->lref = false;
    }
    else
    {
        tp->lref = true;
        tp->rref = false;
    }
    params->arguments = initListListFactory.CreateList();
    auto arg = Allocate<INITLIST>();
    arg->tp = tp;
    arg->exp = right;
    params->arguments->push_back(arg);
    params->thisptr = left;
    params->thistp = MakeType(bt_pointer, base->tp);
    params->ascall = true;
    asn1 = GetOverloadedFunction(&tp, &params->fcall, cons, params, nullptr, true, false, 0);

    if (asn1)
    {
        SYMBOL* parm = nullptr;
        auto it = basetype(asn1->tp)->syms->begin();
        AdjustParams(asn1, it, basetype(asn1->tp)->syms->end(), &params->arguments, false, true);
        ++it;
        if (it != basetype(asn1->tp)->syms->end())
            parm = *it;
        if (parm && isref(parm->tp))
        {
            params->arguments->front()->tp = MakeType(bt_lref, params->arguments->front()->tp);
        }
        if (!isAccessible(base, base, asn1, nullptr, ac_protected, false))
        {
            errorsym(ERR_CANNOT_ACCESS, asn1);
        }
        if (asn1->sb->defaulted && !asn1->sb->inlineFunc.stmt)
            createAssignment(base, asn1);
        noExcept &= asn1->sb->noExcept;

        params->functp = asn1->tp;
        params->sp = asn1;
        params->ascall = true;
        exp = varNode(en_func, nullptr);
        exp->v.func = params;
    }
    st = stmtNode(nullptr, b, st_expr);
    optimize_for_constants(&exp);
    st->select = exp;
}
static EXPRESSION* thunkAssignments(std::list<BLOCKDATA*>& b, SYMBOL* sym, SYMBOL* asnfunc, SymbolTable<SYMBOL>* syms, bool move, bool isconst)
{
    auto it = syms->begin();
    EXPRESSION* thisptr = varNode(en_auto, *it);
    EXPRESSION* other = nullptr;
    BASECLASS* base;
    int oldCodeLabel = codeLabel;
    codeLabel = INT_MIN;
    ++it;
    if (it != syms->end())  // this had better be true
        other = varNode(en_auto, *it);
    deref(&stdpointer, &thisptr);
    deref(&stdpointer, &other);
    if (sym->tp->type == bt_union)
    {
        genAsnData(b, sym, sym, 0, thisptr, other);
    }
    else
    {
        if (sym->sb->vbaseEntries)
        {
            asnVirtualBases(b, sym, sym->sb->vbaseEntries, thisptr, other, move, isconst);
        }
        if (sym->sb->baseClasses)
            for (auto base : *sym->sb->baseClasses)
                if (!base->isvirtual)
                {
                    genAsnCall(b, sym, base->cls, base->offset, thisptr, other, move, isconst);
                }
        for (auto sp : *sym->tp->syms)
        {
            if ((sp->sb->storage_class == sc_member || sp->sb->storage_class == sc_mutable) && sp->tp->type != bt_aggregate)
            {
                if (isstructured(sp->tp))
                {
                    genAsnCall(b, sym, basetype(sp->tp)->sp, sp->sb->offset, thisptr, other, move, isconst);
                }
                else
                {
                    genAsnData(b, sym, sp, sp->sb->offset, thisptr, other);
                }
            }
        }
    }
    asnfunc->sb->labelCount = codeLabel - INT_MIN;
    codeLabel = oldCodeLabel;
    return thisptr;
}
void createAssignment(SYMBOL* sym, SYMBOL* asnfunc)
{
    // if we get here we are just assuming it is a builtin assignment operator
    // because we only get here for 'default' functions and that is the only one
    // that can be defaulted...
    SymbolTable<SYMBOL>* syms;
    bool oldNoExcept = noExcept;
    noExcept = true;
    BLOCKDATA bd = {};
    std::list<BLOCKDATA*> b = { &bd };
    auto it = basetype(asnfunc->tp)->syms->begin();
    ++it;
    bool move = basetype((*it)->tp)->type == bt_rref;
    bool isConst = isconst((*it)->tp);
    bd.type = begin;
    syms = localNameSpace->front()->syms;
    localNameSpace->front()->syms = basetype(asnfunc->tp)->syms;
    auto thisptr = thunkAssignments(b, sym, asnfunc, basetype(asnfunc->tp)->syms, move, isConst);
    auto st = stmtNode(nullptr, b, st_return);
    st->select = thisptr;
    if (!inNoExceptHandler)
    {
        asnfunc->sb->inlineFunc.stmt = stmtListFactory.CreateList();
        auto stmt = stmtNode(nullptr, emptyBlockdata, st_block);
        asnfunc->sb->inlineFunc.stmt->push_back(stmt);
        stmt->lower = bd.statements;
        asnfunc->sb->inlineFunc.syms = basetype(asnfunc->tp)->syms;
        asnfunc->sb->attribs.inheritable.isInline = true;
        //    asnfunc->sb->inlineFunc.stmt->blockTail = b.tail;

        defaultRecursionMap.clear();
        if (noExcept)
        {
            asnfunc->sb->xcMode = xc_none;
            asnfunc->sb->noExcept = true;
        }
        else
        {
            asnfunc->sb->xcMode = xc_unspecified;
            asnfunc->sb->noExcept = false;
        }
    }
    else
    {
        defaultRecursionMap.clear();
        if (noExcept)
        {
            asnfunc->sb->noExcept = true;
        }
        else
        {
            asnfunc->sb->noExcept = false;
        }
    }
    localNameSpace->front()->syms = syms;
    noExcept &= oldNoExcept;
}
static void genDestructorCall(std::list<BLOCKDATA*>& b, SYMBOL* sp, SYMBOL* against, EXPRESSION* base, EXPRESSION* arrayElms, int offset,
                              bool top)
{
    SYMBOL* dest;
    EXPRESSION* exp;
    STATEMENT* st;
    TYPE* tp = PerformDeferredInitialization(sp->tp, nullptr);
    sp = tp->sp;
    dest = search(basetype(sp->tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
    if (!dest)  // error handling
        return;
    exp = base;
    deref(&stdpointer, &exp);
    exp = exprNode(en_add, exp, intNode(en_c_i, offset));
    dest = (SYMBOL*)basetype(dest->tp)->syms->front();
    if (dest->sb->defaulted && !dest->sb->inlineFunc.stmt)
    {
        createDestructor(sp);
    }
    callDestructor(sp, against, &exp, arrayElms, top, true, false, true);
    st = stmtNode(nullptr, b, st_expr);
    optimize_for_constants(&exp);
    st->select = exp;
}
static void undoVars(std::list<BLOCKDATA*>& b, SymbolTable<SYMBOL>* vars, EXPRESSION* base)
{
    if (vars)
    {
        std::stack<SYMBOL*> stk;
        for (auto p : *vars)
        {
            stk.push(p);
        }
        while (stk.size())
        {
            SYMBOL* s = (SYMBOL*)stk.top();
            stk.pop();
            if ((s->sb->storage_class == sc_member || s->sb->storage_class == sc_mutable) && !s->sb->wasUsing)
            {
                if (isstructured(s->tp))
                {
                    genDestructorCall(b, (SYMBOL*)basetype(s->tp)->sp, nullptr, base, nullptr, s->sb->offset, true);
                }
                else if (isarray(s->tp))
                {
                    TYPE* tp = s->tp;
                    while (isarray(tp))
                        tp = basetype(tp)->btp;
                    tp = basetype(tp);
                    if (isstructured(tp))
                        genDestructorCall(b, tp->sp, nullptr, base, intNode(en_c_i, s->tp->size / tp->size), s->sb->offset, true);
                }
            }
        }
    }
}
static void undoBases(std::list<BLOCKDATA*>& b, SYMBOL* against, std::list<BASECLASS*>* bc, EXPRESSION* base)
{
    if (bc && bc->size())
    {
        for (auto it = bc->end(); it != bc->begin();)
        {
            --it;
            if (!(*it)->isvirtual)
            {
                genDestructorCall(b, (*it)->cls, against, base, nullptr, (*it)->offset, false);
            }
        }
    }
}
void thunkDestructorTail(std::list<BLOCKDATA*>& b, SYMBOL* sp, SYMBOL* dest, SymbolTable<SYMBOL>* syms, bool defaulted)
{
    InsertInline(sp);
    if (sp->tp->type != bt_union)
    {
        EXPRESSION* thisptr;
        int oldCodeLabel = codeLabel;
        if (templateNestingCount)
            return;
        if (defaulted)
            codeLabel = INT_MIN;
        thisptr = varNode(en_auto, syms->front());
        undoVars(b, basetype(sp->tp)->syms, thisptr);
        undoBases(b, sp, sp->sb->baseClasses, thisptr);
        if (sp->sb->vbaseEntries)
        {
            auto it = syms->begin();
            ++it;
            auto sp1 = *it;
            EXPRESSION* val = varNode(en_auto, sp1);
            int lbl = codeLabel++;
            STATEMENT* st;
            sp1->sb->decoratedName = sp1->name;
            sp1->sb->offset = Optimizer::chosenAssembler->arch->retblocksize + getSize(bt_pointer);
            Optimizer::SymbolManager::Get(sp1)->offset = sp1->sb->offset;
            deref(&stdint, &val);
            st = stmtNode(nullptr, b, st_notselect);
            optimize_for_constants(&val);
            st->select = val;
            st->label = lbl;
            for (auto vbe : *sp->sb->vbaseEntries)
            {
                if (vbe->alloc)
                    genDestructorCall(b, vbe->cls, sp, thisptr, nullptr, vbe->structOffset, false);
            }
            st = stmtNode(nullptr, b, st_label);
            st->label = lbl;
        }
        dest->sb->labelCount = codeLabel - INT_MIN;
        if (defaulted)
            codeLabel = oldCodeLabel;
    }
}
void createDestructor(SYMBOL* sp)
{
    SymbolTable<SYMBOL>* syms;
    SYMBOL* dest = search(basetype(sp->tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
    bool oldNoExcept = noExcept;
    noExcept = true;
    BLOCKDATA bd = {};
    std::list<BLOCKDATA*> b = { &bd };
    bd.type = begin;
    dest = (SYMBOL*)basetype(dest->tp)->syms->front();
    syms = localNameSpace->front()->syms;
    localNameSpace->front()->syms = basetype(dest->tp)->syms;
    thunkDestructorTail(b, sp, dest, basetype(dest->tp)->syms, true);
    if (!inNoExceptHandler)
    {
        dest->sb->inlineFunc.stmt = stmtListFactory.CreateList();
        auto stmt = stmtNode(nullptr, emptyBlockdata, st_block);
        dest->sb->inlineFunc.stmt->push_back(stmt);
        stmt->lower = bd.statements;
        dest->sb->inlineFunc.syms = basetype(dest->tp)->syms;
        dest->sb->retcount = 1;
        dest->sb->attribs.inheritable.isInline = dest->sb->attribs.inheritable.linkage2 != lk_export;
    }
    if (noExcept)
    {
        dest->sb->xcMode = xc_none;
        dest->sb->noExcept = true;
        noExcept = true;
    }
    else
    {
        dest->sb->xcMode = xc_unspecified;
        dest->sb->noExcept = false;
    }
    //    dest->sb->inlineFunc.stmt->blockTail = b.tail;
    localNameSpace->front()->syms = syms;
    noExcept &= oldNoExcept;
}
void makeArrayConsDest(TYPE** tp, EXPRESSION** exp, SYMBOL* cons, SYMBOL* dest, EXPRESSION* count)
{
    EXPRESSION* size = intNode(en_c_i, (*tp)->size);
    EXPRESSION *econs = (cons ? varNode(en_pc, cons) : nullptr), *edest = varNode(en_pc, dest);
    FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
    SYMBOL* asn1;
    INITLIST* arg0 = Allocate<INITLIST>();  // this
    INITLIST* arg1 = Allocate<INITLIST>();  // cons
    INITLIST* arg2 = Allocate<INITLIST>();  // dest
    INITLIST* arg3 = Allocate<INITLIST>();  // size
    INITLIST* arg4 = Allocate<INITLIST>();  // count
    SYMBOL* ovl = namespacesearch("__arrCall", globalNameSpace, false, false);
    params->arguments = initListListFactory.CreateList();
    params->arguments->push_back(arg0);
    params->arguments->push_back(arg1);
    params->arguments->push_back(arg2);
    params->arguments->push_back(arg3);
    params->arguments->push_back(arg4);

    if (econs)
        thunkForImportTable(&econs);
    thunkForImportTable(&edest);
    arg0->exp = *exp;
    arg0->tp = &stdpointer;
    arg1->exp = econs ? econs : intNode(en_c_i, 0);
    arg1->tp = &stdpointer;
    arg2->exp = edest;
    arg2->tp = &stdpointer;
    arg3->exp = count;
    arg3->tp = &stdint;
    arg4->exp = size;
    arg4->tp = &stdint;

    params->ascall = true;
    asn1 = GetOverloadedFunction(tp, &params->fcall, ovl, params, nullptr, true, false, 0);
    if (!asn1)
    {
        diag("makeArrayConsDest: Can't call array iterator");
    }
    else
    {
        params->functp = asn1->tp;
        params->sp = asn1;
        params->ascall = true;
        *exp = varNode(en_func, nullptr);
        (*exp)->v.func = params;
    }
}
bool callDestructor(SYMBOL* sp, SYMBOL* against, EXPRESSION** exp, EXPRESSION* arrayElms, bool top, bool pointer, bool skipAccess,
                    bool novtab)
{
    if (!sp)
        return false;
    SYMBOL* dest;
    SYMBOL* dest1;
    TYPE *tp = nullptr, *stp;
    FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
    SYMBOL* sym;
    if (!against)
        against = theCurrentFunc ? theCurrentFunc->sb->parentClass : sp;
    if (sp->tp->size == 0)
        sp = PerformDeferredInitialization(sp->tp, nullptr)->sp;
    stp = sp->tp;
    dest = search(basetype(sp->tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
    // if it isn't already defined get out, there will be an error from somewhere else..
    if (!basetype(sp->tp)->syms || !dest)
        return false;
    sym = basetype(sp->tp)->sp;
    if (!*exp)
    {
        diag("callDestructor: no this pointer");
    }
    params->thisptr = *exp;
    params->thistp = MakeType(bt_pointer, sp->tp);
    params->ascall = true;
    dest1 = basetype(dest->tp)->syms->front();
    if (!dest1 || !dest1->sb->defaulted || dest1->sb->storage_class == sc_virtual)
    {
        dest1 = GetOverloadedFunction(&tp, &params->fcall, dest, params, nullptr, true, false, inNothrowHandler ? _F_IS_NOTHROW : 0);
        if (!novtab && dest1 && dest1->sb->storage_class == sc_virtual)
        {
            auto exp_in = params->thisptr;
            deref(&stdpointer, &exp_in);
            exp_in = exprNode(en_add, exp_in, intNode(en_c_i, dest1->sb->vtaboffset));
            deref(&stdpointer, &exp_in);
            params->fcall = exp_in;
        }
        if (dest1)
            noExcept &= dest1->sb->noExcept;
    }
    else
        params->fcall = varNode(en_pc, dest1);
    if (dest1)
    {
        CheckCalledException(dest1, params->thisptr);
        if (params->thisptr->type == en_auto && !dest1->sb->defaulted)
            params->thisptr->v.sp->sb->addressTaken = true;

        if (!skipAccess && dest1 &&
            !isAccessible(against, sp, dest1, theCurrentFunc,
                          top ? (theCurrentFunc && theCurrentFunc->sb->parentClass == sp ? ac_protected : ac_public) : ac_protected,
                          false))
        {
            errorsym(ERR_CANNOT_ACCESS, dest1);
        }
        if (dest1 && dest1->sb->defaulted && !dest1->sb->inlineFunc.stmt)
            createDestructor(sp);
        params->functp = dest1->tp;
        params->sp = dest1;
        params->ascall = true;
        if (arrayElms)
        {
            makeArrayConsDest(&stp, exp, nullptr, dest1, arrayElms);
        }
        else
        {
            if (sp->sb->vbaseEntries)
            {
                INITLIST *x = Allocate<INITLIST>(), **p;
                x->tp = MakeType(bt_int);
                x->exp = intNode(en_c_i, top);
                if (!params->arguments)
                    params->arguments = initListListFactory.CreateList();
                params->arguments->push_back(x);
                params->sp->sb->noinline = true;
            }
            *exp = varNode(en_func, nullptr);
            (*exp)->v.func = params;
        }
        if (*exp && !pointer)
        {
            *exp = exprNode(en_thisref, *exp, nullptr);
            (*exp)->dest = true;
            (*exp)->v.t.thisptr = params->thisptr;
            (*exp)->v.t.tp = sp->tp;
            sp->sb->hasDest = true;
            if (!sp->sb->pureDest)
                hasXCInfo = true;
        }
    }
    return true;
}
bool callConstructor(TYPE** tp, EXPRESSION** exp, FUNCTIONCALL* params, bool checkcopy, EXPRESSION* arrayElms, bool top,
                     bool maybeConversion, bool implicit, bool pointer, bool usesInitList, bool isAssign, bool toErr)
{
    (void)checkcopy;
    TYPE* stp = *tp;
    SYMBOL* sp;
    SYMBOL* against,* against2;
    SYMBOL* cons;
    SYMBOL* cons1;
    EXPRESSION* e1 = nullptr;
    TYPE* initializerListTemplate = nullptr;
    TYPE* initializerListType = nullptr;
    bool initializerRef = false;
    PerformDeferredInitialization(stp, nullptr);
    sp = basetype(*tp)->sp;
    against = theCurrentFunc ? theCurrentFunc->sb->parentClass : top ? sp : sp->sb->parentClass;
    against2 = theCurrentFunc && theCurrentFunc->sb->parentClass ? theCurrentFunc->sb->parentClass : nullptr;
    if (isAssign)
    {
        cons = search(basetype(sp->tp)->syms, overloadNameTab[assign - kw_new + CI_NEW]);
    }
    else
    {
        cons = search(basetype(sp->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
    }

    if (!params)
    {
        params = Allocate<FUNCTIONCALL>();
    }
    else
    {
        if (params->arguments)
        {
            for (auto list : *params->arguments)
            {
                if (!list->nested && isstructured(list->tp))
                {
                    SYMBOL* sp1 = basetype(list->tp)->sp;
                    if (!templateNestingCount && sp1->sb->templateLevel && sp1->templateParams && !sp1->sb->instantiated)
                    {
                        if (!allTemplateArgsSpecified(sp1, sp1->templateParams))
                            sp1 = GetClassTemplate(sp1, sp1->templateParams, false);
                        if (sp1)
                            list->tp = TemplateClassInstantiate(sp1, sp1->templateParams, false, sc_global)->tp;
                    }
                }
            }
        }
    }
    params->thisptr = *exp;
    params->thistp = MakeType(bt_pointer, sp->tp);
    params->ascall = true;

    cons1 = GetOverloadedFunction(tp, &params->fcall, cons, params, nullptr, toErr, maybeConversion, (usesInitList ? _F_INITLIST : 0) | _F_INCONSTRUCTOR | (inNothrowHandler ? _F_IS_NOTHROW : 0));

    if (cons1 && isfunction(cons1->tp))
    {
        CheckCalledException(cons1, params->thisptr);
        if (cons1->sb->castoperator)
        {
            FUNCTIONCALL* oparams = Allocate<FUNCTIONCALL>();
            if (!inNoExceptHandler &&
                !isAccessible(cons1->sb->parentClass, cons1->sb->parentClass, cons1, nullptr, ac_public, false))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            if (cons1->sb->isExplicit && implicit)
                error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
            oparams->fcall = params->fcall;
            oparams->thisptr = params->arguments->front()->exp;
            oparams->thistp = MakeType(bt_pointer, cons1->sb->parentClass->tp);
            oparams->functp = cons1->tp;
            oparams->sp = cons1;
            oparams->ascall = true;
            if (!isref(basetype(cons1->tp)->btp))
            {
                optimize_for_constants(exp);
                oparams->returnEXP = *exp;
                oparams->returnSP = sp;
            }
            e1 = varNode(en_func, nullptr);
            e1->v.func = oparams;
        }
        else
        {
            if (!inNoExceptHandler && cons1->sb->access != ac_public &&
                !isAccessible(against, against2, cons1, theCurrentFunc,
                              top ? (theCurrentFunc && theCurrentFunc->sb->parentClass == sp ? ac_private : ac_public) : ac_private,
                              false))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            if (cons1->sb->isExplicit && implicit)
                error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
            {
                auto it = basetype(cons1->tp)->syms->begin();
                if ((*it)->sb->thisPtr)
                    ++it;
                TYPE* tp = (*it)->tp;
                if (isref(tp))
                {
                    initializerRef = true;
                    tp = basetype(tp)->btp;
                }
                if (isstructured(tp))
                {
                    SYMBOL* sym = (basetype(tp)->sp);
                    if (sym->sb->initializer_list && sym->sb->templateLevel)
                    {
                        auto it = sym->templateParams->begin();
                        ++it;
                        initializerListTemplate = sym->tp;
                        initializerListType = it->second->byClass.val;
                    }
                }
            }
            if (initializerListType && (!params->arguments->front()->tp || !isstructured(params->arguments->front()->tp) ||
                                        !basetype(params->arguments->front()->tp)->sp->sb->initializer_list))
            {
                std::list<INITLIST*>* old = params->arguments;
                std::list<INITLIST*> temp;
                std::list<INITLIST*>* temp2 = &temp;

                if (params->arguments && params->arguments->size())
                {
                    if (params->arguments->front()->nested && params->arguments->front()->nested->front()->nested &&
                        !params->arguments->front()->initializer_list)
                        temp.push_back(params->arguments->front());
                    else
                        temp = *params->arguments;
                }
                if (!params->arguments->front()->initializer_list)
                {
                    temp = *params->arguments->front()->nested;
                }
                CreateInitializerList(cons1, initializerListTemplate, initializerListType, &temp2, false,
                                      initializerRef);
                params->arguments = temp2;
                if (old->size() &&  (!old->front()->initializer_list ||
                             (old->front()->nested && old->front()->nested->front()->nested && !old->front()->initializer_list)))
                {
                    auto it1 = old->begin();
                    if (it1 != old->end())
                    {
                        ++it1;
                        params->arguments->insert(params->arguments->end(), it1, old->end());
                    }
                }
                auto it = basetype(cons1->tp)->syms->begin();
                ++it;
                ++it;
                if (it != basetype(cons1->tp)->syms->end())
                {
                    auto x = params->arguments->front();
                    params->arguments->pop_front();
                    AdjustParams(cons1, it, basetype(cons1->tp)->syms->end(), &params->arguments, false,
                        implicit && !cons1->sb->isExplicit);
                    params->arguments->push_front(x);
                }
            }
            else
            {
                std::list<INITLIST*> temp;
                std::list<INITLIST*>* temp2 = &temp;
                if (params->arguments && params->arguments->size() && params->arguments->front()->nested && !params->arguments->front()->initializer_list)
                {
                    temp = *params->arguments->front()->nested;
                    *params->arguments = temp;
                }
                AdjustParams(cons1, basetype(cons1->tp)->syms->begin(), basetype(cons1->tp)->syms->end(), &params->arguments, false,
                             implicit && !cons1->sb->isExplicit);
            }
            params->functp = cons1->tp;
            params->sp = cons1;
            params->ascall = true;
            if (cons1->sb->defaulted && !cons1->sb->inlineFunc.stmt)
                createConstructor(sp, cons1);
            noExcept &= cons1->sb->noExcept;
            if (arrayElms)
            {
                SYMBOL* dest = search(basetype(sp->tp)->syms, overloadNameTab[CI_DESTRUCTOR]);
                SYMBOL* dest1;
                SYMBOL* against = top ? sp : sp->sb->parentClass;
                TYPE* tp = nullptr;
                FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                if (!*exp)
                {
                    diag("callDestructor: no this pointer");
                }
                params->thisptr = *exp;
                params->thistp = MakeType(bt_pointer, sp->tp);
                params->ascall = true;
                dest1 = GetOverloadedFunction(&tp, &params->fcall, dest, params, nullptr, true, false, 0);
                if (dest1 &&
                    !isAccessible(against, sp, dest1, nullptr,
                                  top ? (theCurrentFunc && theCurrentFunc->sb->parentClass == sp ? ac_protected : ac_public)
                                      : ac_protected,
                                  false))
                {
                    errorsym(ERR_CANNOT_ACCESS, dest1);
                }
                if (dest1 && dest1->sb->defaulted && !dest1->sb->inlineFunc.stmt)
                    createDestructor(sp);
                makeArrayConsDest(&stp, exp, cons1, dest1, arrayElms);
                e1 = *exp;
            }
            else
            {
                if (sp->sb->vbaseEntries)
                {
                    INITLIST *x = Allocate<INITLIST>(), **p;
                    x->tp = MakeType(bt_int);
                    x->exp = intNode(en_c_i, top);
                    if (!params->arguments)
                        params->arguments = initListListFactory.CreateList();
                    params->arguments->push_back(x);
                    params->sp->sb->noinline = true;
                }
                e1 = varNode(en_func, nullptr);
                e1->v.func = params;
            }
        }

        *exp = e1;
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && *exp)
        {
            // this needs work, won't work for structures as members of other structures...
            EXPRESSION* exp1 = params->thisptr;
            if ((exp1->type == en_add || exp1->type == en_structadd) && isconstzero(&stdint, exp1->right))
                exp1 = exp1->left;
            *exp = exprNode(en_assign, exp1, *exp);
            params->thisptr = nullptr;
        }
        else if (*exp && !pointer)
        {
            *exp = exprNode(en_thisref, *exp, nullptr);
            (*exp)->v.t.thisptr = params->thisptr;
            (*exp)->v.t.tp = sp->tp;
            optimize_for_constants(exp);  // for constexpr constructors
            // hasXCInfo = true;
        }

        return true;
    }
    return false;
}
bool callConstructorParam(TYPE** tp, EXPRESSION** exp, TYPE* paramTP, EXPRESSION* paramExp, bool top, bool maybeConversion,
                          bool implicit, bool pointer, bool toErr)
{
    FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
    if (paramTP && paramExp)
    {
        params->arguments = initListListFactory.CreateList();
        params->arguments->push_back(Allocate<INITLIST>());
        params->arguments->front()->tp = paramTP;
        params->arguments->front()->exp = paramExp;
    }
    return callConstructor(tp, exp, params, false, nullptr, top, maybeConversion, implicit, pointer, false, false, toErr);
}

void PromoteConstructorArgs(SYMBOL* cons1, FUNCTIONCALL* params)
{
    if (!cons1)
    {
        return;
    }
    auto it = basetype(cons1->tp)->syms->begin();
    auto ite = basetype(cons1->tp)->syms->end();
    if ((*it)->sb->thisPtr)
        ++it;
    std::list<INITLIST*>::iterator args, argse;
    if (params->arguments)
    {
        args = params->arguments->begin();
        argse = params->arguments->end();
    }
    while (it != ite && args != argse)
    {
        SYMBOL* sp = *it;
        TYPE* tps = basetype(sp->tp);
        TYPE* tpa = basetype((*args)->tp);
        if (isarithmetic(tps) && isarithmetic(tpa))
        {
            if (tps->type > bt_int && tps->type != tpa->type)
            {
                (*args)->tp = sp->tp;
                cast(sp->tp, &(*args)->exp);
            }
        }
        ++it;
        ++args;
    }
}
}  // namespace Parser