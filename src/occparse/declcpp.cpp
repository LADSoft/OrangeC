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

namespace CompletionCompiler
{
void ccInsertUsing(Parser::SYMBOL* ns, Parser::SYMBOL* parentns, const char* file, int line);
}
namespace Parser
{

attributes basisAttribs;

std::list<SYMBOL*> nameSpaceList;
char anonymousNameSpaceName[512];
int noNeedToSpecialize;
int parsingUsing;
int inStaticAssert;

static int dumpVTabEntries(int count, THUNK* thunks, SYMBOL* sym, std::list<VTABENTRY*>* entries)
{
    if (IsCompiler())
    {
        if (entries)
        {
            for (auto entry : *entries)
            {
                if (!entry->isdead)
                {
                    Optimizer::genaddress(entry->dataOffset);
                    Optimizer::genaddress(entry->vtabOffset);
                    if (entry->virtuals)
                    {
                        for (auto func : *entry->virtuals)
                        {
                            if (func->sb->deferredCompile && (!func->sb->templateLevel || func->sb->instantiated))
                            {
                                CallSite fcall = {};
                                Type* tp = nullptr;
                                EXPRESSION* exp = MakeIntExpression(ExpressionNode::c_i_, 0);
                                SYMBOL* sp = func->sb->overloadName;
                                fcall.arguments = argumentListFactory.CreateList();
                                for (auto sym : *func->tp->BaseType()->syms)
                                {
                                    if (sym->sb->thisPtr)
                                    {
                                        fcall.thistp = sym->tp;
                                        fcall.thisptr = exp;
                                    }
                                    else if (sym->tp->type != BasicType::void_)
                                    {
                                        auto arg = Allocate<Argument>();
                                        arg->tp = sym->tp;
                                        arg->exp = exp;
                                        fcall.arguments->push_back(arg);
                                    }
                                }
                                fcall.ascall = true;
                                auto oldnoExcept = noExcept;
                                sp = GetOverloadedFunction(&tp, &exp, sp, &fcall, nullptr, true, false, 0);
                                noExcept = oldnoExcept;
                                if (sp)
                                    func = sp;
                            }
                            InitializeFunctionArguments(func);
                            InsertInline(func);
                            if (func->sb->defaulted && !func->sb->inlineFunc.stmt && func->sb->isDestructor)
                                createDestructor(func->sb->parentClass);
                            if (func->sb->ispure)
                            {
                                Optimizer::genaddress(0);
                            }
                            else if (sym == func->sb->parentClass && entry->vtabOffset)
                            {
                                char buf[512];
                                SYMBOL* localsp;
                                Utils::StrCpy(buf, sym->sb->decoratedName);
                                Optimizer::my_sprintf(buf + strlen(buf), sizeof(buf) - strlen(buf), "_@%c%d", count % 26 + 'A', count / 26);

                                thunks[count].entry = entry;
                                if (func->sb->attribs.inheritable.linkage2 == Linkage::import_)
                                {
                                    EXPRESSION* exp = MakeExpression(ExpressionNode::pc_, func);
                                    thunkForImportTable(&exp);
                                    thunks[count].func = exp->v.sp;
                                }
                                else
                                {
                                    thunks[count].func = func;
                                }
                                thunks[count].name = localsp = makeID(StorageClass::static_, &stdfunc, nullptr, litlate(buf));
                                localsp->sb->decoratedName = localsp->name;
                                localsp->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
                                Optimizer::genref(Optimizer::SymbolManager::Get(localsp), 0);
                                count++;
                            }
                            else
                            {
                                if (func->sb->attribs.inheritable.linkage2 == Linkage::import_)
                                {
                                    EXPRESSION* exp = MakeExpression(ExpressionNode::pc_, func);
                                    thunkForImportTable(&exp);
                                    Optimizer::genref(Optimizer::SymbolManager::Get(exp->v.sp), 0);
                                }
                                else
                                {
                                    Optimizer::genref(Optimizer::SymbolManager::Get(func), 0);
                                }
                            }
                        }
                    }
                }
                count = dumpVTabEntries(count, thunks, sym, entry->children);
            }
        }
    }
    else
    {
        count = 0;
    }
    return count;
}
void dumpVTab(SYMBOL* sym)
{
    if (IsCompiler())
    {
        THUNK thunks[1000];
        SYMBOL* xtSym = RTTIDumpType(sym->tp->BaseType());
        int count = 0;

        Optimizer::dseg();
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(sym->sb->vtabsp), Optimizer::vt_data);
        if (xtSym)
            Optimizer::genref(Optimizer::SymbolManager::Get(xtSym), 0);
        else
            Optimizer::genaddress(0);
        count = dumpVTabEntries(count, thunks, sym, sym->sb->vtabEntries);
        Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(sym->sb->vtabsp));

