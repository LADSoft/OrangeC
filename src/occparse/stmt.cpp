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
#include "template.h"
#include "rtti.h"
#include "ildata.h"
#include "cppbltin.h"
#include "expr.h"
#include "help.h"
#include "declcons.h"
#include "wseh.h"
#include "cpplookup.h"
#include "init.h"
#include "lex.h"
#include "declcpp.h"
#include "constopt.h"
#include "OptUtils.h"
#include "using.h"
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

std::list<BLOCKDATA*> emptyBlockdata;

std::stack<std::list<BLOCKDATA*>*> expressionStatements;
std::deque<std::pair<EXPRESSION*, TYPE*>> expressionReturns;

std::list<Optimizer::LINEDATA*>* lines;

static int matchReturnTypes;
static int endline;
static int caseLevel = 0;
static int controlSequences;
static int expressions;
static bool canFallThrough;

static LEXLIST* autodeclare(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, std::list<BLOCKDATA*>& parent, int asExpression);
static LEXLIST* nononconststatement(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent, bool viacontrol);

static BLOCKDATA* caseDestructBlock;

void statement_ini(bool global)
{
    lines = nullptr;
    functionCanThrow = false;
    funcNesting = 0;
    funcLevel = 0;
    caseDestructBlock = nullptr;
    caseLevel = 0;
    matchReturnTypes = false;
    tryLevel = 0;
    controlSequences = 0;
    expressions = 0;
    inLoopOrConditional = 0;
    bodyIsDestructor = 0;
    while (!expressionStatements.empty())
        expressionStatements.pop();
}
bool msilManaged(SYMBOL* s)
{
    if (IsCompiler())
        return occmsil::msil_managed(Optimizer::SymbolManager::Get(s));
    return false;
}

