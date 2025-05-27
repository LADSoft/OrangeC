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
#include "lex.h"
#include "help.h"
#include "mangle.h"
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
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "class.h"
#include "overload.h"
#include "exprpacked.h"

namespace Parser
{

int dontRegisterTemplate;
int instantiatingTemplate;
int inTemplateBody;
int definingTemplate = 0;
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
    definingTemplate = 0;
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
    defaultParsingContext = nullptr;
    inDeduceArgs = 0;
    classTemplateMap.clear();
    classTemplateMap2.clear();
    classInstantiationMap.clear();
    templateNameTag = 1;
    fullySpecialized = false;
    templateDeclarationLevel = 0;
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
        params->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
    }
    return params;
}
void TemplateRegisterDeferred(LexList* lex)
{
    if (lex && templateDeclarationLevel && !dontRegisterTemplate)
    {
        if (!lex->data->registered)
        {
            LexList* cur = globalAllocate<LexList>();
            if (lex->data->type == LexType::l_id_)
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
                    if (tpx.second->byPack.pack && tpx.second->byPack.pack->size() == 1 &&
                        tpx.second->byPack.pack->front().second->packed)
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
                                    for (auto&& lst : *tpl2)
                                    {
                                        Type* hold = quals->CopyType(true, [&lst](Type*& old, Type*& newx) {
                                            if (newx->type == BasicType::templateparam_)
                                                newx = lst.second->byClass.val ? lst.second->byClass.val : lst.second->byClass.dflt;
                                        });
                                        hold->UpdateRootTypes();
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
    std::list<TEMPLATEPARAMPAIR>* out = templateParamPairListFactory.CreateList();
    for (auto&& in : *inx)
    {
        out->push_back(TEMPLATEPARAMPAIR{in.first, in.second});
        if (in.second->packed && in.second->byPack.pack)
        {
            out->back().second = Allocate<TEMPLATEPARAM>();
            out->back().second->type = in.second->type;
            out->back().second->packed = true;
            auto it = in.second->byPack.pack->begin();
            auto ite = in.second->byPack.pack->end();
            for (; n && it != ite; --n, ++it)
                ;
            if (it != ite)
            {
                found = true;
                out->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                out->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, it->second});
            }
        }
    }
    return found ? out : nullptr;
}
std::list<TEMPLATEPARAMPAIR>** expandTemplateSelector(std::list<TEMPLATEPARAMPAIR>** lst, std::list<TEMPLATEPARAMPAIR>* orig,
                                                      Type* tp)
{
    if (tp->sp && tp->sp->sb->templateSelector)
    {
        TEMPLATEPARAMPAIR* cptr = nullptr;

        for (auto&& s : enclosingDeclarations)
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
        if (!cptr || !cptr->second->packed || !cptr->second->byPack.pack)
        {
            if (definingTemplate && (!inTemplateBody || !instantiatingTemplate))
            {
                if (!*lst)
                    *lst = templateParamPairListFactory.CreateList();
                (*lst)->push_back(TEMPLATEPARAMPAIR(orig->front().first, Allocate<TEMPLATEPARAM>()));
                (*lst)->back().second->type = TplType::typename_;
                (*lst)->back().second->byClass.dflt = tp;
            }
            else if (!definingTemplate && (*tp->sp->sb->templateSelector)[1].isTemplate)
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
                        Type *base = sp->tp, *base1 = base;
                        SYMBOL* s = nullptr;
                        for (++sel, ++sel; sel != sele; ++sel)
                        {
                            base = base1;
                            enclosingDeclarations.Add(base->BaseType()->sp);
                            s = classsearch(sel->name, false, false, false);
                            enclosingDeclarations.Drop();
                            auto it1 = sel;
                            ++it1;
                            // could be optimized
                            if (!s || (s && it1 != sele && !s->tp->IsStructured()))
                                break;
                            base1 = s->tp->BaseType();
                        }
                        if (sel == sele && s)
                        {
                            if (!last)
                                last = templateParamPairListFactory.CreateList();
                            last->push_back(TEMPLATEPARAMPAIR(nullptr, Allocate<TEMPLATEPARAM>()));
                            if (s->sb->storage_class == StorageClass::constant_ ||
                                s->sb->storage_class == StorageClass::constexpr_ ||
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

bool constructedInt(LexList* lex, SYMBOL* funcsp)
{
    // depends on this starting a type
    bool rv = false;
    Type* tp;
    LexList* placeholder = lex;
    Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
    bool defd = false;
    bool notype = false;
    bool cont = false;
    tp = nullptr;

    lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
    if (lex->data->type == LexType::l_id_ || MATCHKW(lex, Keyword::classsel_))
    {
        SYMBOL *sp, *strSym = nullptr;
        LexList* placeholder = lex;
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
        tp = TypeGenerator::UnadornedType(lex, funcsp, tp, nullptr, false, funcsp ? StorageClass::auto_ : StorageClass::global_,
                                          &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd, nullptr, nullptr,
                                          nullptr, false, false, false, nullptr, false);
        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
        if (tp->IsInt())
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
LexList* GetTemplateArguments(LexList* lex, SYMBOL* funcsp, SYMBOL* templ, std::list<TEMPLATEPARAMPAIR>** lst)
{
    std::list<TEMPLATEPARAMPAIR>** start = lst;
    int oldnoTn = noTypeNameError;
    noTypeNameError = 0;
    bool first = true;
    Type* tp = nullptr;
    EXPRESSION* exp = nullptr;
    std::list<TEMPLATEPARAMPAIR>::iterator itorig, iteorig = itorig;
    if (templ)
    {
        auto templ1 = templ;
        if (templ1->sb && templ1->sb->maintemplate)
            templ1 = templ1->sb->maintemplate;
        if (templ->tp && templ1->tp->type == BasicType::aggregate_ && templ->tp->syms->size() == 1)
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
            EnterPackedSequence();
            if (MATCHKW(lex, Keyword::typename_) ||
                (((itorig != iteorig && itorig->second->type != TplType::int_) ||
                  (itorig == iteorig && TypeGenerator::StartOfType(lex, nullptr, true) && !constructedInt(lex, funcsp))) &&
                 !MATCHKW(lex, Keyword::sizeof_)))
            {
                LexList* start = lex;
                tp = nullptr;
                if (ISID(lex))
                {
                    if (expandingParams)
                    {
                        auto id = lex->data->value.s.a;
                        lex = getsym();
                        if (MATCHKW(lex, Keyword::ellipse_))
                        {
                            for (auto its = enclosingDeclarations.begin(); its != enclosingDeclarations.end(); ++its)
                            {
                                if ((*its).tmpl)
                                {
                                    auto rv = templatesearch(id, (*its).tmpl);
                                    if (rv)
                                    {
                                        tp = rv->tp;
                                        break;
                                    }
                                }
                            }
                        }
                        if (!tp)
                        {
                            lex = backupsym();
                        }
                    }
                }
                if (!tp)
                {
                    noTypeNameError++;
                    int old = noNeedToSpecialize;
                    noNeedToSpecialize = itorig != iteorig && itorig->second->type == TplType::template_;
                    tp = TypeGenerator::TypeId(lex, funcsp, StorageClass::parameter_, false, true, false);
                    noNeedToSpecialize = old;
                    noTypeNameError--;
                }
                if (!tp)
                    tp = &stdint;
                if (structLevel && tp->IsStructured())
                {
                    if ((!tp->BaseType()->sp->sb->templateLevel || tp->BaseType()->sp->sb->instantiating) &&
                        tp->BaseType()->sp->sb->declaring)
                    {
                        tp->BaseType()->sp->sb->declaringRecursive = true;
                    }
                }
                if (MATCHKW(lex, Keyword::begin_))  // initializer list?
                {
                    if (definingTemplate)
                    {
                        exp = MakeExpression(ExpressionNode::construct_);
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
                    if (tp)
                    {
                        Type* tp1 = tp;
                        while (tp->IsRef())
                            tp = tp->BaseType()->btp;
                        if (definingTemplate && tp->type == BasicType::templateparam_)
                        {
                            if (!*lst)
                                *lst = templateParamPairListFactory.CreateList();
                            (*lst)->push_back(TEMPLATEPARAMPAIR{tp->templateParam->first, tp->templateParam->second});
                            if (tp1->IsRef())
                            {
                                if (tp1->BaseType()->type == BasicType::lref_)
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
                            if (inTemplateSpecialization && !tp->templateParam->second->packed &&
                                (!definingTemplate || instantiatingTemplate))
                                error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                        }
                        else if (tp->type == BasicType::templateparam_)
                        {
                            EnterPackedSequence();
                            AddPackedEntityToSequence(tp->templateParam->first, tp->templateParam->second);
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back(*tp->templateParam);
                            lst = ExpandTemplateArguments(lst, start, funcsp, &a);
                            ClearPackedSequence();
                            LeavePackedSequence();
                        }
                        else if (tp->type == BasicType::templateselector_)
                        {
                            tp->sp->sb->postExpansion = true;
                            std::list<TEMPLATEPARAMPAIR> a(itorig, iteorig);
                            auto tpl = (*tp->sp->sb->templateSelector)[1].templateParams;
                            if (definingTemplate && !instantiatingTemplate)
                            {
                                lst = expandTemplateSelector(lst, &a, tp1);
                            }
                            else
                            {
                                lst = ExpandTemplateArguments(lst, start, funcsp, &a);
                            }
                        }
                        else if (!definingTemplate && tp->type == BasicType::templatedeferredtype_)
                        {
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            a.back().second->type = TplType::typename_;
                            a.back().second->byClass.dflt = tp;
                            lst = ExpandTemplateArguments(lst, start, funcsp, &a);
                        }
                        else if (itorig != iteorig && itorig->second->type == TplType::typename_ && itorig->second->packed &&
                                 tp->IsStructured())
                        {
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                            a.back().second->type = TplType::typename_;
                            a.back().second->byClass.dflt = tp1;
                            lst = ExpandTemplateArguments(lst, start, funcsp, &a);
                        }
                        else
                        {
                            std::list<TEMPLATEPARAMPAIR> a;
                            a.push_back({nullptr, nullptr});
                            a.front().second = Allocate<_templateParam>();
                            if (itorig != iteorig && itorig->second->type == TplType::template_ && tp->IsStructured() &&
                                tp->BaseType()->sp->sb->templateLevel)
                            {
                                a.front().second->type = TplType::template_;
                                a.front().second->byTemplate.dflt = tp->BaseType()->sp;
                            }
                            else
                            {
                                a.front().second->type = TplType::typename_;
                                a.front().second->byClass.dflt = tp1;
                            }
                            lst = ExpandTemplateArguments(lst, start, funcsp, &a);
                        }
                    }
                    (*lst)->back().second->ellipsis = true;
                    if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                    {
                        for (auto&& tpx : *(*lst)->back().second->byPack.pack)
                            tpx.second->ellipsis = true;
                    }
                    lex = getsym();
                    ClearPackedSequence();
                }
                else if (!definingTemplate && tp->type == BasicType::any_)
                {
                    error(ERR_UNKNOWN_TYPE_TEMPLATE_ARG);
                }
                else if (tp && tp->type == BasicType::templateparam_)
                {

                    if (inTemplateArgs > 1 && tp->templateParam->second->packed)
                    {
                        // unpacked pack gets treated as a single template param
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(TEMPLATEPARAMPAIR{tp->templateParam->first, Allocate<TEMPLATEPARAM>()});
                        if (unpackingTemplate && (!definingTemplate || instantiatingTemplate))
                        {
                            auto tpl = LookupPackedInstance(*tp->templateParam);
                            if (tpl)
                            {
                                *(*lst)->back().second = *tpl;
                                if ((*lst)->back().second->byClass.val)
                                {
                                    (*lst)->back().second->byClass.dflt = (*lst)->back().second->byClass.val;
                                    (*lst)->back().second->byClass.val = nullptr;
                                }
                            }
                        }
                        else
                        {
                            if (tp->templateParam->second->byPack.pack && tp->templateParam->second->byPack.pack->size())
                            {
                                *(*lst)->back().second = *tp->templateParam->second->byPack.pack->front().second;
                                AddPackedEntityToSequence(tp->templateParam->first, tp->templateParam->second);
                            }
                            else
                            {
                                *(*lst)->back().second = *tp->templateParam->second;
                                (*lst)->back().second->ellipsis = false;
                                (*lst)->back().second->usedAsUnpacked = true;
                                if (tp->templateParam->second->packed)
                                    AddPackedEntityToSequence((*lst)->back().first, (*lst)->back().second);
                            }
                        }
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
                                    tp->templateParam->second->byTemplate.dflt = tp->BaseType()->sp;
                                else
                                    tp->templateParam->second->byClass.dflt = tp;
                            }
                        }
                        if (tp->templateParam->second->packed)
                            AddPackedEntityToSequence((*lst)->back().first, (*lst)->back().second);
                    }
                    else
                    {
                        std::list<TEMPLATEPARAMPAIR> a;
                        a.push_back(*tp->templateParam);
                        lst = ExpandTemplateArguments(lst, funcsp, &a);
                        if (tp->templateParam->second->packed)
                            AddPackedEntityToSequence((*lst)->back().first, (*lst)->back().second);
                    }
                }
                else if (itorig != iteorig && itorig->second->packed)
                {
                    if (first || (*lst && ((*lst)->empty() || !(*lst)->back().second->packed)))
                    {
                        if (!*lst)
                            *lst = templateParamPairListFactory.CreateList();
                        (*lst)->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                        if (itorig != iteorig && itorig->second->type == TplType::template_ && tp->IsStructured() &&
                            tp->BaseType()->sp->sb->templateLevel)
                        {
                            (*lst)->back().second->type = TplType::template_;
                            (*lst)->back().second->packed = true;
                            (*lst)->back().second->derivedFromUnpacked = true;
                        }
                        else
                        {
                            (*lst)->back().second->type = TplType::typename_;
                            (*lst)->back().second->packed = true;
                            (*lst)->back().second->derivedFromUnpacked = true;
                        }
                        if (itorig != iteorig)
                            (*lst)->back().first = itorig->first;
                        first = false;
                    }
                    if (!(*lst)->back().second->byPack.pack)
                        (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                    auto last = (*lst)->back().second->byPack.pack;
                    last->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
                    if (itorig != iteorig && itorig->second->type == TplType::template_ && tp->IsStructured() &&
                        tp->BaseType()->sp->sb->templateLevel)
                    {
                        last->back().second->type = TplType::template_;
                        last->back().second->byTemplate.dflt = tp->BaseType()->sp;
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
                    if (itorig != iteorig && itorig->second->type == TplType::template_ && tp->IsStructured() &&
                        tp->BaseType()->sp->sb->templateLevel)
                    {
                        (*lst)->back().second->type = TplType::template_;
                        (*lst)->back().second->byTemplate.dflt = tp->BaseType()->sp;
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
                    if (lex->data->type == LexType::l_id_)
                    {
                        SYMBOL* sp;
                        LexList* last = lex;
                        lex =
                            nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, false, false);
                        if (sp && sp->tp->templateParam)
                        {
                            lex = getsym();
                            if ((!MATCHKW(lex, Keyword::rightshift_) && !MATCHKW(lex, Keyword::gt_) &&
                                !MATCHKW(lex, Keyword::comma_)) || sp->tp->templateParam->second->packed)
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
                    LexList* start;
                    bool skip;
                join:
                    skip = false;
                    name = nullptr;
                    start = lex;
                    if (ISID(lex))
                    {
                        for (auto&& s : enclosingDeclarations)
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
                                        std::list<TEMPLATEPARAMPAIR> a(itorig, iteorig);
                                        lst = expandTemplateSelector(lst, &a, name->tp);
                                        (*lst)->back().second->ellipsis = true;
                                        if ((*lst)->back().second->packed && (*lst)->back().second->byPack.pack)
                                        {
                                            for (auto&& tpl1 : *(*lst)->back().second->byPack.pack)
                                                tpl1.second->ellipsis = true;
                                        }
                                        skip = true;
                                        first = false;
                                        lex = getsym();
                                        ClearPackedSequence();
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
                                        std::list<TEMPLATEPARAMPAIR> *last;
                                        if (first)
                                        {
                                            (*lst)->push_back(TEMPLATEPARAMPAIR{ name, Allocate<TEMPLATEPARAM>() });
                                            *(*lst)->back().second = *name->tp->templateParam->second;
                                            (*lst)->back().second->packed = true;
                                            last = (*lst)->back().second->byPack.pack = templateParamPairListFactory.CreateList();
                                        }
                                        else
                                        {
                                            last = (*lst)->back().second->byPack.pack;
                                        }
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
                                        ClearPackedSequence();
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
                                if (parsingTrailingReturnOrUsing && exp->type != ExpressionNode::c_i_)
                                {
                                    std::list<TEMPLATEPARAMPAIR> a;
                                    a.push_back(*exp->v.sp->tp->templateParam);
                                    lst = ExpandTemplateArguments(lst, funcsp, &a);
                                }
                                else
                                {
                                    std::list<TEMPLATEPARAMPAIR> a;
                                    a.push_back(*tp->templateParam);
                                    lst = ExpandTemplateArguments(lst, funcsp, &a);
                                }
//                                if (tp && tp->type == BasicType::templateparam_ && tp->templateParam->second->packed)
//                                    AddPackedEntityToSequence((*lst)->back().first, (*lst)->back().second);
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
                            if (tp)
                            {
                                // lose p
                                std::list<TEMPLATEPARAMPAIR>** tpx = lst;
                                if (definingTemplate && tp->type == BasicType::templateparam_)
                                {
                                    if (!*lst)
                                        *lst = templateParamPairListFactory.CreateList();
                                    if ((*lst)->empty() || (*lst)->back().second->type != TplType::int_ || !(*lst)->back().second->packed)
                                    {
                                        (*lst)->push_back(TEMPLATEPARAMPAIR{ name, Allocate<TEMPLATEPARAM>() });
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
                                    else  if (!tp->templateParam->second->packed)
                                    {                                    
                                        error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
                                    }
                                    else if (tp->templateParam->second->byPack.pack)
                                    {
                                        for (auto tpl : *tp->templateParam->second->byPack.pack)
                                        {
                                            (*lst)->back().second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, tpl.second});
                                         }
                                    }
                                }
                                else if (definingTemplate)
                                {
                                    if (!*lst)
                                        *lst = templateParamPairListFactory.CreateList();
                                    (*lst)->push_back(TEMPLATEPARAMPAIR{ name, Allocate<TEMPLATEPARAM>() });
                                    (*lst)->back().second->packed = true;
                                    (*lst)->back().second->type = TplType::int_;
                                    if (itorig != iteorig)
                                        (*lst)->back().first = itorig->first;
                                    auto last = (*lst)->back().second->bySpecialization.types =
                                        templateParamPairListFactory.CreateList();
                                    last->push_back(TEMPLATEPARAMPAIR{ name, Allocate<TEMPLATEPARAM>() });
                                    last->back().second->type = TplType::int_;
                                    last->back().second->byNonType.tp = tp;
                                    last->back().second->byNonType.dflt = exp;
                                }
                                else if (exp->type != ExpressionNode::packedempty_)
                                {
                                    ExpandTemplateArguments(lst, start, name, (itorig != iteorig) ? itorig->first : nullptr, funcsp,
                                        &tp, &exp);
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
                            ClearPackedSequence();
                            lex = getsym();
                        }
                        else
                        {
                            if (exp)
                            {
                                int oldStaticAssert = inStaticAssert;
                                int oldArgumentNesting = argumentNesting;
                                inStaticAssert = 0;
                                argumentNesting = 0;
                                optimize_for_constants(&exp);
                                argumentNesting = oldArgumentNesting;
                                inStaticAssert = oldStaticAssert;
                                while (exp->type == ExpressionNode::comma_ && exp->right)
                                    exp = exp->right;
                            }
                            else
                            {
                                exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                                diag("GetTemplateArguments: null expression");
                            }
                            if (tp && (int)tp->type == (int)ExpressionNode::templateparam_)
                            {
                                if (!*lst)
                                {
                                    *lst = templateParamPairListFactory.CreateList();
                                }
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
                            else
                            {
                                /*
                                * some of the stuff in chrono is too complex for the compiler right now..
                                if (exp && !IsConstantExpression(exp, false, false)&& (!definingTemplate ||
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
                                    if (!*lst)
                                        *lst = templateParamPairListFactory.CreateList();
                                    (*lst)->push_back(TEMPLATEPARAMPAIR{name, Allocate<TEMPLATEPARAM>()});
                                    if (exp && parsingTrailingReturnOrUsing && exp->type == ExpressionNode::templateparam_ && exp->v.sp)
                                    {
                                        (*lst)->back().first = exp->v.sp;
                                    }
                                    else if (itorig != iteorig)
                                    {
                                        (*lst)->back().first = itorig->first;
                                    }
                                    if (exp->type == ExpressionNode::templateparam_ && exp->v.sp &&
                                        exp->v.sp->tp->BaseType()->templateParam->second->byClass.dflt)
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
                            if (tp && tp->type == BasicType::templateparam_ && tp->templateParam->second->packed)
                                AddPackedEntityToSequence((*lst)->back().first, (*lst)->back().second);
                        }
                    }
                }
            }
            LeavePackedSequence();
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
static bool SameTemplateSpecialization(Type* P, Type* A)
{
    if (!P || !A)
        return false;
    P = P->BaseType();
    A = A->BaseType();
    if (P->IsRef())
        P = P->btp->BaseType();
    if (A->IsRef())
        A = A->btp->BaseType();
    if (!P->IsStructured() || !A->IsStructured())
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

    if (itPL != itePL && itPA != itePA)
    {
        for (; itPL != itePL && itPA != itePA; ++itPL, ++itPA)
        {
            if (itPL->second->type != itPA->second->type)
            {
                break;
            }
            else if (P->sp->sb->instantiated || A->sp->sb->instantiated)
            {
                if (itPL->second->type == TplType::typename_)
                {
                    if (!templateCompareTypes(itPL->second->byClass.dflt, itPA->second->byClass.val, true))
                        break;
                }
                else if (itPL->second->type == TplType::template_)
                {
                    if (!exactMatchOnTemplateParams(itPL->second->byTemplate.args, itPA->second->byTemplate.args))
                        break;
                }
                else if (itPL->second->type == TplType::int_)
                {
                    if (!templateCompareTypes(itPL->second->byNonType.tp, itPA->second->byNonType.tp, true))
                        break;
                    if (itPL->second->byNonType.dflt &&
                        !equalTemplateMakeIntExpression(itPL->second->byNonType.dflt, itPA->second->byNonType.val))
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
        for (; itold != iteold && itsym != itesym; ++itold, ++itsym)
        {
            if (itold->second->type != itsym->second->type)
                return false;
            switch (itold->second->type)
            {
                case TplType::typename_:
                    if (!SameTemplateSpecialization(itold->second->byClass.dflt, itsym->second->byClass.val))
                    {
                        if (!templateCompareTypes(itold->second->byClass.dflt, itsym->second->byClass.val, true))
                            return false;
                        if (!templateCompareTypes(itsym->second->byClass.val, itold->second->byClass.dflt, true))
                            return false;
                    }
                    break;
                case TplType::template_:
                    if (itold->second->byTemplate.dflt != itsym->second->byTemplate.val)
                        return false;
                    break;
                case TplType::int_:
                    if (!templateCompareTypes(itold->second->byNonType.tp, itsym->second->byNonType.tp, true))
                        return false;
                    if (itold->second->byNonType.dflt &&
                        !equalTemplateMakeIntExpression(itold->second->byNonType.dflt, itsym->second->byNonType.val))
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
    Type* tp;

    // maybe we know this specialization
    if (sym->sb->specializations)
    {
        for (auto candidate : *sym->sb->specializations)
        {
            if (candidate->templateParams &&
                exactMatchOnTemplateArgs(templateParams->front().second->bySpecialization.types,
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
        candidate->tp = sym->tp->CopyType();
        candidate->tp->sp = candidate;
        candidate->tp->UpdateRootTypes();
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
    tp = candidate->tp->CopyType();
    tp->UpdateRootTypes();
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
static bool matchTemplatedType(Type* old, Type* sym, bool strict)
{
    while (1)
    {
        if (old->IsConst() == sym->IsConst() && old->IsVolatile() == sym->IsVolatile())
        {
            old = old->BaseType();
            sym = sym->BaseType();
            if (old->type == sym->type || (old->IsFunction() && sym->IsFunction()))
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
    SYMBOL* sd = enclosingDeclarations.GetFirst();
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
                    if (matchOverload(found1->tp, sp->tp))
                        return sp1;
            }
            if (matchOverload(found1->tp, sp->tp))
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
LexList* TemplateArgGetDefault(LexList** lex, bool isExpression)
{
    LexList *rv = nullptr, **cur = &rv;
    LexList *current = *lex, *end = current;
    // this presumes that the template or expression is small enough to be cached...
    // may have to adjust it later
    // have to properly parse the default value, because it may have
    // embedded expressions that use '<'
    if (isExpression)
    {
        Type* tp;
        EXPRESSION* exp;
        end = current;
        end = expression_no_comma(current, nullptr, nullptr, &tp, &exp, nullptr, _F_INTEMPLATEPARAMS);
    }
    else
    {
        Type* tp;
        end = current;
        tp = TypeGenerator::TypeId(end, nullptr, StorageClass::cast_, false, true, false);
    }
    while (current && current != end)
    {
        *cur = Allocate<LexList>();
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
static SYMBOL* templateParamId(Type* tp, const char* name, int tag)
{
    SYMBOL* rv = Allocate<SYMBOL>();
    rv->tp = tp;
    int len = strlen(name);
    char* buf = Allocate<char>(len + 5);
    Utils::StrCpy(buf, len+1, name);
    *(int*)(buf + len + 1) = tag;
    rv->name = buf;
    return rv;
}
static LexList* TemplateHeader(LexList* lex, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* args)
{
    inTemplateHeader++;
    std::list<TEMPLATEPARAMPAIR>*lst = args, *added = nullptr;
    if (needkw(&lex, Keyword::lt_))
    {
        while (1)
        {
            if (MATCHKW(lex, Keyword::gt_) || MATCHKW(lex, Keyword::rightshift_))
                break;
            args->push_back(TEMPLATEPARAMPAIR{nullptr, Allocate<TEMPLATEPARAM>()});
            lex = TemplateArg(lex, funcsp, args->back(), &lst, true);
            if (args && !added)
            {
                added = args;
                enclosingDeclarations.Add(added);
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
static LexList* TemplateArg(LexList* lex, SYMBOL* funcsp, TEMPLATEPARAMPAIR& arg, std::list<TEMPLATEPARAMPAIR>** lst,
                            bool templateParam)
{
    LexList* current = lex;
    LexList* txttype = nullptr;
    switch (KW(lex))
    {
        Type *tp, *tp1;
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
                    Linkage linkage, linkage2, linkage3;
                    if (strsym->tp->type == BasicType::templateselector_)
                    {
                        sp = sym = templateParamId(strsym->tp, strsym->sb->templateSelector->back().name, 0);
                        lex = getsym();
                        tp = strsym->tp;
                        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
                        // get type qualifiers
                        if (!ISID(lex) && !MATCHKW(lex, Keyword::ellipse_))
                        {
                            tp = TypeGenerator::BeforeName(lex, funcsp, tp, &sp, nullptr, nullptr, false, StorageClass::cast_,
                                                           &linkage, &linkage2, &linkage3, nullptr, false, false, true,
                                                           false); /* fixme at file scope init */
                        }
                        goto non_type_join;
                    }
                    else if (ISID(lex))
                    {
                        tp = Type::MakeType(BasicType::templateselector_);
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
                        lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
                        // get type qualifiers
                        if (!ISID(lex) && !MATCHKW(lex, Keyword::ellipse_))
                        {
                            tp = TypeGenerator::BeforeName(lex, funcsp, tp, &sp, nullptr, nullptr, false, StorageClass::cast_,
                                                           &linkage, &linkage2, &linkage3, nullptr, false, false, true,
                                                           false); /* fixme at file scope init */
                        }
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
                    auto hold = lex;
                    lex = getsym();
                    if (MATCHKW(lex,  Keyword::comma_) || MATCHKW(lex, Keyword::assign_) || MATCHKW(lex, Keyword::gt_) || MATCHKW(lex, Keyword::rightshift_) || MATCHKW(lex, Keyword::ellipse_))
                    {
                        Type* tp = Type::MakeType(BasicType::templateparam_);
                        tp->templateParam = &arg;
                        arg.first = templateParamId(tp, hold->data->value.s.a, templateNameTag++);
                    }
                    else
                    {
                        lex = backupsym();
                        goto nontype_mainjoin;
                    }
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
                Type* tp = Type::MakeType(BasicType::templateparam_);
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
                enclosingDeclarations.Drop();
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
                Type* tp = Type::MakeType(BasicType::templateparam_);
                tp->templateParam = &arg;
                arg.first = templateParamId(tp, lex->data->value.s.a, templateNameTag++);
                lex = getsym();
            }
            else
            {
                Type* tp = Type::MakeType(BasicType::templateparam_);
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
            nontype_mainjoin:
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
            tp = TypeGenerator::UnadornedType(lex, funcsp, tp, nullptr, false, funcsp ? StorageClass::auto_ : StorageClass::global_,
                                              &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd, nullptr,
                                              nullptr, nullptr, false, true, false, nullptr, false);
            noTypeNameError--;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            // get type qualifiers
            if (!ISID(lex) && !MATCHKW(lex, Keyword::ellipse_))
            {
                tp = TypeGenerator::BeforeName(lex, funcsp, tp, &sp, nullptr, nullptr, false, StorageClass::cast_, &linkage,
                                               &linkage2, &linkage3, nullptr, false, false, true,
                                               false); /* fixme at file scope init */
            }
            if (MATCHKW(lex, Keyword::ellipse_))
            {
                arg.second->packed = true;
                lex = getsym();
            }
            // get the name
            tp = TypeGenerator::BeforeName(lex, funcsp, tp, &sp, nullptr, nullptr, false, StorageClass::cast_, &linkage, &linkage2,
                                           &linkage3, nullptr, false, false, false, false); /* fixme at file scope init */
            sizeQualifiers(tp);
            if (notype)
            {
                auto itlist = (*lst)->begin();
                auto itelist = (*lst)->end();
                if (sp && itlist != itelist && itlist->first)
                {
                    for (; itlist != itelist; ++itlist)
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
                sp->tp = Type::MakeType(BasicType::templateparam_);
                sp->tp->templateParam = &arg;
                arg.second->type = TplType::int_;
                arg.first = Allocate<SYMBOL>();
                *arg.first = *sp;
                arg.first->sb = nullptr;
                if (tp->IsArray() || tp->IsFunction())
                {
                    if (tp->IsArray())
                        tp = tp->btp;
                    tp = Type::MakeType(BasicType::pointer_, tp);
                }
                arg.second->byNonType.tp = tp;
                auto tp2 = tp;
                while (tp2->IsPtr())
                    tp2 = tp2->BaseType()->btp;
                tp2 = tp2->BaseType();
                if (!tp->IsInt() && (!tp->IsPtr() || tp2->type == BasicType::templateselector_))
                {
                    LexList* end = lex;
                    LexList** cur = &txttype;
                    // if the type didn't resolve, we want to cache it then evaluate it later...
                    while (current && current != end)
                    {
                        *cur = Allocate<LexList>();
                        **cur = *current;
                        (*cur)->next = nullptr;
                        if (ISID(current))
                            (*cur)->data->value.s.a = litlate((*cur)->data->value.s.a);
                        current = current->next;
                        cur = &(*cur)->next;
                    }
                }
                arg.second->byNonType.txttype = txttype;
                if (tp->BaseType()->type != BasicType::templateparam_ && tp->BaseType()->type != BasicType::templateselector_ &&
                    tp->BaseType()->type != BasicType::templatedeferredtype_ && tp->BaseType()->type != BasicType::enum_ &&
                    !tp->IsInt() && !tp->IsPtr() && tp->BaseType()->type != BasicType::lref_ &&
                    (!definingTemplate || instantiatingTemplate || (tp->BaseType()->type != BasicType::any_ && tp->BaseType()->type != BasicType::templatedecltype_)))
                {
                    if (tp->BaseType()->type == BasicType::auto_)
                        RequiresDialect::Feature(Dialect::cpp17, "auto as non-type template parameter type");
                    else
                        error(ERR_NONTYPE_TEMPLATE_PARAMETER_INVALID_TYPE);
                }
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
                        Type* tp = nullptr;
                        EXPRESSION* exp = nullptr;
                        LexList* lex = SetAlternateLex(arg.second->byNonType.txtdflt);
                        lex = expression_no_comma(lex, nullptr, nullptr, &tp, &exp, nullptr, 0);
                        SetAlternateLex(nullptr);
                        if (tp && isintconst(exp))
                        {
                            arg.second->byNonType.dflt = exp;
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
                    for (; its != itse && its->second->type == ito->second->type; ++its)
                        ;
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
        bool firstVariadic = true;
        for (; its != sym->end() && ita != args->end();)
        {
            its->second->initialized = true;
            if (its->second->type == TplType::template_ && ita->second->type == TplType::typename_)
            {
                Type* tp1 = ita->second->byClass.dflt;
                while (tp1 && tp1->type != BasicType::typedef_ && tp1->btp)
                    tp1 = tp1->btp;
                if (!tp1 || (tp1->type != BasicType::typedef_ && !tp1->IsStructured()))
                    return false;
                its->second->byTemplate.val = tp1->sp;
            }
            else
            {
                if (!matchArg(*its, *ita))
                    return false;
                if (its->second->packed)
                {
                    if (firstVariadic)
                    {
                        its->second->byPack.pack = templateParamPairListFactory.CreateList();
                    }
                    if (ita->second->packed)
                    {
                        if (ita->second->byPack.pack)
                        {
                            for (auto&& tpl : *ita->second->byPack.pack)
                            {
                                if (tpl.second->byClass.dflt->type != BasicType::void_)
                                {
                                    its->second->byPack.pack->push_back(tpl);
                                    auto& item = its->second->byPack.pack->back();
                                    auto second = item.second;
                                    item.second = Allocate<TEMPLATEPARAM>();
                                    *item.second = *second;
                                    item.second->byClass.val = item.second->byClass.dflt;
                                    item.second->byClass.dflt = nullptr;
                                }
                            }
                        }
                    }
                    else
                    {
                        its->second->byPack.pack->push_back(*ita);
                        auto& item = its->second->byPack.pack->back();
                        auto second = item.second;
                        item.second = Allocate<TEMPLATEPARAM>();
                        *item.second = *second;
                        item.second->byClass.val = item.second->byClass.dflt;
                        item.second->byClass.dflt = nullptr;
                    }
                }
                else if (ita->second->packed)
                {
                    if (!unpackingTemplate || ita->second->resolved)
                    {
                        auto itx = its;
                        ++itx;
                        if (itx == sym->end() || itx->second->type != ita->second->type || !itx->second->packed)
                        {
                            return false;
                        }
                        auto it = ita->second->byPack.pack->begin();
                        auto ite = ita->second->byPack.pack->end();
                        its->second->byClass.val = it->second->byClass.dflt;
                        ++it;
                        ++its;
                        its->second->byPack.pack = templateParamPairListFactory.CreateList();
                        for (; it != ita->second->byPack.pack->end(); ++it)
                        {
                            its->second->byPack.pack->push_back(TEMPLATEPARAMPAIR{nullptr, it->second});
                        }
                    }
                    else
                    {
                        auto tpl = LookupPackedInstance(*ita);
                        if (!tpl)
                            return false;
                        its->second->byClass.val = tpl->byClass.dflt;
                    }
                }
                else
                {
                    // takes advantage of the fact all the defaults are in the same structure offset...
                    its->second->byClass.val = ita->second->byClass.dflt;
                }
            }
            auto oldPacked = ita->second->packed;
            ++ita;
            if (!its->second->packed || oldPacked || (ita != args->end() && its->second->type != ita->second->type))
            {
                ++its;
                firstVariadic = true;
            }
            else
            {
                firstVariadic = false;
            }
        }
    }
    return true;
}
bool FunctionTemplateCandidate(std::list<TEMPLATEPARAMPAIR>* sym, std::list<TEMPLATEPARAMPAIR>* args)
{
    if (sym && args)
    {
        auto its = sym->begin();
        auto ita = args->begin();
        if (its != sym->end())
            ++its;
        for (; its != sym->end() && ita != args->end();)
        {
            if (its->second->type == TplType::template_ && ita->second->type == TplType::typename_)
            {
                Type* tp1 = ita->second->byClass.dflt;
                while (tp1 && tp1->type != BasicType::typedef_ && tp1->btp)
                    tp1 = tp1->btp;
                if (!tp1 || (tp1->type != BasicType::typedef_ && !tp1->IsStructured()))
                    return false;
            }
            else
            {
                if (!matchArg(*its, *ita))
                    return false;
            }
            auto oldPacked = ita->second->packed;
            ++ita;
            if (!its->second->packed || oldPacked || (ita != args->end() && its->second->type != ita->second->type))
            {
                ++its;
            }
        }
    }
    return true;
}
static bool comparePointerTypes(Type* tpo, Type* tps)
{
    Type* lastIndirect = nullptr;
    while (tpo && tps)
    {
        tpo = tpo->BaseType();
        tps = tps->BaseType();
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
static bool TemplateInstantiationMatchInternal(std::list<TEMPLATEPARAMPAIR>* porig, std::list<TEMPLATEPARAMPAIR>* psym, bool dflt,
                                               bool bySpecialization)
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
        for (; itOrig != iteOrig && itSym != iteSym; ++itOrig, ++itSym)
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
                            for (; itPOrig != itePOrig && itPSym != itePSym; ++itPOrig, ++itPSym)
                            {
                                Type* torig;
                                Type* tsym;
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
                                if (torig->BaseType()->nullptrType != tsym->BaseType()->nullptrType)
                                    return false;
                                if (torig->IsRef() != tsym->IsRef())
                                    return false;
                                if (torig->BaseType()->array != tsym->BaseType()->array)
                                    return false;
                                if (torig->BaseType()->byRefArray != tsym->BaseType()->byRefArray)
                                    return false;
                                if (torig->BaseType()->array && !!torig->BaseType()->esize != !!tsym->BaseType()->esize)
                                    return false;
                                if ((torig->BaseType()->type == BasicType::enum_) != (tsym->BaseType()->type == BasicType::enum_))
                                    return false;
                                if (tsym->type == BasicType::templateparam_)
                                    tsym = tsym->templateParam->second->byClass.val;
                                if ((!templateCompareTypes(torig, tsym, true, false) ||
                                     !templateCompareTypes(tsym, torig, true, false)) &&
                                    !SameTemplate(torig, tsym, true))
                                    break;
                                if (torig->IsRef())
                                    torig = torig->BaseType()->btp;
                                if (tsym->IsRef())
                                    tsym = tsym->BaseType()->btp;
                                if (torig->IsConst() != tsym->IsConst() || torig->IsVolatile() != tsym->IsVolatile())
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
                        Type* torig = (Type*)xorig;
                        Type* tsym = (Type*)xsym;
                        Type* btorig = torig->BaseType();
                        Type* btsym = tsym->BaseType();
                        if (btorig->nullptrType != btsym->nullptrType)
                            return false;
                        if (torig->IsRef() != tsym->IsRef())
                            return false;
                        if (btorig->array != btsym->array)
                            return false;
                        if (btorig->byRefArray != btsym->byRefArray)
                            return false;
                        if (btorig->array && !!btorig->esize != !!btsym->esize)
                            return false;
                        if ((btorig->type == BasicType::enum_) != (btsym->type == BasicType::enum_))
                            return false;
                        if ((!templateCompareTypes(torig, tsym, true, false) || !templateCompareTypes(tsym, torig, true, false)) &&
                            !SameTemplate(torig, tsym, true))
                            return false;
                        if (!comparePointerTypes(torig, tsym))
                            return false;
                        if (torig->IsRef())
                            torig = btorig->btp;
                        if (tsym->IsRef())
                            tsym = btsym->btp;
                        if (torig->IsConst() != tsym->IsConst() || torig->IsVolatile() != tsym->IsVolatile())
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
                                if (!templateCompareTypes(itPOrig->second->byNonType.tp, itPSym->second->byNonType.tp, true))
                                    return false;
                                if (tsym && !equalTemplateMakeIntExpression((EXPRESSION*)torig, (EXPRESSION*)tsym))
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
                        if (!templateCompareTypes(itOrig->second->byNonType.tp, itSym->second->byNonType.tp, true))
                            return false;
                        if (xsym && xorig && !equalTemplateMakeIntExpression((EXPRESSION*)xorig, (EXPRESSION*)xsym))
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
                                auto tsf = ts2->tp->BaseType()->syms->begin();
                                auto tsfe = ts2->tp->BaseType()->syms->end();
                                auto ssf = ss2->tp->BaseType()->syms->begin();
                                auto ssfe = ss2->tp->BaseType()->syms->end();
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
            ++ns, ++os;  // past embedded reference to self
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
                    if (ss->tp->IsStructured())
                    {
                        if (!ss->sb->deferredCompile)
                        {
                            ss->sb->deferredCompile = ts->sb->deferredCompile;
                            ss->tp->InstantiateDeferred();
                            ss->tp = ss->tp->InitializeDeferred();
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
LexList* GetDeductionGuide(LexList* lex, SYMBOL* funcsp, StorageClass storage_class, Linkage linkage, Type** tp)
{
    if (funcsp || storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_ || (*tp)->sp->sb->parentClass)
    {
        errorsym(ERR_DEDUCTION_GUIDE_GLOBAL_OR_NAMESPACE_SCOPE, (*tp)->sp);
    }
    SYMBOL* sp = makeID(StorageClass::member_, &stdint, nullptr, DG_NAME);
    sp->sb->templateLevel = 1;
    Type* functp = TypeGenerator::FunctionParams(lex, funcsp, &sp, &stdint, true, storage_class, false);
    if (needkw(&lex, Keyword::pointsto_))
    {
        sp->templateParams = TemplateGetParams(sp);
        Linkage linkage2, linkage3;
        bool notype, defd;
        int consdest;
        Type* deductionGuide = TypeGenerator::UnadornedType(lex, funcsp, nullptr, nullptr, true, storage_class, &linkage, &linkage2,
                                                            &linkage3, (*tp)->sp->sb->access, &notype, &defd, &consdest, nullptr,
                                                            nullptr, false, false, false, nullptr, false);
        auto sp1 = (*tp)->sp, sp2 = deductionGuide && deductionGuide->IsStructured() && deductionGuide == deductionGuide->BaseType()
                                        ? deductionGuide->sp
                                        : nullptr;
        if (sp2)
        {
            if (sp1->sb->maintemplate)
                sp1 = sp1->sb->maintemplate;
            if (sp2->sb->maintemplate)
                sp2 = sp2->sb->maintemplate;
            if (sp1->sb->parentTemplate)
                sp1 = sp1->sb->parentTemplate;
            if (sp2->sb->parentTemplate)
                sp2 = sp2->sb->parentTemplate;
        }
        if (!deductionGuide || !sp2 || sp1 != sp2)
        {
            errorsym(ERR_DEDUCTION_GUIDE_DOES_NOT_MATCH_CLASS, (*tp)->sp);
        }
        if (sp2)
        {
            auto enclosing = search(sp1->tp->syms, DG_NAME);
            SYMBOL* deductionSym = nullptr;
            if (enclosing)
                deductionSym = searchOverloads(sp, sp1->tp->syms);
            if (!deductionSym)
                deductionSym = insertFunc(sp1, sp);
            deductionSym->sb->deductionGuide = deductionGuide;
        }
        *tp = sp->tp;
    }
    else
    {
        errskim(&lex, skim_semi, true);
    }
    return lex;
}
static bool ValidSpecialization(std::list<TEMPLATEPARAMPAIR>* special, std::list<TEMPLATEPARAMPAIR>* args, bool templateMatch)
{
    if (special && args)
    {
        auto itSpecial = special->begin();
        auto itArgs = args->begin();
        if (itSpecial->second->type == TplType::new_)
            ++itSpecial;
        for (; itSpecial != special->end() && itArgs != args->end(); ++itSpecial, ++itArgs)
        {
            if (itSpecial->second->type != itArgs->second->type)
            {
                if (itArgs->second->type != TplType::typename_ ||
                    (itArgs->second->byClass.dflt->type != BasicType::templateselector_ &&
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
                            !templateCompareTypes(itSpecial->second->byClass.val, itArgs->second->byClass.dflt, true))
                            return false;
                        break;
                    case TplType::template_:
                        if (itArgs->second->byTemplate.dflt &&
                            !ValidSpecialization(itSpecial->second->byTemplate.args,
                                                 itArgs->second->byTemplate.dflt->templateParams, true))
                            return false;
                        break;
                    case TplType::int_:
                        if (!templateCompareTypes(itSpecial->second->byNonType.tp, itArgs->second->byNonType.tp, true))
                            if (!itSpecial->second->byNonType.tp->IsInt() || !itArgs->second->byNonType.tp->IsInt())
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
void DoInstantiateTemplateFunction(Type* tp, SYMBOL** sp, std::list<NAMESPACEVALUEDATA*>* nsv, SYMBOL* strSym,
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
        ssp = enclosingDeclarations.GetFirst();
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
            CallSite* funcparams = Allocate<CallSite>();
            SYMBOL* instance;
            auto hr = tp->BaseType()->syms->begin();
            auto hre = tp->BaseType()->syms->end();
            funcparams->arguments = argumentListFactory.CreateList();
            funcparams->templateParams = templateParams->front().second->bySpecialization.types;
            funcparams->ascall = true;
            if (templateParams->front().second->bySpecialization.types)
                funcparams->astemplate = true;
            if ((*hr)->sb->thisPtr)
                ++hr;
            while (hr != hre)
            {
                auto init = Allocate<Argument>();
                init->tp = (*hr)->tp;
                init->tp->InstantiateDeferred();
                init->exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                funcparams->arguments->push_back(init);
                ++hr;
            }
            if (spi->sb->parentClass)
            {
                funcparams->thistp = Type::MakeType(BasicType::pointer_, spi->sb->parentClass->tp);
                funcparams->thistp->InstantiateDeferred();
                funcparams->thisptr = MakeIntExpression(ExpressionNode::c_i_, 0);
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
                if (sym->tp->IsStructured())
                {
                    sym = sym->tp->BaseType()->sp;
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
                if (!first)  // past the definition of self
                {
                    if (sym->tp->IsStructured())
                    {
                        sym = sym->tp->BaseType()->sp;
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
                    while (IsCastValue(exp) || IsLValue(exp))
                        exp = exp->left;
                    switch (exp->type)
                    {
                        case ExpressionNode::pc_:
                        case ExpressionNode::global_:
                        case ExpressionNode::callsite_:
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
    int oldCount = definingTemplate;
    struct templateListData* oldList = currents;
    definingTemplate = 0;
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
                    for (auto cur : *p->tp->BaseType()->syms)
                    {
                        if (sym->sb->origdeclline == cur->sb->origdeclline &&
                            !strcmp(sym->sb->origdeclfile, cur->sb->origdeclfile) && cur->sb->deferredCompile)
                        {
                            if (matchesCopy(cur, false) == matchesCopy(sym, false) &&
                                matchesCopy(cur, true) == matchesCopy(sym, true))
                            {
                                sym->sb->deferredCompile = cur->sb->deferredCompile;
                                sym->sb->memberInitializers = cur->sb->memberInitializers;
                                sym->sb->pushedTemplateSpecializationDefinition = 1;
                                sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                if (sym->tp->BaseType()->syms && cur->tp->BaseType()->syms)
                                {
                                    auto src = cur->tp->BaseType()->syms->begin();
                                    auto srce = cur->tp->BaseType()->syms->end();
                                    auto dest = sym->tp->BaseType()->syms->begin();
                                    auto deste = sym->tp->BaseType()->syms->end();
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
                                    for (; itsrc != cur->templateParams->end() && itdest != sym->templateParams->end();
                                         ++itsrc, ++itdest)
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
                for (auto cur : *old->tp->BaseType()->syms)
                {
                    if (sym->sb->origdeclline == cur->sb->origdeclline &&
                        !strcmp(sym->sb->origdeclfile, cur->sb->origdeclfile) && cur->sb->deferredCompile)
                    {
                        sym->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                        sym->sb->deferredCompile = cur->sb->deferredCompile;
                        cur->sb->pushedTemplateSpecializationDefinition = 1;
                        if (sym->tp->BaseType()->syms && cur->tp->BaseType()->syms)
                        {
                            auto src = cur->tp->BaseType()->syms->begin();
                            auto srce = cur->tp->BaseType()->syms->end();
                            auto dest = sym->tp->BaseType()->syms->begin();
                            auto deste = sym->tp->BaseType()->syms->end();
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
    definingTemplate = oldCount;
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
                    if (sym->tp->IsStructured())
                    {
                        sym = sym->tp->BaseType()->sp;
                        if (sym->sb->parentClass == sp && !sym->templateParams)
                            MarkDllLinkage(sym, linkage);
                    }
                }
            }
        }
    }
}
static void DoInstantiate(SYMBOL* strSym, SYMBOL* sym, Type* tp, std::list<NAMESPACEVALUEDATA*>* nsv, bool isExtern)
{
    if (strSym)
    {
        enclosingDeclarations.Add(strSym);
    }
    if (tp->IsFunction())
    {
        SYMBOL* sp = sym;
        std::list<TEMPLATEPARAMPAIR>* templateParams = TemplateGetParams(sym->sb->parentClass);
        DoInstantiateTemplateFunction(tp, &sp, nsv, strSym, templateParams, isExtern);
        sp->sb->attribs.inheritable.linkage2 = sym->sb->attribs.inheritable.linkage2;
        sym = sp;
        sym->sb->parentClass = strSym;
        SetLinkerNames(sym, Linkage::cdecl_);
        if (tp->BaseType()->sp)
            InitializeFunctionArguments(tp->BaseType()->sp);
        tp->BaseType()->btp->InstantiateDeferred();
        if (!sp->tp->BaseType()->btp->CompatibleType(tp->BaseType()->btp))
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
            ssp = enclosingDeclarations.GetFirst();
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
            if ((tmpl && spi->sb->storage_class == StorageClass::static_ || spi->sb->storage_class == StorageClass::constant_) ||
                spi->sb->storage_class == StorageClass::external_)
            {
                TemplateDataInstantiate(spi, true, isExtern);
                spi->sb->dontinstantiate = isExtern;
                spi->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                if (!sym->tp->CompatibleType(spi->tp))
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
        enclosingDeclarations.Drop();
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
    for (auto&& s : enclosingDeclarations)
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

LexList* TemplateDeclaration(LexList* lex, SYMBOL* funcsp, AccessLevel access, StorageClass storage_class, bool isExtern)
{
    SymbolTable<SYMBOL>* oldSyms = localNameSpace->front()->syms;
    lex = getsym();
    localNameSpace->front()->syms = nullptr;
    if (MATCHKW(lex, Keyword::lt_))
    {
        int lasttemplateHeaderCount = templateHeaderCount;
        Type* tp = nullptr;
        struct templateListData l;
        int count = 0;
        int oldInstantiatingTemplate = instantiatingTemplate;
        lex = backupsym();
        if (isExtern)
            error(ERR_DECLARE_SYNTAX);

        auto oldcurrents = currents;
        if (++templateDeclarationLevel == 1)
        {
            l.args = nullptr;
            l.ptail = &l.args;
            l.sp = nullptr;
            l.head = l.tail = nullptr;
            l.bodyHead = l.bodyTail = nullptr;
            currents = &l;
        }
        else
        {
            l.sp = nullptr;
        }

        std::stack<std::list<TEMPLATEPARAMPAIR>**> currentHold;

        currentHold.push(currents->plast);
        currents->plast = currents->ptail;
        definingTemplate++;
        while (MATCHKW(lex, Keyword::template_))
        {
            instantiatingTemplate = 0;
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
        definingTemplate--;
        if (lex)
        {
            definingTemplate++;
            inTemplateType = count != 0;  // checks for full specialization...
            fullySpecialized = count == 0;
            lex = declare(lex, funcsp, &tp, storage_class, Linkage::none_, emptyBlockdata, true, false, true, access);
            fullySpecialized = false;
            inTemplateType = false;
            definingTemplate--;
            instantiatingTemplate = oldInstantiatingTemplate;
            if (!definingTemplate)
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
                    if (l.sp && l.sp->tp->IsFunction() && l.sp->sb->parentClass && !l.sp->sb->deferredCompile)
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
            enclosingDeclarations.Drop();
        templateHeaderCount = lasttemplateHeaderCount;
        (*currents->plast) = nullptr;
        currents->ptail = currents->plast;
        currents->plast = currentHold.top();
        currentHold.pop();
        currents = oldcurrents;
        --templateDeclarationLevel;
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
                lex = id_expression(lex, funcsp, &cls, &strSym, &nsv, nullptr, false, false, idname, sizeof(idname), 0);
                if (!cls || !cls->tp->IsStructured())
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
            Type* tp = nullptr;
            bool defd = false;
            bool notype = false;
            std::list<NAMESPACEVALUEDATA*>* nsv = nullptr;
            SYMBOL* strSym = nullptr;
            int consdest = 0;
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            tp = TypeGenerator::UnadornedType(lex, funcsp, tp, &strSym, true, funcsp ? StorageClass::auto_ : StorageClass::global_,
                                              &linkage, &linkage2, &linkage3, AccessLevel::public_, &notype, &defd, &consdest,
                                              nullptr, nullptr, false, true, false, nullptr, false);
            lex = getQualifiers(lex, &tp, &linkage, &linkage2, &linkage3, nullptr);
            tp = TypeGenerator::BeforeName(lex, funcsp, tp, &sym, &strSym, &nsv, true, StorageClass::cast_, &linkage, &linkage2,
                                           &linkage3, nullptr, false, consdest, false, false);
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