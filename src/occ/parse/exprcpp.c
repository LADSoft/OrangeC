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
#include <assert.h>
#include "rtti.h"

extern COMPILER_PARAMS cparams;
extern ARCH_ASM *chosenAssembler;
extern int stdpragmas;
extern enum e_kw skim_closepa[];
extern enum e_kw skim_closebr[];
extern enum e_kw skim_semi[];
extern enum e_kw skim_end[];
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
extern BOOLEAN setjmp_used;
extern char *overloadNameTab[];
extern char *overloadXlateTab[];
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern SYMBOL *enumSyms;
extern LAMBDA *lambdas;
extern int currentErrorLine;
extern int templateNestingCount;
extern LAMBDA *lambdas;
extern BOOLEAN functionCanThrow;
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
static EXPRESSION **findPointerToExpression(EXPRESSION **parent, EXPRESSION *expr)
{
    EXPRESSION **y;
    if (*parent == expr)
        return parent;
    y = findPointerToExpression(&(*parent)->left, expr);
    if (y)
        return y;
    y = findPointerToExpression(&(*parent)->right, expr);
    if (y)
        return y;
    return NULL;
}
EXPRESSION *baseClassOffset(SYMBOL *base, SYMBOL *derived, EXPRESSION *en)
{
    EXPRESSION *rv = en;
    if (base != derived)
    {
        VBASEENTRY *vbase = derived->vbaseEntries;
        BASECLASS *lst;
        while (vbase)
        {
            // this will get all virtual bases since they are all listed on each deriviation
            if (vbase->alloc && vbase->cls == base)
            {
                EXPRESSION *ec = intNode(en_c_i, vbase->pointerOffset);
                rv = exprNode(en_add, en, ec);
                deref(&stdpointer, &rv);
                break;
            }
            vbase = vbase->next;
        }
        if (!vbase)
        {
            BASECLASS *lst = derived->baseClasses;
            if (lst)
            {
                BASECLASS *stack[200];
                int top=0,i;
                stack[top++] = lst;
                while (lst)
                {
                    if (lst->cls == base)
                        break;
                    if (lst->cls->baseClasses)
                    {
                        lst = lst->cls->baseClasses;
                        stack[top++] = lst;
                    }
                    else if (lst->next)
                    {
                        lst = lst->next;
                        stack[top-1] = lst;
                    }
                    else if (top)
                    {
                        do {
                            lst = stack[--top]->next;
                        } while (top && !lst);
                        stack[top++] = lst;
                    }
                }
                if (top && stack[0])
                {
                    for (i=0; i < top; i++)
                    {
                        if (stack[i]->isvirtual)
                        {
                            int offset;
                            VBASEENTRY *cur = i ? stack[i-1]->cls->vbaseEntries : derived->vbaseEntries;
                            while (cur && cur->cls != stack[i]->cls)
                                cur = cur->next;
                            offset = cur->pointerOffset;
                            rv = exprNode(en_add, rv, intNode(en_c_i, offset));
                           if (stack[i]->isvirtual)
                                deref(&stdpointer, &rv);
                        }
                        else
                        {
                            int offset = stack[i]->offset;
                            rv = exprNode(en_add, rv, intNode(en_c_i, offset));
                        }
                    }
                }
            }
        }
    }
    return rv;
}
void qualifyForFunc(SYMBOL *sym, TYPE **tp, BOOLEAN isMutable)
{
    if (sym)
    {
        if (isconst(sym->tp) && !isMutable)
        {
            TYPE *tp1 = Alloc(sizeof(TYPE));
            tp1->size = (*tp)->size;
            tp1->type = bt_const;
            tp1->btp = *tp;
            tp1->rootType = (*tp)->rootType;
            *tp = tp1;
        }
        if (isvolatile(sym->tp))
        {
            TYPE *tp1 = Alloc(sizeof(TYPE));
            tp1->size = (*tp)->size;
            tp1->type = bt_volatile;
            tp1->btp = *tp;
            tp1->rootType = (*tp)->rootType;
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
    (*tp)->rootType = (*tp);
    qualifyForFunc(sym, tp, FALSE);
}
EXPRESSION *getMemberBase(SYMBOL *memberSym, SYMBOL *strSym, SYMBOL *funcsp, BOOLEAN toError)
{
    EXPRESSION *en;
    SYMBOL *enclosing = getStructureDeclaration();
    
    if (enclosing && (!funcsp || (funcsp->storage_class != sc_global && funcsp->storage_class != sc_static))) // lambdas will be caught by this too
    {
        if (strSym)
        {
            if (!comparetypes(strSym->tp, enclosing->tp, TRUE))
            {
                int n = classRefCount(strSym, enclosing);
                if (n == 0)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
                else if (n > 1)
                    errorsym2(ERR_NOT_UNAMBIGUOUS_BASE,strSym, enclosing);
            }
//            enclosing = strSym;
        }
        if (funcsp)
            en = varNode(en_auto, (SYMBOL *)basetype(funcsp->tp)->syms->table[0]->p); // this ptr
        else
            en = intNode(en_thisshim, 0);
        if (lambdas && !memberSym->parentClass->islambda)
        {
            if (!lambdas->lthis || !lambdas->captured)
            {
                en = intNode(en_c_i, 0);
                if (toError)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
            }
            else
            {
                SYMBOL *sp = search("$this", lambdas->cls->tp->syms);
                enclosing = basetype(lambdas->lthis->tp)->btp->sp;
                if (sp)
                {
                    deref(&stdpointer, &en);
                    en = exprNode(en_add, en, intNode(en_c_i, sp->offset));
                }
                else
                {
                    diag("getMemberBase: cannot find lambda this");
                }
                  
            }
        }
        deref(&stdpointer, &en);
        if (enclosing != memberSym->parentClass && enclosing->mainsym != memberSym->parentClass)
        {
            if (classRefCount(memberSym->parentClass, enclosing) != 1)
            {
                if (toError)
                    errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, memberSym->parentClass, enclosing);
            }
            else if (!isExpressionAccessible(NULL, memberSym, funcsp, en, FALSE))
            {
                if (toError)
                {
                    errorsym(ERR_CANNOT_ACCESS, memberSym);
                }
            }
            en = baseClassOffset(memberSym->parentClass, enclosing, en);
        }
    }
    else
    {
        en = intNode(en_c_i, 0);
        if (toError)
            errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
    }
    return en;
}
EXPRESSION *getMemberNode(SYMBOL *memberSym, SYMBOL *strSym, TYPE **tp, SYMBOL *funcsp)
{
    EXPRESSION *en = getMemberBase(memberSym, strSym, funcsp, TRUE);
    en = exprNode(en_add, en, intNode(en_c_i, memberSym->offset));
    *tp = memberSym->tp;
    return en;
}
EXPRESSION *getMemberPtr(SYMBOL *memberSym, SYMBOL *strSym, TYPE **tp, SYMBOL *funcsp)
{
    EXPRESSION *rv;
    TYPE *tpq = Alloc(sizeof(TYPE));
    tpq->type = bt_memberptr;
    tpq->btp = memberSym->tp;
    tpq->rootType = tpq;
    tpq->sp = memberSym->parentClass;
    *tp = tpq;
    rv = varNode(en_memberptr, memberSym);
    rv->isfunc = TRUE;
    if (!isExpressionAccessible(NULL, memberSym, funcsp, NULL, TRUE))
    {
        errorsym(ERR_CANNOT_ACCESS, memberSym);
    }
    return rv;
}
BOOLEAN castToArithmeticInternal(BOOLEAN integer, TYPE **tp, EXPRESSION **exp, enum e_kw kw, TYPE *other, BOOLEAN implicit)
{
    SYMBOL *sp = basetype(*tp)->sp;
    if (!other || isarithmetic(other))
    {
        SYMBOL *cst = integer ? lookupIntCast(sp, other ? other : &stdint, implicit) 
                              : lookupArithmeticCast(sp, other ? other : &stddouble, implicit);
        if (cst)    
        {
            FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
            EXPRESSION *e1;
            params->fcall = varNode(en_pc, cst);
            params->thisptr = *exp;
            params->thistp = Alloc(sizeof(TYPE));
            params->thistp->type = bt_pointer;
            params->thistp->btp = cst->parentClass->tp;
            params->thistp->rootType = params->thistp;
            params->thistp->size = getSize(bt_pointer);
            params->functp = cst->tp;
            params->sp = cst;
            params->ascall = TRUE;
            if ((*exp)->type == en_literalclass)
            {
                *exp = substitute_params_for_function(params, (*exp)->v.syms);
                optimize_for_constants(exp);
                if (!cst->constexpression || !IsConstantExpression(*exp, TRUE))
                    error(ERR_CONSTANT_FUNCTION_EXPECTED);
            }
            else
            {
                *exp = DerivedToBase(cst->parentClass->tp, *tp, *exp, 0);
                params->thisptr = *exp;
                {
                    e1 = varNode(en_func, NULL);
                    e1->v.func = params;
                    GENREF(cst);
                }
                *exp = e1;
                if (other)
                    cast(other, exp);
            }
            *tp = basetype(cst->tp)->btp;
            return TRUE;
        }
    }
    return FALSE;
}
void castToArithmetic(BOOLEAN integer, TYPE **tp, EXPRESSION **exp, enum e_kw kw, TYPE *other, BOOLEAN implicit)
{
    if (cparams.prm_cplusplus && isstructured(*tp))
    {
        if (!castToArithmeticInternal(integer, tp, exp, kw, other, implicit))
        {
            // failed at conversion
            if (kw >= kw_new && kw <= compl)
            {
                // LHS, put up an operator whatever message
                char buf[256];
                char tbuf[4096];
                memset(tbuf, 0, sizeof(tbuf));
                tbuf[0] = 0;
                typeToString(tbuf, *tp);
                if (other)
                {
                    strcat(tbuf, ", ");
                    typeToString(tbuf+strlen(tbuf), other);
                }
                my_sprintf(buf, "operator %s(%s)", overloadXlateTab[kw - kw_new + CI_NEW], tbuf);
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
}
BOOLEAN castToPointer(TYPE **tp, EXPRESSION **exp, enum e_kw kw, TYPE *other)
{
    if (cparams.prm_cplusplus && isstructured(*tp))
    {
        SYMBOL *sp = basetype(*tp)->sp;
        if (ispointer(other) || basetype(other)->type == bt_memberptr)
        {
            SYMBOL *cst = lookupPointerCast(sp, other) ;
            if (cst)
            {
                FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
                EXPRESSION *e1;
                
                *exp = DerivedToBase(cst->parentClass->tp, *tp, *exp, 0);
                params->fcall = varNode(en_pc, cst);
                params->thisptr = *exp;
                params->thistp = Alloc(sizeof(TYPE));
                params->thistp->type = bt_pointer;
                params->thistp->btp = cst->parentClass->tp;
                params->thistp->rootType = params->thistp;
                params->thistp->size = getSize(bt_pointer);
                params->functp = cst->tp;
                params->sp = cst;
                params->ascall = TRUE;
                e1 = varNode(en_func, NULL);
                e1->v.func = params;
                GENREF(cst);
                *exp = e1;
                if (ispointer(other))
                {
                    cast(other, exp);
                }
                else
                {
                    SYMBOL *retsp = makeID(sc_auto, other, NULL, AnonymousName());
                    retsp->allocate = TRUE;
                    retsp->used = retsp->assigned = TRUE;
                    SetLinkerNames(retsp, lk_cdecl);
                    insert(retsp, localNameSpace->syms);
                    params->returnSP = retsp;
                    params->returnEXP = varNode(en_auto, retsp);
                }
                *tp = basetype(cst->tp)->btp;
                return TRUE;
            }
        }
    }
    return FALSE;
}
BOOLEAN cppCast(TYPE *src, TYPE **tp, EXPRESSION **exp)
{
    if (isstructured(src))
    {
        if (isstructured(*tp))
        {
            SYMBOL *sp = basetype(src)->sp;
            SYMBOL *cst = lookupSpecificCast(sp, *tp);
            if (cst)
            {
                FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
                EXPRESSION *e1;
                CheckCalledException(cst, *exp);
                *exp = DerivedToBase(cst->parentClass->tp, src, *exp, 0);
                params->fcall = varNode(en_pc, cst);
                params->thisptr = *exp;
                params->thistp = Alloc(sizeof(TYPE));
                params->thistp->type = bt_pointer;
                params->thistp->size = getSize(bt_pointer);
                params->thistp->btp = cst->parentClass->tp;
                params->thistp->rootType = params->thistp;
                params->functp = cst->tp;
                params->sp = cst;
                params->ascall = TRUE;
                if (isstructured(*tp))
                {
                    EXPRESSION *ev = anonymousVar(sc_auto, *tp);
                    SYMBOL *av = ev->v.sp;
                    params->returnEXP = ev;
                    params->returnSP = sp;
                    callDestructor(basetype(*tp)->sp, NULL, &ev, NULL, TRUE, FALSE, FALSE);
                    initInsert(&av->dest, *tp, ev, 0, TRUE);
                }
                e1 = varNode(en_func, NULL);
                e1->v.func = params;
                GENREF(cst);
                *exp = e1;
                *exp = DerivedToBase(*tp, basetype(cst->tp)->btp, *exp, 0);
                return TRUE;
            }
        }
        else if (isarithmetic(*tp))
        {
            TYPE *tp1 = src;
            return castToArithmeticInternal(FALSE, &tp1, exp, (enum e_kw)-1, *tp, FALSE);
        }
        else if (ispointer(*tp) || basetype(*tp)->type == bt_memberptr)
        {
            TYPE *tp1 = src;
            return castToPointer(&tp1, exp, (enum e_kw)-1, *tp);
        }
    }
    return FALSE;   
}
typedef struct _subslist
{
    struct _subslist *next;
    EXPRESSION *exp;
    SYMBOL *sym;
} SUBSTITUTIONLIST;

static EXPRESSION *substitute_vars(EXPRESSION *exp, SUBSTITUTIONLIST *match, HASHTABLE *syms)
{
    EXPRESSION *rv;
    if (lvalue(exp))
    {
        if (exp->left->type == en_auto)
        {
            SYMBOL *sym = exp->left->v.sp;
            while (match)
            {
                if (sym == match->sym)
                {                
                    return match->exp;
                }
                match = match->next;
            }
        }
        else if (syms && exp->v.sp)
        {
            HASHREC *hr = syms->table[0];
            while (hr)
            {
                CONSTEXPRSYM *ces = (CONSTEXPRSYM *)hr->p;
                if (ces->sym == exp->v.sp)
                {
                    return ces->exp;
                }
                hr = hr->next;
            }
        }
    }
    rv = Alloc(sizeof(EXPRESSION));
    *rv = *exp;
    if (exp->left)
        rv->left = substitute_vars(exp->left, match, syms);
    if (exp->right)
        rv->right = substitute_vars(exp->right, match, syms);
    return rv;
}
EXPRESSION *substitute_params_for_constexpr(EXPRESSION *exp, FUNCTIONCALL *funcparams, HASHTABLE *syms)
{
    HASHREC *hr = basetype(funcparams->sp->tp)->syms->table[0];
    INITLIST *args = funcparams->arguments;
    SUBSTITUTIONLIST *list=NULL, **plist = &list;
    if (!funcparams->sp->castoperator)
    {
        if (((SYMBOL *)hr->p)->thisPtr)
            hr = hr->next;
        // because we already did function matching to get the constructor,
        // the worst that can happen here is that the specified arg list is shorter
        // than the actual parameters list
        while (hr && args)
        {
            *plist = Alloc(sizeof(SUBSTITUTIONLIST));
            (*plist)->exp = args->exp;
            (*plist)->sym = (SYMBOL *)hr->p;
            plist = &(*plist)->next;
            hr = hr->next;
            args = args->next;
        }
        while (hr)
        {
            *plist = Alloc(sizeof(SUBSTITUTIONLIST));
            (*plist)->exp = ((SYMBOL *)hr->p)->init->exp;
            (*plist)->sym = (SYMBOL *)hr->p;
            plist = &(*plist)->next;
            hr = hr->next;
        }
    }
    return substitute_vars(exp, list, syms);
}
STATEMENT *do_substitute_for_function(STATEMENT *block, FUNCTIONCALL *funcparams, HASHTABLE *syms)
{
    STATEMENT *rv = NULL , **prv = &rv;
    while (block != NULL)
    {
        *prv = Alloc(sizeof(STATEMENT));
        **prv = *block;
        if (block->select)
            (*prv)->select = substitute_params_for_constexpr(block->select, funcparams, syms);
        if (block->lower)
            (*prv)->lower = do_substitute_for_function(block->lower, funcparams, syms);
        prv = &(*prv)->next;
        block = block->next;
    }
    return rv;
}
EXPRESSION *substitute_params_for_function(FUNCTIONCALL *funcparams, HASHTABLE *syms)
{
    STATEMENT *st = do_substitute_for_function(funcparams->sp->inlineFunc.stmt, funcparams, syms);
    EXPRESSION *exp = exprNode(en_stmt, 0, 0);
    exp->v.stmt = st;
    return exp;
}
LEXEME *expression_func_type_cast(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    BOOLEAN defd = FALSE;
    int consdest = FALSE;
    BOOLEAN notype = FALSE;
    *tp = NULL;
    lex = getBasicType(lex, funcsp, tp, NULL, FALSE, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd, &consdest, NULL, FALSE, TRUE);
    if (!MATCHKW(lex, openpa))
    {
        if (MATCHKW(lex, begin))
        {
            INITIALIZER *init = NULL, *dest=NULL;
            SYMBOL *sp = NULL;
            sp = anonymousVar(sc_auto, *tp)->v.sp;
            lex = initType(lex, funcsp, 0, sc_auto, &init, &dest, *tp, sp, FALSE, flags);
            *exp = convertInitToExpression(*tp, sp, funcsp, init, NULL, FALSE);
            if (sp)
            {
                EXPRESSION **e1 = exp;
                while ((*e1)->type == en_void)
                    e1 = &(*e1)->right;
                *e1 = exprNode(en_void, *e1, varNode(en_auto, sp));
                sp->dest = dest;
            }
        }
        else
        {
            *exp = intNode(en_c_i, 0);
            errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, NULL);
            errskim(&lex, skim_semi);
        }
    }
    else if (!cparams.prm_cplusplus)
    {
        *exp = intNode(en_c_i, 0);
        errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, NULL);
        errskim(&lex, skim_semi);
    }
    else
    {

        if (isref(*tp))
            *tp = basetype(basetype(*tp)->btp);
        if (isstructured(*tp))
        {
            SYMBOL *sp;
            TYPE *ctype = *tp;
            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL)); 
            EXPRESSION *exp1;
            ctype = PerformDeferredInitialization(ctype, funcsp);
            lex = getArgs(lex, funcsp, funcparams, closepa, TRUE, flags);
            if (!(flags & _F_SIZEOF))
            {
                exp1 = *exp = anonymousVar(sc_auto, basetype(*tp)->sp->tp);
                sp = exp1->v.sp;
                callConstructor(&ctype, exp, funcparams, FALSE, NULL, TRUE, TRUE, FALSE, FALSE, FALSE);
                callDestructor(basetype(*tp)->sp, NULL, &exp1, NULL, TRUE, FALSE, FALSE);
                initInsert(&sp->dest, *tp, exp1, 0, TRUE);
//                if (flags & _F_SIZEOF)
//                    sp->destructed = TRUE; // in case we don't actually use this instantiation
            }
            else
                *exp = intNode(en_c_i, 0);
        }
        else
        {
            TYPE *throwaway;
            needkw(&lex, openpa);
            if (MATCHKW(lex, closepa))
            {
                // constructor with no args gets a value of zero...
                *exp = intNode(en_c_i, 0);
                needkw(&lex, closepa);
            }
            else
            {
                FUNCTIONCALL funcParams;
                funcParams.arguments = NULL;
                lex = backupsym();
                lex = getArgs(lex, funcsp, &funcParams, closepa, TRUE, flags);
                throwaway = NULL;
                if (funcParams.arguments)
                {
                    while (funcParams.arguments->next)
                        funcParams.arguments = funcParams.arguments->next;
                    throwaway = funcParams.arguments->tp;
                    *exp = funcParams.arguments->exp;
                    if (throwaway && (*tp)->type == bt_auto)
                        *tp = throwaway;
                    if ((*exp)->type == en_func)
                    {
                        *exp = (*exp)->v.func->fcall;
                        if (!*exp)
                            *exp = intNode(en_c_i, 0);
                    }
                    if (throwaway)
                    {
                        if (isvoid(throwaway) && !isvoid(*tp))
                        {
                            error(ERR_NOT_AN_ALLOWED_TYPE);
                        }
                        else if (isstructured(throwaway))
                        {
                            if (!isvoid(*tp))
                            {
                                if (!cparams.prm_cplusplus || !cppCast(throwaway, tp, exp))
                                {
                                    error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                                }
                            }
                        }
                        else if ((basetype(throwaway)->type == bt_memberptr || basetype(*tp)->type == bt_memberptr)
                                  && !comparetypes(throwaway, *tp, TRUE))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        else
                        {
                            cast(*tp, exp);
                        }
                    }
                }
                else
                {
                    *exp = intNode(en_c_i, 0);
                }
            }
        }
    }
    return lex;
}
BOOLEAN doDynamicCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp)
{
    if ((ispointer(oldType) && ispointer(*newType)) || isref(*newType))
    {
        TYPE *tpo = oldType; 
        TYPE *tpn = *newType;
        if (isref(*newType))
        {
            tpn  = basetype(*newType)->btp;
        }
        else
        {
            tpo = basetype(oldType)->btp;
            tpn  = basetype(*newType)->btp;
        }
        if ((!isconst(tpo) || isconst(tpn)) && isstructured(tpo))
        {
            if (isstructured(tpn) || basetype(tpn)->type == bt_void)
            {
                if (basetype(tpn)->type == bt_void || classRefCount(basetype(tpn)->sp, basetype(tpo)->sp) != 1)
                {
                    // if we are going up in the class heirarchy that is the only time we really need to do
                    // a dynamic cast
                    SYMBOL *sp = namespacesearch("__dynamic_cast", globalNameSpace, FALSE, FALSE);
                    if (sp)
                    {
                        EXPRESSION *exp1 = *exp;
                        EXPRESSION *exp2;
                        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                        INITLIST *arg1 = Alloc(sizeof(INITLIST)); // thisptr
                        INITLIST *arg2 = Alloc(sizeof(INITLIST)); // excepttab from thisptr
                        INITLIST *arg3 = Alloc(sizeof(INITLIST)); // oldxt
                        INITLIST *arg4 = Alloc(sizeof(INITLIST)); // newxt
                        SYMBOL *oldrtti = RTTIDumpType(tpo);
                        SYMBOL *newrtti = basetype(tpn)->type == bt_void ? NULL : RTTIDumpType(tpn);
                        deref(&stdpointer, &exp1);
                        sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
                        arg1->next = arg2;
                        arg2->next = arg3;
                        arg3->next = arg4;
                        arg1->exp = *exp;
                        arg1->tp = &stdpointer;
                        arg2->exp = exprNode(en_cond, *exp, exprNode(en_void, exp1, intNode(en_c_i, 0)));

                        arg2->tp = &stdpointer;
                        arg3->exp = oldrtti ? varNode(en_global, oldrtti) : intNode(en_c_i, 0);
                        arg3->tp = &stdpointer;
                        arg4->exp = newrtti ? varNode(en_global, newrtti) : intNode(en_c_i, 0);
                        arg4->tp = &stdpointer;
                        funcparams->arguments = arg1;
                        funcparams->sp = sp;
                        funcparams->functp = sp->tp;
                        funcparams->fcall = varNode(en_pc, sp);
                        funcparams->ascall = TRUE;
                        *exp = exprNode(en_lvalue,0,0);
                        (*exp)->left = exprNode(en_func, 0, 0);
                        (*exp)->left->v.func = funcparams;
                    }
                    if (isref(*newType))
                        *newType = tpn;
                    if (!basetype(tpo)->sp->hasvtab || !basetype(tpo)->sp->tp->syms->table[0])
                        errorsym(ERR_NOT_DEFINED_WITH_VIRTUAL_FUNCS, basetype(tpo)->sp);
                    return TRUE;
                }
                return doStaticCast(newType, oldType, exp, funcsp, TRUE);
            }
        }
    }
    return FALSE;
}
BOOLEAN doStaticCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp, BOOLEAN checkconst)
{
    BOOLEAN rref = FALSE;
    TYPE *orig = *newType;
    if (isref(*newType))
    {
        if (basetype(*newType)->type == bt_rref)
            rref = TRUE;
        while (isref(*newType))
            *newType = basetype(*newType)->btp;
    }
    // no change or stricter const qualification
    if (comparetypes(*newType, oldType, TRUE))
        return TRUE;
    // conversion from NULL to pointer
    if (ispointer(*newType) && isint(oldType)&& isconstzero(oldType, *exp) )
        return TRUE;
    // conversion to or from void pointer
    if (((isvoidptr(*newType) && (ispointer(oldType) || isfunction(oldType)))
        || (((isvoidptr(oldType)||(*exp)->type == en_nullptr) && ispointer(*newType)) 
        && (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp)))))
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
    if ((ispointer(*newType) && ispointer(oldType)) || isref(orig))
    {
        TYPE *tpo = oldType; 
        TYPE *tpn = orig;
        if (isref(tpn))
        {
            *newType = tpn = basetype(orig)->btp;
        }
        else
        {
            tpo = basetype(oldType)->btp;
            tpn = basetype(orig)->btp;
        }
        if ((!checkconst || !isconst(tpo) || isconst(tpn)) && (isstructured(tpn) && isstructured(tpo)))
        {
            int n = classRefCount(basetype(tpn)->sp, basetype(tpo)->sp);
            if (n == 1)
            {
                // derived to base
                EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                v->type = en_c_i;
                v = baseClassOffset(basetype(tpn)->sp, basetype(tpo)->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i) // check for no virtual base
                {
                    EXPRESSION *varsp = anonymousVar(sc_auto, &stdpointer);
                    EXPRESSION *var = exprNode(en_l_p, varsp, NULL);
                    EXPRESSION *asn = exprNode(en_assign, var, *exp);
                    EXPRESSION *left = exprNode(en_add, var, v);
                    EXPRESSION *right = var;
                    if (v->type == en_l_p) // check for virtual base
                        v->left = var;
                    v = exprNode(en_cond, var, exprNode(en_void, left, right));
                    v = exprNode(en_void, asn, v);
                    *exp = exprNode(en_lvalue, v, NULL);
                    return TRUE;
                }
            }
            else if (!n && classRefCount(basetype(tpo)->sp, basetype(tpn)->sp) == 1)
            {
                // base to derived
                EXPRESSION *v = Alloc(sizeof(EXPRESSION));
                v->type = en_c_i;
                v = baseClassOffset(basetype(tpo)->sp, basetype(tpn)->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i) // check for no virtual base
                {
                    EXPRESSION *varsp = anonymousVar(sc_auto, &stdpointer);
                    EXPRESSION *var = exprNode(en_l_p, varsp, NULL);
                    EXPRESSION *asn = exprNode(en_assign, var, *exp);
                    EXPRESSION *left = exprNode(en_sub, var, v);
                    EXPRESSION *right = var;
                    if (v->type == en_l_p) // check for virtual base
                        v->left = var;
                    v = exprNode(en_cond, var, exprNode(en_void, left, right));
                    v = exprNode(en_void, asn, v);
                    *exp = exprNode(en_lvalue, v, NULL);
                    return TRUE;
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
                if ((vbo && vbn) || !vbn)
                {
                    if (isAccessible(basetype(oldType)->sp, basetype(oldType)->sp, basetype(*newType)->sp, funcsp, ac_public, FALSE))
                        return TRUE;
                }
            }
        }
    }
    // class to anything via converwion func
    if (isstructured(oldType))
    {
        BOOLEAN rv= cppCast(oldType, newType, exp);
        if (rv)
            return TRUE;
    }
    *newType = orig;
    return FALSE;
    // e to T:  T t(e) for an invented temp t.
}
BOOLEAN doConstCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp)
{
    TYPE *orig = *newType;
    BOOLEAN rref = FALSE;
    if (isref(*newType))
    {
        if (basetype(*newType)->type == bt_rref)
            rref = TRUE;
        while (isref(*newType))
            *newType = basetype(*newType)->btp;
    }
    // as long as the types match except for any changes to const and 
    // it is not a function or memberptr we can convert it.
    if (comparetypes(*newType, oldType, 2))
    {
        if (!isfunction(oldType) && basetype(oldType)->type != bt_memberptr)
            return TRUE;
    }
    *newType = orig;
    return FALSE;
}
BOOLEAN doReinterpretCast(TYPE **newType, TYPE *oldType, EXPRESSION **exp, SYMBOL *funcsp, BOOLEAN checkconst)
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
    // function to int
    if (isfunction(oldType) && isint(*newType))
    {
        cast(*newType, exp);
        return TRUE;
    }
    if (oldType->type == bt_aggregate)
    {
        // function to int
        if (isint(*newType) || ispointer(*newType))
        {
            if ((*exp)->type == en_func)
            {
                HASHREC *hr = oldType->syms->table[0];
                SYMBOL *sp = (SYMBOL *)hr->p;
                if (hr->next)
                    errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, sp->name);
                (*exp) = varNode(en_pc, sp);
                cast(*newType, exp);
                return TRUE;
            }
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
            return TRUE;
            /*
            SYMBOL *spo = basetype(tpo)->sp;
            SYMBOL *spn = basetype(tpn)->sp;
            if (spo == spn)
            {
                return TRUE;
            }
            if (!isstructured(tpo) || (!spo->hasvtab && !spo->accessspecified && !spo->baseClasses))
            {
                if (!isstructured(tpn) || (!spn->hasvtab && !spn->accessspecified && !spn->baseClasses))
                {
                    // new alignment has to be the same or more restrictive than old
                    if (basetype(tpn)->alignment >= basetype(tpo)->alignment)
                        return TRUE;
                }
            }
            */
        }
    }
    // convert one member pointer to another
    if (basetype(*newType)->type == bt_memberptr)
    {
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
                return TRUE;
                /*
                SYMBOL *spo = basetype(tpo)->sp;
                SYMBOL *spn = basetype(tpn)->sp;
                if (spo == spn)
                {
                    return TRUE;
                }
                if (!isstructured(tpo) || (!spo->hasvtab && !spo->accessspecified && !spo->baseClasses))
                {
                    if (!isstructured(tpn) || (!spn->hasvtab && !spn->accessspecified && !spn->baseClasses))
                    {
                        // new alignment has to be the same or more restrictive than old
                        if (basetype(tpn)->alignment >= basetype(tpo)->alignment)
                        {
                            *newType = nt2;
                            return TRUE;
                        }
                    }
                }
                */
            }
        }
    }
    return FALSE;
    // fixme nullptr conversion to nullptr
}
LEXEME *GetCastInfo(LEXEME *lex, SYMBOL *funcsp, TYPE **newType, TYPE **oldType, EXPRESSION **oldExp, BOOLEAN packed)
{
    lex = getsym();
    *oldExp = NULL;
    *oldType = NULL;
    if (needkw(&lex, lt))
    {
        lex = get_type_id(lex, newType, funcsp, sc_cast, FALSE, TRUE);
        if (!*newType)
        {
            error(ERR_TYPE_NAME_EXPECTED);
            *newType = &stdpointer;
        }
        if (needkw(&lex, gt))
        {
            if (needkw(&lex, openpa))
            {
                lex = expression(lex, funcsp, NULL, oldType, oldExp, packed ? _F_PACKABLE : 0);
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
LEXEME *expression_typeid(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, int flags)
{
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        BOOLEAN byType = FALSE;
        if (startOfType(lex, FALSE))
        {
            lex = get_type_id(lex, tp, funcsp, sc_cast, FALSE, TRUE);
            byType = TRUE;
        }
        else
        {
            lex = expression_no_check(lex, funcsp, NULL, tp, exp, 0);
        }
        if (tp)
        {
            SYMBOL *sp = namespacesearch("__GetTypeInfo", globalNameSpace, FALSE, FALSE);
            if (sp)
            {
                
                FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                INITLIST *arg = Alloc(sizeof(INITLIST));
                INITLIST *arg2 = Alloc(sizeof(INITLIST));
                SYMBOL *rtti;
                SYMBOL *val;
                TYPE *valtp = Alloc(sizeof(TYPE)); // space for the virtual pointer and a pointer to the class data
                valtp->type = bt_pointer;
                valtp->array = TRUE;
                valtp->size = 2 * stdpointer.size;
                valtp->btp = &stdpointer;
                valtp->rootType = valtp;
                valtp->esize = intNode(en_c_i, 2);
                val = makeID(sc_auto, valtp, NULL, AnonymousName());
                val->allocate = TRUE;
                insert(val, localNameSpace->syms);
                sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
                funcparams->arguments = arg;
                funcparams->sp = sp;
                funcparams->functp = sp->tp;
                funcparams->fcall = varNode(en_pc, sp);
                funcparams->ascall = TRUE;
                arg->tp = &stdpointer;
                arg->exp = varNode(en_auto, val);
                arg->next = arg2;
                arg2->tp = &stdpointer;
                if (!byType && isstructured(*tp) && basetype(*tp)->sp->hasvtab)
                {
                    deref(&stdpointer, exp);
                    *exp = exprNode(en_sub, *exp, intNode(en_c_i, VTAB_XT_OFFS));
                    deref(&stdpointer, exp);
                    arg2->exp = *exp;
                }
                else
                {
                    rtti = RTTIDumpType(*tp);
                    arg2->exp = rtti ? varNode(en_global, rtti) : intNode(en_c_i, 0);
                }
                *exp = exprNode(en_func, 0, 0);
                (*exp)->v.func = funcparams;
                sp = namespacesearch("std", globalNameSpace, FALSE, FALSE);
                if (sp->storage_class == sc_namespace)
                {
                    sp = namespacesearch("type_info", sp->nameSpaceValues, TRUE, FALSE);
                    if (sp)
                    {
                        if (!sp->tp->syms)
                            error(ERR_NEED_TYPEINFO_H);
                        *tp = Alloc(sizeof(TYPE));
                        (*tp)->type = bt_const;
                        (*tp)->size = sp->tp->size;
                        (*tp)->btp = sp->tp;
                        (*tp)->rootType = sp->tp->rootType;
                    }
                }
            }
        }
        needkw(&lex, closepa);
    }
    return lex;
}
BOOLEAN insertOperatorParams(SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, FUNCTIONCALL *funcparams, int flags)
{
    SYMBOL *s2 = NULL, *s3;
    HASHREC **hrd, *hrs;
    char *name = overloadNameTab[openpa - kw_new + CI_NEW];
    TYPE *tpx;
    if (!isstructured(*tp) && basetype(*tp)->type != bt_enum)
        return FALSE;
    if (isstructured(*tp))
    {
        STRUCTSYM l ;
        l.str = (void *)basetype(*tp)->sp;
        addStructureDeclaration(&l);
        s2 = classsearch(name, FALSE, TRUE);
        dropStructureDeclaration();
        funcparams->thistp = Alloc(sizeof(TYPE));
        funcparams->thistp->type = bt_pointer;
        funcparams->thistp->size = getSize(bt_pointer);
        funcparams->thistp->btp = *tp;        
        funcparams->thistp->rootType = funcparams->thistp;        
        funcparams->thisptr = *exp;
    }
    // quit if there are no matches because we will use the default...
    if (!s2)
    {
        funcparams->thistp = NULL;
        funcparams->thisptr = NULL;
        return FALSE;
    }
    // finally make a shell to put all this in and add shims for any builtins we want to try
    tpx = (TYPE *)Alloc(sizeof(TYPE));
    tpx->type = bt_aggregate;
    tpx->rootType = tpx;
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
    funcparams->ascall = TRUE;    
    
    s3 = GetOverloadedFunction(tp, &funcparams->fcall, s3, funcparams, NULL, FALSE, FALSE, TRUE, flags);
    if (s3)
    {
        if (!isExpressionAccessible(NULL, s3, funcsp, funcparams->thisptr, FALSE))
            errorsym(ERR_CANNOT_ACCESS, s3);		
        s3->throughClass = s3->parentClass != NULL;
        funcparams->sp = s3;
        funcparams->functp = s3->tp;
        *exp = intNode(en_func, 0);
        (*exp)->v.func = funcparams;
        *tp = s3->tp;
        CheckCalledException(s3, *exp);
        return TRUE;
    }
    funcparams->thistp = NULL;
    funcparams->thisptr = NULL;
    return FALSE;
}
BOOLEAN insertOperatorFunc(enum ovcl cls, enum e_kw kw, SYMBOL *funcsp, 
                        TYPE **tp, EXPRESSION **exp, TYPE *tp1, EXPRESSION *exp1, INITLIST *args, int flags)
{
    static SYMBOL *s30;
    SYMBOL *s1 = NULL, *s2 = NULL, *s3, *s4 = NULL, *s5 = NULL;
    HASHREC **hrd, *hrs;
    FUNCTIONCALL *funcparams;
    char *name = overloadNameTab[kw - kw_new + CI_NEW];
    TYPE *tpin = *tp;
    TYPE *tpx;
    STRUCTSYM l ;
    if (!*tp)
        return FALSE;
    if (!isstructured(*tp) && basetype(*tp)->type != bt_enum
        && ((!tp1 && !args) || (tp1 && !isstructured(tp1) && basetype(tp1)->type != bt_enum)))
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
        case ovcl_assign_any:
        case ovcl_assign_numericptr:
        case ovcl_assign_numeric:
        case ovcl_assign_int:
            s1 = search(name, localNameSpace->syms);
            if (!s1)
                s1 = namespacesearch(name, localNameSpace, FALSE, FALSE);
            if (!s1 && enumSyms)
                s1 = search(name, enumSyms->tp->syms);
            if (!s1)
               s1 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
            break;
        default:
            break;
    }
    // next find some occurrance in the class or struct
    if (isstructured(*tp))
    {
        int n;
        l.str = (void *)basetype(*tp)->sp;
        addStructureDeclaration(&l);
        s2 = classsearch(name, FALSE, TRUE);
        n = PushTemplateNamespace(basetype(*tp)->sp); // used for more than just templates here
        s4 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
        PopTemplateNamespace(n);
    }
    else
    {
        l.str = NULL;
    }
    if (tp1 && isstructured(tp1))
    {
        int n = PushTemplateNamespace(basetype(tp1)->sp); // used for more than just templates here
        s5 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
        PopTemplateNamespace(n);
    }
    // quit if there are no matches because we will use the default...
    if (!s1 && !s2 && !s4 && !s5)
    {
        if (l.str)
            dropStructureDeclaration();
        return FALSE;
    }
    // finally make a shell to put all this in and add shims for any builtins we want to try
    tpx = (TYPE *)Alloc(sizeof(TYPE));
    tpx->type = bt_aggregate;
    tpx->rootType = tpx;
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
    if (s4)
    {
        hrs = s4->tp->syms->table[0];
        while (hrs)
        {
            HASHREC *ins = Alloc(sizeof(HASHREC));
            ins->p = hrs->p;
            *hrd = ins;
            hrd = &(*hrd)->next;
            hrs = hrs->next;
        }
    }
    if (s5)
    {
        hrs = s5->tp->syms->table[0];
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
    {
        INITLIST *one = NULL, *two = NULL;
        if (*tp)
        {
            one = Alloc(sizeof(INITLIST));
            one->exp = *exp;
            one->tp = *tp;
            funcparams->arguments = one;
        }
        if (args)
        {
            INITLIST *one = Alloc(sizeof(INITLIST));
            one->nested = args;
            funcparams->arguments = one;
        }
        else if (tp1)
        {
            two = Alloc(sizeof(INITLIST));
            two->exp = exp1;
            two->tp = tp1;
            funcparams->arguments = two;
        }
        else if (cls == ovcl_unary_postfix)
        {
            two = Alloc(sizeof(INITLIST));
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
    funcparams->ascall = TRUE;    
    s30 = s3;
    s3 = GetOverloadedFunction(tp, &funcparams->fcall, s3, funcparams, NULL, FALSE, FALSE, TRUE, flags);
    if (s3)
    {
        if (!isExpressionAccessible(NULL, s3, funcsp, funcparams->thisptr, FALSE))
            errorsym(ERR_CANNOT_ACCESS, s3);		
        if (ismember(s3))
        {
            funcparams->arguments = funcparams->arguments->next;
            funcparams->thistp = Alloc(sizeof(TYPE));
            funcparams->thistp->type = bt_pointer;
            funcparams->thistp->size = getSize(bt_pointer);
            funcparams->thistp->btp = tpin;        
            funcparams->thistp->rootType = funcparams->thistp;        
            funcparams->thisptr = *exp;
        }
        s3->throughClass = s3->parentClass != NULL;
        funcparams->sp = s3;
        funcparams->functp = s3->tp;
        *exp = intNode(en_func, 0);
        (*exp)->v.func = funcparams;
        *tp = s3->tp;
        expression_arguments(NULL, funcsp, tp, exp, 0);
        if (s3->defaulted && kw == assign)
            createAssignment(s3->parentClass, s3);
        if (l.str)
            dropStructureDeclaration();
        CheckCalledException(s3, funcparams->thisptr);
        return TRUE;
    }
    if (l.str)
        dropStructureDeclaration();
    return FALSE;
}
LEXEME *expression_new(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BOOLEAN global, int flags)
{
    FUNCTIONCALL *placement = Alloc(sizeof(FUNCTIONCALL));
    FUNCTIONCALL *initializers = NULL;
    char *name = overloadNameTab[CI_NEW];
    TYPE *tpf = NULL;
    EXPRESSION *arrSize = NULL, *exp1;
    SYMBOL *s1 = NULL;
    EXPRESSION *val = NULL, *newfunc = NULL;
    *exp = NULL;
    *tp = NULL;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (startOfType(lex, FALSE))
        {
            // type in parenthesis
            lex = get_type_id(lex, tp, funcsp, sc_cast, FALSE, TRUE);
            needkw(&lex, closepa);
        }
        else
        {
            // placement new
            lex = backupsym();
            lex = getArgs(lex, funcsp, placement, closepa, TRUE, 0);
        }
    }
    if (!*tp)
    {
        if (MATCHKW(lex, openpa))
        {
            // type in parenthesis
            lex = getsym();
            lex = get_type_id(lex, tp, funcsp, sc_cast, FALSE, TRUE);
            needkw(&lex, closepa);
        }
        else
        {
            // new type id
            enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            BOOLEAN defd = FALSE;
            SYMBOL *sp = NULL;
            BOOLEAN notype = FALSE;
            lex = get_type_id(lex, tp, funcsp, sc_auto, TRUE, TRUE);
            if (MATCHKW(lex, openbr))
            {
                TYPE *tp1 = NULL;
                EXPRESSION *exp = NULL;
                name = overloadNameTab[CI_NEWA];
                lex = getsym();
                lex = expression(lex, funcsp, NULL, &tp1, &exp, flags);
                if (!isint(tp1))
                {
                    error(ERR_NEED_INTEGER_TYPE);
                }
                needkw(&lex, closebr);
                ParseAttributeSpecifiers(&lex, funcsp, TRUE);
                arrSize = exp;
                while (MATCHKW(lex, openbr))
                {
                    lex = getsym();
                    lex = expression(lex, funcsp, NULL, &tp1, &exp, flags);
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
                    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
                }
                optimize_for_constants(&arrSize);
            }
        }
    }
    if (*tp)
    {
        EXPRESSION *sz;
        INITLIST *sza;
        int n;
        if (isstructured(*tp))
        {
            *tp = PerformDeferredInitialization(*tp, funcsp);
            n = basetype(*tp)->sp->tp->size;
        }
        else
        {
            n = (*tp)->size;
        }
        if (isref(*tp))
            error(ERR_NEW_NO_ALLOCATE_REFERENCE);
        if (arrSize && isstructured(*tp))
        {
            int al = n % basetype(*tp)->sp->structAlign;
            if (al != 0)
                n += basetype(*tp)->sp->structAlign - al;
        }
        sz = intNode(en_c_i, n);
        if (arrSize)
        {
            sz = exprNode(en_mul, sz, arrSize);
        }
        optimize_for_constants(&sz);
        sza = Alloc(sizeof(INITLIST));
        sza->exp = sz;
        sza->tp = &stdint;
        sza->next = placement->arguments;
        placement->arguments = sza;
    }
    else
    {
        error(ERR_TYPE_NAME_EXPECTED);
        *tp = &stdint; // error handling
    }
    if (!global)
    {
        if (isstructured(*tp))
        {
            STRUCTSYM l ;
            
            l.str = (void *)basetype(*tp)->sp;
            addStructureDeclaration(&l);
            s1 = classsearch(name, FALSE, TRUE);
            dropStructureDeclaration();
        }
    }
    if (!s1)
    {
        s1 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
    }
    // should have always found something
    placement->ascall = TRUE;    
    s1 = GetOverloadedFunction(&tpf, &placement->fcall, s1, placement, NULL, TRUE, FALSE, TRUE, flags);
    if (s1)
    {
        SYMBOL *sp;
        val = anonymousVar(sc_auto, &stdpointer);
        sp = val->v.sp;
        sp->decoratedName = sp->errname = sp->name;
//        if (localNameSpace->syms)
//            insert(sp, localNameSpace->syms);
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
//        placement->noinline = (flags & _F_NOINLINE) | s1->noinline; 
        exp1 = intNode(en_func, 0);
        exp1->v.func = placement;
        newfunc = exp1;
    }
    if (KW(lex) == openpa)
    {
        // initializers
        if (isstructured(*tp))
        {
            initializers = Alloc(sizeof(FUNCTIONCALL));
            lex = getArgs(lex, funcsp, initializers, closepa, TRUE, 0);
            if (s1)
            {
                *exp = val;
                tpf = *tp;
                callConstructor(&tpf, exp, initializers, FALSE, arrSize, TRUE, FALSE, FALSE, TRUE, FALSE);
            }
        }
        else
        {
            exp1 = NULL;
            initializers = Alloc(sizeof(FUNCTIONCALL));
            lex = getArgs(lex, funcsp, initializers, closepa, TRUE, 0);
            if (initializers->arguments)
            {
                if (!comparetypes(initializers->arguments->tp, *tp, FALSE) || initializers->arguments->next)
                {
                    error(ERR_NEED_NUMERIC_EXPRESSION);
                }
                else
                {
                    exp1 = initializers->arguments->exp;
                    if ((*tp)->type == bt_auto)
                        *tp = initializers->arguments->tp;
                    if (exp1 && val)
                    {
                        EXPRESSION *pval = val;
                        cast(*tp, &exp1);
                        deref(*tp, &pval);
                        *exp = exprNode(en_assign, pval, exp1);
                    }
                }
            }
            else
            {
                // default-construct
                if (val)
                {
                    EXPRESSION *pval = val;
                    exp1 = intNode(en_c_i, 0);
                    cast(*tp, &exp1);
                    deref(*tp, &pval);
                    *exp = exprNode(en_assign, pval, exp1);
                }
            }
        }
    }
    else if (KW(lex) == begin)
    {
        // braced initializers
        // this is just a little buggy, for example new aa[10][10] { {1},{2},{3} } won't work properly
        // i can't make it work well because the array indexes aren't necessarily constants...
        if (*tp)
        {
            INITIALIZER *init = NULL;
            EXPRESSION *base = val;
            TYPE *tp1 = *tp;
            SYMBOL *sp = NULL;
            if (isstructured(*tp))
            {
                sp = anonymousVar(sc_localstatic, *tp)->v.sp;
            }
            if (arrSize)
            {
                tp1 = Alloc(sizeof(TYPE));
                tp1->size = 0;
                tp1->array = 1;
                tp1->type = bt_pointer;                
                tp1->btp = *tp;
                tp1->rootType = tp1;
                tp1->esize = arrSize;
            }
            lex = initType(lex, funcsp, 0, sc_auto, &init, NULL, tp1, sp, FALSE, 0);
            if (!isstructured(*tp) && !arrSize)
            {
                if (!init || init->next || (init->basetp && isstructured(init->basetp)))
                    error(ERR_NONSTRUCTURED_INIT_LIST);
            }
            // dest is lost for these purposes
            if (theCurrentFunc)
            {
                if (init->exp)
                {
                    *exp = convertInitToExpression(tp1, NULL, funcsp, init, base, FALSE);
                }
                if (arrSize && !isstructured(*tp))
                {
                    if (arrSize)
                    {
                        exp1 = exprNode(en_blockclear, base, exprNode(en_mul, arrSize, intNode(en_c_i, (*tp)->size)));
                    }
                    else
                    {
                        exp1 = exprNode(en_blockclear, base, NULL);
                        exp1->size = (*tp)->size;
                    }
                    if (*exp)
                        *exp = exprNode(en_void, exp1, *exp);
                    else
                        *exp = exp1;
                }
                else if (isstructured(*tp) && (arrSize || !init->exp))
                {
                    EXPRESSION *exp1;
                    exp1 = val;
                    if (arrSize)
                    {
                        INITIALIZER *it = init;
                        while (it && it->exp)
                            it= it->next;
                        arrSize = exprNode(en_sub, arrSize, intNode(en_c_i, it->offset/(basetype(*tp)->size)));
                        exp1 = exprNode(en_add, exp1, intNode(en_c_i, it->offset));
                    }
                    tpf = *tp;
                    callConstructor(&tpf, &exp1, NULL, FALSE, arrSize, TRUE, FALSE, FALSE, TRUE, FALSE);
                    if (*exp)
                    {
                        *exp = exprNode(en_void, *exp, exp1);
                    }
                    else
                    {
                        *exp = exp1;
                    }
                }
            }
        }
    }
    else if (isstructured(*tp))
    {
        if (s1)
        {
            // call default constructor
            *exp = val;
            tpf = *tp;
            callConstructor(&tpf, exp, NULL, FALSE, arrSize, TRUE, FALSE, FALSE, TRUE, FALSE);
        }
    }
    tpf = Alloc(sizeof(TYPE));
    tpf->type = bt_pointer;
    tpf->size = getSize(bt_pointer);
    tpf->btp = *tp;
    tpf->rootType = tpf;
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
LEXEME *expression_delete(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BOOLEAN global, int flags)
{
    BOOLEAN byArray = FALSE;
    SYMBOL *s1 = NULL;
    FUNCTIONCALL *funcparams;
    INITLIST *one = NULL;
    EXPRESSION *exp1 = NULL, *exp2;
    TYPE *tpf;
    char *name = overloadNameTab[CI_DELETE];
    EXPRESSION *in;
    EXPRESSION *var, *asn;
    lex = getsym();
    if (MATCHKW(lex, openbr))
    {
        lex = getsym();
        needkw(&lex, closebr);
        byArray = TRUE;
        name = overloadNameTab[CI_DELETEA];
        exp1 = intNode(en_c_i, 0); // signal to the runtime to load the number of elems dynamically
    }
    lex = expression_cast(lex, funcsp, NULL, tp, exp, NULL, flags);
    if (!*tp)
    {
        error(ERR_IDENTIFIER_EXPECTED);
        *tp = &stdint; // error handling
    }
    if (!ispointer(*tp))
    {
        error(ERR_POINTER_TYPE_EXPECTED);
        *exp = intNode(en_c_i, 0);
        return lex;
    }
    if (!templateNestingCount && isstructured(basetype(*tp)->btp))
    {
        basetype(*tp)->btp = PerformDeferredInitialization(basetype(*tp)->btp, funcsp);
        if (basetype(basetype(*tp)->btp)->sp->tp->size == 0)
            errorsym(ERR_DELETE_OF_POINTER_TO_UNDEFINED_TYPE, basetype(basetype(*tp)->btp)->sp);
    }
    asn = *exp;
    var = anonymousVar(sc_auto, *tp);
    deref(*tp, &var);
    in = exp2 = *exp = var;
    if (basetype(*tp)->btp && isstructured(basetype(*tp)->btp))
    {
        callDestructor(basetype(basetype(*tp)->btp)->sp, NULL, exp, exp1, TRUE, TRUE, FALSE);
    }
    exp1 = exp2;
    if (!global)
    {
        if (isstructured(*tp))
        {
            STRUCTSYM l ;
            l.str = (void *)basetype(*tp)->sp;
            addStructureDeclaration(&l);
            s1 = classsearch(name, FALSE, TRUE);
            dropStructureDeclaration();
        }
    }
    if (!s1)
    {
        s1 = namespacesearch(name, globalNameSpace, FALSE, FALSE);
    }
    // should always find something
    funcparams = Alloc(sizeof(FUNCTIONCALL));
    one = Alloc(sizeof(INITLIST));
    one->exp =exp1;
    one->tp = &stdpointer;
    funcparams->arguments = one;
    funcparams->ascall = TRUE;
    s1 = GetOverloadedFunction(&tpf, &funcparams->fcall, s1, funcparams, NULL, TRUE, FALSE, TRUE, flags);
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
        //funcparams->noinline =  (flags & _F_NOINLINE) | s1->noinline; 
        exp1 = intNode(en_func, 0);
        exp1->v.func = funcparams;
        exp1 = exprNode(en_void, *exp, exp1);
        exp1 = exprNode(en_voidnz, exprNode(en_void, in, exp1), intNode(en_c_i, 0));
        *exp = exp1;
        *tp = s1->tp;
    }
    var = exprNode(en_assign, var, asn);
    *exp = exprNode(en_void, var, *exp); 
    *tp = &stdvoid;
    return lex;
}
BOOLEAN isNoexcept(EXPRESSION *exp)
{
    return FALSE;
}
static BOOLEAN noexceptStmt(STATEMENT *block);
static BOOLEAN noexceptExpression(EXPRESSION *node)
{
    FUNCTIONCALL *fp;
    BOOLEAN rv = TRUE;
    if (node == 0)
        return rv;
    switch (node->type)
    {
        case en_auto:
            break;
        case en_const:
            break;
        case en_c_ll:
        case en_c_ull:
        case en_c_d:
        case en_c_ld:
        case en_c_f:
        case en_c_dc:
        case en_c_ldc:
        case en_c_fc:
        case en_c_di:
        case en_c_ldi:
        case en_c_fi:
        case en_c_i:
        case en_c_l:
        case en_c_ui:
        case en_c_ul:
        case en_c_c:
        case en_c_bool:
        case en_c_uc:
        case en_c_wc:
        case en_c_u16:
        case en_c_u32:        
        case en_nullptr:
        case en_memberptr:
        case en_structelem:
            break;
        case en_global:
        case en_label:
        case en_pc:
        case en_labcon:
        case en_absolute:
        case en_threadlocal:
            break;
        case en_l_sp:
        case en_l_fp:
        case en_bits:
        case en_l_f:
        case en_l_d:
        case en_l_ld:
        case en_l_fi:
        case en_l_di:
        case en_l_ldi:
        case en_l_fc:
        case en_l_dc:
        case en_l_ldc:
        case en_l_c:
        case en_l_wc:
        case en_l_u16:
        case en_l_u32:
        case en_l_s:
        case en_l_ul:
        case en_l_l:
        case en_l_p:
        case en_l_ref:
        case en_l_i:
        case en_l_ui:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
        case en_literalclass:
            rv = noexceptExpression(node->left);
            break;
        case en_uminus:
        case en_compl:
        case en_not:
        case en_x_f:
        case en_x_d:
        case en_x_ld:
        case en_x_fi:
        case en_x_di:
        case en_x_ldi:
        case en_x_fc:
        case en_x_dc:
        case en_x_ldc:
        case en_x_ll:
        case en_x_ull:
        case en_x_i:
        case en_x_ui:
        case en_x_c:
        case en_x_u16:
        case en_x_u32:
        case en_x_wc:
        case en_x_uc:
        case en_x_bool:
        case en_x_bit:
        case en_x_s:
        case en_x_us:
        case en_x_l:
        case en_x_ul:
        case en_x_p:
        case en_x_fp:
        case en_x_sp:
        case en_trapcall:
        case en_shiftby:
/*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
            rv = noexceptExpression(node->left);
            break;
        case en_assign:
            rv = noexceptExpression(node->right) && noexceptExpression(node->left);
            break;
        case en_autoinc:
        case en_autodec:
            rv = noexceptExpression(node->left);
            break;
        case en_add:
        case en_sub:
/*        case en_addcast: */
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_rshd:
        case en_void:
        case en_voidnz:
/*        case en_dvoid: */
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_intcall:
        case en_stackblock:
        case en_blockassign:
        case en_mp_compare:
/*		case en_array: */
            rv = noexceptExpression(node->right) && noexceptExpression(node->left);
            break;
        case en_mp_as_bool:
        case en_blockclear:
        case en_argnopush:
        case en_not_lvalue:
        case en_lvalue:
        case en_thisref:
        case en_funcret:
            rv = noexceptExpression(node->left);
            break;
        case en_atomic:
            break;
        case en_func:
            fp = node->v.func;
            {
                SYMBOL *sp = fp->sp;
                rv = sp->xcMode == xc_none || (sp->xcMode == xc_dynamic && (!sp->xc || !sp->xc->xcDynamic));
            }
            break;
        case en_stmt:
            rv = noexceptStmt(node->v.stmt);
            break;
        default:
            diag("noexceptExpression");
            break;
    }
    return rv;
}
static BOOLEAN noexceptStmt(STATEMENT *block)
{
    BOOLEAN rv = TRUE;
    while (block != NULL)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_catch:
            case st_try:
                rv &= noexceptStmt(block->lower);
                break;
            case st_return:
            case st_expr:
            case st_declare:
                rv &= noexceptExpression(block->select);
                break;
            case st_goto:
            case st_label:
                break;
            case st_select:
            case st_notselect:
                rv &= noexceptExpression(block->select);
                break;
            case st_switch:
                rv &= noexceptExpression(block->select);
                rv &= noexceptStmt(block->lower);
                break;
            case st_block:
                rv &= noexceptStmt(block->lower);
                rv &= noexceptStmt(block->blockTail);
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
                diag("Invalid block type in noexceptStmt");
                break;
        }
        block = block->next;
    }
    return rv;
}
LEXEME *expression_noexcept(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp)
{
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        lex = expression_no_check(lex, funcsp, NULL, tp, exp, _F_SIZEOF);
        *exp = intNode(en_c_i, noexceptExpression(*exp));
        *tp = &stdbool;
        
        needkw(&lex, closepa);
    }
    return lex;
}
