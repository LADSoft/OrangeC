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

#include "compiler.h"
#include "ccerr.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "mangle.h"
#include "initbackend.h"
#include "stmt.h"
#include "occparse.h"
#include "expr.h"
#include "lex.h"
#include "help.h"
#include "declcpp.h"
#include "OptUtils.h"
#include "declare.h"
#include "memory.h"
#include "inline.h"
#include "init.h"
#include "declcons.h"
#include "beinterf.h"
#include "symtab.h"
#include "ListFactory.h"
#include "types.h"

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

std::list<LAMBDA*> lambdas;

static char lambdaQualifier[260];
static int lambdaIndex;
static SYMBOL* lambdaFunc;
static SymbolTableFactory<LAMBDASP> lambdaFactory;
void lambda_init(void)
{
    lambdaFactory.Reset();
    lambdaIndex = 1;
    lambdas.clear();
}
static char* LambdaName(void)
{
    char buf[256];
    if (lambdaIndex == 1)
    {
        char* p = (char*)strrchr(infile, '\\');
        if (!p)
        {
            p = (char*)strrchr(infile, '/');
            if (!p)
                p = infile;
            else
                p++;
        }
        else
            p++;

        sprintf(lambdaQualifier, "__%s__%08x", p, identityValue);
        while ((p = (char*)strchr(lambdaQualifier, '.')) != 0)
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
static void lambda_insert(SYMBOL* sym, LAMBDA* current)
{
    int align = getAlign(StorageClass::member_, sym->tp);
    if (align && current->cls->tp->size % align)
        current->cls->tp->size += align - current->cls->tp->size % align;
    if (align > current->cls->sb->attribs.inheritable.structAlign)
        current->cls->sb->attribs.inheritable.structAlign = align;
    sym->sb->parentClass = current->cls;
    sym->sb->offset = current->cls->tp->size;
    current->cls->tp->syms->Add(sym);
    current->cls->tp->size += sym->tp->size;
}
static Type* lambda_type(Type* tp, e_cm mode)
{
    if (mode == cmRef)
    {
        tp = tp->BaseType();
        tp = Type::MakeType(BasicType::lref_, tp->IsRef() ? tp->btp : tp);
    }
    else  // cmValue
    {
        tp = tp->BaseType();
        if (tp->IsRef())
        {
            tp = tp->btp;
        }
        tp = tp->BaseType();
        if (!lambdas.front()->isMutable)
        {
            tp = Type::MakeType(BasicType::const_, tp);
        }
    }
    return tp;
}
SYMBOL* lambda_capture(SYMBOL* sym, e_cm mode, bool isExplicit)
{
    if (lambdas.size())
    {
        auto cile = lambdas.end();
        auto cilb = lambdas.begin();
        if (mode == cmThis)
        {
            SYMBOL* lthis = lambdas.front()->lthis;
            SYMBOL* base = lthis;
            if (lthis->sb->mainsym)
                lthis = lthis->sb->mainsym;
            if (base->sb->mainsym)
                base = base->sb->mainsym;
            if (!sym || (lambdas.front()->lthis && lthis == base))
            {
                if (lambdas.front()->captureThis)
                {
                    bool errorflg = false;
                    // have to try to replicate the symbol into the current context
                    for (auto cil = cilb ; cil != cile; ++cil )
                    {
                        if ((*cil)->captureMode == cmNone && !lambdas.front()->captureThis)
                        {
                            if (isExplicit)
                            {
                                // this checked the beginning of the list
                                if (cil != lambdas.begin())
                                {
                                    errorflg = true;
                                    errorstr(ERR_EXPLICIT_CAPTURE_BLOCKED, "this");
                                    break;
                                }
                            }
                            else if (cil != cilb)
                            {
                                errorflg = true;
                                errorstr(ERR_IMPLICIT_CAPTURE_BLOCKED, "this");
                                break;
                            }
                        }
                        else if ((*cil)->captureMode == cmValue && isExplicit && !(*cil)->thisByVal)
                        {
                            errorflg = true;
                            errorstr(ERR_EXPLICIT_CAPTURE_BLOCKED, "this");
                            break;
                        }
                    }
                    if (!errorflg)
                    {
                        for (auto cil = cilb; cil != cile; ++cil)
                        {
                            (*cil)->captureThis = true;
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
            if (sym->sb->parent != lambdas.front()->func)
            {
                if (sym->sb->storage_class != StorageClass::auto_ && sym->sb->storage_class != StorageClass::parameter_)
                {
                    error(ERR_MUST_CAPTURE_AUTO_VARIABLE);
                }
                else
                {
                    auto current = cilb;
                    SYMBOL* sym2 = NULL;
                    LAMBDA* check;
                    for ( ; current != cile; ++current)
                    {
                        LAMBDASP* lsp = search((*current)->captured, sym->name);
                        if (lsp)
                        {
                            sym2 = lsp->sp;
                            break;
                        }
                    }
                    if (sym2)
                    {
                        if (cilb == current && isExplicit)
                        {
                            error(ERR_CAPTURE_ITEM_LISTED_MULTIPLE_TIMES);
                        }
                        sym = sym2;
                    }
                    else
                    {
                        auto ilt = cilb;
                        current = cilb;
                        SYMBOL* sp = nullptr;
                        for  ( ; ++ilt != cile ; ++ current)
                        {
                            sp = search((*current)->oldSyms, sym->name);
                            if (sp)
                                break;
                        }
                        ++current;
                    }
                    // have to try to replicate the symbol into the current context
                    for (auto check = current; check != cilb;)
                    {
                        --check;
                        if ((*check)->captureMode == cmNone)
                        {
                            if (isExplicit)
                            {
                                if (check != cilb)
                                {
                                    errorsym(ERR_EXPLICIT_CAPTURE_BLOCKED, sym);
                                    break;
                                }
                            }
                            else
                            {
                                errorsym(ERR_IMPLICIT_CAPTURE_BLOCKED, sym);
                                break;
                            }
                        }
                    }
                    for ( ; current != cilb;)
                    {
                        --current;
                        // we are replicating captures through intermediate lambdas
                        // to make it easier to handle inner lambda creation.
                        LAMBDASP* ins = Allocate<LAMBDASP>();
                        ins->name = sym->name;
                        ins->parent = sym;
                        sym = CopySymbol(sym);
                        sym->sb->lambdaMode = mode == cmNone ? (*current)->captureMode : mode;
                        sym->tp = lambda_type(sym->tp, sym->sb->lambdaMode);
                        sym->sb->storage_class = StorageClass::member_;
                        sym->sb->parent = (*current)->func;
                        sym->sb->access = AccessLevel::private_;
                        if (mode != cmExplicitValue)
                            sym->sb->init = NULL;
                        lambda_insert(sym, (*current));
                        ins->sp = sym;
                        ins->enclosing = (*current);
                        (*current)->captured->baseInsert(ins);
                    }
                }
            }
        }
    }
    return sym;
}
static Type* cloneFuncType(SYMBOL* funcin)
{
    Type* tp_in = funcin->tp;
    Type** tp;
    SYMBOL* func = CopySymbol(funcin);
    tp = &func->tp;
    while (tp_in)
    {
        *tp = tp_in->CopyType();
        tp_in = tp_in->btp;
        tp = &(*tp)->btp;
    }
    func->tp->UpdateRootTypes();
    func->tp->BaseType()->syms = symbols.CreateSymbolTable();
    auto dest = func->tp->BaseType()->syms;
    for (auto sym : *funcin->tp->BaseType()->syms)
    {
        dest->Add(CopySymbolfalse(sym));
    }
    return func->tp;
}
static void cloneTemplateParams(SYMBOL* func)
{
    func->templateParams = templateParamPairListFactory.CreateList();
    int index = 0;
    auto second = Allocate<TEMPLATEPARAM>();
    second->type = TplType::new_;
    func->templateParams->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
    for (auto arg : *func->tp->BaseType()->syms)
    {
        if (!arg->sb->thisPtr)
        {
            arg->tp = arg->tp->CopyType();
            arg->tp->UpdateRootTypes();
            second = Allocate<TEMPLATEPARAM>();
            auto first = Allocate<SYMBOL>();
            *first = *arg;
            first->sb = nullptr;
            second->type = TplType::typename_;
            func->templateParams->push_back(TEMPLATEPARAMPAIR{ first, second });
            arg->tp->templateParam = &func->templateParams->back();
            arg->templateParams = templateParamPairListFactory.CreateList();
            arg->templateParams->push_back(func->templateParams->back());
        }
    }
}
static void convertCallToTemplate(SYMBOL* func)
{
    func->templateParams = templateParamPairListFactory.CreateList();
    auto second = Allocate<TEMPLATEPARAM>();
    second->type = TplType::new_;
    func->templateParams->push_back(TEMPLATEPARAMPAIR{ nullptr, second });

    if (lambdas.front()->functp->IsAutoType())
        func->tp->BaseType()->btp = &stdauto;  // convert return type back to auto
    int index = 0;
    for (auto arg : *func->tp->BaseType()->syms)
    {
        if (arg->tp->IsAutoType())
        {
            second = Allocate<TEMPLATEPARAM>();
            auto first = Allocate<SYMBOL>();
            *first = *arg;
            first->sb = nullptr;
            second->type = TplType::typename_;
            second->index = index++;
            func->templateParams->push_back(TEMPLATEPARAMPAIR{ first, second });
            arg->tp = Type::MakeType(BasicType::templateparam_);
            arg->tp->templateParam = &func->templateParams->back();
        }
    }
    if (func->templateParams->size())
        RequiresDialect::Feature(Dialect::cpp14, "Generic lambdas");

    func->sb->templateLevel = definingTemplate;
    func->sb->parentTemplate = func;
}
static SYMBOL* createPtrToCaller(SYMBOL* self)
{
    Argument** argptr;
    CallSite* params = Allocate<CallSite>();
    Type* args = cloneFuncType(lambdas.front()->func);
    SYMBOL* func = makeID(StorageClass::static_, args, NULL, "___ptrcall");
    FunctionBlock bd1 = {}, bd2 = {};
    std::list<FunctionBlock*> block1{ &bd1 }, block2{ &bd2 };
    Statement* st;
    args->BaseType()->sp = func;
    args->BaseType()->btp = lambdas.front()->func->tp->BaseType()->btp;
    func->sb->parentClass = lambdas.front()->cls;
    func->sb->attribs.inheritable.linkage = Linkage::cdecl_;
    func->sb->attribs.inheritable.isInline = true;
    func->sb->storage_class = StorageClass::static_;
    func->sb->access = AccessLevel::private_;

    insertFunc(lambdas.front()->cls, func);
    func->tp->syms->remove(func->tp->syms->begin()); // elide this pointer
    st = Statement::MakeStatement(NULL, block2, lambdas.front()->func->tp->BaseType()->btp->IsStructured() ? StatementNode::expr_ : StatementNode::return_);
    st->select = MakeExpression(params);
    st->select = MakeExpression(ExpressionNode::thisref_, st->select);

    if (!params->arguments)
        params->arguments = argumentListFactory.CreateList();
    for (auto sym : *args->syms)
    {
        if (sym->tp->type == BasicType::void_)
            break;
        sym = CopySymbol(sym);
        sym->sb->offset -= getSize(BasicType::pointer_);
        auto arg = Allocate<Argument>();
        if (sym->tp->IsStructured() && !sym->tp->IsRef())
        {
            SYMBOL* sym2 = AnonymousVar(StorageClass::auto_, sym->tp)->v.sp;
            sym2->sb->stackblock = true;
            arg->exp = MakeExpression(ExpressionNode::auto_, sym2);  // DAL MODIFIED
        }
        else
        {
            arg->exp = MakeExpression(ExpressionNode::auto_, sym);
            Dereference(sym->tp, &arg->exp);
        }
        arg->tp = sym->tp;
    }
    params->ascall = true;
    params->sp = lambdas.front()->func;
    params->fcall = MakeExpression(ExpressionNode::pc_, lambdas.front()->func);
    params->functp = func->tp;
    params->thisptr = MakeExpression(ExpressionNode::global_, self);
    Dereference(&stdpointer, &params->thisptr);
    params->thistp = self->tp;
    st = Statement::MakeStatement(NULL, block1, StatementNode::block_);
    st->lower = block2.front()->statements;
    st->blockTail = block2.front()->blockTail;
    func->sb->inlineFunc.stmt = stmtListFactory.CreateList();
    func->sb->inlineFunc.stmt->push_back(Statement::MakeStatement(NULL, emptyBlockdata, StatementNode::block_));
    func->sb->inlineFunc.stmt->front()->lower = block1.front()->statements;
    func->sb->inlineFunc.stmt->front()->blockTail = block1.front()->blockTail;
    func->sb->inlineFunc.syms = args->BaseType()->syms;
    if (lambdas.front()->templateFunctions)
    {
        LexList* lex1;
        int l = 0;
        if (lambdas.front()->functp->IsAutoType())
            func->tp->BaseType()->btp = &stdauto;  // convert return type back to auto
        cloneTemplateParams(func);
        func->sb->templateLevel = definingTemplate;
        func->sb->parentTemplate = func;
        std::string val = "{return ___self->operator()(";
        for (auto sym : *lambdas.front()->func->tp->BaseType()->syms)
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
    func->tp->UpdateRootTypes();
    return func;
}
// this next reloes on the CONST specifier on the func not being set up yet.
static void createConverter(SYMBOL* self)
{
    SYMBOL* caller = createPtrToCaller(self);
    Type* args = cloneFuncType(lambdas.front()->func);
    SYMBOL* func = makeID(StorageClass::member_, Type::MakeType(BasicType::func_, Type::MakeType(BasicType::pointer_, args)), NULL, overloadNameTab[CI_CAST]);
    FunctionBlock bd1 = {}, bd2 = {};
    std::list<FunctionBlock*> block1{ &bd1 }, block2{ &bd2 };
    Statement* st;
    EXPRESSION* exp;
    SYMBOL* sym = makeID(StorageClass::parameter_, &stdvoid, NULL, AnonymousName());
    func->tp->syms = symbols.CreateSymbolTable();
    func->sb->parentClass = lambdas.front()->cls;
    func->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
    func->sb->attribs.inheritable.isInline = true;
    func->sb->storage_class = StorageClass::member_;
    func->sb->castoperator = true;
    func->tp->syms = symbols.CreateSymbolTable();
    func->tp->syms->Add(sym);
    injectThisPtr(func, func->tp->syms);
    func->sb->parentClass = lambdas.front()->cls;
    insertFunc(lambdas.front()->cls, func);
    // Keyword::assign_ ___self = this
    st = Statement::MakeStatement(NULL, block2, StatementNode::expr_);
    st->select = MakeExpression(ExpressionNode::auto_, (SYMBOL*)func->tp->BaseType()->syms->front());
    Dereference(&stdpointer, &st->select);
    st->select = MakeExpression(ExpressionNode::assign_, MakeExpression(ExpressionNode::global_, self), st->select);
    Dereference(&stdpointer, &st->select->left);
    // return pointer to ptrtocaller
    st = Statement::MakeStatement(NULL, block2, StatementNode::return_);
    st->select = MakeExpression(ExpressionNode::pc_, caller);
    st = Statement::MakeStatement(NULL, block1, StatementNode::block_);
    st->lower = block2.front()->statements;
    st->blockTail = block2.front()->blockTail;
    func->sb->inlineFunc.stmt = stmtListFactory.CreateList();
    func->sb->inlineFunc.stmt->push_back(Statement::MakeStatement(NULL, emptyBlockdata, StatementNode::block_));
    func->sb->inlineFunc.stmt->front()->lower = block1.front()->statements;
    func->sb->inlineFunc.stmt->front()->blockTail = block1.front()->blockTail;
    func->sb->inlineFunc.syms = symbols.CreateSymbolTable();
    if (lambdas.front()->templateFunctions)
    {
        LexList* lex1;
        CallSite* f = Allocate<CallSite>();
        if (!f->arguments)
            f->arguments = argumentListFactory.CreateList();
        func->templateParams = caller->templateParams;
        func->sb->templateLevel = definingTemplate;
        func->sb->parentTemplate = func;
        func->tp->BaseType()->btp = Type::MakeType(BasicType::templatedecltype_);
        func->tp->BaseType()->btp->templateDeclType = MakeExpression(f);
        func->tp->BaseType()->btp->templateDeclType->v.func = f;
        for (auto sym : *caller->tp->BaseType()->syms)
        {
            if (sym->sb->thisPtr)
            {
                f->thistp = sym->tp;
                f->thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
            }
            else
            {
                auto arg = Allocate<Argument>();
                arg->tp = sym->tp;
                arg->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                f->arguments->push_back(arg);
            }
        }
        f->ascall = false;
        f->asaddress = true;
        f->sp = caller;
        f->fcall = MakeExpression(ExpressionNode::pc_, f->sp);
        f->functp = f->sp->tp;
        if (caller->tp->BaseType()->btp->IsStructured() || caller->tp->BaseType()->btp->IsBitInt())
        {
            f->returnEXP = MakeIntExpression(ExpressionNode::c_i_, 0);
            f->returnSP = caller->tp->BaseType()->btp->BaseType()->sp;
        }
        std::string val = "{ ___self=this;return &___ptrcall;} ;";

        SetAlternateParse(true, val);
        lex1 = getsym();
        getDeferredData(lex1, &func->sb->deferredCompile, true);
        SetAlternateParse(false, "");
        func->sb->templateLevel = definingTemplate;
        func->sb->parentTemplate = func;
    }
    else
    {
        InsertInline(func);
    }
    func->tp->UpdateRootTypes();
}
static bool lambda_get_template_state(SYMBOL* func)
{
    for (auto sym : *func->tp->BaseType()->syms)
    {
        if (sym->tp->IsAutoType())
        {
            return true;
        }
    }
    return false;
}
static void finishClass(void)
{
    Type* tps = Type::MakeType(BasicType::pointer_, lambdas.front()->cls->tp);
    SYMBOL* self = makeID(StorageClass::static_, tps, NULL, "___self");
    SYMLIST* lst;
    self->sb->access = AccessLevel::private_;

    self->sb->label = Optimizer::nextLabel++;
    self->sb->parentClass = lambdas.front()->cls;
    SetLinkerNames(self, Linkage::cdecl_);
    lambdas.front()->cls->tp->syms->Add(self);
    insertInitSym(self);
    createConstructorsForLambda(lambdas.front()->cls);
    if (lambdas.front()->templateFunctions)
    {
        definingTemplate++;
        dontRegisterTemplate++;
        convertCallToTemplate(lambdas.front()->func);
    }
    SetLinkerNames(lambdas.front()->func, Linkage::cdecl_);
    insertFunc(lambdas.front()->cls, lambdas.front()->func);
    createConverter(self);
    if (lambdas.front()->templateFunctions)
    {
        definingTemplate--;
        dontRegisterTemplate--;
    }
    if (!lambdas.front()->isMutable)
    {
        SYMBOL* ths = (SYMBOL*)lambdas.front()->func->tp->syms->front();
        lambdas.front()->func->tp = Type::MakeType(BasicType::const_, lambdas.front()->func->tp);
        ths->tp->BaseType()->btp = Type::MakeType(BasicType::const_, ths->tp->btp->BaseType());
        ths->tp->UpdateRootTypes();
    }
}
static EXPRESSION* createLambda(bool noinline)
{
    EXPRESSION *rv = NULL, **cur = &rv;
    EXPRESSION *clsThs, *parentThs;
    SYMBOL* cls = makeID(lambdas.front()->enclosingFunc ? StorageClass::auto_ : StorageClass::localstatic_, lambdas.front()->cls->tp, NULL,
                         lambdas.front()->enclosingFunc ? AnonymousName() : AnonymousLambdaName());
    SetLinkerNames(cls, Linkage::cdecl_);
    cls->sb->allocate = true;
    cls->sb->assigned = true;
    if (lambdas.front()->enclosingFunc)
    {
        localNameSpace->front()->syms->Add(cls);
        clsThs = MakeExpression(ExpressionNode::auto_, cls);  // this ptr
    }
    else
    {
        globalNameSpace->front()->syms->Add(cls);  // well if we could put this as an auto in the init func that would be good
                                                        // but there is no way to do that here...
        clsThs = MakeExpression(ExpressionNode::global_, cls);               // this ptr
        cls->sb->label = Optimizer::nextLabel++;
        insertInitSym(cls);
    }
    {
        std::list<Initializer*>* init = NULL;
        if (!init)
            init = initListFactory.CreateList();
        EXPRESSION* exp = clsThs;
        CallDestructor(cls->tp->BaseType()->sp, NULL, &exp, NULL, true, false, false, true);
        InsertInitializer(&init, cls->tp, exp, 0, true);
        if (cls->sb->storage_class != StorageClass::auto_)
        {
            insertDynamicDestructor(cls, init);
        }
        else
        {
            cls->sb->dest = init;
        }
    }
    if (lambdas.front()->enclosingFunc)
        parentThs = MakeExpression(ExpressionNode::auto_, (SYMBOL*)lambdas.front()->enclosingFunc->tp->BaseType()->syms->front());  // this ptr
    else
        parentThs = nullptr;
    auto thisByVal = lambdas.size() == 2 && lambdas.back()->thisByVal;
    for (auto sp : *lambdas.front()->cls->tp->syms)
    {
        EXPRESSION *en = NULL, *en1 = NULL;
        if (!strcmp(sp->name, "$parent"))
        {
            if (!parentThs)
                continue;
            en1 = parentThs;  // get parent from function call
            Dereference(&stdpointer, &en1);
            en = MakeExpression(ExpressionNode::add_, clsThs, MakeIntExpression(ExpressionNode::c_i_, sp->sb->offset));
            Dereference(&stdpointer, &en);
            en = MakeExpression(ExpressionNode::assign_, en, en1);
        }
        else if (!strcmp(sp->name, "$this"))
        {
            if (!parentThs)
                continue;
            if (lambdas.size() == 1 || !lambdas.front()->captureThis)
            {
                en1 = parentThs;  // get parent from function call
            }
            else
            {
                SYMBOL* parent = search(lambdas.front()->cls->tp->syms, "$parent");
                en1 = MakeExpression(ExpressionNode::auto_, cls);
                if (!thisByVal)
                    Dereference(&stdpointer, &en1);
                en1 = MakeExpression(ExpressionNode::add_, en1, MakeIntExpression(ExpressionNode::c_i_, parent->sb->offset));
            }
            Dereference(&stdpointer, &en1);
            en = MakeExpression(ExpressionNode::add_, clsThs, MakeIntExpression(ExpressionNode::c_i_, sp->sb->offset));
            Dereference(&stdpointer, &en);
            en = MakeExpression(ExpressionNode::assign_, en, en1);
        }
        else if (!strcmp(sp->name, "*this"))
        {
            if (!parentThs)
                continue;
            if (lambdas.size() == 1 || !lambdas.front()->captureThis)
            {
                en1 = parentThs;  // get parent from function call
            }
            else
            {
                SYMBOL* parent = search(lambdas.front()->cls->tp->syms, "$parent");
                en1 = MakeExpression(ExpressionNode::auto_, cls);
                Dereference(&stdpointer, &en1);
                en1 = MakeExpression(ExpressionNode::add_, en1, MakeIntExpression(ExpressionNode::c_i_, parent->sb->offset));
            }
            Dereference(&stdpointer, &en1);
            en = MakeExpression(ExpressionNode::add_, clsThs, MakeIntExpression(ExpressionNode::c_i_, sp->sb->offset));
            Type* ctp = sp->tp;
            if (!callConstructorParam(&ctp, &en, sp->tp, en1, true, false, true, false, true))
                errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, sp);
        }
        else if (sp->sb->lambdaMode)
        {
            LAMBDASP* lsp = search(lambdas.front()->captured, sp->name);
            if (lsp)
            {
                en1 = MakeExpression(ExpressionNode::add_, clsThs, MakeIntExpression(ExpressionNode::c_i_, sp->sb->offset));
                if (sp->sb->lambdaMode == cmExplicitValue)
                {
                    SYMBOL* capture = lsp->parent;
                    Type* ctp = capture->tp;
                    if (ctp->IsStructured())
                    {
                        if (!callConstructorParam(&ctp, &en1, sp->tp, sp->sb->init->front()->exp, true, false, true, false, true))
                            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, lsp->sp);
                        en = en1;
                    }
                    else
                    {
                        Dereference(ctp, &en1);
                        en = MakeExpression(ExpressionNode::assign_, en1, sp->sb->init->front()->exp);
                    }
                }
                else if (sp->sb->lambdaMode == cmRef)
                {
                    SYMBOL* capture = lsp->parent;
                    Dereference(&stdpointer, &en1);
                    if (capture->sb->lambdaMode)
                    {
                        en = parentThs;
                        Dereference(&stdpointer, &en);
                        en = MakeExpression(ExpressionNode::add_, en, MakeIntExpression(ExpressionNode::c_i_, capture->sb->offset));
                    }
                    else  // must be an StorageClass::auto_
                    {
                        en = MakeExpression(ExpressionNode::auto_, capture);
                    }
                    if (capture->tp->IsRef())
                    {
                        Dereference(&stdpointer, &en);
                    }
                    en = MakeExpression(ExpressionNode::assign_, en1, en);
                }
                else  // cmValue
                {
                    SYMBOL* capture = lsp->parent;
                    Type* ctp = capture->tp;
                    if (capture->sb->lambdaMode)
                    {
                        en = parentThs;
                        Dereference(&stdpointer, &en);
                        en = MakeExpression(ExpressionNode::add_, en, MakeIntExpression(ExpressionNode::c_i_, capture->sb->offset));
                    }
                    else  // must be an StorageClass::auto_
                    {
                        en = MakeExpression(ExpressionNode::auto_, capture);
                    }
                    if (ctp->IsRef())
                    {
                        ctp = ctp->BaseType()->btp;
                        Dereference(&stdpointer, &en);
                    }
                    if (ctp->IsStructured())
                    {
                        if (!callConstructorParam(&ctp, &en1, ctp, en, true, false, true, false, true))
                            errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, lsp->sp);
                        en = en1;
                    }
                    else
                    {
                        Dereference(ctp, &en1);
                        Dereference(ctp, &en);
                        en = MakeExpression(ExpressionNode::assign_, en1, en);
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
            *cur = MakeExpression(ExpressionNode::comma_, en);
            cur = &(*cur)->right;
        }
    } 
    *cur = copy_expression(clsThs);  // this expression will be used in copy constructors, or discarded if unneeded
    return rv;
}
LexList* expression_lambda(LexList* lex, SYMBOL* funcsp, Type* atp, Type** tp, EXPRESSION** exp, int flags)
{
    auto declline = lines;
    LAMBDA* self;
    SYMBOL *vpl;
    SYMLIST* hrl;
    Type* ltp;
    if (funcsp)
        funcsp->sb->noinline = true;
    self = Allocate<LAMBDA>();
    ltp = Type::MakeType(BasicType::struct_);
    ltp->syms = symbols.CreateSymbolTable();
    ltp->tags = symbols.CreateSymbolTable();
    ltp->size = 0;
    self->captured = lambdaFactory.CreateSymbolTable();
    self->oldSyms = localNameSpace->front()->syms;
    self->oldTags = localNameSpace->front()->tags;
    self->index = lambdaIndex;
    self->captureMode = cmNone;
    self->isMutable = false;
    self->captureThis = false;
    self->cls = makeID(StorageClass::type_, ltp, NULL, LambdaName());
    ltp->sp = self->cls;
    SetLinkerNames(self->cls, Linkage::cdecl_);
    self->cls->sb->islambda = true;
    self->cls->sb->attribs.inheritable.structAlign = getAlign(StorageClass::global_, &stdpointer);
    self->func = makeID(StorageClass::member_, ltp, NULL, overloadNameTab[CI_FUNC]);
    self->func->sb->parentClass = self->cls;
    self->functp = &stdauto;
    self->enclosingFunc = theCurrentFunc;
    lambdas.push_front(self);

    localNameSpace->front()->syms = symbols.CreateSymbolTable();
    localNameSpace->front()->tags = symbols.CreateSymbolTable();
    if (lambdas.size() > 1)
    {
        auto itx = lambdas.begin();
        ++itx;
        self->lthis = (*itx)->lthis;
    }
    else if (funcsp && funcsp->sb->parentClass && (funcsp->sb->storage_class == StorageClass::member_ || funcsp->sb->storage_class == StorageClass::virtual_))
    {
        self->lthis = ((SYMBOL*)funcsp->tp->BaseType()->syms->front())->tp->BaseType()->btp->BaseType()->sp;
    }
    else
    {
        self->lthis = enclosingDeclarations.GetFirst();
    }
    lex = getsym();  // past [
    if (funcsp)
    {
        // can have a capture list;
        if (MATCHKW(lex, Keyword::assign_))
        {
            lex = getsym();
            if (MATCHKW(lex, Keyword::comma_) || MATCHKW(lex, Keyword::closebr_))
            {
                self->captureMode = cmValue;
            }
        }
        else if (MATCHKW(lex, Keyword::and_))
        {
            lex = getsym();
            if (MATCHKW(lex, Keyword::comma_) || MATCHKW(lex, Keyword::closebr_))
            {
                self->captureMode = cmRef;
            }
            else
            {
                lex = backupsym();
            }
        }
        if (MATCHKW(lex, Keyword::comma_) || (self->captureMode == cmNone && !MATCHKW(lex, Keyword::closebr_)))
        {
            do
            {
                if (MATCHKW(lex, Keyword::comma_))
                    skip(&lex, Keyword::comma_);
                bool hasStar = MATCHKW(lex, Keyword::star_);
                if (hasStar)
                    skip(&lex, Keyword::star_);
                if (MATCHKW(lex, Keyword::this_))
                {
                    if (hasStar)
                        RequiresDialect::Feature(Dialect::cpp17, "capture of *this");
                    e_cm localMode = self->captureMode;
                    lex = getsym();
                    if (!hasStar && (localMode == cmValue || !self->lthis))
                    {
                        error(ERR_CANNOT_CAPTURE_THIS_BY_VALUE);
                    }
                    else if (hasStar && localMode == cmRef)
                    {
                        error(ERR_CANNOT_CAPTURE_STAR_THIS_BY_VALUE);
                    }
                    else
                    {
                        if (self->captureThis)
                        {
                            error(ERR_CAPTURE_ITEM_LISTED_MULTIPLE_TIMES);
                        }
                        self->captureThis = true;
                        self->thisByVal = hasStar;
                        lambda_capture(NULL, cmThis, true);
                    }
                    continue;
                }
                if (hasStar)
                {
                    error(ERR_INVALID_LAMBDA_CAPTURE_MODE);
                }
                e_cm localMode = cmNone;
                if (MATCHKW(lex, Keyword::and_))
                {
                    localMode = cmRef;
                    lex = getsym();
                }
                else
                {
                    localMode = cmValue;
                }
                if (ISID(lex))
                {
                    if (localMode != cmNone)
                    {
                        if (self->captureMode == localMode)
                        {
                            if (localMode != cmRef)
                            {
                                errorstr(ERR_CANNOT_CAPTURE_BY_REFERENCE, lex->data->value.s.a);
                            }
                            else
                            {
                                errorstr(ERR_CANNOT_CAPTURE_BY_VALUE, lex->data->value.s.a);
                            }
                        }
                    }
                    else
                    {
                        localMode = self->captureMode;
                    }
                    LexList* idlex = lex;
                    SYMBOL* sp;
                    LAMBDA* current;
                    lex = getsym();
                    if (MATCHKW(lex, Keyword::assign_))
                    {
                        RequiresDialect::Feature(Dialect::cpp14, "Lambda Capture Expressions");
                        Type* tp = NULL;
                        EXPRESSION* exp;
                        lex = getsym();
                        lex = expression_no_comma(lex, funcsp, NULL, &tp, &exp, NULL, 0);
                        if (!tp)
                        {
                            error(ERR_EXPRESSION_SYNTAX);
                        }
                        else
                        {
                            SYMBOL* sp = makeID(StorageClass::auto_, tp, NULL, idlex->data->value.s.a);
                            InsertInitializer(&sp->sb->init, tp, exp, 0, true);
                            lambda_capture(sp, cmExplicitValue, true);
                            if (localMode == cmRef)
                            {
                                errorstr(ERR_CANNOT_CAPTURE_BY_REFERENCE, sp->name);
                            }
                        }
                    }
                    else
                    {
                        sp = search(localNameSpace->front()->syms, idlex->data->value.s.a);
                        for (auto current : lambdas)
                        {
                            sp = search(current->oldSyms, idlex->data->value.s.a);
                            if (sp)
                                break;
                        }
                        if (sp)
                        {
                            if (sp->packed)
                            {
                                if (!MATCHKW(idlex, Keyword::ellipse_))
                                    error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                else
                                    idlex = getsym();
                            }
                            if (sp->packed)
                            {
                                int n = sp->tp->templateParam->second->byPack.pack ? sp->tp->templateParam->second->byPack.pack->size() : 0;
                                auto it = funcsp->tp->syms->begin();
                                auto ite= funcsp->tp->syms->end();
                                while (it != ite && *it != sp)
                                    ++it;
                                for ( ; it != ite && n; n--, ++it)
                                    lambda_capture(*it, localMode, true);
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
            } while (MATCHKW(lex, Keyword::comma_));
        }
        else
        {
            self->captureThis = MATCHKW(lex, Keyword::closebr_);
        }
        needkw(&lex, Keyword::closebr_);
    }
    else
    {
        if (!MATCHKW(lex, Keyword::closebr_))
        {
            error(ERR_LAMBDA_CANNOT_CAPTURE);
        }
        else
        {
            lex = getsym();
        }
    }
    if (MATCHKW(lex, Keyword::openpa_))
    {
        Type* tpx = &stdvoid;
        tpx = TypeGenerator::FunctionParams(lex, NULL, &self->func, tpx, false, StorageClass::auto_, false);
        if (!self->func->tp->syms)
        {
            errorstr(ERR_MISSING_TYPE_FOR_PARAMETER, "undefined");
            *tp = &stdint;
            *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
            return lex;
        }
        for (auto sym : *self->func->tp->syms)
        {
            if (sym->sb->init)
            {
                error(ERR_CANNOT_DEFAULT_PARAMETERS_WITH_LAMBDA);
            }
        }
        if (MATCHKW(lex, Keyword::mutable_))
        {
            for (auto lsp : *self->captured)
            {
                if (lsp->sp->sb->lambdaMode == cmValue)
                {
                    lsp->sp->tp = lsp->sp->tp->BaseType();
                }
            }
            self->isMutable = true;

            lex = getsym();
        }
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (MATCHKW(lex, Keyword::pointsto_))
        {
            lex = getsym();
            self->functp = TypeGenerator::TypeId(lex, funcsp, StorageClass::cast_, false, true, false);
            if (!self->functp)
            {
                error(ERR_TYPE_NAME_EXPECTED);
                self->functp = &stdint;
            }
        }
    }
    else
    {
        self->func->tp = Type::MakeType(BasicType::func_, &stdvoid);
        self->func->tp->sp = self->func;
        SYMBOL* spi;
        spi = makeID(StorageClass::parameter_, self->func->tp, NULL, AnonymousName());
        spi->sb->anonymous = true;
        spi->tp = Type::MakeType(BasicType::void_);
        localNameSpace->front()->syms->Add(spi);
        SetLinkerNames(spi, Linkage::cpp_);
        self->func->tp->syms = localNameSpace->front()->syms;
        self->func->tp->syms->Add(makeID(StorageClass::parameter_, &stdvoid, NULL, AnonymousName()));
    }
    self->func->tp->BaseType()->btp = self->functp;
    injectThisPtr(lambdas.front()->func, lambdas.front()->func->tp->BaseType()->syms);
    lambdas.front()->func->tp->btp = self->functp;
    lambdas.front()->func->tp->rootType = lambdas.front()->func->tp;
    lambdas.front()->func->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
    lambdas.front()->func->sb->attribs.inheritable.isInline = true;
    lambdas.front()->templateFunctions = lambda_get_template_state(lambdas.front()->func);
    enclosingDeclarations.Add(self->cls);
    if (lambdas.front()->captureThis)
    {
        if (lambdas.size() > 1)
        {
            SYMBOL* parent = makeID(StorageClass::member_, &stdpointer, NULL, "$parent");
            lambda_insert(parent, lambdas.front());
        }
        if (lambdas.front()->thisByVal)
        {
            if (lambdas.front()->enclosingFunc)
            {
                auto ths =
                    makeID(StorageClass::member_, lambda_type(lambdas.front()->enclosingFunc->tp->BaseType()->syms->front()->tp->BaseType()->btp, cmValue), NULL, "*this");
                SetLinkerNames(ths, Linkage::cdecl_);
                lambda_insert(ths, lambdas.front());
            }
        }
        else
        {
            auto ths = makeID(StorageClass::member_, &stdpointer, NULL, "$this");
            SetLinkerNames(ths, Linkage::cdecl_);
            lambda_insert(ths, lambdas.front());

        }
    }
    else
    {
        auto ths = makeID(StorageClass::member_, &stdpointer, NULL, "$this");
        SetLinkerNames(ths, Linkage::cdecl_);
        lambda_insert(ths, lambdas.front());
    }
    if (MATCHKW(lex, Keyword::begin_))
    {
        lex = getDeferredData(lex, &self->func->sb->deferredCompile, true);
        if (!lambdas.front()->templateFunctions)
        {
            LexList* lex1 = SetAlternateLex(self->func->sb->deferredCompile);
            SetLinkerNames(self->func, Linkage::cdecl_);
            StatementGenerator sg(lex1, self->func);
            sg.FunctionBody();
            sg.BodyGen();
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
    enclosingDeclarations.Drop();
    localNameSpace->front()->syms = self->oldSyms;
    localNameSpace->front()->tags = self->oldTags;
    finishClass();
    *exp = createLambda(0);
    *tp = lambdas.front()->cls->tp;
    lambdas.pop_front();
    lines = declline;
    return lex;
}
}  // namespace Parser