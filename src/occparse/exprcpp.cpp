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
#include <cassert>
#include "rtti.h"
#include "ccerr.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "mangle.h"
#include "template.h"
#include "lambda.h"
#include "declare.h"
#include "expr.h"
#include "help.h"
#include "types.h"
#include "declcons.h"
#include "declcpp.h"
#include "constopt.h"
#include "cpplookup.h"
#include "init.h"
#include "OptUtils.h"
#include "lex.h"
#include "beinterf.h"
#include "Property.h"
#include "memory.h"
#include "exprcpp.h"
#include "constexpr.h"
#include "symtab.h"
#include "ListFactory.h"
namespace Parser
{

/* lvaule */
/* handling of const int */
/*--------------------------------------------------------------------------------------------------------------------------------
 */
void checkscope(TYPE* tp1, TYPE* tp2)
{
    tp1 = basetype(tp1);
    tp2 = basetype(tp2);
    if (tp1->scoped != tp2->scoped)
    {
        error(ERR_SCOPED_TYPE_MISMATCH);
    }
    else if (tp1->scoped && tp2->scoped)
    {
        SYMBOL *sp1 = nullptr, *sp2 = nullptr;
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
static EXPRESSION** findPointerToExpression(EXPRESSION** parent, EXPRESSION* expr)
{
    EXPRESSION** y;
    if (*parent == expr)
        return parent;
    y = findPointerToExpression(&(*parent)->left, expr);
    if (y)
        return y;
    y = findPointerToExpression(&(*parent)->right, expr);
    if (y)
        return y;
    return nullptr;
}
EXPRESSION* baseClassOffset(SYMBOL* base, SYMBOL* derived, EXPRESSION* en)
{
    EXPRESSION* rv = en;
    if (base != derived)
    {
        bool found = false;
        if (derived->sb->vbaseEntries)
        { 
            for (auto vbase : *derived->sb->vbaseEntries)
            {
                // this will get all virtual bases since they are all listed on each deriviation
                if (vbase->alloc && vbase->cls == base)
                {
                    EXPRESSION* ec = intNode(en_c_i, vbase->pointerOffset);
                    rv = exprNode(en_add, en, ec);
                    deref(&stdpointer, &rv);
                    found = true;
                    break;
                }
            }
        }
        if (!found && derived->sb->baseClasses)
        {
            auto it = derived->sb->baseClasses->begin();
            auto ite = derived->sb->baseClasses->end();
            std::deque<std::pair<std::list<BASECLASS*>::iterator, std::list<BASECLASS*>::iterator>> stk;
            if (it != ite)
                stk.push_back(std::pair<std::list<BASECLASS*>::iterator, std::list<BASECLASS*>::iterator>(it, ite));
            while (it != ite)
            {
                if ((*it)->cls == base)
                    break;
                if ((*it)->cls->sb->baseClasses)
                {
                    ite = (*it)->cls->sb->baseClasses->end();
                    it = (*it)->cls->sb->baseClasses->begin();
                    stk.push_back(std::pair<std::list<BASECLASS*>::iterator, std::list<BASECLASS*>::iterator>(it, ite));
                }
                else if (++it != ite)
                {
                    stk.back() = std::pair<std::list<BASECLASS*>::iterator, std::list<BASECLASS*>::iterator>(it, ite);
                }
                else
                {
                    --it;
                    if (stk.size())
                    {
                        do
                        {
                            ite = stk.back().second;
                            it = stk.back().first;
                            ++it;
                            stk.pop_back();
                        } while (stk.size() && it == ite);
                        stk.push_back(std::pair<std::list<BASECLASS*>::iterator, std::list<BASECLASS*>::iterator>(it, ite));
                    }
                }
            }
            if (stk.size() && stk.front().first != stk.front().second)
            {
                int i = 0;
                for (auto it = stk.begin(); it != stk.end(); ++it)
                {
                    auto entry = *(*it).first;
                    if (entry->isvirtual)
                    {
                        int offset;
                        std::list<VBASEENTRY*>::iterator itl, itle = itl, itx;
                        if (i == 0)
                        {
                            itl = derived->sb->vbaseEntries->begin();
                            itle = derived->sb->vbaseEntries->end();
                        }
                        else
                        {
                            itl = entry->cls->sb->vbaseEntries->begin();
                            itle = entry->cls->sb->vbaseEntries->end();                        
                        }
                        for (itx = itl; itx != itl && (*itx)->cls != entry->cls; ++itx)
                            ;
                        offset = (*itx)->pointerOffset;
                        rv = exprNode(en_add, rv, intNode(en_c_i, offset));
                        if (entry->isvirtual)
                            deref(&stdpointer, &rv);
                    }
                    else
                    {
                        int offset = entry->offset;
                        rv = exprNode(en_add, rv, intNode(en_c_i, offset));
                    }
                }
            }
        }
    }
    return rv;
}
void qualifyForFunc(SYMBOL* sym, TYPE** tp, bool isMutable)
{
    if (sym)
    {
        if (isconst(sym->tp) && !isMutable)
        {
            *tp = MakeType(bt_const, *tp);
        }
        if (isvolatile(sym->tp))
        {
            *tp = MakeType(bt_volatile, *tp);
        }
    }
}
void getThisType(SYMBOL* sym, TYPE** tp)
{
    *tp = MakeType(bt_pointer, sym->sb->parentClass->tp);
    qualifyForFunc(sym, tp, false);
}
EXPRESSION* getMemberBase(SYMBOL* memberSym, SYMBOL* strSym, SYMBOL* funcsp, bool toError)
{
    EXPRESSION* en;
    SYMBOL* enclosing = getStructureDeclaration();

    if (enclosing && (!funcsp || (funcsp->sb->storage_class != sc_global &&
                                  funcsp->sb->storage_class != sc_static)))  // lambdas will be caught by this too
    {
        if (strSym)
        {
            if (!comparetypes(strSym->tp, enclosing->tp, true))
            {
                int n = classRefCount(strSym, enclosing);
                if (n == 0)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
                else if (n > 1)
                    errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, strSym, enclosing);
            }
            //            enclosing = strSym;
        }
        if (funcsp)
            en = varNode(en_auto, (SYMBOL*)basetype(funcsp->tp)->syms->front());  // this ptr
        else
            en = intNode(en_thisshim, 0);
        if (lambdas.size() && !memberSym->sb->parentClass->sb->islambda)
        {
            if (!lambdas.front()->lthis || !lambdas.front()->captured)
            {
                en = intNode(en_c_i, 0);
                if (toError)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
            }
            else
            {
                SYMBOL* sym = search(lambdas.front()->cls->tp->syms, "$this");
                enclosing = basetype(lambdas.front()->lthis->tp)->btp->sp;
                if (sym)
                {
                    deref(&stdpointer, &en);
                    en = exprNode(en_add, en, intNode(en_c_i, sym->sb->offset));
                }
                else
                {
                    diag("getMemberBase: cannot find lambda this");
                }
            }
        }
        deref(&stdpointer, &en);
        if (enclosing != memberSym->sb->parentClass && enclosing->sb->mainsym != memberSym->sb->parentClass)
        {
            if (toError && classRefCount(memberSym->sb->parentClass, enclosing) != 1)
            {
                errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, memberSym->sb->parentClass, enclosing);
            }
            else if (toError && !isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, memberSym, funcsp, en, false))
            {
                errorsym(ERR_CANNOT_ACCESS, memberSym);
            }
            en = baseClassOffset(memberSym->sb->parentClass, enclosing, en);
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
EXPRESSION* getMemberNode(SYMBOL* memberSym, SYMBOL* strSym, TYPE** tp, SYMBOL* funcsp)
{
    EXPRESSION* en = getMemberBase(memberSym, strSym, funcsp, true);
    en = exprNode(en_structadd, en, intNode(en_c_i, memberSym->sb->offset));
    *tp = memberSym->tp;
    return en;
}
EXPRESSION* getMemberPtr(SYMBOL* memberSym, SYMBOL* strSym, TYPE** tp, SYMBOL* funcsp)
{
    (void)strSym;
    EXPRESSION* rv;
    *tp = MakeType(bt_memberptr, memberSym->tp);
    (*tp)->sp = memberSym->sb->parentClass;
    rv = varNode(en_memberptr, memberSym);
    rv->isfunc = true;
    if (!isExpressionAccessible(nullptr, memberSym, funcsp, nullptr, true))
    {
        errorsym(ERR_CANNOT_ACCESS, memberSym);
    }
    return rv;
}
bool castToArithmeticInternal(bool integer, TYPE** tp, EXPRESSION** exp, enum e_kw kw, TYPE* other, bool implicit)
{
    (void)kw;
    SYMBOL* sym = basetype(*tp)->sp;

    if (other && ispointer(other) && (kw == plus || kw == minus || (kw >= eq && kw <= geq)))
    {
        return castToPointer(tp, exp, kw, other);
    }
    if (!other || isarithmetic(other))
    {
        SYMBOL* cst = integer ? lookupIntCast(sym, other ? other : &stdint, implicit)
                              : lookupArithmeticCast(sym, other ? other : &stddouble, implicit);
        if (cst)
        {
            FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
            EXPRESSION* e1;
            params->fcall = varNode(en_pc, cst);
            params->thisptr = *exp;
            params->thistp = MakeType(bt_pointer, cst->sb->parentClass->tp);
            params->functp = cst->tp;
            params->sp = cst;
            params->ascall = true;
            if ((*exp)->type == en_literalclass)
            {
                *exp = substitute_params_for_function(params, (*exp)->v.syms);
                optimize_for_constants(exp);
                if (!cst->sb->constexpression || !IsConstantExpression(*exp, true, false))
                    error(ERR_CONSTANT_FUNCTION_EXPECTED);
            }
            else
            {
                *exp = DerivedToBase(cst->sb->parentClass->tp, *tp, *exp, 0);
                params->thisptr = *exp;
                {
                    e1 = varNode(en_func, nullptr);
                    e1->v.func = params;
                    if (params->sp->sb->xcMode != xc_unspecified && params->sp->sb->xcMode != xc_none)
                        hasFuncCall = true;
                }
                *exp = e1;
                if (other)
                    cast(other, exp);
            }
            *tp = basetype(cst->tp)->btp;
            return true;
        }
    }
    return false;
}
void castToArithmetic(bool integer, TYPE** tp, EXPRESSION** exp, enum e_kw kw, TYPE* other, bool implicit)
{
    if (Optimizer::cparams.prm_cplusplus && isstructured(*tp))
    {
        if (!castToArithmeticInternal(integer, tp, exp, kw, other, implicit))
        {
            // failed at conversion
            if (kw >= kw_new && kw <= complx)
            {
                // LHS, put up an operator whatever message
                char buf[4096];
                char tbuf[4096];
                memset(tbuf, 0, sizeof(tbuf));
                tbuf[0] = 0;
                typeToString(tbuf, basetype(*tp));
                if (other)
                {
                    strcat(tbuf, ", ");
                    typeToString(tbuf + strlen(tbuf), basetype(other));
                }
                Optimizer::my_sprintf(buf, "operator %s(%s)", overloadXlateTab[kw - kw_new + CI_NEW], tbuf);
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
        else
        {
            if (isref(*tp))
            {
                *tp = basetype(*tp)->btp;
                deref(*tp, exp);
            }
        }
    }
}
bool castToPointer(TYPE** tp, EXPRESSION** exp, enum e_kw kw, TYPE* other)
{
    (void)kw;
    if (Optimizer::cparams.prm_cplusplus && isstructured(*tp))
    {
        SYMBOL* sym = basetype(*tp)->sp;
        if (ispointer(other) || basetype(other)->type == bt_memberptr)
        {
            SYMBOL* cst = lookupPointerCast(sym, other);
            if (cst)
            {
                FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                EXPRESSION* e1;
                if (isfuncptr(basetype(cst->tp)->btp) && isfuncptr(other))
                {
                    TYPE **tpx = &basetype(cst->tp)->btp, **tpy = &other;
                    while ((*tpx) && (*tpy) && (*tpx)->type != bt_auto)
                    {
                        tpx = &(*tpx)->btp;
                        tpy = &(*tpy)->btp;
                    }
                    if (*tpx && *tpy)
                        *tpx = *tpy;
                }
                *exp = DerivedToBase(cst->sb->parentClass->tp, *tp, *exp, 0);
                params->fcall = varNode(en_pc, cst);
                params->thisptr = *exp;
                params->thistp = MakeType(bt_pointer, cst->sb->parentClass->tp);
                params->functp = cst->tp;
                params->sp = cst;
                params->ascall = true;
                e1 = varNode(en_func, nullptr);
                e1->v.func = params;
                if (params->sp->sb->xcMode != xc_unspecified && params->sp->sb->xcMode != xc_none)
                    hasFuncCall = true;
                *exp = e1;
                if (ispointer(other))
                {
                    cast(other, exp);
                }
                else
                {
                    SYMBOL* retsp = makeID(sc_auto, other, nullptr, AnonymousName());
                    retsp->sb->allocate = true;
                    retsp->sb->attribs.inheritable.used = retsp->sb->assigned = true;
                    SetLinkerNames(retsp, lk_cdecl);
                    localNameSpace->front()->syms->Add(retsp);
                    params->returnSP = retsp;
                    params->returnEXP = varNode(en_auto, retsp);
                }
                *tp = basetype(cst->tp)->btp;
                return true;
            }
        }
    }
    return false;
}
bool cppCast(TYPE* src, TYPE** tp, EXPRESSION** exp)
{
    if (isstructured(src))
    {
        if (isstructured(*tp))
        {
            SYMBOL* sym = basetype(src)->sp;
            SYMBOL* cst = lookupSpecificCast(sym, *tp);
            if (cst)
            {
                FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                EXPRESSION* e1;
                CheckCalledException(cst, *exp);
                *exp = DerivedToBase(cst->sb->parentClass->tp, src, *exp, 0);
                params->fcall = varNode(en_pc, cst);
                params->thisptr = *exp;
                params->thistp = MakeType(bt_pointer, cst->sb->parentClass->tp);
                params->functp = cst->tp;
                params->sp = cst;
                params->ascall = true;
                if (isstructured(*tp))
                {
                    EXPRESSION* ev = anonymousVar(sc_auto, *tp);
                    SYMBOL* av = ev->v.sp;
                    params->returnEXP = ev;
                    params->returnSP = sym;
                    callDestructor(basetype(*tp)->sp, nullptr, &ev, nullptr, true, false, false, true);
                    initInsert(&av->sb->dest, *tp, ev, 0, true);
                }
                e1 = varNode(en_func, nullptr);
                e1->v.func = params;
                if (params->sp->sb->xcMode != xc_unspecified && params->sp->sb->xcMode != xc_none)
                    hasFuncCall = true;
                *exp = e1;
                *exp = DerivedToBase(*tp, basetype(cst->tp)->btp, *exp, 0);
                return true;
            }
        }
        else if (isarithmetic(*tp))
        {
            TYPE* tp1 = src;
            auto rv =  castToArithmeticInternal(false, &tp1, exp, (enum e_kw) - 1, *tp, false);
            if (isref(tp1))
                deref(basetype(tp1)->btp, exp);
            return rv;
        }
        else if (ispointer(*tp) || basetype(*tp)->type == bt_memberptr)
        {
            TYPE* tp1 = src;
            auto rv = castToPointer(&tp1, exp, (enum e_kw) - 1, *tp);
            if (isref(tp1))
                deref(basetype(tp1)->btp, exp);
            return rv;
        }
    }
    return false;
}
typedef struct _subslist
{
    struct _subslist* next;
    EXPRESSION* exp;
    SYMBOL* sp;
} SUBSTITUTIONLIST;

static EXPRESSION* substitute_vars(EXPRESSION* exp, SUBSTITUTIONLIST* match, SymbolTable<SYMBOL>* syms)
{
    EXPRESSION* rv;
    if (lvalue(exp))
    {
        if (exp->left->type == en_auto)
        {
            SYMBOL* sym = exp->left->v.sp;
            while (match)
            {
                if (sym == match->sp)
                {
                    return match->exp;
                }
                match = match->next;
            }
        }
        else if (syms && exp->v.sp)
        {
            for (auto sp : *syms)
            {
                CONSTEXPRSYM* ces = (CONSTEXPRSYM*)sp;
                if (ces->sp == exp->v.sp)
                {
                    return ces->exp;
                }
            }
        }
    }
    rv = Allocate<EXPRESSION>();
    *rv = *exp;
    if (exp->left)
        rv->left = substitute_vars(exp->left, match, syms);
    if (exp->right)
        rv->right = substitute_vars(exp->right, match, syms);
    return rv;
}
EXPRESSION* substitute_params_for_constexpr(EXPRESSION* exp, FUNCTIONCALL* funcparams, SymbolTable<SYMBOL>* syms)
{
    auto it = basetype(funcparams->sp->tp)->syms->begin();
    auto itend = basetype(funcparams->sp->tp)->syms->end();
    SUBSTITUTIONLIST *list = nullptr, **plist = &list;
    if (!funcparams->sp->sb->castoperator)
    {
        if ((*it)->sb->thisPtr)
            ++it;
        // because we already did function matching to get the constructor,
        // the worst that can happen here is that the specified arg list is shorter
        // than the actual parameters list
        if (funcparams->arguments)
        {
            auto ita = funcparams->arguments->begin();
            auto itae = funcparams->arguments->end();
            while (it != itend && ita != itae)
            {
                *plist = Allocate<SUBSTITUTIONLIST>();
                (*plist)->exp = (*ita)->exp;
                (*plist)->sp = *it;
                plist = &(*plist)->next;
                ++it;
                ++ita;
            }
        }
        while (it != itend)
        {
            *plist = Allocate<SUBSTITUTIONLIST>();
            (*plist)->exp = (*it)->sb->init->front()->exp;
            (*plist)->sp = (*it);
            plist = &(*plist)->next;
            ++it;
        }
    }
    return substitute_vars(exp, list, syms);
}
std::list<STATEMENT*>* do_substitute_for_function(std::list<STATEMENT*>* blocks, FUNCTIONCALL* funcparams, SymbolTable<SYMBOL>* syms)
{
    if (blocks)
    {
        std::list<STATEMENT*>* rv = stmtListFactory.CreateList();
        for (auto block : *blocks)
        {
            auto stmt = Allocate<STATEMENT>();
            *stmt = *block;
            rv->push_back(stmt);
            if (stmt->select)
                stmt->select = substitute_params_for_constexpr(block->select, funcparams, syms);
            if (stmt->lower)
                stmt->lower = do_substitute_for_function(block->lower, funcparams, syms);
        }
        return rv;
    }
    return nullptr;
}
EXPRESSION* substitute_params_for_function(FUNCTIONCALL* funcparams, SymbolTable<SYMBOL>* syms)
{
    auto st = do_substitute_for_function(funcparams->sp->sb->inlineFunc.stmt, funcparams, syms);
    EXPRESSION* exp = exprNode(en_stmt, 0, 0);
    exp->v.stmt = st;
    return exp;
}
LEXLIST* expression_func_type_cast(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
    bool defd = false;
    int consdest = false;
    bool notype = false;
    if (!(flags & _F_NOEVAL))
    {
        *tp = nullptr;
        lex = getBasicType(lex, funcsp, tp, nullptr, false, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd,
                           &consdest, nullptr, false, true, false, false, false);
        if (isstructured(*tp) && !(*tp)->size && (!templateNestingCount || !basetype(*tp)->sp->sb->templateLevel))
        {
            (*tp) = basetype(*tp)->sp->tp;
            if (!(*tp)->size)
                errorsym(ERR_STRUCT_NOT_DEFINED, basetype(*tp)->sp);
        }
    }
    if (!MATCHKW(lex, openpa))
    {
        if (MATCHKW(lex, begin))
        {
            flags &= ~_F_NOEVAL;
            std::list<INITIALIZER*>* init = nullptr, *dest = nullptr;
            SYMBOL* sym = nullptr;
            sym = anonymousVar(sc_auto, *tp)->v.sp;

            lex = initType(lex, funcsp, 0, sc_auto, &init, &dest, *tp, sym, false, flags | _F_EXPLICIT);
            if (init && init->size() == 1 && init->front()->exp->type == en_thisref)
            {
                *exp = init->front()->exp;
                if (sym && dest)
                {
                    sym->sb->dest = initListFactory.CreateList();
                    sym->sb->dest->insert(sym->sb->dest->begin(), dest->begin(), dest->end());
                }
            }
            else
            {
                *exp = convertInitToExpression(*tp, sym, nullptr, funcsp, init, nullptr, false);
                if (sym)
                {
                    EXPRESSION** e1 = exp;
                    if (*e1)
                    {
                        while ((*e1)->type == en_void && (*e1)->right)
                            e1 = &(*e1)->right;
                        if ((*e1)->type == en_void)
                            e1 = &(*e1)->left;
                        *e1 = exprNode(en_void, *e1, varNode(en_auto, sym));
                        if (dest)
                        {
                            sym->sb->dest = initListFactory.CreateList();
                            sym->sb->dest->insert(sym->sb->dest->begin(), dest->begin(), dest->end());
                        }
                    }
                }
            }
        }
        else
        {
            if (!(flags & _F_NOEVAL))
            {
                *exp = intNode(en_c_i, 0);
                errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
                errskim(&lex, skim_semi);
            }
        }
    }
    else if (!Optimizer::cparams.prm_cplusplus &&
             ((Optimizer::architecture != ARCHITECTURE_MSIL) || Optimizer::cparams.msilAllowExtensions))
    {
        *exp = intNode(en_c_i, 0);
        errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
        errskim(&lex, skim_semi);
    }
    else
    {
        flags &= ~_F_NOEVAL;
        TYPE* unboxed = nullptr;
        if (isref(*tp))
            *tp = basetype(basetype(*tp)->btp);
        // find structured version of arithmetic types for msil member matching
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && isarithmetic(*tp))
        {
            // auto-boxing for msil
            TYPE* tp1 = find_boxed_type(basetype(*tp));
            if (tp1 && search(basetype(tp1)->syms, overloadNameTab[CI_CONSTRUCTOR]))
            {
                unboxed = *tp;
                *tp = tp1;
            }
        }
        if (isstructured(*tp))
        {
            TYPE* ctype = *tp;
            EXPRESSION* exp1;
            ctype = PerformDeferredInitialization(ctype, funcsp);
            auto bcall = search(basetype(ctype)->syms, overloadNameTab[CI_FUNC]);
            FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
            if (bcall && MATCHKW(lex, openpa))
            {
                lex = getsym();
                if (MATCHKW(lex, closepa))
                {
                    lex = getsym();
                    if (!MATCHKW(lex, openpa))
                    {
                        lex = backupsym();
                        lex = backupsym();
                        bcall = nullptr;
                    }

                }
                else
                {
                    lex = backupsym();
                    bcall = nullptr;
                }
            }
            if (bcall)
            {
                exp1 = anonymousVar(sc_auto, basetype(ctype)->sp->tp);
                *exp = exprNode(en_func, nullptr, nullptr);
                *tp = bcall->tp;
                (*exp)->v.func = funcparams;
                (*exp)->v.func->ascall = true;
                (*exp)->v.func->thisptr = exp1;
                (*exp)->v.func->thistp = MakeType(bt_pointer, basetype(ctype)->sp->tp);
                lex = expression_arguments(lex, funcsp, tp, exp, 0);
            }
            else
            {
                SYMBOL* sym;
                lex = getArgs(lex, funcsp, funcparams, closepa, true, flags);
                EXPRESSION* exp2;
                exp2 = exp1 = *exp = anonymousVar(sc_auto, unboxed ? unboxed : basetype(*tp)->sp->tp);
                sym = exp1->v.sp;
                if (!(flags & _F_SIZEOF))
                {
                    sym->sb->constexpression = true;
                    callConstructor(&ctype, exp, funcparams, false, nullptr, true, true, false, false, false, false, true);
                    if ((*exp)->type == en_thisref && !(*exp)->left->v.func->sp->sb->constexpression)
                        sym->sb->constexpression = false;
                    PromoteConstructorArgs(funcparams->sp, funcparams);
                    callDestructor(basetype(*tp)->sp, nullptr, &exp1, nullptr, true, false, false, true);
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                        *exp = exprNode(en_void, *exp, exp2);
                    else if (!funcparams->arguments ||
                             funcparams->arguments->size() ==
                                 0)  // empty parens means value constructed, e.g. set the thing to zero...
                    {
                        if (basetype(sym->tp)->sp->sb->structuredAliasType)
                        {
                            deref(basetype(sym->tp)->sp->sb->structuredAliasType, &exp2);
                            *exp = exprNode(en_void, exprNode(en_assign, exp2, intNode(en_c_i, 0)), *exp);
                        }
                        else
                        {
                            EXPRESSION* clr = exprNode(en_blockclear, exp2, nullptr);
                            clr->size = sym->tp;
                            *exp = exprNode(en_void, clr, *exp);
                        }
                    }
                    initInsert(&sym->sb->dest, *tp, exp1, 0, true);
                }
                else
                    *exp = exp1;
                if (unboxed)
                    *tp = unboxed;
            }
        }
        else if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            *exp = intNode(en_c_i, 0);
            errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
            errskim(&lex, skim_semi);
        }
        else
        {
            TYPE* throwaway;
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
                funcParams.arguments = nullptr;
                lex = backupsym();
                lex = getArgs(lex, funcsp, &funcParams, closepa, true, flags);
                throwaway = nullptr;
                if (funcParams.arguments && funcParams.arguments->size())
                {
                    throwaway = funcParams.arguments->back()->tp;
                    *exp = funcParams.arguments->back()->exp;
                    if (throwaway && isautotype(*tp))
                        *tp = throwaway;
                    if (isfuncptr(*tp) && (*exp)->type == en_func)
                    {
                        *exp = (*exp)->v.func->fcall;
                        if (!*exp)
                            *exp = intNode(en_c_i, 0);
                    }
                    if (throwaway)
                    {
                        if ((isvoid(throwaway) && !isvoid(*tp)) || ismsil(*tp))
                        {
                            error(ERR_NOT_AN_ALLOWED_TYPE);
                        }
                        else if (isstructured(throwaway))
                        {
                            if (!isvoid(*tp))
                            {
                                if (!Optimizer::cparams.prm_cplusplus || !cppCast(throwaway, tp, exp))
                                {
                                    error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                                }
                            }
                        }
                        else if ((basetype(throwaway)->type == bt_memberptr || basetype(*tp)->type == bt_memberptr) &&
                                 !comparetypes(throwaway, *tp, true))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        else if (!templateNestingCount || (isint(*tp) && isintconst(*exp)))
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
bool doDynamicCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp)
{
    if ((ispointer(oldType) && ispointer(*newType)) || isref(*newType))
    {
        TYPE* tpo = oldType;
        TYPE* tpn = *newType;
        if (isref(*newType))
        {
            tpn = basetype(*newType)->btp;
        }
        else
        {
            tpo = basetype(oldType)->btp;
            tpn = basetype(*newType)->btp;
        }
        if ((!isconst(tpo) || isconst(tpn)) && isstructured(tpo))
        {
            if (isstructured(tpn) || basetype(tpn)->type == bt_void)
            {
                if (basetype(tpn)->type == bt_void || classRefCount(basetype(tpn)->sp, basetype(tpo)->sp) != 1)
                {
                    // if we are going up in the class heirarchy that is the only time we really need to do
                    // a dynamic cast
                    SYMBOL* sym = namespacesearch("__dynamic_cast", globalNameSpace, false, false);
                    if (sym)
                    {
                        EXPRESSION* exp1 = *exp;
                        FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                        INITLIST* arg1 = Allocate<INITLIST>();  // thisptr
                        INITLIST* arg2 = Allocate<INITLIST>();  // excepttab from thisptr
                        INITLIST* arg3 = Allocate<INITLIST>();  // oldxt
                        INITLIST* arg4 = Allocate<INITLIST>();  // newxt
                        SYMBOL* oldrtti = RTTIDumpType(tpo, true);
                        SYMBOL* newrtti = basetype(tpn)->type == bt_void ? nullptr : RTTIDumpType(tpn, true);
                        deref(&stdpointer, &exp1);
                        sym = (SYMBOL*)basetype(sym->tp)->syms->front();
                        arg1->exp = *exp;
                        arg1->tp = &stdpointer;
                        arg2->exp = exprNode(en_cond, *exp, exprNode(en_void, exp1, intNode(en_c_i, 0)));

                        arg2->tp = &stdpointer;
                        arg3->exp = oldrtti ? varNode(en_global, oldrtti) : intNode(en_c_i, 0);
                        arg3->tp = &stdpointer;
                        arg4->exp = newrtti ? varNode(en_global, newrtti) : intNode(en_c_i, 0);
                        arg4->tp = &stdpointer;
                        funcparams->arguments = initListListFactory.CreateList();
                        funcparams->arguments->push_back(arg1);
                        funcparams->arguments->push_back(arg2);
                        funcparams->arguments->push_back(arg3);
                        funcparams->arguments->push_back(arg4);
                        funcparams->sp = sym;
                        funcparams->functp = sym->tp;
                        funcparams->fcall = varNode(en_pc, sym);
                        funcparams->ascall = true;
                        funcparams->rttiType = oldrtti;
                        funcparams->rttiType2 = newrtti;
                        *exp = exprNode(en_lvalue, 0, 0);
                        (*exp)->left = exprNode(en_func, 0, 0);
                        (*exp)->left->v.func = funcparams;
                    }
                    if (isref(*newType))
                        *newType = tpn;
                    if (!basetype(tpo)->sp->sb->hasvtab || !basetype(tpo)->sp->tp->syms->front())
                        errorsym(ERR_NOT_DEFINED_WITH_VIRTUAL_FUNCS, basetype(tpo)->sp);
                    return true;
                }
                return doStaticCast(newType, oldType, exp, funcsp, true);
            }
        }
    }
    return false;
}
bool doStaticCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst)
{
    bool rref = false;
    TYPE* orig = *newType;
    if (isref(*newType))
    {
        if (basetype(*newType)->type == bt_rref)
            rref = true;
        while (isref(*newType))
            *newType = basetype(*newType)->btp;
    }
    // no change or stricter const qualification
    if (comparetypes(*newType, oldType, true))
        return true;
    // conversion from nullptr to pointer
    if (ispointer(*newType) && isint(oldType) && isconstzero(oldType, *exp))
        return true;
    // conversion to or from void pointer
    if (((isvoidptr(*newType) && (ispointer(oldType) || isfunction(oldType))) ||
         (((isvoidptr(oldType) || (*exp)->type == en_nullptr) && ispointer(*newType)) &&
          (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp)))))
        return true;
    // conversion to void (discards type)
    if (isvoid(*newType))
        return true;
    // conversion of one numeric value to another
    if (isarithmetic(*newType) && isarithmetic(oldType))
    {
        cast(*newType, exp);
        return true;
    }
    // floating to enumeration
    if (isfloat(oldType) && basetype(*newType)->type == bt_enum)
    {
        cast(basetype(*newType)->btp, exp);
        return true;
    }
    // int to enum
    if (isint(oldType) && basetype(*newType)->type == bt_enum)
    {
        if (basetype(oldType)->type != basetype(*newType)->btp->type)
            cast(basetype(*newType)->btp, exp);
        return true;
    }
    // enum to int
    if (isint(*newType) && basetype(oldType)->type == bt_enum)
    {
        if (basetype(*newType)->type != basetype(oldType)->btp->type)
            cast(*newType, exp);
        return true;
    }
    // pointer to bool
    if (basetype(*newType)->type == bt_bool && basetype(oldType)->type == bt_pointer)
    {
        cast(basetype(*newType), exp);
        return true;
    }
    // base to derived pointer or derived to base pointer
    if ((ispointer(*newType) && ispointer(oldType)) || isref(orig))
    {
        TYPE* tpo = oldType;
        TYPE* tpn = orig;
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
                EXPRESSION* v = Allocate<EXPRESSION>();
                v->type = en_c_i;
                v = baseClassOffset(basetype(tpn)->sp, basetype(tpo)->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i)  // check for no virtual base
                {
                    if (v->v.i != 0)
                    {
                        EXPRESSION* varsp = anonymousVar(sc_auto, &stdpointer);
                        EXPRESSION* var = exprNode(en_l_p, varsp, nullptr);
                        EXPRESSION* asn = exprNode(en_assign, var, *exp);
                        EXPRESSION* left = exprNode(en_add, var, v);
                        EXPRESSION* right = var;
                        if (v->type == en_l_p)  // check for virtual base
                            v->left = var;
                        v = exprNode(en_cond, var, exprNode(en_void, left, right));
                        v = exprNode(en_void, asn, v);
                        *exp = exprNode(en_lvalue, v, nullptr);
                    }
                    return true;
                }
            }
            else if (!n && classRefCount(basetype(tpo)->sp, basetype(tpn)->sp) == 1)
            {
                // base to derived
                EXPRESSION* v = Allocate<EXPRESSION>();
                v->type = en_c_i;
                v = baseClassOffset(basetype(tpo)->sp, basetype(tpn)->sp, v);
                optimize_for_constants(&v);
                if (v->type == en_c_i)  // check for no virtual base
                {
                    if (v->v.i != 0)
                    {
                        EXPRESSION* varsp = anonymousVar(sc_auto, &stdpointer);
                        EXPRESSION* var = exprNode(en_l_p, varsp, nullptr);
                        EXPRESSION* asn = exprNode(en_assign, var, *exp);
                        EXPRESSION* left = exprNode(en_sub, var, v);
                        EXPRESSION* right = var;
                        if (v->type == en_l_p)  // check for virtual base
                            v->left = var;
                        v = exprNode(en_cond, var, exprNode(en_void, left, right));
                        v = exprNode(en_void, asn, v);
                        *exp = exprNode(en_lvalue, v, nullptr);
                    }
                    return true;
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
                int vbo = basetype(oldType)->sp->sb->vbaseEntries != 0;
                int vbn = basetype(*newType)->sp->sb->vbaseEntries != 0;
                // can't have just new one being virtual...
                if ((vbo && vbn) || !vbn)
                {
                    if (isAccessible(basetype(oldType)->sp, basetype(oldType)->sp, basetype(*newType)->sp, funcsp, ac_public,
                        false))
                        return true;
                }
            }
        }
    }
    // class to anything via conversion func
    if (isstructured(oldType))
    {
        bool rv = cppCast(oldType, newType, exp);
        if (rv)
            return true;
    }
    // class via constructor
    if (isstructured(*newType))
    {
        TYPE* ctype = *newType;
        if (isstructured(oldType)&& isref(orig))
        {
            if (classRefCount(basetype(*newType)->sp, basetype(oldType)->sp) == 1)
            {
                EXPRESSION* exp1 = intNode(en_c_i, 0);
                exp1 = baseClassOffset(basetype(*newType)->sp, basetype(oldType)->sp, exp1);
                if (exp1->type == en_c_i)
                {
                    if (exp1->v.c->i != 0)
                        *exp = exprNode(en_add, *exp, exp1);
                    return true;
                }
            }
        }
        if (callConstructorParam(&ctype, exp, oldType, *exp, true, true, false, false, false))
        {
            EXPRESSION* exp2 = anonymousVar(sc_auto, *newType);
            TYPE* type2 = MakeType(bt_pointer, *newType);
            if (isconst(oldType))
            {
                type2 = MakeType(bt_const, type2);
            }
            if (isvolatile(oldType))
            {
                type2 = MakeType(bt_volatile, type2);
            }
            (*exp)->left->v.func->thistp = type2;
            (*exp)->left->v.func->thisptr = exp2;
            return true;
        }
    }
    *newType = orig;
    return false;
    // e to T:  T t(e) for an invented temp t.
}
bool doConstCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp)
{
    (void)exp;
    (void)funcsp;
    TYPE* orig = *newType;
    bool rref = false;
    if (isref(*newType))
    {
        if (basetype(*newType)->type == bt_rref)
            rref = true;
        while (isref(*newType))
            *newType = basetype(*newType)->btp;
    }
    // as long as the types match except for any changes to const and
    // it is not a function or memberptr we can convert it.
    if (comparetypes(*newType, oldType, 2))
    {
        if (!isfunction(oldType) && basetype(oldType)->type != bt_memberptr)
            return true;
    }
    *newType = orig;
    return false;
}
bool doReinterpretCast(TYPE** newType, TYPE* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst)
{
    (void)funcsp;
    // int to pointer
    if (ispointer(*newType) && isint(oldType))
    {
        cast(*newType, exp);
        return true;
    }
    // enum to pointer
    if (ispointer(*newType) && basetype(oldType)->type == bt_enum)
    {
        cast(*newType, exp);
        return true;
    }
    // pointer to bool
    if (basetype(*newType)->type == bt_bool && basetype(oldType)->type == bt_pointer)
    {
        cast(*newType, exp);
        return true;
    }
    // change type of nullptr
    if ((*exp)->type == en_nullptr)
    {
        return true;
    }
    // pointer to int
    if (ispointer(oldType) && isint(*newType))
    {
        cast(*newType, exp);
        return true;
    }
    // function to int
    if (isfunction(oldType) && isint(*newType))
    {
        cast(*newType, exp);
        return true;
    }
    if (oldType->type == bt_aggregate)
    {
        // function to int
        if (isint(*newType) || ispointer(*newType))
        {
            if ((*exp)->type == en_func)
            {
                LookupSingleAggregate(oldType, exp);
                cast(*newType, exp);
                return true;
            }
        }
    }
    // one function pointer type to another
    if (isfuncptr(oldType) && isfuncptr(*newType))
    {
        if (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp))
        {
            return true;
        }
    }
    // pointer to object to pointer to object (standard layout types, with ok alignment)
    if (ispointer(*newType) && ispointer(oldType))
    {
        TYPE* tpo = basetype(oldType)->btp;
        TYPE* tpn = basetype(*newType)->btp;
        if (!checkconst || isconst(tpn) || !isconst(tpo))
        {
            return true;
        }
    }
    // convert one member pointer to another
    if (basetype(*newType)->type == bt_memberptr)
    {
        if ((*exp)->type == en_nullptr)  // catch nullptr...
        {
            return true;
        }
        else if (basetype(oldType)->type == bt_memberptr)
        {
            // this suffices to check that both are vars or both are functions in this implementation
            if ((*newType)->size == oldType->size)
                if (!checkconst || isconst(basetype(*newType)->btp) || !isconst(basetype(oldType)->btp))
                    return true;
        }
    }
    // conversion to reference
    // the deal is, if T1 is a reference and a pointer to T2 can be cast to a pointer to T1
    // using this methodology, we change the type of the value without any kind of cast operation
    // occurring...
    if (isref(*newType))
    {
        TYPE* nt2 = basetype(*newType)->btp;
        TYPE* tpo = oldType;
        TYPE* tpn = nt2;
        if (!isfuncptr(tpn) && !isfuncptr(tpo))
        {
            if (!checkconst || isconst(tpn) || !isconst(tpo))
            {
                return true;
            }
        }
    }
    return false;
    // fixme nullptr conversion to nullptr
}
LEXLIST* GetCastInfo(LEXLIST* lex, SYMBOL* funcsp, TYPE** newType, TYPE** oldType, EXPRESSION** oldExp, bool packed)
{
    lex = getsym();
    *oldExp = nullptr;
    *oldType = nullptr;
    if (needkw(&lex, lt))
    {
        noTypeNameError++;
        lex = get_type_id(lex, newType, funcsp, sc_cast, false, true, false);
        noTypeNameError--;
        if (!*newType)
        {
            error(ERR_TYPE_NAME_EXPECTED);
            *newType = &stdpointer;
        }
        *newType = ResolveTemplateSelectors(funcsp, *newType);
        if (needkw(&lex, gt))
        {
            if (needkw(&lex, openpa))
            {
                lex = expression(lex, funcsp, nullptr, oldType, oldExp, packed ? _F_PACKABLE : 0);
                ConstExprPatch(oldExp);
                if (!needkw(&lex, closepa))
                {
                    errskim(&lex, skim_closepa);
                }
                if ((*oldType)->type == bt_aggregate)
                {
                    TYPE* tp = *newType;
                    if (isref(tp))
                        tp = basetype(tp)->btp;
                    if (isfuncptr(tp))
                    {
                        tp = basetype(tp)->btp;
                        SYMBOL s = *basetype(tp)->sp;
                        s.tp = tp;
                        auto sym = searchOverloads(&s, (*oldType)->syms);
                        if (sym)
                        {
                            *oldType = MakeType(bt_pointer, sym->tp);
                            UpdateRootTypes(*oldType);
                            (*oldExp)->v.func->sp = sym;
                            (*oldExp)->v.func->functp = sym->tp;
                            (*oldExp)->v.func->fcall = varNode(en_pc, sym);
                        }
                    }
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
LEXLIST* expression_typeid(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    (void)flags;
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        bool byType = false;
        if (startOfType(lex, nullptr, false))
        {
            lex = get_type_id(lex, tp, funcsp, sc_cast, false, true, false);
            byType = true;
        }
        else
        {
            lex = expression_no_check(lex, funcsp, nullptr, tp, exp, 0);
        }
        if (tp)
        {
            SYMBOL* sym = namespacesearch("__GetTypeInfo", globalNameSpace, false, false);
            if (sym)
            {

                FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                INITLIST* arg = Allocate<INITLIST>();
                INITLIST* arg2 = Allocate<INITLIST>();
                SYMBOL* rtti;
                SYMBOL* val;
                TYPE* valtp = MakeType(bt_pointer, &stdpointer);  // space for the virtual pointer and a pointer to the class data
                valtp->array = true;
                valtp->size = 2 * getSize(bt_pointer);
                valtp->esize = intNode(en_c_i, 2);
                val = makeID(sc_auto, valtp, nullptr, AnonymousName());
                val->sb->allocate = true;
                localNameSpace->front()->syms->Add(val);
                sym = (SYMBOL*)basetype(sym->tp)->syms->front();
                funcparams->arguments = initListListFactory.CreateList();
                funcparams->arguments->push_back(arg);
                funcparams->arguments->push_back(arg2);
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = varNode(en_pc, sym);
                funcparams->ascall = true;
                arg->tp = &stdpointer;
                arg->exp = varNode(en_auto, val);
                arg2->tp = &stdpointer;
                if (!byType && isstructured(*tp) && basetype(*tp)->sp->sb->hasvtab)
                {
                    deref(&stdpointer, exp);
                    *exp = exprNode(en_sub, *exp, intNode(en_c_i, VTAB_XT_OFFS));
                    deref(&stdpointer, exp);
                    arg2->exp = *exp;
                }
                else
                {
                    rtti = RTTIDumpType(*tp, true);
                    arg2->exp = rtti ? varNode(en_global, rtti) : intNode(en_c_i, 0);
                    funcparams->rttiType = rtti;
                }
                *exp = exprNode(en_func, 0, 0);
                (*exp)->v.func = funcparams;
                sym = namespacesearch("std", globalNameSpace, false, false);
                if (sym->sb->storage_class == sc_namespace)
                {
                    sym = namespacesearch("type_info", sym->sb->nameSpaceValues, true, false);
                    if (sym)
                    {
                        if (!sym->tp->syms)
                            error(ERR_NEED_TYPEINFO_H);
                        *tp = MakeType(bt_const, sym->tp);
                    }
                }
            }
        }
        needkw(&lex, closepa);
    }
    return lex;
}
bool insertOperatorParams(SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, FUNCTIONCALL* funcparams, int flags)
{
    SYMBOL *s2 = nullptr, *s3;
    SYMLIST **hrd, *hrs;
    const char* name = overloadNameTab[openpa - kw_new + CI_NEW];
    TYPE* tpx;
    if (!isstructured(*tp) && basetype(*tp)->type != bt_enum)
        return false;
    if (isstructured(*tp))
    {
        STRUCTSYM l;
        l.str = (SYMBOL*)basetype(*tp)->sp;
        addStructureDeclaration(&l);
        s2 = classsearch(name, false, true);
        dropStructureDeclaration();
        funcparams->thistp = MakeType(bt_pointer, basetype(*tp));
        funcparams->thisptr = *exp;
    }
    // quit if there are no matches because we will use the default...
    if (!s2)
    {
        funcparams->thistp = nullptr;
        funcparams->thisptr = nullptr;
        return false;
    }
    // finally make a shell to put all this in and add shims for any builtins we want to try
    tpx = MakeType(bt_aggregate);
    s3 = makeID(sc_overloads, tpx, nullptr, name);
    tpx->sp = s3;
    SetLinkerNames(s3, lk_c);
    tpx->syms = symbols.CreateSymbolTable();
    auto itd = tpx->syms->begin();
    if (s2)
    {
        for (auto sp : *s2->tp->syms)
        {
            itd = tpx->syms->insert(itd, sp);
            ++itd;
        }
    }
    funcparams->ascall = true;

    s3 = GetOverloadedFunction(tp, &funcparams->fcall, s3, funcparams, nullptr, F_GOFDELETEDERR, false, flags);
    if (s3)
    {
        if (!isExpressionAccessible(nullptr, s3, funcsp, funcparams->thisptr, false))
            errorsym(ERR_CANNOT_ACCESS, s3);
        s3->sb->throughClass = s3->sb->parentClass != nullptr;
        funcparams->sp = s3;
        funcparams->functp = s3->tp;
        if (funcparams->sp->sb->xcMode != xc_unspecified && funcparams->sp->sb->xcMode != xc_none)
            hasFuncCall = true;
        *exp = intNode(en_func, 0);
        (*exp)->v.func = funcparams;
        *tp = s3->tp;
        CheckCalledException(s3, *exp);
        return true;
    }
    funcparams->thistp = nullptr;
    funcparams->thisptr = nullptr;
    return false;
}
bool insertOperatorFunc(enum ovcl cls, enum e_kw kw, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, TYPE* tp1, EXPRESSION* exp1,
                        std::list<INITLIST*>* args, int flags)
{
    SYMBOL *s1 = nullptr, *s2 = nullptr, *s3, *s4 = nullptr, *s5 = nullptr;
    SYMLIST **hrd, *hrs;
    FUNCTIONCALL* funcparams;
    const char* name = overloadNameTab[kw - kw_new + CI_NEW];
    TYPE* tpin = *tp;
    TYPE* tpx;
    STRUCTSYM l;
    if (!*tp)
        return false;
    TYPE* tpClean = *tp;
    if (isref(tpClean))
        tpClean = basetype(tpClean)->btp;
    TYPE* tp1Clean = tp1;
    if (tp1Clean && isref(tp1Clean))
        tp1Clean = basetype(tp1Clean)->btp;
    if (tpClean && tpClean->scoped && basetype(tpClean)->type != bt_enum)
        tpClean = tpClean->btp;
    if (tp1Clean && tp1Clean->scoped && basetype(tp1Clean)->type != bt_enum)
        tp1Clean = tp1Clean->btp;
    if (!isstructured(tpClean) && basetype(tpClean)->type != bt_enum &&
        ((!tp1Clean && !args) || (tp1Clean && !isstructured(tp1Clean) && basetype(tp1Clean)->type != bt_enum)))
        return false;

    *tp = PerformDeferredInitialization(*tp, funcsp);
    // first find some occurrance either in the locals or in the extant global namespace
    // but only if it is binary or unary...
    switch (cls)
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
            s1 = search(localNameSpace->front()->syms, name);
            if (!s1)
                s1 = namespacesearch(name, localNameSpace, false, false);
            if (!s1 && enumSyms)
                s1 = search(enumSyms->tp->syms, name);
            if (!s1)
                s1 = namespacesearch(name, globalNameSpace, false, false);
            break;
        default:
            break;
    }
    // next find some occurrance in the class or struct
    if (isstructured(*tp))
    {
        int n;
        l.str = (SYMBOL*)basetype(*tp)->sp;
        addStructureDeclaration(&l);
        s2 = classsearch(name, false, true);
        n = PushTemplateNamespace(basetype(*tp)->sp);  // used for more than just templates here
        s4 = namespacesearch(name, globalNameSpace, false, false);
        PopTemplateNamespace(n);
    }
    else
    {
        l.str = nullptr;
        if (basetype(tpClean)->type == bt_enum && tpClean->sp)
        {
            std::list<SYMBOL*> aa{ tpClean->sp->sb->parentNameSpace };
            tpClean->sp->sb->templateNameSpace = tpClean->sp->sb->parentNameSpace ? &aa : nullptr;
            int n = PushTemplateNamespace(basetype(tpClean)->sp);  // used for more than just templates here
            s4 = namespacesearch(name, globalNameSpace, false, false);
            PopTemplateNamespace(n);
            tpClean->sp->sb->templateNameSpace = nullptr;
        }
    }
    if (tp1)
    {
        if (isstructured(tp1))
        {
            int n = PushTemplateNamespace(basetype(tp1)->sp);  // used for more than just templates here
            s5 = namespacesearch(name, globalNameSpace, false, false);
            PopTemplateNamespace(n);
            auto exp3 = exp1;
            while (exp3->type == en_void)
                exp3 = exp3->right;
            if (exp3->type == en_thisref)
            {
                tp1 = CopyType(tp1);
                tp1->lref = false;
                tp1->rref = true;
            }
        }
        else if (basetype(tp1)->type == bt_enum)  // enum
        {
            std::list<SYMBOL*> aa{ basetype(tp1)->sp->sb->parentNameSpace };
            basetype(tp1)->sp->sb->templateNameSpace = basetype(tp1)->sp->sb->parentNameSpace ? &aa : nullptr;
            int n = PushTemplateNamespace(basetype(tp1)->sp);  // used for more than just templates here
            s5 = namespacesearch(name, globalNameSpace, false, false);
            PopTemplateNamespace(n);
            basetype(tp1)->sp->sb->templateNameSpace = nullptr;
        }
    }
    // quit if there are no matches because we will use the default...
    if (!s1 && !s2 && !s4 && !s5)
    {
        if (l.str)
            dropStructureDeclaration();
        return false;
    }
    // finally make a shell to put all this in and add shims for any builtins we want to try
    tpx = MakeType(bt_aggregate);
    s3 = makeID(sc_overloads, tpx, nullptr, name);
    tpx->sp = s3;
    SetLinkerNames(s3, lk_c);
    tpx->syms = symbols.CreateSymbolTable();
    auto itd = tpx->syms->begin();
    if (s1)
    {
        for (auto sp : *s1->tp->syms)
        {
            itd = tpx->syms->insert(itd, sp);
            ++itd;
        }
    }
    if (s2)
    {
        for (auto sp : *s2->tp->syms)
        {
            itd = tpx->syms->insert(itd, sp);
            ++itd;
        }
    }
    if (s4)
    {
        for (auto sp : *s4->tp->syms)
        {
            itd = tpx->syms->insert(itd, sp);
            ++itd;
        }
    }
    if (s5)
    {
        for (auto sp : *s5->tp->syms)
        {
            itd = tpx->syms->insert(itd, sp);
            ++itd;
        }
    }
    funcparams = Allocate<FUNCTIONCALL>();
    {
        INITLIST *one = nullptr, *two = nullptr;
        if (*tp)
        {
            one = Allocate<INITLIST>();
            one->exp = *exp;
            one->tp = *tp;
        }
        if (args)
        {
            INITLIST* one = Allocate<INITLIST>();
            one->nested = args;
        }
        else if (tp1)
        {
            two = Allocate<INITLIST>();
            two->exp = exp1;
            two->tp = tp1;
        }
        else if (cls == ovcl_unary_postfix)
        {
            two = Allocate<INITLIST>();
            two->exp = intNode(en_c_i, 0);
            two->tp = &stdint;
        }
        if (one || two)
        {
            funcparams->arguments = initListListFactory.CreateList();
            if (one)
                funcparams->arguments->push_back(one);
            if (two)
                funcparams->arguments->push_back(two);
        }
    }
    switch (cls)
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
    funcparams->ascall = true;
    TYPE* ctype = *tp;
    s3 = GetOverloadedFunction(&ctype, &funcparams->fcall, s3, funcparams, nullptr, F_GOFDELETEDERR, false, flags);
    if (s3)
    {
        if (!isExpressionAccessible(nullptr, s3, funcsp, funcparams->thisptr, false))
            errorsym(ERR_CANNOT_ACCESS, s3);
        // if both parameters are non-struct, we do a check that enums map to enums
        // if they don't we want to select the builtin function
        // which we do by returning false...
        switch (cls)
        {
            case ovcl_unary_any:
            case ovcl_unary_prefix:
            case ovcl_unary_numeric:
            case ovcl_unary_int:
            case ovcl_unary_numericptr:
            case ovcl_unary_postfix:
            case ovcl_binary_any:
            case ovcl_binary_numeric:
            case ovcl_binary_int:
            case ovcl_binary_numericptr:
                if (!isstructured(tpClean) && (!tp1Clean || !isstructured(tp1Clean)))
                {
                    auto it = basetype(s3->tp)->syms->begin();
                    if ((*it)->sb->thisPtr)
                        ++it;
                    TYPE* arg1 = (*it)->tp;
                    ++it;
                    TYPE* arg2 = tp1 && it != basetype(s3->tp)->syms->end() ? (*it)->tp : nullptr;
                    if (arg1 && isref(arg1))
                        arg1 = basetype(arg1)->btp;
                    if (arg2 && isref(arg2))
                        arg2 = basetype(arg2)->btp;
                    if (((basetype(tpClean)->type == bt_enum) != (basetype(arg1)->type == bt_enum)) ||
                        (tp1Clean && ((basetype(tp1Clean)->type == bt_enum) != (basetype(arg2)->type == bt_enum))))
                        return false;
                    break;
                }
        }
        *tp = ctype;
        if (ismember(s3))
        {
            funcparams->arguments->pop_front();
            funcparams->thistp = MakeType(bt_pointer, tpin);
            funcparams->thisptr = *exp;
        }
        s3->sb->throughClass = s3->sb->parentClass != nullptr;
        funcparams->sp = s3;
        funcparams->functp = s3->tp;
        *exp = intNode(en_func, 0);
        (*exp)->v.func = funcparams;
        *tp = s3->tp;
        expression_arguments(nullptr, funcsp, tp, exp, 0);
        if (s3->sb->defaulted && kw == assign)
            createAssignment(s3->sb->parentClass, s3);
        if (l.str)
            dropStructureDeclaration();
        CheckCalledException(s3, funcparams->thisptr);
        DestructParams(funcparams->arguments);
        return true;
    }
    if (l.str)
        dropStructureDeclaration();
    return false;
}
LEXLIST* expression_new(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, bool global, int flags)
{
    FUNCTIONCALL* placement = Allocate<FUNCTIONCALL>();
    FUNCTIONCALL* initializers = nullptr;
    const char* name = overloadNameTab[CI_NEW];
    TYPE* tpf = nullptr;
    EXPRESSION *arrSize = nullptr, *exp1;
    SYMBOL* s1 = nullptr;
    EXPRESSION *val = nullptr, *newfunc = nullptr;
    *exp = nullptr;
    *tp = nullptr;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (startOfType(lex, nullptr, false))
        {
            // type in parenthesis
            lex = get_type_id(lex, tp, funcsp, sc_cast, false, true, false);
            needkw(&lex, closepa);
        }
        else
        {
            // placement new
            lex = backupsym();
            lex = getArgs(lex, funcsp, placement, closepa, true, 0);
        }
    }
    if (!*tp)
    {
        if (MATCHKW(lex, openpa))
        {
            // type in parenthesis
            lex = getsym();
            lex = get_type_id(lex, tp, funcsp, sc_cast, false, true, false);
            needkw(&lex, closepa);
        }
        else
        {
            // new type id
            lex = get_type_id(lex, tp, funcsp, sc_auto, true, true, false);
            if (MATCHKW(lex, openbr))
            {
                TYPE* tp1 = nullptr;
                EXPRESSION* exp = nullptr;
                name = overloadNameTab[CI_NEWA];
                lex = getsym();
                lex = expression(lex, funcsp, nullptr, &tp1, &exp, flags);
                ConstExprPatch(&exp);
                if (!isint(tp1))
                {
                    error(ERR_NEED_INTEGER_TYPE);
                }
                if (tp1->type != bt_unsigned)
                {
                    tp1 = &stdunsigned;
                    cast(tp1, &exp);
                }
                needkw(&lex, closebr);
                ParseAttributeSpecifiers(&lex, funcsp, true);
                arrSize = exp;
                while (MATCHKW(lex, openbr))
                {
                    lex = getsym();
                    lex = expression(lex, funcsp, nullptr, &tp1, &exp, flags);
                    optimize_for_constants(&exp);
                    ConstExprPatch(&exp);
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
                    ParseAttributeSpecifiers(&lex, funcsp, true);
                }
                optimize_for_constants(&arrSize);
            }
        }
    }
    if (*tp)
    {
        EXPRESSION* sz;
        INITLIST* sza;
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
            int al = n % basetype(*tp)->sp->sb->attribs.inheritable.structAlign;
            if (al != 0)
                n += basetype(*tp)->sp->sb->attribs.inheritable.structAlign - al;
        }
        sz = intNode(en_c_i, n);
        if (arrSize)
        {
            sz = exprNode(en_mul, sz, arrSize);
        }
        optimize_for_constants(&sz);
        sza = Allocate<INITLIST>();
        sza->exp = sz;
        sza->tp = &stdint;
        if (!placement->arguments)
            placement->arguments = initListListFactory.CreateList();
        placement->arguments->push_front(sza);
    }
    else
    {
        error(ERR_TYPE_NAME_EXPECTED);
        *tp = &stdint;  // error handling
    }
    if (!global)
    {
        if (isstructured(*tp))
        {
            STRUCTSYM l;

            l.str = (SYMBOL*)basetype(*tp)->sp;
            addStructureDeclaration(&l);
            s1 = classsearch(name, false, true);
            dropStructureDeclaration();
        }
    }
    if (!s1)
    {
        s1 = namespacesearch(name, globalNameSpace, false, false);
    }
    // should have always found something
    placement->ascall = true;
    s1 = GetOverloadedFunction(&tpf, &placement->fcall, s1, placement, nullptr, true, false, flags);
    if (s1)
    {
        SYMBOL* sym;
        val = anonymousVar(sc_auto, &stdpointer);
        sym = val->v.sp;
        sym->sb->decoratedName = sym->name;
        //        if (localNameSpace->front()->syms)
        //            insert(sym, localNameSpace->front()->syms);
        deref(&stdpointer, &val);
        s1->sb->throughClass = s1->sb->parentClass != nullptr;
        if (s1->sb->throughClass && !isAccessible(s1->sb->parentClass, s1->sb->parentClass, s1, funcsp,
                                                  placement->thisptr ? ac_protected : ac_public, false))
        {
            errorsym(ERR_CANNOT_ACCESS, s1);
        }
        placement->sp = s1;
        placement->functp = s1->tp;
        placement->ascall = true;
        placement->fcall = varNode(en_pc, s1);
        //        placement->sb->noinline = (flags & _F_NOINLINE) | s1->sb->noinline;
        exp1 = intNode(en_func, 0);
        exp1->v.func = placement;
        newfunc = exp1;
    }
    if (KW(lex) == openpa)
    {
        // initializers
        if (isstructured(*tp))
        {
            initializers = Allocate<FUNCTIONCALL>();
            lex = getArgs(lex, funcsp, initializers, closepa, true, 0);
            if (s1)
            {
                *exp = val;
                tpf = *tp;
                callConstructor(&tpf, exp, initializers, false, arrSize, true, false, false, true, false, false, true);
                PromoteConstructorArgs(initializers->sp, initializers);
            }
            if (!initializers->arguments || !initializers->arguments->size())
            {
                // empty arguments means value construct...
                EXPRESSION* clr = exprNode(en_blockclear, val, nullptr);
                clr->size = *tp;
                *exp = exprNode(en_void, clr, *exp);
            }
        }
        else
        {
            exp1 = nullptr;
            initializers = Allocate<FUNCTIONCALL>();
            lex = getArgs(lex, funcsp, initializers, closepa, true, 0);
            if (initializers->arguments && initializers->arguments->size())
            {
                if (!comparetypes(initializers->arguments->front()->tp, *tp, false) || initializers->arguments->size() > 1)
                {
                    if (!templateNestingCount)
                        error(ERR_NEED_NUMERIC_EXPRESSION);
                }
                else
                {
                    exp1 = initializers->arguments->front()->exp;
                    DeduceAuto(tp, initializers->arguments->front()->tp, exp1);
                    UpdateRootTypes(*tp);
                    if (exp1 && val)
                    {
                        EXPRESSION* pval = val;
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
                    EXPRESSION* pval = val;
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
            std::list<INITIALIZER*>* init = nullptr;
            EXPRESSION* base = val;
            TYPE* tp1 = *tp;
            SYMBOL* sym = nullptr;
            if (isstructured(*tp))
            {
                sym = anonymousVar(sc_localstatic, *tp)->v.sp;
            }
            if (arrSize)
            {
                tp1 = MakeType(bt_pointer, *tp);
                tp1->size = 0;
                tp1->array = 1;
                tp1->esize = arrSize;
            }
            lex = initType(lex, funcsp, 0, sc_auto, &init, nullptr, tp1, sym, false, 0);
            if (!isstructured(*tp) && !arrSize)
            {
                if (init->size() != 1 || (init->front()->basetp && isstructured(init->front()->basetp)))
                    error(ERR_NONSTRUCTURED_INIT_LIST);
            }
            // dest is lost for these purposes
            if (theCurrentFunc)
            {
                if (init->front()->exp)
                {
                    *exp = convertInitToExpression(tp1, nullptr, nullptr, funcsp, init, base, false);
                }
                if (arrSize && !isstructured(*tp))
                {
                    exp1 = exprNode(en_blockclear, base, exprNode(en_mul, arrSize, intNode(en_c_i, (*tp)->size)));
                    exp1->size = *tp;
                    *exp = *exp ? exprNode(en_void, exp1, *exp) : exp1;
                }
                else if (isstructured(*tp) && (arrSize || !init->front()->exp))
                {
                    EXPRESSION* exp1;
                    exp1 = val;
                    if (arrSize)
                    {
                        auto back = init->back();
                        arrSize = exprNode(en_sub, arrSize, intNode(en_c_i, back->offset / (basetype(*tp)->size)));
                        exp1 = exprNode(en_add, exp1, intNode(en_c_i, back->offset));
                    }
                    tpf = *tp;
                    callConstructor(&tpf, &exp1, nullptr, false, arrSize, true, false, false, true, false, false, true);
                    *exp = *exp ? exprNode(en_void, *exp, exp1) : exp1;
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
            callConstructor(&tpf, exp, nullptr, false, arrSize, true, false, false, true, false, false, true);
        }
    }
    *tp = MakeType(bt_pointer, *tp);

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
LEXLIST* expression_delete(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, bool global, int flags)
{
    bool byArray = false;
    SYMBOL* s1 = nullptr;
    FUNCTIONCALL* funcparams;
    INITLIST* one = nullptr;
    EXPRESSION *exp1 = nullptr, *exp2;
    TYPE* tpf;
    const char* name = overloadNameTab[CI_DELETE];
    EXPRESSION* in;
    EXPRESSION *var, *asn;
    lex = getsym();
    if (MATCHKW(lex, openbr))
    {
        lex = getsym();
        needkw(&lex, closebr);
        byArray = true;
        name = overloadNameTab[CI_DELETEA];
        exp1 = intNode(en_c_i, 0);  // signal to the runtime to load the number of elems dynamically
    }
    lex = expression_cast(lex, funcsp, nullptr, tp, exp, nullptr, flags);
    if (!*tp)
    {
        error(ERR_IDENTIFIER_EXPECTED);
        *tp = &stdint;  // error handling
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
        callDestructor(basetype(basetype(*tp)->btp)->sp, nullptr, exp, exp1, true, true, false, false);
    }
    exp1 = exp2;
    if (!global)
    {
        if (isstructured(*tp))
        {
            STRUCTSYM l;
            l.str = (SYMBOL*)basetype(*tp)->sp;
            addStructureDeclaration(&l);
            s1 = classsearch(name, false, true);
            dropStructureDeclaration();
        }
    }
    if (!s1)
    {
        s1 = namespacesearch(name, globalNameSpace, false, false);
    }
    // should always find something
    funcparams = Allocate<FUNCTIONCALL>();
    one = Allocate<INITLIST>();
    one->exp = exp1;
    one->tp = &stdpointer;
    funcparams->arguments = initListListFactory.CreateList();
    funcparams->arguments->push_back(one);
    funcparams->ascall = true;
    s1 = GetOverloadedFunction(&tpf, &funcparams->fcall, s1, funcparams, nullptr, true, false, flags);
    if (s1)
    {
        s1->sb->throughClass = s1->sb->parentClass != nullptr;
        if (s1->sb->throughClass && !isAccessible(s1->sb->parentClass, s1->sb->parentClass, s1, funcsp,
                                                  funcparams->thisptr ? ac_protected : ac_public, false))
        {
            errorsym(ERR_CANNOT_ACCESS, s1);
        }
        funcparams->sp = s1;
        funcparams->functp = s1->tp;
        funcparams->ascall = true;
        funcparams->fcall = varNode(en_pc, s1);
        // funcparams->sb->noinline =  (flags & _F_NOINLINE) | s1->sb->noinline;
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
bool isNoexcept(EXPRESSION* exp)
{
    (void)exp;
    return false;
}
static bool noexceptStmt(std::list<STATEMENT*>* block);
static bool noexceptExpression(EXPRESSION* node)
{
    FUNCTIONCALL* fp;
    bool rv = true;
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
        case en_c_string:
        case en_nullptr:
        case en_memberptr:
        case en_structelem:
            break;
        case en_global:
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
        case en_l_inative:
        case en_l_unative:
        case en_l_uc:
        case en_l_us:
        case en_l_bool:
        case en_l_bit:
        case en_l_ll:
        case en_l_ull:
        case en_l_string:
        case en_l_object:
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
        case en_x_inative:
        case en_x_unative:
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
        case en_x_string:
        case en_x_object:
        case en_trapcall:
        case en_shiftby:
            /*        case en_movebyref: */
        case en_substack:
        case en_alloca:
        case en_loadstack:
        case en_savestack:
        case en__initobj:
        case en__sizeof:
            rv = noexceptExpression(node->left);
            break;
        case en_assign:
        case en__initblk:
        case en__cpblk:
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
        case en_dot:
        case en_pointsto:
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
        case en_select:
            rv = noexceptExpression(node->left);
            break;
        case en_atomic:
        case en_construct:
            break;
        case en_func:
            fp = node->v.func;
            {
                SYMBOL* sym = fp->sp;
                if (sym->tp->type == bt_aggregate)
                {
                    if (sym->tp->syms->size() == 1)
                    {
                        sym = sym->tp->syms->front();
                    }
                }
                rv = sym->sb->noExcept;
            }
            break;
        case en_stmt:
            rv = noexceptStmt(node->v.stmt);
            break;
        case en_templateparam:
            break;
        default:
            diag("noexceptExpression");
            break;
    }
    return rv;
}
static bool noexceptStmt(std::list<STATEMENT*>* blocks)
{
    bool rv = true;
    for (auto block : *blocks)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_catch:
            case st_try:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
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
    }
    return rv;
}
LEXLIST* expression_noexcept(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        lex = expression_no_check(lex, funcsp, nullptr, tp, exp, _F_SIZEOF | _F_IS_NOTHROW);
        *exp = intNode(en_c_i, noexceptExpression(*exp));
        *tp = &stdbool;

        needkw(&lex, closepa);
    }
    return lex;
}
void ResolveTemplateVariable(TYPE** ttype, EXPRESSION** texpr, TYPE* rtype, TYPE* atype)
{
    EXPRESSION* exp = *texpr;
    bool lval = false;
    if (lvalue(exp))
    {
        exp = exp->left;
        lval = true;
    }
    if (exp->type == en_templateparam)
    {
        if (exp->v.sp->sb && exp->v.sp->sb->templateLevel && !exp->v.sp->sb->instantiated)
        {
            SYMBOL* sym;
            TYPE* type;
            std::list<TEMPLATEPARAMPAIR>* params = templateParamPairListFactory.CreateList();
            if (atype)
                if (rtype)
                    if (atype->type > rtype->type)
                        type = atype;
                    else
                        type = rtype;
                else
                    type = atype;
            else
                type = rtype;
            auto  second = Allocate<TEMPLATEPARAM>();

            second->type = kw_typename;
            second->byClass.dflt = type;
            params->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
            sym = GetVariableTemplate(exp->v.sp, params);
            if (sym)
            {
                *texpr = varNode(en_global, sym);
                *ttype = type;
                if (lval)
                    deref(type, texpr);
            }
            else
            {
                diag("ResolveTemplateVariables no var");
            }
        }
    }
}
}  // namespace Parser