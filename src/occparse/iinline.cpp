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

#include <cstdio>
#include <cstdlib>
#include "compiler.h"
#include "ccerr.h"
#include "assert.h"
#include "config.h"
#include "istmt.h"
#include "stmt.h"
#include "ildata.h"
#include "expr.h"
#include "mangle.h"
#include "iexpr.h"
#include "declare.h"
#include "lex.h"
#include "help.h"
#include "OptUtils.h"
#include "iblock.h"
#include "memory.h"
#include "initbackend.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "namespace.h"
#include "symtab.h"
#include "types.h"
#include "constopt.h"
#include <unordered_set>

namespace Parser
{
int noinline;
std::vector<EXPRESSION*> inlineSymStructPtr;
std::vector<EXPRESSION*> inlineSymThisPtr;
#undef MAX_INLINE_NESTING
#define MAX_INLINE_NESTING 3

static std::unordered_set<SYMBOL*> inlineSymList;
static int inlineNesting;
static std::unordered_set<Optimizer::SimpleSymbol*> argTable;
static Optimizer::SimpleSymbol* argCurrentFunction;

void iinlineInit(void)
{
    inlineNesting = 0;
    argCurrentFunction = nullptr;
    inlineSymList.clear();
    inlineSymStructPtr.clear();
    inlineSymThisPtr.clear();
    argTable.clear();
}
static bool hasRelativeThis(EXPRESSION* thisPtr)
{
    bool rv = false;
    if (thisPtr->left)
    {
        rv = hasRelativeThis(thisPtr->left);
    }
    if (!rv && thisPtr->right)
    {
        rv = hasRelativeThis(thisPtr->right);
    }
    if (!rv)
    {
        if (thisPtr->type == ExpressionNode::auto_ && thisPtr->v.sp->sb->thisPtr)
            rv = true;
    }
    return rv;
}
static EXPRESSION* inlineGetThisPtr(EXPRESSION* exp)
{
    if (exp)
    {

        if (IsLValue(exp) && exp->left->type == ExpressionNode::auto_ && exp->left->v.sp->sb->thisPtr)
        {
            return inlineSymThisPtr.back();
        }
        else
        {
            EXPRESSION* rv = Allocate<EXPRESSION>();
            *rv = *exp;
            rv->left = inlineGetThisPtr(rv->left);
            rv->right = inlineGetThisPtr(rv->right);
            return rv;
        }
    }
    return nullptr;
}
static EXPRESSION* inlineBindThis(SYMBOL* funcsp, SYMBOL* func, int flags, SymbolTable<SYMBOL>* table, EXPRESSION* thisptr)
{
    EXPRESSION* rv = nullptr;
    if (table->size())
    {
        SYMBOL* sym = table->front();
        if (sym->sb->thisPtr)
        {
            if (thisptr)
            {
                if (Optimizer::architecture == ARCHITECTURE_MSIL)
                {
                    Optimizer::SimpleSymbol* simpleSym;
                    Optimizer::IMODE *src, *ap1, *idest;
                    EXPRESSION* dest;
                    thisptr = inlineSymThisPtr.size() == 0 || inlineSymThisPtr.back() == nullptr || !hasRelativeThis(thisptr)
                                  ? thisptr
                                  : inlineGetThisPtr(thisptr);
                    simpleSym = Optimizer::SymbolManager::Get(sym = makeID(StorageClass::auto_, sym->tp, nullptr, AnonymousName()));
                    simpleSym->allocate = true;
                    simpleSym->anonymous = true;
                    simpleSym->inAllocTable = true;
                    Optimizer::temporarySymbols.push_back(simpleSym);
                    dest = MakeExpression(ExpressionNode::auto_, sym);
                    Dereference(sym->tp, &dest);
                    rv = dest;
                    idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                    src = gen_expr(funcsp, thisptr, 0, natural_size(thisptr));
                    ap1 = Optimizer::LookupLoadTemp(nullptr, src);
                    if (ap1 != src)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                        src = ap1;
                    }
                    Optimizer::gen_icode(Optimizer::i_assn, idest, src, nullptr);
                }
                else
                {
                    Optimizer::SimpleSymbol* simpleSym;
                    Optimizer::IMODE *src, *ap1, *idest;
                    EXPRESSION* dest;
                    thisptr = inlineSymThisPtr.size() == 0 || inlineSymThisPtr.back() == nullptr || !hasRelativeThis(thisptr)
                                  ? thisptr
                                  : inlineGetThisPtr(thisptr);
                    Type* tr = nullptr;

                    // special case the -> operator, as it always must return a pointer
                    if (func->name[0] != '.' || func->name[2] != 'a' || strcmp(func->name, overloadNameTab[CI_POINTSTO]) != 0)
                        if (!(flags & F_RETURNSTRUCTBYADDRESS) &&
                            (func->sb->isConstructor || func->sb->isDestructor ||
                             (!func->tp->BaseType()->btp->IsStructured() && !sym->tp->CompatibleType(func->tp->BaseType()->btp) &&
                              !SameTemplatePointedTo(sym->tp, func->tp->BaseType()->btp))))
                            tr = sym->tp->BaseType()->btp->BaseType();
                    if (tr && tr->sp->sb->structuredAliasType &&
                        (thisptr->type != ExpressionNode::l_p_ || thisptr->left->type != ExpressionNode::auto_ ||
                         !thisptr->left->v.sp->sb->thisPtr ||
                         thisptr->left->v.sp->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType) &&
                        !expressionHasSideEffects(thisptr))
                    {
                        auto val = thisptr;
                        Dereference(tr->sp->sb->structuredAliasType, &val);
                        src = gen_expr(funcsp, val, F_STORE, natural_size(val));
                        dest = makeParamSubs(thisptr, src);
                        Dereference(&stdpointer, &dest);
                    }
                    else
                    {
                        src = gen_expr(funcsp, thisptr, F_RETURNSTRUCTBYADDRESS, natural_size(thisptr));
                        ap1 = Optimizer::LookupLoadTemp(nullptr, src);
                        if (ap1 != src)
                        {
                            Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                            src = ap1;
                        }
                        if (src->mode != Optimizer::i_direct || src->offset->type != Optimizer::se_tempref ||
                            src->size != ISZ_ADDR || src->offset->sp->loadTemp)
                        {
                            if (src->size != ISZ_ADDR)
                            {
                                ap1 = Optimizer::tempreg(src->size, false);
                                Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                                src = ap1;
                            }
                            ap1 = Optimizer::tempreg(ISZ_ADDR, false);
                            Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                            src = ap1;
                            src->offset->sp->pushedtotemp = true;
                            src->offset->sp->pushedtotemp = true;
                        }
                        dest = makeParamSubs(nullptr, Optimizer::tempreg(ISZ_ADDR, 0));
                        Dereference(&stdpointer, &dest);
                        idest = dest->left->v.imode;
                        gen_icode(Optimizer::i_assn, idest, src, nullptr);
                    }
                    rv = dest;
                }
            }
        }
        else if (inlineSymThisPtr.size())
        {
            rv = inlineSymThisPtr.back();
        }
    }
    return rv;
}
static void ArgDeref(Type* desttp, Type* srctp, EXPRESSION** dest)
{
    if (desttp->IsArray())
    {
        *dest = MakeExpression(ExpressionNode::l_p_, *dest);
    }
    else if (desttp->type == BasicType::templateselector_)
    {
        Dereference(&stdpointer, dest);
    }
    else if (desttp->type == BasicType::ellipse_)
    {
        Dereference(srctp, dest);
    }
    else if (desttp->IsStructured())
    {
        Dereference(desttp->BaseType()->sp->sb->structuredAliasType, dest);
    }
    else
    {
        Dereference(desttp, dest);
    }
}
static void inlineBindArgs(SYMBOL* funcsp, SymbolTable<SYMBOL>* table, std::list<Argument*>* args)
{
    if (table->size())
    {
        std::list<EXPRESSION*> argList;
        auto it = table->begin();
        SYMBOL* sym = *it;
        if (sym->sb->thisPtr)
        {
            ++it;
        }

        std::list<Argument*>::iterator ita, itae;
        if (args)
        {
            ita = args->begin();
            itae = args->end();
        }
        for (; it != table->end() && ita != itae;
             ++it,
             ++ita)  // args might go to nullptr for a destructor, which currently has a VOID at the Keyword::end_ of the arg list
        {
            SYMBOL* sym = *it;
            if (!sym->tp->IsVoid())
            {
                Optimizer::IMODE *src, *ap1, *idest;
                EXPRESSION* dest;
                int m = natural_size((*ita)->exp);
                int n;
                if (sym->tp->type == BasicType::ellipse_)
                    n = m;
                else
                    n = SizeFromType(sym->tp->IsStructured() ? sym->tp->BaseType()->sp->sb->structuredAliasType : sym->tp);
                Type* tpr = nullptr;
                if (sym->tp->IsRef())
                    tpr = sym->tp->BaseType()->btp;
                if (tpr && (tpr->IsStructured() && tpr->BaseType()->sp->sb->structuredAliasType) &&
                    tpr->size <= Optimizer::chosenAssembler->arch->word_size && Optimizer::architecture != ARCHITECTURE_MSIL &&
                    !expressionHasSideEffects((*ita)->exp))
                {
                    // can pass reference by value...
                    auto val = (*ita)->exp;
                    auto addr = val;

                    auto ext = val;
                    if (ext->type == ExpressionNode::thisref_)
                        ext = ext->left;
                    if (ext->type != ExpressionNode::callsite_ || ext->v.func->sp->tp->BaseType()->btp->IsRef())
                    {
                        Dereference(tpr->BaseType()->sp->sb->structuredAliasType, &val);
                    }
                    if (sym->sb->addressTaken &&
                        (ext->type == ExpressionNode::callsite_ && !ext->v.func->sp->tp->BaseType()->btp->IsRef()))
                    {
                        src = nullptr;
                        addr = tempVar(&stdpointer);
                        val = MakeExpression(ExpressionNode::assign_, addr, val);
                        gen_expr(funcsp, val, F_STORE, natural_size(val));
                    }
                    else
                    {
                        src = gen_expr(funcsp, val, F_STORE | F_RETURNSTRUCTBYVALUE, natural_size(val));
                    }
                    dest = makeParamSubs(addr, src);
                    ArgDeref(sym->tp, (*ita)->tp, &dest);
                    argList.push_back(dest);
                }
                else
                {
                    auto val = (*ita)->exp;
                    if (val->type == ExpressionNode::structadd_ && isconstzero(&stdint, val->right))
                    {
                        val = val->left;
                    }
                    bool createTemp = sym->sb->addressTaken ||
                                      Optimizer::sizeFromISZ(m) > Optimizer::chosenAssembler->arch->word_size ||
                                      Optimizer::sizeFromISZ(n) > Optimizer::chosenAssembler->arch->word_size ||
                                      Optimizer::architecture == ARCHITECTURE_MSIL;
                    bool createTempByRef = false;
                    if (!createTemp)
                    {
                        if (val->type == ExpressionNode::stackblock_)
                        {
                            val = val->left;
                            if (val->type != ExpressionNode::callsite_ && val->type != ExpressionNode::thisref_)
                            {
                                Dereference(sym->tp->BaseType()->sp->sb->structuredAliasType, &val);
                            }
                        }
                        else if (val->type == ExpressionNode::comma_)
                        {
                            auto val1 = &val;
                            while ((*val1)->type == ExpressionNode::comma_ && (*val1)->right)
                                val1 = &(*val1)->right;
                            if ((*val1)->isStructAddress)
                                Dereference(sym->tp->BaseType()->sp->sb->structuredAliasType, val1);
                        }
                    }
                    src = gen_expr(funcsp, val, F_RETURNSTRUCTBYADDRESS, natural_size(val));

                    if (src->wasinlined && !createTemp)
                    {
                        if (val->type == ExpressionNode::l_ref_ && val->left->type == ExpressionNode::auto_ &&
                            val->left->v.sp->sb->storage_class == StorageClass::parameter_)
                        {
                            if (val->left->v.sp->tp->IsRef() && val->left->v.sp->tp->BaseType()->btp->IsStructured())
                                if (val->left->v.sp->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType)
                                {
                                    createTemp = true;
                                    createTempByRef = true;
                                }
                        }
                    }
                    if (createTemp)
                    {
                        SYMBOL* sym2;
                        sym2 = makeID(StorageClass::auto_, sym->tp, nullptr, AnonymousName());
                        SetLinkerNames(sym2, Linkage::cdecl_);

                        Optimizer::SimpleSymbol* simpleSym = Optimizer::SymbolManager::Get(sym2);
                        Optimizer::temporarySymbols.push_back(simpleSym);
                        simpleSym->allocate = true;
                        simpleSym->inAllocTable = true;
                        dest = MakeExpression(ExpressionNode::auto_, sym2);
                        ArgDeref(sym->tp, (*ita)->tp, &dest);

                        argList.push_back(createTempByRef ? dest->left : dest);
                        idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                        ap1 = Optimizer::LookupLoadTemp(nullptr, src);
                        if (ap1 != src)
                        {
                            Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                            src = ap1;
                        }
                        Optimizer::gen_icode(Optimizer::i_assn, idest, src, nullptr);
                    }
                    else
                    {
                        ap1 = Optimizer::LookupLoadTemp(nullptr, src);
                        if (ap1 != src)
                        {
                            Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                            src = ap1;
                        }
                        if (src->mode != Optimizer::i_direct || src->offset->type != Optimizer::se_tempref || src->size != n ||
                            src->offset->sp->loadTemp)
                        {
                            ap1 = Optimizer::tempreg(src->size, false);
                            Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                            src = ap1;
                            if (src->size != n)
                            {
                                ap1 = Optimizer::tempreg(n, false);
                                Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                                src = ap1;
                            }
                        }
                        if (!src->retval && !sym->sb->assigned && !sym->sb->altered)
                        {
                            dest = makeParamSubs(nullptr, src);
                        }
                        else
                        {
                            dest = makeParamSubs(nullptr, Optimizer::tempreg(n, false));
                            idest = dest->v.imode;
                            Optimizer::gen_icode(Optimizer::i_assn, idest, src, nullptr);
                            if (sym->sb->assigned || sym->sb->altered)
                                idest->offset->sp->pushedtotemp = true;
                        }
                        ArgDeref(sym->tp, (*ita)->tp, &dest);
                        argList.push_back(dest);
                    }
                }
            }
        }
        // we have to fill in the args last in case the same constructor was used
        // in multiple arguments...
        // also deals with things like the << and >> operators, where an expression can have arguments chained into the same
        // operator over and over...
        auto ital = argList.begin();
        for (auto sym : *table)
        {
            if (ital == argList.end())
                break;
            if (!sym->sb->thisPtr && !sym->tp->IsVoid())
            {
                Optimizer::SymbolManager::Get(sym)->paramSubstitute = *ital;
                ++ital;
            }
        }
    }
}
static void inlineUnbindArgs(SymbolTable<SYMBOL>* table)
{
    for (auto sym : *table)
    {
        sym->sb->inlineFunc.stmt = nullptr;
        Optimizer::SymbolManager::Get(sym)->paramSubstitute = nullptr;
    }
}
static void inlineResetSym(Optimizer::SimpleSymbol* sym)
{
    if (sym->storage_class != Optimizer::scc_localstatic)
    {
        sym->imvalue = nullptr;
        sym->imind = nullptr;
        sym->imaddress = nullptr;
        sym->imstore = nullptr;
        if (sym->anonymous)
            sym->allocate = false;
        sym->inAllocTable = false;
    }
}
static void inlineCopySyms(SymbolTable<SYMBOL>* src)
{
    while (src)
    {
        for (auto sym : *src)
        {
            if (!sym->sb->thisPtr && !sym->sb->anonymous && sym->sb->storage_class != StorageClass::parameter_ &&
                sym->sb->storage_class != StorageClass::localstatic_ && sym->sb->storage_class != StorageClass::constant_)
            {
                Optimizer::SimpleSymbol* simpleSym = Optimizer::SymbolManager::Get(sym);
                if (argTable.find(simpleSym) == argTable.end())
                {
                    inlineResetSym(simpleSym);
                    Optimizer::temporarySymbols.push_back(simpleSym);
                    argTable.insert(simpleSym);
                }
            }
        }
        src = src->Next();
    }
}
static bool inlineTooComplex(CallSite* f) { return f->sp->sb->endLine - f->sp->sb->startLine > 15 / (inlineNesting * 2 + 1); }
static bool hasaincdec(EXPRESSION* exp)
{
    if (exp)
    {
        if (exp->type == ExpressionNode::auto_inc_ || exp->type == ExpressionNode::auto_dec_)
            return true;
        if (hasaincdec(exp->left))
            return true;
        if (hasaincdec(exp->right))
            return true;
    }
    return false;
}
Optimizer::IMODE* gen_inline(SYMBOL* funcsp, EXPRESSION* node, int flags)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    int i;
    Optimizer::IMODE* ap3;
    CallSite* f = node->v.func;
    Optimizer::IMODE* oldReturnImode = returnImode;
    int oldretlab = retlab, oldstartlab = startlab;
    int oldretcount = retcount;
    int oldOffset = codeLabelOffset;
    //        return nullptr;
    if (noinline)
        return nullptr;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOINLINE)
        return nullptr;

    // don't put unnecessary destructor calls into the final output file
    if ((Optimizer::cparams.prm_debug || (Optimizer::cparams.prm_stackprotect & STACK_UNINIT_VARIABLE)) &&
        (!f->sp->sb->isDestructor || !f->sp->sb->parentClass->sb->pureDest))
    {
        return nullptr;
    }
    if (inlineSymList.find(f->sp) != inlineSymList.end())
    {
        return nullptr;
    }
    /* measure of complexity */
    if (!f->sp->sb->simpleFunc && (inlineTooComplex(f) || (funcsp->sb->endLine - funcsp->sb->startLine > 400)))
    {
        return nullptr;
    }
    if (f->fcall->type != ExpressionNode::pc_)
    {
        return nullptr;
    }
    if (f->sp->sb->storage_class == StorageClass::virtual_)
    {
        return nullptr;
    }
    if (f->sp == theCurrentFunc)
    {
        return nullptr;
    }
    if (f->sp->sb->canThrow)
    {
        return nullptr;
    }
    if (f->sp->sb->xc && f->sp->sb->xc->xclab)
    {
        return nullptr;
    }
    if (f->sp->sb->allocaUsed)
    {
        return nullptr;
    }
    if (f->sp->sb->templateLevel && f->sp->templateParams && !f->sp->sb->instantiated)  // specialized)
    {
        return nullptr;
    }
    if (!f->sp->sb->inlineFunc.syms)
    {
        return nullptr;
    }
    if (!f->sp->sb->inlineFunc.stmt)
    {
        return nullptr;
    }
    if (f->sp->sb->dontinstantiate)
    {
        return nullptr;
    }
    if (inlineSymThisPtr.size() >= MAX_INLINE_NESTING && !f->sp->sb->simpleFunc)
    {
        return nullptr;
    }
    if (f->thisptr)
    {
        if (f->thisptr->type == ExpressionNode::auto_ && f->thisptr->v.sp->sb->stackblock)
        {
            if (f->sp->sb->parentClass->sb->trivialCons)
            {
                Optimizer::IMODE* ap;
                Optimizer::gen_icode(Optimizer::i_parmstack, ap = Optimizer::tempreg(ISZ_ADDR, 0),
                                     Optimizer::make_immed(ISZ_UINT, f->thisptr->v.sp->tp->size), nullptr);
                Optimizer::intermed_tail->alwayslive = true;
                Optimizer::SymbolManager::Get(f->thisptr->v.sp)->imvalue = ap;
            }
            else
            {
                return nullptr;
            }
        }
        auto ex = f->thisptr;
        TakeAddress(&ex);
        if (ex->type == ExpressionNode::comma_)
        {
            return nullptr;
        }
    }
    if (f->sp->tp->BaseType()->btp->BaseType()->type == BasicType::memberptr_ || f->sp->tp->BaseType()->btp->IsBitInt())
    {
        return nullptr;
    }
    for (auto sp : *f->sp->tp->BaseType()->syms)
    {
        if ((sp->tp->IsStructured() && !sp->tp->BaseType()->sp->sb->structuredAliasType) ||
            sp->tp->BaseType()->type == BasicType::memberptr_)
        {
            return nullptr;
        }
        if (sp->tp->type == BasicType::ellipse_)
        {
            return nullptr;
        }
        if (sp->tp->IsBitInt())
        {
            return nullptr;
        }
    }
    // this is here because cmdswitch uses a unique_ptr and autoincrement of a structure member together, and the resulting code gen
    // fails
    if (f->arguments)
    {
        for (auto fargs : *f->arguments)
        {
            auto exp = fargs->exp;
            if (exp->type == ExpressionNode::comma_)
                exp = exp->left;
            if (hasaincdec(fargs->exp) || (exp->type == ExpressionNode::thisref_ && exp->left->v.func->sp->sb->isConstructor))
            {
                return nullptr;
            }
        }
    }
    if (f->returnEXP && f->returnEXP->type == ExpressionNode::auto_ && f->returnEXP->v.sp->sb->stackblock)
    {
        return nullptr;
    }
    if (currentFunction != argCurrentFunction)
    {
        argTable.clear();
        argCurrentFunction = currentFunction;
    }
    inlineNesting++;
    codeLabelOffset = Optimizer::nextLabel - INT_MIN;
    Optimizer::nextLabel += f->sp->sb->labelCount + 10;
    retcount = 0;
    returnImode = nullptr;
    startlab = Optimizer::nextLabel++;
    retlab = Optimizer::nextLabel++;
    StatementGenerator::AllocateLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);
    bool found = false;
    if (f->sp->tp->BaseType()->btp->IsStructured())
    {
        EXPRESSION* exp = f->returnEXP;
        if (exp && inlineSymStructPtr.size() && IsLValue(exp) && exp->left->type == ExpressionNode::auto_ &&
            exp->left->v.sp->sb->retblk)
        {
            exp = inlineSymStructPtr.back();
        }
        else
        {
            auto spr = f->sp->tp->BaseType()->btp->BaseType()->sp;
            if (spr->sb->structuredAliasType)
            {
                auto val = exp ? exp : tempVar(spr->sb->structuredAliasType);
                if (exp)
                {
                    Dereference(spr->sb->structuredAliasType, &val);
                }
                auto src = gen_expr(funcsp, val, F_STORE, natural_size(val));
                exp = makeParamSubs(val->left, src);
                Dereference(&stdpointer, &exp);
            }
        }
        inlineSymStructPtr.push_back(exp);
        found = true;
    }
    auto thisptr = inlineBindThis(funcsp, f->sp, flags, f->sp->tp->BaseType()->syms, f->thisptr);
    inlineBindArgs(funcsp, f->sp->tp->BaseType()->syms, f->arguments);
    inlineSymList.insert(f->sp);
    inlineSymThisPtr.push_back(thisptr);
    inlineCopySyms(f->sp->sb->inlineFunc.syms);
    genstmt(f->sp->sb->inlineFunc.stmt->front()->lower, f->sp,
            (flags & (F_RETURNREFBYVALUE | F_RETURNSTRUCTBYVALUE)) |
                ((flags & F_NOVALUE) && !f->sp->tp->BaseType()->btp->IsStructured() ? F_NORETURNVALUE : 0));
    if (f->sp->sb->inlineFunc.stmt->front()->blockTail)
    {
        Optimizer::gen_icode(Optimizer::i_functailstart, 0, 0, 0);
        genstmt(f->sp->sb->inlineFunc.stmt->front()->blockTail, funcsp, 0);
        Optimizer::gen_icode(Optimizer::i_functailend, 0, 0, 0);
    }
    genreturn(0, f->sp, F_NEEDEPILOG | (flags & F_RETURNREFBYVALUE), nullptr);
    ap3 = returnImode;
    if (!ap3)
    {
        ap3 = Optimizer::tempreg(ISZ_UINT, 0);
        // this is to make sure the return value gets carried into the optimization code
        // we might be doing a by-ref return value from some presumably unreachable function
        // that elided the return value
        gen_icode(Optimizer::i_assn, ap3, Optimizer::make_immed(-ISZ_UINT, 0), nullptr);
    }
    else
    {
        auto ap4 = LookupLoadTemp(nullptr, ap3);
        ap4->returnRefByVal = ap3->returnRefByVal;
        gen_icode(Optimizer::i_assn, ap4, ap3, nullptr);
        ap3 = ap4;
    }
    inlineUnbindArgs(f->sp->tp->BaseType()->syms);
    if (found)
    {
        inlineSymStructPtr.pop_back();
    }
    StatementGenerator::FreeLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);
    returnImode = oldReturnImode;
    retlab = oldretlab;
    startlab = oldstartlab;
    retcount = oldretcount;
    codeLabelOffset = oldOffset;
    inlineSymThisPtr.pop_back();
    inlineNesting--;
    inlineSymList.erase(inlineSymList.find(f->sp));
    ap3->wasinlined = true;
    return ap3;
}
}  // namespace Parser