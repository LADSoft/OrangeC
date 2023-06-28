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
#include "template.h"
#include "declare.h"
#include "help.h"
#include "expr.h"
#include "cpplookup.h"
#include "occparse.h"
#include "lex.h"
#include "memory.h"
#include "init.h"
#include "exprcpp.h"
#include "template.h"
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
// there is a bug where the compiler needs constant values for the memory order,
// but parsed code may not provide it directly.
// e.g. when an atomic primitive is called from inside a function.
//
// we probably have to force inlining a little better to get around it, but would still need
// some kind of error in the back end if they do try to pass a non-constant value for a memory order.
//
// It isn't critical on the x86 so I'm putting in a temporary workaround for expediency
// this will have to be revisited when we do other architectures though.
//
#define NEED_CONST_MO

namespace Parser
{
int packIndex;

int argumentNesting;
int inAssignRHS;


std::list<SYMBOL*> importThunks;
/* lvaule */
/* handling of const int */
/*--------------------------------------------------------------------------------------------------------------------------------
 */
static EXPRESSION* nodeSizeof(TYPE* tp, EXPRESSION* exp, int flags = 0);
static LEXLIST* expression_primary(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable,
                                   int flags);
static LEXLIST* expression_pm(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
LEXLIST* expression_assign(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags);
static LEXLIST* expression_msilfunc(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags);

void expr_init(void)
{
    packIndex = -1;
    importThunks.clear();
    inGetUserConversion = 0;
    inSearchingFunctions = 0;
    inNothrowHandler = 0;
    argFriend = nullptr;
}
void thunkForImportTable(EXPRESSION** exp)
{
    SYMBOL* sym;
    if (Optimizer::architecture == ARCHITECTURE_MSIL)
        return;
    if ((*exp)->type == en_pc)
        sym = (*exp)->v.sp;
    else
        sym = (*exp)->v.func->sp;
    // order is important here as we might get into this function with an already sanitized symbol
    if (sym && sym->sb->attribs.inheritable.linkage2 == lk_import && isfunction(sym->tp) &&
        ((*exp)->type == en_pc || !(*exp)->v.func->ascall))
    {
        bool found = false;
        for (auto search : importThunks)
        {
            if (search->sb->mainsym == sym)
            {
                *exp = varNode(en_pc, (SYMBOL*)search);
                found = true;
                break;
            }
        }
        if (!found)
        {
            SYMBOL* newThunk;
            char buf[2048];
            Optimizer::my_sprintf(buf, "@@%s", sym->sb->decoratedName);
            newThunk = makeID(sc_global, &stdpointer, nullptr, litlate(buf));
            newThunk->sb->decoratedName = newThunk->name;
            newThunk->sb->mainsym = sym;  // mainsym is the symbol this was derived from
            newThunk->sb->attribs.inheritable.linkage4 = lk_virtual;
            newThunk->sb->importThunk = true;
            importThunks.push_back(newThunk);
            *exp = varNode(en_pc, newThunk);
            Optimizer::SymbolManager::Get(newThunk)->generated = true;
            Optimizer::EnterExternal(Optimizer::SymbolManager::Get(sym));
        }
    }
}
static EXPRESSION* GetUUIDData(SYMBOL* cls)
{
    if (cls && cls->sb->uuidLabel)
    {
        return intNode(en_labcon, cls->sb->uuidLabel);
    }
    else
    {
        EXPRESSION* rv;
        STRING* data = Allocate<STRING>();
        data->pointers = Allocate<Optimizer::SLCHAR*>();
        data->size = 1;
        data->strtype = l_astr;
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
static EXPRESSION* GetManagedFuncData(TYPE* tp)
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
    STRING* data = Allocate<STRING>();
    SYMBOL* sym;
    if (ispointer(tp))
        tp = basetype(tp)->btp;
    sym = basetype(tp)->sp;
    *save++ = 0;  // space for the number of dwords
    *save++ = 0;
    *save++ = sym->sb->attribs.inheritable.linkage == lk_stdcall;
    auto it = basetype(tp)->syms->begin();
    auto itend = basetype(tp)->syms->end();
    if (it != itend && (*it)->tp->type != bt_void)
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
    data->strtype = l_astr;
    data->pointers[0] = Allocate<Optimizer::SLCHAR>();
    data->pointers[0]->count = save - buf;
    data->pointers[0]->str = Allocate<LCHAR>(save - buf);
    for (i = 0; i < save - buf; i++)
        data->pointers[0]->str[i] = buf[i];
    return stringlit(data);
}
void ValidateMSILFuncPtr(TYPE* dest, TYPE* src, EXPRESSION** exp)
{
    bool managedDest = false;
    bool managedSrc = false;
    if ((*exp)->type == en_func && (*exp)->v.func->ascall)
        return;
    // this implementation marshals functions that are put into a pointer variable
    // declared with STDCALL.   If it is not declared with stdcall, then putting it
    // into a structure and passing it to unmanaged code will not work.
    if (isfunction(dest))
    {
        // function arg or assignment to function constant
        managedDest = basetype(dest)->sp->sb->attribs.inheritable.linkage2 != lk_unmanaged && msilManaged(basetype(dest)->sp);
    }
    else if (isfuncptr(dest))
    {
        // function var
        managedDest = basetype(basetype(dest)->btp)->sp->sb->attribs.inheritable.linkage2 != lk_unmanaged &&
                      msilManaged(basetype(basetype(dest)->btp)->sp);
    }
    else
    {
        // unknown
        errortype(ERR_CANNOT_CONVERT_TYPE, src, dest);
    }
    if (isfunction(src))
    {
        // function arg or assignment to function constant
        managedSrc = msilManaged(basetype(src)->sp);
    }
    else if (isfuncptr(src))
    {
        // function var
        managedSrc = msilManaged(basetype(basetype(src)->btp)->sp);
    }
    else
    {
        // unknown
        errortype(ERR_CANNOT_CONVERT_TYPE, src, dest);
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
                FUNCTIONCALL* functionCall = Allocate<FUNCTIONCALL>();
                TYPE* tp1 = src;
                if (ispointer(tp1))
                    tp1 = basetype(tp1)->btp;

                for (auto sp : *basetype(tp1)->syms)
                {
                    int m = sp->tp->size;
                    m += 3;
                    m /= 4;
                    n += m;
                }
                sym = (SYMBOL*)basetype(sym->tp)->syms->front();
                functionCall->sp = sym;
                functionCall->functp = sym->tp;
                functionCall->fcall = varNode(en_pc, sym);
                functionCall->arguments = initListListFactory.CreateList();
                auto arg = Allocate<INITLIST>();
                arg->tp = &stdpointer;
                arg->exp = *exp;
                functionCall->arguments->push_back(arg);
                arg = Allocate<INITLIST>();
                arg->tp = &stdpointer;
                arg->exp = GetManagedFuncData(tp1);
                functionCall->arguments->push_back(arg);
                functionCall->ascall = true;
                *exp = varNode(en_func, nullptr);
                (*exp)->v.func = functionCall;
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
                FUNCTIONCALL* functionCall = Allocate<FUNCTIONCALL>();
                sym = (SYMBOL*)basetype(sym->tp)->syms->front();
                functionCall->sp = sym;
                functionCall->functp = sym->tp;
                functionCall->fcall = varNode(en_pc, sym);
                functionCall->arguments = initListListFactory.CreateList();
                auto arg = Allocate<INITLIST>();
                arg->tp = &stdpointer;
                arg->exp = *exp;
                functionCall->arguments->push_back(arg);
                functionCall->ascall = true;
                *exp = varNode(en_func, nullptr);
                (*exp)->v.func = functionCall;
            }
            else
            {
                diag("ValidateMSILFuncPtr: missing conversion func definition");
            }
        }
    }
}
EXPRESSION* exprNode(enum e_node type, EXPRESSION* left, EXPRESSION* right)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = type;
    rv->left = left;
    rv->right = right;
    return rv;
}
EXPRESSION* varNode(enum e_node type, SYMBOL* sym)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = type;
    rv->v.sp = sym;
    return rv;
}
static bool inreg(EXPRESSION* exp, bool first)
{
    while (castvalue(exp))
        exp = exp->left;
    if (first && lvalue(exp))
    {
        exp = exp->left;
        first = false;
    }
    if (exp->type == en_auto)
    {
        return exp->v.sp->sb->storage_class == sc_register;
    }
    else if (exp->type == en_add || exp->type == en_arrayadd || exp->type == en_arraymul || exp->type == en_lsh ||
             exp->type == en_structadd || exp->type == en_arraylsh)
        return inreg(exp->left, first) || inreg(exp->right, first);
    else
        return false;
}
EXPRESSION* typeNode(TYPE* tp)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = en_type;
    rv->v.tp = tp;
    return rv;
}
EXPRESSION* intNode(enum e_node type, long long val)
{
    EXPRESSION* rv = Allocate<EXPRESSION>();
    rv->type = type;
    rv->v.i = val;
    return rv;
}
static bool isstructuredmath(TYPE* tp1, TYPE* tp2 = nullptr)
{
    if (Optimizer::cparams.prm_cplusplus)
    {
        if (isstructured(tp1) || basetype(tp1)->type == bt_enum || (isint(tp1) && tp1->scoped))
        {
            return true;
        }
        if (tp2 && (isstructured(tp2) || basetype(tp2)->type == bt_enum || (isint(tp2) && tp2->scoped)))
        {
            return true;
        }
    }
    return false;
}
void checkauto(TYPE* tp1, int err)
{
    if (isautotype(tp1))
    {
        error(err);
        while (tp1->type == bt_const || tp1->type == bt_volatile || tp1->type == bt_lrqual || tp1->type == bt_rrqual)
        {
            tp1->size = getSize(bt_int);
            tp1 = tp1->btp;
        }
        tp1->type = bt_int;
        tp1->size = getSize(bt_int);
    }
}
static void tagNonConst(SYMBOL* sym, TYPE* tp)
{
    if (sym && tp)
    {
        if (isref(tp))
            tp = basetype(tp)->btp;
        sym->sb->nonConstVariableUsed |= !isconst(tp);
    }
}

