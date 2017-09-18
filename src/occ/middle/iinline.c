/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

extern ARCH_ASM *chosenAssembler;
extern TYPE stdpointer;
extern IMODE *returnImode;
extern int startlab;
extern int retlab;
extern int nextLabel;
extern int codeLabelOffset;
extern int retcount;
extern LIST *temporarySymbols;

int inlinesym_count;
EXPRESSION *inlinesym_thisptr[MAX_INLINE_NESTING];
static SYMBOL *inlinesym_list[MAX_INLINE_NESTING];
#undef MAX_INLINE_NESTING
#define MAX_INLINE_NESTING 3

static int inline_nesting;
void iinlineInit(void)
{
    inlinesym_count = 0;
    inline_nesting = 0;
}
static BOOLEAN hasRelativeThis(EXPRESSION *thisPtr)
{
    BOOLEAN rv = FALSE;
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
            rv = TRUE;
    }
    return rv;
}
static EXPRESSION *inlineGetThisPtr(EXPRESSION *exp)
{
    if (exp)
    {
        
        if (lvalue(exp) && exp->left->type == en_auto && exp->left->v.sp->thisPtr)
        {
            return inlinesym_thisptr[inlinesym_count-1];
        }
        else
        {
            EXPRESSION *rv = Alloc(sizeof(EXPRESSION));
            *rv = *exp;
            rv->left = inlineGetThisPtr(rv->left);
            rv->right = inlineGetThisPtr(rv->right);
            return rv;
        }
    }
    return NULL;
}
static void inlineBindThis(SYMBOL *funcsp, HASHREC *hr, EXPRESSION *thisptr)
{
    if (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        inlinesym_thisptr[inlinesym_count] = 0;
        if (sym->thisPtr)
        {
            if (thisptr)
            {
                IMODE *src, *ap1, *idest;
                EXPRESSION *dest;
                LIST *lst = Alloc(sizeof(LIST));
                thisptr = inlinesym_count == 0 || 
                            inlinesym_thisptr[inlinesym_count-1] == NULL || 
                            !hasRelativeThis(thisptr) ? thisptr : inlineGetThisPtr(thisptr);
                sym = makeID(sc_auto, sym->tp, NULL, AnonymousName()); 
                sym->allocate = TRUE;
                sym->inAllocTable = TRUE;
                lst->data = sym;
                lst->next = temporarySymbols;
                temporarySymbols = lst;
                dest = varNode(en_auto, sym);
                deref(sym->tp, &dest);
                inlinesym_thisptr[inlinesym_count] = dest;
                idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                src = gen_expr(funcsp, thisptr, 0, natural_size(thisptr));
                ap1 = LookupLoadTemp(NULL, src);
                if (ap1 != src)
                {
                    gen_icode(i_assn, ap1, src, NULL);
                    src = ap1;
                }
                gen_icode(i_assn, idest, src, NULL);
            }
        }
        else if (inlinesym_count)
        {
            inlinesym_thisptr[inlinesym_count] = inlinesym_thisptr[inlinesym_count-1];
        }
    }
}
static void inlineBindArgs(SYMBOL *funcsp, HASHREC *hr, INITLIST *args)
{
    if (hr)
    {
        EXPRESSION **list;
        HASHREC *hr1;
        int cnt = 0;
        SYMBOL *sym = (SYMBOL *)hr->p;
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
        list = (EXPRESSION **)Alloc(sizeof(EXPRESSION *) * cnt);
        cnt = 0;
        while (hr && args) // args might go to NULL for a destructor, which currently has a VOID at the end of the arg list
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (!isvoid(sym->tp))
            {
                IMODE *src, *ap1, *idest;
                EXPRESSION *dest;
                SYMBOL *sym2 = makeID(sc_auto, sym->tp, NULL, AnonymousName()); 
                LIST *lst = Alloc(sizeof(LIST));
                sym2->allocate = TRUE;
                sym2->inAllocTable = TRUE;
                lst->data = sym2;
                lst->next = temporarySymbols;
                temporarySymbols = lst;
                dest = varNode(en_auto, sym2);
                if (isarray(sym->tp))
                {
                    dest = exprNode(en_l_p, dest, NULL);
                }
                else
                {
                    deref(sym->tp, &dest);
                }
                list[cnt++] = dest;
                sym->inlineFunc.stmt = dest;
                idest = gen_expr(funcsp, dest, F_STORE, natural_size(dest));
                src = gen_expr(funcsp, args->exp, 0, natural_size(args->exp));
                ap1 = LookupLoadTemp(NULL, src);
                if (ap1 != src)
                {
                    gen_icode(i_assn, ap1, src, NULL);
                    src = ap1;
                }
                gen_icode(i_assn, idest, src, NULL);
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
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (!isvoid(sym->tp))
            {
                sym->inlineFunc.stmt = (STATEMENT *)list[cnt++];
            }
            hr = hr->next;
        }
    }
}
static void inlineUnbindArgs(HASHREC *hr)
{
    while (hr)
    {
        SYMBOL *sym = (SYMBOL *)hr->p;
        sym->inlineFunc.stmt = NULL;
        hr = hr->next;
    }
}
static void inlineResetTable(HASHREC *table)
{
    while (table)
    {
        SYMBOL *sym = (SYMBOL *)table->p;
        sym->imvalue = NULL;
        sym->imind = NULL;
        sym->imaddress = NULL;
        sym->imstore = NULL;
        sym->allocate = FALSE;
        sym->inAllocTable = FALSE;
        table = table->next;
        
    }
}
// this is overkill since stmt.c disallows inlines with variables other than
// at the opening of the first block.
static void inlineResetVars(HASHTABLE *syms, HASHREC *params)
{
    HASHTABLE *old = syms;
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
static void inlineCopySyms(HASHTABLE *src)
{
    while (src)
    {
        HASHREC *hr = src->table[0];
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (!sym->thisPtr && !sym->anonymous && sym->storage_class != sc_parameter)
            {
                if (!sym->inAllocTable)
                {
                    LIST *lst = Alloc(sizeof(LIST));
                    lst->data = sym;
                    lst->next = temporarySymbols;
                    temporarySymbols = lst;
                    sym->inAllocTable = TRUE;
                }
            }
            hr = hr->next;
        }
        src = src->next;
    }
    
}
static BOOLEAN inlineTooComplex(FUNCTIONCALL *f)
{
    return f->sp->endLine - f->sp->startLine > 15/ (inline_nesting*2 + 1);
    
}
IMODE *gen_inline(SYMBOL *funcsp, EXPRESSION *node, int flags)
/*
 *      generate a function call node and return the address mode
 *      of the result.
 */
{
    int i;
    IMODE *ap3;
    FUNCTIONCALL *f = node->v.func;
    HASHREC *hr;
    IMODE *oldReturnImode = returnImode;
    int oldretlab = retlab, oldstartlab = startlab;
    int oldretcount = retcount;
    int oldOffset = codeLabelOffset;
    EXPRESSION *oldthis = inlinesym_thisptr[inlinesym_count];

//    return NULL;    
    if (chosenAssembler->arch->denyopts & DO_NOINLINE)
        return NULL;
    if (cparams.prm_debug)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    /* measure of complexity */
    if (inlineTooComplex(f))
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (f->fcall->type != en_pc)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (f->sp->storage_class == sc_virtual)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (f->sp == theCurrentFunc)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (f->sp->allocaUsed)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (f->sp->templateLevel && f->sp->templateParams && !f->sp->instantiated) //specialized)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (!f->sp->inlineFunc.syms)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (!f->sp->inlineFunc.stmt)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (inlinesym_count >= MAX_INLINE_NESTING)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (f->thisptr)
    {
        if (f->thisptr->type == en_auto && f->thisptr->v.sp->stackblock)
        {
            f->sp->dumpInlineToFile = TRUE;
            return NULL;
        }
    }
    if (f->returnEXP)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    // if it has a structured return value or structured arguments we don't try to inline it
    if (isstructured(basetype(f->sp->tp)->btp))
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    if (basetype(basetype(f->sp->tp)->btp) == bt_memberptr)
    {
        f->sp->dumpInlineToFile = TRUE;
        return NULL;
    }
    hr = basetype(f->sp->tp)->syms->table[0];
    while (hr)
    {
        if (isstructured(((SYMBOL *)hr->p)->tp) || basetype(((SYMBOL *)hr->p)->tp)->type == bt_memberptr)
        {
            f->sp->dumpInlineToFile = TRUE;
            return NULL;
        }
        hr = hr->next;
    }
    for (i=0; i < inlinesym_count; i++)
        if (f->sp == inlinesym_list[i])
        {
            f->sp->dumpInlineToFile = TRUE;
            return NULL;
        }
    inline_nesting++;
    codeLabelOffset = nextLabel - INT_MIN ;
    nextLabel += f->sp->labelCount + 10;
    retcount = 0;
    returnImode = NULL;
    startlab = nextLabel++;
    retlab = nextLabel++;
    AllocateLocalContext(NULL, funcsp, nextLabel++);
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
    genreturn(0, f->sp, 1, 0, NULL);
    ap3 = returnImode;
    if (!ap3)
        ap3 = tempreg(ISZ_UINT, 0);
    inlineUnbindArgs(basetype(f->sp->tp)->syms->table[0]);
    FreeLocalContext(NULL, funcsp, nextLabel++);
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