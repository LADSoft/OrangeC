/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

#include "common.h"

extern ARCH_ASM *chosenAssembler;
extern enum e_kw skim_end[];
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern TYPE stdvoid;
extern int total_errors;
extern int startlab;
extern int retlab;
extern int nextLabel;
extern int codeLabel;

static void ReorderSEHRecords(STATEMENT **xtry, BLOCKDATA *parent)
{
    STATEMENT *xfinally = NULL, *xfault = NULL, **pass = xtry;
    while (*pass)
    {
        if ((*pass)->type == st___finally)
        {
            xfinally = *pass;
            *pass = (*pass)->next;
        }
        else if ((*pass)->type == st___fault)
        {
            xfault = *pass;
            *pass = (*pass)->next;
        }
        else
        {
            pass = &(*pass)->next;
        }
    }
    if (xfault)
    {
        xfault->next = NULL;
        if ((*xtry)->next)
        {
            STATEMENT *st = stmtNode(NULL, NULL, st___try);
            st->lower = *xtry;
            st->next = xfault;
            *xtry = st;
        }
        else
        {
            (*xtry)->next = xfault;
        }
    }
    if (xfinally)
    {
        xfinally->next = NULL;
        if ((*xtry)->next)
        {
            STATEMENT *st = stmtNode(NULL, NULL, st___try);
            st->lower = *xtry;
            st->next = xfinally;
            *xtry = st;
        }
        else
        {
            (*xtry)->next = xfinally;
        }
    }
    parent->tail = *xtry;
    while (parent->tail->next)
        parent->tail = parent->tail->next;
}
static LEXEME *SEH_catch(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st;
    TYPE *tp = NULL;
    BLOCKDATA *catchstmt = Alloc(sizeof(BLOCKDATA));
    SYMBOL *sym = NULL;
    lex = getsym();
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    catchstmt->breaklabel = -1;
    catchstmt->next = NULL; // so can't break or continue out of the block
    catchstmt->defaultlabel = -1; /* no default */
    catchstmt->type = kw_catch;
    catchstmt->table = localNameSpace->syms;
    AllocateLocalContext(catchstmt, funcsp, codeLabel++);
    if (MATCHKW(lex, openpa))
    {
        needkw(&lex, openpa);
        lex = declare(lex, funcsp, &tp, sc_auto, lk_none, catchstmt, FALSE, TRUE, FALSE, FALSE, ac_public);
        needkw(&lex, closepa);
        sym = SYMTABBEGIN(localNameSpace)->p;
    }
    else
    {
        tp = NULL;
    }
    if (MATCHKW(lex, begin))
    {
        lex = compound(lex, funcsp, catchstmt, FALSE);
        parent->nosemi = TRUE;
        parent->needlabel &= catchstmt->needlabel;
        if (parent->next)
            parent->next->nosemi = TRUE;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    FreeLocalContext(catchstmt, funcsp, codeLabel++);
    st = stmtNode(lex, parent, st___catch);
    st->blockTail = catchstmt->blockTail;
    st->lower = catchstmt->head;
    st->tp = tp;
    st->sym = sym;
    return lex;
}
static LEXEME *SEH_finally(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st;
    BLOCKDATA *catchstmt = Alloc(sizeof(BLOCKDATA));
    lex = getsym();
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    catchstmt->breaklabel = -1;
    catchstmt->next = NULL; // so can't break or continue out of the block
    catchstmt->defaultlabel = -1; /* no default */
    catchstmt->type = kw_catch;
    catchstmt->table = localNameSpace->syms;
    AllocateLocalContext(catchstmt, funcsp, codeLabel++);
    if (MATCHKW(lex, begin))
    {
        lex = compound(lex, funcsp, catchstmt, FALSE);
        parent->nosemi = TRUE;
        parent->needlabel &= catchstmt->needlabel;
        if (parent->next)
            parent->next->nosemi = TRUE;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    FreeLocalContext(catchstmt, funcsp, codeLabel++);
    st = stmtNode(lex, parent, st___finally);
    st->blockTail = catchstmt->blockTail;
    st->lower = catchstmt->head;
    return lex;
}
static LEXEME *SEH_fault(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st;
    BLOCKDATA *catchstmt = Alloc(sizeof(BLOCKDATA));
    lex = getsym();
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    catchstmt->breaklabel = -1;
    catchstmt->next = NULL; // so can't break or continue out of the block
    catchstmt->defaultlabel = -1; /* no default */
    catchstmt->type = kw_catch;
    catchstmt->table = localNameSpace->syms;
    AllocateLocalContext(catchstmt, funcsp, codeLabel++);
    if (MATCHKW(lex, begin))
    {
        lex = compound(lex, funcsp, catchstmt, FALSE);
        parent->nosemi = TRUE;
        parent->needlabel &= catchstmt->needlabel;
        if (parent->next)
            parent->next->nosemi = TRUE;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    FreeLocalContext(catchstmt, funcsp, codeLabel++);
    st = stmtNode(lex, parent, st___fault);
    st->blockTail = catchstmt->blockTail;
    st->lower = catchstmt->head;
    return lex;
}
static LEXEME *SEH_try(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st, **tail = parent->head ? &parent->tail->next : &parent->head;
    BLOCKDATA *trystmt = Alloc(sizeof(BLOCKDATA));
    trystmt->breaklabel = -1;
    trystmt->next = NULL; // so we can't break or continue out of the block
    trystmt->defaultlabel = -1; /* no default */
    trystmt->type = kw_try;
    trystmt->table = localNameSpace->syms;
    lex = getsym();
    if (!MATCHKW(lex, begin))
    {
        error(ERR_EXPECTED_TRY_BLOCK);
    }
    else
    {
        BOOLEAN foundFinally = FALSE, foundFault = FALSE;
        AllocateLocalContext(trystmt, funcsp, codeLabel++);
        lex = compound(lex, funcsp, trystmt, FALSE);
        FreeLocalContext(trystmt, funcsp, codeLabel++);
        parent->needlabel = trystmt->needlabel;
        st = stmtNode(lex, parent, st___try);
        st->blockTail = trystmt->blockTail;
        st->lower = trystmt->head;
        parent->nosemi = TRUE;
        if (parent->next)
            parent->next->nosemi = TRUE;
        if (!MATCHKW(lex, kw___catch) && !MATCHKW(lex, kw___finally) && !MATCHKW(lex, kw___fault))
        {
            error(ERR_EXPECTED_SEH_HANDLER);
        }
        while (MATCHKW(lex, kw___catch) || MATCHKW(lex, kw___finally) || MATCHKW(lex, kw___fault))
        {
            if (MATCHKW(lex, kw___finally))
                if (foundFinally)
                    error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                else
                    foundFinally = TRUE;
            else if (MATCHKW(lex, kw___fault))
                if (foundFault)
                    error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                else
                    foundFault = TRUE;
            lex = statement_SEH(lex, funcsp, parent);
        }
        ReorderSEHRecords(tail, parent);
    }

    return lex;
}

LEXEME *statement_SEH(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    switch (KW(lex))
    {
        case kw___try:
            return SEH_try(lex, funcsp, parent);
        case kw___catch:
            return SEH_catch(lex, funcsp, parent);
        case kw___finally:
            return SEH_finally(lex, funcsp, parent);
        case kw___fault:
            return SEH_fault(lex, funcsp, parent);
    }
    return lex;
}