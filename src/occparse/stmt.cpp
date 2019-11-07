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

/* declare in select has multiple vars */
#include "compiler.h"
#include <limits.h>
#include <assert.h>
#include "PreProcessor.h"

extern TYPE stdint;
extern ARCH_DEBUG* chosenDebugger;
extern ARCH_ASM* chosenAssembler;
extern TYPE stdint, stdvoid, stdpointer;
extern HASHTABLE* labelSyms;
extern NAMESPACEVALUELIST *globalNameSpace, *localNameSpace;
extern enum e_kw skim_colon[], skim_end[];
extern enum e_kw skim_closepa[];
extern enum e_kw skim_semi[];
extern SYMBOL* theCurrentFunc;
extern TYPE stdpointer;
extern const char* overloadNameTab[];
extern LEXCONTEXT* context;
extern TYPE stdXC;
extern TYPE std__string, stdbool;
extern int currentErrorLine;
extern PreProcessor* preProcessor;

extern int templateNestingCount;
extern int nextLabel;
extern bool setjmp_used;
extern bool functionHasAssembly;

bool isCallExit;

int funcNesting;
int funcLevel;
int tryLevel;

bool hasFuncCall;
bool hasXCInfo;
int startlab, retlab;
int codeLabel;
bool declareAndInitialize;
bool functionCanThrow;

LINEDATA *linesHead, *linesTail;

static int matchReturnTypes;
static int endline;

static LEXEME* autodeclare(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, BLOCKDATA* parent, int asExpression);

static BLOCKDATA* caseDestructBlock;

void statement_ini(bool global)
{
    linesHead = linesTail = nullptr;
    functionCanThrow = false;
    funcNesting = 0;
    funcLevel = 0;
    caseDestructBlock = nullptr;
    matchReturnTypes = false;
    tryLevel = 0;
}
bool msilManaged(SYMBOL* s)
{
#ifdef PARSER_ONLY
    return false;
#else
    return msil_managed(SymbolManager::Get(s));
#endif
}

