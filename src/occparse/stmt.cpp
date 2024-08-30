/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

/* declare in select has multiple vars */
#include "compiler.h"
#include <climits>
#include <cassert>
#include "PreProcessor.h"
#include "ccerr.h"
#include "config.h"
#include "initbackend.h"
#include "mangle.h"
#include "lex.h"
#include "occparse.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "rtti.h"
#include "ildata.h"
#include "cppbltin.h"
#include "expr.h"
#include "lex.h"
#include "help.h"
#include "declcons.h"
#include "wseh.h"
#include "cpplookup.h"
#include "init.h"
#include "declcpp.h"
#include "constopt.h"
#include "OptUtils.h"
#ifndef ORANGE_NO_MSIL
#include "using.h"
#endif
#include "declare.h"
#include "memory.h"
#include "exprcpp.h"
#include "inline.h"
#include "inasm.h"
#include "beinterf.h"
#include "istmt.h"
#include "types.h"
#include "browse.h"
#include "stmt.h"
#include "optmodules.h"
#include "constexpr.h"
#include "libcxx.h"
#include "symtab.h"
#include "ListFactory.h"

#define MAX_INLINE_EXPRESSIONS 3

namespace Parser
{
void refreshBackendParams(SYMBOL* funcsp);

bool isCallNoreturnFunction;

int inLoopOrConditional;

int funcNesting;
int funcLevel;
int tryLevel;
int ellipsePos;

bool hasFuncCall;
bool hasXCInfo;
int startlab, retlab;
int codeLabel;
bool declareAndInitialize;
bool functionCanThrow;
int bodyIsDestructor;
bool inFunctionExpressionParsing;

std::list<FunctionBlock*> emptyBlockdata;

std::stack<std::list<FunctionBlock*>*> expressionStatements;
std::deque<std::pair<EXPRESSION*, Type*>> expressionReturns;

std::list<Optimizer::LINEDATA*>* lines;

static int matchReturnTypes;
static int endline;
static int caseLevel = 0;
static int controlSequences;
static int expressions;
static bool canFallThrough;
static FunctionBlock* caseDestructorsForBlock;

void statement_ini(bool global)
{
    lines = nullptr;
    functionCanThrow = false;
    funcNesting = 0;
    funcLevel = 0;
    caseDestructorsForBlock = nullptr;
    caseLevel = 0;
    matchReturnTypes = false;
    tryLevel = 0;
    controlSequences = 0;
    expressions = 0;
    inLoopOrConditional = 0;
    bodyIsDestructor = 0;
    inFunctionExpressionParsing = false;
    while (!expressionStatements.empty())
        expressionStatements.pop();
}
bool msilManaged(SYMBOL* s)
{
#ifndef ORANGE_NO_MSIL
    if (IsCompiler())
       return occmsil::msil_managed(Optimizer::SymbolManager::Get(s));
#endif
    return false;
}

Statement* Statement::MakeStatement(LexList* lex, std::list<FunctionBlock*>& parent, StatementNode stype)
{
    Statement* st = Allocate<Statement>();
    if (!lex)
        lex = context->cur ? context->cur->prev : context->last;
    st->type = stype;
    st->charpos = 0;
    st->line = lex->data->errline;
    st->file = lex->data->errfile;
    st->parent = parent.size() ? parent.front() : nullptr;
    if (&parent != &emptyBlockdata)
    {
        if (!parent.front()->statements)
            parent.front()->statements = stmtListFactory.CreateList();
        parent.front()->statements->push_back(st);
    }
    switch (stype)
    {
        case StatementNode::return_:
        case StatementNode::expr_:
            expressions++;
            break;
        case StatementNode::select_:
        case StatementNode::notselect_:
        case StatementNode::goto_:
        case StatementNode::loopgoto_:
            controlSequences++;
            break;
    }
    return st;
}
void FunctionBlock::AddThis(LexList* lex, std::list<FunctionBlock*>& parent)
{
    Statement* st = Statement::MakeStatement(lex, parent, StatementNode::block_);
    st->blockTail = this->blockTail;
    st->lower = this->statements;
}
bool StatementGenerator::IsSelectTrue(EXPRESSION* exp)
{
    if (isintconst(exp->left))
        return !!exp->left->v.i;
    return false;
}
bool StatementGenerator::IsSelectFalse(EXPRESSION* exp)
{
    if (isintconst(exp))
        return !exp->v.i;
    return false;
}
void StatementGenerator::MarkInitializersAsDeclarations(std::list<Statement*>& lst)
{
    for (auto it = lst.begin(); it != lst.end(); ++it)
        if ((*it)->type == StatementNode::expr_)
            (*it)->hasdeclare = true;
}
void StatementGenerator::SelectionExpression( std::list<FunctionBlock*>& parent, EXPRESSION** exp, Keyword kw,
                                     bool* declaration)
{
    Type* tp = nullptr;
    bool hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, true);
    (void)parent;
    bool structured = false;
    if (TypeGenerator::StartOfType(lex, &structured, false) && (!Optimizer::cparams.prm_cplusplus || StatementGenerator::ResolvesToDeclaration(lex, structured)))
    {
        if (declaration)
        {
            *declaration = true;
        }
        if ((Optimizer::cparams.prm_cplusplus && declaration) ||
            (Optimizer::cparams.c_dialect >= Dialect::c99 && (kw == Keyword::for_ || kw == Keyword::rangefor_)))
        {
            // empty
        }
        else
        {
            error(ERR_NO_DECLARATION_HERE);
        }
        /* fixme need type */
        AutoDeclare(&tp, exp, parent, (kw != Keyword::for_ && kw != Keyword::if_ && kw != Keyword::switch_) | (kw == Keyword::rangefor_ ? _F_NOCHECKAUTO : 0));
        if (tp->type == BasicType::memberptr_)
        {
            *exp = MakeExpression(ExpressionNode::mp_as_bool_, *exp);
            (*exp)->size = tp;
            tp = &stdint;
        }
        if ((kw == Keyword::if_ || kw == Keyword::switch_) && MATCHKW(lex, Keyword::semicolon_))
            RequiresDialect::Feature(Dialect::cpp17, "Initializer in if or switch statement");
    }
    else
    {
        if (hasAttributes)
            error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
        /*		bool openparen = MATCHKW(lex, Keyword::openpa_); */
        if (declaration)
            *declaration = false;
        inFunctionExpressionParsing = true;
        lex = expression(lex, funcsp, nullptr, &tp, exp, kw != Keyword::for_ && kw != Keyword::rangefor_ ? _F_SELECTOR : 0);
        if (tp)
        {
            if (tp->type == BasicType::memberptr_)
            {
                *exp = MakeExpression(ExpressionNode::mp_as_bool_, *exp);
                (*exp)->size = tp;
                tp = &stdint;
            }
            optimize_for_constants(exp);
        }
    }

    if (Optimizer::cparams.prm_cplusplus && tp && tp->IsStructured() && kw != Keyword::for_ && kw != Keyword::rangefor_ &&
        ((kw != Keyword::if_ && kw != Keyword::switch_) || !declaration || !*declaration))
    {
        if (!castToArithmeticInternal(false, &tp, exp, (Keyword) - 1, &stdbool, false))
            if (!castToArithmeticInternal(false, &tp, exp, (Keyword) - 1, &stdint, false))
                if (!castToPointer(&tp, exp, (Keyword) - 1, &stdpointer))
                    errorConversionOrCast(true, tp, &stdint);
    }
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    else if (kw == Keyword::switch_ && !tp->IsInt() && tp->BaseType()->type != BasicType::enum_ &&
             (!tp->IsAutoType() || !LookupTypeFromExpression(*exp, nullptr, false)->IsInt()))
        error(ERR_SWITCH_SELECTION_INTEGRAL);
    else if (kw != Keyword::for_ && kw != Keyword::rangefor_ && tp->IsStructured() &&
             ((kw != Keyword::if_ && kw != Keyword::switch_) || !declaration || !*declaration))

