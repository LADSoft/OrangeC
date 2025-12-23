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
#    include "AsmLexer.h"
#endif
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "inline.h"
#include "expreval.h"
#include "class.h"
#include "overload.h"
#include "exprpacked.h"
#include "SymbolProperties.h"
#include "staticassert.h"
#include "casts.h"
#include "attribs.h"

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
int argumentNestingLevel;
int assigningRHS;

std::list<SYMBOL*> importThunks;
static SYMBOL* msilToString;

std::list<StringData*> strtab;
/* lvaule */
/* handling of const int */
/*--------------------------------------------------------------------------------------------------------------------------------
 */
static void expression_primary( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable,
                                   int flags);
void expression_unary( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
static void expression_pm( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
void expression_assign( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags);
static void expression_msilfunc( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags);

std::unordered_map<std::string, unsigned, StringHash> cattributes = {
    {"deprecated", 202311}, {"fallthrough", 202311}, {"nodiscard", 202311}, {"noreturn", 202311}, {"maybe_unused", 202311}};

void expr_init(void)
{
    importThunks.clear();
    inGetUserConversion = 0;
    currentlyInsertingFunctions = 0;
    inNothrowHandler = 0;
    argFriend = nullptr;
    argumentNestingLevel = 0;
    strtab.clear();
}
void thunkForImportTable(EXPRESSION** exp)
{
    SYMBOL* sym;
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
        return;
    if ((*exp)->type == ExpressionNode::pc_)
        sym = (*exp)->v.sp;
    else
        sym = (*exp)->v.func->sp;
    // order is important here as we might get into this function with an already sanitized symbol
    if (sym && sym->sb->attribs.inheritable.linkage2 == Linkage::import_ && sym->tp->IsFunction() &&
        ((*exp)->type == ExpressionNode::pc_ || !(*exp)->v.func->ascall))
    {
        bool found = false;
        for (auto search : importThunks)
        {
            if (search->sb->mainsym == sym)
            {
                *exp = MakeExpression(ExpressionNode::pc_, (SYMBOL*)search);
                found = true;
                break;
            }
        }
        if (!found)
        {
            SYMBOL* newThunk;
            char buf[2048];
            Optimizer::my_sprintf(buf, "@@%s", sym->sb->decoratedName);
            newThunk = makeID(StorageClass::global_, &stdpointer, nullptr, litlate(buf));
            newThunk->sb->decoratedName = newThunk->name;
            newThunk->sb->mainsym = sym;  // mainsym is the symbol this was derived from
            newThunk->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
            newThunk->sb->importThunk = true;
            importThunks.push_back(newThunk);
            *exp = MakeExpression(ExpressionNode::pc_, newThunk);
            Optimizer::SymbolManager::Get(newThunk)->generated = true;
            Optimizer::EnterExternal(Optimizer::SymbolManager::Get(sym));
        }
    }
}
static SYMBOL* LookupMsilToString()
{
    if (!msilToString)
    {
        SYMBOL* sym = namespacesearch("lsmsilcrtl", globalNameSpace, false, false);
        if (sym && sym->sb->storage_class == StorageClass::namespace_)
        {
            sym = namespacesearch("CString", sym->sb->nameSpaceValues, true, false);
            if (sym && sym->tp->IsStructured())
            {
                sym = search(sym->tp->BaseType()->syms, "ToPointer");
                if (sym)
                {
                    for (auto sp : *sym->tp->syms)
                    {
                        if (sp->sb->storage_class == StorageClass::static_)
                        {
                            msilToString = sp;
                            break;
                        }
                    }
                }
            }
        }
        if (!msilToString)
        {
            Utils::Fatal("internal error");
        }
    }
    return msilToString;
}
EXPRESSION* ConvertToMSILString(EXPRESSION* val)
{
    val->type = ExpressionNode::c_string_;
    SYMBOL* var = LookupMsilToString();

    CallSite* fp = Allocate<CallSite>();
    fp->functp = var->tp;
    fp->sp = var;
    fp->fcall = MakeExpression(ExpressionNode::global_, var);
    fp->arguments = argumentListFactory.CreateList();
    auto arg = Allocate<Argument>();
    arg->exp = val;
    arg->tp = &std__string;
    fp->arguments->push_back(arg);
    fp->ascall = true;
    EXPRESSION* rv = MakeExpression(fp);
    return rv;
}
EXPRESSION* stringlit(StringData* s)
/*
 *      make s a string literal and return it's label number.
 */
{
    EXPRESSION* rv;
    if (Optimizer::cparams.prm_mergestrings)
    {
        for (auto lp : strtab)
        {
            int i;
            if (s->size == lp->size && s->strtype == lp->strtype)
            {
                /* but it won't get in here if s and lp are the same, but
                 * resulted from different concatenation sequences
                 * this whole behavior of using strings over is undefined
                 * in the standard...
                 */
                for (i = 0; i < s->size && i < lp->size; i++)
                {
                    if (lp->pointers[i]->count != s->pointers[i]->count ||
                        Optimizer::wchart_cmp(lp->pointers[i]->str, s->pointers[i]->str, s->pointers[i]->count))
                        break;
                }
                if (i >= s->size)
                {
                    rv = MakeIntExpression(ExpressionNode::labcon_, lp->label);
                    rv->string = s;
                    rv->size = Type::MakeType(BasicType::struct_);
                    rv->altdata = MakeIntExpression(ExpressionNode::c_i_, (int)s->strtype);
                    lp->refCount++;
                    if (Optimizer::msilstrings)
                    {
                        rv = ConvertToMSILString(rv);
                    }
                    return rv;
                }
            }
        }
    }
    s->label = Optimizer::nextLabel++;
    strtab.push_back(s);
    rv = MakeIntExpression(ExpressionNode::labcon_, s->label);
    rv->string = s;
    rv->size = Type::MakeType(BasicType::struct_);
    rv->altdata = MakeIntExpression(ExpressionNode::c_i_, (int)s->strtype);
    s->refCount++;
    if (Optimizer::msilstrings)
    {
        rv = ConvertToMSILString(rv);
    }
    return rv;
}
static EXPRESSION* GetUUIDData(SYMBOL* cls)
{
    if (cls && cls->sb->uuidLabel)
    {
        return MakeIntExpression(ExpressionNode::labcon_, cls->sb->uuidLabel);
    }
    else
    {
        EXPRESSION* rv;
        StringData* data = Allocate<StringData>();
        data->pointers = Allocate<Optimizer::SLCHAR*>();
        data->size = 1;
        data->strtype = LexType::l_astr_;
        data->pointers[0] = Allocate<Optimizer::SLCHAR>();
        data->pointers[0]->count = 16;
        data->pointers[0]->str = Allocate<LCHAR>(16);
        if (cls && cls->sb->uuid)
        {
            int i;
            for (i = 0; i < 16; i++)
                data->pointers[0]->str[i] = cls->sb->uuid[i];
        }
        rv = stringlit(data);
        if (cls)
            cls->sb->uuidLabel = rv->v.i;
        return rv;
    }
}
static EXPRESSION* GetManagedFuncData(Type* tp)
{
    // while managed functions seem to be always stdcall, the caller may be
    // either expecting a stdcall function, or a cdecl function
    // we take care of this by using the 'stdcall' attribute on the function
    // to determine what a caller might expect.
    // this function makes a little thunk table to tell the thunk generator
    // how to set up arguments for the call.  The arguments have to be reversed
    // but we cannot do a naive reversal because arguments of 8 bytes or more have to
    // be copied in order rather than reversed.
    char buf[512], *save = buf;
    int i;
    int sz = 0;
    StringData* data = Allocate<StringData>();
    SYMBOL* sym;
    if (tp->IsPtr())
        tp = tp->BaseType()->btp;
    sym = tp->BaseType()->sp;
    *save++ = 0;  // space for the number of dwords
    *save++ = 0;
    *save++ = sym->sb->attribs.inheritable.linkage == Linkage::stdcall_;
    auto it = tp->BaseType()->syms->begin();
    auto itend = tp->BaseType()->syms->end();
    if (it != itend && (*it)->tp->type != BasicType::void_)
    {
        sz++;
        ++it;
        if (it != itend)
        {
            sz++;
            ++it;
            for (; it != itend; ++it)
            {
                int n = (*it)->tp->size;
                n += 3;
                n /= 4;
                if (n > 32767)
                    diag("GetManagedFuncData: passing too large structure");
                if (n > 0x7f)
                {
                    *save++ = (n >> 8) | 0x80;
                    *save++ = n & 0xff;
                }
                else
                {
                    *save++ = n;
                }
                sz += n;
            }
        }
    }
    buf[0] = (sz >> 8) | 0x80;
    buf[1] = sz & 0xff;
    data->pointers = Allocate<Optimizer::SLCHAR*>();
    data->size = 1;
    data->strtype = LexType::l_astr_;
    data->pointers[0] = Allocate<Optimizer::SLCHAR>();
    data->pointers[0]->count = save - buf;
    data->pointers[0]->str = Allocate<LCHAR>(save - buf);
    for (i = 0; i < save - buf; i++)
        data->pointers[0]->str[i] = buf[i];
    return stringlit(data);
}
void ValidateMSILFuncPtr(Type* dest, Type* src, EXPRESSION** exp)
{
    bool managedDest = false;
    bool managedSrc = false;
    if ((*exp)->type == ExpressionNode::callsite_ && (*exp)->v.func->ascall)
        return;
    // this implementation marshals functions that are put into a pointer variable
    // declared with STDCALL.   If it is not declared with stdcall, then putting it
    // into a structure and passing it to unmanaged code will not work.
    if (dest->IsFunction())
    {
        // function arg or assignment to function constant
        managedDest =
            dest->BaseType()->sp->sb->attribs.inheritable.linkage2 != Linkage::unmanaged_ && msilManaged(dest->BaseType()->sp);
    }
    else if (dest->IsFunctionPtr())
    {
        // function var
        managedDest = dest->BaseType()->btp->BaseType()->sp->sb->attribs.inheritable.linkage2 != Linkage::unmanaged_ &&
                      msilManaged(dest->BaseType()->btp->BaseType()->sp);
    }
    else
    {
        // unknown
        errorConversionOrCast(true, src, dest);
    }
    if (src->IsFunction())
    {
        // function arg or assignment to function constant
        managedSrc = msilManaged(src->BaseType()->sp);
    }
    else if (src->IsFunctionPtr())
    {
        // function var
        managedSrc = msilManaged(src->BaseType()->btp->BaseType()->sp);
    }
    else
    {
        // unknown
        errorConversionOrCast(true, src, dest);
    }
    if (managedDest != managedSrc)
    {
        SYMBOL* sym;
        if (managedSrc)
        {
            sym = gsearch("__OCCMSIL_GetProcThunkToManaged");
            if (sym)
            {
                int n = 0;
                CallSite* functionCall = Allocate<CallSite>();
                Type* tp1 = src;
                if (tp1->IsPtr())
                    tp1 = tp1->BaseType()->btp;

                for (auto sp : *tp1->BaseType()->syms)
                {
                    int m = sp->tp->size;
                    m += 3;
                    m /= 4;
                    n += m;
                }
                sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
                functionCall->sp = sym;
                functionCall->functp = sym->tp;
                functionCall->fcall = MakeExpression(ExpressionNode::pc_, sym);
                functionCall->arguments = argumentListFactory.CreateList();
                auto arg = Allocate<Argument>();
                arg->tp = &stdpointer;
                arg->exp = *exp;
                functionCall->arguments->push_back(arg);
                arg = Allocate<Argument>();
                arg->tp = &stdpointer;
                arg->exp = GetManagedFuncData(tp1);
                functionCall->arguments->push_back(arg);
                functionCall->ascall = true;
                *exp = MakeExpression(functionCall);
            }
            else
            {
                diag("ValidateMSILFuncPtr: missing conversion func definition");
            }
        }
        else
        {
            sym = gsearch("__OCCMSIL_GetProcThunkToUnmanaged");
            if (sym)
            {
                CallSite* functionCall = Allocate<CallSite>();
                sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
                functionCall->sp = sym;
                functionCall->functp = sym->tp;
                functionCall->fcall = MakeExpression(ExpressionNode::pc_, sym);
                functionCall->arguments = argumentListFactory.CreateList();
                auto arg = Allocate<Argument>();
                arg->tp = &stdpointer;
                arg->exp = *exp;
                functionCall->arguments->push_back(arg);
                functionCall->ascall = true;
                *exp = MakeExpression(functionCall);
            }
            else
            {
                diag("ValidateMSILFuncPtr: missing conversion func definition");
            }
        }
    }
}
EXPRESSION* MakeExpression(ExpressionNode type, EXPRESSION* left, EXPRESSION* right)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = type;
    rv->left = left;
    rv->right = right;
    return rv;
}
EXPRESSION* MakeExpression(ExpressionNode type, SYMBOL* sym)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = type;
    rv->v.sp = sym;
    return rv;
}
EXPRESSION* MakeExpression(CallSite* callSite)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = ExpressionNode::callsite_;
    rv->v.func = callSite;
    return rv;
}
EXPRESSION* MakeExpression(Type* tp)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = ExpressionNode::type_;
    rv->v.tp = tp;
    return rv;
}
EXPRESSION* MakeIntExpression(ExpressionNode type, long long val)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = type;
    rv->v.i = val;
    return rv;
}
static bool inreg(EXPRESSION* exp, bool first)
{
    while (IsCastValue(exp))
        exp = exp->left;
    if (first && TakeAddress(&exp))
    {
        first = false;
    }
    if (exp->type == ExpressionNode::auto_)
    {
        return exp->v.sp->sb->storage_class == StorageClass::register_;
    }
    else if (exp->type == ExpressionNode::add_ || exp->type == ExpressionNode::arrayadd_ ||
             exp->type == ExpressionNode::arraymul_ || exp->type == ExpressionNode::lsh_ ||
             exp->type == ExpressionNode::structadd_ || exp->type == ExpressionNode::arraylsh_)
        return inreg(exp->left, first) || inreg(exp->right, first);
    else
        return false;
}
static void tagNonConst(SYMBOL* sym, Type* tp)
{
    if (sym && tp)
    {
        if (tp->IsRef())
            tp = tp->BaseType()->btp;
        sym->sb->nonConstVariableUsed |= !tp->IsConst();
    }
}

