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
#include "config.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "initbackend.h"
#include "unmangle.h"
#include "OptUtils.h"
#include "help.h"
#include "ccerr.h"
#include "declare.h"
#include "declcpp.h"
#include "symtab.h"
#include "stmt.h"
#include "lex.h"
#include "declare.h"
#include "beinterf.h"
#include "memory.h"
#include "cpplookup.h"
#include "mangle.h"
#include "constopt.h"
#include "expr.h"
#include "types.h"
#include "ListFactory.h"
#include "libcxx.h"
#include "Property.h"
#include "lambda.h"
#include "init.h"

namespace Parser
{
TYPE* typenum(char* buf, TYPE* tp);
bool isautotype(TYPE* tp)
{
    if (isref(tp))
        tp = basetype(tp)->btp;
    while (ispointer(tp))
        tp = basetype(tp)->btp;
    return basetype(tp)->type == BasicType::auto_;
}
bool isunsigned(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
        case BasicType::bool_:
        case BasicType::unsigned_:
        case BasicType::unsigned_short_:
        case BasicType::unsigned_char_:
        case BasicType::unsigned_long_:
        case BasicType::unsigned_long_long_:
        case BasicType::wchar_t_:
        case BasicType::unsigned_bitint_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool isbitint(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        return tp->type == BasicType::bitint_ || tp->type == BasicType::unsigned_bitint_;
    }
    return false;
}
bool isint(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
        case BasicType::bool_:
        case BasicType::int_:
        case BasicType::char8_t_:
        case BasicType::char16_t_:
        case BasicType::char32_t_:
        case BasicType::unsigned_:
        case BasicType::short_:
        case BasicType::unsigned_short_:
        case BasicType::char_:
        case BasicType::unsigned_char_:
        case BasicType::signed_char_:
        case BasicType::long_:
        case BasicType::unsigned_long_:
        case BasicType::long_long_:
        case BasicType::unsigned_long_long_:
        case BasicType::wchar_t_:
        case BasicType::inative_:
        case BasicType::unative_:
        case BasicType::bitint_:
        case BasicType::unsigned_bitint_:
            return true;
        case BasicType::templateparam_:
            if (tp->templateParam->second->type == TplType::int_)
                return isint(tp->templateParam->second->byNonType.tp);
            return false;
        default:
            if (tp->type == BasicType::enum_ && !Optimizer::cparams.prm_cplusplus)
                return true;

            return false;
        }
    }
    return false;
}
bool isfloat(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
        case BasicType::float_:
        case BasicType::double_:
        case BasicType::long_double_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool iscomplex(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
        case BasicType::float__complex_:
        case BasicType::double__complex_:
        case BasicType::long_double_complex_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool isimaginary(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
    {
        switch (tp->type)
        {
        case BasicType::float__imaginary_:
        case BasicType::double__imaginary_:
        case BasicType::long_double_imaginary_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool isarithmetic(TYPE* tp)
{
    tp = basetype(tp);
    return isint(tp) || isfloat(tp) || iscomplex(tp) || isimaginary(tp);
}
bool ismsil(TYPE* tp)
{
    tp = basetype(tp);
    return tp->type == BasicType::string_ || tp->type == BasicType::object_;
}
bool isconstraw(const TYPE* tp)
{
    bool done = false;
    bool rv = false;
    while (!done && tp)
    {
        switch (tp->type)
        {
        case BasicType::restrict_:
        case BasicType::volatile_:
        case BasicType::static_:
        case BasicType::atomic_:
        case BasicType::typedef_:
        case BasicType::far_:
        case BasicType::near_:
        case BasicType::lrqual_:
        case BasicType::rrqual_:
        case BasicType::derivedfromtemplate_:
        case BasicType::va_list_:
        case BasicType::objectArray_:
            tp = tp->btp;
            break;
        case BasicType::const_:
            rv = true;
            done = true;
            break;
        default:
            done = true;
            break;
        }
    }
    return rv;
}
bool isconst(const TYPE* tp) { return isconstraw(tp); }

bool isvolatile(const TYPE* tp)
{
    while (tp)
    {
        switch (tp->type)
        {
        case BasicType::restrict_:
        case BasicType::const_:
        case BasicType::va_list_:
        case BasicType::objectArray_:
        case BasicType::static_:
        case BasicType::atomic_:
        case BasicType::typedef_:
        case BasicType::far_:
        case BasicType::near_:
        case BasicType::lrqual_:
        case BasicType::rrqual_:
        case BasicType::derivedfromtemplate_:
            tp = tp->btp;
            break;
        case BasicType::volatile_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool islrqual(TYPE* tp)
{
    while (tp)
    {
        switch (tp->type)
        {
        case BasicType::volatile_:
        case BasicType::const_:
        case BasicType::va_list_:
        case BasicType::objectArray_:
        case BasicType::static_:
        case BasicType::atomic_:
        case BasicType::typedef_:
        case BasicType::far_:
        case BasicType::near_:
        case BasicType::rrqual_:
        case BasicType::restrict_:
        case BasicType::derivedfromtemplate_:
            tp = tp->btp;
            break;
        case BasicType::lrqual_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool isrrqual(TYPE* tp)
{
    while (tp)
    {
        switch (tp->type)
        {
        case BasicType::volatile_:
        case BasicType::const_:
        case BasicType::va_list_:
        case BasicType::objectArray_:
        case BasicType::static_:
        case BasicType::atomic_:
        case BasicType::typedef_:
        case BasicType::far_:
        case BasicType::near_:
        case BasicType::lrqual_:
        case BasicType::restrict_:
        case BasicType::derivedfromtemplate_:
            tp = tp->btp;
            break;
        case BasicType::rrqual_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool isrestrict(TYPE* tp)
{
    while (tp)
    {
        switch (tp->type)
        {
        case BasicType::volatile_:
        case BasicType::const_:
        case BasicType::va_list_:
        case BasicType::objectArray_:
        case BasicType::static_:
        case BasicType::atomic_:
        case BasicType::typedef_:
        case BasicType::far_:
        case BasicType::near_:
        case BasicType::lrqual_:
        case BasicType::rrqual_:
        case BasicType::derivedfromtemplate_:
            tp = tp->btp;
            break;
        case BasicType::restrict_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool isatomic(TYPE* tp)
{
    while (tp)
    {
        switch (tp->type)
        {
        case BasicType::volatile_:
        case BasicType::const_:
        case BasicType::va_list_:
        case BasicType::objectArray_:
        case BasicType::static_:
        case BasicType::restrict_:
        case BasicType::typedef_:
        case BasicType::far_:
        case BasicType::near_:
        case BasicType::lrqual_:
        case BasicType::rrqual_:
        case BasicType::derivedfromtemplate_:
            tp = tp->btp;
            break;
        case BasicType::atomic_:
            return true;
        default:
            return false;
        }
    }
    return false;
}
bool isvoid(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
        return tp->type == BasicType::void_;
    return false;
}
bool isvoidptr(TYPE* tp)
{
    tp = basetype(tp);
    return ispointer(tp) && isvoid(tp->btp);
}
bool isarray(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
        return ispointer(tp) && basetype(tp)->array;
    return false;
}
bool isunion(TYPE* tp)
{
    tp = basetype(tp);
    if (tp)
        return tp->type == BasicType::union_;
    return false;
}
TYPE* MakeType(TYPE& tp, BasicType type, TYPE* base)
{
    tp.type = type;
    tp.btp = base;
    tp.rootType = &tp;
    tp.size = getSize(type);
    switch (type)
    {
    case BasicType::void_:
    case BasicType::ellipse_:
    case BasicType::memberptr_:
        break;
    case BasicType::any_:
        tp.size = getSize(BasicType::int_);
        break;
    case BasicType::far_:
    case BasicType::near_:
    case BasicType::const_:
    case BasicType::va_list_:
    case BasicType::objectArray_:
    case BasicType::volatile_:
    case BasicType::restrict_:
    case BasicType::static_:
    case BasicType::atomic_:
    case BasicType::typedef_:
    case BasicType::lrqual_:
    case BasicType::rrqual_:
    case BasicType::derivedfromtemplate_:
        if (base)
        {
            tp.rootType = base->rootType;
            if (tp.rootType)
                tp.size = tp.rootType->size;
        }
        break;

    case BasicType::lref_:
    case BasicType::rref_:
    case BasicType::func_:
    case BasicType::ifunc_:
    case BasicType::object_:
        tp.size = getSize(BasicType::pointer_);
        break;
    case BasicType::enum_:
        tp.size = getSize(BasicType::int_);
        break;
    default:
        tp.size = getSize(type);
        break;
    }
    return &tp;
}
TYPE* MakeType(BasicType type, TYPE* base)
{
    TYPE* rv = Allocate<TYPE>();
    return MakeType(*rv, type, base);
}
TYPE* CopyType(TYPE* tp, bool deep, std::function<void(TYPE*&, TYPE*&)> callback)
{
    TYPE* rv = nullptr;
    if (deep)
    {
        TYPE** last = &rv;
        for (; tp; tp = tp->btp, last = &(*last)->btp)
        {
            *last = Allocate<TYPE>();
            **last = *tp;
            callback ? callback(tp, *last) : (void)0;
        }
        UpdateRootTypes(rv);
    }
    else
    {
        rv = Allocate<TYPE>();
        *rv = *tp;
        if (tp->rootType == tp)
            rv->rootType = rv;
    }
    return rv;
}
LEXLIST* getExceptionSpecifiers(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* sp, StorageClass storage_class)
{
    (void)storage_class;
    switch (KW(lex))
    {
    case Keyword::throw_:
        lex = getsym();
        if (MATCHKW(lex, Keyword::openpa_))
        {
            lex = getsym();
            if (MATCHKW(lex, Keyword::closepa_))
            {
                sp->sb->xcMode = xc_none;
                sp->sb->noExcept = true;
                if (!sp->sb->xc)
                    sp->sb->xc = Allocate<xcept>();
            }
            else
            {
                sp->sb->xcMode = xc_dynamic;
                if (!sp->sb->xc)
                    sp->sb->xc = Allocate<xcept>();
                lex = backupsym();
                do
                {
                    TYPE* tp = nullptr;
                    lex = getsym();
                    if (!MATCHKW(lex, Keyword::closepa_))
                    {
                        lex = get_type_id(lex, &tp, funcsp, StorageClass::cast_, false, true, false);
                        if (!tp)
                        {
                            error(ERR_TYPE_NAME_EXPECTED);
                        }
                        else
                        {
                            // this is reverse order but who cares?
                            Optimizer::LIST* p = Allocate<Optimizer::LIST>();
                            if (tp->type == BasicType::templateparam_ && tp->templateParam->second->packed)
                            {
                                if (!MATCHKW(lex, Keyword::ellipse_))
                                {
                                    error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                }
                                else
                                {
                                    lex = getsym();
                                }
                            }
                            p->next = sp->sb->xc->xcDynamic;
                            p->data = tp;
                            sp->sb->xc->xcDynamic = p;
                        }
                    }
                } while (MATCHKW(lex, Keyword::comma_));
            }
            needkw(&lex, Keyword::closepa_);
        }
        else
        {
            needkw(&lex, Keyword::openpa_);
        }
        break;
    case Keyword::noexcept_:
        lex = getsym();
        if (MATCHKW(lex, Keyword::openpa_))
        {
            lex = getsym();
            lex = getDeferredData(lex, &sp->sb->deferredNoexcept, false);
            needkw(&lex, Keyword::closepa_);
        }
        else
        {
            sp->sb->xcMode = xc_none;
            sp->sb->noExcept = true;
            sp->sb->deferredNoexcept = (LEXLIST*)-1;
            if (!sp->sb->xc)
                sp->sb->xc = Allocate<xcept>();
        }
        break;
    default:
        break;
    }

    return lex;
}
static LEXLIST* GetFunctionQualifiersAndTrailingReturn(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** sp, TYPE** tp, StorageClass storage_class)
{
    bool foundFinal = false;
    bool foundOverride = false;
    bool done = false;
    bool foundConst = false;
    bool foundVolatile = false;
    bool foundand = false;
    bool foundland = false;
    while (lex != nullptr && !done)
    {
        if (ISID(lex))
        {
            if (!strcmp(lex->data->value.s.a, "final"))
            {
                if (foundFinal)
                    error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                foundFinal = true;
                (*sp)->sb->isfinal = true;
                lex = getsym();
            }
            else if (!strcmp(lex->data->value.s.a, "override"))
            {
                if (foundOverride)
                    error(ERR_FUNCTION_CAN_HAVE_ONE_FINAL_OR_OVERRIDE);
                foundOverride = true;
                (*sp)->sb->isoverride = true;
                lex = getsym();
            }
            else
            {
                if (templateNestingCount)
                {
                    while (lex && ISID(lex))
                        lex = getsym();
                }
                done = true;
            }
        }
        else
            switch (KW(lex))
            {
            case Keyword::const_:
                foundConst = true;
                lex = getsym();
                break;
            case Keyword::volatile_:
                foundVolatile = true;
                lex = getsym();
                break;
            case Keyword::and_:
                foundand = true;
                lex = getsym();
                break;
            case Keyword::land_:
                foundland = true;
                lex = getsym();
                break;
            case Keyword::throw_:
            case Keyword::noexcept_:
                if (Optimizer::cparams.prm_cplusplus && *sp)
                {
                    funcLevel++;
                    lex = getExceptionSpecifiers(lex, funcsp, *sp, storage_class);
                    funcLevel--;
                }
                break;
            default:
                done = true;
                break;
            }
    }
    if (foundand && foundland)
        error(ERR_TOO_MANY_QUALIFIERS);
    if (foundVolatile)
    {
        *tp = MakeType(BasicType::volatile_, *tp);
    }
    if (foundConst)
    {
        *tp = MakeType(BasicType::const_, *tp);
    }
    if (foundand)
    {
        *tp = MakeType(BasicType::lrqual_, *tp);
    }
    else if (foundland)
    {
        *tp = MakeType(BasicType::rrqual_, *tp);
    }
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (MATCHKW(lex, Keyword::pointsto_))
    {
        TYPE* tpx = nullptr;
        SymbolTable<SYMBOL>* locals = localNameSpace->front()->syms;
        localNameSpace->front()->syms = basetype(*tp)->syms;
        funcLevel++;
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        parsingTrailingReturnOrUsing++;
        lex = get_type_id(lex, &tpx, funcsp, StorageClass::cast_, false, true, false);
        parsingTrailingReturnOrUsing--;
        // weed out temporary syms that were added as part of a decltype; they will be
        // reinstated as stackblock syms later
        auto itp = localNameSpace->front()->syms->begin();
        auto itpe = localNameSpace->front()->syms->end();
        while (itp != itpe)
        {
            SYMBOL* sym = *itp;
            auto it1 = itp;
            ++it1;
            if (sym->sb->storage_class != StorageClass::parameter_)
                localNameSpace->front()->syms->remove(itp);
            itp = it1;
        }
        if (tpx)
        {
            if (!isautotype(basetype(*tp)->btp))
                error(ERR_MULTIPLE_RETURN_TYPES_SPECIFIED);
            if (isarray(tpx))
            {
                TYPE* tpn = MakeType(BasicType::pointer_, basetype(tpx)->btp);
                if (isconst(tpx))
                {
                    tpn = MakeType(BasicType::const_, tpn);
                }
                if (isvolatile(tpx))
                {
                    tpn = MakeType(BasicType::volatile_, tpn);
                }
                basetype(*tp)->btp = tpn;
                basetype(*tp)->rootType = tpn->rootType;
            }
            else
            {
                basetype(*tp)->btp = tpx;
                basetype(*tp)->rootType = tpx->rootType;
            }
        }
        localNameSpace->front()->syms = locals;
        funcLevel--;
    }
    return lex;
}
static void resolveVLAs(TYPE* tp)
{
    while (tp && tp->type == BasicType::pointer_ && tp->array)
    {
        tp->vla = true;
        tp = tp->btp;
    }
}
static LEXLIST* getArrayType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, StorageClass storage_class, bool* vla, TYPE** quals, bool first,
    bool msil)
{
    EXPRESSION* constant = nullptr;
    TYPE* tpc = nullptr;
    TYPE* typein = *tp;
    bool unsized = false;
    bool empty = false;
    lex = getsym(); /* past '[' */
    *tp = PerformDeferredInitialization(*tp, funcsp);
    lex = getPointerQualifiers(lex, quals, true);
    if (MATCHKW(lex, Keyword::star_))
    {
        RequiresDialect::Feature(Dialect::c99, "Variable Length Array");
        if (storage_class != StorageClass::parameter_)
            error(ERR_UNSIZED_VLA_PARAMETER);
        lex = getsym();
        unsized = true;
    }
    else if (!MATCHKW(lex, Keyword::closebr_))
    {
        lex = expression_no_comma(lex, funcsp, nullptr, &tpc, &constant, nullptr, 0);
        if (tpc)
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                RemoveSizeofOperators(constant);
            }
            optimize_for_constants(&constant);
        }
        lex = getPointerQualifiers(lex, quals, true);
        if (!tpc)
        {
            tpc = &stdint;
            error(ERR_EXPRESSION_SYNTAX);
        }
        if (*quals)
        {
            RequiresDialect::Feature(Dialect::c99, "Array Qualifiers");
            for (auto t = *quals; t; t = t->btp)
                if (t->type == BasicType::static_)
                {
                    RequiresDialect::Feature(Dialect::c11, "'static' array qualifier");
                }
            if (storage_class != StorageClass::parameter_)
                error(ERR_ARRAY_QUAL_PARAMETER_ONLY);
        }
    }
    else
    {
        if (!first) /* previous was empty */
            error(ERR_ONLY_FIRST_INDEX_MAY_BE_EMPTY);
        empty = true;
    }
    if (MATCHKW(lex, Keyword::closebr_))
    {
        TYPE* tpp, * tpb = *tp;
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (MATCHKW(lex, Keyword::openbr_))
        {
            if (*quals)
                error(ERR_QUAL_LAST_ARRAY_ELEMENT);
            lex = getArrayType(lex, funcsp, tp, storage_class, vla, quals, false, msil);
        }
        tpp = MakeType(BasicType::pointer_, *tp);
        tpp->btp->msil = msil;  // tag the base type as managed, e.g. so we can't take address of it
        tpp->array = true;
        tpp->unsized = unsized;
        tpp->msil = msil;
        if (!unsized)
        {
            if (empty)
            {
                if (storage_class == StorageClass::member_)
                    RequiresDialect::Feature(Dialect::c99, "Flexible Arrays");
                tpp->size = 0;
            }
            else
            {
                if (!isint(tpc))
                    error(ERR_ARRAY_INDEX_INTEGER_TYPE);
                else if (tpc->type != BasicType::templateparam_ && isintconst(constant) && constant->v.i <= 0 - !!getStructureDeclaration())
                    if (!templateNestingCount)
                        error(ERR_ARRAY_INVALID_INDEX);
                if (tpc->type == BasicType::templateparam_)
                {
                    tpp->size = basetype(tpp->btp)->size;
                    tpp->esize = intNode(ExpressionNode::c_i_, 1);
                }
                else if (isarithmeticconst(constant))
                {
                    tpp->size = basetype(tpp->btp)->size;
                    tpp->size *= constant->v.i;
                    tpp->esize = intNode(ExpressionNode::c_i_, constant->v.i);
                }
                else
                {
                    if (!templateNestingCount && !msil)
                        RequiresDialect::Feature(Dialect::c99, "Variable Length Array");
                    tpp->esize = constant;
                    tpp->etype = tpc;
                    *vla = !msil && !templateNestingCount;
                }
            }
            *tp = tpp;
        }
        else
        {
            tpp->size = tpp->btp->size;
            tpp->esize = intNode(ExpressionNode::c_i_, tpp->btp->size);
            *tp = tpp;
        }
        if (typein && isstructured(typein))
        {
            checkIncompleteArray(typein, lex->data->errfile, lex->data->errline);
        }
    }
    else
    {
        error(ERR_ARRAY_NEED_CLOSEBRACKET);
        errskim(&lex, skim_comma);
    }
    return lex;
}
static LEXLIST* getAfterType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** sp, bool inTemplate, StorageClass storage_class,
    int consdest, bool funcptr)
{
    bool isvla = false;
    TYPE* quals = nullptr;
    TYPE* tp1 = nullptr;
    EXPRESSION* exp = nullptr;
    if (ISKW(lex))
    {
        switch (KW(lex))
        {
        case Keyword::openpa_:
            if (*sp)
            {
                lex = getFunctionParams(lex, funcsp, sp, tp, inTemplate, storage_class, funcptr);
                tp1 = *tp;
                if (tp1->type == BasicType::func_)
                {
                    *tp = (*tp)->btp;
                    lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                    tp1->btp = *tp;
                    *tp = tp1;
                    if (Optimizer::cparams.prm_cplusplus)
                    {
                        lex = GetFunctionQualifiersAndTrailingReturn(lex, funcsp, sp, tp, storage_class);
                    }
                }
                UpdateRootTypes(*tp);
            }
            else
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        case Keyword::openbr_:
            lex = getArrayType(lex, funcsp, tp, (*sp) ? (*sp)->sb->storage_class : storage_class, &isvla, &quals, true, false);
            if (isvla)
            {
                resolveVLAs(*tp);
            }
            else if (ispointer(*tp) && (*tp)->btp->vla)
            {
                (*tp)->vla = true;
            }
            if (quals)
            {
                TYPE* q2 = quals;
                while (q2->btp)
                    q2 = q2->btp;
                q2->btp = *tp;
                *tp = quals;
            }
            UpdateRootTypes(*tp);
            break;
        case Keyword::colon_:
            if (consdest == CT_CONS)
            {
                // defer to later
            }
            else if (*sp && ((*sp)->sb->storage_class == StorageClass::member_ || (*sp)->sb->storage_class == StorageClass::mutable_))
            {
                // error(ERR_BIT_STRUCT_MEMBER);
                if ((*sp)->tp)
                {
                    // we aren't allowing _Bitint to participate as a bit field...
                    if (Optimizer::cparams.prm_ansi)
                    {
                        if ((*sp)->tp->type != BasicType::int_ && (*sp)->tp->type != BasicType::unsigned_ && (*sp)->tp->type != BasicType::bool_)
                            error(ERR_ANSI_INT_BIT);
                    }
                    else if (!isint((*sp)->tp) && basetype((*sp)->tp)->type != BasicType::enum_)
                        error(ERR_BIT_FIELD_INTEGER_TYPE);
                }
                lex = getsym();
                lex = optimized_expression(lex, funcsp, nullptr, &tp1, &exp, false);
                if (tp1 && exp && isintconst(exp))
                {
                    int n = (*tp)->size * Optimizer::chosenAssembler->arch->bits_per_mau;
                    TYPE** tp1 = tp, * tpb = basetype(*tp1);
                    *tp1 = CopyType(tpb);
                    (*tp1)->bits = exp->v.i;
                    (*tp1)->hasbits = true;
                    (*tp1)->anonymousbits = (*sp)->sb->anonymous;
                    UpdateRootTypes(*tp1);
                    if ((*tp1)->bits > n)
                        error(ERR_BIT_FIELD_TOO_LARGE);
                    else if ((*tp1)->bits < 0 || ((*tp1)->bits == 0 && !(*sp)->sb->anonymous))
                        error(ERR_BIT_FIELD_MUST_CONTAIN_AT_LEAST_ONE_BIT);
                }
                else
                {
                    (*tp)->bits = 1;
                    error(ERR_CONSTANT_VALUE_EXPECTED);
                    errskim(&lex, skim_semi_declare);
                }
            }
            break;
        case Keyword::lt_:
            if (Optimizer::cparams.prm_cplusplus && inTemplate)
            {
                std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(*sp);
                lex = GetTemplateArguments(lex, funcsp, *sp, &templateParams->front().second->bySpecialization.types);
                lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest, false);
            }
            else
            {
                TEMPLATEPARAM* templateParam = Allocate<TEMPLATEPARAM>();
                templateParam->type = TplType::new_;
                lex = GetTemplateArguments(lex, funcsp, *sp, &templateParam->bySpecialization.types);
                lex = getAfterType(lex, funcsp, tp, sp, inTemplate, storage_class, consdest, false);
                if (*tp && isfunction(*tp))
                {
                    std::list<TEMPLATEPARAMPAIR>* lst = templateParamPairListFactory.CreateList();
                    lst->push_back(TEMPLATEPARAMPAIR{ nullptr, templateParam });
                    DoInstantiateTemplateFunction(*tp, sp, nullptr, nullptr, lst, true);
                    if ((*sp)->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
                        basisAttribs.inheritable.linkage4 = Linkage::virtual_;
                    if (!(*sp)->templateParams)
                    {
                        (*sp)->templateParams = lst;
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    return lex;
}
LEXLIST* getBeforeType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** spi, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv,
    bool inTemplate, StorageClass storage_class, Linkage* linkage, Linkage* linkage2, Linkage* linkage3,
    bool* notype, bool asFriend, int consdest, bool beforeOnly, bool funcptr)
{
    SYMBOL* sp;
    TYPE* ptype = nullptr;
    BasicType xtype = BasicType::none_;
    LEXLIST* pos = lex;
    bool doneAfter = false;

    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && MATCHKW(lex, Keyword::openbr_))
    {
        // managed array
        bool isvla = false;
        TYPE* quals = nullptr;
        lex = getArrayType(lex, funcsp, tp, storage_class, &isvla, &quals, true, true);
        if (quals)
        {
            TYPE* q2 = quals;
            while (q2->btp)
                q2 = q2->btp;
            q2->btp = *tp;
            *tp = quals;
        }
        UpdateRootTypes(*tp);
        //        doneAfter = true;
    }
    if (ISID(lex) || MATCHKW(lex, Keyword::classsel_) || MATCHKW(lex, Keyword::operator_) ||
        (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::ellipse_)))
    {
        SYMBOL* strSymX = nullptr;
        std::list<NAMESPACEVALUEDATA*>* nsvX = nullptr;
        STRUCTSYM s, s1;
        bool oldTemplateSpecialization = inTemplateSpecialization;
        s.tmpl = nullptr;
        inTemplateSpecialization = inTemplateType;
        inTemplateType = false;
        if (Optimizer::cparams.prm_cplusplus)
        {
            STRUCTSYM s;
            bool throughClass = false;
            bool pack = false;
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                pack = true;
                lex = getsym();
            }
            lex = nestedPath(lex, &strSymX, &nsvX, &throughClass, false, storage_class, false, 0);
            inTemplateSpecialization = oldTemplateSpecialization;
            if (strSymX)
            {
                if (structLevel && strSymX->tp->type == BasicType::templateselector_ && !MATCHKW(lex, Keyword::star_) && !MATCHKW(lex, Keyword::complx_))
                {
                    *tp = strSymX->tp;
                    strSymX = nullptr;
                    lex = getsym();
                    if (notype)
                        *notype = false;
                }
                else
                {
                    if (strSym)
                        *strSym = strSymX;
                    s1.str = strSymX;
                    addStructureDeclaration(&s1);
                }
            }
            if (nsv)
                *nsv = nsvX;
            if (strSymX && MATCHKW(lex, Keyword::star_))
            {
                bool inparen = false;
                if (pack)
                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                lex = getsym();
                if (funcptr && MATCHKW(lex, Keyword::openpa_))
                {
                    inparen = true;
                    lex = getsym();
                }
                ptype = MakeType(BasicType::memberptr_, *tp);
                if (strSymX->tp->type == BasicType::templateselector_)
                    ptype->sp = (*strSymX->sb->templateSelector)[1].sp;
                else
                    ptype->sp = strSymX;
                UpdateRootTypes(*tp);
                *tp = ptype;
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                if (strSym)
                    *strSym = nullptr;
                if (nsv)
                    *nsv = nullptr;
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                    nullptr, asFriend, false, beforeOnly, false);
                if (*tp && (ptype != *tp && isref(*tp)))
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                ptype->size = getSize(BasicType::pointer_) + getSize(BasicType::int_) * 2;
                if (inparen)
                {
                    if (!needkw(&lex, Keyword::closepa_))
                    {
                        errskim(&lex, skim_closepa);
                        skip(&lex, Keyword::closepa_);
                    }
                }
            }
            else
            {
                char buf[512];
                int ov = 0;
                TYPE* castType = nullptr;
                if (MATCHKW(lex, Keyword::complx_))
                {
                    lex = getsym();
                    if (!ISID(lex) || !*strSym || strcmp((*strSym)->name, lex->data->value.s.a))
                    {
                        error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                    }
                    else
                    {
                        strcpy(buf, overloadNameTab[CI_DESTRUCTOR]);
                        consdest = CT_DEST;
                    }
                }
                else
                {
                    lex = getIdName(lex, funcsp, buf, &ov, &castType);
                }
                if (!buf[0])
                {
                    if (!pack)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        sp = makeID(storage_class, *tp, *spi, NewUnnamedID());
                        sp->packed = pack;
                        SetLinkerNames(sp, Linkage::none_);
                        *spi = sp;
                    }
                    else
                    {
                        lex = prevsym(pos);
                    }
                }
                else
                {
                    //                    if (consdest == CT_DEST)
                    //                    {
                    //                        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                    //                    }
                    if (ov == CI_CAST)
                    {
                        sp = makeID(storage_class, castType, *spi, litlate(buf));
                        sp->packed = pack;
                        *tp = castType;
                        sp->sb->castoperator = true;
                    }
                    else
                    {
                        sp = makeID(storage_class, *tp, *spi, litlate(buf));
                        sp->packed = pack;
                        lex = getsym();
                    }
                    sp->sb->operatorId = ov;
                    if (lex)
                        sp->sb->declcharpos = lex->data->charindex;
                    *spi = sp;
                }
            }
        }
        else
        {
            inTemplateSpecialization = oldTemplateSpecialization;
            sp = makeID(storage_class, *tp, *spi, litlate(lex->data->value.s.a));
            sp->sb->declcharpos = lex->data->charindex;
            *spi = sp;
            lex = getsym();
        }
        if (inTemplate && *spi)
        {
            std::list<TEMPLATEPARAMPAIR>* templateParams;
            SYMBOL* ssp = strSymX;
            if (!ssp)
                ssp = getStructureDeclaration();
            (*spi)->sb->parentClass = ssp;
            (*spi)->sb->templateLevel = templateNestingCount;
            templateParams = TemplateGetParams(*spi);
            (*spi)->sb->templateLevel = 0;
            (*spi)->sb->parentClass = nullptr;
            if (templateParams)
            {
                s.tmpl = templateParams;
                addTemplateDeclaration(&s);
            }
        }
        if (nsvX && nsvX->front()->name)
        {
            Optimizer::LIST* nlist;

            nameSpaceList.push_front(nsvX->front()->name);

            globalNameSpace->push_front(nsvX->front()->name->sb->nameSpaceValues->front());
        }
        else
        {
            nsvX = nullptr;
        }
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (!doneAfter)
            lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
        if (nsvX)
        {
            nameSpaceList.pop_front();
            globalNameSpace->pop_front();
        }
        if (s.tmpl)
            dropStructureDeclaration();
        if (strSymX)
            dropStructureDeclaration();
    }
    else
        switch (KW(lex))
        {
            LEXLIST* start;
        case Keyword::openpa_:
            if (beforeOnly)
                break;
            start = lex;
            lex = getsym();
            /* in a parameter, open paren followed by a type is an  unnamed function */
            if ((storage_class == StorageClass::parameter_ || parsingUsing) &&
                (MATCHKW(lex, Keyword::closepa_) ||
                    (startOfType(lex, nullptr, false) && (!ISKW(lex) || !(lex->data->kw->tokenTypes & TT_LINKAGE)))))
            {
                TYPE* tp1;
                if (!*spi)
                {
                    sp = makeID(storage_class, *tp, *spi, NewUnnamedID());
                    SetLinkerNames(sp, Linkage::none_);
                    sp->sb->anonymous = true;
                    sp->sb->declcharpos = lex->data->charindex;
                    *spi = sp;
                }
                *tp = (*spi)->tp;
                lex = prevsym(start);
                tp1 = *tp;
                if (!parsingUsing)
                {
                    lex = getFunctionParams(lex, funcsp, spi, tp, inTemplate, storage_class, false);
                    tp1 = *tp;
                    *tp = (*tp)->btp;
                }
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                if (!parsingUsing)
                {
                    tp1->btp = *tp;
                    *tp = tp1;
                }
                UpdateRootTypes(tp1);
                return lex;
            }
            else if (Optimizer::cparams.prm_cplusplus && consdest)
            {
                // constructor or destructor name
                STRUCTSYM s;
                const char* name;
                if (consdest == CT_DEST)
                {
                    *tp = &stdvoid;
                    name = overloadNameTab[CI_DESTRUCTOR];
                }
                else
                {
                    *tp = MakeType(BasicType::pointer_, *tp);
                    name = overloadNameTab[CI_CONSTRUCTOR];
                }
                sp = makeID(storage_class, *tp, *spi, name);
                sp->sb->declcharpos = lex->data->charindex;
                *spi = sp;
                if (*strSym)
                {
                    s.str = *strSym;
                    addStructureDeclaration(&s);
                }
                lex = prevsym(start);
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                if (*strSym)
                {
                    dropStructureDeclaration();
                }
            }
            else if (MATCHKW(lex, Keyword::openbr_))
            {
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                needkw(&lex, Keyword::closepa_);
            }
            else
            {
                TYPE* quals = nullptr;
                /* stdcall et.al. before the ptr means one thing in borland,
                 * stdcall after means another
                 * we are treating them both the same, e.g. the resulting
                 * pointer-to-function will be stdcall linkage either way
                 */
                attributes oldAttribs = basisAttribs;

                basisAttribs = { 0 };

                ParseAttributeSpecifiers(&lex, funcsp, true);
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                lex = getBeforeType(lex, funcsp, &ptype, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2,
                    linkage3, nullptr, asFriend, false, beforeOnly, true);
                basisAttribs = oldAttribs;
                if (!ptype ||
                    (!isref(ptype) && !ispointer(ptype) && !isfunction(ptype) && basetype(ptype)->type != BasicType::memberptr_))
                {
                    // if here is not a potential pointer to func
                    if (!ptype)
                        *tp = ptype;
                    ptype = nullptr;
                    if (quals)
                    {
                        TYPE* atype = *tp;
                        *tp = quals;
                        while (quals->btp)
                            quals = quals->btp;
                        quals->btp = atype;
                        UpdateRootTypes(*tp);
                    }
                }
                if (!needkw(&lex, Keyword::closepa_))
                {
                    errskim(&lex, skim_closepa);
                    skip(&lex, Keyword::closepa_);
                }
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, true);
                if (ptype)
                {
                    // pointer to func or pointer to memberfunc
                    TYPE* atype = *tp;
                    *tp = ptype;
                    if (isref(ptype) && basetype(atype)->array)
                        basetype(atype)->byRefArray = true;
                    while ((isref(ptype) || isfunction(ptype) || ispointer(ptype) || basetype(ptype)->type == BasicType::memberptr_) &&
                        ptype->btp)
                        if (ptype->btp->type == BasicType::any_)
                        {
                            break;
                        }
                        else
                        {
                            ptype = ptype->btp;
                        }
                    ptype->btp = atype;
                    ptype->rootType = atype->rootType;
                    UpdateRootTypes(*tp);

                    if (quals)
                    {
                        atype = *tp;
                        *tp = quals;
                        while (quals->btp)
                            quals = quals->btp;
                        quals->btp = atype;
                        quals->rootType = atype->rootType;
                        UpdateRootTypes(*tp);
                        sizeQualifiers(*tp);
                    }
                    atype = basetype(*tp);
                    if (atype->type == BasicType::memberptr_ && isfunction(atype->btp))
                        atype->size = getSize(BasicType::int_) * 2 + getSize(BasicType::pointer_);

                }
                if (*spi)
                    (*spi)->tp = *tp;
                if (*tp && isfunction(*tp))
                    sizeQualifiers(basetype(*tp)->btp);
                if (*tp)
                    sizeQualifiers(*tp);
            }
            break;
        case Keyword::star_: {
            if (*tp && isref(*tp))
            {
                error(ERR_NO_REF_POINTER_REF);
            }
            bool inparen = false;
            lex = getsym();
            if (funcptr && MATCHKW(lex, Keyword::openpa_))
            {
                lex = getsym();
                // this isn't perfect, it doesn't work with nested parens around the identifier

                if (!ISID(lex))
                    lex = backupsym();
                else
                    inparen = true;
            }
            ParseAttributeSpecifiers(&lex, funcsp, true);
            ptype = *tp;
            while (ptype && ptype->type != BasicType::pointer_ && xtype == BasicType::none_)
            {
                if (ptype->type == BasicType::far_)
                    xtype = BasicType::pointer_;
                else if (ptype->type == BasicType::near_)
                    xtype = BasicType::pointer_;
                else if (ptype->type == BasicType::seg_)
                    xtype = BasicType::seg_;
                ptype = ptype->btp;
            }
            if (xtype == BasicType::none_)
                xtype = BasicType::pointer_;
            *tp = MakeType(xtype, *tp);
            lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
            lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                nullptr, asFriend, false, beforeOnly, false);
            if (inparen)
            {
                if (!needkw(&lex, Keyword::closepa_))
                {
                    errskim(&lex, skim_closepa);
                    skip(&lex, Keyword::closepa_);
                }
            }
        }
                           break;
        case Keyword::and_:
        case Keyword::land_:
            if (storage_class == StorageClass::catchvar_)
            {
                // already a ref;
                lex = getsym();
                ParseAttributeSpecifiers(&lex, funcsp, true);
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                    nullptr, asFriend, false, beforeOnly, false);
                break;
            }
            // using the C++ reference operator as the ref keyword...
            if (Optimizer::cparams.prm_cplusplus ||
                ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions &&
                    storage_class == StorageClass::parameter_ && KW(lex) == Keyword::and_))
            {
                if (*tp && isref(*tp) && !instantiatingTemplate && !templateNestingCount)
                {
                    error(ERR_NO_REF_POINTER_REF);
                }
                *tp = MakeType(MATCHKW(lex, Keyword::and_) ? BasicType::lref_ : BasicType::rref_, *tp);
                if (instantiatingTemplate)
                    CollapseReferences(*tp);
                lex = getsym();
                ParseAttributeSpecifiers(&lex, funcsp, true);
                lex = getQualifiers(lex, tp, linkage, linkage2, linkage3, nullptr);
                lex = getBeforeType(lex, funcsp, tp, spi, strSym, nsv, inTemplate, storage_class, linkage, linkage2, linkage3,
                    nullptr, asFriend, false, beforeOnly, false);
                if (storage_class != StorageClass::typedef_ && !isfunction(*tp) && !templateNestingCount && !instantiatingTemplate)
                {
                    auto tp2 = *tp;
                    while (tp2 && tp2->type != BasicType::lref_ && tp2->type != BasicType::rref_)
                    {
                        if ((tp2->type == BasicType::const_ || tp2->type == BasicType::volatile_))
                        {
                            error(ERR_REF_NO_QUALIFIERS);
                            break;
                        }
                        tp2 = tp2->btp;
                    }
                }
            }
            break;
        case Keyword::colon_: /* may have unnamed bit fields */
            *spi = makeID(storage_class, *tp, *spi, NewUnnamedID());
            SetLinkerNames(*spi, Linkage::none_);
            (*spi)->sb->anonymous = true;
            lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
            break;
        case Keyword::gt_:
        case Keyword::comma_:
            break;
        default:
            if (beforeOnly)
                return lex;
            if (*tp && (isstructured(*tp) || (*tp)->type == BasicType::enum_) && KW(lex) == Keyword::semicolon_)
            {
                lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
                *spi = nullptr;
                return lex;
            }
            if (storage_class != StorageClass::parameter_ && storage_class != StorageClass::cast_ && storage_class != StorageClass::catchvar_ && !asFriend)
            {
                if (MATCHKW(lex, Keyword::openpa_) || MATCHKW(lex, Keyword::openbr_) || MATCHKW(lex, Keyword::assign_) || MATCHKW(lex, Keyword::semicolon_))
                    error(ERR_IDENTIFIER_EXPECTED);
                else
                    errortype(ERR_IMPROPER_USE_OF_TYPE, *tp, nullptr);
            }
            if (!*tp)
            {
                *spi = nullptr;
                return lex;
            }
            *spi = makeID(storage_class, *tp, *spi, NewUnnamedID());
            SetLinkerNames(*spi, Linkage::none_);
            (*spi)->sb->anonymous = true;
            lex = getAfterType(lex, funcsp, tp, spi, inTemplate, storage_class, consdest, false);
            break;
        }
    if (*tp && (ptype = basetype(*tp)))
    {
        if (isfuncptr(ptype))
            ptype = basetype(ptype->btp);
        if (isfunction(ptype))
        {
            if (ptype->btp->type == BasicType::func_ || (ptype->btp->type == BasicType::pointer_ && ptype->btp->array && !ptype->btp->msil))
                error(ERR_FUNCTION_NO_RETURN_FUNCTION_ARRAY);
        }
    }
    else
    {
        // an error occurred
        *tp = &stdint;
    }
    ParseAttributeSpecifiers(&lex, funcsp, true);
    return lex;
}
LEXLIST* getBasicType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, SYMBOL** strSym_out, bool inTemplate, StorageClass storage_class,
    Linkage* linkage_in, Linkage* linkage2_in, Linkage* linkage3_in, AccessLevel access, bool* notype,
    bool* defd, int* consdest, bool* templateArg, bool* deduceTemplate, bool isTypedef, bool templateErr, bool inUsing, bool asfriend,
    bool constexpression)
{
    BasicType type = BasicType::none_;
    TYPE* tn = nullptr;
    TYPE* quals = nullptr;
    TYPE** tl;
    bool extended;
    bool iscomplex = false;
    bool imaginary = false;
    bool flagerror = false;
    bool foundint = false;
    int foundunsigned = 0;
    int foundsigned = 0;
    bool foundtypeof = false;
    bool foundsomething = false;
    bool int64 = false;
    Linkage linkage = Linkage::none_;
    Linkage linkage2 = Linkage::none_;
    Linkage linkage3 = Linkage::none_;
    int bitintbits = 0;

    *defd = false;
    while (KWTYPE(lex, TT_BASETYPE) || MATCHKW(lex, Keyword::decltype_))
    {
        if (foundtypeof)
            flagerror = true;
        switch (KW(lex))
        {
        case Keyword::int_:
            if (foundint)
                flagerror = true;
            foundint = true;
            switch (type)
            {
            case BasicType::unsigned_:
            case BasicType::short_:
            case BasicType::unsigned_short_:
            case BasicType::long_:
            case BasicType::unsigned_long_:
            case BasicType::long_long_:
            case BasicType::unsigned_long_long_:
            case BasicType::inative_:
            case BasicType::unative_:
                break;
            case BasicType::none_:
            case BasicType::signed_:
                type = BasicType::int_;
                break;
            default:
                flagerror = true;
            }
            break;
        case Keyword::native_:
            if (type == BasicType::unsigned_)
                type = BasicType::unative_;
            else if (type == BasicType::int_ || type == BasicType::none_)
                type = BasicType::inative_;
            else
                flagerror = true;
            break;
        case Keyword::char_:
            switch (type)
            {
            case BasicType::none_:
                type = BasicType::char_;
                break;
            case BasicType::int_:
            case BasicType::signed_:
                type = BasicType::signed_char_;
                break;
            case BasicType::unsigned_:
                type = BasicType::unsigned_char_;
                break;
            default:
                flagerror = true;
                break;
            }
            break;
        case Keyword::char8_t_:
            if (type != BasicType::none_)
                flagerror = true;
            type = BasicType::char8_t_;
            break;
        case Keyword::char16_t_:
            if (type != BasicType::none_)
                flagerror = true;
            type = BasicType::char16_t_;
            break;
        case Keyword::char32_t_:
            if (type != BasicType::none_)
                flagerror = true;
            type = BasicType::char32_t_;
            break;
        case Keyword::short_:
            switch (type)
            {
            case BasicType::none_:
            case BasicType::int_:
            case BasicType::signed_:
                type = BasicType::short_;
                break;
            case BasicType::unsigned_:
                type = BasicType::unsigned_short_;
                break;
            default:
                flagerror = true;
                break;
            }
            break;
        case Keyword::long_:
            switch (type)
            {
            case BasicType::double_:
                if (iscomplex)
                    type = BasicType::long_double_complex_;
                else if (imaginary)
                    type = BasicType::long_double_imaginary_;
                else
                    type = BasicType::long_double_;
                break;
            case BasicType::none_:
            case BasicType::int_:
            case BasicType::signed_:
                type = BasicType::long_;
                break;
            case BasicType::unsigned_:
                type = BasicType::unsigned_long_;
                break;
            case BasicType::long_:
                type = BasicType::long_long_;
                break;
            case BasicType::unsigned_long_:
                type = BasicType::unsigned_long_long_;
                break;
            default:
                flagerror = true;
                break;
            }
            break;
        case Keyword::signed_:
            if (foundsigned || foundunsigned)
                flagerror = true;
            if (type == BasicType::none_)
                type = BasicType::signed_;
            else if (type == BasicType::char_)
                type = BasicType::signed_char_;
            foundsigned++;
            break;
        case Keyword::unsigned_:
            if (foundsigned || foundunsigned)
                flagerror = true;
            foundunsigned++;
            switch (type)
            {
            case BasicType::char_:
                type = BasicType::unsigned_char_;
                break;
            case BasicType::short_:
                type = BasicType::unsigned_short_;
                break;
            case BasicType::long_:
                type = BasicType::unsigned_long_;
                break;
            case BasicType::long_long_:
                type = BasicType::unsigned_long_long_;
                break;
            case BasicType::none_:
            case BasicType::int_:
                type = BasicType::unsigned_;
                break;
            case BasicType::inative_:
                type = BasicType::unative_;
                break;
            case BasicType::bitint_:
                RequiresDialect::Keyword(Dialect::c2x, "_Bitint");
                type = BasicType::unsigned_bitint_;
                lex = getsym();
                needkw(&lex, Keyword::openpa_);
                {
                    TYPE* tp = nullptr;
                    EXPRESSION* exp = nullptr;
                    lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
                    if (!tp || !isint(tp) || !isintconst(exp))
                        error(ERR_NEED_INTEGER_EXPRESSION);
                    bitintbits = exp->v.i;
                }
                if (!MATCHKW(lex, Keyword::closepa_))
                    needkw(&lex, Keyword::closepa_);
                break;
            default:
                flagerror = true;
                break;
            }
            break;
        case Keyword::int64_:
            switch (type)
            {
            case BasicType::unsigned_:
                int64 = true;
                type = BasicType::unsigned_long_long_;
                break;
            case BasicType::none_:
            case BasicType::signed_:
                int64 = true;
                type = BasicType::long_long_;
                break;
            default:
                flagerror = true;
                break;
            }
            break;
        case Keyword::bitint_:
            RequiresDialect::Keyword(Dialect::c2x, "_Bitint");
            switch (type)
            {
            case BasicType::unsigned_:
                int64 = true;
                type = BasicType::unsigned_bitint_;
                break;
            case BasicType::none_:
            case BasicType::signed_:
                int64 = true;
                type = BasicType::bitint_;
                break;
            default:
                flagerror = true;
                break;
            }
            lex = getsym();
            needkw(&lex, Keyword::openpa_);
            {
                TYPE* tp = nullptr;
                EXPRESSION* exp = nullptr;
                lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
                if (!tp || !isint(tp) || !isintconst(exp))
                    error(ERR_NEED_INTEGER_EXPRESSION);
                bitintbits = exp->v.i;
            }
            if (!MATCHKW(lex, Keyword::closepa_))
                needkw(&lex, Keyword::closepa_);
            break;
        case Keyword::wchar_t_:
            if (type == BasicType::none_)
                type = BasicType::wchar_t_;
            else
                flagerror = true;
            break;
        case Keyword::auto_:
            if (type == BasicType::none_)
                type = BasicType::auto_;
            else
                flagerror = true;
            break;
        case Keyword::bool_:
            if (type == BasicType::none_)
                if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_USESIZE))
                {
                    if (storage_class == StorageClass::auto_)
                        if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_LOCALBITS))
                        {
                            type = BasicType::bool_;
                        }
                        else
                        {
                            type = BasicType::bit_;
                        }
                    else if (storage_class == StorageClass::global_)
                    {
                        if (!(Optimizer::chosenAssembler->arch->boolmode & ABM_GLOBALBITS))
                        {
                            type = BasicType::bool_;
                        }
                        else
                        {
                            type = BasicType::bit_;
                        }
                    }
                    else
                        type = BasicType::bool_;
                }
                else
                    type = BasicType::bool_;
            else
                flagerror = true;
            break;
        case Keyword::float_:
            if (type == BasicType::none_)
                if (iscomplex)
                    type = BasicType::float__complex_;
                else if (imaginary)
                    type = BasicType::float__imaginary_;
                else
                    type = BasicType::float_;
            else
                flagerror = true;
            break;
        case Keyword::double_:
            if (type == BasicType::none_)
                if (iscomplex)
                    type = BasicType::double__complex_;
                else if (imaginary)
                    type = BasicType::double__imaginary_;
                else
                    type = BasicType::double_;
            else if (type == BasicType::long_)
                if (iscomplex)
                    type = BasicType::long_double_complex_;
                else if (imaginary)
                    type = BasicType::long_double_imaginary_;
                else
                    type = BasicType::long_double_;
            else
                flagerror = true;
            break;
        case Keyword::Complex_:
            switch (type)
            {
            case BasicType::float_:
                type = BasicType::float__complex_;
                break;
            case BasicType::double_:
                type = BasicType::double__complex_;
                break;
            case BasicType::long_double_:
                type = BasicType::long_double_complex_;
                break;
            case BasicType::none_:
                if (iscomplex || imaginary)
                    flagerror = true;
                iscomplex = true;
                break;
            default:
                flagerror = true;
                break;
            }
            break;
        case Keyword::Imaginary_:
            switch (type)
            {
            case BasicType::float_:
                type = BasicType::float__imaginary_;
                break;
            case BasicType::double_:
                type = BasicType::double__imaginary_;
                break;
            case BasicType::long_double_:
                type = BasicType::long_double_imaginary_;
                break;
            case BasicType::none_:
                if (imaginary || iscomplex)
                    flagerror = true;
                imaginary = true;
                break;
            default:
                flagerror = true;
                break;
            }
            break;
        case Keyword::object_:
            if (type != BasicType::none_)
                flagerror = true;
            else
                type = BasicType::object_;
            break;
        case Keyword::string_:
            if (type != BasicType::none_)
                flagerror = true;
            else
                type = BasicType::string_;
            break;
        case Keyword::struct_:
        case Keyword::class_:
        case Keyword::union_:
            inTemplateType = false;
            if (foundsigned || foundunsigned || type != BasicType::none_)
                flagerror = true;
            lex = declstruct(lex, funcsp, &tn, inTemplate, asfriend, storage_class, *linkage2_in, access, defd, constexpression);
            goto exit;
        case Keyword::enum_:
            if (foundsigned || foundunsigned || type != BasicType::none_)
                flagerror = true;
            lex = declenum(lex, funcsp, &tn, storage_class, access, false, defd);
            goto exit;
        case Keyword::void_:
            if (type != BasicType::none_)
                flagerror = true;
            type = BasicType::void_;
            break;
        case Keyword::decltype_:
            //                lex = getDeclType(lex, funcsp, &tn);
            type = BasicType::void_; /* won't really be used */
            foundtypeof = true;
            if (foundsomething)
                flagerror = true;
            goto founddecltype;
        case Keyword::typeof_:
        case Keyword::typeof_unqual_: {
            auto kw = KW(lex);
            type = BasicType::void_; /* won't really be used */
            foundtypeof = true;
            if (foundsomething)
                flagerror = true;
            lex = getsym();
            if (MATCHKW(lex, Keyword::openpa_))
            {
                EXPRESSION* exp;
                lex = getsym();
                lex = expression_no_check(lex, nullptr, nullptr, &tn, &exp, 0);
                if (tn)
                {
                    optimize_for_constants(&exp);
                }
                if (!tn)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                if (!MATCHKW(lex, Keyword::closepa_))
                    needkw(&lex, Keyword::closepa_);
            }
            else if (ISID(lex) || MATCHKW(lex, Keyword::classsel_))
            {
                SYMBOL* sp;
                lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, storage_class, true, true);
                if (sp)
                    tn = sp->tp;
            }
            else
            {
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(&lex, skim_semi_declare);
            }
            if (kw == Keyword::typeof_unqual_)
                tn = basetype(tn);
        }
                                    break;
        case Keyword::atomic_:
            RequiresDialect::Keyword(Dialect::c11, "_Atomic");
            lex = getsym();
            if (needkw(&lex, Keyword::openpa_))
            {
                tn = nullptr;
                lex = get_type_id(lex, &tn, funcsp, StorageClass::cast_, false, true, false);
                if (tn)
                {
                    quals = MakeType(BasicType::atomic_, quals);
                    auto tz = tn;
                    while (tz->type == BasicType::typedef_)
                        tz = tz->btp;
                    if (basetype(tz) != tz)
                        error(ERR_ATOMIC_TYPE_SPECIFIER_NO_QUALS);
                }
                else
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                if (!MATCHKW(lex, Keyword::closepa_))
                {
                    needkw(&lex, Keyword::closepa_);
                    errskim(&lex, skim_closepa);
                    skip(&lex, Keyword::closepa_);
                }
            }
            else
            {
                errskim(&lex, skim_closepa);
                skip(&lex, Keyword::closepa_);
            }
            break;
        case Keyword::underlying_type_: {
            lex = getsym();
            underlying_type(&lex, funcsp, nullptr, &tn, nullptr);
            lex = backupsym();
        }
                                      break;
        default:
            break;
        }
        foundsomething = true;
        lex = getsym();
        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3, nullptr);
        if (linkage != Linkage::none_)
        {
            *linkage_in = linkage;
        }
        if (linkage2 != Linkage::none_)
            *linkage2_in = linkage2;
        if (linkage3 != Linkage::none_)
            *linkage3_in = linkage3;
    }
    if (type == BasicType::signed_)  // BasicType::signed_ is just an internal placeholder
        type = BasicType::int_;
founddecltype:
    if (!foundsomething)
    {
        bool typeName = false;
        type = BasicType::int_;
        if (MATCHKW(lex, Keyword::typename_))
        {
            typeName = true;
            //   lex = getsym();
        }
        if (iscomplex || imaginary)
            error(ERR_MISSING_TYPE_SPECIFIER);
        else if (ISID(lex) || MATCHKW(lex, Keyword::classsel_) || MATCHKW(lex, Keyword::complx_) || MATCHKW(lex, Keyword::decltype_) ||
            MATCHKW(lex, Keyword::typename_))
        {
            std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            SYMBOL* sp = nullptr;
            bool destructor = false;
            LEXLIST* placeholder = lex;
            bool inTemplate = false;  // hides function parameter
            lex = nestedSearch(lex, &sp, &strSym, &nsv, &destructor, &inTemplate, false, storage_class, false, true);
            if (sp && (istype(sp) || (sp->sb && ((sp->sb->storage_class == StorageClass::type_ && inTemplate) ||
                (sp->sb->storage_class == StorageClass::typedef_ && sp->sb->templateLevel)))))
            {
                if (sp->sb && sp->sb->attribs.uninheritable.deprecationText && !isstructured(sp->tp))
                    deprecateMessage(sp);
                lex = getsym();
                if (sp->sb && sp->sb->storage_class == StorageClass::typedef_ && sp->sb->templateLevel)
                {
                    if (MATCHKW(lex, Keyword::lt_))
                    {
                        // throwaway
                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                        SYMBOL* sp1;
                        lex = GetTemplateArguments(lex, funcsp, sp, &lst);
                        if (!parsingTrailingReturnOrUsing)
                        {
                            sp1 = GetTypeAliasSpecialization(sp, lst);
                            if (sp1)
                            {
                                sp = sp1;
                                if (/*!inUsing || */ !templateNestingCount)
                                {
                                    if (isstructured(sp->tp))
                                        sp->tp = PerformDeferredInitialization(sp->tp, funcsp);
                                    else
                                        sp->tp = SynthesizeType(sp->tp, nullptr, false);
                                }
                            }
                        }
                        else
                        {
                            auto oldsp = sp;
                            sp = CopySymbol(sp);
                            sp->sb->mainsym = oldsp;
                            sp->tp = CopyType(sp->tp);
                            sp->tp->sp = sp;
                            sp->templateParams = lst;
                            sp->templateParams->push_front(TEMPLATEPARAMPAIR());
                            sp->templateParams->front().second = Allocate<TEMPLATEPARAM>();
                            sp->templateParams->front().second->type = TplType::new_;
                        }

                        tn = sp->tp;
                        foundsomething = true;
                    }
                    else
                    {
                        SpecializationError(sp);
                        tn = sp->tp;
                    }
                }
                else
                {
                    SYMBOL* ssp = getStructureDeclaration();
                    TYPE* tpx = basetype(sp->tp);
                    foundsomething = true;
                    if (tpx->type == BasicType::templateparam_)
                    {
                        tn = nullptr;
                        if (templateArg)
                            *templateArg = true;
                        if (!tpx->templateParam->second->packed)
                        {
                            if (tpx->templateParam->second->type == TplType::typename_)
                            {
                                tn = tpx->templateParam->second->byClass.val;
                                if (*tp && tn)
                                {
                                    // should only be const vol specifiers
                                    TYPE* tpy = *tp;
                                    while (tpy->btp)
                                        tpy = tpy->btp;
                                    tpy->btp = tpx;
                                    // used to pass a pointer to tpx->templateParam
                                    tn = SynthesizeType(*tp, nullptr, false);
                                    *tp = nullptr;
                                }
                                if (inTemplate)
                                {
                                    if (MATCHKW(lex, Keyword::lt_))
                                    {
                                        // throwaway
                                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                        lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                                    }
                                    errorsym(ERR_NOT_A_TEMPLATE, sp);
                                }
                            }
                            else if (tpx->templateParam->second->type == TplType::template_)
                            {
                                if (MATCHKW(lex, Keyword::lt_))
                                {

                                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                    SYMBOL* sp1 = tpx->templateParam->second->byTemplate.val;
                                    lex = GetTemplateArguments(lex, funcsp, sp1, &lst);
                                    if (sp1)
                                    {
                                        sp1 = GetClassTemplate(sp1, lst, !templateErr);
                                        tn = nullptr;
                                        if (sp1)
                                        {
                                            if (sp1->tp->type == BasicType::typedef_)
                                            {
                                                tn = SynthesizeType(sp1->tp, nullptr, false);
                                            }
                                            else
                                            {
                                                tn = sp1->tp;
                                            }
                                        }
                                    }
                                    else if (templateNestingCount)
                                    {
                                        sp1 = CopySymbol(sp);
                                        sp1->tp = CopyType(sp->tp);
                                        sp1->tp->sp = sp1;
                                        // used to patch in the remainder of the list in the 'next' field
                                        sp1->tp->templateParam = Allocate<TEMPLATEPARAMPAIR>();
                                        //sp1->tp->templateParam->next = sp->tp->templateParam->next;
                                        sp1->tp->templateParam->second = Allocate<TEMPLATEPARAM>();
                                        *sp1->tp->templateParam->second = *sp->tp->templateParam->second;
                                        sp1->tp->templateParam->second->byTemplate.orig = sp->tp->templateParam;
                                        auto itl = lst->begin();
                                        auto itle = lst->end();
                                        auto ito = sp1->tp->templateParam->second->byTemplate.args->begin();
                                        auto itoe = sp1->tp->templateParam->second->byTemplate.args->end();
                                        auto tnew = templateParamPairListFactory.CreateList();
                                        while (itl != itle && ito != itoe)
                                        {
                                            if (ito->second->type == TplType::new_)
                                            {
                                                tnew->push_back(TEMPLATEPARAMPAIR{ nullptr, ito->second });
                                            }
                                            else if (!itl->first)
                                            {
                                                tnew->push_back(TEMPLATEPARAMPAIR{ nullptr, itl->second });
                                            }
                                            else
                                            {
                                                SYMBOL* sp = classsearch(itl->first->name, false, false, false);
                                                if (sp && sp->tp->type == BasicType::templateparam_)
                                                {
                                                    tnew->push_back(TEMPLATEPARAMPAIR{ sp->tp->templateParam->first, sp->tp->templateParam->second });
                                                }
                                                else
                                                {
                                                    tnew->push_back(TEMPLATEPARAMPAIR{ itl->first, itl->second });
                                                }
                                            }
                                            ++itl;
                                            ++ito;
                                        }
                                        sp1->tp->templateParam->second->byTemplate.args = tnew;
                                        sp = sp1;
                                    }
                                }
                                else
                                {
                                    SYMBOL* sp1 = tpx->templateParam->second->byTemplate.val;
                                    if (sp1 && allTemplateArgsSpecified(sp1, tpx->templateParam->second->byTemplate.args))
                                    {
                                        tn = sp1->tp;
                                    }
                                    else
                                    {

                                        tn = sp->tp;
                                    }
                                }
                            }
                            else
                            {
                                diag("getBasicType: expected typename template param");
                            }
                        }
                        else if (expandingParams && tpx->type == BasicType::templateparam_ && tpx->templateParam->second->byPack.pack)
                        {

                            auto packed = tpx->templateParam->second->byPack.pack->begin();
                            auto packede = tpx->templateParam->second->byPack.pack->end();
                            int i;
                            for (i = 0; i < packIndex && packed != packede; i++, ++packed);
                            if (packed != packede)
                                tn = packed->second->byClass.val;
                        }
                        if (!tn)
                            tn = sp->tp;
                    }
                    else if (tpx->type == BasicType::templatedecltype_)
                    {
                        if (templateArg)
                            *templateArg = true;
                        if (!templateNestingCount)
                            TemplateLookupTypeFromDeclType(tpx);
                    }
                    else if (tpx->type == BasicType::templateselector_)
                    {
                        if (templateArg)
                            *templateArg = true;
                        //                        if (!templateNestingCount)
                        //                        {
                        //                            tn = SynthesizeType(sp->tp, nullptr, false);
                        //                        }
                        //                        else
                        {
                            tn = sp->tp;
                        }
                        if ((*tpx->sp->sb->templateSelector).size() > 2 && (*tpx->sp->sb->templateSelector)[1].sp &&
                            !(*tpx->sp->sb->templateSelector)[1].isDeclType &&
                            !strcmp((*tpx->sp->sb->templateSelector)[1].sp->name, (*tpx->sp->sb->templateSelector)[2].name))
                        {
                            if (destructor)
                            {
                                *consdest = CT_DEST;
                            }
                            else
                            {
                                *consdest = CT_CONS;
                            }
                            if (strSym_out)
                                *strSym_out = (*tpx->sp->sb->templateSelector)[1].sp;
                            tn = (*tpx->sp->sb->templateSelector)[1].sp->tp;
                            *notype = true;
                            goto exit;
                        }
                        else
                        {
                            // discard template params, they've already been gathered..
                            TEMPLATESELECTOR* l = &(*tpx->sp->sb->templateSelector).back();
                            if (l->isTemplate)
                            {
                                if (MATCHKW(lex, Keyword::lt_))
                                {
                                    std::list<TEMPLATEPARAMPAIR>* current = nullptr;
                                    lex = GetTemplateArguments(lex, nullptr, nullptr, &current);
                                }
                            }
                        }
                    }
                    else
                    {
                        lex = getQualifiers(lex, &quals, &linkage, &linkage2, &linkage3, nullptr);
                        if (linkage != Linkage::none_)
                        {
                            *linkage_in = linkage;
                        }
                        if (linkage2 != Linkage::none_)
                            *linkage2_in = linkage2;
                        if (sp->sb->templateLevel)
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            if (templateNestingCount)
                                tn = sp->tp;
                            else
                                tn = nullptr;
                            if (MATCHKW(lex, Keyword::lt_))
                            {
                                if (sp->sb->parentTemplate)
                                    sp = sp->sb->parentTemplate;

                                lex = GetTemplateArguments(lex, funcsp, sp, &lst);
                                sp = GetClassTemplate(sp, lst, !templateErr);
                                if (sp)
                                {
                                    if (sp && (!templateNestingCount || inTemplateBody))
                                        sp->tp = PerformDeferredInitialization(sp->tp, funcsp);
                                }
                            }
                            else
                            {
                                if (!sp->sb->mainsym || (sp->sb->mainsym != strSym && sp->sb->mainsym != ssp))
                                {
                                    if (instantiatingMemberFuncClass &&
                                        instantiatingMemberFuncClass->sb->parentClass == sp->sb->parentClass)
                                        sp = instantiatingMemberFuncClass;
                                    else if (deduceTemplate)
                                        *deduceTemplate = MustSpecialize(sp->name);
                                    else
                                        SpecializationError(sp);
                                }
                            }
                            if (sp)
                                tn = sp->tp;
                        }
                        else
                        {
                            if (inTemplate)
                            {
                                if (MATCHKW(lex, Keyword::lt_))
                                {
                                    // throwaway
                                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                    lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                                }
                                errorsym(ERR_NOT_A_TEMPLATE, sp);
                            }
                            if (sp->tp->type == BasicType::typedef_)
                            {
                                tn = PerformDeferredInitialization(sp->tp, funcsp);
                                if (!Optimizer::cparams.prm_cplusplus)
                                    while (tn != basetype(tn) && tn->type != BasicType::va_list_)
                                        tn = tn->btp;
                            }
                            else
                            {
                                tn = sp->tp;
                            }
                            if (!templateNestingCount && isstructured(tn) && basetype(tn)->sp->sb->templateLevel &&
                                !basetype(tn)->sp->sb->instantiated)
                            {
                                sp = GetClassTemplate(basetype(tn)->sp, basetype(tn)->sp->templateParams, false);
                                if (sp)
                                    tn = PerformDeferredInitialization(sp->tp, funcsp);
                            }
                            //                        if (sp->sb->parentClass && !isAccessible(sp->sb->parentClass, ssp ? ssp :
                            //                        sp->sb->parentClass, sp, funcsp, ssp == sp->sb->parentClass ? AccessLevel::protected_ :
                            //                        AccessLevel::public_, false))
                            //                           errorsym(ERR_CANNOT_ACCESS, sp);
                        }
                        // DAL
                        if (Optimizer::cparams.prm_cplusplus && sp && MATCHKW(lex, Keyword::openpa_) &&
                            ((strSym && ((strSym->sb->mainsym && strSym->sb->mainsym == sp->sb->mainsym) ||
                                strSym == sp->sb->mainsym || sameTemplate(strSym->tp, sp->tp))) ||
                                (!strSym && (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_) && ssp &&
                                    ssp == sp->sb->mainsym)))
                        {
                            if (destructor)
                            {
                                *consdest = CT_DEST;
                            }
                            else
                            {
                                *consdest = CT_CONS;
                            }
                            if (ssp && strSym && ssp == strSym->sb->mainsym)
                                strSym = nullptr;
                            if (strSym_out)
                                *strSym_out = strSym;
                            *notype = true;
                            goto exit;
                        }
                        else if (destructor)
                        {
                            error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                        }
                    }
                }
            }
            else if (strSym && basetype(strSym->tp)->type == BasicType::templateselector_)
            {
                //                if (!templateNestingCount && !inTemplateSpecialization && allTemplateArgsSpecified(strSym,
                //                strSym->templateParams))
                //                    tn = SynthesizeType(strSym->tp, nullptr, false);
                //                else
                //                    tn = nullptr;
                if (!tn || tn->type == BasicType::any_ || basetype(tn)->type == BasicType::templateparam_)
                {
                    SYMBOL* sym = (*basetype(strSym->tp)->sp->sb->templateSelector)[1].sp;
                    if ((!templateNestingCount || instantiatingTemplate) && isstructured(sym->tp) && (sym->sb && sym->sb->instantiated && !declaringTemplate(sym) && (!sym->sb->templateLevel || allTemplateArgsSpecified(sym, (*strSym->tp->sp->sb->templateSelector)[1].templateParams))))
                    {

                        errorNotMember(sym, nsv ? nsv->front() : nullptr, (*strSym->tp->sp->sb->templateSelector)[2].name);
                    }
                    tn = strSym->tp;
                }
                else
                {
                    tn = PerformDeferredInitialization(tn, funcsp);
                }
                foundsomething = true;
                lex = getsym();
            }
            else if (strSym && basetype(strSym->tp)->type == BasicType::templatedecltype_)
            {
                tn = strSym->tp;
                foundsomething = true;
                lex = getsym();
            }
            else if (strSym && strSym->sb->templateLevel && !templateNestingCount)
            {
                STRUCTSYM s;
                tn = PerformDeferredInitialization(strSym->tp, funcsp);
                s.str = tn->sp;
                addStructureDeclaration(&s);
                sp = classsearch(lex->data->value.s.a, false, false, true);
                if (sp)
                {
                    tn = sp->tp;
                    foundsomething = true;
                }
                dropStructureDeclaration();
                lex = getsym();
            }
            else if (MATCHKW(lex, Keyword::decltype_))
            {
                lex = getDeclType(lex, funcsp, &tn);
                if (tn)
                    foundsomething = true;
            }
            else if (isTypedef)
            {
                if (!templateNestingCount && !typeName)
                {
                    error(ERR_TYPE_NAME_EXPECTED);
                }
                tn = MakeType(BasicType::any_);
                lex = getsym();
                foundsomething = true;
            }
            else if (Optimizer::cparams.prm_cplusplus)
            {
                if (typeName || ISID(lex))
                {
                    tn = MakeType(BasicType::any_);
                    if (lex->data->type == l_id)
                    {
                        SYMBOL* sp = makeID(StorageClass::global_, tn, nullptr, litlate(lex->data->value.s.a));
                        tn->sp = sp;
                    }
                    lex = getsym();
                    foundsomething = true;
                }
                else
                {
                    if (destructor)
                    {
                        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
                    }
                    lex = prevsym(placeholder);
                }
            }
        }
        if (!foundsomething && (Optimizer::cparams.c_dialect >= Dialect::c99 || Optimizer::cparams.prm_cplusplus))
        {
            if (notype)
                *notype = true;
            else
            {
                error(ERR_MISSING_TYPE_SPECIFIER);
            }
        }
    }
exit:
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && tn)
    {
        // select an unboxed variable type for use in compiler
        // will be converted back to boxed as needed
        TYPE* tnn = find_unboxed_type(tn);
        if (tnn)
        {
            tn = tnn;
            type = tn->type;
        }
    }
    if (Optimizer::cparams.c_dialect < Dialect::c99)
        switch (type)
        {
        case BasicType::bool_:
            if (!Optimizer::cparams.prm_cplusplus)
                RequiresDialect::Keyword(Dialect::c99, "_Bool");
            break;
        case BasicType::long_long_:
        case BasicType::unsigned_long_long_:
            if (!int64 && !Optimizer::cparams.prm_cplusplus)
                RequiresDialect::Keyword(Dialect::c99, "long long");
            break;
        case BasicType::float__complex_:
        case BasicType::double__complex_:
        case BasicType::long_double_complex_:
            RequiresDialect::Keyword(Dialect::c99, "_Complex");
            break;
        case BasicType::long_double_imaginary_:
        case BasicType::float__imaginary_:
        case BasicType::double__imaginary_:
            RequiresDialect::Keyword(Dialect::c99, "_Imaginary");
            break;
        default:
            break;
        }
    if (flagerror)
        error(ERR_TOO_MANY_TYPE_SPECIFIERS);
    if (!tn)
    {
        tn = MakeType(type);
        if (type == BasicType::bitint_ || type == BasicType::unsigned_bitint_)
        {
            tn->bitintbits = bitintbits;
            tn->size = (bitintbits + Optimizer::chosenAssembler->arch->bitintunderlying - 1);
            tn->size /= Optimizer::chosenAssembler->arch->bitintunderlying;
            tn->size *= Optimizer::chosenAssembler->arch->bitintunderlying;
            tn->size /= CHAR_BIT;
        }
    }
    if (quals)
    {
        tl = &quals;
        while (*tl)
        {
            (*tl)->size = tn->size;
            tl = &(*tl)->btp;
        }
        *tl = tn;
        tn = quals;
    }
    if (*tp)
    {
        tl = tp;
        while (*tl)
        {
            tl = &(*tl)->btp;
        }
        *tl = tn;
    }
    else
        *tp = tn;
    UpdateRootTypes(*tp);
    if (isatomic(*tp))
    {

        int sz = basetype(*tp)->size;
        if (needsAtomicLockFromType(*tp))
        {
            int n = getAlign(StorageClass::global_, &stdchar32tptr);
            n = n - (*tp)->size % n;
            if (n != 4)
            {
                sz += n;
            }
            sz += ATOMIC_FLAG_SPACE;
        }
        (*tp)->size = sz;
    }
    sizeQualifiers(*tp);
    return lex;
}
LEXLIST* getPointerQualifiers(LEXLIST* lex, TYPE** tp, bool allowstatic)
{
    while (KWTYPE(lex, TT_TYPEQUAL) || (allowstatic && MATCHKW(lex, Keyword::static_)))
    {
        TYPE* tpn;
        TYPE* tpl;
        if (MATCHKW(lex, Keyword::intrinsic_))
        {
            lex = getsym();
            continue;
        }
        tpn = Allocate<TYPE>();
        if (*tp)
            tpn->size = (*tp)->size;
        switch (KW(lex))
        {
        case Keyword::static_:
            tpn->type = BasicType::static_;
            break;
        case Keyword::const_:
            tpn->type = BasicType::const_;
            break;
        case Keyword::atomic_:
            RequiresDialect::Keyword(Dialect::c11, "_Atomic");
            lex = getsym();
            if (MATCHKW(lex, Keyword::openpa_))
            {
                // being used as  a type specifier not a qualifier
                lex = backupsym();
                return lex;
            }
            lex = backupsym();
            tpn->type = BasicType::atomic_;
            break;
        case Keyword::volatile_:
            tpn->type = BasicType::volatile_;
            break;
        case Keyword::restrict_:
            tpn->type = BasicType::restrict_;
            break;
        case Keyword::far_:
            tpn->type = BasicType::far_;
            break;
        case Keyword::near_:
            tpn->type = BasicType::near_;
            break;
        case Keyword::va_list_:
            tpn->type = BasicType::va_list_;
            break;
        default:
            break;
        }
        /*
                tpl = *tp;
                while (tpl && tpl->type != BasicType::pointer_)
                {
                    if (tpl->type == tpn->type)
                        errorstr(ERR_DUPLICATE_TYPE_QUALIFIER, lex->data->kw->name);
                    tpl = tpl->btp;
                }
        */
        tpn->btp = *tp;
        if (*tp)
            tpn->rootType = (*tp)->rootType;
        *tp = tpn;
        lex = getsym();
    }
    return lex;
}
LEXLIST* getFunctionParams(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** spin, TYPE** tp, bool inTemplate, StorageClass storage_class,
    bool funcptr)
{
    (void)storage_class;
    SYMBOL* sp = *spin;
    SYMBOL* spi;
    TYPE* tp1;
    bool isvoid = false;
    bool pastfirst = false;
    bool voiderror = false;
    bool hasellipse = false;
    LEXLIST* placeholder = lex;
    STRUCTSYM s;
    NAMESPACEVALUEDATA internalNS = {};
    SymbolTable<SYMBOL>* symbolTable = symbols.CreateSymbolTable();
    s.tmpl = nullptr;
    lex = getsym();
    if (*tp == nullptr)
        *tp = &stdint;
    tp1 = MakeType(BasicType::func_, *tp);
    tp1->size = getSize(BasicType::pointer_);
    tp1->sp = sp;
    sp->tp = *tp = tp1;
    localNameSpace->push_front(&internalNS);
    localNameSpace->front()->syms = tp1->syms = symbolTable;
    attributes oldAttribs = basisAttribs;

    basisAttribs = { 0 };
    ParseAttributeSpecifiers(&lex, funcsp, true);
    bool structured = false;
    if (startOfType(lex, &structured, true) && (!Optimizer::cparams.prm_cplusplus || resolveToDeclaration(lex, structured)) ||
        MATCHKW(lex, Keyword::constexpr_))
    {
        sp->sb->hasproto = true;
        while (startOfType(lex, nullptr, true) || MATCHKW(lex, Keyword::ellipse_) || MATCHKW(lex, Keyword::constexpr_))
        {
            if (MATCHKW(lex, Keyword::constexpr_))
            {
                lex = getsym();
                error(ERR_CONSTEXPR_NO_PARAM);
            }
            bool templType = inTemplateType;
            if (fullySpecialized)
                ++instantiatingTemplate;
            inTemplateType = !!templateNestingCount;
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, Linkage::none_);
                spi->tp = MakeType(BasicType::ellipse_);
                (*tp)->syms->Add(spi);
                lex = getsym();
                hasellipse = true;
            }
            else
            {
                StorageClass storage_class = StorageClass::parameter_;
                bool blocked;
                bool constexpression;
                Optimizer::ADDRESS address;
                TYPE* tpb;
                Linkage linkage = Linkage::none_;
                Linkage linkage2 = Linkage::none_;
                Linkage linkage3 = Linkage::none_;
                bool defd = false;
                bool notype = false;
                bool clonedParams = false;
                TYPE* tp2;
                spi = nullptr;
                tp1 = nullptr;

                noTypeNameError++;
                lex = getStorageAndType(lex, funcsp, nullptr, false, true, &storage_class, &storage_class, &address, &blocked,
                    nullptr, &constexpression, &tp1, &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd,
                    nullptr, nullptr, nullptr);
                noTypeNameError--;
                if (!basetype(tp1))
                    error(ERR_TYPE_NAME_EXPECTED);
                else if (isautotype(tp1) && !lambdas.size())
                    error(ERR_AUTO_NOT_ALLOWED_IN_PARAMETER);
                else if (Optimizer::cparams.prm_cplusplus && isstructured((*tp)->btp) && (MATCHKW(lex, Keyword::openpa_) || MATCHKW(lex, Keyword::begin_)))
                {
                    LEXLIST* cur = lex;
                    lex = getsym();
                    if (!MATCHKW(lex, Keyword::star_) && !MATCHKW(lex, Keyword::and_) && !startOfType(lex, nullptr, true))
                    {
                        if (*spin)
                        {
                            (*spin)->tp = (*tp) = (*tp)->btp;
                            // constructor initialization
                            // will do initialization later...
                        }
                        localNameSpace->pop_front();
                        lex = prevsym(placeholder);
                        return lex;
                    }
                    lex = prevsym(cur);
                }
                lex = getBeforeType(lex, funcsp, &tp1, &spi, nullptr, nullptr, false, storage_class, &linkage, &linkage2, &linkage3,
                    nullptr, false, false, false, false);
                if (!templateNestingCount && !structLevel)
                {
                    tp1 = PerformDeferredInitialization(tp1, funcsp);
                }
                if (!spi)
                {
                    spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
                    spi->sb->anonymous = true;
                    SetLinkerNames(spi, Linkage::none_);
                }
                spi->sb->parent = sp;
                tp1 = AttributeFinish(spi, tp1);
                tp2 = tp1;
                while (ispointer(tp2) || isref(tp2))
                    tp2 = basetype(tp2)->btp;
                tp2 = basetype(tp2);
                if (tp2->type == BasicType::templateparam_ && tp2->templateParam->second->packed)
                {
                    spi->packed = true;
                    if (spi->sb->anonymous && MATCHKW(lex, Keyword::ellipse_))
                    {
                        lex = getsym();
                    }
                }
                spi->tp = tp1;
                spi->sb->attribs.inheritable.linkage = linkage;
                spi->sb->attribs.inheritable.linkage2 = linkage2;
                if (spi->packed)
                {
                    checkPackedType(spi);
                    if (!templateNestingCount || (!funcptr && tp2->type == BasicType::templateparam_ && tp2->templateParam->first &&
                        !inCurrentTemplate(tp2->templateParam->first->name) &&
                        definedInTemplate(tp2->templateParam->first->name)))
                    {
                        if (tp2->templateParam && tp2->templateParam->second->packed)
                        {
                            TYPE* newtp1 = Allocate<TYPE>();
                            std::list<TEMPLATEPARAMPAIR>* templateParams = tp2->templateParam->second->byPack.pack;
                            auto tpp = Allocate < TEMPLATEPARAMPAIR>();
                            TEMPLATEPARAM* tpnew = Allocate<TEMPLATEPARAM>();
                            std::list<TEMPLATEPARAMPAIR>* newPack = templateParamPairListFactory.CreateList();
                            bool first = true;
                            *tpnew = *tp2->templateParam->second;
                            *tpp = TEMPLATEPARAMPAIR{ tp2->templateParam->first, tpnew };
                            *newtp1 = *tp2;
                            newtp1->templateParam = tpp;
                            tp1 = newtp1;
                            tp1->templateParam->second->byPack.pack = newPack;
                            tp1->templateParam->second->index = 0;
                            if (templateParams)
                            {
                                for (auto&& tpp : *templateParams)
                                {
                                    SYMBOL* clone = CopySymbol(spi);

                                    clone->tp = CopyType(clone->tp, true, [&tpp](TYPE*& old, TYPE*& newx) {
                                        if (old->type == BasicType::templateparam_)
                                        {
                                            old = tpp.second->byClass.val;
                                            *newx = *old;
                                        }
                                        });
                                    CollapseReferences(clone->tp);
                                    SetLinkerNames(clone, Linkage::none_);
                                    sizeQualifiers(clone->tp);
                                    if (!first)
                                    {
                                        clone->name = clone->sb->decoratedName = AnonymousName();
                                        clone->packed = false;
                                    }
                                    else
                                    {
                                        clone->synthesized = true;
                                        clone->tp->templateParam = tp1->templateParam;
                                    }
                                    auto second = Allocate<TEMPLATEPARAM>();
                                    *second = *tpp.second;
                                    second->packsym = clone;
                                    newPack->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
                                    (*tp)->syms->Add(clone);
                                    first = false;
                                    tp1->templateParam->second->index++;
                                    UpdateRootTypes(clone->tp);
                                }
                            }
                            else
                            {
                                SYMBOL* clone = CopySymbol(spi);
                                clone->tp = CopyType(clone->tp);
                                clone->tp->templateParam = tp1->templateParam;
                                SetLinkerNames(clone, Linkage::none_);
                                UpdateRootTypes(clone->tp);
                                sizeQualifiers(clone->tp);
                                (*tp)->syms->Add(clone);
                                UpdateRootTypes(clone->tp);
                            }
                            clonedParams = true;
                        }
                    }
                }
                if (!clonedParams)
                {
                    if (tp1 && isfunction(tp1))
                    {
                        tp1 = MakeType(BasicType::pointer_, tp1);
                    }
                    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::assign_))
                    {
                        if (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_ || structLevel ||
                            (templateNestingCount == 1 && !instantiatingTemplate))
                        {
                            lex = getDeferredData(lex, &spi->sb->deferredCompile, false);
                        }
                        else
                        {
                            TYPE* tp2 = spi->tp;
                            inDefaultParam++;
                            if (isref(tp2))
                                tp2 = basetype(tp2)->btp;
                            if (isstructured(tp2))
                            {
                                SYMBOL* sym;
                                anonymousNotAlloc++;
                                sym = anonymousVar(StorageClass::auto_, tp2)->v.sp;
                                anonymousNotAlloc--;
                                sym->sb->stackblock = !isref(spi->tp);
                                lex = initialize(lex, funcsp, sym, StorageClass::auto_, true, false, 0); /* also reserves space */
                                spi->sb->init = sym->sb->init;
                                if (spi->sb->init->front()->exp && spi->sb->init->front()->exp->type == ExpressionNode::thisref_)
                                {
                                    EXPRESSION** expr = &spi->sb->init->front()->exp->left->v.func->thisptr;
                                    if (*expr && (*expr)->type == ExpressionNode::add_ && isconstzero(&stdint, (*expr)->right))
                                        spi->sb->init->front()->exp->v.t.thisptr = (*expr) = (*expr)->left;
                                }
                            }
                            else
                            {
                                lex = initialize(lex, funcsp, spi, StorageClass::auto_, true, false, 0); /* also reserves space */
                            }
                            if (spi->sb->init)
                            {
                                checkDefaultArguments(spi);
                            }
                            inDefaultParam--;
                        }
                        spi->sb->defaultarg = true;
                        if (isfuncptr(spi->tp) && spi->sb->init && lvalue(spi->sb->init->front()->exp))
                            error(ERR_NO_POINTER_TO_FUNCTION_DEFAULT_ARGUMENT);
                        if (sp->sb->storage_class == StorageClass::typedef_)
                            error(ERR_NO_DEFAULT_ARGUMENT_IN_TYPEDEF);
                    }
                    SetLinkerNames(spi, Linkage::none_);
                    spi->tp = tp1;
                    (*tp)->syms->Add(spi);
                    tpb = basetype(tp1);
                    if (tpb->array)
                    {
                        if (tpb->vla)
                        {
                            auto tpx = MakeType(BasicType::pointer_, tpb);
                            TYPE* tpn = tpb;
                            tpx->size = tpb->size = getSize(BasicType::pointer_) + getSize(BasicType::unsigned_) * 2;
                            while (tpn->vla)
                            {
                                tpx->size += getSize(BasicType::unsigned_);
                                tpb->size += getSize(BasicType::unsigned_);
                                tpn->sp = spi;
                                tpn = tpn->btp;
                            }
                        }
                    }
                    if (tpb->type == BasicType::void_)
                        if (pastfirst || !spi->sb->anonymous)
                            voiderror = true;
                        else
                            isvoid = true;
                    else if (isvoid)
                        voiderror = true;
                }
            }
            if (fullySpecialized)
                --instantiatingTemplate;
            inTemplateType = templType;
            if (!MATCHKW(lex, Keyword::comma_) && (!Optimizer::cparams.prm_cplusplus || !MATCHKW(lex, Keyword::ellipse_)))
                break;
            if (MATCHKW(lex, Keyword::comma_))
                lex = getsym();
            pastfirst = true;
            basisAttribs = { 0 };
            ParseAttributeSpecifiers(&lex, funcsp, true);
        }
        if (!needkw(&lex, Keyword::closepa_))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
        // weed out temporary syms that were added as part of the default; they will be
        // reinstated as stackblock syms later
        for (SymbolTable<SYMBOL>::iterator it = (*tp)->syms->begin(); it != (*tp)->syms->end();)
        {
            SYMBOL* sym = *it;
            auto it1 = it;
            it1++;
            if (sym->sb->storage_class != StorageClass::parameter_)
                (*tp)->syms->remove(it);
            it = it1;

        }
    }
    else if (!Optimizer::cparams.prm_cplusplus && !(Optimizer::architecture == ARCHITECTURE_MSIL) && ISID(lex))
    {
        RequiresDialect::Removed(Dialect::c2x, "K&R prototypes");
        SYMBOL* spo = nullptr;
        sp->sb->oldstyle = true;
        if (sp->sb->storage_class != StorageClass::member_ && sp->sb->storage_class != StorageClass::mutable_)
        {
            spo = gsearch(sp->name);
            /* temporary for C */
            if (spo && spo->sb->storage_class == StorageClass::overloads_)
                spo = spo->tp->syms->front();
            if (spo && isfunction(spo->tp) && spo->sb->hasproto)
            {
                if (!comparetypes(spo->tp->btp, sp->tp->btp, true))
                {
                    preverrorsym(ERR_TYPE_MISMATCH_FUNC_DECLARATION, spo, spo->sb->declfile, spo->sb->declline);
                }
            }
        }
        while (ISID(lex) || MATCHKW(lex, Keyword::ellipse_))
        {
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                if (!pastfirst)
                    break;
                ;
                if (hasellipse)
                    break;
                if (isvoid)
                    voiderror = true;
                spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
                spi->sb->anonymous = true;
                SetLinkerNames(spi, Linkage::none_);
                spi->tp = MakeType(BasicType::ellipse_);
                lex = getsym();
                hasellipse = true;
                (*tp)->syms->Add(spi);
            }
            else
            {
                spi = makeID(StorageClass::parameter_, 0, 0, litlate(lex->data->value.s.a));
                SetLinkerNames(spi, Linkage::none_);
                (*tp)->syms->Add(spi);
            }
            lex = getsym();
            if (!MATCHKW(lex, Keyword::comma_))
                break;
            lex = getsym();
        }
        if (!needkw(&lex, Keyword::closepa_))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
        if (startOfType(lex, nullptr, false))
        {
            while (startOfType(lex, nullptr, false))
            {
                Optimizer::ADDRESS address;
                bool blocked;
                bool constexpression;
                Linkage linkage = Linkage::none_;
                Linkage linkage2 = Linkage::none_;
                Linkage linkage3 = Linkage::none_;
                StorageClass storage_class = StorageClass::parameter_;
                bool defd = false;
                bool notype = false;
                tp1 = nullptr;
                lex = getStorageAndType(lex, funcsp, nullptr, false, false, &storage_class, &storage_class, &address, &blocked,
                    nullptr, &constexpression, &tp1, &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd,
                    nullptr, nullptr, nullptr);

                while (1)
                {
                    TYPE* tpx = tp1;
                    spi = nullptr;
                    lex = getBeforeType(lex, funcsp, &tpx, &spi, nullptr, nullptr, false, StorageClass::parameter_, &linkage, &linkage2,
                        &linkage3, nullptr, false, false, false, false);
                    sizeQualifiers(tpx);
                    if (!spi || spi->sb->anonymous)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        errskim(&lex, skim_end);
                        break;
                    }
                    else
                    {
                        SYMBOL* spo;
                        TYPE* tpb;
                        spi->sb->attribs.inheritable.linkage = linkage;
                        spi->sb->attribs.inheritable.linkage2 = linkage2;
                        SetLinkerNames(spi, Linkage::none_);
                        if (tpx && isfunction(tpx))
                        {
                            tpx = MakeType(BasicType::pointer_, tpx);
                        }
                        spi->tp = tpx;
                        tpb = basetype(tpx);
                        if (tpb->array)
                        {
                            if (tpb->vla)
                            {
                                auto tpx = MakeType(BasicType::pointer_, tpb);
                                TYPE* tpn = tpb;
                                tpb = tpx;
                                tpx->size = tpb->size = getSize(BasicType::pointer_) + getSize(BasicType::unsigned_) * 2;
                                while (tpn->vla)
                                {
                                    tpx->size += getSize(BasicType::unsigned_);
                                    tpb->size += getSize(BasicType::unsigned_);
                                    tpn->sp = spi;
                                    tpn = tpn->btp;
                                }
                            }
                            else
                            {
                                //								tpb->array = false;
                                //								tpb->size = getSize(BasicType::pointer_);
                            }
                        }
                        sizeQualifiers(tpx);
                        spo = search((*tp)->syms, spi->name);
                        if (!spo)
                            errorsym(ERR_UNDEFINED_IDENTIFIER, spi);
                        else
                        {
                            spo->tp = tpx;
                        }
                        if (MATCHKW(lex, Keyword::comma_))
                        {
                            lex = getsym();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                needkw(&lex, Keyword::semicolon_);
            }
        }
        if ((*tp)->syms->size())
        {
            for (auto spi : *(*tp)->syms)
            {
                if (spi->tp == nullptr)
                {
                    if (Optimizer::cparams.c_dialect >= Dialect::c99)
                        errorsym(ERR_MISSING_TYPE_FOR_PARAMETER, spi);
                    spi->tp = MakeType(BasicType::int_);
                }
            }
        }
        else if (spo)
            (*tp)->syms = spo->tp->syms;
        if (!MATCHKW(lex, Keyword::begin_))
            error(ERR_FUNCTION_BODY_EXPECTED);
    }
    else if (MATCHKW(lex, Keyword::ellipse_))
    {
        spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
        spi->sb->anonymous = true;
        SetLinkerNames(spi, Linkage::none_);
        spi->tp = MakeType(BasicType::ellipse_);
        (*tp)->syms->Add(spi);
        lex = getsym();
        if (!MATCHKW(lex, Keyword::closepa_))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
        skip(&lex, Keyword::closepa_);
    }
    else if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c2x || ((Optimizer::architecture == ARCHITECTURE_MSIL) &&
        Optimizer::cparams.msilAllowExtensions && !MATCHKW(lex, Keyword::closepa_) && *spin))
    {
        // () is a function
        if (MATCHKW(lex, Keyword::closepa_))
        {
            spi = makeID(StorageClass::parameter_, tp1, nullptr, NewUnnamedID());
            spi->sb->anonymous = true;
            spi->sb->attribs.inheritable.structAlign = getAlign(StorageClass::parameter_, &stdpointer);
            SetLinkerNames(spi, Linkage::none_);
            spi->tp = MakeType(BasicType::void_);
            (*tp)->syms->Add(spi);
            lex = getsym();
        }
        // else may have a constructor
        else if (*spin)
        {
            (*spin)->tp = (*tp) = (*tp)->btp;
            // constructor initialization
            lex = backupsym();
            // will do initialization later...
        }
        else
        {
            currentErrorLine = 0;
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
    }
    else
    {
        SYMBOL* spo;
        if (sp->sb->storage_class != StorageClass::member_ && sp->sb->storage_class != StorageClass::mutable_)
        {
            spo = gsearch(sp->name);
            if (spo && spo->sb->storage_class == StorageClass::overloads_)
            {

                if (spo->tp->syms)
                {
                    spo = spo->tp->syms->front();
                    auto symtab = spo->tp->syms;
                    if (symtab && symtab->size())
                    {
                        auto sp2 = symtab->front();
                        if (sp2->tp->type == BasicType::void_)
                        {
                            (*tp)->syms = symtab;
                        }
                    }
                }
            }
        }
        if (!MATCHKW(lex, Keyword::closepa_))
        {
            error(ERR_FUNCTION_PARAMETER_EXPECTED);
            errskim(&lex, skim_closepa);
        }
        skip(&lex, Keyword::closepa_);
    }
    localNameSpace->pop_front();
    basisAttribs = oldAttribs;
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (voiderror)
        error(ERR_VOID_ONLY_PARAMETER);
    return lex;
}
LEXLIST* get_type_id(LEXLIST* lex, TYPE** tp, SYMBOL* funcsp, StorageClass storage_class, bool beforeOnly, bool toErr, bool inUsing)
{
    Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    bool defd = false;
    SYMBOL* sp = nullptr;
    SYMBOL* strSym = nullptr;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    bool notype = false;
    bool oldTemplateType = inTemplateType;
    *tp = nullptr;

    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3, nullptr);
    lex = getBasicType(lex, funcsp, tp, nullptr, false, funcsp ? StorageClass::auto_ : StorageClass::global_, &linkage, &linkage2, &linkage3, AccessLevel::public_,
                       &notype, &defd, nullptr, nullptr, nullptr, false, false, inUsing, false, false);
    lex = getQualifiers(lex, tp, &linkage, &linkage2, &linkage3, nullptr);
    lex = getBeforeType(lex, funcsp, tp, &sp, &strSym, &nsv, false, storage_class, &linkage, &linkage2, &linkage3, &notype, false,
                        false, beforeOnly, false); /* fixme at file scope init */
    sizeQualifiers(*tp);
    if (notype)
        *tp = nullptr;
    else if (sp && !sp->sb->anonymous && toErr)
        if (sp->tp->type != BasicType::templateparam_)
            error(ERR_TOO_MANY_IDENTIFIERS);
    if (sp && sp->sb->anonymous && linkage != Linkage::none_)
    {
        if (sp->sb->attribs.inheritable.linkage != Linkage::none_)
            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
        else
            sp->sb->attribs.inheritable.linkage = linkage;
    }
    inTemplateType = oldTemplateType;
    return lex;
}
bool istype(SYMBOL* sym)
{
    if (!sym->sb || sym->sb->storage_class == StorageClass::templateparam_)
    {
        return sym->tp->templateParam->second->type == TplType::typename_ || sym->tp->templateParam->second->type == TplType::template_;
    }
    return (sym->tp->type != BasicType::templateselector_ && sym->sb->storage_class == StorageClass::type_) || sym->sb->storage_class == StorageClass::typedef_;
}
bool startOfType(LEXLIST* lex, bool* structured, bool assumeType)
{
    if (structured)
        *structured = false;
    auto old = lines;
    if (!lex)
        return false;

    if (lex->data->type == l_id)
    {
        auto tparam = TemplateLookupSpecializationParam(lex->data->value.s.a);
        if (tparam)
        {
            LEXLIST* placeHolder = lex;
            bool member;
            lex = getsym();
            member = MATCHKW(lex, Keyword::classsel_);
            if (member)
            {
                lex = getsym();
                member = MATCHKW(lex, Keyword::star_);
            }
            lex = prevsym(placeHolder);
            if (!member)
            {
                lines = old;
                return tparam->second->type == TplType::typename_ || tparam->second->type == TplType::template_;
            }
        }
    }
    if (lex->data->type == l_id || MATCHKW(lex, Keyword::classsel_) || MATCHKW(lex, Keyword::decltype_))
    {
        bool isdecltype = MATCHKW(lex, Keyword::decltype_);
        SYMBOL *sym, *strSym = nullptr;
        LEXLIST* placeholder = lex;
        bool dest = false;
        nestedSearch(lex, &sym, &strSym, nullptr, &dest, nullptr, false, StorageClass::global_, false, false);
        if (Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL))
            prevsym(placeholder);
        lines = old;
        if (structured && sym && istype(sym))
        {
            if (sym->tp->type == BasicType::templateparam_)
            {
                if (sym->tp->templateParam->second->type == TplType::typename_ && sym->tp->templateParam->second->byClass.val)
                    *structured = isstructured(sym->tp->templateParam->second->byClass.val);
                else if (sym->tp->templateParam->second->type == TplType::template_)
                    *structured = true;
            }
            else
            {
                *structured = isstructured(sym->tp);
            }
        }
        return (!sym && isdecltype) || (sym && sym->tp && istype(sym)) ||
               (assumeType && strSym && (strSym->tp->type == BasicType::templateselector_ || strSym->tp->type == BasicType::templatedecltype_));
    }
    else
    {
        lines = old;
        return KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS | TT_TYPENAME);
    }
}
static TYPE* rootType(TYPE* tp)
{
    while (tp)
    {
        switch (tp->type)
        {
            case BasicType::far_:
            case BasicType::near_:
            case BasicType::const_:
            case BasicType::va_list_:
            case BasicType::objectArray_:
            case BasicType::volatile_:
            case BasicType::restrict_:
            case BasicType::static_:
            case BasicType::atomic_:
            case BasicType::typedef_:
            case BasicType::lrqual_:
            case BasicType::rrqual_:
            case BasicType::derivedfromtemplate_:
                tp = tp->btp;
                break;
            default:
                return tp;
        }
    }
    return nullptr;
}
void UpdateRootTypes(TYPE* tp)
{
    while (tp)
    {
        TYPE* tp1 = rootType(tp);
        while (tp && tp1 != tp)
        {
            tp->rootType = tp1;
            tp = tp->btp;
        }
        if (tp)
        {
            tp->rootType = tp;
            tp = tp->btp;
        }
    }
}

