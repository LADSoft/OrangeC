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
#include "ccerr.h"
#include "template.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "declcpp.h"
#include "lex.h"
#include "declare.h"
#include "memory.h"
#include "wseh.h"
#include "stmt.h"
#include "symtab.h"

namespace Parser
{
static void ReorderSEHRecords(std::list<STATEMENT*>* xtry, std::list<BLOCKDATA*>& parent)
{
    STATEMENT *xfinally = nullptr, *xfault = nullptr;
    for (auto it = xtry->begin(); it != xtry->end(); ++it)
    {
        if ((*it)->type == StatementNode::seh_finally_)
        {
            xfinally = *it;
            it = xtry->erase(it);
        }
        else if ((*it)->type == StatementNode::seh_fault_)
        {
            xfault = *it;
            it = xtry->erase(it);
        }
    }
    if (xfault)
    {
        if (xtry->front() != xtry->back())
        {
            STATEMENT* st = stmtNode(nullptr, emptyBlockdata, StatementNode::seh_try_);
            st->lower = xtry;
            xtry->clear();
            xtry->push_back(st);
        }
        xtry->push_back(xfault);
    }
    if (xfinally)
    {
        if (xtry->front() != xtry->back())
        {
            STATEMENT* st = stmtNode(nullptr, emptyBlockdata, StatementNode::seh_try_);
            st->lower = xtry;
            xtry->clear();
            xtry->push_back(st);
        }
        xtry->push_back(xfinally);
    }
    parent.front()->statements->insert(parent.front()->statements->end(), xtry->begin(), xtry->end());
}
static LEXLIST* SEH_catch(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    STATEMENT* st;
    TYPE* tp = nullptr;
    BLOCKDATA* catchstmt = Allocate<BLOCKDATA>();
    SYMBOL* sym = nullptr;
    lex = getsym();
    ParseAttributeSpecifiers(&lex, funcsp, true);
    catchstmt->breaklabel = -1;
    catchstmt->defaultlabel = -1; /* no default */
    catchstmt->type = Keyword::catch_;
    catchstmt->table = localNameSpace->front()->syms;
    parent.push_front(catchstmt);
    inLoopOrConditional++;
    AllocateLocalContext(parent, funcsp, codeLabel++);
    if (MATCHKW(lex, Keyword::openpa_))
    {
        needkw(&lex, Keyword::openpa_);
        lex = declare(lex, funcsp, &tp, StorageClass::auto_, Linkage::none_, parent, false, true, false, AccessLevel::public_);
        needkw(&lex, Keyword::closepa_);
        sym = localNameSpace->front()->syms->front();
    }
    else
    {
        tp = nullptr;
    }
    if (MATCHKW(lex, Keyword::begin_))
    {
        lex = compound(lex, funcsp, parent, false);
        before->nosemi = true;
        before->needlabel &= catchstmt->needlabel;
        if (next != parent.end())
            (*next)->nosemi = true;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    inLoopOrConditional--;
    FreeLocalContext(parent, funcsp, codeLabel++);
    st = stmtNode(lex, parent, StatementNode::seh_catch_);
    st->blockTail = catchstmt->blockTail;
    st->lower = catchstmt->statements;
    st->tp = tp;
    st->sp = sym;
    parent.pop_front();
    return lex;
}
static LEXLIST* SEH_finally(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    STATEMENT* st;
    BLOCKDATA* finallystmt = Allocate<BLOCKDATA>();
    lex = getsym();
    ParseAttributeSpecifiers(&lex, funcsp, true);
    finallystmt->breaklabel = -1;
    finallystmt->defaultlabel = -1; /* no default */
    finallystmt->type = Keyword::catch_;
    finallystmt->table = localNameSpace->front()->syms;
    parent.push_front(finallystmt);
    AllocateLocalContext(parent, funcsp, codeLabel++);
    inLoopOrConditional++;
    if (MATCHKW(lex, Keyword::begin_))
    {
        lex = compound(lex, funcsp, parent, false);
        before->nosemi = true;
        before->needlabel &= finallystmt->needlabel;
        if (next != parent.end())
            (*next)->nosemi = true;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    FreeLocalContext(parent, funcsp, codeLabel++);
    inLoopOrConditional--;
    st = stmtNode(lex, parent, StatementNode::seh_finally_);
    st->blockTail = finallystmt->blockTail;
    st->lower = finallystmt->statements;
    parent.pop_front();
    return lex;
}
static LEXLIST* SEH_fault(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    STATEMENT* st;
    BLOCKDATA* faultstmt = Allocate<BLOCKDATA>();
    lex = getsym();
    ParseAttributeSpecifiers(&lex, funcsp, true);
    faultstmt->breaklabel = -1;
    faultstmt->defaultlabel = -1; /* no default */
    faultstmt->type = Keyword::catch_;
    faultstmt->table = localNameSpace->front()->syms;
    parent.push_front(faultstmt);
    AllocateLocalContext(parent, funcsp, codeLabel++);
    inLoopOrConditional++;
    if (MATCHKW(lex, Keyword::begin_))
    {
        lex = compound(lex, funcsp, parent, false);
        before->nosemi = true;
        before->needlabel &= faultstmt->needlabel;
        if (next != parent.end())
            (*next)->nosemi = true;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    FreeLocalContext(parent, funcsp, codeLabel++);
    st = stmtNode(lex, parent, StatementNode::seh_fault_);
    st->blockTail = faultstmt->blockTail;
    st->lower = faultstmt->statements;
    inLoopOrConditional--;
    parent.pop_front();
    return lex;
}
static LEXLIST* SEH_try(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    STATEMENT *st;
    BLOCKDATA* trystmt = Allocate<BLOCKDATA>();
    trystmt->breaklabel = -1;
    trystmt->defaultlabel = -1; /* no default */
    trystmt->type = Keyword::seh_try_;
    trystmt->table = localNameSpace->front()->syms;
    parent.push_front(trystmt);
    lex = getsym();
    inLoopOrConditional++;
    if (!MATCHKW(lex, Keyword::begin_))
    {
        error(ERR_EXPECTED_TRY_BLOCK);
    }
    else
    {
        auto prev = parent.front()->statements;
        parent.front()->statements = nullptr;
        bool foundFinally = false, foundFault = false;
        AllocateLocalContext(parent, funcsp, codeLabel++);
        lex = compound(lex, funcsp, parent, false);
        FreeLocalContext(parent, funcsp, codeLabel++);
        before->needlabel = trystmt->needlabel;
        st = stmtNode(lex, parent, StatementNode::seh_try_);
        st->blockTail = trystmt->blockTail;
        st->lower = trystmt->statements;
        before->nosemi = true;
        if (next != parent.end())
            (*next)->nosemi = true;
        if (!MATCHKW(lex, Keyword::seh_catch_) && !MATCHKW(lex, Keyword::seh_finally_) && !MATCHKW(lex, Keyword::seh_fault_))
        {
            error(ERR_EXPECTED_SEH_HANDLER);
        }
        while (MATCHKW(lex, Keyword::seh_catch_) || MATCHKW(lex, Keyword::seh_finally_) || MATCHKW(lex, Keyword::seh_fault_))
        {
            if (MATCHKW(lex, Keyword::seh_finally_))
            {
                if (foundFinally)
                    error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                else
                    foundFinally = true;
            }
            else if (MATCHKW(lex, Keyword::seh_fault_))
            {
                if (foundFault)
                    error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                else
                    foundFault = true;
            }
            lex = statement_SEH(lex, funcsp, parent);
        }
        ReorderSEHRecords(parent.front()->statements, parent);
        parent.front()->statements->insert(parent.front()->statements->begin(), prev->begin(), prev->end());
    }
    inLoopOrConditional--;
    parent.pop_front();
    return lex;
}

LEXLIST* statement_SEH(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    switch (KW(lex))
    {
        case Keyword::seh_try_:
            return SEH_try(lex, funcsp, parent);
        case Keyword::seh_catch_:
            return SEH_catch(lex, funcsp, parent);
        case Keyword::seh_finally_:
            return SEH_finally(lex, funcsp, parent);
        case Keyword::seh_fault_:
            return SEH_fault(lex, funcsp, parent);
        default:
            break;
    }
    return lex;
}
}  // namespace Parser