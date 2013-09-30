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
#include "assert.h"

extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;
extern int stdpragmas;
extern enum e_kw skim_closepa[];
extern enum e_kw skim_closebr[];
extern enum e_kw skim_semi[];
extern TYPE stdpointer;
extern TYPE stdint;
extern TYPE stddouble;
extern TYPE stdvoid;
extern TYPE stdwcharptr;
extern TYPE stdcharptr;
extern TYPE stdfloatimaginary;
extern TYPE stdbool;
extern TYPE stdunsigned;
extern TYPE stdunsignedlong;
extern TYPE stdlong;
extern TYPE stdunsignedlonglong;
extern TYPE stdlonglong;
extern TYPE stdfloat;
extern TYPE stdlongdouble;
extern TYPE stdchar;
extern TYPE stdwidechar;
extern TYPE std__func__;
extern TYPE stdchar16tptr;
extern TYPE stdchar32tptr;
extern BOOL setjmp_used;
extern LIST *structSyms;
extern char *overloadNameTab[];
extern char *overloadXlateTab[];
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern LIST *structSyms;
extern SYMBOL *enumSyms;
/* lvaule */
/* handling of const int */
/*-------------------------------------------------------------------------------------------------------------------------------- */
void checkscope(TYPE *tp1, TYPE *tp2)
{
    if (tp1->scoped != tp2->scoped)
    {
        error(ERR_SCOPED_TYPE_MISMATCH);
    }
    else if (tp1->scoped && tp2->scoped)
    {
        SYMBOL *sp1=NULL, *sp2=NULL;
        if ((tp1)->type == bt_enum)
            sp1 = (tp1)->sp;
        else
            sp1 = (tp1)->btp->sp;
        if (tp2->type == bt_enum)
            sp2 = tp2->sp;
        else
            sp2 = tp2->btp->sp;
        if (sp1 != sp2)
            error(ERR_SCOPED_TYPE_MISMATCH);
    }
}
EXPRESSION *baseClassOffset(SYMBOL *base, SYMBOL *derived, EXPRESSION *en)
{
    EXPRESSION *rv = en;
    BASECLASS *lst = derived->baseClasses;
    if (base == derived)
        rv = en;
    else
    {
        while(lst)
        {
            // start by looking at all the explicit base clases
            if (lst->cls == base)
            {                
                if (lst->isvirtual)
                {
                    VBASEENTRY *p = derived->vbaseEntries;
                    while (p && lst->cls != p->cls && !p->alloc)
                        p = p->next;
                    if (p)
                    {
                        EXPRESSION *ec = intNode(en_c_i, p->pointerOffset);
                        rv = exprNode(en_add, en, ec);
                        deref(&stdpointer, &rv);
                    }
                }
                else 
                {
                    EXPRESSION *ec = intNode(en_c_i, lst->offset);
                    rv = exprNode(en_add, en, ec);
                }
                break;
            }
            lst = lst->next;
        }
        if (!lst)
        {
            // if not found, look through the virtual base entries
            VBASEENTRY *vbase = derived->vbaseEntries;
            while (vbase)
            {
                if (vbase->alloc && vbase->cls == base)
                {
                    EXPRESSION *ec = intNode(en_c_i, vbase->pointerOffset);
                    rv = exprNode(en_add, en, ec);
                    deref(&stdpointer, &rv);
                    break;
                }
                vbase = vbase->next;
            }
            
        }
    }
    return rv;
}
void qualifyForFunc(SYMBOL *sym, TYPE **tp)
{
    if (sym)
    {
        if (isconst(sym->tp))
        {
            TYPE *tp1 = Alloc(sizeof(TYPE));
            tp1->size = (*tp)->size;
            tp1->type = bt_const;
            tp1->btp = *tp;
            *tp = tp1;
        }
        if (isvolatile(sym->tp))
        {
            TYPE *tp1 = Alloc(sizeof(TYPE));
            tp1->size = (*tp)->size;
            tp1->type = bt_volatile;
            tp1->btp = *tp;
            *tp = tp1;
        }
    }
}
void getThisType(SYMBOL *sym, TYPE **tp)
{
    TYPE *n;
    *tp = Alloc(sizeof(TYPE));
    (*tp)->size = stdpointer.size;
    (*tp)->type = bt_pointer;
    (*tp)->size = getSize(bt_pointer);
    (*tp)->btp = sym->parentClass->tp;
    qualifyForFunc(sym, tp);
}
EXPRESSION *getMemberBase(SYMBOL *memberSym, SYMBOL *funcsp)
{
    EXPRESSION *en;
    if (structSyms)
    {
        SYMBOL *enclosing = (SYMBOL *)structSyms->data;
        if (funcsp)
            en = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p); // this ptr
        else
            en = intNode(en_thisshim, 0);
        deref(&stdpointer, &en);
        if (enclosing != memberSym->parentClass)
        {
            if (classRefCount(memberSym->parentClass, enclosing) != 1)
            {
                errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, memberSym->parentClass, enclosing);
            }
            else if (!isExpressionAccessible(memberSym, funcsp, FALSE))
            {
                errorsym(ERR_CANNOT_ACCESS, memberSym);
            }
            en = baseClassOffset(memberSym->parentClass, enclosing, en);
        }
    }
    else
    {
        en = intNode(en_c_i, 0);
        errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
    }
    return en;
}
EXPRESSION *getMemberNode(SYMBOL *memberSym, TYPE **tp, SYMBOL *funcsp)
{
    EXPRESSION *en = getMemberBase(memberSym, funcsp);
    en = exprNode(en_add, en, intNode(en_c_i, memberSym->offset));
    *tp = memberSym->tp;
    return en;
}
EXPRESSION *getMemberPtr(SYMBOL *memberSym, TYPE **tp, SYMBOL *funcsp)
{
    EXPRESSION *rv;
    TYPE *tpq = Alloc(sizeof(TYPE));
    tpq->type = bt_memberptr;
    tpq->btp = memberSym->tp;
    tpq->sp = memberSym->parentClass;
    *tp = tpq;
    rv = varNode(en_memberptr, memberSym);
    rv->isfunc = TRUE;
    if (!isExpressionAccessible(memberSym, funcsp, TRUE))
    {
        errorsym(ERR_CANNOT_ACCESS, memberSym);
    }
    return rv;
}
void castToArithmetic(BOOL integer, TYPE **tp, EXPRESSION **exp, enum e_kw kw, TYPE *other)
{
    if (cparams.prm_cplusplus && isstructured(*tp))
    {
        SYMBOL *sp = basetype(*tp)->sp;
        if (!other || isarithmetic(other))
        {
            SYMBOL *cst = integer ? lookupIntCast(sp, other ? other : &stdint) 
                                  : lookupArithmeticCast(sp, other ? other : &stddouble);
            if (cst)
            {
                FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
                EXPRESSION *e1;
                
                DerivedToBase(cst->parentClass->tp, *tp, exp);
                params->fcall = varNode(en_pc, cst);
                params->thisptr = *exp;
                params->thistp = Alloc(sizeof(TYPE));
                params->thistp->type = bt_pointer;
                params->thistp->btp = cst->parentClass->tp;
                params->thistp->size = getSize(bt_pointer);
                params->functp = cst->tp;
                params->sp = cst;
                params->ascall = TRUE;
                /*
                if (cons1->linkage == lk_inline)  
                {
                    e1 = doinline(params, cons1);
                }
                else
                */
                {
                    e1 = Alloc(sizeof(EXPRESSION));
                    e1->type = en_func;
                    e1->v.func = params;
                    cst->genreffed = TRUE;
                }
                *exp = e1;
                cast(other, exp);
                *tp = basetype(cst->tp)->btp;
                return;
            }
        }
        // failed at conversion
        if (kw >= kw_new && kw <= compl)
        {
            // LHS, put up an operator whatever message
            char buf[256];
            char tbuf[256];
            tbuf[0] = 0;
            typeToString(tbuf, *tp);
            if (other)
            {
                strcat(tbuf, ", ");
                typeToString(tbuf+strlen(tbuf), other);
            }
            sprintf(buf, "operator %s(%s)", overloadXlateTab[kw - kw_new + CI_NEW], tbuf);
            errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, buf);
        }
        else
        {
            // otherwise RHS, do a can't convert message
            if (!isarithmetic(other))
                other = &stdint;
            errortype(ERR_CANNOT_CONVERT_TYPE, *tp, other);
        }
    }
}
BOOL cppCast(TYPE *src, TYPE **tp, EXPRESSION **exp)
{
    if (isstructured(src))
    {
        SYMBOL *sp = basetype(src)->sp;
        SYMBOL *cst = lookupSpecificCast(sp, *tp);
        if (cst)
        {
            FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
            EXPRESSION *e1;
            DerivedToBase(cst->parentClass->tp, src, exp);
            params->fcall = varNode(en_pc, cst);
            params->thisptr = *exp;
            params->thistp = Alloc(sizeof(TYPE));
            params->thistp->type = bt_pointer;
            params->thistp->size = getSize(bt_pointer);
            params->thistp->btp = cst->parentClass->tp;
            params->functp = cst->tp;
            params->sp = cst;
            params->ascall = TRUE;
            if (isstructured(*tp))
            {
                SYMBOL *av = anonymousVar(sc_auto, *tp);
                EXPRESSION *ev = varNode(en_auto, av);
                insert(av, localNameSpace->syms);
                params->returnEXP = ev;
                params->returnSP = sp;
                callDestructor(*tp, &ev, NULL, TRUE);
                initInsert(&av->dest, *tp, exp, 0, TRUE);
            }
            /*
            if (cons1->linkage == lk_inline)  
            {
                e1 = doinline(params, cons1);
            }
            else
            */
            {
                e1 = Alloc(sizeof(EXPRESSION));
                e1->type = en_func;
                e1->v.func = params;
                cst->genreffed = TRUE;
            }
            *exp = e1;
            DerivedToBase(*tp, basetype(cst)->btp, exp);
            return TRUE;
        }
    }
    return FALSE;   
}
LEXEME *expression_func_type_cast(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    BOOL defd = FALSE;
    lex = getBasicType(lex, funcsp, tp, sc_auto, &linkage, &linkage2, &linkage3, ac_public, NULL, &defd,NULL);
    if (!MATCHKW(lex, openpa))
    {
        if (MATCHKW(lex, begin))
        {
            INITIALIZER *init = NULL, *dest=NULL;
            SYMBOL *sp = NULL;
            if (isstructured(*tp))
            {
                sp = anonymousVar(sc_auto, *tp);
                insert(sp, localNameSpace->syms);
            }
            lex = initType(lex, funcsp, NULL, sc_auto, &init, &dest, *tp, sp, FALSE);
            *exp = convertInitToExpression(*tp, NULL, funcsp, init, NULL);
        }
        else
        {
            *exp = intNode(en_c_i, 0);
            needkw(&lex, openpa);
            errskim(&lex, skim_semi);
        }
    }
    else
    {
        if (isstructured(*tp))
        {
            TYPE *ctype = *tp;
            SYMBOL *sp;
            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL)); 
            lex = getArgs(lex, funcsp, &funcparams);
            sp = anonymousVar(sc_auto, *tp);
            insert(sp, localNameSpace->syms);
            *exp = varNode(en_auto, sp);
            callConstructor(&ctype, exp, funcparams, FALSE, NULL, TRUE, TRUE); 
        }
        else
        {
            TYPE *throwaway;
            lex = expression_unary(lex, funcsp, NULL, &throwaway, exp, FALSE);
            if ((*exp)->type == en_func)
                *exp = (*exp)->v.func->fcall;
            if (throwaway)
                if (isvoid(throwaway) && !isvoid(*tp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if ((isstructured(throwaway) && !isvoid(*tp) || basetype(throwaway)->type == bt_memberptr || basetype(*tp)->type == bt_memberptr)
                          && !comparetypes(throwaway, *tp, TRUE))
                    error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                else
                    cast(*tp, exp);
        }
    }
    return lex;
}
BOOL doDynamicCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp)
{
    return FALSE;
}
BOOL doStaticCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp, BOOL checkconst)
{
    // no change or stricter const qualification
    if (comparetypes(*newType, oldType, TRUE))
        return TRUE;
    // conversion to or from void pointer
    if (((isvoidptr(*newType) && ispointer(oldType))
        || (isvoidptr(oldType)||(*exp)->type == en_nullptr) && ispointer(*newType)) 
        && (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp)))
        return TRUE;
    // conversion to void (discards type)
    if (isvoid(*newType))
        return TRUE;
    // conversion of one numeric value to another
    if (isarithmetic(*newType) && isarithmetic(oldType))
    {
        cast(*newType, exp);
        return TRUE;
    }
    // floating to enumeration
    if (isfloat(oldType) && basetype(*newType)->type == bt_enum)
    {
        cast(basetype(*newType)->btp, exp);
        return TRUE;
    }
    // int to enum
    if (isint(oldType) && basetype(*newType)->type == bt_enum)
    {
        if (basetype(oldType)->type != basetype(*newType)->btp->type)
            cast(basetype(*newType)->btp, exp);
        return TRUE;
        
    }
    // enum to int
    if (isint(*newType) && basetype(oldType)->type == bt_enum)
    {
        if (basetype(*newType)->type != basetype(oldType)->btp->type)
            cast(*newType, exp);
        return TRUE;
        
    }
    // base to derived pointer or derived to base pointer
    if (ispointer(*newType) && ispointer(oldType))
    {
        TYPE *tpo = basetype(oldType)->btp;
        TYPE *tpn = basetype(*newType)->btp;
        if ((!checkconst || isconst(tpn) || !isconst(tpo)) && (isstructured(tpn) && isstructured(tpo)))
        {
            int n = classRefCount(tpn->sp, tpo->sp);
            if (n == 1)
            {
                // derived to base
                EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                v->type = en_c_i;
                v = baseClassOffset(tpn->sp, tpo->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i) // check for no virtual base
                {
                    if (isAccessible(tpo->sp, tpo->sp, tpn->sp, funcsp, ac_public, FALSE))
                    {
                        *exp = exprNode(en_add, *exp, v);
                        return TRUE;
                    }
                }
            }
            else if (!n && classRefCount(tpo->sp, tpn->sp) == 1)
            {
                // base to derived
                EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                v->type = en_c_i;
                v = baseClassOffset(tpo->sp, tpn->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i) // check for no virtual base
                {
                    if (isAccessible(tpn->sp, tpn->sp, tpo->sp, funcsp, ac_public, FALSE))
                    {
                        *exp = exprNode(en_sub, *exp, v);
                        return TRUE;
                    }
                }
            }
        }
    }
    // base to derived reference or derived to base reference
    if (isref(*newType))
    {
        TYPE *tpo = oldType;
        TYPE *tpn = basetype(*newType)->btp;
        if ((!checkconst || isconst(tpn) || !isconst(tpo)) && (isstructured(tpn) && isstructured(tpo)))
        {
            int n = classRefCount(tpn->sp, tpo->sp);
            if (n == 1)
            {
                // derived to base
                EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                v->type = en_c_i;
                v = baseClassOffset(tpn->sp, tpo->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i) // check for no virtual base
                {
                    if (isAccessible(tpo->sp, tpo->sp, tpn->sp, funcsp, ac_public, FALSE))
                    {
                        *exp = exprNode(en_add, *exp, v);
                        *newType = tpn;
                        return TRUE;
                    }
                }
            }
            else if (!n && classRefCount(tpo->sp, tpn->sp) == 1)
            {
                // base to derived
                EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                v->type = en_c_i;
                v = baseClassOffset(tpo->sp, tpn->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i) // check for no virtual base
                {
                    if (isAccessible(tpn->sp, tpn->sp, tpo->sp, funcsp, ac_public, FALSE))
                    {
                        *exp = exprNode(en_sub, *exp, v);
                        *newType = tpn;
                        return TRUE;
                    }
                }
            }
        }
    }
    // pointer to member derived to pointer to member base
    if (basetype(*newType)->type == bt_memberptr && basetype(oldType)->type == bt_memberptr)
    {
        if (classRefCount(basetype(*newType)->sp, basetype(oldType)->sp) == 1)
        {
            if (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp))
            {
                int vbo = basetype(oldType)->sp->vbaseEntries != 0;
                int vbn = basetype(*newType)->sp->vbaseEntries != 0;
                // can't have just new one being virtual...
                if (vbo && vbn || !vbn)
                {
                    if (isAccessible(basetype(oldType)->sp, basetype(oldType)->sp, basetype(*newType)->sp, funcsp, ac_public, FALSE))
                        return TRUE;
                }
            }
        }
    }
    return FALSE;
    // e to T:  T t(e) for an invented temp t.
}
BOOL doConstCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp)
{
    // as long as the types match except for any changes to const and 
    // it is not a function or memberptr we can convert it.
    if (comparetypes(*newType, oldType, 2))
    {
        if (!isfunction(oldType) && basetype(oldType)->type != bt_memberptr)
            return TRUE;
    }
    return FALSE;
}
BOOL doReinterpretCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp, BOOL checkconst)
{
    // int to pointer
    if (ispointer(*newType) && isint(oldType))
    {
        cast(*newType, exp);
        return TRUE;
    }
    // enum to pointer
    if (ispointer(*newType) && basetype(oldType)->type == bt_enum)
    {
        cast(*newType, exp);
        return TRUE;
    }
    // change type of nullptr
    if ((*exp)->type == en_nullptr)
    {
        return TRUE;
    }
    // pointer to int
    if (ispointer(oldType) && isint(*newType))
    {
        if ((*newType)->size >= oldType->size)
        {
            cast(*newType, exp);
            return TRUE;
        }
    }
    // one function pointer type to another
    if (isfuncptr(oldType) && isfuncptr(*newType))
    {
        if (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp))
        {
            return TRUE;
        }
    }
    // pointer to object to pointer to object (standard layout types, with ok alignment)
    if (ispointer(*newType) && ispointer(oldType))
    {
        TYPE *tpo = basetype(oldType)->btp;
        TYPE *tpn = basetype(*newType)->btp;
        if (!checkconst || isconst(tpn) || !isconst(tpo)) 
        {
            SYMBOL *spo = basetype(tpo)->sp;
            SYMBOL *spn = basetype(tpn)->sp;
            if (!isstructured(tpo) || !spo->hasvtab && !spo->accessspecified && !spo->baseClasses)
            {
                if (!isstructured(tpn) || !spn->hasvtab && !spn->accessspecified && !spn->baseClasses)
                {
                    // new alignment has to be the same or more restrictive than old
                    if (basetype(tpn)->alignment >= basetype(tpo)->alignment)
                        return TRUE;
                }
            }
        }
    }
    // convert one member pointer to another
    if (basetype(*newType)->type == bt_memberptr)
        if ((*exp)->type == en_nullptr) // catch nullptr...
        {
            return TRUE;
        }
        else if (basetype(oldType)->type == bt_memberptr)
        {
            // this suffices to check that both are vars or both are functions in this implementation
            if ((*newType)->size == oldType->size) 
                if (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp))
                    return TRUE;
        }
    // conversion to reference
    // the deal is, if T1 is a reference and a pointer to T2 can be cast to a pointer to T1
    // using this methodology, we change the type of the value without any kind of cast operation
    // occurring...
    if (isref(*newType))
    {
        TYPE *nt2 = basetype(*newType)->btp;
        TYPE *tpo = oldType;
        TYPE *tpn = nt2;
        if (!isfuncptr(tpn) && !isfuncptr(tpo))
        {
            if (!checkconst || isconst(tpn) || !isconst(tpo)) 
            {
                SYMBOL *spo = basetype(tpo)->sp;
                SYMBOL *spn = basetype(tpn)->sp;
                if (!isstructured(tpo) || !spo->hasvtab && !spo->accessspecified && !spo->baseClasses)
                {
                    if (!isstructured(tpn) || !spn->hasvtab && !spn->accessspecified && !spn->baseClasses)
                    {
                        // new alignment has to be the same or more restrictive than old
                        if (basetype(tpn)->alignment >= basetype(tpo)->alignment)
                        {
                            *newType = nt2;
                            return TRUE;
                        }
                    }
                }
            }
        }
    }
    return FALSE;
    // fixme nullptr conversion to nullptr
}
LEXEME *GetCastInfo(LEXEME *lex, SYMBOL *funcsp, TYPE **newType, TYPE **oldType, EXPRESSION **oldExp)
{
    lex = getsym();
    if (needkw(&lex, lt))
    {
        lex = get_type_id(lex, newType, funcsp, FALSE);
        if (needkw(&lex, gt))
        {
            if (needkw(&lex, openpa))
            {
                lex = expression(lex, funcsp, NULL, oldType, oldExp, FALSE);
                if (!needkw(&lex, closepa))
                {
                    errskim(&lex, skim_closepa);
                }
            }
            else
            {
                   errskim(&lex, skim_closepa);
            }
        }
    } 
    if (!*oldExp)
        *oldExp = intNode(en_c_i, 0);
    if (!*oldType)
        *oldType = &stdvoid;
    return lex;
}
LEXEME *expression_typeid(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    lex = getsym();
    return lex;
}
BOOL insertOperatorParams(SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, FUNCTIONCALL *funcparams)
{
    SYMBOL *s2 = NULL, *s3;
    HASHREC **hrd, *hrs;
    char *name = overloadNameTab[openpa - kw_new + CI_NEW];
    TYPE *tpx;
    if (!isstructured(*tp) && basetype(*tp)->type != bt_enum)
        return FALSE;
    if (isstructured(*tp))
    {
        LIST l ;
        l.data = (void *)basetype(*tp)->sp;
        l.next = structSyms;
        structSyms = &l;
        s2 = classsearch(name, FALSE);
        structSyms = l.next;
        funcparams->thistp = Alloc(sizeof(TYPE));
        funcparams->thistp->type = bt_pointer;
        funcparams->thistp->size = getSize(bt_pointer);
        funcparams->thistp->btp = *tp;        
        funcparams->thisptr = *exp;
    }
    // quit if there are no matches because we will use the default...
    if (!s2)
        return FALSE;
    // finally make a shell to put all this in and add shims for any builtins we want to try
    tpx = (TYPE *)Alloc(sizeof(TYPE));
    tpx->type = bt_aggregate;
    s3 = makeID(sc_overloads, tpx, NULL, name);
    tpx->sp = s3;
    SetLinkerNames(s3, lk_c);
    tpx->syms = CreateHashTable(1);
    hrd = &tpx->syms->table[0];
    if (s2)
    {
        hrs = s2->tp->syms->table[0];
        while (hrs)
        {
            HASHREC *ins = Alloc(sizeof(HASHREC));
            ins->p = hrs->p;
            *hrd = ins;
            hrd = &(*hrd)->next;
            hrs = hrs->next;
        }
    }
    s3 = GetOverloadedFunction(tp, &funcparams->fcall, s3, funcparams, NULL, TRUE, FALSE);
    if (s3)
    {
        s3->throughClass = s3->parentClass != NULL;
        funcparams->sp = s3;
        funcparams->functp = s3->tp;
        *exp = intNode(en_func, 0);
        (*exp)->v.func = funcparams;
        *tp = s3->tp;
        return TRUE;
    }
    return FALSE;
}
BOOL insertOperatorFunc(enum ovcl cls, enum e_kw kw, SYMBOL *funcsp, 
                        TYPE **tp, EXPRESSION **exp, TYPE *tp1, EXPRESSION *exp1)
{
    SYMBOL *s1 = NULL, *s2 = NULL, *s3;
    HASHREC **hrd, *hrs;
    FUNCTIONCALL *funcparams;
    char *name = overloadNameTab[kw - kw_new + CI_NEW];
    TYPE *tpx;
    if (!isstructured(*tp) && basetype(*tp)->type != bt_enum
        && (!tp1 || !isstructured(tp1) && basetype(tp1)->type != bt_enum))
        return FALSE;
        
    // first find some occurrance either in the locals or in the extant global namespace
    // but only if it is binary or unary...
    switch(cls)
    {
        case ovcl_unary_numeric:
        case ovcl_unary_numericptr:
        case ovcl_unary_int:
        case ovcl_unary_prefix:
        case ovcl_unary_postfix:
        case ovcl_unary_pointer:
        case ovcl_unary_any:
        case ovcl_binary_any:
        case ovcl_binary_numericptr: 
        case ovcl_binary_numeric:
        case ovcl_binary_int:
        case ovcl_comma:
            s1 = search(name, localNameSpace->syms);
            if (!s1)
                s1 = namespacesearch(name, localNameSpace, FALSE, FALSE);
            if (!s1 && enumSyms)
                s1 = search(name, enumSyms->tp->syms);
            if (!s1)
               s1 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
            break;
    }
    // next find some occurrance in the class or struct
    if (isstructured(*tp))
    {
        LIST l ;
        l.data = (void *)basetype(*tp)->sp;
        l.next = structSyms;
        structSyms = &l;
        s2 = classsearch(name, FALSE);
        structSyms = l.next;
    }
    // quit if there are no matches because we will use the default...
    if (!s1 && !s2)
        return FALSE;
    // finally make a shell to put all this in and add shims for any builtins we want to try
    tpx = (TYPE *)Alloc(sizeof(TYPE));
    tpx->type = bt_aggregate;
    s3 = makeID(sc_overloads, tpx, NULL, name);
    tpx->sp = s3;
    SetLinkerNames(s3, lk_c);
    tpx->syms = CreateHashTable(1);
    hrd = &tpx->syms->table[0];
    if (s1)
    {
        hrs = s1->tp->syms->table[0];
        while (hrs)
        {
            HASHREC *ins = Alloc(sizeof(HASHREC));
            ins->p = hrs->p;
            *hrd = ins;
            hrd = &(*hrd)->next;
            hrs = hrs->next;
        }
    }
    if (s2)
    {
        hrs = s2->tp->syms->table[0];
        while (hrs)
        {
            HASHREC *ins = Alloc(sizeof(HASHREC));
            ins->p = hrs->p;
            *hrd = ins;
            hrd = &(*hrd)->next;
            hrs = hrs->next;
        }
    }
    funcparams = Alloc(sizeof(FUNCTIONCALL));
    if (isstructured(*tp))
    {
        funcparams->thistp = Alloc(sizeof(TYPE));
        funcparams->thistp->type = bt_pointer;
        funcparams->thistp->size = getSize(bt_pointer);
        funcparams->thistp->btp = *tp;        
        funcparams->thisptr = *exp;
        if (tp1)
        {
            ARGLIST *one = Alloc(sizeof(ARGLIST));
            one->exp = exp1;
            one->tp = tp1;
            funcparams->arguments = one;
        }
        else if (cls == ovcl_unary_postfix)
        {
            ARGLIST *one = Alloc(sizeof(ARGLIST));
            one->exp = intNode(en_c_i, 0);
            one->tp = &stdint;
            funcparams->arguments = one;
        }
    }
    else
    {
        ARGLIST *one = NULL, *two = NULL;
        if (*tp)
        {
            one = Alloc(sizeof(ARGLIST));
            one->exp = *exp;
            one->tp = *tp;
            funcparams->arguments = one;
        }
        if (tp1)
        {
            two = Alloc(sizeof(ARGLIST));
            two->exp = exp1;
            two->tp = tp1;
            funcparams->arguments = two;
        }
        else if (cls == ovcl_unary_postfix)
        {
            two = Alloc(sizeof(ARGLIST));
            two->exp = intNode(en_c_i, 0);
            two->tp = &stdint;
            funcparams->arguments = two;
        }
        if (one && two)
        {
            one->next = two;
        }
        if (one)
        {
            funcparams->arguments = one;
        }
        else
        {
            funcparams->arguments = two;
        }
    }
    switch(cls)
    {
        case ovcl_unary_any:
        case ovcl_unary_prefix:
        case ovcl_unary_numericptr:
        case ovcl_unary_numeric:
        case ovcl_unary_int:
            break;
        case ovcl_unary_postfix:
            break;
        case ovcl_unary_pointer:
            break;
        case ovcl_binary_any:
        case ovcl_binary_numericptr:
        case ovcl_binary_numeric:
        case ovcl_binary_int:
            break;
        case ovcl_assign_any:
        case ovcl_assign_numericptr:
        case ovcl_assign_numeric:
        case ovcl_assign_int:
            break;
        case ovcl_pointsto:
            break;
        case ovcl_openbr:
            break;
        case ovcl_openpa:
            break;
        case ovcl_comma:
            break;
    }
    s3 = GetOverloadedFunction(tp, &funcparams->fcall, s3, funcparams, NULL, TRUE, FALSE);
    if (s3)
    {
        s3->throughClass = s3->parentClass != NULL;
        funcparams->sp = s3;
        funcparams->functp = s3->tp;
        *exp = intNode(en_func, 0);
        (*exp)->v.func = funcparams;
        *tp = s3->tp;
        expression_arguments(NULL, funcsp, tp, exp);
        if (s3->defaulted && kw == assign)
            createAssignment(s3->parentClass, s3);
        return TRUE;
    }
    return FALSE;
}
LEXEME *expression_new(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BOOL global)
{
    FUNCTIONCALL *placement = Alloc(sizeof(FUNCTIONCALL));
    FUNCTIONCALL *initializers = NULL;
    char *name = overloadNameTab[CI_NEW];
    TYPE *tpf = NULL;
    EXPRESSION *arrSize = NULL, *exp1;
    SYMBOL *s1 = NULL;
    EXPRESSION *val = NULL, *newfunc = NULL;
    int lbl = beGetLabel;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (startOfType(lex))
        {
            // type in parenthesis
            lex = get_type_id(lex, tp, funcsp, FALSE);
            needkw(&lex, closepa);
        }
        else
        {
            // placement new
            lex = backupsym(1);
            lex = getArgs(lex, funcsp, placement);
        }
    }
    if (!*tp)
    {
        if (MATCHKW(lex, openpa))
        {
            // type in parenthesis
            lex = getsym();
            lex = get_type_id(lex, tp, funcsp, FALSE);
            needkw(&lex, closepa);
        }
        else
        {
            // new type id
            enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            BOOL defd = FALSE;
            SYMBOL *sp = NULL;
            BOOL notype = FALSE;
            lex = getBasicType(lex, funcsp, tp, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, NULL);
            if (MATCHKW(lex, openbr))
            {
                TYPE *tp1 = NULL;
                EXPRESSION *exp = NULL;
                name = overloadNameTab[CI_NEWA];
                lex = getsym();
                lex = expression(lex, funcsp, NULL, &tp1, &exp, FALSE);
                if (!isint(tp1))
                {
                    error(ERR_NEED_INTEGER_TYPE);
                }
                needkw(&lex, closebr);
                arrSize = exp;
                while (MATCHKW(lex, openbr))
                {
                    lex = getsym();
                    lex = expression(lex, funcsp, NULL, &tp1, &exp, FALSE);
                    optimize_for_constants(&exp);
                    if (!isint(tp1))
                    {
                        error(ERR_NEED_INTEGER_TYPE);
                    }
                    else if (!isintconst(exp))
                    {
                        error(ERR_CONSTANT_EXPRESSION_EXPECTED);
                    }
                    else
                    {
                        arrSize = exprNode(en_mul, arrSize, exp);
                    }
                    needkw(&lex, closebr);
                }
                optimize_for_constants(&arrSize);
            }
        }
    }
    if (*tp)
    {
        EXPRESSION *sz;
        ARGLIST *sza;
        int n = (*tp)->size;
        if (arrSize && isstructured(*tp))
        {
            int al = n % (*tp)->sp->structAlign;
            if (al != 0)
                n += (*tp)->sp->structAlign - al;
        }
        sz = intNode(en_c_i, n);
        if (arrSize)
        {
            sz = exprNode(en_mul, sz, arrSize);
            sz = exprNode(en_add, sz, intNode(en_c_i, chosenAssembler->arch->rtlAlign));
        }
        optimize_for_constants(&sz);
        sza = Alloc(sizeof(ARGLIST));
        sza->exp = sz;
        sza->tp = &stdint;
        sza->next = placement->arguments;
        placement->arguments = sza;
    }
    if (!global)
    {
        if (isstructured(*tp))
        {
            LIST l ;
            l.data = (void *)basetype(*tp)->sp;
            l.next = structSyms;
            structSyms = &l;
            s1 = classsearch(name, FALSE);
            structSyms = l.next;
        }
    }
    if (!s1)
    {
        s1 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
    }
    // should have always found something
    s1 = GetOverloadedFunction(&tpf, &placement->fcall, s1, placement, NULL, TRUE, FALSE);
    if (s1)
    {
        SYMBOL *sp = makeID(sc_auto, &stdpointer, NULL, AnonymousName());
        sp->assigned = TRUE;
        val = varNode(en_auto, sp);
        sp->decoratedName = sp->errname = sp->name;
        insert(sp, localNameSpace->syms);
        deref(&stdpointer, &val);
        s1->throughClass = s1->parentClass != NULL;
        if (s1->throughClass && !isAccessible(s1->parentClass, s1->parentClass, s1, funcsp, placement->thisptr ? ac_protected : ac_public, FALSE))
        {
            errorsym(ERR_CANNOT_ACCESS, s1);
        }
        placement->sp = s1;
        placement->functp = s1->tp;
        placement->ascall = TRUE;
        placement->fcall = varNode(en_pc, s1);
        exp1 = intNode(en_func, 0);
        exp1->v.func = placement;
        exp1 = doinline(placement, s1);
        newfunc = exp1;
    }
    if (KW(lex) == openpa)
    {
        // initializers
        if (isstructured(*tp))
        {
            initializers = Alloc(sizeof(FUNCTIONCALL));
            lex = getArgs(lex, funcsp, initializers);
            if (s1)
            {
                *exp = val;
                if (arrSize)
                    *exp = exprNode(en_add, *exp, intNode(en_c_i, chosenAssembler->arch->rtlAlign));
                callConstructor(tp, exp, initializers, FALSE, arrSize, TRUE, FALSE);
            }
        }
        else
        {
            exp1 = NULL;
            lex = expression_func_type_cast(lex, funcsp, &tpf, &exp1);
            if (exp1 && val)
            {
                cast(*tp, &exp1);
                *exp = exprNode(en_assign, val, exp1);
            }
        }
    }
    else if (KW(lex) == begin)
    {
        // braced initializers
        if (*tp)
        {
            if (!isstructured(*tp) || (*tp)->sp->trivialCons)
            {
                INITIALIZER *init = NULL, *dest = NULL;
                EXPRESSION *base = exp1;
                lex = initType(lex, funcsp, 0, sc_member, &init, &dest, *tp, NULL, FALSE);
                // dest is lost for these purposes
                *exp = convertInitToExpression(*tp, NULL, funcsp, init, base);
                if (isstructured(*tp))
                {
                    EXPRESSION *exp1 = exprNode(en_blockclear, base, NULL);
                    exp1->size = (*tp)->size;
                    *exp = exprNode(en_void, exp1, *exp);
                }
            }
            else
            {
                errorsym(ERR_STRUCTURE_INITIALIZATION_NEEDS_CONSTRUCTOR, (*tp)->sp );
                skip(&lex, end);
            }
        }
    }
    else if (isstructured(*tp))
    {
        if (s1)
        {
            // call default constructor
            *exp = val;
            if (arrSize)
                *exp = exprNode(en_add, *exp, intNode(en_c_i, chosenAssembler->arch->rtlAlign));
            callConstructor(tp, exp, NULL, FALSE, arrSize, TRUE, FALSE);
        }
    }
    tpf = Alloc(sizeof(TYPE));
    tpf->type = bt_pointer;
    tpf->size = getSize(bt_pointer);
    tpf->btp = *tp;
    *tp = tpf;

    if (val && newfunc)
    {
        exp1 = exprNode(en_assign, val, newfunc);
        if (*exp)
            exp1 = exprNode(en_voidnz, exprNode(en_void, exp1, *exp), val);
        *exp = exp1;
    }
    else
    {
        *exp = intNode(en_c_i, 0);
    }
    return lex;
}
LEXEME *expression_delete(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BOOL global)
{
    BOOL byArray = FALSE;
    SYMBOL *s1 = NULL;
    FUNCTIONCALL *funcparams;
    ARGLIST *one = NULL;
    EXPRESSION *exp1 = NULL, *exp2;
    TYPE *tpf;
    int lbl = beGetLabel;
    char *name = overloadNameTab[CI_DELETE];
    EXPRESSION *in;
    lex = getsym();
    if (MATCHKW(lex, openbr))
    {
        lex = getsym();
        needkw(&lex, closebr);
        byArray = TRUE;
        name = overloadNameTab[CI_DELETEA];
    }
    lex = expression_cast(lex, funcsp, NULL, tp, exp, FALSE);
    if (!ispointer(*tp))
        error(ERR_POINTER_TYPE_EXPECTED);
    in = *exp;
    if (byArray)
    {
        exp1 = exprNode(en_sub, *exp, intNode(en_c_i, chosenAssembler->arch->rtlAlign));
        exp2 = exp1;
        deref(&stdint, &exp1);
    }
    else
    {
        exp2 = *exp;
    }
    if (isstructured(basetype(*tp)->btp))
    {
        callDestructor(basetype(*tp)->btp->sp, exp, exp1, TRUE);
    }
    exp1 = exp2;
    if (!global)
    {
        if (isstructured(*tp))
        {
            LIST l ;
            l.data = (void *)basetype(*tp)->sp;
            l.next = structSyms;
            structSyms = &l;
            s1 = classsearch(name, FALSE);
            structSyms = l.next;
        }
    }
    if (!s1)
    {
        s1 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
    }
    // should always find something
    funcparams = Alloc(sizeof(FUNCTIONCALL));
    one = Alloc(sizeof(ARGLIST));
    one->exp =exp1;
    one->tp = &stdpointer;
    funcparams->arguments = one;
    s1 = GetOverloadedFunction(&tpf, &funcparams->fcall, s1, funcparams, NULL, TRUE, FALSE);
    if (s1)
    {
        s1->throughClass = s1->parentClass != NULL;
        if (s1->throughClass && !isAccessible(s1->parentClass, s1->parentClass, s1, funcsp, funcparams->thisptr ? ac_protected : ac_public, FALSE))
        {
            errorsym(ERR_CANNOT_ACCESS, s1);
        }
        funcparams->sp = s1;
        funcparams->functp = s1->tp;
        funcparams->ascall = TRUE;
        funcparams->fcall = varNode(en_pc, s1);
        exp1 = intNode(en_func, 0);
        exp1->v.func = funcparams;
        exp1 = doinline(funcparams, s1);
        exp1 = exprNode(en_void, *exp, exp1);
        exp1 = exprNode(en_voidnz, exprNode(en_void, in, exp1), intNode(en_c_i, 0));
        *exp = exp1;
        *tp = s1->tp;
    }
    *tp = &stdvoid;
    return lex;
}
LEXEME *expression_noexcept(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    assert(0);
    return lex;
}
