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
#include <stack>

#include "compiler.h"
#include "config.h"
#include "ccerr.h"
#include "lex.h"
#include "ildata.h"
#include "SymbolProperties.h"
#include "expr.h"
#include "ListFactory.h"
#include "inline.h"
#include  "overload.h"
#include "templatedecl.h"
#include "vtab.h"
#include "declcons.h"
#include "memory.h"
#include "declare.h"
#include "OptUtils.h"
#include "initbackend.h"
#include "rtti.h"
#include "templateutil.h"
#include "declcpp.h"
#include "beinterf.h"
#include "types.h"
#include "namespace.h"
#include "symtab.h"
#include "stmt.h"

namespace Parser
{
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
                                if ((!func->sb->templateLevel || func->sb->instantiated) && bodyTokenStreams.get(func))
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
                                if (func->sb->defaulted && !func->sb->inlineFunc.stmt && func->sb->isDestructor)
                                    createDestructor(func->sb->parentClass);
                                StatementGenerator sg(func);
                                sg.CompileFunctionFromStream();
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
            Type* tp1 = oldFunc->tp->BaseType()->btp, * tp2 = newFunc->tp->BaseType()->btp;
            if (!tp1->CompatibleType(tp2) && !SameTemplate(tp1, tp2) &&
                (!tp1->IsStructured() || !tp2->IsStructured() ||
                    (tp1->BaseType()->sp->sb->templateLevel &&
                        tp2->BaseType()->sp->sb->parentTemplate != tp1->BaseType()->sp->sb->parentTemplate)))
            {
                if (!templateDefinitionLevel)
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
    void calculateVTabEntries(SYMBOL* sym, SYMBOL* base, std::list<VTABENTRY*>** pos, int offset)
    {
        if (IsDefiningTemplate())
            return;
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
                        VTABENTRY* hold, * hold2;
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
                        if (cur1->sb->isoverride && !found && !isfirst && (!IsDefiningTemplate()))
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
}  // namespace Parser