static TYPE* replaceTemplateSelector(TYPE* tp)
{
    if (!templateNestingCount && tp->type == BasicType::templateselector_ && (*tp->sp->sb->templateSelector)[1].isTemplate)
    {
        SYMBOL* sp2 = (*tp->sp->sb->templateSelector)[1].sp;
        if ((*tp->sp->sb->templateSelector)[1].isDeclType)
        {
            TYPE* tp1 = TemplateLookupTypeFromDeclType((*tp->sp->sb->templateSelector)[1].tp);
            if (tp1 && isstructured(tp1))
                sp2 = basetype(tp1)->sp;
            else
                sp2 = nullptr;
        }
        else
        {
            if (sp2)
            {
                SYMBOL* sp1 = GetClassTemplate(sp2, (*tp->sp->sb->templateSelector)[1].templateParams, true);
                if (sp1)
                {
                    sp1 = search(sp1->tp->syms, (*tp->sp->sb->templateSelector)[2].name);
                    if (sp1)
                    {
                        tp = sp1->tp;
                    }
                }
            }
        }
    }
    return tp;
}
bool comparetypes(TYPE* typ1, TYPE* typ2, int exact)
{
    if (typ1->type == BasicType::any_ || typ2->type == BasicType::any_)
        return true;
    while (typ1->type == BasicType::typedef_)
        typ1 = basetype(typ1);
    while (typ2->type == BasicType::typedef_)
        typ2 = basetype(typ2);
    if (typ1->type == BasicType::derivedfromtemplate_)
        typ1 = typ1->btp;
    if (typ2->type == BasicType::derivedfromtemplate_)
        typ2 = typ2->btp;
    while (isref(typ1))
        typ1 = basetype(typ1)->btp;
    while (isref(typ2))
        typ2 = basetype(typ2)->btp;
    while (typ1->type == BasicType::typedef_)
        typ1 = basetype(typ1);
    while (typ2->type == BasicType::typedef_)
        typ2 = basetype(typ2);
    if (typ1->type == BasicType::templateselector_ && typ2->type == BasicType::templateselector_)
        return templateselectorcompare(typ1->sp->sb->templateSelector, typ2->sp->sb->templateSelector);
    if (typ1->type == BasicType::templatedecltype_ && typ2->type == BasicType::templatedecltype_)
        return templatecompareexpressions(typ1->templateDeclType, typ2->templateDeclType);
    if (ispointer(typ1) && ispointer(typ2))
    {
        if (exact)
        {
            int arr = false;
            int first = true;
            while (ispointer(typ1) && ispointer(typ2))
            {
                if (!first && (exact == 1))
                    if ((isconst(typ2) && !isconst(typ1)) || (isvolatile(typ2) && !isvolatile(typ1)))
                        return false;
                first = false;
                typ1 = basetype(typ1);
                typ2 = basetype(typ2);
                if (typ1->type != typ2->type)
                    return false;
                if (typ1->msil != typ2->msil)
                    return false;
                if (arr && (typ1->array != typ2->array))
                    return false;
                if (arr && typ1->size != typ2->size)
                    return false;
                arr |= typ1->array | typ2->array;
                typ1 = typ1->btp;
                typ2 = typ2->btp;
            }
            if (exact == 1 && ((isconst(typ2) && !isconst(typ1)) || (isvolatile(typ2) && !isvolatile(typ1))))
                return false;
            return comparetypes(typ1, typ2, true);
        }

        else
            return true;
    }
    typ1 = basetype(typ1);
    typ2 = basetype(typ2);
    if (exact && (isfunction(typ1) || isfuncptr(typ1)) && (isfunction(typ2) || isfuncptr(typ2)))
    {
        typ1 = basetype(typ1);
        typ2 = basetype(typ2);
        if (ispointer(typ1))
            typ1 = basetype(typ1)->btp;
        if (ispointer(typ2))
            typ2 = basetype(typ2)->btp;
        if (isref(typ1->btp) != isref(typ2->btp) || !comparetypes(typ1->btp, typ2->btp, exact))
            return false;
        if (!matchOverload(typ1, typ2, true))
            return false;
        return true;
    }
    if (Optimizer::cparams.prm_cplusplus)
    {
        if (typ1->scoped != typ2->scoped)
            return false;
        if (typ1->type == BasicType::enum_)
        {
            if (typ2->type == BasicType::enum_)
                return typ1->sp == typ2->sp;
            else
                return isint(typ2);
        }
        else if (typ2->type == BasicType::enum_)
        {
            return isint(typ1);
        }
        if (typ1->type == typ2->type && typ1->type == BasicType::memberptr_)
        {
            if (typ1->sp != typ2->sp)
            {
                if (classRefCount(typ1->sp, typ2->sp) != 1)
                    return false;
            }
            return comparetypes(typ1->btp, typ2->btp, exact);
        }
    }
    if (typ1->type == typ2->type && typ1->type == BasicType::string_)
        return true;
    if (typ1->type == BasicType::object_)  // object matches anything
        return true;
    if (typ1->type == typ2->type && (isstructured(typ1) || (exact && typ1->type == BasicType::enum_)))
    {
        SYMBOL* s1 = typ1->sp;
        SYMBOL* s2 = typ2->sp;
        if (s1->sb && s1->sb->mainsym)
            s1 = s1->sb->mainsym;
        if (s2->sb && s2->sb->mainsym)
            s2 = s2->sb->mainsym;
        return s1 == s2;
    }
    if (typ1->type == typ2->type || (!exact && isarithmetic(typ2) && isarithmetic(typ1)))
        return typ1->bitintbits == typ2->bitintbits;
    if (isfunction(typ1) && isfunction(typ2) &&
        typ1->sp->sb->attribs.inheritable.linkage == typ2->sp->sb->attribs.inheritable.linkage)
        return true;
    else if (!exact && ((ispointer(typ1) && (isfuncptr(typ2) || isfunction(typ2) || isint(typ2))) ||
                        (ispointer(typ2) && (isfuncptr(typ1) || isfunction(typ1) || isint(typ1)))))
        return (true);
    else if (typ1->type == BasicType::enum_ && isint(typ2))
    {
        return true;
    }
    else if (typ2->type == BasicType::enum_ && isint(typ1))
    {
        return true;
    }
    return false;
}
// this was introduced in c++17, but, since it is a major error if you do this I'm just importing it into all C++ dialects...
bool compareXC(TYPE* typ1, TYPE* typ2)
{
    if (isfuncptr(typ1))
    {
        if (isfunction(typ2) || isfuncptr(typ2))
        {
            if (isfunction(typ2))
            {
                parseNoexcept(basetype(typ2)->sp);
            }
            while (ispointer(typ2))
            {
                typ2 = basetype(typ2)->btp;
            }
            while (ispointer(typ1))
            {
                typ1 = basetype(typ1)->btp;
            }
            typ1 = basetype(typ1);
            typ2 = basetype(typ2);
            if (isfunction(typ1))
            {
                parseNoexcept(typ1->sp);
            }
            if (isfunction(typ2))
            {
                parseNoexcept(typ2->sp);
            }
            if (typ1->sp->sb->noExcept && !typ2->sp->sb->noExcept)
                return false;
        }
    }
    return true;
}
bool matchingCharTypes(TYPE* typ1, TYPE* typ2)
{
    if (isref(typ1))
        typ1 = basetype(typ1)->btp;
    if (isref(typ2))
        typ2 = basetype(typ2)->btp;

    while (ispointer(typ1) && ispointer(typ2))
    {
        typ1 = basetype(typ1)->btp;
        typ2 = basetype(typ2)->btp;
    }
    typ1 = basetype(typ1);
    typ2 = basetype(typ2);
    if (typ1->type == BasicType::char_)
    {
        if (Optimizer::cparams.prm_charisunsigned)
        {
            if (typ2->type == BasicType::unsigned_char_)
                return true;
        }
        else
        {
            if (typ2->type == BasicType::signed_char_)
                return true;
        }
    }
    else if (typ2->type == BasicType::char_)
    {
        if (Optimizer::cparams.prm_charisunsigned)
        {
            if (typ1->type == BasicType::unsigned_char_)
                return true;
        }
        else
        {
            if (typ1->type == BasicType::signed_char_)
                return true;
        }
    }
    else if (typ1->type == BasicType::unsigned_short_ || typ1->type == BasicType::wchar_t_)
        return typ2->type == BasicType::unsigned_short_ || typ2->type == BasicType::wchar_t_;
    return false;
}
static char* putpointer(char* p, TYPE* tp)
{
    *p = 0;
    if (tp->type == BasicType::far_)
        Optimizer::my_sprintf(p, "far ");
    p = p + strlen(p);
    if (tp->array)
        if (tp->btp->size && (!tp->esize || tp->esize->type == ExpressionNode::c_i_))
        {
            Optimizer::my_sprintf(p, "[%d]", tp->size / tp->btp->size);
        }
        else
        {
            Optimizer::my_sprintf(p, "[]");
        }
    else if (tp->vla)
        Optimizer::my_sprintf(p, "[*]");
    else
        Optimizer::my_sprintf(p, " *");
    return p + strlen(p);
}