static EXPRESSION* GetConstMakeExpression(SYMBOL* sym)
{
    if (sym->sb->constexpression && sym->sb->init && !sym->sb->init->empty() && sym->sb->init->front()->exp)
        if (sym->tp->IsArithmetic() || sym->tp->BaseType()->type == BasicType::enum_)
            return MakeExpression(ExpressionNode::const_, sym);
    return nullptr;
}
static void variableName( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    char idname[512];
    CallSite* funcparams = nullptr;
    SYMBOL* sym = nullptr;
    SYMBOL* strSym = nullptr;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    LexemeStreamPosition placeHolder(currentStream);
    if (ismutable)
        *ismutable = false;
    if (Optimizer::cparams.prm_cplusplus ||
        ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions))
    {
        id_expression(funcsp, &sym, &strSym, &nsv, nullptr, false, false, idname, sizeof(idname), flags);
    }
    else
    {
        sym = gsearch(currentLex->value.s.a);
    }
    if (sym)
    {
        if (!(flags & _F_AMPERSAND))
            sym->tp->InstantiateDeferred();
        browse_usage(sym, currentLex->linedata->fileindex);
        *tp = sym->tp;
        getsym();
        if (sym->sb && sym->sb->attribs.uninheritable.deprecationText)
        {
            deprecateMessage(sym);
        }
        switch (sym->sb ? sym->sb->storage_class : StorageClass::templateparam_)
        {
            case StorageClass::member_:
            case StorageClass::mutable_:
                sym = lambda_capture(sym, cmThis, false);
                break;
            case StorageClass::auto_:
            case StorageClass::register_:
            case StorageClass::parameter_:
                sym = lambda_capture(sym, cmNone, false);
                break;
            case StorageClass::constant_:
                if ((flags & _F_AMPERSAND) && sym->sb->parent)
                    sym = lambda_capture(sym, cmNone, false);
                break;
            default:
                break;
        }
        if (sym->sb && sym->sb->templateLevel && !sym->tp->IsFunction() && sym->sb->storage_class != StorageClass::type_ &&
            !ismember(sym))
        {
            if (MATCHKW(Keyword::lt_))
            {
                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                SYMBOL* sp1 = sym;
                GetTemplateArguments(funcsp, sp1, &lst);

                bool deferred = false;
                if (lst)
                {
                    for (auto&& to : *lst)
                    {
                        if (to.second->packed && (!to.second->derivedFromUnpacked && !to.second->resolved))
                        {
                            deferred = true;
                        }
                    }
                }
                if (!deferred || unpackingTemplate)
                {
                    sp1 = GetVariableTemplate(sp1, lst);
                    if (sp1)
                    {
                        sym = sp1;
                        *tp = sym->tp;
                    }
                    else if (IsDefiningTemplate())
                        *tp = &stdint;
                    else
                        errorsym(ERR_NO_TEMPLATE_MATCHES, sym);
                }
                else
                {
                    // set up for unpacking...
                    sp1 = CopySymbolfalse(sym);
                    sp1->templateParams = lst;
                    sym = sp1;
                }
            }
            else
            {
                errorsym(ERR_NEED_TEMPLATE_ARGUMENTS, sym);
            }
        }
        if ((!sym->sb || sym->sb->storage_class == StorageClass::templateparam_) && sym->tp->type == BasicType::templateparam_ &&
            (sym->tp->templateParam->second->type != TplType::int_ ||
             (sym->tp->templateParam->second->type == TplType::int_ && sym->tp->templateParam->second->byNonType.val)))
        {
            switch (sym->tp->templateParam->second->type)
            {
                case TplType::typename_:
                case TplType::template_:
                    placeHolder.Backup();
                    *tp = nullptr;
                    if ((flags & (_F_SIZEOF | _F_PACKABLE)) == (_F_SIZEOF | _F_PACKABLE))
                    {
                        *exp = MakeExpression(ExpressionNode::templateparam_, sym);
                        *tp = sym->tp;
                        getsym();
                        if (MATCHKW(Keyword::openpa_))
                        {
                            getsym();
                            errskim(skim_closepa, false);
                            getsym();
                        }
                        else if (MATCHKW(Keyword::begin_))
                        {
                            getsym();
                            errskim(skim_end, false);
                            getsym();
                        }
                    }
                    else
                    {
                        expression_func_type_cast(funcsp, tp, exp, flags);
                    }
                    return;
                case TplType::int_:
                    if (sym->tp->templateParam->second->packed)
                    {
                        if (unpackingTemplate && !sym->tp->templateParam->second->resolved)
                        {
                            if (sym->tp->templateParam->second->byPack.pack)
                            {
                                auto tpl = LookupPackedInstance(*sym->tp->templateParam);
                                if (tpl)
                                {
                                    *exp = tpl->byNonType.val;
                                }
                                else
                                {
                                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                                }
                            }
                            else
                            {
                                *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                            }
                        }
                        else
                        {
                            *exp = MakeExpression(ExpressionNode::templateparam_);
                            (*exp)->v.sp = sym;
                        }
                    }
                    else
                    {
                        *exp = sym->tp->templateParam->second->byNonType.val;
                    }
                    *tp = sym->tp->templateParam->second->byNonType.tp;
                    if ((*tp)->type == BasicType::templateparam_)
                    {
                        Type* tp1 = (*tp)->templateParam->second->byClass.val;
                        if (tp1)
                        {
                            *tp = tp1;
                        }
                        else
                        {
                            SYMBOL* rv = nullptr;
                            for (auto&& s : enclosingDeclarations)
                            {
                                if (rv)
                                    break;
                                if (s.tmpl)
                                    rv = templatesearch((*tp)->templateParam->first->name, s.tmpl);
                            }
                            if (rv && rv->tp->templateParam->second->type == TplType::typename_)
                                *tp = rv->tp->templateParam->second->byClass.val;
                        }
                    }
                    expression_func_type_cast(funcsp, tp, exp, flags | _F_NOEVAL);
                    if (!*exp)
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    return;
                default:
                    break;
            }
        }
        else
        {
            if (sym->tp->BaseType()->type == BasicType::templateparam_)
            {

                if (((sym->sb && sym->sb->storage_class == StorageClass::parameter_) ||
                     sym->tp->BaseType()->templateParam->second->type == TplType::int_) &&
                    sym->tp->BaseType()->templateParam->second->packed)
                {
                    if (unpackingTemplate && !sym->tp->BaseType()->templateParam->second->resolved)
                    {
                        auto tpl = LookupPackedInstance(*sym->tp->BaseType()->templateParam);
                        if (tpl)
                        {
                            if (tpl->type == TplType::int_ && !tpl->packsym)
                            {
                                *tp = tpl->byNonType.tp;
                                if ((*tp)->type == BasicType::templateparam_)
                                {
                                    if ((*tp)->templateParam->second->byClass.val)
                                    {
                                        *tp = (*tp)->templateParam->second->byClass.val;
                                    }
                                    else
                                    {
                                        *tp = sym->tp;
                                        if ((*tp)->type == BasicType::templateparam_)
                                        {
                                            if ((*tp)->templateParam->second->byNonType.tp)
                                            {
                                                *tp = (*tp)->templateParam->second->byNonType.tp;
                                                if ((*tp)->type == BasicType::templateparam_)
                                                {
                                                    if ((*tp)->templateParam->second->byClass.val)
                                                    {
                                                        *tp = (*tp)->templateParam->second->byClass.val;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                *exp = tpl->byNonType.val;
                            }
                            else
                            {
                                sym = tpl->packsym;
                                *tp = sym->tp;
                                *exp = MakeExpression(ExpressionNode::auto_, sym);
                            }
                        }
                        else
                        {
                            *exp = MakeIntExpression(ExpressionNode::packedempty_, 0);
                        }
                    }
                    else
                    {
                        sym->packed = true;
                        *exp = MakeExpression(ExpressionNode::auto_, sym);
                        AddPackedEntityToSequence(sym->tp->BaseType()->templateParam->first, sym->tp->BaseType()->templateParam->second);
                    }
                }
                else
                {
                    *exp = MakeExpression(ExpressionNode::templateparam_, sym);
                }
            }
            else if (sym->sb)
            {
                switch (sym->sb->storage_class)
                {
                    case StorageClass::mutable_:
                        if (ismutable)
                            *ismutable = true;
                    case StorageClass::member_:
                        /*
                        if (flags & _F_SIZEOF)
                        {
                            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        }
                        else
                        */
                        if (Optimizer::cparams.prm_cplusplus && (flags & _F_AMPERSAND) && strSym)
                        {
                            *exp = getMemberPtr(sym, strSym, tp, funcsp);
                        }
                        else
                        {
                            *exp = getMemberNode(sym, strSym, tp, funcsp);
                        }
                        break;
                    case StorageClass::alias_:
                        *exp = sym->sb->init->front()->exp;
                        *exp = MakeExpression(ExpressionNode::add_, *exp,
                                              MakeIntExpression(ExpressionNode::c_i_, sym->sb->init->front()->offset));
                        break;
                    case StorageClass::type_:
                    case StorageClass::typedef_:
                        placeHolder.Backup();
                        *tp = nullptr;
                        expression_func_type_cast(funcsp, tp, exp, flags);
                        if (!*exp)
                            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        return;
                    case StorageClass::overloads_: {
                        funcparams = Allocate<CallSite>();
                        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::lt_))
                        {
                            GetTemplateArguments(funcsp, sym, &funcparams->templateParams);
                            funcparams->astemplate = true;
                        }
                        if (Optimizer::cparams.prm_cplusplus || sym->tp->BaseType()->syms->size() > 1)
                        {
                            funcparams->nameSpace = strSym ? nullptr : nsv ? nsv->front() : nullptr;
                            funcparams->ascall = MATCHKW(Keyword::openpa_);
                            funcparams->sp = sym;
                        }
                        else
                        {
                            // we only get here for C language, sadly we have to do
                            // argument based lookup in C++...
                            funcparams->sp = sym->tp->BaseType()->syms->front();
                            funcparams->sp->sb->attribs.inheritable.used = true;
                            funcparams->fcall = MakeExpression(ExpressionNode::pc_, funcparams->sp);
                            if (!MATCHKW(Keyword::openpa_))
                                if (funcparams->sp->sb->attribs.inheritable.isInline)
                                {
                                    funcparams->sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                    InsertInline(funcparams->sp);
                                }
                        }
                        funcparams->functp = funcparams->sp->tp;
                        *tp = funcparams->sp->tp;
                        funcparams->asaddress = !!(flags & _F_AMPERSAND);
                        if (Optimizer::cparams.prm_cplusplus && ismember((*tp)->BaseType()->sp) && !MATCHKW(Keyword::openpa_))
                        {
                            EXPRESSION* exp1 = Allocate<EXPRESSION>();
                            exp1->type = ExpressionNode::memberptr_;
                            exp1->left = *exp;
                            exp1->v.sp = funcparams->sp;
                            *exp = exp1;
                            getMemberPtr(sym, strSym, tp, funcsp);
                        }
                        else
                        {
                            *exp = MakeExpression(funcparams);
                        }
                        break;
                    }
                    case StorageClass::catchvar_:
                        makeXCTab(funcsp);
                        *exp = MakeExpression(ExpressionNode::auto_, funcsp->sb->xc->xctab);
                        *exp =
                            MakeExpression(ExpressionNode::add_, *exp, MakeIntExpression(ExpressionNode::c_i_, XCTAB_INSTANCE_OFS));
                        Dereference(&stdpointer, exp);
                        break;
                    case StorageClass::enumconstant_:
                        *exp = MakeIntExpression(ExpressionNode::c_i_, sym->sb->value.i);
                        break;
                    case StorageClass::constant_:
                        *exp = MakeExpression(ExpressionNode::const_, sym);
                        break;
                    case StorageClass::auto_:
                    case StorageClass::register_: /* register variables are treated as
                                                   * auto variables in this compiler
                                                   * of course the usage restraints of the
                                                   * register keyword are enforced elsewhere
                                                   */
                        if (((*exp) = GetConstMakeExpression(sym)))
                            break;
                        if (sym->sb->anonymousGlobalUnion)
                            sym = localAnonymousUnions[sym->sb->label];
                        *exp = MakeExpression(ExpressionNode::auto_, sym);
                        sym->sb->anyTry |= tryLevel != 0;
                        SetRuntimeData(*exp, sym);
                        break;
                    case StorageClass::parameter_:
                        if (sym->packed)
                        {
                            if (!(flags & _F_PACKABLE))
                                error(ERR_PACK_SPECIFIER_MUST_BE_USED_IN_ARGUMENT);
                            if (unpackingTemplate)
                            {
                                Type* tp1 = sym->tp;
                                std::list<TEMPLATEPARAMPAIR>* templateParam;
                                int i;
                                while (tp1->IsPtr() || tp1->IsRef())
                                    tp1 = tp1->BaseType()->btp;
                                tp1 = tp1->BaseType();
                                if (tp1->type == BasicType::templateparam_)
                                {
                                    if (tp1->templateParam->second->resolved)
                                    {
                                        *exp = MakeExpression(ExpressionNode::auto_, sym);
                                    }
                                    else
                                    {
                                        auto tpl = LookupPackedInstance(*sym->tp->templateParam);
                                        if (tpl)
                                        {
                                            sym = tpl->packsym;
                                            *tp = sym->tp;
                                            *exp = MakeExpression(ExpressionNode::auto_, sym);
                                        }
                                        else
                                        {
                                            *exp = MakeIntExpression(ExpressionNode::packedempty_, 0);
                                        }
                                    }
                                }
                                else
                                {
                                    bool found = false;
                                    SymbolTable<SYMBOL>::iterator it, itend;
                                    SymbolTable<SYMBOL>* tables = localNameSpace->front()->syms;

                                    while (tables && !found)
                                    {
                                        for (auto it1 = tables->begin(); it1 != tables->end(); ++it1)
                                        {
                                            if (*it1 == sym)
                                            {
                                                it = it1;
                                                itend = tables->end();
                                                found = true;
                                            }
                                        }
                                        tables = tables->Next();
                                    }
                                    if (found)
                                    {
                                        int i;
                                        for (i = 0; it != itend && i < GetPackIndex(); i++)
                                            ++it;
                                        if (it != itend)
                                        {
                                            sym = *it;
                                        }
                                        *exp = MakeExpression(ExpressionNode::auto_, sym);
                                        *tp = sym->tp;
                                    }
                                    else
                                    {
                                        *exp = MakeIntExpression(ExpressionNode::packedempty_, 0);
                                    }
                                }
                            }
                            else
                            {
                                AddPackedEntityToSequence(sym, nullptr);
                                *exp = MakeExpression(ExpressionNode::auto_, sym);
                            }
                        }
                        else
                        {
                            *exp = MakeExpression(ExpressionNode::auto_, sym);
                        }
                        /* derefereance parameters which are declared as arrays */
                        {
                            Type* tpa = sym->tp->BaseType();
                            if (!tpa->IsRef() && tpa->IsArray())
                                Dereference(&stdpointer, exp);
                        }
                        sym->sb->anyTry |= tryLevel != 0;
                        break;

                    case StorageClass::localstatic_:
                        if (((*exp) = GetConstMakeExpression(sym)))
                            break;
                        tagNonConst(funcsp, sym->tp);
                        if (funcsp && funcsp->sb->attribs.inheritable.isInline)
                        {
                            if (funcsp->sb->promotedToInline || Optimizer::cparams.prm_cplusplus)
                            {
                                funcsp->sb->attribs.inheritable.isInline = funcsp->sb->promotedToInline = false;
                            }
                        }
                        if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                        {
                            if (funcsp)
                            {
                                funcsp->sb->nonConstVariableUsed = true;
                            }
                            *exp = MakeExpression(ExpressionNode::threadlocal_, sym);
                        }
                        else if (sym->sb->anonymousGlobalUnion)
                        {
                            *exp = MakeIntExpression(ExpressionNode::labcon_, sym->sb->label);
                        }
                        else
                        {
                            *exp = MakeExpression(ExpressionNode::global_, sym);
                        }
                        sym->sb->attribs.inheritable.used = true;
                        break;
                    case StorageClass::absolute_:
                        if (funcsp)
                        {
                            funcsp->sb->nonConstVariableUsed = true;
                        }
                        *exp = MakeExpression(ExpressionNode::absolute_, sym);
                        break;
                    case StorageClass::static_:
                    case StorageClass::global_:
                    case StorageClass::external_:
                    {
                        if (sym->sb->storage_class == StorageClass::static_)
                        {
                            sym->sb->attribs.inheritable.used = true;
                        }
                        if (sym->sb->storage_class == StorageClass::static_ || sym->sb->storage_class == StorageClass::global_)
                        {
                            if (((*exp) = GetConstMakeExpression(sym)))
                                break;
                            if (sym->sb->attribs.inheritable.isInlineData)
                            {
                                sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                Optimizer::SymbolManager::Get(sym)->tp = Optimizer::SymbolManager::Get(sym->tp);
                                InsertInlineData(sym);
                            }
                        }
                        tagNonConst(funcsp, sym->tp);
                        SYMBOL* tpl = sym;
                        while (tpl)
                        {
                            if (tpl->sb->templateLevel)
                                break;
                            tpl = tpl->sb->parentClass;
                        }
                        if (tpl && tpl->sb->instantiated)
                        {
                            TemplateDataInstantiate(sym, false, false);
                        }
                        if (sym->sb->parentClass && !isExpressionAccessible(nullptr, sym, funcsp, nullptr, false))
                            errorsym(ERR_CANNOT_ACCESS, sym);
                        if (sym->sb->attribs.inheritable.linkage3 == Linkage::threadlocal_)
                            *exp = MakeExpression(ExpressionNode::threadlocal_, sym);
                        else if (sym->sb->anonymousGlobalUnion)
                            *exp = MakeIntExpression(ExpressionNode::labcon_, sym->sb->label);
                        else
                            *exp = MakeExpression(ExpressionNode::global_, sym);
                        if (sym->sb->attribs.inheritable.linkage2 == Linkage::import_)
                        {
                            Dereference(&stdpointer, exp);
                        }
                        break;
                    }
                    case StorageClass::namespace_:
                    case StorageClass::namespace_alias_:
                        errorsym(ERR_INVALID_USE_OF_NAMESPACE, sym);
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
                        break;
                    default:
                        error(ERR_IDENTIFIER_EXPECTED);
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
                        break;
                }
            }
            else
            {
                if (!templateDefinitionLevel)
                    error(ERR_IDENTIFIER_EXPECTED);
                *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
            }
        }
        sym->tp->used = true;
        if (sym->sb && sym->sb->templateLevel && istype(sym))
        {
            placeHolder.Backup();
            *tp = nullptr;
            expression_func_type_cast(funcsp, tp, exp, flags);
        }
        else
        {
            bool isref = false;
            if (sym->tp->BaseType()->type == BasicType::templateparam_)
            {
                if (*exp && (*exp)->type != ExpressionNode::packedempty_ && !sym->tp->BaseType()->templateParam->second->packed)
                {
                    if (sym->tp->BaseType()->templateParam->second->type == TplType::int_)
                        *tp = sym->tp->BaseType()->templateParam->second->byNonType.tp;
                    else
                        *tp = &stdint;
                    if (sym->sb && sym->sb->templateLevel && sym->sb->storage_class != StorageClass::type_ && !ismember(sym) &&
                        !sym->tp->IsFunction())
                    {
                        if (!(flags & _F_AMPERSAND))
                            Dereference(&stdint, exp);
                    }
                    else if (!templateDefinitionLevel)
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    if (MATCHKW(Keyword::openpa_) || MATCHKW(Keyword::begin_))
                    {
                        placeHolder.Backup();
                        Type* tp1 = nullptr;
                        EXPRESSION* exp1;
                        expression_func_type_cast(funcsp, &tp1, &exp1, flags);
                        return;
                    }
                }
            }
            else if (sym->tp->type == BasicType::any_)
                Dereference(&stdint, exp);
            else
            {
                int offset = 0;
                auto exp4 = relptr(*exp, offset);
                if ((*tp)->IsRef())
                {
                    isref = true;
                    auto type = (*tp)->BaseType()->type;
                    Type* tp1 = *tp;
                    Dereference(*tp, exp);
                    while (tp1->IsRef())
                        tp1 = tp1->BaseType()->btp;
                    *tp = tp1->CopyType();
                    (*tp)->UpdateRootTypes();
                    (*tp)->BaseType()->lref = false;
                    (*tp)->BaseType()->rref = false;
                }
                if (sym->sb && sym->sb->storage_class != StorageClass::overloads_)
                {
                    if (!(*tp)->IsStructured() && (*tp)->BaseType()->type != BasicType::memberptr_ && !(*tp)->IsFunction() &&
                        sym->sb->storage_class != StorageClass::constant_ &&
                        sym->sb->storage_class != StorageClass::enumconstant_ && sym->tp->type != BasicType::void_ &&
                        (*exp)->type != ExpressionNode::const_)
                    {
                        if (!(*tp)->array || (*tp)->vla || (!(*tp)->msil && sym->sb->storage_class == StorageClass::parameter_))
                            if ((*tp)->vla)
                                Dereference(&stdpointer, exp);
                            else
                                Dereference(*tp, exp);
                        else if ((*tp)->array && inreg(*exp, true))
                            error(ERR_CANNOT_TAKE_ADDRESS_OF_REGISTER);
                    }
                }
            }
            *tp = ResolveTemplateSelectors(sym, *tp);

            // in case there was an error somewhere
            if (*exp)
            {
                if (IsLValue(*exp) && (*exp)->type != ExpressionNode::l_object_)
                    (*exp)->v.sp = sym;  // catch for constexpr
                (*exp)->pragmas = preProcessor->GetStdPragmas();
                if ((*tp)->IsVolatile())
                    (*exp)->isvolatile = true;
                if ((*tp)->IsRestrict())
                    (*exp)->isrestrict = true;
                if ((*tp)->IsAtomic())
                {
                    (*exp)->isatomic = true;
                    if (needsAtomicLockFromType(*tp))
                        (*exp)->lockOffset = (*tp)->BaseType()->size;
                }
            }
            if (strSym && funcparams)
                funcparams->novtab = true;

            if (Optimizer::cparams.prm_cplusplus && sym->sb &&
                (sym->sb->storage_class == StorageClass::member_ || sym->sb->storage_class == StorageClass::mutable_))
            {
                if (!isref)
                    qualifyForFunc(funcsp, tp, *ismutable);
            }
        }
    }
    else if (parsingPreprocessorConstant)
    {
        *tp = &stdint;
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        getsym();
    }
    else
    {
        char* name;
        if (strSym && strSym->tp->type == BasicType::templateselector_)
        {
            SYMBOL* sym = (*strSym->tp->BaseType()->sp->sb->templateSelector)[1].sp;
            if ((!IsDefiningTemplate()) &&
                (sym->sb && sym->sb->instantiated && !declaringTemplate(sym) &&
                 (!sym->sb->templateLevel ||
                  allTemplateArgsSpecified(sym, (*strSym->tp->sp->sb->templateSelector)[1].templateParams))))
            {
                errorNotMember(sym, nsv ? nsv->front() : nullptr, ISID() ? currentLex->value.s.a : "__unknown");
            }
            *exp = MakeExpression(ExpressionNode::templateselector_);
            (*exp)->v.templateSelector = strSym->tp->sp->sb->templateSelector;
            *tp = strSym->tp;
            getsym();
            if (MATCHKW(Keyword::lt_))
            {
                std::list<TEMPLATEPARAMPAIR>* current = nullptr;
                GetTemplateArguments(funcsp, nullptr, &current);
            }
            return;
        }
        if (strSym && strSym->tp->BaseType()->type == BasicType::templatedecltype_)
        {
            *tp = strSym->tp;
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            getsym();
            return;
        }
        if (ISID())
            name = litlate(currentLex->value.s.a);
        else
            name = litlate("__unknown");
        sym = SymAlloc();
        sym->name = name;
        sym->sb->attribs.inheritable.used = true;
        if (currentLex)
        {
            sym->sb->declfile = sym->sb->origdeclfile = currentLex->sourceFileName;
            sym->sb->declline = sym->sb->origdeclline = currentLex->sourceLineNumber;
            sym->sb->declcharpos = currentLex->charindex;
            sym->sb->realcharpos = currentLex->realcharindex;
            sym->sb->realdeclline = currentLex->linedata->lineno;
            sym->sb->declfilenum = currentLex->linedata->fileindex;
        }
        getsym();
        if (MATCHKW(Keyword::openpa_))
        {
            if (Optimizer::cparams.prm_cplusplus)
            {
                sym->sb->storage_class = StorageClass::overloads_;
                (*tp) = Type::MakeType(BasicType::aggregate_);
                (*tp)->UpdateRootTypes();
                (*tp)->sp = sym;
            }
            else
            {
                /* no prototype error will be added later */
                sym->sb->storage_class = StorageClass::external_;
                sym->sb->attribs.inheritable.linkage = Linkage::c_;
                sym->tp = Type::MakeType(BasicType::func_, stdint.CopyType());
                sym->tp->syms = symbols->CreateSymbolTable();
                sym->tp->sp = sym;
                sym->sb->oldstyle = true;
                SetLinkerNames(sym, Linkage::c_);
                *tp = sym->tp;
            }
            SetLinkerNames(sym, Linkage::c_);
            if (sym->sb->storage_class != StorageClass::overloads_)
            {
                InsertSymbol(sym, sym->sb->storage_class, Linkage::none_, false);
                *exp = MakeExpression(
                    sym->sb->storage_class == StorageClass::auto_ ? ExpressionNode::auto_ : ExpressionNode::global_, sym);
            }
            else
            {
                sym->tp = Type::MakeType(BasicType::func_, Type::MakeType(BasicType::int_));
                sym->tp->syms = symbols->CreateSymbolTable();
                sym->tp->sp = sym;
                sym->sb->oldstyle = true;
                sym->sb->externShim = !!(flags & _F_INDECLTYPE);
                funcparams = Allocate<CallSite>();
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = MakeExpression(ExpressionNode::pc_, sym);
                funcparams->nameSpace = strSym ? nullptr : nsv ? nsv->front() : nullptr;
                *tp = sym->tp;
                *exp = MakeExpression(funcparams);
            }
        }
        else
        {
            sym->sb->storage_class = funcsp ? StorageClass::auto_ : StorageClass::global_;
            sym->tp = Type::MakeType(BasicType::any_);
            sym->sb->parentClass = strSym;
            *tp = sym->tp;
            Dereference(&stdint, exp);
            SetLinkerNames(sym, Linkage::c_);
            if (!nsv && (!strSym || !templateDefinitionLevel ||
                            (!strSym->sb->templateLevel && strSym->tp->type != BasicType::templateselector_ &&
                            strSym->tp->type != BasicType::templatedecltype_)))
            {
                char buf[4000];
                buf[0] = 0;
                auto currentPos = currentStream->Index();
                placeHolder.Backup();
                while (currentStream->Index() != currentPos)
                {
                    if (ISKW())
                    {
                        Utils::StrCat(buf, currentLex->kw->name);
                    }
                    else if (ISID())
                    {
                        Utils::StrCat(buf, currentLex->value.s.a);
                    }
                    getsym();
                }
                bool found = false;
                if (!(flags & _F_MEMBER) && !IsPacking())
                {
                    // look up packed symbol that has been elided during synthesis.
                    if (theCurrentFunc && theCurrentFunc->sb->maintemplate)
                    {
                        auto syms = theCurrentFunc->sb->maintemplate->tp->BaseType()->syms;
                        auto sym1 = syms->Lookup(sym->name);
                        if (sym1 && sym1->packed)
                        {
                            sym = sym1;
                            *tp = sym->tp;
                            found = true;
                        }
                    }
                    if (!found)
                        errorstr(ERR_UNDEFINED_IDENTIFIER, buf);
                }
                if (!found && sym->sb->storage_class != StorageClass::overloads_ &&
                    (localNameSpace->front()->syms || sym->sb->storage_class != StorageClass::auto_))
                    InsertSymbol(sym, sym->sb->storage_class, Linkage::none_, false);
            }
            if (nsv && (!IsPacking() || unpackingTemplate))
            {
                errorNotMember(strSym, nsv->front(), sym->sb->decoratedName);
            }
            if (sym->sb->storage_class != StorageClass::overloads_)
            {
                *exp = MakeExpression(ExpressionNode::global_, sym);
            }
            else
            {
                funcparams = Allocate<CallSite>();
                funcparams->nameSpace = nsv->front();
                *exp = MakeExpression(funcparams);
            }
        }
    }
    if (!*exp)
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
    if (sym && sym->sb)
    {
        sym->sb->allocate = true;
    }
    return;
}
static void expression_member( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    Type *typein = *tp, *typein2 = typein->IsArray() ? typein : nullptr;
    bool points = false;
    const char* tokenName = ".";
    if (ISKW())
        tokenName = currentLex->kw->name;
    (void)funcsp;
    (*tp)->InstantiateDeferred();
    // find structured version of arithmetic types for msil member matching
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions &&
        ((*tp)->IsArithmetic() || (*tp)->type == BasicType::string_ || ((*tp)->IsArray() && (*tp)->BaseType()->msil)))
    {
        // auto-boxing for msil
        Type* tp1 = find_boxed_type((*tp)->BaseType());
        if (tp1)
        {
            while (IsCastValue(*exp))
                *exp = (*exp)->left;
            if (!IsLValue(*exp))
                *exp = msilCreateTemporary(*tp, *exp);
            else
                *exp = (*exp)->left;
            *tp = tp1;
            typein = tp1;
        }
    }
    if (MATCHKW(Keyword::pointsto_))
    {
        points = true;
        do
        {
            (*tp) = (*tp)->InitializeDeferred();
        } while ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) &&
                 FindOperatorFunction(ovcl_pointsto, Keyword::pointsto_, funcsp, tp, exp, nullptr, nullptr, nullptr, flags));
        typein = *tp;
        if ((*tp)->IsPtr())
        {
            *tp = (*tp)->BaseType();
            *tp = (*tp)->btp;
        }
    }
    else
    {
        (*tp) = (*tp)->InitializeDeferred();
    }
    if (MATCHKW(Keyword::dot_) || MATCHKW(Keyword::pointsto_))
        getsym();
    if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::complx_))
    {
        // direct destructor or psuedo-destructor
        Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
        bool defd = false;
        bool notype = false;
        Type* tp1 = nullptr;
        getsym();
        tp1 = TypeGenerator::UnadornedType(funcsp, tp1, nullptr, false, StorageClass::auto_, &linkage, &linkage2, &linkage3,
                                           AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr, false, true, false,
                                           nullptr, false);
        if (!tp1)
        {
            error(ERR_TYPE_NAME_EXPECTED);
        }
        else
        {
            tp1->InstantiateDeferred();
            if (!(*tp)->CompatibleType(tp1))
            {
                if (!templateDefinitionLevel)
                    error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
            }
            else if ((*tp)->IsStructured())
            {
                // destructor
                SYMBOL* sp2 = search(((*tp)->BaseType()->sp)->tp->syms, overloadNameTab[CI_DESTRUCTOR]);
                if (sp2)
                {
                    if (flags & _F_IS_NOTHROW)
                        inNothrowHandler++;
                    CallDestructor((*tp)->BaseType()->sp, nullptr, exp, nullptr, true, false, false, !points);
                    if (flags & _F_IS_NOTHROW)
                        inNothrowHandler--;
                }
                if (needkw(Keyword::openpa_))
                    needkw(Keyword::closepa_);
            }
            else
            {
                // psuedo-destructor, no further activity required.
                if (needkw(Keyword::openpa_))
                    needkw(Keyword::closepa_);
            }
        }
        *tp = &stdvoid;
    }
    else if (!(*tp)->IsStructured() || (points && !typein->IsPtr()) ||
             (processingTrailingReturnOrUsing && (*tp)->IsStructured() && IsDefiningTemplate() &&
              (*tp)->BaseType()->sp->sb->templateLevel))
    {
        if (Optimizer::cparams.prm_cplusplus && ISKW() && (currentLex->kw->tokenTypes & TT_BASETYPE))
        {
            // possible psuedo destructor with selector
            Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
            bool defd = false;
            bool notype = false;
            Type* tp1 = nullptr;
            tp1 = TypeGenerator::UnadornedType(funcsp, tp1, nullptr, false, StorageClass::auto_, &linkage, &linkage2,
                                               &linkage3, AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr, false,
                                               true, false, nullptr, false);
            if (!tp1)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else if (!(*tp)->CompatibleType(tp1))
            {
                errorConversionOrCast(true, tp1, *tp);
            }
            if (!MATCHKW(Keyword::classsel_))
            {
                error(ERR_INVALID_PSUEDO_DESTRUCTOR);
            }
            else
            {
                getsym();
                if (!MATCHKW(Keyword::complx_))
                {
                    error(ERR_INVALID_PSUEDO_DESTRUCTOR);
                }
                else
                {
                    getsym();
                    tp1 = nullptr;
                    tp1 = TypeGenerator::UnadornedType(funcsp, tp1, nullptr, false, StorageClass::auto_, &linkage, &linkage2,
                                                       &linkage3, AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr,
                                                       false, true, false, nullptr, false);
                    if (!tp1)
                    {
                        error(ERR_TYPE_NAME_EXPECTED);
                    }
                    else if (!(*tp)->CompatibleType(tp1))
                    {
                        error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                    }
                    else if (needkw(Keyword::openpa_))
                        needkw(Keyword::closepa_);
                }
            }
            *tp = &stdvoid;
        }
        else if (templateDefinitionLevel && Optimizer::cparams.prm_cplusplus)
        {
            *exp = MakeExpression(points ? ExpressionNode::pointsto_ : ExpressionNode::dot_, *exp);
            EXPRESSION** ptr = &(*exp)->right;
            while (ISID() || MATCHKW(Keyword::operator_))
            {
                Type* tp2 = nullptr;
                EXPRESSION* exp = nullptr;
                expression_pm(funcsp, nullptr, &tp2, ptr, nullptr, _F_MEMBER);
                if (!MATCHKW(Keyword::pointsto_) && !MATCHKW(Keyword::dot_))
                    break;
                *ptr = MakeExpression(MATCHKW(Keyword::pointsto_) ? ExpressionNode::pointsto_ : ExpressionNode::dot_, *ptr);
                ptr = &(*ptr)->right;
                getsym();
            }
            if (!*ptr)
            {
                *ptr = MakeIntExpression(ExpressionNode::c_i_, 0);
            }
            else
            {
                *tp = Type::MakeType(BasicType::templatedecltype_);
                (*tp)->templateDeclType = *exp;
            }
        }
        else
        {
            if (!hasPackedExpression(*exp, true))
            {
                if (points)
                    errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, tokenName);
                else
                    errorstr(ERR_STRUCTURED_TYPE_EXPECTED, tokenName);
            }
            while (ISID() || MATCHKW(Keyword::operator_))
            {
                Type* tp = nullptr;
                EXPRESSION* exp = nullptr;
                expression_pm(funcsp, nullptr, &tp, &exp, nullptr, _F_NOEVAL);
                if (!MATCHKW(Keyword::pointsto_) && !MATCHKW(Keyword::dot_))
                    break;
                getsym();
            }
        }
    }
    else
    {
        bool isTemplate = false;
        if (MATCHKW(Keyword::template_))
        {
            getsym();
            isTemplate = true;
        }
        if (!ISID() && !MATCHKW(Keyword::operator_))
        {
            error(ERR_IDENTIFIER_EXPECTED);
        }
        else
        {
            SYMBOL* sp2 = nullptr;
            if (Optimizer::cparams.prm_cplusplus)
            {
                DeclarationScope scope((*tp)->BaseType()->sp);
                id_expression(funcsp, &sp2, nullptr, nullptr, &isTemplate, false, true, nullptr, 0, 0);
            }
            else
            {
                sp2 = search(((*tp)->BaseType()->sp)->tp->syms, currentLex->value.s.a);
            }
            if (!sp2)
            {
                if (!templateDefinitionLevel || !(*tp)->BaseType()->sp->sb->templateLevel)
                    errorNotMember((*tp)->BaseType()->sp, nullptr, currentLex->value.s.a ? currentLex->value.s.a : "unknown");
                getsym();
                while (ISID())
                {
                    getsym();
                    if (!MATCHKW(Keyword::pointsto_) && !MATCHKW(Keyword::dot_))
                        break;
                    getsym();
                }
            }
            else
            {
                Type* tpb;
                Type* basetp = *tp;
                Type* typ2 = typein;
                if (sp2->sb->attribs.uninheritable.deprecationText)
                    deprecateMessage(sp2);
                browse_usage(sp2, currentLex->linedata->fileindex);
                if (typ2->IsPtr())
                    typ2 = typ2->BaseType()->btp;
                (*exp)->isatomic = false;
                getsym();
                sp2->sb->attribs.inheritable.used = true;
                *tp = sp2->tp;
                *tp = ResolveTemplateSelectors(sp2, *tp);
                while ((*tp)->type == BasicType::typedef_)
                    (*tp) = (*tp)->btp;
                tpb = (*tp)->BaseType();
                if (sp2->sb->storage_class == StorageClass::overloads_)
                {
                    CallSite* funcparams = Allocate<CallSite>();
                    if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::lt_))
                    {
                        bool isdest = sp2->tp->BaseType()->syms->front()->sb->isDestructor;
                        bool found = false;
                        for (auto sp : *sp2->tp->BaseType()->syms)
                        {
                            if (sp->sb->templateLevel)
                            {
                                found = true;
                                break;
                            }
                        }
                        if (found)
                        {
                            GetTemplateArguments(funcsp, nullptr, &funcparams->templateParams);
                            funcparams->astemplate = true;
                        }
                        else if (isdest && sp2->sb->parentClass->sb->templateLevel)
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            GetTemplateArguments(funcsp, nullptr, &lst);
                            if (!exactMatchOnTemplateArgs(
                                    lst, sp2->sb->parentClass->templateParams->front().second->bySpecialization.types))
                                error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                        }
                        else if (isTemplate)
                        {
                            errorsym(ERR_NOT_A_TEMPLATE, sp2);
                        }
                    }
                    else if (isTemplate)
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, sp2);
                    }
                    funcparams->sp = sp2;
                    funcparams->thisptr = *exp;
                    funcparams->thistp = Type::MakeType(BasicType::pointer_, basetp);

                    if (!points && (*exp)->type != ExpressionNode::l_ref_ && !typein->lref && !typein->rref)
                        funcparams->novtab = true;
                    *exp = MakeExpression(funcparams);
                }
                else
                {
                    SYMBOL* sp3 = sp2->sb->parentClass;
                    if (ismutable && sp2->sb->storage_class == StorageClass::mutable_)
                        *ismutable = true;
                    if (sp2->sb->templateLevel)
                    {
                        if (!sp2->sb->instantiated)
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            SYMBOL* sp1 = sp2;
                            if (MATCHKW(Keyword::lt_))
                            {
                                GetTemplateArguments(funcsp, sp2, &lst);
                                if (sp1)
                                {
                                    sp1 = GetClassTemplate(sp1, lst, false);
                                    if (sp1)
                                        sp2 = sp1;
                                    else
                                        errorsym(ERR_NO_TEMPLATE_MATCHES, sp2);
                                }
                            }
                            else if (sp2)
                            {
                                sp1 = sp2;
                            }
                        }
                    }
                    else if (isTemplate)
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, sp2);
                    }
                    if (sp2->sb->storage_class == StorageClass::external_ || sp2->sb->storage_class == StorageClass::static_)
                    {
                        SYMBOL* tpl = sp2;
                        while (tpl)
                        {
                            if (tpl->sb->templateLevel)
                                break;
                            tpl = tpl->sb->parentClass;
                        }
                        if (tpl && tpl->sb->instantiated)
                        {
                            TemplateDataInstantiate(sp2, false, false);
                        }
                    }
                    {
                        AccessLevel access = AccessLevel::public_;
                        SYMBOL* ssp = enclosingDeclarations.GetFirst();
                        while (ssp)
                        {
                            if (ssp == typ2->BaseType()->sp)
                                access = AccessLevel::protected_;
                            ssp = ssp->sb->parentClass;
                        }
                        ssp = enclosingDeclarations.GetFirst();
                        if (!isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, sp2, funcsp, *exp, true))
                        {
                            errorsym(ERR_CANNOT_ACCESS, sp2);
                        }
                    }
                    bool structuredAlias = false;
                    int offset = 0;
                    auto exp4 = relptr(*exp, offset);
                    // handle members of a constexpression class
                    if (sp2->sb->storage_class == StorageClass::constant_)
                    {
                        *exp = MakeExpression(ExpressionNode::const_, sp2);
                    }
                    else if (sp2->sb->storage_class == StorageClass::enumconstant_)
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, sp2->sb->value.i);
                    }
                    else if (sp2->sb->storage_class == StorageClass::static_ || sp2->sb->storage_class == StorageClass::external_)
                    {
                        if (Optimizer::architecture == ARCHITECTURE_MSIL)
                        {
                            *exp = MakeExpression(ExpressionNode::global_, sp2);
                        }
                        else
                        {
                            EXPRESSION* exp2 = MakeExpression(ExpressionNode::global_, sp2);
                            *exp = MakeExpression(ExpressionNode::comma_, *exp, exp2);
                        }
                    }
                    else
                    {
                        if (typein->IsStructured() && !typein->lref && !typein->rref && !(flags & _F_AMPERSAND) &&
                            typein->BaseType()->sp->sb->structuredAliasType)
                        {
                            structuredAlias = true;
                            // use it as-is
                        }
                        else
                        {
                            EXPRESSION* offset;
                            if (Optimizer::architecture == ARCHITECTURE_MSIL)
                            {
                                if (isconstzero(*tp, *exp))
                                {
                                    offset = MakeIntExpression(ExpressionNode::c_i_, sp2->sb->offset);
                                }
                                else
                                {
                                    offset = MakeExpression(ExpressionNode::structelem_,
                                                            sp2);  // prepare for the MSIL ldflda instruction
                                }
                            }
                            else
                                offset = MakeIntExpression(ExpressionNode::c_i_, sp2->sb->offset);
                            if (!typein2 && sp2->sb->parentClass != typ2->BaseType()->sp)
                            {
                                *exp = baseClassOffset(sp2->sb->parentClass, typ2->BaseType()->sp, *exp);
                            }
                            *exp = MakeExpression(ExpressionNode::structadd_, *exp, offset);
                            if (sp3)
                            {
                                do
                                {
                                    if (sp3->tp->BaseType()->type == BasicType::union_)
                                    {
                                        offset->unionoffset = true;
                                        break;
                                    }
                                    if (sp3 != sp2 && sp3->tp->IsPtr())
                                        break;
                                    sp3 = sp3->sb->parent;
                                } while (sp3);
                            }
                        }
                        if ((*tp)->IsRef())
                        {
                            Dereference(*tp, exp);
                            *tp = (*tp)->BaseType()->btp;
                        }
                        if (typein2)
                            Dereference(typein2, exp);
                    }
                    if (tpb->hasbits)
                    {
                        (*exp) = MakeExpression(ExpressionNode::bits_, *exp);
                        (*exp)->bits = tpb->bits;
                        (*exp)->startbit = tpb->startbit;
                    }
                    if (sp2->sb->storage_class != StorageClass::constant_ && sp2->sb->storage_class != StorageClass::enumconstant_)
                    {
                        Dereference(*tp, exp);
                    }
                    if (sp2->sb->storage_class != StorageClass::enumconstant_ && !structuredAlias)
                    {
                        (*exp)->v.sp = sp2;  // caching the member symbol in the enode for constexpr handling
                    }
                    if (basetp->IsAtomic())
                    {
                        // note this won't work in C++ because of offset2...
                        (*exp)->isatomic = true;
                        if (needsAtomicLockFromType(*tp))
                            (*exp)->lockOffset = basetp->size - ATOMIC_FLAG_SPACE - sp2->sb->offset;
                    }
                }
                if (!points && ((*exp)->type == ExpressionNode::not__lvalue_ || (*exp)->type == ExpressionNode::callsite_ ||
                                (*exp)->type == ExpressionNode::comma_))
                    if (ISKW() && currentLex->kw->key >= Keyword::assign_ && currentLex->kw->key <= Keyword::asxor_)
                        error(ERR_LVALUE);
            }
        }
    }
    return;
}
Type* LookupSingleAggregate(Type* tp, EXPRESSION** exp, bool memberptr)
{
    auto tp1 = tp;
    if (tp->type == BasicType::aggregate_)
    {
        auto sp = tp->syms->front();
        if (memberptr && sp->sb->parentClass && sp->sb->storage_class != StorageClass::static_ &&
            sp->sb->storage_class != StorageClass::global_ && sp->sb->storage_class != StorageClass::external_)
        {
            EXPRESSION* rv;
            tp1 = Type::MakeType(BasicType::memberptr_, sp->tp);
            tp1->sp = sp->sb->parentClass;
            *exp = MakeExpression(ExpressionNode::memberptr_, sp);
            (*exp)->isfunc = true;
        }
        else
        {
            if ((*exp)->type == ExpressionNode::callsite_)
            {
                if ((*exp)->v.func->templateParams)
                {
                    sp = detemplate(sp, (*exp)->v.func, nullptr);
                    if (!sp)
                    {
                        sp = tp->syms->front();
                    }
                    else
                    {
                        if (sp->sb->templateLevel && !templateDefinitionLevel && sp->templateParams)
                        {
                            sp = TemplateFunctionInstantiate(sp, false);
                        }
                    }
                    if (sp->tp->BaseType()->btp->IsAutoType() && bodyTokenStreams.get(sp) && !sp->sb->inlineFunc.stmt)
                    {
                        CompileInlineFunction(sp);
                    }
                }
                for (auto sym : *sp->tp->BaseType()->syms)
                {
                    CollapseReferences(sym->tp);
                }
                CollapseReferences(sp->tp->BaseType()->btp);
            }
            *exp = MakeExpression(ExpressionNode::pc_, sp);
            tp1 = Type::MakeType(BasicType::pointer_, sp->tp);
        }
        auto it = tp->syms->begin();
        for (++it; it != tp->syms->end(); ++it)
        {
            auto sp = *it;
            if (!sp->sb->templateLevel || !sp->sb->mainsym)
            {
                errorsym(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, sp);
                break;
            }
        }
    }
    return tp1;
}
static EXPRESSION* MsilRebalanceArray(EXPRESSION* in)
{
    /* sizeof operators will already be present */
    if (in->type == ExpressionNode::add_ && in->right->type == ExpressionNode::umul_ &&
        in->right->right->type == ExpressionNode::sizeof_)
    {
        if (in->left->type == ExpressionNode::add_ && in->left->right->type == ExpressionNode::umul_ &&
            in->left->right->right->type == ExpressionNode::sizeof_)
        {
            if (in->right->right->left->v.tp->SameType(in->left->right->right->left->v.tp))
            {
                EXPRESSION* temp = MakeExpression(ExpressionNode::add_, in->left->right->left, in->right->left);
                temp = MakeExpression(ExpressionNode::umul_, temp, in->right->right);
                in->left = in->left->left;
                in->right = temp;
            }
        }
    }
    return in;
}
static void expression_bracket( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    Type* tp2 = nullptr;
    EXPRESSION* expr2 = nullptr;
    getsym();
    if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::begin_))
    {
        std::list<Argument*>* args = nullptr;
        getInitList(funcsp, &args);
        if ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) &&
            FindOperatorFunction(ovcl_openbr, Keyword::openbr_, funcsp, tp, exp, nullptr, nullptr, args, flags))
        {
        }
        else
        {
            error(ERR_CANNOT_USE_INIT_LIST_WITH_BUILTIN_BRACKET);
        }
    }
    else
    {
        expression_comma(funcsp, nullptr, &tp2, &expr2, nullptr, flags);
        if (tp2)
        {
            LookupSingleAggregate(tp2, &expr2);
            if (((*tp)->IsStructuredMath(tp2) || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                (FindOperatorFunction(ovcl_openbr, Keyword::openbr_, funcsp, tp, exp, tp2, expr2, nullptr, flags) ||
                 (Optimizer::architecture != ARCHITECTURE_MSIL && tp2->IsStructured() &&
                  castToArithmeticInternal(false, &tp2, &expr2, (Keyword)-1, &stdint, false))))
            {
            }
            else if ((*tp)->IsVoid() || tp2->IsVoid() || (*tp)->type == BasicType::aggregate_ || (*tp)->IsMsil() || tp2->IsMsil())
                error(ERR_NOT_AN_ALLOWED_TYPE);
            else if (tp2->BaseType()->type == BasicType::memberptr_ || (*tp)->BaseType()->type == BasicType::memberptr_)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            else if ((tp2->BaseType()->scoped || (*tp)->BaseType()->scoped) && !(flags & _F_SCOPEDENUM))
                error(ERR_SCOPED_TYPE_MISMATCH);
            else if ((*tp)->IsArray() && (*tp)->msil)
            {
                if (!tp2->IsInt() && tp2->BaseType()->type != BasicType::enum_)
                {
                    if (tp2->IsPtr())
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                if ((*exp)->type != ExpressionNode::msil_array_access_)
                {
                    EXPRESSION* expr3 = MakeExpression(ExpressionNode::msil_array_access_);
                    Type* btp = *tp;
                    int count = 0;
                    while (btp->IsArray() && btp->msil)
                    {
                        count++;
                        btp = btp->btp;
                    }
                    expr3->v.msilArray = (MSIL_ARRAY*)Alloc(sizeof(MSIL_ARRAY) + count - 1 * sizeof(EXPRESSION*));
                    expr3->v.msilArray->max = count;
                    expr3->v.msilArray->count = 0;
                    expr3->v.msilArray->base = (*exp);
                    expr3->v.msilArray->tp = *tp;
                    (*exp) = expr3;
                }
                (*exp)->v.msilArray->indices[(*exp)->v.msilArray->count++] = expr2;
                *tp = (*tp)->BaseType()->btp;
                if (!MATCHKW(Keyword::closebr_))
                {
                    error(ERR_ARRAY_NEED_CLOSEBRACKET);
                    errskim(skim_closebr);
                }
                skip(Keyword::closebr_);
                if ((*exp)->v.msilArray->count < (*exp)->v.msilArray->max && !MATCHKW(Keyword::openbr_))
                {
                    error(ERR_ASSIGN_ONLY_MSIL_ARRAY_ELEMENTS);
                }
                return;
            }
            else if ((*tp)->IsPtr() && (!(*tp)->IsFunctionPtr() || (*tp)->IsArray()))
            {
                if (!tp2->IsInt() && tp2->BaseType()->type != BasicType::enum_)
                {
                    if (tp2->IsPtr())
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                /* unreference pointer to VLA */
                if (!(*tp)->vla && (*tp)->BaseType()->btp->BaseType()->vla)
                    (*tp) = (*tp)->BaseType()->btp;
                if ((*tp)->vla)
                {
                    int soa = getSize(BasicType::pointer_);
                    int sou = getSize(BasicType::unsigned_);
                    EXPRESSION* vlanode = MakeExpression(ExpressionNode::auto_, (*tp)->sp);
                    EXPRESSION* exp1 = MakeExpression(ExpressionNode::add_, vlanode,
                                                      MakeIntExpression(ExpressionNode::c_i_, soa + sou * (1 + (*tp)->vlaindex)));
                    Dereference(&stdint, &exp1);
                    cast(&stdpointer, &expr2);
                    exp1 = MakeExpression(ExpressionNode::umul_, exp1, expr2);
                    *exp = MakeExpression(ExpressionNode::add_, *exp, exp1);
                    *tp = (*tp)->BaseType()->btp;
                }
                else
                {
                    EXPRESSION* exp1 = nullptr;
                    *tp = (*tp)->BaseType()->btp;
                    cast(&stdint, &expr2);
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        cast(&stdinative, &expr2);
                        if (!(*tp)->IsArray())
                        {
                            auto exp3 = MakeExpression(ExpressionNode::sizeof_, MakeExpression(*tp));
                            exp1 = MakeExpression(ExpressionNode::umul_, expr2, exp3);
                        }
                        else
                        {
                            Type* tp1 = *tp;
                            int n = tp1->size;
                            while (tp1->IsArray())
                                tp1 = tp1->btp;
                            n = n / tp1->size;
                            auto exp3 = MakeExpression(ExpressionNode::sizeof_, MakeExpression(tp1));
                            auto exp4 = MakeIntExpression(ExpressionNode::c_i_, n);
                            expr2 = MakeExpression(ExpressionNode::umul_, expr2, exp4);
                            exp1 = MakeExpression(ExpressionNode::umul_, expr2, exp3);
                        }
                    }
                    else
                    {
                        exp1 = MakeExpression(ExpressionNode::umul_, expr2, MakeIntExpression(ExpressionNode::c_i_, (*tp)->size));
                    }
                    expr2 = *exp;
                    *exp = MakeExpression(ExpressionNode::add_, *exp, exp1);

                    if (!inConstantExpression && expr2->type == ExpressionNode::auto_ && expr2->v.sp->sb->constexpression &&
                        !expr2->v.sp->sb->indecltable)
                    {
                        expr2->v.sp->sb->indecltable = true;
                        *exp = MakeExpression(ExpressionNode::comma_,
                                              ConverInitializersToExpression(expr2->v.sp->tp, expr2->v.sp, nullptr, funcsp,
                                                                             expr2->v.sp->sb->init, nullptr, false),
                                              *exp);
                    }
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        *exp = MsilRebalanceArray(*exp);
                    }
                }
                if (!(*tp)->array && !(*tp)->vla)
                    Dereference(*tp, exp);
            }
            else if (tp2->IsPtr() && !tp2->IsFunctionPtr())
            {
                if (!(*tp)->IsInt())
                {
                    if ((*tp)->IsPtr())
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                if (tp2->vla)
                {
                    int soa = getSize(BasicType::pointer_);
                    int sou = getSize(BasicType::unsigned_);
                    EXPRESSION* vlanode = MakeExpression(ExpressionNode::auto_, tp2->sp);
                    expr2 = MakeExpression(ExpressionNode::add_, expr2,
                                           MakeIntExpression(ExpressionNode::c_i_, soa + sou * (1 + tp2->vlaindex)));
                    Dereference(&stdint, exp);
                    *exp = MakeExpression(ExpressionNode::umul_, expr2, *exp);
                    Dereference(&stdpointer, &vlanode);
                    *exp = MakeExpression(ExpressionNode::add_, *exp, vlanode);
                    *tp = tp2->BaseType()->btp;
                }
                else
                {
                    EXPRESSION* exp1 = nullptr;
                    *tp = tp2->BaseType()->btp;
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        cast(&stdinative, exp);
                        if (!(*tp)->IsArray())
                        {
                            auto exp3 = MakeExpression(ExpressionNode::sizeof_, MakeExpression(*tp));
                            exp1 = MakeExpression(ExpressionNode::umul_, *exp, exp3);
                        }
                        else
                        {
                            Type* tp1 = *tp;
                            int n = tp1->size;
                            while (tp1->IsArray())
                                tp1 = tp1->btp;
                            n = n / tp1->size;
                            auto exp3 = MakeExpression(ExpressionNode::sizeof_, MakeExpression(tp1));
                            auto exp4 = MakeIntExpression(ExpressionNode::c_i_, n);
                            expr2 = MakeExpression(ExpressionNode::umul_, *exp, exp4);
                            exp1 = MakeExpression(ExpressionNode::umul_, *exp, exp3);
                        }
                    }
                    else
                    {
                        exp1 = MakeExpression(ExpressionNode::umul_, *exp, MakeIntExpression(ExpressionNode::c_i_, (*tp)->size));
                    }
                    *exp = MakeExpression(ExpressionNode::add_, expr2, exp1);
                    if (!inConstantExpression && expr2->type == ExpressionNode::auto_ && expr2->v.sp->sb->constexpression &&
                        !expr2->v.sp->sb->indecltable && expr2->v.sp->sb->init)
                    {
                        expr2->v.sp->sb->indecltable = true;
                        *exp = MakeExpression(ExpressionNode::comma_,
                                              ConverInitializersToExpression(expr2->v.sp->tp, expr2->v.sp, nullptr, funcsp,
                                                                             expr2->v.sp->sb->init, nullptr, false),
                                              *exp);
                    }
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        *exp = MsilRebalanceArray(*exp);
                    }
                }
                if (!(*tp)->array && !(*tp)->vla)
                    Dereference(*tp, exp);
            }
            else if (!templateDefinitionLevel || (*tp)->BaseType()->type != BasicType::templateselector_)
            {
                if (IsPacking())
                {
                    // this is invalid code gen, we need reference to both expressions for packing error handling later
                    *exp = MakeExpression(ExpressionNode::comma_, *exp, expr2);
                }
                else
                {
                    error(ERR_DEREF);
                }
            }
        }
        else
            error(ERR_EXPRESSION_SYNTAX);
    }
    if (!MATCHKW(Keyword::closebr_))
    {
        error(ERR_ARRAY_NEED_CLOSEBRACKET);
        errskim(skim_closebr);
    }
    skip(Keyword::closebr_);
    return;
}
void checkArgs(CallSite* params, SYMBOL* funcsp)
{
    std::list<Argument*>::iterator itp, itpe = itp;
    if (params->arguments)
    {
        itp = params->arguments->begin();
        itpe = params->arguments->end();
    }

    bool matching = true;
    bool tooshort = false;
    bool toolong = false;
    bool noproto = false;  // params->sp ? params->sp->sb->oldstyle : false;
    bool vararg = false;
    bool hasEllipse = false;
    int argnum = 0;

    auto it = params->functp->BaseType()->syms->begin();
    auto itend = params->functp->BaseType()->syms->end();
    if (it != itend && (*it)->sb->thisPtr)
        ++it;
    if (it == itend)
    {
        matching = false;
        noproto = params->sp->sb->oldstyle;
    }
    if (noproto)
    {
        if (params->sp->sb->hasproto)
            noproto = false;
    }
    if (it != itend && (*it)->tp->type == BasicType::void_)
    {
        if (itp != itpe)
            errorsym(ERR_FUNCTION_TAKES_NO_PARAMETERS, params->sp);
    }
    else
        while (it != itend || itp != itpe)
        {
            Type* dest = nullptr;
            SYMBOL* decl = nullptr;
            if (it == itend || (*it)->tp->type != BasicType::any_)
            {
                argnum++;
                if (matching && it != itend)
                {
                    decl = *it;
                    if (!decl->tp)
                        noproto = true;
                    else if (decl->tp->BaseType()->type == BasicType::ellipse_)
                    {
                        hasEllipse = true;
                        vararg = (Optimizer::architecture == ARCHITECTURE_MSIL) && msilManaged(params->sp);
                        params->vararg = vararg;
                        matching = false;
                        decl = nullptr;
                        it = itend;
                        if (itp == itpe)
                            break;
                    }
                }
                if (!decl && !hasEllipse && (Optimizer::architecture == ARCHITECTURE_MSIL))
                    toolong = true;
                if (decl && itp != itpe)
                    CheckThroughConstObject(decl->tp, (*itp)->exp);
                if (matching)
                {
                    if (!decl)
                        toolong = true;
                    else if (itp == itpe && !(*it)->sb->init && !initTokenStreams.get(*it))
                    {
                        auto tp2 = (*it)->tp;
                        while (tp2->IsRef() || tp2->IsPtr())
                            tp2 = tp2->BaseType()->btp;
                        tp2 = tp2->BaseType();

                        tooshort = tp2->type != BasicType::templateparam_ || !tp2->templateParam->second->packed;
                    }
                    else if (itp != itpe)
                    {
                        decl->tp->InstantiateDeferred();
                        if (decl->sb->attribs.inheritable.zstring)
                        {
                            EXPRESSION* exp = (*itp)->exp;
                            if (!exp)
                            {
                                exp = (*itp)->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                            }
                            TakeAddress(&exp);
                            switch (exp->type)
                            {
                                case ExpressionNode::global_:
                                case ExpressionNode::auto_:
                                case ExpressionNode::absolute_:
                                case ExpressionNode::pc_:
                                case ExpressionNode::threadlocal_:
                                    if (exp->v.sp->sb->attribs.inheritable.nonstring)
                                    {
                                        error(ERR_NULL_TERMINATED_STRING_REQUIRED);
                                    }
                                    break;
                            }
                        }
                        if (!decl->tp->SameExceptionType((*itp)->tp))
                        {
                            errorConversionOrCast(true, (*itp)->tp, decl->tp);
                        }
                        if (decl->tp->IsRef())
                        {
                            Type* tpb = decl->tp->BaseType()->btp->BaseType();
                            Type* tpd = (*itp)->tp;
                            (*itp)->byRef = true;
                            if (tpd->IsRef())
                                tpd = tpd->BaseType()->btp;
                            tpd = tpd->BaseType();
                            if (tpb->IsStructured() && tpd->IsStructured())
                            {
                                SYMBOL* base = tpb->BaseType()->sp;
                                SYMBOL* derived = tpd->BaseType()->sp;
                                if (base != derived && classRefCount(base, derived) != 1)
                                    errorConversionOrCast(true, (*itp)->tp, decl->tp);
                                if (base != derived && !isAccessible(derived, derived, base, funcsp, AccessLevel::public_, false))
                                    errorsym(ERR_CANNOT_ACCESS, base);
                            }
                            else
                            {
                                goto join;
                            }
                        }
                        else
                        {
                        join:
                            if (itp == itpe || !(*itp)->tp)
                            {
                            }
                            else if (decl->tp->BaseType()->type == BasicType::string_)
                            {
                                if ((*itp)->exp->type == ExpressionNode::labcon_)
                                    (*itp)->exp->type = ExpressionNode::c_string_;
                            }
                            else if (!(*itp)->tp->SameType(decl->tp))
                            {
                                if ((Optimizer::architecture != ARCHITECTURE_MSIL) || !decl->tp->IsStructured())
                                {
                                    if (decl->tp->BaseType()->type == BasicType::object_)
                                    {
                                        if (!(*itp)->tp->IsStructured() &&
                                            (!(*itp)->tp->IsArray() || !(*itp)->tp->BaseType()->msil))
                                        {
                                            (*itp)->exp = MakeExpression(ExpressionNode::x_object_, (*itp)->exp);
                                        }
                                    }
                                    else if ((!(*itp)->tp->IsBitInt() && !(*itp)->tp->IsInt()) ||
                                             (!decl->tp->IsBitInt() && !decl->tp->IsInt()))
                                        if (decl->tp->BaseType()->type != BasicType::memberptr_)
                                            errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                                }
                                else if ((*itp)->tp->IsStructured())
                                {
                                    SYMBOL* base = decl->tp->BaseType()->sp;
                                    SYMBOL* derived = (*itp)->tp->BaseType()->sp;
                                    if (base != derived && classRefCount(base, derived) != 1)
                                        errorConversionOrCast(true, (*itp)->tp, decl->tp);
                                    if (base != derived &&
                                        !isAccessible(derived, derived, base, funcsp, AccessLevel::public_, false))
                                        errorsym(ERR_CANNOT_ACCESS, base);
                                }
                                else if (!isconstzero((*itp)->tp, (*itp)->exp))
                                    errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                            }
                            else if (assignDiscardsConst(decl->tp, (*itp)->tp))
                            {
                                if (Optimizer::cparams.prm_cplusplus)
                                    errorConversionOrCast(true, (*itp)->tp, decl->tp);
                                else
                                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                            }
                            else if (decl->tp->IsPtr())
                            {
                                if (!(*itp)->tp->IsPtr() &&
                                    (!isarithmeticconst((*itp)->exp) || !isconstzero(decl->tp, (*itp)->exp)))
                                {
                                    if (!(*itp)->tp->IsFunction() ||
                                        (!decl->tp->IsVoidPtr() &&
                                         (!decl->tp->IsFunctionPtr() || (Optimizer::cparams.prm_cplusplus &&
                                                                         !decl->tp->BaseType()->btp->CompatibleType((*itp)->tp)))))
                                        errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                                }
                                else if (!decl->tp->CompatibleType((*itp)->tp))
                                {
                                    if (!isconstzero((*itp)->tp, (*itp)->exp))
                                    {
                                        if (!decl->tp->IsVoidPtr() && !(*itp)->tp->IsVoidPtr())
                                        {
                                            if (Optimizer::cparams.prm_cplusplus)
                                            {
                                                Type* tpb = decl->tp->BaseType()->btp;
                                                Type* tpd = (*itp)->tp->BaseType()->btp;
                                                if (tpb->IsStructured() && tpd->IsStructured())
                                                {
                                                    SYMBOL* base = tpb->BaseType()->sp;
                                                    SYMBOL* derived = tpd->BaseType()->sp;
                                                    if (base != derived && classRefCount(base, derived) != 1)
                                                        errorConversionOrCast(true, (*itp)->tp, decl->tp);
                                                    if (isAccessible(derived, derived, base, funcsp, AccessLevel::public_, false))
                                                        errorsym(ERR_CANNOT_ACCESS, base);
                                                }
                                                else
                                                {
                                                    errorConversionOrCast(true, (*itp)->tp, decl->tp);
                                                }
                                            }
                                            else
                                            {
                                                if (!(*itp)->tp->SameCharType(decl->tp))
                                                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                                            }
                                        }
                                    }
                                }
                            }
                            else if ((*itp)->tp->IsPtr())
                            {
                                if (!decl->tp->IsPtr() && decl->tp->BaseType()->type != BasicType::bool_)
                                    errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                            }
                            else if ((*itp)->tp->IsArithmetic() && decl->tp->IsArithmetic())
                            {
                                if (decl->tp->BaseType()->type != (*itp)->tp->BaseType()->type && decl->tp->IsLargeEnum())
                                {
                                    cast(decl->tp, &(*itp)->exp);
                                }
                            }
                        }
                        dest = decl->tp;
                    }
                }
                else if ((*itp)->tp->vla)
                {
                    // cast to a regular pointer if there is no declared param
                    (*itp)->tp = Type::MakeType(BasicType::pointer_, (*itp)->tp);
                    while ((*itp)->tp->btp->vla)
                        (*itp)->tp->btp = (*itp)->tp->btp->btp;
                }
                else if ((*itp)->tp->IsInt())
                {
                    if ((*itp)->tp->BaseType()->type <= BasicType::int_)
                        dest = &stdint;
                    else if (!(Optimizer::architecture == ARCHITECTURE_MSIL))
                        cast((*itp)->tp, &(*itp)->exp);
                }
                else if ((*itp)->tp->IsFloat())
                {
                    if ((*itp)->tp->BaseType()->type < BasicType::double_)
                        dest = &stddouble;
                    else if (!(Optimizer::architecture == ARCHITECTURE_MSIL))
                        cast((*itp)->tp, &(*itp)->exp);
                }
                else if ((*itp)->tp->IsPtr())
                {

                    if (Optimizer::architecture == ARCHITECTURE_MSIL &&
                        ((*itp)->exp->type != ExpressionNode::auto_ || !(*itp)->exp->v.sp->sb->va_typeof) && !params->vararg)
                        cast(&stdint, &(*itp)->exp);
                }
                if (dest && itp != itpe && (*itp)->tp && dest->BaseType()->type != BasicType::memberptr_ &&
                    !dest->CompatibleType((*itp)->tp))
                {
                    if ((Optimizer::architecture != ARCHITECTURE_MSIL) ||
                        (!dest->IsStructured() && (!dest->IsArray() || !dest->BaseType()->msil)))
                        cast(dest->BaseType(), &(*itp)->exp);
                    (*itp)->tp = dest;
                }
                else if (dest && itp != itpe && dest->BaseType()->type == BasicType::enum_)
                {
                    // this needs to be revisited to get proper typing in C++
                    cast(&stdint, &(*itp)->exp);
                    (*itp)->tp = &stdint;
                }
            }
            if (itp != itpe)
            {
                bool ivararg = vararg;
                if ((*itp)->exp->type == ExpressionNode::auto_ && (*itp)->exp->v.sp->sb && (*itp)->exp->v.sp->sb->va_typeof)
                    ivararg = false;
                (*itp)->vararg = ivararg;
                if (it != itend)
                {
                    Type* tp = (*it)->tp;
                    while (tp && tp->type != BasicType::pointer_)
                    {
                        if (tp->type == BasicType::va_list_)
                        {
                            (*itp)->valist = true;
                            break;
                        }
                        tp = tp->btp;
                    }
                }
                ++itp;
            }
            if (it != itend)
            {
                if ((*it)->packed && !unpackingTemplate)
                    break;
                ++it;
            }
        }
    if (noproto)
    {
        errorsym(ERR_CALL_FUNCTION_NO_PROTO, params->sp);
    }
    else if (toolong)
    {
        errorsym(ERR_PARAMETER_LIST_TOO_LONG, params->sp);
    }
    else if (tooshort)
    {
        errorsym(ERR_PARAMETER_LIST_TOO_SHORT, params->sp);
    }
}
static void getInitInternal( SYMBOL* funcsp, std::list<Argument*>** lptr, Keyword finish, bool allowNesting,
                                bool allowPack, bool toErr, int flags)
{
    *lptr = argumentListFactory.CreateList();
    getsym(); /* past ( */
    while (!MATCHKW(finish))
    {
        Argument* p = Allocate<Argument>();
        if (finish == Keyword::end_)
            p->initializer_list = true;
        if (MATCHKW(Keyword::begin_))
        {
            getInitInternal(funcsp, &p->nested, Keyword::end_, true, allowPack, false, flags);
            (*lptr)->push_back(p);
            if (!allowNesting)
                error(ERR_CANNOT_USE_INIT_LIST);
        }
        else
        {
            LexemeStreamPosition placeHolder(currentStream);
            EnterPackedSequence();
            expression_assign(funcsp, nullptr, &p->tp, &p->exp, nullptr,
                                    _F_PACKABLE | (finish == Keyword::closepa_ ? _F_INARGS : 0) | (flags & _F_SIZEOF));
            if (p->tp && p->tp->IsVoid() && finish != Keyword::closepa_)
                error(ERR_NOT_AN_ALLOWED_TYPE);
            if (!p->exp)
                p->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            optimize_for_constants(&p->exp);
            if ((!IsDefiningTemplate()) && p->tp && p->tp->type == BasicType::templateselector_)
                p->tp = LookupTypeFromExpression(p->exp, nullptr, false);
            if (finish != Keyword::closepa_)
            {
                assignmentUsages(p->exp, false);
            }
            else if (p->tp && p->tp->IsStructured())
            {
                auto exp3 = p->exp;
                if (exp3->type == ExpressionNode::comma_)
                {
                    while (exp3->type == ExpressionNode::comma_)
                        exp3 = exp3->right;
                }
                if (exp3->type == ExpressionNode::thisref_)
                {
                    p->tp = p->tp->CopyType(true);
                    p->tp->BaseType()->lref = false;
                    p->tp->BaseType()->rref = true;
                }
            }
            if (p->tp)
            {
                if (p->exp && p->exp->type == ExpressionNode::callsite_ && p->exp->v.func->sp->sb->parentClass &&
                    !p->exp->v.func->ascall && !p->exp->v.func->asaddress && p->exp->v.func->functp)
                {
                    for (auto sym : *p->exp->v.func->functp->BaseType()->syms)
                    {
                        if (sym->sb->storage_class == StorageClass::member_ || sym->sb->storage_class == StorageClass::mutable_)
                        {
                            error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                            break;
                        }
                    }
                }
                if (allowPack && Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
                {
                    // lose p
                    getsym();
                    if (templateDefinitionLevel)
                    {
                        (*lptr)->push_back(p);
                        p->exp->packedfunc = true;
                    }
                    else if (p->exp && p->exp->type != ExpressionNode::packedempty_)  // && p->tp->type != BasicType::any_)
                    {
                        if (!p->tp->IsStructured() && !p->tp->templateParam && p->tp->type != BasicType::any_ && !IsPacking())
                            checkPackedExpression(p->exp);
                        // this is going to presume that the expression involved
                        // is not too long to be cached by the LexList mechanism.
                        expandPackedInitList(lptr, funcsp, placeHolder, p->exp);
                    }
                    ClearPackedSequence();
                }
                else
                {
                    // fixme            if (toErr && argumentNestingLevel <= 1)
                    //                        checkUnpackedExpression(p->exp);
                    (*lptr)->push_back(p);
                }
            }
            else
            {
                --*currentStream;
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(finish == Keyword::closepa_ ? skim_closepa : skim_end);
                return;
            }
            LeavePackedSequence();
        }
        if (!MATCHKW(Keyword::comma_))
        {
            break;
        }
        getsym();
        if (MATCHKW(finish))
            break;
    }
    if (!needkw(finish))
    {
        errskim(finish == Keyword::closepa_ ? skim_closepa : skim_end);
        skip(finish);
    }
    StatementGenerator::DestructorsForArguments(*lptr);
    return;
}
void getInitList( SYMBOL* funcsp, std::list<Argument*>** owner)
{
    argumentNestingLevel++;
    getInitInternal(funcsp, owner, Keyword::end_, false, true, true, 0);
    argumentNestingLevel--;
}
void  getArgs( SYMBOL* funcsp, CallSite* funcparams, Keyword finish, bool allowPack, int flags)
{
    argumentNestingLevel++;
    getInitInternal(funcsp, &funcparams->arguments, finish, true, allowPack, argumentNestingLevel == 1, flags);
    argumentNestingLevel--;
}
void GetConstructorInitializers( SYMBOL* funcsp, CallSite* funcparams, Keyword finish, bool allowPack)
{
    argumentNestingLevel++;
    getInitInternal(funcsp, &funcparams->arguments, finish, true, allowPack, false, 0);
    argumentNestingLevel--;
}
static int simpleDerivation(EXPRESSION* exp)
{
    int rv = 0;
    while (IsCastValue(exp))
        exp = exp->left;
    if (exp->left)
        rv |= simpleDerivation(exp->left);
    if (exp->right)
        rv |= simpleDerivation(exp->right);
    switch (exp->type)
    {
        case ExpressionNode::callsite_:
        case ExpressionNode::thisref_:
        case ExpressionNode::stmt_:
        case ExpressionNode::constexprconstructor_:
            rv |= 1;
            break;
        case ExpressionNode::labcon_:
        case ExpressionNode::global_:
        case ExpressionNode::auto_:
        case ExpressionNode::absolute_:
        case ExpressionNode::pc_:
        case ExpressionNode::threadlocal_:
            rv |= 2;
            break;
        default:
            if (exp->left)
            {
                if (IsLValue(exp))
                    rv |= 1;
            }
            break;
    }
    return rv;
}
EXPRESSION* DerivedToBase(Type* tpn, Type* tpo, EXPRESSION* exp, int flags)
{
    if (tpn->IsRef())
        tpn = tpn->BaseType()->btp;
    if (tpo->IsRef())
        tpo = tpo->BaseType()->btp;
    if (tpn->IsStructured() && tpo->IsStructured())
    {
        SYMBOL* spn = tpn->BaseType()->sp;
        SYMBOL* spo = tpo->BaseType()->sp;
        if (spo != spn && !SameTemplate(tpn, tpo))
        {
            int n = classRefCount(spn, spo);
            if (n == 1)
            {
                // derived to base
                EXPRESSION q = {}, *v = &q;
                v->type = ExpressionNode::c_i_;
                v = baseClassOffset(spn, spo, v);

                optimize_for_constants(&v);
                if (v->type != ExpressionNode::c_i_ && (flags & _F_NOVIRTUALBASE))
                    return nullptr;

                v = baseClassOffset(spn, spo, exp);
                if ((flags & _F_VALIDPOINTER) || v->type != ExpressionNode::c_i_ || simpleDerivation(exp) == 2)
                {
                    optimize_for_constants(&v);
                }
                else
                {
                    EXPRESSION* varsp = AnonymousVar(StorageClass::auto_, &stdpointer);
                    EXPRESSION* var = MakeExpression(ExpressionNode::l_p_, varsp);
                    EXPRESSION* asn = MakeExpression(ExpressionNode::assign_, var, exp);
                    EXPRESSION* left = MakeExpression(ExpressionNode::add_, var, v);
                    EXPRESSION* right = var;
                    v = MakeExpression(ExpressionNode::hook_, var, MakeExpression(ExpressionNode::comma_, left, right));
                    v = MakeExpression(ExpressionNode::comma_, asn, v);
                }
                return v;
            }
        }
    }
    return exp;
}
static bool cloneTempExpr(EXPRESSION** expr, SYMBOL** found, SYMBOL** replace);
bool cloneTempStmt(std::list<Statement*>** blocks, SYMBOL** found, SYMBOL** replace)
{
    bool rv = false;
    auto p = *blocks;
    *blocks = stmtListFactory.CreateList();
    for (auto block : *p)
    {
        if (found)
        {
            Statement* b = Allocate<Statement>();
            *b = *block;
            block = b;
            (*blocks)->push_back(b);
        }
        switch (block->type)
        {
            case StatementNode::genword_:
                break;
            case StatementNode::try_:
            case StatementNode::catch_:
            case StatementNode::seh_try_:
            case StatementNode::seh_catch_:
            case StatementNode::seh_finally_:
            case StatementNode::seh_fault_:
                rv |= cloneTempStmt(&block->lower, found, replace);
                rv |= cloneTempStmt(&block->blockTail, found, replace);
                break;
            case StatementNode::return_:
            case StatementNode::expr_:
            case StatementNode::declare_:
                rv |= cloneTempExpr(&block->select, found, replace);
                break;
            case StatementNode::goto_:
            case StatementNode::label_:
                break;
            case StatementNode::select_:
            case StatementNode::notselect_:
                rv |= cloneTempExpr(&block->select, found, replace);
                break;
            case StatementNode::switch_:
                rv |= cloneTempExpr(&block->select, found, replace);
                rv |= cloneTempStmt(&block->lower, found, replace);
                break;
            case StatementNode::block_:
                rv |= cloneTempStmt(&block->lower, found, replace);
                rv |= cloneTempStmt(&block->blockTail, found, replace);
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
                diag("Invalid block type in cloneTempStmt");
                break;
        }
    }
    return rv;
}
#define CLONED_SYMS 20
static bool cloneTempExpr(EXPRESSION** expr, SYMBOL** found, SYMBOL** replace)
{
    bool rv = false;
    if (!*expr)
        return rv;
    if (found)
    {
        EXPRESSION* newval = Allocate<EXPRESSION>();
        *newval = **expr;
        *expr = newval;
    }
    if ((*expr)->type == ExpressionNode::thisref_)
    {
        rv = cloneTempExpr(&(*expr)->left, found, replace);
    }
    else if ((*expr)->type == ExpressionNode::stmt_)
    {
        rv = cloneTempStmt(&(*expr)->v.stmt, found, replace);
    }
    else if ((*expr)->type == ExpressionNode::callsite_)
    {
        if ((*expr)->v.func->sp->sb->isConstructor || (*expr)->v.func->sp->sb->isDestructor)
        {
            Argument** args;
            if (found)
            {
                CallSite* newval = Allocate<CallSite>();
                *newval = *(*expr)->v.func;
                (*expr)->v.func = newval;
            }
            if ((*expr)->v.func->arguments)
            {
                if (found)
                {
                    std::list<Argument*>* newList = argumentListFactory.CreateList();
                    for (auto arg : *(*expr)->v.func->arguments)
                    {
                        Argument* newval = Allocate<Argument>();
                        *newval = *arg;
                        rv |= cloneTempExpr(&newval->exp, found, replace);
                        newList->push_back(newval);
                    }
                    (*expr)->v.func->arguments = newList;
                }
                else
                {
                    for (auto arg : *(*expr)->v.func->arguments)
                    {
                        rv |= cloneTempExpr(&arg->exp, found, replace);
                    }
                }
            }
            rv |= cloneTempExpr(&(*expr)->v.func->thisptr, found, replace);
        }
    }
    else
    {
        rv |= cloneTempExpr(&(*expr)->left, found, replace);
        rv |= cloneTempExpr(&(*expr)->right, found, replace);
        if ((*expr)->type == ExpressionNode::auto_)
        {
            SYMBOL* sym = (*expr)->v.sp;
            if (sym->sb->anonymous && !sym->sb->stackblock)
            {
                if (found)
                {
                    int i = 0;
                    EXPRESSION* newexp = Allocate<EXPRESSION>();
                    *newexp = **expr;
                    *expr = newexp;
                    for (i = 0; i < CLONED_SYMS && found[i]; i++)
                    {
                        if (found[i] == sym)
                            break;
                    }
                    if (i >= CLONED_SYMS)
                    {
                        diag("cloneTempExpr - too many syms");
                    }
                    else
                    {
                        if (!found[i])
                        {
                            found[i] = sym;
                            replace[i] = CopySymbol(sym);
                            if (theCurrentFunc)
                                replace[i]->sb->value.i = theCurrentFunc->sb->value.i;
                            replace[i]->name = AnonymousName();
                        }
                        (*expr)->v.sp = replace[i];
                    }
                }
                rv = true;
            }
        }
    }
    return rv;
}
void CreateInitializerList(SYMBOL* func, Type* initializerListTemplate, Type* initializerListType, std::list<Argument*>** lptr,
                           bool operands, bool asref)
{

    std::list<Argument*>* initial = *lptr;
    EXPRESSION *rv = nullptr, **pos = &rv;
    int count = initial->size(), i;
    EXPRESSION *data, *initList;
    SYMBOL *begin, *size;
    EXPRESSION* dest;
    begin = search(initializerListTemplate->BaseType()->syms, "__begin_");
    size = search(initializerListTemplate->BaseType()->syms, "__size_");
    if (!begin || !size)
        Utils::Fatal("Invalid definition of initializer-list");
    if (initial->size() && !initial->front()->nested && initial->front()->tp->CompatibleType(initializerListTemplate))
    {
        auto exp = initial->front()->exp;
        initial->clear();
        initial->push_back(Allocate<Argument>());
        if (asref)
        {
            initial->front()->tp = Type::MakeType(BasicType::lref_, initializerListTemplate);
            initial->front()->exp = exp;
        }
        else
        {
            initial->front()->tp = initializerListTemplate;
            initial->front()->exp = MakeExpression(ExpressionNode::stackblock_, exp);
            initial->front()->exp->size = initializerListTemplate;
        }
    }
    else
    {
        auto itl = initial->begin();
        auto itle = initial->end();
        Type* tp = Type::MakeType(BasicType::pointer_, initializerListType);
        tp->array = true;
        tp->size = count * (initializerListType->size);
        tp->esize = MakeIntExpression(ExpressionNode::c_i_, count);
        data = AnonymousVar(StorageClass::auto_, tp);
        if (itl != itle && (*itl)->nested)
        {
            bool found = false;
            for (auto l : *(*itl)->nested)
                if (!IsConstantExpression(l->exp, false, true))
                {
                    found = true;
                    break;
                }
            if (!found)
                data->v.sp->sb->constexpression = true;
        }
        EXPRESSION* ildest = nullptr;
        if (count && initializerListType->IsStructured())
        {
            EXPRESSION* exp = data;
            EXPRESSION* elms = MakeIntExpression(ExpressionNode::c_i_, count);
            CallDestructor(initializerListType->BaseType()->sp, nullptr, &exp, elms, true, false, false, true);
            ildest = exp;
        }
        std::deque<EXPRESSION*> listOfScalars;
        for (i = 0; i < count && itl != itle; i++, ++itl)
        {
            EXPRESSION* node = nullptr;
            dest = MakeExpression(ExpressionNode::add_, data,
                                  MakeIntExpression(ExpressionNode::c_i_, i * (initializerListType->size)));
            if (initializerListType->IsStructured())
            {
                initializerListType = initializerListType->BaseType();
                if (initializerListType->sp->sb->trivialCons)
                {
                    auto list = &node;
                    auto ita = (*itl)->nested ? (*itl)->nested->begin() : itl;
                    auto itae = (*itl)->nested ? (*itl)->nested->end() : initial->end();

                    SymbolTable<SYMBOL>::iterator it;
                    for (it = initializerListType->syms->begin(); ita != itae && it != initializerListType->syms->end();
                        ++it, ++ita)
                    {
                        auto sym = *it;
                        if (ismemberdata(sym))
                        {
                            auto pos = MakeExpression(ExpressionNode::structadd_, dest,
                                MakeIntExpression(ExpressionNode::c_i_, sym->sb->offset));
                            Dereference(sym->tp, &pos);
                            auto node1 = MakeExpression(ExpressionNode::assign_, pos, (*ita)->exp);
                            if (node)
                            {
                                *list = MakeExpression(ExpressionNode::comma_, *list, node1);
                                list = &(*list)->right;
                            }
                            else
                            {
                                node = node1;
                            }
                        }
                    }
                    for (; it != initializerListType->syms->end(); ++it)
                    {
                        auto sym = *it;
                        if (ismemberdata(sym))
                        {
                            auto pos = MakeExpression(ExpressionNode::structadd_, dest,
                                MakeIntExpression(ExpressionNode::c_i_, sym->sb->offset));
                            Dereference(sym->tp, &pos);
                            auto node1 = MakeExpression(ExpressionNode::assign_, pos, MakeIntExpression(ExpressionNode::c_i_, 0));
                            if (node)
                            {
                                *list = MakeExpression(ExpressionNode::comma_, *list, node1);
                                list = &(*list)->right;
                            }
                            else
                            {
                                node = node1;
                            }
                        }
                    }
                }
                else if (!(*itl)->initializer_list && (*itl)->nested)
                {
                    auto ita = (*itl)->nested->begin();
                    auto itae = (*itl)->nested->end();
                    node = nullptr;
                    auto list = &node;
                    int count1 = 0;
                    for (; ita != itae; ++ita)
                    {
                        Type* ctype = initializerListType;
                        auto pos = copy_expression(
                            MakeExpression(ExpressionNode::add_, dest,
                                           MakeIntExpression(ExpressionNode::c_i_, count1++ * initializerListType->size)));
                        CallSite* params = Allocate<CallSite>();
                        params->ascall = true;
                        params->thisptr = pos;
                        Argument* next = Allocate<Argument>();
                        *next = **ita;
                        params->arguments = argumentListFactory.CreateList();
                        params->arguments->push_back(next);
                        CallConstructor(&ctype, &pos, params, false, nullptr, true, false, false, false, _F_INITLIST, false, true);
                        if (node)
                        {
                            *list = MakeExpression(ExpressionNode::comma_, *list, pos);
                            list = &(*list)->right;
                        }
                        else
                        {
                            node = pos;
                        }
                    }
                    tp->size += (count1 - 1) * (initializerListType->size);
                    tp->esize->v.i += count1 - 1;
                }
                else if (!(*itl)->nested && initializerListType->IsStructured() &&
                         (initializerListType->CompatibleType((*itl)->tp) || SameTemplate(initializerListType, (*itl)->tp)))
                {
                    node = (*itl)->exp;
                    int offs;
                    auto exp = getFunc((*itl)->exp);
                    if (exp)
                    {
                        if (exp->v.func->sp->sb->isConstructor)
                        {
                            if (exp->v.func->thisptr)
                            {
                                int offs = 0;
                                auto exp1 = relptr(exp->v.func->thisptr, offs);
                                if (exp1)
                                    exp1->v.sp->sb->allocate = false;
                                exp->v.func->thisptr = dest;
                            }
                        }
                        else if (exp->v.func->sp->tp->BaseType()->btp->IsRef())
                        {
                            Type* ctype = initializerListType;
                            EXPRESSION* cdest = dest;
                            CallSite* params = Allocate<CallSite>();
                            params->arguments = argumentListFactory.CreateList();
                            params->arguments->push_back(*itl);

                            CallConstructor(&ctype, &cdest, params, false, nullptr, true, false, false, false, _F_INITLIST, false,
                                            true);
                            node = cdest;
                        }
                    }
                    else
                    {
                        Type* ctype = initializerListType;
                        EXPRESSION* cdest = dest;
                        CallSite* params = Allocate<CallSite>();
                        params->arguments = argumentListFactory.CreateList();
                        params->arguments->push_back(*itl);

                        CallConstructor(&ctype, &cdest, params, false, nullptr, true, false, false, false, _F_INITLIST, false,
                                        true);
                        node = cdest;
                    }
                }
                else
                {
                    Type* ctype = initializerListType;
                    EXPRESSION* cdest = dest;
                    CallSite* params = Allocate<CallSite>();
                    if ((*itl)->nested)
                    {
                        params->arguments = (*itl)->nested;
                    }
                    else
                    {
                        params->arguments = argumentListFactory.CreateList();
                        params->arguments->push_back(*itl);
                    }

                    CallConstructor(&ctype, &cdest, params, false, nullptr, true, false, false, false, _F_INITLIST, false, true);
                    node = cdest;
                }
            }
            else
            {
                auto ita = (*itl)->nested ? (*itl)->nested->begin() : itl;
                auto itae = (*itl)->nested ? (*itl)->nested->end() : initial->end();
                if ((*itl)->nested)
                {
                    node = nullptr;
                    auto list = &node;
                    int count1 = 0;
                    for (; ita != itae; ++ita)
                    {
                        auto pos = MakeExpression(ExpressionNode::structadd_, dest,
                                                  MakeIntExpression(ExpressionNode::c_i_, count1++ * initializerListType->size));
                        Dereference(initializerListType, &pos);
                        auto node1 = MakeExpression(ExpressionNode::assign_, pos, (*ita)->exp);
                        listOfScalars.push_back((*ita)->exp);
                        if (node)
                        {
                            *list = MakeExpression(ExpressionNode::comma_, *list, node1);
                            list = &(*list)->right;
                        }
                        else
                        {
                            node = node1;
                        }
                    }
                    tp->size += (count1 - 1) * (initializerListType->size);
                    tp->esize->v.i += count1 - 1;
                }
                else
                {
                    Dereference(initializerListType, &dest);
                    node = MakeExpression(ExpressionNode::assign_, dest, (*ita)->exp);
                }
            }
            if (rv)
            {
                *pos = MakeExpression(ExpressionNode::comma_, *pos, node);
                pos = &(*pos)->right;
            }
            else
            {
                rv = node;
            }
        }
        initList = AnonymousVar(StorageClass::auto_, initializerListTemplate);
        initList->v.sp->sb->constexpression = true;
        if (func && func->sb->constexpression && !listOfScalars.empty())
        {
            if (!initList->v.sp->sb->init)
                initList->v.sp->sb->init = initListFactory.CreateList();
            for (auto t : listOfScalars)
            {
                auto init = Allocate<Initializer>();
                init->exp = t;
                initList->v.sp->sb->init->push_back(init);
            }
        }
        dest = MakeExpression(ExpressionNode::structadd_, initList, MakeIntExpression(ExpressionNode::c_i_, begin->sb->offset));
        Dereference(&stdpointer, &dest);
        dest = MakeExpression(ExpressionNode::assign_, dest, data);
        if (rv)
        {
            *pos = MakeExpression(ExpressionNode::comma_, *pos, dest);
            pos = &(*pos)->right;
        }
        else
        {
            rv = dest;
        }
        dest = MakeExpression(ExpressionNode::structadd_, initList, MakeIntExpression(ExpressionNode::c_i_, size->sb->offset));
        Dereference(&stdpointer, &dest);
        dest = MakeExpression(ExpressionNode::assign_, dest,
                              MakeIntExpression(ExpressionNode::c_i_, tp->size / initializerListType->size));
        if (rv)
        {
            *pos = MakeExpression(ExpressionNode::comma_, *pos, dest);
            pos = &(*pos)->right;
        }
        else
        {
            rv = dest;
        }
        initial = (*lptr) = argumentListFactory.CreateList();
        initial->push_back(Allocate<Argument>());
        if (asref)
        {
            initial->front()->tp = Type::MakeType(BasicType::lref_, initializerListTemplate);
            initial->front()->exp = MakeExpression(ExpressionNode::comma_, rv, initList);
        }
        else
        {
            initial->front()->tp = initializerListTemplate;
            initial->front()->exp =
                MakeExpression(ExpressionNode::stackblock_, MakeExpression(ExpressionNode::comma_, rv, initList));
            initial->front()->exp->size = initializerListTemplate;
        }
        if (ildest)
        {
            initial->front()->destructors = new std::list<EXPRESSION*>();
            initial->front()->destructors->push_front(ildest);
        }
    }
}
void PromoteConstructorArgs(SYMBOL* cons1, CallSite* params)
{
    if (!cons1)
    {
        return;
    }
    auto it = cons1->tp->BaseType()->syms->begin();
    auto ite = cons1->tp->BaseType()->syms->end();
    if ((*it)->sb->thisPtr)
        ++it;
    std::list<Argument*>::iterator args, argse;
    if (params->arguments)
    {
        args = params->arguments->begin();
        argse = params->arguments->end();
    }
    while (it != ite && args != argse)
    {
        SYMBOL* sp = *it;
        Type* tps = sp->tp->BaseType();
        Type* tpa = (*args)->tp->BaseType();
        if (tps->IsArithmetic() && tpa->IsArithmetic())
        {
            if (tps->type > BasicType::int_ && tps->type != tpa->type)
            {
                (*args)->tp = sp->tp;
                cast(sp->tp, &(*args)->exp);
            }
        }
        ++it;
        ++args;
    }
}
EXPRESSION* convertArgToRef(EXPRESSION* exp, Type* tp, Type* boundTP)
{
    if (exp->type == ExpressionNode::hook_)
    {
        exp->right->left = convertArgToRef(exp->right->left, tp, boundTP);
        exp->right->right = convertArgToRef(exp->right->right, tp, boundTP);
    }
    else if (!TakeAddress(&exp, tp))
    {
        Type* tp1 = tp->BaseType()->btp;
        // make numeric temp and perform cast
        if (tp1->IsArray())
        {
            Type tp2 = {};
            Type::MakeType(tp2, BasicType::lref_, &stdpointer);
            exp = createTemporary(&tp2, exp);
        }
        else if (!tp->IsRef() || exp->type != ExpressionNode::callsite_ ||
                 (boundTP->type == BasicType::aggregate_ ||
                  (exp->v.func->sp->tp->IsFunction() && !exp->v.func->sp->tp->BaseType()->btp->IsRef())))
        {
            exp = createTemporary(tp, exp);
        }
        TakeAddress(&exp, tp);
    }

    return exp;
}
void AdjustParams(SYMBOL* func, SymbolTable<SYMBOL>::iterator it, SymbolTable<SYMBOL>::iterator itend, std::list<Argument*>** lptr,
                  bool operands, bool implicit)
{
    std::list<Argument*>::iterator itl, itle;
    if (*lptr)
    {
        itl = (*lptr)->begin();
        itle = (*lptr)->end();
    }
    auto old = argFriend;
    argFriend = func;
    (void)operands;
    if (func->sb->storage_class == StorageClass::overloads_)
        return;
    if (it != itend && (*it)->sb->thisPtr)
        ++it;
    while (it != itend && (itl != itle || (*it)->sb->init != nullptr ||
                           ((!IsDefiningTemplate()) && initTokenStreams.get(*it) != nullptr)))
    {
        SYMBOL* sym = *it;
        Argument* p;

        if (itl == itle)
        {
            if (!*lptr)
            {
                *lptr = argumentListFactory.CreateList();
                itl = (*lptr)->begin();
                itle = (*lptr)->end();
            }
            deferredInitializeDefaultArg(sym, func);
            EXPRESSION* q;
            if (sym->sb->init && sym->sb->init->front()->exp)
                q = sym->sb->init->front()->exp;
            else
                q = MakeIntExpression(ExpressionNode::c_i_, sym->tp->size);
            optimize_for_constants(&q);
            auto il = Allocate<Argument>();
            il->exp = q;
            il->tp = sym->tp->BaseType();
            (*lptr)->push_back(il);
            itle = (*lptr)->end();
            itl = itle;
            --itl;
            if (cloneTempExpr(&il->exp, nullptr, nullptr))
            {
                int i;
                SYMBOL *ths[CLONED_SYMS], *newval[CLONED_SYMS];
                memset(ths, 0, sizeof(ths));
                memset(newval, 0, sizeof(newval));
                cloneTempExpr(&il->exp, &ths[0], &newval[0]);
            }
            if (sym->tp->IsStructured())
            {
                il->exp = MakeExpression(ExpressionNode::stackblock_, il->exp);
                il->exp->size = sym->tp;
                ++it;
                ++itl;
                continue;
            }
        }
        p = *itl;
        if (p->exp && (p->exp->type == ExpressionNode::pc_ || p->exp->type == ExpressionNode::callsite_))
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                ValidateMSILFuncPtr(func->tp, p->tp, &p->exp);
            }
            thunkForImportTable(&p->exp);
        }
        if (Optimizer::cparams.prm_cplusplus)
        {
            auto exp2 = p->exp;
            if (exp2 && exp2->type == ExpressionNode::thisref_)
                exp2 = exp2->left;

            if (!sym->tp->IsStructured() && p->tp)
            {
                if (p->tp->IsStructured())
                {
                    Type* btp = func->tp->BaseType()->btp;
                    if (btp && (!btp->IsRef() || !btp->BaseType()->btp->IsStructured()))
                    {
                        SYMBOL* sym = nullptr;
                        auto exp2 = p->exp;
                        if (exp2 && exp2->type == ExpressionNode::thisref_)
                            exp2 = exp2->left;
                        if (exp2 && exp2->type == ExpressionNode::callsite_)
                            sym = exp2->v.func->returnSP;
                        if (sym && !sym->sb->destructed && sym->sb->dest && sym->sb->dest->front()->exp &&
                            !sym->tp->BaseType()->sp->sb->trivialCons)
                        {
                            exp2->v.func->returnSP->sb->destructed = true;
                            if (!p->destructors)
                                p->destructors = exprListFactory.CreateList();
                            p->destructors->push_front(exp2->v.func->returnSP->sb->dest->front()->exp);
                        }
                    }
                }
                else
                {
                    GetLogicalDestructors(&p->destructors, p->exp);
                }
            }
            bool done = false;
            if (!done && !p->tp && sym->tp->IsStructured() && sym->tp->BaseType()->sp->sb->initializer_list)
            {
                // initlist
                std::list<Argument*>::iterator itpinit, itpinite = itpinit;
                if (p->nested)
                {
                    itpinit = p->nested->begin();
                    itpinite = p->nested->end();
                }
                if (sym->tp->IsStructured() || (sym->tp->IsRef() && sym->tp->BaseType()->btp->IsStructured()))
                {
                    Type* stype = sym->tp;
                    SYMBOL* sp;
                    EXPRESSION* thisptr;
                    if (stype->IsRef())
                        stype = stype->BaseType()->btp;
                    thisptr =
                        AnonymousVar(theCurrentFunc || !sym->tp->IsRef() ? StorageClass::auto_ : StorageClass::localstatic_, stype);
                    sp = thisptr->v.sp;
                    if (!theCurrentFunc)
                    {
                        sp->sb->label = Optimizer::nextLabel++;
                        insertInitSym(sp);
                    }
                    if (stype->BaseType()->sp->sb->trivialCons)
                    {
                        std::list<Initializer*> init1;
                        std::list<Initializer*>* init = {&init1};
                        for (auto shr : *stype->BaseType()->syms)
                        {
                            if (itpinit == itpinite)
                                break;
                            if (ismemberdata(shr))
                            {
                                InsertInitializer(&init, (*itpinit)->tp, (*itpinit)->exp, shr->sb->offset, false);
                                ++itpinit;
                            }
                        }
                        p->exp = ConverInitializersToExpression(stype, nullptr, nullptr, theCurrentFunc, init, thisptr, false);
                        if (!sym->tp->IsRef())
                            sp->sb->stackblock = true;
                        done = true;
                    }
                    else
                    {
                        CallSite* params = Allocate<CallSite>();
                        params->ascall = true;
                        Type* ctype = sp->tp;
                        EXPRESSION* dexp = thisptr;
                        params->thisptr = thisptr;
                        auto itx = itpinit;
                        if (itpinit != itpinite)
                            ++itx;
                        params->arguments = argumentListFactory.CreateList();
                        if ((itpinit != itpinite && itx != itpinite) ||
                            (itpinit == itpinite && !p->tp && !p->exp))  // empty initializer list)
                        {
                            Type* tp;
                            if (itpinit == itpinite)
                            {
                                params->arguments->push_back(p);
                                tp = &stdint;
                            }
                            else
                            {
                                params->arguments->insert(params->arguments->begin(), itl, itle);
                                tp = (*itpinit)->tp;
                            }
                            p->tp = tp->InitializerListType();
                            p->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                            CreateInitializerList(nullptr, p->tp, tp, &params->arguments, true, sym->tp->IsRef());
                            **itl = *params->arguments->front();
                            (*itl)->tp = sym->tp;
                            if (itpinit == itpinite)
                            {
                                auto itl1 = itl;
                                ++itl1;
                                params->arguments->insert(params->arguments->end(), itl1, itle);
                            }
                        }
                        else
                        {
                            params->arguments->push_back(p);
                            p->exp = thisptr;
                            CallConstructor(&ctype, &p->exp, params, false, nullptr, true, false, true, false, true, false, true);
                        }

                        if (!sym->tp->IsRef())
                        {
                            sp->sb->stackblock = true;
                        }
                        else
                        {
                            CallDestructor(stype->sp, nullptr, &dexp, nullptr, true, false, false, true);
                            if (dexp)
                            {
                                if (!p->destructors)
                                    p->destructors = exprListFactory.CreateList();
                                p->destructors->push_front(dexp);
                            }
                        }
                        done = true;
                    }
                    p->tp = sym->tp;
                }
                else if (sym->tp->IsPtr())
                {
                    EXPRESSION* thisptr = AnonymousVar(theCurrentFunc ? StorageClass::auto_ : StorageClass::localstatic_, sym->tp);
                    SYMBOL* sp = thisptr->v.sp;
                    int n = 0;
                    Type* btp = sym->tp;
                    while (btp->IsArray())
                        btp = btp->BaseType()->btp;
                    if (!theCurrentFunc)
                    {
                        sp->sb->label = Optimizer::nextLabel++;
                        insertInitSym(sp);
                    }
                    if (!sym->tp->IsArray())
                    {
                        auto itx = itpinit;
                        while (itx != itpinite)

                        {
                            n++;
                            ++itx;
                        }
                        sym->tp = sym->tp->CopyType();
                        sym->tp->array = true;
                        sym->tp->esize = MakeIntExpression(ExpressionNode::c_i_, n);
                        sym->tp->UpdateRootTypes();
                        sym->tp->size = btp->size * n;
                    }
                    n = 0;
                    std::list<Initializer*> init1;
                    std::list<Initializer*>* init = &init1;
                    for (; itpinit != itpinite; ++itpinit)
                    {
                        InsertInitializer(&init, (*itpinit)->tp, (*itpinit)->exp, n, false);
                        n += btp->size;
                    }
                    p->exp = ConverInitializersToExpression(sym->tp, nullptr, nullptr, theCurrentFunc, init, thisptr, false);
                    p->tp = sym->tp;
                    done = true;
                }
                else
                {
                    // defer to below processing
                    if (itpinit != itpinite)
                    {
                        p->exp = (*itpinit)->exp;
                        p->tp = (*itpinit)->tp;
                    }
                    else
                    {
                        p->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        p->tp = &stdint;
                    }
                }
            }
            if (!done && (p->exp || p->nested))
            {
                if (sym->tp->type == BasicType::ellipse_)
                {
                    if (!p->tp->IsStructured() && (p->tp->lref || p->tp->rref))
                        if (p->tp->IsFunction())
                            p->exp = MakeExpression(ExpressionNode::l_ref_, p->exp);
                }
                else if (sym->tp->IsStructured())
                {
                    bool sameType = false;
                    EXPRESSION* temp = p->exp;
                    Type* tpx = p->tp;
                    if (!tpx)
                        tpx = sym->tp;
                    if (tpx->IsRef())
                        tpx = tpx->BaseType()->btp;
                    // result of a nested constructor
                    if (temp && temp->type == ExpressionNode::thisref_)
                    {
                        temp = p->exp->left;
                    }
                    if (p->nested)
                    {
                        Type* ctype = sym->tp->BaseType();
                        EXPRESSION* consexp = AnonymousVar(StorageClass::auto_, ctype);  // StorageClass::parameter_ to push it...
                        SYMBOL* esp = consexp->v.sp;
                        p->exp = consexp;
                        esp->sb->stackblock = true;
                        esp->sb->constexpression = true;
                        if (sym->tp->BaseType()->sp->sb->trivialCons)
                        {
                            auto initListType = p->nested->front()->tp->BaseType();
                            EXPRESSION* ptr = AnonymousVar(StorageClass::auto_, &stdpointer);
                            ptr->v.sp->sb->constexpression = true;
                            if (!theCurrentFunc)
                            {
                                ptr->v.sp->sb->label = Optimizer::nextLabel++;
                                insertInitSym(ptr->v.sp);
                            }
                            Dereference(&stdpointer, &ptr);
                            EXPRESSION** last = &p->exp;
                            *last = MakeExpression(ExpressionNode::assign_, ptr, consexp);
                            auto it = ctype->syms->begin();
                            int count = 0;
                            for (auto init : *p->nested)
                            {
                                while (it != ctype->syms->end() && !ismemberdata(*it))
                                    ++it;
                                if (it == ctype->syms->end())
                                    break;
                                count++;
                                auto exp2 = MakeExpression(ExpressionNode::structadd_, ptr,
                                                           MakeIntExpression(ExpressionNode::c_i_, (*it)->sb->offset));
                                Dereference(initListType, &exp2);
                                auto exp1 = MakeExpression(ExpressionNode::assign_, exp2, init->exp);
                                *last = MakeExpression(ExpressionNode::comma_, *last, exp1);
                                last = &(*last)->right;
                                ++it;
                            }
                            if (count < p->nested->size())
                            {
                                error(ERR_TOO_MANY_InitializerS);
                            }
                            else if (it != ctype->syms->end())
                            {
                                while (it != ctype->syms->end())
                                {
                                    while (it != ctype->syms->end() && !ismemberdata(*it))
                                        ++it;
                                    if (it != ctype->syms->end())
                                    {
                                        auto exp2 = MakeExpression(ExpressionNode::structadd_, ptr,
                                                                   MakeIntExpression(ExpressionNode::c_i_, (*it)->sb->offset));
                                        Dereference(initListType, &exp2);
                                        auto exp1 = MakeExpression(ExpressionNode::assign_, exp2,
                                                                   MakeIntExpression(ExpressionNode::c_i_, 0));
                                        *last = MakeExpression(ExpressionNode::comma_, *last, exp1);
                                        last = &(*last)->right;
                                        ++it;
                                    }
                                }
                            }
                            *last = MakeExpression(ExpressionNode::comma_, *last, ptr);
                        }
                        else
                        {
                            CallSite* params = Allocate<CallSite>();
                            params->ascall = true;
                            if (p->nested)
                            {
                                params->arguments = p->nested;
                            }
                            else
                            {
                                params->arguments = argumentListFactory.CreateList();
                                params->arguments->push_back(p);
                            }
                            CallConstructor(&ctype, &p->exp, params, false, nullptr, true, false, false, false, 0, false, true);
                            if (p->exp->type == ExpressionNode::thisref_)
                            {
                                Type* tpx = p->exp->left->v.func->sp->tp->BaseType();
                                if (tpx->IsFunction() && tpx->sp && tpx->sp->sb->castoperator)
                                {
                                    p->tp = tpx->btp;
                                }
                            }
                        }
                    }
                    // use constructor or conversion function and push on stack ( no destructor)
                    else if (temp->type == ExpressionNode::callsite_ && temp->v.func->sp->tp->BaseType()->btp &&
                             !temp->v.func->sp->tp->BaseType()->btp->IsRef() &&
                             ((sameType = sym->tp->CompatibleType(tpx)) ||
                              classRefCount(sym->tp->BaseType()->sp, tpx->BaseType()->sp) == 1))
                    {
                        SYMBOL* esp;
                        EXPRESSION* consexp;
                        // copy constructor...
                        Type* ctype = sym->tp;
                        EXPRESSION* paramexp;
                        consexp = AnonymousVar(StorageClass::auto_, sym->tp);  // StorageClass::parameter_ to push it...
                        esp = consexp->v.sp;
                        esp->sb->stackblock = true;
                        esp->sb->constexpression = true;
                        consexp = MakeExpression(ExpressionNode::auto_, esp);
                        paramexp = p->exp;
                        paramexp = DerivedToBase(sym->tp, tpx, paramexp, _F_VALIDPOINTER);
                        auto exp = consexp;
                        callConstructorParam(&ctype, &exp, sym->tp, paramexp, true, true, implicit, false, true);
                        // copy elision
                        if (p->exp->type != ExpressionNode::thisref_ || !p->exp->left->v.func->thistp ||
                            (!sym->tp->SameType(p->exp->left->v.func->thistp) &&
                             !SameTemplate(sym->tp, p->exp->left->v.func->thistp)) ||
                            !exp || exp->type != ExpressionNode::thisref_ || !exp->left->v.func->sp->sb->isConstructor ||
                            !matchesCopy(exp->left->v.func->sp, false))
                        {
                            if (exp->type == ExpressionNode::auto_)  // recursive call to constructor A<U>(A<U>)
                            {
                                if (temp->v.func->returnEXP)
                                {
                                    temp->v.func->returnEXP = consexp;
                                }
                                else
                                {
                                    if (p->exp->type == ExpressionNode::thisref_ && p->exp->left->v.func->thisptr)
                                        p->exp->left->v.func->thisptr = consexp;
                                    p->exp = paramexp;
                                }
                            }
                            else
                            {
                                p->exp = exp;
                            }
                            if (p->exp->type != ExpressionNode::comma_)
                                p->exp = MakeExpression(ExpressionNode::comma_, p->exp, consexp);
                        }
                    }
                    else if (sym->tp->BaseType()->sp->sb->trivialCons)
                    {
                        p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                        p->exp->size = p->tp;
                    }
                    else
                    {
                        Type* ctype = sym->tp = sym->tp->InitializeDeferred()->BaseType();
                        EXPRESSION* consexp = AnonymousVar(StorageClass::auto_, ctype);  // StorageClass::parameter_ to push it...
                        SYMBOL* esp = consexp->v.sp;
                        EXPRESSION* paramexp = p->exp;
                        esp->sb->stackblock = true;
                        esp->sb->constexpression = true;
                        auto currentFunc = theCurrentFunc;
                        theCurrentFunc = func;
                        callConstructorParam(&ctype, &consexp, p->tp, paramexp, true, true, implicit, false, true);
                        theCurrentFunc = currentFunc;
                        if (consexp->type == ExpressionNode::auto_)  // recursive call to constructor A<U>(A<U>)
                        {
                            p->exp = paramexp;
                        }
                        else
                        {
                            if (consexp->type == ExpressionNode::thisref_)
                                esp->sb->constexpression = false;
                            p->exp = consexp;
                        }
                    }
                    p->tp = sym->tp;
                }
                else if (sym->tp->IsRef())
                {
                    bool nested = false;
                    Type* tpx1 = p->tp;
                    if (!tpx1)
                    {
                        tpx1 = sym->tp;
                    }
                    if (sym->tp->BaseType()->btp->IsStructured())
                    {
                        Type* tpx = tpx1;
                        if (tpx->IsRef())
                            tpx = tpx->BaseType()->btp;
                        if (tpx->type == BasicType::templateparam_)
                        {
                            tpx = tpx->templateParam->second->byClass.val;
                            if (!tpx)
                                tpx = sym->tp->BaseType()->btp;
                        }
                        if (p->exp)
                        {
                            int offset = 0;
                            auto exp4 = relptr(p->exp, offset);
                        }
                        if (p->nested)
                        {
                            nested = true;
                            CallSite* params = Allocate<CallSite>();
                            params->ascall = true;
                            if (p->nested)
                            {
                                params->arguments = p->nested;
                            }
                            else
                            {
                                params->arguments = argumentListFactory.CreateList();
                                params->arguments->push_back(p);
                            }
                            Type* ctype = sym->tp->BaseType()->btp;
                            EXPRESSION* consexp = AnonymousVar(StorageClass::auto_,
                                                               sym->tp->BaseType()->btp);  // StorageClass::parameter_ to push it...
                            SYMBOL* esp = consexp->v.sp;
                            p->exp = consexp;
                            CallConstructor(&ctype, &p->exp, params, false, nullptr, true, false, false, false, 0, false, true);
                            if (p->exp->type == ExpressionNode::thisref_)
                            {
                                Type* tpx = p->exp->left->v.func->sp->tp->BaseType();
                                if (tpx->IsFunction() && tpx->sp && tpx->sp->sb->castoperator)
                                {
                                    p->tp = tpx->btp;
                                }
                            }
                            EXPRESSION* dexp = consexp;
                            CallDestructor(esp->tp->BaseType()->sp, nullptr, &dexp, nullptr, true, false, false, true);
                            InsertInitializer(&esp->sb->dest, sym->tp->BaseType()->btp, dexp, 0, true);
                        }
                        else if ((!sym->tp->BaseType()->btp->IsConst() && !sym->tp->IsConst() &&
                                  (sym->tp->type != BasicType::rref_ &&
                                   (!func->sb->templateLevel &&
                                    (!func->sb->parentClass || !func->sb->parentClass->sb->templateLevel) /*forward*/)) &&
                                  tpx->IsConst()) ||
                                 (!sym->tp->CompatibleType(tpx) && !SameTemplate(sym->tp, tpx) &&
                                  !classRefCount(sym->tp->BaseType()->btp->BaseType()->sp, tpx->BaseType()->sp)))
                        {
                            // make temp via constructor or conversion function
                            EXPRESSION* consexp = AnonymousVar(StorageClass::auto_,
                                                               sym->tp->BaseType()->btp);  // StorageClass::parameter_ to push it...
                            SYMBOL* esp = consexp->v.sp;
                            Type* ctype = sym->tp->BaseType()->btp;
                            EXPRESSION* paramexp = p->exp;
                            p->exp = consexp;
                            callConstructorParam(&ctype, &p->exp, p->tp->BaseType(), paramexp, true, true, false, false, true);
                            bool dodest = true;
                            if (p->exp->type == ExpressionNode::thisref_)
                            {
                                Type* tpx = p->exp->left->v.func->sp->tp->BaseType();
                                if (tpx->IsFunction() && tpx->sp && tpx->sp->sb->castoperator)
                                {
                                    p->tp = tpx->btp;
                                    dodest = !p->tp->IsRef();
                                }
                            }
                            if (dodest)
                            {
                                EXPRESSION* dexp = consexp;
                                CallDestructor(esp->tp->BaseType()->sp, nullptr, &dexp, nullptr, true, false, false, true);
                                InsertInitializer(&esp->sb->dest, sym->tp->BaseType()->btp, dexp, 0, true);
                            }
                            else
                            {
                                esp->sb->allocate = false;
                            }
                        }
                        else
                        {
                            if (!sym->tp->CompatibleType(tpx))
                                p->exp = DerivedToBase(sym->tp, tpx, p->exp, 0);
                        }
                    }
                    else if (sym->tp->BaseType()->btp->IsArray() && p->nested)
                    {
                        Type* sourceType = p->nested->front()->tp;
                        Type* arrtype = sym->tp->BaseType()->btp;
                        Type* basetype = sym->tp->BaseType()->btp;
                        while (basetype->IsArray())
                            basetype = basetype->BaseType()->btp;
                        int elemsize = basetype->size;
                        int elems = arrtype->size / basetype->size;
                        EXPRESSION* consexp =
                            AnonymousVar(StorageClass::auto_, sym->tp->BaseType()->btp);  // StorageClass::parameter_ to push it...
                        auto it = p->nested->begin();
                        auto ite = p->nested->end();
                        EXPRESSION *rv = nullptr, **last = &rv;
                        if (!basetype->IsStructured())
                        {
                            // this is broken right now, arraysize is number of elements instead of total size...
                            *last = MakeExpression(ExpressionNode::blockclear_, copy_expression(consexp));
                            (*last)->size = Type::MakeType(BasicType::struct_);
                            (*last)->size->size = arrtype->size;
                        }
                        if (sourceType->IsArray())
                        {
                            error(ERR_ARRAY_TYPE_NOT_EXPECTED);
                        }
                        else
                        {
                            auto it = p->nested->begin();
                            auto ite = p->nested->end();
                            for (int i = 0; i < elems; i++)
                            {
                                bool skip = false;
                                EXPRESSION* baseAddress = MakeExpression(ExpressionNode::arrayadd_, consexp,
                                                                         MakeIntExpression(ExpressionNode::c_i_, i * elemsize));
                                EXPRESSION* next = nullptr;
                                if (basetype->IsStructured())
                                {
                                    Type* ctype = basetype;
                                    EXPRESSION* paramexp = it == ite ? nullptr : (*it)->exp;
                                    Type* paramtp = it == ite ? nullptr : (*it)->tp;
                                    p->exp = baseAddress;
                                    callConstructorParam(&ctype, &p->exp, paramtp, paramexp, true, true, false, false, true);
                                }
                                else if (it != ite)
                                {
                                    if (sourceType->IsStructured())
                                    {
                                        if (basetype->IsPtr())
                                        {
                                            auto ctype = basetype;
                                            p->exp = (*it)->exp;
                                            castToPointer(&ctype, &p->exp, (Keyword)-1, sourceType);
                                        }
                                        else
                                        {
                                            auto ctype = basetype;
                                            p->exp = (*it)->exp;
                                            castToArithmetic(basetype->IsInt(), &ctype, &p->exp, (Keyword)-1, sourceType, true);
                                        }
                                    }
                                    else
                                    {
                                        // both arithmetic
                                        p->exp = (*it)->exp;
                                        CheckNarrowing(basetype, sourceType, p->exp);
                                        if (basetype->BaseType()->type != sourceType->BaseType()->type)
                                        {
                                            cast(basetype, &p->exp);
                                        }
                                        auto left = baseAddress;
                                        Dereference(basetype, &left);
                                        p->exp = MakeExpression(ExpressionNode::assign_, left, p->exp);
                                    }
                                }
                                else
                                {
                                    skip = true;
                                }
                                if (!skip)
                                {
                                    optimize_for_constants(&p->exp);
                                    if (*last)
                                    {
                                        *last = MakeExpression(ExpressionNode::comma_, *last, p->exp);
                                        last = &(*last)->right;
                                    }
                                    else
                                    {
                                        *last = p->exp;
                                    }
                                    if (it != ite)
                                    {
                                        ++it;
                                    }
                                }
                            }
                            if (*last)
                            {
                                *last = MakeExpression(ExpressionNode::comma_, p->exp, consexp);
                                last = &(*last)->right;
                            }
                            else
                            {
                                *last = consexp;
                            }
                        }
                        p->exp = rv;
                    }
                    else if (sym->tp->BaseType()->btp->BaseType()->type == BasicType::memberptr_)
                    {
                        // in case there was an error somewhere
                        if (p->exp)
                        {
                            Type* tp2 = sym->tp->BaseType()->btp;
                            if (p->exp->type == ExpressionNode::memberptr_)
                            {
                                int lbl = dumpMemberPtr(p->exp->v.sp, tp2, true);
                                p->exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                            }
                            else if (isconstzero(p->tp, p->exp) || p->exp->type == ExpressionNode::nullptr_)
                            {
                                EXPRESSION* dest = createTemporary(tp2, nullptr);
                                p->exp = MakeExpression(ExpressionNode::blockclear_, dest);
                                p->exp->size = tp2;
                                p->exp = MakeExpression(ExpressionNode::comma_, p->exp, dest);
                            }
                            else if (p->exp->type == ExpressionNode::callsite_ && p->exp->v.func->returnSP)
                            {
                                int lbl = dumpMemberPtr(p->exp->v.sp, tp2, true);
                                p->exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                            }
                            else if (p->exp->type == ExpressionNode::pc_)
                            {
                                int lbl = dumpMemberPtr(p->exp->v.sp, tp2, true);
                                p->exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                            }
                        }
                        p->tp = sym->tp;
                    }
                    else if (sym->tp->CompatibleType(p->tp ? p->tp : p->nested->front()->tp))
                    {
                        Argument* p1;
                        if (p->tp)
                            p1 = p;
                        else
                            p1 = p->nested->front();
                        if (isarithmeticconst(p1->exp) || (sym->tp->BaseType()->type != BasicType::rref_ &&
                                                           !sym->tp->BaseType()->btp->IsConst() && p1->tp->IsConst()))
                        {
                            // make numeric temp and perform cast
                            if (p1->exp)
                            {
                                p->exp = createTemporary(sym->tp, p1->exp);
                            }
                        }
                        else
                        {
                            // pass address
                            EXPRESSION* exp = p1->exp;
                            while (IsCastValue(exp) || exp->type == ExpressionNode::not__lvalue_)
                                exp = exp->left;
                            if (exp->type != ExpressionNode::l_ref_)
                            {
                                if (!sym->tp->IsRef() ||
                                    (!sym->tp->BaseType()->btp->IsFunction() && !sym->tp->BaseType()->btp->IsArray()))
                                {
                                    if (exp->type == ExpressionNode::lvalue_)
                                    {
                                        exp = createTemporary(sym->tp, exp);
                                    }
                                    else
                                    {
                                        exp = convertArgToRef(exp, sym->tp, p1->tp);
                                    }
                                }
                                p->exp = exp;
                            }
                            else if (p->tp->IsPtr() && p->tp->BaseType()->btp->IsStructured())
                            {
                                // make numeric temp and perform cast
                                p->exp = createTemporary(sym->tp, exp);
                            }
                            else if (sym->tp->BaseType()->btp->BaseType()->byRefArray)
                            {
                                p->exp->left = p->exp->left->left;
                            }
                        }
                    }
                    // in case there was an error somewhere
                    else if (p->exp)
                    {
                        if (p->tp->IsStructured())
                        {
                            // arithmetic or pointer
                            Type* etp = sym->tp->BaseType()->btp;
                            if (cppCast(p->tp, &etp, &p->exp))
                                p->tp = etp;
                            if (!TakeAddress(&p->exp))
                                p->exp = createTemporary(sym->tp, p->exp);
                        }
                        else
                        {
                            // make numeric temp and perform cast
                            p->exp = createTemporary(sym->tp, p->exp);
                        }
                    }
                    if (!tpx1->IsRef() && ((tpx1->IsConst() && !sym->tp->BaseType()->btp->IsConst()) ||
                                           (tpx1->IsVolatile() && !sym->tp->BaseType()->btp->IsVolatile())))
                        if (sym->tp->BaseType()->type != BasicType::rref_)  // converting const lref to rref is ok...
                            if (!sym->tp->BaseType()->btp->IsStructured())  // structure constructor is ok
                                if (!tpx1->IsArray())
                                    error(ERR_REF_INITIALIZATION_DISCARDS_QUALIFIERS);
                    p->tp = sym->tp;
                }
                else if (p->tp->IsStructured())
                {
                    if (sym->tp->type == BasicType::ellipse_)
                    {
                        p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                        p->exp->size = p->tp;
                    }
                    else
                    {
                        // arithmetic or pointer
                        Type* etp = sym->tp;
                        if (cppCast(p->tp, &etp, &p->exp))
                            p->tp = etp;
                    }
                }
                else if (sym->tp->IsVoidPtr() && p->tp->type == BasicType::aggregate_)
                {
                    LookupSingleAggregate(p->tp, &p->exp);
                }
                else if (sym->tp->IsPtr() && p->tp->IsPtr())
                {
                    // handle vla to pointer conversion
                    if (p->tp->vla && !sym->tp->vla)
                    {
                        Type* tpd1 = Allocate<Type>();
                        *tpd1 = *p->tp;
                        tpd1->vla = false;
                        tpd1->array = false;
                        tpd1->size = getSize(BasicType::pointer_);
                        p->tp = tpd1;
                        Dereference(p->tp, &p->exp);
                    }
                    // handle base class conversion
                    Type* tpb = sym->tp->BaseType()->btp;
                    Type* tpd = p->tp->BaseType()->btp;

                    if (Optimizer::cparams.prm_cplusplus && p->tp->BaseType()->stringconst && !sym->tp->BaseType()->btp->IsConst())
                        error(ERR_INVALID_CHARACTER_STRING_CONVERSION);
                    if (!tpb->BaseType()->CompatibleType(tpd->BaseType()))
                    {
                        if (tpb->IsStructured() && tpd->IsStructured())
                        {
                            p->exp = DerivedToBase(tpb, tpd, p->exp, 0);
                        }
                        p->tp = sym->tp;
                    }
                }
                else if (sym->tp->BaseType()->type == BasicType::memberptr_)
                {
                    if (p->exp->type == ExpressionNode::memberptr_)
                    {
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, true);
                        p->exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                        p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                        p->exp->size = sym->tp;
                    }
                    else if (isconstzero(p->tp, p->exp) || p->exp->type == ExpressionNode::nullptr_)
                    {
                        EXPRESSION* dest = createTemporary(sym->tp, nullptr);
                        p->exp = MakeExpression(ExpressionNode::blockclear_, dest);
                        p->exp->size = sym->tp;
                        p->exp = MakeExpression(ExpressionNode::comma_, p->exp, dest);
                        p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                        p->exp->size = sym->tp;
                    }
                    else if (p->exp->type == ExpressionNode::callsite_ && p->exp->v.func->returnSP)
                    {
                        EXPRESSION* dest = AnonymousVar(StorageClass::auto_, sym->tp);
                        SYMBOL* esp = dest->v.sp;
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, true);
                        esp->sb->stackblock = true;
                        p->exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                        p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                        p->exp->size = sym->tp;
                    }
                    else if (p->exp->type == ExpressionNode::pc_)
                    {
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, true);
                        p->exp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                        p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                        p->exp->size = sym->tp;
                    }
                    else
                    {
                        p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                        p->exp->size = sym->tp;
                    }
                    p->tp = sym->tp;
                }
                else if (sym->tp->IsArithmetic() && p->tp->IsArithmetic())
                    if (sym->tp->BaseType()->type != p->tp->BaseType()->type)
                    {
                        p->tp = sym->tp;
                        cast(p->tp, &p->exp);
                    }
            }
        }
        else if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            if (!p->exp)
            {
                diag("adjust_params, empty expression");
                p->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            }
            if (sym->tp->IsRef())
            {
                if (sym->tp->CompatibleType(p->tp))
                {
                    if (isarithmeticconst(p->exp) ||
                        (sym->tp->BaseType()->type != BasicType::rref_ && !sym->tp->BaseType()->btp->IsConst() && p->tp->IsConst()))
                    {
                        // make numeric temp and perform cast
                        p->exp = createTemporary(sym->tp, p->exp);
                    }
                    else
                    {
                        // pass address
                        EXPRESSION* exp = p->exp;
                        while (IsCastValue(exp) || exp->type == ExpressionNode::not__lvalue_)
                            exp = exp->left;
                        if (exp->type != ExpressionNode::l_ref_)
                        {
                            if (!sym->tp->IsRef() || !sym->tp->BaseType()->btp->IsFunction())
                            {
                                if (!TakeAddress(&exp))
                                {
                                    // make numeric temp and perform cast
                                    exp = createTemporary(sym->tp, exp);
                                }
                            }
                            p->exp = exp;
                        }
                        else if (p->tp->IsPtr() && p->tp->BaseType()->btp->IsStructured())
                        {
                            // make numeric temp and perform cast
                            p->exp = createTemporary(sym->tp, exp);
                        }
                    }
                }
                else
                {
                    // make numeric temp and perform cast
                    p->exp = createTemporary(sym->tp, p->exp);
                }
                p->tp = sym->tp;
            }
            else
            {
                if (p && sym->tp->BaseType()->type == BasicType::string_)
                {
                    if ((p->tp->BaseType()->type == BasicType::string_) ||
                        (p->exp->type == ExpressionNode::labcon_ && p->exp->string))
                    {
                        if (p->exp->type == ExpressionNode::labcon_)
                            p->exp->type = ExpressionNode::c_string_;
                    }
                    else if ((p->tp->IsArray() || p->tp->IsPtr()) && !p->tp->BaseType()->msil &&
                             p->tp->BaseType()->btp->BaseType()->type == BasicType::char_)
                    {
                        // make a 'string' object and initialize it with the string
                        Type* ctype = find_boxed_type(sym->tp->BaseType());
                        EXPRESSION *exp1, *exp2;
                        exp1 = exp2 = AnonymousVar(StorageClass::auto_, &std__string);
                        callConstructorParam(&ctype, &exp2, p->tp, p->exp, true, true, false, false, true);
                        exp2 = MakeExpression(ExpressionNode::l_string_, exp2);
                        p->exp = exp2;
                        p->tp = &std__string;
                    }
                }
                else if (p && sym->tp->BaseType()->type == BasicType::object_)
                {
                    if (p->tp->BaseType()->type != BasicType::object_ && !p->tp->IsStructured() &&
                        (!p->tp->IsArray() || !p->tp->BaseType()->msil))
                    {
                        if ((p->tp->IsArray() || p->tp->IsPtr()) && !p->tp->BaseType()->msil &&
                            p->tp->BaseType()->btp->BaseType()->type == BasicType::char_)
                        {
                            // make a 'string' object and initialize it with the string
                            Type* ctype = find_boxed_type(&std__string);
                            EXPRESSION *exp1, *exp2;
                            exp1 = exp2 = AnonymousVar(StorageClass::auto_, &std__string);
                            callConstructorParam(&ctype, &exp2, p->tp, p->exp, true, true, false, false, true);
                            exp2 = MakeExpression(ExpressionNode::l_string_, exp2);
                            p->exp = exp2;
                            p->tp = &std__string;
                        }
                        else
                            p->exp = MakeExpression(ExpressionNode::x_object_, p->exp);
                    }
                }
                else if (p && p->tp->IsMsil())
                    ;  // error
                // legacy c language support
                else if (p && p->tp && p->tp->IsStructured() && (!p->tp->BaseType()->sp->sb->msil || !isconstzero(p->tp, p->exp)))
                {
                    p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                    p->exp->size = p->tp;
                }
            }
        }
        else
        {
            // legacy c language support
            if (p && p->tp)
            {
                if (p->tp->IsStructured())
                {
                    p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
                    p->exp->size = p->tp;
                }
                else if (sym->tp->IsFloat() || sym->tp->IsImaginary() || sym->tp->IsComplex())
                {
                    cast(sym->tp, &p->exp);
                    optimize_for_constants(&p->exp);
                }
            }
        }
        ++it;
        ++itl;
    }
    while (itl != itle)  // take care of elliptical arguments and arguments without a prototype
    {
        Argument* p = *itl;
        if (!p->tp || !p->exp)
        {
            p->tp = &stdint;
            p->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        }
        if (func->sb->msil && p->exp->type == ExpressionNode::labcon_ && p->exp->string)
        {
            p->exp->type = ExpressionNode::c_string_;
            p->tp = &(std__string);
        }
        if (p->tp->IsStructured())
        {
            p->exp = MakeExpression(ExpressionNode::stackblock_, p->exp);
            p->exp->size = p->tp;
            if (Optimizer::cparams.prm_cplusplus)
            {
                auto dexp = p->exp;
                CallDestructor(p->tp->BaseType()->sp, nullptr, &dexp, nullptr, true, false, false, true);
                if (dexp)
                {
                    if (!p->destructors)
                        p->destructors = exprListFactory.CreateList();
                    p->destructors->push_front(dexp);
                }
            }
        }
        else if (p->tp->type == BasicType::float_)
        {
            cast(&stddouble, &p->exp);
            GetLogicalDestructors(&p->destructors, p->exp);
        }
        else
        {
            GetLogicalDestructors(&p->destructors, p->exp);
        }
        ++itl;
    }
    argFriend = old;
}
static std::list<TEMPLATEPARAMPAIR>* LiftTemplateParams(std::list<TEMPLATEPARAMPAIR>* tplx)
{
    std::list<TEMPLATEPARAMPAIR>* rv = nullptr;
    if (tplx)
    {
        rv = templateParamPairListFactory.CreateList();
        for (auto tpl : *tplx)
        {
            auto second = Allocate<TEMPLATEPARAM>();
            *second = *tpl.second;
            rv->push_back(TEMPLATEPARAMPAIR{tpl.first, second});
            if (tpl.second->type == TplType::typename_ && !tpl.second->packed && tpl.second->byClass.dflt &&
                tpl.second->byClass.dflt->BaseType()->type == BasicType::templateparam_)
            {
                *rv->back().second = *tpl.second->byClass.dflt->BaseType()->templateParam->second;
                rv->back().first = tpl.second->byClass.dflt->BaseType()->templateParam->first;
            }
            else if (tpl.second->type == TplType::int_ && tpl.second->byNonType.dflt &&
                     tpl.second->byNonType.dflt->type == ExpressionNode::templateparam_)
            {
                *rv->back().second = *tpl.second->byNonType.dflt->v.sp->tp->templateParam->second;
                rv->back().first = tpl.second->byNonType.dflt->v.sp->tp->templateParam->first;
            }
            if (rv->back().first)
            {
                TEMPLATEPARAMPAIR* rv1 = nullptr;
                for (auto&& s : enclosingDeclarations)
                {
                    if (s.tmpl)
                    {
                        for (auto&& tpl1 : *s.tmpl)
                        {
                            if (tpl1.first && !strcmp(tpl1.first->name, rv->back().first->name))
                            {
                                rv1 = &tpl1;
                                if (rv1)
                                    break;
                            }
                        }
                    }
                }
                if (rv1)
                {
                    if (rv->back().second->packed)
                    {
                        rv->back().second->byPack.pack = nullptr;
                        if (rv1->second->byPack.pack)
                        {
                            rv->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                            for (auto tpl2 : *rv->front().second->byPack.pack)
                            {
                                auto second = Allocate<TEMPLATEPARAM>();
                                second->type = rv->back().second->type;
                                second->byClass.dflt = tpl2.second->byClass.val;
                                rv->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, second});
                            }
                        }
                    }
                    else
                    {
                        rv->back().second->byClass.dflt = rv->front().second->byClass.val;
                    }
                }
            }
        }
    }
    return rv;
}
void expression_arguments( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags, bool noLex)
{
    Type* tp_cpp = *tp;
    EXPRESSION* exp_cpp = *exp;
    CallSite* funcparams;
    EXPRESSION* exp_in = *exp;
    bool hasThisPtr = false;
    Type* initializerListType = nullptr;
    Type* initializerListTemplate = nullptr;
    bool initializerRef = false;
    bool addedThisPointer = false;
    bool memberPtr = false;
    if (exp_in->type == ExpressionNode::dotstar_ || exp_in->type == ExpressionNode::pointstar_)
    {
        funcparams = Allocate<CallSite>();
        auto right = exp_in->right->v.sp->tp;
        while (right->btp) right = right->btp;
        funcparams->sp = right->templateParam->first;
        funcparams->functp = funcparams->sp->tp;
        funcparams->fcall = exp_in->right;
        exp_in->right = MakeExpression(funcparams);
    }
    else if (exp_in->type != ExpressionNode::callsite_ || (*tp)->IsFunctionPtr() || (*tp)->IsStructured()) 
    {
        if (exp_in->type != ExpressionNode::templateselector_ || !processingTrailingReturnOrUsing)
        {
            Type* tpx = *tp;
            SYMBOL* sym;
            funcparams = Allocate<CallSite>();
            if (exp_in->type == ExpressionNode::templateselector_)
            {
                auto tsl = exp_in->v.templateSelector;
                SYMBOL* ts = (*tsl)[1].sp;
                SYMBOL* sp = ts;
                if ((!sp->sb || !sp->sb->instantiated) && (*tsl)[1].isTemplate)
                {
                    std::list<TEMPLATEPARAMPAIR>* current = (*tsl)[1].templateParams;
                    sp = GetClassTemplate(ts, current, true);
                }
                if (sp && sp->tp->type == BasicType::templateselector_)
                {
                    Type* tp = sp->tp;
                    tp = SynthesizeType(tp, nullptr, false);
                    if (tp && tp->IsStructured())
                        sp = tp->BaseType()->sp;
                }
                sym = nullptr;
                if (sp)
                {
                    sp->tp->InstantiateDeferred();
                    sp = sp->tp->sp;
                    auto find = (*tsl).begin();
                    ++find;
                    ++find;
                    for (; find != (*tsl).end() && sp;)
                    {
                        SYMBOL* spo = sp;
                        if (!spo->tp->IsStructured())
                            break;

                        sp = search(spo->tp->syms, find->name);
                        if (!sp)
                        {
                            sp = classdata(find->name, spo, nullptr, false, false);
                            if (sp == (SYMBOL*)-1)
                                sp = nullptr;
                        }
                        if (sp && sp->tp->type != BasicType::aggregate_ && sp->sb->access != AccessLevel::public_)
                        {
                            sp = nullptr;
                            break;
                        }
                        auto it1 = find;
                        ++it1;
                        if (it1 == (*tsl).end() && sp)
                        {
                            funcparams->astemplate = find->isTemplate;
                            funcparams->templateParams = LiftTemplateParams(find->templateParams);
                        }
                        ++find;
                    }
                    if (find == (*tsl).end())
                        sym = sp;
                }
            }
            else
            {
                if (tpx->type == BasicType::templateparam_)
                {
                    if (tpx->templateParam->second->type == TplType::typename_ && tpx->templateParam->second->byClass.val)
                    {
                        tpx = tpx->templateParam->second->byClass.val;
                        if (tpx->sp && tpx->sp->sb->storage_class == StorageClass::typedef_)
                            tpx = tpx->btp;
                    }
                }
                if (tpx->IsPtr())
                    tpx = tpx->BaseType()->btp;
                sym = tpx->BaseType()->sp;
            }
            if (sym)
            {
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = *exp;
                *exp = MakeExpression(funcparams);
            }
            else if (!templateDefinitionLevel)
            {
                error(ERR_CALL_OF_NONFUNCTION);
            }
        }
        else
        {
            funcparams = Allocate<CallSite>();
        }
    }
    else
    {
        if ((*tp)->IsFunction() && (*exp)->type == ExpressionNode::callsite_ && (*exp)->v.func->resolvedCall)
        {
            funcparams = Allocate<CallSite>();
            funcparams->sp = (*tp)->BaseType()->sp;
            funcparams->functp = (*tp)->BaseType();
            funcparams->fcall = *exp;
            if ((*tp)->lref || (*tp)->rref)
                funcparams->fcall = MakeExpression(ExpressionNode::l_ref_, *exp);
            *exp = MakeExpression(funcparams);
        }
        else
        {
            SYMBOL* ss = enclosingDeclarations.GetFirst();
            funcparams = exp_in->v.func;
            hasThisPtr = funcparams->thisptr != nullptr;
            if ((*tp)->BaseType()->sp)
                funcparams->sp = (*tp)->BaseType()->sp;
            if (ss)
            {
                funcparams->functp = ss->tp;
            }
        }
    }

    if (/*(!IsDefiningTemplate()) &&*/ funcparams->sp && funcparams->sp->name[0] == '_' &&
        parseBuiltInTypelistFunc(funcsp, funcparams->sp, tp, exp))
        return;

    if (!noLex)
    {
        getArgs(funcsp, funcparams, Keyword::closepa_, true, flags);
        if ((*exp)->type == ExpressionNode::templateselector_ && processingTrailingReturnOrUsing)
        {
            (*exp)->v.templateSelector->back().arguments = funcparams->arguments;
            (*exp)->v.templateSelector->back().asCall = true;
        }
    }
    if ((*exp)->type == ExpressionNode::templateselector_ && processingTrailingReturnOrUsing)
    {
        return;
    }

    if (funcparams->astemplate && (argumentNestingLevel || inStaticAssert))
    {
        // if we hit a packed template param here, then this is going to be a candidate
        // for some other function's packed expression
        if (funcparams->templateParams)
            for (auto tl : *funcparams->templateParams)
                if (tl.second->packed && !tl.second->ellipsis)
                    return;
    }
    if (*tp)
        getFunctionSP(tp);
    if ((*exp)->type == ExpressionNode::funcret_)
    {
        (*exp)->v.func = funcparams;
        *exp = MakeExpression(ExpressionNode::funcret_, *exp);
        return;
    }
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && funcparams->sp)
    {
        if (funcparams->sp->sb->storage_class == StorageClass::overloads_)
        {
            // note at this pointer the arglist does NOT have the this pointer,
            // it will be added after we select a member function that needs it.
            funcparams->ascall = true;
            SYMBOL* sym = GetOverloadedFunction(tp, &funcparams->fcall, funcparams->sp, funcparams, nullptr, true, false, flags);
            if (sym)
            {
                sym->sb->attribs.inheritable.used = true;
                funcparams->sp = sym;
                *tp = sym->tp;
            }
        }
    }
    else if ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) && funcparams->sp)
    {
        SYMBOL* sym = nullptr;
        // add this ptr
        if (!funcparams->thisptr && funcparams->sp->sb && funcparams->sp->sb->parentClass && !funcparams->sp->tp->IsFunctionPtr())
        {
            funcparams->thisptr = getMemberBase(funcparams->sp, nullptr, funcsp, false);
            Type* tp = Type::MakeType(BasicType::pointer_, funcparams->sp->sb->parentClass->tp);
            if (funcsp)
            {
                if (funcsp->tp->IsConst())
                {
                    tp->btp = Type::MakeType(BasicType::const_, tp->btp);
                }
                if (funcsp->tp->IsVolatile())
                {
                    tp->btp = Type::MakeType(BasicType::volatile_, tp->btp);
                }
            }
            funcparams->thistp = tp;
            tp->UpdateRootTypes();
            addedThisPointer = true;
        }
        // we may get here with the overload resolution already done, e.g.
        // for operator or cast function calls...
        // also if in a trailing return we want to defer the lookup until later if there are going to be multiple choices...
        static EXPRESSION funcExpr;
        funcExpr.type = ExpressionNode::callsite_;
        funcExpr.v.func = funcparams;
        if (funcparams->sp->sb && funcparams->sp->sb->storage_class == StorageClass::overloads_ &&
            (funcparams->sp->tp->type != BasicType::aggregate_ || funcparams->sp->tp->syms->size() < 2 ||
             (!processingTrailingReturnOrUsing && (!MATCHKW(Keyword::ellipse_) || unpackingTemplate) &&
              (!(flags & _F_INDECLTYPE) || !processingTemplateArgs || unpackingTemplate || !hasPackedExpression(&funcExpr, true)))))
        {
            Type* tp1;
            // note at this pointer the arglist does NOT have the this pointer,
            // it will be ad  ded after we select a member function that needs it.
            funcparams->ascall = true;
            sym = GetOverloadedFunction(tp, &funcparams->fcall, funcparams->sp, funcparams, nullptr,
                                        !(MATCHKW(Keyword::ellipse_)), false, flags);
            if ((*tp)->IsFunction())
            {
                if ((*tp)->BaseType()->btp->IsStructured())
                {
                    // get rid of any invalid sizing
                    Type* tpx = (*tp)->BaseType()->btp->BaseType();
                    tpx->size = tpx->sp->tp->size;
                }
            }
            tp1 = *tp;
            while (tp1->btp && tp1->type != BasicType::bit_)
                tp1 = tp1->btp;
            if (sym)
            {
                sym->sb->attribs.inheritable.used = true;
                if (sym->sb->decoratedName[0] == '@' && currentLex)
                    browse_usage(sym, currentLex->linedata->fileindex);
                if (funcparams->astemplate && sym->sb->templateLevel && !sym->sb->specialized)
                {
                    std::list<TEMPLATEPARAMPAIR>::iterator tpln, tplne;
                    if (funcparams->templateParams)
                    {
                        tpln = funcparams->templateParams->begin();
                        tplne = funcparams->templateParams->end();
                    }
                    auto tplo = sym->sb->parentTemplate->templateParams->begin();
                    ++tplo;
                    auto tploe = sym->sb->parentTemplate->templateParams->end();
                    bool found = false;
                    for (; tpln != tplne && tplo != tploe; ++tpln, ++tplo)
                    {
                        if (tplo->second->packed)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found && tpln != tplne)
                        errorsym(ERR_TOO_MANY_ARGS_PASSED_TO_TEMPLATE, sym);
                }
                sym->sb->throughClass = funcparams->sp->sb->throughClass;
                funcparams->sp = sym;
                if (ismember(sym))
                {
                    if (funcparams->noobject)
                    {
                        if (!funcsp->sb->parentClass || classRefCount(sym->sb->parentClass, funcsp->sb->parentClass) == 0)
                        {
                            errorsym(ERR_USE_DOT_OR_POINTSTO_TO_CALL, sym);
                        }
                        else if (classRefCount(sym->sb->parentClass, funcsp->sb->parentClass) != 1)
                        {
                            errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, sym->sb->parentClass, funcsp->sb->parentClass);
                        }
                        else if (funcsp->sb->storage_class == StorageClass::member_ ||
                                 funcsp->sb->storage_class == StorageClass::virtual_)
                        {
                            funcparams->thisptr =
                                MakeExpression(ExpressionNode::auto_, (SYMBOL*)funcsp->tp->BaseType()->syms->front());
                            Dereference(&stdpointer, &funcparams->thisptr);
                            funcparams->thisptr =
                                DerivedToBase(sym->sb->parentClass->tp, funcparams->thisptr->left->v.sp->tp->BaseType()->btp,
                                              funcparams->thisptr, _F_VALIDPOINTER);
                            funcparams->thistp = Type::MakeType(BasicType::pointer_, sym->sb->parentClass->tp);
                            if (sym->tp->IsConst())
                            {
                                funcparams->thistp->btp = Type::MakeType(BasicType::const_, funcparams->thistp->btp);
                            }
                            if (sym->tp->IsVolatile())
                            {
                                funcparams->thistp->btp = Type::MakeType(BasicType::volatile_, funcparams->thistp->btp);
                            }
                            funcparams->thistp->btp->UpdateRootTypes();
                            cppCast(((SYMBOL*)funcsp->tp->BaseType()->syms->front())->tp, &funcparams->thistp,
                                    &funcparams->thisptr);
                        }
                    }
                }
            }
        }
        else
        {
            ResolveArgumentFunctions(funcparams, true);
            if (funcparams->sp->tp->IsFunction())
                sym = funcparams->sp;
        }
        if (sym)
        {
            *tp = sym->tp;
            if (!isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, sym, funcsp, funcparams->thisptr, false))
            {
                errorsym(ERR_CANNOT_ACCESS, sym);
            }
            if (ismember(funcparams->sp))
            {
                if (funcparams->thistp && funcparams->thistp->BaseType()->btp->IsConst())
                    if (!(*tp)->IsConst())
                        errorsym(ERR_NON_CONST_FUNCTION_CALLED_FOR_CONST_OBJECT, funcparams->sp);
            }
        }
    }
    if ((*tp)->BaseType()->type == BasicType::memberptr_)
    {
        memberPtr = true;
        *tp = (*tp)->BaseType()->btp;
    }
    if (!(*tp)->IsFunction())
    {
        // might be operator ()
        if (Optimizer::cparams.prm_cplusplus && (*tp)->IsStructured())
        {
            if (insertOperatorParams(funcsp, &tp_cpp, &exp_cpp, funcparams, flags))
            {
                hasThisPtr = funcparams->thisptr != nullptr;
                *tp = tp_cpp;
                *exp = exp_cpp;
            }
            else
            {
                if (!templateDefinitionLevel && !(flags & _F_INDECLTYPE))
                    errortype(ERR_UNABLE_TO_FIND_SUITABLE_OPERATOR_CALL, *tp, nullptr);
            }
        }
        else
        {
            bool doit = true;

            // if we are in an argument list and there is an empty packed argument
            // don't generate an error on the theory there will be an ellipsis...
            if ((flags & (_F_INARGS | _F_INCONSTRUCTOR)) && funcparams->arguments)
            {
                for (auto arg : *funcparams->arguments)
                {
                    if (arg->tp && arg->tp->type == BasicType::templateparam_ && arg->tp->templateParam->second->packed)
                        doit = !!arg->tp->templateParam->second->byPack.pack;
                }
            }
            if (doit && !templateDefinitionLevel && !(flags & _F_INDECLTYPE) && (!MATCHKW(Keyword::ellipse_) || unpackingTemplate))
                error(ERR_CALL_OF_NONFUNCTION);
            if ((!IsDefiningTemplate()) && (flags & _F_INDECLTYPE))
                *tp = &stdany;
        }
    }
    {
        SymbolTable<SYMBOL>* temp = (*tp)->BaseType()->syms;
        if (temp)
        {
            auto it = temp->begin();

            if (funcparams->sp && !ismember(funcparams->sp) && !memberPtr)
            {
                funcparams->thisptr = nullptr;
            }
            else
            {
                if (!enclosingDeclarations.GetFirst() && !tp_cpp->IsPtr() && (tp_cpp->BaseType()->type != BasicType::memberptr_) && !hasThisPtr)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, funcparams->sp);
            }
            std::list<Argument*>::iterator itl, itle = itl;
            if (funcparams->arguments)
            {
                itl = funcparams->arguments->begin();
                itle = funcparams->arguments->end();
            }
            if (funcparams->sp)
            {
                if (funcparams->sp->sb->parentClass == nullptr && funcparams->sp->sb->parentNameSpace == nullptr)
                {
                    if (!strcmp(funcparams->sp->name, "setjmp"))
                        Optimizer::setjmp_used = true;
                }
                if (funcparams->sp->sb->attribs.inheritable.linkage3 == Linkage::noreturn_)
                    isCallNoreturnFunction = true;
                Type* tp1 = funcparams->sp->tp;
                if (tp1->IsPtr())
                    tp1 = tp1->BaseType()->btp;
                if (tp1->IsFunction())
                {
                    auto itq = tp1->BaseType()->syms->begin();
                    if (itq != tp1->BaseType()->syms->end())
                    {
                        if ((*itq)->sb->thisPtr)
                            ++itq;
                        if (itq != tp1->BaseType()->syms->end())
                        {
                            Type* tp = (*itq)->tp;
                            if (tp->IsRef())
                            {
                                initializerRef = true;
                                tp = tp->BaseType()->btp;
                            }
                            if (tp->IsStructured())
                            {
                                SYMBOL* sym = (tp->BaseType()->sp);
                                if (sym->sb->initializer_list && sym->sb->templateLevel)
                                {
                                    auto itr = sym->templateParams->begin();
                                    ++itr;
                                    initializerListTemplate = sym->tp;
                                    initializerListType = itr->second->byClass.val;
                                }
                            }
                        }
                    }
                }
            }
            if (!(flags & _F_SIZEOF))
            {
                std::list<Argument*> temp1;
                std::list<Argument*>* temp2 = &temp1;
                if (initializerListType)
                {
                    if (funcparams->sp->sb->constexpression && (argumentNestingLevel == 0 && !inStaticAssert))
                    {
                        EXPRESSION* node = MakeExpression(funcparams);
                        if (EvaluateConstexprFunction(node))
                        {
                            *tp = funcparams->sp->tp->BaseType()->btp;
                            *exp = node;
                            return;
                        }
                    }
                    bool isnested = false;
                    if (funcparams->arguments->front()->nested && funcparams->arguments->front()->nested->front()->nested &&
                        !funcparams->arguments->front()->initializer_list)
                    {
                        funcparams->arguments = funcparams->arguments->front()->nested;
                        itl = funcparams->arguments->begin();
                        itle = funcparams->arguments->end();
                        temp1.insert(temp1.begin(), itl, itle);
                        isnested = true;
                    }
                    else if ((*itl)->nested)
                    {
                        temp1.push_back(*itl);
                    }
                    else
                    {
                        temp1.insert(temp1.begin(), itl, itle);
                    }

                    CreateInitializerList(funcparams->sp, initializerListTemplate, initializerListType, &temp2, false,
                                          initializerRef);
                    if (isnested)
                    {
                        funcparams->arguments = temp2;
                        itl = funcparams->arguments->begin();
                        itle = funcparams->arguments->end();
                    }
                    else
                    {
                        **itl = *temp2->front();
                    }
                    if (it != temp->end())
                    {
                        auto itx = itl;
                        ++itx;
                        temp1.clear();
                        temp1.insert(temp1.begin(), itx, itle);
                        temp2 = &temp1;
                        auto itxn = it;
                        ++itxn;
                        AdjustParams(funcparams->sp, itxn, temp->end(), &temp2, false, true);
                        itx = itl;
                        ++itx;
                        for (auto t : temp1)
                        {
                            *itx = t;
                        }
                    }
                }
                else
                {
                    temp1.insert(temp1.begin(), itl, itle);
                    int n = temp1.size();
                    AdjustParams(funcparams->sp, it, temp->end(), &temp2, false, true);
                    auto itt = temp1.begin();
                    auto itte = temp1.end();
                    for (auto i = 0; i < n; i++)
                        ++itt;
                    if (!funcparams->arguments)
                        funcparams->arguments = argumentListFactory.CreateList();
                    for (; itt != itte; ++itt)
                    {
                        funcparams->arguments->push_back(*itt);
                    }
                }
            }
            if (funcparams->sp->sb->xcMode != xc_unspecified && funcparams->sp->sb->xcMode != xc_none)
                hasFuncCall = true;
            CheckCalledException(funcparams->sp, funcparams->thisptr);
            if (Optimizer::cparams.prm_cplusplus && funcparams->arguments)
            {
                for (auto il : *funcparams->arguments)
                {
                    if (il->tp && il->tp->IsStructured() && il->exp)
                    {
                        EXPRESSION* exp = il->exp;
                        if (exp->type == ExpressionNode::not__lvalue_)
                            exp = exp->left;
                        if (exp->type == ExpressionNode::auto_)
                        {
                            if (!funcparams->callLab)
                                funcparams->callLab = -1;
                        }
                        else if (exp->type == ExpressionNode::thisref_)
                        {
                            if (!funcparams->callLab)
                                funcparams->callLab = -1;
                        }
                    }
                }
            }
            if ((*tp)->IsFunction())
            {
                (*exp)->v.func->resolvedCall = true;
                if (funcparams->thisptr && !memberPtr)
                {
                    SYMBOL* base = funcparams->sp->sb->parentClass;
                    SYMBOL* derived = funcparams->thistp->BaseType()->btp->BaseType()->sp;
                    if (base != derived)
                    {
                        funcparams->thisptr = DerivedToBase(base->tp, derived->tp, funcparams->thisptr, _F_VALIDPOINTER);
                    }
                }
                if ((*tp)->BaseType()->btp->IsStructured() || (*tp)->BaseType()->btp->BaseType()->type == BasicType::memberptr_)
                {
                    if (!(flags & _F_SIZEOF) && !(*tp)->BaseType()->btp->BaseType()->sp->sb->structuredAliasType)
                    {
                        (*tp)->BaseType()->btp->InitializeDeferred();
                        funcparams->returnEXP = AnonymousVar(StorageClass::auto_, (*tp)->BaseType()->btp->BaseType()->sp->tp);
                        funcparams->returnSP = funcparams->returnEXP->v.sp;
                        if (theCurrentFunc && theCurrentFunc->sb->constexpression)
                            funcparams->returnEXP->v.sp->sb->constexpression = true;
                    }
                }
                else if ((*tp)->BaseType()->btp->BaseType()->IsBitInt())
                {
                    if (!(flags & _F_SIZEOF))
                    {
                        funcparams->returnEXP = AnonymousVar(StorageClass::auto_, (*tp)->BaseType()->btp);
                        funcparams->returnSP = funcparams->returnEXP->v.sp;
                        if (theCurrentFunc && theCurrentFunc->sb->constexpression)
                            funcparams->returnEXP->v.sp->sb->constexpression = true;
                    }
                }
                funcparams->ascall = true;
                funcparams->functp = *tp;

                auto tp2 = *tp;
                while (tp2->btp) tp2 = tp2->btp;
                *tp = ResolveTemplateSelectors((*tp)->BaseType()->sp, (*tp)->BaseType()->btp);
                if (tp2->type == BasicType::templateparam_ && tp2->templateParam->second->byClass.val)
                {
                    *tp= tp2->templateParam->second->byClass.val;
                }
                if ((*tp)->IsRef())
                {
                    auto reftype = (*tp)->type;
                    *tp = (*tp)->btp->CopyType();
                    (*tp)->UpdateRootTypes();
                    if (reftype == BasicType::rref_)
                    {
                        (*tp)->rref = true;
                        (*tp)->lref = false;
                    }
                    else
                    {
                        (*tp)->lref = true;
                        (*tp)->rref = false;
                    }
                }
                else if ((*tp)->IsPtr() && (*tp)->array)
                {
                    *tp = (*tp)->CopyType();
                    (*tp)->UpdateRootTypes();
                    (*tp)->lref = true;
                    (*tp)->rref = false;
                }
                auto tp1 = tp;
                while ((*tp1)->IsPtr() || (*tp1)->BaseType()->type == BasicType::memberptr_)
                    tp1 = &(*tp1)->BaseType()->btp;
                while ((*tp1)->btp)
                    tp1 = &(*tp1)->btp;
                if ((*tp1)->IsStructured())
                {
                    *tp1 = (*tp1)->sp->tp;
                }
                if (!(flags & _F_SIZEOF))
                {
                    checkArgs(funcparams, funcsp);
                }
                if (funcparams->returnSP && funcparams->returnSP->tp->IsStructured())
                {
                    SYMBOL* sym = funcparams->returnSP->tp->BaseType()->sp;
                    if (sym->sb->templateLevel && sym->templateParams && !sym->sb->instantiated)
                    {
                        if (!allTemplateArgsSpecified(sym, sym->templateParams))
                            sym = GetClassTemplate(sym, sym->templateParams, false);
                        if (sym && allTemplateArgsSpecified(sym, sym->templateParams))
                            funcparams->returnSP->tp =
                                TemplateClassInstantiate(sym, sym->templateParams, false, StorageClass::global_)->tp;
                    }
                }
                bool dovirtual = false;
                if (funcparams->sp && funcparams->sp->sb->storage_class == StorageClass::virtual_)
                {
                    dovirtual = !funcparams->novtab || (funcparams->sp && funcparams->sp->sb->ispure);
                    if (!dovirtual && funcparams->thisptr->type == ExpressionNode::l_p_)
                    {
                        int offset = 0;
                        auto node = relptr(funcparams->thisptr->left, offset);
                        dovirtual = node && node->type == ExpressionNode::auto_ && node->v.sp->sb->isxctab;
                    }
                }
                if (dovirtual)
                {
                    exp_in = funcparams->thisptr;
                    Dereference(&stdpointer, &exp_in);
                    exp_in = MakeExpression(ExpressionNode::add_, exp_in,
                                            MakeIntExpression(ExpressionNode::c_i_, funcparams->sp->sb->vtaboffset));
                    Dereference(&stdpointer, &exp_in);
                    funcparams->fcall = exp_in;
                }
                else
                {
                    exp_in = MakeExpression(funcparams);
                    if (exp_in && Optimizer::cparams.prm_cplusplus && funcparams->returnEXP)
                    {
                        if (!funcparams->returnSP->tp->BaseType()->sp->sb->trivialCons)
                        {
                            exp_in = MakeExpression(ExpressionNode::thisref_, exp_in);
                            exp_in->v.t.thisptr = funcparams->returnEXP;
                            exp_in->v.t.tp = funcparams->returnSP->tp;

                            if (funcparams->returnSP->tp->IsStructured())
                            {
                                EXPRESSION* expx = funcparams->returnEXP;
                                CallDestructor(funcparams->returnSP->tp->BaseType()->sp, nullptr, &expx, nullptr, true, false, true,
                                               true);
                                InsertInitializer(&funcparams->returnSP->sb->dest, funcparams->returnSP->tp, expx, 0, true);
                            }
                        }
                    }
                    if (exp_in)
                    {
                        Type* tp = funcparams->sp->tp->BaseType()->btp;
                        if ((flags & _F_AMPERSAND) && tp->IsArithmetic())
                        {
                            EXPRESSION* rv = AnonymousVar(StorageClass::auto_, tp);
                            Dereference(tp, &rv);
                            exp_in =
                                MakeExpression(ExpressionNode::comma_, MakeExpression(ExpressionNode::assign_, rv, exp_in), rv);
                            errortype(ERR_CREATE_TEMPORARY, tp, tp);
                        }
                    }
                    if (exp_in)
                        *exp = exp_in;
                }
                if (funcparams->sp && funcparams->sp->tp->IsFunction() && funcparams->sp->tp->BaseType()->btp->IsRef())
                {
                    Type** tp1;
                    Dereference(funcparams->sp->tp->BaseType()->btp->BaseType()->btp, exp);
                    tp1 = &funcparams->sp->tp->BaseType()->btp;
                    *tp = (*tp1)->BaseType()->btp->CopyType();
                    (*tp)->UpdateRootTypes();
                    if ((*tp1)->BaseType()->type == BasicType::rref_)
                    {
                        (*tp)->rref = true;
                        (*tp)->lref = false;
                    }
                    else
                    {
                        (*tp)->lref = true;
                        (*tp)->rref = false;
                    }
                    while ((*tp)->IsRef())
                        *tp = (*tp)->BaseType()->btp;
                }
                GetAssignDestructors(&funcparams->destructors, *exp);
            }
            else if (IsDefiningTemplate() && (*tp)->type == BasicType::aggregate_)
            {
                *exp = MakeExpression(ExpressionNode::funcret_, *exp);
                *tp = Type::MakeType(BasicType::templatedecltype_);
                (*tp)->templateDeclType = *exp;
            }
            else
            {
                // we may get here for a packed argument with an ellipsis
                // if that is the case this won't matter and if it isn't the error will ripple...
                *tp = &stdany;
            }
        }
        else if ((*tp)->type != BasicType::any_)
        {
            *tp = &stdint;
        }
    }
    if (*exp)
    {
        auto exp1 = *exp;
        if (exp1->type == ExpressionNode::thisref_)
            exp1 = exp1->left;
        if (exp1->type == ExpressionNode::callsite_ && exp1->v.func->thisptr && !exp1->v.sp->tp->IsConst())
            CheckThroughConstObject(nullptr, exp1);
    }
    return;
}
static void expression_alloca( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    getsym();
    if (needkw(Keyword::openpa_))
    {
        expression_comma(funcsp, nullptr, tp, exp, nullptr, flags);
        if (*tp)
        {
            ResolveTemplateVariable(tp, exp, &stdint, nullptr);
            if (!(*tp)->IsInt())
                error(ERR_NEED_INTEGER_EXPRESSION);
            optimize_for_constants(exp);

            funcsp->sb->allocaUsed = true;
            *exp = MakeExpression(ExpressionNode::alloca_, *exp);
            needkw(Keyword::closepa_);
            *tp = &stdpointer;
        }
        else
        {
            error(ERR_EXPRESSION_SYNTAX);
            *tp = nullptr;
        }
    }
    else
    {
        errskim(skim_closepa);
        skip(Keyword::closepa_);
        *tp = nullptr;
    }
    return;
}
static void expression_offsetof( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
    getsym();
    if (needkw(Keyword::openpa_))
    {
        // this is naive, not checking the actual type of the sym when these are present
        if (MATCHKW(Keyword::struct_) || MATCHKW(Keyword::class_))
            getsym();
        if (ISID())
        {
            char name[512];
            SYMBOL* sym;
            SymbolTable<SYMBOL>* table;
            SYMBOL* strSym;
            std::list<NAMESPACEVALUEDATA*>* nsv;
            Utils::StrCpy(name, currentLex->value.s.a);
            tagsearch(name, sizeof(name), &sym, &table, &strSym, &nsv, StorageClass::global_);
            if (!sym)  // might be a typedef
            {

                sym = namespacesearch(name, globalNameSpace, false, false);
                if (sym && sym->tp->type == BasicType::typedef_)
                {
                    sym = sym->tp->btp->BaseType()->sp;
                }
                else
                {
                    sym = nullptr;
                }
            }
            if (sym && sym->tp->type == BasicType::templateparam_ && !sym->tp->templateParam->second->packed && sym->tp->templateParam->second->type == TplType::typename_)
            {
                auto tp1 = sym->tp->templateParam->second->byClass.val;
                if (tp1->IsStructured())
                    sym = tp1->BaseType()->sp;
            }
            if (sym && sym->tp->IsStructured())
            {
                if (MATCHKW(Keyword::lt_))
                {
                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                    GetTemplateArguments(funcsp, sym, &lst);
                    if (sym->sb->storage_class == StorageClass::typedef_)
                    {
                        auto sp1 = GetTypeAliasSpecialization(sym, lst);
                        if (sp1)
                            sym = sp1;
                        if (sym->tp->IsStructured())
                            sym = sym->tp->BaseType()->sp;
                    }
                    else
                    {
                        auto sp1 = GetClassTemplate(sym, lst, false);
                        if (sp1)
                            sym = sp1;
                    }
                    sym->tp->InitializeDeferred();
                }
                if (needkw(Keyword::comma_))
                {
                    *tp = sym->tp;
                    do
                    {
                        expression_member(funcsp, tp, exp, ismutable, flags | _F_AMPERSAND);
                        TakeAddress(exp);
                    } while (MATCHKW(Keyword::dot_));
                }
            }
            else
            {
                error(ERR_CLASS_TYPE_EXPECTED);
                errskim(skim_closepa);
            }
        }
        else
        {
            error(ERR_IDENTIFIER_EXPECTED);
            errskim(skim_closepa);
        }
        needkw(Keyword::closepa_);
    }
    *tp = &stdunsigned;

    return;
}
static void expression_msilfunc( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    Keyword kw = currentLex->kw->key;
    getsym();
    if (MATCHKW(Keyword::openpa_))
    {
        CallSite funcparams;
        memset(&funcparams, 0, sizeof(funcparams));
        getArgs(funcsp, &funcparams, Keyword::closepa_, true, flags);
        int n = funcparams.arguments ? funcparams.arguments->size() : 0;
        if (n > 3)
        {
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            errorstr(ERR_PARAMETER_LIST_TOO_LONG, "__cpblk/__initblk");
        }
        else if (n < 3)
        {
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            errorstr(ERR_PARAMETER_LIST_TOO_SHORT, "__cpblk/__initblk");
        }
        else
        {
            auto it = funcparams.arguments->begin();
            auto arg1 = *it;
            ++it;
            auto arg2 = *it;
            ++it;
            auto arg3 = *it;
            switch (kw)
            {
                case Keyword::cpblk_:
                    if (!arg1->tp->IsPtr())
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__dest", "__cpblk");
                    }
                    else if (!arg2->tp->IsPtr())
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__src", "__cpblk");
                    }
                    else if (!arg3->tp->IsInt())
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__size", "__cpblk");
                    }
                    else
                    {
                        *exp = MakeExpression(ExpressionNode::comma_, arg1->exp, arg2->exp);
                        *exp = MakeExpression(ExpressionNode::cpblk_, *exp, arg3->exp);
                    }
                    break;
                case Keyword::initblk_:
                    if (!arg1->tp->IsPtr())
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__dest", "__initblk");
                    }
                    else if (!arg2->tp->IsInt())
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__value", "__initblk");
                    }
                    else if (!arg3->tp->IsInt())
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__size", "__initblk");
                    }
                    else
                    {
                        *exp = MakeExpression(ExpressionNode::comma_, arg1->exp, arg2->exp);
                        *exp = MakeExpression(ExpressionNode::initblk_, *exp, arg3->exp);
                    }
                    break;
                default:
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    break;
            }
        }
        *tp = &stdpointer;
    }
    else
    {
        errskim(skim_closepa);
        skip(Keyword::closepa_);
        *tp = nullptr;
    }
    return;
}
static void expression_string( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    int elems = 0;
    StringData* data;
    (void)funcsp;

    concatStringsInternal(&data, &elems);
    *exp = stringlit(data);
    if (data->suffix)
    {
        BasicType tpb;
        SYMBOL* sym;
        char name[512];
        switch (data->strtype)
        {
            case LexType::l_u8str_:
                if (Optimizer::cparams.c_dialect >= Dialect::c23)
                    tpb = stdchar8_tptr.btp->type;
                else
                    tpb = stdcharptr.btp->type;
                break;
            default:
            case LexType::l_astr_:
                tpb = stdcharptr.btp->type;
                break;
            case LexType::l_wstr_:
                tpb = stdwcharptr.btp->type;
                break;
            case LexType::l_msilstr_:
                tpb = std__string.type;
                break;
            case LexType::l_ustr_:
                tpb = stdchar16tptr.btp->type;
                break;
            case LexType::l_Ustr_:
                tpb = stdchar32tptr.btp->type;
                break;
        }
        Optimizer::my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], data->suffix);
        sym = LookupSym(name);
        if (sym)
        {
            SYMBOL *sym1 = nullptr, *first_param = nullptr;
            bool found = false;
            for (auto candidate_function : *sym->tp->syms)
            {
                first_param = candidate_function->tp->syms->front();
                if (candidate_function->tp->syms->size() > 1 && first_param->tp->IsPtr())
                    if (first_param->tp->btp->IsConst() && first_param->tp->btp->BaseType()->type == tpb)
                    {
                        sym1 = candidate_function;
                        found = true;
                        break;
                    }
            }
            if (found)
            {
                CallSite* f = Allocate<CallSite>();
                f->sp = sym1;
                f->functp = sym1->tp;
                f->fcall = MakeExpression(ExpressionNode::pc_, sym1);
                f->arguments = argumentListFactory.CreateList();
                auto arg = Allocate<Argument>();
                arg->tp = first_param->tp;
                arg->exp = *exp;
                f->arguments->push_back(arg);
                arg = Allocate<Argument>();
                arg->tp = &stdunsigned;
                arg->exp = MakeIntExpression(ExpressionNode::c_i_, elems);
                f->arguments->push_back(arg);
                *exp = MakeExpression(f);
                *tp = sym1->tp;
                expression_arguments(funcsp, tp, exp, false, true);
                return;
            }
        }
        errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, data->suffix);
    }
    if (data->strtype == LexType::l_msilstr_)
    {
        *tp = std__string.CopyType();
        (*tp)->rootType = *tp;
    }
    else
    {
        *tp = Type::MakeType(BasicType::pointer_);
        (*tp)->array = true;
        (*tp)->stringconst = true;
        (*tp)->esize = MakeIntExpression(ExpressionNode::c_i_, elems + 1);
        switch (data->strtype)
        {
            case LexType::l_u8str_:
                if (Optimizer::cparams.c_dialect >= Dialect::c23)
                    (*tp)->btp = stdchar8_tptr.btp;
                else
                    (*tp)->btp = stdcharptr.btp;
                break;
            default:
            case LexType::l_astr_:
                (*tp)->btp = stdcharptr.btp;
                break;
            case LexType::l_wstr_:
                (*tp)->btp = stdwcharptr.btp;
                break;
            case LexType::l_ustr_:
                (*tp)->btp = stdchar16tptr.btp;
                break;
            case LexType::l_Ustr_:
                (*tp)->btp = stdchar32tptr.btp;
                break;
        }
    }
    if ((*tp)->type == BasicType::string_)
        (*tp)->size = 1;
    else
        (*tp)->size = (elems + 1) * (*tp)->btp->size;
    return;
}
static bool matchesAttributes(Type* tp1, Type* tp2)
{
    while (tp1 && tp2)
    {
        if (tp1->IsConst() != tp2->IsConst() || tp1->IsVolatile() != tp2->IsVolatile() || tp1->IsRestrict() != tp2->IsRestrict())
            return false;
        tp1 = tp1->BaseType()->btp;
        tp2 = tp2->BaseType()->btp;
    }
    return true;
}
static bool sameTypedef(Type* tp1, Type* tp2)
{
    while (tp1->btp && !tp1->typedefType)
        tp1 = tp1->btp;
    while (tp2->btp && !tp2->typedefType)
        tp2 = tp2->btp;
    return tp1->typedefType == tp2->typedefType;
}
static void expression_generic( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    RequiresDialect::Keyword(Dialect::c11, "_Generic");
    getsym();
    if (!needkw(Keyword::openpa_))
    {
        errskim(skim_closepa);
        skip(Keyword::closepa_);
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        *tp = &stdint;
    }
    else
    {
        Type* selectType = nullptr;
        EXPRESSION* throwawayExpression = nullptr;
        expression_assign(funcsp, nullptr, &selectType, &throwawayExpression, nullptr, flags);
        if (MATCHKW(Keyword::comma_))
        {
            bool dflt = false;
            struct genericHold
            {
                struct genericHold* next;
                Type* selector;
                Type* type;
                EXPRESSION* exp;
            };
            struct genericHold *list = nullptr, **pos = &list;
            struct genericHold* selectedGeneric = nullptr;
            while (MATCHKW(Keyword::comma_))
            {
                struct genericHold *next = Allocate<genericHold>(), *scan;
                getsym();
                if (MATCHKW(Keyword::default_))
                {
                    getsym();
                    if (dflt)
                    {
                        error(ERR_GENERIC_TOO_MANY_DEFAULTS);
                    }
                    dflt = true;
                    next->selector = nullptr;
                }
                else
                {
                    next->selector = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
                    if (!next->selector)
                    {
                        error(ERR_GENERIC_MISSING_TYPE);
                        break;
                    }
                }
                if (MATCHKW(Keyword::colon_))
                {
                    getsym();
                    expression_assign(funcsp, nullptr, &next->type, &next->exp, nullptr, flags);
                    if (!next->type)
                    {
                        error(ERR_GENERIC_MISSING_EXPRESSION);
                        break;
                    }
                    scan = list;
                    while (scan)
                    {
                        if (scan->selector && next->selector && next->selector->CompatibleType(scan->selector))
                        {
                            if (matchesAttributes(next->selector, scan->selector) && sameTypedef(next->selector, scan->selector))
                            {
                                error(ERR_DUPLICATE_TYPE_IN_GENERIC);
                            }
                        }
                        scan = scan->next;
                    }
                    if (!selectedGeneric && !next->selector)
                        selectedGeneric = next;
                    else
                    {
                        if (selectType && next->selector && next->selector->CompatibleType(selectType))
                        {
                            if (matchesAttributes(next->selector, selectType) && sameTypedef(next->selector, selectType))
                            {
                                if (selectedGeneric && selectedGeneric->selector)
                                    error(ERR_DUPLICATE_TYPE_IN_GENERIC);
                                selectedGeneric = next;
                            }
                        }
                    }
                    *pos = next;
                    pos = &(*pos)->next;
                }
                else
                {
                    error(ERR_GENERIC_MISSING_EXPRESSION);
                    break;
                }
            }
            if (selectedGeneric)
            {
                *tp = selectedGeneric->type;
                *exp = selectedGeneric->exp;
                optimize_for_constants(exp);
            }
            else
            {
                error(ERR_GENERIC_NOTHING_SELECTED);
                *tp = &stdint;
                *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            }
            if (!needkw(Keyword::closepa_))
            {
                errskim(skim_closepa);
                skip(Keyword::closepa_);
            }
        }
        else
        {
            errskim(skim_closepa);
            skip(Keyword::closepa_);
        }
    }
    return;
}
static bool getSuffixedChar( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    char name[512];
    BasicType tpb = (*tp)->type;
    SYMBOL* sym;
    Optimizer::my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], currentLex->suffix);
    sym = LookupSym(name);
    if (sym)
    {
        bool found = false;
        SYMBOL *sym1 = nullptr, *sym2;
        for (auto sp : *sym->tp->syms)
        {
            sym1 = sp;
            sym2 = sym1->tp->syms->front();
            if (sym1->tp->syms->size() == 1 && sym2->tp->type == tpb)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            CallSite* f = Allocate<CallSite>();
            f->sp = sym1;
            f->functp = sym1->tp;
            f->fcall = MakeExpression(ExpressionNode::pc_, sym1);
            f->arguments = argumentListFactory.CreateList();
            auto arg = Allocate<Argument>();
            f->arguments->push_back(arg);
            arg->tp = *tp;
            arg->exp = *exp;
            *exp = MakeExpression(f);
            *tp = sym1->tp;
            expression_arguments(funcsp, tp, exp, false);
            return true;
        }
    }
    errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, currentLex->suffix);
    if (sym)
    {
        SYMBOL *sym1 = nullptr, *sym2;
        for (auto sp : *sym->tp->syms)
        {
            sym1 = sp;
            sym2 = sym1->tp->syms->front();
            // We want to narrow the candidates down to "reasonable" candidates, AKA candidates with only one parameter
            // Other candidates should be ignored as being incorrect for the operator.
            if (sym1->tp->syms->size() == 1)
            {
                if (sym2->tp->type != tpb)
                {
                    char holder[1000];
                    char holder1[1000];
                    Type* typ = Type::MakeType(tpb, nullptr);
                    sym2->tp->ToString(holder + sizeof(holder), holder);
                    typ->ToString(holder1 + sizeof(holder1), holder1);
                    printerr(ERR_CANDIDATE_INCORRECT_TYPE, sym1->sb->declfile, sym1->sb->declline, holder, holder1);
                }
            }
        }
    }
    return false;
}
static bool getSuffixedNumber( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    char name[512];
    BasicType tpb;
    SYMBOL* sym;
    if (currentLex->type == LexType::ull_)
        tpb = BasicType::unsigned_long_long_;
    else
        tpb = BasicType::long_double_;
    Optimizer::my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], currentLex->suffix);
    sym = LookupSym(name);
    if (sym)
    {
        // look for parameter of type unsigned long long or long double
        SYMBOL *sym1 = nullptr;
        auto found = false;
        for (auto sp : *sym->tp->syms)
        {
            sym1 = sp;
            auto sym2 = sym1->tp->syms->front();
            if (sym1->tp->syms->size() == 1 && sym2->tp->type == tpb)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            CallSite* f = Allocate<CallSite>();
            f->sp = sym1;
            f->functp = sym1->tp;
            f->fcall = MakeExpression(ExpressionNode::pc_, sym1);
            f->arguments = argumentListFactory.CreateList();
            auto arg = Allocate<Argument>();
            f->arguments->push_back(arg);
            if (currentLex->type == LexType::ull_)
            {
                arg->tp = &stdunsignedlonglong;
                arg->exp = MakeIntExpression(ExpressionNode::c_ull_, currentLex->value.i);
            }
            else
            {
                arg->tp = &stdlongdouble;
                arg->exp = MakeIntExpression(ExpressionNode::c_ld_, 0);
                arg->exp->v.f = Allocate<FPF>();
                *arg->exp->v.f = *currentLex->value.f;
            }
            *exp = MakeExpression(f);
            *tp = sym1->tp;
            expression_arguments(funcsp, tp, exp, false, true);
            return true;
        }
        else
        {
            bool found = false;
            // not found, look for parameter of type const char *
            for (auto sp : *sym->tp->syms)
            {
                Type* tpx;
                sym1 = sp;
                auto sym2 = sym1->tp->syms->front();
                tpx = sym2->tp;
                if (sym1->tp->syms->size() == 1 && tpx->IsPtr())
                {
                    tpx = tpx->BaseType()->btp;
                    if (tpx->IsConst() && tpx->BaseType()->type == BasicType::char_)
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (found)
            {
                CallSite* f = Allocate<CallSite>();
                StringData* data;
                int i;
                f->sp = sym1;
                f->functp = sym1->tp;
                f->fcall = MakeExpression(ExpressionNode::pc_, sym1);
                f->arguments = argumentListFactory.CreateList();
                auto arg = Allocate<Argument>();
                f->arguments->push_back(arg);
                arg->tp = &stdcharptr;
                arg->tp->size = (strlen(currentLex->litaslit) + 1) * arg->tp->btp->size;
                data = Allocate<StringData>();
                data->strtype = LexType::l_astr_;
                data->size = 1;
                data->pointers = Allocate<Optimizer::SLCHAR*>();
                data->pointers[0] = Allocate<Optimizer::SLCHAR>();
                data->pointers[0]->count = strlen(currentLex->litaslit);
                data->pointers[0]->str = Allocate<LCHAR>(data->pointers[0]->count + 1);
                for (i = 0; i < data->pointers[0]->count; i++)
                    data->pointers[0]->str[i] = currentLex->litaslit[i];
                arg->exp = stringlit(data);
                *exp = MakeExpression(f);
                *tp = sym1->tp;
                expression_arguments(funcsp, tp, exp, false);
                return true;
            }
        }
    }
    errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, currentLex->suffix);
    SYMBOL* sym1 = nullptr;
    auto found = false;
    if (sym)
    {
        for (auto sp : *sym->tp->syms)
        {
            sym1 = sp;
            auto sym2 = sym1->tp->syms->front();
            if (sym1->tp->syms->size() == 1)
            {
                if (sym2->tp->type != tpb)
                {
                    char holder[1000];
                    char holder1[1000];
                    Type* typ = Type::MakeType(tpb, nullptr);
                    sym2->tp->ToString(holder + 1000 - 1, holder);
                    typ->ToString(holder1 + 1000 - 1, holder1);
                    printerr(ERR_CANDIDATE_INCORRECT_TYPE, sym1->sb->declfile, sym1->sb->declline, holder, holder1);
                }
            }
            else
            {
                std::string holder;
                holder.resize(10000);
                sym1->tp->ToString(holder.data() + holder.capacity() - 1, holder.data());
                printerr(ERR_NOTE_GENERIC, sym1->sb->declfile, sym1->sb->declline, "Candidate was checked, but had more than one potential parameter");
            }
        }
        // not found, look for parameter of type const char *
        for (auto sp : *sym->tp->syms)
        {
            Type* tpx;
            sym1 = sp;
            auto sym2 = sym1->tp->syms->front();
            tpx = sym2->tp;
            if (sym1->tp->syms->size() == 1 && tpx->IsPtr())
            {
                tpx = tpx->BaseType()->btp;
                if (!tpx->IsConst())
                {
                    if (tpx->BaseType()->type != BasicType::char_)
                    {
                        char holder[1000];
                        tpx->ToString(holder + sizeof(holder), holder);
                        printerr(ERR_CANDIDATE_INCORRECT_TYPE, sym2->sb->declfile, sym2->sb->declline, "const char*", holder);
                    }
                }
            }
        }
    }
    else
    {
        printerr(ERR_NOTE_GENERIC, currentLex->sourceFileName, currentLex->sourceLineNumber, "No candidates were found");
    }
    return false;
}
static void  atomic_modify_specific_op(Parser::SYMBOL* funcsp, Parser::Type** tp,
                                                  Parser::Type** typf, Parser::ATOMICDATA* d, int flags, Parser::Keyword function,
                                                  bool fetch_first)
{
    Parser::Type* tpf = *typf;
    expression_assign(funcsp, nullptr, &tpf, &d->address, nullptr, flags);
    if (tpf)
    {
        if (!tpf->IsPtr())
        {
            error(ERR_DEREF);
            d->tp = *tp = &stdint;
        }
        else
        {
            d->tp = *tp = tpf->BaseType()->btp;
        }
    }
    d->third = MakeIntExpression(ExpressionNode::c_i_, (int)function);
    if (needkw(Keyword::comma_))
    {
        expression_assign(funcsp, nullptr, &tpf, &d->value, nullptr, flags);
        if (!tpf->SameType(*tp))
        {
            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
        }
        if ((*tp)->IsPtr() && (function == Keyword::asplus_ || function == Keyword::asminus_))
        {
            d->value = MakeExpression(ExpressionNode::mul_, d->value, MakeIntExpression(ExpressionNode::c_i_, (*tp)->size));
        }
    }
    else
    {
        *tp = &stdint;
        d->value = MakeIntExpression(ExpressionNode::c_i_, 0);
    }
    if (needkw(Keyword::comma_))
    {
        expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
    }
    else
    {
        tpf = &stdint;
        d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
    }
    d->atomicOp = fetch_first ? Optimizer::e_ao::ao_fetch_modify : Optimizer::e_ao::ao_modify_fetch;
    if (!d->memoryOrder2)
        d->memoryOrder2 = d->memoryOrder1;
    return;
}
static void expression_atomic_func( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    RequiresDialect::Feature(Dialect::c11, "Atomic Functions");
    Keyword kw = KW();
    getsym();
    if (needkw(Keyword::openpa_))
    {
        if (kw == Keyword::c11_atomic_is_lock_free_)
        {
            optimized_expression(funcsp, nullptr, tp, exp, false);
            if (!(*tp)->IsInt() || !isintconst(*exp))
                error(ERR_NEED_INTEGER_TYPE);
            *tp = &stdint;
            bool found = (*exp)->v.i > 0 && (*exp)->v.i <= Optimizer::chosenAssembler->arch->isLockFreeSize &&
                         ((*exp)->v.i & ((*exp)->v.i - 1)) == 0;
            *exp = MakeIntExpression(ExpressionNode::c_i_, found);
            needkw(Keyword::closepa_);
        }
        else if (kw == Keyword::c11_atomic_init_)
        {
            expression_assign(funcsp, nullptr, tp, exp, nullptr, flags);
            if (!*tp)
                error(ERR_EXPRESSION_SYNTAX);
            if (MATCHKW(Keyword::comma_))  // atomic_init
            {
                Type* tp1;
                EXPRESSION* exp1;
                getsym();
                expression_assign(funcsp, nullptr, &tp1, &exp1, nullptr, flags);
                if (*tp && tp1)
                {
                    ATOMICDATA* d;
                    d = Allocate<ATOMICDATA>();
                    if (!(*tp)->IsPtr())
                    {
                        error(ERR_DEREF);
                        d->tp = *tp = &stdint;
                    }
                    else
                    {
                        Type* tp2 = *tp;
                        d->tp = *tp = (*tp)->BaseType()->btp;
                        if (!(*tp)->SameType(tp1))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        tp1 = tp2;
                    }
                    d->address = *exp;
                    d->value = exp1;
                    d->atomicOp = Optimizer::ao_init;
                    *exp = MakeExpression(ExpressionNode::atomic_);
                    (*exp)->v.ad = d;
                }
                else
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
            }
            else  // ATOMIC_VAR_INIT
            {
                if (*exp)
                    (*exp)->atomicinit = true;
            }
            needkw(Keyword::closepa_);
        }
        else
        {
            Type *tpf = nullptr, *tpf1;
            ATOMICDATA* d;
            d = Allocate<ATOMICDATA>();
            switch (kw)
            {
                case Keyword::atomic_kill_dependency_:
                    expression_assign(funcsp, nullptr, &d->tp, &d->address, nullptr, flags);
                    *tp = d->tp;
                    if (!*tp)
                        error(ERR_EXPRESSION_SYNTAX);
                    d->atomicOp = Optimizer::ao_kill_dependency;
                    break;
                case Keyword::atomic_flag_test_set_:

                    expression_assign(funcsp, nullptr, &tpf, &d->flg, nullptr, flags);
                    if (tpf)
                    {
                        if (!tpf->IsPtr())
                        {
                            error(ERR_DEREF);
                        }
                        Dereference(&stdint, &d->flg);
                        d->tp = *tp = &stdint;
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_flag_set_test;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    break;
                case Keyword::atomic_flag_clear_:
                    expression_assign(funcsp, nullptr, &tpf, &d->flg, nullptr, flags);
                    if (tpf)
                    {
                        if (!tpf->IsPtr())
                        {
                            error(ERR_DEREF);
                        }
                        Dereference(&stdint, &d->flg);
                        d->tp = *tp = &stdint;
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_flag_clear;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    *tp = &stdvoid;
                    break;
                case Keyword::c11_atomic_thread_fence_:
                    expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    d->memoryOrder1 =
                        MakeExpression(ExpressionNode::add_, d->memoryOrder1, MakeIntExpression(ExpressionNode::c_i_, 0x80));
                    d->atomicOp = Optimizer::ao_thread_fence;
                    *tp = &stdvoid;
                    break;
                case Keyword::c11_atomic_signal_fence_:
                    expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    d->memoryOrder1 =
                        MakeExpression(ExpressionNode::add_, d->memoryOrder1, MakeIntExpression(ExpressionNode::c_i_, 0x80));
                    d->atomicOp = Optimizer::ao_signal_fence;
                    *tp = &stdvoid;
                    break;
                case Keyword::c11_atomic_load_:
                    expression_assign(funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!tpf->IsPtr())
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = tpf->BaseType()->btp;
                        }
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_load;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    break;
                case Keyword::c11_atomic_store_:
                    expression_assign(funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!tpf->IsPtr())
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = tpf->BaseType()->btp;
                        }
                    }
                    if (needkw(Keyword::comma_))
                    {
                        Type* tpf1;
                        expression_assign(funcsp, nullptr, &tpf1, &d->value, nullptr, flags);
                        if (!d->tp->SameType(tpf1))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->value = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_store;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    d->memoryOrder1 =
                        MakeExpression(ExpressionNode::add_, d->memoryOrder1, MakeIntExpression(ExpressionNode::c_i_, 0x80));
                    break;
                    // there's extremely likely a better way to do this, maybe a lookup table for these and using that...
                case Keyword::c11_atomic_xchg_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::assign_, true);
                    break;
                case Keyword::c11_atomic_ftchadd_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asplus_, true);
                    break;
                case Keyword::c11_atomic_ftchsub_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asminus_, true);
                    break;
                case Keyword::c11_atomic_ftchand_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asand_, true);
                    break;
                case Keyword::c11_atomic_ftchor_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asor_, true);
                    break;
                case Keyword::c11_atomic_ftchxor_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asxor_, true);
                    break;
                case Keyword::atomic_addftch_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asplus_, false);
                    break;
                case Keyword::atomic_subftch_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asminus_, false);
                    break;
                case Keyword::atomic_andftch_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asand_, false);
                    break;
                case Keyword::atomic_orftch_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asor_, false);
                    break;
                case Keyword::atomic_xorftch_:
                    atomic_modify_specific_op(funcsp, tp, &tpf, d, flags, Keyword::asxor_, false);
                    break;
                case Keyword::atomic_cmpxchg_n_: {
                    bool weak = false;
                    expression_assign(funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!tpf->IsPtr())
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = tpf->BaseType()->btp;
                        }
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf1, &d->third, nullptr, flags);
                        if (!tpf->SameType(tpf1))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->third = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->value, nullptr, flags);
                        if (!tpf->SameType(*tp))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        if (!(*tp)->IsStructured())
                        {
                            cast(*tp, &d->value);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->value = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    if (needkw(Keyword::comma_))
                    {
                        Type* weak_type = nullptr;
                        EXPRESSION* weak_expr = nullptr;

                        optimized_expression(funcsp, nullptr, &weak_type, &weak_expr, false);
                        if (!isintconst(weak_expr))
                        {
                            error(ERR_NEED_INTEGER_TYPE);
                        }
                        weak = (bool)weak_expr->v.i;
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf1, &d->memoryOrder2, nullptr, flags);
                    }
                    else
                    {
                        tpf1 = &stdint;
                        d->memoryOrder2 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = weak ? Optimizer::ao_cmpxchgweak : Optimizer::ao_cmpxchgstrong;
                    if (!d->memoryOrder2)
                    {
                        d->memoryOrder2 = d->memoryOrder1;
                    }
                    *tp = &stdint;
                    break;
                }
                case Keyword::c11_atomic_cmpxchg_weak_:
                case Keyword::c11_atomic_cmpxchg_strong_:
                    expression_assign(funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!tpf->IsPtr())
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = tpf->BaseType()->btp;
                        }
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf1, &d->third, nullptr, flags);
                        if (!tpf->SameType(tpf1))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->third = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }

                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->value, nullptr, flags);
                        if (!tpf->SameType(*tp))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        if (!(*tp)->IsStructured())
                            cast(*tp, &d->value);
                    }
                    else
                    {
                        *tp = &stdint;
                        d->value = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    if (needkw(Keyword::comma_))
                    {
                        expression_assign(funcsp, nullptr, &tpf1, &d->memoryOrder2, nullptr, flags);
                    }
                    else
                    {
                        tpf1 = &stdint;
                        d->memoryOrder2 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = kw == Keyword::c11_atomic_cmpxchg_weak_ ? Optimizer::ao_cmpxchgweak : Optimizer::ao_cmpxchgstrong;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    *tp = &stdint;
                    break;
                default:
                    break;
            }
            if (tpf && !tpf->IsInt())
            {
                error(ERR_NEED_INTEGER_TYPE);
                d->memoryOrder1 = *exp = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_relaxed);
            }
            if (d->memoryOrder1)
                optimize_for_constants(&d->memoryOrder1);
            if (!needkw(Keyword::closepa_))
            {
                errskim(skim_closepa);
                skip(Keyword::closepa_);
            }
