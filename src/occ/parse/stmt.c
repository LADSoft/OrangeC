/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without  met:
    
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
/* declare in select has multiple vars */
#include "compiler.h"
#include <limits.h>
#include <assert.h>
extern TYPE stdint;
extern ARCH_DEBUG *chosenDebugger;
extern ARCH_ASM *chosenAssembler;
extern TYPE stdint, stdvoid, stdpointer;
extern int stdpragmas;
extern INCLUDES *includes;
extern HASHTABLE *labelSyms;
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern INCLUDES *includes;
extern enum e_kw skim_colon[], skim_end[];
extern enum e_kw skim_closepa[];
extern enum e_kw skim_semi[];
extern SYMBOL *theCurrentFunc;
extern TYPE stdpointer;
extern char *overloadNameTab[];
extern LEXCONTEXT *context;
extern TYPE stdXC;
extern TYPE std__string, stdbool;
extern int currentErrorLine;
extern int total_errors;

extern int templateNestingCount;

int funcNesting;

BOOLEAN hasXCInfo;
int startlab, retlab;
int codeLabel;
int nextLabel=1;
BOOLEAN setjmp_used;
BOOLEAN functionHasAssembly;
BOOLEAN declareAndInitialize;
BOOLEAN functionCanThrow;

LINEDATA *linesHead, *linesTail;

static int matchReturnTypes;
static int endline;

static LEXEME *autodeclare(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, 
                           BLOCKDATA *parent, BOOLEAN asExpression);
static LEXEME *statement(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent, 
                           BOOLEAN viacontrol);
static LEXEME *compound(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent,   
                        BOOLEAN first);

static BLOCKDATA *caseDestructBlock;

