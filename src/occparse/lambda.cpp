/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
#include "mangle.h"
#include "initbackend.h"
#include "stmt.h"
#include "occparse.h"
#include "expr.h"
#include "help.h"
#include "lex.h"
#include "declcpp.h"
#include "OptUtils.h"
#include "declare.h"
#include "memory.h"
#include "inline.h"
#include "init.h"
#include "declcons.h"
#include "beinterf.h"
#include "symtab.h"

namespace Parser
{
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

LAMBDA* lambdas;

static char lambdaQualifier[260];
static int lambdaIndex;
static SYMBOL* lambdaFunc;
static SymbolTableFactory<LAMBDASP> lambdaFactory;
void lambda_init(void)
{
    lambdaFactory.Reset();
    lambdaIndex = 1;
    lambdas = NULL;
}
static char* LambdaName(void)
{
    char buf[256];
    if (lambdaIndex == 1)
    {
        char* p = (char *)strrchr(infile, '\\');
        if (!p)
        {
            p = (char *)strrchr(infile, '/');
            if (!p)
                p = infile;
            else
                p++;
        }
        else
            p++;

        sprintf(lambdaQualifier, "__%s__%08x", p, identityValue);
        while ((p = (char *)strchr(lambdaQualifier, '.')) != 0)
            *p = '_';
    }
    Optimizer::my_sprintf(buf, "@LambdaClosure%d%s", lambdaIndex++, lambdaQualifier);
    return litlate(buf);
}
static const char* AnonymousLambdaName(void)
{
    char* name = LambdaName();
    char buf[256];
    Optimizer::my_sprintf(buf, "%s_thunk", name);
    return litlate(buf);
}
static void lambda_insert(SYMBOL* sym, LAMBDA* lambdas)
{
    int align = getAlign(sc_member, sym->tp);
    if (align && lambdas->cls->tp->size % align)
        lambdas->cls->tp->size += align - lambdas->cls->tp->size % align;
    if (align > lambdas->cls->sb->attribs.inheritable.structAlign)
        lambdas->cls->sb->attribs.inheritable.structAlign = align;
    sym->sb->parentClass = lambdas->cls;
    sym->sb->offset = lambdas->cls->tp->size;
    lambdas->cls->tp->syms->Add(sym);
    lambdas->cls->tp->size += sym->tp->size;
}
static TYPE* lambda_type(TYPE* tp, enum e_cm mode)
{
    if (mode == cmRef)
    {
        tp = basetype(tp);
        tp = MakeType(bt_lref, isref(tp) ? tp->btp : tp);
    }
    else  // cmValue
    {
        tp = basetype(tp);
        if (isref(tp))
        {
            tp = tp->btp;
        }
        tp = basetype(tp);
        if (!lambdas->isMutable)
        {
            tp = MakeType(bt_const, tp);
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
            SYMBOL* lthis = lambdas->lthis;
            SYMBOL* base = basetype(lambdas->lthis->tp)->btp->sp;
            if (lthis->sb->mainsym)
                lthis = lthis->sb->mainsym;
            if (base->sb->mainsym)
                base = base->sb->mainsym;
            if (!sym || (lambdas->lthis && lthis == base))
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
            if (sym->sb->parent != lambdas->func)
            {
                if (sym->sb->storage_class != sc_auto && sym->sb->storage_class != sc_parameter)
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
                        LAMBDASP* lsp = current->captured->search(sym->name);
                        if (lsp)
                        {
                            sym2 = lsp->sp;
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
                        SYMBOL* sp = nullptr;
                        while (current->next && !sp)
                        {
                            sp = current->oldSyms->search(sym->name);
                            if (!sp)
                                current = current->next;
                        }
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
                    while (current)
                    {
                        // we are replicating captures through intermediate lambdas
                        // to make it easier to handle inner lambda creation.
                        LAMBDASP* ins = Allocate<LAMBDASP>();
                        ins->name = sym->name;
                        ins->parent = sym;
                        sym = CopySymbol(sym);
                        sym->sb->lambdaMode = mode == cmNone ? current->captureMode : mode;
                        sym->tp = lambda_type(sym->tp, sym->sb->lambdaMode);
                        sym->sb->storage_class = sc_member;
                        sym->sb->parent = current->func;
                        sym->sb->access = ac_private;
                        if (mode != cmExplicitValue)
                            sym->sb->init = NULL;
                        lambda_insert(sym, current);
                        ins->sp = sym;
                        ins->enclosing = current;
                        current->captured->baseInsert(ins);
                        current = current->prev;
                    }
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
    SYMBOL* func = CopySymbol(funcin);
    tp = &func->tp;
    while (tp_in)
    {
        *tp = CopyType(tp_in);
        tp_in = tp_in->btp;
        tp = &(*tp)->btp;
    }
    UpdateRootTypes(func->tp);
    basetype(func->tp)->syms = symbols.CreateSymbolTable();
    auto dest = basetype(func->tp)->syms;
    for (auto sym : *basetype(funcin->tp)->syms)
    {
        dest->Add(CopySymbolfalse(sym));
    }
    return func->tp;
}
static void cloneTemplateParams(SYMBOL* func)
{
    TEMPLATEPARAMLIST** tplp = &func->templateParams;
    int index = 0;
    (*tplp) = Allocate<TEMPLATEPARAMLIST>();
    (*tplp)->p = Allocate<TEMPLATEPARAM>();
    (*tplp)->p->type = kw_new;
    tplp = &(*tplp)->next;
    for (auto arg : *basetype(func->tp)->syms)
    {
        if (!arg->sb->thisPtr)
        {
            arg->tp = CopyType(arg->tp);
            UpdateRootTypes(arg->tp);
            (*tplp) = Allocate<TEMPLATEPARAMLIST>();
            (*tplp)->p = Allocate<TEMPLATEPARAM>();
            (*tplp)->argsym = Allocate<SYMBOL>();
            *(*tplp)->argsym = *arg;
            (*tplp)->argsym->sb = nullptr;
            (*tplp)->p->type = kw_typename;
            arg->tp->templateParam = *tplp;
            arg->templateParams = *tplp;
            tplp = &(*tplp)->next;
        }
    }
}
static void convertCallToTemplate(SYMBOL* func)
{
    TEMPLATEPARAMLIST** tplholder;
    func->templateParams = Allocate<TEMPLATEPARAMLIST>();
    func->templateParams->p = Allocate<TEMPLATEPARAM>();
    func->templateParams->p->type = kw_new;
    if (isautotype(lambdas->functp))
        basetype(func->tp)->btp = &stdauto;  // convert return type back to auto
    tplholder = &func->templateParams->next;
    int index = 0;
    for (auto arg : *basetype(func->tp)->syms)
    {
        if (isautotype(arg->tp))
        {
            (*tplholder) = Allocate<TEMPLATEPARAMLIST>();
            (*tplholder)->p = Allocate<TEMPLATEPARAM>();
            (*tplholder)->argsym = Allocate<SYMBOL>();
            *(*tplholder)->argsym = *arg;
            (*tplholder)->argsym->sb = nullptr;
            arg->tp = MakeType(bt_templateparam);
            arg->tp->templateParam = *tplholder;
            (*tplholder)->p->type = kw_typename;
            (*tplholder)->p->index = index++;
            tplholder = &(*tplholder)->next;
        }
    }
    func->sb->templateLevel = templateNestingCount;
    func->sb->parentTemplate = func;
}
static SYMBOL* createPtrToCaller(SYMBOL* self)
{
    INITLIST** argptr;
    FUNCTIONCALL* params = Allocate<FUNCTIONCALL>();
    TYPE* args = cloneFuncType(lambdas->func);
    SYMBOL* func = makeID(sc_static, args, NULL, "___ptrcall");
    BLOCKDATA block1, block2;
    STATEMENT* st;
    basetype(args)->sp = func;
    basetype(args)->btp = basetype(lambdas->func->tp)->btp;
    func->sb->parentClass = lambdas->cls;
    func->sb->attribs.inheritable.linkage = lk_cdecl;
    func->sb->attribs.inheritable.isInline = true;
    func->sb->storage_class = sc_static;
    func->sb->access = ac_private;

    memset(&block1, 0, sizeof(BLOCKDATA));
    memset(&block2, 0, sizeof(BLOCKDATA));
    insertFunc(lambdas->cls, func);
    func->tp->syms->remove(func->tp->syms->begin()); // elide this pointer
    st = stmtNode(NULL, &block2, isstructured(basetype(lambdas->func->tp)->btp) ? st_expr : st_return);
    st->select = varNode(en_func, NULL);
    st->select->v.func = params;
    st->select = exprNode(en_thisref, st->select, NULL);

    argptr = &params->arguments;
    for (auto sym : *args->syms)
    {
        if (sym->tp->type == bt_void)
            break;
        sym = CopySymbol(sym);
        sym->sb->offset -= getSize(bt_pointer);
        *argptr = Allocate<INITLIST>();
        if (isstructured(sym->tp) && !isref(sym->tp))
        {
            SYMBOL* sym2 = anonymousVar(sc_auto, sym->tp)->v.sp;
            sym2->sb->stackblock = true;
            (*argptr)->exp = varNode(en_auto, sym2);  // DAL MODIFIED
        }
        else
        {
            (*argptr)->exp = varNode(en_auto, sym);
            deref(sym->tp, &(*argptr)->exp);
        }
        (*argptr)->tp = sym->tp;
        argptr = &(*argptr)->next;
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
    func->sb->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    func->sb->inlineFunc.stmt->lower = block1.head;
    func->sb->inlineFunc.stmt->blockTail = block1.blockTail;
    func->sb->inlineFunc.syms = basetype(args)->syms;
    if (lambdas->templateFunctions)
    {
        LEXLIST* lex1;
        int l = 0;
        if (isautotype(lambdas->functp))
            basetype(func->tp)->btp = &stdauto;  // convert return type back to auto
        cloneTemplateParams(func);
        func->sb->templateLevel = templateNestingCount;
        func->sb->parentTemplate = func;
        std::string val = "{return ___self->operator()(";
        for (auto sym : *basetype(lambdas->func->tp)->syms)
        {
            if (!sym->sb->thisPtr)
            {
                val += sym->name;
            }
        }
        val += ");} ;";
        SetAlternateParse(true, val);
        lex1 = getsym();
        getDeferredData(lex1, &func->sb->deferredCompile, true);
        SetAlternateParse(false, "");
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
    SYMBOL* func = makeID(sc_member, MakeType(bt_func, MakeType(bt_pointer, args)), NULL, overloadNameTab[CI_CAST]);
    BLOCKDATA block1, block2;
    STATEMENT* st;
    EXPRESSION* exp;
    SYMBOL* sym = makeID(sc_parameter, &stdvoid, NULL, AnonymousName());
    func->tp->syms = symbols.CreateSymbolTable();
    func->sb->parentClass = lambdas->cls;
    func->sb->attribs.inheritable.linkage4 = lk_virtual;
    func->sb->attribs.inheritable.isInline = true;
    func->sb->storage_class = sc_member;
    func->sb->castoperator = true;
    func->tp->syms = symbols.CreateSymbolTable();
    func->tp->syms->Add(sym);
    injectThisPtr(func, func->tp->syms);
    func->sb->parentClass = lambdas->cls;
    memset(&block1, 0, sizeof(BLOCKDATA));
    memset(&block2, 0, sizeof(BLOCKDATA));
    insertFunc(lambdas->cls, func);
    // assign ___self = this
    st = stmtNode(NULL, &block2, st_expr);
    st->select = varNode(en_auto, (SYMBOL*)basetype(func->tp)->syms->front());
    deref(&stdpointer, &st->select);
    st->select = exprNode(en_assign, varNode(en_global, self), st->select);
    deref(&stdpointer, &st->select->left);
    // return pointer to ptrtocaller
    st = stmtNode(NULL, &block2, st_return);
    st->select = varNode(en_pc, caller);
    st = stmtNode(NULL, &block1, st_block);
    st->lower = block2.head;
    st->blockTail = block2.blockTail;
    func->sb->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    func->sb->inlineFunc.stmt->lower = block1.head;
    func->sb->inlineFunc.stmt->blockTail = block1.blockTail;
    func->sb->inlineFunc.syms = symbols.CreateSymbolTable();
    if (lambdas->templateFunctions)
    {
        LEXLIST* lex1;
        TEMPLATEPARAMLIST *tpl, **tplp, **tplp2;
        FUNCTIONCALL* f = Allocate<FUNCTIONCALL>();
        INITLIST** args = &f->arguments;
        func->templateParams = caller->templateParams;
        func->sb->templateLevel = templateNestingCount;
        func->sb->parentTemplate = func;
        basetype(func->tp)->btp = MakeType(bt_templatedecltype);
        basetype(func->tp)->btp->templateDeclType = exprNode(en_func, NULL, NULL);
        basetype(func->tp)->btp->templateDeclType->v.func = f;
        for (auto sym : *basetype(caller->tp)->syms)
        {
            if (sym->sb->thisPtr)
            {
                f->thistp = sym->tp;
                f->thisptr = intNode(en_c_i, 0);
            }
            else
            {
                (*args) = Allocate<INITLIST>();
                (*args)->tp = sym->tp;
                (*args)->exp = intNode(en_c_i, 0);
                args = &(*args)->next;
            }
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
        std::string val = "{ ___self=this;return &___ptrcall;} ;";

        SetAlternateParse(true, val);
        lex1 = getsym();
        getDeferredData(lex1, &func->sb->deferredCompile, true);
        SetAlternateParse(false, "");
        func->sb->templateLevel = templateNestingCount;
        func->sb->parentTemplate = func;
    }
    else
    {
        InsertInline(func);
    }
    UpdateRootTypes(func->tp);
}
static bool lambda_get_template_state(SYMBOL* func)
{
    for (auto sym : *basetype(func->tp)->syms)
    {
        if (isautotype(sym->tp))
            return true;
    }
    return false;
}
static void finishClass(void)
{
    TYPE* tps = MakeType(bt_pointer, lambdas->cls->tp);
    SYMBOL* self = makeID(sc_static, tps, NULL, "___self");
    SYMLIST* lst;
    self->sb->access = ac_private;

    if (lambdas->captureThis && lambdas->next)
    {
        SYMBOL* parent = makeID(sc_member, &stdpointer, NULL, "$parent");
        lambda_insert(parent, lambdas);
    }
    self->sb->label = Optimizer::nextLabel++;
    self->sb->parentClass = lambdas->cls;
    SetLinkerNames(self, lk_cdecl);
    lambdas->cls->tp->syms->Add(self);
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
        SYMBOL* ths = (SYMBOL*)lambdas->func->tp->syms->front();
        lambdas->func->tp = MakeType(bt_const, lambdas->func->tp);
        basetype(ths->tp)->btp = MakeType(bt_const, basetype(ths->tp->btp));
        UpdateRootTypes(ths->tp);
    }
}
static EXPRESSION* createLambda(bool noinline)
{
    EXPRESSION *rv = NULL, **cur = &rv;
    EXPRESSION *clsThs, *parentThs;
    SYMBOL* cls = makeID(lambdas->enclosingFunc ? sc_auto : sc_localstatic, lambdas->cls->tp, NULL,
                         lambdas->enclosingFunc ? AnonymousName() : AnonymousLambdaName());
    SetLinkerNames(cls, lk_cdecl);
    cls->sb->allocate = true;
    if (lambdas->enclosingFunc)
    {
        localNameSpace->valueData->syms->Add(cls);
        clsThs = varNode(en_auto, cls);  // this ptr
    }
    else
    {
        globalNameSpace->valueData->syms->Add(cls);  // well if we could put this as an auto in the init func that would be good
                                                        // but there is no way to do that here...
        clsThs = varNode(en_global, cls);               // this ptr
        cls->sb->label = Optimizer::nextLabel++;
        insertInitSym(cls);
    }
    {
        INITIALIZER* init = NULL;
        EXPRESSION* exp = clsThs;
        callDestructor(cls, NULL, &exp, NULL, true, false, false, true);
        initInsert(&init, cls->tp, exp, 0, true);
        if (cls->sb->storage_class != sc_auto)
        {
            insertDynamicDestructor(cls, init);
        }
        else
        {
            cls->sb->dest = init;
        }
    }
    parentThs = varNode(en_auto, (SYMBOL*)basetype(lambdas->func->tp)->syms->front());  // this ptr
    for (auto sp : *lambdas->cls->tp->syms)
    {
        EXPRESSION *en = NULL, *en1 = NULL;
        if (!strcmp(sp->name, "$parent"))
        {
            en1 = parentThs;  // get parent from function call
            deref(&stdpointer, &en1);
            en = exprNode(en_add, clsThs, intNode(en_c_i, sp->sb->offset));
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
                SYMBOL* parent = lambdas->cls->tp->syms->search("$parent");
                en1 = varNode(en_auto, cls);
                deref(&stdpointer, &en1);
                en1 = exprNode(en_add, en1, intNode(en_c_i, parent->sb->offset));
            }
            deref(&stdpointer, &en1);
            en = exprNode(en_add, clsThs, intNode(en_c_i, sp->sb->offset));
            deref(&stdpointer, &en);
            en = exprNode(en_assign, en, en1);
        }
        else if (sp->sb->lambdaMode)
        {
            LAMBDASP* lsp = lambdas->captured->search(sp->name);
            if (lsp)
            {
                en1 = exprNode(en_add, clsThs, intNode(en_c_i, sp->sb->offset));
                if (sp->sb->lambdaMode == cmExplicitValue)
                {
                    SYMBOL* capture = lsp->parent;
                    TYPE* ctp = capture->tp;
                    if (isstructured(ctp))
                    {
                        if (!callConstructorParam(&ctp, &en1, sp->tp, sp->sb->init->exp, true, false, true, false, true))
                            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, lsp->sp);
                        en = en1;
                    }
                    else
                    {
                        deref(ctp, &en1);
                        en = exprNode(en_assign, en1, sp->sb->init->exp);
                    }
                }
                else if (sp->sb->lambdaMode == cmRef)
                {
                    SYMBOL* capture = lsp->parent;
                    deref(&stdpointer, &en1);
                    if (capture->sb->lambdaMode)
                    {
                        en = parentThs;
                        deref(&stdpointer, &en);
                        en = exprNode(en_add, en, intNode(en_c_i, capture->sb->offset));
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
                    if (capture->sb->lambdaMode)
                    {
                        en = parentThs;
                        deref(&stdpointer, &en);
                        en = exprNode(en_add, en, intNode(en_c_i, capture->sb->offset));
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
                        if (!callConstructorParam(&ctp, &en1, ctp, en, true, false, true, false, true))
                            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, lsp->sp);
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
    }
    *cur = copy_expression(clsThs);  // this expression will be used in copy constructors, or discarded if unneeded
    return rv;
}
LEXLIST* expression_lambda(LEXLIST* lex, SYMBOL* funcsp, TYPE* atp, TYPE** tp, EXPRESSION** exp, int flags)
{
    LAMBDA* self;
    SYMBOL *vpl, *ths;
    SYMLIST* hrl;
    TYPE* ltp;
    STRUCTSYM ssl;
    if (funcsp)
        funcsp->sb->noinline = true;
    self = Allocate<LAMBDA>();
    ltp = MakeType(bt_struct);
    ltp->syms = symbols.CreateSymbolTable();
    ltp->tags = symbols.CreateSymbolTable();
    ltp->size = 0;
    self->captured = lambdaFactory.CreateSymbolTable();
    self->oldSyms = localNameSpace->valueData->syms;
    self->oldTags = localNameSpace->valueData->tags;
    self->index = lambdaIndex;
    self->captureMode = cmNone;
    self->isMutable = false;
    self->captureThis = false;
    self->cls = makeID(sc_type, ltp, NULL, LambdaName());
    ltp->sp = self->cls;
    SetLinkerNames(self->cls, lk_cdecl);
    self->cls->sb->islambda = true;
    self->cls->sb->attribs.inheritable.structAlign = getAlign(sc_global, &stdpointer);
    self->func = makeID(sc_member, ltp, NULL, overloadNameTab[CI_FUNC]);
    self->func->sb->parentClass = self->cls;
    self->functp = &stdauto;
    self->enclosingFunc = theCurrentFunc;
    if (lambdas)
        lambdas->prev = self;
    self->next = lambdas;
    lambdas = self;

    localNameSpace->valueData->syms = symbols.CreateSymbolTable();
    localNameSpace->valueData->tags = symbols.CreateSymbolTable();
    if (lambdas->next)
    {
        self->lthis = lambdas->next->lthis;
    }
    else if (funcsp && funcsp->sb->parentClass)
    {
        self->lthis = ((SYMBOL*)funcsp->tp->syms->front());
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
                    LEXLIST* idlex = lex;
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
                            SYMBOL* sp = makeID(sc_auto, tp, NULL, idlex->data->value.s.a);
                            initInsert(&sp->sb->init, tp, exp, 0, true);
                            lambda_capture(sp, cmExplicitValue, true);
                            if (localMode == cmRef)
                            {
                                error(ERR_INVALID_LAMBDA_CAPTURE_MODE);
                            }
                        }
                    }
                    else
                    {
                        sp = localNameSpace->valueData->syms->search(idlex->data->value.s.a);
                        current = lambdas;
                        while (current && !sp)
                        {
                            sp = current->oldSyms->search(idlex->data->value.s.a);
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
                                for (n = 0; templateParam; templateParam = templateParam->next, n++)
                                    ;
                                auto it = funcsp->tp->syms->begin();
                                auto ite= funcsp->tp->syms->end();
                                while (it != ite && *it != sp)
                                    ++it;
                                while (it != ite && n)
                                {
                                    lambda_capture(*it, localMode, true);
                                    ++it;
                                    n--;
                                }
                            }
                            else
                            {
                                lambda_capture(sp, localMode, true);
                            }
                        }
                        else
                            errorstr(ERR_UNDEFINED_IDENTIFIER, idlex->data->value.s.a);
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
        lex = getFunctionParams(lex, NULL, &self->func, &tpx, false, sc_auto, false);
        if (!self->func->tp->syms)
        {
            errorstr(ERR_MISSING_TYPE_FOR_PARAMETER, "undefined");
            *tp = &stdint;
            *exp = intNode(en_c_i, 0);
            return lex;
        }
        for (auto sym : *self->func->tp->syms)
        {
            if (sym->sb->init)
            {
                error(ERR_CANNOT_DEFAULT_PARAMETERS_WITH_LAMBDA);
            }
        }
        if (MATCHKW(lex, kw_mutable))
        {
            for (auto lsp : *self->captured)
            {
                if (lsp->sp->sb->lambdaMode == cmValue)
                {
                    lsp->sp->tp = basetype(lsp->sp->tp);
                }
            }
            self->isMutable = true;

            lex = getsym();
        }
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (MATCHKW(lex, pointsto))
        {
            lex = getsym();
            lex = get_type_id(lex, &self->functp, funcsp, sc_cast, false, true, false);
            if (!self->functp)
            {
                error(ERR_TYPE_NAME_EXPECTED);
                self->functp = &stdint;
            }
        }
    }
    else
    {
        self->func->tp = MakeType(bt_func, &stdvoid);
        self->func->tp->sp = self->func;
        SYMBOL* spi;
        spi = makeID(sc_parameter, self->func->tp, NULL, AnonymousName());
        spi->sb->anonymous = true;
        spi->tp = MakeType(bt_void);
        localNameSpace->valueData->syms->Add(spi);
        SetLinkerNames(spi, lk_cpp);
        self->func->tp->syms = localNameSpace->valueData->syms;
        self->func->tp->syms->Add(makeID(sc_parameter, &stdvoid, NULL, AnonymousName()));
    }
    basetype(self->func->tp)->btp = self->functp;
    injectThisPtr(lambdas->func, basetype(lambdas->func->tp)->syms);
    lambdas->func->tp->btp = self->functp;
    lambdas->func->tp->rootType = lambdas->func->tp;
    lambdas->func->sb->attribs.inheritable.linkage4 = lk_virtual;
    lambdas->func->sb->attribs.inheritable.isInline = true;
    lambdas->templateFunctions = lambda_get_template_state(lambdas->func);
    ssl.str = self->cls;
    ssl.tmpl = NULL;
    addStructureDeclaration(&ssl);
    ths = makeID(sc_member, &stdpointer, NULL, "$this");
    SetLinkerNames(ths, lk_cdecl);
    lambda_insert(ths, lambdas);
    if (MATCHKW(lex, begin))
    {
        lex = getDeferredData(lex, &self->func->sb->deferredCompile, true);
        if (!lambdas->templateFunctions)
        {
            LEXLIST* lex1 = SetAlternateLex(self->func->sb->deferredCompile);
            SetLinkerNames(self->func, lk_cdecl);
            body(lex1, self->func);
            lex1 = self->func->sb->deferredCompile;
            while (lex1)
            {
                lex1->data->registered = false;
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
    localNameSpace->valueData->syms = self->oldSyms;
    localNameSpace->valueData->tags = self->oldTags;
    finishClass();
    *exp = createLambda(0);
    *tp = lambdas->cls->tp;
    lambdas = lambdas->next;
    if (lambdas)
        lambdas->prev = NULL;
    return lex;
}
}  // namespace Parser