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

#include <unordered_set>
#include "compiler.h"
#include <stack>
#include "ccerr.h"
#include "namespace.h"
#include "class.h"
#include "overload.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "declare.h"
#include "mangle.h"
#include "lambda.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "declcpp.h"
#include "expr.h"
#include "lex.h"
#include "help.h"
#include "unmangle.h"
#include "types.h"
#include "OptUtils.h"
#include "memory.h"
#include "beinterf.h"
#include "exprcpp.h"
#include "inline.h"
#include "iexpr.h"
#include "libcxx.h"
#include "declcons.h"
#include "ListFactory.h"
#include "constopt.h"
#include "symtab.h"
#include "Utils.h"
#include "exprpacked.h"
#include "using.h"
#include "SymbolProperties.h";

namespace Parser
{

template <size_t n>
static void GetUsingName(char (&buf)[n])
{
    auto sym = enclosingDeclarations.GetFirst();
    if (sym)
    {
        DeclarationScope scope(sym);
        auto sp = classsearch(buf, false, false, false);
        if (sp && sp->sb && sp->sb->usingTypedef)
        {
            if (sp->tp->IsStructured())
                Utils::StrCpy(buf, sp->tp->BaseType()->sp->name);
        }
    }
}
void nestedPath( SYMBOL** sym, std::list<NAMESPACEVALUEDATA*>** ns, bool* throughClass, bool tagsOnly,
                    StorageClass storage_class, bool isType, int flags)
{
    (void)tagsOnly;
    (void)storage_class;
    bool first = true;
    std::list<NAMESPACEVALUEDATA*>* nssym = globalNameSpace;
    SYMBOL* strSym = nullptr;
    bool qualified = false;
    std::vector<TEMPLATESELECTOR>* templateSelector = nullptr;
    LexemeStreamPosition placeHolder(currentStream);
    bool hasTemplate = false;
    TEMPLATEPARAMPAIR* templateParamAsTemplate = nullptr;
    Type* dependentType = nullptr;
    bool typeName = false;
    bool pastClassSel = false;
    std::list<TEMPLATEPARAMPAIR>* current = nullptr;

    if (sym)
        *sym = nullptr;
    if (ns)
        *ns = nullptr;

    if (MATCHKW(Keyword::typename_))
    {
        typeName = true;
        getsym();
    }
    if (MATCHKW(Keyword::classsel_))
    {
        nssym = rootNameSpace;
        getsym();
        qualified = true;
    }

    LexemeStreamPosition finalPos(currentStream);

    while (ISID() || (first && MATCHKW(Keyword::decltype_)) || (templateSelector && MATCHKW(Keyword::operator_)))
    {
        char buf[512];
        SYMBOL* sp = nullptr;
        int ovdummy;
        if (first && MATCHKW(Keyword::decltype_))
        {
            Type* tp = nullptr;
            getDeclType(theCurrentFunc, &tp);
            if (!tp || (!tp->IsStructured() && tp->type != BasicType::templatedecltype_) || !MATCHKW(Keyword::classsel_))
                break;
            if (tp->IsAutoType())
                RequiresDialect::Feature(Dialect::cpp14, "decltype(auto)");
            getsym();
            if (tp->type == BasicType::templatedecltype_)
            {
                if (!templateSelector)
                    templateSelector = templateSelectorListFactory.CreateVector();
                templateSelector->push_back(TEMPLATESELECTOR{});
                templateSelector->back().sp = strSym;
                templateSelector->push_back(TEMPLATESELECTOR{});
                templateSelector->back().tp = tp;
                templateSelector->back().isDeclType = true;
            }
            else
            {
                sp = tp->BaseType()->sp;
                sp->tp->InstantiateDeferred();
                strSym = sp;
            }
            if (!qualified)
                nssym = nullptr;
            finalPos.Bump();
        }
        else if (templateSelector)
        {
            getIdName(nullptr, buf, sizeof(buf), & ovdummy, nullptr);
            getsym();

            if (!typeName)
                GetUsingName(buf);

            if (!templateSelector)
                templateSelector = templateSelectorListFactory.CreateVector();
            templateSelector->push_back(TEMPLATESELECTOR{});
            templateSelector->back().name = litlate(buf);
            if (hasTemplate)
            {
                templateSelector->back().isTemplate = true;
                if (MATCHKW(Keyword::lt_))
                {
                    GetTemplateArguments(nullptr, nullptr, &templateSelector->back().templateParams);
                }
                else if (MATCHKW(Keyword::classsel_))
                {
                    SpecializationError(buf);
                }
            }
            if ((!inTemplateType || processingUsingStatement) && MATCHKW(Keyword::openpa_))
            {
                CallSite funcparams = {};
                getArgs(theCurrentFunc, &funcparams, Keyword::closepa_, true, 0);
                templateSelector->back().arguments = funcparams.arguments;
                templateSelector->back().asCall = true;
            }
            if (!MATCHKW(Keyword::classsel_))
                break;
            getsym();
            finalPos.Bump();
        }
        else
        {
            SYMBOL* sp_orig;
            getIdName(nullptr, buf, sizeof(buf), &ovdummy, nullptr);
            getsym();
            bool hasTemplateArgs = false;
            bool deferred = false;
            bool istypedef = false;
            SYMBOL* currentsp = nullptr;
            if (!strSym)
            {
                TEMPLATEPARAMPAIR* tparam = TemplateLookupSpecializationParam(buf);
                if (tparam)
                {
                    sp = tparam->first;
                }
                else if (!qualified)
                {
                    sp = nullptr;
                    if (defaultParsingContext && !defaultParsingContext->sb->parentClass)
                    {
                        // if parsing default args, need to give precedence to the global namespace
                        // instead of drawing immediately from open classes.
                        sp = namespacesearch(buf, localNameSpace, qualified, tagsOnly);
                        if (!sp && nssym)
                        {
                            sp = namespacesearch(buf, nssym, qualified, tagsOnly);
                        }
                    }
                    if (!sp)
                    {
                        for (auto t : lambdas)
                        {
                            if (t->lthis)
                            {
                                DeclarationScope scope(t->lthis);
                                sp = classsearch(buf, false, MATCHKW(Keyword::classsel_), false);
                            }
                            if (sp)
                                break;
                        }
                        if (!sp)
                            sp = classsearch(buf, false, MATCHKW(Keyword::classsel_), false);
                        if (sp && sp->tp->type == BasicType::templateparam_)
                        {
                            TEMPLATEPARAMPAIR* params = sp->tp->templateParam;
                            if (params->second->type == TplType::typename_)
                            {
                                if (params->second->packed && params->second->byPack.pack && params->second->byPack.pack->size())
                                {
                                    params = &params->second->byPack.pack->front();
                                }
                                if (params && params->second->byClass.val)
                                {
                                    if (!IsDefiningTemplate())
                                        params->second->byClass.val->InstantiateDeferred();
                                    if (params->second->byClass.val->IsStructured())
                                    {
                                        sp = params->second->byClass.val->BaseType()->sp;
                                        dependentType = params->second->byClass.val;
                                    }
                                }
                            }
                            else if (params->second->type == TplType::template_)
                            {
                                if (params->second->byTemplate.val)
                                {
                                    templateParamAsTemplate = params;
                                    sp = params->second->byTemplate.val;
                                }
                                else
                                {
                                    if (MATCHKW(Keyword::lt_))
                                    {
                                        GetTemplateArguments(nullptr, sp, &current);
                                    }
                                    if (!MATCHKW(Keyword::classsel_))
                                        break;
                                    getsym();
                                    finalPos.Bump();
                                    if (!templateSelector)
                                        templateSelector = templateSelectorListFactory.CreateVector();
                                    templateSelector->push_back(TEMPLATESELECTOR{});
                                    templateSelector->back().sp = sp;
                                    templateSelector->push_back(TEMPLATESELECTOR{});
                                    templateSelector->back().sp = sp;
                                    templateSelector->back().templateParams = current;
                                    templateSelector->back().isTemplate = true;
                                }
                            }
                            else
                                break;
                        }
                        else if (sp && sp->tp->IsDeferred() && sp->tp->BaseType()->sp->sb->templateLevel)
                        {
                            dependentType = sp->tp->BaseType()->sp->tp;
                        }
                        if (sp && throughClass)
                            *throughClass = true;
                    }
                }
                else
                {
                    sp = nullptr;
                }
                if (!sp && !templateParamAsTemplate)
                {
                    SYMBOL* cached = nullptr;
                    if (!qualified)
                        sp = namespacesearch(buf, localNameSpace, qualified, tagsOnly);
                    if (sp && MATCHKW(Keyword::classsel_) && !istype(sp))
                    {
                        cached = sp;
                        sp = nullptr;
                    }
                    if (!sp && nssym)
                    {
                        sp = namespacesearch(buf, nssym, qualified, tagsOnly);
                    }
                    if (!sp)
                        sp = cached;
                }
                if (sp && sp->sb && sp->sb->storage_class == StorageClass::typedef_ && !sp->sb->typeAlias)
                {
                    SYMBOL* typedefSym = sp;
                    istypedef = true;
                    if (sp->tp->IsStructured() && !sp->sb->templateLevel && throughClass)
                    {
                        sp = sp->tp->BaseType()->sp;
                        sp->sb->typedefSym = typedefSym;
                        *throughClass = true;
                    }
                    else if (sp->tp->type == BasicType::typedef_)
                    {
                        if (sp->tp->btp->type == BasicType::typedef_)
                        {
                            sp = sp->tp->btp->sp;
                        }
                        else if (sp->tp->btp->IsStructured())
                        {
                            sp = sp->tp->btp->BaseType()->sp;
                        }
                        else
                        {
                            SYMBOL* sp1 = CopySymbol(sp);
                            sp1->sb->mainsym = sp;
                            sp1->tp = sp->tp->btp;
                            sp = sp1;
                        }
                    }
                }
                sp_orig = sp;
            }
            else
            {
                strSym->tp = strSym->tp->InitializeDeferred();
                if (!typeName)
                    GetUsingName(buf);

                if (structLevel && !templateDefinitionLevel && strSym->sb->templateLevel &&
                    (!strSym->sb->instantiated || strSym->sb->attribs.inheritable.linkage4 != Linkage::virtual_))
                {
                    sp = nullptr;
                }
                else
                {
                    DeclarationScope scope(strSym);
                    sp = classsearch(buf, false, MATCHKW(Keyword::classsel_), false);
                }
                if (!sp)
                {
                    if (!templateSelector)
                        templateSelector = templateSelectorListFactory.CreateVector();
                    templateSelector->push_back(TEMPLATESELECTOR{});
                    templateSelector->back().sp = nullptr;
                    templateSelector->push_back(TEMPLATESELECTOR{});
                    templateSelector->back().sp = strSym;
                    templateSelector->back().templateParams = current;
                    templateSelector->back().isTemplate = true;
                    templateSelector->push_back(TEMPLATESELECTOR{});
                    templateSelector->back().name = litlate(buf);
                    if (hasTemplate)
                    {
                        templateSelector->back().isTemplate = true;
                        if (MATCHKW(Keyword::lt_))
                        {
                            GetTemplateArguments(nullptr, nullptr, &templateSelector->back().templateParams);
                        }
                        else if (MATCHKW(Keyword::classsel_))
                        {
                            errorstr(ERR_NEED_TEMPLATE_ARGUMENTS, buf);
                        }
                    }
                    if (!MATCHKW(Keyword::classsel_))
                        break;
                    getsym();
                    finalPos.Bump();
                }
                sp_orig = sp;
                if (sp && sp->sb && sp->sb->typeAlias && !sp->sb->templateLevel && sp->tp->IsStructured())
                {
                    istypedef = true;
                    sp = sp->tp->BaseType()->sp;
                }
                else if (sp && sp->sb && sp->tp->type == BasicType::typedef_ && (templateDefinitionLevel || !sp->sb->templateLevel))
                {
                    istypedef = true;
                    if (sp->tp->btp->type == BasicType::typedef_)
                    {
                        sp = sp->tp->btp->sp;
                    }
                    else if (sp->tp->btp->IsStructured())
                    {
                        sp = sp->tp->btp->BaseType()->sp;
                    }
                    else
                    {
                        SYMBOL* sp1 = CopySymbol(sp);
                        sp1->sb->mainsym = sp;
                        sp1->tp = sp->tp->btp;
                        sp = sp1;
                    }
                    sp_orig = sp;
                }
            }
            if (!templateSelector)
            {
                if (sp && sp->tp->BaseType()->type == BasicType::enum_)
                {
                    if (!MATCHKW(Keyword::classsel_))
                        break;
                    getsym();
                    finalPos.Bump();
                    strSym = sp;
                    qualified = true;
                    break;
                }
                else if (sp)
                {
                    if (sp->sb && sp->sb->templateLevel && (!sp->sb->instantiated || MATCHKW(Keyword::lt_)))
                    {
                        hasTemplateArgs = true;
                        if (MATCHKW(Keyword::lt_))
                        {
                            current = nullptr;
                            GetTemplateArguments(nullptr, sp_orig, &current);
                        }
                        else if (MATCHKW(Keyword::classsel_))
                        {
                            currentsp = sp;
                            if (!istypedef)
                                SpecializationError(sp);
                        }
                        if (!MATCHKW(Keyword::classsel_))
                            break;
                    }
                    else
                    {
                        if (!MATCHKW(Keyword::classsel_))
                            break;
                        if (hasTemplate && (sp->tp->BaseType()->type != BasicType::templateparam_ ||
                                            sp->tp->BaseType()->templateParam->second->type != TplType::template_))
                        {
                            errorsym(ERR_NOT_A_TEMPLATE, sp);
                        }
                    }
                }
                else if (templateParamAsTemplate)
                {
                    hasTemplateArgs = true;
                    if (MATCHKW(Keyword::lt_))
                    {
                        GetTemplateArguments(nullptr, sp, &current);
                    }
                    else if (MATCHKW(Keyword::classsel_))
                    {
                        currentsp = sp;
                        SpecializationError(sp);
                    }
                    if (!MATCHKW(Keyword::classsel_))
                        break;
                }
                else if (!MATCHKW(Keyword::classsel_))
                    break;
                if (templateParamAsTemplate)
                {
                    matchTemplateSpecializationToParams(
                        current, templateParamAsTemplate->second->byTemplate.args,
                        templateParamAsTemplate->first);  // this function is apparently undefined in this file
                }
                if (hasTemplateArgs)
                {
                    deferred =
                        processingTemplateHeader || parsingSpecializationDeclaration || processingTrailingReturnOrUsing || (flags & _F_NOEVAL);
                    if (currentsp)
                    {
                        sp = currentsp;
                        if (inTemplateType)
                        {
                            deferred = true;
                        }
                    }
                    else if (inTemplateType)
                    {
                        deferred = true;
                    }
                    else
                    {
                        if (isType && current)
                        {
                            for (auto&& p : *current)
                            {
                                if (!p.second->byClass.dflt)
                                {
                                    deferred = true;
                                    break;
                                }
                            }
                        }
                        if (!deferred && sp)
                        {
                            if (sp->tp->BaseType()->type == BasicType::templateselector_)
                            {
                                if (sp->sb->mainsym && sp->sb->mainsym->sb->storage_class == StorageClass::typedef_ &&
                                    sp->sb->mainsym->sb->templateLevel)
                                {
                                    SYMBOL* sp1 = GetTypeAliasSpecialization(sp->sb->mainsym, current);
                                    if (sp1 && (!sp1->sb->templateLevel || sp1->sb->instantiated))
                                    {
                                        sp = sp1;
                                        qualified = false;
                                    }
                                    else
                                    {
                                        deferred = true;
                                    }
                                }
                                else
                                {
                                    deferred = true;
                                }
                            }
                            else
                            {
                                if (current)
                                {
                                    for (auto&& p : *current)
                                    {
                                        if (p.second->usedAsUnpacked)
                                        {
                                            deferred = true;
                                            break;
                                        }
                                    }
                                }
                                if (!deferred)
                                {
                                    SYMBOL* sp1 = sp;
                                    if (sp->sb->storage_class == StorageClass::typedef_)
                                    {
                                        if (sp->tp->type == BasicType::templatedecltype_)
                                        {
                                            auto tp = LookupTypeFromExpression(sp->tp->templateDeclType, nullptr, false);
                                            if (!tp)
                                            {
                                                sp = nullptr;
                                            }
                                            else
                                            {
                                                tp->InstantiateDeferred();
                                                if (tp->IsStructured())
                                                {
                                                    sp = tp->BaseType()->sp;
                                                }
                                                else
                                                {
                                                    sp1 = CopySymbol(sp);
                                                    sp1->sb->mainsym = sp;
                                                    sp1->tp = tp;
                                                    sp = sp1;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            sp = GetTypeAliasSpecialization(sp, current);
                                            if (sp && sp->tp->IsStructured())
                                                sp = sp->tp->BaseType()->sp;
                                        }
                                    }
                                    else
                                    {
                                        sp = GetClassTemplate(sp, current, false);
                                    }
                                    if (!sp)
                                    {
                                        if (templateDefinitionLevel)  // || noSpecializationError)
                                        {
                                            sp = sp1;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (sp && !deferred)
                {
                    // order matters here...
                    sp->tp = sp->tp->InitializeDeferred();
                    sp->tp->InstantiateDeferred(false, isFullySpecialized);
                }
                if (sp && (!sp->sb ||
                           (sp->sb->storage_class != StorageClass::namespace_ && (!sp->tp->IsStructured() || sp->templateParams))))
                    pastClassSel = true;
                getsym();
                finalPos.Bump();
                if (deferred)
                {
                    if (istypedef && sp->sb->mainsym && sp->sb->mainsym->sb->templateLevel)
                    {
                        sp->tp = sp->sb->mainsym->tp;
                    }
                    if (sp && sp->tp->type == BasicType::templateselector_)
                    {
                        if (sp->tp->BaseType()->sp->sb->templateSelector)
                        {
                            for (auto s : *sp->tp->BaseType()->sp->sb->templateSelector)
                                templateSelector->push_back(s);
                        }
                        (*templateSelector)[1].templateParams = current;
                        (*templateSelector)[1].isTemplate = true;
                    }
                    else
                    {
                        if (!templateSelector)
                            templateSelector = templateSelectorListFactory.CreateVector();
                        templateSelector->push_back(TEMPLATESELECTOR{});
                        templateSelector->back().sp = strSym;
                        templateSelector->push_back(TEMPLATESELECTOR{});
                        templateSelector->back().sp = sp;
                        templateSelector->back().templateParams = current;
                        templateSelector->back().isTemplate = true;
                    }
                }
                else if (sp && sp->tp->IsStructured())
                {
                    strSym = sp->tp->BaseType()->sp;
                    if (!qualified)
                        nssym = nullptr;
                }
                else if (sp && sp->sb &&
                         (sp->sb->storage_class == StorageClass::namespace_ ||
                          sp->sb->storage_class == StorageClass::namespace_alias_))
                {
                    nssym = sp->sb->nameSpaceValues;
                }
                else if (sp && (sp->tp->BaseType()->type == BasicType::templateparam_ ||
                                sp->tp->BaseType()->type == BasicType::templateselector_))
                {
                    if (!templateSelector)
                        templateSelector = templateSelectorListFactory.CreateVector();
                    templateSelector->push_back(TEMPLATESELECTOR{});
                    templateSelector->back().sp = strSym;
                    templateSelector->push_back(TEMPLATESELECTOR{});
                    templateSelector->back().sp = sp;
                }
                else
                {
                    if (!templateDefinitionLevel || !sp)
                    {
                        if (dependentType)
                            if (dependentType->IsStructured())
                                errorstringtype(ERR_DEPENDENT_TYPE_DOES_NOT_EXIST_IN_TYPE, buf, dependentType->BaseType());
                            else
                                errortype(ERR_DEPENDENT_TYPE_NOT_A_CLASS_OR_STRUCT, dependentType, nullptr);
                        else if (!IsPacking())
                            errorstr(ERR_QUALIFIER_NOT_A_CLASS_OR_NAMESPACE, buf);
                    }
                    placeHolder.Backup();
                    strSym = sp;
                    qualified = true;
                    break;
                }
            }
        }
        first = false;
        hasTemplate = false;
        if (MATCHKW(Keyword::template_))
        {
            hasTemplate = true;
            getsym();
        }
        qualified = true;
    }
    if (pastClassSel && !typeName && !processingTypedef && !hasTemplate && isType && !noTypeNameError)
    {

        if (!strSym || templateSelector)
        {
            char buf[2000];
            buf[0] = 0;

            LexemeStreamPosition end = finalPos;
            ++end;
            for ( ; placeHolder != end; ++placeHolder)
            {
                int l = strlen(buf);
                if (ISKW())
                    Optimizer::my_sprintf(buf + l , sizeof(buf)-l, "%s", placeHolder.get()->kw->name);
                else if (ISID())
                    Optimizer::my_sprintf(buf + l, sizeof(buf) - l, "%s", placeHolder.get()->value.s.a);
            }

            errorstr(ERR_DEPENDENT_TYPE_NEEDS_TYPENAME, buf);
        }
    }
    if (!pastClassSel && typeName && !dependentType && !processingTypedef && !processingUsingStatement && (!IsDefiningTemplate()))
    {
        error(ERR_NO_TYPENAME_HERE);
    }
    finalPos.Backup();
    if (templateSelector)
    {
        auto tp = Type::MakeType(BasicType::templateselector_);
        *sym = makeID(StorageClass::global_, tp, nullptr, AnonymousName());
        (*sym)->sb->templateSelector = templateSelector;
        tp->sp = *sym;
    }
    else if (qualified)
    {
        if (strSym && sym)
            *sym = strSym;

        if (ns)
            if (nssym)
                *ns = nssym;
            else
                *ns = nullptr;
        else
            error(ERR_QUALIFIED_NAME_NOT_ALLOWED_HERE);
    }
    return;
}
SYMBOL* classdata(const char* name, SYMBOL* cls, SYMBOL* last, bool isvirtual, bool tagsOnly)
{
    SYMBOL* rv = nullptr;
    if (cls->sb->storage_class == StorageClass::typedef_)
        cls = cls->tp->BaseType()->sp;
    if (cls->sb->templateLevel && cls->templateParams)
    {
        if (!cls->tp->BaseType()->syms)
        {
            TemplateClassInstantiate(cls, cls->templateParams, false, StorageClass::global_);
        }
    }
    if (cls->sb->baseClasses)
    {
        for (auto bc : *cls->sb->baseClasses)
        {
            if (rv)
                break;
            if (!strcmp(bc->cls->name, name))
            {
                rv = bc->cls;
                rv->sb->temp = bc->isvirtual;
            }
        }
    }

    if (!rv && !tagsOnly)
        rv = search(cls->tp->BaseType()->syms, name);
    if (!rv)
        rv = search(cls->tp->BaseType()->tags, name);
    if (rv)
    {
        if (!last || ((last == rv || SameTemplate(last->tp, rv->tp) || (rv->sb->mainsym && rv->sb->mainsym == last->sb->mainsym)) &&
                      (((isvirtual && isvirtual == last->sb->temp) || ismember(rv)) ||
                       (((last->sb->storage_class == StorageClass::type_ && rv->sb->storage_class == StorageClass::type_) ||
                         (last->sb->storage_class == StorageClass::typedef_ && rv->sb->storage_class == StorageClass::typedef_)) &&
                        (last->sb->parentClass == rv->sb->parentClass)) ||
                       last->sb->parentClass->sb->mainsym == rv->sb->parentClass->sb->mainsym)))
        {
        }
        else
        {
            rv = (SYMBOL*)-1;
        }
    }
    else
    {
        rv = last;
        if (cls->sb->baseClasses)
            for (auto lst : *cls->sb->baseClasses)
            {
                rv = classdata(name, lst->cls, rv, isvirtual | lst->isvirtual, tagsOnly);
                if (rv == (SYMBOL*)-1)
                    break;
            }
    }
    return rv;
}
SYMBOL* templatesearch(const char* name, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (args && args->size())
    {
        auto old = args->front().second->type == TplType::new_ ? args->front().second->bySpecialization.next : nullptr;
        for (auto&& arg : *args)
        {
            if (arg.first && !strcmp(arg.first->name, name))
            {
                if (arg.second->type == TplType::template_ && arg.second->byTemplate.dflt)
                {
                    return arg.second->byTemplate.dflt;
                }
                else
                {
                    arg.first->tp->templateParam = &arg;
                    return arg.first;
                }
            }
        }
        if (old)
        {
            return templatesearch(name, old);
        }
    }
    return nullptr;
}
TEMPLATEPARAMPAIR* getTemplateStruct(char* name)
{
    SYMBOL* cls = enclosingDeclarations.GetFirst();
    while (cls)
    {
        if (cls->templateParams)
        {
            for (auto&& arg : *cls->templateParams)
            {
                if (!strcmp(arg.first->name, name))
                    return &arg;
            }
        }
        cls = cls->sb->parentClass;
    }
    return nullptr;
}
void tagsearch( char* name, int len, SYMBOL** rsp, SymbolTable<SYMBOL>** table, SYMBOL** strSym_out,
                   std::list<NAMESPACEVALUEDATA*>** nsv_out, StorageClass storage_class)
{
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    SYMBOL* strSym = nullptr;

    *rsp = nullptr;
    if (ISID() || MATCHKW(Keyword::classsel_))
    {
        nestedSearch(rsp, &strSym, &nsv, nullptr, nullptr, true, storage_class, false, false);
        if (*rsp)
        {
            Utils::StrCpy(name, len, (*rsp)->name);
            getsym();
            if (MATCHKW(Keyword::begin_))
            {
                // specify EXACTLY the first result if it is a definition
                // otherwise what is found by nestedSearch is fine...
                if (strSym)
                    *rsp = strSym->tp->tags->Lookup((*rsp)->name);
                else if (nsv)
                    *rsp = nsv->front()->tags->Lookup((*rsp)->name);
                else if (Optimizer::cparams.prm_cplusplus &&
                         (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_))
                    *rsp = enclosingDeclarations.GetFirst()->tp->tags->Lookup((*rsp)->name);
                else if (storage_class == StorageClass::auto_)
                    *rsp = localNameSpace->front()->tags->Lookup((*rsp)->name);
                else
                    *rsp = globalNameSpace->front()->tags->Lookup((*rsp)->name);
                if (!*rsp)
                {
                    if (nsv || strSym)
                    {
                        errorNotMember(strSym, nsv->front(), (*rsp)->sb->decoratedName);
                    }
                    *rsp = nullptr;
                }
            }
        }
        else if (ISID())
        {
            Utils::StrCpy(name, len, currentLex->value.s.a);
            getsym();
            if (MATCHKW(Keyword::begin_))
            {
                if (nsv || strSym)
                {
                    errorNotMember(strSym, nsv->front(), name);
                }
            }
        }
    }
    if (nsv)
    {
        *table = nsv->front()->tags;
    }
    else if (strSym)
    {
        *table = strSym->tp->tags;
    }
    else if (Optimizer::cparams.prm_cplusplus &&
             (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_))
    {
        strSym = enclosingDeclarations.GetFirst();
        *table = strSym->tp->tags;
    }
    else
    {
        if (storage_class == StorageClass::auto_)
        {
            *table = localNameSpace->front()->tags;
            nsv = localNameSpace;
        }
        else
        {
            *table = globalNameSpace->front()->tags;
            nsv = globalNameSpace;
        }
    }
    *nsv_out = nsv;
    *strSym_out = strSym;
    return;
}
SYMBOL* classsearch(const char* name, bool tagsOnly, bool needTypeOrNamespace, bool toErr)
{
    SYMBOL* rv = nullptr;
    SYMBOL* symrv = nullptr;
    SYMBOL* cls = enclosingDeclarations.GetFirst();
    EnclosingDeclarations::iterator its = enclosingDeclarations.begin();
    for (; its != enclosingDeclarations.end(); ++its)
    {
        if (!(*its).tmpl || rv)
            break;
        rv = templatesearch(name, (*its).tmpl);
        if (rv && needTypeOrNamespace && !istype(rv))
        {
            symrv = rv;
            rv = nullptr;
        }
    }
    if (cls && !rv)
    {
        /* optimize for the case where the final class has what we need */
        while (cls && !rv)
        {
            if (!tagsOnly)
                rv = search(cls->tp->BaseType()->syms, name);
            if (rv && needTypeOrNamespace && !istype(rv))
            {
                symrv = rv;
                rv = nullptr;
            }
            if (!rv)
                rv = search(cls->tp->BaseType()->tags, name);
            if (rv && needTypeOrNamespace && !istype(rv))
            {
                symrv = rv;
                rv = nullptr;
            }
            if (!rv && cls->sb->baseClasses)
            {
                rv = classdata(name, cls, nullptr, false, tagsOnly);
                if (rv == (SYMBOL*)-1)
                {
                    rv = nullptr;
                    if (toErr)
                        errorstr(ERR_AMBIGUOUS_MEMBER_DEFINITION, name);
                    break;
                }
            }
            cls = cls->sb->parentClass;
        }
    }
    for (; its != enclosingDeclarations.end(); ++its)
    {
        if (rv)
            break;
        if ((*its).tmpl)
            rv = templatesearch(name, (*its).tmpl);
        if (rv && needTypeOrNamespace && !istype(rv))
        {
            symrv = rv;
            rv = nullptr;
        }
    }
    cls = enclosingDeclarations.GetFirst();
    if (cls && !rv)
    {
        /* optimize for the case where the final class has what we need */
        while (cls && !rv)
        {
            if (!rv && cls->templateParams)
                rv = templatesearch(name, cls->templateParams);
            if (rv && needTypeOrNamespace && !istype(rv))
            {
                symrv = rv;
                rv = nullptr;
            }
            cls = cls->sb->parentClass;
        }
    }
    if (!rv)
        rv = symrv;
    return rv;
}
SYMBOL* finishSearch(const char* name, SYMBOL* encloser, std::list<NAMESPACEVALUEDATA*>* ns, bool tagsOnly, bool throughClass,
                     bool namespaceOnly)
{
    SYMBOL* rv = nullptr;
    if (!encloser && !ns && !namespaceOnly)
    {
        SYMBOL* ssp = enclosingDeclarations.GetFirst();
        if (funcNestingLevel || !ssp)
        {
            if (!tagsOnly)
                rv = search(localNameSpace->front()->syms, name);
            if (!rv)
                rv = search(localNameSpace->front()->tags, name);
            if (lambdas.size())
            {
                for (auto srch : lambdas)
                {
                    if (rv)
                        break;
                    if (Optimizer::cparams.prm_cplusplus || !tagsOnly)
                        rv = search(srch->oldSyms, name);
                    if (!rv)
                        rv = search(srch->oldTags, name);
                }
            }
            if (!rv)
                rv = namespacesearch(name, localNameSpace, false, tagsOnly);
        }
        if (!rv && defaultParsingContext && !defaultParsingContext->sb->parentClass)
        {
            rv = namespacesearch(name, globalNameSpace, false, tagsOnly);
        }
        if (!rv && enumSyms)
            rv = search(enumSyms->tp->syms, name);
        if (!rv)
        {
            if (lambdas.size())
            {
                if (lambdas.front()->lthis)
                {
                    rv = search(lambdas.front()->lthis->tp->syms, name);
                    if (!rv && lambdas.front()->lthis->sb->baseClasses)
                    {
                        rv = classdata(name, lambdas.front()->lthis, nullptr, false, tagsOnly);
                        if (rv == (SYMBOL*)-1)
                        {
                            rv = nullptr;
                        }
                    }
                    if (rv)
                        rv->sb->throughClass = true;
                }
            }
            if (!rv)
            {
                rv = classsearch(name, tagsOnly, false, true);
                if (rv && rv->sb)
                    rv->sb->throughClass = true;
            }
        }
        else
        {
            rv->sb->throughClass = false;
        }
        if (!rv && (!ssp || !ssp->sb->nameSpaceValues || ssp->sb->nameSpaceValues->front() != globalNameSpace->front()))
        {
            rv = namespacesearch(name, localNameSpace, false, tagsOnly);
            if (!rv)
                rv = namespacesearch(name, globalNameSpace, false, tagsOnly);
            if (rv)
                rv->sb->throughClass = false;
        }
    }
    else
    {
        if (namespaceOnly && !ns)
        {
            rv = namespacesearch(name, globalNameSpace, false, tagsOnly);
            if (rv)
                rv->sb->throughClass = false;
        }
        else if (encloser)
        {
            DeclarationScope scope(encloser);
            rv = classsearch(name, tagsOnly, false, true);
            if (rv && rv->sb)
                rv->sb->throughClass = throughClass;
        }
        else
        {
            unvisitUsingDirectives(ns->front());
            rv = namespacesearch(name, ns, false, tagsOnly);
            if (rv)
            {
                rv->sb->throughClass = false;
            }
        }
    }
    return rv;
}
void nestedSearch( SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv, bool* destructor,
                      bool* isTemplate, bool tagsOnly, StorageClass storage_class, bool errIfNotFound, bool isType)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    LexemeStreamPosition placeHolder(currentStream);
    bool hasTemplate = false;
    bool namespaceOnly = false;
    bool typeName = false;
    *sym = nullptr;

    if (!Optimizer::cparams.prm_cplusplus &&
        ((Optimizer::architecture != ARCHITECTURE_MSIL) || !Optimizer::cparams.msilAllowExtensions))
    {
        if (ISID())
        {
            if (tagsOnly)
                *sym = tsearch(currentLex->value.s.a);
            else
                *sym = gsearch(currentLex->value.s.a);
        }
        return;
    }
    if (MATCHKW(Keyword::typename_))
    {
        typeName = true;
    }
    nestedPath(&encloser, &ns, &throughClass, tagsOnly, storage_class, isType, 0);
    if (Optimizer::cparams.prm_cplusplus)
    {

        if (MATCHKW(Keyword::complx_))
        {
            if (destructor)
            {
                *destructor = true;
            }
            else
            {
                error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
            }
            getsym();
        }
        else if (MATCHKW(Keyword::template_))
        {
            getsym();
            if (isTemplate)
                *isTemplate = true;
            hasTemplate = true;
        }
    }
    if (ISID() || MATCHKW(Keyword::operator_))
    {
        if (encloser && encloser->tp->type == BasicType::templateselector_)
        {
            if (destructor && *destructor && encloser->sb->templateSelector->size() == 2)
            {
                encloser->sb->templateSelector->push_back(TEMPLATESELECTOR{});
                encloser->sb->templateSelector->back().name = (*encloser->sb->templateSelector)[1].name;
            }
            *sym = makeID(StorageClass::type_, encloser->tp, nullptr, encloser->sb->templateSelector->back().name);
        }
        else
        {
            char buf[512];
            if (!ISID())
            {
                int ovdummy;
                getIdName(nullptr, buf, sizeof(buf), & ovdummy, nullptr);
                *sym = finishSearch(buf, encloser, ns, tagsOnly, throughClass, namespaceOnly);
                if (!*sym)
                    encloser = nullptr;
                if (errIfNotFound && !*sym)
                {
                    errorstr(ERR_UNDEFINED_IDENTIFIER, buf);
                }
            }
            else
            {
                TEMPLATEPARAMPAIR* tparam = TemplateLookupSpecializationParam(currentLex->value.s.a);
                if (tparam)
                {
                    *sym = tparam->first;
                }
                else
                {
                    if (encloser)
                    {
                        Utils::StrCpy(buf, currentLex->value.s.a);
                        if (!typeName)
                            GetUsingName(buf);
                        *sym = finishSearch(buf, encloser, ns, tagsOnly, throughClass, namespaceOnly);
                    }
                    else
                    {
                        *sym = finishSearch(currentLex->value.s.a, encloser, ns, tagsOnly, throughClass, namespaceOnly);
                    }
                    if (!*sym)
                        encloser = nullptr;
                    if (errIfNotFound && !*sym)
                    {
                        errorstr(ERR_UNDEFINED_IDENTIFIER, currentLex->value.s.a);
                    }
                }
            }
        }
    }
    else if (destructor && *destructor)
    {
        *destructor = false;
        error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
    }
    if (*sym && hasTemplate)
    {
        if (!(*sym)->sb->templateLevel &&
            ((*sym)->tp->type != BasicType::templateparam_ || (*sym)->tp->templateParam->second->type != TplType::template_) &&
            (*sym)->tp->type != BasicType::templateselector_ && (*sym)->tp->type != BasicType::templatedecltype_)
        {
            if ((*sym)->sb->storage_class == StorageClass::overloads_)
            {
                bool found = false;
                for (auto sym1 : *(*sym)->tp->BaseType()->syms)
                {
                    if (sym1->sb->templateLevel)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    errorsym(ERR_NOT_A_TEMPLATE, *sym);
            }
            else
            {
                errorsym(ERR_NOT_A_TEMPLATE, *sym);
            }
        }
    }
    if (encloser && strSym)
        *strSym = encloser;
    if (nsv)
        if (ns && ns->front()->name)
            *nsv = ns;
        else
            *nsv = nullptr;
    else if (!*sym)
        placeHolder.Backup();
    return;
}
void getIdName( SYMBOL* funcsp, char* buf, int len, int* ov, Type** castType)
{
    buf[0] = 0;
    if (ISID())
    {
        Utils::StrCpy(buf, len, currentLex->value.s.a);
    }
    else if (MATCHKW(Keyword::operator_))
    {
        getsym();
        if (ISKW() && currentLex->kw->key >= Keyword::new_ && currentLex->kw->key <= Keyword::complx_)
        {
            Keyword kw = currentLex->kw->key;
            switch (kw)
            {
                case Keyword::openpa_:
                    getsym();
                    if (!MATCHKW(Keyword::closepa_))
                    {
                        needkw(Keyword::closepa_);
                        --*currentStream;
                    }
                    break;
                case Keyword::openbr_:
                    getsym();
                    if (!MATCHKW(Keyword::closebr_))
                    {
                        needkw(Keyword::closebr_);
                        --*currentStream;
                    }
                    break;
                case Keyword::new_:
                case Keyword::delete_:
                    getsym();
                    if (!MATCHKW(Keyword::openbr_))
                    {
                        --*currentStream;
                    }
                    else
                    {
                        kw = (Keyword)((int)kw - (int)Keyword::new_ + (int)Keyword::complx_ + 1);
                        getsym();
                        if (!MATCHKW(Keyword::closebr_))
                        {
                            needkw(Keyword::closebr_);
                            --*currentStream;
                        }
                    }
                    break;
                default:
                    break;
            }
            Utils::StrCpy(buf, len, overloadNameTab[*ov = (int)kw - (int)Keyword::new_ + CI_NEW]);
        }
        else if (ISID() || TypeGenerator::StartOfType(nullptr, false))  // potential cast operator
        {
            Type* tp = nullptr;
            tp = TypeGenerator::TypeId(funcsp, StorageClass::cast_, true, true, false);
            if (!tp)
            {
                errorstr(ERR_INVALID_AS_OPERATOR, "");
                tp = &stdint;
            }
            if (castType)
            {
                *castType = tp;
                if (tp->IsAutoType() & !lambdas.size())  // make an exception so we can compile templates for lambdas
                    error(ERR_AUTO_NOT_ALLOWED_IN_CONVERSION_FUNCTION);
            }
            Utils::StrCpy(buf, len, overloadNameTab[*ov = CI_CAST]);
        }
        else if (MATCHTYPE(LexType::l_astr_) || MATCHTYPE(LexType::l_u8str_))
        {
            LexemeStreamPosition placeHolder(currentStream);;
            Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)currentLex->value.s.w;
            if (xx->count)
                error(ERR_OPERATOR_LITERAL_EMPTY_STRING);
            if (currentLex->suffix)
            {
                Optimizer::my_sprintf(buf, len, "%s@%s", overloadNameTab[CI_LIT], currentLex->suffix);
                *ov = CI_LIT;
            }
            else
            {
                getsym();

                if (ISID())
                {
                    Optimizer::my_sprintf(buf, len, "%s@%s", overloadNameTab[CI_LIT], currentLex->value.s.a);
                    *ov = CI_LIT;
                }
                else
                {
                    error(ERR_OPERATOR_LITERAL_NEEDS_ID);
                    placeHolder.Backup();
                }
            }
        }
        else
        {
            if (ISKW())
                errorstr(ERR_INVALID_AS_OPERATOR, currentLex->kw->name);
            else
                errorstr(ERR_INVALID_AS_OPERATOR, "");
            --*currentStream;
        }
    }
    return;
}
void id_expression( SYMBOL* funcsp, SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv,
                       bool* isTemplate, bool tagsOnly, bool membersOnly, char* idname, int len, int flags)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    Type* castType = nullptr;
    LexemeStreamPosition placeHolder(currentStream);
    char buf[512];
    buf[0] = 0;
    int ov = 0;
    bool hasTemplate = false;
    bool namespaceOnly = false;

    *sym = nullptr;

    if (MATCHKW(Keyword::classsel_))
        namespaceOnly = true;
    if (!Optimizer::cparams.prm_cplusplus && (Optimizer::architecture != ARCHITECTURE_MSIL))
    {
        if (ISID())
        {
            if (idname)
                Utils::StrCpy(idname, len, currentLex->value.s.a);
            if (tagsOnly)
                *sym = tsearch(currentLex->value.s.a);
            else
            {
                SYMBOL* ssp = enclosingDeclarations.GetFirst();
                if (ssp)
                {
                    *sym = search(ssp->tp->syms, currentLex->value.s.a);
                }
                if (*sym == nullptr)
                    *sym = gsearch(currentLex->value.s.a);
            }
        }
        return;
    }
    nestedPath(&encloser, &ns, &throughClass, tagsOnly, StorageClass::global_, false, flags);
    if (MATCHKW(Keyword::complx_))
    {
        getsym();
        if (ISID())
        {
            if (encloser)
            {
                if (strcmp(encloser->name, currentLex->value.s.a))
                {
                    error(ERR_DESTRUCTOR_MUST_MATCH_CLASS);
                }
                *sym = finishSearch(overloadNameTab[CI_DESTRUCTOR], encloser, ns, tagsOnly, throughClass, namespaceOnly);
            }
        }
        else
        {
            error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
        }
    }
    else
    {
        if (MATCHKW(Keyword::template_))
        {
            if (isTemplate)
                *isTemplate = true;
            hasTemplate = true;
            getsym();
        }
        getIdName(funcsp, buf, sizeof(buf), &ov, &castType);
        if (buf[0] &&(!IsDefiningTemplate() || !encloser || encloser->tp->BaseType()->type != BasicType::templateselector_))
        {
            if (encloser)
                encloser->tp->InstantiateDeferred();
            if (!encloser && membersOnly)
                encloser = enclosingDeclarations.GetFirst();
            *sym =
                finishSearch(ov == CI_CAST ? overloadNameTab[CI_CAST] : buf, encloser, ns, tagsOnly, throughClass, namespaceOnly);
            if (*sym && hasTemplate)
            {
                if ((*sym)->sb->storage_class == StorageClass::overloads_)
                {
                    bool found = false;
                    for (auto sym1 : *(*sym)->tp->BaseType()->syms)
                    {
                        if (sym1->sb->templateLevel)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        errorsym(ERR_NOT_A_TEMPLATE, *sym);
                }
                else if ((*sym)->sb->storage_class != StorageClass::typedef_ || !(*sym)->sb->templateLevel)
                {
                    errorsym(ERR_NOT_A_TEMPLATE, *sym);
                }
            }
        }
    }
    if (encloser && strSym)
        *strSym = encloser;
    if (nsv)
        if (ns && ns->front()->name)
            *nsv = ns;
        else
            *nsv = nullptr;
    else if (!*sym && (!encloser || encloser->tp->type != BasicType::templateselector_))
        placeHolder.Backup();
    if (!*sym && idname)
    {
        Utils::StrCpy(idname, len, buf);
    }
    return;
}
SYMBOL* LookupSym(char* name)
{
    SYMBOL* rv = nullptr;
    if (!Optimizer::cparams.prm_cplusplus)
    {
        return gsearch(name);
    }
    rv = search(localNameSpace->front()->syms, name);
    if (!rv)
        rv = search(localNameSpace->front()->tags, name);
    if (!rv)
        rv = namespacesearch(name, localNameSpace, false, false);
    if (!rv)
        rv = namespacesearch(name, globalNameSpace, false, false);
    return rv;
}

static bool IsFriend(SYMBOL* cls, SYMBOL* frnd)
{
    if (cls != frnd)
    {
        if (cls && frnd && cls->sb->friends)
        {
            for (auto sym : *cls->sb->friends)
            {
                if (sym == frnd || sym->sb->maintemplate == frnd || sym == frnd->sb->mainsym || sym == frnd->sb->parentTemplate)
                    return true;
                if (sym->tp->IsFunction() && sym->sb->parentClass == frnd->sb->parentClass && !strcmp(sym->name, frnd->name) &&
                    sym->sb->overloadName && searchOverloads(frnd, sym->sb->overloadName->tp->syms))
                    return true;
                if (sym->sb->templateLevel && sym->sb->instantiations)
                {
                    for (auto instants : *sym->sb->instantiations)
                    {
                        if (instants == frnd || instants == frnd->sb->parentTemplate)
                            return true;
                    }
                }
            }
        }
    }
    return false;
}
// works by searching the tree for the base or member symbol, and stopping any
// time the access wouldn't work.  If the symbol is found it is accessible.
static bool isAccessibleInternal(SYMBOL* derived, SYMBOL* currentBase, SYMBOL* member, SYMBOL* funcsp, AccessLevel minAccess,
                                 AccessLevel maxAccess, int level)
{
    AccessLevel memberAccess = member->sb->access > maxAccess ? maxAccess : member->sb->access;
    BASECLASS* lst;
    SYMBOL* ssp;
    if (!Optimizer::cparams.prm_cplusplus)
        return true;
    ssp = enclosingDeclarations.GetFirst();
    if (ssp)
    {
        if (ssp == member)
            return true;
    }
    if (IsFriend(derived, funcsp) || (funcsp && IsFriend(derived, funcsp->sb->parentClass)) || IsFriend(derived, ssp) ||
        IsFriend(member->sb->parentClass, funcsp) || IsFriend(member->sb->parentClass, derived))
        return true;
    if (currentBase)
    {
        if (argFriend && IsFriend(currentBase, argFriend))
            return true;
        if (!currentBase->tp->BaseType()->syms)
            return false;
        if (member->sb->parentClass == currentBase || member->sb->parentClass == currentBase->sb->mainsym)
            return memberAccess >= minAccess || level == 0;
        else if (member->sb->mainsym)
            if (member->sb->mainsym->sb->parentClass == currentBase ||
                member->sb->mainsym->sb->parentClass == currentBase->sb->mainsym)
                return memberAccess >= minAccess || level == 0;
        if (currentBase->sb->baseClasses)
        {
            for (auto lst : *currentBase->sb->baseClasses)
            {
                SYMBOL* sym = lst->cls;
                sym = sym->tp->BaseType()->sp;

                // we have to go through the base classes even if we know that a normal
                // lookup wouldn't work, so we can check their friends lists...
                if (isAccessibleInternal(derived, sym, member, funcsp, minAccess,
                                         lst->accessLevel < maxAccess ? AccessLevel::protected_ : maxAccess,
                                         lst->accessLevel == AccessLevel::private_ ? 2 : 1))
                    return true;
            }
        }
    }
    return (level == 0 && memberAccess >= minAccess);
}
bool isAccessible(SYMBOL* derived, SYMBOL* currentBase, SYMBOL* member, SYMBOL* funcsp, AccessLevel minAccess, bool asAddress)
{
    return IsDefiningTemplate() || instantiatingFunction || member->sb->accessibleTemplateArgument ||
           isAccessibleInternal(derived, currentBase, member, funcsp, minAccess, AccessLevel::public_, 0);
}
static SYMBOL* AccessibleClassInstance(SYMBOL* parent)
{
    // search through all active structure declarations
    // to try to find a structure which is derived from parent...
    for (auto&& s : enclosingDeclarations)
    {
        if (s.str)
        {
            SYMBOL* srch = s.str;
            while (srch)
            {
                if (srch == parent || classRefCount(parent, srch))
                    break;
                srch = srch->sb->parentClass;
            }
            if (srch)
                return srch;
        }
    }
    return nullptr;
}
bool isExpressionAccessible(SYMBOL* derived, SYMBOL* sym, SYMBOL* funcsp, EXPRESSION* exp, bool asAddress)
{
    if (sym->sb->parentClass)
    {
        SYMBOL* parent = sym->sb->parentClass;
        AccessLevel minAccess = AccessLevel::public_;
        if (exp && exp->type == ExpressionNode::auto_)
        {
            parent = exp->v.sp->tp->BaseType()->sp;
        }
        else if (!derived && funcsp && !sym->sb->throughClass)
        {
            if (funcsp->sb->parentClass)
                parent = funcsp->sb->parentClass;
            else
            {
                // have to check for friends, otherwise it has to be public...
                return isAccessible(derived, parent, sym, funcsp, AccessLevel::public_, asAddress);
            }
        }
        SYMBOL* ssp = nullptr;
        if (sym->sb->throughClass && (ssp = AccessibleClassInstance(sym->sb->parentClass)) != nullptr)
        {
            if (ssp == parent)
                minAccess = AccessLevel::private_;
            else
                minAccess = AccessLevel::protected_;
            derived = parent = ssp;
        }
        do
        {
            if (isAccessible(derived, parent, sym, funcsp, minAccess, asAddress))
                return true;
            if (derived)
                derived = derived->sb->parentClass;
        } while (derived && !ssp);
        return false;
    }
    return true;
}
bool checkDeclarationAccessible(SYMBOL* sp, SYMBOL* derived, SYMBOL* funcsp)
{
    Type* tp = sp->tp;
    while (tp)
    {
        if (tp->IsStructured() || tp->type == BasicType::typedef_ || tp->type == BasicType::enum_)
        {
            SYMBOL* sym;
            if (tp->type == BasicType::typedef_)
                sym = tp->sp;
            else
                sym = tp->BaseType()->sp;
            if (sym->sb->parentClass)
            {
                SYMBOL* ssp = nullptr;
                if ((ssp = AccessibleClassInstance(sym->sb->parentClass)) != nullptr)
                {
                    if (!isAccessible(ssp, ssp, sym, funcsp, AccessLevel::protected_, false))
                    {
                        currentErrorLine = 0;
                        errorsym(ERR_CANNOT_ACCESS, sym);
                        return false;
                    }
                }
                else
                {
                    if (derived)
                    {
                        while (derived)
                        {
                            if (isAccessible(derived, sym->sb->parentClass, sym, funcsp, AccessLevel::public_, false))
                                return true;
                            derived = derived->sb->parentClass;
                        }
                        errorsym(ERR_CANNOT_ACCESS, sym);
                        return false;
                    }
                    else
                    {
                        if (!isAccessible(derived, sym->sb->parentClass, sym, funcsp, AccessLevel::public_, false))
                        {
                            errorsym(ERR_CANNOT_ACCESS, sym);
                            return false;
                        }
                    }
                }
            }
            break;
        }
        else if (tp->IsFunction())
        {
            for (auto sym : *tp->BaseType()->syms)
            {
                if (!checkDeclarationAccessible(sym, funcsp ? funcsp->sb->parentClass : nullptr, funcsp))
                    return false;
            }
        }
        tp = tp->btp;
    }
    return true;
}
}  // namespace Parser