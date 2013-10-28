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
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern INCLUDES *includes;
extern char *overloadNameTab[];
extern char *overloadXlateTab[];
extern int nextLabel;
extern enum e_kw skim_end[];
extern enum e_kw skim_closepa[];
extern enum e_kw skim_semi_declare[];
extern enum e_kw skim_comma[];
extern TYPE stdint;
extern TYPE stdpointer;
extern char infile[256];
extern int total_errors;
extern LIST *structSyms;
extern TYPE stdvoid;
extern int currentErrorLine;

LIST *nameSpaceList;
char anonymousNameSpaceName[512];

static LIST *deferredBackfill;

typedef struct 
{
    VTABENTRY *entry;
    SYMBOL *func;
    SYMBOL *name;
} THUNK;
static int dumpVTabEntries(int count, THUNK *thunks, SYMBOL *sym, VTABENTRY *entry)
{
#ifndef PARSER_ONLY    
    while (entry)
    {
        if (!entry->isdead)
        {
            VIRTUALFUNC *vf = entry->virtuals;
            genaddress(entry->dataOffset);
            genaddress(entry->vtabOffset);
            while (vf)
            {
                if (sym == vf->func->parentClass && entry->vtabOffset)
                {
                    char buf[512];
                    SYMBOL *localsp;
                    strcpy(buf, sym->decoratedName);
                    sprintf(buf + strlen(buf), "_$%c%d", count%26 + 'A', count / 26);
                        
                    thunks[count].entry = entry;
                    thunks[count].func = vf->func;
                    thunks[count].name = localsp = makeID(sc_static, &stdvoid, NULL, litlate(buf));
                    localsp->decoratedName = localsp->errname = localsp->name;
                    genref(localsp, 0);
                    count++;
                }
                else
                {
                    genref(vf->func, 0);
                }
                vf = vf->next;
            }
        }
        count = dumpVTabEntries(count, thunks, sym, entry->children);
        entry = entry->next;
    }
#else
    count  =0 ;
#endif
    return count;
}
void dumpVTab(SYMBOL *sym)
{
#ifndef PARSER_ONLY

    char buf[256];
    THUNK thunks[1000];
    SYMBOL *localsp;
    SYMBOL *xtSym = RTTIDumpType(sym);
    int count = 0;

    dseg();
    gen_virtual(sym->vtabsp, TRUE);
    if (xtSym)
        genref(xtSym, NULL);
    else
        genaddress(0);
    count = dumpVTabEntries(count, thunks, sym, sym->vtabEntries);
    gen_endvirtual(sym->vtabsp);

    if (count)
    {
        int i;
        strcpy(buf, sym->decoratedName);
        strcat(buf, "_$vtt");
        localsp = makeID(sc_static, &stdvoid, NULL, litlate(buf));
        localsp->decoratedName = localsp->errname = localsp->name;
        cseg();
        gen_virtual(localsp, FALSE);
        for (i=0; i < count; i++)
        {
            gen_vtt(thunks[i].entry, thunks[i].func, thunks[i].name);
        }
        gen_endvirtual(localsp);
    }
#endif
}
// will return 0 for not a base class, 1 for unambiguous base class, >1 for ambiguous base class
int classRefCount(SYMBOL *base, SYMBOL *derived)
{
    int rv = base == derived;
    if (!rv)
    {
        BASECLASS *lst = derived->baseClasses;
        while(lst)
        {
            if (lst->cls == base)
            {
                return 1;
            }
            rv += classRefCount(base, lst->cls);
            lst = lst->next;
        }
    }
    return rv;
}
static BOOL vfMatch(SYMBOL *sym, SYMBOL *oldFunc, SYMBOL *newFunc)
{
    BOOL rv = FALSE;
    char *p = strrchr(oldFunc->decoratedName, '@');
    char *q = strrchr(newFunc->decoratedName, '@');
    rv = !strcmp(p, q);
    if (rv)
    {
        if (!comparetypes(oldFunc->tp->btp, newFunc->tp->btp, TRUE))
        {
            TYPE *tp1 = oldFunc->tp->btp, *tp2 = newFunc->tp->btp;
            if (tp1->type == tp2->type)
            {
                if (ispointer(tp1) || isref(tp1))
                {
                    if (isconst(tp1) == isconst(tp2) && isvolatile(tp1) == isvolatile(tp2))
                    {
                        tp1 = basetype(tp1)->btp;
                        tp2 = basetype(tp2)->btp;
                        if (isstructured(tp1) && isstructured(tp2))
                        {
                            if ((!isconst(tp1) || isconst(tp2)) && (!isvolatile(tp1) || isvolatile(tp2)))
                            {
                                if (tp1->sp != tp2->sp && tp2->sp != sym && classRefCount(tp1->sp, tp2->sp) != 1)
                                {
                                    errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, oldFunc->parentClass, newFunc->parentClass);
                                }
                            }
                            else
                                errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
                        }
                        else
                            errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
                    }
                    else
                        errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
                }
                else
                    errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
            }
            else
                errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
        }
    }
    return rv;
}
static BOOL backpatchVirtualFunc(SYMBOL *sym, VTABENTRY *entry, SYMBOL *func)
{
    BOOL rv = FALSE;
    while (entry)
    {
        VIRTUALFUNC *vf = entry->virtuals;
        while (vf)
        { 
            if (vfMatch(sym, vf->func, func))
            {
                if (vf->func->isfinal)
                    errorsym(ERR_OVERRIDING_FINAL_VIRTUAL_FUNC, vf->func);
                if (vf->func->deleted != func->deleted)
                    errorsym2(ERR_DELETED_VIRTUAL_FUNC, vf->func->deleted ? vf->func : func, vf->func->deleted ? func : vf->func);
                    
                vf->func = func;
                rv = TRUE;
            }
            vf = vf->next;
        }
        backpatchVirtualFunc(sym, entry->children, func);
        entry = entry->next;
    }
    return rv;
}
static int allocVTabSpace(VTABENTRY *vtab, int offset)
{
    while (vtab)
    {
        VIRTUALFUNC *vf;
        if (!vtab->isdead)
        {
            vtab->vtabOffset = offset;
            vf = vtab->virtuals;
            while (vf)
            {
                offset += getSize(bt_pointer);
                vf = vf->next;
            }
            offset += 2 * getSize(bt_pointer);
        }
        offset = allocVTabSpace(vtab->children, offset);
        vtab = vtab->next;
    }
    return offset;
}
static void copyVTabEntries(VTABENTRY *lst, VTABENTRY **pos, int offset, BOOL isvirtual)
{
    while (lst)
    {
        VTABENTRY *vt = (VTABENTRY *)Alloc(sizeof(VTABENTRY));
        VIRTUALFUNC *vf, **vfc;
        vt->cls = lst->cls;
        vt->isvirtual = lst->isvirtual;
        vt->isdead = vt->isvirtual || lst->dataOffset == 0 || isvirtual;
        vt->dataOffset = offset + lst->dataOffset;
        *pos = vt;
        vf = lst->virtuals;
        vfc = &vt->virtuals;
        while (vf)
        {
            *vfc = Alloc(sizeof(VIRTUALFUNC));
            (*vfc)->func = vf->func;
            vfc = &(*vfc)->next;
            vf = vf->next;
        }
        if (lst->children)
            copyVTabEntries(lst->children, &vt->children, offset + lst->dataOffset, vt->isvirtual || isvirtual || lst->dataOffset == lst->children->dataOffset );
        pos = &(*pos)->next;
        lst = lst->next;
    }
}
static void checkAmbiguousVirtualFunc(SYMBOL *sp, VTABENTRY **match, VTABENTRY *vt)
{
    while (vt)
    {
        if (sp == vt->cls)
        {
            if (!*match)
            {
                (*match) = vt;
            }
            else
            {
                VIRTUALFUNC *f1 = (*match)->virtuals;
                VIRTUALFUNC *f2 = vt->virtuals;
                while (f1 && f2)
                {
                    if (f1->func != f2->func)
                    {
                        errorsym2(ERR_AMBIGUOUS_VIRTUAL_FUNCTIONS, f1->func, f2->func);
                    }
                    f1 = f1->next;
                    f2 = f2->next;
                }
            }
        }
        else
        {
            checkAmbiguousVirtualFunc(sp, match, vt->children);
        }
        vt = vt->next;
    }
}
static void checkXT(SYMBOL *sym1, SYMBOL *sym2)
{

    if (sym1->xcMode == xc_dynamic && sym2->xcMode == xc_dynamic)
    {
        // check to see that everything in sym1 is also in sym2
        LIST *l1 = sym1->xc->xcDynamic;
        while (l1)
        {
            LIST *l2 = sym2->xc->xcDynamic;
            while (l2)
            {
                if (comparetypes((TYPE *)l2->data, (TYPE *)l1->data, TRUE) && intcmp((TYPE *)l2->data, (TYPE *)l1->data))
                    break;
                l2 = l2->next;
            }
            if (!l2)
            {
                currentErrorLine = 0;
                errorsym(ERR_EXCEPTION_SPECIFIER_AT_LEAST_AS_RESTRICTIVE, sym1);
                break;
            }
            l1 = l1->next;
        }
    }
    else if (sym1->xcMode != sym2->xcMode)
    {
        if (sym1->xcMode == xc_all && sym2->xcMode != xc_unspecified 
            || sym1->xcMode == xc_unspecified && sym2->xcMode != xc_all || sym2->xcMode == xc_none)
        {
            currentErrorLine = 0;
            errorsym(ERR_EXCEPTION_SPECIFIER_AT_LEAST_AS_RESTRICTIVE, sym1);
        }
    }
}
static void checkExceptionSpecification(SYMBOL *sp)
{
    HASHREC *hr = basetype(sp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        if (sym->storage_class == sc_overloads)
        {
            HASHREC *hr1 = basetype(sym->tp)->syms->table[0];
            while (hr1)
            {
                SYMBOL *sym1 = (SYMBOL *)hr1->p;
                if (sym1->storage_class == sc_virtual)
                {
                    BASECLASS *bc = sp->baseClasses;
                    char *f1 = strrchr(sym1->decoratedName, '@');
                    while (bc && f1)
                    {
                        SYMBOL *sym2 = search(sym->name, basetype(bc->cls->tp)->syms);
                        if (sym2)
                        {
                            HASHREC *hr3 = basetype(sym2->tp)->syms->table[0];
                            while (hr3)
                            {
                                SYMBOL *sym3 = (SYMBOL *)hr3->p;
                                char *f2 = strrchr(sym3->decoratedName, '@');
                                if (f2 && !strcmp(f1, f2))
                                {
                                    checkXT(sym1, sym3);
                                }
                                hr3 = hr3->next;
                            }
                        }
                        bc = bc->next;
                    }
                }
                hr1 = hr1->next;
            }
        }
        hr = hr->next;
    }
}
void calculateVTabEntries(SYMBOL *sp, SYMBOL *base, VTABENTRY **pos, int offset)
{
    BASECLASS *lst = base->baseClasses;
    VBASEENTRY *vb = sp->vbaseEntries;
    HASHREC *hr = sp->tp->syms->table[0];
    if (sp->hasvtab && (!lst || lst->isvirtual || !lst->cls->vtabEntries))
    {
        VTABENTRY *vt = (VTABENTRY *)Alloc(sizeof(VTABENTRY));
        vt->cls = sp;
        vt->isvirtual = FALSE;
        vt->isdead = FALSE;
        vt->dataOffset = offset;
        *pos = vt;
        pos = &(*pos)->next;
        
    }
    while (lst)
    {
        VTABENTRY *vt = (VTABENTRY *)Alloc(sizeof(VTABENTRY));
        vt->cls = lst->cls;
        vt->isvirtual = lst->isvirtual;
        vt->isdead = vt->isvirtual;
        vt->dataOffset = offset + lst->offset;
        *pos = vt;
        pos = &(*pos)->next;
        copyVTabEntries(lst->cls->vtabEntries, &vt->children, lst->offset, FALSE);
        if (vt->children)
        {
            VIRTUALFUNC *vf, **vfc;
            vf = vt->children->virtuals;
            vfc = &vt->virtuals;
            while (vf)
            {
                *vfc = Alloc(sizeof(VIRTUALFUNC));
                (*vfc)->func = vf->func;
                vfc = &(*vfc)->next;
                vf = vf->next;
            }
        }
        lst = lst->next;
    }
    while (vb)
    {
        if (vb->alloc)
        {
            VTABENTRY *vt = (VTABENTRY *)Alloc(sizeof(VTABENTRY));
            VIRTUALFUNC *vf, **vfc;
            vt->cls = vb->cls;
            vt->isvirtual = TRUE;
            vt->isdead = FALSE;
            vt->dataOffset = vb->structOffset;
            *pos = vt;
            pos = &(*pos)->next;
            if (vb->cls->vtabEntries)
            {
                vf = vb->cls->vtabEntries->virtuals;
                vfc = &vt->virtuals;
                while (vf)
                {
                    *vfc = Alloc(sizeof(VIRTUALFUNC));
                    (*vfc)->func = vf->func;
                    vfc = &(*vfc)->next;
                    vf = vf->next;
                }
            }
            copyVTabEntries(vb->cls->vtabEntries, &vt->children, sp->tp->size, FALSE);
        }
        vb = vb->next;
    }
    while (hr && sp->vtabEntries)
    {
        SYMBOL *cur = (SYMBOL *)hr->p;
        if (cur->storage_class == sc_overloads)
        {
            HASHREC *hrf = cur->tp->syms->table[0];
            while (hrf)
            {
                SYMBOL *cur = (SYMBOL *)hrf->p;
                VTABENTRY *hold, *hold2;
                BOOL found = FALSE;
                BOOL isfirst = FALSE;
                BOOL isvirt = cur->storage_class == sc_virtual;
                hold = sp->vtabEntries->next;
                hold2 = sp->vtabEntries->children;
                sp->vtabEntries->next  = NULL;
                sp->vtabEntries->children  = NULL;
                found = backpatchVirtualFunc(sp, hold, cur);
                found |= backpatchVirtualFunc(sp, hold2, cur);
                isfirst = backpatchVirtualFunc(sp, sp->vtabEntries, cur);
                isvirt |= found | isfirst;
                sp->vtabEntries->next = hold;
                sp->vtabEntries->children = hold2;
                if (isvirt)                        
                {
                    cur->storage_class = sc_virtual;
                    if (!isfirst)
                    {
                        VIRTUALFUNC **vf;
                        vf = &sp->vtabEntries->virtuals;
                        while (*vf)
                            vf = &(*vf)->next;
                        *vf = (VIRTUALFUNC *)Alloc(sizeof(VIRTUALFUNC));
                        (*vf)->func = cur;
                    }
                }
                if (cur->isoverride && !found && !isfirst)
                {
                    errorsym(ERR_FUNCTION_DOES_NOT_OVERRIDE, cur);
                }
                hrf = hrf->next;
            }
        }
        hr = hr->next;
    }
    vb = sp->vbaseEntries;
    while (vb)
    {
        if (vb->alloc)
        {
            VTABENTRY *match = NULL;
            checkAmbiguousVirtualFunc(vb->cls, &match, sp->vtabEntries);
        }
        vb = vb->next;
    }
    checkExceptionSpecification(sp);
    allocVTabSpace(sp->vtabEntries, 0);
    if (sp->vtabEntries)
    {
        VIRTUALFUNC *vf = sp->vtabEntries->virtuals;
        int ofs = 0;
        while (vf)
        {
            if (vf->func->parentClass == sp)
            {
                vf->func->offset = ofs;
            }
            ofs += getSize(bt_pointer);
            vf = vf->next;
        }
    }
}
void calculateVirtualBaseOffsets(SYMBOL *sp, SYMBOL *base, BOOL isvirtual, int offset)
{
    BASECLASS *lst = base->baseClasses;
    while (lst)
    {
        calculateVirtualBaseOffsets(sp, lst->cls, lst->isvirtual, offset + lst->offset);
        lst = lst->next;
    }
    if (isvirtual)
    {
        VBASEENTRY *vbase = (VBASEENTRY *)Alloc(sizeof(VBASEENTRY));
        VBASEENTRY **cur;
        vbase->alloc = TRUE;
        vbase->cls = base;
        vbase->pointerOffset = offset;        
        cur = &sp->vbaseEntries;
        while (*cur)
        {
            if ((*cur)->cls == vbase->cls)
            {
                vbase->alloc = FALSE;
            }
            cur = &(*cur)->next;
        }
        *cur = vbase;
    }
    if (base == sp) // at top
    {
        BASECLASS *base;
        VBASEENTRY *lst = sp->vbaseEntries, *cur;
        HASHREC *hr = sp->tp->syms->table[0];
        VTABENTRY **pos;
        // add virtual base thunks for self
        VBASEENTRY *vbase = sp->vbaseEntries;
        while (vbase)
        {
            if (vbase->alloc)
            {
                int align = getBaseAlign(bt_pointer);
                sp->structAlign = max(sp->structAlign, align );
                if (align != 1)
                {
                    int al = sp->tp->size % align;
                    if (al != 0)
                    {
                        sp->tp->size += align - al;
                    }
                }
                base = sp->baseClasses;
                while (base)
                {
                    if (base->isvirtual && base->cls == vbase->cls)
                    {
                        if (base != sp->baseClasses)
                            base->offset = sp->tp->size;
                        break;
                    }
                    base = base->next;
                }
                vbase->pointerOffset = sp->tp->size;
                sp->tp->size += getSize(bt_pointer);
            }
            vbase = vbase->next;
        }
        sp->sizeNoVirtual = sp->tp->size;
        // now add space for virtual base classes
        while (lst)
        {
            if (lst->alloc)
            {
                int align = lst->cls->structAlign;
                VTABENTRY *old = lst->cls->vtabEntries;
                sp->structAlign = max(sp->structAlign, align );
                if (align != 1)
                {
                    int al = sp->tp->size % align;
                    if (al != 0)
                    {
                        sp->tp->size += align - al;
                    }
                }
                cur = sp->vbaseEntries;
                while (cur)
                {
                    if (cur->cls == lst->cls)
                    {
                        cur->structOffset = sp->tp->size;
                    }
                    cur = cur->next;
                }
                sp->tp->size += lst->cls->sizeNoVirtual;
            }
            lst = lst->next;
        }
    }    
}
void deferredCompile(void)
{
    static int inFunc;
    if (inFunc)
        return;
    inFunc++;
    if (!theCurrentFunc)
    {
        while (deferredBackfill)
        {
            LEXEME *lex;
            SYMBOL *cur = (SYMBOL *)deferredBackfill->data;
            BOOL addedStrSym = FALSE;
            // function body
            cur->linkage = lk_inline;
            if (cur->parentClass)
            {
                LIST *l = Alloc(sizeof(LIST));
                l->data = cur->parentClass;
                l->next = structSyms;
                structSyms = l ;   
                addedStrSym = TRUE;
            }
            lex = SetAlternateLex(cur->deferredCompile);
            lex = body(lex, cur);
            SetAlternateLex(NULL);
            if (addedStrSym)
            {
                structSyms = structSyms->next;
            }
            if (cur->constexpression)
            {
                if (cur->inlineFunc.stmt->type != st_block)
                {
                    error(ERR_CONSTANT_FUNCTION_EXPECTED);
                }
                else
                {
                    STATEMENT *st1 = cur->inlineFunc.stmt->lower;
                    STATEMENT *st;
                    while (st1->type == st_varstart)
                        st1 = st1->next;
                    st = st1->lower;
                    while (st->type == st_line || st->type == st_dbgblock )
                        st = st->next;
                    if (st->type != st_return || !IsConstantExpression(st->select, TRUE))
                    {
                        st1->select = intNode(en_c_i, 0);
                        errorat(ERR_CONSTANT_FUNCTION_EXPECTED, NULL, cur->declfile, cur->declline);
                    }
                }
            }
            deferredBackfill = deferredBackfill->next;
        }
    }
    inFunc--;
}
void backFillDeferredInitializers(SYMBOL *declsym, SYMBOL *funcsp)
{
    HASHREC *hr = basetype(declsym->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL *cur = (SYMBOL *)hr->p;
        BOOL addedStrSym  = FALSE;
        if (cur->parentClass)
        {
            LIST *l = Alloc(sizeof(LIST));
            l->data = cur->parentClass;
            l->next = structSyms;
            structSyms = l ;   
            addedStrSym = TRUE;
        }
        if (cur->storage_class == sc_overloads)
        {
            HASHREC *hrf = cur->tp->syms->table[0];
            while (hrf)
            {
                cur = (SYMBOL *)hrf->p;
                if (isfunction(cur->tp))
                {
                    HASHREC *pr = basetype(cur->tp)->syms->table[0];
                    while (pr)
                    {
                        SYMBOL *cur1 = (SYMBOL *)pr->p;
                        if (cur1->storage_class != sc_parameter)
                            break;
                        if (cur1->deferredCompile)
                        {
                            // default for parameter
                            LEXEME *lex = SetAlternateLex(cur1->deferredCompile);
                            lex = initialize(lex, funcsp, cur1, sc_parameter, TRUE); /* also reserves space */
                            SetAlternateLex(NULL);
                        }
                            
                        pr = pr->next;
                    }
                    if (cur->deferredCompile)
                    {
                        LIST *func = (LIST *)Alloc(sizeof(LIST));
                        func->data = (void *)cur;
                        func->next = deferredBackfill;
                        deferredBackfill = func;
                    }
                }
                hrf = hrf->next;
            }
        }
        else if (cur->deferredCompile)
        {
            // initializer
        }
        if (addedStrSym)
        {
            structSyms = structSyms->next;
        }
        hr = hr->next;
    }
}
void warnCPPWarnings(SYMBOL *sym, BOOL localClassWarnings)
{
    HASHREC *hr = sym->tp->syms->table[0];
    hr = sym->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *cur = (SYMBOL *)hr->p;
        if (cur->storage_class == sc_static && (cur->tp->hasbits || localClassWarnings))
            errorstr(ERR_INVALID_STORAGE_CLASS, "static");
        if (sym != cur && !strcmp(sym->name,cur->name))
        {
            if (sym->hasUserCons || cur->storage_class == sc_static || cur->storage_class == sc_overloads 
                || cur->storage_class == sc_const || cur->storage_class == sc_type)
            {
                errorsym(ERR_MEMBER_SAME_NAME_AS_CLASS, sym);
                break;
            }   
        }
        if (cur->storage_class == sc_overloads)
        {
            HASHREC *hrf = cur->tp->syms->table[0];
            while (hrf)
            {
                cur = (SYMBOL *)hrf->p;
                if (localClassWarnings)
                {
                    if (isfunction(cur->tp))
                        if (!basetype(cur->tp)->sp->inlineFunc.stmt)
                            errorsym(ERR_LOCAL_CLASS_FUNCTION_NEEDS_BODY, cur);
                }
                if (cur->isfinal || cur->isoverride || cur->ispure)
                    if (cur->storage_class != sc_virtual)
                        error(ERR_SPECIFIER_VIRTUAL_FUNC);
                hrf = hrf->next;
            }
        }
        hr = hr->next;
    }
}
BOOL usesVTab(SYMBOL *sym)
{
    HASHREC *hr;
    hr = sym->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *cur = (SYMBOL *)hr->p;
        if (cur->storage_class == sc_overloads)
        {
            HASHREC *hrf = cur->tp->syms->table[0];
            while (hrf)
            {
                if (((SYMBOL *)(hrf->p))->storage_class == sc_virtual)
                    return TRUE;
                hrf = hrf->next;
            }
        }
        hr = hr->next;
    }
    return FALSE;
}
LEXEME *baseClasses(LEXEME *lex, SYMBOL *funcsp, SYMBOL *declsym, enum e_ac defaultAccess)
{
    struct _baseClass **bc = &declsym->baseClasses, *lst;
    enum e_ac currentAccess;
    BOOL isvirtual = FALSE;
    BOOL done = FALSE;
    SYMBOL *bcsym;
    currentAccess = defaultAccess;
    lex = getsym(); // past ':'
    if (basetype(declsym->tp)->type == bt_union)
        error(ERR_UNION_CANNOT_HAVE_BASE_CLASSES);
    do
    {
        if (MATCHKW(lex,classsel) || ISID(lex))
        {
            bcsym = NULL;
            lex = nestedSearch(lex, &bcsym, NULL, NULL, NULL, TRUE);
            if (bcsym)
            {
                BASECLASS *t = declsym->baseClasses;
                while (t)
                {
                    if (t->cls == bcsym)
                    {
                        errorsym(ERR_BASE_CLASS_INCLUDED_MORE_THAN_ONCE, bcsym);
                        break;
                    }
                    t = t->next;
                }
                if (basetype(bcsym->tp)->type == bt_union)
                    error(ERR_UNION_CANNOT_BE_BASE_CLASS);       
                if (bcsym->isfinal)
                    errorsym(ERR_FINAL_BASE_CLASS, bcsym);
                *bc = Alloc(sizeof(BASECLASS));
                (*bc)->accessLevel = currentAccess;
                (*bc)->isvirtual = isvirtual;
                (*bc)->cls = bcsym;
                bc = &(*bc)->next;
                currentAccess = defaultAccess;
                isvirtual = FALSE;
                // inject it in the class so we can look it up as a type later
                //
                if (!t)
                {
                    SYMBOL *injected = clonesym(bcsym);
                    injected->parentClass = declsym;
                    injected->access = currentAccess;
                    insert(injected, declsym->tp->tags);
                }
                lex = getsym();
                done = !MATCHKW(lex, comma);
                if (!done)
                    lex = getsym();
            }
            else
            {
                error(ERR_STRUCTURED_TYPE_EXPECTED);
                done = TRUE;
            }
        }
        else switch (KW(lex))
        {
            case kw_virtual:
                isvirtual = TRUE;
                lex = getsym();
                break;
            case kw_private:
                currentAccess = ac_private;
                lex = getsym();
                break;
            case kw_protected:
                currentAccess = ac_protected;
                lex = getsym();
                break;
            case kw_public:
                currentAccess = ac_public;
                lex = getsym();
                break;
            default:
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(&lex, skim_end);
                return lex;
        }
        
    } while (!done);
    lst = declsym->baseClasses;
    while (lst)
    {
        if (!isExpressionAccessible(lst->cls, NULL ,FALSE))
        {
            BOOL err = TRUE;
            BASECLASS *lst2 = declsym->baseClasses;
            while (lst2)
            {
                if (lst2 != lst)
                {
                    if (isAccessible(lst2->cls, lst2->cls, lst->cls, NULL, ac_protected, FALSE))                        
                    {
                        err = FALSE;
                        break;
                    }
                }
                lst2 = lst2->next;
            }
            if (err)
                errorsym(ERR_CANNOT_ACCESS, lst->cls);
        }
        lst = lst->next;
    }
    return lex;    
}
static BOOL classOrEnumParam(SYMBOL *param)
{
    TYPE *tp = param->tp;
    if (isref(tp))
        tp = basetype(tp)->btp;
    tp = basetype(tp);
    return isstructured(tp) || tp->type == bt_enum;
}
void checkOperatorArgs(SYMBOL *sp)
{
    TYPE *tp = sp->tp;
    if (isref(tp))
        tp = basetype(tp)->btp;
    if (!isfunction (tp))
    {
        char buf[256];
        if (sp->castoperator)
        {
            strcpy(buf, "typedef()");
        }
        else
        {
            strcpy( buf, overloadXlateTab[sp->operatorId]);
        }
        errorstr(ERR_OPERATOR_MUST_BE_FUNCTION, buf);
    }
    else
    {
        HASHREC *hr = basetype(sp->tp)->syms->table[0];
        if (!hr)
            return;
        if (structSyms) // nonstatic member
        {
            if (sp->operatorId == CI_CAST)
            {
                // needs no argument
                SYMBOL *sym = (SYMBOL *)hr->p;
                if (sym->tp->type != bt_void)
                {
                    char buf[256];
                    errortype(ERR_OPERATOR_NEEDS_NO_PARAMETERS, basetype(sp->tp)->btp, NULL);
                }
            }
            else switch((enum e_kw)(sp->operatorId - CI_NEW))
            {
                SYMBOL *sym;
                case plus:
                case minus:
                case star:
                case and:
                    // needs zero or one argument
                    if (hr && hr->next)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;
                case not:
                case compl:
                    // needs no argument
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type != bt_void)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_NO_PARAMETERS, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;            
                case divide:
                case leftshift:
                case rightshift:
                case mod:
                case eq:
                case neq:
                case lt:
                case leq:
                case gt:
                case geq:
                case land:
                case lor:
                case or:
                case uparrow:
                case comma:
                case pointstar:
                    // needs one argument
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type == bt_void || hr->next)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;
                case autoinc:
                case autodec:
                    if (hr->next)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type != bt_void && sym->tp->type != bt_int)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_PARAMETER_OF_TYPE_INT, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;
                case kw_new:
                case kw_delete:
                case kw_newa: // new[]
                case kw_dela: // delete[]
                    break;
                case assign:
                case asplus:
                case asminus:
                case astimes:
                case asdivide:
                case asmod:
                case asleftshift:
                case asrightshift:
                case asand:
                case asor:
                case asxor:
                    // needs one argument
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type == bt_void || hr->next)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;
                
                case openbr:
                    // needs one argument:
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type == bt_void || hr->next)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;
                case openpa:
                    // anything goes
                    break;
                case pointsto:
                    // needs no arguments
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type != bt_void)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_NO_PARAMETERS, overloadXlateTab[sp->operatorId]);                        
                    } else
                    {
                        TYPE *tp = basetype(sp->tp)->btp;
                        if (!ispointer(tp) && !isref(tp))
                        {
                            errorstr(ERR_OPERATOR_RETURN_REFERENCE_OR_POINTER, overloadXlateTab[sp->operatorId]);
                        }
                    }
                    break;
                case quot:
                    errorsym(ERR_OPERATOR_LITERAL_NAMESPACE_SCOPE, sp);
                    break;
                default:
                    diag("checkoperatorargs: unknown operator type");
                    break;
            }
            if (sp->storage_class != sc_member && sp->storage_class != sc_virtual)
            {
                if (sp->operatorId == CI_CAST)
                {
                    errortype(ERR_OPERATOR_NONSTATIC, basetype(sp->tp)->btp, NULL);
                }
                else
                {
                    errorstr(ERR_OPERATOR_NONSTATIC, overloadXlateTab[sp->operatorId]);
                }
            }
        }
        else
        {
            switch((enum e_kw)(sp->operatorId - CI_NEW))
            {
                SYMBOL *sym;
                case plus:
                case minus:
                case star:
                case and:
                    // needs one or two arguments, one being class type
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type == bt_void || (hr->next && hr->next->next))
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    else if (!classOrEnumParam((SYMBOL *)hr->p) && (!hr->next || !classOrEnumParam((SYMBOL *)hr->next->p)))
                    {
                        errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;                    
                case not:
                case compl:
                    // needs one arg of class or enum type
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type == bt_void || hr->next)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    else if (!classOrEnumParam((SYMBOL *)hr->p))
                    {
                        errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;
                
                case divide:
                case leftshift:
                case rightshift:
                case mod:
                case eq:
                case neq:
                case lt:
                case leq:
                case gt:
                case geq:
                case land:
                case lor:
                case or:
                case uparrow:
                case comma:
                    // needs two args, one of class or enum type
                    if (!hr || !hr->next || hr->next->next)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_TWO_PARAMETERS, overloadXlateTab[sp->operatorId]);                        
                    }
                    else if (!classOrEnumParam((SYMBOL *)hr->p) && !classOrEnumParam((SYMBOL *)(hr->next->p)))
                    {
                        errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    break;    
                case autoinc:
                case autodec:
                    // needs one or two args, first of class or enum type 
                    // if second is present int type
                    sym = (SYMBOL *)hr->p;
                    if (sym->tp->type == bt_void || (hr->next && hr->next->next))
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    else if (!classOrEnumParam((SYMBOL *)hr->p))
                    {
                        errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->operatorId]);                        
                    }
                    if (hr && hr->next)
                    {
                        sym = (SYMBOL *)(hr->next->p);
                        if (sym->tp->type != bt_int)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_SECOND_PARAMETER_OF_TYPE_INT, overloadXlateTab[sp->operatorId]);                        
                        }
                    }
                    break;
                case quot:
                    if (hr && hr->next)
                    {
                        if (hr->next->next)
                        {
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                        }
                        else
                        {
                            // two args
                            TYPE *tpl = ((SYMBOL *)hr->p)->tp;
                            TYPE *tpr = ((SYMBOL *)hr->next->p)->tp;
                            if (!isunsigned(tpr) || !ispointer(tpl))
                            {
                                errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);                                
                            }
                            else
                            {
                                tpl = basetype(tpl)->btp;
                                tpr = basetype(tpl);
                                if (!isconst(tpl) || tpr->type != bt_char && tpr->type != bt_wchar_t 
                                        && tpr->type != bt_char16_t &&tpr->type != bt_char32_t)
                                {
                                    errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                                }
                            }
                        }
                    }
                    else
                    {
                        // one arg
                        TYPE *tp = ((SYMBOL *)hr->p)->tp;
                        if ((!ispointer(tp) || !isconst(basetype(tp)->btp) || basetype(basetype(tp)->btp)->type != bt_char)
                            && tp->type != bt_unsigned_long_long && tp-> type != bt_long_double
                            && tp->type != bt_char && tp->type != bt_wchar_t 
                            && tp->type != bt_char16_t &&tp->type != bt_char32_t)
                        {
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                        }
                    }
                    break;
                case kw_new:
                case kw_newa:
                    if (hr)
                    {
                        // any number of args, but first must be a size
                        TYPE *tp = ((SYMBOL *)hr->p)->tp;
                        if (!isint(tp))
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    else
                    {
                        errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    break;
                case kw_delete:
                case kw_dela:
                    if (hr && !hr->next)
                    {
                        // one arg, must be a pointer
                        TYPE *tp = ((SYMBOL *)hr->p)->tp;
                        if (!ispointer(tp))
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    else
                    {
                        errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    break;
                default:
                    if (sp->operatorId == CI_CAST)
                    {
                        errortype(ERR_OPERATOR_MUST_BE_NONSTATIC, basetype(sp->tp)->btp, NULL);
                    }
                    else
                    {
                        errorstr(ERR_OPERATOR_MUST_BE_NONSTATIC, overloadXlateTab[sp->operatorId]);
                    }
                    break;
            }
        }
    }
}
LEXEME *handleStaticAssert(LEXEME *lex)
{
    if (!needkw(&lex, openpa))
    {
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    }
    else
    {
        BOOL v = TRUE;
        char buf[256];
        TYPE *tp;
        EXPRESSION *expr=NULL, *expr2=NULL;
        lex = expression_no_comma(lex, NULL, NULL, &tp, &expr, FALSE);
        expr2 = Alloc(sizeof(EXPRESSION));
        expr2->type = en_x_bool;
        expr2->left = expr;
           optimize_for_constants(&expr2);
            if (!isarithmeticconst(expr2))
            error(ERR_CONSTANT_VALUE_EXPECTED);
        v = expr2->v.i;
        
        if (!needkw(&lex, comma))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
        else
        {
            if (lex->type != l_astr)
            {
                error(ERR_NEEDSTRING);
            }
            else
            {
                int i;
                SLCHAR *ch = (SLCHAR *)lex->value.s.w;
                lex = getsym();
                for (i=0; i < ch->count && i < sizeof(buf)-1; i++)
                    buf[i] = ch->str[i];
                buf[i] = 0;
            }
            if (!needkw(&lex, closepa))
            {
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
            else if (!v)
            {
                errorstr(ERR_PURESTRING, buf);
            }
        }
    }
    return lex;
}
LEXEME *insertNamespace(LEXEME *lex, enum e_lk linkage, enum e_sc storage_class, BOOL *linked)
{
    BOOL anon = FALSE;
    char buf[256], *p;
    HASHREC **hr;
    SYMBOL *sp;
    LIST *list;
    *linked = FALSE;
    if (ISID(lex))
    {
        strcpy(buf, lex->value.s.a);
        lex = getsym();
        if (MATCHKW(lex, assign))
        {
            lex = getsym();
            if (ISID(lex))
            {
                char buf1[512];
                strcpy(buf1, lex->value.s.a);
                lex = nestedSearch(lex, &sp, NULL, NULL, NULL, FALSE);
                if (!sp)
                {
                    errorstr(ERR_UNDEFINED_IDENTIFIER, buf1);
                }
                else if (sp->storage_class != sc_namespace)
                {
                    errorsym(ERR_NOT_A_NAMESPACE, sp);
                }
                else
                {
                    SYMBOL *src = sp;
                    TYPE *tp;
                    HASHREC **p;
                    if (storage_class == sc_auto)
                        p = LookupName(buf, localNameSpace->syms);
                    else
                        p = LookupName(buf, globalNameSpace->syms);
                    if (p)
                    {
                        SYMBOL *sym = (SYMBOL *)(*p)->p;
                        // already exists, bug check it
                        if (sym->storage_class == sc_namespacealias && sym->nameSpaceValues->origname == src)
                        {
                            if (linkage == lk_inline)
                            {
                                error(ERR_INLINE_NOT_ALLOWED);
                            }
                            lex = getsym();
                            return lex;
                        }
                    }
                    tp = (TYPE *)Alloc(sizeof(TYPE));
                    tp->type = bt_void;
                    sp = makeID(sc_namespacealias, tp, NULL, litlate(buf));
                    SetLinkerNames(sp, lk_none);
                    if (storage_class == sc_auto)
                    {
                        insert(sp, localNameSpace->syms);
                        insert(sp, localNameSpace->tags);
                    }
                    else
                    {
                        insert(sp, globalNameSpace->syms);
                        insert(sp, globalNameSpace->tags);
                    }
                    sp->nameSpaceValues = Alloc(sizeof(NAMESPACEVALUES));
                    *sp->nameSpaceValues = *src->nameSpaceValues;
                    sp->nameSpaceValues->name = sp; // this is to rename it with the alias e.g. for errors
                }
                if (linkage == lk_inline)
                {
                    error(ERR_INLINE_NOT_ALLOWED);
                }
                lex = getsym();
            }
            else
            {
                error(ERR_EXPECTED_NAMESPACE_NAME);
            }
            return lex;
        }
    }
    else
    {
        anon = TRUE;
        if (!anonymousNameSpaceName[0])
        {
            p = strrchr(infile, '\\');
            if (!p)
            {
                p = strrchr(infile, '/');
                if (!p)
                    p = infile;
                else
                    p++;
            }
            else
                p++;
            
            sprintf(anonymousNameSpaceName, "__%s__%d", p, rand()*RAND_MAX + rand());
            while ((p = strchr(anonymousNameSpaceName, '.')) != 0)
                *p = '_';			
        }
        strcpy(buf, anonymousNameSpaceName);
    }
    if (storage_class != sc_global)
    {
        error(ERR_NO_NAMESPACE_IN_FUNCTION);
    }
    hr = LookupName(buf, globalNameSpace->syms);
    if (!hr)
    {
        TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
        tp->type = bt_void;
        sp = makeID(sc_namespace, tp, NULL, litlate(buf));
        insert(sp, globalNameSpace->syms);
        insert(sp, globalNameSpace->tags);
        sp->nameSpaceValues = Alloc(sizeof(NAMESPACEVALUES));
        sp->nameSpaceValues->syms = CreateHashTable(GLOBALHASHSIZE);
        sp->nameSpaceValues->tags = CreateHashTable(GLOBALHASHSIZE);
        sp->nameSpaceValues->origname = sp;
        sp->nameSpaceValues->name = sp;
        sp->linkage = linkage;
        SetLinkerNames(sp, lk_none);
        if (anon || linkage == lk_inline)
        {
            // plop in a using directive for the anonymous namespace we are declaring
            list = Alloc(sizeof(LIST));
            list->data = sp;
            if (linkage == lk_inline)
            {
                list->next = globalNameSpace->inlineDirectives;
                globalNameSpace->inlineDirectives = list;
            }
            else
            {
                list->next = globalNameSpace->usingDirectives;
                globalNameSpace->usingDirectives = list;
            }
        }
    }
    else
    {
        sp = (SYMBOL *)(*hr)->p;
        if (sp->storage_class != sc_namespace)
        {
            errorsym(ERR_NOT_A_NAMESPACE, sp);
            return lex;
        }
        if (linkage == lk_inline)
            if (sp->linkage != lk_inline)
                errorsym(ERR_NAMESPACE_NOT_INLINE, sp);
    }
    if (nameSpaceList)
    {
        sp->parentNameSpace = nameSpaceList->data;
    }
    
    list = Alloc(sizeof(LIST));
    list->next = globalNameSpace->childNameSpaces;
    list->data = sp;
    globalNameSpace->childNameSpaces = list;

    list = Alloc(sizeof(LIST));
    list->next = nameSpaceList;
    list->data = sp;
    nameSpaceList = list;

    sp->nameSpaceValues->next = globalNameSpace;
    globalNameSpace = sp->nameSpaceValues;

    *linked = TRUE;
    return lex;
}
void unvisitUsingDirectives(NAMESPACEVALUES *v)
{
    LIST *t = v->usingDirectives;
    while (t)
    {
        SYMBOL *sym = t->data;
        if (sym->visited)
        {
            sym->visited = FALSE;
            unvisitUsingDirectives(sym->nameSpaceValues);
        }
        t = t->next;
    }
    t = v->inlineDirectives;
    while (t)
    {
        SYMBOL *sym = t->data;
        if (sym->visited)
        {
            sym->visited = FALSE;
            unvisitUsingDirectives(sym->nameSpaceValues);
        }
        t = t->next;
    }
}
static void InsertTag(SYMBOL *sp, enum sc storage_class)
{
    HASHTABLE *table;
    if (structSyms && (storage_class == sc_member || storage_class == sc_type))
    {
        SYMBOL *ssp = (SYMBOL *)structSyms->data;
        table = ssp->tp->tags;
    }		
    else if (storage_class == sc_auto || storage_class == sc_register 
        || storage_class == sc_parameter || storage_class == sc_localstatic)
        table = localNameSpace->tags;
    else
        table = globalNameSpace->tags ;
    insert(sp, table);
}
LEXEME *insertUsing(LEXEME *lex, enum e_sc storage_class)
{
    SYMBOL *sp;
    if (MATCHKW(lex, kw_namespace))
    {
        lex = getsym();
        if (ISID(lex))
        {
            // by spec using directives match the current state of 
            // the namespace at all times... so we cache pointers to
            // related namespaces
            char buf1[512];
            HASHREC **hr;
            strcpy(buf1, lex->value.s.a);
            lex = nestedSearch(lex, &sp, NULL, NULL, NULL, FALSE);
            if (!sp)
            {
                errorstr(ERR_UNDEFINED_IDENTIFIER, buf1);
            }
            else if (sp->storage_class != sc_namespace && sp->storage_class != sc_namespacealias)
            {
                errorsym(ERR_NOT_A_NAMESPACE, sp);
            }
            else
            {
                LIST *t = globalNameSpace->usingDirectives;
                while(t)
                {
                    if (t->data == sp)
                        break;
                    t = t->next;
                }
                if (!t)
                {
                    LIST *l = Alloc(sizeof(LIST));
                    l->data = sp;
                    if (storage_class == sc_auto)
                    {
                        l->next = localNameSpace->usingDirectives;
                        localNameSpace->usingDirectives = l;
                    }
                    else
                    {
                        l->next = globalNameSpace->usingDirectives;
                        globalNameSpace->usingDirectives = l;
                    }
                }
            }
            lex = getsym();
        }
        else
        {
            error(ERR_EXPECTED_NAMESPACE_NAME);
        }
    }
    else
    {
        if (ISID(lex))
        {
            marksym();
            lex = getsym();
            if (MATCHKW(lex, assign))
            {
                TYPE *tp = NULL;
                lex = getsym();
                lex = get_type_id(lex, &tp, NULL, FALSE);
                checkauto(tp);
                return lex;
            }
            else
            {
                lex = backupsym(0);
            }
        }
        lex = nestedSearch(lex, &sp, NULL, NULL, NULL, FALSE);
        if (!sp)
        {
            error(ERR_IDENTIFIER_EXPECTED);
        }
        else
        {
            if (sp->storage_class == sc_overloads)
            {
                HASHREC **hr = sp->tp->syms->table;
                while (*hr)
                {
                    SYMBOL *sym = (SYMBOL *)(*hr)->p;
                    InsertSymbol(sym, sc_external, sym->linkage);
                    hr = &(*hr)->next;
                }
            }
            else
            {
                if (sp->storage_class == sc_type)
                    InsertTag(sp, storage_class);
                else
                    InsertSymbol(sp, storage_class, sp->linkage);
            }
            lex = getsym();
        }
        
    }
    return lex;
}
