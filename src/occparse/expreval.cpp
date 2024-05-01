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
#include "help.h"
#include "expr.h"
#include "cpplookup.h"
#include "occparse.h"
#include "lex.h"
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
#include "AsmLexer.h"
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
typedef bool(*EvalFunc)(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** resulttp, EXPRESSION** resultexp, TYPE* lefttp,
                       EXPRESSION* leftexp, TYPE* righttp, EXPRESSION* rightexp, bool ismutable, int flags);

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
bool isstructuredmath(TYPE* righttp, TYPE* tp2)
{
    if (Optimizer::cparams.prm_cplusplus)
    {
        if (isstructured(righttp) || basetype(righttp)->type == BasicType::enum_ || (isint(righttp) && righttp->scoped))
        {
            return true;
        }
        if (tp2 && (isstructured(tp2) || basetype(tp2)->type == BasicType::enum_ || (isint(tp2) && tp2->scoped)))
        {
            return true;
        }
    }
    return false;
}
bool smallint(TYPE * tp)
{
    tp = basetype(tp);
    if (tp->type < BasicType::int_)
    {
        return true;
    }
    else if (isbitint(tp))
    {
        int sz = getSize(BasicType::int_) * CHAR_BIT;
        return tp->bitintbits < sz;
    }
    else
    {
        return false;
    }
}
bool largenum(TYPE * tp)
{
    tp = basetype(tp);
    if (tp->type > BasicType::int_)
    {
        if (isbitint(tp))
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
bool isTemplatedPointer(TYPE * tp)
{
    TYPE* tpb = basetype(tp)->btp;
    while (tp != tpb)
    {
        if (tp->templateTop)
            return true;
        tp = tp->btp;
    }
    return false;
}
EXPRESSION* nodeSizeof(TYPE *tp, EXPRESSION *exp, int flags)
{
    EXPRESSION* exp_in = exp;
    tp = PerformDeferredInitialization(basetype(tp), nullptr);
    if (isstructured(tp))
        tp = basetype(tp)->sp->tp;
    if (isref(tp))
        tp = basetype(tp)->btp;
    tp = basetype(tp);
    if (exp)
    {
        while (castvalue(exp))
            exp = exp->left;
        if (lvalue(exp) && exp->left->type == ExpressionNode::bits_)
            error(ERR_SIZEOF_BITFIELD);
    }
    if (Optimizer::cparams.prm_cplusplus && tp->type == BasicType::enum_ && !tp->fixed)
        error(ERR_SIZEOF_UNFIXED_ENUMERATION);
    if (isfunction(tp))
        error(ERR_SIZEOF_NO_FUNCTION);
    if (Optimizer::cparams.prm_cplusplus && tp->size == 0 && !templateNestingCount)
        errortype(ERR_UNSIZED_TYPE, tp, tp); /* second will be ignored in this case */
    /* this tosses exp...  sizeof expressions don't get evaluated at run time */
    /* unless they are size of a vla... */
    if (tp->vla)
    {
        exp = tp->esize;
        tp = basetype(tp)->btp;

        while (tp->type == BasicType::pointer_)
        {
            exp = exprNode(ExpressionNode::mul_, exp, tp->esize);
            tp = basetype(tp)->btp;
        }
        exp = exprNode(ExpressionNode::mul_, exp, intNode(ExpressionNode::c_i_, tp->size));
    }
    else if (!isarray(tp) && Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        exp = exprNode(ExpressionNode::sizeof_, typeNode(tp), nullptr);
    }
    else
    {
        exp = nullptr;
        if (isstructured(tp))
        {
            if (basetype(tp)->size == 0 && !templateNestingCount)
                errorsym(ERR_UNSIZED_TYPE, basetype(tp)->sp);
            if (basetype(tp)->syms)
            {
                SYMBOL* cache = nullptr;
                TYPE* tpx;
                for (auto sym : *basetype(tp)->syms)
                {
                    if (ismemberdata(sym))
                        cache = sym;
                }
                if (cache)
                {
                    tpx = basetype(cache->tp);
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

static void left_fold(LEXLIST* lex, SYMBOL *funcsp, TYPE** resulttp, EXPRESSION** resultexp, LEXLIST* start, TYPE* seedtp, EXPRESSION* seedexp, TYPE *foldtp, EXPRESSION* foldexp)
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
    std::list<INITLIST*>* lptr = nullptr;
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
                    if (Optimizer::cparams.prm_cplusplus && *resulttp && isstructured(*resulttp))
                    {
                        if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdbool, false))
                            if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdint, false))
                                if (!castToPointer(resulttp, resultexp, (Keyword)-1, &stdpointer))
                                    errorConversionOrCast(true, *resulttp, &stdint);
                    }
                }
                GetLogicalDestructors(&logicaldestructorsright, e->exp);
                if (Optimizer::cparams.prm_cplusplus && e->tp && isstructured(e->tp))
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
static void right_fold(LEXLIST* lex, SYMBOL *funcsp, TYPE** resulttp, EXPRESSION** resultexp, LEXLIST* start, TYPE* seedtp, EXPRESSION* seedexp, TYPE *foldtp, EXPRESSION* foldexp)
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
    std::list<INITLIST*>* lptr = nullptr;
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
                    if (Optimizer::cparams.prm_cplusplus && *resulttp && isstructured(*resulttp))
                    {
                        if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdbool, false))
                            if (!castToArithmeticInternal(false, resulttp, resultexp, (Keyword)-1, &stdint, false))
                                if (!castToPointer(resulttp, resultexp, (Keyword)-1, &stdpointer))
                                    errorConversionOrCast(true, *resulttp, &stdint);
                    }
                }
                GetLogicalDestructors(&logicaldestructorsright, e->exp);
                if (Optimizer::cparams.prm_cplusplus && e->tp && isstructured(e->tp))
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


