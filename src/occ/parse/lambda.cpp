/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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
/*

// this module generates code similar to the following given the declaration:

auto [] (auto a, auto b) { return a + b; }

when the auto return value is specificied explicitly, the 'auto' return value will be replaced with it.
when the parameters aren't auto then the code will be similar, but not templated.


struct LambdaStruct
{
    LambdaStruct() { <captured variables initialized here> }
    LambdaStruct(const LambdaStruct&) = default;

    template <class T, class T1>
    auto operator ()(T a, T1 b) { return a + b; }
    template <class T, class T1>
    operator decltype(&___ptrcall<T, T1>)(){ ptr = this; return &___ptrcall; };

private:
    template <class T, class T1>
    static auto ___ptrcall (T a, T1 b) { return ptr->operator ()(a, b); }

    <nonstatic members list out all the captured variables>

    static struct aa *___self;

};
struct aa *aa::___self;
*/
extern NAMESPACEVALUES *globalNameSpace, *localNameSpace;
extern const char* overloadNameTab[];
extern TYPE stdpointer, stdvoid, stdauto, stdint;
extern int nextLabel;
extern char infile[];
extern INCLUDES* includes;
extern int templateNestingCount;
extern int dontRegisterTemplate;
extern SYMBOL *theCurrentFunc;

LAMBDA* lambdas;

