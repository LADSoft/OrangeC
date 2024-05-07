/* Software License Agreement
 *
 *    Copyright(C) 1994-2023 David Lindauer, (LADSoft)
 *
 *    This file is part of the Orange C Compiler package.
 *
 *    The Orange C Compiler package is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    The Orange C Compiler package is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    contact information:
 *        email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "compiler.h"
#include "rtti.h"
#include "PreProcessor.h"
#include "ccerr.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "ildata.h"
#include "mangle.h"
#include "declcpp.h"
#include "lambda.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "declare.h"
#include "lex.h"
#include "help.h"
#include "expr.h"
#include "cpplookup.h"
#include "occparse.h"
#include "memory.h"
#include "init.h"
#include "exprcpp.h"
#include "OptUtils.h"
#include "constopt.h"
#include "libcxx.h"
#include "beinterf.h"
#include "types.h"
#include "declcons.h"
#include "Property.h"
#include "browse.h"
#include "ildata.h"
#include "optmodules.h"
#include "config.h"
#include "constexpr.h"
#ifndef ORANGE_NO_INASM
#include "AsmLexer.h"
#endif
#include "symtab.h"
#include "ListFactory.h"
#include "inline.h"
#include "expreval.h"

// there is a bug where the compiler needs constant values for the memory order,
// but parsed code may not provide it directly.
// e.g. when an atomic primitive is called from inside a function.
//
// we probably have to force inlining a little better to get around it, but would still need
// some kind of error in the back Keyword::end_ if they do try to pass a non-constant value for a memory order.
//
// It isn't critical on the x86 so I'm putting in a temporary workaround for expediency
// this will have to be revisited when we do other architectures though.
//
#define NEED_CONST_MO

namespace Parser
{
typedef bool(*EvalFunc)(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, Type* lefttp,
                       EXPRESSION* leftexp, Type* righttp, EXPRESSION* rightexp, bool ismutable, int flags);

static std::unordered_map<Keyword, EvalFunc> dispatcher = {
    {Keyword::dotstar_, eval_binary_pm},
    {Keyword::pointstar_, eval_binary_pm},
    {Keyword::star_, eval_binary_times},
    {Keyword::divide_, eval_binary_times},
    {Keyword::mod_, eval_binary_times},
    {Keyword::plus_, eval_binary_add},
    {Keyword::minus_, eval_binary_add},
    {Keyword::leftshift_, eval_binary_shift},
    {Keyword::rightshift_, eval_binary_shift},
    {Keyword::gt_, eval_binary_inequality}, 
    {Keyword::geq_, eval_binary_inequality}, 
    {Keyword::lt_, eval_binary_inequality},
    {Keyword::leq_, eval_binary_inequality},
    {Keyword::eq_, eval_binary_equality},
    {Keyword::neq_, eval_binary_equality},
    {Keyword::and_, eval_binary_logical},
    {Keyword::or_, eval_binary_logical},
    {Keyword::uparrow_, eval_binary_logical},
    {Keyword::land_, eval_binary_logical},
    {Keyword::lor_, eval_binary_logical},
    {Keyword::assign_, eval_binary_assign},
    {Keyword::asplus_, eval_binary_assign},
    {Keyword::asminus_, eval_binary_assign},
    {Keyword::astimes_, eval_binary_assign},
    {Keyword::asdivide_, eval_binary_assign},
    {Keyword::asmod_, eval_binary_assign},
    {Keyword::asleftshift_, eval_binary_assign},
    {Keyword::asrightshift_, eval_binary_assign},
    {Keyword::asand_, eval_binary_assign},
    {Keyword::asor_, eval_binary_assign},
    {Keyword::asxor_, eval_binary_assign},
    {Keyword::comma_, eval_binary_comma},
};
bool isstructuredmath(Type* righttp, Type* tp2)
{
    if (Optimizer::cparams.prm_cplusplus)
    {
        if (righttp->IsStructured() || righttp->BaseType()->type == BasicType::enum_ || (righttp->IsInt() && righttp->scoped))
        {
            return true;
        }
        if (tp2 && (tp2->IsStructured() || tp2->BaseType()->type == BasicType::enum_ || (tp2->IsInt() && tp2->scoped)))
        {
            return true;
        }
    }
    return false;
}
bool smallint(Type * tp)
{
    tp = tp->BaseType();
    if (tp->type < BasicType::int_)
    {
        return true;
    }
    else if (tp->IsBitInt())
    {
        int sz = getSize(BasicType::int_) * CHAR_BIT;
        return tp->bitintbits < sz;
    }
    else
    {
        return false;
    }
}
bool largenum(Type * tp)
{
    tp = tp->BaseType();
    if (tp->type > BasicType::int_)
    {
        if (tp->IsBitInt())
        {
            int sz = getSize(BasicType::int_) * CHAR_BIT;
            return tp->bitintbits > sz || (tp->bitintbits == sz && tp->type == BasicType::unsigned_bitint_);
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}
bool isTemplatedPointer(Type * tp)
{
    Type* tpb = tp->BaseType()->btp;
    while (tp != tpb)
    {
        if (tp->templateTop)
            return true;
        tp = tp->btp;
    }
    return false;
}
EXPRESSION* nodeSizeof(Type *tp, EXPRESSION *exp, int flags)
{
    EXPRESSION* exp_in = exp;
    tp = PerformDeferredInitialization(tp->BaseType(), nullptr);
    if (tp->IsStructured())
        tp = tp->BaseType()->sp->tp;
    if (tp->IsRef())
        tp = tp->BaseType()->btp;
    tp = tp->BaseType();
    if (exp)
    {
        while (castvalue(exp))
            exp = exp->left;
        if (lvalue(exp) && exp->left->type == ExpressionNode::bits_)
            error(ERR_SIZEOF_BITFIELD);
    }
    if (Optimizer::cparams.prm_cplusplus && tp->type == BasicType::enum_ && !tp->fixed)
        error(ERR_SIZEOF_UNFIXED_ENUMERATION);
    if (tp->IsFunction())
        error(ERR_SIZEOF_NO_FUNCTION);
    if (Optimizer::cparams.prm_cplusplus && tp->size == 0 && !templateNestingCount)
        errortype(ERR_UNSIZED_TYPE, tp, tp); /* second will be ignored in this case */
    /* this tosses exp...  sizeof expressions don't get evaluated at run time */
    /* unless they are size of a vla... */
    if (tp->vla)
    {
        exp = tp->esize;
        tp = tp->BaseType()->btp;

        while (tp->type == BasicType::pointer_)
        {
            exp = exprNode(ExpressionNode::mul_, exp, tp->esize);
            tp = tp->BaseType()->btp;
        }
        exp = exprNode(ExpressionNode::mul_, exp, intNode(ExpressionNode::c_i_, tp->size));
    }
    else if (!tp->IsArray() && Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        exp = exprNode(ExpressionNode::sizeof_, typeNode(tp), nullptr);
    }
    else
    {
        exp = nullptr;
        if (tp->IsStructured())
        {
            if (tp->BaseType()->size == 0 && !templateNestingCount)
                errorsym(ERR_UNSIZED_TYPE, tp->BaseType()->sp);
            if (tp->BaseType()->syms)
            {
                SYMBOL* cache = nullptr;
                Type* tpx;
                for (auto sym : *tp->BaseType()->syms)
                {
                    if (ismemberdata(sym))
                        cache = sym;
                }
                if (cache)
                {
                    tpx = cache->tp->BaseType();
                    if (tpx->size == 0) /* if the last element of a structure is unsized */
                                        /* sizeof doesn't add the size of the padding element */
                        exp = intNode(ExpressionNode::c_i_, cache->sb->offset);
                }
            }
        }
    }
    if (!exp)
    {
        // array which is an argument has different sizeof requirements
        if ((flags & _F_SIZEOF) && tp->type == BasicType::pointer_ && tp->array && exp_in && exp_in->type == ExpressionNode::l_p_ &&
            exp_in->left->type == ExpressionNode::auto_ && exp_in->left->v.sp->sb->storage_class == StorageClass::parameter_)
            exp = intNode(ExpressionNode::c_i_, getSize(BasicType::pointer_));
        else
            exp = intNode(ExpressionNode::c_i_, tp->size);
    }
    return exp;
}

static void left_fold(LexList* lex, SYMBOL *funcsp, Type** resulttp, EXPRESSION** resultexp, LexList* start, Type* seedtp, EXPRESSION* seedexp, Type *foldtp, EXPRESSION* foldexp)
{
    auto it = dispatcher.find(lex->data->kw->key);
    if (it == dispatcher.end())
    {
        *resulttp = foldtp;
        *resultexp = intNode(ExpressionNode::c_i_, 0);
        return;
    }
    auto kw = it->first;
    bool first = true;
    *resulttp = seedtp;
    *resultexp = seedexp;
    std::list<Argument*>* lptr = nullptr;
    expandPackedInitList(&lptr, funcsp, start, foldexp);
    if (!lptr)
    {
        if (!*resulttp)
        {
            *resulttp = &stdint;
            *resultexp = intNode(ExpressionNode::c_i_, 0);
        }
        return;
    }
    for (auto e : *lptr)
    {
        if (!*resultexp)
        {
            *resulttp = e->tp;
            *resultexp = e->exp;
        }
        else
        {
            std::list<struct expr*>* logicaldestructorsleft = nullptr;
            std::list<struct expr*>* logicaldestructorsright = nullptr;
            if (kw == Keyword::land_ || kw == Keyword::lor_)
            {
                if (first)
                {
                    first = false;
                    GetLogicalDestructors(&logicaldestructorsleft, *resultexp);
                    if (Optimizer::cparams.prm_cplusplus && *resulttp && (*resulttp)->IsStructured())
                    {
                        if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdbool, false))
                            if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdint, false))
                                if (!castToPointer(resulttp, resultexp, (Keyword)-1, &stdpointer))
                                    errorConversionOrCast(true, *resulttp, &stdint);
                    }
                }
                GetLogicalDestructors(&logicaldestructorsright, e->exp);
                if (Optimizer::cparams.prm_cplusplus && e->tp && e->tp->IsStructured())
                {
                    if (!castToArithmeticInternal(false, &e->tp, &e->exp, (Keyword)-1, &stdbool, false))
                        if (!castToArithmeticInternal(false, &e->tp, &e->exp, (Keyword)-1, &stdint, false))
                            if (!castToPointer(&e->tp, &e->exp, (Keyword)-1, &stdpointer))
                                errorConversionOrCast(true, e->tp, &stdint);
                }
            }
            if (!it->second(lex, funcsp, nullptr, resulttp, resultexp, *resulttp, *resultexp, e->tp, e->exp, false, 0))
            {
                (*resultexp)->v.logicaldestructors.left = logicaldestructorsleft;
                (*resultexp)->v.logicaldestructors.right = logicaldestructorsright;
            }
        }
    }
}
static void right_fold(LexList* lex, SYMBOL *funcsp, Type** resulttp, EXPRESSION** resultexp, LexList* start, Type* seedtp, EXPRESSION* seedexp, Type *foldtp, EXPRESSION* foldexp)
{
    auto it = dispatcher.find(lex->data->kw->key);
    if (it == dispatcher.end())
    {
        *resulttp = foldtp;
        *resultexp = intNode(ExpressionNode::c_i_, 0);
        return;
    }
    *resulttp = seedtp;
    *resultexp = seedexp;
    std::list<Argument*>* lptr = nullptr;
    expandPackedInitList(&lptr, funcsp, start, foldexp);
    if (!lptr)
    {
        if (!*resulttp)
        {
            *resulttp = &stdint;
            *resultexp = intNode(ExpressionNode::c_i_, 0);
        }
        return;
    }
    bool first = true;
    auto kw = it->first;
    for (auto itr = lptr->rbegin(); itr != lptr->rend(); ++itr)
    {
        auto e = *itr;
        if (!*resultexp)
        {
            *resulttp = e->tp;
            *resultexp = e->exp;
        }
        else
        {
            std::list<struct expr*>* logicaldestructorsleft = nullptr;
            std::list<struct expr*>* logicaldestructorsright = nullptr;
            if (kw == Keyword::land_ || kw == Keyword::lor_)
            {
                if (first)
                {
                    first = false;
                    GetLogicalDestructors(&logicaldestructorsleft, *resultexp);
                    if (Optimizer::cparams.prm_cplusplus && *resulttp && (*resulttp)->IsStructured())
                    {
                        if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdbool, false))
                            if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdint, false))
                                if (!castToPointer(resulttp, resultexp, (Keyword)-1, &stdpointer))
                                    errorConversionOrCast(true, *resulttp, &stdint);
                    }
                }
                GetLogicalDestructors(&logicaldestructorsright, e->exp);
                if (Optimizer::cparams.prm_cplusplus && e->tp && e->tp->IsStructured())
                {
                    if (!castToArithmeticInternal(false, &e->tp, &e->exp, (Keyword)-1, &stdbool, false))
                        if (!castToArithmeticInternal(false, &e->tp, &e->exp, (Keyword)-1, &stdint, false))
                            if (!castToPointer(&e->tp, &e->exp, (Keyword)-1, &stdpointer))
                                errorConversionOrCast(true, e->tp, &stdint);
                }
            }
            if (!it->second(lex, funcsp, nullptr, resulttp, resultexp, e->tp, e->exp, *resulttp, *resultexp, false, 0))
            {
                (*resultexp)->v.logicaldestructors.left = logicaldestructorsleft;
                (*resultexp)->v.logicaldestructors.right = logicaldestructorsright;
            }
        }
    }
}