void statement_ini(BOOLEAN global)
{
    if (!global)
        nextLabel = 1;
    linesHead = linesTail = NULL;
    functionCanThrow = FALSE;
    funcNesting = 0;
    caseDestructBlock = NULL;
    matchReturnTypes = FALSE;
}
void InsertLineData(int lineno, int fileindex, char *fname, char *line)
{
    LINEDATA *ld ;
    IncGlobalFlag();
    ld = Alloc(sizeof(LINEDATA));
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
void FlushLineData(char *file, int lineno)
{
    while (linesHead)
    {
        if (strcmp(file, linesHead->file) != 0 || linesHead->lineno < lineno)
            linesHead = linesHead->next;
        else
            break;
    }
}
STATEMENT *currentLineData(BLOCKDATA *parent, LEXEME *lex, int offset)
{
    STATEMENT *rv = NULL;
    LINEDATA *ld = linesHead, **p = &ld;
    int lineno;
    char *file;
    if (!lex)
        return NULL;
    lineno = lex->line + offset;
    file = lex->file;
    while (*p && (strcmp((*p)->file, file) != 0 || lineno >= (*p)->lineno))
    {
        p = &(*p)->next;
    }
    linesHead = *p;
    *p = NULL;
    if (ld)
    {
        rv = stmtNode(lex, parent, st_line);
        rv->lineData = ld;
    }
    return rv;
}
STATEMENT *stmtNode(LEXEME *lex, BLOCKDATA *parent, enum e_stmt stype)
{
    STATEMENT *st = Alloc(sizeof(STATEMENT));
    if (!lex)
        lex = context->cur ? context->cur->prev : context->last;
    st->type = stype;
    st->charpos = 0;
    st->line = lex->line;
    st->file = lex->file;
    st->parent = parent;
    if (chosenDebugger && chosenDebugger->blocknum)
        st->blocknum = *chosenDebugger->blocknum + 1;
    if (parent)
    {
        if (parent->head)
            parent->tail = parent->tail->next = st;
        else
            parent->head = parent->tail = st;
    }
    return st;
}
static void AddBlock(LEXEME *lex, BLOCKDATA *parent, BLOCKDATA *newbl)
{
    STATEMENT *st = stmtNode(lex, parent, st_block);
    st->blockTail = newbl->blockTail;
    st->lower = newbl->head;
}
static BOOLEAN isselecttrue(EXPRESSION *exp)
{
    if (isintconst(exp))
        return !!exp->v.i;
    return FALSE;
}
static BOOLEAN isselectfalse(EXPRESSION *exp)
{
    if (isintconst(exp))
        return !exp->v.i;
    return FALSE;
}
static void markInitializers(STATEMENT *prev)
{
    if (prev)
    {
        prev = prev->next;
        while (prev)
        {
            if (prev->type == st_expr)
                prev->hasdeclare = TRUE;
            prev = prev->next;
        }
    }
}
static LEXEME *selection_expression(LEXEME *lex, BLOCKDATA *parent, EXPRESSION **exp, SYMBOL *funcsp, enum e_kw kw, BOOLEAN *declaration)
{
    TYPE *tp = NULL;
    BOOLEAN hasAttributes = ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    (void)parent;
    if (startOfType(lex,FALSE) && (!cparams.prm_cplusplus || resolveToDeclaration(lex)))
    {
        if (declaration)
            *declaration = TRUE;
        if ((cparams.prm_cplusplus && kw != kw_do && kw != kw_else) || (cparams.prm_c99 && (kw == kw_for || kw == kw_rangefor)))
        {
            // empty
        }
        else
        {
            error(ERR_NO_DECLARATION_HERE);
        }
        /* fixme need type */
        lex = autodeclare(lex, funcsp, &tp, exp, parent, kw != kw_for | (kw == kw_rangefor ? _F_NOCHECKAUTO : 0));
        if (tp->type == bt_memberptr)
        {
            *exp = exprNode(en_mp_as_bool, *exp, NULL);
            (*exp)->size = tp->size;
            tp = &stdint;
        }
    }
    else
    {
        if (hasAttributes)
            error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
/*		BOOLEAN openparen = MATCHKW(lex, openpa); */
        if (declaration)
            *declaration = FALSE;
        lex = expression(lex, funcsp, NULL, &tp, exp, kw != kw_for && kw != kw_rangefor? _F_SELECTOR : 0);
        if (tp)
        {
            if (tp->type == bt_memberptr)
            {
                *exp = exprNode(en_mp_as_bool, *exp, NULL);
                (*exp)->size = tp->size;
                tp = &stdint;
            }
            optimize_for_constants(exp);
        }
    }
        
    if (cparams.prm_cplusplus && tp && isstructured(tp) && kw != kw_for && kw != kw_rangefor)
    {
        if (!castToArithmeticInternal(FALSE, &tp, exp, (enum e_kw) - 1, &stdbool, FALSE))
            if (!castToArithmeticInternal(FALSE, &tp, exp, (enum e_kw)-1, &stdint, FALSE))
                if (!castToPointer(&tp, exp, (enum e_kw)-1, &stdpointer))
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
static BLOCKDATA *getCommonParent(BLOCKDATA *src, BLOCKDATA *dest)
{
    BLOCKDATA *top = src;
    while (top)
    {
        BLOCKDATA *test = dest;
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
void makeXCTab(SYMBOL *funcsp)
{
    char name[512];
    SYMBOL *sp; 
    if (!funcsp->xc)
    {
        funcsp->xc = Alloc(sizeof(struct xcept));
    }
    if (!funcsp->xc->xctab)
    {
        sp = makeID(sc_auto, &stdXC, NULL, "$$xctab");
        sp->decoratedName = sp->errname = sp->name;
        sp->allocate = TRUE;
        sp->used = sp->assigned = TRUE;
        insert(sp, localNameSpace->syms);
        funcsp->xc->xctab = sp;
    }
}
static void thunkCatchCleanup(STATEMENT *st, SYMBOL *funcsp, BLOCKDATA *src, BLOCKDATA *dest)
{
    BLOCKDATA *top = dest ? getCommonParent(src, dest) : NULL, *srch = src;
    while (srch != top)
    {
        if (srch->type == kw_catch)
        {
            SYMBOL *sp = namespacesearch("_CatchCleanup", globalNameSpace, FALSE, FALSE);
            if (sp)
            {
                FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                STATEMENT **find = &src->head;
                INITLIST *arg1 = Alloc(sizeof(INITLIST)); // exception table
                makeXCTab(funcsp);
                sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
                funcparams->ascall = TRUE;
                funcparams->sp = sp;
                funcparams->functp = sp->tp;
                funcparams->fcall = varNode(en_pc, sp);
                funcparams->arguments = arg1;
                arg1->exp = varNode(en_auto, funcsp->xc->xctab);
                arg1->tp = &stdpointer;
                while (*find && *find != st)
                    find = &(*find)->next;
                if (*find == st)
                {
                    *find = Alloc(sizeof(STATEMENT));
                    **find = *st;
                    (*find)->next = st;
                    (*find)->type = st_expr;
                    (*find)->select = exprNode(en_func, NULL, NULL);
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
static void ThunkUndestructSyms(HASHTABLE *syms)
{
    HASHREC *hr = syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        sp->destructed = TRUE;
        hr = hr->next;
    }
}
static void thunkRetDestructors(EXPRESSION **exp, HASHTABLE *top, HASHTABLE *syms)
{
    if (syms)
    {
        if (syms != top)
        {
            thunkRetDestructors(exp, top, syms->chain);
            destructBlock(exp, syms->table[0], FALSE);
        }
    }
}
static void thunkGotoDestructors(EXPRESSION **exp, BLOCKDATA *gotoTab, BLOCKDATA *labelTab)
{
    // find the common parent
    BLOCKDATA *realtop;
    BLOCKDATA *top = getCommonParent(gotoTab, labelTab);
    if (gotoTab->next != top)
    {
        realtop = gotoTab;
        while (realtop->next != top)
            realtop = realtop->next;
        thunkRetDestructors(exp, realtop->table, gotoTab->table);
    }
}
static void InSwitch()
{

}
static void HandleStartOfCase(BLOCKDATA *parent)
{
    // this is a little buggy in that it doesn't check to see if we are already in a switch
    // statement, however if we aren't we should get a compile erroir that would halt program generation anyway
    if (parent != caseDestructBlock)
    {
        parent->caseDestruct = caseDestructBlock;
        caseDestructBlock = parent;
    }
}
static void HandleEndOfCase(BLOCKDATA *parent)
{
    if (parent == caseDestructBlock)
    {
        EXPRESSION *exp = NULL;
        STATEMENT *st;
        // the destruct is only used for endin
        destructBlock(&exp, localNameSpace->syms->table[0], FALSE);
        if (exp)
        {
            st = stmtNode(NULL, parent, st_nop);
            st->destexp = exp;
        }
        ThunkUndestructSyms(localNameSpace->syms);
    }
}
static void HandleEndOfSwitchBlock(BLOCKDATA *parent)
{
    if (parent == caseDestructBlock)
    {
        caseDestructBlock = caseDestructBlock->caseDestruct;
    }
}
static LEXEME *statement_break(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *breakableStatement = parent, *last = NULL;
    EXPRESSION *exp = NULL;
    (void)lex;
    (void)funcsp;
    (void)parent;
    while (breakableStatement && (breakableStatement->type == begin ||breakableStatement->type == kw_try ||breakableStatement->type == kw_catch || breakableStatement->type == kw_if || breakableStatement->type == kw_else))
    {
        last = breakableStatement;
        breakableStatement = breakableStatement->next;
    }
    
    if (!breakableStatement)
        error(ERR_BREAK_NO_LOOP);
    else
    {
        STATEMENT *st ;
        currentLineData(parent, lex, 0);
        if (last)
            thunkRetDestructors(&exp, last->table, localNameSpace->syms);
        st = stmtNode(lex, parent, st_goto);
        st->label = breakableStatement->breaklabel;
        st->destexp = exp;
        thunkCatchCleanup(st, funcsp, parent, breakableStatement);
        parent->needlabel = TRUE;
        breakableStatement->needlabel = FALSE;
        breakableStatement->hasbreak = TRUE;
    }
    return getsym();
}
static LEXEME *statement_case(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    LLONG_TYPE val;
    BLOCKDATA dummy;
    TYPE *tp = NULL;
    EXPRESSION *exp = NULL;
    BLOCKDATA *switchstmt = parent;
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
        switchstmt->needlabel = FALSE;
        parent->needlabel = FALSE;
    }
        
    lex = optimized_expression(lex, funcsp, NULL, &tp, &exp, FALSE);
    if (!tp)
    {
        error(ERR_EXPRESSION_SYNTAX);
    }
    else if (isintconst(exp))
    {
        CASEDATA **cases = &switchstmt->cases, *data;
        char *fname = lex->file;
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
            STATEMENT *st = stmtNode(lex, parent, st_label);
            st->label = codeLabel++;
            data = Alloc(sizeof(CASEDATA));
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
static LEXEME *statement_continue(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *continuableStatement = parent;
    BLOCKDATA *last = NULL;
    EXPRESSION *exp = NULL;
    (void)lex;
    (void)funcsp;
    while (continuableStatement && (continuableStatement->type == kw_switch || continuableStatement->type == begin || continuableStatement->type == kw_try || continuableStatement->type == kw_catch || continuableStatement->type == kw_if || continuableStatement->type == kw_else))
    {
        last = continuableStatement;
        continuableStatement = continuableStatement->next;
    }
    if (!continuableStatement)
        error(ERR_CONTINUE_NO_LOOP);
    else
    {
        STATEMENT *st;
        if (last)
            thunkRetDestructors(&exp, last->table, localNameSpace->syms);
        currentLineData(parent, lex, 0);
        st = stmtNode(lex, parent, st_goto);
        st->label = continuableStatement->continuelabel;		
        st->destexp = exp;
        thunkCatchCleanup(st, funcsp, parent, continuableStatement);
        parent->needlabel = TRUE;
    }
    return getsym();
}
static LEXEME *statement_default(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *defaultableStatement = parent;
    (void)lex;
    (void)funcsp;
    while (defaultableStatement && defaultableStatement->type != kw_switch)
        defaultableStatement = defaultableStatement->next;
    lex = getsym();
    if (!defaultableStatement)
        error(ERR_DEFAULT_NO_SWITCH);
    else
    {
        STATEMENT *st = stmtNode(lex, parent, st_label);
        st->label = codeLabel++;
        if (defaultableStatement->defaultlabel != -1)
            error(ERR_SWITCH_HAS_DEFAULT);
        defaultableStatement->defaultlabel = st->label;
        defaultableStatement->needlabel = FALSE;
        parent->needlabel = FALSE;
    }
    needkw(&lex, colon);
    return lex;
}
static LEXEME *statement_do(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *dostmt = Alloc(sizeof(BLOCKDATA)) ;
    STATEMENT *st, *lastLabelStmt;
    EXPRESSION *select = NULL;
    int addedBlock = 0;
    int loopLabel = codeLabel++;
    lex = getsym();
    dostmt->breaklabel = codeLabel++;
    dostmt->continuelabel = codeLabel++;
    dostmt->next = parent;
    dostmt->type = kw_do;
    dostmt->table = localNameSpace->syms;
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
        lex = statement(lex, funcsp, dostmt,TRUE);
    }
    while (lex && dostmt->tail != lastLabelStmt && dostmt->tail->purelabel);
    if (cparams.prm_cplusplus || cparams.prm_c99)
    {
        addedBlock--;
        FreeLocalContext(dostmt, funcsp, codeLabel++);
    }
    parent->nosemi = FALSE;
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
            lex = selection_expression(lex, dostmt, &select, funcsp, kw_do, NULL);
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
            if (!dostmt->hasbreak && isselecttrue(st->select))
                parent->needlabel = TRUE;
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
        parent->nosemi = TRUE;
        error(ERR_DO_STMT_NEEDS_WHILE);
        errskim(&lex, skim_semi);
        skip(&lex, semicolon);
    }
    while (addedBlock--)
        FreeLocalContext(dostmt, funcsp, codeLabel++);
    AddBlock(lex, parent, dostmt);
    return lex;
}
static LEXEME *statement_for(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *forstmt = Alloc(sizeof(BLOCKDATA)) ;
    STATEMENT *st, *lastLabelStmt;
    STATEMENT *forline ;
    int addedBlock = 0;
    EXPRESSION *init = NULL, *before = NULL, *select = NULL;
    int loopLabel = codeLabel++, testlabel = codeLabel++;
    forstmt->breaklabel = codeLabel++;
    forstmt->continuelabel = codeLabel++;
    forstmt->next = parent;
    forstmt->type = kw_for;
    forstmt->table = localNameSpace->syms;
    currentLineData(forstmt,lex,-1);
    forline = currentLineData(NULL, lex, 0);
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        BOOLEAN declaration = FALSE;
        lex = getsym();
        if (!MATCHKW(lex, semicolon))
        {
            BOOLEAN hasColon = FALSE;
            if ((cparams.prm_cplusplus && !cparams.prm_oldfor) || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            if (cparams.prm_cplusplus)
            {
                LEXEME *origLex = lex;
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
                TYPE *selectTP;
                SYMBOL *declSP = (SYMBOL *)localNameSpace->syms->table[0]->p;
                EXPRESSION *declExp;
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
                    declSP->dest = NULL;
                    declExp = varNode(en_auto, declSP);
                    declSP->assigned = declSP->used = TRUE;
                }
                lex = getsym();
                if (MATCHKW(lex, begin))
                {
                    assert(0);
                }
                else
                {
                    lex = expression_no_comma(lex, funcsp, NULL, &selectTP, &select, NULL, 0);
                }
                if (!selectTP || selectTP->type == bt_any)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                {
                    EXPRESSION *ibegin=NULL, *iend=NULL;
                    SYMBOL *sbegin=NULL, *send=NULL;
                    TYPE *iteratorType = NULL;
                    TYPE *tpref = Alloc(sizeof(TYPE));
                    EXPRESSION *rangeExp = anonymousVar(sc_auto, tpref);
                    SYMBOL *rangeSP = rangeExp->v.sp;
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
                            SYMBOL *beginFunc =NULL, *endFunc = NULL;
                            FUNCTIONCALL fcb, fce;
                            TYPE *ctp;
                            memset(&fcb, 0, sizeof(fcb));
                            fcb.thistp = &thisTP;
                            fcb.thisptr = rangeExp;
                            fcb.ascall = TRUE;
                            ctp = rangeSP->tp;
                            beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, sbegin, &fcb, NULL, FALSE, FALSE, TRUE, 0);
                            memset(&fce, 0, sizeof(fce));
                            fce.thistp = &thisTP;
                            fce.thisptr = rangeExp;
                            fce.ascall = TRUE;
                            ctp = rangeSP->tp;
                            endFunc = GetOverloadedFunction(&ctp, &fce.fcall, send, &fce, NULL, FALSE, FALSE, TRUE, 0);
                            if (beginFunc && endFunc)
                            {
                                if (!comparetypes(basetype(beginFunc->tp)->btp, basetype(endFunc->tp)->btp, TRUE))
                                {
                                    error(ERR_MISMATCHED_FORRANGE_BEGIN_END_TYPES);
                                }
                                else
                                {
                                    FUNCTIONCALL *fc;
                                    iteratorType = basetype(beginFunc->tp)->btp;
                                    if (isstructured(iteratorType))
                                    {
                                        INITIALIZER *dest;
                                        EXPRESSION *exp;
                                        fcb.returnEXP = anonymousVar(sc_auto, iteratorType);
                                        fcb.returnSP = fcb.returnEXP->v.sp;
                                        exp = fcb.returnEXP;
                                        dest = NULL;
                                        callDestructor(fcb.returnSP, NULL, &exp, NULL, TRUE, FALSE, FALSE);
                                        initInsert(&dest, iteratorType, exp, 0, TRUE);
                                        fcb.returnSP->dest = dest;
                                       
                                        fce.returnEXP = anonymousVar(sc_auto, iteratorType);
                                        fce.returnSP = fcb.returnEXP->v.sp;
                                        exp = fce.returnEXP;
                                        dest = NULL;
                                        callDestructor(fce.returnSP, NULL, &exp, NULL, TRUE, FALSE, FALSE);
                                        initInsert(&dest, iteratorType, exp, 0, TRUE);
                                        fce.returnSP->dest = dest;
                                    }
                                    fc = Alloc(sizeof(FUNCTIONCALL));
                                    *fc = fcb;
                                    fc->sp = beginFunc;
                                    fc->functp = beginFunc->tp;
                                    fc->ascall = TRUE;
                                    ibegin = exprNode(en_func, NULL, NULL);
                                    ibegin->v.func = fc;
                                    fc = Alloc(sizeof(FUNCTIONCALL));
                                    *fc = fce;
                                    fc->sp = endFunc;
                                    fc->functp = endFunc->tp;
                                    fc->ascall = TRUE;
                                    iend = exprNode(en_func, NULL, NULL);
                                    iend->v.func = fc;
                                    iteratorType = basetype(beginFunc->tp)->btp;
                                        
                                }
                                
                            }
                            else
                            {
                                ibegin = iend = NULL;
                            }
                        }
                        // possibly lookup in search area
                        if (!ibegin && !iend)
                        {
                            sbegin = namespacesearch("begin", globalNameSpace, FALSE, FALSE);
                            send = namespacesearch("end", globalNameSpace, FALSE, FALSE);
                            // now possibly lookup in namespace std
                            if (!sbegin || !send)
                            {
                                SYMBOL *standard = namespacesearch("std", globalNameSpace, FALSE, FALSE);
                                if (standard)
                                {
                                    sbegin = namespacesearch("begin", standard->nameSpaceValues, FALSE, FALSE);
                                    send = namespacesearch("end", standard->nameSpaceValues, FALSE, FALSE);
                                }
                            }
                            if (!sbegin || !send)
                            {
                                if (rangeSP->tp->btp->sp->parentNameSpace)
                                {
                                    sbegin = namespacesearch("begin", rangeSP->tp->btp->sp->parentNameSpace->nameSpaceValues, FALSE, FALSE);
                                    send = namespacesearch("end", rangeSP->tp->btp->sp->parentNameSpace->nameSpaceValues, FALSE, FALSE);                                    
                                }
                            }
                            {
                                SYMBOL *beginFunc =NULL, *endFunc = NULL;
                                INITLIST args;
                                FUNCTIONCALL fcb, fce;
                                TYPE *ctp;
                                memset(&fcb, 0, sizeof(fcb));
                                memset(&args, 0, sizeof(args));
                                args.tp = rangeSP->tp->btp;
                                args.exp = rangeExp;
                                fcb.arguments = &args;
                                fcb.ascall = TRUE;
                                ctp = rangeSP->tp;
                                beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, sbegin, &fcb, NULL, FALSE, FALSE, TRUE, 0);
                                memset(&fce, 0, sizeof(fce));
                                fce.arguments = &args;
                                fce.ascall = TRUE;
                                ctp = rangeSP->tp;
                                endFunc = GetOverloadedFunction(&ctp, &fce.fcall, send, &fce, NULL, FALSE, FALSE, TRUE, 0);
                                if (beginFunc && endFunc)
                                {
                                    TYPE *it2;
                                    it2 = iteratorType = basetype(beginFunc->tp)->btp;
                                    if (isref(it2))
                                        it2 = it2->btp;
                                    if (!comparetypes(basetype(beginFunc->tp)->btp, basetype(endFunc->tp)->btp, TRUE))
                                    {
                                        error(ERR_MISMATCHED_FORRANGE_BEGIN_END_TYPES);
                                    }
                                    else
                                    {
                                        FUNCTIONCALL *fc;
                                        if (isstructured(iteratorType))
                                        {
                                            INITIALIZER *dest;
                                            EXPRESSION *exp;
                                            fcb.returnEXP = anonymousVar(sc_auto, iteratorType);
                                            fcb.returnSP = fcb.returnEXP->v.sp;
                                            exp = fcb.returnEXP;
                                            dest = NULL;
                                            callDestructor(fcb.returnSP, NULL, &exp, NULL, TRUE, FALSE, FALSE);
                                            initInsert(&dest, iteratorType, exp, 0, TRUE);
                                            fcb.returnSP->dest = dest;
                                            
                                            fce.returnEXP = anonymousVar(sc_auto, iteratorType);
                                            fce.returnSP = fcb.returnEXP->v.sp;
                                            exp = fce.returnEXP;
                                            dest = NULL;
                                            callDestructor(fce.returnSP, NULL, &exp, NULL, TRUE, FALSE, FALSE);
                                            initInsert(&dest, iteratorType, exp, 0, TRUE);
                                            fce.returnSP->dest = dest;
                                        }
                                        fc = Alloc(sizeof(FUNCTIONCALL));
                                        *fc = fcb;
                                        fc->sp = beginFunc;
                                        fc->functp = beginFunc->tp;
                                        fc->ascall = TRUE;
                                        fc->arguments = Alloc(sizeof(INITLIST));                                        
                                        *fc->arguments = *fcb.arguments;
                                        if (isstructured(it2) && isstructured(((SYMBOL *)(it2->syms->table[0]->p))->tp))
                                        {
                                            EXPRESSION *consexp = anonymousVar(sc_auto, basetype(rangeSP->tp)->btp); // sc_parameter to push it...
                                            SYMBOL *esp = consexp->v.sp;
                                            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                            TYPE *ctype = basetype(rangeSP->tp)->btp;
                                            esp->stackblock = TRUE;
                                            funcparams->arguments = Alloc(sizeof(INITLIST));
                                            *funcparams->arguments = *fc->arguments;
                                            callConstructor(&ctype, &consexp, funcparams, FALSE, 0,TRUE, FALSE, TRUE, FALSE, FALSE);
                                            fc->arguments->exp = consexp;                                                             
                                        }
                                        else
                                        {
                                            fc->arguments->tp = Alloc(sizeof(TYPE));
                                            fc->arguments->tp->type = bt_lref;
                                            fc->arguments->tp->size = getSize(bt_pointer);
                                            fc->arguments->tp->btp = fcb.arguments->tp;
                                            fc->arguments->tp->rootType = fc->arguments->tp;
                                        }
                                        ibegin = exprNode(en_func, NULL, NULL);
                                        ibegin->v.func = fc;
                                        fc = Alloc(sizeof(FUNCTIONCALL));
                                        *fc = fce;
                                        fc->sp = endFunc;
                                        fc->functp = endFunc->tp;
                                        fc->ascall = TRUE;
                                        fc->arguments = Alloc(sizeof(INITLIST));
                                        *fc->arguments = *fce.arguments;
                                        if (isstructured(it2) && isstructured(((SYMBOL *)(it2->syms->table[0]->p))->tp))
                                        {
                                            EXPRESSION *consexp = anonymousVar(sc_auto, basetype(rangeSP->tp)->btp); // sc_parameter to push it...
                                            SYMBOL *esp = consexp->v.sp;
                                            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                            TYPE *ctype = basetype(rangeSP->tp)->btp;
                                            esp->stackblock = TRUE;
                                            funcparams->arguments = Alloc(sizeof(INITLIST));
                                            *funcparams->arguments = *fc->arguments;
                                            callConstructor(&ctype, &consexp, funcparams, FALSE, 0,TRUE, FALSE, TRUE, FALSE, FALSE);
                                            fc->arguments->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->tp = Alloc(sizeof(TYPE));
                                            fc->arguments->tp->type = bt_lref;
                                            fc->arguments->tp->size = getSize(bt_pointer);
                                            fc->arguments->tp->btp = fce.arguments->tp;
                                            fc->arguments->tp->rootType = fc->arguments->tp;
                                        }
                                        iend = exprNode(en_func, NULL, NULL);
                                        iend->v.func = fc;
                                    }
                                    
                                }
                                else
                                {
                                    ibegin = iend = NULL;
                                }
                            }
                        }
                    }
                    if (ibegin && iend)
                    {
                        EXPRESSION *compare;
                        EXPRESSION *eBegin;
                        EXPRESSION *eEnd;
                        EXPRESSION *declDest = NULL;
                        if (isstructured(selectTP) && isstructured(iteratorType) && ibegin->type == en_func && iend->type == en_func )
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
                            SYMBOL *sBegin;
                            SYMBOL *sEnd;
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
                            TYPE *eqType = iteratorType;
                            compare = eBegin;
                            if (!insertOperatorFunc(ovcl_unary_prefix, eq,
                                                   funcsp, &eqType, &compare, iteratorType, eEnd, NULL, 0))
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
                            TYPE **tp = &declSP->tp;
                            if (isref(*tp))
                                tp = &(*tp)->btp;
                            while (isconst(*tp) || isvolatile(*tp))
                                tp = &(*tp)->btp;
                            *tp = assignauto(*tp, basetype(selectTP)->btp);
                            UpdateRootTypes(declSP->tp);
                            if (isarray(selectTP) && !comparetypes(declSP->tp, basetype(selectTP)->btp, TRUE))
                            {
                                error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                            }
                            if (isstructured(declSP->tp))
                            {
                                EXPRESSION *decl = declExp;
                                TYPE *ctype = declSP->tp;
                                FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                INITLIST *args = Alloc(sizeof(INITLIST));
                                funcparams->arguments = args;
                                args->tp = declSP->tp;
                                args->exp = eBegin;
                                callConstructor(&ctype, &decl,funcparams, FALSE, 0, TRUE, FALSE, TRUE, FALSE, FALSE);
                                st->select = decl;
                                declDest = declExp;
                                callDestructor(declSP, NULL, &declDest, NULL, TRUE, FALSE, FALSE);
                            }
                            else if (isarray(selectTP))
                            {
                                EXPRESSION *decl = declExp;
                                deref(declSP->tp, &decl);
                                st->select = eBegin;
                                if (!isref(declSP->tp))
                                    deref(basetype(selectTP)->btp, &st->select);
                                st->select = exprNode(en_assign, decl, st->select);
                            }
                        }
                        else
                        {
                            TYPE *starType = iteratorType;
                            st->select = eBegin;
                            if (ispointer(iteratorType))
                            {
                                TYPE **tp = &declSP->tp;
                                if (isref(*tp))
                                    tp = &(*tp)->btp;
                                while (isconst(*tp) || isvolatile(*tp))
                                    tp = &(*tp)->btp;
                                *tp = assignauto(*tp, basetype(iteratorType)->btp);
                                UpdateRootTypes(declSP->tp);
                                if (!comparetypes(declSP->tp, basetype(iteratorType)->btp, TRUE))
                                {
                                    error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                }
                                else if (!isstructured(declSP->tp))
                                {
                                    EXPRESSION *decl = declExp;
                                    deref(declSP->tp, &decl);
                                    st->select = eBegin;
                                    if (!isref(declSP->tp))
                                        deref(basetype(iteratorType)->btp, &st->select);
                                    st->select = exprNode(en_assign, decl, st->select);
                                }
                                else
                                {
                                    EXPRESSION *decl = declExp;
                                    TYPE *ctype = declSP->tp;
                                    FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                    INITLIST *args = Alloc(sizeof(INITLIST));
                                    funcparams->arguments = args;
                                    args->tp = declSP->tp;
                                    args->exp = eBegin;
                                    callConstructor(&ctype, &decl,funcparams, FALSE, 0, TRUE, FALSE, TRUE, FALSE, FALSE);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(declSP, NULL, &declDest, NULL, TRUE, FALSE, FALSE);
                                }
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, star,
                                                   funcsp, &starType, &st->select, NULL,NULL, NULL, 0))
                            {
                                error(ERR_MISSING_OPERATOR_STAR_FORRANGE_ITERATOR);
                                
                            }
                            else {
                                TYPE **tp = &declSP->tp;
                                BOOLEAN ref = FALSE;
                                if (isref(*tp))
                                {
                                    ref = TRUE;
                                    tp = &(*tp)->btp;
                                }
                                while (isconst(*tp) || isvolatile(*tp))
                                    tp = &(*tp)->btp;
                                *tp = assignauto(*tp, starType);
                                UpdateRootTypes(declSP->tp);
                                if (!comparetypes(declSP->tp, starType, TRUE))
                                {
                                    comparetypes(declSP->tp, starType, TRUE);
                                    error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                                }
                                else if (!isstructured(declSP->tp))
                                {
                                    EXPRESSION *decl = declExp;
                                    if (ref && (starType->lref || starType ->rref))
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
                                    EXPRESSION *decl = declExp;
                                    TYPE *ctype = declSP->tp;
                                    FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                    INITLIST *args = Alloc(sizeof(INITLIST));
                                    funcparams->arguments = args;
                                    args->tp = declSP->tp;
                                    args->exp = st->select;
                                    callConstructor(&ctype, &decl,funcparams, FALSE, 0, TRUE, FALSE, TRUE, FALSE, FALSE);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(declSP, NULL, &declDest, NULL, TRUE, FALSE, FALSE);
                                }
                            }
                        }
                        do
                        {
                            lastLabelStmt = forstmt->tail;
                            lex = statement(lex, funcsp, forstmt,TRUE);
                        }
                        while (lex && forstmt->tail != lastLabelStmt && forstmt->tail->purelabel);
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
                                st->select = exprNode(en_assign, eBegin, exprNode(en_add, eBegin, intNode(en_c_i, basetype(basetype(selectTP)->btp)->size)));
                        }
                        else
                        {
                            TYPE *ppType = iteratorType;
                            ppType = basetype(ppType);
                            st->select = eBegin;
                            if (ispointer(iteratorType))
                            {
                                st->select = exprNode(en_assign, eBegin, exprNode(en_add, eBegin, intNode(en_c_i, basetype(basetype(iteratorType)->btp)->size)));
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, autoinc,
                                                   funcsp, &ppType, &st->select, NULL,NULL, NULL, 0))
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
                                    callDestructor(st->select->v.func->returnSP, NULL, &declDest, NULL, TRUE, FALSE, FALSE);
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
                            parent->needlabel = TRUE;
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
                    FreeLocalContext( forstmt, funcsp, codeLabel++);
                AddBlock(lex, parent, forstmt);
                return lex;
            }
            else
            {
                if (declaration && cparams.prm_cplusplus)
                {
                    HASHREC *hr = localNameSpace->syms->table[0];
                    while (hr && ((SYMBOL *)hr->p)->anonymous)
                        hr = hr->next;
                    if (!hr)
                    {
                        error(ERR_FOR_DECLARATOR_MUST_INITIALIZE);
                    }
                    else
                    {
                        SYMBOL *declSP = (SYMBOL *)hr->p;
                        if (!declSP->init)
                        {
                            if (isstructured(declSP->tp) && !basetype(declSP->tp)->sp->trivialCons)
                            {
                                lex = initialize(lex, funcsp, declSP, sc_auto, FALSE, 0);
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
                TYPE *tp = NULL;
                lex = optimized_expression(lex, funcsp, NULL, &tp, &select, TRUE);
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
                    TYPE *tp = NULL;
                    lex = optimized_expression(lex, funcsp, NULL, &tp, &before, TRUE);
                    if (!tp)
                    {
                        error(ERR_EXPRESSION_SYNTAX);
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
                        lex = statement(lex, funcsp, forstmt,TRUE);
                    }
                    while (lex && forstmt->tail != lastLabelStmt && forstmt->tail->purelabel);
                    if (cparams.prm_cplusplus || cparams.prm_c99)
                    {
                        addedBlock--;
                        FreeLocalContext(forstmt, funcsp, codeLabel++);
                    }
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
                        parent->needlabel = TRUE;
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
static LEXEME *statement_if(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st, *st1, *st2, *lastLabelStmt ;
    EXPRESSION *select=NULL;
    int addedBlock = 0;
    BOOLEAN needlabelif;
    BOOLEAN needlabelelse = FALSE;
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
        lex = selection_expression(lex, parent, &select, funcsp, kw_if, NULL);
        if (MATCHKW(lex, closepa))
        {
            BOOLEAN optimized = FALSE;
            STATEMENT *sti;
            currentLineData(parent, lex, 0);
            lex = getsym();
            st = stmtNode(lex, parent, st_notselect);
            st->label = ifbranch;
            st->select = select;
            sti = st;
            parent->needlabel = FALSE;
            if (cparams.prm_cplusplus || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp, codeLabel++);
            }
            do
            {
                lastLabelStmt = parent->tail;
                lex = statement(lex, funcsp, parent,TRUE);
            }
            while (lex && parent->tail != lastLabelStmt && parent->tail->purelabel);
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
                        optimized = TRUE;
                        st2->next = NULL;
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
                parent->needlabel = FALSE;
                do
                {
                    lastLabelStmt = parent->tail;
                    lex = statement(lex, funcsp, parent,TRUE);
                }
                while (lex && parent->tail != lastLabelStmt && parent->tail->purelabel);
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
//						optimized = TRUE;
                        st2->next = NULL;
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
//						optimized = TRUE;
                        st2->next = NULL;
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
                parent->nosemi = TRUE;
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
static LEXEME *statement_goto(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    (void)funcsp;
    (void)parent;
    lex = getsym();
    currentLineData(parent, lex, 0);
    if (ISID(lex))
    {
        SYMBOL *spx = search(lex->value.s.a, labelSyms);
        BLOCKDATA *block = Alloc(sizeof(BLOCKDATA));
        STATEMENT *st = stmtNode(lex, block, st_goto);
        block->next = parent;
        block->type = begin;
        block->table = localNameSpace->syms;
        if (!spx)
        {
            spx = makeID(sc_ulabel, NULL, NULL, litlate(lex->value.s.a));
            spx->declfile = spx->origdeclfile = lex->file;
            spx->declline = spx->origdeclline = lex->line;
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
        parent->needlabel = TRUE;
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
static LEXEME *statement_label(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    SYMBOL *spx = search(lex->value.s.a, labelSyms);
    STATEMENT *st;
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
        spx = makeID(sc_label, NULL, NULL, litlate(lex->value.s.a));
        SetLinkerNames(spx, lk_none);
        spx->offset = codeLabel++;
        spx->gotoTable = st;
        insert(spx, labelSyms);
    }
    st->label = spx->offset;
    st->purelabel = TRUE;
    getsym(); /* colon */
    lex = getsym();	/* next sym */
    parent->needlabel = FALSE;
    return lex;
}
static EXPRESSION *ConvertReturnToRef(EXPRESSION *exp, TYPE *tp, TYPE *boundTP)
{
    if (lvalue(exp))
    {
        EXPRESSION *exp2;
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
        else if (exp->type == en_auto)
        {
            if (exp->v.sp->storage_class == sc_auto)
            {
                error(ERR_REF_RETURN_LOCAL);
            }
        }
    }
    return exp;
}
static SYMBOL *baseNode(EXPRESSION *node)
{
    if (!node)
        return 0;
    switch (node->type)
    {
        case en_label:
        case en_auto:
        case en_pc:
        case en_global:
        case en_tempref:
        case en_threadlocal:
            return node;
        case en_add:
        {
            EXPRESSION *rv = baseNode(node->left);
            if (rv)
                return rv;
            return baseNode(node->right);
        }
        default:
            return 0;
    }
}
static void MatchReturnTypes(SYMBOL *funcsp, TYPE *tp1, TYPE *tp2)
{
    if (matchReturnTypes)
    {
        BOOLEAN err = FALSE;
        if (isref(tp1))
            tp1 = basetype(tp1)->btp;
        if (isref(tp2))
            tp2 = basetype(tp2)->btp;
        while (tp1 && tp2 && !err)
        {
            //if (isconst(tp1) != isconst(tp2) || isvolatile(tp1) != isvolatile(tp2))
            //    err = TRUE;
            tp1 = basetype(tp1);
            tp2 = basetype(tp2);
            if (tp1->type != tp2->type)
                err = TRUE;
            tp1 = tp1->btp;
            tp2 = tp2->btp;
        }
        if (tp1 || tp2 || err)
        {
            errorsym(ERR_RETURN_TYPE_MISMATCH_FOR_AUTO_FUNCTION, funcsp);
        }
    }
}
static LEXEME *statement_return(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st;
    TYPE *tp = NULL;
    EXPRESSION *returnexp = NULL;
    TYPE *returntype = NULL;
    EXPRESSION *destexp = NULL;

    if (funcsp->linkage3 == lk_noreturn)
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
            TYPE *tp1;
            EXPRESSION *exp1;
            LEXEME *current = lex;
            lex = expression(lex, funcsp, NULL, &tp1, &exp1, 0);
            lex = prevsym(current);
            while (tp1->type == bt_typedef)
                tp1 = tp1->btp;
            basetype(funcsp->tp)->btp = tp = assignauto(basetype(funcsp->tp)->btp, tp1);
            UpdateRootTypes(funcsp->tp);
            SetLinkerNames(funcsp, funcsp->linkage);
            matchReturnTypes = TRUE;
        }
        if (isstructured(tp) || basetype(tp)->type == bt_memberptr)
        {
            EXPRESSION *en = anonymousVar(sc_parameter, &stdpointer);
            SYMBOL *sp = en->v.sp;
            BOOLEAN maybeConversion = TRUE;
            sp->allocate = FALSE; // static var
            sp->offset = chosenAssembler->arch->retblocksize;
            sp->structuredReturn = TRUE;
            sp->name = "__retblock";
            if ((funcsp->linkage == lk_pascal) &&
                    basetype(funcsp->tp)->syms->table[0] && 
                    ((SYMBOL *)basetype(funcsp->tp)->syms->table[0])->tp->type != bt_void)
                sp->offset = funcsp->paramsize;
            deref(&stdpointer, &en);
            if (cparams.prm_cplusplus && isstructured(tp))
            {
                BOOLEAN implicit = FALSE;
                if (basetype(tp)->sp->templateLevel && basetype(tp)->sp->templateParams && !basetype(tp)->sp->instantiated)
                {
                    SYMBOL * sp = basetype(tp)->sp;
                    if (!allTemplateArgsSpecified(sp, sp->templateParams))
                        sp = GetClassTemplate(sp, sp->templateParams->next, FALSE);
                    if (sp)
                        tp = TemplateClassInstantiate(sp, sp->templateParams, FALSE, sc_global)->tp;
                }
                if (MATCHKW(lex, begin))
                {
                    INITIALIZER *init = NULL, *dest=NULL;
                    SYMBOL *sp = NULL;
                    sp = anonymousVar(sc_localstatic, tp)->v.sp;
                    lex = initType(lex, funcsp, 0, sc_auto, &init, &dest, tp, sp, FALSE, 0);
                    returnexp = convertInitToExpression(tp, NULL, funcsp, init, en, FALSE);
                    returntype = tp;
                    if (sp)
                        sp->dest = dest;
                }
                else
                {
                    BOOLEAN oldrref, oldlref;
                    FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                    TYPE *ctype = tp;
                    // shortcut for conversion from single expression
                    EXPRESSION *exp1 = NULL, *exp2, *exp3 = NULL;
                    TYPE *tp1 = NULL;
                    lex = expression_no_comma(lex, funcsp, NULL, &tp1, &exp1, NULL, 0);
                    MatchReturnTypes(funcsp, tp, tp1);
                    if (tp1 && isstructured(tp1))
                    {
                        if (basetype(tp1)->sp->templateLevel && basetype(tp1)->sp->templateParams && !basetype(tp1)->sp->instantiated)
                        {
                            SYMBOL * sp = basetype(tp1)->sp;
                            if (!allTemplateArgsSpecified(sp, sp->templateParams))
                                sp = GetClassTemplate(sp, sp->templateParams->next, FALSE);
                            if (sp)
                                tp1 = TemplateClassInstantiate(sp, sp->templateParams, FALSE, sc_global)->tp;
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
                        (exp2->v.func->sp->isConstructor && comparetypes(basetype(exp2->v.func->thistp)->btp, tp1, TRUE) ||
                         !exp2->v.func->sp->isConstructor && exp2->v.func->returnSP && comparetypes(exp2->v.func->returnSP->tp, tp1, TRUE)))
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
                        funcparams->arguments = Alloc(sizeof(INITLIST));
                        funcparams->arguments->tp = tp1;
                        funcparams->arguments->exp = exp1;
                        oldrref = basetype(tp1)->rref;
                        oldlref = basetype(tp1)->lref;
                        basetype(tp1)->rref = exp1->type == en_func || exp1->type == en_thisref || exp1->type == en_auto && exp1->v.sp->storage_class != sc_parameter;// || lvalue(exp1) && (exp1->type != en_l_ref || exp1->left->type != en_auto || exp1->left->v.sp->storage_class != sc_parameter);
                        basetype(tp1)->lref = !basetype(tp1)->rref;
                        maybeConversion = FALSE;
                        returntype = tp;
                        implicit = TRUE;
                        callConstructor(&ctype, &en, funcparams, FALSE, NULL, TRUE, maybeConversion, FALSE, FALSE, FALSE); 
                        returnexp = en;
                        basetype(tp1)->rref = oldrref;
                        basetype(tp1)->lref = oldlref;
                        if (funcparams->sp && matchesCopy(funcparams->sp, TRUE))
                        {
                            switch (exp1->type)
                            {
                                case en_global:
                                case en_label:
                                case en_auto:
                                case en_threadlocal:
                                    exp1->v.sp->dest = NULL;
                                    break;
                            }
                        }
                    }
                }
            }
            else
            {
                    
                TYPE *tp1;
                lex = optimized_expression(lex, funcsp, NULL, &tp1, &returnexp, TRUE);
                if (!tp1)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                    MatchReturnTypes(funcsp, tp, tp1);
                if (!comparetypes(tp, tp1, TRUE))
                {
                    errortype(ERR_CANNOT_CONVERT_TYPE, tp1, tp);
                }
                else
                {
                    if (returnexp->type == en_func && !returnexp->v.func->ascall)
                    {
                        if (returnexp->v.func->sp->storage_class == sc_overloads)
                        {
                            SYMBOL *funcsp;
                            if (returnexp->v.func->sp->parentClass && !returnexp->v.func->asaddress)
                                error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                            funcsp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &returnexp, 0);
                            if (funcsp && basetype(tp)->type == bt_memberptr)
                            {
                                int lbl = dumpMemberPtr(funcsp, tp, TRUE);
                                returnexp = intNode(en_labcon, lbl);
                            }
                        }
                        else
                        {
                            returnexp = intNode(en_labcon, dumpMemberPtr(returnexp->v.func->sp, tp, TRUE));
                        }
                    }
                    else  if (returnexp->type == en_pc || returnexp->type == en_memberptr)
                    {
                        returnexp = intNode(en_labcon, dumpMemberPtr(returnexp->v.sp, tp, TRUE));
                    }
                    if (!chosenAssembler->msil || funcsp->linkage2 == lk_unmanaged || !chosenAssembler->msil->managed(funcsp))
                    { 
                        returnexp = exprNode(en_blockassign, en, returnexp);
                        returnexp->size = basetype(tp)->size;
                        returnexp->altdata = (long)(basetype(tp));
                    }
                    returntype = tp;
                }
            }
        }
        else
        {
            TYPE *tp1 = NULL;
            BOOLEAN needend = FALSE;
            if (MATCHKW(lex, begin))
            {
                needend = TRUE;
                lex = getsym();
            }
            lex = optimized_expression(lex, funcsp, tp, &tp1, &returnexp, TRUE);
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
                    returnexp = exprNode(en_x_string, returnexp, NULL);
                tp1 = &std__string;
            }
            else if (!comparetypes(tp1, tp, TRUE) && ismsil(tp1))
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
                if (basetype(tp1)->type != bt___object)
                    returnexp = exprNode(en_x_object, returnexp, NULL);
            if (isstructured(tp1) && isarithmetic(tp))
            {
                if (cparams.prm_cplusplus)
                {
                    castToArithmetic(FALSE, &tp1, &returnexp, (enum e_kw)-1, tp, TRUE);
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
                    SYMBOL *funcsp;
                    EXPRESSION *exp1 = returnexp;
                    if (returnexp->v.func->sp->parentClass && !returnexp->v.func->asaddress)
                        error(ERR_NO_IMPLICIT_MEMBER_FUNCTION_ADDRESS);
                    returnexp->v.func->sp = MatchOverloadedFunction(tp, &tp1, returnexp->v.func->sp, &exp1, 0);
                    returnexp->v.func->fcall = varNode(en_pc, returnexp->v.func->sp);
                }
            }
            if (cparams.prm_cplusplus && isstructured(returntype))
            {
                TYPE *tp1 = basetype(funcsp->tp)->btp; 
                if (isref(tp1))
                    tp1 = basetype(tp1)->btp;
                if (cppCast(returntype, &tp1, &returnexp))
                    returntype = tp = basetype(funcsp->tp)->btp;
            }            
        }
        if (isref(basetype(funcsp->tp)->btp))
        {
            if (basetype(basetype(tp)->btp)->type != bt_memberptr)
                returnexp=ConvertReturnToRef(returnexp, basetype(funcsp->tp)->btp, returntype);
        }
        else if (returnexp && returnexp->type == en_auto && 
            returnexp->v.sp->storage_class == sc_auto)
        {
            if (!isstructured(basetype(funcsp->tp)->btp) && 
                basetype(basetype(funcsp->tp)->btp)->type != bt_memberptr)
                error(ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE);
        }
        if (!returnexp)
            returnexp = intNode(en_c_i, 0); // errors
    }
    currentLineData(parent, lex, 0);
    thunkRetDestructors(&destexp, NULL, localNameSpace->syms);
    st = stmtNode(lex, parent, st_return);
    st->select = returnexp;
    st->destexp = destexp;
    thunkCatchCleanup(st, funcsp, parent, NULL); // to top level
    if (returnexp && returntype && !isautotype(returntype) && !matchReturnTypes)
    {
        if (!tp) // some error...
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
                if (!comparetypes(returntype, tp, FALSE) && !sameTemplate(returntype, tp))
                    error(ERR_RETMISMATCH);
            }
            else if (basetype(returntype)->type == bt_memberptr || basetype(tp)->type == bt_memberptr)
            {
                if (isconstzero(tp,st->select))
                {
                    int lbl = dumpMemberPtr(NULL, returntype, TRUE);
                    st->select = intNode(en_labcon, lbl);
                }
                else 
                {
                    if (st->select->type == en_memberptr)
                    {
                        int lbl = dumpMemberPtr(st->select->v.sp, returntype, TRUE);
                        st->select = intNode(en_labcon, lbl);
                    }
                    if (!comparetypes(returntype, tp, TRUE))
                        error(ERR_RETMISMATCH);
                }
            }
            else 
            {
                if (!isref(basetype(funcsp->tp)->btp))
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
                        if (!comparetypes(returntype, tp, TRUE))
                        {
                            if (!isvoidptr(returntype) && !isvoidptr(tp))
                            {
                                if (!matchingCharTypes(returntype, tp))
                                    error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                            }
                            else 
                            {
                                if (cparams.prm_cplusplus && !isvoidptr(returntype) && returnexp->type != en_nullptr && isvoidptr(tp))
                                    error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                            }
                        }
                    }
                    else if (isfunction(tp))
                    {
                        if (!isvoidptr(returntype) && 
                            (!isfunction(basetype(returntype)->btp) || !comparetypes(returntype, tp, TRUE)))
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
    parent->needlabel = TRUE;
    return lex;
}
static LEXEME *statement_switch(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *switchstmt = Alloc(sizeof(BLOCKDATA)) ;
    STATEMENT *st;
    EXPRESSION *select = NULL;
    int addedBlock = 0;
    funcsp->noinline = TRUE;
    switchstmt->breaklabel = codeLabel++;
    switchstmt->next = parent;
    switchstmt->defaultlabel = -1; /* no default */
    switchstmt->type = kw_switch;
    switchstmt->table = localNameSpace->syms;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        lex = selection_expression(lex, switchstmt, &select, funcsp, kw_switch, NULL);
        if (MATCHKW(lex, closepa))
        {
            STATEMENT *st1;
            currentLineData(switchstmt, lex, 0);
            lex = getsym();
            st = stmtNode(lex, switchstmt, st_switch);
            st->select = select;
            st->breaklabel = switchstmt->breaklabel;
            lex = statement(lex, funcsp, switchstmt, TRUE);
            st->cases = switchstmt->cases;
            st->label = switchstmt->defaultlabel;
            if (st->label != -1 && switchstmt->needlabel && !switchstmt->hasbreak)
                parent->needlabel = TRUE;
            /* force a default if there was none */
            if (st->label == -1)
            {
                st->label = codeLabel;
                st = stmtNode(lex, switchstmt, st_label);
                st->label = codeLabel++;
            }
            st = stmtNode(lex, switchstmt, st_label);
            st->label = switchstmt->breaklabel ;
            if (!switchstmt->nosemi && !switchstmt->hassemi)
                errorint(ERR_NEEDY, ';');
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
static LEXEME *statement_while(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *whilestmt = Alloc(sizeof(BLOCKDATA))  ;
    STATEMENT *st, *lastLabelStmt;
    STATEMENT *whileline;
    EXPRESSION *select = NULL;
    BOOLEAN addedBlock = FALSE;
    int loopLabel = codeLabel++;
    whilestmt->breaklabel = codeLabel++;
    whilestmt->continuelabel = codeLabel++;
    whilestmt->next = parent;
    whilestmt->type = kw_while;
    whilestmt->table = localNameSpace->syms;
    whileline = currentLineData(NULL, lex, 0);
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp, codeLabel++);
        }
        lex = selection_expression(lex, whilestmt, &select, funcsp, kw_while, NULL);
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
                lex = statement(lex, funcsp, whilestmt,TRUE);
            }
            while (lex && whilestmt->tail != lastLabelStmt && whilestmt->tail->purelabel);
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
                parent->needlabel = TRUE;
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
static void checkNoEffect(EXPRESSION *exp)
{
    if (exp->noexprerr)
        return;
        switch(exp->type)
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
                break;                
            case en_not_lvalue:
            case en_lvalue:
            case en_thisref:
            case en_literalclass:
            case en_funcret:
                checkNoEffect(exp->left);
                break;
            default:
                error(ERR_EXPRESSION_HAS_NO_EFFECT);
                break;
        }
}
static LEXEME *statement_expr(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st ;
    EXPRESSION *select = NULL;
    TYPE *tp = NULL;
    LINEDATA *oldLineHead = linesHead, *oldLineTail = linesTail;
    linesHead = linesTail = NULL;
    (void)parent;
    lex = optimized_expression(lex, funcsp, NULL, &tp, &select, TRUE);
    linesHead = oldLineHead;
    linesTail = oldLineTail;
    currentLineData(parent, lex, 0);
    st = stmtNode(lex, parent, st_expr);
    st->select = select;
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    else if (tp->type != bt_void && tp->type != bt_any)
    {
        checkNoEffect(st->select);
        if (cparams.prm_cplusplus && isstructured(tp) && select->type == en_func)
        {
            SYMBOL *sp = select->v.func->returnSP;
            if (sp && sp->allocate)
            {
                INITIALIZER *init = NULL;
                EXPRESSION *exp = select->v.func->returnEXP;
                callDestructor(basetype(tp)->sp, NULL, &exp, NULL, TRUE, FALSE, FALSE);
                initInsert(&init, sp->tp, exp, 0, FALSE);                
                sp->dest = init;
            }
        }
    }
    return lex;
}
static LEXEME *asm_declare(LEXEME *lex)
{
    enum e_kw kw = lex->kw->key;
    do
    {
        lex = getsym();
        if (lex)
        {
            if (ISID(lex))
            {
                SYMBOL *sp = search(lex->value.s.a, globalNameSpace->syms);
                if (!sp)
                {
                    sp = makeID(sc_label,NULL, NULL, litlate(lex->value.s.a));
                }
                switch(kw)
                {
                    case kw_public:
                        if (sp->storage_class != sc_global)
                            InsertGlobal(sp);
                        sp->storage_class = sc_global;
                        break;
                    case kw_extern:
                        if (sp->storage_class != sc_external)
                            InsertExtern(sp);
                        sp->storage_class = sc_external;
                        break;
                    case kw_const:
                        sp->storage_class = sc_constant;
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
LEXEME *statement_catch(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent, 
                        int label, int startlab, int endlab)
{
    BOOLEAN last = FALSE;
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
            STATEMENT *st;
            TYPE *tp = NULL;
            BLOCKDATA *catchstmt = Alloc(sizeof(BLOCKDATA));
            ParseAttributeSpecifiers(&lex, funcsp, TRUE);
            catchstmt->breaklabel = label;
            catchstmt->next = parent;
            catchstmt->defaultlabel = -1; /* no default */
            catchstmt->type = kw_catch;
            catchstmt->table = localNameSpace->syms;
            AllocateLocalContext(catchstmt, funcsp, codeLabel++);
            if (MATCHKW(lex, ellipse))
            {
                last = TRUE;
                lex = getsym();
            }
            else
            {
                lex = declare(lex, funcsp, &tp, sc_catchvar, lk_none, catchstmt , FALSE, TRUE,FALSE, FALSE, ac_public);               
            }
            if (needkw(&lex, closepa))
            {
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
LEXEME *statement_try(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st;
    BLOCKDATA *trystmt = Alloc(sizeof(BLOCKDATA));
    hasXCInfo = TRUE;
    trystmt->breaklabel = codeLabel++;
    trystmt->next = parent;
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
        AllocateLocalContext(trystmt, funcsp, codeLabel++);
        lex = compound(lex, funcsp, trystmt, FALSE);
        FreeLocalContext(trystmt, funcsp, codeLabel++);
        parent->needlabel = trystmt->needlabel;
        st = stmtNode(lex, parent, st_try);
        st->label = codeLabel++;
        st->endlabel = codeLabel++;
        st->breaklabel = trystmt->breaklabel;
        st->blockTail = trystmt->blockTail;
        st->lower = trystmt->head;
        parent->nosemi = TRUE;
        if (parent->next)
            parent->next->nosemi = TRUE;
        lex = statement_catch(lex, funcsp, parent,st->breaklabel, st->label, st->endlabel);
    }
    
    return lex;
}
LEXEME *statement_asm(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    (void)funcsp;
    (void)parent;
    functionHasAssembly = TRUE;
    if (chosenAssembler->inlineAsm)
    {
        parent->hassemi = FALSE;
        lex = getsym();
        if (MATCHKW(lex, begin))
        {
            lex = getsym();
            while (lex && !MATCHKW(lex, end))
            {
                currentLineData(parent, lex, 0);
                lex = chosenAssembler->inlineAsm(lex, parent);
                if (KW(lex) == semicolon)
                {
                    skip(&lex, semicolon);
                }
            }
            needkw(&lex, end);
            parent->nosemi = TRUE;
            return lex;
        }
        else
        {
            currentLineData(parent, lex, 0);
            while (cparams.prm_assemble && lex && MATCHKW(lex, semicolon))
                lex = SkipToNextLine();
            if (lex)
            {
                if (cparams.prm_assemble &&
                    (MATCHKW(lex, kw_public) || MATCHKW(lex, kw_extern) ||
                    MATCHKW(lex, kw_const)))
                {
                    lex = asm_declare(lex);
                }
                else
                {
                    lex = chosenAssembler->inlineAsm(lex, parent);
                }
                if (MATCHKW(lex, semicolon))
                {
                    if (cparams.prm_assemble)
                        lex = SkipToNextLine();
                    else
                        skip(&lex, semicolon);
                }
            }
            parent->hassemi = TRUE;
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
            parent->nosemi = TRUE;			
         }
         else
         {
             /* problematic, ASM keyword without a block->  Skip to end of line... */
            currentLineData(parent, lex, 0);
            parent->hassemi = TRUE;
            while (*includes->lptr)
                includes->lptr++;
            lex = getsym();
         }
         
    }
    return lex;
}
static void reverseAssign(STATEMENT *current, EXPRESSION **exp)
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
static LEXEME *autodeclare(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BLOCKDATA *parent, BOOLEAN asExpression)
{
    BLOCKDATA block;
    (void)parent;
    declareAndInitialize = FALSE;
    memset(&block, 0, sizeof(block));
    lex = declare(lex, funcsp, tp, sc_auto, lk_none, &block, FALSE, asExpression, FALSE, FALSE, ac_public);

    // move any auto assignments
    reverseAssign(block.head, exp);
    
    // now move variable declarations
    while (block.head)
    {
        if (block.head->type == st_varstart)
        {
            STATEMENT *s = stmtNode(lex, parent, st_varstart);
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
BOOLEAN resolveToDeclaration(LEXEME * lex)
{
    LEXEME *placeholder = lex;
    if (ISKW(lex))
        switch (KW(lex))
        {
            case kw_struct:
            case kw_union:
            case kw_class:
                return TRUE;
        }
    lex = getsym();
    if (MATCHKW(lex, begin))
    {
        prevsym(placeholder);
        return FALSE;
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
        while (level && lex != NULL && !MATCHKW(lex, semicolon))
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
        BOOLEAN sawClose = FALSE;
        int level = 1;
        lex = getsym();
        while (level && lex != NULL && !MATCHKW(lex, semicolon))
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
            return TRUE;
        }
        prevsym(placeholder);
        return FALSE;
    }
    prevsym(placeholder);
    return TRUE;
}
static LEXEME *statement(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent, 
                         BOOLEAN viacontrol)
{
    ParseAttributeSpecifiers(&lex, funcsp, TRUE);
    if (ISID(lex))
    {
        lex = getsym();
        if (MATCHKW(lex, colon))
        {
            lex = backupsym();
            lex = statement_label(lex, funcsp, parent);
            parent->needlabel = FALSE;
            parent->nosemi = TRUE;
            return lex;
        }
        else
        {
            lex = backupsym();
        }
    }
    if (parent->needlabel && !MATCHKW(lex, kw_case) && !MATCHKW(lex, kw_default)
        && !MATCHKW(lex, begin))
        error(ERR_UNREACHABLE_CODE);

    if (!MATCHKW(lex,begin))
        if (MATCHKW(lex, kw_throw))
            parent->needlabel = TRUE;
        else
            parent->needlabel = FALSE;
    
    parent->nosemi = FALSE;
    switch(KW(lex))
    {
        case kw_try:
            lex = statement_try(lex, funcsp, parent);
            break;
        case kw_catch:
            error(ERR_CATCH_WITHOUT_TRY);
            lex = statement_catch(lex, funcsp, parent,1,1,1);
            break;
        case begin:
            lex = compound(lex, funcsp, parent, FALSE);
            parent->nosemi = TRUE;
            if (parent->next)
                parent->next->nosemi = TRUE;
            break;
        case end:
            /* don't know how it could get here :) */
//			error(ERR_UNEXPECTED_END_OF_BLOCKDATA);
//			lex = getsym();
            parent->hassemi = TRUE;
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
            parent->nosemi = TRUE;
            break;
        case kw_case:
            if (cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            lex = statement_case(lex, funcsp, parent);
            parent->nosemi = TRUE;
            if (cparams.prm_cplusplus)
                HandleStartOfCase(parent);
            parent->lastcaseordefault = TRUE;
            break;
        case kw_default:
            if (cparams.prm_cplusplus)
                HandleEndOfCase(parent);
            lex = statement_default(lex, funcsp, parent);
            parent->nosemi = TRUE;
            if (cparams.prm_cplusplus)
                HandleStartOfCase(parent);
            parent->lastcaseordefault = TRUE;
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
        default:
            if ((startOfType(lex, FALSE) && (!cparams.prm_cplusplus && !chosenAssembler->msil || resolveToDeclaration(lex)))
                 || MATCHKW(lex, kw_namespace) || MATCHKW(lex, kw_using) || MATCHKW(lex, kw_decltype) || MATCHKW(lex, kw_static_assert) )
            {
                if (!cparams.prm_c99 && !cparams.prm_cplusplus)
                {
                    error(ERR_NO_DECLARATION_HERE);
                }
                if (viacontrol)
                {
                    AllocateLocalContext(parent, funcsp, codeLabel++);
                }
                while ((startOfType(lex, FALSE) && (!cparams.prm_cplusplus && !chosenAssembler->msil || resolveToDeclaration(lex)))
                    || MATCHKW(lex, kw_namespace) || MATCHKW(lex, kw_using) || MATCHKW(lex, kw_decltype) || MATCHKW(lex, kw_static_assert))
                {
                    STATEMENT *current = parent->tail;
                    declareAndInitialize = FALSE;
                    lex = declare(lex, funcsp, NULL, sc_auto, lk_none, parent, FALSE, FALSE, FALSE, FALSE, ac_public);
                    markInitializers(current);
                    if (MATCHKW(lex, semicolon))
                    {
                        parent->hassemi = TRUE;
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
                lex = statement_expr(lex, funcsp, parent);
            }
    }
    if (MATCHKW(lex, semicolon))
    {
        parent->hassemi = TRUE;
        skip(&lex, semicolon);
    }
    else 
        parent->hassemi = FALSE;
    return lex;
}
static BOOLEAN thunkmainret(SYMBOL *funcsp, BLOCKDATA *parent, BOOLEAN always)
{
    if (always || !strcmp(funcsp->name, "main") && !funcsp->parentClass && !funcsp->parentNameSpace)
    {
        STATEMENT *s = stmtNode(NULL, parent, st_return);
        s->select = intNode(en_c_i, 0);
        return TRUE;
    }
    return FALSE;
}
static void thunkThisReturns(STATEMENT *st, EXPRESSION *thisptr)
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
static void insertXCInfo(SYMBOL *funcsp)
{
    char name[2048];
    SYMBOL *sp; 
    makeXCTab(funcsp);
    my_sprintf(name, "@$xc%s", funcsp->decoratedName);
    sp = makeID(sc_global, &stdpointer, NULL, litlate(name));
    sp->linkage = lk_virtual;
    sp->decoratedName = sp->errname = sp->name;
    sp->allocate = TRUE;
    sp->used = sp->assigned = TRUE;
    funcsp->xc->xcInitLab = codeLabel++;
    funcsp->xc->xcDestLab = codeLabel++;
    funcsp->xc->xclab = sp;
    
    sp = namespacesearch("_InitializeException", globalNameSpace, FALSE, FALSE);
    if (sp)
    {
        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
        INITLIST *arg1 = Alloc(sizeof(INITLIST));
        INITLIST *arg2 = Alloc(sizeof(INITLIST));
        EXPRESSION *exp;
        sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
        funcparams->functp = sp->tp;
        funcparams->sp = sp;
        funcparams->fcall = varNode(en_pc, sp);
        funcparams->ascall = TRUE;
        funcparams->arguments = arg1;
        arg1->next = arg2;
        arg1->tp = &stdpointer;


        arg1->exp = varNode(en_auto, funcsp->xc->xctab);
        arg2->tp = &stdpointer;
        arg2->exp = varNode(en_global, funcsp->xc->xclab);
        exp = exprNode(en_func, 0, 0);
        exp->v.func = funcparams;
        funcsp->xc->xcInitializeFunc = exp;
        sp = namespacesearch("_RundownException", globalNameSpace, FALSE, FALSE);
        if (sp)
        {
            sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
            funcparams = Alloc(sizeof(FUNCTIONCALL));
            funcparams->functp = sp->tp;
            funcparams->sp = sp;
            funcparams->fcall = varNode(en_pc, sp);
            funcparams->ascall = TRUE;
            exp = exprNode(en_func, 0, 0);
            exp->v.func = funcparams;
            funcsp->xc->xcRundownFunc = exp;
        }
    }
}
static LEXEME *compound(LEXEME *lex, SYMBOL *funcsp,
                        BLOCKDATA *parent,   
                        BOOLEAN first)
{
    BLOCKDATA *blockstmt = Alloc(sizeof(BLOCKDATA))  ;
    int pragmas = stdpragmas;
    STATEMENT *st;
    int blknum;
    EXPRESSION *thisptr = NULL;
    browse_blockstart(lex->line);
    blockstmt->next = parent;
    blockstmt->type = begin;
    blockstmt->needlabel = parent->needlabel;
    blockstmt->table = localNameSpace->syms;
    currentLineData(blockstmt, lex, 0);
    AllocateLocalContext(blockstmt, funcsp, codeLabel++);
    parent->needlabel = FALSE;
    if (first)
    {
        HASHREC *hr = basetype(funcsp->tp)->syms->table[0];
        int n = 1;
        browse_startfunc(funcsp, funcsp->declline);
        while (hr)
        {
            SYMBOL *sp2 = (SYMBOL *)hr->p;
            if (!cparams.prm_cplusplus && sp2->tp->type != bt_ellipse && !isvoid(sp2->tp) && sp2->anonymous)
                errorarg(ERR_PARAMETER_MUST_HAVE_NAME, n, sp2, funcsp);
            sp2->destructed = FALSE;
            insert(sp2, localNameSpace->syms);
            browse_variable(sp2);
            n++;
            hr = hr->next;
        }
        if (cparams.prm_cplusplus && funcsp->isConstructor && funcsp->parentClass)
        {
            ParseMemberInitializers(funcsp->parentClass, funcsp);
            thisptr = thunkConstructorHead(blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms, TRUE, FALSE);
        }
    }
    lex = getsym(); /* past { */
    
    st = blockstmt->tail;
    if (!cparams.prm_cplusplus)
    {
        // have to defer so we can get expression like constructor calls
        while (startOfType(lex, FALSE))
        {
            STATEMENT *current = blockstmt->tail;
            declareAndInitialize = FALSE;
            lex = declare(lex, funcsp, NULL, sc_auto, lk_none, blockstmt, FALSE, FALSE, FALSE, FALSE, ac_public);
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
    blockstmt->nosemi = TRUE ; /* in case it is an empty body */
    while (lex && !MATCHKW(lex, end))
    {
        if (!blockstmt->hassemi && !blockstmt->nosemi)
            errorint(ERR_NEEDY, ';');
        blockstmt->lastcaseordefault = FALSE;
        lex = statement(lex, funcsp, blockstmt, FALSE);
    }
    if (first)
    {
        browse_endfunc(funcsp, funcsp->endLine = lex?lex->line : endline);
    }
    if (!lex)
    {
        needkw(&lex, end);
        return lex;
    }
    browse_blockend(endline = lex->line);
    currentLineData(blockstmt, lex, -!first);
    if (parent->type == begin || parent->type == kw_switch || parent->type == kw_try || parent->type == kw_catch)
        parent->needlabel = blockstmt->needlabel;
    if (!blockstmt->hassemi && (!blockstmt->nosemi || blockstmt->lastcaseordefault))
    {
        errorint(ERR_NEEDY, ';');
    }
    st = blockstmt->head;
    if (st)
    {
        BOOLEAN hasvla = FALSE;
        while (st)
        {
            hasvla |= st->hasvla;
            st = st->next;
        }
        if (hasvla)
        {
            if (first)
            {
                funcsp->allocaUsed = TRUE;
            }
        }
    }
    if (first && cparams.prm_cplusplus)
    {
        if (functionCanThrow)
        {
            if (funcsp->xcMode == xc_none || funcsp->xcMode == xc_dynamic)
            {
                hasXCInfo = TRUE;
            }
        }
        else if (funcsp->xcMode == xc_dynamic && funcsp->xc && funcsp->xc->xcDynamic)
            hasXCInfo = TRUE;
        if (hasXCInfo && cparams.prm_xcept)
        {
            funcsp->noinline = TRUE;
            insertXCInfo(funcsp);
        }
        if (!strcmp(funcsp->name, overloadNameTab[CI_DESTRUCTOR]))
            thunkDestructorTail(blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms);
    }
    if (cparams.prm_cplusplus)
        HandleEndOfSwitchBlock(blockstmt);
    FreeLocalContext(blockstmt, funcsp, codeLabel++);
    if (first && !blockstmt->needlabel && !isvoid(basetype(funcsp->tp)->btp) && basetype(funcsp->tp)->btp->type != bt_auto && !funcsp->isConstructor)
    {
        if (funcsp->linkage3 == lk_noreturn)
            error(ERR_NORETURN);
        else if (cparams.prm_c99 || cparams.prm_cplusplus)
        {
            if (!thunkmainret(funcsp, blockstmt, FALSE))
            {
                if (isref(basetype(funcsp->tp)->btp))
                    error(ERR_FUNCTION_RETURNING_REF_SHOULD_RETURN_VALUE);
                else
                {
                    error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
                    if (chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                        thunkmainret(funcsp, blockstmt, TRUE);
                }
            }
        }
        else
        {
            error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
            if (chosenAssembler->arch->preferopts & OPT_THUNKRETVAL)
                thunkmainret(funcsp, blockstmt, TRUE);
        }
    }
    needkw(&lex, end);
    if (first && cparams.prm_cplusplus)
    {
        if (funcsp->hasTry)
        {
            STATEMENT *last = stmtNode(NULL, blockstmt, st_return);
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
            hasXCInfo = TRUE;
        }
    }
    if (parent->type == kw_catch)
    {
        SYMBOL *sp = namespacesearch("_CatchCleanup", globalNameSpace, FALSE, FALSE);
        if (sp)
        {
            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
            STATEMENT *st = stmtNode(lex, blockstmt, st_expr);
            INITLIST *arg1 = Alloc(sizeof(INITLIST)); // exception table
            makeXCTab(funcsp);
            sp = (SYMBOL *)basetype(sp->tp)->syms->table[0]->p;
            funcparams->ascall = TRUE;
            funcparams->sp = sp;
            funcparams->functp = sp->tp;
            funcparams->fcall = varNode(en_pc, sp);
            funcparams->arguments = arg1;
            arg1->exp = varNode(en_auto, funcsp->xc->xctab);
            arg1->tp = &stdpointer;
            st->select = exprNode(en_func, NULL, NULL);
            st->select->v.func = funcparams;
        }
    }
    if (first)
    {
        if (thisptr)
        {
            stmtNode(NULL, blockstmt, st_return);
            thunkThisReturns(blockstmt->head, thisptr);
        }
    }
    AddBlock(lex, parent, blockstmt);
    stdpragmas = pragmas;
    return lex;
}
void assignParam(SYMBOL *funcsp, int *base, SYMBOL *param)
{
    TYPE *tp = basetype(param->tp);
    param->parent = funcsp;
    if (tp->type == bt_void)
        return;
    if (isstructured(tp) && !basetype(tp)->sp->pureDest)
        hasXCInfo = TRUE;
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
static void assignCParams(LEXEME *lex, SYMBOL *funcsp, int *base, HASHREC *params, TYPE *rv, BLOCKDATA *block)
{
    while (params)
    {
        STATEMENT *s = stmtNode(lex, block, st_varstart);
        s->select = varNode(en_auto, (SYMBOL *)params->p);
        assignParam(funcsp, base, (SYMBOL *)params->p);
        params = params->next;
    }
}
static void assignPascalParams(LEXEME *lex, SYMBOL *funcsp, int *base, HASHREC *params, TYPE *rv, BLOCKDATA *block)
{
    if (params)
    {
        STATEMENT *s;
        if (params->next)
            assignPascalParams(lex, funcsp, base, params->next, rv, block);
        assignParam(funcsp, base, (SYMBOL *)params->p);
        s = stmtNode(lex, block, st_varstart);
        s->select = varNode(en_auto, (SYMBOL *)params->p);
    }
}
static void assignParameterSizes(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *block)
{
    HASHREC *params = basetype(funcsp->tp)->syms->table[0];
    int base;
    if (chosenAssembler->arch->denyopts & DO_NOPARMADJSIZE)
        base = 0;
    else
        base = chosenAssembler->arch->retblocksize;
    if (funcsp->linkage == lk_pascal)
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
                if (funcsp->linkage2 == lk_unmanaged || ! chosenAssembler->msil || !chosenAssembler->msil->managed(funcsp))
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
            assignParam(funcsp, &base, (SYMBOL *)params->p);
            params = params->next;
        }
        assignCParams(lex, funcsp, &base, params, basetype(funcsp->tp)->btp, block);
    }
    funcsp->paramsize = base - chosenAssembler->arch->retblocksize;
}
static void checkUndefinedStructures(SYMBOL *funcsp)
{
    HASHREC *hr;
    TYPE *tp = basetype(funcsp->tp)->btp;
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
        SYMBOL *sp = (SYMBOL *)hr->p;
        TYPE *tp = basetype(sp->tp);
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
static int inlineStatementCount(STATEMENT *block)
{
    int rv = 0;
    while (block != NULL)
    {
        switch (block->type)
        {
            case st__genword:
                break;
            case st_try:
            case st_catch:
                rv += 1000;
                break;
            case st_return:
            case st_expr:
            case st_declare:
                rv ++;
                break;
            case st_goto:
            case st_label:
                rv++;
                break;
            case st_select:
            case st_notselect:
                rv ++;
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
static void handleInlines(SYMBOL *funcsp)
{
    /* so it will get instantiated as a virtual function */
    if (!funcsp->isInline)
        return;
    if (cparams.prm_c99)
        funcsp->used = TRUE;
    /* this unqualified the current function if it has structured
     * args or return value, or if it has nested declarations
     */
    {
        if (funcsp->inlineFunc.syms)
        {
            HASHTABLE *ht = funcsp->inlineFunc.syms->next; /* past params */
            if (ht)
                ht = ht->next; /* past first level */
            /* if any vars declared at another level */
            while (ht && ht->next)
            {
                if (ht->table[0])
                {
                    funcsp->noinline = TRUE;
                    break;
                }
                ht = ht->next;
            }
            if (funcsp->inlineFunc.syms->next)
            {
                if (funcsp->inlineFunc.syms->next->table[0])
                    funcsp->noinline = TRUE;
            }
        }
        if (funcsp->linkage == lk_virtual)
        {
            
            HASHREC *hr = basetype(funcsp->tp)->syms->table[0];
            SYMBOL *head;
            while (hr)
            {
                head = (SYMBOL *)hr->p;
                if (isstructured(head->tp))
                {
                    funcsp->noinline = TRUE;
                    break;
                }
                hr = hr->next;
            }
        }
        if (inlineStatementCount(funcsp->inlineFunc.stmt) > 100)
        {
            funcsp->noinline = TRUE;
        }
    }
}
LEXEME *body(LEXEME *lex, SYMBOL *funcsp)
{
    int n1;
    BOOLEAN oldsetjmp_used = setjmp_used;
    BOOLEAN oldfunctionHasAssembly = functionHasAssembly;
    BOOLEAN oldDeclareAndInitialize = declareAndInitialize;
    BOOLEAN oldHasXCInfo = hasXCInfo;
    BOOLEAN oldFunctionCanThrow = functionCanThrow;
    HASHTABLE *oldSyms = localNameSpace->syms;
    HASHTABLE *oldLabelSyms = labelSyms;
    SYMBOL *oldtheCurrentFunc = theCurrentFunc;
    BLOCKDATA *block = Alloc(sizeof(BLOCKDATA)) ;
    STATEMENT *startStmt;
    SYMBOL *spt = funcsp;
    int oldCodeLabel = codeLabel;
    int oldMatchReturnTypes = matchReturnTypes;
    funcNesting++;
    functionCanThrow = FALSE;
    codeLabel = INT_MIN;
    hasXCInfo = FALSE;
    localNameSpace->syms = NULL;
    functionHasAssembly = FALSE;
    setjmp_used = FALSE;
    declareAndInitialize = FALSE;
    block->type = funcsp->hasTry ? kw_try : begin;
    theCurrentFunc = funcsp;

    checkUndefinedStructures(funcsp);
    FlushLineData(funcsp->declfile, funcsp->declline);
    if (!funcsp->linedata)
    {
        startStmt = currentLineData(NULL, lex, 0);
        if (startStmt)
            funcsp->linedata = startStmt->lineData;
    }
    funcsp->declaring = TRUE;
    labelSyms = CreateHashTable(1);
    assignParameterSizes(lex, funcsp, block);
    funcsp->startLine = lex->line;
    lex = compound(lex, funcsp, block, TRUE);
    checkUnlabeledReferences(block);
    checkGotoPastVLA(block->head, TRUE);
    if (isautotype(basetype(funcsp->tp)->btp) && !templateNestingCount)
        basetype(funcsp->tp)->btp = &stdvoid; // return value for auto function without return statements
    if (cparams.prm_cplusplus)
        if (!funcsp->constexpression)
            funcsp->nonConstVariableUsed = TRUE;
        else
            funcsp->nonConstVariableUsed |= !MatchesConstFunction(funcsp);
    funcsp->labelCount = codeLabel - INT_MIN;
    n1 = codeLabel;
    if (!funcsp->templateLevel || funcsp->instantiated || funcsp->instantiated2)
    {
        funcsp->inlineFunc.stmt = stmtNode(lex, NULL, st_block);
        funcsp->inlineFunc.stmt->lower = block->head;
        funcsp->inlineFunc.stmt->blockTail = block->blockTail;
        funcsp->declaring = FALSE;
        if (funcsp->isInline && (functionHasAssembly || funcsp->linkage2 == lk_export))
            funcsp->isInline = funcsp->dumpInlineToFile = funcsp->promotedToInline = FALSE;
        // if it is variadic don't allow it to be inline
        if (funcsp->isInline)
        {
            HASHREC *hr = basetype(funcsp->tp)->syms->table[0];
            if (hr)
            {
                while (hr->next)
                    hr = hr->next;
                if (((SYMBOL *)hr->p)->tp->type == bt_ellipse)
                    funcsp->isInline = funcsp->dumpInlineToFile = funcsp->promotedToInline = FALSE;
            }
        }
        if (funcsp->linkage == lk_virtual || funcsp->isInline)
        {
            InsertInline(funcsp);
            if (!cparams.prm_cplusplus && funcsp->storage_class != sc_static)
                GENREF(funcsp);
        }
    #ifndef PARSER_ONLY
        else 
        {
            BOOLEAN isTemplate = FALSE;
            SYMBOL *spt = funcsp;
            while (spt && !isTemplate)
            {
                if (spt->templateLevel)
                    isTemplate = TRUE;
                else
                    spt = spt->parentClass;
            }
            
            if (!isTemplate)
            {
                if (!total_errors)
                {
                    int oldstartlab = startlab;
                    int oldretlab = retlab;
                    startlab = nextLabel++;
                    retlab = nextLabel++;
                    genfunc(funcsp, TRUE);
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
    declareAndInitialize = oldDeclareAndInitialize;
    theCurrentFunc = oldtheCurrentFunc;
    hasXCInfo = oldHasXCInfo;
    setjmp_used = oldsetjmp_used;
    functionHasAssembly = oldfunctionHasAssembly;
    localNameSpace->syms = oldSyms;
    labelSyms = oldLabelSyms;
    codeLabel = oldCodeLabel;
    functionCanThrow = oldFunctionCanThrow;
    matchReturnTypes = oldMatchReturnTypes;
    funcNesting--;
    return lex;
}
