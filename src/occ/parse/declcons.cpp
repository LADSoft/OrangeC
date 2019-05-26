/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
extern ARCH_ASM* chosenAssembler;
extern bool hasXCInfo;
extern int templateNestingCount;
extern SYMBOL* theCurrentFunc;
extern int codeLabel;
extern const char* overloadNameTab[];
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern TYPE stdpointer, stdint;
extern int total_errors;
extern INCLUDES* includes;
extern bool functionCanThrow;
extern int argument_nesting;

static void genAsnCall(BLOCKDATA* b, SYMBOL* cls, SYMBOL* base, int offset, EXPRESSION* thisptr, EXPRESSION* other, bool move,
                       bool isconst);
static void createDestructor(SYMBOL* sp);

void ConsDestDeclarationErrors(SYMBOL* sp, bool notype)
{
    if (sp->isConstructor)
    {
        if (!notype)
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_TYPE);
        else if (sp->storage_class == sc_virtual)
            errorstr(ERR_INVALID_STORAGE_CLASS, "virtual");
        else if (sp->storage_class == sc_static)
            errorstr(ERR_INVALID_STORAGE_CLASS, "static");
        else if (isconst(sp->tp) || isvolatile(sp->tp))
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_CONST_VOLATILE);
    }
    else if (sp->isDestructor)
    {
        if (!notype)
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_TYPE);
        else if (sp->storage_class == sc_static)
            errorstr(ERR_INVALID_STORAGE_CLASS, "static");
        else if (isconst(sp->tp) || isvolatile(sp->tp))
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_CONST_VOLATILE);
    }
    else if (sp->parentClass && !strcmp(sp->name, sp->parentClass->name))
    {
        error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_TYPE);
    }
}
LEXEME* FindClass(LEXEME* lex, SYMBOL* funcsp, SYMBOL** sym)
{
    SYMBOL* encloser = NULL;
    NAMESPACEVALUES* ns = NULL;
    bool throughClass = false;
    TYPE* castType = NULL;
    char buf[512];
    int ov = 0;
    bool namespaceOnly = false;

    *sym = NULL;

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
MEMBERINITIALIZERS* GetMemberInitializers(LEXEME** lex2, SYMBOL* funcsp, SYMBOL* sym)
{
    (void)sym;
    LEXEME *lex = *lex2, *last = NULL;
    MEMBERINITIALIZERS *first = NULL, **cur = &first;
    //    if (sym->name != overloadNameTab[CI_CONSTRUCTOR])
    //        error(ERR_INITIALIZER_LIST_REQUIRES_CONSTRUCTOR);
    while (lex != NULL)
    {
        if (ISID(lex) || MATCHKW(lex, classsel))
        {
            SYMBOL* sym = NULL;
            lex = FindClass(lex, funcsp, &sym);
            LEXEME** mylex;
            char name[1024];
            *cur = (MEMBERINITIALIZERS *)Alloc(sizeof(MEMBERINITIALIZERS));
            (*cur)->line = lex->line;
            (*cur)->file = lex->file;
            mylex = &(*cur)->initData;
            name[0] = 0;
            if (ISID(lex))
            {
                strcpy(name, lex->value.s.a);
                lex = getsym();
            }
            (*cur)->name = litlate(name);
            if (sym && istype(sym))
                (*cur)->basesym = sym;
            if (MATCHKW(lex, lt))
            {
                int paren = 0, tmpl = 0;
                *mylex = (LEXEME *)Alloc(sizeof(*(*cur)->initData));
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
                    if (lex->type == l_id)
                        lex->value.s.a = litlate(lex->value.s.a);
                    *mylex = (LEXEME *)Alloc(sizeof(*(*cur)->initData));
                    if (MATCHKW(lex, rightshift))
                    {
                        lex = getGTSym(lex);
                        **mylex = *lex;
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
                    *mylex = (LEXEME *)Alloc(sizeof(*(*cur)->initData));
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
                *mylex = (LEXEME *)Alloc(sizeof(*(*cur)->initData));
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
                    if (lex->type == l_id)
                        lex->value.s.a = litlate(lex->value.s.a);
                    *mylex = (LEXEME *)Alloc(sizeof(*(*cur)->initData));
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
                if (MATCHKW(lex, close))
                {
                    *mylex = (LEXEME *)Alloc(sizeof(*(*cur)->initData));
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
                if (MATCHKW(lex, ellipse))
                {
                    (*cur)->packed = true;
                    lex = getsym();
                }
                (*mylex) = NULL;
            }
            else
            {
                error(ERR_MEMBER_INITIALIZATION_REQUIRED);
                skip(&lex, closepa);
                break;
            }
            cur = &(*cur)->next;
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
    return first;
}
void SetParams(SYMBOL* cons)
{
    // c style only
    HASHREC* params = basetype(cons->tp)->syms->table[0];
    int base = chosenAssembler->arch->retblocksize;
    if (isstructured(basetype(cons->tp)->btp) || basetype(basetype(cons->tp)->btp)->type == bt_memberptr)
    {
        // handle structured return values
        base += getSize(bt_pointer);
        if (base % chosenAssembler->arch->parmwidth)
            base += chosenAssembler->arch->parmwidth - base % chosenAssembler->arch->parmwidth;
    }
    if (ismember(cons))
    {
        // handle 'this' pointer
        assignParam(cons, &base, (SYMBOL*)params->p);
        params = params->next;
    }
    while (params)
    {
        assignParam(cons, &base, (SYMBOL*)params->p);
        params = params->next;
    }
    cons->paramsize = base - chosenAssembler->arch->retblocksize;
}
SYMBOL* insertFunc(SYMBOL* sp, SYMBOL* ovl)
{
    SYMBOL* funcs = search(ovl->name, basetype(sp->tp)->syms);
    ovl->parentClass = sp;
    ovl->internallyGenned = true;
    ovl->linkage = lk_virtual;
    ovl->defaulted = true;
    ovl->access = ac_public;
    ovl->templateLevel = templateNestingCount;
    if (!ovl->decoratedName)
        SetLinkerNames(ovl, lk_cdecl);
    if (!funcs)
    {
        TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
        tp->type = bt_aggregate;
        tp->rootType = tp;
        funcs = makeID(sc_overloads, tp, 0, ovl->name);
        funcs->parentClass = sp;
        tp->sp = funcs;
        SetLinkerNames(funcs, lk_cdecl);
        insert(funcs, basetype(sp->tp)->syms);
        funcs->parent = sp;
        funcs->tp->syms = CreateHashTable(1);
        insert(ovl, funcs->tp->syms);
        ovl->overloadName = funcs;
    }
    else if (funcs->storage_class == sc_overloads)
    {
        insertOverload(ovl, funcs->tp->syms);
        ovl->overloadName = funcs;
    }
    else
    {
        diag("insertFunc: invalid overload tab");
    }
    injectThisPtr(ovl, basetype(ovl->tp)->syms);
    SetParams(ovl);
    return ovl;
}
static SYMBOL* declareDestructor(SYMBOL* sp)
{
    SYMBOL* rv;
    SYMBOL *func, *sp1;
    TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
    VBASEENTRY* e;
    BASECLASS* b;
    HASHREC* hr;
    tp->type = bt_func;
    tp->size = getSize(bt_pointer);
    tp->btp = (TYPE*)Alloc(sizeof(TYPE));
    tp->btp->type = bt_void;
    tp->rootType = tp;
    tp->btp->rootType = tp->btp;
    func = makeID(sc_member, tp, NULL, overloadNameTab[CI_DESTRUCTOR]);
    func->xcMode = xc_none;
    func->attribs.inheritable.linkage2 = sp->attribs.inheritable.linkage2;
    tp->syms = CreateHashTable(1);
    sp1 = makeID(sc_parameter, tp->btp, NULL, AnonymousName());
    insert(sp1, tp->syms);
    if (sp->vbaseEntries)
    {
        sp1 = makeID(sc_parameter, &stdint, NULL, AnonymousName());
        sp1->isDestructor = true;
        insert(sp1, tp->syms);
    }
    rv = insertFunc(sp, func);
    rv->isDestructor = true;
    b = sp->baseClasses;
    while (b)
    {
        if (!b->cls->pureDest)
            break;
        b = b->next;
    }
    if (!b)
    {
        e = sp->vbaseEntries;
        while (e)
        {
            if (!e->cls->pureDest)
                break;
            e = e->next;
        }
        if (!e)
        {
            hr = basetype(sp->tp)->syms->table[0];
            while (hr)
            {
                SYMBOL* cls = (SYMBOL*)hr->p;
                TYPE* tp = cls->tp;
                while (isarray(tp))
                    tp = basetype(tp)->btp;
                if (isstructured(tp) && !basetype(tp)->sp->pureDest)
                    break;
                hr = hr->next;
            }
            if (!hr)
                sp->pureDest = true;
        }
    }

    return rv;
}
static bool hasConstFunc(SYMBOL* sp, int type, bool move)
{
    SYMBOL* ovl = search(overloadNameTab[type], basetype(sp->tp)->syms);
    if (ovl)
    {
        HASHREC* hr = basetype(ovl->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* func = (SYMBOL*)hr->p;
            HASHREC* hra = basetype(func->tp)->syms->table[0]->next;
            if (hra && (!hra->next || ((SYMBOL*)hra->next->p)->init))
            {
                SYMBOL* arg = (SYMBOL*)hra->p;
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
            hr = hr->next;
        }
    }
    return false;
}
static bool constCopyConstructor(SYMBOL* sp)
{
    VBASEENTRY* e;
    BASECLASS* b;
    HASHREC* hr;
    b = sp->baseClasses;
    while (b)
    {
        if (!b->isvirtual && !hasConstFunc(b->cls, CI_CONSTRUCTOR, false))
            return false;
        b = b->next;
    }
    e = sp->vbaseEntries;
    while (e)
    {
        if (e->alloc && !hasConstFunc(e->cls, CI_CONSTRUCTOR, false))
            return false;
        e = e->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* cls = (SYMBOL*)hr->p;
        if (isstructured(cls->tp) && cls->storage_class != sc_typedef && !cls->trivialCons)
            if (!hasConstFunc(basetype(cls->tp)->sp, CI_CONSTRUCTOR, false))
                return false;
        hr = hr->next;
    }

    return true;
}
static SYMBOL* declareConstructor(SYMBOL* sp, bool deflt, bool move)
{
    SYMBOL *func, *sp1;
    TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
    tp->type = bt_func;
    tp->size = getSize(bt_pointer);
    tp->btp = (TYPE*)Alloc(sizeof(TYPE));
    tp->btp->type = bt_void;
    tp->rootType = tp;
    tp->btp->rootType = tp->btp;
    func = makeID(sc_member, tp, NULL, overloadNameTab[CI_CONSTRUCTOR]);
    func->isConstructor = true;
    func->attribs.inheritable.linkage2 = sp->attribs.inheritable.linkage2;
    sp1 = makeID(sc_parameter, NULL, NULL, AnonymousName());
    tp->syms = CreateHashTable(1);
    tp->syms->table[0] = (HASHREC*)Alloc(sizeof(HASHREC));
    tp->syms->table[0]->p = (struct sym *)sp1;
    sp1->tp = (TYPE*)Alloc(sizeof(TYPE));
    if (deflt)
    {
        sp1->tp->type = bt_void;
        sp1->tp->rootType = sp1->tp;
    }
    else
    {
        TYPE* tpx = sp1->tp;
        tpx->type = move ? bt_rref : bt_lref;
        tpx->size = getSize(bt_pointer);
        tpx->btp = (TYPE*)Alloc(sizeof(TYPE));
        tpx = tpx->btp;
        if (constCopyConstructor(sp))
        {
            tpx->type = bt_const;
            tpx->size = getSize(bt_pointer);
            tpx->btp = (TYPE*)Alloc(sizeof(TYPE));
            tpx = tpx->btp;
        }
        *tpx = *basetype(sp->tp);
    }
    UpdateRootTypes(sp1->tp);
    return insertFunc(sp, func);
}
static bool constAssignmentOp(SYMBOL* sp, bool move)
{
    VBASEENTRY* e;
    BASECLASS* b;
    HASHREC* hr;
    b = sp->baseClasses;
    while (b)
    {
        if (!b->isvirtual && !hasConstFunc(b->cls, assign - kw_new + CI_NEW, move))
            return false;
        b = b->next;
    }
    e = sp->vbaseEntries;
    while (e)
    {
        if (e->alloc && !hasConstFunc(e->cls, assign - kw_new + CI_NEW, move))
            return false;
        e = e->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* cls = (SYMBOL*)hr->p;
        if (isstructured(cls->tp) && cls->storage_class != sc_typedef && !cls->trivialCons)
            if (!hasConstFunc(cls, assign - kw_new + CI_NEW, move))
                return false;
        hr = hr->next;
    }

    return true;
}
static SYMBOL* declareAssignmentOp(SYMBOL* sp, bool move)
{
    SYMBOL *func, *sp1;
    TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
    TYPE* tpx;
    tp->type = bt_func;
    tp->size = getSize(bt_pointer);
    tp->btp = (TYPE*)Alloc(sizeof(TYPE));
    tpx = tp->btp = (TYPE*)Alloc(sizeof(TYPE));
    if (isstructured(sp->tp))
    {
        tpx->type = move ? bt_rref : bt_lref;
        tpx->size = getSize(bt_pointer);
        tpx = tpx->btp = (TYPE*)Alloc(sizeof(TYPE));
    }
    *(tpx) = *basetype(sp->tp);
    UpdateRootTypes(tp);
    func = makeID(sc_member, tp, NULL, overloadNameTab[assign - kw_new + CI_NEW]);
    func->attribs.inheritable.linkage2 = sp->attribs.inheritable.linkage2;
    sp1 = makeID(sc_parameter, NULL, NULL, AnonymousName());
    tp->syms = CreateHashTable(1);
    tp->syms->table[0] = (HASHREC*)Alloc(sizeof(HASHREC));
    tp->syms->table[0]->p = (struct sym *)sp1;
    sp1->tp = (TYPE*)Alloc(sizeof(TYPE));
    tpx = sp1->tp;
    tpx->type = move ? bt_rref : bt_lref;
    tpx->btp = (TYPE*)Alloc(sizeof(TYPE));
    tpx = tpx->btp;
    if (constAssignmentOp(sp, move))
    {
        tpx->type = bt_const;
        tpx->size = getSize(bt_pointer);
        tpx->btp = (TYPE*)Alloc(sizeof(TYPE));
        tpx = tpx->btp;
    }
    *tpx = *basetype(sp->tp);
    UpdateRootTypes(sp1->tp);
    return insertFunc(sp, func);
}
static bool matchesDefaultConstructor(SYMBOL* sp)
{
    HASHREC* hr = basetype(sp->tp)->syms->table[0]->next;
    if (hr)
    {
        SYMBOL* arg1 = (SYMBOL*)hr->p;
        if (arg1->tp->type == bt_void || arg1->init)
            return true;
    }
    return false;
}
bool matchesCopy(SYMBOL* sp, bool move)
{
    HASHREC* hr = basetype(sp->tp)->syms->table[0]->next;
    if (hr)
    {
        SYMBOL* arg1 = (SYMBOL*)hr->p;
        if (!hr->next || ((SYMBOL*)hr->next->p)->init || ((SYMBOL*)hr->next->p)->constop)
        {
            if (basetype(arg1->tp)->type == (move ? bt_rref : bt_lref))
            {
                TYPE* tp = basetype(arg1->tp)->btp;
                if (isstructured(tp))
                    if (basetype(tp)->sp == sp->parentClass || basetype(tp)->sp == sp->parentClass->mainsym ||
                        basetype(tp)->sp->mainsym == sp->parentClass || sameTemplate(tp, sp->parentClass->tp))
                        return true;
            }
        }
    }
    return false;
}
static bool hasCopy(SYMBOL* func, bool move)
{
    HASHREC* hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (!sp->internallyGenned && matchesCopy(sp, move))
            return true;
        hr = hr->next;
    }
    return false;
}
static bool checkDest(SYMBOL* sp, HASHTABLE* syms, enum e_ac access)
{
    SYMBOL* dest = search(overloadNameTab[CI_DESTRUCTOR], syms);

    if (dest)
    {
        dest = (SYMBOL*)basetype(dest->tp)->syms->table[0]->p;
        if (dest->deleted)
            return true;
        if (!isAccessible(sp, dest->parentClass, dest, NULL, access, false))
            return true;
    }
    return false;
}
static bool checkDefaultCons(SYMBOL* sp, HASHTABLE* syms, enum e_ac access)
{
    SYMBOL* cons = search(overloadNameTab[CI_CONSTRUCTOR], syms);
    if (cons)
    {
        SYMBOL* dflt = NULL;
        HASHREC* hr = basetype(cons->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* cur = (SYMBOL*)hr->p;
            if (matchesDefaultConstructor(cur))
            {
                if (dflt)
                    return true;  // ambiguity
                dflt = cur;
            }
            hr = hr->next;
        }
        if (dflt)
        {
            if (dflt->deleted)
                return true;
            if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, false))
                return true;
        }
    }
    return false;
}
SYMBOL* getCopyCons(SYMBOL* base, bool move)
{
    (void)move;
    SYMBOL* ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(base->tp)->syms);
    if (ovl)
    {
        HASHREC* hr = basetype(ovl->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL*)hr->p, *sym1 = NULL;
            HASHREC* hrArgs = basetype(sym->tp)->syms->table[0];
            sym = (SYMBOL*)hrArgs->p;
            if (sym->thisPtr)
            {
                hrArgs = hrArgs->next;
                if (hrArgs)
                    sym = (SYMBOL*)hrArgs->p;
            }
            if (hrArgs && hrArgs->next)
            {
                sym1 = (SYMBOL*)hrArgs->next->p;
            }
            if (hrArgs && (!sym1 || sym1->init))
            {
                TYPE* tp = basetype(sym->tp);
                if (tp->type == (move ? bt_rref : bt_lref))
                {
                    tp = basetype(tp->btp);
                    if (isstructured(tp))
                    {
                        if (!base->tp->sp)
                        {
                            hr = hr->next;
                            continue;
                        }
                        if (tp->sp == base->tp->sp || tp->sp == base->tp->sp->mainsym || sameTemplate(tp, base->tp))
                        {
                            return (SYMBOL*)hr->p;
                        }
                    }
                }
            }
            hr = hr->next;
        }
    }
    return NULL;
}
static SYMBOL* GetCopyAssign(SYMBOL* base, bool move)
{
    (void)move;
    SYMBOL* ovl = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(base->tp)->syms);
    if (ovl)
    {
        HASHREC* hr = basetype(ovl->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL*)hr->p, *sym1 = NULL;
            HASHREC* hrArgs = basetype(sym->tp)->syms->table[0];
            sym = (SYMBOL*)hrArgs->p;
            if (sym->thisPtr)
            {
                hrArgs = hrArgs->next;
                if (hrArgs)
                    sym = (SYMBOL*)hrArgs->p;
            }
            if (hrArgs && hrArgs->next)
            {
                sym1 = (SYMBOL*)hrArgs->next->p;
            }
            if (hrArgs && (!sym1 || sym1->init))
            {
                TYPE* tp = basetype(sym->tp);
                if (tp->type == (move ? bt_rref : bt_lref))
                {
                    tp = basetype(tp->btp);
                    if (isstructured(tp))
                    {
                        if (comparetypes(tp, base->tp, true) || sameTemplate(tp, base->tp))
                        {
                            return (SYMBOL*)hr->p;
                        }
                    }
                }
            }
            hr = hr->next;
        }
    }
    return NULL;
}
bool hasVTab(SYMBOL* sp)
{
    VTABENTRY* vt = sp->vtabEntries;
    while (vt)
    {
        if (vt->virtuals)
            return true;
        vt = vt->next;
    }
    return false;
}
static bool hasTrivialCopy(SYMBOL* sp, bool move)
{
    HASHREC* hr;
    SYMBOL* dflt;
    BASECLASS* base;
    if (sp->vbaseEntries || hasVTab(sp))
        return false;
    base = sp->baseClasses;
    while (base)
    {
        dflt = getCopyCons(base->cls, move);
        if (!dflt)
            return false;
        if (!dflt->trivialCons)
            return false;
        base = base->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* cls = (SYMBOL*)hr->p;
        if (isstructured(cls->tp))
        {
            dflt = getCopyCons(basetype(cls->tp)->sp, move);
            if (!dflt)
                return false;
            if (!dflt->trivialCons)
                return false;
        }
        hr = hr->next;
    }
    return true;
}
static bool hasTrivialAssign(SYMBOL* sp, bool move)
{
    HASHREC* hr;
    SYMBOL* dflt;
    BASECLASS* base;
    if (sp->vbaseEntries || hasVTab(sp))
        return false;
    base = sp->baseClasses;
    while (base)
    {
        dflt = GetCopyAssign(base->cls, move);
        if (!dflt)
            return false;
        if (!dflt->trivialCons)
            return false;
        base = base->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* cls = (SYMBOL*)hr->p;
        if (isstructured(cls->tp))
        {
            dflt = getCopyCons(basetype(cls->tp)->sp, move);
            if (!dflt)
                return false;
            if (!dflt->trivialCons)
                return false;
        }
        hr = hr->next;
    }
    return true;
}
static bool checkCopyCons(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = getCopyCons(base, false);
    if (dflt)
    {
        if (dflt->deleted)
            return true;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, false))
            return true;
    }
    return false;
}
static bool checkCopyAssign(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = GetCopyAssign(base, false);
    if (dflt)
    {
        if (dflt->deleted)
            return true;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, false))
            return true;
    }
    return false;
}
static bool checkMoveCons(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = getCopyCons(base, true);
    if (dflt)
    {
        if (dflt->deleted)
            return true;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, false))
            return true;
    }
    return false;
}
static bool checkMoveAssign(SYMBOL* sp, SYMBOL* base, enum e_ac access)
{
    SYMBOL* dflt = GetCopyAssign(base, true);
    if (dflt)
    {
        if (dflt->deleted)
            return true;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, false))
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
    HASHREC* hr;
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        bool allconst = true;
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = (SYMBOL*)hr->p;
            if (!isconst(sp->tp) && sp->tp->type != bt_aggregate)
                allconst = false;
            if (isstructured(sp->tp))
            {
                SYMBOL* cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
                HASHREC* hr1 = basetype(cons->tp)->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL*)hr1->p;
                    if (matchesDefaultConstructor(cons))
                        if (!cons->trivialCons)
                            return true;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
        if (allconst)
            return true;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp1 = (SYMBOL*)hr->p;
        TYPE* m;
        if (sp1->storage_class == sc_member || sp1->storage_class == sc_mutable)
        {
            if (isref(sp1->tp))
                if (!sp1->init)
                    return true;
            if (basetype(sp1->tp)->type == bt_union)
            {
                HASHREC* hr1 = basetype(sp1->tp)->syms->table[0];
                while (hr1)
                {
                    SYMBOL* member = (SYMBOL*)hr1->p;
                    if (!isconst(member->tp) && basetype(member->tp)->type != bt_aggregate)
                    {
                        break;
                    }
                    hr1 = hr1->next;
                }
                if (!hr1)
                    return true;
            }
            if (isstructured(sp1->tp))
            {
                TYPE* tp = basetype(sp1->tp);
                if (checkDest(sp, basetype(tp->sp->tp)->syms, ac_public))
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
        hr = hr->next;
    }

    base = sp->baseClasses;
    while (base)
    {
        if (checkDest(sp, basetype(base->cls->tp)->syms, ac_protected))
            return true;
        if (checkDefaultCons(sp, basetype(base->cls->tp)->syms, ac_protected))
            return true;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc)
        {
            if (checkDest(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return true;
            if (checkDefaultCons(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return true;
        }
        vbase = vbase->next;
    }
    return false;
}
static bool isCopyConstructorDeleted(SYMBOL* sp)
{
    HASHREC* hr;
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = (SYMBOL*)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL* cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
                HASHREC* hr1 = basetype(cons->tp)->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL*)hr1->p;
                    if (matchesCopy(cons, false))
                        if (!cons->trivialCons)
                            return true;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp1 = (SYMBOL*)hr->p;
        TYPE* m;
        if (sp1->storage_class == sc_member || sp1->storage_class == sc_mutable)
        {
            if (basetype(sp1->tp)->type == bt_rref)
                return true;
            if (isstructured(sp1->tp))
            {
                TYPE* tp = basetype(sp1->tp);
                if (checkDest(sp, basetype(tp->sp->tp)->syms, ac_public))
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
        hr = hr->next;
    }

    base = sp->baseClasses;
    while (base)
    {
        if (checkDest(sp, basetype(base->cls->tp)->syms, ac_protected))
            return true;
        if (checkCopyCons(sp, base->cls, ac_protected))
            return true;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc)
        {
            if (checkDest(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return true;
            if (checkCopyCons(sp, vbase->cls, ac_protected))
                return true;
        }
        vbase = vbase->next;
    }
    return false;
}
static bool isCopyAssignmentDeleted(SYMBOL* sp)
{
    HASHREC* hr;
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = (SYMBOL*)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL* cons = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
                HASHREC* hr1 = basetype(cons->tp)->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL*)hr1->p;
                    if (matchesCopy(cons, false))
                        if (!cons->trivialCons)
                            return true;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp1 = (SYMBOL*)hr->p;
        TYPE* m;
        if (sp1->storage_class == sc_member || sp1->storage_class == sc_mutable)
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
                if (checkCopyAssign(sp, basetype(m)->sp, ac_public))
                    return true;
            }
        }
        hr = hr->next;
    }

    base = sp->baseClasses;
    while (base)
    {
        if (checkCopyAssign(sp, base->cls, ac_protected))
            return true;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc && checkCopyAssign(sp, vbase->cls, ac_protected))
            return true;
        vbase = vbase->next;
    }
    return false;
}
static bool isMoveConstructorDeleted(SYMBOL* sp)
{
    HASHREC* hr;
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = (SYMBOL*)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL* cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
                HASHREC* hr1 = basetype(cons->tp)->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL*)hr1->p;
                    if (matchesCopy(cons, true))
                        if (!cons->trivialCons)
                            return true;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp1 = (SYMBOL*)hr->p;
        TYPE* m;
        if (sp1->storage_class == sc_member || sp1->storage_class == sc_mutable)
        {
            if (basetype(sp1->tp)->type == bt_rref)
                return true;
            if (isstructured(sp1->tp))
            {
                TYPE* tp = basetype(sp1->tp);
                if (checkDest(sp, basetype(tp->sp->tp)->syms, ac_public))
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
        hr = hr->next;
    }

    base = sp->baseClasses;
    while (base)
    {
        if (checkDest(sp, basetype(base->cls->tp)->syms, ac_protected))
            return true;
        if (checkMoveCons(sp, base->cls, ac_protected))
            return true;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc)
        {
            if (checkDest(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return true;
            if (checkMoveCons(sp, vbase->cls, ac_protected))
                return true;
        }
        vbase = vbase->next;
    }
    return false;
}
static bool isMoveAssignmentDeleted(SYMBOL* sp)
{
    HASHREC* hr;
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = (SYMBOL*)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL* cons = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
                HASHREC* hr1 = basetype(cons->tp)->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL*)hr1->p;
                    if (matchesCopy(cons, true))
                        if (!cons->trivialCons)
                            return true;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp1 = (SYMBOL*)hr->p;
        TYPE* m;
        if (sp1->storage_class == sc_member || sp1->storage_class == sc_mutable)
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
        hr = hr->next;
    }

    base = sp->baseClasses;
    while (base)
    {
        if (checkMoveAssign(sp, base->cls, ac_protected))
            return true;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc && checkMoveAssign(sp, vbase->cls, ac_protected))
            return true;
        vbase = vbase->next;
    }
    return false;
}
static void conditionallyDeleteDefaultConstructor(SYMBOL* func)
{
    HASHREC* hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (sp->defaulted && matchesDefaultConstructor(sp))
        {
            if (isDefaultDeleted(sp->parentClass))
            {
                sp->deleted = true;
            }
        }
        hr = hr->next;
    }
}
static bool conditionallyDeleteCopyConstructor(SYMBOL* func, bool move)
{
    HASHREC* hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (sp->defaulted && matchesCopy(sp, move))
        {
            if (isCopyConstructorDeleted(sp->parentClass))
                sp->deleted = true;
        }
        hr = hr->next;
    }
    return false;
}
static bool conditionallyDeleteCopyAssignment(SYMBOL* func, bool move)
{
    HASHREC* hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (sp->defaulted && matchesCopy(sp, move))
        {
            if (isCopyAssignmentDeleted(sp->parentClass))
                sp->deleted = true;
        }
        hr = hr->next;
    }
    return false;
}
void createConstructorsForLambda(SYMBOL* sp)
{
    SYMBOL* newcons;
    declareDestructor(sp);
    newcons = declareConstructor(sp, true, false);  // default
    newcons->deleted = true;
    newcons = declareConstructor(sp, false, false);  // copy
    conditionallyDeleteCopyConstructor(newcons, false);
    newcons = declareAssignmentOp(sp, false);
    newcons->deleted = true;
    if (!isMoveConstructorDeleted(sp))
    {
        newcons = declareConstructor(sp, false, true);
        //        newcons->deleted = isMoveAssignmentDeleted(sp);
    }
}
// if there is a constructor with defaulted args that would match the
// default constructor,   make a shim default constructor that wraps it...
//
static void shimDefaultConstructor(SYMBOL* sp, SYMBOL* cons)
{
    SYMBOL* match = NULL;
    HASHREC* hr = basetype(cons->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sym = (SYMBOL*)hr->p;
        if (matchesDefaultConstructor(sym))
        {
            if (match)
                return;  // will get an error elsewhere because of the duplicate
            match = sym;
        }
        hr = hr->next;
    }
    if (match)
    {
        hr = basetype(match->tp)->syms->table[0];
        if (hr->next && ((SYMBOL*)hr->next->p)->init)
        {
            // will match a default constructor but has defaulted args
            SYMBOL* consfunc = declareConstructor(sp, true, false);  // default
            HASHTABLE* syms;
            BLOCKDATA b;
            STATEMENT* st;
            EXPRESSION* thisptr = varNode(en_auto, (SYMBOL*)hr->p);
            EXPRESSION* e1;
            FUNCTIONCALL* params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
            memset(&b, 0, sizeof(BLOCKDATA));
            ((SYMBOL*)hr->p)->offset = chosenAssembler->arch->retblocksize;
            deref(&stdpointer, &thisptr);
            b.type = begin;
            syms = localNameSpace->syms;
            localNameSpace->syms = basetype(consfunc->tp)->syms;
            params->thisptr = thisptr;
            params->thistp = (TYPE *)Alloc(sizeof(TYPE));
            params->thistp->type = bt_pointer;
            params->thistp->btp = sp->tp;
            params->thistp->rootType = params->thistp;
            params->thistp->size = getSize(bt_pointer);
            params->fcall = varNode(en_pc, match);
            params->functp = match->tp;
            params->sp = match;
            params->ascall = true;
            AdjustParams(match, basetype(match->tp)->syms->table[0], &params->arguments, false, true);
            if (sp->vbaseEntries)
            {
                INITLIST *x = (INITLIST*)Alloc(sizeof(INITLIST)), **p;
                x->tp = (TYPE*)Alloc(sizeof(TYPE));
                x->tp->type = bt_int;
                x->tp->rootType = x->tp;
                x->tp->size = getSize(bt_int);
                x->exp = intNode(en_c_i, 1);
                p = &params->arguments;
                while (*p)
                    p = &(*p)->next;
                *p = x;
            }
            e1 = varNode(en_func, NULL);
            e1->v.func = params;
            if (e1)  // could probably remove this, only null if ran out of memory.
            {
                e1 = exprNode(en_thisref, e1, NULL);
                e1->v.t.thisptr = params->thisptr;
                e1->v.t.tp = sp->tp;
                // hasXCInfo = true;
            }
            st = stmtNode(NULL, &b, st_return);
            st->select = e1;
            consfunc->xcMode = cons->xcMode;
            consfunc->xc->xcDynamic = cons->xc->xcDynamic;
            consfunc->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
            consfunc->inlineFunc.stmt->lower = b.head;
            consfunc->inlineFunc.syms = basetype(consfunc->tp)->syms;
            consfunc->retcount = 1;
            consfunc->isInline = consfunc->attribs.inheritable.linkage2 != lk_export;
            InsertInline(consfunc);
            // now get rid of the first default arg
            // leave others so the old constructor can be considered
            // under other circumstances
            hr = hr->next;
            ((SYMBOL*)hr->p)->init = NULL;
            if (match->deferredCompile && !match->inlineFunc.stmt)
                deferredCompileOne(match);
            localNameSpace->syms = syms;
        }
    }
}
void createDefaultConstructors(SYMBOL* sp)
{
    SYMBOL* cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
    SYMBOL* dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    SYMBOL* asgn = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
    SYMBOL* newcons = NULL;
    if (!dest)
        declareDestructor(sp);
    else
        sp->hasDest = true;
    if (cons)
    {
        sp->hasUserCons = true;
        shimDefaultConstructor(sp, cons);
    }
    else
    {
        // create the default constructor
        newcons = declareConstructor(sp, true, false);
        cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
        conditionallyDeleteDefaultConstructor(cons);
    }
    // see if the default constructor could be trivial
    if (!hasVTab(sp) && sp->vbaseEntries == NULL && !dest)
    {
        BASECLASS* base = sp->baseClasses;
        while (base)
        {
            if (!base->cls->trivialCons || base->accessLevel != ac_public)
                break;
            base = base->next;
        }
        if (!base)
        {
            HASHREC* p = basetype(sp->tp)->syms->table[0];
            while (p)
            {
                SYMBOL* pcls = (SYMBOL*)p->p;
                if (pcls->storage_class == sc_member || pcls->storage_class == sc_mutable || pcls->storage_class == sc_overloads)
                {
                    if (isstructured(pcls->tp))
                    {
                        if (!basetype(pcls->tp)->sp->trivialCons)
                            break;
                    }
                    else if (pcls->storage_class == sc_overloads)
                    {
                        bool err = false;
                        HASHREC* p = basetype(pcls->tp)->syms->table[0];
                        while (p && !err)
                        {
                            SYMBOL* s = (SYMBOL*)p->p;
                            if (s->storage_class != sc_static)
                            {
                                err |= s->isConstructor && !s->defaulted && !s->deleted;
                                err |= s->access != ac_public;
                            }
                            p = p->next;
                        }
                        if (err)
                            break;
                    }
                    else if (pcls->access != ac_public)
                        break;
                }
                p = p->next;
            }
            if (!p)
            {
                sp->trivialCons = true;
            }
        }
    }
    if (newcons)
        newcons->trivialCons = true;
    // now if there is no copy constructor or assignment operator declare them
    if (!hasCopy(cons, false))
    {
        SYMBOL* newcons = declareConstructor(sp, false, false);
        newcons->trivialCons = hasTrivialCopy(sp, false);
        if (hasCopy(cons, true) || (asgn && hasCopy(asgn, true)))
            newcons->deleted = true;
        if (!asgn)
            asgn = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
        conditionallyDeleteCopyConstructor(cons, false);
    }
    if (!asgn || !hasCopy(asgn, false))
    {
        SYMBOL* newsp = declareAssignmentOp(sp, false);
        newsp->trivialCons = hasTrivialAssign(sp, false);
        if (hasCopy(cons, true) || (asgn && hasCopy(asgn, true)))
            newsp->deleted = true;
        if (!asgn)
            asgn = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
        conditionallyDeleteCopyAssignment(asgn, false);
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
            newcons->trivialCons = hasTrivialCopy(sp, true);
        }
        newcons = declareAssignmentOp(sp, true);
        newcons->trivialCons = hasTrivialAssign(sp, true);
        newcons->deleted = isMoveAssignmentDeleted(sp);
    }
    else
    {
        conditionallyDeleteCopyConstructor(cons, true);
        conditionallyDeleteCopyAssignment(asgn, true);
    }
}
void destructBlock(EXPRESSION** exp, HASHREC* hr, bool mainDestruct)
{
    //    if (!cparams.prm_cplusplus)
    //        return;
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (sp->allocate && !sp->destructed && !isref(sp->tp))
        {
            sp->destructed = mainDestruct;
            if (sp->storage_class == sc_parameter)
            {
                if (isstructured(sp->tp))
                {
                    EXPRESSION* iexp = getThisNode(sp);
                    //                    iexp = exprNode(en_add, iexp, intNode(en_c_i, chosenAssembler->arch->retblocksize));
                    callDestructor(basetype(sp->tp)->sp, NULL, &iexp, NULL, true, false, false);
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
            else if (sp->storage_class != sc_localstatic && sp->dest)
            {

                EXPRESSION* iexp = sp->dest->exp;
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
        hr = hr->next;
    }
}
static void genConsData(BLOCKDATA* b, SYMBOL* cls, MEMBERINITIALIZERS* mi, SYMBOL* member, int offset, EXPRESSION* thisptr,
                        EXPRESSION* otherptr, SYMBOL* parentCons, bool doCopy)
{
    (void)cls;
    (void)mi;
    if (doCopy && (matchesCopy(parentCons, false) || matchesCopy(parentCons, true)))
    {
        thisptr = exprNode(en_add, thisptr, intNode(en_c_i, offset));
        otherptr = exprNode(en_add, otherptr, intNode(en_c_i, offset));
        if (isstructured(member->tp) || isarray(member->tp) || basetype(member->tp)->type == bt_memberptr)
        {
            EXPRESSION* exp = exprNode(en_blockassign, thisptr, otherptr);
            STATEMENT* st = stmtNode(NULL, b, st_expr);
            exp->size = member->tp->size;
            exp->altdata = (void*)member->tp;
            optimize_for_constants(&exp);
            st->select = exp;
        }
        else
        {
            STATEMENT* st = stmtNode(NULL, b, st_expr);
            EXPRESSION* exp;
            deref(member->tp, &thisptr);
            deref(member->tp, &otherptr);
            exp = exprNode(en_assign, thisptr, otherptr);
            optimize_for_constants(&exp);
            st->select = exp;
        }
    }
    else if (member->init)
    {
        EXPRESSION* exp;
        STATEMENT* st = stmtNode(NULL, b, st_expr);
        exp = convertInitToExpression(member->tp, member, NULL, member->init, thisptr, false);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void genConstructorCall(BLOCKDATA* b, SYMBOL* cls, MEMBERINITIALIZERS* mi, SYMBOL* member, int memberOffs, bool top,
                               EXPRESSION* thisptr, EXPRESSION* otherptr, SYMBOL* parentCons, bool baseClass, bool doCopy, bool useDefault)
{
    STATEMENT* st = NULL;
    if (cls != member && member->init)
    {
        EXPRESSION* exp;
        if (member->init->exp)
        {
            exp = convertInitToExpression(member->tp, member, NULL, member->init, thisptr, false);
        }
        else
        {
            exp = exprNode(en_add, thisptr, intNode(en_c_i, member->offset));
            exp = exprNode(en_blockclear, exp, 0);
            exp->size = member->tp->size;
        }
        st = stmtNode(NULL, b, st_expr);
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
                if (!callConstructor(&ctype, &exp, NULL, false, NULL, top, false, false, false, false, false))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
            else
            {
                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                EXPRESSION* other = exprNode(en_add, otherptr, intNode(en_c_i, memberOffs));
                if (basetype(parentCons->tp)->type == bt_rref)
                    other = exprNode(en_not_lvalue, other, NULL);
                if (isconst(((SYMBOL*)basetype(parentCons->tp)->syms->table[0]->next->p)->tp->btp))
                {
                    tp->type = bt_const;
                    tp->size = basetype(member->tp)->size;
    //                tp->btp = member->tp;
                    tp->btp = (TYPE *)Alloc(sizeof(TYPE));
                    *tp->btp = *member->tp;
    //                tp->rootType = member->tp->rootType;
                    UpdateRootTypes(tp);
                    tp->btp->lref = true;
                    tp->btp->rref = false;
                }
                else
                {
                    tp = (TYPE *) Alloc(sizeof(TYPE));
                    *tp = *member->tp;
                    tp->lref = true;
                    tp->rref = false;
    //                tp = member->tp;
                }
                //			member->tp->lref = true;
                if (!callConstructorParam(&ctype, &exp, tp, other, top, false, false, false))
                    errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
                //			member->tp->lref = false;
            }
        }
        else if (doCopy && matchesCopy(parentCons, true))
        {
            if (useDefault)
            {
                if (!callConstructor(&ctype, &exp, NULL, false, NULL, top, false, false, false, false, false))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
            else
            {
                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                EXPRESSION* other = exprNode(en_add, otherptr, intNode(en_c_i, memberOffs));
                if (basetype(parentCons->tp)->type == bt_rref)
                    other = exprNode(en_not_lvalue, other, NULL);
                if (isconst(((SYMBOL*)basetype(parentCons->tp)->syms->table[0]->next->p)->tp->btp))
                {
                    tp->type = bt_const;
                    tp->size = basetype(member->tp)->size;
    //                tp->btp = member->tp;
                    tp->btp = (TYPE *)Alloc(sizeof(TYPE));
                    *tp->btp = *member->tp;
    //                tp->rootType = member->tp->rootType;
                    UpdateRootTypes(tp);
                    tp->btp->rref = true;
                    tp->btp->lref = false;
                }
                else
                {
                    tp = (TYPE *)Alloc(sizeof(TYPE));
                    *tp = *member->tp;
                    tp->rref = true;
                    tp->lref = false;
    //                tp = member->tp;
                }
                //			member->tp->rref = true;
                if (!callConstructorParam(&ctype, &exp, tp, other, top, false, false, false))
                    errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
                //			member->tp->rref = false;
            }
        }
        else
        {
            if (mi && mi->sp && baseClass)
            {
                while (mi)
                {
                    if (mi->sp && isstructured(mi->sp->tp) &&
                        (basetype(mi->sp->tp)->sp == member || sameTemplate(mi->sp->tp, member->tp)))
                    {
                        break;
                    }
                    mi = mi->next;
                }
            }
            else
            {
                mi = NULL;
            }
            if (mi)
            {
                INITIALIZER* init = mi->init;
                FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                INITLIST** args = &funcparams->arguments;
                while (init && init->exp)
                {
                    *args = (INITLIST*)Alloc(sizeof(INITLIST));
                    (*args)->tp = init->basetp;
                    (*args)->exp = init->exp;
                    args = &(*args)->next;
                    init = init->next;
                }
                if (!callConstructor(&ctype, &exp, funcparams, false, NULL, top, false, false, false, false, false))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
                PromoteConstructorArgs(funcparams->sp, funcparams);
            }
            else
            {
                if (!callConstructor(&ctype, &exp, NULL, false, NULL, top, false, false, false, false, false))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
            matchesCopy(parentCons, false);
        }
        st = stmtNode(NULL, b, st_expr);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void virtualBaseThunks(BLOCKDATA* b, SYMBOL* sp, EXPRESSION* thisptr)
{
    VBASEENTRY* entries = sp->vbaseEntries;
    EXPRESSION *first = NULL, **pos = &first;
    STATEMENT* st;
    while (entries)
    {
        EXPRESSION* left = exprNode(en_add, thisptr, intNode(en_c_i, entries->pointerOffset));
        EXPRESSION* right = exprNode(en_add, thisptr, intNode(en_c_i, entries->structOffset));
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
        entries = entries->next;
    }
    if (first)
    {
        st = stmtNode(NULL, b, st_expr);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void dovtabThunks(BLOCKDATA* b, SYMBOL* sym, EXPRESSION* thisptr, bool isvirtual)
{
    VTABENTRY* entries = sym->vtabEntries;
    EXPRESSION *first = NULL, **pos = &first;
    STATEMENT* st;
    SYMBOL* localsp;
    localsp = sym->vtabsp;
    EXPRESSION* vtabBase = varNode(en_global, localsp);
    if (localsp->attribs.inheritable.linkage2 == lk_import)
        deref(&stdpointer, &vtabBase);
    while (entries)
    {
        if (!entries->isdead && entries->isvirtual == isvirtual && hasVTab(entries->cls))
        {
            EXPRESSION* left = exprNode(en_add, thisptr, intNode(en_c_i, entries->dataOffset));
            EXPRESSION* right =
                exprNode(en_add, exprNode(en_add, vtabBase, intNode(en_c_i, entries->vtabOffset)), intNode(en_c_i, VTAB_XT_OFFS));
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
        entries = entries->next;
    }
    if (first)
    {
        st = stmtNode(NULL, b, st_expr);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void doVirtualBases(BLOCKDATA* b, SYMBOL* sp, MEMBERINITIALIZERS* mi, VBASEENTRY* vbe, EXPRESSION* thisptr,
                           EXPRESSION* otherptr, SYMBOL* parentCons, bool doCopy)
{
    if (vbe)
    {
        doVirtualBases(b, sp, mi, vbe->next, thisptr, otherptr, parentCons, doCopy);
        if (vbe->alloc)
            genConstructorCall(b, sp, mi, vbe->cls, vbe->structOffset, false, thisptr, otherptr, parentCons, true, doCopy, false);
    }
}
static EXPRESSION* unshim(EXPRESSION* exp, EXPRESSION* ths);
static STATEMENT* unshimstmt(STATEMENT* block, EXPRESSION* ths)
{
    STATEMENT *rv = NULL, **last = &rv;
    while (block != NULL)
    {
        *last = (STATEMENT *)Alloc(sizeof(STATEMENT));
        **last = *block;
        block = *last;
        last = &(*last)->next;
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
        block = block->next;
    }
    return rv;
}

static EXPRESSION* unshim(EXPRESSION* exp, EXPRESSION* ths)
{
    EXPRESSION* nw = NULL;
    if (!exp)
        return exp;
    if (exp->type == en_thisshim)
        return ths;
    nw = (EXPRESSION *)Alloc(sizeof(EXPRESSION));
    *nw = *exp;
    nw->left = unshim(nw->left, ths);
    nw->right = unshim(nw->right, ths);
    if (nw->type == en_stmt)
    {
        nw->v.stmt = unshimstmt(nw->v.stmt, ths);
    }
    return nw;
}
SYMBOL* findClassName(const char* name, SYMBOL* cls, BASECLASS* bc, VBASEENTRY* vbase, int* offset)
{
    int n = 0;
    char str[1024];
    char* clslst[100];
    char *p = str, *c;
    SYMBOL* sp = NULL;
    int vcount = 0, ccount = 0;
    strcpy(str, name);
    while ((c = strstr(p, "::")))
    {
        clslst[n++] = p;
        p = c;
        *p = 0;
        p += 2;
    }
    clslst[n++] = p;

    while (bc)
    {
        if (!bc->isvirtual)
        {
            SYMBOL* parent = bc->cls;
            int i;
            if (parent->tp->type == bt_typedef)
                parent = basetype(parent->tp)->sp;
            for (i = n - 1; i >= 0 && parent; i--, parent = parent->parentClass ? parent->parentClass : parent->parentNameSpace)
                if (strcmp(parent->name, clslst[i]))
                    break;
            if (i < 0 || (i == 0 && parent == NULL && clslst[0][0] == '\0'))
            {
                ccount++;
                sp = bc->cls;
                *offset = bc->offset;
            }
        }
        bc = bc->next;
    }
    while (vbase)
    {
        SYMBOL* parent = vbase->cls;
        int i;
        if (parent->tp->type == bt_typedef)
            parent = basetype(parent->tp)->sp;
        for (i = n - 1; i >= 0 && parent; i--, parent = parent->parentClass ? parent->parentClass : parent->parentNameSpace)
            if (strcmp(parent->name, clslst[i]))
                break;
        if (i < 0 || (i == 0 && parent == NULL && clslst[0][0] == '\0'))
        {
            vcount++;
            sp = vbase->cls;
            *offset = vbase->structOffset;
        }
        vbase = vbase->next;
    }
    if ((ccount && vcount) || ccount > 1)
    {
        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, sp, cls);
    }
    return sp;
}
void ParseMemberInitializers(SYMBOL* cls, SYMBOL* cons)
{
    MEMBERINITIALIZERS* init = cons->memberInitializers;
    bool first = true;
    bool hasDelegate = false;
    while (init)
    {
        LEXEME* lex;
        BASECLASS* bc = cls->baseClasses;
        VBASEENTRY* vbase = cls->vbaseEntries;
        if (!first && hasDelegate)
            error(ERR_DELEGATING_CONSTRUCTOR_ONLY_INITIALIZER);
        init->sp = search(init->name, basetype(cls->tp)->syms);
        if (init->sp && (!init->basesym || !istype(init->sp)))
        {
            if (init->sp->storage_class == sc_typedef)
            {
                TYPE* tp = basetype(init->sp->tp);
                if (isstructured(tp))
                {
                    init->name = basetype(tp)->sp->name;
                    init->sp = search(init->name, basetype(cls->tp)->syms);
                }
            }
        }
        if (init->sp && (!init->basesym || !istype(init->sp)))
        {
            if (init->sp != cls && init->sp->storage_class != sc_member && init->sp->storage_class != sc_mutable)
            {
                errorsym(ERR_NEED_NONSTATIC_MEMBER, init->sp);
            }
            else
            {
                bool done = false;
                lex = SetAlternateLex(init->initData);
                if (MATCHKW(lex, lt))
                {
                    TEMPLATEPARAMLIST* lst = NULL;
                    lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                    if (init->sp->templateLevel)
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
                    if (MATCHKW(lex, openpa))
                    {
                        if (MATCHKW(lex, openpa))
                        {
                            lex = getsym();
                            if (MATCHKW(lex, closepa))
                            {
                                lex = getsym();
                                init->init = NULL;
                                initInsert(&init->init, init->sp->tp, intNode(en_c_i, 0), 0 /*init->sp->offset*/, false);
                                done = true;
                            }
                            else
                            {
                                lex = backupsym();
                            }
                        }
                        if (!done)
                        {
                            needkw(&lex, openpa);
                            init->init = NULL;
                            argument_nesting++;
                            lex = initType(lex, cons, 0, sc_auto, &init->init, NULL, init->sp->tp, init->sp, false, 0);
                            argument_nesting--;
                            done = true;
                            needkw(&lex, closepa);
                        }
                    }
                }
                else
                {
                    if (MATCHKW(lex, openpa) && basetype(init->sp->tp)->sp->trivialCons)
                    {
                        init->init = NULL;
                        argument_nesting++;
                        lex = initType(lex, cons, 0, sc_auto, &init->init, NULL, init->sp->tp, init->sp, false, 0);
                        argument_nesting--;
                        done = true;
                        if (init->packed || MATCHKW(lex, ellipse))
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                    }
                    else
                    {
                        init->init = NULL;
                        lex = initType(lex, cons, 0, sc_auto, &init->init, NULL, init->sp->tp, init->sp, false, 0);
                        if (init->packed)
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                    }
                }
                SetAlternateLex(NULL);
            }
        }
        else
        {
            SYMBOL* sp = init->basesym;
            if (!sp)
                sp = classsearch(init->name, false, true);
            else
                init->sp = sp;
            if (sp && sp->tp->type == bt_templateparam)
            {
                if (sp->tp->templateParam->p->type == kw_typename)
                {
                    if (sp->tp->templateParam->p->packed)
                    {
                        MEMBERINITIALIZERS** p = &cons->memberInitializers;
                        FUNCTIONCALL shim;
                        lex = SetAlternateLex(init->initData);
                        shim.arguments = NULL;
                        getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                        if (!init->packed)
                            error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                        SetAlternateLex(NULL);
                        expandPackedMemberInitializers(cls, cons, sp->tp->templateParam->p->byPack.pack, p, init->initData,
                                                       shim.arguments);
                        init->sp = cls;
                    }
                    else if (sp->tp->templateParam->p->byClass.val && isstructured(sp->tp->templateParam->p->byClass.val))
                    {
                        TYPE* tp = sp->tp->templateParam->p->byClass.val;
                        int offset = 0;
                        int vcount = 0, ccount = 0;
                        init->name = basetype(tp)->sp->name;
                        while (bc)
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
                            bc = bc->next;
                        }
                        if ((ccount && vcount) || ccount > 1)
                        {
                            errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, init->sp, cls);
                        }
                        if (init->sp && init->sp == basetype(tp)->sp)
                        {
                            SYMBOL* sp = makeID(sc_member, init->sp->tp, NULL, init->sp->name);
                            FUNCTIONCALL shim;
                            INITIALIZER** xinit = &init->init;
                            sp->offset = offset;
                            init->sp = sp;
                            lex = SetAlternateLex(init->initData);
                            shim.arguments = NULL;
                            getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                            if (init->packed)
                                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                            SetAlternateLex(NULL);
                            while (shim.arguments)
                            {
                                *xinit = (INITIALIZER*)Alloc(sizeof(INITIALIZER));
                                (*xinit)->basetp = shim.arguments->tp;
                                (*xinit)->exp = shim.arguments->exp;
                                xinit = &(*xinit)->next;
                                shim.arguments = shim.arguments->next;
                            }
                        }
                        else
                        {
                            init->sp = NULL;
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
                init = expandPackedBaseClasses(cls, cons, init == cons->memberInitializers ? &cons->memberInitializers : &init, bc,
                                               vbase);
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
                }
                if (!init->sp)
                    init->sp = findClassName(init->name, cls, bc, vbase, &offset);
                if (init->sp)
                {
                    // have to make a *real* variable as a fudge...
                    SYMBOL* sp;
                    FUNCTIONCALL shim;
                    INITIALIZER** xinit;
                    lex = SetAlternateLex(init->initData);
                    if (MATCHKW(lex, lt))
                    {
                        TEMPLATEPARAMLIST* lst = NULL;
                        lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                        if (init->sp->templateLevel)
                        {
                            init->sp = TemplateClassInstantiate(init->sp, lst, false, sc_global);
                        }
                        else
                        {
                            errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                        }
                    }
                    sp = makeID(sc_member, init->sp->tp, NULL, init->sp->name);
                    xinit = &init->init;
                    sp->offset = offset;
                    init->sp = sp;
                    shim.arguments = NULL;
                    getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                    SetAlternateLex(NULL);
                    if (init->packed)
                    {
                        error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                    }
                    while (shim.arguments)
                    {
                        *xinit = (INITIALIZER*)Alloc(sizeof(INITIALIZER));
                        (*xinit)->basetp = shim.arguments->tp;
                        (*xinit)->exp = shim.arguments->exp;
                        xinit = &(*xinit)->next;
                        shim.arguments = shim.arguments->next;
                    }
                }
            }
        }
        if (!init->sp)
        {
            // might be a typedef?
            init->sp = finishSearch(init->name, NULL, NULL, false, false, false);
            if (init->sp && init->sp->storage_class == sc_typedef)
            {
                int offset = 0;
                TYPE* tp = init->sp->tp;
                tp = basetype(tp);
                if (isstructured(tp))
                {
                    BASECLASS* bc = cls->baseClasses;
                    while (bc)
                    {
                        if (!comparetypes(bc->cls->tp, init->sp->tp, true) || sameTemplate(bc->cls->tp, init->sp->tp))
                            break;
                        bc = bc->next;
                    }
                    if (bc)
                    {
                        // have to make a *real* variable as a fudge...
                        SYMBOL* sp;
                        FUNCTIONCALL shim;
                        INITIALIZER** xinit;
                        lex = SetAlternateLex(init->initData);
                        if (MATCHKW(lex, lt))
                        {
                            TEMPLATEPARAMLIST* lst = NULL;
                            lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                            if (init->sp->templateLevel)
                            {
                                init->sp = TemplateClassInstantiate(init->sp, lst, false, sc_global);
                            }
                            else
                            {
                                errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                            }
                        }
                        sp = makeID(sc_member, init->sp->tp, NULL, init->sp->name);
                        xinit = &init->init;
                        sp->offset = offset;
                        init->sp = sp;
                        shim.arguments = NULL;
                        getMemberInitializers(lex, cons, &shim, MATCHKW(lex, openpa) ? closepa : end, true);
                        if (init->packed)
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        SetAlternateLex(NULL);
                        while (shim.arguments)
                        {
                            *xinit = (INITIALIZER*)Alloc(sizeof(INITIALIZER));
                            (*xinit)->basetp = shim.arguments->tp;
                            (*xinit)->exp = shim.arguments->exp;
                            xinit = &(*xinit)->next;
                            shim.arguments = shim.arguments->next;
                        }
                    }
                }
                else
                {
                    init->sp = NULL;
                }
            }
            else
            {
                init->sp = NULL;
            }
        }
        if (!init->sp)
        {
            errorstrsym(ERR_NOT_A_MEMBER_OR_BASE_CLASS, init->name, cls);
        }
        first = false;
        init = init->next;
    }
}
static void allocInitializers(SYMBOL* cls, SYMBOL* cons, EXPRESSION* ths)
{
    HASHREC* hr = basetype(cls->tp)->syms->table[0];
    MEMBERINITIALIZERS* init = cons->memberInitializers;
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (sp->storage_class == sc_member || sp->storage_class == sc_mutable)
        {
            sp->lastInit = sp->init;
            if (sp->init)
            {
                sp->init = (INITIALIZER *)Alloc(sizeof(*sp->init));
                *sp->init = *sp->lastInit;
                sp->init->exp = unshim(sp->init->exp, ths);
            }
        }
        hr = hr->next;
    }
    while (init)
    {
        if (init->init)
        {
            init->sp->init = init->init;
            if (init->init->exp)
                init->init->exp = unshim(init->init->exp, ths);
        }
        init = init->next;
    }
    hr = basetype(cls->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (!sp->init && ismember(sp))
        {
            if (isref(sp->tp))
                errorsym(ERR_REF_MEMBER_MUST_INITIALIZE, sp);
            else if (isconst(sp->tp))
                errorsym(ERR_CONSTANT_MEMBER_MUST_BE_INITIALIZED, sp);
        }
        hr = hr->next;
    }
}
static void releaseInitializers(SYMBOL* cls, SYMBOL* cons)
{
    (void)cons;
    HASHREC* hr = basetype(cls->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (sp->storage_class == sc_member || sp->storage_class == sc_mutable)
            sp->init = sp->lastInit;
        hr = hr->next;
    }
}
EXPRESSION* thunkConstructorHead(BLOCKDATA* b, SYMBOL* sym, SYMBOL* cons, HASHTABLE* syms, bool parseInitializers,
                                 bool doCopy)
{
    BASECLASS* bc;
    HASHREC* hr = syms->table[0];
    EXPRESSION* thisptr = varNode(en_auto, (SYMBOL*)hr->p);
    EXPRESSION* otherptr = NULL;
    int oldCodeLabel = codeLabel;
    codeLabel = INT_MIN;
    if (hr->next)
        otherptr = varNode(en_auto, (SYMBOL*)hr->next->p);
    deref(&stdpointer, &thisptr);
    deref(&stdpointer, &otherptr);
    if (parseInitializers)
        allocInitializers(sym, cons, thisptr);
    if (cons->memberInitializers && cons->memberInitializers->delegating)
    {
        genConstructorCall(b, sym, cons->memberInitializers, sym, 0, false, thisptr, otherptr, cons, true, doCopy, !cons->defaulted);
    }
    else
    {
        if (sym->tp->type == bt_union)
        {
            AllocateLocalContext(NULL, cons, codeLabel++);
            hr = sym->tp->syms->table[0];
            while (hr)
            {
                SYMBOL* sp = (SYMBOL*)hr->p;
                if ((sp->storage_class == sc_member || sp->storage_class == sc_mutable) && sp->tp->type != bt_aggregate)
                {
                    if (sp->init)
                    {
                        if (isstructured(sp->tp))
                        {
                            genConstructorCall(b, basetype(sp->tp)->sp, cons->memberInitializers, sp, sp->offset, true, thisptr,
                                               otherptr, cons, false, doCopy, !cons->defaulted);
                        }
                        else
                        {
                            genConsData(b, sym, cons->memberInitializers, sp, sp->offset, thisptr, otherptr, cons, doCopy);
                        }
                    }
                }
                hr = hr->next;
            }
            FreeLocalContext(NULL, cons, codeLabel++);
        }
        else
        {
            if (sym->vbaseEntries)
            {
                SYMBOL* sp = makeID(sc_parameter, &stdint, NULL, "__$$constop");
                EXPRESSION* val = varNode(en_auto, sp);
                int lbl = codeLabel++;
                STATEMENT* st;
                sp->constop = true;
                sp->decoratedName = sp->errname = sp->name;
                sp->offset = chosenAssembler->arch->retblocksize + cons->paramsize;
                insert(sp, localNameSpace->syms);

                deref(&stdint, &val);
                st = stmtNode(NULL, b, st_notselect);
                optimize_for_constants(&val);
                st->select = val;
                st->label = lbl;
                virtualBaseThunks(b, sym, thisptr);
                doVirtualBases(b, sym, cons->memberInitializers, sym->vbaseEntries, thisptr, otherptr, cons, doCopy);
                if (hasVTab(sym))
                    dovtabThunks(b, sym, thisptr, true);
                st = stmtNode(NULL, b, st_label);
                st->label = lbl;
            }
            AllocateLocalContext(NULL, cons, codeLabel++);
            bc = sym->baseClasses;
            while (bc)
            {
                if (!bc->isvirtual)
                    genConstructorCall(b, sym, cons->memberInitializers, bc->cls, bc->offset, false, thisptr, otherptr, cons, true,
                                       doCopy || !cons->memberInitializers, !cons->defaulted);
                bc = bc->next;
            }
            if (hasVTab(sym))
                dovtabThunks(b, sym, thisptr, false);
            hr = sym->tp->syms->table[0];
            while (hr)
            {
                SYMBOL* sp = (SYMBOL*)hr->p;
                if ((sp->storage_class == sc_member || sp->storage_class == sc_mutable) && sp->tp->type != bt_aggregate)
                {
                    if (isstructured(sp->tp))
                    {
                        genConstructorCall(b, basetype(sp->tp)->sp, cons->memberInitializers, sp, sp->offset, true, thisptr,
                                           otherptr, cons, false, doCopy, !cons->defaulted);
                    }
                    else
                    {
                        genConsData(b, sym, cons->memberInitializers, sp, sp->offset, thisptr, otherptr, cons, doCopy);
                    }
                }
                hr = hr->next;
            }
            FreeLocalContext(NULL, cons, codeLabel++);
        }
    }
    if (parseInitializers)
        releaseInitializers(sym, cons);
    cons->labelCount = codeLabel - INT_MIN;
    codeLabel = oldCodeLabel;
    return thisptr;
}
void createConstructor(SYMBOL* sp, SYMBOL* consfunc)
{
    HASHTABLE* syms;
    BLOCKDATA b;
    STATEMENT* st;
    EXPRESSION* thisptr;
    memset(&b, 0, sizeof(BLOCKDATA));
    b.type = begin;
    syms = localNameSpace->syms;
    localNameSpace->syms = basetype(consfunc->tp)->syms;
    thisptr = thunkConstructorHead(&b, sp, consfunc, basetype(consfunc->tp)->syms, false, true);
    st = stmtNode(NULL, &b, st_return);
    st->select = thisptr;
    consfunc->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    consfunc->inlineFunc.stmt->lower = b.head;
    consfunc->inlineFunc.syms = basetype(consfunc->tp)->syms;
    consfunc->retcount = 1;
    consfunc->isInline = consfunc->attribs.inheritable.linkage2 != lk_export;
    //    consfunc->inlineFunc.stmt->blockTail = b.tail;
    InsertInline(consfunc);
    localNameSpace->syms = syms;
}
void asnVirtualBases(BLOCKDATA* b, SYMBOL* sp, VBASEENTRY* vbe, EXPRESSION* thisptr, EXPRESSION* other, bool move,
                     bool isconst)
{
    if (vbe)
    {
        asnVirtualBases(b, sp, vbe->next, thisptr, other, move, isconst);
        if (vbe->alloc)
            genAsnCall(b, sp, vbe->cls, vbe->structOffset, thisptr, other, move, isconst);
    }
}
static void genAsnData(BLOCKDATA* b, SYMBOL* cls, SYMBOL* member, int offset, EXPRESSION* thisptr, EXPRESSION* other)
{
    EXPRESSION* left = exprNode(en_add, thisptr, intNode(en_c_i, offset));
    EXPRESSION* right = exprNode(en_add, other, intNode(en_c_i, offset));
    STATEMENT* st;
    (void)cls;
    if (isstructured(member->tp) || isarray(member->tp))
    {
        left = exprNode(en_blockassign, left, right);
        left->size = member->tp->size;
        left->altdata = (void*)member->tp;
    }
    else
    {
        deref(member->tp, &left);
        deref(member->tp, &right);
        left = exprNode(en_assign, left, right);
    }
    st = stmtNode(NULL, b, st_expr);
    optimize_for_constants(&left);
    st->select = left;
}
static void genAsnCall(BLOCKDATA* b, SYMBOL* cls, SYMBOL* base, int offset, EXPRESSION* thisptr, EXPRESSION* other, bool move,
                       bool isconst)
{
    (void)cls;
    EXPRESSION* exp = NULL;
    STATEMENT* st;
    FUNCTIONCALL* params = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
    TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
    SYMBOL* asn1;
    SYMBOL* cons = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(base->tp)->syms);
    EXPRESSION* left = exprNode(en_add, thisptr, intNode(en_c_i, offset));
    EXPRESSION* right = exprNode(en_add, other, intNode(en_c_i, offset));
    if (move)
    {
        right = exprNode(en_not_lvalue, right, NULL);
    }
    if (isconst)
    {
        tp->type = bt_const;
        tp->size = base->tp->size;
        tp->btp = base->tp;
        tp->rootType = base->tp->rootType;
    }
    else
    {
        tp = base->tp;
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
    params->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
    params->arguments->tp = tp;
    params->arguments->exp = right;
    params->thisptr = left;
    params->thistp = (TYPE *)Alloc(sizeof(TYPE));
    params->thistp->type = bt_pointer;
    params->thistp->size = getSize(bt_pointer);
    params->thistp->btp = base->tp;
    params->thistp->rootType = params->thistp;
    params->ascall = true;
    asn1 = GetOverloadedFunction(&tp, &params->fcall, cons, params, NULL, true, false, true, 0);

    if (asn1)
    {
        SYMBOL* parm;
        AdjustParams(asn1, basetype(asn1->tp)->syms->table[0], &params->arguments, false, true);
        parm = (SYMBOL*)basetype(asn1->tp)->syms->table[0]->next->p;
        if (parm && isref(parm->tp))
        {
            TYPE* tp1 = (TYPE *)Alloc(sizeof(TYPE));
            tp1->type = bt_lref;
            tp1->size = getSize(bt_lref);
            tp1->btp = params->arguments->tp;
            tp1->rootType = tp1;
            params->arguments->tp = tp1;
        }
        if (!isAccessible(base, base, asn1, NULL, ac_protected, false))
        {
            errorsym(ERR_CANNOT_ACCESS, asn1);
        }
        if (asn1->defaulted && !asn1->inlineFunc.stmt)
            createAssignment(base, asn1);
        params->functp = asn1->tp;
        params->sp = asn1;
        params->ascall = true;
        exp = varNode(en_func, NULL);
        exp->v.func = params;
    }
    st = stmtNode(NULL, b, st_expr);
    optimize_for_constants(&exp);
    st->select = exp;
}
static void thunkAssignments(BLOCKDATA* b, SYMBOL* sym, SYMBOL* asnfunc, HASHTABLE* syms, bool move, bool isconst)
{
    HASHREC* hr = syms->table[0];
    EXPRESSION* thisptr = varNode(en_auto, (SYMBOL*)hr->p);
    EXPRESSION* other = NULL;
    BASECLASS* base;
    int oldCodeLabel = codeLabel;
    codeLabel = INT_MIN;
    if (hr->next)  // this had better be true
        other = varNode(en_auto, (SYMBOL*)hr->next->p);
    deref(&stdpointer, &thisptr);
    deref(&stdpointer, &other);
    if (sym->tp->type == bt_union)
    {
        genAsnData(b, sym, sym, 0, thisptr, other);
    }
    else
    {
        if (sym->vbaseEntries)
        {
            asnVirtualBases(b, sym, sym->vbaseEntries, thisptr, other, move, isconst);
        }
        base = sym->baseClasses;
        while (base)
        {
            if (!base->isvirtual)
            {
                genAsnCall(b, sym, base->cls, base->offset, thisptr, other, move, isconst);
            }
            base = base->next;
        }
        hr = sym->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = (SYMBOL*)hr->p;
            if ((sp->storage_class == sc_member || sp->storage_class == sc_mutable) && sp->tp->type != bt_aggregate)
            {
                if (isstructured(sp->tp))
                {
                    genAsnCall(b, sym, basetype(sp->tp)->sp, sp->offset, thisptr, other, move, isconst);
                }
                else
                {
                    genAsnData(b, sym, sp, sp->offset, thisptr, other);
                }
            }
            hr = hr->next;
        }
    }
    asnfunc->labelCount = codeLabel - INT_MIN;
    codeLabel = oldCodeLabel;
}
void createAssignment(SYMBOL* sym, SYMBOL* asnfunc)
{
    // if we get here we are just assuming it is a builtin assignment operator
    // because we only get here for 'default' functions and that is the only one
    // that can be defaulted...
    HASHTABLE* syms;
    BLOCKDATA b;
    bool move = basetype(((SYMBOL*)basetype(asnfunc->tp)->syms->table[0]->next->p)->tp)->type == bt_rref;
    bool isConst = isconst(((SYMBOL*)basetype(asnfunc->tp)->syms->table[0]->next->p)->tp);
    memset(&b, 0, sizeof(BLOCKDATA));
    b.type = begin;
    syms = localNameSpace->syms;
    localNameSpace->syms = basetype(asnfunc->tp)->syms;
    thunkAssignments(&b, sym, asnfunc, basetype(asnfunc->tp)->syms, move, isConst);
    asnfunc->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    asnfunc->inlineFunc.stmt->lower = b.head;
    asnfunc->inlineFunc.syms = basetype(asnfunc->tp)->syms;
    //    asnfunc->inlineFunc.stmt->blockTail = b.tail;
    InsertInline(asnfunc);
    localNameSpace->syms = syms;
}
static void genDestructorCall(BLOCKDATA* b, SYMBOL* sp, SYMBOL* against, EXPRESSION* base, EXPRESSION* arrayElms, int offset,
                              bool top)
{
    SYMBOL* dest;
    EXPRESSION* exp;
    STATEMENT* st;
    TYPE* tp = PerformDeferredInitialization(sp->tp, NULL);
    sp = tp->sp;
    dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    if (!dest)  // error handling
        return;
    exp = base;
    deref(&stdpointer, &exp);
    exp = exprNode(en_add, exp, intNode(en_c_i, offset));
    dest = (SYMBOL*)basetype(dest->tp)->syms->table[0]->p;
    if (dest->defaulted && !dest->inlineFunc.stmt)
    {
        createDestructor(sp);
    }
    callDestructor(sp, against, &exp, arrayElms, top, true, false);
    st = stmtNode(NULL, b, st_expr);
    optimize_for_constants(&exp);
    st->select = exp;
}
static void undoVars(BLOCKDATA* b, HASHREC* vars, EXPRESSION* base)
{
    if (vars)
    {
        SYMBOL* s = (SYMBOL*)vars->p;
        undoVars(b, vars->next, base);
        if ((s->storage_class == sc_member || s->storage_class == sc_mutable))
        {
            if (isstructured(s->tp))
            {
                genDestructorCall(b, (SYMBOL*)basetype(s->tp)->sp, NULL, base, NULL, s->offset, true);
            }
            else if (isarray(s->tp))
            {
                TYPE* tp = s->tp;
                while (isarray(tp))
                    tp = basetype(tp)->btp;
                tp = basetype(tp);
                if (isstructured(tp))
                    genDestructorCall(b, tp->sp, NULL, base, intNode(en_c_i, s->tp->size / tp->size), s->offset, true);
            }
        }
    }
}
static void undoBases(BLOCKDATA* b, SYMBOL* against, BASECLASS* bc, EXPRESSION* base)
{
    if (bc)
    {
        undoBases(b, against, bc->next, base);
        if (!bc->isvirtual)
        {
            genDestructorCall(b, bc->cls, against, base, NULL, bc->offset, false);
        }
    }
}
void thunkDestructorTail(BLOCKDATA* b, SYMBOL* sp, SYMBOL* dest, HASHTABLE* syms)
{
    EXPRESSION* thisptr;
    VBASEENTRY* vbe = sp->vbaseEntries;
    int oldCodeLabel = codeLabel;
    if (templateNestingCount)
        return;
    codeLabel = INT_MIN;
    thisptr = varNode(en_auto, (SYMBOL*)syms->table[0]->p);
    undoVars(b, basetype(sp->tp)->syms->table[0], thisptr);
    undoBases(b, sp, sp->baseClasses, thisptr);
    if (vbe)
    {
        SYMBOL* sp = (SYMBOL*)syms->table[0]->next->p;
        EXPRESSION* val = varNode(en_auto, sp);
        int lbl = codeLabel++;
        STATEMENT* st;
        sp->decoratedName = sp->errname = sp->name;
        sp->offset = chosenAssembler->arch->retblocksize + getSize(bt_pointer);
        deref(&stdint, &val);
        st = stmtNode(NULL, b, st_notselect);
        optimize_for_constants(&val);
        st->select = val;
        st->label = lbl;
        while (vbe)
        {
            if (vbe->alloc)
                genDestructorCall(b, vbe->cls, sp, thisptr, NULL, vbe->structOffset, false);
            vbe = vbe->next;
        }
        st = stmtNode(NULL, b, st_label);
        st->label = lbl;
    }
    dest->labelCount = codeLabel - INT_MIN;
    codeLabel = oldCodeLabel;
}
static void createDestructor(SYMBOL* sp)
{
    HASHTABLE* syms;
    SYMBOL* dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    BLOCKDATA b;
    memset(&b, 0, sizeof(BLOCKDATA));
    b.type = begin;
    dest = (SYMBOL*)basetype(dest->tp)->syms->table[0]->p;
    syms = localNameSpace->syms;
    localNameSpace->syms = basetype(dest->tp)->syms;
    thunkDestructorTail(&b, sp, dest, basetype(dest->tp)->syms);
    dest->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    dest->inlineFunc.stmt->lower = b.head;
    dest->inlineFunc.syms = basetype(dest->tp)->syms;
    dest->retcount = 1;
    dest->isInline = dest->attribs.inheritable.linkage2 != lk_export;
    //    dest->inlineFunc.stmt->blockTail = b.tail;
    InsertInline(dest);
    InsertExtern(dest);
    localNameSpace->syms = syms;
}
void makeArrayConsDest(TYPE** tp, EXPRESSION** exp, SYMBOL* cons, SYMBOL* dest, EXPRESSION* count)
{
    EXPRESSION* size = intNode(en_c_i, (*tp)->size);
    EXPRESSION *econs = (cons ? varNode(en_pc, cons) : NULL), *edest = varNode(en_pc, dest);
    FUNCTIONCALL* params = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
    SYMBOL* asn1;
    INITLIST* arg0 = (INITLIST*)Alloc(sizeof(INITLIST));  // this
    INITLIST* arg1 = (INITLIST*)Alloc(sizeof(INITLIST));  // cons
    INITLIST* arg2 = (INITLIST*)Alloc(sizeof(INITLIST));  // dest
    INITLIST* arg3 = (INITLIST*)Alloc(sizeof(INITLIST));  // size
    INITLIST* arg4 = (INITLIST*)Alloc(sizeof(INITLIST));  // count
    SYMBOL* ovl = namespacesearch("__arrCall", globalNameSpace, false, false);
    params->arguments = arg0;
    arg0->next = arg1;
    arg1->next = arg2;
    arg2->next = arg3;
    arg3->next = arg4;

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
    asn1 = GetOverloadedFunction(tp, &params->fcall, ovl, params, NULL, true, false, true, 0);
    if (!asn1)
    {
        diag("makeArrayConsDest: Can't call array iterator");
    }
    else
    {
        params->functp = asn1->tp;
        params->sp = asn1;
        params->ascall = true;
        *exp = varNode(en_func, NULL);
        (*exp)->v.func = params;
        // asn1->genreffed = true;
        // if (cons)
        // cons->genreffed = true;
        // dest->genreffed = true;
    }
}
void callDestructor(SYMBOL* sp, SYMBOL* against, EXPRESSION** exp, EXPRESSION* arrayElms, bool top, bool pointer,
                    bool skipAccess)
{
    SYMBOL* dest;
    SYMBOL* dest1;
    TYPE *tp = NULL, *stp;
    FUNCTIONCALL* params = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
    SYMBOL* sym;
    if (!against)
        against = sp;
    if (sp->tp->size == 0)
        sp = PerformDeferredInitialization(sp->tp, NULL)->sp;
    stp = sp->tp;
    dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    // if it isn't already defined get out, there will be an error from somewhere else..
    if (!basetype(sp->tp)->syms || !dest)
        return;
    sym = basetype(sp->tp)->sp;
    if (!*exp)
    {
        diag("callDestructor: no this pointer");
    }
    params->thisptr = *exp;
    params->thistp = (TYPE *)Alloc(sizeof(TYPE));
    params->thistp->type = bt_pointer;
    params->thistp->size = getSize(bt_pointer);
    params->thistp->btp = sp->tp;
    params->thistp->rootType = params->thistp;
    params->ascall = true;
    dest1 = basetype(dest->tp)->syms->table[0]->p;
    if (!dest1 || !dest1->defaulted)
        dest1 = GetOverloadedFunction(&tp, &params->fcall, dest, params, NULL, true, false, true, 0);
    else
        params->fcall = varNode(en_pc, dest1);
    if (dest1)
    {
        CheckCalledException(dest1, params->thisptr);
        if (!skipAccess && dest1 &&
            !isAccessible(against, sp, dest1, theCurrentFunc,
                          top ? (theCurrentFunc && theCurrentFunc->parentClass == sp ? ac_protected : ac_public) : ac_protected,
                          false))
        {
            errorsym(ERR_CANNOT_ACCESS, dest1);
        }
        if (dest1 && dest1->defaulted && !dest1->inlineFunc.stmt)
            createDestructor(sp);
        params->functp = dest1->tp;
        params->sp = dest1;
        params->ascall = true;
        if (arrayElms)
        {
            makeArrayConsDest(&stp, exp, NULL, dest1, arrayElms);
            // dest1->genreffed = true;
        }
        else
        {
            if (sp->vbaseEntries)
            {
                INITLIST *x = (INITLIST*)Alloc(sizeof(INITLIST)), **p;
                x->tp = (TYPE*)Alloc(sizeof(TYPE));
                x->tp->type = bt_int;
                x->tp->size = getSize(bt_int);
                x->tp->rootType = x->tp;
                x->exp = intNode(en_c_i, top);
                p = &params->arguments;
                while (*p)
                    p = &(*p)->next;
                *p = x;
                params->sp->noinline = true;
            }
            *exp = varNode(en_func, NULL);
            (*exp)->v.func = params;
        }
        if (*exp && !pointer)
        {
            *exp = exprNode(en_thisref, *exp, NULL);
            (*exp)->dest = true;
            (*exp)->v.t.thisptr = params->thisptr;
            (*exp)->v.t.tp = sp->tp;
            sp->hasDest = true;
            if (!sp->pureDest)
                hasXCInfo = true;
        }
    }
}
bool callConstructor(TYPE** tp, EXPRESSION** exp, FUNCTIONCALL* params, bool checkcopy, EXPRESSION* arrayElms, bool top,
                        bool maybeConversion, bool implicit, bool pointer, bool usesInitList, bool isAssign)
{
    (void)checkcopy;
    TYPE* stp = *tp;
    SYMBOL* sp;
    SYMBOL* against;
    SYMBOL* cons;
    SYMBOL* cons1;
    EXPRESSION* e1 = NULL;
    TYPE* initializerListTemplate = NULL;
    TYPE* initializerListType = NULL;
    bool initializerRef = false;
    PerformDeferredInitialization(stp, NULL);
    sp = basetype(*tp)->sp;
    against = top ? sp : sp->parentClass;
    
    if (isAssign)
        cons = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
    else
        cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);

    if (!params)
    {
        params = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
    }
    else
    {
        INITLIST* list = params->arguments;
        while (list)
        {
            if (!list->nested && isstructured(list->tp))
            {
                SYMBOL* sp1 = basetype(list->tp)->sp;
                if (!templateNestingCount && sp1->templateLevel && sp1->templateParams && !sp1->instantiated)
                {
                    if (!allTemplateArgsSpecified(sp1, sp1->templateParams))
                        sp1 = GetClassTemplate(sp1, sp1->templateParams->next, false);
                    if (sp1)
                        list->tp = TemplateClassInstantiate(sp1, sp1->templateParams, false, sc_global)->tp;
                }
            }
            list = list->next;
        }
    }
    params->thisptr = *exp;
    params->thistp = (TYPE *)Alloc(sizeof(TYPE));
    params->thistp->type = bt_pointer;params->thistp->btp = sp->tp;
    params->thistp->rootType = params->thistp;
    params->thistp->size = getSize(bt_pointer);
    params->ascall = true;
    cons1 = GetOverloadedFunction(tp, &params->fcall, cons, params, NULL, true, maybeConversion, true, usesInitList);

    if (cons1 && isfunction(cons1->tp))
    {
        CheckCalledException(cons1, params->thisptr);

        if (cons1->castoperator)
        {
            FUNCTIONCALL* oparams = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
            if (!isAccessible(cons1->parentClass, cons1->parentClass, cons1, NULL, ac_public, false))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            if (cons1->isExplicit && implicit)
                error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
            oparams->fcall = params->fcall;
            oparams->thisptr = params->arguments->exp;
            oparams->thistp = (TYPE *)Alloc(sizeof(TYPE));
            oparams->thistp->type = bt_pointer;
            oparams->thistp->size = getSize(bt_pointer);
            oparams->thistp->btp = cons1->parentClass->tp;
            oparams->thistp->rootType = oparams->thistp;
            oparams->functp = cons1->tp;
            oparams->sp = cons1;
            oparams->ascall = true;
            if (!isref(basetype(cons1->tp)->btp))
            {
                optimize_for_constants(exp);
                oparams->returnEXP = *exp;
                oparams->returnSP = sp;
            }
            e1 = varNode(en_func, NULL);
            e1->v.func = oparams;
        }
        else
        {
            if (!isAccessible(against, sp, cons1, NULL,
                              top ? (theCurrentFunc && theCurrentFunc->parentClass == sp ? ac_protected : ac_public) : ac_protected,
                              false))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            if (cons1->isExplicit && implicit)
                error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
            {
                HASHREC* hr = basetype(cons1->tp)->syms->table[0];
                if (((SYMBOL*)hr->p)->thisPtr)
                    hr = hr->next;
                if (!hr->next || ((SYMBOL*)hr->next->p)->init)
                {
                    TYPE* tp = ((SYMBOL*)hr->p)->tp;
                    if (isref(tp))
                    {
                        initializerRef = true;
                        tp = basetype(tp)->btp;
                    }
                    if (isstructured(tp))
                    {
                        SYMBOL* sym = (basetype(tp)->sp);
                        if (sym->parentNameSpace && !strcmp(sym->parentNameSpace->name, "std"))
                        {
                            if (!strcmp(sym->name, "initializer_list") && sym->templateLevel)
                            {
                                initializerListTemplate = sym->tp;
                                initializerListType = sym->templateParams->next->p->byClass.val;
                            }
                        }
                    }
                }
            }
            if (initializerListType)
            {
                CreateInitializerList(initializerListTemplate, initializerListType, &params->arguments, false, initializerRef);
                if (basetype(cons1->tp)->syms->table[0]->next->next)
                    AdjustParams(cons1, basetype(cons1->tp)->syms->table[0]->next->next, &params->arguments->next, false,
                                 implicit && !cons1->isExplicit);
            }
            else
            {
                AdjustParams(cons1, basetype(cons1->tp)->syms->table[0], &params->arguments, false, implicit && !cons1->isExplicit);
            }
            params->functp = cons1->tp;
            params->sp = cons1;
            params->ascall = true;
            if (cons1->defaulted && !cons1->inlineFunc.stmt)
                createConstructor(sp, cons1);
            if (arrayElms)
            {
                SYMBOL* dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
                SYMBOL* dest1;
                SYMBOL* against = top ? sp : sp->parentClass;
                TYPE* tp = NULL;
                FUNCTIONCALL* params = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                if (!*exp)
                {
                    diag("callDestructor: no this pointer");
                }
                params->thisptr = *exp;
                params->thistp = (TYPE *)Alloc(sizeof(TYPE));
                params->thistp->type = bt_pointer;
                params->thistp->size = getSize(bt_pointer);
                params->thistp->btp = sp->tp;
                params->thistp->rootType = params->thistp;
                params->ascall = true;
                dest1 = GetOverloadedFunction(&tp, &params->fcall, dest, params, NULL, true, false, true, 0);
                if (dest1 && !isAccessible(against, sp, dest1, NULL,
                                           top ? (theCurrentFunc && theCurrentFunc->parentClass == sp ? ac_protected : ac_public)
                                               : ac_protected,
                                           false))
                {
                    errorsym(ERR_CANNOT_ACCESS, dest1);
                }
                if (dest1 && dest1->defaulted && !dest1->inlineFunc.stmt)
                    createDestructor(sp);
                makeArrayConsDest(&stp, exp, cons1, dest1, arrayElms);
                e1 = *exp;
            }
            else
            {
                if (sp->vbaseEntries)
                {
                    INITLIST *x = (INITLIST*)Alloc(sizeof(INITLIST)), **p;
                    x->tp = (TYPE*)Alloc(sizeof(TYPE));
                    x->tp->type = bt_int;
                    x->tp->rootType = x->tp;
                    x->tp->size = getSize(bt_int);
                    x->exp = intNode(en_c_i, top);
                    p = &params->arguments;
                    while (*p)
                        p = &(*p)->next;
                    *p = x;
                    params->sp->noinline = true;
                }
                e1 = varNode(en_func, NULL);
                e1->v.func = params;
            }
        }

        *exp = e1;
        if (chosenAssembler->msil && *exp)
        {
            // this needs work, won't work for structures as members of other structures...
            EXPRESSION* exp1 = params->thisptr;
            if (exp1->type == en_add && isconstzero(&stdint, exp1->right))
                exp1 = exp1->left;
            *exp = exprNode(en_assign, exp1, *exp);
            params->thisptr = NULL;
        }
        else if (*exp && !pointer)
        {
            *exp = exprNode(en_thisref, *exp, NULL);
            (*exp)->v.t.thisptr = params->thisptr;
            (*exp)->v.t.tp = sp->tp;
            // hasXCInfo = true;
        }

        return true;
    }
    return false;
}
bool callConstructorParam(TYPE** tp, EXPRESSION** exp, TYPE* paramTP, EXPRESSION* paramExp, bool top, bool maybeConversion,
                             bool implicit, bool pointer)
{
    FUNCTIONCALL* params = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
    if (paramTP && paramExp)
    {
        params->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
        params->arguments->tp = paramTP;
        params->arguments->exp = paramExp;
    }
    return callConstructor(tp, exp, params, false, NULL, top, maybeConversion, implicit, pointer, false, false);
}

void PromoteConstructorArgs(SYMBOL* cons1, FUNCTIONCALL* params)
{
    HASHREC* hr = basetype(cons1->tp)->syms->table[0];
    if (((SYMBOL*)hr->p)->thisPtr)
        hr = hr->next;
    INITLIST* args = params->arguments;
    while (hr && args)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        TYPE* tps = basetype(sp->tp);
        TYPE* tpa = basetype(args->tp);
        if (isarithmetic(tps) && isarithmetic(tpa))
        {
            if (tps->type > bt_int && tps->type != tpa->type)
            {
                args->tp = sp->tp;
                cast(sp->tp, &args->exp);
            }
        }
        hr = hr->next;
        args = args->next;
    }
}