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
#include "rtti.h"
#include <stack>
#include <unordered_set>
#include "ccerr.h"
#include "config.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "stmt.h"
#include "mangle.h"
#include "initbackend.h"
#include "expr.h"
#include "lex.h"
#include "help.h"
#include "declcpp.h"
#include "declare.h"
#include "constopt.h"
#include "memory.h"
#include "inline.h"
#include "init.h"
#include "beinterf.h"
#include "types.h"
#include "lex.h"
#include "declcons.h"
#include "libcxx.h"
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "constexpr.h"
#include "overload.h"
#include "class.h"
#include "stmt.h"
#include "exprpacked.h"
#include "Utils.h"
namespace Parser
{
std::set<SYMBOL*> defaultRecursionMap;
bool noExcept = true;

static void genAsnCall(std::list<FunctionBlock*>& b, SYMBOL* cls, SYMBOL* base, int offset, EXPRESSION* thisptr, EXPRESSION* other,
                       bool move, bool isconst);
void createDestructor(SYMBOL* sp);

LexList* FindClass(LexList* lex, SYMBOL* funcsp, SYMBOL** sym)
{
    SYMBOL* encloser = nullptr;
    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
    bool throughClass = false;
    Type* castType = nullptr;
    char buf[512];
    int ov = 0;
    bool namespaceOnly = false;

    *sym = nullptr;

    if (MATCHKW(lex, Keyword::classsel_))
        namespaceOnly = true;
    lex = nestedPath(lex, &encloser, &ns, &throughClass, true, StorageClass::global_, false, 0);
    lex = getIdName(lex, funcsp, buf, sizeof(buf), & ov, &castType);
    if (buf[0])
    {
        *sym = finishSearch(buf, encloser, ns, false, throughClass, namespaceOnly);  // undefined in local context
    }
    return lex;
}
std::list<MEMBERINITIALIZERS*>* GetMemberInitializers(LexList** lex2, SYMBOL* funcsp, SYMBOL* sym)
{
    (void)sym;
    std::list<MEMBERINITIALIZERS*>* rv = memberInitializersListFactory.CreateList();
    LexList *lex = *lex2, *last = nullptr;
    //    if (sym->name != overloadNameTab[CI_CONSTRUCTOR])
    //        error(ERR_Initializer_LIST_REQUIRES_CONSTRUCTOR);
    while (lex != nullptr)
    {
        EnterPackedSequence();
        if (ISID(lex) || MATCHKW(lex, Keyword::classsel_))
        {
            SYMBOL* sym = nullptr;
            lex = FindClass(lex, funcsp, &sym);
            LexList** mylex;
            char name[1024];
            auto v = Allocate<MEMBERINITIALIZERS>();
            v->line = lex->data->errline;
            v->file = lex->data->errfile;
            mylex = &v->initData;
            name[0] = 0;
            if (ISID(lex))
            {
                Utils::StrCpy(name, lex->data->value.s.a);
                lex = getsym();
            }
            v->name = litlate(name);
            if (sym && istype(sym))
                v->basesym = sym;
            if (MATCHKW(lex, Keyword::lt_))
            {
                int paren = 0, tmpl = 0;
                *mylex = Allocate<LexList>();
                **mylex = *lex;
                (*mylex)->prev = last;
                last = *mylex;
                mylex = &(*mylex)->next;
                lex = getsym();
                while (lex && (!MATCHKW(lex, Keyword::gt_) || paren || tmpl))
                {
                    if (MATCHKW(lex, Keyword::openpa_))
                        paren++;
                    if (MATCHKW(lex, Keyword::closepa_))
                        paren--;
                    if (!paren && MATCHKW(lex, Keyword::lt_))
                        tmpl++;
                    if (!paren && (MATCHKW(lex, Keyword::gt_) || MATCHKW(lex, Keyword::rightshift_)))
                        tmpl--;
                    if (lex->data->type == LexType::l_id_)
                        lex->data->value.s.a = litlate(lex->data->value.s.a);
                    *mylex = Allocate<LexList>();
                    if (MATCHKW(lex, Keyword::rightshift_))
                    {
                        lex = getGTSym(lex);
                        **mylex = *lex;
                        (*mylex)->data = Allocate<Lexeme>();
                        *(*mylex)->data = *lex->data;
                    }
                    else
                    {
                        **mylex = *lex;
                        lex = getsym();
                    }
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                }
                if (MATCHKW(lex, Keyword::gt_))
                {
                    *mylex = Allocate<LexList>();
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
            }
            if (MATCHKW(lex, Keyword::openpa_) || MATCHKW(lex, Keyword::begin_))
            {
                Keyword open = KW(lex), close = open == Keyword::openpa_ ? Keyword::closepa_ : Keyword::end_;
                int paren = 0;
                *mylex = Allocate<LexList>();
                **mylex = *lex;
                (*mylex)->prev = last;
                last = *mylex;
                mylex = &(*mylex)->next;
                lex = getsym();
                while (lex && (!MATCHKW(lex, close) || paren))
                {
                    if (MATCHKW(lex, open))
                        paren++;
                    if (MATCHKW(lex, close))
                        paren--;
                    if (lex->data->type == LexType::l_id_)
                        lex->data->value.s.a = litlate(lex->data->value.s.a);
                    *mylex = Allocate<LexList>();
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
                if (MATCHKW(lex, close))
                {
                    *mylex = Allocate<LexList>();
                    **mylex = *lex;
                    (*mylex)->prev = last;
                    last = *mylex;
                    mylex = &(*mylex)->next;
                    lex = getsym();
                }
                if (MATCHKW(lex, Keyword::ellipse_))
                {
                    ClearPackedSequence();
                    v->packed = true;
                    lex = getsym();
                }
                (*mylex) = nullptr;
            }
            else
            {
                error(ERR_MEMBER_INITIALIZATION_REQUIRED);
                skip(&lex, Keyword::closepa_);
                break;
            }
            rv->push_back(v);
        }
        else
        {
            error(ERR_MEMBER_NAME_REQUIRED);
        }
        LeavePackedSequence();
        if (!MATCHKW(lex, Keyword::comma_))
            break;
        lex = getsym();
    }
    *lex2 = lex;
    return rv;
}
void SetParams(SYMBOL* cons)
{
    // c style only
    int base = Optimizer::chosenAssembler->arch->retblocksize;
    if ((cons->tp->BaseType()->btp->IsStructured() && !cons->tp->BaseType()->btp->BaseType()->sp->sb->structuredAliasType) ||
        cons->tp->BaseType()->btp->BaseType()->type == BasicType::memberptr_)
    {
        // handle structured return values
        base += getSize(BasicType::pointer_);
        if (base % Optimizer::chosenAssembler->arch->parmwidth)
            base += Optimizer::chosenAssembler->arch->parmwidth - base % Optimizer::chosenAssembler->arch->parmwidth;
    }
    for (auto sp : *cons->tp->BaseType()->syms)
    {
        StatementGenerator::AssignParam(cons, &base, sp);
    }
    cons->sb->paramsize = base - Optimizer::chosenAssembler->arch->retblocksize;
}
SYMBOL* insertFunc(SYMBOL* sp, SYMBOL* ovl)
{
    SYMBOL* funcs = search(sp->tp->BaseType()->syms, ovl->name);
    ovl->sb->parentClass = sp;
    ovl->sb->internallyGenned = true;
    ovl->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
    ovl->sb->defaulted = true;
    ovl->sb->access = AccessLevel::public_;
    ovl->sb->templateLevel = definingTemplate;
    if (!ovl->sb->decoratedName)
        SetLinkerNames(ovl, Linkage::cdecl_);
    if (!funcs)
    {
        Type* tp = Type::MakeType(BasicType::aggregate_);
        funcs = makeID(StorageClass::overloads_, tp, 0, ovl->name);
        funcs->sb->parentClass = sp;
        tp->sp = funcs;
        SetLinkerNames(funcs, Linkage::cdecl_);
        sp->tp->BaseType()->syms->Add(funcs);
        funcs->sb->parent = sp;
        funcs->tp->syms = symbols->CreateSymbolTable();
        funcs->tp->syms->Add(ovl);
        ovl->sb->overloadName = funcs;
    }
    else if (funcs->sb->storage_class == StorageClass::overloads_)
    {
        funcs->tp->syms->insertOverload(ovl);
        ovl->sb->overloadName = funcs;
    }
    else
    {
        diag("insertFunc: invalid overload tab");
    }
    injectThisPtr(ovl, ovl->tp->BaseType()->syms);
    SetParams(ovl);
    return ovl;
}
static bool BaseWithVirtualDestructor(SYMBOL* sp)
{
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
        {
            SYMBOL* dest = search(b->cls->tp->syms, overloadNameTab[CI_DESTRUCTOR]);
            if (dest)
            {
                dest = dest->tp->syms->front();
                if (dest->sb->storage_class == StorageClass::virtual_)
                    return true;
            }
        }
    return false;
}
static SYMBOL* declareDestructor(SYMBOL* sp)
{
    SYMBOL* rv;
    SYMBOL *func, *sp1;
    Type* tp = Type::MakeType(BasicType::func_, Type::MakeType(BasicType::void_));
    func = makeID(BaseWithVirtualDestructor(sp) ? StorageClass::virtual_ : StorageClass::member_, tp, nullptr,
                  overloadNameTab[CI_DESTRUCTOR]);
    func->sb->xcMode = xc_none;
    func->sb->noExcept = true;
    //    func->sb->attribs.inheritable.linkage2 = sp->sb->attribs.inheritable.linkage2;
    tp->syms = symbols->CreateSymbolTable();
    sp1 = makeID(StorageClass::parameter_, tp->btp, nullptr, AnonymousName());
    tp->syms->Add(sp1);
    if (sp->sb->vbaseEntries)
    {
        sp1 = makeID(StorageClass::parameter_, &stdint, nullptr, AnonymousName());
        sp1->sb->isDestructor = true;
        tp->syms->Add(sp1);
    }
    rv = insertFunc(sp, func);
    rv->sb->isDestructor = true;
    bool found = false;
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
            if (!b->cls->sb->pureDest)
            {
                found = true;
                break;
            }
    if (!found)
    {
        if (sp->sb->vbaseEntries)
            for (auto e : *sp->sb->vbaseEntries)
                if (!e->cls->sb->pureDest)
                {
                    found = true;
                    break;
                }
        if (!found)
        {
            for (auto cls : *sp->tp->BaseType()->syms)
            {
                Type* tp = cls->tp;
                while (tp->IsArray())
                    tp = tp->BaseType()->btp;
                if (tp->IsStructured() && !tp->BaseType()->sp->sb->pureDest)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                sp->sb->pureDest = true;
            }
        }
    }

    return rv;
}
static bool hasConstFunc(SYMBOL* sp, int type, bool move)
{
    SYMBOL* ovl = search(sp->tp->BaseType()->syms, overloadNameTab[type]);
    if (ovl)
    {
        for (auto func : *ovl->tp->BaseType()->syms)
        {
            auto it = func->tp->BaseType()->syms->begin();
            ++it;
            if (it != func->tp->BaseType()->syms->end())
            {
                auto arg = *it;
                ++it;
                if (it == func->tp->BaseType()->syms->end() || (*it)->sb->init || (*it)->sb->deferredCompile)
                {
                    if (arg->tp->IsRef())
                    {
                        if (arg->tp->BaseType()->btp->IsStructured())
                        {
                            if (arg->tp->BaseType()->btp->BaseType()->sp == sp ||
                                SameTemplate(arg->tp->BaseType()->btp->BaseType(), sp->tp))
                            {
                                if ((arg->tp->BaseType()->type == BasicType::lref_ && !move) ||
                                    (arg->tp->BaseType()->type == BasicType::rref_ && move))
                                {
                                    return arg->tp->BaseType()->btp->IsConst();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
static bool constCopyConstructor(SYMBOL* sp)
{
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
            if (!b->isvirtual && !hasConstFunc(b->cls, CI_CONSTRUCTOR, false))
                return false;
    if (sp->sb->vbaseEntries)
        for (auto e : *sp->sb->vbaseEntries)
            if (e->alloc && !hasConstFunc(e->cls, CI_CONSTRUCTOR, false))
                return false;
    for (auto cls : *sp->tp->BaseType()->syms)
        if (cls->tp->IsStructured() && cls->sb->storage_class != StorageClass::typedef_ && !cls->sb->trivialCons)
            if (!hasConstFunc(cls->tp->BaseType()->sp, CI_CONSTRUCTOR, false))
                return false;
    return true;
}
static SYMBOL* declareConstructor(SYMBOL* sp, bool deflt, bool move)
{
    SYMBOL *func, *sp1;
    Type* tp = Type::MakeType(BasicType::func_, Type::MakeType(BasicType::void_));
    func = makeID(StorageClass::member_, tp, nullptr, overloadNameTab[CI_CONSTRUCTOR]);
    func->sb->isConstructor = true;
    //    func->sb->attribs.inheritable.linkage2 = sp->sb->attribs.inheritable.linkage2;
    sp1 = makeID(StorageClass::parameter_, nullptr, nullptr, AnonymousName());
    tp->syms = symbols->CreateSymbolTable();
    tp->syms->Add(sp1);
    if (deflt)
    {
        sp1->tp = Type::MakeType(BasicType::void_);
    }
    else
    {
        sp1->tp = Type::MakeType(move ? BasicType::rref_ : BasicType::lref_, sp->tp->BaseType());
        if (!move && constCopyConstructor(sp))
        {
            sp1->tp->btp = Type::MakeType(BasicType::const_, sp1->tp->btp);
        }
    }
    sp1->tp->UpdateRootTypes();
    return insertFunc(sp, func);
}
static bool constAssignmentOp(SYMBOL* sp, bool move)
{
    if (sp->sb->baseClasses)
        for (auto b : *sp->sb->baseClasses)
            if (!b->isvirtual && !hasConstFunc(b->cls, (int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW, move))
                return false;
    if (sp->sb->vbaseEntries)
        for (auto e : *sp->sb->vbaseEntries)
            if (e->alloc && !hasConstFunc(e->cls, (int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW, move))
                return false;
    for (auto cls : *sp->tp->BaseType()->syms)
        if (cls->tp->IsStructured() && cls->sb->storage_class != StorageClass::typedef_ && !cls->sb->trivialCons)
            if (!hasConstFunc(cls->tp->BaseType()->sp, (int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW, move))
                return false;
    return true;
}
static SYMBOL* declareAssignmentOp(SYMBOL* sp, bool move)
{
    SYMBOL *func, *sp1;
    Type* tp = Type::MakeType(BasicType::func_, sp->tp->BaseType());
    Type* tpx;
    if (sp->tp->IsStructured())
    {
        tp->btp = Type::MakeType(move ? BasicType::rref_ : BasicType::lref_, tp->btp);
    }
    tp->UpdateRootTypes();
    func = makeID(StorageClass::member_, tp, nullptr, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
    sp1 = makeID(StorageClass::parameter_, nullptr, nullptr, AnonymousName());
    tp->syms = symbols->CreateSymbolTable();
    tp->syms->Add(sp1);
    sp1->tp = Type::MakeType(move ? BasicType::rref_ : BasicType::lref_, sp->tp->BaseType());
    if (constAssignmentOp(sp, move))
    {
        sp1->tp->btp = Type::MakeType(BasicType::const_, sp1->tp->btp);
    }
    sp1->tp->UpdateRootTypes();
    return insertFunc(sp, func);
}
bool matchesDefaultConstructor(SYMBOL* sp)
{
    auto it = sp->tp->BaseType()->syms->begin();
    ++it;
    if (it != sp->tp->BaseType()->syms->end())
    {
        SYMBOL* arg1 = *it;
        if (arg1->tp->type == BasicType::void_ || arg1->sb->init || arg1->sb->deferredCompile)
            return true;
    }
    return false;
}
bool matchesCopy(SYMBOL* sp, bool move)
{
    if (sp->sb->parentClass)
    {
        auto it = sp->tp->BaseType()->syms->begin();
        ++it;
        if (it != sp->tp->BaseType()->syms->end())
        {
            SYMBOL* arg1 = *it;
            ++it;
            if (it == sp->tp->BaseType()->syms->end() || (*it)->sb->init || (*it)->sb->deferredCompile || (*it)->sb->constop)
            {
                if (arg1->tp->BaseType()->type == (move ? BasicType::rref_ : BasicType::lref_))
                {
                    Type* tp = arg1->tp->BaseType()->btp;
                    tp->InstantiateDeferred();
                    if (tp->IsStructured())
                        if (tp->BaseType()->sp == sp->sb->parentClass || tp->BaseType()->sp == sp->sb->parentClass->sb->mainsym ||
                            tp->BaseType()->sp->sb->mainsym == sp->sb->parentClass || SameTemplate(tp, sp->sb->parentClass->tp))
                            return true;
                }
            }
        }
    }
    return false;
}
static bool hasCopy(SYMBOL* func, bool move)
{
    for (auto sp : *func->tp->BaseType()->syms)
    {
        if (!sp->sb->internallyGenned && matchesCopy(sp, move))
            return true;
    }
    return false;
}
static bool checkDest(SYMBOL* sp, SYMBOL* parent, SymbolTable<SYMBOL>* syms, AccessLevel access)
{
    SYMBOL* dest = search(syms, overloadNameTab[CI_DESTRUCTOR]);

    if (dest)
    {
        dest = dest->tp->BaseType()->syms->front();
        if (dest->sb->deleted)
            return true;
        if (dest->sb->access < access)
            return true;
    }
    return false;
}
static bool noexceptDest(SYMBOL* sp)
{
    SYMBOL* dest = search(sp->tp->syms, overloadNameTab[CI_DESTRUCTOR]);

    if (dest)
    {
        dest = dest->tp->BaseType()->syms->front();
        return dest->sb->noExcept;
    }
    return true;
}

static bool checkDefaultCons(SYMBOL* sp, SymbolTable<SYMBOL>* syms, AccessLevel access)
{
    SYMBOL* cons = search(syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (cons)
    {
        SYMBOL* dflt = nullptr;
        for (auto cur : *cons->tp->BaseType()->syms)
        {
            if (matchesDefaultConstructor(cur))
            {
                if (dflt)
                    return true;  // ambiguity
                dflt = cur;
            }
        }
        if (dflt)
        {
            if (dflt->sb->deleted)
                return true;
            if (dflt->sb->access < access)
                return true;
        }
        else
        {
            return true;
        }
    }
    return false;
}
SYMBOL* getCopyCons(SYMBOL* base, bool move)
{
    (void)move;
    SYMBOL* ovl = search(base->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    if (ovl)
    {
        for (auto sym2 : *ovl->tp->BaseType()->syms)
        {
            auto itArgs = sym2->tp->BaseType()->syms->begin();
            auto itArgsend = sym2->tp->BaseType()->syms->end();
            auto sym = *itArgs;
            SYMBOL* sym1 = nullptr;
            if (sym->sb->thisPtr)
            {
                ++itArgs;
                if (itArgs != itArgsend)
                    sym = *itArgs;
            }
            auto it1 = itArgs;
            ++it1;
            if (itArgs != itArgsend && it1 != itArgsend)
            {
                sym1 = *it1;
            }
            if (itArgs != itArgsend && (!sym1 || sym1->sb->init || sym1->sb->deferredCompile))
            {
                Type* tp = sym->tp->BaseType();
                if (tp->type == (move ? BasicType::rref_ : BasicType::lref_))
                {
                    tp = tp->btp->BaseType();
                    if (tp->IsStructured())
                    {
                        if (!base->tp->sp)
                        {
                            continue;
                        }
                        if (tp->sp == base->tp->sp || tp->sp == base->tp->sp->sb->mainsym || SameTemplate(tp, base->tp))
                        {
                            return sym2;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}
static SYMBOL* GetCopyAssign(SYMBOL* base, bool move)
{
    (void)move;
    SYMBOL* ovl = search(base->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
    if (ovl)
    {
        for (auto sym2 : *ovl->tp->BaseType()->syms)
        {
            auto itArgs = sym2->tp->BaseType()->syms->begin();
            auto itArgsend = sym2->tp->BaseType()->syms->end();
            auto sym = *itArgs;
            SYMBOL* sym1 = nullptr;
            if (sym->sb->thisPtr)
            {
                ++itArgs;
                if (itArgs != itArgsend)
                    sym = *itArgs;
            }
            auto it1 = itArgs;
            ++it1;
            if (itArgs != itArgsend && it1 != itArgsend)
            {
                sym1 = *it1;
            }
            if (itArgs != itArgsend && (!sym1 || sym1->sb->init || sym1->sb->deferredCompile))
            {
                Type* tp = sym->tp->BaseType();
                if (tp->type == (move ? BasicType::rref_ : BasicType::lref_))
                {
                    tp = tp->btp->BaseType();
                    if (tp->IsStructured())
                    {
                        if (tp->CompatibleType(base->tp) || SameTemplate(tp, base->tp))
                        {
                            return sym2;
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}
bool hasVTab(SYMBOL* sp)
{
    if (sp->sb->vtabEntries)
        for (auto vt : *sp->sb->vtabEntries)
            if (vt->virtuals)
                return true;
    return false;
}
static bool hasTrivialCopy(SYMBOL* sp, bool move)
{
    SYMBOL* dflt;
    if (sp->sb->vbaseEntries || hasVTab(sp))
        return false;
    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            dflt = getCopyCons(base->cls, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    for (auto cls : *sp->tp->BaseType()->syms)
    {
        if (cls->tp->IsStructured())
        {
            dflt = getCopyCons(cls->tp->BaseType()->sp, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    }
    return true;
}
static bool hasTrivialAssign(SYMBOL* sp, bool move)
{
    SYMBOL* dflt;
    if (sp->sb->vbaseEntries || hasVTab(sp))
        return false;
    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            dflt = GetCopyAssign(base->cls, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    for (auto cls : *sp->tp->BaseType()->syms)
    {
        if (cls->tp->IsStructured())
        {
            dflt = getCopyCons(cls->tp->BaseType()->sp, move);
            if (!dflt)
                return false;
            if (!dflt->sb->trivialCons)
                return false;
        }
    }
    return true;
}
static bool checkCopyCons(SYMBOL* sp, SYMBOL* base, AccessLevel access)
{
    SYMBOL* dflt = getCopyCons(base, false);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    else
    {
        return true;
    }
    return false;
}
static bool checkCopyAssign(SYMBOL* sp, SYMBOL* base, AccessLevel access)
{
    SYMBOL* dflt = GetCopyAssign(base, false);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    else
    {
        return true;
    }
    return false;
}
static bool checkMoveCons(SYMBOL* sp, SYMBOL* base, AccessLevel access)
{
    SYMBOL* dflt = getCopyCons(base, true);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    return false;
}
static bool checkMoveAssign(SYMBOL* sp, SYMBOL* base, AccessLevel access)
{
    SYMBOL* dflt = GetCopyAssign(base, true);
    if (dflt)
    {
        if (dflt->sb->deleted)
            return true;
        if (dflt->sb->access < access)
            return true;
    }
    else
    {
        if (!hasTrivialAssign(sp, true))
            return true;
    }
    return false;
}
static bool isDefaultDeleted(SYMBOL* sp)
{
    if (sp->tp->BaseType()->type == BasicType::union_)
    {
        bool allconst = true;
        for (auto sp1 : *sp->tp->BaseType()->syms)
        {
            if (!sp1->tp->IsConst() && sp1->tp->type != BasicType::aggregate_)
                allconst = false;
            if (sp1->tp->IsStructured())
            {
                SYMBOL* consovl = search(sp1->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
                for (auto cons : *consovl->tp->BaseType()->syms)
                {
                    if (matchesDefaultConstructor(cons))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
        if (allconst)
            return true;
    }
    for (auto sp1 : *sp->tp->BaseType()->syms)
    {
        Type* m;
        if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
        {
            if (sp1->tp->IsRef())
                if (!sp1->sb->init)
                    return true;
            if (sp1->tp->BaseType()->type == BasicType::union_)
            {
                bool found = false;
                for (auto member : *sp1->tp->BaseType()->syms)
                {
                    if (!member->tp->IsConst() && member->tp->BaseType()->type != BasicType::aggregate_)
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    return true;
            }
            if (sp1->tp->IsStructured())
            {
                Type* tp = sp1->tp->BaseType();
                if (checkDest(sp, tp->sp, tp->sp->tp->BaseType()->syms, AccessLevel::public_))
                    return true;
            }
            m = sp1->tp;
            if (m->IsArray())
                m = sp1->tp->BaseType()->btp;
            if (m->IsStructured())
            {
                Type* tp = m->BaseType();
                if (checkDefaultCons(sp, tp->sp->tp->BaseType()->syms, AccessLevel::public_))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            if (checkDest(sp, base->cls, base->cls->tp->BaseType()->syms, AccessLevel::protected_))
                return true;
            if (checkDefaultCons(sp, base->cls->tp->BaseType()->syms, AccessLevel::protected_))
                return true;
        }
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                if (checkDest(sp, vbase->cls, vbase->cls->tp->BaseType()->syms, AccessLevel::protected_))
                    return true;
                if (checkDefaultCons(sp, vbase->cls->tp->BaseType()->syms, AccessLevel::protected_))
                    return true;
            }
        }
    return false;
}
static bool isCopyConstructorDeleted(SYMBOL* sp)
{
    if (sp->tp->BaseType()->type == BasicType::union_)
    {
        for (auto sp1 : *sp->tp->BaseType()->syms)
        {
            if (sp1->tp->IsStructured())
            {
                SYMBOL* consovl = search(sp1->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
                for (auto cons : *consovl->tp->BaseType()->syms)
                {
                    if (matchesCopy(cons, false))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
    }
    for (auto sp1 : *sp->tp->BaseType()->syms)
    {
        Type* m;
        if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
        {
            if (sp1->tp->BaseType()->type == BasicType::rref_)
                return true;
            if (sp1->tp->IsStructured())
            {
                Type* tp = sp1->tp->BaseType();
                if (checkDest(sp, tp->sp, tp->sp->tp->BaseType()->syms, AccessLevel::public_))
                    return true;
            }
            m = sp1->tp;
            if (m->IsArray())
                m = sp1->tp->BaseType()->btp;
            if (m->IsStructured())
            {
                if (checkCopyCons(sp, m->BaseType()->sp, AccessLevel::public_))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            if (checkDest(sp, base->cls, base->cls->tp->BaseType()->syms, AccessLevel::protected_))
                return true;
            if (checkCopyCons(sp, base->cls, AccessLevel::protected_))
                return true;
        }
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                if (checkDest(sp, vbase->cls, vbase->cls->tp->BaseType()->syms, AccessLevel::protected_))
                    return true;
                if (checkCopyCons(sp, vbase->cls, AccessLevel::protected_))
                    return true;
            }
        }
    return false;
}
static bool isCopyAssignmentDeleted(SYMBOL* sp)
{
    if (sp->tp->BaseType()->type == BasicType::union_)
    {
        for (auto sp1 : *sp->tp->BaseType()->syms)
        {
            if (sp1->tp->IsStructured())
            {
                SYMBOL* consovl =
                    search(sp1->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
                for (auto cons : *consovl->tp->BaseType()->syms)
                {
                    if (matchesCopy(cons, false))
                        if (!cons->sb->trivialCons)
                            return true;
                }
            }
        }
    }
    for (auto sp1 : *sp->tp->BaseType()->syms)
    {
        Type* m;
        if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
        {
            if (sp1->tp->IsRef())
                return true;
            m = sp1->tp;
            if (m->IsArray())
                m = sp1->tp->BaseType()->btp;
            if (!m->IsStructured() && m->IsConst() && m->type != BasicType::aggregate_)
                return true;

            if (m->IsStructured())
            {
                if (checkCopyAssign(sp, m->BaseType()->sp, AccessLevel::public_))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (checkCopyAssign(sp, base->cls, AccessLevel::protected_))
                return true;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc && checkCopyAssign(sp, vbase->cls, AccessLevel::protected_))
                return true;
    return false;
}
static bool isMoveConstructorDeleted(SYMBOL* sp)
{
    if (sp->tp->BaseType()->type == BasicType::union_)
    {
        for (auto sp1 : *sp->tp->BaseType()->syms)
        {
            if (sp1->tp->IsStructured())
            {
                SYMBOL* consovl =
                    search(sp1->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
                if (consovl)
                {
                    for (auto cons : *consovl->tp->BaseType()->syms)
                    {
                        if (matchesCopy(cons, true))
                            if (!cons->sb->trivialCons)
                                return true;
                    }
                }
            }
        }
    }
    for (auto sp1 : *sp->tp->BaseType()->syms)
    {
        Type* m;
        if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
        {
            if (sp1->tp->IsStructured())
            {
                Type* tp = sp1->tp->BaseType();
                if (checkDest(sp, tp->sp, tp->sp->tp->BaseType()->syms, AccessLevel::public_))
                    return true;
            }
            m = sp1->tp;
            if (m->IsArray())
                m = sp1->tp->BaseType()->btp;
            if (m->IsStructured())
            {
                if (checkMoveCons(sp, m->BaseType()->sp, AccessLevel::public_))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            if (checkDest(sp, base->cls, base->cls->tp->BaseType()->syms, AccessLevel::protected_))
                return true;
            if (checkMoveCons(sp, base->cls, AccessLevel::protected_))
                return true;
        }
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                if (checkDest(sp, vbase->cls, vbase->cls->tp->BaseType()->syms, AccessLevel::protected_))
                    return true;
                if (checkMoveCons(sp, vbase->cls, AccessLevel::protected_))
                    return true;
            }
        }
    return false;
}
static bool isMoveAssignmentDeleted(SYMBOL* sp)
{
    if (sp->tp->BaseType()->type == BasicType::union_)
    {
        for (auto sp1 : *sp->tp->BaseType()->syms)
        {
            if (sp1->tp->IsStructured())
            {
                SYMBOL* consovl =
                    search(sp1->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
                if (consovl)
                {
                    for (auto cons : *consovl->tp->BaseType()->syms)
                    {
                        if (matchesCopy(cons, true))
                            if (!cons->sb->trivialCons)
                                return true;
                    }
                }
            }
        }
    }
    for (auto sp1 : *sp->tp->BaseType()->syms)
    {
        Type* m;
        if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
        {
            if (sp1->tp->IsRef())
                return true;
            if (!sp1->tp->IsStructured() && sp1->tp->IsConst() && sp1->tp->type != BasicType::aggregate_)
                return true;
            m = sp1->tp;
            if (m->IsArray())
                m = sp1->tp->BaseType()->btp;
            if (m->IsStructured())
            {
                if (checkMoveAssign(sp, m->BaseType()->sp, AccessLevel::public_))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (checkMoveAssign(sp, base->cls, AccessLevel::protected_))
                return true;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc && checkMoveAssign(sp, vbase->cls, AccessLevel::protected_))
                return true;
    return false;
}
static bool isDestructorDeleted(SYMBOL* sp)
{
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (sp->tp->BaseType()->type == BasicType::union_)
    {
        return false;
    }
    for (auto sp1 : *sp->tp->BaseType()->syms)
    {
        Type* m;
        if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
        {
            if (sp1->tp->IsStructured())
            {
                Type* tp = sp1->tp->BaseType();
                if (checkDest(sp, tp->sp, tp->sp->tp->BaseType()->syms, AccessLevel::public_))
                    return true;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (checkDest(sp, base->cls, base->cls->tp->BaseType()->syms, AccessLevel::protected_))
                return true;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc)
                if (checkDest(sp, vbase->cls, vbase->cls->tp->BaseType()->syms, AccessLevel::protected_))
                    return true;
    return false;
}
static bool isDestructorNoexcept(SYMBOL* sp)
{
    BASECLASS* base;
    VBASEENTRY* vbase;
    if (sp->tp->BaseType()->type == BasicType::union_)
    {
        return false;
    }
    for (auto sp1 : *sp->tp->BaseType()->syms)
    {
        Type* m;
        if (sp1->sb->storage_class == StorageClass::member_ || sp1->sb->storage_class == StorageClass::mutable_)
        {
            if (sp1->tp->IsStructured())
            {
                if (!noexceptDest(sp1->tp->BaseType()->sp))
                    return false;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
            if (!noexceptDest(base->cls))
                return false;
    if (sp->sb->vbaseEntries)
        for (auto vbase : *sp->sb->vbaseEntries)
            if (vbase->alloc)
                if (!noexceptDest(vbase->cls))
                    return false;
    return true;
}
static void conditionallyDeleteDefaultConstructor(SYMBOL* func)
{
    for (auto sp : *func->tp->BaseType()->syms)
    {
        if (sp->sb->defaulted && matchesDefaultConstructor(sp))
        {
            if (isDefaultDeleted(sp->sb->parentClass))
            {
                sp->sb->deleted = true;
            }
        }
    }
}
static bool conditionallyDeleteCopyConstructor(SYMBOL* func, bool move)
{
    for (auto sp : *func->tp->BaseType()->syms)
    {
        if (sp->sb->defaulted && matchesCopy(sp, move))
        {
            if (move && isMoveConstructorDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
            else if (!move && isCopyConstructorDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
        }
    }
    return false;
}
static bool conditionallyDeleteCopyAssignment(SYMBOL* func, bool move)
{
    for (auto sp : *func->tp->BaseType()->syms)
    {
        if (sp->sb->defaulted && matchesCopy(sp, move))
        {
            if (move && isMoveAssignmentDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
            else if (!move && isCopyAssignmentDeleted(sp->sb->parentClass))
                sp->sb->deleted = true;
        }
    }
    return false;
}
static bool conditionallyDeleteDestructor(SYMBOL* sp)
{
    if (isDestructorDeleted(sp->sb->parentClass))
    {
        sp->sb->deleted = true;
    }
    return false;
}
static void SetDestructorNoexcept(SYMBOL* sp)
{
    if (!definingTemplate || instantiatingTemplate)
    {
        if (sp->sb->deferredNoexcept && sp->sb->deferredNoexcept != (LexList*)-1)
        {
            StatementGenerator::ParseNoExceptClause(sp);
        }
        else if (sp->sb->deferredNoexcept == 0)
        {
            sp->sb->noExcept = isDestructorNoexcept(sp->sb->parentClass);
        }
    }
}
void createConstructorsForLambda(SYMBOL* sp)
{
    SYMBOL* newcons;
    declareDestructor(sp);
    newcons = declareConstructor(sp, true, false);  // default
    newcons->sb->deleted = true;
    newcons = declareConstructor(sp, false, false);  // copy
    conditionallyDeleteCopyConstructor(newcons, false);
    newcons = declareAssignmentOp(sp, false);
    newcons->sb->deleted = true;
    if (!isMoveConstructorDeleted(sp))
    {
        newcons = declareConstructor(sp, false, true);
        //        newcons->sb->deleted = isMoveAssignmentDeleted(sp);
    }
}
// if there is a constructor with defaulted args that would match the
// default constructor,   make a shim default constructor that wraps it...
//
static void shimDefaultConstructor(SYMBOL* sp, SYMBOL* cons)
{
    SYMBOL* match = nullptr;
    for (auto sym : *cons->tp->BaseType()->syms)
    {
        if (matchesDefaultConstructor(sym))
        {
            if (match)
                return;  // will get an error elsewhere because of the duplicate
            match = sym;
        }
    }
    if (match)
    {
        auto it = match->tp->BaseType()->syms->begin();
        auto it1 = it;
        auto itend = match->tp->BaseType()->syms->end();
        ++it1;
        if (it1 != itend && ((*it1)->sb->init || (*it1)->sb->deferredCompile))
        {
            if (sp->templateParams == nullptr || (!definingTemplate || instantiatingTemplate))
            {
                // will match a default constructor but has defaulted args
                SYMBOL* consfunc = declareConstructor(sp, true, false);  // default
                consfunc->sb->defaulted = false;
                SymbolTable<SYMBOL>* syms;
                FunctionBlock bd = {};
                std::list<FunctionBlock*> b = {&bd};
                Statement* st;
                EXPRESSION* thisptr = MakeExpression(ExpressionNode::auto_, consfunc->tp->syms->front());
                EXPRESSION* e1;
                CallSite* params = Allocate<CallSite>();
                (*it)->sb->offset = Optimizer::chosenAssembler->arch->retblocksize;
                Dereference(&stdpointer, &thisptr);
                bd.type = Keyword::begin_;
                syms = localNameSpace->front()->syms;
                localNameSpace->front()->syms = consfunc->tp->BaseType()->syms;
                params->thisptr = thisptr;
                params->thistp = Type::MakeType(BasicType::pointer_, sp->tp);
                params->fcall = MakeExpression(ExpressionNode::pc_, match);
                params->functp = match->tp;
                params->sp = match;
                params->ascall = true;
                params->arguments = argumentListFactory.CreateList();
                AdjustParams(match, match->tp->BaseType()->syms->begin(), match->tp->BaseType()->syms->end(), &params->arguments,
                             false, true);
                if (sp->sb->vbaseEntries)
                {
                    Argument *x = Allocate<Argument>(), **p;
                    x->tp = Type::MakeType(BasicType::int_);
                    x->exp = MakeIntExpression(ExpressionNode::c_i_, 1);
                    params->arguments->push_back(x);
                }
                e1 = MakeExpression(params);
                e1 = MakeExpression(ExpressionNode::thisref_, e1);
                e1->v.t.thisptr = params->thisptr;
                e1->v.t.tp = sp->tp;
                st = Statement::MakeStatement(nullptr, b, StatementNode::return_);
                st->select = e1;
                consfunc->sb->xcMode = cons->sb->xcMode;
                if (consfunc->sb->xc)
                    consfunc->sb->xc->xcDynamic = cons->sb->xc->xcDynamic;
                consfunc->sb->inlineFunc.stmt = stmtListFactory.CreateList();
                auto stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_);
                consfunc->sb->inlineFunc.stmt->push_back(stmt);
                stmt->lower = bd.statements;
                consfunc->sb->inlineFunc.syms = consfunc->tp->BaseType()->syms;
                consfunc->sb->retcount = 1;
                consfunc->sb->attribs.inheritable.isInline = true;
                // now get rid of the first default arg
                // leave others so the old constructor can be considered
                // under other circumstances
                ++it;
                (*it)->sb->init = nullptr;
                localNameSpace->front()->syms = syms;
            }
        }
    }
}
void createDefaultConstructors(SYMBOL* sp)
{
    SYMBOL* cons = search(sp->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    SYMBOL* dest = search(sp->tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
    SYMBOL* asgn = search(sp->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
    SYMBOL* newcons = nullptr;
    if (!dest)
    {
        declareDestructor(sp);
        auto dest1 = search(sp->tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
        SetDestructorNoexcept(dest1->tp->syms->front());
    }
    else
    {
        sp->sb->hasDest = true;
        SetDestructorNoexcept(dest->tp->syms->front());
    }
    if (cons)
    {
        bool defaulted = true;
        for (auto sp1 : *cons->tp->syms)
            if (!sp1->sb->defaulted || sp1->sb->isExplicit)
            {
                defaulted = false;
                break;
            }
        sp->sb->hasUserCons = !defaulted;
        shimDefaultConstructor(sp, cons);
    }
    else
    {
        // create the default constructor
        newcons = declareConstructor(sp, true, false);
        cons = search(sp->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    }
    // see if the default constructor could be trivial
    if (!hasVTab(sp) && sp->sb->vbaseEntries == nullptr && !dest)
    {
        bool found = false;
        if (sp->sb->baseClasses)
            for (auto base : *sp->sb->baseClasses)
                if (!base->cls->sb->trivialCons || base->accessLevel != AccessLevel::public_)
                {
                    found = true;
                    break;
                }
        if (!found)
        {
            bool trivialCons = true;
            bool trivialDest = true;
            for (auto pcls : *sp->tp->BaseType()->syms)
            {
                Type* tp = pcls->tp;
                while (tp->IsArray())
                    tp = tp->BaseType()->btp;
                if (pcls->sb->storage_class == StorageClass::member_ || pcls->sb->storage_class == StorageClass::mutable_ ||
                    pcls->sb->storage_class == StorageClass::overloads_)
                {
                    if (pcls->sb->memberInitializers)
                        trivialCons = false;
                    if (tp->IsStructured())
                    {
                        if (!tp->BaseType()->sp->sb->trivialCons)
                            trivialCons = false;
                    }
                    else if (pcls->sb->storage_class == StorageClass::overloads_)
                    {
                        bool err = false;
                        for (auto s : *tp->BaseType()->syms)
                        {
                            if (err)
                                break;
                            if (s->sb->storage_class != StorageClass::static_)
                            {
                                err |= s->sb->isConstructor && !s->sb->defaulted;
                                err |= s->sb->deleted;
                                err |= s->sb->access != AccessLevel::public_;
                                err |= s->sb->isConstructor && s->sb->isExplicit && !s->sb->defaulted;
                                if (s->sb->isDestructor && !s->sb->defaulted)
                                    trivialDest = false;
                            }
                        }
                        if (err)
                            trivialCons = false;
                    }
                    else if (pcls->sb->access != AccessLevel::public_)
                        trivialCons = false;
                }
            }
            sp->sb->trivialCons = trivialCons;
            sp->sb->trivialDest = trivialDest;
        }
        else
        {
            sp->sb->trivialCons = false;
            sp->sb->trivialDest = false;
        }
        for (auto s : *cons->tp->syms)
        {
            if (s->sb->constexpression | s->sb->defaulted)
                sp->sb->literalClass = true;
        }
    }
    if (newcons)
        newcons->sb->trivialCons = true;
    // now if there is no copy constructor or assignment operator declare them
    if (!hasCopy(cons, false))
    {
        SYMBOL* newcons = declareConstructor(sp, false, false);
        newcons->sb->trivialCons = hasTrivialCopy(sp, false);
        if (hasCopy(cons, true) || (asgn && hasCopy(asgn, true)))
            newcons->sb->deleted = true;
        if (!asgn)
            asgn = search(sp->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
        sp->sb->deleteCopyCons = true;
    }
    if (!asgn || !hasCopy(asgn, false))
    {
        SYMBOL* newsp = declareAssignmentOp(sp, false);
        newsp->sb->trivialCons = hasTrivialAssign(sp, false);
        if (hasCopy(cons, true) || (asgn && hasCopy(asgn, true)))
            newsp->sb->deleted = true;
        if (!asgn)
            asgn = search(sp->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
        sp->sb->deleteCopyAssign = true;
    }
    // now if there is no move constructor, no copy constructor,
    // no copy assignment, no move assignment, no destructor
    // and wouldn't be defined as deleted
    // declare a move constructor and assignment operator
    if (!dest && !hasCopy(cons, false) && !hasCopy(cons, true) && (!asgn || (!hasCopy(asgn, false) && !hasCopy(asgn, true))))
    {
        bool b = isMoveConstructorDeleted(sp);
        SYMBOL* newcons;
        if (!b)
        {
            newcons = declareConstructor(sp, false, true);
            newcons->sb->trivialCons = hasTrivialCopy(sp, true);
        }
        newcons = declareAssignmentOp(sp, true);
        newcons->sb->trivialCons = hasTrivialAssign(sp, true);
        newcons->sb->deleted |= isMoveAssignmentDeleted(sp);
    }
    else
    {
        sp->sb->deleteMove = true;
    }
    // now tag the copy/move assignment operators
    for (auto a : *asgn->tp->syms)
    {
        a->sb->isAssign = true;
    }
}
void ConditionallyDeleteClassMethods(SYMBOL* sp)
{
    auto cons = search(sp->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    auto asgn = search(sp->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
    conditionallyDeleteDefaultConstructor(cons);
    if (sp->sb->deleteCopyCons)
    {
        conditionallyDeleteCopyConstructor(cons, false);
    }
    if (sp->sb->deleteCopyAssign)
    {
        conditionallyDeleteCopyAssignment(asgn, false);
    }
    if (sp->sb->deleteMove)
    {
        conditionallyDeleteCopyConstructor(cons, true);
        conditionallyDeleteCopyAssignment(asgn, true);
    }
    if (sp->sb->defaulted)
    {
        auto dest = search(sp->tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
        conditionallyDeleteDestructor(dest->tp->syms->front());
    }
}
static void genConsData(std::list<FunctionBlock*>& b, SYMBOL* cls, std::list<MEMBERINITIALIZERS*>* mi, SYMBOL* member, int offset,
                        EXPRESSION* thisptr, EXPRESSION* otherptr, SYMBOL* parentCons, bool doCopy)
{
    (void)cls;
    (void)mi;
    if (doCopy && (matchesCopy(parentCons, false) || matchesCopy(parentCons, true)))
    {
        thisptr = MakeExpression(ExpressionNode::structadd_, thisptr, MakeIntExpression(ExpressionNode::c_i_, offset));
        otherptr = MakeExpression(ExpressionNode::structadd_, otherptr, MakeIntExpression(ExpressionNode::c_i_, offset));
        thisptr->right->keepZero = true;
        otherptr->right->keepZero = true;
        if (member->tp->IsStructured() || member->tp->IsArray() || member->tp->BaseType()->type == BasicType::memberptr_)
        {
            EXPRESSION* exp = MakeExpression(ExpressionNode::blockassign_, thisptr, otherptr);
            Statement* st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
            exp->size = member->tp;
            exp->altdata = (void*)member->tp;
            optimize_for_constants(&exp);
            st->select = exp;
        }
        else
        {
            Statement* st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
            EXPRESSION* exp;
            Dereference(member->tp, &thisptr);
            Dereference(member->tp, &otherptr);
            exp = MakeExpression(ExpressionNode::assign_, thisptr, otherptr);
            optimize_for_constants(&exp);
            st->select = exp;
        }
    }
    else if (member->sb->init)
    {
        EXPRESSION* exp;
        Statement* st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
        exp = ConverInitializersToExpression(member->tp, member, nullptr, nullptr, member->sb->init, thisptr, false);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void genConstructorCall(std::list<FunctionBlock*>& b, SYMBOL* cls, std::list<MEMBERINITIALIZERS*>* mi, SYMBOL* member,
                               int memberOffs, bool top, EXPRESSION* thisptr, EXPRESSION* otherptr, SYMBOL* parentCons,
                               bool baseClass, bool doCopy, bool useDefault)
{
    Statement* st = nullptr;
    if (cls != member && member->sb->init)
    {
        EXPRESSION* exp;
        if (member->sb->init->front()->exp)
        {
            exp = ConverInitializersToExpression(member->tp, member, nullptr, nullptr, member->sb->init, thisptr, false);
            if (mi && mi->size() && mi->front()->valueInit)
            {
                auto ths =
                    MakeExpression(ExpressionNode::add_, thisptr, MakeIntExpression(ExpressionNode::c_i_, member->sb->offset));
                auto clr = MakeExpression(ExpressionNode::blockclear_, ths);
                clr->size = member->tp;
                exp = MakeExpression(ExpressionNode::comma_, clr, exp);
            }
        }
        else
        {
            exp = MakeExpression(ExpressionNode::add_, thisptr, MakeIntExpression(ExpressionNode::c_i_, member->sb->offset));
            exp = MakeExpression(ExpressionNode::blockclear_, exp);
            exp->size = member->tp;
        }
        st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
        optimize_for_constants(&exp);
        st->select = exp;
    }
    else
    {
        Type* ctype = member->tp;
        EXPRESSION* exp = MakeExpression(ExpressionNode::add_, thisptr, MakeIntExpression(ExpressionNode::c_i_, memberOffs));
        if (doCopy && matchesCopy(parentCons, false))
        {
            if (useDefault)
            {
                if (!CallConstructor(&ctype, &exp, nullptr, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
            else
            {
                EXPRESSION* other =
                    MakeExpression(ExpressionNode::add_, otherptr, MakeIntExpression(ExpressionNode::c_i_, memberOffs));

                Type* tp = Type::MakeType(BasicType::lref_, member->tp);
                tp->size = getSize(BasicType::pointer_);

                auto it = parentCons->tp->BaseType()->syms->begin();
                ++it;
                if ((*it)->tp->btp->IsConst())
                {
                    tp->btp = Type::MakeType(BasicType::const_, tp->btp);
                }
                tp->UpdateRootTypes();
                if (!callConstructorParam(&ctype, &exp, tp, other, top, false, false, false, true))
                    errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
            }
        }
        else if (doCopy && matchesCopy(parentCons, true))
        {
            if (useDefault)
            {
                if (!CallConstructor(&ctype, &exp, nullptr, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
            else
            {
                EXPRESSION* other =
                    MakeExpression(ExpressionNode::add_, otherptr, MakeIntExpression(ExpressionNode::c_i_, memberOffs));
                other = MakeExpression(ExpressionNode::not__lvalue_, other);
                Type* tp = Type::MakeType(BasicType::rref_, member->tp);

                auto it = parentCons->tp->BaseType()->syms->begin();
                ++it;
                if ((*it)->tp->btp->IsConst())
                {
                    tp->btp = Type::MakeType(BasicType::const_, tp->btp);
                }
                tp->UpdateRootTypes();
                if (!callConstructorParam(&ctype, &exp, tp, other, top, false, false, false, true))
                    errorsym(ERR_NO_APPROPRIATE_CONSTRUCTOR, member);
            }
        }
        else
        {
            MEMBERINITIALIZERS* mix = nullptr;
            if (mi && mi->size() && mi->front() && mi->front()->sp && baseClass)
            {
                for (auto mi2 : *mi)
                {
                    if (mi2->sp)
                    {
                        mi2->sp->tp->InstantiateDeferred();
                        if (mi2->sp->tp->IsStructured() &&
                            (mi2->sp->tp->BaseType()->sp == member || mi2->sp->tp->BaseType()->sp == member->sb->maintemplate ||
                                SameTemplate(mi2->sp->tp, member->tp)))
                                {
                                    mix = mi2;
                                    break;
                    }
                    }
                }
            }
            if (mix)
            {
                CallSite* funcparams = Allocate<CallSite>();
                if (!funcparams->arguments)
                    funcparams->arguments = argumentListFactory.CreateList();
                if (mix->init)
                {
                    for (auto init : *mix->init)
                    {
                        if (!init->exp)
                            break;
                        auto arg = Allocate<Argument>();
                        arg->tp = init->basetp;
                        arg->exp = init->exp;
                        funcparams->arguments->push_back(arg);
                    }
                }
                if (!CallConstructor(&ctype, &exp, funcparams, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
                if (mix->sp && !mix->init)
                {
                    EXPRESSION* clr = MakeExpression(ExpressionNode::blockclear_, exp);
                    clr->size = mix->sp->tp;
                    exp = MakeExpression(ExpressionNode::comma_, clr, exp);
                }
                // previously, CallConstructor can return false here, meaning that funcparams->sp is null
                // This used to create a nullptr dereference in PromoteConstructorArgs
                // Why this is only being found NOW is somewhat dumb, but it is.
                PromoteConstructorArgs(funcparams->sp, funcparams);
            }
            else
            {
                if (!CallConstructor(&ctype, &exp, nullptr, false, nullptr, top, false, false, false, false, false, true))
                    errorsym(ERR_NO_DEFAULT_CONSTRUCTOR, member);
            }
        }
        st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
        optimize_for_constants(&exp);
        st->select = exp;
    }
}
static void virtualBaseThunks(std::list<FunctionBlock*>& b, SYMBOL* sp, EXPRESSION* thisptr)
{
    EXPRESSION *first = nullptr, **pos = &first;
    Statement* st;
    if (sp->sb->vbaseEntries)
    {
        for (auto entry : *sp->sb->vbaseEntries)
        {
            EXPRESSION* left =
                MakeExpression(ExpressionNode::add_, thisptr, MakeIntExpression(ExpressionNode::c_i_, entry->pointerOffset));
            EXPRESSION* right =
                MakeExpression(ExpressionNode::add_, thisptr, MakeIntExpression(ExpressionNode::c_i_, entry->structOffset));
            EXPRESSION* asn;
            Dereference(&stdpointer, &left);
            asn = MakeExpression(ExpressionNode::assign_, left, right);
            if (!*pos)
            {
                *pos = asn;
            }
            else
            {
                *pos = MakeExpression(ExpressionNode::comma_, *pos, asn);
                pos = &(*pos)->right;
            }
        }
    }
    if (first)
    {
        st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void HandleEntries(EXPRESSION** pos, std::list<VTABENTRY*>* entries, EXPRESSION* thisptr, EXPRESSION* vtabBase,
                          bool isvirtual)
{
    std::list<VTABENTRY*>* children = entries->front()->children;
    for (auto entry : *entries)
    {
        if (!entry->isdead && entry->isvirtual == isvirtual && hasVTab(entry->cls))
        {
            EXPRESSION* left =
                MakeExpression(ExpressionNode::add_, thisptr, MakeIntExpression(ExpressionNode::c_i_, entry->dataOffset));
            EXPRESSION* right = MakeExpression(
                ExpressionNode::add_,
                MakeExpression(ExpressionNode::add_, vtabBase, MakeIntExpression(ExpressionNode::c_i_, entry->vtabOffset)),
                MakeIntExpression(ExpressionNode::c_i_, VTAB_XT_OFFS));
            EXPRESSION* asn;
            Dereference(&stdpointer, &left);
            asn = MakeExpression(ExpressionNode::assign_, left, right);
            if (!*pos)
            {
                *pos = asn;
            }
            else
            {
                *pos = MakeExpression(ExpressionNode::comma_, *pos, asn);
                pos = &(*pos)->right;
            }
        }
    }
    if (children)
    {
        HandleEntries(pos, children, thisptr, vtabBase, isvirtual);
    }
}
static void dovtabThunks(std::list<FunctionBlock*>& b, SYMBOL* sym, EXPRESSION* thisptr, bool isvirtual)
{
    auto entries = sym->sb->vtabEntries;
    EXPRESSION* first = nullptr;
    Statement* st;
    SYMBOL* localsp;
    localsp = sym->sb->vtabsp;
    EXPRESSION* vtabBase = MakeExpression(ExpressionNode::global_, localsp);
    if (localsp->sb->attribs.inheritable.linkage2 == Linkage::import_)
        Dereference(&stdpointer, &vtabBase);
    HandleEntries(&first, entries, thisptr, vtabBase, isvirtual);
    if (first)
    {
        st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
        optimize_for_constants(&first);
        st->select = first;
    }
}
static void doVirtualBases(std::list<FunctionBlock*>& b, SYMBOL* sp, std::list<MEMBERINITIALIZERS*>* mi,
                           std::list<VBASEENTRY*>* vbe, EXPRESSION* thisptr, EXPRESSION* otherptr, SYMBOL* parentCons, bool doCopy)
{
    if (vbe && vbe->size())
    {
        for (auto it = vbe->end(); it != vbe->begin();)
        {
            --it;
            if ((*it)->alloc)
                genConstructorCall(b, sp, mi, (*it)->cls, (*it)->structOffset, false, thisptr, otherptr, parentCons, true, doCopy,
                                   false);
        }
    }
}
static EXPRESSION* unshim(EXPRESSION* exp, EXPRESSION* ths);
static std::list<Statement*>* unshimstmt(std::list<Statement*>* block_in, EXPRESSION* ths)
{
    if (block_in)
    {
        std::list<Statement*>* rv = stmtListFactory.CreateList();
        for (auto block : *block_in)
        {
            auto v = Allocate<Statement>();
            *v = *block;
            block = v;
            rv->push_back(block);
            switch (block->type)
            {
                case StatementNode::genword_:
                    break;
                case StatementNode::try_:
                case StatementNode::catch_:
                case StatementNode::seh_try_:
                case StatementNode::seh_catch_:
                case StatementNode::seh_finally_:
                case StatementNode::seh_fault_:
                    block->lower = unshimstmt(block->lower, ths);
                    block->blockTail = unshimstmt(block->blockTail, ths);
                    break;
                case StatementNode::return_:
                case StatementNode::expr_:
                case StatementNode::declare_:
                    block->select = unshim(block->select, ths);
                    break;
                case StatementNode::goto_:
                case StatementNode::label_:
                    break;
                case StatementNode::select_:
                case StatementNode::notselect_:
                    block->select = unshim(block->select, ths);
                    break;
                case StatementNode::switch_:
                    block->select = unshim(block->select, ths);
                    block->lower = unshimstmt(block->lower, ths);
                    break;
                case StatementNode::block_:
                    block->lower = unshimstmt(block->lower, ths);
                    block->blockTail = unshimstmt(block->blockTail, ths);
                    break;
                case StatementNode::passthrough_:
                    break;
                case StatementNode::datapassthrough_:
                    break;
                case StatementNode::nop_:
                    break;
                case StatementNode::line_:
                case StatementNode::varstart_:
                case StatementNode::dbgblock_:
                    break;
                default:
                    diag("Invalid block type in unshimstmt");
                    break;
            }
        }
        return rv;
    }
    return nullptr;
}

static EXPRESSION* unshim(EXPRESSION* exp, EXPRESSION* ths)
{
    EXPRESSION* nw = nullptr;
    if (!exp)
        return exp;
    if (exp->type == ExpressionNode::thisshim_)
    {
        return ths;
    }
    nw = Allocate<EXPRESSION>();
    *nw = *exp;
    nw->left = unshim(nw->left, ths);
    nw->right = unshim(nw->right, ths);
    if (nw->type == ExpressionNode::callsite_)
    {
        if (nw->v.func->sp && nw->v.func->arguments && nw->v.func->arguments->size())
        {
            auto a = nw->v.func->sp->name;
            if (a[0] == '_' && a[1] == '_' && !strcmp(a, "__arrCall"))
            {
                for (auto&& e : *nw->v.func->arguments)
                {
                    e->exp = unshim(e->exp, ths);
                }
            }
        }
    }
    else if (nw->type == ExpressionNode::stmt_)
    {
        nw->v.stmt = unshimstmt(nw->v.stmt, ths);
    }
    return nw;
}
SYMBOL* findClassName(const char* name, SYMBOL* cls, std::list<BASECLASS*>* bc, std::list<VBASEENTRY*>* vbases, int* offset)
{
    int n = 0;
    char str[1024];
    char* clslst[100];
    char *p = str, *c;
    SYMBOL* sp = nullptr;
    int vcount = 0, ccount = 0;
    Utils::StrCpy(str, name);
    while ((c = (char*)strstr(p, "::")))
    {
        clslst[n++] = p;
        p = c;
        *p = 0;
        p += 2;
    }
    clslst[n++] = p;

    if (bc)
    {
        for (auto base : *bc)
        {
            if (!base->isvirtual)
            {
                SYMBOL* parent = base->cls;
                int i;
                if (parent->tp->type == BasicType::typedef_)
                    parent = parent->tp->BaseType()->sp;
                for (i = n - 1; i >= 0 && parent;
                     i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                    if (strcmp(parent->name, clslst[i]))
                        break;
                if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
                {
                    ccount++;
                    sp = base->cls;
                    *offset = base->offset;
                }
            }
        }
    }
    if (vbases)
        for (auto vbase : *vbases)
        {
            SYMBOL* parent = vbase->cls;
            int i;
            if (parent->tp->type == BasicType::typedef_)
                parent = parent->tp->BaseType()->sp;
            for (i = n - 1; i >= 0 && parent;
                 i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                if (strcmp(parent->name, clslst[i]))
                    break;
            if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
            {
                vcount++;
                sp = vbase->cls;
                *offset = vbase->structOffset;
            }
        }
    if ((ccount && vcount) || ccount > 1)
    {
        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, sp, cls);
    }
    return sp;
}
void ParseMemberInitializers(SYMBOL* cls, SYMBOL* cons)
{
    bool first = true;
    bool hasDelegate = false;
    if (cons->sb->memberInitializers)
    {
        enclosingDeclarations.Mark();
        auto ite = cons->sb->memberInitializers->end();
        if (cons->templateParams)
        {
            enclosingDeclarations.Add(cons->templateParams);
        }
        for (auto it = cons->sb->memberInitializers->begin(); it != ite;)
        {
            auto init = *it;
            LexList* lex;
            if (!first && hasDelegate)
                error(ERR_DELEGATING_CONSTRUCTOR_ONLY_Initializer);
            init->sp = search(cls->tp->BaseType()->syms, init->name);
            if (init->sp && (!init->basesym || !istype(init->sp)))
            {
                if (init->sp->sb->storage_class == StorageClass::typedef_)
                {
                    Type* tp = init->sp->tp->BaseType();
                    if (tp->IsStructured())
                    {
                        init->name = tp->BaseType()->sp->name;
                        init->sp = search(cls->tp->BaseType()->syms, init->name);
                    }
                }
            }
            if (init->sp && (!init->basesym || !istype(init->sp)))
            {
                if (init->sp != cls && init->sp->sb->storage_class != StorageClass::member_ &&
                    init->sp->sb->storage_class != StorageClass::mutable_)
                {
                    errorsym(ERR_NEED_NONSTATIC_MEMBER, init->sp);
                }
                else
                {
                    bool done = false;
                    lex = SetAlternateLex(init->initData);
                    if (MATCHKW(lex, Keyword::lt_))
                    {
                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                        lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                        if (init->sp->sb->templateLevel)
                        {
                            init->sp = TemplateClassInstantiate(init->sp, lst, false, StorageClass::global_);
                        }
                        else
                        {
                            errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                        }
                    }
                    if (!init->sp->tp->IsStructured())
                    {
                        bool bypa = true;
                        if (MATCHKW(lex, Keyword::openpa_) || MATCHKW(lex, Keyword::begin_))
                        {
                            bypa = MATCHKW(lex, Keyword::openpa_);
                            lex = getsym();
                            if ((bypa && MATCHKW(lex, Keyword::closepa_)) || (!bypa && MATCHKW(lex, Keyword::end_)))
                            {
                                lex = getsym();
                                init->init = nullptr;
                                InsertInitializer(&init->init, init->sp->tp, MakeIntExpression(ExpressionNode::c_i_, 0), 0, false);
                                done = true;
                            }
                            else
                            {
                                lex = backupsym();
                            }
                        }
                        if (!done)
                        {
                            needkw(&lex, bypa ? Keyword::openpa_ : Keyword::begin_);
                            init->init = nullptr;
                            argumentNesting++;
                            lex = initType(lex, cons, 0, StorageClass::auto_, &init->init, nullptr, init->sp->tp, init->sp, false,
                                           false, _F_MEMBERINITIALIZER);
                            argumentNesting--;
                            done = true;
                            needkw(&lex, bypa ? Keyword::closepa_ : Keyword::end_);
                        }
                    }
                    else
                    {
                        bool empty = false;
                        if (MATCHKW(lex, Keyword::openpa_))
                        {
                            lex = getsym();
                            if (MATCHKW(lex, Keyword::closepa_))
                                empty = true;
                            lex = backupsym();
                        }
                        if (MATCHKW(lex, Keyword::openpa_) && init->sp->tp->BaseType()->sp->sb->trivialCons)
                        {
                            init->init = nullptr;
                            argumentNesting++;
                            lex = initType(lex, cons, 0, StorageClass::auto_, &init->init, nullptr, init->sp->tp, init->sp, false,
                                           false, _F_MEMBERINITIALIZER);
                            argumentNesting--;
                            done = true;
                            if (init->packed || MATCHKW(lex, Keyword::ellipse_))
                                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        }
                        else
                        {
                            init->init = nullptr;
                            lex = initType(lex, cons, 0, StorageClass::auto_, &init->init, nullptr, init->sp->tp, init->sp, false,
                                           false, _F_MEMBERINITIALIZER);
                            if (init->packed)
                                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        }
                        init->valueInit = empty;
                    }
                    SetAlternateLex(nullptr);
                }
            }
            else
            {
                SYMBOL* sp = init->basesym;
                if (!sp)
                    sp = classsearch(init->name, false, false, true);
                else
                    init->sp = sp;
                if (sp && sp->tp->type == BasicType::templateparam_)
                {
                    if (sp->tp->templateParam->second->type == TplType::typename_)
                    {
                        if (sp->tp->templateParam->second->packed)
                        {
                            CallSite shim;
                            lex = SetAlternateLex(init->initData);
                            shim.arguments = nullptr;
                            getMemberInitializers(lex, cons, &shim,
                                                  MATCHKW(lex, Keyword::openpa_) ? Keyword::closepa_ : Keyword::end_, true);
                            if (!init->packed)
                                error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                            SetAlternateLex(nullptr);
                            expandPackedMemberInitializers(cls, cons, sp->tp->templateParam->second->byPack.pack,
                                                           &cons->sb->memberInitializers, init->initData, shim.arguments);
                            init->sp = cls;
                        }
                        else if (sp->tp->templateParam->second->byClass.val &&
                                 sp->tp->templateParam->second->byClass.val->IsStructured())
                        {
                            Type* tp = sp->tp->templateParam->second->byClass.val;
                            int offset = 0;
                            int vcount = 0, ccount = 0;
                            init->name = tp->BaseType()->sp->name;
                            if (cls->sb->baseClasses)
                            {
                                for (auto bc : *cls->sb->baseClasses)
                                {
                                    if (!strcmp(bc->cls->name, init->name))
                                    {
                                        if (bc->isvirtual)
                                            vcount++;
                                        else
                                            ccount++;
                                        init->sp = bc->cls;
                                        offset = bc->offset;
                                    }
                                }
                            }
                            if ((ccount && vcount) || ccount > 1)
                            {
                                errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, init->sp, cls);
                            }
                            if (init->sp && init->sp == tp->BaseType()->sp)
                            {
                                SYMBOL* sp = makeID(StorageClass::member_, init->sp->tp, nullptr, init->sp->name);
                                CallSite shim;
                                sp->sb->offset = offset;
                                init->sp = sp;
                                lex = SetAlternateLex(init->initData);
                                shim.arguments = nullptr;
                                getMemberInitializers(lex, cons, &shim,
                                                      MATCHKW(lex, Keyword::openpa_) ? Keyword::closepa_ : Keyword::end_, true);
                                if (init->packed)
                                    error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                                SetAlternateLex(nullptr);
                                if (shim.arguments)
                                {
                                    init->init = initListFactory.CreateList();
                                    for (auto a : *shim.arguments)
                                    {
                                        auto xinit = Allocate<Initializer>();
                                        xinit->basetp = a->tp;
                                        xinit->exp = a->exp;
                                        init->init->push_back(xinit);
                                    }
                                }
                            }
                            else
                            {
                                init->sp = nullptr;
                            }
                        }
                        else
                        {
                            error(ERR_STRUCTURED_TYPE_EXPECTED_IN_TEMPLATE_PARAMETER);
                        }
                    }
                    else
                    {
                        error(ERR_CLASS_TEMPLATE_PARAMETER_EXPECTED);
                    }
                }
                else if (init->packed)
                {
                    EnterPackedSequence();
                    expandPackedBaseClasses(cls, cons, it, ite, cons->sb->memberInitializers, cls->sb->baseClasses,
                                            cls->sb->vbaseEntries);
                    ClearPackedSequence();
                    LeavePackedSequence();
                    continue;
                }
                else
                {
                    int offset = 0;
                    // check for a delegating constructor call
                    if (!strcmp(init->name, cls->name))
                    {
                        init->sp = cls;
                        init->delegating = true;
                        if (!first)
                            error(ERR_DELEGATING_CONSTRUCTOR_ONLY_Initializer);
                        hasDelegate = true;
                        cons->sb->delegated = true;
                    }
                    if (!init->sp)
                        init->sp = findClassName(init->name, cls, cls->sb->baseClasses, cls->sb->vbaseEntries, &offset);
                    if (init->sp)
                    {
                        // have to make a *real* variable as a fudge...
                        SYMBOL* sp;
                        CallSite shim;
                        lex = SetAlternateLex(init->initData);
                        if (MATCHKW(lex, Keyword::lt_))
                        {
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                            if (init->sp->sb->templateLevel)
                            {
                                init->sp = TemplateClassInstantiate(init->sp, lst, false, StorageClass::global_);
                            }
                            else
                            {
                                errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                            }
                        }
                        sp = makeID(StorageClass::member_, init->sp->tp, nullptr, init->sp->name);
                        sp->sb->offset = offset;
                        init->sp = sp;
                        shim.arguments = nullptr;
                        getMemberInitializers(lex, cons, &shim, MATCHKW(lex, Keyword::openpa_) ? Keyword::closepa_ : Keyword::end_,
                                              true);
                        SetAlternateLex(nullptr);
                        if (init->packed)
                        {
                            error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                        }
                        if (shim.arguments)
                        {
                            init->init = initListFactory.CreateList();
                            for (auto a : *shim.arguments)
                            {
                                auto xinit = Allocate<Initializer>();
                                xinit->basetp = a->tp;
                                xinit->exp = a->exp;
                                init->init->push_back(xinit);
                            }
                        }
                    }
                }
            }
            if (!init->sp)
            {
                // might be a typedef?
                init->sp = finishSearch(init->name, nullptr, nullptr, false, false, false);
                if (init->sp && init->sp->sb->storage_class == StorageClass::typedef_)
                {
                    int offset = 0;
                    Type* tp = init->sp->tp;
                    tp = tp->BaseType();
                    if (tp->IsStructured())
                    {
                        bool found = false;
                        if (cls->sb->baseClasses)
                            for (auto bc : *cls->sb->baseClasses)
                            {
                                if (!bc->cls->tp->CompatibleType(init->sp->tp) || SameTemplate(bc->cls->tp, init->sp->tp))
                                {
                                    found = true;
                                    break;
                                }
                            }
                        if (found)
                        {
                            // have to make a *real* variable as a fudge...
                            SYMBOL* sp;
                            CallSite shim;
                            lex = SetAlternateLex(init->initData);
                            if (MATCHKW(lex, Keyword::lt_))
                            {
                                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                                lex = GetTemplateArguments(lex, cons, init->sp, &lst);
                                if (init->sp->sb->templateLevel)
                                {
                                    init->sp = TemplateClassInstantiate(init->sp, lst, false, StorageClass::global_);
                                }
                                else
                                {
                                    errorsym(ERR_NOT_A_TEMPLATE, init->sp);
                                }
                            }
                            sp = makeID(StorageClass::member_, init->sp->tp, nullptr, init->sp->name);
                            sp->sb->offset = offset;
                            init->sp = sp;
                            shim.arguments = nullptr;
                            getMemberInitializers(lex, cons, &shim,
                                                  MATCHKW(lex, Keyword::openpa_) ? Keyword::closepa_ : Keyword::end_, true);
                            if (init->packed)
                                error(ERR_PACK_SPECIFIER_NOT_ALLOWED_HERE);
                            SetAlternateLex(nullptr);
                            if (shim.arguments)
                            {
                                init->init = initListFactory.CreateList();
                                for (auto a : *shim.arguments)
                                {
                                    auto xinit = Allocate<Initializer>();
                                    xinit->basetp = a->tp;
                                    xinit->exp = a->exp;
                                    init->init->push_back(xinit);
                                }
                            }
                        }
                    }
                    else
                    {
                        init->sp = nullptr;
                    }
                }
                else
                {
                    init->sp = nullptr;
                }
            }
            if (!init->sp)
            {
                errorstrsym(ERR_NOT_A_MEMBER_OR_BASE_CLASS, init->name, cls);
            }
            ++it;
            first = false;
        }
        enclosingDeclarations.Release();
    }
}
static void allocInitializers(SYMBOL* cls, SYMBOL* cons, EXPRESSION* ths)
{
    for (auto sp : *cls->tp->BaseType()->syms)
    {
        if (sp->sb->storage_class == StorageClass::member_ || sp->sb->storage_class == StorageClass::mutable_)
        {
            sp->sb->lastInit = sp->sb->init;
            if (sp->sb->init)
            {
                sp->sb->init = initListFactory.CreateList();
                *sp->sb->init = *sp->sb->lastInit;
                sp->sb->init->front()->exp = unshim(sp->sb->init->front()->exp, ths);
            }
        }
    }
    if (cons->sb->memberInitializers)
    {
        for (auto init : *cons->sb->memberInitializers)
        {
            if (init->init)
            {
                init->sp->sb->init = init->init;
                if (init->init->size() && init->init->front()->exp)
                    init->init->front()->exp = unshim(init->init->front()->exp, ths);
            }
        }
    }
    if (!cons->sb->delegated)
    {
        for (auto sp : *cls->tp->BaseType()->syms)
        {
            if (!sp->sb->init && ismember(sp))
            {
                if (sp->tp->IsRef())
                    errorsym(ERR_REF_MEMBER_MUST_INITIALIZE, sp);
                else if (sp->tp->IsConst())
                    errorsym(ERR_CONSTANT_MEMBER_MUST_BE_INITIALIZED, sp);
            }
        }
    }
}
static void releaseInitializers(SYMBOL* cls, SYMBOL* cons)
{
    (void)cons;
    for (auto sp : *cls->tp->BaseType()->syms)
    {
        if (sp->sb->storage_class == StorageClass::member_ || sp->sb->storage_class == StorageClass::mutable_)
            sp->sb->init = sp->sb->lastInit;
    }
}
EXPRESSION* thunkConstructorHead(std::list<FunctionBlock*>& b, SYMBOL* sym, SYMBOL* cons, SymbolTable<SYMBOL>* syms,
                                 bool parseInitializers, bool doCopy, bool defaulted)
{
    BASECLASS* bc;
    EXPRESSION* thisptr = MakeExpression(ExpressionNode::auto_, syms->front());
    EXPRESSION* otherptr = nullptr;
    int oldCodeLabel = codeLabel;
    if (defaulted)
        codeLabel = INT_MIN;
    auto it = syms->begin();
    ++it;
    if (it != syms->end())
        otherptr = MakeExpression(ExpressionNode::auto_, *it);
    Dereference(&stdpointer, &thisptr);
    Dereference(&stdpointer, &otherptr);
    if (parseInitializers)
        allocInitializers(sym, cons, thisptr);
    if (cons->sb->memberInitializers && cons->sb->memberInitializers->size() && cons->sb->memberInitializers->front()->delegating)
    {
        genConstructorCall(b, sym, cons->sb->memberInitializers, sym, 0, true, thisptr, otherptr, cons, true, doCopy,
                           !cons->sb->defaulted);
    }
    else
    {
        if (sym->tp->type == BasicType::union_)
        {
            StatementGenerator::AllocateLocalContext(emptyBlockdata, cons, codeLabel++);
            auto spMatch = cons->sb->memberInitializers && cons->sb->memberInitializers->size() == 1 ? cons->sb->memberInitializers->front()->sp : nullptr;
            for (auto sp : *sym->tp->BaseType()->syms)
            {
                if ((sp->sb->storage_class == StorageClass::member_ || sp->sb->storage_class == StorageClass::mutable_) &&
                    sp->tp->type != BasicType::aggregate_)
                {
                    if (sp->sb->init && (spMatch == nullptr || sp == spMatch))
                    {
                        if (sp->tp->IsStructured())
                        {
                            genConstructorCall(b, sp->tp->BaseType()->sp, cons->sb->memberInitializers, sp, sp->sb->offset, true,
                                               thisptr, otherptr, cons, false, doCopy, !cons->sb->defaulted);
                        }
                        else
                        {
                            genConsData(b, sym, cons->sb->memberInitializers, sp, sp->sb->offset, thisptr, otherptr, cons, doCopy);
                        }
                    }
                }
            }
            StatementGenerator::FreeLocalContext(emptyBlockdata, cons, codeLabel++);
        }
        else
        {
            if (sym->sb->vbaseEntries)
            {
                SYMBOL* sp = makeID(StorageClass::parameter_, &stdint, nullptr, AnonymousName());
                EXPRESSION* val = MakeExpression(ExpressionNode::auto_, sp);
                int lbl = codeLabel++;
                Statement* st;
                sp->sb->constop = true;
                sp->sb->decoratedName = sp->name;
                sp->sb->offset = Optimizer::chosenAssembler->arch->retblocksize + cons->sb->paramsize;
                localNameSpace->front()->syms->Add(sp);

                Dereference(&stdint, &val);
                st = Statement::MakeStatement(nullptr, b, StatementNode::notselect_);
                optimize_for_constants(&val);
                st->select = val;
                st->label = lbl;
                virtualBaseThunks(b, sym, thisptr);
                doVirtualBases(b, sym, cons->sb->memberInitializers, sym->sb->vbaseEntries, thisptr, otherptr, cons, doCopy);
                if (hasVTab(sym))
                    dovtabThunks(b, sym, thisptr, true);
                st = Statement::MakeStatement(nullptr, b, StatementNode::label_);
                st->label = lbl;
            }
            StatementGenerator::AllocateLocalContext(emptyBlockdata, cons, codeLabel++);
            if (sym->sb->baseClasses)
                for (auto bc : *sym->sb->baseClasses)
                    if (!bc->isvirtual)
                        genConstructorCall(b, sym, cons->sb->memberInitializers, bc->cls, bc->offset, false, thisptr, otherptr,
                                           cons, true, doCopy || !cons->sb->memberInitializers, !cons->sb->defaulted);
            if (hasVTab(sym))
                dovtabThunks(b, sym, thisptr, false);
            for (auto sp : *sym->tp->syms)
            {
                if ((sp->sb->storage_class == StorageClass::member_ || sp->sb->storage_class == StorageClass::mutable_) &&
                    sp->tp->type != BasicType::aggregate_ && !sp->sb->wasUsing)
                {
                    if (sp->tp->IsStructured())
                    {
                        sp->tp->InitializeDeferred();
                        genConstructorCall(b, sp->tp->BaseType()->sp, cons->sb->memberInitializers, sp, sp->sb->offset, true,
                                           thisptr, otherptr, cons, false, doCopy, !cons->sb->defaulted);
                    }
                    else
                    {
                        genConsData(b, sym, cons->sb->memberInitializers, sp, sp->sb->offset, thisptr, otherptr, cons, doCopy);
                    }
                }
            }
            StatementGenerator::FreeLocalContext(emptyBlockdata, cons, codeLabel++);
        }
    }
    if (parseInitializers)
        releaseInitializers(sym, cons);
    cons->sb->labelCount = codeLabel - INT_MIN;
    if (defaulted)
        codeLabel = oldCodeLabel;
    return thisptr;
}
static bool DefaultConstructorConstExpression(SYMBOL* sp)
{
    if (sp->sb->constexpression)
        return true;
    if (sp->sb->vbaseEntries)
        return false;
    if (sp->tp->type != BasicType::union_)
    {
        for (auto sp1 : *sp->tp->BaseType()->syms)
        {
            Type* m;
            if (sp1->sb->storage_class == StorageClass::mutable_)
                return false;
            if (sp1->sb->storage_class == StorageClass::member_)
            {
                if (!is_literal_type(sp1->tp))
                    return false;
                if (!sp1->sb->memberInitializers)
                    return false;
            }
        }
    }

    if (sp->sb->baseClasses)
        for (auto base : *sp->sb->baseClasses)
        {
            base->cls->tp->InitializeDeferred();
            if (base->cls->tp->size && !DefaultConstructorConstExpression(base->cls))
                return false;
        }
    sp->sb->constexpression = true;
    return true;
}

void createConstructor(SYMBOL* sp, SYMBOL* consfunc)
{
    SymbolTable<SYMBOL>* syms;
    FunctionBlock bd = {};
    std::list<FunctionBlock*> b{&bd};
    Statement* st;
    EXPRESSION* thisptr;
    bool oldNoExcept = noExcept;
    noExcept = true;
    bd.type = Keyword::begin_;
    syms = localNameSpace->front()->syms;
    localNameSpace->front()->syms = consfunc->tp->BaseType()->syms;
    thisptr = thunkConstructorHead(b, sp, consfunc, consfunc->tp->BaseType()->syms, false, true, true);
    st = Statement::MakeStatement(nullptr, b, StatementNode::return_);
    st->select = thisptr;
    if (!inNoExceptHandler)
    {
        auto stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_);
        consfunc->sb->inlineFunc.stmt = stmtListFactory.CreateList();
        stmt->lower = bd.statements;
        consfunc->sb->inlineFunc.stmt->push_back(stmt);
        consfunc->sb->inlineFunc.syms = consfunc->tp->BaseType()->syms;
        consfunc->sb->retcount = 1;
        consfunc->sb->attribs.inheritable.isInline = true;
        consfunc->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
        //    consfunc->sb->inlineFunc.stmt->blockTail = b.tail;
        defaultRecursionMap.clear();
        if (noExcept)
        {
            consfunc->sb->xcMode = xc_none;
            consfunc->sb->noExcept = true;
        }
        else
        {
            consfunc->sb->xcMode = xc_unspecified;
            consfunc->sb->noExcept = false;
        }
    }
    else
    {
        defaultRecursionMap.clear();
        if (noExcept)
        {
            consfunc->sb->noExcept = true;
        }
        else
        {
            consfunc->sb->noExcept = false;
        }
    }
    consfunc->sb->constexpression =
        DefaultConstructorConstExpression(sp) || matchesCopy(consfunc, false) || matchesCopy(consfunc, true);
    sp->sb->literalClass |= consfunc->sb->constexpression;
    localNameSpace->front()->syms = syms;
    noExcept &= oldNoExcept;
}
void asnVirtualBases(std::list<FunctionBlock*>& b, SYMBOL* sp, std::list<VBASEENTRY*>* vbe, EXPRESSION* thisptr, EXPRESSION* other,
                     bool move, bool isconst)
{
    if (vbe && vbe->size())
    {
        for (auto it = vbe->end(); it != vbe->begin();)
        {
            --it;
            if ((*it)->alloc)
                genAsnCall(b, sp, (*it)->cls, (*it)->structOffset, thisptr, other, move, isconst);
        }
    }
}
static void genAsnData(std::list<FunctionBlock*>& b, SYMBOL* cls, SYMBOL* member, int offset, EXPRESSION* thisptr,
                       EXPRESSION* other)
{
    EXPRESSION* left = MakeExpression(ExpressionNode::structadd_, thisptr, MakeIntExpression(ExpressionNode::c_i_, offset));
    EXPRESSION* right = MakeExpression(ExpressionNode::structadd_, other, MakeIntExpression(ExpressionNode::c_i_, offset));
    left->right->keepZero = true;
    right->right->keepZero = true;
    Statement* st;
    (void)cls;
    if (member->tp->IsStructured() || member->tp->IsArray())
    {
        left = MakeExpression(ExpressionNode::blockassign_, left, right);
        left->size = member->tp;
        left->altdata = (void*)member->tp;
    }
    else
    {
        Dereference(member->tp, &left);
        Dereference(member->tp, &right);
        left = MakeExpression(ExpressionNode::assign_, left, right);
    }
    st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
    optimize_for_constants(&left);
    st->select = left;
}
static void genAsnCall(std::list<FunctionBlock*>& b, SYMBOL* cls, SYMBOL* base, int offset, EXPRESSION* thisptr, EXPRESSION* other,
                       bool move, bool isconst)
{
    (void)cls;
    EXPRESSION* exp = nullptr;
    Statement* st;
    CallSite* params = Allocate<CallSite>();
    Type* tp = base->tp->CopyType();
    SYMBOL* asn1;
    SYMBOL* cons = search(base->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
    EXPRESSION* left = MakeExpression(ExpressionNode::add_, thisptr, MakeIntExpression(ExpressionNode::c_i_, offset));
    EXPRESSION* right = MakeExpression(ExpressionNode::add_, other, MakeIntExpression(ExpressionNode::c_i_, offset));
    if (move)
    {
        right = MakeExpression(ExpressionNode::not__lvalue_, right);
    }
    if (isconst)
    {
        tp = Type::MakeType(BasicType::const_, tp);
    }
    if (move)
    {
        tp->rref = true;
        tp->lref = false;
    }
    else
    {
        tp->lref = true;
        tp->rref = false;
    }
    params->arguments = argumentListFactory.CreateList();
    auto arg = Allocate<Argument>();
    arg->tp = tp;
    arg->exp = right;
    params->arguments->push_back(arg);
    params->thisptr = left;
    params->thistp = Type::MakeType(BasicType::pointer_, base->tp);
    params->ascall = true;
    asn1 = GetOverloadedFunction(&tp, &params->fcall, cons, params, nullptr, true, false, 0);

    if (asn1)
    {
        SYMBOL* parm = nullptr;
        auto it = asn1->tp->BaseType()->syms->begin();
        AdjustParams(asn1, it, asn1->tp->BaseType()->syms->end(), &params->arguments, false, true);
        ++it;
        if (it != asn1->tp->BaseType()->syms->end())
            parm = *it;
        if (parm && parm->tp->IsRef())
        {
            params->arguments->front()->tp = Type::MakeType(BasicType::lref_, params->arguments->front()->tp);
        }
        if (!isAccessible(base, base, asn1, nullptr, AccessLevel::protected_, false))
        {
            errorsym(ERR_CANNOT_ACCESS, asn1);
        }
        if (asn1->sb->defaulted && !asn1->sb->inlineFunc.stmt)
            createAssignment(base, asn1);
        noExcept &= asn1->sb->noExcept;

        params->functp = asn1->tp;
        params->sp = asn1;
        params->ascall = true;
        exp = MakeExpression(params);
    }
    st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
    optimize_for_constants(&exp);
    st->select = exp;
}
static EXPRESSION* thunkAssignments(std::list<FunctionBlock*>& b, SYMBOL* sym, SYMBOL* asnfunc, SymbolTable<SYMBOL>* syms,
                                    bool move, bool isconst)
{
    auto it = syms->begin();
    EXPRESSION* thisptr = MakeExpression(ExpressionNode::auto_, *it);
    EXPRESSION* other = nullptr;
    BASECLASS* base;
    int oldCodeLabel = codeLabel;
    codeLabel = INT_MIN;
    ++it;
    if (it != syms->end())  // this had better be true
        other = MakeExpression(ExpressionNode::auto_, *it);
    Dereference(&stdpointer, &thisptr);
    Dereference(&stdpointer, &other);
    if (sym->tp->type == BasicType::union_)
    {
        genAsnData(b, sym, sym, 0, thisptr, other);
    }
    else
    {
        if (sym->sb->vbaseEntries)
        {
            asnVirtualBases(b, sym, sym->sb->vbaseEntries, thisptr, other, move, isconst);
        }
        if (sym->sb->baseClasses)
            for (auto base : *sym->sb->baseClasses)
                if (!base->isvirtual)
                {
                    genAsnCall(b, sym, base->cls, base->offset, thisptr, other, move, isconst);
                }
        for (auto sp : *sym->tp->syms)
        {
            if ((sp->sb->storage_class == StorageClass::member_ || sp->sb->storage_class == StorageClass::mutable_) &&
                sp->tp->type != BasicType::aggregate_)
            {
                if (sp->tp->IsStructured())
                {
                    genAsnCall(b, sym, sp->tp->BaseType()->sp, sp->sb->offset, thisptr, other, move, isconst);
                }
                else
                {
                    genAsnData(b, sym, sp, sp->sb->offset, thisptr, other);
                }
            }
        }
    }
    asnfunc->sb->labelCount = codeLabel - INT_MIN;
    codeLabel = oldCodeLabel;
    return thisptr;
}
void createAssignment(SYMBOL* sym, SYMBOL* asnfunc)
{
    // if we get here we are just assuming it is a builtin assignment operator
    // because we only get here for 'default' functions and that is the only one
    // that can be defaulted...
    SymbolTable<SYMBOL>* syms;
    bool oldNoExcept = noExcept;
    noExcept = true;
    FunctionBlock bd = {};
    std::list<FunctionBlock*> b = {&bd};
    auto it = asnfunc->tp->BaseType()->syms->begin();
    ++it;
    bool move = (*it)->tp->BaseType()->type == BasicType::rref_;
    bool isConst = (*it)->tp->IsConst();
    bd.type = Keyword::begin_;
    syms = localNameSpace->front()->syms;
    localNameSpace->front()->syms = asnfunc->tp->BaseType()->syms;
    auto thisptr = thunkAssignments(b, sym, asnfunc, asnfunc->tp->BaseType()->syms, move, isConst);
    auto st = Statement::MakeStatement(nullptr, b, StatementNode::return_);
    st->select = thisptr;
    if (!inNoExceptHandler)
    {
        asnfunc->sb->inlineFunc.stmt = stmtListFactory.CreateList();
        auto stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_);
        asnfunc->sb->inlineFunc.stmt->push_back(stmt);
        stmt->lower = bd.statements;
        asnfunc->sb->inlineFunc.syms = asnfunc->tp->BaseType()->syms;
        asnfunc->sb->attribs.inheritable.isInline = true;
        //    asnfunc->sb->inlineFunc.stmt->blockTail = b.tail;

        defaultRecursionMap.clear();
        if (noExcept)
        {
            asnfunc->sb->xcMode = xc_none;
            asnfunc->sb->noExcept = true;
        }
        else
        {
            asnfunc->sb->xcMode = xc_unspecified;
            asnfunc->sb->noExcept = false;
        }
    }
    else
    {
        defaultRecursionMap.clear();
        if (noExcept)
        {
            asnfunc->sb->noExcept = true;
        }
        else
        {
            asnfunc->sb->noExcept = false;
        }
    }
    localNameSpace->front()->syms = syms;
    noExcept &= oldNoExcept;
}
static void genDestructorCall(std::list<FunctionBlock*>& b, SYMBOL* sp, SYMBOL* against, EXPRESSION* base, EXPRESSION* arrayElms,
                              int offset, bool top)
{
    SYMBOL* dest;
    EXPRESSION* exp;
    Statement* st;
    sp->tp->InstantiateDeferred();
    sp = sp->tp->sp;
    dest = search(sp->tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
    if (!dest)  // error handling
        return;
    exp = base;
    Dereference(&stdpointer, &exp);
    exp = MakeExpression(ExpressionNode::add_, exp, MakeIntExpression(ExpressionNode::c_i_, offset));
    dest = (SYMBOL*)dest->tp->BaseType()->syms->front();
    if (dest->sb->defaulted && !dest->sb->inlineFunc.stmt)
    {
        createDestructor(sp);
    }
    CallDestructor(sp, against, &exp, arrayElms, top, true, false, true);
    st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
    optimize_for_constants(&exp);
    st->select = exp;
}
static void undoVars(std::list<FunctionBlock*>& b, SymbolTable<SYMBOL>* vars, EXPRESSION* base)
{
    if (vars)
    {
        std::stack<SYMBOL*> stk;
        for (auto p : *vars)
        {
            stk.push(p);
        }
        while (stk.size())
        {
            SYMBOL* s = (SYMBOL*)stk.top();
            stk.pop();
            if ((s->sb->storage_class == StorageClass::member_ || s->sb->storage_class == StorageClass::mutable_) &&
                !s->sb->wasUsing)
            {
                if (s->tp->IsStructured())
                {
                    genDestructorCall(b, (SYMBOL*)s->tp->BaseType()->sp, nullptr, base, nullptr, s->sb->offset, true);
                }
                else if (s->tp->IsArray())
                {
                    Type* tp = s->tp;
                    while (tp->IsArray())
                        tp = tp->BaseType()->btp;
                    tp = tp->BaseType();
                    if (tp->IsStructured())
                        genDestructorCall(b, tp->sp, nullptr, base, MakeIntExpression(ExpressionNode::c_i_, s->tp->size / tp->size),
                                          s->sb->offset, true);
                }
            }
        }
    }
}
static void undoBases(std::list<FunctionBlock*>& b, SYMBOL* against, std::list<BASECLASS*>* bc, EXPRESSION* base)
{
    if (bc && bc->size())
    {
        for (auto it = bc->end(); it != bc->begin();)
        {
            --it;
            if (!(*it)->isvirtual)
            {
                genDestructorCall(b, (*it)->cls, against, base, nullptr, (*it)->offset, false);
            }
        }
    }
}
void thunkDestructorTail(std::list<FunctionBlock*>& b, SYMBOL* sp, SYMBOL* dest, SymbolTable<SYMBOL>* syms, bool defaulted)
{
    if (!sp)  // error....
        return;
    InsertInline(sp);
    if (sp->tp->type != BasicType::union_)
    {
        EXPRESSION* thisptr;
        int oldCodeLabel = codeLabel;
        if (definingTemplate)
            return;
        if (defaulted)
            codeLabel = INT_MIN;
        thisptr = MakeExpression(ExpressionNode::auto_, syms->front());
        undoVars(b, sp->tp->BaseType()->syms, thisptr);
        undoBases(b, sp, sp->sb->baseClasses, thisptr);
        if (sp->sb->vbaseEntries)
        {
            auto it = syms->begin();
            ++it;
            auto sp1 = *it;
            EXPRESSION* val = MakeExpression(ExpressionNode::auto_, sp1);
            int lbl = codeLabel++;
            Statement* st;
            sp1->sb->decoratedName = sp1->name;
            sp1->sb->offset = Optimizer::chosenAssembler->arch->retblocksize + getSize(BasicType::pointer_);
            Optimizer::SymbolManager::Get(sp1)->offset = sp1->sb->offset;
            Dereference(&stdint, &val);
            st = Statement::MakeStatement(nullptr, b, StatementNode::notselect_);
            optimize_for_constants(&val);
            st->select = val;
            st->label = lbl;
            for (auto vbe : *sp->sb->vbaseEntries)
            {
                if (vbe->alloc)
                    genDestructorCall(b, vbe->cls, sp, thisptr, nullptr, vbe->structOffset, false);
            }
            st = Statement::MakeStatement(nullptr, b, StatementNode::label_);
            st->label = lbl;
        }
        dest->sb->labelCount = codeLabel - INT_MIN;
        if (defaulted)
            codeLabel = oldCodeLabel;
    }
}
void createDestructor(SYMBOL* sp)
{
    SymbolTable<SYMBOL>* syms;
    SYMBOL* dest = search(sp->tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
    bool oldNoExcept = noExcept;
    noExcept = true;
    FunctionBlock bd = {};
    std::list<FunctionBlock*> b = {&bd};
    bd.type = Keyword::begin_;
    dest = (SYMBOL*)dest->tp->BaseType()->syms->front();
    syms = localNameSpace->front()->syms;
    localNameSpace->front()->syms = dest->tp->BaseType()->syms;
    thunkDestructorTail(b, sp, dest, dest->tp->BaseType()->syms, true);
    if (!inNoExceptHandler)
    {
        dest->sb->inlineFunc.stmt = stmtListFactory.CreateList();
        auto stmt = Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_);
        dest->sb->inlineFunc.stmt->push_back(stmt);
        stmt->lower = bd.statements;
        dest->sb->inlineFunc.syms = dest->tp->BaseType()->syms;
        dest->sb->retcount = 1;
        dest->sb->attribs.inheritable.isInline = dest->sb->attribs.inheritable.linkage2 != Linkage::export_;
        dest->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
    }
    if (noExcept)
    {
        dest->sb->xcMode = xc_none;
        dest->sb->noExcept = true;
        noExcept = true;
    }
    else
    {
        dest->sb->xcMode = xc_unspecified;
        dest->sb->noExcept = false;
    }
    //    dest->sb->inlineFunc.stmt->blockTail = b.tail;
    localNameSpace->front()->syms = syms;
    noExcept &= oldNoExcept;
}
void makeArrayConsDest(Type** tp, EXPRESSION** exp, SYMBOL* cons, SYMBOL* dest, EXPRESSION* count)
{
    EXPRESSION* size = MakeIntExpression(ExpressionNode::c_i_, (*tp)->size);
    EXPRESSION *econs = (cons ? MakeExpression(ExpressionNode::pc_, cons) : nullptr),
               *edest = MakeExpression(ExpressionNode::pc_, dest);
    CallSite* params = Allocate<CallSite>();
    SYMBOL* asn1;
    Argument arg0_value = {};
    Argument* arg0 = &arg0_value;           // this
    Argument* arg1 = Allocate<Argument>();  // cons
    Argument* arg2 = Allocate<Argument>();  // dest
    Argument* arg3 = Allocate<Argument>();  // size
    Argument* arg4 = Allocate<Argument>();  // count
    SYMBOL* ovl = namespacesearch("__arrCall", globalNameSpace, false, false);
    params->arguments = argumentListFactory.CreateList();
    params->arguments->push_back(arg0);
    params->arguments->push_back(arg1);
    params->arguments->push_back(arg2);
    params->arguments->push_back(arg3);
    params->arguments->push_back(arg4);

    if (econs)
        thunkForImportTable(&econs);
    thunkForImportTable(&edest);
    arg0->exp = *exp;
    arg0->tp = &stdpointer;
    arg1->exp = econs ? econs : MakeIntExpression(ExpressionNode::c_i_, 0);
    arg1->tp = &stdpointer;
    arg2->exp = edest;
    arg2->tp = &stdpointer;
    arg3->exp = count;
    arg3->tp = &stdint;
    arg4->exp = size;
    arg4->tp = &stdint;

    params->ascall = true;
    asn1 = GetOverloadedFunction(tp, &params->fcall, ovl, params, nullptr, true, false, 0);
    if (!asn1)
    {
        diag("makeArrayConsDest: Can't call array iterator");
    }
    else
    {
        // treating this as a member function even though it isn't, because,
        // it really is a constructor and the places other parts of the code find it in
        // are just assuming there will be a constructor...
        params->thistp = arg0->tp;
        params->thisptr = arg0->exp;
        params->arguments->pop_front();
        params->functp = asn1->tp;
        params->sp = asn1;
        params->ascall = true;
        *exp = MakeExpression(params);
    }
}
bool CallDestructor(SYMBOL* sp, SYMBOL* against, EXPRESSION** exp, EXPRESSION* arrayElms, bool top, bool pointer, bool skipAccess,
                    bool novtab)
{
    if (!sp)
        return false;
    SYMBOL* dest;
    SYMBOL* dest1;
    Type *tp = nullptr, *stp;
    CallSite* params = Allocate<CallSite>();
    SYMBOL* sym;
    if (!against)
        against = theCurrentFunc ? theCurrentFunc->sb->parentClass : sp;
    sp->tp->InstantiateDeferred();
    sp->tp->InitializeDeferred();
    stp = sp->tp;
    dest = search(sp->tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
    // if it isn't already defined get out, there will be an error from somewhere else..
    if (!sp->tp->BaseType()->syms || !dest)
        return false;
    sym = sp->tp->BaseType()->sp;
    if (!*exp)
    {
        *exp = MakeIntExpression(ExpressionNode::c_i_, 0);
        diag("CallDestructor: no this pointer");
    }
    params->thisptr = *exp;
    params->thistp = Type::MakeType(BasicType::pointer_, sp->tp);
    params->ascall = true;
    dest1 = dest->tp->BaseType()->syms->front();
    if (!dest1 || !dest1->sb->defaulted || dest1->sb->storage_class == StorageClass::virtual_)
    {
        dest1 =
            GetOverloadedFunction(&tp, &params->fcall, dest, params, nullptr, true, false, inNothrowHandler ? _F_IS_NOTHROW : 0);
        if (!novtab && dest1 && dest1->sb->storage_class == StorageClass::virtual_)
        {
            auto exp_in = params->thisptr;
            Dereference(&stdpointer, &exp_in);
            exp_in = MakeExpression(ExpressionNode::add_, exp_in, MakeIntExpression(ExpressionNode::c_i_, dest1->sb->vtaboffset));
            Dereference(&stdpointer, &exp_in);
            params->fcall = exp_in;
        }
        if (dest1)
            noExcept &= dest1->sb->noExcept;
    }
    else
        params->fcall = MakeExpression(ExpressionNode::pc_, dest1);
    if (dest1)
    {
        CheckCalledException(dest1, params->thisptr);
        if (params->thisptr->type == ExpressionNode::auto_ && !dest1->sb->defaulted)
            params->thisptr->v.sp->sb->addressTaken = true;

        if (!skipAccess && dest1 &&
            !isAccessible(
                against, sp, dest1, theCurrentFunc,
                top ? (theCurrentFunc && theCurrentFunc->sb->parentClass == sp ? AccessLevel::protected_ : AccessLevel::public_)
                    : AccessLevel::protected_,
                false))
        {
            errorsym(ERR_CANNOT_ACCESS, dest1);
        }
        if (dest1 && dest1->sb->defaulted && !dest1->sb->inlineFunc.stmt)
            createDestructor(sp);
        params->functp = dest1->tp;
        params->sp = dest1;
        params->ascall = true;
        if (arrayElms)
        {
            makeArrayConsDest(&stp, exp, nullptr, dest1, arrayElms);
        }
        else
        {
            if (sp->sb->vbaseEntries)
            {
                Argument *x = Allocate<Argument>(), **p;
                x->tp = Type::MakeType(BasicType::int_);
                x->exp = MakeIntExpression(ExpressionNode::c_i_, top);
                if (!params->arguments)
                    params->arguments = argumentListFactory.CreateList();
                params->arguments->push_back(x);
                params->sp->sb->noinline = true;
            }
            *exp = MakeExpression(params);
        }
        if (*exp && !pointer)
        {
            *exp = MakeExpression(ExpressionNode::thisref_, *exp);
            (*exp)->dest = true;
            (*exp)->v.t.thisptr = params->thisptr;
            (*exp)->v.t.tp = sp->tp;
            sp->sb->hasDest = true;
            if (!sp->sb->pureDest)
                hasXCInfo = true;
        }
    }
    return true;
}
bool CallConstructor(Type** tp, EXPRESSION** exp, CallSite* params, bool checkcopy, EXPRESSION* arrayElms, bool top,
                     bool maybeConversion, bool implicit, bool pointer, bool usesInitList, bool isAssign, bool toErr)
{
    (void)checkcopy;
    Type* stp = *tp;
    SYMBOL* sp;
    SYMBOL *against, *against2;
    SYMBOL* cons;
    SYMBOL* cons1;
    EXPRESSION* e1 = nullptr;
    Type* initializerListTemplate = nullptr;
    Type* initializerListType = nullptr;
    bool initializerRef = false;
    stp->InstantiateDeferred();
    sp = (*tp)->BaseType()->sp;

    against = theCurrentFunc ? theCurrentFunc->sb->parentClass : top ? sp : sp->sb->parentClass;
    against2 = theCurrentFunc && theCurrentFunc->sb->parentClass ? theCurrentFunc->sb->parentClass : nullptr;
    if (isAssign)
    {
        cons = search(sp->tp->BaseType()->syms, overloadNameTab[(int)Keyword::assign_ - (int)Keyword::new_ + CI_NEW]);
    }
    else
    {
        cons = search(sp->tp->BaseType()->syms, overloadNameTab[CI_CONSTRUCTOR]);
    }

    if (!params)
    {
        params = Allocate<CallSite>();
    }
    else
    {
        if (params->arguments)
        {
            for (auto list : *params->arguments)
            {
                if (!list->nested && list->tp->IsStructured())
                {
                    SYMBOL* sp1 = list->tp->BaseType()->sp;
                    if (!definingTemplate && sp1->sb->templateLevel && sp1->templateParams && !sp1->sb->instantiated)
                    {
                        if (!allTemplateArgsSpecified(sp1, sp1->templateParams))
                            sp1 = GetClassTemplate(sp1, sp1->templateParams, false);
                        if (sp1)
                            list->tp = TemplateClassInstantiate(sp1, sp1->templateParams, false, StorageClass::global_)->tp;
                    }
                }
            }
        }
    }
    params->thisptr = *exp;
    params->thistp = Type::MakeType(BasicType::pointer_, sp->tp);
    params->ascall = true;

    cons1 = GetOverloadedFunction(tp, &params->fcall, cons, params, nullptr, toErr, maybeConversion,
                                  (usesInitList ? _F_INITLIST : 0) | _F_INCONSTRUCTOR | (inNothrowHandler ? _F_IS_NOTHROW : 0));

    if (cons1 && cons1->tp->IsFunction())
    {
        CheckCalledException(cons1, params->thisptr);
        if (cons1->sb->castoperator)
        {
            CallSite* oparams = Allocate<CallSite>();
            if (!inNoExceptHandler &&
                !isAccessible(cons1->sb->parentClass, cons1->sb->parentClass, cons1, nullptr, AccessLevel::public_, false))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            if (cons1->sb->isExplicit && implicit)
                error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
            oparams->fcall = params->fcall;
            oparams->thisptr = params->arguments->front()->exp;
            oparams->thistp = Type::MakeType(BasicType::pointer_, cons1->sb->parentClass->tp);
            oparams->functp = cons1->tp;
            oparams->sp = cons1;
            oparams->ascall = true;
            if (!cons1->tp->BaseType()->btp->IsRef())
            {
                optimize_for_constants(exp);
                oparams->returnEXP = *exp;
                oparams->returnSP = sp;
            }
            e1 = MakeExpression(oparams);
            params = oparams;
        }
        else
        {
            if (!inNoExceptHandler && cons1->sb->access != AccessLevel::public_ &&
                !isAccessible(
                    against, against2, cons1, theCurrentFunc,
                    top ? (theCurrentFunc && theCurrentFunc->sb->parentClass == sp ? AccessLevel::private_ : AccessLevel::public_)
                        : AccessLevel::private_,
                    false))
            {
                errorsym(ERR_CANNOT_ACCESS, cons1);
            }
            if (cons1->sb->isExplicit && implicit)
                error(ERR_IMPLICIT_USE_OF_EXPLICIT_CONVERSION);
            {
                auto it = cons1->tp->BaseType()->syms->begin();
                if ((*it)->sb->thisPtr)
                    ++it;
                Type* tp = (*it)->tp;
                if (tp->IsRef())
                {
                    initializerRef = true;
                    tp = tp->BaseType()->btp;
                }
                if (tp->IsStructured())
                {
                    SYMBOL* sym = (tp->BaseType()->sp);
                    if (sym->sb->initializer_list && sym->sb->templateLevel)
                    {
                        auto it = sym->templateParams->begin();
                        ++it;
                        initializerListTemplate = sym->tp;
                        initializerListType = it->second->byClass.val;
                    }
                }
            }
            if (initializerListType && (!params->arguments->front()->tp || !params->arguments->front()->tp->IsStructured() ||
                                        !params->arguments->front()->tp->BaseType()->sp->sb->initializer_list))
            {
                std::list<Argument*>* old = params->arguments;
                std::list<Argument*> temp;
                std::list<Argument*>* temp2 = &temp;

                if (params->arguments && params->arguments->size())
                {
                    if (params->arguments->front()->nested && params->arguments->front()->nested->size() &&
                        params->arguments->front()->nested->front()->nested && !params->arguments->front()->initializer_list)
                        temp.push_back(params->arguments->front());
                    else
                        temp = *params->arguments;
                }
                if (!params->arguments->front()->initializer_list)
                {
                    temp = *params->arguments->front()->nested;
                }
                CreateInitializerList(cons1, initializerListTemplate, initializerListType, &temp2, false, initializerRef);
                params->arguments = temp2;
                if (old->size() &&
                    (!old->front()->initializer_list ||
                     (old->front()->nested && old->front()->nested->front()->nested && !old->front()->initializer_list)))
                {
                    auto it1 = old->begin();
                    if (it1 != old->end())
                    {
                        ++it1;
                        params->arguments->insert(params->arguments->end(), it1, old->end());
                    }
                }
                auto it = cons1->tp->BaseType()->syms->begin();
                ++it;
                ++it;
                if (it != cons1->tp->BaseType()->syms->end())
                {
                    auto x = params->arguments->front();
                    params->arguments->pop_front();
                    AdjustParams(cons1, it, cons1->tp->BaseType()->syms->end(), &params->arguments, false,
                                 implicit && !cons1->sb->isExplicit);
                    params->arguments->push_front(x);
                }
            }
            else
            {
                std::list<Argument*> temp;
                std::list<Argument*>* temp2 = &temp;
                if (params->arguments && params->arguments->size() && params->arguments->front()->nested &&
                    !params->arguments->front()->initializer_list)
                {
                    temp = *params->arguments->front()->nested;
                    *params->arguments = std::move(temp);
                }
                AdjustParams(cons1, cons1->tp->BaseType()->syms->begin(), cons1->tp->BaseType()->syms->end(), &params->arguments,
                             false, implicit && !cons1->sb->isExplicit);
            }
            params->functp = cons1->tp;
            params->sp = cons1;
            params->ascall = true;
            if (cons1->sb->defaulted && !cons1->sb->inlineFunc.stmt)
            {
                // if this class has a constructor from a base class,
                // the next access will take care of creating the base class constructor
                // properly
                createConstructor(cons1->sb->parentClass, cons1);
            }
            noExcept &= cons1->sb->noExcept;
            if (arrayElms)
            {
                SYMBOL* dest = search(sp->tp->BaseType()->syms, overloadNameTab[CI_DESTRUCTOR]);
                SYMBOL* dest1;
                SYMBOL* against = top ? sp : sp->sb->parentClass;
                Type* tp = nullptr;
                CallSite* params = Allocate<CallSite>();
                if (!*exp)
                {
                    diag("CallDestructor: no this pointer");
                }
                params->thisptr = *exp;
                params->thistp = Type::MakeType(BasicType::pointer_, sp->tp);
                params->ascall = true;
                dest1 = GetOverloadedFunction(&tp, &params->fcall, dest, params, nullptr, true, false, 0);
                if (dest1 && !isAccessible(against, sp, dest1, nullptr,
                                           top ? (theCurrentFunc && theCurrentFunc->sb->parentClass == sp ? AccessLevel::protected_
                                                                                                          : AccessLevel::public_)
                                               : AccessLevel::protected_,
                                           false))
                {
                    errorsym(ERR_CANNOT_ACCESS, dest1);
                }
                if (dest1 && dest1->sb->defaulted && !dest1->sb->inlineFunc.stmt)
                    createDestructor(sp);
                makeArrayConsDest(&stp, exp, cons1, dest1, arrayElms);
                e1 = *exp;
            }
            else
            {
                if (sp->sb->vbaseEntries)
                {
                    Argument *x = Allocate<Argument>(), **p;
                    x->tp = Type::MakeType(BasicType::int_);
                    x->exp = MakeIntExpression(ExpressionNode::c_i_, top);
                    if (!params->arguments)
                        params->arguments = argumentListFactory.CreateList();
                    params->arguments->push_back(x);
                    params->sp->sb->noinline = true;
                }
                e1 = MakeExpression(params);
            }
        }
        if (params->sp->sb->constexpression && (argumentNesting == 0 && !inStaticAssert))
        {
            EXPRESSION* node = MakeExpression(params);
            if (EvaluateConstexprFunction(node))
            {
                e1 = node;
            }
        }
        *exp = e1;
        if ((Optimizer::architecture == ARCHITECTURE_MSIL) && *exp)
        {
            // this needs work, won't work for structures as members of other structures...
            EXPRESSION* exp1 = params->thisptr;
            if ((exp1->type == ExpressionNode::add_ || exp1->type == ExpressionNode::structadd_) &&
                isconstzero(&stdint, exp1->right))
                exp1 = exp1->left;
            *exp = MakeExpression(ExpressionNode::assign_, exp1, *exp);
            params->thisptr = nullptr;
        }
        else if (*exp && !pointer)
        {
            *exp = MakeExpression(ExpressionNode::thisref_, *exp);
            (*exp)->v.t.thisptr = params->thisptr;
            (*exp)->v.t.tp = sp->tp;
            optimize_for_constants(exp);  // for constexpr constructors
            // hasXCInfo = true;
        }

        return true;
    }
    return false;
}
bool callConstructorParam(Type** tp, EXPRESSION** exp, Type* paramTP, EXPRESSION* paramExp, bool top, bool maybeConversion,
                          bool implicit, bool pointer, bool toErr)
{
    CallSite* params = Allocate<CallSite>();
    if (paramTP && paramExp)
    {
        params->arguments = argumentListFactory.CreateList();
        params->arguments->push_back(Allocate<Argument>());
        params->arguments->front()->tp = paramTP;
        params->arguments->front()->exp = paramExp;
    }
    return CallConstructor(tp, exp, params, false, nullptr, top, maybeConversion, implicit, pointer, false, false, toErr);
}

}  // namespace Parser