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
 *     contact information:
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

namespace Parser
{
int noinline;
int inlinesym_count;
EXPRESSION* inlinesym_thisptr[1000];
#undef MAX_INLINE_NESTING
#define MAX_INLINE_NESTING 3

static SYMBOL* inlinesym_list[1000];
static int inline_nesting;
void iinlineInit(void)
{
    inlinesym_count = 0;
    inline_nesting = 0;
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
            return inlinesym_thisptr[inlinesym_count - 1];
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
static void inlineBindThis(SYMBOL* funcsp, SymbolTable<SYMBOL>* table, EXPRESSION* thisptr)
{
    if (table->size())
    {
        SYMBOL* sym = table->front();
        inlinesym_thisptr[inlinesym_count] = 0;
        if (sym->sb->thisPtr)
        {
            if (thisptr)
            {
                if(Optimizer::architecture == ARCHITECTURE_MSIL)
                {
                    Optimizer::SimpleSymbol* simpleSym;
                    Optimizer::IMODE *src, *ap1, *idest;
                    EXPRESSION* dest;
                    thisptr = inlinesym_count == 0 || inlinesym_thisptr[inlinesym_count - 1] == nullptr || !hasRelativeThis(thisptr)
                              ? thisptr
                              : inlineGetThisPtr(thisptr);
                    simpleSym = Optimizer::SymbolManager::Get(sym = makeID(sc_auto, sym->tp, nullptr, AnonymousName()));
                    simpleSym->allocate = true;
                    simpleSym->anonymous = true;
                    simpleSym->inAllocTable = true;
                    Optimizer::temporarySymbols.push_back(simpleSym);
                    dest = varNode(en_auto, sym);
                    deref(sym->tp, &dest);
                    inlinesym_thisptr[inlinesym_count] = dest;
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
                    thisptr = inlinesym_count == 0 || inlinesym_thisptr[inlinesym_count - 1] == nullptr || !hasRelativeThis(thisptr)
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
                    inlinesym_thisptr[inlinesym_count] = dest;
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
        else if (inlinesym_count)
        {
            inlinesym_thisptr[inlinesym_count] = inlinesym_thisptr[inlinesym_count - 1];
        }
    }
}
static void inlineBindArgs(SYMBOL* funcsp, SymbolTable<SYMBOL>* table, INITLIST* args)
{
    if (table->size())
    {
        EXPRESSION** list;
        int cnt = table->size();
        auto it = table->begin();
        SYMBOL* sym = *it;
        if (sym->sb->thisPtr)
        {
            ++it;
            cnt--;
        }
        
        list = Allocate<EXPRESSION*>(cnt);
        cnt = 0;
        while (it != table->end() && args)  // args might go to nullptr for a destructor, which currently has a VOID at the end of the arg list
        {
            SYMBOL* sym = *it;
            if (!isvoid(sym->tp))
            {
                Optimizer::IMODE *src, *ap1, *idest;
                EXPRESSION* dest;
                int n = sizeFromType(sym->tp);
                int m = natural_size(args->exp);
                if (sym->sb->addressTaken || n == ISZ_ULONGLONG || n == -ISZ_ULONGLONG || m == ISZ_ULONGLONG || m == -ISZ_ULONGLONG || Optimizer::architecture == ARCHITECTURE_MSIL)
                {
                    SYMBOL* sym2;
                    sym2 = makeID(sc_auto, sym->tp, nullptr, AnonymousName());
                    SetLinkerNames(sym2, lk_cdecl);

                    Optimizer::SimpleSymbol* simpleSym = Optimizer::SymbolManager::Get(sym2);
                    Optimizer::temporarySymbols.push_back(simpleSym);
                    simpleSym->allocate = true;
                    simpleSym->inAllocTable = true;
                    dest = varNode(en_auto, sym2);
                    if (isarray(sym->tp))
                    {
                        dest = exprNode(en_l_p, dest, nullptr);
                    }
                    else
                    {
                        deref(sym->tp, &dest);
                    }

                    list[cnt++] = dest;
                    idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                    src = gen_expr(funcsp, args->exp, 0, natural_size(args->exp));
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
                    dest = exprNode(en_paramsubstitute, nullptr, nullptr);
                    if (isarray(sym->tp))
                    {
                        dest = exprNode(en_l_p, dest, nullptr);
                    }
                    else if (sym->tp->type == bt_templateselector)
                    {
                        deref(&stdpointer, &dest);
                    }
                    else
                    {
                        deref(sym->tp, &dest);
                    }
                    list[cnt++] = dest;
                    src = gen_expr(funcsp, args->exp, 0, natural_size(args->exp));
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
            args = args->next;
        }
        // we have to fill in the args last in case the same constructor was used
        // in multiple arguments...
        // also deals with things like the << and >> operators, where an expression can have arguments chained into the same
          // operator over and over...
        cnt = 0;
        for (auto sym : *table)
        {
            if (!isvoid(sym->tp))
            {
                Optimizer::SymbolManager::Get(sym)->paramSubstitute = list[cnt++];
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
static void inlineResetTable(SymbolTable<SYMBOL>* table)
{
    for (auto sp : * table)
    {
        Optimizer::SimpleSymbol* sym = Optimizer::SymbolManager::Get((SYMBOL*)sp);
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
}
// this is overkill since stmt.c disallows inlines with variables other than
// at the opening of the first block.
static void inlineResetVars(SymbolTable<SYMBOL>* syms, SymbolTable<SYMBOL>* params)
{
    SymbolTable<SYMBOL>* old = syms;
    inlineResetTable(params);
    while (syms)
    {

        inlineResetTable(syms);
        syms = syms->Next();
    }
    while (old)
    {

        inlineResetTable(old);
        old = old->Chain();
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
                if (!simpleSym->inAllocTable)
                {
                    Optimizer::temporarySymbols.push_back(simpleSym);
                    simpleSym->inAllocTable = true;
                }
            }
        }
        src = src->Next();
    }
}
static bool inlineTooComplex(FUNCTIONCALL* f) { return f->sp->sb->endLine - f->sp->sb->startLine > 15 / (inline_nesting * 2 + 1); }
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
    EXPRESSION* oldthis = inlinesym_thisptr[inlinesym_count];
    //    return nullptr;
    if (noinline)
        return nullptr;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOINLINE)
        return nullptr;

    if (Optimizer::cparams.prm_debug)
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
    if (inlinesym_count >= MAX_INLINE_NESTING && !f->sp->sb->simpleFunc)
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
    if (f->returnEXP && !isref(basetype(f->sp->tp)->btp))
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    // if it has a structured return value or structured arguments we don't try to inline it
    if (isstructured(basetype(f->sp->tp)->btp))
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    if (basetype(basetype(f->sp->tp)->btp)->type == bt_memberptr)  // DAL FIXED
    {
        f->sp->sb->dumpInlineToFile = true;
        return nullptr;
    }
    for (auto sp : *basetype(f->sp->tp)->syms)
    {
        if (isstructured(sp->tp) || basetype(sp->tp)->type == bt_memberptr)
        {
            f->sp->sb->dumpInlineToFile = true;
            return nullptr;
        }
    }
    for (i = 0; i < inlinesym_count; i++)
        if (f->sp == inlinesym_list[i])
        {
            f->sp->sb->dumpInlineToFile = true;
            return nullptr;
        }
    // this is here because cmdswitch uses a unique_ptr and autoincrement of a structure member together, and the resulting code gen
    // fails
    INITLIST* fargs = f->arguments;
    while (fargs)
    {
        if (hasaincdec(fargs->exp))
        {
            f->sp->sb->dumpInlineToFile = true;
            return nullptr;
        }
        fargs = fargs->next;
    }
    inline_nesting++;
    codeLabelOffset = Optimizer::nextLabel - INT_MIN;
    Optimizer::nextLabel += f->sp->sb->labelCount + 10;
    retcount = 0;
    returnImode = nullptr;
    startlab = Optimizer::nextLabel++;
    retlab = Optimizer::nextLabel++;
    AllocateLocalContext(nullptr, funcsp, Optimizer::nextLabel++);
    inlineBindThis(funcsp, basetype(f->sp->tp)->syms, f->thisptr);
    inlineBindArgs(funcsp, basetype(f->sp->tp)->syms, f->arguments);
    inlinesym_list[inlinesym_count++] = f->sp;
    inlineResetVars(f->sp->sb->inlineFunc.syms, basetype(f->sp->tp)->syms);
    inlineCopySyms(f->sp->sb->inlineFunc.syms);
    genstmt(f->sp->sb->inlineFunc.stmt->lower, f->sp);
    if (f->sp->sb->inlineFunc.stmt->blockTail)
    {
        Optimizer::gen_icode(Optimizer::i_functailstart, 0, 0, 0);
        genstmt(f->sp->sb->inlineFunc.stmt->blockTail, funcsp);
        Optimizer::gen_icode(Optimizer::i_functailend, 0, 0, 0);
    }
    genreturn(0, f->sp, 1, 0, nullptr);
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
    FreeLocalContext(nullptr, funcsp, Optimizer::nextLabel++);
    returnImode = oldReturnImode;
    retlab = oldretlab;
    startlab = oldstartlab;
    retcount = oldretcount;
    codeLabelOffset = oldOffset;
    inlinesym_count--;
    inlinesym_thisptr[inlinesym_count] = oldthis;
    inline_nesting--;
    return ap3;
}
}  // namespace Parser