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
#include <functional>
#include <stack>

#include "compiler.h"
#include "config.h"
#include "types.h"
#include "overload.h"
#include "lex.h"
#include "expr.h"
#include "stmt.h"
#include "ccerr.h"
#include "OptUtils.h"
#include "mangle.h"
#include "help.h"
#include "memory.h"
#include "initbackend.h"
#include "templatedecl.h"
#include "namespace.h"
#include "declare.h"
#include "declcons.h"
#include "declcpp.h"
#include "init.h"
#include "rtti.h"
#include "ListFactory.h"
#include "constopt.h"
#include "exprcpp.h"
#include "class.h"
#include "templateutil.h"
#include "casts.h"
namespace Parser
{
    bool castToArithmeticInternal(bool integer, Type** tp, EXPRESSION** exp, Keyword kw, Type* other, bool implicit)
    {
        (void)kw;
        SYMBOL* sym = (*tp)->BaseType()->sp;

        if (other && other->IsPtr() && (kw == Keyword::plus_ || kw == Keyword::minus_ || (kw >= Keyword::eq_ && kw <= Keyword::geq_)))
        {
            return castToPointer(tp, exp, kw, other);
        }
        if (!other || other->IsArithmetic())
        {
            SYMBOL* cst = integer ? lookupIntCast(sym, other ? other : &stdint, implicit)
                : lookupArithmeticCast(sym, other ? other : &stddouble, implicit);
            if (cst)
            {
                CallSite* params = Allocate<CallSite>();
                EXPRESSION* e1;
                params->fcall = MakeExpression(ExpressionNode::pc_, cst);
                params->thisptr = *exp;
                params->thistp = Type::MakeType(BasicType::pointer_, cst->sb->parentClass->tp);
                params->functp = cst->tp;
                params->sp = cst;
                params->ascall = true;
                *exp = DerivedToBase(cst->sb->parentClass->tp, *tp, *exp, 0);
                params->thisptr = *exp;
                {
                    e1 = MakeExpression(params);
                    if (params->sp->sb->xcMode != xc_unspecified && params->sp->sb->xcMode != xc_none)
                        hasFuncCall = true;
                }
                *exp = e1;
                if (other)
                    cast(other, exp);
                *tp = cst->tp->BaseType()->btp;
                return true;
            }
        }
        return false;
    }
    void castToArithmetic(bool integer, Type** tp, EXPRESSION** exp, Keyword kw, Type* other, bool implicit)
    {
        if (Optimizer::cparams.prm_cplusplus && (*tp)->IsStructured())
        {
            if (!castToArithmeticInternal(integer, tp, exp, kw, other, implicit))
            {
                // failed at conversion
                if (kw >= Keyword::new_ && kw <= Keyword::complx_)
                {
                    // LHS, put up an operator whatever message
                    char buf[4096];
                    char tbuf[4096];
                    memset(tbuf, 0, sizeof(tbuf));
                    tbuf[0] = 0;
                    (*tp)->BaseType()->ToString(tbuf + sizeof(tbuf), tbuf);
                    if (other)
                    {
                        Utils::StrCat(tbuf, ", ");
                        other->BaseType()->ToString(tbuf + sizeof(tbuf), tbuf + strlen(tbuf));
                    }
                    Optimizer::my_sprintf(buf, "operator %s(%s)", overloadXlateTab[(int)kw - (int)Keyword::new_ + CI_NEW], tbuf);
                    errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, buf);
                }
                else
                {
                    // otherwise RHS, do a can't convert message
                    if (!other->IsArithmetic())
                        other = &stdint;
                    errorConversionOrCast(true, *tp, other);
                }
            }
            else
            {
                if ((*tp)->IsRef())
                {
                    *tp = (*tp)->BaseType()->btp;
                    Dereference(*tp, exp);
                }
            }
        }
    }
    bool castToPointer(Type** tp, EXPRESSION** exp, Keyword kw, Type* other)
    {
        (void)kw;
        if (Optimizer::cparams.prm_cplusplus && (*tp)->IsStructured())
        {
            SYMBOL* sym = (*tp)->BaseType()->sp;
            if (other->IsPtr() || other->BaseType()->type == BasicType::memberptr_)
            {
                SYMBOL* cst = lookupPointerCast(sym, other);
                if (cst)
                {
                    CallSite* params = Allocate<CallSite>();
                    EXPRESSION* e1;
                    if (cst->tp->BaseType()->btp->IsFunctionPtr() && other->IsFunctionPtr())
                    {
                        Type** tpx = &cst->tp->BaseType()->btp, ** tpy = &other;
                        while ((*tpx) && (*tpy) && (*tpx)->type != BasicType::auto_)
                        {
                            tpx = &(*tpx)->btp;
                            tpy = &(*tpy)->btp;
                        }
                        if (*tpx && *tpy)
                            *tpx = *tpy;
                    }
                    *exp = DerivedToBase(cst->sb->parentClass->tp, *tp, *exp, 0);
                    params->fcall = MakeExpression(ExpressionNode::pc_, cst);
                    params->thisptr = *exp;
                    params->thistp = Type::MakeType(BasicType::pointer_, cst->sb->parentClass->tp);
                    params->functp = cst->tp;
                    params->sp = cst;
                    params->ascall = true;
                    e1 = MakeExpression(params);
                    if (params->sp->sb->xcMode != xc_unspecified && params->sp->sb->xcMode != xc_none)
                        hasFuncCall = true;
                    *exp = e1;
                    if (other->IsPtr())
                    {
                        cast(other, exp);
                    }
                    else
                    {
                        SYMBOL* retsp = makeID(StorageClass::auto_, other, nullptr, AnonymousName());
                        retsp->sb->allocate = true;
                        retsp->sb->attribs.inheritable.used = retsp->sb->assigned = true;
                        SetLinkerNames(retsp, Linkage::cdecl_);
                        localNameSpace->front()->syms->Add(retsp);
                        params->returnSP = retsp;
                        params->returnEXP = MakeExpression(ExpressionNode::auto_, retsp);
                    }
                    *tp = cst->tp->BaseType()->btp;
                    return true;
                }
            }
        }
        return false;
    }
    bool cppCast(Type* src, Type** tp, EXPRESSION** exp)
    {
        if (src->IsStructured())
        {
            if ((*tp)->IsStructured())
            {
                SYMBOL* sym = src->BaseType()->sp;
                SYMBOL* cst = lookupSpecificCast(sym, *tp);
                if (cst)
                {
                    CallSite* params = Allocate<CallSite>();
                    EXPRESSION* e1;
                    CheckCalledException(cst, *exp);
                    *exp = DerivedToBase(cst->sb->parentClass->tp, src, *exp, 0);
                    params->fcall = MakeExpression(ExpressionNode::pc_, cst);
                    params->thisptr = *exp;
                    params->thistp = Type::MakeType(BasicType::pointer_, cst->sb->parentClass->tp);
                    params->functp = cst->tp;
                    params->sp = cst;
                    params->ascall = true;
                    if ((*tp)->IsStructured() || (*tp)->IsBitInt())
                    {
                        EXPRESSION* ev = AnonymousVar(StorageClass::auto_, *tp);
                        SYMBOL* av = ev->v.sp;
                        params->returnEXP = ev;
                        params->returnSP = sym;
                        CallDestructor((*tp)->BaseType()->sp, nullptr, &ev, nullptr, true, false, false, true);
                        InsertInitializer(&av->sb->dest, *tp, ev, 0, true);
                    }
                    e1 = MakeExpression(params);
                    if (params->sp->sb->xcMode != xc_unspecified && params->sp->sb->xcMode != xc_none)
                        hasFuncCall = true;
                    *exp = e1;
                    *exp = DerivedToBase(*tp, cst->tp->BaseType()->btp, *exp, 0);
                    return true;
                }
            }
            else if ((*tp)->IsArithmetic())
            {
                Type* tp1 = src;
                auto rv = castToArithmeticInternal(false, &tp1, exp, (Keyword)-1, *tp, false);
                if (tp1->IsRef())
                    Dereference(tp1->BaseType()->btp, exp);
                return rv;
            }
            else if ((*tp)->IsPtr() || (*tp)->BaseType()->type == BasicType::memberptr_)
            {
                Type* tp1 = src;
                auto rv = castToPointer(&tp1, exp, (Keyword)-1, *tp);
                if (tp1->IsRef())
                    Dereference(tp1->BaseType()->btp, exp);
                return rv;
            }
        }
        return false;
    }
    bool doDynamicCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp)
    {
        if ((oldType->IsPtr() && (*newType)->IsPtr()) || (*newType)->IsRef())
        {
            Type* tpo = oldType;
            Type* tpn = *newType;
            if ((*newType)->IsRef())
            {
                tpn = (*newType)->BaseType()->btp;
            }
            else
            {
                tpo = oldType->BaseType()->btp;
                tpn = (*newType)->BaseType()->btp;
            }
            if ((!tpo->IsConst() || tpn->IsConst()) && tpo->IsStructured())
            {
                if (tpn->IsStructured() || tpn->BaseType()->type == BasicType::void_)
                {
                    if (tpn->BaseType()->type == BasicType::void_ || classRefCount(tpn->BaseType()->sp, tpo->BaseType()->sp) != 1)
                    {
                        // if we are going up in the class heirarchy that is the only time we really need to do
                        // a dynamic cast
                        SYMBOL* sym = namespacesearch("__dynamic_cast", globalNameSpace, false, false);
                        if (sym)
                        {
                            EXPRESSION* exp1 = *exp;
                            CallSite* funcparams = Allocate<CallSite>();
                            Argument* arg1 = Allocate<Argument>();  // thisptr
                            Argument* arg2 = Allocate<Argument>();  // excepttab from thisptr
                            Argument* arg3 = Allocate<Argument>();  // oldxt
                            Argument* arg4 = Allocate<Argument>();  // newxt
                            SYMBOL* oldrtti = RTTIDumpType(tpo, true);
                            SYMBOL* newrtti = tpn->BaseType()->type == BasicType::void_ ? nullptr : RTTIDumpType(tpn, true);
                            Dereference(&stdpointer, &exp1);
                            sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
                            arg1->exp = *exp;
                            arg1->tp = &stdpointer;
                            arg2->exp = MakeExpression(
                                ExpressionNode::hook_, *exp,
                                MakeExpression(ExpressionNode::comma_, exp1, MakeIntExpression(ExpressionNode::c_i_, 0)));

                            arg2->tp = &stdpointer;
                            arg3->exp =
                                oldrtti ? MakeExpression(ExpressionNode::global_, oldrtti) : MakeIntExpression(ExpressionNode::c_i_, 0);
                            arg3->tp = &stdpointer;
                            arg4->exp =
                                newrtti ? MakeExpression(ExpressionNode::global_, newrtti) : MakeIntExpression(ExpressionNode::c_i_, 0);
                            arg4->tp = &stdpointer;
                            funcparams->arguments = argumentListFactory.CreateList();
                            funcparams->arguments->push_back(arg1);
                            funcparams->arguments->push_back(arg2);
                            funcparams->arguments->push_back(arg3);
                            funcparams->arguments->push_back(arg4);
                            funcparams->sp = sym;
                            funcparams->functp = sym->tp;
                            funcparams->fcall = MakeExpression(ExpressionNode::pc_, sym);
                            funcparams->ascall = true;
                            funcparams->rttiType = oldrtti;
                            funcparams->rttiType2 = newrtti;
                            *exp = MakeExpression(ExpressionNode::lvalue_, MakeExpression(funcparams));
                        }
                        if ((*newType)->IsRef())
                            *newType = tpn;
                        if (!tpo->BaseType()->sp->sb->hasvtab || !tpo->BaseType()->sp->tp->syms->front())
                            errorsym(ERR_NOT_DEFINED_WITH_VIRTUAL_FUNCS, tpo->BaseType()->sp);
                        return true;
                    }
                    return doStaticCast(newType, oldType, exp, funcsp, true);
                }
            }
        }
        return false;
    }
    bool doStaticCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst)
    {
        bool rref = false;
        Type* orig = *newType;
        if ((*newType)->IsRef())
        {
            if ((*newType)->BaseType()->type == BasicType::rref_)
                rref = true;
            while ((*newType)->IsRef())
                *newType = (*newType)->BaseType()->btp;
        }
        // no change or stricter const qualification
        if ((*newType)->CompatibleType(oldType))
            return true;
        // conversion from nullptr to pointer
        if ((*newType)->IsPtr() && oldType->IsInt() && isconstzero(oldType, *exp))
            return true;
        // conversion to or from void pointer
        if ((((*newType)->IsVoidPtr() && (oldType->IsPtr() || oldType->IsFunction() || oldType->BaseType()->type == BasicType::memberptr_)) ||
            (((oldType->IsVoidPtr() || (*exp)->type == ExpressionNode::nullptr_) && (*newType)->IsPtr()) &&
                (!checkconst || (*newType)->BaseType()->btp->IsConst() || !oldType->BaseType()->btp->IsConst()))))
            return true;
        // conversion to void (discards type)
        if ((*newType)->IsVoid())
            return true;
        // conversion of one numeric value to another
        if (((*newType)->IsArithmetic() || ((*newType)->BaseType()->type == BasicType::enum_ && !(*newType)->BaseType()->scoped)) &&
            (oldType->IsArithmetic() || (oldType->BaseType()->type == BasicType::enum_ && !oldType->BaseType()->scoped)))
        {
            cast(*newType, exp);
            return true;
        }
        // floating to enumeration
        if (oldType->IsFloat() && (*newType)->BaseType()->type == BasicType::enum_)
        {
            cast((*newType)->BaseType()->btp, exp);
            return true;
        }
        // int to enum
        if (oldType->IsInt() && (*newType)->BaseType()->type == BasicType::enum_)
        {
            if (oldType->BaseType()->type != (*newType)->BaseType()->btp->type)
                cast((*newType)->BaseType()->btp, exp);
            return true;
        }
        // to int
        if ((*newType)->IsInt() && oldType->BaseType()->type == BasicType::enum_)
        {
            if ((*newType)->BaseType()->type != oldType->BaseType()->btp->type)
                cast(*newType, exp);
            return true;
        }
        // pointer to bool
        if ((*newType)->BaseType()->type == BasicType::bool_ &&
            (oldType->BaseType()->type == BasicType::pointer_ || oldType->BaseType()->type == BasicType::memberptr_))
        {
            cast((*newType)->BaseType(), exp);
            return true;
        }
        // base to derived pointer or derived to base pointer
        if (((*newType)->IsPtr() && oldType->IsPtr()) || orig->IsRef())
        {
            Type* tpo = oldType;
            Type* tpn = orig;
            if (tpn->IsRef())
            {
                *newType = tpn = orig->BaseType()->btp;
            }
            else
            {
                tpo = oldType->BaseType()->btp;
                tpn = orig->BaseType()->btp;
            }
            if ((!checkconst || !tpo->IsConst() || tpn->IsConst()) && (tpn->IsStructured() && tpo->IsStructured()))
            {
                int n = classRefCount(tpn->BaseType()->sp, tpo->BaseType()->sp);
                if (n == 1)
                {
                    // derived to base
                    EXPRESSION* v = Allocate<EXPRESSION>();
                    v->type = ExpressionNode::c_i_;
                    v = baseClassOffset(tpn->BaseType()->sp, tpo->BaseType()->sp, v);
                    optimize_for_constants(&v);
                    if (v->type == ExpressionNode::c_i_)  // check for no virtual base
                    {
                        if (v->v.i != 0)
                        {
                            EXPRESSION* varsp = AnonymousVar(StorageClass::auto_, &stdpointer);
                            EXPRESSION* var = MakeExpression(ExpressionNode::l_p_, varsp);
                            EXPRESSION* asn = MakeExpression(ExpressionNode::assign_, var, *exp);
                            EXPRESSION* left = MakeExpression(ExpressionNode::add_, var, v);
                            EXPRESSION* right = var;
                            if (v->type == ExpressionNode::l_p_)  // check for virtual base
                                v->left = var;
                            v = MakeExpression(ExpressionNode::hook_, var, MakeExpression(ExpressionNode::comma_, left, right));
                            v = MakeExpression(ExpressionNode::comma_, asn, v);
                            *exp = MakeExpression(ExpressionNode::lvalue_, v);
                        }
                        return true;
                    }
                }
                else if (!n && classRefCount(tpo->BaseType()->sp, tpn->BaseType()->sp) == 1)
                {
                    // base to derived
                    EXPRESSION* v = Allocate<EXPRESSION>();
                    v->type = ExpressionNode::c_i_;
                    v = baseClassOffset(tpo->BaseType()->sp, tpn->BaseType()->sp, v);
                    optimize_for_constants(&v);
                    if (v->type == ExpressionNode::c_i_)  // check for no virtual base
                    {
                        if (v->v.i != 0)
                        {
                            EXPRESSION* varsp = AnonymousVar(StorageClass::auto_, &stdpointer);
                            EXPRESSION* var = MakeExpression(ExpressionNode::l_p_, varsp);
                            EXPRESSION* asn = MakeExpression(ExpressionNode::assign_, var, *exp);
                            EXPRESSION* left = MakeExpression(ExpressionNode::sub_, var, v);
                            EXPRESSION* right = var;
                            if (v->type == ExpressionNode::l_p_)  // check for virtual base
                                v->left = var;
                            v = MakeExpression(ExpressionNode::hook_, var, MakeExpression(ExpressionNode::comma_, left, right));
                            v = MakeExpression(ExpressionNode::comma_, asn, v);
                            *exp = MakeExpression(ExpressionNode::lvalue_, v);
                        }
                        return true;
                    }
                }
            }
        }
        // pointer to member derived to pointer to member base
        if ((*newType)->BaseType()->type == BasicType::memberptr_ && oldType->BaseType()->type == BasicType::memberptr_)
        {
            if (classRefCount((*newType)->BaseType()->sp, oldType->BaseType()->sp) == 1)
            {
                if (!checkconst || (*newType)->BaseType()->btp->IsConst() || !oldType->BaseType()->btp->IsConst())
                {
                    int vbo = oldType->BaseType()->sp->sb->vbaseEntries != 0;
                    int vbn = (*newType)->BaseType()->sp->sb->vbaseEntries != 0;
                    // can't have just new one being virtual...
                    if ((vbo && vbn) || !vbn)
                    {
                        if (isAccessible(oldType->BaseType()->sp, oldType->BaseType()->sp, (*newType)->BaseType()->sp, funcsp,
                            AccessLevel::public_, false))
                            return true;
                    }
                }
            }
        }
        // class to anything via conversion func
        if (oldType->IsStructured())
        {
            bool rv = cppCast(oldType, newType, exp);
            if (rv)
                return true;
        }
        // class via constructor
        if ((*newType)->IsStructured())
        {
            Type* ctype = *newType;
            if (oldType->IsStructured() && orig->IsRef())
            {
                if (classRefCount((*newType)->BaseType()->sp, oldType->BaseType()->sp) == 1)
                {
                    EXPRESSION* exp1 = MakeIntExpression(ExpressionNode::c_i_, 0);
                    exp1 = baseClassOffset((*newType)->BaseType()->sp, oldType->BaseType()->sp, exp1);
                    if (exp1->type == ExpressionNode::c_i_)
                    {
                        if (exp1->v.c->i != 0)
                            *exp = MakeExpression(ExpressionNode::add_, *exp, exp1);
                        return true;
                    }
                }
            }
            if (callConstructorParam(&ctype, exp, oldType, *exp, true, true, false, false, false))
            {
                EXPRESSION* exp2 = AnonymousVar(StorageClass::auto_, *newType);
                Type* type2 = Type::MakeType(BasicType::pointer_, *newType);
                if (oldType->IsConst())
                {
                    type2 = Type::MakeType(BasicType::const_, type2);
                }
                if (oldType->IsVolatile())
                {
                    type2 = Type::MakeType(BasicType::volatile_, type2);
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
    bool doConstCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp)
    {
        (void)exp;
        (void)funcsp;
        Type* orig = *newType;
        bool rref = false;
        if ((*newType)->IsRef())
        {
            if ((*newType)->BaseType()->type == BasicType::rref_)
                rref = true;
            while ((*newType)->IsRef())
                *newType = (*newType)->BaseType()->btp;
        }
        if (!(*newType)->SameExceptionType(oldType))
            return false;
        // as long as the types match except for any changes to const and
        // it is not a function or memberptr we can convert it.
        if ((*newType)->CompatibleTypeNoQualifiers(oldType))  // 2comparetypes
        {
            if (!oldType->IsFunction() && oldType->BaseType()->type != BasicType::memberptr_)
                return true;
        }
        *newType = orig;
        return false;
    }
    bool doReinterpretCast(Type** newType, Type* oldType, EXPRESSION** exp, SYMBOL* funcsp, bool checkconst)
    {
        (void)funcsp;
        // int to pointer
        if ((*newType)->IsPtr() && oldType->IsInt())
        {
            cast(*newType, exp);
            return true;
        }
        // to pointer
        if ((*newType)->IsPtr() && oldType->BaseType()->type == BasicType::enum_)
        {
            cast(*newType, exp);
            return true;
        }
        // pointer to bool
        if ((*newType)->BaseType()->type == BasicType::bool_ && oldType->BaseType()->type == BasicType::pointer_)
        {
            cast(*newType, exp);
            return true;
        }
        // change type of nullptr
        if ((*exp)->type == ExpressionNode::nullptr_)
        {
            return true;
        }
        // pointer to int
        if (oldType->IsPtr() && (*newType)->IsInt())
        {
            cast(*newType, exp);
            return true;
        }
        // function to int
        if (oldType->IsFunction() && (*newType)->IsInt())
        {
            cast(*newType, exp);
            return true;
        }
        if (oldType->type == BasicType::aggregate_)
        {
            // function to int
            if ((*newType)->IsInt() || (*newType)->IsPtr())
            {
                if ((*exp)->type == ExpressionNode::callsite_)
                {
                    LookupSingleAggregate(oldType, exp);
                    cast(*newType, exp);
                    return true;
                }
            }
        }
        // one function pointer type to another
        if (oldType->IsFunctionPtr() && (*newType)->IsFunctionPtr())
        {
            if (!checkconst || (*newType)->BaseType()->btp->IsConst() || !oldType->BaseType()->btp->IsConst())
            {
                return true;
            }
        }
        // pointer to object to pointer to object (standard layout types, with ok alignment)
        if ((*newType)->IsPtr() && oldType->IsPtr())
        {
            Type* tpo = oldType->BaseType()->btp;
            Type* tpn = (*newType)->BaseType()->btp;
            if (!checkconst || tpn->IsConst() || !tpo->IsConst())
            {
                return true;
            }
        }
        // convert one member pointer to another
        if ((*newType)->BaseType()->type == BasicType::memberptr_)
        {
            if ((*exp)->type == ExpressionNode::nullptr_)  // catch nullptr...
            {
                return true;
            }
            else if (oldType->BaseType()->type == BasicType::memberptr_)
            {
                // this suffices to check that both are vars or both are functions in this implementation
                if ((*newType)->size == oldType->size)
                    if (!checkconst || (*newType)->BaseType()->btp->IsConst() || !oldType->BaseType()->btp->IsConst())
                        return true;
            }
        }
        // conversion to reference
        // the deal is, if T1 is a reference and a pointer to T2 can be cast to a pointer to T1
        // using this methodology, we change the type of the value without any kind of cast operation
        // occurring...
        if ((*newType)->IsRef())
        {
            Type* nt2 = (*newType)->BaseType()->btp;
            Type* tpo = oldType;
            Type* tpn = nt2;
            if (!tpn->IsFunctionPtr() && !tpo->IsFunctionPtr())
            {
                if (!checkconst || tpn->IsConst() || !tpo->IsConst())
                {
                    if (tpn->IsArray())
                    {
                        //conversion to reference to array....
                        EXPRESSION* exp1 = *exp;
                        while (IsCastValue(exp1)) exp1 = exp1->left;
                        if (IsLValue(exp1))
                        {
                            *exp = exp1->left;
                        }
                    }
                    return true;
                }
            }
        }
        return false;
        // fixme nullptr conversion to nullptr
    }
    void GetCastInfo(SYMBOL* funcsp, Type** newType, Type** oldType, EXPRESSION** oldExp, bool packed)
    {
        getsym();
        *oldExp = nullptr;
        *oldType = nullptr;
        if (needkw(Keyword::lt_))
        {
            noTypeNameError++;
            *newType = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
            noTypeNameError--;
            if (!*newType)
            {
                error(ERR_TYPE_NAME_EXPECTED);
                *newType = &stdpointer;
            }
            (*newType)->InstantiateDeferred();
            *newType = ResolveTemplateSelectors(funcsp, *newType);
            if (needkw(Keyword::gt_))
            {
                if (needkw(Keyword::openpa_))
                {
                    expression(funcsp, nullptr, oldType, oldExp, packed ? _F_PACKABLE : 0);
                    if (!needkw(Keyword::closepa_))
                    {
                        errskim(skim_closepa);
                    }
                    if ((*oldType)->type == BasicType::aggregate_)
                    {
                        Type* tp = *newType;
                        if (tp->IsRef())
                            tp = tp->BaseType()->btp;
                        if (tp->IsFunctionPtr())
                        {
                            tp = tp->BaseType()->btp;
                            SYMBOL s = *tp->BaseType()->sp;
                            s.tp = tp;
                            auto sym = searchOverloads(&s, (*oldType)->syms);
                            if (sym)
                            {
                                *oldType = Type::MakeType(BasicType::pointer_, sym->tp);
                                (*oldType)->UpdateRootTypes();
                                (*oldExp)->v.func->sp = sym;
                                (*oldExp)->v.func->functp = sym->tp;
                                (*oldExp)->v.func->fcall = MakeExpression(ExpressionNode::pc_, sym);
                            }
                        }
                    }
                    (*oldType)->InstantiateDeferred();
                }
                else
                {
                    errskim(skim_closepa);
                }
            }
        }
        if (!*oldExp)
            *oldExp = MakeIntExpression(ExpressionNode::c_i_, 0);
        if (!*oldType)
            *oldType = &stdvoid;
        return;
    }

}  // namespace Parser