        if (count)
        {
            int i;
            Optimizer::cseg();
            for (i = 0; i < count; i++)
            {
                Optimizer::gen_virtual(Optimizer::SymbolManager::Get(thunks[i].name), Optimizer::vt_code);
                Optimizer::gen_vtt(-(int)thunks[i].entry->dataOffset, Optimizer::SymbolManager::Get(thunks[i].func),
                                   Optimizer::SymbolManager::Get(thunks[i].name));
                Optimizer::gen_endvirtual(Optimizer::SymbolManager::Get(thunks[i].name));
            }
        }
    }
}
void internalClassRefCount(SYMBOL* base, SYMBOL* derived, int* vcount, int* ccount, bool isVirtual)
{
    bool ok = false;
    if (base == derived || (base && derived && SameTemplate(derived->tp, base->tp)))
    {

        if (!definingTemplate || instantiatingTemplate)
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
static bool vfMatch(SYMBOL* sym, SYMBOL* oldFunc, SYMBOL* newFunc)
{
    bool rv = false;
    if (oldFunc->sb->isDestructor && newFunc->sb->isDestructor)
        return true;
    InitializeFunctionArguments(oldFunc);
    InitializeFunctionArguments(newFunc);
    rv = !strcmp(oldFunc->name, newFunc->name) && matchOverload(oldFunc->tp, newFunc->tp);
    if (rv && !oldFunc->sb->isDestructor)
    {
        Type *tp1 = oldFunc->tp->BaseType()->btp, *tp2 = newFunc->tp->BaseType()->btp;
        if (!tp1->CompatibleType(tp2) && !SameTemplate(tp1, tp2) &&
            (!tp1->IsStructured() || !tp2->IsStructured() ||
             (tp1->BaseType()->sp->sb->templateLevel &&
              tp2->BaseType()->sp->sb->parentTemplate != tp1->BaseType()->sp->sb->parentTemplate)))
        {
            if (!definingTemplate)
            {
                if (tp1->BaseType()->type == tp2->BaseType()->type)
                {
                    if (tp1->IsPtr() || tp1->IsRef())
                    {
                        if (tp1->IsConst() == tp2->IsConst() && tp1->IsVolatile() == tp2->IsVolatile())
                        {
                            tp1 = tp1->BaseType()->btp;
                            tp2 = tp2->BaseType()->btp;
                            tp1->InstantiateDeferred();
                            tp1->InitializeDeferred();
                            tp2->InstantiateDeferred();
                            tp2->InitializeDeferred();
                            if (tp1->IsStructured() && tp2->IsStructured())
                            {
                                if ((!tp1->IsConst() || tp2->IsConst()) && (!tp1->IsVolatile() || tp2->IsVolatile()))
                                {
                                    tp1 = tp1->BaseType();
                                    tp2 = tp2->BaseType();
                                    if (tp1->sp != tp2->sp && tp2->sp != sym && classRefCount(tp1->sp, tp2->sp) != 1)
                                    {
                                        bool bad =
                                            tp1->sp->sb->maintemplate && tp1->sp->sb->maintemplate != tp2->sp->sb->maintemplate;
                                        if (!bad)
                                        {
                                            bad = !tp1->CompatibleType(tp2) && !SameTemplate(tp1, tp2);
                                        }
                                        if (bad)
                                        {
                                            errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, oldFunc->sb->parentClass, newFunc->sb->parentClass);
                                        }
                                    }
                                }
                                else
                                    errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
                            }
                            else
                                errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
                        }
                        else
                            errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
                    }
                    else
                        errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
                }
                else
                    errorsym2(ERR_RETURN_TYPES_NOT_COVARIANT, oldFunc, newFunc);
            }
        }
    }
    return rv;
}
static bool backpatchVirtualFunc(SYMBOL* sym, std::list<VTABENTRY*>* entries, SYMBOL* func_in)
{
    bool rv = false;
    if (entries)
    {
        for (auto entry : *entries)
        {
            if (entry->virtuals)
            {
                for (auto&& func : *entry->virtuals)
                {
                    if (vfMatch(sym, func, func_in))
                    {
                        if (func->sb->isfinal)
                            errorsym(ERR_OVERRIDING_FINAL_VIRTUAL_FUNC, func);
                        if (func->sb->deleted)
                            errorsym2(ERR_DELETED_VIRTUAL_FUNC, func->sb->deleted ? func : func_in,
                                      func->sb->deleted ? func_in : func);

                        func = func_in;
                        rv = true;
                    }
                }
            }
            backpatchVirtualFunc(sym, entry->children, func_in);
        }
    }
    return rv;
}
static int allocVTabSpace(std::list<VTABENTRY*>* vtab, int offset)
{
    if (vtab)
    {
        for (auto vt : *vtab)
        {
            if (!vt->isdead)
            {
                vt->vtabOffset = offset;
                if (vt->virtuals)
                    for (auto vf : *vt->virtuals)
                    {
                        offset += getSize(BasicType::pointer_);
                    }
                offset += 2 * getSize(BasicType::pointer_);
            }
            offset = allocVTabSpace(vt->children, offset);
        }
    }
    return offset;
}
static void copyVTabEntries(std::list<VTABENTRY*>* vtab, std::list<VTABENTRY*>** pos, int offset, bool isvirtual)
{
    if (vtab)
    {
        for (auto lst : *vtab)
        {
            VTABENTRY* vt = Allocate<VTABENTRY>();
            vt->cls = lst->cls;
            vt->isvirtual = lst->isvirtual;
            vt->isdead = vt->isvirtual || lst->dataOffset == 0 || isvirtual;
            vt->dataOffset = offset + lst->dataOffset;
            if (!(*pos))
                *pos = vtabEntryListFactory.CreateList();
            (*pos)->push_back(vt);

            if (lst->virtuals)
            {
                vt->virtuals = symListFactory.CreateList();
                for (auto vf : *lst->virtuals)
                    vt->virtuals->push_back(vf);
            }
            if (lst->children)
                copyVTabEntries(lst->children, &vt->children, offset + lst->dataOffset,
                                vt->isvirtual || isvirtual || lst->dataOffset == lst->children->front()->dataOffset);
        }
    }
}
static void checkAmbiguousVirtualFunc(SYMBOL* sym, VTABENTRY** match, std::list<VTABENTRY*>* vtab)
{
    if (vtab)
    {
        for (auto vt : *vtab)
        {
            if (sym == vt->cls)
            {
                if (!*match)
                {
                    (*match) = vt;
                }
                else
                {
                    std::list<SYMBOL*>::iterator itm, itme, itv, itve;
                    if ((*match)->virtuals)
                    {
                        itm = (*match)->virtuals->begin();
                        itme = (*match)->virtuals->end();
                    }
                    if (vt->virtuals)
                    {
                        itv = vt->virtuals->begin();
                        itve = vt->virtuals->end();
                    }
                    for (; itm != itme && itv != itve; ++itm, ++itv)
                    {
                        if (*itm != *itv)
                        {
                            errorsym2(ERR_AMBIGUOUS_VIRTUAL_FUNCTIONS, *itm, *itv);
                        }
                    }
                }
            }
            else
            {
                checkAmbiguousVirtualFunc(sym, match, vt->children);
            }
        }
    }
}
static void checkXT(SYMBOL* sym1, SYMBOL* sym2, bool func)
{

    if (sym1->sb->xcMode == xc_dynamic && sym2->sb->xcMode == xc_dynamic)
    {
        // check to see that everything in sym1 is also in sym2
        Optimizer::LIST* l1 = sym1->sb->xc->xcDynamic;
        while (l1)
        {
            Optimizer::LIST* l2 = sym2->sb->xc->xcDynamic;
            while (l2)
            {
                if (((Type*)l2->data)->CompatibleType((Type*)l1->data) && ((Type*)l2->data)->SameIntegerType((Type*)l1->data))
                    break;
                l2 = l2->next;
            }
            if (!l2)
            {
                currentErrorLine = 0;
                if (func)
                    errorsym(ERR_EXCEPTION_SPECIFIER_BLOCKS_EXCEPTIONS, sym1);
                else
                    errorsym(ERR_EXCEPTION_SPECIFIER_AT_LEAST_AS_RESTRICTIVE, sym1);
                break;
            }
            l1 = l1->next;
        }
    }
    else if (sym1->sb->xcMode != sym2->sb->xcMode)
    {
        if (sym2->sb->xcMode != xc_unspecified && sym2->sb->xcMode != xc_all)
        {
            if ((sym1->sb->xcMode == xc_dynamic && sym1->sb->xc->xcDynamic) || sym1->sb->xcMode == xc_unspecified ||
                sym1->sb->xcMode == xc_all)
            {
                currentErrorLine = 0;
                if (func)
                    errorsym(ERR_EXCEPTION_SPECIFIER_BLOCKS_EXCEPTIONS, sym1);
                else
                    errorsym(ERR_EXCEPTION_SPECIFIER_AT_LEAST_AS_RESTRICTIVE, sym1);
            }
        }
    }
}
static void checkExceptionSpecification(SYMBOL* sp)
{
    for (auto sym : *sp->tp->BaseType()->syms)
    {
        if (sym->sb->storage_class == StorageClass::overloads_)
        {
            for (auto sym1 : *sym->tp->BaseType()->syms)
            {
                if (sym1->sb->storage_class == StorageClass::virtual_)
                {
                    const char* f1 = (char*)strrchr(sym1->sb->decoratedName, '@');
                    if (f1 && sp->sb->baseClasses)
                    {
                        for (auto bc : *sp->sb->baseClasses)
                        {

                            SYMBOL* sym2 = search(bc->cls->tp->BaseType()->syms, sym->name);
                            if (sym2)
                            {
                                for (auto sym3 : *sym2->tp->BaseType()->syms)
                                {
                                    const char* f2 = (char*)strrchr(sym3->sb->decoratedName, '@');
                                    if (f2 && !strcmp(f1, f2))
                                    {
                                        checkXT(sym1, sym3, false);
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
void CheckCalledException(SYMBOL* cst, EXPRESSION* exp)
{
    (void)exp;
    //    if (cst->sb->xcMode != xc_none && (cst->sb->xcMode != xc_dynamic || (cst->sb->xc && cst->sb->xc->xcDynamic)))
    //        functionCanThrow = true;
}
void calculateVTabEntries(SYMBOL* sym, SYMBOL* base, std::list<VTABENTRY*>** pos, int offset)
{
    auto lst = base->sb->baseClasses;
    *pos = vtabEntryListFactory.CreateList();
    if (sym->sb->hasvtab && (!lst || !lst->size() || lst->front()->isvirtual || !lst->front()->cls->sb->vtabEntries))
    {
        VTABENTRY* vt = Allocate<VTABENTRY>();
        vt->cls = sym;
        vt->isvirtual = false;
        vt->isdead = false;
        vt->dataOffset = offset;
        (*pos)->push_back(vt);
    }
    if (base->sb->baseClasses)
    {
        for (auto lst : *base->sb->baseClasses)
        {
            VTABENTRY* vt = Allocate<VTABENTRY>();
            vt->cls = lst->cls;
            vt->isvirtual = lst->isvirtual;
            vt->isdead = vt->isvirtual;
            vt->dataOffset = offset + lst->offset;
            (*pos)->push_back(vt);
            copyVTabEntries(lst->cls->sb->vtabEntries, &vt->children, lst->offset, false);
            if (vt->children && vt->children->front()->virtuals)
            {
                vt->virtuals = symListFactory.CreateList();
                for (auto vt1 : *vt->children->front()->virtuals)
                    vt->virtuals->push_back(vt1);
            }
        }
    }
    if (sym->sb->vtabEntries && sym->sb->vtabEntries->size())
    {
        for (auto cur : *sym->tp->syms)
        {
            std::list<VTABENTRY*> temp;
            if (cur->sb->storage_class == StorageClass::overloads_)
            {
                for (auto cur1 : *cur->tp->syms)
                {
                    VTABENTRY *hold, *hold2;
                    bool found = false;
                    bool isfirst = false;
                    bool isvirt = cur1->sb->storage_class == StorageClass::virtual_;
                    temp.clear();
                    temp.push_back(sym->sb->vtabEntries->front());
                    auto children = sym->sb->vtabEntries->front()->children;
                    sym->sb->vtabEntries->front()->children = nullptr;
                    sym->sb->vtabEntries->pop_front();
                    found = backpatchVirtualFunc(sym, sym->sb->vtabEntries, cur1);
                    found |= backpatchVirtualFunc(sym, children, cur1);
                    isfirst = backpatchVirtualFunc(sym, &temp, cur1);
                    isvirt |= found | isfirst;
                    sym->sb->vtabEntries->push_front(temp.front());
                    sym->sb->vtabEntries->front()->children = children;
                    if (isvirt)
                    {
                        cur1->sb->storage_class = StorageClass::virtual_;
                        if (!isfirst)
                        {
                            if (sym->sb->vtabEntries->front()->virtuals == nullptr)
                                sym->sb->vtabEntries->front()->virtuals = symListFactory.CreateList();
                            sym->sb->vtabEntries->front()->virtuals->push_back(cur1);
                        }
                    }
                    if (cur1->sb->isoverride && !found && !isfirst && (!definingTemplate || instantiatingTemplate))
                    {
                        errorsym(ERR_FUNCTION_DOES_NOT_OVERRIDE, cur1);
                    }
                }
            }
        }
    }
    if (sym->sb->vbaseEntries)
        for (auto vb : *sym->sb->vbaseEntries)
            if (vb->alloc)
            {
                VTABENTRY* match = nullptr;
                checkAmbiguousVirtualFunc(vb->cls, &match, sym->sb->vtabEntries);
            }
    checkExceptionSpecification(sym);
    allocVTabSpace(sym->sb->vtabEntries, 0);
    if (sym->sb->vtabEntries->size() && sym->sb->vtabEntries->front()->virtuals)
    {
        int ofs = 0;
        for (auto func : *sym->sb->vtabEntries->front()->virtuals)
        {
            if (func->sb->parentClass == sym)
            {
                func->sb->vtaboffset = ofs;
            }
            ofs += getSize(BasicType::pointer_);
        }
    }
}
void calculateVirtualBaseOffsets(SYMBOL* sym)
{
    if (sym->sb->vbaseEntries)
        sym->sb->vbaseEntries->clear();
    // copy all virtual base classes of direct base classes
    if (sym->sb->baseClasses)
    {
        for (auto lst : *sym->sb->baseClasses)
        {
            if (lst->cls->sb->vbaseEntries)
            {
                for (auto cur : *lst->cls->sb->vbaseEntries)
                {
                    auto vbase = Allocate<VBASEENTRY>();
                    vbase->alloc = false;
                    vbase->cls = cur->cls;
                    vbase->pointerOffset = cur->pointerOffset + lst->offset;
                    vbase->structOffset = 0;
                    if (!sym->sb->vbaseEntries)
                        sym->sb->vbaseEntries = vbaseEntryListFactory.CreateList();
                    sym->sb->vbaseEntries->push_back(vbase);

                    bool found = false;
                    for (auto search : *sym->sb->vbaseEntries)
                        if (search->cls == vbase->cls && search->alloc)
                        {
                            found = true;
                            break;
                        }
                    if (!found)
                    {
                        // copy for the derived class's vbase table
                        vbase = Allocate<VBASEENTRY>();
                        vbase->alloc = true;
                        vbase->cls = cur->cls;
                        vbase->pointerOffset = 0;
                        vbase->structOffset = 0;
                        sym->sb->vbaseEntries->push_back(vbase);
                    }
                }
            }
        }
    }
    // now add any new base classes for this derived class
    if (sym->sb->baseClasses)
    {
        for (auto lst : *sym->sb->baseClasses)
        {
            if (lst->isvirtual)
            {
                bool found = false;
                if (sym->sb->vbaseEntries)
                {
                    for (auto search : *sym->sb->vbaseEntries)
                    {
                        if (search->cls == lst->cls && search->alloc)
                        {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                {
                    auto vbase = Allocate<VBASEENTRY>();
                    vbase->alloc = true;
                    vbase->cls = lst->cls;
                    vbase->pointerOffset = 0;
                    vbase->structOffset = 0;
                    if (!sym->sb->vbaseEntries)
                        sym->sb->vbaseEntries = vbaseEntryListFactory.CreateList();
                    sym->sb->vbaseEntries->push_back(vbase);
                }
            }
        }
    }
    // modify virtual base thunks for self
    if (sym->sb->vbaseEntries)
    {
        for (auto vbase : *sym->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                int align;
                BASECLASS* base;
                align = getBaseAlign(BasicType::pointer_);
                sym->sb->attribs.inheritable.structAlign = imax(sym->sb->attribs.inheritable.structAlign, align);
                if (align != 1)
                {
                    int al = sym->tp->size % align;
                    if (al != 0)
                    {
                        sym->tp->size += align - al;
                    }
                }
                if (sym->sb->baseClasses)
                    for (auto base : *sym->sb->baseClasses)
                        if (base->isvirtual && base->cls == vbase->cls)
                        {
                            if (base != sym->sb->baseClasses->front())
                                base->offset = sym->tp->size;
                            break;
                        }
                vbase->pointerOffset = sym->tp->size;
                sym->tp->size += getSize(BasicType::pointer_);
            }
        }
    }
    sym->sb->sizeNoVirtual = sym->tp->size;
    if (sym->sb->vbaseEntries)
    {
        for (auto vbase : *sym->sb->vbaseEntries)
        {
            if (vbase->alloc)
            {
                int n;
                int align = vbase->cls->sb->attribs.inheritable.structAlign;
                VBASEENTRY* cur;
                sym->sb->attribs.inheritable.structAlign = imax(sym->sb->attribs.inheritable.structAlign, align);
                if (align != 1)
                {
                    int al = sym->tp->size % align;
                    if (al != 0)
                    {
                        sym->tp->size += align - al;
                    }
                }
                if (sym->sb->vbaseEntries)
                    for (auto cur : *sym->sb->vbaseEntries)
                        if (cur->cls == vbase->cls)
                            cur->structOffset = sym->tp->size;
                if (vbase->cls->sb->maintemplate && !vbase->cls->sb->sizeNoVirtual)
                    n = vbase->cls->sb->maintemplate->sb->sizeNoVirtual;
                else
                    n = vbase->cls->sb->sizeNoVirtual;
                sym->tp->size += n;
            }
        }
    }
}
void deferredCompileOne(SYMBOL* cur)
{
    LexList* lex;
    std::list<LAMBDA*> oldLambdas;
    // function body
    if (!cur->sb->inlineFunc.stmt && (!cur->sb->templateLevel || !cur->templateParams || cur->sb->instantiated))
    {
        int tns;
        if (cur->sb->parentClass)
            tns = PushTemplateNamespace(cur->sb->parentClass);
        else
            tns = PushTemplateNamespace(cur);
        auto linesOld = lines;
        lines = nullptr;

        cur->sb->attribs.inheritable.linkage4 = Linkage::virtual_;
        enclosingDeclarations.Mark();
        if (cur->templateParams && cur->sb->templateLevel)
        {
            enclosingDeclarations.Add(cur->templateParams);
        }
        if (cur->sb->parentClass)
        {
            enclosingDeclarations.Add(cur->sb->parentClass);
            if (cur->sb->parentClass->templateParams)
            {
                enclosingDeclarations.Add(cur->sb->parentClass->templateParams);
            }
        }
        dontRegisterTemplate++;
        lex = SetAlternateLex(cur->sb->deferredCompile);
        if (MATCHKW(lex, Keyword::colon_))
        {
            lex = getsym();
            cur->sb->memberInitializers = GetMemberInitializers(&lex, nullptr, cur);
        }
        oldLambdas = lambdas;
        lambdas.clear();
        int oldStructLevel = structLevel;
        structLevel = 0;
        auto oldOpen = openStructs;
        openStructs = nullptr;
        cur->sb->deferredCompile = nullptr;
        StatementGenerator sg(lex, cur);
        sg.FunctionBody();
        SetAlternateLex(nullptr);
        dontRegisterTemplate--;
        lambdas = std::move(oldLambdas);
        openStructs = oldOpen;
        structLevel = oldStructLevel;
        enclosingDeclarations.Release();
        PopTemplateNamespace(tns);
        lines = linesOld;
    }
}
static void RecalcArraySize(Type* tp)
{
    if (tp->BaseType()->btp->IsArray())
        RecalcArraySize(tp->BaseType()->btp);
    tp->size = tp->BaseType()->btp->BaseType()->size * tp->BaseType()->esize->v.i;
}
void deferredInitializeDefaultArg(SYMBOL* arg, SYMBOL* func)
{
    if (!definingTemplate || instantiatingTemplate)
    {
        if (arg->sb->deferredCompile && !arg->sb->init)
        {
            bool initted = false;
            int oldInstantiatingTemplate;
            auto str = func->sb->parentClass;
            if (str && str->templateParams && !allTemplateArgsSpecified(str, str->templateParams))
            {
                initted = true;
                oldInstantiatingTemplate = instantiatingTemplate;
                instantiatingTemplate = 0;
                definingTemplate++;
            }
            LexList* lex;
            Type* tp2;
            int tns = PushTemplateNamespace(str ? str : func);
            enclosingDeclarations.Mark();
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
            lex = SetAlternateLex(arg->sb->deferredCompile);

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
                lex = initialize(lex, theCurrentFunc, sym2, StorageClass::auto_, false, false, false, 0); /* also reserves space */
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
                lex = initialize(lex, theCurrentFunc, arg, StorageClass::member_, false, false, false, 0);
            }
            SetAlternateLex(nullptr);
            arg->sb->deferredCompile = nullptr;
            enclosingDeclarations.Release();
            PopTemplateNamespace(tns);
            if (initted)
            {
                definingTemplate--;
                instantiatingTemplate = oldInstantiatingTemplate;
            }
        }
    }
}
void deferredInitializeStructFunctions(SYMBOL* cur)
{
    enclosingDeclarations.Mark();
    LexList* lex;
    int tns = PushTemplateNamespace(cur);
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
            if (definingTemplate != 1 || instantiatingTemplate)
            {
                for (auto sp1 : *sp->tp->syms)
                {
                    if (!sp1->sb->templateLevel)
                    {
                        for (auto sp2 : *sp1->tp->BaseType()->syms)
                        {
                            if (!sp2->sb->thisPtr)
                            {
                                if (sp2->sb->deferredCompile && !sp2->sb->init)
                                {
                                    sp2->tp->InstantiateDeferred();
                                    Type* tp2;
                                    lex = SetAlternateLex(sp2->sb->deferredCompile);
                                    tp2 = sp2->tp;
                                    if (tp2->IsRef())
                                        tp2 = tp2->BaseType()->btp;
                                    if (!tp2->IsStructured())
                                    {
                                        initialize(lex, theCurrentFunc, sp2, StorageClass::member_, false, false, false, 0);
                                        sp2->sb->deferredCompile = nullptr;
                                    }
                                    SetAlternateLex(nullptr);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //    dontRegisterTemplate--;
    enclosingDeclarations.Release();
    PopTemplateNamespace(tns);
}
void deferredInitializeStructMembers(SYMBOL* cur)
{
    Optimizer::LIST* staticAssert;
    LexList* lex;
    int tns = PushTemplateNamespace(cur);
    enclosingDeclarations.Mark();
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
        else if (sp->sb->deferredCompile && !sp->sb->init)
        {
            lex = SetAlternateLex(sp->sb->deferredCompile);
            sp->sb->deferredCompile = nullptr;
            lex = initialize(lex, theCurrentFunc, sp, StorageClass::member_, false, false, false, 0);
            SetAlternateLex(nullptr);
            if (sp->sb->constexpression)
            {
                optimize_for_constants(&sp->sb->init->front()->exp);
            }
        }
    }
    dontRegisterTemplate--;
    enclosingDeclarations.Release();
    PopTemplateNamespace(tns);
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
LexList* baseClasses(LexList* lex, SYMBOL* funcsp, SYMBOL* declsym, AccessLevel defaultAccess)
{
    auto baseClasses = baseClassListFactory.CreateList();
    AccessLevel currentAccess;
    bool isvirtual = false;
    bool done = false;
    SYMBOL* bcsym;
    currentAccess = defaultAccess;
    lex = getsym();  // past ':'
    if (declsym->tp->BaseType()->type == BasicType::union_)
        error(ERR_UNION_CANNOT_HAVE_BASE_CLASSES);
    enclosingDeclarations.Add(declsym);
    do
    {
        ParseAttributeSpecifiers(&lex, funcsp, true);
        EnterPackedSequence();
        if (MATCHKW(lex, Keyword::decltype_))
        {
            Type* tp = nullptr;
            tp = TypeGenerator::TypeId(lex, funcsp, StorageClass::type_, true, true, false);
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
                auto bc = innerBaseClass(declsym, tp->sp, isvirtual, currentAccess);
                if (bc)
                    baseClasses->push_back(bc);
            }
            done = !MATCHKW(lex, Keyword::comma_);
            if (!done)
                lex = getsym();
        }
        else if (MATCHKW(lex, Keyword::classsel_) || ISID(lex))
        {
            char name[512];
            name[0] = 0;
            if (ISID(lex))
                Utils::StrCpy(name, lex->data->value.s.a);
            bcsym = nullptr;
            lex = nestedSearch(lex, &bcsym, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, false, false);
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
            lex = getsym();
        restart:
            if (bcsym && bcsym->tp->type == BasicType::templateselector_)
            {
                //                if (!definingTemplate && !declaringTemplate((*bcsym->tp->sp->sb->templateSelector)[1].sp))
                //                    error(ERR_STRUCTURED_TYPE_EXPECTED_IN_TEMPLATE_PARAMETER);
                if (MATCHKW(lex, Keyword::lt_))
                {
                    inTemplateSpecialization++;
                    std::list<TEMPLATEPARAMPAIR>* nullLst = nullptr;
                    lex = GetTemplateArguments(lex, funcsp, bcsym, &nullLst);
                    inTemplateSpecialization--;
                    auto&& tsl = bcsym->tp->sp->sb->templateSelector->back();
                    tsl.isTemplate = true;
                    tsl.templateParams = nullLst;
                }
                //                bcsym = nullptr;
                if (bcsym && (!definingTemplate || instantiatingTemplate))
                {
                    auto bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                    if (bc)
                        baseClasses->push_back(bc);
                }
                done = !MATCHKW(lex, Keyword::comma_);
                if (!done)
                {
                    lex = getsym();
                    LeavePackedSequence();
                    continue;
                }
                goto endloop;
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
                        lex = GetTemplateArguments(lex, funcsp, bcsym, &lst);
                        inTemplateSpecialization--;
                        currentAccess = defaultAccess;
                        isvirtual = false;
                        done = !MATCHKW(lex, Keyword::comma_);
                        if (!done)
                            lex = getsym();
                        goto endloop;
                    }
                }
                if (bcsym->sb->storage_class == StorageClass::typedef_)
                {
                    if (MATCHKW(lex, Keyword::lt_))
                    {
                        std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                        SYMBOL* sp1;
                        inTemplateSpecialization++;
                        lex = GetTemplateArguments(lex, funcsp, bcsym, &lst);
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
                            if (definingTemplate && bcsym->tp->type == BasicType::any_)
                            {
                                currentAccess = defaultAccess;
                                isvirtual = false;
                                done = !MATCHKW(lex, Keyword::comma_);
                                if (!done)
                                    lex = getsym();
                                LeavePackedSequence();
                                continue;
                            }
                            else if (bcsym && (!definingTemplate || instantiatingTemplate))
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
                    done = !MATCHKW(lex, Keyword::comma_);
                    if (!done)
                        lex = getsym();
                    LeavePackedSequence();
                    continue;
                }
                else
                {
                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                    if (MATCHKW(lex, Keyword::lt_))
                    {
                        int i;
                        inTemplateSpecialization++;
                        lex = GetTemplateArguments(lex, funcsp, bcsym, &lst);
                        inTemplateSpecialization--;
                        if (MATCHKW(lex, Keyword::ellipse_))
                        {
                            if (definingTemplate)
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
                                int n = 0;
                                bool done = false;
                                bool failed = false;
                                while (!done)
                                {
                                    std::list<TEMPLATEPARAMPAIR>* workingList = templateParamPairListFactory.CreateList();
                                    SYMBOL* temp;
                                    bool packed = false;
                                    auto itdest = bcsym->templateParams->begin();
                                    ++itdest;
                                    auto itdeste = bcsym->templateParams->end();
                                    auto itsrc = lst->begin();
                                    auto itsrce = lst->end();
                                    for (; itsrc != itsrce && itdest != itdeste; ++itsrc, ++itdest)
                                    {
                                        auto second = Allocate<TEMPLATEPARAM>();
                                        workingList->push_back(TEMPLATEPARAMPAIR{nullptr, second});
                                        if (itsrc->second->packed && itsrc->second->byPack.pack)
                                        {
                                            auto itp = itsrc->second->byPack.pack->begin();
                                            auto itpe = itsrc->second->byPack.pack->end();
                                            packed = true;
                                            for (i = 0; i < n && itp != itpe; i++, ++itp)
                                                ;
                                            if (itp == itpe)
                                            {
                                                done = true;
                                                break;
                                            }
                                            *second = *(itp->second);
                                            second->byClass.dflt = itp->second->byClass.val;
                                        }
                                        else
                                        {
                                            *second = *(itsrc->second);
                                            second->byClass.dflt = itsrc->second->byClass.val;
                                        }
                                    }
                                    if (done)
                                        break;
                                    if (workingList)
                                    {
                                        temp = GetClassTemplate(bcsym, workingList, true);
                                        if (temp && allTemplateArgsSpecified(temp, temp->templateParams))
                                            temp = TemplateClassInstantiateInternal(temp, temp->templateParams, false);
                                        if (temp)
                                        {
                                            auto bc = innerBaseClass(declsym, temp, isvirtual, currentAccess);
                                            if (bc)
                                            {
                                                baseClasses->push_back(bc);
                                            }
                                        }
                                        n++;
                                    }
                                    if (!packed)
                                        break;
                                }
                                auto itdest = bcsym->templateParams->begin();
                                ++itdest;
                                auto itdeste = bcsym->templateParams->end();
                                auto itsrc = lst->begin();
                                auto itsrce = lst->end();
                                for (; itsrc != itsrce && itdest != itdeste; ++itdest)
                                {
                                    if (itsrc->second->packed && itsrc->second->byPack.pack)
                                    {
                                        auto itp = itsrc->second->byPack.pack->begin();
                                        auto itpe = itsrc->second->byPack.pack->end();
                                        for (i = 0; i < n && itp != itpe; i++, ++itp)
                                            ;
                                        if (itp != itpe)
                                            failed = true;
                                    }
                                }
                                if (failed)
                                {
                                    error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                                }
                            }
                            lex = getsym();
                            currentAccess = defaultAccess;
                            isvirtual = false;
                            done = !MATCHKW(lex, Keyword::comma_);
                            if (!done)
                                lex = getsym();
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
                                bcsym->tp->sp = bcsym;
                            }
                        }
                    }
                }
            }
            else if (MATCHKW(lex, Keyword::lt_))
            {
                errorstr(ERR_NOT_A_TEMPLATE, bcsym ? bcsym->name : name[0] ? name : "unknown");
            }
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
                if (!MATCHKW(lex, Keyword::ellipse_))
                    error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                else
                    lex = getsym();
                currentAccess = defaultAccess;
                isvirtual = false;
                done = !MATCHKW(lex, Keyword::comma_);
                if (!done)
                    lex = getsym();
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
                done = !MATCHKW(lex, Keyword::comma_);
                if (!done)
                    lex = getsym();
            }
            else if (bcsym && (istype(bcsym) && bcsym->tp->IsStructured()))
            {
                auto bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                if (bc)
                    baseClasses->push_back(bc);
                currentAccess = defaultAccess;
                isvirtual = false;
                done = !MATCHKW(lex, Keyword::comma_);
                if (!done)
                    lex = getsym();
            }
            else
            {
                if (!definingTemplate)
                    error(ERR_CLASS_TYPE_EXPECTED);
                done = true;
            }
        }
        else
            switch (KW(lex))
            {
                case Keyword::virtual_:
                    isvirtual = true;
                    lex = getsym();
                    break;
                case Keyword::private_:
                    currentAccess = AccessLevel::private_;
                    lex = getsym();
                    break;
                case Keyword::protected_:
                    currentAccess = AccessLevel::protected_;
                    lex = getsym();
                    break;
                case Keyword::public_:
                    currentAccess = AccessLevel::public_;
                    lex = getsym();
                    break;
                default:
                    error(ERR_IDENTIFIER_EXPECTED);
                    errskim(&lex, skim_end);
                    enclosingDeclarations.Drop();
                    return lex;
            }
    endloop:
        LeavePackedSequence();
        if (!done)
            ParseAttributeSpecifiers(&lex, funcsp, true);
    } while (!done);
    enclosingDeclarations.Drop();

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
    return lex;
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
                            if (!definingTemplate)
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
                            if (!definingTemplate)
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
                            if (!definingTemplate)
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
                            if (!definingTemplate)
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
LexList* handleStaticAssert(LexList* lex)
{
    RequiresDialect::Keyword(Dialect::c11, "_Static_assert");
    if (!MATCHKW(lex, Keyword::openpa_))
    {
        errskim(&lex, skim_closepa);
        skip(&lex, Keyword::closepa_);
    }
    else if (structLevel && enclosingDeclarations.GetFirst())
    {
        LexList* deferredCompile = nullptr;
        lex = getDeferredData(lex, &deferredCompile, false);
        EnterStructureStaticAssert(enclosingDeclarations.GetFirst(), deferredCompile);
    }
    else
    {
        lex = getsym();  // past '('
        bool v = true;
        char buf[5000];
        Type* tp;
        EXPRESSION *expr = nullptr, *expr2 = nullptr;
        inConstantExpression++;
        anonymousNotAlloc++;
        inStaticAssert++;
        lex = expression_assign(lex, nullptr, nullptr, &tp, &expr, nullptr, 0);
        inStaticAssert--;
        anonymousNotAlloc--;
        expr2 = Allocate<EXPRESSION>();
        expr2->type = ExpressionNode::x_bool_;
        expr2->left = expr->type == ExpressionNode::select_ ? expr->left : expr;
        optimize_for_constants(&expr2);
        inConstantExpression--;
        if (!isarithmeticconst(expr2) && !definingTemplate)
            error(ERR_CONSTANT_VALUE_EXPECTED);
        v = expr2->v.i;

        if (MATCHKW(lex, Keyword::comma_))
        {
            lex = getsym();
            if (lex->data->type != LexType::l_astr_)
            {
                error(ERR_NEEDSTRING);
                buf[0] = 0;
                v = true; // don't generate the static assert
            }
            else
            {
                int i, pos = 0;
                while (lex->data->type == LexType::l_astr_)
                {
                    Optimizer::SLCHAR* ch = (Optimizer::SLCHAR*)lex->data->value.s.w;
                    lex = getsym();
                    for (i = 0; i < ch->count && i + pos < sizeof(buf) - 1; i++)
                        buf[i + pos] = ch->str[i];
                    pos += i;
                }
                buf[pos] = 0;
            }
        }
        else
        {
            Utils::StrCpy(buf, "(unspecified)");
        }
        if (!needkw(&lex, Keyword::closepa_))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, Keyword::closepa_);
        }
        else if (!v && (!definingTemplate))  // || instantiatingTemplate))
        {
            errorstr(ERR_STATIC_ASSERT, buf);
        }
    }
    return lex;
}
static void InsertTag(SYMBOL* sym, StorageClass storage_class, bool allowDups)
{
    SymbolTable<SYMBOL>* table;
    SYMBOL* ssp = enclosingDeclarations.GetFirst();
    SYMBOL* sp1 = nullptr;
    if (ssp &&
        (storage_class == StorageClass::member_ || storage_class == StorageClass::mutable_ || storage_class == StorageClass::type_))
    {
        table = ssp->tp->tags;
    }
    else if (storage_class == StorageClass::auto_ || storage_class == StorageClass::register_ ||
             storage_class == StorageClass::parameter_ || storage_class == StorageClass::localstatic_)
        table = localNameSpace->front()->tags;
    else
        table = globalNameSpace->front()->tags;
    if (allowDups)
        sp1 = search(table, sym->name);
    if (!allowDups || !sp1 || (sym != sp1 && sym->sb->mainsym && sym->sb->mainsym != sp1->sb->mainsym))
        table->Add(sym);
}
LexList* insertUsing(LexList* lex, SYMBOL** sp_out, AccessLevel access, StorageClass storage_class, bool inTemplate,
                     bool hasAttributes)
{
    SYMBOL* sp;
    if (MATCHKW(lex, Keyword::namespace_))
    {
        lex = getsym();
        if (ISID(lex))
        {
            // by spec using directives match the current state of
            // the namespace at all times... so we cache pointers to
            // related namespaces
            lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, StorageClass::global_, true, false);
            if (sp)
            {
                if (sp->sb->storage_class != StorageClass::namespace_ && sp->sb->storage_class != StorageClass::namespace_alias_)
                {
                    errorsym(ERR_NOT_A_NAMESPACE, sp);
                }
                else
                {
                    bool found = false;
                    if (globalNameSpace->front()->usingDirectives)
                        for (auto t : *globalNameSpace->front()->usingDirectives)
                            if (t == sp)
                            {
                                found = true;
                                break;
                            }
                    if (!found)
                    {
                        if (!globalNameSpace->front()->usingDirectives)
                            globalNameSpace->front()->usingDirectives = symListFactory.CreateList();
                        globalNameSpace->front()->usingDirectives->push_front(sp);
                        if (!IsCompiler() && lex)
                            CompletionCompiler::ccInsertUsing(sp, nameSpaceList.size() ? nameSpaceList.front() : nullptr,
                                                              lex->data->errfile, lex->data->errline);
                    }
                }
                lex = getsym();
            }
        }
        else
        {
            error(ERR_EXPECTED_NAMESPACE_NAME);
        }
    }
    else
    {
        bool isTypename = false;
        if (MATCHKW(lex, Keyword::typename_))
        {
            isTypename = true;
            lex = getsym();
        }

        if (hasAttributes)
            error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
        if (!isTypename && ISID(lex))
        {
            LexList* idsym = lex;
            lex = getsym();
            attributes oldAttribs = basisAttribs;
            basisAttribs = {0};
            ParseAttributeSpecifiers(&lex, nullptr, true);
            if (MATCHKW(lex, Keyword::assign_))
            {
                Type* tp = nullptr;
                SYMBOL* sp;
                lex = getsym();
                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                bool pulledtypename = false;
                if (MATCHKW(lex, Keyword::typename_))
                {
                    pulledtypename = true;
                    lex = getsym();
                }
                if (inTemplate && (ISID(lex) || MATCHKW(lex, Keyword::classsel_) || MATCHKW(lex, Keyword::typename_)))
                {
                    SYMBOL *sym = nullptr, *strsym = nullptr;
                    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
                    bool throughClass = false;
                    parsingTrailingReturnOrUsing++;
                    lex = id_expression(lex, nullptr, &sym, &strsym, nullptr, nullptr, false, false, nullptr, 0, 0);
                    if (sym)
                    {
                        tp = sym->tp;
                        lex = getsym();
                        if (MATCHKW(lex, Keyword::lt_))
                        {
                            lex = GetTemplateArguments(lex, nullptr, sym, &lst);
                        }
                    }
                    else if (strsym && strsym->tp->type == BasicType::templateselector_)
                    {
                        tp = strsym->tp;
                        lex = getsym();
                        if (MATCHKW(lex, Keyword::lt_))
                        {
                            lex = GetTemplateArguments(lex, (*strsym->tp->sp->sb->templateSelector)[1].sp, sym, &lst);
                        }
                    }
                    parsingTrailingReturnOrUsing--;
                    Linkage linkage = Linkage::none_, linkage2 = Linkage::none_, linkage3 = Linkage::none_;
                    bool defd = false;
                    SYMBOL* sp = nullptr;
                    bool notype = false;
                    bool oldTemplateType = inTemplateType;

                    tp = TypeGenerator::BeforeName(lex, nullptr, tp, &sp, nullptr, nullptr, false, storage_class, &linkage,
                                                   &linkage2, &linkage3, nullptr, false, false, true,
                                                   false); /* fixme at file scope init */
                }
                else
                {
                    if (pulledtypename)
                        lex = backupsym();
                    parsingUsing++;
                    tp = TypeGenerator::TypeId(lex, nullptr, StorageClass::cast_, false, true, true);
                    parsingUsing--;
                }
                if (!tp)
                {
                    tp = &stdint;
                }
                checkauto(tp, ERR_AUTO_NOT_ALLOWED_IN_USING_STATEMENT);
                sp = makeID(StorageClass::typedef_, tp, nullptr, litlate(idsym->data->value.s.a));
                sp->sb->attribs = basisAttribs;
                sp->tp = Type::MakeType(BasicType::typedef_, tp);
                sp->tp->sp = sp;
                sp->sb->access = access;
                if (inTemplate)
                {
                    sp->sb->templateLevel = definingTemplate;
                    sp->templateParams = TemplateGetParams(sp);
                    if (tp->IsStructured() || tp->BaseType()->type == BasicType::templateselector_)
                        sp->sb->typeAlias = lst;
                }
                else if (sp->tp->IsDeferred() && (!definingTemplate || instantiatingTemplate))
                {
                    if (!sp->tp->BaseType()->sp->sb->templateLevel)
                        sp->tp->InstantiateDeferred();
                }
                if (storage_class == StorageClass::member_)
                    sp->sb->parentClass = enclosingDeclarations.GetFirst();
                sp->sb->usingTypedef = true;
                SetLinkerNames(sp, Linkage::cdecl_);
                // note that we can redeclare a using statement...
                // so look it up and if it exists make sure the types match...
                SYMBOL* spi;
                if (storage_class == StorageClass::auto_ || storage_class == StorageClass::parameter_)
                {
                    spi = localNameSpace->front()->syms->Lookup(sp->name);
                }
                else
                {
                    auto ssp = enclosingDeclarations.GetFirst();
                    if (ssp && ssp->sb->templateLevel)
                    {
                        SYMBOL* ssp2 = FindSpecialization(ssp, ssp->templateParams);
                        if (ssp2)
                            ssp = ssp2;
                    }
                    if (ssp && ssp->tp->syms)
                    {
                        spi = ssp->tp->syms->Lookup(sp->name);
                    }
                    else
                    {
                        spi = globalNameSpace->front()->syms->Lookup(sp->name);
                    }
                }
                if (spi)
                {
                    if (!spi->tp->CompatibleType(sp->tp) && !SameTemplate(spi->tp, sp->tp))
                    {
                        errorsym(ERR_TYPE_MISMATCH_IN_REDECLARATION, sp);
                    }
                    sp = spi;
                }
                else
                {
                    InsertSymbol(sp, storage_class, Linkage::cdecl_, false);
                }
                if (sp_out)
                    *sp_out = sp;
                basisAttribs = oldAttribs;
                return lex;
            }
            else
            {
                lex = backupsym();
            }
            basisAttribs = oldAttribs;
        }
        // if we get here we suppose it is classic using, e.g. import the type or variable from another context...
        std::list<NAMESPACEVALUEDATA*>* nssym = globalNameSpace;
        enum Mode
        {
            parseFirst,
            parseClass,
            parseNamespace
        } mode = parseFirst;
        if (MATCHKW(lex, Keyword::classsel_))
        {
            nssym = rootNameSpace;
            mode = parseNamespace;
            lex = getsym();
        }
        // there is an extension to C++17 that allows packed variables to be used as part of the pack,
        // then the using is applied to all elements of the path.   So we have to gather the path independently of
        // evaluating it...
        std::list<std::pair<std::string, LexList*>> path;
        while (true)
        {
            if (!ISID(lex) && !MATCHKW(lex, Keyword::operator_))
            {
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(&lex, skim_semi, false);
                break;
            }
            char buf[512];
            int ov;
            Type* castType;
            lex = getIdName(lex, theCurrentFunc, buf, sizeof(buf), & ov, &castType);
            lex = getsym();
            LexList* start = nullptr;
            if (MATCHKW(lex, Keyword::lt_))
            {
                start = lex;
                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                lex = GetTemplateArguments(lex, theCurrentFunc, nullptr, &lst);
            }
            path.push_back(std::pair<std::string, LexList*>(buf, start));
            if (!MATCHKW(lex, Keyword::classsel_))
            {
                break;
            }
            lex = getsym();
        }
        bool ellipsis = false;
        if (MATCHKW(lex, Keyword::ellipse_))
        {
            ellipsis = true;
            lex = getsym();
            RequiresDialect::Feature(Dialect::cpp17, "Packed Using Extension");
        }
        SYMBOL *sp = nullptr, *strSym = nullptr;
        bool done = false;
        bool absorbedPack = false;
        std::stack<std::tuple<decltype(path)::iterator, int, int, const char*, SYMBOL*, Mode>> stk;
        const char* lastName = nullptr;
        auto it = path.begin();
        while (!done)
        {
            done = true;
            bool skipProcess = false;
            for (; it != path.end();)
            {
                const char* buf = it->first.c_str();
                if (lastName && !strcmp(lastName, buf))
                {
                    buf = overloadNameTab[CI_CONSTRUCTOR];
                }
                strSym = sp;
                switch (mode)
                {
                    case parseFirst:
                        sp = namespacesearch(buf, localNameSpace, false, false);
                        if (!sp)
                            sp = classsearch(buf, false, MATCHKW(lex, Keyword::classsel_), false);
                        if (!sp)
                            sp = namespacesearch(buf, globalNameSpace, false, false);
                        break;
                    case parseNamespace:
                        sp = namespacesearch(buf, nssym, true, false);
                        break;
                    case parseClass: {
                        enclosingDeclarations.Add(strSym);
                        sp = classsearch(buf, false, MATCHKW(lex, Keyword::classsel_), false);
                        enclosingDeclarations.Drop();
                        if (!sp)
                            sp = classsearch(buf, false, MATCHKW(lex, Keyword::classsel_), false);
                    }
                    break;
                }
                if (sp)
                {
                    auto spin = sp;
                    if (sp->tp->type == BasicType::templateparam_ && sp->tp->templateParam->second->packed &&
                        sp->tp->templateParam->second->type == TplType::typename_ && !stk.empty() &&
                        std::get<0>(stk.top())->first == sp->name)
                    {
                        int n = std::get<1>(stk.top());
                        auto it1 = sp->tp->templateParam->second->byPack.pack->begin();
                        for (; it1 != sp->tp->templateParam->second->byPack.pack->end() && n--; ++it1)
                            ;
                        if (it1 == sp->tp->templateParam->second->byPack.pack->end())
                        {
                            // kind of a hokey error but this *should* never happen
                            error(ERR_PACKED_TEMPLATE_TYPE_MISMATCH);
                            return lex;
                        }
                        else
                        {
                            auto tp1 = (*it1).second->byClass.val;
                            if (!tp1->IsStructured())
                            {
                                error(ERR_NEED_PACKED_TEMPLATE_OF_TYPE_CLASS);
                                return lex;
                            }
                            auto sp1 = tp1->BaseType()->sp;
                            sp = sp1;
                            /*'
                        switch (mode)
                        {
                        case parseFirst:
                            sp = namespacesearch(sp1->name, localNameSpace, false, false);
                            if (!sp)
                                sp = classsearch(sp1->name, false, MATCHKW(lex, Keyword::classsel_), false);
                            if (!sp)
                                sp = namespacesearch(sp1->name, globalNameSpace, false, false);
                            break;
                        case parseNamespace:
                            sp = namespacesearch(sp1->name, nssym, true, false);
                            break;
                        case parseClass:
                        {
                            enclosingDeclarations.Add(strSym);
                            sp = classsearch(sp1->name, false, MATCHKW(lex, Keyword::classsel_), false);
                            enclosingDeclarations.Drop();
                            if (!sp)
                                sp = classsearch(sp1->name, false, MATCHKW(lex, Keyword::classsel_), false);
                        }
                        break;

                        }
                        */
                            if (!sp || strcmp(sp->name, sp1->name) != 0)
                            {
                                error(ERR_TYPE_NAME_EXPECTED);
                                return lex;
                            }
                        }
                    }
                    sp->tp->InstantiateDeferred();
                    sp->tp->InitializeDeferred();
                    if (sp->sb && (sp->sb->storage_class == StorageClass::namespace_ ||
                                   sp->sb->storage_class == StorageClass::namespace_alias_))
                    {
                        nssym = sp->sb->nameSpaceValues;
                        mode = parseNamespace;
                    }
                    else if (sp->tp->templateParam && sp->tp->templateParam->second->packed &&
                             sp->tp->templateParam->second->type == TplType::typename_)
                    {
                        if (!ellipsis)
                        {
                            // not unpacking
                            error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                            return lex;
                        }
                        else if (!sp->tp->templateParam->second->byPack.pack || !sp->tp->templateParam->second->byPack.pack->size())
                        {
                            // pack is empty, do nothing....
                            return lex;
                        }
                        else
                        {
                            absorbedPack = true;
                            stk.push(std::tuple<decltype(path)::iterator, int, int, const char*, SYMBOL*, Mode>(
                                it, -1, (int)sp->tp->templateParam->second->byPack.pack->size(), lastName, strSym, mode));
                            skipProcess = true;
                            break;
                        }
                    }
                    else if (sp->tp->IsStructured())
                    {
                        if (it->second)
                        {
                            auto lex1 = SetAlternateLex(it->second);
                            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                            lex1 = GetTemplateArguments(lex1, theCurrentFunc, sp, &lst);
                            SetAlternateLex(nullptr);
                            SYMBOL* sp1;
                            sp1 = GetClassTemplate(sp, lst, false);
                            if (sp1)
                            {
                                sp1 = TemplateClassInstantiate(sp1, lst, false, StorageClass::global_);
                                if (sp1)
                                {
                                    sp = sp1;
                                    sp->tp = sp->tp->InitializeDeferred();
                                }
                            }
                        }
                        mode = parseClass;
                    }
                    else
                    {
                        break;
                    }
                }
                lastName = buf;
                ++it;
            }
            if (!skipProcess)
            {
                if (!sp || sp->tp->type == BasicType::templateparam_ ||
                     (sp->sb && (!ismemberdata(sp) && !istype(sp) && sp->sb->storage_class != StorageClass::overloads_ &&
                     sp->sb->storage_class != StorageClass::global_ && sp->sb->storage_class != StorageClass::external_ &&
                     sp->sb->storage_class != StorageClass::static_ && sp->sb->storage_class != StorageClass::localstatic_ &&
                     sp->sb->storage_class != StorageClass::constant_)))
                {
                    if (!definingTemplate || instantiatingTemplate)
                        error(ERR_TYPE_OR_VARIABLE_EXPECTED);
                    errskim(&lex, skim_semi, false);
                }
                else
                {
                    if (sp && sp->sb)
                    {
                        if (!definingTemplate)
                        {
                            if (sp->sb->storage_class == StorageClass::overloads_)
                            {
                                for (auto sp2 : *sp->tp->syms)
                                {
                                    SYMBOL *ssp = enclosingDeclarations.GetFirst(), *ssp1;
                                    SYMBOL* sp1 = CopySymbol(sp2);
                                    sp1->sb->wasUsing = true;
                                    ssp1 = sp1->sb->parentClass;
                                    if (ssp && ismember(sp1))
                                        sp1->sb->parentClass = ssp;
                                    sp1->sb->mainsym = sp2;
                                    sp1->sb->access = access;
                                    InsertSymbol(sp1, storage_class, sp1->sb->attribs.inheritable.linkage, true);
                                    sp1->sb->parentClass = ssp1;
                                }
                            }
                            else
                            {
                                SYMBOL *ssp = enclosingDeclarations.GetFirst(), *ssp1;
                                SYMBOL* sp1 = CopySymbol(sp);
                                sp1->sb->wasUsing = true;
                                sp1->sb->mainsym = sp;
                                sp1->sb->access = access;
                                ssp1 = sp1->sb->parentClass;
                                if (ssp && ismember(sp1))
                                    sp1->sb->parentClass = ssp;
                                if (isTypename && !istype(sp) && sp->tp->type != BasicType::templateselector_)
                                    error(ERR_TYPE_NAME_EXPECTED);
                                if (istype(sp))
                                    InsertTag(sp1, storage_class, true);
                                else
                                    InsertSymbol(sp1, storage_class, Linkage::cdecl_, true);
                                sp1->sb->parentClass = ssp1;
                            }
                        }
                    }
                }
            }
            if (!stk.empty())
            {
                bool done2 = false;
                while (!done2)
                {
                    auto&& top = stk.top();
                    int start = std::get<1>(top);
                    ++start;
                    std::get<1>(top) = start;
                    it = std::get<0>(top);
                    lastName = std::get<3>(top);
                    sp = std::get<4>(top);
                    mode = std::get<5>(top);
                    if (start >= std::get<2>(top))
                    {
                        stk.pop();
                        done2 = stk.empty();
                    }
                    else
                    {
                        done2 = true;
                    }
                }
                // if we get here we used up all the tokens so exit...
                done = stk.empty();
            }
        }
        if (!absorbedPack && ellipsis)
        {
            error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
        }
    }
    return lex;
}
static void balancedAttributeParameter(LexList** lex)
{
    Keyword start = KW(*lex);
    Keyword endp = (Keyword)-1;
    *lex = getsym();
    switch (start)
    {
        case Keyword::openpa_:
            endp = Keyword::closepa_;
            break;
        case Keyword::begin_:
            endp = Keyword::end_;
            break;
        case Keyword::openbr_:
            endp = Keyword::closebr_;
            break;
        default:
            break;
    }
    if (endp != (Keyword)-1)
    {
        while (*lex && !MATCHKW(*lex, endp))
            balancedAttributeParameter(lex);
        needkw(lex, endp);
    }
}
Type* AttributeFinish(SYMBOL* sym, Type* tp)
{
    sym->sb->attribs = basisAttribs;
    tp->BaseType()->alignment = sym->sb->attribs.inheritable.structAlign;
    // should come first to overwrite all other attributes
    if (sym->sb->attribs.uninheritable.copyFrom)
    {
        if (sym->tp->IsFunction() == sym->sb->attribs.uninheritable.copyFrom->tp->IsFunction() &&
            istype(sym) == istype(sym->sb->attribs.uninheritable.copyFrom))
        {
            sym->sb->attribs.inheritable = sym->sb->attribs.uninheritable.copyFrom->sb->attribs.inheritable;
        }
        else
        {
            error(ERR_INVALID_ATTRIBUTE_COPY);
        }
    }
    if (sym->sb->attribs.inheritable.vectorSize)
    {
        if (tp->IsArithmetic())
        {
            int n = sym->sb->attribs.inheritable.vectorSize % tp->size;
            int m = sym->sb->attribs.inheritable.vectorSize / tp->size;
            if (n || m > 0x10000 || (m & (m - 1)) != 0)
                error(ERR_INVALID_VECTOR_SIZE);
            tp = Type::MakeType(BasicType::pointer_, tp);
            tp->size = sym->sb->attribs.inheritable.vectorSize;
            tp->array = true;
        }
        else
        {
            error(ERR_INVALID_VECTOR_TYPE);
        }
    }
    if (sym->sb->attribs.inheritable.cleanup && sym->sb->storage_class == StorageClass::auto_)
    {
        CallSite* fc = Allocate<CallSite>();
        fc->arguments = argumentListFactory.CreateList();
        auto arg = Allocate<Argument>();
        arg->tp = &stdpointer;
        arg->exp = MakeExpression(ExpressionNode::auto_, sym);
        fc->arguments->push_back(arg);
        fc->ascall = true;
        fc->functp = sym->sb->attribs.inheritable.cleanup->tp;
        fc->fcall = MakeExpression(ExpressionNode::pc_, sym->sb->attribs.inheritable.cleanup);
        fc->sp = sym->sb->attribs.inheritable.cleanup;
        EXPRESSION* expl = MakeExpression(fc);
        InsertInitializer(&sym->sb->dest, sym->tp, expl, 0, true);
    }
    return tp;
}
static const std::unordered_map<std::string, int, StringHash> gccStyleAttribNames = {
    {"alias", 1},    // 1 arg, alias name
    {"aligned", 2},  // arg is alignment; for members only increase unless also packed, otherwise can increase or decrease
    {"warn_if_not_aligned", 3},  // arg is the desired minimum alignment
    {"alloc_size", 4},           // implement by ignoring one or two args
    {"cleanup", 5},              // arg is afunc: similar to a destructor.   Also gets called during exception processing
                                 //                    { "common", 6 }, // no args, decide whether to support
                                 //                    { "nocommon", 7 }, // no args, decide whether to support
    {"copy", 8},          // one arg, varible/func/type, the two variable kinds must match don't copy alias visibility or weak
    {"deprecated", 9},    // zero or one arg, match C++
    {"nonstring", 10},    // has no null terminator
    {"packed", 11},       // ignore auto-align on this field
                          //                    { "section", 12 }, // one argument, the section name
                          //                    { "tls_model", 13 }, // one arg, the model.   Probably shouldn't support
    {"unused", 14},       // warning control
    {"used", 15},         // warning control
    {"vector_size", 16},  // one arg, which must be a power of two multiple of the base size.  implement as fixed-size array
    //                    { "visibility", 17 }, // one arg, 'default' ,'hidden', 'internal', 'protected.   don't
    //                    support for now as requires linker changes. { "weak", 18 }, // not supporting
    {"dllimport", 19},
    {"dllexport", 20},
    //                    { "selectany", 21 },  // requires linker support
    //                    { "shared", 22 },
    {"zstring", 23},  // non-gcc, added to support nonstring
    {"noreturn", 24},
    {"stdcall", 25},
    {"always_inline", 26},  // we don't really force inline this is still just a suggestion.   in practice the types of functions
                            // that get flagged with this will likely always be inlined anyway
    {"format", 27},
    {"internal_linkage", 28},
    {"exclude_from_explicit_instantiation", 29},
    {"constructor", 30},
    {"destructor", 31},
    {"stack_protect", 32},
};
#define DEFAULT_CONSTRUCTOR_PRIORITY 101
#define DEFAULT_DESTRUCTOR_PRIORITY 101

void ParseOut___attribute__(LexList** lex, SYMBOL* funcsp)
{
    if (MATCHKW(*lex, Keyword::attribute_))
    {
        *lex = getsym();
        if (needkw(lex, Keyword::openpa_))
        {
            if (needkw(lex, Keyword::openpa_))
            {
                if (ISID(*lex) || ISKW(*lex))
                {
                    std::string name;
                    if (ISID(*lex))
                        name = (*lex)->data->value.s.a;
                    else
                        name = (*lex)->data->kw->name;
                    // get rid of leading and trailing "__" if they both exist
                    if (name.size() >= 5 && name.substr(0, 2) == "__" && name.substr(name.size() - 2, 2) == "__")
                        name = name.substr(2, name.size() - 4);
                    *lex = getsym();
                    auto attrib = gccStyleAttribNames.find(name);
                    if (attrib == gccStyleAttribNames.end())
                    {
                        errorstr(ERR_ATTRIBUTE_DOES_NOT_EXIST, name.c_str());
                    }
                    else
                    {
                        switch (attrib->second)
                        {
                            case 1:  // alias
                                if (MATCHKW(*lex, Keyword::openpa_))
                                {
                                    *lex = getsym();
                                    if ((*lex)->data->type == LexType::l_astr_)
                                    {
                                        char buf[1024];
                                        int i;
                                        Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)(*lex)->data->value.s.w;
                                        for (i = 0; i < 1024 && i < xx->count; i++)
                                            buf[i] = (char)xx->str[i];
                                        buf[i] = 0;
                                        basisAttribs.uninheritable.alias = litlate(buf);
                                        *lex = getsym();
                                    }
                                    needkw(lex, Keyword::closepa_);
                                }
                                break;
                            case 2:  // aligned
                                if (MATCHKW(*lex, Keyword::openpa_))
                                {
                                    *lex = getsym();
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.structAlign = exp->v.i;
                                    basisAttribs.inheritable.alignedAttribute = true;
                                    needkw(lex, Keyword::closepa_);

                                    if (basisAttribs.inheritable.structAlign > 0x10000 ||
                                        (basisAttribs.inheritable.structAlign & (basisAttribs.inheritable.structAlign - 1)) != 0)
                                        error(ERR_INVALID_ALIGNMENT);
                                }
                                else
                                {
                                    // no argument means use max meaningful size
                                    basisAttribs.inheritable.structAlign =
                                        Optimizer::chosenAssembler->arch->type_align->a_alignedstruct;
                                    basisAttribs.inheritable.alignedAttribute = true;
                                }
                                break;
                            case 3:  // warn_if_not_aligned
                                if (needkw(lex, Keyword::openpa_))
                                {
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.warnAlign = exp->v.i;
                                    needkw(lex, Keyword::closepa_);

                                    if (basisAttribs.inheritable.warnAlign > 0x10000 ||
                                        (basisAttribs.inheritable.warnAlign & (basisAttribs.inheritable.warnAlign - 1)) != 0)
                                        error(ERR_INVALID_ALIGNMENT);
                                }
                                break;
                            case 4:  // alloc_size // doesn't restrict to numbers but maybe should?
                                if (needkw(lex, Keyword::openpa_))
                                {
                                    errskim(lex, skim_comma);
                                    if (MATCHKW(*lex, Keyword::comma_))
                                    {
                                        *lex = getsym();
                                        errskim(lex, skim_closepa);
                                    }
                                    needkw(lex, Keyword::closepa_);
                                }
                                break;
                            case 5:  // cleanup - needs work, should be in the C++ exception table for the function...
                                if (MATCHKW(*lex, Keyword::openpa_))
                                {
                                    *lex = getsym();
                                    if (ISID(*lex))
                                    {
                                        SYMBOL* sym = gsearch((*lex)->data->value.s.a);
                                        if (sym)
                                        {
                                            if (sym->tp->type == BasicType::aggregate_)
                                                if (sym->tp->BaseType()->syms->size() == 1)
                                                    sym = sym->tp->BaseType()->syms->front();
                                            if (sym->tp->IsFunction() && sym->tp->BaseType()->btp->IsVoid())
                                            {
                                                if (sym->tp->BaseType()->syms->size() == 0)
                                                {
                                                    basisAttribs.inheritable.cleanup = sym;
                                                }
                                                else if (sym->tp->BaseType()->syms->size() == 1)
                                                {
                                                    auto sp = sym->tp->BaseType()->syms->front();
                                                    if (sp->tp->IsVoid() || sp->tp->IsVoidPtr())
                                                        basisAttribs.inheritable.cleanup = sym;
                                                    else
                                                        error(ERR_INVALID_ATTRIBUTE_CLEANUP);
                                                }
                                                else
                                                {
                                                    error(ERR_INVALID_ATTRIBUTE_CLEANUP);
                                                }
                                            }
                                            else
                                            {
                                                error(ERR_INVALID_ATTRIBUTE_CLEANUP);
                                            }
                                        }
                                        else
                                        {
                                            errorstr(ERR_UNDEFINED_IDENTIFIER, (*lex)->data->value.s.a);
                                        }
                                        *lex = getsym();
                                    }
                                    else
                                    {
                                        error(ERR_IDENTIFIER_EXPECTED);
                                    }
                                    needkw(lex, Keyword::closepa_);
                                }
                                break;
                            case 8:  // copy
                                if (MATCHKW(*lex, Keyword::openpa_))
                                {
                                    *lex = getsym();
                                    if (ISID(*lex))
                                    {
                                        SYMBOL* sym = gsearch((*lex)->data->value.s.a);
                                        if (sym)
                                        {
                                            basisAttribs.uninheritable.copyFrom = sym;
                                        }
                                        else
                                        {
                                            errorstr(ERR_UNDEFINED_IDENTIFIER, (*lex)->data->value.s.a);
                                        }
                                        *lex = getsym();
                                    }
                                    else
                                    {
                                        error(ERR_IDENTIFIER_EXPECTED);
                                    }
                                    needkw(lex, Keyword::closepa_);
                                }
                                break;
                            case 9:  // deprecated
                                basisAttribs.uninheritable.deprecationText = (const char*)-1;
                                if (MATCHKW(*lex, Keyword::openpa_))
                                {
                                    *lex = getsym();
                                    if ((*lex)->data->type == LexType::l_astr_)
                                    {
                                        char buf[1024];
                                        int i;
                                        Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)(*lex)->data->value.s.w;
                                        for (i = 0; i < 1024 && i < xx->count; i++)
                                            buf[i] = (char)xx->str[i];
                                        buf[i] = 0;
                                        basisAttribs.uninheritable.deprecationText = litlate(buf);
                                        *lex = getsym();
                                    }
                                    needkw(lex, Keyword::closepa_);
                                }
                                break;
                            case 10:  // nonstring
                                basisAttribs.inheritable.nonstring = true;
                                break;
                            case 11:  // packed
                                basisAttribs.inheritable.packed = true;
                                break;
                            case 14:  // unused
                                basisAttribs.inheritable.used = true;
                                break;
                            case 15:  // used - this forces emission of static variables.  Since we always emit it is a noop
                                break;
                            case 16:  // vector_size
                                if (needkw(lex, Keyword::openpa_))
                                {
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.vectorSize = exp->v.i;
                                    needkw(lex, Keyword::closepa_);
                                }
                                break;
                            case 19:  // dllimport
                                if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage2 = Linkage::import_;
                                break;
                            case 20:  // dllexport
                                if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage2 = Linkage::export_;
                                break;
                            case 23:  // zstring
                                basisAttribs.inheritable.zstring = true;
                                break;
                            case 24:
                                if (basisAttribs.inheritable.linkage3 != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage3 = Linkage::noreturn_;
                                break;
                            case 25:  // stdcall
                                if (basisAttribs.inheritable.linkage != Linkage::none_)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage = Linkage::stdcall_;
                                break;
                            case 26:  // always inline
                                basisAttribs.inheritable.isInline = true;
                                break;
                            case 27:  // format
                                needkw(lex, Keyword::openpa_);
                                while (*lex && !MATCHKW(*lex, Keyword::closepa_))
                                    *lex = getsym();
                                if (*lex)
                                    *lex = getsym();
                                break;
                            case 28:  // internal_linkage
                                basisAttribs.inheritable.linkage2 = Linkage::internal_;
                                break;
                            case 29:  // exclude_from_explicit_instantiation
                                basisAttribs.inheritable.excludeFromExplicitInstantiation = true;
                                break;
                            case 30:  // constructor
                                if (MATCHKW(*lex, Keyword::openpa_))
                                {
                                    *lex = getsym();
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.uninheritable.constructorPriority = exp->v.i;
                                    basisAttribs.inheritable.warnAlign = exp->v.i;
                                    needkw(lex, Keyword::closepa_);
                                }
                                else
                                {
                                    basisAttribs.uninheritable.constructorPriority = DEFAULT_CONSTRUCTOR_PRIORITY;
                                }
                                break;
                            case 31:  // destructor
                                if (MATCHKW(*lex, Keyword::openpa_))
                                {
                                    *lex = getsym();
                                    Type* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !tp->IsInt())
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.uninheritable.destructorPriority = exp->v.i;
                                    basisAttribs.inheritable.warnAlign = exp->v.i;
                                    needkw(lex, Keyword::closepa_);
                                }
                                else
                                {
                                    basisAttribs.uninheritable.destructorPriority = DEFAULT_DESTRUCTOR_PRIORITY;
                                }
                                break;
                            case 32:  // stack-protect explicit
                                basisAttribs.uninheritable.stackProtect = true;
                                break;
                        }
                    }
                }
                needkw(lex, Keyword::closepa_);
                needkw(lex, Keyword::closepa_);
            }
            else
            {
                errskim(lex, skim_closepa);
                skip(lex, Keyword::closepa_);
                error(ERR_IGNORING__ATTRIBUTE);
            }
        }
    }
}
static const std::unordered_map<std::string, int, StringHash> occCPPStyleAttribNames = {
    {"zstring", 23},  // non-gcc, added to support nonstring
};
static const std::unordered_map<std::string, int, StringHash> clangCPPStyleAttribNames = {
    {"internal_linkage", 28},
    {"exclude_from_explicit_instantiation", 29},
};
static const std::unordered_map<std::string, int, StringHash> gccCPPStyleAttribNames = {
    {"alloc_size", 4},  // implement by ignoring one or two args
                        //                    { "common", 6 }, // no args, decide whether to support
                        //                    { "nocommon", 7 }, // no args, decide whether to support
                        //                    { "section", 12 }, // one argument, the section name
    //                    { "tls_model", 13 }, // one arg, the model.   Probably shouldn't
    //                    support { "visibility", 17 }, // one arg, 'default' ,'hidden',
    //                    'internal', 'protected.   don't support for now as requires linker
    //                    changes. { "weak", 18 }, // not supporting { "selectany", 21 }, //
    //                    requires linker support { "shared", 22 },
    {"dllexport", 25},
    {"dllimport", 26},
    {"stdcall", 27}};
std::string StripUnderscores(std::string str)
{
    size_t len = str.length();
    if (str[0] == '_' && str[1] == '_' && str[len - 2] == '_' && str[len - 1] == '_')
    {
        return str.substr(2, str.length() - 4);
    }
    return str;
}
bool ParseAttributeSpecifiers(LexList** lex, SYMBOL* funcsp, bool always)
{
    (void)always;
    bool rv = false;
    while (MATCHKW(*lex, Keyword::alignas_) || MATCHKW(*lex, Keyword::attribute_) || MATCHKW(*lex, Keyword::openbr_))
    {
        if (MATCHKW(*lex, Keyword::attribute_))
        {
            ParseOut___attribute__(lex, funcsp);
        }
        else if (MATCHKW(*lex, Keyword::alignas_))
        {
            RequiresDialect::Keyword(Dialect::c23, "alignas");
            rv = true;
            *lex = getsym();
            if (needkw(lex, Keyword::openpa_))
            {
                int align = 1;
                if (TypeGenerator::StartOfType(*lex, nullptr, false))
                {
                    Type* tp = nullptr;
                    tp = TypeGenerator::TypeId(*lex, funcsp, StorageClass::cast_, false, true, false);

                    if (!tp)
                    {
                        error(ERR_TYPE_NAME_EXPECTED);
                    }
                    else if (tp->type == BasicType::templateparam_)
                    {
                        if (tp->templateParam->second->type == TplType::typename_)
                        {
                            if (tp->templateParam->second->packed)
                            {
                                std::list<TEMPLATEPARAMPAIR>* packs = tp->templateParam->second->byPack.pack;
                                if (!MATCHKW(*lex, Keyword::ellipse_))
                                {
                                    error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                }
                                else
                                {
                                    *lex = getsym();
                                }
                                if (packs)
                                {
                                    for (auto&& pack : *packs)
                                    {
                                        int v = getAlign(StorageClass::global_, pack.second->byClass.val);
                                        if (v > align)
                                            align = v;
                                    }
                                }
                            }
                            else
                            {
                                // it will only get here while parsing the template...
                                // when generating the instance the class member will already be
                                // filled in so it will get to the below...
                                if (tp->templateParam->second->byClass.val)
                                    align = getAlign(StorageClass::global_, tp->templateParam->second->byClass.val);
                            }
                        }
                    }
                    else
                    {
                        align = getAlign(StorageClass::global_, tp);
                    }
                }
                else
                {
                    Type* tp = nullptr;
                    EXPRESSION* exp = nullptr;

                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                    if (!tp || !tp->IsInt())
                        error(ERR_NEED_INTEGER_TYPE);
                    else
                    {
                        if (!isintconst(exp))
                        {
                            align = 1;
                            if (exp->type != ExpressionNode::templateparam_)
                            {
                                error(ERR_CONSTANT_VALUE_EXPECTED);
                            }
                            else if (exp->v.templateParam && exp->v.templateParam->second->byNonType.val)
                            {
                                exp = exp->v.templateParam->second->byNonType.val;
                                if (!isintconst(exp))
                                    error(ERR_CONSTANT_VALUE_EXPECTED);
                                align = exp->v.i;
                            }
                        }
                        else
                        {
                            align = exp->v.i;
                        }
                    }
                }
                needkw(lex, Keyword::closepa_);
                basisAttribs.inheritable.structAlign = align;
                if (basisAttribs.inheritable.structAlign > 0x10000 ||
                    (basisAttribs.inheritable.structAlign & (basisAttribs.inheritable.structAlign - 1)) != 0)
                    error(ERR_INVALID_ALIGNMENT);
            }
        }
        else if (MATCHKW(*lex, Keyword::openbr_))
        {
            *lex = getsym();
            if (MATCHKW(*lex, Keyword::openbr_))
            {
                RequiresDialect::Feature(Dialect::c23, "Attribute specifiers");
                const std::string occNamespace = "occ";
                const std::string gccNamespace = "gnu";
                const std::string clangNamespace = "clang";
                rv = true;
                *lex = getsym();
                if (!MATCHKW(*lex, Keyword::closebr_))
                {
                    while (*lex)
                    {
                        bool special = false;
                        if (!ISID(*lex))
                        {
                            *lex = getsym();
                            error(ERR_IDENTIFIER_EXPECTED);
                        }
                        else
                        {
                            std::string stripped_ver = StripUnderscores((std::string)(*lex)->data->value.s.a);
                            if (stripped_ver == occNamespace)
                            {
                                *lex = getsym();
                                if (MATCHKW(*lex, Keyword::classsel_))
                                {
                                    *lex = getsym();
                                    if (!ISID(*lex))
                                    {
                                        *lex = getsym();
                                        error(ERR_IDENTIFIER_EXPECTED);
                                    }
                                    else if (*lex)
                                    {
                                        std::string name = (*lex)->data->value.s.a;
                                        name = StripUnderscores(name);
                                        auto searchedName = occCPPStyleAttribNames.find(name);
                                        if (searchedName != occCPPStyleAttribNames.end())
                                        {
                                            switch (searchedName->second)
                                            {
                                                case 23:
                                                    basisAttribs.inheritable.zstring = true;
                                                    break;
                                            }
                                        }
                                        else
                                        {
                                            errorstr2(ERR_ATTRIBUTE_DOES_NOT_EXIST_IN_NAMESPACE, name.c_str(),
                                                      occNamespace.c_str());
                                        }
                                        *lex = getsym();
                                    }
                                }
                                else
                                {
                                    errorstr(ERR_ATTRIBUTE_NAMESPACE_NOT_ATTRIBUTE, occNamespace.c_str());
                                }
                            }
                            else if (stripped_ver == clangNamespace)
                            {
                                *lex = getsym();
                                if (MATCHKW(*lex, Keyword::classsel_))
                                {
                                    *lex = getsym();
                                    if (!ISID(*lex))
                                    {
                                        *lex = getsym();
                                        error(ERR_IDENTIFIER_EXPECTED);
                                    }
                                    else if (*lex)
                                    {
                                        std::string name = (*lex)->data->value.s.a;
                                        name = StripUnderscores(name);
                                        auto searchedName = clangCPPStyleAttribNames.find(name);
                                        if (searchedName != clangCPPStyleAttribNames.end())
                                        {
                                            switch (searchedName->second)
                                            {
                                                case 28:
                                                    basisAttribs.inheritable.linkage2 = Linkage::internal_;
                                                    break;
                                                case 29:
                                                    basisAttribs.inheritable.excludeFromExplicitInstantiation = true;
                                                    break;
                                            }
                                        }
                                        else
                                        {
                                            errorstr2(ERR_ATTRIBUTE_DOES_NOT_EXIST_IN_NAMESPACE, name.c_str(),
                                                      clangNamespace.c_str());
                                        }
                                        *lex = getsym();
                                    }
                                }
                                else
                                {
                                    errorstr(ERR_ATTRIBUTE_NAMESPACE_NOT_ATTRIBUTE, clangNamespace.c_str());
                                }
                            }
                            else if (stripped_ver == gccNamespace)
                            {
                                *lex = getsym();
                                if (MATCHKW(*lex, Keyword::classsel_))
                                {
                                    *lex = getsym();
                                    if (!ISID(*lex))
                                    {
                                        *lex = getsym();
                                        error(ERR_IDENTIFIER_EXPECTED);
                                    }
                                    else if (*lex)
                                    {
                                        // note: these are only the namespaced names listed, the ___attribute__ names are
                                        // unlisted here as they don't exist in GCC and we want ours to follow theirs for actual
                                        // consistency reasons.
                                        std::string name = (*lex)->data->value.s.a;
                                        name = StripUnderscores(name);
                                        auto searchedName = gccCPPStyleAttribNames.find(name);
                                        if (searchedName != gccCPPStyleAttribNames.end())
                                        {
                                            switch (searchedName->second)
                                            {
                                                case 25:
                                                    if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                    basisAttribs.inheritable.linkage2 = Linkage::export_;
                                                    break;
                                                case 26:
                                                    if (basisAttribs.inheritable.linkage2 != Linkage::none_)
                                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                    basisAttribs.inheritable.linkage2 = Linkage::import_;
                                                    break;
                                                case 27:
                                                    if (basisAttribs.inheritable.linkage != Linkage::none_)
                                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                    basisAttribs.inheritable.linkage = Linkage::stdcall_;
                                            }
                                        }
                                        else
                                        {
                                            errorstr2(ERR_ATTRIBUTE_DOES_NOT_EXIST_IN_NAMESPACE, name.c_str(),
                                                      gccNamespace.c_str());
                                        }
                                        *lex = getsym();
                                    }
                                }
                                else
                                {
                                    errorstr(ERR_ATTRIBUTE_NAMESPACE_NOT_ATTRIBUTE, gccNamespace.c_str());
                                }
                            }
                            else
                            {

                                if (stripped_ver == "noreturn")
                                {
                                    *lex = getsym();
                                    special = true;
                                    if (basisAttribs.inheritable.linkage3 != Linkage::none_)
                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                    basisAttribs.inheritable.linkage3 = Linkage::noreturn_;
                                }
                                else if (stripped_ver == "carries_dependency")
                                {
                                    *lex = getsym();
                                    special = true;
                                }
                                else if (stripped_ver == "fallthrough")
                                {
                                    *lex = getsym();
                                    basisAttribs.uninheritable.fallthrough = true;
                                }
                                else if (stripped_ver == "maybe_unused")
                                {
                                    *lex = getsym();
                                    basisAttribs.uninheritable.maybe_unused = true;
                                }
                                else if (stripped_ver == "nodiscard")
                                {
                                    *lex = getsym();
                                    basisAttribs.uninheritable.nodiscard = true;
                                }
                                else
                                {
                                    if (stripped_ver == "deprecated")
                                        basisAttribs.uninheritable.deprecationText = (char*)-1;
                                    *lex = getsym();
                                    if (MATCHKW(*lex, Keyword::classsel_))
                                    {
                                        *lex = getsym();
                                        if (!ISID(*lex))
                                            error(ERR_IDENTIFIER_EXPECTED);
                                        *lex = getsym();
                                    }
                                }
                            }
                        }
                        if (MATCHKW(*lex, Keyword::openpa_))
                        {
                            if (special)
                                error(ERR_NO_ATTRIBUTE_ARGUMENT_CLAUSE_HERE);
                            *lex = getsym();
                            if (basisAttribs.uninheritable.deprecationText)
                            {
                                if ((*lex)->data->type == LexType::l_astr_)
                                {
                                    char buf[1024];
                                    int i;
                                    Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)(*lex)->data->value.s.w;
                                    for (i = 0; i < 1024 && i < xx->count; i++)
                                        buf[i] = (char)xx->str[i];
                                    buf[i] = 0;
                                    basisAttribs.uninheritable.deprecationText = litlate(buf);
                                    *lex = getsym();
                                }
                            }

                            else
                                while (*lex && !MATCHKW(*lex, Keyword::closepa_))
                                {
                                    balancedAttributeParameter(lex);
                                }
                            needkw(lex, Keyword::closepa_);
                        }
                        if (MATCHKW(*lex, Keyword::ellipse_))
                            *lex = getsym();
                        if (!MATCHKW(*lex, Keyword::comma_))
                            break;
                        *lex = getsym();
                    }
                    if (needkw(lex, Keyword::closebr_))
                        needkw(lex, Keyword::closebr_);
                }
                else
                {
                    // empty
                    *lex = getsym();
                    needkw(lex, Keyword::closebr_);
                }
            }
            else
            {
                *lex = backupsym();
                break;
            }
        }
    }
    return rv;
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
LexList* getDeclType(LexList* lex, SYMBOL* funcsp, Type** tn)
{
    bool hasAmpersand = false;
    bool hasAuto = false;
    EXPRESSION *exp, *exp2;
    lex = getsym();
    needkw(&lex, Keyword::openpa_);
    bool extended = MATCHKW(lex, Keyword::openpa_);
    hasAmpersand = MATCHKW(lex, Keyword::and_);
    if (extended || hasAmpersand)
    {
        lex = getsym();
        hasAuto = MATCHKW(lex, Keyword::auto_);
        lex = backupsym();
    }
    else
    {
        hasAuto = MATCHKW(lex, Keyword::auto_);
    }
    if (hasAuto)
    {
        if (extended || hasAmpersand)
            lex = getsym();
        lex = getsym();
        if (MATCHKW(lex, Keyword::and_) || MATCHKW(lex, Keyword::land_))
        {
            lex = getsym();
            error(ERR_DECLTYPE_AUTO_NO_REFERENCE);
        }
        if (extended)
            needkw(&lex, Keyword::closepa_);
        (*tn) = Type::MakeType(BasicType::auto_);
        (*tn)->size = 0;
        (*tn)->decltypeauto = true;
    }
    else
    {
        auto oldnoExcept = noExcept;
        lex = expression_no_check(lex, nullptr, nullptr, &(*tn), &exp, _F_SIZEOF | _F_INDECLTYPE);
        noExcept = oldnoExcept;
        if (exp->type == ExpressionNode::callsite_ && exp->v.func->sp->sb->deleted)
        {
            *tn = &stdany;
            return lex;
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
            if (definingTemplate && !instantiatingTemplate || (*tn)->type == BasicType::any_)
            {
                (*tn) = Type::MakeType(BasicType::templatedecltype_);
                (*tn)->templateDeclType = exp;
            }
        }
        exp2 = exp;
        if (!(*tn))
        {
            error(ERR_IDENTIFIER_EXPECTED);
            errskim(&lex, skim_semi_declare);
            return lex;
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
    needkw(&lex, Keyword::closepa_);
    if (*tn && (*tn)->IsAutoType())
        RequiresDialect::Feature(Dialect::cpp14, "decltype(auto)");
    return lex;
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
    if (!definingTemplate || instantiatingTemplate)
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

LexList* GetStructuredBinding(LexList* lex, SYMBOL* funcsp, StorageClass storage_class, Linkage linkage,
                              std::list<FunctionBlock*>& block)
{
    lex = getsym();
    if (!ISID(lex))
    {
        errskim(&lex, skim_semi_declare);
        error(ERR_IDENTIFIER_EXPECTED);
    }
    else
    {
        std::deque<std::string> identifiers;
        do
        {
            identifiers.push_back(lex->data->value.s.a);
            lex = getsym();
            if (ISID(lex))
                break;
            if (MATCHKW(lex, Keyword::comma_))
                lex = getsym();
        } while (lex && ISID(lex));
        needkw(&lex, Keyword::closebr_);
        if (!needkw(&lex, Keyword::assign_))
        {
            errskim(&lex, skim_semi_declare);
        }
        else
        {
            EXPRESSION* exp = nullptr;
            Type* tp = nullptr;
            lex = expression_no_comma(lex, funcsp, nullptr, &tp, &exp, nullptr, 0);
            currentLineData(block, lex, -1);
            if (tp == nullptr)
            {
                error(ERR_EXPRESSION_SYNTAX);
                errskim(&lex, skim_semi_declare);
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
                                auto st = Statement::MakeStatement(lex, block, StatementNode::expr_);
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
                            auto st = Statement::MakeStatement(lex, block, StatementNode::expr_);
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
                            Statement* s = Statement::MakeStatement(lex, block, StatementNode::varstart_);
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
                        auto st = Statement::MakeStatement(lex, block, StatementNode::expr_);
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
                            Statement* s = Statement::MakeStatement(lex, block, StatementNode::varstart_);
                            s->select = MakeExpression(ExpressionNode::auto_, sym);
                        }
                        ++it;
                    }
                }
            }
        }
    }
    return lex;
}

}  // namespace Parser
