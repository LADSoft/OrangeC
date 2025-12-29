/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
#include <unordered_map>
#include <stack>
#include "Utils.h"
#include "ccerr.h"
#include "declcpp.h"
#include "config.h"
#include "mangle.h"
#include "initbackend.h"
#include "occparse.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "lambda.h"
#include "lex.h"
#include "help.h"
#include "stmt.h"
#include "expr.h"
#include "rtti.h"
#include "constopt.h"
#include "OptUtils.h"
#include "memory.h"
#include "inline.h"
#include "init.h"
#include "beinterf.h"
#include "declcons.h"
#include "ildata.h"
#include "types.h"
#include "declare.h"
#include "constexpr.h"
#include "optmain.h"
#include "iexpr.h"
#include "iblock.h"
#include "istmt.h"
#include "iinline.h"
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "overload.h"
#include "class.h"
#include "exprpacked.h"
#include "SymbolProperties.h"
#include "attribs.h"

namespace CompletionCompiler
{
void ccInsertUsing(Parser::SYMBOL* ns, Parser::SYMBOL* parentns, const char* file, int line);
}
namespace Parser
{

int noNeedToSpecialize;

void internalClassRefCount(SYMBOL* base, SYMBOL* derived, int* vcount, int* ccount, bool isVirtual)
{
    bool ok = false;
    if (base == derived || (base && derived && SameTemplate(derived->tp, base->tp)))
    {

        if (!IsDefiningTemplate())
        {
            if (base->templateParams && derived->templateParams && base->templateParams->front().second->bySpecialization.types &&
                derived->templateParams->front().second->bySpecialization.types)
            {
                if (exactMatchOnTemplateArgs(derived->templateParams->front().second->bySpecialization.types,
                                             base->templateParams->front().second->bySpecialization.types))
                {
                    (*ccount)++;
                    ok = true;
                }
            }
            else
            {
                if (exactMatchOnTemplateArgs(derived->templateParams, base->templateParams))
                {
                    (*ccount)++;
                    ok = true;
                }
            }
        }
        else
        {
            (*ccount)++;
            ok = true;
        }
    }
    if (!ok)
    {
        if (base && derived && derived->sb)
        {
            if (derived->sb->baseClasses)
            {
                for (auto lst : *derived->sb->baseClasses)
                {
                    SYMBOL* sym = lst->cls;
                    if (sym->tp->type == BasicType::typedef_)
                    {
                        sym = sym->tp->BaseType()->sp;
                    }
                    if (sym == base || SameTemplate(sym->tp, base->tp))
                    {
                        if (isVirtual || lst->isvirtual)
                            (*vcount)++;
                        else
                            (*ccount)++;
                    }
                    else
                    {
                        internalClassRefCount(base, sym, vcount, ccount, isVirtual | lst->isvirtual);
                    }
                }
            }
        }
    }
}
// will return 0 for not a base class, 1 for unambiguous base class, >1 for ambiguous base class
int classRefCount(SYMBOL* base, SYMBOL* derived)
{
    int vcount = 0, ccount = 0;

    if (base && base->sb && base->sb->mainsym)
        base = base->sb->mainsym;
    if (derived)
    {
        if (derived->sb && derived->sb->mainsym)
            derived = derived->sb->mainsym;
        internalClassRefCount(base, derived, &vcount, &ccount, false);
    }
    if (vcount)
        ccount++;
    return ccount;
}
static void RecalcArraySize(Type* tp)
{
    if (tp->BaseType()->btp->IsArray())
        RecalcArraySize(tp->BaseType()->btp);
    tp->size = tp->BaseType()->btp->BaseType()->size * tp->BaseType()->esize->v.i;
}
void deferredInitializeDefaultArg(SYMBOL* arg, SYMBOL* func)
{
    if (!IsDefiningTemplate())
    {
        if (!arg->sb->init && initTokenStreams.get(arg))
        {
            // we are evauating a function argument
            // have to disable searching in any function blocks
            static SymbolTable<struct sym> syms;
            static NAMESPACEVALUEDATA ns{ &syms };
            static std::list<NAMESPACEVALUEDATA*> nswrapper = { &ns };

            auto oldNS = localNameSpace;
            localNameSpace = &nswrapper;
            bool initted = false;
            int oldInstantiatingTemplate;
            auto str = func->sb->parentClass;
            if (str && str->templateParams && !allTemplateArgsSpecified(str, str->templateParams))
            {
                initted = true;
                oldInstantiatingTemplate = templateInstantiationLevel;
                templateInstantiationLevel = 0;
                templateDefinitionLevel++;
            }
            Type* tp2;
            TemplateNamespaceScope namespaceScope(str ? str : func);
            DeclarationScope scope;
            if (str)
            {
                enclosingDeclarations.Add(str);
                if (str->templateParams)
                {
                    enclosingDeclarations.Add(str->templateParams);
                }
            }
            if (func->templateParams)
            {
                enclosingDeclarations.Add(func->templateParams);
            }
            ParseOnStream(initTokenStreams.get(arg), [&]() {
                arg->tp->InstantiateDeferred();
                tp2 = arg->tp;
                if (tp2->IsRef())
                    tp2 = tp2->BaseType()->btp;
                if (tp2->IsStructured())
                {
                    SYMBOL* sym2;
                    anonymousNotAlloc++;
                    sym2 = AnonymousVar(StorageClass::auto_, tp2)->v.sp;
                    anonymousNotAlloc--;
                    sym2->sb->stackblock = !arg->tp->IsRef();
                    initialize(theCurrentFunc, sym2, StorageClass::auto_, false, false, false, 0); /* also reserves space */
                    arg->sb->init = sym2->sb->init;
                    if (arg->sb->init->front()->exp && arg->sb->init->front()->exp->type == ExpressionNode::thisref_)
                    {
                        EXPRESSION** expr = &arg->sb->init->front()->exp->left->v.func->thisptr;
                        if ((*expr)->type == ExpressionNode::add_ && isconstzero(&stdint, (*expr)->right))
                            arg->sb->init->front()->exp->v.t.thisptr = (*expr) = (*expr)->left;
                    }
                }
                else
                {
                    initialize(theCurrentFunc, arg, StorageClass::member_, false, false, false, 0);
                }
            });
            if (initted)
            {
                templateDefinitionLevel--;
                templateInstantiationLevel = oldInstantiatingTemplate;
            }
            localNameSpace = oldNS;
        }
    }
}
void deferredInitializeStructFunctions(SYMBOL* cur)
{
    DeclarationScope scope;
    TemplateNamespaceScope namespaceScope(cur);

    enclosingDeclarations.Add(cur);

    if (cur->templateParams)
    {
        enclosingDeclarations.Add(cur->templateParams);
    }
    //    dontRegisterTemplate++;
    for (auto sp : *cur->tp->syms)
    {
        if (sp->sb->storage_class == StorageClass::overloads_)
        {
            if (templateDefinitionLevel != 1 || templateInstantiationLevel)
            {
                for (auto sp1 : *sp->tp->syms)
                {
                    if (!sp1->sb->templateLevel)
                    {
                        for (auto sp2 : *sp1->tp->BaseType()->syms)
                        {
                            if (!sp2->sb->thisPtr)
                            {
                                if (!sp2->sb->init && initTokenStreams.get(sp2))
                                {
                                    sp2->tp->InstantiateDeferred();
                                    Type* tp2;
                                    ParseOnStream(initTokenStreams.get(sp2), [&]() {
                                        tp2 = sp2->tp;
                                        if (tp2->IsRef())
                                            tp2 = tp2->BaseType()->btp;
                                        if (!tp2->IsStructured())
                                        {
                                            initialize(theCurrentFunc, sp2, StorageClass::member_, false, false, false, 0);
                                            initTokenStreams.set(sp2, nullptr);
                                        }
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //    dontRegisterTemplate--;
}
void deferredInitializeStructMembers(SYMBOL* cur)
{
    Optimizer::LIST* staticAssert;
    TemplateNamespaceScope namespaceScope(cur);
    DeclarationScope scope;
    enclosingDeclarations.Add(cur);

    if (cur->templateParams)
    {
        enclosingDeclarations.Add(cur->templateParams);
    }
    dontRegisterTemplate++;
    for (auto sp : *cur->tp->syms)
    {
        if (sp->tp->IsArray() && sp->tp->esize)
        {
            RecalcArraySize(sp->tp);
        }
        if (sp->sb->storage_class == StorageClass::overloads_)
        {
        }
        else if (!sp->sb->init && initTokenStreams.get(sp))
        {
            ParseOnStream(initTokenStreams.get(sp), [=]() {
                initTokenStreams.set(sp, nullptr);
                initialize(theCurrentFunc, sp, StorageClass::member_, false, false, false, 0);
            });
            if (sp->sb->constexpression)
            {
                optimize_for_constants(&sp->sb->init->front()->exp);
            }
        }
    }
    dontRegisterTemplate--;
}
bool declaringTemplate(SYMBOL* sym)
{
    for (auto&& l : enclosingDeclarations)
    {
        if (l.str && l.str->sb->templateLevel)
        {
            if (sym->sb->decoratedName && !strcmp(sym->sb->decoratedName, l.str->sb->decoratedName))
                return true;
        }
    }
    return false;
}
bool usesVTab(SYMBOL* sym)
{
    BASECLASS* base;
    for (auto cur : *sym->tp->syms)
    {
        if (cur->sb->storage_class == StorageClass::overloads_)
        {
            for (auto sp : *cur->tp->syms)
            {
                if (sp->sb->storage_class == StorageClass::virtual_)
                    return true;
            }
        }
    }
    if (sym->sb->baseClasses)
        for (auto base : *sym->sb->baseClasses)
            if (base->cls->sb->hasvtab)
                return true;
    return false;
}
BASECLASS* innerBaseClass(SYMBOL* declsym, SYMBOL* bcsym, bool isvirtual, AccessLevel currentAccess)
{
    bool found = false;
    if (declsym->sb->baseClasses)
        for (auto t : *declsym->sb->baseClasses)
            if (t->cls == bcsym)
            {
                found = true;
                errorsym(ERR_BASE_CLASS_INCLUDED_MORE_THAN_ONCE, bcsym);
                break;
            }
    if (bcsym->tp->BaseType()->type == BasicType::union_)
        error(ERR_UNION_CANNOT_BE_BASE_CLASS);
    if (bcsym->sb->isfinal)
        errorsym(ERR_FINAL_BASE_CLASS, bcsym);
    auto bc = Allocate<BASECLASS>();
    bc->accessLevel = currentAccess;
    bc->isvirtual = isvirtual;
    bc->cls = bcsym;
    // inject it in the class so we can look it up as a type later
    //
    if (!found)
    {
        SYMBOL* injected = CopySymbol(bcsym);
        if (injected->sb->decoratedName)
            injected->name = injected->sb->decoratedName;  // for nested templates
        injected->sb->mainsym = bcsym;
        injected->sb->parentClass = declsym;
        injected->sb->access = currentAccess;
        declsym->tp->tags->Add(injected);
    }
    return bc;
}
void baseClasses( SYMBOL* funcsp, SYMBOL* declsym, AccessLevel defaultAccess)
{
    auto baseClasses = baseClassListFactory.CreateList();
    AccessLevel currentAccess;
    bool isvirtual = false;
    bool done = false;
    SYMBOL* bcsym;
    currentAccess = defaultAccess;
    getsym();  // past ':'
    if (declsym->tp->BaseType()->type == BasicType::union_)
        error(ERR_UNION_CANNOT_HAVE_BASE_CLASSES);
    {
        DeclarationScope scope(declsym);
        bool skiptoend;
        do
        {
            skiptoend = false;
            // this next needs work, OCC throws errors because of the gotos later in this function...
            LexemeStreamPosition placeHolder(currentStream);
            ParseAttributeSpecifiers(funcsp, true);
            EnterPackedSequence();
            if (MATCHKW(Keyword::decltype_))
            {
                Type* tp = nullptr;
                tp = TypeGenerator::TypeId(funcsp, StorageClass::type_, true, true, false);
                tp->InstantiateDeferred();
                if (!tp)
                {
                    error(ERR_TYPE_NAME_EXPECTED);
                }
                else if (!tp->IsStructured())
                {
                    error(ERR_STRUCTURED_TYPE_EXPECTED);
                }
                else
                {
                    auto bc = innerBaseClass(declsym, tp->BaseType()->sp, isvirtual, currentAccess);
                    if (bc)
                        baseClasses->push_back(bc);
                }
                done = !MATCHKW(Keyword::comma_);
                if (!done)
                    getsym();
            }
            else if (MATCHKW(Keyword::classsel_) || ISID())
            {
                char name[512];
                name[0] = 0;
                if (ISID())
                    Utils::StrCpy(name, currentLex->value.s.a);
                bcsym = nullptr;
                nestedSearch(&bcsym, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, false, false);
                if (bcsym && bcsym->sb && bcsym->sb->storage_class == StorageClass::typedef_)
                {
                    if (!bcsym->sb->templateLevel)
                    {
                        // in case typedef is being used as a base class specifier
                        Type* tp = bcsym->tp->BaseType();
                        tp->InstantiateDeferred();
                        tp->InitializeDeferred();
                        if (tp->IsStructured())
                        {
                            bcsym = tp->sp;
                        }
                        else if (tp->type != BasicType::templateselector_)
                        {
                            bcsym = nullptr;
                        }
                    }
                }
                getsym();
            restart:
                if (bcsym && bcsym->tp->type == BasicType::templateselector_)
                {
                    //                if (!templateDefinitionLevel && !declaringTemplate((*bcsym->tp->sp->sb->templateSelector)[1].sp))
                    //                    error(ERR_STRUCTURED_TYPE_EXPECTED_IN_TEMPLATE_PARAMETER);
                    if (MATCHKW(Keyword::lt_))
                    {
                        inTemplateSpecialization++;
                        std::list<TEMPLATEPARAMPAIR>* nullLst = nullptr;
                        GetTemplateArguments(funcsp, bcsym, &nullLst);
                        inTemplateSpecialization--;
                        auto&& tsl = bcsym->tp->sp->sb->templateSelector->back();
                        tsl.isTemplate = true;
                        tsl.templateParams = nullLst;
                    }
                    //                bcsym = nullptr;
                    if (bcsym && (!IsDefiningTemplate()))
                    {
                        auto bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                        if (bc)
                            baseClasses->push_back(bc);
                    }
                    done = !MATCHKW(Keyword::comma_);
                    if (!done)
                    {
                        getsym();
                        LeavePackedSequence();
                        continue;
                    }
                    skiptoend = true;
                }
                else if (bcsym &&
                    (bcsym->sb && bcsym->sb->templateLevel ||
                        bcsym->tp->type == BasicType::templateparam_ && bcsym->tp->templateParam->second->type == TplType::template_))
                {
                    if (bcsym->tp->type == BasicType::templateparam_)
                    {
                        auto v = bcsym->tp->templateParam->second->byTemplate.val;
                        if (v)
                        {
                            bcsym = v;
                        }
                        else
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            SYMBOL* sp1;
                            inTemplateSpecialization++;
                            GetTemplateArguments(funcsp, bcsym, &lst);
                            inTemplateSpecialization--;
                            currentAccess = defaultAccess;
                            isvirtual = false;
                            done = !MATCHKW(Keyword::comma_);
                            if (!done)
                                getsym();
                            skiptoend = true;
                        }
                    }
                    if (!skiptoend)
                    {
                        if (bcsym->sb->storage_class == StorageClass::typedef_)
                        {
                            if (MATCHKW(Keyword::lt_))
                            {
                                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                SYMBOL* sp1;
                                inTemplateSpecialization++;
                                GetTemplateArguments(funcsp, bcsym, &lst);
                                inTemplateSpecialization--;
                                sp1 = GetTypeAliasSpecialization(bcsym, lst);
                                if (sp1)
                                {
                                    bcsym = sp1;
                                    bcsym->tp->InstantiateDeferred();
                                    if (bcsym->tp->IsStructured())
                                        bcsym->tp = bcsym->tp->InitializeDeferred();
                                    else
                                        bcsym->tp = SynthesizeType(bcsym->tp, nullptr, false);
                                    if (templateDefinitionLevel && bcsym->tp->type == BasicType::any_)
                                    {
                                        currentAccess = defaultAccess;
                                        isvirtual = false;
                                        done = !MATCHKW(Keyword::comma_);
                                        if (!done)
                                            getsym();
                                        LeavePackedSequence();
                                        continue;
                                    }
                                    else if (bcsym && (!IsDefiningTemplate()))
                                    {
                                        auto bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                                        if (bc)
                                            baseClasses->push_back(bc);
                                    }
                                }
                            }
                            else
                            {
                                SpecializationError(bcsym);
                            }
                            done = !MATCHKW(Keyword::comma_);
                            if (!done)
                                getsym();
                            LeavePackedSequence();
                            continue;
                        }
                        else
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            if (MATCHKW(Keyword::lt_))
                            {
                                int i;
                                inTemplateSpecialization++;
                                GetTemplateArguments(funcsp, bcsym, &lst);
                                inTemplateSpecialization--;
                                if (MATCHKW(Keyword::ellipse_))
                                {
                                    if (templateDefinitionLevel)
                                    {
                                        bcsym = GetClassTemplate(bcsym, lst, true);
                                        if (bcsym)
                                        {
                                            bcsym->packed = true;
                                            auto bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                                            if (bc)
                                                baseClasses->push_back(bc);
                                        }
                                    }
                                    else
                                    {
                                        int n = GetPackCount();
                                        if (n)
                                        {
                                            PushPackIndex();
                                            processingTemplateArgs++;
                                            for (int i = 0; i < n; i++)
                                            {
                                                SetPackIndex(i);
                                                placeHolder.Replay([&]() {
                                                    SYMBOL* sym = nullptr;
                                                    nestedSearch(&sym, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, false, false);
                                                    getsym();
                                                    if (sym)
                                                    {
                                                        inTemplateSpecialization++;
                                                        lst = nullptr;
                                                        GetTemplateArguments(funcsp, sym, &lst);
                                                        inTemplateSpecialization--;
                                                        sym = GetClassTemplate(sym, lst, false);
                                                        if (sym)
                                                        {
                                                            if (allTemplateArgsSpecified(sym, sym->templateParams))
                                                                sym = TemplateClassInstantiateInternal(sym, sym->templateParams, false);
                                                            if (sym)
                                                            {
                                                                SetLinkerNames(sym, Linkage::cdecl_);
                                                                auto bc = innerBaseClass(declsym, sym, isvirtual, currentAccess);
                                                                if (bc)
                                                                {
                                                                    baseClasses->push_back(bc);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    });
                                            }
                                            processingTemplateArgs--;
                                            PopPackIndex();
                                        }
                                    }
                                    getsym();
                                    currentAccess = defaultAccess;
                                    isvirtual = false;
                                    done = !MATCHKW(Keyword::comma_);
                                    if (!done)
                                        getsym();
                                    ClearPackedSequence();
                                    LeavePackedSequence();
                                    continue;
                                }
                                else
                                {
                                    bcsym = GetClassTemplate(bcsym, lst, true);
                                    if (bcsym && bcsym->sb->attribs.inheritable.linkage4 != Linkage::virtual_ &&
                                        allTemplateArgsSpecified(bcsym, bcsym->templateParams))
                                    {
                                        bcsym->tp = TemplateClassInstantiateInternal(bcsym, bcsym->templateParams, false)->tp;
                                        bcsym = bcsym->tp->BaseType()->sp;
                                        //                                bcsym->tp->sp = bcsym;
                                    }
                                }
                            }
                        }
                    }
                }
                else if (MATCHKW(Keyword::lt_))
                {
                    errorstr(ERR_NOT_A_TEMPLATE, bcsym ? bcsym->name : name[0] ? name : "unknown");
                }
                if (!skiptoend)
                {
                    if (bcsym && bcsym->tp->templateParam && bcsym->tp->templateParam->second->packed)
                    {
                        if (bcsym->tp->templateParam->second->type != TplType::typename_)
                            error(ERR_NEED_PACKED_TEMPLATE_OF_TYPE_CLASS);
                        else if (bcsym->tp->templateParam->second->byPack.pack)
                        {
                            for (auto tpp : *bcsym->tp->templateParam->second->byPack.pack)
                            {
                                if (!tpp.second->byClass.val->IsStructured())
                                {
                                    errorcurrent(ERR_STRUCTURED_TYPE_EXPECTED_IN_PACKED_TEMPLATE_PARAMETER);
                                }
                                else
                                {
                                    auto bc = innerBaseClass(declsym, tpp.second->byClass.val->sp, isvirtual, currentAccess);
                                    if (bc)
                                        baseClasses->push_back(bc);
                                }
                            }
                        }
                        if (!MATCHKW(Keyword::ellipse_))
                            error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                        else
                            getsym();
                        currentAccess = defaultAccess;
                        isvirtual = false;
                        done = !MATCHKW(Keyword::comma_);
                        if (!done)
                            getsym();
                    }
                    else if (bcsym && bcsym->tp->templateParam && !bcsym->tp->templateParam->second->packed)
                    {
                        if (bcsym->tp->templateParam->second->type != TplType::typename_)
                            error(ERR_CLASS_TEMPLATE_PARAMETER_EXPECTED);
                        else
                        {
                            Type* tp = bcsym->tp->templateParam->second->byClass.val;
                            if (tp)
                            {
                                tp->InstantiateDeferred();
                                tp = tp->BaseType();
                                if (tp->type == BasicType::templateselector_)
                                {
                                    SYMBOL* sym = (*tp->sp->sb->templateSelector)[1].sp;
                                    for (int i = 2; i < (*tp->sp->sb->templateSelector).size() && sym; ++i)
                                    {
                                        sym->tp->InstantiateDeferred();
                                        auto lst = &(*tp->sp->sb->templateSelector)[i];
                                        sym = search(sym->tp->syms, lst->name);
                                    }
                                    if (sym)
                                    {
                                        bcsym = sym;
                                        goto restart;
                                    }
                                }
                            }
                            if (!tp || !tp->IsStructured())
                            {
                                if (tp)
                                    error(ERR_STRUCTURED_TYPE_EXPECTED_IN_TEMPLATE_PARAMETER);
                            }
                            else
                            {
                                auto bc = innerBaseClass(declsym, tp->sp, isvirtual, currentAccess);
                                if (bc)
                                    baseClasses->push_back(bc);
                                currentAccess = defaultAccess;
                                isvirtual = false;
                            }
                        }
                        done = !MATCHKW(Keyword::comma_);
                        if (!done)
                            getsym();
                    }
                    else if (bcsym && (istype(bcsym) && bcsym->tp->IsStructured()))
                    {
                        auto bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                        if (bc)
                            baseClasses->push_back(bc);
                        currentAccess = defaultAccess;
                        isvirtual = false;
                        done = !MATCHKW(Keyword::comma_);
                        if (!done)
                            getsym();
                    }
                    else
                    {
                        if (!templateDefinitionLevel)
                            error(ERR_CLASS_TYPE_EXPECTED);
                        done = true;
                    }
                }
            }
            else
                switch (KW())
                {
                case Keyword::virtual_:
                    isvirtual = true;
                    getsym();
                    break;
                case Keyword::private_:
                    currentAccess = AccessLevel::private_;
                    getsym();
                    break;
                case Keyword::protected_:
                    currentAccess = AccessLevel::protected_;
                    getsym();
                    break;
                case Keyword::public_:
                    currentAccess = AccessLevel::public_;
                    getsym();
                    break;
                default:
                    error(ERR_IDENTIFIER_EXPECTED);
                    errskim(skim_end);
                    return;
                }
            LeavePackedSequence();
            if (!done)
                ParseAttributeSpecifiers(funcsp, true);
        } while (!done);
    }
    declsym->sb->baseClasses = baseClasses;
    for (auto lst : *declsym->sb->baseClasses)
    {
        if (!isExpressionAccessible(nullptr, lst->cls, nullptr, nullptr, false))
        {
            bool err = true;
            for (auto lst2 : *declsym->sb->baseClasses)
            {
                if (lst2 != lst)
                {
                    if (isAccessible(lst2->cls, lst2->cls, lst->cls, nullptr, AccessLevel::protected_, false))
                    {
                        err = false;
                        break;
                    }
                }
            }
            if (err)
            {
                isExpressionAccessible(nullptr, lst->cls, nullptr, nullptr, false);
                errorsym(ERR_CANNOT_ACCESS, lst->cls);
            }
        }
    }
    return;
}
static bool classOrEnumParam(SYMBOL* param)
{
    Type* tp = param->tp;
    if (tp->IsRef())
        tp = tp->BaseType()->btp;
    tp = tp->BaseType();
    return tp->IsStructured() || tp->type == BasicType::enum_ || tp->type == BasicType::templateparam_ ||
           tp->type == BasicType::templateselector_ || tp->type == BasicType::templatedecltype_;
}
void checkOperatorArgs(SYMBOL* sp, bool asFriend)
{
    Type* tp = sp->tp;
    if (tp->IsRef())
        tp = tp->BaseType()->btp;
    if (!tp->IsFunction())
    {
        char buf[256];
        if (sp->sb->castoperator)
        {
            Utils::StrCpy(buf, "typedef()");
        }
        else
        {
            Utils::StrCpy(buf, overloadXlateTab[sp->sb->operatorId]);
        }
        errorstr(ERR_OPERATOR_MUST_BE_FUNCTION, buf);
    }
    else
    {
        auto it = sp->tp->BaseType()->syms->begin();
        auto itend = sp->tp->BaseType()->syms->end();
        if (it == itend)
            return;
        if (!asFriend && enclosingDeclarations.GetFirst())  // nonstatic member
        {
            if (sp->sb->operatorId == CI_CAST)
            {
                // needs no argument
                SYMBOL* sym = *it;
                if (sym->tp->type != BasicType::void_)
                {
                    errortype(ERR_OPERATOR_NEEDS_NO_PARAMETERS, sym->tp->BaseType()->btp, nullptr);
                }
            }
            else
                switch ((Keyword)(sp->sb->operatorId - CI_NEW))
                {
                    SYMBOL* sym;
                    case Keyword::plus_:
                    case Keyword::minus_:
                    case Keyword::star_:
                    case Keyword::and_:
                        // needs zero or one argument
                        if (sp->tp->BaseType()->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        if (sp->tp->BaseType()->syms->size() == 0)
                        {
                            switch ((Keyword)(sp->sb->operatorId - CI_NEW))
                            {
                                case Keyword::plus_:
                                    sp->sb->operatorId = (int)Keyword::unary_plus_;
                                    break;
                                case Keyword::minus_:
                                    sp->sb->operatorId = (int)Keyword::unary_minus_;
                                    break;
                                case Keyword::star_:
                                    sp->sb->operatorId = (int)Keyword::unary_star_;
                                    break;
                                case Keyword::and_:
                                    sp->sb->operatorId = (int)Keyword::unary_and_;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case Keyword::not_:
                    case Keyword::complx_:
                        // needs no argument
                        sym = *it;
                        if (sym->tp->type != BasicType::void_)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_NO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case Keyword::divide_:
                    case Keyword::leftshift_:
                    case Keyword::rightshift_:
                    case Keyword::mod_:
                    case Keyword::eq_:
                    case Keyword::neq_:
                    case Keyword::lt_:
                    case Keyword::leq_:
                    case Keyword::gt_:
                    case Keyword::geq_:
                    case Keyword::land_:
                    case Keyword::lor_:
                    case Keyword::or_:
                    case Keyword::uparrow_:
                    case Keyword::comma_:
                    case Keyword::pointstar_:
                        // needs one argument
                        sym = *it;
                        if (sym->tp->type == BasicType::void_ || sp->tp->BaseType()->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case Keyword::autoinc_:
                    case Keyword::autodec_:
                        if (sp->tp->BaseType()->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        sym = *it;
                        if (sym->tp->type != BasicType::void_ && sym->tp->type != BasicType::int_)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_PARAMETER_OF_TYPE_INT, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case Keyword::new_:
                    case Keyword::delete_:
                    case Keyword::newa_:  // new[]
                    case Keyword::dela_:  // delete[]
                        break;
                    case Keyword::assign_:
                    case Keyword::asplus_:
                    case Keyword::asminus_:
                    case Keyword::astimes_:
                    case Keyword::asdivide_:
                    case Keyword::asmod_:
                    case Keyword::asleftshift_:
                    case Keyword::asrightshift_:
                    case Keyword::asand_:
                    case Keyword::asor_:
                    case Keyword::asxor_:
                        // needs one argument
                        sym = *it;
                        if (sym->tp->type == BasicType::void_ || sp->tp->BaseType()->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;

                    case Keyword::openbr_:
                        // needs one argument:
                        sym = *it;
                        if (sym->tp->type == BasicType::void_ || sp->tp->BaseType()->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case Keyword::openpa_:
                        // anything goes
                        break;
                    case Keyword::pointsto_:
                        // needs no arguments
                        sym = *it;
                        if (sym->tp->type != BasicType::void_)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_NO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case Keyword::quot_:
                        errorsym(ERR_OPERATOR_LITERAL_NAMESPACE_SCOPE, sp);
                        break;
                    default:
                        diag("checkoperatorargs: unknown operator type");
                        break;
                }
            if (!ismember(sp))
            {
                if (sp->sb->operatorId == CI_CAST)
                {
                    errortype(ERR_OPERATOR_NONSTATIC, sp->tp->BaseType()->btp, nullptr);
                }
                else if ((Keyword)(sp->sb->operatorId - CI_NEW) != Keyword::new_ &&
                         (Keyword)(sp->sb->operatorId - CI_NEW) != Keyword::delete_ &&
                         (Keyword)(sp->sb->operatorId - CI_NEW) != Keyword::newa_ &&
                         (Keyword)(sp->sb->operatorId - CI_NEW) != Keyword::dela_)
                {
                    errorstr(ERR_OPERATOR_NONSTATIC, overloadXlateTab[sp->sb->operatorId]);
                }
            }
        }
        else
        {
            switch ((Keyword)(sp->sb->operatorId - CI_NEW))
            {
                SYMBOL* sym;
                case Keyword::plus_:
                case Keyword::minus_:
                case Keyword::star_:
                case Keyword::and_:
                case Keyword::asplus_:
                case Keyword::asminus_:
                case Keyword::astimes_:
                case Keyword::asdivide_:
                case Keyword::asmod_:
                case Keyword::asleftshift_:
                case Keyword::asrightshift_:
                case Keyword::asand_:
                case Keyword::asor_:
                case Keyword::asxor_:
                    // needs one or two arguments, one being class type
                    sym = (*it);
                    ++it;
                    if (sym->tp->type == BasicType::void_ || sp->tp->BaseType()->syms->size() > 2)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else
                    {
                        sym->tp->InstantiateDeferred();
                        if (!classOrEnumParam(sym) && (sp->tp->BaseType()->syms->size() == 1 || !classOrEnumParam((*it))))
                        {
                            if (!templateDefinitionLevel)
                                errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                    }
                    break;
                case Keyword::not_:
                case Keyword::complx_:
                    // needs one arg of class or type
                    sym = *it;
                    if (sym->tp->type == BasicType::void_ || sp->tp->BaseType()->syms->size() != 1)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else
                    {
                        sym->tp->InstantiateDeferred();
                        if (!classOrEnumParam(sym))
                        {
                            if (!templateDefinitionLevel)
                                errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                    }
                    break;

                case Keyword::divide_:
                case Keyword::leftshift_:
                case Keyword::rightshift_:
                case Keyword::mod_:
                case Keyword::eq_:
                case Keyword::neq_:
                case Keyword::lt_:
                case Keyword::leq_:
                case Keyword::gt_:
                case Keyword::geq_:
                case Keyword::land_:
                case Keyword::lor_:
                case Keyword::or_:
                case Keyword::uparrow_:
                case Keyword::comma_:
                    // needs two args, one of class or type
                    if (sp->tp->BaseType()->syms->size() != 2)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_TWO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else
                    {
                        auto test = false;
                        (*it)->tp->InstantiateDeferred();
                        test = classOrEnumParam(*it);
                        if (!test)
                        {
                            (*++it)->tp->InstantiateDeferred();
                            test = classOrEnumParam(*it);
                        }
                        if (!test)
                        {
                            if (!templateDefinitionLevel)
                                errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                    }
                    break;
                case Keyword::autoinc_:
                case Keyword::autodec_:
                    // needs one or two args, first of class or type
                    // if second is present int type
                    sym = (*it);
                    if (sym->tp->type == BasicType::void_ || sp->tp->BaseType()->syms->size() > 2)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else
                    {
                        sym->tp->InstantiateDeferred();
                        if (!classOrEnumParam(sym))
                        {
                            if (!templateDefinitionLevel)
                                errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                    }
                    if (sp->tp->BaseType()->syms->size() == 2)
                    {
                        sym = *++it;
                        if (sym->tp->type != BasicType::int_)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_SECOND_PARAMETER_OF_TYPE_INT, overloadXlateTab[sp->sb->operatorId]);
                        }
                    }
                    break;
                case Keyword::quot_:
                    if (sp->tp->BaseType()->syms->size() > 2)
                    {
                        errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    else if (sp->tp->BaseType()->syms->size() == 2)
                    {
                        // two args
                        Type* tpl = (*it)->tp;
                        Type* tpr = (*++it)->tp;
                        if (!tpr->IsUnsigned() || !tpl->IsPtr())
                        {
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                        }
                        else
                        {
                            tpl = tpl->BaseType()->btp;
                            tpr = tpl->BaseType();
                            if (!tpl->IsConst() || (tpr->type != BasicType::char_ && tpr->type != BasicType::wchar_t_ &&
                                                    tpr->type != BasicType::char8_t_ && tpr->type != BasicType::char16_t_ &&
                                                    tpr->type != BasicType::char32_t_))
                            {
                                errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                            }
                        }
                    }
                    else
                    {
                        // one arg
                        Type* tp = (*it)->tp;
                        if ((!tp->IsPtr() || !tp->BaseType()->btp->IsConst() ||
                             tp->BaseType()->btp->BaseType()->type != BasicType::char_) &&
                            tp->type != BasicType::unsigned_long_long_ && tp->type != BasicType::long_double_ &&
                            tp->type != BasicType::char_ && tp->type != BasicType::wchar_t_ && tp->type != BasicType::char8_t_ &&
                            tp->type != BasicType::char16_t_ && tp->type != BasicType::char32_t_)
                        {
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                        }
                    }
                    break;
                case Keyword::new_:
                case Keyword::newa_:
                    if (sp->tp->BaseType()->syms->size() > 0)
                    {
                        // any number of args, but first must be a size
                        Type* tp = (*it)->tp;
                        if (!tp->IsInt())
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    else
                    {
                        errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    break;
                case Keyword::delete_:
                case Keyword::dela_:
                    if (sp->tp->BaseType()->syms->size() > 0)
                    {
                        // one or more args, first must be a pointer
                        Type* tp = (*it)->tp;
                        if (!tp->IsPtr())
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    else
                    {
                        errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    break;
                default:
                    if (sp->sb->operatorId == CI_CAST)
                    {
                        errortype(ERR_OPERATOR_MUST_BE_NONSTATIC, sp->tp->BaseType()->btp, nullptr);
                    }
                    else
                    {
                        errorstr(ERR_OPERATOR_MUST_BE_NONSTATIC, overloadXlateTab[sp->sb->operatorId]);
                    }
                    break;
            }
        }
    }
}
static bool constArgValid(Type* tp)
{
    while (tp->IsArray())
        tp = tp->BaseType()->btp;
    if (tp->IsFunction())
        return false;
    if (tp->type == BasicType::templateparam_ || tp->type == BasicType::templateselector_)
        return true;
    tp->InstantiateDeferred();
    if (tp->IsStructured())
    {
        SYMBOL *sym = tp->BaseType()->sp, *sym1;
        SYMBOL *cpy, *mv;
        BASECLASS* bc;
        tp = tp->BaseType();
        if (sym->sb->trivialCons)
            return true;
        sym1 = search(tp->syms, overloadNameTab[CI_DESTRUCTOR]);
        if (sym1 && !((SYMBOL*)sym1->tp->syms->front())->sb->defaulted)
            return false;
        sym1 = search(tp->syms, overloadNameTab[CI_CONSTRUCTOR]);
        if (sym1)
        {
            cpy = getCopyCons(sym, false);
            mv = getCopyCons(sym, true);
            bool found = false;
            for (auto sym2 : *sym1->tp->syms)
            {
                if (sym2 != cpy && sym2 != mv && isConstexprConstructor(sym2))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                return false;
        }
        if (tp->syms)
        {
            for (auto sp : *tp->syms)
            {
                if (sp->sb->storage_class == StorageClass::member_ && !sp->tp->IsFunction())
                    if (!constArgValid(sp->tp))
                        return false;
            }
        }
        if (sym->sb->baseClasses)
            for (auto bc : *sym->sb->baseClasses)
                if (!constArgValid(bc->cls->tp))
                    return false;
        return true;
    }
    return true;
}
bool MatchesConstFunction(SYMBOL* sym)
{
    if (sym->sb->storage_class == StorageClass::virtual_)
        return false;
    if (!constArgValid(sym->tp->BaseType()->btp))
        return false;
    for (auto sym1 : *sym->tp->BaseType()->syms)
    {
        if (sym1->tp->type != BasicType::void_ && !constArgValid(sym1->tp))
            return false;
    }
    return true;
}
void getDeclType( SYMBOL* funcsp, Type** tn)
{
    bool hasAmpersand = false;
    bool hasAuto = false;
    EXPRESSION *exp, *exp2;
    getsym();
    needkw(Keyword::openpa_);
    bool extended = MATCHKW(Keyword::openpa_);
    hasAmpersand = MATCHKW(Keyword::and_);
    if (extended || hasAmpersand)
    {
        getsym();
        hasAuto = MATCHKW(Keyword::auto_);
        --*currentStream;
    }
    else
    {
        hasAuto = MATCHKW(Keyword::auto_);
    }
    if (hasAuto)
    {
        if (extended || hasAmpersand)
            getsym();
        getsym();
        if (MATCHKW(Keyword::and_) || MATCHKW(Keyword::land_))
        {
            getsym();
            error(ERR_DECLTYPE_AUTO_NO_REFERENCE);
        }
        if (extended)
            needkw(Keyword::closepa_);
        (*tn) = Type::MakeType(BasicType::auto_);
        (*tn)->size = 0;
        (*tn)->decltypeauto = true;
    }
    else
    {
        auto oldnoExcept = noExcept;
        expression_no_check(nullptr, nullptr, &(*tn), &exp, _F_SIZEOF | _F_INDECLTYPE);
        noExcept = oldnoExcept;
        if (exp->type == ExpressionNode::callsite_ && exp->v.func->sp->sb->deleted)
        {
            *tn = &stdany;
            return;
        }
        if ((*tn) && (*tn)->type == BasicType::aggregate_ && exp->type == ExpressionNode::callsite_)
        {
            if ((*tn)->syms->size() > 1)
            {
                auto it = (*tn)->syms->begin();
                auto it1 = it;
                ++it1;
                errorsym2(ERR_AMBIGUITY_BETWEEN, *it, *it1);
            }
            exp->v.func->sp = (*tn)->syms->front();
            if (hasAmpersand)
            {
                if (ismember(exp->v.func->sp))
                {
                    (*tn) = Type::MakeType(BasicType::memberptr_, exp->v.func->sp->tp);
                    (*tn)->sp = exp->v.func->sp->sb->parentClass;
                }
                else
                {
                    (*tn) = Type::MakeType(BasicType::pointer_, exp->v.func->sp->tp);
                    (*tn)->size = getSize(BasicType::pointer_);
                }
                exp->v.func->functp = (*tn)->btp;
            }
            else
            {
                (*tn) = exp->v.func->functp = exp->v.func->sp->tp;
            }
        }
        if ((*tn) && (*tn)->IsFunction() && exp->type == ExpressionNode::callsite_)
        {
            if (exp->v.func->ascall)
            {
                *tn = (*tn)->BaseType()->btp;
            }
        }
        if ((*tn))
        {
            optimize_for_constants(&exp);
            if (IsDefiningTemplate() || ((*tn)->type == BasicType::any_ && !inDeduceArgs))
            {
                (*tn) = Type::MakeType(BasicType::templatedecltype_);
                (*tn)->templateDeclType = exp;
            }
        }
        exp2 = exp;
        if (!(*tn))
        {
            error(ERR_IDENTIFIER_EXPECTED);
            errskim(skim_semi_declare);
            return;
        }
        if ((*tn)->lref)
        {
            (*tn) = Type::MakeType(BasicType::lref_, *tn);
        }
        else if ((*tn)->rref)
        {
            (*tn) = Type::MakeType(BasicType::rref_, *tn);
        }
        if (extended && IsLValue(exp) && exp->left->type == ExpressionNode::auto_)
        {
            if (!lambdas.size() && xvalue(exp))
            {
                if ((*tn)->IsRef())
                    (*tn) = (*tn)->BaseType()->btp;
                (*tn) = Type::MakeType(BasicType::rref_, *tn);
            }
            else if (IsLValue(exp))
            {
                if ((*tn)->IsRef())
                    (*tn) = (*tn)->BaseType()->btp;
                (*tn) = Type::MakeType(BasicType::lref_, *tn);
                if (lambdas.size() && !lambdas.front()->isMutable)
                {
                    (*tn)->btp = Type::MakeType(BasicType::const_, (*tn)->btp);
                }
            }
        }
    }
    needkw(Keyword::closepa_);
    if (*tn && (*tn)->IsAutoType())
        RequiresDialect::Feature(Dialect::cpp14, "decltype(auto)");
    return;
}

void CollapseReferences(Type* tp_in)
{
    Type* tp1 = tp_in;
    int count = 0;
    bool lref = false;
    while (tp1 && tp1->IsRef())
    {
        if (tp1->BaseType()->type == BasicType::lref_)
            lref = true;
        count++;
        tp1 = tp1->BaseType()->btp;
    }
    if (count > 1)
    {
        tp1 = tp_in;
        tp1->BaseType()->type = lref ? BasicType::lref_ : BasicType::rref_;
        tp1 = tp1->BaseType();
        while (tp1->btp->BaseType() && tp1->btp->IsRef())
        {
            // yes we intend to get rid of top-level CV qualifiers, reference collapsing demands it.
            tp1->btp = tp1->btp->btp;
        }
        tp_in->UpdateRootTypes();
    }
    else if (count == 1 && tp_in->BaseType()->btp && tp_in->BaseType()->btp->IsVoid())
    {
        // get rid of rrefs to void...
        *tp_in = stdvoid;
        tp_in->rootType = tp_in;
    }
}
EXPRESSION* addLocalDestructor(EXPRESSION* exp, SYMBOL* decl)
{
    if (decl->sb->dest)
    {
        auto atexitfunc = namespacesearch("atexit", globalNameSpace, false, false);
        if (atexitfunc)
        {
            atexitfunc = atexitfunc->tp->syms->front();
            EXPRESSION** last = &exp;
            while (*last && (*last)->type == ExpressionNode::comma_)
                last = &(*last)->right;
            if (*last)
            {
                *last = MakeExpression(ExpressionNode::comma_, *last);
                last = &(*last)->right;
            }
            auto newFunc =
                makeID(StorageClass::global_, &stdfunc, nullptr, litlate((std::string(decl->sb->decoratedName) + "_dest").c_str()));
            SetLinkerNames(newFunc, Linkage::c_);
            optimize_for_constants(&decl->sb->dest->front()->exp);
            auto body = decl->sb->dest->front()->exp;
            InsertLocalStaticUnInitializer(newFunc, body);

            EXPRESSION* callexp = MakeExpression(Allocate<CallSite>());
            callexp->v.func->sp = atexitfunc;
            callexp->v.func->functp = atexitfunc->tp;
            callexp->v.func->fcall = MakeExpression(ExpressionNode::pc_, atexitfunc);
            callexp->v.func->ascall = true;
            callexp->v.func->arguments = argumentListFactory.CreateList();
            auto arg = Allocate<Argument>();
            callexp->v.func->arguments->push_back(arg);
            arg->tp = &stdpointer;
            arg->exp = MakeExpression(ExpressionNode::pc_, newFunc);
            *last = callexp;
        }
    }
    return exp;
}
void CheckIsLiteralClass(Type* tp)
{
    if (!IsDefiningTemplate())
    {
        if (tp->IsRef())
            tp = tp->BaseType()->btp;
        if (tp->IsStructured())
        {
            if (tp->BaseType()->sp->tp->size && !tp->BaseType()->sp->sb->literalClass)
            {
                errorsym(ERR_CONSTEXPR_CLASS_NOT_LITERAL, tp->BaseType()->sp);
            }
            else if (tp->BaseType()->sp->templateParams)
            {
                for (auto&& tpl : *tp->BaseType()->sp->templateParams)
                    if (tpl.second->type == TplType::typename_ && tpl.second->byClass.val)
                        CheckIsLiteralClass(tpl.second->byClass.val);
            }
        }
    }
}
static int CountMembers(SYMBOL* sym)
{
    int count = 0;
    if (sym->tp->syms)
    {
        for (auto d : *sym->tp->syms)
        {
            if (ismember(d))
            {
                count++;
            }
        }
    }
    return count;
}

void  GetStructuredBinding( SYMBOL* funcsp, StorageClass storage_class, Linkage linkage,
                              std::list<FunctionBlock*>& block)
{
    getsym();
    if (!ISID())
    {
        errskim(skim_semi_declare);
        error(ERR_IDENTIFIER_EXPECTED);
    }
    else
    {
        std::deque<std::string> identifiers;
        do
        {
            identifiers.push_back(currentLex->value.s.a);
            getsym();
            if (ISID())
                break;
            if (MATCHKW(Keyword::comma_))
                getsym();
        } while (currentLex && ISID());
        needkw(Keyword::closebr_);
        if (!needkw(Keyword::assign_))
        {
            errskim(skim_semi_declare);
        }
        else
        {
            EXPRESSION* exp = nullptr;
            Type* tp = nullptr;
            expression_no_comma(funcsp, nullptr, &tp, &exp, nullptr, 0);
            currentLineData(block, currentLex,-1);
            if (tp == nullptr)
            {
                error(ERR_EXPRESSION_SYNTAX);
                errskim(skim_semi_declare);
            }
            else if ((!tp->IsStructured() && !tp->IsArray()) || tp->BaseType()->type == BasicType::union_)
            {
                errortype(ERR_STRUCTURED_BINDING_STRUCT_ARRAY, tp, nullptr);
            }
            else if (tp->IsArray())
            {
                auto btp = tp->BaseType()->btp->BaseType();
                int count = tp->BaseType()->size / btp->size;
                if (count != identifiers.size())
                {
                    error(ERR_STRUCTURED_BINDING_ARRAY_MISMATCH);
                }
                else
                {
                    auto copy = AnonymousVar(storage_class, btp);
                    if (btp->IsStructured())
                    {
                        std::list<Initializer*>* constructors = nullptr;

                        for (int i = 0; i < identifiers.size(); i++)
                        {
                            auto src = i ? MakeExpression(ExpressionNode::add_, exp,
                                                          MakeIntExpression(ExpressionNode::c_i_, i * btp->size))
                                         : exp;
                            auto dest = i ? MakeExpression(ExpressionNode::add_, copy,
                                                           MakeIntExpression(ExpressionNode::c_i_, i * btp->size))
                                          : copy;
                            Type* ctype = btp;
                            auto expx = dest;
                            callConstructorParam(&ctype, &expx, btp, src, true, false, true, false, true);
                            if (storage_class != StorageClass::auto_ && storage_class != StorageClass::localstatic_ &&
                                storage_class != StorageClass::parameter_ && storage_class != StorageClass::member_ &&
                                storage_class != StorageClass::mutable_)
                            {
                                InsertInitializer(&constructors, tp, expx, i * btp->size, true);
                            }
                            else
                            {
                                auto st = Statement::MakeStatement(block, StatementNode::expr_);
                                st->select = expx;
                            }
                        }
                        std::list<Initializer*>* destructors = nullptr;
                        EXPRESSION* sz = nullptr;
                        if (identifiers.size() > 1)
                        {
                            sz = MakeIntExpression(ExpressionNode::c_i_, identifiers.size());
                        }
                        auto expy = copy;
                        CallDestructor(btp->sp, nullptr, &expy, sz, true, false, false, true);
                        InsertInitializer(&destructors, tp, expy, 0, true);
                        if (storage_class != StorageClass::auto_ && storage_class != StorageClass::localstatic_ &&
                            storage_class != StorageClass::parameter_ && storage_class != StorageClass::member_ &&
                            storage_class != StorageClass::mutable_)
                        {
                            insertDynamicInitializer(copy->v.sp, constructors);
                            insertDynamicDestructor(copy->v.sp, destructors);
                        }
                        else
                        {
                            copy->v.sp->sb->dest = destructors;
                        }
                    }
                    else
                    {
                        auto epc = MakeExpression(ExpressionNode::blockassign_, copy, exp);
                        epc->size = tp;
                        epc->altdata = (void*)(tp);
                        if (storage_class != StorageClass::auto_ && storage_class != StorageClass::localstatic_ &&
                            storage_class != StorageClass::parameter_ && storage_class != StorageClass::member_ &&
                            storage_class != StorageClass::mutable_)
                        {
                            std::list<Initializer*>* constructors = initListFactory.CreateList();
                            InsertInitializer(&constructors, tp, epc, 0, false);
                            insertDynamicInitializer(copy->v.sp, constructors);
                        }
                        else
                        {
                            auto st = Statement::MakeStatement(block, StatementNode::expr_);
                            st->select = epc;
                        }
                    }
                    int i = 0;
                    for (const auto& id : identifiers)
                    {
                        auto sym = makeID(StorageClass::alias_, btp, nullptr, litlate(id.c_str()));
                        InsertSymbol(sym, storage_class, linkage, false);
                        InsertInitializer(&sym->sb->init, btp, copy, (i++) * btp->size, true);
                        if (storage_class == StorageClass::auto_ || storage_class == StorageClass::localstatic_)
                        {
                            Statement* s = Statement::MakeStatement(block, StatementNode::varstart_);
                            s->select = MakeExpression(ExpressionNode::auto_, sym);
                        }
                    }
                }
            }
            else
            {
                SYMBOL *one = nullptr, *two = nullptr;
                int count = 0;
                int offset = 0;
                auto structsym = tp->BaseType()->sp;
                if (structsym->sb->baseClasses)
                {
                    for (auto b : *structsym->sb->baseClasses)
                    {
                        int count1 = CountMembers(b->cls);
                        if (count1)
                        {
                            if (one)
                            {
                                two = b->cls;
                                break;
                            }
                            else
                            {
                                one = b->cls;
                                offset = b->offset;
                                count = count1;
                            }
                        }
                    }
                }
                int count1 = CountMembers(structsym);
                if (count1)
                {
                    if (one)
                    {
                        two = structsym;
                    }
                    else
                    {
                        count = count1;
                        offset = 0;
                        one = structsym;
                    }
                }
                if (!one)
                {
                    errorsym(ERR_STRUCTURED_BINDING_CANT_DECOMPOSE_NONE, structsym);
                }
                else if (two)
                {
                    errorsym2(ERR_STRUCTURED_BINDING_CANT_DECOMPOSE_ALL, two, one);
                }
                else if (count != identifiers.size())
                {
                    error(ERR_STRUCTURED_BINDING_STRUCT_MISMATCH);
                }
                else
                {
                    if (one != structsym)
                    {
                        if (structsym->sb->vbaseEntries)
                        {
                            for (auto v : *structsym->sb->vbaseEntries)
                                if (one == v->cls)
                                {
                                    offset = v->structOffset;
                                    break;
                                }
                        }
                    }
                    auto copy = AnonymousVar(storage_class, tp);
                    std::list<Initializer*>* constructors = nullptr;
                    std::list<Initializer*>* destructors = nullptr;

                    auto src = exp;
                    auto dest = copy;
                    Type* ctype = tp;

                    auto expx = dest;
                    callConstructorParam(&ctype, &expx, tp, src, true, false, true, false, true);
                    if (storage_class != StorageClass::auto_ && storage_class != StorageClass::localstatic_ &&
                        storage_class != StorageClass::parameter_ && storage_class != StorageClass::member_ &&
                        storage_class != StorageClass::mutable_)
                    {
                        InsertInitializer(&constructors, tp, expx, 0, true);
                    }
                    else
                    {
                        auto st = Statement::MakeStatement(block, StatementNode::expr_);
                        st->select = expx;
                    }
                    expx = dest;
                    CallDestructor(tp->sp, nullptr, &expx, nullptr, true, false, false, true);
                    InsertInitializer(&destructors, tp, expx, 0, true);
                    if (storage_class != StorageClass::auto_ && storage_class != StorageClass::localstatic_ &&
                        storage_class != StorageClass::parameter_ && storage_class != StorageClass::member_ &&
                        storage_class != StorageClass::mutable_)
                    {
                        insertDynamicInitializer(copy->v.sp, constructors);
                        insertDynamicDestructor(copy->v.sp, destructors);
                    }
                    else
                    {
                        copy->v.sp->sb->dest = destructors;
                    }

                    auto it = one->tp->syms->begin();
                    for (const auto& id : identifiers)
                    {
                        while (!ismember(*it))
                            ++it;
                        auto sym = makeID(StorageClass::alias_, (*it)->tp, nullptr, litlate(id.c_str()));
                        SetLinkerNames(sym, Linkage::cdecl_);
                        InsertSymbol(sym, storage_class, linkage, false);
                        InsertInitializer(&sym->sb->init, (*it)->tp, copy, (*it)->sb->offset + offset, true);
                        if (storage_class == StorageClass::auto_ || storage_class == StorageClass::localstatic_)
                        {
                            Statement* s = Statement::MakeStatement(block, StatementNode::varstart_);
                            s->select = MakeExpression(ExpressionNode::auto_, sym);
                        }
                        ++it;
                    }
                }
            }
        }
    }
    return;
}

}  // namespace Parser