void eval_unary_left_fold(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexList* start,
    Type* lefttp, EXPRESSION* leftexp, bool ismutable, int flags)
{
    if (!hasPackedExpression(leftexp, true))
    {
        error(ERR_UNARY_LEFT_FOLDING_NEEDS_VARIADIC_TEMPLATE);
        *resulttp = lefttp;
        *resultexp = intNode(ExpressionNode::c_i_, 0);
    }
    else
    {
        left_fold(lex, funcsp, resulttp, resultexp, start, nullptr, nullptr, lefttp, leftexp);
    }
}
void eval_unary_right_fold(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexList* start,
    Type* lefttp, EXPRESSION* leftexp, bool ismutable, int flags)
{
    if (!hasPackedExpression(leftexp, true))
    {
        error(ERR_UNARY_RIGHT_FOLDING_NEEDS_VARIADIC_TEMPLATE);
        *resulttp = lefttp;
        *resultexp = intNode(ExpressionNode::c_i_, 0);
    }
    else
    {
        right_fold(lex, funcsp, resulttp, resultexp, start, nullptr, nullptr, lefttp, leftexp);
    }
}
void eval_binary_fold(LexList* lex, SYMBOL* funcsp, Type* atp, Type** resulttp, EXPRESSION** resultexp, LexList* leftstart,
    Type* lefttp, EXPRESSION* leftexp, LexList* rightstart, Type* righttp, EXPRESSION* rightexp, bool ismutable,
    int flags)
{
    if (!hasPackedExpression(leftexp, true))
    {
        // better be right fold...
        if (!hasPackedExpression(rightexp, true))
        {
            error(ERR_BINARY_FOLDING_NEEDS_VARIADIC_TEMPLATE);
            *resulttp = lefttp;
            *resultexp = intNode(ExpressionNode::c_i_, 0);
        }
        else
        {
            left_fold(lex, funcsp, resulttp, resultexp, leftstart, lefttp, leftexp, righttp, rightexp);
        }
    }
    else
    {
        if (hasPackedExpression(rightexp, true))
        {
            error(ERR_BINARY_FOLDING_TOO_MANY_VARIADIC_TEMPLATES);
            *resulttp = lefttp;
            *resultexp = intNode(ExpressionNode::c_i_, 0);
        }
        else
        {
            right_fold(lex, funcsp, resulttp, resultexp, rightstart, righttp, rightexp, lefttp, leftexp);
        }
    }
}