void InsertLineData(int lineno, int fileindex, const char* fname, char* line)
{
    LINEDATA* ld;
    IncGlobalFlag();
    ld = (LINEDATA*)Alloc(sizeof(LINEDATA));
    ld->file = fname;
    ld->line = litlate(line);
    ld->lineno = lineno;
    ld->fileindex = fileindex;
    if (linesHead)
        linesTail = linesTail->next = ld;
    else
        linesHead = linesTail = ld;
    DecGlobalFlag();
}
void FlushLineData(const char* file, int lineno)
{
    while (linesHead)
    {
        if (strcmp(file, linesHead->file) != 0 || linesHead->lineno < lineno)
            linesHead = linesHead->next;
        else
            break;
    }
}
STATEMENT* currentLineData(BLOCKDATA* parent, LEXEME* lex, int offset)
{
    STATEMENT* rv = nullptr;
    LINEDATA *ld = linesHead, **p = &ld;
    int lineno;
    const char* file;
    if (!lex)
        return nullptr;
    lineno = lex->realline + offset;
    file = lex->file;
    while (*p && (strcmp((*p)->file, file) != 0 || lineno >= (*p)->lineno))
    {
        p = &(*p)->next;
    }
    linesHead = *p;
    *p = nullptr;
    if (ld)
    {
        rv = stmtNode(lex, parent, st_line);
        rv->lineData = ld;
    }
    return rv;
}
STATEMENT* stmtNode(LEXEME* lex, BLOCKDATA* parent, enum e_stmt stype)
{
    STATEMENT* st = (STATEMENT*)Alloc(sizeof(STATEMENT));
    if (!lex)
        lex = context->cur ? context->cur->prev : context->last;
    st->type = stype;
    st->charpos = 0;
    st->line = lex->line;
    st->file = lex->file;
    st->parent = parent;
    if (parent)
    {
        if (parent->head)
            parent->tail = parent->tail->next = st;
        else
            parent->head = parent->tail = st;
    }
    return st;
}
static void AddBlock(LEXEME* lex, BLOCKDATA* parent, BLOCKDATA* newbl)
{
    STATEMENT* st = stmtNode(lex, parent, st_block);
    st->blockTail = newbl->blockTail;
    st->lower = newbl->head;
}
static bool isselecttrue(EXPRESSION* exp)
{
    if (isintconst(exp))
        return !!exp->v.i;
    return false;
}
static bool isselectfalse(EXPRESSION* exp)
{
    if (isintconst(exp))
        return !exp->v.i;
    return false;
}
static void markInitializers(STATEMENT* prev)
{
    if (prev)
    {
        prev = prev->next;
        while (prev)
        {
            if (prev->type == st_expr)
                prev->hasdeclare = true;
            prev = prev->next;
        }
    }
}
static LEXEME* selection_expression(LEXEME* lex, BLOCKDATA* parent, EXPRESSION** exp, SYMBOL* funcsp, enum e_kw kw,
                                    bool* declaration)
{
    TYPE* tp = nullptr;
    bool hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, true);
    (void)parent;
    if (startOfType(lex, false) && (!cparams.prm_cplusplus || resolveToDeclaration(lex)))
    {
        if (declaration)
            *declaration = true;
        if ((cparams.prm_cplusplus && kw != kw_do && kw != kw_else) || (cparams.prm_c99 && (kw == kw_for || kw == kw_rangefor)))
        {
            // empty
        }
        else
        {
            error(ERR_NO_DECLARATION_HERE);
        }
        /* fixme need type */
        lex = autodeclare(lex, funcsp, &tp, exp, parent, (kw != kw_for) | (kw == kw_rangefor ? _F_NOCHECKAUTO : 0));
        if (tp->type == bt_memberptr)
        {
            *exp = exprNode(en_mp_as_bool, *exp, nullptr);
            (*exp)->size = tp->size;
            tp = &stdint;
        }
    }
    else
    {
        if (hasAttributes)
            error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
        /*		bool openparen = MATCHKW(lex, openpa); */
        if (declaration)
            *declaration = false;
        lex = expression(lex, funcsp, nullptr, &tp, exp, kw != kw_for && kw != kw_rangefor ? _F_SELECTOR : 0);
        if (tp)
        {
            if (tp->type == bt_memberptr)
            {
                *exp = exprNode(en_mp_as_bool, *exp, nullptr);
                (*exp)->size = tp->size;
                tp = &stdint;
            }
            optimize_for_constants(exp);
        }
    }

    if (cparams.prm_cplusplus && tp && isstructured(tp) && kw != kw_for && kw != kw_rangefor)
    {
        if (!castToArithmeticInternal(false, &tp, exp, (enum e_kw) - 1, &stdbool, false))
            if (!castToArithmeticInternal(false, &tp, exp, (enum e_kw) - 1, &stdint, false))
                if (!castToPointer(&tp, exp, (enum e_kw) - 1, &stdpointer))
                    errortype(ERR_CANNOT_CONVERT_TYPE, tp, &stdint);
    }
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    else if (kw == kw_switch && !isint(tp) && basetype(tp)->type != bt_enum)
        error(ERR_SWITCH_SELECTION_INTEGRAL);
    else if (kw != kw_for && kw != kw_rangefor && isstructured(tp))
    {
        error(ERR_ILL_STRUCTURE_OPERATION);
    }
    return lex;
}
static BLOCKDATA* getCommonParent(BLOCKDATA* src, BLOCKDATA* dest)
{
    BLOCKDATA* top = src;
    while (top)
    {
        BLOCKDATA* test = dest;
        while (test && test != top)
            test = test->next;
        if (test)
        {
            top = test;
            break;
        }
        top = top->next;
    }
    return top;
}
void makeXCTab(SYMBOL* funcsp)
{
    SYMBOL* sym;
    if (!funcsp->xc)
    {
        funcsp->xc = (xcept*)Alloc(sizeof(struct xcept));
    }
    if (!funcsp->xc->xctab)
    {
        sym = makeID(sc_auto, &stdXC, nullptr, "$$xctab");
        sym->decoratedName = sym->errname = sym->name;
        sym->allocate = true;
        sym->attribs.inheritable.used = sym->assigned = true;
        insert(sym, localNameSpace->valueData->syms);
        funcsp->xc->xctab = sym;
    }
}
static void thunkCatchCleanup(STATEMENT* st, SYMBOL* funcsp, BLOCKDATA* src, BLOCKDATA* dest)
{
    BLOCKDATA *top = dest ? getCommonParent(src, dest) : nullptr, *srch = src;
    while (srch != top)
    {
        if (srch->type == kw_catch)
        {
            SYMBOL* sym = namespacesearch("_CatchCleanup", globalNameSpace, false, false);
            if (sym)
            {
                FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                STATEMENT** find = &src->head;
                INITLIST* arg1 = (INITLIST*)Alloc(sizeof(INITLIST));  // exception table
                makeXCTab(funcsp);
                sym = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
                funcparams->ascall = true;
                funcparams->sp = sym;
                funcparams->functp = sym->tp;
                funcparams->fcall = varNode(en_pc, sym);
                funcparams->arguments = arg1;
                arg1->exp = varNode(en_auto, funcsp->xc->xctab);
                arg1->tp = &stdpointer;
                while (*find && *find != st)
                    find = &(*find)->next;
                if (*find == st)
                {
                    *find = (STATEMENT*)Alloc(sizeof(STATEMENT));
                    **find = *st;
                    (*find)->next = st;
                    (*find)->type = st_expr;
                    (*find)->select = exprNode(en_func, nullptr, nullptr);
                    (*find)->select->v.func = funcparams;
                }
            }
            break;
        }
        srch = srch->next;
    }
    srch = dest;
    while (srch != top)
    {
        if (srch->type == kw_try || srch->type == kw_catch)
        {
            error(ERR_GOTO_INTO_TRY_OR_CATCH_BLOCK);
            break;
        }
        srch = srch->next;
    }
}
static void ThunkUndestructSyms(HASHTABLE* syms)
{
    SYMLIST* hr = syms->table[0];
    while (hr)
    {
        SYMBOL* sym = hr->p;
        sym->destructed = true;
        hr = hr->next;
    }
}
static void thunkRetDestructors(EXPRESSION** exp, HASHTABLE* top, HASHTABLE* syms)
{
    if (syms)
    {
        if (syms != top)
        {
            thunkRetDestructors(exp, top, syms->chain);
            destructBlock(exp, syms->table[0], false);
        }
    }
}
static void thunkGotoDestructors(EXPRESSION** exp, BLOCKDATA* gotoTab, BLOCKDATA* labelTab)
{
    // find the common parent
    BLOCKDATA* realtop;
    BLOCKDATA* top = getCommonParent(gotoTab, labelTab);
    if (gotoTab->next != top)
    {
        realtop = gotoTab;
        while (realtop->next != top)
            realtop = realtop->next;
        thunkRetDestructors(exp, realtop->table, gotoTab->table);
    }
}
static void InSwitch() {}
static void HandleStartOfCase(BLOCKDATA* parent)
{
    // this is a little buggy in that it doesn't check to see if we are already in a switch
    // statement, however if we aren't we should get a compile erroir that would halt program generation anyway
    if (parent != caseDestructBlock)
    {
        parent->caseDestruct = caseDestructBlock;
        caseDestructBlock = parent;
    }
}
static void HandleEndOfCase(BLOCKDATA* parent)
{
    if (parent == caseDestructBlock)
    {
        EXPRESSION* exp = nullptr;
        STATEMENT* st;
        // the destruct is only used for endin
        destructBlock(&exp, localNameSpace->valueData->syms->table[0], false);
        if (exp)
        {
            st = stmtNode(nullptr, parent, st_nop);
            st->destexp = exp;
        }
        ThunkUndestructSyms(localNameSpace->valueData->syms);
    }
}
static void HandleEndOfSwitchBlock(BLOCKDATA* parent)
{
    if (parent == caseDestructBlock)
    {
        caseDestructBlock = caseDestructBlock->caseDestruct;
    }
}
static LEXEME* statement_break(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    BLOCKDATA *breakableStatement = parent, *last = nullptr;
    EXPRESSION* exp = nullptr;
    (void)lex;
    (void)funcsp;
    (void)parent;
    while (breakableStatement &&
           (breakableStatement->type == begin || breakableStatement->type == kw_try || breakableStatement->type == kw_catch ||
            breakableStatement->type == kw_if || breakableStatement->type == kw_else))
    {
        last = breakableStatement;
        breakableStatement = breakableStatement->next;
    }

    if (!breakableStatement)
        error(ERR_BREAK_NO_LOOP);
    else
    {
        STATEMENT* st;
        currentLineData(parent, lex, 0);
        if (last)
            thunkRetDestructors(&exp, last->table, localNameSpace->valueData->syms);
        st = stmtNode(lex, parent, st_goto);
        st->label = breakableStatement->breaklabel;
        st->destexp = exp;
        thunkCatchCleanup(st, funcsp, parent, breakableStatement);
        parent->needlabel = true;
        breakableStatement->needlabel = false;
        breakableStatement->hasbreak = true;
    }
    return getsym();
}
static LEXEME* statement_case(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    long long val;
    BLOCKDATA dummy;
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    BLOCKDATA* switchstmt = parent;
    lex = getsym();
    while (switchstmt && switchstmt->type != kw_switch)
    {
        switchstmt = switchstmt->next;
    }
    if (!switchstmt)
    {
        memset(&dummy, 0, sizeof(dummy));
        switchstmt = &dummy;
        switchstmt->type = kw_switch;
        error(ERR_CASE_NO_SWITCH);
    }
    else
    {
        switchstmt->needlabel = false;
        parent->needlabel = false;
    }

    lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
    if (!tp)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (isintconst(exp))
    {
        CASEDATA **cases = &switchstmt->cases, *data;
        const char* fname = lex->file;
        int line = lex->line;
        val = exp->v.i;
        /* need error: lost conversion on case value */
        while (*cases)
        {
            if ((*cases)->val == val)
            {
                char buf[256];
                my_sprintf(buf, LLONG_FORMAT_SPECIFIER, val);
                preverror(ERR_DUPLICATE_CASE, buf, (*cases)->file, (*cases)->line);
                break;
            }
            cases = &(*cases)->next;
        }
        if (!*cases)
        {
            STATEMENT* st = stmtNode(lex, parent, st_label);
            st->label = codeLabel++;
            codeLabel++; // reserve a label in case a bingen is used in the back end...
            data = (CASEDATA*)Alloc(sizeof(CASEDATA));
            data->val = val;
            data->label = st->label;
            data->file = fname;
            data->line = line;
            *cases = data;
        }
        needkw(&lex, colon);
    }
    else
    {
        error(ERR_CASE_INTEGER_CONSTANT);
        errskim(&lex, skim_colon);
        skip(&lex, colon);
    }
    return lex;
}
static LEXEME* statement_continue(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    BLOCKDATA* continuableStatement = parent;
    BLOCKDATA* last = nullptr;
    EXPRESSION* exp = nullptr;
    (void)lex;
    (void)funcsp;
    while (continuableStatement && (continuableStatement->type == kw_switch || continuableStatement->type == begin ||
                                    continuableStatement->type == kw_try || continuableStatement->type == kw_catch ||
                                    continuableStatement->type == kw_if || continuableStatement->type == kw_else))
    {
        last = continuableStatement;
        continuableStatement = continuableStatement->next;
    }
    if (!continuableStatement)
        error(ERR_CONTINUE_NO_LOOP);
    else
    {
        STATEMENT* st;
        if (last)
            thunkRetDestructors(&exp, last->table, localNameSpace->valueData->syms);
        currentLineData(parent, lex, 0);
        st = stmtNode(lex, parent, st_goto);
        st->label = continuableStatement->continuelabel;
        st->destexp = exp;
        thunkCatchCleanup(st, funcsp, parent, continuableStatement);
        parent->needlabel = true;
    }
    return getsym();
}
static LEXEME* statement_default(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    BLOCKDATA* defaultableStatement = parent;
    (void)lex;
    (void)funcsp;
    while (defaultableStatement && defaultableStatement->type != kw_switch)
        defaultableStatement = defaultableStatement->next;
    lex = getsym();
    if (!defaultableStatement)
        error(ERR_DEFAULT_NO_SWITCH);
    else
    {
        STATEMENT* st = stmtNode(lex, parent, st_label);
        st->label = codeLabel++;
        if (defaultableStatement->defaultlabel != -1)
            error(ERR_SWITCH_HAS_DEFAULT);
        defaultableStatement->defaultlabel = st->label;
        defaultableStatement->needlabel = false;
        parent->needlabel = false;
    }
    needkw(&lex, colon);
    return lex;
}
static LEXEME* statement_do(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    BLOCKDATA* dostmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
    STATEMENT *st, *lastLabelStmt;
    EXPRESSION* select = nullptr;
    int addedBlock = 0;
    int loopLabel = codeLabel++;
    lex = getsym();
    dostmt->breaklabel = codeLabel++;
    dostmt->continuelabel = codeLabel++;
    dostmt->next = parent;
    dostmt->type = kw_do;
    dostmt->table = localNameSpace->valueData->syms;
    currentLineData(dostmt, lex, 0);
    st = stmtNode(lex, dostmt, st_label);
    st->label = loopLabel;
    if (cparams.prm_cplusplus || cparams.prm_c99)
    {
        addedBlock++;
        AllocateLocalContext(parent, funcsp, codeLabel++);
    }
    do
    {
        lastLabelStmt = dostmt->tail;
        lex = statement(lex, funcsp, dostmt, true);
    } while (lex && dostmt->tail != lastLabelStmt && dostmt->tail->purelabel);
    if (cparams.prm_cplusplus || cparams.prm_c99)
    {
        addedBlock--;
        FreeLocalContext(dostmt, funcsp, codeLabel++);
    }
    parent->nosemi = false;
    if (MATCHKW(lex, kw_while))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        if (MATCHKW(lex, openpa))
        {
            lex = getsym();
            lex = selection_expression(lex, dostmt, &select, funcsp, kw_do, nullptr);
            if (!MATCHKW(lex, closepa))
            {
                error(ERR_DOWHILE_NEEDS_CLOSEPA);
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
            else
                lex = getsym();
            st = stmtNode(lex, dostmt, st_label);
            st->label = dostmt->continuelabel;
            currentLineData(dostmt, lex, 0);
            st = stmtNode(lex, dostmt, st_select);
            st->select = select;
            if (!dostmt->hasbreak && (dostmt->needlabel || isselecttrue(st->select)))
                parent->needlabel = true;
            st->label = loopLabel;
            st = stmtNode(lex, dostmt, st_label);
            st->label = dostmt->breaklabel;
        }
        else
        {
            error(ERR_DOWHILE_NEEDS_OPENPA);
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
    }
    else
    {
        parent->nosemi = true;
        error(ERR_DO_STMT_NEEDS_WHILE);
        errskim(&lex, skim_semi);
        skip(&lex, semicolon);
    }
    while (addedBlock--)
        FreeLocalContext(dostmt, funcsp, codeLabel++);
    AddBlock(lex, parent, dostmt);
    return lex;
}
static LEXEME* statement_for(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    BLOCKDATA* forstmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
    STATEMENT *st, *lastLabelStmt;
    STATEMENT* forline;
    int addedBlock = 0;
    EXPRESSION *init = nullptr, *before = nullptr, *select = nullptr;
    int loopLabel = codeLabel++, testlabel = codeLabel++;
    forstmt->breaklabel = codeLabel++;
    forstmt->continuelabel = codeLabel++;
    forstmt->next = parent;
    forstmt->type = kw_for;
    forstmt->table = localNameSpace->valueData->syms;
    currentLineData(forstmt, lex, -1);
    forline = currentLineData(nullptr, lex, 0);
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        bool declaration = false;
        lex = getsym();
        if (!MATCHKW(lex, semicolon))
        {
            bool hasColon = false;
            if ((cparams.prm_cplusplus && !cparams.prm_oldfor) || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            if (cparams.prm_cplusplus)
            {
                LEXEME* origLex = lex;
                while (lex && !MATCHKW(lex, semicolon) && !MATCHKW(lex, colon))
                    lex = getsym();
                hasColon = MATCHKW(lex, colon);
                lex = prevsym(origLex);
            }

            lex = selection_expression(lex, forstmt, &init, funcsp, hasColon ? kw_rangefor : kw_for, &declaration);
            if (cparams.prm_cplusplus && !cparams.prm_oldfor && declaration && MATCHKW(lex, colon))
            {
                // range based for statement
                // we will ignore 'init'.
                TYPE* selectTP = nullptr;
                SYMBOL* declSP = (SYMBOL*)localNameSpace->valueData->syms->table[0]->p;
                EXPRESSION* declExp;
                if (!declSP)
                {
                    diag("statement_for: Cannot get range based range variable");
                    declExp = intNode(en_c_i, 0);
                }
                else
                {
                    if (declSP->init)
                    {
                        error(ERR_FORRANGE_DECLARATOR_NO_INIT);
                    }
                    declSP->dest = nullptr;
                    declExp = varNode(en_auto, declSP);
                    declSP->assigned = declSP->attribs.inheritable.used = true;
                }
                lex = getsym();
                if (MATCHKW(lex, begin))
                {
                    assert(0);
                }
                else
                {
                    lex = expression_no_comma(lex, funcsp, nullptr, &selectTP, &select, nullptr, 0);
                }
                if (!selectTP || selectTP->type == bt_any)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                {
                    EXPRESSION *ibegin = nullptr, *iend = nullptr;
                    SYMBOL *sbegin = nullptr, *send = nullptr;
                    TYPE* iteratorType = nullptr;
                    TYPE* tpref = (TYPE*)Alloc(sizeof(TYPE));
                    EXPRESSION* rangeExp = anonymousVar(sc_auto, tpref);
                    SYMBOL* rangeSP = rangeExp->v.sp;
                    if (isstructured(selectTP))
                        selectTP = basetype(selectTP)->sp->tp;
                    deref(&stdpointer, &rangeExp);
                    needkw(&lex, closepa);
                    while (castvalue(select))
                        select = select->left;
                    if (lvalue(select) && select->type != en_l_ref && !isstructured(selectTP))
                        select = select->left;
                    tpref->size = getSize(bt_pointer);
                    tpref->type = bt_rref;
                    tpref->btp = selectTP;
                    tpref->rootType = tpref;
                    st = stmtNode(lex, forstmt, st_expr);
                    st->select = exprNode(en_assign, rangeExp, select);
                    if (!isstructured(selectTP))
                    {
                        // create array references for begin and end
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
                        iend = exprNode(en_add, rangeExp, intNode(en_c_i, selectTP->size));
                    }
                    else
                    {
                        // try to lookup in structure
                        TYPE thisTP;
                        memset(&thisTP, 0, sizeof(thisTP));
                        thisTP.type = bt_pointer;
                        thisTP.size = getSize(bt_pointer);
                        thisTP.btp = rangeSP->tp->btp;
                        thisTP.rootType = &thisTP;
                        sbegin = search("begin", basetype(selectTP)->syms);
                        send = search("end", basetype(selectTP)->syms);
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
                            beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, sbegin, &fcb, nullptr, false, false, true, 0);
                            memset(&fce, 0, sizeof(fce));
                            fce.thistp = &thisTP;
                            fce.thisptr = rangeExp;
                            fce.ascall = true;
                            ctp = rangeSP->tp;
                            endFunc = GetOverloadedFunction(&ctp, &fce.fcall, send, &fce, nullptr, false, false, true, 0);
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
                                        INITIALIZER* dest;
                                        EXPRESSION* exp;
                                        fcb.returnEXP = anonymousVar(sc_auto, iteratorType);
                                        fcb.returnSP = fcb.returnEXP->v.sp;
                                        exp = fcb.returnEXP;
                                        dest = nullptr;
                                        callDestructor(fcb.returnSP, nullptr, &exp, nullptr, true, false, false);
                                        initInsert(&dest, iteratorType, exp, 0, true);
                                        fcb.returnSP->dest = dest;

                                        fce.returnEXP = anonymousVar(sc_auto, iteratorType);
                                        fce.returnSP = fcb.returnEXP->v.sp;
                                        exp = fce.returnEXP;
                                        dest = nullptr;
                                        callDestructor(fce.returnSP, nullptr, &exp, nullptr, true, false, false);
                                        initInsert(&dest, iteratorType, exp, 0, true);
                                        fce.returnSP->dest = dest;
                                    }
                                    fc = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                    *fc = fcb;
                                    fc->sp = beginFunc;
                                    fc->functp = beginFunc->tp;
                                    fc->ascall = true;
                                    ibegin = exprNode(en_func, nullptr, nullptr);
                                    ibegin->v.func = fc;
                                    fc = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                    *fc = fce;
                                    fc->sp = endFunc;
                                    fc->functp = endFunc->tp;
                                    fc->ascall = true;
                                    iend = exprNode(en_func, nullptr, nullptr);
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
                                    sbegin = namespacesearch("begin", standard->nameSpaceValues, false, false);
                                    send = namespacesearch("end", standard->nameSpaceValues, false, false);
                                }
                            }
                            if (!sbegin || !send)
                            {
                                if (rangeSP->tp->btp->sp->parentNameSpace)
                                {
                                    sbegin = namespacesearch("begin", rangeSP->tp->btp->sp->parentNameSpace->nameSpaceValues, false,
                                                             false);
                                    send = namespacesearch("end", rangeSP->tp->btp->sp->parentNameSpace->nameSpaceValues, false,
                                                           false);
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
                                fcb.arguments = &args;
                                fcb.ascall = true;
                                ctp = rangeSP->tp;
                                beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, sbegin, &fcb, nullptr, false, false, true, 0);
                                memset(&fce, 0, sizeof(fce));
                                fce.arguments = &args;
                                fce.ascall = true;
                                ctp = rangeSP->tp;
                                endFunc = GetOverloadedFunction(&ctp, &fce.fcall, send, &fce, nullptr, false, false, true, 0);
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
                                            INITIALIZER* dest;
                                            EXPRESSION* exp;
                                            fcb.returnEXP = anonymousVar(sc_auto, iteratorType);
                                            fcb.returnSP = fcb.returnEXP->v.sp;
                                            exp = fcb.returnEXP;
                                            dest = nullptr;
                                            callDestructor(fcb.returnSP, nullptr, &exp, nullptr, true, false, false);
                                            initInsert(&dest, iteratorType, exp, 0, true);
                                            fcb.returnSP->dest = dest;

                                            fce.returnEXP = anonymousVar(sc_auto, iteratorType);
                                            fce.returnSP = fcb.returnEXP->v.sp;
                                            exp = fce.returnEXP;
                                            dest = nullptr;
                                            callDestructor(fce.returnSP, nullptr, &exp, nullptr, true, false, false);
                                            initInsert(&dest, iteratorType, exp, 0, true);
                                            fce.returnSP->dest = dest;
                                        }
                                        fc = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                        *fc = fcb;
                                        fc->sp = beginFunc;
                                        fc->functp = beginFunc->tp;
                                        fc->ascall = true;
                                        fc->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
                                        *fc->arguments = *fcb.arguments;
                                        if (isstructured(it2) && isstructured(((SYMBOL*)(it2->syms->table[0]->p))->tp))
                                        {
                                            EXPRESSION* consexp =
                                                anonymousVar(sc_auto, basetype(rangeSP->tp)->btp);  // sc_parameter to push it...
                                            SYMBOL* esp = consexp->v.sp;
                                            FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                            TYPE* ctype = basetype(rangeSP->tp)->btp;
                                            esp->stackblock = true;
                                            funcparams->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
                                            *funcparams->arguments = *fc->arguments;
                                            callConstructor(&ctype, &consexp, funcparams, false, 0, true, false, true, false, false,
                                                            false);
                                            fc->arguments->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->tp = (TYPE*)Alloc(sizeof(TYPE));
                                            fc->arguments->tp->type = bt_lref;
                                            fc->arguments->tp->size = getSize(bt_pointer);
                                            fc->arguments->tp->btp = fcb.arguments->tp;
                                            fc->arguments->tp->rootType = fc->arguments->tp;
                                        }
                                        ibegin = exprNode(en_func, nullptr, nullptr);
                                        ibegin->v.func = fc;
                                        fc = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                        *fc = fce;
                                        fc->sp = endFunc;
                                        fc->functp = endFunc->tp;
                                        fc->ascall = true;
                                        fc->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
                                        *fc->arguments = *fce.arguments;
                                        if (isstructured(it2) && isstructured(((SYMBOL*)(it2->syms->table[0]->p))->tp))
                                        {
                                            EXPRESSION* consexp =
                                                anonymousVar(sc_auto, basetype(rangeSP->tp)->btp);  // sc_parameter to push it...
                                            SYMBOL* esp = consexp->v.sp;
                                            FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                            TYPE* ctype = basetype(rangeSP->tp)->btp;
                                            esp->stackblock = true;
                                            funcparams->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
                                            *funcparams->arguments = *fc->arguments;
                                            callConstructor(&ctype, &consexp, funcparams, false, 0, true, false, true, false, false,
                                                            false);
                                            fc->arguments->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->tp = (TYPE*)Alloc(sizeof(TYPE));
                                            fc->arguments->tp->type = bt_lref;
                                            fc->arguments->tp->size = getSize(bt_pointer);
                                            fc->arguments->tp->btp = fce.arguments->tp;
                                            fc->arguments->tp->rootType = fc->arguments->tp;
                                        }
                                        iend = exprNode(en_func, nullptr, nullptr);
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
                        if (isstructured(selectTP) && isstructured(iteratorType) && ibegin->type == en_func &&
                            iend->type == en_func)
                        {
                            eBegin = ibegin->v.func->returnEXP;
                            eEnd = iend->v.func->returnEXP;
                            st = stmtNode(lex, forstmt, st_expr);
                            st->select = ibegin;
                            st = stmtNode(lex, forstmt, st_expr);
                            st->select = iend;
                        }
                        else
                        {
                            SYMBOL* sBegin;
                            SYMBOL* sEnd;
                            eBegin = anonymousVar(sc_auto, iteratorType);
                            eEnd = anonymousVar(sc_auto, iteratorType);
                            sBegin = eBegin->v.sp;
                            sEnd = eEnd->v.sp;
                            deref(&stdpointer, &eBegin);
                            deref(&stdpointer, &eEnd);
                            st = stmtNode(lex, forstmt, st_expr);
                            st->select = exprNode(en_assign, eBegin, ibegin);
                            st = stmtNode(lex, forstmt, st_expr);
                            st->select = exprNode(en_assign, eEnd, iend);
                        }
                        if (isref(iteratorType))
                            iteratorType = iteratorType->btp;
                        if (!isstructured(selectTP) || !isstructured(iteratorType))
                        {
                            compare = exprNode(en_eq, eBegin, eEnd);
                        }
                        else
                        {
                            TYPE* eqType = iteratorType;
                            compare = eBegin;
                            if (!insertOperatorFunc(ovcl_unary_prefix, eq, funcsp, &eqType, &compare, iteratorType, eEnd, nullptr,
                                                    0))
                            {
                                error(ERR_MISSING_OPERATOR_EQ_FORRANGE_ITERATOR);
                            }
                        }

                        st = stmtNode(lex, forstmt, st_select);
                        st->label = forstmt->breaklabel;
                        st->altlabel = testlabel;
                        st->select = compare;

                        st = stmtNode(lex, forstmt, st_label);
                        st->label = loopLabel;

                        AllocateLocalContext(parent, funcsp, codeLabel++);

                        // initialize var here
                        st = stmtNode(lex, forstmt, st_expr);
                        if (!isstructured(selectTP))
                        {
                            DeduceAuto(&declSP->tp, selectTP);
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
                                FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                INITLIST* args = (INITLIST*)Alloc(sizeof(INITLIST));
                                funcparams->arguments = args;
                                args->tp = declSP->tp;
                                args->exp = eBegin;
                                callConstructor(&ctype, &decl, funcparams, false, 0, true, false, true, false, false, false);
                                st->select = decl;
                                declDest = declExp;
                                callDestructor(declSP, nullptr, &declDest, nullptr, true, false, false);
                            }
                            else if (isarray(selectTP))
                            {
                                EXPRESSION* decl = declExp;
                                deref(declSP->tp, &decl);
                                st->select = eBegin;
                                if (!isref(declSP->tp))
                                    deref(basetype(selectTP)->btp, &st->select);
                                st->select = exprNode(en_assign, decl, st->select);
                            }
                        }
                        else
                        {
                            TYPE* starType = iteratorType;
                            st->select = eBegin;
                            if (ispointer(iteratorType))
                            {
                                DeduceAuto(&declSP->tp, basetype(iteratorType)->btp);
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
                                    st->select = exprNode(en_assign, decl, st->select);
                                }
                                else
                                {
                                    EXPRESSION* decl = declExp;
                                    TYPE* ctype = declSP->tp;
                                    FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                    INITLIST* args = (INITLIST*)Alloc(sizeof(INITLIST));
                                    funcparams->arguments = args;
                                    args->tp = declSP->tp;
                                    args->exp = eBegin;
                                    callConstructor(&ctype, &decl, funcparams, false, 0, true, false, true, false, false, false);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(declSP, nullptr, &declDest, nullptr, true, false, false);
                                }
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, star, funcsp, &starType, &st->select, nullptr, nullptr,
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
                                DeduceAuto(&declSP->tp, starType);
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
                                    st->select = exprNode(en_assign, decl, st->select);
                                }
                                else
                                {
                                    EXPRESSION* decl = declExp;
                                    TYPE* ctype = declSP->tp;
                                    FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                                    INITLIST* args = (INITLIST*)Alloc(sizeof(INITLIST));
                                    funcparams->arguments = args;
                                    args->tp = declSP->tp;
                                    args->exp = st->select;
                                    callConstructor(&ctype, &decl, funcparams, false, 0, true, false, true, false, false, false);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(declSP, nullptr, &declDest, nullptr, true, false, false);
                                }
                            }
                        }
                        do
                        {
                            lastLabelStmt = forstmt->tail;
                            lex = statement(lex, funcsp, forstmt, true);
                        } while (lex && forstmt->tail != lastLabelStmt && forstmt->tail->purelabel);
                        FreeLocalContext(forstmt, funcsp, codeLabel++);
                        if (declDest)
                        {
                            st = stmtNode(lex, forstmt, st_expr);
                            st->select = declDest;
                        }
                        st = stmtNode(lex, forstmt, st_label);
                        st->label = forstmt->continuelabel;
                        st = stmtNode(lex, forstmt, st_expr);

                        // do ++ here
                        if (!isstructured(selectTP))
                        {
                            if (isarray(selectTP))
                                st->select =
                                    exprNode(en_assign, eBegin,
                                             exprNode(en_add, eBegin, intNode(en_c_i, basetype(basetype(selectTP)->btp)->size)));
                        }
                        else
                        {
                            TYPE* ppType = iteratorType;
                            ppType = basetype(ppType);
                            st->select = eBegin;
                            if (ispointer(iteratorType))
                            {
                                st->select = exprNode(
                                    en_assign, eBegin,
                                    exprNode(en_add, eBegin, intNode(en_c_i, basetype(basetype(iteratorType)->btp)->size)));
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, autoinc, funcsp, &ppType, &st->select, nullptr, nullptr,
                                                         nullptr, 0))
                            {
                                error(ERR_MISSING_OPERATOR_PLUSPLUS_FORRANGE_ITERATOR);
                            }
                            else
                            {
                                if (isstructured(ppType))
                                {
                                    st->select->v.func->returnEXP = anonymousVar(sc_auto, ppType);
                                    st->select->v.func->returnSP = st->select->v.func->returnEXP->v.sp;
                                    declDest = st->select->v.func->returnEXP;
                                    callDestructor(st->select->v.func->returnSP, nullptr, &declDest, nullptr, true, false, false);
                                    st = stmtNode(lex, forstmt, st_expr);
                                    st->select = declDest;
                                }
                            }
                        }

                        if (forline)
                        {
                            if (forstmt->head)
                                forstmt->tail = forstmt->tail->next = forline;
                            else
                                forstmt->head = forstmt->tail = forline;
                            while (forstmt->tail->next)
                                forstmt->tail->next = forstmt->tail;
                        }
                        st = stmtNode(lex, forstmt, st_label);
                        st->label = testlabel;

                        st = stmtNode(lex, forstmt, st_notselect);
                        st->label = loopLabel;
                        st->select = compare;

                        if (!forstmt->hasbreak && (!st->select || isselectfalse(st->select)))
                            parent->needlabel = true;
                        st = stmtNode(lex, forstmt, st_label);
                        st->label = forstmt->breaklabel;
                        parent->hassemi = forstmt->hassemi;
                        parent->nosemi = forstmt->nosemi;
                    }
                    else
                    {
                        error(ERR_MISSING_FORRANGE_BEGIN_END);
                    }
                }
                while (addedBlock--)
                    FreeLocalContext(forstmt, funcsp, codeLabel++);
                AddBlock(lex, parent, forstmt);
                return lex;
            }
            else
            {
                if (declaration && cparams.prm_cplusplus)
                {
                    SYMLIST* hr = localNameSpace->valueData->syms->table[0];
                    while (hr && hr->p->anonymous)
                        hr = hr->next;
                    if (!hr)
                    {
                        error(ERR_FOR_DECLARATOR_MUST_INITIALIZE);
                    }
                    else
                    {
                        SYMBOL* declSP = hr->p;
                        if (!declSP->init)
                        {
                            if (isstructured(declSP->tp) && !basetype(declSP->tp)->sp->trivialCons)
                            {
                                lex = initialize(lex, funcsp, declSP, sc_auto, false, 0);
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
        if (!needkw(&lex, semicolon))
        {
            error(ERR_FOR_NEEDS_SEMI);
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
        else
        {
            if (!MATCHKW(lex, semicolon))
            {
                TYPE* tp = nullptr;
                lex = optimized_expression(lex, funcsp, nullptr, &tp, &select, true);
                if (!tp)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
            }
            if (!needkw(&lex, semicolon))
            {
                error(ERR_FOR_NEEDS_SEMI);
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
            else
            {
                if (!MATCHKW(lex, closepa))
                {
                    TYPE* tp = nullptr;
                    lex = expression_comma(lex, funcsp, nullptr, &tp, &before, nullptr, 0);
                    if (!tp)
                    {
                        error(ERR_EXPRESSION_SYNTAX);
                    }
                    else
                    {
                        optimize_for_constants(&before);
                    }
                }
                if (!MATCHKW(lex, closepa))
                {
                    error(ERR_FOR_NEEDS_CLOSEPA);
                    errskim(&lex, skim_closepa);
                    skip(&lex, closepa);
                }
                else
                {
                    lex = getsym();
                    if (init)
                    {
                        st = stmtNode(lex, forstmt, st_expr);
                        st->select = init;
                    }
                    if (cparams.prm_debug || cparams.prm_optimize_for_size || (chosenAssembler->arch->denyopts & DO_NOENTRYIF))
                    {
                        st = stmtNode(lex, forstmt, st_goto);
                        st->label = testlabel;
                    }
                    else
                    {
                        st = stmtNode(lex, forstmt, st_notselect);
                        st->label = forstmt->breaklabel;
                        st->altlabel = testlabel;
                        st->select = select;
                    }
                    st = stmtNode(lex, forstmt, st_label);
                    st->label = loopLabel;
                    if (cparams.prm_cplusplus || cparams.prm_c99)
                    {
                        addedBlock++;
                        AllocateLocalContext(parent, funcsp, codeLabel++);
                    }
                    do
                    {
                        lastLabelStmt = forstmt->tail;
                        lex = statement(lex, funcsp, forstmt, true);
                    } while (lex && forstmt->tail != lastLabelStmt && forstmt->tail->purelabel);
                    if (cparams.prm_cplusplus || cparams.prm_c99)
                    {
                        addedBlock--;
                        FreeLocalContext(forstmt, funcsp, codeLabel++);
                    }
                    if (before)
                        assignmentUsages(before, false);
                    st = stmtNode(lex, forstmt, st_label);
                    st->label = forstmt->continuelabel;
                    st = stmtNode(lex, forstmt, st_expr);
                    st->select = before;
                    if (forline)
                    {
                        if (forstmt->head)
                            forstmt->tail = forstmt->tail->next = forline;
                        else
                            forstmt->head = forstmt->tail = forline;
                        while (forstmt->tail->next)
                            forstmt->tail = forstmt->tail->next;
                    }
                    st = stmtNode(lex, forstmt, st_label);
                    st->label = testlabel;
                    if (select)
                    {
                        st = stmtNode(lex, forstmt, st_select);
                        st->label = loopLabel;
                        st->select = select;
                    }
                    else
                    {
                        st = stmtNode(lex, forstmt, st_goto);
                        st->label = loopLabel;
                    }
                    if (!forstmt->hasbreak && (!st->select || isselectfalse(st->select)))
                        parent->needlabel = true;
                    st = stmtNode(lex, forstmt, st_label);
                    st->label = forstmt->breaklabel;
                    parent->hassemi = forstmt->hassemi;
                    parent->nosemi = forstmt->nosemi;
                }
            }
        }
    }
    else
    {
        error(ERR_FOR_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    }
    while (addedBlock--)
        FreeLocalContext(forstmt, funcsp, codeLabel++);
    AddBlock(lex, parent, forstmt);
    return lex;
}
static LEXEME* statement_if(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    STATEMENT *st, *st1, *st2, *lastLabelStmt;
    EXPRESSION* select = nullptr;
    int addedBlock = 0;
    bool needlabelif;
    bool needlabelelse = false;
    int ifbranch = codeLabel++;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        lex = selection_expression(lex, parent, &select, funcsp, kw_if, nullptr);
        if (MATCHKW(lex, closepa))
        {
            bool optimized = false;
            STATEMENT* sti;
            currentLineData(parent, lex, 0);
            lex = getsym();
            st = stmtNode(lex, parent, st_notselect);
            st->label = ifbranch;
            st->select = select;
            sti = st;
            parent->needlabel = false;
            if (cparams.prm_cplusplus || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            do
            {
                lastLabelStmt = parent->tail;
                lex = statement(lex, funcsp, parent, true);
            } while (lex && parent->tail != lastLabelStmt && parent->tail->purelabel);
            needlabelif = parent->needlabel;
            if (MATCHKW(lex, kw_else))
            {
                int elsebr = codeLabel++;
                if (cparams.prm_cplusplus || cparams.prm_c99)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                    AllocateLocalContext(parent, funcsp, codeLabel++);
                }
                st = stmtNode(lex, parent, st_goto);
                st->label = elsebr;
                if (cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size)
                {
                    st2 = sti;
                    st1 = sti->next;
                    while (st1 && (st1)->type == st_line)
                    {
                        st2 = st1;
                        st1 = st1->next;
                    }
                    if (st1 && st1->type == st_goto)
                    {
                        optimized = true;
                        st2->next = nullptr;
                        parent->tail = st2;
                        sti->type = st_select;
                        sti->label = st1->label;
                    }
                }
                st = stmtNode(lex, parent, st_label);
                st->label = ifbranch;
                if (!parent->nosemi && !parent->hassemi)
                    errorint(ERR_NEEDY, ';');
                if (parent->nosemi && parent->hassemi)
                    error(ERR_MISPLACED_ELSE);
                currentLineData(parent, lex, 0);
                lex = getsym();
                parent->needlabel = false;
                do
                {
                    lastLabelStmt = parent->tail;
                    lex = statement(lex, funcsp, parent, true);
                } while (lex && parent->tail != lastLabelStmt && parent->tail->purelabel);
                if ((cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size) && !optimized)
                {
                    st1 = st->next;
                    st2 = st;
                    while (st1 && (st1)->type == st_line)
                    {
                        st2 = st1;
                        st1 = st1->next;
                    }
                    if (st1 && st1->type == st_goto)
                    {
                        //						optimized = true;
                        st2->next = nullptr;
                        parent->tail = st2;
                        sti->label = st1->label;
                    }
                }
                needlabelelse = parent->needlabel;
                if (cparams.prm_cplusplus || cparams.prm_c99)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                    addedBlock--;
                }
                st = stmtNode(lex, parent, st_label);
                st->label = elsebr;
            }
            else
            {
                if (cparams.prm_optimize_for_speed || cparams.prm_optimize_for_size)
                {
                    st2 = sti;
                    st1 = sti->next;
                    while (st1 && (st1)->type == st_line)
                    {
                        st2 = st1;
                        st1 = st1->next;
                    }
                    if (st1 && st1->type == st_goto)
                    {
                        //						optimized = true;
                        st2->next = nullptr;
                        parent->tail = st2;
                        sti->type = st_select;
                        sti->label = st1->label;
                    }
                }
                if (cparams.prm_cplusplus || cparams.prm_c99)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                    addedBlock--;
                }
                st = stmtNode(lex, parent, st_label);
                st->label = ifbranch;
            }
            if (parent->hassemi)
                parent->nosemi = true;
            parent->needlabel = (needlabelif && needlabelelse);
        }
        else
        {
            error(ERR_IF_NEEDS_CLOSEPA);
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
    }
    else
    {
        error(ERR_IF_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    }
    while (addedBlock--)
        FreeLocalContext(parent, funcsp, codeLabel++);
    return lex;
}
static LEXEME* statement_goto(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    (void)funcsp;
    (void)parent;
    lex = getsym();
    currentLineData(parent, lex, 0);
    if (ISID(lex))
    {
        SYMBOL* spx = search(lex->value.s.a, labelSyms);
        BLOCKDATA* block = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
        STATEMENT* st = stmtNode(lex, block, st_goto);
        st->explicitGoto = true;
        block->next = parent;
        block->type = begin;
        block->table = localNameSpace->valueData->syms;
        if (!spx)
        {
            spx = makeID(sc_ulabel, nullptr, nullptr, litlate(lex->value.s.a));
            spx->declfile = spx->origdeclfile = lex->file;
            spx->declline = spx->origdeclline = lex->line;
            spx->realdeclline = lex->realline;
            spx->declfilenum = lex->filenum;
            SetLinkerNames(spx, lk_none);
            spx->offset = codeLabel++;
            spx->gotoTable = st;
            insert(spx, labelSyms);
        }
        else
        {
            thunkGotoDestructors(&st->destexp, block, spx->gotoTable->parent);
            thunkCatchCleanup(st, funcsp, block, spx->gotoTable->parent);
        }
        st->label = spx->offset;
        lex = getsym();
        parent->needlabel = true;
        AddBlock(lex, parent, block);
    }
    else
    {
        error(ERR_GOTO_NEEDS_LABEL);
        errskim(&lex, skim_semi);
        skip(&lex, semicolon);
    }
    if (funcsp->constexpression)
        error(ERR_CONSTEXPR_FUNC_NO_GOTO);
    return lex;
}
static LEXEME* statement_label(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    SYMBOL* spx = search(lex->value.s.a, labelSyms);
    STATEMENT* st;
    (void)funcsp;
    st = stmtNode(lex, parent, st_label);
    if (spx)
    {
        if (spx->storage_class == sc_ulabel)
        {
            spx->storage_class = sc_label;
            // may come here from assembly language...
            if (spx->gotoTable)
            {
                thunkGotoDestructors(&spx->gotoTable->destexp, spx->gotoTable->parent, parent);
                thunkCatchCleanup(spx->gotoTable, funcsp, spx->gotoTable->parent, parent);
            }
        }
        else
        {
            errorsym(ERR_DUPLICATE_LABEL, spx);
        }
    }
    else
    {
        spx = makeID(sc_label, nullptr, nullptr, litlate(lex->value.s.a));
        SetLinkerNames(spx, lk_none);
        spx->offset = codeLabel++;
        spx->gotoTable = st;
        insert(spx, labelSyms);
    }
    st->label = spx->offset;
    st->purelabel = true;
    getsym();       /* colon */
    lex = getsym(); /* next sym */
    parent->needlabel = false;
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
                    if (exp->left->type != en_auto || exp->left->v.sp->storage_class != sc_parameter)
                        exp = exp->left;
                    if (exp->type == en_l_ref)
                        if (exp->left->type != en_auto || exp->left->v.sp->storage_class != sc_parameter)
                            exp = exp->left;
                }
                else
                {
                    exp = exp->left;
                }
            }
            else
            {
                if (exp->left->type != en_auto || exp->left->v.sp->storage_class != sc_parameter)
                    exp = exp->left;
            }
        }
        else if (basetype(tp)->btp->type == bt_aggregate)
        {
            if (!isfunction(basetype(boundTP)->btp))
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
        else if (isfunction(basetype(tp)->btp))
        {
            if (!isfunction(basetype(boundTP)->btp))
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
        else if (exp->type == en_auto)
        {
            if (exp->v.sp->storage_class == sc_auto)
            {
                error(ERR_REF_RETURN_LOCAL);
            }
            else if (exp->v.sp->storage_class == sc_parameter)
            {
                exp = exp2;
            }
        }
        else
        {
            if (referenceTypeError(tp, exp2) != exp2->type && (!isstructured(basetype(tp)->btp) || exp2->type != en_lvalue))
                errortype(ERR_REF_INIT_TYPE_CANNOT_BE_BOUND, tp, boundTP);
        }
    }
    else
    {
        if (exp->type == en_cond)
        {
            exp->right->left = ConvertReturnToRef(exp->right->left, tp, boundTP);
            exp->right->right = ConvertReturnToRef(exp->right->right, tp, boundTP);
        }
        else if (!isstructured(basetype(tp)->btp) && !isref(boundTP))
        {
            error(ERR_LVALUE);
        }
        else if (exp->type == en_auto && exp->v.sp->storage_class == sc_auto)
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
        case en_auto:
        case en_pc:
        case en_global:
        case en_tempref:
        case en_threadlocal:
            return node;
        case en_add:
        {
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
            if (tp1->type != tp2->type)
                err = true;
            tp1 = tp1->btp;
            tp2 = tp2->btp;
        }
        if (tp1 || tp2 || err)
        {
            errorsym(ERR_RETURN_TYPE_MISMATCH_FOR_AUTO_FUNCTION, funcsp);
        }
    }
}
static int aa;
static LEXEME* statement_return(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    STATEMENT* st;
    TYPE* tp = nullptr;
    EXPRESSION* returnexp = nullptr;
    TYPE* returntype = nullptr;
    EXPRESSION* destexp = nullptr;

    if (funcsp->attribs.inheritable.linkage3 == lk_noreturn)
        error(ERR_NORETURN);
    funcsp->retcount++;

    lex = getsym();
    if (MATCHKW(lex, semicolon))
    {
        if (!isvoid(basetype(funcsp->tp)->btp))
        {
            if (!basetype(funcsp->tp)->sp->isConstructor && !basetype(funcsp->tp)->sp->isDestructor)
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
            LEXEME* current = lex;
            lex = expression(lex, funcsp, nullptr, &tp1, &exp1, _F_SIZEOF);
            lex = prevsym(current);
            while (tp1->type == bt_typedef)
                tp1 = tp1->btp;
            DeduceAuto(&basetype(funcsp->tp)->btp, tp1);
            tp = basetype(funcsp->tp)->btp;
            UpdateRootTypes(funcsp->tp);
            SetLinkerNames(funcsp, funcsp->attribs.inheritable.linkage);
            matchReturnTypes = true;
        }
        if (isstructured(tp) || basetype(tp)->type == bt_memberptr)
        {
            EXPRESSION* en = anonymousVar(sc_parameter, &stdpointer);
            SYMBOL* sp = en->v.sp;
            bool maybeConversion = true;
            sp->allocate = false;  // static var
            sp->offset = chosenAssembler->arch->retblocksize;
            sp->structuredReturn = true;
            sp->name = "__retblock";
            if ((funcsp->attribs.inheritable.linkage == lk_pascal) && basetype(funcsp->tp)->syms->table[0] &&
                ((SYMBOL*)basetype(funcsp->tp)->syms->table[0])->tp->type != bt_void)
                sp->offset = funcsp->paramsize;
            deref(&stdpointer, &en);
            if (cparams.prm_cplusplus && isstructured(tp))
            {
                bool implicit = false;
                if (basetype(tp)->sp->templateLevel && basetype(tp)->sp->templateParams && !basetype(tp)->sp->instantiated)
                {
                    SYMBOL* sym = basetype(tp)->sp;
                    if (!allTemplateArgsSpecified(sym, sym->templateParams))
                        sym = GetClassTemplate(sym, sym->templateParams->next, false);
                    if (sym && allTemplateArgsSpecified(sym, sym->templateParams))
                        tp = TemplateClassInstantiate(sym, sym->templateParams, false, sc_global)->tp;
                }
                if (MATCHKW(lex, begin))
                {
                    INITIALIZER *init = nullptr, *dest = nullptr;
                    SYMBOL* sym = nullptr;
                    sym = anonymousVar(sc_localstatic, tp)->v.sp;
                    lex = initType(lex, funcsp, 0, sc_auto, &init, &dest, tp, sym, false, 0);
                    returnexp = convertInitToExpression(tp, nullptr, nullptr, funcsp, init, en, false);
                    returntype = tp;
                    if (sym)
                        sym->dest = dest;
                }
                else
                {
                    bool oldrref, oldlref;
                    FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
                    TYPE* ctype = tp;
                    // shortcut for conversion from single expression
                    EXPRESSION* exp1 = nullptr;
                    TYPE* tp1 = nullptr;
                    lex = expression_no_comma(lex, funcsp, nullptr, &tp1, &exp1, nullptr, 0);
                    MatchReturnTypes(funcsp, tp, tp1);
                    if (tp1 && isstructured(tp1))
                    {
                        if (sameTemplate(tp, tp1))
                            basetype(funcsp->tp)->btp = tp1;
                        ctype = tp1;
                        if (basetype(tp1)->sp->templateLevel && basetype(tp1)->sp->templateParams &&
                            !basetype(tp1)->sp->instantiated && !templateNestingCount)
                        {
                            SYMBOL* sym = basetype(tp1)->sp;
                            if (!allTemplateArgsSpecified(sym, sym->templateParams))
                                sym = GetClassTemplate(sym, sym->templateParams->next, false);
                            if (sym)
                                ctype = tp1 = TemplateClassInstantiate(sym, sym->templateParams, false, sc_global)->tp;
                        }
                        optimize_for_constants(&exp1);
                    }
                    /*
                    exp2 = exp1;
                    if (exp2->type == en_thisref)
                        exp2 = exp2->left;
                    if (exp2->type == en_func)
                        if (exp2->v.func->sp->isConstructor)
                            exp3 = baseNode(exp2->v.func->thisptr);
                        else if (exp2->v.func->returnEXP)
                            exp3 = baseNode(exp2->v.func->returnEXP);
                    if (exp2->type == en_func && exp3 && exp3->type == en_auto && exp3->v.sp->anonymous &&
                        (exp2->v.func->sp->isConstructor && comparetypes(basetype(exp2->v.func->thistp)->btp, tp1, true) ||
                         !exp2->v.func->sp->isConstructor && exp2->v.func->returnSP && comparetypes(exp2->v.func->returnSP->tp, tp1,
                    true)))
                    {
                        // either a constructor for the return type or function returning the return type
                        if (exp2->v.func->sp->isConstructor)
                        {
                            exp2->v.func->thisptr = en;
                        }
                        else
                        {
                            exp2->v.func->returnEXP = en;
                        }
                        returntype = tp;
                        returnexp = exp1;
                    }
                    else
                    */
                    {
                        bool nonconst = funcsp->nonConstVariableUsed;
                        funcparams->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
                        funcparams->arguments->tp = tp1;
                        funcparams->arguments->exp = exp1;
                        oldrref = basetype(tp1)->rref;
                        oldlref = basetype(tp1)->lref;
                        basetype(tp1)->rref = exp1->type == en_auto && exp1->v.sp->storage_class != sc_parameter;
                        EXPRESSION* exptemp = exp1;
                        if (exptemp->type == en_thisref)
                            exptemp = exptemp->left;
                        if (exptemp->type == en_func && isfunction(exptemp->v.func->sp->tp) &&
                            basetype(basetype(exptemp->v.func->sp->tp)->btp)->type != bt_lref)
                            basetype(tp1)->rref = true;
                        basetype(tp1)->lref = !basetype(tp1)->rref;
                        maybeConversion = false;
                        returntype = tp;
                        implicit = true;
                        callConstructor(&ctype, &en, funcparams, false, nullptr, true, maybeConversion, false, false, false, false);
                        funcsp->nonConstVariableUsed = nonconst;
                        returnexp = en;
                        basetype(tp1)->rref = oldrref;
                        basetype(tp1)->lref = oldlref;
                        if (funcparams->sp && matchesCopy(funcparams->sp, true))
                        {
                            switch (exp1->type)
                            {
                                case en_global:
                                case en_auto:
                                case en_threadlocal:
                                    exp1->v.sp->dest = nullptr;
                                    break;
                                default:
                                    break;
                            }
                        }
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
                    ((architecture != ARCHITECTURE_MSIL) || !isstructured(tp) || !isconstzero(&stdint, returnexp)))
                {
                    errortype(ERR_CANNOT_CONVERT_TYPE, tp1, tp);
                }
                else
                {
                    if (returnexp->type == en_func && !returnexp->v.func->ascall)
                    {
                        if (returnexp->v.func->sp->storage_class == sc_overloads)
                        {
                            SYMBOL* funcsp;
                            if (returnexp->v.func->sp->parentClass && !returnexp->v.func->asaddress)
                                error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                            funcsp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &returnexp, 0);
                            if (funcsp && basetype(tp)->type == bt_memberptr)
                            {
                                int lbl = dumpMemberPtr(funcsp, tp, true);
                                returnexp = intNode(en_labcon, lbl);
                            }
                        }
                        else
                        {
                            returnexp = intNode(en_labcon, dumpMemberPtr(returnexp->v.func->sp, tp, true));
                        }
                    }
                    else if (returnexp->type == en_pc || returnexp->type == en_memberptr)
                    {
                        returnexp = intNode(en_labcon, dumpMemberPtr(returnexp->v.sp, tp, true));
                    }
                    if ((architecture != ARCHITECTURE_MSIL) || funcsp->attribs.inheritable.linkage2 == lk_unmanaged ||
                        msilManaged(funcsp))
                    {
                        returnexp = exprNode(en_blockassign, en, returnexp);
                        returnexp->size = basetype(tp)->size;
                        returnexp->altdata = (void*)(basetype(tp));
                    }
                    returntype = tp;
                }
            }
        }
        else
        {
            TYPE* tp1 = nullptr;
            bool needend = false;
            if (MATCHKW(lex, begin))
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
            if (basetype(tp)->type == bt___string)
            {
                if (returnexp->type == en_labcon)
                    returnexp->type = en_c_string;
                else if (basetype(tp1)->type != bt___string)
                    returnexp = exprNode(en_x_string, returnexp, nullptr);
                tp1 = &std__string;
            }
            else if (!comparetypes(tp1, tp, true) && ismsil(tp1))
            {
                errortype(ERR_CANNOT_CONVERT_TYPE, tp1, tp);
            }
            if (needend)
            {
                if (!needkw(&lex, end))
                {
                    errskim(&lex, skim_end);
                    skip(&lex, end);
                }
            }
            if (basetype(tp)->type == bt___object)
                if (basetype(tp1)->type != bt___object && !isstructured(tp1) && (!isarray(tp1) || !basetype(tp1)->msil))
                    returnexp = exprNode(en_x_object, returnexp, nullptr);
            if (isstructured(tp1) && isarithmetic(tp))
            {
                if (cparams.prm_cplusplus)
                {
                    castToArithmetic(false, &tp1, &returnexp, (enum e_kw) - 1, tp, true);
                }
                else
                {
                    errortype(ERR_CANNOT_CAST_TYPE, tp1, tp);
                }
            }
            if (tp->type == bt_auto)
                returntype = tp = tp1;
            else
            {
                returntype = tp;
            }
            if (returnexp->type == en_func)
            {
                if (returnexp->v.func->sp->storage_class == sc_overloads)
                {
                    EXPRESSION* exp1 = returnexp;
                    if (returnexp->v.func->sp->parentClass && !returnexp->v.func->asaddress)
                        error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                    returnexp->v.func->sp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &exp1, 0);
                    returnexp->v.func->fcall = varNode(en_pc, returnexp->v.func->sp);
                }
            }
            if (cparams.prm_cplusplus && isstructured(returntype))
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
            if (basetype(basetype(tp)->btp)->type != bt_memberptr)
                returnexp = ConvertReturnToRef(returnexp, basetype(funcsp->tp)->btp, returntype);
        }
        else if (returnexp && returnexp->type == en_auto && returnexp->v.sp->storage_class == sc_auto)
        {
            if (!isstructured(basetype(funcsp->tp)->btp) && basetype(basetype(funcsp->tp)->btp)->type != bt_memberptr)
                if (basetype(basetype(funcsp->tp)->btp)->type != bt___object &&
                    (!isarray(basetype(funcsp->tp)->btp) || !basetype(funcsp->tp)->btp->msil) &&
                    basetype(basetype(funcsp->tp)->btp)->type != bt_templateselector)
                    error(ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE);
        }
        if (!returnexp)
            returnexp = intNode(en_c_i, 0);  // errors
    }
    currentLineData(parent, lex, 0);
    thunkRetDestructors(&destexp, nullptr, localNameSpace->valueData->syms);
    st = stmtNode(lex, parent, st_return);
    st->select = returnexp;
    st->destexp = destexp;
    thunkCatchCleanup(st, funcsp, parent, nullptr);  // to top level
    if (returnexp && returntype && !isautotype(returntype) && !matchReturnTypes)
    {
        if (!tp)  // some error...
            tp = &stdint;
        if (tp->type == bt_void)
        {
            if (!cparams.prm_cplusplus || returntype->type != bt_void)
                error(ERR_CANNOT_RETURN_VOID_VALUE);
        }
        else if (returntype && returntype->type == bt_void)
            error(ERR_RETURN_NO_VALUE);
        else
        {
            if (cparams.prm_cplusplus && (funcsp->isConstructor || funcsp->isDestructor))
            {
                error(ERR_CONSTRUCTOR_HAS_RETURN);
            }
            else if (isstructured(returntype) || isstructured(tp))
            {
                if (!comparetypes(returntype, tp, false) && !sameTemplate(returntype, tp))
                    error(ERR_RETMISMATCH);
            }
            else if (basetype(returntype)->type == bt_memberptr || basetype(tp)->type == bt_memberptr)
            {
                if (isconstzero(tp, st->select))
                {
                    int lbl = dumpMemberPtr(nullptr, returntype, true);
                    st->select = intNode(en_labcon, lbl);
                }
                else
                {
                    if (st->select->type == en_memberptr)
                    {
                        int lbl = dumpMemberPtr(st->select->v.sp, returntype, true);
                        st->select = intNode(en_labcon, lbl);
                    }
                    if (!comparetypes(returntype, tp, true))
                        error(ERR_RETMISMATCH);
                }
            }
            else
            {
                if (!isref(basetype(funcsp->tp)->btp) &&
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
                        else if (returnexp->type != en_func || returnexp->v.func->fcall->type != en_l_p)
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
                                if (cparams.prm_cplusplus && !isvoidptr(returntype) && returnexp->type != en_nullptr &&
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
    parent->needlabel = true;
    return lex;
}
static LEXEME* statement_switch(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    BLOCKDATA* switchstmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
    STATEMENT* st;
    EXPRESSION* select = nullptr;
    int addedBlock = 0;
    funcsp->noinline = true;
    switchstmt->breaklabel = codeLabel++;
    switchstmt->next = parent;
    switchstmt->defaultlabel = -1; /* no default */
    switchstmt->type = kw_switch;
    switchstmt->table = localNameSpace->valueData->syms;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        lex = selection_expression(lex, switchstmt, &select, funcsp, kw_switch, nullptr);
        if (MATCHKW(lex, closepa))
        {
            currentLineData(switchstmt, lex, 0);
            lex = getsym();
            st = stmtNode(lex, switchstmt, st_switch);
            st->select = select;
            st->breaklabel = switchstmt->breaklabel;
            lex = statement(lex, funcsp, switchstmt, true);
            st->cases = switchstmt->cases;
            st->label = switchstmt->defaultlabel;
            if (st->label != -1 && switchstmt->needlabel && !switchstmt->hasbreak)
                parent->needlabel = true;
            /* force a default if there was none */
            if (st->label == -1)
            {
                st->label = codeLabel;
                st = stmtNode(lex, switchstmt, st_label);
                st->label = codeLabel++;
            }
            st = stmtNode(lex, switchstmt, st_label);
            st->label = switchstmt->breaklabel;
            if (!switchstmt->nosemi && !switchstmt->hassemi)
                errorint(ERR_NEEDY, ';');
            parent->nosemi = true;
        }
        else
        {
            error(ERR_SWITCH_NEEDS_CLOSEPA);
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
    }
    else
    {
        error(ERR_SWITCH_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    }
    while (addedBlock--)
        FreeLocalContext(switchstmt, funcsp, codeLabel++);
    AddBlock(lex, parent, switchstmt);
    return lex;
}
static LEXEME* statement_while(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    BLOCKDATA* whilestmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
    STATEMENT *st, *lastLabelStmt;
    STATEMENT* whileline;
    EXPRESSION* select = nullptr;
    int addedBlock = 0;
    int loopLabel = codeLabel++;
    whilestmt->breaklabel = codeLabel++;
    whilestmt->continuelabel = codeLabel++;
    whilestmt->next = parent;
    whilestmt->type = kw_while;
    whilestmt->table = localNameSpace->valueData->syms;
    whileline = currentLineData(nullptr, lex, 0);
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        lex = selection_expression(lex, whilestmt, &select, funcsp, kw_while, nullptr);
        if (!MATCHKW(lex, closepa))
        {
            error(ERR_WHILE_NEEDS_CLOSEPA);

            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
        else
        {
            lex = getsym();
            if (cparams.prm_debug || cparams.prm_optimize_for_size || (chosenAssembler->arch->denyopts & DO_NOENTRYIF))
            {
                st = stmtNode(lex, whilestmt, st_goto);
                st->label = whilestmt->continuelabel;
            }
            else
            {
                st = stmtNode(lex, whilestmt, st_notselect);
                st->label = whilestmt->breaklabel;
                st->altlabel = whilestmt->continuelabel;
                st->select = select;
            }

            st = stmtNode(lex, whilestmt, st_label);
            st->label = loopLabel;
            if (cparams.prm_cplusplus || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            do
            {
                lastLabelStmt = whilestmt->tail;
                lex = statement(lex, funcsp, whilestmt, true);
            } while (lex && whilestmt->tail != lastLabelStmt && whilestmt->tail->purelabel);
            if (cparams.prm_cplusplus || cparams.prm_c99)
            {
                addedBlock--;
                FreeLocalContext(whilestmt, funcsp, codeLabel++);
            }
            st = stmtNode(lex, whilestmt, st_label);
            st->label = whilestmt->continuelabel;
            if (whileline)
            {
                if (whilestmt->head)
                    whilestmt->tail = whilestmt->tail->next = whileline;
                else
                    whilestmt->head = whilestmt->tail = whileline;
                while (whilestmt->tail->next)
                    whilestmt->tail = whilestmt->tail->next;
            }
            st = stmtNode(lex, whilestmt, st_select);
            st->label = loopLabel;
            st->select = select;
            if (!whilestmt->hasbreak && isselecttrue(st->select))
                parent->needlabel = true;
            st = stmtNode(lex, whilestmt, st_label);
            st->label = whilestmt->breaklabel;
            parent->hassemi = whilestmt->hassemi;
            parent->nosemi = whilestmt->nosemi;
        }
    }
    else
    {
        error(ERR_WHILE_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    }
    while (addedBlock--)
        FreeLocalContext(whilestmt, funcsp, codeLabel++);
    AddBlock(lex, parent, whilestmt);
    return lex;
}
static bool checkNoEffect(EXPRESSION* exp)
{
    if (exp->noexprerr)
        return false;
    switch (exp->type)
    {
        case en_func:
        case en_assign:
        case en_autoinc:
        case en_autodec:
        case en_blockassign:
        case en_blockclear:
        case en_stmt:
        case en_atomic:
        case en_voidnz:
        case en_void:
        case en__initblk:
        case en__cpblk:
            return false;
            break;
        case en_not_lvalue:
        case en_lvalue:
        case en_thisref:
        case en_literalclass:
        case en_funcret:
            return checkNoEffect(exp->left);
        case en_cond:
            return checkNoEffect(exp->right->left) & checkNoEffect(exp->right->right);
        default:
            return true;
    }
}
static LEXEME* statement_expr(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    STATEMENT* st;
    EXPRESSION* select = nullptr;
    TYPE* tp = nullptr;
    LINEDATA *oldLineHead = linesHead, *oldLineTail = linesTail;
    linesHead = linesTail = nullptr;
    (void)parent;

    lex = optimized_expression(lex, funcsp, nullptr, &tp, &select, true);
    linesHead = oldLineHead;
    linesTail = oldLineTail;
    currentLineData(parent, lex, 0);
    st = stmtNode(lex, parent, st_expr);
    st->select = select;
    if (!tp)
    {
        lex = getsym();
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (tp->type != bt_void && tp->type != bt_any)
    {
        if (checkNoEffect(st->select))
            error(ERR_EXPRESSION_HAS_NO_EFFECT);
        if (cparams.prm_cplusplus && isstructured(tp) && select->type == en_func)
        {
            SYMBOL* sym = select->v.func->returnSP;
            if (sym && sym->allocate)
            {
                INITIALIZER* init = nullptr;
                EXPRESSION* exp = select->v.func->returnEXP;
                callDestructor(basetype(tp)->sp, nullptr, &exp, nullptr, true, false, false);
                initInsert(&init, sym->tp, exp, 0, false);
                sym->dest = init;
            }
        }
    }
    else
    {
        if (select->type == en_func && select->v.func->sp && select->v.func->sp->attribs.inheritable.linkage3 == lk_noreturn)
        {
            parent->needlabel = true;
        }
    }
    return lex;
}
static LEXEME* asm_declare(LEXEME* lex)
{
    enum e_kw kw = lex->kw->key;
    do
    {
        lex = getsym();
        if (lex)
        {
            if (ISID(lex))
            {
                SYMBOL* sym = search(lex->value.s.a, globalNameSpace->valueData->syms);
                if (!sym)
                {
                    sym = makeID(sc_label, nullptr, nullptr, litlate(lex->value.s.a));
                }
                switch (kw)
                {
                    case kw_public:
                        if (sym->storage_class != sc_global)
                            InsertGlobal(sym);
                        sym->storage_class = sc_global;
                        break;
                    case kw_extern:
                        if (sym->storage_class != sc_external)
                            InsertExtern(sym);
                        sym->storage_class = sc_external;
                        break;
                    case kw_const:
                        sym->storage_class = sc_constant;
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
    } while (lex && MATCHKW(lex, comma));
    return lex;
}
LEXEME* statement_catch(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent, int label, int startlab, int endlab)
{
    bool last = false;
    if (!MATCHKW(lex, kw_catch))
    {
        error(ERR_EXPECTED_CATCH_CLAUSE);
    }
    while (MATCHKW(lex, kw_catch))
    {
        if (last)
            error(ERR_NO_MORE_CATCH_CLAUSES_ALLOWED);
        lex = getsym();
        if (needkw(&lex, openpa))
        {
            STATEMENT* st;
            TYPE* tp = nullptr;
            BLOCKDATA* catchstmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
            ParseAttributeSpecifiers(&lex, funcsp, true);
            catchstmt->breaklabel = label;
            catchstmt->next = parent;
            catchstmt->defaultlabel = -1; /* no default */
            catchstmt->type = kw_catch;
            catchstmt->table = localNameSpace->valueData->syms;
            AllocateLocalContext(catchstmt, funcsp, codeLabel++);
            if (MATCHKW(lex, ellipse))
            {
                last = true;
                lex = getsym();
            }
            else
            {
                lex = declare(lex, funcsp, &tp, sc_catchvar, lk_none, catchstmt, false, true, false, ac_public);
            }
            if (needkw(&lex, closepa))
            {
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
            }
            FreeLocalContext(catchstmt, funcsp, codeLabel++);
            st = stmtNode(lex, parent, st_catch);
            st->label = startlab;
            st->endlabel = endlab;
            st->altlabel = codeLabel++;
            st->breaklabel = catchstmt->breaklabel;
            st->blockTail = catchstmt->blockTail;
            st->lower = catchstmt->head;
            st->tp = tp;
        }
        else
        {
            errskim(&lex, skim_end);
        }
    }
    return lex;
}
LEXEME* statement_try(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    STATEMENT* st;
    BLOCKDATA* trystmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
    hasXCInfo = true;
    trystmt->breaklabel = codeLabel++;
    trystmt->next = parent;
    trystmt->defaultlabel = -1; /* no default */
    trystmt->type = kw_try;
    trystmt->table = localNameSpace->valueData->syms;
    funcsp->anyTry = true;
    lex = getsym();
    if (!MATCHKW(lex, begin))
    {
        error(ERR_EXPECTED_TRY_BLOCK);
    }
    else
    {
        AllocateLocalContext(trystmt, funcsp, codeLabel++);
        tryLevel++;
        lex = compound(lex, funcsp, trystmt, false);
        tryLevel--;
        FreeLocalContext(trystmt, funcsp, codeLabel++);
        parent->needlabel = trystmt->needlabel;
        st = stmtNode(lex, parent, st_try);
        st->label = codeLabel++;
        st->endlabel = codeLabel++;
        st->breaklabel = trystmt->breaklabel;
        st->blockTail = trystmt->blockTail;
        st->lower = trystmt->head;
        parent->nosemi = true;
        if (parent->next)
            parent->next->nosemi = true;
        lex = statement_catch(lex, funcsp, parent, st->breaklabel, st->label, st->endlabel);
    }

    return lex;
}
bool hasInlineAsm()
{
    return architecture == ARCHITECTURE_X86;
}
LEXEME* statement_asm(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent)
{
    (void)funcsp;
    (void)parent;
    functionHasAssembly = true;
    if (hasInlineAsm())
    {
        parent->hassemi = false;
        lex = getsym();
        if (MATCHKW(lex, begin))
        {
            lex = getsym();
            while (lex && !MATCHKW(lex, end))
            {
                currentLineData(parent, lex, 0);
                lex = inlineAsm(lex, parent);
                if (KW(lex) == semicolon)
                {
                    skip(&lex, semicolon);
                }
            }
            needkw(&lex, end);
            parent->nosemi = true;
            return lex;
        }
        else
        {
            currentLineData(parent, lex, 0);
            while (cparams.prm_assemble && lex && MATCHKW(lex, semicolon))
                lex = SkipToNextLine();
            if (lex)
            {
                if (cparams.prm_assemble && (MATCHKW(lex, kw_public) || MATCHKW(lex, kw_extern) || MATCHKW(lex, kw_const)))
                {
                    lex = asm_declare(lex);
                }
                else
                {
                    lex = inlineAsm(lex, parent);
                }
                if (MATCHKW(lex, semicolon))
                {
                    if (cparams.prm_assemble)
                        lex = SkipToNextLine();
                    else
                        skip(&lex, semicolon);
                }
            }
            parent->hassemi = true;
        }
    }
    else
    {
        /* if we get here the backend doesn't have an assembler, for now we
         * are just going to make an error and scan past tokens
         */
        lex = getsym();
        errorstr(ERR_ASM, "Assembly language not supported by this compiler");
        if (MATCHKW(lex, begin))
        {
            while (lex && !MATCHKW(lex, end))
            {
                currentLineData(parent, lex, 0);
                lex = getsym();
            }
            needkw(&lex, end);
            parent->nosemi = true;
        }
        else
        {
            /* problematic, ASM keyword without a block->  Skip to end of line... */
            currentLineData(parent, lex, 0);
            parent->hassemi = true;
            SkipToEol();
            lex = getsym();
        }
    }
    return lex;
}
static void reverseAssign(STATEMENT* current, EXPRESSION** exp)
{
    if (current)
    {
        reverseAssign(current->next, exp);
        if (current->type != st_line)
        {
            if (*exp)
                *exp = exprNode(en_void, current->select, *exp);
            else
                *exp = current->select;
        }
    }
}
static LEXEME* autodeclare(LEXEME* lex, SYMBOL* funcsp, TYPE** tp, EXPRESSION** exp, BLOCKDATA* parent, int asExpression)
{
    BLOCKDATA block;
    (void)parent;
    declareAndInitialize = false;
    memset(&block, 0, sizeof(block));
    lex = declare(lex, funcsp, tp, sc_auto, lk_none, &block, false, asExpression, false, ac_public);

    // move any auto assignments
    reverseAssign(block.head, exp);

    // now move variable declarations
    while (block.head)
    {
        if (block.head->type == st_varstart)
        {
            STATEMENT* s = stmtNode(lex, parent, st_varstart);
            s->select = block.head->select;
        }
        block.head = block.head->next;
    }
    if (!*exp)
    {
        *exp = intNode(en_c_i, 0);
        errorint(ERR_NEEDY, '=');
    }

    return lex;
}
bool resolveToDeclaration(LEXEME* lex)
{
    LEXEME* placeholder = lex;
    if (ISKW(lex))
        switch (KW(lex))
        {
            case kw_struct:
            case kw_union:
            case kw_class:
            case kw_decltype:
                return true;
            default:
                break;
        }
    lex = getsym();
    if (MATCHKW(lex, begin))
    {
        prevsym(placeholder);
        return false;
    }
    while (MATCHKW(lex, classsel))
    {
        lex = getsym();
        lex = getsym();
    }
    if (MATCHKW(lex, lt))
    {
        int level = 1;
        lex = getsym();
        while (level && lex != nullptr && !MATCHKW(lex, semicolon))
        {
            if (MATCHKW(lex, lt))
            {
                level++;
            }
            else if (MATCHKW(lex, gt))
            {
                level--;
            }
            else if (MATCHKW(lex, rightshift))
            {
                level--;
                lex = getGTSym(lex);
                continue;
            }
            lex = getsym();
        }
    }
    if (MATCHKW(lex, openpa))
    {
        int level = 1;
        lex = getsym();
        while (level && lex != nullptr && !MATCHKW(lex, semicolon))
        {
            if (MATCHKW(lex, openpa))
            {
                level++;
            }
            else if (MATCHKW(lex, closepa))
            {
                level--;
            }
            lex = getsym();
        }
        if (MATCHKW(lex, assign) || MATCHKW(lex, openpa) || MATCHKW(lex, openbr))
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
LEXEME* statement(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent, bool viacontrol)
{
    LEXEME* start = lex;
    ParseAttributeSpecifiers(&lex, funcsp, true);
    if (ISID(lex))
    {
        lex = getsym();
        if (MATCHKW(lex, colon))
        {
            lex = backupsym();
            lex = statement_label(lex, funcsp, parent);
            parent->needlabel = false;
            parent->nosemi = true;
            return lex;
        }
        else
        {
            lex = backupsym();
        }
    }
    if (parent->needlabel && !MATCHKW(lex, kw_case) && !MATCHKW(lex, kw_default) && !MATCHKW(lex, begin))
        error(ERR_UNREACHABLE_CODE);

    if (!MATCHKW(lex, begin))
    {
        if (MATCHKW(lex, kw_throw))
            parent->needlabel = true;
        else
            parent->needlabel = false;
    }
    parent->nosemi = false;
    switch (KW(lex))
    {
        case kw_try:
            lex = statement_try(lex, funcsp, parent);
            break;
        case kw_catch:
            error(ERR_CATCH_WITHOUT_TRY);
            lex = statement_catch(lex, funcsp, parent, 1, 1, 1);
            break;
        case begin:
            lex = compound(lex, funcsp, parent, false);
            parent->nosemi = true;
            if (parent->next)
                parent->next->nosemi = true;
            break;
        case end:
            /* don't know how it could get here :) */
            //			error(ERR_UNEXPECTED_END_OF_BLOCKDATA);
            //			lex = getsym();
            parent->hassemi = true;
            return lex;
            break;
        case kw_do:
            lex = statement_do(lex, funcsp, parent);
            break;
        case kw_while:
            lex = statement_while(lex, funcsp, parent);
            return lex;
        case kw_for:
            lex = statement_for(lex, funcsp, parent);
            return lex;
        case kw_switch:
            lex = statement_switch(lex, funcsp, parent);
            break;
        case kw_if:
            lex = statement_if(lex, funcsp, parent);
            break;
        case kw_else:
            error(ERR_MISPLACED_ELSE);
            skip(&lex, kw_else);
            parent->nosemi = true;
            break;
        case kw_case:
            while (KW(lex) == kw_case)
            {
                if (cparams.prm_cplusplus)
                    HandleEndOfCase(parent);
                lex = statement_case(lex, funcsp, parent);
                if (cparams.prm_cplusplus)
                    HandleStartOfCase(parent);
            }
            lex = statement(lex, funcsp, parent, false);
            parent->nosemi = true;
            return lex;
            break;
        case kw_default:
            if (cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            lex = statement_default(lex, funcsp, parent);
            if (cparams.prm_cplusplus)
                HandleStartOfCase(parent);
            lex = statement(lex, funcsp, parent, true);
            parent->nosemi = true;
            return lex;
            break;
        case kw_continue:
            lex = statement_continue(lex, funcsp, parent);
            if (cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case kw_break:
            lex = statement_break(lex, funcsp, parent);
            if (cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case kw_goto:
            lex = statement_goto(lex, funcsp, parent);
            if (cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case kw_return:
            lex = statement_return(lex, funcsp, parent);
            if (cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            break;
        case semicolon:
            break;
        case kw_asm:
            lex = statement_asm(lex, funcsp, parent);
            return lex;
        case kw___try:
            lex = statement_SEH(lex, funcsp, parent);
            break;
        case kw___catch:
        case kw___finally:
        case kw___fault:
            error(ERR_SEH_HANDLER_WITHOUT_TRY);
            lex = statement_SEH(lex, funcsp, parent);
            break;
        default:
            if (((startOfType(lex, false) &&
                  ((!cparams.prm_cplusplus && ((architecture != ARCHITECTURE_MSIL) || cparams.msilAllowExtensions)) ||
                   resolveToDeclaration(lex)))) ||
                MATCHKW(lex, kw_namespace) || MATCHKW(lex, kw_using) || MATCHKW(lex, kw_decltype) || MATCHKW(lex, kw_static_assert))
            {
                if (!cparams.prm_c99 && !cparams.prm_cplusplus)
                {
                    error(ERR_NO_DECLARATION_HERE);
                }
                if (viacontrol)
                {
                    AllocateLocalContext(parent, funcsp, codeLabel++);
                }
                while (((startOfType(lex, false) &&
                         ((!cparams.prm_cplusplus && (architecture != ARCHITECTURE_MSIL)) || resolveToDeclaration(lex)))) ||
                       MATCHKW(lex, kw_namespace) || MATCHKW(lex, kw_using) || MATCHKW(lex, kw_decltype) ||
                       MATCHKW(lex, kw_static_assert))
                {
                    STATEMENT* current = parent->tail;
                    declareAndInitialize = false;
                    if (start)
                    {
                        lex = prevsym(start);
                        start = nullptr;
                    }
                    lex = declare(lex, funcsp, nullptr, sc_auto, lk_none, parent, false, false, false, ac_public);
                    markInitializers(current);
                    if (MATCHKW(lex, semicolon))
                    {
                        parent->hassemi = true;
                        skip(&lex, semicolon);
                    }
                    else
                        break;
                }
                if (viacontrol)
                {
                    FreeLocalContext(parent, funcsp, codeLabel++);
                }
                return lex;
            }
            else
            {
                isCallExit = false;
                lex = statement_expr(lex, funcsp, parent);
                parent->needlabel = isCallExit;
            }
    }
    if (MATCHKW(lex, semicolon))
    {
        parent->hassemi = true;
        skip(&lex, semicolon);
    }
    else
        parent->hassemi = false;
    return lex;
}
static bool thunkmainret(SYMBOL* funcsp, BLOCKDATA* parent, bool always)
{
    if (always || (!strcmp(funcsp->name, "main") && !funcsp->parentClass && !funcsp->parentNameSpace))
    {
        STATEMENT* s = stmtNode(nullptr, parent, st_return);
        s->select = intNode(en_c_i, 0);
        return true;
    }
    return false;
}
static void thunkThisReturns(STATEMENT* st, EXPRESSION* thisptr)
{
    while (st)
    {
        if (st->lower)
            thunkThisReturns(st->lower, thisptr);
        if (st->type == st_return)
            st->select = thisptr;
        st = st->next;
    }
}
static void insertXCInfo(SYMBOL* funcsp)
{
    char name[2048];
    SYMBOL* sym;
    makeXCTab(funcsp);
    my_sprintf(name, "@$xc%s", funcsp->decoratedName);
    sym = makeID(sc_global, &stdpointer, nullptr, litlate(name));
    sym->attribs.inheritable.linkage = lk_virtual;
    sym->decoratedName = sym->errname = sym->name;
    sym->allocate = true;
    sym->attribs.inheritable.used = sym->assigned = true;
    funcsp->xc->xcInitLab = codeLabel++;
    funcsp->xc->xcDestLab = codeLabel++;
    funcsp->xc->xclab = sym;

    sym = namespacesearch("_InitializeException", globalNameSpace, false, false);
    if (sym)
    {
        FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
        INITLIST* arg1 = (INITLIST*)Alloc(sizeof(INITLIST));
        INITLIST* arg2 = (INITLIST*)Alloc(sizeof(INITLIST));
        EXPRESSION* exp;
        sym = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
        funcparams->functp = sym->tp;
        funcparams->sp = sym;
        funcparams->fcall = varNode(en_pc, sym);
        funcparams->ascall = true;
        funcparams->arguments = arg1;
        arg1->next = arg2;
        arg1->tp = &stdpointer;

        arg1->exp = varNode(en_auto, funcsp->xc->xctab);
        arg2->tp = &stdpointer;
        arg2->exp = varNode(en_global, funcsp->xc->xclab);
        exp = exprNode(en_func, 0, 0);
        exp->v.func = funcparams;
        funcsp->xc->xcInitializeFunc = exp;
        sym = namespacesearch("_RundownException", globalNameSpace, false, false);
        if (sym)
        {
            sym = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
            funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
            funcparams->functp = sym->tp;
            funcparams->sp = sym;
            funcparams->fcall = varNode(en_pc, sym);
            funcparams->ascall = true;
            exp = exprNode(en_func, 0, 0);
            exp->v.func = funcparams;
            funcsp->xc->xcRundownFunc = exp;
        }
    }
}
LEXEME* compound(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* parent, bool first)
{
    BLOCKDATA* blockstmt = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
    preProcessor->MarkStdPragma();
    STATEMENT* st;
    EXPRESSION* thisptr = nullptr;
    browse_blockstart(lex->line);
    blockstmt->next = parent;
    blockstmt->type = begin;
    blockstmt->needlabel = parent->needlabel;
    blockstmt->table = localNameSpace->valueData->syms;
    currentLineData(blockstmt, lex, 0);
    AllocateLocalContext(blockstmt, funcsp, codeLabel++);
    parent->needlabel = false;
    if (first)
    {
        SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
        int n = 1;
        browse_startfunc(funcsp, funcsp->declline);
        while (hr)
        {
            SYMBOL* sp2 = hr->p;
            if (!cparams.prm_cplusplus && sp2->tp->type != bt_ellipse && !isvoid(sp2->tp) && sp2->anonymous)
                errorarg(ERR_PARAMETER_MUST_HAVE_NAME, n, sp2, funcsp);
            sp2->destructed = false;
            insert(sp2, localNameSpace->valueData->syms);
            browse_variable(sp2);
            n++;
            hr = hr->next;
        }
        if (cparams.prm_cplusplus && funcsp->isConstructor && funcsp->parentClass)
        {
            ParseMemberInitializers(funcsp->parentClass, funcsp);
            thisptr = thunkConstructorHead(blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms, true, false);
        }
    }
    lex = getsym(); /* past { */

    st = blockstmt->tail;
    if (!cparams.prm_cplusplus)
    {
        // have to defer so we can get expression like constructor calls
        while (startOfType(lex, false))
        {
            STATEMENT* current = blockstmt->tail;
            declareAndInitialize = false;
            lex = declare(lex, funcsp, nullptr, sc_auto, lk_none, blockstmt, false, false, false, ac_public);
            markInitializers(current);
            if (MATCHKW(lex, semicolon))
            {
                lex = getsym();
            }
            else
            {
                error(ERR_DECLARE_SYNTAX);
            }
        }
    }
    if (parent->type == kw_switch)
    {
        if (st != blockstmt->tail)
            /* kinda naive... */
            error(ERR_INITIALIZATION_MAY_BE_BYPASSED);
    }
    currentLineData(blockstmt, lex, -1);
    blockstmt->nosemi = true; /* in case it is an empty body */
    while (lex && !MATCHKW(lex, end))
    {
        if (!blockstmt->hassemi && !blockstmt->nosemi)
            errorint(ERR_NEEDY, ';');
        if (MATCHKW(lex, semicolon))
        {
            lex = getsym();  // helps in error processing to not do default statement processing here...
        }
        else
        {
            lex = statement(lex, funcsp, blockstmt, false);
        }
    }
    if (first)
    {
        browse_endfunc(funcsp, funcsp->endLine = lex ? lex->line : endline);
    }
    if (!lex)
    {
        needkw(&lex, end);
        return lex;
    }
    browse_blockend(endline = lex->line);
    currentLineData(blockstmt, lex, -!first);
    if (parent->type == begin || parent->type == kw_switch || parent->type == kw_try || parent->type == kw_catch ||
        parent->type == kw_do)
        parent->needlabel = blockstmt->needlabel;
    if (!blockstmt->hassemi && !blockstmt->nosemi)
    {
        errorint(ERR_NEEDY, ';');
    }
    st = blockstmt->head;
    if (st)
    {
        bool hasvla = false;
        while (st)
        {
            hasvla |= st->hasvla;
            st = st->next;
        }
        if (hasvla)
        {
            if (first)
            {
                funcsp->allocaUsed = true;
            }
        }
    }
    if (first && cparams.prm_cplusplus)
    {
        if (functionCanThrow)
        {
            if (funcsp->xcMode == xc_none || funcsp->xcMode == xc_dynamic)
            {
                hasXCInfo = true;
            }
        }
        else if (funcsp->xcMode == xc_dynamic && funcsp->xc && funcsp->xc->xcDynamic)
            hasXCInfo = true;
        if (hasXCInfo && cparams.prm_xcept)
        {
            if (funcsp->anyTry)
	        funcsp->noinline = true;
            insertXCInfo(funcsp);
        }
        if (!strcmp(funcsp->name, overloadNameTab[CI_DESTRUCTOR]))
            thunkDestructorTail(blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms);
    }
    if (cparams.prm_cplusplus)
        HandleEndOfSwitchBlock(blockstmt);
    FreeLocalContext(blockstmt, funcsp, codeLabel++);
    if (first && !blockstmt->needlabel && !isvoid(basetype(funcsp->tp)->btp) && basetype(funcsp->tp)->btp->type != bt_auto &&
        !funcsp->isConstructor)
    {
        if (funcsp->attribs.inheritable.linkage3 == lk_noreturn)
            error(ERR_NORETURN);
        else if (cparams.prm_c99 || cparams.prm_cplusplus)
        {
            if (!thunkmainret(funcsp, blockstmt, false))
            {
                if (isref(basetype(funcsp->tp)->btp))
                    error(ERR_FUNCTION_RETURNING_REF_SHOULD_RETURN_VALUE);
                else
                {
                    error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
                    if (chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                        thunkmainret(funcsp, blockstmt, true);
                }
            }
        }
        else
        {
            error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
            if (chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                thunkmainret(funcsp, blockstmt, true);
        }
    }
    needkw(&lex, end);
    if (first && cparams.prm_cplusplus)
    {
        if (funcsp->hasTry)
        {
            STATEMENT* last = stmtNode(nullptr, blockstmt, st_return);
            lex = statement_catch(lex, funcsp, blockstmt, retlab, startlab, 0);
            if (last->next)
            {
                int label;
                last = last->next;
                label = last->altlabel;
                do
                {
                    last->endlabel = label;
                    last = last->next;
                } while (last);
            }
            hasXCInfo = true;
            funcsp->anyTry = true;
        }
    }
    if (parent->type == kw_catch)
    {
        SYMBOL* sym = namespacesearch("_CatchCleanup", globalNameSpace, false, false);
        if (sym)
        {
            FUNCTIONCALL* funcparams = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
            STATEMENT* st = stmtNode(lex, blockstmt, st_expr);
            INITLIST* arg1 = (INITLIST*)Alloc(sizeof(INITLIST));  // exception table
            makeXCTab(funcsp);
            sym = (SYMBOL*)basetype(sym->tp)->syms->table[0]->p;
            funcparams->ascall = true;
            funcparams->sp = sym;
            funcparams->functp = sym->tp;
            funcparams->fcall = varNode(en_pc, sym);
            funcparams->arguments = arg1;
            arg1->exp = varNode(en_auto, funcsp->xc->xctab);
            arg1->tp = &stdpointer;
            st->select = exprNode(en_func, nullptr, nullptr);
            st->select->v.func = funcparams;
        }
    }
    if (first)
    {
        if (thisptr)
        {
            stmtNode(nullptr, blockstmt, st_return);
            thunkThisReturns(blockstmt->head, thisptr);
        }
    }
    AddBlock(lex, parent, blockstmt);
    preProcessor->ReleaseStdPragma();
    return lex;
}
void assignParam(SYMBOL* funcsp, int* base, SYMBOL* param)
{
    TYPE* tp = basetype(param->tp);
    param->parent = funcsp;
    if (tp->type == bt_void)
        return;
    if (isstructured(tp) && !basetype(tp)->sp->pureDest)
        hasXCInfo = true;
    if (chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
    {
        // calculate index for CIL
        param->offset = (*base)++;
    }
    else if (!ispointer(tp) && tp->size <= chosenAssembler->arch->parmwidth)
    {
        param->offset = *base + funcvaluesize(tp->size);
        *base += chosenAssembler->arch->parmwidth;
    }
    else
    {
        param->offset = *base;
        if (tp->type == bt_pointer)
        {
            if (!tp->vla)
                *base += getSize(bt_pointer);
            else
            {
                *base += tp->size;
            }
        }
        else
            *base += tp->size;
        if (*base % chosenAssembler->arch->parmwidth)
            *base += chosenAssembler->arch->parmwidth - *base % chosenAssembler->arch->parmwidth;
    }
}
static void assignCParams(LEXEME* lex, SYMBOL* funcsp, int* base, SYMLIST* params, TYPE* rv, BLOCKDATA* block)
{
    (void)rv;
    while (params)
    {
        STATEMENT* s = stmtNode(lex, block, st_varstart);
        s->select = varNode(en_auto, (SYMBOL*)params->p);
        assignParam(funcsp, base, (SYMBOL*)params->p);
        params = params->next;
    }
}
static void assignPascalParams(LEXEME* lex, SYMBOL* funcsp, int* base, SYMLIST* params, TYPE* rv, BLOCKDATA* block)
{
    if (params)
    {
        STATEMENT* s;
        if (params->next)
            assignPascalParams(lex, funcsp, base, params->next, rv, block);
        assignParam(funcsp, base, (SYMBOL*)params->p);
        s = stmtNode(lex, block, st_varstart);
        s->select = varNode(en_auto, (SYMBOL*)params->p);
    }
}
static void assignParameterSizes(LEXEME* lex, SYMBOL* funcsp, BLOCKDATA* block)
{
    SYMLIST* params = basetype(funcsp->tp)->syms->table[0];
    int base;
    if (chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
        base = 0;
    else
        base = chosenAssembler->arch->retblocksize;
    if (funcsp->attribs.inheritable.linkage == lk_pascal)
    {
        assignPascalParams(lex, funcsp, &base, params, basetype(funcsp->tp)->btp, block);
    }
    else
    {
        if (isstructured(basetype(funcsp->tp)->btp) || basetype(basetype(funcsp->tp)->btp)->type == bt_memberptr)
        {
            // handle structured return values
            if (chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
            {
                if (funcsp->attribs.inheritable.linkage2 == lk_unmanaged || (architecture != ARCHITECTURE_MSIL) ||
                    msilManaged(funcsp))
                    base++;
            }
            else
            {
                base += getSize(bt_pointer);
                if (base % chosenAssembler->arch->parmwidth)
                    base += chosenAssembler->arch->parmwidth - base % chosenAssembler->arch->parmwidth;
            }
        }
        if (ismember(funcsp))
        {
            // handle 'this' pointer
            assignParam(funcsp, &base, (SYMBOL*)params->p);
            params = params->next;
        }
        assignCParams(lex, funcsp, &base, params, basetype(funcsp->tp)->btp, block);
    }
    funcsp->paramsize = base - chosenAssembler->arch->retblocksize;
}
static void checkUndefinedStructures(SYMBOL* funcsp)
{
    SYMLIST* hr;
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
    hr = basetype(funcsp->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sym = hr->p;
        TYPE* tp = basetype(sym->tp);
        if (isstructured(tp) && !basetype(tp)->sp->tp->syms)
        {
            tp = PerformDeferredInitialization(tp, funcsp);
            if (!basetype(tp)->sp->tp->syms)
            {
                currentErrorLine = 0;
                errorsym(ERR_STRUCT_NOT_DEFINED, basetype(tp)->sp);
            }
        }
        hr = hr->next;
    }
}
static int inlineStatementCount(STATEMENT* block)
{
    int rv = 0;
    while (block != nullptr)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_try:
            case st_catch:
            case st___try:
            case st___catch:
            case st___finally:
            case st___fault:
                rv += 1000;
                break;
            case st_return:
            case st_expr:
            case st_declare:
                rv++;
                break;
            case st_goto:
            case st_label:
                rv++;
                break;
            case st_select:
            case st_notselect:
                rv++;
                break;
            case st_switch:
                rv++;
                rv += inlineStatementCount(block->lower);
                break;
            case st_block:
                rv++;
                rv += inlineStatementCount(block->lower) + inlineStatementCount(block->blockTail);
                break;
            case st_passthrough:
            case st_nop:
                break;
            case st_datapassthrough:
                break;
            case st_line:
            case st_varstart:
            case st_dbgblock:
                break;
            default:
                diag("Invalid block type in inlinestmtCount");
                break;
        }
        block = block->next;
    }
    return rv;
}
static void handleInlines(SYMBOL* funcsp)
{
    /* so it will get instantiated as a virtual function */
    if (!funcsp->isInline)
        return;
    if (cparams.prm_c99)
        funcsp->attribs.inheritable.used = true;
    /* this unqualified the current function if it has structured
     * args or return value, or if it has nested declarations
     */
    {
        if (funcsp->inlineFunc.syms)
        {
            HASHTABLE* ht = funcsp->inlineFunc.syms->next; /* past params */
            if (ht)
                ht = ht->next; /* past first level */
            /* if any vars declared at another level */
            while (ht && ht->next)
            {
                if (ht->table[0])
                {
                    funcsp->noinline = true;
                    break;
                }
                ht = ht->next;
            }
            if (funcsp->inlineFunc.syms->next)
            {
                if (funcsp->inlineFunc.syms->next->table[0])
                    funcsp->noinline = true;
            }
        }
        if (funcsp->attribs.inheritable.linkage == lk_virtual)
        {

            SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
            SYMBOL* head;
            while (hr)
            {
                head = hr->p;
                if (isstructured(head->tp))
                {
                    funcsp->noinline = true;
                    break;
                }
                hr = hr->next;
            }
        }
        if (inlineStatementCount(funcsp->inlineFunc.stmt) > 100)
        {
            funcsp->noinline = true;
        }
    }
}
void parseNoexcept(SYMBOL* funcsp)
{
    if (funcsp->deferredNoexcept)
    {
        STRUCTSYM s;
        if (funcsp->parentClass)
        {
            s.str = funcsp->parentClass;
            addStructureDeclaration(&s);
        }
        LEXEME* lex = SetAlternateLex(funcsp->deferredNoexcept);
        SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
        TYPE* tp = nullptr;
        EXPRESSION* exp = nullptr;
        AllocateLocalContext(nullptr, nullptr, 0);
        while (hr)
        {
            SYMBOL* sp2 = hr->p;
            insert(sp2, localNameSpace->valueData->syms);
            hr = hr->next;
        }
        lex = optimized_expression(lex, funcsp, nullptr, &tp, &exp, false);
        FreeLocalContext(nullptr, nullptr, 0);
        if (!IsConstantExpression(exp, false, false))
        {
            if (!templateNestingCount)
                error(ERR_CONSTANT_VALUE_EXPECTED);
        }
        else
        {
            funcsp->xcMode = exp->v.i ? xc_none : xc_all;
        }
        SetAlternateLex(nullptr);
        if (funcsp->parentClass)
            dropStructureDeclaration();
    }
}
LEXEME* body(LEXEME* lex, SYMBOL* funcsp)
{
    int n1;
    bool oldsetjmp_used = setjmp_used;
    bool oldfunctionHasAssembly = functionHasAssembly;
    bool oldDeclareAndInitialize = declareAndInitialize;
    bool oldHasXCInfo = hasXCInfo;
    bool oldFunctionCanThrow = functionCanThrow;
    HASHTABLE* oldSyms = localNameSpace->valueData->syms;
    HASHTABLE* oldLabelSyms = labelSyms;
    SYMBOL* oldtheCurrentFunc = theCurrentFunc;
    BLOCKDATA* block = (BLOCKDATA*)Alloc(sizeof(BLOCKDATA));
    STATEMENT* startStmt;
    int oldCodeLabel = codeLabel;
    int oldMatchReturnTypes = matchReturnTypes;
    bool oldHasFuncCall = hasFuncCall;
    hasFuncCall = false;
    funcNesting++;
    funcLevel++;
    functionCanThrow = false;
    codeLabel = INT_MIN;
    hasXCInfo = false;
    localNameSpace->valueData->syms = nullptr;
    functionHasAssembly = false;
    setjmp_used = false;
    declareAndInitialize = false;
    block->type = funcsp->hasTry ? kw_try : begin;
    theCurrentFunc = funcsp;

    checkUndefinedStructures(funcsp);
    parseNoexcept(funcsp);
    FlushLineData(funcsp->declfile, funcsp->realdeclline);
    if (!funcsp->linedata)
    {
        startStmt = currentLineData(nullptr, lex, 0);
        if (startStmt)
            funcsp->linedata = startStmt->lineData;
    }
    funcsp->declaring = true;
    labelSyms = CreateHashTable(1);
    assignParameterSizes(lex, funcsp, block);
    funcsp->startLine = lex->line;
    lex = compound(lex, funcsp, block, true);
    checkUnlabeledReferences(block);
    checkGotoPastVLA(block->head, true);
    if (isautotype(basetype(funcsp->tp)->btp) && !templateNestingCount)
        basetype(funcsp->tp)->btp = &stdvoid;  // return value for auto function without return statements
    if (cparams.prm_cplusplus)
    {
        if (!funcsp->constexpression)
            funcsp->nonConstVariableUsed = true;
        else
            funcsp->nonConstVariableUsed |= !MatchesConstFunction(funcsp);

        if (funcsp->xcMode == xc_none && !funcsp->anyTry && !hasFuncCall)
        {
            funcsp->xcMode= xc_unspecified;
            funcsp->xc = nullptr;
            if (funcsp->mainsym)
            {
                funcsp->mainsym->xcMode= xc_unspecified;
                funcsp->mainsym->xc = nullptr;
            }
            hasXCInfo = false;
        }
        funcsp->canThrow = functionCanThrow;
    }
    funcsp->labelCount = codeLabel - INT_MIN;
    n1 = codeLabel;
    if (!funcsp->templateLevel || funcsp->instantiated || funcsp->instantiated2)
    {
        funcsp->inlineFunc.stmt = stmtNode(lex, nullptr, st_block);
        funcsp->inlineFunc.stmt->lower = block->head;
        funcsp->inlineFunc.stmt->blockTail = block->blockTail;
        funcsp->declaring = false;
        if (funcsp->isInline && (functionHasAssembly || funcsp->attribs.inheritable.linkage2 == lk_export))
            funcsp->isInline = funcsp->dumpInlineToFile = funcsp->promotedToInline = false;
        if (!cparams.prm_allowinline)
            funcsp->isInline = funcsp->dumpInlineToFile = funcsp->promotedToInline = false;
        // if it is variadic don't allow it to be inline
        if (funcsp->isInline)
        {
            SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
            if (hr)
            {
                while (hr->next)
                    hr = hr->next;
                if (hr->p->tp->type == bt_ellipse)
                    funcsp->isInline = funcsp->dumpInlineToFile = funcsp->promotedToInline = false;
            }
        }
        if (funcsp->attribs.inheritable.linkage == lk_virtual || funcsp->isInline)
        {
            if (funcsp->isInline)
                funcsp->attribs.inheritable.linkage2 = lk_none;
            InsertInline(funcsp);
            if (!cparams.prm_cplusplus && funcsp->storage_class != sc_static)
                GENREF(funcsp);
        }
#ifndef PARSER_ONLY
        else
        {
            bool isTemplate = false;
            SYMBOL* spt = funcsp;
            while (spt && !isTemplate)
            {
                if (spt->templateLevel)
                    isTemplate = true;
                else
                    spt = spt->parentClass;
            }

            if (!isTemplate)
            {
                if (!TotalErrors())
                {
                    int oldstartlab = startlab;
                    int oldretlab = retlab;
                    startlab = nextLabel++;
                    retlab = nextLabel++;
                    genfunc(funcsp, true);
                    retlab = oldretlab;
                    startlab = oldstartlab;
                }
            }
        }
#endif
    }
#ifndef PARSER_ONLY
    localFree();
#endif
    handleInlines(funcsp);

    hasFuncCall = oldHasFuncCall;
    declareAndInitialize = oldDeclareAndInitialize;
    theCurrentFunc = oldtheCurrentFunc;
    hasXCInfo = oldHasXCInfo;
    setjmp_used = oldsetjmp_used;
    functionHasAssembly = oldfunctionHasAssembly;
    localNameSpace->valueData->syms = oldSyms;
    labelSyms = oldLabelSyms;
    codeLabel = oldCodeLabel;
    functionCanThrow = oldFunctionCanThrow;
    matchReturnTypes = oldMatchReturnTypes;
    funcLevel--;
    funcNesting--;
    return lex;
}
