/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <unordered_set>
#include "compiler.h"
#include <stack>
#include "ccerr.h"
#include "cpplookup.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "declare.h"
#include "mangle.h"
#include "lambda.h"
#include "template.h"
#include "declcpp.h"
#include "expr.h"
#include "help.h"
#include "unmangle.h"
#include "types.h"
#include "lex.h"
#include "OptUtils.h"
#include "memory.h"
#include "beinterf.h"
#include "exprcpp.h"
#include "inline.h"
#include "iexpr.h"
#include "libcxx.h"
#include "declcons.h"
#include "template.h"
#include "symtab.h"
#include "ListFactory.h"
#include "constopt.h"

namespace Parser
{
int inGetUserConversion;
int inSearchingFunctions;
int inNothrowHandler;
SYMBOL* argFriend;
static int insertFuncs(SYMBOL** spList, std::list<SYMBOL* >& gather, FUNCTIONCALL* args, TYPE* atp, int flags);

static const int rank[] = {0, 1, 1, 1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6, 7, 8, 8, 9};
static bool getFuncConversions(SYMBOL* sym, FUNCTIONCALL* f, TYPE* atp, SYMBOL* parent, enum e_cvsrn arr[], int* sizes, int count,
                               SYMBOL** userFunc, bool usesInitList);
static void WeedTemplates(SYMBOL** table, int count, FUNCTIONCALL* args, TYPE* atp);

SYMBOL* tablesearchone(const char* name, NAMESPACEVALUEDATA* ns, bool tagsOnly)
{
    SYMBOL* rv = nullptr;
    if (!tagsOnly)
        rv = search(ns->syms, name);
    if (!rv)
        rv = search(ns->tags, name);
    if (rv)
    {
        return rv;
    }
    return nullptr;
}
static void namespacesearchone(const char* name, NAMESPACEVALUEDATA* ns, std::list<SYMBOL*>& gather, bool tagsOnly,
                                           bool allowUsing);
std::list<SYMBOL*> tablesearchinline(const char* name, NAMESPACEVALUEDATA* ns, bool tagsOnly, bool allowUsing)
{
    // main namespace
    std::list<SYMBOL*> rv;
    SYMBOL* find = tablesearchone(name, ns, tagsOnly);
    if (find)
        rv.insert(rv.begin(), find);
    // included inlines
    if (ns->inlineDirectives)
    {
        for (auto x : *ns->inlineDirectives)
        {
            if (!x->sb->visited)
            {
                x->sb->visited = true;
                auto rv1 = tablesearchinline(name, x->sb->nameSpaceValues->front(), tagsOnly, allowUsing);
                if (rv1.size())
                {
                    rv.insert(rv.begin(), rv1.begin(), rv1.end());
                }
            }
        }
    }
    // any using definitions in this inline namespace
    if (allowUsing && ns->usingDirectives)
    {
        for (auto x : *ns->usingDirectives)
        {
            if (!x->sb->visited)
            {
                x->sb->visited = true;
                namespacesearchone(name, x->sb->nameSpaceValues->front(), rv, tagsOnly, allowUsing);
            }
        }
    }
    // enclosing ns if this one is inline
    if (ns->name && !ns->name->sb->visited &&
        ns->name->sb->attribs.inheritable.linkage == lk_inline)
    {
        ns->name->sb->visited = true;
        auto rv1 = tablesearchinline(name, ns->name->sb->nameSpaceValues->front(), tagsOnly, allowUsing);
        if (rv1.size())
        {
            rv.insert(rv.begin(), rv1.begin(), rv1.end());
        }
    }
    return rv;
}
static void namespacesearchone(const char* name, NAMESPACEVALUEDATA* ns, std::list<SYMBOL*>& gather, bool tagsOnly,
                                           bool allowUsing)
{
    auto rv = tablesearchinline(name, ns, tagsOnly, allowUsing);
    if (rv.size())
    {
        rv.insert(rv.end(), gather.begin(), gather.end());
    }
    else
    {
        rv = gather;
    }
    if (allowUsing && ns->usingDirectives)
    {
        for (auto x : *ns->usingDirectives)
        {
            if (!x->sb->visited)
            {
                x->sb->visited = true;
                namespacesearchone(name, x->sb->nameSpaceValues->front(), rv, tagsOnly, allowUsing);
            }
        }
    }
    gather = rv;
}
static std::list<SYMBOL*> namespacesearchInternal(const char* name, std::list<NAMESPACEVALUEDATA*>* ns, bool qualified, bool tagsOnly,
                                                bool allowUsing)
{
    std::list<SYMBOL*> lst;

    for (auto ns1 : *ns )
    {
        unvisitUsingDirectives(ns1);
        namespacesearchone(name, ns1, lst, tagsOnly, allowUsing);
        if (qualified || lst.size())
            break;
    }
    return lst;
}
SYMBOL* namespacesearch(const char* name, std::list<NAMESPACEVALUEDATA*>* ns, bool qualified, bool tagsOnly)
{
    auto lst = namespacesearchInternal(name, ns, qualified, tagsOnly, true);

    if (lst.size())
    {
        if (lst.size() > 1)
        {
            bool found = false;
            for (auto a : lst)
            {
                if (a->sb->storage_class != sc_overloads)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                TYPE* tp = MakeType(bt_aggregate);
                SYMBOL* sym = makeID(sc_overloads, tp, nullptr, lst.front()->name);
                tp->sp = sym;
                tp->syms = symbols.CreateSymbolTable();
                for (auto a : lst)
                {
                    for (auto b : *a->tp->syms)
                    { 
                        tp->syms->AddName(b);
                    }
                }
                return sym;
            }
        }
        for (auto it = lst.begin(); it != lst.end(); ++it)
        {
            // collision
            auto it1 = it;
            for (++it1; it1 != lst.end(); ++it1)
            {
                if ((*it) != (*it1) && (*it)->sb->mainsym != (*it1) && ((SYMBOL*)(*it1))->sb->mainsym != (*it))
                {
                    if ((*it)->sb->mainsym && (*it)->sb->mainsym != ((SYMBOL*)(*it1))->sb->mainsym)
                        errorsym2(ERR_AMBIGUITY_BETWEEN, (*it), (SYMBOL*)(*it1));
                }
            }
        }
        return (SYMBOL*)lst.front();
    }
    return nullptr;
}
LEXLIST* nestedPath(LEXLIST* lex, SYMBOL** sym, std::list<NAMESPACEVALUEDATA*>** ns, bool* throughClass, bool tagsOnly, enum e_sc storage_class,
                    bool isType)
{
    (void)tagsOnly;
    (void)storage_class;
    bool first = true;
    std::list<NAMESPACEVALUEDATA*>* nssym = globalNameSpace;
    SYMBOL* strSym = nullptr;
    bool qualified = false;
    std::vector<TEMPLATESELECTOR>* templateSelector = nullptr;
    LEXLIST *placeholder = lex, *finalPos;
    bool hasTemplate = false;
    TEMPLATEPARAMPAIR* templateParamAsTemplate = nullptr;
    TYPE* dependentType = nullptr;
    bool typeName = false;
    bool pastClassSel = false;
    std::list<TEMPLATEPARAMPAIR>* current = nullptr;

    if (sym)
        *sym = nullptr;
    if (ns)
        *ns = nullptr;

    if (MATCHKW(lex, kw_typename))
    {
        typeName = true;
        lex = getsym();
    }
    if (MATCHKW(lex, classsel))
    {
        nssym = rootNameSpace;
        lex = getsym();
        qualified = true;
    }
    finalPos = lex;
    while (ISID(lex) || (first && MATCHKW(lex, kw_decltype)) || (templateSelector && MATCHKW(lex, kw_operator)))
    {
        char buf[512];
        SYMBOL* sp = nullptr;
        int ovdummy;
        if (first && MATCHKW(lex, kw_decltype))
        {
            TYPE* tp = nullptr;
            lex = getDeclType(lex, theCurrentFunc, &tp);
            if (!tp || (!isstructured(tp) && tp->type != bt_templatedecltype) || !MATCHKW(lex, classsel))
                break;
            lex = getsym();
            if (tp->type == bt_templatedecltype)
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
                sp = basetype(tp)->sp;
                if (sp)
                    sp->tp = PerformDeferredInitialization(sp->tp, nullptr);
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
            if (!templateSelector)
                templateSelector = templateSelectorListFactory.CreateVector();
            templateSelector->push_back(TEMPLATESELECTOR{});
            templateSelector->back().name = litlate(buf);
            if (hasTemplate)
            {
                templateSelector->back().isTemplate = true;
                if (MATCHKW(lex, lt))
                {
                    lex = GetTemplateArguments(lex, nullptr, nullptr, &templateSelector->back().templateParams);
                }
                else if (MATCHKW(lex, classsel))
                {
                    SpecializationError(buf);
                }
            }
            if ((!inTemplateType || parsingUsing) && MATCHKW(lex, openpa))
            {
                FUNCTIONCALL funcparams = { };
                lex = getArgs(lex, theCurrentFunc, &funcparams, closepa, true, 0);
                templateSelector->back().arguments = funcparams.arguments;
                templateSelector->back().asCall = true;
            }
            if (!MATCHKW(lex, classsel))
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
                                STRUCTSYM s;
                                s.str = basetype(t->lthis->tp)->btp->sp;
                                addStructureDeclaration(&s);
                                sp = classsearch(buf, false, MATCHKW(lex, classsel), false);
                                dropStructureDeclaration();
                            }
                        }
                        if (!sp)
                            sp = classsearch(buf, false, MATCHKW(lex, classsel), false);
                        if (sp && sp->tp->type == bt_templateparam)
                        {
                            TEMPLATEPARAMPAIR* params = sp->tp->templateParam;
                            if (params->second->type == kw_typename)
                            {
                                if (params->second->packed && params->second->byPack.pack && params->second->byPack.pack->size())
                                {
                                    params = &params->second->byPack.pack->front();
                                }
                                if (params && params->second->byClass.val)
                                {
                                    sp = basetype(params->second->byClass.val)->sp;
                                    dependentType = params->second->byClass.val;
                                }
                            }
                            else if (params->second->type == kw_template)
                            {
                                if (params->second->byTemplate.val)
                                {
                                    templateParamAsTemplate = params;
                                    sp = params->second->byTemplate.val;
                                }
                                else
                                {
                                    if (MATCHKW(lex, lt))
                                    {
                                        lex = GetTemplateArguments(lex, nullptr, sp, &current);
                                    }
                                    if (!MATCHKW(lex, classsel))
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
                    SYMBOL *cached = nullptr;
                    if (!qualified)
                        sp = namespacesearch(buf, localNameSpace, qualified, tagsOnly);
                    if (sp && MATCHKW(lex, classsel) && !istype(sp))
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
                if (sp && sp->sb && sp->sb->storage_class == sc_typedef && !sp->sb->typeAlias)
                {
                    SYMBOL* typedefSym = sp;
                    istypedef = true;
                    if (isstructured(sp->tp) && !sp->sb->templateLevel && throughClass)
                    {
                        sp = basetype(sp->tp)->sp;
                        sp->sb->typedefSym = typedefSym;
                        *throughClass = true;
                    }
                    else if (sp->tp->type == bt_typedef)
                    {
                        if (sp->tp->btp->type == bt_typedef)
                        {
                            sp = sp->tp->btp->sp;
                        }
                        else if (isstructured(sp->tp->btp))
                        {
                            sp = basetype(sp->tp->btp)->sp;
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
                if (structLevel && !templateNestingCount && strSym->sb->templateLevel &&
                    (!strSym->sb->instantiated || strSym->sb->attribs.inheritable.linkage4 != lk_virtual))
                {
                    sp = nullptr;
                }
                else
                {
                    STRUCTSYM s;
                    s.str = strSym;
                    addStructureDeclaration(&s);
                    sp = classsearch(buf, false, MATCHKW(lex, classsel), false);
                    dropStructureDeclaration();
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
                        if (MATCHKW(lex, lt))
                        {
                            lex = GetTemplateArguments(lex, nullptr, nullptr, &templateSelector->back().templateParams);
                        }
                        else if (MATCHKW(lex, classsel))
                        {
                            errorstr(ERR_NEED_TEMPLATE_ARGUMENTS, buf);
                        }
                    }
                    if (!MATCHKW(lex, classsel))
                        break;
                    lex = getsym();
                    finalPos = lex;
                }
                sp_orig = sp;
                if (sp && sp->sb && sp->sb->typeAlias && !sp->sb->templateLevel && isstructured(sp->tp))
                {
                    istypedef = true;
                    sp = basetype(sp->tp)->sp;
                }
                else if (sp && sp->sb && sp->tp->type == bt_typedef)
                {
                    istypedef = true;
                    if (sp->tp->btp->type == bt_typedef)
                    {
                        sp = sp->tp->btp->sp;
                    }
                    else if (isstructured(sp->tp->btp))
                    {
                        sp = basetype(sp->tp->btp)->sp;
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
                if (sp && basetype(sp->tp)->type == bt_enum)
                {
                    if (!MATCHKW(lex, classsel))
                        break;
                    lex = getsym();
                    finalPos = lex;
                    strSym = sp;
                    qualified = true;
                    break;
                }
                else if (sp)
                {
                    if (sp->sb && sp->sb->templateLevel && (!sp->sb->instantiated || MATCHKW(lex, lt)))
                    {
                        hasTemplateArgs = true;
                        if (MATCHKW(lex, lt))
                        {
                            current = nullptr;
                            lex = GetTemplateArguments(lex, nullptr, sp_orig, &current);
                        }
                        else if (MATCHKW(lex, classsel))
                        {
                            currentsp = sp;
                            if (!istypedef)
                                SpecializationError(sp);
                        }
                        if (!MATCHKW(lex, classsel))
                            break;
                    }
                    else
                    {
                        if (!MATCHKW(lex, classsel))
                            break;
                        if (hasTemplate &&
                            (basetype(sp->tp)->type != bt_templateparam || basetype(sp->tp)->templateParam->second->type != kw_template))
                        {
                            errorsym(ERR_NOT_A_TEMPLATE, sp);
                        }
                    }
                }
                else if (templateParamAsTemplate)
                {
                    hasTemplateArgs = true;
                    if (MATCHKW(lex, lt))
                    {
                        lex = GetTemplateArguments(lex, nullptr, sp, &current);
                    }
                    else if (MATCHKW(lex, classsel))
                    {
                        currentsp = sp;
                        SpecializationError(sp);
                    }
                    if (!MATCHKW(lex, classsel))
                        break;
                }
                else if (!MATCHKW(lex, classsel))
                    break;
                if (templateParamAsTemplate)
                {
                    matchTemplateSpecializationToParams(
                        current, templateParamAsTemplate->second->byTemplate.args,
                        templateParamAsTemplate->first);  // this function is apparently undefined in this file
                }
                if (hasTemplateArgs)
                {
                    deferred = inTemplateHeader || parsingSpecializationDeclaration || parsingTrailingReturnOrUsing;
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
                            if (basetype(sp->tp)->type == bt_templateselector)
                            {
                                if (sp->sb->mainsym && sp->sb->mainsym->sb->storage_class == sc_typedef &&
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
                                    if (sp->sb->storage_class == sc_typedef)
                                    {
                                        sp = GetTypeAliasSpecialization(sp, current);
                                        if (isstructured(sp->tp))
                                            sp = basetype(sp->tp)->sp;
                                    }
                                    else
                                    {
                                        sp = GetClassTemplate(sp, current, false);
                                    }
                                    if (!sp)
                                    {
                                        if (templateNestingCount)  // || noSpecializationError)
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
                    sp->tp = PerformDeferredInitialization(sp->tp, nullptr);
                }
                if (sp && (!sp->sb || (sp->sb->storage_class != sc_namespace && (!isstructured(sp->tp) || sp->templateParams))))
                    pastClassSel = true;
                lex = getsym();
                finalPos = lex;
                if (deferred)
                {
                    if (istypedef && sp->sb->mainsym && sp->sb->mainsym->sb->templateLevel)
                    {
                        sp->tp = sp->sb->mainsym->tp;
                    }
                    if (sp && sp->tp->type == bt_templateselector)
                    {
                        if (basetype(sp->tp)->sp->sb->templateSelector)
                        {
                            for (auto s : *basetype(sp->tp)->sp->sb->templateSelector)
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
                else if (sp && isstructured(sp->tp))
                {
                    strSym = sp;
                    if (!qualified)
                        nssym = nullptr;
                }
                else if (sp && sp->sb && (sp->sb->storage_class == sc_namespace || sp->sb->storage_class == sc_namespacealias))
                {
                    nssym = sp->sb->nameSpaceValues;
                }
                else if (sp && (basetype(sp->tp)->type == bt_templateparam || basetype(sp->tp)->type == bt_templateselector))
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
                    if (!templateNestingCount || !sp)
                    {
                        if (dependentType)
                            if (isstructured(dependentType))
                                errorstringtype(ERR_DEPENDENT_TYPE_DOES_NOT_EXIST_IN_TYPE, buf, basetype(dependentType));
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
        if (MATCHKW(lex, kw_template))
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
    if (!pastClassSel && typeName && !dependentType && !inTypedef && (!templateNestingCount || instantiatingTemplate))
    {
        error(ERR_NO_TYPENAME_HERE);
    }
    lex = prevsym(finalPos);
    if (templateSelector)
    {
        auto tp = MakeType(bt_templateselector);
        *sym = makeID(sc_global, tp, nullptr, AnonymousName());
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
    if (cls->sb->storage_class == sc_typedef)
        cls = basetype(cls->tp)->sp;
    if (cls->sb->templateLevel && cls->templateParams)
    {
        if (!basetype(cls->tp)->syms)
        {
            TemplateClassInstantiate(cls, cls->templateParams, false, sc_global);
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
        rv = search(basetype(cls->tp)->syms, name);
    if (!rv)
        rv = search(basetype(cls->tp)->tags, name);
    if (rv)
    {
        if (!last || ((last == rv || sameTemplate(last->tp, rv->tp) || (rv->sb->mainsym && rv->sb->mainsym == last->sb->mainsym)) &&
                      (((isvirtual && isvirtual == last->sb->temp) || ismember(rv)) ||
                       (((last->sb->storage_class == sc_type && rv->sb->storage_class == sc_type) ||
                         (last->sb->storage_class == sc_typedef && rv->sb->storage_class == sc_typedef)) &&
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
        auto old = args->front().second->type == kw_new ? args->front().second->bySpecialization.next : nullptr;
        for (auto&& arg : *args)
        {
            if (arg.first && !strcmp(arg.first->name, name))
            {
                if (arg.second->type == kw_template && arg.second->byTemplate.dflt)
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
    SYMBOL* cls = getStructureDeclaration();
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
SYMBOL* classsearch(const char* name, bool tagsOnly, bool needTypeOrNamespace, bool toErr)
{
    SYMBOL* rv = nullptr;
    SYMBOL* symrv = nullptr;
    SYMBOL* cls = getStructureDeclaration();

    decltype(structSyms)::iterator its;
    for( its = structSyms.begin(); its != structSyms.end(); ++its)
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
                rv = search(basetype(cls->tp)->syms, name);
            if (rv && needTypeOrNamespace && !istype(rv))
            {
                symrv = rv;
                rv = nullptr;
            }
            if (!rv)
                rv = search(basetype(cls->tp)->tags, name);
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
    for (; its != structSyms.end(); ++its)
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
    cls = getStructureDeclaration();
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
        SYMBOL* ssp = getStructureDeclaration();
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
                    rv = search(basetype(lambdas.front()->lthis->tp)->btp->syms, name);
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
            STRUCTSYM l;
            l.str = (SYMBOL*)encloser;
            addStructureDeclaration(&l);
            rv = classsearch(name, tagsOnly, false, true);
            dropStructureDeclaration();
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
LEXLIST* nestedSearch(LEXLIST* lex, SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv, bool* destructor, bool* isTemplate,
                      bool tagsOnly, enum e_sc storage_class, bool errIfNotFound, bool isType)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    LEXLIST* placeholder = lex;
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

    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly, storage_class, isType);
    if (Optimizer::cparams.prm_cplusplus)
    {

        if (MATCHKW(lex, complx))
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
        else if (MATCHKW(lex, kw_template))
        {
            lex = getsym();
            if (isTemplate)
                *isTemplate = true;
            hasTemplate = true;
        }
    }
    if (ISID(lex) || MATCHKW(lex, kw_operator))
    {
        if (encloser && encloser->tp->type == bt_templateselector)
        {
            if (destructor && *destructor && encloser->sb->templateSelector->size() == 2)
            {
                encloser->sb->templateSelector->push_back(TEMPLATESELECTOR{});
                encloser->sb->templateSelector->back().name = (*encloser->sb->templateSelector)[1].name;
            }
            *sym = makeID(sc_type, encloser->tp, nullptr, encloser->sb->templateSelector->back().name);
        }
        else
        {
            if (!ISID(lex))
            {
                char buf[512];
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
                    *sym = finishSearch(lex->data->value.s.a, encloser, ns, tagsOnly, throughClass, namespaceOnly);
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
            ((*sym)->tp->type != bt_templateparam || (*sym)->tp->templateParam->second->type != kw_template) &&
            (*sym)->tp->type != bt_templateselector && (*sym)->tp->type != bt_templatedecltype)
        {
            if ((*sym)->sb->storage_class == sc_overloads)
            {
                bool found = false;
                for (auto sym1 : *basetype((*sym)->tp)->syms)
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
LEXLIST* getIdName(LEXLIST* lex, SYMBOL* funcsp, char* buf, int* ov, TYPE** castType)
{
    buf[0] = 0;
    if (ISID(lex))
    {
        strcpy(buf, lex->data->value.s.a);
    }
    else if (MATCHKW(lex, kw_operator))
    {
        lex = getsym();
        if (ISKW(lex) && lex->data->kw->key >= kw_new && lex->data->kw->key <= complx)
        {
            enum e_kw kw = lex->data->kw->key;
            switch (kw)
            {
                case openpa:
                    lex = getsym();
                    if (!MATCHKW(lex, closepa))
                    {
                        needkw(&lex, closepa);
                        lex = backupsym();
                    }
                    break;
                case openbr:
                    lex = getsym();
                    if (!MATCHKW(lex, closebr))
                    {
                        needkw(&lex, closebr);
                        lex = backupsym();
                    }
                    break;
                case kw_new:
                case kw_delete:
                    lex = getsym();
                    if (!MATCHKW(lex, openbr))
                    {
                        lex = backupsym();
                    }
                    else
                    {
                        kw = (e_kw)(kw - kw_new + complx + 1);
                        lex = getsym();
                        if (!MATCHKW(lex, closebr))
                        {
                            needkw(&lex, closebr);
                            lex = backupsym();
                        }
                    }
                    break;
                default:
                    break;
            }
            strcpy(buf, overloadNameTab[*ov = kw - kw_new + CI_NEW]);
        }
        else if (ISID(lex) || startOfType(lex, nullptr, false))  // potential cast operator
        {
            TYPE* tp = nullptr;
            lex = get_type_id(lex, &tp, funcsp, sc_cast, true, true, false);
            if (!tp)
            {
                errorstr(ERR_INVALID_AS_OPERATOR, "");
                tp = &stdint;
            }
            if (castType)
            {
                *castType = tp;
                if (isautotype(tp) & !lambdas.size())  // make an exception so we can compile templates for lambdas
                    error(ERR_AUTO_NOT_ALLOWED_IN_CONVERSION_FUNCTION);
            }
            strcpy(buf, overloadNameTab[*ov = CI_CAST]);
        }
        else if (lex->data->type == l_astr)
        {
            LEXLIST* placeholder = lex;
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
LEXLIST* id_expression(LEXLIST* lex, SYMBOL* funcsp, SYMBOL** sym, SYMBOL** strSym, std::list<NAMESPACEVALUEDATA*>** nsv, bool* isTemplate,
                       bool tagsOnly, bool membersOnly, char* idname)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    TYPE* castType = nullptr;
    LEXLIST* placeholder = lex;
    char buf[512];
    int ov = 0;
    bool hasTemplate = false;
    bool namespaceOnly = false;

    *sym = nullptr;

    if (MATCHKW(lex, classsel))
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
                SYMBOL* ssp = getStructureDeclaration();
                if (ssp)
                {
                    *sym =search( ssp->tp->syms, lex->data->value.s.a);
                }
                if (*sym == nullptr)
                    *sym = gsearch(lex->data->value.s.a);
            }
        }
        return lex;
    }
    lex = nestedPath(lex, &encloser, &ns, &throughClass, tagsOnly, sc_global, false);
    if (MATCHKW(lex, complx))
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
        if (MATCHKW(lex, kw_template))
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
                encloser = getStructureDeclaration();
            *sym =
                finishSearch(ov == CI_CAST ? overloadNameTab[CI_CAST] : buf, encloser, ns, tagsOnly, throughClass, namespaceOnly);
            if (*sym && hasTemplate)
            {
                if ((*sym)->sb->storage_class == sc_overloads)
                {
                    bool found = false;
                    for (auto sym1 : *basetype((*sym)->tp)->syms)
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
    else if (!*sym && (!encloser || encloser->tp->type != bt_templateselector))
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
                if (isfunction(sym->tp) && sym->sb->parentClass == frnd->sb->parentClass && !strcmp(sym->name, frnd->name) &&
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
static bool isAccessibleInternal(SYMBOL* derived, SYMBOL* currentBase, SYMBOL* member, SYMBOL* funcsp, enum e_ac minAccess,
                                 enum e_ac maxAccess, int level)
{
    enum e_ac memberAccess = member->sb->access > maxAccess ? maxAccess : member->sb->access;
    BASECLASS* lst;
    SYMBOL* ssp;
    if (!Optimizer::cparams.prm_cplusplus)
        return true;
    ssp = getStructureDeclaration();
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
        if (!basetype(currentBase->tp)->syms)
            return false;
        if (member->sb->parentClass == currentBase || member->sb->parentClass == currentBase->sb->mainsym)
            return memberAccess >= minAccess || level == 0;
        else if (member->sb->mainsym)
            if (member->sb->mainsym->sb->parentClass == currentBase || member->sb->mainsym->sb->parentClass == currentBase->sb->mainsym)
                return memberAccess >= minAccess || level == 0;
        if (currentBase->sb->baseClasses)
        {
            for (auto lst : *currentBase->sb->baseClasses)
            {
                SYMBOL* sym = lst->cls;
                sym = basetype(sym->tp)->sp;

                // we have to go through the base classes even if we know that a normal
                // lookup wouldn't work, so we can check their friends lists...
                if (isAccessibleInternal(derived, sym, member, funcsp, minAccess, lst->accessLevel < maxAccess ? ac_protected : maxAccess, lst->accessLevel == ac_private ? 2 : 1))
                    return true;
            }
        }
    }
    return (level == 0 && memberAccess >= minAccess);
}
bool isAccessible(SYMBOL* derived, SYMBOL* currentBase, SYMBOL* member, SYMBOL* funcsp, enum e_ac minAccess, bool asAddress)
{
    return (templateNestingCount && !instantiatingTemplate) || instantiatingFunction || member->sb->accessibleTemplateArgument ||
           isAccessibleInternal(derived, currentBase, member, funcsp, minAccess, ac_public, 0);
}
static SYMBOL* AccessibleClassInstance(SYMBOL* parent)
{
    // search through all active structure declarations
    // to try to find a structure which is derived from parent...
    for (auto&& s : structSyms)
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
        enum e_ac minAccess = ac_public;
        if (exp && exp->type == en_auto)
        {
            parent = basetype(exp->v.sp->tp)->sp;
        }
        else if (!derived && funcsp && !sym->sb->throughClass)
        {
            if (funcsp->sb->parentClass)
                parent = funcsp->sb->parentClass;
            else
                return sym->sb->access == ac_public;
        }
        SYMBOL* ssp = nullptr;
        if (sym->sb->throughClass && (ssp = AccessibleClassInstance(sym->sb->parentClass)) != nullptr)
        {
            if (ssp == parent)
                minAccess = ac_private;
            else
                minAccess = ac_protected;
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
    TYPE* tp = sp->tp;
    while (tp)
    {
        if (isstructured(tp) || tp->type == bt_typedef || tp->type == bt_enum)
        {
            SYMBOL* sym;
            if (tp->type == bt_typedef)
                sym = tp->sp;
            else
                sym = basetype(tp)->sp;
            if (sym->sb->parentClass)
            {
                SYMBOL* ssp = nullptr;
                if ((ssp = AccessibleClassInstance(sym->sb->parentClass)) != nullptr)
                {
                    if (!isAccessible(ssp, ssp, sym, funcsp, ac_protected, false))
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
                            if (isAccessible(derived, sym->sb->parentClass, sym, funcsp, ac_public, false))
                                return true;
                            derived = derived->sb->parentClass;
                        }
                        errorsym(ERR_CANNOT_ACCESS, sym);
                        return false;
                    }
                    else
                    {
                        if (!isAccessible(derived, sym->sb->parentClass, sym, funcsp, ac_public, false))
                        {
                            errorsym(ERR_CANNOT_ACCESS, sym);
                            return false;
                        }
                    }
                }
            }
            break;
        }
        else if (isfunction(tp))
        {
            for (auto sym : *basetype(tp)->syms)
            {
                if (!checkDeclarationAccessible(sym, funcsp ? funcsp->sb->parentClass : nullptr, funcsp))
                    return false;
            }
        }
        tp = tp->btp;
    }
    return true;
}
void searchNS(SYMBOL* sym, SYMBOL* nssp, std::list<SYMBOL*>& gather)
{
    if (nssp)
    {
        auto x = namespacesearchInternal(sym->name, nssp->sb->nameSpaceValues, true, false, false);
        if (x.size())
        {
            gather.insert(gather.end(), x.begin(), x.end());
        }
    }
}
SYMBOL* lookupGenericConversion(SYMBOL* sym, TYPE* tp)
{
    inGetUserConversion -= 3;
    SYMBOL* rv = getUserConversion(F_CONVERSION | F_WITHCONS, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, false);
    inGetUserConversion += 3;
    return rv;
}
SYMBOL* lookupSpecificCast(SYMBOL* sym, TYPE* tp)
{
    return getUserConversion(F_CONVERSION | F_STRUCTURE, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, false);
}
SYMBOL* lookupNonspecificCast(SYMBOL* sym, TYPE* tp)
{
    return getUserConversion(F_CONVERSION, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, true);
}
SYMBOL* lookupIntCast(SYMBOL* sym, TYPE* tp, bool implicit)
{
    return getUserConversion(F_CONVERSION | F_INTEGER, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, implicit);
}
SYMBOL* lookupArithmeticCast(SYMBOL* sym, TYPE* tp, bool implicit)
{
    return getUserConversion(F_CONVERSION | F_ARITHMETIC, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, implicit);
}
SYMBOL* lookupPointerCast(SYMBOL* sym, TYPE* tp)
{
    return getUserConversion(F_CONVERSION | F_POINTER, tp, sym->tp, nullptr, nullptr, nullptr, nullptr, nullptr, true);
}
static void structuredArg(SYMBOL* sym, std::list<SYMBOL*>& gather, TYPE* tp)
{
    if (basetype(tp)->sp->sb->parentNameSpace)
    {
        searchNS(sym, basetype(tp)->sp->sb->parentNameSpace, gather);
        return;
    }
    // a null value means the global namespace
    SYMBOL nssp = {0};
    SYMBOL::_symbody sb = {0};
    nssp.sb = &sb;
    sb.nameSpaceValues = rootNameSpace;
    searchNS(sym, &nssp, gather);
}
static void searchOneArg(SYMBOL* sym, std::list<SYMBOL*>& gather, TYPE* tp);
static void funcArg(SYMBOL* sp, std::list<SYMBOL*>& gather, TYPE* tp)
{
    for (auto sym : *basetype(tp)->syms)
    {
        searchOneArg(sp, gather, sym->tp);
    }
    searchOneArg(sp, gather, basetype(tp)->btp);
}
static void searchOneArg(SYMBOL* sym, std::list<SYMBOL*>& gather, TYPE* tp)
{
    if (ispointer(tp) || isref(tp))
    {
        searchOneArg(sym, gather, basetype(tp)->btp);
    }
    if (isarithmetic(tp))
    {
        tp = basetype(tp);
        if (tp->btp && tp->btp->type == bt_enum)
            structuredArg(sym, gather, tp);
        return;
    }
    if (isstructured(tp) || basetype(tp)->type == bt_enum)
    {
        structuredArg(sym, gather, tp);
        return;
    }
    if (isfunction(tp))
    {
        funcArg(sym, gather, tp);
        return;
    }
    // member pointers...
}
static void weedToFunctions(std::list<SYMBOL*>& lst)
{
    std::list<SYMBOL*> remove;
    for (auto sym : lst)
    {
        if (sym->sb->storage_class != sc_overloads)
            remove.insert(remove.begin(), sym);
    }
    for (auto sym : remove)
    {
        lst.remove(sym);
    }
}
static void GatherConversions(SYMBOL* sym, SYMBOL** spList, int n, FUNCTIONCALL* args, TYPE* atp, enum e_cvsrn** icsList,
                              int** lenList, int argCount, SYMBOL*** funcList, bool usesInitList)
{
    int i;
    for (i = 0; i < n; i++)
    {
        int j;
        if (spList[i])
        {
            enum e_cvsrn arr[500][10];
            int counts[500];
            SYMBOL* funcs[200];
            bool t;
            memset(counts, 0, argCount * sizeof(int));
            for (j = i + 1; j < n; j++)
                if (spList[i] == spList[j])
                    spList[j] = 0;
            memset(funcs, 0, sizeof(funcs));
            t = getFuncConversions(spList[i], args, atp, sym->sb->parentClass, (enum e_cvsrn*)arr, counts, argCount, funcs,
                                   usesInitList);
            if (!t)
            {
                spList[i] = nullptr;
            }
            else
            {
                int n1 = 0;
                for (j = 0; j < argCount; j++)
                    n1 += counts[j];
                icsList[i] = Allocate<e_cvsrn>(n1);
                memcpy(icsList[i], arr, n1 * sizeof(enum e_cvsrn));
                lenList[i] = Allocate<int>(argCount);
                memcpy(lenList[i], counts, argCount * sizeof(int));
                funcList[i] = Allocate<SYMBOL*>(argCount);
                memcpy(funcList[i], funcs, argCount * sizeof(SYMBOL*));
            }
        }
    }
}
enum e_ct
{
    conv,
    user,
    ellipses
};
static bool ismath(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case en_uminus:
        case en_compl:
        case en_not:
        case en_shiftby:
        case en_autoinc:
        case en_autodec:
        case en_add:
        case en_sub:
        case en_lsh:
        case en_arraylsh:
        case en_rsh:
        case en_arraymul:
        case en_arrayadd:
        case en_arraydiv:
        case en_structadd:
        case en_mul:
        case en_div:
        case en_umul:
        case en_udiv:
        case en_umod:
        case en_ursh:
        case en_mod:
        case en_and:
        case en_or:
        case en_xor:
        case en_lor:
        case en_land:
        case en_eq:
        case en_ne:
        case en_gt:
        case en_ge:
        case en_lt:
        case en_le:
        case en_ugt:
        case en_uge:
        case en_ult:
        case en_ule:
        case en_cond:
        case en_select:
            return true;
        default:
            return false;
    }
}
static bool ismem(EXPRESSION* exp)
{
    switch (exp->type)
    {
        case en_global:
        case en_pc:
        case en_auto:
        case en_threadlocal:
        case en_construct:
        case en_labcon:
            return true;
        case en_thisref:
            exp = exp->left;
            if (exp->v.func->sp->sb->isConstructor || exp->v.func->sp->sb->isDestructor)
                return false;
            /* fallthrough */
        case en_func: {
            TYPE* tp = exp->v.func->sp->tp;
            if (tp->type == bt_aggregate || !isfunction(tp))
                return false;
            tp = basetype(tp)->btp;
            return ispointer(tp) || isref(tp);
        }
        case en_add:
        case en_sub:
        case en_structadd:
            return ismem(exp->left) || ismem(exp->right);
        case en_l_p:
            return (exp->left->type == en_auto && exp->left->v.sp->sb->thisPtr);
        default:
            return false;
    }
}
static TYPE* toThis(TYPE* tp)
{
    if (ispointer(tp))
        return tp;
    return MakeType(bt_pointer, tp);
}
static int compareConversions(SYMBOL* spLeft, SYMBOL* spRight, enum e_cvsrn* seql, enum e_cvsrn* seqr, TYPE* ltype, TYPE* rtype,
                              TYPE* atype, EXPRESSION* expa, SYMBOL* funcl, SYMBOL* funcr, int lenl, int lenr, bool fromUser)
{
    (void)spLeft;
    (void)spRight;
    enum e_ct xl = conv, xr = conv;
    int lderivedfrombase = 0, rderivedfrombase = 0;
    int rankl, rankr;
    int i;
    // must be of same general type, types are standard conversion, user defined conversion, ellipses
    for (i = 0; i < lenl; i++)
    {
        if (seql[i] == CV_ELLIPSIS)
            xl = ellipses;
        if (xl != ellipses && seql[i] == CV_USER)
            xl = user;
    }
    for (i = 0; i < lenr; i++)
    {
        if (seqr[i] == CV_ELLIPSIS)
            xr = ellipses;
        if (xr != ellipses && seqr[i] == CV_USER)
            xr = user;
    }
    if (xl != xr)
    {
        if (xl < xr)
            return -1;
        else
            return 1;
    }
    if (xl == conv)
    {
        // one seq is a subseq of the other
        int l = 0, r = 0;
        for (; l < lenl; l++)
            if (seql[l] == CV_DERIVEDFROMBASE || seql[l] == CV_LVALUETORVALUE)
                lderivedfrombase++;
        for (; r < lenr; r++)
            if (seqr[r] == CV_DERIVEDFROMBASE || seqr[r] == CV_LVALUETORVALUE)
                rderivedfrombase++;
        l = 0, r = 0;
        for (; l < lenl && r < lenr;)
        {
            bool cont = false;
            switch (seql[l])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    l++;
                    cont = true;
                    break;
                default:
                    break;
            }
            switch (seqr[r])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    r++;
                    cont = true;
                    break;
                default:
                    break;
            }
            if (cont)
                continue;
            if (seql[l] != seqr[r])
                break;
            l++, r++;
        }
        // special check, const zero to pointer is higher pref than int
        if (expa && isconstzero(ltype, expa))
        {
            auto lt2 = ltype;
            if (isref(lt2))
            {
                lt2 = basetype(lt2)->btp;
                if (ispointer(lt2))
                {
                    lt2 = rtype;
                    if (isref(lt2))
                        lt2 = basetype(lt2)->btp;
                    if (isint(lt2))
                        return -1;
                }
            }
        }
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (l == lenl && r != lenr)
        {
            return -1;
        }
        else if (l != lenl && r == lenr)
        {
            return 1;
        }
        // compare ranks
        rankl = CV_IDENTITY;
        for (l = 0; l < lenl; l++)
            if (rank[seql[l]] > rankl && seql[l] != CV_DERIVEDFROMBASE)
                rankl = rank[seql[l]];
        rankr = CV_IDENTITY;
        for (r = 0; r < lenr; r++)
            if (rank[seqr[r]] > rankr && seqr[r] != CV_DERIVEDFROMBASE)
                rankr = rank[seqr[r]];
        if (rankl < rankr)
            return -1;
        else if (rankr < rankl)
            return 1;
        else if (lenl < lenr)
        {
            return -1;
        }
        else if (lenr < lenl)
        {
            return 1;
        }
        else
        {

            // ranks are same, do same rank comparisons
            TYPE* tl = ltype, * tr = rtype, * ta = atype;
            // check if one or the other but not both converts a pointer to bool
            rankl = 0;
            for (l = 0; l < lenl; l++)
                if (seql[l] == CV_BOOLCONVERSION)
                    rankl = 1;
            rankr = 0;
            for (r = 0; r < lenr; r++)
                if (seqr[r] == CV_BOOLCONVERSION)
                    rankr = 1;
            if (rankl != rankr)
            {
                if (rankl)
                    return 1;
                else
                    return -1;
            }
            if (fromUser)
            {
                // conversion from pointer to base class to void * is better than pointer
                // to derived class to void *
                if (ispointer(ta) && basetype(basetype(ta)->btp)->type == bt_void)
                {
                    SYMBOL* second = basetype(basetype(tl)->btp)->sp;
                    SYMBOL* first = basetype(basetype(tr)->btp)->sp;
                    int v;
                    v = classRefCount(first, second);
                    if (v == 1)
                        return 1;
                    v = classRefCount(second, first);
                    if (v == 1)
                        return -1;
                }
            }
            else if (ta)
            {
                // conversion to pointer to base class is better than conversion to void *
                if (ispointer(tl) && ispointer(ta) && basetype(basetype(tl)->btp)->type == bt_void)
                {
                    if (isstructured(basetype(ta)->btp))
                    {
                        if (ispointer(tr) && isstructured(basetype(tr)->btp))
                        {
                            SYMBOL* derived = basetype(basetype(ta)->btp)->sp;
                            SYMBOL* base = basetype(basetype(tr)->btp)->sp;
                            int v = classRefCount(base, derived);
                            if (v == 1)
                                return 1;
                        }
                    }
                }
                else if (ispointer(tr) && ispointer(ta) && basetype(basetype(tr)->btp)->type == bt_void)
                {
                    if (isstructured(basetype(ta)->btp))
                    {
                        if (ispointer(tl) && isstructured(basetype(tl)->btp))
                        {
                            SYMBOL* derived = basetype(basetype(ta)->btp)->sp;
                            SYMBOL* base = basetype(basetype(tl)->btp)->sp;
                            int v = classRefCount(base, derived);
                            if (v == 1)
                                return -1;
                        }
                    }
                }
            }
            // various rules for the comparison of two pairs of structures
            if (ta && ispointer(ta) && ispointer(tr) && ispointer(tl))
            {
                ta = basetype(ta)->btp;
                tl = basetype(tl)->btp;
                tr = basetype(tr)->btp;
                // prefer a const function when the expression is a string literal
                if (expa->type == en_labcon)
                {
                    if (isconst(tl))
                    {
                        if (!isconst(tr))
                            return -1;
                    }
                    else if (isconst(tr))
                        return 1;
                }
                // if qualifiers are mismatched, choose a matching argument

                bool va = isvolatile(ta);
                bool vl = isvolatile(tl);
                bool vr = isvolatile(tr);
                bool ca = isconst(ta);
                bool cl = isconst(tl);
                bool cr = isconst(tr);
                if (cl == cr && vl != vr)
                {
                    if (va == vl)
                        return -1;
                    else if (va == vr)
                        return 1;
                }
                else if (vl == vr && cl != cr)
                {
                    if (ca == cl)
                        return -1;
                    else if (ca == cr)
                        return 1;
                }
            }
            else
            {
                if (isref(tl) && isref(tr))
                {
                    enum e_bt refa = bt_rref;
                    if (ta)
                    {
                        if (ta->lref || basetype(ta)->lref)
                            refa = bt_lref;

                    }
                    if (refa == bt_rref && expa && !ta->rref && !basetype(ta)->rref)
                    {
                        if (expa->type != en_thisref && expa->type != en_func)
                            refa = bt_lref;
                    }
                    // const rref is better than const lref
                    enum e_bt refl = basetype(tl)->type;
                    enum e_bt refr = basetype(tr)->type;
                    if (refl == bt_rref && refr == bt_lref && isconst(basetype(tr)->btp))
                    {
                        if (refa != bt_lref || isconst(basetype(ta)->btp))
                            return -1;
                        else
                            return 1;
                    }
                    if (refr == bt_rref && refl == bt_lref && isconst(basetype(tl)->btp))
                    {
                        if (refa != bt_lref || isconst(basetype(ta)->btp))
                            return 1;
                        else
                            return -1;
                    }
                    if (ta && !isref(ta))
                    {
                        // try to choose a const ref when there are two the same
                        if (refl == refr)
                        {
                            bool lc = isconst(basetype(tl)->btp);
                            bool rc = isconst(basetype(tr)->btp);
                            if (lc && !rc)
                                return -1;
                            if (rc && !lc)
                                return 1;
                        }
                    }
                }
                if (ta && isref(ta))
                    ta = basetype(ta)->btp;
                if (isref(tl))
                    tl = basetype(tl)->btp;
                if (isref(tr))
                    tr = basetype(tr)->btp;
            }


            if (ta && isstructured(ta) && isstructured(tl) && isstructured(tr))
            {
                ta = basetype(ta);
                tl = basetype(tl);
                tr = basetype(tr);
                int cmpl = comparetypes(tl, ta, true) && sameTemplate(tl, ta);
                int cmpr = comparetypes(tr, ta, true) && sameTemplate(tr, ta);
                if (fromUser)
                {
                    if (cmpr || cmpl)
                    {
                        if (cmpr)
                        {
                            if (cmpl)
                                return 0;
                            return -1;
                        }
                        else
                            return 1;
                    }
                    else if (classRefCount(ta->sp, tl->sp) == 1 && classRefCount(ta->sp, tr->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return -1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return 1;
                                else
                                    return 0;
                            }
                            return -1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return 1;
                        }
                    }
                }
                else
                {
                    if (cmpr || cmpl)
                    {
                        if (cmpr)
                        {
                            if (cmpl)
                                return 0;
                            return 1;
                        }
                        else
                            return -1;
                    }
                    else if (classRefCount(tl->sp, ta->sp) == 1 && classRefCount(tr->sp, ta->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return 1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return -1;
                                else
                                    return 0;
                            }
                            return 1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return -1;
                        }
                    }
                }
            }

            if (ta && basetype(ta)->type == bt_memberptr && basetype(tl)->type == bt_memberptr &&
                basetype(tr)->type == bt_memberptr)
            {
                ta = basetype(ta);
                tl = basetype(tl);
                tr = basetype(tr);
                if (fromUser)
                {
                    if (classRefCount(tl->sp, ta->sp) == 1 && classRefCount(tr->sp, ta->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return 1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return -1;
                                else
                                    return 0;
                            }
                            return 1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return -1;
                        }
                    }
                }
                else
                {
                    if (classRefCount(ta->sp, tl->sp) == 1 && classRefCount(ta->sp, tr->sp) == 1)
                    {
                        if (classRefCount(tl->sp, tr->sp) == 1)
                        {
                            if (classRefCount(tr->sp, tl->sp) == 1)
                            {
                                if (lderivedfrombase > rderivedfrombase)
                                    return -1;
                                else if (rderivedfrombase > lderivedfrombase)
                                    return 1;
                                else
                                    return 0;
                            }
                            return -1;
                        }
                        else if (classRefCount(tr->sp, tl->sp) == 1)
                        {
                            return 1;
                        }
                    }
                }
            }
        }
        // compare qualifiers at top level
        rankl = !!isconst(ltype) + !!isvolatile(ltype) * 2;
        rankr = !!isconst(rtype) + !!isvolatile(rtype) * 2;
        if (rankl != rankr)
        {
            if (comparetypes(basetype(ltype), basetype(rtype), true))
            {
                int n1 = rankl ^ rankr;
                if ((n1 & rankl) && !(n1 & rankr))
                    return 1;
                if ((n1 & rankr) && !(n1 & rankl))
                    return -1;
            }
        }
        if (atype && isref(rtype) && isref(ltype))
        {
            // rvalue matches an rvalue reference better than an lvalue reference

            if (isref(rtype) && isref(ltype) && basetype(ltype)->type != basetype(rtype)->type)
            {
                int lref = expa && lvalue(expa);
                int rref = expa && (!lvalue(expa) && (!isstructured(rtype) || !ismem(expa)));
                if (expa && expa->type == en_func)
                {
                    TYPE* tp = basetype(expa->v.func->sp->tp)->btp;
                    if (tp)
                    {
                        if (tp->type == bt_rref)
                            rref = true;
                        if (tp->type == bt_lref)
                            lref = true;
                    }
                }
                lref |= expa && isstructured(atype) && expa->type != en_not_lvalue;
                if (basetype(ltype)->type == bt_rref)
                {
                    if (lref)
                        return 1;
                    else if (rref)
                        return -1;
                }
                else if (basetype(ltype)->type == bt_lref)
                {
                    if (lref)
                        return -1;
                    else if (rref)
                        return 1;
                }
            }
            // compare qualifiers at top level
            rankl = !!isconst(basetype(ltype)->btp) + !!isvolatile(basetype(ltype)->btp) * 2;
            rankr = !!isconst(basetype(rtype)->btp) + !!isvolatile(basetype(rtype)->btp) * 2;
            if (rankl != rankr)
            {
                if (comparetypes(basetype(basetype(ltype)->btp), basetype(basetype(rtype)->btp), true))
                {
                    int n1 = rankl ^ rankr;
                    if ((n1 & rankl) && !(n1 & rankr))
                        return 1;
                    if ((n1 & rankr) && !(n1 & rankl))
                        return -1;
                }
            }
        }
        // make sure base types are same
        if (atype)
        {
            while (ispointer(ltype) || isref(ltype))
                ltype = basetype(ltype)->btp;
            while (ispointer(rtype) || isref(rtype))
                rtype = basetype(rtype)->btp;
            while (ispointer(atype) || isref(atype))
                atype = basetype(atype)->btp;
            ltype = basetype(ltype);
            rtype = basetype(rtype);
            atype = basetype(atype);
            if (atype->type == ltype->type)
            {
                if (atype->type != rtype->type)
                    return -1;
            }
            else if (atype->type == rtype->type)
            {
                return 1;
            }
        }
    }
    else if (xl == user)
    {
        TYPE *ta = atype, *tl = ltype, *tr = rtype;
        if (isref(ltype) && isref(rtype))
        {
            // rref is better than const lref
            int refl = basetype(ltype)->type;
            int refr = basetype(rtype)->type;
            if (refl == bt_rref && refr == bt_lref && isconst(basetype(rtype)->btp))
                return -1;
            if (refr == bt_rref && refl == bt_lref && isconst(basetype(ltype)->btp))
                return 1;
        }
        int l = 0, r = 0, llvr = 0, rlvr = 0;
        if (seql[l] == CV_DERIVEDFROMBASE && seqr[r] == CV_DERIVEDFROMBASE)
        {
            auto it = basetype(funcl->tp)->syms->begin();
            if (!funcl->sb->castoperator)
                ++it;
            ltype = (*it)->tp;
            it = basetype(funcr->tp)->syms->begin();
            if (!funcr->sb->castoperator)
                ++it;
            rtype = (*it)->tp;
            if (isref(ltype))
                ltype = basetype(ltype)->btp;
            if (isref(rtype))
                rtype = basetype(rtype)->btp;
            if (isref(atype))
                atype = basetype(atype)->btp;
            ltype = basetype(ltype);
            rtype = basetype(rtype);
            atype = basetype(atype);
            if (classRefCount(ltype->sp, atype->sp) == 1 && classRefCount(ltype->sp, atype->sp) == 1)
            {
                if (classRefCount(ltype->sp, rtype->sp) == 1)
                {
                    return 1;
                }
                else if (classRefCount(rtype->sp, ltype->sp) == 1)
                {
                    return -1;
                }
            }
            if (!comparetypes(ltype, rtype, true))
                return 0;
        }
        if (seql[l] == CV_USER && seqr[r] == CV_USER && funcl && funcr)
        {
            return 0;
        }
        l = 0, r = 0;
        for (; l < lenl && seql[l] != CV_USER && r < lenr && seqr[r] != CV_USER;)
        {
            bool cont = false;
            switch (seql[l])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    l++;
                    cont = true;
                    break;
                case CV_LVALUETORVALUE:
                    llvr++;
                    break;
                default:
                    break;
            }
            switch (seqr[r])
            {
                case CV_ARRAYTOPOINTER:
                case CV_FUNCTIONTOPOINTER:
                    r++;
                    cont = true;
                    break;
                case CV_LVALUETORVALUE:
                    rlvr++;
                default:
                    break;
            }
            if (cont)
                continue;
            if (seql[l] != seqr[r])
                break;
            l++, r++;
        }
        if (llvr && !rlvr)
            return -1;
        if (!llvr && rlvr)
            return 1;
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (seql[l] == CV_USER && seqr[r] != CV_USER)
        {
            return -1;
        }
        else if (seql[l] != CV_USER && seqr[r] == CV_USER)
        {
            return 1;
        }
        while (l < lenl && seql[l] == CV_IDENTITY)
            l++;
        while (r < lenr && seqr[r] == CV_IDENTITY)
            r++;
        if (l == lenl && r != lenr)
        {
            return -1;
        }
        else if (l != lenl && r == lenr)
        {
            return 1;
        }
        l++, r++;
        // compare ranks
        rankl = CV_IDENTITY;
        for (; l < lenl; l++)
            if (rank[seql[l]] > rankl && seql[l] != CV_DERIVEDFROMBASE)
                rankl = rank[seql[l]];
        rankr = CV_IDENTITY;
        for (; r < lenr; r++)
            if (rank[seqr[r]] > rankr && seqr[r] != CV_DERIVEDFROMBASE)
                rankr = rank[seqr[r]];
        if (rankl < rankr)
            return -1;
        else if (rankr < rankl)
            return 1;
        else if (lenl < lenr)
        {
            return -1;
        }
        else if (lenr < lenl)
        {
            return 1;
        }
        // if qualifiers are mismatched, choose a matching argument
        if (tl && tr)
        {
            if (ta && (isref(tl) || isref(tr)))
            {
                bool ll = false;
                bool lr = false;
                if (basetype(tl)->type == bt_rref)
                    lr = true;
                else
                    ll = true;
                bool rl = false;
                bool rr = false;
                if (basetype(tr)->type == bt_rref)
                    rr = true;
                else
                    rl = true;
                if (ll != rl)
                {
                    bool lref = !isref(ta) || basetype(ta)->type == bt_lref;
                    if (ll)
                    {
                        if (lref)
                            return -1;
                        else
                            return 1;
                    }
                    else
                    {
                        if (lref)
                            return 1;
                        else
                            return -1;
                    }
                }
            }
            if (isref(tl))
                tl = basetype(tl)->btp;
            if (isref(tr))
                tr = basetype(tr)->btp;
            bool vl = isvolatile(tl);
            bool vr = isvolatile(tr);
            bool cl = isconst(tl);
            bool cr = isconst(tr);
            if (cl == cr && vl != vr)
            {
                if (vl)
                    return 1;
                else
                    return -1;
            }
            else if (vl == vr && cl != cr)
            {
                if (cl)
                    return 1;
                else
                    return -1;
            }
        }
    }
    // ellipse always returns 0;
    return 0;
}
static bool ellipsed(SYMBOL* sym)
{
    for (auto sym1 : *basetype(sym->tp)->syms)
        if (basetype(sym1->tp)->type == bt_ellipse)
            return true;
    return false;
}
static int ChooseLessConstTemplate(SYMBOL* left, SYMBOL* right)
{
    if (left->templateParams && right->templateParams)
    {
        int lcount = 0, rcount = 0;
        std::list<TEMPLATEPARAMPAIR>::iterator itl, itr, itle, itre;
        auto xx = left->templateParams->front().second->bySpecialization.types;
        if (xx)
        {
            itl = xx->begin();
            itle = xx->end();
        }
        else
        {
            itl = left->templateParams->begin();
            ++itl;
            itle = left->templateParams->end();
        }
        xx = right->templateParams->front().second->bySpecialization.types;
        if (xx)
        {
            itr = xx->begin();
            itre = xx->end();
        }
        else
        {
            itr = right->templateParams->begin();
            ++itr;
            itre = right->templateParams->end();
        }
        for ( ; itl != itle && itr != itre; ++itl, ++itr)
        {
            auto&& tpl = *itl;
            auto&& tpr = *itr;
            if (tpl.second->packed || tpr.second->packed)
                return 0;
            if (tpl.second->type == tpr.second->type && tpl.second->type == kw_typename)
            {
                auto tppl = tpl.second->byClass.val;
                auto tppr = tpr.second->byClass.val;
                if (tppl && tppr)
                {
                    bool lptr = false, rptr = false;
                    while (isref(tppl) || ispointer(tppl))
                    {
                        if (isconst(tppl))
                            lcount++;
                        if (isvolatile(tppl))
                            lcount++;
                        lptr = true;
                        tppl = basetype(tppl)->btp;
                    }
                    while (isref(tppr) || ispointer(tppr))
                    {
                        if (isconst(tppr))
                            rcount++;
                        if (isvolatile(tppr))
                            rcount++;
                        rptr = true;
                        tppr = basetype(tppr)->btp;
                    }
                    if (!lptr)
                    {
                        if (isconst(tppl))
                            lcount++;
                        if (isvolatile(tppl))
                            lcount++;
                    }
                    if (!rptr)
                    {
                        if (isconst(tppr))
                            rcount++;
                        if (isvolatile(tppr))
                            rcount++;
                    }
                    if (isstructured(tppl) && isstructured(tppr))
                    {
                        switch (ChooseLessConstTemplate(basetype(tppl)->sp, basetype(tppr)->sp))
                        {
                        case -1:
                            lcount++;
                            break;
                        case 1:
                            rcount++;
                            break;
                        }

                    }
                }
            }
        }
        if (itl == itle && itr == itre)
        {
            if (lcount < rcount)
            {
                return -1;
            }
            if (rcount < lcount)
            {
                return 1;
            }
        }
    }
    else if (isfunction(left->tp))
    {
        int lcount = 0, rcount = 0;
        auto l = basetype(left->tp)->syms->begin();
        auto lend = basetype(left->tp)->syms->end();
        auto r = basetype(right->tp)->syms->begin();
        auto rend = basetype(right->tp)->syms->end();
        if (isconst(left->tp))
            lcount++;
        if (isconst(right->tp))
            rcount++;
        for (; l != lend && r != rend; ++l, ++r)
        {
            auto ltp = (*l)->tp;
            auto rtp = (*r)->tp;
            while (isref(ltp) || ispointer(ltp))
                ltp = basetype(ltp)->btp;
            while (isref(rtp) || ispointer(rtp))
                rtp = basetype(rtp)->btp;
            if (isstructured(ltp) && isstructured(rtp))
                switch (ChooseLessConstTemplate(basetype(ltp)->sp, basetype(rtp)->sp))
                {
                case -1:
                    lcount++;
                    break;
                case 1:
                    rcount++;
                    break;
                }
        }
        if (l == lend && r == rend)
        {
            if (lcount < rcount)
            {
                return -1;
            }
            if (rcount < lcount)
            {
                return 1;
            }
        }
    }
    return 0;
}
static void SelectBestFunc(SYMBOL** spList, enum e_cvsrn** icsList, int** lenList, FUNCTIONCALL* funcparams, int argCount,
                           int funcCount, SYMBOL*** funcList)
{
    static enum e_cvsrn identity = CV_IDENTITY;
    char arr[500];
    int i, j;
    for (i = 0; i < funcCount; i++)
    {
        for (j = i + 1; j < funcCount && spList[i]; j++)
        {
            if (spList[j])
            {
                if (spList[i] && spList[j])
                {
                    int bothCast = spList[i]->sb->castoperator && spList[j]->sb->castoperator;
                    int left = 0, right = 0;
                    int l = 0, r = 0;
                    int k = 0;
                    int lk = 0, rk = 0;
                    std::list<INITLIST*>* args = funcparams ? funcparams->arguments : nullptr;
                    std::list<INITLIST*>::iterator argit;
                    std::list<INITLIST*>::iterator argite;
                    std::list<INITLIST*> dummy;
                    if (args)
                    {
                        argit = args->begin();
                        argite = args->end();
                    }
                    else
                    {
                        argit = dummy.begin();
                        argite = dummy.end();
                    }
                    auto itl = basetype(spList[i]->tp)->syms->begin();
                    auto itlend = basetype(spList[i]->tp)->syms->end();
                    auto itr = basetype(spList[j]->tp)->syms->begin();
                    auto itrend = basetype(spList[j]->tp)->syms->end();
                    memset(arr, 0, sizeof(arr));
                    for (k = 0; k < argCount ; k++)
                    {
                        enum e_cvsrn* seql = &icsList[i][l];
                        enum e_cvsrn* seqr = &icsList[j][r];
                        int lenl = lenList[i][k];
                        int lenr = lenList[j][k];
                        if (!lenl)
                        {
                            seql = &identity;
                            lenl = 1;
                        }
                        if (!lenr)
                        {
                            seqr = &identity;
                            lenr = 1;
                        }
                        int bl = 0, br = 0;
                        for (int i = 0; i < lenl; i++)
                            if (seql[i] == CV_USER)
                                bl++;
                        for (int i = 0; i < lenr; i++)
                            if (seqr[i] == CV_USER)
                                br++;
                        if (bl > 1 || br > 1 || !spList[i] || !spList[j])
                        {
                            if (bl > 1)
                                spList[i] = nullptr;
                            if (br > 1)
                                spList[j] = nullptr;
                        }
                        else if (k == 0 && funcparams && funcparams->thisptr && (spList[i]->sb->castoperator || (*itl)->sb->thisPtr) && (spList[i]->sb->castoperator || (*itr)->sb->thisPtr))
                        {
                            TYPE *tpl, *tpr;
                            if (0 && spList[i]->sb->castoperator)
                            {
                                tpl = toThis(basetype(spList[i]->tp)->btp);
                            }
                            else
                            {
                                tpl = (*itl)->tp;
                                ++itl;
                            }
                            if (0 && spList[j]->sb->castoperator)
                            {
                                tpr = toThis(basetype(spList[j]->tp)->btp);
                            }
                            else
                            {
                                tpr = (*itr)->tp;
                                ++itr;
                            }
                            arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, funcparams->thistp,
                                                        funcparams->thisptr, funcList ? funcList[i][k] : nullptr,
                                                        funcList ? funcList[j][k] : nullptr, lenl, lenr, false);
                        }
                        else
                        {
                            TYPE *tpl, *tpr;
                            if (funcparams->thisptr)
                            {
                                if (itl != itlend && (*itl)->sb->thisPtr)
                                {
                                    l += lenList[i][k + lk++];
                                    lenl = lenList[i][k+lk];
                                    ++itl;
                                }
                                if (itr != itrend && (*itr)->sb->thisPtr)
                                {
                                    r += lenList[j][k + rk++];
                                    lenr = lenList[j][k+rk];
                                    ++itr;
                                }
                            }
                            if (spList[i]->sb->castoperator)
                                tpl = spList[i]->tp;
                            else
                                tpl = itl != itlend ? (*itl)->tp : nullptr;
                            if (spList[j]->sb->castoperator)
                                tpr = spList[j]->tp;
                            else
                                tpr = itr != itrend ? (*itr)->tp : nullptr;
                            if (tpl && tpr)
                                arr[k] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, argit != argite ? (*argit)->tp : 0,
                                    argit != argite ? (*argit)->exp : 0, funcList ? funcList[i][k + lk] : nullptr,
                                                            funcList ? funcList[j][k + rk] : nullptr, lenl, lenr, false);
                            else
                                arr[k] = 0;
                            if (bothCast)
                            {
                                tpl = basetype(spList[i]->tp)->btp;
                                tpr = basetype(spList[j]->tp)->btp;
                                arr[k + 1] = compareConversions(spList[i], spList[j], seql, seqr, tpl, tpr, argit != argite ? (*argit)->tp : 0,
                                                                argit != argite ? (*argit)->exp : 0, funcList ? funcList[i][k + lk] : nullptr,
                                                                funcList ? funcList[j][k + rk] : nullptr, lenl, lenr, false);
                            }
                            if (itl != itlend)
                                ++itl;
                            if (itr != itrend)
                                ++itr;
                            if (argit != argite)
                                ++argit;
                        }
                        l += lenList[i][k + lk];
                        r += lenList[j][k + rk];
                    }
                    for (k = 0; k < argCount + bothCast; k++)
                    {
                        if (arr[k] > 0)
                            right++;
                        else if (arr[k] < 0)
                            left++;
                    }
                    if (left && !right)
                    {
                        spList[j] = nullptr;
                    }
                    else if (right && !left)
                    {
                        spList[i] = nullptr;
                    }
                    else if (spList[i] && spList[j])
                    {
                        if (spList[i]->sb->castoperator)
                        {
                            if (!spList[j]->sb->castoperator)
                                spList[j] = nullptr;
                        }
                        else
                        {
                            if (spList[j]->sb->castoperator)
                                spList[i] = nullptr;
                        }
                        if (spList[i] && spList[j])
                        {
                            switch (ChooseLessConstTemplate(spList[i], spList[j]))
                            {
                            case -1:
                                spList[j] = nullptr;
                                break;
                            case 1:
                                spList[i] = nullptr;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    for (i = 0, j = 0; i < funcCount; i++)
    {
        if (spList[i])
            j++;
    }
    if (j > 1)
    {
        int ellipseCount = 0, unellipseCount = 0;
        for (i = 0, j = 0; i < funcCount; i++)
        {
            if (spList[i])
            {
                if (ellipsed(spList[i]))
                    ellipseCount++;
                else
                    unellipseCount++;
            }
        }
        if (unellipseCount && ellipseCount)
        {
            for (i = 0, j = 0; i < funcCount; i++)
            {
                if (spList[i] && ellipsed(spList[i]))
                    spList[i] = 0;
            }
        }
        for (i = 0, j = 0; i < funcCount; i++)
        {
            if (spList[i])
                j++;
        }
        if (j > 1)
        {
            if (funcparams->arguments)
            {
                int* match = (int*)alloca(sizeof(int) * 500);
                bool found = false;
                for (auto arg : *funcparams->arguments)
                {
                    if (isarithmetic(arg->tp))
                    {
                        found = true;
                        break;
                    }           
                }
                if (found)
                {
                    for (int i = 0; i < funcCount; i++)
                    {
                        match[i] = INT_MIN;
                        if (spList[i] && !spList[i]->sb->templateLevel)
                        {
                            auto it = basetype(spList[i]->tp)->syms->begin();
                            auto ite = basetype(spList[i]->tp)->syms->end();
                            if ((*it)->sb->thisPtr)
                                ++it;
                            int n = 0;
                            found = false;
                            for (auto arg : *funcparams->arguments)
                            {
                                if (it == ite)
                                {       
                                    found = true;
                                    break;
                                }
                                TYPE* target = (*it)->tp;
                                TYPE* current = arg->tp;
                                if (!current) // initlist, don't finish this screening
                                    return;
                                while (isref(target))
                                    target = basetype(target)->btp;
                                while (isref(current))
                                    current = basetype(current)->btp;
                                if (isarithmetic(target) && isarithmetic(current))
                                {
                                    if (isint(target))
                                    {
                                        if (isfloat(current))
                                            current = &stdint;
                                    }
                                    else if (isfloat(target))
                                    {
                                        if (isint(current))
                                            current = &stddouble;
                                    }
                                    if (basetype(current)->type <= basetype(target)->type)
                                    {
                                        n++;
                                    }
                                    else if (isint(current) && isint(target))
                                    {
                                        if (getSize(basetype(current)->type) == getSize(basetype(target)->type))
                                            n++;
                                    }
                                }
                                else if (!ispointer(current) || !ispointer(target))
                                {
                                    n = INT_MIN;
                                }
                                ++it;
                            }
                            if (!found && (it == ite || (*it)->sb->defaultarg))
                            {
                                match[i] = n;
                            }
                        }
                    }
                    int sum = 0;
                    for (int i = 0; i < funcCount; i++)
                    {
                        if (match[i] > sum)
                        {
                            sum = match[i];
                        }
                    }
                    for (int i = 0; i < funcCount; i++)
                        if (match[i] != sum && match[i] >= 0)
                            spList[i] = nullptr;
                }
            }
        }
    }
}
static void GetMemberCasts(std::list<SYMBOL*>& gather, SYMBOL* sym)
{
    if (sym)
    {
        SYMBOL* find = search(basetype(sym->tp)->syms, overloadNameTab[CI_CAST]);
        if (find)
            gather.insert(gather.begin(), find);
        if (sym->sb->baseClasses)
            for (auto bcl : *sym->sb->baseClasses)
                GetMemberCasts(gather, bcl->cls);
    }
}
static void GetMemberConstructors(std::list<SYMBOL*>& gather, SYMBOL* sym)
{
    SYMBOL* find = search(basetype(sym->tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (find)
    {
        gather.insert(gather.begin(), find);
    }
    if (sym->sb->baseClasses)
        for (auto bcl : *sym->sb->baseClasses)
            GetMemberConstructors(gather, bcl->cls);
}
SYMBOL* getUserConversion(int flags, TYPE* tpp, TYPE* tpa, EXPRESSION* expa, int* n, enum e_cvsrn* seq, SYMBOL* candidate_in,
                                 SYMBOL** userFunc, bool honorExplicit)
{
    if (inGetUserConversion < 1)
    {
        std::list<SYMBOL*> gather;
        TYPE* tppp;
        if (tpp->type == bt_typedef)
            tpp = tpp->btp;
        tppp = tpp;
        if (isref(tppp))
            tppp = basetype(tppp)->btp;
        inGetUserConversion++;
        if (flags & F_WITHCONS)
        {
            if (isstructured(tppp))
            {
                SYMBOL* sym = basetype(tppp)->sp;
                sym->tp = PerformDeferredInitialization(sym->tp, nullptr);
                /*
                if (sym->sb->templateLevel && !templateNestingCount && !sym->sb->instantiated &&
                    allTemplateArgsSpecified(sym, sym->templateParams))
                {
                    sym = TemplateClassInstantiate(sym, sym->templateParams, false, sc_global);
                }
                */
                GetMemberConstructors(gather, sym);
                tppp = sym->tp;
            }
        }
        GetMemberCasts(gather, basetype(tpa)->sp);
        if (gather.size())
        {
            int funcs = 0;
            int i;
            SYMBOL **spList;
            enum e_cvsrn** icsList;
            int** lenList;
            int m = 0;
            SYMBOL *found1, *found2;
            FUNCTIONCALL funcparams;
            std::list<INITLIST*> args;
            TYPE thistp;
            EXPRESSION exp;
            memset(&funcparams, 0, sizeof(funcparams));
            memset(&thistp, 0, sizeof(thistp));
            memset(&exp, 0, sizeof(exp));
            funcparams.arguments = &args;
            INITLIST arg0 = {};
            args.push_back(&arg0);
            arg0.tp = tpa;
            arg0.exp = &exp;
            exp.type = en_c_i;
            funcparams.ascall = true;
            funcparams.thisptr = expa;
            funcparams.thistp = &thistp;
            MakeType(thistp, bt_pointer, tpp);
            for (auto sp : gather)
            {
                funcs += sp->tp->syms->size();
            }
            spList = (SYMBOL **)alloca(funcs * sizeof(SYMBOL *));
            icsList = (e_cvsrn**)alloca(funcs * sizeof(e_cvsrn*));
            lenList = (int **)alloca(funcs * sizeof(int*));
            i = 0;
            std::set<SYMBOL*> filters;
            for (auto sp : gather)
            {
                for (auto sym : *sp->tp->syms)
                {
                    if (!sym->sb->instantiated && filters.find(sym) == filters.end() && filters.find(sym->sb->mainsym) == filters.end())
                    {
                        filters.insert(sym);
                        if (sym->sb->mainsym)
                            filters.insert(sym->sb->mainsym);
                        if (sym->sb->templateLevel && sym->templateParams)
                        {
                            if (sym->sb->castoperator)
                            {
                                spList[i++] = detemplate(sym, nullptr, tppp);
                            }
                            else
                            {
                                spList[i++] = detemplate(sym, &funcparams, nullptr);
                            }
                        }
                        else
                        {
                            spList[i++] = sym;
                        }
                    }
                }
            }
            funcs = i;
            memset(&exp, 0, sizeof(exp));
            exp.type = en_not_lvalue;
            for (i = 0; i < funcs; i++)
            {
                SYMBOL* candidate = spList[i];
                if (candidate)
                {
                    if (honorExplicit && candidate->sb->isExplicit && !(flags & F_CONVERSION))
                    {
                        spList[i] = nullptr;
                    }
                    else
                    {
                        int j;
                        int n3 = 0, n2 = 0, m1;
                        enum e_cvsrn seq3[50];
                        if (candidate->sb->castoperator)
                        {
                            TYPE* tpc = basetype(candidate->tp)->btp;
                            if (tpc->type == bt_typedef)
                                tpc = tpc->btp;
                            if (isref(tpc))
                                tpc = basetype(tpc)->btp;
                            if (tpc->type != bt_auto &&
                                (((flags & F_INTEGER) && !isint(tpc)) ||
                                 ((flags & F_POINTER) && !ispointer(tpc) && basetype(tpc)->type != bt_memberptr) ||
                                 ((flags & F_ARITHMETIC) && !isarithmetic(tpc)) || ((flags & F_STRUCTURE) && !isstructured(tpc))))
                            {
                                seq3[n2++] = CV_NONE;
                                seq3[n2 + n3++] = CV_NONE;
                            }
                            else
                            {
                                bool lref = false;
                                TYPE* tpn = basetype(candidate->tp)->btp;
                                if (tpn->type == bt_typedef)
                                    tpn = tpn->btp;
                                if (isref(tpn))
                                {
                                    if (basetype(tpn)->type == bt_lref)
                                        lref = true;
                                }
                                MakeType(thistp, bt_pointer, tpa);
                                getSingleConversion((*basetype(candidate->tp)->syms->begin())->tp, &thistp, &exp, &n2, seq3, candidate, nullptr, true);
                                seq3[n2 + n3++] = CV_USER;
                                inGetUserConversion--;
                                if (tpc->type == bt_auto)
                                {
                                    seq3[n2 + n3++] = CV_USER;
                                }
                                else if (isfuncptr(tppp))
                                {
                                    int n77 = n3;
                                    getSingleConversion(tppp, basetype(candidate->tp)->btp, lref ? nullptr : &exp, &n3, seq3 + n2,
                                                        candidate, nullptr, true);
                                    if (n77 != n3 - 1 || seq3[n2 + n77] != CV_IDENTITY)
                                    {
                                        SYMBOL* spf = basetype(basetype(tppp)->btp)->sp;
                                        n3 = n77;
                                        if (spf->sb->templateLevel && spf->sb->storage_class == sc_typedef &&
                                            !spf->sb->instantiated)
                                        {
                                            auto ita = spf->templateParams->begin();
                                            ++ita;
                                            auto itae = spf->templateParams->end();
                                            spf = spf->sb->mainsym;
                                            if (spf)
                                            {
                                                TYPE* hold[100];
                                                int count = 0;
                                                for (auto it = ita; it != itae; ++it)
                                                {
                                                    auto&& srch = *it;
                                                    hold[count++] = srch.second->byClass.dflt;
                                                    srch.second->byClass.dflt = srch.second->byClass.val;
                                                }
                                                spf = GetTypeAliasSpecialization(spf, spf->templateParams);
                                                spf->tp = SynthesizeType(spf->tp, nullptr, false);
                                                getSingleConversion(spf->tp, basetype(candidate->tp)->btp, lref ? nullptr : &exp,
                                                                    &n3, seq3 + n2, candidate, nullptr, true);
                                                count = 0;
                                                for (auto it = ita; it != itae; ++it)
                                                {
                                                    auto&& srch = *it;
                                                    srch.second->byClass.val = srch.second->byClass.dflt;
                                                    srch.second->byClass.dflt = hold[count++];
                                                }
                                            }
                                            else
                                            {
                                                seq3[n2 + n3++] = CV_NONE;
                                            }
                                        }
                                        else
                                        {
                                            getSingleConversion(tppp, basetype(candidate->tp)->btp, lref ? nullptr : &exp, &n3,
                                                                seq3 + n2, candidate, nullptr, true);
                                        }
                                    }
                                }
                                else if (!comparetypes(basetype(candidate->tp)->btp, tpa, true) && !sameTemplate(basetype(candidate->tp)->btp, tpa))
                                {
                                    if (isvoidptr(tppp))
                                    {
                                        if (isvoidptr(basetype(candidate->tp)->btp))
                                            seq3[n3++ + n2] = CV_IDENTITY;
                                        else 
                                            seq3[n3++ + n2] = CV_POINTERCONVERSION;
                                    }
                                    else
                                    {
                                        getSingleConversion(tppp, basetype(candidate->tp)->btp, lref ? nullptr : &exp, &n3,
                                                            seq3 + n2,
                                                        candidate, nullptr, false);
                                    }
                                }
                                inGetUserConversion++;
                            }
                        }
                        else
                        {
                            auto sparg = basetype(candidate->tp)->syms->begin();
                            auto spend = basetype(candidate->tp)->syms->end();
                            if (sparg != spend)
                            {
                                if (candidate_in && candidate_in->sb->isConstructor &&
                                    candidate_in->sb->parentClass == candidate->sb->parentClass)
                                {
                                    seq3[n2++] = CV_NONE;
                                }
                                else
                                {
                                    auto th = *sparg;
                                    ++sparg;
                                    SYMBOL* first = *sparg;
                                    ++sparg;
                                    SYMBOL* next = sparg == spend ? nullptr : *sparg;
                                    if (!next || next->sb->init || next->sb->deferredCompile)
                                    {
                                        if (first->tp->type != bt_ellipse)
                                        {
                                            getSingleConversion(first->tp, tpa, expa, &n2, seq3, candidate, nullptr, true);
                                            if (n2 && seq3[n2 - 1] == CV_IDENTITY)
                                            {
                                                n2--;
                                            }
                                        }
                                        seq3[n2 + n3++] = CV_USER;
                                        getSingleConversion(tppp, basetype(basetype(th->tp)->btp)->sp->tp, &exp, &n3, seq3 + n2,
                                                            candidate, nullptr, true);
                                    }
                                    else
                                    {
                                        seq3[n2++] = CV_NONE;
                                    }
                                }
                            }
                        }
                        for (j = 0; j < n2 + n3; j++)
                            if (seq3[j] == CV_NONE)
                                break;
                        m1 = n2 + n3;
                        while (m1 && seq3[m1 - 1] == CV_IDENTITY)
                            m1--;
                        if (j >= n2 + n3 && m1 <= 7)
                        {
                            lenList[i] = Allocate<int>(2);
                            icsList[i] = Allocate<e_cvsrn>(n2 + n3);
                            lenList[i][0] = n2;
                            lenList[i][1] = n3;
                            memcpy(&icsList[i][0], seq3, (n2 + n3) * sizeof(enum e_cvsrn));
                        }
                        else
                        {
                            spList[i] = nullptr;
                        }
                    }
                }
            }
            SelectBestFunc(spList, icsList, lenList, &funcparams, 2, funcs, nullptr);
            WeedTemplates(spList, funcs, &funcparams, nullptr);
            found1 = found2 = nullptr;

            for (i = 0; i < funcs && !found1; i++)
            {
                int j;
                found1 = spList[i];
                m = i;
                for (j = i + 1; j < funcs && found1 && !found2; j++)
                {
                    if (spList[j])
                    {
                        found2 = spList[j];
                    }
                }
            }
            if (found1)
            {
                if (!found2)
                {
                    if (honorExplicit && found1->sb->isExplicit)
                    {
                        error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
                    }
                    if (seq)
                    {
                        int l = lenList[m][0] + (found1->sb->castoperator ? lenList[m][1] : 1);
                        memcpy(&seq[*n], &icsList[m][0], l * sizeof(enum e_cvsrn));
                        *n += l;
                        if (userFunc)
                            *userFunc = found1;
                    }
                    inGetUserConversion--;
                    if (flags & F_CONVERSION)
                    {
                        if (found1->sb->templateLevel && !templateNestingCount && found1->templateParams)
                        {
                            if (!inSearchingFunctions || inTemplateArgs)
                                found1 = TemplateFunctionInstantiate(found1, false);
                        }
                    }
                    return found1;
                }
            }
        }
        inGetUserConversion--;
    }
    if (seq)
        seq[(*n)++] = CV_NONE;
    return nullptr;
}
static void getQualConversion(TYPE* tpp, TYPE* tpa, EXPRESSION* exp, int* n, enum e_cvsrn* seq)
{
    bool hasconst = true, hasvol = true;
    bool sameconst = true, samevol = true;
    bool first = true;
    while (exp && castvalue(exp))
        exp = exp->left;
    bool strconst = false;
    while (tpa && tpp)  // && ispointer(tpa) && ispointer(tpp))
    {
        strconst = exp && exp->type == en_labcon && basetype(tpa)->type == bt_char;
        if (isconst(tpp) != isconst(tpa))
        {
            sameconst = false;
            if (isconst(tpa) && !isconst(tpp))
                break;
            if (!hasconst)
                break;
        }
        if (isvolatile(tpp) != isvolatile(tpa))
        {
            samevol = false;
            if (isvolatile(tpa) && !isvolatile(tpp))
                break;
            if (!hasvol)
                break;
        }

        if (!first)
        {
            if (!isconst(tpp))
                hasconst = false;
            if (!isvolatile(tpp))
                hasvol = false;
        }
        first = false;
        if (tpa->type == bt_enum)
            tpa = tpa->btp;
        if (isarray(tpa))
            while (isarray(tpa))
                tpa = basetype(tpa)->btp;
        else
            tpa = basetype(tpa)->btp;
        if (tpp->type == bt_enum)
            tpp = tpp->btp;
        if (isarray(tpp))
            while (isarray(tpp))
                tpp = basetype(tpp)->btp;
        else
            tpp = basetype(tpp)->btp;
    }
    if ((!tpa && !tpp) || (tpa && tpp && tpa->type != bt_pointer && tpp->type != bt_pointer))
    {
        if (tpa && tpp && ((hasconst && isconst(tpa) && !isconst(tpp)) || (hasvol && isvolatile(tpa) && !isvolatile(tpp))))
            seq[(*n)++] = CV_NONE;
        else if (!sameconst || !samevol)
            seq[(*n)++] = CV_QUALS;
        else if (strconst && !isconst(tpp))
            seq[(*n)++] = CV_QUALS;
        else
            seq[(*n)++] = CV_IDENTITY;
    }
    else
    {
        seq[(*n)++] = CV_NONE;
    }
}
static void getPointerConversion(TYPE* tpp, TYPE* tpa, EXPRESSION* exp, int* n, enum e_cvsrn* seq)
{
    if (basetype(tpa)->btp->type == bt_void && exp && (isconstzero(&stdint, exp) || exp->type == en_nullptr))
    {
        seq[(*n)++] = CV_POINTERCONVERSION;
        return;
    }
    else
    {
        if (basetype(tpa)->array)
            seq[(*n)++] = CV_ARRAYTOPOINTER;
        if (isfunction(basetype(tpa)->btp))
            seq[(*n)++] = CV_FUNCTIONTOPOINTER;
        if (basetype(basetype(tpp)->btp)->type == bt_void)
        {
            if (basetype(basetype(tpa)->btp)->type != bt_void)
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            if (ispointer(basetype(tpa)->btp))
            {
                if ((isconst(tpa) && !isconst(tpp)) || (isvolatile(tpa) && !isvolatile(tpp)))
                    seq[(*n)++] = CV_NONE;
                else if ((isconst(tpp) != isconst(tpa)) || (isvolatile(tpa) != isvolatile(tpp)))
                    seq[(*n)++] = CV_QUALS;
                return;
            }
        }
        else if (isstructured(basetype(tpp)->btp) && isstructured(basetype(tpa)->btp))
        {
            SYMBOL* base = basetype(basetype(tpp)->btp)->sp;
            SYMBOL* derived = basetype(basetype(tpa)->btp)->sp;

            if (base != derived && !comparetypes(base->tp, derived->tp, true) && !sameTemplate(base->tp, derived->tp))
            {
                int v = classRefCount(base, derived);
                if (v != 1)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    seq[(*n)++] = CV_DERIVEDFROMBASE;
                }
            }
        }
        else
        {
            TYPE* t1 = tpp;
            TYPE* t2 = tpa;
            if (isarray(t2) && ispointer(t1))
            {
                while (isarray(t2))
                    t2 = basetype(t2)->btp;
                if (isarray(t1))
                    while (isarray(t1))
                        t1 = basetype(t1)->btp;
                else
                    t1 = basetype(t1)->btp;
            }
            if (basetype(tpa)->nullptrType)
            {
                if (!basetype(tpp)->nullptrType)
                {
                    if (ispointer(tpa))
                        seq[(*n)++] = CV_POINTERCONVERSION;
                    else if (!basetype(tpp)->nullptrType && !isconstzero(basetype(tpa), exp) && exp->type != en_nullptr)
                        seq[(*n)++] = CV_NONE;
                }
            }
            else if (!comparetypes(t1, t2, true))
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        getQualConversion(tpp, tpa, exp, n, seq);
    }
}
bool sameTemplateSelector(TYPE* tnew, TYPE* told)
{
    while (isref(tnew) && isref(told))
    {
        tnew = basetype(tnew)->btp;
        told = basetype(told)->btp;
    }
    while (ispointer(tnew) && ispointer(told))
    {
        tnew = basetype(tnew)->btp;
        told = basetype(told)->btp;
    }
    if (tnew->type == bt_templateselector && told->type == bt_templateselector)
    {
        auto tsn = tnew->sp->sb->templateSelector->begin();
        auto tsne = tnew->sp->sb->templateSelector->end();
        auto tso = told->sp->sb->templateSelector->begin();
        auto tsoe = told->sp->sb->templateSelector->end();
        ++tsn;
        ++tso;
        // this is kinda loose, ideally we ought to go through template parameters/decltype expressions
        // looking for equality...  
        if (tsn->isTemplate || tso->isTemplate)
            return false;
        if (tsn->isDeclType || tso->isDeclType)
            return false;
        for ( ++ tsn, ++ tso; tsn != tsne && tso != tsoe; ++tsn, ++tso)
        {
            if (strcmp(tsn->name, tso->name) != 0)
                return false;
        }
        return tsn == tsne && tso == tsoe;
    }
    else if (tnew->type == bt_templateselector || told->type == bt_templateselector)
    {
        auto y = tnew->type == bt_templateselector ? told : tnew;
        if (!isstructured(y))
            return false;
        auto& x = tnew->type == bt_templateselector ? tnew->sp->sb->templateSelector : told->sp->sb->templateSelector;
        auto ts = x->begin();
        auto tse = x->end();
        ++ts;
        if (ts->isDeclType)
            return false;
        auto tp = ts->sp->tp;
        for (++ts; ts != tse; ++ts)
        {
            if (!isstructured(tp))
                return false;

            auto sp = search(basetype(tp)->syms, ts->name);
            if (!sp)
            {
                sp = classdata(ts->name, basetype(tp)->sp, nullptr, false, false);
                if (sp == (SYMBOL*)-1 || sp == nullptr)
                    return false;
            }
            if (sp->sb->access != ac_public && !resolvingStructDeclarations)
            {
                return false;
            }
            tp = sp->tp;
        }
        return comparetypes(tp, y, true) || sameTemplate(tp, y);
    }
    return false;
}
bool sameTemplatePointedTo(TYPE* tnew, TYPE* told, bool quals)
{
    if (isconst(tnew) != isconst(told) || isvolatile(tnew) != isvolatile(told))
        return false;
    while (basetype(tnew)->type == basetype(told)->type && basetype(tnew)->type == bt_pointer)
    {
        tnew = basetype(tnew)->btp;
        told = basetype(told)->btp;
        if (isconst(tnew) != isconst(told) || isvolatile(tnew) != isvolatile(told))
            return false;
    }
    return sameTemplate(tnew, told, quals);
}
bool sameTemplate(TYPE* P, TYPE* A, bool quals)
{
    bool PLd, PAd;
    std::list<TEMPLATEPARAMPAIR>::iterator PL, PLE, PA, PAE;
    if (!P || !A)
        return false;
    P = basetype(P);
    A = basetype(A);
    if (isref(P))
        P = basetype(P->btp);
    if (isref(A))
        A = basetype(A->btp);
    if (!isstructured(P) || !isstructured(A))
        return false;
    if (!P->sp->sb || !A->sp->sb || P->sp->sb->parentClass != A->sp->sb->parentClass || strcmp(P->sp->name, A->sp->name) != 0)
        return false;
    if (P->sp->sb->templateLevel != A->sp->sb->templateLevel)
        return false;
    if (!P->sp->templateParams || !A->sp->templateParams)
    {
        if (P->size == 0 && !strcmp(P->sp->sb->decoratedName, A->sp->sb->decoratedName))
            return true;
        return false;
    }
    // this next if stmt is a horrible hack.
    PL = P->sp->templateParams->begin();
    PLE = P->sp->templateParams->end();
    PA = A->sp->templateParams->begin();
    PAE = A->sp->templateParams->end();
    if (P->sp->templateParams->size() == 0 || A->sp->templateParams->size() == 0)
    {
        if (P->sp->templateParams->size() == 0 && !strcmp(P->sp->sb->decoratedName, A->sp->sb->decoratedName))
            return true;
        return false;
    }
    PLd = PAd = false;
    if (PL != PLE)
    {
        if (PL->second->bySpecialization.types)
        {
            PLE = PL->second->bySpecialization.types->end();
            PL = PL->second->bySpecialization.types->begin();
            PLd = true;
        }
        else
        {
            ++PL;
        }
    }
    if (PA != PAE)
    {
        if (PA->second->bySpecialization.types)
        {
            PAE = PA->second->bySpecialization.types->end();
            PA = PA->second->bySpecialization.types->begin();
            PAd = true;
        }
        else
        {
            ++PA;
        }
    }
    if (PL != PLE && PA != PAE)
    {
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> pls;
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> pas;
        while (PL != PLE && PA != PAE)
        {
            if (PL->second->packed != PA->second->packed)
                break;

            while (PL != PLE && PA != PAE && PL->second->packed)
            {
                pls.push(PL);
                pls.push(PLE);
                pas.push(PA);
                pas.push(PAE);
                if (PL->second->byPack.pack)
                {
                    PLE = PL->second->byPack.pack->end();
                    PL = PL->second->byPack.pack->begin();
                }
                else
                {
                    PLE = std::list<TEMPLATEPARAMPAIR>::iterator();
                    PL = PLE;
                }
                if (PA->second->byPack.pack)
                {
                    PAE = PA->second->byPack.pack->end();
                    PA = PA->second->byPack.pack->begin();
                }
                else
                {
                    PAE = std::list<TEMPLATEPARAMPAIR>::iterator();
                    PA = PAE;
                }
            }
            if (PL == PLE || PA == PAE)
                break;
            if (PL->second->type != PA->second->type)
            {
                break;
            }
            else if (P->sp->sb->instantiated || A->sp->sb->instantiated || (PL->second->byClass.dflt && PA->second->byClass.dflt))
            {
                if (PL->second->type == kw_typename)
                {
                    TYPE* pl = PL->second->byClass.val /*&& !PL->second->byClass.dflt*/ ? PL->second->byClass.val : PL->second->byClass.dflt;
                    TYPE* pa = PA->second->byClass.val /*&& !PL->second->byClass.dflt*/ ? PA->second->byClass.val : PA->second->byClass.dflt;
                    if (!pl || !pa)
                        break;
                    if ((PAd || PA->second->byClass.val) && (PLd || PL->second->byClass.val) && !templatecomparetypes(pa, pl, true))
                    {
                        break;
                    }
                    // now make sure the qualifiers match...
                    if (quals)
                    {
                        int n = 0;
                        enum e_cvsrn xx[5];
                        getQualConversion(pl, pa, nullptr, &n, xx);
                        if (n != 1 || xx[0] != CV_IDENTITY)
                        {
                            break;
                        }
                    }
                }
                else if (PL->second->type == kw_template)
                {
                    SYMBOL* plt = PL->second->byTemplate.val && !PL->second->byTemplate.dflt ? PL->second->byTemplate.val : PL->second->byTemplate.dflt;
                    SYMBOL* pat = PA->second->byTemplate.val && !PL->second->byTemplate.dflt ? PA->second->byTemplate.val : PA->second->byTemplate.dflt;
                    if ((plt || pat) && !exactMatchOnTemplateParams(PL->second->byTemplate.args, PA->second->byTemplate.args))
                        break;
                }
                else if (PL->second->type == kw_int)
                {
                    EXPRESSION* plt = PL->second->byNonType.val && !PL->second->byNonType.dflt ? PL->second->byNonType.val : PL->second->byNonType.dflt;
                    EXPRESSION* pat = PA->second->byNonType.val && !PA->second->byNonType.dflt ? PA->second->byNonType.val : PA->second->byNonType.dflt;
                    if (!templatecomparetypes(PL->second->byNonType.tp, PA->second->byNonType.tp, true))
                        break;
                    if ((!plt || !pat) || !equalTemplateIntNode(plt, pat))
                        break;
                }
            }
            ++PL;
            ++PA;
            if (PL == PLE && PA == PAE && !pls.empty() && !pas.empty())
            {
                PLE = pls.top();
                pls.pop();
                PL = pls.top();
                pls.pop();
                PAE = pas.top();
                pas.pop();
                PA = pas.top();
                pas.pop();
                ++PL;
                ++PA;
            }
        }
        return PL == PLE && PA == PAE;
    }
    return false;
}
void GetRefs(TYPE* tpp, TYPE* tpa, EXPRESSION* expa, bool& lref, bool& rref)
{
    bool func = false;
    bool func2 = false;
    bool notlval = false;
    if (tpp)
    {
        TYPE *tpp1 = tpp;
        if (isref(tpp1))
            tpp1 = basetype(tpp1)->btp;
        if (isstructured(tpp1))
        {
            TYPE *tpa1 = tpa;
            if (isref(tpa1))
                tpa1 = basetype(tpa1)->btp;
            if (!isstructured(tpa1))
            {
                lref = false;
                rref = true;
                return;
            }
            else if (classRefCount(basetype(tpp1)->sp, basetype(tpa1)->sp) != 1 && !comparetypes(tpp1, tpa1, true) && !sameTemplate(tpp1, tpa1))
            {
                lref = false;
                rref = true;
                return;
            }
        }
    }
    if (expa)
    {
        if (isstructured(tpa))
        {
            // function call as an argument can result in an rref
            EXPRESSION* expb = expa;
            if (expb->type == en_thisref)
                expb = expb->left;
            if (expb->type == en_func && expb->v.func->sp)
                if (isfunction(expb->v.func->sp->tp))
                {
                    func = expb->v.func->sp->sb->isConstructor || isstructured(basetype(expb->v.func->sp->tp)->btp);
                }
            if (expa->type == en_not_lvalue)
                notlval = true;
        }
        else if (isfunction(tpa) || isfuncptr(tpa))
        {
            EXPRESSION* expb = expa;
            if (expb->type == en_thisref)
               expb = expb->left;
            if (expb->type == en_func)
                func2 = !expb->v.func->ascall;
            else if (expb->type == en_pc)
                func2 = true;
		func2 = false;
        }
    }
    lref = (basetype(tpa)->type == bt_lref || tpa->lref || (isstructured(tpa) && !notlval && !func) || (expa && lvalue(expa))) &&
           !tpa->rref;
    rref = (basetype(tpa)->type == bt_rref || tpa->rref || notlval || func || func2 ||
            (expa && (isarithmeticconst(expa) || !lvalue(expa) && !ismem(expa) && !ismath(expa) && !castvalue(expa)))) &&
           !lref && !tpa->lref;
}
void getSingleConversionWrapped(TYPE* tpp, TYPE* tpa, EXPRESSION* expa, int* n, enum e_cvsrn* seq, SYMBOL* candidate,
                                SYMBOL** userFunc, bool ref, bool allowUser)
{
    int rref = tpa->rref;
    int lref = tpa->lref;
    tpa->rref = false;
    tpa->lref = false;
    getSingleConversion(tpp, tpa, expa, n, seq, candidate, userFunc, allowUser, ref);
    tpa->rref = rref;
    tpa->lref = lref;
}
void getSingleConversion(TYPE* tpp, TYPE* tpa, EXPRESSION* expa, int* n, enum e_cvsrn* seq, SYMBOL* candidate, SYMBOL** userFunc,
                         bool allowUser, bool ref)
{
    bool lref = false;
    bool rref = false;
    EXPRESSION* exp = expa;
    TYPE* tpax = tpa;
    TYPE* tppx = tpp;
    if (isarray(tpax))
        tpax = basetype(tpax);
    tpa = basetype(tpa);
    tpp = basetype(tpp);
    // when evaluating decltype we sometimes come up with these
    if (tpa->type == bt_templateparam)
        tpa = tpa->templateParam->second->byClass.val;
    if (!tpa)
    {
        seq[(*n)++] = CV_NONE;
        return;
    }
    while (expa && expa->type == en_void)
        expa = expa->right;
    if (tpp->type != tpa->type && (tpp->type == bt_void || tpa->type == bt_void))
    {
        seq[(*n)++] = CV_NONE;
        return;
    }
    GetRefs(tpp, tpa, exp, lref, rref);
    if (exp && exp->type == en_thisref)
        exp = exp->left;
    if (exp && exp->type == en_func)
    {
        if (basetype(exp->v.func->sp->tp)->type != bt_aggregate)
        {
            TYPE* tp = basetype(basetype(exp->v.func->functp)->btp);
            if (tp)
            {
                if (tp->type == bt_rref)
                {
                    if (!tpa->lref)
                    {
                        rref = true;
                        lref = false;
                    }
                }
                else if (tp->type == bt_lref)
                {
                    if (!tpa->rref)
                    {
                        lref = true;
                        rref = false;
                    }
                }
            }
        }
    }
    if (isref(tpa))
    {
        if (basetype(tpa)->type == bt_rref)
        {
            rref = true;
            lref = false;
        }
        else if (basetype(tpa)->type == bt_lref)
        {
            lref = true;
            rref = false;
        }
        tpa = basetype(tpa)->btp;
        while (isref(tpa))
            tpa = basetype(tpa)->btp;
    }
    if (isref(tpp))
    {
        TYPE* tppp = basetype(tpp)->btp;
        while (isref(tppp))
            tppp = basetype(tppp)->btp;
        if (!rref && expa && isstructured(tppp) && expa->type != en_not_lvalue)
        {
            EXPRESSION* expx = expa;
            if (expx->type == en_thisref)
                expx = expx->left;
            if (expx->type == en_func)
            {
                if (expx->v.func->returnSP)
                {
                    if (!expx->v.func->returnSP->sb->anonymous)
                        lref = true;
                }
            }
            else
            {
                lref = true;
            }
        }
        if (isref(tpax))
        {
            if ((isconst(tpa) != isconst(tppp)) || (isvolatile(tpa) != isvolatile(tppp)))
            {
                seq[(*n)++] = CV_QUALS;
            }
        }
        else
        {
            if (isconst(tpax) != isconst(tppp))
            {
                if (!isconst(tppp) || !rref)
                    seq[(*n)++] = CV_QUALS;
            }
            else if (isvolatile(tpax) != isvolatile(tppp))
            {
                seq[(*n)++] = CV_QUALS;
            }
        }
        if (((isconst(tpa) || isconst(tpax)) && !isconst(tppp)) ||
            ((isvolatile(tpa) || isvolatile(tpax)) && !isvolatile(tppp) && !isconst(tppp)))
        {
            if (tpp->type != bt_rref)
                seq[(*n)++] = CV_NONE;
        }
        if (lref && !rref && tpp->type == bt_rref)
            seq[(*n)++] = CV_LVALUETORVALUE;
        if (tpp->type == bt_rref && lref && !isfunction(tpa) && !isfuncptr(tpa) && !ispointer(tpa) &&
            (expa && !isarithmeticconst(expa)))
        {
            // lvalue to rvalue ref not allowed unless the lvalue is nonvolatile and const
            if (!isDerivedFromTemplate(tppx) && (!isconst(tpax) || isvolatile(tpax)))
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (tpp->type == bt_lref && rref && !lref)
        {
            // rvalue to lvalue reference not allowed unless the lvalue is a function or const
            if (!isfunction(basetype(tpp)->btp) && basetype(tpp)->btp->type != bt_aggregate)
            {
                if (!isconst(tppp))
                    seq[(*n)++] = CV_LVALUETORVALUE;
            }
            if (isconst(tppp) && !isvolatile(tppp) && !rref)
                seq[(*n)++] = CV_QUALS;
        }
        tpa = basetype(tpa);
        if (isstructured(tpa))
        {
            if (isstructured(tppp))
            {
                SYMBOL* s1 = basetype(tpa)->sp;
                SYMBOL* s2 = basetype(tppp)->sp;
                if (s1->sb->mainsym)
                    s1 = s1->sb->mainsym;
                if (s2->sb->mainsym)
                    s2 = s2->sb->mainsym;
                if (s1 != s2 && !sameTemplate(tppp, tpa))
                {
                    if (classRefCount(s2, s1) == 1)
                    {
                        seq[(*n)++] = CV_DERIVEDFROMBASE;
                    }
                    else if (s2->sb->trivialCons)
                    {
                        seq[(*n)++] = CV_NONE;
                    }
                    else
                    {
                        if (allowUser)
                            getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                        else
                            seq[(*n)++] = CV_NONE;
                    }
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                if (allowUser)
                    getUserConversion(0, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                else
                    seq[(*n)++] = CV_NONE;
            }
        }
        else if (isstructured(tppp))
        {
            if (allowUser)
                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (isfuncptr(tppp))
        {
            tpp = basetype(tppp)->btp;
            if (isfuncptr(tpa))
                tpa = basetype(tpa)->btp;
            if (comparetypes(tpp, tpa, true))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else
        {
            if (allowUser)
            {
                getSingleConversionWrapped(tppp, tpa, expa, n, seq, candidate, userFunc,
                                           !isconst(tppp) && (basetype(tpp)->type == bt_lref || !isarithmetic(tppp)), allowUser);
            }
            else
                seq[(*n)++] = CV_NONE;
        }
    }
    else
    {
        if ((isconst(tpax) != isconst(tppx)) || (isvolatile(tpax) != isvolatile(tppx)))
            seq[(*n)++] = CV_QUALS;
        if (basetype(tpp)->type == bt___string)
        {
            if (basetype(tpa)->type == bt___string || (expa && expa->type == en_labcon && expa->string))
                seq[(*n)++] = CV_IDENTITY;
            else
                seq[(*n)++] = CV_POINTERCONVERSION;
        }
        else if (basetype(tpp)->type == bt___object)
        {
            if (basetype(tpa)->type == bt___object)
                seq[(*n)++] = CV_IDENTITY;
            else
                seq[(*n)++] = CV_POINTERCONVERSION;
        }
        else if (ispointer(tpp) && basetype(tpp)->nullptrType)
        {
            if ((ispointer(tpa) && basetype(tpa)->nullptrType) || (expa && isconstzero(tpa, expa)))
            {
                if (basetype(tpa)->type == bt_bool)
                    seq[(*n)++] = CV_BOOLCONVERSION;
                else
                    seq[(*n)++] = CV_IDENTITY;
            }
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (isstructured(tpa))
        {
            if (isstructured(tpp))
            {
                if (basetype(tpa)->sp == basetype(tpp)->sp || sameTemplate(tpp, tpa))
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
                else if (classRefCount(basetype(tpp)->sp, basetype(tpa)->sp) == 1)
                {
                    seq[(*n)++] = CV_DERIVEDFROMBASE;
                }
                else if (basetype(tpp)->sp->sb->trivialCons)
                {
                    if (!candidate->sb->isConstructor && !candidate->sb->castoperator && !candidate->sb->isAssign && lookupSpecificCast(basetype(tpa)->sp, tpp))
                        getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                    else
                        seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if (allowUser)
                        getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                    else
                        seq[(*n)++] = CV_NONE;
                }
            }
            else
            {
                if (allowUser)
                    getUserConversion(0, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                else
                    seq[(*n)++] = CV_NONE;
            }
        }
        else if ((Optimizer::architecture == ARCHITECTURE_MSIL) && isstructured(tpp))
        {
            if (basetype(tpa)->nullptrType || (expa && isconstzero(tpa, expa)))
                seq[(*n)++] = CV_POINTERCONVERSION;
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (isarray(tpp) && basetype(tpp)->msil)
        {
            if (basetype(tpa)->nullptrType || (expa && isconstzero(tpa, expa)))
                seq[(*n)++] = CV_POINTERCONVERSION;
            else if (isarray(tpa) && basetype(tpa)->msil)
                getSingleConversionWrapped(basetype(tpp)->btp, basetype(tpa)->btp, nullptr, n, seq, candidate, userFunc, false, allowUser);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (isstructured(tpp))
        {
            if (allowUser)
                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
            else
                seq[(*n)++] = CV_NONE;
        }
        else if (isfuncptr(tpp))
        {
            TYPE* rv;
            tpp = basetype(tpp)->btp;
            rv = basetype(tpp)->btp;
            if (isfuncptr(tpa))
            {
                tpa = basetype(tpa)->btp;
                if (rv->type == bt_auto)
                    basetype(tpp)->btp = basetype(tpa)->btp;
            }
            if (comparetypes(tpp, tpa, true))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if ((isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr)) ||
                     (tpa->type == bt_pointer && tpa->nullptrType))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
            basetype(tpp)->btp = rv;
        }
        else if (basetype(tpp)->nullptrType)
        {
            if (basetype(tpa)->nullptrType || (ispointer(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr)))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (ispointer(tpp))
        {
            if (ispointer(tpa))
            {
                if (isvoidptr(tpp))
                {
                    if (isvoidptr(tpa))
                        seq[(*n)++] = CV_IDENTITY;
                    else
                        seq[(*n)++] = CV_POINTERCONVERSION;
                }
                else
                {
                    // cvqual
                    getPointerConversion(tpp, tpa, expa, n, seq);
                }
            }
            else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else if (isvoidptr(tpp) && (isfunction(tpa) || tpa->type == bt_aggregate))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (basetype(tpp)->type == bt_memberptr)
        {
            if (basetype(tpa)->type == bt_memberptr)
            {
                if (comparetypes(basetype(tpp)->btp, basetype(tpa)->btp, true))
                {
                    if (basetype(tpa)->sp != basetype(tpp)->sp)
                    {
                        if (classRefCount(basetype(tpa)->sp, basetype(tpp)->sp) == 1)
                        {
                            seq[(*n)++] = CV_POINTERTOMEMBERCONVERSION;
                        }
                        else
                        {
                            if (allowUser)
                                getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                            else
                                seq[(*n)++] = CV_NONE;
                        }
                    }
                    else
                    {
                        seq[(*n)++] = CV_IDENTITY;
                    }
                }
                else if (isint(tpa) && expa && (isconstzero(tpa, expa) || expa->type == en_nullptr))
                {
                    seq[(*n)++] = CV_POINTERCONVERSION;
                }
                else
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
            else if (expa && ((isconstzero(tpa, expa) || expa->type == en_nullptr)))
            {
                seq[(*n)++] = CV_POINTERCONVERSION;
            }
            else if (isfunction(tpa))
            {
                if (!comparetypes(basetype(tpp)->btp, tpa, true))
                    seq[(*n)++] = CV_NONE;

                else if (basetype(tpa)->sp->sb->parentClass != basetype(tpp)->sp &&
                         basetype(tpa)->sp->sb->parentClass->sb->mainsym != tpp->sp &&
                         basetype(tpa)->sp->sb->parentClass != basetype(tpp)->sp->sb->mainsym)
                {
                    if (classRefCount(basetype(tpa)->sp->sb->parentClass, basetype(tpp)->sp) == 1)
                    {
                        seq[(*n)++] = CV_POINTERTOMEMBERCONVERSION;
                    }
                    else
                    {
                        if (allowUser)
                            getUserConversion(F_WITHCONS, tpp, tpa, expa, n, seq, candidate, userFunc, true);
                        else
                            seq[(*n)++] = CV_NONE;
                    }
                }
                else
                {
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (isfunction(tpa))
        {
            if (isfunction(tpp) && comparetypes(tpp, tpa, true))
            {
                seq[(*n)++] = CV_IDENTITY;
            }
            else if (basetype(tpp)->type == bt_bool)
            {
                seq[(*n)++] = CV_BOOLCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (ispointer(tpa))
        {
            if (basetype(tpp)->type == bt_bool)
            {
                seq[(*n)++] = CV_BOOLCONVERSION;
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else if (basetype(tpa)->type == bt_memberptr)
        {
            seq[(*n)++] = CV_NONE;
        }
        else if (basetype(tpa)->type == bt_enum)
        {
            if (basetype(tpp)->type == bt_enum)
            {
                if (basetype(tpa)->sp != basetype(tpp)->sp)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if ((isconst(tpax) != isconst(tppx)) || (isvolatile(tpax) != isvolatile(tppx)))
                        seq[(*n)++] = CV_QUALS;
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else
            {
                if (isint(tpp) && !basetype(tpa)->scoped)
                {
                    if (basetype(tpp)->type == basetype(tpa)->btp->type)
                        seq[(*n)++] = CV_INTEGRALCONVERSIONWEAK;
                    else
                        seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                }
                else
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
        else if (basetype(tpp)->type == bt_enum)
        {
            if (tpa->enumConst && tpa->btp)
            {
                tpa = tpa->btp;
                if (basetype(tpa)->sp != basetype(tpp)->sp)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if ((isconst(tpax) != isconst(tppx)) || (isvolatile(tpax) != isvolatile(tppx)))
                        seq[(*n)++] = CV_QUALS;
                    seq[(*n)++] = CV_IDENTITY;
                }
            }
            else if (isint(tpa))
            {
                if (tpa->enumConst)
                {
                    if (tpa->sp == basetype(tpp)->sp)
                        seq[(*n)++] = CV_IDENTITY;
                    else
                        seq[(*n)++] = CV_NONE;
                }
                else
                {
                    if (tpp->scoped)
                    {
                        seq[(*n)++] = CV_NONE;

                    }
                    else
                    {
                        seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                    }
                }
            }
            else
            {
                seq[(*n)++] = CV_NONE;
            }
        }
        else
        {
            bool isenumconst = false;
//            if ((isconst(tpax) != isconst(tppx)) || (isvolatile(tpax) != isvolatile(tppx)))
//                seq[(*n)++] = CV_QUALS;
            if (tpa->enumConst)
            {
                seq[(*n)++] = CV_ENUMINTEGRALCONVERSION;
                isenumconst = true;
            }
            if (basetype(tpp)->type != basetype(tpa)->type)
            {
                if (ref)
                {
                    seq[(*n)++] = CV_NONE;
                }
                else if (isint(tpa))
                    if (basetype(tpp)->type == bt_bool)
                    {
                        seq[(*n)++] = CV_BOOLCONVERSION;
                    }
                    // take char of converting wchar_t to char
                    else if (basetype(tpa)->type == bt_wchar_t && basetype(tpp)->type == bt_char)
                    {
                        seq[(*n)++] = CV_IDENTITY;
                    }
                    else if ((basetype(tpp)->type == bt_int || basetype(tpp)->type == bt_unsigned) &&
                             basetype(tpa)->type < basetype(tpp)->type)
                    {
                        seq[(*n)++] = CV_INTEGRALPROMOTION;
                    }
                    else if (isint(tpp))
                    {
                        // this next along with a change in the ranking takes care of the case where
                        // long is effectively the same as int on some architectures.   It prefers a mapping between the
                        // two to a mapping between other integer types...
                        if (basetype(tpa)->type == bt_bool || isunsigned(tpa) != isunsigned(tpp) ||
                            getSize(basetype(tpa)->type) != getSize(basetype(tpp)->type))
                            // take char of converting wchar_t to char
                            seq[(*n)++] = CV_INTEGRALCONVERSION;
                        else
                            seq[(*n)++] = CV_INTEGRALCONVERSIONWEAK;
                    }
                    else
                    {
                        seq[(*n)++] = CV_FLOATINGCONVERSION;
                        if (basetype(tpp)->type == bt_float)
                            seq[(*n)++] = CV_FLOATINGCONVERSION;
                        else if (basetype(tpp)->type == bt_long_double)
                            seq[(*n)++] = CV_FLOATINGPROMOTION;
	                    }

                else /* floating */
                    if (basetype(tpp)->type == bt_bool)
                        seq[(*n)++] = CV_BOOLCONVERSION;
                    else if (isint(tpp))
                        seq[(*n)++] = CV_FLOATINGINTEGRALCONVERSION;
                    else if (isfloat(tpp))
                    {
                    	if (basetype(tpp)->type == bt_double)
	                  {    
                           if (basetype(tpa)->type == bt_float)
                               seq[(*n)++] = CV_FLOATINGPROMOTION;
                           else
                               seq[(*n)++] = CV_FLOATINGCONVERSION;
                        }
                        else
                        {
                           if (basetype(tpp)->type < basetype(tpa)->type)
                               seq[(*n)++] = CV_FLOATINGCONVERSION;
                           else
                               seq[(*n)++] = CV_FLOATINGPROMOTION;
                        }
                    }
                    else
                        seq[(*n)++] = CV_NONE;
            }
            else if (!isenumconst)
            {
                seq[(*n)++] = CV_IDENTITY;
            }
        }
    }
}
static void getInitListConversion(TYPE* tp, std::list<INITLIST*>* list, TYPE* tpp, int* n, enum e_cvsrn* seq, SYMBOL* candidate,
                                  SYMBOL** userFunc)
{
    if (isstructured(tp) || (isref(tp) && isstructured(basetype(tp)->btp)))
    {
        if (isref(tp))
            tp = basetype(tp)->btp;
        tp = basetype(tp);
        if (tp->sp->sb->trivialCons)
        {
            if (list)
            {
                auto it = list->begin();
                auto ite = list->end();
                if (it != ite && !(*it)->nested)
                {
                    while (it != ite)
                    {
                        if (comparetypes((*it)->tp, tp, 0) || sameTemplate((*it)->tp, tp))
                        {
                           getSingleConversion(tp, (*it)->tp, (*it)->exp, n, seq, candidate, userFunc, true);
                           ++it;
                        }
                        else
                        {
                           bool changed = false;
                           for (auto member : *tp->syms)
                           {
                               if (it == ite)
                                    break;
                               if (ismemberdata(member))
                               {
                                    getSingleConversion(member->tp, (*it)->tp, (*it)->exp, n, seq, candidate, userFunc, true);
                                    if (*n > 10)
                                        break;
                                    ++it;
                                    changed = true;
                               }
                           }
                           if (!changed)
                               break;
                        }
                        if (*n > 10)
                           break;
                    }
                }
                if (it != ite)
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
        else
        {
            SYMBOL* cons = search(basetype(tp)->syms, overloadNameTab[CI_CONSTRUCTOR]);
            if (!cons)
            {
                // should never happen
                seq[(*n)++] = CV_NONE;
            }
            else
            {
                std::deque<EXPRESSION*> hold;
                EXPRESSION exp = {}, *expp = &exp;
                TYPE* ctype = cons->tp;
                TYPE thistp = {};
                FUNCTIONCALL funcparams = {};
                funcparams.arguments = list;
                exp.type = en_c_i;
                MakeType(thistp, bt_pointer, basetype(tp));
                funcparams.thistp = &thistp;
                funcparams.thisptr = &exp;
                funcparams.ascall = true;
                cons = GetOverloadedFunction(&ctype, &expp, cons, &funcparams, nullptr, false, true, _F_SIZEOF);
                if (!cons)
                {
                    seq[(*n)++] = CV_NONE;
                }
            }
        }
    }
    else if (ispointer(tp))
    {
        if (list)
        {
            TYPE* btp = tp;
            int x;
            while (isarray(btp))
                btp = basetype(btp)->btp;
            x = tp->size / btp->size;
            for (auto a : *list )
            {
                getSingleConversion(btp, a->tp, a->exp, n, seq, candidate, userFunc, true);
                if (*n > 10)
                    break;
                if (--x < 0)  // too many items...
                {
                    seq[(*n)++] = CV_NONE;
                    break;
                }
            }
        }
    }
    else if (list)
    {
        for (auto a : *list)
        {
            if (a->nested)
            {
                for (auto b : *a->nested)
                {
                    getSingleConversion(tp, b->tp, b->exp, n, seq, candidate, userFunc, true);
                }
            }
            else
            {
                getSingleConversion(tp, a->tp, a->exp, n, seq, candidate, userFunc, true);
            }
        }
    }
}
static bool getFuncConversions(SYMBOL* sym, FUNCTIONCALL* f, TYPE* atp, SYMBOL* parent, enum e_cvsrn arr[], int* sizes, int count,
                               SYMBOL** userFunc, bool usesInitList)
{
    (void)usesInitList;
    int pos = 0;
    int n = 0;
    int i;
    std::list<INITLIST*> a;
    enum e_cvsrn seq[500];
    TYPE* initializerListType = nullptr;
    int m = 0, m1;
    if (sym->tp->type == bt_any)
        return false;

    auto it = basetype(sym->tp)->syms->begin();
    auto ite = basetype(sym->tp)->syms->end();
    SymbolTable<SYMBOL>::iterator itt;
    if (f)
    {
        if (f->arguments)
            a = *f->arguments;
    }
    else
        itt = atp->syms->begin();
    auto ita = a.begin();
    auto itae = a.end();
    for (i = 0; i < count; i++)
    {
        arr[i] = CV_PAD;
    }
    /* takes care of THIS pointer */
    if (sym->sb->castoperator)
    {
        TYPE tpx;
        TYPE* tpp;
        SYMBOL* argsym = *it;
        memset(&tpx, 0, sizeof(tpx));
        m = 0;
        getSingleConversion(parent->tp, basetype(sym->tp)->btp, nullptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr, false);
        m1 = m;
        while (m1 && seq[m1 - 1] == CV_IDENTITY)
            m1--;
        if (m1 > 10)
        {
            return false;
        }
        for (i = 0; i < m; i++)
            if (seq[i] == CV_NONE)
                return false;
        memcpy(arr + pos, seq, m * sizeof(enum e_cvsrn));
        sizes[n++] = m;
        pos += m;
        ++it;
        tpp = argsym->tp;
        MakeType(tpx, bt_pointer, f->arguments->front()->tp);
        m = 0;
        seq[m++] = CV_USER;
        getSingleConversion(tpp, &tpx, f->thisptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr, true);
        m1 = m;
        while (m1 && seq[m1 - 1] == CV_IDENTITY)
            m1--;
        if (m1 > 10)
        {
            return false;
        }
        for (i = 0; i < m; i++)
            if (seq[i] == CV_NONE)
                return false;
        memcpy(arr + pos, seq, m * sizeof(enum e_cvsrn));
        sizes[n++] = m;
        pos += m;
        return true;
    }
    else
    {
        if (f)
        {
            if ((f->thistp || (ita != itae && (*ita)->tp)) && ismember(sym))
            {
                // nonstatic function
                TYPE* argtp = sym->tp;
                if (!argtp)
                {
                    arr[n++] = CV_NONE;
                    return false;
                }
                else
                {
                    TYPE tpx;
                    TYPE* tpp;
                    TYPE* tpthis = f->thistp;
                    SYMBOL* argsym = *it;
                    memset(&tpx, 0, sizeof(tpx));
                    ++it;
                    tpp = argsym->tp;
                    if (!tpthis)
                    {
                        tpthis = ita != itae ? (*ita)->tp : nullptr;
                        if (ita != itae)
                            ++ita;
                    }
                    if (sym->sb->castoperator || (tpthis && f->thistp == nullptr))
                    {
                        tpthis = &tpx;
                        MakeType(tpx, bt_pointer, f->arguments->front()->tp);
                    }
                    else if (sym->sb->isDestructor)
                    {
                        tpthis = &tpx;
                        MakeType(tpx, bt_pointer, basetype(basetype(f->thistp)->btp));
                    }
                    if (islrqual(sym->tp) || isrrqual(sym->tp))
                    {
                        bool lref = lvalue(f->thisptr);
                        auto strtype = basetype(f->thistp)->btp;
                        if (isstructured(strtype) && f->thisptr->type != en_not_lvalue)
                        {
                            if (strtype->lref)
                                lref = true;
                            else if (!strtype->rref)
                            {
                                EXPRESSION* expx = f->thisptr;
                                if (expx->type == en_thisref)
                                    expx = expx->left;
                                if (expx->type == en_func)
                                {
                                    if (expx->v.func->returnSP)
                                    {
                                        if (!expx->v.func->returnSP->sb->anonymous)
                                            lref = true;
                                    }
                                }
                                else
                                {
                                    lref = true;
                                }
                            }
                        }
                        if (isrrqual(sym->tp))
                        {
                            if (lref)
                                return false;
                        }
                        else if (!lref)
                            return false;
                    }
                    m = 0;
                    if (((f->thisptr && isconstexpr(f->thisptr)) ||
                         (!f->thisptr && f->arguments && isconstexpr(f->arguments->front()->exp))) &&
                        !isconst(sym->tp))
                        seq[m++] = CV_QUALS;
                    getSingleConversion(tpp, tpthis, f->thisptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr, true);
                    m1 = m;
                    while (m1 && seq[m1 - 1] == CV_IDENTITY)
                        m1--;
                    if (m1 > 10)
                    {
                        return false;
                    }
                    for (i = 0; i < m; i++)
                        if (seq[i] == CV_NONE)
                            return false;
                    memcpy(arr + pos, seq, m * sizeof(enum e_cvsrn));
                    sizes[n++] = m;
                    pos += m;
                }
            }
        }
        else
        {
            if (ismember(sym))
            {
                TYPE* argtp = sym->sb->parentClass->tp;
                if (!argtp)
                {
                    return false;
                }
                else if (ita != itae || f && itt != atp->syms->end())
                {
                    getSingleConversion(argtp, ita != itae ? (*ita)->tp : (*itt)->tp, (ita != itae) ? (*ita)->exp : nullptr, &m, seq, sym,
                                        userFunc ? &userFunc[n] : nullptr, true);
                    if (ita != itae)
                        ++ita;
                    else if (f)
                        ++itt;
                }
            }
        }
        std::list<TEMPLATEPARAMPAIR>* tr = nullptr;
        std::list<TEMPLATEPARAMPAIR>::iterator itr, itre;
        while (it != ite && (ita != itae || (!f && itt != atp->syms->end())))
        {
            SYMBOL* argsym = *it;
            if (argsym->tp->type != bt_any)
            {
                TYPE* tp;
                if (argsym->sb->constop)
                    break;
                if (argsym->sb->storage_class != sc_parameter)
                    return false;
                if (!tr && argsym->tp->type == bt_templateparam && argsym->tp->templateParam->second->packed)
                {
                    tr = argsym->tp->templateParam->second->byPack.pack;
                    if (tr)
                    {
                        itr = tr->begin();
                        itre = tr->end();
                    }
                }
                if (tr && itr != itre)
                    tp = itr->second->byClass.val;  // DAL not modified
                else
                    tp = argsym->tp;
                if (basetype(tp)->type == bt_ellipse)
                {
                    arr[pos] = CV_ELLIPSIS;
                    sizes[n++] = 1;
                    return true;
                }
                m = 0;
                TYPE* tp1 = tp;
                if (isref(tp1))
                    tp1 = basetype(tp1)->btp;
                initializerListType = nullptr;
                if (isstructured(tp1))
                {
                    SYMBOL* sym1 = basetype(tp1)->sp;
                    if (sym1->sb->initializer_list && sym1->sb->templateLevel)
                    {
                        auto it = sym1->templateParams->begin();
                        ++it;
                        initializerListType = it->second->byClass.val;
                    }
                }
                if (initializerListType)
                {
                    if (ita != itae && (*ita)->nested && (*ita)->nested->size())
                    {
                        if (isstructured(initializerListType))
                        {
                            std::list<INITLIST*> nested;
                            if (!(*ita)->initializer_list)
                            {
                                nested.push_back((*ita)->nested->front());
                            }
                            else
                            {
                                nested.insert(nested.begin(), (*ita)->nested->begin(), (*ita)->nested->end());
                            }
                            getInitListConversion(initializerListType, &nested, nullptr, &m, seq, sym,
                                                  userFunc ? &userFunc[n] : nullptr);
                        }
                        else
                        {
                            std::list<INITLIST*> nested;
                            if (!(*ita)->initializer_list)
                            {
                                nested.push_back((*ita)->nested->front());
                            }
                            else
                            {
                                nested.insert(nested.begin(), (*ita)->nested->begin(), (*ita)->nested->end());
                            }
                            getInitListConversion(initializerListType, &nested, nullptr, &m, seq, sym,
                                                  userFunc ? &userFunc[n] : nullptr);
                            if ((*ita)->initializer_list && (*ita)->nested->front()->nested)
                                ++it;
                        }
                    }
                    else if ((*ita)->initializer_list)
                    {
                        getSingleConversion(initializerListType, ita != itae ? (*ita)->tp : (*itt)->tp, ita != itae ? (*ita)->exp : nullptr, &m,
                                            seq, sym, userFunc ? &userFunc[n] : nullptr, true);
                    }
                    else if ((*ita)->tp && (*ita)->exp)  // might be an empty initializer list...
                    {
                        getSingleConversion((basetype(tp1)->sp)->tp, ita != itae ? (*ita)->tp : (*itt)->tp, ita != itae ? (*ita)->exp : nullptr, &m,
                                            seq, sym, userFunc ? &userFunc[n] : nullptr, true);
                    }
                }
                else if (ita != itae && ((*ita)->nested || (!(*ita)->tp && !(*ita)->exp)))
                {
                    seq[m++] = CV_QUALS;  // have to make a distinction between an initializer list and the same func without one...
                    if (basetype(tp)->type == bt_lref)
                    {
                        seq[m++] = CV_LVALUETORVALUE;
                    }
                    if ((*ita)->nested && (*ita)->nested->size())
                    {
                        auto ita1 = ita;
                        ++ita1;
                        if ((*ita)->nested->front()->nested || (*ita)->initializer_list || ita1 != itae ||
                            (isstructured(tp1) &&
                             (!sym->sb->isConstructor || (!comparetypes(basetype(tp1), sym->sb->parentClass->tp, true) &&
                                                          !sameTemplate(basetype(tp1), sym->sb->parentClass->tp)))))
                        {
                            initializerListType = basetype(tp1);
                            if (!sym->sb->parentClass || (!matchesCopy(sym, false) && !matchesCopy(sym, true)))
                            {
                                getInitListConversion(basetype(tp1), (*ita)->nested, nullptr, &m, seq, sym,
                                                          userFunc ? &userFunc[n] : nullptr);
                                if ((*ita)->initializer_list)
                                {
                                    ++it;
                                }
                            }
                            else
                            {
                                seq[m++] = CV_NONE;
                            }
                        }
                        else
                        {
                            // this didn't originally reset end
                            itae = (*ita)->nested->end();
                            ita = (*ita)->nested->begin();
                            if (ita != itae)
                            {
                                getSingleConversion(tp1,
                                                    ita != itae     ? (*ita)->tp
                                                    : itt != atp->syms->end() ? (*itt)->tp
                                                          : tp1,
                                                    ita != itae ? (*ita)->exp : nullptr, &m, seq, sym, userFunc ? &userFunc[n] : nullptr, true);
                            }
                        }
                    }
                }
                else
                {
                    TYPE* tp2 = tp;
                    if (isref(tp2))
                        tp2 = basetype(tp2)->btp;
                    if (ita != itae && (*ita)->tp->type == bt_aggregate &&
                        (isfuncptr(tp2) || (basetype(tp2)->type == bt_memberptr && isfunction(basetype(tp2)->btp))))
                    {
                        MatchOverloadedFunction(tp2, &(*ita)->tp, (*ita)->tp->sp, &(*ita)->exp, 0);
                    }
                    getSingleConversion(tp, ita != itae ? (*ita)->tp : (*itt)->tp, ita != itae ? (*ita)->exp : nullptr, &m, seq, sym,
                                        userFunc ? &userFunc[n] : nullptr, true);
                }
                m1 = m;
                while (m1 && seq[m1 - 1] == CV_IDENTITY)
                    m1--;
                if (m1 > 10)
                {
                    return false;
                }
                for (i = 0; i < m; i++)
                    if (seq[i] == CV_NONE)
                        return false;
                memcpy(arr + pos, seq, m * sizeof(enum e_cvsrn));
                sizes[n++] = m;
                pos += m;
            }
            if (tr && itr != itre)
                ++itr;
            if (ita != itae)
                ++ita;
            else
                ++itt;
            if ((!initializerListType || ita == itae || !(*ita)->initializer_list) && !tr)
                ++it;
        }
        if (it != ite)
        {
            SYMBOL* sym = *it;
            if (sym->sb->init || sym->sb->deferredCompile || sym->packed)
            {
                return true;
            }
            if (basetype(sym->tp)->type == bt_ellipse)
            {
                sizes[n++] = 1;
                arr[pos++] = CV_ELLIPSIS;
                return true;
            }
            if (sym->tp->type == bt_void || sym->tp->type == bt_any)
                return true;
            return false;
        }
        return ita == itae||
               ((*ita)->tp && (*ita)->tp->type == bt_templateparam && (*ita)->tp->templateParam->second->packed && !(*ita)->tp->templateParam->second->byPack.pack);
    }
}
SYMBOL* detemplate(SYMBOL* sym, FUNCTIONCALL* args, TYPE* atp)
{
    inDeduceArgs++;
    if (sym->sb->templateLevel)
    {
        if (atp || args)
        {
            bool linked = false;
            if (sym->sb->parentNameSpace && !sym->sb->parentNameSpace->sb->value.i)
            {
                Optimizer::LIST* list;
                SYMBOL* ns = sym->sb->parentNameSpace;
                linked = true;
                ns->sb->value.i++;

                nameSpaceList.push_front(ns);

                globalNameSpace->push_front(ns->sb->nameSpaceValues->front());
            }
            if (args && !TemplateIntroduceArgs(sym->templateParams, args->templateParams))
                sym = nullptr;
            else if (atp)
                sym = TemplateDeduceArgsFromType(sym, atp);
            else if (args->ascall)
                sym = TemplateDeduceArgsFromArgs(sym, args);
            else
                sym = TemplateDeduceWithoutArgs(sym);
            if (linked)
            {
                SYMBOL* sym = nameSpaceList.front();
                sym->sb->value.i--;
                nameSpaceList.pop_front();
                globalNameSpace->pop_front();;
            }
        }
        else
        {
            sym = nullptr;
        }
    }
    inDeduceArgs--;
    return sym;
}
static int CompareArgs(SYMBOL* left, SYMBOL* right)
{
    int countl = 0, countr = 0;
    auto itl = basetype(left->sb->parentTemplate->tp)->syms->begin();
    auto itlend = basetype(left->sb->parentTemplate->tp)->syms->end();
    auto itr = basetype(right->sb->parentTemplate->tp)->syms->begin();
    auto itrend = basetype(right->sb->parentTemplate->tp)->syms->end();
    if ((*itl)->sb->thisPtr)
        ++itl;
    if ((*itr)->sb->thisPtr)
        ++itr;
    while (itl != itlend && itr != itrend)
    {
        auto tpl = (*itl)->tp;
        auto tpr = (*itr)->tp;
        if (isref(tpl))
            tpl = basetype(tpl)->btp;
        if (isref(tpr))
            tpr = basetype(tpr)->btp;
        while (ispointer(tpl) && ispointer(tpr))
        {
            tpl = basetype(tpl)->btp;
            tpr = basetype(tpr)->btp;
        }
        tpl = basetype(tpl);
        tpr = basetype(tpr);
        if (tpl->type != bt_templateparam && tpl->type != bt_templateselector)
            countl++;
        if (tpr->type != bt_templateparam && tpr->type != bt_templateselector)
            countr++;
        ++itl;
        ++itr;
    }
    if (countl > countr)
        return -1;
    if (countr > countl)
        return 1;
    return 0;
}
static void WeedTemplates(SYMBOL** table, int count, FUNCTIONCALL* args, TYPE* atp)
{
    int i = count;
    if (atp || !args->astemplate)
    {
        for (i = 0; i < count; i++)
            if (table[i] && (!table[i]->sb->templateLevel || !table[i]->templateParams))
                break;
    }
    else
    {
        for (i = 0; i < count; i++)
            if (table[i] && (!table[i]->sb->templateLevel || !table[i]->templateParams))
                table[i] = nullptr;
    }
    if (i < count)
    {
        // one or more first class citizens, don't match templates
        for (i = 0; i < count; i++)
            if (table[i] && table[i]->sb->templateLevel && table[i]->templateParams)
                table[i] = nullptr;
    }
    else
    {
        TemplatePartialOrdering(table, count, args, atp, false, true);
        // now we out nonspecializations if specializations are present
        int i;
        for (i = 0; i < count; i++)
        {
            if (table[i] && table[i]->sb->specialized)
                break;
        }
        if (i < count)
        {
            for (int i = 0; i < count; i++)
            {
                if (table[i] && !table[i]->sb->specialized)
                    table[i] = 0;
            }
        }
        int argCount = INT_MAX;
        int* counts = (int*)alloca(sizeof(int) * count);
        // choose the template with the smallest argument count
        // on the theory it is more specialized
        for (int i = 0; i < count; i++)
        {
            if (table[i])
            {
                if (table[i]->templateParams)
                    counts[i] = table[i]->templateParams->size();
                else
                    counts[i] = 0;

                if (counts[i] < argCount)
                    argCount = counts[i];
            }
        }
        for (int i = 0; i < count; i++)
        {
            if (table[i])
            {
                if (counts[i] > argCount)
                    table[i] = 0;
            }
        }
        // prefer templates that have args with a type that arent templateselectors or templateparams
        for (int i = 0; i < count - 1; i++)
        {
            if (table[i])
            {
                for (int j = i + 1; table[i] && j < count; j++)
                {
                    if (table[j])
                    {
                        switch (CompareArgs(table[i], table[j]))
                        {
                        case -1:
                            table[j] = nullptr;
                            break;
                        case 1:
                            table[i] = nullptr;
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}
SYMBOL* GetOverloadedTemplate(SYMBOL* sp, FUNCTIONCALL* args)
{
    SYMBOL *found1 = nullptr, *found2 = nullptr;
    std::vector<SYMBOL *> spList;
    std::vector<enum e_cvsrn*> icsList;
    std::vector<int*> lenList;
    std::vector<SYMBOL**> funcList;
    std::list<SYMBOL*> gather;
    int n = 0, i, argCount = args->arguments ? args->arguments->size() : 0;
    gather.push_front(sp);
    n += sp->tp->syms->size();
    if (n == 0)
        return nullptr;
    spList.resize(n);
    icsList.resize(n);
    lenList.resize(n);
    funcList.resize(n);
    n = insertFuncs(&spList[0], gather, args, nullptr, 0);
    if (n != 1 || (spList[0] && !spList[0]->sb->isDestructor))
    {
        if (args->ascall)
        {
            GatherConversions(sp, &spList[0], n, args, nullptr, &icsList[0], &lenList[0], argCount, &funcList[0], 0);
            SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
        }
        WeedTemplates(&spList[0], n, args, nullptr);
        for (i = 0; i < n && !found1; i++)
        {
            int j;
            found1 = spList[i];
            for (j = i + 1; j < n && found1 && !found2; j++)
            {
                if (spList[j] && found1 != spList[j] && !sameTemplate(found1->tp, spList[j]->tp))
                {
                    found2 = spList[j];
                }
            }
        }
    }
    else
    {
        found1 = spList[0];
    }
    if (!found1 || found2)
    {
        return nullptr;
    }
    return found1;
}
void weedgathering(std::list<SYMBOL*>& gather)
{
    std::unordered_set<SYMBOL*>toRemove;
    std::set<SYMBOL*> set;
    for (auto p : gather)
        if (set.find(p) == set.end())
            set.insert(p);
        else
            toRemove.insert(toRemove.begin(), p);
    for (auto it = gather.begin(); it != gather.end();)
    {
        auto itr = toRemove.find(*it);
        if (itr != toRemove.end())
        {
            it = gather.erase(it);
            toRemove.erase(itr);
        }
        else
        {
            ++it;
        }
    }
}
static int insertFuncs(SYMBOL** spList, std::list<SYMBOL* >& gather, FUNCTIONCALL* args, TYPE* atp, int flags)
{
    std::set<SYMBOL*> filters;
    inSearchingFunctions++;
    int n = 0;
    for (auto sp : gather)
    {
        for (auto sym : *sp->tp->syms)
        {
            if (filters.find(sym) == filters.end() && filters.find(sym->sb->mainsym) == filters.end() &&
                (!args || !args->astemplate || sym->sb->templateLevel) &&
                (!sym->sb->instantiated || sym->sb->specialized2 || sym->sb->isDestructor))
            {
                auto it1 = basetype(sym->tp)->syms->begin();
                auto it1end = basetype(sym->tp)->syms->end();
                auto arg = args->arguments;
                bool ellipse = false;
                bool found = false;
                if (sym->name[0] == '.' || sym->sb->templateLevel)
                {
                    it1 = it1end;
                }
                else
                {
                    if ((*it1)->sb->thisPtr)
                        ++it1;
                    if ((*it1)->tp->type == bt_void)
                        ++it1;
                    if (args->arguments && args->arguments->size())
                    {
                        auto ita = args->arguments->begin();
                        auto itae = args ->arguments->end();
                        if (ita != itae && (*ita)->tp && (*ita)->tp->type == bt_void)
                            ++ ita;
                        while (ita != itae && it1 != it1end)
                        {

                            if ((*it1)->tp->type == bt_ellipse || !(*ita)->tp) // ellipse or initializer list
                                ellipse = true;
                            ++ita;
                            ++it1;
                        }
                        found = ita != itae;
                    }
                }
                if ((!found || ellipse) && (it1 == it1end || (*it1)->sb->defaultarg || (*it1)->tp->type == bt_ellipse))
                {
                    if (sym->sb->templateLevel && (sym->templateParams || sym->sb->isDestructor))
                    {
                        if (sym->sb->castoperator)
                        {
                            spList[n] = detemplate(sym, nullptr, basetype(args->thistp)->btp);
                        }
                        else
                        {
                            spList[n] = detemplate(sym, args, atp);
                        }
                    }
                    else
                    {
                        spList[n] = sym;
                    }
                }
                filters.insert(sym);
                if (sym->sb->mainsym)
                    filters.insert(sym->sb->mainsym);
                n++;
            }
        }
    }
    inSearchingFunctions--;
    return n;
}
static void doNames(SYMBOL* sym)
{
    if (sym->sb->parentClass)
        doNames(sym->sb->parentClass);
    SetLinkerNames(sym, lk_cdecl);
}
static bool IsMove(SYMBOL* sp)
{
    bool rv = false;
    if (sp->sb->isConstructor)
    {
        auto it = basetype(sp->tp)->syms->begin();
        auto itend = basetype(sp->tp)->syms->end();
        auto thisPtr = it != itend ? *it : nullptr;
        if (thisPtr && thisPtr->sb->thisPtr)
            ++it;
        if (it != itend && thisPtr->sb->thisPtr)
        {
            if (basetype((*it)->tp)->type == bt_rref)
            {
                auto tp1 = basetype(basetype((*it)->tp)->btp);
                auto tp2 = basetype(basetype(thisPtr->tp)->btp);
                if (isstructured(tp1) && isstructured(tp2))
                {
                    rv = comparetypes(tp2, tp1, true) || sameTemplate(tp2, tp1);
                }
            }
        }
    }
    return rv;
}
SYMBOL* GetOverloadedFunction(TYPE** tp, EXPRESSION** exp, SYMBOL* sp, FUNCTIONCALL* args, TYPE* atp, int toErr,
                              bool maybeConversion, int flags)
{
    STRUCTSYM s;
    s.tmpl = 0;
    if (atp && ispointer(atp))
        atp = basetype(atp)->btp;
    if (atp && !isfunction(atp))
        atp = nullptr;
    if (args && args->thisptr)
    {
        SYMBOL* spt = basetype(basetype(args->thistp)->btp)->sp;
        s.tmpl = spt->templateParams;
        if (s.tmpl)
            addTemplateDeclaration(&s);
    }
    if (!sp || sp->sb->storage_class == sc_overloads)
    {
        std::list<SYMBOL*> gather;
        SYMBOL *found1 = nullptr, *found2 = nullptr;
        if (!Optimizer::cparams.prm_cplusplus && ((Optimizer::architecture != ARCHITECTURE_MSIL) ||
                                                  !Optimizer::cparams.msilAllowExtensions || (sp && sp->tp->syms->size() == 1)))
        {
            if (sp->tp->syms->begin() != sp->tp->syms->end())
            {
                sp = *sp->tp->syms->begin();
                *exp = varNode(en_pc, sp);
                *tp = sp->tp;
            }
            return nullptr;
        }
        if (sp)
        {
            if (args || atp)
            {
                if ((!sp->tp || (!sp->sb->wasUsing && !sp->sb->parentClass)) && !args->nameSpace)
                {
                    // ok the sp is a valid candidate for argument search
                    if (args)
                    {
                        if (args->arguments)
                        {
                            for (auto item : *args->arguments)
                            {
                                if (item->tp)
                                    searchOneArg(sp, gather, item->tp);
                            }
                            if (args->thisptr)
                                searchOneArg(sp, gather, args->thistp);
                        }
                    }
                    else
                    {
                        for (auto sp : *atp->syms)
                        {
                            if (sp->sb->storage_class != sc_parameter)
                                break;
                            searchOneArg(sp, gather, sp->tp);
                        }
                    }
                }
                weedToFunctions(gather);
            }
            if (sp->tp)
            {
                if (args->nameSpace)
                {
                    unvisitUsingDirectives(args->nameSpace);
                    gather = tablesearchinline(sp->name, args->nameSpace, false, true);
                }
                else
                {
                    bool found = false;
                    for (auto sym : gather)
                        if (sym == sp)
                        {
                            found = true;
                            break;
                        }
                    if (!found)
                    {
                        gather.push_front(sp);
                    }
                }
            }
            weedgathering(gather);
        }
        // ok got the initial list, time for phase 2
        // which is to add any other functions that have to be added...
        // constructors, member operator '()' and so forth...
        if (gather.size())
        {
            // we are only doing global functions for now...
            // so nothing here...
        }
        if (maybeConversion)
        {
            if (args->arguments && args->arguments->size() == 1 && !args->arguments->front()->nested)  // one arg
                GetMemberCasts(gather, basetype(args->arguments->front()->tp)->sp);
        }
        // pass 3 - the actual argument-based resolution
        if (gather.size())
        {

            Optimizer::LIST* lst2;
            int n = 0;
            if (args->arguments)
            {
                for (auto argl : *args->arguments)
                {
                    if (argl->tp && argl->tp->type == bt_aggregate)
                    {
                        auto it = argl->tp->syms->begin();
                        SYMBOL* func = *it;
                        if (!func->sb->templateLevel && ++it == argl->tp->syms->end())
                        {
                            argl->tp = func->tp;
                            argl->exp = varNode(en_pc, func);
                        }
                        else if (argl->exp->type == en_func && argl->exp->v.func->astemplate && !argl->exp->v.func->ascall)
                        {
                            TYPE* ctype = argl->tp;
                            EXPRESSION* exp = nullptr;
                            auto sp = GetOverloadedFunction(&ctype, &exp, argl->exp->v.func->sp, argl->exp->v.func, nullptr, toErr,
                                false, 0);
                            if (sp)
                            {
                                argl->tp = ctype;
                                argl->exp = exp;
                            }
                        }
                    }
                }
            }
            for (auto sym1 : gather)
            {
                for (auto sym : *sym1->tp->syms)
                {
                    if ((!args || !args->astemplate || sym->sb->templateLevel) && (!sym->sb->instantiated || sym->sb->isDestructor))
                    {
                        n++;
                    }
                }
            }
            if (args || atp)
            {
                int i;
                std::vector<SYMBOL*> spList;
                std::vector<enum e_cvsrn*> icsList;
                std::vector<int*> lenList;
                std::vector<SYMBOL**> funcList;
                int argCount = 0;
                if (args)
                {
                    argCount = args->arguments ? args->arguments->size() : 0;
                    if (args->thisptr)
                        argCount++;
                }
                else
                {

                    for (auto sym : *atp->syms)
                    {
                        if (sym->sb->storage_class == sc_parameter)
                        {
                            argCount++;
                        }
                        else
                        {
                            if (ismember(sym))
                                argCount++;
                            break;
                        }
                    }
                }
                if (n == 0)
                    return nullptr;
                spList.resize(n);
                icsList.resize(n);
                lenList.resize(n);
                funcList.resize(n);
                n = insertFuncs(&spList[0], gather, args, atp, flags);
                if (n != 1 || (spList[0] && !spList[0]->sb->isDestructor && !spList[0]->sb->specialized2))
                {
                    bool hasDest = false;
           
                    
                    std::unordered_map<int, SYMBOL*> storage;
                    if (atp || args->ascall)
                        GatherConversions(sp, &spList[0], n, args, atp, &icsList[0], &lenList[0], argCount, &funcList[0],
                                          flags & _F_INITLIST);
                    for (int i = 0; i < n; i++)
                    {
                        storage[i] = spList[i];
                        hasDest |= spList[i] && spList[i]->sb->deleted;
                    }
                    if (atp || args->ascall)
                        SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
                    WeedTemplates(&spList[0], n, args, atp);
                    for (i = 0; i < n && !found1; i++)
                    {
                        if (spList[i] && !spList[i]->sb->deleted && !spList[i]->sb->castoperator)
                           found1 = spList[i];
                    }
                    for (i = 0; i < n && !found1; i++)
                    {
                        if (spList[i] && !spList[i]->sb->deleted)
                            found1 = spList[i];
                    }
                    for (i = 0; i < n; i++)
                    {
                        int j;
                        if (!found1)
                            found1 = spList[i];
                        for (j = i; j < n && found1; j++)
                        {
                            if (spList[j] && found1 != spList[j] && found1->sb->castoperator == spList[j]->sb->castoperator && !sameTemplate(found1->tp, spList[j]->tp))
                            {
                                found2 = spList[j];
                            }
                        }
                        if (found1)
                            break;
                    }
                    if ((!found1 || (!IsMove(found1) && found1->sb->deleted)) && hasDest)
                    {
                        auto found3 = found1;
                        auto found4 = found2;
                        // there were no matches.   But there are deleted functions
                        // see if we can find a match among them...
                        found1 = found2 = 0;
                        for (auto v : storage)
                            if (!v.second || !v.second->sb->deleted)
                                spList[v.first] = v.second;
                            else
                                spList[v.first] = nullptr;
                        if (atp || args->ascall)
                            SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
                        WeedTemplates(&spList[0], n, args, atp);
                        for (i = 0; i < n && !found1; i++)
                        {
                            if (spList[i] && !spList[i]->sb->deleted && !spList[i]->sb->castoperator)
                               found1 = spList[i];
                        }
                        for (i = 0; i < n && !found1; i++)
                        {
                            if (spList[i] && !spList[i]->sb->deleted)
                               found1 = spList[i];
                        }
                        for (i = 0; i < n; i++)
                        {
                            int j;
                            if (!found1)
                                found1 = spList[i];
                            for (j = i; j < n && found1 && !found2; j++)
                            {
                            if (spList[j] && found1 != spList[j] && found1->sb->castoperator == spList[j]->sb->castoperator && !sameTemplate(found1->tp, spList[j]->tp))
                                {
                                    found2 = spList[j];
                                }
                            }
                            if (found1)
                                break;
                        }
                        if (!found1)
                        {
                            found1 = found3;
                            found2 = found4;
                        }
                    }
                    if (found1 && found2 && !found1->sb->deleted && found2->sb->deleted)
                        found2 = nullptr;
#if !NDEBUG
                    // this block to aid in debugging unfound functions...
                    if ((toErr & F_GOFERR) && !inDeduceArgs && (!found1 || (found1 && found2)) && !templateNestingCount)
                    {

                        n = insertFuncs(&spList[0], gather, args, atp, flags);
                        if (atp || args->ascall)
                        {
                            GatherConversions(sp, &spList[0], n, args, atp, &icsList[0], &lenList[0], argCount, &funcList[0],
                                              flags & _F_INITLIST);
                            SelectBestFunc(&spList[0], &icsList[0], &lenList[0], args, argCount, n, &funcList[0]);
                        }
                        WeedTemplates(&spList[0], n, args, atp);
                    }
#endif
                }
                else
                {
                    found1 = spList[0];
                }
            }
            else
            {
                auto it = gather.front()->tp->syms->begin();
                found1 = *it;
                if (n > 1 && ++it != gather.front()->tp->syms->end())
                    found2 = *it;
            }
        }
        // any errors
        if ((toErr & F_GOFERR) || (found1 && !found2))
        {
            if (!found1)
            {
                bool doit = true;

                // if we are in an argument list and there is an empty packed argument
                // don't generate an error on the theory there will be an ellipsis...
                if (flags & (_F_INARGS | _F_INCONSTRUCTOR))
                {
                    if (args->arguments)
                    for (auto arg : *args->arguments)
                    {
                        if (arg->tp && arg->tp->type == bt_templateparam && arg->tp->templateParam->second->packed)
                            doit = !!arg->tp->templateParam->second->byPack.pack;
                    }
                }
                if (doit)
                {
                    if (args && args->arguments && args->arguments->size() == 1  // one arg
                        && sp && sp->sb->isConstructor)                    // conversion constructor
                    {
                        errortype(ERR_CANNOT_CONVERT_TYPE, args->arguments->front()->tp, sp->sb->parentClass->tp);
                    }
                    else if (!sp)
                    {
                        if (*tp && isstructured(*tp))
                        {
                            char *buf = (char*)alloca(4096), *p;
                            int n;
                            buf[0] = 0;
                            unmangle(buf, basetype(*tp)->sp->sb->decoratedName);
                            n = strlen(buf);
                            p = (char*)strrchr(buf, ':');
                            if (p)
                                p++;
                            else
                                p = buf;
                            strcpy(buf + n + 2, p);
                            buf[n] = buf[n + 1] = ':';
                            strcat(buf, "(");
                            if (args->arguments)
                            {
                                for (auto a : *args->arguments)
                                {
                                    typeToString(buf + strlen(buf), a->tp);
                                    strcat(buf, ",");
                                }
                                buf[strlen(buf) - 1] = 0;
                            }
                            strcat(buf, ")");
                            errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, buf);
                        }
                        else
                        {
                            errorstr(ERR_NO_OVERLOAD_MATCH_FOUND, "unknown");
                        }
                    }
                    else
                    {
                        SYMBOL* sym = SymAlloc();
                        sym->sb->parentClass = sp->sb->parentClass;
                        sym->name = sp->name;
                        sym->sb->parentNameSpace = args->nameSpace ? args->nameSpace->name : nullptr;
                        if (atp)
                        {
                            sym->tp = atp;
                        }
                        else
                        {
                            int v = 1;
                            sym->tp = MakeType(bt_func, &stdint);
                            sym->tp->size = getSize(bt_pointer);
                            sym->tp->syms = symbols.CreateSymbolTable();
                            sym->tp->sp = sym;
                            if (args->arguments)
                            {
                                for (auto a : *args->arguments)
                                {
                                    SYMBOL* sym1 = SymAlloc();
                                    char nn[10];
                                    Optimizer::my_sprintf(nn, "%d", v++);
                                    sym1->name = litlate(nn);
                                    sym1->tp = a->tp;
                                    sym->tp->syms->Add(sym1);
                                }
                            }
                        }
                        SetLinkerNames(sym, lk_cpp);

                        errorsym(ERR_NO_OVERLOAD_MATCH_FOUND, sym);
                    }
                }
            }
            else if (found1 && found2)
            {
                if (toErr && !(flags & _F_INDECLTYPE))
                {
                    errorsym2(ERR_AMBIGUITY_BETWEEN, found1, found2);
                }
                else
                {
                    found1 = found2 = nullptr;
                }
            }
            else if (found1->sb->deleted && !templateNestingCount)
            {
                if (toErr)
                    errorsym(ERR_DELETED_FUNCTION_REFERENCED, found1);
                else if (!(flags & _F_RETURN_DELETED))
                    found1 = nullptr;
            }
            if (found1)
            {
                if (flags & _F_IS_NOTHROW)
                    inNothrowHandler++;
                if (found1->sb->attribs.uninheritable.deprecationText)
                    deprecateMessage(found1);
                if (!(flags & _F_SIZEOF) ||
                    ((flags & _F_IS_NOTHROW) && found1->sb->deferredNoexcept != 0 && found1->sb->deferredNoexcept != (LEXLIST*)-1))
                {
                    if (theCurrentFunc && !found1->sb->constexpression)
                    {
                        theCurrentFunc->sb->nonConstVariableUsed = true;
                    }
                    if (found1->sb->templateLevel && (found1->templateParams || found1->sb->isDestructor))
                    {
                        found1 = found1->sb->mainsym;
                        inSearchingFunctions++;
                        if (found1->sb->castoperator)
                        {
                            found1 = detemplate(found1, nullptr, basetype(args->thistp)->btp);
                        }
                        else
                        {
                            found1 = detemplate(found1, args, atp);
                        }
                        inSearchingFunctions--;
                        found1->sb->attribs.inheritable.linkage4 = lk_virtual;
                    }
                    else if (!found1->sb->templateLevel && found1->sb->parentClass && found1->sb->parentClass->templateParams && (!templateNestingCount || instantiatingTemplate) && !found1->sb->isDestructor)
                    {
                        auto old = found1;
                        if (found1->sb->mainsym)
                            found1 = found1->sb->mainsym;
                        found1 = CopySymbol(found1);
                        found1->sb->mainsym = old;
                        found1->sb->parentClass = CopySymbol(found1->sb->parentClass);
                        found1->sb->parentClass->sb->mainsym = old->sb->parentClass;
                        found1->sb->parentClass->templateParams = copyParams(found1->sb->parentClass->templateParams, true);
                    }
                    if (isstructured(basetype(found1->tp)->btp))
                    {
                        TYPE** tp1 = &basetype(found1->tp)->btp;
                        while ((*tp1)->rootType != *tp1)
                            tp1 = &(*tp1)->btp;
                        *tp1 = (*tp1)->sp->tp;
                    }
                    for (auto sym : *basetype(found1->tp)->syms)
                    {
                        CollapseReferences(sym->tp);
                    }
                    CollapseReferences(basetype(found1->tp)->btp);
                    if (found1->sb->templateLevel && (!templateNestingCount || instantiatingTemplate) && found1->templateParams)
                    {
                        if (!inSearchingFunctions || inTemplateArgs)
                        {
                            found1 = TemplateFunctionInstantiate(found1, false);
                        }
                    }
                        
                    if ((flags & _F_IS_NOTHROW) || (found1->sb->constexpression && (!templateNestingCount || instantiatingTemplate)))
                    {
                        if (found1->sb->deferredNoexcept && (!found1->sb->constexpression || (templateNestingCount && !instantiatingTemplate)))
                        {
                            STRUCTSYM s;
                            if (!found1->sb->deferredCompile && !found1->sb->deferredNoexcept)
                                propagateTemplateDefinition(found1);
                            if (found1->templateParams)
                            {
                                s.tmpl = found1->templateParams;
                                addTemplateDeclaration(&s);
                            }
                            parseNoexcept(found1);
                            if (found1->templateParams)
                            {
                                dropStructureDeclaration();
                            }
                        }
                        else if (found1->sb->deferredCompile && !found1->sb->inlineFunc.stmt)
                        {
                            if (!inSearchingFunctions || inTemplateArgs)
                            {
                                CompileInline(found1, false);
                            }
                        }
                    }
                    if (found1->sb->inlineFunc.stmt)
                    {
                        noExcept &= found1->sb->noExcept;
                    }
                }
                else
                {
                    CollapseReferences(basetype(found1->tp)->btp);
                }
                if (found1->templateParams)
                    SetLinkerNames(found1, lk_cdecl);
                if (isautotype(basetype(found1->tp)->btp))
                    errorsym(ERR_AUTO_FUNCTION_RETURN_TYPE_NOT_DEFINED, found1);
                if (flags & _F_IS_NOTHROW)
                    inNothrowHandler--;
            }
        }
        if (!(toErr & F_GOFERR) && found2)
        {
            sp = nullptr;
        }
        else
        {
            sp = found1;
            if (sp)
            {
                UpdateRootTypes(basetype(sp->tp)->btp);
                *exp = varNode(en_pc, sp);
                *tp = sp->tp;
            }
        }
    }
     if (s.tmpl)
        dropStructureDeclaration();
    return sp;
}
SYMBOL* MatchOverloadedFunction(TYPE* tp, TYPE** mtp, SYMBOL* sym, EXPRESSION** exp, int flags)
{
    FUNCTIONCALL fpargs;
    std::list<INITLIST*> args;
    fpargs.arguments = &args;
    EXPRESSION* exp2 = *exp;
    bool found = false;
    SymbolTable<SYMBOL>::iterator itp, itpe;
    tp = basetype(tp);
    if (isfuncptr(tp) || tp->type == bt_memberptr)
    {
        itp = basetype(basetype(tp)->btp)->syms->begin();
        itpe = basetype(basetype(tp)->btp)->syms->end();
        found = true;
    }
    else
    {
        if (!*exp)
            return nullptr;
        if ((*exp)->v.func->sp->tp->syms)
        {
            SymbolTable<SYMBOL>* syms = (*exp)->v.func->sp->tp->syms;
            itp = syms->begin();
            if (itp != syms->end() && (*itp)->tp->syms)
            {
                itpe = (*itp)->tp->syms->end();
                itp = (*itp)->tp->syms->begin();
                found = true;
            }
        }
    }
    while (castvalue(exp2))
        exp2 = exp2->left;

    memset(&fpargs, 0, sizeof(fpargs));
    if (found && (*itp)->sb->thisPtr)
    {
        fpargs.thistp = (*itp)->tp;
        fpargs.thisptr = intNode(en_c_i, 0);
        ++itp;
    }
    else if (tp->type == bt_memberptr)
    {
        fpargs.thistp = MakeType(bt_pointer, tp->sp->tp);
        fpargs.thisptr = intNode(en_c_i, 0);
    }
    if (found)
    {
        while (itp != itpe)
        {
            auto il = Allocate<INITLIST>();
            il->tp = ((*itp))->tp;
            il->exp = intNode(en_c_i, 0);
            if (isref(il->tp))
                il->tp = basetype(il->tp)->btp;
            if (!fpargs.arguments)
                fpargs.arguments = initListListFactory.CreateList();
            fpargs.arguments->push_back(il);
            ++itp;
        }
    }
    if (exp2 && exp2->type == en_func)
        fpargs.templateParams = exp2->v.func->templateParams;
    fpargs.ascall = true;
    return GetOverloadedFunction(mtp, exp, sym, &fpargs, nullptr, true, false, flags);
}
}  // namespace Parser