/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
#include "ccerr.h"
#include "config.h"
#include "symtab.h"
#include "initbackend.h"
#include "stmt.h"
#include "declcpp.h"
#include "lex.h"
#include "declare.h"
#include "memory.h"
#include "wseh.h"
namespace Parser
{
    static void ReorderSEHRecords(STATEMENT** xtry, BLOCKDATA* parent)
    {
        STATEMENT *xfinally = nullptr, *xfault = nullptr, **pass = xtry;
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
            xfault->next = nullptr;
            if ((*xtry)->next)
            {
                STATEMENT* st = stmtNode(nullptr, nullptr, st___try);
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
            xfinally->next = nullptr;
            if ((*xtry)->next)
            {
                STATEMENT* st = stmtNode(nullptr, nullptr, st___try);
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
    static LEXEME* SEH_catch(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
    {
        STATEMENT* st;
        TYPE* tp = nullptr;
        BLOCKDATA* catchstmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
        SYMBOL* sym = nullptr;
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        catchstmt->breaklabel = -1;
        catchstmt->next = nullptr;    // so can't break or continue out of the block
        catchstmt->defaultlabel = -1; /* no default */
        catchstmt->type = kw_catch;
        catchstmt->table = localNameSpace->valueData->syms;
        AllocateLocalContext(catchstmt, funcsp, codeLabel++);
        if (MATCHKW(lex, openpa))
        {
            needkw(&lex, openpa);
            lex = declare(lex, funcsp, &tp, sc_auto, lk_none, catchstmt, false, true, false, ac_public);
            needkw(&lex, closepa);
            sym = localNameSpace->valueData->syms->table[0]->p;
        }
        else
        {
            tp = nullptr;
        }
        if (MATCHKW(lex, begin))
        {
            lex = compound(lex, funcsp, catchstmt, false);
            parent->nosemi = true;
            parent->needlabel &= catchstmt->needlabel;
            if (parent->next)
                parent->next->nosemi = true;
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
        st->sp = sym;
        return lex;
    }
    static LEXEME* SEH_finally(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
    {
        STATEMENT* st;
        BLOCKDATA* catchstmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        catchstmt->breaklabel = -1;
        catchstmt->next = nullptr;    // so can't break or continue out of the block
        catchstmt->defaultlabel = -1; /* no default */
        catchstmt->type = kw_catch;
        catchstmt->table = localNameSpace->valueData->syms;
        AllocateLocalContext(catchstmt, funcsp, codeLabel++);
        if (MATCHKW(lex, begin))
        {
            lex = compound(lex, funcsp, catchstmt, false);
            parent->nosemi = true;
            parent->needlabel &= catchstmt->needlabel;
            if (parent->next)
                parent->next->nosemi = true;
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
    static LEXEME* SEH_fault(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
    {
        STATEMENT* st;
        BLOCKDATA* catchstmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
        lex = getsym();
        ParseAttributeSpecifiers(&lex, funcsp, true);
        catchstmt->breaklabel = -1;
        catchstmt->next = nullptr;    // so can't break or continue out of the block
        catchstmt->defaultlabel = -1; /* no default */
        catchstmt->type = kw_catch;
        catchstmt->table = localNameSpace->valueData->syms;
        AllocateLocalContext(catchstmt, funcsp, codeLabel++);
        if (MATCHKW(lex, begin))
        {
            lex = compound(lex, funcsp, catchstmt, false);
            parent->nosemi = true;
            parent->needlabel &= catchstmt->needlabel;
            if (parent->next)
                parent->next->nosemi = true;
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
    static LEXEME* SEH_try(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
    {
        STATEMENT *st, **tail = parent->head ? &parent->tail->next : &parent->head;
        BLOCKDATA* trystmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
        trystmt->breaklabel = -1;
        trystmt->next = nullptr;    // so we can't break or continue out of the block
        trystmt->defaultlabel = -1; /* no default */
        trystmt->type = kw_try;
        trystmt->table = localNameSpace->valueData->syms;
        lex = getsym();
        if (!MATCHKW(lex, begin))
        {
            error(ERR_EXPECTED_TRY_BLOCK);
        }
        else
        {
            bool foundFinally = false, foundFault = false;
            AllocateLocalContext(trystmt, funcsp, codeLabel++);
            lex = compound(lex, funcsp, trystmt, false);
            FreeLocalContext(trystmt, funcsp, codeLabel++);
            parent->needlabel = trystmt->needlabel;
            st = stmtNode(lex, parent, st___try);
            st->blockTail = trystmt->blockTail;
            st->lower = trystmt->head;
            parent->nosemi = true;
            if (parent->next)
                parent->next->nosemi = true;
            if (!MATCHKW(lex, kw___catch) && !MATCHKW(lex, kw___finally) && !MATCHKW(lex, kw___fault))
            {
                error(ERR_EXPECTED_SEH_HANDLER);
            }
            while (MATCHKW(lex, kw___catch) || MATCHKW(lex, kw___finally) || MATCHKW(lex, kw___fault))
            {
                if (MATCHKW(lex, kw___finally))
                {
                    if (foundFinally)
                        error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                    else
                        foundFinally = true;
                }
                else if (MATCHKW(lex, kw___fault))
                {
                    if (foundFault)
                        error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                    else
                        foundFault = true;
                }
                lex = statement_SEH(lex, funcsp, parent);
            }
            ReorderSEHRecords(tail, parent);
        }

        return lex;
    }

    LEXEME* statement_SEH(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
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
        default:
            break;
        }
        return lex;
    }
}