#ifdef NEED_CONST_MO
            if (d->memoryOrder1 && !isintconst(d->memoryOrder1))
                d->memoryOrder1 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
            if (d->memoryOrder2 && !isintconst(d->memoryOrder2))
                d->memoryOrder2 = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_seq_cst);
#endif
            *exp = MakeExpression(ExpressionNode::atomic_);
            (*exp)->v.ad = d;
        }
    }
    else
    {
        *tp = &stdint;
        *exp = MakeIntExpression(ExpressionNode::c_i_, Optimizer::mo_relaxed);
        errskim(skim_closepa);
        skip(Keyword::closepa_);
    }
    return;
}
static bool validate_checked_args(std::list<Argument*>* args)
{

    if (!args || args->size() < 3)
    {
        errorstr(ERR_PARAMETER_LIST_TOO_SHORT, "<checked math>");
        return false;
    }
    else if (args->size() > 3)
    {
        errorstr(ERR_PARAMETER_LIST_TOO_LONG, "<checked math>");
        return false;
    }
    else
        for (auto&& arg : *args)
        {
            Type* tp = arg->tp->BaseType();
            if (&arg == &args->front())
            {
                if (!tp->IsPtr() || tp->BaseType()->btp->IsConst())
                {
                    error(ERR_EXPECTED_POINTER_FOR_CHECKED_MATH_RESULT);
                    return false;
                }
                else
                {
                    tp = tp->btp->BaseType();
                }
            }
            if (!tp->IsInt() || tp->type == BasicType::char_ || tp->type == BasicType::bitint_ ||
                tp->type == BasicType::unsigned_bitint_ || tp->type == BasicType::bool_)
            {
                error(ERR_EXPECTED_VALID_CHECKED_MATH_TYPE);
                return false;
            }
        }
    return true;
}
static int ___typeid_val(Type* tp);
static std::list<Argument*>* checked_arguments(std::list<Argument*>* args)
{
    Argument* arr[3];
    int i = 0;
    // already been validated at three arguments
    for (auto entry : *args)
        arr[i++] = entry;
    for (int i = 1; i < 3; i++)
    {
        arr[i]->tp = Type::MakeType(BasicType::pointer_, arr[i]->tp);
        auto exp = arr[i]->exp;
        while (IsCastValue(exp))
            exp = exp->left;
        TakeAddress(&exp);
        arr[i]->exp = exp;
    }
    args->clear();
    for (int i = 0; i < 3; i++)
    {
        args->push_back(arr[i]);
        Argument* typeval = Allocate<Argument>();
        typeval->tp = &stdint;
        typeval->exp = MakeIntExpression(ExpressionNode::c_i_, ___typeid_val(arr[i]->tp->BaseType()->btp->BaseType()));
        args->push_back(typeval);
    }
    return args;
}
static void expression_checked_int( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    Keyword kw = KW();
    getsym();
    if (MATCHKW(Keyword::openpa_))
    {
        CallSite* funcparams = Allocate<CallSite>();
        getArgs(funcsp, funcparams, Keyword::closepa_, true, flags);
        if (validate_checked_args(funcparams->arguments))
        {
            const char* name = nullptr;
            switch (kw)
            {
                case Keyword::ckdadd_:
                    name = "___ckdadd";
                    break;
                case Keyword::ckdmul_:
                    name = "___ckdmul";
                    break;
                case Keyword::ckdsub_:
                    name = "___ckdsub";
                    break;
                default:
                    break;
            }
            if (name)
            {
                auto sym = gsearch(name);
                if (!sym)
                {
                    diag("expression_checked_int unknown func");
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                }
                else
                {
                    sym = sym->tp->syms->front();
                    funcparams->sp = sym;
                    funcparams->functp = sym->tp;
                    funcparams->fcall = MakeExpression(ExpressionNode::pc_, sym);
                    funcparams->ascall = true;
                    funcparams->arguments = checked_arguments(funcparams->arguments);
                    *exp = MakeExpression(funcparams);
                }
            }
            else
            {
                diag("expression_checked_int unknown func2");
                *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            }
        }
        else
        {
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        }

        *tp = &stdbool;
    }
    else
    {
        needkw(Keyword::openpa_);
        *tp = &stdint;
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        errskim(skim_closepa);
    }
    return;
}
static int ___typeid_val(Type* tp)
{
    int id;
    switch (tp->type)
    {
        case BasicType::bool_:
        case BasicType::char_:
        case BasicType::signed_char_:
            id = -1;
            break;
        case BasicType::short_:
            id = -2;
            break;
        case BasicType::int_:
            id = -3;
            break;
        case BasicType::long_:
            id = -4;
            break;
        case BasicType::long_long_:
            id = -5;
            break;
        case BasicType::bitint_:
            id = -(0x40000000 | tp->bitintbits);
            break;
        case BasicType::unsigned_bitint_:
            id = 0x40000000 | tp->bitintbits;
            break;
        case BasicType::unsigned_char_:
            id = 1;
            break;
        case BasicType::unsigned_short_:
        case BasicType::wchar_t_:
            id = 2;
            break;
        case BasicType::unsigned_:
            id = 3;
            break;
        case BasicType::unsigned_long_:
            id = 4;
            break;
        case BasicType::unsigned_long_long_:
            id = 5;
            break;
        case BasicType::float_:
            id = 7;
            break;
        case BasicType::double_:
            id = 8;
            break;
        case BasicType::long_double_:
            id = 10;
            break;
        case BasicType::float__imaginary_:
            id = 15;
            break;
        case BasicType::double__imaginary_:
            id = 16;
            break;
        case BasicType::long_double_imaginary_:
            id = 17;
            break;
        case BasicType::float__complex_:
            id = 20;
            break;
        case BasicType::double__complex_:
            id = 21;
            break;
        case BasicType::long_double_complex_:
            id = 22;
            break;
        default:
            id = 100000;
            break;
    }
    return id;
}
static void expression___typeid( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    getsym();
    if (needkw(Keyword::openpa_))
    {
        expression_comma(funcsp, nullptr, tp, exp, nullptr, _F_SIZEOF);
        if (!*tp)
        {
            error(ERR_TYPE_NAME_EXPECTED);
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        }
        else
        {
            Type* tp1 = *tp;
            if (tp1->IsRef())
                *tp = tp1->BaseType()->btp;  // DAL fixed
            *exp = MakeIntExpression(ExpressionNode::c_i_, ___typeid_val(tp1));
        }
        *tp = &stdint;
        needkw(Keyword::closepa_);
    }
    return;
}
static void expression_statement( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable,
                                     int flags)
{
    // gcc extension
    if (expressionStatements.size())
    {
        expressionReturns.push_back(std::pair<EXPRESSION*, Type*>(nullptr, nullptr));
        StatementGenerator sg(funcsp);
        sg.Compound(*expressionStatements.top(), false);
        std::pair<EXPRESSION*, Type*> rv = std::move(expressionReturns.back());
        expressionReturns.pop_back();
        *tp = rv.second;
        *exp = rv.first;
        if (!*tp)
            *tp = &stdvoid;
        if (!*exp)
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
    }
    else
    {
        errskim(skim_end);
        if (currentLex)
            needkw(Keyword::end_);
        *tp = &stdvoid;
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
    }
    return;
}