static char lambdaQualifier[260];
static int lambdaIndex;
static SYMBOL* lambdaFunc;
void lambda_init(void)
{
    lambdaIndex = 1;
    lambdas = NULL;
}
static char* LambdaName(void)
{
    char buf[256];
    if (lambdaIndex == 1)
    {
        char* p = strrchr(infile, '\\');
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

        my_sprintf(lambdaQualifier, "__%s__%d", p, rand() * RAND_MAX + rand());
        while ((p = strchr(lambdaQualifier, '.')) != 0)
            *p = '_';
    }
    my_sprintf(buf, "$$LambdaClosure%d%s", lambdaIndex++, lambdaQualifier);
    return litlate(buf);
}
static void lambda_insert(SYMBOL* sym, LAMBDA* lambdas)
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
static TYPE* lambda_type(TYPE* tp, enum e_cm mode)
{
    if (mode == cmRef)
    {
        TYPE* tp1;
        tp = basetype(tp);
        if (isref(tp))
        {
            tp = tp->btp;
        }
        tp1 = (TYPE *)Alloc(sizeof(TYPE));
        tp1->type = bt_lref;
        tp1->size = getSize(bt_pointer);
        tp1->btp = tp;
        tp1->rootType = tp1;
        tp = tp1;
    }
    else  // cmValue
    {
        TYPE* tp1;
        tp = basetype(tp);
        if (isref(tp))
        {
            tp = tp->btp;
        }
        tp = basetype(tp);
        if (!lambdas->isMutable)
        {
            tp1 = (TYPE *)Alloc(sizeof(TYPE));
            tp1->type = bt_const;
            tp1->size = tp->size;
            tp1->btp = tp;
            tp1->rootType = tp->rootType;
            tp = tp1;
        }
    }
    return tp;
}
SYMBOL* lambda_capture(SYMBOL* sym, enum e_cm mode, bool isExplicit)
{
    if (lambdas)
    {
        if (mode == cmThis)
        {
            if (!sym || (lambdas->lthis && sym->parentClass == basetype(lambdas->lthis->tp)->btp->sp))
            {
                if (lambdas->captureThis)
                {
                    bool errorflg = false;
                    LAMBDA* check = lambdas;
                    // have to try to replicate the symbol into the current context
                    while (check && !errorflg)
                    {
                        if (check->captureMode == cmNone)
                        {
                            if (isExplicit)
                            {
                                if (lambdas->prev)
                                {
                                    errorflg = true;
                                    errorstr(ERR_EXPLICIT_CAPTURE_BLOCKED, "this");
                                }
                            }
                            else
                            {
                                errorflg = true;
                                errorstr(ERR_IMPLICIT_CAPTURE_BLOCKED, "this");
                            }
                        }
                        else if (check->captureMode == cmValue && isExplicit)
                        {
                            errorflg = true;
                            errorstr(ERR_EXPLICIT_CAPTURE_BLOCKED, "this");
                        }
                        check = check->next;
                    }
                    if (!errorflg)
                    {
                        check = lambdas;
                        while (check)
                        {
                            check->captureThis = true;
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
                    LAMBDA* current = lambdas;
                    SYMBOL* sym2 = NULL;
                    LAMBDA* check;
                    while (current)
                    {
                        LAMBDASP* lsp = (LAMBDASP*)search(sym->name, current->captured);
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
                        while (current->next)
                            current = current->next;
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
                        LAMBDASP* ins = (LAMBDASP *)Alloc(sizeof(LAMBDASP));
                        ins->name = sym->name;
                        ins->parent = sym;
                        sym = clonesym(sym);
                        sym->lambdaMode = mode == cmNone ? current->captureMode : mode;
                        sym->tp = lambda_type(sym->tp, sym->lambdaMode);
                        sym->storage_class = sc_member;
                        sym->parent = current->func;
                        sym->access = ac_private;
                        if (mode != cmExplicitValue)
                            sym->init = NULL;
                        lambda_insert(sym, current);
                        ins->sym = sym;
                        ins->enclosing = current;
                        baseinsert((SYMBOL*)ins, current->captured);
                        current = current->prev;
                    }
                    DecGlobalFlag();
                }
            }
        }
    }
    return sym;
}
static TYPE* cloneFuncType(SYMBOL* funcin)
{
    TYPE* tp_in = funcin->tp;
    TYPE** tp;
    HASHREC **dest, *src;
    SYMBOL* func = clonesym(funcin);
    tp = &func->tp;
    while (tp_in)
    {
        *tp = (TYPE *)Alloc(sizeof(TYPE));
        **tp = *(tp_in);
        tp_in = tp_in->btp;
        tp = &(*tp)->btp;
    }
    UpdateRootTypes(func->tp);
    basetype(func->tp)->syms = CreateHashTable(1);
    dest = &basetype(func->tp)->syms->table[0];
    src = basetype(funcin->tp)->syms->table[0];
    while (src)
    {
        *dest = (HASHREC *)Alloc(sizeof(HASHREC));
        (*dest)->p = (struct sym *)clonesym((SYMBOL*)src->p);
        dest = &(*dest)->next;
        src = src->next;
    }
    return func->tp;
}
static void cloneTemplateParams(SYMBOL* func)
{
    TEMPLATEPARAMLIST** tplp = &func->templateParams;
    HASHREC* hr = basetype(func->tp)->syms->table[0];
    int index = 0;
    (*tplp) = (TEMPLATEPARAMLIST*)(TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
    (*tplp)->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
    (*tplp)->p->type = kw_new;
    tplp = &(*tplp)->next;
    while (hr)
    {
        SYMBOL* arg = (SYMBOL*)(hr->p);
        if (!arg->thisPtr)
        {
            TYPE* tpn = (TYPE*)(TYPE *)Alloc(sizeof(TYPE));
            *tpn = *arg->tp;
            arg->tp = tpn;
            UpdateRootTypes(tpn);
            (*tplp) = (TEMPLATEPARAMLIST*)(TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
            (*tplp)->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
            (*tplp)->argsym = arg;
            (*tplp)->p->type = kw_typename;
            arg->tp->templateParam = *tplp;
            arg->templateParams = *tplp;
            tplp = &(*tplp)->next;
        }
        hr = hr->next;
    }
}
static void convertCallToTemplate(SYMBOL* func)
{
    TEMPLATEPARAMLIST** tplholder;
    HASHREC* hr;
    func->templateParams = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST *)Alloc(sizeof(TEMPLATEPARAMLIST));
    func->templateParams->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
    func->templateParams->p->type = kw_new;
    if (isautotype(lambdas->functp))
        basetype(func->tp)->btp = &stdauto;  // convert return type back to auto
    tplholder = &func->templateParams->next;
    hr = basetype(func->tp)->syms->table[0];
    int index = 0;
    while (hr)
    {
        SYMBOL* arg = (SYMBOL*)hr->p;
        if (isautotype(arg->tp))
        {
            (*tplholder) = (TEMPLATEPARAMLIST*)(TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
            (*tplholder)->p = (TEMPLATEPARAM *)Alloc(sizeof(TEMPLATEPARAM));
            (*tplholder)->argsym = arg;
            arg->tp = (TYPE*)(TYPE *)Alloc(sizeof(TYPE));
            arg->tp->type = bt_templateparam;
            arg->tp->templateParam = *tplholder;
            (*tplholder)->p->type = kw_typename;
            (*tplholder)->p->index = index++;
            tplholder = &(*tplholder)->next;
        }
        hr = hr->next;
    }
    func->templateLevel = templateNestingCount;
    func->parentTemplate = func;
}
static SYMBOL* createPtrToCaller(SYMBOL* self)
{
    INITLIST** argptr;
    HASHREC* hr;
    FUNCTIONCALL* params = (FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
    TYPE* args = cloneFuncType(lambdas->func);
    SYMBOL* func = makeID(sc_static, args, NULL, "___ptrcall");
    BLOCKDATA block1, block2;
    STATEMENT* st;
    basetype(args)->sp = func;
    basetype(args)->btp = basetype(lambdas->func->tp)->btp;
    func->parentClass = lambdas->cls;
    func->linkage = lk_cdecl;
    func->isInline = true;
    func->storage_class = sc_static;
    func->access = ac_private;

    memset(&block1, 0, sizeof(BLOCKDATA));
    memset(&block2, 0, sizeof(BLOCKDATA));
    insertFunc(lambdas->cls, func);
    func->tp->syms->table[0] = func->tp->syms->table[0]->next;  // elide this pointer
    st = stmtNode(NULL, &block2, isstructured(basetype(lambdas->func->tp)->btp) ? st_expr : st_return);
    st->select = varNode(en_func, NULL);
    st->select->v.func = params;
    st->select = exprNode(en_thisref, st->select, NULL);

    argptr = &params->arguments;
    hr = args->syms->table[0];
    /*
    if (isstructured(basetype(lambdas->func->tp)->btp))
    {
        TYPE *tp= basetype(lambdas->func->tp)->btp;
        if (isref(tp))
            tp = basetype(tp)->btp;
        tp = basetype(tp);
        params->returnSP = tp->sp;
        params->returnEXP = varNode(en_auto, ((SYMBOL *)hr->p));
        hr = hr->next;
    }
    */
    while (hr)
    {
        SYMBOL* sym = (SYMBOL*)hr->p;
        if (sym->tp->type == bt_void)
            break;
        sym = clonesym(sym);
        sym->offset -= getSize(bt_pointer);
        *argptr = (INITLIST *)Alloc(sizeof(INITLIST));
        if (isstructured(sym->tp) && !isref(sym->tp))
        {
            SYMBOL* sym2 = anonymousVar(sc_auto, sym->tp)->v.sp;
            sym2->stackblock = true;
            (*argptr)->exp = varNode(en_auto, sym2); // DAL MODIFIED
        }
        else
        {
            (*argptr)->exp = varNode(en_auto, sym);
            deref(sym->tp, &(*argptr)->exp);
        }
        (*argptr)->tp = sym->tp;
        argptr = &(*argptr)->next;
        hr = hr->next;
    }
    params->ascall = true;
    params->sp = lambdas->func;
    params->fcall = varNode(en_pc, lambdas->func);
    params->functp = func->tp;
    params->thisptr = varNode(en_global, self);
    deref(&stdpointer, &params->thisptr);
    params->thistp = self->tp;
    st = stmtNode(NULL, &block1, st_block);
    st->lower = block2.head;
    st->blockTail = block2.blockTail;
    func->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    func->inlineFunc.stmt->lower = block1.head;
    func->inlineFunc.stmt->blockTail = block1.blockTail;
    func->inlineFunc.syms = basetype(args)->syms;
    if (lambdas->templateFunctions)
    {
        LEXEME* lex1;
        FILE* handle = includes->handle;
        unsigned char* p = includes->lptr;
        int line = includes->line;
        char buf[1000];
        int l = 0;
        HASHREC* hr = basetype(lambdas->func->tp)->syms->table[0];
        if (isautotype(lambdas->functp))
            basetype(func->tp)->btp = &stdauto;  // convert return type back to auto
        cloneTemplateParams(func);
        func->templateLevel = templateNestingCount;
        func->parentTemplate = func;
        strcpy(buf + l, "{return ___self->operator()(");
        l += strlen(buf + l);
        if (!hr)
            strcpy(buf, ",");
        while (hr)
        {
            SYMBOL* sym = (SYMBOL*)hr->p;
            if (!sym->thisPtr)
            {
                sprintf(buf + l, "%s,", sym->name);
                l += strlen(buf + l);
            }
            hr = hr->next;
        }
        strcpy(buf + l - 1, ");} ;");
        includes->handle = NULL;
        includes->lptr = (unsigned char *)buf;
        lex1 = getsym();
        getDeferredData(lex1, func, true);
        includes->handle = handle;
        includes->lptr = p;
        includes->line = line;
    }
    else
    {
        InsertInline(func);
    }
    UpdateRootTypes(func->tp);
    return func;
}
// this next reloes on the CONST specifier on the func not being set up yet.
static void createConverter(SYMBOL* self)
{
    SYMBOL* caller = createPtrToCaller(self);
    TYPE* args = cloneFuncType(lambdas->func);
    SYMBOL* func = makeID(sc_member, (TYPE *)Alloc(sizeof(TYPE)), NULL, overloadNameTab[CI_CAST]);
    BLOCKDATA block1, block2;
    STATEMENT* st;
    EXPRESSION* exp;
    SYMBOL* sym = makeID(sc_parameter, &stdvoid, NULL, AnonymousName());
    HASHREC* hr = (HASHREC *)Alloc(sizeof(HASHREC));
    func->tp->type = bt_func;
    func->tp->btp = (TYPE *)Alloc(sizeof(TYPE));
    func->tp->btp->type = bt_pointer;
    func->tp->btp->size = getSize(bt_pointer);
    func->tp->btp->btp = args;
    func->tp->rootType = func->tp;
    func->tp->btp->rootType = func->tp->btp;
    func->tp->syms = CreateHashTable(1);
    func->parentClass = lambdas->cls;
    func->linkage = lk_virtual;
    func->isInline = true;
    func->storage_class = sc_member;
    func->castoperator = true;
    func->tp->syms = CreateHashTable(1);
    hr->p = (SYMBOL *)sym;
    func->tp->syms->table[0] = hr;
    injectThisPtr(func, func->tp->syms);
    func->parentClass = lambdas->cls;
    memset(&block1, 0, sizeof(BLOCKDATA));
    memset(&block2, 0, sizeof(BLOCKDATA));
    insertFunc(lambdas->cls, func);
    // assign ___self = this
    st = stmtNode(NULL, &block2, st_expr);
    st->select = varNode(en_auto, (SYMBOL*)basetype(func->tp)->syms->table[0]->p);
    deref(&stdpointer, &st->select);
    st->select = exprNode(en_assign, varNode(en_global, self), st->select);
    deref(&stdpointer, &st->select->left);
    // return pointer to ptrtocaller
    st = stmtNode(NULL, &block2, st_return);
    st->select = varNode(en_pc, caller);
    st = stmtNode(NULL, &block1, st_block);
    st->lower = block2.head;
    st->blockTail = block2.blockTail;
    func->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    func->inlineFunc.stmt->lower = block1.head;
    func->inlineFunc.stmt->blockTail = block1.blockTail;
    func->inlineFunc.syms = CreateHashTable(1);
    if (lambdas->templateFunctions)
    {
        LEXEME* lex1;
        FILE* handle = includes->handle;
        unsigned char* p = includes->lptr;
        int line = includes->line;
        char buf[1000];
        int l = 0;
        TEMPLATEPARAMLIST *tpl, **tplp, **tplp2;
        FUNCTIONCALL* f = (FUNCTIONCALL*)(FUNCTIONCALL *)Alloc(sizeof(FUNCTIONCALL));
        INITLIST** args = &f->arguments;
        HASHREC* hr;
        func->templateParams = caller->templateParams;
        func->templateLevel = templateNestingCount;
        func->parentTemplate = func;
        basetype(func->tp)->btp = (TYPE *)Alloc(sizeof(TYPE));
        basetype(func->tp)->btp->type = bt_templatedecltype;
        basetype(func->tp)->btp->templateDeclType = exprNode(en_func, NULL, NULL);
        basetype(func->tp)->btp->templateDeclType->v.func = f;
        hr = basetype(caller->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = (SYMBOL*)hr->p;
            if (sym->thisPtr)
            {
                f->thistp = sym->tp;
                f->thisptr = intNode(en_c_i, 0);
            }
            else
            {
                (*args) = (INITLIST*)(INITLIST *)Alloc(sizeof(INITLIST));
                (*args)->tp = sym->tp;
                (*args)->exp = intNode(en_c_i, 0);
                args = &(*args)->next;
            }
            hr = hr->next;
        }
        f->ascall = false;
        f->asaddress = true;
        f->sp = caller;
        f->fcall = varNode(en_pc, f->sp);
        f->functp = f->sp->tp;
        if (isstructured(basetype(caller->tp)->btp))
        {
            f->returnEXP = intNode(en_c_i, 0);
            f->returnSP = basetype(basetype(caller->tp)->btp)->sp;
        }
        strcpy(buf + l, "{ ___self=this;return &___ptrcall;} ;");

        includes->handle = NULL;
        includes->lptr = (unsigned char *)buf;
        lex1 = getsym();
        getDeferredData(lex1, func, true);
        includes->handle = handle;
        includes->lptr = p;
        includes->line = line;
        func->templateLevel = templateNestingCount;
        func->parentTemplate = func;
    }
    else
    {
        InsertInline(func);
    }
    UpdateRootTypes(func->tp);
}
static bool lambda_get_template_state(SYMBOL* func)
{
    HASHREC* hr = basetype(func->tp)->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        if (isautotype(sp->tp))
            return true;
        hr = hr->next;
    }
    return false;
}
static void finishClass(void)
{
    TYPE* tps = (TYPE*)(TYPE *)Alloc(sizeof(TYPE));
    SYMBOL* self = makeID(sc_static, tps, NULL, "___self");
    HASHREC* lst;
    tps->type = bt_pointer;
    tps->size = getSize(bt_pointer);
    tps->btp = lambdas->cls->tp;
    self->access = ac_private;

    if (lambdas->captureThis && lambdas->next)
    {
        SYMBOL* parent = makeID(sc_member, &stdpointer, NULL, "$parent");
        lambda_insert(parent, lambdas);
    }
    self->label = nextLabel++;
    self->parentClass = lambdas->cls;
    SetLinkerNames(self, lk_cdecl);
    insert(self, lambdas->cls->tp->syms);
    insertInitSym(self);
    createConstructorsForLambda(lambdas->cls);
    if (lambdas->templateFunctions)
    {
        templateNestingCount++;
        dontRegisterTemplate++;
        convertCallToTemplate(lambdas->func);
    }
    SetLinkerNames(lambdas->func, lk_cdecl);
    insertFunc(lambdas->cls, lambdas->func);
    createConverter(self);
    if (lambdas->templateFunctions)
    {
        templateNestingCount--;
        dontRegisterTemplate--;
    }
    if (!lambdas->isMutable)
    {
        SYMBOL* ths = (SYMBOL*)lambdas->func->tp->syms->table[0]->p;
        TYPE* tp2 = (TYPE *)Alloc(sizeof(TYPE));
        tp2->type = bt_const;
        tp2->size = lambdas->func->tp->size;
        tp2->btp = lambdas->func->tp;
        tp2->rootType = lambdas->func->tp->rootType;
        lambdas->func->tp = tp2;
        tp2 = (TYPE *)Alloc(sizeof(TYPE));
        tp2->type = bt_const;
        tp2->size = basetype(ths->tp)->btp->size;
        tp2->btp = basetype(ths->tp)->btp;
        basetype(ths->tp)->btp = tp2;
        UpdateRootTypes(ths->tp);
    }
}
static EXPRESSION* createLambda(bool noinline)
{
    EXPRESSION *rv = NULL, **cur = &rv;
    HASHREC* hr;
    EXPRESSION *clsThs, *parentThs;
    SYMBOL* cls = makeID(lambdas->enclosingFunc ? sc_auto : sc_localstatic, lambdas->cls->tp, NULL, AnonymousName());
    SetLinkerNames(cls, lk_cdecl);
    cls->allocate = true;
    if (lambdas->enclosingFunc)
    {
        insert(cls, localNameSpace->syms);
        clsThs = varNode(en_auto, cls);  // this ptr
    }
    else
    {
        insert(cls, globalNameSpace->syms);  // well if we could put this as an auto in the init func that would be good but there
                                             // is no way to do that here...
        clsThs = varNode(en_global, cls);    // this ptr
        cls->label = nextLabel++;
        insertInitSym(cls);
    }
    {
        INITIALIZER* init = NULL;
        EXPRESSION* exp = clsThs;
        callDestructor(cls, NULL, &exp, NULL, true, false, false);
        initInsert(&init, cls->tp, exp, 0, true);
        if (cls->storage_class != sc_auto)
        {
            insertDynamicDestructor(cls, init);
        }
        else
        {
            cls->dest = init;
        }
    }
    parentThs = varNode(en_auto, (SYMBOL*)basetype(lambdas->func->tp)->syms->table[0]->p);  // this ptr
    hr = lambdas->cls->tp->syms->table[0];
    while (hr)
    {
        SYMBOL* sp = (SYMBOL*)hr->p;
        EXPRESSION *en = NULL, *en1 = NULL;
        if (!strcmp(sp->name, "$parent"))
        {
            en1 = parentThs;  // get parent from function call
            deref(&stdpointer, &en1);
            en = exprNode(en_add, clsThs, intNode(en_c_i, sp->offset));
            deref(&stdpointer, &en);
            en = exprNode(en_assign, en, en1);
        }
        else if (!strcmp(sp->name, "$this"))
        {
            if (!lambdas->next || !lambdas->captureThis)
            {
                en1 = parentThs;  // get parent from function call
            }
            else
            {
                SYMBOL* parent = search("$parent", lambdas->cls->tp->syms);
                en1 = varNode(en_auto, cls);
                deref(&stdpointer, &en1);
                en1 = exprNode(en_add, en1, intNode(en_c_i, parent->offset));
            }
            deref(&stdpointer, &en1);
            en = exprNode(en_add, clsThs, intNode(en_c_i, sp->offset));
            deref(&stdpointer, &en);
            en = exprNode(en_assign, en, en1);
        }
        else if (sp->lambdaMode)
        {
            LAMBDASP* lsp = (LAMBDASP*)search(sp->name, lambdas->captured);
            if (lsp)
            {
                en1 = exprNode(en_add, clsThs, intNode(en_c_i, sp->offset));
                if (sp->lambdaMode == cmExplicitValue)
                {
                    SYMBOL* capture = lsp->parent;
                    TYPE* ctp = capture->tp;
                    if (isstructured(ctp))
                    {
                        if (!callConstructorParam(&ctp, &en1, sp->tp, sp->init->exp, true, false, true, false))
                            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, lsp->sym);
                        en = en1;
                    }
                    else
                    {
                        deref(ctp, &en1);
                        en = exprNode(en_assign, en1, sp->init->exp);
                    }
                }
                else if (sp->lambdaMode == cmRef)
                {
                    SYMBOL* capture = lsp->parent;
                    deref(&stdpointer, &en1);
                    if (capture->lambdaMode)
                    {
                        en = parentThs;
                        deref(&stdpointer, &en);
                        en = exprNode(en_add, en, intNode(en_c_i, capture->offset));
                    }
                    else  // must be an sc_auto
                    {
                        en = varNode(en_auto, capture);
                    }
                    if (isref(capture->tp))
                    {
                        deref(&stdpointer, &en);
                    }
                    en = exprNode(en_assign, en1, en);
                }
                else  // cmValue
                {
                    SYMBOL* capture = lsp->parent;
                    TYPE* ctp = capture->tp;
                    if (capture->lambdaMode)
                    {
                        en = parentThs;
                        deref(&stdpointer, &en);
                        en = exprNode(en_add, en, intNode(en_c_i, capture->offset));
                    }
                    else  // must be an sc_auto
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
                        if (!callConstructorParam(&ctp, &en1, ctp, en, true, false, true, false))
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
    *cur = clsThs;  // this expression will be used in copy constructors, or discarded if unneeded
    return rv;
}
LEXEME* expression_lambda(LEXEME* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags)
{

    LAMBDA* self;
    SYMBOL *vpl, *ths;
    HASHREC* hrl;
    TYPE* ltp;
    STRUCTSYM ssl;
    if (funcsp)
        funcsp->noinline = true;
    IncGlobalFlag();
    self = (LAMBDA *) Alloc(sizeof(LAMBDA));
    ltp = (TYPE *)Alloc(sizeof(TYPE));
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
    self->isMutable = false;
    self->captureThis = false;
    self->cls = makeID(sc_type, ltp, NULL, LambdaName());
    ltp->sp = self->cls;
    SetLinkerNames(self->cls, lk_cdecl);
    self->cls->islambda = true;
    self->cls->structAlign = getAlign(sc_global, &stdpointer);
    self->func = makeID(sc_member, ltp, NULL, overloadNameTab[CI_FUNC]);
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
        self->lthis = ((SYMBOL*)funcsp->tp->syms->table[0]->p);
    }
    lex = getsym();  // past [
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
        else if (MATCHKW(lex, andx))
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
                        self->captureThis = true;
                        lambda_capture(NULL, cmThis, true);
                    }
                    continue;
                }
                else if (MATCHKW(lex, andx))
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
                    LEXEME* idlex = lex;
                    SYMBOL* sp;
                    LAMBDA* current;
                    lex = getsym();
                    if (MATCHKW(lex, assign))
                    {
                        TYPE* tp = NULL;
                        EXPRESSION* exp;
                        lex = getsym();
                        lex = expression_no_comma(lex, funcsp, NULL, &tp, &exp, NULL, 0);
                        if (!tp)
                        {
                            error(ERR_EXPRESSION_SYNTAX);
                        }
                        else
                        {
                            SYMBOL* sp = makeID(sc_auto, tp, NULL, idlex->value.s.a);
                            initInsert(&sp->init, tp, exp, 0, true);
                            lambda_capture(sp, cmExplicitValue, true);
                            if (localMode == cmRef)
                            {
                                error(ERR_INVALID_LAMBDA_CAPTURE_MODE);
                            }
                        }
                    }
                    else
                    {
                        sp = search(idlex->value.s.a, localNameSpace->syms);
                        current = lambdas;
                        while (current && !sp)
                        {
                            sp = search(idlex->value.s.a, current->oldSyms);
                            current = current->next;
                        }
                        if (sp)
                        {
                            if (sp->packed)
                            {
                                if (!MATCHKW(idlex, ellipse))
                                    error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                else
                                    idlex = getsym();
                            }
                            if (sp->packed)
                            {
                                int n;
                                TEMPLATEPARAMLIST* templateParam = sp->tp->templateParam->p->byPack.pack;
                                HASHREC* hr;
                                for (n = 0; templateParam; templateParam = templateParam->next, n++)
                                    ;
                                hr = funcsp->tp->syms->table[0];
                                while (hr && ((SYMBOL*)hr->p) != sp)
                                    hr = hr->next;
                                while (hr && n)
                                {
                                    lambda_capture((SYMBOL*)hr->p, localMode, true);
                                    hr = hr->next;
                                    n--;
                                }
                            }
                            else
                            {
                                lambda_capture(sp, localMode, true);
                            }
                        }
                        else
                            errorstr(ERR_UNDEFINED_IDENTIFIER, idlex->value.s.a);
                    }
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
        TYPE* tpx = &stdvoid;
        HASHREC* hr;
        lex = getFunctionParams(lex, NULL, &self->func, &tpx, false, sc_auto);
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
            SYMBOL* sym = (SYMBOL*)hr->p;
            if (sym->init)
            {
                error(ERR_CANNOT_DEFAULT_PARAMETERS_WITH_LAMBDA);
            }
            hr = hr->next;
        }
        if (MATCHKW(lex, kw_mutable))
        {
            HASHREC* hr = self->captured->table[0];
            while (hr)
            {
                LAMBDASP* lsp = (LAMBDASP*)hr->p;
                if (lsp->sym->lambdaMode == cmValue)
                {
                    lsp->sym->tp = basetype(lsp->sym->tp);
                }
                hr = hr->next;
            }
            self->isMutable = true;

            lex = getsym();
        }
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (MATCHKW(lex, pointsto))
        {
            lex = getsym();
            lex = get_type_id(lex, &self->functp, funcsp, sc_cast, false, true);
            if (!self->functp)
            {
                error(ERR_TYPE_NAME_EXPECTED);
                self->functp = &stdint;
            }
        }
    }
    else
    {
        TYPE* tp1 = (TYPE *)Alloc(sizeof(TYPE));
        SYMBOL* spi;
        tp1->type = bt_func;
        tp1->size = getSize(bt_pointer);
        tp1->btp = &stdvoid;
        tp1->rootType = tp1;
        tp1->sp = self->func;
        self->func->tp = tp1;
        spi = makeID(sc_parameter, tp1, NULL, AnonymousName());
        spi->anonymous = true;
        spi->tp = (TYPE *)Alloc(sizeof(TYPE));
        spi->tp->type = bt_void;
        spi->tp->rootType = spi->tp;
        insert(spi, localNameSpace->syms);
        SetLinkerNames(spi, lk_cpp);
        self->func->tp->syms = localNameSpace->syms;
        self->funcargs = self->func->tp->syms->table[0];
        self->func->tp->syms->table[0] = NULL;
    }
    basetype(self->func->tp)->btp = self->functp;
    injectThisPtr(lambdas->func, basetype(lambdas->func->tp)->syms);
    lambdas->func->tp->btp = self->functp;
    lambdas->func->tp->rootType = lambdas->func->tp;
    lambdas->func->linkage = lk_virtual;
    lambdas->func->isInline = true;
    lambdas->templateFunctions = lambda_get_template_state(lambdas->func);
    ssl.str = self->cls;
    ssl.tmpl = NULL;
    addStructureDeclaration(&ssl);
    ths = makeID(sc_member, &stdpointer, NULL, "$this");
    lambda_insert(ths, lambdas);
    if (MATCHKW(lex, begin))
    {
        lex = getDeferredData(lex, self->func, true);
        if (!lambdas->templateFunctions)
        {
            LEXEME* lex1 = SetAlternateLex(self->func->deferredCompile);
            SetLinkerNames(self->func, lk_cdecl);
            body(lex1, self->func);
            lex1 = self->func->deferredCompile;
            while (lex1)
            {
                lex1->registered = false;
                lex1 = lex1->next;
            }
            SetAlternateLex(NULL);
        }
    }
    else
    {
        error(ERR_LAMBDA_FUNCTION_BODY_EXPECTED);
    }
    dropStructureDeclaration();
    localNameSpace->syms = self->oldSyms;
    localNameSpace->tags = self->oldTags;
    finishClass();
    *exp = createLambda(0);
    *tp = lambdas->cls->tp;
    lambdas = lambdas->next;
    if (lambdas)
        lambdas->prev = NULL;
    DecGlobalFlag();
    return lex;
}
