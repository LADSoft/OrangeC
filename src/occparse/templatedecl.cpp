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

#include "compiler.h"
#include "assert.h"
#include <stack>
#include "PreProcessor.h"
#include <malloc.h>
#include "ccerr.h"
#include "initbackend.h"
#include "declare.h"
#include "declcpp.h"
#include "stmt.h"
#include "expr.h"
#include "lambda.h"
#include "occparse.h"
#include "help.h"
#include "cpplookup.h"
#include "mangle.h"
#include "lex.h"
#include "constopt.h"
#include "memory.h"
#include "init.h"
#include "rtti.h"
#include "declcons.h"
#include "exprcpp.h"
#include "inline.h"
#include "beinterf.h"
#include "types.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "libcxx.h"
#include "constexpr.h"
#include "symtab.h"
#include "ListFactory.h"
namespace Parser
{

int dontRegisterTemplate;
int instantiatingTemplate;
int inTemplateBody;
int templateNestingCount = 0;
int templateHeaderCount;
int inTemplateSpecialization = 0;
int inDeduceArgs;
bool parsingSpecializationDeclaration;
bool inTemplateType;
int noTypeNameError;
int inTemplateHeader;
SYMBOL* instantiatingMemberFuncClass;
int instantiatingFunction;
int instantiatingClass;
int parsingDefaultTemplateArgs;
int count1;
int inTemplateArgs;
bool fullySpecialized;

int templateNameTag;
std::unordered_map<SYMBOL*, std::unordered_map<std::string, SYMBOL*, StringHash>> classTemplateMap;
std::unordered_map<std::string, SYMBOL*, StringHash> classTemplateMap2;
std::unordered_map<std::string, SYMBOL*, StringHash> classInstantiationMap;

struct templateListData* currents;

void templateInit(void)
{
    inTemplateBody = false;
    inTemplateHeader = false;
    templateNestingCount = 0;
    templateHeaderCount = 0;
    instantiatingTemplate = 0;
    instantiatingClass = 0;
    currents = nullptr;
    inTemplateArgs = 0;
    inTemplateType = false;
    dontRegisterTemplate = 0;
    inTemplateSpecialization = 0;
    instantiatingMemberFuncClass = nullptr;
    parsingSpecializationDeclaration = false;
    instantiatingFunction = 0;
    parsingDefaultTemplateArgs = 0;
    inDeduceArgs = 0;
    classTemplateMap.clear();
    classTemplateMap2.clear();
    classInstantiationMap.clear();
    templateNameTag = 1;
    fullySpecialized = false;
}
void TemplateGetDeferred(SYMBOL* sym)
{
    if (currents)
    {
        if (currents->bodyHead)
        {
            sym->sb->deferredCompile = currents->bodyHead;
            for (auto v = currents->bodyHead; v; v = v->next)
                v->data->registered = false;
        }
    }
}
TEMPLATEPARAMPAIR* TemplateLookupSpecializationParam(const char* name)
{
    if (parsingSpecializationDeclaration && currents->args)
        for (auto&& search : *currents->args)
            if (search.first && !strcmp(search.first->name, name))
                return &search;
    return nullptr;
}
std::list<TEMPLATEPARAMPAIR>* TemplateGetParams(SYMBOL* sym)
{
    std::list<TEMPLATEPARAMPAIR>* params = nullptr;
    if (currents)
    {
        int n = -1;
        params = (std::list<TEMPLATEPARAMPAIR>*)(*currents->plast);
        while (sym)
        {
            if (sym->sb->templateLevel && !sym->sb->instantiated)
                n++;
            sym = sym->sb->parentClass;
        }
        if (n > 0 && params)
        {
            while (n-- && params->front().second->bySpecialization.next)
            {
                params = params->front().second->bySpecialization.next;
            }
        }
    }
    if (!params)
    {
        params = templateParamPairListFactory.CreateList();
        params->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
    }
    return params;
}
void TemplateRegisterDeferred(LEXLIST* lex)
{
    if (lex && templateNestingCount && !dontRegisterTemplate)
    {
        if (!lex->data->registered)
        {
            LEXLIST* cur = globalAllocate<LEXLIST>();
            if (lex->data->type == l_id)
                lex->data->value.s.a = litlate(lex->data->value.s.a);
            *cur = *lex;
            cur->next = nullptr;
            if (inTemplateBody)
            {
                if (currents->bodyHead)
                {
                    cur->prev = currents->bodyTail;
                    currents->bodyTail = currents->bodyTail->next = cur;
                }
                else
                {
                    cur->prev = nullptr;
                    currents->bodyHead = currents->bodyTail = cur;
                }
            }
            else
            {
                if (currents->head)
                {
                    cur->prev = currents->tail;
                    currents->tail = currents->tail->next = cur;
                }
                else
                {
                    cur->prev = nullptr;
                    currents->head = currents->tail = cur;
                }
            }
            lex->data->registered = true;
        }
    }
}
static std::list<TEMPLATEPARAMPAIR>** expandArgs(std::list<TEMPLATEPARAMPAIR>** lst, LEXLIST* start, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* select, bool packable)
{
    int beginning = 0;
    if (*lst)
        beginning = (*lst)->size();
    // this is going to presume that the expression involved
    // is not too long to be cached by the LEXLIST mechanism.
    int oldPack = packIndex;
    int count = 0;
    TEMPLATEPARAMPAIR* arg[500];
    if (!packable)
    {
        if (!*lst)
            *lst = templateParamPairListFactory.CreateList();
        if (select->front().second->packed && packIndex >= 0)
        {
            std::list<TEMPLATEPARAMPAIR>* templateParam = select->front().second->byPack.pack;
            int i;
            auto it = select->front().second->byPack.pack->begin();
            auto ite = select->front().second->byPack.pack->end();
            for (i = 0; i < packIndex && it != ite; i++, ++it);
            if (it != ite)
            {
                (*lst)->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
                *(*lst)->back().second = *it->second;
                (*lst)->back().second->ellipsis = false;
                (*lst)->back().second->byClass.dflt = (*lst)->back().second->byClass.val;
                return lst;
            }
        }
        if (select->front().second->ellipsis)
        {
            (*lst)->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
            *(*lst)->back().second = *select->front().second;
            (*lst)->back().second->ellipsis = false;
        }
        else
        {
            (*lst)->push_back(TEMPLATEPARAMPAIR(nullptr, select->front().second));
        }
        (*lst)->back().first = select->front().first;
        return lst;
    }
    std::list<TEMPLATEPARAMPAIR> temp = { {select->front().first, select->front().second } };
    GetPackedTypes(arg, &count, &temp);
    expandingParams++;
    if (count)
    {
        int i;
        int n = CountPacks(arg[0]->second->byPack.pack);  // undefined in local context
        for (i = 1; i < count; i++)
        {
            if (CountPacks(arg[i]->second->byPack.pack) != n)
            {
                error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                break;
            }
        }
        for (i = 0; i < n; i++)
        {
            LEXLIST* lex = SetAlternateLex(start);
            TYPE* tp;
            packIndex = i;
            lex = get_type_id(lex, &tp, funcsp, StorageClass::parameter_, false, true, false);
            SetAlternateLex(nullptr);
            if (tp)
            {
                if (!*lst)
                    *lst = templateParamPairListFactory.CreateList();
                (*lst)->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
                (*lst)->back().second->type = TplType::typename_;
                (*lst)->back().second->byClass.dflt = tp;
            }
        }
    }
    else if (select)
    {
        if (!*lst)
            *lst = templateParamPairListFactory.CreateList();
        (*lst)->push_back(select->front());
    }
    expandingParams--;
    packIndex = oldPack;
    // make it packed again...   we aren't flattening at this point.
    if (select->front().second->packed)
    {
        TEMPLATEPARAMPAIR next;
        next.second = Allocate<TEMPLATEPARAM>();
        *next.second = *select->front().second;
        next.first = select->front().first;
        if (*lst && beginning != (*lst)->size())
        {
            next.second->byPack.pack = templateParamPairListFactory.CreateList();
            auto itbeginning = (*lst)->begin();
            int n = (*lst)->size() - beginning;
            while (beginning--)
                ++itbeginning;
            (*next.second->byPack.pack) = std::move(std::list<TEMPLATEPARAMPAIR>(itbeginning, (*lst)->end()));
            while (n--)
                (*lst)->pop_back();
        }
        next.second->resolved = true;
        if (!*lst)
            (*lst) = templateParamPairListFactory.CreateList();
        (*lst)->push_back(next);
    }
    return lst;
}
void UnrollTemplatePacks(std::list<TEMPLATEPARAMPAIR>* tplx)
{ 
    if (tplx)
    {
        for (auto&& tpx : *tplx)
        {
            if (tpx.second->type == TplType::typename_)
            {
                auto tpl2 = tpx;
                if (tpx.second->packed)
                {
                    if (tpx.second->byPack.pack && tpx.second->byPack.pack->size() == 1 && tpx.second->byPack.pack->front().second->packed)
                        tpx.second->byPack.pack = tpx.second->byPack.pack->front().second->byPack.pack;
                }
                else if (tpx.second->byClass.dflt)
                {
                    auto quals = tpx.second->byClass.dflt;
                    auto end = quals;
                    while (end->btp)
                        end = end->btp;
                    if (end->type == BasicType::templateparam_)
                    {
                        auto ths = end->templateParam;
                        if (ths->second->packed)
                        {
                            auto tpl2 = ths->second->byPack.pack;
                            if (tpl2)
                            {
                                tpx.second = ths->second;
                                if (quals != end)
                                {
                                    for (auto&& lst :*tpl2)
                                    {
                                        TYPE* hold = CopyType(quals, true, [&lst](TYPE*& old, TYPE*& newx) {
                                            if (newx->type == BasicType::templateparam_)
                                                newx = lst.second->byClass.val ? lst.second->byClass.val : lst.second->byClass.dflt;
                                            });
                                        UpdateRootTypes(hold);
                                        CollapseReferences(hold);
                                        lst.second->byClass.dflt = hold;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
static std::list<TEMPLATEPARAMPAIR>* nextExpand(std::list<TEMPLATEPARAMPAIR>* inx, int n)
{
    bool packed = false;
    if (inx)
    {
        for (auto&& t : *inx)
        {
            if (t.second->packed)
            {
                packed = true;
                break;
            }
        }
    }
    if (!packed)
    {
        if (!n)
            return inx;
        return nullptr;
    }
    bool found = false;
    if (inx)
    {
        std::list<TEMPLATEPARAMPAIR>* out = templateParamPairListFactory.CreateList();
        for (auto&& in : *inx)
        {
            out->push_back(TEMPLATEPARAMPAIR{ in.first, in.second });
            if (in.second->packed && in.second->byPack.pack)
            {
                out->back().second = Allocate<TEMPLATEPARAM>();
                out->back().second->type = in.second->type;
                out->back().second->packed = true;
                auto it = in.second->byPack.pack->begin();
                auto ite = in.second->byPack.pack->end();
                for (; n && it != ite; --n, ++it);
                if (it != ite)
                {
                    found = true;
                    out->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                    out->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{ nullptr, it->second });
                }
            }
        }
        return found ? out : nullptr;
    }
    return nullptr;
}
std::list<TEMPLATEPARAMPAIR>** expandTemplateSelector(std::list<TEMPLATEPARAMPAIR>** lst, std::list<TEMPLATEPARAMPAIR>* orig, TYPE* tp)
{
    if (tp->sp && tp->sp->sb->templateSelector)
    {
        TEMPLATEPARAMPAIR* cptr = nullptr;

        for (auto&& s : structSyms)
        {
            if (s.tmpl)
            {
                SYMBOL* sym = templatesearch((*tp->sp->sb->templateSelector)[1].sp->name, s.tmpl);
                if (sym)
                {
                    cptr = sym->tp->templateParam;
                    if (cptr)
                        break;
                }
            }
        }
        // i'm not sure why this first clause is in here.   I couldn't find anything we compile that uses it.
        // i'm adding necessary things in the if's else.
        if (cptr && cptr->second->packed && cptr->second->byPack.pack)
        {
            auto xlst = cptr->second->byPack.pack;
            if (!*lst)
                *lst = templateParamPairListFactory.CreateList();
            (*lst)->push_back(TEMPLATEPARAMPAIR(orig->front().first, Allocate<TEMPLATEPARAM>()));
            (*lst)->back().second->type = TplType::typename_;
            (*lst)->back().second->packed = true;
            std::list<TEMPLATEPARAMPAIR> *last = nullptr;
            for (auto&& clst : *xlst)
            {
                auto sel = (*tp->sp->sb->templateSelector).begin();
                auto sele = (*tp->sp->sb->templateSelector).end();
                TYPE* base = clst.second->byClass.val, *base1 = base;
                SYMBOL* s = nullptr;
                for ( ; sel != sele; ++ sel)
                {
                    base = base1;
                    s = search(base->syms, sel->name);
                    auto it1 = sel;
                    ++it1;
                    // it was written this way but could be optimized
                    if (!s || (s && it1 != sele && !isstructured(s->tp)))
                        break;
                    base = basetype(s->tp);
                }
                if (sel == sele && s)
                {
                    if (!last)
                        last = templateParamPairListFactory.CreateList();
                    last->push_back(TEMPLATEPARAMPAIR(nullptr, Allocate<TEMPLATEPARAM>()));
                    if (s->sb->storage_class == StorageClass::constant_ || s->sb->storage_class == StorageClass::constexpr_ ||
                        s->sb->storage_class == StorageClass::enumconstant_)
                    {
                        (*lst)->back().second->type = last->back().second->type = TplType::int_;
                        last->back().second->byNonType.dflt = s->sb->init->front()->exp;
                        last->back().second->byNonType.tp = s->tp;
                    }
                    else
                    {
                        (*lst)->back().second->type = last->back().second->type = TplType::typename_;
                        last->back().second->byClass.dflt = s->tp;
                    }
                }
            }
            (*lst)->back().second->byPack.pack = last;
        }
        else
        {
            if (templateNestingCount && (!inTemplateBody || !instantiatingTemplate))
            {
                if (!*lst)
                    *lst = templateParamPairListFactory.CreateList();
                (*lst)->push_back(TEMPLATEPARAMPAIR(orig->front().first, Allocate<TEMPLATEPARAM>()));
                (*lst)->back().second->type = TplType::typename_;
                (*lst)->back().second->byClass.dflt = tp;
            }
            else if (!templateNestingCount && (*tp->sp->sb->templateSelector)[1].isTemplate)
            {
                if (!*lst)
                    *lst = templateParamPairListFactory.CreateList();
                (*lst)->push_back(TEMPLATEPARAMPAIR(orig->front().first, Allocate<TEMPLATEPARAM>()));
                (*lst)->back().second->type = TplType::typename_;
                (*lst)->back().second->packed = true;
                std::list<TEMPLATEPARAMPAIR>* last = nullptr;
                for (int i = 0; i < INT_MAX; i++)
                {
                    auto clst = nextExpand((*tp->sp->sb->templateSelector)[1].templateParams, i);
                    if (!clst)
                    {
                        break;
                    }
                    SYMBOL* sp = GetClassTemplate((*tp->sp->sb->templateSelector)[1].sp, clst, false);
                    if (sp)
                    {
                        sp = TemplateClassInstantiate(sp, clst, false, StorageClass::global_);
                    }
                    if (sp)
                    {
                        auto sel = (*tp->sp->sb->templateSelector).begin();
                        auto sele = (*tp->sp->sb->templateSelector).end();
                        TYPE* base = sp->tp, *base1 = base;
                        SYMBOL* s = nullptr;
                        for (++sel, ++sel; sel != sele; ++sel)
                        {
                            base = base1;
                            STRUCTSYM ss;
                            ss.str = basetype(base)->sp;
                            addStructureDeclaration(&ss);
                            s = classsearch(sel->name, false, false, false);
                            dropStructureDeclaration();
                            auto it1 = sel;
                            ++it1;
                            // could be optimized
                            if (!s || (s && it1 != sele && !isstructured(s->tp)))
                                break;
                            base1 = basetype(s->tp);
                        }
                        if (sel == sele && s)
                        {
                            if (!last)
                                last = templateParamPairListFactory.CreateList();
                            last->push_back(TEMPLATEPARAMPAIR(nullptr, Allocate<TEMPLATEPARAM>()));
                            if (s->sb->storage_class == StorageClass::constant_ || s->sb->storage_class == StorageClass::constexpr_ ||
                                s->sb->storage_class == StorageClass::enumconstant_)
                            {
                                (*lst)->back().second->type = last->back().second->type = TplType::int_;
                                last->back().second->byNonType.dflt = s->sb->init->front()->exp;
                                last->back().second->byNonType.tp = s->tp;
                            }
                            else
                            {
                                (*lst)->back().second->type = last->back().second->type = TplType::typename_;
                                last->back().second->byClass.dflt = s->tp;
                            }
                        }
                    }
                }
                (*lst)->back().second->byPack.pack = last;
            }
        }
    }
    return lst;
}

bool constructedInt(LEXLIST* lex, SYMBOL* funcsp)
{
    // depends on this starting a type
    bool rv = false;
    TYPE* tp;
    LEXLIST* placeholder = lex;
    Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    bool defd = false;
    bool notype = false;
    bool cont = false;
    tp = nullptr;

    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
    if (lex->data->type == l_id || MATCHKW(lex, Keyword::classsel_))
    {
        SYMBOL *sp, *strSym = nullptr;
        LEXLIST* placeholder = lex;
        bool dest = false;
        nestedSearch(lex, &sp, &strSym, nullptr, &dest, nullptr, false, StorageClass::global_, false, false);
        if (Optimizer::cparams.prm_cplusplus)
            prevsym(placeholder);
        if (sp && sp->sb && sp->sb->storage_class == StorageClass::typedef_)
            cont = true;
    }
    else
    {
        if (KWTYPE(lex, TT_BASETYPE))
            cont = true;
    }
    if (cont)
    {
        lex = getBasicType(lex, funcsp, &tp, nullptr, false, funcsp ? StorageClass::auto_ : StorageClass::global_, &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr, false, false,
                           false, false, false);
        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
        if (isint(tp))
        {
            if (MATCHKW(lex, Keyword::openpa_))
            {
                rv = true;
            }
        }
    }
    lex = prevsym(placeholder);
    return rv;
}
LEXLIST* GetTemplateArguments(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* templ, std::list<TEMPLATEPARAMPAIR>** lst)
{
    std::list<TEMPLATEPARAMPAIR>** start = lst;
    int oldnoTn = noTypeNameError;
    noTypeNameError = 0;
    bool first = true;
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    std::list<TEMPLATEPARAMPAIR>::iterator itorig, iteorig = itorig;
    if (templ)
    {
        auto templ1 = templ;
        if (templ1->sb && templ1->sb->maintemplate)
            templ1 = templ1->sb->maintemplate;
        if (templ->tp && templ1->tp->type == BasicType::aggregate_)
            templ1 = templ1->tp->syms->front();
        if (templ1->templateParams)
        {
            if (templ1->templateParams->front().second->bySpecialization.types)
            {
                itorig = templ1->templateParams->front().second->bySpecialization.types->begin();
                iteorig = templ1->templateParams->front().second->bySpecialization.types->end();
            }
            else
            {
                itorig = templ1->templateParams->begin();
                iteorig = templ1->templateParams->end();
                ++itorig;
            }
        }
    }
    // entered with lex set to the opening <
    inTemplateArgs++;
    lex = getsym();
    if (!MATCHKW(lex, Keyword::rightshift_) && !MATCHKW(lex, Keyword::gt_))
    {
        do
        {
            tp = nullptr;
            if (MATCHKW(lex, Keyword::typename_) || (((itorig != iteorig && itorig->second->type != TplType::int_) ||
                                               (itorig == iteorig && startOfType(lex, nullptr, true) && !constructedInt(lex, funcsp))) &&
                                              !MATCHKW(lex, Keyword::sizeof_)))
            {
                LEXLIST* start = lex;
                noTypeNameError++;
                int old = noNeedToSpecialize;
                noNeedToSpecialize = itorig != iteorig && itorig->second->type == TplType::template_;
                lex = get_type_id(lex, &tp, funcsp, StorageClass::parameter_, false, true, false);
                noNeedToSpecialize = old;
                noTypeNameError--;
                if (!tp)
                    tp = &stdint;
                if (structLevel && isstructured(tp))
                {
                    if ((!basetype(tp)->sp->sb->templateLevel || basetype(tp)->sp->sb->instantiating) &&
                        basetype(tp)->sp->sb->declaring)
                    {
                        basetype(tp)->sp->sb->declaringRecursive = true;
                    }
                }
                if (!templateNestingCount && tp->type == BasicType::any_)
                {
                    error(ERR_UNKNOWN_TYPE_TEMPLATE_ARG);
                }
                else if (tp && !templateNestingCount)
                {
                    tp = PerformDeferredInitialization(tp, nullptr);
                }
                if (MATCHKW(lex, Keyword::begin_))  // initializer list?
                {
                    if (templateNestingCount)
                    {
                        exp = exprNode(ExpressionNode::construct_, nullptr, nullptr);
                        exp->v.construct.tp = tp;
                        lex = getDeferredData(lex, &exp->v.construct.deferred, true);
                    }
                    else
                    {
                        lex = expression_func_type_cast(lex, funcsp, &tp, &exp, _F_NOEVAL);
                    }
                    goto initlistjoin;
                    // makes it an expression
                }
                if (MATCHKW(lex, Keyword::ellipse_))
                {
                    lex = getsym();
                    if (tp)
                    {
                        TYPE* tp1 = tp;
                        while (isref(tp))
                            tp = basetype(tp)->btp;
                        if (templateNestingCount && tp->type == BasicType::templateparam_)
                        {
                            if (!*lst)
                                *lst = templateParamPairListFactory.CreateList();
                            (*lst)->push_back(TEMPLATEPARAMPAIR{tp->templateParam->first, tp->templateParam->second});
                            if (isref(tp1))
                            {
                                if (basetype(tp1)->type == BasicType::lref_)
                                {
                                    (*lst)->back().second->lref = true;
                                    (*lst)->back().second->rref = false;
                                }
                                else
                                {
                                    (*lst)->back().second->rref = true;
                                    (*lst)->back().second->lref = false;
                                }
                            }
                            if (inTemplateSpecialization && !tp->templateParam->second->packed)
                                error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                        }
                        else if (tp->type == BasicType::templateparam_)
                        {
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back(*tp->templateParam);
                            lst = expandArgs(lst, start, funcsp, &a, true);
                        }
                        else if (tp->type == BasicType::templateselector_)
                        {
                            tp->sp->sb->postExpansion = true;
                            std::list<TEMPLATEPARAMPAIR> a(itorig, iteorig);
                            lst = expandTemplateSelector(lst, &a, tp1);
                        }
                        else if (itorig != iteorig && itorig->second->type == TplType::typename_ && itorig->second->packed && isstructured(tp))
                        {
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            a.back().second->type = TplType::typename_;
                            a.back().second->byClass.dflt = tp1;
                            lst = expandArgs(lst, start, funcsp, &a, true);
                        }
                        else
                        {
                            if (!*lst)
                                *lst = templateParamPairListFactory.CreateList();
                            (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            if (itorig != iteorig && itorig->second->type == TplType::template_ && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                            {
                                (*lst)->back().second->type = TplType::template_;
                                (*lst)->back().second->byTemplate.dflt = basetype(tp)->sp;
                            }
                            else
                            {
                                (*lst)->back().second->type = TplType::typename_;
                                (*lst)->back().second->byClass.dflt = tp1;
                            }
                        }
                    }
                    (*lst)->back().second->ellipsis = true;
                    if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                    {
                        for (auto&& tpx : *(*lst)->back().second->byPack.pack)
                            tpx.second->ellipsis = true;
                    }
                }
                else if (tp && tp->type == BasicType::templateparam_)
                {
                    if (inTemplateArgs > 1 && tp->templateParam->second->packed)
                    {
                        // unpacked pack gets treated as a single template param
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(TEMPLATEPARAMPAIR{tp->templateParam->first, Allocate<TEMPLATEPARAM>()});
                        *(*lst)->back().second = *tp->templateParam->second;
                        (*lst)->back().second->ellipsis = false;
                        (*lst)->back().second->usedAsUnpacked = true;
                    }
                    else if (inTemplateSpecialization)
                    {
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(*tp->templateParam);
                        if (tp->templateParam->second->packed)
                        {
                            if (tp->templateParam->second->byPack.pack)
                                for (auto&& find : *tp->templateParam->second->byPack.pack)
                                    if (!find.second->byClass.dflt)
                                        find.second->byClass.dflt = tp;
                        }
                        else
                        {
                            if (!tp->templateParam->second->byClass.dflt)
                            {
                                if (tp->templateParam->second->type == TplType::template_)
                                    tp->templateParam->second->byTemplate.dflt = basetype(tp)->sp;
                                else
                                    tp->templateParam->second->byClass.dflt = tp;
                            }
                        }
                    }
                    else
                    {
                        std::list<TEMPLATEPARAMPAIR> a;
                        a.push_back(*tp->templateParam);
                        lst = expandArgs(lst, start, funcsp, &a, false);
                    }
                }
                else if (itorig != iteorig && itorig->second->packed)
                {
                    if (first)
                    {
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                        if (itorig != iteorig && itorig->second->type == TplType::template_ && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                        {
                            (*lst)->back().second->type = TplType::template_;
                            (*lst)->back().second->packed = true;
                        }
                        else
                        {
                            (*lst)->back().second->type = TplType::typename_;
                            (*lst)->back().second->packed = true;
                        }
                        if (itorig != iteorig)
                            (*lst)->back().first = itorig->first;
                        first = false;
                    }
                    if (!(*lst)->back().second->byPack.pack)
                        (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                    auto last = (*lst)->back().second->byPack.pack;
                    last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                    if (itorig != iteorig && itorig->second->type == TplType::template_ && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                    {
                        last->back().second->type = TplType::template_;
                        last->back().second->byTemplate.dflt = basetype(tp)->sp;
                    }
                    else
                    {
                        last->back().second->type = TplType::typename_;
                        last->back().second->byClass.dflt = tp;
                    }
                    if (last->back().second->type != (*lst)->back().second->type)
                    {
                        (*lst)->back().second->byPack.pack = nullptr;
                        error(ERR_PACKED_TEMPLATE_TYPE_MISMATCH);
                    }
                }
                else
                {
                    if (!*lst)
                        *lst = templateParamPairListFactory.CreateList();
                    (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});

                    if (itorig != iteorig)
                        (*lst)->back().first = itorig->first;
                    if (itorig != iteorig && itorig->second->type == TplType::template_ && isstructured(tp) && basetype(tp)->sp->sb->templateLevel)
                    {
                        (*lst)->back().second->type = TplType::template_;
                        (*lst)->back().second->byTemplate.dflt = basetype(tp)->sp;
                    }
                    else
                    {
                        (*lst)->back().second->type = TplType::typename_;
                        (*lst)->back().second->byClass.dflt = tp;
                    }
                }
            }
            else
            {
                exp = nullptr;
                tp = nullptr;
                if (inTemplateSpecialization)
                {
                    if (lex->data->type == l_id)
                    {
                        SYMBOL* sp;
                        LEXLIST* last = lex;
                        lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, false, false);
                        if (sp && sp->tp->templateParam)
                        {
                            lex = getsym();
                            if (!MATCHKW(lex, Keyword::rightshift_) && !MATCHKW(lex, Keyword::gt_) && !MATCHKW(lex, Keyword::comma_))
                            {
                                lex = prevsym(last);
                                goto join;
                            }
                            else
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, sp->tp->templateParam->second});
                            }
                        }
                        else
                        {
                            lex = prevsym(last);
                            goto join;
                        }
                    }
                    else
                    {
                        goto join;
                    }
                }
                else
                {
                    SYMBOL* name;
                    LEXLIST* start;
                    bool skip;
                join:
                    skip = false;
                    name = nullptr;
                    start = lex;
                    if (ISID(lex))
                    {
                        for (auto&& s : structSyms)
                        {
                            if (s.tmpl)
                            {
                                name = templatesearch(lex->data->value.s.a, s.tmpl);
                                if (name)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    if (name)
                    {
                        if (name->tp->type == BasicType::templateparam_)
                        {
                            bool found = true;
                            lex = getsym();

                            if (MATCHKW(lex, Keyword::classsel_))
                            {
                                std::list<NAMESPACEVALUEDATA*>* nsv;
                                lex = prevsym(start);
                                lex = nestedPath(lex, &name, &nsv, nullptr, false, StorageClass::parameter_, false, 0);
                                if (name && name->tp->type == BasicType::templateselector_)
                                {
                                    lex = getsym();
                                    if (MATCHKW(lex, Keyword::ellipse_))
                                    {
                                        lex = getsym();
                                        std::list<TEMPLATEPARAMPAIR> a(itorig, iteorig);
                                        lst = expandTemplateSelector(lst, &a, tp);
                                        (*lst)->back().second->ellipsis = true;
                                        if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                                        {
                                            for (auto&& tpl1 : *(*lst)->back().second->byPack.pack)
                                                tpl1.second->ellipsis = true;
                                        }
                                        skip = true;
                                        first = false;
                                    }
                                    else
                                    {
                                        lex = prevsym(start);
                                    }
                                }
                                else
                                {
                                    lex = prevsym(start);
                                }
                            }
                            else if (name->tp->templateParam->second->type == TplType::int_)
                            {
                                if (MATCHKW(lex, Keyword::ellipse_))
                                {
                                    {
                                        if (!*lst)
                                            *lst = templateParamPairListFactory.CreateList();
                                        (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                        *(*lst)->back().second = *name->tp->templateParam->second;
                                        (*lst)->back().second->packed = true;
                                        auto last = (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList(); 
                                        if (name->tp->templateParam->second->packed)
                                        {
                                            if (name->tp->templateParam->second->byPack.pack)
                                                for (auto&& tpx : *name->tp->templateParam->second->byPack.pack)
                                                {
                                                    last->push_back(TEMPLATEPARAMPAIR{nullptr, tpx.second});
                                                }
                                        }
                                        else
                                        {
                                            last->push_back(TEMPLATEPARAMPAIR(nullptr, name->tp->templateParam->second));
                                        }
                                        lex = getsym();
                                        skip = true;
                                        first = false;
                                    }
                                }
                                else
                                {
                                    lex = prevsym(start);
                                }
                            }
                            else
                            {
                                lex = prevsym(start);
                            }
                        }
                        if (!skip)
                        {
                            lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
                            if (tp && tp->type == BasicType::templateparam_)
                            {
                                if (parsingTrailingReturnOrUsing)
                                {
                                    std::list<TEMPLATEPARAMPAIR> a;
                                    a.push_back(*exp->v.sp->tp->templateParam);
                                    lst = expandArgs(lst, start, funcsp, &a, false);
                                }
                                else
                                {
                                    std::list<TEMPLATEPARAMPAIR> a;
                                    a.push_back(*tp->templateParam);
                                    lst = expandArgs(lst, start, funcsp, &a, false);
                                }
                                skip = true;
                                first = false;
                            }
                        }
                    }
                    else
                    {
                        lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
                        if (!tp)
                        {
                            error(ERR_EXPRESSION_SYNTAX);
                        }
                    }
                    if (MATCHKW(lex, Keyword::begin_))
                    {
                        error(ERR_EXPECTED_TYPE_NEED_TYPENAME);
                        lex = getsym();
                        errskim(&lex, skim_end);
                        if (lex)
                            needkw(&lex, Keyword::end_);
                    }
                    if (!skip)
                    {
                        if (0)
                        {
                        initlistjoin:
                            start = nullptr;
                            name = nullptr;
                        }
                        if (MATCHKW(lex, Keyword::ellipse_))
                        {
                            // lose p
                            std::list<TEMPLATEPARAMPAIR>** tpx = lst;
                            lex = getsym();
                            if (templateNestingCount && tp->type == BasicType::templateparam_)
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                *(*lst)->back().second = *tp->templateParam->second;
                                if (!tp->templateParam->second->packed)
                                {
                                    error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                                }
                                else
                                {
                                    (*lst)->back().second->packed = false;
                                    (*lst)->back().second->ellipsis = true;
                                    (*lst)->back().second->byNonType.dflt = exp;
                                    (*lst)->back().second->byNonType.tp = tp;
                                }
                            }
                            else if (templateNestingCount)
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                (*lst)->back().second->packed = true;
                                (*lst)->back().second->type = TplType::int_;
                                if (itorig != iteorig)
                                    (*lst)->back().first = itorig->first;
                                auto last = (*lst)->back().second->bySpecialization.types =
                                    templateParamPairListFactory.CreateList();
                                last->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                last->back().second->type = TplType::int_;
                                last->back().second->byNonType.tp = tp;
                                last->back().second->byNonType.dflt = exp;
                            }
                            else if (exp->type != ExpressionNode::packedempty_)
                            {
                                // this is going to presume that the expression involved
                                // is not too long to be cached by the LEXLIST mechanism.
                                int oldPack = packIndex;
                                int count = 0;
                                SYMBOL* arg[200];
                                GatherPackedVars(&count, arg, exp);
                                expandingParams++;
                                if (count)
                                {
                                    int i;
                                    int n = CountPacks(arg[0]->tp->templateParam->second->byPack.pack);
                                    for (i = 1; i < count; i++)
                                    {
                                        if (CountPacks(arg[i]->tp->templateParam->second->byPack.pack) != n)
                                        {
                                            error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                                            break;
                                        }
                                    }
                                    for (i = 0; i < n; i++)
                                    {
                                        LEXLIST* lex = SetAlternateLex(start);
                                        packIndex = i;
                                        expression_assign(lex, funcsp, nullptr, &tp, &exp, nullptr, _F_PACKABLE);
                                        if (exp)
                                        {
                                            optimize_for_constants(&exp);
                                            while (exp->type == ExpressionNode::void_ && exp->right)
                                                exp = exp->right;
                                        }
                                        SetAlternateLex(nullptr);
                                        if (tp)
                                        {
                                            if (!*lst)
                                                *lst = templateParamPairListFactory.CreateList();
                                            (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                            (*lst)->back().second->type = TplType::int_;
                                            (*lst)->back().second->byNonType.dflt = exp;
                                            (*lst)->back().second->byNonType.tp = tp;
                                            if (itorig != iteorig)
                                                (*lst)->back().first = itorig->first;
                                        }
                                    }
                                }
                                expandingParams--;
                                packIndex = oldPack;
                            }
                            if (*lst)
                            {
                                (*lst)->back().second->ellipsis = true;
                                if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                                {
                                    for (auto&& tpl1 : *(*tpx)->back().second->byPack.pack)
                                        tpl1.second->ellipsis = true;
                                }
                            }
                        }
                        else
                        {
                            if (exp)
                            {
                                optimize_for_constants(&exp);
                                while (exp->type == ExpressionNode::void_ && exp->right)
                                    exp = exp->right;
                            }
                            if (tp && (int)tp->type == (int)ExpressionNode::templateparam_)
                            {
                                if (!*lst)
                                    *lst = templateParamPairListFactory.CreateList();
                                (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                *(*lst)->back().second = *tp->templateParam->second;
                                (*lst)->back().second->ellipsis = false;

                                if ((*lst)->back().second->packed)
                                {
                                    auto last = (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                    last->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                    last->back().second->type = TplType::int_;
                                    last->back().second->byNonType.dflt = exp;
                                    last->back().second->byNonType.val = nullptr;
                                }
                                else
                                {

                                    (*lst)->back().second->type = TplType::int_;
                                    (*lst)->back().second->byNonType.dflt = exp;
                                    (*lst)->back().second->byNonType.val = nullptr;
                                    (*lst)->back().second->byNonType.tp = tp;
                                }
                                if (exp->type == ExpressionNode::templateparam_)
                                {
                                    (*lst)->back().first = exp->v.sp->tp->templateParam->first;
                                }
                                else if (itorig != iteorig)
                                {
                                    (*lst)->back().first = itorig->first;
                                }
                            }
                            else {
                                /*
                                * some of the stuff in chrono is too complex for the compiler right now..
                                if (exp && !IsConstantExpression(exp, false, false)&& (!templateNestingCount ||
                                instantiatingTemplate)) error(ERR_CONSTANT_VALUE_EXPECTED);
                                */
                                if (itorig != iteorig && itorig->second->packed)
                                {
                                    std::list<TEMPLATEPARAMPAIR>** p;
                                    if (first)
                                    {
                                        if (!*lst)
                                            *lst = templateParamPairListFactory.CreateList();
                                        (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                        (*lst)->back().second->type = TplType::int_;
                                        (*lst)->back().second->packed = true;
                                        if (itorig != iteorig)
                                            (*lst)->back().first = itorig->first;
                                        first = false;
                                        (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                    }
                                    auto last = (*lst)->back().second->byPack.pack;
                                    last->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                    last->back().second->type = TplType::int_;
                                    last->back().second->byNonType.dflt = exp;
                                    last->back().second->byNonType.tp = tp;
                                }
                                else
                                {
                                    //                                checkUnpackedExpression(exp);
                                    if (!*lst)
                                        *lst = templateParamPairListFactory.CreateList();
                                    (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                    if (parsingTrailingReturnOrUsing && exp->type == ExpressionNode::templateparam_ && exp->v.sp) 
                                    {
                                        (*lst)->back().first = exp->v.sp;
                                    }
                                    else if (itorig != iteorig)
                                    {
                                        (*lst)->back().first = itorig->first;
                                    }
                                    if (exp->type == ExpressionNode::templateparam_ && exp->v.sp->tp->templateParam->second->byClass.dflt)
                                    {
                                        *(*lst)->back().second = *exp->v.sp->tp->templateParam->second;
                                    }
                                    else
                                    {
                                        (*lst)->back().second->type = TplType::int_;
                                        (*lst)->back().second->byNonType.dflt = exp;
                                        (*lst)->back().second->byNonType.tp = tp;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (MATCHKW(lex, Keyword::comma_))
                lex = getsym();
            else
                break;
            if (itorig != iteorig && !itorig->second->packed)
            {
                ++itorig;
            }
        } while (true);
    }
    if (MATCHKW(lex, Keyword::rightshift_))
    {
        lex = getGTSym(lex);
    }
    else
    {
        if (!MATCHKW(lex, Keyword::gt_) && (tp && tp->type == BasicType::any_ && tp->sp))
        {
            errorsym(ERR_EXPECTED_END_OF_TEMPLATE_ARGUMENTS_NEAR_UNDEFINED_TYPE, tp->sp);
            errskim(&lex, skim_templateend);
        }
        else
        {
            needkw(&lex, Keyword::gt_);
        }
    }
    UnrollTemplatePacks(*start);
    inTemplateArgs--;
    noTypeNameError = oldnoTn;
    return lex;
}
static bool sameTemplateSpecialization(TYPE* P, TYPE* A)
{
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
    if (P->sp->sb->parentClass != A->sp->sb->parentClass || strcmp(P->sp->name, A->sp->name) != 0)
        return false;
    if (P->sp->sb->templateLevel != A->sp->sb->templateLevel)
        return false;
    // this next if stmt is a horrible hack.
    if (P->size == 0 && !strcmp(P->sp->sb->decoratedName, A->sp->sb->decoratedName))
        return true;
    if (!P->sp->templateParams || !A->sp->templateParams)
        return false;
    auto itPL = P->sp->templateParams->begin();
    auto itePL = P->sp->templateParams->end();
    auto itPA = A->sp->templateParams->begin();
    auto itePA = A->sp->templateParams->end();
    if (itPL == itePL || itPA == itePA)  // errors
        return false;
    if (itPL->second->bySpecialization.types || !itPA->second->bySpecialization.types)
        return false;
    ++itPL;
    itePA = itPA->second->bySpecialization.types->end();
    itPA = itPA->second->bySpecialization.types->begin();

    if (itPL != itePL  && itPA != itePA)
    {
        for ( ; itPL != itePL && itPA != itePA; ++itPL, ++itPA)
        {
            if (itPL->second->type != itPA->second->type)
            {
                break;
            }
            else if (P->sp->sb->instantiated || A->sp->sb->instantiated)
            {
                if (itPL->second->type == TplType::typename_)
                {
                    if (!templatecomparetypes(itPL->second->byClass.dflt, itPA->second->byClass.val, true))
                        break;
                }
                else if (itPL->second->type == TplType::template_)
                {
                    if (!exactMatchOnTemplateParams(itPL->second->byTemplate.args, itPA->second->byTemplate.args))
                        break;
                }
                else if (itPL->second->type == TplType::int_)
                {
                    if (!templatecomparetypes(itPL->second->byNonType.tp, itPA->second->byNonType.tp, true))
                        break;
                    if (itPL->second->byNonType.dflt && !equalTemplateIntNode(itPL->second->byNonType.dflt, itPA->second->byNonType.val))
                        break;
                }
            }
        }
        return itPL == itePL && itPA == itePA;
    }
    return false;
}
bool exactMatchOnTemplateSpecialization(std::list<TEMPLATEPARAMPAIR>* old, std::list<TEMPLATEPARAMPAIR>* sym)
{
    if (old && sym)
    {
        auto itold = old->begin();
        auto iteold = old->end();
        auto itsym = sym->begin();
        auto itesym = sym->end();
        if (itsym != itesym && itsym->second->type == TplType::new_)
            ++itsym;
        for ( ; itold != iteold && itsym != itesym; ++itold, ++itsym)
        {
            if (itold->second->type != itsym->second->type)
                return false;
            switch (itold->second->type)
            {
                case TplType::typename_:
                    if (!sameTemplateSpecialization(itold->second->byClass.dflt, itsym->second->byClass.val))
                    {
                        if (!templatecomparetypes(itold->second->byClass.dflt, itsym->second->byClass.val, true))
                            return false;
                        if (!templatecomparetypes(itsym->second->byClass.val, itold->second->byClass.dflt, true))
                            return false;
                    }
                    break;
                case TplType::template_:
                    if (itold->second->byTemplate.dflt != itsym->second->byTemplate.val)
                        return false;
                    break;
                case TplType::int_:
                    if (!templatecomparetypes(itold->second->byNonType.tp, itsym->second->byNonType.tp, true))
                        return false;
                    if (itold->second->byNonType.dflt && !equalTemplateIntNode(itold->second->byNonType.dflt, itsym->second->byNonType.val))
                        return false;
                    break;
                default:
                    break;
            }
        }
        return itold == iteold && itsym == itesym;
    }
    return !old && !sym;
}
SYMBOL* FindSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams)
{
    if (sym->sb->specializations)
    {
        for (auto candidate : *sym->sb->specializations)
        {
            if (candidate->templateParams &&
                exactMatchOnTemplateArgs(templateParams, candidate->templateParams->front().second->bySpecialization.types))
            {
                return candidate;
            }
        }
    }
    return nullptr;
}
SYMBOL* LookupSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* templateParams)
{
    TYPE* tp;
    // maybe we know this specialization
    if (sym->sb->specializations)
    {
        for (auto candidate : *sym->sb->specializations)
        {
            if (candidate->templateParams && exactMatchOnTemplateArgs(templateParams->front().second->bySpecialization.types,
                                         candidate->templateParams->front().second->bySpecialization.types))
            {
                if (templateParams->size() == candidate->templateParams->size())
                {
                    return candidate;
                }
            }
        }
    }
    // maybe we know this as an instantiation
    bool found = false;
    SYMBOL* candidate = nullptr;
    if (sym->sb->instantiations)
    {
        for (auto it = sym->sb->instantiations->begin(); it != sym->sb->instantiations->end(); ++it)
        {
            if ((*it)->templateParams &&
                exactMatchOnTemplateSpecialization(templateParams->front().second->bySpecialization.types, (*it)->templateParams))
            {
                found = true;
                candidate = *it;
                sym->sb->instantiations->erase(it);
                break;
            }
        }
    }
    if (!found)
    {
        candidate = CopySymbol(sym);
        candidate->tp = CopyType(sym->tp);
        candidate->tp->sp = candidate;
        UpdateRootTypes(candidate->tp);
    }
    else
    {
        candidate->sb->maintemplate = candidate;
    }
    candidate->templateParams = templateParams;
    if (!sym->sb->specializations)
        sym->sb->specializations = symListFactory.CreateList();
    sym->sb->specializations->push_front(candidate);
    candidate->sb->overloadName = sym->sb->overloadName;
    candidate->sb->specialized = true;
    if (!candidate->sb->parentTemplate)
        candidate->sb->parentTemplate = sym;
    candidate->sb->baseClasses = nullptr;
    candidate->sb->vbaseEntries = nullptr;
    candidate->sb->vtabEntries = nullptr;
    tp = CopyType(candidate->tp);
    UpdateRootTypes(tp);
    candidate->tp = tp;
    candidate->tp->syms = nullptr;
    candidate->tp->tags = nullptr;
    candidate->sb->baseClasses = nullptr;
    candidate->sb->declline = candidate->sb->origdeclline = preProcessor->GetErrLineNo();
    candidate->sb->realdeclline = preProcessor->GetRealLineNo();
    candidate->sb->declfile = candidate->sb->origdeclfile = litlate(preProcessor->GetErrFile().c_str());
    candidate->sb->trivialCons = false;
    SetLinkerNames(candidate, Linkage::cdecl_);
    return candidate;
}
static bool matchTemplatedType(TYPE* old, TYPE* sym, bool strict)
{
    while (1)
    {
        if (isconst(old) == isconst(sym) && isvolatile(old) == isvolatile(sym))
        {
            old = basetype(old);
            sym = basetype(sym);
            if (old->type == sym->type || (isfunction(old) && isfunction(sym)))
            {
                switch (old->type)
                {
                    case BasicType::struct_:
                    case BasicType::class_:
                    case BasicType::union_:
                        return old->sp == sym->sp;
                    case BasicType::func_:
                    case BasicType::ifunc_:
                        if (!matchTemplatedType(old->btp, sym->btp, strict))
                            return false;
                        {
                            auto ito = old->syms->begin();
                            auto itoend = old->syms->end();
                            auto its = sym->syms->begin();
                            auto itsend = sym->syms->end();
                            if ((*ito)->sb->thisPtr)
                                ++ito;
                            if ((*its)->sb->thisPtr)
                                ++its;
                            while (ito != itoend && its != itsend)
                            {
                                if (!matchTemplatedType((*ito)->tp, (*its)->tp, strict))
                                    return false;
                                ++ito;
                                ++its;
                            }
                            return ito == itoend && its == itsend;
                        }
                    case BasicType::pointer_:
                    case BasicType::lref_:
                    case BasicType::rref_:
                        if (old->array == sym->array && old->size == sym->size)
                        {
                            old = old->btp;
                            sym = sym->btp;
                            break;
                        }
                        return false;
                    case BasicType::templateparam_:
                        return old->templateParam->second->type == sym->templateParam->second->type;
                    default:
                        return true;
                }
            }
            else
            {
                return !strict && old->type == BasicType::templateparam_;
            }
        }
        else
        {
            return false;
        }
    }
}
SYMBOL* LookupFunctionSpecialization(SYMBOL* overloads, SYMBOL* sp)
{
    SYMBOL* found1 = nullptr;
    SYMBOL* sym = (SYMBOL*)overloads->tp->syms->front();
    SYMBOL* sd = getStructureDeclaration();
    saveParams(&sd, 1);
    if (sym->sb->templateLevel && !sym->sb->instantiated &&
        (!sym->sb->parentClass || sym->sb->parentClass->sb->templateLevel != sym->sb->templateLevel))
    {
        found1 = detemplate(overloads->tp->syms->front(), nullptr, sp->tp);
        if (found1 &&
            allTemplateArgsSpecified(
                found1, found1->templateParams))  // && exactMatchOnTemplateArgs(found1->templateParams, sp->templateParams))
        {
            std::list<TEMPLATEPARAMPAIR>* tpx;
            for (auto sp1 : *overloads->tp->syms)
            {
                if (exactMatchOnTemplateArgs(found1->templateParams, sp1->templateParams))
                    if (matchOverload(found1->tp, sp->tp, true))
                        return sp1;
            }
            if (matchOverload(found1->tp, sp->tp, true))
            {
                sp->templateParams->front().second->bySpecialization.types = copyParams(found1->templateParams, false);
                sp->templateParams->front().second->bySpecialization.types->pop_front();
                for (auto&& tpx : *sp->templateParams->front().second->bySpecialization.types)
                {
                    tpx.second->byClass.dflt = tpx.second->byClass.val;
                    tpx.second->byClass.val = nullptr;
                }
                SetLinkerNames(sp, Linkage::cdecl_);
                found1 = sp;
            }
            else
            {
                found1 = nullptr;
            }
        }
        else
        {
            found1 = nullptr;
        }
    }
    restoreParams(&sd, 1);
    return found1;
}
LEXLIST* TemplateArgGetDefault(LEXLIST** lex, bool isExpression)
{
    LEXLIST *rv = nullptr, **cur = &rv;
    LEXLIST *current = *lex, *end = current;
    // this presumes that the template or expression is small enough to be cached...
    // may have to adjust it later
    // have to properly parse the default value, because it may have
    // embedded expressions that use '<'
    if (isExpression)
    {
        TYPE* tp;
        EXPRESSION* exp;
        end = expression_no_comma(current, nullptr, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
    }
    else
    {
        TYPE* tp;
        end = get_type_id(current, &tp, nullptr, StorageClass::cast_, false, true, false);
    }
    while (current && current != end)
    {
        *cur = Allocate<LEXLIST>();
        **cur = *current;
        (*cur)->next = nullptr;
        if (ISID(current))
            (*cur)->data->value.s.a = litlate((*cur)->data->value.s.a);
        current = current->next;
        cur = &(*cur)->next;
    }
    *lex = end;
    return rv;
}
static SYMBOL* templateParamId(TYPE* tp, const char* name, int tag )
{
    SYMBOL* rv = Allocate<SYMBOL>();
    rv->tp = tp;
    int len = strlen(name);
    char* buf = Allocate<char>(len + 5);
    strcpy(buf, name);
    *(int*)(buf + len + 1) = tag;
    rv->name = buf;
    return rv;
}
static LEXLIST* TemplateHeader(LEXLIST* lex, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* args)
{
    inTemplateHeader++;
    STRUCTSYM* structSyms = nullptr;
    std::list<TEMPLATEPARAMPAIR>* lst = args;
    if (needkw(&lex, Keyword::lt_))
    {
        while (1)
        {
            if (MATCHKW(lex, Keyword::gt_) || MATCHKW(lex, Keyword::rightshift_))
                break;
            args->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            lex = TemplateArg(lex, funcsp, args->back(), &lst, true);
            if (args)
            {
                if (!structSyms)
                {
                    structSyms = Allocate<STRUCTSYM>();
                    structSyms->tmpl = args;
                    addTemplateDeclaration(structSyms);
                }
            }
            if (!MATCHKW(lex, Keyword::comma_))
                break;
            lex = getsym();
        }
        for (auto&& search : *lst)
        {
            if (search.second->byClass.txtdflt)
            {
                std::list<SYMBOL*>* txtargs = symListFactory.CreateList();
                for (auto&& srch : *lst)
                    txtargs->push_back(srch.first);
                for (auto&& srch : *lst)
                    if (search.second->byClass.txtdflt)
                        search.second->byClass.txtargs = txtargs;
                break;
            }
        }
        if (MATCHKW(lex, Keyword::rightshift_))
            lex = getGTSym(lex);
        else
            needkw(&lex, Keyword::gt_);
    }
    inTemplateHeader--;
    return lex;
}
static LEXLIST* TemplateArg(LEXLIST* lex, SYMBOL* funcsp, TEMPLATEPARAMPAIR& arg, std::list<TEMPLATEPARAMPAIR>** lst, bool templateParam)
{
    LEXLIST* current = lex;
    LEXLIST* txttype = nullptr;
    switch (KW(lex))
    {
        TYPE *tp, *tp1;
        EXPRESSION* exp1;
        SYMBOL* sp;
        case Keyword::typename_:
        case Keyword::class_:
            arg.second->type = TplType::typename_;
            arg.second->packed = false;
            lex = getsym();
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                arg.second->packed = true;
                lex = getsym();
            }
            if (ISID(lex) || MATCHKW(lex, Keyword::classsel_))
            {
                SYMBOL *sym = nullptr, *strsym = nullptr;
                std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;

                lex = nestedPath(lex, &strsym, &nsv, nullptr, false, StorageClass::global_, false, 0);
                if (strsym)
                {
                    if (strsym->tp->type == BasicType::templateselector_)
                    {
                        sp = sym = templateParamId(strsym->tp, strsym->sb->templateSelector->back().name, 0);
                        lex = getsym();
                        tp = strsym->tp;
                        goto non_type_join;
                    }
                    else if (ISID(lex))
                    {
                        tp = MakeType(BasicType::templateselector_);
                        sp = sym = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                        tp->sp = sym;
                        auto last = sym->sb->templateSelector = templateSelectorListFactory.CreateVector();
                        last->push_back(TEMPLATESELECTOR{});
                        last->back().sp = nullptr;
                        last->push_back(TEMPLATESELECTOR{});
                        last->back().sp = strsym;
                        if (strsym->sb->templateLevel)
                        {
                            last->back().isTemplate = true;
                            last->back().templateParams = strsym->templateParams;
                        }
                        last->push_back(TEMPLATESELECTOR{});
                        last->back().name = litlate(lex->data->value.s.a);
                        lex = getsym();
                        goto non_type_join;
                    }
                    else
                    {
                        lex = getsym();
                        error(ERR_TYPE_NAME_EXPECTED);
                        break;
                    }
                }
                else if (ISID(lex))
                {
                    TYPE* tp = MakeType(BasicType::templateparam_);
                    tp->templateParam = &arg;
                    arg.first = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                    lex = getsym();
                }
                else
                {
                    lex = getsym();
                    error(ERR_TYPE_NAME_EXPECTED);
                    break;
                }
            }
            else
            {
                TYPE* tp = MakeType(BasicType::templateparam_);
                tp->templateParam = &arg;
                arg.first = templateParamId(tp, AnonymousName(), templateNameTag++);
            }
            if (MATCHKW(lex, Keyword::assign_))
            {
                if (arg.second->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                lex = getsym();
                arg.second->byClass.txtdflt = TemplateArgGetDefault(&lex, false);
                if (!arg.second->byClass.txtdflt)
                {
                    error(ERR_CLASS_TEMPLATE_DEFAULT_MUST_REFER_TO_TYPE);
                }
            }
            if (!MATCHKW(lex, Keyword::gt_) && !MATCHKW(lex, Keyword::leftshift_) && !MATCHKW(lex, Keyword::comma_))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        case Keyword::template_:
            arg.second->type = TplType::template_;
            lex = getsym();
            arg.second->byTemplate.args = templateParamPairListFactory.CreateList();
            lex = TemplateHeader(lex, funcsp, arg.second->byTemplate.args);
            if (arg.second->byTemplate.args)
                dropStructureDeclaration();
            arg.second->packed = false;
            if (!MATCHKW(lex, Keyword::class_) && !MATCHKW(lex, Keyword::typename_))
            {
                error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MUST_NAME_CLASS);
            }
            else
            {
                if (MATCHKW(lex, Keyword::typename_))
                    RequiresDialect::Feature(Dialect::cpp17, "'typename' in template template parameter");
                lex = getsym();
            }
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                arg.second->packed = true;
                lex = getsym();
            }
            if (ISID(lex))
            {
                TYPE* tp = MakeType(BasicType::templateparam_);
                tp->templateParam = &arg;
                arg.first = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                lex = getsym();
            }
            else
            {
                TYPE* tp = MakeType(BasicType::templateparam_);
                tp->templateParam = &arg;
                arg.first = templateParamId(tp, AnonymousName(), templateNameTag++);
            }
            if (MATCHKW(lex, Keyword::assign_))
            {
                if (arg.second->packed)
                {
                    error(ERR_CANNOT_USE_DEFAULT_WITH_PACKED_TEMPLATE_PARAMETER);
                }
                arg.second->byTemplate.txtdflt = TemplateArgGetDefault(&lex, false);
                if (!arg.second->byTemplate.txtdflt)
                {
                    error(ERR_TEMPLATE_TEMPLATE_PARAMETER_MISSING_DEFAULT);
                }
            }
            if (!MATCHKW(lex, Keyword::gt_) && !MATCHKW(lex, Keyword::leftshift_) && !MATCHKW(lex, Keyword::comma_))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            break;
        default:  // non-type
        {
            Linkage linkage, linkage2, linkage3;
            bool defd;
            bool notype;
            linkage = Linkage::none_;
            linkage2 = Linkage::none_;
            linkage3 = Linkage::none_;
            defd = false;
            notype = false;
            arg.second->type = TplType::int_;
            arg.second->packed = false;
            tp = nullptr;
            sp = nullptr;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            noTypeNameError++;
            lex = getBasicType(lex, funcsp, &tp, nullptr, false, funcsp ? StorageClass::auto_ : StorageClass::global_, &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd, nullptr, nullptr, nullptr, false, true,
                               false, false, false);
            noTypeNameError--;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            // get type qualifiers
            if (!ISID(lex) && !MATCHKW(lex, Keyword::ellipse_))
            {
                lex = getBeforeType(lex, funcsp, &tp, &sp, nullptr, nullptr, false, StorageClass::cast_, &linkage, &linkage2, &linkage3,
                                    nullptr, false, false, true, false); /* fixme at file scope init */
            }
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                arg.second->packed = true;
                lex = getsym();
            }
            // get the name
            lex = getBeforeType(lex, funcsp, &tp, &sp, nullptr, nullptr, false, StorageClass::cast_, &linkage, &linkage2, &linkage3, nullptr,
                                false, false, false, false); /* fixme at file scope init */
            sizeQualifiers(tp);
            if (!tp || notype)
            {
                auto itlist = (*lst)->begin();
                auto itelist = (*lst)->end();
                if (sp && itlist != itelist && itlist->first)
                {
                    for ( ; itlist != itelist; ++itlist)
                    {
                        if (!itlist->first)
                            break;
                        if (!strcmp(itlist->first->name, sp->name))
                        {
                            tp = itlist->first->tp;
                            if (ISID(lex))
                            {
                                sp = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                                lex = getsym();
                            }
                            else
                            {
                                sp = templateParamId(tp, AnonymousName(), templateNameTag++);
                            }
                            goto non_type_join;
                        }
                    }
                }
                error(ERR_INVALID_TEMPLATE_PARAMETER);
            }
            else
            {
                if (!sp)
                {
                    sp = templateParamId(nullptr, AnonymousName(), templateNameTag++);
                }
            non_type_join:
                if (sp->sb)
                    sp->sb->storage_class = StorageClass::templateparam_;
                sp->tp = MakeType(BasicType::templateparam_);
                sp->tp->templateParam = &arg;
                arg.second->type = TplType::int_;
                arg.first = Allocate<SYMBOL>();
                *arg.first = *sp;
                arg.first->sb = nullptr;
                if (isarray(tp) || isfunction(tp))
                {
                    if (isarray(tp))
                        tp = tp->btp;
                    tp = MakeType(BasicType::pointer_, tp);
                }
                arg.second->byNonType.tp = tp;
                auto tp2 = tp;
                while (ispointer(tp2))
                    tp2 = basetype(tp2)->btp;
                tp2 = basetype(tp2);
                if (!isint(tp) && (!ispointer(tp) || tp2->type == BasicType::templateselector_))
                {
                    LEXLIST* end = lex;
                    LEXLIST** cur = &txttype;
                    // if the type didn't resolve, we want to cache it then evaluate it later...
                    while (current && current != end)
                    {
                        *cur = Allocate<LEXLIST>();
                        **cur = *current;
                        (*cur)->next = nullptr;
                        if (ISID(current))
                            (*cur)->data->value.s.a = litlate((*cur)->data->value.s.a);
                        current = current->next;
                        cur = &(*cur)->next;
                    }
                }
                arg.second->byNonType.txttype = txttype;
                if (basetype(tp)->type != BasicType::templateparam_ && basetype(tp)->type != BasicType::templateselector_ &&
                    basetype(tp)->type != BasicType::enum_ && !isint(tp) && !ispointer(tp) && basetype(tp)->type != BasicType::lref_ && 
                    (!templateNestingCount || basetype(tp)->type != BasicType::any_))
                {
                    if (basetype(tp)->type == BasicType::auto_)
                        RequiresDialect::Feature(Dialect::cpp17, "auto as non-type template parameter type");
                    else
                        error(ERR_NONTYPE_TEMPLATE_PARAMETER_INVALID_TYPE);
                }
                if (sp)
                {
                    if (MATCHKW(lex, Keyword::assign_))
                    {
                        tp1 = nullptr;
                        exp1 = nullptr;
                        lex = getsym();
                        arg.second->byNonType.txtdflt = TemplateArgGetDefault(&lex, true);
                        if (!arg.second->byNonType.txtdflt)
                        {
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                        else
                        {
                            TYPE* tp = nullptr;
                            EXPRESSION* exp = nullptr;
                            LEXLIST* lex = SetAlternateLex(arg.second->byNonType.txtdflt);
                            lex = expression_no_comma(lex, nullptr, nullptr, &tp, &exp, nullptr, 0);
                            SetAlternateLex(nullptr);
                            if (tp && isintconst(exp))
                            {
                                arg.second->byNonType.dflt = exp;
                            }
                        }
                    }
                }
            }
            break;
        }
    }

    return lex;
}
static bool matchArg(TEMPLATEPARAMPAIR& param, TEMPLATEPARAMPAIR& arg)
{
    if (param.second->type != arg.second->type)
    {
        return false;
    }
    else if (param.second->type == TplType::template_)
    {
        if (arg.second->byTemplate.dflt)
        {
            auto ito = param.second->byTemplate.args->begin();
            auto itoe = param.second->byTemplate.args->end();
            auto its = arg.second->byTemplate.dflt->templateParams->begin();
            auto itse = arg.second->byTemplate.dflt->templateParams->end();
            ++its;
            for (; ito != itoe && its != itse; ++ito, ++its)
            {
                if (ito->second->type != its->second->type)
                {
                    return false;
                }
                if (ito->second->packed)
                {
                    auto itt = ito;
                    ++itt;
                    if (itt != itoe)
                    {
                        return false;
                    }
                    for (; its != itse && its->second->type == ito->second->type; ++its);
                    --its;
                }
                else if (its->second->packed && !ito->second->packed)
                {
                    return false;
                }
            }
            if (itoe != ito || itse != its)
            {
                return false;
            }
        }
    }
    return true;
}
bool TemplateIntroduceArgs(std::list<TEMPLATEPARAMPAIR>* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (sym && args)
    {
        auto its = sym->begin();
        auto ita = args->begin();
        if (its != sym->end())
            ++its;
        for ( ; its != sym->end() && ita != args->end(); ++its, ++ita)
        {
            if (its->second->type == TplType::template_ && ita->second->type == TplType::typename_)
            {
                TYPE* tp1 = ita->second->byClass.dflt;
                while (tp1 && tp1->type != BasicType::typedef_ && tp1->btp)
                    tp1 = tp1->btp;
                if (tp1->type != BasicType::typedef_ && !isstructured(tp1))
                    return false;
                its->second->byTemplate.val = tp1->sp;
            }
            else
            {
                if (!matchArg(*its, *ita))
                    return false;
                switch (ita->second->type)
                {
                    case TplType::typename_:
                        its->second->byClass.val = ita->second->byClass.dflt;
                        break;
                    case TplType::template_:
                        its->second->byTemplate.val = ita->second->byTemplate.dflt;
                        break;
                    case TplType::int_:
                        its->second->byNonType.val = ita->second->byNonType.dflt;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return true;
}
static bool comparePointerTypes(TYPE* tpo, TYPE* tps)
{
    TYPE* lastIndirect = nullptr;
    while (tpo && tps)
    {
        tpo = basetype(tpo);
        tps = basetype(tps);
        if (!tpo || !tps)
            return false;
        if (tpo->type == BasicType::templateparam_)
        {
            if (tpo == lastIndirect)
                return true;
            lastIndirect = tpo;
            tpo = tpo->templateParam->second->byClass.dflt;
        }
        if (tps->type == BasicType::templateparam_)
            tps = tps->templateParam->second->byClass.dflt;
        if (!tpo || !tps || tpo->type == BasicType::templateparam_ || tps->type == BasicType::templateparam_)
            return false;
        if (tpo->type != tps->type)
            return false;
        tpo = tpo->btp;
        tps = tps->btp;
    }
    return tpo == tps;
}
static bool TemplateInstantiationMatchInternal(std::list<TEMPLATEPARAMPAIR>* porig, std::list<TEMPLATEPARAMPAIR>* psym, bool dflt, bool bySpecialization)
{
    if (porig && psym)
    {
        auto itOrig = porig->begin();
        auto iteOrig = porig->end();
        auto itSym = psym->begin();
        auto iteSym = psym->end();
        if (bySpecialization && itOrig->second->bySpecialization.types)
        {
            iteOrig = itOrig->second->bySpecialization.types->end();
            itOrig = itOrig->second->bySpecialization.types->begin();
        }
        else
        {
            ++itOrig;
        }

        if (bySpecialization && itSym->second->bySpecialization.types)
        {
            iteSym = itSym->second->bySpecialization.types->end();
            itSym = itSym->second->bySpecialization.types->begin();
        }
        else
        {
            ++itSym;
        }
        for ( ; itOrig != iteOrig && itSym != iteSym; ++itOrig, ++itSym)
        {
            void *xorig, *xsym;
            if (dflt)
            {
                xorig = itOrig->second->byClass.dflt;
                xsym = itSym->second->byClass.dflt;
                if (!xsym)
                    xsym = itSym->second->byClass.val;
            }
            else
            {
                xorig = itOrig->second->byClass.val;
                xsym = itSym->second->byClass.val;
            }
            if (itOrig->second->type != itSym->second->type)
                return false;
            switch (itOrig->second->type)
            {
                case TplType::typename_: {
                    if (itOrig->second->packed != itSym->second->packed)
                        return false;
                    if (itOrig->second->packed)
                    {
                        std::list<TEMPLATEPARAMPAIR>* packorig = itOrig->second->byPack.pack;
                        std::list<TEMPLATEPARAMPAIR>* packsym = itSym->second->byPack.pack;
                        if (packorig && packsym)
                        {
                            auto itPOrig = packorig->begin();
                            auto itePOrig = packorig->end();
                            auto itPSym = packsym->begin();
                            auto itePSym = packsym->end();
                            for ( ; itPOrig != itePOrig && itPSym != itePSym; ++itPOrig, ++itPSym)
                            {
                                TYPE* torig;
                                TYPE* tsym;
                                if (dflt)
                                {
                                    torig = itPOrig->second->byClass.dflt;
                                    if (!torig)
                                        torig = itPOrig->second->byClass.val;
                                    tsym = itPSym->second->byClass.dflt;
                                    if (!tsym)
                                        tsym = itPSym->second->byClass.val;
                                }
                                else
                                {
                                    torig = itPOrig->second->byClass.val;
                                    tsym = itPSym->second->byClass.val;
                                }
                                if (basetype(torig)->nullptrType != basetype(tsym)->nullptrType)
                                    return false;
                                if (isref(torig) != isref(tsym))
                                    return false;
                                if (basetype(torig)->array != basetype(tsym)->array)
                                    return false;
                                if (basetype(torig)->byRefArray != basetype(tsym)->byRefArray)
                                    return false;
                                if (basetype(torig)->array && !!basetype(torig)->esize != !!basetype(tsym)->esize)
                                    return false;
                                if ((basetype(torig)->type == BasicType::enum_) != (basetype(tsym)->type == BasicType::enum_))
                                    return false;
                                if (tsym->type == BasicType::templateparam_)
                                    tsym = tsym->templateParam->second->byClass.val;
                                if ((!templatecomparetypes(torig, tsym, true, false) ||
                                     !templatecomparetypes(tsym, torig, true, false)) &&
                                    !sameTemplate(torig, tsym, true))
                                    break;
                                if (isref(torig))
                                    torig = basetype(torig)->btp;
                                if (isref(tsym))
                                    tsym = basetype(tsym)->btp;
                                if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
                                    return false;
                            }
                            if (itPOrig != itePOrig || itPSym != itePSym)
                                return false;
                        }
                        else if (packorig || packsym)
                            return false;
                    }
                    else if (xorig && xsym)
                    {
                        TYPE* torig = (TYPE*)xorig;
                        TYPE* tsym = (TYPE*)xsym;
                        TYPE* btorig = basetype(torig);
                        TYPE* btsym = basetype(tsym);
                        if (btorig->nullptrType != btsym->nullptrType)
                            return false;
                        if (isref(torig) != isref(tsym))
                            return false;
                        if (btorig->array != btsym->array)
                            return false;
                        if (btorig->byRefArray != btsym->byRefArray)
                            return false;
                        if (btorig->array && !!btorig->esize != !!btsym->esize)
                            return false;
                        if ((btorig->type == BasicType::enum_) != (btsym->type == BasicType::enum_))
                            return false;
                        if ((!templatecomparetypes(torig, tsym, true, false) || !templatecomparetypes(tsym, torig, true, false)) &&
                            !sameTemplate(torig, tsym, true))
                            return false;
                        if (!comparePointerTypes(torig, tsym))
                            return false;
                        if (isref(torig))
                            torig = btorig->btp;
                        if (isref(tsym))
                            tsym = btsym->btp;
                        if (isconst(torig) != isconst(tsym) || isvolatile(torig) != isvolatile(tsym))
                            return false;
                        if (btsym->type == BasicType::enum_ || btsym->enumConst)
                        {
                            if (btorig->sp != btsym->sp)
                                return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                    break;
                }
                case TplType::template_:
                    if (xorig != xsym)
                        return false;
                    break;
                case TplType::int_:
                    if (itOrig->second->packed != itSym->second->packed)
                        return false;
                    if (itOrig->second->packed)
                    {
                        std::list<TEMPLATEPARAMPAIR>* packorig = itOrig->second->byPack.pack;
                        std::list<TEMPLATEPARAMPAIR>* packsym = itSym->second->byPack.pack;
                        if (packorig && packsym)
                        {
                            auto itPOrig = packorig->begin();
                            auto itePOrig = packorig->end();
                            auto itPSym = packsym->begin();
                            auto itePSym = packsym->end();
                            for (; itPOrig != itePOrig && itPSym != itePSym; ++itPOrig, ++itPSym)
                            {
                                EXPRESSION* torig;
                                EXPRESSION* tsym;
                                if (dflt)
                                {
                                    torig = itPOrig->second->byNonType.dflt;
                                    if (!torig)
                                        torig = itPSym->second->byNonType.val;
                                    tsym = itPSym->second->byNonType.dflt;
                                    if (!tsym)
                                        tsym = itPSym->second->byNonType.val;
                                }
                                else
                                {
                                    torig = itPOrig->second->byNonType.val;
                                    tsym = itPSym->second->byNonType.val;
                                }
                                if (!templatecomparetypes(itPOrig->second->byNonType.tp, itPSym->second->byNonType.tp, true))
                                    return false;
                                if (tsym && !equalTemplateIntNode((EXPRESSION*)torig, (EXPRESSION*)tsym))
                                    return false;
                            }
                            if (itPOrig != itePOrig || itPSym != itePSym)
                                return false;
                        }
                        else if (packorig || packsym)
                            return false;
                    }
                    else
                    {
                        if (!templatecomparetypes(itOrig->second->byNonType.tp, itSym->second->byNonType.tp, true))
                            return false;
                        if (xsym && xorig && !equalTemplateIntNode((EXPRESSION*)xorig, (EXPRESSION*)xsym))
                            return false;
                    }
                    break;
                default:
                    break;
            }
        }
        if (itSym != iteSym && itSym->second->packed && !itSym->second->byPack.pack)
            return true;
        return itOrig == iteOrig && itSym == iteSym;
    }
    return !porig && !psym;
}
bool TemplateInstantiationMatch(SYMBOL* orig, SYMBOL* sym, bool bySpecialization)
{
    if (orig && orig->sb->parentTemplate == sym->sb->parentTemplate)
    {
        if (!TemplateInstantiationMatchInternal(orig->templateParams, sym->templateParams, false, bySpecialization))
            return false;
        while (orig->sb->parentClass && sym->sb->parentClass)
        {
            orig = orig->sb->parentClass;
            sym = sym->sb->parentClass;
        }
        if (orig->sb->parentClass || sym->sb->parentClass)
            return false;
        return true;
    }
    return false;
}
void TemplateTransferClassDeferred(SYMBOL* newCls, SYMBOL* tmpl)
{
    if (newCls->tp->syms && (!newCls->templateParams || !newCls->templateParams->front().second->bySpecialization.types))
    {
        if (newCls->tp->syms && tmpl->tp->syms)
        {
            auto ns = newCls->tp->syms->begin();
            auto nse = newCls->tp->syms->end();
            auto os = tmpl->tp->syms->begin();
            auto ose = tmpl->tp->syms->end();
            while (ns != nse && os != ose)
            {
                SYMBOL* ss = *ns;
                SYMBOL* ts = *os;
                if (strcmp(ss->name, ts->name) != 0)
                {
                    ts = search(tmpl->tp->syms, ss->name);
                    // we might get here with ts = nullptr for example when a using statement inside a template
                    // references base class template members which aren't defined yet.
                }
                if (ts)
                {
                    if (ss->tp->type == BasicType::aggregate_ && ts->tp->type == BasicType::aggregate_)
                    {
                        auto os2 = ts->tp->syms->begin();
                        auto os2e = ts->tp->syms->end();
                        auto ns2 = ss->tp->syms->begin();
                        auto ns2e = ss->tp->syms->end();

                        // these lists may be mismatched, in particular the old symbol table
                        // may have partial specializations for templates added after the class was defined...
                        while (ns2 != ns2e && os2 != os2e)
                        {
                            SYMBOL* ts2 = *os2;
                            SYMBOL* ss2 = *ns2;
                            if (ts2->sb->defaulted || ss2->sb->defaulted)
                                break;
                            ss2->sb->copiedTemplateFunction = true;
                            if (ts2->sb->deferredCompile && !ss2->sb->deferredCompile)
                            {
                                auto tsf = basetype(ts2->tp)->syms->begin();
                                auto tsfe = basetype(ts2->tp)->syms->end();
                                auto ssf = basetype(ss2->tp)->syms->begin();
                                auto ssfe = basetype(ss2->tp)->syms->end();
                                while (tsf != tsfe && ssf != ssfe)
                                {
                                    if (!(*ssf)->sb->anonymous || !(*tsf)->sb->anonymous)
                                        (*ssf)->name = (*tsf)->name;
                                    ++tsf;
                                    ++ssf;
                                }
                                ss2->sb->deferredCompile = ts2->sb->deferredCompile;
                                ss2->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                            }
                            ss2->sb->maintemplate = ts2;
                            ++ns2;
                            ++os2;
                        }
                    }
                }
                ++ns;
                ++os;
            }
        }
        if (newCls->tp->tags && tmpl->tp->tags)
        {
            auto ns = newCls->tp->tags->begin();
            auto nse = newCls->tp->tags->end();
            auto os = tmpl->tp->tags->begin();
            auto ose = tmpl->tp->tags->end();
            ++ns, ++os; // past embedded reference to self
            while (ns != nse && os != ose)
            {
                SYMBOL* ss = *ns;
                SYMBOL* ts = *os;
                if (strcmp(ss->name, ts->name) != 0)
                {
                    ts = search(tmpl->tp->syms, ss->name);
                    // we might get here with ts = nullptr for example when a using statement inside a template
                    // references base class template members which aren't defined yet.
                }
                if (ts)
                {
                    if (isstructured(ss->tp))
                    {
                        if (!ss->sb->deferredCompile)
                        {
                            ss->sb->deferredCompile = ts->sb->deferredCompile;
                            PerformDeferredInitialization(ss->tp, nullptr);
                        }
                        TemplateTransferClassDeferred(ss, ts);
                    }
                }
                ++ns;
                ++os;
            }
        }
    }
}
static bool ValidSpecialization(std::list<TEMPLATEPARAMPAIR>* special, std::list<TEMPLATEPARAMPAIR>* args, bool templateMatch)
{
    if (special && args)
    {
        auto itSpecial = special->begin();
        auto itArgs = args->begin();
        if (itSpecial->second->type == TplType::new_)
            ++itSpecial;
        for (; itSpecial != special->end() && itArgs != args->end(); ++ itSpecial, ++itArgs)
        {
            if (itSpecial->second->type != itArgs->second->type)
            {
                if (itArgs->second->type != TplType::typename_ || (itArgs->second->byClass.dflt->type != BasicType::templateselector_ &&
                                                          itArgs->second->byClass.dflt->type != BasicType::templatedecltype_))
                    return false;
            }
            if (!templateMatch)
            {
                if ((itSpecial->second->byClass.val && !itArgs->second->byClass.dflt) ||
                    (!itSpecial->second->byClass.val && itArgs->second->byClass.dflt))
                    return false;
                switch (itArgs->second->type)
                {
                    case TplType::typename_:
                        if (itArgs->second->byClass.dflt &&
                            !templatecomparetypes(itSpecial->second->byClass.val, itArgs->second->byClass.dflt, true))
                            return false;
                        break;
                    case TplType::template_:
                        if (itArgs->second->byTemplate.dflt &&
                            !ValidSpecialization(itSpecial->second->byTemplate.args, itArgs->second->byTemplate.dflt->templateParams,
                                                 true))
                            return false;
                        break;
                    case TplType::int_:
                        if (!templatecomparetypes(itSpecial->second->byNonType.tp, itArgs->second->byNonType.tp, true))
                            if (!isint(itSpecial->second->byNonType.tp) || !isint(itArgs->second->byNonType.tp))
                                return false;
                        break;
                    default:
                        break;
                }
            }
        }
        return (itSpecial == special->end() || itSpecial->second->byClass.txtdflt) && !args;
    }
    return (!special || special->front().second->byClass.txtdflt) && !args;
}
SYMBOL* MatchSpecialization(SYMBOL* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (sym->sb->specialized)
    {
        if (ValidSpecialization(sym->templateParams->front().second->bySpecialization.types, args, false))
            return sym;
    }
    else
    {
        if (ValidSpecialization(sym->templateParams, args, true))
            return sym;
    }
    return nullptr;
}
void DoInstantiateTemplateFunction(TYPE* tp, SYMBOL** sp, std::list<NAMESPACEVALUEDATA*>* nsv, SYMBOL* strSym,
                                   std::list<TEMPLATEPARAMPAIR>* templateParams, bool isExtern)
{
    SYMBOL* sym = *sp;
    SYMBOL *spi = nullptr, *ssp;
    SYMLIST** p = nullptr;
    if (nsv)
    {
        auto rvl = tablesearchone(sym->name, nsv->front(), false);
        if (rvl)
            spi = rvl;
        else
            errorNotMember(strSym, nsv->front(), sym->sb->decoratedName);
    }
    else
    {
        ssp = getStructureDeclaration();
        if (ssp)
        {
            spi = ssp->tp->syms->Lookup(sym->name);
        }
        if (!spi)
        {
            spi = namespacesearch(sym->name, globalNameSpace, false, false);
        }
    }
    if (spi)
    {
        if (spi->sb->storage_class == StorageClass::overloads_)
        {
            FUNCTIONCALL* funcparams = Allocate<FUNCTIONCALL>();
            SYMBOL* instance;
            auto hr = basetype(tp)->syms->begin();
            auto hre = basetype(tp)->syms->end();
            funcparams->arguments = initListListFactory.CreateList();
            funcparams->templateParams = templateParams->front().second->bySpecialization.types;
            funcparams->ascall = true;
            if (templateParams->front().second->bySpecialization.types)
                funcparams->astemplate = true;
            if ((*hr)->sb->thisPtr)
                ++hr;
            while (hr != hre)
            {
                auto init = Allocate<INITLIST>();
                init->tp = (*hr)->tp;
                init->exp = intNode(ExpressionNode::c_i_, 0);
                funcparams->arguments->push_back(init);
                ++hr;
            }
            if (spi->sb->parentClass)
            {
                funcparams->thistp = MakeType(BasicType::pointer_, spi->sb->parentClass->tp);
                funcparams->thisptr = intNode(ExpressionNode::c_i_, 0);
            }
            instance = GetOverloadedTemplate(spi, funcparams);
            if (instance)
            {
                if (instance->sb->templateLevel)
                {
                    if (!isExtern)
                    {
                        instance = TemplateFunctionInstantiate(instance, true);
                        spi->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                        InsertInline(instance);
                    }
                }
                *sp = instance;
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
}
static void referenceInstanceMembers(SYMBOL* cls, bool excludeFromExplicitInstantiation)
{
    if (Optimizer::cparams.prm_xcept)
    {
        RTTIDumpType(cls->tp);
    }
    if (cls->sb->vtabsp)
    {
        Optimizer::SymbolManager::Get(cls->sb->vtabsp);
    }
    if (cls->tp->syms)
    {
        BASECLASS* lst;
        for (auto sym : *cls->tp->syms)
        {
            if (sym->sb->storage_class == StorageClass::overloads_)
            {
                auto sp = sym;
                for (auto sym : *sp->tp->syms)
                {
                    if (sym->sb->templateLevel <= cls->sb->templateLevel && !sym->templateParams)
                    {
                        sym->sb->dontinstantiate = false;
                        sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                        Optimizer::SymbolManager::Get(sym)->dontinstantiate = false;
                        if (!excludeFromExplicitInstantiation && !sym->sb->attribs.inheritable.excludeFromExplicitInstantiation)
                        {
                            if (sym->sb->defaulted && !sym->sb->deleted && !sym->sb->inlineFunc.stmt)
                            {
                                createConstructor(cls, sym);
                            }
                            if (!sym->sb->deleted)
                            {
                                InsertInline(sym);
                            }
                        }
                    }
                }
            }
            else if (!ismember(sym) && !istype(sym) && sym->sb->storage_class != StorageClass::enumconstant_)
            {
                if (cls->sb->templateLevel || sym->sb->templateLevel)
                    InsertInlineData(sym);
            }
        }

        if (cls->tp->tags)
        {
            for (auto sym : *cls->tp->tags)
            {
                if (isstructured(sym->tp))
                {
                    sym = basetype(sym->tp)->sp;
                    if (sym->sb->parentClass == cls && !sym->templateParams)
                        referenceInstanceMembers(sym, excludeFromExplicitInstantiation ||
                                                          sym->sb->attribs.inheritable.excludeFromExplicitInstantiation);
                }
            }
        }
    }
}
static void dontInstantiateInstanceMembers(SYMBOL* cls, bool excludeFromExplicitInstantiation)
{
    if (cls->tp->syms)
    {
        BASECLASS* lst;
        for (auto sym : *cls->tp->syms)
        {
            if (sym->sb->storage_class == StorageClass::overloads_)
            {
                auto sp1 = sym;
                for (auto sym : *sp1->tp->syms)
                {
                    if (sym->sb->templateLevel <= cls->sb->templateLevel && !sym->templateParams)
                    {
                        if (!excludeFromExplicitInstantiation && !sym->sb->attribs.inheritable.excludeFromExplicitInstantiation)
                        {
                            sym->sb->dontinstantiate = true;
                        }
                    }
                }
            }
        }
        if (cls->tp->tags)
        {
            bool first = true;
            for (auto sym : *cls->tp->tags)
            {
                if (!first) // past the definition of self
                {
                    if (isstructured(sym->tp))
                    {
                        sym = basetype(sym->tp)->sp;
                        if (sym->sb->parentClass == cls && !sym->templateParams)
                            dontInstantiateInstanceMembers(sym, excludeFromExplicitInstantiation ||
                                sym->sb->attribs.inheritable.excludeFromExplicitInstantiation);
                    }
                }
                first = false;
            }
        }
    }
}

static bool IsFullySpecialized(TEMPLATEPARAMPAIR* tpx)
{
    switch (tpx->second->type)
    {
        case TplType::typename_:
            return !typeHasTemplateArg(tpx->second->byClass.dflt);
        case TplType::template_: {
            if (tpx->second->byTemplate.args)
            {
                for (auto&& tpxl : *tpx->second->byTemplate.args)
                    if (!IsFullySpecialized(tpx))
                        return false;
            }
            return true;
        }
        case TplType::int_:
            if (!tpx->second->byNonType.dflt)
                return false;
            if (!isarithmeticconst(tpx->second->byNonType.dflt))
            {
                EXPRESSION* exp = tpx->second->byNonType.dflt;
                if (exp && tpx->second->byNonType.tp->type != BasicType::templateparam_)
                {
                    while (castvalue(exp) || lvalue(exp))
                        exp = exp->left;
                    switch (exp->type)
                    {
                        case ExpressionNode::pc_:
                        case ExpressionNode::global_:
                        case ExpressionNode::func_:
                            return true;
                        default:
                            break;
                    }
                }
                return false;
            }
            else
            {
                return true;
            }
            break;
        default:
            return false;
    }
}
bool TemplateFullySpecialized(SYMBOL* sp)
{
    if (sp)
    {
        if (sp->templateParams && sp->templateParams->front().second->bySpecialization.types)
        {
            for (auto&& tpx : *sp->templateParams->front().second->bySpecialization.types)
                if (!IsFullySpecialized(&tpx))
                    return false;
            return true;
        }
    }
    return false;
}

void propagateTemplateDefinition(SYMBOL* sym)
{
    int oldCount = templateNestingCount;
    struct templateListData* oldList = currents;
    templateNestingCount = 0;
    currents = nullptr;
    if (!sym->sb->deferredCompile && !sym->sb->inlineFunc.stmt)
    {
        SYMBOL* parent = sym->sb->parentClass;
        if (parent)
        {
            SYMBOL* old = parent->sb->maintemplate;
            if (!old)
                old = parent;
            if (old && old->tp->syms)
            {
                SYMBOL* p = old->tp->syms->Lookup(sym->name);
                if (p)
                {
                    for (auto cur : *basetype(p->tp)->syms)
                    {
                        if (sym && sym->sb->origdeclline == cur->sb->origdeclline &&
                            !strcmp(sym->sb->origdeclfile, cur->sb->origdeclfile) && cur->sb->deferredCompile)
                        {
                            if (matchesCopy(cur, false) == matchesCopy(sym, false) &&
                                matchesCopy(cur, true) == matchesCopy(sym, true))
                            {
                                sym->sb->deferredCompile = cur->sb->deferredCompile;
                                sym->sb->memberInitializers = cur->sb->memberInitializers;
                                sym->sb->pushedTemplateSpecializationDefinition = 1;
                                sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                                {
                                    auto src = basetype(cur->tp)->syms->begin();
                                    auto srce = basetype(cur->tp)->syms->end();
                                    auto dest = basetype(sym->tp)->syms->begin();
                                    auto deste = basetype(sym->tp)->syms->end();
                                    while (src != srce && dest != deste)
                                    {
                                        (*dest)->name = (*src)->name;
                                        ++src;
                                        ++dest;
                                    }
                                }
                                if (cur->templateParams && sym->templateParams)
                                {
                                    auto itsrc = cur->templateParams->begin();
                                    auto itdest = sym->templateParams->begin();
                                    for (; itsrc != cur->templateParams->end() && itdest != sym->templateParams->end(); ++itsrc, ++itdest)
                                        if (itsrc->first && itdest->first)
                                            itdest->first->name = itsrc->first->name;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            SYMBOL* old;
            int tns = PushTemplateNamespace(sym);
            old = namespacesearch(sym->name, globalNameSpace, false, false);
            if (old)
            {
                for (auto cur : *basetype(old->tp)->syms)
                {
                    if (sym && sym->sb->origdeclline == cur->sb->origdeclline && !strcmp(sym->sb->origdeclfile, cur->sb->origdeclfile) &&
                        cur->sb->deferredCompile)
                    {
                        sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                        sym->sb->deferredCompile = cur->sb->deferredCompile;
                        cur->sb->pushedTemplateSpecializationDefinition = 1;
                        if (basetype(sym->tp)->syms && basetype(cur->tp)->syms)
                        {
                            auto src = basetype(cur->tp)->syms->begin();
                            auto srce = basetype(cur->tp)->syms->end();
                            auto dest = basetype(sym->tp)->syms->begin();
                            auto deste = basetype(sym->tp)->syms->end();
                            while (src != srce && dest != deste)
                            {
                                (*dest)->name = (*src)->name;
                                ++src;
                                ++dest;
                            }
                        }
                    }
                }
            }
            PopTemplateNamespace(tns);
        }
    }
    currents = oldList;
    templateNestingCount = oldCount;
}
static void MarkDllLinkage(SYMBOL* sp, Linkage linkage)
{
    if (linkage != Linkage::none_ && sp->sb->attribs.inheritable.linkage2 != linkage)
    {
        if (sp->sb->attribs.inheritable.linkage2 != Linkage::none_)
        {
            errorsym(ERR_ATTEMPING_TO_REDEFINE_DLL_LINKAGE, sp);
        }
        else
        {
            sp->sb->attribs.inheritable.linkage2 = linkage;
            Optimizer::SymbolManager::Get(sp)->isexport = linkage == Linkage::export_;
            Optimizer::SymbolManager::Get(sp)->isimport = linkage == Linkage::import_;
            Optimizer::SymbolManager::Get(sp)->isinternal = linkage == Linkage::internal_;
            if (sp->sb->vtabsp)
            {
                sp->sb->vtabsp->sb->attribs.inheritable.linkage2 = linkage;
                if (sp->sb->vtabsp->sb->symRef)
                {
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isexport = linkage == Linkage::export_;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isimport = linkage == Linkage::import_;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->isinternal = linkage == Linkage::internal_;
                }
                if (sp->sb->vtabsp->sb->attribs.inheritable.linkage2 == Linkage::import_)
                {
                    sp->sb->vtabsp->sb->dontinstantiate = true;
                    Optimizer::SymbolManager::Get(sp->sb->vtabsp)->dontinstantiate = true;
                }
            }
            if (sp->tp->syms)
            {
                for (auto sym : *sp->tp->syms)
                {
                    if (sym->sb->storage_class == StorageClass::overloads_)
                    {
                        for (auto sp1 : *sym->tp->syms)
                        {
                            if (!(sp1)->templateParams)
                            {
                                (sp1)->sb->attribs.inheritable.linkage2 = linkage;
                                (sp1)->sb->attribs.inheritable.isInline = false;
                                Optimizer::SymbolManager::Get(sp1)->isexport = linkage == Linkage::export_;
                                Optimizer::SymbolManager::Get(sp1)->isimport = linkage == Linkage::import_;
                                Optimizer::SymbolManager::Get(sp1)->isinternal = linkage == Linkage::internal_;
                            }
                        }
                    }
                    else if (!ismember(sym) && !istype(sym))
                    {
                        sym->sb->attribs.inheritable.linkage2 = linkage;
                        Optimizer::SymbolManager::Get(sym)->isexport = linkage == Linkage::export_;
                        Optimizer::SymbolManager::Get(sym)->isimport = linkage == Linkage::import_;
                        Optimizer::SymbolManager::Get(sym)->isinternal = linkage == Linkage::internal_;
                    }
                }
            }
            if (sp->tp->tags)
            {
                for (auto sym : *sp->tp->tags)
                {
                    if (isstructured(sym->tp))
                    {
                        sym = basetype(sym->tp)->sp;
                        if (sym->sb->parentClass == sp && !sym->templateParams)
                            MarkDllLinkage(sym, linkage);
                    }
                }
            }
        }
    }
}
static void DoInstantiate(SYMBOL* strSym, SYMBOL* sym, TYPE* tp, std::list<NAMESPACEVALUEDATA*>* nsv, bool isExtern)
{
    STRUCTSYM s;
    if (strSym)
    {
        s.str = strSym;
        addStructureDeclaration(&s);
    }
    if (isfunction(tp))
    {
        SYMBOL* sp = sym;
        std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sym->sb->parentClass);
        DoInstantiateTemplateFunction(tp, &sp, nsv, strSym, templateParams, isExtern);
        sp->sb->attribs.inheritable.linkage2 = sym->sb->attribs.inheritable.linkage2;
        sym = sp;
        sym->sb->parentClass = strSym;
        SetLinkerNames(sym, Linkage::cdecl_);
        if (!comparetypes(basetype(sp->tp)->btp, basetype(tp)->btp, true))
        {
            errorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp);
        }
        if (isExtern)
        {
            sp->sb->dontinstantiate = true;
        }
        else
        {
            sp->sb->dontinstantiate = false;
            sp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
            InsertInline(sp);
        }
    }
    else
    {
        SYMBOL *spi = nullptr, *ssp;
        SYMLIST** p = nullptr;
        if (nsv)
        {
            auto rvl = tablesearchone(sym->name, nsv->front(), false);
            if (rvl)
                spi = rvl;
            else
                errorNotMember(strSym, nsv->front(), sym->sb->decoratedName);
        }
        else
        {
            ssp = getStructureDeclaration();
            if (ssp)
                spi = ssp->tp->syms->Lookup(sym->name);
            else
                spi = globalNameSpace->front()->syms->Lookup(sym->name);
        }
        if (spi)
        {
            SYMBOL* tmpl = spi;
            while (tmpl)
                if (tmpl->sb->templateLevel)
                    break;
                else
                    tmpl = tmpl->sb->parentClass;
            if ((tmpl && spi->sb->storage_class == StorageClass::static_) || spi->sb->storage_class == StorageClass::external_)
            {
                TemplateDataInstantiate(spi, true, isExtern);
                spi->sb->dontinstantiate = isExtern;
                spi->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                if (!comparetypes(sym->tp, spi->tp, true))
                    preverrorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, spi, sym->sb->declfile, sym->sb->declline);
            }
            else
            {
                errorsym(ERR_NOT_A_TEMPLATE, sym);
            }
        }
        else
        {
            errorsym(ERR_NOT_A_TEMPLATE, sym);
        }
    }
    if (strSym)
    {
        dropStructureDeclaration();
    }
}
bool inCurrentTemplate(const char* name)
{
    if (*currents->ptail)
    {
        for (auto t : **currents->ptail)
            if (t.first && !strcmp(name, t.first->name))
                return true;
    }
    return false;
}
bool definedInTemplate(const char* name)
{
    for (auto&& s : structSyms)
    {
        if (s.str && s.str->templateParams)
            for (auto&& t : *s.str->templateParams)
                if (t.first && !strcmp(t.first->name, name))
                {
                    if (t.second->packed)
                    {
                        if (s.str->sb->instantiated && !t.second->byPack.pack)
                            return true;
                        bool rv = true;
                        if (t.second->byPack.pack)
                        {
                            for (auto r : *t.second->byPack.pack)
                                if (!r.second->byClass.val)
                                {
                                    rv = false;
                                    break;
                                }
                        }
                        if (rv)
                            return true;
                    }
                    else
                    {
                        if (t.second->byClass.val)
                            return true;
                    }
                }
    }
    return false;
}

LEXLIST* TemplateDeclaration(LEXLIST* lex, SYMBOL* funcsp, AccessLevel access, StorageClass storage_class, bool isExtern)
{
    SymbolTable<SYMBOL>* oldSyms = localNameSpace->front()->syms;
    lex = getsym();
    localNameSpace->front()->syms = nullptr;
    if (MATCHKW(lex, Keyword::lt_))
    {
        int lasttemplateHeaderCount = templateHeaderCount;
        TYPE* tp = nullptr;
        struct templateListData l;
        int count = 0;
        int oldInstantiatingTemplate = instantiatingTemplate;
        lex = backupsym();
        if (isExtern)
            error(ERR_DECLARE_SYNTAX);

        if (templateNestingCount == 0)
        {
            l.args = nullptr;
            l.ptail = &l.args;
            l.sp = nullptr;
            l.head = l.tail = nullptr;
            l.bodyHead = l.bodyTail = nullptr;
            currents = &l;
        }

        std::stack<std::list<TEMPLATEPARAMPAIR>**> currentHold;

        currentHold.push(currents->plast);
        currents->plast = currents->ptail;
        templateNestingCount++;
        instantiatingTemplate = 0;
        while (MATCHKW(lex, Keyword::template_))
        {
            std::list<TEMPLATEPARAMPAIR>* temp;
            templateHeaderCount++;
            temp = (*currents->ptail) = templateParamPairListFactory.CreateList();
            temp->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            temp->back().second->type = TplType::new_;
            lex = getsym();
            currents->ptail = &(*currents->ptail)->back().second->bySpecialization.next;

            lex = TemplateHeader(lex, funcsp, temp);
            if (temp->size() > 1)
            {
                count++;
            }
        }
        templateNestingCount--;
        if (lex)
        {
            templateNestingCount++;
            inTemplateType = count != 0;  // checks for full specialization...
            fullySpecialized = count == 0;
            lex = declare(lex, funcsp, &tp, storage_class, Linkage::none_, emptyBlockdata, true, false, true, access);
            fullySpecialized = false;
            inTemplateType = false;
            templateNestingCount--;
            instantiatingTemplate = oldInstantiatingTemplate;
            if (!templateNestingCount)
            {
                if (!tp)
                {
                    error(ERR_TEMPLATES_MUST_BE_CLASSES_OR_FUNCTIONS);
                }
                if (l.sp)
                {
                    if (l.sp->sb->storage_class == StorageClass::typedef_ && !l.sp->sb->typeAlias)
                    {
                        errorat(ERR_TYPEDEFS_CANNOT_BE_TEMPLATES, "", l.sp->sb->declfile, l.sp->sb->declline);
                    }
                    if (l.sp && isfunction(l.sp->tp) && l.sp->sb->parentClass && !l.sp->sb->deferredCompile)
                    {
                        SYMBOL* srch = l.sp->sb->parentClass;
                        while (srch)
                        {
                            if (srch->sb->deferredCompile)
                                break;
                            srch = srch->sb->parentClass;
                        }
                        if (srch)
                        {
                            std::list<TEMPLATEPARAMPAIR>** srch1 = currents->plast;
                            while (srch1 && srch1 != currents->ptail)
                            {
                                if ((*srch1)->size() > 1)
                                    break;
                                srch1 = &(*srch1)->front().second->bySpecialization.next;
                            }
                            if (srch1 == currents->ptail)
                            {
                                DoInstantiate(l.sp->sb->parentClass, l.sp, l.sp->tp, l.sp->sb->nameSpaceValues, false);
                            }
                        }
                    }
                }
                FlushLineData("", INT_MAX);
            }
        }
        while (count--)
            dropStructureDeclaration();
        templateHeaderCount = lasttemplateHeaderCount;
        (*currents->plast) = nullptr;
        currents->ptail = currents->plast;
        currents->plast = currentHold.top();
        currentHold.pop();
        if (templateNestingCount == 0)
            currents = nullptr;
    }
    else  // instantiation
    {
        if (KWTYPE(lex, TT_STRUCT))
        {
            Linkage linkage1 = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
            lex = getsym();
            if (MATCHKW(lex, Keyword::declspec_))
            {
                lex = getsym();
                lex = parse_declspec(lex, &linkage1, &linkage2, &linkage3);
            }
            if (!ISID(lex))
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else
            {
                char idname[512];
                SYMBOL* cls = nullptr;
                SYMBOL* strSym = nullptr;
                std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
                lex = id_expression(lex, funcsp, &cls, &strSym, &nsv, nullptr, false, false, idname, 0);
                if (!cls || !isstructured(cls->tp))
                {
                    if (!cls)
                    {
                        errorstr(ERR_NOT_A_TEMPLATE, idname);
                    }
                    else
                    {
                        errorsym(ERR_CLASS_TYPE_EXPECTED, cls);
                    }
                }
                else
                {
                    std::list<TEMPLATEPARAMPAIR>* templateParams = nullptr;
                    SYMBOL* instance;
                    lex = getsym();
                    lex = GetTemplateArguments(lex, funcsp, cls, &templateParams);
                    instance = GetClassTemplate(cls, templateParams, false);
                    if (instance)
                    {
                        MarkDllLinkage(instance, linkage2);
                        if (!isExtern)
                        {
                            instance->sb->explicitlyInstantiated = true;
                            instance->sb->dontinstantiate = false;
                            instance = TemplateClassInstantiate(instance, templateParams, false, StorageClass::global_);
                            referenceInstanceMembers(instance, false);
                        }
                        else if (!instance->sb->explicitlyInstantiated)
                        {
                            instance->sb->dontinstantiate = true;
                            instance = TemplateClassInstantiate(instance, templateParams, false, StorageClass::global_);
                            dontInstantiateInstanceMembers(instance, false);
                        }
                    }
                    else
                    {
                        errorsym(ERR_NOT_A_TEMPLATE, cls);
                    }
                }
            }
        }
        else
        {
            SYMBOL* sym = nullptr;
            Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
            TYPE* tp = nullptr;
            bool defd = false;
            bool notype = false;
            std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            int consdest = 0;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            lex = getBasicType(lex, funcsp, &tp, &strSym, true, funcsp ? StorageClass::auto_ : StorageClass::global_, &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd, &consdest, nullptr, nullptr, false, true,
                               false, false, false);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            lex = getBeforeType(lex, funcsp, &tp, &sym, &strSym, &nsv, true, StorageClass::cast_, &linkage, &linkage2, &linkage3, nullptr,
                                false, consdest, false, false);
            sizeQualifiers(tp);
            if (!sym)
            {
                error(ERR_IDENTIFIER_EXPECTED);
            }
            else if (notype && !consdest)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else
            {
                if (sym->sb->attribs.inheritable.linkage2 == Linkage::none_)
                    sym->sb->attribs.inheritable.linkage2 = linkage2;
                DoInstantiate(strSym, sym, tp, nsv, false);
            }
        }
    }
    localNameSpace->front()->syms = oldSyms;
    return lex;
}
}  // namespace Parser