static void expression_primary( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    switch (currentLex ? currentLex->type : LexType::none_)
    {
        case LexType::l_id_:
            variableName(funcsp, atp, tp, exp, ismutable, flags);
            break;
        case LexType::l_kw_:
            switch (KW())
            {
                SYMBOL* sym;
                case Keyword::openbr_:
                    if (Optimizer::cparams.prm_cplusplus)
                        expression_lambda(funcsp, atp, tp, exp, flags);
                    break;
                case Keyword::classsel_:
                case Keyword::operator_:
                case Keyword::decltype_:
                    variableName(funcsp, atp, tp, exp, ismutable, flags);
                    break;
                case Keyword::nullptr_:
                    *exp = MakeIntExpression(ExpressionNode::nullptr_, 0);
                    *tp = &stdnullpointer;
                    getsym();
                    break;
                case Keyword::this_:
                    if (lambdas.size())
                    {
                        lambda_capture(nullptr, cmThis, true);
                        if (lambdas.front()->captureThis)
                        {
                            SYMBOL* ths = search(lambdas.front()->cls->tp->syms, lambdas.front()->thisByVal ? "*this" : "$this");
                            if (ths)
                            {
                                *tp = Type::MakeType(BasicType::pointer_, lambdas.front()->lthis->tp);
                                *exp = MakeExpression(ExpressionNode::auto_,
                                                      (SYMBOL*)funcsp->tp->BaseType()->syms->front());  // this ptr
                                Dereference(&stdpointer, exp);
                                *exp = MakeExpression(ExpressionNode::structadd_, *exp,
                                                      MakeIntExpression(ExpressionNode::c_i_, ths->sb->offset));
                                if (!lambdas.front()->thisByVal)
                                {
                                    Dereference(&stdpointer, exp);
                                }
                            }
                            else
                            {
                                diag("expression_primary: missing lambda this");
                            }
                        }
                        else
                        {
                            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                            *tp = &stdint;
                        }
                    }
                    else if (enclosingDeclarations.GetFirst() && theCurrentFunc && (theCurrentFunc->sb->parentClass || theCurrentFunc->sb->storage_class == StorageClass::member_))
                    {
                        // the complexity is a nod to the fact that 'this' can appear in a trailing return statement...
                        auto old = theCurrentFunc->sb->parentClass;
                        if (!old)
                            theCurrentFunc->sb->parentClass = enclosingDeclarations.GetFirst();
                        getThisType(theCurrentFunc, tp);
                        theCurrentFunc->sb->parentClass = old;
                        *exp = MakeExpression(ExpressionNode::auto_, (SYMBOL*)theCurrentFunc->tp->BaseType()->syms->front());  // this ptr
                        Dereference(&stdpointer, exp);
                    }
                    else
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        *tp = &stdint;
                        error(ERR_THIS_MEMBER_FUNC);
                    }
                    getsym();
                    break;
                case Keyword::I_:
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    (*exp)->type = ExpressionNode::c_fi_;
                    (*exp)->v.f = Allocate<FPF>();
                    *(*exp)->v.f = (long long)1;
                    *tp = &stdfloatimaginary;
                    (*exp)->pragmas = preProcessor->GetStdPragmas();
                    getsym();
                    break;
                case Keyword::offsetof_:
                    expression_offsetof(funcsp, tp, exp, ismutable, flags);
                    break;
                case Keyword::volatile__:
                    getsym();
                    expression_no_comma(funcsp, nullptr, tp, exp, nullptr, 0);
                    *exp = GetSymRef(*exp);
                    if (!*exp)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    else
                    {
                        if (!(*exp)->v.sp->tp->IsVolatile())
                        {
                            (*exp)->v.sp->tp = Type::MakeType(BasicType::volatile_, (*exp)->v.sp->tp);
                        }
                    }
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    *tp = &stdvoid;
                    break;
                case Keyword::land_:
                    // computed goto: take the address of a label
                    getsym();
                    if (!ISID())
                    {
                        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                        error(ERR_IDENTIFIER_EXPECTED);
                    }
                    else
                    {
                        *exp = MakeIntExpression(ExpressionNode::labcon_, StatementGenerator::GetLabelValue(nullptr, nullptr));
                        (*exp)->computedLabel = true;
                        getsym();
                    }
                    *tp = &stdpointer;
                    break;
                case Keyword::true_:
                    getsym();
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
                    (*exp)->type = ExpressionNode::c_bool_;
                    *tp = &stdbool;
                    break;
                case Keyword::false_:
                    getsym();
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    (*exp)->type = ExpressionNode::c_bool_;
                    *tp = &stdbool;
                    return;
                case Keyword::alloca_:
                    expression_alloca(funcsp, tp, exp, flags);
                    return;
                case Keyword::initblk_:
                case Keyword::cpblk_:
                    expression_msilfunc(funcsp, tp, exp, flags);
                    break;
                case Keyword::openpa_: {
                    getsym();
                    if (MATCHKW(Keyword::begin_))
                    {
                        expression_statement(funcsp, nullptr, tp, exp, ismutable, flags);
                        needkw(Keyword::closepa_);
                        break;
                    }
                    else if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
                    {

                        // unary left folding
                        RequiresDialect::Feature(Dialect::cpp17, "Fold expressions");
                        getsym();
                        if (Optimizer::cparams.prm_cplusplus && KWTYPE(TT_OPERATOR))
                        {
                            LexemeStreamPosition placeHolder(currentStream);
                            if (!KWTYPE(TT_ASSIGN | TT_BINARY))
                            {
                                getsym();
                                error(ERR_FOLDING_USES_INVALID_OPERATOR);
                                errskim(skim_closepa);
                            }
                            else
                            {
                                getsym();
                                LexemeStreamPosition start(currentStream);
                                expression_cast(funcsp, atp, tp, exp, nullptr, flags);
                                if (!*tp)
                                {
                                    *tp = &stdint;
                                    error(ERR_EXPRESSION_SYNTAX);
                                    errskim(skim_closepa);
                                }
                                else
                                {
                                    placeHolder.Replay([&]() {
                                        eval_unary_left_fold(funcsp, atp, tp, exp, start, *tp, *exp, false, flags);
                                    });
                                    needkw(Keyword::closepa_);
                                }
                            }
                        }
                        else
                        {
                            error(ERR_FOLDING_NEEDS_OPERATOR);
                            errskim(skim_closepa);
                        }
                        break;
                    }
                    else
                    {
                        LexemeStreamPosition start(currentStream);
                        expression_comma(funcsp, nullptr, tp, exp, ismutable,
                                               (flags & ~(_F_INTEMPLATEPARAMS | _F_SELECTOR | _F_NOVARIADICFOLD)) |
                                                   _F_EXPRESSIONINPAREN);
                        if (!*tp)
                        {
                            *tp = &stdint;
                            error(ERR_EXPRESSION_SYNTAX);
                            errskim(skim_closepa);
                        }
                        else if (Optimizer::cparams.prm_cplusplus && KWTYPE(TT_OPERATOR))
                        {
                            // unary right folding, or binary folding
                            RequiresDialect::Feature(Dialect::cpp17, "Fold expressions");
                            LexemeStreamPosition placeHolder(currentStream);
                            if (!KWTYPE(TT_ASSIGN | TT_BINARY))
                            {
                                getsym();
                                error(ERR_FOLDING_USES_INVALID_OPERATOR);
                                errskim(skim_closepa);
                            }
                            else
                            {
                                getsym();
                                if (!MATCHKW(Keyword::ellipse_))
                                {
                                    diag("expression_primary: variadic folding missing ellipse");
                                }
                                else
                                {
                                    getsym();
                                }
                                if (Optimizer::cparams.prm_cplusplus && KWTYPE(TT_OPERATOR))
                                {
                                    // binary folding
                                    if (!MATCHKW((placeHolder.get())->kw->key))
                                    {
                                        error(ERR_BINARY_FOLDING_OPERATOR_MISMATCH);
                                    }
                                    getsym();
                                    Type* tp1 = nullptr;
                                    EXPRESSION* exp1 = nullptr;
                                    LexemeStreamPosition start2(currentStream);;
                                    expression_cast(funcsp, atp, &tp1, &exp1, nullptr,
                                                          flags & ~(_F_INTEMPLATEPARAMS | _F_SELECTOR | _F_NOVARIADICFOLD));
                                    if (!tp1)
                                    {
                                        error(ERR_EXPRESSION_SYNTAX);
                                        errskim(skim_closepa);
                                    }
                                    else
                                    {
                                        placeHolder.Replay([&]() {
                                            eval_binary_fold(funcsp, atp, tp, exp, start, *tp, *exp, start2, tp1, exp1, false,
                                                         flags);
                                        });
                                    }
                                }
                                else
                                {
                                    placeHolder.Replay([&]() {
                                        eval_unary_right_fold(funcsp, atp, tp, exp, start, *tp, *exp, false, flags);
                                    });
                                }
                            }
                        }
                        //  fixme                      else if (!processingTemplateArgs)
                        //                       {
                        //                          if (argumentNestingLevel <= 1)
                        //                           checkUnpackedExpression(*exp);
                        //                 }
                        needkw(Keyword::closepa_);
                        break;
                    }
                }
                case Keyword::func_:
                    *tp = &std__func__;
                    if (!funcsp->sb->__func__label || Optimizer::msilstrings)
                    {
                        LCHAR buf[256], *q = buf;
                        const char* p = funcsp->name;
                        StringData* string;
                        string = Allocate<StringData>();
                        string->strtype = LexType::l_astr_;
                        string->size = 1;
                        string->pointers = Allocate<Optimizer::SLCHAR*>();
                        while (*p)
                            *q++ = *p++;
                        *q = 0;
                        string->pointers[0] = Allocate<Optimizer::SLCHAR>();
                        string->pointers[0]->str = wlitlate(buf);
                        string->pointers[0]->count = q - buf;
                        *exp = stringlit(string);
                        funcsp->sb->__func__label = string->label;
                    }
                    else
                    {
                        *exp = MakeIntExpression(ExpressionNode::labcon_, funcsp->sb->__func__label);
                        (*exp)->altdata = MakeIntExpression(ExpressionNode::c_i_, (int)LexType::l_astr_);
                    }
                    getsym();
                    break;
                case Keyword::uuidof_:
                    getsym();
                    needkw(Keyword::openpa_);
                    if (ISID())
                    {
                        sym = tsearch(currentLex->value.s.a);
                        getsym();
                    }
                    else
                    {
                        Type* tp1;
                        EXPRESSION* exp1;
                        expression_no_comma(funcsp, nullptr, &tp1, &exp1, nullptr, 0);
                        if (tp1 && tp1->IsStructured())
                        {
                            sym = tp1->BaseType()->sp;
                        }
                        else
                        {
                            sym = nullptr;
                        }
                    }
                    needkw(Keyword::closepa_);
                    *exp = GetUUIDData(sym);
                    *tp = &stdpointer;
                    break;
                case Keyword::va_typeof_:
                    getsym();
                    if (MATCHKW(Keyword::openpa_))
                    {
                        getsym();
                        if (TypeGenerator::StartOfType(nullptr, false))
                        {
                            SYMBOL* sym;
                            *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
                            (*tp)->used = true;
                            needkw(Keyword::closepa_);
                            // don't enter in table, this is purely so we can cache the type info
                            sym = makeID(StorageClass::auto_, *tp, nullptr, AnonymousName());
                            sym->sb->va_typeof = true;
                            *exp = MakeExpression(ExpressionNode::auto_, sym);
                            break;
                        }
                    }
                    error(ERR_TYPE_NAME_EXPECTED);
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    *tp = &stdint;
                    break;
                case Keyword::typeid__:
                    expression___typeid(funcsp, tp, exp);
                    break;
                case Keyword::has_c_attribute_:
                    getsym();
                    *tp = &stdint;
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    if (needkw(Keyword::openpa_))
                    {
                        if (ISID())
                        {
                            (*exp)->v.i = cattributes[currentLex->value.s.a];
                            getsym();
                        }
                        else
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                        needkw(Keyword::closepa_);
                    }
                    break;
                case Keyword::D0_:
                case Keyword::D1_:
                case Keyword::D2_:
                case Keyword::D3_:
                case Keyword::D4_:
                case Keyword::D5_:
                case Keyword::D6_:
                case Keyword::D7_:
                case Keyword::A0_:
                case Keyword::A1_:
                case Keyword::A2_:
                case Keyword::A3_:
                case Keyword::A4_:
                case Keyword::A5_:
                case Keyword::A6_:
                case Keyword::A7_:
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    *tp = &stdint;
                    getsym();
                    break;
                case Keyword::NAN_:
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    (*exp)->type = ExpressionNode::c_f_;
                    (*exp)->v.f = Allocate<FPF>();
                    (*exp)->v.f->SetNaN();
                    getsym();
                    *tp = &stdfloat;
                    break;
                case Keyword::INF_:
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    (*exp)->type = ExpressionNode::c_f_;
                    (*exp)->v.f = Allocate<FPF>();
                    (*exp)->v.f->SetInfinity(0);
                    getsym();
                    *tp = &stdfloat;
                    break;
                case Keyword::generic_:
                    expression_generic(funcsp, tp, exp, flags);
                    break;
                case Keyword::atomic_addftch_:
                case Keyword::atomic_subftch_:
                case Keyword::atomic_andftch_:
                case Keyword::atomic_xorftch_:
                case Keyword::atomic_orftch_:
                case Keyword::atomic_cmpxchg_n_:
                case Keyword::atomic_flag_test_set_:
                case Keyword::atomic_flag_clear_:
                case Keyword::atomic_kill_dependency_:
                case Keyword::c11_atomic_is_lock_free_:
                case Keyword::c11_atomic_load_:
                case Keyword::c11_atomic_store_:
                case Keyword::c11_atomic_cmpxchg_strong_:
                case Keyword::c11_atomic_cmpxchg_weak_:
                case Keyword::c11_atomic_ftchadd_:
                case Keyword::c11_atomic_ftchsub_:
                case Keyword::c11_atomic_xchg_:
                case Keyword::c11_atomic_ftchand_:
                case Keyword::c11_atomic_ftchor_:
                case Keyword::c11_atomic_ftchxor_:
                case Keyword::c11_atomic_init_:
                case Keyword::c11_atomic_signal_fence_:
                case Keyword::c11_atomic_thread_fence_:
                    expression_atomic_func(funcsp, tp, exp, flags);
                    break;
                case Keyword::ckdadd_:
                case Keyword::ckdsub_:
                case Keyword::ckdmul_:
                    expression_checked_int(funcsp, tp, exp, flags);
                    break;
                case Keyword::typename_:
                    *tp = nullptr;
                    expression_func_type_cast(funcsp, tp, exp, flags);
                    break;
                default:
                    *tp = nullptr;
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                    getsym();
                    break;
            }
            break;
        case LexType::i_:
            *exp = MakeIntExpression(ExpressionNode::c_i_, currentLex->value.i);
            *tp = &stdint;
            getsym();
            break;
        case LexType::ui_:
            *exp = MakeIntExpression(ExpressionNode::c_ui_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_ui_;
            *tp = &stdunsigned;
            getsym();
            break;
        case LexType::l_:
            *exp = MakeIntExpression(ExpressionNode::c_l_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_l_;
            *tp = &stdlong;
            getsym();
            break;
        case LexType::ul_:
            *exp = MakeIntExpression(ExpressionNode::c_ul_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_ul_;
            *tp = &stdunsignedlong;
            getsym();
            break;
        case LexType::ll_:
            *exp = MakeIntExpression(ExpressionNode::c_ll_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_ll_;
            *tp = &stdlonglong;
            getsym();
            break;
        case LexType::ull_:
            if (!currentLex->suffix || !getSuffixedNumber(funcsp, tp, exp))
            {
                *exp = MakeIntExpression(ExpressionNode::c_ull_, currentLex->value.i);
                (*exp)->type = ExpressionNode::c_ull_;
                *tp = &stdunsignedlonglong;
            }
            getsym();
            break;
        case LexType::bitint_:
            *exp = MakeExpression(ExpressionNode::c_bitint_);
            (*exp)->v.b.bits = currentLex->value.b.bits;
            (*exp)->v.b.value = currentLex->value.b.value;
            *tp = Type::MakeType(BasicType::bitint_);
            (*tp)->bitintbits = currentLex->value.b.bits;
            (*tp)->size = currentLex->value.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
            (*tp)->size /= Optimizer::chosenAssembler->arch->bitintunderlying;
            (*tp)->size *= Optimizer::chosenAssembler->arch->bitintunderlying;
            (*tp)->size /= CHAR_BIT;
            getsym();
            break;
        case LexType::ubitint_:
            *exp = MakeExpression(ExpressionNode::c_ubitint_);
            (*exp)->v.b.bits = currentLex->value.b.bits;
            (*exp)->v.b.value = currentLex->value.b.value;
            *tp = Type::MakeType(BasicType::unsigned_bitint_);
            (*tp)->bitintbits = currentLex->value.b.bits;
            (*tp)->size = currentLex->value.b.bits + Optimizer::chosenAssembler->arch->bitintunderlying - 1;
            (*tp)->size /= Optimizer::chosenAssembler->arch->bitintunderlying;
            (*tp)->size *= Optimizer::chosenAssembler->arch->bitintunderlying;
            (*tp)->size /= CHAR_BIT;
            getsym();
            break;
        case LexType::l_f_:
            *exp = MakeIntExpression(ExpressionNode::c_f_, 0);
            (*exp)->v.f = Allocate<FPF>();
            *(*exp)->v.f = *currentLex->value.f;
            (*exp)->pragmas = preProcessor->GetStdPragmas();
            *tp = &stdfloat;
            getsym();
            break;
        case LexType::l_d_:
            *exp = MakeIntExpression(ExpressionNode::c_d_, 0);
            (*exp)->v.f = Allocate<FPF>();
            *(*exp)->v.f = *currentLex->value.f;
            (*exp)->pragmas = preProcessor->GetStdPragmas();
            *tp = &stddouble;
            getsym();
            break;
        case LexType::l_ld_:
            if (!currentLex->suffix || !getSuffixedNumber(funcsp, tp, exp))
            {
                *exp = MakeIntExpression(ExpressionNode::c_ld_, 0);
                (*exp)->v.f = Allocate<FPF>();
                *(*exp)->v.f = *currentLex->value.f;
                (*exp)->pragmas = preProcessor->GetStdPragmas();
                *tp = &stdlongdouble;
            }
            getsym();
            break;
        case LexType::l_astr_:
        case LexType::l_wstr_:
        case LexType::l_ustr_:
        case LexType::l_Ustr_:
        case LexType::l_msilstr_:
        case LexType::l_u8str_:
            expression_string(funcsp, tp, exp);
            break;
        case LexType::l_wchr_:
            *exp = MakeIntExpression(ExpressionNode::c_wc_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_wc_;
            *tp = &stdwidechar;
            if (currentLex->suffix)
                getSuffixedChar(funcsp, tp, exp);
            getsym();
            break;
        case LexType::l_achr_:
            *exp = MakeIntExpression(ExpressionNode::c_c_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_c_;
            *tp = &stdchar;
            if (currentLex->suffix)
                getSuffixedChar(funcsp, tp, exp);
            getsym();
            break;
        case LexType::l_u8chr_:
            *exp = MakeIntExpression(ExpressionNode::c_uc_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_uc_;
            *tp = &stdunsignedchar;
            if (currentLex->suffix)
                getSuffixedChar(funcsp, tp, exp);
            getsym();
            break;
        case LexType::l_uchr_:
            *exp = MakeIntExpression(ExpressionNode::c_u16_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_u16_;
            *tp = stdchar16tptr.btp;
            if (currentLex->suffix)
                getSuffixedChar(funcsp, tp, exp);
            getsym();
            break;
        case LexType::l_Uchr_:
            *exp = MakeIntExpression(ExpressionNode::c_u32_, currentLex->value.i);
            (*exp)->type = ExpressionNode::c_u32_;
            *tp = stdchar32tptr.btp;
            if (currentLex->suffix)
                getSuffixedChar(funcsp, tp, exp);
            getsym();
            break;
        default:
            *tp = nullptr;
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            break;
    }
    return;
}
static int widelen(LCHAR* s)
{
    int rv = 0;
    while (*s++)
        rv++;
    return rv;
}
static void expression_sizeof( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    int paren = false;
    *exp = nullptr;
    getsym();
    if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
    {
        getsym();
        if (MATCHKW(Keyword::openpa_))
        {
            paren = true;
            getsym();
        }
        if (!ISID())
        {
            error(ERR_SIZEOFELLIPSE_NEEDS_TEMPLATE_PACK);
            errskim(skim_closepa);
        }
        else
        {
            EXPRESSION* exp1 = nullptr;
            EnterPackedSequence();
            variableName(funcsp, nullptr, tp, &exp1, nullptr, _F_PACKABLE | _F_SIZEOF);
            ClearPackedSequence();
            LeavePackedSequence();
            if (!exp1 || !exp1->v.sp->tp->templateParam || !exp1->v.sp->tp->templateParam->second->packed)
            {
                *tp = &stdunsigned;
                *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
            }
            else if (templateDefinitionLevel)
            {
                *exp = MakeIntExpression(ExpressionNode::sizeofellipse_, 0);
                (*exp)->v.templateParam = (*tp)->templateParam;
                *tp = &stdunsigned;
            }
            else if (!(*tp)->templateParam->second->packed)
            {
                *tp = &stdunsigned;
                *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
            }
            else
            {
                int n = 0;
                if ((*tp)->templateParam->second->byPack.pack)
                    n = (*tp)->templateParam->second->byPack.pack->size();
                *tp = &stdunsigned;
                *exp = MakeIntExpression(ExpressionNode::c_i_, n);
            }
        }
        if (paren)
            needkw(Keyword::closepa_);
    }
    else
    {
        if (MATCHKW(Keyword::openpa_))
        {
            paren = true;
            getsym();
        }
        if (!TypeGenerator::StartOfType(nullptr, false))
        {
            if (paren)
            {
                expression_comma(funcsp, nullptr, tp, exp, nullptr, _F_SIZEOF);
                needkw(Keyword::closepa_);
            }
            else
            {
                expression_unary(funcsp, nullptr, tp, exp, nullptr, _F_SIZEOF);
            }
            ResolveTemplateVariable(tp, exp, &stdint, nullptr);
            if (!*tp)
            {
                *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
                error(ERR_EXPRESSION_SYNTAX);
            }
            else
            {
                *exp = nodeSizeof(*tp, *exp, _F_SIZEOF);
            }
        }
        else
        {
            LexemeStreamPosition prev(currentStream);
            *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, Optimizer::cparams.prm_cplusplus, true, false);
            if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::openpa_))
            {
                getsym();
                if (MATCHKW(Keyword::star_) || MATCHKW(Keyword::cdecl_) || MATCHKW(Keyword::stdcall_))
                {
                    prev.Backup();
                    *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
                }
                else
                {
                    prev.Backup();
                    expression_func_type_cast(funcsp, tp, exp, 0);
                }
            }
            if (paren)
                needkw(Keyword::closepa_);
            if (unpackingTemplate && (*tp)->type == BasicType::templateparam_ && (*tp)->templateParam->second->packed)
            {
                auto tpl = LookupPackedInstance(*(*tp)->templateParam);
                if (tpl)
                    *tp = tpl->byClass.val;
            }
            if (!*tp)
            {
                *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
            }
            else
            {
                checkauto(*tp, ERR_AUTO_NOT_ALLOWED);
                *exp = nodeSizeof(*tp, *exp, _F_SIZEOF);
            }
        }

        *tp = &stdint; /* other compilers use sizeof as a signed value so we do too... */
    }
    return;
}
static void expression_alignof( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    getsym();
    if (needkw(Keyword::openpa_))
    {
        if (TypeGenerator::StartOfType(nullptr, false))
        {
            *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
        }
        else
        {
            EXPRESSION* exp = nullptr;
            expression(funcsp, nullptr, tp, &exp, 0);
        }
        needkw(Keyword::closepa_);
        if (unpackingTemplate && (*tp)->type == BasicType::templateparam_ && (*tp)->templateParam->second->packed)
        {
            auto tpl = LookupPackedInstance(*(*tp)->templateParam);
            if (tpl)
                *tp = tpl->byClass.val;
        }
        if (!*tp)
        {
            *exp = MakeIntExpression(ExpressionNode::c_i_, 1);
        }
        else
        {
            Type* itp = *tp;
            checkauto(itp, ERR_AUTO_NOT_ALLOWED);
            if (itp->IsRef())
                itp = (itp->BaseType()->btp);
            itp = itp->BaseType();

            *exp = MakeIntExpression(ExpressionNode::c_i_, getAlign(StorageClass::global_, itp));
        }
    }
    *tp = &stdint;
    return;
}
static void expression_ampersand( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags)
{
    getsym();
    expression_cast(funcsp, atp, tp, exp, nullptr, (flags) | _F_AMPERSAND);
    if (*tp)
    {
        Type *btp, *tp1;
        EXPRESSION *exp1 = *exp, *symRef;
        if (exp1->type != ExpressionNode::const_)
        {
            optimize_for_constants(&exp1);
        }
        while (exp1->type == ExpressionNode::comma_ && exp1->right)
        {
            exp1 = exp1->right;
        }
        if (exp1->type == ExpressionNode::comma_)
        {
            exp1 = exp1->left;
        }
        symRef = (Optimizer::architecture == ARCHITECTURE_MSIL) ? GetSymRef(exp1) : nullptr;
        btp = (*tp)->BaseType();
        tp1 = LookupSingleAggregate(btp, &exp1, true);
        if (((*tp)->IsStructuredMath() || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            FindOperatorFunction(ovcl_unary_any, Keyword::unary_and_, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
        {
            return;
        }
        else if ((*tp)->IsVoid() || (*tp)->IsMsil())
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (btp->hasbits)
            error(ERR_CANNOT_TAKE_ADDRESS_OF_BIT_FIELD);
        else if (btp->msil)
            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
        else if (symRef && symRef->type != ExpressionNode::labcon_ &&
                 symRef->v.sp->sb->attribs.inheritable.linkage2 == Linkage::property_)
            errorsym(ERR_CANNOT_TAKE_ADDRESS_OF_PROPERTY, symRef->v.sp);
        else if (inreg(*exp, true))
            error(ERR_CANNOT_TAKE_ADDRESS_OF_REGISTER);
        else if ((!btp->IsPtr() || !(btp)->array) && !btp->IsStructured() && !btp->IsFunction() &&
                 btp->type != BasicType::aggregate_ && (exp1)->type != ExpressionNode::memberptr_)
        {
            if (exp1->type != ExpressionNode::callsite_)
                if ((exp1)->type != ExpressionNode::const_ && exp1->type != ExpressionNode::assign_)
                    if (!IsLValue(exp1))
                        if (Optimizer::cparams.prm_ansi || !IsCastValue(exp1))
                            if (!IsDefiningTemplate() || exp1->type != ExpressionNode::templateselector_)
                                error(ERR_MUST_TAKE_ADDRESS_OF_MEMORY_LOCATION);
            if (IsLValue(exp1))
            {
                if (exp1->left->type == ExpressionNode::structadd_ && isconstzero(&stdint, exp1->left->right) &&
                    exp1->left->left->type == ExpressionNode::l_ref_ && exp1->left->left->left->type == ExpressionNode::auto_)
                {
                    if (exp1->left->left->left->v.sp->sb->storage_class == StorageClass::parameter_)
                        exp1->left->left->left->v.sp->sb->addressTaken = true;
                }
            }
        }
        else
            switch ((exp1)->type)
            {
                case ExpressionNode::pc_:
                case ExpressionNode::auto_:
                case ExpressionNode::global_:
                case ExpressionNode::absolute_:
                case ExpressionNode::threadlocal_: {
                    SYMBOL* sym = exp1->v.sp;
                    if (sym->sb->isConstructor || sym->sb->isDestructor)
                        error(ERR_CANNOT_TAKE_ADDRESS_OF_CONSTRUCTOR_OR_DESTRUCTOR);
                    break;
                }
                default:
                    break;
            }
        if (exp1->type == ExpressionNode::memberptr_)
        {
            *tp = tp1;
            *exp = exp1;
        }
        else if ((*exp)->type == ExpressionNode::const_)
        {
            /* if a variable propagated silently to an inline constant
             * this will restore it as a static variable in the const section
             * when we take its address
             * but the constant will still be used inline when possible
             */
            SYMBOL* sym = (*exp)->v.sp;
            if (!sym->sb->indecltable)
            {
                if (!sym->sb->label)
                {
                    Type *tp, *tpb, *tpn, **tpnp = &tpn;
                    bool done = false;
                    SYMBOL* spold = sym;
                    sym->sb->label = Optimizer::nextLabel++;
                    sym = CopySymbol(sym);
                    spold->sb->indecltable = true;
                    tp = sym->tp;
                    tpb = tp->BaseType();
                    do
                    {
                        *tpnp = tp->CopyType();
                        (*tpnp)->UpdateRootTypes();
                        tpnp = &(*tpnp)->btp;
                        if (tp != tpb)
                            tp = tp->btp;
                        else
                            done = true;
                    } while (!done);
                    sym->tp = tpn;
                    sym->sb->storage_class = StorageClass::localstatic_;
                    SetLinkerNames(sym, Linkage::cdecl_);
                    Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::scc_localstatic;
                    insertInitSym(sym);
                }
                else
                {
                    insertInitSym(sym);
                }
                if (!sym->sb->parent)
                    sym->sb->parent = funcsp;  // this promotion of a global to local is necessary to not make it linkable
                *exp = MakeExpression(ExpressionNode::global_, sym);
            }
            else
            {
                *exp = MakeExpression(ExpressionNode::global_, sym);
            }
            *tp = Type::MakeType(BasicType::pointer_, *tp);
        }
        else if (!(*tp)->IsFunction() && (*tp)->type != BasicType::aggregate_)
        {
            if (!TakeAddress(exp, btp))
            {
                if (!btp->array && !btp->vla && !btp->IsStructured() && btp->BaseType()->type != BasicType::memberptr_ &&
                    btp->BaseType()->type != BasicType::templateparam_)
                    if (!IsDefiningTemplate() || (*exp)->type != ExpressionNode::templateselector_)
                        error(ERR_LVALUE);
            }
            if (btp->type != BasicType::memberptr_)
            {
                *tp = Type::MakeType(BasicType::pointer_, *tp);
            }
        }
    }
    return;
}
static void expression_deref( SYMBOL* funcsp, Type** tp, EXPRESSION** exp, int flags)
{
    getsym();
    expression_cast(funcsp, nullptr, tp, exp, nullptr, flags);
    if (((*tp)->IsStructuredMath() || Optimizer::architecture == ARCHITECTURE_MSIL) &&
        FindOperatorFunction(ovcl_unary_pointer, Keyword::unary_star_, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
    {
        return;
    }
    if (*tp && ((*tp)->IsVoid() || (*tp)->type == BasicType::aggregate_ || (*tp)->IsMsil()))
    {
        error(ERR_NOT_AN_ALLOWED_TYPE);
    }
    else if (*tp && (*tp)->BaseType()->type == BasicType::memberptr_)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    if (*tp && !(*tp)->IsFunction() && !(*tp)->IsFunctionPtr())
    {
        Type *btp = (*tp)->BaseType(), *btp2, *btp3;
        ;
        if (!(*tp)->IsPtr())
        {
            if (!IsDefiningTemplate())
                error(ERR_DEREF);
            Dereference(&stdpointer, exp);
        }
        else
        {
            btp3 = btp->btp;
            btp2 = btp->btp->BaseType();
            if (btp2->type == BasicType::void_)
            {
                error(ERR_DEREF);
                Dereference(&stdpointer, exp);
            }
            else if (btp2->IsStructured())
            {
                if ((*exp)->type == ExpressionNode::comma_)
                {
                    *exp = MakeExpression(ExpressionNode::lvalue_, *exp);
                }
                *tp = btp3;
            }
            else
            {
                if (!btp2->IsFunction())
                {
                    *tp = btp3;
                    Dereference(*tp, exp);
                    if (btp3->IsVolatile())
                        (*exp)->isvolatile = true;
                    if (btp3->IsRestrict())
                        (*exp)->isrestrict = true;
                    if (btp3->IsAtomic())
                    {
                        (*exp)->isatomic = true;
                        if (needsAtomicLockFromType(btp3))
                            (*exp)->lockOffset = btp3->BaseType()->size;
                    }
                }
            }
        }
    }
    tagNonConst(funcsp, *tp);
    return;
}
static void expression_postfix( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    Type* oldType;
    bool done = false;
    bool localMutable = false;
    if (KWTYPE(TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS))
    {
        expression_func_type_cast(funcsp, tp, exp, flags);
    }
    else
        switch (KW())
        {
            case Keyword::dynamic_cast_:
                oldType = nullptr;
                GetCastInfo(funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doDynamicCast(tp, oldType, exp, funcsp))
                    if (!typeHasTemplateArg(*tp))
                        errorConversionOrCast(false, oldType, *tp);
                if ((*tp)->IsRef())
                    *tp = (*tp)->BaseType()->btp;
                break;
            case Keyword::static_cast_:
                oldType = nullptr;
                GetCastInfo(funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doStaticCast(tp, oldType, exp, funcsp, true))
                    if (!typeHasTemplateArg(*tp))
                        errorConversionOrCast(false, oldType, *tp);
                if ((*tp)->IsRef())
                    *tp = (*tp)->BaseType()->btp;
                break;
            case Keyword::const_cast_:
                oldType = nullptr;
                GetCastInfo(funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doConstCast(tp, oldType, exp, funcsp))
                    if (!typeHasTemplateArg(*tp))
                        errorConversionOrCast(false, oldType, *tp);
                if ((*tp)->IsRef())
                    *tp = (*tp)->BaseType()->btp;
                break;
            case Keyword::reinterpret_cast_:
                oldType = nullptr;
                GetCastInfo(funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doReinterpretCast(tp, oldType, exp, funcsp, true))
                    if (!typeHasTemplateArg(*tp))
                        errorConversionOrCast(false, oldType, *tp);
                if ((*tp)->IsRef())
                    *tp = (*tp)->BaseType()->btp;
                break;
            case Keyword::typeid_:
                expression_typeid(funcsp, tp, exp, (flags & _F_PACKABLE));
                break;
            default:
                expression_primary(funcsp, atp, tp, exp, &localMutable, flags);
                if (ismutable)
                    *ismutable = localMutable;
                break;
        }
        if (!*tp)
        return;
    while (!done && currentLex && !parsingPreprocessorConstant)
    {
        Keyword kw;
        switch (KW())
        {
            case Keyword::openbr_:
                expression_bracket(funcsp, tp, exp, flags);
                break;
            case Keyword::openpa_:
                expression_arguments(funcsp, tp, exp, flags);
                break;
            case Keyword::pointsto_:
            case Keyword::dot_:
                expression_member(funcsp, tp, exp, ismutable, flags);
                break;
            case Keyword::autoinc_:
            case Keyword::autodec_:
                kw = KW();
                getsym();
                if ((*tp)->IsStructuredMath())
                {
                    if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                        FindOperatorFunction(ovcl_unary_postfix, kw, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
                    {
                        continue;
                    }
                    else
                    {
                        castToArithmetic(false, tp, exp, kw, nullptr, true);
                    }
                }
                if ((*tp)->IsConst() && !localMutable)
                    error(ERR_CANNOT_MODIFY_CONST_OBJECT);
                else if ((*tp)->IsStructured())
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (!IsLValue(*exp) && (*tp)->BaseType()->type != BasicType::templateparam_)
                {
                    error(ERR_LVALUE);
                }
                else
                {
                    CheckThroughConstObject(nullptr, *exp);
                    EXPRESSION *exp3 = nullptr, *exp1 = nullptr;
                    if ((*exp)->left->type == ExpressionNode::callsite_ || (*exp)->left->type == ExpressionNode::thisref_)
                    {
                        EXPRESSION* exp2 = AnonymousVar(StorageClass::auto_, *tp);
                        Dereference(&stdpointer, &exp2);
                        exp3 = MakeExpression(ExpressionNode::assign_, exp2, (*exp)->left);
                        Dereference(*tp, &exp2);
                        *exp = exp2;
                    }
                    if ((*tp)->BaseType()->type == BasicType::pointer_)
                    {
                        Type* btp = (*tp)->BaseType()->btp;
                        if (btp->BaseType()->type == BasicType::void_)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                error(ERR_ARITHMETIC_WITH_VOID_STAR);
                            exp1 = nodeSizeof(&stdchar, *exp);
                        }
                        else
                        {
                            exp1 = nodeSizeof(btp, *exp);
                        }
                    }
                    else
                    {
                        if ((*tp)->IsVoid() || (*tp)->type == BasicType::aggregate_ || (*tp)->IsMsil())
                            error(ERR_NOT_AN_ALLOWED_TYPE);
                        if ((*tp)->BaseType()->scoped && !(flags & _F_SCOPEDENUM))
                            error(ERR_SCOPED_TYPE_MISMATCH);
                        if ((*tp)->BaseType()->type == BasicType::memberptr_)
                            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                        exp1 = MakeIntExpression(ExpressionNode::c_i_, 1);
                    }
                    if ((*tp)->BaseType()->type == BasicType::bool_)
                    {
                        /* Keyword::autoinc_ of a bool sets it true.  Keyword::autodec_ not allowed in C++
                         * these aren't spelled out in the C99 standard, we are
                         * doing the normal thing here...
                         */
                        if (kw == Keyword::autodec_)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                error(ERR_CANNOT_USE_bool_HERE);
                            *exp = MakeExpression(ExpressionNode::assign_, *exp, MakeIntExpression(ExpressionNode::c_bool_, 0));
                        }
                        else
                        {
                            *exp = MakeExpression(ExpressionNode::assign_, *exp, MakeIntExpression(ExpressionNode::c_bool_, 1));
                        }
                    }
                    else
                    {
                        if ((*tp)->IsPtr())
                            cast(&stdint, &exp1);
                        else
                            cast(*tp, &exp1);
                        *exp = MakeExpression(kw == Keyword::autoinc_ ? ExpressionNode::auto_inc_ : ExpressionNode::auto_dec_, *exp,
                                              exp1);
                    }
                    if (exp3)
                        *exp = MakeExpression(ExpressionNode::comma_, exp3, *exp);
                    while (TakeAddress(&exp1))
                        ;
                    if (exp1->type == ExpressionNode::auto_)
                        exp1->v.sp->sb->altered = true;
                }
                break;
            default:
                done = true;
                break;
        }
    }
    return;
}
void expression_unary( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    bool localMutable = false;
    LexemeStreamPosition placeHolder(currentStream);
    /* note some of the math ops are speced to do integer promotions
     * if being stored, the proposed place to store them is not known, so e.g.
     * a ~ on a unsigned char would promote to int to be evaluated,
     * the int would be further sign
     * extended to a long long though through the assignment promotion
     */
    switch (KW())
    {
        case Keyword::plus_:
            getsym();
            expression_cast(funcsp, atp, tp, exp, nullptr, flags);
            if (*tp)
            {
                placeHolder.Replay([=]() {
                    eval_unary_plus(funcsp, atp, tp, exp, *tp, *exp, false, flags);
                });
            }
            break;
        case Keyword::minus_:
            getsym();
            expression_cast(funcsp, atp, tp, exp, nullptr, flags);
            if (!*tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
                *tp = &stdint;
            }
            else
            {
                placeHolder.Replay([=]() {
                    eval_unary_minus(funcsp, atp, tp, exp, *tp, *exp, false, flags);
                });
            }
            break;
        case Keyword::star_:
            expression_deref(funcsp, tp, exp, flags);
            break;
        case Keyword::and_:
            expression_ampersand(funcsp, atp, tp, exp, flags);
            break;
        case Keyword::not_:
            getsym();
            expression_cast(funcsp, atp, tp, exp, nullptr, flags);
            if (!*tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
                *tp = &stdint;
            }
            else
            {
                placeHolder.Replay([=]() {
                    eval_unary_not(funcsp, atp, tp, exp, *tp, *exp, false, flags);
                });
            }
            break;
        case Keyword::complx_:
            getsym();
            expression_cast(funcsp, atp, tp, exp, nullptr, flags);
            if (!*tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
                *tp = &stdint;
            }
            else
            {
                placeHolder.Replay([=]() {
                    eval_unary_complement(funcsp, atp, tp, exp, *tp, *exp, false, flags);
                });
            }
            break;
        case Keyword::autoinc_:
        case Keyword::autodec_:
            getsym();
            expression_cast(funcsp, atp, tp, exp, &localMutable, flags);
            if (*tp)
            {
                CheckThroughConstObject(nullptr, *exp);
                placeHolder.Replay([=]() {
                    eval_unary_autoincdec(funcsp, atp, tp, exp, *tp, *exp, localMutable, flags);
                });
            }
            break;
        case Keyword::sizeof_:
            expression_sizeof(funcsp, tp, exp);
            break;
        case Keyword::alignof_:
            expression_alignof(funcsp, tp, exp);
            break;
        case Keyword::new_:
            expression_new(funcsp, tp, exp, false, flags);
            break;
        case Keyword::delete_:
            expression_delete(funcsp, tp, exp, false, flags);
            break;
        case Keyword::noexcept_:
            expression_noexcept(funcsp, tp, exp);
            break;
        case Keyword::classsel_: {
            LexemeStreamPosition placeHolder(currentStream);
            getsym();
            switch (KW())
            {
                case Keyword::new_:
                    return expression_new(funcsp, tp, exp, true, flags);
                case Keyword::delete_:
                    return expression_delete(funcsp, tp, exp, true, flags);
                default:
                    break;
            }
            placeHolder.Backup();
        }
            // fallthrough
        default:
            expression_postfix(funcsp, atp, tp, exp, ismutable, flags);
            break;
    }
    return;
}
void expression_cast( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    Type* throwaway = nullptr;
    if (MATCHKW(Keyword::openpa_))
    {
        bool loadedAttribs = false;
        attributes oldAttribs;
        LexemeStreamPosition  start(currentStream);;
        getsym();
        if (MATCHKW(Keyword::attribute_))
        {
            loadedAttribs = true;
            oldAttribs = basisAttribs;

            basisAttribs = {0};
            ParseAttributeSpecifiers(funcsp, true);
        }
        bool structured = false;
        if (TypeGenerator::StartOfType(&structured, false))
        {
            if (!Optimizer::cparams.prm_cplusplus || StatementGenerator::ResolvesToDeclaration(structured))
            {
                bool done = false;
                *tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, false, true, false);
                if (!*tp)
                {
                    error(ERR_TYPE_NAME_EXPECTED);
                    *tp = &stdint;
                    *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                }
                else
                {
                    (*tp)->InstantiateDeferred();
                    (*tp)->used = true;
                    needkw(Keyword::closepa_);
                    checkauto(*tp, ERR_AUTO_NOT_ALLOWED);
                    if (MATCHKW(Keyword::begin_))
                    {
                        std::list<Initializer*>* temp = nullptr;
                        Initializer* init = nullptr;
                        SYMBOL* sym = nullptr;
                        if (!Optimizer::cparams.prm_cplusplus)
                            RequiresDialect::Feature(Dialect::c99, "Compound Literals");
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            sym = makeID(StorageClass::auto_, *tp, nullptr, AnonymousName());
                            localNameSpace->front()->syms->Add(sym);
                        }
                        initType(funcsp, 0, StorageClass::auto_, &temp, nullptr, *tp, sym, false, false, flags);
                        *exp = ConverInitializersToExpression(*tp, nullptr, nullptr, funcsp, temp, *exp, false);
                        while (!done && currentLex)
                        {
                            Keyword kw;
                            switch (KW())
                            {
                                case Keyword::openbr_:
                                    expression_bracket(funcsp, tp, exp, flags);
                                    break;
                                case Keyword::openpa_:
                                    expression_arguments(funcsp, tp, exp, flags);
                                    break;
                                case Keyword::pointsto_:
                                case Keyword::dot_:
                                    expression_member(funcsp, tp, exp, ismutable, flags);
                                    break;
                                case Keyword::autoinc_:
                                case Keyword::autodec_:
                                    kw = KW();
                                    getsym();
                                    if ((*tp)->IsStructuredMath())
                                    {
                                        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                                            FindOperatorFunction(ovcl_unary_postfix, kw, funcsp, tp, exp, nullptr, nullptr, nullptr,
                                                                 flags))
                                        {
                                            continue;
                                        }
                                        else
                                        {
                                            castToArithmetic(false, tp, exp, kw, nullptr, true);
                                        }
                                    }
                                    if ((*tp)->IsStructured())
                                        error(ERR_ILL_STRUCTURE_OPERATION);
                                    else if (!IsLValue(*exp) && (*tp)->BaseType()->type != BasicType::templateparam_)
                                        error(ERR_LVALUE);
                                    else
                                    {
                                        CheckThroughConstObject(nullptr, *exp);
                                        EXPRESSION* exp1 = nullptr;
                                        if ((*tp)->BaseType()->type == BasicType::pointer_)
                                        {
                                            Type* btp = (*tp)->BaseType()->btp;
                                            exp1 = nodeSizeof(btp, *exp);
                                        }
                                        else
                                        {
                                            if ((*tp)->IsVoid() || (*tp)->type == BasicType::aggregate_ || (*tp)->IsMsil())
                                                error(ERR_NOT_AN_ALLOWED_TYPE);
                                            if ((*tp)->BaseType()->scoped && !(flags & _F_SCOPEDENUM))
                                                error(ERR_SCOPED_TYPE_MISMATCH);
                                            if ((*tp)->BaseType()->type == BasicType::memberptr_)
                                                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                                            exp1 = MakeIntExpression(ExpressionNode::c_i_, 1);
                                        }
                                        if ((*tp)->BaseType()->type == BasicType::bool_)
                                        {
                                            /* Keyword::autoinc_ of a bool sets it true.  Keyword::autodec_ not allowed
                                             * these aren't spelled out in the C99 standard, we are
                                             * following the C++ standard here
                                             */
                                            if (kw == Keyword::autodec_)
                                                error(ERR_CANNOT_USE_bool_HERE);
                                            *exp = MakeExpression(ExpressionNode::assign_, *exp,
                                                                  MakeIntExpression(ExpressionNode::c_bool_, 1));
                                        }
                                        else
                                        {
                                            cast(*tp, &exp1);
                                            *exp = MakeExpression(kw == Keyword::autoinc_ ? ExpressionNode::auto_inc_
                                                                                          : ExpressionNode::auto_dec_,
                                                                  *exp, exp1);
                                        }
                                        while (TakeAddress(&exp1))
                                            ;
                                        if (exp1->type == ExpressionNode::auto_)
                                            exp1->v.sp->sb->altered = true;
                                    }
                                    break;
                                default:
                                    done = true;
                                    break;
                            }
                        }
                    }
                    else
                    {
                        LexemeStreamPosition  lastSym(currentStream);
                        expression_cast(funcsp, nullptr, &throwaway, exp, ismutable, flags);
                        if (throwaway)
                        {
                            if ((*exp)->type == ExpressionNode::pc_ ||
                                ((*exp)->type == ExpressionNode::callsite_ && !(*exp)->v.func->ascall))
                                thunkForImportTable(exp);
                            if ((*tp)->BaseType()->type == BasicType::string_)
                            {
                                if ((*exp)->type == ExpressionNode::labcon_ && (*exp)->string)
                                    (*exp)->type = ExpressionNode::c_string_;
                                else if (throwaway->BaseType()->type != BasicType::string_)
                                    *exp = MakeExpression(ExpressionNode::x_string_, *exp);
                            }
                            else if ((*tp)->BaseType()->type == BasicType::object_)
                            {
                                if (throwaway->BaseType()->type != BasicType::object_)
                                    if (!throwaway->IsStructured() && (!throwaway->IsArray() || !throwaway->BaseType()->msil))
                                        *exp = MakeExpression(ExpressionNode::x_object_, *exp);
                            }
                            else if ((throwaway->IsVoid() && !(*tp)->IsVoid()) || (*tp)->IsMsil())
                            {
                                error(ERR_NOT_AN_ALLOWED_TYPE);
                            }
                            else if (!Optimizer::cparams.prm_cplusplus &&
                                     ((throwaway->IsStructured() && !(*tp)->IsVoid()) ||
                                      throwaway->BaseType()->type == BasicType::memberptr_ ||
                                      (*tp)->BaseType()->type == BasicType::memberptr_) &&
                                     !throwaway->CompatibleType(*tp))
                            {
                                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                            }
                            else if (Optimizer::cparams.prm_cplusplus)
                            {
                                if (!doStaticCast(tp, throwaway, exp, funcsp, false) &&
                                    !doReinterpretCast(tp, throwaway, exp, funcsp, false))
                                {
                                    if (throwaway->IsStructured() || (*tp)->IsStructured())
                                    {
                                        errorConversionOrCast(false, throwaway, *tp);
                                    }
                                    cast(*tp, exp);
                                }
                            }
                            else if (!(*tp)->IsStructured() && (!(*tp)->IsArray() || !(*tp)->BaseType()->msil))
                            {
                                cast(*tp, exp);
                            }
                            else
                            {
                                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                            }
                        }
                        else
                        {
                            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                            *tp = &stdint;
                            lastSym.Backup();
                            error(ERR_EXPRESSION_SYNTAX);
                        }
                    }
                }
            }
            else  // expression in parenthesis
            {
                start.Backup();
                expression_unary(funcsp, atp, tp, exp, ismutable, flags);
            }
        }
        else
        {
            start.Backup();
            expression_unary(funcsp, atp, tp, exp, ismutable, flags);
        }
        if (loadedAttribs)
            basisAttribs = oldAttribs;
    }
    else
    {
        expression_unary(funcsp, atp, tp, exp, ismutable, flags);
    }
    return;
}
static void expression_pm( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    expression_cast(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while (MATCHKW(Keyword::dotstar_) || MATCHKW(Keyword::pointstar_))
    {
        bool points = false;
        LexemeStreamPosition placeHolder(currentStream);
        Keyword kw = KW();
        Type* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        expression_cast(funcsp, nullptr, &tp1, &exp1, nullptr, flags | _F_NOVARIADICFOLD);
        placeHolder.Replay([=]() {
            eval_binary_pm(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags);
        });
    }
    return;
}
static void expression_times( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    expression_pm(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while (MATCHKW(Keyword::star_) || MATCHKW(Keyword::divide_) || MATCHKW(Keyword::mod_))
    {
        LexemeStreamPosition placeHolder(currentStream);
        ExpressionNode type;
        Type* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        expression_pm(funcsp, nullptr, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return;
        }
        placeHolder.Replay([=]() {
            eval_binary_times(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags | _F_NOVARIADICFOLD);
        });
    }
    return;
}
static void expression_add( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    /* fixme add vlas */
    expression_times(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while (MATCHKW(Keyword::plus_) || MATCHKW(Keyword::minus_))
    {
        bool msil = false;
        LexemeStreamPosition placeHolder(currentStream);
        Type* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        expression_times(funcsp, atp, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return;
        }
        placeHolder.Replay([=]() {
            eval_binary_add(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags | _F_NOVARIADICFOLD);
        });
    }
    return;
}
static void expression_shift( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    expression_add(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while ((MATCHKW(Keyword::rightshift_) && !(flags & _F_INTEMPLATEPARAMS)) || MATCHKW(Keyword::leftshift_))
    {
        Type* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        ExpressionNode type;
        LexemeStreamPosition placeHolder(currentStream);
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        expression_add(funcsp, nullptr, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return;
        }
        placeHolder.Replay([=]() {
            eval_binary_shift(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags | _F_NOVARIADICFOLD);
        });
    }
    return;
}
static void expression_inequality( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable,
                                      int flags)
{
    bool done = false;
    expression_shift(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while (!done && currentLex)
    {
        Type* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        switch (KW())
        {
            case Keyword::gt_:
                done = (flags & _F_INTEMPLATEPARAMS);
                break;
            case Keyword::geq_:
            case Keyword::lt_:
            case Keyword::leq_:
                break;
            default:
                done = true;
                break;
        }
        if (!done)
        {
            LexemeStreamPosition placeHolder(currentStream);
            getsym();
            if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
            {
                if (!(flags & _F_EXPRESSIONINPAREN))
                {
                    getsym();
                    error(ERR_FOLDING_NEEDS_PARENTHESIS);
                }
                else if (flags & _F_NOVARIADICFOLD)
                {
                    getsym();
                    error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
                }
                else
                    --*currentStream;
                return;
            }
            expression_shift(funcsp, nullptr, &tp1, &exp1, nullptr, flags);
            if (!tp1)
            {
                *tp = nullptr;
                return;
            }
            placeHolder.Replay([=]() {
                eval_binary_inequality(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags | _F_NOVARIADICFOLD);
            });
        }
    }
    return;
}
static void expression_equality( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable,
                                    int flags)
{
    expression_inequality(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while (MATCHKW(Keyword::eq_) || MATCHKW(Keyword::neq_))
    {
        Type* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        LexemeStreamPosition placeHolder(currentStream);
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        expression_inequality(funcsp, nullptr, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return;
        }
        placeHolder.Replay([=]() {
            eval_binary_equality(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags | _F_NOVARIADICFOLD);
        });
    }
    return;
}
void GetLogicalDestructors(std::list<struct expr*>** rv, EXPRESSION* cur)
{

    if (!cur || cur->type == ExpressionNode::land_ || cur->type == ExpressionNode::lor_ || cur->type == ExpressionNode::hook_)
        return;
    // nots are genned with a branch, so, get past them...
    while (cur->type == ExpressionNode::not_)
        cur = cur->left;
    if (cur->type == ExpressionNode::callsite_)
    {
        if (cur->v.func->arguments)
            for (auto t : *cur->v.func->arguments)
                if (t->tp->IsRef())
                    GetLogicalDestructors(rv, t->exp);
        if (cur->v.func->thisptr)
            GetLogicalDestructors(rv, cur->v.func->thisptr);
        if (cur->v.func->returnSP)
        {
            SYMBOL* sym = cur->v.func->returnSP;
            if (!sym->sb->destructed && sym->sb->dest && sym->sb->dest->front()->exp && !sym->tp->BaseType()->sp->sb->trivialCons)
            {
                Optimizer::LIST* listitem;
                sym->sb->destructed = true;
                if (!*rv)
                    *rv = exprListFactory.CreateList();
                (*rv)->push_front(sym->sb->dest->front()->exp);
            }
        }
        else if (cur->v.func->sp && cur->v.func->sp->sb->isConstructor)
        {
            // it is going to be a local symbol if we get here...
            EXPRESSION* exp = cur->v.func->thisptr;
            int offset;
            SYMBOL* sym = nullptr;
            exp = relptr(exp, offset, true);
            if (exp)
                sym = exp->v.sp;
            if (sym && !sym->sb->destructed && sym->sb->dest && sym->sb->dest->front()->exp)
            {
                Optimizer::LIST* listitem;
                sym->sb->destructed = true;
                if (!*rv)
                    *rv = exprListFactory.CreateList();
                (*rv)->push_front(sym->sb->dest->front()->exp);
            }
        }
    }
    if (cur->left)
    {
        GetLogicalDestructors(rv, cur->left);
    }
    if (cur->right)
    {
        GetLogicalDestructors(rv, cur->right);
    }
}
void GetAssignDestructors(std::list<EXPRESSION*>** rv, EXPRESSION* exp)
{
    while (IsCastValue(exp) || IsLValue(exp) || exp->type == ExpressionNode::thisref_)
        exp = exp->left;
    if (exp->type == ExpressionNode::assign_ || exp->type == ExpressionNode::structadd_)
    {
        GetAssignDestructors(rv, exp->right);
        GetAssignDestructors(rv, exp->left);
    }
    else if (exp->type == ExpressionNode::callsite_ && exp->v.func->arguments)
    {
        for (auto t : *exp->v.func->arguments)
        {
            GetAssignDestructors(rv, t->exp);
            if (t->destructors)
            {
                if (!*rv)
                    *rv = exprListFactory.CreateList();
                (*rv)->insert((*rv)->end(), t->destructors->begin(), t->destructors->end());
                t->destructors = nullptr;
            }
        }
        if (exp->v.func->destructors && &exp->v.func->destructors != rv)
        {
            if (!*rv)
                *rv = exprListFactory.CreateList();
            (*rv)->insert((*rv)->end(), exp->v.func->destructors->begin(), exp->v.func->destructors->end());
            exp->v.func->destructors = nullptr;
        }
    }
}

static void  binop( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, Keyword kw,
                      void(nextFunc)( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable,
                                         int flags),
                      bool* ismutable, int flags)
{
    bool first = true;
    (*nextFunc)(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while (MATCHKW(kw))
    {
        Type* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        std::list<struct expr*>* logicaldestructorsleft = nullptr;
        std::list<struct expr*>* logicaldestructorsright = nullptr;
        if (first && (kw == Keyword::land_ || kw == Keyword::lor_))
        {
            first = false;
            GetLogicalDestructors(&logicaldestructorsleft, *exp);
            if (Optimizer::cparams.prm_cplusplus && *tp && (*tp)->IsStructured())
            {
                if (!castToArithmeticInternal(false, tp, exp, (Keyword)-1, &stdbool, false))
                    if (!castToArithmeticInternal(false, tp, exp, (Keyword)-1, &stdint, false))
                        if (!castToPointer(tp, exp, (Keyword)-1, &stdpointer))
                            errorConversionOrCast(true, *tp, &stdint);
            }
        }
        LexemeStreamPosition placeHolder(currentStream);
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        (*nextFunc)(funcsp, atp, &tp1, &exp1, nullptr, flags | _F_NOVARIADICFOLD);
        if (!tp1)
        {
            *tp = nullptr;
            break;
        }
        if (kw == Keyword::land_ || kw == Keyword::lor_)
        {
            GetLogicalDestructors(&logicaldestructorsright, exp1);
            if (Optimizer::cparams.prm_cplusplus && tp1 && tp1->IsStructured())
            {
                if (!castToArithmeticInternal(false, &tp1, &exp1, (Keyword)-1, &stdbool, false))
                    if (!castToArithmeticInternal(false, &tp1, &exp1, (Keyword)-1, &stdint, false))
                        if (!castToPointer(&tp1, &exp1, (Keyword)-1, &stdpointer))
                            errorConversionOrCast(true, tp1, &stdint);
            }
        }
        placeHolder.Replay([=]() {
            if (!eval_binary_logical(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags))
            {
                (*exp)->v.logicaldestructors.left = logicaldestructorsleft;
                (*exp)->v.logicaldestructors.right = logicaldestructorsright;
            }
        });
    }
    return;
}
static void expression_and( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(funcsp, atp, tp, exp, Keyword::and_, expression_equality, ismutable, flags);
}
static void expression_xor( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(funcsp, atp, tp, exp, Keyword::uparrow_, expression_and, ismutable, flags);
}
static void expression_or( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(funcsp, atp, tp, exp, Keyword::or_, expression_xor, ismutable, flags);
}
static void expression_land( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(funcsp, atp, tp, exp, Keyword::land_, expression_or, ismutable, flags);
}
static void expression_lor( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(funcsp, atp, tp, exp, Keyword::lor_, expression_land, ismutable, flags);
}

static void expression_hook( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    expression_lor(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    if (MATCHKW(Keyword::hook_))
    {
        Type *tph = nullptr, *tpc = nullptr;
        EXPRESSION *eph = nullptr, *epc = nullptr;
        if (Optimizer::cparams.prm_cplusplus && *tp && (*tp)->IsStructured())
        {
            if (!castToArithmeticInternal(false, tp, exp, (Keyword)-1, &stdbool, false))
                if (!castToArithmeticInternal(false, tp, exp, (Keyword)-1, &stdint, false))
                    if (!castToPointer(tp, exp, (Keyword)-1, &stdpointer))
                        errorConversionOrCast(true, *tp, &stdint);
        }
        std::list<EXPRESSION*>* logicaldestructors = nullptr;
        GetLogicalDestructors(&logicaldestructors, *exp);
        LookupSingleAggregate(*tp, exp);

        if ((*tp)->IsStructured())
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if ((*tp)->IsVoid() || (*tp)->IsMsil())
            error(ERR_NOT_AN_ALLOWED_TYPE);
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        isCallNoreturnFunction = false;
        if (MATCHKW(Keyword::colon_))
        {
            // replicate the selector into the 'true' value
            eph = AnonymousVar(StorageClass::auto_, *tp);
            Dereference(*tp, &eph);
            tph = *tp;
            *exp = MakeExpression(ExpressionNode::assign_, eph, *exp);
        }
        else
        {
            expression_comma(funcsp, nullptr, &tph, &eph, nullptr, flags | _F_NOVARIADICFOLD);
            tph = LookupSingleAggregate(tph, &eph);
        }
        bool oldCallExit = isCallNoreturnFunction;
        isCallNoreturnFunction = false;
        if (!tph)
        {
            *tp = nullptr;
        }
        else if (MATCHKW(Keyword::colon_))
        {
            getsym();
            expression_assign(funcsp, nullptr, &tpc, &epc, nullptr, flags | _F_NOVARIADICFOLD);
            isCallNoreturnFunction &= oldCallExit;
            tpc = LookupSingleAggregate(tpc, &epc);
            if (!tpc)
            {
                *tp = nullptr;
            }
            else
            {
                ResolveTemplateVariable(tp, exp, tph, atp);
                ResolveTemplateVariable(&tph, &eph, *tp, atp);
                ResolveTemplateVariable(tp, exp, tpc, atp);
                ResolveTemplateVariable(&tpc, &epc, *tp, atp);
                if ((*tp)->BaseType()->type == BasicType::memberptr_)
                {
                    *exp = MakeExpression(ExpressionNode::mp_as_bool_, *exp);
                    (*exp)->size = *tp;
                }
                if (tph->type == BasicType::void_)
                    tph = tpc;
                else if (tpc->type == BasicType::void_)
                    tpc = tph;
                if (tpc->lref && tph->lref)
                {
                    if (!tpc->CompatibleType(tph) && !SameTemplate(tpc, tph))
                    {
                        tpc->lref = 0;
                        tpc->rref = 0;
                    }
                }
                if (Optimizer::cparams.prm_cplusplus && (tpc->IsStructured() || tph->IsStructured()) && (!tpc->lref || !tph->lref))
                {
                    if (atp && !atp->IsStructured() && (!atp->IsRef() || !atp->BaseType()->btp->IsStructured()))
                    {
                        if (tpc->IsStructured())
                        {
                            if (!castToArithmeticInternal(true, &tpc, &epc, Keyword::plus_, atp, false))
                                errorConversionOrCast(true, tpc, atp);
                        }
                        if (tph->IsStructured())
                        {
                            if (!castToArithmeticInternal(true, &tph, &eph, Keyword::plus_, atp, false))
                                errorConversionOrCast(true, tph, atp);
                        }
                    }
                    else if (!tpc->IsStructured() || !tph->IsStructured() ||
                             (((!tpc->lref && !tpc->rref && !tph->lref && !tph->rref) ||
                               (tpc->lref != tph->lref && tpc->rref != tph->rref)) &&
                              ((tph->SameType(tpc) && !SameTemplate(tph, tpc, false)) || epc->type == ExpressionNode::thisref_ ||
                               epc->type == ExpressionNode::callsite_ || eph->type == ExpressionNode::thisref_ ||
                               eph->type == ExpressionNode::callsite_)))
                    {
                        // structure is result of constructor or return value
                        // at this point we want to check that both sides
                        // are the same type; if not make a constructor
                        EXPRESSION* rv = nullptr;
                        if (!tph->SameType(tpc))
                        {
                            if (atp && atp->IsStructured())
                            {
                                if (!atp->SameType(tpc) || !SameTemplate(atp, tpc))
                                {
                                    rv = AnonymousVar(StorageClass::auto_, tph);
                                    EXPRESSION* exp = rv;
                                    Type* ctype = atp;
                                    callConstructorParam(&ctype, &exp, tpc, epc, true, false, false, false, true);
                                    epc = exp;
                                    tpc = atp;
                                }

                                if (!atp->SameType(tph) || !SameTemplate(atp, tph))
                                {
                                    rv = AnonymousVar(StorageClass::auto_, tph);
                                    EXPRESSION* exp = rv;
                                    Type* ctype = atp;
                                    callConstructorParam(&ctype, &exp, tph, eph, true, false, false, false, true);
                                    eph = exp;
                                    tph = atp->CopyType();
                                    tph->lref = tph->rref = false;
                                }
                            }
                            else if (tph->IsStructured())
                            {
                                rv = AnonymousVar(StorageClass::auto_, tph);
                                EXPRESSION* exp = rv;
                                Type* ctype = tph;
                                callConstructorParam(&ctype, &exp, tpc, epc, true, false, false, false, true);
                                epc = exp;
                                tpc = tph;
                            }
                            else
                            {
                                rv = AnonymousVar(StorageClass::auto_, tpc);
                                EXPRESSION* exp = rv;
                                Type* ctype = tpc;
                                callConstructorParam(&ctype, &exp, tph, eph, true, false, false, false, true);
                                eph = exp;
                                tph = tpc;
                                tph->lref = tph->rref = false;
                            }
                        }
                        // now check for structured alias types
                        if (tpc->IsStructured() && tpc->BaseType()->sp->sb->structuredAliasType)
                        {
                            auto srp = tpc->BaseType()->sp->sb->structuredAliasType;
                            rv = AnonymousVar(StorageClass::auto_, srp);
                            epc = MakeExpression(ExpressionNode::blockassign_, rv, epc);
                            epc->size = tpc;
                            epc->altdata = (void*)(tpc);
                            eph = MakeExpression(ExpressionNode::blockassign_, rv, eph);
                            eph->size = tph;
                            eph->altdata = (void*)(tph);
                        }
                        else
                        {
                            // now make sure both sides are in an anonymous variable
                            if (tph->lref || tph->rref ||
                                (eph->type != ExpressionNode::callsite_ && eph->type != ExpressionNode::thisref_))
                            {
                                if (!rv)
                                    rv = AnonymousVar(StorageClass::auto_, tph);
                                EXPRESSION* exp = rv;
                                Type* ctype = tph;
                                callConstructorParam(&ctype, &exp, tph, eph, true, false, false, false, true);
                                eph = exp;
                            }
                            if (tpc->lref || tpc->rref ||
                                (epc->type != ExpressionNode::callsite_ && epc->type != ExpressionNode::thisref_))
                            {
                                // if (!rv)
                                rv = AnonymousVar(StorageClass::auto_, tph);
                                EXPRESSION* exp = rv;
                                Type* ctype = tpc;
                                callConstructorParam(&ctype, &exp, tpc, epc, true, false, false, false, true);
                                epc = exp;
                            }
                            // now make sure both sides are using the same anonymous variable
                            if (!rv)
                                rv = AnonymousVar(StorageClass::auto_, tph);
                            EXPRESSION* dexp = rv;
                            CallDestructor(tph->BaseType()->sp, nullptr, &dexp, nullptr, true, false, false, true);
                            InsertInitializer(&rv->v.sp->sb->dest, rv->v.sp->tp, dexp, 0, true);

                            EXPRESSION* exp = eph;
                            if (exp->type == ExpressionNode::thisref_)
                                exp = exp->left;
                            if (exp->v.func->returnSP)
                            {
                                if (exp->v.func->returnEXP && exp->v.func->returnEXP->type == ExpressionNode::auto_ &&
                                    exp->v.func->returnEXP->v.sp != rv->v.sp)
                                    exp->v.func->returnEXP->v.sp->sb->allocate = false;
                                exp->v.func->returnEXP = rv;
                            }
                            else if (exp->v.func->thisptr)
                            {
                                if (exp->v.func->thisptr->type == ExpressionNode::auto_ && exp->v.func->thisptr->v.sp != rv->v.sp)
                                    exp->v.func->thisptr->v.sp->sb->allocate = false;
                                exp->v.func->thisptr = rv;
                            }
                            exp = epc;
                            if (exp->type == ExpressionNode::thisref_)
                                exp = exp->left;
                            if (exp->v.func->returnSP)
                            {
                                if (exp->v.func->returnEXP && exp->v.func->returnEXP->type == ExpressionNode::auto_ &&
                                    exp->v.func->returnEXP->v.sp != rv->v.sp)
                                    exp->v.func->returnEXP->v.sp->sb->allocate = false;
                                exp->v.func->returnEXP = rv;
                            }
                            else if (exp->v.func->thisptr)
                            {
                                if (exp->v.func->thisptr->type == ExpressionNode::auto_ && exp->v.func->thisptr->v.sp != rv->v.sp)
                                    exp->v.func->thisptr->v.sp->sb->allocate = false;
                                exp->v.func->thisptr = rv;
                            }
                        }
                    }
                    else
                    {
                        tpc = tph;
                    }
                }
                if (tph->IsPtr() || tpc->IsPtr())
                    if (!tph->SameType(tpc))
                        if (!isconstzero(tph, eph) && !isconstzero(tpc, epc))
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                if (tph->IsFunction() || tpc->IsFunction())
                    if (!tph->CompatibleType(tpc))
                        if (!isconstzero(tph, eph) && !isconstzero(tpc, epc))
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                if (tph != tpc && (tph->IsPtr() || tph->IsFunction() || tpc->IsPtr() || tpc->IsFunction()))
                {
                    if (!tpc->CompatibleType(tph))
                    {
                        if ((tpc->IsVoidPtr() && tph->IsPtr()) || (tph->IsVoidPtr() && tpc->IsPtr()))
                        {
                            if (tph->nullptrType)
                                tph = tpc;
                            else if (tpc->nullptrType)
                                tpc = tph;
                            else
                                tpc = tph = &stdpointer;
                        }
                        else if (!((tph->IsPtr() || tph->IsFunction())) && !((tpc->IsPtr() || tpc->IsFunction())))
                        {
                            if (!tpc->SameType(tph))
                                if (!(tpc->IsArithmetic() && tph->IsArithmetic()))
                                    errortype(ERR_TWO_OPERANDS_SAME_TYPE, tpc, tph);
                        }
                        else if ((tph->IsFunction() || tpc->IsFunction()) && !tpc->CompatibleType(tph))
                            if (!tph->IsArithmetic() && !tpc->IsArithmetic())
                                errortype(ERR_TWO_OPERANDS_SAME_TYPE, tpc, tph);
                    }
                }
                if (tph->IsFunction())
                {
                    *tp = tph;
                }
                else if (tpc->IsFunction())
                {
                    *tp = tpc;
                }
                else if (tpc->lref && tph->lref)
                {
                    *tp = tpc;
                }
                else if (!tpc->IsVoid())
                {
                    if (tpc->scoped)
                    {
                        *tp = tpc;
                    }
                    else
                    {
                        if (!tpc->IsArithmetic() || !tph->IsArithmetic() || tpc->BaseType()->type != tph->BaseType()->type)
                            *tp = destSize(tpc, tph, &epc, &eph, false, nullptr);
                        else if (tph->IsConst())
                            *tp = tph;
                        else
                            *tp = tpc;
                    }
                }
                else
                {
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        EXPRESSION* exp1 = eph;
                        while (IsCastValue(exp1))
                            exp1 = exp1->left;
                        if (exp1->type == ExpressionNode::thisref_)
                            exp1 = exp1->left;
                        if (exp1->type == ExpressionNode::callsite_)
                            if (exp1->v.func->sp->tp->BaseType()->btp->IsVoid())
                                eph = MakeExpression(ExpressionNode::comma_, eph, MakeIntExpression(ExpressionNode::c_i_, 0));
                        exp1 = epc;
                        while (IsCastValue(exp1))
                            exp1 = exp1->left;
                        if (exp1->type == ExpressionNode::thisref_)
                            exp1 = exp1->left;
                        if (exp1->type == ExpressionNode::callsite_)
                            if (exp1->v.func->sp->tp->BaseType()->btp->IsVoid())
                                epc = MakeExpression(ExpressionNode::comma_, epc, MakeIntExpression(ExpressionNode::c_i_, 0));
                    }
                    *tp = tpc;
                }
                *exp = MakeExpression(ExpressionNode::hook_, *exp, MakeExpression(ExpressionNode::comma_, eph, epc));
                (*exp)->v.logicaldestructors.left = logicaldestructors;
                // when assigning a structure to itself, need an intermediate copy
                // this always puts it in...
                if (Optimizer::cparams.prm_cplusplus && (*tp)->IsStructured() && !(*tp)->lref && atp &&
                    !(*tp)->BaseType()->sp->sb->structuredAliasType)
                {
                    EXPRESSION* rv = AnonymousVar(StorageClass::auto_, *tp);
                    Type* ctype = *tp;
                    callConstructorParam(&ctype, &rv, *tp, *exp, true, false, false, false, true);
                    *exp = rv;
                }
            }
        }
        else
        {
            error(ERR_HOOK_NEEDS_COLON);
            *tp = nullptr;
        }
    }
    return;
}
void expression_throw( SYMBOL* funcsp, Type** tp, EXPRESSION** exp)
{
    Type* tp1 = nullptr;
    EXPRESSION* exp1 = nullptr;
    *tp = &stdvoid;
    hasXCInfo = true;
    getsym();
    functionCanThrow = true;
    if (!MATCHKW(Keyword::semicolon_))
    {
        SYMBOL* sym = namespacesearch("_ThrowException", globalNameSpace, false, false);
        makeXCTab(funcsp);
        expression_assign(funcsp, nullptr, &tp1, &exp1, nullptr, 0);
        if (!tp1)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else if (sym)
        {
            CallSite* params = Allocate<CallSite>();
            Argument* arg1 = Allocate<Argument>();  // exception table
            Argument* arg2 = Allocate<Argument>();  // instance
            Argument* arg3 = Allocate<Argument>();  // array size
            Argument* arg4 = Allocate<Argument>();  // constructor
            Argument* arg5 = Allocate<Argument>();  // exception block
            SYMBOL* rtti = RTTIDumpType(tp1, true);
            SYMBOL* cons = nullptr;
            if (tp1->IsStructured())
            {
                cons = getCopyCons(tp1->BaseType()->sp, false);
                if (cons && !cons->sb->inlineFunc.stmt)
                {
                    if (cons->sb->defaulted)
                        createConstructor(tp1->BaseType()->sp, cons);
                }
                if (cons && cons->sb->isExplicit)
                {
                    error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
                }
            }
            sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
            arg1->exp = MakeExpression(ExpressionNode::auto_, funcsp->sb->xc->xctab);
            arg1->tp = &stdpointer;
            if (tp1->IsStructured())
            {
                arg2->exp = exp1;
            }
            else
            {
                EXPRESSION* exp3 = AnonymousVar(StorageClass::auto_, tp1);
                arg2->exp = exp3;
                Dereference(tp1->type == BasicType::pointer_ ? &stdpointer : tp1, &exp3);
                exp3 = MakeExpression(ExpressionNode::assign_, exp3, exp1);
                arg2->exp = MakeExpression(ExpressionNode::comma_, exp3, arg2->exp);
            }
            arg2->tp = &stdpointer;
            arg3->exp = tp1->IsArray() ? MakeIntExpression(ExpressionNode::c_i_, tp1->size / (tp1->BaseType()->btp->size))
                                       : MakeIntExpression(ExpressionNode::c_i_, 1);
            arg3->tp = &stdint;
            arg4->exp = cons ? MakeExpression(ExpressionNode::pc_, cons) : MakeIntExpression(ExpressionNode::c_i_, 0);
            arg4->tp = &stdpointer;
            if (cons && cons->sb->attribs.inheritable.linkage2 == Linkage::import_)
            {
                arg4->exp = MakeExpression(ExpressionNode::l_p_, arg4->exp);
            }
            arg5->exp = rtti ? MakeExpression(ExpressionNode::global_, rtti) : MakeIntExpression(ExpressionNode::c_i_, 0);
            arg5->tp = &stdpointer;
            params->arguments = argumentListFactory.CreateList();
            params->arguments->push_back(arg1);
            params->arguments->push_back(arg2);
            params->arguments->push_back(arg3);
            params->arguments->push_back(arg4);
            params->arguments->push_back(arg5);
            params->ascall = true;
            params->sp = sym;
            params->functp = sym->tp;
            params->fcall = MakeExpression(ExpressionNode::pc_, sym);
            params->rttiType = rtti;
            *exp = MakeExpression(params);
        }
    }
    else
    {
        SYMBOL* sym = namespacesearch("_RethrowException", globalNameSpace, false, false);
        if (sym)
        {
            CallSite* parms = Allocate<CallSite>();
            Argument* arg1 = Allocate<Argument>();  // exception table
            makeXCTab(funcsp);
            sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
            parms->ascall = true;
            parms->sp = sym;
            parms->functp = sym->tp;
            parms->fcall = MakeExpression(ExpressionNode::pc_, sym);
            parms->arguments = argumentListFactory.CreateList();
            parms->arguments->push_back(arg1);
            arg1->exp = MakeExpression(ExpressionNode::auto_, funcsp->sb->xc->xctab);
            arg1->tp = &stdpointer;
            *exp = MakeExpression(parms);
        }
    }
    isCallNoreturnFunction = true;
    return;
}
static void ReplaceThisAssign(EXPRESSION** init, SYMBOL* sym, EXPRESSION* exp)
{
    if (*init)
    {
        if ((*init)->type == ExpressionNode::global_)
        {
            if ((*init)->v.sp == sym)
                *init = exp;
        }
        else
        {
            ReplaceThisAssign(&(*init)->left, sym, exp);
            ReplaceThisAssign(&(*init)->right, sym, exp);
        }
    }
}
void expression_assign( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    bool done = false;
    EXPRESSION *exp1 = nullptr, **exp2;
    EXPRESSION* asndest = nullptr;

    bool localMutable = false;
    Type* tp2;
    if (MATCHKW(Keyword::throw_))
    {
        return expression_throw(funcsp, tp, exp);
    }
    expression_hook(funcsp, atp, tp, exp, &localMutable, flags);
    if (*tp == nullptr)
        return;
    while (currentLex)
    {
        LexemeStreamPosition placeHolder(currentStream);
        Type* tp1 = nullptr;
        switch (KW())
        {
            case Keyword::assign_:
                getsym();
                if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
                {
                    if (!(flags & _F_EXPRESSIONINPAREN))
                    {
                        getsym();
                        error(ERR_FOLDING_NEEDS_PARENTHESIS);
                    }
                    else if (flags & _F_NOVARIADICFOLD)
                    {
                        getsym();
                        error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
                    }
                    else
                        --*currentStream;
                    return;
                }
                if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::begin_))
                {
                    if ((*tp)->IsStructured())
                    {
                        --*currentStream;
                        std::list<Initializer*>* init = nullptr;
                        SYMBOL* spinit = nullptr;
                        tp1 = *tp;
                        spinit = AnonymousVar(StorageClass::localstatic_, tp1)->v.sp;
                        localNameSpace->front()->syms->Add(spinit);
                        initType(funcsp, 0, StorageClass::auto_, &init, nullptr, tp1, spinit, false, false,
                                       flags | _F_ASSIGNINIT);
                        EXPRESSION* exp2 = nullptr;
                        if (init && init->front()->exp->type == ExpressionNode::thisref_)
                        {
                            ReplaceThisAssign(&init->front()->exp->left->v.func->thisptr, spinit, *exp);
                            spinit->sb->allocate = false;
                        }
                        else
                        {
                            exp2 = *exp;
                        }
                        *exp = ConverInitializersToExpression(tp1, nullptr, exp2, funcsp, init, exp1, false);

                        return;
                    }
                    else
                    {
                        ++assigningRHS;
                        expression_assign(funcsp, *tp, &tp1, &exp1, nullptr, flags | _F_NOVARIADICFOLD);
                        --assigningRHS;
                        if (!needkw(Keyword::end_))
                        {
                            errskim(skim_end);
                            skip(Keyword::end_);
                        }
                    }
                }
                else
                {
                    ++assigningRHS;
                    expression_assign(funcsp, *tp, &tp1, &exp1, nullptr, flags | _F_NOVARIADICFOLD);
                    --assigningRHS;
                }
                break;
            case Keyword::asplus_:
            case Keyword::asminus_:
            case Keyword::asand_:
            case Keyword::asor_:
            case Keyword::asxor_:
                getsym();
                if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
                {
                    if (!(flags & _F_EXPRESSIONINPAREN))
                    {
                        getsym();
                        error(ERR_FOLDING_NEEDS_PARENTHESIS);
                    }
                    else if (flags & _F_NOVARIADICFOLD)
                    {
                        getsym();
                        error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
                    }
                    else
                        --*currentStream;
                    return;
                }
                ++assigningRHS;
                expression_assign(funcsp, *tp, &tp1, &exp1, nullptr, flags | _F_NOVARIADICFOLD);
                --assigningRHS;
                break;
            case Keyword::asmod_:
            case Keyword::asleftshift_:
            case Keyword::asrightshift_:
            case Keyword::astimes_:
            case Keyword::asdivide_:
                getsym();
                if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
                {
                    if (!(flags & _F_EXPRESSIONINPAREN))
                    {
                        getsym();
                        error(ERR_FOLDING_NEEDS_PARENTHESIS);
                    }
                    else if (flags & _F_NOVARIADICFOLD)
                    {
                        getsym();
                        error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
                    }
                    else
                        --*currentStream;
                    return;
                }
                ++assigningRHS;
                expression_assign(funcsp, nullptr, &tp1, &exp1, nullptr, flags | _F_NOVARIADICFOLD);
                --assigningRHS;
                break;
            default:
                return;
        }
        if (!tp1)
        {
            *tp = nullptr;
            return;
        }
        if (!(*tp)->IsStructured())
            CheckThroughConstObject(nullptr, *exp);
        placeHolder.Replay([=]() {
            eval_binary_assign(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, localMutable, flags);
        });
    }
    return;
}
void expression_comma( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    expression_assign(funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return;
    while (MATCHKW(Keyword::comma_))
    {
        EXPRESSION* exp1 = nullptr;
        Type* tp1 = nullptr;
        LexemeStreamPosition placeHolder(currentStream);
        getsym();
        if (Optimizer::cparams.prm_cplusplus && MATCHKW(Keyword::ellipse_))
        {
            if (!(flags & _F_EXPRESSIONINPAREN))
            {
                getsym();
                error(ERR_FOLDING_NEEDS_PARENTHESIS);
            }
            else if (flags & _F_NOVARIADICFOLD)
            {
                getsym();
                error(ERR_FOLDING_NEEDS_SIMPLE_EXPRESSION);
            }
            else
                --*currentStream;
            return;
        }
        expression_assign(funcsp, atp, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            break;
        }
        placeHolder.Replay([=]() {
            eval_binary_comma(funcsp, atp, tp, exp, *tp, *exp, tp1, exp1, false, flags);
        });
    }
    return;
}
void expression_no_comma( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    expression_assign(funcsp, atp, tp, exp, ismutable, flags);
    assignmentUsages(*exp, false);
    return;
}
void expression_no_check( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags)
{
    if (flags & _F_TYPETEST)
    {
        anonymousNotAlloc++;
        expression_no_comma(funcsp, atp, tp, exp, nullptr, flags);
        anonymousNotAlloc--;
    }
    else
    {
        expression_comma(funcsp, atp, tp, exp, nullptr, flags);
    }
    return;
}

void expression( SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags)
{
    expression_comma(funcsp, atp, tp, exp, nullptr, flags);
    assignmentUsages(*exp, false);
    return;
}
}  // namespace Parser
