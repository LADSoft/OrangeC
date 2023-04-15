/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
 *     contact information:i
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <stdio.h>
#include <stdlib.h>
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
#include "help.h"
#include "OptUtils.h"
#include "iblock.h"
#include "memory.h"
#include "initbackend.h"
#include "template.h"
#include "symtab.h"
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
        if (thisPtr->type == en_auto && thisPtr->v.sp->sb->thisPtr)
            rv = true;
    }
    return rv;
}
static EXPRESSION* inlineGetThisPtr(EXPRESSION* exp)
{
    if (exp)
    {

        if (lvalue(exp) && exp->left->type == en_auto && exp->left->v.sp->sb->thisPtr)
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
static EXPRESSION* inlineBindThis(SYMBOL* funcsp, SymbolTable<SYMBOL>* table, EXPRESSION* thisptr)
{
    EXPRESSION *rv = nullptr;
    if (table->size())
    {
        SYMBOL* sym = table->front();
        if (sym->sb->thisPtr)
        {
            if (thisptr)
            {
                if(Optimizer::architecture == ARCHITECTURE_MSIL)
                {
                    Optimizer::SimpleSymbol* simpleSym;
                    Optimizer::IMODE *src, *ap1, *idest;
                    EXPRESSION* dest;
                    thisptr = inlineSymThisPtr.size() == 0 || inlineSymThisPtr.back() == nullptr || !hasRelativeThis(thisptr)
                              ? thisptr
                              : inlineGetThisPtr(thisptr);
                    simpleSym = Optimizer::SymbolManager::Get(sym = makeID(sc_auto, sym->tp, nullptr, AnonymousName()));
                    simpleSym->allocate = true;
                    simpleSym->anonymous = true;
                    simpleSym->inAllocTable = true;
                    Optimizer::temporarySymbols.push_back(simpleSym);
                    dest = varNode(en_auto, sym);
                    deref(sym->tp, &dest);
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
                    dest = exprNode(en_paramsubstitute, nullptr, nullptr);
                    deref(&stdpointer, &dest);
                    src = gen_expr(funcsp, thisptr, 0, natural_size(thisptr));
                    ap1 = Optimizer::LookupLoadTemp(nullptr, src);
                    if (ap1 != src)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                        src = ap1;
                    }
                    rv = dest;
                    if (src->mode != Optimizer::i_direct || src->offset->type != Optimizer::se_tempref || src->size != ISZ_ADDR || src->offset->sp->loadTemp)
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
                    }
                    dest->left->v.imode = Optimizer::tempreg(ISZ_ADDR, 0);
                    idest = dest->left->v.imode;
                    gen_icode(Optimizer::i_assn, idest, src, nullptr);
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
static void ArgDeref(TYPE* desttp, TYPE* srctp, EXPRESSION **dest)
{
    if (isarray(desttp))
    {
        *dest = exprNode(en_l_p, *dest, nullptr);
    }
    else if (desttp->type == bt_templateselector)
    {
        deref(&stdpointer, dest);
    }
    else if (desttp->type == bt_ellipse)
    {
        deref(srctp, dest);
    }
    else if (isstructured(desttp))
    {
        deref(basetype(desttp)->sp->sb->structuredAliasType, dest);
    }
    else
    {
        deref(desttp, dest);
    }
}
static void inlineBindArgs(SYMBOL* funcsp, SymbolTable<SYMBOL>* table, std::list<INITLIST*>* args)
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
        
        std::list<INITLIST*>::iterator ita, itae;
        if (args)
        {
            ita = args->begin();
            itae = args->end();
        }
        for ( ; it != table->end() && ita != itae; ++it, ++ita )  // args might go to nullptr for a destructor, which currently has a VOID at the end of the arg list
        {
            SYMBOL* sym = *it;
            if (!isvoid(sym->tp))
            {
                Optimizer::IMODE *src, *ap1, *idest;
                EXPRESSION* dest;
                int m = natural_size((*ita)->exp);
                int n;
                if (sym->tp->type == bt_ellipse)
                    n = m;
                else
                    n = sizeFromType(isstructured(sym->tp) ? basetype(sym->tp)->sp->sb->structuredAliasType : sym->tp);
                TYPE *tpr = nullptr;
                if (isref(sym->tp))
                    tpr = basetype(sym->tp)->btp;
                if ((!tpr && sym->sb->addressTaken) || Optimizer::sizeFromISZ(m) > Optimizer::chosenAssembler->arch->word_size ||
                    Optimizer::sizeFromISZ(n) > Optimizer::chosenAssembler->arch->word_size || Optimizer::architecture == ARCHITECTURE_MSIL)
                {
                    SYMBOL* sym2;
                    sym2 = makeID(sc_auto, sym->tp, nullptr, AnonymousName());
                    SetLinkerNames(sym2, lk_cdecl);

                    Optimizer::SimpleSymbol* simpleSym = Optimizer::SymbolManager::Get(sym2);
                    Optimizer::temporarySymbols.push_back(simpleSym);
                    simpleSym->allocate = true;
                    simpleSym->inAllocTable = true;
                    dest = varNode(en_auto, sym2);
                    ArgDeref(sym->tp, (*ita)->tp, &dest);

                    argList.push_back(dest);
                    idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                    src = gen_expr(funcsp, (*ita)->exp, 0, natural_size((*ita)->exp));
                    ap1 = Optimizer::LookupLoadTemp(nullptr, src);
                    if (ap1 != src)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                        src = ap1;
                    }
                    Optimizer::gen_icode(Optimizer::i_assn, idest, src, nullptr);
                }
                else if (tpr && (!isstructured(tpr) || basetype(tpr)->sp->sb->structuredAliasType) && tpr->size <= Optimizer::chosenAssembler->arch->word_size && ((*ita)->exp->type == en_auto || (*ita)->exp->type == en_global || (*ita)->exp->type == en_pc))
                {
                    // can pass reference by value...
                    dest = exprNode(en_paramsubstitute, (*ita)->exp, nullptr);
                    ArgDeref(sym->tp, (*ita)->tp, &dest);

                    argList.push_back(dest);
                    auto val = (*ita)->exp;
                    deref(isstructured(tpr) ? basetype(tpr)->sp->sb->structuredAliasType : tpr, &val);
                    src = gen_expr(funcsp, val, F_STORE, natural_size(val));
                    dest->left->v.imode = src;
                }
                else
                {
                    dest = exprNode(en_paramsubstitute, nullptr, nullptr);
                    ArgDeref(sym->tp, (*ita)->tp, &dest);
                    argList.push_back(dest);
                    auto val = (*ita)->exp;
                    if (val->type == en_stackblock)
                    {
                        val = val->left;
                        if (val->type != en_func && val->type != en_thisref)
                        {
                            deref(basetype(sym->tp)->sp->sb->structuredAliasType, &val);
                        }
                    }
                    src = gen_expr(funcsp, val, 0, natural_size(val));
                    ap1 = Optimizer::LookupLoadTemp(nullptr, src);
                    if (ap1 != src)
                    {
                        Optimizer::gen_icode(Optimizer::i_assn, ap1, src, nullptr);
                        src = ap1;
                    }
                    if (src->mode != Optimizer::i_direct || src->offset->type != Optimizer::se_tempref || src->size != n || src->offset->sp->loadTemp)
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
                        dest->left->v.imode = src;
                    }
                    else
                    {
                        dest->left->v.imode = Optimizer::tempreg(n, false);
                        idest = dest->left->v.imode;
                        Optimizer::gen_icode(Optimizer::i_assn, idest, src, nullptr);
                        if (sym->sb->assigned || sym->sb->altered)
                            idest->offset->sp->pushedtotemp = true;
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
            if (!sym->sb->thisPtr && !isvoid(sym->tp))
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
        for (auto sym : * src)
        {
            if (!sym->sb->thisPtr && !sym->sb->anonymous && sym->sb->storage_class != sc_parameter &&
                sym->sb->storage_class != sc_localstatic)
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
static bool inlineTooComplex(FUNCTIONCALL* f) { return f->sp->sb->endLine - f->sp->sb->startLine > 15 / (inlineNesting * 2 + 1); }
static bool hasaincdec(EXPRESSION* exp)
{
    if (exp)
    {
        if (exp->type == en_autoinc || exp->type == en_autodec)
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
    FUNCTIONCALL* f = node->v.func;
    Optimizer::IMODE* oldReturnImode = returnImode;
    int oldretlab = retlab, oldstartlab = startlab;
    int oldretcount = retcount;
    int oldOffset = codeLabelOffset;
//        return nullptr;
    if (noinline)
        return nullptr;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOINLINE)
        return nullptr;

    if (Optimizer::cparams.prm_debug)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (inlineSymList.find(f->sp) != inlineSymList.end())
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    /* measure of complexity */
    if (!f->sp->sb->simpleFunc && (inlineTooComplex(f) || (funcsp->sb->endLine - funcsp->sb->startLine > 400)))
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->fcall->type != en_pc)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->sb->storage_class == sc_virtual)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp == theCurrentFunc)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->sb->canThrow)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->sb->xc && f->sp->sb->xc->xclab)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->sb->allocaUsed)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->sb->templateLevel && f->sp->templateParams && !f->sp->sb->instantiated)  // specialized)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (!f->sp->sb->inlineFunc.syms)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (!f->sp->sb->inlineFunc.stmt)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->sb->dontinstantiate)
    {
        return nullptr;
    }
    if (inlineSymThisPtr.size() >= MAX_INLINE_NESTING && !f->sp->sb->simpleFunc)
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->thisptr)
    {
        if (f->thisptr->type == en_auto && f->thisptr->v.sp->sb->stackblock)
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
                f->sp->sb->dumpInlineToFile = true;
                return nullptr;
            }
        }
        auto ex = f->thisptr;
        if (lvalue(ex))
            ex = ex->left;
        if (ex->type == en_void)
        {
            f->sp->sb->dumpInlineToFile = true;
            return nullptr;
        }
    }
    if (basetype(basetype(f->sp->tp)->btp)->type == bt_memberptr)  // DAL FIXED
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    for (auto sp : *basetype(f->sp->tp)->syms)
    {
        if ((isstructured(sp->tp) && !basetype(sp->tp)->sp->sb->structuredAliasType) || basetype(sp->tp)->type == bt_memberptr)
        {
            f->sp->sb->dumpInlineToFile = true;
            return nullptr;
        }
    }
    // this is here because cmdswitch uses a unique_ptr and autoincrement of a structure member together, and the resulting code gen
    // fails
    if (f->arguments)
    {
        for (auto fargs : *f->arguments)
        {
            if (hasaincdec(fargs->exp))
            {
                f->sp->sb->dumpInlineToFile = true;
                return nullptr;
            }
        }
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
    AllocateLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);
    bool found = false;
    if (isstructured(basetype(f->sp->tp)->btp))
    {
        EXPRESSION* exp = f->returnEXP;
        if (inlineSymStructPtr.size() && lvalue(exp) && exp->left->type == en_auto && exp->left->v.sp->sb->retblk)
        {
             exp = inlineSymStructPtr.back();
        }
        inlineSymStructPtr.push_back(exp);
        found = true;
    }
    auto thisptr = inlineBindThis(funcsp, basetype(f->sp->tp)->syms, f->thisptr);
    inlineBindArgs(funcsp, basetype(f->sp->tp)->syms, f->arguments);
    inlineSymList.insert(f->sp);
    inlineSymThisPtr.push_back(thisptr);
    inlineCopySyms(f->sp->sb->inlineFunc.syms);
    genstmt(f->sp->sb->inlineFunc.stmt->front()->lower, f->sp);
    if (f->sp->sb->inlineFunc.stmt->front()->blockTail)
    {
        Optimizer::gen_icode(Optimizer::i_functailstart, 0, 0, 0);
        genstmt(f->sp->sb->inlineFunc.stmt->front()->blockTail, funcsp);
        Optimizer::gen_icode(Optimizer::i_functailend, 0, 0, 0);
    }
    genreturn(0, f->sp, F_NEEDEPILOG, nullptr);
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
        auto ap4 = LookupLoadTemp(nullptr,ap3);
        gen_icode(Optimizer::i_assn, ap4, ap3, nullptr);
        ap3 = ap4;
    }
    inlineUnbindArgs(basetype(f->sp->tp)->syms);
    if (found)
    {
        inlineSymStructPtr.pop_back();
    }
    FreeLocalContext(emptyBlockdata, funcsp, Optimizer::nextLabel++);
    returnImode = oldReturnImode;
    retlab = oldretlab;
    startlab = oldstartlab;
    retcount = oldretcount;
    codeLabelOffset = oldOffset;
    inlineSymThisPtr.pop_back();
    inlineNesting--;
    inlineSymList.erase(inlineSymList.find(f->sp));
    return ap3;
}
}  // namespace Parser