void InsertLineData(int lineno, int fileindex, const char* fname, char* line)
{
    if (!lines)
        lines = lineDataListFactory.CreateList();
    auto ld = Allocate<Optimizer::LINEDATA>();
    ld->file = fname;
    ld->line = litlate(line);
    ld->lineno = lineno;
    ld->fileindex = fileindex;
    lines->push_back(ld);
}
void FlushLineData(const char* file, int lineno)
{
    if (lines)
    {
        while (lines->size())
        {
            if (strcmp(file, lines->front()->file) != 0 || lines->front()->lineno < lineno)
                lines->pop_front();
            else
                break;
        }
    }
}
std::list<STATEMENT*>* currentLineData(std::list<BLOCKDATA*>& parent, LEXLIST* lex, int offset)
{
    if (!lex || !lines)
        return nullptr;
    std::list<STATEMENT*> rv;
    int lineno;
    const char* file;
    lineno = lex->data->linedata->lineno + offset + 1;
    file = lex->data->errfile;
    while (lines->size() && (strcmp(lines->front()->file, file) != 0 || lineno >= lines->front()->lineno))
    {
        rv.push_back(stmtNode(lex, parent, StatementNode::line_));
        rv.back()->lineData = lines->front();
        lines->pop_front();
    }
    if (rv.size())
    {
        auto rva = stmtListFactory.CreateList();
        *rva = rv;
        return rva;
    }
    return nullptr;
}
STATEMENT* stmtNode(LEXLIST* lex, std::list<BLOCKDATA*>& parent, StatementNode stype)
{
    STATEMENT* st = Allocate<STATEMENT>();
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
static void AddBlock(LEXLIST* lex, std::list<BLOCKDATA*>& parent, BLOCKDATA* newbl)
{
    STATEMENT* st = stmtNode(lex, parent, StatementNode::block_);
    st->blockTail = newbl->blockTail;
    st->lower = newbl->statements;
}
static bool isselecttrue(EXPRESSION* exp)
{
    if (isintconst(exp->left))
        return !!exp->left->v.i;
    return false;
}
static bool isselectfalse(EXPRESSION* exp)
{
    if (isintconst(exp))
        return !exp->v.i;
    return false;
}
static void markInitializers(std::list<STATEMENT*>& lst)
{
    for (auto it = lst.begin(); it != lst.end(); ++it)
        if ((*it)->type == StatementNode::expr_)
            (*it)->hasdeclare = true;
}
static LEXLIST* selection_expression(LEXLIST* lex, std::list<BLOCKDATA*>& parent, EXPRESSION** exp, SYMBOL* funcsp, Keyword kw,
                                     bool* declaration)
{
    TYPE* tp = nullptr;
    bool hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, true);
    (void)parent;
    bool structured = false;
    if (startOfType(lex, &structured, false) && (!Optimizer::cparams.prm_cplusplus || resolveToDeclaration(lex, structured)))
    {
        if (declaration)
            *declaration = true;
        if ((Optimizer::cparams.prm_cplusplus && declaration) ||
            (Optimizer::cparams.c_dialect >= Dialect::c99 && (kw == Keyword::for_ || kw == Keyword::rangefor_)))
        {
            // empty
            if (declaration)
                *declaration = true;
        }
        else
        {
            error(ERR_NO_DECLARATION_HERE);
        }
        /* fixme need type */
        lex = autodeclare(lex, funcsp, &tp, exp, parent, (kw != Keyword::for_ && kw != Keyword::if_ && kw != Keyword::switch_) | (kw == Keyword::rangefor_ ? _F_NOCHECKAUTO : 0));
        if (tp->type == BasicType::memberptr_)
        {
            *exp = exprNode(ExpressionNode::mp_as_bool_, *exp, nullptr);
            (*exp)->size = tp;
            tp = &stdint;
        }
    }
    else
    {
        if (hasAttributes)
            error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
        /*		bool openparen = MATCHKW(lex, Keyword::openpa_); */
        if (declaration)
            *declaration = false;
        lex = expression(lex, funcsp, nullptr, &tp, exp, kw != Keyword::for_ && kw != Keyword::rangefor_ ? _F_SELECTOR : 0);
        ConstExprPatch(exp);
        if (tp)
        {
            if (tp->type == BasicType::memberptr_)
            {
                *exp = exprNode(ExpressionNode::mp_as_bool_, *exp, nullptr);
                (*exp)->size = tp;
                tp = &stdint;
            }
            optimize_for_constants(exp);
        }
    }

    if (Optimizer::cparams.prm_cplusplus && tp && isstructured(tp) && kw != Keyword::for_ && kw != Keyword::rangefor_ &&
        ((kw != Keyword::if_ && kw != Keyword::switch_) || !declaration))
    {
        if (!castToArithmeticInternal(false, &tp, exp, (Keyword) - 1, &stdbool, false))
            if (!castToArithmeticInternal(false, &tp, exp, (Keyword) - 1, &stdint, false))
                if (!castToPointer(&tp, exp, (Keyword) - 1, &stdpointer))
                    errorConversionOrCast(true, tp, &stdint);
    }
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    else if (kw == Keyword::switch_ && !isint(tp) && basetype(tp)->type != BasicType::enum_)
        error(ERR_SWITCH_SELECTION_INTEGRAL);
    else if (kw != Keyword::for_ && kw != Keyword::rangefor_ && isstructured(tp))
    {
        error(ERR_ILL_STRUCTURE_OPERATION);
    }
    *exp = exprNode(ExpressionNode::select_, *exp, nullptr);
    GetLogicalDestructors(&(*exp)->v.logicaldestructors.left, *exp);
    return lex;
}
static BLOCKDATA* getCommonParent(std::list<BLOCKDATA*>& src, std::list<BLOCKDATA*>& dest)
{
    BLOCKDATA* rv = nullptr;
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
void makeXCTab(SYMBOL* funcsp)
{
    SYMBOL* sym;
    if (!funcsp->sb->xc)
    {
        funcsp->sb->xc = Allocate<xcept>();
        Optimizer::SymbolManager::Get(funcsp)->xc = true;
    }
    if (!funcsp->sb->xc->xctab)
    {
        sym = makeID(StorageClass::auto_, &stdXC, nullptr, "$$xctab");
        sym->sb->decoratedName = sym->name;
        sym->sb->allocate = true;
        sym->sb->attribs.inheritable.used = sym->sb->assigned = true;
        localNameSpace->front()->syms->Add(sym);
        funcsp->sb->xc->xctab = sym;
    }
}
static void thunkCatchCleanup(STATEMENT* st, SYMBOL* funcsp, std::list<BLOCKDATA*>& src, std::list<BLOCKDATA*>& dest)
{
    BLOCKDATA *top = dest.size() ? getCommonParent(src, dest) : nullptr;
    for (auto srch : src)
    {
        if (srch == top)
            break;
        if (srch->type == Keyword::catch_)
        {
            SYMBOL* sym = namespacesearch("_CatchCleanup", globalNameSpace, false, false);
            if (sym)
            {
                FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                INITLIST* arg1 = Allocate<INITLIST>();  // exception table
                makeXCTab(funcsp);
                sym = (SYMBOL*)basetype(sym->tp)->syms->front();
                funcparams->ascall = true;
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = varNode(ExpressionNode::pc_, sym);
                funcparams->arguments = initListListFactory.CreateList();
                funcparams->arguments->push_back(arg1);
                arg1->exp = varNode(ExpressionNode::auto_, funcsp->sb->xc->xctab);
                arg1->tp = &stdpointer;
                for (auto it = src.front()->statements->begin(); it != src.front()->statements->end(); ++it)
                {
                    if ((*it) == st)
                    {
                        auto next = Allocate<STATEMENT>();
                        *next = *st;
                        next->type = StatementNode::expr_;
                        next->select = exprNode(ExpressionNode::func_, nullptr, nullptr);
                        next->select->v.func = funcparams;
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
static void ThunkUndestructSyms(SymbolTable<SYMBOL>* syms)
{
    for (auto sym : *syms)
    {
        sym->sb->destructed = true;
    }
}
static void thunkRetDestructors(EXPRESSION** exp, SymbolTable<SYMBOL>* top, SymbolTable<SYMBOL>* syms)
{
    if (syms)
    {
        if (syms != top)
        {
            thunkRetDestructors(exp, top, syms->Chain());
            destructBlock(exp, syms, false);
        }
    }
}
static void thunkGotoDestructors(EXPRESSION** exp, std::list<BLOCKDATA*>& gotoTab, std::list<BLOCKDATA*>& labelTab)
{
    // find the common parent
    std::list<BLOCKDATA*>::iterator realtop;
    BLOCKDATA* top = getCommonParent(gotoTab, labelTab);
    auto il = gotoTab.begin();
    if ((*il) != top && (*il)->orig != top)
    {
        realtop = il;
        while ((*il) != top && (*il)->orig != top) ++il;
        thunkRetDestructors(exp, (*il)->table, gotoTab.front()->table);
    }
}
static void InSwitch() {}
static void StartOfCaseGroup(SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    caseLevel++;
    STATEMENT* st = stmtNode(nullptr, parent, StatementNode::dbgblock_);
    st->label = 1;
}
static void EndOfCaseGroup(SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    if (caseLevel)
    {
        caseLevel--;
        STATEMENT* st = stmtNode(nullptr, parent, StatementNode::dbgblock_);
        st->label = 0;
    }
}
static void HandleStartOfCase(std::list<BLOCKDATA*>& parent)
{
    // this is a little buggy in that it doesn't check to see if we are already in a switch
    // statement, however if we aren't we should get a compile erroir that would halt program generation anyway
    if (parent.front() != caseDestructBlock)
    {
        parent.front()->caseDestruct = caseDestructBlock;
        caseDestructBlock = parent.front();
    }
}
static void HandleEndOfCase(std::list<BLOCKDATA*>& parent)
{
    if (parent.front() == caseDestructBlock)
    {
        EXPRESSION* exp = nullptr;
        STATEMENT* st;
        // the destruct is only used for endin
        destructBlock(&exp, localNameSpace->front()->syms, false);
        if (exp)
        {
            st = stmtNode(nullptr, parent, StatementNode::nop_);
            st->destexp = exp;
        }
        ThunkUndestructSyms(localNameSpace->front()->syms);
    }
}
static void HandleEndOfSwitchBlock(std::list<BLOCKDATA*>& parent)
{
    if (parent.front() == caseDestructBlock)
    {
        caseDestructBlock = caseDestructBlock->caseDestruct;
    }
}
static LEXLIST* statement_break(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    std::list<BLOCKDATA*>::iterator breakableStatement, last = parent.end();
    EXPRESSION* exp = nullptr;
    (void)lex;
    (void)funcsp;
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
        STATEMENT* st;
        currentLineData(parent, lex, 0);
        if (last != parent.end())
            thunkRetDestructors(&exp, (*last)->table, localNameSpace->front()->syms);
        st = stmtNode(lex, parent, StatementNode::goto_);
        st->label = (*breakableStatement)->breaklabel;
        st->destexp = exp;
        auto range = std::list<BLOCKDATA*>(breakableStatement, parent.end());
        thunkCatchCleanup(st, funcsp, parent, range);
        parent.front()->needlabel = true;
        (*breakableStatement)->needlabel = false;
        (*breakableStatement)->hasbreak = true;
    }
    return getsym();
}
static LEXLIST* statement_case(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    long long val;
    BLOCKDATA dummy;
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    BLOCKDATA* switchstmt = nullptr;
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
            STATEMENT* st = stmtNode(lex, parent, StatementNode::label_);
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
    return lex;
}
static LEXLIST* statement_continue(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    std::list<BLOCKDATA*>::iterator continuableStatement, last = parent.end();
    EXPRESSION* exp = nullptr;
    (void)lex;
    (void)funcsp;
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
        STATEMENT* st;
        if (last != parent.end())
            thunkRetDestructors(&exp, (*last)->table, localNameSpace->front()->syms);
        currentLineData(parent, lex, 0);
        st = stmtNode(lex, parent, StatementNode::goto_);
        st->label = (*continuableStatement)->continuelabel;
        st->destexp = exp;
        auto range = std::list<BLOCKDATA*>(continuableStatement, parent.end());
        thunkCatchCleanup(st, funcsp, parent, range);
        parent.front()->needlabel = true;
    }
    return getsym();
}
static LEXLIST* statement_default(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    lex = getsym();
    std::list<BLOCKDATA*>::iterator defaultableStatement;
    EXPRESSION* exp = nullptr;
    (void)lex;
    (void)funcsp;
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
        STATEMENT* st = stmtNode(lex, parent, StatementNode::label_);
        st->label = codeLabel++;
        if ((*defaultableStatement)->defaultlabel != -1)
            error(ERR_SWITCH_HAS_DEFAULT);
        (*defaultableStatement)->defaultlabel = st->label;
        (*defaultableStatement)->needlabel = false;
        parent.front()->needlabel = false;
    }
    needkw(&lex, Keyword::colon_);
    return lex;
}
static LEXLIST* statement_do(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    BLOCKDATA* dostmt = Allocate<BLOCKDATA>();
    STATEMENT *st, *lastLabelStmt;
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
    st = stmtNode(lex, parent, StatementNode::label_);
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
        lex = nononconststatement(lex, funcsp, parent, true);
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
            lex = selection_expression(lex, parent, &select, funcsp, Keyword::do_, nullptr);
            if (!MATCHKW(lex, Keyword::closepa_))
            {
                error(ERR_DOWHILE_NEEDS_CLOSEPA);
                errskim(&lex, skim_closepa);
                skip(&lex, Keyword::closepa_);
            }
            else
                lex = getsym();
            st = stmtNode(lex, parent, StatementNode::label_);
            st->label = dostmt->continuelabel;
            currentLineData(parent, lex, 0);
            st = stmtNode(lex, parent, StatementNode::select_);
            st->select = select;
            if (!dostmt->hasbreak && (dostmt->needlabel || isselecttrue(st->select)))
                before->needlabel = true;
            st->label = loopLabel;
            st = stmtNode(lex, parent, StatementNode::label_);
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
    AddBlock(lex, parent, dostmt);
    return lex;
}
static LEXLIST* statement_for(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    BLOCKDATA* before = parent.front();
    BLOCKDATA* forstmt = Allocate<BLOCKDATA>();
    STATEMENT *st, *lastLabelStmt;
    std::list<STATEMENT*>* forline;
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
                std::list<BLOCKDATA*> dummy(beforeit, parent.end());
                AllocateLocalContext(dummy, funcsp, codeLabel++);
            }
            if (Optimizer::cparams.prm_cplusplus)
            {
                LEXLIST* origLex = lex;
                while (lex && !MATCHKW(lex, Keyword::semicolon_) && !MATCHKW(lex, Keyword::colon_))
                    lex = getsym();
                hasColon = MATCHKW(lex, Keyword::colon_);
                lex = prevsym(origLex);
            }

            lex = selection_expression(lex, parent, &init, funcsp, hasColon ? Keyword::rangefor_ : Keyword::for_, &declaration);
            if (Optimizer::cparams.prm_cplusplus && !Optimizer::cparams.prm_oldfor && declaration && MATCHKW(lex, Keyword::colon_))
            {
                // range based for statement
                // we will ignore 'init'.
                TYPE* selectTP = nullptr;
                SYMBOL* declSP = (SYMBOL*)localNameSpace->front()->syms->front();
                EXPRESSION* declExp;
                if (!declSP)
                {
                    diag("statement_for: Cannot get range based range variable");
                    declExp = intNode(ExpressionNode::c_i_, 0);
                }
                else
                {
                    if (declSP->sb->init)
                    {
                        error(ERR_FORRANGE_DECLARATOR_NO_INIT);
                    }
                    declSP->sb->dest = nullptr;
                    declExp = varNode(ExpressionNode::auto_, declSP);
                    declSP->sb->assigned = declSP->sb->attribs.inheritable.used = true;
                }
                lex = getsym();

                auto page = localNameSpace->front()->syms;
                localNameSpace->front()->syms = static_cast<SymbolTable<SYMBOL>*>(localNameSpace->front()->syms->Next());
                if (MATCHKW(lex, Keyword::begin_))
                {
                    TYPE* matchtp = &stdint;
                    EXPRESSION *begin, *size;
                    std::deque<std::pair<TYPE*, EXPRESSION*>> save;
                    std::list<INITLIST*>* lst = nullptr;
                    lex = getInitList(lex, funcsp, &lst);
                    int offset = 0;

                    if (lst)
                    {
                        int n = lst->size();

                        matchtp = lst->front()->tp;
                        if (isref(matchtp))
                            matchtp = basetype(matchtp)->btp;

                        auto valueList = MakeType(BasicType::pointer_, matchtp);
                        valueList->array = true;
                        valueList->size = n * matchtp->size;

                        EXPRESSION* val = anonymousVar(StorageClass::auto_, valueList);

                        int sz = lst->front()->tp->size;

                        for (auto lstitem : *lst)
                        { 
                            TYPE* ittp = lstitem->tp;
                            EXPRESSION* base = exprNode(ExpressionNode::add_, val, intNode(ExpressionNode::c_i_, offset));
                            offset += sz;
                            if (isref(ittp))
                                ittp = basetype(ittp)->btp;
                            if (!comparetypes(matchtp, ittp, true))
                            {
                                if (!isstructured(matchtp))
                                {
                                    errorConversionOrCast(true, ittp, matchtp);
                                }
                                else
                                {
                                    TYPE* ctype = matchtp;
                                    EXPRESSION* newExp = base;
                                    if (!callConstructorParam(&ctype, &newExp, ittp, lstitem->exp, true, false, true, false, true))
                                    {
                                        errorConversionOrCast(true, ittp, matchtp);
                                    }
                                    else
                                    {
                                        st = stmtNode(lex, parent, StatementNode::expr_);
                                        st->select = newExp;
                                        newExp = base;
                                        callDestructor(matchtp->sp, matchtp->sp, &newExp, intNode(ExpressionNode::c_i_, offset / sz), true, true,
                                                       false, true);
                                    }
                                }
                            }
                            else if (isstructured(matchtp))
                            {
                                TYPE* ctype = matchtp;
                                EXPRESSION* newExp = base;
                                ittp->lref = true;
                                if (lstitem->exp->type == ExpressionNode::thisref_)
                                {
                                    lstitem->exp->left->v.func->thisptr->v.sp->sb->dest = nullptr;
                                    lstitem->exp->left->v.func->thisptr = base;
                                    st = stmtNode(lex, parent, StatementNode::expr_);
                                    st->select = lstitem->exp;
                                }
                                else if (!callConstructorParam(&ctype, &newExp, ittp, lstitem->exp, true, false, true, false, true))
                                {
                                    errorConversionOrCast(true, ittp, matchtp);
                                }
                                else
                                {
                                    st = stmtNode(lex, parent, StatementNode::expr_);
                                    st->select = newExp;
                                }
                                newExp = base;
                            }
                            else
                            {
                                st = stmtNode(lex, parent, StatementNode::expr_);
                                deref(matchtp, &base);
                                st->select = exprNode(ExpressionNode::assign_, base, lstitem->exp);
                            }
                        }
                        if (isstructured(matchtp))
                        {
                            auto newExp = val;
                            callDestructor(matchtp->sp, matchtp->sp, &newExp, intNode(ExpressionNode::c_i_, offset / sz), true, true, false,
                                           true);
                            initInsert(&val->v.sp->sb->dest, matchtp, newExp, 0, false);
                        }

                        begin = val;
                        size = intNode(ExpressionNode::c_i_, offset / matchtp->size);
                    }
                    else
                    {
                        select = anonymousVar(StorageClass::auto_, &stdint);
                        begin = select;
                        size = intNode(ExpressionNode::c_i_, 0);
                    }
                    selectTP = InitializerListType(matchtp);
                    std::list<INITIALIZER*>* init = nullptr;
                    initInsert(&init, &stdpointer, begin, 0, false);
                    initInsert(&init, &stdpointer, size, stdpointer.size, false);
                    TYPE* tp2 = MakeType(BasicType::pointer_, &stdpointer);
                    tp2->size = 2 * stdpointer.size;
                    tp2->array = true;
                    EXPRESSION* val = anonymousVar(StorageClass::auto_, tp2);
                    select = convertInitToExpression(tp2, nullptr, val, funcsp, init, nullptr, false);
                    st = stmtNode(lex, parent, StatementNode::expr_);
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
                    TYPE* iteratorType = nullptr;
                    TYPE* tpref = MakeType(BasicType::rref_, selectTP);
                    EXPRESSION* rangeExp = anonymousVar(StorageClass::auto_, tpref);
                    SYMBOL* rangeSP = rangeExp->v.sp;
                    if (isstructured(selectTP))
                        selectTP = basetype(selectTP)->sp->tp;
                    deref(&stdpointer, &rangeExp);
                    needkw(&lex, Keyword::closepa_);
                    while (castvalue(select))
                        select = select->left;
                    if (lvalue(select) && select->type != ExpressionNode::l_ref_ && !isstructured(selectTP))
                        select = select->left;
//                    st = stmtNode(lex, parent, StatementNode::expr_);
//                    st->select = exprNode(ExpressionNode::assign_, rangeExp, select);
                    rangeExp = select;
                    if (!isstructured(selectTP))
                    {
                        // create array references for Keyword::begin_ and Keyword::end_
                        iteratorType = basetype(selectTP)->btp;
                        if (!isarray(selectTP))
                        {
                            error(ERR_FORRANGE_REQUIRES_STRUCT_OR_ARRAY);
                            iteratorType = &stdint;
                        }
                        else if (!selectTP->size)
                        {
                            error(ERR_FORRANGE_ARRAY_UNSIZED);
                        }
                        ibegin = rangeExp;
                        iend = exprNode(ExpressionNode::add_, rangeExp, intNode(ExpressionNode::c_i_, selectTP->size));
                    }
                    else
                    {
                        // try to lookup in structure
                        TYPE thisTP = {};
                        MakeType(thisTP, BasicType::pointer_, rangeSP->tp->btp);
                        sbegin = search(basetype(selectTP)->syms, "begin");
                        send = search(basetype(selectTP)->syms, "end");
                        if (sbegin && send)
                        {
                            SYMBOL *beginFunc = nullptr, *endFunc = nullptr;
                            FUNCTIONCALL fcb, fce;
                            TYPE* ctp;
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
                                if (!comparetypes(basetype(beginFunc->tp)->btp, basetype(endFunc->tp)->btp, true))
                                {
                                    error(ERR_MISMATCHED_FORRANGE_BEGIN_END_TYPES);
                                }
                                else
                                {
                                    FUNCTIONCALL* fc;
                                    iteratorType = basetype(beginFunc->tp)->btp;
                                    if (isstructured(iteratorType))
                                    {
                                        std::list<INITIALIZER*>* dest = nullptr;
                                        EXPRESSION* exp;
                                        fcb.returnEXP = anonymousVar(StorageClass::auto_, iteratorType);
                                        fcb.returnEXP->v.sp->sb->anonymous = false;
                                        fcb.returnSP = fcb.returnEXP->v.sp;
                                        exp = fcb.returnEXP;
                                        dest = nullptr;
                                        callDestructor(basetype(fcb.returnSP->tp)->sp, nullptr, &exp, nullptr, true, true, false, true);
                                        initInsert(&dest, iteratorType, exp, 0, true);
                                        fcb.returnSP->sb->dest = dest;

                                        dest = nullptr;
                                        fce.returnEXP = anonymousVar(StorageClass::auto_, iteratorType);
                                        fce.returnEXP->v.sp->sb->anonymous = false;
                                        fce.returnSP = fcb.returnEXP->v.sp;
                                        exp = fce.returnEXP;
                                        dest = nullptr;
                                        callDestructor(basetype(fce.returnSP->tp)->sp, nullptr, &exp, nullptr, true, true, false, true);
                                        initInsert(&dest, iteratorType, exp, 0, true);
                                        fce.returnSP->sb->dest = dest;
                                    }
                                    fc = Allocate<FUNCTIONCALL>();
                                    *fc = fcb;
                                    fc->sp = beginFunc;
                                    fc->functp = beginFunc->tp;
                                    fc->ascall = true;
                                    ibegin = exprNode(ExpressionNode::func_, nullptr, nullptr);
                                    ibegin->v.func = fc;
                                    fc = Allocate<FUNCTIONCALL>();
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
                                        if (expx->type == ExpressionNode::func_)
                                            if (expx->v.func->returnEXP)
                                                fc->thisptr = expx->v.func->returnEXP;
                                    }
                                    iend = exprNode(ExpressionNode::func_, nullptr, nullptr);
                                    iend->v.func = fc;
                                    iteratorType = basetype(beginFunc->tp)->btp;
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
                                INITLIST args;
                                FUNCTIONCALL fcb, fce;
                                TYPE* ctp;
                                memset(&fcb, 0, sizeof(fcb));
                                memset(&args, 0, sizeof(args));
                                args.tp = rangeSP->tp->btp;
                                args.exp = rangeExp;
                                fcb.arguments = initListListFactory.CreateList();
                                fcb.arguments->push_back(&args);
                                fcb.ascall = true;
                                ctp = rangeSP->tp;
                                beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, sbegin, &fcb, nullptr, false, false, 0);
                                memset(&fce, 0, sizeof(fce));
                                fce.arguments = initListListFactory.CreateList();
                                fce.arguments->push_back(&args);
                                fce.ascall = true;
                                ctp = rangeSP->tp;
                                endFunc = GetOverloadedFunction(&ctp, &fce.fcall, send, &fce, nullptr, false, false, 0);
                                if (beginFunc && endFunc)
                                {
                                    TYPE* it2;
                                    it2 = iteratorType = basetype(beginFunc->tp)->btp;
                                    if (isref(it2))
                                        it2 = it2->btp;
                                    if (!comparetypes(basetype(beginFunc->tp)->btp, basetype(endFunc->tp)->btp, true))
                                    {
                                        error(ERR_MISMATCHED_FORRANGE_BEGIN_END_TYPES);
                                    }
                                    else
                                    {
                                        FUNCTIONCALL* fc;
                                        if (isstructured(iteratorType))
                                        {
                                            std::list<INITIALIZER*>* dest = nullptr;
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
                                        fc = Allocate<FUNCTIONCALL>();
                                        *fc = fcb;
                                        fc->sp = beginFunc;
                                        fc->functp = beginFunc->tp;
                                        fc->ascall = true;
                                        fc->arguments = initListListFactory.CreateList();
                                        for (auto i : *fcb.arguments)
                                            fc->arguments->push_back(i);
                                        if (isstructured(it2) && isstructured(((SYMBOL*)(it2->syms->front()))->tp))
                                        {
                                            EXPRESSION* consexp =
                                                anonymousVar(StorageClass::auto_, basetype(rangeSP->tp)->btp);  // StorageClass::parameter_ to push it...
                                            SYMBOL* esp = consexp->v.sp;
                                            FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                                            TYPE* ctype = basetype(rangeSP->tp)->btp;
                                            esp->sb->stackblock = true;
                                            funcparams->arguments = initListListFactory.CreateList();
                                            for (auto i : *fc->arguments)
                                                funcparams->arguments->push_back(i);
                                            callConstructor(&ctype, &consexp, funcparams, false, 0, true, false, false, false,
                                                            false, false, true);
                                            fc->arguments->front()->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->front()->tp = MakeType(BasicType::lref_, fcb.arguments->front()->tp);
                                        }
                                        ibegin = exprNode(ExpressionNode::func_, nullptr, nullptr);
                                        ibegin->v.func = fc;
                                        fc = Allocate<FUNCTIONCALL>();
                                        *fc = fce;
                                        fc->sp = endFunc;
                                        fc->functp = endFunc->tp;
                                        fc->ascall = true;
                                        fc->arguments = initListListFactory.CreateList();
                                        for (auto i : *fce.arguments)
                                            fc->arguments->push_back(i);
                                        if (isstructured(it2) && isstructured(((SYMBOL*)(it2->syms->front()))->tp))
                                        {
                                            EXPRESSION* consexp =
                                                anonymousVar(StorageClass::auto_, basetype(rangeSP->tp)->btp);  // StorageClass::parameter_ to push it...
                                            SYMBOL* esp = consexp->v.sp;
                                            FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                                            TYPE* ctype = basetype(rangeSP->tp)->btp;
                                            esp->sb->stackblock = true;
                                            funcparams->arguments = initListListFactory.CreateList();
                                            for (auto i : *fc->arguments)
                                                funcparams->arguments->push_back(i);
                                            callConstructor(&ctype, &consexp, funcparams, false, 0, true, false, false, false,
                                                            false, false, true);
                                            fc->arguments->front()->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->front()->tp = MakeType(BasicType::lref_, fce.arguments->front()->tp);
                                        }
                                        iend = exprNode(ExpressionNode::func_, nullptr, nullptr);
                                        iend->v.func = fc;
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
                        if (isstructured(selectTP) && isstructured(iteratorType) && ibegin->type == ExpressionNode::func_ &&
                            iend->type == ExpressionNode::func_)
                        {       
                            eBegin = ibegin->v.func->returnEXP;
                            eEnd = iend->v.func->returnEXP;
                            if (basetype(iteratorType)->sp->sb->structuredAliasType)
                            {
                                deref(basetype(iteratorType)->sp->sb->structuredAliasType, &ibegin->v.func->returnEXP);
                                deref(basetype(iteratorType)->sp->sb->structuredAliasType, &iend->v.func->returnEXP);
                                st = stmtNode(lex, parent, StatementNode::expr_);
                                st->select = exprNode(ExpressionNode::assign_, ibegin->v.func->returnEXP, ibegin);
                                ibegin->v.func->returnEXP = nullptr;
                                ibegin->v.func->returnSP = nullptr;
                                st = stmtNode(lex, parent, StatementNode::expr_);
                                st->select = exprNode(ExpressionNode::assign_, iend->v.func->returnEXP, iend);
                                iend->v.func->returnEXP = nullptr;
                                iend->v.func->returnSP = nullptr;

                            }
                            else
                            {
                                st = stmtNode(lex, parent, StatementNode::expr_);
                                st->select = ibegin;
                                st = stmtNode(lex, parent, StatementNode::expr_);
                                st->select = iend;
                            }
                        }
                        else
                        {
                            TYPE* tpx = MakeType(BasicType::pointer_, iteratorType);
                            eBegin = anonymousVar(StorageClass::auto_, tpx);
                            eEnd = anonymousVar(StorageClass::auto_, tpx);
                            deref(&stdpointer, &eBegin);
                            deref(&stdpointer, &eEnd);
                            st = stmtNode(lex, parent, StatementNode::expr_);
                            st->select = exprNode(ExpressionNode::assign_, eBegin, ibegin);
                            st = stmtNode(lex, parent, StatementNode::expr_);
                            st->select = exprNode(ExpressionNode::assign_, eEnd, iend);
                        }
                        if (isref(iteratorType))
                            iteratorType = iteratorType->btp;
                        if (!isstructured(selectTP) || !isstructured(iteratorType))
                        {
                            compare = exprNode(ExpressionNode::eq_, eBegin, eEnd);
                        }
                        else
                        {
                            TYPE* eqType = iteratorType;
                            compare = eBegin;
                            if (!insertOperatorFunc(ovcl_unary_prefix, Keyword::eq_, funcsp, &eqType, &compare, iteratorType, eEnd, nullptr,
                                                    0))
                            {
                                error(ERR_MISSING_OPERATOR_EQ_FORRANGE_ITERATOR);
                            }
                        }

                        st = stmtNode(lex, parent, StatementNode::select_);
                        st->label = parent.front()->breaklabel;
                        st->altlabel = testlabel;
                        st->select = compare;

                        st = stmtNode(lex, parent, StatementNode::label_);
                        st->label = loopLabel;

                        std::list<BLOCKDATA*> dummy(beforeit, parent.end());
                        AllocateLocalContext(dummy, funcsp, codeLabel++);

                        // initialize var here
                        st = stmtNode(lex, parent, StatementNode::expr_);
                        if (!isstructured(selectTP))
                        {
                            DeduceAuto(&declSP->tp, selectTP, declExp);
                            if (ispointer(selectTP) && ispointer(declSP->tp))
                                declSP->tp = basetype(declSP->tp)->btp;
                            UpdateRootTypes(declSP->tp);
                            if (isarray(selectTP) && !comparetypes(declSP->tp, basetype(selectTP)->btp, true))
                            {
                                error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                            }
                            if (isstructured(declSP->tp))
                            {
                                EXPRESSION* decl = declExp;
                                TYPE* ctype = declSP->tp;
                                FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                                INITLIST* args = Allocate<INITLIST>();
                                funcparams->arguments = initListListFactory.CreateList();
                                funcparams->arguments->push_back(Allocate<INITLIST>());
                                funcparams->arguments->front()->tp = declSP->tp;
                                funcparams->arguments->front()->exp = eBegin;
                                callConstructor(&ctype, &decl, funcparams, false, 0, true, false, false, false, false, false, true);
                                st->select = decl;
                                declDest = declExp;
                                callDestructor(basetype(declSP->tp)->sp, nullptr, &declDest, nullptr, true, true, false, true);
                            }
                            else if (isarray(selectTP))
                            {
                                EXPRESSION* decl = declExp;
                                deref(declSP->tp, &decl);
                                st->select = eBegin;
                                if (!isref(declSP->tp))
                                    deref(basetype(selectTP)->btp, &st->select);
                                st->select = exprNode(ExpressionNode::assign_, decl, st->select);
                            }
                        }
                        else
                        {
                            TYPE* starType = iteratorType;
                            st->select = eBegin;
                            if (ispointer(iteratorType))
                            {
                                DeduceAuto(&declSP->tp, basetype(iteratorType)->btp, declExp);
                                UpdateRootTypes(declSP->tp);
                                if (!comparetypes(declSP->tp, basetype(iteratorType)->btp, true))
                                {
                                    error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                }
                                else if (!isstructured(declSP->tp))
                                {
                                    EXPRESSION* decl = declExp;
                                    deref(declSP->tp, &decl);
                                    st->select = eBegin;
                                    if (!isref(declSP->tp))
                                        deref(basetype(iteratorType)->btp, &st->select);
                                    st->select = exprNode(ExpressionNode::assign_, decl, st->select);
                                }
                                else
                                {
                                    EXPRESSION* decl = declExp;
                                    TYPE* ctype = declSP->tp;
                                    FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                                    INITLIST* args = Allocate<INITLIST>();
                                    funcparams->arguments = initListListFactory.CreateList();
                                    funcparams->arguments->push_back(Allocate<INITLIST>());
                                    funcparams->arguments->front()->tp = declSP->tp;
                                    funcparams->arguments->front()->exp = eBegin;
                                    callConstructor(&ctype, &decl, funcparams, false, 0, true, false, false, false, false, false,
                                                    true);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(basetype(declSP->tp)->sp, nullptr, &declDest, nullptr, true, true, false, true);
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
                                if (isref(declSP->tp))
                                {
                                    ref = true;
                                }
                                DeduceAuto(&declSP->tp, starType, declExp);
                                UpdateRootTypes(declSP->tp);
                                if (!comparetypes(declSP->tp, starType, true) &&
                                    (!isarithmetic(declSP->tp) || !isarithmetic(starType)))
                                {
                                    error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                }
                                else if (!isstructured(declSP->tp))
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

                                    st->select = exprNode(ExpressionNode::assign_, decl, st->select);
                                }
                                else
                                {
                                    EXPRESSION* decl = declExp;
                                    TYPE* ctype = declSP->tp;
                                    FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                                    INITLIST* args = Allocate<INITLIST>();
                                    funcparams->arguments = initListListFactory.CreateList();
                                    funcparams->arguments->push_back(Allocate<INITLIST>());
                                    funcparams->arguments->front()->tp = declSP->tp;
                                    funcparams->arguments->front()->exp = st->select;
                                    callConstructor(&ctype, &decl, funcparams, false, 0, true, false, false, false, false, false,
                                                    true);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(basetype(declSP->tp)->sp, nullptr, &declDest, nullptr, true, true, false, true);
                                }
                            }
                        }
                        do
                        {
                            lastLabelStmt = parent.front()->statements->back();
                            lex = nononconststatement(lex, funcsp, parent, true);
                        } while (lex && parent.front()->statements->back() != lastLabelStmt && parent.front()->statements->back()->purelabel);
                        FreeLocalContext(parent, funcsp, codeLabel++);
                        if (declDest)
                        {
                            st = stmtNode(lex, parent, StatementNode::expr_);
                            st->select = declDest;
                        }
                        st = stmtNode(lex, parent, StatementNode::label_);
                        st->label = parent.front()->continuelabel;
                        st = stmtNode(lex, parent, StatementNode::expr_);

                        // do ++ here
                        if (!isstructured(selectTP))
                        {
                            if (isarray(selectTP))
                                st->select =
                                    exprNode(ExpressionNode::assign_, eBegin,
                                             exprNode(ExpressionNode::add_, eBegin, intNode(ExpressionNode::c_i_, basetype(basetype(selectTP)->btp)->size)));
                        }
                        else
                        {
                            TYPE* ppType = iteratorType;
                            ppType = basetype(ppType);
                            st->select = eBegin;
                            if (ispointer(iteratorType))
                            {
                                st->select = exprNode(
                                    ExpressionNode::assign_, eBegin,
                                    exprNode(ExpressionNode::add_, eBegin, intNode(ExpressionNode::c_i_, basetype(basetype(iteratorType)->btp)->size)));
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, Keyword::autoinc_, funcsp, &ppType, &st->select, nullptr, nullptr,
                                                         nullptr, 0))
                            {
                                error(ERR_MISSING_OPERATOR_PLUSPLUS_FORRANGE_ITERATOR);
                            }
                            else
                            {
                                if (isstructured(ppType))
                                {
                                    st->select->v.func->returnEXP = anonymousVar(StorageClass::auto_, ppType);
                                    st->select->v.func->returnSP = st->select->v.func->returnEXP->v.sp;
                                    declDest = st->select->v.func->returnEXP;
                                    callDestructor(basetype(st->select->v.func->returnSP->tp)->sp, nullptr, &declDest, nullptr, true, true, false,
                                                   true);
                                    st = stmtNode(lex, parent, StatementNode::expr_);
                                    st->select = declDest;
                                }
                            }
                        }

                        if (forline)
                        {
                            parent.front()->statements->insert(parent.front()->statements->end(), forline->begin(), forline->end());
                        }
                        st = stmtNode(lex, parent, StatementNode::label_);
                        st->label = testlabel;

                        st = stmtNode(lex, parent, StatementNode::notselect_);
                        st->label = loopLabel;
                        st->select = compare;

                        if (!parent.front()->hasbreak && (!st->select || isselectfalse(st->select)))
                            before->needlabel = true;
                        st = stmtNode(lex, parent, StatementNode::label_);
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
                AddBlock(lex, parent, forstmt);
                return lex;
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
                            if (isstructured(declSP->tp) && !basetype(declSP->tp)->sp->sb->trivialCons)
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
                TYPE* tp = nullptr;
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
                    TYPE* tp = nullptr;
                    lex = expression_comma(lex, funcsp, nullptr, &tp, &incrementer, nullptr, 0);
                    if (!tp)
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    else
                    {
                        optimize_for_constants(&incrementer);
                        ConstExprPatch(&incrementer);
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
                        st = stmtNode(lex, parent, StatementNode::expr_);
                        st->select = init;
                    }
                    if (Optimizer::cparams.prm_debug || Optimizer::cparams.prm_optimize_for_size ||
                        (Optimizer::chosenAssembler->arch->denyopts & DO_NOENTRYIF))
                    {
                        st = stmtNode(lex, parent, StatementNode::goto_);
                        st->label = testlabel;
                    }
                    else
                    {
                        st = stmtNode(lex, parent, StatementNode::notselect_);
                        st->label = parent.front()->breaklabel;
                        st->altlabel = testlabel;
                        st->select = select;
                    }
                    st = stmtNode(lex, parent, StatementNode::label_);
                    st->label = loopLabel;
                    if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                    {
                        addedBlock++;
                        AllocateLocalContext(parent, funcsp, codeLabel++);
                    }
                    do
                    {
                        lastLabelStmt = parent.front()->statements->back();
                        lex = nononconststatement(lex, funcsp, parent, true);
                    } while (lex && parent.front()->statements->back() != lastLabelStmt && parent.front()->statements->back()->purelabel);
                    if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                    {
                        addedBlock--;
                        FreeLocalContext(parent, funcsp, codeLabel++);
                    }
                    if (before)
                        assignmentUsages(incrementer, false);
                    st = stmtNode(lex, parent, StatementNode::label_);
                    st->label = parent.front()->continuelabel;
                    st = stmtNode(lex, parent, StatementNode::expr_);
                    st->select = incrementer;
                    if (forline)
                    {
                        parent.front()->statements->insert(parent.front()->statements->end(), forline->begin(), forline->end());
                    }
                    st = stmtNode(lex, parent, StatementNode::label_);
                    st->label = testlabel;
                    if (select)
                    {
                        st = stmtNode(lex, parent, StatementNode::select_);
                        st->label = loopLabel;
                        st->select = select;
                    }
                    else
                    {
                        st = stmtNode(lex, parent, StatementNode::goto_);
                        st->label = loopLabel;
                    }
                    if (!parent.front()->hasbreak && (!st->select || isselectfalse(st->select)))
                        before->needlabel = true;
                    st = stmtNode(lex, parent, StatementNode::label_);
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
    AddBlock(lex, parent, forstmt);
    return lex;
}
static LEXLIST* statement_if(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    STATEMENT *st, *lastLabelStmt;
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
        lex = selection_expression(lex, parent, &select, funcsp, Keyword::if_, &declaration);
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
                lex = selection_expression(lex, parent, &select, funcsp, Keyword::if_, nullptr);
            }
        }
        if (MATCHKW(lex, Keyword::closepa_))
        {
            if (isconstexpr && !IsConstantExpression(select, false, true))
                error(ERR_CONSTANT_VALUE_EXPECTED);
            bool optimized = false;
            std::list<STATEMENT*>::iterator sti;
            currentLineData(parent, lex, 0);
            lex = getsym();
            if (init)
            {
                st = stmtNode(lex, parent, StatementNode::expr_);
                st->select = init;
            }
            st = stmtNode(lex, parent, StatementNode::notselect_);
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
                lex = nononconststatement(lex, funcsp, parent, true);
            } while (lex && parent.front()->statements->back() != lastLabelStmt && parent.front()->statements->back()->purelabel);
            needlabelif = parent.front()->needlabel;
            if (MATCHKW(lex, Keyword::else_))
            {
                int elsebr = codeLabel++;
                if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                }
                st = stmtNode(lex, parent, StatementNode::goto_);
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
                st = stmtNode(lex, parent, StatementNode::label_);
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
                    lex = nononconststatement(lex, funcsp, parent, true);
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
                st = stmtNode(lex, parent, StatementNode::label_);
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
                st = stmtNode(lex, parent, StatementNode::label_);
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

    return lex;
}
int GetLabelValue(LEXLIST* lex, std::list<BLOCKDATA*>* parent, STATEMENT* st)
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
                    auto x = Allocate<BLOCKDATA>();
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
static LEXLIST* statement_goto(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    (void)funcsp;
    (void)parent;
    lex = getsym();
    currentLineData(parent, lex, 0);
    if (ISID(lex))
    {
        // standard c/c++ goto
        SYMBOL* spx = search(labelSyms, lex->data->value.s.a);
        BLOCKDATA* block = Allocate<BLOCKDATA>();
        parent.push_front(block);
        block->type = Keyword::begin_;
        block->table = localNameSpace->front()->syms;
        STATEMENT* st = stmtNode(lex, parent, StatementNode::goto_);
        st->explicitGoto = true;
        int lbl = 0;
        if (!spx)
        {
            lbl = GetLabelValue(lex, &parent, st);
        }
        else
        {
            lbl = spx->sb->offset;
            thunkGotoDestructors(&st->destexp, parent, *spx->sb->gotoBlockTable);
            thunkCatchCleanup(st, funcsp, parent, *spx->sb->gotoBlockTable);
        }
        st->label = lbl;
        lex = getsym();
        before->needlabel = true;
        parent.pop_front();
        AddBlock(lex, parent, block);
        canFallThrough = true;
    }
    else if (MATCHKW(lex, Keyword::star_))
    {
        // extension: computed goto
        BLOCKDATA* block = Allocate<BLOCKDATA>();
        STATEMENT* st = stmtNode(lex, parent, StatementNode::goto_);
        parent.push_front(block);
        block->type = Keyword::begin_;
        block->table = localNameSpace->front()->syms;
        st->explicitGoto = true;
        st->indirectGoto = true;
        Optimizer::functionHasAssembly = true; // don't optimize
        // turn off optimizations
        lex = getsym();
        TYPE*tp = nullptr;
        EXPRESSION* exp = nullptr;
        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
        if (!tp)
            error(ERR_IDENTIFIER_EXPECTED);
        else if (!ispointer(tp))
            error(ERR_INVALID_POINTER_CONVERSION);
        else
        {
            st->select = exp;
            AddBlock(lex, parent, block);
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
    return lex;
}
static LEXLIST* statement_label(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    SYMBOL* spx = search(labelSyms, lex->data->value.s.a);
    STATEMENT* st;
    (void)funcsp;
    st = stmtNode(lex, parent, StatementNode::label_);
    if (spx)
    {
        if (spx->sb->storage_class == StorageClass::ulabel_){
            spx->sb->storage_class = StorageClass::label_;
            // may come here from assembly language...
            if (spx->sb->gotoTable && spx->sb->gotoTable->size())
            {
                thunkGotoDestructors(&spx->sb->gotoTable->front()->destexp, *spx->sb->gotoBlockTable, parent);
                thunkCatchCleanup(spx->sb->gotoTable->front(), funcsp, *spx->sb->gotoBlockTable, parent);
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
                auto x = Allocate<BLOCKDATA>();
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
    return lex;
}
static EXPRESSION* ConvertReturnToRef(EXPRESSION* exp, TYPE* tp, TYPE* boundTP)
{
    if (lvalue(exp))
    {
        EXPRESSION* exp2;
        while (castvalue(exp))
            exp = exp->left;
        exp2 = exp;
        if (!isstructured(basetype(tp)->btp))
        {
            if (isref(basetype(tp)->btp))
            {
                if (!isstructured(basetype(basetype(tp)->btp)->btp))
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
        else if (basetype(tp)->btp->type == BasicType::aggregate_)
        {
            if (!isfunction(basetype(boundTP)->btp))
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
        else if (isfunction(basetype(tp)->btp))
        {
            if (!isfunction(basetype(boundTP)->btp))
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
            if (referenceTypeError(tp, exp2) != exp2->type && (!isstructured(basetype(tp)->btp) || exp2->type != ExpressionNode::lvalue_))
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
    }
    else
    {
        if (exp->type == ExpressionNode::cond_)
        {
            exp->right->left = ConvertReturnToRef(exp->right->left, tp, boundTP);
            exp->right->right = ConvertReturnToRef(exp->right->right, tp, boundTP);
        }
        else if (!isstructured(basetype(tp)->btp) && !isref(boundTP))
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
static EXPRESSION* baseNode(EXPRESSION* node)
{
    if (!node)
        return 0;
    switch (node->type)
    {
        case ExpressionNode::auto_:
        case ExpressionNode::pc_:
        case ExpressionNode::global_:
        case ExpressionNode::threadlocal_:
            return node;
        case ExpressionNode::add_: {
            EXPRESSION* rv = baseNode(node->left);
            if (rv)
                return rv;
            return baseNode(node->right);
        }
        default:
            return 0;
    }
}
static void MatchReturnTypes(SYMBOL* funcsp, TYPE* tp1, TYPE* tp2)
{
    if (matchReturnTypes)
    {
        bool err = false;
        if (isref(tp1))
            tp1 = basetype(tp1)->btp;
        if (isref(tp2))
            tp2 = basetype(tp2)->btp;
        while (tp1 && tp2 && !err)
        {
            // if (isconst(tp1) != isconst(tp2) || isvolatile(tp1) != isvolatile(tp2))
            //    err = true;
            tp1 = basetype(tp1);
            tp2 = basetype(tp2);
            if (isstructured(tp1) && isstructured(tp2))
            {
                if (!comparetypes(tp1, tp2, true) && classRefCount(tp1->sp, tp2->sp) != 1)
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
static LEXLIST* statement_return(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    STATEMENT* st;
    TYPE* tp = nullptr;
    EXPRESSION* returnexp = nullptr;
    TYPE* returntype = nullptr;
    EXPRESSION* destexp = nullptr;

    canFallThrough = true;
    if (funcsp->sb->attribs.inheritable.linkage3 == Linkage::noreturn_)
        error(ERR_NORETURN);
    funcsp->sb->retcount++;

    lex = getsym();
    if (MATCHKW(lex, Keyword::semicolon_))
    {
        if (!isvoid(basetype(funcsp->tp)->btp))
        {
            if (!basetype(funcsp->tp)->sp->sb->isConstructor && !basetype(funcsp->tp)->sp->sb->isDestructor)
                error(ERR_RETURN_MUST_RETURN_VALUE);
        }
    }
    else
    {
        tp = basetype(funcsp->tp)->btp;

        if (isautotype(tp))
        {
            TYPE* tp1;
            EXPRESSION* exp1;
            LEXLIST* current = lex;
            lex = expression(lex, funcsp, nullptr, &tp1, &exp1, _F_SIZEOF);
            ConstExprPatch(&exp1);
            lex = prevsym(current);
            while (tp1->type == BasicType::typedef_)
                tp1 = tp1->btp;
            DeduceAuto(&basetype(funcsp->tp)->btp, tp1, exp1);
            tp = basetype(funcsp->tp)->btp;
            UpdateRootTypes(funcsp->tp);
            SetLinkerNames(funcsp, funcsp->sb->attribs.inheritable.linkage);
            matchReturnTypes = true;
        }
        if (isstructured(tp) || isbitint(tp) || basetype(tp)->type == BasicType::memberptr_)
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
            if ((funcsp->sb->attribs.inheritable.linkage == Linkage::pascal_) && basetype(funcsp->tp)->syms->size() &&
                ((SYMBOL*)basetype(funcsp->tp)->syms->front())->tp->type != BasicType::void_)
                sp->sb->offset = funcsp->sb->paramsize;
            deref(&stdpointer, &en);
            if (Optimizer::cparams.prm_cplusplus && isstructured(tp) && (!basetype(tp)->sp->sb->trivialCons || MATCHKW(lex, Keyword::begin_)))
            {
                bool implicit = false;
                if (basetype(tp)->sp->sb->templateLevel && basetype(tp)->sp->templateParams && !basetype(tp)->sp->sb->instantiated)
                {
                    SYMBOL* sym = basetype(tp)->sp;
                    if (!allTemplateArgsSpecified(sym, sym->templateParams))
                        sym = GetClassTemplate(sym, sym->templateParams, false);
                    if (sym && allTemplateArgsSpecified(sym, sym->templateParams))
                        tp = TemplateClassInstantiate(sym, sym->templateParams, false, StorageClass::global_)->tp;
                }
                if (MATCHKW(lex, Keyword::begin_))
                {
                    implicit = true;
                    std::list<INITIALIZER*>* init = nullptr, *dest = nullptr;
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
                    FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
                    TYPE* ctype = tp;
                    // shortcut for conversion from single expression
                    EXPRESSION* exp1 = nullptr;
                    TYPE* tp1 = nullptr;
                    lex = expression_no_comma(lex, funcsp, nullptr, &tp1, &exp1, nullptr, 0);
                    MatchReturnTypes(funcsp, tp, tp1);
                    if (!tp1)
                    {
                        tp1 = &stdint;
                        exp1 = intNode(ExpressionNode::c_i_, 0);
                        error(ERR_IDENTIFIER_EXPECTED);
                    }
                    if (tp1 && isstructured(tp1))
                    {
                        if (sameTemplate(tp, tp1))
                        {
                            ctype = tp = tp1;
                            basetype(funcsp->tp)->btp = tp1;
                        }
                        else if (!comparetypes(tp, tp1, true))
                        {
                            implicit = true;
                        }
                        if (basetype(tp1)->sp->sb->templateLevel && basetype(tp1)->sp->templateParams &&
                            !basetype(tp1)->sp->sb->instantiated && !templateNestingCount)
                        {
                            SYMBOL* sym = basetype(tp1)->sp;
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
                    if (exptemp->type == ExpressionNode::func_ && isfunction(exptemp->v.func->sp->tp) && exptemp->v.func->thisptr &&
                        comparetypes(tp, basetype(exptemp->v.func->thistp)->btp, 0) &&
                        (!basetype(tp)->sp->sb->templateLevel || sameTemplate(tp, basetype(exptemp->v.func->thistp)->btp)) &&
                        exptemp->v.func->thisptr->type == ExpressionNode::auto_ && exptemp->v.func->thisptr->v.sp->sb->anonymous)
                    {
                        exptemp->v.func->thisptr->v.sp->sb->destructed = true;
                        exptemp->v.func->thisptr = en;
                        returntype = tp;
                        returnexp = exp1;
                        maybeConversion = false;
                        implicit = true;
                    }
                    else
                    {
                        bool nonconst = funcsp->sb->nonConstVariableUsed;
                        funcparams->arguments = initListListFactory.CreateList();
                        funcparams->arguments->push_back(Allocate<INITLIST>());
                        funcparams->arguments->front()->tp = tp1;
                        funcparams->arguments->front()->exp = exp1;
                        oldrref = basetype(tp1)->rref;
                        oldlref = basetype(tp1)->lref;
                        basetype(tp1)->rref = exp1->type == ExpressionNode::auto_ && exp1->v.sp->sb->storage_class != StorageClass::parameter_;
                        if (exptemp->type == ExpressionNode::func_ && isfunction(exptemp->v.func->sp->tp) &&
                            basetype(basetype(exptemp->v.func->sp->tp)->btp)->type != BasicType::lref_)
                            basetype(tp1)->rref = true;
                        basetype(tp1)->lref = !basetype(tp1)->rref;
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
                            basetype(tp1)->rref = false;
                            basetype(tp1)->lref = true;
                            callConstructor(&ctype, &en, funcparams, false, nullptr, true, maybeConversion, implicit, false, false,
                                            false, true);
                        }
                        basetype(tp1)->rref = oldrref;
                        basetype(tp1)->lref = oldlref;
                        funcsp->sb->nonConstVariableUsed = nonconst;
                        returnexp = en;
                    }
                }
            }
            else
            {

                TYPE* tp1;
                lex = optimized_expression(lex, funcsp, nullptr, &tp1, &returnexp, true);
                if (!tp1)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                    MatchReturnTypes(funcsp, tp, tp1);
                if (!comparetypes(tp, tp1, true) &&
                    (!isbitint(tp) || !isbitint(tp1)) &&
                    ((Optimizer::architecture != ARCHITECTURE_MSIL) || !isstructured(tp) || !isconstzero(&stdint, returnexp)))
                {
                    bool toErr = true;
                    if (isstructured(tp) && isstructured(tp1) && classRefCount(basetype(tp)->sp,basetype(tp1)->sp))
                    {
                        toErr = false;
                        EXPRESSION q = {}, *v = &q;
                        v->type = ExpressionNode::c_i_;
                        v = baseClassOffset(basetype(tp)->sp, basetype(tp1)->sp, v);
                        returnexp = exprNode(ExpressionNode::add_, returnexp, v);
                    
                    }
                    if (toErr)
                    {
                        errorConversionOrCast(true, tp1, tp);
                    }
                }
                else
                {
                    if (returnexp->type == ExpressionNode::func_ && !returnexp->v.func->ascall)
                    {
                        if (returnexp->v.func->sp->sb->storage_class == StorageClass::overloads_)
                        {
                            SYMBOL* funcsp;
                            if (returnexp->v.func->sp->sb->parentClass && !returnexp->v.func->asaddress)
                                error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                            funcsp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &returnexp, 0);
                            if (funcsp && basetype(tp)->type == BasicType::memberptr_)
                            {
                                int lbl = dumpMemberPtr(funcsp, tp, true);
                                returnexp = intNode(ExpressionNode::labcon_, lbl);
                            }
                        }
                        else
                        {
                            returnexp = intNode(ExpressionNode::labcon_, dumpMemberPtr(returnexp->v.func->sp, tp, true));
                        }
                    }
                    else if (returnexp->type == ExpressionNode::pc_ || returnexp->type == ExpressionNode::memberptr_)
                    {
                        returnexp = intNode(ExpressionNode::labcon_, dumpMemberPtr(returnexp->v.sp, tp, true));
                    }
                    if ((Optimizer::architecture != ARCHITECTURE_MSIL) ||
                        funcsp->sb->attribs.inheritable.linkage2 == Linkage::unmanaged_ || !msilManaged(funcsp))
                    {
                        if (isbitint(tp))
                        {
                            if (!comparetypes(tp, tp1, 0))
                            {
                                cast(tp, &returnexp);
                            }
                            returnexp = exprNode(ExpressionNode::blockassign_, en, returnexp);
                            returnexp->size = tp;
                            returnexp->altdata = (void*)(basetype(tp));

                        }
                        else if (!isstructured(tp) || !basetype(tp)->sp->sb->structuredAliasType)
                        {
                            returnexp = exprNode(ExpressionNode::blockassign_, en, returnexp);
                            returnexp->size = tp;
                            returnexp->altdata = (void*)(basetype(tp));
                        }
                        else
                        {
                            EXPRESSION** expx = &returnexp;
                            if (*expx && (*expx)->type == ExpressionNode::void_)
                            {
                                while ((*expx)->right && (*expx)->right->type == ExpressionNode::void_)
                                    expx = &(*expx)->right;
                                expx = &(*expx)->right;
                            }
                            deref(basetype(tp)->sp->sb->structuredAliasType, expx);
                        }
                    }
                    returntype = tp;
                }
            }
        }
        else
        {
            TYPE* tp1 = nullptr;
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
                MatchReturnTypes(funcsp, tp, tp1);
            if (basetype(tp)->type == BasicType::string_)
            {
                if (returnexp->type == ExpressionNode::labcon_)
                    returnexp->type = ExpressionNode::c_string_;
                else if (basetype(tp1)->type != BasicType::string_)
                    returnexp = exprNode(ExpressionNode::x_string_, returnexp, nullptr);
                tp1 = &std__string;
            }
            else if (!comparetypes(tp1, tp, true) && ismsil(tp1))
            {
                errorConversionOrCast(true, tp1, tp);
            }
            if (needend)
            {
                if (!needkw(&lex, Keyword::end_))
                {
                    errskim(&lex, skim_end);
                    skip(&lex, Keyword::end_);
                }
            }
            if (basetype(tp)->type == BasicType::object_)
                if (basetype(tp1)->type != BasicType::object_ && !isstructured(tp1) && (!isarray(tp1) || !basetype(tp1)->msil))
                    returnexp = exprNode(ExpressionNode::x_object_, returnexp, nullptr);
            if (isstructured(tp1) && isarithmetic(tp))
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
            if (returnexp->type == ExpressionNode::func_)
            {
                if (returnexp->v.func->sp->sb->storage_class == StorageClass::overloads_)
                {
                    EXPRESSION* exp1 = returnexp;
                    if (returnexp->v.func->sp->sb->parentClass && !returnexp->v.func->asaddress)
                        error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                    returnexp->v.func->sp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &exp1, 0);
                    returnexp->v.func->fcall = varNode(ExpressionNode::pc_, returnexp->v.func->sp);
                }
            }
            if (Optimizer::cparams.prm_cplusplus && isstructured(returntype))
            {
                TYPE* tp1 = basetype(funcsp->tp)->btp;
                if (isref(tp1))
                    tp1 = basetype(tp1)->btp;
                if (cppCast(returntype, &tp1, &returnexp))
                    returntype = tp = basetype(funcsp->tp)->btp;
            }
        }
        if (isref(basetype(funcsp->tp)->btp))
        {
            if (basetype(basetype(tp)->btp)->type != BasicType::memberptr_)
                returnexp = ConvertReturnToRef(returnexp, basetype(funcsp->tp)->btp, returntype);
        }
        else if (returnexp && returnexp->type == ExpressionNode::auto_ && returnexp->v.sp->sb->storage_class == StorageClass::auto_)
        {
            if (!isstructured(basetype(funcsp->tp)->btp) && basetype(basetype(funcsp->tp)->btp)->type != BasicType::memberptr_ &&
                !isbitint(basetype(funcsp->tp)->btp))
                if (basetype(basetype(funcsp->tp)->btp)->type != BasicType::object_ &&
                    (!isarray(basetype(funcsp->tp)->btp) || !basetype(funcsp->tp)->btp->msil) &&
                    basetype(basetype(funcsp->tp)->btp)->type != BasicType::templateselector_)
                    error(ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE);
        }
        if (!returnexp)
            returnexp = intNode(ExpressionNode::c_i_, 0);  // errors
    }
    currentLineData(parent, lex, 0);
    thunkRetDestructors(&destexp, nullptr, localNameSpace->front()->syms);
    st = stmtNode(lex, parent, StatementNode::return_);
    st->select = returnexp;
    st->destexp = destexp;
    thunkCatchCleanup(st, funcsp, parent, emptyBlockdata);  // to top level
    if (returnexp && returntype && !isautotype(returntype) && !matchReturnTypes)
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
            else if (isstructured(returntype) || isstructured(tp))
            {
                if (!comparetypes(returntype, tp, false) && !sameTemplate(returntype, tp))
                    error(ERR_RETMISMATCH);
            }
            else if (basetype(returntype)->type == BasicType::memberptr_ || basetype(tp)->type == BasicType::memberptr_)
            {
                if (isconstzero(tp, st->select))
                {
                    int lbl = dumpMemberPtr(nullptr, returntype, true);
                    st->select = intNode(ExpressionNode::labcon_, lbl);
                }
                else
                {
                    if (st->select->type == ExpressionNode::memberptr_)
                    {
                        int lbl = dumpMemberPtr(st->select->v.sp, returntype, true);
                        st->select = intNode(ExpressionNode::labcon_, lbl);
                    }
                    if (!comparetypes(returntype, tp, true))
                        error(ERR_RETMISMATCH);
                }
            }
            else
            {
                if (!isref(basetype(funcsp->tp)->btp) &&
                    !isbitint(basetype(funcsp->tp)->btp) &&
                    (isarithmetic(basetype(funcsp->tp)->btp) ||
                     (ispointer(basetype(funcsp->tp)->btp) && !isarray(basetype(funcsp->tp)->btp))))
                    cast(returntype, &st->select);
                if (ispointer(returntype))
                {
                    if (isarithmetic(tp))
                    {
                        if (iscomplex(tp))
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (isfloat(tp) || isimaginary(tp))
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (isarithmeticconst(returnexp))
                        {
                            if (!isintconst(returnexp) || !isconstzero(returntype, returnexp))
                                error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                        else if (returnexp->type != ExpressionNode::func_ || returnexp->v.func->fcall->type != ExpressionNode::l_p_)
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    }
                    else if (ispointer(tp))
                    {
                        if (!comparetypes(returntype, tp, true))
                        {
                            if (!isvoidptr(returntype) && !isvoidptr(tp))
                            {
                                if (!matchingCharTypes(returntype, tp))
                                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                            }
                            else
                            {
                                if (Optimizer::cparams.prm_cplusplus && !isvoidptr(returntype) && returnexp->type != ExpressionNode::nullptr_ &&
                                    isvoidptr(tp))
                                    error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                            }
                        }
                    }
                    else if (isfunction(tp))
                    {
                        if (!isvoidptr(returntype) &&
                            (!isfunction(basetype(returntype)->btp) || !comparetypes(returntype, tp, true)))
                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                    }
                    else
                        error(ERR_INVALID_POINTER_CONVERSION);
                }
                else if (ispointer(tp))
                {
                    if (iscomplex(returntype))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (isfloat(returntype) || isimaginary(returntype))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (isint(returntype))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                }
                else if (ispointer(tp))
                {
                    if (iscomplex(returntype))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (isfloat(returntype) || isimaginary(returntype))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (isint(returntype))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                }
            }
        }
    }
    before->needlabel = true;
    return lex;
}
static LEXLIST* statement_switch(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    bool oldfallthrough = canFallThrough;
    canFallThrough = true;
    auto before = parent.front();
    BLOCKDATA* switchstmt = Allocate<BLOCKDATA>();
    STATEMENT* st;
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
        lex = selection_expression(lex, parent, &select, funcsp, Keyword::switch_, &declaration);
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
                lex = selection_expression(lex, parent, &select, funcsp, Keyword::if_, nullptr);
            }
        }
        if (MATCHKW(lex, Keyword::closepa_))
        {
            currentLineData(parent, lex, 0);
            lex = getsym();
            if (init)
            {
                st = stmtNode(lex, parent, StatementNode::expr_);
                st->select = init;
            }
            st = stmtNode(lex, parent, StatementNode::switch_);
            st->select = select;
            st->breaklabel = switchstmt->breaklabel;
            switchstmt->cases = casedataListFactory.CreateList();
            lex = nononconststatement(lex, funcsp, parent, true);
            EndOfCaseGroup(funcsp, parent);
            st->cases = switchstmt->cases;
            st->label = switchstmt->defaultlabel;
            if (st->label != -1 && switchstmt->needlabel && !switchstmt->hasbreak)
                before->needlabel = true;
            /* force a default if there was none */
            if (st->label == -1)
            {
                st->label = codeLabel;
                st = stmtNode(lex, parent, StatementNode::label_);
                st->label = codeLabel++;
            }
            st = stmtNode(lex, parent, StatementNode::label_);
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
    AddBlock(lex, parent, switchstmt);
    canFallThrough = oldfallthrough;
    return lex;
}
static LEXLIST* statement_while(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    BLOCKDATA* whilestmt = Allocate<BLOCKDATA>();
    STATEMENT *st, *lastLabelStmt;
    std::list<STATEMENT*>* whileline;
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
        lex = selection_expression(lex, parent, &select, funcsp, Keyword::while_, &declaration);
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
                st = stmtNode(lex, parent, StatementNode::goto_);
                st->label = whilestmt->continuelabel;
            }
            else
            {
                st = stmtNode(lex, parent, StatementNode::notselect_);
                st->label = whilestmt->breaklabel;
                st->altlabel = whilestmt->continuelabel;
                st->select = select;
            }

            st = stmtNode(lex, parent, StatementNode::label_);
            st->label = loopLabel;
            if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            do
            {
                lastLabelStmt = whilestmt->statements->back();
                lex = nononconststatement(lex, funcsp, parent, true);
            } while (lex && whilestmt->statements->back() != lastLabelStmt && whilestmt->statements->back()->purelabel);
            if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c99)
            {
                addedBlock--;
                FreeLocalContext(parent, funcsp, codeLabel++);
            }
            st = stmtNode(lex, parent, StatementNode::label_);
            st->label = whilestmt->continuelabel;
            if (whileline)
            {
                parent.front()->statements->insert(parent.front()->statements->end(), whileline->begin(), whileline->end());
            }
            st = stmtNode(lex, parent, StatementNode::select_);
            st->label = loopLabel;
            st->select = select;
            if (!whilestmt->hasbreak && isselecttrue(st->select))
                before->needlabel = true;
            st = stmtNode(lex, parent, StatementNode::label_);
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
    std::list<BLOCKDATA*> dummy{ whilestmt };
    parent.pop_front();
    AddBlock(lex, parent, whilestmt);
    return lex;
}
static bool checkNoEffect(EXPRESSION* exp)
{
    if (exp->noexprerr)
        return false;
    while (castvalue(exp) || lvalue(exp))
        exp = exp->left;
    switch (exp->type)
    {
        case ExpressionNode::func_:
        case ExpressionNode::assign_:
        case ExpressionNode::auto_inc_:
        case ExpressionNode::auto_dec_:
        case ExpressionNode::blockassign_:
        case ExpressionNode::blockclear_:
        case ExpressionNode::stmt_:
        case ExpressionNode::atomic_:
        case ExpressionNode::void_nz_:
        case ExpressionNode::void_:
        case ExpressionNode::initblk_:
        case ExpressionNode::cpblk_:
        case ExpressionNode::initobj_:
        case ExpressionNode::sizeof_:
            return false;
        case ExpressionNode::not__lvalue_:
        case ExpressionNode::lvalue_:
        case ExpressionNode::thisref_:
        case ExpressionNode::literalclass_:
        case ExpressionNode::funcret_:
            return checkNoEffect(exp->left);
        case ExpressionNode::cond_:
            return checkNoEffect(exp->right->left) & checkNoEffect(exp->right->right);
        default:
            return true;
    }
}
static LEXLIST* statement_expr(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    STATEMENT* st;
    EXPRESSION* select = nullptr;
    TYPE* tp = nullptr;
    auto oldLines = lines;
    lines = nullptr;
    (void)parent;

    auto prevlex = lex;
    lex = optimized_expression(prevlex, funcsp, nullptr, &tp, &select, true);
    if (expressionReturns.size())
        expressionReturns.back() = std::move(std::pair<EXPRESSION*, TYPE*>(select, tp));
    lines = oldLines;

    currentLineData(parent, prevlex, 0);
    st = stmtNode(lex, parent, StatementNode::expr_);
    st->select = select;
    if (!tp)
    {
        lex = getsym();
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (tp->type != BasicType::void_ && tp->type != BasicType::any_)
    {
        if (checkNoEffect(st->select))
            error(ERR_EXPRESSION_HAS_NO_EFFECT);
        if (Optimizer::cparams.prm_cplusplus && isstructured(tp) && select->type == ExpressionNode::func_)
        {
            SYMBOL* sym = select->v.func->returnSP;
            if (sym && sym->sb->allocate)
            {
                std::list<INITIALIZER*>* init = nullptr;
                EXPRESSION* exp = select->v.func->returnEXP;
                callDestructor(basetype(tp)->sp, nullptr, &exp, nullptr, true, false, false, true);
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
        if (exp1->type == ExpressionNode::func_)
        {
            auto sp = exp1->v.func->sp;
            if (sp->sb->attribs.uninheritable.nodiscard)
            {
                if (!sp->sb->isConstructor && !sp->sb->isDestructor)
                {
                    if (!isvoid(basetype(sp->tp)->btp))
                        error(ERR_RETURN_VALUE_NO_DISCARD);
                }
            }
        }
    }
    return lex;
}
static LEXLIST* asm_declare(LEXLIST* lex)
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
    return lex;
}
LEXLIST* statement_catch(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent, int label, int startlab, int endlab)
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
            STATEMENT* st;
            TYPE* tp = nullptr;
            BLOCKDATA* catchstmt = Allocate<BLOCKDATA>();
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
            if (needkw(&lex, Keyword::closepa_))
            {
                if (MATCHKW(lex, Keyword::begin_))
                {
                    lex = compound(lex, funcsp, parent, false);
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
            st = stmtNode(lex, parent, StatementNode::catch_);
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
    return lex;
}
LEXLIST* statement_try(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    auto ilbefore = parent.begin();
    ++ilbefore;
    auto next = ilbefore != parent.end() ? *ilbefore : nullptr;
    STATEMENT* st;
    BLOCKDATA* trystmt = Allocate<BLOCKDATA>();
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
        lex = compound(lex, funcsp, parent, false);
        tryLevel--;
        FreeLocalContext(parent, funcsp, codeLabel++);
        parent.pop_front();
        before->needlabel = trystmt->needlabel;
        st = stmtNode(lex, parent, StatementNode::try_);
        st->label = codeLabel++;
        st->endlabel = codeLabel++;
        st->breaklabel = trystmt->breaklabel;
        st->blockTail = trystmt->blockTail;
        st->lower = trystmt->statements;
        before->nosemi = true;
        if (next)
            next->nosemi = true;
        lex = statement_catch(lex, funcsp, parent, st->breaklabel, st->label, st->endlabel);
    }
    inLoopOrConditional--;

    return lex;
}
bool hasInlineAsm() { return Optimizer::architecture == ARCHITECTURE_X86; }
LEXLIST* statement_asm(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent)
{
    auto before = parent.front();
    (void)funcsp;
    (void)parent;  //
    Optimizer::functionHasAssembly = true;
    if (hasInlineAsm())
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
            return lex;
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
                    lex = asm_declare(lex);
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
    return lex;
}
static void reverseAssign(std::list<STATEMENT*>* current, EXPRESSION** exp)
{
    if (current)
    {
        for (auto it = current->end(); it != current->begin();)
        {
            --it;
            if ((*it)->type != StatementNode::line_)
            {
                if (*exp)
                    *exp = exprNode(ExpressionNode::void_, (*it)->select, *exp);
                else
                    *exp = (*it)->select;
            }
        }
    }
}
static LEXLIST* autodeclare(LEXLIST* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, std::list<BLOCKDATA*>& parent, int asExpression)
{
    BLOCKDATA block;
    (void)parent;
    declareAndInitialize = false;
    memset(&block, 0, sizeof(block));
    parent.push_front(&block);
    lex = declare(lex, funcsp, tp, StorageClass::auto_, Linkage::none_, parent, false, asExpression, false, AccessLevel::public_);
    parent.pop_front();
   
    // move any auto assignments
    reverseAssign(block.statements, exp);

    // now move variable declarations
    if (block.statements)
    {
        for (auto stmt : *block.statements)
        {
            if (stmt->type == StatementNode::varstart_)
            {
                STATEMENT* s = stmtNode(lex, parent, StatementNode::varstart_);
                s->select = stmt->select;
            }
            else if (stmt->type == StatementNode::line_)
            {
                STATEMENT* s = stmtNode(lex, parent, StatementNode::line_);
                s->lineData = stmt->lineData;
            }
        }
    }
    if (!*exp)
    {
        *exp = intNode(ExpressionNode::c_i_, 0);
        errorint(ERR_NEEDY, '=');
    }
    return lex;

}
bool resolveToDeclaration(LEXLIST* lex, bool structured)
{
    LEXLIST* placeholder = lex;
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
        int level = 1;
        lex = getsym();
        while (level && lex != nullptr && !MATCHKW(lex, Keyword::semicolon_))
        {
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
        if (MATCHKW(lex, Keyword::assign_) || ((hasStar || !structured) && MATCHKW(lex, Keyword::openpa_)) || MATCHKW(lex, Keyword::openbr_))
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
static LEXLIST* nononconststatement(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent, bool viacontrol)
{
    int old = funcsp->sb->nonConstVariableUsed;
    auto rv = statement(lex, funcsp, parent, viacontrol);
    funcsp->sb->nonConstVariableUsed = old;
    return rv;
}

LEXLIST* statement(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent, bool viacontrol)
{
    auto before = parent.front();
    LEXLIST* start = lex;
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (ISID(lex))
    {
        lex = getsym();
        if (MATCHKW(lex, Keyword::colon_))
        {
            lex = backupsym();
            lex = statement_label(lex, funcsp, parent);
            before->needlabel = false;
            before->nosemi = true;
            return lex;
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
    before->nosemi = false;
    expressionStatements.push(&parent);
    switch (KW(lex))
    {
        case Keyword::try_:
            canFallThrough = false;
            lex = statement_try(lex, funcsp, parent);
            break;
        case Keyword::catch_:
            canFallThrough = false;
            error(ERR_CATCH_WITHOUT_TRY);
            lex = statement_catch(lex, funcsp, parent, 1, 1, 1);
            break;
        case Keyword::begin_: {
            canFallThrough = parent.front()->type == Keyword::switch_;
            lex = compound(lex, funcsp, parent, false);
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
            return lex;
        case Keyword::do_:
            canFallThrough = false;
            lex = statement_do(lex, funcsp, parent);
            break;
        case Keyword::while_:
            canFallThrough = false;
            lex = statement_while(lex, funcsp, parent);
            expressionStatements.pop();
            return lex;
        case Keyword::for_:
            canFallThrough = false;
            lex = statement_for(lex, funcsp, parent);
            expressionStatements.pop();
            return lex;
        case Keyword::switch_:
            lex = statement_switch(lex, funcsp, parent);
            break;
        case Keyword::if_:
            canFallThrough = false;
            lex = statement_if(lex, funcsp, parent);
            break;
        case Keyword::else_:
            canFallThrough = false;
            error(ERR_MISPLACED_ELSE);
            skip(&lex, Keyword::else_);
            before->nosemi = true;
            break;
        case Keyword::case_:
            EndOfCaseGroup(funcsp, parent);
            while (KW(lex) == Keyword::case_)
            {
                if (Optimizer::cparams.prm_cplusplus)
                    HandleEndOfCase(parent);
                lex = statement_case(lex, funcsp, parent);
                if (Optimizer::cparams.prm_cplusplus)
                    HandleStartOfCase(parent);
            }
            StartOfCaseGroup(funcsp, parent);
            lex = nononconststatement(lex, funcsp, parent, false);
            before->nosemi = true;
            expressionStatements.pop();
            return lex;
        case Keyword::default_:
            EndOfCaseGroup(funcsp, parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            lex = statement_default(lex, funcsp, parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleStartOfCase(parent);
            StartOfCaseGroup(funcsp, parent);
            lex = nononconststatement(lex, funcsp, parent, false);
            before->nosemi = true;
            expressionStatements.pop();
            return lex;
        case Keyword::continue_:
            lex = statement_continue(lex, funcsp, parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::break_:
            lex = statement_break(lex, funcsp, parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::goto_:
            lex = statement_goto(lex, funcsp, parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::return_:
            lex = statement_return(lex, funcsp, parent);
            if (Optimizer::cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case Keyword::semicolon_:
            canFallThrough = false;
            break;
        case Keyword::asm_:
            canFallThrough = false;
            lex = statement_asm(lex, funcsp, parent);
            expressionStatements.pop();
            return lex;
        case Keyword::seh_try_:
            canFallThrough = false;
            lex = statement_SEH(lex, funcsp, parent);
            break;
        case Keyword::seh_catch_:
        case Keyword::seh_finally_:
        case Keyword::seh_fault_:
            canFallThrough = false;
            error(ERR_SEH_HANDLER_WITHOUT_TRY);
            lex = statement_SEH(lex, funcsp, parent);
            break;
        default: {
            canFallThrough = false;
            bool structured = false;

            if (((startOfType(lex, &structured, false) &&
                  ((!Optimizer::cparams.prm_cplusplus &&
                    ((Optimizer::architecture != ARCHITECTURE_MSIL) || !Optimizer::cparams.msilAllowExtensions)) ||
                   resolveToDeclaration(lex, structured)))) ||
                MATCHKW(lex, Keyword::namespace_) || MATCHKW(lex, Keyword::using_) || MATCHKW(lex, Keyword::constexpr_) || MATCHKW(lex, Keyword::decltype_) ||
                MATCHKW(lex, Keyword::static_assert_))
            {
                if (Optimizer::cparams.c_dialect < Dialect::c99 && !Optimizer::cparams.prm_cplusplus)
                {
                    error(ERR_NO_DECLARATION_HERE);
                }
                if (viacontrol)
                {
                    AllocateLocalContext(parent, funcsp, codeLabel++);
                }
                while (((startOfType(lex, &structured, false) &&
                         ((!Optimizer::cparams.prm_cplusplus && (Optimizer::architecture != ARCHITECTURE_MSIL)) ||
                          resolveToDeclaration(lex, structured)))) ||
                       MATCHKW(lex, Keyword::namespace_) || MATCHKW(lex, Keyword::using_) || MATCHKW(lex, Keyword::decltype_) ||
                       MATCHKW(lex, Keyword::static_assert_) || MATCHKW(lex, Keyword::constexpr_))
                {
                        std::list<STATEMENT*>* prev = before->statements;
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
                        markInitializers(*before->statements);
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
                return lex;
            }
            else
            {
                isCallNoreturnFunction = false;
                lex = statement_expr(lex, funcsp, parent);
                before->needlabel = isCallNoreturnFunction;
            }
        }
    }
    if (before->nosemi && expressionReturns.size())
        expressionReturns.back() = std::move(std::pair<EXPRESSION*, TYPE*>(nullptr, &stdvoid));
    expressionStatements.pop();
    if (MATCHKW(lex, Keyword::semicolon_))
    {
        before->hassemi = true;
        skip(&lex, Keyword::semicolon_);
    }
    else
        before->hassemi = false;
    basisAttribs = {};
    return lex;
}
static bool thunkmainret(SYMBOL* funcsp, std::list<BLOCKDATA*>& parent, bool always)
{
    if (always || (!strcmp(funcsp->name, "main") && !funcsp->sb->parentClass && !funcsp->sb->parentNameSpace))
    {
        STATEMENT* s = stmtNode(nullptr, parent, StatementNode::return_);
        s->select = intNode(ExpressionNode::c_i_, 0);
        return true;
    }
    return false;
}
static void thunkThisReturns(std::list<STATEMENT*>* st, EXPRESSION* thisptr)
{
    for (auto stmt : *st)
    {
        if (stmt->lower)
            thunkThisReturns(stmt->lower, thisptr);
        if (stmt->type == StatementNode::return_)
            stmt->select = thisptr;
    }
}
static void insertXCInfo(SYMBOL* funcsp)
{
    char name[2048];
    SYMBOL* sym;
    makeXCTab(funcsp);
    Optimizer::my_sprintf(name, "@.xc%s", funcsp->sb->decoratedName);
    sym = makeID(StorageClass::global_, &stdpointer, nullptr, litlate(name));
    sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
    sym->sb->decoratedName = sym->name;
    sym->sb->allocate = true;
    sym->sb->attribs.inheritable.used = sym->sb->assigned = true;
    funcsp->sb->xc->xcInitLab = codeLabel++;
    funcsp->sb->xc->xcDestLab = codeLabel++;
    funcsp->sb->xc->xclab = sym;
}
static bool isvoidreturntype(TYPE* tp, SYMBOL* funcsp)
{
    if (isvoid(tp))
        return true;
    if (tp->type == BasicType::templateparam_)
    {
        if (tp->templateParam->second->byClass.val)
            return isvoid(tp->templateParam->second->byClass.val);
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
                    return isvoid(tp);
                }
            }
        }
    }
    return false;
}
LEXLIST* compound(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& parent, bool first)
{
    auto before = parent.front();
    BLOCKDATA* blockstmt = Allocate<BLOCKDATA>();
    preProcessor->MarkStdPragma();
    STATEMENT* st;
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
        for (auto sp2 : *basetype(funcsp->tp)->syms)
        {
            if (!Optimizer::cparams.prm_cplusplus && Optimizer::cparams.c_dialect < Dialect::c2x && sp2->tp->type != BasicType::ellipse_ && !isvoid(sp2->tp) && sp2->sb->anonymous)
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
                thunkConstructorHead(parent, funcsp->sb->parentClass, funcsp, basetype(funcsp->tp)->syms, true, false, false);
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
        std::list<BLOCKDATA*> tempBlockData = {blockstmt};
        expressionStatements.push(&tempBlockData);
        // have to defer so we can get expression like constructor calls
        while (startOfType(lex, nullptr, false))
        {
            blockstmt->statements = nullptr;
            declareAndInitialize = false;
            lex = declare(lex, funcsp, nullptr, StorageClass::auto_, Linkage::none_, parent, false, false, false, AccessLevel::public_);
            if (blockstmt->statements)
            {
                markInitializers(*blockstmt->statements);
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
            lex = statement(lex, funcsp, parent, false);
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
        
        return lex;
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
            thunkDestructorTail(parent, funcsp->sb->parentClass, funcsp, basetype(funcsp->tp)->syms, false);
        }
    }
    if (Optimizer::cparams.prm_cplusplus)
        HandleEndOfSwitchBlock(parent);
    FreeLocalContext(parent, funcsp, codeLabel++);
    if (first && !blockstmt->needlabel && !isvoidreturntype(basetype(funcsp->tp)->btp, funcsp) &&
        basetype(funcsp->tp)->btp->type != BasicType::auto_ && !funcsp->sb->isConstructor && basetype(funcsp->tp)->btp->type != BasicType::void_)
    {
        if (funcsp->sb->attribs.inheritable.linkage3 == Linkage::noreturn_)
        {
            // explicitly do nothing, refer to https://github.com/LADSoft/OrangeC/issues/651 for more information
            // Keeping this here prevents nonsensical errors such as "FUNCTION SHOULD RETURN VALUE!!!!" when a function is marked
            // noreturn. Noreturn functions can have non-void return types in order for things to work such as in ObjIeee.h's ThrowSyntax functions
        }
        else if (Optimizer::cparams.c_dialect >= Dialect::c99 || Optimizer::cparams.c_dialect >= Dialect::c11 || Optimizer::cparams.c_dialect >= Dialect::c2x || Optimizer::cparams.prm_cplusplus)
        {
            if (!thunkmainret(funcsp, parent, false))
            {
                if (isref(basetype(funcsp->tp)->btp))
                    error(ERR_FUNCTION_RETURNING_REF_SHOULD_RETURN_VALUE);
                else
                {
                    error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
                    if (Optimizer::chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                        thunkmainret(funcsp, parent, true);
                }
            }
        }
        else
        {
            error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
            if (Optimizer::chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                thunkmainret(funcsp, parent, true);
        }
    }
    needkw(&lex, Keyword::end_);
    if (first && Optimizer::cparams.prm_cplusplus)
    {
        if (funcsp->sb->hasTry)
        {
            stmtNode(nullptr, parent, StatementNode::return_);
            auto it = blockstmt->statements->end();
            --it;
            lex = statement_catch(lex, funcsp, parent, retlab, startlab, 0);
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
            FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
            STATEMENT* st = stmtNode(lex, parent, StatementNode::expr_);
            INITLIST* arg1 = Allocate<INITLIST>();  // exception table
            makeXCTab(funcsp);
            sym = (SYMBOL*)basetype(sym->tp)->syms->front();
            funcparams->ascall = true;
            funcparams->sp = sym;
            funcparams->functp = sym->tp;
            funcparams->fcall = varNode(ExpressionNode::pc_, sym);
            funcparams->arguments = initListListFactory.CreateList();
            funcparams->arguments->push_back(arg1);
            arg1->exp = varNode(ExpressionNode::auto_, funcsp->sb->xc->xctab);
            arg1->tp = &stdpointer;
            st->select = exprNode(ExpressionNode::func_, nullptr, nullptr);
            st->select->v.func = funcparams;
        }
    }
    if (first)
    {
        if (thisptr)
        {
            stmtNode(nullptr, parent, StatementNode::return_);
            thunkThisReturns(blockstmt->statements, thisptr);
        }
    }
    auto it = parent.begin();
    ++it;
    std::list<BLOCKDATA*> dummy(it, parent.end());
    parent.pop_front();
    AddBlock(lex, parent, blockstmt);
    preProcessor->ReleaseStdPragma();
    return lex;
}
void assignParam(SYMBOL* funcsp, int* base, SYMBOL* param)
{
    TYPE* tp = basetype(param->tp);
    param->sb->parent = funcsp;
    if (tp->type == BasicType::ellipse_)
        ellipsePos = *base;
    if (tp->type == BasicType::void_)
        return;
    if (isstructured(tp) && !basetype(tp)->sp->sb->pureDest)
        hasXCInfo = true;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
    {
        // calculate index for CIL
        param->sb->offset = (*base)++;
    }
    else if (!ispointer(tp) && tp->size <= Optimizer::chosenAssembler->arch->parmwidth)
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
static void assignCParams(LEXLIST* lex, SYMBOL* funcsp, int* base, SymbolTable<SYMBOL>* params, TYPE* rv, std::list<BLOCKDATA*>& block)
{
    (void)rv;
    for (auto sym : *params)
    {
        STATEMENT* s = stmtNode(lex, block, StatementNode::varstart_);
        s->select = varNode(ExpressionNode::auto_, sym);
        assignParam(funcsp, base, sym);
    }
}
static void assignPascalParams(LEXLIST* lex, SYMBOL* funcsp, int* base, SymbolTable<SYMBOL>* params, TYPE* rv, std::list<BLOCKDATA*>& block)
{
    std::stack<SYMBOL*> stk;
    for (auto sym : *params)
        stk.push(sym);
    while (!stk.empty())
    {
        assignParam(funcsp, base, stk.top());
        STATEMENT* s = stmtNode(lex, block, StatementNode::varstart_);
        s->select = varNode(ExpressionNode::auto_, stk.top());
        stk.pop();
    }
}
static void assignParameterSizes(LEXLIST* lex, SYMBOL* funcsp, std::list<BLOCKDATA*>& block)
{
    int base;
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
        base = 0;
    else
        base = Optimizer::chosenAssembler->arch->retblocksize;
    if (funcsp->sb->attribs.inheritable.linkage == Linkage::pascal_)
    {
        assignPascalParams(lex, funcsp, &base, basetype(funcsp->tp)->syms, basetype(funcsp->tp)->btp, block);
    }
    else
    {
        if ((isstructured(basetype(funcsp->tp)->btp) && !basetype(basetype(funcsp->tp)->btp)->sp->sb->structuredAliasType) || basetype(basetype(funcsp->tp)->btp)->type == BasicType::memberptr_ || isbitint(basetype(funcsp->tp)->btp))
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
            assignParam(funcsp, &base, (SYMBOL*)params->p);
            params = params->next;
        }
        */
        assignCParams(lex, funcsp, &base, basetype(funcsp->tp)->syms, basetype(funcsp->tp)->btp, block);
    }
    funcsp->sb->paramsize = base - Optimizer::chosenAssembler->arch->retblocksize;
}
static void checkUndefinedStructures(SYMBOL* funcsp)
{
    TYPE* tp = basetype(funcsp->tp)->btp;
    if (isstructured(tp) && !basetype(tp)->sp->tp->syms)
    {
        tp = PerformDeferredInitialization(tp, funcsp);
        if (!basetype(tp)->sp->tp->syms)
        {
            currentErrorLine = 0;
            errorsym(ERR_STRUCT_NOT_DEFINED, basetype(tp)->sp);
        }
    }
    for (auto sym : *basetype(funcsp->tp)->syms)
    {
        TYPE* tp = basetype(sym->tp);
        if (isstructured(tp) && !basetype(tp)->sp->tp->syms)
        {
            if (basetype(tp)->sp->sb->templateLevel)
            {
                auto sym1 = basetype(tp)->sp;
                std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> stk;
                while (!stk.empty())
                    stk.pop();
                auto it = sym1->templateParams->begin();
                auto ite = sym1->templateParams->end();
                for ( ; it != ite ;)
                {
                    if (it->second->packed && it->second->byPack.pack)
                    {
                        stk.push(it);
                        stk.push(ite);
                        ite = it->second->byPack.pack->end();
                        it = it->second->byPack.pack->begin();
                    }
                    it->second->byClass.dflt = it->second->byClass.val;
                    if (++it != ite && !stk.empty())
                    {
                        ite = stk.top();
                        stk.pop();
                        it = stk.top();
                        stk.pop();
                        ++it;
                    }
                }
                sym1 = GetClassTemplate(sym1, sym1->templateParams, false);
                if (sym1)
                    basetype(tp)->sp = sym1;
            }
            tp = PerformDeferredInitialization(tp, funcsp);
            if (!basetype(tp)->sp->tp->syms)
            {
                currentErrorLine = 0;
                errorsym(ERR_STRUCT_NOT_DEFINED, basetype(tp)->sp);
            }
        }
    }
}
static int inlineStatementCount(std::list<STATEMENT*>* block)
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
                    rv += inlineStatementCount(stmt->lower);
                    break;
                case StatementNode::block_:
                    rv++;
                    rv += inlineStatementCount(stmt->lower) + inlineStatementCount(stmt->blockTail);
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
static void handleInlines(SYMBOL* funcsp)
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

            for (auto head : *basetype(funcsp->tp)->syms)
            {
                if (isstructured(head->tp))
                {
                    funcsp->sb->noinline = true;
                    break;
                }
            }
        }
        if (inlineStatementCount(funcsp->sb->inlineFunc.stmt) > 100)
        {
            funcsp->sb->noinline = true;
        }
    }
}
void parseNoexcept(SYMBOL* funcsp)
{
    if (funcsp->sb->deferredNoexcept && funcsp->sb->deferredNoexcept != (LEXLIST*)-1)
    {
        dontRegisterTemplate++;
        LEXLIST* lex = SetAlternateLex(funcsp->sb->deferredNoexcept);
        STRUCTSYM s, t;
        int n = PushTemplateNamespace(funcsp);
        if (funcsp->sb->parentClass)
        {
            s.str = funcsp->sb->parentClass;
            addStructureDeclaration(&s);
            t.tmpl = funcsp->sb->parentClass->templateParams;
            if (t.tmpl)
                addTemplateDeclaration(&t);
        }
        TYPE* tp = nullptr;
        EXPRESSION* exp = nullptr;
        AllocateLocalContext(emptyBlockdata, nullptr, 0);
        for (auto sp2 : *basetype(funcsp->tp)->syms)
        {
            localNameSpace->front()->syms->Add(sp2);
        }
        lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
        FreeLocalContext(emptyBlockdata, nullptr, 0);
        if (!IsConstantExpression(exp, false, false))
        {
            if (!templateNestingCount)
                error(ERR_CONSTANT_VALUE_EXPECTED);
        }
        else
        {
            funcsp->sb->xcMode = exp->v.i ? xc_none : xc_all;
            if (exp->v.i)
                funcsp->sb->noExcept = true;
        }
        SetAlternateLex(nullptr);
        if (funcsp->sb->parentClass)
        {
            if (t.tmpl)
                dropStructureDeclaration();
            dropStructureDeclaration();
        }
        PopTemplateNamespace(n);
        funcsp->sb->deferredNoexcept = (LEXLIST*)-1;
        dontRegisterTemplate--;
    }
}
LEXLIST* body(LEXLIST* lex, SYMBOL* funcsp)
{
    int oldNoexcept = funcsp->sb->noExcept;
    if (bodyIsDestructor)
        funcsp->sb->noExcept = true;
    if (funcsp->sb->isDestructor)
        bodyIsDestructor++;
    int oldNestingCount = templateNestingCount;
    int n1;
    bool oldsetjmp_used = Optimizer::setjmp_used;
    bool oldfunctionHasAssembly = Optimizer::functionHasAssembly;
    bool oldDeclareAndInitialize = declareAndInitialize;
    bool oldHasXCInfo = hasXCInfo;
    bool oldFunctionCanThrow = functionCanThrow;
    SymbolTable<SYMBOL>* oldSyms = localNameSpace->front()->syms;
    SymbolTable<SYMBOL>* oldLabelSyms = labelSyms;
    SYMBOL* oldtheCurrentFunc = theCurrentFunc;
    BLOCKDATA* block = Allocate<BLOCKDATA>();
    std::list<BLOCKDATA*> parent { block };
    std::list<STATEMENT*>* startStmt;
    int oldCodeLabel = codeLabel;
    int oldMatchReturnTypes = matchReturnTypes;
    bool oldHasFuncCall = hasFuncCall;
    int oldExpressionCount = expressions;
    int oldControlSequences = controlSequences;
    bool oldNoExcept = noExcept;
    int oldEllipsePos = ellipsePos;
    ellipsePos = 0;
    constexprfunctioninit(true);
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
    basetype(funcsp->tp)->btp = ResolveTemplateSelectors(funcsp, basetype(funcsp->tp)->btp);
    if (inTemplateHeader)
        templateNestingCount--;
    checkUndefinedStructures(funcsp);
    parseNoexcept(funcsp);
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
        assignParameterSizes(lex, funcsp, parent);
        funcsp->sb->startLine = lex->data->errline;
        lex = compound(lex, funcsp, parent, true);
        if (isstructured(basetype(funcsp->tp)->btp))
            assignParameterSizes(lex, funcsp, parent);
        refreshBackendParams(funcsp);
        checkUnlabeledReferences(parent);
        checkGotoPastVLA(block->statements, true);
        if (isautotype(basetype(funcsp->tp)->btp) && !templateNestingCount)
            basetype(funcsp->tp)->btp = &stdvoid;  // return value for auto function without return statements
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
            funcsp->sb->inlineFunc.stmt->push_back(stmtNode(lex, emptyBlockdata, StatementNode::block_));
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
                if (basetype(funcsp->tp)->syms->size())
                {
                    if (basetype(funcsp->tp)->syms->back()->tp->type == BasicType::ellipse_)
                        funcsp->sb->attribs.inheritable.isInline = funcsp->sb->promotedToInline =
                            false;
                }
            }
        }
        if (IsCompiler() && funcNesting == 1)  // top level function
            localFree();
        handleInlines(funcsp);
    }
    constexprfunctioninit(false);
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
    templateNestingCount = oldNestingCount;
    if (funcsp->sb->isDestructor)
        bodyIsDestructor--;
    return lex;
}
void bodygen(SYMBOL* funcsp)
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