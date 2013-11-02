/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modificatsion, are permitted provided that the following 
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
/* declare in select has multiple vars */
#include "compiler.h"
#include <assert.h>
extern ARCH_DEBUG *chosenDebugger;
extern ARCH_ASM *chosenAssembler;
extern TYPE stdint, stdvoid;
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
extern int endline;
extern char *overloadNameTab[];
extern LEXCONTEXT *context;
extern TYPE stdXC;

BOOL hasXCInfo;
int startlab, retlab;
int nextLabel;
BOOL setjmp_used;
BOOL functionHasAssembly;
BOOL declareAndInitialize;

static LINEDATA *linesHead, *linesTail;
static LEXEME *autodeclare(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, 
                           BLOCKDATA *parent, BOOL asExpression);
static BOOL resolveToDeclaration(LEXEME *lex);
static LEXEME *statement(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent, 
                           BOOL viacontrol);
static LEXEME *compound(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent,   
                        BOOL first);
void statement_ini()
{
    nextLabel = 1;
    linesHead = linesTail = NULL;
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
        if (strcmp(file, linesHead->file) || linesHead->lineno < lineno)
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
    while (*p && (strcmp((*p)->file, file) || lineno >= (*p)->lineno))
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
        lex = context->cur;
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
static BOOL isselecttrue(EXPRESSION *exp)
{
    if (isintconst(exp))
        return !!exp->v.i;
    return FALSE;
}
static BOOL isselectfalse(EXPRESSION *exp)
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
static LEXEME *selection_expression(LEXEME *lex, BLOCKDATA *parent, EXPRESSION **exp, SYMBOL *funcsp, enum e_kw kw, BOOL *declaration)
{
    TYPE *tp = NULL;
    (void)parent;
    if (startOfType(lex) && (!cparams.prm_cplusplus || resolveToDeclaration(lex)))
    {
        if (declaration)
            *declaration = TRUE;
        if ((cparams.prm_cplusplus && kw != kw_do || cparams.prm_c99 && kw == kw_for)
            && kw != kw_else)
        {
            // empty
        }
        else
        {
            error(ERR_NO_DECLARATION_HERE);
        }
        /* fixme need type */
        lex = autodeclare(lex, funcsp, &tp, exp, NULL, kw != kw_for);
        if (tp->type == bt_memberptr)
        {
            *exp = exprNode(en_mp_as_bool, *exp, NULL);
            (*exp)->size = tp->size;
            tp = &stdint;
        }
    }
    else
    {
/*		BOOL openparen = MATCHKW(lex, openpa); */
        if (declaration)
            *declaration = FALSE;
        lex = expression(lex, funcsp, NULL, &tp, exp, kw != kw_for, FALSE);
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
        
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    else if (kw == kw_switch && !isint(tp))
        error(ERR_SWITCH_SELECTION_INTEGRAL);
    else if (kw != kw_for && isstructured(tp))
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
static void thunkCatchCleanup(STATEMENT *st, BLOCKDATA *src, BLOCKDATA *dest)
{
    BLOCKDATA *top = dest ? getCommonParent(src, dest) : NULL, *srch = src;
    while (srch != top)
    {
        if (srch->type == kw_catch)
        {
            SYMBOL *sp = gsearch("_CatchCleanup");
            if (sp)
            {
                FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                STATEMENT **find = &src->head;
                sp = basetype(sp->tp)->syms->table[0]->p;
                funcparams->ascall = TRUE;
                funcparams->sp = sp;
                funcparams->functp = sp->tp;
                funcparams->fcall = varNode(en_pc, sp);
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
static void thunkRetDestructors(EXPRESSION **exp, HASHTABLE *top, HASHTABLE *syms)
{
    if (syms)
    {
        if (syms != top)
            thunkRetDestructors(exp, top, syms->chain);
        destructBlock(exp, syms->table[0]);
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
static LEXEME *statement_break(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *breakableStatement = parent;
    EXPRESSION *exp = NULL;
    (void)lex;
    (void)funcsp;
    (void)parent;
    while (breakableStatement && (breakableStatement->type == begin ||breakableStatement->type == kw_try ||breakableStatement->type == kw_catch))
    {
        breakableStatement = breakableStatement->next;
    }
    
    if (!breakableStatement)
        error(ERR_BREAK_NO_LOOP);
    else
    {
        STATEMENT *st ;
        currentLineData(parent, lex, 0);
        thunkRetDestructors(&exp, breakableStatement->table, localNameSpace->syms);
        st = stmtNode(lex, parent, st_goto);
        st->label = breakableStatement->breaklabel;
        st->destexp = exp;
        thunkCatchCleanup(st, parent, breakableStatement);
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
                sprintf(buf, "%d", val);
                preverror(ERR_DUPLICATE_CASE, buf, (*cases)->file, (*cases)->line);
                break;
            }
            cases = &(*cases)->next;
        }
        if (!*cases)
        {
            STATEMENT *st = stmtNode(lex, parent, st_label);
            st->label = nextLabel++;
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
    EXPRESSION *exp = NULL;
    (void)lex;
    (void)funcsp;
    while (continuableStatement && (continuableStatement->type == kw_switch || continuableStatement->type == begin || continuableStatement->type == kw_try || continuableStatement->type == kw_catch))
    {
        continuableStatement = continuableStatement->next;
    }
    if (!continuableStatement)
        error(ERR_CONTINUE_NO_LOOP);
    else
    {
        STATEMENT *st;
        thunkRetDestructors(&exp, continuableStatement->table, localNameSpace->syms);
        currentLineData(parent, lex, 0);
        st = stmtNode(lex, parent, st_goto);
        st->label = continuableStatement->continuelabel;		
        st->destexp = exp;
        thunkCatchCleanup(st, parent, continuableStatement);
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
        st->label = nextLabel++;
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
    STATEMENT *st;
    EXPRESSION *select = NULL;
    int addedBlock = 0;
    int loopLabel = nextLabel++;
    lex = getsym();
    dostmt->breaklabel = beGetLabel;
    dostmt->continuelabel = beGetLabel;
    dostmt->next = parent;
    dostmt->type = kw_do;
    dostmt->table = localNameSpace->syms;
    currentLineData(dostmt, lex, 0);
    st = stmtNode(lex, dostmt, st_label);
    st->label = loopLabel;
    if (cparams.prm_cplusplus || cparams.prm_c99)
    {
        addedBlock++;
        AllocateLocalContext(parent, funcsp);
    }
    lex = statement(lex, funcsp, dostmt,TRUE);
    parent->nosemi = FALSE;
    if (MATCHKW(lex, kw_while))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp);
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
        FreeLocalContext(dostmt, funcsp);
    AddBlock(lex, parent, dostmt);
    return lex;
}
static LEXEME *statement_for(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *forstmt = Alloc(sizeof(BLOCKDATA)) ;
    STATEMENT *st;
    STATEMENT *forline ;
    int addedBlock = 0;
    EXPRESSION *init = NULL, *before = NULL, *select = NULL;
    int loopLabel = nextLabel++, testlabel = nextLabel++;
    forstmt->breaklabel = beGetLabel;
    forstmt->continuelabel = beGetLabel;
    forstmt->next = parent;
    forstmt->type = kw_for;
    forstmt->table = localNameSpace->syms;
    currentLineData(forstmt,lex,-1);
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        BOOL declaration = FALSE;
        lex = getsym();
        if (!MATCHKW(lex, semicolon))
        {
            if (cparams.prm_cplusplus && !cparams.prm_oldfor || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp);
            }
        
            lex = selection_expression(lex, forstmt, &init, funcsp, kw_for, &declaration);
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
                        if (isstructured(declSP->init))
                        {
                            error(ERR_FORRANGE_DECLARATOR_NO_INIT);
                        }
                        else
                        {
                            error(ERR_FORRANGE_DECLARATOR_NO_INIT);
                        }
                    }
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
                    lex = expression_no_comma(lex, funcsp, NULL, &selectTP, &select, FALSE);
                }
                if (!selectTP)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                {
                    EXPRESSION *ibegin=NULL, *iend=NULL;
                    TYPE *iteratorType = NULL;
                    TYPE *tpref = Alloc(sizeof(TYPE));
                    SYMBOL *rangeSP = anonymousVar(sc_auto, tpref);
                    EXPRESSION *rangeExp = varNode(en_auto, rangeSP);
                    deref(&stdpointer, &rangeExp);
                    needkw(&lex, closepa);
                    forline = currentLineData(NULL, lex, 0);
                    while (castvalue(select))
                        select = select->left;
                    if (lvalue(select))
                        select = select->left;
                    tpref->size = getSize(bt_pointer);
                    tpref->type = bt_rref;
                    tpref->btp = selectTP;
                    insert(rangeSP, localNameSpace->syms);
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
                        memset(&thisTP, NULL, sizeof(thisTP));
                        thisTP.type = bt_pointer;
                        thisTP.size - getSize(bt_pointer);
                        thisTP.btp = rangeSP->tp->btp;
                        ibegin = search("begin", basetype(selectTP)->syms);
                        iend = search("end", basetype(selectTP)->syms);
                        if (ibegin && iend)
                        {
                            SYMBOL *beginFunc =NULL, *endFunc = NULL;
                            FUNCTIONCALL fcb, fce;
                            TYPE *ctp;
                            memset(&fcb, 0, sizeof(fcb));
                            fcb.thistp = &thisTP;
                            fcb.thisptr = rangeExp;
                            ctp = rangeSP->tp;
                            beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, ibegin, &fcb, NULL, FALSE, FALSE);
                            memset(&fce, 0, sizeof(fce));
                            fce.thistp = &thisTP;
                            fce.thisptr = rangeExp;
                            ctp = rangeSP->tp;
                            endFunc = GetOverloadedFunction(&ctp, &fce.fcall, iend, &fce, NULL, FALSE, FALSE);
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
                                        fcb.returnSP = anonymousVar(sc_auto, iteratorType);
                                        fcb.returnEXP = varNode(en_auto, fcb.returnSP);
                                        insert(fcb.returnSP, localNameSpace->syms);
                                        exp = fcb.returnEXP;
                                        dest = NULL;
                                        callDestructor(fcb.returnSP, &exp, NULL, TRUE, FALSE);
                                        initInsert(&dest, iteratorType, exp, 0, TRUE);
                                        fcb.returnSP->dest = dest;
                                       
                                        fce.returnSP = anonymousVar(sc_auto, iteratorType);
                                        fce.returnEXP = varNode(en_auto, fce.returnSP);
                                        insert(fce.returnSP, localNameSpace->syms);
                                        exp = fce.returnEXP;
                                        dest = NULL;
                                        callDestructor(fce.returnSP, &exp, NULL, TRUE, FALSE);
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
                            ibegin = namespacesearch("begin", globalNameSpace, FALSE, FALSE);
                            iend = namespacesearch("end", globalNameSpace, FALSE, FALSE);
                            // now possibly lookup in namespace std
                            if (!ibegin || !iend)
                            {
                                SYMBOL *standard = namespacesearch("std", globalNameSpace, FALSE, FALSE);
                                if (standard)
                                {
                                    ibegin = namespacesearch("begin", standard->nameSpaceValues, FALSE, FALSE);
                                    iend = namespacesearch("end", standard->nameSpaceValues, FALSE, FALSE);
                                }
                            }
                            if (!ibegin || !iend)
                            {
                                if (rangeSP->tp->btp->sp->parentNameSpace)
                                {
                                    ibegin = namespacesearch("begin", rangeSP->tp->btp->sp->parentNameSpace->nameSpaceValues, FALSE, FALSE);
                                    iend = namespacesearch("end", rangeSP->tp->btp->sp->parentNameSpace->nameSpaceValues, FALSE, FALSE);                                    
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
                                ctp = rangeSP->tp;
                                beginFunc = GetOverloadedFunction(&ctp, &fcb.fcall, ibegin, &fcb, NULL, FALSE, FALSE);
                                memset(&fce, 0, sizeof(fce));
                                fce.arguments = &args;
                                ctp = rangeSP->tp;
                                endFunc = GetOverloadedFunction(&ctp, &fce.fcall, iend, &fce, NULL, FALSE, FALSE);
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
                                            fcb.returnSP = anonymousVar(sc_auto, iteratorType);
                                            fcb.returnEXP = varNode(en_auto, fcb.returnSP);
                                            insert(fcb.returnSP, localNameSpace->syms);
                                            exp = fcb.returnEXP;
                                            dest = NULL;
                                            callDestructor(fcb.returnSP, &exp, NULL, TRUE, FALSE);
                                            initInsert(&dest, iteratorType, exp, 0, TRUE);
                                            fcb.returnSP->dest = dest;
                                            
                                            fce.returnSP = anonymousVar(sc_auto, iteratorType);
                                            fce.returnEXP = varNode(en_auto, fce.returnSP);
                                            insert(fce.returnSP, localNameSpace->syms);
                                            exp = fce.returnEXP;
                                            dest = NULL;
                                            callDestructor(fce.returnSP, &exp, NULL, TRUE, FALSE);
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
                                            SYMBOL *esp = anonymousVar(sc_auto, basetype(rangeSP->tp)->btp); // sc_parameter to push it...
                                            EXPRESSION *consexp = varNode(en_auto, esp);
                                            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                            TYPE *ctype = basetype(rangeSP->tp)->btp;
                                            esp->stackblock = TRUE;
                                            funcparams->arguments = Alloc(sizeof(INITLIST));
                                            *funcparams->arguments = *fc->arguments;
                                            callConstructor(&ctype, &consexp, funcparams, FALSE, 0,TRUE, FALSE, FALSE);
                                            fc->arguments->exp = consexp;                                                             
                                        }
                                        else
                                        {
                                            fc->arguments->tp = Alloc(sizeof(TYPE));
                                            fc->arguments->tp->type = bt_lref;
                                            fc->arguments->tp->size = getSize(bt_pointer);
                                            fc->arguments->tp->btp = fcb.arguments->tp;
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
                                            SYMBOL *esp = anonymousVar(sc_auto, basetype(rangeSP->tp)->btp); // sc_parameter to push it...
                                            EXPRESSION *consexp = varNode(en_auto, esp);
                                            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                                            TYPE *ctype = basetype(rangeSP->tp)->btp;
                                            esp->stackblock = TRUE;
                                            funcparams->arguments = Alloc(sizeof(INITLIST));
                                            *funcparams->arguments = *fc->arguments;
                                            callConstructor(&ctype, &consexp, funcparams, FALSE, 0,TRUE, FALSE, FALSE);
                                            fc->arguments->exp = consexp;
                                        }
                                        else
                                        {
                                            fc->arguments->tp = Alloc(sizeof(TYPE));
                                            fc->arguments->tp->type = bt_lref;
                                            fc->arguments->tp->size = getSize(bt_pointer);
                                            fc->arguments->tp->btp = fce.arguments->tp;
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
                            SYMBOL *sBegin = anonymousVar(sc_auto, iteratorType);
                            SYMBOL *sEnd = anonymousVar(sc_auto, iteratorType);
                            eBegin = varNode(en_auto, sBegin);
                            eEnd = varNode(en_auto, sEnd);
                            deref(&stdpointer, &eBegin);
                            deref(&stdpointer, &eEnd);
                            insert(sBegin, localNameSpace->syms);
                            insert(sEnd, localNameSpace->syms);
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
                                                   funcsp, &eqType, &compare, iteratorType, eEnd, NULL, FALSE ))
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

                        AllocateLocalContext(parent, funcsp);
                        
                        // initialize var here
                        st = stmtNode(lex, forstmt, st_expr);
                        if (!isstructured(selectTP))
                        {
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
                                callConstructor(&ctype, &decl,funcparams, FALSE, 0, TRUE, FALSE, FALSE);
                                st->select = decl;
                                declDest = declExp;
                                callDestructor(declSP, &declDest, NULL, TRUE, FALSE);
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
                                    callConstructor(&ctype, &decl,funcparams, FALSE, 0, TRUE, FALSE, FALSE);
                                    st->select = decl;
                                    declDest = declExp;
                                    callDestructor(declSP, &declDest, NULL, TRUE, FALSE);
                                }
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, star,
                                                   funcsp, &starType, &st->select, NULL,NULL, NULL, FALSE))
                            {
                                error(ERR_MISSING_OPERATOR_STAR_FORRANGE_ITERATOR);
                                
                            }
                            else if (!comparetypes(declSP->tp, starType, TRUE))
                            {
                                error(ERR_OPERATOR_STAR_FORRANGE_WRONG_TYPE);
                            }
                            else if (!isstructured(declSP->tp))
                            {
                                EXPRESSION *decl = declExp;
                                deref(declSP->tp, &decl);
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
                                args->exp = st->select;
                                callConstructor(&ctype, &decl,funcparams, FALSE, 0, TRUE, FALSE, FALSE);
                                st->select = decl;
                                declDest = declExp;
                                callDestructor(declSP, &declDest, NULL, TRUE, FALSE);
                            }
                        }
                        
                        lex = statement(lex, funcsp, forstmt, TRUE);
                        FreeLocalContext(forstmt, funcsp);
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
                                st->select = exprNode(en_assign, eBegin, exprNode(en_add, eBegin, intNode(en_c_i, basetype(selectTP)->btp->size)));
                        }
                        else
                        {
                            EXPRESSION *ppType = iteratorType;
                            st->select = eBegin;
                            if (ispointer(iteratorType))
                            {
                                st->select = exprNode(en_assign, eBegin, exprNode(en_add, eBegin, intNode(en_c_i, basetype(iteratorType)->btp->size)));
                            }
                            else if (!insertOperatorFunc(ovcl_unary_prefix, autoinc,
                                                   funcsp, &ppType, &st->select, NULL,NULL, NULL, FALSE))
                            {
                                error(ERR_MISSING_OPERATOR_PLUSPLUS_FORRANGE_ITERATOR);
                            }
                            else
                            {
                                if (isstructured(ppType))
                                {
                                    st->select->v.func->returnSP = anonymousVar(sc_auto, ppType);
                                    st->select->v.func->returnEXP = varNode(en_auto, st->select->v.func->returnSP);
                                    insert(st->select->v.func->returnSP, localNameSpace->syms);
                                    declDest = st->select->v.func->returnEXP;
                                    callDestructor(st->select->v.func->returnSP, &declDest, NULL, TRUE, FALSE);
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
                    FreeLocalContext( forstmt, funcsp);
                AddBlock(lex, parent, forstmt);
                return lex;
            }
            else
            {
                if (declaration)
                {
                    SYMBOL *declSP = (SYMBOL *)localNameSpace->syms->table[0]->p;
                    if (!declSP->init)
                    {
                        error(ERR_FOR_DECLARATOR_MUST_INITIALIZE);                        
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
                    forline = currentLineData(NULL, lex, 0);
                    lex = getsym();
                    if (init)
                    {
                        st = stmtNode(lex, forstmt, st_expr);
                        st->select = init;
                    }
//					st = stmtNode(lex, forstmt, st_goto);
//					st->label = testlabel;
                    st = stmtNode(lex, forstmt, st_notselect);
                    st->label = forstmt->breaklabel;
                    st->altlabel = testlabel;
                    st->select = select;
                        
                    st = stmtNode(lex, forstmt, st_label);
                    st->label = loopLabel;
                    if (cparams.prm_cplusplus || cparams.prm_c99)
                    {
                        addedBlock++;
                        AllocateLocalContext(parent, funcsp);
                    }
                    lex = statement(lex, funcsp, forstmt, TRUE);
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
        FreeLocalContext(forstmt, funcsp);
    AddBlock(lex, parent, forstmt);
    return lex;
}
static LEXEME *statement_if(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    STATEMENT *st, *st1, *st2 ;
    EXPRESSION *select=NULL;
    int addedBlock = 0;
    BOOL needlabelif;
    BOOL needlabelelse = FALSE;
    int ifbranch = nextLabel++;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp);
        }
        lex = selection_expression(lex, parent, &select, funcsp, kw_if, NULL);
        if (MATCHKW(lex, closepa))
        {
            BOOL optimized = FALSE;
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
                AllocateLocalContext(parent, funcsp);
            }
            lex = statement(lex, funcsp, parent, TRUE);
            needlabelif = parent->needlabel;
            if (MATCHKW(lex, kw_else))
            {
                int elsebr = nextLabel++;
                if (cparams.prm_cplusplus || cparams.prm_c99)
                {
                    FreeLocalContext(parent, funcsp);
                    AllocateLocalContext(parent, funcsp);
                }
                st = stmtNode(lex, parent, st_goto);
                st->label = elsebr;
                if (cparams.prm_optimize)
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
                lex = statement(lex, funcsp, parent, TRUE);
                if (cparams.prm_optimize && !optimized)
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
                st = stmtNode(lex, parent, st_label);
                st->label = elsebr;
            }
            else
            {
                if (cparams.prm_optimize)
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
        FreeLocalContext(parent, funcsp);
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
            spx->declfile = lex->file;
            spx->declline = lex->line;
            spx->declfilenum = lex->filenum;
            SetLinkerNames(spx, lk_none);
            spx->offset = nextLabel++;
            spx->gotoTable = st;
            insert(spx, labelSyms);
        }
        else
        {
            thunkGotoDestructors(&st->destexp, block, spx->gotoTable->parent);
            thunkCatchCleanup(st, block, spx->gotoTable->parent);
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
            thunkGotoDestructors(&spx->gotoTable->destexp, spx->gotoTable->parent, parent);
            thunkCatchCleanup(spx->gotoTable, spx->gotoTable->parent, parent);
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
        spx->offset = nextLabel++;
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
            error(ERR_RETURN_MUST_RETURN_VALUE);
        }
    }
    else
    {   
        tp = basetype(funcsp->tp)->btp;
        if (cparams.prm_cplusplus && tp->type == bt_auto & ISID(lex))
        {
            SYMBOL *sp = NULL;
            SYMBOL *strSym = NULL;
            NAMESPACEVALUES *nsv = NULL;
            marksym();
            lex = id_expression(lex, funcsp, &sp, &strSym, &nsv, FALSE, FALSE, lex->value.s.a);
            lex = backupsym(0);
            if (sp && isstructured(sp->tp))
                tp = sp->tp;
        }
        if (isstructured(tp) || basetype(tp)->type == bt_memberptr)
        {
            SYMBOL *sp = anonymousVar(sc_parameter, &stdpointer);
            EXPRESSION *en = varNode(en_auto, sp);
            BOOL maybeConversion = TRUE;
            sp->offset = chosenAssembler->arch->retblocksize;
            if ((funcsp->linkage == lk_pascal) &&
                    basetype(funcsp->tp)->syms->table[0] && 
                    ((SYMBOL *)basetype(funcsp->tp)->syms->table[0])->tp->type != bt_void)
                sp->offset = funcsp->paramsize;
            deref(&stdpointer, &en);
            if (cparams.prm_cplusplus && isstructured(tp))
            {
                if (MATCHKW(lex, begin))
                {
                    INITIALIZER *init = NULL, *dest=NULL;
                    SYMBOL *sp = NULL;
                    sp = anonymousVar(sc_localstatic, tp);
                    insert(sp, localNameSpace->syms);
                    lex = initType(lex, funcsp, NULL, sc_auto, &init, &dest, tp, sp, FALSE);
                    returnexp = convertInitToExpression(tp, NULL, funcsp, init, en, FALSE);
                    returntype = tp;
                    if (sp)
                        sp->dest = dest;
                }
                else
                {
                    FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
                    TYPE *ctype = tp;
                    if (startOfType(lex))
                    {
                        TYPE *tp1 = NULL;
                        enum e_lk linkage, linkage2, linkage3;
                        BOOL defd = FALSE;
                        lex = getBasicType(lex, funcsp, &tp1, funcsp ? sc_auto : sc_global, &linkage, &linkage2, &linkage3, ac_public, NULL, &defd, NULL);
                        if (!tp1 || !comparetypes(basetype(tp1), basetype(tp), TRUE))
                        {
                            error(ERR_INCOMPATIBLE_TYPE_CONVERSION);
                            errskim(&lex, skim_semi);
                            return lex;
                        }
                        else if (MATCHKW(lex, openpa))
                        {
                            // conversion constructor params
                            lex = getArgs(lex, funcsp, funcparams, closepa);
                        }
                        else
                        {
                            // default constructor without param list
                            errorsym(ERR_IMPROPER_USE_OF_TYPEDEF, basetype(tp)->sp);
                        }
                        returntype = tp1;
                    }
                    else
                    {
                        // shortcut for conversion from single expression
                        EXPRESSION *exp1 = NULL;
                        TYPE *tp1 = NULL;
                        lex = optimized_expression(lex, funcsp, NULL, &tp1, &exp1, FALSE);
                        funcparams->arguments = Alloc(sizeof(INITLIST));
                        funcparams->arguments->tp = tp1;
                        funcparams->arguments->exp = exp1;
                        maybeConversion = FALSE;
                        returntype = tp1;
                    }
                    callConstructor(&ctype, &en, funcparams, FALSE, NULL, TRUE, maybeConversion, FALSE); 
                    returnexp = en;
                }
            
            }
            else
            {
                    
                lex = optimized_expression(lex, funcsp, NULL, &tp, &returnexp, TRUE);
                if (!tp)
                {
                    error(ERR_EXPRESSION_SYNTAX);
                }
                else
                {
                    returnexp = exprNode(en_blockassign, en, returnexp);
                    returnexp->size = basetype(tp)->size;
                    returntype = tp;
                }
            }
        }
        else
        {
            BOOL needend = FALSE;
            if (MATCHKW(lex, begin))
            {
                needend = TRUE;
                lex = getsym();
            }
            lex = optimized_expression(lex, funcsp, NULL, &tp, &returnexp, TRUE);
            if (!tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
            }
            if (needend)
            {
                if (!needkw(&lex, end))
                {
                    errskim(&lex, skim_end);
                    skip(&lex, end);
                }
            }
            returntype = tp;
        }
        if (isref(basetype(funcsp->tp)->btp))
        {
            if (lvalue(returnexp))
            {
                EXPRESSION *exp2 = returnexp;
                if (!isstructured(basetype(funcsp->tp)->btp->btp))
                    returnexp = returnexp->left;
                if (returnexp->type == en_auto)
                {
                    if (returnexp->v.sp->storage_class == sc_auto)
                    {
                        error(ERR_REF_RETURN_LOCAL);
                    }
                    else if (returnexp->v.sp->storage_class == sc_parameter)
                    {
                        returnexp = exp2;
                    }
                }
                else
                {
                    
                     if (referenceTypeError(basetype(funcsp->tp)->btp, exp2) != exp2->type)
                        errortype(ERR_REF_INIT_TYPE_REQUIRES_LVALUE_OF_TYPE, tp, tp);
                }
            }
            else
            {
                if (returnexp->v.sp->storage_class == sc_auto)
                {
                    error(ERR_REF_RETURN_LOCAL);
                }
            }
        }
        else if (returnexp->type == en_auto && 
            returnexp->v.sp->storage_class == sc_auto)
        {
            if (!isstructured(basetype(funcsp->tp)->btp) && 
                basetype(basetype(funcsp->tp)->btp)->type != bt_memberptr)
                error(ERR_FUNCTION_RETURNING_ADDRESS_STACK_VARIABLE);
        }
    }
    currentLineData(parent, lex, 0);
    thunkRetDestructors(&destexp, NULL, localNameSpace->syms);
    st = stmtNode(lex, parent, st_return);
    st->select = returnexp;
    st->destexp = destexp;
    thunkCatchCleanup(st, parent, NULL); // to top level
    // for infering the return type of lambda functions
    if (tp)
    {
        st->returntype = returntype;
    }
    else
    {
        st->returntype = &stdvoid;
    }
    if (returnexp && basetype(funcsp->tp)->btp->type != bt_auto)
    {
        if (tp->type == bt_void)
        {
            if (!cparams.prm_cplusplus || basetype(funcsp->tp)->btp->type != bt_void)
                error(ERR_CANNOT_RETURN_VOID_VALUE);                                                 
        }
        else if (basetype(funcsp->tp)->btp && basetype(funcsp->tp)->btp->type == bt_void)
            error(ERR_RETURN_NO_VALUE);
        else
        {
            if (cparams.prm_cplusplus && (funcsp->isConstructor || funcsp->isDestructor))
            {
                error(ERR_CONSTRUCTOR_HAS_RETURN);
            }
            else if (isstructured(basetype(funcsp->tp)->btp) || isstructured(tp))
            {
                if (!comparetypes(basetype(funcsp->tp)->btp, tp, FALSE))
                    error(ERR_RETMISMATCH);
            }
            else if (basetype(basetype(funcsp->tp)->btp)->type == bt_memberptr || basetype(tp)->type == bt_memberptr)
            {
                if (isconstzero(tp,st->select))
                {
                    int lbl = dumpMemberPtr(NULL, basetype(funcsp->tp)->btp, TRUE);
                    st->select = intNode(en_labcon, lbl);
                }
                else 
                {
                    if (st->select->type == en_memberptr)
                    {
                        int lbl = dumpMemberPtr(st->select->v.sp, basetype(funcsp->tp)->btp, TRUE);
                        st->select = intNode(en_labcon, lbl);
                    }
                    if (!comparetypes(basetype(funcsp->tp)->btp, tp, TRUE))
                        error(ERR_RETMISMATCH);
                }
            }
            else 
            {
                cast(basetype(funcsp->tp)->btp, &st->select);
                if (ispointer(basetype(funcsp->tp)->btp))
                {
                    if (isarithmetic(tp))
                    {
                        if (iscomplex(tp))
                            error(ERR_ILL_USE_OF_COMPLEX);
                        else if (isfloat(tp) || isimaginary(tp))
                            error(ERR_ILL_USE_OF_FLOATING);
                        else if (isarithmeticconst(returnexp))
                        {
                            if (!isintconst(returnexp) || !isconstzero(basetype(funcsp->tp)->btp, returnexp))
                                error(ERR_NONPORTABLE_POINTER_CONVERSION);
                        }
                        else
                            error(ERR_NONPORTABLE_POINTER_CONVERSION);
                    }
                    else if (ispointer(tp))
                    {
                        if (!comparetypes(basetype(funcsp->tp)->btp, tp, TRUE))
                        {
                            if (!isvoidptr(basetype(funcsp->tp)->btp) && !isvoidptr(tp))
                                error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                            else if (cparams.prm_cplusplus && !isvoidptr(basetype(funcsp->tp)->btp) && returnexp->type != en_nullptr && isvoidptr(tp))
                                error(ERR_ANSI_FORBIDS_IMPLICIT_CONVERSION_FROM_VOID);
                        }
                    }
                    else if (isfunction(tp))
                    {
                        if (!isvoidptr(basetype(funcsp->tp)->btp) && 
                            (!isfunction(basetype(basetype(funcsp->tp)->btp)->btp) || !comparetypes(basetype(funcsp->tp)->btp, tp, TRUE)))
                            error(ERR_SUSPICIOUS_POINTER_CONVERSION);
                    }
                    else 
                        error(ERR_INVALID_POINTER_CONVERSION);
                }
                else if (ispointer(tp))
                {
                    if (iscomplex(basetype(funcsp->tp)->btp))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (isfloat(basetype(funcsp->tp)->btp) || isimaginary(basetype(funcsp->tp)->btp))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (isint(basetype(funcsp->tp)->btp))
                        error(ERR_NONPORTABLE_POINTER_CONVERSION);
                }
                else if (ispointer(tp))
                {
                    if (iscomplex(basetype(funcsp->tp)->btp))
                        error(ERR_ILL_USE_OF_COMPLEX);
                    else if (isfloat(basetype(funcsp->tp)->btp) || isimaginary(basetype(funcsp->tp)->btp))
                        error(ERR_ILL_USE_OF_FLOATING);
                    else if (isint(basetype(funcsp->tp)->btp))
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
    switchstmt->breaklabel = beGetLabel;
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
            AllocateLocalContext(parent, funcsp);
        }
        lex = selection_expression(lex, switchstmt, &select, funcsp, kw_switch, NULL);
        if (MATCHKW(lex, closepa))
        {
            STATEMENT *st1;
            lex = getsym();
            currentLineData(switchstmt, lex, 0);
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
                st->label = nextLabel;
                st = stmtNode(lex, switchstmt, st_label);
                st->label = nextLabel++;
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
        FreeLocalContext(switchstmt, funcsp);
    AddBlock(lex, parent, switchstmt);
    return lex;
}
static LEXEME *statement_while(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *whilestmt = Alloc(sizeof(BLOCKDATA))  ;
    STATEMENT *st;
    STATEMENT *whileline;
    EXPRESSION *select = NULL;
    BOOL addedBlock = FALSE;
    int loopLabel = nextLabel++;
    whilestmt->breaklabel = beGetLabel;
    whilestmt->continuelabel = beGetLabel;
    whilestmt->next = parent;
    whilestmt->type = kw_while;
    whilestmt->table = localNameSpace->syms;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp);
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
            whileline = currentLineData(NULL, lex, 0);
            lex = getsym();
//			st = stmtNode(lex, whilestmt, st_goto);
//			st->label = whilestmt->continuelabel;
            st = stmtNode(lex, whilestmt, st_notselect);
            st->label = whilestmt->breaklabel;
            st->altlabel = whilestmt->continuelabel;
            st->select = select;

            st = stmtNode(lex, whilestmt, st_label);
            st->label = loopLabel;
            if (cparams.prm_cplusplus || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp);
            }
            lex = statement(lex, funcsp, whilestmt, TRUE);
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
        FreeLocalContext(whilestmt, funcsp);
    AddBlock(lex, parent, whilestmt);
    return lex;
}
static void checkNoEffect(EXPRESSION *exp)
{
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
                break;
            case en_void:
                checkNoEffect(exp->right);
            case en_not_lvalue:
            case en_thisref:
            case en_literalclass:
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
    (void)parent;
    lex = optimized_expression(lex, funcsp, NULL, &tp, &select, TRUE);
    currentLineData(parent, lex, 0);
    st = stmtNode(lex, parent, st_expr);
    st->select = select;
    if (!tp)
        error(ERR_EXPRESSION_SYNTAX);
    else if (tp->type != bt_void && tp->type != bt_any)
    {
        checkNoEffect(st->select);
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
LEXEME *statement_throw(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    TYPE *tp = NULL;
    EXPRESSION *exp = NULL;
    STATEMENT *st = stmtNode(lex, parent, st_expr); // rethow has null expression
    hasXCInfo = TRUE;
    lex = getsym();
    if (!MATCHKW(lex, semicolon))
    {
        SYMBOL *sp = gsearch("_ThrowException");
        makeXCTab(funcsp);
        lex = expression_assign(lex, funcsp, NULL, &tp, &exp, FALSE, FALSE);
        if (!tp)
        {
            error(ERR_EXPRESSION_SYNTAX);
        }   
        else if (sp)
        {
            FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
            INITLIST *arg1 = Alloc(sizeof(INITLIST)); // exception table
            INITLIST *arg2 = Alloc(sizeof(INITLIST)); // instance
            INITLIST *arg3 = Alloc(sizeof(INITLIST)); // array size
            INITLIST *arg4 = Alloc(sizeof(INITLIST)); // constructor
            INITLIST *arg5 = Alloc(sizeof(INITLIST)); // exception block
            SYMBOL *rtti = RTTIDumpType(tp);
            SYMBOL *cons = NULL;
            if (isstructured(tp))
            {
                cons = getCopyCons(basetype(tp)->sp, FALSE);
            }
            sp = basetype(sp->tp)->syms->table[0]->p;
            arg1->next = arg2;
            arg2->next = arg3;
            arg3->next = arg4;
            arg4->next = arg5;
            arg1->exp = varNode(en_auto, funcsp->xc->xctab);
            arg1->tp = &stdpointer;
            arg2->exp = exp;
            arg2->tp = tp;
            arg3->exp = isarray(tp) ? intNode(en_c_i, tp->size/(basetype(tp)->btp->size + basetype(tp)->btp->arraySkew)) : intNode(en_c_i, 1);
            arg3->tp = &stdint;
            arg4->exp = cons ? varNode(en_pc, cons) : 0;
            arg4->tp = &stdpointer;
            arg5->exp = rtti ? varNode(en_global, rtti) : intNode(en_c_i, 0);
            arg5->tp = &stdvoid;
            params->arguments = arg1;
            params->ascall = TRUE;
            params->sp = sp;
            params->functp = sp->tp;
            params->fcall = varNode(en_pc, sp);
            st->select = exprNode(en_func, NULL, NULL);
            st->select->v.func = params;
        }
    }
    else
    {
        SYMBOL *sp = gsearch("_RethrowException");
        if (sp)
        {
            FUNCTIONCALL *parms = Alloc(sizeof(FUNCTIONCALL));
            sp = basetype(sp->tp)->syms->table[0]->p;
            parms->ascall = TRUE;
            parms->sp = sp;
            parms->functp = sp->tp;
            parms->fcall = varNode(en_pc, sp);
            st->select = exprNode(en_func, NULL, NULL);
            st->select->v.func = parms;
        }
    }
    return lex;
}
LEXEME *statement_catch(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent, int label, int startlab, int endlab)
{
    BOOL last = FALSE;
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
            catchstmt->breaklabel = label;
            catchstmt->next = parent;
            catchstmt->defaultlabel = -1; /* no default */
            catchstmt->type = kw_catch;
            catchstmt->table = localNameSpace->syms;
            AllocateLocalContext(catchstmt, funcsp);
            if (MATCHKW(lex, ellipse))
            {
                last = TRUE;
                lex = getsym();
            }
            else
            {
                lex = declare(lex, funcsp,&tp, sc_catchvar, lk_none, catchstmt , FALSE, TRUE,FALSE, ac_public);               
            }
            if (needkw(&lex, closepa))
            {
                if (MATCHKW(lex, begin))
                {
                    lex = compound(lex, funcsp, catchstmt, FALSE);
                    parent->nosemi = TRUE;
                    if (parent->next)
                        parent->next->nosemi = TRUE;
                }
                else
                {
                    error(ERR_EXPECTED_CATCH_BLOCK);
                }
            }
            FreeLocalContext(catchstmt, funcsp);
            st = stmtNode(lex, parent, st_catch);
            st->label = startlab;
            st->endlabel = endlab;
            st->altlabel = beGetLabel;
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
    trystmt->breaklabel = beGetLabel;
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
        AllocateLocalContext(trystmt, funcsp);
        lex = compound(lex, funcsp, trystmt, FALSE);
        FreeLocalContext(trystmt, funcsp);
        st = stmtNode(lex, parent, st_try);
        st->label = beGetLabel;
        st->endlabel = beGetLabel;
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
            while (!MATCHKW(lex, end) && !MATCHKW(lex, eof))
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
             while (!MATCHKW(lex, end) && !MATCHKW(lex, eof))
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
        if (*exp)
            *exp = exprNode(en_void, current->select, *exp);
        else
            *exp = current->select;
    }
}
static LEXEME *autodeclare(LEXEME *lex, SYMBOL *funcsp, TYPE **tp, EXPRESSION **exp, BLOCKDATA *parent, BOOL asExpression)
{
    BLOCKDATA block;
    (void)parent;
    declareAndInitialize = FALSE;
    lex = declare(lex, funcsp, tp, sc_auto, lk_none, &block, FALSE, asExpression, FALSE, ac_public);
    memset(&block, 0, sizeof(BLOCKDATA));
    reverseAssign(block.head, exp);
    if (!*exp)
    {
        *exp = intNode(en_c_i, 0);
        errorint(ERR_NEEDY, '=');        
    }
        
    return lex;
}
static BOOL resolveToDeclaration(LEXEME * lex)
{
    marksym();
    lex = getsym();
    if (MATCHKW(lex, '{'))
    {
        backupsym(0);
        return FALSE;
    }
    if (MATCHKW(lex, openpa))
    {
        BOOL sawClose = FALSE;
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
            backupsym(0);
            return TRUE;
        }
        backupsym(0);
        return FALSE;
    }
    backupsym(0);
    return TRUE;
}
static LEXEME *statement(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent, 
                         BOOL viacontrol)
{
    if (ISID(lex))
    {
        marksym();
        lex = getsym();
        if (MATCHKW(lex, colon))
        {
            lex = backupsym(0);
            lex = statement_label(lex, funcsp, parent);
            parent->needlabel = FALSE;
            parent->nosemi = TRUE;
        }
        else
        {
            lex = backupsym(0);
        }
    }
    if (parent->needlabel && !MATCHKW(lex, kw_case) && !MATCHKW(lex, kw_default)
        && !MATCHKW(lex, begin))
        error(ERR_UNREACHABLE_CODE);

    if (!MATCHKW(lex,begin))
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
        case kw_throw:
            lex = statement_throw(lex, funcsp, parent);
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
            lex = statement_case(lex, funcsp, parent);
            parent->nosemi = TRUE;
            parent->lastcaseordefault = TRUE;
            break;
        case kw_default:
            lex = statement_default(lex, funcsp, parent);
            parent->nosemi = TRUE;
            parent->lastcaseordefault = TRUE;
            break;
        case kw_continue:
            lex = statement_continue(lex, funcsp, parent);
            break;
        case kw_break:
            lex = statement_break(lex, funcsp, parent);
            break;
        case kw_goto:
            lex = statement_goto(lex, funcsp, parent);
            break;
        case kw_return:
            lex = statement_return(lex, funcsp, parent);
            break;
        case semicolon:
            break;
        case kw_asm:
            lex = statement_asm(lex, funcsp, parent);
            return lex;
        default:
            if (startOfType(lex) && (!cparams.prm_cplusplus || resolveToDeclaration(lex)) || MATCHKW(lex, kw_namespace) || MATCHKW(lex, kw_using))
            {
                if (!cparams.prm_c99 && !cparams.prm_cplusplus)
                {
                    error(ERR_NO_DECLARATION_HERE);
                }
                if (viacontrol)
                {
                    AllocateLocalContext(parent, funcsp);
                }
                while (startOfType(lex) || MATCHKW(lex, kw_namespace) || MATCHKW(lex, kw_using))
                {
                    STATEMENT *current = parent->tail;
                    declareAndInitialize = FALSE;
                    lex = declare(lex, funcsp, NULL, sc_auto, lk_none, parent, FALSE, FALSE, FALSE, ac_public);
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
                    FreeLocalContext(parent, funcsp);
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
static BOOL thunkmainret(SYMBOL *funcsp, BLOCKDATA *parent)
{
    if (!strcmp(funcsp->name, "main") && !funcsp->parentClass && !funcsp->parentNameSpace)
    {
        STATEMENT *s = stmtNode(NULL, parent, st_return);
        s->select = intNode(en_c_i, 0);
        s->returntype = &stdint;
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
    char name[512];
    SYMBOL *sp; 
    makeXCTab(funcsp);
    sprintf(name, "@$xc%s", funcsp->decoratedName);
    sp = makeID(sc_global, &stdpointer, NULL, litlate(name));
    sp->linkage = lk_inline;
    sp->decoratedName = sp->errname = sp->name;
    sp->allocate = TRUE;
    sp->used = sp->assigned = TRUE;
    funcsp->xc->xcInitLab = beGetLabel;
    funcsp->xc->xcDestLab = beGetLabel;
    funcsp->xc->xclab = sp;
    
    sp = gsearch("_InitializeException");
    if (sp)
    {
        FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
        INITLIST *arg1 = Alloc(sizeof(INITLIST));
        INITLIST *arg2 = Alloc(sizeof(INITLIST));
        EXPRESSION *exp;
        sp = basetype(sp->tp)->syms->table[0]->p;
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
        sp = gsearch("_RundownException");
        if (sp)
        {
            sp = basetype(sp->tp)->syms->table[0]->p;
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
                        BOOL first)
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
    AllocateLocalContext(blockstmt, funcsp);
    parent->needlabel = FALSE;
    if (first)
    {
        HASHREC *hr = basetype(funcsp->tp)->syms->table[0];
        int n = 1;
        while (hr)
        {
            SYMBOL *sp2 = (SYMBOL *)hr->p;
            if (!cparams.prm_cplusplus && sp2->tp->type != bt_ellipse && !isvoid(sp2->tp) && sp2->anonymous)
                errorarg(ERR_PARAMETER_MUST_HAVE_NAME, n, sp2, funcsp);
            insert(sp2, localNameSpace->syms);
            browse_variable(sp2);
            n++;
            hr = hr->next;
        }
        if (cparams.prm_cplusplus && funcsp->isConstructor)
        {
            ParseMemberInitializers(funcsp->parentClass, funcsp);
            thisptr = thunkConstructorHead(blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms, TRUE);
        }
    }
    lex = getsym(); /* past { */
    
    st = blockstmt->tail;
    if (!cparams.prm_cplusplus)
    {
        // have to defer so we can get expression like constructor calls
        while (startOfType(lex))
        {
            STATEMENT *current = blockstmt->tail;
            declareAndInitialize = FALSE;
            lex = declare(lex, funcsp, NULL, sc_auto, lk_none, blockstmt, FALSE, FALSE, FALSE, ac_public);
            markInitializers(current);
            if (MATCHKW(lex, semicolon))
            {
                lex = getsym();
            }
            else
                break;
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
    if (!lex)
        return lex;
    browse_blockend(lex->line);
    currentLineData(blockstmt, lex, -!first);
    if (parent->type == begin || parent->type == kw_switch)
        parent->needlabel = blockstmt->needlabel;
    if (!blockstmt->hassemi && (!blockstmt->nosemi || blockstmt->lastcaseordefault))
    {
        errorint(ERR_NEEDY, ';');
    }
    st = blockstmt->head;
    if (st)
    {
        BOOL hasvla = FALSE;
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
            else
            {
                SYMBOL *var = anonymousVar(sc_auto, &stdpointer);
                EXPRESSION *exp = varNode(en_auto, var);
                deref(&stdpointer, &exp);
                st = stmtNode(lex, NULL, st_expr);
                st->select = exprNode(en_savestack, exp, NULL);
                st->next = blockstmt->head;
                blockstmt->head = st;
                if (blockstmt->blockTail)
                {
                    st = blockstmt->blockTail;
                    while (st->next)
                        st = st->next;
                    st->next = stmtNode(lex, NULL, st_expr);
                    st->next->select = exprNode(en_loadstack, exp, NULL);
                }
                else
                {
                    blockstmt->blockTail = stmtNode(lex, NULL, st_expr);
                    blockstmt->blockTail->select = exprNode(en_loadstack, exp, NULL);
                }
            }
        }
    }
    if (first && cparams.prm_cplusplus)
    {
        if (!strcmp(funcsp->name, overloadNameTab[CI_DESTRUCTOR]))
            thunkDestructorTail(blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms);
    }
    if (first && !blockstmt->needlabel && !isvoid(basetype(funcsp->tp)->btp) && basetype(funcsp->tp)->btp->type != bt_auto)
    {
        if (funcsp->linkage3 == lk_noreturn)
            error(ERR_NORETURN);
        else if (cparams.prm_c99 || cparams.prm_cplusplus)
        {
            if (!thunkmainret(funcsp, blockstmt))
                if (isref(basetype(funcsp->tp)->btp))
                    error(ERR_FUNCTION_RETURNING_REF_SHOULD_RETURN_VALUE);
                else
                    error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
        }
        else
            error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
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
        if (hasXCInfo && cparams.prm_xcept)
        {
            insertXCInfo(funcsp);
        }
    }
    FreeLocalContext(blockstmt, funcsp);
    if (parent->type == kw_catch)
    {
        SYMBOL *sp = gsearch("_CatchCleanup");
        if (sp)
        {
            FUNCTIONCALL *funcparams = Alloc(sizeof(FUNCTIONCALL));
            STATEMENT *st = stmtNode(lex, blockstmt, st_expr);
            sp = basetype(sp->tp)->syms->table[0]->p;
            funcparams->ascall = TRUE;
            funcparams->sp = sp;
            funcparams->functp = sp->tp;
            funcparams->fcall = varNode(en_pc, sp);
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
    if (!ispointer(tp) && tp->size <= chosenAssembler->arch->parmwidth)
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
    int base = chosenAssembler->arch->retblocksize;
    if (funcsp->linkage == lk_pascal)
    {
        assignPascalParams(lex, funcsp, &base, params, basetype(funcsp->tp)->btp, block);
    }
    else
    {
        if (isstructured(basetype(funcsp->tp)->btp) || basetype(basetype(funcsp->tp)->btp)->type == bt_memberptr)
        {
            // handle structured return values
            base += getSize(bt_pointer);
            if (base % chosenAssembler->arch->parmwidth)
                base += chosenAssembler->arch->parmwidth - base % chosenAssembler->arch->parmwidth;
        }
        if (funcsp->storage_class == sc_member || funcsp->storage_class == sc_virtual)
        {
            // handle 'this' pointer
            assignParam(funcsp, &base, (SYMBOL *)params->p);
            params = params->next;
        }
        assignCParams(lex, funcsp, &base, params, basetype(funcsp->tp)->btp, block);
    }
    funcsp->paramsize = base - chosenAssembler->arch->retblocksize;
}
static void handleInlines(SYMBOL *funcsp)
{
    /* so it will get instantiated as a virtual function */
    if (funcsp->linkage != lk_inline)
        return;
    if (cparams.prm_c99)
        funcsp->used = TRUE;
    /* this unqualified the current function if it has structured
     * args or return value, or if it has nested declarations
     */
         
    if (isstructured(basetype(funcsp->tp)->btp))
    {
        funcsp->noinline = TRUE;
    }
    else
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
            funcsp->noinline = TRUE;
        }
        if (funcsp->linkage == lk_inline)
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
    }
}
LEXEME *body(LEXEME *lex, SYMBOL *funcsp)
{
    int oldstartlab = startlab, oldretlab = retlab;
    BOOL oldsetjmp_used = setjmp_used;
    BOOL oldfunctionHasAssembly = functionHasAssembly;
    BOOL oldDeclareAndInitialize = declareAndInitialize;
    BOOL oldXCInfo = hasXCInfo;
    SYMBOL *oldtheCurrentFunc = theCurrentFunc;
    BLOCKDATA *block = Alloc(sizeof(BLOCKDATA)) ;
    STATEMENT *startStmt;
    functionHasAssembly = FALSE;
    setjmp_used = FALSE;
    declareAndInitialize = FALSE;
    hasXCInfo = FALSE;
    startlab = nextLabel++;
    retlab = nextLabel++;
    block->type = funcsp->hasTry ? kw_try : begin;
    theCurrentFunc = funcsp;
    if (funcsp->xcMode != xc_unspecified)
        hasXCInfo = TRUE;
    FlushLineData(funcsp->declfile, funcsp->declline);
    startStmt = currentLineData(NULL, lex, 0);
    if (startStmt)
        funcsp->linedata = startStmt->lineData;
    funcsp->declaring = TRUE;
    labelSyms = CreateHashTable(1);
    assignParameterSizes(lex, funcsp, block);
    browse_startfunc(funcsp, funcsp->declline);
    lex = compound(lex, funcsp, block, TRUE);
    browse_endfunc(funcsp, lex?lex->line : endline);
    handleInlines(funcsp);
    checkUnlabeledReferences(block);
    checkGotoPastVLA(block->head, TRUE);
    funcsp->inlineFunc.stmt = stmtNode(lex, NULL, st_block);
    funcsp->inlineFunc.stmt->lower = block->head;
    funcsp->inlineFunc.stmt->blockTail = block->blockTail;
    funcsp->declaring = FALSE;
    if (funcsp->linkage == lk_inline)
    {
        InsertInline(funcsp);
        if (!cparams.prm_cplusplus && funcsp->storage_class != sc_static)
            funcsp->genreffed = TRUE;
    }
#ifndef PARSER_ONLY
    else
    {
        genfunc(funcsp);
    }
#endif
#ifndef PARSER_ONLY
    localFree();
#endif
    hasXCInfo = oldXCInfo;
    declareAndInitialize = oldDeclareAndInitialize;
    theCurrentFunc = oldtheCurrentFunc;
    startlab = oldstartlab;
    retlab = oldretlab;
    setjmp_used = oldsetjmp_used;
    functionHasAssembly = oldfunctionHasAssembly;
    return lex;
}
