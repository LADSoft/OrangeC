/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

/*
 * iexpr.c
 *
 * routies to take an enode list and generate icode
 */
#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "assert.h"

extern ARCH_ASM* chosenAssembler;
extern TYPE stdpointer;
extern IMODE* returnImode;
extern int startlab;
extern int retlab;
extern int nextLabel;
extern int codeLabelOffset;
extern int retcount;
extern LIST* temporarySymbols;
extern SYMBOL* theCurrentFunc;
int inlinesym_count;
EXPRESSION* inlinesym_thisptr[MAX_INLINE_NESTING];
static SYMBOL* inlinesym_list[MAX_INLINE_NESTING];
#undef MAX_INLINE_NESTING
#define MAX_INLINE_NESTING 3

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
        if (thisPtr->type == en_auto && thisPtr->v.sp->thisPtr)
            rv = true;
    }
    return rv;
}
static EXPRESSION* inlineGetThisPtr(EXPRESSION* exp)
{
    if (exp)
    {

        if (lvalue(exp) && exp->left->type == en_auto && exp->left->v.sp->thisPtr)
        {
            return inlinesym_thisptr[inlinesym_count - 1];
        }
        else
        {
            EXPRESSION* rv = (EXPRESSION*)Alloc(sizeof(EXPRESSION));
            *rv = *exp;
            rv->left = inlineGetThisPtr(rv->left);
            rv->right = inlineGetThisPtr(rv->right);
            return rv;
        }
    }
    return nullptr;
}
static void inlineBindThis(SYMBOL* funcsp, SYMLIST* hr, EXPRESSION* thisptr)
{
    if (hr)
    {
        SYMBOL* sym = hr->p;
        inlinesym_thisptr[inlinesym_count] = 0;
        if (sym->thisPtr)
        {
            if (thisptr)
            {
                IMODE *src, *ap1, *idest;
                EXPRESSION* dest;
                LIST* lst = (LIST*)Alloc(sizeof(LIST));
                thisptr = inlinesym_count == 0 || inlinesym_thisptr[inlinesym_count - 1] == nullptr || !hasRelativeThis(thisptr)
                              ? thisptr
                              : inlineGetThisPtr(thisptr);
                sym = makeID(sc_auto, sym->tp, nullptr, AnonymousName());
                sym->allocate = true;
                sym->inAllocTable = true;
                lst->data = sym;
                lst->next = temporarySymbols;
                temporarySymbols = lst;
                dest = varNode(en_auto, sym);
                deref(sym->tp, &dest);
                inlinesym_thisptr[inlinesym_count] = dest;
                idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                src = gen_expr(funcsp, thisptr, 0, natural_size(thisptr));
                ap1 = LookupLoadTemp(nullptr, src);
                if (ap1 != src)
                {
                    gen_icode(i_assn, ap1, src, nullptr);
                    src = ap1;
                }
                gen_icode(i_assn, idest, src, nullptr);
            }
        }
        else if (inlinesym_count)
        {
            inlinesym_thisptr[inlinesym_count] = inlinesym_thisptr[inlinesym_count - 1];
        }
    }
}
static void inlineBindArgs(SYMBOL* funcsp, SYMLIST* hr, INITLIST* args)
{
    if (hr)
    {
        EXPRESSION** list;
        SYMLIST* hr1;
        int cnt = 0;
        SYMBOL* sym = hr->p;
        if (sym->thisPtr)
        {
            hr = hr->next;
        }
        hr1 = hr;
        while (hr1)
        {
            cnt++;
            hr1 = hr1->next;
        }
        hr1 = hr;
        list = (EXPRESSION**)(EXPRESSION*)Alloc(sizeof(EXPRESSION*) * cnt);
        cnt = 0;
        while (hr && args)  // args might go to nullptr for a destructor, which currently has a VOID at the end of the arg list
        {
            SYMBOL* sym = hr->p;
            if (!isvoid(sym->tp))
            {
                IMODE *src, *ap1, *idest;
                EXPRESSION* dest;
                SYMBOL* sym2 = makeID(sc_auto, sym->tp, nullptr, AnonymousName());
                LIST* lst = (LIST*)Alloc(sizeof(LIST));
                sym2->allocate = true;
                sym2->inAllocTable = true;
                lst->data = sym2;
                lst->next = temporarySymbols;
                temporarySymbols = lst;
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
                sym->inlineFunc.stmt = (STATEMENT*)dest;
                idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                src = gen_expr(funcsp, args->exp, 0, natural_size(args->exp));
                ap1 = LookupLoadTemp(nullptr, src);
                if (ap1 != src)
                {
                    gen_icode(i_assn, ap1, src, nullptr);
                    src = ap1;
                }
                gen_icode(i_assn, idest, src, nullptr);
            }
            args = args->next;
            hr = hr->next;
        }
        // we have to fill in the args last in case the same constructor was used
        // in multiple arguments...
        hr = hr1;
        cnt = 0;
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (!isvoid(sym->tp))
            {
                sym->inlineFunc.stmt = (STATEMENT*)list[cnt++];
            }
            hr = hr->next;
        }
    }
}
static void inlineUnbindArgs(SYMLIST* hr)
{
    while (hr)
    {
        SYMBOL* sym = hr->p;
        sym->inlineFunc.stmt = nullptr;
        hr = hr->next;
    }
}
static void inlineResetTable(SYMLIST* table)
{
    while (table)
    {
        SYMBOL* sym = (SYMBOL*)table->p;
        sym->imvalue = nullptr;
        sym->imind = nullptr;
        sym->imaddress = nullptr;
        sym->imstore = nullptr;
        sym->allocate = false;
        sym->inAllocTable = false;
        table = table->next;
    }
}
// this is overkill since stmt.c disallows inlines with variables other than
// at the opening of the first block.
static void inlineResetVars(HASHTABLE* syms, SYMLIST* params)
{
    HASHTABLE* old = syms;
    inlineResetTable(params);
    while (syms)
    {

        inlineResetTable(syms->table[0]);
        syms = syms->next;
    }
    while (old)
    {

        inlineResetTable(old->table[0]);
        old = old->chain;
    }
}
static void inlineCopySyms(HASHTABLE* src)
{
    while (src)
    {
        SYMLIST* hr = src->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (!sym->thisPtr && !sym->anonymous && sym->storage_class != sc_parameter)
            {
                if (!sym->inAllocTable)
                {
                    LIST* lst = (LIST*)Alloc(sizeof(LIST));
                    lst->data = sym;
                    lst->next = temporarySymbols;
                    temporarySymbols = lst;
                    sym->inAllocTable = true;
                }
            }
            hr = hr->next;
        }
        src = src->next;
    }
}
static bool inlineTooComplex(FUNCTIONCALL* f) { return f->sp->endLine - f->sp->startLine > 15 / (inline_nesting * 2 + 1); }
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
IMODE* gen_inline(SYMBOL* funcsp, EXPRESSION* node, int flags)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    int i;
    IMODE* ap3;
    FUNCTIONCALL* f = node->v.func;
    SYMLIST* hr;
    IMODE* oldReturnImode = returnImode;
    int oldretlab = retlab, oldstartlab = startlab;
    int oldretcount = retcount;
    int oldOffset = codeLabelOffset;
    EXPRESSION* oldthis = inlinesym_thisptr[inlinesym_count];
    //    return nullptr;
    if (chosenAssembler->arch->denyopts & DO_NOINLINE)
        return nullptr;
    if (cparams.prm_debug)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    /* measure of complexity */
    if (inlineTooComplex(f))
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->fcall->type != en_pc)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->storage_class == sc_virtual)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp == theCurrentFunc)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->allocaUsed)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->sp->templateLevel && f->sp->templateParams && !f->sp->instantiated)  // specialized)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (!f->sp->inlineFunc.syms)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (!f->sp->inlineFunc.stmt)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (inlinesym_count >= MAX_INLINE_NESTING)
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (f->thisptr)
    {
        if (f->thisptr->type == en_auto && f->thisptr->v.sp->stackblock)
        {
            f->sp->dumpInlineToFile = true;
            return nullptr;
        }
    }
    if (f->returnEXP && !isref(basetype(f->sp->tp)->btp))
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    // if it has a structured return value or structured arguments we don't try to inline it
    if (isstructured(basetype(f->sp->tp)->btp))
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    if (basetype(basetype(f->sp->tp)->btp)->type == bt_memberptr)  // DAL FIXED
    {
        f->sp->dumpInlineToFile = true;
        return nullptr;
    }
    hr = basetype(f->sp->tp)->syms->table[0];
    while (hr)
    {
        if (isstructured(hr->p->tp) || basetype(hr->p->tp)->type == bt_memberptr)
        {
            f->sp->dumpInlineToFile = true;
            return nullptr;
        }
        hr = hr->next;
    }
    for (i = 0; i < inlinesym_count; i++)
        if (f->sp == inlinesym_list[i])
        {
            f->sp->dumpInlineToFile = true;
            return nullptr;
        }
    // this is here because cmdswitch uses a unique_ptr and autoincrement of a structure member together, and the resulting code gen
    // fails
    INITLIST* fargs = f->arguments;
    while (fargs)
    {
        if (hasaincdec(fargs->exp))
        {
            f->sp->dumpInlineToFile = true;
            return nullptr;
        }
        fargs = fargs->next;
    }
    inline_nesting++;
    codeLabelOffset = nextLabel - INT_MIN;
    nextLabel += f->sp->labelCount + 10;
    retcount = 0;
    returnImode = nullptr;
    startlab = nextLabel++;
    retlab = nextLabel++;
    AllocateLocalContext(nullptr, funcsp, nextLabel++);
    inlineBindThis(funcsp, basetype(f->sp->tp)->syms->table[0], f->thisptr);
    inlineBindArgs(funcsp, basetype(f->sp->tp)->syms->table[0], f->arguments);
    inlinesym_list[inlinesym_count++] = f->sp;
    inlineResetVars(f->sp->inlineFunc.syms, basetype(f->sp->tp)->syms->table[0]);
    inlineCopySyms(f->sp->inlineFunc.syms);
    genstmt(f->sp->inlineFunc.stmt->lower, f->sp);
    if (f->sp->inlineFunc.stmt->blockTail)
    {
        gen_icode(i_functailstart, 0, 0, 0);
        genstmt(f->sp->inlineFunc.stmt->blockTail, funcsp);
        gen_icode(i_functailend, 0, 0, 0);
    }
    genreturn(0, f->sp, 1, 0, nullptr);
    ap3 = returnImode;
    if (!ap3)
        ap3 = tempreg(ISZ_UINT, 0);
    inlineUnbindArgs(basetype(f->sp->tp)->syms->table[0]);
    FreeLocalContext(nullptr, funcsp, nextLabel++);
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