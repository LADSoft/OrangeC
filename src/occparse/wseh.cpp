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
#include "ccerr.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "declcpp.h"
#include "lex.h"
#include "declare.h"
#include "memory.h"
#include "wseh.h"
#include "stmt.h"
#include "namespace.h"
#include "symtab.h"
#include "attribs.h"

namespace Parser
{
static void ReorderSEHRecords(std::list<Statement*>* xtry, std::list<FunctionBlock*>& parent)
{
    Statement *xfinally = nullptr, *xfault = nullptr;
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
            Statement* st = Statement::MakeStatement(emptyBlockdata, StatementNode::seh_try_);
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
            Statement* st = Statement::MakeStatement(emptyBlockdata, StatementNode::seh_try_);
            st->lower = xtry;
            xtry->clear();
            xtry->push_back(st);
        }
        xtry->push_back(xfinally);
    }
    parent.front()->statements->insert(parent.front()->statements->end(), xtry->begin(), xtry->end());
}
static void SEH_catch( SYMBOL* funcsp, std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    Statement* st;
    Type* tp = nullptr;
    FunctionBlock* catchstmt = Allocate<FunctionBlock>();
    SYMBOL* sym = nullptr;
    getsym();
    ParseAttributeSpecifiers(funcsp, true);
    catchstmt->breaklabel = -1;
    catchstmt->defaultlabel = -1; /* no default */
    catchstmt->type = Keyword::catch_;
    catchstmt->table = localNameSpace->front()->syms;
    parent.push_front(catchstmt);
    processingLoopOrConditional++;
    StatementGenerator::AllocateLocalContext(parent, funcsp, codeLabel++);
    if (MATCHKW(Keyword::openpa_))
    {
        needkw(Keyword::openpa_);
        declare(funcsp, &tp, StorageClass::auto_, Linkage::none_, parent, false, true, false, AccessLevel::public_);
        needkw(Keyword::closepa_);
        sym = localNameSpace->front()->syms->front();
    }
    else
    {
        tp = nullptr;
    }
    if (MATCHKW(Keyword::begin_))
    {
        StatementGenerator sg(funcsp);
        sg.Compound(parent, false);
        before->nosemi = true;
        before->needlabel &= catchstmt->needlabel;
        if (next != parent.end())
            (*next)->nosemi = true;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    processingLoopOrConditional--;
    StatementGenerator::FreeLocalContext(parent, funcsp, codeLabel++);
    st = Statement::MakeStatement(parent, StatementNode::seh_catch_);
    st->blockTail = catchstmt->blockTail;
    st->lower = catchstmt->statements;
    st->tp = tp;
    st->sp = sym;
    parent.pop_front();
    return;
}
static void SEH_finally( SYMBOL* funcsp, std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    Statement* st;
    FunctionBlock* finallystmt = Allocate<FunctionBlock>();
    getsym();
    ParseAttributeSpecifiers(funcsp, true);
    finallystmt->breaklabel = -1;
    finallystmt->defaultlabel = -1; /* no default */
    finallystmt->type = Keyword::catch_;
    finallystmt->table = localNameSpace->front()->syms;
    parent.push_front(finallystmt);
    StatementGenerator::AllocateLocalContext(parent, funcsp, codeLabel++);
    processingLoopOrConditional++;
    if (MATCHKW(Keyword::begin_))
    {
        StatementGenerator sg(funcsp);
        sg.Compound(parent, false);
        before->nosemi = true;
        before->needlabel &= finallystmt->needlabel;
        if (next != parent.end())
            (*next)->nosemi = true;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    StatementGenerator::FreeLocalContext(parent, funcsp, codeLabel++);
    processingLoopOrConditional--;
    st = Statement::MakeStatement(parent, StatementNode::seh_finally_);
    st->blockTail = finallystmt->blockTail;
    st->lower = finallystmt->statements;
    parent.pop_front();
    return;
}
static void SEH_fault( SYMBOL* funcsp, std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    Statement* st;
    FunctionBlock* faultstmt = Allocate<FunctionBlock>();
    getsym();
    ParseAttributeSpecifiers(funcsp, true);
    faultstmt->breaklabel = -1;
    faultstmt->defaultlabel = -1; /* no default */
    faultstmt->type = Keyword::catch_;
    faultstmt->table = localNameSpace->front()->syms;
    parent.push_front(faultstmt);
    StatementGenerator::AllocateLocalContext(parent, funcsp, codeLabel++);
    processingLoopOrConditional++;
    if (MATCHKW(Keyword::begin_))
    {
        StatementGenerator sg(funcsp);
        sg.Compound(parent, false);
        before->nosemi = true;
        before->needlabel &= faultstmt->needlabel;
        if (next != parent.end())
            (*next)->nosemi = true;
    }
    else
    {
        error(ERR_EXPECTED_CATCH_BLOCK);
    }
    StatementGenerator::FreeLocalContext(parent, funcsp, codeLabel++);
    st = Statement::MakeStatement(parent, StatementNode::seh_fault_);
    st->blockTail = faultstmt->blockTail;
    st->lower = faultstmt->statements;
    processingLoopOrConditional--;
    parent.pop_front();
    return;
}
static void SEH_try( SYMBOL* funcsp, std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    auto next = parent.begin();
    ++next;
    Statement* st;
    FunctionBlock* trystmt = Allocate<FunctionBlock>();
    trystmt->breaklabel = -1;
    trystmt->defaultlabel = -1; /* no default */
    trystmt->type = Keyword::seh_try_;
    trystmt->table = localNameSpace->front()->syms;
    parent.push_front(trystmt);
    getsym();
    processingLoopOrConditional++;
    if (!MATCHKW(Keyword::begin_))
    {
        error(ERR_EXPECTED_TRY_BLOCK);
    }
    else
    {
        auto prev = parent.front()->statements;
        parent.front()->statements = nullptr;
        bool foundFinally = false, foundFault = false;
        StatementGenerator::AllocateLocalContext(parent, funcsp, codeLabel++);
        StatementGenerator sg(funcsp);
        sg.Compound(parent, false);
        StatementGenerator::FreeLocalContext(parent, funcsp, codeLabel++);
        before->needlabel = trystmt->needlabel;
        st = Statement::MakeStatement(parent, StatementNode::seh_try_);
        st->blockTail = trystmt->blockTail;
        st->lower = trystmt->statements;
        before->nosemi = true;
        if (next != parent.end())
            (*next)->nosemi = true;
        if (!MATCHKW(Keyword::seh_catch_) && !MATCHKW(Keyword::seh_finally_) && !MATCHKW(Keyword::seh_fault_))
        {
            error(ERR_EXPECTED_SEH_HANDLER);
        }
        while (MATCHKW(Keyword::seh_catch_) || MATCHKW(Keyword::seh_finally_) || MATCHKW(Keyword::seh_fault_))
        {
            if (MATCHKW(Keyword::seh_finally_))
            {
                if (foundFinally)
                    error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                else
                    foundFinally = true;
            }
            else if (MATCHKW(Keyword::seh_fault_))
            {
                if (foundFault)
                    error(ERR_FINALLY_FAULT_APPEAR_ONLY_ONCE);
                else
                    foundFault = true;
            }
            ParseSEH(funcsp, parent);
        }
        ReorderSEHRecords(parent.front()->statements, parent);
        parent.front()->statements->insert(parent.front()->statements->begin(), prev->begin(), prev->end());
    }
    processingLoopOrConditional--;
    parent.pop_front();
    return;
}

void ParseSEH( SYMBOL* funcsp, std::list<FunctionBlock*>& parent)
{
    switch (KW())
    {
        case Keyword::seh_try_:
            return SEH_try(funcsp, parent);
        case Keyword::seh_catch_:
            return SEH_catch(funcsp, parent);
        case Keyword::seh_finally_:
            return SEH_finally(funcsp, parent);
        case Keyword::seh_fault_:
            return SEH_fault(funcsp, parent);
        default:
            break;
    }
    return;
}
}  // namespace Parser