static LEXLIST* variableName(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    char idname[512];
    FUNCTIONCALL* funcparams = nullptr;
    SYMBOL* sym = nullptr;
    SYMBOL* strSym = nullptr;
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    LEXLIST* placeholder = lex;
    if (ismutable)
        *ismutable = false;
    if (Optimizer::cparams.prm_cplusplus ||
        ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions))
    {
        lex = id_expression(lex, funcsp, &sym, &strSym, &nsv, nullptr, false, false, idname);
    }
    else
    {
        sym = gsearch(lex->data->value.s.a);
    }
    if (sym)
    {
        browse_usage(sym, lex->data->linedata->fileindex);
        *tp = sym->tp;
        lex = getsym();
        if (sym->sb && sym->sb->attribs.uninheritable.deprecationText)
        {
            deprecateMessage(sym);
        }
        switch (sym->sb ? sym->sb->storage_class : sc_templateparam)
        {
            case sc_member:
            case sc_mutable:
                sym = lambda_capture(sym, cmThis, false);
                break;
            case sc_auto:
            case sc_register:
            case sc_parameter:
                sym = lambda_capture(sym, cmNone, false);
                break;
            case sc_constant:
                if ((flags & _F_AMPERSAND) && sym->sb->parent)
                    sym = lambda_capture(sym, cmNone, false);
                break;
            default:
                break;
        }
        if (sym->sb && sym->sb->templateLevel && !isfunction(sym->tp) && sym->sb->storage_class != sc_type && !ismember(sym) &&
            MATCHKW(lex, lt))
        {
            lex = getsym();
            if (startOfType(lex, nullptr, false))
            {
                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                SYMBOL* sp1 = sym;
                lex = backupsym();
                lex = GetTemplateArguments(lex, funcsp, sp1, &lst);
                sp1 = GetVariableTemplate(sp1, lst);

                if (sp1)
                {
                    sym = sp1;
                    *tp = sym->tp;
                }
                else
                    errorsym(ERR_NO_TEMPLATE_MATCHES, sym);
            }
            else
            {
                lex = backupsym();
            }
        }
        if ((!sym->sb || sym->sb->storage_class == sc_templateparam) && sym->tp->type == bt_templateparam &&
            (sym->tp->templateParam->second->type != kw_int ||
             (sym->tp->templateParam->second->type == kw_int && sym->tp->templateParam->second->byNonType.val)))
        {
            switch (sym->tp->templateParam->second->type)
            {
                case kw_typename:
                case kw_template:
                    lex = prevsym(placeholder);
                    *tp = nullptr;
                    if ((flags & (_F_SIZEOF | _F_PACKABLE)) == (_F_SIZEOF | _F_PACKABLE))
                    {
                        *exp = varNode(en_templateparam, sym);
                        *tp = sym->tp;
                        lex = getsym();
                        if (MATCHKW(lex, openpa))
                        {
                            lex = getsym();
                            errskim(&lex, skim_closepa, false);
                            lex = getsym();
                        }
                        else if (MATCHKW(lex, begin))
                        {
                            lex = getsym();
                            errskim(&lex, skim_end, false);
                            lex = getsym();
                        }
                    }
                    else
                    {
                        lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
                    }
                    return lex;
                case kw_int:
                    if (sym->tp->templateParam->second->packed)
                    {
                        if (packIndex >= 0)
                        {
                            if (sym->tp->templateParam->second->byPack.pack)
                            {
                                auto it = sym->tp->templateParam->second->byPack.pack->begin();
                                auto ite = sym->tp->templateParam->second->byPack.pack->end();
                                int n = packIndex;
                                for (; n && it != ite; n--, ++it);
                                if (it != ite)
                                {
                                    *exp = it->second->byNonType.val;
                                }
                                else
                                {
                                    *exp = intNode(en_c_i, 0);
                                }
                            }
                            else
                            {
                                *exp = intNode(en_c_i, 0);
                            }
                        }
                        else
                        {
                            *exp = exprNode(en_templateparam, nullptr, nullptr);
                            (*exp)->v.sp = sym;
                        }
                    }
                    else
                    {
                        *exp = sym->tp->templateParam->second->byNonType.val;
                    }
                    *tp = sym->tp->templateParam->second->byNonType.tp;
                    if ((*tp)->type == bt_templateparam)
                    {
                        TYPE* tp1 = (*tp)->templateParam->second->byClass.val;
                        if (tp1)
                        {
                            *tp = tp1;
                        }
                        else
                        {
                            SYMBOL* rv = nullptr;
                            for (auto&& s : structSyms)
                            {
                                if (rv)
                                    break;
                                if (s.tmpl)
                                    rv = templatesearch((*tp)->templateParam->first->name, s.tmpl);
                            }
                            if (rv && rv->tp->templateParam->second->type == kw_typename)
                                *tp = rv->tp->templateParam->second->byClass.val;
                        }
                    }
                    lex = expression_func_type_cast(lex, funcsp, tp, exp, flags | _F_NOEVAL);
                    if (!*exp)
                        *exp = intNode(en_c_i, 0);
                    return lex;
                default:
                    break;
            }
        }
        else
        {
            if (sym->tp->type == bt_templateparam)
            {

                if (((sym->sb && sym->sb->storage_class == sc_parameter) || sym->tp->templateParam->second->type == kw_int) &&
                    sym->tp->templateParam->second->packed)
                {
                    if (packIndex >= 0)
                    {
                        std::list<TEMPLATEPARAMPAIR>::iterator itt, ite = itt;
                        if (sym->tp->templateParam->second->byPack.pack)
                        {
                            itt = sym->tp->templateParam->second->byPack.pack->begin();
                            ite = sym->tp->templateParam->second->byPack.pack->end();
                            int i;
                            for (i = 0; i < packIndex && itt != ite; i++, ++itt);
                        }
                        if (itt != ite)
                        {
                            if (itt->second->type == kw_int && !itt->second->packsym)
                            {
                                *tp = itt->second->byNonType.tp;
                                if ((*tp)->type == bt_templateparam)
                                {
                                    if ((*tp)->templateParam->second->byClass.val)
                                    {
                                        *tp = (*tp)->templateParam->second->byClass.val;
                                    }
                                    else
                                    {
                                        *tp = sym->tp;
                                        if ((*tp)->type == bt_templateparam)
                                        {
                                            if ((*tp)->templateParam->second->byNonType.tp)
                                            {
                                                *tp = (*tp)->templateParam->second->byNonType.tp;
                                                if ((*tp)->type == bt_templateparam)
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
                                *exp = itt->second->byNonType.val;
                            }
                            else
                            {
                                sym = itt->second->packsym;
                                *tp = sym->tp;
                                *exp = varNode(en_auto, sym);
                            }
                        }
                        else
                        {
                            *exp = intNode(en_packedempty, 0);
                        }
                    }
                    else
                    {
                        sym->packed = true;
                        *exp = varNode(en_auto, sym);
                    }
                }
                else
                {
                    *exp = varNode(en_templateparam, sym);
                }
            }
            else if (sym->sb)
            {
                switch (sym->sb->storage_class)
                {
                    case sc_mutable:
                        if (ismutable)
                            *ismutable = true;
                    case sc_member:
                        /*
                        if (flags & _F_SIZEOF)
                        {
                            *exp = intNode(en_c_i, 0);
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
                    case sc_type:
                    case sc_typedef:
                        lex = prevsym(placeholder);
                        *tp = nullptr;
                        lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
                        if (!*exp)
                            *exp = intNode(en_c_i, 0);
                        return lex;
                    case sc_overloads:
                    {
                        funcparams = Allocate<FUNCTIONCALL>();
                        if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, lt))
                        {
                            lex = GetTemplateArguments(lex, funcsp, sym, &funcparams->templateParams);
                            funcparams->astemplate = true;
                        }
                        if (Optimizer::cparams.prm_cplusplus || basetype(sym->tp)->syms->size() > 1)
                        {
                            funcparams->nameSpace = strSym ? nullptr : nsv ? nsv->front() : nullptr;
                            funcparams->ascall = MATCHKW(lex, openpa);
                            funcparams->sp = sym;
                        }
                        else
                        {
                            // we only get here for C language, sadly we have to do
                            // argument based lookup in C++...
                            funcparams->sp = basetype(sym->tp)->syms->front();
                            funcparams->sp->sb->attribs.inheritable.used = true;
                            funcparams->fcall = varNode(en_pc, funcparams->sp);
                            if (!MATCHKW(lex, openpa))
                                if (funcparams->sp->sb->attribs.inheritable.isInline)
                                    InsertInline(funcparams->sp);
                        }
                        funcparams->functp = funcparams->sp->tp;
                        *tp = funcparams->sp->tp;
                        funcparams->asaddress = !!(flags & _F_AMPERSAND);
                        if (Optimizer::cparams.prm_cplusplus && ismember(basetype(*tp)->sp) && !MATCHKW(lex, openpa))
                        {
                            EXPRESSION* exp1 = Allocate<EXPRESSION>();
                            exp1->type = en_memberptr;
                            exp1->left = *exp;
                            exp1->v.sp = funcparams->sp;
                            *exp = exp1;
                            getMemberPtr(sym, strSym, tp, funcsp);
                        }
                        else
                        {
                            *exp = varNode(en_func, nullptr);
                            (*exp)->v.func = funcparams;
                        }
                        break;
                    }
                    case sc_catchvar:
                        makeXCTab(funcsp);
                        *exp = varNode(en_auto, funcsp->sb->xc->xctab);
                        *exp = exprNode(en_add, *exp, intNode(en_c_i, XCTAB_INSTANCE_OFS));
                        deref(&stdpointer, exp);
                        break;
                    case sc_enumconstant:
                        *exp = intNode(en_c_i, sym->sb->value.i);
                        break;
                    case sc_constant:
                        *exp = varNode(en_const, sym);
                        break;
                    case sc_auto:
                    case sc_register: /* register variables are treated as
                                       * auto variables in this compiler
                                       * of course the usage restraints of the
                                       * register keyword are enforced elsewhere
                                       */
                        *exp = varNode(en_auto, sym);
                        sym->sb->anyTry |= tryLevel != 0;
                        break;
                    case sc_parameter:
                        if (sym->packed)
                        {
                            if (!(flags & _F_PACKABLE))
                                error(ERR_PACK_SPECIFIER_MUST_BE_USED_IN_ARGUMENT);
                            if (packIndex >= 0)
                            {
                                TYPE* tp1 = sym->tp;
                                std::list<TEMPLATEPARAMPAIR>* templateParam;
                                int i;
                                while (ispointer(tp1) || isref(tp1))
                                    tp1 = basetype(tp1)->btp;
                                tp1 = basetype(tp1);
                                if (tp1->type == bt_templateparam)
                                {
                                    std::list<TEMPLATEPARAMPAIR>::iterator itt, ite = itt;
                                    if (sym->tp->templateParam->second->byPack.pack)
                                    {
                                        itt = sym->tp->templateParam->second->byPack.pack->begin();
                                        ite = sym->tp->templateParam->second->byPack.pack->end();
                                        int i;
                                        for (i = 0; i < packIndex && itt != ite; i++, ++itt);
                                    }
                                    if (itt != ite)
                                    {
                                        sym = itt->second->packsym;
                                        *tp = sym->tp;
                                        *exp = varNode(en_auto, sym);
                                    }
                                    else
                                    {
                                        *exp = intNode(en_packedempty, 0);
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
                                        for (i = 0; it != itend && i < packIndex; i++)
                                            ++it;
                                        if (it != itend)
                                        {
                                            sym = *it;
                                        }
                                        *exp = varNode(en_auto, sym);
                                        *tp = sym->tp;
                                    }
                                    else
                                    {
                                        *exp = intNode(en_packedempty, 0);
                                    }
                                }
                            }
                            else
                            {
                                *exp = varNode(en_auto, sym);
                            }
                        }
                        else
                        {
                            *exp = varNode(en_auto, sym);
                        }
                        /* derefereance parameters which are declared as arrays */
                        {
                            TYPE* tpa = basetype(sym->tp);
                            if (isref(tpa))
                                tpa = basetype(tpa->btp);
                            if (tpa->array)
                                deref(&stdpointer, exp);
                        }
                        sym->sb->anyTry |= tryLevel != 0;
                        break;

                    case sc_localstatic:
                        tagNonConst(funcsp, sym->tp);
                        if (funcsp && funcsp->sb->attribs.inheritable.isInline)
                        {
                            if (funcsp->sb->promotedToInline || Optimizer::cparams.prm_cplusplus)
                            {
                                funcsp->sb->attribs.inheritable.isInline =
                                    funcsp->sb->promotedToInline = false;
                            }
                        }
                        if (sym->sb->attribs.inheritable.linkage3 == lk_threadlocal)
                        {
                            funcsp->sb->nonConstVariableUsed = true;
                            *exp = varNode(en_threadlocal, sym);
                        }
                        else
                        {
                            *exp = varNode(en_global, sym);
                        }
                        sym->sb->attribs.inheritable.used = true;
                        break;
                    case sc_absolute:
                        funcsp->sb->nonConstVariableUsed = true;
                        *exp = varNode(en_absolute, sym);
                        break;
                    case sc_static:
                        sym->sb->attribs.inheritable.used = true;
                    case sc_global:
                    case sc_external: {
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
                        if (sym->sb->attribs.inheritable.linkage3 == lk_threadlocal)
                            *exp = varNode(en_threadlocal, sym);
                        else
                            *exp = varNode(en_global, sym);
                        if (sym->sb->attribs.inheritable.linkage2 == lk_import)
                        {
                            deref(&stdpointer, exp);
                        }
                        break;
                    }
                    case sc_namespace:
                    case sc_namespacealias:
                        errorsym(ERR_INVALID_USE_OF_NAMESPACE, sym);
                        *exp = intNode(en_c_i, 1);
                        break;
                    default:
                        error(ERR_IDENTIFIER_EXPECTED);
                        *exp = intNode(en_c_i, 1);
                        break;
                }
            }
            else
            {
                if (!templateNestingCount)
                    error(ERR_IDENTIFIER_EXPECTED);
                *exp = intNode(en_c_i, 1);
            }
        }
        sym->tp->used = true;
        if (sym->sb && sym->sb->templateLevel && istype(sym))
        {
            lex = prevsym(placeholder);
            *tp = nullptr;
            lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
        }
        else
        {
            if (sym->tp->type == bt_templateparam)
            {
                if (*exp && (*exp)->type != en_packedempty && !sym->tp->templateParam->second->packed)
                {
                    if (sym->tp->templateParam->second->type == kw_int)
                        *tp = sym->tp->templateParam->second->byNonType.tp;
                    else
                        *tp = &stdint;
                    if (sym->sb && sym->sb->templateLevel && sym->sb->storage_class != sc_type && !ismember(sym) &&
                        !isfunction(sym->tp))
                    {
                        if (!(flags & _F_AMPERSAND))
                            deref(&stdint, exp);
                    }
                    else if (!templateNestingCount)
                    {
                        *exp = intNode(en_c_i, 0);
                    }
                    if (MATCHKW(lex, openpa) || MATCHKW(lex, begin))
                    {
                        lex = prevsym(placeholder);
                        TYPE* tp1 = nullptr;
                        EXPRESSION* exp1;
                        lex = expression_func_type_cast(lex, funcsp, &tp1, &exp1, flags);
                        return lex;
                    }
                }
            }
            else if (sym->tp->type == bt_any)
                deref(&stdint, exp);
            else
            {
                if (isref(*tp))
                {
                    TYPE* tp1 = *tp;
                    deref(*tp, exp);
                    while (isref(tp1))
                        tp1 = basetype(tp1)->btp;
                    *tp = CopyType(tp1);
                    UpdateRootTypes(*tp);
                    basetype(*tp)->lref = false;
                    basetype(*tp)->rref = false;
                }
                if (sym->sb && sym->sb->storage_class != sc_overloads)
                {
                    if (!isstructured(*tp) && basetype(*tp)->type != bt_memberptr && !isfunction(*tp) &&
                        sym->sb->storage_class != sc_constant && sym->sb->storage_class != sc_enumconstant &&
                        sym->tp->type != bt_void)
                    {
                        if (!(*tp)->array || (*tp)->vla || (!(*tp)->msil && sym->sb->storage_class == sc_parameter))
                            if ((*tp)->vla)
                                deref(&stdpointer, exp);
                            else
                                deref(*tp, exp);
                        else if ((*tp)->array && inreg(*exp, true))
                            error(ERR_CANNOT_TAKE_ADDRESS_OF_REGISTER);
                    }
                }
            }
            *tp = ResolveTemplateSelectors(sym, *tp);

            if (lvalue(*exp) && (*exp)->type != en_l_object)
                (*exp)->v.sp = sym;  // catch for constexpr
            (*exp)->pragmas = preProcessor->GetStdPragmas();
            if (isvolatile(*tp))
                (*exp)->isvolatile = true;
            if (isrestrict(*tp))
                (*exp)->isrestrict = true;
            if (isatomic(*tp))
            {
                (*exp)->isatomic = true;
                if (needsAtomicLockFromType(*tp))
                    (*exp)->lockOffset = basetype(*tp)->size;
            }
            if (strSym && funcparams)
                funcparams->novtab = true;
            if (Optimizer::cparams.prm_cplusplus && sym->sb &&
                (sym->sb->storage_class == sc_member || sym->sb->storage_class == sc_mutable))
            {
                qualifyForFunc(funcsp, tp, *ismutable);
            }
        }
    }
    else if (parsingPreprocessorConstant)
    {
        *tp = &stdint;
        *exp = intNode(en_c_i, 0);
        lex = getsym();
    }
    else
    {
        char* name;
        if (strSym && strSym->tp->type == bt_templateselector)
        {
            SYMBOL* sym = (*basetype(strSym->tp)->sp->sb->templateSelector)[1].sp;
            if ((!templateNestingCount || instantiatingTemplate) && (sym->sb && sym->sb->instantiated && !declaringTemplate(sym) && (!sym->sb->templateLevel || allTemplateArgsSpecified(sym, (*strSym->tp->sp->sb->templateSelector)[1].templateParams))))
            {
                errorNotMember(sym, nsv ? nsv->front() : nullptr, ISID(lex) ? lex->data->value.s.a : "__unknown");
            }
            *exp = exprNode(en_templateselector, nullptr, nullptr);
            (*exp)->v.templateSelector = strSym->tp->sp->sb->templateSelector;
            *tp = strSym->tp;
            lex = getsym();
            if (MATCHKW(lex, lt))
            {
                std::list<TEMPLATEPARAMPAIR>* current = nullptr;
                lex = GetTemplateArguments(lex, funcsp, nullptr, &current);
            }
            return lex;
        }
        if (strSym && basetype(strSym->tp)->type == bt_templatedecltype)
        {
            *tp = strSym->tp;
            *exp = intNode(en_c_i, 0);
            lex = getsym();
            return lex;
        }
        if (ISID(lex))
            name = litlate(lex->data->value.s.a);
        else
            name = litlate("__unknown");
        sym = SymAlloc();
        sym->name = name;
        sym->sb->attribs.inheritable.used = true;
        sym->sb->declfile = sym->sb->origdeclfile = lex->data->errfile;
        sym->sb->declline = sym->sb->origdeclline = lex->data->errline;
        sym->sb->realdeclline = lex->data->linedata->lineno;
        sym->sb->declfilenum = lex->data->linedata->fileindex;
        lex = getsym();
        if (MATCHKW(lex, openpa))
        {
            if (Optimizer::cparams.prm_cplusplus)
            {
                sym->sb->storage_class = sc_overloads;
                (*tp) = MakeType(bt_aggregate);
                UpdateRootTypes(*tp);
                (*tp)->sp = sym;
            }
            else
            {
                /* no prototype error will be added later */
                sym->sb->storage_class = sc_external;
                sym->sb->attribs.inheritable.linkage = lk_c;
                sym->tp = MakeType(bt_func, CopyType(&stdint));
                sym->tp->syms = symbols.CreateSymbolTable();
                sym->tp->sp = sym;
                sym->sb->oldstyle = true;
                SetLinkerNames(sym, lk_c);
                *tp = sym->tp;
            }
            SetLinkerNames(sym, lk_c);
            if (sym->sb->storage_class != sc_overloads)
            {
                InsertSymbol(sym, sym->sb->storage_class, lk_none, false);
                *exp = varNode(sym->sb->storage_class == sc_auto ? en_auto : en_global, sym);
            }
            else
            {
                sym->tp = MakeType(bt_func, MakeType(bt_int));
                sym->tp->syms = symbols.CreateSymbolTable();
                sym->tp->sp = sym;
                sym->sb->oldstyle = true;
                sym->sb->externShim = !!(flags & _F_INDECLTYPE);
                funcparams = Allocate<FUNCTIONCALL>();
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = varNode(en_pc, sym);
                funcparams->nameSpace = strSym ? nullptr : nsv ? nsv->front() : nullptr;
                *tp = sym->tp;
                *exp = varNode(en_func, nullptr);
                (*exp)->v.func = funcparams;
            }
        }
        else
        {
            if (Optimizer::cparams.prm_cplusplus && atp && isfuncptr(atp))
            {
                SYMBOL* sym = nullptr;
                // this code was written this way before I fixed the unitialized variable.   Did we really never get here?
                sym->sb->storage_class = sc_overloads;
                (*tp) = MakeType(bt_aggregate);
                UpdateRootTypes(*tp);
                (*tp)->sp = sym;
                funcparams = Allocate<FUNCTIONCALL>();
                funcparams->ascall = true;
                sym = GetOverloadedFunction(tp, &funcparams->fcall, sym, nullptr, atp, true, false, flags);
                if (sym)
                {
                    sym->sb->throughClass = sym->sb->throughClass;
                    sym = sym;
                    if (!isExpressionAccessible(nullptr, sym, funcsp, nullptr, false))
                        errorsym(ERR_CANNOT_ACCESS, sym);
                }
                funcparams->sp = sym;
                funcparams->functp = funcparams->sp->tp;
                funcparams->nameSpace = nsv->front();
                *exp = varNode(en_func, nullptr);
                (*exp)->v.func = funcparams;
            }
            else
            {
                sym->sb->storage_class = funcsp ? sc_auto : sc_global;
                sym->tp = MakeType(bt_any);
                sym->sb->parentClass = strSym;
                *tp = sym->tp;
                deref(&stdint, exp);
                SetLinkerNames(sym, lk_c);
                if (!nsv && (!strSym || !templateNestingCount ||
                             (!strSym->sb->templateLevel && strSym->tp->type != bt_templateselector &&
                              strSym->tp->type != bt_templatedecltype)))
                {
                    char buf[4000];
                    buf[0] = 0;
                    LEXLIST* find = placeholder;
                    while (lex != find)
                    {
                        if (ISKW(find))
                        {
                            strcat(buf, find->data->kw->name);
                        }
                        else if (ISID(find))
                        {
                            strcat(buf, find->data->value.s.a);
                        }
                        find = find->next;
                    }
                    if (!(flags & _F_MEMBER))
                    {
                        errorstr(ERR_UNDEFINED_IDENTIFIER, buf);
                    }
                    if (sym->sb->storage_class != sc_overloads &&
                        (localNameSpace->front()->syms || sym->sb->storage_class != sc_auto))
                        InsertSymbol(sym, sym->sb->storage_class, lk_none, false);
                }
                if (nsv)
                {
                    errorNotMember(strSym, nsv->front(), sym->name);
                }
                if (sym->sb->storage_class != sc_overloads)
                {
                    *exp = varNode(en_global, sym);
                }
                else
                {
                    funcparams = Allocate<FUNCTIONCALL>();
                    funcparams->nameSpace = nsv->front();
                    *exp = varNode(en_func, nullptr);
                    (*exp)->v.func = funcparams;
                }
            }
        }
    }
    if (!*exp)
        *exp = intNode(en_c_i, 0);
    if (sym && sym->sb)
    {
        sym->sb->allocate = true;
    }
    return lex;
}
static LEXLIST* expression_member(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    TYPE *typein = *tp, *typein2 = isarray(typein) ? typein : nullptr;
    bool points = false;
    const char* tokenName = ".";
    if (ISKW(lex))
        tokenName = lex->data->kw->name;
    (void)funcsp;
    // find structured version of arithmetic types for msil member matching
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions &&
        (isarithmetic(*tp) || (*tp)->type == bt___string || (isarray(*tp) && basetype(*tp)->msil)))
    {
        // auto-boxing for msil
        TYPE* tp1 = find_boxed_type(basetype(*tp));
        if (tp1)
        {
            while (castvalue(*exp))
                *exp = (*exp)->left;
            if (!lvalue(*exp))
                *exp = msilCreateTemporary(*tp, *exp);
            else
                *exp = (*exp)->left;
            *tp = tp1;
            typein = tp1;
        }
    }
    if (MATCHKW(lex, pointsto))
    {
        TYPE* nesting[100];
        int n = 0;
        points = true;
        do
        {
            if (isstructured(*tp))
            {
                TYPE* x = basetype(*tp);
                int i;
                for (i = 0; i < n; i++)
                    if (nesting[i] == x)
                    {
                        break;
                    }
                nesting[n++] = x;
                if (n >= sizeof(nesting) / sizeof(nesting[0]))
                {
                    break;
                }
            }
        } while ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) &&
                 insertOperatorFunc(ovcl_pointsto, pointsto, funcsp, tp, exp, nullptr, nullptr, nullptr, flags));
        typein = *tp;
        if (ispointer(*tp))
        {
            *tp = basetype(*tp);
            *tp = (*tp)->btp;
        }
    }
    *tp = PerformDeferredInitialization(*tp, funcsp);
    if (MATCHKW(lex, dot) || MATCHKW(lex, pointsto))
        lex = getsym();
    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, complx))
    {
        // direct destructor or psuedo-destructor
        enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
        bool defd = false;
        bool notype = false;
        TYPE* tp1 = nullptr;
        lex = getsym();
        lex = getBasicType(lex, funcsp, &tp1, nullptr, false, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype, &defd,
                           nullptr, nullptr, false, true, false, false, false);
        if (!tp1)
        {
            error(ERR_TYPE_NAME_EXPECTED);
        }
        else if (!comparetypes(*tp, tp1, true))
        {
            if (!templateNestingCount)
                error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
        }
        else if (isstructured(*tp))
        {
            // destructor
            SYMBOL* sp2 = search((basetype(*tp)->sp)->tp->syms, overloadNameTab[CI_DESTRUCTOR]);
            if (sp2)
            {
                if (flags && _F_IS_NOTHROW)
                    inNothrowHandler++;
                callDestructor(basetype(*tp)->sp, nullptr, exp, nullptr, true, false, false, !points);
                if (flags && _F_IS_NOTHROW)
                    inNothrowHandler--;
            }
            if (needkw(&lex, openpa))
                needkw(&lex, closepa);
        }
        else
        {
            // psuedo-destructor, no further activity required.
            if (needkw(&lex, openpa))
                needkw(&lex, closepa);
        }
        *tp = &stdvoid;
    }
    else if (!isstructured(*tp) || (points && !ispointer(typein)) ||
             (parsingTrailingReturnOrUsing && isstructured(*tp) && templateNestingCount && !instantiatingTemplate &&
              basetype(*tp)->sp->sb->templateLevel))
    {
        if (Optimizer::cparams.prm_cplusplus && ISKW(lex) && (lex->data->kw->tokenTypes & TT_BASETYPE))
        {
            // possible psuedo destructor with selector
            enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
            bool defd = false;
            bool notype = false;
            TYPE* tp1 = nullptr;
            lex = getBasicType(lex, funcsp, &tp1, nullptr, false, sc_auto, &linkage, &linkage2, &linkage3, ac_public, &notype,
                               &defd, nullptr, nullptr, false, true, false, false, false);
            if (!tp1)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else if (!comparetypes(*tp, tp1, true))
            {
                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
            }
            if (!MATCHKW(lex, classsel))
            {
                error(ERR_INVALID_PSUEDO_DESTRUCTOR);
            }
            else
            {
                lex = getsym();
                if (!MATCHKW(lex, complx))
                {
                    error(ERR_INVALID_PSUEDO_DESTRUCTOR);
                }
                else
                {
                    lex = getsym();
                    tp1 = nullptr;
                    lex = getBasicType(lex, funcsp, &tp1, nullptr, false, sc_auto, &linkage, &linkage2, &linkage3, ac_public,
                                       &notype, &defd, nullptr, nullptr, false, true, false, false, false);
                    if (!tp1)
                    {
                        error(ERR_TYPE_NAME_EXPECTED);
                    }
                    else if (!comparetypes(*tp, tp1, true))
                    {
                        error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                    }
                    else if (needkw(&lex, openpa))
                        needkw(&lex, closepa);
                }
            }
            *tp = &stdvoid;
        }
        else if (templateNestingCount && Optimizer::cparams.prm_cplusplus)
        {
            *exp = exprNode(points ? en_pointsto : en_dot, *exp, nullptr);
            EXPRESSION** ptr = &(*exp)->right;
            while (ISID(lex) || MATCHKW(lex, kw_operator))
            {
                TYPE* tp2 = nullptr;
                EXPRESSION* exp = nullptr;
                lex = expression_pm(lex, funcsp, nullptr, &tp2, ptr, nullptr, _F_MEMBER);
                if (!MATCHKW(lex, pointsto) && !MATCHKW(lex, dot))
                    break;
                *ptr = exprNode(MATCHKW(lex, pointsto) ? en_pointsto : en_dot, *ptr, nullptr);
                ptr = &(*ptr)->right;
                lex = getsym();
            }
            if (!*ptr)
            {
                *ptr = intNode(en_c_i, 0);
            }
            else
            {
                *tp = MakeType(bt_templatedecltype);
                (*tp)->templateDeclType = *exp;
            }
        }
        else
        {
            if (points)
                errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, tokenName);
            else
                errorstr(ERR_STRUCTURED_TYPE_EXPECTED, tokenName);
            while (ISID(lex) || MATCHKW(lex, kw_operator))
            {
                TYPE* tp = nullptr;
                EXPRESSION* exp = nullptr;
                lex = expression_pm(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
                if (!MATCHKW(lex, pointsto) && !MATCHKW(lex, dot))
                    break;
                lex = getsym();
            }
        }
    }
    else
    {
        bool isTemplate = false;
        if (MATCHKW(lex, kw_template))
        {
            lex = getsym();
            isTemplate = true;
        }
        if (!ISID(lex) && !MATCHKW(lex, kw_operator))
        {
            error(ERR_IDENTIFIER_EXPECTED);
        }
        else
        {
            SYMBOL* sp2 = nullptr;
            if ((*exp)->type == en_literalclass)
            {
                CONSTEXPRSYM* ces = (CONSTEXPRSYM*)search((*exp)->v.syms, lex->data->value.s.a);
                if (ces)
                {
                    lex = getsym();
                    *exp = ces->exp;
                    *tp = ces->sp->tp;
                    return lex;
                }
                else
                {
                    STRUCTSYM l;
                    l.str = basetype(*tp)->sp;
                    addStructureDeclaration(&l);
                    lex = id_expression(lex, funcsp, &sp2, nullptr, nullptr, &isTemplate, false, true, nullptr);
                    dropStructureDeclaration();
                    if (!sp2)
                    {
                        errorNotMember(basetype(*tp)->sp, nullptr, lex->data->value.s.a);
                        lex = getsym();
                        while (ISID(lex))
                        {
                            lex = getsym();
                            if (!MATCHKW(lex, pointsto) && !MATCHKW(lex, dot))
                                break;
                            lex = getsym();
                        }
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        return lex;
                    }
                    lex = getsym();
                    if (sp2->tp->type != bt_aggregate)
                    {
                        error(ERR_CONSTANT_FUNCTION_EXPECTED);
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        return lex;
                    }
                    if (MATCHKW(lex, openpa))
                    {
                        FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                        SYMBOL* match;
                        TYPE* tp1 = nullptr;
                        EXPRESSION* exp1 = nullptr;
                        lex = getArgs(lex, funcsp, funcparams, closepa, true, flags);
                        funcparams->thisptr = intNode(en_c_i, 0);
                        funcparams->thistp = MakeType(bt_pointer, *tp);
                        funcparams->ascall = true;
                        match = GetOverloadedFunction(&tp1, &exp1, sp2, funcparams, nullptr, true, false, flags);
                        if (match)
                        {
                            funcparams->sp = match;
                            funcparams->functp = match->tp;
                            *exp = substitute_params_for_function(funcparams, (*exp)->v.syms);
                            optimize_for_constants(exp);
                            *tp = basetype(match->tp)->btp;
                            if (!match->sb->constexpression || !IsConstantExpression(*exp, true, false))
                                error(ERR_CONSTANT_FUNCTION_EXPECTED);
                        }
                        else
                        {
                            *exp = intNode(en_c_i, 0);
                            *tp = &stdint;
                        }
                        return lex;
                    }
                    else
                    {
                        needkw(&lex, openpa);
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        return lex;
                    }
                }
            }
            if (Optimizer::cparams.prm_cplusplus)
            {
                STRUCTSYM l;
                l.str = basetype(*tp)->sp;
                addStructureDeclaration(&l);
                lex = id_expression(lex, funcsp, &sp2, nullptr, nullptr, &isTemplate, false, true, nullptr);
                dropStructureDeclaration();
            }
            else
            {
                sp2 = search((basetype(*tp)->sp)->tp->syms, lex->data->value.s.a);
            }
            if (!sp2)
            {
                if (!templateNestingCount || !basetype(*tp)->sp->sb->templateLevel)
                    errorNotMember(basetype(*tp)->sp, nullptr, lex->data->value.s.a ? lex->data->value.s.a : "unknown");
                lex = getsym();
                while (ISID(lex))
                {
                    lex = getsym();
                    if (!MATCHKW(lex, pointsto) && !MATCHKW(lex, dot))
                        break;
                    lex = getsym();
                }
            }
            else
            {
                TYPE* tpb;
                TYPE* basetp = *tp;
                TYPE* typ2 = typein;
                if (sp2->sb->attribs.uninheritable.deprecationText)
                    deprecateMessage(sp2);
                browse_usage(sp2, lex->data->linedata->fileindex);
                if (ispointer(typ2))
                    typ2 = basetype(typ2)->btp;
                (*exp)->isatomic = false;
                lex = getsym();
                sp2->sb->attribs.inheritable.used = true;
                *tp = sp2->tp;
                while ((*tp)->type == bt_typedef)
                    (*tp) = (*tp)->btp;
                tpb = basetype(*tp);
                if (sp2->sb->storage_class == sc_overloads)
                {
                    FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, lt))
                    {
                        bool isdest = basetype(sp2->tp)->syms->front()->sb->isDestructor;
                        bool found = false;
                        for (auto sp : *basetype(sp2->tp)->syms)
                        {
                            if (sp->sb->templateLevel)
                            {
                                found = true;
                                break;
                            }
                        }
                        if (found)
                        {
                            lex = GetTemplateArguments(lex, funcsp, nullptr, &funcparams->templateParams);
                            funcparams->astemplate = true;
                        }
                        else if (isdest && sp2->sb->parentClass->sb->templateLevel)
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            lex = GetTemplateArguments(lex, funcsp, nullptr, &lst);
                            if (!exactMatchOnTemplateArgs(lst, sp2->sb->parentClass->templateParams->front().second->bySpecialization.types))
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
                    funcparams->thistp = MakeType(bt_pointer, basetp);

                    if (!points && (*exp)->type != en_l_ref && !typein->lref && !typein->rref)
                        funcparams->novtab = true;
                    *exp = varNode(en_func, nullptr);
                    (*exp)->v.func = funcparams;
                }
                else
                {
                    SYMBOL* sp3 = sp2->sb->parentClass;
                    if (ismutable && sp2->sb->storage_class == sc_mutable)
                        *ismutable = true;
                    if (sp2->sb->templateLevel)
                    {
                        if (!sp2->sb->instantiated)
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            SYMBOL* sp1 = sp2;
                            if (MATCHKW(lex, lt))
                            {
                                lex = GetTemplateArguments(lex, funcsp, sp2, &lst);
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
                    if (sp2->sb->storage_class == sc_external || sp2->sb->storage_class == sc_static)
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
                        enum e_ac access = ac_public;
                        SYMBOL* ssp = getStructureDeclaration();
                        while (ssp)
                        {
                            if (ssp == basetype(typ2)->sp)
                                access = ac_protected;
                            ssp = ssp->sb->parentClass;
                        }
                        ssp = getStructureDeclaration();
                        if (!isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, sp2, funcsp, *exp, access))
                        {
                            errorsym(ERR_CANNOT_ACCESS, sp2);
                        }
                    }
                    bool structuredAlias = false;
                    if (sp2->sb->storage_class == sc_constant)
                    {
                        *exp = varNode(en_const, sp2);
                    }
                    else if (sp2->sb->storage_class == sc_enumconstant)
                    {
                        *exp = intNode(en_c_i, sp2->sb->value.i);
                    }
                    else if (sp2->sb->storage_class == sc_static || sp2->sb->storage_class == sc_external)
                    {
                        if (Optimizer::architecture == ARCHITECTURE_MSIL)
                        {
                            *exp = varNode(en_global, sp2);
                        }
                        else
                        {
                            EXPRESSION* exp2 = varNode(en_global, sp2);
                            *exp = exprNode(en_void, *exp, exp2);
                        }
                    }
                    else
                    {
                        if (isstructured(typein) && !typein->lref && !typein->rref && !(flags & _F_AMPERSAND) && basetype(typein)->sp->sb->structuredAliasType)
                        {
                            structuredAlias = true;
                            // use it as-is
                        }
                        else
                        {
                            EXPRESSION* offset;
                            if (Optimizer::architecture == ARCHITECTURE_MSIL)
                            {
                                offset = varNode(en_structelem, sp2);  // prepare for the MSIL ldflda instruction
                            }
                            else
                                offset = intNode(en_c_i, sp2->sb->offset);
                            if (!typein2 && sp2->sb->parentClass != basetype(typ2)->sp)
                            {
                                *exp = baseClassOffset(sp2->sb->parentClass, basetype(typ2)->sp, *exp);
                            }
                            *exp = exprNode(en_structadd, *exp, offset);
                            if (sp3)
                            {
                                do
                                {
                                    if (basetype(sp3->tp)->type == bt_union)
                                    {
                                        offset->unionoffset = true;
                                        break;
                                    }
                                    if (sp3 != sp2 && ispointer(sp3->tp))
                                        break;
                                    sp3 = sp3->sb->parent;
                                } while (sp3);
                            }
                        }
                        if (isref(*tp))
                        {
                            deref(*tp, exp);
                            *tp = basetype(*tp)->btp;
                        }
                        if (typein2)
                            deref(typein2, exp);
                    }
                    if (tpb->hasbits)
                    {
                        (*exp) = exprNode(en_bits, *exp, 0);
                        (*exp)->bits = tpb->bits;
                        (*exp)->startbit = tpb->startbit;
                    }
                    if (sp2->sb->storage_class != sc_constant && sp2->sb->storage_class != sc_enumconstant && (!structuredAlias || ((*exp)->type != en_func && (*exp)->type != en_thisref ) ))
                    {
                        deref(*tp, exp);
                    }
                    if (sp2->sb->storage_class != sc_enumconstant && !structuredAlias)
                    {
                        (*exp)->v.sp = sp2;  // caching the member symbol in the enode for constexpr handling
                    }
                    if (isatomic(basetp))
                    {
                        // note this won't work in C++ because of offset2...
                        (*exp)->isatomic = true;
                        if (needsAtomicLockFromType(*tp))
                            (*exp)->lockOffset = basetp->size - ATOMIC_FLAG_SPACE - sp2->sb->offset;
                    }
                }
                if (!points && ((*exp)->type == en_not_lvalue || (*exp)->type == en_func || (*exp)->type == en_void))
                    if (ISKW(lex) && lex->data->kw->key >= assign && lex->data->kw->key <= asxor)
                        error(ERR_LVALUE);
            }
        }
    }
    return lex;
}
TYPE* LookupSingleAggregate(TYPE* tp, EXPRESSION** exp, bool memberptr)
{
    auto tp1 = tp;
    if (tp->type == bt_aggregate)
    {
        auto sp = tp->syms->front();
        if (memberptr && sp->sb->parentClass && sp->sb->storage_class != sc_static && sp->sb->storage_class != sc_global &&
            sp->sb->storage_class != sc_external)
        {
            EXPRESSION* rv;
            tp1 = MakeType(bt_memberptr, sp->tp);
            tp1->sp = sp->sb->parentClass;
            *exp = varNode(en_memberptr, sp);
            (*exp)->isfunc = true;
        }
        else
        {
            if ((*exp)->type == en_func && (*exp)->v.func->templateParams)
            {
                sp = detemplate(sp, (*exp)->v.func, nullptr);
                if (!sp)
                {
                    sp = tp->syms->front();
                }
                else
                {
                    if (sp->sb->templateLevel && !templateNestingCount && sp->templateParams)
                    {
                        sp = TemplateFunctionInstantiate(sp, false);
                    }
                }
            }
            *exp = varNode(en_pc, sp);
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
    if (in->type == en_add && in->right->type == en_umul && in->right->right->type == en__sizeof)
    {
        if (in->left->type == en_add && in->left->right->type == en_umul && in->left->right->right->type == en__sizeof)
        {
            if (comparetypes(in->right->right->left->v.tp, in->left->right->right->left->v.tp, 0))
            {
                EXPRESSION* temp = exprNode(en_add, in->left->right->left, in->right->left);
                temp = exprNode(en_umul, temp, in->right->right);
                in->left = in->left->left;
                in->right = temp;
            }
        }
    }
    return in;
}
static LEXLIST* expression_bracket(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    TYPE* tp2 = nullptr;
    EXPRESSION* expr2 = nullptr;
    lex = getsym();
    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, begin))
    {
        std::list<INITLIST*>* args = nullptr;
        lex = getInitList(lex, funcsp, &args);
        if ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) &&
            insertOperatorFunc(ovcl_openbr, openbr, funcsp, tp, exp, nullptr, nullptr, args, flags))
        {
        }
        else
        {
            error(ERR_CANNOT_USE_INIT_LIST_WITH_BUILTIN_BRACKET);
        }
    }
    else
    {
        lex = expression_comma(lex, funcsp, nullptr, &tp2, &expr2, nullptr, flags);
        if (tp2)
        {
            LookupSingleAggregate(tp2, &expr2);
            if ((isstructuredmath(*tp, tp2) || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                insertOperatorFunc(ovcl_openbr, openbr, funcsp, tp, exp, tp2, expr2, nullptr, flags))
            {
            }
            else if (isvoid(*tp) || isvoid(tp2) || (*tp)->type == bt_aggregate || ismsil(*tp) || ismsil(tp2))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            else if (basetype(tp2)->type == bt_memberptr || basetype(*tp)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            else if ((basetype(tp2)->scoped || basetype(*tp)->scoped) && !(flags & _F_SCOPEDENUM))
                error(ERR_SCOPED_TYPE_MISMATCH);
            else if (isarray(*tp) && (*tp)->msil)
            {
                if (!isint(tp2) && basetype(tp2)->type != bt_enum)
                {
                    if (ispointer(tp2))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                if ((*exp)->type != en_msil_array_access)
                {
                    EXPRESSION* expr3 = exprNode(en_msil_array_access, nullptr, nullptr);
                    TYPE* btp = *tp;
                    int count = 0;
                    while (isarray(btp) && btp->msil)
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
                *tp = basetype(*tp)->btp;
                if (!MATCHKW(lex, closebr))
                {
                    error(ERR_ARRAY_NEED_CLOSEBRACKET);
                    errskim(&lex, skim_closebr);
                }
                skip(&lex, closebr);
                if ((*exp)->v.msilArray->count < (*exp)->v.msilArray->max && !MATCHKW(lex, openbr))
                {
                    error(ERR_ASSIGN_ONLY_MSIL_ARRAY_ELEMENTS);
                }
                *tp = PerformDeferredInitialization(*tp, funcsp);
                return lex;
            }
            else if (ispointer(*tp) && !isfuncptr(*tp))
            {
                if (!isint(tp2) && basetype(tp2)->type != bt_enum)
                {
                    if (ispointer(tp2))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                /* unreference pointer to VLA */
                if (!(*tp)->vla && basetype(basetype(*tp)->btp)->vla)
                    (*tp) = basetype(*tp)->btp;
                if ((*tp)->vla)
                {
                    int soa = getSize(bt_pointer);
                    int sou = getSize(bt_unsigned);
                    EXPRESSION* vlanode = varNode(en_auto, (*tp)->sp);
                    EXPRESSION* exp1 = exprNode(en_add, vlanode, intNode(en_c_i, soa + sou * (1 + (*tp)->vlaindex)));
                    deref(&stdint, &exp1);
                    cast(&stdpointer, &expr2);
                    exp1 = exprNode(en_umul, exp1, expr2);
                    *exp = exprNode(en_add, *exp, exp1);
                    *tp = basetype(*tp)->btp;
                }
                else
                {
                    EXPRESSION* exp1 = nullptr;
                    *tp = basetype(*tp)->btp;
                    if (isstructured(*tp))
                        *tp = PerformDeferredInitialization(*tp, funcsp);
                    cast(&stdint, &expr2);
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        cast(&stdinative, &expr2);
                        if (!isarray(*tp))
                        {
                            auto exp3 = exprNode(en__sizeof, typeNode(*tp), nullptr);
                            exp1 = exprNode(en_umul, expr2, exp3);
                        }
                        else
                        {
                            TYPE* tp1 = *tp;
                            int n = tp1->size;
                            while (isarray(tp1))
                                tp1 = tp1->btp;
                            n = n / tp1->size;
                            auto exp3 = exprNode(en__sizeof, typeNode(tp1), nullptr);
                            auto exp4 = intNode(en_c_i, n);
                            expr2 = exprNode(en_umul, expr2, exp4);
                            exp1 = exprNode(en_umul, expr2, exp3);
                        }
                    }
                    else
                    {
                        exp1 = exprNode(en_umul, expr2, intNode(en_c_i, (*tp)->size));
                    }
                    *exp = exprNode(en_add, *exp, exp1);
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        *exp = MsilRebalanceArray(*exp);
                    }
                }
                if (!(*tp)->array && !(*tp)->vla)
                    deref(*tp, exp);
            }
            else if (ispointer(tp2) && !isfuncptr(tp2))
            {
                if (!isint(*tp))
                {
                    if (ispointer(*tp))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    else
                        error(ERR_NEED_INTEGER_EXPRESSION);
                }
                if (tp2->vla)
                {
                    int soa = getSize(bt_pointer);
                    int sou = getSize(bt_unsigned);
                    EXPRESSION* vlanode = varNode(en_auto, tp2->sp);
                    expr2 = exprNode(en_add, expr2, intNode(en_c_i, soa + sou * (1 + tp2->vlaindex)));
                    deref(&stdint, exp);
                    *exp = exprNode(en_umul, expr2, *exp);
                    deref(&stdpointer, &vlanode);
                    *exp = exprNode(en_add, *exp, vlanode);
                    *tp = basetype(tp2)->btp;
                }
                else
                {
                    EXPRESSION* exp1 = nullptr;
                    *tp = basetype(tp2)->btp;
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        cast(&stdinative, exp);
                        if (!isarray(*tp))
                        {
                            auto exp3 = exprNode(en__sizeof, typeNode(*tp), nullptr);
                            exp1 = exprNode(en_umul, *exp, exp3);
                        }
                        else
                        {
                            TYPE* tp1 = *tp;
                            int n = tp1->size;
                            while (isarray(tp1))
                                tp1 = tp1->btp;
                            n = n / tp1->size;
                            auto exp3 = exprNode(en__sizeof, typeNode(tp1), nullptr);
                            auto exp4 = intNode(en_c_i, n);
                            expr2 = exprNode(en_umul, *exp, exp4);
                            exp1 = exprNode(en_umul, *exp, exp3);
                        }
                    }
                    else
                    {
                        exp1 = exprNode(en_umul, *exp, intNode(en_c_i, (*tp)->size));
                    }
                    *exp = exprNode(en_add, expr2, exp1);
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        *exp = MsilRebalanceArray(*exp);
                    }
                }
                if (!(*tp)->array && !(*tp)->vla)
                    deref(*tp, exp);
            }
            else if (!templateNestingCount || basetype(*tp)->type != bt_templateselector)
            {
                error(ERR_DEREF);
            }
        }
        else
            error(ERR_EXPRESSION_SYNTAX);
    }
    if (!MATCHKW(lex, closebr))
    {
        error(ERR_ARRAY_NEED_CLOSEBRACKET);
        errskim(&lex, skim_closebr);
    }
    skip(&lex, closebr);
    *tp = PerformDeferredInitialization(*tp, funcsp);
    return lex;
}
void checkArgs(FUNCTIONCALL* params, SYMBOL* funcsp)
{
    std::list<INITLIST*>::iterator itp, itpe = itp;
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

    auto it = basetype(params->functp)->syms->begin();
    auto itend = basetype(params->functp)->syms->end();
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
    if (it != itend && (*it)->tp->type == bt_void)
    {
        if (itp != itpe)
            errorsym(ERR_FUNCTION_TAKES_NO_PARAMETERS, params->sp);
    }
    else
        while (it != itend || itp != itpe)
        {
            TYPE* dest = nullptr;
            SYMBOL* decl = nullptr;
            if (it == itend || (*it)->tp->type != bt_any)
            {
                argnum++;
                if (matching && it != itend)
                {
                    decl = *it;
                    if (!decl->tp)
                        noproto = true;
                    else if (basetype(decl->tp)->type == bt_ellipse)
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
                if (matching)
                {
                    if (!decl)
                        toolong = true;
                    else if (itp == itpe && !(*it)->sb->init && !(*it)->sb->deferredCompile)
                        tooshort = true;
                    else
                    {
                        if (decl->sb->attribs.inheritable.zstring)
                        {
                            EXPRESSION* exp = (*itp)->exp;
                            if (!exp)
                            {
                                exp = (*itp)->exp = intNode(en_c_i, 0);
                            }
                            if (lvalue(exp))
                                exp = exp->left;
                            switch (exp->type)
                            {
                                case en_global:
                                case en_auto:
                                case en_absolute:
                                case en_pc:
                                case en_threadlocal:
                                    if (exp->v.sp->sb->attribs.inheritable.nonstring)
                                    {
                                        error(ERR_NULL_TERMINATED_STRING_REQUIRED);
                                    }
                                    break;
                            }
                        }
                        if (isref(decl->tp))
                        {
                            TYPE* tpb = basetype(basetype(decl->tp)->btp);
                            TYPE* tpd = (*itp)->tp;
                            (*itp)->byRef = true;
                            if (isref(tpd))
                                tpd = basetype(tpd)->btp;
                            tpd = basetype(tpd);
                            if (isstructured(tpb) && isstructured(tpd))
                            {
                                SYMBOL* base = basetype(tpb)->sp;
                                SYMBOL* derived = basetype(tpd)->sp;
                                if (base != derived && classRefCount(base, derived) != 1)
                                    errortype(ERR_CANNOT_CONVERT_TYPE, (*itp)->tp, decl->tp);
                                if (base != derived && !isAccessible(derived, derived, base, funcsp, ac_public, false))
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
                            else if (basetype(decl->tp)->type == bt___string)
                            {
                                if ((*itp)->exp->type == en_labcon)
                                    (*itp)->exp->type = en_c_string;
                            }
                            else if (!comparetypes((*itp)->tp, decl->tp, false))
                            {
                                if ((Optimizer::architecture != ARCHITECTURE_MSIL) || !isstructured(decl->tp))
                                {
                                    if (basetype(decl->tp)->type == bt___object)
                                    {
                                        if (!isstructured((*itp)->tp) && (!isarray((*itp)->tp) || !basetype((*itp)->tp)->msil))
                                        {
                                            (*itp)->exp = exprNode(en_x_object, (*itp)->exp, nullptr);
                                        }
                                    }
                                    else if (basetype(decl->tp)->type != bt_memberptr)
                                        errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                                }
                                else if (isstructured((*itp)->tp))
                                {
                                    SYMBOL* base = basetype(decl->tp)->sp;
                                    SYMBOL* derived = basetype((*itp)->tp)->sp;
                                    if (base != derived && classRefCount(base, derived) != 1)
                                        errortype(ERR_CANNOT_CONVERT_TYPE, (*itp)->tp, decl->tp);
                                    if (base != derived && !isAccessible(derived, derived, base, funcsp, ac_public, false))
                                        errorsym(ERR_CANNOT_ACCESS, base);
                                }
                                else if (!isconstzero((*itp)->tp, (*itp)->exp))
                                    errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                            }
                            else if (assignDiscardsConst(decl->tp, (*itp)->tp))
                            {
                                if (Optimizer::cparams.prm_cplusplus)
                                    errortype(ERR_CANNOT_CONVERT_TYPE, (*itp)->tp, decl->tp);
                                else
                                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                            }
                            else if (ispointer(decl->tp))
                            {
                                if (!ispointer((*itp)->tp) && (!isarithmeticconst((*itp)->exp) || !isconstzero(decl->tp, (*itp)->exp)))
                                {
                                    if (!isfunction((*itp)->tp) ||
                                        (!isvoidptr(decl->tp) &&
                                         (!isfuncptr(decl->tp) || (Optimizer::cparams.prm_cplusplus &&
                                                                   !comparetypes(basetype(decl->tp)->btp, (*itp)->tp, true)))))
                                        errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                                }
                                else if (!comparetypes(decl->tp, (*itp)->tp, true))
                                {
                                    if (!isconstzero((*itp)->tp, (*itp)->exp))
                                    {
                                        if (!isvoidptr(decl->tp) && !isvoidptr((*itp)->tp))
                                        {
                                            if (Optimizer::cparams.prm_cplusplus)
                                            {
                                                TYPE* tpb = basetype(decl->tp)->btp;
                                                TYPE* tpd = basetype((*itp)->tp)->btp;
                                                if (isstructured(tpb) && isstructured(tpd))
                                                {
                                                    SYMBOL* base = basetype(tpb)->sp;
                                                    SYMBOL* derived = basetype(tpd)->sp;
                                                    if (base != derived && classRefCount(base, derived) != 1)
                                                        errortype(ERR_CANNOT_CONVERT_TYPE, (*itp)->tp, decl->tp);
                                                    if (isAccessible(derived, derived, base, funcsp, ac_public, false))
                                                        errorsym(ERR_CANNOT_ACCESS, base);
                                                }
                                                else
                                                {
                                                    errortype(ERR_CANNOT_CONVERT_TYPE, (*itp)->tp, decl->tp);
                                                }
                                            }
                                            else
                                            {
                                                if (!matchingCharTypes((*itp)->tp, decl->tp))
                                                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                                            }
                                        }
                                    }
                                }
                            }
                            else if (ispointer((*itp)->tp))
                            {
                                if (!ispointer(decl->tp) && basetype(decl->tp)->type != bt_bool)
                                    errorarg(ERR_TYPE_MISMATCH_IN_ARGUMENT, argnum, decl, params->sp);
                            }
                            else if (isarithmetic((*itp)->tp) && isarithmetic(decl->tp))
                            {
                                if (basetype(decl->tp)->type != basetype((*itp)->tp)->type && basetype(decl->tp)->type > bt_int)
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
                    (*itp)->tp = MakeType(bt_pointer, (*itp)->tp);
                    while ((*itp)->tp->btp->vla)
                        (*itp)->tp->btp = (*itp)->tp->btp->btp;
                }
                else if (isint((*itp)->tp))
                {
                    if (basetype((*itp)->tp)->type <= bt_int)
                        dest = &stdint;
                    else if (!(Optimizer::architecture == ARCHITECTURE_MSIL))
                        cast((*itp)->tp, &(*itp)->exp);
                }
                else if (isfloat((*itp)->tp))
                {
                    if (basetype((*itp)->tp)->type < bt_double)
                        dest = &stddouble;
                    else if (!(Optimizer::architecture == ARCHITECTURE_MSIL))
                        cast((*itp)->tp, &(*itp)->exp);
                }
                else if (ispointer((*itp)->tp))
                {

                    if (Optimizer::architecture == ARCHITECTURE_MSIL &&
                        ((*itp)->exp->type != en_auto || !(*itp)->exp->v.sp->sb->va_typeof) && !params->vararg)
                        cast(&stdint, &(*itp)->exp);
                }
                if (dest && itp != itpe && (*itp)->tp && basetype(dest)->type != bt_memberptr && !comparetypes(dest, (*itp)->tp, true))
                {
                    if ((Optimizer::architecture != ARCHITECTURE_MSIL) ||
                        (!isstructured(dest) && (!isarray(dest) || !basetype(dest)->msil)))
                        cast(basetype(dest), &(*itp)->exp);
                    (*itp)->tp = dest;
                }
                else if (dest && itp != itpe && basetype(dest)->type == bt_enum)
                {
                    // this needs to be revisited to get proper typing in C++
                    cast(&stdint, &(*itp)->exp);
                    (*itp)->tp = &stdint;
                }
            }
            if (itp != itpe)
            {
                bool ivararg = vararg;
                if ((*itp)->exp->type == en_auto && (*itp)->exp->v.sp->sb && (*itp)->exp->v.sp->sb->va_typeof)
                    ivararg = false;
                (*itp)->vararg = ivararg;
                if (it != itend)
                {
                    TYPE* tp = (*it)->tp;
                    while (tp && tp->type != bt_pointer)
                    {
                        if (tp->type == bt_va_list)
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
                ++it;
        }
    if (noproto)
        errorsym(ERR_CALL_FUNCTION_NO_PROTO, params->sp);
    else if (toolong)
        errorsym(ERR_PARAMETER_LIST_TOO_LONG, params->sp);
    else if (tooshort)
        errorsym(ERR_PARAMETER_LIST_TOO_SHORT, params->sp);
}
static LEXLIST* getInitInternal(LEXLIST* lex, SYMBOL* funcsp, std::list<INITLIST*>** lptr, enum e_kw finish, bool allowNesting, bool allowPack,
                                bool toErr, int flags)
{
    *lptr = initListListFactory.CreateList();
    lex = getsym(); /* past ( */
    while (!MATCHKW(lex, finish))
    {
        INITLIST* p = Allocate<INITLIST>();
        if (finish == end)
            p->initializer_list = true;
        if (MATCHKW(lex, begin))
        {
            lex = getInitInternal(lex, funcsp, &p->nested, end, true, false, false, flags);
            (*lptr)->push_back(p);
            if (!allowNesting)
                error(ERR_CANNOT_USE_INIT_LIST);
        }
        else
        {
            LEXLIST* start = lex;
            lex = expression_assign(lex, funcsp, nullptr, &p->tp, &p->exp, nullptr,
                                    _F_PACKABLE | (finish == closepa ? _F_INARGS : 0) | (flags & _F_SIZEOF));
            if (p->tp && isvoid(p->tp) && finish != closepa)
                error(ERR_NOT_AN_ALLOWED_TYPE);
            if (!p->exp)
                p->exp = intNode(en_c_i, 0);
            optimize_for_constants(&p->exp);
            if ((!templateNestingCount || instantiatingTemplate) && p->tp && p->tp->type == bt_templateselector)
                p->tp = LookupTypeFromExpression(p->exp, nullptr, false);
            if (finish != closepa)
            {
                assignmentUsages(p->exp, false);
            }
            else if (p->tp && isstructured(p->tp))
            {
                auto exp3 = p->exp;
                while (exp3->type == en_void)
                    exp3 = exp3->right;
                if (exp3->type == en_thisref)
                {
                    p->tp = CopyType(p->tp);
                    p->tp->lref = false;
                    p->tp->rref = true;
                }
            }
            if (p->tp)
            {
                if (p->exp && p->exp->type == en_func && p->exp->v.func->sp->sb->parentClass && !p->exp->v.func->ascall &&
                    !p->exp->v.func->asaddress && p->exp->v.func->functp)
                {
                    for (auto sym: *basetype(p->exp->v.func->functp)->syms)
                    {
                        if (sym->sb->storage_class == sc_member || sym->sb->storage_class == sc_mutable)
                        {
                            error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                            break;
                        }
                    }
                }
                if (allowPack && Optimizer::cparams.prm_cplusplus && MATCHKW(lex, ellipse))
                {
                    // lose p
                    lex = getsym();
                    if (templateNestingCount)
                    {
                        (*lptr)->push_back(p);
                    }
                    else if (p->exp && p->exp->type != en_packedempty && p->tp->type != bt_any)
                    {
                        if (!isstructured(p->tp) && !p->tp->templateParam)
                            checkPackedExpression(p->exp);
                        // this is going to presume that the expression involved
                        // is not too long to be cached by the LEXLIST mechanism.
                        expandPackedInitList(lptr, funcsp, start, p->exp);
                    }
                }
                else
                {
                    if (toErr && argumentNesting <= 1)
                        checkUnpackedExpression(p->exp);
                    (*lptr)->push_back(p);
                }
            }
            else
            {
                lex = backupsym();
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(&lex, finish == closepa ? skim_closepa : skim_end);
                return lex;
            }
        }
        if (!MATCHKW(lex, comma))
        {
            break;
        }
        lex = getsym();
        if (MATCHKW(lex, finish))
            break;
    }
    if (!needkw(&lex, finish))
    {
        errskim(&lex, finish == closepa ? skim_closepa : skim_end);
        skip(&lex, finish);
    }
    DestructParams(*lptr);
    return lex;
}
LEXLIST* getInitList(LEXLIST* lex, SYMBOL* funcsp, std::list<INITLIST*>** owner)
{
    argumentNesting++;
    auto rv = getInitInternal(lex, funcsp, owner, end, false, true, true, 0);
    argumentNesting--;
    return rv;
}
LEXLIST* getArgs(LEXLIST* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, enum e_kw finish, bool allowPack, int flags)
{
    LEXLIST* rv;
    argumentNesting++;
    rv = getInitInternal(lex, funcsp, &funcparams->arguments, finish, true, allowPack, argumentNesting == 1, flags);
    argumentNesting--;
    return rv;
}
LEXLIST* getMemberInitializers(LEXLIST* lex, SYMBOL* funcsp, FUNCTIONCALL* funcparams, enum e_kw finish, bool allowPack)
{
    LEXLIST* rv;
    argumentNesting++;
    rv = getInitInternal(lex, funcsp, &funcparams->arguments, finish, true, allowPack, false, 0);
    argumentNesting--;
    return rv;
}
static int simpleDerivation(EXPRESSION* exp)
{
    int rv = 0;
    while (castvalue(exp))
        exp = exp->left;
    if (exp->left)
        rv |= simpleDerivation(exp->left);
    if (exp->right)
        rv |= simpleDerivation(exp->right);
    switch (exp->type)
    {
        case en_func:
        case en_thisref:
        case en_stmt:
            rv |= 1;
            break;
        case en_labcon:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_pc:
        case en_threadlocal:
            rv |= 2;
            break;
        default:
            if (lvalue(exp))
                rv |= 1;
            break;
    }
    return rv;
}
EXPRESSION* DerivedToBase(TYPE* tpn, TYPE* tpo, EXPRESSION* exp, int flags)
{
    if (isref(tpn))
        tpn = basetype(tpn)->btp;
    if (isref(tpo))
        tpo = basetype(tpo)->btp;
    if (isstructured(tpn) && isstructured(tpo))
    {
        SYMBOL* spn = basetype(tpn)->sp;
        SYMBOL* spo = basetype(tpo)->sp;
        if (spo != spn && !sameTemplate(tpn, tpo))
        {
            int n = classRefCount(spn, spo);
            if (n == 1)
            {
                // derived to base
                EXPRESSION q = {}, *v = &q;
                v->type = en_c_i;
                v = baseClassOffset(spn, spo, v);

                optimize_for_constants(&v);
                if (v->type != en_c_i && (flags & _F_NOVIRTUALBASE))
                    return nullptr;

                v = baseClassOffset(spn, spo, exp);
                if ((flags & _F_VALIDPOINTER) || v->type != en_c_i || simpleDerivation(exp) == 2)
                {
                    optimize_for_constants(&v);
                }
                else
                {
                    EXPRESSION* varsp = anonymousVar(sc_auto, &stdpointer);
                    EXPRESSION* var = exprNode(en_l_p, varsp, nullptr);
                    EXPRESSION* asn = exprNode(en_assign, var, exp);
                    EXPRESSION* left = exprNode(en_add, var, v);
                    EXPRESSION* right = var;
                    v = exprNode(en_cond, var, exprNode(en_void, left, right));
                    v = exprNode(en_void, asn, v);
                }
                return v;
            }
        }
    }
    return exp;
}
static bool cloneTempExpr(EXPRESSION** expr, SYMBOL** found, SYMBOL** replace);
bool cloneTempStmt(std::list<STATEMENT*>** blocks, SYMBOL** found, SYMBOL** replace)
{
    bool rv = false;
    auto p = *blocks;
    *blocks = stmtListFactory.CreateList();
    for (auto block : *p)
    {
        if (found)
        {
            STATEMENT* b = Allocate<STATEMENT>();
            *b = *block;
            block = b;
            (*blocks)->push_back(b);
        }
        switch (block->type)
        {
            case st__genword:
                break;
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                rv |= cloneTempStmt(&block->lower, found, replace);
                rv |= cloneTempStmt(&block->blockTail, found, replace);
                break;
            case st_return:
            case st_expr:
            case st_declare:
                rv |= cloneTempExpr(&block->select, found, replace);
                break;
            case st_goto:
            case st_label:
                break;
            case st_select:
            case st_notselect:
                rv |= cloneTempExpr(&block->select, found, replace);
                break;
            case st_switch:
                rv |= cloneTempExpr(&block->select, found, replace);
                rv |= cloneTempStmt(&block->lower, found, replace);
                break;
            case st_block:
                rv |= cloneTempStmt(&block->lower, found, replace);
                rv |= cloneTempStmt(&block->blockTail, found, replace);
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
    if ((*expr)->type == en_thisref)
    {
        rv = cloneTempExpr(&(*expr)->left, found, replace);
    }
    else if ((*expr)->type == en_stmt)
    {
        rv = cloneTempStmt(&(*expr)->v.stmt, found, replace);
    }
    else if ((*expr)->type == en_func)
    {
        if ((*expr)->v.func->sp->sb->isConstructor || (*expr)->v.func->sp->sb->isDestructor)
        {
            INITLIST** args;
            if (found)
            {
                FUNCTIONCALL* newval = Allocate<FUNCTIONCALL>();
                *newval = *(*expr)->v.func;
                (*expr)->v.func = newval;
            }
            if ((*expr)->v.func->arguments)
            {
                if (found)
                {
                    std::list<INITLIST*>* newList = initListListFactory.CreateList();
                    for (auto arg : *(*expr)->v.func->arguments)
                    {
                        INITLIST* newval = Allocate<INITLIST>();
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
        if ((*expr)->type == en_auto)
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
TYPE* InitializerListType(TYPE* arg)
{
    SYMBOL* sym = namespacesearch("std", globalNameSpace, false, false);
    if (sym && sym->sb->storage_class == sc_namespace)
    {
        sym = namespacesearch("initializer_list", sym->sb->nameSpaceValues, true, false);
        if (sym)
        {
            std::list<TEMPLATEPARAMPAIR>* tplp = templateParamPairListFactory.CreateList();
            auto tpl = Allocate<TEMPLATEPARAM>();
            tpl->type = kw_typename;
            tpl->byClass.dflt = arg;
            tplp->push_back(TEMPLATEPARAMPAIR{ nullptr, tpl });
            auto sym1 = GetClassTemplate(sym, tplp, true);
            if (sym1)
            {
                sym1 = TemplateClassInstantiate(sym1, tplp, false, sc_auto);
                if (sym1)
                    sym = sym1;
            }
        }
    }
    TYPE* rtp;
    if (sym)
    {
        rtp = sym->tp;
    }
    else
    {
        rtp = MakeType(bt_struct);
        rtp->sp = makeID(sc_type, rtp, nullptr, "initializer_list");
        rtp->sp->sb->initializer_list = true;
    }
    return rtp;
}
EXPRESSION* getFunc(EXPRESSION* exp)
{
    EXPRESSION* rv = nullptr;
    while (exp->type == en_void && exp->right)
    {
        rv = getFunc(exp->left);
        if (rv)
            return rv;
        exp = exp->right;
    }
    if (exp->type == en_thisref)
        exp = exp->left;
    if (exp->type == en_add)
    {
        rv = getFunc(exp->left);
        if (!rv)
            rv = getFunc(exp->right);
    }
    else if (exp->type == en_func)
    {
        return exp;
    }
    return rv;
}
void CreateInitializerList(SYMBOL* func, TYPE* initializerListTemplate, TYPE* initializerListType, std::list<INITLIST*>** lptr, bool operands,
                           bool asref)
{
 
    std::list<INITLIST*>* initial = *lptr;
    EXPRESSION *rv = nullptr, **pos = &rv;
    int count = initial->size(), i;
    EXPRESSION *data, *initList;
    SYMBOL *begin, *size;
    EXPRESSION* dest;
    begin = search(basetype(initializerListTemplate)->syms, "__begin_");
    size = search(basetype(initializerListTemplate)->syms, "__size_");
    if (!begin || !size)
        Utils::fatal("Invalid definition of initializer-list");
    if (!initial->front()->nested && comparetypes(initial->front()->tp, initializerListTemplate, true))
    {
        auto exp = initial->front()->exp;
        initial->clear();
        initial->push_back(Allocate<INITLIST>());
        if (asref)
        {
            initial->front()->tp = MakeType(bt_lref, initializerListTemplate);
            initial->front()->exp = exp;
        }
        else
        {
            initial->front()->tp = initializerListTemplate;
            initial->front()->exp = exprNode(en_stackblock, exp, nullptr);
            initial->front()->exp->size = initializerListTemplate;
        }
    }
    else
    {
        auto itl = initial->begin();
        auto itle = initial->end();
        TYPE* tp = MakeType(bt_pointer, initializerListType);
        tp->array = true;
        tp->size = count * (initializerListType->size);
        tp->esize = intNode(en_c_i, count);
        data = anonymousVar(sc_auto, tp);
        if ((*itl)->nested)
        {
            bool found = false;
            for (auto l : *(*itl)->nested)
                if (!IsConstantExpression(l->exp, false, false))
                {
                    found = true;
                    break;
                }
            if (!found)
                data->v.sp->sb->constexpression = true;
        }
        if (isstructured(initializerListType))
        {
            EXPRESSION* exp = data;
            EXPRESSION* elms = intNode(en_c_i, count);
            callDestructor(basetype(initializerListType)->sp, nullptr, &exp, elms, true, false, false, true);
            initInsert(&data->v.sp->sb->dest, tp, exp, 0, false);
        }
        std::deque<EXPRESSION*> listOfScalars;
        for (i = 0; i < count; i++, ++itl)
        {
            EXPRESSION* node;
            dest = exprNode(en_add, data, intNode(en_c_i, i * (initializerListType->size)));
            if (isstructured(initializerListType))
            {
                initializerListType = basetype(initializerListType);
                if (initializerListType->sp->sb->trivialCons)
                {
                    node = nullptr;
                    auto list = &node;
                    auto ita = (*itl)->nested ? (*itl)->nested->begin() : itl;
                    auto itae = (*itl)->nested ? (*itl)->nested->end() : initial->end();

                    SymbolTable<SYMBOL>::iterator it;
                    for (it = initializerListType->syms->begin(); ita != itae && it != initializerListType->syms->end(); ++it, ++ita)
                    {
                        auto sym = *it;
                        if (ismemberdata(sym))
                        {
                            auto pos = exprNode(en_structadd, dest, intNode(en_c_i, sym->sb->offset));
                            deref(sym->tp, &pos);
                            auto node1 = exprNode(en_assign, pos, (*ita)->exp);
                            if (node)
                            {
                                *list = exprNode(en_void, *list, node1);
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
                            auto pos = exprNode(en_structadd, dest, intNode(en_c_i, sym->sb->offset));
                            deref(sym->tp, &pos);
                            auto node1 = exprNode(en_assign, pos, intNode(en_c_i, 0));
                            if (node)
                            {
                                *list = exprNode(en_void, *list, node1);
                                list = &(*list)->right;
                            }
                            else
                            {
                                node = node1;
                            }
                        }
                    }
                }
                else if (*itl && !(*itl)->initializer_list && (*itl)->nested)
                {
                    auto ita = (*itl)->nested->begin();
                    auto itae = (*itl)->nested->end();
                    node = nullptr;
                    auto list = &node;
                    int count1 = 0;
                    for (; ita != itae; ++ita)
                    {
                        TYPE* ctype = initializerListType;
                        auto pos = copy_expression(exprNode(en_add, dest, intNode(en_c_i, count1++ * initializerListType->size)));
                        FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                        params->ascall = true;
                        params->thisptr = pos;
                        INITLIST* next = Allocate<INITLIST>();
                        *next = **ita;
                        params->arguments = initListListFactory.CreateList();
                        params->arguments->push_back(next);
                        callConstructor(&ctype, &pos, params, false, nullptr, true, false, false, false, _F_INITLIST, false, true);
                        if (node)
                        {
                            *list = exprNode(en_void, *list, pos);
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
                else if (!(*itl)->nested && isstructured(initializerListType) && (comparetypes(initializerListType, (*itl)->tp, true) || sameTemplate(initializerListType, (*itl)->tp)))
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
                        else if (isref(basetype(exp->v.func->sp->tp)->btp))
                        {
                            TYPE* ctype = initializerListType;
                            EXPRESSION* cdest = dest;
                            FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                            params->arguments = initListListFactory.CreateList();
                            params->arguments->push_back(*itl);

                            callConstructor(&ctype, &cdest, params, false, nullptr, true, false, false, false, _F_INITLIST, false,
                                            true);
                            node = cdest;

                        }
                    }
                    else
                    {
                        TYPE* ctype = initializerListType;
                        EXPRESSION* cdest = dest;
                        FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                        params->arguments = initListListFactory.CreateList();
                        params->arguments->push_back(*itl);

                        callConstructor(&ctype, &cdest, params, false, nullptr, true, false, false, false, _F_INITLIST, false,
                                        true);
                        node = cdest;
                    }
                }
                else
                {
                    TYPE* ctype = initializerListType;
                    EXPRESSION* cdest = dest;
                    FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                    if ((*itl)->nested)
                    {
                        params->arguments = (*itl)->nested;
                    }
                    else
                    {
                        params->arguments = initListListFactory.CreateList();
                        params->arguments->push_back(*itl);
                    }

                    callConstructor(&ctype, &cdest, params, false, nullptr, true, false, false, false, _F_INITLIST, false, true);
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
                    for ( ; ita != itae ; ++ita)
                    {
                        auto pos = exprNode(en_structadd, dest, intNode(en_c_i, count1++ * initializerListType->size));
                        deref(initializerListType, &pos);
                        auto node1 = exprNode(en_assign, pos, (*ita)->exp);
                        listOfScalars.push_back((*ita)->exp);
                        if (node)
                        {
                            *list = exprNode(en_void, *list, node1);
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
                    deref(initializerListType, &dest);
                    node = exprNode(en_assign, dest, (*ita)->exp);
                }
            }
            if (rv)
            {
                *pos = exprNode(en_void, *pos, node);
                pos = &(*pos)->right;
            }
            else
            {
                rv = node;
            }
        }
        initList = anonymousVar(sc_auto, initializerListTemplate);
        initList->v.sp->sb->constexpression = true;
        if (func && func->sb->constexpression && !listOfScalars.empty())
        {
            if (!initList->v.sp->sb->init)
                initList->v.sp->sb->init = initListFactory.CreateList();
            for (auto t : listOfScalars)
            {
                auto init = Allocate<INITIALIZER>();
                init->exp = t;
                initList->v.sp->sb->init->push_back(init);
            }
        }
        dest = exprNode(en_structadd, initList, intNode(en_c_i, begin->sb->offset));
        deref(&stdpointer, &dest);
        dest = exprNode(en_assign, dest, data);
        if (rv)
        {
            *pos = exprNode(en_void, *pos, dest);
            pos = &(*pos)->right;
        }
        else
        {
            rv = dest;
        }
        dest = exprNode(en_structadd, initList, intNode(en_c_i, size->sb->offset));
        deref(&stdpointer, &dest);
        dest = exprNode(en_assign, dest, intNode(en_c_i, tp->size / initializerListType->size));
        if (rv)
        {
            *pos = exprNode(en_void, *pos, dest);
            pos = &(*pos)->right;
        }
        else
        {
            rv = dest;
        }
        initial = (*lptr) = initListListFactory.CreateList();
        initial->push_back(Allocate<INITLIST>());
        if (asref)
        {
            initial->front()->tp = MakeType(bt_lref, initializerListTemplate);
            initial->front()->exp = exprNode(en_void, rv, initList);
        }
        else
        {
            initial->front()->tp = initializerListTemplate;
            initial->front()->exp = exprNode(en_stackblock, exprNode(en_void, rv, initList), nullptr);
            initial->front()->exp->size = initializerListTemplate;
        }
    }
}
void AdjustParams(SYMBOL* func, SymbolTable<SYMBOL>::iterator it, SymbolTable<SYMBOL>::iterator itend, std::list<INITLIST*>** lptr, bool operands, bool implicit)
{
    std::list<INITLIST*>::iterator itl, itle;
    if (*lptr)
    {
        itl = (*lptr)->begin();
        itle = (*lptr)->end();
    }
    auto old = argFriend;
    argFriend = func;
    (void)operands;
    if (func->sb->storage_class == sc_overloads)
        return;	
    if (it != itend && (*it)->sb->thisPtr)
        ++it;
    while (it != itend  && (itl != itle || (*it)->sb->init != nullptr ||
                  ((*it)->sb->deferredCompile != nullptr && (!templateNestingCount || instantiatingTemplate))))
    {
        SYMBOL* sym = *it;
        INITLIST* p;

        if (itl == itle)
        {
            if (!*lptr)
            {
                *lptr = initListListFactory.CreateList();
                itl = (*lptr)->begin();
                itle = (*lptr)->end();
            }
            deferredInitializeDefaultArg(sym, func);
            EXPRESSION* q;
            if (sym->sb->init && sym->sb->init->front()->exp)
                q = sym->sb->init->front()->exp;
            else
                q = intNode(en_c_i, sym->tp->size);
            optimize_for_constants(&q);
            auto il = Allocate<INITLIST>();
            il->exp = q;
            il->tp = basetype(sym->tp);
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
            if (isstructured(sym->tp))
            {
                il->exp = exprNode(en_stackblock, il->exp, nullptr);
                il->exp->size = sym->tp;
                ++it;
                ++itl;
                continue;
            }
        }
        p = *itl;
        if (p && p->exp && (p->exp->type == en_pc || p->exp->type == en_func))
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
            if (exp2 && exp2->type == en_thisref)
                exp2 = exp2->left;

            if (!isstructured(sym->tp) && p->tp)
            {
                if (isstructured(p->tp))
                {
                    TYPE* btp = basetype(func->tp)->btp;
                    if (btp && (!isref(btp) || !isstructured(basetype(btp)->btp)))
                    {
                        SYMBOL* sym = nullptr;
                        auto exp2 = p->exp;
                        if (exp2 && exp2->type == en_thisref)
                            exp2 = exp2->left;
                        if (exp2 && exp2->type == en_func)
                            sym = exp2->v.func->returnSP;
                        if (sym && !sym->sb->destructed && sym->sb->dest && sym->sb->dest->front()->exp && !basetype(sym->tp)->sp->sb->trivialCons )
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
            if (!done && !p->tp && isstructured(sym->tp) && basetype(sym->tp)->sp->sb->initializer_list)
            {
                // initlist
                std::list<INITLIST*>::iterator itpinit, itpinite = itpinit;
                if (p->nested)
                {
                    itpinit = p->nested->begin();
                    itpinite = p->nested->end();
                }
                if (isstructured(sym->tp) || (isref(sym->tp) && isstructured(basetype(sym->tp)->btp)))
                {
                    TYPE* stype = sym->tp;
                    SYMBOL* sp;
                    EXPRESSION* thisptr;
                    if (isref(stype))
                        stype = basetype(stype)->btp;
                    thisptr = anonymousVar(theCurrentFunc || !isref(sym->tp) ? sc_auto : sc_localstatic, stype);
                    sp = thisptr->v.sp;
                    if (!theCurrentFunc)
                    {
                        sp->sb->label = Optimizer::nextLabel++;
                        insertInitSym(sp);
                    }
                    if (basetype(stype)->sp->sb->trivialCons)
                    {
                        std::list<INITIALIZER*> init1;
                        std::list<INITIALIZER*>* init = { &init1 };
                        for (auto shr : *basetype(stype)->syms)
                        {
                            if (itpinit == itpinite)
                                break;
                            if (ismemberdata(shr))
                            {
                                initInsert(&init, (*itpinit)->tp, (*itpinit)->exp, shr->sb->offset, false);
                                ++itpinit;
                            }
                        }
                        p->exp = convertInitToExpression(stype, nullptr, nullptr, theCurrentFunc, init, thisptr, false);
                        if (!isref(sym->tp))
                            sp->sb->stackblock = true;
                        done = true;
                    }
                    else
                    {
                        FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                        params->ascall = true;
                        TYPE* ctype = sp->tp;
                        EXPRESSION* dexp = thisptr;
                        params->thisptr = thisptr;
                        auto itx = itpinit;
                        if (itpinit != itpinite)
                            ++itx;
                        params->arguments = initListListFactory.CreateList();
                        if ((itpinit != itpinite && itx != itpinite) || (itpinit == itpinite && !p->tp && !p->exp))  // empty initializer list)
                        {
                            TYPE* tp;
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
                            p->tp = InitializerListType(tp);
                            p->exp = intNode(en_c_i, 0);
                            CreateInitializerList(nullptr, p->tp, tp, &params->arguments, true, isref(sym->tp));
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
                            callConstructor(&ctype, &p->exp, params, false, nullptr, true, false, true, false, true, false, true);
                        }
                
                        if (!isref(sym->tp))
                        {
                            sp->sb->stackblock = true;
                        }
                        else
                        {
                            callDestructor(stype->sp, nullptr, &dexp, nullptr, true, false, false, true);
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
                else if (ispointer(sym->tp))
                {
                    EXPRESSION* thisptr = anonymousVar(theCurrentFunc ? sc_auto : sc_localstatic, sym->tp);
                    SYMBOL* sp = thisptr->v.sp;
                    int n = 0;
                    TYPE* btp = sym->tp;
                    while (isarray(btp))
                        btp = basetype(btp)->btp;
                    if (!theCurrentFunc)
                    {
                        sp->sb->label = Optimizer::nextLabel++;
                        insertInitSym(sp);
                    }
                    if (!isarray(sym->tp))
                    {
                        auto itx = itpinit;
                        while (itx != itpinite)

                        {
                            n++;
                            ++itx;
                        }
                        sym->tp = CopyType(sym->tp);
                        sym->tp->array = true;
                        sym->tp->esize = intNode(en_c_i, n);
                        UpdateRootTypes(sym->tp);
                        sym->tp->size = btp->size * n;
                    }
                    n = 0;
                    std::list<INITIALIZER*> init1;
                    std::list<INITIALIZER*>* init = &init1;
                    for ( ; itpinit != itpinite ; ++itpinit)
                    {
                        initInsert(&init, (*itpinit)->tp, (*itpinit)->exp, n, false);
                        n += btp->size;
                    }
                    p->exp = convertInitToExpression(sym->tp, nullptr, nullptr, theCurrentFunc, init, thisptr, false);
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
                        p->exp = intNode(en_c_i, 0);
                        p->tp = &stdint;
                    }
                }
            }
            if (!done && (p->exp || p->nested))
            {
                if (sym->tp->type == bt_ellipse)
                {

                    if (!isstructured(p->tp) && (p->tp->lref || p->tp->rref))
                        if (isfunction(p->tp))
                            p->exp = exprNode(en_l_ref, p->exp, nullptr);
                }
                else if (isstructured(sym->tp))
                {
                    bool sameType = false;
                    EXPRESSION* temp = p->exp;
                    TYPE* tpx = p->tp;
                    sym->tp = PerformDeferredInitialization(sym->tp, nullptr);
                    if (!tpx)
                        tpx = sym->tp;
                    if (isref(tpx))
                        tpx = basetype(tpx)->btp;
                    // result of a nested constructor
                    if (temp && temp->type == en_thisref)
                    {
                        temp = p->exp->left;
                    }
                    if (p->nested)
                    {
                        FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                        params->ascall = true;
                        params->arguments = p->nested;
                        TYPE* ctype = basetype(sym->tp);
                        EXPRESSION* consexp = anonymousVar(sc_auto, ctype);  // sc_parameter to push it...
                        SYMBOL* esp = consexp->v.sp;
                        p->exp = consexp;
                        esp->sb->stackblock = true;
                        callConstructor(&ctype, &p->exp, params, false, nullptr, true, false, false, false, 0, false, true);
                        if (p->exp->type == en_thisref)
                        {
                            TYPE* tpx = basetype(p->exp->left->v.func->sp->tp);
                            if (isfunction(tpx) && tpx->sp && tpx->sp->sb->castoperator)
                            {
                                p->tp = tpx->btp;
                            }
                        }
                    }
                    // use constructor or conversion function and push on stack ( no destructor)
                    else if (temp->type == en_func && basetype(temp->v.func->sp->tp)->btp &&
                             !isref(basetype(temp->v.func->sp->tp)->btp) &&
                             ((sameType = comparetypes(sym->tp, tpx, true)) ||
                              classRefCount(basetype(sym->tp)->sp, basetype(tpx)->sp) == 1))
                    {
                        SYMBOL* esp;
                        EXPRESSION* consexp;
                        // copy constructor...
                        TYPE* ctype = sym->tp;
                        EXPRESSION* paramexp;
                        consexp = anonymousVar(sc_auto, sym->tp);  // sc_parameter to push it...
                        esp = consexp->v.sp;
                        esp->sb->stackblock = true;
                        consexp = varNode(en_auto, esp);
                        paramexp = p->exp;
                        paramexp = DerivedToBase(sym->tp, tpx, paramexp, _F_VALIDPOINTER);
                        auto exp = consexp;
                        callConstructorParam(&ctype, &exp, sym->tp, paramexp, true, true, implicit, false, true);
                        if (exp->type == en_auto)  // recursive call to constructor A<U>(A<U>)
                        {
                            if (temp->v.func->returnEXP)
                            {
                                temp->v.func->returnEXP = consexp;
                            }
                            else
                            {
                                if (p->exp->type == en_thisref && p->exp->left->v.func->thisptr)
                                    p->exp->left->v.func->thisptr = consexp;
                                p->exp = paramexp;
                            }
                        }
                        else
                        {
                            p->exp = exp;
                        }
                        p->exp = exprNode(en_void, p->exp, consexp);
                    }
                    else if (basetype(sym->tp)->sp->sb->trivialCons)
                    {
                        p->exp = exprNode(en_stackblock, p->exp, nullptr);
                        p->exp->size = p->tp;
                    }
                    else
                    {
                        TYPE* ctype = basetype(sym->tp = PerformDeferredInitialization(sym->tp, nullptr));
                        EXPRESSION* consexp = anonymousVar(sc_auto, ctype);  // sc_parameter to push it...
                        SYMBOL* esp = consexp->v.sp;
                        EXPRESSION* paramexp = p->exp;
                        esp->sb->stackblock = true;
                        esp->sb->constexpression = true;
                        callConstructorParam(&ctype, &consexp, p->tp, paramexp, true, true, implicit, false, true);
                        if (consexp->type == en_auto)  // recursive call to constructor A<U>(A<U>)
                        {
                            p->exp = paramexp;
                        }
                        else
                        {
                            if (consexp->type == en_thisref)
                                esp->sb->constexpression = false;
                            p->exp = consexp;
                        }
                    }
                    p->tp = sym->tp;
                }
                else if (isref(sym->tp))
                {
                    bool nested = false;
                    TYPE* tpx1 = p->tp;
                    if (!tpx1)
                    {
                        tpx1 = sym->tp;
                    }
                    if (isstructured(basetype(sym->tp)->btp))
                    {
                        TYPE* tpx = tpx1;
                        if (isref(tpx))
                            tpx = basetype(tpx)->btp;
                        if (tpx->type == bt_templateparam)
                        {
                            tpx = tpx->templateParam->second->byClass.val;
                            if (!tpx)
                                tpx = basetype(sym->tp)->btp;
                        }
                        if (p->nested)
                        {
                            nested = true;
                            FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                            params->ascall = true;
                            params->arguments = p->nested;
                            TYPE* ctype = basetype(sym->tp)->btp;
                            EXPRESSION* consexp = anonymousVar(sc_auto, basetype(sym->tp)->btp);  // sc_parameter to push it...
                            SYMBOL* esp = consexp->v.sp;
                            p->exp = consexp;
                            callConstructor(&ctype, &p->exp, params, false, nullptr, true, false, false, false, 0, false, true);
                            if (p->exp->type == en_thisref)
                            {
                                TYPE* tpx = basetype(p->exp->left->v.func->sp->tp);
                                if (isfunction(tpx) && tpx->sp && tpx->sp->sb->castoperator)
                                {
                                    p->tp = tpx->btp;
                                }
                            }
                            EXPRESSION* dexp = consexp;
                            callDestructor(basetype(esp->tp)->sp, nullptr, &dexp, nullptr, true, false, false, true);
                            initInsert(&esp->sb->dest, basetype(sym->tp)->btp, dexp, 0, true);
                        }
                        else if ((!isconst(basetype(sym->tp)->btp) && !isconst(sym->tp) &&
                                 (sym->tp->type != bt_rref &&
                                   (!func->sb->templateLevel &&
                                   (!func->sb->parentClass || !func->sb->parentClass->sb->templateLevel) /*forward*/)) &&
                                  isconst(tpx)) ||
                                 (!comparetypes(sym->tp, tpx, true) && !sameTemplate(sym->tp, tpx) &&
                                  !classRefCount(basetype(basetype(sym->tp)->btp)->sp, basetype(tpx)->sp)))
                        {
                            // make temp via constructor or conversion function
                            EXPRESSION* consexp = anonymousVar(sc_auto, basetype(sym->tp)->btp);  // sc_parameter to push it...
                            SYMBOL* esp = consexp->v.sp;
                            TYPE* ctype = basetype(sym->tp)->btp;
                            EXPRESSION* paramexp = p->exp;
                            p->exp = consexp;
                            callConstructorParam(&ctype, &p->exp, basetype(p->tp), paramexp, true, true, false, false, true);
                            if (p->exp->type == en_thisref)
                            {
                                TYPE* tpx = basetype(p->exp->left->v.func->sp->tp);
                                if (isfunction(tpx) && tpx->sp && tpx->sp->sb->castoperator)
                                {
                                    p->tp = tpx->btp;
                                }
                            }
                            EXPRESSION* dexp = consexp;
                            callDestructor(basetype(esp->tp)->sp, nullptr, &dexp, nullptr, true, false, false, true);
                            initInsert(&esp->sb->dest, basetype(sym->tp)->btp, dexp, 0, true);
                        }
                        else
                        {
                            if (!comparetypes(sym->tp, tpx, true))
                                p->exp = DerivedToBase(sym->tp, tpx, p->exp, 0);
                        }
                        ConstExprPromote(p->exp, isconst(basetype(sym->tp)->btp));
                    }
                    else if (basetype(basetype(sym->tp)->btp)->type == bt_memberptr)
                    {
                        TYPE* tp2 = basetype(sym->tp)->btp;
                        if (p->exp->type == en_memberptr)
                        {
                            int lbl = dumpMemberPtr(p->exp->v.sp, tp2, true);
                            p->exp = intNode(en_labcon, lbl);
                        }
                        else if (isconstzero(p->tp, p->exp) || p->exp->type == en_nullptr)
                        {
                            EXPRESSION* dest = createTemporary(tp2, nullptr);
                            p->exp = exprNode(en_blockclear, dest, nullptr);
                            p->exp->size = tp2;
                            p->exp = exprNode(en_void, p->exp, dest);
                        }
                        else if (p->exp->type == en_func && p->exp->v.func->returnSP)
                        {
                            int lbl = dumpMemberPtr(p->exp->v.sp, tp2, true);
                            p->exp = intNode(en_labcon, lbl);
                        }
                        else if (p->exp->type == en_pc)
                        {
                            int lbl = dumpMemberPtr(p->exp->v.sp, tp2, true);
                            p->exp = intNode(en_labcon, lbl);
                        }
                        p->tp = sym->tp;
                    }
                    else if (comparetypes(sym->tp, p->tp ? p->tp : p->nested->front()->tp, true))
                    {
                        INITLIST* p1;
                        if (p->tp)
                                p1 = p;
                        else
                            p1 = p->nested->front();
                        if (isarithmeticconst(p1->exp) ||
                            (basetype(sym->tp)->type != bt_rref && !isconst(basetype(sym->tp)->btp) && isconst(p1->tp)))
                        {
                            // make numeric temp and perform cast
                            p->exp = createTemporary(sym->tp, p1->exp);
                        }
                        else
                        {
                            // pass address
                            EXPRESSION* exp = p1->exp;
                            while (castvalue(exp) || exp->type == en_not_lvalue)
                                exp = exp->left;
                            if (exp->type != en_l_ref)
                            {
                                if (!isref(sym->tp) || !isfunction(basetype(sym->tp)->btp))
                                {
                                    if (!lvalue(exp))
                                    {
                                        TYPE* tp1 = basetype(sym->tp)->btp;
                                        // make numeric temp and perform cast
                                        if (isarray(tp1))
                                        {
                                            TYPE tp2 = {};
                                            MakeType(tp2, bt_lref, &stdpointer);
                                            exp = createTemporary(&tp2, exp);
                                        }
                                        else if (!isref(sym->tp) || exp->type != en_func || (p1->tp->type == bt_aggregate || (isfunction(exp->v.func->sp->tp) && !isref(basetype(exp->v.func->sp->tp)->btp))))
                                        {
                                            exp = createTemporary(sym->tp, exp);
                                        }
                                    }
                                    else if (exp->type == en_lvalue)
                                    {
                                        exp = createTemporary(sym->tp, exp);
                                    }
                                    else
                                    {
                                        exp = exp->left;  // take address
                                        if (exp->type == en_auto)
                                        {
                                            exp->v.sp->sb->addressTaken = true;
                                        }
                                    }
                                }
                                p->exp = exp;
                            }
                            else if (ispointer(p->tp) && isstructured(basetype(p->tp)->btp))
                            {
                                // make numeric temp and perform cast
                                p->exp = createTemporary(sym->tp, exp);
                            }
                            else if (basetype(basetype(sym->tp)->btp)->byRefArray)
                            {
                                 p->exp->left = p->exp->left->left;
                            }
                        }
                    }
                    else if (isstructured(p->tp))
                    {
                        // arithmetic or pointer
                        TYPE* etp = basetype(sym->tp)->btp;
                        if (cppCast(p->tp, &etp, &p->exp))
                            p->tp = etp;
                        if (lvalue(p->exp))
                            p->exp = p->exp->left;
                        else
                            p->exp = createTemporary(sym->tp, p->exp);
                    }
                    else
                    {
                        // make numeric temp and perform cast
                        p->exp = createTemporary(sym->tp, p->exp);
                    }
                    if (!isref(tpx1) && ((isconst(tpx1) && !isconst(basetype(sym->tp)->btp)) ||
                                         (isvolatile(tpx1) && !isvolatile(basetype(sym->tp)->btp))))
                        if (basetype(sym->tp)->type != bt_rref)         // converting const lref to rref is ok...
                            if (!isstructured(basetype(sym->tp)->btp))  // structure constructor is ok
                                if (!isarray(tpx1))
                                    error(ERR_REF_INITIALIZATION_DISCARDS_QUALIFIERS);
                    p->tp = sym->tp;
                }
                else if (isstructured(p->tp))
                {
                    if (sym->tp->type == bt_ellipse)
                    {
                        p->exp = exprNode(en_stackblock, p->exp, nullptr);
                        p->exp->size = p->tp;
                    }
                    else
                    {
                        // arithmetic or pointer
                        TYPE* etp = sym->tp;
                        if (cppCast(p->tp, &etp, &p->exp))
                            p->tp = etp;
                    }
                }
                else if (isvoidptr(sym->tp) && p->tp->type == bt_aggregate)
                {
                    LookupSingleAggregate(p->tp, &p->exp);
                }
                else if (ispointer(sym->tp) && ispointer(p->tp))
                {
                    // handle vla to pointer conversion
                    if (p->tp->vla && !sym->tp->vla)
                    {
                        TYPE* tpd1 = Allocate<TYPE>();
                        *tpd1 = *p->tp;
                        tpd1->vla = false;
                        tpd1->array = false;
                        tpd1->size = getSize(bt_pointer);
                        p->tp = tpd1;
                        deref(p->tp, &p->exp);
                    }
                    // handle base class conversion
                    TYPE* tpb = basetype(sym->tp)->btp;
                    TYPE* tpd = basetype(p->tp)->btp;

                    if (Optimizer::cparams.prm_cplusplus && basetype(p->tp)->stringconst && !isconst(basetype(sym->tp)->btp))
                        error(ERR_INVALID_CHARACTER_STRING_CONVERSION);
                    if (!comparetypes(basetype(tpb), basetype(tpd), true))
                    {
                        if (isstructured(tpb) && isstructured(tpd))
                        {
                            p->exp = DerivedToBase(tpb, tpd, p->exp, 0);
                        }
                        p->tp = sym->tp;
                    }
                }
                else if (basetype(sym->tp)->type == bt_memberptr)
                {
                    if (p->exp->type == en_memberptr)
                    {
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, true);
                        p->exp = intNode(en_labcon, lbl);
                        p->exp = exprNode(en_stackblock, p->exp, nullptr);
                        p->exp->size = sym->tp;
                    }
                    else if (isconstzero(p->tp, p->exp) || p->exp->type == en_nullptr)
                    {
                        EXPRESSION* dest = createTemporary(sym->tp, nullptr);
                        p->exp = exprNode(en_blockclear, dest, nullptr);
                        p->exp->size = sym->tp;
                        p->exp = exprNode(en_void, p->exp, dest);
                        p->exp = exprNode(en_stackblock, p->exp, nullptr);
                        p->exp->size = sym->tp;
                    }
                    else if (p->exp->type == en_func && p->exp->v.func->returnSP)
                    {
                        EXPRESSION* dest = anonymousVar(sc_auto, sym->tp);
                        SYMBOL* esp = dest->v.sp;
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, true);
                        esp->sb->stackblock = true;
                        p->exp = intNode(en_labcon, lbl);
                        p->exp = exprNode(en_stackblock, p->exp, nullptr);
                        p->exp->size = sym->tp;
                    }
                    else if (p->exp->type == en_pc)
                    {
                        int lbl = dumpMemberPtr(p->exp->v.sp, sym->tp, true);
                        p->exp = intNode(en_labcon, lbl);
                        p->exp = exprNode(en_stackblock, p->exp, nullptr);
                        p->exp->size = sym->tp;
                    }
                    else
                    {
                        p->exp = exprNode(en_stackblock, p->exp, nullptr);
                        p->exp->size = sym->tp;
                    }
                    p->tp = sym->tp;
                }
                else if (isarithmetic(sym->tp) && isarithmetic(p->tp))
                    if (basetype(sym->tp)->type != basetype(p->tp)->type)
                    {
                        p->tp = sym->tp;
                        cast(p->tp, &p->exp);
                    }
            }
        }
        else if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            if (isref(sym->tp))
            {
                if (comparetypes(sym->tp, p->tp, true))
                {
                    if (isarithmeticconst(p->exp) ||
                        (basetype(sym->tp)->type != bt_rref && !isconst(basetype(sym->tp)->btp) && isconst(p->tp)))
                    {
                        // make numeric temp and perform cast
                        p->exp = createTemporary(sym->tp, p->exp);
                    }
                    else
                    {
                        // pass address
                        EXPRESSION* exp = p->exp;
                        while (castvalue(exp) || exp->type == en_not_lvalue)
                            exp = exp->left;
                        if (exp->type != en_l_ref)
                        {
                            if (!isref(sym->tp) || !isfunction(basetype(sym->tp)->btp))
                            {
                                if (!lvalue(exp))
                                {
                                    // make numeric temp and perform cast
                                    exp = createTemporary(sym->tp, exp);
                                }
                                else
                                {
                                    exp = exp->left;  // take address
                                }
                            }
                            p->exp = exp;
                        }
                        else if (ispointer(p->tp) && isstructured(basetype(p->tp)->btp))
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
                if (basetype(sym->tp)->type == bt___string)
                {
                    if ((basetype(p->tp)->type == bt___string) || (p->exp->type == en_labcon && p->exp->string))
                    {
                        if (p->exp->type == en_labcon)
                            p->exp->type = en_c_string;
                    }
                    else if ((isarray(p->tp) || ispointer(p->tp)) && !basetype(p->tp)->msil &&
                             basetype(basetype(p->tp)->btp)->type == bt_char)
                    {
                        // make a 'string' object and initialize it with the string
                        TYPE* ctype = find_boxed_type(basetype(sym->tp));
                        EXPRESSION *exp1, *exp2;
                        exp1 = exp2 = anonymousVar(sc_auto, &std__string);
                        callConstructorParam(&ctype, &exp2, p->tp, p->exp, true, true, false, false, true);
                        exp2 = exprNode(en_l_string, exp2, nullptr);
                        p->exp = exp2;
                        p->tp = &std__string;
                    }
                }
                else if (basetype(sym->tp)->type == bt___object)
                {
                    if (basetype(p->tp)->type != bt___object && !isstructured(p->tp) && (!isarray(p->tp) || !basetype(p->tp)->msil))
                    {
                        if ((isarray(p->tp) || ispointer(p->tp)) && !basetype(p->tp)->msil &&
                            basetype(basetype(p->tp)->btp)->type == bt_char)
                        {
                            // make a 'string' object and initialize it with the string
                            TYPE* ctype = find_boxed_type(&std__string);
                            EXPRESSION *exp1, *exp2;
                            exp1 = exp2 = anonymousVar(sc_auto, &std__string);
                            callConstructorParam(&ctype, &exp2, p->tp, p->exp, true, true, false, false, true);
                            exp2 = exprNode(en_l_string, exp2, nullptr);
                            p->exp = exp2;
                            p->tp = &std__string;
                        }
                        else
                            p->exp = exprNode(en_x_object, p->exp, nullptr);
                    }
                }
                else if (ismsil(p->tp))
                    ;  // error
                // legacy c language support
                else if (p && p->tp && isstructured(p->tp) && (!basetype(p->tp)->sp->sb->msil || !isconstzero(p->tp, p->exp)))
                {
                    p->exp = exprNode(en_stackblock, p->exp, nullptr);
                    p->exp->size = p->tp;
                }
            }
        }
        else
        {
            // legacy c language support
            if (p && p->tp)
            {
                if (isstructured(p->tp))
                {
                    p->exp = exprNode(en_stackblock, p->exp, nullptr);
                    p->exp->size = p->tp;
                }
                else if (isfloat(sym->tp) || isimaginary(sym->tp) || iscomplex(sym->tp))
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
        INITLIST* p = *itl;
        if (!p->tp || !p->exp)
        {
            p->tp = &stdint;
            p->exp = intNode(en_c_i, 0);
        }
        if (func->sb->msil && p->exp->type == en_labcon && p->exp->string)
        {
            p->exp->type = en_c_string;
            p->tp = &(std__string);
        }
        if (isstructured(p->tp))
        {
            p->exp = exprNode(en_stackblock, p->exp, nullptr);
            p->exp->size = p->tp;
            if (Optimizer::cparams.prm_cplusplus)
            {
                auto dexp = p->exp;
                callDestructor(basetype(p->tp)->sp, nullptr, &dexp, nullptr, true, false, false, true);
                if (dexp)
                {
                    if (!p->destructors)
                        p->destructors = exprListFactory.CreateList();
                    p->destructors->push_front(dexp);
                }
            }
        }
        else if (p->tp->type == bt_float)
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
        for (auto tpl :*tplx)
        {
            auto second = Allocate<TEMPLATEPARAM>();
            *second = *tpl.second;
            rv->push_back(TEMPLATEPARAMPAIR{ tpl.first, second });
            if (tpl.second->type == kw_typename && !tpl.second->packed && tpl.second->byClass.dflt &&
                basetype(tpl.second->byClass.dflt)->type == bt_templateparam)
            {
                *rv->back().second = *basetype(tpl.second->byClass.dflt)->templateParam->second;
                rv->back().first = basetype(tpl.second->byClass.dflt)->templateParam->first;
            }
            else if (tpl.second->type == kw_int && tpl.second->byNonType.dflt && tpl.second->byNonType.dflt->type == en_templateparam)
            {
                *rv->back().second = *tpl.second->byNonType.dflt->v.sp->tp->templateParam->second;
                rv->back().first = tpl.second->byNonType.dflt->v.sp->tp->templateParam->first;
            }
            if (rv->back().first)
            {
                TEMPLATEPARAMPAIR* rv1 = nullptr;
                for (auto&& s : structSyms)
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
                                rv->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
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
LEXLIST* expression_arguments(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    TYPE* tp_cpp = *tp;
    EXPRESSION* exp_cpp = *exp;
    FUNCTIONCALL* funcparams;
    EXPRESSION* exp_in = *exp;
    bool operands = false;
    bool hasThisPtr = false;
    TYPE* initializerListType = nullptr;
    TYPE* initializerListTemplate = nullptr;
    bool initializerRef = false;
    bool addedThisPointer = false;
    bool memberPtr = false;
    if (exp_in->type != en_func || isfuncptr(*tp) || isstructured(*tp))
    {
        TYPE* tpx = *tp;
        SYMBOL* sym;
        funcparams = Allocate<FUNCTIONCALL>();
        if (exp_in->type == en_templateselector)
        {
            auto tsl = exp_in->v.templateSelector;
            SYMBOL* ts = (*tsl)[1].sp;
            SYMBOL* sp = ts;
            if ((!sp->sb || !sp->sb->instantiated) && (*tsl)[1].isTemplate)
            {
                std::list<TEMPLATEPARAMPAIR>* current = (*tsl)[1].templateParams;
                sp = GetClassTemplate(ts, current, true);
            }
            if (sp && sp->tp->type == bt_templateselector)
            {
                TYPE* tp = sp->tp;
                tp = SynthesizeType(tp, nullptr, false);
                if (tp && isstructured(tp))
                    sp = basetype(tp)->sp;
            }
            sym = nullptr;
            if (sp)
            {
                sp = basetype(PerformDeferredInitialization(sp->tp, nullptr))->sp;
                auto find = (*tsl).begin();
                ++find;
                ++find;
                for (; find != (*tsl).end() && sp ; )
                {
                    SYMBOL* spo = sp;
                    if (!isstructured(spo->tp))
                        break;

                    sp = search(spo->tp->syms, find->name);
                    if (!sp)
                    {
                        sp = classdata(find->name, spo, nullptr, false, false);
                        if (sp == (SYMBOL*)-1)
                            sp = nullptr;
                    }
                    if (sp && sp->sb->access != ac_public)
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
            if (tpx->type == bt_templateparam)
            {
                if (tpx->templateParam->second->type == kw_typename && tpx->templateParam->second->byClass.val)
                {
                    tpx = tpx->templateParam->second->byClass.val;
                    if (tpx->sp && tpx->sp->sb->storage_class == sc_typedef)
                        tpx = tpx->btp;
                }
            }
            if (ispointer(tpx))
                tpx = basetype(tpx)->btp;
            sym = basetype(tpx)->sp;
        }
        if (sym)
        {
            funcparams->sp = sym;
            funcparams->functp = sym->tp;
            funcparams->fcall = *exp;
            *exp = varNode(en_func, nullptr);
            (*exp)->v.func = funcparams;
        }
        else if (!templateNestingCount)
            error(ERR_CALL_OF_NONFUNCTION);
    }
    else
    {
        if (isfunction(*tp) && (*exp)->type == en_func && (*exp)->v.func->resolvedCall)
        {
            funcparams = Allocate<FUNCTIONCALL>();
            funcparams->sp = basetype(*tp)->sp;
            funcparams->functp = basetype(*tp);
            funcparams->fcall = *exp;
            if ((*tp)->lref || (*tp)->rref)
                funcparams->fcall = exprNode(en_l_ref, *exp, nullptr);
            *exp = varNode(en_func, nullptr);
            (*exp)->v.func = funcparams;
        }
        else
        {
            SYMBOL* ss = getStructureDeclaration();
            funcparams = exp_in->v.func;
            hasThisPtr = funcparams->thisptr != nullptr;
            if (basetype(*tp)->sp)
                funcparams->sp = basetype(*tp)->sp;
            if (ss)
            {
                funcparams->functp = ss->tp;
            }
        }
    }

    if (/*(!templateNestingCount || instantiatingTemplate) &&*/ funcparams->sp && funcparams->sp->name[0] == '_' &&
        parseBuiltInTypelistFunc(&lex, funcsp, funcparams->sp, tp, exp))
        return lex;

    if (lex)
    {
        lex = getArgs(lex, funcsp, funcparams, closepa, true, flags);
    }
    if (funcparams->astemplate && argumentNesting)
    {
        // if we hit a packed template param here, then this is going to be a candidate
        // for some other function's packed expression
        if (funcparams->templateParams)
            for (auto tl : *funcparams->templateParams)
                if (tl.second->packed && !tl.second->ellipsis)
                    return lex;
    }
    if (*tp)
        getFunctionSP(tp);
    if ((*exp)->type == en_funcret)
    {
        (*exp)->v.func = funcparams;
        *exp = exprNode(en_funcret, *exp, nullptr);
        return lex;
    }
    if ((Optimizer::architecture == ARCHITECTURE_MSIL) && funcparams->sp)
    {
        if (funcparams->sp->sb->storage_class == sc_overloads)
        {
            // note at this pointer the arglist does NOT have the this pointer,
            // it will be added after we select a member function that needs it.
            funcparams->ascall = true;
            SYMBOL* sym =
                GetOverloadedFunction(tp, &funcparams->fcall, funcparams->sp, funcparams, nullptr, true, false, flags);
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
        if (!funcparams->thisptr && funcparams->sp->sb->parentClass && !isfuncptr(funcparams->sp->tp))
        {
            funcparams->thisptr = getMemberBase(funcparams->sp, nullptr, funcsp, false);
            TYPE* tp = MakeType(bt_pointer, funcparams->sp->sb->parentClass->tp);
            if (funcsp)
            {
                if (isconst(funcsp->tp))
                {
                    tp->btp = MakeType(bt_const, tp->btp);
                }
                if (isvolatile(funcsp->tp))
                {
                    tp->btp = MakeType(bt_volatile, tp->btp);
                }
            }
            funcparams->thistp = tp;
            UpdateRootTypes(tp);
            addedThisPointer = true;
        }
        // we may get here with the overload resolution already done, e.g.
        // for operator or cast function calls...
        // also if in a trailing return we want to defer the lookup until later if there are going to be multiple choices...
        if (funcparams->sp->sb->storage_class == sc_overloads &&
            (!parsingTrailingReturnOrUsing || funcparams->sp->tp->type != bt_aggregate ||
             funcparams->sp->tp->syms->size() < 2))
        {
            TYPE* tp1;
            // note at this pointer the arglist does NOT have the this pointer,
            // it will be added after we select a member function that needs it.
            funcparams->ascall = true;
            sym = GetOverloadedFunction(tp, &funcparams->fcall, funcparams->sp, funcparams, nullptr, true, false, flags);
            if (isfunction(*tp))
            {
                if (isstructured(basetype(*tp)->btp))
                {
                    // get rid of any invalid sizing
                    TYPE* tpx = basetype(basetype(*tp)->btp);
                    tpx->size = tpx->sp->tp->size;
                }
            }
            tp1 = *tp;
            while (tp1->btp && tp1->type != bt_bit)
                tp1 = tp1->btp;
            if (sym)
            {
                sym->sb->attribs.inheritable.used = true;
                if (sym->sb->decoratedName[0] == '@' && lex)
                    browse_usage(sym, lex->data->linedata->fileindex);
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
                    for ( ; tpln != tplne && tplo != tploe; ++ tpln, ++tplo)
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
                        else if (funcsp->sb->storage_class == sc_member || funcsp->sb->storage_class == sc_virtual)
                        {
                            funcparams->thisptr = varNode(en_auto, (SYMBOL*)basetype(funcsp->tp)->syms->front());
                            deref(&stdpointer, &funcparams->thisptr);
                            funcparams->thisptr =
                                DerivedToBase(sym->sb->parentClass->tp, basetype(funcparams->thisptr->left->v.sp->tp)->btp,
                                              funcparams->thisptr, _F_VALIDPOINTER);
                            funcparams->thistp = MakeType(bt_pointer, sym->sb->parentClass->tp);
                            if (isconst(sym->tp))
                            {
                                funcparams->thistp->btp = MakeType(bt_const, funcparams->thistp->btp);
                            }
                            if (isvolatile(sym->tp))
                            {
                                funcparams->thistp->btp = MakeType(bt_volatile, funcparams->thistp->btp);
                            }
                            UpdateRootTypes(funcparams->thistp->btp);
                            cppCast(((SYMBOL*)basetype(funcsp->tp)->syms->front())->tp, &funcparams->thistp,
                                    &funcparams->thisptr);
                        }
                    }
                }
            }
        }
        else
        {
            /*
            operands = !ismember(funcparams->sp) && funcparams->thisptr && !addedThisPointer;
            if (!isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, funcparams->sp, funcsp, funcparams->thisptr,
                                        false))
                errorsym(ERR_CANNOT_ACCESS, funcparams->sp);
             */
        }
        if (sym)
        {
            bool test;
            *tp = sym->tp;
            if (hasThisPtr)
            {
                test = isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, sym, funcsp, funcparams->thisptr, false);
            }
            else
            {
                test = isExpressionAccessible(funcsp ? funcsp->sb->parentClass : nullptr, sym, funcsp, funcparams->thisptr, false);
            }
            if (!test)
            {
                errorsym(ERR_CANNOT_ACCESS, sym);
            }
            if (ismember(funcparams->sp))
            {
                if (funcparams->thistp && isconst(basetype(funcparams->thistp)->btp))
                    if (!isconst(*tp))
                        errorsym(ERR_NON_CONST_FUNCTION_CALLED_FOR_CONST_OBJECT, funcparams->sp);
            }
        }
    }
    if (basetype(*tp)->type == bt_memberptr)
    {
        memberPtr = true;
        *tp = basetype(*tp)->btp;
    }
    if (!isfunction(*tp))
    {
        // might be operator ()
        if (Optimizer::cparams.prm_cplusplus)
        {
            if (insertOperatorParams(funcsp, &tp_cpp, &exp_cpp, funcparams, flags))
            {
                hasThisPtr = funcparams->thisptr != nullptr;
                *tp = tp_cpp;
                *exp = exp_cpp;
            }
        }
    }
    {
        SymbolTable<SYMBOL>* temp = basetype(*tp)->syms;
        if (temp)
        {
            auto it = temp->begin();

            if (funcparams->sp && !ismember(funcparams->sp) && !memberPtr)
            {
                if (operands)
                {
                    INITLIST* al = Allocate<INITLIST>();
                    al->exp = funcparams->thisptr;
                    al->tp = funcparams->thistp;
                    if (!funcparams->arguments)
                        funcparams->arguments = initListListFactory.CreateList();
                    funcparams->arguments->push_front(al);
                }
                funcparams->thisptr = nullptr;
            }
            else
            {
                if (!getStructureDeclaration() && !ispointer(tp_cpp) && !hasThisPtr)
                    errorsym(ERR_ACCESS_MEMBER_NO_OBJECT, funcparams->sp);
                operands = false;
            }
            std::list<INITLIST*>::iterator itl, itle = itl;
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
                if (funcparams->sp->sb->attribs.inheritable.linkage3 == lk_noreturn)
                    isCallNoreturnFunction = true;
                TYPE* tp1 = funcparams->sp->tp;
                if (ispointer(tp1))
                    tp1 = basetype(tp1)->btp;
                if (isfunction(tp1))
                {
                    auto itq = basetype(tp1)->syms->begin();
                    if (itq != basetype(tp1)->syms->end())
                    {
                        if ((*itq)->sb->thisPtr)
                            ++itq;
                        TYPE* tp = (*itq)->tp;
                        if (isref(tp))
                        {
                            initializerRef = true;
                            tp = basetype(tp)->btp;
                        }
                        if (isstructured(tp))
                        {
                            SYMBOL* sym = (basetype(tp)->sp);
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
            if (!(flags & _F_SIZEOF))
            {
                std::list<INITLIST*> temp1;
                std::list<INITLIST*>* temp2 = &temp1;
                if (initializerListType)
                {
                    if (funcparams->sp->sb->constexpression)
                    {
                        EXPRESSION* node = Allocate<EXPRESSION>();
                        node->type = en_func;
                        node->v.func = funcparams;
                        if (EvaluateConstexprFunction(node))
                        {
                            *tp = basetype(funcparams->sp->tp)->btp;
                            *exp = node;
                            return lex;
                        }
                    }
                    bool isnested = false;
                    if (funcparams->arguments->front()->nested && funcparams->arguments->front()->nested->front()->nested && !funcparams->arguments->front()->initializer_list)
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

                    CreateInitializerList(funcparams->sp, initializerListTemplate, initializerListType, &temp2, operands,
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
                        AdjustParams(funcparams->sp, itxn, temp->end(), &temp2, operands, true);
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
                    AdjustParams(funcparams->sp, it, temp->end(), &temp2, operands, true);
                    auto itt = temp1.begin();
                    auto itte = temp1.end();
                    for (auto i = 0; i < n; i++)
                        ++itt;
                    if (!funcparams->arguments)
                        funcparams->arguments = initListListFactory.CreateList();
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
                    if (il->tp && isstructured(il->tp) && il->exp)
                    {
                        EXPRESSION* exp = il->exp;
                        if (exp->type == en_not_lvalue)
                            exp = exp->left;
                        if (exp->type == en_auto)
                        {
                            if (!funcparams->callLab)
                                funcparams->callLab = -1;
                        }
                        else if (exp->type == en_thisref)
                        {
                            if (!funcparams->callLab)
                                funcparams->callLab = -1;
                        }
                    }
                }
            }
            if (isfunction(*tp))
            {
                (*exp)->v.func->resolvedCall = true;
                if (funcparams->thisptr && !memberPtr)
                {
                    SYMBOL* base = funcparams->sp->sb->parentClass;
                    SYMBOL* derived = basetype(basetype(funcparams->thistp)->btp)->sp;
                    if (base != derived)
                    {
                        funcparams->thisptr = DerivedToBase(base->tp, derived->tp, funcparams->thisptr, _F_VALIDPOINTER);
                    }
                }
                if (isstructured(basetype(*tp)->btp) || basetype(basetype(*tp)->btp)->type == bt_memberptr)
                {
                    if (!(flags & _F_SIZEOF) && !basetype(basetype(*tp)->btp)->sp->sb->structuredAliasType)
                    {
                        funcparams->returnEXP = anonymousVar(sc_auto, basetype(*tp)->btp);
                        funcparams->returnSP = funcparams->returnEXP->v.sp;
                        if (theCurrentFunc && theCurrentFunc->sb->constexpression)
                            funcparams->returnEXP->v.sp->sb->constexpression = true;
                    }
                }
                funcparams->ascall = true;
                funcparams->functp = *tp;

                *tp = ResolveTemplateSelectors(basetype(*tp)->sp, basetype(*tp)->btp);
                if (isref(*tp))
                {
                    auto reftype = (*tp)->type;
                    *tp = CopyType((*tp)->btp);
                    UpdateRootTypes(*tp);
                    if (reftype == bt_rref)
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
                else if (ispointer(*tp) && (*tp)->array)
                {
                    *tp = CopyType(*tp);
                    UpdateRootTypes(*tp);
                    (*tp)->lref = true;
                    (*tp)->rref = false;
                }
                auto tp1 = tp;
                while (ispointer(*tp1) || basetype(*tp1)->type == bt_memberptr)
                    tp1 = &basetype(*tp1)->btp;
                while ((*tp1)->btp)
                    tp1 = &(*tp1)->btp;
                if (isstructured(*tp1))
                {
                    *tp1 = (*tp1)->sp->tp;
                }
                if (!(flags & _F_SIZEOF))
                {
                    checkArgs(funcparams, funcsp);
                }
                if (funcparams->returnSP)
                {
                    SYMBOL* sym = basetype(funcparams->returnSP->tp)->sp;
                    if (sym->sb->templateLevel && sym->templateParams && !sym->sb->instantiated)
                    {
                        if (!allTemplateArgsSpecified(sym, sym->templateParams))
                            sym = GetClassTemplate(sym, sym->templateParams, false);
                        if (sym && allTemplateArgsSpecified(sym, sym->templateParams))
                            funcparams->returnSP->tp = TemplateClassInstantiate(sym, sym->templateParams, false, sc_global)->tp;
                    }
                }
                if ((!funcparams->novtab || (funcparams->sp && funcparams->sp->sb->ispure)) && funcparams->sp &&
                    funcparams->sp->sb->storage_class == sc_virtual)
                {
                    exp_in = funcparams->thisptr;
                    deref(&stdpointer, &exp_in);
                    exp_in = exprNode(en_add, exp_in, intNode(en_c_i, funcparams->sp->sb->vtaboffset));
                    deref(&stdpointer, &exp_in);
                    funcparams->fcall = exp_in;
                }
                else
                {
                    exp_in = varNode(en_func, nullptr);
                    exp_in->v.func = funcparams;
                    if (exp_in && Optimizer::cparams.prm_cplusplus && funcparams->returnEXP)
                    {
                        if (!basetype(funcparams->returnSP->tp)->sp->sb->trivialCons)
                        {
                            exp_in = exprNode(en_thisref, exp_in, nullptr);
                            exp_in->v.t.thisptr = funcparams->returnEXP;
                            exp_in->v.t.tp = funcparams->returnSP->tp;

                            if (isstructured(funcparams->returnSP->tp))
                            {
                                EXPRESSION* expx = funcparams->returnEXP;
                                callDestructor(basetype(funcparams->returnSP->tp)->sp, nullptr, &expx, nullptr, true, false, true,
                                               true);
                                initInsert(&funcparams->returnSP->sb->dest, funcparams->returnSP->tp, expx, 0, true);
                            }
                        }
                    }
                    if (exp_in)
                    {
                        TYPE* tp = basetype(funcparams->sp->tp)->btp;
                        if ((flags & _F_AMPERSAND) && isarithmetic(tp))
                        {
                            EXPRESSION* rv = anonymousVar(sc_auto, tp);
                            deref(tp, &rv);
                            exp_in = exprNode(en_void, exprNode(en_assign, rv, exp_in), rv);
                            errortype(ERR_CREATE_TEMPORARY, tp, tp);
                        }
                    }
                    if (exp_in)
                        *exp = exp_in;
                }
                if (funcparams->sp && isfunction(funcparams->sp->tp) && isref(basetype(funcparams->sp->tp)->btp))
                {
                    TYPE** tp1;
                    deref(basetype(basetype(funcparams->sp->tp)->btp)->btp, exp);
                    tp1 = &basetype(funcparams->sp->tp)->btp;
                    *tp = CopyType(basetype(*tp1)->btp);
                    UpdateRootTypes(*tp);
                    if (basetype(*tp1)->type == bt_rref)
                    {
                        (*tp)->rref = true;
                        (*tp)->lref = false;
                    }
                    else
                    {
                        (*tp)->lref = true;
                        (*tp)->rref = false;
                    }
                    while (isref(*tp))
                        *tp = basetype(*tp)->btp;
                }
            }
            else if (templateNestingCount && !instantiatingTemplate && (*tp)->type == bt_aggregate)
            {
                *exp = exprNode(en_funcret, *exp, nullptr);
                *tp = MakeType(bt_templatedecltype);
                (*tp)->templateDeclType = *exp;
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
                        if (arg->tp->type == bt_templateparam && arg->tp->templateParam->second->packed)
                            doit = !!arg->tp->templateParam->second->byPack.pack;
                    }
                }
                if (doit && !templateNestingCount && !(flags & _F_INDECLTYPE))
                    error(ERR_CALL_OF_NONFUNCTION);
                *tp = &stdany;
            }
        }
        else
        {
            *tp = &stdint;
            if (!templateNestingCount && !(flags & _F_INDECLTYPE))
                error(ERR_CALL_OF_NONFUNCTION);
        }
    }
    return lex;
}
static LEXLIST* expression_alloca(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        lex = expression_comma(lex, funcsp, nullptr, tp, exp, nullptr, flags);
        if (*tp)
        {
            ResolveTemplateVariable(tp, exp, &stdint, nullptr);
            if (!isint(*tp))
                error(ERR_NEED_INTEGER_EXPRESSION);
            optimize_for_constants(exp);

            funcsp->sb->allocaUsed = true;
            *exp = exprNode(en_alloca, *exp, 0);
            needkw(&lex, closepa);
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
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
        *tp = nullptr;
    }
    return lex;
}
static LEXLIST* expression_offsetof(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    *exp = intNode(en_c_i, 0);
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        // this is naive, not checking the actual type of the sym when these are present
        if (MATCHKW(lex, kw_struct) || MATCHKW(lex, kw_class))
            lex = getsym();
        if (ISID(lex))
        {
            char name[512];
            SYMBOL* sym;
            SymbolTable<SYMBOL>* table;
            SYMBOL* strSym;
            std::list<NAMESPACEVALUEDATA*>* nsv;
            strcpy(name, lex->data->value.s.a);
            lex = tagsearch(lex, name, &sym, &table, &strSym, &nsv, sc_global);
            if (!sym)  // might be a typedef
            {

                sym = namespacesearch(name, globalNameSpace, false, false);
                if (sym && sym->tp->type == bt_typedef)
                {
                    sym = basetype(sym->tp->btp)->sp;
                }
                else
                {
                    sym = nullptr;
                }
            }
            if (sym && isstructured(sym->tp))
            {
                if (needkw(&lex, comma))
                {
                    *tp = sym->tp;
                    do
                    {
                        lex = expression_member(lex, funcsp, tp, exp, ismutable, flags | _F_AMPERSAND);
                        if (lvalue(*exp))
                            *exp = (*exp)->left;
                    } while (MATCHKW(lex, dot));
                }
            }
            else
            {
                error(ERR_CLASS_TYPE_EXPECTED);
            }
        }
        else
        {
            error(ERR_IDENTIFIER_EXPECTED);
        }
        needkw(&lex, closepa);
    }
    *tp = &stdunsigned;

    return lex;
}
static LEXLIST* expression_msilfunc(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    enum e_kw kw = lex->data->kw->key;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        FUNCTIONCALL funcparams;
        memset(&funcparams, 0, sizeof(funcparams));
        lex = getArgs(lex, funcsp, &funcparams, closepa, true, flags);
        int n = funcparams.arguments ? funcparams.arguments->size() : 0;
        if (n > 3)
        {
            *exp = intNode(en_c_i, 0);
            errorstr(ERR_PARAMETER_LIST_TOO_LONG, "__cpblk/__initblk");
        }
        else if (n < 3)
        {
            *exp = intNode(en_c_i, 0);
            errorstr(ERR_PARAMETER_LIST_TOO_SHORT, "__cpblk/__initblk");
        }
        else {
            auto it = funcparams.arguments->begin();
            auto arg1 = *it;
            ++it;
            auto arg2 = *it;
            ++it;
            auto arg3 = *it;
            switch (kw)
            {
            case kw__cpblk:
                if (!ispointer(arg1->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__dest", "__cpblk");
                }
                else if (!ispointer(arg2->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__src", "__cpblk");
                }
                else if (!isint(arg3->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__size", "__cpblk");
                }
                else
                {
                    *exp = exprNode(en_void, arg1->exp, arg2->exp);
                    *exp = exprNode(en__cpblk, *exp, arg3->exp);
                }
                break;
            case kw__initblk:
                if (!ispointer(arg1->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__dest", "__initblk");
                }
                else if (!isint(arg2->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__value", "__initblk");
                }
                else if (!isint(arg3->tp))
                {
                    *exp = intNode(en_c_i, 0);
                    errorstr2(ERR_TYPE_MISMATCH_IN_ARGUMENT, "__size", "__initblk");
                }
                else
                {
                    *exp = exprNode(en_void, arg1->exp,  arg2->exp);
                    *exp = exprNode(en__initblk, *exp, arg3->exp);
                }
                break;
            default:
                *exp = intNode(en_c_i, 0);
                break;
            }
        }
        *tp = &stdpointer;
    }
    else
    {
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
        *tp = nullptr;
    }
    return lex;
}
static LEXLIST* expression_string(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    int elems = 0;
    STRING* data;
    (void)funcsp;

    lex = concatStringsInternal(lex, &data, &elems);
    *exp = stringlit(data);
    if (data->suffix)
    {
        enum e_bt tpb;
        SYMBOL* sym;
        char name[512];
        switch (data->strtype)
        {
            default:
            case l_astr:
                tpb = stdcharptr.btp->type;
                break;
            case l_wstr:
                tpb = stdwcharptr.btp->type;
                break;
            case l_msilstr:
                tpb = std__string.type;
                break;
            case l_ustr:
                tpb = stdchar16tptr.btp->type;
                break;
            case l_Ustr:
                tpb = stdchar32tptr.btp->type;
                break;
        }
        Optimizer::my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], data->suffix);
        sym = LookupSym(name);
        if (sym)
        {
            SYMBOL *sym1 = nullptr, *sym2 = nullptr;
            bool found = false;
            for (auto sym3 : *sym->tp->syms)
            {
                sym2 = sym3->tp->syms->front();
                if (sym3->tp->syms->size() > 1 && ispointer(sym2->tp))
                    if (isconst(sym2->tp->btp) && basetype(sym2->tp->btp)->type == tpb)
                    {
                        sym1 = sym3;
                        found = true;
                        break;
                    }
            }
            if (found)
            {
                FUNCTIONCALL* f = Allocate<FUNCTIONCALL>();
                f->sp = sym1;
                f->functp = sym1->tp;
                f->fcall = varNode(en_pc, sym1);
                f->arguments = initListListFactory.CreateList();
                auto arg = Allocate<INITLIST>();
                arg->tp = sym2->tp;
                arg->exp = *exp;
                f->arguments->push_back(arg);
                arg = Allocate<INITLIST>();
                arg->tp = &stdunsigned;
                arg->exp = intNode(en_c_i, elems);
                f->arguments->push_back(arg);
                *exp = intNode(en_func, 0);
                (*exp)->v.func = f;
                *tp = sym1->tp;
                expression_arguments(nullptr, funcsp, tp, exp, false);
                return lex;
            }
        }
        errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, data->suffix);
    }
    if (data->strtype == l_msilstr)
    {
        *tp = CopyType(&std__string);
        (*tp)->rootType = *tp;
    }
    else
    {
        *tp = MakeType(bt_pointer);
        (*tp)->array = true;
        (*tp)->stringconst = true;
        (*tp)->esize = intNode(en_c_i, elems + 1);
        switch (data->strtype)
        {
            default:
            case l_astr:
                (*tp)->btp = stdcharptr.btp;
                break;
            case l_wstr:
                (*tp)->btp = stdwcharptr.btp;
                break;
            case l_ustr:
                (*tp)->btp = stdchar16tptr.btp;
                break;
            case l_Ustr:
                (*tp)->btp = stdchar32tptr.btp;
                break;
        }
    }
    if ((*tp)->type == bt___string)
        (*tp)->size = 1;
    else
        (*tp)->size = (elems + 1) * (*tp)->btp->size;
    return lex;
}
static bool matchesAttributes(TYPE *tp1, TYPE *tp2)
{
    while (tp1 && tp2)
    {
       if (isconst(tp1) != isconst(tp2) ||
           isvolatile(tp1) != isvolatile(tp2) ||
           isrestrict(tp1) != isrestrict(tp2))
           return false;
       tp1 = basetype(tp1)->btp;
       tp2 = basetype(tp2)->btp;
    }
    return true;
}
static bool sameTypedef(TYPE* tp1, TYPE* tp2)
{
    while (tp1->btp && !tp1->typedefType)
       tp1 = tp1->btp;
    while (tp2->btp && !tp2->typedefType)
       tp2 = tp2->btp;
    return tp1->typedefType == tp2->typedefType;
}
static LEXLIST* expression_generic(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    lex = getsym();
    if (!needkw(&lex, openpa))
    {
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
        *exp = intNode(en_c_i, 0);
        *tp = &stdint;
    }
    else
    {
        TYPE* selectType = nullptr;
        EXPRESSION* throwawayExpression = nullptr;
        lex = expression_assign(lex, funcsp, nullptr, &selectType, &throwawayExpression, nullptr, flags);
        if (MATCHKW(lex, comma))
        {
            bool dflt = false;
            struct genericHold
            {
                struct genericHold* next;
                TYPE* selector;
                TYPE* type;
                EXPRESSION* exp;
            };
            struct genericHold *list = nullptr, **pos = &list;
            struct genericHold* selectedGeneric = nullptr;
            while (MATCHKW(lex, comma))
            {
                struct genericHold *next = Allocate<genericHold>(), *scan;
                lex = getsym();
                if (MATCHKW(lex, kw_default))
                {
                    lex = getsym();
                    if (dflt)
                    {
                        error(ERR_GENERIC_TOO_MANY_DEFAULTS);
                    }
                    dflt = true;
                    next->selector = nullptr;
                }
                else
                {
                    lex = get_type_id(lex, &next->selector, funcsp, sc_cast, false, true, false);
                    if (!next->selector)
                    {
                        error(ERR_GENERIC_MISSING_TYPE);
                        break;
                    }
                }
                if (MATCHKW(lex, colon))
                {
                    lex = getsym();
                    lex = expression_assign(lex, funcsp, nullptr, &next->type, &next->exp, nullptr, flags);
                    if (!next->type)
                    {
                        error(ERR_GENERIC_MISSING_EXPRESSION);
                        break;
                    }
                    scan = list;
                    while (scan)
                    {
                        if (scan->selector && next->selector && comparetypes(next->selector, scan->selector, true))
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
                        if (selectType && next->selector && comparetypes(next->selector, selectType, true))
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
                *exp = intNode(en_c_i, 0);
            }
            if (!needkw(&lex, closepa))
            {
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
        }
        else
        {
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
    }
    return lex;
}
static bool getSuffixedChar(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    char name[512];
    enum e_bt tpb = (*tp)->type;
    SYMBOL* sym;
    Optimizer::my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], lex->data->suffix);
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
            FUNCTIONCALL* f = Allocate<FUNCTIONCALL>();
            f->sp = sym1;
            f->functp = sym1->tp;
            f->fcall = varNode(en_pc, sym1);
            f->arguments = initListListFactory.CreateList();
            auto arg = Allocate<INITLIST>();
            f->arguments->push_back(arg);
            arg->tp = *tp;
            arg->exp = *exp;
            *exp = intNode(en_func, 0);
            (*exp)->v.func = f;
            *tp = sym1->tp;
            expression_arguments(nullptr, funcsp, tp, exp, false);
            return true;
        }
    }
    errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, lex->data->suffix);
    return false;
}
static bool getSuffixedNumber(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    char name[512];
    enum e_bt tpb;
    SYMBOL* sym;
    if (lex->data->type == l_ull)
        tpb = bt_unsigned_long_long;
    else
        tpb = bt_long_double;
    Optimizer::my_sprintf(name, "%s@%s", overloadNameTab[CI_LIT], lex->data->suffix);
    sym = LookupSym(name);
    if (sym)
    {
        // look for parameter of type unsigned long long or long double
        SYMBOL *sym1 = nullptr, *sym2 = nullptr;
        auto found = false;
        for (auto sp : *sym->tp->syms)
        {
            sym1 = sp;
            sym2 = sym2->tp->syms->front();
            if (sym1->tp->syms->size() == 1 && sym2->tp->type == tpb)
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            FUNCTIONCALL* f = Allocate<FUNCTIONCALL>();
            f->sp = sym1;
            f->functp = sym1->tp;
            f->fcall = varNode(en_pc, sym1);
            f->arguments = initListListFactory.CreateList();
            auto arg = Allocate<INITLIST>();
            f->arguments->push_back(arg);
            if (lex->data->type == l_ull)
            {
                arg->tp = &stdunsignedlonglong;
                arg->exp = intNode(en_c_ull, lex->data->value.i);
            }
            else
            {
                arg->tp = &stdlongdouble;
                arg->exp = intNode(en_c_ld, 0);
                arg->exp->v.f = Allocate<FPF>();
                *arg->exp->v.f = *lex->data->value.f;
            }
            *exp = intNode(en_func, 0);
            (*exp)->v.func = f;
            *tp = sym1->tp;
            expression_arguments(nullptr, funcsp, tp, exp, false);
            return true;
        }
        else
        {
            bool found = false;
            // not found, look for parameter of type const char *
            for (auto sp : *sym->tp->syms)
            {
                TYPE* tpx;
                sym1 = sp;
                sym2 = sym1->tp->syms->front();
                tpx = sym2->tp;
                if (sym1->tp->syms->size() == 1 && ispointer(tpx))
                {
                    tpx = basetype(tpx)->btp;
                    if (isconst(tpx) && basetype(tpx)->type == bt_char)
                    {
                        found = true;
                        break;
                    }
                }
            }
            if (found)
            {
                FUNCTIONCALL* f = Allocate<FUNCTIONCALL>();
                STRING* data;
                int i;
                f->sp = sym1;
                f->functp = sym1->tp;
                f->fcall = varNode(en_pc, sym1);
                f->arguments = initListListFactory.CreateList();
                auto arg = Allocate<INITLIST>();
                f->arguments->push_back(arg);
                arg->tp = &stdcharptr;
                arg->tp->size = (strlen(lex->data->litaslit) + 1) * arg->tp->btp->size;
                data = Allocate<STRING>();
                data->strtype = l_astr;
                data->size = 1;
                data->pointers = Allocate<Optimizer::SLCHAR*>();
                data->pointers[0] = Allocate<Optimizer::SLCHAR>();
                data->pointers[0]->count = strlen(lex->data->litaslit);
                data->pointers[0]->str = Allocate<LCHAR>(data->pointers[0]->count + 1);
                for (i = 0; i < data->pointers[0]->count; i++)
                    data->pointers[0]->str[i] = lex->data->litaslit[i];
                arg->exp = stringlit(data);
                *exp = intNode(en_func, 0);
                (*exp)->v.func = f;
                *tp = sym1->tp;
                expression_arguments(nullptr, funcsp, tp, exp, false);
                return true;
            }
        }
    }
    errorstr(ERR_COULD_NOT_FIND_A_MATCH_FOR_LITERAL_SUFFIX, lex->data->suffix);
    return false;
}
static Parser::LEXLIST* atomic_modify_specific_op(Parser::LEXLIST* lex, Parser::SYMBOL* funcsp, Parser::TYPE** tp,
                                                  Parser::TYPE** typf, Parser::ATOMICDATA* d, int flags, Parser::e_kw function,
                                                  bool fetch_first)
{
    Parser::TYPE* tpf = *typf;
    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->address, nullptr, flags);
    if (tpf)
    {
        if (!ispointer(tpf))
        {
            error(ERR_DEREF);
            d->tp = *tp = &stdint;
        }
        else
        {
            d->tp = *tp = basetype(tpf)->btp;
        }
    }
    d->third = intNode(en_c_i, function);
    if (needkw(&lex, comma))
    {
        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->value, nullptr, flags);
        if (!comparetypes(tpf, *tp, false))
        {
            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
        }
        if (ispointer(*tp) && (function == asplus || function == asminus))
        {
            d->value = exprNode(en_mul, d->value, intNode(en_c_i, (*tp)->size));
        }
    }
    else
    {
        *tp = &stdint;
        d->value = intNode(en_c_i, 0);
    }
    if (needkw(&lex, comma))
    {
        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
    }
    else
    {
        tpf = &stdint;
        d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
    }
    d->atomicOp = fetch_first ? Optimizer::e_ao::ao_fetch_modify : Optimizer::e_ao::ao_modify_fetch;
    if (!d->memoryOrder2)
        d->memoryOrder2 = d->memoryOrder1;
    return lex;
}
static LEXLIST* expression_atomic_func(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    enum e_kw kw = KW(lex);
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        if (kw == kw_c11_atomic_is_lock_free)
        {
            lex = optimized_expression(lex, funcsp, nullptr, tp, exp, false);
            if (!isint(*tp) || !isintconst(*exp))
                error(ERR_NEED_INTEGER_TYPE);
            *tp = &stdint;
            bool found = (*exp)->v.i > 0 && (*exp)->v.i <= Optimizer::chosenAssembler->arch->isLockFreeSize &&
                         ((*exp)->v.i & ((*exp)->v.i - 1)) == 0;
            *exp = intNode(en_c_i, found);
            needkw(&lex, closepa);
        }
        else if (kw == kw_c11_atomic_init)
        {
            lex = expression_assign(lex, funcsp, nullptr, tp, exp, nullptr, flags);
            if (!*tp)
                error(ERR_EXPRESSION_SYNTAX);
            if (MATCHKW(lex, comma))  // atomic_init
            {
                TYPE* tp1;
                EXPRESSION* exp1;
                lex = getsym();
                lex = expression_assign(lex, funcsp, nullptr, &tp1, &exp1, nullptr, flags);
                if (*tp && tp1)
                {
                    ATOMICDATA* d;
                    d = Allocate<ATOMICDATA>();
                    if (!ispointer(*tp))
                    {
                        error(ERR_DEREF);
                        d->tp = *tp = &stdint;
                    }
                    else
                    {
                        TYPE* tp2 = *tp;
                        d->tp = *tp = basetype(*tp)->btp;
                        if (!comparetypes(*tp, tp1, false))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        tp1 = tp2;
                    }
                    d->address = *exp;
                    d->value = exp1;
                    d->atomicOp = Optimizer::ao_init;
                    *exp = exprNode(en_atomic, nullptr, nullptr);
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
            needkw(&lex, closepa);
        }
        else
        {
            TYPE *tpf = nullptr, *tpf1;
            ATOMICDATA* d;
            d = Allocate<ATOMICDATA>();
            switch (kw)
            {
                case kw_atomic_kill_dependency:
                    lex = expression_assign(lex, funcsp, nullptr, &d->tp, &d->address, nullptr, flags);
                    *tp = d->tp;
                    if (!*tp)
                        error(ERR_EXPRESSION_SYNTAX);
                    d->atomicOp = Optimizer::ao_kill_dependency;
                    break;
                case kw_atomic_flag_test_set:

                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->flg, nullptr, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                        }
                        deref(&stdint, &d->flg);
                        d->tp = *tp = &stdint;
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_flag_set_test;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    break;
                case kw_atomic_flag_clear:
                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->flg, nullptr, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                        }
                        deref(&stdint, &d->flg);
                        d->tp = *tp = &stdint;
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_flag_clear;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    *tp = &stdvoid;
                    break;
                case kw_c11_atomic_thread_fence:
                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    d->memoryOrder1 = exprNode(en_add, d->memoryOrder1, intNode(en_c_i, 0x80));
                    d->atomicOp = Optimizer::ao_thread_fence;
                    *tp = &stdvoid;
                    break;
                case kw_c11_atomic_signal_fence:
                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    d->memoryOrder1 = exprNode(en_add, d->memoryOrder1, intNode(en_c_i, 0x80));
                    d->atomicOp = Optimizer::ao_signal_fence;
                    *tp = &stdvoid;
                    break;
                case kw_c11_atomic_load:
                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_load;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    break;
                case kw_c11_atomic_store:
                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        TYPE* tpf1;
                        lex = expression_assign(lex, funcsp, nullptr, &tpf1, &d->value, nullptr, flags);
                        if (!comparetypes(d->tp, tpf1, false))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->value = intNode(en_c_i, 0);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = Optimizer::ao_store;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    d->memoryOrder1 = exprNode(en_add, d->memoryOrder1, intNode(en_c_i, 0x80));
                    break;
                    // there's extremely likely a better way to do this, maybe a lookup table for these and using that...
                case kw_c11_atomic_xchg:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, assign, true);
                    break;
                case kw_c11_atomic_ftchadd:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asplus, true);
                    break;
                case kw_c11_atomic_ftchsub:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asminus, true);
                    break;
                case kw_c11_atomic_ftchand:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asand, true);
                    break;
                case kw_c11_atomic_ftchor:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asor, true);
                    break;
                case kw_c11_atomic_ftchxor:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asxor, true);
                    break;
                case kw_atomic_addftch:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asplus, false);
                    break;
                case kw_atomic_subftch:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asminus, false);
                    break;
                case kw_atomic_andftch:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asand, false);
                    break;
                case kw_atomic_orftch:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asor, false);
                    break;
                case kw_atomic_xorftch:
                    lex = atomic_modify_specific_op(lex, funcsp, tp, &tpf, d, flags, asxor, false);
                    break;
                case kw_atomic_cmpxchg_n: {
                    bool weak = false;
                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf1, &d->third, nullptr, flags);
                        if (!comparetypes(tpf, tpf1, false))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->third = intNode(en_c_i, 0);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->value, nullptr, flags);
                        if (!comparetypes(tpf, *tp, false))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        if (!isstructured(*tp))
                        {
                            cast(*tp, &d->value);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->value = intNode(en_c_i, 0);
                    }
                    if (needkw(&lex, comma))
                    {
                        TYPE* weak_type = nullptr;
                        EXPRESSION* weak_expr = nullptr;

                        lex = optimized_expression(lex, funcsp, nullptr, &weak_type, &weak_expr, false);
                        if (!isintconst(weak_expr))
                        {
                            error(ERR_NEED_INTEGER_TYPE);
                        }
                        weak = (bool)weak_expr->v.i;
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf1, &d->memoryOrder2, nullptr, flags);
                    }
                    else
                    {
                        tpf1 = &stdint;
                        d->memoryOrder2 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = weak ? Optimizer::ao_cmpxchgweak : Optimizer::ao_cmpxchgstrong;
                    if (!d->memoryOrder2)
                    {
                        d->memoryOrder2 = d->memoryOrder1;
                    }
                    *tp = &stdint;
                    break;
                }
                case kw_c11_atomic_cmpxchg_weak:
                case kw_c11_atomic_cmpxchg_strong:
                    lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->address, nullptr, flags);
                    if (tpf)
                    {
                        if (!ispointer(tpf))
                        {
                            error(ERR_DEREF);
                            d->tp = *tp = &stdint;
                        }
                        else
                        {
                            d->tp = *tp = basetype(tpf)->btp;
                        }
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf1, &d->third, nullptr, flags);
                        if (!comparetypes(tpf, tpf1, false))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                    }
                    else
                    {
                        *tp = &stdint;
                        d->third = intNode(en_c_i, 0);
                    }

                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->value, nullptr, flags);
                        if (!comparetypes(tpf, *tp, false))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                        }
                        if (!isstructured(*tp))
                            cast(*tp, &d->value);
                    }
                    else
                    {
                        *tp = &stdint;
                        d->value = intNode(en_c_i, 0);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf, &d->memoryOrder1, nullptr, flags);
                    }
                    else
                    {
                        tpf = &stdint;
                        d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    if (needkw(&lex, comma))
                    {
                        lex = expression_assign(lex, funcsp, nullptr, &tpf1, &d->memoryOrder2, nullptr, flags);
                    }
                    else
                    {
                        tpf1 = &stdint;
                        d->memoryOrder2 = intNode(en_c_i, Optimizer::mo_seq_cst);
                    }
                    d->atomicOp = kw == kw_c11_atomic_cmpxchg_weak ? Optimizer::ao_cmpxchgweak : Optimizer::ao_cmpxchgstrong;
                    if (!d->memoryOrder2)
                        d->memoryOrder2 = d->memoryOrder1;
                    *tp = &stdint;
                    break;
                default:
                    break;
            }
            if (tpf && !isint(tpf))
            {
                error(ERR_NEED_INTEGER_TYPE);
                d->memoryOrder1 = *exp = intNode(en_c_i, Optimizer::mo_relaxed);
            }
            if (d->memoryOrder1)
                optimize_for_constants(&d->memoryOrder1);
            if (!needkw(&lex, closepa))
            {
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
#ifdef NEED_CONST_MO
            if (d->memoryOrder1 && !isintconst(d->memoryOrder1))
                d->memoryOrder1 = intNode(en_c_i, Optimizer::mo_seq_cst);
            if (d->memoryOrder2 && !isintconst(d->memoryOrder2))
                d->memoryOrder2 = intNode(en_c_i, Optimizer::mo_seq_cst);
#endif
            *exp = exprNode(en_atomic, nullptr, nullptr);
            (*exp)->v.ad = d;
        }
    }
    else
    {
        *tp = &stdint;
        *exp = intNode(en_c_i, Optimizer::mo_relaxed);
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    }
    return lex;
}
static LEXLIST* expression___typeid(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        lex = expression_comma(lex, funcsp, nullptr, tp, exp, nullptr, _F_SIZEOF);
        if (!*tp)
        {
            error(ERR_TYPE_NAME_EXPECTED);
            *exp = intNode(en_c_i, 0);
        }
        else
        {
            int id;
            TYPE* tp1 = *tp;
            if (isref(tp1))
                *tp = basetype(tp1)->btp;  // DAL fixed
            switch (tp1->type)
            {

                case bt_bool:
                case bt_char:
                    id = -1;
                    break;
                case bt_short:
                    id = -2;
                    break;
                case bt_int:
                    id = -3;
                    break;
                case bt_long:
                    id = -4;
                    break;
                case bt_long_long:
                    id = -5;
                    break;
                case bt_unsigned_char:
                    id = 1;
                    break;
                case bt_unsigned_short:
                case bt_wchar_t:
                    id = 2;
                    break;
                case bt_unsigned:
                    id = 3;
                    break;
                case bt_unsigned_long:
                    id = 4;
                    break;
                case bt_unsigned_long_long:
                    id = 5;
                    break;
                case bt_float:
                    id = 7;
                    break;
                case bt_double:
                    id = 8;
                    break;
                case bt_long_double:
                    id = 10;
                    break;
                case bt_float_imaginary:
                    id = 15;
                    break;
                case bt_double_imaginary:
                    id = 16;
                    break;
                case bt_long_double_imaginary:
                    id = 17;
                    break;
                case bt_float_complex:
                    id = 20;
                    break;
                case bt_double_complex:
                    id = 21;
                    break;
                case bt_long_double_complex:
                    id = 22;
                    break;
                default:
                    id = 100000;
                    break;
            }
            *exp = intNode(en_c_i, id);
        }
        *tp = &stdint;
        needkw(&lex, closepa);
    }
    return lex;
}
static LEXLIST* expression_statement(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
	// gcc extension
     if (expressionStatements.size())
     {
         expressionReturns.push_back(std::pair<EXPRESSION*, TYPE*>(nullptr, nullptr));
         lex = compound(lex, funcsp, *expressionStatements.top(), false);
         std::pair<EXPRESSION*, TYPE*> rv = std::move(expressionReturns.back());
         expressionReturns.pop_back();
         *tp = rv.second;
         *exp = rv.first;
         if (!*tp)
             *tp = &stdvoid;
         if (!*exp)
             *exp = intNode(en_c_i, 0);
     }
     else
     {
         errskim(&lex, skim_end);
         if (lex)
             needkw(&lex, end);
         *tp = &stdvoid;
         *exp = intNode(en_c_i, 0);
     }
     return lex;
}

static LEXLIST* expression_primary(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    switch (lex ? lex->data->type : l_none)
    {
        case l_id:
            lex = variableName(lex, funcsp, atp, tp, exp, ismutable, flags);
            break;
        case l_kw:
            switch (KW(lex))
            {
                SYMBOL* sym;
                case openbr:
                    if (Optimizer::cparams.prm_cplusplus)
                        lex = expression_lambda(lex, funcsp, atp, tp, exp, flags);
                    break;
                case classsel:
                case kw_operator:
                case kw_decltype:
                    lex = variableName(lex, funcsp, atp, tp, exp, ismutable, flags);
                    break;
                case kw_nullptr:
                    *exp = intNode(en_nullptr, 0);
                    *tp = &stdnullpointer;
                    lex = getsym();
                    break;
                case kw_this:
                    if (lambdas.size())
                    {
                        lambda_capture(nullptr, cmThis, false);
                        if (lambdas.front()->captureThis)
                        {
                            SYMBOL* ths = search(lambdas.front()->cls->tp->syms, "$this");
                            if (ths)
                            {
                                *tp = MakeType(bt_pointer, basetype(lambdas.front()->lthis->tp)->btp);
                                *exp = varNode(en_auto, (SYMBOL*)basetype(funcsp->tp)->syms->front());  // this ptr
                                deref(&stdpointer, exp);
                                *exp = exprNode(en_structadd, *exp, intNode(en_c_i, ths->sb->offset));
                                deref(&stdpointer, exp);
                            }
                            else
                            {
                                diag("expression_primary: missing lambda this");
                            }
                        }
                        else
                        {
                            *exp = intNode(en_c_i, 0);
                            *tp = &stdint;
                        }
                    }
                    else if (getStructureDeclaration() && funcsp && funcsp->sb->parentClass)
                    {
                        getThisType(funcsp, tp);
                        *exp = varNode(en_auto, (SYMBOL*)basetype(funcsp->tp)->syms->front());  // this ptr
                        deref(&stdpointer, exp);
                    }
                    else
                    {
                        *exp = intNode(en_c_i, 0);
                        *tp = &stdint;
                        error(ERR_THIS_MEMBER_FUNC);
                    }
                    lex = getsym();
                    break;
                case kw___I:
                    *exp = intNode(en_c_i, 0);
                    (*exp)->type = en_c_fi;
                    (*exp)->v.f = Allocate<FPF>();
                    *(*exp)->v.f = (long long)1;
                    *tp = &stdfloatimaginary;
                    (*exp)->pragmas = preProcessor->GetStdPragmas();
                    lex = getsym();
                    break;
                case kw___offsetof:
                    lex = expression_offsetof(lex, funcsp, tp, exp, ismutable, flags);
                    break;
                case kw__volatile:
                    lex = getsym();
                    lex = expression_no_comma(lex, funcsp, nullptr, tp, exp, nullptr, 0);
                    *exp = GetSymRef(*exp);
                    if (!*exp)
                    {
                        error(ERR_IDENTIFIER_EXPECTED);
                        *exp = intNode(en_c_i, 0);
                    }
                    else
                    {
                        if (!isvolatile((*exp)->v.sp->tp))
                        {
                            (*exp)->v.sp->tp = MakeType(bt_volatile, (*exp)->v.sp->tp);
                        }
                    }
                    *exp = intNode(en_c_i, 0);
                    *tp = &stdvoid;
                    break;
                case land:
                    // computed goto: take the address of a label
                    lex = getsym();
                    if (!ISID(lex))
                    {
                        *exp = intNode(en_c_i, 0);
                        error(ERR_IDENTIFIER_EXPECTED);
                    }
                    else
                    {
                        *exp = intNode(en_labcon, GetLabelValue(lex, nullptr, nullptr));
                        (*exp)->adjustLabel = true;
                        lex = getsym();
                    }
                    *tp = &stdpointer;
                    break;
                case kw_true:
                    lex = getsym();
                    *exp = intNode(en_c_i, 1);
                    (*exp)->type = en_c_bool;
                    *tp = &stdbool;
                    break;
                case kw_false:
                    lex = getsym();
                    *exp = intNode(en_c_i, 0);
                    (*exp)->type = en_c_bool;
                    *tp = &stdbool;
                    return lex;
                case kw_alloca:
                    lex = expression_alloca(lex, funcsp, tp, exp, flags);
                    return lex;
                case kw__initblk:
                case kw__cpblk:
                    lex = expression_msilfunc(lex, funcsp, tp, exp, flags);
                    break;
                case openpa: {
                    lex = getsym();
                    if (MATCHKW(lex, begin))
                    {
                        lex = expression_statement(lex, funcsp, nullptr, tp, exp, ismutable, flags);
                        needkw(&lex, closepa);
                        break;
                    }
                    else
                    {
                        LEXLIST* start = lex;
                        lex = expression_comma(lex, funcsp, nullptr, tp, exp, ismutable, flags & ~(_F_INTEMPLATEPARAMS | _F_SELECTOR));
                        if (!*tp)
                            error(ERR_EXPRESSION_SYNTAX);
                        if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, ellipse))
                        {
                            // lose p
                            lex = getsym();
                            if (!templateNestingCount && *exp && (*exp)->type != en_packedempty)
                            {
                                if (!isstructured(*tp) && !(*tp)->templateParam)
                                    checkPackedExpression(*exp);
                                // this is going to presume that the expression involved
                                // is not too long to be cached by the LEXLIST mechanism.
                                int oldPack = packIndex;
                                int count = 0;
                                SYMBOL* arg[200];
                                GatherPackedVars(&count, arg, *exp);
                                expandingParams++;
                                if (count)
                                {
                                    *exp = nullptr;
                                    EXPRESSION** next = exp;
                                    int i;
                                    int n = CountPacks(arg[0]->tp->templateParam->second->byPack.pack);
                                    for (i = 0; i < n; i++)
                                    {
                                        INITLIST* p = Allocate<INITLIST>();
                                        LEXLIST* lex = SetAlternateLex(start);
                                        TYPE* tp1;
                                        EXPRESSION* exp1;
                                        packIndex = i;
                                        expression_comma(lex, funcsp, nullptr, &tp1, &exp1, nullptr, _F_PACKABLE);
                                        SetAlternateLex(nullptr);
                                        if (tp1)
                                        {
                                            if (!*next)
                                            {
                                                *next = exp1;
                                            }
                                            else
                                            {
                                                *next = exprNode(en_void, *next, exp1);
                                                next = &(*next)->right;
                                            }
                                        }
                                    }
                                }
                                expandingParams--;
                                packIndex = oldPack;
                            }
                        }
                        else
                        {
                            if (argumentNesting <= 1)
                                checkUnpackedExpression(*exp);
                        }
                        needkw(&lex, closepa);
                        break;
                    }
                }  
                case kw___func__:
                    *tp = &std__func__;
                    if (!funcsp->sb->__func__label || Optimizer::msilstrings)
                    {
                        LCHAR buf[256], *q = buf;
                        const char* p = funcsp->name;
                        STRING* string;
                        string = Allocate<STRING>();
                        string->strtype = l_astr;
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
                        *exp = intNode(en_labcon, funcsp->sb->__func__label);
                        (*exp)->altdata = intNode(en_c_i, l_astr);
                    }
                    lex = getsym();
                    break;
                case kw__uuidof:
                    lex = getsym();
                    needkw(&lex, openpa);
                    if (ISID(lex))
                    {
                        sym = tsearch(lex->data->value.s.a);
                        lex = getsym();
                    }
                    else
                    {
                        TYPE* tp1;
                        EXPRESSION* exp1;
                        lex = expression_no_comma(lex, funcsp, nullptr, &tp1, &exp1, nullptr, 0);
                        if (tp1 && isstructured(tp1))
                        {
                            sym = basetype(tp1)->sp;
                        }
                        else
                        {
                            sym = nullptr;
                        }
                    }
                    needkw(&lex, closepa);
                    *exp = GetUUIDData(sym);
                    *tp = &stdpointer;
                    break;
                case kw___va_typeof__:
                    lex = getsym();
                    if (MATCHKW(lex, openpa))
                    {
                        lex = getsym();
                        if (startOfType(lex, nullptr, false))
                        {
                            SYMBOL* sym;
                            lex = get_type_id(lex, tp, funcsp, sc_cast, false, true, false);
                            (*tp)->used = true;
                            needkw(&lex, closepa);
                            // don't enter in table, this is purely so we can cache the type info
                            sym = makeID(sc_auto, *tp, nullptr, AnonymousName());
                            sym->sb->va_typeof = true;
                            *exp = varNode(en_auto, sym);
                            break;
                        }
                    }
                    error(ERR_TYPE_NAME_EXPECTED);
                    *exp = intNode(en_c_i, 0);
                    *tp = &stdint;
                    break;
                case kw___typeid:
                    lex = expression___typeid(lex, funcsp, tp, exp);
                    break;
                case kw_D0:
                case kw_D1:
                case kw_D2:
                case kw_D3:
                case kw_D4:
                case kw_D5:
                case kw_D6:
                case kw_D7:
                case kw_A0:
                case kw_A1:
                case kw_A2:
                case kw_A3:
                case kw_A4:
                case kw_A5:
                case kw_A6:
                case kw_A7:
                    *exp = intNode(en_c_i, 0);
                    *tp = &stdint;
                    lex = getsym();
                    break;
                case kw__NAN:
                    *exp = intNode(en_c_i, 0);
                    (*exp)->type = en_c_f;
                    (*exp)->v.f = Allocate<FPF>();
                    (*exp)->v.f->SetNaN();
                    lex = getsym();
                    *tp = &stdfloat;
                    break;
                case kw__INF:
                    *exp = intNode(en_c_i, 0);
                    (*exp)->type = en_c_f;
                    (*exp)->v.f = Allocate<FPF>();
                    (*exp)->v.f->SetInfinity(0);
                    lex = getsym();
                    *tp = &stdfloat;
                    break;
                case kw_generic:
                    lex = expression_generic(lex, funcsp, tp, exp, flags);
                    break;
                case kw_atomic_addftch:
                case kw_atomic_subftch:
                case kw_atomic_andftch:
                case kw_atomic_xorftch:
                case kw_atomic_orftch:
                case kw_atomic_cmpxchg_n:
                case kw_atomic_flag_test_set:
                case kw_atomic_flag_clear:
                case kw_atomic_kill_dependency:
                case kw_c11_atomic_is_lock_free:
                case kw_c11_atomic_load:
                case kw_c11_atomic_store:
                case kw_c11_atomic_cmpxchg_strong:
                case kw_c11_atomic_cmpxchg_weak:
                case kw_c11_atomic_ftchadd:
                case kw_c11_atomic_ftchsub:
                case kw_c11_atomic_xchg:
                case kw_c11_atomic_ftchand:
                case kw_c11_atomic_ftchor:
                case kw_c11_atomic_ftchxor:
                case kw_c11_atomic_init:
                case kw_c11_atomic_signal_fence:
                case kw_c11_atomic_thread_fence:
                    lex = expression_atomic_func(lex, funcsp, tp, exp, flags);
                    break;
                case kw_typename:
                    *tp = nullptr;
                    lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
                    break;
                default:
                    /*					errorstr(ERR_UNEXPECTED_KEYWORD, lex->data->kw->name); */
                    *tp = nullptr;
                    *exp = intNode(en_c_i, 0);
                    lex = getsym();
                    break;
            }
            break;
        case l_i:
            *exp = intNode(en_c_i, lex->data->value.i);
            *tp = &stdint;
            lex = getsym();
            break;
        case l_ui:
            *exp = intNode(en_c_ui, lex->data->value.i);
            (*exp)->type = en_c_ui;
            *tp = &stdunsigned;
            lex = getsym();
            break;
        case l_l:
            *exp = intNode(en_c_l, lex->data->value.i);
            (*exp)->type = en_c_l;
            *tp = &stdlong;
            lex = getsym();
            break;
        case l_ul:
            *exp = intNode(en_c_ul, lex->data->value.i);
            (*exp)->type = en_c_ul;
            *tp = &stdunsignedlong;
            lex = getsym();
            break;
        case l_ll:
            *exp = intNode(en_c_ll, lex->data->value.i);
            (*exp)->type = en_c_ll;
            *tp = &stdlonglong;
            lex = getsym();
            break;
        case l_ull:
            if (!lex->data->suffix || !getSuffixedNumber(lex, funcsp, tp, exp))
            {
                *exp = intNode(en_c_ull, lex->data->value.i);
                (*exp)->type = en_c_ull;
                *tp = &stdunsignedlonglong;
            }
            lex = getsym();
            break;
        case l_f:
            *exp = intNode(en_c_f, 0);
            (*exp)->v.f = Allocate<FPF>();
            *(*exp)->v.f = *lex->data->value.f;
            (*exp)->pragmas = preProcessor->GetStdPragmas();
            *tp = &stdfloat;
            lex = getsym();
            break;
        case l_d:
            *exp = intNode(en_c_d, 0);
            (*exp)->v.f = Allocate<FPF>();
            *(*exp)->v.f = *lex->data->value.f;
            (*exp)->pragmas = preProcessor->GetStdPragmas();
            *tp = &stddouble;
            lex = getsym();
            break;
        case l_ld:
            if (!lex->data->suffix || !getSuffixedNumber(lex, funcsp, tp, exp))
            {
                *exp = intNode(en_c_ld, 0);
                (*exp)->v.f = Allocate<FPF>();
                *(*exp)->v.f = *lex->data->value.f;
                (*exp)->pragmas = preProcessor->GetStdPragmas();
                *tp = &stdlongdouble;
            }
            lex = getsym();
            break;
        case l_astr:
        case l_wstr:
        case l_ustr:
        case l_Ustr:
        case l_msilstr:
            lex = expression_string(lex, funcsp, tp, exp);
            break;
        case l_wchr:
            *exp = intNode(en_c_wc, lex->data->value.i);
            (*exp)->type = en_c_wc;
            *tp = &stdwidechar;
            if (lex->data->suffix)
                getSuffixedChar(lex, funcsp, tp, exp);
            lex = getsym();
            break;
        case l_achr:
            *exp = intNode(en_c_c, lex->data->value.i);
            (*exp)->type = en_c_c;
            *tp = &stdchar;
            if (lex->data->suffix)
                getSuffixedChar(lex, funcsp, tp, exp);
            lex = getsym();
            break;
        case l_uchr:
            *exp = intNode(en_c_u16, lex->data->value.i);
            (*exp)->type = en_c_u16;
            *tp = stdchar16tptr.btp;
            if (lex->data->suffix)
                getSuffixedChar(lex, funcsp, tp, exp);
            lex = getsym();
            break;
        case l_Uchr:
            *exp = intNode(en_c_u32, lex->data->value.i);
            (*exp)->type = en_c_u32;
            *tp = stdchar32tptr.btp;
            if (lex->data->suffix)
                getSuffixedChar(lex, funcsp, tp, exp);
            lex = getsym();
            break;
        default:
            *tp = nullptr;
            *exp = intNode(en_c_i, 0);
            break;
    }
    return lex;
}
static EXPRESSION* nodeSizeof(TYPE* tp, EXPRESSION* exp, int flags)
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
        if (lvalue(exp) && exp->left->type == en_bits)
            error(ERR_SIZEOF_BITFIELD);
    }
    if (Optimizer::cparams.prm_cplusplus && tp->type == bt_enum && !tp->fixed)
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

        while (tp->type == bt_pointer)
        {
            exp = exprNode(en_mul, exp, tp->esize);
            tp = basetype(tp)->btp;
        }
        exp = exprNode(en_mul, exp, intNode(en_c_i, tp->size));
    }
    else if (!isarray(tp) && Optimizer::architecture == ARCHITECTURE_MSIL)
    {
        exp = exprNode(en__sizeof, typeNode(tp), nullptr);
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
                        exp = intNode(en_c_i, cache->sb->offset);
                }
            }
        }
    }
    if (!exp)
    {
        // array which is an argument has different sizeof requirements
        if ((flags & _F_SIZEOF) && tp->type == bt_pointer && tp->array && exp_in && exp_in->type == en_l_p &&
            exp_in->left->type == en_auto && exp_in->left->v.sp->sb->storage_class == sc_parameter)
            exp = intNode(en_c_i, getSize(bt_pointer));
        else
            exp = intNode(en_c_i, tp->size);
    }
    return exp;
}
static int widelen(LCHAR* s)
{
    int rv = 0;
    while (*s++)
        rv++;
    return rv;
}
static LEXLIST* expression_sizeof(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    int paren = false;
    *exp = nullptr;
    lex = getsym();
    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, ellipse))
    {
        lex = getsym();
        if (MATCHKW(lex, openpa))
        {
            paren = true;
            lex = getsym();
        }
        if (!ISID(lex))
        {
            error(ERR_SIZEOFELLIPSE_NEEDS_TEMPLATE_PACK);
            errskim(&lex, skim_closepa);
        }
        else
        {
            EXPRESSION* exp1 = nullptr;
            lex = variableName(lex, funcsp, nullptr, tp, &exp1, nullptr, _F_PACKABLE | _F_SIZEOF);
            if (!exp1 || !exp1->v.sp->tp->templateParam || !exp1->v.sp->tp->templateParam->second->packed)
            {
                *tp = &stdunsigned;
                *exp = intNode(en_c_i, 1);
            }
            else if (templateNestingCount)
            {
                *exp = intNode(en_sizeofellipse, 0);
                (*exp)->v.templateParam = (*tp)->templateParam;
                *tp = &stdunsigned;
            }
            else if (!(*tp)->templateParam->second->packed)
            {
                *tp = &stdunsigned;
                *exp = intNode(en_c_i, 1);
            }
            else
            {
                int n = 0;
                if ((*tp)->templateParam->second->byPack.pack)
                    n = (*tp)->templateParam->second->byPack.pack->size();
                *tp = &stdunsigned;
                *exp = intNode(en_c_i, n);
            }
        }
        if (paren)
            needkw(&lex, closepa);
    }
    else
    {
        if (MATCHKW(lex, openpa))
        {
            paren = true;
            lex = getsym();
        }
        if (!paren || !startOfType(lex, nullptr, false))
        {
            if (paren)
            {
                lex = expression_comma(lex, funcsp, nullptr, tp, exp, nullptr, _F_SIZEOF);
                needkw(&lex, closepa);
            }
            else
            {
                lex = expression_unary(lex, funcsp, nullptr, tp, exp, nullptr, _F_SIZEOF);
            }
            ResolveTemplateVariable(tp, exp, &stdint, nullptr);
            if (!*tp)
            {
                *exp = intNode(en_c_i, 1);
                error(ERR_EXPRESSION_SYNTAX);
            }
            else
            {
                *exp = nodeSizeof(*tp, *exp, _F_SIZEOF);
            }
        }
        else
        {
            LEXLIST* prev = lex;
            lex = get_type_id(lex, tp, funcsp, sc_cast, Optimizer::cparams.prm_cplusplus, true, false);
            if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, openpa))
            {
                lex = prevsym(prev);
                lex = expression_func_type_cast(lex, funcsp, tp, exp, 0);
            }
            if (paren)
                needkw(&lex, closepa);
            if (MATCHKW(lex, ellipse))
            {
                lex = getsym();
                if ((*tp)->type == bt_templateparam)
                {
                    if (!(*tp)->templateParam->second->packed)
                    {
                        error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                    }
                    else if (packIndex != -1 && (*tp)->templateParam->second->byPack.pack)
                    {
                        auto tpl = (*tp)->templateParam->second->byPack.pack->begin();
                        auto tple = (*tp)->templateParam->second->byPack.pack->end();
                        int i;
                        for (i = 0; tpl != tple && i < packIndex; i++, ++tpl);
                        if (tpl != tple)
                            *tp = tpl->second->byClass.val;
                    }
                }
                else
                {
                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                }
            }
            if (!*tp)
            {
                *exp = intNode(en_c_i, 1);
            }
            else
            {
                checkauto(*tp, ERR_AUTO_NOT_ALLOWED);
                *exp = nodeSizeof(*tp, *exp, _F_SIZEOF);
            }
        }

        *tp = &stdint; /* other compilers use sizeof as a signed value so we do too... */
    }
    return lex;
}
static LEXLIST* expression_alignof(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    lex = getsym();
    if (needkw(&lex, openpa))
    {
        if (startOfType(lex, nullptr, false))
        {
            lex = get_type_id(lex, tp, funcsp, sc_cast, false, true, false);
        }
        else
        {
            EXPRESSION* exp = nullptr;
            lex = expression(lex, funcsp, nullptr, tp, &exp, 0);
        }
        needkw(&lex, closepa);
        if (MATCHKW(lex, ellipse))
        {
            lex = getsym();
            if ((*tp)->type == bt_templateparam)
            {
                if (!(*tp)->templateParam->second->packed)
                {
                    error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                }
                else if (packIndex != -1)
                {
                    auto tpl = (*tp)->templateParam->second->byPack.pack->begin();
                    auto tple = (*tp)->templateParam->second->byPack.pack->end();
                    int i;
                    for (i = 0; tpl != tple && i < packIndex; i++, ++tpl);
                    if (tpl != tple)
                        *tp = tpl->second->byClass.val;
                }
            }
            else
            {
                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
            }
        }
        if (!*tp)
        {
            *exp = intNode(en_c_i, 1);
        }
        else
        {
            TYPE* itp = *tp;
            checkauto(itp, ERR_AUTO_NOT_ALLOWED);
            if (isref(itp))
                itp = (basetype(itp)->btp);
            itp = basetype(itp);

            *exp = intNode(en_c_i, getAlign(sc_global, itp));
        }
    }
    *tp = &stdint;
    return lex;
}
static LEXLIST* expression_ampersand(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags)
{
    lex = getsym();
    lex = expression_cast(lex, funcsp, atp, tp, exp, nullptr, (flags) | _F_AMPERSAND);
    if (*tp)
    {
        TYPE *btp, *tp1;
        EXPRESSION *exp1 = *exp, *symRef;
        while (exp1->type == en_void && exp1->right)
            exp1 = exp1->right;
        if (exp1->type == en_void)
            exp1 = exp1->left;
        symRef = (Optimizer::architecture == ARCHITECTURE_MSIL) ? GetSymRef(exp1) : nullptr;
        btp = basetype(*tp);
        tp1 = LookupSingleAggregate(btp, &exp1, true);
        if ((isstructuredmath(*tp) || Optimizer::architecture == ARCHITECTURE_MSIL) &&
            insertOperatorFunc(ovcl_unary_any, and_unary, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
        {
            return lex;
        }
        else if (isvoid(*tp) || ismsil(*tp))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (btp->hasbits)
            error(ERR_CANNOT_TAKE_ADDRESS_OF_BIT_FIELD);
        else if (btp->msil)
            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
        else if (symRef && symRef->type != en_labcon && symRef->v.sp->sb->attribs.inheritable.linkage2 == lk_property)
            errorsym(ERR_CANNOT_TAKE_ADDRESS_OF_PROPERTY, symRef->v.sp);
        else if (inreg(*exp, true))
            error(ERR_CANNOT_TAKE_ADDRESS_OF_REGISTER);
        else if ((!ispointer(btp) || !(btp)->array) && !isstructured(btp) && !isfunction(btp) && btp->type != bt_aggregate &&
                 (exp1)->type != en_memberptr)
        {
            if ((exp1)->type != en_const && exp1->type != en_assign)
                if (!lvalue(exp1))
                    if (Optimizer::cparams.prm_ansi || !castvalue(exp1))
                        error(ERR_MUST_TAKE_ADDRESS_OF_MEMORY_LOCATION);
            if (lvalue(exp1))
            {
                if (exp1->left->type == en_structadd && isconstzero(&stdint, exp1->left->right) && exp1->left->left->type == en_l_ref && exp1->left->left->left->type == en_auto)
                {
                    if (exp1->left->left->left->v.sp->sb->storage_class == sc_parameter)
                        exp1->left->left->left->v.sp->sb->addressTaken = true;
                }
            }
        }
        else
            switch ((exp1)->type)
            {
                case en_pc:
                case en_auto:
                case en_global:
                case en_absolute:
                case en_threadlocal: {
                    SYMBOL* sym = exp1->v.sp;
                    if (sym->sb->isConstructor || sym->sb->isDestructor)
                        error(ERR_CANNOT_TAKE_ADDRESS_OF_CONSTRUCTOR_OR_DESTRUCTOR);
                    break;
                }
                default:
                    break;
            }
        if (exp1->type == en_memberptr)
        {
            *tp = tp1;
            *exp = exp1;
        }
        else if ((*exp)->type == en_const)
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
                    TYPE *tp, *tpb, *tpn, **tpnp = &tpn;
                    bool done = false;
                    SYMBOL* spold = sym;
                    sym->sb->label = Optimizer::nextLabel++;
                    sym = CopySymbol(sym);
                    spold->sb->indecltable = true;
                    tp = sym->tp;
                    tpb = basetype(tp);
                    do
                    {
                        *tpnp = CopyType(tp);
                        UpdateRootTypes(*tpnp);
                        tpnp = &(*tpnp)->btp;
                        if (tp != tpb)
                            tp = tp->btp;
                        else
                            done = true;
                    } while (!done);
                    sym->tp = tpn;
                    sym->sb->storage_class = sc_localstatic;
                    SetLinkerNames(sym, lk_cdecl);
                    Optimizer::SymbolManager::Get(sym)->storage_class = Optimizer::scc_localstatic;
                    insertInitSym(sym);
                }
                else
                {
                    insertInitSym(sym);
                }
                if (!sym->sb->parent)
                    sym->sb->parent = funcsp;  // this promotion of a global to local is necessary to not make it linkable
                *exp = varNode(en_global, sym);
            }
            else
            {
                *exp = varNode(en_global, sym);
            }
            *tp = MakeType(bt_pointer, *tp);
        }
        else if (!isfunction(*tp) && (*tp)->type != bt_aggregate)
        {
            EXPRESSION *expasn = nullptr, **exp2;
            while (castvalue(exp1))
                exp1 = (exp1)->left;
            if (exp1->type == en_assign)
            {
                expasn = exp1;
                exp1 = exp1->left;
                while (castvalue(exp1))
                    exp1 = (exp1)->left;
            }
            if (!lvalue(exp1))
            {
                if (!btp->array && !btp->vla && !isstructured(btp) && basetype(btp)->type != bt_memberptr &&
                    basetype(btp)->type != bt_templateparam)
                    error(ERR_LVALUE);
            }
            else if (!isstructured(btp) && exp1->type != en_l_ref)
                exp1 = (exp1)->left;

            switch ((exp1)->type)
            {
                case en_pc:
                case en_auto:
                case en_global:
                case en_absolute:
                case en_threadlocal:
                    (exp1)->v.sp->sb->addressTaken = true;
                    break;
                default:
                    break;
            }
            exp2 = exp;
            while ((*exp2)->type == en_void && (*exp2)->right)
                exp2 = &(*exp2)->right;
            if ((*exp2)->type == en_void)
                exp2 = &(*exp2)->left;
            if (basetype(btp)->type != bt_memberptr)
            {
                *tp = MakeType(bt_pointer, *tp);
                if (expasn)
                    *exp2 = exprNode(en_void, expasn, exp1);
                else
                    *exp2 = exp1;
            }
        }
    }
    return lex;
}
static LEXLIST* expression_deref(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, int flags)
{
    lex = getsym();
    lex = expression_cast(lex, funcsp, nullptr, tp, exp, nullptr, flags);
    if ((isstructuredmath(*tp) || Optimizer::architecture == ARCHITECTURE_MSIL) &&
        insertOperatorFunc(ovcl_unary_pointer, star_unary, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
    {
        return lex;
    }
    if (*tp && (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp)))
    {
        error(ERR_NOT_AN_ALLOWED_TYPE);
    }
    else if (*tp && basetype(*tp)->type == bt_memberptr)
        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
    if (*tp && !isfunction(*tp) && !isfuncptr(*tp))
    {
        TYPE *btp = basetype(*tp), *btp2, *btp3;
        ;
        if (!ispointer(*tp))
        {
            if (!templateNestingCount || instantiatingTemplate)
                error(ERR_DEREF);
            deref(&stdpointer, exp);
        }
        else
        {
            btp3 = btp->btp;
            btp2 = basetype(btp->btp);
            if (btp2->type == bt_void)
            {
                error(ERR_DEREF);
                deref(&stdpointer, exp);
            }
            else if (isstructured(btp2))
            {
                *tp = btp3;
                *tp = PerformDeferredInitialization(*tp, funcsp);
            }
            else
            {
                if (!isfunction(btp2))
                {
                    *tp = btp3;
                    deref(*tp, exp);
                    if (isvolatile(btp3))
                        (*exp)->isvolatile = true;
                    if (isrestrict(btp3))
                        (*exp)->isrestrict = true;
                    if (isatomic(btp3))
                    {
                        (*exp)->isatomic = true;
                        if (needsAtomicLockFromType(btp3))
                            (*exp)->lockOffset = basetype(btp3)->size;
                    }
                }
            }
        }
    }
    tagNonConst(funcsp, *tp);
    return lex;
}
static LEXLIST* expression_postfix(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    TYPE* oldType;
    bool done = false;
    bool localMutable = false;
    if (KWTYPE(lex, TT_POINTERQUAL | TT_LINKAGE | TT_BASETYPE | TT_STORAGE_CLASS))
    {
        lex = expression_func_type_cast(lex, funcsp, tp, exp, flags);
    }
    else
        switch (KW(lex))
        {
            case kw_dynamic_cast:
                oldType = nullptr;
                lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doDynamicCast(tp, oldType, exp, funcsp))
                    if (!typeHasTemplateArg(*tp))
                        errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
                if (isref(*tp))
                    *tp = basetype(*tp)->btp;
                break;
            case kw_static_cast:
                oldType = nullptr;
                lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doStaticCast(tp, oldType, exp, funcsp, true))
                    if (!typeHasTemplateArg(*tp))
                        errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
                if (isref(*tp))
                    *tp = basetype(*tp)->btp;
                break;
            case kw_const_cast:
                oldType = nullptr;
                lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doConstCast(tp, oldType, exp, funcsp))
                    if (!typeHasTemplateArg(*tp))
                        errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
                if (isref(*tp))
                    *tp = basetype(*tp)->btp;
                break;
            case kw_reinterpret_cast:
                oldType = nullptr;
                lex = GetCastInfo(lex, funcsp, tp, &oldType, exp, (flags & _F_PACKABLE));
                if (*tp && !doReinterpretCast(tp, oldType, exp, funcsp, true))
                    if (!typeHasTemplateArg(*tp))
                        errortype(ERR_CANNOT_CAST_TYPE, oldType, *tp);
                if (isref(*tp))
                    *tp = basetype(*tp)->btp;
                break;
            case kw_typeid:
                lex = expression_typeid(lex, funcsp, tp, exp, (flags & _F_PACKABLE));
                break;
            default:
                lex = expression_primary(lex, funcsp, atp, tp, exp, &localMutable, flags);
                if (ismutable)
                    *ismutable = localMutable;
                break;
        }
    if (!*tp)
        return lex;
    while (!done && lex && !parsingPreprocessorConstant)
    {
        enum e_kw kw;
        switch (KW(lex))
        {
            case openbr:
                lex = expression_bracket(lex, funcsp, tp, exp, flags);
                break;
            case openpa:
                lex = expression_arguments(lex, funcsp, tp, exp, flags);
                break;
            case pointsto:
            case dot:
                lex = expression_member(lex, funcsp, tp, exp, ismutable, flags);
                break;
            case autoinc:
            case autodec:
                ConstExprPromote(*exp, false);
                kw = KW(lex);
                lex = getsym();
                if (isstructuredmath(*tp))
                {
                    if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                        insertOperatorFunc(ovcl_unary_postfix, kw, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
                    {
                        continue;
                    }
                    else
                    {
                        castToArithmetic(false, tp, exp, kw, nullptr, true);
                    }
                }
                if (isconstraw(*tp) && !localMutable)
                    error(ERR_CANNOT_MODIFY_CONST_OBJECT);
                else if (isstructured(*tp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (!lvalue(*exp) && basetype(*tp)->type != bt_templateparam)
                {
                    error(ERR_LVALUE);
                }
                else
                {
                    EXPRESSION *exp3 = nullptr, *exp1 = nullptr;
                    if ((*exp)->left->type == en_func || (*exp)->left->type == en_thisref)
                    {
                        EXPRESSION* exp2 = anonymousVar(sc_auto, *tp);
                        deref(&stdpointer, &exp2);
                        exp3 = exprNode(en_assign, exp2, (*exp)->left);
                        deref(*tp, &exp2);
                        *exp = exp2;
                    }
                    if (basetype(*tp)->type == bt_pointer)
                    {
                        TYPE* btp = basetype(*tp)->btp;
                        if (basetype(btp)->type == bt_void)
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
                        if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                            error(ERR_NOT_AN_ALLOWED_TYPE);
                        if (basetype(*tp)->scoped && !(flags & _F_SCOPEDENUM))
                            error(ERR_SCOPED_TYPE_MISMATCH);
                        if (basetype(*tp)->type == bt_memberptr)
                            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                        exp1 = intNode(en_c_i, 1);
                    }
                    if (basetype(*tp)->type == bt_bool)
                    {
                        /* autoinc of a bool sets it true.  autodec not allowed in C++
                            * these aren't spelled out in the C99 standard, we are
                            * doing the normal thing here...
                            */
                        if (kw == autodec)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                error(ERR_CANNOT_USE_bool_HERE);
                            *exp = exprNode(en_assign, *exp, intNode(en_c_bool, 0));
                        }
                        else
                        {
                            *exp = exprNode(en_assign, *exp, intNode(en_c_bool, 1));
                        }
                    }
                    else
                    {
                        if (ispointer(*tp))
                            cast(&stdint, &exp1);
                        else
                            cast(*tp, &exp1);
                        *exp = exprNode(kw == autoinc ? en_autoinc : en_autodec, *exp, exp1);
                    }
                    if (exp3)
                        *exp = exprNode(en_void, exp3, *exp);
                    while (lvalue(exp1))
                        exp1 = exp1->left;
                    if (exp1->type == en_auto)
                        exp1->v.sp->sb->altered = true;
                }
                break;
            default:
                done = true;
                break;
        }
    }
    /*	if (isfunction(*tp))
            error(ERR_FUNCTION_ARGUMENTS_EXPECTED);
    */
    return lex;
}
LEXLIST* expression_unary(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    bool localMutable = false;
    enum e_kw kw = KW(lex);
    /* note some of the math ops are speced to do integer promotions
     * if being stored, the proposed place to store them is not known, so e.g.
     * a ~ on a unsigned char would promote to int to be evaluated,
     * the int would be further sign
     * extended to a long long though through the assignment promotion
     */
    switch (kw)
    {
        case plus:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, nullptr, flags);
            if (*tp)
            {
                if (isstructuredmath(*tp))
                {
                    if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                        insertOperatorFunc(ovcl_unary_numeric, plus_unary, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
                    {
                        break;
                    }
                    else
                    {
                        LookupSingleAggregate(*tp, exp);
                        castToArithmetic(false, tp, exp, kw, nullptr, true);
                    }
                }
                if (isstructured(*tp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (isvoid(*tp) || ismsil(*tp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (basetype(*tp)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if (basetype(*tp)->scoped && !(flags & _F_SCOPEDENUM))
                    error(ERR_SCOPED_TYPE_MISMATCH);
                else if (ispointer(*tp))
                    error(ERR_ILL_POINTER_OPERATION);
                else if (atp && basetype(atp)->type < bt_int)
                {
                    cast(atp, exp);
                    *tp = atp;
                }
                else if (basetype(*tp)->type < bt_int)
                {
                    cast(&stdint, exp);
                    *tp = &stdint;
                }
            }
            break;
        case minus:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, nullptr, flags);
            if (!*tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
                *tp = &stdint;
            }
            LookupSingleAggregate(*tp, exp);
            if (*tp)
            {
                if (isstructuredmath(*tp))
                {
                    if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                        insertOperatorFunc(ovcl_unary_numeric, minus_unary, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
                    {
                        break;
                    }
                    else
                    {
                        castToArithmetic(false, tp, exp, kw, nullptr, true);
                    }
                }
                if (isstructured(*tp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (isvoid(*tp) || ismsil(*tp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (basetype(*tp)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if (basetype(*tp)->scoped && !(flags & _F_SCOPEDENUM))
                    error(ERR_SCOPED_TYPE_MISMATCH);
                else if (ispointer(*tp))
                    error(ERR_ILL_POINTER_OPERATION);
                else if (atp && basetype(atp)->type < bt_int)
                {
                    *exp = exprNode(en_uminus, *exp, nullptr);
                    cast(atp, exp);
                    *tp = atp;
                }
                else if (basetype(*tp)->type < bt_int)
                {
                    cast(&stdint, exp);
                    *exp = exprNode(en_uminus, *exp, nullptr);
                    *tp = &stdint;
                }
                else
                {
                    *exp = exprNode(en_uminus, *exp, nullptr);
                }
            }
            break;
        case star:
            lex = expression_deref(lex, funcsp, tp, exp, flags);
            break;
        case andx:
            lex = expression_ampersand(lex, funcsp, atp, tp, exp, flags);
            break;
        case notx:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, nullptr, flags);
            if (!*tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
                *tp = &stdint;
            }
            LookupSingleAggregate(*tp, exp);
            if (*tp)
            {
                if (isstructuredmath(*tp))
                {
                    if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                        insertOperatorFunc(ovcl_unary_numericptr, notx, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
                    {
                        break;
                    }
                    else
                    {
                        castToArithmetic(false, tp, exp, kw, nullptr, false);
                    }
                }
                if (isstructured(*tp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (isvoid(*tp) || ismsil(*tp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (basetype(*tp)->scoped && !(flags & _F_SCOPEDENUM))
                    error(ERR_SCOPED_TYPE_MISMATCH);
                if ((*tp)->type == bt_memberptr)
                {
                    *exp = exprNode(en_mp_as_bool, *exp, nullptr);
                    (*exp)->size = *tp;
                    *exp = exprNode(en_not, *exp, nullptr);
                }
                else
                {
                    *exp = exprNode(en_not, *exp, nullptr);
                }
                if (Optimizer::cparams.prm_cplusplus)
                    *tp = &stdbool;
                else
                    *tp = &stdint;
            }
            break;
        case complx:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, nullptr, flags);
            if (!*tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
                *tp = &stdint;
            }
            LookupSingleAggregate(*tp, exp);
            if (*tp)
            {
                if (isstructuredmath(*tp))
                {
                    if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                        insertOperatorFunc(ovcl_unary_int, complx, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
                    {
                        break;
                    }
                    else
                    {
                        castToArithmetic(true, tp, exp, kw, nullptr, true);
                    }
                }
                if (isstructured(*tp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (iscomplex(*tp))
                    error(ERR_ILL_USE_OF_COMPLEX);
                else if (isfloat(*tp) || isimaginary(*tp))
                    error(ERR_ILL_USE_OF_FLOATING);
                else if (ispointer(*tp))
                    error(ERR_ILL_POINTER_OPERATION);
                else if (isvoid(*tp) || ismsil(*tp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (basetype(*tp)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if (basetype(*tp)->scoped && !(flags & _F_SCOPEDENUM))
                    error(ERR_SCOPED_TYPE_MISMATCH);
                else if (atp && basetype(atp)->type < bt_int)
                {
                    *exp = exprNode(en_compl, *exp, nullptr);
                    cast(atp, exp);
                    *tp = atp;
                }
                else if (basetype(*tp)->type < bt_int)
                {
                    cast(&stdint, exp);
                    *exp = exprNode(en_compl, *exp, nullptr);
                    *tp = &stdint;
                }
                else
                {
                    *exp = exprNode(en_compl, *exp, nullptr);
                    cast(basetype(*tp), exp);
                }
            }
            break;
        case autoinc:
        case autodec:
            lex = getsym();
            lex = expression_cast(lex, funcsp, atp, tp, exp, &localMutable, flags);
            ConstExprPromote(*exp, false);
            if (*tp)
            {
                if (isstructuredmath(*tp))
                {
                    if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                        insertOperatorFunc(ovcl_unary_prefix, kw, funcsp, tp, exp, nullptr, nullptr, nullptr, flags))
                    {
                        break;
                    }
                    else
                    {
                        castToArithmetic(false, tp, exp, kw, nullptr, true);
                    }
                }
                if (isstructured(*tp))
                    error(ERR_ILL_STRUCTURE_OPERATION);
                else if (iscomplex(*tp))
                    error(ERR_ILL_USE_OF_COMPLEX);
                else if (isconstraw(*tp) && !localMutable)
                    error(ERR_CANNOT_MODIFY_CONST_OBJECT);
                else if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                    error(ERR_NOT_AN_ALLOWED_TYPE);
                else if (basetype(*tp)->type == bt_memberptr)
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                else if (basetype(*tp)->scoped && !(flags & _F_SCOPEDENUM))
                    error(ERR_SCOPED_TYPE_MISMATCH);
                else if (!lvalue(*exp))
                {
                    if ((*exp)->type != en_templateparam && basetype(*tp)->type != bt_templateparam &&
                        basetype(*tp)->type != bt_templateselector && basetype(*tp)->type != bt_templatedecltype)
                        error(ERR_LVALUE);
                }
                else
                {
                    EXPRESSION* exp3 = nullptr;
                    if ((*exp)->left->type == en_func || (*exp)->left->type == en_thisref)
                    {
                        EXPRESSION* exp2 = anonymousVar(sc_auto, *tp);
                        deref(&stdpointer, &exp2);
                        exp3 = exprNode(en_assign, exp2, (*exp)->left);
                        deref(*tp, &exp2);
                        *exp = exp2;
                    }
                    if (ispointer(*tp))
                    {
                        TYPE* tpx;
                        if (basetype(basetype(*tp)->btp)->type == bt_void)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                error(ERR_ARITHMETIC_WITH_VOID_STAR);
                            tpx = &stdchar;
                        }
                        else
                        {
                            tpx = basetype(*tp)->btp;
                        }
                        *exp =
                            exprNode(en_assign, *exp, exprNode(kw == autoinc ? en_add : en_sub, *exp, nodeSizeof(tpx, *exp)));
                        (*exp)->preincdec = true;
                    }
                    else if (kw == autoinc && basetype(*tp)->type == bt_bool)
                    {
                        *exp = exprNode(en_assign, *exp, intNode(en_c_i, 1));  // set to true as per C++
                    }
                    else if (kw == autodec && basetype(*tp)->type == bt_bool)
                    {
                        if (Optimizer::cparams.prm_cplusplus)
                            error(ERR_CANNOT_USE_bool_HERE);

                        *exp = exprNode(en_assign, *exp, intNode(en_c_i, 0));  // c++ doesn't allow it, set it to true for C.
                    }
                    else
                    {
                        EXPRESSION *dest = *exp, *exp1 = intNode(en_c_i, 1);
                        *exp = RemoveAutoIncDec(*exp);
                        cast(*tp, &exp1);
                        *exp = exprNode(en_assign, dest, exprNode(kw == autoinc ? en_add : en_sub, *exp, exp1));
                        (*exp)->preincdec = true;
                    }
                    if (exp3)
                        *exp = exprNode(en_void, exp3, *exp);
                }
            }
            break;
        case kw_sizeof:
            lex = expression_sizeof(lex, funcsp, tp, exp);
            break;
        case kw_alignof:
            lex = expression_alignof(lex, funcsp, tp, exp);
            break;
        case kw_new:
            lex = expression_new(lex, funcsp, tp, exp, false, flags);
            break;
        case kw_delete:
            lex = expression_delete(lex, funcsp, tp, exp, false, flags);
            break;
        case kw_noexcept:
            lex = expression_noexcept(lex, funcsp, tp, exp);
            break;
        case classsel: {
            LEXLIST* placeholder = lex;
            lex = getsym();
            switch (KW(lex))
            {
                case kw_new:
                    return expression_new(lex, funcsp, tp, exp, true, flags);
                case kw_delete:
                    return expression_delete(lex, funcsp, tp, exp, true, flags);
                default:
                    break;
            }
            lex = prevsym(placeholder);
        }
            // fallthrough
        default:
            lex = expression_postfix(lex, funcsp, atp, tp, exp, ismutable, flags);
            break;
    }
    return lex;
}
LEXLIST* expression_cast(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    TYPE* throwaway = nullptr;
    if (MATCHKW(lex, openpa))
    {
        bool loadedAttribs = false;
        attributes oldAttribs;
        LEXLIST* start = lex;
        lex = getsym();
        if (MATCHKW(lex, kw__attribute))
        {
            loadedAttribs = true;
            oldAttribs = basisAttribs;

            basisAttribs = {0};
            ParseAttributeSpecifiers(&lex, funcsp, true);
        }
        bool structured = false;
        if (startOfType(lex, &structured, false))
        {
            if (!Optimizer::cparams.prm_cplusplus || resolveToDeclaration(lex, structured))
            {
                bool done = false;
                lex = get_type_id(lex, tp, funcsp, sc_cast, false, true, false);
                if (!*tp)
                {
                    error(ERR_TYPE_NAME_EXPECTED);
                    *tp = &stdint;
                    *exp = intNode(en_c_i, 0);
                }
                else
                {
                    (*tp)->used = true;
                    needkw(&lex, closepa);
                    checkauto(*tp, ERR_AUTO_NOT_ALLOWED);
                    if (MATCHKW(lex, begin))
                    {
                        std::list<INITIALIZER*>* temp = nullptr;
                        INITIALIZER* init = nullptr;
                        SYMBOL* sym = nullptr;
                        if (!Optimizer::cparams.prm_c99 && !Optimizer::cparams.prm_cplusplus)
                            error(ERR_C99_STYLE_INITIALIZATION_USED);
                        if (Optimizer::cparams.prm_cplusplus)
                        {
                            sym = makeID(sc_auto, *tp, nullptr, AnonymousName());
                            localNameSpace->front()->syms->Add(sym);
                        }
                        lex = initType(lex, funcsp, 0, sc_auto, &temp, nullptr, *tp, sym, false, flags);
                        *exp = convertInitToExpression(*tp, nullptr, nullptr, funcsp, temp, *exp, false);
                        while (!done && lex)
                        {
                            enum e_kw kw;
                            switch (KW(lex))
                            {
                                case openbr:
                                    lex = expression_bracket(lex, funcsp, tp, exp, flags);
                                    break;
                                case openpa:
                                    lex = expression_arguments(lex, funcsp, tp, exp, flags);
                                    break;
                                case pointsto:
                                case dot:
                                    lex = expression_member(lex, funcsp, tp, exp, ismutable, flags);
                                    break;
                                case autoinc:
                                case autodec:
                                    ConstExprPromote(*exp, false);

                                    kw = KW(lex);
                                    lex = getsym();
                                    if (isstructuredmath(*tp))
                                    {
                                        if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                                            insertOperatorFunc(ovcl_unary_postfix, kw, funcsp, tp, exp, nullptr, nullptr, nullptr,
                                                flags))
                                        {
                                            continue;
                                        }
                                        else
                                        {
                                            castToArithmetic(false, tp, exp, kw, nullptr, true);
                                        }
                                    }
                                    if (isstructured(*tp))
                                        error(ERR_ILL_STRUCTURE_OPERATION);
                                    else if (!lvalue(*exp) && basetype(*tp)->type != bt_templateparam)
                                        error(ERR_LVALUE);
                                    else
                                    {
                                        EXPRESSION* exp1 = nullptr;
                                        if (basetype(*tp)->type == bt_pointer)
                                        {
                                            TYPE* btp = basetype(*tp)->btp;
                                            exp1 = nodeSizeof(btp, *exp);
                                        }
                                        else
                                        {
                                            if (isvoid(*tp) || (*tp)->type == bt_aggregate || ismsil(*tp))
                                                error(ERR_NOT_AN_ALLOWED_TYPE);
                                            if (basetype(*tp)->scoped && !(flags & _F_SCOPEDENUM))
                                                error(ERR_SCOPED_TYPE_MISMATCH);
                                            if (basetype(*tp)->type == bt_memberptr)
                                                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                                            exp1 = intNode(en_c_i, 1);
                                        }
                                        if (basetype(*tp)->type == bt_bool)
                                        {
                                            /* autoinc of a bool sets it true.  autodec not allowed
                                                * these aren't spelled out in the C99 standard, we are
                                                * following the C++ standard here
                                                */
                                            if (kw == autodec)
                                                error(ERR_CANNOT_USE_bool_HERE);
                                            *exp = exprNode(en_assign, *exp, intNode(en_c_bool, 1));
                                        }
                                        else
                                        {
                                            cast(*tp, &exp1);
                                            *exp = exprNode(kw == autoinc ? en_autoinc : en_autodec, *exp, exp1);
                                        }
                                        while (lvalue(exp1))
                                            exp1 = exp1->left;
                                        if (exp1->type == en_auto)
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
                        LEXLIST* lastSym = lex;
                        lex = expression_cast(lex, funcsp, nullptr, &throwaway, exp, ismutable, flags);
                        if (throwaway)
                        {
                            if ((*exp)->type == en_pc || ((*exp)->type == en_func && !(*exp)->v.func->ascall))
                                thunkForImportTable(exp);
                            if (basetype(*tp)->type == bt___string)
                            {
                                if ((*exp)->type == en_labcon && (*exp)->string)
                                    (*exp)->type = en_c_string;
                                else if (basetype(throwaway)->type != bt___string)
                                    *exp = exprNode(en_x_string, *exp, nullptr);
                            }
                            else if (basetype(*tp)->type == bt___object)
                            {
                                if (basetype(throwaway)->type != bt___object)
                                    if (!isstructured(throwaway) && (!isarray(throwaway) || !basetype(throwaway)->msil))
                                        *exp = exprNode(en_x_object, *exp, nullptr);
                            }
                            else if ((isvoid(throwaway) && !isvoid(*tp)) || ismsil(*tp))
                            {
                                error(ERR_NOT_AN_ALLOWED_TYPE);
                            }
                            else if (!Optimizer::cparams.prm_cplusplus &&
                                     ((isstructured(throwaway) && !isvoid(*tp)) || basetype(throwaway)->type == bt_memberptr ||
                                      basetype(*tp)->type == bt_memberptr) &&
                                     !comparetypes(throwaway, *tp, true))
                            {
                                error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                            }
                            else if (Optimizer::cparams.prm_cplusplus)
                            {
                                if (!doStaticCast(tp, throwaway, exp, funcsp, false) &&
                                    !doReinterpretCast(tp, throwaway, exp, funcsp, false))
                                {
                                    cast(*tp, exp);
                                }
                            }
                            else if (!isstructured(*tp) && (!isarray(*tp) || !basetype(*tp)->msil))
                            {
                                cast(*tp, exp);
                            }
                        }
                        else
                        {
                            *exp = intNode(en_c_i, 0);
                            *tp = &stdint;
                            prevsym(lastSym);
                            error(ERR_EXPRESSION_SYNTAX);
                        }
                    }
                }
            }
            else  // expression in parenthesis
            {
                lex = prevsym(start);
                lex = expression_unary(lex, funcsp, atp, tp, exp, ismutable, flags);
            }
        }
        else
        {
            lex = prevsym(start);
            lex = expression_unary(lex, funcsp, atp, tp, exp, ismutable, flags);
        }
        if (loadedAttribs)
            basisAttribs = oldAttribs;
    }
    else
    {
        lex = expression_unary(lex, funcsp, atp, tp, exp, ismutable, flags);
    }
    return lex;
}
static LEXLIST* expression_pm(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    lex = expression_cast(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while (MATCHKW(lex, dotstar) || MATCHKW(lex, pointstar))
    {
        bool points = false;
        enum e_kw kw = KW(lex);
        TYPE* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        lex = getsym();
        lex = expression_cast(lex, funcsp, nullptr, &tp1, &exp1, nullptr, flags);
        if (isstructuredmath(*tp, tp1) && Optimizer::cparams.prm_cplusplus && kw == pointstar &&
            insertOperatorFunc(ovcl_binary_any, pointstar, funcsp, tp, exp, tp1, exp1, nullptr, flags))
        {
            continue;
        }
        if (kw == pointstar)
        {
            points = true;
            if (ispointer(*tp))
            {
                *tp = basetype(*tp);
                *tp = (*tp)->btp;
                if (!isstructured(*tp))
                {
                    errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->data->kw->name);
                }
            }
            else
                errorstr(ERR_POINTER_TO_STRUCTURE_EXPECTED, lex->data->kw->name);
        }
        else if (!isstructured(*tp))
        {
            errorstr(ERR_STRUCTURED_TYPE_EXPECTED, lex->data->kw->name);
        }
        if (isfunction(tp1) && isstructured(*tp))
        {
            exp1 = getMemberPtr(basetype(tp1)->sp, basetype(*tp)->sp, &tp1, funcsp);
        }
        if (basetype(tp1)->type != bt_memberptr)
        {
            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
        }
        else
        {
            if (isstructured(*tp) && basetype(tp1)->type == bt_memberptr)
            {
                if (basetype(*tp)->sp != basetype(tp1)->sp && basetype(*tp)->sp->sb->mainsym != basetype(tp1)->sp &&
                    basetype(*tp)->sp != basetype(tp1)->sp->sb->mainsym)
                {
                    if (classRefCount(basetype(tp1)->sp, (*tp)->sp) != 1)
                    {
                        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, basetype(tp1)->sp, (*tp)->sp);
                    }
                    else
                    {
                        *exp = baseClassOffset(basetype(tp1)->sp, (*tp)->sp, *exp);
                    }
                }
                if (exp1->type == en_memberptr)
                {
                    int lab = dumpMemberPtr(exp1->v.sp, *tp, true);
                    exp1 = intNode(en_labcon, lab);
                }
                if (isfunction(basetype(tp1)->btp))
                {
                    FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                    if ((basetype(*tp))->sp->sb->vbaseEntries)
                    {
                        EXPRESSION* ec = exprNode(en_add, exp1, intNode(en_c_i, getSize(bt_pointer) + getSize(bt_int)));
                        EXPRESSION* ec1;
                        deref(&stdint, &ec);
                        ec1 = exprNode(en_sub, ec, intNode(en_c_i, 1));
                        deref(&stdint, &ec1);
                        ec = exprNode(en_cond, ec, exprNode(en_void, ec1, intNode(en_c_i, 0)));
                        *exp = exprNode(en_add, *exp, ec);
                    }
                    else
                    {
                        EXPRESSION* ec = exprNode(en_add, exp1, intNode(en_c_i, getSize(bt_pointer)));
                        deref(&stdpointer, &ec);
                        *exp = exprNode(en_add, *exp, ec);
                    }
                    funcparams->sp = tp1->btp->sp;
                    funcparams->fcall = exp1;
                    deref(&stdpointer, &funcparams->fcall);
                    funcparams->thisptr = *exp;
                    funcparams->thistp = MakeType(bt_pointer, *tp);
                    *exp = varNode(en_func, nullptr);
                    (*exp)->v.func = funcparams;
                    *tp = basetype(tp1);
                }
                else
                {
                    if ((*tp)->sp->sb->vbaseEntries)
                    {
                        EXPRESSION* ec = exprNode(en_add, exp1, intNode(en_c_i, getSize(bt_int)));
                        EXPRESSION* ec1;
                        deref(&stdint, &ec);
                        ec1 = exprNode(en_add, ec, intNode(en_c_i, -1));
                        ec1 = exprNode(en_add, *exp, ec1);
                        deref(&stdint, &ec1);
                        *exp = exprNode(en_cond, ec, exprNode(en_void, ec1, *exp));
                    }
                    deref(&stdint, &exp1);
                    *exp = exprNode(en_add, *exp, exp1);
                    *exp = exprNode(en_add, *exp, intNode(en_c_i, -1));
                    if (!isstructured(basetype(tp1)->btp))
                    {
                        deref(basetype(tp1)->btp, exp);
                    }
                    *tp = basetype(tp1)->btp;
                }
            }
        }
    }
    return lex;
}
static LEXLIST* expression_times(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    lex = expression_pm(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while (MATCHKW(lex, star) || MATCHKW(lex, divide) || MATCHKW(lex, mod))
    {
        enum e_kw kw = KW(lex);
        enum e_node type;
        TYPE* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        lex = getsym();
        lex = expression_pm(lex, funcsp, nullptr, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return lex;
        }
        ResolveTemplateVariable(tp, exp, tp1, atp);
        ResolveTemplateVariable(&tp1, &exp1, *tp, atp);
        if (isstructuredmath(*tp, tp1))
        {

            if (Optimizer::cparams.prm_cplusplus &&
                insertOperatorFunc(kw == mod ? ovcl_binary_int : ovcl_binary_numeric, kw, funcsp, tp, exp, tp1, exp1, nullptr, flags))
            {
                continue;
            }
            else
            {
                castToArithmetic(kw == mod, tp, exp, kw, tp1, true);
                castToArithmetic(kw == mod, &tp1, &exp1, (enum e_kw)-1, *tp, true);
                LookupSingleAggregate(*tp, exp);
                LookupSingleAggregate(tp1, &exp1);
            }
        }
        if (isstructured(*tp) || isstructured(tp1))
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if (isvoid(*tp) || isvoid(tp1) || ismsil(*tp) || ismsil(tp1))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
        else if ((basetype(*tp)->scoped || basetype(tp1)->scoped) && !(flags & _F_SCOPEDENUM))
            error(ERR_SCOPED_TYPE_MISMATCH);
        else if (ispointer(*tp) || ispointer(tp1))
            error(ERR_ILL_POINTER_OPERATION);
        else
        {
            int m1 = -1, m2 = -1;
            if (isimaginary(*tp) && isimaginary(tp1))
            {
                if (kw == star)
                    *exp = exprNode(en_uminus, *exp, nullptr);
                m1 = (*tp)->type - bt_float_imaginary;
                m2 = tp1->type - bt_float_imaginary;
                m1 = m1 > m2 ? m1 : m2;
                switch (m1)
                {
                    case 0:
                        *tp = &stdfloat;
                        tp1 = &stdfloatimaginary;
                        break;
                    case 1:
                        *tp = &stddouble;
                        tp1 = &stddoubleimaginary;
                        break;
                    case 2:
                        *tp = &stdlongdouble;
                        tp1 = &stdlongdoubleimaginary;
                        break;
                }
                cast(tp1, exp);
                cast(tp1, &exp1);
            }
            else
            {
                if (isimaginary(*tp) && (isfloat(tp1) || isint(tp1)))
                {
                    m1 = (*tp)->type - bt_float_imaginary;
                    m2 = isfloat(tp1) ? tp1->type - bt_float : m1;
                }
                else if ((isfloat(*tp) || isint(*tp)) && isimaginary(tp1))
                {
                    m1 = tp1->type - bt_float_imaginary;
                    m2 = isfloat(*tp) ? (*tp)->type - bt_float : m1;
                }
                if (m1 >= 0)
                {
                    bool b = isimaginary(*tp);
                    m1 = m1 > m2 ? m1 : m2;
                    switch (m1)
                    {
                        case 0:
                            *tp = &stdfloatimaginary;
                            tp1 = &stdfloat;
                            break;
                        case 1:
                            *tp = &stddoubleimaginary;
                            tp1 = &stddouble;
                            break;
                        default:
                            *tp = &stdlongdoubleimaginary;
                            tp1 = &stdlongdouble;
                            break;
                    }
                    if (b)
                    {
                        cast(*tp, exp);
                        cast(tp1, &exp1);
                    }
                    else
                    {
                        cast(tp1, exp);
                        cast(*tp, &exp1);
                    }
                }
                else
                {
                    *tp = destSize(*tp, tp1, exp, &exp1, false, nullptr);
                }
            }
            switch (kw)
            {
                case star:
                    type = en_mul;
                    break;
                case divide:
                    type = isunsigned(*tp) ? en_udiv : en_div;
                    break;
                case mod:
                    type = isunsigned(*tp) ? en_umod : en_mod;
                    break;
                default:
                    break;
            }
            *exp = exprNode(type, *exp, exp1);
        }
    }
    return lex;
}
static LEXLIST* expression_add(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    /* fixme add vlas */
    lex = expression_times(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while (MATCHKW(lex, plus) || MATCHKW(lex, minus))
    {
        bool msil = false;
        enum e_kw kw = KW(lex);
        TYPE* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        lex = getsym();
        lex = expression_times(lex, funcsp, atp, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return lex;
        }
        ResolveTemplateVariable(tp, exp, tp1, atp);
        ResolveTemplateVariable(&tp1, &exp1, *tp, atp);
        if (isstructuredmath(*tp, tp1))
        {

            if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                insertOperatorFunc(ovcl_binary_numericptr, kw, funcsp, tp, exp, tp1, exp1, nullptr, flags))
            {
                continue;
            }
            else
            {
                if (Optimizer::cparams.prm_cplusplus && *tp && isstructured(*tp))
                {
                    if (!castToArithmeticInternal(false, tp, exp, kw, isstructured(tp1) ? &stdint : tp1, false))
                        castToPointer(tp, exp, kw, &stdpointer);
                }
                if (Optimizer::cparams.prm_cplusplus && tp1 && isstructured(tp1))
                {
                    if (!castToArithmeticInternal(false, &tp1, &exp1, (enum e_kw)-1, isstructured(*tp) ? &stdint : *tp, false))
                        castToPointer(&tp1, &exp1, kw, &stdpointer);
                }
                LookupSingleAggregate(*tp, exp);
                LookupSingleAggregate(tp1, &exp1);
            }
        }
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions && kw == plus &&
            (basetype(*tp)->type == bt___string || basetype(tp1)->type == bt___string ||
                (atp && basetype(atp)->type == bt___string)))
        {
            msil = true;
            if ((*exp)->type == en_labcon && (*exp)->string)
                (*exp)->type = en_c_string;
            else if (!ismsil(*tp))
                *exp = exprNode(en_x_object, *exp, nullptr);
            if (exp1->type == en_labcon && exp1->string)
                exp1->type = en_c_string;
            else if (!ismsil(tp1))
                exp1 = exprNode(en_x_object, exp1, nullptr);
        }
        else if (kw == plus && ispointer(*tp) && ispointer(tp1))
            error(ERR_ILL_POINTER_ADDITION);
        else if (isvoid(*tp) || isvoid(tp1) || ismsil(*tp) || ismsil(tp1))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
        else if ((basetype(*tp)->scoped || basetype(tp1)->scoped) && !(flags & _F_SCOPEDENUM))
            error(ERR_SCOPED_TYPE_MISMATCH);
        else if (kw != plus && !ispointer(*tp) && ispointer(tp1))
            error(ERR_ILL_POINTER_SUBTRACTION);
        else if (isstructured(*tp) || isstructured(tp1))
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if (ispointer(*tp))
        {
            if (isarray(*tp) && (*tp)->msil)
                error(ERR_MANAGED_OBJECT_NO_ADDRESS);
            else if (ispointer(tp1) && !comparetypes(*tp, tp1, true) && !comparetypes(tp1, *tp, true))
                error(ERR_NONPORTABLE_POINTER_CONVERSION);
            else if (iscomplex(tp1))
                error(ERR_ILL_USE_OF_COMPLEX);
            else if (isfloat(tp1) || isimaginary(tp1))
                error(ERR_ILL_USE_OF_FLOATING);
        }
        else if (ispointer(tp1))
        {
            if (iscomplex(*tp))
                error(ERR_ILL_USE_OF_COMPLEX);
            else if (isfloat(*tp) || isimaginary(*tp))
                error(ERR_ILL_USE_OF_FLOATING);
            else if (isarray(tp1) && (tp1)->msil)
                error(ERR_MANAGED_OBJECT_NO_ADDRESS);
        }
        if (msil)
        {
            // MSIL back end will take care of figuring out what function to call
            // to perform the concatenation
            *exp = exprNode(en_add, *exp, exp1);
            *tp = &std__string;
        }
        else if (ispointer(*tp))
        {
            EXPRESSION* ns;
            if (basetype(basetype(*tp)->btp)->type == bt_void)
            {
                if (Optimizer::cparams.prm_cplusplus)
                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                ns = nodeSizeof(&stdchar, *exp);
            }
            else
            {
                ns = nodeSizeof(basetype(*tp)->btp, *exp);
            }
            if (ispointer(tp1))
            {
                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                {
                    cast(&stdinative, exp);
                    cast(&stdinative, &exp1);
                }
                *exp = exprNode(en_sub, *exp, exp1);
                *exp = exprNode(en_arraydiv, *exp, ns);
                *tp = &stdint; /* ptrdiff_t */
            }
            else
            {
                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                {
                    cast(&stdinative, exp);
                }
                /*				*tp = tp1 = destSize(*tp, tp1, exp, &exp1, false, nullptr); */
                if (basetype(tp1)->type < bt_int)
                    cast(&stdint, &exp1);
                exp1 = exprNode(en_umul, exp1, ns);
                *exp = exprNode(kw == plus ? en_add : en_sub, *exp, exp1);
            }
        }
        else if (ispointer(tp1))
        {
            EXPRESSION* ns;
            if (basetype(basetype(tp1)->btp)->type == bt_void)
            {
                if (Optimizer::cparams.prm_cplusplus)
                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                ns = nodeSizeof(&stdchar, *exp);
            }
            else
            {
                ns = nodeSizeof(basetype(tp1)->btp, *exp);
            }
            /*			*tp = tp1 = destSize(*tp, tp1, exp, &exp1, false, nullptr); */
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                cast(&stdinative, &exp1);
            }
            if (basetype(*tp)->type < bt_int)
                cast(&stdint, exp);
            *exp = exprNode(en_umul, *exp, ns);
            *exp = exprNode(en_add, *exp, exp1);
            *tp = tp1;
        }
        else
        {
            int m1 = -1, m2 = -1;
            if (isimaginary(*tp) && (isfloat(tp1) || isint(tp1)))
            {
                m1 = (*tp)->type - bt_float_imaginary;
                m2 = isfloat(tp1) ? tp1->type - bt_float : m1;
            }
            else if ((isfloat(*tp) || isint(*tp)) && isimaginary(tp1))
            {
                m1 = tp1->type - bt_float_imaginary;
                m2 = isfloat(*tp) ? (*tp)->type - bt_float : m1;
            }
            if (m1 >= 0)
            {
                m1 = m1 > m2 ? m1 : m2;
                TYPE *tpa, *tpb;
                switch (m1)
                {
                    case 0:
                        *tp = &stdfloatcomplex;
                        tpa = &stdfloat;
                        tpb = &stdfloatimaginary;
                        break;
                    case 1:
                        *tp = &stddoublecomplex;
                        tpa = &stddouble;
                        tpb = &stddoubleimaginary;
                        break;
                    default:
                        *tp = &stdlongdoublecomplex;
                        tpa = &stdlongdouble;
                        tpb = &stdlongdoubleimaginary;
                        break;
                }
                if (isimaginary(tp1))
                {
                    cast(tpa, exp);
                    cast(tpb, &exp1);
                }
                else
                {
                    cast(tpb, exp);
                    cast(tpa, &exp1);
                }
                if (kw != plus)
                    exp1 = exprNode(en_uminus, exp1, nullptr);
                *exp = exprNode(en_add, *exp, exp1);
                cast(*tp, exp);
            }
            else
            {
                *tp = destSize(*tp, tp1, exp, &exp1, true, atp);
                *exp = exprNode(kw == plus ? en_add : en_sub, *exp, exp1);
            }
        }
    }
    return lex;
}
static LEXLIST* expression_shift(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    lex = expression_add(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while ((MATCHKW(lex, rightshift) && !(flags & _F_INTEMPLATEPARAMS)) || MATCHKW(lex, leftshift))
    {
        TYPE* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        enum e_node type;
        enum e_kw kw = KW(lex);
        lex = getsym();
        lex = expression_add(lex, funcsp, nullptr, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return lex;
        }
        ResolveTemplateVariable(tp, exp, tp1, atp);
        ResolveTemplateVariable(&tp1, &exp1, *tp, atp);
        if (isstructuredmath(*tp, tp1))
        {
            LookupSingleAggregate(*tp, exp);
            LookupSingleAggregate(tp1, &exp1);
            if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                insertOperatorFunc(ovcl_binary_int, kw, funcsp, tp, exp, tp1, exp1, nullptr, flags))
            {
                continue;
            }
            else
            {
                castToArithmetic(true, tp, exp, kw, tp1, true);
                castToArithmetic(true, &tp1, &exp1, (enum e_kw)-1, *tp, true);
            }
        }
        if (isstructured(*tp) || isstructured(tp1))
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if (isvoid(*tp) || isvoid(tp1) || ismsil(*tp) || ismsil(tp1))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
            error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
        else if ((basetype(*tp)->scoped || basetype(tp1)->scoped) && !(flags & _F_SCOPEDENUM))
            error(ERR_SCOPED_TYPE_MISMATCH);
        else if (ispointer(*tp) || ispointer(tp1))
            error(ERR_ILL_POINTER_OPERATION);
        else if (isfloat(*tp) || isfloat(tp1) || isimaginary(*tp) || isimaginary(tp1))
            error(ERR_ILL_USE_OF_FLOATING);
        else if (iscomplex(*tp) || iscomplex(tp1))
            error(ERR_ILL_USE_OF_COMPLEX);
        else
        {
            if (kw == rightshift)
                if (isunsigned(*tp))
                    type = en_ursh;
                else
                    type = en_rsh;
            else
                type = en_lsh;
            if (basetype(*tp)->type < bt_int)
            {
                *tp = &stdint;
                cast(*tp, exp);
            }
            if (Optimizer::architecture == ARCHITECTURE_MSIL && tp1->size != stdint.size)
            {
                cast(&stdint, &exp1);
            } 
            *exp = exprNode(type, *exp, exprNode(en_shiftby, exp1, 0));
        }
    }
    return lex;
}
static LEXLIST* expression_inequality(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable,
                                      int flags)
{
    bool done = false;
    lex = expression_shift(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while (!done && lex)
    {
        enum e_kw kw = KW(lex);
        enum e_node type;
        const char* opname = "";
        TYPE* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        switch (kw)
        {
            case gt:
                opname = lex->data->kw->name;
                done = (flags & _F_INTEMPLATEPARAMS);
                break;
            case geq:
            case lt:
            case leq:
                opname = lex->data->kw->name;
                break;
            default:
                done = true;
                break;
        }
        if (!done)
        {
            lex = getsym();

            lex = expression_shift(lex, funcsp, nullptr, &tp1, &exp1, nullptr, flags);
            if (!tp1)
            {
                *tp = nullptr;
                return lex;
            }
            ResolveTemplateVariable(tp, exp, tp1, atp);
            ResolveTemplateVariable(&tp1, &exp1, *tp, atp);
            if (isstructuredmath(*tp, tp1))
            {
                if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                    insertOperatorFunc(ovcl_binary_numericptr, kw, funcsp, tp, exp, tp1, exp1, nullptr, flags))
                {
                    continue;
                }
                else
                {
                    checkscope(*tp, tp1);
                    if (Optimizer::cparams.prm_cplusplus && *tp && isstructured(*tp))
                    {
                        if (!castToArithmeticInternal(false, tp, exp, kw, tp1, true))
                            castToPointer(tp, exp, kw, ispointer(tp1) ? tp1 : &stdpointer);
                    }
                    if (Optimizer::cparams.prm_cplusplus && tp1 && isstructured(tp1))
                    {
                        if (!castToArithmeticInternal(false, &tp1, &exp1, (enum e_kw) - 1, *tp, true))
                            castToPointer(&tp1, &exp1, (enum e_kw) -1, ispointer(*tp) ? *tp : &stdpointer);
                    }
                }
                LookupSingleAggregate(*tp, exp);
                LookupSingleAggregate(tp1, &exp1);
            }

            if (Optimizer::cparams.prm_cplusplus)
            {
                SYMBOL* funcsp = nullptr;
                if ((ispointer(*tp) || basetype(*tp)->type == bt_memberptr) && tp1->type == bt_aggregate)
                {
                    if (tp1->syms->size() > 1)
                        errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)tp1->syms->front())->name);
                    exp1 = varNode(en_pc, tp1->syms->front());
                    tp1 = ((SYMBOL*)tp1->syms->front())->tp;
                }
                else if ((ispointer(tp1) || basetype(tp1)->type == bt_memberptr) && (*tp)->type == bt_aggregate)
                {
                    if ((*tp)->syms->size() > 1)
                        errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)(*tp)->syms->front())->name);
                    (*exp) = varNode(en_pc, (*tp)->syms->front());
                    (*tp) = ((SYMBOL*)(*tp)->syms->front())->tp;
                }
            }
            if ((*exp)->type == en_pc || ((*exp)->type == en_func && !(*exp)->v.func->ascall))
                thunkForImportTable(exp);
            if (exp1->type == en_pc || (exp1->type == en_func && !exp1->v.func->ascall))
                thunkForImportTable(&exp1);
            if (isstructured(*tp) || isstructured(tp1))
                error(ERR_ILL_STRUCTURE_OPERATION);
            else if (isvoid(*tp) || isvoid(tp1) || ismsil(*tp) || ismsil(tp1))
                error(ERR_NOT_AN_ALLOWED_TYPE);
            else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            else if (ispointer(*tp))
            {
                if (isintconst(exp1))
                {
                    if (!isconstzero(tp1, exp1))
                        error(ERR_NONPORTABLE_POINTER_COMPARISON);
                }
                else if (isint(tp1))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
                else if (isfloat(tp1) || isimaginary(tp1))
                    error(ERR_ILL_USE_OF_FLOATING);
                else if (iscomplex(tp1))
                    error(ERR_ILL_USE_OF_COMPLEX);
                else if (ispointer(tp1) && !isvoidptr(*tp) && !isvoidptr(tp1) && !comparetypes(*tp, tp1, false))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
            }
            else if (ispointer(tp1))
            {
                if (isintconst(*exp))
                {
                    if (!isconstzero(*tp, *exp))
                        error(ERR_NONPORTABLE_POINTER_COMPARISON);
                }
                else if (isint(*tp))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
                else if (isfloat(*tp) || isimaginary(*tp))
                    error(ERR_ILL_USE_OF_FLOATING);
                else if (iscomplex(*tp))
                    error(ERR_ILL_USE_OF_COMPLEX);
            }
            else if (isint(*tp) && isint(tp1))
            {
                if ((isunsigned(*tp) && !isunsigned(tp1)) || (isunsigned(tp1) && !isunsigned(*tp)))
                    errorstr(ERR_SIGNED_UNSIGNED_MISMATCH_RELAT, opname);
            }
            *tp = destSize(*tp, tp1, exp, &exp1, true, nullptr);
            switch (kw)
            {
                case gt:
                    type = isunsigned(*tp) ? en_ugt : en_gt;
                    break;
                case geq:
                    type = isunsigned(*tp) ? en_uge : en_ge;
                    break;
                case lt:
                    type = isunsigned(*tp) ? en_ult : en_lt;
                    break;
                case leq:
                    type = isunsigned(*tp) ? en_ule : en_le;
                    break;
                default:
                    done = true;
                    break;
            }
            *exp = exprNode(type, *exp, exp1);
            if (Optimizer::cparams.prm_cplusplus)
                *tp = &stdbool;
            else
                *tp = &stdint;
        }
    }
    return lex;
}
static LEXLIST* expression_equality(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable,
                                    int flags)
{
    lex = expression_inequality(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while (MATCHKW(lex, eq) || MATCHKW(lex, neq))
    {
        bool done = false;
        TYPE* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        enum e_kw kw = KW(lex);
        lex = getsym();
        lex = expression_inequality(lex, funcsp, nullptr, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            return lex;
        }
        ResolveTemplateVariable(tp, exp, tp1, atp);
        ResolveTemplateVariable(&tp1, &exp1, *tp, atp);
        if (isstructuredmath(*tp, tp1))
        {
            if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                insertOperatorFunc(ovcl_binary_numericptr, kw, funcsp, tp, exp, tp1, exp1, nullptr, flags))
            {
                continue;
            }
            else
            {
                if (Optimizer::cparams.prm_cplusplus && *tp && isstructured(*tp))
                {
                    if (!castToArithmeticInternal(false, tp, exp, kw, tp1, true))
                        castToPointer(tp, exp, kw, ispointer(tp1) ? tp1 : &stdpointer);
                }
                if (Optimizer::cparams.prm_cplusplus && tp1 && isstructured(tp1))
                {
                    if (!castToArithmeticInternal(false, &tp1, &exp1, (enum e_kw) - 1, *tp, true))
                        castToPointer(&tp1, &exp1, (enum e_kw) -1, ispointer(*tp) ? *tp : &stdpointer);
                }
            }
        }
        checkscope(*tp, tp1);
        if (Optimizer::cparams.prm_cplusplus)
        {
            SYMBOL* funcsp = nullptr;
            if ((ispointer(*tp) || basetype(*tp)->type == bt_memberptr) && tp1->type == bt_aggregate)
            {
                if (tp1->syms->size() > 1)
                    errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)tp1->syms->front())->name);
                exp1 = varNode(en_pc, tp1->syms->front());
                tp1 = ((SYMBOL*)tp1->syms->front())->tp;
            }
            else if ((ispointer(tp1) || basetype(tp1)->type == bt_memberptr) && (*tp)->type == bt_aggregate)
            {
                if (tp1->syms->size() > 1)
                    errorstr(ERR_OVERLOADED_FUNCTION_AMBIGUOUS, ((SYMBOL*)(*tp)->syms->front())->name);
                (*exp) = varNode(en_pc, (*tp)->syms->front());
                (*tp) = ((SYMBOL*)(*tp)->syms->front())->tp;
            }
        }
        if ((*exp)->type == en_pc || ((*exp)->type == en_func && !(*exp)->v.func->ascall))
            thunkForImportTable(exp);
        if (exp1->type == en_pc || (exp1->type == en_func && !exp1->v.func->ascall))
            thunkForImportTable(&exp1);
        if (isstructured(*tp) || isstructured(tp1))
        {
            if ((Optimizer::architecture != ARCHITECTURE_MSIL) || (!isconstzero(*tp, *exp) && !isconstzero(tp1, exp1)))
                error(ERR_ILL_STRUCTURE_OPERATION);
        }
        else if (isvoid(*tp) || isvoid(tp1) || ismsil(*tp) || ismsil(tp1))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        if (ispointer(*tp))
        {
            if (isintconst(exp1))
            {
                if (!isconstzero(tp1, exp1))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
            }
            else if (isint(tp1))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
            else if (isfloat(tp1) || isimaginary(tp1))
                error(ERR_ILL_USE_OF_FLOATING);
            else if (iscomplex(tp1))
                error(ERR_ILL_USE_OF_COMPLEX);
            else if (ispointer(tp1) && !isvoidptr(*tp) && !isvoidptr(tp1) && !comparetypes(*tp, tp1, false))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
        }
        else if (ispointer(tp1))
        {
            if (isintconst(*exp))
            {
                if (!isconstzero(*tp, *exp))
                    error(ERR_NONPORTABLE_POINTER_COMPARISON);
            }
            else if (isint(*tp))
                error(ERR_NONPORTABLE_POINTER_COMPARISON);
            else if (isfloat(*tp) || isimaginary(*tp))
                error(ERR_ILL_USE_OF_FLOATING);
            else if (iscomplex(*tp))
                error(ERR_ILL_USE_OF_COMPLEX);
        }
        else if (basetype(*tp)->type == bt_memberptr)
        {
            if (basetype(tp1)->type == bt_memberptr)
            {
                if (!comparetypes(basetype(*tp)->btp, basetype(tp1)->btp, true))
                {
                    error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                }
                *exp = exprNode(en_mp_compare, *exp, exp1);
                (*exp)->size = *tp;
                if (kw == neq)
                    *exp = exprNode(en_not, *exp, nullptr);
                done = true;
            }
            else if (isconstzero(tp1, exp1))
            {
                *exp = exprNode(en_mp_as_bool, *exp, nullptr);
                (*exp)->size = *tp;
                if (kw == eq)
                    *exp = exprNode(en_not, *exp, nullptr);
                done = true;
            }
            else if (comparetypes(basetype(*tp)->btp, tp1, true))
            {
                int lbl = dumpMemberPtr(exp1->v.sp, *tp, true);
                exp1 = intNode(en_labcon, lbl);
                *exp = exprNode(en_mp_compare, *exp, exp1);
                (*exp)->size = *tp;
                if (kw == neq)
                    *exp = exprNode(en_not, *exp, nullptr);
                done = true;
            }
            else
            {
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            }
        }
        else if (basetype(tp1)->type == bt_memberptr)
        {
            if (isconstzero(*tp, *exp))
            {
                *exp = exprNode(en_mp_as_bool, exp1, nullptr);
                (*exp)->size = tp1;
                if (kw == eq)
                    *exp = exprNode(en_not, *exp, nullptr);
                done = true;
            }
            else if (comparetypes(*tp, basetype(tp1)->btp, true))
            {
                int lbl = dumpMemberPtr((*exp)->v.sp, tp1, true);
                *(exp) = intNode(en_labcon, lbl);
                *exp = exprNode(en_mp_compare, *exp, exp1);
                (*exp)->size = tp1;
                if (kw == neq)
                    *exp = exprNode(en_not, *exp, nullptr);
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
                (!fittedConst(*tp, *exp) && !fittedConst(tp1, exp1)) || !isint(*tp) || !isint(tp1))
                if (!isstructured(*tp) && !isstructured(tp1))
                    destSize(*tp, tp1, exp, &exp1, true, nullptr);
            *exp = exprNode(kw == eq ? en_eq : en_ne, *exp, exp1);
        }
        if (Optimizer::cparams.prm_cplusplus)
            *tp = &stdbool;
        else
            *tp = &stdint;
    }
    return lex;
}
void GetLogicalDestructors(std::list<struct expr*>** rv, EXPRESSION* cur)
{

    if (!cur || cur->type == en_land || cur->type == en_lor || cur->type == en_cond)
        return;
    // nots are genned with a branch, so, get past them...
    while (cur->type == en_not)
        cur = cur->left;
    if (cur->type == en_func)
    {
        if (cur->v.func->arguments)
            for (auto t : *cur->v.func->arguments)
                if (isref(t->tp))
                    GetLogicalDestructors(rv, t->exp);
        if (cur->v.func->thisptr)
            GetLogicalDestructors(rv, cur->v.func->thisptr);
        if (cur->v.func->returnSP)
        {
            SYMBOL* sym = cur->v.func->returnSP;
            if (!sym->sb->destructed && sym->sb->dest && sym->sb->dest->front()->exp && !basetype(sym->tp)->sp->sb->trivialCons)
            {
                Optimizer::LIST* listitem;
                sym->sb->destructed = true;
                if (!*rv)
                    *rv = exprListFactory.CreateList();
                (*rv)->push_front(sym->sb->dest->front()->exp);
            }
        }
        else if (cur->v.func->sp->sb->isConstructor)
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
static void GetAssignDestructors (std::list<EXPRESSION*> **rv, EXPRESSION *exp)
{
    while (castvalue(exp) || lvalue(exp) || exp->type == en_thisref)
        exp = exp->left;
    if (exp->type == en_assign)
    {
        GetAssignDestructors(rv, exp->right);
        GetAssignDestructors(rv, exp->left);
    }
    else if (exp->type == en_func && exp->v.func->arguments)
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
    }
}

static LEXLIST* binop(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, enum e_kw kw, enum e_node type,
                      LEXLIST*(nextFunc)(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable,
                                         int flags),
                      bool* ismutable, int flags)
{
    bool first = true;
    lex = (*nextFunc)(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while (MATCHKW(lex, kw))
    {
        TYPE* tp1 = nullptr;
        EXPRESSION* exp1 = nullptr;
        std::list<struct expr*>* logicaldestructorsleft = nullptr;
        std::list<struct expr*>* logicaldestructorsright = nullptr;
        if (first && (kw == land || kw == lor))
        {
            first = false;
            GetLogicalDestructors(&logicaldestructorsleft, *exp);
            if (Optimizer::cparams.prm_cplusplus && *tp && isstructured(*tp))
            {
                if (!castToArithmeticInternal(false, tp, exp, (enum e_kw) - 1, &stdbool, false))
                    if (!castToArithmeticInternal(false, tp, exp, (enum e_kw) - 1, &stdint, false))
                        if (!castToPointer(tp, exp, (enum e_kw) - 1, &stdpointer))
                            errortype(ERR_CANNOT_CONVERT_TYPE, *tp, &stdint);
            }
        }
        lex = getsym();
        lex = (*nextFunc)(lex, funcsp, atp, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            *tp = nullptr;
            break;
        }
        if (kw == land || kw == lor)
        {
            GetLogicalDestructors(&logicaldestructorsright, exp1);
            if (Optimizer::cparams.prm_cplusplus && tp1 && isstructured(tp1))
            {
                if (!castToArithmeticInternal(false, &tp1, &exp1, (enum e_kw) - 1, &stdbool, false))
                    if (!castToArithmeticInternal(false, &tp1, &exp1, (enum e_kw) - 1, &stdint, false))
                        if (!castToPointer(&tp1, &exp1, (enum e_kw) - 1, &stdpointer))
                            errortype(ERR_CANNOT_CONVERT_TYPE, tp1, &stdint);
            }
        }
        if (isstructuredmath(*tp, tp1))
        {
            if (Optimizer::cparams.prm_cplusplus &&
                insertOperatorFunc(kw == lor || kw == land ? ovcl_binary_numericptr : ovcl_binary_int, kw, funcsp, tp, exp, tp1, exp1,
                    nullptr, flags))
            {
                continue;
            }
            if (kw == land || kw == lor)
            {
                castToArithmetic(kw != land && kw != lor, tp, exp, kw, &stdbool, kw != land && kw != lor);
                castToArithmetic(kw != land && kw != lor, &tp1, &exp1, (enum e_kw)-1, &stdbool, kw != land && kw != lor);
            }
            else
            {
                castToArithmetic(kw != land && kw != lor, tp, exp, kw, tp1, kw != land && kw != lor);
                castToArithmetic(kw != land && kw != lor, &tp1, &exp1, (enum e_kw)-1, *tp, kw != land && kw != lor);
            }
            LookupSingleAggregate(*tp, exp);
            LookupSingleAggregate(tp1, &exp1);
        }
        if (isstructured(*tp) || isstructured(tp1))
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if (isvoid(*tp) || isvoid(tp1) || ismsil(*tp) || ismsil(tp1))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if ((basetype(*tp)->scoped || basetype(tp1)->scoped) && !(flags & _F_SCOPEDENUM) &&
                 ((Optimizer::architecture != ARCHITECTURE_MSIL) || Optimizer::cparams.msilAllowExtensions))
            error(ERR_SCOPED_TYPE_MISMATCH);
        if (type != en_lor && type != en_land)
        {
            if (ispointer(*tp) || ispointer(tp1))
                error(ERR_ILL_POINTER_OPERATION);
            else if (isfloat(*tp) || isfloat(tp1) || isimaginary(*tp) || isimaginary(tp1))
                error(ERR_ILL_USE_OF_FLOATING);
            else if (iscomplex(*tp) || iscomplex(tp1))
                error(ERR_ILL_USE_OF_COMPLEX);
            else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
            *tp = destSize(*tp, tp1, exp, &exp1, true, atp);
        }
        else
        {
            if (Optimizer::cparams.prm_cplusplus)
                *tp = &stdbool;
            else
                *tp = &stdint;
        }
        if (basetype(*tp)->type == bt_memberptr)
        {
            *exp = exprNode(en_mp_as_bool, *exp, nullptr);
            (*exp)->size = *tp;
        }
        if (basetype(tp1)->type == bt_memberptr)
        {
            exp1 = exprNode(en_mp_as_bool, exp1, nullptr);
            exp1->size = tp1;
        }
        *exp = exprNode(type, *exp, exp1);
        (*exp)->v.logicaldestructors.left = logicaldestructorsleft;
        (*exp)->v.logicaldestructors.right = logicaldestructorsright;
    }
    return lex;
}
static LEXLIST* expression_and(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, andx, en_and, expression_equality, ismutable, flags);
}
static LEXLIST* expression_xor(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, uparrow, en_xor, expression_and, ismutable, flags);
}
static LEXLIST* expression_or(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, orx, en_or, expression_xor, ismutable, flags);
}
static LEXLIST* expression_land(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, land, en_land, expression_or, ismutable, flags);
}
static LEXLIST* expression_lor(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    return binop(lex, funcsp, atp, tp, exp, lor, en_lor, expression_land, ismutable, flags);
}

static LEXLIST* expression_hook(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    lex = expression_lor(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    if (MATCHKW(lex, hook))
    {
        TYPE *tph = nullptr, *tpc = nullptr;
        EXPRESSION *eph = nullptr, *epc = nullptr;
        if (Optimizer::cparams.prm_cplusplus && *tp && isstructured(*tp))
        {
            if (!castToArithmeticInternal(false, tp, exp, (enum e_kw) - 1, &stdbool, false))
                if (!castToArithmeticInternal(false, tp, exp, (enum e_kw) - 1, &stdint, false))
                    if (!castToPointer(tp, exp, (enum e_kw) - 1, &stdpointer))
                        errortype(ERR_CANNOT_CONVERT_TYPE, *tp, &stdint);
        }
        std::list<EXPRESSION*>* logicaldestructors = nullptr;
        GetLogicalDestructors(&logicaldestructors, *exp);
        LookupSingleAggregate(*tp, exp);

        if (isstructured(*tp))
            error(ERR_ILL_STRUCTURE_OPERATION);
        else if (isvoid(*tp) || ismsil(*tp))
            error(ERR_NOT_AN_ALLOWED_TYPE);
        lex = getsym();
        isCallNoreturnFunction = false;
        if (MATCHKW(lex, colon))
        {
            // replicate the selector into the 'true' value
            eph = anonymousVar(sc_auto, *tp);
            deref(*tp, &eph);
            tph = *tp;
            *exp = exprNode(en_assign, eph, *exp);
        }
        else
        {
            lex = expression_comma(lex, funcsp, nullptr, &tph, &eph, nullptr, flags);
        }
        bool oldCallExit = isCallNoreturnFunction;
        isCallNoreturnFunction = false;
        if (!tph)
        {
            *tp = nullptr;
        }
        else if (MATCHKW(lex, colon))
        {
            lex = getsym();
            lex = expression_assign(lex, funcsp, nullptr, &tpc, &epc, nullptr, flags);
            isCallNoreturnFunction &= oldCallExit;
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
                if (basetype(*tp)->type == bt_memberptr)
                {
                    *exp = exprNode(en_mp_as_bool, *exp, nullptr);
                    (*exp)->size = *tp;
                }
                if (tph->type == bt_void)
                    tph = tpc;
                else if (tpc->type == bt_void)
                    tpc = tph;
                if (Optimizer::cparams.prm_cplusplus && (isstructured(tpc) || isstructured(tph)))
                {
                    if ( ! isstructured(tpc) || !isstructured(tph) || (comparetypes(tph, tpc, false) && !sameTemplate(tph, tpc, false)) || epc->type == en_thisref || epc->type == en_func || eph->type == en_thisref || eph->type == en_func)
                    {
                        // structure is result of constructor or return value
                        // at this point we want to check that both sides
                        // are the same type; if not make a constructor
                        EXPRESSION* rv = nullptr;
                        if (!comparetypes(tph, tpc, false))
                        {
                            if (atp && isstructured(atp))
                            {
                                if (!comparetypes(atp, tpc, false) || !sameTemplate(atp, tpc))
                                {
                                    rv = anonymousVar(sc_auto, tph);
                                    EXPRESSION* exp = rv;
                                    TYPE* ctype = atp;
                                    callConstructorParam(&ctype, &exp, tpc, epc, true, false, false, false, true);
                                    epc = exp;
                                    tpc = atp;
                                }

                                if (!comparetypes(atp, tph, false) || !sameTemplate(atp, tph))
                                {
                                    rv = anonymousVar(sc_auto, tph);
                                    EXPRESSION* exp = rv;
                                    TYPE* ctype = atp;
                                    callConstructorParam(&ctype, &exp, tph, eph, true, false, false, false, true);
                                    eph = exp;
                                    tph = CopyType(atp);
                                    tph->lref = tph->rref = false;
                                }
                            }
                            else if (isstructured(tph))
                            {
                                rv = anonymousVar(sc_auto, tph);
                                EXPRESSION* exp = rv;
                                TYPE* ctype = tph;
                                callConstructorParam(&ctype, &exp, tpc, epc, true, false, false, false, true);
                                epc = exp;
                                tpc = tph;
                            }
                            else
                            {
                                rv = anonymousVar(sc_auto, tpc);
                                EXPRESSION* exp = rv;
                                TYPE* ctype = tpc;
                                callConstructorParam(&ctype, &exp, tph, eph, true, false, false, false, true);
                                eph = exp;
                                tph = tpc;
                                tph->lref = tph->rref = false;
                            }
                        }
                        // now check for structured alias types
                        if (basetype(tpc)->sp->sb->structuredAliasType)
                        {
                            auto srp = basetype(tpc)->sp->sb->structuredAliasType;
                            rv = anonymousVar(sc_auto, srp);
                            epc = exprNode(en_blockassign, rv, epc);
                            epc->size = tpc;
                            epc->altdata = (void*)(tpc);
                            eph = exprNode(en_blockassign, rv, eph);
                            eph->size = tph;
                            eph->altdata = (void*)(tph);
                        }
                        else
                        {
                            // now make sure both sides are in an anonymous variable
                            if (tph->lref || tph->rref || (eph->type != en_func && eph->type != en_thisref))
                            {
                                if (!rv)
                                    rv = anonymousVar(sc_auto, tph);
                                EXPRESSION* exp = rv;
                                TYPE* ctype = tph;
                                callConstructorParam(&ctype, &exp, tph, eph, true, false, false, false, true);
                                eph = exp;
                            }
                            if (tpc->lref || tpc->rref || (epc->type != en_func && epc->type != en_thisref))
                            {
                                //if (!rv)
                                    rv = anonymousVar(sc_auto, tph);
                                EXPRESSION* exp = rv;
                                TYPE* ctype = tpc;
                                callConstructorParam(&ctype, &exp, tpc, epc, true, false, false, false, true);
                                epc = exp;
                            }
                            // now make sure both sides are using the same anonymous variable
                            if (!rv)
                                rv = anonymousVar(sc_auto, tph);
                            EXPRESSION* dexp = rv;
                            callDestructor(basetype(tph)->sp, nullptr, &dexp, nullptr, true, false, false, true);
                            initInsert(&rv->v.sp->sb->dest, rv->v.sp->tp, dexp, 0, true);

                            EXPRESSION* exp = eph;
                            if (exp->type == en_thisref)
                                exp = exp->left;
                            if (exp->v.func->returnSP)
                            {
                                if (exp->v.func->returnEXP->type == en_auto && exp->v.func->returnEXP->v.sp != rv->v.sp)
                                    exp->v.func->returnEXP->v.sp->sb->allocate = false;
                                exp->v.func->returnEXP = rv;
                            }
                            else if (exp->v.func->thisptr)
                            {
                                if (exp->v.func->thisptr->type == en_auto && exp->v.func->thisptr->v.sp != rv->v.sp)
                                    exp->v.func->thisptr->v.sp->sb->allocate = false;
                                exp->v.func->thisptr = rv;
                            }
                            exp = epc;
                            if (exp->type == en_thisref)
                                exp = exp->left;
                            if (exp->v.func->returnSP)
                            {
                                if (exp->v.func->returnEXP->type == en_auto && exp->v.func->returnEXP->v.sp != rv->v.sp)
                                    exp->v.func->returnEXP->v.sp->sb->allocate = false;
                                exp->v.func->returnEXP = rv;
                            }
                            else if (exp->v.func->thisptr)
                            {
                                if (exp->v.func->thisptr->type == en_auto && exp->v.func->thisptr->v.sp != rv->v.sp)
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
                if (ispointer(tph) || ispointer(tpc))
                    if (!comparetypes(tph, tpc, false))
                        if (!isconstzero(tph, eph) && !isconstzero(tpc, epc))
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                if (isfunction(tph) || isfunction(tpc))
                    if (!comparetypes(tph, tpc, true))
                        if (!isconstzero(tph, eph) && !isconstzero(tpc, epc))
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                if (tph != tpc && (ispointer(tph) || isfunction(tph) || ispointer(tpc) || isfunction(tpc)))
                {
                    if (!comparetypes(tpc, tph, true))
                    {
                        if ((isvoidptr(tpc) && ispointer(tph)) || (isvoidptr(tph) && ispointer(tpc)))
                        {
                            if (tph->nullptrType)
                                tph = tpc;
                            else if (tpc->nullptrType)
                                tpc = tph;
                            else
                                tpc = tph = &stdpointer;
                        }
                        else if (!((ispointer(tph) || isfunction(tph))) && !((ispointer(tpc) || isfunction(tpc))))
                        {
                            if (!comparetypes(tpc, tph, false))
                                if (!(isarithmetic(tpc) && isarithmetic(tph)))
                                    errortype(ERR_TWO_OPERANDS_SAME_TYPE, tpc, tph);
                        }
                        else if ((isfunction(tph) || isfunction(tpc)) && !comparetypes(tpc, tph, true))
                            if (!isarithmetic(tph) && !isarithmetic(tpc))
                                errortype(ERR_TWO_OPERANDS_SAME_TYPE, tpc, tph);
                    }
                }
                if (isfunction(tph))
                    *tp = tph;
                else if (isfunction(tpc))
                    *tp = tpc;
                else if (!isvoid(tpc))
                    *tp = destSize(tpc, tph, &epc, &eph, false, nullptr);
                else
                {
                    if (Optimizer::architecture == ARCHITECTURE_MSIL)
                    {
                        EXPRESSION* exp1 = eph;
                        while (castvalue(exp1))
                            exp1 = exp1->left;
                        if (exp1->type == en_thisref)
                            exp1 = exp1->left;
                        if (exp1->type == en_func)
                            if (isvoid(basetype(exp1->v.func->sp->tp)->btp))
                                eph = exprNode(en_void, eph, intNode(en_c_i, 0));
                        exp1 = epc;
                        while (castvalue(exp1))
                            exp1 = exp1->left;
                        if (exp1->type == en_thisref)
                            exp1 = exp1->left;
                        if (exp1->type == en_func)
                            if (isvoid(basetype(exp1->v.func->sp->tp)->btp))
                                epc = exprNode(en_void, epc, intNode(en_c_i, 0));
                    }
                    *tp = tpc;
                }
                *exp = exprNode(en_cond, *exp, exprNode(en_void, eph, epc));
                (*exp)->v.logicaldestructors.left = logicaldestructors;
                // when assigning a structure to itself, need an intermediate copy
                // this always puts it in...
                if (Optimizer::cparams.prm_cplusplus && isstructured(*tp) && atp && !basetype(*tp)->sp->sb->structuredAliasType)
                {
                    EXPRESSION* rv = anonymousVar(sc_auto, *tp);
                    TYPE* ctype = *tp;
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
    return lex;
}
static bool isTemplatedPointer(TYPE* tp)
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
LEXLIST* expression_throw(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp)
{
    TYPE* tp1 = nullptr;
    EXPRESSION* exp1 = nullptr;
    *tp = &stdvoid;
    hasXCInfo = true;
    lex = getsym();
    functionCanThrow = true;
    if (!MATCHKW(lex, semicolon))
    {
        SYMBOL* sym = namespacesearch("_ThrowException", globalNameSpace, false, false);
        makeXCTab(funcsp);
        lex = expression_assign(lex, funcsp, nullptr, &tp1, &exp1, nullptr, 0);
        if (!tp1)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }
        else if (sym)
        {
            FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
            INITLIST* arg1 = Allocate<INITLIST>();  // exception table
            INITLIST* arg2 = Allocate<INITLIST>();  // instance
            INITLIST* arg3 = Allocate<INITLIST>();  // array size
            INITLIST* arg4 = Allocate<INITLIST>();  // constructor
            INITLIST* arg5 = Allocate<INITLIST>();  // exception block
            SYMBOL* rtti = RTTIDumpType(tp1, true);
            SYMBOL* cons = nullptr;
            if (isstructured(tp1))
            {
                cons = getCopyCons(basetype(tp1)->sp, false);
                if (cons && !cons->sb->inlineFunc.stmt)
                {
                    if (cons->sb->defaulted)
                        createConstructor(basetype(tp1)->sp, cons);
                }
            }
            sym = (SYMBOL*)basetype(sym->tp)->syms->front();
            arg1->exp = varNode(en_auto, funcsp->sb->xc->xctab);
            arg1->tp = &stdpointer;
            if (isstructured(tp1))
            {
                arg2->exp = exp1;
            }
            else
            {
                EXPRESSION* exp3 = anonymousVar(sc_auto, tp1);
                arg2->exp = exp3;
                deref(tp1->type == bt_pointer ? &stdpointer : tp1, &exp3);
                exp3 = exprNode(en_assign, exp3, exp1);
                arg2->exp = exprNode(en_void, exp3, arg2->exp);
            }
            arg2->tp = &stdpointer;
            arg3->exp = isarray(tp1) ? intNode(en_c_i, tp1->size / (basetype(tp1)->btp->size)) : intNode(en_c_i, 1);
            arg3->tp = &stdint;
            arg4->exp = cons ? varNode(en_pc, cons) : intNode(en_c_i, 0);
            arg4->tp = &stdpointer;
            if (cons && cons->sb->attribs.inheritable.linkage2 == lk_import)
            {
                arg4->exp = exprNode(en_l_p, arg4->exp, nullptr);
            }
            arg5->exp = rtti ? varNode(en_global, rtti) : intNode(en_c_i, 0);
            arg5->tp = &stdpointer;
            params->arguments = initListListFactory.CreateList();
            params->arguments->push_back(arg1);
            params->arguments->push_back(arg2);
            params->arguments->push_back(arg3);
            params->arguments->push_back(arg4);
            params->arguments->push_back(arg5);
            params->ascall = true;
            params->sp = sym;
            params->functp = sym->tp;
            params->fcall = varNode(en_pc, sym);
            params->rttiType = rtti;
            *exp = exprNode(en_func, nullptr, nullptr);
            (*exp)->v.func = params;
        }
    }
    else
    {
        SYMBOL* sym = namespacesearch("_RethrowException", globalNameSpace, false, false);
        if (sym)
        {
            FUNCTIONCALL* parms = Allocate<FUNCTIONCALL>();
            INITLIST* arg1 = Allocate<INITLIST>();  // exception table
            makeXCTab(funcsp);
            sym = (SYMBOL*)basetype(sym->tp)->syms->front();
            parms->ascall = true;
            parms->sp = sym;
            parms->functp = sym->tp;
            parms->fcall = varNode(en_pc, sym);
            parms->arguments = initListListFactory.CreateList();
            parms->arguments->push_back(arg1);
            arg1->exp = varNode(en_auto, funcsp->sb->xc->xctab);
            arg1->tp = &stdpointer;
            *exp = exprNode(en_func, nullptr, nullptr);
            (*exp)->v.func = parms;
        }
    }
    isCallNoreturnFunction = true;
    return lex;
}
static void ReplaceThisAssign(EXPRESSION** init, SYMBOL* sym, EXPRESSION* exp)
{
    if (*init)
    {
        if ((*init)->type == en_global)
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
LEXLIST* expression_assign(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    bool done = false;
    EXPRESSION *exp1 = nullptr, **exp2;
    EXPRESSION* asndest = nullptr;

    bool localMutable = false;
    TYPE* tp2;
    if (MATCHKW(lex, kw_throw))
    {
        return expression_throw(lex, funcsp, tp, exp);
    }
    lex = expression_hook(lex, funcsp, atp, tp, exp, &localMutable, flags);
    if (*tp == nullptr)
        return lex;
    while (!done && lex)
    {
        enum e_kw kw = KW(lex);
        enum e_node op;
        enum ovcl selovcl;
        TYPE* tp1 = nullptr;
        EXPRESSION* symRef;
        switch (kw)
        {
            case asand:
            case asor:
            case asxor:
            case asmod:
            case asleftshift:
            case asrightshift:
                selovcl = ovcl_assign_int;
                break;
            case astimes:
            case asdivide:
                selovcl = ovcl_assign_numeric;
                break;
            case assign:
                selovcl = ovcl_assign_any;
                break;
            case asplus:
            case asminus:
                selovcl = ovcl_assign_numericptr;
                break;
            default:
                if (ismutable)
                    *ismutable = localMutable;
                done = true;
                continue;
        }
        lex = getsym();
        switch (kw)
        {
            case assign:
                if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, begin))
                {
                    if (isstructured(*tp))
                    {
                        std::list<INITIALIZER*>* init = nullptr;
                        SYMBOL* spinit = nullptr;
                        tp1 = *tp;
                        spinit = anonymousVar(sc_localstatic, tp1)->v.sp;
                        localNameSpace->front()->syms->Add(spinit);
                        lex = initType(lex, funcsp, 0, sc_auto, &init, nullptr, tp1, spinit, false, flags | _F_ASSIGNINIT);
                        EXPRESSION* exp2 = nullptr;
                        if (init && init->front()->exp->type == en_thisref)
                        {
                            ReplaceThisAssign(&init->front()->exp->left->v.func->thisptr, spinit, *exp);
                            spinit->sb->allocate = false;
                        }
                        else
                        {
                            exp2 = *exp;
                        }
                        *exp = convertInitToExpression(tp1, nullptr, exp2, funcsp, init, exp1, false);

                        return lex;
                    }
                    else
                    {
                        lex = getsym();
                        ++inAssignRHS;
                        lex = expression_assign(lex, funcsp, *tp, &tp1, &exp1, nullptr, flags);
                        --inAssignRHS;
                        if (!needkw(&lex, end))
                        {
                            errskim(&lex, skim_end);
                            skip(&lex, end);
                        }
                    }
                }
                else
                {
                    ++inAssignRHS;
                    lex = expression_assign(lex, funcsp, *tp, &tp1, &exp1, nullptr, flags);
                    --inAssignRHS;
                }
                break;
            case asplus:
            case asminus:
            case asand:
            case asor:
            case asxor:
                ++inAssignRHS;
                lex = expression_assign(lex, funcsp, *tp, &tp1, &exp1, nullptr, flags);
                --inAssignRHS;
                break;
            default:
                ++inAssignRHS;
                lex = expression_assign(lex, funcsp, nullptr, &tp1, &exp1, nullptr, flags);
                --inAssignRHS;
                break;
        }
        if (!tp1)
        {
            *tp = nullptr;
            return lex;
        }
        ResolveTemplateVariable(tp, exp, tp1, nullptr);
        ResolveTemplateVariable(&tp1, &exp1, *tp, nullptr);
        ConstExprPromote(*exp, false);
        if (isstructuredmath(*tp, tp1))
        {
            if ((Optimizer::cparams.prm_cplusplus || Optimizer::architecture == ARCHITECTURE_MSIL) &&
                insertOperatorFunc(selovcl, kw, funcsp, tp, exp, tp1, exp1, nullptr, flags))
            {
                // unallocated var for destructor
                if (!inAssignRHS)
                {
                    GetAssignDestructors(&(*exp)->v.func->destructors, *exp);
                }
                if (asndest)
                {
                    SYMBOL* sym = anonymousVar(sc_auto, tp1)->v.sp;
                    callDestructor(basetype(tp1)->sp, nullptr, &asndest, nullptr, true, false, false, true);
                    initInsert(&sym->sb->dest, tp1, asndest, 0, true);
                }

                continue;
            }
        }
        if (kw == assign && (flags & _F_SELECTOR))
        {
            /* a little naive... */
            error(ERR_POSSIBLE_INCORRECT_ASSIGNMENT);
            flags &= ~_F_SELECTOR;
        }
        checkscope(*tp, tp1);
        if (Optimizer::cparams.prm_cplusplus)
        {
            if (isarithmetic(*tp))
            {
                castToArithmetic(false, &tp1, &exp1, (enum e_kw) - 1, *tp, true);
            }
            else if (isstructured(tp1))
            {
                cppCast(*tp, &tp1, &exp1);
            }
        }
        exp2 = &exp1;
        while (castvalue(*exp2))
            exp2 = &(*exp2)->left;
        if ((*exp2)->type == en_func && (*exp2)->v.func->sp->sb->storage_class == sc_overloads)
        {
            TYPE* tp2 = nullptr;
            SYMBOL* funcsp;
            if ((*exp2)->v.func->sp->sb->parentClass && !(*exp2)->v.func->asaddress)
                if ((*exp2)->v.func->sp->sb->storage_class == sc_member || (*exp2)->v.func->sp->sb->storage_class == sc_mutable)
                    error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
            funcsp = MatchOverloadedFunction((*tp), isfuncptr(*tp) || basetype(*tp)->type == bt_memberptr ? &tp1 : &tp2,
                                             (*exp2)->v.func->sp, exp2, flags);
            if (funcsp && basetype(*tp)->type == bt_memberptr)
            {
                int lbl = dumpMemberPtr(funcsp, *tp, true);
                exp1 = intNode(en_labcon, lbl);
            }
            if (funcsp)
            {
                tp1 = funcsp->tp;
                if (exp1->type == en_pc || (exp1->type == en_func && !exp1->v.func->ascall))
                {
                    thunkForImportTable(&exp1);
                }
            }
            if (basetype(*tp)->btp && !comparetypes(basetype(*tp)->btp, tp1, true))
            {
                if (!isvoidptr(*tp))
                    errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
            }
        }
        if (exp1->type == en_pc || (exp1->type == en_func && !exp1->v.func->ascall))
        {
            if (Optimizer::architecture == ARCHITECTURE_MSIL)
            {
                ValidateMSILFuncPtr(*tp, tp1, &exp1);
            }
        }
        EXPRESSION* temp = GetSymRef(*exp);
        symRef = (Optimizer::architecture == ARCHITECTURE_MSIL) ? temp : nullptr;
        LookupSingleAggregate(tp1, &exp1);

        if (isstructured(tp1))
        {
            SYMBOL* conv = lookupNonspecificCast(basetype(tp1)->sp, *tp);
            if (conv)
            {
                FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
                params->thisptr = exp1;
                params->thistp = tp1;
                params->ascall = true;
                params->functp = conv->tp;
                params->fcall = varNode(en_pc, conv);
                params->sp = conv;
                exp1 = exprNode(en_func, nullptr, nullptr);
                exp1->v.func = params;
                tp1 = basetype(conv->tp)->btp;
            }
        }
        if (((*exp)->type == en_const || isconstraw(*tp)) && !localMutable &&
            (!temp || temp->v.sp->sb->storage_class != sc_parameter || !isarray(*tp)) &&
            ((*exp)->type != en_func || !isconstraw(basetype((*exp)->v.func->sp->tp)->btp)))
            error(ERR_CANNOT_MODIFY_CONST_OBJECT);
        else if (isvoid(*tp) || isvoid(tp1) || (*tp)->type == bt_aggregate)
            error(ERR_NOT_AN_ALLOWED_TYPE);
        else if (!isstructured(*tp) && ((*tp)->btp && !ispointer((*tp)->btp)) && (!isarray(*tp) || !basetype(*tp)->msil) &&
                 basetype(*tp)->type != bt_memberptr && basetype(*tp)->type != bt_templateparam &&
                 basetype(*tp)->type != bt_templateselector && !lvalue(*exp) && (*exp)->type != en_msil_array_access)
            error(ERR_LVALUE);
        else if (symRef && symRef->v.sp->sb->attribs.inheritable.linkage2 == lk_property && !symRef->v.sp->sb->has_property_setter)
            errorsym(ERR_CANNOT_MODIFY_PROPERTY_WITHOUT_SETTER, symRef->v.sp);
        else if (isstructured(*tp) && !basetype(*tp)->sp->sb->trivialCons)
        {
            errorsym(ERR_NO_ASSIGNMENT_OPERATOR, basetype(*tp)->sp);
        }
        else
            switch (kw)
            {
                case asand:
                case asor:
                case asxor:
                case asleftshift:
                case asrightshift:
                    if (ismsil(*tp) || ismsil(tp1))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    if (iscomplex(*tp) || iscomplex(tp1))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    if (isfloat(*tp) || isfloat(tp1) || isimaginary(*tp) || isimaginary(tp1))
                        error(ERR_ILL_USE_OF_FLOATING);
                    /* fall through */
                case astimes:
                case asdivide:
                case asmod:
                    if (ismsil(*tp) || ismsil(tp1))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    if (ispointer(*tp) || ispointer(tp1))
                        error(ERR_ILL_POINTER_OPERATION);
                    if (isstructured(*tp) || isstructured(tp1))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    break;
                case asplus:
                    if ((*tp)->type == bt___string)
                    {
                        if (exp1->type == en_labcon && exp1->string)
                            exp1->type = en_c_string;
                        else if (!ismsil(tp1))
                            exp1 = exprNode(en_x_object, exp1, nullptr);
                    }
                    else if (ismsil(*tp) || ismsil(tp1))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    else if (ispointer(*tp))
                    {
                        if (ispointer(tp1))
                            error(ERR_ILL_POINTER_ADDITION);
                        else
                        {
                            EXPRESSION* ns;
                            if (iscomplex(tp1))
                                error(ERR_ILL_USE_OF_COMPLEX);
                            else if (isfloat(tp1) || isimaginary(tp1))
                                error(ERR_ILL_USE_OF_FLOATING);
                            else if (isstructured(tp1))
                                error(ERR_ILL_STRUCTURE_OPERATION);
                            if (basetype(basetype(*tp)->btp)->type == bt_void)
                            {
                                if (Optimizer::cparams.prm_cplusplus)
                                    error(ERR_ARITHMETIC_WITH_VOID_STAR);
                                cast(&stdint, &exp1);
                                ns = nodeSizeof(&stdchar, exp1);
                            }
                            else
                            {
                                cast(&stdint, &exp1);
                                ns = nodeSizeof(basetype(*tp)->btp, exp1);
                            }
                            exp1 = exprNode(en_umul, exp1, ns);
                        }
                    }
                    else if (ispointer(tp1))
                    {
                        error(ERR_ILL_POINTER_OPERATION);
                    }
                    else if (isstructured(*tp) || isstructured(tp1))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    break;
                case asminus:
                    if (ismsil(*tp) || ismsil(tp1))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    if (isstructured(*tp) || isstructured(tp1))
                        error(ERR_ILL_STRUCTURE_OPERATION);
                    else if (ispointer(tp1))
                    {
                        error(ERR_ILL_POINTER_SUBTRACTION);
                    }
                    else if (basetype(*tp)->type == bt_memberptr || basetype(tp1)->type == bt_memberptr)
                        error(ERR_ILLEGAL_USE_OF_MEMBER_PTR);
                    else if (ispointer(*tp))
                    {
                        EXPRESSION* ns;
                        if (basetype(basetype(*tp)->btp)->type == bt_void)
                        {
                            if (Optimizer::cparams.prm_cplusplus)
                                error(ERR_ARITHMETIC_WITH_VOID_STAR);
                            cast(&stdint, &exp1);
                            ns = nodeSizeof(&stdchar, exp1);
                        }
                        else
                        {
                            cast(&stdint, &exp1);
                            ns = nodeSizeof(basetype(*tp)->btp, exp1);
                        }
                        exp1 = exprNode(en_umul, exp1, ns);
                    }
                    break;
                case assign:
                    if (basetype(*tp)->type == bt___string)
                    {
                        if (exp1->type == en_labcon && exp1->string)
                            exp1->type = en_c_string;
                    }
                    else if (basetype(*tp)->type == bt___object)
                    {
                        if (tp1->type != bt___object && !isstructured(tp1) && (!isarray(tp1) || !basetype(tp1)->msil))
                            exp1 = exprNode(en_x_object, exp1, nullptr);
                    }
                    else if (ismsil(*tp) || ismsil(tp1))
                        error(ERR_NOT_AN_ALLOWED_TYPE);
                    if (ispointer(*tp))
                    {
                        if (isarray(*tp) && (*tp)->msil && !comparetypes(*tp, tp1, true) && natural_size(*exp) != ISZ_OBJECT)
                        {
                            *exp = exprNode(en_l_object, *exp, nullptr);
                            (*exp)->v.tp = tp1;
                        }
                        if (isarithmetic(tp1))
                        {
                            if (iscomplex(tp1))
                                error(ERR_ILL_USE_OF_COMPLEX);
                            else if (isfloat(tp1) || isimaginary(tp1))
                                error(ERR_ILL_USE_OF_FLOATING);
                            else if (isarithmeticconst(exp1))
                            {
                                if (!isintconst(exp1) || !isconstzero(*tp, exp1))
                                    error(ERR_NONPORTABLE_POINTER_CONVERSION);
                            }
                            else if (Optimizer::cparams.prm_cplusplus)
                            {
                                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                            }
                            else
                            {
                                error(ERR_NONPORTABLE_POINTER_CONVERSION);
                            }
                        }
                        else if (ispointer(basetype(tp1)) || tp1->type == bt_any)
                        {
                            if (Optimizer::cparams.prm_cplusplus && basetype(tp1)->stringconst && !isconst(basetype(*tp)->btp))
                                error(ERR_INVALID_CHARACTER_STRING_CONVERSION);

                            while (tp1->type == bt_any && tp1->btp)
                                tp1 = tp1->btp;
                            if (!ispointer(basetype(tp1)))
                                goto end;
                            if (!comparetypes(*tp, tp1, true))
                            {
                                bool found = false;
                                if (ispointer(tp1))
                                {
                                    TYPE* tpo = basetype(basetype(tp1)->btp);
                                    TYPE* tpn = basetype(basetype(*tp)->btp);
                                    if (isstructured(tpo) && isstructured(tpn))
                                    {
                                        if (classRefCount(tpo->sp, tpn->sp) == 1)
                                        {
                                            EXPRESSION* v = Allocate<EXPRESSION>();
                                            v->type = en_c_i;
                                            v = baseClassOffset(tpo->sp, tpn->sp, v);
                                            optimize_for_constants(&v);
                                            if (v->type == en_c_i)  // check for no virtual base
                                            {
                                                if (isAccessible(tpn->sp, tpn->sp, tpo->sp, funcsp, ac_public, false))
                                                {
                                                    *exp = exprNode(en_add, *exp, v);
                                                    found = true;
                                                }
                                            }
                                        }
                                        else if (classRefCount(tpn->sp, tpo->sp) == 1)
                                        {
                                            EXPRESSION* v = Allocate<EXPRESSION>();
                                            v->type = en_c_i;
                                            v = baseClassOffset(tpn->sp, tpo->sp, v);
                                            optimize_for_constants(&v);
                                            if (v->type == en_c_i)  // check for no virtual base
                                            {
                                                if (isAccessible(tpo->sp, tpo->sp, tpn->sp, funcsp, ac_public, false))
                                                {
                                                    *exp = exprNode(en_sub, *exp, v);
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
                                        if ((!isvoidptr(*tp) || !ispointer(tp1)) && !tp1->nullptrType)
                                            if (!isTemplatedPointer(*tp))
                                            {
                                                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                                            }
                                    }
                                    else if (!isvoidptr(*tp) && !isvoidptr(tp1))
                                    {
                                        if (!matchingCharTypes(*tp, tp1))
                                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                                    }
                                    else if (Optimizer::cparams.prm_cplusplus && !isvoidptr(*tp) && isvoidptr(tp1) &&
                                             exp1->type != en_nullptr)
                                    {
                                        error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                                    }
                                }
                            }
                        }
                        else if (isfunction(tp1))
                        {
                            if (!isvoidptr(*tp) && (!isfunction(basetype(*tp)->btp) ||
                                                    !comparetypes(basetype(basetype(*tp)->btp)->btp, basetype(tp1)->btp, true)))
                                error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                        }
                        else
                        {
                            if (tp1->type == bt_memberptr)
                            {
                                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                            }
                            else
                                error(ERR_INVALID_POINTER_CONVERSION);
                        }
                    end:;
                    }
                    else if (ispointer(tp1))
                    {
                        if (iscomplex(*tp))
                        {
                            error(ERR_ILL_USE_OF_COMPLEX);
                        }
                        else if (isfloat(*tp) || isimaginary(*tp))
                        {
                            error(ERR_ILL_USE_OF_FLOATING);
                        }
                        else if (isint(*tp))
                        {
                            if (basetype(*tp)->type != bt_bool)
                                error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                        else if (isarray(tp1) && (tp1)->msil)
                        {
                            error(ERR_MANAGED_OBJECT_NO_ADDRESS);
                        }
                    }
                    if (isstructured(*tp) && (!isstructured(tp1) || (!comparetypes(*tp, tp1, true) && !sameTemplate(tp1, *tp))))
                    {
                        if (!((Optimizer::architecture == ARCHITECTURE_MSIL) && basetype(*tp)->sp->sb->msil &&
                              (isconstzero(tp1, exp1) || basetype(tp1)->nullptrType)))
                        {
                            error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                        }
                    }
                    else if (isstructured(*tp) && !(*tp)->size)
                    {
                        if (!(flags & _F_SIZEOF))
                            errorsym(ERR_STRUCT_NOT_DEFINED, basetype(*tp)->sp);
                    }
                    else if (!isstructured(*tp) && isstructured(tp1))
                    {
                        error(ERR_ILL_STRUCTURE_ASSIGNMENT);
                    }
                    else if (basetype(*tp)->type == bt_memberptr)
                    {
                        if (exp1->type == en_memberptr)
                        {
                            if (exp1->v.sp != basetype(*tp)->sp && exp1->v.sp != basetype(*tp)->sp->sb->mainsym &&
                                !sameTemplate(exp1->v.sp->tp, basetype(*tp)->sp->tp))  // DAL FIXED
                                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                        }
                        else if ((!isfunction(basetype(*tp)->btp) || !comparetypes(basetype(*tp)->btp, tp1, true)) &&
                                 !isconstzero(tp1, *exp) && !comparetypes(*tp, tp1, true))
                        {
                            errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                        }
                    }
                    else if (basetype(tp1)->type == bt_memberptr)
                        errortype(ERR_CANNOT_CONVERT_TYPE, tp1, *tp);
                    break;
                default:
                    done = true;
                    break;
            }
        switch (kw)
        {
            case asand:
                op = en_and;
                break;
            case asor:
                op = en_or;
                break;
            case asxor:
                op = en_xor;
                break;

            case astimes:
                op = en_mul;
                break;
            case asdivide:
                tp2 = destSize(*tp, tp1, nullptr, nullptr, false, nullptr);
                op = isunsigned(*tp) && basetype(*tp)->type > bt_int ? en_udiv : en_div;
                break;
            case asmod:
                tp2 = destSize(*tp, tp1, nullptr, nullptr, false, nullptr);
                op = isunsigned(*tp) && basetype(*tp)->type > bt_int ? en_umod : en_mod;
                break;
            case assign:
                op = en_assign;
                break;

            case asplus:
                op = en_add;
                break;
            case asminus:
                op = en_sub;
                break;

            case asleftshift:
                op = en_lsh;
                break;
            case asrightshift:
                tp2 = destSize(*tp, tp1, nullptr, nullptr, false, nullptr);
                op = isunsigned(*tp) ? en_ursh : en_rsh;
                break;
            default:
                break;
        }
        if (ismsil(*tp))
        {
            if (exp1->type == en_labcon && exp1->string)
                exp1->type = en_c_string;
            else if (!ismsil(tp1) && !isstructured(tp1) && (!isarray(tp1) || !basetype(tp1)->msil))
                exp1 = exprNode(en_x_object, exp1, nullptr);
            if (op == en_assign)
            {
                *exp = exprNode(op, *exp, exp1);
            }
            else
            {
                EXPRESSION* dest = *exp;
                *exp = exprNode(op, *exp, exp1);
                *exp = exprNode(en_assign, dest, *exp);
            }
        }
        else if (basetype(*tp)->type == bt_memberptr)
        {
            if ((*exp)->type == en_not_lvalue || ((*exp)->type == en_func && !(*exp)->v.func->ascall) || (*exp)->type == en_void ||
                (*exp)->type == en_memberptr)
            {
                if (basetype(*tp)->type != bt_templateparam)
                    error(ERR_LVALUE);
            }
            else if (exp1->type == en_memberptr)
            {
                int lbl = dumpMemberPtr(exp1->v.sp, *tp, true);
                exp1 = intNode(en_labcon, lbl);
                *exp = exprNode(en_blockassign, *exp, exp1);
                (*exp)->size = *tp;
                (*exp)->altdata = (void*)(*tp);
            }
            else if (isconstzero(tp1, exp1) || exp1->type == en_nullptr)
            {
                *exp = exprNode(en_blockclear, *exp, nullptr);
                (*exp)->size = *tp;
            }
            else if (exp1->type == en_func && exp1->v.func->returnSP)
            {
                exp1->v.func->returnSP->sb->allocate = false;
                exp1->v.func->returnEXP = *exp;
                *exp = exp1;
            }
            else
            {
                *exp = exprNode(en_blockassign, *exp, exp1);
                (*exp)->size = *tp;
                (*exp)->altdata = (void*)(*tp);
            }
        }
        else if (isstructured(*tp))
        {
            EXPRESSION* exp2 = exp1;
            if (((*exp)->type == en_not_lvalue || ((*exp)->type == en_func && (!(*exp)->v.func->ascall || (*exp)->v.func->returnSP)) || ((*exp)->type == en_void) && !(flags & _F_SIZEOF)))
                error(ERR_LVALUE);
            if (lvalue(exp2))
                exp2 = exp2->left;
            if (exp2->type == en_func && exp2->v.func->returnSP)
            {
                exp2->v.func->returnSP->sb->allocate = false;
                exp2->v.func->returnEXP = *exp;
                *exp = exp1;
            }
            else
            {
                *exp = exprNode(en_blockassign, *exp, exp1);
                (*exp)->size = *tp;
                (*exp)->altdata = (void*)(*tp);
                if (isatomic(*tp))
                {
                    (*exp)->size = CopyType((*exp)->size);
                    (*exp)->size->size -= ATOMIC_FLAG_SPACE;
                }
            }
            *exp = exprNode(en_not_lvalue, *exp, nullptr);
        }
        else
        {
            if (kw == assign)
            {
                if ((*exp)->type != en_msil_array_access && exp1->type != en_msil_array_access)
                {
                    if (isarithmetic(*tp) || isarithmetic(tp1) ||
                        ((ispointer(*tp) && (!isarray(*tp) || !basetype(*tp)->msil)) ||
                         (ispointer(tp1) && (!isarray(tp1) || !basetype(tp1)->msil))))
                    {
                        int n = natural_size(*exp);
                        if (natural_size(exp1) != n)
                            cast((*tp), &exp1);
                    }
                    else if (isarray(*tp) && basetype(*tp)->msil)
                    {
                        *exp = exprNode(en_l_object, *exp, nullptr);
                        (*exp)->v.tp = *tp;
                    }
                }

                *exp = exprNode(op, *exp, exp1);
            }
            else
            {
                EXPRESSION* src, *dest;
                dest = EvaluateDest(*exp, *tp);
                if (dest)
                {
                    src = dest->left;
                    dest = dest->right;
                }
                else
                {
                    dest = RemoveAutoIncDec(*exp);
                    src = *exp;
                }
                // we want to optimize the as* operations for the backend
                // but can't do the optimization for divisions
                // otherwise it is fine for the processor we are on
                if (kw == asmod || kw == asdivide || basetype(*tp)->type == bt_bool)
                {
                    int n = natural_size(*exp);
                    destSize(*tp, tp1, &src, &exp1, false, nullptr);
                    *exp = exprNode(op, src, exp1);
                    if (natural_size(exp1) != n)
                        cast((*tp), exp);
                    *exp = exprNode(en_assign, dest, *exp);
                }
                else if (kw == asleftshift || kw == asrightshift)
                {
                    int n = natural_size(*exp);
                    if (natural_size(exp1) != n)
                        cast(&stdint, &exp1);
                    *exp = exprNode(op, src, exp1);
                    *exp = exprNode(en_assign, dest, *exp);
                }
                else
                {
                    int n = natural_size(*exp);
                    if (natural_size(exp1) != n)
                        destSize(*tp, tp1, &src, &exp1, false, nullptr);
                    *exp = exprNode(op, src, exp1);
                    if (natural_size(*exp) != n)
                        cast(*tp, exp);
                    *exp = exprNode(en_assign, dest, *exp);
                }
            }
        }
        /* now if there is a cast on the LHS move it to the RHS */
        EXPRESSION* expl = (*exp)->left;
        if (castvalue(expl))
        {
            auto exp2 = expl;
            auto exp3 = expl;
            while (castvalue(exp2))
            {
                /* cast on the lhs isn't sign-extended */
                switch (exp2->type)
                {
                    case en_x_c:
                        exp2->type = en_x_uc;
                        break;
                    case en_x_s:
                        exp2->type = en_x_us;
                        break;
                    case en_x_i:
                        exp2->type = en_x_ui;
                        break;
                    case en_x_l:
                        exp2->type = en_x_ul;
                        break;
                    case en_x_ll:
                        exp2->type = en_x_ull;
                        break;
                }
                if (castvalue(exp2->left))
                    exp3 = exp2->left;
                exp2 = exp2->left;
            }
            (*exp)->left = exp2;
            exp3->left = (*exp)->right;
            (*exp)->right = exp3;
        }
    }
    if ((*exp)->type == en_pc || ((*exp)->type == en_func && !(*exp)->v.func->ascall))
        thunkForImportTable(exp);
    return lex;
}
LEXLIST* expression_comma(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    lex = expression_assign(lex, funcsp, atp, tp, exp, ismutable, flags);
    if (*tp == nullptr)
        return lex;
    while (MATCHKW(lex, comma))
    {
        EXPRESSION* exp1 = nullptr;
        TYPE* tp1 = nullptr;
        lex = getsym();
        lex = expression_assign(lex, funcsp, atp, &tp1, &exp1, nullptr, flags);
        if (!tp1)
        {
            break;
        }
        if ((Optimizer::cparams.prm_cplusplus || (Optimizer::architecture == ARCHITECTURE_MSIL)) &&
            insertOperatorFunc(ovcl_comma, comma, funcsp, tp, exp, tp1, exp1, nullptr, flags))
        {
            continue;
        }
        else
        {
            *tp = tp1;
            *exp = exprNode(en_void, *exp, exp1);
            exp = &(*exp)->right;
        }
    }
    return lex;
}
LEXLIST* expression_no_comma(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, bool* ismutable, int flags)
{
    lex = expression_assign(lex, funcsp, atp, tp, exp, ismutable, flags);
    assignmentUsages(*exp, false);
    return lex;
}
LEXLIST* expression_no_check(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags)
{
    if (flags & _F_TYPETEST)
    {
        anonymousNotAlloc++;
        lex = expression_no_comma(lex, funcsp, atp, tp, exp, nullptr, flags);        
        anonymousNotAlloc--;
    }
    else
    {
        lex = expression_comma(lex, funcsp, atp, tp, exp, nullptr, flags);
    }
    return lex;
}

LEXLIST* expression(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags)
{
    lex = expression_comma(lex, funcsp, atp, tp, exp, nullptr, flags);
    assignmentUsages(*exp, false);
    return lex;
}
}  // namespace Parser