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
namespace Parser
{

static void GetUsingName(char* buf)
{
    auto sym = enclosingDeclarations.GetFirst();
    if (sym)
    {
        enclosingDeclarations.Add(sym);
        auto sp = classsearch(buf, false, false, false);
        enclosingDeclarations.Drop();
        if (sp && sp->sb && sp->sb->usingTypedef)
        {
            if (sp->tp->IsStructured())
                strcpy(buf, sp->tp->BaseType()->sp->name);
        }
    }
}
LexList* nestedPath(LexList* lex, SYMBOL** sym, std::list<NAMESPACEVALUEDATA*>** ns, bool* throughClass, bool tagsOnly,
                    StorageClass storage_class, bool isType, int flags)
{
    (void)tagsOnly;
    (void)storage_class;
    bool first = true;
    std::list<NAMESPACEVALUEDATA*>* nssym = globalNameSpace;
    SYMBOL* strSym = nullptr;
    bool qualified = false;
    std::vector<TEMPLATESELECTOR>* templateSelector = nullptr;
    LexList *placeholder = lex, *finalPos;
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

    if (MATCHKW(lex, Keyword::typename_))
    {
        typeName = true;
        lex = getsym();
    }
    if (MATCHKW(lex, Keyword::classsel_))
    {
        nssym = rootNameSpace;
        lex = getsym();
        qualified = true;
    }
    finalPos = lex;
    while (ISID(lex) || (first && MATCHKW(lex, Keyword::decltype_)) || (templateSelector && MATCHKW(lex, Keyword::operator_)))
    {
        char buf[512];
        SYMBOL* sp = nullptr;
        int ovdummy;
        if (first && MATCHKW(lex, Keyword::decltype_))
        {
            Type* tp = nullptr;
            lex = getDeclType(lex, theCurrentFunc, &tp);
            if (!tp || (!tp->IsStructured() && tp->type != BasicType::templatedecltype_) || !MATCHKW(lex, Keyword::classsel_))
                break;
            if (tp->IsAutoType())
                RequiresDialect::Feature(Dialect::cpp14, "decltype(auto)");
            lex = getsym();
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
            finalPos = lex;
        }
        else if (templateSelector)
        {
            lex = getIdName(lex, nullptr, buf, &ovdummy, nullptr);
            lex = getsym();

            GetUsingName(buf);

            if (!templateSelector)
                templateSelector = templateSelectorListFactory.CreateVector();
            templateSelector->push_back(TEMPLATESELECTOR{});
            templateSelector->back().name = litlate(buf);
            if (hasTemplate)
            {
                templateSelector->back().isTemplate = true;
                if (MATCHKW(lex, Keyword::lt_))
                {
                    lex = GetTemplateArguments(lex, nullptr, nullptr, &templateSelector->back().templateParams);
                }
                else if (MATCHKW(lex, Keyword::classsel_))
                {
                    SpecializationError(buf);
                }
            }
            if ((!inTemplateType || parsingUsing) && MATCHKW(lex, Keyword::openpa_))
            {
                CallSite funcparams = {};
                lex = getArgs(lex, theCurrentFunc, &funcparams, Keyword::closepa_, true, 0);
                templateSelector->back().arguments = funcparams.arguments;
                templateSelector->back().asCall = true;
            }
            if (!MATCHKW(lex, Keyword::classsel_))
                break;
            lex = getsym();
            finalPos = lex;
        }
        else
        {
            SYMBOL* sp_orig;
            lex = getIdName(lex, nullptr, buf, &ovdummy, nullptr);
            lex = getsym();
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
                    if (parsingDefaultTemplateArgs)
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
                                enclosingDeclarations.Add(t->lthis);
                                sp = classsearch(buf, false, MATCHKW(lex, Keyword::classsel_), false);
                                enclosingDeclarations.Drop();
                            }
                            if (sp)
                                break;
                        }
                        if (!sp)
                            sp = classsearch(buf, false, MATCHKW(lex, Keyword::classsel_), false);
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
                                    if (!definingTemplate || instantiatingTemplate)
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
                                    if (MATCHKW(lex, Keyword::lt_))
                                    {
                                        lex = GetTemplateArguments(lex, nullptr, sp, &current);
                                    }
                                    if (!MATCHKW(lex, Keyword::classsel_))
                                        break;
                                    lex = getsym();
                                    finalPos = lex;
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
                    if (sp && MATCHKW(lex, Keyword::classsel_) && !istype(sp))
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
                GetUsingName(buf);

                if (structLevel && !definingTemplate && strSym->sb->templateLevel &&
                    (!strSym->sb->instantiated || strSym->sb->attribs.inheritable.linkage4 != Linkage::virtual_))
                {
                    sp = nullptr;
                }
                else
                {
                    enclosingDeclarations.Add(strSym);
                    sp = classsearch(buf, false, MATCHKW(lex, Keyword::classsel_), false);
                    enclosingDeclarations.Drop();
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
                        if (MATCHKW(lex, Keyword::lt_))
                        {
                            lex = GetTemplateArguments(lex, nullptr, nullptr, &templateSelector->back().templateParams);
                        }
                        else if (MATCHKW(lex, Keyword::classsel_))
                        {
                            errorstr(ERR_NEED_TEMPLATE_ARGUMENTS, buf);
                        }
                    }
                    if (!MATCHKW(lex, Keyword::classsel_))
                        break;
                    lex = getsym();
                    finalPos = lex;
                }
                sp_orig = sp;
                if (sp && sp->sb && sp->sb->typeAlias && !sp->sb->templateLevel && sp->tp->IsStructured())
                {
                    istypedef = true;
                    sp = sp->tp->BaseType()->sp;
                }
                else if (sp && sp->sb && sp->tp->type == BasicType::typedef_)
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
                    if (!MATCHKW(lex, Keyword::classsel_))
                        break;
                    lex = getsym();
                    finalPos = lex;
                    strSym = sp;
                    qualified = true;
                    break;
                }
                else if (sp)
                {
                    if (sp->sb && sp->sb->templateLevel && (!sp->sb->instantiated || MATCHKW(lex, Keyword::lt_)))
                    {
                        hasTemplateArgs = true;
                        if (MATCHKW(lex, Keyword::lt_))
                        {
                            current = nullptr;
                            lex = GetTemplateArguments(lex, nullptr, sp_orig, &current);
                        }
                        else if (MATCHKW(lex, Keyword::classsel_))
                        {
                            currentsp = sp;
                            if (!istypedef)
                                SpecializationError(sp);
                        }
                        if (!MATCHKW(lex, Keyword::classsel_))
                            break;
                    }
                    else
                    {
                        if (!MATCHKW(lex, Keyword::classsel_))
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
                    if (MATCHKW(lex, Keyword::lt_))
                    {
                        lex = GetTemplateArguments(lex, nullptr, sp, &current);
                    }
                    else if (MATCHKW(lex, Keyword::classsel_))
                    {
                        currentsp = sp;
                        SpecializationError(sp);
                    }
                    if (!MATCHKW(lex, Keyword::classsel_))
                        break;
                }
                else if (!MATCHKW(lex, Keyword::classsel_))
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
                        inTemplateHeader || parsingSpecializationDeclaration || parsingTrailingReturnOrUsing || (flags & _F_NOEVAL);
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
                                        if (definingTemplate)  // || noSpecializationError)
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
                    sp->tp->InstantiateDeferred();
                }
                if (sp && (!sp->sb ||
                           (sp->sb->storage_class != StorageClass::namespace_ && (!sp->tp->IsStructured() || sp->templateParams))))
                    pastClassSel = true;
                lex = getsym();
                finalPos = lex;
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
                    if (!definingTemplate || !sp)
                    {
                        if (dependentType)
                            if (dependentType->IsStructured())
                                errorstringtype(ERR_DEPENDENT_TYPE_DOES_NOT_EXIST_IN_TYPE, buf, dependentType->BaseType());
                            else
                                errortype(ERR_DEPENDENT_TYPE_NOT_A_CLASS_OR_STRUCT, dependentType, nullptr);
                        else
                            errorstr(ERR_QUALIFIER_NOT_A_CLASS_OR_NAMESPACE, buf);
                    }
                    lex = prevsym(placeholder);
                    strSym = sp;
                    qualified = true;
                    break;
                }
            }
        }
        first = false;
        hasTemplate = false;
        if (MATCHKW(lex, Keyword::template_))
        {
            hasTemplate = true;
            lex = getsym();
        }
        qualified = true;
    }
    if (pastClassSel && !typeName && !inTypedef && !hasTemplate && isType && !noTypeNameError)
    {

        if (!strSym || !allTemplateArgsSpecified(strSym, strSym->templateParams))
        {
            char buf[2000];
            buf[0] = 0;

            while (placeholder != finalPos->next)
            {
                if (ISKW(placeholder))
                    Optimizer::my_sprintf(buf + strlen(buf), "%s", placeholder->data->kw->name);
                else if (ISID(placeholder))
                    Optimizer::my_sprintf(buf + strlen(buf), "%s", placeholder->data->value.s.a);
                placeholder = placeholder->next;
            }

            errorstr(ERR_DEPENDENT_TYPE_NEEDS_TYPENAME, buf);
        }
    }
    if (!pastClassSel && typeName && !dependentType && !inTypedef && (!definingTemplate || instantiatingTemplate))
    {
        error(ERR_NO_TYPENAME_HERE);
    }
    lex = prevsym(finalPos);
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
    return lex;
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
LexList* tagsearch(LexList* lex, char* name, SYMBOL** rsp, SymbolTable<SYMBOL>** table, SYMBOL** strSym_out,
                   std::list<NAMESPACEVALUEDATA*>** nsv_out, StorageClass storage_class)
{
    std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
    SYMBOL* strSym = nullptr;

    *rsp = nullptr;
    if (ISID(lex) || MATCHKW(lex, Keyword::classsel_))
    {
        lex = nestedSearch(lex, rsp, &strSym, &nsv, nullptr, nullptr, true, storage_class, false, false);
        if (*rsp)
        {
            strcpy(name, (*rsp)->name);
            lex = getsym();
            if (MATCHKW(lex, Keyword::begin_))
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
        else if (ISID(lex))
        {
            strcpy(name, lex->data->value.s.a);
            lex = getsym();
            if (MATCHKW(lex, Keyword::begin_))
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
    return lex;
}
SYMBOL* classsearch(const char* name, bool tagsOnly, bool needTypeOrNamespace, bool toErr)
{
    SYMBOL* rv = nullptr;
    SYMBOL* symrv = nullptr;
    SYMBOL* cls = enclosingDeclarations.GetFirst();

    EnclosingDeclarations::iterator its;
    for (its = enclosingDeclarations.begin(); its != enclosingDeclarations.end(); ++its)
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
        if (funcLevel || !ssp)
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
        if (!rv && parsingDefaultTemplateArgs)
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
            enclosingDeclarations.Add(encloser);
            rv = classsearch(name, tagsOnly, false, true);
            enclosingDeclarations.Drop();
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
LexList* nestedSearch(LexList* lex, SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv, bool* destructor,
                      bool* isTemplate, bool tagsOnly, StorageClass storage_class, bool errIfNotFound, bool isType)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    LexList* placeholder = lex;
    bool hasTemplate = false;
    bool namespaceOnly = false;
    *sym = nullptr;

    if (!Optimizer::cparams.prm_cplusplus &&
        ((Optimizer::architecture != ARCHITECTURE_MSIL) || !Optimizer::cparams.msilAllowExtensions))
    {
        if (ISID(lex))
        {
            if (tagsOnly)
                *sym = tsearch(lex->data->value.s.a);
            else
                *sym = gsearch(lex->data->value.s.a);
        }
        return lex;
    }
    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly, storage_class, isType, 0);
    if (Optimizer::cparams.prm_cplusplus)
    {

        if (MATCHKW(lex, Keyword::complx_))
        {
            if (destructor)
            {
                *destructor = true;
            }
            else
            {
                error(ERR_CANNOT_USE_DESTRUCTOR_HERE);
            }
            lex = getsym();
        }
        else if (MATCHKW(lex, Keyword::template_))
        {
            lex = getsym();
            if (isTemplate)
                *isTemplate = true;
            hasTemplate = true;
        }
    }
    if (ISID(lex) || MATCHKW(lex, Keyword::operator_))
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
            if (!ISID(lex))
            {
                int ovdummy;
                lex = getIdName(lex, nullptr, buf, &ovdummy, nullptr);
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
                TEMPLATEPARAMPAIR* tparam = TemplateLookupSpecializationParam(lex->data->value.s.a);
                if (tparam)
                {
                    *sym = tparam->first;
                }
                else
                {
                    if (encloser)
                    {
                        strcpy(buf, lex->data->value.s.a);
                        GetUsingName(buf);
                        *sym = finishSearch(buf, encloser, ns, tagsOnly, throughClass, namespaceOnly);
                    }
                    else
                    {
                        *sym = finishSearch(lex->data->value.s.a, encloser, ns, tagsOnly, throughClass, namespaceOnly);
                    }
                    if (!*sym)
                        encloser = nullptr;
                    if (errIfNotFound && !*sym)
                    {
                        errorstr(ERR_UNDEFINED_IDENTIFIER, lex->data->value.s.a);
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
        lex = prevsym(placeholder);
    return lex;
}
LexList* getIdName(LexList* lex, SYMBOL* funcsp, char* buf, int* ov, Type** castType)
{
    buf[0] = 0;
    if (ISID(lex))
    {
        strcpy(buf, lex->data->value.s.a);
    }
    else if (MATCHKW(lex, Keyword::operator_))
    {
        lex = getsym();
        if (ISKW(lex) && lex->data->kw->key >= Keyword::new_ && lex->data->kw->key <= Keyword::complx_)
        {
            Keyword kw = lex->data->kw->key;
            switch (kw)
            {
                case Keyword::openpa_:
                    lex = getsym();
                    if (!MATCHKW(lex, Keyword::closepa_))
                    {
                        needkw(&lex, Keyword::closepa_);
                        lex = backupsym();
                    }
                    break;
                case Keyword::openbr_:
                    lex = getsym();
                    if (!MATCHKW(lex, Keyword::closebr_))
                    {
                        needkw(&lex, Keyword::closebr_);
                        lex = backupsym();
                    }
                    break;
                case Keyword::new_:
                case Keyword::delete_:
                    lex = getsym();
                    if (!MATCHKW(lex, Keyword::openbr_))
                    {
                        lex = backupsym();
                    }
                    else
                    {
                        kw = (Keyword)((int)kw - (int)Keyword::new_ + (int)Keyword::complx_ + 1);
                        lex = getsym();
                        if (!MATCHKW(lex, Keyword::closebr_))
                        {
                            needkw(&lex, Keyword::closebr_);
                            lex = backupsym();
                        }
                    }
                    break;
                default:
                    break;
            }
            strcpy(buf, overloadNameTab[*ov = (int)kw - (int)Keyword::new_ + CI_NEW]);
        }
        else if (ISID(lex) || TypeGenerator::StartOfType(lex, nullptr, false))  // potential cast operator
        {
            Type* tp = nullptr;
            tp = TypeGenerator::TypeId(lex, funcsp, StorageClass::cast_, true, true, false);
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
            strcpy(buf, overloadNameTab[*ov = CI_CAST]);
        }
        else if (lex->data->type == LexType::l_astr_ || lex->data->type == LexType::l_u8str_)
        {
            LexList* placeholder = lex;
            Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)lex->data->value.s.w;
            if (xx->count)
                error(ERR_OPERATOR_LITERAL_EMPTY_STRING);
            if (lex->data->suffix)
            {
                Optimizer::my_sprintf(buf, "%s@%s", overloadNameTab[CI_LIT], lex->data->suffix);
                *ov = CI_LIT;
            }
            else
            {
                lex = getsym();

                if (ISID(lex))
                {
                    Optimizer::my_sprintf(buf, "%s@%s", overloadNameTab[CI_LIT], lex->data->value.s.a);
                    *ov = CI_LIT;
                }
                else
                {
                    error(ERR_OPERATOR_LITERAL_NEEDS_ID);
                    prevsym(placeholder);
                }
            }
        }
        else
        {
            if (ISKW(lex))
                errorstr(ERR_INVALID_AS_OPERATOR, lex->data->kw->name);
            else
                errorstr(ERR_INVALID_AS_OPERATOR, "");
            lex = backupsym();
        }
    }
    return lex;
}
LexList* id_expression(LexList* lex, SYMBOL* funcsp, SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv,
                       bool* isTemplate, bool tagsOnly, bool membersOnly, char* idname, int flags)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    Type* castType = nullptr;
    LexList* placeholder = lex;
    char buf[512];
    int ov = 0;
    bool hasTemplate = false;
    bool namespaceOnly = false;

    *sym = nullptr;

    if (MATCHKW(lex, Keyword::classsel_))
        namespaceOnly = true;
    if (!Optimizer::cparams.prm_cplusplus && (Optimizer::architecture != ARCHITECTURE_MSIL))
    {
        if (ISID(lex))
        {
            if (idname)
                strcpy(idname, lex->data->value.s.a);
            if (tagsOnly)
                *sym = tsearch(lex->data->value.s.a);
            else
            {
                SYMBOL* ssp = enclosingDeclarations.GetFirst();
                if (ssp)
                {
                    *sym = search(ssp->tp->syms, lex->data->value.s.a);
                }
                if (*sym == nullptr)
                    *sym = gsearch(lex->data->value.s.a);
            }
        }
        return lex;
    }
    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly, StorageClass::global_, false, flags);
    if (MATCHKW(lex, Keyword::complx_))
    {
        lex = getsym();
        if (ISID(lex))
        {
            if (encloser)
            {
                if (strcmp(encloser->name, lex->data->value.s.a))
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
        if (MATCHKW(lex, Keyword::template_))
        {
            if (isTemplate)
                *isTemplate = true;
            hasTemplate = true;
            lex = getsym();
        }
        lex = getIdName(lex, funcsp, buf, &ov, &castType);
        if (buf[0])
        {
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
                else
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
        lex = prevsym(placeholder);
    if (!*sym && idname)
    {
        strcpy(idname, buf);
    }
    return lex;
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
    return (definingTemplate && !instantiatingTemplate) || instantiatingFunction || member->sb->accessibleTemplateArgument ||
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