void eval_unary_plus(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp,
                    bool ismutable, int flags)
{
    *resulttp = lefttp;
    *resultexp = leftexp;
    if (isstructuredmath(*resulttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_unary_numeric, Keyword::unary_plus_, funcsp, resulttp, resultexp, nullptr, nullptr, nullptr, flags))
        {
            return;
        }
        else
        {
            LookupSingleAggregate(*resulttp, resultexp);
            castToArithmetic(false, resulttp, resultexp, KW(lex), nullptr, true);
        }
    }
    if ((*resulttp)->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsVoid() || (*resulttp)->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((*resulttp)->BaseType()->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if ((*resulttp)->IsPtr())
        error(ERR_ILL_POINTER_OPERATION);
    else if (atp && atp->BaseType()->type < BasicType::int_)
    {
        cast(atp, resultexp);
        *resulttp = atp;
    }
    else if (smallint(*resulttp))
    {
        cast(&stdint, resultexp);
        *resulttp = &stdint;
    }
}
void eval_unary_minus(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
{
    *resulttp = lefttp;
    *resultexp = leftexp;
    LookupSingleAggregate(*resulttp, resultexp);
    if (isstructuredmath(*resulttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_unary_numeric, Keyword::unary_minus_, funcsp, resulttp, resultexp, nullptr, nullptr, nullptr, flags))
        {
            return;
        }
        else
        {
            castToArithmetic(false, resulttp, resultexp, KW(lex), nullptr, true);
        }
    }
    if ((*resulttp)->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsVoid() || (*resulttp)->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((*resulttp)->BaseType()->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if ((*resulttp)->IsPtr())
        error(ERR_ILL_POINTER_OPERATION);
    else if (atp && atp->BaseType()->type < BasicType::int_)
    {
        *resultexp = exprNode(ExpressionNode::uminus_, *resultexp, nullptr);
        cast(atp, resultexp);
        *resulttp = atp;
    }
    else if (smallint(*resulttp))
    {
        cast(&stdint, resultexp);
        *resultexp = exprNode(ExpressionNode::uminus_, *resultexp, nullptr);
        *resulttp = &stdint;
    }
    else
    {
        *resultexp = exprNode(ExpressionNode::uminus_, *resultexp, nullptr);
    }
}
void eval_unary_not(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
{
    *resulttp = lefttp;
    *resultexp = leftexp;
    LookupSingleAggregate(*resulttp, resultexp);
    if (isstructuredmath(*resulttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_unary_numericptr, Keyword::not_, funcsp, resulttp, resultexp, nullptr, nullptr, nullptr, flags))
        {
            return;
        }
        else
        {
            castToArithmetic(false, resulttp, resultexp, KW(lex), nullptr, false);
        }
    }
    if ((*resulttp)->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsVoid() || (*resulttp)->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    if ((*resulttp)->type == BasicType::memberptr_)
    {
        *resultexp = exprNode(ExpressionNode::mp_as_bool_, *resultexp, nullptr);
        (*resultexp)->size = *resulttp;
        *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
    }
    else
    {
        *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
    }
    if (Optimizer::cparams.prm_cplusplus)
        *resulttp = &stdbool;
    else
        *resulttp = &stdint;
}
void eval_unary_complement(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
{
    *resulttp = lefttp;
    *resultexp = leftexp;
    LookupSingleAggregate(*resulttp, resultexp);
    if (isstructuredmath(*resulttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_unary_int, KW(lex), funcsp, resulttp, resultexp, nullptr, nullptr, nullptr, flags))
        {
            return;
        }
        else
        {
            castToArithmetic(true, resulttp, resultexp, KW(lex), nullptr, true);
        }
    }
    if ((*resulttp)->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsComplex())
        error(ERR_ILL_USE_OF_COMPLEX);
    else if ((*resulttp)->IsFloat() || (*resulttp)->IsImaginary())
        error(ERR_ILL_USE_OF_FLOATING);
    else if ((*resulttp)->IsPtr())
        error(ERR_ILL_POINTER_OPERATION);
    else if ((*resulttp)->IsVoid() || (*resulttp)->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((*resulttp)->BaseType()->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (atp && atp->BaseType()->type < BasicType::int_)
    {
        *resultexp = exprNode(ExpressionNode::compl_, *resultexp, nullptr);
        cast(atp, resultexp);
        *resulttp = atp;
    }
    else if (smallint(*resulttp))
    {
        cast(&stdint, resultexp);
        *resultexp = exprNode(ExpressionNode::compl_, *resultexp, nullptr);
        *resulttp = &stdint;
    }
    else
    {
        *resultexp = exprNode(ExpressionNode::compl_, *resultexp, nullptr);
        if (!(*resulttp)->IsBitInt())
            cast((*resulttp)->BaseType(), resultexp);
    }
}
void eval_unary_autoincdec(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
{
    *resulttp = lefttp;
    *resultexp = leftexp;
    if (isstructuredmath(*resulttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_unary_prefix, KW(lex), funcsp, resulttp, resultexp, nullptr, nullptr, nullptr, flags))
        {
            return;
        }
        else
        {
            castToArithmetic(false, resulttp, resultexp, KW(lex), nullptr, true);
        }
    }
    if ((*resulttp)->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsComplex())
        error(ERR_ILL_USE_OF_COMPLEX);
    else if ((*resulttp)->IsConst() && !ismutable)
        error(ERR_CANNOT_MODIFY_CONST_OBJECT);
    else if ((*resulttp)->IsVoid() || (*resulttp)->type == BasicType::aggregate_ || (*resulttp)->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((*resulttp)->BaseType()->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (!lvalue(*resultexp))
    {
        if ((*resultexp)->type != ExpressionNode::templateparam_ && (*resulttp)->BaseType()->type != BasicType::templateparam_ &&
            (*resulttp)->BaseType()->type != BasicType::templateselector_ && (*resulttp)->BaseType()->type != BasicType::templatedecltype_)
            error(ERR_LVALUE);
    }
    else
    {
        Keyword kw = KW(lex);
        EXPRESSION *exp3 = nullptr;
        if ((*resultexp)->left->type == ExpressionNode::func_ || (*resultexp)->left->type == ExpressionNode::thisref_)
        {
            EXPRESSION *exp2 = anonymousVar(StorageClass::auto_, *resulttp);
            deref(&stdpointer, &exp2);
            exp3 = exprNode(ExpressionNode::assign_, exp2, (*resultexp)->left);
            deref(*resulttp, &exp2);
            *resultexp = exp2;
        }
        if ((*resulttp)->IsPtr())
        {
            Type *tpx;
            if ((*resulttp)->BaseType()->btp->BaseType()->type == BasicType::void_)
            {
                if (Optimizer::cparams.prm_cplusplus)
                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                tpx = &stdchar;
            }
            else
            {
                tpx = (*resulttp)->BaseType()->btp;
            }
            *resultexp = exprNode(
                ExpressionNode::assign_, *resultexp,
                exprNode(kw == Keyword::autoinc_ ? ExpressionNode::add_ : ExpressionNode::sub_, *resultexp, nodeSizeof(tpx, *resultexp)));
            (*resultexp)->preincdec = true;
        }
        else if (kw == Keyword::autoinc_ && (*resulttp)->BaseType()->type == BasicType::bool_)
        {
            *resultexp = exprNode(ExpressionNode::assign_, *resultexp, intNode(ExpressionNode::c_i_, 1));  // set to true as per C++
        }
        else if (kw == Keyword::autodec_ && (*resulttp)->BaseType()->type == BasicType::bool_)
        {
            if (Optimizer::cparams.prm_cplusplus)
                error(ERR_CANNOT_USE_bool_HERE);

            *resultexp = exprNode(ExpressionNode::assign_, *resultexp,
                            intNode(ExpressionNode::c_i_, 0));  // c++ doesn't allow it, set it to true for C.
        }
        else
        {
            EXPRESSION *dest = *resultexp, *rightexp = intNode(ExpressionNode::c_i_, 1);
            *resultexp = RemoveAutoIncDec(*resultexp);
            cast(*resulttp, &rightexp);
            *resultexp = exprNode(ExpressionNode::assign_, dest,
                            exprNode(kw == Keyword::autoinc_ ? ExpressionNode::add_ : ExpressionNode::sub_, *resultexp, rightexp));
            (*resultexp)->preincdec = true;
        }
        if (exp3)
            *resultexp = exprNode(ExpressionNode::comma_, exp3, *resultexp);
    }
}
bool eval_binary_pm(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp,
                    EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    if (isstructuredmath(*resulttp, righttp) && Optimizer::cparams.prm_cplusplus && kw == Keyword::pointstar_ &&
        insertOperatorFunc(ovcl_binary_any, Keyword::pointstar_, funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
    {
        return true;
    }
    bool points = false;
    if (kw == Keyword::pointstar_)
    {
        points = true;
        if ((*resulttp)->IsPtr())
        {
            *resulttp = (*resulttp)->BaseType();
            *resulttp = (*resulttp)->btp;
            if (!(*resulttp)->IsStructured())
            {
                errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->data->kw->name);
            }
        }
        else
            errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->data->kw->name);
    }
    else if (!(*resulttp)->IsStructured())
    {
        errorstr(ERR_STRUCTURED_TYPE_EXPECTED, lex->data->kw->name);
    }
    if (righttp->IsFunction() && (*resulttp)->IsStructured())
    {
        rightexp = getMemberPtr(righttp->BaseType()->sp, (*resulttp)->BaseType()->sp, &righttp, funcsp);
    }
    if (righttp->BaseType()->type != BasicType::memberptr_)
    {
        error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
    }
    else
    {
        if ((*resulttp)->IsStructured() && righttp->BaseType()->type == BasicType::memberptr_)
        {
            if ((*resulttp)->BaseType()->sp != righttp->BaseType()->sp && (*resulttp)->BaseType()->sp->sb->mainsym != righttp->BaseType()->sp &&
                (*resulttp)->BaseType()->sp != righttp->BaseType()->sp->sb->mainsym)
            {
                if (classRefCount(righttp->BaseType()->sp, (*resulttp)->sp) != 1)
                {
                    errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, righttp->BaseType()->sp, (*resulttp)->sp);
                }
                else
                {
                    *resultexp = baseClassOffset(righttp->BaseType()->sp, (*resulttp)->sp, *resultexp);
                }
            }
            if (rightexp->type == ExpressionNode::memberptr_)
            {
                int lab = dumpMemberPtr(rightexp->v.sp, *resulttp, true);
                rightexp = intNode(ExpressionNode::labcon_, lab);
            }
            if (righttp->BaseType()->btp->IsFunction())
            {
                CallSite *funcparams = Allocate<CallSite>();
                if (((*resulttp)->BaseType())->sp->sb->vbaseEntries)
                {
                    EXPRESSION *ec =
                        exprNode(ExpressionNode::add_, rightexp,
                                 intNode(ExpressionNode::c_i_, getSize(BasicType::pointer_) + getSize(BasicType::int_)));
                    EXPRESSION *ec1;
                    deref(&stdint, &ec);
                    ec1 = exprNode(ExpressionNode::sub_, ec, intNode(ExpressionNode::c_i_, 1));
                    deref(&stdint, &ec1);
                    ec =
                        exprNode(ExpressionNode::hook_, ec, exprNode(ExpressionNode::comma_, ec1, intNode(ExpressionNode::c_i_, 0)));
                    *resultexp = exprNode(ExpressionNode::add_, *resultexp, ec);
                }
                else
                {
                    EXPRESSION *ec =
                        exprNode(ExpressionNode::add_, rightexp, intNode(ExpressionNode::c_i_, getSize(BasicType::pointer_)));
                    deref(&stdpointer, &ec);
                    *resultexp = exprNode(ExpressionNode::add_, *resultexp, ec);
                }
                funcparams->sp = righttp->btp->sp;
                funcparams->fcall = rightexp;
                deref(&stdpointer, &funcparams->fcall);
                funcparams->thisptr = *resultexp;
                funcparams->thistp = Type::MakeType(BasicType::pointer_, *resulttp);
                *resultexp = varNode(ExpressionNode::func_, nullptr);
                (*resultexp)->v.func = funcparams;
                *resulttp = righttp->BaseType();
            }
            else
            {
                if ((*resulttp)->sp->sb->vbaseEntries)
                {
                    EXPRESSION *ec = exprNode(ExpressionNode::add_, rightexp, intNode(ExpressionNode::c_i_, getSize(BasicType::int_)));
                    EXPRESSION *ec1;
                    deref(&stdint, &ec);
                    ec1 = exprNode(ExpressionNode::add_, ec, intNode(ExpressionNode::c_i_, -1));
                    ec1 = exprNode(ExpressionNode::add_, *resultexp, ec1);
                    deref(&stdint, &ec1);
                    *resultexp = exprNode(ExpressionNode::hook_, ec, exprNode(ExpressionNode::comma_, ec1, *resultexp));
                }
                deref(&stdint, &rightexp);
                *resultexp = exprNode(ExpressionNode::add_, *resultexp, rightexp);
                *resultexp = exprNode(ExpressionNode::add_, *resultexp, intNode(ExpressionNode::c_i_, -1));
                if (!righttp->BaseType()->btp->IsStructured())
                {
                    deref(righttp->BaseType()->btp, resultexp);
                }
                *resulttp = righttp->BaseType()->btp;
            }
        }
    }
    return false;
}

bool eval_binary_times(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp,
                    EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    ResolveTemplateVariable(resulttp, resultexp, righttp, atp);
    ResolveTemplateVariable(&righttp, &rightexp, *resulttp, atp);
    if (isstructuredmath(*resulttp, righttp))
    {

        if (Optimizer::cparams.prm_cplusplus && insertOperatorFunc(kw == Keyword::mod_ ? ovcl_binary_int : ovcl_binary_numeric, kw,
                                                                   funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
        {
            return true;
        }
        else
        {
            castToArithmetic(kw == Keyword::mod_, resulttp, resultexp, kw, righttp, true);
            castToArithmetic(kw == Keyword::mod_, &righttp, &rightexp, (Keyword)-1, *resulttp, true);
            LookupSingleAggregate(*resulttp, resultexp);
            LookupSingleAggregate(righttp, &rightexp);
        }
    }
    if ((*resulttp)->IsStructured() || righttp->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsVoid() || righttp->IsVoid() || (*resulttp)->IsMsil() || righttp->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (((*resulttp)->BaseType()->scoped || righttp->BaseType()->scoped) && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if ((*resulttp)->IsPtr() || righttp->IsPtr())
        error(ERR_ILL_POINTER_OPERATION);
    else
    {
        int m1 = -1, m2 = -1;
        if ((*resulttp)->IsImaginary() && righttp->IsImaginary())
        {
            if (kw == Keyword::star_)
                *resultexp = exprNode(ExpressionNode::uminus_, *resultexp, nullptr);
            m1 = (int)(*resulttp)->type - (int)BasicType::float__imaginary_;
            m2 = (int)(int)righttp->type - (int)BasicType::float__imaginary_;
            m1 = m1 > m2 ? m1 : m2;
            switch (m1)
            {
                case 0:
                    *resulttp = &stdfloat;
                    righttp = &stdfloatimaginary;
                    break;
                case 1:
                    *resulttp = &stddouble;
                    righttp = &stddoubleimaginary;
                    break;
                case 2:
                    *resulttp = &stdlongdouble;
                    righttp = &stdlongdoubleimaginary;
                    break;
            }
            cast(righttp, resultexp);
            cast(righttp, &rightexp);
        }
        else
        {
            if ((*resulttp)->IsImaginary() && (righttp->IsFloat() || righttp->IsInt()))
            {
                m1 = (int)(*resulttp)->type - (int)BasicType::float__imaginary_;
                m2 = righttp->IsFloat() ? (int)(int)righttp->type - (int)BasicType::float_ : m1;
            }
            else if (((*resulttp)->IsFloat() || (*resulttp)->IsInt()) && righttp->IsImaginary())
            {
                m1 = (int)(int)righttp->type - (int)BasicType::float__imaginary_;
                m2 = (*resulttp)->IsFloat() ? (int)(*resulttp)->type - (int)BasicType::float_ : m1;
            }
            if (m1 >= 0)
            {
                bool b = (*resulttp)->IsImaginary();
                m1 = m1 > m2 ? m1 : m2;
                switch (m1)
                {
                    case 0:
                        *resulttp = &stdfloatimaginary;
                        righttp = &stdfloat;
                        break;
                    case 1:
                        *resulttp = &stddoubleimaginary;
                        righttp = &stddouble;
                        break;
                    default:
                        *resulttp = &stdlongdoubleimaginary;
                        righttp = &stdlongdouble;
                        break;
                }
                if (b)
                {
                    cast(*resulttp, resultexp);
                    cast(righttp, &rightexp);
                }
                else
                {
                    cast(righttp, resultexp);
                    cast(*resulttp, &rightexp);
                }
            }
            else
            {
                *resulttp = destSize(*resulttp, righttp, resultexp, &rightexp, false, nullptr);
            }
        }
        ExpressionNode type = ExpressionNode::mul_;
        switch (kw)
        {
            case Keyword::star_:
                type = ExpressionNode::mul_;
                break;
            case Keyword::divide_:
                type = (*resulttp)->IsUnsigned() ? ExpressionNode::udiv_ : ExpressionNode::div_;
                break;
            case Keyword::mod_:
                type = (*resulttp)->IsUnsigned() ? ExpressionNode::umod_ : ExpressionNode::mod_;
                break;
            default:
                break;
        }
        *resultexp = exprNode(type, *resultexp, rightexp);
    }
    return false;
}
bool eval_binary_add(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp,
    EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    ResolveTemplateVariable(resulttp, resultexp, righttp, atp);
    ResolveTemplateVariable(&righttp, &rightexp, *resulttp, atp);
    if (isstructuredmath(*resulttp, righttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_binary_numericptr, kw, funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
        {
            return true;
        }
        else
        {
            if (Optimizer::cparams.prm_cplusplus && *resulttp && (*resulttp)->IsStructured())
            {
                if (!castToArithmeticInternal(false, resulttp, resultexp, kw, righttp->IsStructured() ? &stdint : righttp, false))
                    castToPointer(resulttp, resultexp, kw, &stdpointer);
            }
            if (Optimizer::cparams.prm_cplusplus && righttp && righttp->IsStructured())
            {
                if (!castToArithmeticInternal(false, &righttp, &rightexp, (Keyword)-1, (*resulttp)->IsStructured() || (*resulttp)->IsPtr() ? &stdint : *resulttp, false))
                    castToPointer(&righttp, &rightexp, kw, &stdpointer);
            }
            LookupSingleAggregate(*resulttp, resultexp);
            LookupSingleAggregate(righttp, &rightexp);
        }
    }
    bool msil = false;
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && kw == Keyword::plus_ &&
        ((*resulttp)->BaseType()->type == BasicType::string_ || righttp->BaseType()->type == BasicType::string_ ||
         (atp && atp->BaseType()->type == BasicType::string_)))
    {
        msil = true;
        if ((*resultexp)->type == ExpressionNode::labcon_ && (*resultexp)->string)
            (*resultexp)->type = ExpressionNode::c_string_;
        else if (!(*resulttp)->IsMsil())
            *resultexp = exprNode(ExpressionNode::x_object_, *resultexp, nullptr);
        if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
            rightexp->type = ExpressionNode::c_string_;
        else if (!righttp->IsMsil())
            rightexp = exprNode(ExpressionNode::x_object_, rightexp, nullptr);
    }
    else if (kw == Keyword::plus_ && (*resulttp)->IsPtr() && righttp->IsPtr())
        error(ERR_ILL_POINTER_ADDITION);
    else if ((*resulttp)->IsVoid() || righttp->IsVoid() || (*resulttp)->IsMsil() || righttp->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (((*resulttp)->BaseType()->scoped || righttp->BaseType()->scoped) && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (kw != Keyword::plus_ && !(*resulttp)->IsPtr() && righttp->IsPtr())
        error(ERR_ILL_POINTER_SUBTRACTION);
    else if ((*resulttp)->IsStructured() || righttp->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsPtr())
    {
        if ((*resulttp)->IsArray() && (*resulttp)->msil)
            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
        else if (righttp->IsPtr() && !(*resulttp)->ExactSameType(righttp) && !righttp->ExactSameType(*resulttp))
            error(ERR_NONPORTABLE_POINTER_CONVERSION);
        else if (righttp->IsComplex())
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (righttp->IsFloat() || righttp->IsImaginary())
            error(ERR_ILL_USE_OF_FLOATING);
    }
    else if (righttp->IsPtr())
    {
        if ((*resulttp)->IsComplex())
            error(ERR_ILL_USE_OF_COMPLEX);
        else if ((*resulttp)->IsFloat() || (*resulttp)->IsImaginary())
            error(ERR_ILL_USE_OF_FLOATING);
        else if (righttp->IsArray() && (righttp)->msil)
            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
    }
    if (msil)
    {
        // MSIL back Keyword::end_ will take care of figuring out what function to call
        // to perform the concatenation
        *resultexp = exprNode(ExpressionNode::add_, *resultexp, rightexp);
        *resulttp = &std__string;
    }
    else if ((*resulttp)->IsPtr())
    {
        EXPRESSION *ns;
        if ((*resulttp)->BaseType()->btp->BaseType()->type == BasicType::void_)
        {
            if (Optimizer::cparams.prm_cplusplus)
                error(ERR_ARITHMETIC_WITH_VOID_STAR);
            ns = nodeSizeof(&stdchar, *resultexp);
        }
        else
        {
            ns = nodeSizeof((*resulttp)->BaseType()->btp, *resultexp);
        }
        if (righttp->IsPtr())
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                cast(&stdinative, resultexp);
                cast(&stdinative, &rightexp);
            }
            *resultexp = exprNode(ExpressionNode::sub_, *resultexp, rightexp);
            *resultexp = exprNode(ExpressionNode::arraydiv_, *resultexp, ns);
            *resulttp = &stdint; /* ptrdiff_t */
        }
        else
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                cast(&stdinative, resultexp);
            }
            /*				*resulttp = righttp = destSize(*resulttp, righttp, resultexp, &rightexp, false, nullptr); */
            if (smallint(righttp))
                cast(&stdint, &rightexp);
            rightexp = exprNode(ExpressionNode::umul_, rightexp, ns);
            *resultexp = exprNode(kw == Keyword::plus_ ? ExpressionNode::add_ : ExpressionNode::sub_, *resultexp, rightexp);
        }
    }
    else if (righttp->IsPtr())
    {
        EXPRESSION *ns;
        if (righttp->BaseType()->btp->BaseType()->type == BasicType::void_)
        {
            if (Optimizer::cparams.prm_cplusplus)
                error(ERR_ARITHMETIC_WITH_VOID_STAR);
            ns = nodeSizeof(&stdchar, *resultexp);
        }
        else
        {
            ns = nodeSizeof(righttp->BaseType()->btp, *resultexp);
        }
        /*			*resulttp = righttp = destSize(*resulttp, righttp, resultexp, &rightexp, false, nullptr); */
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            cast(&stdinative, &rightexp);
        }
        if (smallint(*resulttp))
            cast(&stdint, resultexp);
        *resultexp = exprNode(ExpressionNode::umul_, *resultexp, ns);
        *resultexp = exprNode(ExpressionNode::add_, *resultexp, rightexp);
        *resulttp = righttp;
    }
    else
    {
        int m1 = -1, m2 = -1;
        if ((*resulttp)->IsImaginary() && (righttp->IsFloat() || righttp->IsInt()))
        {
            m1 = (int)(*resulttp)->type - (int)BasicType::float__imaginary_;
            m2 = righttp->IsFloat() ? (int)(int)righttp->type - (int)BasicType::float_ : m1;
        }
        else if (((*resulttp)->IsFloat() || (*resulttp)->IsInt()) && righttp->IsImaginary())
        {
            m1 = (int)(int)righttp->type - (int)BasicType::float__imaginary_;
            m2 = (*resulttp)->IsFloat() ? (int)(*resulttp)->type - (int)BasicType::float_ : m1;
        }
        if (m1 >= 0)
        {
            m1 = m1 > m2 ? m1 : m2;
            Type *tpa, *tpb;
            switch (m1)
            {
                case 0:
                    *resulttp = &stdfloatcomplex;
                    tpa = &stdfloat;
                    tpb = &stdfloatimaginary;
                    break;
                case 1:
                    *resulttp = &stddoublecomplex;
                    tpa = &stddouble;
                    tpb = &stddoubleimaginary;
                    break;
                default:
                    *resulttp = &stdlongdoublecomplex;
                    tpa = &stdlongdouble;
                    tpb = &stdlongdoubleimaginary;
                    break;
            }
            if (righttp->IsImaginary())
            {
                cast(tpa, resultexp);
                cast(tpb, &rightexp);
            }
            else
            {
                cast(tpb, resultexp);
                cast(tpa, &rightexp);
            }
            if (kw != Keyword::plus_)
                rightexp = exprNode(ExpressionNode::uminus_, rightexp, nullptr);
            *resultexp = exprNode(ExpressionNode::add_, *resultexp, rightexp);
            cast(*resulttp, resultexp);
        }
        else
        {
            *resulttp = destSize(*resulttp, righttp, resultexp, &rightexp, true, atp);
            *resultexp = exprNode(kw == Keyword::plus_ ? ExpressionNode::add_ : ExpressionNode::sub_, *resultexp, rightexp);
        }
    }
    return false;
}
bool eval_binary_shift(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp, Type *righttp,
    EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    ResolveTemplateVariable(resulttp, resultexp, righttp, atp);
    ResolveTemplateVariable(&righttp, &rightexp, *resulttp, atp);
    if (isstructuredmath(*resulttp, righttp))
    {
        LookupSingleAggregate(*resulttp, resultexp);
        LookupSingleAggregate(righttp, &rightexp);
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_binary_int, kw, funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
        {
            return true;
        }
        else
        {
            castToArithmetic(true, resulttp, resultexp, kw, righttp, true);
            castToArithmetic(true, &righttp, &rightexp, (Keyword)-1, *resulttp, true);
        }
    }
    if ((*resulttp)->IsStructured() || righttp->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsVoid() || righttp->IsVoid() || (*resulttp)->IsMsil() || righttp->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (((*resulttp)->BaseType()->scoped || righttp->BaseType()->scoped) && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if ((*resulttp)->IsPtr() || righttp->IsPtr())
        error(ERR_ILL_POINTER_OPERATION);
    else if ((*resulttp)->IsFloat() || righttp->IsFloat() || (*resulttp)->IsImaginary() || righttp->IsImaginary())
        error(ERR_ILL_USE_OF_FLOATING);
    else if ((*resulttp)->IsComplex() || righttp->IsComplex())
        error(ERR_ILL_USE_OF_COMPLEX);
    else
    {
        ExpressionNode type = ExpressionNode::lsh_;
        if (kw == Keyword::rightshift_)
            if ((*resulttp)->IsUnsigned())
                type = ExpressionNode::ursh_;
            else
                type = ExpressionNode::rsh_;
        else
            type = ExpressionNode::lsh_;
        if (smallint(*resulttp))
        {
            *resulttp = &stdint;
            cast(*resulttp, resultexp);
        }
        if (Optimizer::architecture == ARCHITECTURE_MSIL && righttp->size != stdint.size)
        {
            cast(&stdint, &rightexp);
        }
        else if (righttp->IsBitInt())
        {
            cast(&stdint, &rightexp);
        }
        *resultexp = exprNode(type, *resultexp, exprNode(ExpressionNode::shiftby_, rightexp, 0));
    }
    return false;
}
bool eval_binary_inequality(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp,
                       Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    auto opname = lex->data->kw->name;
    ResolveTemplateVariable(resulttp, resultexp, righttp, atp);
    ResolveTemplateVariable(&righttp, &rightexp, *resulttp, atp);
    if (isstructuredmath(*resulttp, righttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_binary_numericptr, kw, funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
        {
            return true;
        }
        else
        {
            checkscope(*resulttp, righttp);
            if (Optimizer::cparams.prm_cplusplus && *resulttp && (*resulttp)->IsStructured())
            {
                if (!castToArithmeticInternal(false, resulttp, resultexp, kw, righttp, true))
                    castToPointer(resulttp, resultexp, kw, righttp->IsPtr() ? righttp : &stdpointer);
            }
            if (Optimizer::cparams.prm_cplusplus && righttp && righttp->IsStructured())
            {
                if (!castToArithmeticInternal(false, &righttp, &rightexp, (Keyword)-1, *resulttp, true))
                    castToPointer(&righttp, &rightexp, (Keyword)-1, (*resulttp)->IsPtr() ? *resulttp : &stdpointer);
            }
        }
        LookupSingleAggregate(*resulttp, resultexp);
        LookupSingleAggregate(righttp, &rightexp);
    }

    if (Optimizer::cparams.prm_cplusplus)
    {
        SYMBOL *funcsp = nullptr;
        if (((*resulttp)->IsPtr() || (*resulttp)->BaseType()->type == BasicType::memberptr_) && righttp->type == BasicType::aggregate_)
        {
            if (righttp->syms->size() > 1)
                errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)righttp->syms->front())->name);
            rightexp = varNode(ExpressionNode::pc_, righttp->syms->front());
            righttp = ((SYMBOL*)righttp->syms->front())->tp;
        }
        else if ((righttp->IsPtr() || righttp->BaseType()->type == BasicType::memberptr_) && (*resulttp)->type == BasicType::aggregate_)
        {
            if ((*resulttp)->syms->size() > 1)
                errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)(*resulttp)->syms->front())->name);
            (*resultexp) = varNode(ExpressionNode::pc_, (*resulttp)->syms->front());
            (*resulttp) = ((SYMBOL*)(*resulttp)->syms->front())->tp;
        }
    }
    if ((*resultexp)->type == ExpressionNode::pc_ || ((*resultexp)->type == ExpressionNode::func_ && !(*resultexp)->v.func->ascall))
        thunkForImportTable(resultexp);
    if (rightexp->type == ExpressionNode::pc_ || (rightexp->type == ExpressionNode::func_ && !rightexp->v.func->ascall))
        thunkForImportTable(&rightexp);
    if ((*resulttp)->IsStructured() || righttp->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsVoid() || righttp->IsVoid() || (*resulttp)->IsMsil() || righttp->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((*resulttp)->IsPtr())
    {
        if (isintconst(rightexp))
        {
            if (!isconstzero(righttp, rightexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if (righttp->IsInt())
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if (righttp->IsFloat() || righttp->IsImaginary())
            error(ERR_ILL_USE_OF_FLOATING);
        else if (righttp->IsComplex())
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (righttp->IsPtr() && !(*resulttp)->IsVoidPtr() && !righttp->IsVoidPtr() && !(*resulttp)->SameType(righttp))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
    }
    else if (righttp->IsPtr())
    {
        if (isintconst(*resultexp))
        {
            if (!isconstzero(*resulttp, *resultexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if ((*resulttp)->IsInt())
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if ((*resulttp)->IsFloat() || (*resulttp)->IsImaginary())
            error(ERR_ILL_USE_OF_FLOATING);
        else if ((*resulttp)->IsComplex())
            error(ERR_ILL_USE_OF_COMPLEX);
    }
    else if ((*resulttp)->IsInt() && righttp->IsInt())
    {
        if (((*resulttp)->IsUnsigned() && !righttp->IsUnsigned()) || (righttp->IsUnsigned() && !(*resulttp)->IsUnsigned()))
            errorstr(ERR_SIGNED_UNSIGNED_MISMATCH_RELAT, opname);
    }
    *resulttp = destSize(*resulttp, righttp, resultexp, &rightexp, true, nullptr);
    ExpressionNode type = ExpressionNode::gt_;
    switch (kw)
    {
        case Keyword::gt_:
            type = (*resulttp)->IsUnsigned() ? ExpressionNode::ugt_ : ExpressionNode::gt_;
            break;
        case Keyword::geq_:
            type = (*resulttp)->IsUnsigned() ? ExpressionNode::uge_ : ExpressionNode::ge_;
            break;
        case Keyword::lt_:
            type = (*resulttp)->IsUnsigned() ? ExpressionNode::ult_ : ExpressionNode::lt_;
            break;
        case Keyword::leq_:
            type = (*resulttp)->IsUnsigned() ? ExpressionNode::ule_ : ExpressionNode::le_;
            break;
    }
    *resultexp = exprNode(type, *resultexp, rightexp);
    if (Optimizer::cparams.prm_cplusplus)
        *resulttp = &stdbool;
    else
        *resulttp = &stdint;
    return false;
}
bool eval_binary_equality(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp,
                            Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    ResolveTemplateVariable(resulttp, resultexp, righttp, atp);
    ResolveTemplateVariable(&righttp, &rightexp, *resulttp, atp);
    if (isstructuredmath(*resulttp, righttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_binary_numericptr, kw, funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
        {
            return true;
        }
        else
        {
            if (Optimizer::cparams.prm_cplusplus && *resulttp && (*resulttp)->IsStructured())
            {
                if (!castToArithmeticInternal(false, resulttp, resultexp, kw, righttp, true))
                    castToPointer(resulttp, resultexp, kw, righttp->IsPtr() ? righttp : &stdpointer);
            }
            if (Optimizer::cparams.prm_cplusplus && righttp && righttp->IsStructured())
            {
                if (!castToArithmeticInternal(false, &righttp, &rightexp, (Keyword)-1, *resulttp, true))
                    castToPointer(&righttp, &rightexp, (Keyword)-1, (*resulttp)->IsPtr() ? *resulttp : &stdpointer);
            }
        }
    }
    checkscope(*resulttp, righttp);
    if (Optimizer::cparams.prm_cplusplus)
    {
        SYMBOL *funcsp = nullptr;
        if (((*resulttp)->IsPtr() || (*resulttp)->BaseType()->type == BasicType::memberptr_) && righttp->type == BasicType::aggregate_)
        {
            if (righttp->syms->size() > 1)
                errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)righttp->syms->front())->name);
            rightexp = varNode(ExpressionNode::pc_, righttp->syms->front());
            righttp = ((SYMBOL*)righttp->syms->front())->tp;
        }
        else if ((righttp->IsPtr() || righttp->BaseType()->type == BasicType::memberptr_) && (*resulttp)->type == BasicType::aggregate_)
        {
            if (righttp->syms->size() > 1)
                errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)(*resulttp)->syms->front())->name);
            (*resultexp) = varNode(ExpressionNode::pc_, (*resulttp)->syms->front());
            (*resulttp) = ((SYMBOL*)(*resulttp)->syms->front())->tp;
        }
    }
    if ((*resultexp)->type == ExpressionNode::pc_ || ((*resultexp)->type == ExpressionNode::func_ && !(*resultexp)->v.func->ascall))
        thunkForImportTable(resultexp);
    if (rightexp->type == ExpressionNode::pc_ || (rightexp->type == ExpressionNode::func_ && !rightexp->v.func->ascall))
        thunkForImportTable(&rightexp);
    if ((*resulttp)->IsStructured() || righttp->IsStructured())
    {
        if ((Optimizer::architecture != ARCHITECTURE_MSIL) || (!isconstzero(*resulttp, *resultexp) && !isconstzero(righttp, rightexp)))
            error(ERR_ILL_STRUCTURE_OPERATION);
    }
    else if ((*resulttp)->IsVoid() || righttp->IsVoid() || (*resulttp)->IsMsil() || righttp->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    bool done = false;
    if ((*resulttp)->IsPtr())
    {
        if (isintconst(rightexp))
        {
            if (!isconstzero(righttp, rightexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if (righttp->IsInt())
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if (righttp->IsFloat() || righttp->IsImaginary())
            error(ERR_ILL_USE_OF_FLOATING);
        else if (righttp->IsComplex())
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (righttp->IsPtr() && !(*resulttp)->IsVoidPtr() && !righttp->IsVoidPtr() && !(*resulttp)->SameType(righttp))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
    }
    else if (righttp->IsPtr())
    {
        if (isintconst(*resultexp))
        {
            if (!isconstzero(*resulttp, *resultexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if ((*resulttp)->IsInt())
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if ((*resulttp)->IsFloat() || (*resulttp)->IsImaginary())
            error(ERR_ILL_USE_OF_FLOATING);
        else if ((*resulttp)->IsComplex())
            error(ERR_ILL_USE_OF_COMPLEX);
    }
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
    {
        if (righttp->BaseType()->type == BasicType::memberptr_)
        {
            if (!(*resulttp)->BaseType()->btp->ExactSameType(righttp->BaseType()->btp))
            {
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            }
            *resultexp = exprNode(ExpressionNode::mp_compare_, *resultexp, rightexp);
            (*resultexp)->size = *resulttp;
            if (kw == Keyword::neq_)
                *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
            done = true;
        }
        else if (isconstzero(righttp, rightexp))
        {
            *resultexp = exprNode(ExpressionNode::mp_as_bool_, *resultexp, nullptr);
            (*resultexp)->size = *resulttp;
            if (kw == Keyword::eq_)
                *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
            done = true;
        }
        else if ((*resulttp)->BaseType()->btp->ExactSameType(righttp))
        {
            int lbl = dumpMemberPtr(rightexp->v.sp, *resulttp, true);
            rightexp = intNode(ExpressionNode::labcon_, lbl);
            *resultexp = exprNode(ExpressionNode::mp_compare_, *resultexp, rightexp);
            (*resultexp)->size = *resulttp;
            if (kw == Keyword::neq_)
                *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
            done = true;
        }
        else
        {
            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
        }
    }
    else if (righttp->BaseType()->type == BasicType::memberptr_)
    {
        if (isconstzero(*resulttp, *resultexp))
        {
            *resultexp = exprNode(ExpressionNode::mp_as_bool_, rightexp, nullptr);
            (*resultexp)->size = righttp;
            if (kw == Keyword::eq_)
                *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
            done = true;
        }
        else if ((*resulttp)->ExactSameType(righttp->BaseType()->btp))
        {
            int lbl = dumpMemberPtr((*resultexp)->v.sp, righttp, true);
            (*resultexp) = intNode(ExpressionNode::labcon_, lbl);
            *resultexp = exprNode(ExpressionNode::mp_compare_, *resultexp, rightexp);
            (*resultexp)->size = righttp;
            if (kw == Keyword::neq_)
                *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
            done = true;
        }
        else
        {
            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
        }
    }
    if (!done)
    {
        if (!(Optimizer::chosenAssembler->arch->preferopts & OPT_BYTECOMPARE) ||
            (!fittedConst(*resulttp, *resultexp) && !fittedConst(righttp, rightexp)) || !(*resulttp)->IsInt() || !righttp->IsInt())
            if (!(*resulttp)->IsStructured() && !righttp->IsStructured())
                destSize(*resulttp, righttp, resultexp, &rightexp, true, nullptr);
        *resultexp = exprNode(kw == Keyword::eq_ ? ExpressionNode::eq_ : ExpressionNode::ne_, *resultexp, rightexp);
    }
    if (Optimizer::cparams.prm_cplusplus)
        *resulttp = &stdbool;
    else
        *resulttp = &stdint;
    return false;
}
bool eval_binary_logical(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp,
                          Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    auto type = ExpressionNode::or_;
    switch (kw)
    {
        case Keyword::or_:
            type = ExpressionNode::or_;
            break;
        case Keyword::and_:
            type = ExpressionNode::and_;
            break;
        case Keyword::uparrow_:
            type = ExpressionNode::xor_;
            break;
        case Keyword::land_:
            type = ExpressionNode::land_;
            break;
        case Keyword::lor_:
            type = ExpressionNode::lor_;
            break;
    }
    *resulttp = lefttp;
    *resultexp = leftexp;
    if (isstructuredmath(*resulttp, righttp))
    {
        if (Optimizer::cparams.prm_cplusplus &&
            insertOperatorFunc(kw == Keyword::lor_ || kw == Keyword::land_ ? ovcl_binary_numericptr : ovcl_binary_int, kw, funcsp,
                               resulttp, resultexp, righttp, rightexp, nullptr, flags))
        {
            return true;
        }
        if (kw == Keyword::land_ || kw == Keyword::lor_)
        {
            castToArithmetic(kw != Keyword::land_ && kw != Keyword::lor_, resulttp, resultexp, kw, &stdbool,
                             kw != Keyword::land_ && kw != Keyword::lor_);
            castToArithmetic(kw != Keyword::land_ && kw != Keyword::lor_, &righttp, &rightexp, (Keyword)-1, &stdbool,
                             kw != Keyword::land_ && kw != Keyword::lor_);
        }
        else
        {
            castToArithmetic(kw != Keyword::land_ && kw != Keyword::lor_, resulttp, resultexp, kw, righttp,
                             kw != Keyword::land_ && kw != Keyword::lor_);
            castToArithmetic(kw != Keyword::land_ && kw != Keyword::lor_, &righttp, &rightexp, (Keyword)-1, *resulttp,
                             kw != Keyword::land_ && kw != Keyword::lor_);
        }
        LookupSingleAggregate(*resulttp, resultexp);
        LookupSingleAggregate(righttp, &rightexp);
    }
    if ((*resulttp)->IsStructured() || righttp->IsStructured())
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if ((*resulttp)->IsVoid() || righttp->IsVoid() || (*resulttp)->IsMsil() || righttp->IsMsil())
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (((*resulttp)->BaseType()->scoped || righttp->BaseType()->scoped) && !(flags & _F_SCOPEDENUM) &&
             ((Optimizer::architecture != ARCHITECTURE_MSIL) || Optimizer::cparams.msilAllowExtensions))
        error(ERR_SCOPED_TYPE_MISMATCH);
    if (kw != Keyword::lor_ && kw != Keyword::land_)
    {
        if ((*resulttp)->IsPtr() || righttp->IsPtr())
            error(ERR_ILL_POINTER_OPERATION);
        else if ((*resulttp)->IsFloat() || righttp->IsFloat() || (*resulttp)->IsImaginary() || righttp->IsImaginary())
            error(ERR_ILL_USE_OF_FLOATING);
        else if ((*resulttp)->IsComplex() || righttp->IsComplex())
            error(ERR_ILL_USE_OF_COMPLEX);
        else if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
        *resulttp = destSize(*resulttp, righttp, resultexp, &rightexp, true, atp);
    }
    else
    {
        if (Optimizer::cparams.prm_cplusplus)
            *resulttp = &stdbool;
        else
            *resulttp = &stdint;
    }
    if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
    {
        *resultexp = exprNode(ExpressionNode::mp_as_bool_, *resultexp, nullptr);
        (*resultexp)->size = *resulttp;
    }
    if (righttp->BaseType()->type == BasicType::memberptr_)
    {
        rightexp = exprNode(ExpressionNode::mp_as_bool_, rightexp, nullptr);
        rightexp->size = righttp;
    }
    *resultexp = exprNode(type, *resultexp, rightexp);
    return false;
}
bool eval_binary_assign(LexList *lex, SYMBOL *funcsp,Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp,
                          Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    auto selovcl = ovcl_assign_any;
    switch (kw)
    {
        case Keyword::asand_:
        case Keyword::asor_:
        case Keyword::asxor_:
        case Keyword::asmod_:
        case Keyword::asleftshift_:
        case Keyword::asrightshift_:
            selovcl = ovcl_assign_int;
            break;
        case Keyword::astimes_:
        case Keyword::asdivide_:
            selovcl = ovcl_assign_numeric;
            break;
        case Keyword::assign_:
            selovcl = ovcl_assign_any;
            break;
        case Keyword::asplus_:
        case Keyword::asminus_:
            selovcl = ovcl_assign_numericptr;
            break;
    }
    ResolveTemplateVariable(resulttp, resultexp, righttp, nullptr);
    ResolveTemplateVariable(&righttp, &rightexp, *resulttp, nullptr);
    if (isstructuredmath(*resulttp, righttp))
    {
        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(selovcl, kw, funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
        {
            // unallocated var for destructor
            int offset;
            auto exp2 = relptr(rightexp, offset);
            if (exp2 && !inConstantExpression)
                exp2->v.sp->sb->ignoreconstructor = false;
            if (!inAssignRHS)
            {
                GetAssignDestructors(&(*resultexp)->v.func->destructors, *resultexp);
            }
            return true;
        }
    }
    if (kw == Keyword::assign_ && (flags & _F_SELECTOR))
    {
        /* a little naive... */
        error(ERR_POSSIBLE_INCORRECT_ASSIGNMENT);
        flags &= ~_F_SELECTOR;
    }
    checkscope(*resulttp, righttp);
    if (Optimizer::cparams.prm_cplusplus)
    {
        if ((*resulttp)->IsArithmetic())
        {
            castToArithmetic(false, &righttp, &rightexp, (Keyword)-1, *resulttp, true);
        }
        else if (righttp->IsStructured())
        {
            cppCast(*resulttp, &righttp, &rightexp);
        }
    }
    auto exp2 = &rightexp;
    while (castvalue(*exp2))
        exp2 = &(*exp2)->left;
    if (((*resulttp)->IsFunction() || (*resulttp)->IsFunctionPtr()) && (*exp2)->type == ExpressionNode::func_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_)
    {
        Type *tp2 = nullptr;
        SYMBOL *funcsp;
        if ((*exp2)->v.func->sp->sb->parentClass && !(*exp2)->v.func->asaddress)
            if ((*exp2)->v.func->sp->sb->storage_class == StorageClass::member_ ||
                (*exp2)->v.func->sp->sb->storage_class == StorageClass::mutable_)
                error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
        funcsp = MatchOverloadedFunction((*resulttp), (*resulttp)->IsFunctionPtr() || (*resulttp)->BaseType()->type == BasicType::memberptr_ ? &righttp : &tp2,
                                         (*exp2)->v.func->sp, exp2, flags);
        if (funcsp && (*resulttp)->BaseType()->type == BasicType::memberptr_)
        {
            int lbl = dumpMemberPtr(funcsp, *resulttp, true);
            rightexp = intNode(ExpressionNode::labcon_, lbl);
        }
        if (funcsp)
        {
            righttp = funcsp->tp;
            if (rightexp->type == ExpressionNode::pc_ || (rightexp->type == ExpressionNode::func_ && !rightexp->v.func->ascall))
            {
                thunkForImportTable(&rightexp);
            }
        }
        if ((*resulttp)->BaseType()->btp && !(*resulttp)->BaseType()->btp->ExactSameType(righttp))
        {
            if (!(*resulttp)->IsVoidPtr())
                errorConversionOrCast(true, righttp, *resulttp);
        }
    }
    if (!(*resulttp)->SameExceptionType(righttp))
    {
        errorConversionOrCast(true, righttp, *resulttp);
    }
    if (rightexp->type == ExpressionNode::pc_ || (rightexp->type == ExpressionNode::func_ && !rightexp->v.func->ascall))
    {
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            ValidateMSILFuncPtr(*resulttp, righttp, &rightexp);
        }
    }
    EXPRESSION *temp = GetSymRef(*resultexp);
    auto symRef = (Optimizer::architecture == ARCHITECTURE_MSIL) ? temp : nullptr;
    LookupSingleAggregate(righttp, &rightexp);
    if (righttp->IsStructured())
    {
        SYMBOL *conv = lookupNonspecificCast(righttp->BaseType()->sp, *resulttp);
        if (conv)
        {
            CallSite *params = Allocate<CallSite>();
            params->thisptr = rightexp;
            params->thistp = righttp;
            params->ascall = true;
            params->functp = conv->tp;
            params->fcall = varNode(ExpressionNode::pc_, conv);
            params->sp = conv;
            rightexp = exprNode(ExpressionNode::func_, nullptr, nullptr);
            rightexp->v.func = params;
            righttp = conv->tp->BaseType()->btp;
        }
    }
    if (((*resultexp)->type == ExpressionNode::const_ || (*resulttp)->IsConst()) && !ismutable &&
        (!temp || temp->v.sp->sb->storage_class != StorageClass::parameter_ || !(*resulttp)->IsArray()) &&
        ((*resultexp)->type != ExpressionNode::func_ || !(*resultexp)->v.func->sp->tp->BaseType()->btp->IsConst()))
        error(ERR_CANNOT_MODIFY_CONST_OBJECT);
    else if ((*resulttp)->IsVoid() || righttp->IsVoid() || (*resulttp)->type == BasicType::aggregate_)
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((!templateNestingCount || instantiatingTemplate) && !(*resulttp)->IsStructured() && /*((*resulttp)->btp && !(*resulttp)->btp->IsPtr()) &&*/ (!(*resulttp)->IsArray() || !(*resulttp)->BaseType()->msil) &&
             (*resulttp)->BaseType()->type != BasicType::memberptr_ && (*resulttp)->BaseType()->type != BasicType::templateparam_ &&
             (*resulttp)->BaseType()->type != BasicType::templateselector_ && !lvalue(*resultexp) &&
             (*resultexp)->type != ExpressionNode::msil_array_access_)
        error(ERR_LVALUE);
    else if (symRef && symRef->v.sp->sb->attribs.inheritable.linkage2 == Linkage::property_ &&
             !symRef->v.sp->sb->has_property_setter)
        errorsym(ERR_CANNOT_MODIFY_PROPERTY_WITHOUT_SETTER, symRef->v.sp);
    else if ((*resulttp)->IsStructured() && !(*resulttp)->BaseType()->sp->sb->trivialCons)
    {
        errorsym(ERR_NO_ASSIGNMENT_OPERATOR, (*resulttp)->BaseType()->sp);
    }
    else
        switch (kw)
        {
            case Keyword::asand_:
            case Keyword::asor_:
            case Keyword::asxor_:
            case Keyword::asleftshift_:
            case Keyword::asrightshift_:
                if ((*resulttp)->IsMsil() || righttp->IsMsil())
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if ((*resulttp)->IsComplex() || righttp->IsComplex())
                    error(ERR_ILL_USE_OF_COMPLEX);
                if ((*resulttp)->IsFloat() || righttp->IsFloat() || (*resulttp)->IsImaginary() || righttp->IsImaginary())
                    error(ERR_ILL_USE_OF_FLOATING);
                /* fall through */
            case Keyword::astimes_:
            case Keyword::asdivide_:
            case Keyword::asmod_:
                if ((*resulttp)->IsMsil() || righttp->IsMsil())
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if ((*resulttp)->IsPtr() || righttp->IsPtr())
                    error(ERR_ILL_POINTER_OPERATION);
                if ((*resulttp)->IsStructured() || righttp->IsStructured())
                    error(ERR_ILL_STRUCTURE_OPERATION);
                if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                break;
            case Keyword::asplus_:
                if ((*resulttp)->type == BasicType::string_)
                {
                    if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
                        rightexp->type = ExpressionNode::c_string_;
                    else if (!righttp->IsMsil())
                        rightexp = exprNode(ExpressionNode::x_object_, rightexp, nullptr);
                }
                else if ((*resulttp)->IsMsil() || righttp->IsMsil())
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if ((*resulttp)->IsPtr())
                {
                    if (righttp->IsPtr())
                        error(ERR_ILL_POINTER_ADDITION);
                    else
                    {
                        EXPRESSION *ns;
                        if (righttp->IsComplex())
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (righttp->IsFloat() || righttp->IsImaginary())
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (righttp->IsStructured())
                            error(ERR_ILL_STRUCTURE_OPERATION);
                        if ((*resulttp)->BaseType()->btp->BaseType()->type == BasicType::void_)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                error(ERR_ARITHMETIC_WITH_VOID_STAR);
                            cast(&stdint, &rightexp);
                            ns = nodeSizeof(&stdchar, rightexp);
                        }
                        else
                        {
                            cast(&stdint, &rightexp);
                            ns = nodeSizeof((*resulttp)->BaseType()->btp, rightexp);
                        }
                        rightexp = exprNode(ExpressionNode::umul_, rightexp, ns);
                    }
                }
                else if (righttp->IsPtr())
                {
                    error(ERR_ILL_POINTER_OPERATION);
                }
                else if ((*resulttp)->IsStructured() || righttp->IsStructured())
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                break;
            case Keyword::asminus_:
                if ((*resulttp)->IsMsil() || righttp->IsMsil())
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if ((*resulttp)->IsStructured() || righttp->IsStructured())
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (righttp->IsPtr())
                {
                    error(ERR_ILL_POINTER_SUBTRACTION);
                }
                else if ((*resulttp)->BaseType()->type == BasicType::memberptr_ || righttp->BaseType()->type == BasicType::memberptr_)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if ((*resulttp)->IsPtr())
                {
                    EXPRESSION *ns;
                    if ((*resulttp)->BaseType()->btp->BaseType()->type == BasicType::void_)
                    {
                        if (Optimizer::cparams.prm_cplusplus)
                            error(ERR_ARITHMETIC_WITH_VOID_STAR);
                        cast(&stdint, &rightexp);
                        ns = nodeSizeof(&stdchar, rightexp);
                    }
                    else
                    {
                        cast(&stdint, &rightexp);
                        ns = nodeSizeof((*resulttp)->BaseType()->btp, rightexp);
                    }
                    rightexp = exprNode(ExpressionNode::umul_, rightexp, ns);
                }
                break;
            case Keyword::assign_:
                if ((*resulttp)->BaseType()->type == BasicType::string_)
                {
                    if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
                        rightexp->type = ExpressionNode::c_string_;
                }
                else if ((*resulttp)->BaseType()->type == BasicType::object_)
                {
                    if (righttp->type != BasicType::object_ && !righttp->IsStructured() && (!righttp->IsArray() || !righttp->BaseType()->msil))
                        rightexp = exprNode(ExpressionNode::x_object_, rightexp, nullptr);
                }
                else if ((*resulttp)->IsMsil() || righttp->IsMsil())
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if ((*resulttp)->IsPtr())
                {
                    if ((*resulttp)->IsArray() && (*resulttp)->msil && !(*resulttp)->ExactSameType(righttp) && natural_size(*resultexp) != ISZ_OBJECT)
                    {
                        *resultexp = exprNode(ExpressionNode::l_object_, *resultexp, nullptr);
                        (*resultexp)->v.tp = righttp;
                    }
                    if (righttp->IsArithmetic())
                    {
                        if (righttp->IsComplex())
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (righttp->IsFloat() || righttp->IsImaginary())
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (isarithmeticconst(rightexp))
                        {
                            if (!isintconst(rightexp) || !isconstzero(*resulttp, rightexp))
                                error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                        else if (Optimizer::cparams.prm_cplusplus)
                        {
                            errorConversionOrCast(true, righttp, *resulttp);
                        }
                        else
                        {
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                    }
                    else if (righttp->BaseType()->IsPtr() || righttp->type == BasicType::any_)
                    {
                        if (Optimizer::cparams.prm_cplusplus && righttp->BaseType()->stringconst && !(*resulttp)->BaseType()->btp->IsConst())
                            error(ERR_INVALID_CHARACTER_STRING_CONVERSION);

                        while (righttp->type == BasicType::any_ && righttp->btp)
                            righttp = righttp->btp;
                        if (!righttp->BaseType()->IsPtr())
                            goto end;
                        if (!(*resulttp)->ExactSameType(righttp))
                        {
                            bool found = false;
                            if (righttp->IsPtr())
                            {
                                Type *tpo = righttp->BaseType()->btp->BaseType();
                                Type *tpn = (*resulttp)->BaseType()->btp->BaseType();
                                if (tpo->IsStructured() && tpn->IsStructured())
                                {
                                    if (classRefCount(tpo->sp, tpn->sp) == 1)
                                    {
                                        EXPRESSION *v = Allocate<EXPRESSION>();
                                        v->type = ExpressionNode::c_i_;
                                        v = baseClassOffset(tpo->sp, tpn->sp, v);
                                        optimize_for_constants(&v);
                                        if (v->type == ExpressionNode::c_i_)  // check for no virtual base
                                        {
                                            if (isAccessible(tpn->sp, tpn->sp, tpo->sp, funcsp, AccessLevel::public_, false))
                                            {
                                                rightexp = exprNode(ExpressionNode::sub_, rightexp, v);
                                                found = true;
                                            }
                                        }
                                    }
                                    else if (classRefCount(tpn->sp, tpo->sp) == 1)
                                    {
                                        EXPRESSION *v = Allocate<EXPRESSION>();
                                        v->type = ExpressionNode::c_i_;
                                        v = baseClassOffset(tpn->sp, tpo->sp, v);
                                        optimize_for_constants(&v);
                                        if (v->type == ExpressionNode::c_i_)  // check for no virtual base
                                        {
                                            if (isAccessible(tpo->sp, tpo->sp, tpn->sp, funcsp, AccessLevel::public_, false))
                                            {
                                                rightexp = exprNode(ExpressionNode::add_, rightexp, v);
                                                found = true;
                                            }
                                        }
                                    }
                                }
                            }
                            if (!found)
                            {
                                if (Optimizer::cparams.prm_cplusplus)
                                {
                                    if ((!(*resulttp)->IsVoidPtr() || !righttp->IsPtr()) && !righttp->nullptrType)
                                        if (!isTemplatedPointer(*resulttp))
                                        {
                                            errorConversionOrCast(true, righttp, *resulttp);
                                        }
                                }
                                else if (!(*resulttp)->IsVoidPtr() && !righttp->IsVoidPtr())
                                {
                                    if (!(*resulttp)->SameCharType(righttp))
                                        error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                                }
                                else if (Optimizer::cparams.prm_cplusplus && !(*resulttp)->IsVoidPtr() && righttp->IsVoidPtr() &&
                                         rightexp->type != ExpressionNode::nullptr_)
                                {
                                    error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                                }
                            }
                        }
                    }
                    else if (righttp->IsFunction())
                    {
                        if (!(*resulttp)->IsVoidPtr() && (!(*resulttp)->BaseType()->btp->IsFunction() ||
                                                !(*resulttp)->BaseType()->btp->BaseType()->btp->ExactSameType(righttp->BaseType()->btp)))
                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                    }
                    else
                    {
                        if (righttp->type == BasicType::memberptr_)
                        {
                            errorConversionOrCast(true, righttp, *resulttp);
                        }
                        else
                            error(ERR_INVALID_POINTER_CONVERSION);
                    }
                end:;
                }
                else if (righttp->IsPtr())
                {
                    if ((*resulttp)->IsComplex())
                    {
                        error(ERR_ILL_USE_OF_COMPLEX);
                    }
                    else if ((*resulttp)->IsFloat() || (*resulttp)->IsImaginary())
                    {
                        error(ERR_ILL_USE_OF_FLOATING);
                    }
                    else if ((*resulttp)->IsInt())
                    {
                        if ((*resulttp)->BaseType()->type != BasicType::bool_)
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    }
                    else if (righttp->IsArray() && (righttp)->msil)
                    {
                        error(ERR_MANAGED_OBJECT_NO_ADDRESS);
                    }
                }
                if ((*resulttp)->IsStructured() && (!righttp->IsStructured() || (!(*resulttp)->ExactSameType(righttp) && !sameTemplate(righttp, *resulttp))))
                {
                    if (!((Optimizer::architecture == ARCHITECTURE_MSIL) && (*resulttp)->BaseType()->sp->sb->msil &&
                          (isconstzero(righttp, rightexp) || righttp->BaseType()->nullptrType)))
                    {
                        error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                    }
                }
                else if ((*resulttp)->IsStructured() && !(*resulttp)->size)
                {
                    if (!(flags & _F_SIZEOF))
                        errorsym(ERR_STRUCT_NOT_DEFINED, (*resulttp)->BaseType()->sp);
                }
                else if (!(*resulttp)->IsStructured() && righttp->IsStructured())
                {
                    error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                }
                else if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
                {
                    if (rightexp->type == ExpressionNode::memberptr_)
                    {
                        if (rightexp->v.sp != (*resulttp)->BaseType()->sp && rightexp->v.sp != (*resulttp)->BaseType()->sp->sb->mainsym &&
                            !sameTemplate(rightexp->v.sp->tp, (*resulttp)->BaseType()->sp->tp))  // DAL FIXED
                            errorConversionOrCast(true, righttp, *resulttp);
                    }
                    else if ((!(*resulttp)->BaseType()->btp->IsFunction() || !(*resulttp)->BaseType()->btp->ExactSameType(righttp)) &&
                             !isconstzero(righttp, *resultexp) && !(*resulttp)->ExactSameType(righttp))
                    {
                        errorConversionOrCast(true, righttp, *resulttp);
                    }
                }
                else if (righttp->BaseType()->type == BasicType::memberptr_)
                    errorConversionOrCast(true, righttp, *resulttp);
                break;
        }
    ExpressionNode op = ExpressionNode::or_;
    Type* tp2;
    switch (kw)
    {
        case Keyword::asand_:
            op = ExpressionNode::and_;
            break;
        case Keyword::asor_:
            op = ExpressionNode::or_;
            break;
        case Keyword::asxor_:
            op = ExpressionNode::xor_;
            break;

        case Keyword::astimes_:
            op = ExpressionNode::mul_;
            break;
        case Keyword::asdivide_:
            tp2 = destSize(*resulttp, righttp, nullptr, nullptr, false, nullptr);
            op = (*resulttp)->IsUnsigned() && largenum(*resulttp) ? ExpressionNode::udiv_ : ExpressionNode::div_;
            break;
        case Keyword::asmod_:
            tp2 = destSize(*resulttp, righttp, nullptr, nullptr, false, nullptr);
            op = (*resulttp)->IsUnsigned() && largenum(*resulttp) ? ExpressionNode::umod_ : ExpressionNode::mod_;
            break;
        case Keyword::assign_:
            op = ExpressionNode::assign_;
            break;
        case Keyword::asplus_:
            op = ExpressionNode::add_;
            break;
        case Keyword::asminus_:
            op = ExpressionNode::sub_;
            break;

        case Keyword::asleftshift_:
            op = ExpressionNode::lsh_;
            break;
        case Keyword::asrightshift_:
            tp2 = destSize(*resulttp, righttp, nullptr, nullptr, false, nullptr);
            op = (*resulttp)->IsUnsigned() ? ExpressionNode::ursh_ : ExpressionNode::rsh_;
            break;
        default:
            break;
    }
    if ((*resulttp)->IsMsil())
    {
        if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
            rightexp->type = ExpressionNode::c_string_;
        else if (!righttp->IsMsil() && !righttp->IsStructured() && (!righttp->IsArray() || !righttp->BaseType()->msil))
            rightexp = exprNode(ExpressionNode::x_object_, rightexp, nullptr);
        if (op == ExpressionNode::assign_)
        {
            *resultexp = exprNode(op, *resultexp, rightexp);
        }
        else
        {
            EXPRESSION *dest = *resultexp;
            *resultexp = exprNode(op, *resultexp, rightexp);
            *resultexp = exprNode(ExpressionNode::assign_, dest, *resultexp);
        }
    }
    else if ((*resulttp)->BaseType()->type == BasicType::memberptr_)
    {
        if ((*resultexp)->type == ExpressionNode::not__lvalue_ || ((*resultexp)->type == ExpressionNode::func_ && !(*resultexp)->v.func->ascall) ||
            (*resultexp)->type == ExpressionNode::comma_ || (*resultexp)->type == ExpressionNode::memberptr_)
        {
            if ((*resulttp)->BaseType()->type != BasicType::templateparam_)
                error(ERR_LVALUE);
        }
        else if (rightexp->type == ExpressionNode::memberptr_)
        {
            int lbl = dumpMemberPtr(rightexp->v.sp, *resulttp, true);
            rightexp = intNode(ExpressionNode::labcon_, lbl);
            *resultexp = exprNode(ExpressionNode::blockassign_, *resultexp, rightexp);
            (*resultexp)->size = *resulttp;
            (*resultexp)->altdata = (void*)(*resulttp);
        }
        else if (isconstzero(righttp, rightexp) || rightexp->type == ExpressionNode::nullptr_)
        {
            *resultexp = exprNode(ExpressionNode::blockclear_, *resultexp, nullptr);
            (*resultexp)->size = *resulttp;
        }
        else if (rightexp->type == ExpressionNode::func_ && rightexp->v.func->returnSP)
        {
            rightexp->v.func->returnSP->sb->allocate = false;
            rightexp->v.func->returnEXP = *resultexp;
            *resultexp = rightexp;
        }
        else
        {
            *resultexp = exprNode(ExpressionNode::blockassign_, *resultexp, rightexp);
            (*resultexp)->size = *resulttp;
            (*resultexp)->altdata = (void*)(*resulttp);
        }
    }
    else if ((*resulttp)->IsStructured())
    {
        EXPRESSION *exp2 = rightexp;
        if (((*resultexp)->type == ExpressionNode::not__lvalue_ ||
             ((*resultexp)->type == ExpressionNode::func_ && (!(*resultexp)->v.func->ascall || (*resultexp)->v.func->returnSP)) ||
             ((*resultexp)->type == ExpressionNode::comma_) && !(flags & _F_SIZEOF)))
            error(ERR_LVALUE);
        if (lvalue(exp2))
            exp2 = exp2->left;
        if (exp2->type == ExpressionNode::func_ && exp2->v.func->returnSP)
        {
            exp2->v.func->returnSP->sb->allocate = false;
            exp2->v.func->returnEXP = *resultexp;
            *resultexp = rightexp;
        }
        else
        {
            *resultexp = exprNode(ExpressionNode::blockassign_, *resultexp, rightexp);
            (*resultexp)->size = *resulttp;
            (*resultexp)->altdata = (void*)(*resulttp);
            if ((*resulttp)->IsAtomic())
            {
                (*resultexp)->size = (*resultexp)->size->CopyType();
                (*resultexp)->size->size -= ATOMIC_FLAG_SPACE;
            }
        }
        *resultexp = exprNode(ExpressionNode::not__lvalue_, *resultexp, nullptr);
    }
    else
    {
        if (kw == Keyword::assign_)
        {
            if ((*resultexp)->type != ExpressionNode::msil_array_access_ && rightexp->type != ExpressionNode::msil_array_access_)
            {
                if ((*resulttp)->IsArithmetic() || righttp->IsArithmetic() ||
                    (((*resulttp)->IsPtr() && (!(*resulttp)->IsArray() || !(*resulttp)->BaseType()->msil)) ||
                     (righttp->IsPtr() && (!righttp->IsArray() || !righttp->BaseType()->msil))))
                {
                    int n = natural_size(*resultexp);
                    if (natural_size(rightexp) != n)
                        cast((*resulttp), &rightexp);
                    else if (abs(n) == ISZ_BITINT && !(*resulttp)->SameType(righttp))
                        cast((*resulttp), &rightexp);
                }
                else if ((*resulttp)->IsArray() && (*resulttp)->BaseType()->msil)
                {
                    *resultexp = exprNode(ExpressionNode::l_object_, *resultexp, nullptr);
                    (*resultexp)->v.tp = *resulttp;
                }
            }

            *resultexp = exprNode(op, *resultexp, rightexp);
            // unallocated var for destructor
            GetAssignDestructors(&(*resultexp)->v.logicaldestructors.left, *resultexp);
        }
        else
        {
            EXPRESSION *src, *dest;
            dest = EvaluateDest(*resultexp, *resulttp);
            if (dest)
            {
                src = dest->left;
                dest = dest->right;
            }
            else
            {
                dest = RemoveAutoIncDec(*resultexp);
                src = *resultexp;
            }
            // we want to optimize the as *operations for the backend
            // but can't do the optimization for divisions
            // otherwise it is fine for the processor we are on
            if ((*resulttp)->IsBitInt())
            {
                if (kw == Keyword::asleftshift_ || kw == Keyword::asrightshift_)
                {
                    cast(&stdint, &rightexp);
                    *resultexp = exprNode(op, src, rightexp);
                }
                else
                {
                    auto tp2 = destSize(*resulttp, righttp, &src, &rightexp, false, nullptr);
                    *resultexp = exprNode(op, src, rightexp);
                    if (!tp2->SameType(*resulttp))
                        cast(*resulttp, resultexp);
                }
                *resultexp = exprNode(ExpressionNode::assign_, dest, *resultexp);
            }
            else if (kw == Keyword::asmod_ || kw == Keyword::asdivide_ || (*resulttp)->BaseType()->type == BasicType::bool_)
            {
                int n = natural_size(*resultexp);
                destSize(*resulttp, righttp, &src, &rightexp, false, nullptr);
                *resultexp = exprNode(op, src, rightexp);
                if (natural_size(rightexp) != n)
                    cast((*resulttp), resultexp);
                *resultexp = exprNode(ExpressionNode::assign_, dest, *resultexp);
            }
            else if (kw == Keyword::asleftshift_ || kw == Keyword::asrightshift_)
            {
                int n = natural_size(*resultexp);
                if (natural_size(rightexp) != n)
                    cast(&stdint, &rightexp);
                *resultexp = exprNode(op, src, rightexp);
                *resultexp = exprNode(ExpressionNode::assign_, dest, *resultexp);
            }
            else
            {
                int n = natural_size(*resultexp);
                if (natural_size(rightexp) != n)
                    destSize(*resulttp, righttp, &src, &rightexp, false, nullptr);
                *resultexp = exprNode(op, src, rightexp);
                if (natural_size(*resultexp) != n)
                    cast(*resulttp, resultexp);
                *resultexp = exprNode(ExpressionNode::assign_, dest, *resultexp);
            }
        }
    }
    /* now if there is a cast on the LHS move it to the RHS */
    EXPRESSION *expl = (*resultexp)->left;
    if (castvalue(expl))
    {
        auto exp2 = expl;
        auto exp3 = expl;
        while (castvalue(exp2))
        {
            /* cast on the lhs isn't sign-extended */
            switch (exp2->type)
            {
                case ExpressionNode::x_c_:
                    exp2->type = ExpressionNode::x_uc_;
                    break;
                case ExpressionNode::x_s_:
                    exp2->type = ExpressionNode::x_us_;
                    break;
                case ExpressionNode::x_i_:
                    exp2->type = ExpressionNode::x_ui_;
                    break;
                case ExpressionNode::x_l_:
                    exp2->type = ExpressionNode::x_ul_;
                    break;
                case ExpressionNode::x_ll_:
                    exp2->type = ExpressionNode::x_ull_;
                    break;
                case ExpressionNode::x_bitint_:
                    exp2->type = ExpressionNode::x_ubitint_;
                    break;
            }
            if (castvalue(exp2->left))
                exp3 = exp2->left;
            exp2 = exp2->left;
        }
        (*resultexp)->left = exp2;
        exp3->left = (*resultexp)->right;
        (*resultexp)->right = exp3;
    }
    if ((*resultexp)->type == ExpressionNode::pc_ || ((*resultexp)->type == ExpressionNode::func_ && !(*resultexp)->v.func->ascall))
        thunkForImportTable(resultexp);
    return false;
}
bool eval_binary_comma(LexList *lex, SYMBOL *funcsp, Type *atp, Type **resulttp, EXPRESSION **resultexp, Type *lefttp, EXPRESSION *leftexp,
                       Type *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
{
    auto kw = KW(lex);
    *resulttp = lefttp;
    *resultexp = leftexp;
    if ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) &&
        insertOperatorFunc(ovcl_comma, Keyword::comma_, funcsp, resulttp, resultexp, righttp, rightexp, nullptr, flags))
    {
        return true;
    }
    else
    {
        *resulttp = righttp;
        *resultexp = exprNode(ExpressionNode::comma_, *resultexp, rightexp);
        resultexp = &(*resultexp)->right;
    }
    return false;
}

}  // namespace Parser