static TYPE* enumConst(char* buf, TYPE* tp)
{
    while (tp && (isconst(tp) || isvolatile(tp) || isrestrict(tp) || tp->type == BasicType::derivedfromtemplate_))
    {
        switch (tp->type)
        {
            case BasicType::lrqual_:
                strcat(buf, "& ");
                break;
            case BasicType::rrqual_:
                strcat(buf, "&& ");
                break;
            case BasicType::const_:
                strcat(buf, tn_const);
                break;
            case BasicType::volatile_:
                strcat(buf, tn_volatile);
                break;
            case BasicType::restrict_:
            case BasicType::derivedfromtemplate_:
                /*				strcat(buf, tn_restrict); */
                break;
            default:
                break;
        }
        tp = tp->btp;
    }
    return tp;
}
void typenumptr(char* buf, TYPE* tp)
{
    char bf[256], *p = bf;
    p = putpointer(p, tp);
    tp = enumConst(buf, tp->btp);
    if (!tp)
        return;
    while (ispointer(tp))
    {
        p = putpointer(p, tp);
        tp = enumConst(buf, tp->btp);
        //		tp = tp->btp;
    }
    typenum(buf, tp);
    strcat(buf, bf);
}
void RenderExpr(char* buf, EXPRESSION* exp)
{
    (void)exp;
    strcpy(buf, "decltype(...)");
}
TYPE* typenum(char* buf, TYPE* tp)
{
    SYMBOL* sym;
    char name[4096];
    if (tp == nullptr)
    {
        diag("typenum - nullptr type");
        return &stdvoid;
    }
    if (tp->type == BasicType::derivedfromtemplate_)
        tp = tp->btp;
    tp = enumConst(buf, tp);
    if (!tp)
        return nullptr;
    buf += strlen(buf);
    switch (tp->type)
    {
        case BasicType::typedef_:
            return typenum(buf, tp->btp);
            break;
        case BasicType::aggregate_:
            if (!tp->syms)
                break;
            sym = tp->syms->front();
            if (tp->syms->size() > 1 ||
                !strcmp(sym->name, tp->sp->name))  // the tail is to prevent a problem when there are a lot of errors
            {
                strcpy(buf, " (*)(\?\?\?)");
                break;
            }
            tp = sym->tp;
            /* fall through */
        case BasicType::func_:
        case BasicType::ifunc_:
            typenum(buf, tp->btp);
            buf = buf + strlen(buf);
            if (tp->syms)
            {
                auto it = tp->syms->begin();
                if (it != tp->syms->end() && *it)
                {
                    if ((*it)->sb->thisPtr)
                    {
                        SYMBOL* thisptr = *it;
                        *buf++ = ' ';
                        *buf++ = '(';
                        getcls(buf, basetype(basetype(thisptr->tp)->btp)->sp);
                        strcat(buf, "::*)(");
                        buf += strlen(buf);
                        ++it;
                    }
                    else
                    {
                        strcat(buf, " (*)(");
                        buf += strlen(buf);
                    }
                }
                else
                {
                    strcat(buf, " (*)(");
                    buf += strlen(buf);
                }
                bool cleanup = it != tp->syms->end();
                while (it != tp->syms->end())
                {
                    sym = *it;
                    *buf = 0;
                    typenum(buf, sym->tp);
                    buf = buf + strlen(buf);
                    *buf++ = ',';
                    ++it;
                }
                if (cleanup)
                    buf--;
            }
            else
            {
                strcat(buf, " (*)(");
                buf += strlen(buf);
            }
            *buf++ = ')';
            *buf = 0;
            break;
        case BasicType::float__complex_:
            strcpy(buf, tn_floatcomplex);
            break;
        case BasicType::double__complex_:
            strcpy(buf, tn_doublecomplex);
            break;
        case BasicType::long_double_complex_:
            strcpy(buf, tn_longdoublecomplex);
            break;
        case BasicType::float__imaginary_:
            strcpy(buf, tn_floatimaginary);
            break;
        case BasicType::double__imaginary_:
            strcpy(buf, tn_doubleimaginary);
            break;
        case BasicType::long_double_imaginary_:
            strcpy(buf, tn_longdoubleimaginary);
            break;
        case BasicType::float_:
            strcpy(buf, tn_float);
            break;
        case BasicType::double_:
            strcpy(buf, tn_double);
            break;
        case BasicType::long_double_:
            strcpy(buf, tn_longdouble);
            break;
        case BasicType::unsigned_:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::int_:
            strcpy(buf, tn_int);
            break;
        case BasicType::char8_t_:
            strcpy(buf, tn_char8_t);
            break;
        case BasicType::char16_t_:
            strcpy(buf, tn_char16_t);
            break;
        case BasicType::char32_t_:
            strcpy(buf, tn_char32_t);
            break;
        case BasicType::unsigned_long_long_:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::long_long_:
            strcpy(buf, tn_longlong);
            break;
        case BasicType::unsigned_long_:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::long_:
            strcpy(buf, tn_long);
            break;
        case BasicType::wchar_t_:
            strcpy(buf, tn_wchar_t);
            break;
        case BasicType::unsigned_short_:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::short_:
            strcpy(buf, tn_short);
            break;
        case BasicType::signed_char_:
            strcpy(buf, tn_signed);
            buf = buf + strlen(buf);
            strcpy(buf, tn_char);
            break;
        case BasicType::unsigned_char_:
            strcpy(buf, tn_unsigned);
            buf = buf + strlen(buf);
        case BasicType::char_:
            strcpy(buf, tn_char);
            break;
        case BasicType::bool_:
            strcpy(buf, tn_bool);
            break;
        case BasicType::bit_:
            strcpy(buf, "bit");
            break;
        case BasicType::inative_:
            strcpy(buf, "native int");
            break;
        case BasicType::unative_:
            strcpy(buf, "native unsigned int");
            break;
        case BasicType::bitint_:
            strcpy(buf, "_Bitint");
            sprintf(buf + strlen(buf), "(%d)", tp->bitintbits);
            break;
        case BasicType::unsigned_bitint_:
            strcpy(buf, "unsigned _Bitint");
            sprintf(buf + strlen(buf), "(%d)", tp->bitintbits);
            break;
        case BasicType::void_:
            strcpy(buf, tn_void);
            break;
        case BasicType::string_:
            strcpy(buf, "__string");
            break;
        case BasicType::object_:
            strcpy(buf, "__object");
            break;
        case BasicType::pointer_:
            if (tp->nullptrType)
            {
                strcpy(buf, "nullptr_t");
            }
            else
            {
                typenumptr(buf, tp);
            }
            break;
        case BasicType::memberptr_:
            if (isfunction(basetype(tp)->btp))
            {
                TYPE* func = basetype(tp)->btp;
                typenum(buf, basetype(func)->btp);
                strcat(buf, " (");
                buf += strlen(buf);
                getcls(buf, tp->sp);
                buf += strlen(buf);
                strcpy(buf, "::*)(");
                buf += strlen(buf);
                if (basetype(func)->syms)
                {
                    for (auto sym : *basetype(func)->syms)
                    {
                        *buf = 0;
                        typenum(buf, sym->tp);
                        buf = buf + strlen(buf);
                    }
                    if (basetype(func)->syms->size())
                        buf--;
                }
                *buf++ = ')';
                *buf = 0;
            }
            else
            {
                typenum(buf, tp->btp);
                strcat(buf, " ");
                buf += strlen(buf);
                getcls(buf, tp->sp);
                buf += strlen(buf);
                strcpy(buf, "::*");
            }
            break;
        case BasicType::seg_:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            strcpy(buf, " _seg *");
            break;
        case BasicType::lref_:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf = 0;
            break;
        case BasicType::rref_:
            typenum(buf, tp->btp);
            buf += strlen(buf);
            *buf++ = ' ';
            *buf++ = '&';
            *buf++ = '&';
            *buf = 0;
            break;
        case BasicType::ellipse_:
            strcpy(buf, tn_ellipse);
            break;
        case BasicType::any_:
            strcpy(buf, "???");
            break;
        case BasicType::class_:
            /*                strcpy(buf, tn_class); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::struct_:
            /*                strcpy(buf, tn_struct); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::union_:
            /*                strcpy(buf, tn_union); */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::enum_:
            /*                strcpy(buf, tn_enum);  */
            unmangle(name, tp->sp->sb->decoratedName ? tp->sp->sb->decoratedName : tp->sp->name);
            strcpy(buf, name);
            break;
        case BasicType::templateselector_: {
            auto itts = tp->sp->sb->templateSelector->begin();
            ++itts;
            if (itts->sp)
            {
                strcpy(buf, itts->sp->name);
                for (++itts; itts != tp->sp->sb->templateSelector->end(); ++itts)
                {
                    strcat(buf, "::");
                    strcat(buf, itts->name);
                }
            }
            break;
        }
        case BasicType::templatedecltype_:
            RenderExpr(buf, tp->templateDeclType);
            break;
        case BasicType::auto_:
            strcpy(buf, "auto ");
            break;
        default:
            strcpy(buf, "\?\?\?");
    }
    return 0;
}
void xcTypeToString(char* buf, TYPE* typ)
{
    if (isfunction(typ) || isfuncptr(typ))
    {
        while (ispointer(typ))
            typ = basetype(typ)->btp;
        typ = basetype(typ);
        switch (typ->sp->sb->xcMode)
        {
            case xc_all:
                strcpy(buf, " noexcept(false)");
                break;
            case xc_none:
                strcpy(buf, " noexcept");
                break;
            case xc_dynamic:
                strcpy(buf, " throw(...)");
                break;
            case xc_unspecified:
                break;
        }
    }
}
void typeToString(char* buf, TYPE* typ)
{
    auto typ2 = typ;
    *buf = 0;
    while (typ)
    {
        typ = typenum(buf, typ);
        buf = buf + strlen(buf);
        if (typ)
            *buf++ = ',';
    }
    *buf = 0;
    xcTypeToString(buf, typ2);
}
}  // namespace Parser