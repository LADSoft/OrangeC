/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "lambda.h"
#include "declare.h"
#include "expr.h"
#include "lex.h"
#include "help.h"
#include "types.h"
#include "declcons.h"
#include "declcpp.h"
#include "constopt.h"
#include "init.h"
#include "OptUtils.h"
#include "beinterf.h"
#include "Property.h"
#include "memory.h"
#include "exprcpp.h"
#include "constexpr.h"
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "overload.h"
#include "class.h"
#include "Utils.h"
#include "casts.h"
#include "attribs.h"

namespace Parser
{

/* lvaule */
/* handling of const int */
/*--------------------------------------------------------------------------------------------------------------------------------
 */
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
                    EXPRESSION* ec = MakeIntExpression(ExpressionNode::c_i_, vbase->pointerOffset);
                    rv = MakeExpression(ExpressionNode::add_, en, ec);
                    Dereference(&stdpointer, &rv);
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
                for (auto it = stk.begin(); it != stk.end(); ++it)
                {
                    auto entry = *(*it).first;
                    if (entry->isvirtual)
                    {
                        int offset;
                        std::list<VBASEENTRY*>::iterator itl, itle = itl, itx;
                        itl = derived->sb->vbaseEntries->begin();
                        itle = derived->sb->vbaseEntries->end();
                        for (itx = itl; itx != itl && (*itx)->cls != entry->cls; ++itx)
                            ;
                        offset = (*itx)->pointerOffset;
                        rv = MakeExpression(ExpressionNode::add_, rv, MakeIntExpression(ExpressionNode::c_i_, offset));
                        if (entry->isvirtual)
                            Dereference(&stdpointer, &rv);
                    }
                    else
                    {
                        int offset = entry->offset;
                        rv = MakeExpression(ExpressionNode::add_, rv, MakeIntExpression(ExpressionNode::c_i_, offset));
                    }
                }
            }
        }
    }
    return rv;
}
void qualifyForFunc(SYMBOL* sym, Type** tp, bool isMutable)
{
    if (sym)
    {
        if (sym->tp->IsConst() && !isMutable)
        {
            *tp = Type::MakeType(BasicType::const_, *tp);
        }
        if (sym->tp->IsVolatile())
        {
            *tp = Type::MakeType(BasicType::volatile_, *tp);
        }
    }
}
void getThisType(SYMBOL* sym, Type** tp)
{
    *tp = Type::MakeType(BasicType::pointer_, sym->sb->parentClass->tp);
    qualifyForFunc(sym, tp, false);
}
EXPRESSION* getMemberBase(SYMBOL* memberSym, SYMBOL* strSym, SYMBOL* funcsp, bool toError)
{
    EXPRESSION* en;
    SYMBOL* enclosing = enclosingDeclarations.GetFirst();

    if (enclosing && (!funcsp || (funcsp->sb->storage_class != StorageClass::global_ &&
                                  funcsp->sb->storage_class != StorageClass::static_)))  // lambdas will be caught by this too
    {
        if (strSym)
        {
            if (!strSym->tp->CompatibleType(enclosing->tp))
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
            en = MakeExpression(ExpressionNode::auto_, (SYMBOL*)funcsp->tp->BaseType()->syms->front());  // this ptr
        else
            en = MakeIntExpression(ExpressionNode::thisshim_, 0);
        if (lambdas.size() && !memberSym->sb->parentClass->sb->islambda)
        {
            if (!lambdas.front()->lthis || !lambdas.front()->captured)
            {
                en = MakeIntExpression(ExpressionNode::c_i_, 0);
                if (toError)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
            }
            else
            {
                SYMBOL* sym = search(lambdas.front()->cls->tp->syms, lambdas.front()->thisByVal ? "*this" : "$this");
                enclosing = lambdas.front()->lthis;
                if (sym)
                {
                    Dereference(&stdpointer, &en);
                    en = MakeExpression(ExpressionNode::add_, en, MakeIntExpression(ExpressionNode::c_i_, sym->sb->offset));
                    if (!lambdas.front()->thisByVal)
                        Dereference(&stdpointer, &en);
                }
                else
                {
                    diag("getMemberBase: cannot find lambda this");
                }
            }
        }
        else
        {
            Dereference(&stdpointer, &en);
        }
        if (enclosing != memberSym->sb->parentClass && enclosing->sb->mainsym != memberSym->sb->parentClass &&
            !SameTemplate(enclosing->tp, memberSym->sb->parentClass->tp))
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
        en = MakeIntExpression(ExpressionNode::c_i_, 0);
        if (toError)
            errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, memberSym);
    }
    return en;
}
EXPRESSION* getMemberNode(SYMBOL* memberSym, SYMBOL* strSym, Type** tp, SYMBOL* funcsp)
{
    EXPRESSION* en = getMemberBase(memberSym, strSym, funcsp, true);
    en = MakeExpression(ExpressionNode::structadd_, en, MakeIntExpression(ExpressionNode::c_i_, memberSym->sb->offset));
    *tp = memberSym->tp;
    return en;
}
EXPRESSION* getMemberPtr(SYMBOL* memberSym, SYMBOL* strSym, Type** tp, SYMBOL* funcsp)
{
    (void)strSym;
    EXPRESSION* rv;
    *tp = Type::MakeType(BasicType::memberptr_, memberSym->tp);
    (*tp)->sp = memberSym->sb->parentClass;
    rv = MakeExpression(ExpressionNode::memberptr_, memberSym);
    rv->isfunc = true;
    if (!isExpressionAccessible(nullptr, memberSym, funcsp, nullptr, true))
    {
        errorsym(ERR_CANNOT_ACCESS, memberSym);
    }
    return rv;
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
    if (IsLValue(exp))
    {
        if (exp->left->type == ExpressionNode::auto_)
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
EXPRESSION* substitute_params_for_constexpr(EXPRESSION* exp, CallSite* funcparams, SymbolTable<SYMBOL>* syms)
{
    auto it = funcparams->sp->tp->BaseType()->syms->begin();
    auto itend = funcparams->sp->tp->BaseType()->syms->end();
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
std::list<Statement*>* do_substitute_for_function(std::list<Statement*>* blocks, CallSite* funcparams, SymbolTable<SYMBOL>* syms)
{
    if (blocks)
    {
        std::list<Statement*>* rv = stmtListFactory.CreateList();
        for (auto block : *blocks)
        {
            auto stmt = Allocate<Statement>();
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
EXPRESSION* substitute_params_for_function(CallSite* funcparams, SymbolTable<SYMBOL>* syms)
{
    auto st = do_substitute_for_function(funcparams->sp->sb->inlineFunc.stmt, funcparams, syms);
    EXPRESSION* exp = MakeExpression(ExpressionNode::stmt_);
    exp->v.stmt = st;
    return exp;
}
void expression_func_type_cast( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    bool defd = false;
    int consdest = false;
    bool notype = false;
    bool deduceTemplate = false;
    if (!(flags & _F_NOEVAL))
    {
        *tp = nullptr;
        *tp = TypeGenerator::UnadornedType(funcsp, *tp, nullptr, false, StorageClass::auto_, &linkage, &linkage2, &linkage3,
                                           AccessLevel::public_, &notype, &defd, &consdest, nullptr,
                                           Optimizer::cparams.cpp_dialect >= Dialect::cpp17 ? &deduceTemplate : nullptr, false,
                                           true, false, nullptr, false);
        (*tp)->InstantiateDeferred();
        (*tp)->InitializeDeferred();
        if ((*tp)->IsStructured() && !(*tp)->size && (!templateDefinitionLevel || !(*tp)->BaseType()->sp->sb->templateLevel))
        {
            (*tp) = (*tp)->BaseType()->sp->tp;
            if (!(*tp)->size && !deduceTemplate)
                errorsym(ERR_STRUCT_NOT_DEFINED, (*tp)->BaseType()->sp);
        }
        if ((*tp)->IsStructured() && deduceTemplate && !MATCHKW(Keyword::openpa_))
        {
            SpecializationError((*tp)->BaseType()->sp);
        }
    }
    if (!MATCHKW(Keyword::openpa_))
    {
        if (MATCHKW(Keyword::begin_))
        {
            flags &= ~_F_NOEVAL;
            std::list<Initializer*>*init = nullptr, *dest = nullptr;
            SYMBOL* sym = nullptr;
            sym = AnonymousVar(StorageClass::auto_, *tp)->v.sp;

            initType(funcsp, 0, StorageClass::auto_, &init, &dest, *tp, sym, false, false, flags | _F_EXPLICIT);
            (*tp)->InstantiateDeferred();
            if (init && init->size() == 1 && init->front()->exp->type == ExpressionNode::thisref_)
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
                *exp = ConverInitializersToExpression(*tp, sym, nullptr, funcsp, init, nullptr, false);
                if (sym && (*tp)->IsStructured())
                {
                    EXPRESSION** e1 = exp;
                    if (*e1)
                    {
                        while ((*e1)->type == ExpressionNode::comma_ && (*e1)->right)
                            e1 = &(*e1)->right;
                        if ((*e1)->type == ExpressionNode::comma_)
                            e1 = &(*e1)->left;
                        *e1 = MakeExpression(ExpressionNode::comma_, *e1, MakeExpression(ExpressionNode::auto_, sym));
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
                *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
                errskim(skim_semi);
            }
            else if (ISID())
            {
                getsym();
            }
        }
    }
    else if (!Optimizer::cparams.prm_cplusplus &&
             ((Optimizer::architecture != ARCHITECTURE_MSIL) || Optimizer::cparams.msilAllowExtensions))
    {
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
        errskim(skim_semi);
    }
    else
    {
        flags &= ~_F_NOEVAL;
        Type* unboxed = nullptr;
        if ((*tp)->IsRef())
            *tp = (*tp)->BaseType()->btp->BaseType();
        // find structured version of arithmetic types for msil member matching
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && (*tp)->IsArithmetic())
        {
            // auto-boxing for msil
            Type* tp1 = find_boxed_type((*tp)->BaseType());
            if (tp1 && search(tp1->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]))
            {
                unboxed = *tp;
                *tp = tp1;
            }
        }
        if ((*tp)->IsStructured())
        {
            Type* ctype = *tp;
            EXPRESSION* exp1;
            auto bcall = search(ctype->BaseType()->syms, overloadNameTab[CI_FUNC]);
            CallSite* funcparams = Allocate<CallSite>();
            if (bcall && !deduceTemplate && MATCHKW(Keyword::openpa_))
            {
                getsym();
                if (MATCHKW(Keyword::closepa_))
                {
                    getsym();
                    if (!MATCHKW(Keyword::openpa_))
                    {
                        bcall = nullptr;
                    }
                    --*currentStream;
                    --*currentStream;
                }
                else
                {
                    --*currentStream;
                    bcall = nullptr;
                }
            }
            if (!bcall && deduceTemplate && (Optimizer::architecture != ARCHITECTURE_MSIL))
            {
                *exp = nullptr;
                RequiresDialect::Feature(Dialect::cpp17, "Class template argument deduction");
                getArgs(funcsp, funcparams, Keyword::closepa_, true, flags);
                CTADLookup(funcsp, exp, tp, funcparams, flags);
                int offset = 0;
                auto exp2 = relptr((*exp)->left->v.func->thisptr, offset);
                if (!(flags & _F_SIZEOF) && exp2 && exp2->type != ExpressionNode::thisref_)
                {
                    auto exp3 = exp2;
                    CallDestructor((*tp)->BaseType()->sp, nullptr, &exp2, nullptr, true, false, false, true);
                    InsertInitializer(&exp3->v.sp->sb->dest, (*tp), exp2, 0, true);
                }
            }
            else
            {
                SYMBOL* sym;
                getArgs(funcsp, funcparams, Keyword::closepa_, true, flags);
                EXPRESSION* exp2;
                exp2 = exp1 = *exp = AnonymousVar(StorageClass::auto_, unboxed ? unboxed : (*tp)->BaseType()->sp->tp);
                sym = exp1->v.sp;
                if (!(flags & _F_SIZEOF))
                {
                    sym->sb->constexpression = true;
                    CallConstructor(&ctype, exp, funcparams, false, nullptr, true, true, false, false, false, false, true);
                    if ((*exp)->type == ExpressionNode::thisref_ && !(*exp)->left->v.func->sp->sb->constexpression)
                        sym->sb->constexpression = false;
                    PromoteConstructorArgs(funcparams->sp, funcparams);
                    CallDestructor((*tp)->BaseType()->sp, nullptr, &exp1, nullptr, true, false, false, true);
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                        *exp = MakeExpression(ExpressionNode::comma_, *exp, exp2);
                    else if (!funcparams->arguments ||
                             funcparams->arguments->size() ==
                                 0)  // empty parens means value constructed, e.g. set the thing to zero...
                    {
                        if (sym->tp->BaseType()->sp->sb->structuredAliasType)
                        {
                            Dereference(sym->tp->BaseType()->sp->sb->structuredAliasType, &exp2);
                            *exp = MakeExpression(
                                ExpressionNode::comma_,
                                MakeExpression(ExpressionNode::assign_, exp2, MakeIntExpression(ExpressionNode::c_i_, 0)), *exp);
                        }
                        else
                        {
                            EXPRESSION* clr = MakeExpression(ExpressionNode::blockclear_, exp2);
                            clr->size = sym->tp;
                            *exp = MakeExpression(ExpressionNode::comma_, clr, *exp);
                        }
                    }
                    InsertInitializer(&sym->sb->dest, *tp, exp1, 0, true);
                }
                else
                    *exp = exp1;
                if (unboxed)
                    *tp = unboxed;
                if (bcall)
                {
                    auto tp1 = *tp;
                    while (tp1->type == BasicType::typedef_)
                        tp1 = tp1->btp;
                    tp1 = tp1->MakeType(BasicType::pointer_, tp1);
                    funcparams = Allocate<CallSite>();
                    funcparams->ascall = true;
                    funcparams->thisptr = *exp;
                    funcparams->thistp = tp1;
                    *exp = MakeExpression(funcparams);
                    *tp = bcall->tp;
                    expression_arguments(funcsp, tp, exp, 0);
                }
            }
        }
        else if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
            errskim(skim_semi);
        }
        else
        {
            Type* throwaway;
            needkw(Keyword::openpa_);
            if (MATCHKW(Keyword::closepa_))
            {
                // constructor with no args gets a value of zero...
                *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                needkw(Keyword::closepa_);
            }
            else
            {
                CallSite funcParams;
                funcParams.arguments = nullptr;
                --*currentStream;
                getArgs(funcsp, &funcParams, Keyword::closepa_, true, flags);
                throwaway = nullptr;
                if (funcParams.arguments && funcParams.arguments->size())
                {
                    throwaway = funcParams.arguments->back()->tp;
                    *exp = funcParams.arguments->back()->exp;
                    if (throwaway && (*tp)->IsAutoType())
                        *tp = throwaway;
                    if ((*tp)->IsFunctionPtr() && (*exp)->type == ExpressionNode::callsite_)
                    {
                        *exp = (*exp)->v.func->fcall;
                        if (!*exp)
                            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    if (throwaway)
                    {
                        if ((throwaway->IsVoid() && !(*tp)->IsVoid()) || (*tp)->IsMsil())
                        {
                            error(ERR_NOT_AN_ALLOWED_TYPE);
                        }
                        else if (throwaway->IsStructured())
                        {
                            if (!(*tp)->IsVoid())
                            {
                                if (!Optimizer::cparams.prm_cplusplus || !cppCast(throwaway, tp, exp))
                                {
                                    error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                                }
                            }
                        }
                        else if ((throwaway->BaseType()->type == BasicType::memberptr_ ||
                                  (*tp)->BaseType()->type == BasicType::memberptr_) &&
                                 !throwaway->CompatibleType(*tp))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        else if (!templateDefinitionLevel || ((*tp)->IsInt() && isintconst(*exp)))
                        {
                            cast(*tp, exp);
                        }
                    }
                }
                else
                {
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                }
            }
        }
    }
    return;
}
void expression_typeid( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    (void)flags;
    getsym();
    if (needkw(Keyword::openpa_))
    {
        bool byType = false;
        if (TypeGenerator::StartOfType(nullptr, false))
        {
            *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
            byType = true;
        }
        else
        {
            expression_no_check(funcsp, nullptr, tp, exp, 0);
        }
        if (tp)
        {
            SYMBOL* sym = namespacesearch("__GetTypeInfo", globalNameSpace, false, false);
            if (sym)
            {

                CallSite* funcparams = Allocate<CallSite>();
                Argument* arg = Allocate<Argument>();
                Argument* arg2 = Allocate<Argument>();
                SYMBOL* rtti;
                SYMBOL* val;
                Type* valtp = Type::MakeType(BasicType::pointer_,
                                             &stdpointer);  // space for the virtual pointer and a pointer to the class data
                valtp->array = true;
                valtp->size = 2 * getSize(BasicType::pointer_);
                valtp->esize = MakeIntExpression(ExpressionNode::c_i_, 2);
                val = makeID(StorageClass::auto_, valtp, nullptr, AnonymousName());
                val->sb->allocate = true;
                if (theCurrentFunc)
                {
                    localNameSpace->front()->syms->Add(val);
                }
                else
                {
                    insertInitSym(val);
                }
                sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
                funcparams->arguments = argumentListFactory.CreateList();
                funcparams->arguments->push_back(arg);
                funcparams->arguments->push_back(arg2);
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = MakeExpression(ExpressionNode::pc_, sym);
                funcparams->ascall = true;
                arg->tp = &stdpointer;
                arg->exp = MakeExpression(ExpressionNode::auto_, val);
                arg2->tp = &stdpointer;
                if (!byType && (*tp)->IsStructured() && (*tp)->BaseType()->sp->sb->hasvtab)
                {
                    Dereference(&stdpointer, exp);
                    *exp = MakeExpression(ExpressionNode::sub_, *exp, MakeIntExpression(ExpressionNode::c_i_, VTAB_XT_OFFS));
                    Dereference(&stdpointer, exp);
                    arg2->exp = *exp;
                }
                else
                {
                    rtti = RTTIDumpType(*tp, true);
                    arg2->exp = rtti ? MakeExpression(ExpressionNode::global_, rtti) : MakeIntExpression(ExpressionNode::c_i_, 0);
                    funcparams->rttiType = rtti;
                }
                *exp = MakeExpression(funcparams);
                sym = namespacesearch("std", globalNameSpace, false, false);
                if (sym && sym->sb->storage_class == StorageClass::namespace_)
                {
                    sym = namespacesearch("type_info", sym->sb->nameSpaceValues, true, false);
                    if (sym)
                    {
                        if (!sym->tp->syms)
                            error(ERR_NEED_TYPEINFO_H);
                        *tp = Type::MakeType(BasicType::const_, sym->tp);
                        (*tp)->lref = true;
                    }
                }
            }
        }
        needkw(Keyword::closepa_);
    }
    return;
}
bool insertOperatorParams(SYMBOL* funcsp, Type** tp, EXPRESSION** exp, CallSite* funcparams, int flags)
{
    SYMBOL *s2 = nullptr, *s3;
    const char* name = overloadNameTab[(int)Keyword::openpa_ - (int)Keyword::new_ + CI_NEW];
    Type* tpx;
    if (!(*tp)->IsStructured())
        return false;
    if ((*tp)->IsStructured())
    {
        DeclarationScope scope((*tp)->BaseType()->sp);
        s2 = classsearch(name, false, false, true);
        funcparams->thistp = Type::MakeType(BasicType::pointer_, (*tp)->BaseType());
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
    tpx = Type::MakeType(BasicType::aggregate_);
    s3 = makeID(StorageClass::overloads_, tpx, nullptr, name);
    tpx->sp = s3;
    SetLinkerNames(s3, Linkage::c_);
    tpx->syms = symbols->CreateSymbolTable();
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
        if (!isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, s3, funcsp, funcparams->thisptr, false))
            errorsym(ERR_CANNOT_ACCESS, s3);
        s3->sb->throughClass = s3->sb->parentClass != nullptr;
        funcparams->sp = s3;
        funcparams->functp = s3->tp;
        if (funcparams->sp->sb->xcMode != xc_unspecified && funcparams->sp->sb->xcMode != xc_none)
            hasFuncCall = true;
        *exp = MakeExpression(funcparams);
        *tp = s3->tp;
        CheckCalledException(s3, *exp);
        StatementGenerator sg(s3);
        sg.CompileFunctionFromStream();
        return true;
    }
    funcparams->thistp = nullptr;
    funcparams->thisptr = nullptr;
    return false;
}
bool FindOperatorFunction(ovcl cls, Keyword kw, SYMBOL* funcsp, Type** tp, EXPRESSION** exp, Type* tp1, EXPRESSION* exp1,
                          std::list<Argument*>* args, int flags)
{ 
    if ((int)kw >= (int)Keyword::new_ && (int)kw <= (int)Keyword::unary_and_)
    {
        SYMBOL* s1 = nullptr, * s2 = nullptr, * s3, * s4 = nullptr, * s5 = nullptr;
        CallSite* funcparams;
        const char* name = overloadNameTab[(int)kw - (int)Keyword::new_ + CI_NEW];
        Type* tpin = *tp;
        Type* tpx;
        if (!*tp)
            return false;
        Type* tpClean = *tp;
        if (tpClean->IsRef())
            tpClean = tpClean->BaseType()->btp;
        Type* tp1Clean = tp1;
        if (tp1Clean && tp1Clean->IsRef())
            tp1Clean = tp1Clean->BaseType()->btp;
        if (tpClean && tpClean->scoped && tpClean->BaseType()->type != BasicType::enum_)
            tpClean = tpClean->btp;
        if (tp1Clean && tp1Clean->scoped && tp1Clean->BaseType()->type != BasicType::enum_)
            tp1Clean = tp1Clean->btp;
        if (!tpClean->IsStructured() && tpClean->BaseType()->type != BasicType::enum_ &&
            ((!tp1Clean && !args) || (tp1Clean && !tp1Clean->IsStructured() && tp1Clean->BaseType()->type != BasicType::enum_)))
            return false;

        (*tp)->InstantiateDeferred();
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
        DeclarationScope scope;
        // next find some occurrance in the class or struct
        if ((*tp)->IsStructured())
        {
            int n;
            enclosingDeclarations.Add((*tp)->BaseType()->sp);
            s2 = classsearch(name, false, false, true);
            TemplateNamespaceScope namespaceScope((*tp)->BaseType()->sp);  // used for more than just templates here
            s4 = namespacesearch(name, globalNameSpace, false, false);
        }
        else
        {
            if (tpClean->BaseType()->type == BasicType::enum_ && tpClean->sp)
            {
                std::list<SYMBOL*> aa{ tpClean->sp->sb->parentNameSpace };
                tpClean->sp->sb->templateNameSpace = tpClean->sp->sb->parentNameSpace ? &aa : nullptr;
                TemplateNamespaceScope namespaceScope(tpClean->BaseType()->sp);  // used for more than just templates here
                s4 = namespacesearch(name, globalNameSpace, false, false);
                tpClean->sp->sb->templateNameSpace = nullptr;
            }
        }
        if (tp1)
        {
            if (tp1->IsStructured())
            {
                TemplateNamespaceScope namespaceScope(tp1->BaseType()->sp);  // used for more than just templates here
                s5 = namespacesearch(name, globalNameSpace, false, false);
                auto exp3 = exp1;
                while (exp3->type == ExpressionNode::comma_)
                    exp3 = exp3->right;
                if (exp3->type == ExpressionNode::thisref_)
                {
                    tp1 = tp1->CopyType(true);
                    tp1->BaseType()->lref = false;
                    tp1->BaseType()->rref = true;
                }
            }
            else if (tp1->BaseType()->type == BasicType::enum_)  // enum
            {
                std::list<SYMBOL*> aa{ tp1->BaseType()->sp->sb->parentNameSpace };
                tp1->BaseType()->sp->sb->templateNameSpace = tp1->BaseType()->sp->sb->parentNameSpace ? &aa : nullptr;
                TemplateNamespaceScope namespaceScope(tp1->BaseType()->sp);  // used for more than just templates here
                s5 = namespacesearch(name, globalNameSpace, false, false);
                tp1->BaseType()->sp->sb->templateNameSpace = nullptr;
            }
        }
        // quit if there are no matches because we will use the default...
        if (!s1 && !s2 && !s4 && !s5)
        {
            return false;
        }
        // finally make a shell to put all this in and add shims for any builtins we want to try
        tpx = Type::MakeType(BasicType::aggregate_);
        s3 = makeID(StorageClass::overloads_, tpx, nullptr, name);
        tpx->sp = s3;
        SetLinkerNames(s3, Linkage::c_);
        tpx->syms = symbols->CreateSymbolTable();
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
        funcparams = Allocate<CallSite>();
        {
            Argument* one = nullptr, * two = nullptr;
            if (*tp)
            {
                one = Allocate<Argument>();
                one->exp = *exp;
                one->tp = *tp;
            }
            if (args)
            {
                Argument* one = Allocate<Argument>();
                one->nested = args;
            }
            else if (tp1)
            {
                two = Allocate<Argument>();
                two->exp = exp1;
                two->tp = tp1;
            }
            else if (cls == ovcl_unary_postfix)
            {
                two = Allocate<Argument>();
                two->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                two->tp = &stdint;
            }
            if (one || two)
            {
                funcparams->arguments = argumentListFactory.CreateList();
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
        Type* ctype = *tp;
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
                if (!tpClean->IsStructured() && (!tp1Clean || !tp1Clean->IsStructured()))
                {
                    auto it = s3->tp->BaseType()->syms->begin();
                    if ((*it)->sb->thisPtr)
                        ++it;
                    Type* arg1 = (*it)->tp;
                    ++it;
                    Type* arg2 = tp1 && it != s3->tp->BaseType()->syms->end() ? (*it)->tp : nullptr;
                    if (arg1 && arg1->IsRef())
                        arg1 = arg1->BaseType()->btp;
                    if (arg2 && arg2->IsRef())
                        arg2 = arg2->BaseType()->btp;
                    if (((tpClean->BaseType()->type == BasicType::enum_) != (arg1->BaseType()->type == BasicType::enum_)) ||
                        (tp1Clean &&
                            ((tp1Clean->BaseType()->type == BasicType::enum_) != (arg2->BaseType()->type == BasicType::enum_))))
                        return false;
                    break;
                }
            }
            *tp = ctype;
            if (ismember(s3))
            {
                funcparams->arguments->pop_front();
                funcparams->thistp = Type::MakeType(BasicType::pointer_, tpin);
                funcparams->thisptr = *exp;
            }
            s3->sb->throughClass = s3->sb->parentClass != nullptr;
            funcparams->sp = s3;
            funcparams->functp = s3->tp;
            *exp = MakeExpression(funcparams);
            *tp = s3->tp;
            expression_arguments(funcsp, tp, exp, 0, true);
            if (s3->sb->defaulted && kw == Keyword::assign_)
                createAssignment(s3->sb->parentClass, s3);
            CheckCalledException(s3, funcparams->thisptr);
            StatementGenerator::DestructorsForArguments(funcparams->arguments);
            StatementGenerator sg(s3);
            sg.CompileFunctionFromStream();
            return true;
        }
    }
    return false;
}
void expression_new( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool global, int flags)
{
    CallSite* placement = Allocate<CallSite>();
    CallSite* initializers = nullptr;
    const char* name = overloadNameTab[CI_NEW];
    Type* tpf = nullptr;
    EXPRESSION *arrSize = nullptr, *exp1;
    SYMBOL* s1 = nullptr;
    EXPRESSION *val = nullptr, *newfunc = nullptr;
    *exp = nullptr;
    *tp = nullptr;
    getsym();
    if (MATCHKW(Keyword::openpa_))
    {
        getsym();
        if (TypeGenerator::StartOfType(nullptr, false))
        {
            // type in parenthesis
            *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
            needkw(Keyword::closepa_);
        }
        else
        {
            // placement new
            --*currentStream;
            getArgs(funcsp, placement, Keyword::closepa_, true, 0);
        }
    }
    if (!*tp)
    {
        if (MATCHKW(Keyword::openpa_))
        {
            // type in parenthesis
            getsym();
            *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
            needkw(Keyword::closepa_);
        }
        else
        {
            // new type Keyword::id_
            *tp = TypeGenerator::TypeId(funcsp, StorageClass::auto_, true, true, false);
            if (MATCHKW(Keyword::openbr_))
            {
                Type* tp1 = nullptr;
                EXPRESSION* exp = nullptr;
                name = overloadNameTab[CI_NEWA];
                getsym();
                expression(funcsp, nullptr, &tp1, &exp, flags);
                if (tp1->IsStructured())
                    castToArithmeticInternal(false, &tp1, &exp, (Keyword)-1, &stdint, false);
                if (!tp1->IsInt())
                {
                    error(ERR_NEED_INTEGER_TYPE);
                }
                if (tp1->type != BasicType::unsigned_)
                {
                    tp1 = &stdunsigned;
                    cast(tp1, &exp);
                }
                needkw(Keyword::closebr_);
                ParseAttributeSpecifiers(funcsp, true);
                arrSize = exp;
                while (MATCHKW(Keyword::openbr_))
                {
                    getsym();
                    expression(funcsp, nullptr, &tp1, &exp, flags);
                    optimize_for_constants(&exp);
                    if (!tp1->IsInt())
                    {
                        error(ERR_NEED_INTEGER_TYPE);
                    }
                    else if (!isintconst(exp))
                    {
                        error(ERR_CONSTANT_EXPRESSION_EXPECTED);
                    }
                    else
                    {
                        arrSize = MakeExpression(ExpressionNode::mul_, arrSize, exp);
                    }
                    needkw(Keyword::closebr_);
                    ParseAttributeSpecifiers(funcsp, true);
                }
                optimize_for_constants(&arrSize);
            }
        }
    }
    if (*tp)
    {
        EXPRESSION* sz;
        Argument* sza;
        int n;
        (*tp)->InstantiateDeferred();
        if ((*tp)->IsStructured())
        {
            (*tp)->InitializeDeferred();
            (*tp)->InstantiateDeferred();
            n = (*tp)->BaseType()->sp->tp->size;
        }
        else
        {
            n = (*tp)->size;
        }
        if ((*tp)->IsRef())
            error(ERR_NEW_NO_ALLOCATE_REFERENCE);
        if (arrSize && (*tp)->IsStructured())
        {
            int al = n % (*tp)->BaseType()->sp->sb->attribs.inheritable.structAlign;
            if (al != 0)
                n += (*tp)->BaseType()->sp->sb->attribs.inheritable.structAlign - al;
        }
        sz = MakeIntExpression(ExpressionNode::c_i_, n);
        if (arrSize)
        {
            sz = MakeExpression(ExpressionNode::mul_, sz, arrSize);
        }
        optimize_for_constants(&sz);
        sza = Allocate<Argument>();
        sza->exp = sz;
        sza->tp = &stdint;
        if (!placement->arguments)
            placement->arguments = argumentListFactory.CreateList();
        placement->arguments->push_front(sza);
    }
    else
    {
        error(ERR_TYPE_NAME_EXPECTED);
        *tp = &stdint;  // error handling
    }
    if (!global)
    {
        if ((*tp)->IsStructured())
        {
            DeclarationScope scope((*tp)->BaseType()->sp);
            s1 = classsearch(name, false, false, true);
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
        val = AnonymousVar(StorageClass::auto_, &stdpointer);
        sym = val->v.sp;
        sym->sb->decoratedName = sym->name;
        //        if (localNameSpace->front()->syms)
        //            insert(sym, localNameSpace->front()->syms);
        Dereference(&stdpointer, &val);
        s1->sb->throughClass = s1->sb->parentClass != nullptr;
        if (s1->sb->throughClass && !isAccessible(s1->sb->parentClass, s1->sb->parentClass, s1, funcsp,
                                                  placement->thisptr ? AccessLevel::protected_ : AccessLevel::public_, false))
        {
            errorsym(ERR_CANNOT_ACCESS, s1);
        }
        placement->sp = s1;
        placement->functp = s1->tp;
        placement->ascall = true;
        placement->fcall = MakeExpression(ExpressionNode::pc_, s1);
        //        placement->sb->noinline = (flags & _F_NOINLINE) | s1->sb->noinline;
        exp1 = MakeExpression(placement);
        newfunc = exp1;
    }
    if (KW() == Keyword::openpa_)
    {
        // initializers
        if ((*tp)->IsStructured())
        {
            initializers = Allocate<CallSite>();
            getArgs(funcsp, initializers, Keyword::closepa_, true, 0);
            if (s1)
            {
                *exp = val;
                tpf = *tp;
                CallConstructor(&tpf, exp, initializers, false, arrSize, true, false, false, true, false, false, true);
                PromoteConstructorArgs(initializers->sp, initializers);
            }
            if (!initializers->arguments || !initializers->arguments->size())
            {
                // empty arguments means value construct...
                EXPRESSION* clr = MakeExpression(ExpressionNode::blockclear_, val);
                clr->size = *tp;
                *exp = MakeExpression(ExpressionNode::comma_, clr, *exp);
            }
        }
        else
        {
            exp1 = nullptr;
            initializers = Allocate<CallSite>();
            getArgs(funcsp, initializers, Keyword::closepa_, true, 0);
            if (initializers->arguments && initializers->arguments->size())
            {
                if (!initializers->arguments->front()->tp->SameType(*tp) || initializers->arguments->size() > 1)
                {
                    if (!templateDefinitionLevel)
                        error(ERR_NEED_NUMERIC_EXPRESSION);
                }
                else
                {
                    exp1 = initializers->arguments->front()->exp;
                    DeduceAuto(tp, initializers->arguments->front()->tp, exp1, false);
                    (*tp)->UpdateRootTypes();
                    if (exp1 && val)
                    {
                        EXPRESSION* pval = val;
                        cast(*tp, &exp1);
                        Dereference(*tp, &pval);
                        *exp = MakeExpression(ExpressionNode::assign_, pval, exp1);
                    }
                }
            }
            else
            {
                // default-construct
                if (val)
                {
                    EXPRESSION* pval = val;
                    exp1 = MakeIntExpression(ExpressionNode::c_i_, 0);
                    cast(*tp, &exp1);
                    Dereference(*tp, &pval);
                    *exp = MakeExpression(ExpressionNode::assign_, pval, exp1);
                }
            }
        }
    }
    else if (KW() == Keyword::begin_)
    {
        // braced initializers
        // this is just a little buggy, for example new aa[10][10] { {1},{2},{3} } won't work properly
        // i can't make it work well because the array indexes aren't necessarily constants...
        if (*tp)
        {
            std::list<Initializer*>* init = nullptr;
            EXPRESSION* base = val;
            Type* tp1 = *tp;
            SYMBOL* sym = nullptr;
            if ((*tp)->IsStructured())
            {
                sym = AnonymousVar(StorageClass::localstatic_, *tp)->v.sp;
            }
            if (arrSize)
            {
                tp1 = Type::MakeType(BasicType::pointer_, *tp);
                tp1->size = 0;
                tp1->array = 1;
                tp1->esize = arrSize;
            }
            initType(funcsp, 0, StorageClass::auto_, &init, nullptr, tp1, sym, false, false, _F_EXPLICIT);
            if (!(*tp)->IsStructured() && !arrSize)
            {
                if (init->size() != 1 || (init->front()->basetp && init->front()->basetp->IsStructured()))
                    error(ERR_NONSTRUCTURED_INIT_LIST);
            }
            // dest is lost for these purposes
            if (theCurrentFunc)
            {
                if (init->front()->exp)
                {
                    *exp = ConverInitializersToExpression(tp1, nullptr, nullptr, funcsp, init, base, false);
                }
                if (arrSize && !(*tp)->IsStructured())
                {
                    exp1 = MakeExpression(
                        ExpressionNode::blockclear_, base,
                        MakeExpression(ExpressionNode::mul_, arrSize, MakeIntExpression(ExpressionNode::c_i_, (*tp)->size)));
                    exp1->size = *tp;
                    *exp = *exp ? MakeExpression(ExpressionNode::comma_, exp1, *exp) : exp1;
                }
                else if ((*tp)->IsStructured() && (arrSize || !init->front()->exp))
                {
                    EXPRESSION* exp1;
                    exp1 = val;
                    if (arrSize)
                    {
                        auto back = init->back();
                        arrSize = MakeExpression(ExpressionNode::sub_, arrSize,
                                                 MakeIntExpression(ExpressionNode::c_i_, back->offset / ((*tp)->BaseType()->size)));
                        exp1 = MakeExpression(ExpressionNode::add_, exp1, MakeIntExpression(ExpressionNode::c_i_, back->offset));
                    }
                    tpf = *tp;
                    CallConstructor(&tpf, &exp1, nullptr, false, arrSize, true, false, false, true, false, false, true);
                    *exp = *exp ? MakeExpression(ExpressionNode::comma_, *exp, exp1) : exp1;
                }
            }
        }
    }
    else if ((*tp)->IsStructured())
    {
        if (s1)
        {
            // call default constructor
            *exp = val;
            tpf = *tp;
            CallConstructor(&tpf, exp, nullptr, false, arrSize, true, false, false, true, false, false, true);
        }
    }
    *tp = Type::MakeType(BasicType::pointer_, *tp);

    if (val && newfunc)
    {
        exp1 = MakeExpression(ExpressionNode::assign_, val, newfunc);
        if (*exp)
            exp1 = MakeExpression(ExpressionNode::check_nz_, MakeExpression(ExpressionNode::comma_, exp1, *exp), val);
        *exp = exp1;
    }
    else
    {
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
    }
    return;
}
void expression_delete( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool global, int flags)
{
    bool byArray = false;
    SYMBOL* s1 = nullptr;
    CallSite* funcparams;
    Argument* one = nullptr;
    EXPRESSION *exp1 = nullptr, *exp2;
    Type* tpf;
    const char* name = overloadNameTab[CI_DELETE];
    EXPRESSION* in;
    EXPRESSION *var, *asn;
    getsym();
    if (MATCHKW(Keyword::openbr_))
    {
        getsym();
        needkw(Keyword::closebr_);
        byArray = true;
        name = overloadNameTab[CI_DELETEA];
        exp1 = MakeIntExpression(ExpressionNode::c_i_, 0);  // signal to the runtime to load the number of elems dynamically
    }
    expression_cast(funcsp, nullptr, tp, exp, nullptr, flags);
    if (!*tp)
    {
        error(ERR_IDENTIFIER_EXPECTED);
        *tp = &stdint;  // error handling
    }
    if (!(*tp)->IsPtr())
    {
        error(ERR_POINTER_TYPE_EXPECTED);
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        return;
    }
    if (!templateDefinitionLevel && (*tp)->BaseType()->btp->IsStructured())
    {
        (*tp)->BaseType()->btp->InstantiateDeferred();
        if ((*tp)->BaseType()->btp->BaseType()->sp->tp->size == 0)
            errorsym(ERR_DELETE_OF_POINTER_TO_UNDEFINED_TYPE, (*tp)->BaseType()->btp->BaseType()->sp);
    }
    asn = *exp;
    var = AnonymousVar(StorageClass::auto_, *tp);
    Dereference(*tp, &var);
    in = exp2 = *exp = var;
    if ((*tp)->BaseType()->btp && (*tp)->BaseType()->btp->IsStructured())
    {
        CallDestructor((*tp)->BaseType()->btp->BaseType()->sp, nullptr, exp, exp1, true, true, false, false);
    }
    exp1 = exp2;
    if (!global)
    {
        if ((*tp)->IsStructured())
        {
            DeclarationScope scope((*tp)->BaseType()->sp);
            s1 = classsearch(name, false, false, true);
        }
    }
    if (!s1)
    {
        s1 = namespacesearch(name, globalNameSpace, false, false);
    }
    // should always find something
    funcparams = Allocate<CallSite>();
    one = Allocate<Argument>();
    one->exp = exp1;
    one->tp = &stdpointer;
    funcparams->arguments = argumentListFactory.CreateList();
    funcparams->arguments->push_back(one);
    funcparams->ascall = true;
    s1 = GetOverloadedFunction(&tpf, &funcparams->fcall, s1, funcparams, nullptr, true, false, flags);
    if (s1)
    {
        s1->sb->throughClass = s1->sb->parentClass != nullptr;
        if (s1->sb->throughClass && !isAccessible(s1->sb->parentClass, s1->sb->parentClass, s1, funcsp,
                                                  funcparams->thisptr ? AccessLevel::protected_ : AccessLevel::public_, false))
        {
            errorsym(ERR_CANNOT_ACCESS, s1);
        }
        funcparams->sp = s1;
        funcparams->functp = s1->tp;
        funcparams->ascall = true;
        funcparams->fcall = MakeExpression(ExpressionNode::pc_, s1);
        // funcparams->sb->noinline =  (flags & _F_NOINLINE) | s1->sb->noinline;
        exp1 = MakeExpression(funcparams);
        exp1 = MakeExpression(ExpressionNode::comma_, *exp, exp1);
        exp1 = MakeExpression(ExpressionNode::check_nz_, MakeExpression(ExpressionNode::comma_, in, exp1),
                              MakeIntExpression(ExpressionNode::c_i_, 0));
        *exp = exp1;
        *tp = s1->tp;
    }
    var = MakeExpression(ExpressionNode::assign_, var, asn);
    *exp = MakeExpression(ExpressionNode::comma_, var, *exp);
    *tp = &stdvoid;
    return;
}
static bool noexceptStmt(std::list<Statement*>* block);
static bool noexceptExpression(EXPRESSION* node)
{
    CallSite* fp;
    bool rv = true;
    if (node == 0)
        return rv;
    switch (node->type)
    {
        case ExpressionNode::auto_:
            break;
        case ExpressionNode::const_:
            break;
        case ExpressionNode::c_ll_:
        case ExpressionNode::c_ull_:
        case ExpressionNode::c_d_:
        case ExpressionNode::c_ld_:
        case ExpressionNode::c_f_:
        case ExpressionNode::c_dc_:
        case ExpressionNode::c_ldc_:
        case ExpressionNode::c_fc_:
        case ExpressionNode::c_di_:
        case ExpressionNode::c_ldi_:
        case ExpressionNode::c_fi_:
        case ExpressionNode::c_i_:
        case ExpressionNode::c_l_:
        case ExpressionNode::c_ui_:
        case ExpressionNode::c_ul_:
        case ExpressionNode::c_c_:
        case ExpressionNode::c_bool_:
        case ExpressionNode::c_uc_:
        case ExpressionNode::c_wc_:
        case ExpressionNode::c_u16_:
        case ExpressionNode::c_u32_:
        case ExpressionNode::c_string_:
        case ExpressionNode::nullptr_:
        case ExpressionNode::memberptr_:
        case ExpressionNode::structelem_:
            break;
        case ExpressionNode::global_:
        case ExpressionNode::pc_:
        case ExpressionNode::labcon_:
        case ExpressionNode::absolute_:
        case ExpressionNode::threadlocal_:
            break;
        case ExpressionNode::l_sp_:
        case ExpressionNode::l_fp_:
        case ExpressionNode::bits_:
        case ExpressionNode::l_f_:
        case ExpressionNode::l_d_:
        case ExpressionNode::l_ld_:
        case ExpressionNode::l_fi_:
        case ExpressionNode::l_di_:
        case ExpressionNode::l_ldi_:
        case ExpressionNode::l_fc_:
        case ExpressionNode::l_dc_:
        case ExpressionNode::l_ldc_:
        case ExpressionNode::l_c_:
        case ExpressionNode::l_wc_:
        case ExpressionNode::l_u16_:
        case ExpressionNode::l_u32_:
        case ExpressionNode::l_s_:
        case ExpressionNode::l_ul_:
        case ExpressionNode::l_l_:
        case ExpressionNode::l_p_:
        case ExpressionNode::l_ref_:
        case ExpressionNode::l_i_:
        case ExpressionNode::l_ui_:
        case ExpressionNode::l_inative_:
        case ExpressionNode::l_unative_:
        case ExpressionNode::l_uc_:
        case ExpressionNode::l_us_:
        case ExpressionNode::l_bool_:
        case ExpressionNode::l_bit_:
        case ExpressionNode::l_ll_:
        case ExpressionNode::l_ull_:
        case ExpressionNode::l_bitint_:
        case ExpressionNode::l_ubitint_:
        case ExpressionNode::l_string_:
        case ExpressionNode::l_object_:
            rv = noexceptExpression(node->left);
            break;
        case ExpressionNode::uminus_:
        case ExpressionNode::compl_:
        case ExpressionNode::not_:
        case ExpressionNode::x_f_:
        case ExpressionNode::x_d_:
        case ExpressionNode::x_ld_:
        case ExpressionNode::x_fi_:
        case ExpressionNode::x_di_:
        case ExpressionNode::x_ldi_:
        case ExpressionNode::x_fc_:
        case ExpressionNode::x_dc_:
        case ExpressionNode::x_ldc_:
        case ExpressionNode::x_ll_:
        case ExpressionNode::x_ull_:
        case ExpressionNode::x_i_:
        case ExpressionNode::x_ui_:
        case ExpressionNode::x_inative_:
        case ExpressionNode::x_unative_:
        case ExpressionNode::x_c_:
        case ExpressionNode::x_u16_:
        case ExpressionNode::x_u32_:
        case ExpressionNode::x_wc_:
        case ExpressionNode::x_uc_:
        case ExpressionNode::x_bool_:
        case ExpressionNode::x_bit_:
        case ExpressionNode::x_s_:
        case ExpressionNode::x_us_:
        case ExpressionNode::x_l_:
        case ExpressionNode::x_ul_:
        case ExpressionNode::x_p_:
        case ExpressionNode::x_fp_:
        case ExpressionNode::x_sp_:
        case ExpressionNode::x_bitint_:
        case ExpressionNode::x_ubitint_:
        case ExpressionNode::x_string_:
        case ExpressionNode::x_object_:
        case ExpressionNode::trapcall_:
        case ExpressionNode::shiftby_:
            /*        case ExpressionNode::movebyref_: */
        case ExpressionNode::substack_:
        case ExpressionNode::alloca_:
        case ExpressionNode::loadstack_:
        case ExpressionNode::savestack_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
            rv = noexceptExpression(node->left);
            break;
        case ExpressionNode::assign_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
            rv = noexceptExpression(node->right) && noexceptExpression(node->left);
            break;
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
            rv = noexceptExpression(node->left);
            break;
        case ExpressionNode::add_:
        case ExpressionNode::sub_:
            /*        case ExpressionNode::addcast_: */
        case ExpressionNode::lsh_:
        case ExpressionNode::arraylsh_:
        case ExpressionNode::rsh_:
        case ExpressionNode::comma_:
        case ExpressionNode::check_nz_:
            /*        case ExpressionNode::dvoid_: */
        case ExpressionNode::arraymul_:
        case ExpressionNode::arrayadd_:
        case ExpressionNode::arraydiv_:
        case ExpressionNode::structadd_:
        case ExpressionNode::mul_:
        case ExpressionNode::div_:
        case ExpressionNode::umul_:
        case ExpressionNode::udiv_:
        case ExpressionNode::umod_:
        case ExpressionNode::ursh_:
        case ExpressionNode::mod_:
        case ExpressionNode::and_:
        case ExpressionNode::or_:
        case ExpressionNode::xor_:
        case ExpressionNode::lor_:
        case ExpressionNode::land_:
        case ExpressionNode::eq_:
        case ExpressionNode::ne_:
        case ExpressionNode::gt_:
        case ExpressionNode::ge_:
        case ExpressionNode::lt_:
        case ExpressionNode::le_:
        case ExpressionNode::ugt_:
        case ExpressionNode::uge_:
        case ExpressionNode::ult_:
        case ExpressionNode::ule_:
        case ExpressionNode::hook_:
        case ExpressionNode::intcall_:
        case ExpressionNode::stackblock_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::mp_compare_:
        case ExpressionNode::dot_:
        case ExpressionNode::pointsto_:
            /*		case ExpressionNode::array_: */
            rv = noexceptExpression(node->right) && noexceptExpression(node->left);
            break;
        case ExpressionNode::mp_as_bool_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::argnopush_:
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
        case ExpressionNode::thisref_:
        case ExpressionNode::funcret_:
        case ExpressionNode::select_:
        case ExpressionNode::constexprconstructor_:
            rv = noexceptExpression(node->left);
            break;
        case ExpressionNode::atomic_:
        case ExpressionNode::construct_:
            break;
        case ExpressionNode::callsite_:
            fp = node->v.func;
            {
                SYMBOL* sym = fp->sp;
                if (sym->tp->type == BasicType::aggregate_)
                {
                    if (sym->tp->syms->size() == 1)
                    {
                        sym = sym->tp->syms->front();
                    }
                }
                rv = sym->sb->noExcept;
            }
            break;
        case ExpressionNode::stmt_:
            rv = noexceptStmt(node->v.stmt);
            break;
        case ExpressionNode::templateparam_:
            break;
        default:
            diag("noexceptExpression");
            break;
    }
    return rv;
}
static bool noexceptStmt(std::list<Statement*>* blocks)
{
    bool rv = true;
    for (auto block : *blocks)
    {
        switch (block->type)
        {
            case StatementNode::genword_:
                break;
            case StatementNode::catch_:
            case StatementNode::try_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                rv &= noexceptStmt(block->lower);
                break;
            case StatementNode::return_:
            case StatementNode::expr_:
            case StatementNode::declare_:
                rv &= noexceptExpression(block->select);
                break;
            case StatementNode::goto_:
            case StatementNode::label_:
                break;
            case StatementNode::select_:
            case StatementNode::notselect_:
                rv &= noexceptExpression(block->select);
                break;
            case StatementNode::switch_:
                rv &= noexceptExpression(block->select);
                rv &= noexceptStmt(block->lower);
                break;
            case StatementNode::block_:
                rv &= noexceptStmt(block->lower);
                rv &= noexceptStmt(block->blockTail);
                break;
            case StatementNode::passthrough_:
                break;
            case StatementNode::datapassthrough_:
                break;
            case StatementNode::nop_:
                break;
            case StatementNode::line_:
            case StatementNode::varstart_:
            case StatementNode::dbgblock_:
                break;
            default:
                diag("Invalid block type in noexceptStmt");
                break;
        }
    }
    return rv;
}
void expression_noexcept( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    getsym();
    if (needkw(Keyword::openpa_))
    {
        expression_no_check(funcsp, nullptr, tp, exp, _F_SIZEOF | _F_IS_NOTHROW);
        *exp = MakeIntExpression(ExpressionNode::c_i_, noexceptExpression(*exp));
        *tp = &stdbool;

        needkw(Keyword::closepa_);
    }
    return;
}
void ResolveTemplateVariable(Type** ttype, EXPRESSION** texpr, Type* rtype, Type* atype)
{
    EXPRESSION* exp = *texpr;
    bool lval = false;
    if (TakeAddress(&exp))
    {
        lval = true;
    }
    if (exp->type == ExpressionNode::templateparam_)
    {
        if (exp->v.sp->sb && exp->v.sp->sb->templateLevel && !exp->v.sp->sb->instantiated)
        {
            SYMBOL* sym;
            Type* type;
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
            auto second = Allocate<TEMPLATEPARAM>();

            second->type = TplType::typename_;
            second->byClass.dflt = type;
            params->push_back(TEMPLATEPARAMPAIR{nullptr, second});
            sym = GetVariableTemplate(exp->v.sp, params);
            if (sym)
            {
                *texpr = MakeExpression(ExpressionNode::global_, sym);
                *ttype = type;
                if (lval)
                    Dereference(type, texpr);
            }
            else
            {
                diag("ResolveTemplateVariables no var");
            }
        }
    }
}
}  // namespace Parser