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

extern ARCH_DEBUG *chosenDebugger;
extern ARCH_ASM *chosenAssembler;
extern TYPE stdint, stdvoid;
extern int stdpragmas;
extern INCLUDES *includes;
extern HASHTABLE *labelSyms;
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern INCLUDES *includes;
extern enum e_kw skim_colon[];
extern enum e_kw skim_closepa[];
extern enum e_kw skim_semi[];
extern SYMBOL *theCurrentFunc;
extern TYPE stdpointer;
extern int endline;
extern char *overloadNameTab[];
extern LEXCONTEXT *context;

int startlab, retlab;
int nextLabel;
BOOL setjmp_used;
BOOL functionHasAssembly;

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
STATEMENT *currentLineData(BLOCKDATA *parent, LEXEME *lex)
{
    STATEMENT *rv = NULL;
    LINEDATA *ld = linesHead, **p = &ld;
    int lineno = lex->line;
    char *file = lex->file;
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
static LEXEME *selection_expression(LEXEME *lex, BLOCKDATA *parent, EXPRESSION **exp, SYMBOL *funcsp, enum e_kw kw)
{
    TYPE *tp = NULL;
    (void)parent;
    if (startOfType(lex) && (!cparams.prm_cplusplus || resolveToDeclaration(lex)))
    {
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
        lex = expression(lex, funcsp, NULL, &tp, exp, kw != kw_for);
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
static LEXEME *statement_break(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA *breakableStatement = parent;
    (void)lex;
    (void)funcsp;
    (void)parent;
    while (breakableStatement && breakableStatement->type == begin)
        breakableStatement = breakableStatement->next;
    if (!breakableStatement)
        error(ERR_BREAK_NO_LOOP);
    else
    {
        STATEMENT *st ;
        currentLineData(parent, lex);
        st = stmtNode(lex, parent, st_goto);
        st->label = breakableStatement->breaklabel;
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
    (void)lex;
    (void)funcsp;
    while (continuableStatement && (continuableStatement->type == kw_switch || continuableStatement->type == begin))
        continuableStatement = continuableStatement->next;
    if (!continuableStatement)
        error(ERR_CONTINUE_NO_LOOP);
    else
    {
        STATEMENT *st;
        currentLineData(parent, lex);
        st = stmtNode(lex, parent, st_goto);
        st->label = continuableStatement->continuelabel;		
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
    BLOCKDATA dostmt ;
    STATEMENT *st;
    EXPRESSION *select = NULL;
    int addedBlock = 0;
    int loopLabel = nextLabel++;
    lex = getsym();
    memset(&dostmt, 0 , sizeof(dostmt));
    dostmt.breaklabel = beGetLabel;
    dostmt.continuelabel = beGetLabel;
    dostmt.next = parent;
    dostmt.type = kw_do;
    currentLineData(&dostmt, lex);
    st = stmtNode(lex, &dostmt, st_label);
    st->label = loopLabel;
    if (cparams.prm_cplusplus || cparams.prm_c99)
    {
        addedBlock++;
        AllocateLocalContext(parent, funcsp);
    }
    lex = statement(lex, funcsp, &dostmt,TRUE);
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
            lex = selection_expression(lex, &dostmt, &select, funcsp, kw_do);
            if (!MATCHKW(lex, closepa))
            {
                error(ERR_DOWHILE_NEEDS_CLOSEPA);
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
            else
                lex = getsym();
            currentLineData(&dostmt, lex);
            st = stmtNode(lex, &dostmt, st_label);
            st->label = dostmt.continuelabel;
            st = stmtNode(lex, &dostmt, st_select);
            st->select = select;
            if (!dostmt.hasbreak && isselecttrue(st->select))
                parent->needlabel = TRUE;
            st->label = loopLabel;
            st = stmtNode(lex, &dostmt, st_label);
            st->label = dostmt.breaklabel;
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
        FreeLocalContext(&dostmt, funcsp);
    AddBlock(lex, parent, &dostmt);
    return lex;
}
static LEXEME *statement_for(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA forstmt ;
    STATEMENT *st;
    STATEMENT *forline ;
    int addedBlock = 0;
    EXPRESSION *init = NULL, *before = NULL, *select = NULL;
    int loopLabel = nextLabel++, testlabel = nextLabel++;
    memset(&forstmt, 0 , sizeof(forstmt));
    forstmt.breaklabel = beGetLabel;
    forstmt.continuelabel = beGetLabel;
    forstmt.next = parent;
    forstmt.type = kw_for;
    currentLineData(&forstmt, lex);
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (!MATCHKW(lex, semicolon))
        {
            if (cparams.prm_cplusplus && !cparams.prm_oldfor || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp);
            }
        
            lex = selection_expression(lex, &forstmt, &init, funcsp, kw_for);
        }
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
                    forline = currentLineData(NULL, lex);
                    lex = getsym();
                    if (init)
                    {
                        st = stmtNode(lex, &forstmt, st_expr);
                        st->select = init;
                    }
//					st = stmtNode(lex, &forstmt, st_goto);
//					st->label = testlabel;
                    st = stmtNode(lex, &forstmt, st_notselect);
                    st->label = forstmt.breaklabel;
                    st->altlabel = testlabel;
                    st->select = select;
                        
                    st = stmtNode(lex, &forstmt, st_label);
                    st->label = loopLabel;
                    if (cparams.prm_cplusplus || cparams.prm_c99)
                    {
                        addedBlock++;
                        AllocateLocalContext(parent, funcsp);
                    }
                    lex = statement(lex, funcsp, &forstmt, TRUE);
                    st = stmtNode(lex, &forstmt, st_label);
                    st->label = forstmt.continuelabel;
                    st = stmtNode(lex, &forstmt, st_expr);
                    st->select = before;
                    if (forline)
                    {
                        if (forstmt.head)
                            forstmt.tail = forstmt.tail->next = forline;
                        else
                            forstmt.head = forstmt.tail = forline;
                    }
                    st = stmtNode(lex, &forstmt, st_label);
                    st->label = testlabel;
                    if (select)
                    {
                        st = stmtNode(lex, &forstmt, st_select);
                        st->label = loopLabel;
                        st->select = select;
                    }
                    else
                    {
                        st = stmtNode(lex, &forstmt, st_goto);
                        st->label = loopLabel;
                    }
                    if (!forstmt.hasbreak && (!st->select || isselectfalse(st->select)))
                        parent->needlabel = TRUE;
                    st = stmtNode(lex, &forstmt, st_label);
                    st->label = forstmt.breaklabel;
                    parent->hassemi = forstmt.hassemi;
                    parent->nosemi = forstmt.nosemi;
                    
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
        FreeLocalContext(parent, funcsp);
    AddBlock(lex, parent, &forstmt);
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
        lex = selection_expression(lex, parent, &select, funcsp, kw_if);
        if (MATCHKW(lex, closepa))
        {
            BOOL optimized = FALSE;
            STATEMENT *sti;
            currentLineData(parent, lex);
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
                currentLineData(parent, lex);
                st = stmtNode(lex, parent, st_label);
                st->label = ifbranch;
                if (!parent->nosemi && !parent->hassemi)
                    errorint(ERR_NEEDY, ';');
                if (parent->nosemi && parent->hassemi)
                    error(ERR_MISPLACED_ELSE);
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
    currentLineData(parent, lex);
    if (ISID(lex))
    {
        STATEMENT *st = stmtNode(lex, parent, st_goto);
        SYMBOL *spx = search(lex->value.s.a, labelSyms);
        if (!spx)
        {
            spx = makeID(sc_ulabel, NULL, NULL, litlate(lex->value.s.a));
            spx->declfile = lex->file;
            spx->declline = lex->line;
            spx->declfilenum = lex->filenum;
            SetLinkerNames(spx, lk_none);
            spx->offset = nextLabel++;
            insert(spx, labelSyms);
        }
        st->label = spx->offset;
        lex = getsym();
        parent->needlabel = TRUE;
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
    if (spx)
    {
        if (spx->storage_class == sc_ulabel)
        {
            spx->storage_class = sc_label;
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
        insert(spx, labelSyms);
    }
    st = stmtNode(lex, parent, st_label);
    st->label = spx->offset;
    st->purelabel = TRUE;
    getsym(); /* colon */
    lex = getsym();	/* next sym */
    parent->needlabel = FALSE;
    return lex;
}
static void thunkRetDestructors(EXPRESSION **exp, HASHTABLE *syms)
{
    if (syms)
    {
        thunkRetDestructors(exp, syms->next);
        destructBlock(exp, syms->table[0]);
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
            error(ERR_RETURN_MUST_RETURN_VALUE);
        }
    }
    else
    {   tp = basetype(funcsp->tp)->btp;
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
                        lex = getArgs(lex, funcsp, funcparams);
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
                    funcparams->arguments = Alloc(sizeof(ARGLIST));
                    funcparams->arguments->tp = tp1;
                    funcparams->arguments->exp = exp1;
                    maybeConversion = FALSE;
                    returntype = tp1;
                }
                callConstructor(&ctype, &en, funcparams, FALSE, NULL, TRUE, maybeConversion); 
                returnexp = en;
            
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
            lex = optimized_expression(lex, funcsp, NULL, &tp, &returnexp, TRUE);
            if (!tp)
            {
                error(ERR_EXPRESSION_SYNTAX);
            }
            returntype = tp;
        }
        if (isref(basetype(funcsp->tp)->btp))
        {
            if (lvalue(returnexp))
            {
                EXPRESSION *exp2 = returnexp;
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
    currentLineData(parent, lex);
    thunkRetDestructors(&destexp, localNameSpace->syms);
    st = stmtNode(lex, parent, st_return);
    st->select = returnexp;
    st->destexp = destexp;
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
            if (isstructured(basetype(funcsp->tp)->btp) || isstructured(tp))
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
    BLOCKDATA switchstmt ;
    STATEMENT *st;
    EXPRESSION *select = NULL;
    int addedBlock = 0;
    memset(&switchstmt, 0 , sizeof(switchstmt));
    switchstmt.breaklabel = beGetLabel;
    switchstmt.next = parent;
    switchstmt.defaultlabel = -1; /* no default */
    switchstmt.type = kw_switch;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp);
        }
        lex = selection_expression(lex, &switchstmt, &select, funcsp, kw_switch);
        if (MATCHKW(lex, closepa))
        {
            STATEMENT *st1;
            lex = getsym();
            currentLineData(&switchstmt, lex);
            st = stmtNode(lex, &switchstmt, st_switch);
            st->select = select;
            st->breaklabel = switchstmt.breaklabel;
            lex = statement(lex, funcsp, &switchstmt, TRUE);
            st->cases = switchstmt.cases;
            st->label = switchstmt.defaultlabel;
            if (st->label != -1 && switchstmt.needlabel && !switchstmt.hasbreak)
                parent->needlabel = TRUE;
            /* force a default if there was none */
            if (st->label == -1)
            {
                st->label = nextLabel;
                st = stmtNode(lex, &switchstmt, st_label);
                st->label = nextLabel++;
            }
            st = stmtNode(lex, &switchstmt, st_label);
            st->label = switchstmt.breaklabel ;
            if (!switchstmt.nosemi && !switchstmt.hassemi)
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
        FreeLocalContext(&switchstmt, funcsp);
    AddBlock(lex, parent, &switchstmt);
    return lex;
}
static LEXEME *statement_while(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent)
{
    BLOCKDATA whilestmt ;
    STATEMENT *st;
    STATEMENT *whileline;
    EXPRESSION *select = NULL;
    BOOL addedBlock = FALSE;
    int loopLabel = nextLabel++;
    memset(&whilestmt, 0 , sizeof(whilestmt));
    whilestmt.breaklabel = beGetLabel;
    whilestmt.continuelabel = beGetLabel;
    whilestmt.next = parent;
    whilestmt.type = kw_while;
    lex = getsym();
    if (MATCHKW(lex, openpa))
    {
        lex = getsym();
        if (cparams.prm_cplusplus || cparams.prm_c99)
        {
            addedBlock++;
            AllocateLocalContext(parent, funcsp);
        }
        lex = selection_expression(lex, &whilestmt, &select, funcsp, kw_while);
        if (!MATCHKW(lex, closepa))
        {
            error(ERR_WHILE_NEEDS_CLOSEPA);
            
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
        else
        {
            whileline = currentLineData(NULL, lex);
            lex = getsym();
//			st = stmtNode(lex, &whilestmt, st_goto);
//			st->label = whilestmt.continuelabel;
            st = stmtNode(lex, &whilestmt, st_notselect);
            st->label = whilestmt.breaklabel;
            st->altlabel = whilestmt.continuelabel;
            st->select = select;

            st = stmtNode(lex, &whilestmt, st_label);
            st->label = loopLabel;
            if (cparams.prm_cplusplus || cparams.prm_c99)
            {
                addedBlock++;
                AllocateLocalContext(parent, funcsp);
            }
            lex = statement(lex, funcsp, &whilestmt, TRUE);
            st = stmtNode(lex, &whilestmt, st_label);
            st->label = whilestmt.continuelabel;
            if (whileline)
            {
                if (whilestmt.head)
                    whilestmt.tail = whilestmt.tail->next = whileline;
                else
                    whilestmt.head = whilestmt.tail = whileline;
                while (whilestmt.tail->next)
                    whilestmt.tail = whilestmt.tail->next;
            }
            st = stmtNode(lex, &whilestmt, st_select);
            st->label = loopLabel;
            st->select = select;
            if (!whilestmt.hasbreak && isselecttrue(st->select))
                parent->needlabel = TRUE;
            st = stmtNode(lex, &whilestmt, st_label);
            st->label = whilestmt.breaklabel;
            parent->hassemi = whilestmt.hassemi;
            parent->nosemi = whilestmt.nosemi;
        }
    }
    else
    {
        error(ERR_WHILE_NEEDS_OPENPA);
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    } 
    while (addedBlock--)
        FreeLocalContext(&whilestmt, funcsp);
    AddBlock(lex, parent, &whilestmt);
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
    currentLineData(parent, lex);
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
                currentLineData(parent, lex);
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
            currentLineData(parent, lex);
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
                currentLineData(parent, lex);
                lex = getsym();
            }
            needkw(&lex, end);
            parent->nosemi = TRUE;			
         }
         else
         {
             /* problematic, ASM keyword without a block.  Skip to end of line... */
            currentLineData(parent, lex);
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
    memset(&block, 0, sizeof(block));
    lex = declare(lex, funcsp, tp, sc_auto, lk_none, &block, FALSE, asExpression, FALSE, ac_public);
    
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
    BOOL isDeclaration = TRUE;
    marksym();
    lex = getsym();
    if (MATCHKW(lex, '{'))
    {
        backupsym(0);
        return FALSE;
    }
    while (lex != NULL)
    {
        if (MATCHKW(lex, semicolon) || MATCHKW(lex, closepa))
        {
            backupsym(0);
            return isDeclaration;
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
                    isDeclaration = FALSE;
                    level++;
                    if (sawClose)
                    {
                        backupsym(0);
                        return TRUE;
                        
                    }
                }
                else if (MATCHKW(lex, closepa))
                {
                    sawClose = TRUE;
                    level--;
                }
                else if (MATCHKW(lex, openbr))
                {
                    if (sawClose)
                    {
                        backupsym(0);
                        return TRUE;
                    }
                }
                lex = getsym();
            }
            if (MATCHKW(lex, assign) || MATCHKW(lex, openpa) || MATCHKW(lex, openbr))
            {
                backupsym(0);
                return TRUE;
            }
            while (lex != NULL && !MATCHKW(lex, semicolon) && !MATCHKW(lex, comma) && !MATCHKW(lex, closepa))
                lex = getsym();
        }
        else
        {
            lex = getsym();
        }
    }
    backupsym(0);
    return TRUE;
}
static LEXEME *statement(LEXEME *lex, SYMBOL *funcsp, BLOCKDATA *parent, 
                         BOOL viacontrol)
{
    while (ISID(lex) && *includes->lptr == ':' && includes->lptr[1] != ':')
    {
        lex = statement_label(lex, funcsp, parent);
        parent->needlabel = FALSE;
        parent->nosemi = TRUE;
    }
    if (parent->needlabel && !MATCHKW(lex, kw_case) && !MATCHKW(lex, kw_default)
        && !MATCHKW(lex, begin))
        error(ERR_UNREACHABLE_CODE);

    if (!MATCHKW(lex,begin))
        parent->needlabel = FALSE;
    
    parent->nosemi = FALSE;
    switch(KW(lex))
    {
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
                    lex = declare(lex, funcsp, NULL, sc_auto, lk_none, parent, FALSE, FALSE, FALSE, ac_public);
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
static LEXEME *compound(LEXEME *lex, SYMBOL *funcsp, 
                        BLOCKDATA *parent,   
                        BOOL first)
{
    BLOCKDATA blockstmt ;
    int pragmas = stdpragmas;
    STATEMENT *st;
    int blknum;
    browse_blockstart(lex->line);
    memset(&blockstmt, 0 , sizeof(blockstmt));
    blockstmt.next = parent;
    blockstmt.type = begin;
    blockstmt.needlabel = parent->needlabel;
    currentLineData(&blockstmt, lex);
    AllocateLocalContext(&blockstmt, funcsp);
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
        if (cparams.prm_cplusplus && !strcmp(funcsp->name, overloadNameTab[CI_CONSTRUCTOR]))
            thunkConstructorHead(&blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms, TRUE);
    }
    lex = getsym(); /* past { */
    
    st = blockstmt.tail;
    while (startOfType(lex))
    {
        lex = declare(lex, funcsp, NULL, sc_auto, lk_none, &blockstmt, FALSE, FALSE, FALSE, ac_public);
        if (MATCHKW(lex, semicolon))
        {
            lex = getsym();
        }
        else
            break;
    }
    if (parent->type == kw_switch)
    {
        if (st != blockstmt.tail)
        /* kinda naive... */
            error(ERR_INITIALIZATION_MAY_BE_BYPASSED);
    }
    currentLineData(&blockstmt, lex);
    blockstmt.nosemi = TRUE ; /* in case it is an empty body */
    while (lex && !MATCHKW(lex, end))
    {
        if (!blockstmt.hassemi && !blockstmt.nosemi)
            errorint(ERR_NEEDY, ';');
        blockstmt.lastcaseordefault = FALSE;
        lex = statement(lex, funcsp, &blockstmt, FALSE);
    }
    browse_blockend(lex->line);
    currentLineData(&blockstmt, lex);
    if (parent->type == begin || parent->type == kw_switch)
        parent->needlabel = blockstmt.needlabel;
    if (!blockstmt.hassemi && (!blockstmt.nosemi || blockstmt.lastcaseordefault))
    {
        errorint(ERR_NEEDY, ';');
    }
    st = blockstmt.head;
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
                st->next = blockstmt.head;
                blockstmt.head = st;
                if (blockstmt.blockTail)
                {
                    st = blockstmt.blockTail;
                    while (st->next)
                        st = st->next;
                    st->next = stmtNode(lex, NULL, st_expr);
                    st->next->select = exprNode(en_loadstack, exp, NULL);
                }
                else
                {
                    blockstmt.blockTail = stmtNode(lex, NULL, st_expr);
                    blockstmt.blockTail->select = exprNode(en_loadstack, exp, NULL);
                }
            }
        }
    }
    if (first && cparams.prm_cplusplus && !strcmp(funcsp->name, overloadNameTab[CI_DESTRUCTOR]))
        thunkDestructorTail(&blockstmt, funcsp->parentClass, funcsp, basetype(funcsp->tp)->syms);
    FreeLocalContext(&blockstmt, funcsp);
    if (first && !blockstmt.needlabel && !isvoid(basetype(funcsp->tp)->btp))
    {
        if (funcsp->linkage3 == lk_noreturn)
            error(ERR_NORETURN);
        else if (cparams.prm_c99 || cparams.prm_cplusplus)
        {
            if (!thunkmainret(funcsp, &blockstmt))
                if (isref(basetype(funcsp->tp)->btp))
                    error(ERR_FUNCTION_RETURNING_REF_SHOULD_RETURN_VALUE);
                else
                    error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
        }
        else
            error(ERR_FUNCTION_SHOULD_RETURN_VALUE);
    }
    needkw(&lex, end);
    AddBlock(lex, parent, &blockstmt);
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
    SYMBOL *oldtheCurrentFunc = theCurrentFunc;
    BLOCKDATA block;
    STATEMENT *startStmt;
    functionHasAssembly = FALSE;
    setjmp_used = FALSE;
    startlab = nextLabel++;
    retlab = nextLabel++;
    memset(&block, 0, sizeof(block));
    block.type = begin;
    theCurrentFunc = funcsp;
    FlushLineData(funcsp->declfile, funcsp->declline);
    startStmt = currentLineData(NULL, lex);
    if (startStmt)
        funcsp->linedata = startStmt->lineData;
    funcsp->declaring = TRUE;
    labelSyms = CreateHashTable(1);
    assignParameterSizes(lex, funcsp, &block);
    browse_startfunc(funcsp, funcsp->declline);
    lex = compound(lex, funcsp, &block, TRUE);
    browse_endfunc(funcsp, lex?lex->line : endline);
    handleInlines(funcsp);
    checkUnlabeledReferences(&block);
    checkGotoPastVLA(block.head, TRUE);
    funcsp->inlineFunc.stmt = stmtNode(lex, NULL, st_block);
    funcsp->inlineFunc.stmt->lower = block.head;
    funcsp->inlineFunc.stmt->blockTail = block.blockTail;
    funcsp->declaring = FALSE;
    if (funcsp->linkage == lk_inline)
    {
        InsertInline(funcsp);
        if (!cparams.prm_cplusplus && funcsp->storage_class != sc_static)
            funcsp->genreffed = TRUE;
    }
#ifndef PARSER_ONLY
    else
        genfunc(funcsp);
#endif
#ifndef PARSER_ONLY
    localFree();
#endif
    theCurrentFunc = oldtheCurrentFunc;
    startlab = oldstartlab;
    retlab = oldretlab;
    setjmp_used = oldsetjmp_used;
    functionHasAssembly = oldfunctionHasAssembly;
    return lex;
}