    {
        error(ERR_ILL_STRUCTURE_OPERATION);
    }
    *exp = MakeExpression(ExpressionNode::select_, *exp);
    GetLogicalDestructors(&(*exp)->v.logicaldestructors.left, *exp);
}
FunctionBlock* StatementGenerator::GetCommonParent(std::list<FunctionBlock*>& src, std::list<FunctionBlock*>& dest)
{
    FunctionBlock* rv = nullptr;
    for (auto s : src)
    {
        bool exit = false;
        auto s1 = s->orig ? s->orig : s;
        for (auto d : dest)
        {
            auto d1 = d->orig ? d->orig : d;
            if (d1 == s1)
            {
                rv = d1;
                exit = true;
                break;
            }
        }
        if (exit)
            break;
    }
    return rv;
}
void StatementGenerator::ThunkCatchCleanup(Statement* st, std::list<FunctionBlock*>& src, std::list<FunctionBlock*>& dest)
{
    FunctionBlock *top = dest.size() ? GetCommonParent(src, dest) : nullptr;
    for (auto srch : src)
    {
        if (srch == top)
            break;
        if (srch->type == Keyword::catch_)
        {
            SYMBOL* sym = namespacesearch("_CatchCleanup", globalNameSpace, false, false);
            if (sym)
            {
                CallSite* funcparams = Allocate<CallSite>();
                Argument* arg1 = Allocate<Argument>();  // exception table
                makeXCTab(funcsp);
                sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
                funcparams->ascall = true;
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = MakeExpression(ExpressionNode::pc_, sym);
                funcparams->arguments = argumentListFactory.CreateList();
                funcparams->arguments->push_back(arg1);
                arg1->exp = MakeExpression(ExpressionNode::auto_, funcsp->sb->xc->xctab);
                arg1->tp = &stdpointer;
                for (auto it = src.front()->statements->begin(); it != src.front()->statements->end(); ++it)
                {
                    if ((*it) == st)
                    {
                        auto next = Allocate<Statement>();
                        *next = *st;
                        next->type = StatementNode::expr_;
                        next->select = MakeExpression(funcparams);
                        it = src.front()->statements->insert(it, next);
                        break;
                    }
                }
            }
            break;
        }
    }
    for (auto srch : dest)
    {
        if (srch == top)
            break;
        if (srch->type == Keyword::try_ || srch->type == Keyword::catch_)
        {
            error(ERR_GOTO_INTO_TRY_OR_CATCH_BLOCK);
            break;
        }
    }
}
void StatementGenerator::DestructSymbolsInTable(SymbolTable<SYMBOL>* syms)
{
    for (auto sym : *syms)
    {
        sym->sb->destructed = true;
    }
}
EXPRESSION* StatementGenerator::DestructorsForExpression(EXPRESSION* exp)
{
    std::stack<SYMBOL*> destructList;
    std::stack<EXPRESSION*> stk;
    stk.push(exp);
    while (stk.size())
    {
        EXPRESSION* e = stk.top();
        stk.pop();
        if (!isintconst(e) && !isfloatconst(e))
        {
            if (e->left)
                stk.push(e->left);
            if (e->right)
                stk.push(e->right);
        }
        if (e->type == ExpressionNode::thisref_)
            e = e->left;
        if (e->type == ExpressionNode::callsite_)
        {
            if (e->v.func->arguments)
                for (auto il : *e->v.func->arguments)
                    stk.push(il->exp);
        }
        if (e->type == ExpressionNode::auto_ && e->v.sp->sb->allocate && !e->v.sp->sb->destructed)
        {
            Type* tp = e->v.sp->tp;
            while (tp->IsArray())
                tp = tp->BaseType()->btp;
            if (tp->IsStructured() && !tp->IsRef())
            {
                e->v.sp->sb->destructed = true;
                destructList.push(e->v.sp);
            }
        }
    }

    EXPRESSION* rv = exp;
    while (destructList.size())
    {
        SYMBOL* sp = destructList.top();
        destructList.pop();
        if (sp->sb->dest && sp->sb->dest->front()->exp)
            rv = MakeExpression(ExpressionNode::comma_, rv, sp->sb->dest->front()->exp);
    }
    return rv;
}
void StatementGenerator::DestructorsForArguments(std::list<Argument*>* il)
{
    if (Optimizer::cparams.prm_cplusplus)
        for (auto first : *il)
        {
            Type* tp = first->tp;
            if (tp)
            {
                bool ref = false;
                if (tp->IsRef())
                {
                    ref = true;
                    tp = tp->BaseType()->btp;
                }
                else if (tp->lref || tp->rref)
                {
                    ref = true;
                }
                if (ref || !tp->IsStructured())
                {
                    std::stack<EXPRESSION*> stk;

                    stk.push(first->exp);
                    while (!stk.empty())
                    {
                        auto tst = stk.top();
                        stk.pop();
                        if (tst->type == ExpressionNode::thisref_)
                            tst = tst->left;
                        if (tst->type == ExpressionNode::callsite_)
                        {
                            if (tst->v.func->sp->sb->isConstructor)
                            {
                                EXPRESSION* iexp = tst->v.func->thisptr;
                                auto sp = tst->v.func->thistp->BaseType()->btp->BaseType()->sp;
                                int offs;
                                auto xexp = relptr(iexp, offs);
                                if (xexp)
                                    xexp->v.sp->sb->destructed = true;
                                if (callDestructor(sp, nullptr, &iexp, nullptr, true, false, false, true))
                                {
                                    if (!first->destructors)
                                        first->destructors = exprListFactory.CreateList();
                                    first->destructors->push_front(iexp);
                                }
                            }
                        }
                        else if (tst->type == ExpressionNode::comma_)
                        {
                            if (tst->right)
                                stk.push(tst->right);
                            if (tst->left)
                                stk.push(tst->left);
                        }
                    }
                }
            }
        }
}
void StatementGenerator::DestructorsForBlock(EXPRESSION** exp, SymbolTable<SYMBOL>* table, bool mainDestruct)
{
    for (auto sp : *table)
    {
        if ((sp->sb->allocate || sp->sb->storage_class == StorageClass::parameter_) && !sp->sb->destructed && !sp->tp->IsRef())
        {
            sp->sb->destructed = mainDestruct;
            if (sp->sb->storage_class == StorageClass::parameter_)
            {
                if (sp->tp->IsStructured())
                {
                    EXPRESSION* iexp = getThisNode(sp);
                    if (callDestructor(sp->tp->BaseType()->sp, nullptr, &iexp, nullptr, true, false, false, true))
                    {
                        optimize_for_constants(&iexp);
                        if (*exp)
                        {
                            *exp = MakeExpression(ExpressionNode::comma_, iexp, *exp);
                        }
                        else
                        {
                            *exp = iexp;
                        }
                    }
                }
            }
            else if (sp->sb->storage_class != StorageClass::localstatic_ && sp->sb->dest)
            {

                EXPRESSION* iexp = sp->sb->dest->front()->exp;
                if (iexp)
                {
                    optimize_for_constants(&iexp);
                    if (*exp)
                    {
                        *exp = MakeExpression(ExpressionNode::comma_, iexp, *exp);
                    }
                    else
                    {
                        *exp = iexp;
                    }
                }
            }
        }
    }
}
void StatementGenerator::ThunkReturnDestructors(EXPRESSION** exp, SymbolTable<SYMBOL>* top, SymbolTable<SYMBOL>* syms)
{
    if (syms)
    {
        if (syms != top)
        {
            ThunkReturnDestructors(exp, top, syms->Chain());
            StatementGenerator::DestructorsForBlock(exp, syms, false);
        }
    }
}
void StatementGenerator::ThunkGotoDestructors(EXPRESSION** exp, std::list<FunctionBlock*>& gotoTab, std::list<FunctionBlock*>& labelTab)
{
    // find the common parent
    std::list<FunctionBlock*>::iterator realtop;
    FunctionBlock* top = GetCommonParent(gotoTab, labelTab);
    auto il = gotoTab.begin();
    if ((*il) != top && (*il)->orig != top)
    {
        realtop = il;
        while ((*il) != top && (*il)->orig != top) ++il;
        ThunkReturnDestructors(exp, (*il)->table, gotoTab.front()->table);
    }
}
void StatementGenerator::StartOfCaseGroup(std::list<FunctionBlock*>& parent)
{
    caseLevel++;
    Statement* st = Statement::MakeStatement(nullptr, parent, StatementNode::dbgblock_);
    st->label = 1;
}
void StatementGenerator::EndOfCaseGroup(std::list<FunctionBlock*>& parent)
{
    if (caseLevel)
    {
        caseLevel--;
        Statement* st = Statement::MakeStatement(nullptr, parent, StatementNode::dbgblock_);
        st->label = 0;
    }
}
void StatementGenerator::HandleStartOfCase(std::list<FunctionBlock*>& parent)
{
    // this is a little buggy in that it doesn't check to see if we are already in a switch
    // statement, however if we aren't we should get a compile erroir that would halt program generation anyway
    if (parent.front() != caseDestructorsForBlock)
    {
        parent.front()->caseDestruct = caseDestructorsForBlock;
        caseDestructorsForBlock = parent.front();
    }
}
void StatementGenerator::HandleEndOfCase(std::list<FunctionBlock*>& parent)
{
    if (parent.front() == caseDestructorsForBlock)
    {
        EXPRESSION* exp = nullptr;
        Statement* st;
        // the destruct is only used for endin
        StatementGenerator::DestructorsForBlock(&exp, localNameSpace->front()->syms, false);
        if (exp)
        {
            st = Statement::MakeStatement(nullptr, parent, StatementNode::nop_);
            st->destexp = exp;
        }
        DestructSymbolsInTable(localNameSpace->front()->syms);
    }
}
void StatementGenerator::HandleEndOfSwitchBlock(std::list<FunctionBlock*>& parent)
{
    if (parent.front() == caseDestructorsForBlock)
    {
        caseDestructorsForBlock = caseDestructorsForBlock->caseDestruct;
    }
}
void StatementGenerator::ParseBreak(std::list<FunctionBlock*>& parent)
{
    std::list<FunctionBlock*>::iterator breakableStatement, last = parent.end();
    EXPRESSION* exp = nullptr;
    (void)parent;
    bool found = false;
    for (auto it = parent.begin(); it != parent.end(); ++it)
    {
        auto b = *it;
        if (b->type != Keyword::begin_ && b->type != Keyword::try_ && b->type != Keyword::catch_ &&
            b->type != Keyword::if_ && b->type != Keyword::else_)
        {
            breakableStatement = it;
            found = true;
            break;
        }
        last = it;
    }

    if (!found)
        error(ERR_BREAK_NO_LOOP);
    else
    {
        canFallThrough = true;
        Statement* st;
        currentLineData(parent, lex, 0);
        if (last != parent.end())
            ThunkReturnDestructors(&exp, (*last)->table, localNameSpace->front()->syms);
        st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
        st->label = (*breakableStatement)->breaklabel;
        st->destexp = exp;
        auto range = std::list<FunctionBlock*>(breakableStatement, parent.end());
        ThunkCatchCleanup(st, parent, range);
        parent.front()->needlabel = true;
        (*breakableStatement)->needlabel = false;
        (*breakableStatement)->hasbreak = true;
    }
    lex = getsym();
}
void StatementGenerator::ParseCase(std::list<FunctionBlock*>& parent)
{
    long long val;
    FunctionBlock dummy;
    Type* tp = nullptr;
    EXPRESSION* exp = nullptr;
    FunctionBlock* switchstmt = nullptr;
    lex = getsym();
    bool found = false;
    for (auto s : parent)
    {
        if (s->type == Keyword::switch_)
        {
            found = true;
            switchstmt = s;
            break;
        }
    }
    if (!found)
    {
        memset(&dummy, 0, sizeof(dummy));
        switchstmt = &dummy;
        switchstmt->type = Keyword::switch_;
        error(ERR_CASE_NO_SWITCH);
    }
    else
    {
        switchstmt->needlabel = false;
        parent.front()->needlabel = false;
        if (!canFallThrough && !basisAttribs.uninheritable.fallthrough)
            error(ERR_FALLTHROUGH);
        canFallThrough = true;
    }

    lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
    if (!tp)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (isintconst(exp))
    {
        needkw(&lex, Keyword::colon_);
        const char* fname = lex->data->errfile;
        int line = lex->data->errline;
        val = exp->v.i;
        /* need error: lost conversion on case value */
        bool found = false;
        if (!switchstmt->cases)
            switchstmt->cases = casedataListFactory.CreateList();
        for (auto cases : *switchstmt->cases)
        {
            if (cases->val == val)
            {
                found = true;
                char buf[256];
                Optimizer::my_sprintf(buf, LLONG_FORMAT_SPECIFIER, val);
                preverror(ERR_DUPLICATE_CASE, buf, cases->file, cases->line);
                break;
            }
        }
        if (!found)
        {
            Statement* st = Statement::MakeStatement(lex, parent, StatementNode::label_);
            st->label = codeLabel++;
            codeLabel++;  // reserve a label in case a bingen is used in the back Keyword::end_...
            auto data = Allocate<CASEDATA>();
            data->val = val;
            data->label = st->label;
            data->file = fname;
            data->line = line;
            switchstmt->cases->push_back(data);
        }
    }
    else
    {
        error(ERR_CASE_INTEGER_CONSTANT);
        errskim(&lex, skim_colon);
        skip(&lex, Keyword::colon_);
    }
}
void StatementGenerator::ParseContinue(std::list<FunctionBlock*>& parent)
{
    std::list<FunctionBlock*>::iterator continuableStatement, last = parent.end();
    EXPRESSION* exp = nullptr;
    bool found = false;
    for (auto it = parent.begin(); it != parent.end(); ++it)
    {
        auto c = *it;
        if (c->type != Keyword::begin_ && c->type != Keyword::try_ && c->type != Keyword::catch_ &&
            c->type != Keyword::if_ && c->type != Keyword::else_ && c->type != Keyword::switch_)
        {
            continuableStatement = it;
            found = true;
            break;
        }
        last = it;
    }

    if (!found)
        error(ERR_CONTINUE_NO_LOOP);
    else
    {
        canFallThrough = true;
        Statement* st;
        if (last != parent.end())
            ThunkReturnDestructors(&exp, (*last)->table, localNameSpace->front()->syms);
        currentLineData(parent, lex, 0);
        st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
        st->label = (*continuableStatement)->continuelabel;
        st->destexp = exp;
        auto range = std::list<FunctionBlock*>(continuableStatement, parent.end());
        ThunkCatchCleanup(st, parent, range);
        parent.front()->needlabel = true;
    }
    lex = getsym();
}
void StatementGenerator::ParseDefault(std::list<FunctionBlock*>& parent)
{
    lex = getsym();
    std::list<FunctionBlock*>::iterator defaultableStatement;
    EXPRESSION* exp = nullptr;
    bool found = false;
    for (auto it = parent.begin(); it != parent.end(); ++it)
    {
        auto c = *it;
        if (c->type == Keyword::switch_)
        {
            defaultableStatement = it;
            found = true;
            break;
        }
    }

    if (!found)
        error(ERR_DEFAULT_NO_SWITCH);
    else
    {
        if (!canFallThrough && !basisAttribs.uninheritable.fallthrough)
            error(ERR_FALLTHROUGH);
        canFallThrough = true;
        Statement* st = Statement::MakeStatement(lex, parent, StatementNode::label_);
        st->label = codeLabel++;
        if ((*defaultableStatement)->defaultlabel != -1)
            error(ERR_SWITCH_HAS_DEFAULT);
        (*defaultableStatement)->defaultlabel = st->label;
        (*defaultableStatement)->needlabel = false;
        parent.front()->needlabel = false;
    }
    needkw(&lex, Keyword::colon_);
}
void StatementGenerator::ParseDo(std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    FunctionBlock* dostmt = Allocate<FunctionBlock>();
    Statement *st, *lastLabelStmt;
    EXPRESSION* select = nullptr;
    int addedBlock = 0;
    int loopLabel = codeLabel++;
    lex = getsym();
    dostmt->breaklabel = codeLabel++;
    dostmt->continuelabel = codeLabel++;
    dostmt->type = Keyword::do_;
    dostmt->table = localNameSpace->front()->syms;
    parent.push_front(dostmt);
    currentLineData(parent, lex, 0);
    st = Statement::MakeStatement(lex, parent, StatementNode::label_);
    st->label = loopLabel;
    inLoopOrConditional++;
    if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
    {
        addedBlock++;
        AllocateLocalContext(parent, funcsp, codeLabel++);
    }
    do
    {
        lastLabelStmt = dostmt->statements->back();
        StatementWithoutNonconst(parent, true);
    } while (lex && dostmt->statements->back() != lastLabelStmt && dostmt->statements->back()->purelabel);
    if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
    {
        addedBlock--;
        FreeLocalContext(parent, funcsp, codeLabel++);
    }
    before->nosemi = false;
    if (MATCHKW(lex, Keyword::while_))
    {
        lex = getsym();
        if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        if (MATCHKW(lex, Keyword::openpa_))
        {
            lex = getsym();
            SelectionExpression(parent, &select, Keyword::do_, nullptr);
            if (!MATCHKW(lex, Keyword::closepa_))
            {
                error(ERR_DOWHILE_NEEDS_CLOSEPA);
                errskim(&lex, skim_closepa);
                skip(&lex, Keyword::closepa_);
            }
            else
                lex = getsym();
            st = Statement::MakeStatement(lex, parent, StatementNode::label_);
            st->label = dostmt->continuelabel;
            currentLineData(parent, lex, 0);
            st = Statement::MakeStatement(lex, parent, StatementNode::select_);
            st->select = select;
            if (!dostmt->hasbreak && (dostmt->needlabel || IsSelectTrue(st->select)))
                before->needlabel = true;
            st->label = loopLabel;
            st = Statement::MakeStatement(lex, parent, StatementNode::label_);
            st->label = dostmt->breaklabel;
        }
        else
        {
            error(ERR_DOWHILE_NEEDS_OPENPA);
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
    }
    else
    {
        before->nosemi = true;
        error(ERR_DO_STMT_NEEDS_WHILE);
        errskim(&lex, skim_semi);
        skip(&lex, Keyword::semicolon_);
    }
    inLoopOrConditional--;
    while (addedBlock--)
        FreeLocalContext(parent, funcsp, codeLabel++);
    parent.pop_front();
    dostmt->AddThis(lex, parent);
}
void StatementGenerator::ParseFor(std::list<FunctionBlock*>& parent)
{
    FunctionBlock* before = parent.front();
    FunctionBlock* forstmt = Allocate<FunctionBlock>();
    Statement *st, *lastLabelStmt;
    std::list<Statement*>* forline;
    int addedBlock = 0;
    EXPRESSION *init = nullptr, *incrementer = nullptr, *select = nullptr;
    int loopLabel = codeLabel++, testlabel = codeLabel++;
    forstmt->breaklabel = codeLabel++;
    forstmt->continuelabel = codeLabel++;
    forstmt->type = Keyword::for_;
    forstmt->table = localNameSpace->front()->syms;
    parent.push_front(forstmt);
    auto beforeit = parent.begin();
    ++beforeit;
    currentLineData(parent, lex, -1);
    forline = currentLineData(emptyBlockdata, lex, 0);
    lex = getsym();
    inLoopOrConditional++;
    if (MATCHKW(lex, Keyword::openpa_))
    {
        bool declaration = false;
        lex = getsym();
        if (!MATCHKW(lex, Keyword::semicolon_))
        {
            bool hasColon = false;
            if ((Optimizer::cparams.prm_cplusplus && !Optimizer::cparams.prm_oldfor) || Optimizer::cparams.c_dialect >= Dialect::c99)
            {
                addedBlock++;
                std::list<FunctionBlock*> dummy(beforeit, parent.end());
                AllocateLocalContext(dummy, funcsp, codeLabel++);
            }
            if (Optimizer::cparams.prm_cplusplus)
            {
                LexList* origLex = lex;
                while (lex && !MATCHKW(lex, Keyword::semicolon_) && !MATCHKW(lex, Keyword::colon_))
                    lex = getsym();
                hasColon = MATCHKW(lex, Keyword::colon_);
                lex = prevsym(origLex);
            }

            SelectionExpression(parent, &init, hasColon ? Keyword::rangefor_ : Keyword::for_, &declaration);
            if (Optimizer::cparams.prm_cplusplus && !Optimizer::cparams.prm_oldfor && declaration && MATCHKW(lex, Keyword::colon_))
            {
                // range based for statement
                // we will ignore 'init'.
                Type* selectTP = nullptr;
                SYMBOL* declSP = (SYMBOL*)localNameSpace->front()->syms->front();
                EXPRESSION* declExp;
                if (!declSP)
                {
                    diag("statement_for: Cannot get range based range variable");
                    declExp = MakeIntExpression(ExpressionNode::c_i_, 0);
                }
                else
                {
                    if (declSP->sb->init)
                    {
                        error(ERR_FORRANGE_DECLARATOR_NO_INIT);
                    }
                    declSP->sb->dest = nullptr;
                    declExp = MakeExpression(ExpressionNode::auto_, declSP);
                    declSP->sb->assigned = declSP->sb->attribs.inheritable.used = true;
                }
                lex = getsym();

                auto page = localNameSpace->front()->syms;
                localNameSpace->front()->syms = static_cast<SymbolTable<SYMBOL>*>(localNameSpace->front()->syms->Next());
                if (MATCHKW(lex, Keyword::begin_))
                {
                    Type* matchtp = &stdint;
                    EXPRESSION *begin, *size;
                    std::deque<std::pair<Type*, EXPRESSION*>> save;
                    std::list<Argument*>* lst = nullptr;
                    lex = getInitList(lex, funcsp,&lst);
                    int offset = 0;

                    if (lst)
                    {
                        int n = lst->size();

                        matchtp = lst->front()->tp;
                        if (matchtp->IsRef())
                            matchtp = matchtp->BaseType()->btp;

                        auto valueList = Type::MakeType(BasicType::pointer_, matchtp);
                        valueList->array = true;
                        valueList->size = n * matchtp->size;

                        EXPRESSION* val = anonymousVar(StorageClass::auto_, valueList);

                        int sz = lst->front()->tp->size;

                        for (auto lstitem : *lst)
                        { 
                            Type* ittp = lstitem->tp;
                            EXPRESSION* base = MakeExpression(ExpressionNode::add_, val, MakeIntExpression(ExpressionNode::c_i_, offset));
                            offset += sz;
                            if (ittp->IsRef())
                                ittp = ittp->BaseType()->btp;
                            if (!matchtp->ExactSameType(ittp))
                            {
                                if (!matchtp->IsStructured())
                                {
                                    errorConversionOrCast(true, ittp, matchtp);
                                }
                                else
                                {
                                    Type* ctype = matchtp;
                                    EXPRESSION* newExp = base;
                                    if (!callConstructorParam(&ctype, &newExp, ittp, lstitem->exp, true, false, true, false, true))
                                    {
                                        errorConversionOrCast(true, ittp, matchtp);
                                    }
                                    else
                                    {
                                        st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                        st->select = newExp;
                                        newExp = base;
                                        callDestructor(matchtp->sp, matchtp->sp, &newExp, MakeIntExpression(ExpressionNode::c_i_, offset / sz), true, true,
                                                       false, true);
                                    }
                                }
                            }
                            else if (matchtp->IsStructured())
                            {
                                Type* ctype = matchtp;
                                EXPRESSION* newExp = base;
                                ittp->lref = true;
                                if (lstitem->exp->type == ExpressionNode::thisref_)
                                {
                                    lstitem->exp->left->v.func->thisptr->v.sp->sb->dest = nullptr;
                                    lstitem->exp->left->v.func->thisptr = base;
                                    st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                    st->select = lstitem->exp;
                                }
                                else if (!callConstructorParam(&ctype, &newExp, ittp, lstitem->exp, true, false, true, false, true))
                                {
                                    errorConversionOrCast(true, ittp, matchtp);
                                }
                                else
                                {
                                    st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                    st->select = newExp;
                                }
                                newExp = base;
                            }
                            else
                            {
                                st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                deref(matchtp, &base);
                                st->select = MakeExpression(ExpressionNode::assign_, base, lstitem->exp);
                            }
                        }
                        if (matchtp->IsStructured())
                        {
                            auto newExp = val;
                            callDestructor(matchtp->sp, matchtp->sp, &newExp, MakeIntExpression(ExpressionNode::c_i_, offset / sz), true, true, false,
                                           true);
                            initInsert(&val->v.sp->sb->dest, matchtp, newExp, 0, false);
                        }

                        begin = val;
                        size = MakeIntExpression(ExpressionNode::c_i_, offset / matchtp->size);
                    }
                    else
                    {
                        select = anonymousVar(StorageClass::auto_, &stdint);
                        begin = select;
                        size = MakeIntExpression(ExpressionNode::c_i_, 0);
                    }
                    selectTP = matchtp->InitializerListType();
                    std::list<Initializer*>* init = nullptr;
                    initInsert(&init, &stdpointer, begin, 0, false);
                    initInsert(&init, &stdpointer, size, stdpointer.size, false);
                    Type* tp2 = Type::MakeType(BasicType::pointer_, &stdpointer);
                    tp2->size = 2 * stdpointer.size;
                    tp2->array = true;
                    EXPRESSION* val = anonymousVar(StorageClass::auto_, tp2);
                    select = convertInitToExpression(tp2, nullptr, val, funcsp, init, nullptr, false);
                    st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                    st->select = select;
                    select = val;
                }
                else
                {
                    lex = expression_no_comma(lex, funcsp, nullptr, &selectTP, &select, nullptr, 0);
                }
                localNameSpace->front()->syms = page;
                if (!selectTP || selectTP->type == BasicType::any_)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                {
                    EXPRESSION *ibegin = nullptr, *iend = nullptr;
                    SYMBOL *sbegin = nullptr, *send = nullptr;
                    Type* iteratorType = nullptr;
                    Type* tpref = Type::MakeType(BasicType::rref_, selectTP);
                    EXPRESSION* rangeExp = anonymousVar(StorageClass::auto_, tpref);
                    SYMBOL* rangeSP = rangeExp->v.sp;
                    if (selectTP->IsStructured())
                        selectTP = selectTP->BaseType()->sp->tp;
                    deref(&stdpointer, &rangeExp);
                    needkw(&lex, Keyword::closepa_);
                    while (castvalue(select))
                        select = select->left;
                    if (lvalue(select) && select->type != ExpressionNode::l_ref_ && !selectTP->IsStructured())
                        select = select->left;
//                    st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
//                    st->select = MakeExpression(ExpressionNode::assign_, rangeExp, select);
                    rangeExp = select;
                    if (!selectTP->IsStructured())
                    {
                        // create array references for Keyword::begin_ and Keyword::end_
                        iteratorType = selectTP->BaseType()->btp;
                        if (!selectTP->IsArray())
                        {
                            error(ERR_FORRANGE_REQUIRES_STRUCT_OR_ARRAY);
                            iteratorType = &stdint;
                        }
                        else if (!selectTP->size)
                        {
                            error(ERR_FORRANGE_ARRAY_UNSIZED);
                        }
                        ibegin = rangeExp;
                        iend = MakeExpression(ExpressionNode::add_, rangeExp, MakeIntExpression(ExpressionNode::c_i_, selectTP->size));
                    }
                    else
                    {
                        // try to lookup in structure
                        Type thisTP = {};
                        Type::MakeType(thisTP, BasicType::pointer_, rangeSP->tp->btp);
                        sbegin = search(selectTP->BaseType()->syms, "begin");
                        send = search(selectTP->BaseType()->syms, "end");
                        if (sbegin && send)
                        {
                            SYMBOL *beginFunc = nullptr, *endFunc = nullptr;
                            CallSite fcb, fce;
                            Type* ctp;
                            memset(&fcb, 0, sizeof(fcb));
                            fcb.thistp = &thisTP;
                            fcb.thisptr = rangeExp;
                            fcb.ascall = true;
                            ctp = rangeSP->tp;
                            beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, sbegin, &fcb, nullptr, false, false, 0);
                            memset(&fce, 0, sizeof(fce));
                            fce.thistp = &thisTP;
                            fce.thisptr = rangeExp;
                            fce.ascall = true;
                            ctp = rangeSP->tp;
                            endFunc = GetOverloadedFunction(&ctp, &fce.fcall, send, &fce, nullptr, false, false, 0);
                            if (beginFunc && endFunc)
                            {
                                if (!beginFunc->tp->BaseType()->btp->ExactSameType(endFunc->tp->BaseType()->btp))
                                {
                                    error(ERR_MISMATCHED_FORRANGE_BEGIN_END_TYPES);
                                }
                                else
                                {
                                    CallSite* fc;
                                    iteratorType = beginFunc->tp->BaseType()->btp;
                                    if (iteratorType->IsStructured())
                                    {
                                        std::list<Initializer*>* dest = nullptr;
                                        EXPRESSION* exp;
                                        fcb.returnEXP = anonymousVar(StorageClass::auto_, iteratorType);
                                        fcb.returnEXP->v.sp->sb->anonymous = false;
                                        fcb.returnSP = fcb.returnEXP->v.sp;
                                        exp = fcb.returnEXP;
                                        dest = nullptr;
                                        callDestructor(fcb.returnSP->tp->BaseType()->sp, nullptr, &exp, nullptr, true, true, false, true);
                                        initInsert(&dest, iteratorType, exp, 0, true);
                                        fcb.returnSP->sb->dest = dest;

                                        dest = nullptr;
                                        fce.returnEXP = anonymousVar(StorageClass::auto_, iteratorType);
                                        fce.returnEXP->v.sp->sb->anonymous = false;
                                        fce.returnSP = fcb.returnEXP->v.sp;
                                        exp = fce.returnEXP;
                                        dest = nullptr;
                                        callDestructor(fce.returnSP->tp->BaseType()->sp, nullptr, &exp, nullptr, true, true, false, true);
                                        initInsert(&dest, iteratorType, exp, 0, true);
                                        fce.returnSP->sb->dest = dest;
                                    }
                                    fc = Allocate<CallSite>();
                                    *fc = fcb;
                                    fc->sp = beginFunc;
                                    fc->functp = beginFunc->tp;
                                    fc->ascall = true;
                                    ibegin = MakeExpression(fc);
                                    fc = Allocate<CallSite>();
                                    *fc = fce;
                                    fc->sp = endFunc;
                                    fc->functp = endFunc->tp;
                                    fc->ascall = true;
                                    if (fc->thisptr)
                                    {
                                        auto expx = fc->thisptr;
                                        while(lvalue(expx) || castvalue(expx)) expx = expx->left;
                                        if (expx->type == ExpressionNode::thisref_)
                                            expx = expx->left;
                                        if (expx->type == ExpressionNode::callsite_)
                                            if (expx->v.func->returnEXP)
                                                fc->thisptr = expx->v.func->returnEXP;
                                    }
                                    iend = MakeExpression(fc);
                                    iteratorType = beginFunc->tp->BaseType()->btp;
                                }
                            }
                            else
                            {
                                ibegin = iend = nullptr;
                            }
                        }
                        // possibly lookup in search area
                        if (!ibegin && !iend)
                        {
                            sbegin = namespacesearch("begin", globalNameSpace, false, false);
                            send = namespacesearch("end", globalNameSpace, false, false);
                            // now possibly lookup in namespace std
                            if (!sbegin || !send)
                            {
                                SYMBOL* standard = namespacesearch("std", globalNameSpace, false, false);
                                if (standard)
                                {
                                    sbegin = namespacesearch("begin", standard->sb->nameSpaceValues, false, false);
                                    send = namespacesearch("end", standard->sb->nameSpaceValues, false, false);
                                }
                            }
                            if (!sbegin || !send)
                            {
                                if (rangeSP->tp->btp->sp->sb->parentNameSpace)
                                {
                                    sbegin = namespacesearch(
                                        "begin", rangeSP->tp->btp->sp->sb->parentNameSpace->sb->nameSpaceValues, false, false);
                                    send = namespacesearch("end", rangeSP->tp->btp->sp->sb->parentNameSpace->sb->nameSpaceValues,
                                                           false, false);
                                }
                            }
                            {
                                SYMBOL *beginFunc = nullptr, *endFunc = nullptr;
                                Argument args;
                                CallSite fcb, fce;
                                Type* ctp;
                                memset(&fcb, 0, sizeof(fcb));
                                memset(&args, 0, sizeof(args));
                                args.tp = rangeSP->tp->btp;
                                args.exp = rangeExp;
                                fcb.arguments = argumentListFactory.CreateList();
                                fcb.arguments->push_back(&args);
                                fcb.ascall = true;
                                ctp = rangeSP->tp;
                                beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, sbegin, &fcb, nullptr, false, false, 0);
                                memset(&fce, 0, sizeof(fce));
                                fce.arguments = argumentListFactory.CreateList();
                                fce.arguments->push_back(&args);
                                fce.ascall = true;
                                ctp = rangeSP->tp;
                                endFunc = GetOverloadedFunction(&ctp, &fce.fcall, send, &fce, nullptr, false, false, 0);
                                if (beginFunc && endFunc)
                                {
                                    Type* it2;
                                    it2 = iteratorType = beginFunc->tp->BaseType()->btp;
                                    if (it2->IsRef())
                                        it2 = it2->btp;
                                    if (!beginFunc->tp->BaseType()->btp->ExactSameType(endFunc->tp->BaseType()->btp))
                                    {
                                        error(ERR_MISMATCHED_FORRANGE_BEGIN_END_TYPES);
                                    }
                                    else
                                    {
                                        CallSite* fc;
                                        if (iteratorType->IsStructured())
                                        {
                                            std::list<Initializer*>* dest = nullptr;
                                            EXPRESSION* exp;
                                            fcb.returnEXP = anonymousVar(StorageClass::auto_, iteratorType);
                                            fcb.returnSP = fcb.returnEXP->v.sp;
                                            exp = fcb.returnEXP;
                                            dest = nullptr;
                                            callDestructor(fcb.returnSP, nullptr, &exp, nullptr, true, true, false, true);
                                            initInsert(&dest, iteratorType, exp, 0, true);
                                            fcb.returnSP->sb->dest = dest;

                                            dest = nullptr;
                                            fce.returnEXP = anonymousVar(StorageClass::auto_, iteratorType);
                                            fce.returnSP = fcb.returnEXP->v.sp;
                                            exp = fce.returnEXP;
                                            dest = nullptr;
                                            callDestructor(fce.returnSP, nullptr, &exp, nullptr, true, true, false, true);
                                            initInsert(&dest, iteratorType, exp, 0, true);
                                            fce.returnSP->sb->dest = dest;
                                        }
                                        fc = Allocate<CallSite>();
                                        *fc = fcb;
                                        fc->sp = beginFunc;
                                        fc->functp = beginFunc->tp;
                                        fc->ascall = true;
                                        fc->arguments = argumentListFactory.CreateList();
                                        for (auto i : *fcb.arguments)
                                            fc->arguments->push_back(i);
                                        if (it2->IsStructured() && ((SYMBOL*)(it2->syms->front()))->tp->IsStructured())
                                        {
                                            EXPRESSION* consexp =
                                                anonymousVar(StorageClass::auto_, rangeSP->tp->BaseType()->btp);  // StorageClass::parameter_ to push it...
                                            SYMBOL* esp = consexp->v.sp;
                                            CallSite* funcparams = Allocate<CallSite>();
                                            Type* ctype = rangeSP->tp->BaseType()->btp;
                                            esp->sb->stackblock = true;
                                            funcparams->arguments = argumentListFactory.CreateList();
                                            for (auto i : *fc->arguments)
                                                funcparams->arguments->push_back(i);
                                            callConstructor(&ctype, &consexp, funcparams, false, 0, true, false, false, false,
                                                            false, false, true);
                                            fc->arguments->front()->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->front()->tp = Type::MakeType(BasicType::lref_, fcb.arguments->front()->tp);
                                        }
                                        ibegin = MakeExpression(fc);
                                        fc = Allocate<CallSite>();
                                        *fc = fce;
                                        fc->sp = endFunc;
                                        fc->functp = endFunc->tp;
                                        fc->ascall = true;
                                        fc->arguments = argumentListFactory.CreateList();
                                        for (auto i : *fce.arguments)
                                            fc->arguments->push_back(i);
                                        if (it2->IsStructured() && ((SYMBOL*)(it2->syms->front()))->tp->IsStructured())
                                        {
                                            EXPRESSION* consexp =
                                                anonymousVar(StorageClass::auto_, rangeSP->tp->BaseType()->btp);  // StorageClass::parameter_ to push it...
                                            SYMBOL* esp = consexp->v.sp;
                                            CallSite* funcparams = Allocate<CallSite>();
                                            Type* ctype = rangeSP->tp->BaseType()->btp;
                                            esp->sb->stackblock = true;
                                            funcparams->arguments = argumentListFactory.CreateList();
                                            for (auto i : *fc->arguments)
                                                funcparams->arguments->push_back(i);
                                            callConstructor(&ctype, &consexp, funcparams, false, 0, true, false, false, false,
                                                            false, false, true);
                                            fc->arguments->front()->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->front()->tp = Type::MakeType(BasicType::lref_, fce.arguments->front()->tp);
                                        }
                                        iend = MakeExpression(fc);
                                    }
                                }
                                else
                                {
                                    ibegin = iend = nullptr;
                                }
                            }
                        }
                    }
                    if (ibegin && iend)
                    {
                        EXPRESSION* compare;
                        EXPRESSION* eBegin;
                        EXPRESSION* eEnd;
                        EXPRESSION* declDest = nullptr;
                        if (selectTP->IsStructured() && iteratorType->IsStructured() && ibegin->type == ExpressionNode::callsite_ &&
                            iend->type == ExpressionNode::callsite_)
                        {       
                            eBegin = ibegin->v.func->returnEXP;
                            eEnd = iend->v.func->returnEXP;
                            if (iteratorType->BaseType()->sp->sb->structuredAliasType)
                            {
                                deref(iteratorType->BaseType()->sp->sb->structuredAliasType, &ibegin->v.func->returnEXP);
                                deref(iteratorType->BaseType()->sp->sb->structuredAliasType, &iend->v.func->returnEXP);
                                st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                st->select = MakeExpression(ExpressionNode::assign_, ibegin->v.func->returnEXP, ibegin);
                                ibegin->v.func->returnEXP = nullptr;
                                ibegin->v.func->returnSP = nullptr;
                                st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                st->select = MakeExpression(ExpressionNode::assign_, iend->v.func->returnEXP, iend);
                                iend->v.func->returnEXP = nullptr;
                                iend->v.func->returnSP = nullptr;

                            }
                            else
                            {
                                st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                st->select = ibegin;
                                st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                st->select = iend;
                            }
                        }
                        else
                        {
                            Type* tpx = Type::MakeType(BasicType::pointer_, iteratorType);
                            eBegin = anonymousVar(StorageClass::auto_, tpx);
                            eEnd = anonymousVar(StorageClass::auto_, tpx);
                            deref(&stdpointer, &eBegin);
                            deref(&stdpointer, &eEnd);
                            st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                            st->select = MakeExpression(ExpressionNode::assign_, eBegin, ibegin);
                            st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                            st->select = MakeExpression(ExpressionNode::assign_, eEnd, iend);
                        }
                        if (iteratorType->IsRef())
                            iteratorType = iteratorType->btp;
                        if (!selectTP->IsStructured() || !iteratorType->IsStructured())
                        {
                            compare = MakeExpression(ExpressionNode::eq_, eBegin, eEnd);
                        }
                        else
                        {
                            iteratorType = iteratorType->BaseType();
                            Type* eqType = iteratorType;
                            compare = eBegin;
                            if (!insertOperatorFunc(ovcl_unary_prefix, Keyword::eq_, funcsp, &eqType, &compare, iteratorType, eEnd, nullptr,
                                                    0))
                            {
                                error(ERR_MISSING_OPERATOR_EQ_FORRANGE_ITERATOR);
                            }
                        }

                        st = Statement::MakeStatement(lex, parent, StatementNode::select_);
                        st->label = parent.front()->breaklabel;
                        st->altlabel = testlabel;
                        st->select = compare;

                        st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                        st->label = loopLabel;

                        std::list<FunctionBlock*> dummy(beforeit, parent.end());
                        AllocateLocalContext(dummy, funcsp, codeLabel++);

                        // initialize var here
                        st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                        if (!selectTP->IsStructured())
                        {
                            DeduceAuto(&declSP->tp, selectTP, declExp);
                            if (selectTP->IsPtr() && declSP->tp->IsPtr())
                                declSP->tp = declSP->tp->BaseType()->btp;
                            declSP->tp->UpdateRootTypes();
                            selectTP->UpdateRootTypes();
                            if (selectTP->IsArray())
                            {
                                Type* tp = declSP->tp;
                                if (tp->IsRef())
                                {
                                    tp = tp->BaseType()->btp;
                                    if (!tp->btp->ExactSameType(selectTP->BaseType()->btp))
                                    {
                                        error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                    }
                                }
                                else if (!tp->ExactSameType(selectTP->BaseType()->btp))
                                {
                                    error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                }
                            }
                            if (declSP->tp->IsStructured())
                            {
                                EXPRESSION* decl = declExp;
                                Type* ctype = declSP->tp;
                                CallSite* funcparams = Allocate<CallSite>();
                                Argument* args = Allocate<Argument>();
                                funcparams->arguments = argumentListFactory.CreateList();
                                funcparams->arguments->push_back(Allocate<Argument>());
                                funcparams->arguments->front()->tp = declSP->tp;
                                funcparams->arguments->front()->exp = eBegin;
                                callConstructor(&ctype, &decl, funcparams, false, 0, true, false, false, false, false, false, true);
                                st->select = decl;
                                declDest = declExp;
                                callDestructor(declSP->tp->BaseType()->sp, nullptr, &declDest, nullptr, true, true, false, true);
                            }
                            else if (selectTP->IsArray())
                            {
                                EXPRESSION* decl = declExp;
                                deref(declSP->tp, &decl);
                                st->select = eBegin;
                                deref(selectTP->BaseType()->btp, &st->select);
                                st->select = MakeExpression(ExpressionNode::assign_, decl, st->select);
                                if (declSP->tp->IsRef())
                                {
                                    declSP->tp = declSP->tp->BaseType()->btp;
                                    if (selectTP->IsPtr() && declSP->tp->IsPtr())
                                        declSP->tp = declSP->tp->BaseType()->btp;
                                }
                            }
                        }
                        else
                        {
                            Type* starType = iteratorType;
                            st->select = eBegin;
                            if (iteratorType->IsPtr())
                            {
                                DeduceAuto(&declSP->tp, iteratorType->BaseType()->btp, declExp);
                                declSP->tp->UpdateRootTypes();
                                if (!declSP->tp->ExactSameType(iteratorType->BaseType()->btp))
                                {
                                    error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                }
                                else if (!declSP->tp->IsStructured())
                                {
                                    EXPRESSION* decl = declExp;
                                    deref(declSP->tp, &decl);
                                    st->select = eBegin;
                                    if (!declSP->tp->IsRef())
                                        deref(iteratorType->BaseType()->btp, &st->select);
                                    st->select = MakeExpression(ExpressionNode::assign_, decl, st->select);
                                }
                                else
                                {
                                    EXPRESSION* decl = declExp;
                                    Type* ctype = declSP->tp;
                                    CallSite* funcparams = Allocate<CallSite>();
                                    Argument* args = Allocate<Argument>();
                                    funcparams->arguments = argumentListFactory.CreateList();
                                    funcparams->arguments->push_back(Allocate<Argument>());
                                    funcparams->arguments->front()->tp = declSP->tp;
                                    funcparams->arguments->front()->exp = eBegin;
                                    callConstructor(&ctype, &decl, funcparams, false, 0, true, false, false, false, false, false,
                                                    true);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(declSP->tp->BaseType()->sp, nullptr, &declDest, nullptr, true, true, false, true);
                                }
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, Keyword::star_, funcsp, &starType, &st->select, nullptr, nullptr,
                                                         nullptr, 0))
                            {
                                error(ERR_MISSING_OPERATOR_STAR_FORRANGE_ITERATOR);
                            }
                            else
                            {
                                bool ref = false;
                                if (declSP->tp->IsRef())
                                {
                                    ref = true;
                                }
                                DeduceAuto(&declSP->tp, starType, declExp);
                                declSP->tp->UpdateRootTypes();
                                if (!declSP->tp->ExactSameType(starType) &&
                                    (!declSP->tp->IsArithmetic() || !starType->IsArithmetic()))
                                {
                                    error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                }
                                else if (!declSP->tp->IsStructured())
                                {
                                    EXPRESSION* decl = declExp;
                                    if (ref && (starType->lref || starType->rref))
                                    {
                                        while (castvalue(st->select))
                                            st->select = st->select->left;
                                        if (lvalue(st->select))
                                            st->select = st->select->left;
                                    }
                                    deref(declSP->tp, &decl);

                                    st->select = MakeExpression(ExpressionNode::assign_, decl, st->select);
                                }
                                else
                                {
                                    EXPRESSION* decl = declExp;
                                    Type* ctype = declSP->tp;
                                    CallSite* funcparams = Allocate<CallSite>();
                                    Argument* args = Allocate<Argument>();
                                    funcparams->arguments = argumentListFactory.CreateList();
                                    funcparams->arguments->push_back(Allocate<Argument>());
                                    funcparams->arguments->front()->tp = declSP->tp;
                                    funcparams->arguments->front()->exp = st->select;
                                    callConstructor(&ctype, &decl, funcparams, false, 0, true, false, false, false, false, false,
                                                    true);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(declSP->tp->BaseType()->sp, nullptr, &declDest, nullptr, true, true, false, true);
                                }
                            }
                        }
                        do
                        {
                            lastLabelStmt = parent.front()->statements->back();
                            StatementWithoutNonconst(parent, true);
                        } while (lex && parent.front()->statements->back() != lastLabelStmt && parent.front()->statements->back()->purelabel);
                        FreeLocalContext(parent, funcsp, codeLabel++);
                        if (declDest)
                        {
                            st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                            st->select = declDest;
                        }
                        st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                        st->label = parent.front()->continuelabel;
                        st = Statement::MakeStatement(lex, parent, StatementNode::expr_);

                        // do ++ here
                        if (!selectTP->IsStructured())
                        {
                            if (selectTP->IsArray())
                                st->select =
                                    MakeExpression(ExpressionNode::assign_, eBegin,
                                             MakeExpression(ExpressionNode::add_, eBegin, MakeIntExpression(ExpressionNode::c_i_, selectTP->BaseType()->btp->BaseType()->size)));
                        }
                        else
                        {
                            Type* ppType = iteratorType;
                            ppType = ppType->BaseType();
                            st->select = eBegin;
                            if (iteratorType->IsPtr())
                            {
                                st->select = MakeExpression(
                                    ExpressionNode::assign_, eBegin,
                                    MakeExpression(ExpressionNode::add_, eBegin, MakeIntExpression(ExpressionNode::c_i_, iteratorType->BaseType()->btp->BaseType()->size)));
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, Keyword::autoinc_, funcsp, &ppType, &st->select, nullptr, nullptr,
                                                         nullptr, 0))
                            {
                                error(ERR_MISSING_OPERATOR_PLUSPLUS_FORRANGE_ITERATOR);
                            }
                            else
                            {
                                if (ppType->IsStructured())
                                {
                                    st->select->v.func->returnEXP = anonymousVar(StorageClass::auto_, ppType);
                                    st->select->v.func->returnSP = st->select->v.func->returnEXP->v.sp;
                                    declDest = st->select->v.func->returnEXP;
                                    callDestructor(st->select->v.func->returnSP->tp->BaseType()->sp, nullptr, &declDest, nullptr, true, true, false,
                                                   true);
                                    st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                                    st->select = declDest;
                                }
                            }
                        }

                        if (forline)
                        {
                            parent.front()->statements->insert(parent.front()->statements->end(), forline->begin(), forline->end());
                        }
                        st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                        st->label = testlabel;

                        st = Statement::MakeStatement(lex, parent, StatementNode::notselect_);
                        st->label = loopLabel;
                        st->select = compare;

                        if (!parent.front()->hasbreak && (!st->select || IsSelectFalse(st->select)))
                            before->needlabel = true;
                        st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                        st->label = parent.front()->breaklabel;
                        before->hassemi = parent.front()->hassemi;
                        before->nosemi = parent.front()->nosemi;
                    }
                    else
                    {
                        error(ERR_MISSING_FORRANGE_BEGIN_END);
                    }
                }
                while (addedBlock--)
                    FreeLocalContext(parent, funcsp, codeLabel++);
                parent.pop_front();
                forstmt->AddThis(lex, parent);
                return;
            }
            else
            {
                if (declaration && Optimizer::cparams.prm_cplusplus)
                {
                    auto it = localNameSpace->front()->syms->begin();
                    auto ite = localNameSpace->front()->syms->end();
                    while (it != ite && (*it)->sb->anonymous)
                        ++it;
                    if (it == ite)
                    {
                        error(ERR_FOR_DECLARATOR_MUST_INITIALIZE);
                    }
                    else
                    {
                        SYMBOL* declSP = *it;
                        if (!declSP->sb->init)
                        {
                            if (declSP->tp->IsStructured() && !declSP->tp->BaseType()->sp->sb->trivialCons)
                            {
                                lex = initialize(lex, funcsp, declSP, StorageClass::auto_, false, false, 0);
                            }
                            else
                            {
                                error(ERR_FOR_DECLARATOR_MUST_INITIALIZE);
                            }
                        }
                    }
                }
            }
        }
        // normal FOR statement continues here
        if (!needkw(&lex, Keyword::semicolon_))
        {
            error(ERR_FOR_NEEDS_SEMI);
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
        else
        {
            if (!MATCHKW(lex, Keyword::semicolon_))
            {
                Type* tp = nullptr;
                lex = optimized_expression(lex, funcsp, nullptr, &tp, &select, true);
                if (!tp)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
            }
            if (!needkw(&lex, Keyword::semicolon_))
            {
                error(ERR_FOR_NEEDS_SEMI);
                errskim(&lex, skim_closepa);
                skip(&lex, Keyword::closepa_);
            }
            else
            {
                if (!MATCHKW(lex, Keyword::closepa_))
                {
                    Type* tp = nullptr;
                    lex = expression_comma(lex, funcsp, nullptr, &tp, &incrementer, nullptr, 0);
                    if (!tp)
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    else
                    {
                        optimize_for_constants(&incrementer);
                    }
                }
                if (!MATCHKW(lex, Keyword::closepa_))
                {
                    error(ERR_FOR_NEEDS_CLOSEPA);
                    errskim(&lex, skim_closepa);
                    skip(&lex, Keyword::closepa_);
                }
                else
                {
                    lex = getsym();
                    if (init)
                    {
                        st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                        st->select = init;
                    }
                    if (Optimizer::cparams.prm_debug || Optimizer::cparams.prm_optimize_for_size ||
                        (Optimizer::chosenAssembler->arch->denyopts & DO_NOENTRYIF))
                    {
                        st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
                        st->label = testlabel;
                    }
                    else
                    {
                        st = Statement::MakeStatement(lex, parent, StatementNode::notselect_);
                        st->label = parent.front()->breaklabel;
                        st->altlabel = testlabel;
                        st->select = select;
                    }
                    st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                    st->label = loopLabel;
                    if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                    {
                        addedBlock++;
                        AllocateLocalContext(parent, funcsp, codeLabel++);
                    }
                    do
                    {
                        lastLabelStmt = parent.front()->statements->back();
                        StatementWithoutNonconst(parent, true);
                    } while (lex && parent.front()->statements->back() != lastLabelStmt && parent.front()->statements->back()->purelabel);
                    if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                    {
                        addedBlock--;
                        FreeLocalContext(parent, funcsp, codeLabel++);
                    }
                    if (before)
                        assignmentUsages(incrementer, false);
                    st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                    st->label = parent.front()->continuelabel;
                    st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                    st->select = incrementer;
                    if (forline)
                    {
                        parent.front()->statements->insert(parent.front()->statements->end(), forline->begin(), forline->end());
                    }
                    st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                    st->label = testlabel;
                    if (select)
                    {
                        st = Statement::MakeStatement(lex, parent, StatementNode::select_);
                        st->label = loopLabel;
                        st->select = select;
                    }
                    else
                    {
                        st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
                        st->label = loopLabel;
                    }
                    if (!parent.front()->hasbreak && (!st->select || IsSelectFalse(st->select)))
                        before->needlabel = true;
                    st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                    st->label = parent.front()->breaklabel;
                    before->hassemi = parent.front()->hassemi;
                    before->nosemi = parent.front()->nosemi;
                }
            }
        }
    }
    else
    {
        error(ERR_FOR_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, Keyword::closepa_);
    }
    inLoopOrConditional--;
    while (addedBlock--)
        FreeLocalContext(parent, funcsp, codeLabel++);
    parent.pop_front();
    forstmt->AddThis(lex, parent);
}
void StatementGenerator::ParseIf(std::list<FunctionBlock*>& parent)
{
    Statement *st, *lastLabelStmt;
    EXPRESSION* select = nullptr, *init = nullptr;
    int addedBlock = 0;
    bool needlabelif;
    bool needlabelelse = false;
    bool isconstexpr = false;
    int ifbranch = codeLabel++;
    lex = getsym();
    inLoopOrConditional++;
    if (Optimizer::cparams.prm_cplusplus && MATCHKW(lex, Keyword::constexpr_))
    {
        RequiresDialect::Feature(Dialect::cpp17, "Compile-time static if");
        isconstexpr = true;
        lex = getsym();
    }
    if (MATCHKW(lex, Keyword::openpa_))
    {
        lex = getsym();
        if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        bool declaration = false;
        SelectionExpression(parent, &select, Keyword::if_, &declaration);
        if (declaration && MATCHKW(lex, Keyword::semicolon_))
        {
            lex = getsym();
            init = select;
            SelectionExpression(parent, &select, Keyword::if_, nullptr);
        }
        if (MATCHKW(lex, Keyword::closepa_))
        {
            if (isconstexpr && !IsConstantExpression(select, false, true))
                error(ERR_CONSTANT_VALUE_EXPECTED);
            bool optimized = false;
            std::list<Statement*>::iterator sti;
            currentLineData(parent, lex, 0);
            lex = getsym();
            if (init)
            {
                st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                st->select = init;
            }
            st = Statement::MakeStatement(lex, parent, StatementNode::notselect_);
            st->label = ifbranch;
            st->select = select;
            sti = parent.front()->statements->end();
            --sti;
            parent.front()->needlabel = false;
            if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            do
            {
                lastLabelStmt = parent.front()->statements->back();
                StatementWithoutNonconst(parent, true);
            } while (lex && parent.front()->statements->back() != lastLabelStmt && parent.front()->statements->back()->purelabel);
            needlabelif = parent.front()->needlabel;
            if (MATCHKW(lex, Keyword::else_))
            {
                int elsebr = codeLabel++;
                if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                }
                st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
                st->label = elsebr;
                if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                {
                    AllocateLocalContext(parent, funcsp, codeLabel++);
                }
                if (Optimizer::cparams.prm_optimize_for_speed || Optimizer::cparams.prm_optimize_for_size)
                {
                    auto st2 = sti;
                    auto st1 = sti;
                    ++st1;
                    for  ( ; st1 != parent.front()->statements->end() && (*st1)->type == StatementNode::line_; ++ st1)
                    {
                        st2 = st1;
                    }
                    if  (st1 != parent.front()->statements->end() && (*st1)->type == StatementNode::goto_)
                    {
                        optimized = true;
                        parent.front()->statements->back() = *st2;
                        (*sti)->type = StatementNode::select_;
                        (*sti)->label = (*st1)->label;
                    }
                }
                st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                st->label = ifbranch;
                if (!parent.front()->nosemi && !parent.front()->hassemi)
                    errorint(ERR_NEEDY, ';');
                if (parent.front()->nosemi && parent.front()->hassemi)
                    error(ERR_MISPLACED_ELSE);
                currentLineData(parent, lex, 0);
                lex = getsym();
                parent.front()->needlabel = false;
                do
                {
                    lastLabelStmt = parent.front()->statements->back();
                    StatementWithoutNonconst(parent, true);
                } while (lex && parent.front()->statements->back() != lastLabelStmt && parent.front()->statements->back()->purelabel);
                if ((Optimizer::cparams.prm_optimize_for_speed || Optimizer::cparams.prm_optimize_for_size) && !optimized)
                {
                    auto st2 = sti;
                    auto st1 = sti;
                    ++st1;
                    for ( ; st1 != parent.front()->statements->end() && (*st1)->type == StatementNode::line_; ++st1)
                    {
                        st2 = st1;
                    }
                    if (st1 != parent.front()->statements->end() && (*st1)->type == StatementNode::goto_)
                    {
                        //						optimized = true;
                        parent.front()->statements->back() = *st2;
                        (*sti)->type = StatementNode::select_;
                        (*sti)->label = (*st1)->label;
                    }
                }
                needlabelelse = parent.front()->needlabel;
                if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                    addedBlock--;
                }
                st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                st->label = elsebr;
            }
            else
            {
                if (Optimizer::cparams.prm_optimize_for_speed || Optimizer::cparams.prm_optimize_for_size)
                {
                    auto st2 = sti;
                    auto st1 = sti;
                    ++st1;
                    for ( ; st1 != parent.front()->statements->end() && (*st1)->type == StatementNode::line_; ++st1)
                    {
                        st2 = st1;
                    }
                    if (st1 != parent.front()->statements->end() && (*st1)->type == StatementNode::goto_)
                    {
                        //						optimized = true;
                        parent.front()->statements->back() = *st2;
                        (*sti)->type = StatementNode::select_;
                        (*sti)->label = (*st1)->label;
                    }
                }
                if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                    addedBlock--;
                }
                st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                st->label = ifbranch;
            }
            if (parent.front()->hassemi)
                parent.front()->nosemi = true;
            parent.front()->needlabel = (needlabelif && needlabelelse);
        }
        else
        {
            error(ERR_IF_NEEDS_CLOSEPA);
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
    }
    else
    {
        error(ERR_IF_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, Keyword::closepa_);
    }
    inLoopOrConditional--;
    while (addedBlock--)
        FreeLocalContext(parent, funcsp, codeLabel++);
}
int StatementGenerator::GetLabelValue(LexList* lex, std::list<FunctionBlock*>* parent, Statement* st)
{
    SYMBOL* spx = search(labelSyms, lex->data->value.s.a);
    if (!spx)
    {
        spx = makeID(StorageClass::ulabel_, nullptr, nullptr, litlate(lex->data->value.s.a));
        spx->sb->declfile = spx->sb->origdeclfile = lex->data->errfile;
        spx->sb->declline = spx->sb->origdeclline = lex->data->errline;
        spx->sb->realdeclline = lex->data->linedata->lineno;
        spx->sb->declfilenum = lex->data->linedata->fileindex;
        SetLinkerNames(spx, Linkage::none_);
        spx->sb->offset = codeLabel++;
        if (!spx->sb->gotoTable)
        {
            spx->sb->gotoTable = stmtListFactory.CreateList();
            spx->sb->gotoBlockTable = blockDataListFactory.CreateList();
            if (parent)
            {
                for (auto b : *parent)
                {
                    auto x = Allocate<FunctionBlock>();
                    *x = *b;
                    x->orig = b;
                    spx->sb->gotoBlockTable->push_back(x);
                }
            }
        }
        if (st)
            spx->sb->gotoTable->push_back(st);
        labelSyms->Add(spx);
    }
    return spx->sb->offset;
}
void StatementGenerator::ParseGoto(std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    (void)parent;
    lex = getsym();
    currentLineData(parent, lex, 0);
    if (ISID(lex))
    {
        // standard c/c++ goto
        SYMBOL* spx = search(labelSyms, lex->data->value.s.a);
        FunctionBlock* block = Allocate<FunctionBlock>();
        parent.push_front(block);
        block->type = Keyword::begin_;
        block->table = localNameSpace->front()->syms;
        Statement* st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
        st->explicitGoto = true;
        int lbl = 0;
        if (!spx)
        {
            lbl = StatementGenerator::GetLabelValue(lex, &parent, st);
        }
        else
        {
            lbl = spx->sb->offset;
            ThunkGotoDestructors(&st->destexp, parent, *spx->sb->gotoBlockTable);
            ThunkCatchCleanup(st, parent, *spx->sb->gotoBlockTable);
        }
        st->label = lbl;
        lex = getsym();
        before->needlabel = true;
        parent.pop_front();
        block->AddThis(lex, parent);
        canFallThrough = true;
    }
    else if (MATCHKW(lex, Keyword::star_))
    {
        // extension: computed goto
        FunctionBlock* block = Allocate<FunctionBlock>();
        Statement* st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
        parent.push_front(block);
        block->type = Keyword::begin_;
        block->table = localNameSpace->front()->syms;
        st->explicitGoto = true;
        st->indirectGoto = true;
        Optimizer::functionHasAssembly = true; // don't optimize
        // turn off optimizations
        lex = getsym();
        Type*tp = nullptr;
        EXPRESSION* exp = nullptr;
        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
        if (!tp)
            error(ERR_IDENTIFIER_EXPECTED);
        else if (!tp->IsPtr())
            error(ERR_INVALID_POINTER_CONVERSION);
        else
        {
            st->select = exp;
            block->AddThis(lex, parent);
        }
        before->needlabel = true;
        parent.pop_front();
        canFallThrough = true;
    }
    else
    {
        error(ERR_GOTO_NEEDS_LABEL);
        errskim(&lex, skim_semi);
        skip(&lex, Keyword::semicolon_);
    }
    if (funcsp->sb->constexpression)
        error(ERR_CONSTEXPR_FUNC_NO_GOTO);
}
void StatementGenerator::ParseLabel(std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    SYMBOL* spx = search(labelSyms, lex->data->value.s.a);
    Statement* st;
    st = Statement::MakeStatement(lex, parent, StatementNode::label_);
    if (spx)
    {
        if (spx->sb->storage_class == StorageClass::ulabel_){
            spx->sb->storage_class = StorageClass::label_;
            // may come here from assembly language...
            if (spx->sb->gotoTable && spx->sb->gotoTable->size())
            {
                ThunkGotoDestructors(&spx->sb->gotoTable->front()->destexp, *spx->sb->gotoBlockTable, parent);
                ThunkCatchCleanup(spx->sb->gotoTable->front(), *spx->sb->gotoBlockTable, parent);
            }
        }
        else
        {
            errorsym(ERR_DUPLICATE_LABEL, spx);
        }
    }
    else
    {
        spx = makeID(StorageClass::label_, nullptr, nullptr, litlate(lex->data->value.s.a));
        SetLinkerNames(spx, Linkage::none_);
        spx->sb->offset = codeLabel++;
        if (!spx->sb->gotoTable)
        {
            spx->sb->gotoTable = stmtListFactory.CreateList();
            spx->sb->gotoBlockTable = blockDataListFactory.CreateList();
            for (auto b : parent)
            {
                auto x = Allocate<FunctionBlock>();
                *x = *b;
                x->orig = b;
                spx->sb->gotoBlockTable->push_back(x);
            }
        }
        spx->sb->gotoTable->push_back(st);
        labelSyms->Add(spx);
    }
    st->label = spx->sb->offset;
    st->purelabel = true;
    getsym();       /* Keyword::colon_ */
    lex = getsym(); /* next sym */
    before->needlabel = false;
}
EXPRESSION* StatementGenerator::ConvertReturnToRef(EXPRESSION* exp, Type* tp, Type* boundTP)
{
    if (lvalue(exp))
    {
        EXPRESSION* exp2;
        while (castvalue(exp))
            exp = exp->left;
        exp2 = exp;
        if (!tp->BaseType()->btp->IsStructured())
        {
            if (tp->BaseType()->btp->IsRef())
            {
                if (!tp->BaseType()->btp->BaseType()->btp->IsStructured())
                {
                    if (exp->left->type != ExpressionNode::auto_ || exp->left->v.sp->sb->storage_class != StorageClass::parameter_)
                        exp = exp->left;
                    if (exp->type == ExpressionNode::l_ref_)
                        if (exp->left->type != ExpressionNode::auto_ || exp->left->v.sp->sb->storage_class != StorageClass::parameter_)
                            exp = exp->left;
                }
                else
                {
                    exp = exp->left;
                }
            }
            else
            {
                if (exp->left->type != ExpressionNode::auto_ || exp->left->v.sp->sb->storage_class != StorageClass::parameter_)
                    exp = exp->left;
            }
        }
        else if (tp->BaseType()->btp->type == BasicType::aggregate_)
        {
            if (!boundTP->BaseType()->btp->IsFunction())
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
        else if (tp->BaseType()->btp->IsFunction())
        {
            if (!boundTP->BaseType()->btp->IsFunction())
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
        else if (exp->type == ExpressionNode::auto_)
        {
            if (exp->v.sp->sb->storage_class == StorageClass::auto_)
            {
                error(ERR_REF_RETURN_LOCAL);
            }
            else if (exp->v.sp->sb->storage_class == StorageClass::parameter_)
            {
                exp = exp2;
            }
        }
        else
        {
            if (referenceTypeError(tp, exp2) != exp2->type && (!tp->BaseType()->btp->IsStructured() || exp2->type != ExpressionNode::lvalue_))
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
    }
    else
    {
        if (exp->type == ExpressionNode::hook_)
        {
            exp->right->left = ConvertReturnToRef(exp->right->left, tp, boundTP);
            exp->right->right = ConvertReturnToRef(exp->right->right, tp, boundTP);
        }
        else if (!tp->BaseType()->btp->IsStructured() && !boundTP->IsRef())
        {
            error(ERR_LVALUE);
        }
        else if (exp->type == ExpressionNode::auto_ && exp->v.sp->sb->storage_class == StorageClass::auto_)
        {
            error(ERR_REF_RETURN_LOCAL);
        }
        else if (isintconst(exp) || isfloatconst(exp) || isimaginaryconst(exp) || iscomplexconst(exp))
        {
            error(ERR_REF_RETURN_TEMPORARY);
        }
        // this probably needs a little more work, I think if the two structures don't match types it will give an error...
    }
    return exp;
}
void StatementGenerator::MatchReturnTypes(Type* tp1, Type* tp2)
{
    if (matchReturnTypes)
    {
        if (tp2->type == BasicType::enum_ && tp1->IsInt() && tp1->btp)
            tp1 = tp1->btp;
        if (tp1->type == BasicType::enum_ && tp2->IsInt() && tp2->btp)
            tp2 = tp2->btp;
        bool err = false;
        if (tp1->IsRef())
            tp1 = tp1->BaseType()->btp;
        if (tp2->IsRef())
            tp2 = tp2->BaseType()->btp;
        while (tp1 && tp2 && !err)
        {
            // if (tp1->IsConst() != tp2->IsConst() || tp1->IsVolatile() != tp2->IsVolatile())
            //    err = true;
            tp1 = tp1->BaseType();
            tp2 = tp2->BaseType();
            if (tp1->IsStructured() && tp2->IsStructured())
            {
                if (!tp1->ExactSameType(tp2) && classRefCount(tp1->sp, tp2->sp) != 1)
                    err = true;
            }
            else if (tp1->type != tp2->type)
            {
                err = true;
            }
            tp1 = tp1->btp;
            tp2 = tp2->btp;
        }
        if (tp1 || tp2 || err)
        {
            errorsym(ERR_RETURN_TYPE_MISMATCH_FOR_AUTO_FUNCTION, funcsp);
        }
    }
}
void StatementGenerator::ParseReturn(std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    Statement* st;
    Type* tp = nullptr;
    EXPRESSION* returnexp = nullptr;
    Type* returntype = nullptr;
    EXPRESSION* destexp = nullptr;

    canFallThrough = true;
    if (funcsp->sb->attribs.inheritable.linkage3 == Linkage::noreturn_)
        error(ERR_NORETURN);
    funcsp->sb->retcount++;

    lex = getsym();
    if (MATCHKW(lex, Keyword::semicolon_))
    {
        if (!funcsp->tp->BaseType()->btp->IsVoid())
        {
            if (!funcsp->tp->BaseType()->sp->sb->isConstructor && !funcsp->tp->BaseType()->sp->sb->isDestructor)
                error(ERR_RETURN_MUST_RETURN_VALUE);
        }
    }
    else
    {
        int oldInLoop = inLoopOrConditional;
        inLoopOrConditional = 0;
        tp = funcsp->tp->BaseType()->btp;

        if (tp->IsAutoType())
        {
            Type* tp1;
            EXPRESSION* exp1;
            LexList* current = lex;
            lex = expression(lex, funcsp, nullptr, &tp1, &exp1, _F_SIZEOF);
            if (tp1->BaseType()->lref)
            {
                tp1 = Type::MakeType(BasicType::lref_, tp1);
            }
            else if (tp1->BaseType()->rref)
            {
                tp1 = Type::MakeType(BasicType::rref_, tp1);
            }
            lex = prevsym(current);
            while (tp1->type == BasicType::typedef_)
                tp1 = tp1->btp;
            DeduceAuto(&funcsp->tp->BaseType()->btp, tp1, exp1);
            tp = funcsp->tp->BaseType()->btp;
            funcsp->tp->UpdateRootTypes();
            SetLinkerNames(funcsp, funcsp->sb->attribs.inheritable.linkage);
            matchReturnTypes = true;
        }
        if (tp->IsStructured() || tp->IsBitInt() || tp->BaseType()->type == BasicType::memberptr_)
        {
            EXPRESSION* en = anonymousVar(StorageClass::parameter_, &stdpointer);
            SYMBOL* sp = en->v.sp;
            bool maybeConversion = true;
            sp->sb->allocate = false;  // static var
            sp->sb->offset = Optimizer::chosenAssembler->arch->retblocksize;
            sp->sb->structuredReturn = true;
            sp->name = "__retblock";
            sp->sb->retblk = true;
            // this next omiited the ->p
            if ((funcsp->sb->attribs.inheritable.linkage == Linkage::pascal_) && funcsp->tp->BaseType()->syms->size() &&
                ((SYMBOL*)funcsp->tp->BaseType()->syms->front())->tp->type != BasicType::void_)
                sp->sb->offset = funcsp->sb->paramsize;
            deref(&stdpointer, &en);
            if (Optimizer::cparams.prm_cplusplus && tp->IsStructured() && (!tp->BaseType()->sp->sb->trivialCons || MATCHKW(lex, Keyword::begin_)))
            {
                bool implicit = false;
                if (tp->BaseType()->sp->sb->templateLevel && tp->BaseType()->sp->templateParams && !tp->BaseType()->sp->sb->instantiated)
                {
                    SYMBOL* sym = tp->BaseType()->sp;
                    if (!allTemplateArgsSpecified(sym, sym->templateParams))
                        sym = GetClassTemplate(sym, sym->templateParams, false);
                    if (sym && allTemplateArgsSpecified(sym, sym->templateParams))
                        tp = TemplateClassInstantiate(sym, sym->templateParams, false, StorageClass::global_)->tp;
                }
                if (MATCHKW(lex, Keyword::begin_))
                {
                    implicit = true;
                    std::list<Initializer*>* init = nullptr, *dest = nullptr;
                    SYMBOL* sym = nullptr;
                    sym = anonymousVar(StorageClass::localstatic_, tp)->v.sp;
                    lex = initType(lex, funcsp, 0, StorageClass::auto_, &init, &dest, tp, sym, false, 0);
                    returnexp = convertInitToExpression(tp, nullptr, nullptr, funcsp, init, en, false);
                    returntype = tp;
                    if (sym)
                        sym->sb->dest = dest;
                }
                else
                {
                    bool oldrref, oldlref;
                    CallSite* funcparams = Allocate<CallSite>();
                    Type* ctype = tp;
                    // shortcut for conversion from single expression
                    EXPRESSION* exp1 = nullptr;
                    Type* tp1 = nullptr;
                    lex = expression_no_comma(lex, funcsp, nullptr, &tp1, &exp1, nullptr, 0);
                    MatchReturnTypes(tp, tp1);
                    if (!tp1)
                    {
                        tp1 = &stdint;
                        exp1 = MakeIntExpression(ExpressionNode::c_i_, 0);
                        error(ERR_IDENTIFIER_EXPECTED);
                    }
                    if (tp1 && tp1->IsStructured())
                    {
                        if (sameTemplate(tp, tp1))
                        {
                            ctype = tp = tp1;
                            funcsp->tp->BaseType()->btp = tp1;
                        }
                        else if (tp->ExactSameType(tp1))
                        {
                            implicit = true;
                        }
                        if (tp1->BaseType()->sp->sb->templateLevel && tp1->BaseType()->sp->templateParams &&
                            !tp1->BaseType()->sp->sb->instantiated && !definingTemplate)
                        {
                            SYMBOL* sym = tp1->BaseType()->sp;
                            if (!allTemplateArgsSpecified(sym, sym->templateParams))
                                sym = GetClassTemplate(sym, sym->templateParams, false);
                            if (sym)
                                ctype = tp1 = TemplateClassInstantiate(sym, sym->templateParams, false, StorageClass::global_)->tp;
                        }
                        optimize_for_constants(&exp1);
                    }
                    EXPRESSION* exptemp = exp1;
                    if (exptemp->type == ExpressionNode::thisref_)
                        exptemp = exptemp->left;
                    if (exptemp->type == ExpressionNode::callsite_ && exptemp->v.func->sp->sb->isConstructor && 
                        exptemp->v.func->sp->tp->IsFunction() && exptemp->v.func->thisptr &&
                        tp->SameType(exptemp->v.func->thistp->BaseType()->btp) &&
                        (!tp->BaseType()->sp->sb->templateLevel || sameTemplate(tp, exptemp->v.func->thistp->BaseType()->btp)) &&
                        exptemp->v.func->thisptr->type == ExpressionNode::auto_ && exptemp->v.func->thisptr->v.sp->sb->anonymous)
                    {
                        exptemp->v.func->thisptr->v.sp->sb->destructed = true;
                        exptemp->v.func->thisptr = en;
                        returntype = tp;
                        returnexp = exp1;
                        maybeConversion = false;
                        implicit = false;
                    }
                    else if (exptemp->type == ExpressionNode::comma_)
                    {
                        // a list of initializers into a temp var...   we don't want to do a constructor here because we just constructe it...
                        // so replace the expression
                        while (exptemp->type == ExpressionNode::comma_) exptemp = exptemp->right;
                        if (exptemp->type == ExpressionNode::auto_)
                        {
                            exptemp->v.sp->sb->destructed = true;
                        }
                        auto targetPointer = anonymousVar(StorageClass::auto_, &stdpointer);
                        deref(&stdpointer, &targetPointer);
                        auto targetExpr = MakeExpression(ExpressionNode::assign_, targetPointer, en);
                        ReplaceVarRef(&exp1, exptemp->v.sp, targetPointer);
                        exp1 = MakeExpression(ExpressionNode::comma_, targetExpr, exp1);
                        returntype = tp;
                        returnexp = exp1;
                        maybeConversion = false;
                        implicit = false;
                    }
                    /*
                    else if ((tp1->ExactSameType(tp) || sameTemplate(tp, tp1)) && !tp1->rref && !tp1->lref)
                    {
                        returntype = tp;
                        returnexp = exp1;
                        maybeConversion = false;
                        implicit = false;
                    }
                    */
                    else
                    {
                        bool nonconst = funcsp->sb->nonConstVariableUsed;
                        funcparams->arguments = argumentListFactory.CreateList();
                        funcparams->arguments->push_back(Allocate<Argument>());
                        funcparams->arguments->front()->tp = tp1;
                        funcparams->arguments->front()->exp = exp1;
                        oldrref = tp1->BaseType()->rref;
                        oldlref = tp1->BaseType()->lref;
                        tp1->BaseType()->rref = exp1->type == ExpressionNode::auto_ && exp1->v.sp->sb->storage_class != StorageClass::parameter_;
                        if (exptemp->type == ExpressionNode::callsite_ && exptemp->v.func->sp->tp->IsFunction() &&
                            exptemp->v.func->sp->tp->BaseType()->btp->BaseType()->type != BasicType::lref_)
                            tp1->BaseType()->rref = true;
                        tp1->BaseType()->lref = !tp1->BaseType()->rref;
                        maybeConversion = false;
                        returntype = tp;
                        // try the rref constructor first
                        if (callConstructor(&ctype, &en, funcparams, false, nullptr, true, maybeConversion, implicit, false, false,
                            false, false))
                        {
                            if (funcparams->sp && matchesCopy(funcparams->sp, true))
                            {
                                switch (exp1->type)
                                {
                                case ExpressionNode::global_:
                                case ExpressionNode::auto_:
                                case ExpressionNode::threadlocal_:
                                    exp1->v.sp->sb->dest = nullptr;
                                    break;
                                default:
                                    break;
                                }
                            }
                        }
                        else
                        {
                            // not there try an lref version of the constructor
                            ctype = tp;
                            tp1->BaseType()->rref = false;
                            tp1->BaseType()->lref = true;
                            callConstructor(&ctype, &en, funcparams, false, nullptr, true, maybeConversion, implicit, false, false,
                                false, true);
                        }
                        tp1->BaseType()->rref = oldrref;
                        tp1->BaseType()->lref = oldlref;
                        funcsp->sb->nonConstVariableUsed = nonconst;
                        returnexp = en;
                    }
                }
            }
            else
            {

                Type* tp1;
                lex = optimized_expression(lex, funcsp, nullptr, &tp1, &returnexp, true);
                if (!tp1)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                    MatchReturnTypes(tp, tp1);
                if (!tp->ExactSameType(tp1) &&
                    (!tp->IsBitInt() || !tp1->IsBitInt()) &&
                    ((Optimizer::architecture != ARCHITECTURE_MSIL) || !tp->IsStructured() || !isconstzero(&stdint, returnexp)))
                {
                    bool toErr = true;
                    if (tp->IsStructured() && tp1->IsStructured() && classRefCount(tp->BaseType()->sp,tp1->BaseType()->sp))
                    {
                        toErr = false;
                        returnexp = baseClassOffset(tp->BaseType()->sp, tp1->BaseType()->sp, returnexp);
                    }
                    if (toErr)
                    {
                        errorConversionOrCast(true, tp1, tp);
                    }
                }
                else
                {
                    if (returnexp->type == ExpressionNode::callsite_ && !returnexp->v.func->ascall)
                    {
                        if (returnexp->v.func->sp->sb->storage_class == StorageClass::overloads_)
                        {
                            SYMBOL* funcsp;
                            if (returnexp->v.func->sp->sb->parentClass && !returnexp->v.func->asaddress)
                                error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                            funcsp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &returnexp, 0);
                            if (funcsp && tp->BaseType()->type == BasicType::memberptr_)
                            {
                                int lbl = dumpMemberPtr(funcsp, tp, true);
                                returnexp = MakeIntExpression(ExpressionNode::labcon_, lbl);
                            }
                        }
                        else
                        {
                            returnexp = MakeIntExpression(ExpressionNode::labcon_, dumpMemberPtr(returnexp->v.func->sp, tp, true));
                        }
                    }
                    else if (returnexp->type == ExpressionNode::pc_ || returnexp->type == ExpressionNode::memberptr_)
                    {
                        returnexp = MakeIntExpression(ExpressionNode::labcon_, dumpMemberPtr(returnexp->v.sp, tp, true));
                    }
                    if ((Optimizer::architecture != ARCHITECTURE_MSIL) ||
                        funcsp->sb->attribs.inheritable.linkage2 == Linkage::unmanaged_ || !msilManaged(funcsp))
                    {
                        if (tp->IsBitInt())
                        {
                            if (!tp->SameType(tp1))
                            {
                                cast(tp, &returnexp);
                            }
                            returnexp = MakeExpression(ExpressionNode::blockassign_, en, returnexp);
                            returnexp->size = tp;
                            returnexp->altdata = (void*)(tp->BaseType());

                        }
                        else if (!tp->IsStructured() || !tp->BaseType()->sp->sb->structuredAliasType)
                        {
                            if (Optimizer::cparams.prm_cplusplus && returnexp->type == ExpressionNode::comma_)
                            {
                                // a list of initializers into a temp var...   we don't want to do a constructor here because we just constructe it...
                                // so replace the expression
                                auto exptemp = returnexp;
                                while (exptemp->type == ExpressionNode::comma_) exptemp = exptemp->right;
                                if (exptemp->type == ExpressionNode::auto_)
                                {
                                    exptemp->v.sp->sb->destructed = true;
                                }
                                auto targetPointer = anonymousVar(StorageClass::auto_, &stdpointer);
                                deref(&stdpointer, &targetPointer);
                                auto targetExpr = MakeExpression(ExpressionNode::assign_, targetPointer, en);
                                ReplaceVarRef(&returnexp, exptemp->v.sp, targetPointer);
                                returnexp = MakeExpression(ExpressionNode::comma_, targetExpr, returnexp);
                            }
                            else
                            {
                                returnexp = MakeExpression(ExpressionNode::blockassign_, en, returnexp);
                                returnexp->size = tp;
                                returnexp->altdata = (void*)(tp->BaseType());
                            }
                        }
                        else
                        {
                            EXPRESSION** expx = &returnexp;
                            if (*expx && (*expx)->type == ExpressionNode::comma_)
                            {
                                while ((*expx)->right && (*expx)->right->type == ExpressionNode::comma_)
                                    expx = &(*expx)->right;
                                expx = &(*expx)->right;
                            }
                            deref(tp->BaseType()->sp->sb->structuredAliasType, expx);
                        }
                    }
                    returntype = tp;
                }
            }
        }
        else
        {
            Type* tp1 = nullptr;
            bool needend = false;
            if (MATCHKW(lex, Keyword::begin_))
            {
                needend = true;
                lex = getsym();
            }
            lex = optimized_expression(lex, funcsp, tp, &tp1, &returnexp, true);
            if (!tp1)
            {
                tp1 = &stdint;
                error(ERR_EXPRESSION_SYNTAX);
            }
            else
                MatchReturnTypes(tp, tp1);
            if (tp->BaseType()->type == BasicType::string_)
            {
                if (returnexp->type == ExpressionNode::labcon_)
                    returnexp->type = ExpressionNode::c_string_;
                else if (tp1->BaseType()->type != BasicType::string_)
                    returnexp = MakeExpression(ExpressionNode::x_string_, returnexp);
                tp1 = &std__string;
            }
            else if (!tp->ExactSameType(tp1))
            {
                bool err = false;
                if (Optimizer::cparams.prm_cplusplus)
                {
                    if (tp1->IsStructured())
                    {
                        auto tpx = tp;
                        if (tpx->IsRef())
                            tpx = tpx->BaseType()->btp;
                        if (tpx->IsStructured())
                        {
                            if (!doStaticCast(&tp, tp1, &returnexp, funcsp, true))
                                err = true;
                        }
                        else
                        {
                            // handles pointers as well...
                            if (!castToArithmeticInternal(false, &tp1, &returnexp, Keyword::plus_, tpx, false))
                                err = true;
                        }
                    }
                    else if (tp1->IsPtr() && tp->IsPtr())
                    {
                        if (!doStaticCast(&tp, tp1, &returnexp, funcsp, true))
                            err = true;
                    }
                    else if ((!tp1->IsArithmetic() && tp1->type != BasicType::enum_) || (!tp->IsArithmetic() && tp->type != BasicType::enum_))
                    {
                        if ((!tp->IsPtr() || (!isconstzero(tp1, returnexp) && tp1->type != BasicType::aggregate_ && !tp1->IsFunction())) &&
                            (tp->BaseType()->type != BasicType::bool_ || !tp1->IsPtr()))
                        {
                            err = true;
                        }
                    }
                }
                else if (tp1->IsMsil() || tp1->IsStructured())
                {
                    err = true;
                }
                if (err)
                {
                    errorConversionOrCast(true, tp1, tp);
                }
            }
            if (needend)
            {
                if (!needkw(&lex, Keyword::end_))
                {
                    errskim(&lex, skim_end);
                    skip(&lex, Keyword::end_);
                }
            }
            if (tp->BaseType()->type == BasicType::object_)
                if (tp1->BaseType()->type != BasicType::object_ && !tp1->IsStructured() && (!tp1->IsArray() || !tp1->BaseType()->msil))
                    returnexp = MakeExpression(ExpressionNode::x_object_, returnexp);
            if (tp1->IsStructured() && tp->IsArithmetic())
            {
                if (Optimizer::cparams.prm_cplusplus)
                {
                    castToArithmetic(false, &tp1, &returnexp, (Keyword) - 1, tp, true);
                }
                else
                {
                    errorConversionOrCast(false, tp1, tp);
                }
            }
            if (tp->type == BasicType::auto_)
                returntype = tp = tp1;
            else
            {
                returntype = tp;
            }
            if (returnexp->type == ExpressionNode::callsite_)
            {
                if (returnexp->v.func->sp->sb->storage_class == StorageClass::overloads_)
                {
                    EXPRESSION* exp1 = returnexp;
                    if (returnexp->v.func->sp->sb->parentClass && !returnexp->v.func->asaddress)
                        error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                    returnexp->v.func->sp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &exp1, 0);
                    returnexp->v.func->fcall = MakeExpression(ExpressionNode::pc_, returnexp->v.func->sp);
                }
            }
            if (Optimizer::cparams.prm_cplusplus && returntype->IsStructured())
            {
                Type* tp1 = funcsp->tp->BaseType()->btp;
                if (tp1->IsRef())
                    tp1 = tp1->BaseType()->btp;
                if (cppCast(returntype, &tp1, &returnexp))
                    returntype = tp = funcsp->tp->BaseType()->btp;
            }
        }
        if (funcsp->tp->BaseType()->btp->IsRef())
        {
            if (funcsp->tp->BaseType()->btp->BaseType()->type != BasicType::memberptr_)
                returnexp = ConvertReturnToRef(returnexp, funcsp->tp->BaseType()->btp, returntype);
        }
        else if (returnexp && returnexp->type == ExpressionNode::auto_ && returnexp->v.sp->sb->storage_class == StorageClass::auto_)
        {
            if (!funcsp->tp->BaseType()->btp->IsStructured() && funcsp->tp->BaseType()->btp->BaseType()->type != BasicType::memberptr_ &&
                !funcsp->tp->BaseType()->btp->IsBitInt())
                if (funcsp->tp->BaseType()->btp->BaseType()->type != BasicType::object_ &&
                    (!funcsp->tp->BaseType()->btp->IsArray() || !funcsp->tp->BaseType()->btp->msil) &&
                    funcsp->tp->BaseType()->btp->BaseType()->type != BasicType::templateselector_)
                    error(ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE);
        }
        if (!returnexp)
            returnexp = MakeIntExpression(ExpressionNode::c_i_, 0);  // errors
        inLoopOrConditional = oldInLoop;
    }
    currentLineData(parent, lex, 0);
    ThunkReturnDestructors(&destexp, nullptr, localNameSpace->front()->syms);
    st = Statement::MakeStatement(lex, parent, StatementNode::return_);
    st->select = returnexp;
    st->destexp = destexp;
    ThunkCatchCleanup(st, parent, emptyBlockdata);  // to top level
    if (returnexp && returntype && !returntype->IsAutoType() && !matchReturnTypes)
    {
        if (!tp)  // some error...
            tp = &stdint;
        if (tp->type == BasicType::void_)
        {
            if (!Optimizer::cparams.prm_cplusplus || returntype->type != BasicType::void_)
                error(ERR_VOID_FUNCTION_RETURNS_VALUE);
        }
        else if (returntype && returntype->type == BasicType::void_)
            error(ERR_RETURN_NO_VALUE);
        else
        {
            if (Optimizer::cparams.prm_cplusplus && (funcsp->sb->isConstructor || funcsp->sb->isDestructor))
            {
                error(ERR_CONSTRUCTOR_HAS_RETURN);
            }
            else if (returntype->IsStructured() || tp->IsStructured())
            {
                if (!returntype->SameType(tp) && !sameTemplate(returntype, tp))
                    error(ERR_RETMISMATCH);
            }
            else if (returntype->BaseType()->type == BasicType::memberptr_ || tp->BaseType()->type == BasicType::memberptr_)
            {
                if (isconstzero(tp, st->select))
                {
                    int lbl = dumpMemberPtr(nullptr, returntype, true);
                    st->select = MakeIntExpression(ExpressionNode::labcon_, lbl);
                }
                else
                {
                    if (st->select->type == ExpressionNode::memberptr_)
                    {
                        int lbl = dumpMemberPtr(st->select->v.sp, returntype, true);
                        st->select = MakeIntExpression(ExpressionNode::labcon_, lbl);
                    }
                    if (!returntype->ExactSameType(tp))
                        error(ERR_RETMISMATCH);
                }
            }
            else
            {
                if (!funcsp->tp->BaseType()->btp->IsRef() &&
                    !funcsp->tp->BaseType()->btp->IsBitInt() &&
                    (funcsp->tp->BaseType()->btp->IsArithmetic() ||
                     (funcsp->tp->BaseType()->btp->IsPtr() && !funcsp->tp->BaseType()->btp->IsArray())))
                    cast(returntype, &st->select);
                if (returntype->IsPtr())
                {
                    if (tp->IsArithmetic())
                    {
                        if (tp->IsComplex())
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (tp->IsFloat() || tp->IsImaginary())
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (isarithmeticconst(returnexp))
                        {
                            if (!isintconst(returnexp) || !isconstzero(returntype, returnexp))
                                error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                        else if (returnexp->type != ExpressionNode::callsite_ || returnexp->v.func->fcall->type != ExpressionNode::l_p_)
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    }
                    else if (tp->IsPtr())
                    {
                        if (!returntype->ExactSameType(tp))
                        {
                            if (!returntype->IsVoidPtr() && !tp->IsVoidPtr())
                            {
                                if (!returntype->SameCharType(tp))
                                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                            }
                            else
                            {
                                if (Optimizer::cparams.prm_cplusplus && !returntype->IsVoidPtr() && returnexp->type != ExpressionNode::nullptr_ &&
                                    tp->IsVoidPtr())
                                    error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                            }
                        }
                    }
                    else if (tp->IsFunction())
                    {
                        if (!returntype->IsVoidPtr() &&
                            (!returntype->BaseType()->btp->IsFunction() || !returntype->ExactSameType(tp)))
                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                    }
                    else
                        error(ERR_INVALID_POINTER_CONVERSION);
                }
                else if (tp->IsPtr())
                {
                    if (returntype->IsComplex())
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (returntype->IsFloat() || returntype->IsImaginary())
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (returntype->IsInt())
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                }
                else if (tp->IsPtr())
                {
                    if (returntype->IsComplex())
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (returntype->IsFloat() || returntype->IsImaginary())
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (returntype->IsInt())
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                }
            }
        }
    }
    before->needlabel = true;
}
void StatementGenerator::ParseSwitch(std::list<FunctionBlock*>& parent)
{
    bool oldfallthrough = canFallThrough;
    canFallThrough = true;
    auto before = parent.front();
    FunctionBlock* switchstmt = Allocate<FunctionBlock>();
    Statement* st;
    EXPRESSION* select = nullptr, *init = nullptr;
    int addedBlock = 0;
    funcsp->sb->noinline = true;
    switchstmt->breaklabel = codeLabel++;
    switchstmt->defaultlabel = -1; /* no default */
    switchstmt->type = Keyword::switch_;
    switchstmt->table = localNameSpace->front()->syms;
    parent.push_front(switchstmt);
    lex = getsym();
    inLoopOrConditional++;
    if (MATCHKW(lex, Keyword::openpa_))
    {
        lex = getsym();
        if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        bool declaration = false;
        SelectionExpression(parent, &select, Keyword::switch_, &declaration);
        if (declaration)
        {
            if (!needkw(&lex, Keyword::semicolon_))
            {
                error(ERR_FOR_NEEDS_SEMI);
                errskim(&lex, skim_closepa);
                skip(&lex, Keyword::closepa_);
            }
            else
            {
                init = select;
                SelectionExpression(parent, &select, Keyword::if_, nullptr);
            }
        }
        if (MATCHKW(lex, Keyword::closepa_))
        {
            currentLineData(parent, lex, 0);
            lex = getsym();
            if (init)
            {
                st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
                st->select = init;
            }
            st = Statement::MakeStatement(lex, parent, StatementNode::switch_);
            st->select = select;
            st->breaklabel = switchstmt->breaklabel;
            switchstmt->cases = casedataListFactory.CreateList();
            StatementWithoutNonconst(parent, true);
            EndOfCaseGroup(parent);
            st->cases = switchstmt->cases;
            st->label = switchstmt->defaultlabel;
            if (st->label != -1 && switchstmt->needlabel && !switchstmt->hasbreak)
                before->needlabel = true;
            /* force a default if there was none */
            if (st->label == -1)
            {
                st->label = codeLabel;
                st = Statement::MakeStatement(lex, parent, StatementNode::label_);
                st->label = codeLabel++;
            }
            st = Statement::MakeStatement(lex, parent, StatementNode::label_);
            st->label = switchstmt->breaklabel;
            if (!switchstmt->nosemi && !switchstmt->hassemi)
                errorint(ERR_NEEDY, ';');
            before->nosemi = true;
        }
        else
        {
            error(ERR_SWITCH_NEEDS_CLOSEPA);
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
    }
    else
    {
        error(ERR_SWITCH_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, Keyword::closepa_);
    }
    inLoopOrConditional--;
    while (addedBlock--)
        FreeLocalContext(parent, funcsp, codeLabel++);
    parent.pop_front();
    switchstmt->AddThis(lex, parent);
    canFallThrough = oldfallthrough;
}
void StatementGenerator::ParseWhile(std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    FunctionBlock* whilestmt = Allocate<FunctionBlock>();
    Statement *st, *lastLabelStmt;
    std::list<Statement*>* whileline;
    EXPRESSION* select = nullptr;
    int addedBlock = 0;
    int loopLabel = codeLabel++;
    whilestmt->breaklabel = codeLabel++;
    whilestmt->continuelabel = codeLabel++;
    whilestmt->type = Keyword::while_;
    whilestmt->table = localNameSpace->front()->syms;
    whileline = currentLineData(emptyBlockdata, lex, 0);
    parent.push_front(whilestmt);
    lex = getsym();
    inLoopOrConditional++;
    if (MATCHKW(lex, Keyword::openpa_))
    {
        lex = getsym();
        if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        bool declaration = false;
        SelectionExpression(parent, &select, Keyword::while_, &declaration);
        if (!MATCHKW(lex, Keyword::closepa_))
        {
            error(ERR_WHILE_NEEDS_CLOSEPA);

            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
        else
        {
            lex = getsym();
            if (Optimizer::cparams.prm_debug || Optimizer::cparams.prm_optimize_for_size ||
                (Optimizer::chosenAssembler->arch->denyopts & DO_NOENTRYIF))
            {
                st = Statement::MakeStatement(lex, parent, StatementNode::goto_);
                st->label = whilestmt->continuelabel;
            }
            else
            {
                st = Statement::MakeStatement(lex, parent, StatementNode::notselect_);
                st->label = whilestmt->breaklabel;
                st->altlabel = whilestmt->continuelabel;
                st->select = select;
            }

            st = Statement::MakeStatement(lex, parent, StatementNode::label_);
            st->label = loopLabel;
            if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            do
            {
                lastLabelStmt = whilestmt->statements->back();
                StatementWithoutNonconst(parent, true);
            } while (lex && whilestmt->statements->back() != lastLabelStmt && whilestmt->statements->back()->purelabel);
            if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
            {
                addedBlock--;
                FreeLocalContext(parent, funcsp, codeLabel++);
            }
            st = Statement::MakeStatement(lex, parent, StatementNode::label_);
            st->label = whilestmt->continuelabel;
            if (whileline)
            {
                parent.front()->statements->insert(parent.front()->statements->end(), whileline->begin(), whileline->end());
            }
            st = Statement::MakeStatement(lex, parent, StatementNode::select_);
            st->label = loopLabel;
            st->select = select;
            if (!whilestmt->hasbreak && IsSelectTrue(st->select))
                before->needlabel = true;
            st = Statement::MakeStatement(lex, parent, StatementNode::label_);
            st->label = whilestmt->breaklabel;
            before->hassemi = whilestmt->hassemi;
            before->nosemi = whilestmt->nosemi;
        }
    }
    else
    {
        error(ERR_WHILE_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, Keyword::closepa_);
    }
    inLoopOrConditional--;
    while (addedBlock--)
        FreeLocalContext(parent, funcsp, codeLabel++);
    std::list<FunctionBlock*> dummy{ whilestmt };
    parent.pop_front();
    whilestmt->AddThis(lex, parent);
}
bool StatementGenerator::NoSideEffect(EXPRESSION* exp)
{
    if (exp->noexprerr)
        return false;
    while (castvalue(exp) || lvalue(exp))
        exp = exp->left;
    switch (exp->type)
    {
        case ExpressionNode::callsite_:
        case ExpressionNode::assign_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::stmt_:
        case ExpressionNode::atomic_:
        case ExpressionNode::check_nz_:
        case ExpressionNode::comma_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
            return false;
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
        case ExpressionNode::thisref_:
        case ExpressionNode::funcret_:
        case ExpressionNode::constexprconstructor_:
            return NoSideEffect(exp->left);
        case ExpressionNode::hook_:
            return NoSideEffect(exp->right->left) & NoSideEffect(exp->right->right);
        default:
            return true;
    }
}
void StatementGenerator::ParseExpr(std::list<FunctionBlock*>& parent)
{
    Statement* st;
    EXPRESSION* select = nullptr;
    Type* tp = nullptr;
    auto oldLines = lines;
    lines = nullptr;
    (void)parent;

    auto prevlex = lex;
    lex = optimized_expression(prevlex, funcsp, nullptr, &tp, &select, true);
    if (expressionReturns.size())
        expressionReturns.back() = std::move(std::pair<EXPRESSION*, Type*>(select, tp));
    lines = oldLines;

    currentLineData(parent, prevlex, 0);
    st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
    st->select = select;
    if (!tp)
    {
        lex = getsym();
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (tp->type != BasicType::void_ && tp->type != BasicType::any_)
    {
        if (NoSideEffect(st->select))
            error(ERR_EXPRESSION_HAS_NO_EFFECT);
        if (Optimizer::cparams.prm_cplusplus && tp->IsStructured() && select->type == ExpressionNode::callsite_)
        {
            SYMBOL* sym = select->v.func->returnSP;
            if (sym && sym->sb->allocate)
            {
                std::list<Initializer*>* init = nullptr;
                EXPRESSION* exp = select->v.func->returnEXP;
                callDestructor(tp->BaseType()->sp, nullptr, &exp, nullptr, true, false, false, true);
                initInsert(&init, sym->tp, exp, 0, false);
                sym->sb->dest = init;
            }
        }
    }
    if (select)
    {
        auto exp1 = select;
        if (exp1->type == ExpressionNode::thisref_)
        {
            exp1 = exp1->left;
        }
        if (exp1->type == ExpressionNode::callsite_)
        {
            auto sp = exp1->v.func->sp;
            if (sp->sb->attribs.uninheritable.nodiscard)
            {
                if (!sp->sb->isConstructor && !sp->sb->isDestructor)
                {
                    if (!sp->tp->BaseType()->btp->IsVoid())
                        error(ERR_RETURN_VALUE_NO_DISCARD);
                }
            }
        }
    }
}
void StatementGenerator::AsmDeclare()
{
    Keyword kw = lex->data->kw->key;
    do
    {
        lex = getsym();
        if (lex)
        {
            if (ISID(lex))
            {
                SYMBOL* sym = search(globalNameSpace->front()->syms, lex->data->value.s.a);
                if (!sym)
                {
                    sym = makeID(StorageClass::label_, nullptr, nullptr, litlate(lex->data->value.s.a));
                }
                switch (kw)
                {
                    case Keyword::public_:
                        sym->sb->storage_class = StorageClass::global_;
                        break;
                    case Keyword::extern_:
                        sym->sb->storage_class = StorageClass::external_;
                        break;
                    case Keyword::const_:
                        sym->sb->storage_class = StorageClass::constant_;
                        break;
                    default:
                        break;
                }
            }
            else
            {
                error(ERR_IDENTIFIER_EXPECTED);
                break;
            }
            lex = getsym();
        }
    } while (lex && MATCHKW(lex, Keyword::comma_));
}
void StatementGenerator::ParseCatch(std::list<FunctionBlock*>& parent, int label, int startlab, int endlab)
{
    auto before = parent.front();
    auto ilbefore = parent.begin();
    ++ilbefore;
    auto next = ilbefore != parent.end() ? *ilbefore : nullptr;
    bool last = false;
    inLoopOrConditional++;
    if (!MATCHKW(lex, Keyword::catch_))
    {
        error(ERR_EXPECTED_CATCH_CLAUSE);
    }
    while (MATCHKW(lex, Keyword::catch_))
    {
        if (last)
            error(ERR_NO_MORE_CATCH_CLAUSES_ALLOWED);
        lex = getsym();
        if (needkw(&lex, Keyword::openpa_))
        {
            Statement* st;
            Type* tp = nullptr;
            FunctionBlock* catchstmt = Allocate<FunctionBlock>();
            ParseAttributeSpecifiers(&lex, funcsp, true);
            catchstmt->breaklabel = label;
            catchstmt->defaultlabel = -1; /* no default */
            catchstmt->type = Keyword::catch_;
            catchstmt->table = localNameSpace->front()->syms;
            parent.push_front(catchstmt);
            AllocateLocalContext(parent, funcsp, codeLabel++);
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                last = true;
                lex = getsym();
            }
            else
            {
                lex = declare(lex, funcsp, &tp, StorageClass::catchvar_, Linkage::none_, parent, false, true, false, AccessLevel::public_);
            }
            if (tp && tp->IsStructured())
            {
                auto cc = getCopyCons(tp->BaseType()->sp, false);
                if (cc && cc->sb->isExplicit)
                    error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
            }
            if (needkw(&lex, Keyword::closepa_))
            {
                if (MATCHKW(lex, Keyword::begin_))
                {
                    Compound(parent, false);
                    before->nosemi = true;
                    before->needlabel &= catchstmt->needlabel;
                    if (next)
                        next->nosemi = true;
                }
                else
                {
                    error(ERR_EXPECTED_CATCH_BLOCK);
                }
            }
            FreeLocalContext(parent, funcsp, codeLabel++);
            parent.pop_front();
            st = Statement::MakeStatement(lex, parent, StatementNode::catch_);
            st->label = startlab;
            st->endlabel = endlab;
            st->altlabel = codeLabel++;
            st->breaklabel = catchstmt->breaklabel;
            st->blockTail = catchstmt->blockTail;
            st->lower = catchstmt->statements;
            st->tp = tp;
        }
        else
        {
            errskim(&lex, skim_end);
        }
    }
    inLoopOrConditional--;
}
void StatementGenerator::ParseTry(std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    auto ilbefore = parent.begin();
    ++ilbefore;
    auto next = ilbefore != parent.end() ? *ilbefore : nullptr;
    Statement* st;
    FunctionBlock* trystmt = Allocate<FunctionBlock>();
    hasXCInfo = true;
    trystmt->breaklabel = codeLabel++;
    trystmt->defaultlabel = -1; /* no default */
    trystmt->type = Keyword::try_;
    trystmt->table = localNameSpace->front()->syms;
    funcsp->sb->anyTry = true;
    lex = getsym();
    inLoopOrConditional++;
    if (!MATCHKW(lex, Keyword::begin_))
    {
        error(ERR_EXPECTED_TRY_BLOCK);
    }
    else
    {
        parent.push_front(trystmt);
        AllocateLocalContext(parent, funcsp, codeLabel++);
        tryLevel++;
        Compound(parent, false);
        tryLevel--;
        FreeLocalContext(parent, funcsp, codeLabel++);
        parent.pop_front();
        before->needlabel = trystmt->needlabel;
        st = Statement::MakeStatement(lex, parent, StatementNode::try_);
        st->label = codeLabel++;
        st->endlabel = codeLabel++;
        st->breaklabel = trystmt->breaklabel;
        st->blockTail = trystmt->blockTail;
        st->lower = trystmt->statements;
        before->nosemi = true;
        if (next)
            next->nosemi = true;
        ParseCatch(parent, st->breaklabel, st->label, st->endlabel);
    }
    inLoopOrConditional--;
}
bool StatementGenerator::ParseAsm(std::list<FunctionBlock*>& parent)
{
    auto before = parent.front();
    (void)parent;  //
    Optimizer::functionHasAssembly = true;
#ifndef ORANGE_NO_INASM
    if (StatementGenerator::HasInlineAsm())
    {
        before->hassemi = false;
        lex = getsym();
        if (MATCHKW(lex, Keyword::begin_))
        {
            lex = getsym();
            while (lex && !MATCHKW(lex, Keyword::end_))
            {
                currentLineData(parent, lex, 0);
                lex = inlineAsm(lex, parent);
                if (KW(lex) == Keyword::semicolon_)
                {
                    skip(&lex, Keyword::semicolon_);
                }
            }
            needkw(&lex, Keyword::end_);
            before->nosemi = true;
            return lex != nullptr;
        }
        else
        {
            currentLineData(parent, lex, 0);
            while (Optimizer::cparams.prm_assemble && lex && MATCHKW(lex, Keyword::semicolon_))
                lex = SkipToNextLine();
            if (lex)
            {
                if (Optimizer::cparams.prm_assemble &&
                    (MATCHKW(lex, Keyword::public_) || MATCHKW(lex, Keyword::extern_) || MATCHKW(lex, Keyword::const_)))
                {
                    AsmDeclare();
                }
                else
                {
                    lex = inlineAsm(lex, parent);
                }
                if (MATCHKW(lex, Keyword::semicolon_))
                {
                    if (Optimizer::cparams.prm_assemble)
                        lex = SkipToNextLine();
                    else
                        skip(&lex, Keyword::semicolon_);
                }
            }
            before->hassemi = true;
        }
    }
    else
#endif
    {
        /* if we get here the backend doesn't have an assembler, for now we
         * are just going to make an error and scan past tokens
         */
        lex = getsym();
        errorstr(ERR_ASM, "Assembly language not supported by this compiler");
        if (MATCHKW(lex, Keyword::begin_))
        {
            while (lex && !MATCHKW(lex, Keyword::end_))
            {
                currentLineData(parent, lex, 0);
                lex = getsym();
            }
            needkw(&lex, Keyword::end_);
            before->nosemi = true;
        }
        else
        {
            /* problematic, ASM keyword without a block->  Skip to Keyword::end_ of line... */
            currentLineData(parent, lex, 0);
            before->hassemi = true;
            SkipToEol();
            lex = getsym();
        }
    }
    return lex != nullptr;
}
void StatementGenerator::AssignInReverse(std::list<Statement*>* current, EXPRESSION** exp)
{
    if (current)
    {
        for (auto it = current->end(); it != current->begin();)
        {
            --it;
            if ((*it)->type != StatementNode::line_ && (*it)->type != StatementNode::varstart_)
            {
                if (*exp)
                    *exp = MakeExpression(ExpressionNode::comma_, (*it)->select, *exp);
                else
                    *exp = (*it)->select;
            }
        }
    }
}
void StatementGenerator::AutoDeclare(Type** tp, EXPRESSION** exp, std::list<FunctionBlock*>& parent, int asExpression)
{
    FunctionBlock block;
    (void)parent;
    declareAndInitialize = false;
    memset(&block, 0, sizeof(block));
    parent.push_front(&block);
    lex = declare(lex, funcsp, tp, StorageClass::auto_, Linkage::none_, parent, false, asExpression, false, AccessLevel::public_);
    parent.pop_front();
   
    // move any auto assignments
    AssignInReverse(block.statements, exp);

    // now move variable declarations
    if (block.statements)
    {
        for (auto stmt : *block.statements)
        {
            if (stmt->type == StatementNode::varstart_)
            {
                Statement* s = Statement::MakeStatement(lex, parent, StatementNode::varstart_);
                s->select = stmt->select;
            }
            else if (stmt->type == StatementNode::line_)
            {
                Statement* s = Statement::MakeStatement(lex, parent, StatementNode::line_);
                s->lineData = stmt->lineData;
            }
        }
    }
    if (!*exp)
    {
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        if (!(asExpression & _F_NOCHECKAUTO))
            errorint(ERR_NEEDY, '=');
    }
}
bool StatementGenerator::ResolvesToDeclaration(LexList* lex, bool structured)
{
    LexList* placeholder = lex;
    if (ISKW(lex))
        switch (KW(lex))
        {
            case Keyword::struct_:
            case Keyword::union_:
            case Keyword::class_:
            case Keyword::decltype_:
                return true;
            default:
                break;
        }
    lex = getsym();
    while (MATCHKW(lex, Keyword::classsel_))
    {
        lex = getsym();
        lex = getsym();
    }
    if (MATCHKW(lex, Keyword::lt_))
    {
        int level = 1;
        lex = getsym();
        while (level && lex != nullptr && !MATCHKW(lex, Keyword::semicolon_))
        {
            if (MATCHKW(lex, Keyword::lt_))
            {
                level++;
            }
            else if (MATCHKW(lex, Keyword::gt_))
            {
                level--;
            }
            else if (MATCHKW(lex, Keyword::rightshift_))
            {
                level--;
                lex = getGTSym(lex);
                continue;
            }
            lex = getsym();
        }
    }
    if (MATCHKW(lex, Keyword::begin_))
    {
        prevsym(placeholder);
        return false;
    }
    if (MATCHKW(lex, Keyword::openpa_))
    {
        bool hasStar = false;
        bool hasThis = false;
        int level = 1;
        lex = getsym();
        while (level && lex != nullptr && !MATCHKW(lex, Keyword::semicolon_))
        {
            if (hasStar && MATCHKW(lex, Keyword::this_))
            {
                hasThis = true;
            }
            if (MATCHKW(lex, Keyword::openpa_))
            {
                level++;
            }
            else if (MATCHKW(lex, Keyword::closepa_))
            {
                level--;
            }
            else if (MATCHKW(lex, Keyword::star_))
            {
                hasStar = true;
            }
            lex = getsym();
        }
        if (MATCHKW(lex, Keyword::assign_) || ((hasStar || !structured) && !hasThis && MATCHKW(lex, Keyword::openpa_)) || MATCHKW(lex, Keyword::openbr_))
        {
            prevsym(placeholder);
            return true;
        }
        prevsym(placeholder);
        return false;
    }
    prevsym(placeholder);
    return true;
}

void StatementGenerator::SingleStatement(std::list<FunctionBlock*>& parent, bool viacontrol)
{
    auto before = parent.front();
    LexList* start = lex;
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (ISID(lex))
    {
        lex = getsym();
        if (MATCHKW(lex, Keyword::colon_))
        {
            lex = backupsym();
            ParseLabel(parent);
            before->needlabel = false;
            before->nosemi = true;
            return;
        }
        else
        {
            lex = backupsym();
        }
    }
    if (before->needlabel && !MATCHKW(lex, Keyword::case_) && !MATCHKW(lex, Keyword::default_) && !MATCHKW(lex, Keyword::begin_))
        error(ERR_UNREACHABLE_CODE);

    if (!MATCHKW(lex, Keyword::begin_))
    {
        if (MATCHKW(lex, Keyword::throw_))
            before->needlabel = true;
        else
            before->needlabel = false;
    }
    bool oldExpressionParsing = inFunctionExpressionParsing;
    inFunctionExpressionParsing = false;
    before->nosemi = false;
    expressionStatements.push(&parent);
    switch (KW(lex))
    {
        case Keyword::try_:
            canFallThrough = false;
            ParseTry(parent);
            break;
        case Keyword::catch_:
            canFallThrough = false;
            error(ERR_CATCH_WITHOUT_TRY);
            ParseCatch(parent, 1, 1, 1);
            break;
        case Keyword::begin_: {
            canFallThrough = parent.front()->type == Keyword::switch_;
            Compound(parent, false);
            before->nosemi = true;
            auto il = parent.begin();
            ++il;
            auto next = il != parent.end() ? *il : nullptr;
            if (next)
                next->nosemi = true;
            break;
        }
        case Keyword::end_:
            /* don't know how it could get here :) */
            //			error(ERR_UNEXPECTED_END_OF_BLOCKDATA);
            //			lex = getsym();
            canFallThrough = false;
            before->hassemi = true;
            expressionStatements.pop();
            inFunctionExpressionParsing = oldExpressionParsing;
            return;
        case Keyword::do_:
            canFallThrough = false;
            ParseDo(parent);
            break;
        case Keyword::while_:
            canFallThrough = false;
            ParseWhile(parent);
            expressionStatements.pop();
            inFunctionExpressionParsing = oldExpressionParsing;
            return;
        case Keyword::for_:
            canFallThrough = false;
            ParseFor(parent);
            expressionStatements.pop();
            inFunctionExpressionParsing = oldExpressionParsing;
            return;
        case Keyword::switch_:
            ParseSwitch(parent);
            break;
        case Keyword::if_:
            canFallThrough = false;
            ParseIf(parent);
            break;
        case Keyword::else_:
            canFallThrough = false;
            error(ERR_MISPLACED_ELSE);
            skip(&lex, Keyword::else_);
            before->nosemi = true;
            break;
        case Keyword::case_:
            EndOfCaseGroup(parent);
            while (KW(lex) == Keyword::case_)
            {
                if (Optimizer::cparams.prm_cplusplus)
                    HandleEndOfCase(parent);
                ParseCase(parent);
                if (Optimizer::cparams.prm_cplusplus)
                    HandleStartOfCase(parent);
            }
            StartOfCaseGroup(parent);
            StatementWithoutNonconst(parent, false);
            before->nosemi = true;
            expressionStatements.pop();
            inFunctionExpressionParsing = oldExpressionParsing;
            return;
        case Keyword::default_:
            EndOfCaseGroup(parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            ParseDefault(parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleStartOfCase(parent);
            StartOfCaseGroup(parent);
            StatementWithoutNonconst(parent, false);
            before->nosemi = true;
            expressionStatements.pop();
            inFunctionExpressionParsing = oldExpressionParsing;
            return;
        case Keyword::continue_:
            ParseContinue(parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::break_:
            ParseBreak(parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::goto_:
            ParseGoto(parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::return_:
            ParseReturn(parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::semicolon_:
            canFallThrough = false;
            break;
        case Keyword::asm_:
            canFallThrough = false;
            ParseAsm(parent);
            expressionStatements.pop();
            inFunctionExpressionParsing = oldExpressionParsing;
            return;
        case Keyword::seh_try_:
            canFallThrough = false;
            lex = ParseSEH(lex, funcsp, parent);
            break;
        case Keyword::seh_catch_:
        case Keyword::seh_finally_:
        case Keyword::seh_fault_:
            canFallThrough = false;
            error(ERR_SEH_HANDLER_WITHOUT_TRY);
            lex = ParseSEH(lex, funcsp, parent);
            break;
        default: {
            canFallThrough = false;
            bool structured = false;

            if (MATCHKW(lex, Keyword::typedef_) || ((TypeGenerator::StartOfType(lex, &structured, false) &&
                  ((!Optimizer::cparams.prm_cplusplus &&
                    ((Optimizer::architecture != ARCHITECTURE_MSIL) || !Optimizer::cparams.msilAllowExtensions)) ||
                   StatementGenerator::ResolvesToDeclaration(lex, structured)))) ||
                MATCHKW(lex, Keyword::namespace_) || MATCHKW(lex, Keyword::using_) || MATCHKW(lex, Keyword::constexpr_) || MATCHKW(lex, Keyword::decltype_) ||
                MATCHKW(lex, Keyword::static_assert_))
            {
                RequiresDialect::Feature(Dialect::c99, "Intermingled declarations");
                if (viacontrol)
                {
                    AllocateLocalContext(parent, funcsp, codeLabel++);
                }
                while (MATCHKW(lex, Keyword::typedef_) || ((TypeGenerator::StartOfType(lex, &structured, false) &&
                         ((!Optimizer::cparams.prm_cplusplus && (Optimizer::architecture != ARCHITECTURE_MSIL)) ||
                          StatementGenerator::ResolvesToDeclaration(lex, structured)))) ||
                       MATCHKW(lex, Keyword::namespace_) || MATCHKW(lex, Keyword::using_) || MATCHKW(lex, Keyword::decltype_) ||
                       MATCHKW(lex, Keyword::static_assert_) || MATCHKW(lex, Keyword::constexpr_))
                {
                    std::list<Statement*>* prev = before->statements;
                    before->statements = nullptr;
                    declareAndInitialize = false;
                    if (start)
                    {
                        lex = prevsym(start);
                        start = nullptr;
                    }
                    lex = declare(lex, funcsp, nullptr, StorageClass::auto_, Linkage::none_, parent, false, false, false, AccessLevel::public_);
                    // this originally did the last existing statement as well
                    if (before->statements)
                    {
                        MarkInitializersAsDeclarations(*before->statements);
                        if (prev)
                        {
                            auto temp = before->statements;
                            before->statements = prev;
                            before->statements->insert(before->statements->end(), temp->begin(), temp->end());
                        }
                    }
                    else
                    {
                        before->statements = prev;
                    }
                    if (MATCHKW(lex, Keyword::semicolon_))
                    {
                        before->hassemi = true;
                        skip(&lex, Keyword::semicolon_);
                    }
                    else
                        break;
                }
                if (viacontrol)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                }
                expressionStatements.pop();
                inFunctionExpressionParsing = oldExpressionParsing;
                return;
            }
            else
            {
                isCallNoreturnFunction = false;
                inFunctionExpressionParsing = true;
                ParseExpr(parent);
                inFunctionExpressionParsing = false;
                before->needlabel = isCallNoreturnFunction;
            }
        }
    }
    if (before->nosemi && expressionReturns.size())
        expressionReturns.back() = std::move(std::pair<EXPRESSION*, Type*>(nullptr, &stdvoid));
    expressionStatements.pop();
    if (MATCHKW(lex, Keyword::semicolon_))
    {
        before->hassemi = true;
        skip(&lex, Keyword::semicolon_);
    }
    else
        before->hassemi = false;
    basisAttribs = {};
    inFunctionExpressionParsing = oldExpressionParsing;
}
void StatementGenerator::StatementWithoutNonconst(std::list<FunctionBlock*>& parent, bool viacontrol)
{
    int old = funcsp->sb->nonConstVariableUsed;
    SingleStatement(parent, viacontrol);
    funcsp->sb->nonConstVariableUsed = old;
}
bool StatementGenerator::ThunkReturnInMain(std::list<FunctionBlock*>& parent, bool always)
{
    if (always || (!strcmp(funcsp->name, "main") && !funcsp->sb->parentClass && !funcsp->sb->parentNameSpace))
    {
        Statement* s = Statement::MakeStatement(nullptr, parent, StatementNode::return_);
        s->select = MakeIntExpression(ExpressionNode::c_i_, 0);
        return true;
    }
    return false;
}
void StatementGenerator::ReturnThIsPtr(std::list<Statement*>* st, EXPRESSION* thisptr)
{
    for (auto stmt : *st)
    {
        if (stmt->lower)
            ReturnThIsPtr(stmt->lower, thisptr);
        if (stmt->type == StatementNode::return_)
            stmt->select = thisptr;
    }
}
bool StatementGenerator::IsReturnTypeVoid(Type* tp)
{
    if (tp->IsVoid())
        return true;
    if (tp->type == BasicType::templateparam_)
    {
        if (tp->templateParam->second->byClass.val)
            return tp->templateParam->second->byClass.val->IsVoid();
        if (tp->templateParam->second->byClass.dflt)
        {
            std::list<TEMPLATEPARAMPAIR> temp;
            temp.push_back(*tp->templateParam);
            auto second = Allocate<TEMPLATEPARAM>();
            *second = *tp->templateParam->second;
            std::list<TEMPLATEPARAMPAIR> param;
            param.push_back(TEMPLATEPARAMPAIR{ tp->templateParam->first, second });
            if (TemplateParseDefaultArgs(funcsp, nullptr, &param, &temp, &temp))
            {
                if (param.front().second->byClass.val)
                {
                    tp = param.front().second->byClass.val;
                    return tp->IsVoid();
                }
            }
        }
    }
    return false;
}
void StatementGenerator::Compound(std::list<FunctionBlock*>& parent, bool first)
{
    auto before = parent.front();
    FunctionBlock* blockstmt = Allocate<FunctionBlock>();
    preProcessor->MarkStdPragma();
    Statement* st;
    EXPRESSION* thisptr = nullptr;
    browse_blockstart(lex->data->errline);
    blockstmt->type = Keyword::begin_;
    blockstmt->needlabel = before->needlabel;
    blockstmt->table = localNameSpace->front()->syms;
    parent.push_front(blockstmt);
    currentLineData(parent, lex, 0);
    AllocateLocalContext(parent, funcsp, codeLabel++);
    before->needlabel = false;

    if (first)
    {
        int n = 1;
        browse_startfunc(funcsp, funcsp->sb->declline);
        for (auto sp2 : *funcsp->tp->BaseType()->syms)
        {
            if (!Optimizer::cparams.prm_cplusplus && Optimizer::cparams.c_dialect < Dialect::c2x && sp2->tp->type != BasicType::ellipse_ && !sp2->tp->IsVoid() && sp2->sb->anonymous)
                errorarg(ERR_PARAMETER_MUST_HAVE_NAME, n, sp2, funcsp);
            sp2->sb->destructed = false;
            localNameSpace->front()->syms->Add(sp2);
            browse_variable(sp2);
            n++;
        }
        if (Optimizer::cparams.prm_cplusplus && funcsp->sb->isConstructor && funcsp->sb->parentClass)
        {
            ParseMemberInitializers(funcsp->sb->parentClass, funcsp);
            thisptr =
                thunkConstructorHead(parent, funcsp->sb->parentClass, funcsp, funcsp->tp->BaseType()->syms, true, false, false);
        }
    }
    lex = getsym(); /* past { */

    if (blockstmt->statements)
        st = blockstmt->statements->back();
    else
        st = nullptr;
    if (!Optimizer::cparams.prm_cplusplus)
    {
        auto prev = blockstmt->statements;
        std::list<FunctionBlock*> tempBlockData = {blockstmt};
        expressionStatements.push(&tempBlockData);
        // have to defer so we can get expression like constructor calls
        while (TypeGenerator::StartOfType(lex, nullptr, false))
        {
            blockstmt->statements = nullptr;
            declareAndInitialize = false;
            lex = declare(lex, funcsp, nullptr, StorageClass::auto_, Linkage::none_, parent, false, false, false, AccessLevel::public_);
            if (blockstmt->statements)
            {
                MarkInitializersAsDeclarations(*blockstmt->statements);
                if (prev)
                    prev->insert(prev->end(), blockstmt->statements->begin(), blockstmt->statements->end());
                else
                    prev = blockstmt->statements;
            }
            if (MATCHKW(lex, Keyword::semicolon_))
            {
                lex = getsym();
            }
            else
            {
                error(ERR_DECLARE_SYNTAX);
            }
        }
        blockstmt->statements = prev;
        expressionStatements.pop();
    }
    if (before->type == Keyword::switch_)
    {
        if (st != blockstmt->statements->back())
        {
            /* kinda naive other than this one check for msil... */
            bool toErr = false;
            // this only checked the last block...
            /*
            st = blockstmt->tail;
            while (st)
            {
                if (st->select->type != ExpressionNode::initobj_ && st->type != StatementNode::varstart_)
                {
                    toErr = true;
                }
                st = st->next;
            }
            */
            st = blockstmt->statements->back();
            if (st->select->type != ExpressionNode::initobj_ && st->type != StatementNode::varstart_)
            {
                toErr = true;
            }
            if (toErr)
            {
                error(ERR_INITIALIZATION_MAY_BE_BYPASSED);
            }
        }
    }
    currentLineData(parent, lex, -1);
    blockstmt->nosemi = true; /* in case it is an empty body */
    while (lex && !MATCHKW(lex, Keyword::end_))
    {
        if (!blockstmt->hassemi && !blockstmt->nosemi)
            errorint(ERR_NEEDY, ';');
        if (MATCHKW(lex, Keyword::semicolon_))
        {
            lex = getsym();  // helps in error processing to not do default statement processing here...
        }
        else
        {
            SingleStatement(parent, false);
        }
    }
    if (first)
    {
        browse_endfunc(funcsp, funcsp->sb->endLine = lex ? lex->data->errline : endline);
    }
    if (!lex)
    {
        parent.pop_front();
        needkw(&lex, Keyword::end_);
        if (expressionReturns.size())
            expressionReturns.pop_back();
        
        return;
    }
    browse_blockend(endline = lex->data->errline);
    currentLineData(parent, lex, -!first);
    if (before->type == Keyword::begin_ || before->type == Keyword::switch_ || before->type == Keyword::try_ || before->type == Keyword::catch_ ||
        before->type == Keyword::do_)
        before->needlabel = blockstmt->needlabel;
    if (!blockstmt->hassemi && !blockstmt->nosemi)
    {
        errorint(ERR_NEEDY, ';');
    }
    if (blockstmt->statements)
    {
        bool hasvla = false;
        for (auto stmt : *blockstmt->statements)
        {
            hasvla |= st->hasvla;
        }
        if (hasvla)
        {
            if (first)
            {
                funcsp->sb->allocaUsed = true;
            }
        }
    }
    if (first && Optimizer::cparams.prm_cplusplus)
    {
        if (functionCanThrow)
        {
            if (funcsp->sb->xcMode == xc_none || funcsp->sb->xcMode == xc_dynamic)
            {
                hasXCInfo = true;
            }
        }
        else if (funcsp->sb->xcMode == xc_dynamic && funcsp->sb->xc && funcsp->sb->xc->xcDynamic)
            hasXCInfo = true;
        if (hasXCInfo && Optimizer::cparams.prm_xcept)
        {
            if (funcsp->sb->anyTry)
            {
                Optimizer::SymbolManager::Get(funcsp)->anyTry = true;
                funcsp->sb->noinline = true;
            }
            insertXCInfo(funcsp);
        }
        if (!strcmp(funcsp->name, overloadNameTab[CI_DESTRUCTOR]))
        {
            thunkDestructorTail(parent, funcsp->sb->parentClass, funcsp, funcsp->tp->BaseType()->syms, false);
        }
    }
    if (Optimizer::cparams.prm_cplusplus)
        HandleEndOfSwitchBlock(parent);
    FreeLocalContext(parent, funcsp, codeLabel++);
    if (first && !blockstmt->needlabel && !IsReturnTypeVoid(funcsp->tp->BaseType()->btp) &&
        funcsp->tp->BaseType()->btp->type != BasicType::auto_ && !funcsp->sb->isConstructor && funcsp->tp->BaseType()->btp->type != BasicType::void_)
    {
        if (funcsp->sb->attribs.inheritable.linkage3 == Linkage::noreturn_)
        {
            // explicitly do nothing, refer to https://github.com/LADSoft/OrangeC/issues/651 for more information
            // Keeping this here prevents nonsensical errors such as "FUNCTION SHOULD RETURN VALUE!!!!" when a function is marked
            // noreturn. Noreturn functions can have non-void return types in order for things to work such as in ObjIeee.h's ThrowSyntax functions
        }
        else if (Optimizer::cparams.c_dialect >= Dialect::c99 || Optimizer::cparams.prm_cplusplus)
        {
            if (!ThunkReturnInMain(parent, false))
            {
                if (funcsp->tp->BaseType()->btp->IsRef())
                    error(ERR_FUNCTION_RETURNING_REF_SHOULD_RETURN_VALUE);
                else
                {
                    error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
                    if (Optimizer::chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                        ThunkReturnInMain(parent, true);
                }
            }
        }
        else
        {
            error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
            if (Optimizer::chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                ThunkReturnInMain(parent, true);
        }
    }
    needkw(&lex, Keyword::end_);
    if (first && Optimizer::cparams.prm_cplusplus)
    {
        if (funcsp->sb->hasTry)
        {
            Statement::MakeStatement(nullptr, parent, StatementNode::return_);
            auto it = blockstmt->statements->end();
            --it;
            ParseCatch(parent, retlab, startlab, 0);
            if (++it != blockstmt->statements->end())
            {
                int label = (*it)->altlabel;
                do
                {
                    (*it)->endlabel = label;
                } while (++it != blockstmt->statements->end());
            }
            hasXCInfo = true;
            funcsp->sb->anyTry = true;
        }
    }
    if (before->type == Keyword::catch_)
    {
        SYMBOL* sym = namespacesearch("_CatchCleanup", globalNameSpace, false, false);
        if (sym)
        {
            CallSite* funcparams = Allocate<CallSite>();
            Statement* st = Statement::MakeStatement(lex, parent, StatementNode::expr_);
            Argument* arg1 = Allocate<Argument>();  // exception table
            makeXCTab(funcsp);
            sym = (SYMBOL*)sym->tp->BaseType()->syms->front();
            funcparams->ascall = true;
            funcparams->sp = sym;
            funcparams->functp = sym->tp;
            funcparams->fcall = MakeExpression(ExpressionNode::pc_, sym);
            funcparams->arguments = argumentListFactory.CreateList();
            funcparams->arguments->push_back(arg1);
            arg1->exp = MakeExpression(ExpressionNode::auto_, funcsp->sb->xc->xctab);
            arg1->tp = &stdpointer;
            st->select = MakeExpression(funcparams);
        }
    }
    if (first)
    {
        if (thisptr)
        {
            Statement::MakeStatement(nullptr, parent, StatementNode::return_);
            ReturnThIsPtr(blockstmt->statements, thisptr);
        }
    }
    auto it = parent.begin();
    ++it;
    std::list<FunctionBlock*> dummy(it, parent.end());
    parent.pop_front();
    blockstmt->AddThis(lex, parent);
    preProcessor->ReleaseStdPragma();
}
void StatementGenerator::AssignParam(SYMBOL* funcsp, int* base, SYMBOL* param)
{
    Type* tp = param->tp->BaseType();
    param->sb->parent = funcsp;
    if (tp->type == BasicType::ellipse_)
        ellipsePos = *base;
    if (tp->type == BasicType::void_)
        return;
    if (tp->IsStructured() && !tp->BaseType()->sp->sb->pureDest)
        hasXCInfo = true;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
    {
        // calculate index for CIL
        param->sb->offset = (*base)++;
    }
    else if (!tp->IsPtr() && tp->size <= Optimizer::chosenAssembler->arch->parmwidth)
    {
        param->sb->offset = *base + funcvaluesize(tp->size);
        *base += Optimizer::chosenAssembler->arch->parmwidth;
    }
    else
    {
        param->sb->offset = *base;
        if (tp->type == BasicType::pointer_)
        {
            if (!tp->vla)
                *base += getSize(BasicType::pointer_);
            else
            {
                *base += tp->size;
            }
        }
        else
            *base += tp->size;
        if (*base % Optimizer::chosenAssembler->arch->parmwidth)
            *base += Optimizer::chosenAssembler->arch->parmwidth - *base % Optimizer::chosenAssembler->arch->parmwidth;
    }
}
void StatementGenerator::AssignCParams(int* base, SymbolTable<SYMBOL>* params, Type* rv, std::list<FunctionBlock*>& block)
{
    (void)rv;
    for (auto sym : *params)
    {
        Statement* s = Statement::MakeStatement(lex, block, StatementNode::varstart_);
        s->select = MakeExpression(ExpressionNode::auto_, sym);
        StatementGenerator::AssignParam(funcsp, base, sym);
    }
}
void StatementGenerator::AssignPascalParams(int* base, SymbolTable<SYMBOL>* params, Type* rv, std::list<FunctionBlock*>& block)
{
    std::stack<SYMBOL*> stk;
    for (auto sym : *params)
        stk.push(sym);
    while (!stk.empty())
    {
        StatementGenerator::AssignParam(funcsp, base, stk.top());
        Statement* s = Statement::MakeStatement(lex, block, StatementNode::varstart_);
        s->select = MakeExpression(ExpressionNode::auto_, stk.top());
        stk.pop();
    }
}
void StatementGenerator::AssignParameterSizes(std::list<FunctionBlock*>& block)
{
    int base;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
        base = 0;
    else
        base = Optimizer::chosenAssembler->arch->retblocksize;
    if (funcsp->sb->attribs.inheritable.linkage == Linkage::pascal_)
    {
        AssignPascalParams(&base, funcsp->tp->BaseType()->syms, funcsp->tp->BaseType()->btp, block);
    }
    else
    {
        if ((funcsp->tp->BaseType()->btp->IsStructured() && !funcsp->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType) || funcsp->tp->BaseType()->btp->BaseType()->type == BasicType::memberptr_ || funcsp->tp->BaseType()->btp->IsBitInt())
        {
            // handle structured return values
            if (Optimizer::chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
            {
                if (funcsp->sb->attribs.inheritable.linkage2 == Linkage::unmanaged_ || (Optimizer::architecture != ARCHITECTURE_MSIL) ||
                    msilManaged(funcsp))
                    base++;
            }
            else
            {
                base += getSize(BasicType::pointer_);
                if (base % Optimizer::chosenAssembler->arch->parmwidth)
                    base += Optimizer::chosenAssembler->arch->parmwidth - base % Optimizer::chosenAssembler->arch->parmwidth;
            }
        }
        /*
        if (ismember(funcsp))
        {
            // handle 'this' pointer
            StatementGenerator::AssignParam(&base, (SYMBOL*)params->p);
            params = params->next;
        }
        */
        AssignCParams(&base, funcsp->tp->BaseType()->syms, funcsp->tp->BaseType()->btp, block);
    }
    funcsp->sb->paramsize = base - Optimizer::chosenAssembler->arch->retblocksize;
}
int StatementGenerator::CountInlineStatements(std::list<Statement*>* block)
{
    int rv = 0;
    if (block)
    {
        for (auto stmt : *block)
        {
            switch (stmt->type)
            {
                case StatementNode::genword_:
                    break;
                case StatementNode::try_:
                case StatementNode::catch_:
                case StatementNode::seh_try_:
                case StatementNode::seh_catch_:
                case StatementNode::seh_finally_:
                case StatementNode::seh_fault_:
                    rv += 1000;
                    break;
                case StatementNode::return_:
                case StatementNode::expr_:
                case StatementNode::declare_:
                    rv++;
                    break;
                case StatementNode::goto_:
                case StatementNode::label_:
                    rv++;
                    break;
                case StatementNode::select_:
                case StatementNode::notselect_:
                    rv++;
                    break;
                case StatementNode::switch_:
                    rv++;
                    rv += CountInlineStatements(stmt->lower);
                    break;
                case StatementNode::block_:
                    rv++;
                    rv += CountInlineStatements(stmt->lower) + CountInlineStatements(stmt->blockTail);
                    break;
                case StatementNode::passthrough_:
                case StatementNode::nop_:
                    break;
                case StatementNode::datapassthrough_:
                    break;
                case StatementNode::line_:
                case StatementNode::varstart_:
                case StatementNode::dbgblock_:
                    break;
                default:
                    diag("Invalid stmt type in inlinestmtCount");
                    break;
            }
        }
    }
    return rv;
}
void StatementGenerator::HandleInlines()
{
    /* so it will get instantiated as a virtual function */
    if (!funcsp->sb->attribs.inheritable.isInline)
        return;
    if (Optimizer::cparams.c_dialect >= Dialect::c99)
        funcsp->sb->attribs.inheritable.used = true;
    /* this unqualified the current function if it has structured
     * args or return value, or if it has nested declarations
     */
    {
        if (funcsp->sb->inlineFunc.syms)
        {
            SymbolTable<SYMBOL>* ht = funcsp->sb->inlineFunc.syms->Next(); /* past params */
            if (ht)
                ht = ht->Next(); /* past first level */
            /* if any vars declared at another level */
            while (ht && ht->Next())
            {
                if (ht->size())
                {
                    funcsp->sb->noinline = true;
                    break;
                }
                ht = ht->Next();
            }
            if (funcsp->sb->inlineFunc.syms->Next())
            {
                if (funcsp->sb->inlineFunc.syms->Next()->size())
                    funcsp->sb->noinline = true;
            }
        }
        if (funcsp->sb->attribs.inheritable.linkage4 == Linkage::virtual_)
        {

            for (auto head : *funcsp->tp->BaseType()->syms)
            {
                if (head->tp->IsStructured())
                {
                    funcsp->sb->noinline = true;
                    break;
                }
            }
        }
        if (CountInlineStatements(funcsp->sb->inlineFunc.stmt) > 100)
        {
            funcsp->sb->noinline = true;
        }
    }
}
void StatementGenerator::ParseNoExceptClause(SYMBOL* funcsp)
{
    if (funcsp->sb->deferredNoexcept && funcsp->sb->deferredNoexcept != (LexList*)-1)
    {
        dontRegisterTemplate++;
        LexList* lex = SetAlternateLex(funcsp->sb->deferredNoexcept);
        int n = PushTemplateNamespace(funcsp);
        enclosingDeclarations.Mark();
        if (funcsp->sb->parentClass)
        {
            enclosingDeclarations.Add(funcsp->sb->parentClass);
            auto tpl = funcsp->sb->parentClass->templateParams;
            if (tpl)
                enclosingDeclarations.Add(tpl);
        }
        Type* tp = nullptr;
        EXPRESSION* exp = nullptr;
        AllocateLocalContext(emptyBlockdata, nullptr, 0);
        for (auto sp2 : *funcsp->tp->BaseType()->syms)
        {
            localNameSpace->front()->syms->Add(sp2);
        }
        lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
        FreeLocalContext(emptyBlockdata, nullptr, 0);
        if (!IsConstantExpression(exp, false, false))
        {
            if (!definingTemplate)
                error(ERR_CONSTANT_VALUE_EXPECTED);
        }
        else
        {
            funcsp->sb->xcMode = exp->v.i ? xc_none : xc_all;
            if (exp->v.i)
                funcsp->sb->noExcept = true;
        }
        SetAlternateLex(nullptr);
        enclosingDeclarations.Release();
        PopTemplateNamespace(n);
        funcsp->sb->deferredNoexcept = (LexList*)-1;
        dontRegisterTemplate--;
    }
}
void StatementGenerator::Body()
{
    int oldNoexcept = funcsp->sb->noExcept;
    if (bodyIsDestructor)
        funcsp->sb->noExcept = true;
    if (funcsp->sb->isDestructor)
        bodyIsDestructor++;
    int oldNestingCount = definingTemplate;
    int n1;
    bool oldsetjmp_used = Optimizer::setjmp_used;
    bool oldfunctionHasAssembly = Optimizer::functionHasAssembly;
    bool oldDeclareAndInitialize = declareAndInitialize;
    bool oldHasXCInfo = hasXCInfo;
    bool oldFunctionCanThrow = functionCanThrow;
    SymbolTable<SYMBOL>* oldSyms = localNameSpace->front()->syms;
    SymbolTable<SYMBOL>* oldLabelSyms = labelSyms;
    SYMBOL* oldtheCurrentFunc = theCurrentFunc;
    FunctionBlock* block = Allocate<FunctionBlock>();
    std::list<FunctionBlock*> parent { block };
    std::list<Statement*>* startStmt;
    int oldCodeLabel = codeLabel;
    int oldMatchReturnTypes = matchReturnTypes;
    bool oldHasFuncCall = hasFuncCall;
    int oldExpressionCount = expressions;
    int oldControlSequences = controlSequences;
    bool oldNoExcept = noExcept;
    int oldEllipsePos = ellipsePos;
    ellipsePos = 0;
    noExcept = true;
    expressions = 0;
    controlSequences = 0;
    auto oldGlobal = globalNameSpace->front()->usingDirectives;
    hasFuncCall = false;
    funcNesting++;
    funcLevel++;
    functionCanThrow = false;
    codeLabel = INT_MIN;
    hasXCInfo = false;
    localNameSpace->front()->syms = nullptr;
    Optimizer::functionHasAssembly = false;
    Optimizer::setjmp_used = false;
    declareAndInitialize = false;
    block->type = funcsp->sb->hasTry ? Keyword::try_ : Keyword::begin_;
    theCurrentFunc = funcsp;


    if (Optimizer::cparams.prm_cplusplus)
    {
        InitializeFunctionArguments(funcsp, true);
    }

    if (inTemplateHeader)
        definingTemplate--;
    CheckUndefinedStructures(funcsp);
    StatementGenerator::ParseNoExceptClause(funcsp);
    if (!inNoExceptHandler)
    {
        FlushLineData(funcsp->sb->declfile, funcsp->sb->realdeclline);
        if (!funcsp->sb->linedata)
        {
            startStmt = currentLineData(emptyBlockdata, lex, 0);
            if (startStmt)
                funcsp->sb->linedata = startStmt->front()->lineData;
        }
        funcsp->sb->declaring = true;
        labelSyms = symbols.CreateSymbolTable();
        StatementGenerator::AssignParameterSizes(parent);
        funcsp->sb->startLine = lex->data->errline;
        Compound(parent, true);
        if (funcsp->tp->BaseType()->btp->IsStructured())
            StatementGenerator::AssignParameterSizes(parent);
        refreshBackendParams(funcsp);
        checkUnlabeledReferences(parent);
        checkGotoPastVLA(block->statements, true);
        if (funcsp->tp->BaseType()->btp->IsAutoType() && !definingTemplate)
            funcsp->tp->BaseType()->btp = &stdvoid;  // return value for auto function without return statements
        if (Optimizer::cparams.prm_cplusplus)
        {
            if ((!oldNoexcept || funcsp->sb->isDestructor) && funcsp->sb->noExcept && !funcsp->sb->deferredNoexcept && !noExcept)
            {
                // destructor needs to be demoted
                funcsp->sb->noExcept = false;
                funcsp->sb->xcMode = xc_unspecified;
            }
            if (!funcsp->sb->constexpression)
                funcsp->sb->nonConstVariableUsed = true;
            else
                funcsp->sb->nonConstVariableUsed |= !MatchesConstFunction(funcsp);

            if (funcsp->sb->xcMode == xc_none && !funcsp->sb->anyTry && !hasFuncCall)
            {
                funcsp->sb->xcMode = xc_unspecified;
                Optimizer::SymbolManager::Get(funcsp)->xc = false;
                funcsp->sb->xc = nullptr;
                if (funcsp->sb->mainsym)
                {
                    funcsp->sb->mainsym->sb->xcMode = xc_unspecified;
                    funcsp->sb->mainsym->sb->xc = nullptr;
                }
                hasXCInfo = false;
            }
            funcsp->sb->canThrow = functionCanThrow;
        }
        Optimizer::SymbolManager::Get(funcsp)->ellipsePos = ellipsePos;
        if (expressions <= MAX_INLINE_EXPRESSIONS && !controlSequences)
        {
            funcsp->sb->simpleFunc = true;
        }
        funcsp->sb->labelCount = codeLabel - INT_MIN;
        n1 = codeLabel;
        if (!funcsp->sb->templateLevel || funcsp->sb->instantiated)
        {
            funcsp->sb->inlineFunc.stmt = stmtListFactory.CreateList();
            funcsp->sb->inlineFunc.stmt->push_back(Statement::MakeStatement(lex, emptyBlockdata, StatementNode::block_));
            funcsp->sb->inlineFunc.stmt->front()->lower = block->statements;
            funcsp->sb->inlineFunc.stmt->front()->blockTail = block->blockTail;
            funcsp->sb->declaring = false;
            if (funcsp->sb->attribs.inheritable.isInline &&
                (Optimizer::functionHasAssembly || funcsp->sb->attribs.inheritable.linkage2 == Linkage::export_))
                funcsp->sb->attribs.inheritable.isInline = funcsp->sb->promotedToInline = false;
            if (!Optimizer::cparams.prm_allowinline)
                funcsp->sb->attribs.inheritable.isInline = funcsp->sb->promotedToInline = false;
            // if it is variadic don't allow it to be inline
            if (funcsp->sb->attribs.inheritable.isInline)
            {
                if (funcsp->tp->BaseType()->syms->size())
                {
                    if (funcsp->tp->BaseType()->syms->back()->tp->type == BasicType::ellipse_)
                        funcsp->sb->attribs.inheritable.isInline = funcsp->sb->promotedToInline =
                            false;
                }
            }
        }
        if (IsCompiler() && funcNesting == 1)  // top level function
            localFree();
        HandleInlines();
    }
    ellipsePos = oldEllipsePos;
    noExcept = oldNoExcept;
    controlSequences = oldControlSequences;
    expressions = oldExpressionCount;
    globalNameSpace->front()->usingDirectives = oldGlobal;
    hasFuncCall = oldHasFuncCall;
    declareAndInitialize = oldDeclareAndInitialize;
    theCurrentFunc = oldtheCurrentFunc;
    hasXCInfo = oldHasXCInfo;
    Optimizer::setjmp_used = oldsetjmp_used;
    Optimizer::functionHasAssembly = oldfunctionHasAssembly;
    localNameSpace->front()->syms = oldSyms;
    labelSyms = oldLabelSyms;
    codeLabel = oldCodeLabel;
    functionCanThrow = oldFunctionCanThrow;
    matchReturnTypes = oldMatchReturnTypes;
    funcLevel--;
    funcNesting--;
    definingTemplate = oldNestingCount;
    if (funcsp->sb->isDestructor)
        bodyIsDestructor--;
}
void StatementGenerator::BodyGen()
{
    if (funcsp->sb->inlineFunc.stmt)
    {
        if (funcsp->sb->attribs.inheritable.linkage4 == Linkage::virtual_ || (funcsp->sb->attribs.inheritable.isInline && !funcsp->sb->promotedToInline))
        {
            if (funcsp->sb->attribs.inheritable.isInline)
                funcsp->sb->attribs.inheritable.linkage2 = Linkage::none_;
            if (!Optimizer::cparams.prm_cplusplus && funcsp->sb->storage_class != StorageClass::static_)
                Optimizer::SymbolManager::Get(funcsp);
        }
        else if (!funcsp->sb->constexpression && IsCompiler())
        {
            bool isTemplate = false;
            SYMBOL* spt = funcsp;
            while (spt && !isTemplate)
            {
                if (spt->sb->templateLevel)
                    isTemplate = true;
                else
                    spt = spt->sb->parentClass;
            }
            if (!isTemplate)
            {
                if (!TotalErrors())
                {
                    int oldstartlab = startlab;
                    int oldretlab = retlab;
                    startlab = Optimizer::nextLabel++;
                    retlab = Optimizer::nextLabel++;
                    genfunc(funcsp, true);
                    retlab = oldretlab;
                    startlab = oldstartlab;
                }
            }
        }
    }
}
}  // namespace Parser