void eval_unary_left_fold(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** resulttp, EXPRESSION** resultexp, LEXLIST* start,
    TYPE* lefttp, EXPRESSION* leftexp, bool ismutable, int flags)
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
void eval_unary_right_fold(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** resulttp, EXPRESSION** resultexp, LEXLIST* start,
    TYPE* lefttp, EXPRESSION* leftexp, bool ismutable, int flags)
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
void eval_binary_fold(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** resulttp, EXPRESSION** resultexp, LEXLIST* leftstart,
    TYPE* lefttp, EXPRESSION* leftexp, LEXLIST* rightstart, TYPE* righttp, EXPRESSION* rightexp, bool ismutable,
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

void eval_unary_plus(LEXLIST *lex, SYMBOL *funcsp, TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp,
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
    if (isstructured(*resulttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (isvoid(*resulttp) || ismsil(*resulttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (basetype(*resulttp)->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (ispointer(*resulttp))
        error(ERR_ILL_POINTER_OPERATION);
    else if (atp && basetype(atp)->type < BasicType::int_)
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
void eval_unary_minus(LEXLIST *lex, SYMBOL *funcsp, TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
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
    if (isstructured(*resulttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (isvoid(*resulttp) || ismsil(*resulttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (basetype(*resulttp)->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (ispointer(*resulttp))
        error(ERR_ILL_POINTER_OPERATION);
    else if (atp && basetype(atp)->type < BasicType::int_)
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
void eval_unary_not(LEXLIST *lex, SYMBOL *funcsp, TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
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
    if (isstructured(*resulttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (isvoid(*resulttp) || ismsil(*resulttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->scoped && !(flags & _F_SCOPEDENUM))
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
void eval_unary_complement(LEXLIST *lex, SYMBOL *funcsp, TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
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
    if (isstructured(*resulttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (iscomplex(*resulttp))
        error(ERR_ILL_USE_OF_COMPLEX);
    else if (isfloat(*resulttp) || isimaginary(*resulttp))
        error(ERR_ILL_USE_OF_FLOATING);
    else if (ispointer(*resulttp))
        error(ERR_ILL_POINTER_OPERATION);
    else if (isvoid(*resulttp) || ismsil(*resulttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (basetype(*resulttp)->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (atp && basetype(atp)->type < BasicType::int_)
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
        if (!isbitint(*resulttp))
            cast(basetype(*resulttp), resultexp);
    }
}
void eval_unary_autoincdec(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, bool ismutable, int flags)
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
    if (isstructured(*resulttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (iscomplex(*resulttp))
        error(ERR_ILL_USE_OF_COMPLEX);
    else if (isconstraw(*resulttp) && !ismutable)
        error(ERR_CANNOT_MODIFY_CONST_OBJECT);
    else if (isvoid(*resulttp) || (*resulttp)->type == BasicType::aggregate_ || ismsil(*resulttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (basetype(*resulttp)->scoped && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (!lvalue(*resultexp))
    {
        if ((*resultexp)->type != ExpressionNode::templateparam_ && basetype(*resulttp)->type != BasicType::templateparam_ &&
            basetype(*resulttp)->type != BasicType::templateselector_ && basetype(*resulttp)->type != BasicType::templatedecltype_)
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
        if (ispointer(*resulttp))
        {
            TYPE *tpx;
            if (basetype(basetype(*resulttp)->btp)->type == BasicType::void_)
            {
                if (Optimizer::cparams.prm_cplusplus)
                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                tpx = &stdchar;
            }
            else
            {
                tpx = basetype(*resulttp)->btp;
            }
            *resultexp = exprNode(
                ExpressionNode::assign_, *resultexp,
                exprNode(kw == Keyword::autoinc_ ? ExpressionNode::add_ : ExpressionNode::sub_, *resultexp, nodeSizeof(tpx, *resultexp)));
            (*resultexp)->preincdec = true;
        }
        else if (kw == Keyword::autoinc_ && basetype(*resulttp)->type == BasicType::bool_)
        {
            *resultexp = exprNode(ExpressionNode::assign_, *resultexp, intNode(ExpressionNode::c_i_, 1));  // set to true as per C++
        }
        else if (kw == Keyword::autodec_ && basetype(*resulttp)->type == BasicType::bool_)
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
            *resultexp = exprNode(ExpressionNode::void_, exp3, *resultexp);
    }
}
bool eval_binary_pm(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, TYPE *righttp,
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
        if (ispointer(*resulttp))
        {
            *resulttp = basetype(*resulttp);
            *resulttp = (*resulttp)->btp;
            if (!isstructured(*resulttp))
            {
                errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->data->kw->name);
            }
        }
        else
            errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->data->kw->name);
    }
    else if (!isstructured(*resulttp))
    {
        errorstr(ERR_STRUCTURED_TYPE_EXPECTED, lex->data->kw->name);
    }
    if (isfunction(righttp) && isstructured(*resulttp))
    {
        rightexp = getMemberPtr(basetype(righttp)->sp, basetype(*resulttp)->sp, &righttp, funcsp);
    }
    if (basetype(righttp)->type != BasicType::memberptr_)
    {
        error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
    }
    else
    {
        if (isstructured(*resulttp) && basetype(righttp)->type == BasicType::memberptr_)
        {
            if (basetype(*resulttp)->sp != basetype(righttp)->sp && basetype(*resulttp)->sp->sb->mainsym != basetype(righttp)->sp &&
                basetype(*resulttp)->sp != basetype(righttp)->sp->sb->mainsym)
            {
                if (classRefCount(basetype(righttp)->sp, (*resulttp)->sp) != 1)
                {
                    errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, basetype(righttp)->sp, (*resulttp)->sp);
                }
                else
                {
                    *resultexp = baseClassOffset(basetype(righttp)->sp, (*resulttp)->sp, *resultexp);
                }
            }
            if (rightexp->type == ExpressionNode::memberptr_)
            {
                int lab = dumpMemberPtr(rightexp->v.sp, *resulttp, true);
                rightexp = intNode(ExpressionNode::labcon_, lab);
            }
            if (isfunction(basetype(righttp)->btp))
            {
                FUNCTIONCALL *funcparams = Allocate<FUNCTIONCALL>();
                if ((basetype(*resulttp))->sp->sb->vbaseEntries)
                {
                    EXPRESSION *ec =
                        exprNode(ExpressionNode::add_, rightexp,
                                 intNode(ExpressionNode::c_i_, getSize(BasicType::pointer_) + getSize(BasicType::int_)));
                    EXPRESSION *ec1;
                    deref(&stdint, &ec);
                    ec1 = exprNode(ExpressionNode::sub_, ec, intNode(ExpressionNode::c_i_, 1));
                    deref(&stdint, &ec1);
                    ec =
                        exprNode(ExpressionNode::cond_, ec, exprNode(ExpressionNode::void_, ec1, intNode(ExpressionNode::c_i_, 0)));
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
                funcparams->thistp = MakeType(BasicType::pointer_, *resulttp);
                *resultexp = varNode(ExpressionNode::func_, nullptr);
                (*resultexp)->v.func = funcparams;
                *resulttp = basetype(righttp);
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
                    *resultexp = exprNode(ExpressionNode::cond_, ec, exprNode(ExpressionNode::void_, ec1, *resultexp));
                }
                deref(&stdint, &rightexp);
                *resultexp = exprNode(ExpressionNode::add_, *resultexp, rightexp);
                *resultexp = exprNode(ExpressionNode::add_, *resultexp, intNode(ExpressionNode::c_i_, -1));
                if (!isstructured(basetype(righttp)->btp))
                {
                    deref(basetype(righttp)->btp, resultexp);
                }
                *resulttp = basetype(righttp)->btp;
            }
        }
    }
    return false;
}

bool eval_binary_times(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, TYPE *righttp,
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
    if (isstructured(*resulttp) || isstructured(righttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (isvoid(*resulttp) || isvoid(righttp) || ismsil(*resulttp) || ismsil(righttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((basetype(*resulttp)->scoped || basetype(righttp)->scoped) && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (ispointer(*resulttp) || ispointer(righttp))
        error(ERR_ILL_POINTER_OPERATION);
    else
    {
        int m1 = -1, m2 = -1;
        if (isimaginary(*resulttp) && isimaginary(righttp))
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
            if (isimaginary(*resulttp) && (isfloat(righttp) || isint(righttp)))
            {
                m1 = (int)(*resulttp)->type - (int)BasicType::float__imaginary_;
                m2 = isfloat(righttp) ? (int)(int)righttp->type - (int)BasicType::float_ : m1;
            }
            else if ((isfloat(*resulttp) || isint(*resulttp)) && isimaginary(righttp))
            {
                m1 = (int)(int)righttp->type - (int)BasicType::float__imaginary_;
                m2 = isfloat(*resulttp) ? (int)(*resulttp)->type - (int)BasicType::float_ : m1;
            }
            if (m1 >= 0)
            {
                bool b = isimaginary(*resulttp);
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
                type = isunsigned(*resulttp) ? ExpressionNode::udiv_ : ExpressionNode::div_;
                break;
            case Keyword::mod_:
                type = isunsigned(*resulttp) ? ExpressionNode::umod_ : ExpressionNode::mod_;
                break;
            default:
                break;
        }
        *resultexp = exprNode(type, *resultexp, rightexp);
    }
    return false;
}
bool eval_binary_add(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, TYPE *righttp,
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
            if (Optimizer::cparams.prm_cplusplus && *resulttp && isstructured(*resulttp))
            {
                if (!castToArithmeticInternal(false, resulttp, resultexp, kw, isstructured(righttp) ? &stdint : righttp, false))
                    castToPointer(resulttp, resultexp, kw, &stdpointer);
            }
            if (Optimizer::cparams.prm_cplusplus && righttp && isstructured(righttp))
            {
                if (!castToArithmeticInternal(false, &righttp, &rightexp, (Keyword)-1, isstructured(*resulttp) || ispointer(*resulttp) ? &stdint : *resulttp, false))
                    castToPointer(&righttp, &rightexp, kw, &stdpointer);
            }
            LookupSingleAggregate(*resulttp, resultexp);
            LookupSingleAggregate(righttp, &rightexp);
        }
    }
    bool msil = false;
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && kw == Keyword::plus_ &&
        (basetype(*resulttp)->type == BasicType::string_ || basetype(righttp)->type == BasicType::string_ ||
         (atp && basetype(atp)->type == BasicType::string_)))
    {
        msil = true;
        if ((*resultexp)->type == ExpressionNode::labcon_ && (*resultexp)->string)
            (*resultexp)->type = ExpressionNode::c_string_;
        else if (!ismsil(*resulttp))
            *resultexp = exprNode(ExpressionNode::x_object_, *resultexp, nullptr);
        if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
            rightexp->type = ExpressionNode::c_string_;
        else if (!ismsil(righttp))
            rightexp = exprNode(ExpressionNode::x_object_, rightexp, nullptr);
    }
    else if (kw == Keyword::plus_ && ispointer(*resulttp) && ispointer(righttp))
        error(ERR_ILL_POINTER_ADDITION);
    else if (isvoid(*resulttp) || isvoid(righttp) || ismsil(*resulttp) || ismsil(righttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((basetype(*resulttp)->scoped || basetype(righttp)->scoped) && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (kw != Keyword::plus_ && !ispointer(*resulttp) && ispointer(righttp))
        error(ERR_ILL_POINTER_SUBTRACTION);
    else if (isstructured(*resulttp) || isstructured(righttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (ispointer(*resulttp))
    {
        if (isarray(*resulttp) && (*resulttp)->msil)
            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
        else if (ispointer(righttp) && !comparetypes(*resulttp, righttp, true) && !comparetypes(righttp, *resulttp, true))
            error(ERR_NONPORTABLE_POINTER_CONVERSION);
        else if (iscomplex(righttp))
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (isfloat(righttp) || isimaginary(righttp))
            error(ERR_ILL_USE_OF_FLOATING);
    }
    else if (ispointer(righttp))
    {
        if (iscomplex(*resulttp))
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (isfloat(*resulttp) || isimaginary(*resulttp))
            error(ERR_ILL_USE_OF_FLOATING);
        else if (isarray(righttp) && (righttp)->msil)
            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
    }
    if (msil)
    {
        // MSIL back Keyword::end_ will take care of figuring out what function to call
        // to perform the concatenation
        *resultexp = exprNode(ExpressionNode::add_, *resultexp, rightexp);
        *resulttp = &std__string;
    }
    else if (ispointer(*resulttp))
    {
        EXPRESSION *ns;
        if (basetype(basetype(*resulttp)->btp)->type == BasicType::void_)
        {
            if (Optimizer::cparams.prm_cplusplus)
                error(ERR_ARITHMETIC_WITH_VOID_STAR);
            ns = nodeSizeof(&stdchar, *resultexp);
        }
        else
        {
            ns = nodeSizeof(basetype(*resulttp)->btp, *resultexp);
        }
        if (ispointer(righttp))
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
    else if (ispointer(righttp))
    {
        EXPRESSION *ns;
        if (basetype(basetype(righttp)->btp)->type == BasicType::void_)
        {
            if (Optimizer::cparams.prm_cplusplus)
                error(ERR_ARITHMETIC_WITH_VOID_STAR);
            ns = nodeSizeof(&stdchar, *resultexp);
        }
        else
        {
            ns = nodeSizeof(basetype(righttp)->btp, *resultexp);
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
        if (isimaginary(*resulttp) && (isfloat(righttp) || isint(righttp)))
        {
            m1 = (int)(*resulttp)->type - (int)BasicType::float__imaginary_;
            m2 = isfloat(righttp) ? (int)(int)righttp->type - (int)BasicType::float_ : m1;
        }
        else if ((isfloat(*resulttp) || isint(*resulttp)) && isimaginary(righttp))
        {
            m1 = (int)(int)righttp->type - (int)BasicType::float__imaginary_;
            m2 = isfloat(*resulttp) ? (int)(*resulttp)->type - (int)BasicType::float_ : m1;
        }
        if (m1 >= 0)
        {
            m1 = m1 > m2 ? m1 : m2;
            TYPE *tpa, *tpb;
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
            if (isimaginary(righttp))
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
bool eval_binary_shift(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp, TYPE *righttp,
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
    if (isstructured(*resulttp) || isstructured(righttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (isvoid(*resulttp) || isvoid(righttp) || ismsil(*resulttp) || ismsil(righttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if ((basetype(*resulttp)->scoped || basetype(righttp)->scoped) && !(flags & _F_SCOPEDENUM))
        error(ERR_SCOPED_TYPE_MISMATCH);
    else if (ispointer(*resulttp) || ispointer(righttp))
        error(ERR_ILL_POINTER_OPERATION);
    else if (isfloat(*resulttp) || isfloat(righttp) || isimaginary(*resulttp) || isimaginary(righttp))
        error(ERR_ILL_USE_OF_FLOATING);
    else if (iscomplex(*resulttp) || iscomplex(righttp))
        error(ERR_ILL_USE_OF_COMPLEX);
    else
    {
        ExpressionNode type = ExpressionNode::lsh_;
        if (kw == Keyword::rightshift_)
            if (isunsigned(*resulttp))
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
        else if (isbitint(righttp))
        {
            cast(&stdint, &rightexp);
        }
        *resultexp = exprNode(type, *resultexp, exprNode(ExpressionNode::shiftby_, rightexp, 0));
    }
    return false;
}
bool eval_binary_inequality(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp,
                       TYPE *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
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
            if (Optimizer::cparams.prm_cplusplus && *resulttp && isstructured(*resulttp))
            {
                if (!castToArithmeticInternal(false, resulttp, resultexp, kw, righttp, true))
                    castToPointer(resulttp, resultexp, kw, ispointer(righttp) ? righttp : &stdpointer);
            }
            if (Optimizer::cparams.prm_cplusplus && righttp && isstructured(righttp))
            {
                if (!castToArithmeticInternal(false, &righttp, &rightexp, (Keyword)-1, *resulttp, true))
                    castToPointer(&righttp, &rightexp, (Keyword)-1, ispointer(*resulttp) ? *resulttp : &stdpointer);
            }
        }
        LookupSingleAggregate(*resulttp, resultexp);
        LookupSingleAggregate(righttp, &rightexp);
    }

    if (Optimizer::cparams.prm_cplusplus)
    {
        SYMBOL *funcsp = nullptr;
        if ((ispointer(*resulttp) || basetype(*resulttp)->type == BasicType::memberptr_) && righttp->type == BasicType::aggregate_)
        {
            if (righttp->syms->size() > 1)
                errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)righttp->syms->front())->name);
            rightexp = varNode(ExpressionNode::pc_, righttp->syms->front());
            righttp = ((SYMBOL*)righttp->syms->front())->tp;
        }
        else if ((ispointer(righttp) || basetype(righttp)->type == BasicType::memberptr_) && (*resulttp)->type == BasicType::aggregate_)
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
    if (isstructured(*resulttp) || isstructured(righttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (isvoid(*resulttp) || isvoid(righttp) || ismsil(*resulttp) || ismsil(righttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    else if (ispointer(*resulttp))
    {
        if (isintconst(rightexp))
        {
            if (!isconstzero(righttp, rightexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if (isint(righttp))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if (isfloat(righttp) || isimaginary(righttp))
            error(ERR_ILL_USE_OF_FLOATING);
        else if (iscomplex(righttp))
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (ispointer(righttp) && !isvoidptr(*resulttp) && !isvoidptr(righttp) && !comparetypes(*resulttp, righttp, false))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
    }
    else if (ispointer(righttp))
    {
        if (isintconst(*resultexp))
        {
            if (!isconstzero(*resulttp, *resultexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if (isint(*resulttp))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if (isfloat(*resulttp) || isimaginary(*resulttp))
            error(ERR_ILL_USE_OF_FLOATING);
        else if (iscomplex(*resulttp))
            error(ERR_ILL_USE_OF_COMPLEX);
    }
    else if (isint(*resulttp) && isint(righttp))
    {
        if ((isunsigned(*resulttp) && !isunsigned(righttp)) || (isunsigned(righttp) && !isunsigned(*resulttp)))
            errorstr(ERR_SIGNED_UNSIGNED_MISMATCH_RELAT, opname);
    }
    *resulttp = destSize(*resulttp, righttp, resultexp, &rightexp, true, nullptr);
    ExpressionNode type = ExpressionNode::gt_;
    switch (kw)
    {
        case Keyword::gt_:
            type = isunsigned(*resulttp) ? ExpressionNode::ugt_ : ExpressionNode::gt_;
            break;
        case Keyword::geq_:
            type = isunsigned(*resulttp) ? ExpressionNode::uge_ : ExpressionNode::ge_;
            break;
        case Keyword::lt_:
            type = isunsigned(*resulttp) ? ExpressionNode::ult_ : ExpressionNode::lt_;
            break;
        case Keyword::leq_:
            type = isunsigned(*resulttp) ? ExpressionNode::ule_ : ExpressionNode::le_;
            break;
    }
    *resultexp = exprNode(type, *resultexp, rightexp);
    if (Optimizer::cparams.prm_cplusplus)
        *resulttp = &stdbool;
    else
        *resulttp = &stdint;
    return false;
}
bool eval_binary_equality(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp,
                            TYPE *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
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
            if (Optimizer::cparams.prm_cplusplus && *resulttp && isstructured(*resulttp))
            {
                if (!castToArithmeticInternal(false, resulttp, resultexp, kw, righttp, true))
                    castToPointer(resulttp, resultexp, kw, ispointer(righttp) ? righttp : &stdpointer);
            }
            if (Optimizer::cparams.prm_cplusplus && righttp && isstructured(righttp))
            {
                if (!castToArithmeticInternal(false, &righttp, &rightexp, (Keyword)-1, *resulttp, true))
                    castToPointer(&righttp, &rightexp, (Keyword)-1, ispointer(*resulttp) ? *resulttp : &stdpointer);
            }
        }
    }
    checkscope(*resulttp, righttp);
    if (Optimizer::cparams.prm_cplusplus)
    {
        SYMBOL *funcsp = nullptr;
        if ((ispointer(*resulttp) || basetype(*resulttp)->type == BasicType::memberptr_) && righttp->type == BasicType::aggregate_)
        {
            if (righttp->syms->size() > 1)
                errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)righttp->syms->front())->name);
            rightexp = varNode(ExpressionNode::pc_, righttp->syms->front());
            righttp = ((SYMBOL*)righttp->syms->front())->tp;
        }
        else if ((ispointer(righttp) || basetype(righttp)->type == BasicType::memberptr_) && (*resulttp)->type == BasicType::aggregate_)
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
    if (isstructured(*resulttp) || isstructured(righttp))
    {
        if ((Optimizer::architecture != ARCHITECTURE_MSIL) || (!isconstzero(*resulttp, *resultexp) && !isconstzero(righttp, rightexp)))
            error(ERR_ILL_STRUCTURE_OPERATION);
    }
    else if (isvoid(*resulttp) || isvoid(righttp) || ismsil(*resulttp) || ismsil(righttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    bool done = false;
    if (ispointer(*resulttp))
    {
        if (isintconst(rightexp))
        {
            if (!isconstzero(righttp, rightexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if (isint(righttp))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if (isfloat(righttp) || isimaginary(righttp))
            error(ERR_ILL_USE_OF_FLOATING);
        else if (iscomplex(righttp))
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (ispointer(righttp) && !isvoidptr(*resulttp) && !isvoidptr(righttp) && !comparetypes(*resulttp, righttp, false))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
    }
    else if (ispointer(righttp))
    {
        if (isintconst(*resultexp))
        {
            if (!isconstzero(*resulttp, *resultexp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if (isint(*resulttp))
            error(ERR_NONPORTABLE_POINTER_COMPARISON);
        else if (isfloat(*resulttp) || isimaginary(*resulttp))
            error(ERR_ILL_USE_OF_FLOATING);
        else if (iscomplex(*resulttp))
            error(ERR_ILL_USE_OF_COMPLEX);
    }
    else if (basetype(*resulttp)->type == BasicType::memberptr_)
    {
        if (basetype(righttp)->type == BasicType::memberptr_)
        {
            if (!comparetypes(basetype(*resulttp)->btp, basetype(righttp)->btp, true))
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
        else if (comparetypes(basetype(*resulttp)->btp, righttp, true))
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
    else if (basetype(righttp)->type == BasicType::memberptr_)
    {
        if (isconstzero(*resulttp, *resultexp))
        {
            *resultexp = exprNode(ExpressionNode::mp_as_bool_, rightexp, nullptr);
            (*resultexp)->size = righttp;
            if (kw == Keyword::eq_)
                *resultexp = exprNode(ExpressionNode::not_, *resultexp, nullptr);
            done = true;
        }
        else if (comparetypes(*resulttp, basetype(righttp)->btp, true))
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
            (!fittedConst(*resulttp, *resultexp) && !fittedConst(righttp, rightexp)) || !isint(*resulttp) || !isint(righttp))
            if (!isstructured(*resulttp) && !isstructured(righttp))
                destSize(*resulttp, righttp, resultexp, &rightexp, true, nullptr);
        *resultexp = exprNode(kw == Keyword::eq_ ? ExpressionNode::eq_ : ExpressionNode::ne_, *resultexp, rightexp);
    }
    if (Optimizer::cparams.prm_cplusplus)
        *resulttp = &stdbool;
    else
        *resulttp = &stdint;
    return false;
}
bool eval_binary_logical(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp,
                          TYPE *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
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
    if (isstructured(*resulttp) || isstructured(righttp))
        error(ERR_ILL_STRUCTURE_OPERATION);
    else if (isvoid(*resulttp) || isvoid(righttp) || ismsil(*resulttp) || ismsil(righttp))
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((basetype(*resulttp)->scoped || basetype(righttp)->scoped) && !(flags & _F_SCOPEDENUM) &&
             ((Optimizer::architecture != ARCHITECTURE_MSIL) || Optimizer::cparams.msilAllowExtensions))
        error(ERR_SCOPED_TYPE_MISMATCH);
    if (kw != Keyword::lor_ && kw != Keyword::land_)
    {
        if (ispointer(*resulttp) || ispointer(righttp))
            error(ERR_ILL_POINTER_OPERATION);
        else if (isfloat(*resulttp) || isfloat(righttp) || isimaginary(*resulttp) || isimaginary(righttp))
            error(ERR_ILL_USE_OF_FLOATING);
        else if (iscomplex(*resulttp) || iscomplex(righttp))
            error(ERR_ILL_USE_OF_COMPLEX);
        else if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
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
    if (basetype(*resulttp)->type == BasicType::memberptr_)
    {
        *resultexp = exprNode(ExpressionNode::mp_as_bool_, *resultexp, nullptr);
        (*resultexp)->size = *resulttp;
    }
    if (basetype(righttp)->type == BasicType::memberptr_)
    {
        rightexp = exprNode(ExpressionNode::mp_as_bool_, rightexp, nullptr);
        rightexp->size = righttp;
    }
    *resultexp = exprNode(type, *resultexp, rightexp);
    return false;
}
bool eval_binary_assign(LEXLIST *lex, SYMBOL *funcsp,TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp,
                          TYPE *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
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
        if (isarithmetic(*resulttp))
        {
            castToArithmetic(false, &righttp, &rightexp, (Keyword)-1, *resulttp, true);
        }
        else if (isstructured(righttp))
        {
            cppCast(*resulttp, &righttp, &rightexp);
        }
    }
    auto exp2 = &rightexp;
    while (castvalue(*exp2))
        exp2 = &(*exp2)->left;
    if ((isfunction(*resulttp) || isfuncptr(*resulttp)) && (*exp2)->type == ExpressionNode::func_ && (*exp2)->v.func->sp->sb->storage_class == StorageClass::overloads_)
    {
        TYPE *tp2 = nullptr;
        SYMBOL *funcsp;
        if ((*exp2)->v.func->sp->sb->parentClass && !(*exp2)->v.func->asaddress)
            if ((*exp2)->v.func->sp->sb->storage_class == StorageClass::member_ ||
                (*exp2)->v.func->sp->sb->storage_class == StorageClass::mutable_)
                error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
        funcsp = MatchOverloadedFunction((*resulttp), isfuncptr(*resulttp) || basetype(*resulttp)->type == BasicType::memberptr_ ? &righttp : &tp2,
                                         (*exp2)->v.func->sp, exp2, flags);
        if (funcsp && basetype(*resulttp)->type == BasicType::memberptr_)
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
        if (basetype(*resulttp)->btp && !comparetypes(basetype(*resulttp)->btp, righttp, true))
        {
            if (!isvoidptr(*resulttp))
                errorConversionOrCast(true, righttp, *resulttp);
        }
    }
    if (!compareXC(*resulttp, righttp))
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
    if (isstructured(righttp))
    {
        SYMBOL *conv = lookupNonspecificCast(basetype(righttp)->sp, *resulttp);
        if (conv)
        {
            FUNCTIONCALL *params = Allocate<FUNCTIONCALL>();
            params->thisptr = rightexp;
            params->thistp = righttp;
            params->ascall = true;
            params->functp = conv->tp;
            params->fcall = varNode(ExpressionNode::pc_, conv);
            params->sp = conv;
            rightexp = exprNode(ExpressionNode::func_, nullptr, nullptr);
            rightexp->v.func = params;
            righttp = basetype(conv->tp)->btp;
        }
    }
    if (((*resultexp)->type == ExpressionNode::const_ || isconstraw(*resulttp)) && !ismutable &&
        (!temp || temp->v.sp->sb->storage_class != StorageClass::parameter_ || !isarray(*resulttp)) &&
        ((*resultexp)->type != ExpressionNode::func_ || !isconstraw(basetype((*resultexp)->v.func->sp->tp)->btp)))
        error(ERR_CANNOT_MODIFY_CONST_OBJECT);
    else if (isvoid(*resulttp) || isvoid(righttp) || (*resulttp)->type == BasicType::aggregate_)
        error(ERR_NOT_AN_ALLOWED_TYPE);
    else if ((!templateNestingCount || instantiatingTemplate) && !isstructured(*resulttp) && /*((*resulttp)->btp && !ispointer((*resulttp)->btp)) &&*/ (!isarray(*resulttp) || !basetype(*resulttp)->msil) &&
             basetype(*resulttp)->type != BasicType::memberptr_ && basetype(*resulttp)->type != BasicType::templateparam_ &&
             basetype(*resulttp)->type != BasicType::templateselector_ && !lvalue(*resultexp) &&
             (*resultexp)->type != ExpressionNode::msil_array_access_)
        error(ERR_LVALUE);
    else if (symRef && symRef->v.sp->sb->attribs.inheritable.linkage2 == Linkage::property_ &&
             !symRef->v.sp->sb->has_property_setter)
        errorsym(ERR_CANNOT_MODIFY_PROPERTY_WITHOUT_SETTER, symRef->v.sp);
    else if (isstructured(*resulttp) && !basetype(*resulttp)->sp->sb->trivialCons)
    {
        errorsym(ERR_NO_ASSIGNMENT_OPERATOR, basetype(*resulttp)->sp);
    }
    else
        switch (kw)
        {
            case Keyword::asand_:
            case Keyword::asor_:
            case Keyword::asxor_:
            case Keyword::asleftshift_:
            case Keyword::asrightshift_:
                if (ismsil(*resulttp) || ismsil(righttp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (iscomplex(*resulttp) || iscomplex(righttp))
                    error(ERR_ILL_USE_OF_COMPLEX);
                if (isfloat(*resulttp) || isfloat(righttp) || isimaginary(*resulttp) || isimaginary(righttp))
                    error(ERR_ILL_USE_OF_FLOATING);
                /* fall through */
            case Keyword::astimes_:
            case Keyword::asdivide_:
            case Keyword::asmod_:
                if (ismsil(*resulttp) || ismsil(righttp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (ispointer(*resulttp) || ispointer(righttp))
                    error(ERR_ILL_POINTER_OPERATION);
                if (isstructured(*resulttp) || isstructured(righttp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                break;
            case Keyword::asplus_:
                if ((*resulttp)->type == BasicType::string_)
                {
                    if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
                        rightexp->type = ExpressionNode::c_string_;
                    else if (!ismsil(righttp))
                        rightexp = exprNode(ExpressionNode::x_object_, rightexp, nullptr);
                }
                else if (ismsil(*resulttp) || ismsil(righttp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (ispointer(*resulttp))
                {
                    if (ispointer(righttp))
                        error(ERR_ILL_POINTER_ADDITION);
                    else
                    {
                        EXPRESSION *ns;
                        if (iscomplex(righttp))
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (isfloat(righttp) || isimaginary(righttp))
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (isstructured(righttp))
                            error(ERR_ILL_STRUCTURE_OPERATION);
                        if (basetype(basetype(*resulttp)->btp)->type == BasicType::void_)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                error(ERR_ARITHMETIC_WITH_VOID_STAR);
                            cast(&stdint, &rightexp);
                            ns = nodeSizeof(&stdchar, rightexp);
                        }
                        else
                        {
                            cast(&stdint, &rightexp);
                            ns = nodeSizeof(basetype(*resulttp)->btp, rightexp);
                        }
                        rightexp = exprNode(ExpressionNode::umul_, rightexp, ns);
                    }
                }
                else if (ispointer(righttp))
                {
                    error(ERR_ILL_POINTER_OPERATION);
                }
                else if (isstructured(*resulttp) || isstructured(righttp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                break;
            case Keyword::asminus_:
                if (ismsil(*resulttp) || ismsil(righttp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (isstructured(*resulttp) || isstructured(righttp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (ispointer(righttp))
                {
                    error(ERR_ILL_POINTER_SUBTRACTION);
                }
                else if (basetype(*resulttp)->type == BasicType::memberptr_ || basetype(righttp)->type == BasicType::memberptr_)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if (ispointer(*resulttp))
                {
                    EXPRESSION *ns;
                    if (basetype(basetype(*resulttp)->btp)->type == BasicType::void_)
                    {
                        if (Optimizer::cparams.prm_cplusplus)
                            error(ERR_ARITHMETIC_WITH_VOID_STAR);
                        cast(&stdint, &rightexp);
                        ns = nodeSizeof(&stdchar, rightexp);
                    }
                    else
                    {
                        cast(&stdint, &rightexp);
                        ns = nodeSizeof(basetype(*resulttp)->btp, rightexp);
                    }
                    rightexp = exprNode(ExpressionNode::umul_, rightexp, ns);
                }
                break;
            case Keyword::assign_:
                if (basetype(*resulttp)->type == BasicType::string_)
                {
                    if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
                        rightexp->type = ExpressionNode::c_string_;
                }
                else if (basetype(*resulttp)->type == BasicType::object_)
                {
                    if (righttp->type != BasicType::object_ && !isstructured(righttp) && (!isarray(righttp) || !basetype(righttp)->msil))
                        rightexp = exprNode(ExpressionNode::x_object_, rightexp, nullptr);
                }
                else if (ismsil(*resulttp) || ismsil(righttp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                if (ispointer(*resulttp))
                {
                    if (isarray(*resulttp) && (*resulttp)->msil && !comparetypes(*resulttp, righttp, true) && natural_size(*resultexp) != ISZ_OBJECT)
                    {
                        *resultexp = exprNode(ExpressionNode::l_object_, *resultexp, nullptr);
                        (*resultexp)->v.tp = righttp;
                    }
                    if (isarithmetic(righttp))
                    {
                        if (iscomplex(righttp))
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (isfloat(righttp) || isimaginary(righttp))
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
                    else if (ispointer(basetype(righttp)) || righttp->type == BasicType::any_)
                    {
                        if (Optimizer::cparams.prm_cplusplus && basetype(righttp)->stringconst && !isconst(basetype(*resulttp)->btp))
                            error(ERR_INVALID_CHARACTER_STRING_CONVERSION);

                        while (righttp->type == BasicType::any_ && righttp->btp)
                            righttp = righttp->btp;
                        if (!ispointer(basetype(righttp)))
                            goto end;
                        if (!comparetypes(*resulttp, righttp, true))
                        {
                            bool found = false;
                            if (ispointer(righttp))
                            {
                                TYPE *tpo = basetype(basetype(righttp)->btp);
                                TYPE *tpn = basetype(basetype(*resulttp)->btp);
                                if (isstructured(tpo) && isstructured(tpn))
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
                                    if ((!isvoidptr(*resulttp) || !ispointer(righttp)) && !righttp->nullptrType)
                                        if (!isTemplatedPointer(*resulttp))
                                        {
                                            errorConversionOrCast(true, righttp, *resulttp);
                                        }
                                }
                                else if (!isvoidptr(*resulttp) && !isvoidptr(righttp))
                                {
                                    if (!matchingCharTypes(*resulttp, righttp))
                                        error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                                }
                                else if (Optimizer::cparams.prm_cplusplus && !isvoidptr(*resulttp) && isvoidptr(righttp) &&
                                         rightexp->type != ExpressionNode::nullptr_)
                                {
                                    error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                                }
                            }
                        }
                    }
                    else if (isfunction(righttp))
                    {
                        if (!isvoidptr(*resulttp) && (!isfunction(basetype(*resulttp)->btp) ||
                                                !comparetypes(basetype(basetype(*resulttp)->btp)->btp, basetype(righttp)->btp, true)))
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
                else if (ispointer(righttp))
                {
                    if (iscomplex(*resulttp))
                    {
                        error(ERR_ILL_USE_OF_COMPLEX);
                    }
                    else if (isfloat(*resulttp) || isimaginary(*resulttp))
                    {
                        error(ERR_ILL_USE_OF_FLOATING);
                    }
                    else if (isint(*resulttp))
                    {
                        if (basetype(*resulttp)->type != BasicType::bool_)
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    }
                    else if (isarray(righttp) && (righttp)->msil)
                    {
                        error(ERR_MANAGED_OBJECT_NO_ADDRESS);
                    }
                }
                if (isstructured(*resulttp) && (!isstructured(righttp) || (!comparetypes(*resulttp, righttp, true) && !sameTemplate(righttp, *resulttp))))
                {
                    if (!((Optimizer::architecture == ARCHITECTURE_MSIL) && basetype(*resulttp)->sp->sb->msil &&
                          (isconstzero(righttp, rightexp) || basetype(righttp)->nullptrType)))
                    {
                        error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                    }
                }
                else if (isstructured(*resulttp) && !(*resulttp)->size)
                {
                    if (!(flags & _F_SIZEOF))
                        errorsym(ERR_STRUCT_NOT_DEFINED, basetype(*resulttp)->sp);
                }
                else if (!isstructured(*resulttp) && isstructured(righttp))
                {
                    error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                }
                else if (basetype(*resulttp)->type == BasicType::memberptr_)
                {
                    if (rightexp->type == ExpressionNode::memberptr_)
                    {
                        if (rightexp->v.sp != basetype(*resulttp)->sp && rightexp->v.sp != basetype(*resulttp)->sp->sb->mainsym &&
                            !sameTemplate(rightexp->v.sp->tp, basetype(*resulttp)->sp->tp))  // DAL FIXED
                            errorConversionOrCast(true, righttp, *resulttp);
                    }
                    else if ((!isfunction(basetype(*resulttp)->btp) || !comparetypes(basetype(*resulttp)->btp, righttp, true)) &&
                             !isconstzero(righttp, *resultexp) && !comparetypes(*resulttp, righttp, true))
                    {
                        errorConversionOrCast(true, righttp, *resulttp);
                    }
                }
                else if (basetype(righttp)->type == BasicType::memberptr_)
                    errorConversionOrCast(true, righttp, *resulttp);
                break;
        }
    ExpressionNode op = ExpressionNode::or_;
    TYPE* tp2;
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
            op = isunsigned(*resulttp) && largenum(*resulttp) ? ExpressionNode::udiv_ : ExpressionNode::div_;
            break;
        case Keyword::asmod_:
            tp2 = destSize(*resulttp, righttp, nullptr, nullptr, false, nullptr);
            op = isunsigned(*resulttp) && largenum(*resulttp) ? ExpressionNode::umod_ : ExpressionNode::mod_;
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
            op = isunsigned(*resulttp) ? ExpressionNode::ursh_ : ExpressionNode::rsh_;
            break;
        default:
            break;
    }
    if (ismsil(*resulttp))
    {
        if (rightexp->type == ExpressionNode::labcon_ && rightexp->string)
            rightexp->type = ExpressionNode::c_string_;
        else if (!ismsil(righttp) && !isstructured(righttp) && (!isarray(righttp) || !basetype(righttp)->msil))
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
    else if (basetype(*resulttp)->type == BasicType::memberptr_)
    {
        if ((*resultexp)->type == ExpressionNode::not__lvalue_ || ((*resultexp)->type == ExpressionNode::func_ && !(*resultexp)->v.func->ascall) ||
            (*resultexp)->type == ExpressionNode::void_ || (*resultexp)->type == ExpressionNode::memberptr_)
        {
            if (basetype(*resulttp)->type != BasicType::templateparam_)
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
    else if (isstructured(*resulttp))
    {
        EXPRESSION *exp2 = rightexp;
        if (((*resultexp)->type == ExpressionNode::not__lvalue_ ||
             ((*resultexp)->type == ExpressionNode::func_ && (!(*resultexp)->v.func->ascall || (*resultexp)->v.func->returnSP)) ||
             ((*resultexp)->type == ExpressionNode::void_) && !(flags & _F_SIZEOF)))
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
            if (isatomic(*resulttp))
            {
                (*resultexp)->size = CopyType((*resultexp)->size);
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
                if (isarithmetic(*resulttp) || isarithmetic(righttp) ||
                    ((ispointer(*resulttp) && (!isarray(*resulttp) || !basetype(*resulttp)->msil)) ||
                     (ispointer(righttp) && (!isarray(righttp) || !basetype(righttp)->msil))))
                {
                    int n = natural_size(*resultexp);
                    if (natural_size(rightexp) != n)
                        cast((*resulttp), &rightexp);
                    else if (abs(n) == ISZ_BITINT && !comparetypes(*resulttp, righttp, false))
                        cast((*resulttp), &rightexp);
                }
                else if (isarray(*resulttp) && basetype(*resulttp)->msil)
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
            if (isbitint(*resulttp))
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
                    if (!comparetypes(tp2, *resulttp, 0))
                        cast(*resulttp, resultexp);
                }
                *resultexp = exprNode(ExpressionNode::assign_, dest, *resultexp);
            }
            else if (kw == Keyword::asmod_ || kw == Keyword::asdivide_ || basetype(*resulttp)->type == BasicType::bool_)
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
bool eval_binary_comma(LEXLIST *lex, SYMBOL *funcsp, TYPE *atp, TYPE **resulttp, EXPRESSION **resultexp, TYPE *lefttp, EXPRESSION *leftexp,
                       TYPE *righttp, EXPRESSION *rightexp, bool ismutable, int flags)
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
        *resultexp = exprNode(ExpressionNode::void_, *resultexp, rightexp);
        resultexp = &(*resultexp)->right;
    }
    return false;
}

}  // namespace Parser