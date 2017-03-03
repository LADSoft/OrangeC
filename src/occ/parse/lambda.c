/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
  
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
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
#include "compiler.h"

extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern char *overloadNameTab[];
extern TYPE stdpointer, stdvoid, stdauto, stdint;
extern int nextLabel;
extern char infile[];

LAMBDA *lambdas;

static char lambdaQualifier[260];
static int lambdaIndex;
static SYMBOL *lambdaFunc;
void lambda_init(void)
{
    lambdaIndex = 1;
    lambdas = NULL;
}
static char *LambdaName(void)
{
    char buf[256];
    if (lambdaIndex == 1)
    {
        char *p = strrchr(infile, '\\');
        if (!p)
        {
            p = strrchr(infile, '/');
            if (!p)
                p = infile;
            else
                p++;
        }
        else
            p++;
        
        my_sprintf(lambdaQualifier, "__%s__%d", p, rand()*RAND_MAX + rand());
        while ((p = strchr(lambdaQualifier, '.')) != 0)
            *p = '_';			
    }
    my_sprintf(buf,"$$LambdaClosure%d%s", lambdaIndex++, lambdaQualifier);
    return litlate(buf);
}
static void lambda_insert(SYMBOL *sym, LAMBDA *lambdas)
{
    int align = getAlign(sc_member, sym->tp);
    if (align && lambdas->cls->tp->size % align)
        lambdas->cls->tp->size += align - lambdas->cls->tp->size % align;
    if (align > lambdas->cls->structAlign)
        lambdas->cls->structAlign = align;
    sym->parentClass = lambdas->cls;
    sym->offset = lambdas->cls->tp->size;
    insert(sym, lambdas->cls->tp->syms);
    lambdas->cls->tp->size += sym->tp->size;
}
static TYPE *lambda_type(TYPE *tp, enum e_cm mode)
{
    if (mode == cmRef)
    {
        TYPE *tp1;
        tp = basetype(tp);
        if (isref(tp))
        {
            tp = tp->btp;
        }
        tp1 = Alloc(sizeof(TYPE));
        tp1->type = bt_lref;
        tp1->size = getSize(bt_pointer);
        tp1->btp = tp;
        tp1->rootType = tp1;
        tp = tp1;
    }
    else // cmValue
    {
        TYPE *tp1;
        tp = basetype(tp);
        if (isref(tp))
        {
            tp = tp->btp;
        }
        tp = basetype(tp);
        if (!lambdas->isMutable)
        {
            tp1 = Alloc(sizeof(TYPE));
            tp1->type = bt_const;
            tp1->size = tp->size;
            tp1->btp = tp;
            tp1->rootType = tp->rootType;
            tp = tp1;
        }
    }
    return tp;
}
SYMBOL *lambda_capture(SYMBOL *sym, enum e_cm mode, BOOLEAN isExplicit)
{
    if (lambdas)
    {
        if (mode == cmThis)
        {
            if (!sym || (lambdas->lthis && sym->parentClass == basetype(lambdas->lthis->tp)->btp->sp))
            {
                if (lambdas->captureThis)
                {
                    BOOLEAN errorflg = FALSE;
                    LAMBDA *check = lambdas;
                    // have to try to replicate the symbol into the current context
                    while (check && !error)
                    {
                        if (check->captureMode == cmNone)
                        {
                            if (isExplicit)
                            {
                                if (lambdas->prev)
                                {
                                    errorflg = TRUE;
                                    errorstr(ERR_EXPLICIT_CAPTURE_BLOCKED, "this");
                                }
                            }
                            else
                            {
                                errorflg = TRUE;
                                errorstr(ERR_IMPLICIT_CAPTURE_BLOCKED, "this");
                            }
                        }
                        else if (check->captureMode == cmValue && isExplicit)
                        {
                            errorflg = TRUE;
                            errorstr(ERR_EXPLICIT_CAPTURE_BLOCKED, "this");
                        }
                        check = check->next;
                    }
                    if (!errorflg)
                    {
                        check = lambdas;
                        while (check)
                        {
                            check->captureThis = TRUE;
                            check = check->next;
                        }
                    }
                }
                else
                {
                     errorstr(ERR_IMPLICIT_CAPTURE_BLOCKED, "this");
                }
            }
        }
        else
        {
            if (sym->parent != lambdas->func)
            {
                if (sym->storage_class != sc_auto && sym->storage_class != sc_parameter)
                {
                    error(ERR_MUST_CAPTURE_AUTO_VARIABLE);
                }
                else
                {
                    LAMBDA *current = lambdas;
                    SYMBOL *sym2 = NULL;
                    LAMBDA *check;
                    while (current)
                    {
                        LAMBDASP *lsp = (LAMBDASP *)search(sym->name, current->captured);
                        if (lsp)
                        {
                            sym2 = lsp->sym;
                            break;
                        }
                        current = current->next;
                    }
                    if (sym2)
                    {
                        if (lambdas == current && isExplicit)
                        {
                            error(ERR_CAPTURE_ITEM_LISTED_MULTIPLE_TIMES);        
                        }
                        sym = sym2;
                        current = current->prev;
                    }
                    else
                    {
                        current = lambdas;
                        while (current->next) current = current->next;
                    }
                    check = current;
                    // have to try to replicate the symbol into the current context
                    while (check)
                    {
                        if (check->captureMode == cmNone)
                        {
                            if (isExplicit)
                            {
                                if (check->prev)
                                {
                                    check = NULL;
                                    errorsym(ERR_EXPLICIT_CAPTURE_BLOCKED, sym);
                                }
                            }
                            else
                            {
                                check = NULL;
                                errorsym(ERR_IMPLICIT_CAPTURE_BLOCKED, sym);
                            }
                        }
                        if (check)
                            check = check->prev;
                    }
                    IncGlobalFlag();
                    while (current)
                    {
                        // we are replicating captures through intermediate lambdas
                        // to make it easier to handle inner lambda creation.
                        LAMBDASP *ins = Alloc(sizeof(LAMBDASP));
                        ins->name = sym->name;
                        ins->parent = sym;
                        sym = clonesym(sym);
                        sym->lambdaMode = mode == cmNone ? current->captureMode : mode;
                        sym->tp = lambda_type(sym->tp, sym->lambdaMode);
                        sym->storage_class = sc_member;
                        sym->parent = current->func;
                        sym->init = NULL;
                        lambda_insert(sym, current);
                        ins->sym = sym;
                        ins->enclosing = current;
                        baseinsert((SYMBOL *)ins, current->captured);
                        current = current->prev;
                    }
                    DecGlobalFlag();
                }
            }
        }
    }
    return sym;
}
static void inferType(void)
{
    TYPE *tp = lambdas->functp;
    if (tp->type == bt_auto)
        tp = &stdvoid;
    lambdas->func->tp->btp = tp;
}
static TYPE * realArgs(SYMBOL *func)
{
    TYPE *tp;
    HASHREC **dest, *src;
    func = clonesym(func);
    tp = Alloc(sizeof(TYPE));
    *tp = *(func->tp);
    if (tp->type == bt_const)
    {
        tp->btp = Alloc(sizeof(TYPE));
        *(tp->btp) = *(func->tp->btp);
    }
    UpdateRootTypes(tp);
    func->tp = tp;
    dest = &basetype(func->tp)->syms->table[0] ;
    src = lambdas->funcargs;
    while (src)
    {
        *dest = Alloc(sizeof(HASHREC));
        (*dest)->p = (struct _hrintern_ *)clonesym((SYMBOL *)src->p);
        dest = &(*dest)->next;
        src = src->next;
    }
    return func->tp;
}
static void createCaller(void)
{
    FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
    TYPE *args = realArgs(lambdas->func);
    SYMBOL *func = makeID(sc_member, args, NULL, overloadNameTab[CI_FUNC]);
    SYMBOL *lambdaCall = namespacesearch(isstructured(basetype(lambdas->func->tp)->btp) ? "__lambdaCallS" : "__lambdaCall", globalNameSpace, FALSE, FALSE);
    BLOCKDATA block1, block2;
    STATEMENT *st;
    lambdaCall = (SYMBOL *)lambdaCall->tp->syms->table[0]->p;
    func->parentClass = lambdas->cls;
    func->linkage = lk_virtual;
    func->isInline = FALSE;
    func->omitFrame = TRUE;
    memset(&block1, 0, sizeof(BLOCKDATA));
    memset(&block2, 0, sizeof(BLOCKDATA));
    insertFunc(lambdas->cls, func);
    InsertInline(func);
    st = stmtNode(NULL, &block2, isstructured(basetype(lambdas->func->tp)->btp) ? st_expr : st_return);
    st->select = varNode(en_func, NULL);
    st->select->v.func = params;
    params->arguments = Alloc(sizeof(INITLIST));
    params->arguments->exp = varNode(en_pc, lambdas->func);
    params->arguments->tp = &stdpointer;
    params->ascall = TRUE;
    params->sp = func;
    params->fcall = varNode(en_pc, lambdaCall);
    params->functp = func->tp;
    st = stmtNode(NULL, &block1, st_block);
    st->lower = block2.head;
    st->blockTail = block2.blockTail;
    func->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    func->inlineFunc.stmt->lower = block1.head;
    func->inlineFunc.stmt->blockTail = block1.blockTail;
}
// these next two rely on the CONST specifier on the func not being set up yet.
static SYMBOL *createPtrCaller(SYMBOL *self)
{
    // if the closure is copied then used on another thread yes the resulting
    // code can get into a race condition...
    INITLIST *args;
    FUNCTIONCALL *params = Alloc(sizeof(FUNCTIONCALL));
    TYPE *pargs = realArgs(lambdas->func);
    SYMBOL *func = makeID(sc_static, pargs, NULL, "$ptrcaller");
    SYMBOL *lambdaCall = namespacesearch(isstructured(basetype(lambdas->func->tp)->btp) ? "__lambdaPtrCallS" : "__lambdaPtrCall", globalNameSpace, FALSE, FALSE);
    BLOCKDATA block1, block2;
    STATEMENT *st;
    EXPRESSION *exp = varNode(en_label, self);
    lambdaCall = (SYMBOL *)lambdaCall->tp->syms->table[0]->p;
    func->parentClass = lambdas->cls;
    func->linkage = lk_virtual;
    func->isInline = FALSE;
    func->omitFrame = TRUE;
    deref(&stdpointer, &exp);
    memset(&block1, 0, sizeof(BLOCKDATA));
    memset(&block2, 0, sizeof(BLOCKDATA));
    insertFunc(lambdas->cls, func);
    st = stmtNode(NULL, &block2, isstructured(basetype(lambdas->func->tp)->btp) ? st_expr : st_return);
    st->select = varNode(en_func, NULL);
    st->select->v.func = params;
    params->arguments = Alloc(sizeof(INITLIST));
    params->arguments->exp = varNode(en_pc, lambdas->func);
    params->arguments->tp = &stdpointer;
    args = Alloc(sizeof(INITLIST));
    args->next = params->arguments;
    params->arguments = args;
    params->arguments->exp = exp;
    params->arguments->tp = &stdpointer;
    params->ascall = TRUE;
    params->sp = func;
    params->fcall = varNode(en_pc, lambdaCall);
    params->functp = func->tp;
    st = stmtNode(NULL, &block1, st_block);
    st->lower = block2.head;
    st->blockTail = block2. blockTail;
    func->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    func->inlineFunc.stmt->lower = block1.head;
    func->inlineFunc.stmt->blockTail = block1.blockTail;
    return func;
}
static void createConverter(SYMBOL *self)
{
    SYMBOL *caller = createPtrCaller(self);
    TYPE *args = realArgs(lambdas->func);
    SYMBOL *func = makeID(sc_member, Alloc(sizeof(TYPE)), NULL, overloadNameTab[CI_CAST]);
    BLOCKDATA block1, block2;
    STATEMENT *st;
    EXPRESSION *exp;
    SYMBOL *sym = makeID(sc_parameter, &stdvoid, NULL, AnonymousName());
    HASHREC *hr = Alloc(sizeof(HASHREC));
    func->tp->type = bt_func;
    func->tp->btp = Alloc(sizeof(TYPE));
    func->tp->btp->type = bt_pointer;
    func->tp->btp->size = getSize(bt_pointer);
    func->tp->btp->btp = args;
    func->tp->rootType = func->tp;
    func->tp->btp->rootType = func->tp->btp;
    func->tp->syms = CreateHashTable(1);
    func->linkage = lk_virtual;
    func->isInline = FALSE;
    hr->p = (struct _hrintern_ *)sym;
    func->tp->syms->table[0] = hr;
    func->parentClass = lambdas->cls;
    memset(&block1, 0, sizeof(BLOCKDATA));
    memset(&block2, 0, sizeof(BLOCKDATA));
    func->castoperator = TRUE;
    insertFunc(lambdas->cls, func);
    InsertInline(func);
    InsertInline(caller);
    st = stmtNode(NULL, &block2, st_return);
    st->select = varNode(en_pc, caller);
    st = stmtNode(NULL, &block1, st_block);
    st->lower = block2.head;
    st->blockTail = block2. blockTail;
    func->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    func->inlineFunc.stmt->lower = block1.head;
    func->inlineFunc.stmt->blockTail = block1.blockTail;
}
static void finishClass(void)
{
    SYMBOL *self = makeID(sc_localstatic, &stdpointer, NULL, "$self");
    HASHREC *lst;
    if (lambdas->captureThis && lambdas->next)
    {
        SYMBOL *parent = makeID(sc_member, &stdpointer, NULL, "$parent");
        lambda_insert(parent, lambdas);
    }
    self->label = nextLabel++;
    insert(self, lambdas->cls->tp->syms);    
    insertInitSym(self);
    createConstructorsForLambda(lambdas->cls);
    insertFunc(lambdas->cls, lambdas->func);
    createConverter(self);
    if (!lambdas->isMutable)
    {
        TYPE *tp2 = Alloc(sizeof(TYPE));
        tp2->type = bt_const;
        tp2->size = lambdas->func->tp->size;
        tp2->btp = lambdas->func->tp;
        tp2->rootType = lambdas->func->tp->rootType;
        lambdas->func->tp = tp2;
    }
    createCaller();
}
static EXPRESSION *createLambda(BOOLEAN noinline)
{
    EXPRESSION *rv = NULL, **cur = &rv;
    HASHREC *hr;
    EXPRESSION *clsThs, *parentThs;
    SYMBOL *cls = makeID(lambdas->enclosingFunc ? sc_auto : sc_localstatic, lambdas->cls->tp, NULL, AnonymousName());
    SetLinkerNames(cls, lk_cdecl);
    cls->allocate = TRUE;
    if (lambdas->enclosingFunc)
    {
        insert(cls, localNameSpace->syms);
        clsThs = varNode(en_auto, cls); // this ptr
    }
    else
    {
        insert(cls, globalNameSpace->syms); // well if we could put this as an auto in the init func that would be good but there is no way to do that here...
        clsThs = varNode(en_label, cls); // this ptr
        cls->label = nextLabel++;
        insertInitSym(cls);
    }
    {
        INITIALIZER *init = NULL;
        EXPRESSION *exp = clsThs;
        callDestructor(cls, NULL, &exp, NULL, TRUE, FALSE, FALSE);
        initInsert(&init, cls->tp, exp, 0, TRUE);
        if (cls->storage_class != sc_auto)
        {
            insertDynamicDestructor(cls, init);
        }
        else
        {
            cls->dest = init;
        }
    }
    parentThs = varNode(en_auto, (SYMBOL *)basetype(lambdas->func->tp)->syms->table[0]->p); // this ptr
    hr = lambdas->cls->tp->syms->table[0];
    while (hr)
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        EXPRESSION *en = NULL, *en1 = NULL;
        if (!strcmp(sp->name, "$self"))
        {
            en1 = clsThs;
            en = varNode(en_label, sp);
            deref(&stdpointer, &en);
            en = exprNode(en_assign, en, en1);
        }
        else if (!strcmp(sp->name, "$parent"))
        {
            en1 = parentThs; // get parent from function call
            deref(&stdpointer, &en1);
            en = exprNode(en_add, clsThs, intNode(en_c_i, sp->offset)) ;
            deref(&stdpointer, &en);
            en = exprNode(en_assign, en, en1);
        }
        else if (!strcmp(sp->name, "$this"))
        {
            if (!lambdas->next || !lambdas->captureThis)
            {
                en1 = parentThs; // get parent from function call
            }
            else
            {
                SYMBOL *parent = search("$parent", lambdas->cls->tp->syms);
                en1 = varNode(en_auto, cls);
                deref(&stdpointer, &en1);
                en1 = exprNode(en_add, en1, intNode(en_c_i, parent->offset));
            }
            deref(&stdpointer, &en1);
            en = exprNode(en_add, clsThs, intNode(en_c_i, sp->offset)) ;
            deref(&stdpointer, &en);
            en = exprNode(en_assign, en, en1);
        }
        else if (sp->lambdaMode)
        {
            LAMBDASP *lsp = (LAMBDASP *)search(sp->name, lambdas->captured);
            if (lsp)
            {
                en1 = exprNode(en_add, clsThs, intNode(en_c_i, sp->offset));
                if (sp->lambdaMode == cmRef)
                {
                    SYMBOL *capture = lsp->parent;
                    deref(&stdpointer, &en1);
                    if (capture->lambdaMode)
                    {
                        en = parentThs;
                        deref(&stdpointer, &en);
                        en = exprNode(en_add, en, intNode(en_c_i, capture->offset));
                    }
                    else // must be an sc_auto
                    {
                        en = varNode(en_auto, capture);
                    }
                    if (isref(capture->tp))
                    {
                        deref(&stdpointer, &en);
                    }
                    en = exprNode(en_assign, en1, en);
                }
                else // cmValue
                {
                    SYMBOL *capture = lsp->parent;
                    TYPE *ctp = capture->tp;
                    if (capture->lambdaMode)
                    {
                        en = parentThs;
                        deref(&stdpointer, &en);
                        en = exprNode(en_add, en, intNode(en_c_i, capture->offset));
                    }
                    else // must be an sc_auto
                    {
                        en = varNode(en_auto, capture);
                    }
                    if (isref(ctp))
                    {
                        ctp = basetype(ctp)->btp;
                        deref(&stdpointer, &en);
                    }
                    if (isstructured(ctp))
                    {
                        FUNCTIONCALL *params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
                        params->arguments = (INITLIST *)Alloc(sizeof(INITLIST));
                        params->arguments->tp = ctp;
                        params->arguments->exp = en;
                        if (!callConstructor(&ctp, &en1, params, FALSE, NULL, TRUE, FALSE, TRUE, FALSE, FALSE))
                            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, lsp->sym);
                        en = en1;
                    }
                    else
                    {
                        deref(ctp, &en1);
                        deref(ctp, &en);
                        en = exprNode(en_assign, en1, en);
                    }
                }
            }
            else
            {
                diag("createLambda: no capture var");
            }
        }
        if (en)
        {
            *cur = exprNode(en_void, en, NULL);
            cur = &(*cur)->right;            
        }
        hr = hr->next;
    }
    *cur = clsThs; // this expression will be used in copy constructors, or discarded if unneeded
    return rv;
}
LEXEME *expression_lambda(LEXEME *lex, SYMBOL *funcsp, TYPE *atp, TYPE **tp, EXPRESSION **exp, int flags)
{
    LAMBDA *self;
    SYMBOL *vpl, *ths;
    HASHREC *hrl;
    TYPE *ltp;
    STRUCTSYM ssl;
    if (funcsp)
        funcsp->noinline = TRUE;
    IncGlobalFlag();
    self = Alloc(sizeof(LAMBDA));
    ltp = Alloc(sizeof(TYPE));
    ltp->type = bt_struct;
    ltp->syms = CreateHashTable(1);
    ltp->tags = CreateHashTable(1);
    ltp->size = 0;
    ltp->rootType = ltp;
    self->captured = CreateHashTable(1);
    self->oldSyms = localNameSpace->syms;
    self->oldTags = localNameSpace->tags;
    self->index = lambdaIndex;
    self->captureMode = cmNone;
    self->isMutable = FALSE;
    self->captureThis = FALSE;
    self->cls = makeID(sc_type, ltp, NULL, LambdaName());
    ltp->sp = self->cls;
    SetLinkerNames(self->cls, lk_cdecl);
    self->cls->islambda = TRUE;
    self->cls->structAlign = getAlign(sc_global, &stdpointer);
    self->func = makeID(sc_member, ltp, NULL, "$internalFunc");
    self->func->parentClass = self->cls;
    self->functp = &stdauto;
    self->enclosingFunc = theCurrentFunc;
    if (lambdas)
        lambdas->prev = self;
    self->next = lambdas;
    lambdas = self;
    
    localNameSpace->syms = CreateHashTable(1);
    localNameSpace->tags = CreateHashTable(1);
    if (lambdas->next)
    {
        self->lthis = lambdas->next->lthis;
    }
    else if (funcsp && funcsp->parentClass)
    {
        self->lthis = ((SYMBOL *)funcsp->tp->syms->table[0]->p);
    }
    lex = getsym(); // past [
    if (funcsp)
    {
        // can have a capture list;
        if (MATCHKW(lex, assign))
        {
            lex = getsym();
            if (MATCHKW(lex, comma) || MATCHKW(lex, closebr))
            {
                self->captureMode = cmValue;
                skip(&lex, comma);
            }
            else
            {
                lex = backupsym();
            }
        }
        else if (MATCHKW(lex, and))
        {
            lex = getsym();
            if (MATCHKW(lex, comma) || MATCHKW(lex, closebr))
            {
                self->captureMode = cmRef;
                skip(&lex, comma);
            }
            else
            {
                lex = backupsym();
            }
        }
        if (!MATCHKW(lex, comma) && !MATCHKW(lex, closebr))
        {
            do            
            {
                enum e_cm localMode = self->captureMode;
                if (MATCHKW(lex, comma))
                    skip(&lex, comma);
                if (MATCHKW(lex, kw_this))
                {
                    lex = getsym();
                    if (localMode == cmValue || !self->lthis)
                    {
                        error(ERR_CANNOT_CAPTURE_THIS);
                    }
                    else
                    {
                        if (self->captureThis)
                        {
                            error(ERR_CAPTURE_ITEM_LISTED_MULTIPLE_TIMES);
                        }
                        self->captureThis = TRUE;
                        lambda_capture(NULL, cmThis, TRUE);
                    }
                    continue;
                }
                else if (MATCHKW(lex, and))
                {
                    if (localMode == cmRef)
                    {
                        error(ERR_INVALID_LAMBDA_CAPTURE_MODE);
                    }
                    localMode = cmRef;
                    lex = getsym();
                }
                else 
                {
                    if (localMode == cmValue)
                    {
                        error(ERR_INVALID_LAMBDA_CAPTURE_MODE);
                    }
                    localMode = cmValue;
                }
                if (ISID(lex))
                {
                    SYMBOL *sp = search(lex->value.s.a, localNameSpace->syms);
                    LAMBDA *current = lambdas;
                    while (current && !sp)
                    {
                        sp = search(lex->value.s.a, current->oldSyms);
                        current = current->next;
                        
                    }
                    lex = getsym();
                    if (sp)
                    {
                        if (sp->packed)
                        {
                            if (!MATCHKW(lex, ellipse))
                                error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                            else
                                lex = getsym();
                        }
                        if (sp->packed)
                        {
                            int n;
                            TEMPLATEPARAMLIST * templateParam = sp->tp->templateParam->p->byPack.pack;
                            HASHREC *hr;
                            for (n=0; templateParam; templateParam = templateParam->next, n++);
                            hr = funcsp->tp->syms->table[0];
                            while (hr && ((SYMBOL *)hr->p) != sp)
                                hr = hr->next;
                            while (hr && n)
                            {
                                lambda_capture((SYMBOL *)hr->p, localMode, TRUE);
                                hr = hr->next;
                                n--;
                            }
                        }
                        else
                        {
                            lambda_capture(sp, localMode, TRUE);
                        }
                    }
                    else
                        errorstr(ERR_UNDEFINED_IDENTIFIER, lex->value.s.a);
                }
                else
                {
                    error(ERR_INVALID_LAMBDA_CAPTURE_MODE);
                }
            } while (MATCHKW(lex, comma));
        }
        needkw(&lex, closebr);
    }
    else
    {
        if (!MATCHKW(lex, closebr))
        {
            error(ERR_LAMBDA_CANNOT_CAPTURE);
        }
        else
        {
            lex = getsym();
        }
    }
    if (MATCHKW(lex, openpa))
    {
        TYPE *tpx = &stdvoid;
        HASHREC *hr;
        lex = getFunctionParams(lex, NULL, &self->func, &tpx, FALSE, sc_auto);
        if (!self->func->tp->syms)
        {
            errorstr(ERR_MISSING_TYPE_FOR_PARAMETER, "undefined");
            *tp = &stdint;
            *exp = intNode(en_c_i, 0);
            return lex;
        }
        else
        {
            self->funcargs = self->func->tp->syms->table[0];
            hr = self->func->tp->syms->table[0];
        }
        while (hr)
        {
            SYMBOL *sym = (SYMBOL *)hr->p;
            if (sym->init)
            {
                error(ERR_CANNOT_DEFAULT_PARAMETERS_WITH_LAMBDA);
            }
            hr = hr->next;
        }
        if (MATCHKW(lex, kw_mutable))
        {
            HASHREC *hr = self->captured->table[0];
            while (hr)
            {
                LAMBDASP *lsp = (LAMBDASP *)hr->p;
                if (lsp->sym->lambdaMode == cmValue)
                {
                    lsp->sym->tp = basetype(lsp->sym->tp);
                }
                hr = hr->next;
            }
            self->isMutable = TRUE;
            
            lex = getsym();
        }
        ParseAttributeSpecifiers(&lex, funcsp, TRUE);
        if (MATCHKW(lex, pointsto))
        {
            lex = getsym();
            lex = get_type_id(lex, &self->functp, funcsp, sc_cast, FALSE, TRUE);
        }
    }
    else
    {
        TYPE *tp1 = Alloc(sizeof(TYPE));
        SYMBOL *spi;
        tp1->type = bt_func;
        tp1->size = getSize(bt_pointer);
        tp1->btp = &stdvoid;
        tp1->rootType = tp1;
        tp1->sp = self->func;
        self->func->tp = tp1;
        spi = makeID(sc_parameter, tp1, NULL, AnonymousName());
        spi->anonymous = TRUE;
        spi->tp = Alloc(sizeof(TYPE));
        spi->tp->type = bt_void;
        spi->tp->rootType = spi->tp;
        insert(spi, localNameSpace->syms);
        SetLinkerNames(spi, lk_cpp);
        self->func->tp->syms = localNameSpace->syms;
        self->funcargs = self->func->tp->syms->table[0];
        self->func->tp->syms->table[0] = NULL;
    }
    vpl = makeID(sc_parameter, &stdpointer, NULL, AnonymousName());
    vpl->assigned = vpl->used = TRUE;
    SetLinkerNames(vpl, lk_cdecl);
    hrl = Alloc(sizeof(HASHREC));
    hrl->p = (struct _hrintern_ *)vpl;
    hrl->next = lambdas->func->tp->syms->table[0];
    lambdas->func->tp->syms->table[0] = hrl;
    vpl = makeID(sc_parameter, &stdpointer, NULL, AnonymousName());
    vpl->assigned = vpl->used = TRUE;
    SetLinkerNames(vpl, lk_cdecl);
    hrl = Alloc(sizeof(HASHREC));
    hrl->p = (struct _hrintern_ *)vpl;
    hrl->next = lambdas->func->tp->syms->table[0];
    lambdas->func->tp->syms->table[0] = hrl;
    SetLinkerNames(lambdas->func, lk_cdecl);
    injectThisPtr(lambdas->func, basetype(lambdas->func->tp)->syms);
    lambdas->func->tp->btp = self->functp;
    lambdas->func->tp->rootType = lambdas->func->tp;
    lambdas->func->linkage = lk_virtual;
    lambdas->func->isInline = TRUE;
    ssl.str = self->cls;
    ssl.tmpl = NULL;
    addStructureDeclaration(&ssl);
    ths = makeID(sc_member, &stdpointer, NULL, "$this");
    lambda_insert(ths, lambdas);
    if (MATCHKW(lex, begin))
    {
        lex = body(lex, self->func);
    }
    else
    {
        error(ERR_LAMBDA_FUNCTION_BODY_EXPECTED);
    }
    dropStructureDeclaration();
    localNameSpace->syms = self->oldSyms;
    localNameSpace->tags = self->oldTags;
    inferType();
    finishClass();
    *exp = createLambda(0);
    *tp = lambdas->cls->tp;
    lambdas = lambdas->next;
    if (lambdas)
        lambdas->prev = NULL;
    DecGlobalFlag();
    return lex;
}
