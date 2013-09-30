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
extern ARCH_ASM *chosenAssembler;

extern char *overloadNameTab[];
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern TYPE stdpointer, stdint;

static void genAsnCall(BLOCKDATA *b, SYMBOL *cls, SYMBOL *base, int offset, EXPRESSION *thisptr, EXPRESSION *other, BOOL move, BOOL isconst);
static void createDestructor(SYMBOL *sp);

void ConsDestDeclarationErrors(SYMBOL *sp, BOOL notype)
{
    if (sp->name == overloadNameTab[CI_CONSTRUCTOR])
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
    else if (sp->name == overloadNameTab[CI_DESTRUCTOR])
    {
        if (!notype)
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_TYPE);
        else if (sp->storage_class == sc_static)
            errorstr(ERR_INVALID_STORAGE_CLASS, "static");
        else if (isconst(sp->tp) || isvolatile(sp->tp))
            error(ERR_CONSTRUCTOR_OR_DESTRUCTOR_NO_CONST_VOLATILE);
    }
}
MEMBERINITIALIZERS *GetMemberInitializers(LEXEME **lex2, SYMBOL *sym)
{
    LEXEME *lex = *lex2;
    MEMBERINITIALIZERS *first = NULL, **cur = &first ;
//    if (sym->name != overloadNameTab[CI_CONSTRUCTOR])
//        error(ERR_INITIALIZER_LIST_REQUIRES_CONSTRUCTOR);
    while (lex != NULL)
    {
        if (ISID(lex))
        {
            *cur = Alloc(sizeof(MEMBERINITIALIZERS));
            (*cur)->name = litlate(lex->value.s.a);
            (*cur)->line = lex->line;
            (*cur)->file = lex->file;
            lex = getsym();
            if (MATCHKW(lex, openpa))
            {
                int paren = 0;
                LEXEME **mylex = &(*cur)->initData;
                *mylex = Alloc(sizeof(*(*cur)->initData));
                **mylex = *lex;
                mylex = &(*mylex)->next;
                lex = getsym();
                while (lex && (!MATCHKW(lex, closepa) || paren))
                {
                    if (MATCHKW(lex, openpa))
                        paren++;
                    if (MATCHKW(lex, closepa))
                        paren--;
                    if (lex->type == l_id)
                        lex->value.s.a = litlate(lex->value.s.a);
                    *mylex = Alloc(sizeof(*(*cur)->initData));
                    **mylex = *lex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
                if (MATCHKW(lex, closepa))
                {
                    *mylex = Alloc(sizeof(*(*cur)->initData));
                    **mylex = *lex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
            }
            else
            {
                error(ERR_MEMBER_INITIALIZATION_REQUIRED);
                skip(&lex, openbr);
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
static void SetParams(SYMBOL *cons)
{
    // c style only
    HASHREC *params = basetype(cons->tp)->syms->table[0];
    int base = chosenAssembler->arch->retblocksize;
    while (params)
    {
        assignParam(&base, (SYMBOL *)params->p);
        params = params->next;
    }
    cons->paramsize = base - chosenAssembler->arch->retblocksize;
}
static SYMBOL *insertFunc(SYMBOL *sp, SYMBOL *ovl)
{
    SYMBOL *funcs = search(ovl->name, basetype(sp->tp)->syms);
    ovl->parentClass = sp;
    ovl->internallyGenned = TRUE;
    ovl->linkage = lk_inline;
    ovl->defaulted = TRUE;
    ovl->access = ac_public;
    SetLinkerNames(ovl, lk_cdecl);
    if (!funcs)
    {
        TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
        tp->type = bt_aggregate;
        funcs = makeID(sc_overloads, tp, 0, ovl->name) ;
        funcs->parentClass = sp;
        tp->sp = funcs;
        SetLinkerNames(funcs, lk_cdecl);
        insert(funcs, basetype(sp->tp)->syms);
        funcs->parent = sp;
        funcs->tp->syms = CreateHashTable(1);
        insert(ovl, funcs->tp->syms);
    }
    else if (funcs->storage_class == sc_overloads)
    {
        insertOverload(ovl, funcs->tp->syms);
    }
    else 
    {
        diag("insertFunc: invalid overload tab");
    }
    injectThisPtr(ovl, basetype(ovl->tp)->syms);
    SetParams(ovl);
    return ovl;
}
static SYMBOL *declareDestructor(SYMBOL *sp)
{
    SYMBOL *func, *sp1;
    TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
    TYPE *tp1;
    tp->type = bt_func;
    tp->size = getSize(bt_pointer);
    tp->btp = (TYPE *)Alloc(sizeof(TYPE));
    tp->btp->type = bt_void;
    func = makeID(sc_member, tp, NULL, overloadNameTab[CI_DESTRUCTOR]);
    sp1= makeID(sc_parameter, NULL, NULL, AnonymousName());
    tp->syms = CreateHashTable(1);        
    return insertFunc(sp, func);
}
static BOOL hasConstFuncs(SYMBOL *sp, int type)
{
    TYPE *tp = NULL;
    EXPRESSION *exp = NULL;
    SYMBOL *ovl = search(overloadNameTab[type], basetype(sp->tp)->syms);
    if (ovl)
    {
        FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
        params->arguments = (ARGLIST *)Alloc(sizeof(ARGLIST));
        params->arguments->tp = (TYPE *)Alloc(sizeof(TYPE));
        params->arguments->tp->type = bt_const;
        params->arguments->tp->size = basetype(sp->tp)->size;
        params->arguments->tp->btp = basetype(sp->tp);
        params->arguments->exp = intNode(en_not_lvalue, 0);
        params->thisptr = varNode(en_auto, sp);
        params->thistp = Alloc(sizeof(TYPE));
        params->thistp->type = bt_pointer;
        params->thistp->size = getSize(bt_pointer);
        params->thistp->btp = sp->tp;
        return !!GetOverloadedFunction(&tp, &params->fcall, ovl, params, NULL, FALSE, FALSE);
    }
    return FALSE;
}
static BOOL constCopyConstructor(SYMBOL *sp)
{
    VBASEENTRY *e;
    BASECLASS *b;
    HASHREC *hr;
    b = sp->baseClasses;
    while (b)
    {
        if (!b->isvirtual && !hasConstFuncs(b->cls, CI_CONSTRUCTOR))
            return FALSE;
        b = b->next;
    }
    e = sp->vbaseEntries;
    while (e)
    {
        if (e->alloc && !hasConstFuncs(e->cls, CI_CONSTRUCTOR))
            return FALSE;
        e = e->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *cls = (SYMBOL *)hr->p;
        if (isstructured(cls->tp) && !cls->trivialCons)
            if (!hasConstFuncs(cls, CI_CONSTRUCTOR))
                return FALSE;
        hr = hr->next;
    }
    
    return TRUE;
}
static SYMBOL *declareConstructor(SYMBOL *sp, BOOL deflt, BOOL move)
{
    SYMBOL *func, *sp1;
    TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
    TYPE *tp1;
    tp->type = bt_func;
    tp->size = getSize(bt_pointer);
    tp->btp = (TYPE *)Alloc(sizeof(TYPE));
    tp->btp->type = bt_void;
    func = makeID(sc_member, tp, NULL, overloadNameTab[CI_CONSTRUCTOR]);
    sp1= makeID(sc_parameter, NULL, NULL, AnonymousName());
    tp->syms = CreateHashTable(1);        
    tp->syms->table[0] = (HASHREC *)Alloc(sizeof(HASHREC));
    tp->syms->table[0]->p = sp1;
    sp1->tp = (TYPE *)Alloc(sizeof(TYPE));
    if (deflt)
    {
        sp1->tp->type = bt_void;
    }
    else
    {
        TYPE *tpx = sp1->tp;
        tpx->type = move ? bt_rref : bt_lref;
        tpx->size = getSize(bt_pointer);
        tpx->btp = (TYPE *)Alloc(sizeof(TYPE));
        tpx = tpx->btp;
        if (constCopyConstructor(sp))
        {
            tpx->type = bt_const;
            tpx->size = getSize(bt_pointer);
            tpx->btp = (TYPE *)Alloc(sizeof(TYPE));
            tpx = tpx->btp;
        }
        *tpx = *basetype(sp->tp);
    }
    return insertFunc(sp, func);
}
static BOOL constAssignmentOp(SYMBOL *sp)
{
    VBASEENTRY *e;
    BASECLASS *b;
    HASHREC *hr;
    b = sp->baseClasses;
    while (b)
    {
        if (!b->isvirtual && !hasConstFuncs(b->cls, assign - kw_new + CI_NEW))
            return FALSE;
        b = b->next;
    }
    e = sp->vbaseEntries;
    while (e)
    {
        if (e->alloc && !hasConstFuncs(e->cls, assign - kw_new + CI_NEW))
            return FALSE;
        e = e->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *cls = (SYMBOL *)hr->p;
        if (isstructured(cls->tp) && !cls->trivialCons)
            if (!hasConstFuncs(cls, assign - kw_new + CI_NEW))
                return FALSE;
        hr = hr->next;
    }
    
    return TRUE;
}
static SYMBOL *declareAssignmentOp(SYMBOL *sp, BOOL move)
{
    SYMBOL *func, *sp1;
    TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
    TYPE *tp1, *tpx;
    tp->type = bt_func;
    tp->size = getSize(bt_pointer);
    tp->btp = (TYPE *)Alloc(sizeof(TYPE));
    tp->btp->type = bt_pointer;
    tp->btp->size = getSize(bt_pointer);
    tpx = tp->btp->btp = (TYPE *)Alloc(sizeof(TYPE));
    *(tp->btp->btp) = *basetype(sp->tp);
    func = makeID(sc_member, tp, NULL, overloadNameTab[assign - kw_new + CI_NEW]);
    sp1= makeID(sc_parameter, NULL, NULL, AnonymousName());
    tp->syms = CreateHashTable(1);
    tp->syms->table[0] = (HASHREC *)Alloc(sizeof(HASHREC));
    tp->syms->table[0]->p = sp1;
    sp1->tp = (TYPE *)Alloc(sizeof(TYPE));
    tpx = sp1->tp;
    tpx->type = move ? bt_rref : bt_lref;
    tpx->btp = (TYPE *)Alloc(sizeof(TYPE));
    tpx = tpx->btp;
    if (constAssignmentOp(sp))
    {
        tpx->type = bt_const;
        tpx->size = getSize(bt_pointer);
        tpx->btp = (TYPE *)Alloc(sizeof(TYPE));
        tpx = tpx->btp;
    }
    *tpx = *basetype(sp->tp);
    return insertFunc(sp, func);
}
static BOOL matchesDefaultConstructor(SYMBOL *sp)
{
    HASHREC *hr = basetype(sp->tp)->syms->table[0]->next;
    if (hr)
    {
        SYMBOL *arg1 = (SYMBOL *)hr->p;
        if (arg1->tp->type == bt_void || arg1->init)
            return TRUE;
    }
    return FALSE;
}
static BOOL matchesCopy(SYMBOL *sp, BOOL move)
{
    HASHREC *hr = basetype(sp->tp)->syms->table[0]->next;
    if (hr)
    {
        SYMBOL *arg1 = (SYMBOL *)hr->p;
        if (!hr->next || ((SYMBOL *)hr->next->p)->init || ((SYMBOL *)hr->next->p)->constop)
        {
            if (arg1->tp->type == (move ? bt_rref : bt_lref))
            {
                TYPE *tp  = basetype(arg1->tp)->btp;
                if (isstructured(tp))
                    if (basetype(tp)->sp == sp->parentClass)
                        return TRUE;
            }
        }
    }
    return FALSE;
}
static BOOL hasCopy(SYMBOL *func, BOOL move)
{
    HASHREC *hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (!sp->internallyGenned && matchesCopy(sp, move))
            return TRUE;
        hr = hr->next;
    }
    return FALSE;
}
static BOOL checkDest(SYMBOL *sp, HASHTABLE *syms, enum e_ac access)
{
    SYMBOL *dest = search(overloadNameTab[CI_DESTRUCTOR], syms);

    if (dest)
    {
        dest = (SYMBOL *)basetype(dest->tp)->syms->table[0]->p;
        if (dest->deleted)
            return TRUE;
        if (!isAccessible(sp, dest->parentClass, dest, NULL, access, FALSE))
            return TRUE;
    }
    return FALSE;
}
static BOOL checkDefaultCons(SYMBOL *sp, HASHTABLE *syms, enum e_ac access)
{
    SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], syms);
    if (cons)
    {
        SYMBOL *dflt = NULL;
        HASHREC *hr = cons->tp->syms->table[0];
        while (hr)
        {
            SYMBOL *cur = (SYMBOL *)hr->p;
            if (matchesDefaultConstructor(cur))
            {
                if (dflt)
                    return TRUE; // ambiguity
                dflt = cur;
            }
            hr = hr->next;
        }
        if (dflt)
        {
            if (dflt->deleted)
                return TRUE;
            if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, FALSE))
                return TRUE;
        }
    }
    return FALSE;
}
static SYMBOL *getCopyCons(SYMBOL *base, BOOL move)
{
    SYMBOL *ovl = search(overloadNameTab[CI_CONSTRUCTOR], basetype(base->tp)->syms);
    if (ovl)
    {
        FUNCTIONCALL funcparams;
        ARGLIST arg;
        EXPRESSION exp,exp1;
        TYPE tpp;
        TYPE *tpx = NULL;
        EXPRESSION *epx = NULL;
        memset(&funcparams, 0, sizeof(funcparams));
        memset(&arg, 0, sizeof(arg));
        memset(&exp, 0, sizeof(exp));
        memset(&exp1, 0, sizeof(exp1));
        if (move)
        {
            exp.type = en_auto;
            exp.v.sp = base;
        }
        else
        {
            exp.type = en_not_lvalue;
            exp.left = &exp1;
            exp1.type = en_auto;
            exp1.v.sp = base;
        }
        arg.exp = &exp;
        arg.tp = base->tp;
        tpp.type = bt_pointer;
        tpp.size = getSize(bt_pointer);
        tpp.btp = base->tp;
        funcparams.arguments = &arg;
        funcparams.thisptr = &exp;
        funcparams.thistp = &tpp;
        return GetOverloadedFunction(&tpx, &funcparams.fcall, ovl, &funcparams, NULL, FALSE, FALSE);
    }
    return NULL;
}
static SYMBOL *GetCopyAssign(SYMBOL *base, BOOL move)
{
    SYMBOL *ovl = search(overloadNameTab[assign - kw_new + CI_NEW ], basetype(base->tp)->syms);
    if (ovl)
    {
        FUNCTIONCALL funcparams;
        ARGLIST arg;
        EXPRESSION exp, exp1;
        TYPE tpt;
        TYPE *tpx = NULL;
        EXPRESSION *epx = NULL;
        memset(&funcparams, 0, sizeof(funcparams));
        memset(&arg, 0, sizeof(arg));
        memset(&exp, 0, sizeof(exp));
        memset(&exp1, 0, sizeof(exp1));
        memset(&tpt,0, sizeof(tpt));
        if (move)
        {
            exp.type = en_auto;
            exp.v.sp = base;
        }
        else
        {
            exp.type = en_not_lvalue;
            exp.left = &exp1;
            exp1.type = en_auto;
            exp1.v.sp = base;
        }
        tpt.type = bt_pointer;
        tpt.btp = base->tp;
        tpt.size = getSize(bt_pointer);
        arg.exp = &exp;
        arg.tp = &tpt;
        funcparams.arguments = &arg;
        funcparams.thisptr = &exp;
        funcparams.thistp = &tpt;
        return GetOverloadedFunction(&tpx, &funcparams.fcall, ovl, &funcparams, NULL, FALSE, FALSE);
    }
    return NULL;
}
BOOL hasVTab(SYMBOL *sp)
{
    VTABENTRY *vt = sp->vtabEntries;
    while( vt)
    {
        if (vt->virtuals)
            return TRUE;
        vt = vt->next;
    }
    return FALSE;
}
static BOOL hasTrivialCopy(SYMBOL *sp, BOOL move)
{
    HASHREC *hr;
    SYMBOL *dflt;
    BASECLASS * base;
    if (sp->vbaseEntries || hasVTab(sp))
        return FALSE;
    base = sp->baseClasses;
    while (base)
    {
        dflt = getCopyCons(base->cls, move);
        if (!dflt)
            return FALSE;
        if (!dflt->trivialCons)
            return FALSE;
        base = base->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *cls = (SYMBOL *)hr->p;
        if (isstructured(cls->tp))
        {
            dflt = getCopyCons(basetype(cls->tp)->sp, move);
            if (!dflt)
                return FALSE;
            if (!dflt->trivialCons)
                return FALSE;
        }
        hr = hr->next;
    }
    return TRUE;
}
static BOOL hasTrivialAssign(SYMBOL *sp, BOOL move)
{
    HASHREC *hr;
    SYMBOL *dflt;
    BASECLASS * base;
    if (sp->vbaseEntries || hasVTab(sp))
        return FALSE;
    base = sp->baseClasses;
    while (base)
    {
        dflt = GetCopyAssign(base->cls, move);
        if (!dflt)
            return FALSE;
        if (!dflt->trivialCons)
            return FALSE;
        base = base->next;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *cls = (SYMBOL *)hr->p;
        if (isstructured(cls->tp))
        {
            dflt = getCopyCons(basetype(cls->tp)->sp, move);
            if (!dflt)
                return FALSE;
            if (!dflt->trivialCons)
                return FALSE;
        }
        hr = hr->next;
    }
    return TRUE;
}
static BOOL checkCopyCons(SYMBOL *sp, SYMBOL *base, enum e_ac access)
{
    SYMBOL *dflt = getCopyCons(base, FALSE);
    if (dflt)
    {
        if (dflt->deleted)
            return TRUE;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, FALSE))
            return TRUE;
    }
    return FALSE;
}
static BOOL checkCopyAssign(SYMBOL *sp, SYMBOL *base, enum e_ac access)
{
    SYMBOL *dflt = GetCopyAssign(base, FALSE);
    if (dflt)
    {
        if (dflt->deleted)
            return TRUE;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, FALSE))
            return TRUE;
    }
    return FALSE;
}
static BOOL checkMoveCons(SYMBOL *sp, SYMBOL *base, enum e_ac access)
{
    SYMBOL *dflt = getCopyCons(base, TRUE);
    if (dflt)
    {
        if (dflt->deleted)
            return TRUE;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, FALSE))
            return TRUE;
    }
    return FALSE;
}
static BOOL checkMoveAssign(SYMBOL *sp, SYMBOL *base, enum e_ac access)
{
    SYMBOL *dflt = GetCopyAssign(base, TRUE);
    if (dflt)
    {
        if (dflt->deleted)
            return TRUE;
        if (!isAccessible(sp, dflt->parentClass, dflt, NULL, access, FALSE))
            return TRUE;
    }
    else
    {
        if (!hasTrivialAssign(sp, TRUE))
            return TRUE;
    }
    return FALSE;
}
static BOOL isDefaultDeleted(SYMBOL *sp)
{
    HASHREC *hr;
    BASECLASS *base;
    VBASEENTRY *vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        BOOL allconst = TRUE;
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (!isconst(sp->tp) && sp->tp->type != bt_aggregate)
                allconst = FALSE;
            if (isstructured(sp->tp))
            {
                SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
                HASHREC *hr1 = cons->tp->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL *)hr1->p;
                    if (matchesDefaultConstructor(cons))
                        if (!cons->trivialCons)
                            return TRUE;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
        if (allconst)
            return TRUE;
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp1 = (SYMBOL *)hr->p;
        TYPE *m;
        if (sp1->storage_class == sc_member)
        {
            if (isref(sp1->tp))
                if (!sp1->init)
                    return TRUE;
            if (basetype(sp1->tp)->type == bt_union)
            {
                HASHREC *hr1 = sp1->tp->syms->table[0];
                while (hr1)
                {
                    SYMBOL *member = (SYMBOL *)hr1->p;
                    if (!isconst(member->tp) && member->tp->type != bt_aggregate)
                    {
                        break;
                    }
                    hr1 = hr1->next;
                }
                if (!hr1)
                    return TRUE;
            }
            if (isstructured(sp1->tp))
            {
                TYPE *tp = basetype(sp1->tp);
                if (checkDest(sp, basetype(tp->sp->tp)->syms, ac_public))
                    return TRUE;
            }
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                TYPE *tp = basetype(m);
                if (checkDefaultCons(sp, basetype(tp->sp->tp)->syms, ac_public))
                    return TRUE;
            }
        }        
        hr = hr->next;
    }
    
    base = sp->baseClasses;
    while (base)
    {
        if (checkDest(sp, basetype(base->cls->tp)->syms, ac_protected))
            return TRUE;
        if (checkDefaultCons(sp, basetype(base->cls->tp)->syms, ac_protected))
            return TRUE;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc)
        {
            if (checkDest(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return TRUE;
            if (checkDefaultCons(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return TRUE;
        }
        vbase = vbase->next;
    }
    return FALSE;
}
static BOOL isCopyConstructorDeleted(SYMBOL *sp)
{
    HASHREC *hr;
    BASECLASS *base;
    VBASEENTRY *vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
                HASHREC *hr1 = cons->tp->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL *)hr1->p;
                    if (matchesCopy(cons, FALSE))
                        if (!cons->trivialCons)
                            return TRUE;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp1 = (SYMBOL *)hr->p;
        TYPE *m;
        if (sp1->storage_class == sc_member)
        {
            if (basetype(sp1->tp)->type == bt_rref)
                return TRUE;
            if (isstructured(sp1->tp))
            {
                TYPE *tp = basetype(sp1->tp);
                if (checkDest(sp, basetype(tp->sp->tp)->syms, ac_public))
                    return TRUE;
            }
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                if (checkCopyCons(sp, basetype(m)->sp, ac_public))
                    return TRUE;
            }
        }        
        hr = hr->next;
    }
    
    base = sp->baseClasses;
    while (base)
    {
        if (checkDest(sp, basetype(base->cls->tp)->syms, ac_protected))
            return TRUE;
        if (checkCopyCons(sp, base->cls, ac_protected))
            return TRUE;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc)
        {
            if (checkDest(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return TRUE;
            if (checkCopyCons(sp, vbase->cls, ac_protected))
                return TRUE;
        }
        vbase = vbase->next;
    }
    return FALSE;
    
}
static BOOL isCopyAssignmentDeleted(SYMBOL *sp)
{
    HASHREC *hr;
    BASECLASS *base;
    VBASEENTRY *vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL *cons = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
                HASHREC *hr1 = cons->tp->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL *)hr1->p;
                    if (matchesCopy(cons, FALSE))
                        if (!cons->trivialCons)
                            return TRUE;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp1 = (SYMBOL *)hr->p;
        TYPE *m;
        if (sp1->storage_class == sc_member)
        {
            if (isref(sp1->tp))
                return TRUE;
            if (!isstructured(sp1->tp) && isconst(sp1->tp) && sp1->tp->type != bt_aggregate)
                return TRUE;
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                if (checkCopyAssign(sp, basetype(m)->sp, ac_public))
                    return TRUE;
            }
        }        
        hr = hr->next;
    }
    
    base = sp->baseClasses;
    while (base)
    {
        if (checkCopyAssign(sp, base->cls, ac_protected))
            return TRUE;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc && checkCopyAssign(sp, vbase->cls, ac_protected))
            return TRUE;
        vbase = vbase->next;
    }
    return FALSE;
    
}
static BOOL isMoveConstructorDeleted(SYMBOL *sp)
{
    HASHREC *hr;
    BASECLASS *base;
    VBASEENTRY *vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
                HASHREC *hr1 = cons->tp->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL *)hr1->p;
                    if (matchesCopy(cons, TRUE))
                        if (!cons->trivialCons)
                            return TRUE;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp1 = (SYMBOL *)hr->p;
        TYPE *m;
        if (sp1->storage_class == sc_member)
        {
            if (basetype(sp1->tp)->type == bt_rref)
                return TRUE;
            if (isstructured(sp1->tp))
            {
                TYPE *tp = basetype(sp1->tp);
                if (checkDest(sp, basetype(tp->sp->tp)->syms, ac_public))
                    return TRUE;
            }
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                if (checkMoveCons(sp, basetype(m)->sp, ac_public))
                    return TRUE;
            }
        }        
        hr = hr->next;
    }
    
    base = sp->baseClasses;
    while (base)
    {
        if (checkDest(sp, basetype(base->cls->tp)->syms, ac_protected))
            return TRUE;
        if (checkMoveCons(sp, base->cls, ac_protected))
            return TRUE;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc)
        {
            if (checkDest(sp, basetype(vbase->cls->tp)->syms, ac_protected))
                return TRUE;
            if (checkMoveCons(sp, vbase->cls, ac_protected))
                return TRUE;
        }
        vbase = vbase->next;
    }
    return FALSE;
    
}
static BOOL isMoveAssignmentDeleted(SYMBOL *sp)
{
    HASHREC *hr;
    BASECLASS *base;
    VBASEENTRY *vbase;
    if (basetype(sp->tp)->type == bt_union)
    {
        hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (isstructured(sp->tp))
            {
                SYMBOL *cons = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
                HASHREC *hr1 = cons->tp->syms->table[0];
                while (hr1)
                {
                    cons = (SYMBOL *)hr1->p;
                    if (matchesCopy(cons, TRUE))
                        if (!cons->trivialCons)
                            return TRUE;
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp1 = (SYMBOL *)hr->p;
        TYPE *m;
        if (sp1->storage_class == sc_member)
        {
            if (isref(sp1->tp))
                return TRUE;
            if (!isstructured(sp1->tp) && isconst(sp1->tp) && sp1->tp->type != bt_aggregate)
                return TRUE;
            m = sp1->tp;
            if (isarray(m))
                m = basetype(sp1->tp)->btp;
            if (isstructured(m))
            {
                if (checkMoveAssign(sp, basetype(m)->sp, ac_public))
                    return TRUE;
            }
        }        
        hr = hr->next;
    }
    
    base = sp->baseClasses;
    while (base)
    {
        if (checkMoveAssign(sp, base->cls, ac_protected))
            return TRUE;
        base = base->next;
    }
    vbase = sp->vbaseEntries;
    while (vbase)
    {
        if (vbase->alloc && checkMoveAssign(sp, vbase->cls, ac_protected))
            return TRUE;
        vbase = vbase->next;
    }
    return FALSE;
    
}
static BOOL conditionallyDeleteDefaultConstructor(SYMBOL *func)
{
    HASHREC *hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->defaulted && matchesDefaultConstructor(sp))
        {
            if (isDefaultDeleted(sp->parentClass))
            {
                sp->deleted = TRUE;
            }
        }
        hr = hr->next;
    }
}
static BOOL conditionallyDeleteCopyConstructor(SYMBOL *func, BOOL move)
{
    HASHREC *hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->defaulted && matchesCopy(sp, move))
        {
            if (isCopyConstructorDeleted(sp->parentClass))
                sp->deleted = TRUE;
        }
        hr = hr->next;
    }
    return FALSE;
}
static BOOL conditionallyDeleteCopyAssignment(SYMBOL *func, BOOL move)
{
    HASHREC *hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->defaulted && matchesCopy(sp, move))
        {
            if (isCopyAssignmentDeleted(sp->parentClass))
                sp->deleted = TRUE;
        }
        hr = hr->next;
    }
    return FALSE;
}
void createDefaultConstructors(SYMBOL *sp)
{
    SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
    SYMBOL *dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    SYMBOL *asgn = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
    if (!dest)
        declareDestructor(sp);
    if (cons)
    {
        sp->hasUserCons = TRUE;
    }
    else
    {
        SYMBOL *newcons;
        // first see if the default constructor could be trivial
        if (!hasVTab(sp) && sp->vbaseEntries == NULL)
        {
            BASECLASS *base = sp->baseClasses;
            while (base)
            {
                if (!base->cls->trivialCons)
                    break;
                base = base->next;
            }
            if (!base)
            {
                HASHREC *p = basetype(sp->tp)->syms->table[0];
                while (p)
                {
                    SYMBOL *pcls = (SYMBOL *)p->p;
                    if (pcls->storage_class == sc_member)
                    {
                        if (isstructured(pcls->tp))
                        {
                            if (!sp->trivialCons)
                                break;
                        }
                        else if (pcls->init) // brace or equal initializer goes here
                            break;       
                    }
                    p = p->next;
                }
                if (!p)
                {
                    sp->trivialCons = TRUE;
                }
            }
        }
        // now create the default constructor
        newcons = declareConstructor(sp, TRUE, FALSE);
        newcons->trivialCons = sp->trivialCons;
        cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
    }
    conditionallyDeleteDefaultConstructor(cons);
    // now if there is no copy constructor or assignment operator declare them
    if (!hasCopy(cons, FALSE))
    {
        SYMBOL *newcons = declareConstructor(sp, FALSE, FALSE);
        newcons->trivialCons = hasTrivialCopy(sp, FALSE);
        if (hasCopy(cons, TRUE) || asgn && hasCopy(asgn, TRUE))
            newcons->deleted = TRUE;
        if (!asgn)
            asgn = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
    }
    conditionallyDeleteCopyConstructor(cons, FALSE);
    if (!asgn || !hasCopy(asgn, FALSE))
    {
        SYMBOL *newsp = declareAssignmentOp(sp, FALSE);
        newsp->trivialCons = hasTrivialAssign(sp, FALSE);
        if (hasCopy(cons, TRUE) || asgn && hasCopy(asgn, TRUE))
            newsp->deleted = TRUE;            
        if (!asgn)
            asgn = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(sp->tp)->syms);
    }
    conditionallyDeleteCopyAssignment(asgn,FALSE);
    // now if there is no move constructor, no copy constructor,
        // no copy assignment, no move assignment, no destructor
        // and wouldn't be defined as deleted
        // declare a move constructor and assignment operator
    if (!dest && !hasCopy(cons,FALSE ) && !hasCopy(cons,TRUE) &&
        !hasCopy(asgn, FALSE) && (!asgn || !hasCopy(asgn, TRUE)))
    {
        BOOL b = isMoveConstructorDeleted(sp);
        SYMBOL *newcons;
        if (!b)
        {
            newcons = declareConstructor(sp, FALSE, TRUE);
            newcons->trivialCons = hasTrivialCopy(sp, TRUE);
        }
        newcons = declareAssignmentOp(sp, TRUE);
        newcons->trivialCons = hasTrivialAssign(sp, TRUE);
        newcons->deleted = isMoveAssignmentDeleted(sp);
    }
    else
    {
        conditionallyDeleteCopyConstructor(cons,TRUE);
        conditionallyDeleteCopyAssignment(asgn,TRUE);
    }
}
void destructBlock(EXPRESSION **exp, HASHREC *hr)
{
    if (!cparams.prm_cplusplus)
        return;
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->storage_class != sc_localstatic && sp->dest)
        {
            
            EXPRESSION *iexp = convertInitToExpression(sp->dest->basetp, sp, NULL, sp->dest, NULL);
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
        else if (sp->storage_class == sc_parameter && isstructured(sp->tp))
        {
            EXPRESSION *iexp = getVarNode(sp);
            callDestructor(basetype(sp->tp)->sp, &iexp, NULL, TRUE);
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
        hr = hr->next;
    }
}
static MEMBERINITIALIZERS *getInit(MEMBERINITIALIZERS *init, SYMBOL *member)
{
    while (init)
    {
        if (init->sp == member)
            return init;
        init = init->next;
    }
    return NULL;
}
static void genConsData(BLOCKDATA *b, SYMBOL *cls, MEMBERINITIALIZERS *mi, 
                        SYMBOL *member, int offset, 
                        EXPRESSION *thisptr, EXPRESSION *otherptr, SYMBOL *parentCons)
{
    thisptr = exprNode(en_add, thisptr, intNode(en_c_i, offset));
    if (matchesCopy(parentCons, FALSE) || matchesCopy(parentCons, TRUE))
    {
        otherptr = exprNode(en_add, otherptr, intNode(en_c_i, offset));
        if (isstructured(member->tp))
        {
            EXPRESSION *exp = exprNode(en_blockassign, thisptr,otherptr);
            STATEMENT *st = stmtNode(NULL,b, st_expr);
            exp->size = member->tp->size;
            optimize_for_constants(&exp);
            st->select = exp;
        }
        else
        {
            STATEMENT *st = stmtNode(NULL,b, st_expr);
            EXPRESSION *exp;
            deref(member->tp, &thisptr);
            deref(member->tp, &otherptr);
            exp = exprNode(en_assign, thisptr, otherptr);
            optimize_for_constants(&exp);
            st->select = exp;
        }
    }
    else if (member->init)
    {
        EXPRESSION *exp;
        STATEMENT *st = stmtNode(NULL,b, st_expr);
        exp = convertInitToExpression(member->tp, member, NULL, member->init, thisptr);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void genConstructorCall(BLOCKDATA *b, SYMBOL *cls, MEMBERINITIALIZERS *mi, SYMBOL *member, int memberOffs, BOOL top, EXPRESSION *thisptr, EXPRESSION *otherptr, SYMBOL *parentCons)
{
    MEMBERINITIALIZERS *v = getInit(mi, member);
    if (v)
    {
        TYPE *ctype = member->tp;
        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
        EXPRESSION *exp = exprNode(en_add, thisptr, intNode(en_c_i, memberOffs));
        STATEMENT *st;
        LEXEME *lex = SetAlternateLex(v->initData);
        lex = getArgs(lex, NULL, funcparams);
        SetAlternateLex(NULL);
        if (!callConstructor(&ctype, &exp, funcparams, FALSE, NULL, top, FALSE))
            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
            
        st = stmtNode(NULL,b, st_expr);
        optimize_for_constants(&exp);
        st->select = exp;
    }
    else if (member->init)
    {
        TYPE *ctype = member->tp;
        EXPRESSION *iexp = convertInitToExpression(member->tp, member, NULL, member->init, thisptr);
        EXPRESSION *dexp;
        EXPRESSION *exp = exprNode(en_add, thisptr, intNode(en_c_i, memberOffs));
        FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
        STATEMENT *st;
        params->arguments = (ARGLIST *)Alloc(sizeof(ARGLIST));
        params->arguments->tp = member->tp;
        params->arguments->exp = iexp;
        if (!callConstructor(&ctype, &exp, params, FALSE, NULL, top, FALSE))
            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
        if (member->dest)
        {
            dexp = convertInitToExpression(member->tp, member, NULL, member->dest, thisptr);
            exp = exprNode(en_void, iexp, exprNode(en_void, exp, dexp));
        }
        else
        {
            exp = exprNode(en_void, iexp, exp);
        }
        st = stmtNode(NULL,b, st_expr);
        optimize_for_constants(&exp);
        st->select = exp;
    }
    else
        
    {
        TYPE *ctype = member->tp;
        EXPRESSION *exp = exprNode(en_add, thisptr, intNode(en_c_i, memberOffs));
        STATEMENT *st;
        if (matchesCopy(parentCons, FALSE))
        {
            FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
            TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
            EXPRESSION *other = exprNode(en_add, otherptr, intNode(en_c_i, memberOffs));
            if (basetype(parentCons->tp)->type == bt_rref)
                other = exprNode(en_not_lvalue, other, NULL);
            if (isconst(((SYMBOL *)basetype(parentCons->tp)->syms->table[0]->next->p)->tp->btp))
            {
                tp->type = bt_const;
                tp->size = basetype(member->tp)->size;
                tp->btp = member->tp;
            }
            else
            {
                tp=member->tp;
            }
            params->arguments = (ARGLIST *)Alloc(sizeof(ARGLIST));
            params->arguments->tp = tp;
            params->arguments->exp = other;
            if (!callConstructor(&ctype, &exp, params, FALSE, NULL, top, FALSE))
                errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
        }
        else if (matchesCopy(parentCons, TRUE))
        {
            FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
            TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
            EXPRESSION *other = exprNode(en_add, otherptr, intNode(en_c_i, memberOffs));
            if (basetype(parentCons->tp)->type == bt_rref)
                other = exprNode(en_not_lvalue, other, NULL);
            if (isconst(((SYMBOL *)basetype(parentCons->tp)->syms->table[0]->next->p)->tp->btp))
            {
                tp->type = bt_const;
                tp->size = basetype(member->tp)->size;
                tp->btp = member->tp;
            }
            else
            {
                tp = member->tp;
            }
            params->arguments = (ARGLIST *)Alloc(sizeof(ARGLIST));
            params->arguments->tp = tp;
            params->arguments->exp = other;
            if (!callConstructor(&ctype, &exp, params, FALSE, NULL, top, FALSE))
                errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
        }
        else
        {
            if (!callConstructor(&ctype, &exp, NULL, FALSE, NULL, top, FALSE))
                errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
        }
        st = stmtNode(NULL,b, st_expr);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void virtualBaseThunks(BLOCKDATA *b, SYMBOL *sp, EXPRESSION *thisptr)
{
    VBASEENTRY *entries = sp->vbaseEntries;
    EXPRESSION *first = NULL, **pos = &first;
    STATEMENT *st;
    while (entries)
    {
        if (entries->alloc)
        {
            EXPRESSION *left = exprNode(en_add, thisptr, intNode(en_c_i, entries->pointerOffset));
            EXPRESSION *right = exprNode(en_add, thisptr, intNode(en_c_i, entries->structOffset));
            EXPRESSION *asn;
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
        st = stmtNode(NULL,b, st_expr);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void dovtabThunks(BLOCKDATA *b, SYMBOL *sym, EXPRESSION *thisptr)
{
    VTABENTRY *entries = sym->vtabEntries;
    EXPRESSION *first = NULL, **pos = &first;
    STATEMENT *st;
    SYMBOL *localsp;
    localsp = sym->vtabsp;
    while (entries)
    {
        if (!entries->isdead)
        {
            EXPRESSION *left = exprNode(en_add, thisptr, intNode(en_c_i, entries->dataOffset));
            EXPRESSION *right = exprNode(en_add, varNode(en_global, localsp), intNode(en_c_i, entries->vtabOffset));
            EXPRESSION *asn;
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
        st = stmtNode(NULL,b, st_expr);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void doVirtualBases(BLOCK *b, SYMBOL *sp, MEMBERINITIALIZERS *mi, VBASEENTRY *vbe, EXPRESSION *thisptr, EXPRESSION *otherptr, SYMBOL *parentCons)
{
	if (vbe)
	{
	    doVirtualBases(b, sp, mi, vbe->next, thisptr, otherptr, parentCons);
        if (vbe->alloc)
    		genConstructorCall(b, sp, mi, vbe->cls, vbe->structOffset, FALSE, thisptr, otherptr, parentCons);
	}
}
static EXPRESSION *unshim(EXPRESSION *exp, EXPRESSION *ths)
{
    EXPRESSION *nw;
    if (!exp)
        return exp;
    if (exp->type == en_thisshim)
        return ths;
    nw = Alloc(sizeof(EXPRESSION));
    *nw = *exp;
    nw->left = unshim(nw->left, ths);
    nw->right = unshim(nw->right, ths);
    return nw;    
}
static void lookupInitializers(SYMBOL *cls, SYMBOL *cons, EXPRESSION *ths)
{
    MEMBERINITIALIZERS *init = cons->memberInitializers;
    HASHREC *hr = basetype(cls->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->storage_class == sc_member)
        {
            sp->lastInit = sp->init;
            if (sp->init)
            {
                sp->init = Alloc(sizeof(*sp->init));
                *sp->init = *sp->lastInit;
                sp->init->exp = unshim(sp->init->exp, ths);
            }
        }
        hr = hr->next;
    }
    while (init)
    {
        LEXEME *lex;
        BASECLASS *bc = cls->baseClasses;
        init->sp = search(init->name, basetype(cls->tp)->syms);
        if (init->sp)
        {
            if (init->sp->storage_class != sc_member)
            {
                errorsym(ERR_NEED_NONSTATIC_MEMBER, init->sp);
            }
            else
            {
                init->sp->init = NULL;
                lex = SetAlternateLex(init->initData);
                lex = initialize(lex, NULL, init->sp, sc_member);
                SetAlternateLex(NULL);
                init->sp->init->exp = unshim(init->sp->init->exp, ths);
            }
        }
        else
        {
            while (bc)
            {
                if (!strcmp(bc->cls->name, init->name))
                {
                    if (init->sp)
                    {
                        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, init->sp, cls);
                    }
                    init->sp = bc->cls;
                }
                bc = bc->next;
            }
        }
        if (!init->sp)
        {
            errorstrsym(ERR_NOT_A_MEMBER_OR_BASE_CLASS, init->name, cls);
        }
        init = init->next;
    }
    hr = basetype(cls->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (!sp->init)
        {
            if (isref(sp->tp))
                errorsym(ERR_REF_MEMBER_MUST_INITIALIZE, sp);
            else if (isconst(sp->tp))
                errorsym(ERR_CONSTANT_MEMBER_MUST_BE_INITIALIZED, sp);
        }
        hr = hr->next;
    }
}
static void releaseInitializers(SYMBOL *cls)
{
    HASHREC *hr = basetype(cls->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        if (sp->storage_class == sc_member)
            sp->init = sp->lastInit;
        hr = hr->next;
    }
}
void thunkConstructorHead(BLOCKDATA *b, SYMBOL *sym, SYMBOL *cons, HASHTABLE *syms, BOOL parseInitializers)
{
    BASECLASS *bc;
    HASHREC *hr = syms->table[0];
    EXPRESSION *thisptr = varNode(en_auto, (SYMBOL *)hr->p);
    EXPRESSION *otherptr = NULL;
    if (hr->next)
        otherptr = varNode(en_auto, (SYMBOL *)hr->next->p);
    deref(&stdpointer, &thisptr);
    deref(&stdpointer, &otherptr);
    if (parseInitializers)
        lookupInitializers(sym, cons, thisptr);
    if (sym->tp->type == bt_union)
    {
        genConsData(b, sym, cons->memberInitializers, sym, 0, thisptr, otherptr, cons);
    }
    else
    {
        if (sym->vbaseEntries)
        {
            SYMBOL *sp = makeID(sc_parameter, &stdint, NULL, "__$$constop");
            EXPRESSION *val = varNode(en_auto, sp);
            int lbl = beGetLabel;
            STATEMENT *st;
            sp->constop = TRUE;
            sp->decoratedName = sp->errname = sp->name;
            sp->offset = chosenAssembler->arch->retblocksize + cons->paramSize;
            insert(sp, localNameSpace->syms);
    
            deref(&stdint, &val);
            st = stmtNode(NULL,b, st_notselect);
            optimize_for_constants(&val);
            st->select = val;
            st->label = lbl;
            virtualBaseThunks(b, sym, thisptr);
            doVirtualBases(b, sym, cons->memberInitializers, sym->vbaseEntries, thisptr, otherptr, cons);
            st = stmtNode(NULL,b, st_label);
            st->label = lbl;
        }
        bc = sym->baseClasses;
        while (bc)
        {
            if (!bc->isvirtual)
                genConstructorCall(b, sym, cons->memberInitializers, bc->cls, bc->offset, FALSE, thisptr, otherptr, cons);
            bc = bc->next;
        }
        if (hasVTab(sym))
            dovtabThunks(b, sym, thisptr);
        hr = sym->tp->syms->table[0];
        while (hr)
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->storage_class == sc_member && sp->tp->type != bt_aggregate)
            {
                if (isstructured(sp->tp))
                {
                    genConstructorCall(b, basetype(sp->tp)->sp, cons->memberInitializers, sp, sp->offset,TRUE, thisptr, otherptr, cons);
                }
                else
                {
                    genConsData(b, sym, cons->memberInitializers, sp, sp->offset, thisptr, otherptr, cons);
                }
            }
            hr = hr->next;
        }
    }
    if (parseInitializers)
        releaseInitializers(sym);
}
static void createConstructor(SYMBOL *sp, SYMBOL *consfunc)
{
    HASHTABLE *syms;
    BLOCKDATA b;
    memset(&b, 0, sizeof(BLOCKDATA));
    b.type = begin;
    syms = localNameSpace->syms;
    localNameSpace->syms = basetype(consfunc->tp)->syms;
    thunkConstructorHead(&b, sp, consfunc, basetype(consfunc->tp)->syms, FALSE);
    consfunc->inlineFunc.stmt = stmtNode(NULL,NULL, st_block);
    consfunc->inlineFunc.stmt->lower = b.head;
//    consfunc->inlineFunc.stmt->blockTail = b.tail;
    InsertInline(consfunc);
    localNameSpace->syms = syms;
}
static void asnVirtualBases(BLOCKDATA *b, SYMBOL *sp, VBASEENTRY *vbe, 
                            EXPRESSION *thisptr, EXPRESSION *other, BOOL move, BOOL isconst)
{
	if (vbe)
	{
	    asnVirtualBases(b, sp, vbe->next, thisptr, other, move, isconst);
        if (vbe->alloc)
    		genAsnCall(b, sp, vbe->cls, vbe->structOffset, thisptr, other, move, isconst);
	}
}
static void genAsnData(BLOCKDATA *b, SYMBOL *cls, SYMBOL *member, int offset, EXPRESSION *thisptr, EXPRESSION *other)
{
    EXPRESSION *left = exprNode(en_add, thisptr, intNode(en_c_i, offset));
    EXPRESSION *right = exprNode(en_add, other, intNode(en_c_i, offset));
    STATEMENT *st;
    (void)cls;
    if (isstructured(member->tp))
    {
        left = exprNode(en_blockassign, left, right);
        left->size = member->tp->size;
    }
    else
    {
        deref(member->tp, &left);
        deref(member->tp, &right);
        left = exprNode(en_assign, left, right);
    }
    st = stmtNode(NULL,b, st_expr);
    optimize_for_constants(&left);
    st->select = left;
}
static void genAsnCall(BLOCKDATA *b, SYMBOL *cls, SYMBOL *base, int offset, EXPRESSION *thisptr, EXPRESSION *other, BOOL move, BOOL isconst)
{
    EXPRESSION *exp = NULL;
    STATEMENT *st;
    FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
    TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
    SYMBOL *asn1;
    SYMBOL *cons = search(overloadNameTab[assign - kw_new + CI_NEW], basetype(base->tp)->syms);
    EXPRESSION *left = exprNode(en_add, thisptr, intNode(en_c_i, offset));
    EXPRESSION *right = exprNode(en_add, other, intNode(en_c_i, offset));
    if (move)
    {
        right = exprNode(en_not_lvalue, right, NULL);
    }
    if (isconst)
    {
        tp->type = bt_const;
        tp->size = base->tp->size;
        tp->btp = base->tp;
    }
    else
    {
        tp = base->tp;
    }
    params->arguments = (ARGLIST *)Alloc(sizeof(ARGLIST));
    params->arguments->tp = tp;
    params->arguments->exp = right;
    AdjustParams(basetype(cons->tp)->syms->table[0], &params->arguments, FALSE);
    params->thisptr = left;
    params->thistp = Alloc(sizeof(TYPE));
    params->thistp->type = bt_pointer;
    params->thistp->size = getSize(bt_pointer);
    params->thistp->btp = base->tp;
    asn1 = GetOverloadedFunction(&tp, &params->fcall, cons, params, NULL, TRUE, FALSE);
        
    if (asn1)
    {
        if (!isAccessible(base,base, asn1, NULL, ac_protected, FALSE))
        {
            errorsym(ERR_CANNOT_ACCESS, asn1);
        }
        if (asn1->defaulted && !asn1->inlineFunc.stmt)
            createAssignment(base, asn1);
        params->functp = asn1->tp;
        params->sp = asn1;
        params->ascall = TRUE;
        /*
        if (asn1->linkage == lk_inline)
        {
            exp = doinline(params, asn1);
        }
        else
        */
        {
            exp = Alloc(sizeof(EXPRESSION));
            exp->type = en_func;
            exp->v.func = params;
            asn1->genreffed = TRUE;
        }
    }
    st = stmtNode(NULL,b, st_expr);
    optimize_for_constants(&exp);
    st->select = exp;
}
static void thunkAssignments(BLOCKDATA *b, SYMBOL *sym, SYMBOL *asnfunc, HASHTABLE *syms, BOOL move, BOOL isconst)
{
    HASHREC *hr = syms->table[0];
    EXPRESSION *thisptr = varNode(en_auto, (SYMBOL *)hr->p);
    EXPRESSION *other = NULL;
    BASECLASS *base;
    if (hr->next) // this had better be true
        other = varNode(en_auto, (SYMBOL *)hr->next->p);
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
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (sp->storage_class == sc_member && sp->tp->type != bt_aggregate)
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
}
void createAssignment(SYMBOL *sym, SYMBOL *asnfunc)
{
    // if we get here we are just assuming it is a builtin assignment operator
    // because we only get here for 'default' functions and that is the only one
    // that can be defaulted...
    HASHTABLE *syms;
    BLOCKDATA b;
    BOOL move = basetype(((SYMBOL *)basetype(asnfunc->tp)->syms->table[0]->next->p)->tp)->type == bt_rref;
    BOOL isConst = isconst(((SYMBOL *)basetype(asnfunc->tp)->syms->table[0]->next->p)->tp);
    memset(&b, 0, sizeof(BLOCKDATA));
    b.type = begin;
    syms = localNameSpace->syms;
    localNameSpace->syms = basetype(asnfunc->tp)->syms;
    thunkAssignments(&b, sym, asnfunc, basetype(asnfunc->tp)->syms, move, isConst);
    asnfunc->inlineFunc.stmt = stmtNode(NULL,NULL, st_block);
    asnfunc->inlineFunc.stmt->lower = b.head;
//    asnfunc->inlineFunc.stmt->blockTail = b.tail;
    InsertInline(asnfunc);
    localNameSpace->syms = syms;
}
static void genDestructorCall(BLOCKDATA *b, SYMBOL *sp, EXPRESSION *base, int offset, BOOL top)
{
    SYMBOL *dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    EXPRESSION *exp = exprNode(en_add, base, intNode(en_c_i, offset)) ;
    STATEMENT *st;
    deref(&stdpointer, &exp);
    dest = (SYMBOL *)basetype(dest->tp)->syms->table[0]->p;
    if (dest->defaulted && !dest->inlineFunc.stmt)
    {
        createDestructor(sp);
    }
    callDestructor(sp, &exp, NULL, top);
    st = stmtNode(NULL,b, st_expr);
    optimize_for_constants(&exp);
    st->select = exp;
}
static void undoVars(BLOCKDATA *b, HASHREC *vars, EXPRESSION *base)
{
	if (vars)
	{
		SYMBOL *s = (SYMBOL *)vars->p;
		undoVars(b, vars->next, base);
		if (s->storage_class == sc_member && isstructured(s->tp))
			genDestructorCall(b, (SYMBOL *)basetype(s->tp)->sp, base, s->offset, TRUE);
	}
}
static void undoBases(BLOCKDATA *b, BASECLASS *bc, EXPRESSION *base)
{
	if (bc)
	{
		undoBases(b, bc->next, base);
		if (!bc->isvirtual)
		{
			genDestructorCall(b, bc->cls, base, bc->offset, FALSE);
		}
	}
}
void thunkDestructorTail(BLOCKDATA *b, SYMBOL *sp, SYMBOL *dest, HASHTABLE *syms)
{
    EXPRESSION *thisptr;
    VBASEENTRY *vbe = sp->vbaseEntries;    
    thisptr = varNode(en_auto, (SYMBOL *)syms->table[0]->p);
    undoVars(b, basetype(sp->tp)->syms->table[0], thisptr);
    undoBases(b, sp->baseClasses, thisptr);
    if (vbe)
    {
        SYMBOL *sp = makeID(sc_parameter, &stdint, NULL, "__$$desttop");
        EXPRESSION *val = varNode(en_auto, sp);
        int lbl = beGetLabel;
        STATEMENT *st;
        sp->decoratedName = sp->errname = sp->name;
        sp->offset = chosenAssembler->arch->retblocksize + getSize(bt_pointer);
        insert(sp, localNameSpace->syms);
        deref(&stdint, &val);
        st = stmtNode(NULL,b, st_notselect);
        optimize_for_constants(&val);
        st->select = val;
        st->label = lbl;
        while (vbe)
        {
            if (vbe->alloc)
                genDestructorCall(b, vbe->cls, thisptr, vbe->structOffset, FALSE);
            vbe = vbe->next;
        }
        st = stmtNode(NULL,b, st_label);
        st->label = lbl;
    }
}
static void createDestructor(SYMBOL *sp)
{
    HASHTABLE *syms;
    SYMBOL *dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    BLOCKDATA b;
    memset(&b, 0, sizeof(BLOCKDATA));
    b.type = begin;
    dest = (SYMBOL *)basetype(dest->tp)->syms->table[0]->p;
    syms = localNameSpace->syms;
    localNameSpace->syms = basetype(dest->tp)->syms;
    thunkDestructorTail(&b, sp, dest, basetype(dest->tp)->syms);
    dest->inlineFunc.stmt = stmtNode(NULL,NULL, st_block);
    dest->inlineFunc.stmt->lower = b.head;
//    dest->inlineFunc.stmt->blockTail = b.tail;
    InsertInline(dest);
    localNameSpace->syms = syms;
}
static void makeArrayConsDest(TYPE **tp, EXPRESSION **exp, SYMBOL *func, BOOL forward, EXPRESSION *count)
{
    EXPRESSION *size = intNode(en_c_i, (*tp)->size + (*tp)->arraySkew);
    EXPRESSION *efunc = varNode(en_pc, func);
    FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
    SYMBOL *asn1;
    ARGLIST *arg1 = (ARGLIST *)Alloc(sizeof(ARGLIST)); // func
    ARGLIST *arg2 = (ARGLIST *)Alloc(sizeof(ARGLIST)); // size
    ARGLIST *arg3 = (ARGLIST *)Alloc(sizeof(ARGLIST)); // count
    ARGLIST *arg4 = (ARGLIST *)Alloc(sizeof(ARGLIST)); // forward
    SYMBOL *ovl = search("__arrCall", globalNameSpace->syms);
    params->arguments = arg1;
    arg1->next = arg2;
    arg2->next = arg3;
    arg3->next = arg4;
    
    arg1->exp = efunc;
    arg1->tp = &stdpointer;
    arg2->exp = size;
    arg2->tp = &stdint;
    arg3->exp = count;
    arg3->tp = &stdint;
    arg4->exp = intNode(en_c_i, forward);
    arg4->tp = &stdint;
    
    asn1 = GetOverloadedFunction(tp, &params->fcall, ovl, params, NULL, TRUE, FALSE);
    if (!asn1)
    {
        diag("makeArrayConsDest: Can't call array iterator");
    }
    else
    {
        asn1->genreffed = TRUE;
    }
    
}
void callDestructor(SYMBOL *sp, EXPRESSION **exp, EXPRESSION *arrayElms, BOOL top)
{
    SYMBOL *dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
    SYMBOL *dest1;
    SYMBOL *against = top ? sp : sp->parentClass;
    TYPE *tp = NULL;
    FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
    if (!*exp)
    {
        diag("callDestructor: no this pointer");
    }
    params->thisptr= *exp;
    params->thistp = Alloc(sizeof(TYPE));
    params->thistp->type = bt_pointer;
    params->thistp->size = getSize(bt_pointer);
    params->thistp->btp = sp->tp;
    dest1 = GetOverloadedFunction(&tp, &params->fcall, dest, params, NULL, TRUE, FALSE);
    if (dest1 && !isAccessible(against,sp, dest1, NULL, against == sp ? ac_public : ac_protected, FALSE))
    {
        errorsym(ERR_CANNOT_ACCESS, dest1);
    }
    if (dest1 && dest1->defaulted && !dest1->inlineFunc.stmt)
        createDestructor(sp);
    params->functp = dest1->tp;
    params->sp = dest1;
    params->ascall = TRUE;
    if (arrayElms)
    {
        makeArrayConsDest(&tp, exp, dest1, FALSE, arrayElms);
        dest1->genreffed = TRUE;
    }
    /*
    else if (dest1->linkage == lk_inline)
    {
        EXPRESSION *e1;
        if (sp->vbaseEntries)
        {
            ARGLIST *x = (ARGLIST *)Alloc(sizeof(ARGLIST)), **p;
            x->tp = (TYPE *)Alloc(sizeof(TYPE));
            x->tp->type = bt_int;
            x->tp->size = getSize(bt_int);
            x->exp = intNode(en_c_i, top);
            p = &params->arguments;
            while (*p) p = &(*p)->next;
            *p = x;
        }
        e1 = doinline(params, dest1);
        if (e1)
            *exp = e1;
    }
        */
    else
    {
        *exp = Alloc(sizeof(EXPRESSION));
        (*exp)->type = en_func;
        (*exp)->v.func = params;
        dest1->genreffed = TRUE;
    }
}
BOOL callConstructor(TYPE **tp, EXPRESSION **exp, FUNCTIONCALL *params, 
                     BOOL checkcopy, EXPRESSION *arrayElms, BOOL top, 
                     BOOL maybeConversion)
{
    SYMBOL *sp = basetype(*tp)->sp;
    SYMBOL *against = top ? sp : sp->parentClass;
    SYMBOL *cons = search(overloadNameTab[CI_CONSTRUCTOR], basetype(sp->tp)->syms);
    SYMBOL *cons1;
    EXPRESSION *e1 = NULL, *e2 = NULL;
    /*
    if (checkcopy)
    {
        SYMBOL *copy = getCopyCons(sp, TRUE);
        SYMBOL *dest = search(overloadNameTab[CI_DESTRUCTOR], basetype(sp->tp)->syms);
        dest = (SYMBOL *)basetype(dest->tp)->syms->table[0]->p;
        if (!copy || !isAccessible(sp,sp, copy, NULL, ac_protected, FALSE) || copy->deleted)
        {
            errorsym(ERR_CANNOT_ACCESS, copy);
        }
        if (!dest || !isAccessible(sp,sp, dest, NULL, ac_protected, FALSE) || dest->deleted)
        {
            errorsym(ERR_CANNOT_ACCESS, dest);
        }
    }
    */
    /*
    if (params && params->arguments && !params->arguments->next &&
        params->arguments->exp->type == en_func)
    {
        // might be a function returning the primary type, in which case we can elide
        // the constructor and just call the function with an appropriate
        if (params->arguments->exp->v.func->returnSP && 
            basetype(params->arguments->exp->v.func->returnSP->tp)->sp == sp)
        {
            EXPRESSION *e1 = params->arguments->exp;
            params = e1->v.func;
            params->returnEXP = *exp;
            *exp = e1;
            return TRUE;
        }
    }
    */
    if (!params)
    {
        params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
    }
    params->thisptr = *exp;
    params->thistp = Alloc(sizeof(TYPE));
    params->thistp->type = bt_pointer;
    params->thistp->btp = sp->tp;
    params->thistp->size = getSize(bt_pointer);
    cons1 = GetOverloadedFunction(tp, &params->fcall, cons, params, NULL, TRUE, 
                                  maybeConversion);
        
    if (cons1)
    {
        if (cons1->castoperator)
        {
            FUNCTIONCALL *oparams = Alloc(sizeof(FUNCTIONCALL));
            if (!isAccessible(cons1->parentClass,cons1->parentClass, cons1, NULL, ac_public, FALSE))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            oparams->fcall = params->fcall;
            oparams->thisptr = params->arguments->exp;
            oparams->thistp = Alloc(sizeof(TYPE));
            oparams->thistp->type = bt_pointer;
            oparams->thistp->size = getSize(bt_pointer);
            oparams->thistp->btp = cons1->parentClass->tp;
            oparams->functp = cons1->tp;
            oparams->sp = cons1;
            oparams->ascall = TRUE;
            if (!isref(basetype(cons1->tp)->btp))
            {
                optimize_for_constants(exp);
                oparams->returnEXP = *exp;
                oparams->returnSP = sp;
            }
            /*
            if (cons1->linkage == lk_inline)  
            {
                e1 = doinline(oparams, cons1);
            }
            else
            */
            {
                e1 = Alloc(sizeof(EXPRESSION));
                e1->type = en_func;
                e1->v.func = oparams;
                cons1->genreffed = TRUE;
            }
            /*
            if (!isref(basetype(cons1->tp)->btp))
            {
                cons1 = NULL;
            }
            else
            {
                e1 = exprNode(en_blockassign, *exp, e1);
                e1->size = sp->tp->size;
            }
            */
        }
        else 
        {
            if (!isAccessible(against,sp, cons1, NULL, against == sp ? ac_public : ac_protected, FALSE))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            AdjustParams(basetype(cons1->tp)->syms->table[0], &params->arguments, FALSE);
            params->functp = cons1->tp;
            params->sp = cons1;
            params->ascall = TRUE;
            if (cons1->defaulted && !cons1->inlineFunc.stmt)
                createConstructor(sp, cons1);
            if (arrayElms)
            {
                makeArrayConsDest(tp, exp, cons1, TRUE, arrayElms);
                cons1->genreffed = TRUE;
            }
            /*
            else if (cons1->linkage == lk_inline)
            {
                if (sp->vbaseEntries)
                {
                    ARGLIST *x = (ARGLIST *)Alloc(sizeof(ARGLIST)), **p;
                    x->tp = (TYPE *)Alloc(sizeof(TYPE));
                    x->tp->type = bt_int;
                    x->tp->size = getSize(bt_int);
                    x->exp = intNode(en_c_i, top);
                    p = &params->arguments;
                    while (*p) p = &(*p)->next;
                    *p = x;
                }
                e1 = doinline(params, cons1);
            }
            */
            else
            {
                e1 = Alloc(sizeof(EXPRESSION));
                e1->type = en_func;
                e1->v.func = params;
                cons1->genreffed = TRUE;
            }
        }
        *exp = e1;
        return TRUE;
    }
    return FALSE;
}
